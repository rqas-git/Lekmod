// Exercise the production adapter and method wrappers against a real Lua 5.1 state.
#include <cassert>
extern "C" {
#include <lua.h>
#include <lauxlib.h>
}
#include "CvLuaMethodWrapper.h"

struct CvPlayer {
    int value;
    CvPlayer() : value(0) {}
    int get() const { return value; }
    void set(int next) { value = next; }
    bool compare(int other, bool enabled) const { return enabled && value == other; }
    int sum(int a, int b, int c, int d) const { return value + a + b + c + d; }
};
struct CvPlayerAI : CvPlayer {
    void increment() { ++value; }
};
class CvLuaPlayer : public CvLuaMethodWrapper<CvLuaPlayer, CvPlayerAI> {
#include "player_adapter.h"
public:
    static CvPlayerAI* GetInstance(lua_State* L) { return static_cast<CvPlayerAI*>(lua_touserdata(L, 1)); }
    static int GetStartingArgIndex() { return 2; }
    static int get(lua_State* L) { return BasicLuaMethod(L, &CvPlayerAI::get); }
    static int set(lua_State* L) { return BasicLuaMethod(L, &CvPlayerAI::set); }
    static int compare(lua_State* L) { return BasicLuaMethod(L, &CvPlayerAI::compare); }
    static int sum(lua_State* L) { return BasicLuaMethod(L, &CvPlayerAI::sum); }
    static int increment(lua_State* L) { return BasicLuaMethod(L, &CvPlayerAI::increment); }
};

int main() {
    lua_State* L = luaL_newstate();
    CvPlayerAI player;
    lua_pushcfunction(L, CvLuaPlayer::set);
    lua_pushlightuserdata(L, &player);
    lua_pushinteger(L, 7);
    assert(lua_pcall(L, 2, LUA_MULTRET, 0) == 0 && lua_gettop(L) == 0 && player.value == 7);
    lua_pushcfunction(L, CvLuaPlayer::increment);
    lua_pushlightuserdata(L, &player);
    assert(lua_pcall(L, 1, LUA_MULTRET, 0) == 0 && lua_gettop(L) == 0 && player.value == 8);
    lua_pushcfunction(L, CvLuaPlayer::get);
    lua_pushlightuserdata(L, &player);
    assert(lua_pcall(L, 1, LUA_MULTRET, 0) == 0 && lua_gettop(L) == 1 && lua_tointeger(L, -1) == 8);
    lua_settop(L, 0);
    lua_pushcfunction(L, CvLuaPlayer::compare);
    lua_pushlightuserdata(L, &player);
    lua_pushinteger(L, 8);
    lua_pushboolean(L, true);
    assert(lua_pcall(L, 3, LUA_MULTRET, 0) == 0 && lua_isboolean(L, -1) && lua_toboolean(L, -1));
    lua_settop(L, 0);
    lua_pushcfunction(L, CvLuaPlayer::sum);
    lua_pushlightuserdata(L, &player);
    for (int i = 1; i <= 4; ++i) lua_pushinteger(L, i);
    assert(lua_pcall(L, 5, LUA_MULTRET, 0) == 0 && lua_gettop(L) == 1 && lua_tointeger(L, -1) == 18);
    lua_close(L);
}
