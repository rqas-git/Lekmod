






#pragma once
#include "CvDllInterfaces.h"

class CvFeatureInfo;

class CvDllFeatureInfo : public ICvFeatureInfo1
{
public:
	CvDllFeatureInfo(_In_ CvFeatureInfo* pFeatureInfo);
	~CvDllFeatureInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvFeatureInfo* GetInstance();

	const char* DLLCALL GetType();
	const char* DLLCALL GetDescription();
	bool DLLCALL IsNoCoast();
	bool DLLCALL IsNoRiver();
	bool DLLCALL IsNoAdjacent();
	bool DLLCALL IsRequiresFlatlands();
	bool DLLCALL IsRequiresRiver();
	bool DLLCALL IsNaturalWonder();
	const char* DLLCALL GetArtDefineTag();
	int DLLCALL GetWorldSoundscapeScriptId();
	const char* DLLCALL GetEffectTypeTag();
	bool DLLCALL IsTerrain(int i);

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvFeatureInfo* m_pFeatureInfo;
};
