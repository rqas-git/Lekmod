
from pathlib import Path
import ast
import os
import sys
import tempfile
import unittest
from unittest.mock import MagicMock, patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import installer_updater
from ui_manager import UIManager
from payload_fixture import payload_files


class InstallTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.game = self.root / 'game'
        self.dlc = self.game / 'Assets/DLC'
        self.old = self.dlc / 'LEKMOD_v1'
        self.old.mkdir(parents=True)
        (self.old / 'old.txt').write_text('working')
        self.source = self.root / 'source'
        self.source.mkdir()
        (self.source / 'new.txt').write_text('replacement')
        for name, data in payload_files().items():
            path = self.source / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(data)
        self.manager = UIManager()
        self.manager._find_lekmod_folder = lambda _: str(self.source)

    def install(self, version='v2'):
        self.manager.install_mod(str(self.source), str(self.game), version, lambda _: None)

    def test_success_replaces_existing_and_same_version(self):
        self.install()
        self.install()
        self.assertFalse(self.old.exists())
        self.assertEqual((self.dlc / 'LEKMOD_v2/new.txt').read_text(), 'replacement')

    def test_copy_failure_preserves_working_installation(self):
        with patch('ui_manager.shutil.copytree', side_effect=OSError('disk full')):
            with self.assertRaises(OSError):
                self.install()
        self.assertEqual((self.old / 'old.txt').read_text(), 'working')

    def test_catalog_failure_preserves_working_installation(self):

        tree = ast.parse((Path(__file__).resolve().parents[1] / 'installer.py').read_text())
        cls = next(n for n in tree.body if isinstance(n, ast.ClassDef) and n.name == 'LekmodInstaller')
        method = next(n for n in cls.body if isinstance(n, ast.FunctionDef) and n.name == '_install_thread')
        scope = {'os': os, 'messagebox': MagicMock()}
        scope['messagebox'].askyesnocancel.return_value = True
        exec(compile(ast.Module(body=[method], type_ignores=[]), 'installer.py', 'exec'), scope)
        worker = MagicMock()
        worker.ui_manager = self.manager
        worker.install_path_var.get.return_value = str(self.game)
        worker.updater.get_available_versions.side_effect = RuntimeError('offline')
        with self.assertRaisesRegex(RuntimeError, 'offline'):
            scope['_install_thread'](worker, 'v2', 'standard', str(self.game))
        self.assertEqual((self.old / 'old.txt').read_text(), 'working')

    def test_commit_failure_restores_all_previous_versions(self):
        other = self.dlc / 'LEKMOD_v0'
        other.mkdir()
        (other / 'keep.txt').write_text('older')
        rename = Path.rename

        def fail_commit(path, target):
            if path.name == 'replacement':
                raise OSError('commit failed')
            return rename(path, target)

        with patch.object(Path, 'rename', fail_commit), self.assertRaises(OSError):
            self.install()
        self.assertTrue((self.old / 'old.txt').exists())
        self.assertTrue((other / 'keep.txt').exists())
        self.assertFalse((self.dlc / 'LEKMOD_v2').exists())

    def test_failed_restore_keeps_recoverable_backup(self):
        rename = Path.rename

        def fail_commit_and_restore(path, target):
            if path.parent.name.startswith('.lekmod-install-'):
                raise OSError('commit or restore failed')
            return rename(path, target)

        with patch.object(Path, 'rename', fail_commit_and_restore):
            with self.assertRaisesRegex(RuntimeError, 'Previous files are saved'):
                self.install()
        self.assertEqual(len(list(self.dlc.parent.glob('.lekmod-install-*/LEKMOD_v1/old.txt'))), 1)

    def test_unsafe_versions_are_rejected_before_writes(self):
        for version in ('v/../../../escaped', r'v\..\escaped', 'v2:stream', 'v2.', '', None):
            with self.subTest(version=version), self.assertRaises(ValueError):
                self.install(version)
        self.assertTrue((self.old / 'old.txt').exists())
        self.assertEqual(list(self.dlc.iterdir()), [self.old])

    def test_missing_empty_and_invalid_payloads_preserve_working_installation(self):
        for name in payload_files():
            if name.startswith('Lua/tmp'):
                continue
            path = self.source / name
            original = path.read_bytes()
            for contents in (None, b'', b'invalid'):
                if contents == b'invalid' and name.endswith('.lua'):
                    continue
                with self.subTest(name=name, contents=contents):
                    if contents is None:
                        path.unlink()
                    else:
                        path.write_bytes(contents)
                    with self.assertRaises(RuntimeError):
                        self.install()
                    self.assertEqual((self.old / 'old.txt').read_text(), 'working')
                    self.assertFalse((self.dlc / 'LEKMOD_v2').exists())
                    self.assertFalse(list(self.dlc.parent.glob('.lekmod-install-*')))
                    path.write_bytes(original)

    def test_empty_legacy_package_is_rejected_after_ui_configuration(self):
        import shutil
        shutil.rmtree(self.source)
        (self.source / 'Lua/tmp').mkdir(parents=True)
        self.manager.configure_ui_files(str(self.source), 'Standard UI', lambda _: None)
        with self.assertRaisesRegex(RuntimeError, 'Incomplete Lekmod package'):
            self.install()
        self.assertEqual((self.old / 'old.txt').read_text(), 'working')

    def test_modern_package_rejects_missing_selected_ui_source(self):
        import json
        (self.source / 'ui_manifest.json').write_text(json.dumps({
            'format': 1, 'preserve': [], 'rules': [{'files': ['ui/missing.lua']}]}))
        with self.assertRaisesRegex(RuntimeError, 'Missing UI source'):
            self.manager.configure_ui_files(str(self.source), 'Standard UI', lambda _: None)
        self.assertEqual((self.old / 'old.txt').read_text(), 'working')


class MapInstallTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.game = self.root / 'game'
        self.maps = self.game / 'Assets/Maps'
        self.old = self.maps / 'Lekmap v6.2'
        self.old.mkdir(parents=True)
        (self.old / 'old.lua').write_text('working')
        self.source = self.root / 'download'
        self.source.mkdir()
        (self.source / 'LekmapPangaea.lua').write_text('new')
        self.manager = UIManager()

    def install(self, version='Lekmap v6.2'):
        return self.manager.install_lekmap(str(self.source), version, lambda _: None, str(self.game))

    def test_unsafe_versions_do_not_modify_any_files(self):
        dlc = self.game / 'Assets/DLC'
        dlc.mkdir()
        (dlc / 'expansion').write_text('keep')
        for version in ('Lekmap v6.2/../../DLC', r'v6.2\..\..\DLC', 'v2:stream', 'v2.', '', None):
            with self.subTest(version=version), self.assertRaises(ValueError):
                self.install(version)
        self.assertEqual((self.old / 'old.lua').read_text(), 'working')
        self.assertEqual((dlc / 'expansion').read_text(), 'keep')
        self.assertEqual(list(self.maps.iterdir()), [self.old])

    def test_copy_and_stamp_failures_preserve_old_files(self):
        for target in ('ui_manager.shutil.copytree', 'pathlib.Path.write_text'):
            with self.subTest(target=target), patch(target, side_effect=OSError('disk full')):
                with self.assertRaises(OSError):
                    self.install()
            self.assertEqual((self.old / 'old.lua').read_text(), 'working')
            self.assertFalse(list(self.maps.parent.glob('.lekmap-install-*')))

    def test_failed_commit_restores_old_files(self):
        rename = Path.rename
        def fail(path, target):
            if path.name == 'replacement':
                raise OSError('commit failed')
            return rename(path, target)
        with patch.object(Path, 'rename', fail), self.assertRaises(OSError):
            self.install()
        self.assertEqual((self.old / 'old.lua').read_text(), 'working')
        self.assertFalse(list(self.maps.parent.glob('.lekmap-install-*')))

    def test_failed_rollback_keeps_backup(self):
        rename = Path.rename
        def fail(path, target):
            if path.name in ('replacement', 'previous'):
                raise OSError('rename failed')
            return rename(path, target)
        with patch.object(Path, 'rename', fail), self.assertRaisesRegex(RuntimeError, 'Previous files are saved'):
            self.install()
        backups = list(self.maps.parent.glob('.lekmap-install-*/previous/old.lua'))
        self.assertEqual(len(backups), 1)
        self.assertEqual(backups[0].read_text(), 'working')

    def test_success_replaces_same_version_and_preserves_other_maps(self):
        (self.maps / 'other.lua').write_text('keep')
        for version in ('6.2', 'v6.2', 'Lekmap_v6.2'):
            self.assertEqual(Path(self.install(version)), self.old)
            self.assertEqual((self.old / 'LekmapPangaea.lua').read_text(), 'new')
            self.assertEqual((self.old / 'Lekmap VERSION.txt').read_text(), 'Lekmap v6.2\n')
        self.assertFalse((self.old / 'old.lua').exists())
        self.assertEqual((self.maps / 'other.lua').read_text(), 'keep')
        self.assertFalse(list(self.maps.parent.glob('.lekmap-install-*')))

    def test_linked_destination_is_rejected(self):
        target = self.root / 'outside'
        target.mkdir()
        link = self.maps / 'Lekmap v7'
        try:
            link.symlink_to(target, target_is_directory=True)
        except OSError:
            self.skipTest('Symbolic links are unavailable')
        with self.assertRaises(ValueError):
            self.install('v7')
        self.assertTrue(link.is_symlink())
        self.assertEqual(list(target.iterdir()), [])


class SelfUpdateTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.current = self.root / 'installer.exe'
        self.current.write_bytes(b'original')
        for name, value in (('frozen', True), ('executable', str(self.current))):
            patcher = patch.object(installer_updater.sys, name, value, create=True)
            patcher.start()
            self.addCleanup(patcher.stop)

    def download(self, body, http_error=None, length=None):
        response = MagicMock()
        response.__enter__.return_value = response
        response.url = 'https://example.invalid/installer.exe'
        response.raise_for_status.side_effect = http_error
        response.headers = {'content-length': str(len(body) if length is None else length)}
        response.iter_content.return_value = [body]
        with patch.object(installer_updater.requests, 'get', return_value=response):
            return installer_updater.InstallerUpdater({}).download_and_update(
                {'download_url': response.url}, lambda _: None)

    def test_http_error_html_and_truncated_download_are_rejected(self):
        cases = [(b'not found', RuntimeError('HTTP 404'), None),
                 (b'<html>error</html>', None, None), (b'MZ', None, 100),
                 (b'MZ' + bytes(62), None, None)]
        for body, error, length in cases:
            with self.subTest(body=body), self.assertRaises((ValueError, RuntimeError)):
                self.download(body, error, length)
            self.assertEqual(self.current.read_bytes(), b'original')
            self.assertFalse(list(self.root.glob('LekmodInstaller-update-*')))

    def test_pe_payload_is_staged_without_touching_current_executable(self):
        body = bytearray(128)
        body[:2] = b'MZ'
        body[60:64] = (64).to_bytes(4, 'little')
        body[64:68] = b'PE\0\0'
        script, downloaded = self.download(bytes(body))
        self.assertEqual(Path(downloaded).read_bytes(), body)
        self.assertTrue(Path(script).exists())
        self.assertEqual(self.current.read_bytes(), b'original')

    def test_source_mode_cannot_replace_python_source(self):
        with patch.object(installer_updater.sys, 'frozen', False):
            with self.assertRaisesRegex(RuntimeError, 'packaged Windows installer'):
                self.download(b'MZ')


if __name__ == '__main__':
    unittest.main()
