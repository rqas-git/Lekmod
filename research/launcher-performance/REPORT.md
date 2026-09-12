Measured September 12, 2026 against the installed Steam Civ V app. Baseline:
`Lekmod` commit `38a4db60`. Candidate: the working tree launcher changes.
These original measurements were taken in main before moving the launcher
optimizations and this report into the perf worktree.
Samples were collected before the separate, concurrently edited EUI feature
changed checkout fingerprints. Repeating the benchmark requires a ready game
installation matching the checkout at the time of the new run.

| Operation | Baseline median | Updated median | Reduction |
| --- | ---: | ---: | ---: |
| Installation checks when opening the launcher | 2.628 s | 1.849 s | 29.6% |
| Play checks before handing off to Steam | 5.234 s | 1.923 s | 63.3% |

Each median covers three repetitions, alternating baseline/candidate order.
These are repeated reads with warm OS caches, not cold-boot timings. All content,
native-interface, compatibility and signature checks ran against the real game.
Only the final Steam-open command was intercepted, and repair was prohibited.
The measurements exclude Swift window creation, Steam/game startup, new-map
generation and save loading. The installed game passed all checks; no game files
or settings were changed by the benchmark.

The launcher now performs one authoritative inspection under its installation
lock for a ready Play action, reuses the validated core digest, and overlaps
codesign with content checks. Repairs release the launcher lock while the
installer holds its own, then reacquire it before post-repair validation and
Steam handoff. No previous UI report or persistent validation cache authorizes
launch.

The Swift lifecycle monitor also avoids a full scan as the game process starts.
It updates the controls immediately and requests fresh validation after exit.
This removes another inspection that formerly overlapped game startup; its
effect on time to the game menu is not included in the table.

The user's `UserSettings.ini` already has `SkipIntroVideo = 1`. Menu background
artwork is cached: an isolated measurement took about 150 ms on initial
conversion and under 1 ms on reuse. Logos took about 16 ms. These are minor
relative to the removed full inspections.

Reproduce from the perf worktree root with
`python3 research/launcher-performance/benchmark.py`. macOS process discovery and
signature verification require execution outside the agent's restricted sandbox;
inside it they can produce misleading process/signature failures.

Raw measurements: [results.txt](results.txt). The earlier duplicate-removal-only
experiment measured 5.308 s → 2.607 s (50.9%) before overlapping checks:
[duplicate-removal-results.txt](duplicate-removal-results.txt).

Validation: 46 Python tests cover installation, cross-play and launch gating,
including locking, changed content, failed signatures and repairs. The native
Swift lifecycle check covers external starts, launcher starts, game exits,
busy-state probes and pending-launch timeout behavior.
