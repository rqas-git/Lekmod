






#pragma once
#include "CvDllInterfaces.h"

class CvTerrainInfo;

class CvDllTerrainInfo : public ICvTerrainInfo1
{
public:
	CvDllTerrainInfo(_In_ CvTerrainInfo* pTerrainInfo);
	~CvDllTerrainInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvTerrainInfo* GetInstance();

	const char* DLLCALL GetType();
	const char* DLLCALL GetDescription();
	bool DLLCALL IsWater();
	const char* DLLCALL GetArtDefineTag();
	int DLLCALL GetWorldSoundscapeScriptId();
	const char* DLLCALL GetEffectTypeTag();

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvTerrainInfo* m_pTerrainInfo;
};
