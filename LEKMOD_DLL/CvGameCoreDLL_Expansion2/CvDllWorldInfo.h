






#pragma once
#include "CvDllInterfaces.h"

class CvWorldInfo;

class CvDllWorldInfo : public ICvWorldInfo1
{
public:
	CvDllWorldInfo(_In_ CvWorldInfo* pWorldInfo);
	~CvDllWorldInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvWorldInfo* GetInstance();

	const char* DLLCALL GetType();
	const char* DLLCALL GetDescriptionKey();
	int DLLCALL GetDefaultPlayers();
	int DLLCALL GetDefaultMinorCivs();

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvWorldInfo* m_pWorldInfo;
};
