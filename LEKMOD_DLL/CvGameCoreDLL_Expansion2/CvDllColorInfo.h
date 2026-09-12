






#pragma once
#include "CvDllInterfaces.h"

class CvColorInfo;

class CvDllColorInfo : public ICvColorInfo1
{
public:
	CvDllColorInfo(_In_ CvColorInfo* pColorInfo);
	~CvDllColorInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvColorInfo* GetInstance();

	const char* DLLCALL GetType();
	const CvColorA& DLLCALL GetColor();

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvColorInfo* m_pColorInfo;
};
