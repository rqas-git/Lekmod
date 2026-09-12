














#pragma once
#ifndef CVLUASUPPORT_H
#define CVLUASUPPORT_H


extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
};


#include <Fireworks/FLua/Include/FLua.h>
#include <Fireworks/FLua/Include/FLuaFStringSupport.h>


#include "CvLuaArgsHandle.h"
class ICvEngineScriptSystem1;
class ICvEngineScriptSystemArgs1;

namespace LuaSupport
{


void RegisterScriptData(lua_State* L);

void DumpCallStack(lua_State* L);

bool CallHook(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, bool& value);
bool CallTestAll(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, bool& value);
bool CallTestAny(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, bool& value);
bool CallAccumulator(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, int& value);
bool CallAccumulator(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, float& value);

}

extern bool luaL_optbool(lua_State* L, int idx, bool bdefault);

#endif
