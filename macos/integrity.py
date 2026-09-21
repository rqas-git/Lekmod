
import hashlib
import json
from pathlib import Path

from game_install import sha256


def build_manifest_path(library):
    return library.with_name(library.name + '.build.json')


def write_build_manifest(library, source, configuration):
    manifest = build_manifest_path(library)
    temporary = manifest.with_suffix('.tmp')
    temporary.write_text(json.dumps({'format': 1, 'library_sha256': sha256(library),
                                    'source_sha256': source, 'configuration': configuration}, indent=2) + '\n')
    temporary.replace(manifest)


def validate_build_manifest(library, source):
    try:
        manifest = json.loads(build_manifest_path(library).read_text())
    except (OSError, ValueError) as error:
        raise RuntimeError('Native build provenance is missing or invalid. Run without --skip-build.') from error
    configuration = {'release': True, 'lto': False, 'precompute_neighbors': False}
    if (not isinstance(manifest, dict) or manifest.get('format') != 1
            or manifest.get('library_sha256') != sha256(library)
            or manifest.get('source_sha256') != source
            or manifest.get('configuration') != configuration):
        raise RuntimeError('Native library does not match the current source and release configuration. Run without --skip-build.')
    return manifest


def _fingerprint(files):
    digest = hashlib.sha256()
    for name, path in sorted(files):
        if path.is_symlink():
            raise RuntimeError(f'Unexpected symbolic link: {path}')
        digest.update(name.encode('utf-8') + b'\0')
        digest.update(sha256(path).encode('ascii') + b'\n')
    return digest.hexdigest()


def tree_digest(root, allow_empty=False):
    root = Path(root)
    if not root.is_dir() or root.is_symlink():
        raise RuntimeError(f'Missing or linked content directory: {root}')
    files = []
    for path in root.rglob('*'):
        if path.is_symlink():
            raise RuntimeError(f'Unexpected symbolic link: {path}')
        if path.is_file() and not any(part.startswith('.') for part in path.relative_to(root).parts):
            files.append((path.relative_to(root).as_posix(), path))
    if not files and not allow_empty:
        raise RuntimeError(f'Empty content directory: {root}')
    return _fingerprint(files)


def source_digest(root):

    files = []
    for directory in ('LEKMOD', 'LEKMOD_DLL/CvGameCoreDLL_Expansion2', 'macos/include'):
        base = root / directory
        if not base.is_dir():
            raise RuntimeError(f'Missing checkout directory: {base}')
        for path in base.rglob('*'):
            relative = path.relative_to(root)
            if any(part.startswith('.') or part in ('BuildOutput', 'BuildTemp', 'Debug', 'Release')
                   for part in relative.parts):
                continue
            if path.suffix.lower() in ('.dll', '.pdb', '.obj', '.log', '.exe'):
                continue
            if path.is_symlink():
                raise RuntimeError(f'Unexpected source link: {path}')
            if path.is_file():
                files.append((relative.as_posix(), path))
    for name in ('build.py', 'package_assets.py', 'crossplay.py', 'integrity.py', 'eui.py'):
        path = root / 'macos' / name
        files.append(('macos/' + name, path))

    files.append(('LekmodInstaller/ui_assets.py', root / 'LekmodInstaller/ui_assets.py'))
    return _fingerprint(files)
