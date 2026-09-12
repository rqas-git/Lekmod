







#include "CvGameCoreDLLPCH.h"
#include "CvLuaSupport.h"
#include "CvLuaArea.h"
#include "CvLuaCity.h"


#define Method(Name)			\
	lua_pushcclosure(L, l##Name, 0);	\
	lua_setfield(L, t, #Name);

void CvLuaArea::PushMethods(lua_State* L, int t)
{
	Method(IsNone);

	Method(CalculateTotalBestNatureYield);
	Method(CountCoastalLand);
	Method(CountNumUniqueResourceTypes);
	Method(GetID);
	Method(GetNumTiles);

	Method(GetNumOwnedTiles);
	Method(GetNumUnownedTiles);
	Method(GetNumRiverEdges);
	Method(GetNumCities);
	Method(GetNumUnits);
	Method(GetTotalPopulation);
	Method(GetNumStartingPlots);
	Method(IsWater);

	Method(GetUnitsPerPlayer);
	Method(GetCitiesPerPlayer);
	Method(GetPopulationPerPlayer);
	Method(GetFreeSpecialist);

	Method(GetNumRevealedTiles);
	Method(GetNumUnrevealedTiles);

	Method(GetTargetCity);
	Method(GetYieldRateModifier);

	Method(GetNumResources);
	Method(GetNumTotalResources);
	Method(GetNumImprovements);
}

void CvLuaArea::HandleMissingInstance(lua_State* L)
{
	DefaultHandleMissingInstance(L);
}

const char* CvLuaArea::GetTypeName()
{
	return "Area";
}






int CvLuaArea::lIsNone(lua_State* L)
{
#ifdef AUI_WARNING_FIXES
	const bool bExists = (GetInstance(L, 1, false) != NULL);
#else
	const bool bExists = (GetInstance(L, false) != NULL);
#endif
	lua_pushboolean(L, bExists);

	return 1;
}

int CvLuaArea::lCalculateTotalBestNatureYield(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->calculateTotalBestNatureYield();
	lua_pushinteger(L, iResult);
	return 1;
}


int CvLuaArea::lCountCoastalLand(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->countCoastalLand();
	lua_pushinteger(L, iResult);
	return 1;
}


int CvLuaArea::lCountNumUniqueResourceTypes(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->countNumUniqueResourceTypes();
	lua_pushinteger(L, iResult);
	return 1;
}


int CvLuaArea::lGetID(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->GetID();
	lua_pushinteger(L, iResult);
	return 1;
}


int CvLuaArea::lGetNumTiles(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->getNumTiles();
	lua_pushinteger(L, iResult);
	return 1;
}












int CvLuaArea::lGetNumOwnedTiles(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->getNumOwnedTiles();
	lua_pushinteger(L, iResult);
	return 1;
}


int CvLuaArea::lGetNumUnownedTiles(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->getNumUnownedTiles();
	lua_pushinteger(L, iResult);
	return 1;
}


int CvLuaArea::lGetNumRiverEdges(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->getNumRiverEdges();
	lua_pushinteger(L, iResult);
	return 1;
}


int CvLuaArea::lGetNumCities(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->getNumCities();
	lua_pushinteger(L, iResult);
	return 1;
}


int CvLuaArea::lGetNumUnits(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->getNumUnits();
	lua_pushinteger(L, iResult);
	return 1;
}


int CvLuaArea::lGetTotalPopulation(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->getTotalPopulation();
	lua_pushinteger(L, iResult);
	return 1;
}


int CvLuaArea::lGetNumStartingPlots(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->getNumStartingPlots();
	lua_pushinteger(L, iResult);
	return 1;
}


int CvLuaArea::lIsWater(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);
	const bool bResult = pkArea->isWater();
	lua_pushboolean(L, bResult);
	return 1;
}


int CvLuaArea::lGetUnitsPerPlayer(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);
	const PlayerTypes eIndex = (PlayerTypes)lua_tointeger(L, 2);

	const int iResult = pkArea->getUnitsPerPlayer(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}


int CvLuaArea::lGetCitiesPerPlayer(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);
	const PlayerTypes eIndex = (PlayerTypes)lua_tointeger(L, 2);

	const int iResult = pkArea->getCitiesPerPlayer(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}


int CvLuaArea::lGetPopulationPerPlayer(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);
	const PlayerTypes eIndex = (PlayerTypes)lua_tointeger(L, 2);

	const int iResult = pkArea->getPopulationPerPlayer(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}


int CvLuaArea::lGetFreeSpecialist(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);
	const PlayerTypes eIndex = (PlayerTypes)lua_tointeger(L, 2);

	const int iResult = pkArea->getFreeSpecialist(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}


int CvLuaArea::lGetNumRevealedTiles(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);
	const TeamTypes eIndex = (TeamTypes)lua_tointeger(L, 2);

	const int iResult = pkArea->getNumRevealedTiles(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}


int CvLuaArea::lGetNumUnrevealedTiles(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);
	const TeamTypes eIndex = (TeamTypes)lua_tointeger(L, 2);

	const int iResult = pkArea->getNumUnrevealedTiles(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}



int CvLuaArea::lGetTargetCity(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);
	const PlayerTypes eIndex = (PlayerTypes)lua_tointeger(L, 2);

	CvCity* pkCity = pkArea->getTargetCity(eIndex);
	CvLuaCity::Push(L, pkCity);
	return 1;
}


int CvLuaArea::lGetYieldRateModifier(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);
	const PlayerTypes eIndex1 = (PlayerTypes)lua_tointeger(L, 2);
	const YieldTypes eIndex2 = (YieldTypes)lua_tointeger(L, 3);

	const int iResult = pkArea->getYieldRateModifier(eIndex1, eIndex2);
	lua_pushinteger(L, iResult);
	return 1;
}


int CvLuaArea::lGetNumResources(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);
	const ResourceTypes eResource = (ResourceTypes)lua_tointeger(L, 2);

	const int iResult = pkArea->getNumResources(eResource);
	lua_pushinteger(L, iResult);
	return 1;
}


int CvLuaArea::lGetNumTotalResources(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->getNumTotalResources();
	lua_pushinteger(L, iResult);
	return 1;
}


int CvLuaArea::lGetNumImprovements(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);
	const ImprovementTypes eImprovement = (ImprovementTypes)lua_tointeger(L, 2);

	const int iResult = pkArea->getNumImprovements(eImprovement);
	lua_pushinteger(L, iResult);
	return 1;
}
