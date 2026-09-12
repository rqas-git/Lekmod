











#ifndef FLua_h
#define FLua_h
#pragma once

#include "FLuaTypes.h"
#include "FLuaGlobalData.h"
#include "FLuaStaticFunctions.h"
#include "FLuaTypeExposure.h"
#include "FLuaPointerProtection.h"

namespace FLua
{

	inline void RegisterAll(lua_State *L)
	{
		GlobalData::Register(L);
		StaticFunctions::Register(L);
		TypeExposures::Register(L, TypeExposures::sm_uiUnrestrictedExposure);
	}


	inline void RegisterAll(lua_State *L, unsigned int uiCategoryFlags)
	{
		GlobalData::Register(L);
		StaticFunctions::Register(L, uiCategoryFlags);
		TypeExposures::Register(L, uiCategoryFlags);
	}


	inline lua_Integer tointeger(lua_State* L, int iIndex) 
	{ 
		FAssert(!lua_isnil(L, iIndex)); 
		return lua_tointeger(L, iIndex);
	}


	inline lua_Number tonumber(lua_State* L, int iIndex) 
	{ 
		FAssert(!lua_isnil(L, iIndex)); 
		return lua_tonumber(L, iIndex);
	}


	inline int toboolean(lua_State* L, int iIndex) 
	{ 
		FAssert(!lua_isnil(L, iIndex)); 
		return lua_toboolean(L, iIndex);
	}

}

#endif
