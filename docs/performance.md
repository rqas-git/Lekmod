# Performance changes on `perf`

This branch starts at `1bc2ff42`. Its initial batch implemented seven items from the performance worklist; additional trade optimizations and the worker/sight/map batch build on `a90d5bbe`. The performance edits remove repeated work while retaining the existing gameplay rules and calculations. The initial correctness fixes below intentionally repair existing behavior. Subsequent performance batches contain no gameplay-rule fixes. Targeted checks and isolated component benchmarks have passed; full gameplay equivalence and whole-game speedups remain unmeasured.

## Implemented

| Worklist item | Change | Work removed and behavior retained |
| --- | --- | --- |
| 1. Global dummy buildings | Stop collecting a rule after its first eligible city. | One application instruction per eligible player/rule, rather than one per city. The existing application loop still visits every eligible city. Clear/reapply phases, duplicate database rows, and capital-only conditions remain intact. |
| 2. City connection indices | Resolve each city's route-array index once per `UpdateRouteInfo` call. | Removes repeated linear searches inside the city-pair loops. Uses `GetIndexFromCity`, preserving the distinction between the candidate-city list and the route-index list, including team games and missing indices. |
| 3. Water connection eligibility | Compute active-harbor and blockade eligibility once per city for the current update. | Removes repeated building and nearby-unit scans. The cache is discarded when the call returns. Route passes, pair visitation order, inverse-route handling, pass-state updates, and pathfinder calls retain their existing logic. |
| 4. Coastal improvement yields | Read the coastal modifier before checking the coastline. | Skips the coastline scan when the contribution is zero. Uses `!= 0`, so positive and negative modifiers retain their existing calculation. |
| 7. UAE trade-route rewards | Check `IsCombatUnit` before requesting a plot tooltip. | Avoids trade-route scans, formatting, and Lua table allocation for units that cannot receive the reward. Eligible units still use the original tooltip-presence condition, rewards, experience changes, and popup behavior. |
| 8. Policy promotions | Resolve policy, promotion, and combat-class IDs once on first use in the Lua context. | Avoids repeated database iteration and type-name lookups. Preserves rule order, event subscriptions, and per-rule checks of current player/unit state. Adopting a policy still reconciles all applicable rules for existing units. |
| 10. Improvement adjacency metadata | Cache the existing adjacency-presence calculation after database prefetch completes. | Replaces repeated table scans during improvement changes with a boolean read. The original calculation is retained in `ComputeHasAnyAdjacencyYieldBonus`; its indexing and treatment of yield values are unchanged. |

The connection caches contain no state that survives an update. Policy metadata is scoped to the loaded Lua context. The improvement flag is initialized in the entry constructor, reset during entry loading, and populated after the full improvement collection has loaded. Computing it inside each entry's `CacheResults` would use an incomplete global improvement count because the loader appends entries afterward. Database reloads recreate and repopulate the entries.

These edits introduce no saved-game fields, random draws, floating-point setting changes, or changes to pathfinding tie order.

## Deferred suggestions

| Worklist item | Reason it is outside this patch |
| --- | --- |
| 5. A* neighbor precomputation | This was proposed as a separate build experiment. It enables a dormant implementation and adds six pointers per node per pathfinder. Memory use and path equivalence need assessment before adopting it. |
| 6. Mughal holy-city API shortcut | The API resolves stored holy-city coordinates, whereas the original script searches city religion-status flags. Those sources need not be equivalent in edge cases. The original lookup remains in place. |
| 9. Creation-only embark repair | Restricting the callback to the newly created unit can delay repairs to other existing units until their next turn. That changes event behavior. |
| 11. Windows whole-program optimization | This was proposed as a separate compiler/linker experiment. Build compatibility, deterministic behavior, and any performance gain remain to be established independently. |
| 12. Lekmap shuffle | The faster removal algorithm changes which map the same random seed generates. That conflicts with the requirement to preserve outcomes. |

## Correctness fixes and validation

Dummy rules now include player zero and distinguish player events from team events. Consulates awards work for every adopter and each eligible team member. City-connection capacity covers teammate city indices. Installer updates stage replacements before moving existing files, restore them on commit failure, reject unsafe version paths, and reject failed or non-executable self-update downloads. Self-update completion runs on the GUI thread and retains the previous executable for recovery.

