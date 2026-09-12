






#pragma once
#include "CvDllInterfaces.h"

class CvInterfaceModeInfo;

class CvDllInterfaceModeInfo : public ICvInterfaceModeInfo1
{
public:
	CvDllInterfaceModeInfo(_In_ CvInterfaceModeInfo* pInterfaceModeInfo);
	~CvDllInterfaceModeInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvInterfaceModeInfo* GetInstance();

	int DLLCALL GetMissionType();

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvInterfaceModeInfo* m_pInterfaceModeInfo;
};
