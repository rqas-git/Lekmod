














#pragma once

#include "ICvDLLScriptSystem.h"


class CvLuaArgsHandle
{
public:
	CvLuaArgsHandle();
	CvLuaArgsHandle(uint uiReserve);
	~CvLuaArgsHandle();

	ICvEngineScriptSystemArgs1* operator->() const;
	ICvEngineScriptSystemArgs1* get() const;

private:
	ICvEngineScriptSystemArgs1* m_Ptr;
};




inline ICvEngineScriptSystemArgs1* CvLuaArgsHandle::operator->() const
{
	return m_Ptr;
}

inline ICvEngineScriptSystemArgs1* CvLuaArgsHandle::get() const
{
	return m_Ptr;
}