Initial validation on 2026-09-12 passed for all seven performance edits. The reproducible suite is in `tests/performance`; its latest output, including the additional trade checks and production/fixture source hashes, is in `docs/performance-validation.json`. The table below records the initial batch; current UAE results are documented in the additional trade section.

| Optimization | Differential coverage | Fixture operations, before → after |
| --- | --- | --- |
| Global dummy buildings | 352 cases, including empty empires, capital conditions and duplicate rules | Building setter calls: 828,840 → 48,456 |
| City connection indices | 720 updates, including team cities, shuffled route indices and missing indices | Index searches: 130,111 → 4,694 |
| Water connection eligibility | Same 720 updates, with harbor/blockade changes between updates | Harbor checks: 53,721 → 10,029; blockade checks: 49,190 → 3,713 |
| Coastal improvement yields | 2,200 cases with zero, positive and negative modifiers | Coast checks: 2,200 → 2,000 |
| UAE trade-route rewards | 144 cases comparing gold, experience and popups | Tooltip calls: 4,104 → 2,736 |
| Policy promotions | 20 fresh Lua contexts, including policy changes and alternate metadata | Rule table reads: 1,008 → 16 |
| Improvement adjacency metadata | 40,000 repeated comparisons across 400 configurations, including cache resets | Query-time metadata getter calls: 69,373,000 → 0; initialization requires 693,730 calls |

The Lua tests execute the actual scripts with Lua 5.1. C++ fixtures compile the actual extracted methods against controlled engine doubles. The comparison keeps the dummy-event and connection-capacity correctness fixes on both sides, isolating the performance edits. Connection tests compare route states, capital flags and pathfinder requests, but the pathfinder itself is a test double. Metadata tests exercise reset/recompute behavior and check loader ordering in source; they do not run a live game database reload. These are controlled operation counts, not elapsed-time benchmarks or predicted whole-game speedups.

A full macOS release build compiled 161 translation units, linked successfully, verified all 358 engine imports against the installed Civilization V executable, and passed six pre-game vtable anchor checks. The build log is in `macos/build/validation-build.log`. All 9 installer regression tests and all 15 existing macOS tests also passed.

Reproduce from the worktree root (Python 3.9+, clang++, and Git with the recorded baseline commit available):

```sh
python3 -m venv /tmp/lekmod-performance-venv
/tmp/lekmod-performance-venv/bin/pip install -r tests/performance/requirements.txt
/tmp/lekmod-performance-venv/bin/python tests/performance/validate.py
python3 -B macos/build.py --release --jobs 4
python3 -B -m unittest discover -s LekmodInstaller/tests -v
python3 -B -m unittest discover -s macos/tests -v
```

No additional production fixes were needed during this validation. The installed game and checked-in binaries have not been replaced. Live gameplay, save/load integration, multiplayer synchronization, a native Windows build, Windows updater execution, and actual turn-time gains remain unvalidated. The deleted `research/perf-audit` folder remains absent.

## Additional trade optimizations after `a90d5bbe`

These changes preserve the committed branch's gameplay behavior. Candidate generation, all three scoring functions, the original tooltip function, and event subscriptions remain unchanged.

- **Rank compact records.** `CvTradeAI::PrioritizeTradeRoutes` sorts candidate indices and scores instead of copying complete routes into its ranking vectors. On the current macOS x86_64 ABI, a ranking record occupies 8 bytes instead of 480. A temporary output vector materializes selected routes before clearing the candidates; this retains valid indices and duplicate outputs. Selected paths are still copied into the result.
- **Select the first minimum for food and production.** Only the first entry of each former stable sort was used. `std::min_element` retains the same first equal minimum in linear time. Gold keeps its original ascending stable sort, and output retains the same two-gold/food/production/remaining-gold order. No scoring calls are removed or reordered.
- **Use a boolean UAE reward query.** `CvPlayerTrade::HasPlotToolTips`, exposed as `Player:HasInternationalTradeRoutePlotToolTip`, answers whether the original tooltip list would be nonempty without localization, city-name resolution, or Lua table construction. Routes involving the player qualify along their path; third-party routes qualify only at the trade unit's current position. It reads current route state each call, so movement, ownership changes and route removal require no cache invalidation. The Lua handler retains the old tooltip call as a fallback for older gameplay libraries. Combat filtering, reward timing, gold, XP and popup behavior are unchanged.

