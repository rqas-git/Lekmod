#!/usr/bin/env python3






import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import tempfile
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from source_text import canonical
from next_checks import cpp_checks, shuffle_checks, normalize_trade_section
import additional_checks

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
CORE = ROOT / 'LEKMOD_DLL/CvGameCoreDLL_Expansion2'
BASE = '1bc2ff423a51a0b5740acf77c7b6c31d1d245c8b'
TRADE_BASE = 'a90d5bbe07a9cbf521190cdcc33304739f5375f2'


def source(path, baseline=False):
    if baseline:
        return subprocess.check_output(['git', 'show', f'{BASE}:{path}'], cwd=ROOT).decode(errors='replace')
    return (ROOT / path).read_text(errors='replace')


def extract(text, signature):
    start = text.index(signature)
    opening = text.index('{', start)
    end, depth = opening + 1, 1
    while depth:
        depth += (text[end] == '{') - (text[end] == '}')
        end += 1
    return text[start:end]


def instrument(method, counter):
    return method.replace('{', '{ ++' + counter + ';', 1)


def compile_run(template, substitutions, directory, flags=()):
    text = (HERE / template).read_text()
    for key, value in substitutions.items():
        text = text.replace('@' + key + '@', value)
    file = directory / template.removesuffix('.in')
    file.write_text(text)
    executable = file.with_suffix('')
    subprocess.run(['clang++', '-std=c++14', '-O2', '-Wall', '-Wextra', *flags,
                    str(file), '-o', str(executable)], check=True, timeout=60)
    return json.loads(subprocess.check_output([str(executable)], text=True, timeout=60))


def trade_checks(directory, address_sanitizer=False):
    path = 'LEKMOD_DLL/CvGameCoreDLL_Expansion2/CvTradeClasses.cpp'
    current = source(path)
    baseline = subprocess.check_output(['git', 'show', f'{TRADE_BASE}:{path}'], cwd=ROOT).decode(errors='replace')
    old_candidates = extract(baseline, 'void CvTradeAI::GetAvailableTR(')
    new_candidates = extract(current, 'void CvTradeAI::GetAvailableTR(')
    assert canonical(old_candidates) == canonical(normalize_trade_section(new_candidates))
    def ranking(text):
        start = text.index('struct TRSortElement')
        end = text.index('bool CvTradeAI::ChooseTradeUnitTargetPlot(', start)
        end = text.rfind('#ifdef AUI_CONSTIFY', start, end)
        return text[start:end]
    old = ranking(baseline).replace('TRSortElement', 'BaselineTRSortElement').replace('SortTR', 'BaselineSortTR')
    old = old.replace('::PrioritizeTradeRoutes(', '::PrioritizeTradeRoutesBaseline(')
    start = baseline.index('#ifdef AUI_CONSTIFY\nstd::vector<CvString> CvPlayerTrade::GetPlotToolTips(')
    end = baseline.index('#ifdef AUI_CONSTIFY\nstd::vector<CvString> CvPlayerTrade::GetPlotMouseoverToolTips(', start)
    tooltip = baseline[start:end]
    current_start = current.index('#ifdef AUI_CONSTIFY\nstd::vector<CvString> CvPlayerTrade::GetPlotToolTips(')
    current_end = current.index('#ifdef AUI_CONSTIFY\nstd::vector<CvString> CvPlayerTrade::GetPlotMouseoverToolTips(', current_start)
    assert canonical(tooltip) == canonical(current[current_start:current_end])
    start = baseline.index('{', baseline.index('bool CvGameTrade::IsTradeRouteIndexEmpty(int iIndex)'))
    empty = 'bool CvGameTrade::IsTradeRouteIndexEmpty(int iIndex) const\n' + baseline[start:baseline.index('\n}\n', start)+3]
    substitutions = {
        'BASELINE_RANKING': old, 'CURRENT_RANKING': ranking(current),
        'IS_EMPTY': empty, 'TOOLTIP': tooltip,
        'PREDICATE': extract(current, 'bool CvPlayerTrade::HasPlotToolTips('),
    }
    results = {'baseline': TRADE_BASE}
    modes = [
            ('default', ()),
            ('iterator_const', ('-DAUI_ITERATORIZE', '-DAUI_CONSTIFY')),
            ('ubsan_checked', ('-fsanitize=undefined', '-fno-sanitize-recover=all',
                               '-D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_DEBUG')),
    ]
    if address_sanitizer:
        modes.append(('asan', ('-fsanitize=address,undefined', '-fno-sanitize-recover=all',
                              '-fno-omit-frame-pointer', '-DTRADE_SANITIZER_FIXTURES')))
    for name, flags in modes:
        results[name] = compile_run('trade.cpp.in', substitutions, directory, flags)
    assert results['default'] == results['iterator_const'] == results['ubsan_checked']
    cpp, header = (source('LEKMOD_DLL/CvGameCoreDLL_Expansion2/Lua/CvLuaPlayer.' + ext) for ext in ('cpp', 'h'))
    assert 'Method(HasInternationalTradeRoutePlotToolTip);' in cpp
    assert 'static int lHasInternationalTradeRoutePlotToolTip(lua_State* L);' in header
    binding = extract(cpp, 'int CvLuaPlayer::lHasInternationalTradeRoutePlotToolTip(')
    assert 'CvLuaPlot::GetInstance(L, 2, false)' in binding
    assert 'lua_pushboolean(L, pkPlayer->GetTrade()->HasPlotToolTips(pPlot))' in binding
    return results


