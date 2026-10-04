# Local Mac–Windows multiplayer validation

For installation on another machine, see [the Mac/UTM setup guide](utm-setup.md).
This reference covers running and diagnosing tests after provisioning.

## Historical v35.3 capability

A local Windows/UTM host and native macOS client completed turns 0–30
continuously after launch. Both clients recorded all 31 turn IDs, produced
independently verified turn-30 saves and matched all 18 final-state values.
This exercised research, production, growth, policies, pantheons, normal unit
orders and network turns. Combat and remote-host connections require separate
tests. Checkpoint loading was demonstrated in a separate resumed run.

Tested environment: Aspyr 180925 x86_64 native Mac with the experimental
Windows-registration prototype (403694 FINAL_RELEASE), Windows Civ V
403694/Lekmod v35.3 in UTM Windows 11 ARM, distinct Steam accounts.
This was an installed-core result, not current-checkout multiplayer validation.
Hash the actual tested binary and measure current engine/mod/account versions
in a private run manifest. Do not change registration identity merely because
this prototype worked. Generated evidence is private and is not a skill input.

## Workflow and temporary hooks

The normal skill CLI runs native single-player AI only. Multiplayer hooks below
are tested components, not a complete launcher. Prepare per-run host creation,
join/invite handling, guarded native launch, and guest task orchestration using
the current game state. Do not depend on a previous /tmp launcher or old run.

1. Inspect live test processes and Steam identities. Use distinct accounts for
   two Steam Internet peers; sharing one disconnected the Mac with `Logged In
   Elsewhere`. Keep only one native test client alive and give each run a fresh
   profile/ledger. Respect any original game the user is using.
