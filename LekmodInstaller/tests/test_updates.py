"""Update regression tests using local fixtures; no GUI, network or batch execution."""
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
        # Execute the real worker without importing Tk or opening a window.
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
