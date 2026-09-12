






#pragma once
#include "CvDllInterfaces.h"

class CvPlayerOptionInfo;

class CvDllPlayerOptionInfo : public ICvPlayerOptionInfo1
{
public:
	CvDllPlayerOptionInfo(_In_ CvPlayerOptionInfo* pPlayerOptionInfo);
	~CvDllPlayerOptionInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvPlayerOptionInfo* GetInstance();

	bool DLLCALL GetDefault();

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvPlayerOptionInfo* m_pPlayerOptionInfo;
};
