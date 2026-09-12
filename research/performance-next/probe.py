"""Research models, not game integration tests. Run with --lua-python-path PATH."""
import argparse
import hashlib
import heapq
import itertools
import json
from pathlib import Path
import random
import subprocess
import sys

parser = argparse.ArgumentParser()
parser.add_argument('--lua-python-path', required=True)
args = parser.parse_args()
sys.path.insert(0, args.lua_python_path)
from lupa.lua51 import LuaRuntime

root = Path(__file__).resolve().parents[2]
sources = ['Lekmap/HBMapmakerUtilities.lua',
           'LEKMOD_DLL/CvGameCoreDLL_Expansion2/CvGameCoreUtils.h',
           'LEKMOD_DLL/CvGameCoreDLL_Expansion2/CvBuilderTaskingAI.cpp',
           'LEKMOD_DLL/CvGameCoreDLL_Expansion2/CvTradeClasses.cpp']
result = {'source_sha256': {p: hashlib.sha256((root / p).read_bytes()).hexdigest() for p in sources}}

# Original DX-major order, including axis/sign boundaries.
for radius in range(65):
    original = [(x, y) for x in range(-radius, radius + 1)
                for y in range(-radius, radius + 1)
                if (abs(x) + abs(y) if (x >= 0) == (y >= 0)
                    else max(abs(x), abs(y))) <= radius]
    bounded = [(x, y) for x in range(-radius, radius + 1)
               for y in range(max(-radius, -radius-x), min(radius, radius-x)+1)]
    assert original == bounded
result['hex_order'] = {'radii_checked': 65, 'radius_8_square': 289, 'radius_8_bounded': 217}

# Stable descending score order represented by heap keys; model all reachability
# patterns in randomized lists. Compare attempted candidates.
rng = random.Random(274)
cases = 0
for n in list(range(9)) + [32, 100, 1000]:
    for trial in range(100):
        weights = [rng.randrange(-3, 4) for _ in range(n)]
        reachable = [rng.choice([False, True]) for _ in range(n)]
        sorted_ids = sorted(range(n), key=lambda i: -weights[i])
        heap = [(-w, i) for i, w in enumerate(weights)]
        heapq.heapify(heap)
        heap_ids = [heapq.heappop(heap)[1] for _ in range(n)]
        assert sorted_ids == heap_ids
        def attempted(ids):
            trace = []
            for i in ids:
                trace.append(i)
                if reachable[i]:
                    break
            return trace
        assert attempted(sorted_ids) == attempted(heap_ids)
        cases += 1
result['worker_order_model_cases'] = cases

lua = LuaRuntime(unpack_returned_tuples=True)
# Keep the research comparison reproducible after the production implementation
# has changed; integration tests compare the new shipping code separately.
shuffle_baseline = 'a90d5bbe07a9cbf521190cdcc33304739f5375f2'
source_bytes = subprocess.check_output(['git', 'show', f'{shuffle_baseline}:{sources[0]}'], cwd=root)
source = source_bytes.decode()
result['shuffle_baseline'] = shuffle_baseline
result['source_sha256'][sources[0]] = hashlib.sha256(source_bytes).hexdigest()
start = source.index('function GetShuffledCopyOfTable(incoming_table)')
end = source.index('\nend', start) + len('\nend')
lua.execute(source[start:end])
lua.execute('''
function treeShuffle(input)
  local n = table.maxn(input)
  local tree, low, out = {}, {}, {}
  for i=1,n do
    low[i] = i % 2 == 1 and 1 or 2 * low[i/2]
    tree[i] = low[i]
  end
  local top = 1
  while top * 2 <= n do top = top * 2 end
  for i=1,n do
    local rank = 1 + Map.Rand(n-i+1, "Shuffling table entry - Lua")
    local index, bit = 0, top
    while bit >= 1 do
      local nextIndex = index + bit
      if nextIndex <= n and tree[nextIndex] < rank then
        rank = rank - tree[nextIndex]
        index = nextIndex
      end
      bit = bit / 2
    end
    index = index + 1
    out[i] = input[index]
    while index <= n do
      tree[index] = tree[index] - 1
      index = index + low[index]
    end
  end
  return out
end
function compare(ranks)
  local n = #ranks
  local input = {}
  for i=1,n do input[i] = {id=i} end
  local function run(fn)
    local calls = 0
    Map = {Rand=function(bound, label)
      calls = calls + 1
      assert(bound == n-calls+1)
      assert(label == "Shuffling table entry - Lua")
      return ranks[calls]
    end}
    local output = fn(input)
    assert(calls == n)
    for i=1,n do assert(input[i].id == i) end
    return output
  end
  local a, b = run(GetShuffledCopyOfTable), run(treeShuffle)
  assert(#a == #b)
  for i=1,n do assert(a[i] == b[i]) end
end
function benchmark(n, repeats, fn)
  local input = {}
  for i=1,n do input[i] = i end
  Map = {Rand=function(bound, label) return math.floor(bound/2) end}
  collectgarbage('collect')
  local before = os.clock()
  for r=1,repeats do fn(input) end
  return (os.clock()-before)/repeats
end
''')
shuffle_cases = 0
for n in range(8):
    for ranks in itertools.product(*(range(bound) for bound in range(n, 0, -1))):
        lua.globals().compare(lua.table_from(ranks))
        shuffle_cases += 1
for n in [8, 32, 100, 1000, 10000]:
    for trial in range(20):
        ranks = [rng.randrange(bound) for bound in range(n, 0, -1)]
        lua.globals().compare(lua.table_from(ranks))
        shuffle_cases += 1
result['shuffle_equivalence_cases'] = shuffle_cases
result['shuffle_benchmark'] = []
for n in [32, 128, 512, 2048, 8192, 32768]:
    repeats = max(3, 32768 // n)
    old = lua.globals().benchmark(n, repeats, lua.globals().GetShuffledCopyOfTable)
    new = lua.globals().benchmark(n, repeats, lua.globals().treeShuffle)
    result['shuffle_benchmark'].append({'length': n, 'repeats': repeats,
        'original_seconds': old, 'tree_seconds': new, 'original_over_tree': old/new})
print(json.dumps(result, indent=2))
