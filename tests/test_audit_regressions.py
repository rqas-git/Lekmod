import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest
import xml.etree.ElementTree as ET

from reference import ROOT


def extract_function(source, signature):
    start = source.index(signature)
    # Native definitions end at column zero; alternative preprocessor branches
    # can contain duplicate opening braces, so raw brace counting is unsuitable.
    return source[start:source.index('\n}', start) + 2]


class AuditLuaTests(unittest.TestCase):
    def scenario(self, name):
        lua = os.environ.get('LUA51') or shutil.which('lua5.1')
        if not lua:
            self.skipTest('Set LUA51 to a Lua 5.1 interpreter')
        result = subprocess.run([lua, str(ROOT / 'tests/audit_scenarios.lua'), str(ROOT), name],
                                text=True, capture_output=True)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_older_core_science_survives_reload_and_is_awarded_once(self):
        self.scenario('science_legacy')

    def test_every_new_zealand_teammate_receives_first_contact_reward(self):
        self.scenario('team_reward')

    def test_duplicate_one_time_abilities_survive_context_reload(self):
        self.scenario('one_time_players')

    def test_bolivia_choices_are_independent_and_legacy_data_migrates(self):
        self.scenario('bolivia_players')

    def test_georgia_creation_golden_age_and_load_events(self):
        self.scenario('georgia_events')

    def test_mughal_holy_city_reward_requires_original_city(self):
        self.scenario('mughal_original_owner')

    def test_dummy_policy_preserves_free_choices_and_policy_cost(self):
        self.scenario('dummy_policies')

    def test_constrained_drafts_preserve_guarantees_and_uniqueness(self):
        self.scenario('draft_guarantees')

    def test_protocol_authority_validation_and_host_mediated_swaps(self):
        self.scenario('draft_protocol')

    def test_teamer_resolves_real_owned_helpers_and_mirrors_coordinates(self):
        self.scenario('teamer_includes')


