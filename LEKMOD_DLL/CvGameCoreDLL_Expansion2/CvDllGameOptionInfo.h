






#pragma once
#include "CvDllInterfaces.h"

class CvGameOptionInfo;

class CvDllGameOptionInfo : public ICvGameOptionInfo1
{
public:
	CvDllGameOptionInfo(_In_ CvGameOptionInfo* pGameOptionInfo);
	~CvDllGameOptionInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvGameOptionInfo* GetInstance();

	const char* DLLCALL GetType();
	const char* DLLCALL GetDescription();
	bool DLLCALL GetDefault();

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvGameOptionInfo* m_pGameOptionInfo;
};
