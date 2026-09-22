#!/usr/bin/env python3

import argparse
import hashlib
from pathlib import Path
import shutil
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'LekmodInstaller'))
from ui_assets import load_manifest, materialize_release

STOCK_VERSION = 'v35.3'
STOCK_DLL_SHA256 = 'c8c265d26e6d67bab7c99371692a5b001d274cea6e80a3d9794b5626c994f357'


def validate_native(library):
    if not library.is_file():
        raise RuntimeError(f'Windows gameplay DLL is missing: {library}. Use the official {STOCK_VERSION} DLL.')
    data = library.read_bytes()
    if hashlib.sha256(data).hexdigest() != STOCK_DLL_SHA256:
        raise RuntimeError(f'Windows gameplay DLL does not match official Lekmod {STOCK_VERSION}. '
                           'Use the checked-in DLL or an identical release copy with --dll. '
                           'A custom build has not been validated against unchanged stock peers.')


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
    parser.add_argument('--dll', type=Path, help='Official v35.3 DLL; must match the pinned release hash')
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
