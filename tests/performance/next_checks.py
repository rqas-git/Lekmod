"""Source-extracted checks for the second performance implementation batch."""
import itertools
from pathlib import Path
import random
import statistics
import subprocess

ROOT = Path(__file__).resolve().parents[2]
PREFIX = 'LEKMOD_DLL/CvGameCoreDLL_Expansion2/'
BASE = 'a90d5bbe07a9cbf521190cdcc33304739f5375f2'


def baseline(path):
    return subprocess.check_output(['git', 'show', f'{BASE}:{path}'], cwd=ROOT).decode(errors='replace')


def normalize_trade_section(text):
    """Undo only the enumerator cache plumbing and read-only parameter change."""
    text = text.replace('''		// Range inputs stay fixed during this read-only origin enumeration.
		int aiCachedRanges[NUM_DOMAIN_TYPES];
		std::fill(aiCachedRanges, aiCachedRanges + NUM_DOMAIN_TYPES, -1);
''', '')
    text = text.replace('IsValidTradeRoutePathWithCachedRange(pOriginCity, pDestCity, eDomain, aiCachedRanges[eDomain])',
                        'IsValidTradeRoutePath(pOriginCity, pDestCity, eDomain)')
    text = text.replace('const std::vector<CvCity*>& aTargetCityList)', 'std::vector<CvCity*> aTargetCityList)')
    text = text.replace('const std::vector<const CvCity*>& aTargetCityList)', 'std::vector<const CvCity*>& aTargetCityList)')
    return text.replace('std::vector<const CvCity*>::const_iterator it = aTargetCityList.begin()',
                        'std::vector<const CvCity*>::iterator it = aTargetCityList.begin()')


def cpp_checks(directory, source, extract, compile_run):
    worker, old_worker = source(PREFIX+'CvBuilderTaskingAI.cpp'), baseline(PREFIX+'CvBuilderTaskingAI.cpp')
    start = worker.index('\t// A stable heap produces')
    current = worker[start:worker.index('\n\tif(m_bLogging)', start)]
    start = old_worker.index('\tm_aDirectives.StableSortItems();', old_worker.index('bool CvBuilderTaskingAI::EvaluateBuilder('))
    old = old_worker[start:old_worker.index('\n\tif(m_bLogging)', start)]
    start = worker.index('\t\t// Keep a snapshot,')
    snapshot = worker[start:worker.index('\n\t}\n', start)]
    entry = extract(worker, 'struct BuilderDirectiveHeapEntry').replace('const\n\t\t{', 'const\n\t\t{\n\t\t\t++heapComparisons;')
    assert '++heapComparisons;' in entry
    worker_sub = {'HEAP_ENTRY': entry, 'BASELINE': old, 'CURRENT': current, 'SNAPSHOT': snapshot}

    plot, old_plot = source(PREFIX+'CvPlot.cpp'), baseline(PREFIX+'CvPlot.cpp')
    promotion = source(PREFIX+'CvPromotionClasses.cpp')
    for sig in ('CvPromotionEntry::~CvPromotionEntry(void)', 'bool CvPromotionEntry::CacheResults('):
        assert 'InvalidateVisibilityChangeCache();' in extract(promotion, sig)
    ctor = promotion[promotion.index('CvPromotionEntry::CvPromotionEntry()'):promotion.index('CvPromotionEntry::~CvPromotionEntry')]
    assert 'InvalidateVisibilityChangeCache();' in ctor
    prefetch = source(PREFIX+'CvDllDatabaseUtility.cpp')
    assert 'PrefetchCollection(GC.getPromotionInfo(), "UnitPromotions");\n\tCvPromotionEntry::InvalidateVisibilityChangeCache();' in prefetch
    def sight_method(text):
        start = text.index('void CvPlot::updateSeeFromSight(')
        return text[start:text.index('\n\n//', start)]
    sight_sub = {
        'BASELINE': sight_method(old_plot).replace('::updateSeeFromSight(', '::baseline('),
        'CURRENT': sight_method(plot),
        'INVALIDATE': extract(promotion, 'void CvPromotionEntry::InvalidateVisibilityChangeCache('),
        'AGGREGATE': extract(promotion, 'int CvPromotionEntry::GetTotalVisibilityChange('),
        'RANGE_CHECK': extract(source(PREFIX+'CvGameCoreUtils.h'), 'inline CvPlot* plotXYWithRangeCheck(').replace('{', '{ ++rangeChecks;', 1),
    }

    trade, old_trade = source(PREFIX+'CvTradeClasses.cpp'), baseline(PREFIX+'CvTradeClasses.cpp')
    def path_methods(text, current):
        start = text.index('#ifdef AUI_CONSTIFY\nbool CvGameTrade::IsValidTradeRoutePath(')
        return text[start:text.index('\n//\t---', start)]
    range_sub = {
        'BASELINE_PATH': path_methods(old_trade, False).replace('::IsValidTradeRoutePath', '::IsValidTradeRoutePathBaseline'),
        'CURRENT_PATH': path_methods(trade, True),
        'BASELINE_CANDIDATES': extract(old_trade, 'void CvTradeAI::GetAvailableTR(').replace('::GetAvailableTR(', '::GetAvailableTRBaseline(').replace('->IsValidTradeRoutePath(', '->IsValidTradeRoutePathBaseline('),
        'CURRENT_CANDIDATES': extract(trade, 'void CvTradeAI::GetAvailableTR('),
    }
    # The range formula itself is still byte-for-byte identical.
    assert extract(old_trade, 'int CvPlayerTrade::GetTradeRouteRange (') == extract(trade, 'int CvPlayerTrade::GetTradeRouteRange (')
    results = {}
    modes = [('default', ()), ('ubsan_checked', ('-fsanitize=undefined', '-fno-sanitize-recover=all',
              '-D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_DEBUG'))]
    for name, subs in [('worker', worker_sub), ('sight', sight_sub), ('trade_range', range_sub)]:
        results[name] = {}
        variants = modes + ([('const', ('-DAUI_CONSTIFY',))] if name == 'trade_range' else [])
        if name == 'worker':
            variants += [('benchmark', ('-DPERF_BENCHMARK',))]
        for mode, flags in variants:
            results[name][mode] = compile_run(name+'.cpp.in', subs, directory, flags)
        # Checked libc++ also invokes comparators for its own heap assertions.
        comparable = lambda value: {k:v for k,v in value.items() if k not in ('selection_comparisons', 'timings')}
        assert all(comparable(value) == comparable(results[name]['default']) for value in results[name].values()), results[name]
    return results