There are no additional object data members, virtual methods, saved-game fields, RNG calls, floating-point changes, or pathfinder changes. New native code uses existing C++03-compatible constructs. The existing correctness fixes in `a90d5bbe` were not altered or expanded.

### Additional validation

`tests/performance/trade.cpp.in` compiles the actual committed and current ranking methods against controlled scoring/game/container doubles. The tests compare the complete ordered output payloads and the order and arguments of every scoring call. They cover empty inputs, negative/zero/extreme/tied scores, shuffled candidates, repeated route coordinates, city-population ties, missing unit metadata, happiness transitions, duplicate production target cities, paths through 128 plots, and lists exceeding the production container's initial 168-route capacity. The container double uses `std::vector`, so these checks do not independently validate Firaxis container internals.

The unchanged production tooltip method serves as the boolean predicate oracle. Fixtures cover null plots, empty route slots, own/incoming/internal/third-party routes, land/sea domains, overlapping paths, trade-unit movement, owner changes, route removal and absent endpoint cities. All Lua UAE fixtures compare gold, experience and popup outputs with both the new native query and the legacy fallback.

| Check | Coverage per C++ configuration | Fixture operations, before → after |
| --- | --- | --- |
| Trade ranking | 720 differential updates; identical outputs and scoring traces | Route-path copies: 779,599 → 71,744; copied plot entries: 27,574,335 → 2,539,745 |
| Tooltip-presence predicate | 175,680 differential queries | Formatted strings: 404,585 → 0 |
| UAE Lua handler | 144 cases, checked with both APIs | Original main tooltip calls: 4,104 → 0; new boolean calls: 2,736; legacy fallback tooltip calls: 2,736 |

The C++ checks passed in the default configuration, with `AUI_ITERATORIZE` plus `AUI_CONSTIFY`, and with UndefinedBehaviorSanitizer plus libc++ checked containers. The UBSan run stops on the first diagnostic. All existing connection, metadata, coastal-yield, dummy-building and policy fixtures also passed. These are operation counts from controlled fixtures, not elapsed-time measurements or whole-game speedups.

AddressSanitizer was attempted but could not complete in this sandbox: both the trade fixture and a trivial standalone ASan program timed out without output; the standalone UBSan control passed. No ASan pass is claimed. The optional `--address-sanitizer` flag retains a smaller ASan run for an environment with a working runtime.

A fresh full macOS Release build compiled and linked successfully, verified 358 engine imports and all six pre-game vtable anchors. Its log is `macos/build/trade-performance-build.log`. Neither the installed game nor the checked-in Windows DLL was replaced. Live gameplay, save/reload integration, Windows execution, multiplayer synchronization and turn-time gains still require validation.

Run the full differential suite with the same command above; add `--address-sanitizer` only when ASan works locally. The recorded trade baseline is `a90d5bbe07a9cbf521190cdcc33304739f5375f2`, independent of the initial batch's `1bc2ff42` baseline.

## Worker, sight, trade-range and map batch

Implemented the first seven candidates in [the follow-up research](../research/performance-next/REPORT.md), continuing on the existing `perf` branch.

| Change | Behavior preserved |
| --- | --- |
| Pass production target cities by const reference | The scorer's calculations and target order stay unchanged, including the optional const/iterator configuration. Removes a vector copy per scoring call. |
| Narrow sight-loop DY bounds inside the existing DX loop | Visits exactly the same valid offsets in the same order, including wrapped duplicates and map-edge null handling. Radius 8 has 217 iterations instead of 289. |
| Cache the sum of promotion visibility modifiers | Retains negative modifiers and null definitions. Invalidates on promotion construction, destruction, `CacheResults`, and completed promotion prefetch. Runtime visibility defines and radius limits are still read/applied on each sight update. Adds no instance fields or serialization state. |
| Reuse trade range per origin/domain during candidate enumeration | Keeps all candidate checks, pathfinder calls, last-node reads, copied paths and the `range * 100 + 99` allowance in order. Standalone path checks receive a fresh cache; failed paths do not trigger a range scan. A range of -1 simply remains uncached. |
| Select worker directives through a stable index heap | Used only for non-logging requests for one result with more than 32 candidates. Weight and insertion index reproduce stable descending order. After 32 failed candidates, sort the remaining index records and consume them in the same order. Logging, smaller lists and requests for multiple results retain full stable sorting. |
| Copy only the meaningful owned-plot prefix | Keeps a snapshot and the first -1 sentinel. Preserves the evaluation boundary even when optional minor-player plots are appended. |
| Use a rank-select tree for dense shuffle lists of at least 1,024 entries | Keeps the input snapshot, output object references and every RNG bound, label and draw count. Small, sparse and nonintegral-length cases use the original removal algorithm. |

