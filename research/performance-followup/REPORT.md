# Further optimization without gameplay changes

2026-09-12. Audited the existing `perf` worktree at `2742e9d2`, including its uncommitted, validated gameplay-performance edits. This pass adds research and source-extracted prototypes only. Production files were not edited.

The worktree received concurrent implementation changes during this audit. In particular, `MaxRows` prepared-query reuse is now present and is treated as existing work below. The prototype results validate the recorded ranking/mission source, not those concurrent changes or the entire current worktree.

The next small batch should target **technology rank calculation, boolean worker mission checks, and route-build metadata lookups**. A larger candidate is **save-time type-hash reuse**. These remove work while retaining the original decisions; none requires changing AI heuristics, update frequency, RNG consumption, or pathfinding rules.

## Prioritized findings

| Candidate | Work removed | Behavior constraint | Evidence status |
| --- | --- | --- | --- |
| Direct technology rank | Temporary vector, sorting and allocations | Count higher technology totals plus earlier equal-score players; keep the exact float division | Source-extracted prototype passed 128,000 bitwise float comparisons |
| Boolean worker mission query | Counting the remaining units after a matching mission is found | Preserve mission type and exact `distance == range`, including the worker's own-mission exemption at the caller | Source-extracted prototype passed 153,600 queries |
| Route-build metadata lookup | Repeated scans over every build definition | Preserve first build ID, raw minimum repair time, null handling and reload lifetime | Source-audited; needs integration fixtures |
| Map-save type-hash reuse | Repeated string length/CRC calculations for the same metadata types | Write the same hash bytes and return the same presence result; keep save format and object layouts | Source-audited; needs stream equivalence and timing |
| Annex candidate ranking | Sorting all city productions to choose the first puppet in a prefix | Preserve cutoff rounding, stable ties, and resistance blocking | Source-audited; lower expected impact |
| Worker mission index within one evaluation | Repeated scans for negative mission queries | Build an index from a verified fixed mission snapshot; retain own-mission exception | Larger follow-up to the boolean query; lifetime audit needed |
| Prepared metadata-sizing query | Re-preparing `SELECT max(rowid)` | Re-execute for current data; do not cache a stale row count | Already added concurrently; not a new candidate or validated by this pass |

### 1. Technology rank: direct computation

`CvTechAI.cpp:293`, `CvTechAI::GetTechRatio`, gathers living major players, stable-sorts by technologies known, finds the current player's rank and divides by `livingPlayers - 1`. Callers include espionage (`CvEspionageClasses.cpp:5172`) and voting (`CvVotingClasses.cpp:10310`). This is a small allocation reduction, not a claimed dominant turn-time cost.

The prototype retains the same player/team technology reads, stores the bounded player list in a stack array, then counts the players that precede the current player in the stable order. Ties use original insertion order, not a new tie policy. It retains the original `int / (float)int` expression. Missing-player and fewer-than-two-player behavior are included even though the original asserts on those cases in diagnostic builds.

**Measured:** 128,000 cases, including tied scores, shared teams, dead players, empty lists and missing current players. Results match bit-for-bit, and technology-read counts match. Allocations in the fixture fell from **894,656 to 0**. Normal and UBSan x86_64 runs agree. These counts include baseline vector growth; they are not save-game or turn-time measurements.

### 2. Worker mission presence

`CvBuilderTaskingAI::ShouldBuilderConsiderPlot` at 2600 uses `AI_plotTargetMissionAIs(...) > 0`. The counter at `CvPlayerAI.cpp:665` scans every unit, retrieves its mission plot/type, computes distance and counts all matches. Its getters do not issue missions or consume RNG.

Keep the existing counting API for callers needing a count. Add a separate boolean query for this caller that returns on the first match. Retain `distance == iRange`; replacing that with `<=` changes eligibility. Retain the caller's exemption when the evaluated worker already has that exact mission plot.

The active compiler configuration includes this call: `AUI_WORKER_FIX_SHOULD_BUILDER_CONSIDER_PLOT_EXISTING_BUILD_MISSIONS_SHIFT` is off, verified using the actual release include/define flags.

