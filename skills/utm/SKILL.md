---
name: utm
description: Inspect and operate UTM virtual machines on macOS using utmctl and guest-agent scripting, including guest file transfer and interactive Windows launches. Use for UTM automation and diagnostics rather than generic virtualization advice.
---

# UTM

Discover `/Applications/UTM.app/Contents/MacOS/utmctl` (or the installed app path),
then read `version`, `help`, `list` and the relevant subcommand's help. Select the
VM by its current UUID/name and inspect its state. Installed commands can differ
from current online documentation; do not assume snapshot or screenshot support.

Use CLI/guest-agent operations when possible. Read
[guest-operations.md](references/guest-operations.md) before file transfer,
Windows execution or interactive application launches. Read-only computer-use
screenshots can diagnose guest state without desktop input when uninterrupted
operation is required.

Preserve unrelated VMs and processes. Track owned tasks, guest backups and fresh
results. Before a retry, verify the prior operation finished and that an owned
application actually exited. Restore temporary guest configuration and remove
only tasks created by the workflow. VM stop, application exit and task stop are
different operations; `stop` defaults to force in UTM 4.7.5, so inspect help and
use `--request` for a requested graceful VM shutdown.
