














#include "CvGameCoreDLLPCH.h"
#include "CvLuaArgsHandle.h"

CvLuaArgsHandle::CvLuaArgsHandle()
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		m_Ptr = pkScriptSystem->CreateArgs();
	}
}
CvLuaArgsHandle::CvLuaArgsHandle(uint uiReserve)
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		m_Ptr = pkScriptSystem->CreateArgs(uiReserve);
	}
}

CvLuaArgsHandle::~CvLuaArgsHandle()
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		pkScriptSystem->DestroyArgs(m_Ptr);
	}
}
