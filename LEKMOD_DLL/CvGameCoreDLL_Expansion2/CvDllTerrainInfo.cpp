






#include "CvGameCoreDLLPCH.h"
#include "CvDllTerrainInfo.h"
#include "CvDllContext.h"

#ifdef AUI_WARNING_FIXES
CvDllTerrainInfo::CvDllTerrainInfo(_In_ CvTerrainInfo* pTerrainInfo)
#else
CvDllTerrainInfo::CvDllTerrainInfo(CvTerrainInfo* pTerrainInfo)
#endif
	: m_pTerrainInfo(pTerrainInfo)
	, m_uiRefCount(1)
{
	FAssertMsg(pTerrainInfo != NULL, "SHOULD NOT HAPPEN");
}

CvDllTerrainInfo::~CvDllTerrainInfo()
{
}

void* CvDllTerrainInfo::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvTerrainInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}

unsigned int CvDllTerrainInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}

unsigned int CvDllTerrainInfo::DecrementReference()
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

unsigned int CvDllTerrainInfo::GetReferenceCount()
{
	return m_uiRefCount;
}

void CvDllTerrainInfo::Destroy()
{
	DecrementReference();
}

void CvDllTerrainInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}

void* CvDllTerrainInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}

CvTerrainInfo* CvDllTerrainInfo::GetInstance()
{
	return m_pTerrainInfo;
}

const char* CvDllTerrainInfo::GetType()
{
	return m_pTerrainInfo->GetType();
}

const char* CvDllTerrainInfo::GetDescription()
{
	return m_pTerrainInfo->GetDescription();
}

bool CvDllTerrainInfo::IsWater()
{
	return m_pTerrainInfo->isWater();
}

const char* CvDllTerrainInfo::GetArtDefineTag()
{
	return m_pTerrainInfo->getArtDefineTag();
}

int CvDllTerrainInfo::GetWorldSoundscapeScriptId()
{
	return m_pTerrainInfo->getWorldSoundscapeScriptId();
}

const char* CvDllTerrainInfo::GetEffectTypeTag()
{
	return m_pTerrainInfo->getEffectTypeTag();
}
