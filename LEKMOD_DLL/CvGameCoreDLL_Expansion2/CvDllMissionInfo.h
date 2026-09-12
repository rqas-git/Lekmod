






#pragma once
#include "CvDllInterfaces.h"

class CvMissionInfo;

class CvDllMissionInfo : public ICvMissionInfo1
{
public:
	CvDllMissionInfo(_In_ CvMissionInfo* pMissionInfo);
	~CvDllMissionInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvMissionInfo* GetInstance();

	const char* DLLCALL GetType();
	const char* DLLCALL GetDescription();

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvMissionInfo* m_pMissionInfo;
};
