---
name: lekmod-test
description: Validate Lekmod in native macOS Civilization V with an isolated 30-turn AI runner, or diagnose local Mac–Windows multiplayer using documented temporary hooks. Verify turns, saves, and runtime findings without desktop input. Does not perform visual UI testing.
---

# Lekmod test

Use the bundled runner rather than rediscovering native launch and Lua behavior.
It freezes the requested checkout, builds its native core, stages a disposable
app and private profile, runs AI turns, and exports evidence. Default: turns
0–30, quick speed, small Lekmap Pangaea, native quick-play participants.

## Choose the test

For current-checkout AI validation, use the CLI below and the native reference.
For a join failure or Mac–Windows self-play, read
[references/multiplayer.md](references/multiplayer.md); its temporary hooks need
per-run launch/host/join orchestration and are not a turnkey multiplayer CLI.
For a new Mac or Windows VM, first follow
[references/utm-setup.md](references/utm-setup.md), which covers provisioning,
guest-agent access, interactive launches, native isolation, and acceptance checks.
Do not run AI autoplay as a substitute for a requested network test.

## Run

Read [references/native-mac.md](references/native-mac.md) before the first run
or when adapting the host, game setup, or instrumentation. Resolve `SKILL_DIR`
to this skill's directory and `REPO` to the user's requested checkout; do not
assume the installed game contains the current source.

```sh
python3 "$SKILL_DIR/scripts/background_test.py" preflight --repo "$REPO"
python3 "$SKILL_DIR/scripts/background_test.py" prepare --repo "$REPO" --turns 30 --jobs 2
python3 "$SKILL_DIR/scripts/background_test.py" play --run-dir "$RUN_DIR" --timeout 900
```

`prepare` prints its fresh run directory under `macos/build/background-tests/`.
It does not launch the game. `play` owns the child process, polls the private
SQLite ledger, verifies the quicksave header, exports the report, and stops
that child on completion, failure, timeout, or interruption. Keep the tool
session alive and provide progress updates using its output and files.
Do not launch multiple test copies concurrently.

Options: `--app` selects the installed source app; `--map` selects a filename
inside the checkout's `Lekmap` directory; `--expect-civ CIVILIZATION_…` (repeat)
requires the actual participant set to match. `--reuse-release PATH` skips
compilation only when that library's release manifest matches the frozen
source. Native quick-play slot configuration was not reliably overridable:
record actual participants; a requested specific scenario needs verified
setup rather than an assumed six-civilization game.

To re-export existing evidence without launching anything:

```sh
python3 "$SKILL_DIR/scripts/background_test.py" report --run-dir "$RUN_DIR"
```

## Uninterrupted operation

- Operate through native Automation and InGame Lua plus file telemetry. A
  separate macOS Desktop does not isolate global input. Do not use clicks,
  keystrokes, foreground activation, `open -a`, or UI automation as a fallback
  when the user requires uninterrupted background operation.
- Keep every patch, profile write, signature change, and test save inside the
  fresh run directory. Preserve the installed app, ordinary profile, and
  original running game during isolated AI tests. If the user expressly
  requests diagnosing their installed game, use that authorization and the
  multiplayer backup/restore procedure. Use `CFFIXED_USER_HOME`; leave `HOME`
  untouched.
- Require the staged background-only plist, injected guard, and its startup
  acknowledgement. If these checks fail, stop the owned child and diagnose
  the isolated copy; do not retry in the foreground.
- A private profile does not isolate Steam: native test copies still register
  AppID 8930 with the same Steam client. Check the active Steam identity before
  multiplayer tests and use distinct accounts for two Steam peers.
- The current guard makes the test window unwatchable. A separate Desktop is
  unnecessary; manually focusing the guarded copy is not supported.
- Before `Game.SetAIAutoPlay`, reserve an explicit unused observer slot. Without
  an observer, this native implementation can kill the human empire.
- Use a new run directory for retries; never kill a PID read from a stale file.
  The runner's saved PID is evidence only. SDL may ignore SIGTERM, so cleanup
  may require SIGKILL of the still-owned child after exporting the save.

Read [references/coverage.md](references/coverage.md) when planning acceptance
coverage beyond a smoke test, especially after upstream gameplay changes.

## Assess and report

Success requires every turn 0 through the requested target, live civilization
activity, a completion marker, and an independent matching quicksave header.
The report retains source/core/save hashes, actual map and civilizations,
per-turn metrics, runtime errors, warnings, and shutdown details. Completion
with findings is distinct from a clean run. Do not suppress errors in the
mod to obtain a passing smoke test.

Link `validation-report.txt`, `validation-results.json`, and the exported
`.Civ5Save`. State this is an AI single-player smoke test; visual UI, human
choices, multiplayer, and save reload require separate validation.
Read [references/native-mac.md](references/native-mac.md#diagnosing-a-run)
for the observed warnings and startup delays before labeling a run hung.

For multiplayer diagnosis, read [references/multiplayer.md](references/multiplayer.md).
The AI runner cannot substitute for a two-client network test.

## Retain new learnings

Put demonstrated new host or Lua quirks in `references/native-mac.md`, with
the game/build context and supporting run evidence. Keep unconfirmed causes
marked as hypotheses. For a reliable workflow fix, update the bundled helper,
run the relevant checks, and synchronize the repository and installed skill
copies. Preserve concise guidance instead of accumulating attempt transcripts.
