# Performance changes on `perf`

This branch starts at `1bc2ff42` and implements seven items from the performance worklist. The performance edits remove repeated work while retaining the existing gameplay rules and calculations. The subsequent correctness fixes below intentionally repair existing behavior. Targeted checks have passed; full gameplay equivalence and actual speedups remain unmeasured.

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

Validation on 2026-09-12 passed for all seven performance edits. The reproducible suite is in `tests/performance`; its recorded output, including production source hashes, is in `docs/performance-validation.json`.

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