Trade range inputs were traced through `CanCreateTradeRoute`, `IsAllowedToTradeWith`, and land/water trade-path callbacks. Those paths read the relevant city/technology/building state without gameplay event callbacks. Cache lifetime is restricted to the synchronous origin enumeration; no cross-turn or persistent city cache was added.

### Validation and measured work

The expanded `tests/performance/validate.py` compiles source-extracted worker selection, sight propagation, metadata aggregation, trade candidate enumeration and path-range checks against controlled engine doubles. The range formula and every trade scoring rule are also checked against the baseline source, allowing only the documented cache plumbing and const-reference changes.

| Differential check | Coverage | Before → after in fixtures |
| --- | --- | --- |
| Worker selection | 5,040 cases, 307,580 path attempts; same target visits, path attempts, selected payloads and sorted logging state | First-reachable selection comparisons: 1,450,453 → 130,482 |
| Owned-plot snapshots | Empty/full/partial vectors with optional appended plots; same visited prefix | Copied entries: 1,001,032 → 501,522 |
| Sight updates | 1,600 cases; 127,483 identical sight events, wrapped and bounded maps, negative modifiers, definition reloads and changed runtime defines | Promotion reads: 31,040 → 1,940; 265,966 range predicates removed |
| Trade enumeration and range boundaries | 618 cases; 9,476 identical candidate records, including failed/null paths and repeated enumerations with changed ranges | Range scans: 8,589 → 1,764 |
| Lua shuffling | 6,096 cases using actual baseline/current functions; threshold boundaries, sparse inputs, false values and shared references | Identical output references, input contents and RNG traces |

The shuffle test additionally forces the actual tree branch on short arrays to exhaust all rank sequences for lengths 0–7. C++ checks pass normally and with UBSan plus checked libc++ containers; the trade-range checks also pass with `AUI_CONSTIFY`. Checked-library comparator calls are recorded separately because heap assertions add diagnostic comparisons. The earlier trade ranking, tooltip, Lua, connection and metadata tests still pass.

Local component benchmarks use controlled path/RNG doubles and median CPU timings. Worker selection with 8,192 directives was about 7.2× faster when an eligible target was found early. With every target unreachable, the fallback reduced the overhead substantially, but that isolated 8,192-candidate case remained about 12% slower than the original sort; at 1,024 candidates it was approximately even. These selection timings omit real pathfinding cost and are not turn-time predictions.

For dense shuffles using midpoint draws, the 1,024-, 2,048- and 8,192-entry cases were roughly 2.5×, 4.3× and 14× faster. Adversarial all-last-element draws are slower with the tree because the original removal is already constant-time for each such draw. Actual random workloads and shipping Lua GC costs require live measurement. Full measurements and source/fixture hashes are in [performance-validation.json](performance-validation.json).

The final macOS Release build linked successfully and verified 358 engine imports and six pre-game vtable anchors (`macos/build/next-performance-build.log`). Production changes use C++03-compatible constructs and do not add virtual methods or object data members. Tests do not cover live save/reload, complete seeded map generation, Windows execution, or multiplayer synchronization. Run those before release; no whole-game speedup is claimed.

Ordered sparse metadata lists, database sizing/index changes and shared worker scoring snapshots remain research candidates. Their lifetimes, duplicate-row semantics or state invalidation need further validation before implementation.

The [follow-up audit](../research/performance-followup/REPORT.md) identifies direct technology ranking, boolean worker mission checks, route-build metadata and map-save hash reuse as next candidates. Its source-extracted prototypes and evidence are separate from the production validation above; those candidates are not yet implemented.

## Minimal additional optimizations

This batch continues from the existing dirty `perf` worktree without changing its earlier optimizations or correctness fixes. It implements the small, source-confirmed parts of the additional research list. It does not enable every suggested experiment or claim whole-game equivalence.

