# Standard Lekmod gameplay target

This checkout targets **Lekmod v35.4**, upstream commit
`201df6c56b41cc353ec01b2523bf852733d922e8`, released October 1, 2026.
Local improvements must preserve that release's shared simulation behavior.
The Windows reference is the official release DLL with SHA-256
`a5ac79567eff7c738f12c655457d2255199ef5da1e9e43aeb0e2b688b9161fd2`.

This policy removes known gameplay differences in the fork. It does **not**
certify native Mac–Windows multiplayer. Real two-peer testing has covered joining,
initialization and sustained gameplay, with the findings below. Save reload,
reconnect and individual release mechanics require their own evidence. The existing Mac cross-play switch only adapts the registration
boundary. No installed game is changed by editing or building this checkout.

## Restored stock behavior

- Bolivia, Georgia, Italy, Mexico, Mughals, New Zealand, and Venice abilities,
  including stock event registration, player selection, rewards, and save keys.
- Dummy-building player/team dispatch and dummy-policy initialization. The
  optimization removing repeated identical building writes remains.
- Draft allocation and lobby message handling, so legitimate stock messages
  and random choices follow the stock implementation.
- City-state personality trait identifiers, lake and freshwater behavior,
  improvement pillage/repair handling, AI specialization iteration, peace filters,
  and the barbarian-strategy war-state condition.
- Consulates vote awards and event callback removal, matching stock peers.
- Research Lua argument handling and the stock Lua game API. The added overflow
  setter and audit-version marker are removed.
- City-connection storage growth, which affects serialized state; per-update
  lookup caches remain.
- Legacy Teamer generator dependencies now use the shared HBMapGenerator and
  HBFeatureGenerator helpers; its own mirroring logic remains unchanged. Both
  peers need the complete matching fork map folder, including its shared helpers.

Several restored behaviors are upstream bugs. Fixing them only on this machine
can produce different shared game state. Any future fix needs an upstream release
change or independent evidence that it preserves behavior with stock peers.

## Retained local improvements

- Native macOS ABI, integer-width and serialization adapters, and the opt-in
  registration prototype.
- Launcher startup optimizations, EUI selection, installation repair, backups,
  rollback, operation locking, download validation, and source/build provenance.
- Launcher save browsing and explicit copy-only backups outside the game save
  folder; backup creation never replaces an existing file.
- UI query and text-generation improvements, shared UI packaging, and source
  refactors whose outputs are covered by regression tests.
- Yellow city headers in trade-route tooltips for both supported UI modes.
- City-connection metadata caches; improvement and promotion metadata caches;
  reduced trade-ranking copies and cached trade ranges; stable worker selection
  and plot snapshots; prepared database queries and build-type caches.
- UAE and Kilwa query shortcuts with stock-core fallbacks, ordered policy-row
  caching, and early unit filtering.
- Rank-based map shuffling and map helper refactors that preserve tested seeded
  outputs and random-call traces.
- Pointer-width allocator alignment, read-only Lua query fixes, fractional trade
  tooltips, route countdown displays, UI control guards, and corrected visual assets.
  Mac layout overrides are applied during packaging; the Windows DLL remains stock.
- Owning-string lifetime protection for `unit:GetScriptData()`, preserving its
  intended return value without reading freed memory.

The performance fixtures compare directly with the pinned stock source. They
cover outputs, ordering, and random calls where relevant, and run native cases
with undefined-behavior checking. These isolated checks establish component
equivalence, not complete live-game determinism or a whole-game speedup.

## Verification

Provide Python dependencies from `tests/performance/requirements.txt` and
`requests`, plus a Lua 5.1 interpreter/development library. Set `LUA51` to that
interpreter if it is not named `lua5.1` on PATH.

```sh
python3 -B -m unittest discover -s tests -v
python3 -B -m unittest discover -s macos/tests -v
python3 -B -m unittest discover -s LekmodInstaller/tests -v
python3 -B tests/performance/validate.py
python3 macos/build.py --release
```

`tests/test_stock_compatibility.py` guards the restored behavior against the pinned
release. Previous tests requiring divergent gameplay fixes have been replaced
with these stock comparisons; installer, UI, binding, map, lifetime, and
performance coverage is retained.

Windows packages use the exact public DLL by default. The packager rejects
unverified replacements. Mac native builds remain separate artifacts with
source/build manifests and must be rebuilt after changing gameplay source.

