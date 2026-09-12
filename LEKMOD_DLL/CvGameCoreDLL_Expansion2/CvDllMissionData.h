







#pragma once

#include "CvDllInterfaces.h"

class CvDllMissionData: public ICvMissionData1
{
public:
	CvDllMissionData(_In_ const MissionData* pMissionData);
	~CvDllMissionData();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	int DLLCALL GetData1() const;
	int DLLCALL GetData2() const;
	int DLLCALL GetFlags() const;
	int DLLCALL GetPushTurn() const;
	MissionTypes DLLCALL GetMissionType() const;

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	const MissionData* m_pMissionData;
};
