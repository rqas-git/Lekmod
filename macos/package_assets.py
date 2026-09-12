"""Prepare repository assets for the native Mac gameplay library."""
from pathlib import Path
import sys
import shutil

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "LekmodInstaller"))
from ui_assets import configure_ui


def prepare_lekmod(source, destination):
    shutil.copytree(source, destination,
                    ignore=lambda _path, names: [n for n in names
                        if n.startswith('.') or Path(n).suffix.lower() in ('.dll', '.pdb', '.bat')])
    configure_ui(destination, preserve_all=False)
    # Aspyr's HttpRequest crashes natively; Lua pcall cannot catch it.
    version = destination / 'Lua/Utilities/Lekmod_version.lua'
    original = 'return Network ~= nil and type(Network.HttpRequest) == "function"'
    contents = version.read_text()
    if contents.count(original) != 1:
        raise RuntimeError('Mac HTTP compatibility patch no longer matches the version helper.')
    version.write_text(contents.replace(
        original, 'return false -- Native Mac HTTP requests are unsupported'))


def prepare_lekmap(source, destination):
    if not any(source.glob('Lekmap*.lua')):
        raise RuntimeError(f'No Lekmap scripts found in {source}')
    shutil.copytree(source, destination, ignore=shutil.ignore_patterns('.*'))
