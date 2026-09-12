






#pragma once
#include "CvDllInterfaces.h"

class CvResourceInfo;

class CvDllResourceInfo : public ICvResourceInfo1
{
public:
	CvDllResourceInfo(_In_ CvResourceInfo* pResourceInfo);
	~CvDllResourceInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvResourceInfo* GetInstance();

	const char* DLLCALL GetType();
	const char* DLLCALL GetDescription();
	int DLLCALL GetResourceClassType();
	ResourceUsageTypes DLLCALL GetResourceUsage();
	const char* DLLCALL GetIconString();
	const char* DLLCALL GetArtDefineTag();
	const char* DLLCALL GetArtDefineTagHeavy();
	const char* DLLCALL GetAltArtDefineTag();
	const char* DLLCALL GetAltArtDefineTagHeavy();
	bool DLLCALL IsTerrain(int i);
	bool DLLCALL IsFeature(int i);
	bool DLLCALL IsFeatureTerrain(int i);

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvResourceInfo* m_pResourceInfo;
};
