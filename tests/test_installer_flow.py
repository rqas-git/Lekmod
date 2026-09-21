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
sys.path.insert(0, str(ROOT / 'LekmodInstaller/tests'))
from payload_fixture import payload_files


class InstallerFlowTests(unittest.TestCase):
    def test_invalid_map_payload_preserves_working_installation(self):
        payloads = ({}, {'HBHelper.lua': 'helper'}, {'LekmapPangaea.lua': ''},
                    {'LekmapPangaea.lua': 'include("HBMissing")'},
                    {'LekmapPangaea.lua': 'include("HBHelper")', 'HBHelper.lua': ''})
        for payload in payloads:
            with self.subTest(payload=payload), tempfile.TemporaryDirectory() as temporary:
                root = Path(temporary)
                old = root / 'game/Assets/Maps/Lekmap v6.2'
                old.mkdir(parents=True)
                (old / 'LekmapPangaea.lua').write_text('working map')
                incoming = root / 'archive/Lekmap v6.2'
                incoming.mkdir(parents=True)
                for name, value in payload.items():
                    (incoming / name).write_text(value)
                with self.assertRaises(Exception):
                    UIManager().install_lekmap(str(root / 'archive'), 'v6.2', lambda _: None, str(root / 'game'))
                self.assertEqual((old / 'LekmapPangaea.lua').read_text(), 'working map')
                self.assertFalse(list((root / 'game/Assets').glob('.lekmap-install-*')))

    def test_map_payload_with_its_helpers_commits_successfully(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            incoming = root / 'archive/Lekmap v6.2'
            incoming.mkdir(parents=True)
            (incoming / 'LekmapPangaea.lua').write_text('include("HBHelper")')
            (incoming / 'HBHelper.lua').write_text('function Generate() end')
            installed = Path(UIManager().install_lekmap(str(root / 'archive'), 'v6.2', lambda _: None, str(root / 'game')))
            self.assertTrue((installed / 'HBHelper.lua').is_file())
            self.assertTrue((installed / 'Lekmap VERSION.txt').is_file())

    def test_invalid_map_label_shows_error_without_starting_worker(self):
        app, _, _ = self.make_installer()
        app.lekmap_version_var = SimpleNamespace(get=lambda: 'Lekmap v6.2/../../DLC')
        app.install_path_var = SimpleNamespace(get=lambda: 'game')
        app._start_install = Mock()
        with patch('installer.messagebox') as dialogs:
            app.install_lekmap_update()
            dialogs.showerror.assert_called_once()
            dialogs.askyesno.assert_not_called()
        app._start_install.assert_not_called()

    def test_version_error_callback_keeps_message_after_worker_returns(self):
        app, callbacks, _ = self.make_installer()
        app.refresh_btn = Mock()
        app.updater.get_available_versions = Mock(side_effect=RuntimeError('offline'))
        with patch('installer.tk', NORMAL='normal'), patch('installer.messagebox') as dialogs:
            app._check_updates_thread()
            self.assertEqual(len(callbacks), 2)
            for callback in callbacks:
                callback()
            dialogs.showerror.assert_called_once()
            self.assertIn('offline', dialogs.showerror.call_args.args[1])
            self.assertFalse(app._busy)

    def test_self_update_cancel_and_no_update_finish_on_main_thread(self):
        for update in (None, {'version': '2.0'}):
            with self.subTest(update=update), patch('installer.messagebox') as dialogs, patch('installer.threading.Thread') as worker:
                app, callbacks, _ = self.make_installer()
                app.installer_updater = Mock()
                app.installer_updater.check_for_installer_update.return_value = update
                dialogs.askyesno.return_value = False
                app.update_installer()
                self.assertTrue(app._busy)
                self.assertFalse(worker.call_args.kwargs['daemon'])
                app._update_installer_thread()
                dialogs.showinfo.assert_not_called()
                dialogs.askyesno.assert_not_called()
                self.assertTrue(app._busy)
                callbacks.pop(0)()
                self.assertFalse(app._busy)
                app.hide_progress.assert_called_once()
                app._set_action_buttons.assert_called_with(True)

    def test_install_self_update_and_close_share_exclusion(self):
        for first in ('install', 'update'):
            with self.subTest(first=first), patch('installer.threading.Thread') as worker, patch('installer.messagebox'):
                app, callbacks, _ = self.make_installer()
                app.root.destroy = Mock()
                if first == 'install':
                    app._start_install(lambda: None)
                else:
                    app.update_installer()
                self.assertTrue(app._busy)
                app._start_install(lambda: None)
                app.update_installer()
                app._on_close()
                worker.assert_called_once()
                app.root.destroy.assert_not_called()
                app._end_operation()
                app._on_close()
                app.root.destroy.assert_called_once()

    def test_self_update_errors_release_busy_state_and_do_not_exit(self):
        for stage in ('check', 'download', 'apply'):
            with self.subTest(stage=stage), patch('installer.messagebox') as dialogs:
                app, callbacks, _ = self.make_installer()
                app.installer_updater = Mock()
                app._begin_operation()
                method = {'check': 'check_for_installer_update', 'download': 'download_and_update', 'apply': 'apply_update'}[stage]
                getattr(app.installer_updater, method).side_effect = RuntimeError('fixture failure')
                if stage == 'check':
                    app._update_installer_thread()
                elif stage == 'download':
                    app._download_installer_update({'version': '2.0'})
                else:
                    app._apply_installer_update('fixture.cmd')
                for callback in callbacks:
                    callback()
                self.assertFalse(app._busy)
                app.hide_progress.assert_called_once()
                dialogs.showerror.assert_called_once()

    def test_all_action_buttons_include_self_update(self):
        app, _, _ = self.make_installer()
        for name in ('install_btn', 'refresh_btn', 'lekmap_install_btn', 'update_installer_btn'):
            setattr(app, name, Mock())
        app.lekmap_version_var = SimpleNamespace(get=lambda: 'v6.2')
        with patch('installer.tk', NORMAL='normal', DISABLED='disabled'):
            LekmodInstaller._set_action_buttons(app, False)
            for name in ('install_btn', 'refresh_btn', 'lekmap_install_btn', 'update_installer_btn'):
                getattr(app, name).config.assert_called_with(state='disabled')

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
                    for name, data in payload_files().items():
                        out.writestr('LEKMOD/' + name, data)
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
