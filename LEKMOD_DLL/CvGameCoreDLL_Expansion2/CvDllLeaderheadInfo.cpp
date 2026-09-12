






#include "CvGameCoreDLLPCH.h"
#include "CvDllLeaderHeadInfo.h"
#include "CvDllContext.h"

#ifdef AUI_WARNING_FIXES
CvDllLeaderHeadInfo::CvDllLeaderHeadInfo(_In_ CvLeaderHeadInfo* pLeaderHeadInfo)
#else
CvDllLeaderHeadInfo::CvDllLeaderHeadInfo(CvLeaderHeadInfo* pLeaderHeadInfo)
#endif
	: m_pLeaderHeadInfo(pLeaderHeadInfo)
	, m_uiRefCount(1)
{
	FAssertMsg(pLeaderHeadInfo != NULL, "SHOULD NOT HAPPEN");
}

CvDllLeaderHeadInfo::~CvDllLeaderHeadInfo()
{
}

void* CvDllLeaderHeadInfo::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvLeaderHeadInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}

unsigned int CvDllLeaderHeadInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}

unsigned int CvDllLeaderHeadInfo::DecrementReference()
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

unsigned int CvDllLeaderHeadInfo::GetReferenceCount()
{
	return m_uiRefCount;
}

void CvDllLeaderHeadInfo::Destroy()
{
	DecrementReference();
}

void CvDllLeaderHeadInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}

void* CvDllLeaderHeadInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}

CvLeaderHeadInfo* CvDllLeaderHeadInfo::GetInstance()
{
	return m_pLeaderHeadInfo;
}

const char* CvDllLeaderHeadInfo::GetDescription()
{
	return m_pLeaderHeadInfo->GetDescription();
}

const char* CvDllLeaderHeadInfo::GetArtDefineTag()
{
	return m_pLeaderHeadInfo->getArtDefineTag();
}
