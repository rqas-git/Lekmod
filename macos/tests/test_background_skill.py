"""Exercise background-skill evidence and child ownership without desktop UI."""
import importlib.util
from contextlib import closing
from pathlib import Path
import sqlite3
import struct
import subprocess
import sys
import tempfile
import unittest

SKILL = Path(__file__).resolve().parents[2] / 'skills/lekmod-test'
spec = importlib.util.spec_from_file_location('lekmod_background_test', SKILL / 'scripts/background_test.py')
runner = importlib.util.module_from_spec(spec)
spec.loader.exec_module(runner)


def header(turn):
    data = b'CIV5' + b'\0' * 4
    for text in (b'1.0.3.279 (180925)', b'180925'):
        data += struct.pack('<I', len(text)) + text
    return data + struct.pack('<I', turn) + b'fixture-body'


class EvidenceTest(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix='lekmod-skill-test-')
        self.addCleanup(self.temporary.cleanup)
        self.run = Path(self.temporary.name)
        self.support = self.run / 'profile' / runner.SUPPORT
        (self.support / 'ModUserData').mkdir(parents=True)
        self.database = self.support / 'ModUserData/LekmodBackgroundValidation-1.db'
        self.manifest = {'turn_target': 1, 'source_sha256': 'fixture-source', 'core_sha256': 'fixture-core',
                         'requested_map': 'Assets/Maps/Lekmap v6.2/example.lua'}
        runner.write_json(self.run / 'run-manifest.json', self.manifest)
        self.values = {'turn': '1', 'result': 'completed-1-turns', 'map_script': self.manifest['requested_map']}
        for turn in (0, 1):
            for player in (0, 1):
                stats = {'civilization': f'CIVILIZATION_TEST_{player}', 'alive': 'true', 'cities': turn,
                         'units': 2 - turn, 'population': turn, 'technologies': turn, 'policies': 0,
                         'score': turn * 10, 'gold': turn * 5, 'science': turn * 3}
                for metric, value in stats.items():
                    self.values[f'turn.{turn}.player.{player}.{metric}'] = str(value)
        self.write_ledger()
        self.save = self.support / 'Saves/single/quick/QuickSave.Civ5Save'
        self.save.parent.mkdir(parents=True)
        self.save.write_bytes(header(1))
        (self.run / 'runtime.log').write_text('BACKGROUND_GUARD ready; fixture\n')

    def write_ledger(self):
        with closing(sqlite3.connect(self.database)) as connection, connection:
            connection.execute('CREATE TABLE IF NOT EXISTS SimpleValues(Name TEXT PRIMARY KEY, Value VARIANT)')
            connection.execute('DELETE FROM SimpleValues')
            connection.executemany('INSERT INTO SimpleValues VALUES (?,?)', self.values.items())

    def test_export_uses_all_turns_save_and_findings(self):
        logs = self.support / 'Logs'
        logs.mkdir()
        (logs / 'Lua.log').write_text('[10] Runtime Error: example.lua: missing API\n[11] table does not exist, check the xml!\n')
        result = runner.report(self.run)
        self.assertEqual(result['turns_recorded'], 2)
        self.assertEqual(result['assessment'], 'completed-with-findings')
        self.assertEqual(len(result['runtime_errors']), 1)
        self.assertEqual(len(result['warnings']), 1)
        self.assertEqual((self.run / 'Lekmod-turn-1.Civ5Save').read_bytes(), self.save.read_bytes())

    def test_missing_turn_rejected(self):
        self.values = {k: v for k, v in self.values.items() if not k.startswith('turn.0.')}
        self.write_ledger()
        with self.assertRaisesRegex(ValueError, 'turn snapshots'):
            runner.verify_evidence(self.run)

    def test_telemetry_failure_rejected(self):
        self.values['error'] = 'no safe observer slot; autoplay refused'
        self.write_ledger()
        with self.assertRaisesRegex(ValueError, 'observer slot'):
            runner.verify_evidence(self.run)

    def test_scenario_mismatch_rejected(self):
        self.manifest['expected_civilizations'] = ['CIVILIZATION_UNREQUESTED']
        runner.write_json(self.run / 'run-manifest.json', self.manifest)
        with self.assertRaisesRegex(ValueError, 'Civilization setup mismatch'):
            runner.verify_evidence(self.run)

    def test_save_must_independently_match_turn(self):
        self.save.write_bytes(header(0))
        with self.assertRaises(runner.EvidencePending):
            runner.verify_evidence(self.run)
        for data in (b'CIV5', b'CIV5' + b'\0' * 4 + struct.pack('<I', 999999)):
            with self.assertRaises(ValueError):
                runner.save_header(data)

    def test_guard_acknowledgement_required(self):
        (self.run / 'runtime.log').write_text('guard not loaded\n')
        child = subprocess.Popen([sys.executable, '-c', 'import time; time.sleep(10)'])
        try:
            with self.assertRaisesRegex(RuntimeError, 'guard acknowledgement'):
                runner.monitor(child, self.run, timeout=2, guard_timeout=0.02, interval=0.01)
        finally:
            runner.stop_owned(child, grace=0.1)

    def test_verified_completion_return_then_child_cleanup(self):
        child = subprocess.Popen([sys.executable, '-c', 'import time; time.sleep(10)'])
        try:
            runner.monitor(child, self.run, timeout=2, interval=0.01)
            self.assertIsNone(child.poll())
            self.assertEqual(runner.stop_owned(child, grace=1), 'sigterm')
            self.assertIsNotNone(child.poll())
        finally:
            runner.stop_owned(child, grace=0.1)

    def test_sigterm_ignoring_child_is_only_owned_target(self):
        other = subprocess.Popen([sys.executable, '-c', 'import time; time.sleep(10)'])
        child = subprocess.Popen([sys.executable, '-u', '-c',
                                 'import signal,time; signal.signal(signal.SIGTERM, signal.SIG_IGN); print("ready",flush=True); time.sleep(10)'],
                                 stdout=subprocess.PIPE, text=True)
        try:
            self.assertEqual(child.stdout.readline().strip(), 'ready')
            self.assertEqual(runner.stop_owned(child, grace=0.05), 'sigkill-after-grace')
            self.assertIsNone(other.poll())
            self.assertEqual(runner.stop_owned(child), 'already-exited')
        finally:
            runner.stop_owned(child, grace=0.1)
            runner.stop_owned(other, grace=0.1)
            child.stdout.close()

    def test_paths_cannot_escape_run(self):
        with self.assertRaisesRegex(RuntimeError, 'escapes'):
            runner.private_path(self.run, self.run.parent / 'original-game')


if __name__ == '__main__':
    unittest.main()
