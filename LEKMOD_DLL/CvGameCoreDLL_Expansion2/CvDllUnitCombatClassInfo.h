






#pragma once
#include "CvDllInterfaces.h"

class CvBaseInfo;

class CvDllUnitCombatClassInfo : public ICvUnitCombatClassInfo1
{
public:
	CvDllUnitCombatClassInfo(_In_ CvBaseInfo* pUnitCombatClassInfo);
	~CvDllUnitCombatClassInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvBaseInfo* GetInstance();

	const char* DLLCALL GetType();
	const char* DLLCALL GetDescription();

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvBaseInfo* m_pUnitCombatClassInfo;
};
