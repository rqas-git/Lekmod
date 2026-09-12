






#pragma once
#include "CvDllInterfaces.h"

typedef std::list<GUID> PackageIDList;

class CvDllDlcPackageInfo : public ICvDlcPackageInfo1
{
public:
	CvDllDlcPackageInfo(const GUID& kPackageID);
	~CvDllDlcPackageInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	GUID DLLCALL GetPackageID();

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	GUID m_packageId;
};

class CvDllDlcPackageInfoList : public ICvEnumerator
{
public:
	CvDllDlcPackageInfoList(const PackageIDList& packageIDs);
	~CvDllDlcPackageInfoList();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	bool DLLCALL MoveNext();
	void DLLCALL Reset();

	ICvUnknown* DLLCALL GetCurrent();

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	const std::vector<GUID> m_packageIDs;
	int m_iIndex;
};
