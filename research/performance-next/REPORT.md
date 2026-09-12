# Further performance opportunities

Research date: 2026-09-12. Baseline: `perf` at `a90d5bbe`, including the pending trade-ranking and UAE predicate changes. This pass adds research and executable prototypes only; it does not change gameplay code. Rankings below reflect source evidence and implementation risk, not measured shares of turn time.

Implementation follow-up: candidates 1–7 are now implemented in the same `perf` worktree. Worker selection adds a fallback after 32 rejected candidates, and shuffling uses a 1,024-entry threshold. See [implementation and validation](../../docs/performance.md#worker-sight-trade-range-and-map-batch) for current results and remaining limits. The research evidence below records the earlier prototype measurements.

## Recommended sequence

| Priority | Opportunity | Scope | Confidence / next gate |
|---|---|---|---|
| 1 | Pass production target cities by const reference | Tiny allocation/copy reduction during trade scoring | High; compile both signatures and test scoring |
| 2 | Keep DX-major order while narrowing hex-loop bounds | Repeated sight/danger neighborhood traversal | Offset equivalence tested; integrate selected loops and compare event traces |
| 3 | Cache promotion visibility sum after metadata loading | Terrain/feature sight rebuilds | High after checking reload/reset lifetime |
| 4 | Reuse trade range within one candidate enumeration | Tech/building scans per origin/destination/domain | Verify callback purity and compare path-call traces |
| 5 | Use a stable heap for worker directive selection | Avoid fully sorting candidates when requesting one | Ordering model tested; integrate real C++ fixture and benchmark rejection rates |
| 6 | Use rank-select shuffle for large lists | Map generation, quadratic to N log N | Actual Lua function compared; choose threshold on game runtime |
| 7 | Avoid copying the owned-plot vector per worker | Worker candidate enumeration | Check ownership mutation/reentrancy before introducing a view |
| 8 | Build ordered sparse metadata lists | Trade building bonuses and build lookup | Preserve ID order, first-match rules and dynamic eligibility |
| 9 | Reduce repeated metadata sizing SQL | Startup / database caching | Instrument query counts and establish immutable loading phase |
| 10 | Share worker plot scoring within a safe snapshot | Larger worker-AI redesign | Profile first; requires robust invalidation and serial decisions |

## Evidence and behavior constraints

### 1. Production target-vector copies

`LEKMOD_DLL/CvGameCoreDLL_Expansion2/CvTradeClasses.cpp:5963` passes `std::vector<CvCity*> aTargetCityList` by value. `PrioritizeTradeRoutes` calls it for every candidate when the target list is nonempty, including non-production candidates which immediately return zero. The body only reads the list. Change the active declaration and definition to a const reference; preserve candidate order and all scoring arithmetic.

For C calls and T targets this eliminates C vector copies and C×T pointer copies, normally including C allocations for nonempty lists. This is a structural estimate, not a recorded runtime counter. The existing ranking fixture mocks this parameter by reference already, so its previously reported route-copy savings do **not** include this additional opportunity. The dormant constify/iterator variants need compatible declarations and const iterators if updated together.

### 2. Order-preserving hex bounds

`CvGameCoreUtils.h:257` rejects offsets outside the hex radius. `CvPlot.cpp:1940` uses square DX-major loops for sight propagation, as do other neighborhood consumers. For nonnegative R, retain the outer DX loop and use:

```cpp
for (int dx = -R; dx <= R; ++dx)
    for (int dy = std::max(-R, -R-dx); dy <= std::min(R, R-dx); ++dy)
        /* original body */;
```

The accepted offsets and their order are identical. The probe checks every radius 0–64. Square iteration count is `(2R+1)^2`; bounded count is `3R(R+1)+1`. At R=8 this is 289→217, a 24.9% reduction in loop iterations. It does not reduce the number of valid plots processed. Wrapping receives identical coordinate arguments, retaining even duplicate wrapped plots. Integration must confirm that skipped out-of-range iterations had no other side effects and preserve null/map-edge handling.

Do not simply enable `AUI_HEXSPACE_DX_LOOPS`: its existing alternative is DY-major, which changes visitation order. Use the new bounds only at verified sites, rather than a blanket flag change.

### 3. Promotion visibility aggregation

`CvPlot::updateSeeFromSight` scans all promotion entries and sums their visibility changes on each call, before applying the recon minimum and radius cap. Terrain/feature/plot mutations call this around sight changes. Promotion definitions are a candidate for aggregation once after metadata loading (see promotion prefetch in `CvDllDatabaseUtility.cpp`), with reset on reload. Continue reading runtime visibility defines and applying the existing max/min at the call site.

Preserve negative promotion values and null entries. Early exit on reaching the radius cap is invalid because later negative values can lower the sum. Do not add serialized state or change exposed object layouts merely for this cache. Benefit depends on actual sight-rebuild frequency.

### 4. Call-local trade calculations

`CvTradeAI::GetAvailableTR` at `CvTradeClasses.cpp:5671` enumerates origins, destinations and domains. `IsValidTradeRoutePath` at 472 generates a path and then calls `GetTradeRouteRange` at 4698, which scans all technologies and building classes. Range depends on the origin and domain, not the destination.

A lazy cache scoped to one enumeration can reduce range scans from successful-path pairs to at most the encountered origin/domain combinations. Preserve path generation, its order, the `range * 100 + 99` comparison, integer rounding, positive-only building modifiers and civilization-specific class mappings. Verify that the enumeration's callbacks cannot mutate range inputs before applying the cache. Avoid persistent caches until every invalidation source is known. Candidate generation already checks the path outside the connection-type loop; that optimization is already present.

Similarly, `GetTradeConnectionYourBuildingValueTimes100` at 3153 scans building classes for route yield bonuses. An ordered sparse definition list is less invasive than caching dynamic yields across decisions. A local score context might reuse origin/domain building contributions, but must preserve recipient/origin distinctions and formula stages.

### 5. Stable worker candidate selection

`CvBuilderTaskingAI::EvaluateBuilder` at 932 stable-sorts all directives, then checks reachability in sorted order until it fills the requested output. Both repository call sites (`CvHomelandAI.cpp:5928`, `CvUnit.cpp:2275`) request one directive. `CvWeightedVector.h:44` orders descending weight; stable sorting preserves insertion order on ties.

An index heap keyed by descending weight and ascending insertion index can produce exactly that sequence: O(D) heap construction plus O(K log D) to inspect K candidates instead of O(D log D) sorting all D. The probe compares full order and attempted candidates in 1,200 cases, including ties, negative weights and unreachable candidates. It is an algorithm model, not an extracted C++ test. Worst-case rejection still visits all candidates and can erase the benefit; benchmark several rejection rates before landing.

Keep existing threshold/zero-weight semantics and path-query order. A single maximum is insufficient because the best candidate can be unreachable. Logging consumes the fully sorted list, so retain the original sort when logging or recreate the identical complete log order. Do not alter the global weighted-vector implementation for unrelated callers.

`FindTurnsAway` at 2588 currently uses raw plot distance in the active configuration. The expensive confirmed path checks occur after scoring; claiming pathfinding for every gathered directive would overstate this opportunity.

### 6. Seed-identical large-list shuffling

`Lekmap/HBMapmakerUtilities.lua:891` repeatedly chooses a random rank and removes that element from a shrinking array. Array removal shifts subsequent entries, as documented in the [Lua 5.1 manual](https://www.lua.org/manual/5.1/manual.html#pdf-table.remove). Repeated removal produces quadratic element movement. Consumers include feature-generator plot lists and resource-placement lists, as well as small player/team lists.

A Fenwick tree tracks which original indices remain. Selecting the same random rank among live indices preserves the exact permutation, object references, input table and random call count/bounds/label. Unlike swap-and-pop, it preserves seed behavior. The prototype uses Lua 5.1 arithmetic without bitwise extensions.

Compared the **actual extracted original Lua function** with the prototype: all rank sequences for lengths 0–7, plus randomized larger cases through length 10,000, totaling 6,014 comparisons. The contract is dense ordinary arrays as documented by the original; sparse/metatable-driven inputs require separate treatment.

Isolated Lua 5.1 CPU timings on this host, with deterministic midpoint draws and both implementations using the same stub RNG:

| Entries | Original | Tree | Original / tree |
|---:|---:|---:|---:|
| 32 | 4.85 µs | 8.60 µs | 0.56× |
| 128 | 27.2 µs | 38.1 µs | 0.71× |
| 512 | 246 µs | 162 µs | 1.51× |
| 2,048 | 3.21 ms | 0.741 ms | 4.33× |
| 8,192 | 49.6 ms | 3.19 ms | 15.5× |
| 32,768 | 775 ms | 15.1 ms | 51.4× |

These are exploratory single-run averages, not a game benchmark. Keep the original below a measured threshold; 512–1,024 is a starting range for further testing. Record actual list-size distributions, compare several RNG patterns and repeat on the shipping Lua runtime. Tree allocation/GC and real RNG overhead can change the crossover. A full seeded map comparison remains required before shipping.

### 7–8. Worker copies and sparse metadata

`EvaluateBuilder` at 794 copies the player's complete plot vector, then stops at the first -1 sentinel at 828. A const view in the normal branch or copying only the meaningful prefix avoids unused capacity work. Preserve the own-plot branch, optional minor-player extension, and iteration order; ensure called code cannot invalidate the source view.

`GetBuildTypeFromImprovement` at 3103 scans all builds and returns the first match. A metadata lookup can preserve that exact first ID, including NO_IMPROVEMENT handling. This is a smaller opportunity with a single call site. `GetRepairBuild` is already cached in `m_eRepairBuild` during initialization: do not count it as new work.

Building and specialist scans can use ordered lists of statically eligible definitions, while still evaluating dynamic ownership/eligibility. Preserve class-to-civilization mapping and ID order. Existing specialist selection sometimes returns the first eligible building; changing it to a mathematically better selection changes gameplay. Some scoring loops multiply weights for every matching building, so replacing them with a boolean existence test also changes results.

### 9. Database sizing and indexing

`CvDatabaseUtility::MaxRows` at 325 executes `SELECT max(rowid)` repeatedly from array initialization helpers. Reusing a prepared sizing query is a conservative initial experiment; caching results requires an immutable load phase and reset when tables change. Measure query preparation/execution counts before prioritizing this over turn-time work. The existing `PopulateArrayByValue` already caches its main prepared statement; 2D arrays already use contiguous storage in the active branch.

Indexes on frequently filtered owner columns are a separate startup opportunity, but require actual database plans and duplicate audits. `PopulateArrayByValue` at 268 assigns each returned row into an array, so duplicate keys have last-row-wins behavior. SQLite explicitly leaves result order undefined without ORDER BY ([SELECT documentation](https://www.sqlite.org/lang_select.html)); indexes can change traversal plans ([query planner](https://www.sqlite.org/queryplanner.html)). Do not add blanket indexes until loaded arrays are compared byte-for-byte and duplicate semantics are resolved. No live database plans were collected in this pass.

### 10. Larger worker-AI reuse

Many workers independently revisit the same owned plots and build definitions. A shared snapshot could retain unit-independent yield/scoring inputs, leaving unit movement and eligibility local. This potentially saves more than sorting, but requires tracing every scoring dependency and invalidating after builds, ownership changes, city assignments, policies and other mutations. Apply decisions serially in the existing order. Do not parallelize gameplay callbacks or introduce cross-turn caches on the strength of a synthetic test.

## Measurement and implementation gates

First instrument a dedicated performance build with aggregate counters/timers for trade candidate generation, range scans, worker gathering/scoring/selection/path checks, sight rebuilds and map-shuffle list sizes. Existing AI_PERF macros compile away under FINAL_RELEASE (`CvGameCoreDLLPCH.h:87`), so a runtime setting alone cannot expose those timings. Avoid per-candidate formatting/I/O in timed loops.

Capture representative early/late saves and map sizes, including many workers and cities. Report phase times and total turn/load/map time separately. For each landed optimization, compare selected actions, path-query traces, RNG draws, event order and resulting game state; replay multiplayer determinism separately. No whole-game speedup or multiplayer equivalence is established by this research.

Reproduce the research models:

```sh
python3 -B research/performance-next/probe.py --lua-python-path /tmp/lekmod-perf-audit-python
```

The dependency is Lupa with its Lua 5.1 runtime. `evidence.json` records source hashes and results. Existing production changes and their validation artifacts were left intact.
