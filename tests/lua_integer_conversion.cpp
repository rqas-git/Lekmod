#include <cassert>
#include <cfenv>
#include <limits>
#include "lua_compat.hpp"

static int checkInteger(lua_State* state) {
    lua_pushinteger(state, luaL_checkinteger(state, 1));
    return 1;
}

int main() {
    lua_State* state = luaL_newstate();
    struct Case { double input; lua_Integer expected; };
    const Case cases[] = {
        {2.5, 2}, {2.6, 3}, {3.5, 4}, {61.6, 62}, {260.0 / 3.0, 87},
        {-2.5, -2}, {-2.6, -3}, {-3.5, -4}, {0.5, 0}, {-0.5, 0},
        {2147483647.0, 2147483647}, {-2147483648.0, -2147483648LL},
        {2147483648.0, -2147483648LL}, {-2147483649.0, -2147483648LL},
        {std::numeric_limits<double>::infinity(), -2147483648LL},
        {std::numeric_limits<double>::quiet_NaN(), -2147483648LL}
    };
    const int modes[] = {FE_TONEAREST, FE_UPWARD, FE_DOWNWARD, FE_TOWARDZERO};
    for (int mode : modes) {
        std::fesetround(mode);
        for (const Case& item : cases) {
            lua_pushnumber(state, item.input);
            assert(lua_tointeger(state, -1) == item.expected);
            assert(luaL_checkinteger(state, -1) == item.expected);
            assert(luaL_optinteger(state, -1, 9) == item.expected);
            lua_pop(state, 1);
        }
    }
    std::fesetround(FE_TONEAREST);
    lua_pushstring(state, "61.6");
    assert(lua_tointeger(state, -1) == 62);
    assert(luaL_checkinteger(state, -1) == 62);
    lua_pop(state, 1);
    lua_pushnil(state);
    assert(lua_tointeger(state, -1) == 0);
    assert(luaL_optinteger(state, -1, 9) == 9);
    lua_pop(state, 1);
    assert(luaL_optinteger(state, 1, 9) == 9);
    lua_pushcfunction(state, checkInteger);
    lua_pushstring(state, "not a number");
    assert(lua_pcall(state, 1, 1, 0) != 0);
    lua_close(state);
}
