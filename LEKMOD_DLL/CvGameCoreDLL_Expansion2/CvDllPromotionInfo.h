






#pragma once
#include "CvDllInterfaces.h"

class CvPromotionEntry;

class CvDllPromotionInfo : public ICvPromotionInfo1
{
public:
	CvDllPromotionInfo(_In_ CvPromotionEntry* pPromotionInfo);
	~CvDllPromotionInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvPromotionEntry* GetInstance();

	const char* DLLCALL GetType();
	const char* DLLCALL GetDescription();

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvPromotionEntry* m_pPromotionInfo;
};
