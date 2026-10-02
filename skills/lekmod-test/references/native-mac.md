# Native Mac behavior and evidence

## Proven environment

Validated with the native Aspyr Steam build `1.0.3.279 (180925)`, x86_64,
EUI 1.28g, and the repository's macOS port. An isolated guarded app completed
turns 0–30 without desktop input, with an independently verified turn-30 save.
This establishes a tested engine configuration; a different engine or OS may
require adapting the guard.

Quick-play participant selection was not reliably overridable. Measure actual
participants rather than assuming a requested civilization set. The helper was
also validated for a short run, report re-export and owned-process cleanup.
Generated run artifacts are private local evidence, not skill dependencies.

Behavior checks in `macos/tests/test_background_skill.py` cover evidence
failures, guard acknowledgement, and process ownership without desktop input.
Run them after helper changes:
`python3 -m unittest discover -s macos/tests -p test_background_skill.py -v`.

## Isolation and staging

Default app:
`~/Library/Application Support/Steam/steamapps/common/Sid Meier's Civilization V/Civilization V.app`.
Use checkout helpers `macos/game_install.py`, `integrity.py`, `audit.py`, and
`package_assets.py` rather than the installer, which requires closing the
original game. APFS clone the app and source trees; never modify the installed
bundle. Snapshot LEKMOD, LEKMOD_DLL, LekmodInstaller, Lekmap, and macos, excluding
build/cache outputs. Compare fingerprints across the copy to detect edits.

Build the snapshot with `macos/build.py --release --jobs 2 --app …` and validate
its build manifest and engine imports. Stage LEKMOD and Lekmap with the
snapshot's packaging helpers so current Mac UI compatibility patches apply.
Use the cloned EUI folder when present. Stock loading-screen patching is
implemented but was not part of the original 30-turn run.

Code signing order matters: install the core, compile/install/sign the guard,
repair nested signatures, then sign and strictly verify the app. Signing
changes Mach-O bytes: record the unsigned release library hash separately
from the staged signed core hash. Verify the staged hashes before launching.

Change the copy's bundle ID to a unique `org.lekmod.backgroundvalidation.…`,
set `LSBackgroundOnly=true`, and use `CFBundleExecutable='Civilization V'`.
Invoke `Contents/MacOS/Civilization V` directly, bypassing AppBundleExe.
Private preferences disable fullscreen. Set `CFFIXED_USER_HOME` to the fresh
profile, `DYLD_INSERT_LIBRARIES` to the staged guard, `SDL_MAC_BACKGROUND_APP=1`,
`SDL_VIDEO_MAC_FULLSCREEN_SPACES=0`, and `SteamAppId/SteamGameId=8930`.
`C.UTF-8` was replaced by `en_US.UTF-8` in locale variables for this host.

The guard blocks AppKit activation/window presentation, cursor changes,
Carbon foreground promotion, display mode changes and gamma changes in the
test process. Its constructor refuses a non-private home or a non-background
bundle. No debug listener, FireTuner installation, or global input hook is
needed. A ready line in `runtime.log` is required.

Copy only settings and EUI Text from the ordinary profile, not saves/cache.
In `[CONFIG]`, `Audio=1` **disables** audio (0 enables it). Limit engine threads
to two, disable focus-loss throttling, enable logs and quick movement/combat,
disable intro video/mouse binding/debug tuner, and reduce child process priority.
The local sandbox may deny `os.nice` or native game startup; a permitted
scoped execution outside that sandbox is then needed. This does not require
foreground input or changing the user's game installation.

## Lua boundaries

The native startup Automation state exposes `Automation` and `Events`, but
not `PreGame` or `Game`. Use `Automation.SetGameCoreInit` and
`Events.SerialEventStartGame(0)` only there. The startup `EndTurn` callback did
not fire in the observed run; do not use it to count turns or exit reliably.
Inject telemetry into the disposable LEKMOD `Lua/UI/InGame.lua`, where normal
gameplay objects exist.

The loading screen pauses a single-player game for “Begin your journey.”
Patch only the copy's EUI `GameSetup/LoadScreen.lua` (or stock
`UI/FrontEnd/LoadScreen.lua`) to call the existing `OnActivateButtonClicked`
path for single-player too. Require exactly one matching conditional.
The initialization event must close the loading screen and unpause before
starting AI autoplay. Hidden MainMenu update callbacks were unreliable;
do not try to configure slots through that unverified mechanism.

`Modding.OpenUserData('LekmodBackgroundValidation', 1)` produces a userdata
with **dot-call** methods: `ledger.SetValue(key, tostring(value))`.
`ledger:SetValue` fails with a bad-self error. The live ledger is
`ModUserData/LekmodBackgroundValidation-1.db`, table `SimpleValues(Name,Value)`.
Lua.log is buffered; early empty logs do not establish lack of progress.

