import hashlib
import importlib.util
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest

from reference import ROOT, extract_lua, original as reference_original
from source_text import canonical


BASELINE = '41a1d154'


def original(path):
    return reference_original(path, BASELINE)


sys.path.insert(0, str(ROOT / 'LekmodInstaller'))
from ui_assets import configure_ui, load_manifest, materialize_release, resolve_source
sys.path.insert(0, str(ROOT / 'tools'))
from package_lekmod import package


def normalized(path):
    language = 'xml' if '.xml' in path.suffixes else 'lua'
    return canonical(path.read_bytes().decode('latin1'), language).encode('latin1')


def contents(folder):
    return {p.name: hashlib.sha256(normalized(p)).hexdigest()
            for p in folder.iterdir() if p.is_file()}


class UIAssetsTests(unittest.TestCase):
    def assert_ui_matches_previous(self, old, new):
        before, after = contents(old), contents(new)
        self.assertEqual(set(after) - set(before),
                         {'DiploRelationships.lua', 'CityStateGreetingPopup.lua'} & set(after))
        self.assertFalse(set(before) - set(after))
        for name in ('DiploRelationships.lua', 'CityStateGreetingPopup.lua'):
            after.pop(name, None)
        for name in ('CityBannerManager.xml', 'NotificationPanel.xml', 'TechPopup.xml',
                     'TopPanel.lua', 'CultureOverview.lua', 'MPGameOptions.lua',
                     'VictoryProgress.lua', 'EUI_tooltip_library.lua'):
            before.pop(name, None)
            after.pop(name, None)
        helper = 'TradeRouteHelpers.lua'
        self.assertIn(helper, before)
        before.pop(helper)
        after.pop(helper)
        self.assertEqual(before, after)

        # The one deliberate presentation change colors the two city headers.
        old_text = (old / helper).read_text()
        new_text = (new / helper).read_text()
        for name in ('header1', 'header2'):
            old_line = next(line for line in old_text.splitlines()
                            if line.lstrip().startswith('local ' + name + ' ='))
            new_line = next(line for line in new_text.splitlines()
                            if line.lstrip().startswith('local ' + name + ' ='))
            self.assertEqual(new_line,
                             old_line.replace('= string.format(', '= "[COLOR_YELLOW]" .. string.format(')
                             + ' .. "[ENDCOLOR]"')
            new_text = new_text.replace(new_line, old_line, 1)
        self.assertEqual(canonical(old_text, 'lua'), canonical(new_text, 'lua'))

    @classmethod
    def setUpClass(cls):
        cls.temporary = tempfile.TemporaryDirectory()
        cls.addClassCleanup(cls.temporary.cleanup)
        cls.reference = Path(cls.temporary.name)
        extract_lua(cls.reference, BASELINE)
        for name in ('StagingRoom.lua', 'StagingRoom.xml', 'EnemyUnitPanel.lua'):
            path = 'LEKMOD/Lua/tmp/ui/' + ('UnitPanel/' if name == 'EnemyUnitPanel.lua' else 'Lobby/') + name + '.ignore'
            (cls.reference / path).write_bytes(reference_original(path, '201df6c56b41cc353ec01b2523bf852733d922e8'))
        module_path = cls.reference / 'original_ui_manager.py'
        module_path.write_bytes(original('LekmodInstaller/ui_manager.py'))
        spec = importlib.util.spec_from_file_location('original_ui_manager', module_path)
        cls.old = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(cls.old)

    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.mod = self.root / 'new/LEKMOD'
        self.mod.mkdir(parents=True)
        shutil.copytree(ROOT / 'LEKMOD/Lua', self.mod / 'Lua')
        shutil.copy2(ROOT / 'LEKMOD/ui_manifest.json', self.mod)
        shutil.copy2(ROOT / 'LEKMOD/CvGameCore_Expansion2.dll', self.mod)
        self.manifest = load_manifest(self.mod)

    def eui_pack(self, game, variant):
        if variant == 'missing':
            return None
        eui = game / 'Assets/DLC/UI_bc1'
        eui.mkdir(parents=True)
        for rule in self.manifest['rules']:
            if 'eui' not in rule or variant == 'empty':
                continue
            if variant == 'partial' and not rule['eui'].startswith(('TechTree/', 'Core/')):
                continue
            path = eui / rule['eui']
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(rule.get('contains', '') if variant == 'markers' else 'older EUI')
        return eui

    def test_selection_matches_previous_installer(self):
        for variant in ('standard', 'missing', 'empty', 'markers', 'legacy', 'partial'):
            with self.subTest(variant=variant):
                old = self.root / variant / 'old/LEKMOD'
                new = self.root / variant / 'new/LEKMOD'
                shutil.copytree(self.reference / 'LEKMOD', old)
                shutil.copytree(self.mod, new)
                for mod in (old, new):
                    (mod / 'Lua/UI/custom.lua').write_text('preserved')
                eui = self.eui_pack(self.root / variant / 'game', variant)
                want_eui = variant != 'standard'
                self.old.UIManager().configure_ui_files(str(old),
                    'Enhanced UI' if want_eui else 'Standard UI', lambda _: None,
                    civ5_path=str(self.root / variant / 'game'))
                configure_ui(new, want_eui, eui, log=lambda _: None)
                self.assert_ui_matches_previous(old / 'Lua/UI', new / 'Lua/UI')

    def test_personality_contexts_follow_eui_ownership(self):
        configure_ui(self.mod, log=lambda _: None)
        ui = self.mod / 'Lua/UI'
        for name in ('DiploRelationships.lua', 'CityStateGreetingPopup.lua'):
            self.assertIn('GetMinorCivPersonalityDisplayText(pPlayer)', (ui / name).read_text())
        pack = self.root / 'UI_bc1/CityStatePopup'
        pack.mkdir(parents=True)
        (pack / 'CityStateGreetingPopup.lua').write_text('')
        configure_ui(self.mod, True, pack.parent, log=lambda _: None)
        self.assertFalse((ui / 'CityStateGreetingPopup.lua').exists())
        self.assertTrue((ui / 'DiploRelationships.lua').exists())

    def test_older_archive_uses_original_paths(self):
        old = self.root / 'old/LEKMOD'
        shutil.copytree(self.reference / 'LEKMOD', old)
        manifest = load_manifest(old)
        for source in manifest['aliases']:
            self.assertEqual(resolve_source(old, source, manifest).read_bytes(),
                             (old / 'Lua/tmp' / (source + '.ignore')).read_bytes())

    def test_materialized_package_supports_legacy_installers(self):
        materialize_release(self.mod)
        for source in self.manifest['aliases']:
            target = self.mod / 'Lua/tmp' / (source + '.ignore')
            self.assertEqual(normalized(target),
                             normalized(self.reference / 'LEKMOD/Lua/tmp' / (source + '.ignore')))
        before = contents(self.mod / 'Lua/UI')
        materialize_release(self.mod)
        self.assertEqual(before, contents(self.mod / 'Lua/UI'))
        self.assertIn(b'local LEKMOD_UI_CHECK_DONE = false', (self.mod / 'Lua/UI/FrontEnd.lua').read_bytes())
        self.assertTrue(all((self.mod / 'Lua/UI' / name).is_file() for name in self.manifest['bootstrap']))

    def test_packaging_ignores_configured_ui_and_stale_aliases(self):
        materialize_release(self.mod)
        configure_ui(self.mod, want_eui=True, log=lambda _: None)
        alias = self.mod / 'Lua/tmp/ui/CultureOverview/EconomicGeneralInfo.lua.ignore'
        alias.write_text('stale generated copy')
        destination = package(self.mod, self.root / 'package/LEKMOD')
        self.assertEqual(set(contents(destination / 'Lua/UI')),
                         set(self.manifest['bootstrap'] + self.manifest['preserve']))
        self.assertNotIn(b'local LEKMOD_UI_CHECK_DONE = true', (destination / 'Lua/UI/FrontEnd.lua').read_bytes())
        self.assertNotEqual((destination / alias.relative_to(self.mod)).read_text(), 'stale generated copy')
        self.assertFalse((destination / 'Lua/Utilities/LekmodUiConfigured.lua').exists())
        self.assertTrue((destination / 'ui_check.bat').is_file())

    def test_modified_or_missing_dll_is_rejected_before_package_creation(self):
        library = self.mod / 'CvGameCore_Expansion2.dll'
        for data in (b'MZ old GetOverflowResearch', b'MZ ChangeOverflowResearch GetLekmodCoreVersion', None):
            if data is None:
                library.unlink()
            else:
                library.write_bytes(data)
            destination = self.root / 'invalid-package'
            with self.assertRaisesRegex(RuntimeError, 'DLL'):
                package(self.mod, destination)
            self.assertFalse(destination.exists())

    def test_official_release_override_is_the_binary_in_the_package(self):
        (self.mod / 'CvGameCore_Expansion2.dll').write_bytes(b'MZ old')
        library = self.root / 'official.dll'
        shutil.copy2(ROOT / 'LEKMOD/CvGameCore_Expansion2.dll', library)
        destination = package(self.mod, self.root / 'release', library)
        self.assertEqual((destination / 'CvGameCore_Expansion2.dll').read_bytes(), library.read_bytes())

    def test_missing_source_does_not_clear_existing_ui(self):
        before = contents(self.mod / 'Lua/UI')
        (self.mod / 'Lua/tmp/ui/FrontEnd/FrontEnd.lua.ignore').unlink()
        with self.assertRaisesRegex(RuntimeError, 'Missing UI source'):
            configure_ui(self.mod)
        self.assertEqual(before, contents(self.mod / 'Lua/UI'))

    def test_macos_preserves_only_standalone_files(self):
        (self.mod / 'Lua/UI/old-eui.lua').write_text('remove')
        configure_ui(self.mod, preserve_all=False, log=lambda _: None)
        self.assertFalse((self.mod / 'Lua/UI/old-eui.lua').exists())
        self.assertTrue((self.mod / 'Lua/UI/LegalScreen.lua').exists())

    @unittest.skipUnless(os.name == 'nt', 'Requires the Windows batch/PowerShell entry point')
    def test_batch_matches_previous_batch(self):
        for variant in ('missing', 'empty', 'markers', 'legacy', 'partial'):
            with self.subTest(variant=variant):
                old = self.root / variant / 'old/LEKMOD'
                new = self.root / variant / 'new/LEKMOD'
                shutil.copytree(self.reference / 'LEKMOD', old)
                shutil.copytree(self.mod, new)
                (old / 'ui_check.bat').write_bytes(original('LEKMOD/ui_check.bat'))
                materialize_release(new)
                for mod in (old, new):
                    eui = self.eui_pack(mod.parent, variant)
                    if eui:
                        eui.rename(mod.parent / 'UI_bc1')
                    result = subprocess.run(['cmd', '/c', str(mod / 'ui_check.bat')],
                                            capture_output=True, text=True)
                    self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assert_ui_matches_previous(old / 'Lua/UI', new / 'Lua/UI')
