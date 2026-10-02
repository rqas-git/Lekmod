#!/usr/bin/env python3
"""Prepare, run, and inspect isolated native Mac Lekmod AI smoke tests."""
import argparse
import configparser
from contextlib import closing
from datetime import datetime, timezone
import fcntl
import hashlib
import importlib
import json
import os
from pathlib import Path
import plistlib
import re
import shutil
import signal
import sqlite3
import struct
import subprocess
import sys
import tempfile
import time
import uuid

ASSETS = Path('Contents/Assets/Assets')
CORE = Path('Contents/MacOS/libCvGameCoreDLL_Expansion2_DLL.dylib')
SUPPORT = Path("Library/Application Support/Sid Meier's Civilization 5")
DEFAULT_APP = Path.home() / 'Library/Application Support/Steam/steamapps/common' / "Sid Meier's Civilization V/Civilization V.app"
DEFAULT_MAP = 'LekmapPangaeaFractalv6.3.lua'
BUNDLED = Path(__file__).resolve().parents[1] / 'assets'
SOURCE_DIRS = ('LEKMOD', 'LEKMOD_DLL', 'LekmodInstaller', 'Lekmap', 'macos')
EXCLUDED = {'build', '__pycache__', 'BuildOutput', 'BuildTemp', 'Debug', 'Release', '.vs'}


class EvidencePending(ValueError):
    """A completion marker may arrive before the quicksave finishes flushing."""


def sha256(path):
    with path.open('rb') as stream:
        digest = hashlib.sha256()
        for chunk in iter(lambda: stream.read(1024 * 1024), b''):
            digest.update(chunk)
    return digest.hexdigest()


def write_json(path, value):
    temporary = path.with_suffix(path.suffix + '.tmp')
    temporary.write_text(json.dumps(value, indent=2) + '\n')
    temporary.replace(path)


def helpers(repo):
    # Each CLI invocation imports one checkout, or its frozen copy.
    sys.path.insert(0, str(repo / 'macos'))
    return {name: importlib.import_module(name) for name in
            ('game_install', 'integrity', 'audit', 'package_assets')}


def fingerprint(repo):
    """Include packaging code and maps in addition to core build provenance."""
    digest = hashlib.sha256()
    for directory in SOURCE_DIRS:
        for path in sorted((repo / directory).rglob('*')):
            parts = path.relative_to(repo).parts
            if any(p in EXCLUDED or p.startswith('.') for p in parts):
                continue
            if path.is_symlink():
                raise RuntimeError(f'Unexpected source link: {path}')
            if path.is_file():
                digest.update(path.relative_to(repo).as_posix().encode() + b'\0')
                digest.update(sha256(path).encode() + b'\n')
    return digest.hexdigest()


def private_path(run, path):
    path = path.resolve()
    if not path.is_relative_to(run.resolve()):
        raise RuntimeError(f'Test path escapes its run directory: {path}')
    return path


def low_priority():
    os.nice(10)


def preflight(args):
    if sys.platform != 'darwin':
        raise RuntimeError('This skill runner supports native macOS only.')
    repo, app = args.repo.resolve(), args.app.expanduser().resolve()
    for directory in SOURCE_DIRS:
        if not (repo / directory).is_dir():
            raise RuntimeError(f'Missing checkout directory: {repo / directory}')
    for command in ('clang', 'clang++', 'codesign', 'cp'):
        if not shutil.which(command):
            raise RuntimeError(f'Missing required command: {command}')
    modules = helpers(repo)
    modules['game_install'].validate_app(app)
    if Path(args.map).name != args.map or not (repo / 'Lekmap' / args.map).is_file():
        raise RuntimeError('--map must name a script in this checkout\'s Lekmap directory.')
    if not (Path.home() / SUPPORT / 'config.ini').is_file():
        raise RuntimeError('Run Civ V normally once to create its ordinary settings profile.')
    result = {'repo': str(repo), 'app': str(app),
              'source_sha256': modules['integrity'].source_digest(repo),
              'eui': (app / ASSETS / 'DLC/UI_bc1').is_dir(),
              'map': args.map, 'desktop_input': 'none'}
    print(json.dumps(result, indent=2), flush=True)
    return modules