def connections(directory):
    path = 'LEKMOD_DLL/CvGameCoreDLL_Expansion2/CvCityConnections.cpp'
    current, baseline = source(path), source(path, True)
    old_method = extract(baseline, 'void CvCityConnections::UpdateRouteInfo(')

    old_growth = extract(old_method, 'if(vpCities.size() > m_uiRouteInfosDimension)')
    new_growth = extract(current, 'if(m_aiCityPlotIDs.size() > m_uiRouteInfosDimension)')
    old_method = old_method.replace(old_growth, new_growth).replace('::UpdateRouteInfo(', '::UpdateRouteInfoBaseline(')
    return compile_run('connections.cpp.in', {
        'BASELINE': old_method,
        'CURRENT': extract(current, 'void CvCityConnections::UpdateRouteInfo('),
        'INDEX': instrument(extract(current, 'uint CvCityConnections::GetIndexFromCity('), 'counts.indices'),
    }, directory)


def metadata(directory):
    prefix = 'LEKMOD_DLL/CvGameCoreDLL_Expansion2/'
    current, baseline = source(prefix+'CvImprovementClasses.cpp'), source(prefix+'CvImprovementClasses.cpp', True)
    old = extract(baseline, 'bool CvImprovementEntry::HasAnyAdjacencyYieldBonus() const')
    compute = extract(current, 'bool CvImprovementEntry::ComputeHasAnyAdjacencyYieldBonus() const')
    assert canonical(old.replace('HasAnyAdjacencyYieldBonus', 'ComputeHasAnyAdjacencyYieldBonus')) == canonical(compute)
    predicates = old.replace('HasAnyAdjacencyYieldBonus', 'HasAnyAdjacencyYieldBonusBaseline')
    for signature in ('void CvImprovementEntry::CacheAdjacencyYieldBonus()',
                      'bool CvImprovementEntry::HasAnyAdjacencyYieldBonus() const',
                      'bool CvImprovementEntry::ComputeHasAnyAdjacencyYieldBonus() const'):
        predicates += '\n' + extract(current, signature)
    accessors = '\n'.join(instrument(extract(current, f'int CvImprovementEntry::{name}('), 'reads')
        for name in ('GetImprovementAdjacentBonus', 'GetImprovementAdjacentBonusCivilization',
                     'GetImprovementAdjacentBonusCivilizationNoAmount', 'GetImprovementAdjacentAmount',
                     'GetImprovementAdjacentCivilizationAmount'))
    prefetch = source(prefix+'CvDllDatabaseUtility.cpp')
    collection = source(prefix+'CvDllDatabaseUtility.h')
    cache_loop = extract(prefetch, 'for(int iImprovement = 0; iImprovement < GC.getNumImprovementInfos(); iImprovement++)')
    assert prefetch.index('PrefetchCollection(GC.getImprovementInfo(), "Improvements")') < prefetch.index(cache_loop)
    assert collection.index('pkItem->CacheResults(kResults, kUtility)') < collection.index('kCollection.push_back(pkItem)')
    assert 'delete pkItem;' in collection and 'kCollection.clear();' in collection
    assert 'm_bHasAnyAdjacencyYieldBonus(false)' in current
    reset = 'm_bHasAnyAdjacencyYieldBonus = false;'
    assert reset in extract(current, 'bool CvImprovementEntry::CacheResults(')
    plot, old_plot = source(prefix+'CvPlot.cpp'), source(prefix+'CvPlot.cpp', True)
    old_yield = extract(old_plot[old_plot.index('int CvPlot::calculateImprovementYieldChange('):], 'if(isCoastalLand())')
    new_yield = 'const int iCoastalYield = pImprovement->GetCoastalLandYieldChange(eYield);\n'
    assert new_yield.strip() in plot
    new_yield += extract(plot, 'if(iCoastalYield != 0 && isCoastalLand())')
    return compile_run('metadata.cpp.in', {
        'ACCESSORS': accessors, 'PREDICATES': predicates, 'RESET': reset,
        'PREFETCH': cache_loop, 'COAST_BASELINE': old_yield, 'COAST_CURRENT': new_yield,
        'COAST_METHOD': instrument(extract(plot, 'bool CvPlot::isCoastalLand(int iMinWaterSize) const'), 'coastCalls'),
    }, directory)


