"""Browse Steam Civilization V saves and copy a selected save outside the game."""

import argparse
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import stat
import sys
import tempfile


SAVE_ROOT = Path.home() / "Library/Application Support/Sid Meier's Civilization 5/Saves"
CATEGORIES = frozenset(('single', 'multi', 'hotseat', 'pbem', 'pitboss'))
MAX_RESULTS = 200


def _root(root):
    root = Path(root)
    if root.is_symlink() or not root.is_dir():
        raise RuntimeError(f'Steam Civilization V save folder was not found: {root}')
    return root


def _save_file(root, relative):
    parts = PurePosixPath(relative).parts
    if (len(parts) not in (2, 3) or parts[0] not in CATEGORIES
            or any(part in ('', '.', '..') for part in parts)
            or Path(parts[-1]).suffix.casefold() != '.civ5save'):
        raise ValueError('Choose a Civilization V save from the browser.')
    path = root
    for part in parts[:-1]:
        path /= part
        if path.is_symlink() or not path.is_dir():
            raise ValueError('The save folder is missing or linked elsewhere.')
    path /= parts[-1]
    if path.is_symlink() or not path.is_file():
        raise ValueError('The selected save is missing or linked elsewhere.')
    return path


def list_saves(root=SAVE_ROOT):
    root = _root(root)
    saves = []

    def fail(error):
        raise error

    for directory, children, files in os.walk(root, topdown=True, followlinks=False, onerror=fail):
        folder = Path(directory)
        parts = folder.relative_to(root).parts
        if not parts:
            children[:] = [name for name in children if name in CATEGORIES
                           and not (folder / name).is_symlink()]
        elif len(parts) == 1:
            children[:] = [name for name in children if not (folder / name).is_symlink()]
        else:
            children.clear()
        if not parts:
            continue
        for name in files:
            if Path(name).suffix.casefold() != '.civ5save':
                continue
            path = folder / name
            info = path.stat(follow_symlinks=False)
            if stat.S_ISREG(info.st_mode):
                saves.append(dict(path=path.relative_to(root).as_posix(), name=name,
                                  category='/'.join(parts), modified=info.st_mtime,
                                  modified_ns=info.st_mtime_ns, size=info.st_size))
    saves.sort(key=lambda item: (-item['modified'], item['path'].casefold()))
    return saves[:MAX_RESULTS]


def copy_backup(relative, destination, root=SAVE_ROOT, expected_size=None, expected_modified_ns=None):
    root = _root(root)
    source = _save_file(root, relative)
    destination = Path(destination).expanduser()
    if destination.suffix.casefold() != '.civ5save':
        raise ValueError('Choose a backup filename ending in .Civ5Save.')
    if not destination.parent.is_dir():
        raise ValueError('Choose an existing backup folder.')
    if destination.parent.resolve().is_relative_to(root.resolve()):
        raise ValueError('Choose a backup destination outside the game save folder.')

    flags = os.O_RDONLY | getattr(os, 'O_NOFOLLOW', 0)
    with os.fdopen(os.open(source, flags), 'rb') as stream:
        before = os.fstat(stream.fileno())
        if not stat.S_ISREG(before.st_mode):
            raise ValueError('The selected save is not a regular file.')
        if ((expected_size is not None and before.st_size != expected_size)
                or (expected_modified_ns is not None and before.st_mtime_ns != expected_modified_ns)):
            raise RuntimeError('The save changed since it was listed. Refresh the browser and try again.')
        temporary = None
        try:
            with tempfile.NamedTemporaryFile(prefix='.lekmod-save-', dir=destination.parent,
                                             delete=False) as output:
                temporary = Path(output.name)
                digest = hashlib.sha256()
                while chunk := stream.read(1024 * 1024):
                    output.write(chunk)
                    digest.update(chunk)
                output.flush()
                os.fsync(output.fileno())
            after = os.fstat(stream.fileno())
            if (before.st_ino, before.st_size, before.st_mtime_ns) != (after.st_ino, after.st_size, after.st_mtime_ns):
                raise RuntimeError('The save changed during backup. Try again after the game finishes saving.')
            try:
                os.link(temporary, destination)
            except FileExistsError as error:
                raise ValueError('A backup with that name already exists. Choose a new name.') from error
            return dict(destination=str(destination), size=after.st_size,
                        sha256=digest.hexdigest())
        finally:
            if temporary is not None:
                temporary.unlink(missing_ok=True)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    actions = parser.add_subparsers(dest='action', required=True)
    actions.add_parser('list')
    backup = actions.add_parser('backup')
    backup.add_argument('save', help='Relative path from the save browser')
    backup.add_argument('destination', type=Path)
    backup.add_argument('size', type=int)
    backup.add_argument('modified_ns', type=int)
    args = parser.parse_args(argv)
    try:
        if args.action == 'list':
            print(json.dumps({'saves': list_saves(SAVE_ROOT), 'root': str(SAVE_ROOT)}))
        else:
            print(json.dumps(copy_backup(args.save, args.destination, expected_size=args.size,
                                         expected_modified_ns=args.modified_ns, root=SAVE_ROOT)))
        return 0
    except (OSError, ValueError, RuntimeError) as error:
        print(json.dumps({'error': str(error)}))
        return 1


if __name__ == '__main__':
    sys.exit(main())