2. Use the native window/input/display guard and private profile described in
   [native-mac.md](native-mac.md#isolation-and-staging). Operate Windows through
   the UTM guest agent; no host mouse, keyboard, activation or Desktop switching.
   Discover the current VM/user/paths rather than hard-coding old identifiers.
3. Back up the guest's MainMenu.lua, StagingRoom.lua, InGame.lua, config.ini and
   UserSettings.ini before changing them; record original hashes. Keep native
   patches inside a disposable app. Set silent Windows audio as described below.
4. Append `assets/multiplayer-staging.lua` once to backed-up StagingRoom.lua and
   `assets/multiplayer-player.lua` once to backed-up InGame.lua on each client.
   Before staging, replace the two hooks' `LekmodCrossplayStage` and
   `LekmodCrossplayTurns` ledger names with unique per-run names. Give the player
   a unique save name; if changing its target, update every turn-30 condition,
   completion marker and save name consistently. Defaults test 30 turns.
5. Create the host lobby only after MainMenu's SystemUpdateUI RestoreUI event,
   DLC activation and PreGame.LoadPreGameSettings have finished. Early creation
   reset world/slot settings. The lobby name must include the exact localized
   TXT_KEY_LEKMOD_VERSION string (version returned by the current install).
6. Configure human slots OPEN before the host claims its slot; a pre-set TAKEN
   slot can become a phantom extra human and prevent readiness. Verify actual
   human and AI slots after hosting. Join the intended test lobby. A direct Steam lobby invite worked when the
   host was absent from the Mac browser. Inspect browser assets_hash/game_version
   filters when relevant; their presence does not prove why a lobby is omitted.
7. Monitor fresh lobby/game telemetry on both clients. Handle demonstrated
   draft or stacked-unit blockers below. Completion requires actual game
   initialization, every turn 0–target, independent save-header checks and
   matching final state; lobby presence alone is insufficient.
8. Stop the owned test games, verify disappearance using a fresh process list,
   restore the five guest files and compare original hashes, and remove only
   the test's named scheduled task. Preserve exported evidence and test saves.

The user's existing authorization to close/reopen a game carries forward;
these instructions do not introduce another approval gate. Closing a test
process or restoring its temporary files is normal cleanup. Installed-game
changes stay within the requested diagnosis.

## Lobby and draft behavior

The staging hook respects the normal EnsureStagingUpdate/show path and delays
ledger access until ready. Do not add an eager startup staging callback: that
previously failed during native startup; the exact interaction was not isolated.
For a saved-game lobby, the host must call stock LaunchGame after both clients
connect: GameStarted suppresses the new-game readiness countdown. The hook does
this. Fresh lobbies use the default draft and the first civ in each player's pool.

In both completed local tests, the Mac received draft LOCK messages but stayed
unlocked. Reconnecting the sole native client before turn 0 restored persisted
PreGame draft state and allowed readiness. This is an observed workaround, not
a mod code fix. Verify draft lock/readiness after reconnect, and preserve evidence
if it still fails rather than treating repeated blind reconnects as progress.

## Gameplay hook and live diagnostics

The player starts at SequenceGameInitComplete, uses canonical network commands,
records snapshots and initialization/tick timestamps, stops at the target, and
requests a named save. Do not call Game.SetAIAutoPlay on human network clients:
the observer switch can destroy a human empire.

Important API behavior already incorporated into the player:

- Unit:CanFound takes a Plot. Founding consumes the settler: cache unit IDs
  before missions and do not access a consumed unit afterward.
- Require both not IsPolicyBranchUnlocked and CanUnlockPolicyBranch. The latter
  does not exclude an already unlocked branch here; repeating it stalled a run.
- Return after policy/pantheon commands so network state settles before end-turn.

The bot does not automatically make room for newly produced combat units.
Stacked-unit blockers occurred at turns 25 and 28 in the continuous test.
`assets/multiplayer-resolve-stack.lua` is the tested normal MISSION_MOVE_TO
command: it moves an awake unit off a stacked tile to an empty adjacent tile
accepted by CanMoveThrough. Queue it in the disposable player's `command`
ledger value, then verify commandResult, coordinates, blockers and turn progress.
Append a unique Lua comment when repeating: identical commands execute once.
The command mailbox evaluates trusted test Lua only; it is not a remote service.

On the older installed core, CanMoveOrAttackInto was false for checked empty
passable neighbors while CanMoveThrough was true and normal moves succeeded.
Default and explicit numeric forms were false; Boolean optional arguments caused
a number-required error. A fresh-checkout AI probe returned true on eligible
neighbors, but both build and human/AI context changed. No current-source human
multiplayer repair is established by this comparison.

## Telemetry and failure stages

ModUserData ledgers use `SimpleValues(Name,Value)`; Lua methods are dot calls,
not colon calls. Old keys survive app restarts. Use unique per-run names,
initializationTime/tickTime, fresh export timestamps, and actual process state;
a stale error/completion key or empty log is not current-state proof.

Track MultiplayerJoinRoomComplete, ConnectedToNetworkHost,
MultiplayerNetRegistered, MultiplayerConnectionComplete, failure/mismatch events,
then actual game initialization and synchronized turns. The first three events
alone do not prove game-state handshake completion. Logs are buffered: use a
normal authorized game shutdown when needed to read them. Do not explicitly
shut down SteamAPI before game shutdown; duplicate shutdown caused a cleanup
crash. A TERM request may leave a confirmation pending: verify disappearance
before relaunching and never signal a stale saved PID.

Historical failures, relevant only if reproduced:

- Remote Steam P2P can time out before host registration. Successful local
  UTM turns do not establish that a remote network path works.
- One local repeat registered then reported message chunk 1 of 1347372365
  (0x504f454d), type 1900544, and stalled at game-state synchronization.
  The cause is unestablished. Successful joins decoded 19 chunks, type 1011,
  and completed NetInitInfo; do not infer universal cross-platform incompatibility.
- A reverse-host attempt reached Windows MultiplayerConnectionComplete and
  StagingRoom, but game launch/turns in that direction were not demonstrated.

## UTM guest-agent operations

`utmctl exec` runs as SYSTEM and, on observed UTM 4.7.5, returns before completion; empty stdout
proves neither success nor failure. Write guest results to a file, poll a fresh
UTC timestamp, then pull it. Interactive Steam/game launches need a temporary
scheduled task under the existing logged-in guest user's principal. Remove that
task after testing. SYSTEM may need an explicit force option to close that user's
owned test game; verify the process before editing or relaunching.

`utmctl file push UUID GUEST_PATH` uploads stdin, not a local-file positional
argument. For binary/in-use saves or SQLite files, read through .NET FileStream
with FileShare.ReadWrite, encode Base64 in text/JSON, then pull/decode. Verify
fresh timestamps and the expected schema. Read-only VM screenshots can help
observe progress without changing host focus.

## Silent Windows settings

On Windows v35.3, config.ini Audio=1 disabled the audio system and stock lobby
chat recorded `Sound cannot be found`. The validated silent configuration was
Audio=0 plus UserSettings.ini Music.Volume, Effects.Volume, Ambience.Volume and
Speech.Volume all 0. Restore the exact originals after testing. Native guarded
AI tests use their separately demonstrated Audio=1 setup.

## Read the intended friend's lobby without registering another game

`assets/read_peer.c` uses installed steamclient.dylib SteamClient017 /
SteamFriends015, opens a temporary client pipe, reads GetFriendGamePlayed, and
releases the pipe. Compile with `clang -arch x86_64`; pass steamclient.dylib and
the intended test friend's decimal SteamID. Require ok=true, gameID=8930 and a
nonzero lobbyID string. Preserve IDs as decimal strings: uint64 Steam IDs exceed
JavaScript's safe integer range.

Prefer this helper to LLDB SteamFriends evaluation, which stalled on this Rosetta
host. Do not use another helper calling SteamAPI_Init for AppID 8930: it registered
as another game process, replaced the tracked game PID, then unregistered itself.
Its effect on packet delivery is unconfirmed; the client-pipe helper produced no
game-process registration update. Keep debugger detachment/cleanup reachable
if debugging is independently needed.

## Expanded actions and entity snapshots

Use `assets/multiplayer-expanded-player.lua` instead of the basic player when
expansion, worker improvements and combat are requested. It prioritizes settlers
and workers, moves combat units toward an opposing capital, and has the host
request war from turn 12 and peace from turn 24 for contacted opponents. Actual
results still depend on production, terrain and contact: check coverage rather
than assuming every requested action occurred. This hook completed a v35.4
30-turn local run with AI expansion and actual unit/city damage; that run also
reported synchronization findings, so this is not proof of clean compatibility.

Its snapshots include sorted unitIDs/cityIDs. Filter entity fields through those
current rosters: same-turn keys from consumed settlers or killed units otherwise
remain in SimpleValues and create false differences. Compare all live entities
and primitive state after repeated unchanged snapshots on both peers. Record
resyncs and state differences explicitly; matching population/score is insufficient.

Before upgrading map versions, archive the old versioned map folder outside
active Maps on each peer. Duplicate helper filenames can be discovered from the
old folder. Verify the staged app as well as the ordinary installation; cloning
an older installed app otherwise carries v6.2 into a v6.3 validation run.

## v35.4 validation findings

A two-human, two-AI v35.4 run reached turns 0–30 on both peers and wrote
independently verified turn-30 saves. Expanded actions exercised AI city founding,
worker improvements, policies, unit/city damage, war and peace. Initial map and
starting-state differences triggered automatic resynchronization. A later live
probe still found one AI worker at different coordinates on the peers at turn 30.
A repeat founded an additional human city at turn 27, then lost its Windows peer
at turn 28; both ledgers stopped there. These are findings, not clean acceptance.
Removing duplicate v6.2 map folders did not eliminate the initial resync. Do not
attribute it to a particular seed, map helper or platform until diagnostics prove
the cause. Reload/reconnect and release-specific capture, belief and yield
scenarios remain separate coverage requirements.

When a fork adds shared map helpers, compare the entire map-folder inventory and
hashes on both peers. Uploading only a changed map is insufficient. In the October
4 validation, public Windows v6.3 had 21 scripts and lacked HBMapOptions, while
the fork had 24. Updating only Teamer caused LekmapOptions=nil during discovery.
Install the complete folder and verify every helper before restarting the game.
An existing lobby's option cache is not evidence that new disk files were loaded.
Keep any diagnostic map instrumentation separate from the permanent map payload.

A read-only integer-conversion probe narrowed an October 4 map divergence:
Map.GetPlotByIndex(61.6) returned index 61 on native Aspyr and 62 on Windows;
86.666... returned 86 versus 87. Windows also rounded 2.5 to 2 and 3.5 to 4,
while native truncated. The same first random values and initial fractal samples
preceded a differing shallow-water random call. Fractional fractal dimensions
and ridge counts therefore need explicit compatibility validation. Do not assume
Lua 5.1 versions use identical number-to-integer conversion. The pinned Windows
Lua header contains its x87 nearest-even conversion; test inputs, checked/optional
arguments, negative values and boundaries when adapting native bindings. The
native compatibility adapter passed this probe in a staged Teamer game; a fresh
Pangaea network run then matched all 64 bounded random calls and four fractal
samples without an initial resync. Sustained gameplay remains a separate check.

For frequent polling, transfer compressed ledger copies and keep full log
collection for checkpoints or shutdown. In one run, Base64 gzip reduced guest
SQLite telemetry enough to avoid repeated large pulls. Decode and verify SQLite
readability and fresh timestamps; compression does not make a live copy atomic.
RandLog bit 2 includes pregame calls, but map RNG is asynchronous and also needs
bit 3 to log it. Full fractal/ridge traces can be enormous and slow generation;
prefer bounded Lua probes and retain only necessary diagnostics for routine runs.

Treat foundRequest/action markers as requests. Confirm city founding, movement,
construction and damage from subsequent state snapshots; an accepted-looking Lua
call can precede a rejected or delayed network action. GameInfoActions is a table;
use pairs when inspecting the normal Game.HandleAction path.

Production requests are asynchronous: after selecting a unit, do not treat a
still-empty local queue as permission to request fallback production. The hook
now records its choice before sending it; otherwise a settler could be replaced
by a building or scout before the first request was acknowledged.

Install revised hooks before a fresh launch. After a live replacement, later external native-ledger
command updates did not persist; multiple userdata handles were present, but the precise cause was
not isolated. Use a new prefix and checkpoint reload instead of
counting a hot-patched harness as a reproducible acceptance run. Track pending
city production until a nonempty queue acknowledges it, including between ticks.

Build test lobby names from Locale.ConvertTextKey("TXT_KEY_LEKMOD_VERSION").
JoiningRoom compares that exact token; a v35.4-only label failed when the
installed token was v35.4.003. For checkpoint hosts, use SetLoadFileName,
GetFileHeader and ReadActiveSlotCountFromSaveGame as the stock LoadMenu does.
When reusing a private profile, filter telemetry by the new exact prefix;
old databases are not evidence that the new client initialized.

The expanded hook’s greedy movement can stall behind obstacles, and its first
other-player target may be peaceful. Arrange combat deliberately and verify
actual damage; war declarations alone are insufficient. Do not call
unit:GeneratePath: the exposed v35.4 Lua binding raises NYI. Normal destination
missions use the engine pathfinder. Changing a running hook is not a substitute
for validating a revised hook from a fresh launch.