### Historical v35.3 validation: September 21, 2026

- 125 unit/regression tests passed across the main, macOS, and installer suites.
  The Windows batch/PowerShell test was skipped on macOS.
- Stock-source performance comparisons passed, including ordering, random-call
  traces, and the native undefined-behavior sanitizer configurations. Swift
  launcher lifecycle checks passed.
- A clean native release build passed all 358 engine-import checks and six
  pregame ABI anchors. Its manifest matched the current source and library.
- Real Windows and Mac packages were built in temporary directories. All 304
  packaged Lua scripts across the three tested UI/platform variants, plus 24 map
  scripts, passed Lua 5.1 syntax checks. The official Windows DLL, 519 Override
  XML files, and 40 SQL files matched the public v35.3 archive; the five retained
  shared-Lua optimizations are covered by stock-comparison fixtures.
- Standard-UI package XML parsed successfully, excluding 509 empty stock
  placeholders. Optional EUI retains three files with strict XML parser errors
  already present in the official archive: `TechPopup.xml` (comment-only),
  `NotificationPanel.xml` (invalid token), and `CityBannerManager.xml` (duplicate
  attribute). These inherited issues remain unresolved.
- AddressSanitizer was unavailable: even a trivial standalone probe timed out.
  No live Windows peer, save/load, or reconnect session was exercised, so native
  Mac–Windows compatibility remains unverified.

Validation also caught and fixed the legacy-Teamer-only installer case: an
unusable payload is rejected before replacing existing maps, while the complete
official v6.2 map archive remained accepted.

### Historical v35.4 validation: October 2, 2026

The upstream v35.4 source, Lua/UI, official Windows DLL and Lekmap v6.3
updates are integrated. The installer removes older Lekmap folders from its
staged installation so duplicate helper files are not discovered.

- Main tests: 47 run, one Windows-only check skipped; macOS: 81 passed;
  installer: 22 passed. Stock performance comparisons and undefined-behavior
  checks passed. The release build passed 358 imports and six pregame ABI anchors.
- Native AI smoke test recorded turns 0–30 with active civilizations and a
  verified final save. It is supplementary single-player coverage.
- Windows was upgraded to v35.4.003 using the official DLL hash above; the
  official payload and v6.3 maps were audited, allowing only text line-ending
  differences and the documented temporary test hooks.
- A real Windows-host/native-Mac session with two humans, two major AIs and four
  configured city-states recorded all turns 0–30 on both peers. Independent final saves
  identify turn 30. Observed actions included research, production, policies,
  improvements, AI expansion, unit/city damage, war and peace.
- This **failed clean crossplay acceptance**: initial map/starting-state
  divergence required automatic resynchronization, and a later live turn-30
  probe found one Huns worker at different coordinates on the peers. Matching
  population/score and completing 30 turns do not erase that difference.
- A repeat with only v6.3 maps founded an additional human city at turn 27, then
  lost its Windows peer at turn 28. Both ledgers stopped there. Guest-agent calls
  also timed out; the disconnect's cause is unestablished. Removing duplicate
  maps did not eliminate the initial synchronization finding.
- Legacy Teamer's missing FeatureGenerator, an absent adjacency-yield table
  warning and duplicate StrategicView registration remain runtime findings.
  Capture/recapture, Tithe, specific civilization/wonder yields, crossbow defense,
  save reload and reconnect still require targeted current-release scenarios.

Evidence is retained under `macos/build/upstream-v35.4/`, including the R8 and
R9 ledgers/reports, independent R8 saves, Windows integrity audit, test/build
logs and guest restoration hashes. Temporary hooks/settings were restored and
the owned guest task removed again after the separate seed diagnostic. The
diagnostic did not reach map initialization and established no seed cause.
All test clients were stopped and the Windows VM returned to its original
stopped state, retaining v35.4 and v6.3.
Native Mac–Windows compatibility is not certified.

### Current v35.4 validation: October 4, 2026

The reported stale-unit crash was reproduced from the user's turn-80 save at
turn 85. EUI flag callbacks now resolve the current unit instead of retaining a
removed unit wrapper. The same untraced save completed turns 80–110 after the
fix, with an independently verified turn-110 save. A separate allocator-abort
report has no established cause; this fix does not claim to explain it.

