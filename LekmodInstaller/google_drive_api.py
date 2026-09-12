

import requests
import os
from pathlib import Path

class GoogleDriveDownloader:
    def __init__(self, config):
        self.config = config
        self.base_url = "https://drive.google.com/uc?export=download"

    def download_version_with_info(self, version, version_info, log_callback, progress_callback=None, filename_prefix="LEKMOD", download_dir=None):

        if not version_info:
            raise Exception(f"No configuration found for version {version}")

        file_id = version_info.get('file_id')
        if not file_id:
            raise Exception(f"No Google Drive file ID found for version {version}")

        return self._download_file(file_id, version, log_callback, progress_callback, filename_prefix=filename_prefix, download_dir=download_dir)

    def download_version(self, version, log_callback):


        versions_data = self.config.get('versions', {})
        version_info = versions_data.get(version)

        if not version_info:
            raise Exception(f"No configuration found for version {version}")

        file_id = version_info.get('file_id')
        if not file_id:
            raise Exception(f"No Google Drive file ID found for version {version}")

        return self._download_file(file_id, version, log_callback, filename_prefix="LEKMOD")

    def _download_file(self, file_id, version, log_callback, progress_callback=None, filename_prefix="LEKMOD", download_dir=None):

        log_callback(f"Connecting to Google Drive...")
        log_callback(f"File ID: {file_id}")


        session = requests.Session()


        response = session.get(self.base_url, params={'id': file_id}, stream=True)



        token = None
        for key, value in response.cookies.items():
            if key.startswith('download_warning'):
                token = value
                break


        if not token:
            for key, value in response.cookies.items():
                if 'confirm' in key.lower():
                    token = value
                    break

        if token:
            log_callback(f"Large file detected, confirming download...")
            params = {'id': file_id, 'confirm': token}
            response = session.get(self.base_url, params=params, stream=True)


        if not token and 'text/html' in response.headers.get('content-type', ''):
            log_callback(f"Large file detected (>100MB), bypassing virus scan warning...")


            confirmation_attempts = ['t', '1', 'true', 'yes']

            for confirm_value in confirmation_attempts:
                params = {'id': file_id, 'confirm': confirm_value, 'export': 'download'}
                test_response = session.get(self.base_url, params=params, stream=True)


                if 'text/html' not in test_response.headers.get('content-type', ''):
                    log_callback(f"✓ Bypassed virus scan warning")
                    response = test_response
                    break


            if 'text/html' in response.headers.get('content-type', ''):
                log_callback(f"Trying direct download URL...")
                direct_url = f"https://drive.usercontent.google.com/download?id={file_id}&export=download&confirm=t"
                response = session.get(direct_url, stream=True)


        content_type = response.headers.get('content-type', '')
        if 'text/html' in content_type and response.status_code == 200:

            peek = response.content[:500].decode('utf-8', errors='ignore')
            if 'quota' in peek.lower():
                raise Exception(
                    "Google Drive download quota exceeded.\n"
                    "Please try again later."
                )
            else:
                raise Exception(
                    f"Failed to download from Google Drive.\n"
                    f"Response type: {content_type}\n"
                    f"Status code: {response.status_code}\n\n"
                    f"The file might be:\n"
                    f"- Not shared publicly (verify in incognito mode)\n"
                    f"- The file ID is incorrect\n"
                    f"- Google Drive link restrictions\n"
                    f"- Download quota exceeded"
                )


        total_size = int(response.headers.get('content-length', 0))


        download_dir = Path(download_dir) if download_dir is not None else Path.cwd() / "downloads"
        download_dir.mkdir(exist_ok=True)


        safe_version = "".join(ch if ch.isalnum() or ch in "._-" else "_" for ch in str(version))
        download_path = download_dir / f"{filename_prefix}_{safe_version}.zip"


        log_callback(f"Downloading to {download_path}...")

        if total_size > 0:
            log_callback(f"File size: {self._format_size(total_size)}")

        downloaded = 0
        chunk_size = 32768
        last_update = 0
        update_interval = 512 * 1024

        with open(download_path, 'wb') as f:
            for chunk in response.iter_content(chunk_size=chunk_size):
                if chunk:
                    f.write(chunk)
                    downloaded += len(chunk)


                    if (downloaded - last_update) >= update_interval or downloaded == total_size:
                        if total_size > 0:
                            progress = (downloaded / total_size) * 100


                            if progress_callback:
                                progress_callback(progress,
                                    f"Downloading... {self._format_size(downloaded)} / "
                                    f"{self._format_size(total_size)}")

                            log_callback(
                                f"Downloading... {self._format_size(downloaded)} / "
                                f"{self._format_size(total_size)} ({progress:.1f}%)"
                            )
                        else:
                            log_callback(f"Downloading... {self._format_size(downloaded)}")
                        last_update = downloaded

        log_callback(f"✓ Downloaded successfully!")
        return str(download_path)

    def _format_size(self, bytes_size):

        for unit in ['B', 'KB', 'MB', 'GB']:
            if bytes_size < 1024.0:
                return f"{bytes_size:.1f} {unit}"
            bytes_size /= 1024.0
        return f"{bytes_size:.1f} TB"

    def get_file_info(self, file_id):



        return {
            'name': 'Lekmod.zip',
            'size': 'Unknown',
            'modified': 'Unknown'
        }

