"""Pin shared gameplay to the public release, independently of local presentation."""
from pathlib import Path
import subprocess
import unittest
import xml.etree.ElementTree as ET

from reference import ROOT, original
from source_text import canonical

STOCK = '201df6c56b41cc353ec01b2523bf852733d922e8'
CORE = 'LEKMOD_DLL/CvGameCoreDLL_Expansion2/'


def stock(path):
    return original(path, STOCK).decode('latin1')


def current(path):
    return (ROOT / path).read_bytes().decode('latin1')


def function(source, signature):
    start = source.index(signature)
    return source[start:source.index('\n}', start) + 2]


class StockCompatibilityTests(unittest.TestCase):
    def assert_stock(self, path, language='cpp'):
        self.assertEqual(canonical(current(path), language), canonical(stock(path), language), path)

    def test_civilizations_preserve_stock_logic_except_tested_query_optimizations(self):
        paths = subprocess.check_output(['git', 'ls-tree', '-r', '--name-only', STOCK,
                                         'LEKMOD/Lua/Civilizations'], cwd=ROOT, text=True).splitlines()
        optimized = {'Lekmod_uae.lua', 'Lekmod_kilwa.lua'}
        for path in paths:
            if path.endswith('.lua') and Path(path).name not in optimized:
                with self.subTest(path=path):
                    source = current(path)
                    if path.endswith('/Lekmod_nabatea.lua'):
                        source = source.replace('if not player then return end', '')
                        source = source.replace('if not unit_plot then return end', '')
                        source = source.replace('if not unit_id or unit_id:GetUnitType()', 'if unit_id:GetUnitType()')
                    self.assertEqual(canonical(source, 'lua'), canonical(stock(path), 'lua'))

    def test_draft_allocation_and_protocol_match_stock_peers(self):
        for name in ('Lekmod_drafter.lua', 'Lekmod_staging_draft.lua'):
            self.assert_stock('LEKMOD/Lua/Utilities/' + name, 'lua')

    def test_dummy_dispatch_and_policies_only_remove_idempotent_writes(self):
        path = 'LEKMOD/Lua/Lekmod_global_dummies.lua'
        source = current(path)
        self.assertEqual(source.count('break'), 1)
        self.assertEqual(canonical(source.replace('break', ''), 'lua'), canonical(stock(path), 'lua'))

    def test_native_ai_diplomacy_and_game_api_match_stock(self):
        for name in ('CvCitySpecializationAI.cpp', 'CvMilitaryAI.cpp', 'CvTeam.cpp', 'Lua/CvLuaGame.cpp'):
            self.assert_stock(CORE + name)

    def test_lake_freshwater_and_improvement_rules_match_stock(self):
        path = CORE + 'CvPlot.cpp'
        for signature in ('bool CvPlot::isPseudoLake(', 'void CvPlot::setPseudoLake(',
                          'bool CvPlot::isFreshWater(', 'void CvPlot::setFreshWater(',
                          'void CvPlot::setImprovementType(', 'void CvPlot::SetImprovementPillaged(',
                          'bool CvPlot::changeBuildProgress('):
            with self.subTest(signature=signature):
                self.assertEqual(canonical(function(current(path), signature)),
                                 canonical(function(stock(path), signature)))

    def test_v354_capture_contact_and_lake_yields_match_stock(self):
        methods = {
            'CvPlayer.cpp': ('void CvPlayer::acquireCity(',),
            'CvMinorCivAI.cpp': ('void CvMinorCivAI::DoFirstContactWithMajor(',),
            'CvGlobals.cpp': ('int CvGlobals::getNoDestructiveRecaptureTurns(',
                              'bool CvGlobals::isNoDestructiveRecaptureObsolete('),
            'CvPlot.cpp': ('int CvPlot::calculateNatureYield(', 'int CvPlot::calculateYield(',
                           'int CvPlot::getYieldWithBuild('),
        }
        for name, signatures in methods.items():
            for signature in signatures:
                with self.subTest(signature=signature):
                    self.assertEqual(canonical(function(current(CORE + name), signature)),
                                     canonical(function(stock(CORE + name), signature)))

    def test_v354_tithe_and_improvement_defaults_match_stock(self):
        path = CORE + 'CvBeliefClasses.cpp'
        def tithe(source):
            start = source.index('\t\tif (results != NULL)', source.index('std::string key("Belief_YieldChangePerXFollowers")'))
            return source[start:source.index('\n\t}\n#endif', start)]
        self.assertEqual(canonical(tithe(current(path))), canonical(tithe(stock(path))))
        line = 'kUtility.Initialize2DArray(m_piImprovementAdjacentBonusCivilization, 200, iImprovementTypes, -1);'
        for source in (current(CORE + 'CvImprovementClasses.cpp'), stock(CORE + 'CvImprovementClasses.cpp')):
            self.assertIn(line, source)

    def test_research_binding_preserves_stock_argument_behavior(self):
        self.assert_stock(CORE + 'Lua/CvLuaTeamTech.cpp')
        self.assertNotIn('Method(ChangeOverflowResearch);', current(CORE + 'Lua/CvLuaPlayer.cpp'))

    def test_connection_storage_and_serialization_match_stock(self):
        path = CORE + 'CvCityConnections.cpp'
        for signature in ('void CvCityConnections::Read(', 'void CvCityConnections::Write('):
            self.assertEqual(canonical(function(current(path), signature)),
                             canonical(function(stock(path), signature)))
        # This dimension is serialized, so even allocation growth must stay stock.
        for source in (stock(path), current(path)):
            self.assertIn('if(vpCities.size()>m_uiRouteInfosDimension){'
                          'ResizeRouteInfo((uint)((float)m_uiRouteInfosDimension*1.5f));}', canonical(source))

    def test_minor_civilization_database_matches_stock(self):
        def rows(text):
            root = ET.fromstring(text)
            for row in root.findall('Leaders/Row'):
                art = row.find('ArtDefineTag')
                if art is not None and art.text == 'LEKMOD_StaticLeaderScene.xml':
                    row.remove(art)
            atlases = root.find('IconTextureAtlases')
            for row in list(atlases):
                if row.findtext('Atlas') == 'LEKMOD_TUNISIA_PRIVATEER_FLAG':
                    atlases.remove(row)
            for row in root.findall('Units/Row'):
                if row.findtext('UnitFlagAtlas') == 'EXPANSION_UNIT_FLAG_ATLAS' and row.findtext('IconAtlas') == 'LEKMOD_TUNISIA_ATLAS':
                    row.find('UnitFlagAtlas').text = 'LEKMOD_TUNISIA_PRIVATEER_FLAG'
                    row.find('UnitFlagIconOffset').text = '0'
            for node in root.iter():
                node.text = (node.text or '').strip()
                node.tail = None
            return ET.tostring(root)
        path = 'LEKMOD/Override/CIV5Units.xml'
        self.assertEqual(rows(current(path)), rows(stock(path)))

    def test_consulates_votes_and_callbacks_match_stock(self):
        path = 'LEKMOD/Lua/Lekmod_policies.lua'
        marker = 'local function AwardConsulatesVotesForEra'
        self.assertEqual(canonical(current(path)[current(path).index(marker):], 'lua'),
                         canonical(stock(path)[stock(path).index(marker):], 'lua'))

    def test_teamer_keeps_stock_generator_dependencies(self):
        path = 'Lekmap/LekmapTeamerMapLegacy.lua'
        for module in ('HBMapGeneratorMirrored', 'HBFeatureGeneratorMirrored'):
            self.assertIn('include("' + module + '")', current(path))
            self.assertIn('include("' + module + '")', stock(path))
