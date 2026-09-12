
from collections import defaultdict
from contextlib import redirect_stdout
from datetime import datetime, timezone
import hashlib
import io
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'macos'))
import launcher


def metadata_snapshot(root):

    records = []
    def visit(directory):
        with os.scandir(directory) as entries:
            for entry in entries:
                stat = entry.stat(follow_symlinks=False)
                records.append((os.path.relpath(entry.path, root), stat.st_dev,
                                stat.st_ino, stat.st_mode, stat.st_size,
                                stat.st_mtime_ns, stat.st_ctime_ns))
                if entry.is_dir(follow_symlinks=False):
                    visit(entry.path)
    visit(root)
    return sorted(records)


def main():
    settings = json.loads((Path.home() / 'Library/Application Support/Lekmod Launcher/settings.json').read_text())
    app = Path(settings['selected_app'])
    evidence = {'observed_utc': datetime.now(timezone.utc).isoformat(),
                'timings': [], 'logs': {}, 'art': {}}
    def timed(name, operation):
        start = time.perf_counter()
        result = operation()
        evidence['timings'].append({'operation': name, 'seconds': time.perf_counter() - start})
        return result



    for trial in range(3):
        for name, flags in [('full_signature', []), ('code_only_diagnostic', ['--ignore-resources'])]:
            result = timed(name, lambda flags=flags: subprocess.run(
                ['/usr/bin/codesign', '--verify', '--strict', *flags, str(app)],
                capture_output=True, text=True, timeout=60))
            evidence['timings'][-1].update(trial=trial + 1, returncode=result.returncode)
            if result.returncode:
                evidence['timings'][-1]['error'] = result.stderr
        records = timed('bundle_metadata_walk', lambda: metadata_snapshot(app))
        evidence['timings'][-1].update(trial=trial + 1, entries=len(records))

    timed('checkout_digest', lambda: launcher.source_digest(ROOT))
    timed('installed_mod_digest', lambda: launcher.tree_digest(app / launcher.ASSETS / 'DLC/LEKMOD'))
    with redirect_stdout(io.StringIO()):
        timed('native_interface_check', lambda: launcher.check_imports(app / launcher.CORE, app))

    art = app / launcher.ASSETS / 'DLC/LEKMOD/Art'
    groups = defaultdict(lambda: {'files': 0, 'bytes': 0})
    names = defaultdict(list)
    for path in art.rglob('*'):
        if path.is_file():
            group = groups[path.suffix.lower()]
            group['files'] += 1
            group['bytes'] += path.stat().st_size
            names[path.name.casefold()].append(path.relative_to(art).as_posix())
    evidence['art'] = {'suffixes': dict(groups), 'files': sum(g['files'] for g in groups.values()),
                       'bytes': sum(g['bytes'] for g in groups.values()),
                       'duplicate_basenames': {name: paths for name, paths in names.items() if len(paths) > 1}}
    differing = []
    duplicates = evidence['art']['duplicate_basenames']
    for name, paths in duplicates.items():
        if len({hashlib.sha256((art / path).read_bytes()).hexdigest() for path in paths}) > 1:
            differing.append(name)
    evidence['art']['duplicate_basename_analysis'] = {
        'identical_groups': len(duplicates) - len(differing),
        'different_content_groups': len(differing), 'different_content_names': differing}
    temporary_ui = app / launcher.ASSETS / 'DLC/LEKMOD/Lua/tmp'
    files = [p for p in temporary_ui.rglob('*') if p.is_file()]
    evidence['unused_ui_sources_candidate'] = {'files': len(files), 'bytes': sum(p.stat().st_size for p in files)}

    logs = Path.home() / "Library/Application Support/Sid Meier's Civilization 5/Logs"
    for name in ('system_init.log', 'stopwatch.log', 'xml-perf.log'):
        path = logs / name
        evidence['logs'][name] = {'modified_utc': datetime.fromtimestamp(path.stat().st_mtime, timezone.utc).isoformat(),
                                  'text': path.read_text(errors='replace')}
    system_text = evidence['logs']['system_init.log']['text']
    evidence['system_startup_ms'] = {name: int(ms) for name, ms in re.findall(
        r'SystemInit: (.*?): (\d+) Milliseconds', system_text)}
    print(json.dumps(evidence, indent=2))


if __name__ == '__main__':
    main()
