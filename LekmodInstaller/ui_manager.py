

import os
import re
import shutil
import tempfile
import zipfile
from pathlib import Path
from ui_assets import configure_ui, stamp_ui

try:
    import winreg
except ImportError:
    winreg = None

class UIManager:
    def __init__(self):
        self.civ5_path = None

    def find_civ5_installation(self):


        common_paths = [
            r"C:\Program Files (x86)\Steam\steamapps\common\Sid Meier's Civilization V",
            r"C:\Program Files\Steam\steamapps\common\Sid Meier's Civilization V",
            r"D:\Steam\steamapps\common\Sid Meier's Civilization V",
            r"E:\Steam\steamapps\common\Sid Meier's Civilization V",
        ]

        for path in common_paths:
            if os.path.exists(path):
                self.civ5_path = path
                return path


        if winreg:
            try:
                key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE,
                                    r"SOFTWARE\WOW6432Node\Valve\Steam")
                steam_path, _ = winreg.QueryValueEx(key, "InstallPath")
                winreg.CloseKey(key)

                civ5_path = os.path.join(steam_path,
                                        "steamapps", "common",
                                        "Sid Meier's Civilization V")
                if os.path.exists(civ5_path):
                    self.civ5_path = civ5_path
                    return civ5_path
            except:
                pass


        try:
            docs_path = Path.home() / "Documents" / "My Games" / "Sid Meier's Civilization 5"
            if docs_path.exists():

                pass
        except:
            pass

        return None

    def check_eui_installed(self, civ5_path):

        if not civ5_path:
            return False


        dlc_path = os.path.join(civ5_path, "Assets", "DLC")

        if not os.path.exists(dlc_path):
            return False


        eui_folders = ["UI_bc1", "UI_bc1_xits"]

        for folder in eui_folders:
            folder_path = os.path.join(dlc_path, folder)
            if os.path.exists(folder_path):
                return True

        return False

    def get_current_lekmod_version(self, civ5_path):

        if not civ5_path:
            return None

        dlc_path = os.path.join(civ5_path, "Assets", "DLC")

        if not os.path.exists(dlc_path):
            return None


        lekmod_folders = []
        try:
            for item in os.listdir(dlc_path):
                if item.startswith("LEKMOD_"):
                    lekmod_folders.append(item)
        except:
            return None


        if not lekmod_folders:
            return None


        latest_folder = sorted(lekmod_folders)[-1]
        version_from_folder = latest_folder.replace("LEKMOD_", "")

        lekmod_path = os.path.join(dlc_path, latest_folder)


        version_files = [
            os.path.join(lekmod_path, "version.txt"),
            os.path.join(lekmod_path, "VERSION"),
            os.path.join(lekmod_path, "Lekmod (v 1).modinfo"),
            os.path.join(lekmod_path, "LEKMOD.modinfo"),
        ]

        for version_file in version_files:
            if os.path.exists(version_file):
                try:
                    with open(version_file, 'r', encoding='utf-8') as f:
                        content = f.read()

                        import re
                        match = re.search(r'v?\d+\.\d+', content)
                        if match:
                            return match.group(0)
                except:
                    pass


        if version_from_folder:
            return version_from_folder

        return None

    def extract_mod(self, zip_path, log_callback, extract_name="lekmod_temp"):

        extract_dir = os.path.join(os.path.dirname(zip_path), extract_name)

        if os.path.exists(extract_dir):
            shutil.rmtree(extract_dir)
        os.makedirs(extract_dir)

        with zipfile.ZipFile(zip_path, 'r') as zip_ref:

            total_files = len(zip_ref.namelist())
            extracted = 0

            for file in zip_ref.namelist():
                zip_ref.extract(file, extract_dir)
                extracted += 1
                if extracted % 100 == 0 or extracted == total_files:
                    log_callback(f"Extracting... {extracted}/{total_files} files")

        return extract_dir

    def _find_lekmod_folder(self, root_path):

        if os.path.isdir(os.path.join(root_path, "Lua", "tmp")):
            return root_path

        exact = None
        versioned = []
        for dirpath, dirnames, _files in os.walk(root_path):
            dirnames[:] = [d for d in dirnames if d.upper() != "LEKMOD_DLL"]
            for dir_name in list(dirnames):
                upper = dir_name.upper()
                if not upper.startswith("LEKMOD"):
                    continue
                full = os.path.join(dirpath, dir_name)
                if not os.path.isdir(os.path.join(full, "Lua", "tmp")):
                    continue
                if upper == "LEKMOD":
                    exact = full
                else:
                    versioned.append(full)
            if exact:
                break

        if exact:
            return exact
        if versioned:
            return versioned[0]
        return None

    def _find_eui_folder(self, civ5_path):

        if not civ5_path:
            return None
        dlc_path = os.path.join(civ5_path, "Assets", "DLC")
        for folder in ("UI_bc1", "UI_bc1_xits"):
            path = os.path.join(dlc_path, folder)
            if os.path.isdir(path):
                return path
        return None

    def configure_ui_files(self, mod_path, ui_type, log_callback, civ5_path=None):

        lekmod_path = self._find_lekmod_folder(mod_path)
        if not lekmod_path:
            raise RuntimeError("LEKMOD folder not found in extracted files")
        want_eui = "Enhanced UI" in ui_type
        eui_folder = self._find_eui_folder(civ5_path) if want_eui else None
        configure_ui(lekmod_path, want_eui, eui_folder, log=log_callback, strict=False)

    def find_existing_lekmod_folders(self, civ5_path):

        dlc_path = os.path.join(civ5_path, "Assets", "DLC")

        if not os.path.exists(dlc_path):
            return []

        existing_folders = []
        try:
            for item in os.listdir(dlc_path):
                item_path = os.path.join(dlc_path, item)
                if os.path.isdir(item_path) and item.startswith("LEKMOD"):
                    existing_folders.append(item)
        except:
            pass

        return existing_folders

    def remove_lekmod_folders(self, civ5_path, folders, log_callback):

        dlc_path = os.path.join(civ5_path, "Assets", "DLC")

        for folder in folders:
            folder_path = os.path.join(dlc_path, folder)
            if os.path.exists(folder_path):
                log_callback(f"Removing {folder}...")
                shutil.rmtree(folder_path)
                log_callback(f"✓ Removed {folder}")

    def install_mod(self, mod_source, civ5_path, version, log_callback):

        if not isinstance(version, str) or not re.fullmatch(r"v?[0-9]+(?:\.[0-9]+)*", version):
            raise ValueError("Invalid Lekmod version label")

        dlc_path = os.path.join(civ5_path, "Assets", "DLC")


        if not os.path.exists(dlc_path):
            os.makedirs(dlc_path)
            log_callback(f"✓ Created DLC folder: {dlc_path}")


        folder_name = f"LEKMOD_{version}"
        dlc_dest = os.path.join(dlc_path, folder_name)
        if Path(dlc_dest).resolve().parent != Path(dlc_path).resolve():
            raise ValueError("Lekmod destination must stay inside the DLC directory")

        lekmod_source = self._find_lekmod_folder(mod_source)
        if lekmod_source:
            log_callback(f"Found mod folder: {os.path.basename(lekmod_source)}")

        if not lekmod_source or not os.path.exists(lekmod_source):
            raise Exception("LEKMOD source folder not found in downloaded archive!")

        staging = Path(tempfile.mkdtemp(prefix=".lekmod-install-", dir=Path(dlc_path).resolve().parent))
        backups = []
        installed = False
        try:
            replacement = staging / "replacement"
            log_callback(f"Preparing files for {dlc_dest}...")
            shutil.copytree(lekmod_source, replacement)
            stamp_ui(replacement)
            ui_check = replacement / "ui_check.bat"
            if ui_check.exists():
                ui_check.unlink()


            for name in self.find_existing_lekmod_folders(civ5_path):
                original, backup = Path(dlc_path) / name, staging / name
                original.rename(backup)
                backups.append((original, backup))
            replacement.rename(dlc_dest)
            installed = True
        except Exception as error:
            restore_failed = False
            for original, backup in reversed(backups):
                try:
                    backup.rename(original)
                except OSError:
                    restore_failed = True
            if restore_failed:
                raise RuntimeError(f"Installation failed. Previous files are saved at {staging}") from error
            raise
        finally:
            if installed or not any(backup.exists() for _, backup in backups):
                shutil.rmtree(staging, ignore_errors=True)
        log_callback("✓ Files installed successfully!")


    _LEKMAP_V6_NAMES = {
        "lekmappangaeafractalv6.lua",
        "lekmappangaeafractalv6.0.lua",
        "lekmap_bonus.lua",
        "lekmap_citystates.lua",
        "lekmap_constants.lua",
        "lekmap_featuregenerator.lua",
        "lekmap_fjords.lua",
        "lekmap_fractalworld.lua",
        "lekmap_hexutil.lua",
        "lekmap_impact.lua",
        "lekmap_islands.lua",
        "lekmap_landmass.lua",
        "lekmap_luxuries.lua",
        "lekmap_mapgenerator.lua",
        "lekmap_naturalwonders.lua",
        "lekmap_options.lua",
        "lekmap_regions.lua",
        "lekmap_resourcedefs.lua",
        "lekmap_resources.lua",
        "lekmap_spawns.lua",
        "lekmap_strategics.lua",
        "lekmap_tectonicislands.lua",
        "lekmap_terraingenerator.lua",
        "lekmap_utilities.lua",
    }
    _LEKMAP_V6_MAIN_NAMES = {
        "lekmappangaeafractalv6.lua",
        "lekmappangaeafractalv6.0.lua",
    }
    _LEKMAP_PANGAEA_VERSION_RE = re.compile(
        r"^lekmappangaeafractalv(\d+(?:\.\d+)*)\.lua$",
        re.IGNORECASE,
    )
    _LEKMAP_FOLDER_VERSION_RE = re.compile(
        r"^lekmap[\s_]+v?(\d+(?:\.\d+)*)$",
        re.IGNORECASE,
    )
    _MAPS_SKIP_DIRS = {"__macosx", ".git", ".svn", "lekmod_dll"}
    _LEKMAP_STAMP_NAMES = ("Lekmap VERSION.txt", "Lekmap_VERSION.txt")

    def _normalize_lekmap_version_label(self, raw):

        if not raw:
            return None
        text = " ".join(str(raw).strip().replace("_", " ").split())
        if text.lower() == "installed":
            return "installed"
        if text.lower().startswith("lekmap"):
            text = text[6:].strip()
        if not text:
            return "installed"
        if not text.lower().startswith("v"):
            text = "v" + text
        if text.lower() == "v6":
            text = "v6.0"
        return f"Lekmap {text}"

    def find_civ5_maps_folder(self, civ5_path=None):

        civ5_path = civ5_path or self.civ5_path
        if not civ5_path:
            return None
        return os.path.join(civ5_path, "Assets", "Maps")

    def _iter_maps_files(self, maps_dir, max_depth=3):

        maps_dir = os.path.abspath(maps_dir)
        for root, dirs, files in os.walk(maps_dir):
            rel = os.path.relpath(root, maps_dir)
            depth = 0 if rel == "." else rel.count(os.sep) + 1
            dirs[:] = [
                d for d in dirs
                if d.lower() not in self._MAPS_SKIP_DIRS and not d.startswith(".")
            ]
            if depth >= max_depth:
                dirs[:] = []
            for name in files:
                yield root, name

    def _collect_lekmap_lua_names(self, maps_dir):
        names = set()
        try:
            for _root, name in self._iter_maps_files(maps_dir):
                if name.lower().endswith(".lua") and name.lower().startswith("lekmap"):
                    names.add(name.lower())
        except Exception:
            pass
        return names

    def _infer_lekmap_version_from_names(self, lua_names):

        if not lua_names:
            return None
        if lua_names & self._LEKMAP_V6_MAIN_NAMES:
            return self._normalize_lekmap_version_label("v6.0")
        if len(lua_names & self._LEKMAP_V6_NAMES) >= 3:
            return self._normalize_lekmap_version_label("v6.0")
        detected = []
        for name in lua_names:
            match = self._LEKMAP_PANGAEA_VERSION_RE.match(name)
            if match:
                detected.append(match.group(1))
        if detected:
            detected.sort(key=lambda part: [int(x) for x in part.split(".")], reverse=True)
            return self._normalize_lekmap_version_label(detected[0])
        return "installed"

    def _infer_lekmap_version_from_folders(self, maps_dir):

        try:
            for name in os.listdir(maps_dir):
                full = os.path.join(maps_dir, name)
                if not os.path.isdir(full):
                    continue
                match = self._LEKMAP_FOLDER_VERSION_RE.match(name.strip())
                if match:
                    return self._normalize_lekmap_version_label(match.group(1))
        except Exception:
            pass
        return None

    def _find_lekmap_folder(self, root_path):

        skip = self._MAPS_SKIP_DIRS

        def is_lekmap_content(path):
            try:
                names = os.listdir(path)
            except Exception:
                return False
            for name in names:
                lower = name.lower()
                if lower.endswith(".lua") and (lower.startswith("lekmap") or lower.startswith("hb")):
                    return True
            return False

        named = []
        for dirpath, dirnames, _files in os.walk(root_path):
            dirnames[:] = [
                d for d in dirnames
                if d.lower() not in skip and not d.startswith(".")
            ]
            for dir_name in list(dirnames):
                if not dir_name.lower().startswith("lekmap"):
                    continue
                full = os.path.join(dirpath, dir_name)
                if is_lekmap_content(full) or self._LEKMAP_FOLDER_VERSION_RE.match(dir_name.strip()):
                    named.append(full)
            if named:
                break

        if named:
            named.sort(key=lambda path: (
                0 if self._LEKMAP_FOLDER_VERSION_RE.match(os.path.basename(path).strip()) else 1,
                path,
            ))
            return named[0]

        if is_lekmap_content(root_path):
            return root_path

        try:
            children = [
                name for name in os.listdir(root_path)
                if os.path.isdir(os.path.join(root_path, name))
                and name.lower() not in skip
                and not name.startswith(".")
            ]
            if len(children) == 1:
                candidate = os.path.join(root_path, children[0])
                if is_lekmap_content(candidate):
                    return candidate
        except Exception:
            pass
        return None

    def _lekmap_dest_folder(self, maps_dir, version):
        folder_name = self._normalize_lekmap_version_label(version) or "Lekmap"
        return os.path.join(maps_dir, folder_name), folder_name

    def _read_lekmap_stamp(self, maps_dir):
        candidates = [maps_dir]
        try:
            for name in os.listdir(maps_dir):
                full = os.path.join(maps_dir, name)
                if os.path.isdir(full) and name.lower().startswith("lekmap"):
                    candidates.append(full)
        except Exception:
            pass

        for folder in candidates:
            for stamp_name in self._LEKMAP_STAMP_NAMES:
                version_file = os.path.join(folder, stamp_name)
                if not os.path.isfile(version_file):
                    continue
                try:
                    with open(version_file, "r", encoding="utf-8") as handle:
                        line = handle.readline().strip()
                        if line:
                            return self._normalize_lekmap_version_label(line)
                except Exception:
                    pass
        return None

    def get_current_lekmap_version(self, civ5_path=None):

        maps_dir = self.find_civ5_maps_folder(civ5_path)
        if not maps_dir or not os.path.isdir(maps_dir):
            return None

        stamped = self._read_lekmap_stamp(maps_dir)
        if stamped:
            return stamped

        from_folder = self._infer_lekmap_version_from_folders(maps_dir)
        if from_folder:
            return from_folder

        lua_names = self._collect_lekmap_lua_names(maps_dir)
        return self._infer_lekmap_version_from_names(lua_names)

    def install_lekmap(self, extract_path, version, log_callback, civ5_path=None):

        maps_dir = self.find_civ5_maps_folder(civ5_path)
        if not maps_dir:
            raise Exception("Civilization V path is not set; cannot find Assets/Maps.")
        os.makedirs(maps_dir, exist_ok=True)

        source = self._find_lekmap_folder(extract_path)
        if not source or not os.path.isdir(source):
            raise Exception("Lekmap folder not found in downloaded archive!")

        dest, folder_name = self._lekmap_dest_folder(maps_dir, version)
        log_callback(f"Found map folder: {os.path.basename(source)}")
        log_callback(f"Copying folder to {dest}...")

        if os.path.isdir(dest):
            shutil.rmtree(dest)
        elif os.path.exists(dest):
            os.remove(dest)

        skip_names = set(self._MAPS_SKIP_DIRS)
        skip_names.update(("lekmap version.txt", "lekmap_version.txt"))

        def ignore(_directory, contents):
            ignored = []
            for name in contents:
                lower = name.lower()
                if lower in skip_names or name.startswith("."):
                    ignored.append(name)
            return ignored

        shutil.copytree(source, dest, ignore=ignore)

        version_label = self._normalize_lekmap_version_label(version) or str(version).strip()
        version_path = os.path.join(dest, "Lekmap VERSION.txt")
        with open(version_path, "w", encoding="utf-8") as handle:
            handle.write(version_label + "\n")


        try:
            inner_files = {
                name.lower()
                for name in os.listdir(dest)
                if os.path.isfile(os.path.join(dest, name))
            }
            for name in os.listdir(maps_dir):
                full = os.path.join(maps_dir, name)
                if not os.path.isfile(full):
                    continue
                lower = name.lower()
                if lower in inner_files or lower in skip_names:
                    os.remove(full)
                    log_callback(f"✓ Removed leftover {name} from Maps root")
        except Exception as err:
            log_callback(f"⚠ Could not clean leftover map files: {err}")

        log_callback(f"✓ Installed folder {folder_name}")
        return dest

