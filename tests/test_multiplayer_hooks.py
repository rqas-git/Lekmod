import os
import shutil
import subprocess
import unittest

from reference import ROOT


class MultiplayerHookTests(unittest.TestCase):
    def test_production_waits_for_network_acknowledgement(self):
        lua = os.environ.get('LUA51') or shutil.which('lua5.1')
        if not lua:
            self.skipTest('Requires Lua 5.1')
        result = subprocess.run([lua, str(ROOT / 'tests/multiplayer_production_scenarios.lua'),
                                 str(ROOT / 'skills/lekmod-test/assets/multiplayer-expanded-player.lua')],
                                capture_output=True, text=True, timeout=20)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