Native Lua bindings now reproduce the Windows Lua 5.1 nearest-even 32-bit
integer conversion, including checked/optional arguments and invalid/range
behavior. Previously Aspyr truncated fractional fractal sizes and ridge counts.
The patched fresh Pangaea crossplay run matched 64 bounded random calls and four
fractal samples and had no initial resynchronization. This fixes a demonstrated
initial map divergence; it does not establish complete determinism.

- Main suite: 55 tests, one Windows-only test skipped on macOS; macOS: 84 passed;
  installer: 22 passed. The skipped batch behavior was separately executed on
  real Windows for all five UI fixture variants (ten batch executions), and each
  output matched the canonical package comparison.
- Stock performance/output/RNG comparisons and native undefined-behavior checks
  passed. Swift launcher lifecycle checks passed. The final native release passed
  356 engine imports and six pregame anchors. The latest R17 run retained the
  exact source/build manifest and staged core hash.
- Explicit native Teamer setup used four major AIs, two teams and a reserved
  observer. It recorded all turns 0–30 and an independently verified final save,
  with AI expansion, research and policies. The observer trait lookup now safely
  rejects an unassigned leader. Reserving the observer before creation remains
  necessary; a late-observer setup encountered a separate engine crash.
- The patched R13 Windows-host/Mac-client run had two humans, two major AIs and
  four living city-states, every turn 0–30 on both peers, and independent turn-30
  saves. A repeated settled comparison of recorded live entities found zero
  differences. Human/AI expansion, policy/research progress, AI improvements,
  war and peace occurred; actual combat damage did not occur in this run.
- R13 still logged one late AI `CvPlayer::m_paiBuildingClassMaking` sync warning.
  It is **completed with findings**, not clean crossplay acceptance. A generic
  Windows lobby Lua nil-call error also remains unattributed. Neither finding
  is suppressed or explained by matching final aggregate statistics.
- A fresh latest-core R17 Teamer run used two humans and two AIs on opposing
  teams, recording every turn 0–30 and independent saves on both platforms.
  Both humans reached two cities and both AIs three. Research, policies and
  improvements progressed. Two repeated settled probes matched 962 recorded
  live-state and supplemental counter fields, with unchanged state between probes.
  Consumed-unit rows were excluded using the current entity rosters.
- R17 retained one AI unit synchronization event covering five movement/mission
  variables and the generic Windows lobby Lua error. There was no initial full
  resync. Actual combat damage was absent, so this is **completed with findings**
  and incomplete combat coverage, not clean acceptance. Normal exploration and
  auto-move interventions were recorded rather than attributed to a proven cause.
- Checkpoint attempt R15 loaded the prior turn-30 save but stopped progressing
  at native turn 43/Windows ledger turn 42. The cause is unestablished; it is
  incomplete, not a successful sustained reload validation.
- An initial team attempt R16 crashed at turn zero in the upstream tourism
  calculation after raw founding requests had not produced human capitals.
  The native code now checks for a capital before reading its city culture.
  Extracted-function undefined-behavior tests cover absent and valid capitals;
  R17 uses normal founding actions and waits for acknowledgement before ending
  turn zero. The official Windows DLL remains unchanged and retains that
  unguarded upstream path. The Windows process also exited during R16, but no
  Windows stack established the same cause.
- Reserved Pangaea Dummy 1–9 options are hidden without renumbering subsequent
  options. Legacy Teamer now loads existing shared helpers; both peers require
  the full matching 24-script map folder. Connected-region fixture outputs are
  unchanged, with 72 cases taking 1.84 seconds before and 0.087 seconds after;
  this measures that routine, not overall map startup.

Evidence is retained in `macos/build/validation-20261004/`, including source/core
hashes, crash reproduction, independent saves, per-peer ledgers and network logs,
real Windows batch results, final build checks and launcher DMG verification.
The [automation research](research/game-automation/README.md) describes checkpoint
fixtures and a scenario matrix; the generalized framework was not implemented.

The upstream diff review restored stock comments, license/vendor notes and
installation documentation, removed redundant platform branches, and restored
unrelated generated reports rather than leaving large report deletions in the
functional comparison. Remaining large changes include shared UI materialization,
ordered source/map refactors with equivalence fixtures, native Mac support,
installer/launcher work, and tests/skills/research. The generalized automation
framework remains research only.
