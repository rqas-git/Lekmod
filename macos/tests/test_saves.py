from contextlib import redirect_stdout
import io
import json
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

import saves


def write(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(data)


class SaveBrowserTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.folder = Path(temporary.name)
        self.root = self.folder / 'Saves'
        self.root.mkdir()

    def test_lists_only_local_saves_newest_first(self):
        older = self.root / 'single/old.Civ5Save'
        newer = self.root / 'multi/auto/new.Civ5Save'
        write(older, b'old')
        write(newer, b'new')
        write(self.root / 'single/notes.txt', b'not a save')
        (self.root / 'single/linked.Civ5Save').symlink_to(older)
        (self.root / 'pbem').symlink_to(self.folder, target_is_directory=True)
        import os
        os.utime(older, (10, 10))
        os.utime(newer, (20, 20))
        listed = saves.list_saves(self.root)
        self.assertEqual([item['path'] for item in listed],
                         ['multi/auto/new.Civ5Save', 'single/old.Civ5Save'])
        self.assertEqual([item['size'] for item in listed], [3, 3])
        self.assertEqual([item['modified_ns'] for item in listed], [20_000_000_000, 10_000_000_000])

    def test_backup_copies_bytes_without_changing_save_or_existing_destination(self):
        source = self.root / 'single/game.Civ5Save'
        write(source, b'game data')
        destination = self.folder / 'game-backup.Civ5Save'
        result = saves.copy_backup('single/game.Civ5Save', destination, self.root)
        self.assertEqual(destination.read_bytes(), b'game data')
        self.assertEqual(source.read_bytes(), b'game data')
        self.assertEqual(result['size'], 9)
        with self.assertRaisesRegex(RuntimeError, 'changed since'):
            saves.copy_backup('single/game.Civ5Save', self.folder / 'stale.Civ5Save', self.root,
                              expected_size=8)
        self.assertFalse((self.folder / 'stale.Civ5Save').exists())
        with self.assertRaisesRegex(ValueError, 'already exists'):
            saves.copy_backup('single/game.Civ5Save', destination, self.root)
        self.assertEqual(destination.read_bytes(), b'game data')
        self.assertEqual(list(self.folder.glob('.lekmod-save-*')), [])

    def test_rejects_unsafe_sources_and_game_folder_destinations(self):
        source = self.root / 'single/game.Civ5Save'
        write(source, b'game data')
        (self.root / 'multi').symlink_to(self.root / 'single', target_is_directory=True)
        (self.root / 'single/linked.Civ5Save').symlink_to(source)
        for relative in ('../outside.Civ5Save', '/single/game.Civ5Save',
                         'multi/game.Civ5Save', 'single/linked.Civ5Save'):
            with self.subTest(relative=relative), self.assertRaises(ValueError):
                saves.copy_backup(relative, self.folder / 'backup.Civ5Save', self.root)
        with self.assertRaisesRegex(ValueError, 'outside the game'):
            saves.copy_backup('single/game.Civ5Save', self.root / 'single/backup.Civ5Save', self.root)
        self.assertFalse((self.root / 'single/backup.Civ5Save').exists())

    def test_command_line_lists_then_backs_up_selected_entry(self):
        write(self.root / 'single/game.Civ5Save', b'game data')
        with patch.object(saves, 'SAVE_ROOT', self.root):
            output = io.StringIO()
            with redirect_stdout(output):
                self.assertEqual(saves.main(['list']), 0)
            selected = json.loads(output.getvalue())['saves'][0]
            destination = self.folder / 'export.Civ5Save'
            output = io.StringIO()
            with redirect_stdout(output):
                self.assertEqual(saves.main(['backup', selected['path'], str(destination),
                                             str(selected['size']), str(selected['modified_ns'])]), 0)
            self.assertEqual(json.loads(output.getvalue())['destination'], str(destination))
            self.assertEqual(destination.read_bytes(), b'game data')
