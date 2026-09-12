"""
Installer Self-Updater - Allows the installer to update itself
"""
import requests
import os
import sys
import subprocess
import shutil
from pathlib import Path
import tempfile

class InstallerUpdater:
    def __init__(self, config):
        self.config = config
        self.current_version = config.get('installer_version', '1.0.0')
        self.update_url = config.get('installer_update_url')
        
    def check_for_installer_update(self):
        """Check if there's a newer version of the installer available"""
        if not self.update_url:
            return None
            
        try:
            response = requests.get(self.update_url, timeout=10)
            if response.status_code == 200:
                update_info = response.json()
                latest_version = update_info.get('version')
                
                if latest_version and self._is_newer_version(latest_version, self.current_version):
                    return update_info
                    
        except Exception as e:
            print(f"Failed to check for installer updates: {e}")
            
        return None
    
    def _is_newer_version(self, new_version, current_version):
        """Compare version strings (e.g., 1.2.0 vs 1.1.0)"""
        def parse_version(v):
            return tuple(int(x) for x in v.split('.') if x.isdigit())
        
        try:
            new_parts = parse_version(new_version)
            current_parts = parse_version(current_version)
            return new_parts > current_parts
        except:
            return False
    
    def download_and_update(self, update_info, log_callback):
        """Download new installer and replace current one"""
        download_url = update_info.get('download_url')
        if not isinstance(download_url, str) or not download_url.startswith('https://'):
            raise ValueError("Installer downloads require an HTTPS URL")
        if not getattr(sys, 'frozen', False):
            raise RuntimeError("Self-update requires the packaged Windows installer")
        
        log_callback("Downloading new installer version...")
        
        # Keep the download and backup on the installer's volume for renames.
        temp_dir = tempfile.mkdtemp(prefix="LekmodInstaller-update-", dir=Path(sys.executable).parent)
        temp_installer = os.path.join(temp_dir, "LekmodInstaller_new.exe")
        try:
            with requests.get(download_url, stream=True, timeout=(10, 60)) as response:
                response.raise_for_status()
                if not response.url.startswith('https://'):
                    raise ValueError("Installer download redirected away from HTTPS")
                total_size = int(response.headers.get('content-length', 0))
                downloaded = 0
                with open(temp_installer, 'wb') as f:
                    for chunk in response.iter_content(chunk_size=8192):
                        if chunk:
                            f.write(chunk)
                            downloaded += len(chunk)
                            if total_size > 0:
                                log_callback(f"Downloading... {downloaded / total_size * 100:.1f}%")
                if total_size and downloaded != total_size:
                    raise ValueError("Incomplete installer download")

            # Reject HTML/error pages and files missing DOS/PE signatures.
            with open(temp_installer, 'rb') as f:
                header = f.read(64)
                if len(header) != 64 or header[:2] != b'MZ':
                    raise ValueError("Download is not a Windows executable")
                f.seek(int.from_bytes(header[60:64], 'little'))
                if f.read(4) != b'PE\0\0':
                    raise ValueError("Download is not a Windows executable")
            update_script = self._create_update_script(sys.executable, temp_installer)
        except Exception:
            shutil.rmtree(temp_dir, ignore_errors=True)
            raise
        log_callback("✓ Download complete!")
        
        log_callback("Preparing to update installer...")
        log_callback("The installer will restart after update.")
        
        return update_script, temp_installer
    
    def _create_update_script(self, current_exe, new_exe):
        """Create a batch script to replace the installer and restart it"""
        script_path = os.path.join(os.path.dirname(new_exe), "update_installer.bat")
        backup = os.path.join(os.path.dirname(new_exe), "previous.exe")
        # Percent signs expand even inside cmd.exe quotes; disable ! expansion too.
        current_exe, new_exe, backup = (str(path).replace('%', '%%')
                                      for path in (current_exe, new_exe, backup))
        
        script_content = f"""@echo off
setlocal DisableDelayedExpansion
echo Updating Lekmod Installer...
timeout /t 2 /nobreak >nul
set attempts=0

:retry
move /y "{current_exe}" "{backup}" >nul 2>&1
if not errorlevel 1 goto install
set /a attempts+=1
if %attempts% geq 30 goto failed
timeout /t 1 /nobreak >nul
goto retry

:install
move /y "{new_exe}" "{current_exe}"
if errorlevel 1 (
    move /y "{backup}" "{current_exe}"
    goto failed
)

echo Update complete! Restarting installer...
start "" "{current_exe}"
del "%~f0"
exit /b 0

:failed
echo Update failed. Previous installer retained at "{current_exe}" or "{backup}".
exit /b 1
"""
        
        with open(script_path, 'w') as f:
            f.write(script_content)
        
        return script_path
    
    def apply_update(self, update_script):
        """Execute update script and exit current installer"""
        subprocess.Popen(f'"{update_script}"', shell=True)
        sys.exit(0)

