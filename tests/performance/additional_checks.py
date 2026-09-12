
from pathlib import Path
import subprocess
from source_text import canonical

PREFIX = 'LEKMOD_DLL/CvGameCoreDLL_Expansion2/'
BASE = '2742e9d29bcf184d41d31272b0e5f07f18dfecf1'


def cpp_checks(directory, source, extract, compile_run):
    root = Path(__file__).resolve().parents[2]
    def old(path):
        return subprocess.check_output(['git', 'show', f'{BASE}:{path}'], cwd=root).decode(errors='replace')
    def method(text, signature):

        result = extract(text, signature)
        return result[:result.index('{')].replace('#endif\n', '') + result[result.index('{'):]
    trade = source(PREFIX+'CvTradeClasses.cpp')
    db = source(PREFIX+'CvDatabaseUtility.cpp')
    worker = source(PREFIX+'CvBuilderTaskingAI.cpp')
    lookup = 'BuildTypes CvBuilderTaskingAI::GetBuildTypeFromImprovement(ImprovementTypes eImprovement)\n'
    substitutions = {
        'CACHE_CLEAR': extract(worker, 'void CvBuilderTaskingAI::ClearBuildTypeCache('),
        'CACHE_BUILD': extract(worker, 'void CvBuilderTaskingAI::CacheBuildTypes('),
        'LOOKUP_BASELINE': method(old(PREFIX+'CvBuilderTaskingAI.cpp'), lookup).replace('::GetBuildTypeFromImprovement(', '::BaselineLookup('),
        'LOOKUP_CURRENT': method(worker, lookup),
        'ROUTE_COUNT': extract(trade, 'int CvPlayerTrade::GetNumInternationalTradeRoutesFromCity('),
        'DB_CLEAR': extract(db, 'void CvDatabaseUtility::ClearResults()'),
        'DB_GET': extract(db, 'Database::Results* CvDatabaseUtility::GetResults('),
        'DB_PREPARE': extract(db, 'Database::Results* CvDatabaseUtility::PrepareResults('),
        'DB_BASELINE': extract(old(PREFIX+'CvDatabaseUtility.cpp'), 'int CvDatabaseUtility::MaxRows(').replace('::MaxRows(', '::BaselineMaxRows('),
        'DB_CURRENT': extract(db, 'int CvDatabaseUtility::MaxRows('),
    }
    result = {}
    for name, flags in [('default', ()), ('ubsan', ('-fsanitize=undefined', '-fno-sanitize-recover=all'))]:
        result[name] = compile_run('additional.cpp.in', substitutions, directory, flags)
    assert result['default'] == result['ubsan']
    loader = extract(source(PREFIX+'CvDllDatabaseUtility.cpp'), 'bool CvDllDatabaseUtility::PrefetchGameData(')
    assert loader.index('ClearBuildTypeCache()') < loader.index('PrefetchCollection(')
    assert loader.index('CacheBuildTypes()') > loader.index('PrefetchCollection(GC.getBuildInfo()')
    binding = source(PREFIX+'Lua/CvLuaPlayer.cpp')
    assert 'Method(GetNumInternationalTradeRoutesFromCity);' in binding
    assert 'CvLuaCity::GetInstance(L, 2)' in extract(binding, 'int CvLuaPlayer::lGetNumInternationalTradeRoutesFromCity(')

    assert canonical(extract(binding, 'int CvLuaPlayer::lGetTradeRoutes(')) == canonical(extract(old(PREFIX+'Lua/CvLuaPlayer.cpp'), 'int CvLuaPlayer::lGetTradeRoutes('))

    astar = source(PREFIX+'CvAStar.cpp')
    header = source(PREFIX+'CvAStar.h')
    first = astar.index('void CvAStar::CreateChildren(')
    second = astar.index('void CvAStar::CreateChildren(', first+1)
    neighbors = {
        'X_RANGE': method(header, 'inline int CvAStar::xRange(int iX)\n'),
        'Y_RANGE': method(header, 'inline int CvAStar::yRange(int iY)\n'),
        'IS_VALID': method(header, 'inline bool CvAStar::isValid(int iX, int iY)\n'),
        'PRECOMPUTE': extract(astar, 'void CvAStar::PrecalcNeighbors('),
        'CACHED': extract(astar[first:], 'void CvAStar::CreateChildren('),
        'BASELINE': extract(astar[second:], 'void CvAStar::CreateChildren(').replace('::CreateChildren(', '::CreateChildrenBaseline('),
    }
    result['neighbors'] = compile_run('neighbors.cpp.in', neighbors, directory, ('-fsanitize=undefined', '-fno-sanitize-recover=all'))
    return result


def lua_checks(lua, source):

    root = Path(__file__).resolve().parents[2]
    baseline = subprocess.check_output(['git', 'show', f'{BASE}:LEKMOD/Lua/Civilizations/Lekmod_kilwa.lua'], cwd=root).decode()
    current = source('LEKMOD/Lua/Civilizations/Lekmod_kilwa.lua')
    setup = (Path(__file__).with_name('additional_fixtures.lua')).read_text()
    lua.execute(setup)
    checks = 0
    for native in (False, True):
        for seed in range(1, 101):
            before, old_tables = lua.globals().run_kilwa(baseline, source('LEKMOD/Lua/Utilities/Lekmod_utilities.lua'), seed, native)
            after, new_tables = lua.globals().run_kilwa(current, source('LEKMOD/Lua/Utilities/Lekmod_utilities.lua'), seed, native)
            assert before == after
            assert new_tables == (0 if native else old_tables)
            checks += 1

    panel = source('LEKMOD/Lua/tmp/eui/NotificationPanel/NotificationPanel.lua.ignore')
    lua.eval('function(s) assert(loadstring(s)) end')(panel)
    old_panel = subprocess.check_output(['git', 'show', f'{BASE}:LEKMOD/Lua/tmp/eui/NotificationPanel/NotificationPanel.lua.ignore'], cwd=root).decode()
    def fragment(text, start):
        first = text.index(start)
        return text[first:text.index('instance.Button:SetDisabled(', first)]
    before = fragment(old_panel, 'instance.TheirTradeItems:SetText( table_concat( theirTradeItems ) )')
    after = fragment(panel, 'local theirTradeText = table_concat( theirTradeItems )')
    for size in range(10):
        assert lua.globals().compare_trade_text(size, before, after)
    old_units = subprocess.check_output(['git', 'show', f'{BASE}:LEKMOD/Lua/Lekmod_units.lua'], cwd=root).decode()
    assert lua.globals().compare_unit_filter(old_units, source('LEKMOD/Lua/Lekmod_units.lua'))
    return {'kilwa_callbacks': checks, 'trade_text_cases': 10, 'unit_filter_comparisons': 24}
