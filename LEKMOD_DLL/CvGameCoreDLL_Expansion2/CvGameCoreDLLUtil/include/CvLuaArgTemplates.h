
#pragma once 
#ifndef CVLUAARGTEMPLATES
#define CVLUAARGTEMPLATES

namespace CvLuaArgs
{

	template<typename T>
	static T toValue(lua_State* L, int idx)
	{


		return (T)lua_tointeger(L, idx);
	}

	template<> static int toValue(lua_State* L, int idx)
	{
		return lua_tointeger(L, idx);
	}

	template<> static bool toValue(lua_State* L, int idx)
	{
		return lua_toboolean(L, idx) != 0;
	}


	template<typename T>
	static void pushValue(lua_State* L, T t)
	{


		lua_pushinteger(L, t);
	}

	template<>
	static void pushValue(lua_State* L, int t)
	{
		lua_pushinteger(L, t);
	}

	template<>
	static void pushValue(lua_State* L, bool t)
	{
		lua_pushboolean(L, t);
	}

}

#endif
