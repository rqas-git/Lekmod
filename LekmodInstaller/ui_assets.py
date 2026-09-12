
import json
from pathlib import Path
import shutil


class MissingUISource(RuntimeError):
    pass


def load_manifest(lekmod):


    for path in (Path(lekmod) / 'ui_manifest.json',
                 Path(__file__).with_name('ui_manifest.json'),
                 Path(__file__).resolve().parents[1] / 'LEKMOD/ui_manifest.json'):
        if path.is_file():
            manifest = json.loads(path.read_text(encoding='utf-8'))
            if manifest.get('format') != 1:
                raise RuntimeError(f'Unsupported UI manifest: {path}')
            return manifest
    raise RuntimeError('UI manifest is missing.')


def relative_path(root, relative):

    root = Path(root).resolve()
    current = root
    for part in relative.replace('\\', '/').split('/'):
        if part in ('', '.', '..'):
            raise RuntimeError(f'Invalid UI path: {relative}')
        if not current.is_dir():
            return None
        matches = [p for p in current.iterdir() if p.name.casefold() == part.casefold()]
        if not matches:
            return None
        if len(matches) != 1:
            raise RuntimeError(f'Ambiguous UI path: {relative}')
        current = matches[0]
        if not current.resolve().is_relative_to(root):
            raise RuntimeError(f'UI path points outside its directory: {relative}')
    return current if current.is_file() else None


def resolve_source(lekmod, source, manifest):
    sources = [manifest.get('aliases', {}).get(source, source), source]

    if source == 'ui/Popups/SocialPolicyPopup.lua':
        sources.append('ui/ToolTips/SocialPolicyPopup.lua')
    for candidate in dict.fromkeys(sources):
        for suffix in ('.ignore', ''):
            path = relative_path(Path(lekmod) / 'Lua/tmp', candidate + suffix)
            if path:
                return path
    raise MissingUISource(f'Missing UI source: {source}')


def file_pairs(entries):
    for entry in entries:
        source, target = (entry, entry.rsplit('/', 1)[-1]) if isinstance(entry, str) else entry
        if Path(target).name != target or '\\' in target or target in ('.', '..'):
            raise RuntimeError(f'Invalid UI destination: {target}')
        yield source, target


def selected_files(manifest, want_eui=False, eui_folder=None):

    for rule in manifest['rules']:
        entries = rule['files']
        if want_eui and 'eui' in rule:
            probe = relative_path(eui_folder, rule['eui']) if eui_folder else None
            if eui_folder is None:
                entries = rule.get('fallback_overlay', rule['overlay'])
            elif probe:
                if 'contains' not in rule or rule['contains'] in probe.read_text(
                        encoding='utf-8', errors='ignore'):
                    entries = rule['overlay']
                else:
                    entries = rule.get('fallback_overlay', entries)
        yield from file_pairs(entries)


def configure_ui(lekmod, want_eui=False, eui_folder=None, preserve_all=True, log=print, strict=True):
    lekmod = Path(lekmod)
    manifest = load_manifest(lekmod)
    ui = lekmod / 'Lua/UI'

    plan = []
    for source, target in selected_files(manifest, want_eui, eui_folder):
        try:
            plan.append((resolve_source(lekmod, source, manifest), target))
        except MissingUISource as error:
            if strict:
                raise
            log(str(error))
    preserved = {p.name: p.read_bytes() for p in ui.glob('*') if p.is_file()
                 and (preserve_all or p.name in manifest['preserve'])}
    if ui.exists():
        shutil.rmtree(ui)
    ui.mkdir(parents=True)
    for source, target in plan:
        shutil.copy2(source, ui / target)
    for name, data in preserved.items():
        if not (ui / name).exists():
            (ui / name).write_bytes(data)
    stamp_ui(lekmod)
    log(f'Configured {len(plan)} UI files in Lua/UI')


