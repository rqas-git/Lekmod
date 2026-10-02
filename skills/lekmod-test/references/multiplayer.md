# Local Mac–Windows multiplayer validation

For installation on another machine, see [the Mac/UTM setup guide](utm-setup.md).
This reference covers running and diagnosing tests after provisioning.

## Current demonstrated capability

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
