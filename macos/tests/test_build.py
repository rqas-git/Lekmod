"""Keep experimental flags and artifacts out of the default gameplay build."""
from pathlib import Path
import os
import subprocess
import sys
import tempfile
import time
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import build


class BuildTests(unittest.TestCase):
    def test_catalog_edit_invalidates_a_cached_object(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            core, here, output = root / 'core', root / 'macos', root / 'build'
            core.mkdir()
            (here / 'include').mkdir(parents=True)
            (core / 'CvGameCoreDLL_Expansion2.vcxproj').write_text(
                '<Project><ItemGroup><ClCompile Include="catalog-user.cpp"/></ItemGroup></Project>')
            (core / 'catalog-user.cpp').write_text('#include "CvGlobalDefines.inc"')
            (here / 'include/native.hpp').write_text('// header')
            catalog = core / 'CvGlobalDefines.inc'
            catalog.write_text('CV_GLOBAL_DEFINE(INT, FIRST, 1)')
            timestamp = time.time()

            def compile_object(command, **kwargs):
                target = Path(command[command.index('-o') + 1])
                target.write_bytes(b'compiled object')
                os.utime(target, (timestamp + 100, timestamp + 100))
                return subprocess.CompletedProcess(command, 0, '', '')

            with patch.object(build, 'CORE', core), patch.object(build, 'HERE', here), \
                    patch.object(build, 'BUILD', output), \
                    patch.object(sys, 'argv', ['build.py', '--release', '--source', 'catalog-user.cpp']), \
                    patch.object(build.subprocess, 'run', side_effect=compile_object) as compiler:
                self.assertEqual(build.main(), 0)
                self.assertEqual(build.main(), 0)
                self.assertEqual(compiler.call_count, 1)
                catalog.write_text('CV_GLOBAL_DEFINE(INT, FIRST, 2)')
                os.utime(catalog, (timestamp + 200, timestamp + 200))
                self.assertEqual(build.main(), 0)
                self.assertEqual(compiler.call_count, 2)

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
