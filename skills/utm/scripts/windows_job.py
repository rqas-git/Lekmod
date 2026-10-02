#!/usr/bin/env python3
"""Run a PowerShell script through UTM with a fresh completion envelope."""
import argparse
import json
from pathlib import Path
import subprocess
import tempfile
import time
import uuid


def literal(value):
    return "'" + value.replace("'", "''") + "'"


def run_job(cli, vm, script, timeout):
    token = uuid.uuid4().hex
    remote = 'C:\\Windows\\Temp\\utm-job-' + token
    result_path = remote + '.json'
    wrapper = f"""$ErrorActionPreference='Stop'
$r=@{{id={literal(token)};startedUtc=(Get-Date).ToUniversalTime().ToString('o');ok=$false}}
try {{
$r.result=@(& {{
{script}
}})
$r.ok=$true
}} catch {{$r.error=$_.Exception.ToString()}}
finally {{
$r.finishedUtc=(Get-Date).ToUniversalTime().ToString('o')
[IO.File]::WriteAllText({literal(result_path)},($r|ConvertTo-Json -Depth 10))
}}
"""
    with tempfile.TemporaryFile() as source:
        source.write(wrapper.encode('utf-8-sig'))
        source.seek(0)
        subprocess.run([cli, 'file', 'push', vm, remote + '.ps1'], stdin=source,
                       check=True, timeout=timeout)
    subprocess.run([cli, 'exec', vm, '--cmd', 'powershell.exe', '-NoProfile',
                    '-ExecutionPolicy', 'Bypass', '-File', remote + '.ps1'],
                   check=True, timeout=timeout)
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        remaining = deadline - time.monotonic()
        pulled = subprocess.run([cli, 'file', 'pull', vm, result_path],
                                capture_output=True, timeout=max(1, remaining))
        if pulled.returncode == 0:
            try:
                result = json.loads(pulled.stdout.decode('utf-8-sig'))
            except (ValueError, UnicodeError):
                result = None
            if result and result.get('id') == token and result.get('finishedUtc'):
                return result
        time.sleep(min(1, max(0, deadline - time.monotonic())))
    raise TimeoutError(f'No completed guest envelope: {result_path}')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cli', default='/Applications/UTM.app/Contents/MacOS/utmctl')
    parser.add_argument('--vm', required=True)
    parser.add_argument('--script', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--timeout', type=float, default=60)
    args = parser.parse_args()
    if args.timeout <= 0:
        parser.error('--timeout must be positive')
    result = run_job(args.cli, args.vm, args.script.read_text(encoding='utf-8-sig'), args.timeout)
    args.output.write_text(json.dumps(result, indent=2) + '\n')
    print(f"Guest job {result['id']}: ok={result['ok']}; {args.output}")
    return 0 if result['ok'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
