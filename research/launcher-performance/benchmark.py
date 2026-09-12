"""Compare launcher validation without opening Steam or modifying the game."""
import argparse
import json
from pathlib import Path
import statistics
import subprocess
import sys
import time
import types
from unittest.mock import patch


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path(__file__).resolve().parents[2])
    parser.add_argument('--baseline', default='38a4db60')
    parser.add_argument('--iterations', type=int, default=3)
    args = parser.parse_args()
    if args.iterations < 1:
        parser.error('--iterations must be positive')
    repo = args.repo.resolve()
    sys.path.insert(0, str(repo / 'macos'))
    import launcher
    settings = json.loads((Path.home() / 'Library/Application Support/Lekmod Launcher/settings.json').read_text())
    app = Path(settings['selected_app'])
    desired = settings['installations'][str(app)]['crossplay']

    def baseline_module(name):
        filename = repo / 'macos' / (name + '.py')
        source = subprocess.check_output(
            ['git', '-C', str(repo), 'show', f'{args.baseline}:macos/{name}.py'], text=True)
        module = types.ModuleType('baseline_' + name)
        module.__file__ = str(filename)
        exec(compile(source, str(filename), 'exec'), module.__dict__)
        return module

    baseline = baseline_module('launcher')
    baseline.validate_core = baseline_module('game_install').validate_core
    real_run = subprocess.run
    handoffs = 0

    def read_only_run(command, *args, **kwargs):
        nonlocal handoffs
        if command == ['/usr/bin/open', 'steam://rungameid/8930']:
            handoffs += 1
            return subprocess.CompletedProcess(command, 0)
        return real_run(command, *args, **kwargs)

    observations = []
    with patch.object(launcher.subprocess, 'run', side_effect=read_only_run), \
            patch.object(launcher.installer, 'install', side_effect=RuntimeError('Benchmark will not repair the game')), \
            patch.object(launcher, 'uninstall', side_effect=RuntimeError('Benchmark will not uninstall')):
        for trial in range(args.iterations):
            # Alternate the order to avoid always giving the candidate warmer caches.
            order = [('baseline', baseline), ('candidate', launcher)]
            if trial % 2:
                order.reverse()
            for action in ('status', 'launch'):
                for name, module in order:
                    started = time.perf_counter()
                    before = handoffs
                    with patch.object(module, 'inspect', wraps=module.inspect) as inspections:
                        report = module.run_action(app, desired, action)
                    if not report['ready'] or bool(report.get('launched')) != (action == 'launch'):
                        raise RuntimeError('The installed game must be ready for this benchmark')
                    if handoffs != before + (action == 'launch'):
                        raise RuntimeError('Unexpected number of intercepted Steam handoffs')
                    observation = dict(trial=trial + 1, version=name, action=action,
                                       seconds=round(time.perf_counter() - started, 4),
                                       inspections=inspections.call_count)
                    observations.append(observation)
                    print(json.dumps(observation), flush=True)
    medians = {action: {name: statistics.median(o['seconds'] for o in observations
                                               if o['version'] == name and o['action'] == action)
                        for name in ('baseline', 'candidate')}
               for action in ('status', 'launch')}
    print(json.dumps(dict(median_seconds=medians,
                          reduction_percent={action: round(100 * (1 - times['candidate'] / times['baseline']), 1)
                                             for action, times in medians.items()},
                          note='Warm repeated validation; Steam handoff intercepted. No game startup or save loading measured.'), indent=2))


if __name__ == '__main__':
    main()
