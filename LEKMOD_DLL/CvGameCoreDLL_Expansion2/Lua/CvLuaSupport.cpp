













#include <CvGameCoreDLLPCH.h>
#include "CvLuaSupport.h"
#include "CvLuaEnums.h"
#include "CvLuaFractal.h"
#include "CvLuaGameInfo.h"
#include "CvLuaMap.h"
#include "CvLuaGame.h"
#include "CvLuaPlayer.h"
#include "CvLuaTeam.h"




bool luaL_optbool(lua_State* L, int idx, bool bdefault)
{
	if(lua_isnoneornil(L, idx))
	{
		return bdefault;
	}
	else
	{
		return (bool)lua_toboolean(L, idx);
	}
}


void LuaSupport::RegisterScriptData(lua_State* L)
{
	CvLuaEnums::Register(L);
	CvLuaFractal::Register(L);
	CvLuaGameInfo::Register(L);
	CvLuaMap::Register(L);
	CvLuaGame::Register(L);
	CvLuaPlayer::Register(L);
	CvLuaTeam::Register(L);
}


void LuaSupport::DumpCallStack(lua_State* L)
{
	for(int i = 1; i < 10; ++i)
	{
		lua_Debug ar;
		if(lua_getstack(L, i, &ar))
		{
			lua_getinfo(L, "Sl", &ar);
			if(ar.currentline > 0)
			{
				FStringFixedBuffer(szTemp, 512);
				szTemp.Format("%s:%d\n", (ar.source)?ar.source:"", ar.currentline);
				OutputDebugString(szTemp.c_str());
			}
		}
	}
}


#ifdef AUI_WARNING_FIXES
bool LuaSupport::CallHook(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, bool& value)
#else
bool LuaSupport::CallHook(ICvEngineScriptSystem1* pkScriptSystem, const char* szName, ICvEngineScriptSystemArgs1* args, bool& value)
#endif
{

	bool bHadLock = gDLL->HasGameCoreLock();
	if(bHadLock)
		gDLL->ReleaseGameCoreLock();
	bool bResult = pkScriptSystem->CallHook(szName, args, value);
	if(bHadLock)
		gDLL->GetGameCoreLock();
	return bResult;
}


#ifdef AUI_WARNING_FIXES
bool LuaSupport::CallTestAll(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, bool& value)
#else
bool LuaSupport::CallTestAll(ICvEngineScriptSystem1* pkScriptSystem, const char* szName, ICvEngineScriptSystemArgs1* args, bool& value)
#endif
{

	bool bHadLock = gDLL->HasGameCoreLock();
	if(bHadLock)
		gDLL->ReleaseGameCoreLock();
	bool bResult = pkScriptSystem->CallTestAll(szName, args, value);
	if(bHadLock)
		gDLL->GetGameCoreLock();
	return bResult;
}


#ifdef AUI_WARNING_FIXES
bool LuaSupport::CallTestAny(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, bool& value)
#else
bool LuaSupport::CallTestAny(ICvEngineScriptSystem1* pkScriptSystem, const char* szName, ICvEngineScriptSystemArgs1* args, bool& value)
#endif
{

	bool bHadLock = gDLL->HasGameCoreLock();
	if(bHadLock)
		gDLL->ReleaseGameCoreLock();
	bool bResult = pkScriptSystem->CallTestAny(szName, args, value);
	if(bHadLock)
		gDLL->GetGameCoreLock();
	return bResult;
}


bool LuaSupport::CallAccumulator(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, int& value)
{

	bool bHadLock = gDLL->HasGameCoreLock();
	if(bHadLock)
		gDLL->ReleaseGameCoreLock();
	bool bResult = pkScriptSystem->CallAccumulator(szName, args, value);
	if(bHadLock)
		gDLL->GetGameCoreLock();
	return bResult;
}


bool LuaSupport::CallAccumulator(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, float& value)
{

	bool bHadLock = gDLL->HasGameCoreLock();
	if(bHadLock)
		gDLL->ReleaseGameCoreLock();
	bool bResult = pkScriptSystem->CallAccumulator(szName, args, value);
	if(bHadLock)
		gDLL->GetGameCoreLock();
	return bResult;
}
