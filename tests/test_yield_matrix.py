import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import unittest

from reference import ROOT, original

CORE = ROOT / 'LEKMOD_DLL/CvGameCoreDLL_Expansion2'


class YieldMatrixTests(unittest.TestCase):
    def test_actual_loader_with_sqlite_fixtures_and_unchanged_queries(self):
        compiler = os.environ.get('CXX') or shutil.which('c++')
        if not compiler or os.name == 'nt':
            self.skipTest('Requires a C++ compiler and SQLite development library')
        cases = []
        for name in ('CvTraitClasses.cpp', 'CvBuildingClasses.cpp', 'CvPolicyClasses.cpp', 'CvBeliefClasses.cpp'):
            before = original('LEKMOD_DLL/CvGameCoreDLL_Expansion2/' + name).decode('latin1')
            after = (CORE / name).read_bytes().decode('latin1')
            for match in re.finditer(r'SetYieldMatrix\(\w+, "(\w+)", "(\w+)",\s*'
                                     r'((?:"[^"\n]*"\s*)+),\s*\w+\);', after):
                dimension, key = match[1], match[2]
                query = ''.join(re.findall(r'"([^"\n]*)"', match[3]))
                block = before[before.index('std::string strKey("' + key + '");'):]
                old_query = re.search(r'PrepareResults\(strKey,\s*((?:"[^"\n]*"\s*)+)\);', block)
                self.assertIsNotNone(old_query, key)
                self.assertEqual(query, ''.join(re.findall(r'"([^"\n]*)"', old_query[1])), key)
                column = re.search(r'\bON\s+' + dimension + r'\.Type\s*=\s*([\w.]+)', query, re.I)[1].split('.')[-1]
                owner = re.search(r'\bWHERE\s+([\w.]+)\s*=', query, re.I)[1].split('.')[-1]
                value = re.split(r'\s+FROM\s+', query, flags=re.I)[0].rsplit(',', 1)[1].strip().split('.')[-1]
                fixture = (f'CREATE TABLE {key} ({column} TEXT, YieldType TEXT, {owner} TEXT, {value} INTEGER); '
                           f"INSERT INTO {key} VALUES ('row0','y1','ownerA',7),('row0','y1','ownerA',9),"
                           "('row4','y3','ownerA',-4),('row1','y0','ownerB',11),"
                           "('missing','y1','ownerA',99),('row0','missing','ownerA',99)")
                cases.append((dimension, key, query, fixture))
        self.assertEqual(len(cases), 20)
        source = (CORE / 'CvDatabaseUtility.cpp').read_bytes().decode('latin1')
        implementations = []
        for signature in ('void CvDatabaseUtility::ClearResults()',
                          'Database::Results* CvDatabaseUtility::GetResults(',
                          'Database::Results* CvDatabaseUtility::GetOrPrepareResults(',
                          'Database::Results* CvDatabaseUtility::PrepareResults(',
                          'int CvDatabaseUtility::MaxRows(',
                          'bool CvDatabaseUtility::Initialize2DArray(int**& ppArray, const char*',
                          'void CvDatabaseUtility::SetYieldMatrix('):
            start = source.index(signature)
            implementations.append(source[start:source.index('//------------------------------------------------------------------------------', start)])
        with tempfile.TemporaryDirectory() as folder:
            folder = Path(folder)
            (folder / 'production_matrix.h').write_text('\n'.join(implementations))
            (folder / 'matrix_cases.h').write_text(
                'struct MatrixCase { const char *dimension, *key, *query, *fixture; };\nMatrixCase cases[] = {\n' +
                ',\n'.join('{' + ','.join(json.dumps(value) for value in case) + '}' for case in cases) + '\n};\n')
            executable = folder / 'matrix-test'
            subprocess.run([compiler, '-std=c++98', '-I' + str(folder), str(ROOT / 'tests/yield_matrix.cpp'),
                            '-lsqlite3', '-o', str(executable)], check=True, capture_output=True, text=True)
            result = subprocess.run([str(executable)], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
