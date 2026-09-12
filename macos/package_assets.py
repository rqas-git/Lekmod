
from pathlib import Path
import sys
import shutil

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "LekmodInstaller"))
from ui_assets import configure_ui


def prepare_lekmod(source, destination, eui=None):
    shutil.copytree(source, destination,
                    ignore=lambda _path, names: [n for n in names
                        if n.startswith('.') or Path(n).suffix.lower() in ('.dll', '.pdb', '.bat')])
    configure_ui(destination, want_eui=eui is not None, eui_folder=eui, preserve_all=False)
    if eui is not None and (eui / 'Core/CityStateStatusHelper.lua').is_file():
        shutil.copy2(destination / 'Lua/tmp/eui/Core/CityStateStatusHelper.lua.ignore',
                     destination / 'Lua/UI/CityStateStatusHelper.lua')

    version = destination / 'Lua/Utilities/Lekmod_version.lua'
    original = 'return Network ~= nil and type(Network.HttpRequest) == "function"'
    contents = version.read_text()
    if contents.count(original) != 1:
        raise RuntimeError('Mac HTTP compatibility patch no longer matches the version helper.')
    version.write_text(contents.replace(
        original, 'return false'))


def prepare_lekmap(source, destination):
    if not any(source.glob('Lekmap*.lua')):
        raise RuntimeError(f'No Lekmap scripts found in {source}')
    shutil.copytree(source, destination, ignore=shutil.ignore_patterns('.*'))
