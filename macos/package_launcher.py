#!/usr/bin/env python3

import argparse
import os
from pathlib import Path
import platform
import plistlib
import shutil
import subprocess
import sys
import sysconfig
import tempfile

from build_launcher import build
from game_install import CORE
from integrity import build_manifest_path, source_digest, validate_build_manifest

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent


def copy_sources(source, destination):
    excluded = {'BuildOutput', 'BuildTemp', 'Debug', 'Release', '__pycache__'}
    def ignore(directory, names):
        return [name for name in names if name.startswith('.') or name in excluded
                or Path(name).suffix.lower() in ('.dll', '.pdb', '.obj', '.log', '.exe')]
    shutil.copytree(source, destination, ignore=ignore)


def package(output):
    library = HERE / 'build' / CORE.name
    fingerprint = source_digest(ROOT)
    validate_build_manifest(library, fingerprint)
    launcher = build()
    output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='launcher-package-', dir=HERE / 'build') as temporary:
        temporary = Path(temporary)
        environment = dict(os.environ, PYINSTALLER_CONFIG_DIR=str(temporary / 'cache'))
        subprocess.run([sys.executable, '-m', 'PyInstaller', '--noconfirm', '--clean',
                        '--onedir', '--contents-directory', '.', '--name', 'Lekmod Service',
                        '--paths', str(HERE), '--paths', str(ROOT / 'LekmodInstaller'),
                        '--distpath', str(temporary / 'service'),
                        '--workpath', str(temporary / 'work'), '--specpath', str(temporary),
                        str(HERE / 'launcher_service.py')], check=True, env=environment)
        staging = temporary / 'disk'
        staging.mkdir()
        app = staging / launcher.name
        shutil.copytree(launcher, app, symlinks=True)
        payload = app / 'Contents/Resources/payload'
        for directory in ('LEKMOD', 'LEKMOD_DLL/CvGameCoreDLL_Expansion2', 'macos/include', 'Lekmap'):
            copy_sources(ROOT / directory, payload / directory)
        for name in HERE.glob('*.py'):
            shutil.copy2(name, payload / 'macos' / name.name)
        (payload / 'LekmodInstaller').mkdir()
        shutil.copy2(ROOT / 'LekmodInstaller/ui_assets.py', payload / 'LekmodInstaller/ui_assets.py')
        native = payload / 'macos/build' / library.name
        native.parent.mkdir()
        shutil.copy2(library, native)
        shutil.copy2(build_manifest_path(library), build_manifest_path(native))
        shutil.copytree(temporary / 'service/Lekmod Service', payload / 'macos',
                        symlinks=True, dirs_exist_ok=True)
        if source_digest(payload) != fingerprint or source_digest(ROOT) != fingerprint:
            raise RuntimeError('The checkout changed during packaging. Retry before distributing.')
        validate_build_manifest(native, fingerprint)
        info_path = app / 'Contents/Info.plist'
        info = plistlib.loads(info_path.read_bytes())
        info.pop('LekmodRepository', None)
        info.pop('LekmodPython', None)
        info['LekmodService'] = 'payload/macos/Lekmod Service'
        info['CFBundleShortVersionString'] = (ROOT / 'LEKMOD/VERSION').read_text().strip().removeprefix('v')
        target = sysconfig.get_config_var('MACOSX_DEPLOYMENT_TARGET') or '13.0'
        minimum = max((13, 0), tuple(map(int, (target + '.0').split('.')[:2])))
        info['LSMinimumSystemVersion'] = '.'.join(map(str, minimum))
        info_path.write_bytes(plistlib.dumps(info))
        subprocess.run(['codesign', '--force', '--deep', '--sign', '-', str(app)], check=True)
        subprocess.run(['codesign', '--verify', '--deep', '--strict', str(app)], check=True)
        validate_build_manifest(native, fingerprint)
        (staging / 'Applications').symlink_to('/Applications')
        (staging / 'Read Me.txt').write_text(
            'Drag Lekmod Launcher into Applications, then open it and choose Repair.\n'
            'Steam Civilization V and its required DLC must already be installed.\n'
            f'This build: {platform.machine()}, macOS {info["LSMinimumSystemVersion"]} or later.\n'
            'Python, the mod files and the prebuilt native core are included.\n'
            'Windows crossplay remains experimental; synchronization issues are unresolved.\n'
            'This local build is ad-hoc signed, not notarized for public distribution.\n')
        candidate = temporary / 'Launcher.dmg'
        subprocess.run(['hdiutil', 'create', '-volname', 'Lekmod Launcher', '-srcfolder', str(staging),
                        '-format', 'UDZO', '-ov', str(candidate)], check=True)
        candidate.replace(output)
    return output


def main():
    parser = argparse.ArgumentParser(description='Build a self-contained drag-to-Applications launcher DMG.')
    version = (ROOT / 'LEKMOD/VERSION').read_text().strip()
    parser.add_argument('--output', type=Path,
                        default=HERE / f'build/Lekmod-{version}-Launcher-{platform.machine()}.dmg')
    args = parser.parse_args()
    try:
        print(package(args.output.resolve()))
        return 0
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError) as error:
        print(f'Could not package the launcher: {error}', file=sys.stderr)
        return 1


if __name__ == '__main__':
    raise SystemExit(main())
