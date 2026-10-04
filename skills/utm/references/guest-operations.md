# Guest-agent operations

Official references: [CLI overview](https://docs.getutm.app/scripting/scripting/),
[scripting reference](https://docs.getutm.app/scripting/reference/) and
[examples](https://docs.getutm.app/scripting/cheat-sheet/).
Guest execution/file access requires a running supported QEMU VM with guest agent.

Observed UTM 4.7.5 syntax:

```sh
utmctl exec VM_UUID --cmd powershell.exe -NoProfile -File 'C:\Windows\Temp\job.ps1'
utmctl file push VM_UUID 'C:\Windows\Temp\input.bin' < local.bin
utmctl file pull VM_UUID 'C:\Windows\Temp\result.json' > result.json
```

Push consumes stdin; pull writes stdout. There is no local-file positional argument.
Transfers can be slow because of small guest-agent chunks. Prefer small verified
deltas when updating a known installation; hash both source and guest output.
A delta requires a verified base, not merely a similarly named directory.

On tested 4.7.5, `exec` returned empty success before guest completion. Do not
interpret this as task success. Give each job a random ID and a fresh output file;
write `{id, startedUtc, finishedUtc, ok, result, error}` as JSON from a guest
try/catch/finally envelope. Poll that exact file with a finite timeout, require the
matching ID and completion timestamp, and inspect `ok`. Current upstream CLI
source has different waiting behavior; recheck the installed version.

Windows agent commands run as SYSTEM. Discover the interactive user's account,
Documents path and application path. GUI apps such as Steam require a temporary
scheduled task using that logged-in principal with interactive logon, rather than
launching in SYSTEM's session. Use a unique task name and the least needed run
level. Inspect the actual application PID/path after starting it.

For an owned game restart, stop its named task/process, wait for process exit,
and obtain a fresh process list before restoring files or launching again.
A successful Stop-ScheduledTask call is not proof the game exited. Preserve exact
backup hashes for every modified file, and compare restored originals. Keep an
explicitly requested permanent application upgrade when restoring test hooks.

For locked saves or SQLite telemetry, guest .NET FileStream with
FileShare.ReadWrite can copy bytes into a Base64 JSON envelope. Pull and decode
locally; verify the database schema, fresh run ID and expected turn. File copies
can be inconsistent while writers are active; prefer a settled checkpoint or
owned game shutdown when final evidence must be stable.

The bundled `scripts/windows_job.py` implements that fresh envelope for an
existing guest-agent connection:

```sh
python3 "$SKILL_DIR/scripts/windows_job.py" --vm "$VM_UUID" \
  --script probe.ps1 --output probe.json --timeout 60
```

The script runs the supplied PowerShell as SYSTEM and returns failure when the
job envelope reports an error. It deliberately retains uniquely named guest
script/result files as evidence. Avoid scripts that call `exit`, launch detached
work, or suppress errors if the envelope must prove the whole operation completed.
It does not select a VM, start an interactive session or grant authorization for
the supplied commands. Do not print sensitive command results into public logs.

Use -LiteralPath for PowerShell reads, enumeration and hashing of actual filenames.
Civ V asset folders contain brackets, which ordinary -Path parameters interpret
as wildcard expressions. A failed/empty hash is an audit failure, not proof that
the asset is missing. UTM 4.7.5 file pull can also return process status 0 while
printing a guest open-file error; validate the expected payload, not status alone.

If guest execution and file transfer both time out, inspect fresh VM status and
owned processes before retrying. A cached started state is not proof of a working
guest-agent connection. Preserve completed evidence; classify a disconnected run
as incomplete. Recover only the owned VM, prefer graceful shutdown and verify its
actual state afterward. In one 4.7.5 recovery, `start --hide` returned Operation not
available for the stopped test VM, while normal `start` succeeded; the cause was
not established. Do not assume a timeout means an operation had no effect.

In an October 2026 Windows 11 ARM validation, two owned PowerShell preflights
containing Get-CimInstance Win32_ComputerSystem grew to roughly 10 GB private
memory each. Guest commit space was nearly exhausted and DWM/Game Bar crashed.
Minimal file/hash envelopes worked. The individual query's cause was not isolated;
avoid repeating a stalled broad preflight or assuming every timeout is an agent
disconnect. Track each owned guest process and start timestamp; inspect memory
and task/crash evidence, then stop only confirmed owned jobs. A lightweight
taskkill recovered memory when another PowerShell cleanup could not start.
After stopping those two jobs, guest memory load fell from 91% to 31%, available
commit space rose from about 0.8 GB to 13.9 GB, and the game created its lobby.
Use an already verified install/user record or a small independent probe instead
of rediscovering account paths with that query on the affected VM. Keep actual
Steam identities private and verify distinct peers without dumping account files.

The job envelope now records the guest process ID and writes a matching
.started.json breadcrumb before executing the script. A timeout reports its
path; inspect that fresh ID/start time and verify the live process before
cleanup. The helper bounds transport/polling but does not automatically stop a
hung guest script. Failure envelopes retain errorMessage, errorRecord and
scriptStackTrace as well as the exception string: a plain PowerShell throw
sometimes supplied only RuntimeException through Exception.ToString().