def stamp_ui(lekmod):
    lekmod = Path(lekmod)
    ui = lekmod / 'Lua/UI'
    for directory in (ui, lekmod / 'Lua/Utilities'):
        directory.mkdir(parents=True, exist_ok=True)
        (directory / 'LekmodUiConfigured.lua').write_bytes(b'LekmodUiConfigured = true\r\n')
    frontend = ui / 'FrontEnd.lua'
    if frontend.is_file():
        frontend.write_bytes(frontend.read_bytes().replace(
            b'local LEKMOD_UI_CHECK_DONE = false', b'local LEKMOD_UI_CHECK_DONE = true', 1))


def materialize_release(lekmod):

    lekmod = Path(lekmod)
    manifest = load_manifest(lekmod)
    for source, target in selected_files(manifest):
        if target in manifest['bootstrap']:
            destination = lekmod / 'Lua/UI' / target
            destination.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(resolve_source(lekmod, source, manifest), destination)
    for source in manifest.get('aliases', {}):
        original = resolve_source(lekmod, source, manifest)
        destination = lekmod / 'Lua/tmp' / (source + '.ignore')
        destination.parent.mkdir(parents=True, exist_ok=True)
        if original.resolve() != destination.resolve():
            shutil.copy2(original, destination)
    (lekmod / 'ui_check.bat').write_bytes(render_batch(lekmod, manifest).encode('utf-8'))


def render_batch(lekmod, manifest):

    lekmod = Path(lekmod).resolve()
    lines = ['@echo off', 'pushd "%~dp0"', 'set "patchfolder=%cd%"', 'cd ..',
             'set "euifolder=UI_bc1_xits"', 'if exist "UI_bc1" set "euifolder=UI_bc1"']
    for name in manifest['preserve']:
        lines.append(f'if exist "%patchfolder%\\Lua\\UI\\{name}" copy /y '
                     f'"%patchfolder%\\Lua\\UI\\{name}" "%patchfolder%\\{name}.bak" > nul')
    lines += ['if not exist "%patchfolder%\\Lua\\UI" mkdir "%patchfolder%\\Lua\\UI"',
              'del /q "%patchfolder%\\Lua\\UI\\"']

    def copies(entries, indent='  '):
        for source, target in file_pairs(entries):
            path = resolve_source(lekmod, source, manifest).relative_to(lekmod)
            path = str(path).replace('/', '\\')
            lines.append(f'{indent}copy /y "%patchfolder%\\{path}" "%patchfolder%\\Lua\\UI\\{target}" > nul')
        if not entries:
            lines.append(indent + 'ver > nul')

    for rule in manifest['rules']:
        if 'eui' not in rule:
            copies(rule['files'], '')
            continue
        probe = rule['eui'].replace('/', '\\')
        lines.append(f'if exist "%euifolder%\\{probe}" (')
        if 'contains' in rule:
            lines.append(f'  find "{rule["contains"]}" "%euifolder%\\{probe}" > nul 2>&1 && (')
            copies(rule['overlay'], '    ')
            lines.append('  ) || (')
            copies(rule.get('fallback_overlay', rule['files']), '    ')
            lines.append('  )')
        else:
            copies(rule.get('batch_overlay', rule['overlay']))
        lines.append(') else (')
        copies(rule['files'])
        lines.append(')')
    for name in manifest['preserve']:
        lines.append(f'if exist "%patchfolder%\\{name}.bak" move /y '
                     f'"%patchfolder%\\{name}.bak" "%patchfolder%\\Lua\\UI\\{name}" > nul')
    for directory in ('UI', 'Utilities'):
        lines.append(f'echo LekmodUiConfigured = true> "%patchfolder%\\Lua\\{directory}\\LekmodUiConfigured.lua"')
    lines += ['set "LEKMOD_FRONTEND=%patchfolder%\\Lua\\UI\\FrontEnd.lua"',
              'powershell -NoProfile -Command "$p=$env:LEKMOD_FRONTEND; '
              'if([IO.File]::Exists($p)){ $c=[IO.File]::ReadAllText($p); '
              "$n=$c.Replace('local LEKMOD_UI_CHECK_DONE = false','local LEKMOD_UI_CHECK_DONE = true'); "
              'if($n -ne $c){ [IO.File]::WriteAllText($p,$n) } }"', 'popd', 'exit /b']
    return '\r\n'.join(lines) + '\r\n'
