"""Compile the actual launcher model and exercise its process transitions."""
from pathlib import Path
import platform
import subprocess
import tempfile

HERE = Path(__file__).resolve().parent
source = (HERE.parent / 'Launcher.swift').read_text()
declarations, separator, _ = source.partition('\nlet application = NSApplication.shared\n')
if not separator:
    raise RuntimeError('Could not locate the launcher application entry point')
build = HERE.parent / 'build'
build.mkdir(exist_ok=True)
with tempfile.TemporaryDirectory(prefix='lifecycle-test-', dir=build) as directory:
    directory = Path(directory)
    main = directory / 'main.swift'
    main.write_text(declarations + '\n' + (HERE / 'launcher_lifecycle.swift').read_text())
    executable = directory / 'check-lifecycle'
    subprocess.run(['xcrun', 'swiftc', '-O', '-target',
                    f'{platform.machine()}-apple-macosx13.0',
                    '-module-cache-path', str(build / 'swift-module-cache'),
                    str(main), '-o', str(executable)], check=True)
    subprocess.run([str(executable)], check=True)