def snapshot(repo, destination):
    before = fingerprint(repo)
    destination.mkdir()
    def copy_file(source, target):
        # Clone individual files while excluding generated directories up front.
        result = subprocess.run(['cp', '-c', source, target], capture_output=True)
        if result.returncode:
            shutil.copy2(source, target)
        return target
    def ignore(_path, names):
        return [n for n in names if n in EXCLUDED or n.startswith('.')]
    for name in SOURCE_DIRS:
        shutil.copytree(repo / name, destination / name, copy_function=copy_file, ignore=ignore)
    frozen = fingerprint(destination)
    if before != frozen or fingerprint(repo) != before:
        raise RuntimeError('Checkout changed during snapshot; preserve this attempt and retry in a fresh directory.')
    return frozen


def settings(path, replacements):
    parser = configparser.ConfigParser(strict=False, interpolation=None)
    parser.optionxform = str
    parser.read(path)
    for section, values in replacements.items():
        if not parser.has_section(section):
            parser.add_section(section)
        for key, value in values.items():
            parser[section][key] = str(value)
    # These host settings may live in different sections across installations.
    for section in parser.sections():
        for key, value in (('MaxSimultaneousThreads', '2'), ('ThrottleOnLossOfFocus', '0')):
            if key in parser[section]:
                parser[section][key] = value
    with path.open('w') as stream:
        parser.write(stream)


def create_profile(run, bundle_id, map_script):
    profile = run / 'profile'
    support = profile / SUPPORT
    support.mkdir(parents=True)
    ordinary = Path.home() / SUPPORT
    for name in ('config.ini', 'UserSettings.ini', 'GraphicsSettingsDX9.ini'):
        if (ordinary / name).is_file():
            shutil.copy2(ordinary / name, support / name)
    if (ordinary / 'Text').is_dir():
        shutil.copytree(ordinary / 'Text', support / 'Text')
    settings(support / 'config.ini', {
        'CONFIG': {'Audio': 1},
        'DEBUG': {'LoggingEnabled': 1, 'AILog': 1, 'AIPerfLog': 1,
                  'BuilderAILog': 1, 'MessageLog': 1, 'PlayerAndCityAILogSplit': 1,
                  'Autorun': 0, 'AutorunTurnLimit': 0},
        'Debugging': {'EnableTuner': 0},
        'GAME': {'QuickCombat': 1, 'QuickStart': 0, 'GameSpeed': 'GAMESPEED_QUICK',
                 'Map': map_script, 'WorldSize': 'WORLDSIZE_SMALL', 'QuickHandicap': 'HANDICAP_PRINCE'},
    })
    settings(support / 'UserSettings.ini', {
        'AutoSave': {'TurnsBetweenAutosave': 10, 'NumAutosavesKept': 5},
        'GameSettings': {'SkipIntroVideo': 1, 'SinglePlayerQuickCombatEnabled': 1,
                         'SinglePlayerQuickMovementEnabled': 1, 'BindMouse': 0},
    })
    settings(support / 'GraphicsSettingsDX9.ini', {'GraphicsSettings': {'WindowResX': 1024, 'WindowResY': 768}})
    pref = profile / 'Library/Preferences' / (bundle_id + '.plist')
    pref.parent.mkdir(parents=True)
    pref.write_bytes(plistlib.dumps({'DisplayFullScreen': False}))
    return profile


def install_telemetry(app, target, eui):
    automation = app / ASSETS / 'Automation/LekmodBackgroundTest.lua'
    automation.parent.mkdir(parents=True, exist_ok=True)
    automation.write_text((BUNDLED / 'Automation.lua').read_text().replace('__TURN_TARGET__', str(target)))
    in_game = app / ASSETS / 'DLC/LEKMOD/Lua/UI/InGame.lua'
    marker = '\n-- BACKGROUND VALIDATION TELEMETRY\n'
    contents = in_game.read_text()
    if marker in contents:
        raise RuntimeError('Source already contains test instrumentation; refusing duplicate injection.')
    in_game.write_text(contents + marker + (BUNDLED / 'Telemetry.lua').read_text().replace('__TURN_TARGET__', str(target)))
    loadscreen = eui / 'GameSetup/LoadScreen.lua' if eui else app / ASSETS / 'UI/FrontEnd/LoadScreen.lua'
    contents = loadscreen.read_text()
    pattern = r'if\s*\(?(?:\s*)PreGame\.IsMultiplayerGame\(\)\s+or\s+PreGame\.IsHotSeatGame\(\)\s*\)?\s+then'
    contents, count = re.subn(pattern, 'if true then -- Background test uses the existing activation path.', contents)
    if count != 1:
        raise RuntimeError(f'Loading-screen patch requires one match, found {count}: {loadscreen}')
    loadscreen.write_text(contents)


