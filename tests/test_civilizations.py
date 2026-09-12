import os
import shutil
import subprocess
import unittest

from reference import ROOT


class CivilizationTests(unittest.TestCase):
    def scenario(self, name):
        lua = os.environ.get('LUA51') or shutil.which('lua5.1')
        if not lua:
            self.skipTest('Set LUA51 to a Lua 5.1 interpreter or install lua5.1')
        result = subprocess.run([lua, str(ROOT / 'tests/civilization_scenarios.lua'),
                                 str(ROOT / 'LEKMOD/Lua/Civilizations'), name],
                                capture_output=True, text=True)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_italy_rewards_are_identical_for_local_remote_and_ai_players(self):
        self.scenario('italy')

    def test_new_zealand_science_preserves_overflow_and_selected_research(self):
        self.scenario('science')

    def test_defender_uses_player_ids_for_friends_and_city_radius(self):
        self.scenario('defender')

    def test_bolivia_founding_and_capture_preserve_great_person_choice(self):
        self.scenario('bolivia')

    def test_colorado_creation_and_happiness_refresh_agree(self):
        self.scenario('colorado')

    def test_mughal_capture_and_conversion_refresh_the_affected_player(self):
        self.scenario('mughals')