**Measured:** 153,600 queries across empty and populated unit lists, missing mission plots, multiple mission types, duplicates and negative/zero/positive ranges. The boolean matches `count > 0` in every case. Unit mission-plot reads fell from **8,473,600 to 4,480,282**, about **47%**. Negative-result queries still require the full scan.

A later step could index occupied mission plots once per `EvaluateBuilder` call, reducing negative-query work too. That requires checking the complete evaluation call graph for mission changes and reentrancy. Keep this separate from the simpler boolean optimization; do not maintain a cross-turn index without explicit invalidation coverage.

### 3. Route-build metadata

The same static relationships are rediscovered in several places:

- `CvUnit::canBuildRoute`, `CvUnit.cpp:12228`, scans all builds to find route builds before checking current unit permissions and technologies.
- `CvUnit::getBuildType`, immediately below, scans builds for the first build matching the current route mission's selected route.
- `CvUnit::GetBestBuildRoute`, `CvUnit.cpp:24526`, scans builds in ID order, evaluates current build eligibility, and keeps the first highest-value route.
- `CvPlot::getBuildTime`, `CvPlot.cpp:2672`, scans matching route builds to clamp repair time.

Build an ordered list of route-build IDs, a first-build ID per route, and a raw minimum build time per route after build metadata loads. Keep eligibility checks dynamic and in the same order. The repair minimum currently includes matching definitions irrespective of whether their technologies are researched; filtering the cache by current tech would change repair time.

`LEKMOD_NO_INSTANT_REPAIR_ON_ROUTE` is active. Preserve **`WasRoutePillaged()`**, which the code distinguishes from current pillage state, and the existing improvement-pillage condition. Apply the cached minimum at the original point in `getBuildTime`, before subsequent modifiers. Preserve negative metadata values and distinguish an absent matching definition from a zero minimum.

This is a concrete extension of the earlier sparse-metadata proposal. Required tests: duplicate route builds, equal values, null entries, unresearched builds, trait-granted build permissions, route missions, feature/improvement transitions, pillage timing and metadata reload. Rebuild/reset the metadata structures without adding serialized state or changing exposed object layouts.

### 4. Autosave cost: reuse existing type hashes

`CvInfosSerializationHelper.cpp:293`, `WriteHashed`, recalculates `FString::Hash(pkInfo->GetType())` each time. `FStringA.inl:997` computes string length and CRC for that call. `CvMap::Write` at `CvMap.cpp:1612` serially writes every plot. `CvPlot::write` at 11605 writes feature/resource/improvement hashes and a revealed-improvement hash for each team. Many plots therefore repeatedly hash the same small set of metadata strings.

A map-write-scoped typed lookup can reuse hashes computed by the existing engine hash implementation. This gives a narrow initial lifetime and avoids a generic pointer cache or extra fields in `CvBaseInfo`. Keep the original fallback for standalone plot serialization. Do not replace the hash algorithm or serialize the cache.

Preserve both the hash and the presence result. A nonempty type could have a hash of zero: the original writes zero **and returns true**. Treating `hash == 0` as missing would change subsequent serialized fields. Preserve null and empty-type handling as well.

Metadata can change through `CvBaseInfo::CacheResults` and `readFrom`, and implicit object assignment must be considered for persistent caches. Starting with one map-write scope avoids most persistent-cache invalidation. Before implementation, compare the complete DLL-written byte stream and profile CRC time separately from compression and disk I/O. No save-time improvement has been measured yet.

### 5. Annex ranking without full sorting

`CvPlayerAI::AI_considerAnnex`, `CvPlayerAI.cpp:553`, sorts all cities by production, examines a cutoff-sized prefix and selects its first puppet. An equivalent O(C) selection can find the highest-production puppet with original-order ties, calculate that city's stable rank, and retain the existing cutoff expression.

Do not skip puppets in resistance while selecting. The original chooses the first puppet and then declines to annex it if it is in resistance; it does **not** try the next puppet. Keep capital handling, happiness/victory/trait gates, courthouse lookup and float-to-unsigned cutoff calculation unchanged. This is suitable for a small later patch after a source-extracted fixture; it is not the first macro optimization to pursue.

### 6. Startup query preparation

At the start of this pass, `CvDatabaseUtility::MaxRows`, `CvDatabaseUtility.cpp:325`, constructed and executed a fresh sizing query on each array initialization. A concurrent edit now reuses the utility's prepared-statement cache and resets/steps the statement on each call. This is already implemented in the worktree and should not be duplicated.

