from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

from reference import ROOT


CORE = ROOT / 'LEKMOD_DLL/CvGameCoreDLL_Expansion2'


def method(path, signature):
    source = (CORE / path).read_text(encoding='latin1')
    start = source.index(signature)
    return source[start:source.index('\n}', start) + 2]


class ReadOnlyQueryTests(unittest.TestCase):
    def compile_and_run(self, source):
        compiler = shutil.which('clang++') or shutil.which('c++')
        if not compiler:
            self.skipTest('A C++ compiler is required')
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory)
            (path / 'queries.cpp').write_text(source)
            build = subprocess.run([compiler, '-std=c++14', '-fsanitize=undefined',
                                    '-fno-sanitize-recover=undefined', str(path / 'queries.cpp'),
                                    '-o', str(path / 'queries')], capture_output=True, text=True)
            self.assertEqual(build.returncode, 0, build.stderr)
            run = subprocess.run([str(path / 'queries')], capture_output=True, text=True, timeout=20)
            self.assertEqual(run.returncode, 0, run.stderr)

    def test_tourism_without_a_capital_keeps_the_unmodified_yield(self):
        body = method('CvCultureClasses.cpp', 'int CvPlayerCulture::GetInfluencePerTurn(PlayerTypes')
        self.compile_and_run(r'''
#include <cassert>
#include <cstddef>
#define LEKMOD_GREAT_FIREWALL_PLAYER_EFFECT
#define STANDARDIZE_YIELDS
using PlayerTypes = int;
using TechTypes = int;
const int YIELD_TOURISM = 7;
struct CityCulture {
    int GetTourismMultiplier(int, bool, bool, bool, bool, bool) const { return 33; }
};
struct City {
    CityCulture culture;
    const CityCulture* GetCityCulture() const { return &culture; }
};
struct Player {
    int id;
    bool alive = true, minor = false;
    City* capital = nullptr;
    int yield = 0, cityYield = 0;
    int GetID() const { return id; }
    int getTeam() const { return id; }
    bool isAlive() const { return alive; }
    bool isMinorCiv() const { return minor; }
    bool IsNullifyInfluenceModifier() const { return false; }
    City* getCapitalCity() const { return capital; }
    int getYieldTimes100(int, bool) const { return yield; }
    int getYieldFromCitiesTimes100(int, bool) const { return cityYield; }
};
struct Techs { bool HasTech(int) const { return false; } };
struct CvTeam {
    bool met = true;
    Techs techs;
    bool isHasMet(int) const { return met; }
    const Techs* GetTeamTechs() const { return &techs; }
};
struct Globals { int getInfoTypeForString(const char*) const { return 0; } } GC;
using CvPlayer = Player;
using CvCity = City;
Player players[] = {{0}, {1}};
CvTeam teams[2];
#define GET_PLAYER(id) players[id]
#define GET_TEAM(id) teams[id]
struct CvPlayerCulture {
    Player* m_pPlayer;
    int GetInfluencePerTurn(PlayerTypes) const;
};
''' + body + r'''
int main() {
    CvPlayerCulture culture{&players[0]};
    assert(culture.GetInfluencePerTurn(1) == 0);
    players[0].yield = 1000;
    assert(culture.GetInfluencePerTurn(1) == 10);
    City capital;
    players[0].capital = &capital;
    assert(culture.GetInfluencePerTurn(1) == 13);
    assert(culture.GetInfluencePerTurn(0) == 0);
    teams[1].met = false;
    assert(culture.GetInfluencePerTurn(1) == 0);
    teams[1].met = true;
    players[1].alive = false;
    assert(culture.GetInfluencePerTurn(1) == 0);
    players[1].alive = true;
    players[1].minor = true;
    assert(culture.GetInfluencePerTurn(1) == 0);
}
''')

    def test_observer_without_leader_has_no_traits(self):
        body = method('CvTraitClasses.cpp', 'bool CvPlayerTraits::HasTrait(TraitTypes')
        self.compile_and_run(r'''
#include <cassert>
#include <cstddef>
#define CvAssert(x) ((void)0)
#define CvAssertMsg(x, message) ((void)0)
using TraitTypes = int;
struct Leader { bool hasTrait(int) const { return true; } };
struct Player {
    int leader;
    int getLeaderType() const { return leader; }
    int getTeam() const { return 0; }
    Leader getLeaderInfo() const { assert(leader >= 0); return {}; }
};
struct Trait {
    bool obsolete, enabled;
    bool IsObsoleteByTech(int) const { return obsolete; }
    bool IsEnabledByTech(int) const { return enabled; }
};
struct Traits { Trait trait; Trait* GetEntry(int) { return &trait; } };
struct CvPlayerTraits {
    Player* m_pPlayer;
    Traits* m_pTraits;
    bool HasTrait(TraitTypes) const;
};
''' + body + r'''
int main() {
    Traits traits{{false, true}};
    Player observer{-1}, player{0};
    CvPlayerTraits query{nullptr, &traits};
    assert(!query.HasTrait(0));
    query.m_pPlayer = &observer;
    assert(!query.HasTrait(0));
    query.m_pPlayer = &player;
    assert(query.HasTrait(0));
    traits.trait.obsolete = true;
    assert(!query.HasTrait(0));
    traits.trait.obsolete = false;
    traits.trait.enabled = false;
    assert(!query.HasTrait(0));
}
''')

    def test_trade_countdown_matches_movement_without_changing_the_route(self):
        countdown = method('CvTradeClasses.cpp', 'int TradeConnection::GetTurnsRemaining(')
        source = '''
#include <cassert>
#include <vector>
struct TradeConnection {
    std::vector<int> m_aPlotList;
    int m_iCircuitsCompleted, m_iCircuitsToComplete;
    unsigned int m_iTradeUnitLocationIndex;
    bool m_bTradeUnitMovingForward;
    int GetTurnsRemaining(int) const;
};
''' + countdown + '''
int main() {
    for (int length = 2; length <= 20; ++length)
    for (int circuits = 1; circuits <= 5; ++circuits)
    for (int completed = 0; completed < circuits; ++completed)
    for (int location = 0; location < length; ++location)
    for (bool forward : {false, true})
    for (int speed = 1; speed <= 12; ++speed) {
        TradeConnection route{std::vector<int>(length), completed, circuits,
                              static_cast<unsigned int>(location), forward};
        const int remaining = route.GetTurnsRemaining(speed);
        assert(route.m_iCircuitsCompleted == completed);
        assert(route.m_iTradeUnitLocationIndex == static_cast<unsigned int>(location));
        assert(route.m_bTradeUnitMovingForward == forward);
        int turns = 0;
        while (route.m_iCircuitsCompleted < circuits) {
            ++turns;
            for (int step = 0; step < speed && route.m_iCircuitsCompleted < circuits; ++step) {
                if ((route.m_bTradeUnitMovingForward && route.m_iTradeUnitLocationIndex == length - 1)
                    || (!route.m_bTradeUnitMovingForward && route.m_iTradeUnitLocationIndex == 0))
                    route.m_bTradeUnitMovingForward = !route.m_bTradeUnitMovingForward;
                if (route.m_bTradeUnitMovingForward) ++route.m_iTradeUnitLocationIndex;
                else if (--route.m_iTradeUnitLocationIndex == 0) ++route.m_iCircuitsCompleted;
            }
        }
        assert(turns == remaining);
        assert(route.GetTurnsRemaining(speed) == 0);
    }
    TradeConnection invalid{std::vector<int>(2), 0, 1, 0, true};
    assert(invalid.GetTurnsRemaining(0) == -1);
    invalid.m_aPlotList.clear();
    assert(invalid.GetTurnsRemaining(1) == -1);
}
'''
        self.compile_and_run(source)

    def test_gold_history_query_is_read_only(self):
        header = (CORE / 'CvTreasury.h').read_text(encoding='latin1')
        start = header.index('\tint GetLastGoldChangeTimes100() const')
        getter = header[start:header.index('\n\t}', start) + 3]
        self.compile_and_run('''
#include <cassert>
#include <vector>
struct Treasury {
    std::vector<int> m_GoldChangeForTurnTimes100;
''' + getter + '''
};
int main() {
    Treasury treasury;
    assert(treasury.GetLastGoldChangeTimes100() == 0);
    for (int value : {101, -255, 0, 9876}) {
        treasury.m_GoldChangeForTurnTimes100.push_back(value);
        auto before = treasury.m_GoldChangeForTurnTimes100;
        assert(treasury.GetLastGoldChangeTimes100() == value);
        assert(before == treasury.m_GoldChangeForTurnTimes100);
    }
}
''')

    def test_batch_allocator_uses_full_pointer_addresses(self):
        header = (CORE / 'FirePlace/include/FireWorks/FBatchAllocate.h').read_text()
        header = header[:header.index('inline void TestBatchAlloc()')]
        header = header.replace('#pragma once', '').replace('#include "FAssert.h"', '')
        header = header.replace('#include "FArray.h"', '')
        self.compile_and_run('''
#include <cassert>
#include <cstddef>
#include <cstdint>
#define FAssert(value) assert(value)
#define FNEW(value, pool, id) new value
''' + header + '''
struct TwelveBytes { int values[3]; };
int main() {
    for (unsigned int count = 1; count <= 100; ++count) {
        TwelveBytes* records;
        double** grid;
        unsigned char* bytes;
        using Allocation = FAllocArrayType<TwelveBytes,
                           FAllocArray2DType<double,
                           FAllocArrayType<unsigned char, FAllocBase<0, 0>>>>;
        Allocation allocation;
        AllocData data[] = {{&records, count, 0}, {&grid, count, 7}, {&bytes, count, 0}};
        allocation.Alloc(data);
        assert(reinterpret_cast<uintptr_t>(records) % sizeof(TwelveBytes) == 0);
        assert(reinterpret_cast<uintptr_t>(grid) % alignof(double*) == 0);
        for (unsigned int row = 0; row < count; ++row) {
            assert(reinterpret_cast<uintptr_t>(grid[row]) % alignof(double) == 0);
            records[row].values[0] = row;
            bytes[row] = row;
            for (int column = 0; column < 7; ++column) grid[row][column] = row + column;
        }
        for (unsigned int row = 0; row < count; ++row) {
            assert(records[row].values[0] == row);
            assert(bytes[row] == row);
            for (int column = 0; column < 7; ++column) assert(grid[row][column] == row + column);
        }
        allocation.Free();
    }
}
''')

    def test_route_display_marks_only_the_new_countdown(self):
        source = (CORE / 'Lua/CvLuaPlayer.cpp').read_text(encoding='latin1')
        self.assertEqual(source.count('"TurnsLeftIncludesCurrentTurn"'), 3)
        self.assertEqual(source.count('pConnection->GetTurnsRemaining('), 3)
        tooltip = (ROOT / 'LEKMOD/Lua/tmp/eui/Core/EUI_tooltip_library.lua.ignore').read_text()
        self.assertIn('route.TurnsLeft - (route.TurnsLeftIncludesCurrentTurn and 0 or 1)', tooltip)
