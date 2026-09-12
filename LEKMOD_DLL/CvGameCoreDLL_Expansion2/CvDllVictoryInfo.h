






#pragma once
#include "CvDllInterfaces.h"

class CvVictoryInfo;

class CvDllVictoryInfo : public ICvVictoryInfo1
{
public:
	CvDllVictoryInfo(_In_ CvVictoryInfo* pVictoryInfo);
	~CvDllVictoryInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvVictoryInfo* GetInstance();

	const char* DLLCALL GetType();
	const char* DLLCALL GetDescription();

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvVictoryInfo* m_pVictoryInfo;
};
