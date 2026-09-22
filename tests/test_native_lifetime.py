from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

from reference import ROOT


class NativeLifetimeTests(unittest.TestCase):
    def test_script_data_keeps_its_owner_until_lua_copies_the_string(self):
        compiler = shutil.which('clang++') or shutil.which('c++')
        if not compiler:
            self.skipTest('A C++ compiler is required')
        source = (ROOT / 'LEKMOD_DLL/CvGameCoreDLL_Expansion2/Lua/CvLuaUnit.cpp').read_text()
        start = source.index('int CvLuaUnit::lGetScriptData(')
        method = source[start:source.index('\n}', start) + 2]
        cpp = '''
#include <cassert>
#include <string>
struct lua_State {};
std::string result;
void lua_pushstring(lua_State*, const char* value) { result = value; }
struct CvUnit { std::string data; std::string getScriptData() { return data; } } unit;
struct CvLuaUnit {
    static CvUnit* GetInstance(lua_State*) { return &unit; }
    static int lGetScriptData(lua_State*);
};
''' + method + '''
int main() {
    lua_State lua;
    for (int length : {0, 8, 256, 65536}) {
        unit.data = std::string(length, 'x');
        CvLuaUnit::lGetScriptData(&lua);
        assert(result == unit.data);
    }
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory)
            (path / 'native.cpp').write_text(cpp)
            warnings = ['-Werror=dangling-gsl'] if 'clang' in Path(compiler).name else []
            build = subprocess.run([compiler, '-std=c++14', '-Wall', *warnings,
                                    str(path / 'native.cpp'), '-o', str(path / 'native')],
                                   capture_output=True, text=True)
            self.assertEqual(build.returncode, 0, build.stderr)
            run = subprocess.run([str(path / 'native')], capture_output=True, text=True, timeout=20)
            self.assertEqual(run.returncode, 0, run.stderr)