In `SequenceGameInitComplete`, find an inactive major slot and explicitly set
`SlotStatus.SS_OBSERVER` and `SlotClaim.SLOTCLAIM_UNASSIGNED` before calling
`Game.SetAIAutoPlay(target, activePlayer)`. A merely closed slot does not count
as an observer. The core's fallback can destroy the active human's units and
cities if no observer exists. Fail the test if no safe slot is available.

Record only live major players and valid teams, using GameDataDirty and
PlayerDoTurn events. Capture turn 0 before autoplay and every subsequent turn.
At the target call `pcall(UI.QuickSave)` and record completion. External Python
must verify the file and own cleanup; Automation.ExitGame was unreliable here.

The save header starts `CIV5`; at byte 8 are two little-endian uint32-length
UTF-8 strings (version and build), followed by a little-endian uint32 turn.
Validate lengths/truncation, then compare this turn with the ledger target.
Header validation does not establish that the save can be reloaded.

## Diagnosing a run

- Pangaea hit its 300-attempt reroll cap and spent roughly four minutes
  generating the map. Default total gameplay timeout is 15 minutes. Inspect
  native logs and the live ledger before diagnosing a stall; do not restart
  repeatedly while map generation is advancing.
- `LekmapTeamerMapLegacy.lua:508` raised `FeatureGenerator` nil while listing
  maps. The selected Pangaea map still generated and the test completed.
- `Lekmod_improvements.lua:15` printed `table does not exist, check the xml!`
  because `Improvement_Adjacency_Yields` was absent from the live database and
  source XML/SQL. Adjacency initialization returned early. Whether that is
  intentional requires review; preserve the warning as a finding.
- Existing StrategicView UNIQUE-constraint warnings also appeared. Retain
  database/runtime logs and distinguish observations from inferred causes.
- SDL turns SIGTERM into a quit event that may leave a confirmation pending.
  The runner waits briefly then kills only its retained child handle. Forced
  cleanup after verified completion is not a spontaneous gameplay crash.

On a failure, retain the run directory, logs, ledger, source and manifest.
Correct the isolated runner or the requested repository issue within scope;
start a fresh run for a justified retry. Keep ordinary-profile edits and original-game shutdown out of isolated AI
validation. An explicitly requested installed-game diagnosis uses its existing
authorization and the multiplayer backup/restore procedure.

## Steam and local Windows testing

Private macOS profiles and separate Desktops do not isolate Steam identity.
Native copies register AppID 8930 with the existing Steam client. Sharing one
account between Mac and Windows caused `Logged In Elsewhere`, a Steam IPC fatal
exit, and no established LAN join. Use distinct accounts for two Internet peers;
inspect current identities rather than assuming the historical account setup.

A remote Steam P2P timeout before host registration requires separate
diagnosis. Successful local two-account UTM games do not establish that every
remote host's network path works.

Read [multiplayer.md](multiplayer.md) for the latest validated local setup,
UTM guest-agent behavior, temporary hooks, and unresolved draft issue. Earlier
same-account and partial-join attempts are historical diagnostic evidence,
not prerequisites to repeat. Generated run artifacts are optional evidence;
the native runner does not depend on them.

## Additional native validation

A frozen-checkout run completed turns 0–30 with an independently verified
turn-30 save. Its assessment remained `completed-with-findings`: the legacy
Teamer FeatureGenerator error, missing adjacency-yield table and StrategicView
UNIQUE warning reproduced. Retain each new run's source/core hashes and evidence
privately when comparing results; historical run paths are not required inputs.

A staged read-only movement probe found CanMoveThrough and both default and
numeric-destination CanMoveOrAttackInto forms true on an eligible empty
neighbor for each player at turn 0. The installed older multiplayer core
returned false for CanMoveOrAttackInto in an analogous probe while normal
moves succeeded. Build and AI/human context both differ: do not infer that
current-source human multiplayer movement is fixed. Retain the probe JSON and
its instrumentation hash with the manifest when comparing builds.

After custom staged Lua instrumentation, repair nested library signatures
before signing the app; app signing alone can fail on Bink. Preserve the
original prepared manifest and refresh every staged hash plus diagnostics
provenance before the runner checks or launches the modified test app.

A v35.4 four-player crossplay attempt joined and drafted correctly but the native
engine crashed during terrain initialization before the Mac gameplay hook ran.
The faulting engine frames concern Voronoi polygon fixup, outside the native mod
core. This identifies the failing subsystem, not its cause; retain the crash
report and do not count the Windows-only turn-0 snapshot as a passed test.

Give each retry a distinct bundle identifier and corresponding private preference
file. Repair nested signatures before signing the application. Reused bundle
identifiers and app-only re-signing preceded early exit-255 attempts; a fresh
identifier plus nested signing restored startup, but those changes were not
isolated individually. Avoid claiming either alone was the confirmed cause.
