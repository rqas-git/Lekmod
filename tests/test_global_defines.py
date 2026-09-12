import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import unittest

from reference import ROOT

CORE = ROOT / 'LEKMOD_DLL/CvGameCoreDLL_Expansion2'
BASELINE = 'b3dc1df5'
FIRST = 'OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL'
LAST = 'MIN_UNIT_GOLDEN_AGE_TURNS'


class GlobalDefinesTests(unittest.TestCase):
    def test_catalog_preserves_layout_defaults_getters_and_cache_order(self):
        compiler = os.environ.get('CXX') or shutil.which('c++')
        if not compiler or os.name == 'nt':
            self.skipTest('Requires a C++ compiler; Windows also builds the actual DLL in CI')
        old = {name: subprocess.check_output(
            ['git', 'show', f'{BASELINE}:LEKMOD_DLL/CvGameCoreDLL_Expansion2/{name}'], cwd=ROOT).decode('latin1')
            for name in ('CvGlobals.h', 'CvGlobals.cpp')}
        patterns = {
            'DECL': ('CvGlobals.h', r'\b(int|float) m_([if])([A-Z][A-Z0-9_]*);', 3),
            'DEF': ('CvGlobals.h', r'inline (int|float) get([A-Z][A-Z0-9_]*)\(\)\s*\{\s*return m_([if])\2;\s*\}', 2),
            'INIT': ('CvGlobals.cpp', r'\bm_([if])([A-Z][A-Z0-9_]*)\(([^()\n]+)\),', 2),
            'CACHE': ('CvGlobals.cpp', r'\bm_([if])([A-Z][A-Z0-9_]*) = getDefine(?:INT|FLOAT)\("\2"\);', 2),
        }
        before, after, fields = {}, {}, []
        for phase, (name, pattern, key) in patterns.items():
            matches = list(re.finditer(pattern, old[name]))
            start = next(i for i, m in enumerate(matches) if m[key] == FIRST)
            end = next(i for i, m in enumerate(matches) if m[key] == LAST)
            before[phase] = old[name][matches[start].start():matches[end].end()]
            if phase == 'DECL':
                fields = [(m[1], m[2], m[3]) for m in matches[start:end+1]]
            current = (CORE / name).read_bytes().decode('latin1')
            match = re.search(r'^#define CV_GLOBAL_DEFINE\([^\n]*GD_##kind##_' + phase +
                              r'[^\n]*\n#include "CvGlobalDefines.inc"\n#undef CV_GLOBAL_DEFINE', current, re.M)
            self.assertIsNotNone(match, phase)
            after[phase] = match[0]
        self.assertEqual(len(fields), 672)
        macros = '\n'.join(line for line in (CORE / '_Defines.h').read_text().splitlines()
                           if re.match(r'#define GD_(INT|FLOAT)_(DECL|DEF|INIT|CACHE)\(', line))

        def facade(name, blocks):


            return f'''struct {name} {{
{blocks['DECL']}
    int sentinel;
    std::vector<std::string> lookups;
    {name}() :
{blocks['INIT']}
        sentinel(0) {{}}
{blocks['DEF']}
    int getDefineINT(const char* name) {{ lookups.push_back(std::string("int:") + name); return -int(lookups.size()); }}
    float getDefineFLOAT(const char* name) {{ lookups.push_back(std::string("float:") + name); return float(lookups.size()) + 0.125f; }}
    void cache() {{
{blocks['CACHE']}
    }}
}};
'''

        compare = '\n'.join(f'assert(before.get{name}() == after.get{name}());' for _, _, name in fields)

        layout = '\n'.join(f'assert((char*)&before.m_{prefix}{name} - (char*)&before == '
                           f'(char*)&after.m_{prefix}{name} - (char*)&after);' for _, prefix, name in fields)
        source = ('#include <cassert>\n#include <string>\n#include <vector>\n' + macros + '\n' +
                  facade('Before', before) + facade('After', after) + f'''
int main() {{
    Before before;
    After after;
    assert(sizeof(before) == sizeof(after));
    {layout}
    {compare}
    before.cache(); after.cache();
    assert(before.lookups == after.lookups);
    {compare}
}}
''')
        with tempfile.TemporaryDirectory() as directory:
            directory = Path(directory)
            fixture, executable = directory / 'globals.cpp', directory / 'globals'
            fixture.write_text(source)
            result = subprocess.run([compiler, '-std=c++98', '-I' + str(CORE), str(fixture), '-o', str(executable)],
                                    capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run([str(executable)], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
