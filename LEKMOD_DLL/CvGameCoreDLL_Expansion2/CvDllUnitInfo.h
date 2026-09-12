






#pragma once
#include "CvDllInterfaces.h"

class CvDllUnitInfo : public ICvUnitInfo1
{
public:
	CvDllUnitInfo(_In_ CvUnitEntry* pUnitInfo);
	~CvDllUnitInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	int DLLCALL GetCombat() const;
	int DLLCALL GetDomainType() const;
	const char* DLLCALL GetType() const;
	const char* DLLCALL GetText() const;
	UnitMoveRate DLLCALL GetMoveRate(int numHexes) const;
	const char* DLLCALL GetUnitArtInfoTag() const;
	bool DLLCALL GetUnitArtInfoCulturalVariation() const;
	bool DLLCALL GetUnitArtInfoEraVariation() const;
	int DLLCALL GetUnitFlagIconOffset() const;

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvUnitEntry* m_pUnitInfo;
};
