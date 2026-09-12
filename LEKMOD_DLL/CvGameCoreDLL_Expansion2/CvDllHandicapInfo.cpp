






#include "CvGameCoreDLLPCH.h"
#include "CvDllHandicapInfo.h"
#include "CvDllContext.h"

#ifdef AUI_WARNING_FIXES
CvDllHandicapInfo::CvDllHandicapInfo(_In_ CvHandicapInfo* pHandicapInfo)
#else
CvDllHandicapInfo::CvDllHandicapInfo(CvHandicapInfo* pHandicapInfo)
#endif
	: m_pHandicapInfo(pHandicapInfo)
	, m_uiRefCount(1)
{
	FAssertMsg(pHandicapInfo != NULL, "SHOULD NOT HAPPEN");
}

CvDllHandicapInfo::~CvDllHandicapInfo()
{
}

void* CvDllHandicapInfo::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvHandicapInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}

unsigned int CvDllHandicapInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}

unsigned int CvDllHandicapInfo::DecrementReference()
{
	if(m_uiRefCount == 1)
	{
		delete this;
		return 0;
	}
	else
	{
		--m_uiRefCount;
		return m_uiRefCount;
	}
}

unsigned int CvDllHandicapInfo::GetReferenceCount()
{
	return m_uiRefCount;
}

void CvDllHandicapInfo::Destroy()
{
	DecrementReference();
}

void CvDllHandicapInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}

void* CvDllHandicapInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}

CvHandicapInfo* CvDllHandicapInfo::GetInstance()
{
	return m_pHandicapInfo;
}

const char* CvDllHandicapInfo::GetType()
{
	return m_pHandicapInfo->GetType();
}

const char* CvDllHandicapInfo::GetDescription()
{
	return m_pHandicapInfo->GetDescription();
}

int CvDllHandicapInfo::GetBarbSpawnMod()
{
	return m_pHandicapInfo->getBarbSpawnMod();
}
