#!/usr/bin/env python3

import argparse
from pathlib import Path
import shutil
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'LekmodInstaller'))
from ui_assets import load_manifest, materialize_release


def package(source, destination):
    source, destination = Path(source).resolve(), Path(destination).resolve()
    if destination.is_relative_to(source):
        raise ValueError('Package destination must be outside the source directory.')
    manifest = load_manifest(source)
    aliases = {'Lua/tmp/' + path + '.ignore' for path in manifest.get('aliases', {})}
    def ignored(directory, names):
        directory = Path(directory)
        if directory == source / 'Lua/UI':
            return [name for name in names if name not in manifest['preserve']]
        return [name for name in names if name.startswith('.') or name in ('ui_check.bat', 'LekmodUiConfigured.lua')
                or (directory / name).relative_to(source).as_posix() in aliases]
    shutil.copytree(source, destination, ignore=ignored)
    materialize_release(destination)
    return destination


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, default=ROOT / 'LEKMOD')
    destination = parser.add_mutually_exclusive_group(required=True)
    destination.add_argument('--destination', type=Path, help='New package directory')
    destination.add_argument('--materialize', action='store_true', help='Restore generated paths in the checkout')
    args = parser.parse_args()
    if args.materialize:
        materialize_release(args.source)
    else:
        print(package(args.source, args.destination))


if __name__ == '__main__':
    main()