class AuditNativeTests(unittest.TestCase):
    def test_lake_repair_overlapping_sources_and_cached_freshwater_yields(self):
        compiler = shutil.which('clang++') or shutil.which('c++')
        if not compiler:
            self.skipTest('A C++ compiler is required')
        plot = (ROOT / 'LEKMOD_DLL/CvGameCoreDLL_Expansion2/CvPlot.cpp').read_text()
        methods = '\n'.join(extract_function(plot, signature) for signature in (
            'bool CvPlot::isPseudoLake(', 'void CvPlot::setPseudoLake(',
            'bool CvPlot::isFreshWater(', 'void CvPlot::setFreshWater('))
        start = plot.index('#if defined(LEKMOD_BUGANDA_LAKE)', plot.index('void CvPlot::SetImprovementPillaged('))
        pillage = plot[start:plot.index('#endif', start) + len('#endif')]
        start = plot.index('#ifndef AUI_PLOT_FIX_PILLAGED_PLOT_ON_NEW_IMPROVEMENT',
                           plot.index('m_eImprovementType = eNewValue;'))
        creation = plot[start:plot.index('\t\tfor (iI = 0; iI < MAX_TEAMS;', start)]
        cpp = '''
#include <cassert>
#include <cstdlib>
#define LEKMOD_BUGANDA_LAKE
#define LEKMOD_NEW_LUA_METHODS
typedef int FeatureTypes; typedef int DirectionTypes;
const int NO_FEATURE=-1, NO_IMPROVEMENT=-1, NUM_DIRECTION_TYPES=6;
struct CvImprovementEntry {bool IsFreshWaterSource(){return true;}} improvement;
struct Feature {bool isAddsFreshWater(){return false;}} feature;
struct Globals {int getInfoTypeForString(const char*){return 7;} Feature* getFeatureInfo(int){return &feature;}
 CvImprovementEntry* getImprovementInfo(int){return &improvement;}} GC;
struct CvPlot {
 int x=0,y=0,yield=1,updates=0,improvementType=1; bool m_bPseudoLake=false,m_bIsSetFreshWater=false,river=false,lake=false;
 int getX()const{return x;} int getY()const{return y;} int getFeatureType()const{return NO_FEATURE;}
 int getImprovementType()const{return improvementType;}
 bool isSetFreshWater()const{return m_bIsSetFreshWater;} bool isWater()const{return false;}
 bool isImpassable()const{return false;} bool isMountain()const{return false;}
 bool isRiver()const{return river;} bool isLake()const{return lake;}
 bool isPseudoLake()const; void setPseudoLake(bool); bool isFreshWater()const; void setFreshWater(bool);
 void updateYield(){yield=isFreshWater()?2:1;++updates;} void pillage(bool);
 void SetImprovementPillaged(bool value){pillage(value);} void setImprovementType(int);
} plots[3][3];
CvPlot* plotXYWithRangeCheck(int x,int y,int dx,int dy,int range){
 if((std::abs(dx)+std::abs(dy)+std::abs(dx+dy))/2>range)return nullptr;
 x+=dx;y+=dy;return x<0||y<0||x>=3||y>=3?nullptr:&plots[x][y];
}
CvPlot* plotDirection(int x,int y,int direction){const int dx[]={1,0,-1,-1,0,1},dy[]={0,1,1,0,-1,-1};
 return plotXYWithRangeCheck(x,y,dx[direction],dy[direction],1);}
''' + methods + '\nvoid CvPlot::setImprovementType(int eNewValue){improvementType=eNewValue;\n' + creation + \
            '}\nvoid CvPlot::pillage(bool bPillaged){\n' + pillage + '''
}
int main(){
 for(int x=0;x<3;++x)for(int y=0;y<3;++y){plots[x][y].x=x;plots[x][y].y=y;}
 CvPlot& farm=plots[1][1]; CvPlot& first=plots[2][1]; CvPlot& second=plots[1][2];
 first.setImprovementType(1); assert(farm.isFreshWater()&&farm.yield==2);
 second.setPseudoLake(true); first.pillage(true); assert(!first.isPseudoLake()&&farm.yield==2);
 second.pillage(true); assert(!farm.isFreshWater()&&farm.yield==1);
 first.pillage(false); assert(first.isPseudoLake()&&farm.yield==2);
 farm.setFreshWater(true); first.pillage(true); assert(farm.isFreshWater()&&farm.yield==2);
 farm.setFreshWater(false); assert(!farm.isFreshWater()&&farm.yield==1);
 farm.river=true;farm.setFreshWater(true);farm.setFreshWater(false);assert(farm.isFreshWater()&&farm.yield==2);
 farm.river=false; first.setImprovementType(1);assert(farm.yield==2);
 first.setImprovementType(NO_IMPROVEMENT);assert(!farm.isFreshWater()&&farm.yield==1);
 assert(farm.updates>=8);
}
'''
        with tempfile.TemporaryDirectory() as temp:
            path=Path(temp)
            (path/'plot.cpp').write_text(cpp)
            for flags in ([], ['-DAUI_PLOT_FIX_PILLAGED_PLOT_ON_NEW_IMPROVEMENT']):
                build=subprocess.run([compiler,'-std=c++14',*flags,str(path/'plot.cpp'),'-o',str(path/'plot')],capture_output=True,text=True)
                self.assertEqual(build.returncode,0,build.stderr)
                run=subprocess.run([str(path/'plot')],capture_output=True,text=True,timeout=20)
                self.assertEqual(run.returncode,0,run.stderr)

    def test_native_lifetime_iteration_peace_and_war_guards(self):
        compiler = shutil.which('clang++') or shutil.which('c++')
        if not compiler:
            self.skipTest('A C++ compiler is required')
        core = ROOT / 'LEKMOD_DLL/CvGameCoreDLL_Expansion2'
        source = (core / 'CvCitySpecializationAI.cpp').read_text()
        methods = '\n'.join(extract_function(source, signature) for signature in (
            'CitySpecializationTypes CvCitySpecializationXMLEntries::GetFirstSpecializationForYield(',
            'CitySpecializationTypes CvCitySpecializationXMLEntries::GetNextSpecializationForYield('))
        unit = extract_function((core / 'Lua/CvLuaUnit.cpp').read_text(), 'int CvLuaUnit::lGetScriptData(')
        # Keep the production conditions; check all enum states and equal/unequal IDs.
        import re
        peace = re.search(r'if\(([^\n]*eMakingPeaceWithMinor[^\n]*eTeamWeMadePeaceWith)\)',
                          (core / 'CvTeam.cpp').read_text()).group(1)
        military = extract_function((core / 'CvMilitaryAI.cpp').read_text(),
                                    'bool MilitaryAIHelpers::IsTestStrategy_EradicateBarbarians(')
        war = re.search(r'if\(([^\n]*GetStateAllWars\(\)[^\n]*STATE_ALL_WARS_WINNING)\)', military).group(1)
        enum_source = (core / 'CvDiplomacyAIEnums.h').read_text()
        enum = re.search(r'enum StateAllWars[^{]*\{[^}]+\}', enum_source).group(0)
        cpp = '''
#include <cassert>
#include <string>
#include <vector>
typedef int CitySpecializationTypes; typedef int YieldTypes;
const int NO_CITY_SPECIALIZATION=-1;
struct Entry { int yield; int GetYieldType(){return yield;} };
struct CvCitySpecializationXMLEntries {
 int m_CurrentIndex=0,m_CurrentYield=0; std::vector<Entry*> m_paCitySpecializationEntries;
 int GetFirstSpecializationForYield(int); int GetNextSpecializationForYield();
};
struct lua_State{};
std::string result;
void lua_pushstring(lua_State*,const char* value){result=value;}
struct CvUnit {std::string data; std::string getScriptData(){return data;}} unit;
struct CvLuaUnit {static CvUnit* GetInstance(lua_State*){return &unit;} static int lGetScriptData(lua_State*);};
''' + enum + ''';
struct Player {int team,state; int getTeam(){return team;}
 Player* GetDiplomacyAI(){return this;} StateAllWars GetStateAllWars(){return (StateAllWars)state;}} player;
#define GET_PLAYER(id) player
''' + methods + '\n' + unit + '''
int main(){
 Entry food{0},a{1},b{1},c{1}; CvCitySpecializationXMLEntries entries;
 entries.m_paCitySpecializationEntries={&food,&a,&b,&c};
 assert(entries.GetFirstSpecializationForYield(1)==1);
 assert(entries.GetNextSpecializationForYield()==2); assert(entries.GetNextSpecializationForYield()==3);
 assert(entries.GetNextSpecializationForYield()==NO_CITY_SPECIALIZATION);
 lua_State lua; for(int length: {0,8,256,65536}){unit.data=std::string(length,'x');CvLuaUnit::lGetScriptData(&lua);assert(result==unit.data);}
 int eMakingPeaceWithMinor=1; int eTeamWeMadePeaceWith=24;
 for(int team: {0,1,24,25}){player.team=team;assert((''' + peace + ''')==(team!=24));}
 Player* pPlayer=&player;
 for(int state: {STATE_ALL_WARS_NEUTRAL,STATE_ALL_WARS_WINNING,STATE_ALL_WARS_LOSING}){player.state=state;assert((''' + war + ''')==(state!=STATE_ALL_WARS_WINNING));}
}
'''
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp)
            (path / 'native.cpp').write_text(cpp)
            lifetime_warnings = ['-Werror=dangling-gsl'] if 'clang' in Path(compiler).name else []
            build = subprocess.run([compiler, '-std=c++14', '-Wall', *lifetime_warnings,
                                    str(path / 'native.cpp'), '-o', str(path / 'native')], capture_output=True, text=True)
            self.assertEqual(build.returncode, 0, build.stderr)
            run = subprocess.run([str(path / 'native')], capture_output=True, text=True, timeout=20)
            self.assertEqual(run.returncode, 0, run.stderr)

    def test_personality_trait_references_resolve(self):
        database = ET.parse(ROOT / 'LEKMOD/Override/CIV5Units.xml').getroot()
        traits = {row.findtext('Type') for table in database.findall('MinorCivTraits') for row in table.findall('Row')}
        checked = 0
        for table in database:
            for row in table.findall('Row'):
                for field in ('RequiredMinorCivTrait', 'ForbiddenMinorCivTrait'):
                    value = row.findtext(field)
                    if value:
                        self.assertIn(value, traits, f'{table.tag}: {field}={value}')
                        checked += 1
        self.assertGreaterEqual(checked, 3)