| Change | Scope and behavior retained |
| --- | --- |
| Native Kilwa route count | Adds `Player:GetNumInternationalTradeRoutesFromCity(city)`. Matches the existing helper's outgoing route and **current destination-city owner** checks, across both domains, without building names, yields, tourism, pressure or Lua route tables. Kilwa retains its original callbacks and building updates, with the old helper as a fallback for older DLLs. |
| Static Lua IDs and earlier filtering | Cache Kilwa's trait building ID and the workboat unit ID in their existing Lua contexts. Non-workboat training checks return before fetching the player. No subscriptions or embark-repair timing change. |
| First-build metadata lookup | Cache the first build ID for each improvement, including `NO_IMPROVEMENT`, missing IDs and duplicate matches. Clear before metadata prefetch and rebuild after definitions load. Use the original scan before initialization. No serialized fields or object-layout changes. |
| Prepared sizing statements | `MaxRows` uses the existing per-utility statement cache. It executes on every call and resets afterward: **row counts are not cached**. Changing table contents still changes the result. The existing utility cleanup owns statement lifetime. |
| Reuse display results | EUI's notification panel calculates each paired score/trade string once for its left/right controls. Both controls, text ordering, resource order, truncation and refresh scheduling are retained. |

### Pathfinding and compiler experiments are opt-in

`macos/build.py --release --precompute-neighbors` enables only the existing neighbor-precomputation define. Its helper is made non-const so it builds with the normal, non-const coordinate helpers; no surrounding AUI feature block is enabled. The default pathfinder remains unchanged. Six extra pointers per node per pathfinder increase memory use; a real gameplay/memory comparison is still required.

`macos/build.py --release --lto` enables ThinLTO for compilation and linking. Both flags may be combined. Experimental objects, logs and libraries are isolated under `macos/build/experimental-{lto,neighbors,lto-neighbors}` and do not replace the normal build output. Existing arithmetic flags and host audits remain enabled. These are build experiments, not automatically installed upgrades.

### Research items deliberately not added

- **Old trade-building sparse scan:** the earlier research identified a scan in the inactive, pre-`TRADE_REFACTOR` implementation. The enabled implementation already reads city-level extra-yield values. No redundant trade-building cache was retained; the active worker build lookup is cached instead.
- **More UI refresh coalescing:** the inspected EUI panel already defers repeated refreshes. This patch only removes duplicate calculations. No tutorial/context lifecycle changes or replacement EUI package were introduced.
- **Additional search-local caches and shared worker scoring:** the confirmed range cache already exists in the previous batch. No further dynamic cache is added without a demonstrated redundant computation and complete invalidation model.
- **Profile-guided optimization:** no representative training profile is available, so no profile-derived binary is claimed. ThinLTO is independently testable without introducing a profiling runtime or changing the installed game.
- **Broad runtime, pathfinder, SQL-index and threading changes:** remain deferred for gameplay/determinism validation. Presentation mods and user graphics settings are unchanged.

The native count query and shared UI strings also cover the proposed reductions in temporary allocations and duplicated callback inputs; no generic pooling framework was needed.

### Validation

The expanded `tests/performance/validate.py` compares 17,400 build lookups, 144,000 native route counts, and 146,016 neighbor callback traces on bounded/wrapped grids (including one-tile dimensions, changing validity and extra children). Database fixtures verify statement reuse with changing values, missing tables and utility resets. Lua 5.1 executes 200 old/new Kilwa callback scenarios with native and fallback APIs, 24 unit-filter comparisons, and ten actual old/new UI formatting fragments; the entire edited panel also parses successfully.

These are source-extracted methods with game/database doubles, not live game or SQLite-engine integration tests. C++ comparisons also run under UndefinedBehaviorSanitizer. The existing performance suite remains enabled. The normal macOS Release build and combined ThinLTO/neighbors build both compiled, linked and passed engine-import and six pre-game vtable-anchor audits. All 47 macOS regression tests passed, including experimental output/flag isolation.

Run `python3 -B tests/performance/validate.py --lua-python-path /tmp/lekmod-perf-audit-python --output docs/performance-validation.json` on this host to reproduce the recorded suite. Windows builds, live saves, multiplayer synchronization, whole-turn timings and the experimental memory tradeoff remain unvalidated. No installed game files were replaced.