def prepare(args):
    preflight(args)
    repo, original = args.repo.resolve(), args.app.expanduser().resolve()
    parent = args.output_root.resolve() if args.output_root else repo / 'macos/build/background-tests'
    for directory in SOURCE_DIRS:
        if parent.is_relative_to(repo / directory) and not parent.is_relative_to(repo / 'macos/build'):
            raise RuntimeError('Output cannot be inside a snapshotted source tree; use macos/build or an external directory.')
    parent.mkdir(parents=True, exist_ok=True)
    run = Path(tempfile.mkdtemp(prefix=datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%SZ-'), dir=parent))
    print(f'RUN_DIR={run}', flush=True)
    frozen = run / 'source'
    packaging_digest = snapshot(repo, frozen)
    # Imports from the live checkout may now differ: execute preparation in a
    # new interpreter that imports only the frozen source.
    write_json(run / 'prepare-request.json', {
        'app': str(original), 'turn_target': args.turns, 'jobs': args.jobs,
        'map': args.map, 'expected_civilizations': args.expect_civ,
        'reuse_release': str(args.reuse_release.resolve()) if args.reuse_release else None,
        'checkout': str(repo), 'snapshot_sha256': packaging_digest,
    })
    subprocess.run([sys.executable, str(Path(__file__).resolve()), '_stage', '--run-dir', str(run)],
                   check=True, preexec_fn=low_priority)
    print(f'Prepared. RUN_DIR={run}', flush=True)


def stage(run):
    run = run.resolve()
    if (run / 'Civilization V Background Test.app').exists() or (run / 'profile').exists():
        raise RuntimeError('Staging was already attempted here; prepare a fresh run directory.')
    request = json.loads((run / 'prepare-request.json').read_text())
    frozen = private_path(run, run / 'source')
    modules = helpers(frozen)
    installer, integrity = modules['game_install'], modules['integrity']
    source_digest = integrity.source_digest(frozen)
    library = frozen / 'macos/build' / CORE.name
    if request['reuse_release']:
        reuse = Path(request['reuse_release'])
        integrity.validate_build_manifest(reuse, source_digest)
        library.parent.mkdir(parents=True)
        shutil.copy2(reuse, library)
        shutil.copy2(integrity.build_manifest_path(reuse), integrity.build_manifest_path(library))
    else:
        with (run / 'build.log').open('w') as log:
            subprocess.run([sys.executable, str(frozen / 'macos/build.py'), '--release',
                            '--jobs', str(request['jobs']), '--app', request['app']], stdout=log,
                           stderr=subprocess.STDOUT, check=True)
    integrity.validate_build_manifest(library, source_digest)
    app = run / 'Civilization V Background Test.app'
    installer.clone_app(Path(request['app']), app)
    installer.validate_app(app)
    for path in app.rglob('*'):
        if path.is_symlink() and not path.resolve().is_relative_to(app.resolve()):
            raise RuntimeError(f'Cloned app contains an external link: {path}')
    bundle_id = 'org.lekmod.backgroundvalidation.r' + uuid.uuid4().hex
    plist = app / 'Contents/Info.plist'
    data = plistlib.loads(plist.read_bytes())
    data.update({'CFBundleIdentifier': bundle_id, 'CFBundleName': 'Lekmod Background Test',
                 'CFBundleDisplayName': 'Lekmod Background Test',
                 'CFBundleExecutable': 'Civilization V', 'LSBackgroundOnly': True})
    data.pop('LSUIElement', None)
    plist.write_bytes(plistlib.dumps(data))
    map_script = (installer.LEKMAP / request['map']).relative_to(ASSETS.parent).as_posix()
    profile = create_profile(run, bundle_id, map_script)
    eui = app / ASSETS / 'DLC/UI_bc1'
    eui = eui if eui.is_dir() else None
    legacy_maps = getattr(installer, 'LEGACY_LEKMAPS', (installer.LEGACY_LEKMAP,))
    for target in (app / ASSETS / 'DLC/LEKMOD', app / installer.LEKMAP,
                   *[app / path for path in legacy_maps]):
        target = private_path(run, target)
        if target.exists():
            shutil.rmtree(target)
    modules['package_assets'].prepare_lekmod(frozen / 'LEKMOD', app / ASSETS / 'DLC/LEKMOD', eui=eui)
    modules['package_assets'].prepare_lekmap(frozen / 'Lekmap', app / installer.LEKMAP)
    install_telemetry(app, request['turn_target'], eui)
    modules['audit'].check_imports(library, app)
    shutil.copy2(library, app / CORE)
    guard = app / 'Contents/MacOS/background_guard.dylib'
    subprocess.run(['clang', '-arch', 'x86_64', '-mmacosx-version-min=10.13', '-dynamiclib',
                    '-framework', 'AppKit', '-framework', 'Carbon', '-Wno-deprecated-declarations',
                    str(BUNDLED / 'background_guard.m'), '-o', str(guard)], check=True)
    installer.sign_core(app)
    # The executable is treated as a bundle root by codesign. Every nested
    # dylib must already have a valid signature before it can be repaired.
    for binary in sorted((app / 'Contents/MacOS').glob('*.dylib')):
        valid = subprocess.run(['codesign', '--verify', '--strict', str(binary)], capture_output=True)
        if valid.returncode:
            subprocess.run(['codesign', '--force', '--sign', '-', str(binary)], check=True)
    installer.sign_nested(app)
    installer.sign_app(app)
    hashes = {str(p.relative_to(run)): sha256(p) for p in
              (app / CORE, guard, app / 'Contents/MacOS/Civilization V', plist,
               app / ASSETS / 'Automation/LekmodBackgroundTest.lua',
               app / ASSETS / 'DLC/LEKMOD/Lua/UI/InGame.lua',
               eui / 'GameSetup/LoadScreen.lua' if eui else app / ASSETS / 'UI/FrontEnd/LoadScreen.lua')}
    write_json(run / 'run-manifest.json', {
        'format': 1, 'app': str(app), 'private_profile': str(profile), 'bundle_id': bundle_id,
        'source_sha256': source_digest, 'snapshot_sha256': request['snapshot_sha256'],
        'source_core_sha256': sha256(library), 'core_sha256': sha256(app / CORE),
        'staged_sha256': hashes, 'turn_target': request['turn_target'], 'requested_map': map_script,
        'expected_civilizations': request['expected_civilizations'], 'eui': bool(eui),
        'input': 'none; native Lua automation', 'checkout': request['checkout'],
    })


def ledger_values(run):
    path = run / 'profile' / SUPPORT / 'ModUserData/LekmodBackgroundValidation-1.db'
    if not path.is_file():
        return {}
    with closing(sqlite3.connect(path.resolve().as_uri() + '?mode=ro', uri=True, timeout=1)) as connection:
        return dict(connection.execute('SELECT Name, Value FROM SimpleValues'))


def save_header(data):
    if len(data) < 12 or data[:4] != b'CIV5':
        raise ValueError('Missing or invalid CIV5 save header.')
    offset, strings = 8, []
    for _ in range(2):
        if offset + 4 > len(data):
            raise ValueError('Truncated save string length.')
        size = struct.unpack_from('<I', data, offset)[0]
        offset += 4
        if size > 4096 or offset + size > len(data):
            raise ValueError('Invalid or truncated save string.')
        strings.append(data[offset:offset + size].decode('utf-8'))
        offset += size
    if offset + 4 > len(data):
        raise ValueError('Truncated save turn.')
    return strings, struct.unpack_from('<I', data, offset)[0]


def verify_evidence(run):
    manifest = json.loads((run / 'run-manifest.json').read_text())
    target = int(manifest['turn_target'])
    values = ledger_values(run)
    if values.get('error'):
        raise ValueError(f'Telemetry failure: {values["error"]}')
    if int(values.get('turn', -1)) != target or values.get('result') != f'completed-{target}-turns':
        raise ValueError('Telemetry has not completed the target turn.')
    turns = {}
    for key, value in values.items():
        match = re.fullmatch(r'turn\.(\d+)\.player\.(\d+)\.(.+)', key)
        if match:
            turn, player, metric = match.groups()
            turns.setdefault(turn, {}).setdefault(player, {})[metric] = value
    if sorted(map(int, turns)) != list(range(target + 1)):
        raise ValueError('Missing or unexpected turn snapshots.')
    if len(turns['0']) < 2:
        raise ValueError('Smoke test needs at least two live major civilizations.')
    required = {'civilization', 'alive', 'cities', 'units', 'population', 'technologies', 'policies', 'score', 'gold', 'science'}
    for turn, players in turns.items():
        for player, stats in players.items():
            if not required <= stats.keys() or str(stats['alive']).lower() not in ('true', '1'):
                raise ValueError(f'Incomplete player metrics: turn {turn}, player {player}')
    first, final = turns['0'], turns[str(target)]
    if not any(int(stats['cities']) > 0 and int(stats['population']) > 0 for stats in final.values()):
        raise ValueError('No civilization established a populated city.')
    if first == final:
        raise ValueError('No observable civilization activity across the test.')
    actual_civs = sorted(stats['civilization'] for stats in first.values())
    expected = manifest.get('expected_civilizations', [])
    if expected and sorted(expected) != actual_civs:
        raise ValueError(f'Civilization setup mismatch: expected {sorted(expected)}, actual {actual_civs}')
    if manifest.get('requested_map') and str(values.get('map_script', '')).replace('\\', '/') != manifest['requested_map']:
        raise ValueError('Actual map differs from requested map.')
    save = run / 'profile' / SUPPORT / 'Saves/single/quick/QuickSave.Civ5Save'
    try:
        data = save.read_bytes()
        header, save_turn = save_header(data)
    except (OSError, ValueError) as error:
        raise EvidencePending(f'Quicksave is not yet readable: {error}') from error
    if save_turn != target:
        raise EvidencePending(f'Save turn {save_turn} differs from target {target}.')
    return manifest, values, turns, header, data


def report(run):
    run = run.resolve()
    manifest, values, turns, header, data = verify_evidence(run)
    errors, warnings = [], []
    logs = run / 'profile' / SUPPORT / 'Logs'
    for name in ('Lua.log', 'Database.log', 'xml.log'):
        path = logs / name
        if not path.is_file():
            continue
        for line in path.read_text(errors='replace').splitlines():
            line = re.sub(r'^\[[^]]+\]\s*', '', line)
            finding = f'{name}: {line}'
            if 'Runtime Error:' in line:
                errors.append(finding)
            elif any(token in line for token in ('table does not exist, check the xml!', 'constraint failed', 'ERROR', 'Error:')):
                warnings.append(finding)
    errors, warnings = list(dict.fromkeys(errors)), list(dict.fromkeys(warnings))
    shutdown = json.loads((run / 'process-result.json').read_text()) if (run / 'process-result.json').is_file() else None
    target = int(manifest['turn_target'])
    exported_save = run / f'Lekmod-turn-{target}.Civ5Save'
    exported_save.write_bytes(data)
    result = {
        'result': f'completed-{target}-turns', 'assessment': 'completed-with-findings' if errors or warnings else 'completed',
        'turns_recorded': len(turns), 'save_turn': target, 'game_header': header,
        'source_sha256': manifest['source_sha256'], 'core_sha256': manifest['core_sha256'],
        'save_sha256': hashlib.sha256(data).hexdigest(), 'manifest': manifest,
        'settings': {k: v for k, v in values.items() if not k.startswith('turn.')},
        'turns': turns, 'runtime_errors': errors, 'warnings': warnings, 'shutdown': shutdown,
        'scope': 'Native Mac single-player AI smoke test; hidden copy, private profile, no desktop input.',
        'limitations': ['Visual UI and human choices were not inspected.', 'Multiplayer and save reload were not tested.'],
    }
    write_json(run / 'validation-results.json', result)
    lines = [f'Lekmod background validation: {target} turns completed', '',
             f'Assessment: {result["assessment"]}', f'All turns 0–{target} recorded; quicksave header independently verified.',
             f'Map: {values.get("map_script")} ({values.get("map_width")} × {values.get("map_height")})',
             f'Speed: {values.get("game_speed")}', '', 'Final civilizations:']
    for stats in turns[str(target)].values():
        lines.append(f'  {stats["civilization"]}: cities {stats["cities"]}, population {stats["population"]}, '
                     f'units {stats["units"]}, technologies {stats["technologies"]}, policies {stats["policies"]}.')
    lines += ['', 'Runtime errors:'] + (['  ' + e for e in errors] or ['  None recorded.'])
    lines += ['', 'Warnings:'] + (['  ' + w for w in warnings] or ['  None recorded.'])
    lines += ['', f'Shutdown: {json.dumps(shutdown)}', result['scope'], *result['limitations'],
              f'Source SHA-256: {result["source_sha256"]}', f'Signed core SHA-256: {result["core_sha256"]}',
              f'Save SHA-256: {result["save_sha256"]}']
    (run / 'validation-report.txt').write_text('\n'.join(lines) + '\n')
    print(f'{result["assessment"]}: turns 0–{target}; {run / "validation-report.txt"}', flush=True)
    return result


def stop_owned(game, grace=5):
    """Only signal the live, unreaped Popen child, never a PID file."""
    if game.poll() is not None:
        return 'already-exited'
    game.terminate()
    try:
        game.wait(timeout=grace)
        return 'sigterm'
    except subprocess.TimeoutExpired:
        game.kill()
        game.wait(timeout=10)
        return 'sigkill-after-grace'


def monitor(game, run, timeout, guard_timeout=20, interval=2):
    started, next_update, ready = time.monotonic(), 0, False
    while True:
        elapsed = time.monotonic() - started
        if not ready:
            with (run / 'runtime.log').open('rb') as stream:
                startup = stream.read(65536)
            ready = b'BACKGROUND_GUARD ready;' in startup
            if not ready and elapsed >= guard_timeout:
                raise RuntimeError('No guard acknowledgement; background run refused.')
        try:
            values = ledger_values(run)
        except sqlite3.OperationalError:
            values = {} # Database creation or a transient writer lock.
        if values.get('error'):
            raise RuntimeError(f'Telemetry failure: {values["error"]}')
        if ready and str(values.get('result', '')).startswith('completed-'):
            try:
                verify_evidence(run)
                return
            except (EvidencePending, sqlite3.OperationalError):
                # Quicksave may still be flushing; full validation is retried.
                pass
        if game.poll() is not None:
            raise RuntimeError(f'Test process exited before verified completion: {game.returncode}')
        if elapsed >= timeout:
            raise RuntimeError(f'Timed out after {timeout}s; evidence retained at {run}')
        if elapsed >= next_update:
            print(f'{int(elapsed)}s: guard={ready}, turn={values.get("turn", "initializing/map generation")}', flush=True)
            next_update = elapsed + 30
        time.sleep(interval)


def play(args):
    run = args.run_dir.resolve()
    manifest = json.loads((run / 'run-manifest.json').read_text())
    app = private_path(run, Path(manifest['app']))
    profile = private_path(run, Path(manifest['private_profile']))
    if (run / 'game.pid').exists() or (run / 'process-result.json').exists():
        raise RuntimeError('This run was already launched; prepare a fresh directory for a retry.')
    if manifest.get('format') != 1 or not manifest.get('staged_sha256'):
        raise RuntimeError('Missing staged background checks; prepare with this runner first.')
    for name, digest in manifest['staged_sha256'].items():
        path = private_path(run, run / name)
        if sha256(path) != digest:
            raise RuntimeError(f'Staged file changed after signing: {path}')
    plist = plistlib.loads((app / 'Contents/Info.plist').read_bytes())
    if not plist.get('LSBackgroundOnly') or plist.get('CFBundleIdentifier') != manifest['bundle_id']:
        raise RuntimeError('Background-only bundle identity check failed.')
    subprocess.run(['codesign', '--verify', '--strict', str(app)], check=True)
    # One test at a time across checkouts. The OS releases this lock on exit.
    lock_path = Path(tempfile.gettempdir()) / f'lekmod-background-test-{os.getuid()}.lock'
    with lock_path.open('a') as lock:
        try:
            fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except BlockingIOError as error:
            raise RuntimeError('Another background test runner is active.') from error
        environment = os.environ.copy()
        environment.update({'CFFIXED_USER_HOME': str(profile), 'SDL_MAC_BACKGROUND_APP': '1',
                            'SDL_VIDEO_MAC_FULLSCREEN_SPACES': '0', 'SDL_VIDEO_ALLOW_SCREENSAVER': '1',
                            'SteamAppId': '8930', 'SteamGameId': '8930',
                            'DYLD_INSERT_LIBRARIES': str(app / 'Contents/MacOS/background_guard.dylib')})
        for key in ('LANG', 'LC_ALL', 'LC_CTYPE'):
            if environment.get(key) == 'C.UTF-8':
                environment[key] = 'en_US.UTF-8'
        executable = app / 'Contents/MacOS/Civilization V'
        failure = None
        with (run / 'runtime.log').open('wb') as output:
            game = subprocess.Popen([str(executable), '-Automation', 'LekmodBackgroundTest.lua',
                                     '-DisplayFullScreen', 'NO'], cwd=executable.parent, env=environment,
                                    stdout=output, stderr=subprocess.STDOUT, start_new_session=True,
                                    preexec_fn=low_priority)
            (run / 'game.pid').write_text(str(game.pid) + '\n')
            print(f'Owned background test PID {game.pid}; no desktop input.', flush=True)
            try:
                monitor(game, run, args.timeout)
                # Export while the live child is still owned and the save verified.
                report(run)
            except (Exception, KeyboardInterrupt) as error:
                failure = str(error) or 'interrupted'
            finally:
                shutdown = stop_owned(game)
                write_json(run / 'process-result.json', {'pid': game.pid, 'exit_code': game.returncode,
                           'cleanup': shutdown, 'verified_completion': failure is None, 'failure': failure})
        if failure:
            raise RuntimeError(f'{failure}; test child stopped, artifacts retained: {run}')
        report(run) # Include flushed logs and shutdown outcome.


def positive(value):
    number = int(value)
    if number < 1:
        raise argparse.ArgumentTypeError('Must be a positive integer.')
    return number


def main():
    def interrupted(_signal, _frame):
        raise KeyboardInterrupt('runner interrupted')
    signal.signal(signal.SIGTERM, interrupted)
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest='command', required=True)
    for name in ('preflight', 'prepare'):
        command = commands.add_parser(name)
        command.add_argument('--repo', type=Path, required=True)
        command.add_argument('--app', type=Path, default=DEFAULT_APP)
        command.add_argument('--map', default=DEFAULT_MAP)
        if name == 'prepare':
            command.add_argument('--turns', type=positive, default=30)
            command.add_argument('--jobs', type=positive, default=2)
            command.add_argument('--output-root', type=Path)
            command.add_argument('--reuse-release', type=Path)
            command.add_argument('--expect-civ', action='append', default=[])
    for name in ('play', 'report', '_stage'):
        command = commands.add_parser(name)
        command.add_argument('--run-dir', type=Path, required=True)
        if name == 'play':
            command.add_argument('--timeout', type=positive, default=900)
    args = parser.parse_args()
    try:
        if args.command == 'preflight': preflight(args)
        elif args.command == 'prepare': prepare(args)
        elif args.command == '_stage': stage(args.run_dir)
        elif args.command == 'play': play(args)
        else: report(args.run_dir)
    except (RuntimeError, ValueError, OSError, subprocess.SubprocessError, sqlite3.Error) as error:
        print(f'ERROR: {error}', file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
