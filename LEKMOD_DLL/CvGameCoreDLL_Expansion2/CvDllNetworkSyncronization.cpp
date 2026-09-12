






#include "CvGameCoreDLLPCH.h"
#include "CvDllNetworkSyncronization.h"
#include "CvDllContext.h"
#include "CvCity.h"
#include "CvPlayer.h"
#include "CvPlot.h"
#include "CvUnit.h"


CvDllNetworkSyncronization::CvDllNetworkSyncronization()
{
}

CvDllNetworkSyncronization::~CvDllNetworkSyncronization()
{
}

void* CvDllNetworkSyncronization::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvNetworkSyncronization1::GetInterfaceId())
	{
		return this;
	}

	return NULL;
}

void CvDllNetworkSyncronization::Destroy()
{


}

void CvDllNetworkSyncronization::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}

void* CvDllNetworkSyncronization::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}

void CvDllNetworkSyncronization::ClearCityDeltas()
{
	FSerialization::ClearCityDeltas();
}

void CvDllNetworkSyncronization::ClearPlayerDeltas()
{
	FSerialization::ClearPlayerDeltas();
}

void CvDllNetworkSyncronization::ClearPlotDeltas()
{
	FSerialization::ClearPlotDeltas();
}

void CvDllNetworkSyncronization::ClearRandomDeltas()
{
	GC.getGame().getJonRand().clearCallstacks();
}

void CvDllNetworkSyncronization::ClearUnitDeltas()
{
	FSerialization::ClearUnitDeltas();
}

void CvDllNetworkSyncronization::SyncCities()
{
	FSerialization::SyncCities();
}

void CvDllNetworkSyncronization::SyncPlayers()
{
	FSerialization::SyncPlayer();
}

void CvDllNetworkSyncronization::SyncPlots()
{
	FSerialization::SyncPlots();
}

void CvDllNetworkSyncronization::SyncUnits()
{
	FSerialization::SyncUnits();
}