Validation for that change should measure prepare/step counts and preserve empty-table behavior, the current `max(rowid) + 1` convention and query failure behavior. No live database query plans, timings or tests of this concurrent edit were collected in this pass. Index changes remain separate because unordered duplicate rows can affect array initialization results.

## Optimizations this audit does not recommend yet

**Blindly replacing small stable sorts.** `CvCityStrategyAI::UpdateBestYields` sorts scalar yields. In the active byte-storage configuration, `std::sort` produced identical arrays in 7,600 tests, but **both implementations performed zero allocations** on this compiler. There is no demonstrated allocation saving. Further timing would need to justify the patch.

There is also a configuration trap: `AUI_WARNING_FIXES` changes storage to `unsigned int`, while `ReverseSort` still accepts `unsigned char`. Distinct wide values can compare equal after narrowing. Replacing stable sort changed full-array output in **123 of 200** stress cases. Do not apply that replacement indiscriminately or fix the narrowing as part of a performance patch.

**Using the existing city building list as an authoritative index.** `CvBuildingClasses.h:919` exposes `GetAllBuildingsHere()`, but a complete source search found only declaration, clearing and reads of `m_buildingsThatExistAtLeastOnce`; no population path. Replacing all-building scans with this list risks dropping results. An ordered index must be built and validated from authoritative definitions/counts. Repairing the existing cache's semantics would be a separate correctness task.

**Collapsing economic savings to an unordered sum.** `CvEconomicAI.cpp:1160` and 1206 stable-sort requests and may fail as soon as balance reaches zero. With negative request amounts, reordering or summing can change an early failure into success. Example: balance 5, higher-priority requests of 6 then -10. Preserve priority order, insertion ties, priority mutations and early-return rules; request lists are small, so prioritize larger work first.

**Treating each danger read as a fresh path search.** `CvPlayer::GetPlotDanger` delegates to `CvDangerPlots::GetDanger`, which is an indexed array read. The duplicate call in `ShouldBuilderConsiderPlot` can be hoisted, but it is a micro optimization. The surrounding nested condition must remain equivalent in optional configurations.

## Validation approach for the next implementation batch

1. Record both the commit and dirty-source hashes. The current branch contains performance edits beyond HEAD, so a commit ID alone is an incomplete baseline.
2. Give each optimization an explicit equivalence claim: identical rank bits, boolean result, ordered target/path/RNG trace, or serialized bytes. Preserve existing quirks unless a separate gameplay fix is requested.
3. Use actual extracted methods in fixtures, then compile the integrated DLL. Exercise ties, zero/negative values, missing entries, reloads and repeated calls after state changes. Compare optional configurations when edited code has active alternatives.
4. Measure removed operations and elapsed phase time separately. The tests here establish component properties, not full-game equivalence or end-turn gains.
5. Add opt-in aggregate profiling to a **Release** configuration. `AI_PERF_LOGGING` is compiled out under the current `FINAL_RELEASE`; toggling the runtime setting alone cannot recover those timers. Preserve Release gameplay defines and avoid per-candidate formatting/I/O. Useful phase totals include worker gathering/mission scans/path confirmation, trade enumeration, sight rebuilds, map serialization and database loading.
6. Replay representative early/late saves with the same commands and seeds against baseline/candidate builds. Compare action order, paths, RNG state and DLL-serialized gameplay data before save-container timestamps/compression. Investigate every unexplained difference. Include full seeded map generation, save/reload and multiplayer synchronization before release.

The previous batch's worker/shuffle timing limits still apply: finding a reachable worker target early is much cheaper with the heap, but all-unreachable selection can have some overhead; all-last-element shuffle draws favor the old removal algorithm. Those controlled cases justify thresholds/fallbacks and live profiling, not a universal speedup claim.

Reproduce this pass with:

```sh
python3 -B research/performance-followup/probe.py
```

`selection.cpp.in` uses source-extracted baseline methods and prototype substitutions with controlled engine doubles. It builds x86_64 with the release arithmetic flags and runs normally and under UBSan. `evidence.json` records results, active macros, compiler and source hashes. No game installation or production code was changed by these probes.
