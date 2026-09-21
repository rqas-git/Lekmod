#!/usr/bin/env python3

import argparse
from pathlib import Path
import shutil
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'LekmodInstaller'))
from ui_assets import load_manifest, materialize_release


def validate_native(library):
    if not library.is_file():
        raise RuntimeError(f'Windows gameplay DLL is missing: {library}. Build the Mod configuration first.')
    data = library.read_bytes()
    required = (b'ChangeOverflowResearch', b'GetLekmodCoreVersion')
    if not data.startswith(b'MZ') or any(method not in data for method in required):
        raise RuntimeError('Windows gameplay DLL is stale or incompatible. Rebuild the current Mod configuration '
                           'and pass its output with --dll; the checked-in DLL must not be released.')


def package(source, destination, dll=None):
    source, destination = Path(source).resolve(), Path(destination).resolve()
    if destination.is_relative_to(source):
        raise ValueError('Package destination must be outside the source directory.')
    library = Path(dll).resolve() if dll else source / 'CvGameCore_Expansion2.dll'
    validate_native(library)
    manifest = load_manifest(source)
    aliases = {'Lua/tmp/' + path + '.ignore' for path in manifest.get('aliases', {})}
    def ignored(directory, names):
        directory = Path(directory)
        if directory == source / 'Lua/UI':
            return [name for name in names if name not in manifest['preserve']]
        return [name for name in names if name.startswith('.') or name in ('ui_check.bat', 'LekmodUiConfigured.lua')
                or (directory / name).relative_to(source).as_posix() in aliases]
    shutil.copytree(source, destination, ignore=ignored)
    shutil.copy2(library, destination / 'CvGameCore_Expansion2.dll')
    materialize_release(destination)
    return destination


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, default=ROOT / 'LEKMOD')
    parser.add_argument('--dll', type=Path, help='Fresh Windows Mod build to package instead of the checked-in DLL')
    destination = parser.add_mutually_exclusive_group(required=True)
    destination.add_argument('--destination', type=Path, help='New package directory')
    destination.add_argument('--materialize', action='store_true', help='Restore generated paths in the checkout')
    args = parser.parse_args()
    if args.materialize:
        materialize_release(args.source)
    else:
        print(package(args.source, args.destination, args.dll))


if __name__ == '__main__':
    main()
