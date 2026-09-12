






#pragma once
#include "CvDllInterfaces.h"

class CvPolicyEntry;

class CvDllPolicyInfo : public ICvPolicyInfo1
{
public:
	CvDllPolicyInfo(_In_ CvPolicyEntry* pPolicyInfo);
	~CvDllPolicyInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvPolicyEntry* GetInstance();

	const char* DLLCALL GetType();
	const char* DLLCALL GetDescription();

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvPolicyEntry* m_pPolicyInfo;
};
