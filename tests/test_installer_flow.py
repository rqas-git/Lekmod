from pathlib import Path
from types import SimpleNamespace
import sys
import tempfile
import unittest
from unittest.mock import Mock, patch
import zipfile

from reference import ROOT

try:
    import tkinter
except ImportError:
    sys.modules['tkinter'] = SimpleNamespace(**{
        name: Mock() for name in ('ttk', 'messagebox', 'scrolledtext', 'filedialog')})
sys.path.insert(0, str(ROOT / 'LekmodInstaller'))
from installer import LekmodInstaller
from ui_manager import UIManager


class InstallerFlowTests(unittest.TestCase):
    def make_installer(self, failure=None):
        app = LekmodInstaller.__new__(LekmodInstaller)
        callbacks = []
        app.root = SimpleNamespace(after=lambda delay, callback: callbacks.append(callback))
        for name in ('log', 'set_progress', 'show_indeterminate_progress', 'hide_progress',
                     '_set_action_buttons', 'check_installed_version'):
            setattr(app, name, Mock())
        app.config = {}
        app.updater = SimpleNamespace(get_available_versions=lambda **kw: {'v35.3': {'file_id': 'fixture'}})
        app.ui_manager = UIManager()
        app.ui_manager.remove_lekmod_folders = Mock()
        app.ui_manager.configure_ui_files = Mock(side_effect=RuntimeError('configure') if failure == 'configure' else None)
        app.ui_manager.install_mod = Mock(side_effect=OSError('install') if failure == 'install' else None)
        app.ui_manager.install_lekmap = Mock(return_value='maps/Lekmap_v6')
        temporary_paths = []
        def download(version, info, log, progress, filename_prefix, download_dir):
            directory = Path(download_dir)
            temporary_paths.append(directory)
            archive = directory / (filename_prefix + '.zip')
            if failure == 'download':
                archive.write_bytes(b'partial download')
                raise OSError('download')
            if failure == 'extract':
                archive.write_bytes(b'not a ZIP file')
            else:
                with zipfile.ZipFile(archive, 'w') as out:
                    out.writestr('LEKMOD/payload.txt', 'new release')
                    out.writestr('LEKMOD/Lua/tmp/fixture.lua.ignore', '-- fixture')
            return str(archive)
        app.downloader = SimpleNamespace(download_version_with_info=Mock(side_effect=download))
        return app, callbacks, temporary_paths

    def test_cleanup_and_main_thread_completion_on_each_failure(self):
        for stage in ('download', 'extract', 'configure', 'install'):
            with self.subTest(stage=stage), patch('installer.messagebox') as dialogs:
                app, callbacks, paths = self.make_installer(stage)
                app._run_install(lambda: app._install_thread('v35.3', 'Standard UI', 'game'))
                self.assertTrue(paths)
                self.assertTrue(all(not p.exists() for p in paths))
                dialogs.showerror.assert_not_called()
                app.ui_manager.remove_lekmod_folders.assert_not_called()
                self.assertEqual(len(callbacks), 1)
                callbacks[0]()
                dialogs.showerror.assert_called_once()
                app._set_action_buttons.assert_called_once_with(True)
                app.check_installed_version.assert_called_once()
                app.ui_manager.remove_lekmod_folders.assert_not_called()

    def test_success_and_lekmap_alias_share_the_same_lifecycle(self):
        for component in ('lekmod', 'lekmap'):
            with self.subTest(component=component), patch('installer.messagebox') as dialogs:
                app, callbacks, paths = self.make_installer()
                operation = (lambda: app._install_thread('v35.3', 'Standard UI', 'game')) if component == 'lekmod' else (
                    lambda: app._install_lekmap_thread('v6', 'captured game', {'Lekmap v6': {'file_id': 'fixture'}}))
                app._run_install(operation)
                self.assertTrue(all(not p.exists() for p in paths))
                dialogs.showinfo.assert_not_called()
                callbacks[0]()
                dialogs.showinfo.assert_called_once()
                dialogs.showerror.assert_not_called()
                if component == 'lekmap':
                    self.assertEqual(app.ui_manager.install_lekmap.call_args.kwargs['civ5_path'], 'captured game')
                    self.assertEqual(app.downloader.download_version_with_info.call_args.args[0], 'Lekmap v6')

    def test_worker_uses_transactional_replacement_and_cleans_failed_downloads(self):
        for failure in ('copy', 'commit', None):
            with self.subTest(failure=failure), tempfile.TemporaryDirectory() as game, patch('installer.messagebox') as dialogs:
                app, callbacks, paths = self.make_installer()
                old = Path(game) / 'Assets/DLC/LEKMOD_v1'
                old.mkdir(parents=True)
                (old / 'working.txt').write_text('working installation')
                app.ui_manager.install_mod = UIManager.install_mod.__get__(app.ui_manager, UIManager)

                app.install_path_var = SimpleNamespace(get=Mock(side_effect=AssertionError('Tk access from worker')))
                rename = Path.rename
                def move(source, target):
                    if failure == 'commit' and source.name == 'replacement':
                        raise OSError('commit failed')
                    return rename(source, target)
                with patch.object(Path, 'rename', move):
                    if failure == 'copy':
                        with patch('ui_manager.shutil.copytree', side_effect=OSError('copy failed')):
                            app._run_install(lambda: app._install_thread('v35.3', 'Standard UI', game))
                    else:
                        app._run_install(lambda: app._install_thread('v35.3', 'Standard UI', game))
                self.assertTrue(paths)
                self.assertTrue(all(not path.exists() for path in paths))
                app.ui_manager.remove_lekmod_folders.assert_not_called()
                dialogs.showerror.assert_not_called()
                dialogs.showinfo.assert_not_called()
                self.assertEqual(len(callbacks), 1)
                callbacks[0]()
                if failure:
                    self.assertEqual((old / 'working.txt').read_text(), 'working installation')
                    dialogs.showerror.assert_called_once()
                    self.assertIn(failure + ' failed', dialogs.showerror.call_args.args[1])
                else:
                    self.assertFalse(old.exists())
                    self.assertEqual((old.parent / 'LEKMOD_v35.3/payload.txt').read_text(), 'new release')
                    dialogs.showinfo.assert_called_once()

    def test_cancel_existing_installation_does_not_start_worker(self):
        app, _, _ = self.make_installer()
        with tempfile.TemporaryDirectory() as game, patch('installer.messagebox') as dialogs:
            app.version_var = SimpleNamespace(get=lambda: 'v35.3 - release')
            app.use_eui = SimpleNamespace(get=lambda: False)
            app.install_path_var = SimpleNamespace(get=lambda: game)
            app.ui_manager.find_existing_lekmod_folders = lambda path: ['LEKMOD_old']
            app._start_install = Mock()
            dialogs.askyesno.return_value = True
            dialogs.askyesnocancel.return_value = None
            app.install_update()
            app._start_install.assert_not_called()
            app.ui_manager.remove_lekmod_folders.assert_not_called()
