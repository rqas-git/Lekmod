# Standard Lekmod gameplay target

This checkout targets **Lekmod v35.3**, upstream commit
`70aa6ad5e343845903719aea48f55bbba8edebb8`, released September 5, 2026.
Local improvements must preserve that release's shared simulation behavior.
The Windows reference is the official release DLL with SHA-256
`c8c265d26e6d67bab7c99371692a5b001d274cea6e80a3d9794b5626c994f357`.

This policy removes known gameplay differences in the fork. It does **not**
certify native Mac–Windows multiplayer: joining, initial-state transfer, sustained
gameplay, save/load, and reconnect still need testing against an unchanged Windows
v35.3 installation. The existing Mac cross-play switch only adapts the registration
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
- Research Lua argument handling and the stock Lua game API. The added overflow
  setter and audit-version marker are removed.
- City-connection storage growth, which affects serialized state; per-update
  lookup caches remain.
- Legacy Teamer generator dependencies. Official Lekmap v6.2 lacks its two
  mirrored helpers, so that legacy entry point remains unsupported. The installer
  allows this specific known stock packaging gap only alongside usable maps,
  without substituting different generators; validation of other required helpers
  remains in place. An unusable payload cannot replace a working installation.

Several restored behaviors are upstream bugs. Fixing them only on this machine
can produce different shared game state. Any future fix needs an upstream release
change or independent evidence that it preserves behavior with stock peers.

## Retained local improvements

- Native macOS ABI, integer-width and serialization adapters, and the opt-in
  registration prototype.
- Launcher startup optimizations, EUI selection, installation repair, backups,
  rollback, operation locking, download validation, and source/build provenance.
- UI query and text-generation improvements, shared UI packaging, and source
  refactors whose outputs are covered by regression tests.
- City-connection metadata caches; improvement and promotion metadata caches;
  reduced trade-ranking copies and cached trade ranges; stable worker selection
  and plot snapshots; prepared database queries and build-type caches.
- UAE and Kilwa query shortcuts with stock-core fallbacks, ordered policy-row
  caching, and early unit filtering.
- Rank-based map shuffling and map helper refactors that preserve tested seeded
  outputs and random-call traces.
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

### Validation snapshot: September 21, 2026

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
official v6.2 map archive remains accepted.
