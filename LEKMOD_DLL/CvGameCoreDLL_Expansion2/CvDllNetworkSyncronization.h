






#pragma once

#include "CvDllInterfaces.h"

class CvDllNetworkSyncronization : public ICvNetworkSyncronization1
{
public:
	CvDllNetworkSyncronization();
	~CvDllNetworkSyncronization();

	void* DLLCALL QueryInterface(GUID guidInterface);

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	void DLLCALL ClearCityDeltas();
	void DLLCALL ClearPlayerDeltas();
	void DLLCALL ClearPlotDeltas();
	void DLLCALL ClearRandomDeltas();
	void DLLCALL ClearUnitDeltas();

	void DLLCALL SyncCities();
	void DLLCALL SyncPlayers();
	void DLLCALL SyncPlots();
	void DLLCALL SyncUnits();

private:
	void DLLCALL Destroy();
};
