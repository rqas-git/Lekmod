# Reproduce the Mac–UTM Windows compatibility test setup

This guide provisions a second Mac with the same kind of automation used for
Lekmod's local two-client test. Read it before [multiplayer.md](multiplayer.md),
which contains the demonstrated runtime quirks and troubleshooting rules.
The skill includes all its native runner code, guard, Lua players and peer reader;
files under `macos/build/` and old `/tmp` scripts are **not dependencies**.

The finished setup supports a hidden native Mac client, a Windows host inside
UTM, normal networked game commands on both clients, file-based monitoring,
independent save verification, and restoration of temporary changes. The
`background_test.py` CLI automates **single-player AI**. Multiplayer uses the
bundled hooks plus per-run host/join and guest-control scripts; there is currently
no one-command cross-play runner. Installing the skill alone does not provision
Windows or automate Steam login.

## 1. Record the target and prerequisites

A successful continuous baseline used Aspyr Mac Civ V
`1.0.3.279 (180925)` x86_64, experimental registration `403694 FINAL_RELEASE`,
Windows Civ V `403694`, Lekmod v35.3, separate Steam accounts, and Windows 11 ARM.
Both clients reached turns 0–30 on Duel stock Continents, Quick, without combat. All 18 final snapshot values matched.
It used an older installed Mac core, **not the then-current checkout**.
See [the demonstrated capability](multiplayer.md#current-demonstrated-capability)
for provenance and limitations. New source/core/mod versions require a new test.

The following reference configuration supports reproducing the tested setup.
CPU/RAM values are example allocations, not requirements or hardware inventory:

| Component | Reference configuration | Reproduction guidance |
|---|---|---|
| UTM | 4.7.5, QEMU backend | Record your installed version; confirm its guest-agent CLI works |
| Guest | Windows 11 ARM, aarch64 `virt` | Apple Silicon path; Windows runs the x86 game through its emulation |
| CPU / RAM | 8 virtual CPUs / 12 GiB | Example allocation, not an established minimum; leave resources for macOS |
| Boot / disk | UEFI, TPM, NVMe qcow2 | Allocate enough disk for Windows, Steam, Civ V, backups and exports |
| Display | `virtio-ramfb-gl`, dynamic resolution | Install guest display drivers; establish a working game renderer |
| Network | Shared, `virtio-net-pci`, no port forwarding | Validated Steam Internet path; LAN is a separate unverified scenario |
| Guest integration | Windows guest tools / QEMU guest agent | Required for `utmctl exec` and file operations |
| Sound | `intel-hda` | Silence game channels; disabling Windows game audio caused a lobby error |
| Sharing | Read-only WebDAV and clipboard enabled | Optional; tests transfer files through the guest agent |

Discover your own VM UUID, user name, Steam IDs and process IDs; keep them
private along with credentials and network identifiers. On Intel Macs the ARM virtualization recipe does not apply;
use an appropriate Windows architecture and separately validate that environment.

Have two Steam accounts that can run Civ V with the same required expansions/DLC.
Sign-in, ownership, Steam Guard and initial Windows setup need the user's own
credentials. Make the accounts Steam friends for peer discovery. A private Mac
profile does not create a second Steam identity; the same account on both clients
previously disconnected the Mac with `Logged In Elsewhere`.

## 2. Prepare macOS and install the repository skill

1. Install Git, Python **3.10 or newer** (the runner uses `Path.is_relative_to`),
   and Apple's Command Line Tools. Run `xcode-select --install` if needed, complete
   the installer, then check `xcrun --find clang` and `python3 --version`.
   The runner uses the Python standard library, `clang`, `clang++`, `codesign`,
   and macOS copy tools; FireTuner and a debug server are unnecessary.
2. On Apple Silicon, install Rosetta if the x86_64 Steam/game/helper binaries
   cannot execute (`softwareupdate --install-rosetta --agree-to-license`).
3. Install Mac Steam and Civ V plus the required DLC. Launch the stock game once
   under the Mac test account to finish first-run setup and create
   `~/Library/Application Support/Sid Meier's Civilization 5/config.ini`.
   Exit normally. Record the actual Steam library and app path.
4. Clone this repository or select the requested checkout. Work from its root.
   The native build checks the engine binary; a different Aspyr build should
   fail validation rather than be made to pass by editing pinned hashes.
5. Install the skill by copying the **whole directory**, including `scripts`,
   `assets`, `references` and `agents`, into your agent's skill directory.
   For Codex, a new installation can use:

   ```sh
   REPO="$PWD"
   SKILL_DEST="${CODEX_HOME:-$HOME/.codex}/skills/lekmod-test"
   mkdir -p "$(dirname "$SKILL_DEST")"
   test ! -e "$SKILL_DEST" && cp -R "$REPO/skills/lekmod-test" "$SKILL_DEST"
   ```

   If that destination already exists, compare it and back it up before updating;
   do not silently replace another agent's changes. Reload skill discovery in
   the agent session. Alternatively, explicitly read and run the repo copy:
   `SKILL_DIR="$REPO/skills/lekmod-test"`. The runner always needs `--repo` to
   select the source checkout; its installed location is not the source tree.

Use the native runner's preflight to check Mac prerequisites without launching:

```sh
MAC_APP="$HOME/Library/Application Support/Steam/steamapps/common/Sid Meier's Civilization V/Civilization V.app"
SKILL_DIR="$REPO/skills/lekmod-test"
python3 "$SKILL_DIR/scripts/background_test.py" preflight --repo "$REPO" --app "$MAC_APP"
```

An optional native smoke test verifies the guard/build/profile path before
adding networking:

```sh
python3 "$SKILL_DIR/scripts/background_test.py" prepare --repo "$REPO" --app "$MAC_APP" --turns 2 --jobs 2
# Use the fresh directory printed by prepare, not a previous run.
python3 "$SKILL_DIR/scripts/background_test.py" play --run-dir "$RUN_DIR" --timeout 900
```

Require a guard acknowledgement, turns 0–2 and an independently verified turn-2
save. This proves the native automation path, not multiplayer compatibility.

## 3. Create and initialize the Windows VM

1. Install UTM in `/Applications/UTM.app`. Obtain legitimate Windows installation
   media and licensing. On Apple Silicon create a **QEMU-backed Windows ARM VM**
   with hardware virtualization, matching the backend used locally; do not choose
   another backend and assume its guest-agent interfaces are identical.
2. Configure UEFI/TPM, disk, RAM/CPU, display and Shared networking as above.
   Follow the [official Windows installation guide](https://docs.getutm.app/guides/windows/)
   for current media and installer instructions. Finish Windows setup, create
   an ordinary test user, install updates and reboot. Eject the Windows installer
   ISO after installation so it does not restart setup.
3. Mount **Install Windows Guest Tools** from UTM's removable-media controls.
   Run its installer inside Windows and reboot. The
   [official guest-tools instructions](https://docs.getutm.app/guest-support/windows/)
   describe installation and drivers. Confirm that the QEMU guest-agent service
   is installed/running in Services; working clipboard alone is insufficient.
4. Stay logged into the Windows test user's desktop. Do not log out when hiding
   the VM window. The automation launches graphical apps in this session.
   Configure sleep/lock behavior for the intended test duration; automatic
   Windows login is optional, not required. Check the session again after reboots.
5. Install Windows Steam and sign into the **second** Steam account. Install
   Civ V and the same required DLC. Let Steam finish the game's first-run
   redistributables. Launch `CivilizationV.exe` (the DX9 executable used locally)
   once, get to the main menu and exit normally. Establish a working display at
   modest windowed resolution before hiding the VM. A process existing without
   a rendered game/menu or Lua activity is not sufficient.
6. Locate the actual game library and Documents path. Default examples are
   `C:\Program Files (x86)\Steam\steamapps\common\Sid Meier's Civilization V`
   and `C:\Users\<user>\Documents\My Games\Sid Meier's Civilization 5`.
   Documents may be redirected to OneDrive; resolve it as the logged-in user,
   not SYSTEM. Record `whoami` in that user's terminal.
7. In the Windows Lekmod installer, select the Civ V game directory, desired
   Lekmod version and Standard UI or Enhanced UI (EUI), then install that version.
   Select and install the corresponding Lekmap version separately. The installer
   configures UI files before placing the package at `Assets\DLC\LEKMOD_<version>`;
   maps belong under the game's `Assets\Maps`. For an offline package, configure
   its UI using the repository's `LekmodInstaller/ui_manager.py` workflow before
   copying it into DLC. Verify the materialized `Lua\UI\MainMenu.lua`,
   `StagingRoom.lua` and `InGame.lua` actually exist. Raw repo UI templates are
   not a ready-to-use installation. Keep one active Lekmod package; an old
   duplicate can load the wrong UI/version. Align EUI use and DLC between peers.
8. Launch the mod normally once. Confirm the displayed mod/engine version,
   required expansions, and that the expected UI overrides load. Exit the game.
   Retain a powered-off backup/clone of this known-working VM before test patches.

For reproducing the historical baseline, obtain the corresponding v35.3 payload
for both clients. For **current-checkout** testing, deploy matching current mod
assets/maps and intended Windows DLL, build the Mac core from that checkout,
record hashes, and report this as a new experiment. Do not pair v35.3 Windows
with current Mac assets and label the result a same-version test. The public
packager's official-DLL policy is documented in [tools/README.md](../../../tools/README.md).
Windows compiler tooling is not needed to use an existing package; a custom
Windows DLL build needs its own compiler/build validation.

## 4. Establish host-to-guest automation

The Mac agent must run in a logged-in local macOS session. UTM's scripting bridge
can require macOS **Automation** permission for the actual terminal/agent host
process to control UTM. Grant that permission when macOS prompts; if blocked,
check Privacy & Security → Automation. An SSH-only Mac session is not a supported
replacement. Guest-agent control does not need host Accessibility/input control.
See [UTM's scripting reference](https://docs.getutm.app/scripting/reference/).

```sh
UTMCTL=/Applications/UTM.app/Contents/MacOS/utmctl
"$UTMCTL" version
"$UTMCTL" list
VM_UUID='<UUID of the intended Windows VM from list>'
"$UTMCTL" status "$VM_UUID"
# If stopped, boot it; then log into its Windows desktop.
"$UTMCTL" start "$VM_UUID"
```

Record the version and inspect `exec --help` and `file --help` for that release.
[UTM's CLI source](https://github.com/utmapp/UTM/blob/main/utmctl/UTMCtl.swift)
uses stdin for uploads and stdout for downloads. The locally tested release
returned from guest commands before completion and often gave empty stdout;
newer implementations may wait. In either case use an explicit result file.

Create a Mac-side `ProbeGuest.ps1` with this content:

```powershell
$ErrorActionPreference = 'Stop'
$result = @{
    time = (Get-Date).ToUniversalTime().ToString('o')
    identity = [Security.Principal.WindowsIdentity]::GetCurrent().Name
    interactiveUser = (Get-CimInstance Win32_ComputerSystem).UserName
    agent = @(Get-Service | Where-Object { $_.Name -match 'qemu' } |
        Select-Object Name, Status)
    game = @(Get-Process CivilizationV -ErrorAction SilentlyContinue |
        Select-Object Id, Path)
}
[IO.File]::WriteAllText('C:\Windows\Temp\Lekmod-Probe.json',
    ($result | ConvertTo-Json -Depth 5))
```

Push and run it from the Mac:

```sh
"$UTMCTL" file push "$VM_UUID" 'C:\Windows\Temp\ProbeGuest.ps1' < "$RUN_DIR/ProbeGuest.ps1"
"$UTMCTL" exec "$VM_UUID" --cmd 'C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe' -NoProfile -ExecutionPolicy Bypass -File 'C:\Windows\Temp\ProbeGuest.ps1'
"$UTMCTL" file pull "$VM_UUID" 'C:\Windows\Temp\Lekmod-Probe.json' > "$RUN_DIR/guest-probe.json"
```

Poll the pull if the result is not ready, using short waits and progress updates.
Require a fresh UTC time later than the request, SYSTEM identity, the correct
interactive user and running agent. In real runs use **unique script/result
filenames per request** to exclude stale successes. Write `ok`/`error` JSON in
`try`/`catch` for guest mutations. Transfer a small known file in both directions
and compare SHA256 before relying on the channel for game files. No guest SSH,
port forwarding, WebDAV share or host mouse/keyboard input is required.

## 5. Make the native Mac client multiplayer-capable

Close the native game for installation. On a newly provisioned machine, use the
repo's validated installer and cross-play configurator:

```sh
python3 macos/install.py --app "$MAC_APP" --component both --jobs 2
python3 macos/crossplay.py --app "$MAC_APP" --enable
```

These change the selected installed app and retain backups. Run them as setup,
not during an isolated background test while the user is playing. Existing
validated installations can skip reinstalling; inspect their manifest and hashes.
The configurator checks the inspected Aspyr executable, native registration
marker and build metadata. Do not bypass those checks or change engine identity
strings manually. The prototype keeps native build metadata at 180925 and writes
`Contents/Resources/lekmod-crossplay.txt` containing `403694 FINAL_RELEASE`.
A successful configuration is not a compatibility certificate.

For each multiplayer run, **clone** this app to a fresh directory under
`macos/build/multiplayer-diagnosis/`; do not patch the installed bundle. Follow
[native isolation and staging](native-mac.md#isolation-and-staging), using the
checkout's `macos/game_install.py` clone/sign helpers. Do not use the native AI
runner's prepared app unchanged: its startup automation launches single-player
and its InGame telemetry calls AI autoplay.

Build a private multiplayer app/profile with these requirements:

1. Give the clone a unique bundle ID, `LSBackgroundOnly=true` and
   `CFBundleExecutable='Civilization V'`; remove `LSUIElement` if present.
2. Create a fresh private home and its
   `Library/Application Support/Sid Meier's Civilization 5` directory. Copy
   ordinary settings and EUI Text only, excluding saves, caches and ledgers.
   Set the unique bundle's `Library/Preferences/<bundle-id>.plist` to
   `DisplayFullScreen=false`. Use `background_test.py`'s `create_profile` and
   `stage` as implementation references, adapting them for multiplayer.
3. Compile the bundled guard into the clone:

   ```sh
   clang -arch x86_64 -mmacosx-version-min=10.13 -dynamiclib \
     -framework AppKit -framework Carbon -Wno-deprecated-declarations \
     "$SKILL_DIR/assets/background_guard.m" \
     -o "$TEST_APP/Contents/MacOS/background_guard.dylib"
   ```

4. Append the multiplayer hooks and MainMenu join instrumentation described
   below. Keep normal frontend startup; exclude `Automation.lua`, single-player
   autoplay telemetry and their launch arguments. Repair/sign the core, guard
   and nested libraries **before** signing/verifying the app. Record the unsigned
   release core hash separately from the signed staged core hash.
5. Launch the clone's `Contents/MacOS/Civilization V` directly with a retained
   Python `subprocess.Popen` handle, `cwd=Contents/MacOS`, redirected runtime log,
   and `-DisplayFullScreen NO`. Set child environment:
   `CFFIXED_USER_HOME=<private-home>`,
   `DYLD_INSERT_LIBRARIES=<clone>/Contents/MacOS/background_guard.dylib`,
   `SDL_MAC_BACKGROUND_APP=1`, `SDL_VIDEO_MAC_FULLSCREEN_SPACES=0`,
   `SteamAppId=8930`, `SteamGameId=8930`. Leave `HOME` untouched; on this host
   `C.UTF-8` locale values needed replacement by `en_US.UTF-8`.
6. Require the guard's startup ready line. Stop the retained child if it fails.
   The owner must remain alive through the run and clean up on timeout/interruption.
   TERM may need escalation to KILL after a bounded wait on the same owned handle.
   Verify there is no other native test client before launching or reconnecting.

The guard deliberately makes the native client unwatchable. A separate Desktop
is unnecessary. Never replace it with host clicks, activation or global input.

## 6. Prepare temporary Windows patches and interactive launch

Use a unique run ID, guest working directory (for example
`C:\Windows\Temp\Lekmod-<run-id>`), ledger names, save name, lobby suffix and
scheduled-task name. Keep a host-side manifest containing actual app/game/mod/
profile paths, guest user, VM UUID, source commit and dirty-source digest, engine
versions, core/DLL/assets/instrumentation hashes, and request timestamps.

With **both test games stopped**:

1. Read and back up the guest's `MainMenu.lua`, `StagingRoom.lua`, `InGame.lua`,
   `config.ini` and `UserSettings.ini` byte-for-byte. Export copies to the Mac
   and record SHA256. Recheck current destination hashes before replacing them;
   abort if anything changed since backup. Retain originals in the guest too.
2. Append `assets/multiplayer-staging.lua` to the actual mod StagingRoom.lua
   and `assets/multiplayer-player.lua` to InGame.lua **once on each client**.
   Replace `LekmodCrossplayStage` and `LekmodCrossplayTurns` with unique per-run
   names. Both hooks target turn 30; give the save a unique name on both clients.
   If changing the target, change every target check, marker and save name.
   Preserve original Lua contents/newlines; do not overwrite whole UI files
   with a hook or with another mod version's script.
3. Enable `LoggingEnabled` in `[DEBUG]`, turn off tuner/autorun, skip the intro,
   disable mouse binding, and use windowed, modest graphics. Limit simulation
   threads to two as in the tested settings. On **Windows**, `[CONFIG] Audio=0`
   keeps audio initialized; set `Music.Volume`, `Effects.Volume`,
   `Ambience.Volume`, `Speech.Volume` in `[Audio]` of UserSettings.ini to 0.
   `Audio=1` disabled Windows sound and caused `Sound cannot be found` in chat.
   The native guard's AI profile uses its separately tested Audio=1 setting.
4. Add a temporary Windows MainMenu host hook as in the next section, then
   copy prepared files into their backed-up destinations. Verify their hashes.

SYSTEM must not launch Steam/game directly: it lacks the user's interactive
Steam session. Execute a script like this through the guest agent, substituting
**discovered** paths, account and unique task name:

```powershell
$ErrorActionPreference = 'Stop'
$gameDir = '<actual Steam Civ V directory>'
$user = '<COMPUTER\logged-in-user>'
$task = 'Lekmod-Validation-<run-id>'
if (Get-Process CivilizationV -ErrorAction SilentlyContinue) {
    throw 'An existing game must be identified and stopped first'
}
if (Get-ScheduledTask -TaskName $task -ErrorAction SilentlyContinue) {
    throw 'Task name already exists; use a fresh run ID'
}
$action = New-ScheduledTaskAction -Execute (Join-Path $gameDir 'CivilizationV.exe') -WorkingDirectory $gameDir
$principal = New-ScheduledTaskPrincipal -UserId $user -LogonType Interactive -RunLevel Limited
$settings = New-ScheduledTaskSettingsSet -AllowStartIfOnBatteries -DontStopIfGoingOnBatteries -ExecutionTimeLimit (New-TimeSpan -Minutes 30)
Register-ScheduledTask -TaskName $task -Action $action -Principal $principal -Settings $settings | Out-Null
Start-ScheduledTask -TaskName $task
```

Steam should already run under this user. If restarting it is necessary, use
another named interactive task for Steam, wait for login/connection, then launch
the game. No password is needed for an Interactive principal that is logged in.
Export fresh task status, process paths and Lua initialization telemetry. Task
registration/start success alone does not prove game startup. Clean up only the
tasks created by this run.

## 7. Host, join and run without desktop input

Keep the default Windows-host/Mac-client direction for reproducing the completed
baseline. Reverse-host lobby handshake was observed but reverse-host gameplay
was not demonstrated.

**Host:** append a temporary MainMenu Lua hook on Windows. Wait for the stock
`Events.SystemUpdateUI` event with type `SystemUpdateUIType.RestoreUI` and tag
`MainMenu`, after DLC activation and `PreGame.LoadPreGameSettings`. Require
`Network.IsConnectedToSteam()`, then perform these calls once:

```lua
UI.SetMultiplayerLobbyMode(MultiplayerLobbyMode.LOBBYMODE_STANDARD_INTERNET)
PreGame.SetInternetGame(true)
PreGame.SetGameType(GameTypes.GAME_NETWORK_MULTIPLAYER)
PreGame.ResetSlots()
ResetMultiplayerOptions()
PreGame.SetWorldSize(GameInfo.Worlds.WORLDSIZE_DUEL.ID)
PreGame.SetGameSpeed(GameInfo.GameSpeeds.GAMESPEED_QUICK.ID)
PreGame.SetGameOption("GAMEOPTION_NO_BARBARIANS", true)
PreGame.SetPrivateGame(false)
PreGame.SetLoadFileName("")
local name = Locale.ConvertTextKey("TXT_KEY_LEKMOD_VERSION") .. " Validation <run-id>"
local result, pending = Matchmaking.HostInternetGame(name, 2)
```

Wrap the hook in `pcall` and record the result/pending values, exact lobby name,
Steam connection, engine version and errors in its own unique ModUserData ledger.
Defer retries until menu readiness; do not install an eager update callback that
replaces stock DLC initialization. The tested map was stock Continents; verify
actual map/size/speed in telemetry instead of inferring them from requested values.
The localized version prefix is required by Lekmod's lobby checks.

**Join:** in the Mac clone's MainMenu context, initialize the Internet browser
with `UI.SetMultiplayerLobbyMode`, `Matchmaking.InitInternetLobby()`,
`Matchmaking.SetMultiplayerGameListType(0,0)` and
`Matchmaking.RefreshInternetGameList()`. Observe
`MultiplayerGameListComplete`/`MultiplayerGameListUpdated`, enumerate
`Matchmaking.GetMultiplayerGameList()`, match the **exact intended lobby name**
and call `Matchmaking.JoinMultiplayerGame(entry.serverID)` once. Record names,
result/pending, join failures and connection events in another unique ledger.
Use the stock lobby's joining-screen handling for that engine/UI version.

The historical Mac browser sometimes omitted the Windows lobby. The completed
run used the game's direct Steam lobby-invite path. Compile the bundled helper
and read the intended Windows friend's cached lobby:

```sh
clang -arch x86_64 "$SKILL_DIR/assets/read_peer.c" -o "$RUN_DIR/read-peer"
"$RUN_DIR/read-peer" "$STEAMCLIENT_DYLIB" "$WINDOWS_FRIEND_STEAM_ID" > "$RUN_DIR/peer.json"
```

Discover `steamclient.dylib` in the installed Mac Steam distribution. Preserve
Steam/lobby uint64 IDs as **decimal strings**, not JavaScript numbers. Require
`ok=true`, `gameID=8930`, a nonzero lobby ID and a fresh read while the intended
host is present. Route that lobby through the installed game's normal
`MultiplayerGameLobbyInvite`/command-line-invitation handling; inspect the actual
engine/UI's invite signature before adding the per-run Lua or launch hook.
Do not assume the browser's serverID and Steam lobbyID are interchangeable.
This invite orchestration is an engine-specific part the agent must implement;
there is no bundled universal invitation CLI. Do not run a second
`SteamAPI_Init` AppID 8930 helper, which displaced the game's tracked Steam PID.

Require actual `MultiplayerConnectionComplete`, StagingRoom initialization and
two connected players. Registration alone does not establish synchronization.
The bundled staging hook drafts, selects a pool civ and readies each client.
If Mac receives LOCK messages but remains unlocked, preserve telemetry and
reconnect **only the owned native client** before turn 0 into the same lobby;
this restored persisted draft state in both successful tests. Verify lock and
readiness rather than repeatedly reconnecting blindly. For a checkpoint lobby,
the host must call stock `LaunchGame`; the staging hook handles it.

After `SequenceGameInitComplete`, the player hook performs networked founding,
research, production, policies, pantheons, unit orders and end turns. Never use
`Game.SetAIAutoPlay` on human network clients. Monitor each client's fresh ledger
and actual processes, allowing map generation time. If production stacks units,
queue `assets/multiplayer-resolve-stack.lua` in that client's `command` key and
check `commandResult`, coordinates and continued turns. See
[multiplayer live diagnostics](multiplayer.md#gameplay-hook-and-live-diagnostics)
for the normal move path and unique-command requirement.

## 8. Export evidence and establish full functionality

Native telemetry is under the private profile's `ModUserData`; Windows telemetry
is under the resolved user's game profile. Lua `Modding.OpenUserData(name,1)`
creates `<name>-1.db`, table `SimpleValues(Name,Value)`. Methods use **dot calls**.
Poll/export using unique request names and fresh initialization/tick/export times.
Logs may be buffered until shutdown. Empty stdout/logs do not establish failure.

For Windows binary or in-use files, execute a collector through the guest agent.
Open each SQLite/save/log with .NET `FileStream` and `FileShare.ReadWrite`, encode
binary bytes as Base64 in a JSON result, include fresh UTC time and per-file
errors, then pull that text result and decode on the Mac. For example:

```powershell
$f = [IO.File]::Open($path, [IO.FileMode]::Open, [IO.FileAccess]::Read, [IO.FileShare]::ReadWrite)
try {
    $m = New-Object IO.MemoryStream
    try { $f.CopyTo($m); $encoded = [Convert]::ToBase64String($m.ToArray()) }
    finally { $m.Dispose() }
} finally { $f.Dispose() }
```

Copy final database/save files after the game stops when possible. Live file
sharing permits reading but does not guarantee a transaction-consistent SQLite
snapshot; include any `-wal`/`-shm` companions or use a SQLite backup API and retry
if the exported database is inconsistent. On macOS use Python sqlite3 read-only
connections/backup to inspect the live ledger. Archive failures, not just the
last successful state.

Full automation acceptance requires:

- A fresh guest-agent probe and verified file round trip.
- An interactive-user Windows game launch and native guard acknowledgement.
- Correct distinct Steam identities, engine/mod/core provenance and actual map.
- Both clients initialized; every turn ID **0 through 30** recorded on both.
- Live empires and activity, completion markers, and empty or reported error fields.
- Both uniquely named saves independently verified with `CIV5` headers and turn 30.
- Matching settled final snapshots for every actual participant, including
  identity, human/AI status, team, cities, units, population, gold, science,
  score, policies and technologies.
- Verified test-process shutdown, restored guest hashes and removed launch tasks.

The save header layout is documented in [native-mac.md](native-mac.md#lua-boundaries)
and implemented in the runner. Do not require platform save byte hashes to match;
Mac and Windows headers/serialized bytes differ. Header checks do not prove reload.
Save separate reports for source/build validity, single-player AI, lobby handshake
and synchronized multiplayer gameplay. Label findings and checkpoint/reconnect
interventions. Combat, visual UI, reverse-host gameplay, save reload and remote-host network paths remain separate tests.

## 9. Stop and restore

1. Export final saves/ledgers and diagnostics. Stop only the retained native child
   and the Windows game identified by fresh executable path/PID checks. A guest
   SYSTEM `Stop-Process` may require `-Force` for the user's owned test process.
   Verify disappearance before replacing files; never kill a stale recorded PID.
2. Restore the five exact guest originals. Compare every restored SHA256 with
   the preflight backup, not with last run's originals. Preserve saves/evidence.
3. Unregister only the unique temporary tasks, then verify they are absent and
   export a fresh restoration result. Keep backups if restoration failed.
4. Leave the installed native app and ordinary Mac profile intact after isolated
   testing. Setup's persistent cross-play option can be disabled later with
   `python3 macos/crossplay.py --app "$MAC_APP" --disable`; do not silently undo
   a user's existing installation preference.
5. Keep the per-run manifest, reports, logs, scripts, saves and restoration JSON
   under the generated run directory. Commit reusable instructions/assets, not
   VM disks, guest credentials, Steam IDs or cloned game bundles.

For handoff, give the next agent this guide, the skill directory, actual local
paths/VM/user identifiers in a private setup manifest, and the last report's
scope. Do not treat historical generated artifacts as installed tooling.

## Four-player scenarios

The host example above is the historical two-player baseline. For two humans
and two AIs, use a map supporting four major civilizations, capacity 4, two
SS_OPEN human slots and two SS_COMPUTER slots; close unused major slots. Assign
different teams if combat is required. Host only after MainMenu RestoreUI, then
verify which OPEN slots the host and joining client actually claimed. A preset
SS_TAKEN slot caused a phantom human and blocked drafting. Record the final
players after initialization; slot configuration alone is not participant proof.
See [coverage.md](coverage.md) for the basic player hook’s missing expansion/combat behaviors.
