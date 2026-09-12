







#include "CvGameCoreDLLPCH.h"
#include "CvDllDlcPackageInfo.h"
#include "CvDllContext.h"

CvDllDlcPackageInfo::CvDllDlcPackageInfo(const GUID& kGuid)
	: m_packageId(kGuid)
	, m_uiRefCount(1)
{
}

CvDllDlcPackageInfo::~CvDllDlcPackageInfo()
{
}

void* CvDllDlcPackageInfo::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvDlcPackageInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}

unsigned int CvDllDlcPackageInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}

unsigned int CvDllDlcPackageInfo::DecrementReference()
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

unsigned int CvDllDlcPackageInfo::GetReferenceCount()
{
	return m_uiRefCount;
}

void CvDllDlcPackageInfo::Destroy()
{
	DecrementReference();
}

void CvDllDlcPackageInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}

void* CvDllDlcPackageInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}

GUID CvDllDlcPackageInfo::GetPackageID()
{
	return m_packageId;
}

CvDllDlcPackageInfoList::CvDllDlcPackageInfoList(const PackageIDList& packageIDs)
	: m_uiRefCount(1)
	, m_packageIDs(packageIDs.begin(), packageIDs.end())
	, m_iIndex(-1)
{
}

CvDllDlcPackageInfoList::~CvDllDlcPackageInfoList()
{
}

void* CvDllDlcPackageInfoList::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvEnumerator::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}

unsigned int CvDllDlcPackageInfoList::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}

unsigned int CvDllDlcPackageInfoList::DecrementReference()
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

unsigned int CvDllDlcPackageInfoList::GetReferenceCount()
{
	return m_uiRefCount;
}

void CvDllDlcPackageInfoList::Destroy()
{
	DecrementReference();
}

void CvDllDlcPackageInfoList::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}

void* CvDllDlcPackageInfoList::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}

bool CvDllDlcPackageInfoList::MoveNext()
{
	if(m_iIndex == -1)
	{
		if(m_packageIDs.size() > 0)
		{
			m_iIndex = 0;
			return true;
		}
		else
			return false;
	}

	if((size_t)m_iIndex < m_packageIDs.size())
		m_iIndex++;

	return (size_t)m_iIndex < m_packageIDs.size();
}

void CvDllDlcPackageInfoList::Reset()
{
	m_iIndex = -1;
}

ICvUnknown* CvDllDlcPackageInfoList::GetCurrent()
{
	if(m_iIndex > -1 && (size_t)m_iIndex < m_packageIDs.size())
	{
		return new CvDllDlcPackageInfo(m_packageIDs[m_iIndex]);
	}

	return NULL;
}
