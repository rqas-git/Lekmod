"""Keep experimental flags and artifacts out of the default gameplay build."""
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import build


class BuildTests(unittest.TestCase):
    def test_check_flags_and_output_isolation(self):
        for lto, neighbors in ((False, False), (True, False), (False, True), (True, True)):
            with self.subTest(lto=lto, neighbors=neighbors), tempfile.TemporaryDirectory() as temp:
                base = Path(temp) / 'build'
                args = ['build.py', '--release', '--check']
                names = []
                if lto:
                    args += ['--lto']
                    names.append('lto')
                if neighbors:
                    args += ['--precompute-neighbors']
                    names.append('neighbors')
                with patch.object(build, 'BUILD', base), patch.object(sys, 'argv', args), \
                     patch.object(build.subprocess, 'run', return_value=subprocess.CompletedProcess([], 0)) as run:
                    self.assertEqual(build.main(), 0)
                command = run.call_args.args[0]
                self.assertEqual('-flto=thin' in command, lto)
                self.assertEqual('-DAUI_ASTAR_PRECALCULATE_NEIGHBORS_ON_INITIALIZE' in command, neighbors)
                self.assertIn('-ffp-contract=off', command)
                self.assertNotIn('-ffast-math', command)
                expected = base / ('experimental-' + '-'.join(names)) if names else base
                self.assertTrue(expected.is_dir())
                self.assertFalse((base / 'libCvGameCoreDLL_Expansion2_DLL.dylib').exists())


if __name__ == '__main__':
    unittest.main()
