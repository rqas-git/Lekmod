import os
from pathlib import Path
import shlex
import shutil
import subprocess
import tempfile
import unittest

from reference import ROOT


class LuaBindingTests(unittest.TestCase):
    def test_inherited_const_void_and_argument_wrappers(self):
        compiler = os.environ.get('CXX') or shutil.which('c++')
        if not compiler or os.name == 'nt':
            self.skipTest('Requires a C++ compiler and Lua 5.1 development library')
        local = Path(os.environ.get('LUA51', '/nonexistent')).parent
        if (local / 'liblua.a').is_file():
            flags = ['-I' + str(local), str(local / 'liblua.a'), '-lm']
        else:
            try:
                flags = shlex.split(subprocess.check_output(
                    ['pkg-config', '--cflags', '--libs', 'lua5.1'], text=True, stderr=subprocess.DEVNULL))
            except (OSError, subprocess.CalledProcessError):
                self.skipTest('Install the Lua 5.1 development library or set LUA51')
        core = ROOT / 'LEKMOD_DLL/CvGameCoreDLL_Expansion2'
        header = (core / 'Lua/CvLuaPlayer.h').read_bytes().decode('latin1')
        start = header.index('#if defined(LEKMOD_MACOS)')
        adapter = header[start:header.index('#endif', start) + len('#endif')]
        with tempfile.TemporaryDirectory() as directory:
            directory = Path(directory)
            (directory / 'player_adapter.h').write_text(adapter)
            executable = directory / 'lua-bindings'
            command = [compiler, '-std=c++14', '-DLEKMOD_MACOS', '-I' + str(directory),
                       '-I' + str(core / 'CvGameCoreDLLUtil/include'), str(ROOT / 'tests/lua_bindings.cpp'),
                       *flags, '-o', str(executable)]
            build = subprocess.run(command, capture_output=True, text=True)
            self.assertEqual(build.returncode, 0, build.stdout + build.stderr)
            result = subprocess.run([str(executable)], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
