#pragma once

#include <cmath>
#include <cstdint>
#include <limits>
extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

inline lua_Integer windowsLuaInteger(lua_Number value) {
    const lua_Number lower = std::floor(value);
    const lua_Number fraction = value - lower;
    const lua_Number rounded = lower + (fraction > 0.5 ||
        (fraction == 0.5 && std::fmod(lower, 2.0) != 0.0));
    if (!(rounded >= std::numeric_limits<int32_t>::min() &&
          rounded <= std::numeric_limits<int32_t>::max()))
        return std::numeric_limits<int32_t>::min();
    return static_cast<lua_Integer>(rounded);
}

inline lua_Integer windowsLuaToInteger(lua_State* state, int index) {
    return windowsLuaInteger(lua_isnumber(state, index) ? luaL_checknumber(state, index) : 0);
}

inline lua_Integer windowsLuaCheckInteger(lua_State* state, int index) {
    return windowsLuaInteger(luaL_checknumber(state, index));
}

inline lua_Integer windowsLuaOptInteger(lua_State* state, int index, lua_Integer fallback) {
    return lua_isnoneornil(state, index) ? fallback : windowsLuaCheckInteger(state, index);
}

// Windows Lua 5.1 uses 32-bit nearest-even conversion instead of the host's truncation.
#define lua_tointeger windowsLuaToInteger
#define luaL_checkinteger windowsLuaCheckInteger
#define luaL_optinteger windowsLuaOptInteger