def lua_checks(lua_path):
    if lua_path:
        sys.path.insert(0, str(lua_path))
    from lupa.lua51 import LuaRuntime
    lua = LuaRuntime(unpack_returned_tuples=True)
    lua.execute((HERE/'lua_fixtures.lua').read_text())
    funcs = lua.globals()
    result = {'runtime': lua.eval('_VERSION')}
    dummy = source('LEKMOD/Lua/Lekmod_global_dummies.lua')
    optimized_line = 'break'
    assert dummy.count(optimized_line) == 1
    dummy_before = dummy.replace(optimized_line, '')
    cases = old_writes = new_writes = 0
    for count in (0,1,2,5,20,64):
        for capital in {1,count}:
            for flags in range(16):
                for duplicate in (False,True):
                    old, before = funcs.run_dummy(dummy_before,count,capital,flags,duplicate)
                    now, after = funcs.run_dummy(dummy,count,capital,flags,duplicate)
                    assert old == now
                    cases+=1; old_writes+=before; new_writes+=after
    assert new_writes < old_writes
    result['dummy'] = {'cases': cases, 'building_setter_calls': [old_writes,new_writes]}
    current, baseline = (source('LEKMOD/Lua/Civilizations/Lekmod_uae.lua', b) for b in (False,True))
    cases = before_calls = after_calls = fallback_calls = predicate_calls = 0
    for count in (0,1,2,8,32,128):
        for mask in range(12):
            for active in (True,False):
                old,before,_ = funcs.run_uae(baseline,count,mask,active,False)
                now,after,predicates = funcs.run_uae(current,count,mask,active,True)
                fallback,fallback_reads,_ = funcs.run_uae(current,count,mask,active,False)
                assert old==now==fallback
                cases+=1; before_calls+=before; after_calls+=after
                fallback_calls+=fallback_reads; predicate_calls+=predicates
    assert after_calls < before_calls
    result['uae'] = {'cases': cases, 'tooltip_calls': [before_calls,after_calls],
                     'predicate_calls': predicate_calls, 'legacy_fallback_tooltip_calls': fallback_calls}
    current, baseline = (source('LEKMOD/Lua/Lekmod_policies.lua', b) for b in (False,True))
    cases = before_calls = after_calls = 0
    for count in (0,1,2,10,50):
        for reordered in (False,True):
            for alternate in (False,True):
                old,before = funcs.run_policies(baseline,count,reordered,alternate)
                now,after = funcs.run_policies(current,count,reordered,alternate)
                assert old==now and after==(1 if count else 0)
                cases+=1; before_calls+=before; after_calls+=after
    result['policies'] = {'contexts': cases, 'rule_table_reads': [before_calls,after_calls]}
    result['shuffle'] = shuffle_checks(lua)
    result['additional'] = additional_checks.lua_checks(lua, source)
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--lua-python-path', type=Path)
    parser.add_argument('--address-sanitizer', action='store_true',
                        help='Also run ASan fixtures; requires a working local ASan runtime')
    parser.add_argument('--output', type=Path, default=ROOT/'macos/build/performance-validation.json')
    args = parser.parse_args()
    result = {'baseline': BASE}
    with tempfile.TemporaryDirectory(prefix='lekmod-performance-') as temp:
        result['connections'] = connections(Path(temp))
        result['metadata_and_coast'] = metadata(Path(temp))
        result['trade'] = trade_checks(Path(temp), args.address_sanitizer)
        result['next_batch'] = cpp_checks(Path(temp), source, extract, compile_run)
        result['additional'] = additional_checks.cpp_checks(Path(temp), source, extract, compile_run)
    result['lua'] = lua_checks(args.lua_python_path)

    paths = [
        'LEKMOD/Lua/Civilizations/Lekmod_uae.lua', 'LEKMOD/Lua/Lekmod_global_dummies.lua',
        'LEKMOD/Lua/Lekmod_policies.lua', 'Lekmap/HBMapmakerUtilities.lua',
        'LEKMOD/Lua/Civilizations/Lekmod_kilwa.lua', 'LEKMOD/Lua/Lekmod_units.lua',
        'LEKMOD/Lua/tmp/eui/NotificationPanel/NotificationPanel.lua.ignore', 'macos/build.py',
    ] + ['LEKMOD_DLL/CvGameCoreDLL_Expansion2/' + name for name in (
        'CvCityConnections.cpp', 'CvDllDatabaseUtility.cpp', 'CvDllDatabaseUtility.h',
        'CvDatabaseUtility.cpp', 'CvAStar.cpp', 'CvAStar.h', 'CvBuilderTaskingAI.h',
        'CvImprovementClasses.cpp', 'CvImprovementClasses.h', 'CvPlot.cpp', 'CvGameCoreUtils.h',
        'CvPromotionClasses.cpp', 'CvPromotionClasses.h', 'CvBuilderTaskingAI.cpp',
        'CvTradeClasses.cpp', 'CvTradeClasses.h', 'Lua/CvLuaPlayer.cpp', 'Lua/CvLuaPlayer.h')]

    paths += ['LEKMOD_DLL/CvGameCoreDLL_Expansion2/' + name for name in (
        'CvDatabaseUtility.h', 'CvBeliefClasses.cpp', 'CvBuildingClasses.cpp',
        'CvInfos.cpp', 'CvMinorCivAI.cpp', 'CvPolicyClasses.cpp', 'CvTechClasses.cpp',
        'CvTraitClasses.cpp', 'CvUnitClasses.cpp', 'CvGlobals.cpp', 'CvGlobals.h',
        'CvGlobalDefines.inc', '_Defines.h')]
    paths += [str(p.relative_to(ROOT)) for p in sorted((ROOT/'Lekmap').glob('*.lua'))]
    paths += ['LEKMOD/ui_manifest.json', 'LekmodInstaller/ui_assets.py',
              'LekmodInstaller/installer.py', 'LekmodInstaller/ui_manager.py',
              'LekmodInstaller/google_drive_api.py', 'tools/package_lekmod.py',
              'macos/package_assets.py', 'macos/install.py', 'macos/integrity.py']
    result['source_sha256'] = {p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths}
    fixtures = sorted(set(HERE.iterdir()) | set((ROOT/'tests').iterdir()) |
                      set((ROOT/'macos/tests').iterdir()) | set((ROOT/'LekmodInstaller/tests').iterdir()))
    result['fixture_sha256'] = {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest()
                               for p in fixtures if p.suffix in ('.py', '.lua', '.in', '.cpp')}
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(result,indent=2))


if __name__ == '__main__':
    main()