def shuffle_checks(lua):
    old = baseline('Lekmap/HBMapmakerUtilities.lua')
    current = (ROOT/'Lekmap/HBMapmakerUtilities.lua').read_text()
    def function(text):
        start = text.index('function GetShuffledCopyOfTable(incoming_table)')
        return text[start:text.index('\nend', start)+4]
    lua.execute(function(old).replace('GetShuffledCopyOfTable', 'shuffleOld', 1))
    lua.execute(function(current).replace('GetShuffledCopyOfTable', 'shuffleNew', 1))
    # Exercise the actual tree code exhaustively on short arrays too.
    lua.execute(function(current).replace('GetShuffledCopyOfTable', 'shuffleForced', 1).replace('len >= 1024', 'len >= 0'))
    lua.execute('''
    function compareShuffle(ranks, sparse)
      local n = #ranks
      local input, shared, snapshot = {}, {}, {}
      for i=1,n do
        if i % 3 == 0 then input[i] = false
        elseif i % 3 == 1 then input[i] = shared
        else input[i] = {id=i} end
      end
      if sparse and n > 2 then input[2] = nil end
      for i=1,n do snapshot[i] = input[i] end
      local function run(fn)
        local calls, trace = 0, {}
        Map = {Rand=function(bound, label)
          calls = calls + 1
          trace[#trace+1] = tostring(bound)..':'..label
          return ranks[calls] % math.max(1,bound)
        end}
        local output = fn(input)
        for i=1,n do assert(input[i] == snapshot[i]) end
        return output, table.concat(trace, '|'), calls
      end
      local old, oldTrace, oldCalls = run(shuffleOld)
      for _,fn in ipairs({shuffleNew,shuffleForced}) do
        local current, currentTrace, currentCalls = run(fn)
        assert(#old == #current and oldTrace == currentTrace and oldCalls == currentCalls)
        for i=1,#old do assert(old[i] == current[i]) end
      end
    end
    function timeShuffle(fn,n,repeats,pattern)
      local input = {}; for i=1,n do input[i]=i end
      Map = {Rand=function(bound,label)
        if pattern == 'first' then return 0 end
        if pattern == 'last' then return bound-1 end
        return math.floor(bound/2)
      end}
      collectgarbage('collect')
      local start = os.clock()
      for i=1,repeats do fn(input) end
      return (os.clock()-start)/repeats
    end
    ''')
    funcs, rng, cases = lua.globals(), random.Random(424), 0
    for n in range(8):
        for ranks in itertools.product(*(range(i) for i in range(n,0,-1))):
            funcs.compareShuffle(lua.table_from(ranks), False)
            cases += 1
    for n in (8,32,1023,1024,1025,2048,8192):
        for trial in range(25):
            ranks = [rng.randrange(i) for i in range(n,0,-1)]
            funcs.compareShuffle(lua.table_from(ranks), False)
            cases += 1
        funcs.compareShuffle(lua.table_from([0]*n), True)
        cases += 1
    timings = []
    for n in (32,128,1023,1024,2048,8192):
        for pattern in ('first','middle','last'):
            repeats = max(2, 8192//n)
            before = statistics.median(funcs.timeShuffle(funcs.shuffleOld,n,repeats,pattern) for _ in range(3))
            after = statistics.median(funcs.timeShuffle(funcs.shuffleNew,n,repeats,pattern) for _ in range(3))
            timings.append({'length':n, 'pattern':pattern, 'before_seconds':before, 'after_seconds':after})
    return {'cases':cases, 'timings':timings}
