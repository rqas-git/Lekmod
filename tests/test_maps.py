import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

from reference import BASELINE, ROOT, original


class MapTests(unittest.TestCase):
    def test_regional_maps_match_original_rivers_starts_sizes_and_fractals(self):
        self._compare_scenarios('regional_map_scenarios.lua', 468)

    def test_seeded_output_and_random_calls_match_original_generators(self):
        self._compare_scenarios('map_scenarios.lua', 288)

    def test_complete_menu_metadata_matches_original_and_remains_independent(self):
        self._compare_scenarios('map_metadata_scenarios.lua', 12)

    def _compare_scenarios(self, script, expected_count):
        lua = os.environ.get('LUA51') or shutil.which('lua5.1')
        if not lua:
            self.skipTest('Set LUA51 to a Lua 5.1 interpreter or install lua5.1')
        with tempfile.TemporaryDirectory() as directory:
            names = subprocess.check_output(
                ['git', 'ls-tree', '-r', '--name-only', BASELINE, 'Lekmap'], cwd=ROOT, text=True)
            for name in names.splitlines():
                if name.endswith('.lua'):
                    (Path(directory) / Path(name).name).write_bytes(original(name))
            def run(folder):
                result = subprocess.run([lua, str(ROOT / 'tests' / script), str(folder)],
                                        capture_output=True, text=True)
                self.assertEqual(result.returncode, 0, result.stderr)
                return result.stdout.splitlines()
            expected = run(directory)
            self.assertEqual(len(expected), expected_count)
            self.assertEqual(expected, run(ROOT / 'Lekmap'))
