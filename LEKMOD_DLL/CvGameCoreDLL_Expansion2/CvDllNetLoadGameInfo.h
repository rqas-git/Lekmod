






#pragma once
#include "CvDllInterfaces.h"

class CvDllNetLoadGameInfo : public ICvNetLoadGameInfo1
{
public:
	CvDllNetLoadGameInfo();
	~CvDllNetLoadGameInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	bool DLLCALL Read(FDataStream& kStream);
	bool DLLCALL Write(FDataStream& kStream);
	bool DLLCALL Commit();

private:
	void DLLCALL Destroy();


	std::vector<SlotStatus> m_slotStatus;

	unsigned int m_uiRefCount;
};
