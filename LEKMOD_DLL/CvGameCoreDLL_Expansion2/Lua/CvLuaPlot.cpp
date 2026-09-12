













#include <CvGameCoreDLLPCH.h>
#include "CvLuaSupport.h"
#include "CvLuaArea.h"
#include "CvLuaCity.h"
#include "CvLuaPlot.h"
#include "CvLuaUnit.h"
#include "../CvGameCoreUtils.h"


#define Method(Name)			\
	lua_pushcclosure(L, l##Name, 0);	\
	lua_setfield(L, t, #Name);


void CvLuaPlot::PushMethods(lua_State* L, int t)
{
	Method(CanHaveFeature);
	Method(GetFeatureType);
	Method(GetTerrainType);
	Method(IsRiver);
	Method(IsWater);
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
	Method(IsAllowsWalkWater);
#endif
	Method(SetFeatureType);
	Method(SetTerrainType);

	Method(IsNone);
	Method(Erase);
	Method(GetTeam);

	Method(NukeExplosion);

	Method(GetInlandCorner);

	Method(UpdateFog);
	Method(UpdateVisibility);
	Method(IsAdjacentToArea);
	Method(ShareAdjacentArea);
	Method(IsAdjacentToLand);
	Method(IsAdjacentToShallowWater);
	Method(IsCoastalLand);

	Method(IsWithinTeamCityRadius);

	Method(IsLake);
	Method(IsFreshWater);

#ifdef LEKMOD_NEW_LUA_METHODS
	Method(SetFreshWater);
#endif

	Method(IsRiverSide);
	Method(IsRiverConnection);
	Method(IsRiverCrossingFlowClockwise);

	Method(GetNearestLandArea);
	Method(SeeFromLevel);
	Method(GetNearestLandPlot);
	Method(SeeThroughLevel);
	Method(CanHaveResource);
	Method(CanHaveImprovement);
	Method(CanBuild);
	Method(GetBuildTime);
	Method(GetBuildTurnsLeft);
	Method(GetBuildTurnsTotal);
	Method(GetFeatureProduction);
#ifdef NQ_FOOD_FROM_CHOPS
	Method(GetFeatureFood);
#endif

	Method(GetBestDefender);
	Method(GetSelectedUnit);
	Method(GetUnitPower);

	Method(DefenseModifier);
	Method(MovementCost);

	Method(GetExtraMovePathCost);
	Method(ChangeExtraMovePathCost);

	Method(IsAdjacentOwned);
	Method(IsAdjacentPlayer);
	Method(IsAdjacentTeam);

	Method(IsOwned);
	Method(IsBarbarian);
	Method(IsRevealedBarbarian);
	Method(HasBarbarianCamp);
	Method(IsVisible);
	Method(IsActiveVisible);
	Method(IsVisibleToWatchingHuman);
	Method(IsAdjacentVisible);
	Method(IsAdjacentNonvisible);
	Method(IsAdjacentRevealed);
	Method(IsAdjacentNonrevealed);

	Method(RemoveGoody);
	Method(IsGoody);
	Method(IsRevealedGoody);

	Method(IsFriendlyTerritory);

	Method(IsCity);
	Method(IsFriendlyCity);
	Method(IsEnemyCity);
	Method(IsBeingWorked);

	Method(IsUnit);
	Method(IsVisibleEnemyDefender);
	Method(GetNumDefenders);
	Method(GetNumVisibleEnemyDefenders);
	Method(GetNumVisiblePotentialEnemyDefenders);
	Method(IsVisibleEnemyUnit);
	Method(IsVisibleOtherUnit);
	Method(GetNumFriendlyUnitsOfType);
	Method(IsFighting);

	Method(IsRoute);
	Method(IsTradeRoute);

	Method(IsValidDomainForLocation);
	Method(IsValidDomainForAction);
	Method(IsImpassable);

	Method(GetX);
	Method(GetY);
	Method(At);
	Method(GetPlotIndex);
	Method(GetLatitude);
	Method(Area);
	Method(WaterArea);
	Method(GetArea);
	Method(SetArea);
	Method(GetFeatureVariety);

	Method(GetOwnershipDuration);
	Method(IsOwnershipScore);
	Method(SetOwnershipDuration);
	Method(ChangeOwnershipDuration);

	Method(GetImprovementDuration);
	Method(SetImprovementDuration);
	Method(ChangeImprovementDuration);

	Method(GetUpgradeProgress);
	Method(GetUpgradeTimeLeft);

	Method(SetUpgradeProgress);
	Method(ChangeUpgradeProgress);

	Method(GetCityRadiusCount);
	Method(IsCityRadius);

	Method(IsStartingPlot);
	Method(SetStartingPlot);
	Method(IsNEOfRiver);
	Method(SetNEOfRiver);
	Method(IsWOfRiver);
	Method(SetWOfRiver);
	Method(IsNWOfRiver);
	Method(SetNWOfRiver);
	Method(GetRiverEFlowDirection);
	Method(GetRiverSEFlowDirection);
	Method(GetRiverSWFlowDirection);

	Method(IsPotentialCityWork);
	Method(IsPotentialCityWorkForArea);

	Method(GetOwner);
	Method(SetOwner);
	Method(GetPlotType);
	Method(IsFlatlands);
	Method(IsHills);
	Method(IsOpenGround);
	Method(IsRoughGround);
	Method(IsMountain);

	Method(SetPlotType);
	Method(GetResourceType);
	Method(GetNonObsoleteResourceType);
	Method(SetResourceType);
	Method(GetNumResource);
	Method(SetNumResource);
	Method(ChangeNumResource);

	Method(GetImprovementType);
	Method(SetImprovementType);
	Method(SetImprovementPillaged);
	Method(GetRouteType);
	Method(SetRouteType);
	Method(IsRoutePillaged);

	Method(GetPlotCity);
	Method(GetWorkingCity);
	Method(GetWorkingCityOverride);
	Method(GetReconCount);
	Method(GetRiverCrossingCount);
	Method(GetYield);
	Method(CalculateNatureYield);
	Method(CalculateBestNatureYield);
	Method(CalculateTotalBestNatureYield);
	Method(CalculateImprovementYieldChange);
	Method(CalculateYield);
	Method(HasYield);
	Method(GetYieldWithBuild);

	Method(CountNumAirUnits);

	Method(GetFoundValue);
	Method(IsBestAdjacentFound);

	Method(GetPlayerCityRadiusCount);
	Method(IsPlayerCityRadius);

	Method(GetVisibilityCount);
	Method(ChangeVisibilityCount);

	Method(GetRevealedOwner);
	Method(GetRevealedTeam);

	Method(IsRiverCrossing);
	Method(IsRiverCrossingToPlot);

	Method(IsRevealed);
	Method(SetRevealed);
	Method(GetRevealedImprovementType);
	Method(GetRevealedRouteType);
	Method(GetBuildProgress);
	Method(ChangeBuildProgress);

	Method(GetInvisibleVisibilityCount);
	Method(IsInvisibleVisible);
	Method(ChangeInvisibleVisibilityCount);

	Method(GetNumUnits);
	Method(GetUnit);

	Method(GetNumLayerUnits);
	Method(GetLayerUnit);

	Method(GetScriptData);
	Method(SetScriptData);

	Method(GetActiveFogOfWarMode);

	Method(IsImprovementPillaged);

	Method(CanSeePlot);

	Method(GetContinentArtType);
	Method(SetContinentArtType);

	Method(IsResourceConnectedByImprovement);

	Method(IsBuildRemovesFeature);

	Method(GetArchaeologyArtifactType);
	Method(GetArchaeologyArtifactEra);
	Method(GetArchaeologyArtifactPlayer1);
	Method(GetArchaeologyArtifactPlayer2);
	Method(GetArchaeologyArtifactWork);
	Method(HasWrittenArtifact);

	Method(GetCityPurchaseID);
	Method(SetCityPurchaseID);
}

void CvLuaPlot::HandleMissingInstance(lua_State* L)
{
	DefaultHandleMissingInstance(L);
}

const char* CvLuaPlot::GetTypeName()
{
	return "Plot";
}





int CvLuaPlot::lCanHaveFeature(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);

	const int featureType = lua_tointeger(L, 2);
	const bool canHasFeature = pkPlot->canHaveFeature((FeatureTypes)featureType);

	lua_pushboolean(L, canHasFeature);

	return 1;
}

int CvLuaPlot::lGetFeatureType(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);

	const int featureType = (int)pkPlot->getFeatureType();
	lua_pushinteger(L, featureType);

	return 1;
}

int CvLuaPlot::lGetTerrainType(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);

	lua_pushinteger(L, pkPlot->getTerrainType());

	return 1;
}

int CvLuaPlot::lIsRiver(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);

	lua_pushboolean(L, pkPlot->isRiver());

	return 1;
}

int CvLuaPlot::lIsWater(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);

	lua_pushboolean(L, pkPlot->isWater());

	return 1;
}

#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
int CvLuaPlot::lIsAllowsWalkWater(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::IsAllowsWalkWater);
}
#endif

int CvLuaPlot::lSetFeatureType(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);

	const int featureType = lua_tointeger(L, 2);
	const int variety = luaL_optinteger(L, 3, -1);


	pkPlot->setFeatureType((FeatureTypes)featureType, variety);

	return 0;
}

int CvLuaPlot::lSetTerrainType(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);

	const int terrainType = lua_tointeger(L, 2);
	const bool bRecalculate = lua_toboolean(L, 3);
	const bool bRebuildGraphics = lua_toboolean(L, 4);

	pkPlot->setTerrainType((TerrainTypes)terrainType, bRecalculate, bRebuildGraphics);

	return 0;
}









int CvLuaPlot::lIsNone(lua_State* L)
{
#ifdef AUI_WARNING_FIXES
	const bool bDoesNotExist = (GetInstance(L, 1, false) == NULL);
	lua_pushboolean(L, bDoesNotExist ? 1 : 0);
#else
	const bool bDoesNotExist = (GetInstance(L, false) == NULL);
	lua_pushboolean(L, bDoesNotExist);
#endif

	return 1;
}


int CvLuaPlot::lErase(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::erase);
}



int CvLuaPlot::lGetTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getTeam);
}



int CvLuaPlot::lNukeExplosion(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::nukeExplosion);
}



int CvLuaPlot::lUpdateFog(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::updateFog);
}



int CvLuaPlot::lUpdateVisibility(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::updateVisibility);
}


int CvLuaPlot::lIsAdjacentToArea(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	CvArea* pkArea = CvLuaArea::GetInstance(L, 2);

	const bool bResult = pkPlot->isAdjacentToArea(pkArea);
	lua_pushboolean(L, bResult);

	return 1;
}


int CvLuaPlot::lShareAdjacentArea(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	CvPlot* pkPlot2 = GetInstance(L, 2);

	const bool bResult = pkPlot->shareAdjacentArea(pkPlot2);
	lua_pushboolean(L, bResult);

	return 1;
}


int CvLuaPlot::lIsAdjacentToLand(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isAdjacentToLand);
}


int CvLuaPlot::lIsAdjacentToShallowWater(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isAdjacentToShallowWater);
}


int CvLuaPlot::lIsCoastalLand(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isCoastalLand);
}



int CvLuaPlot::lIsWithinTeamCityRadius(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isWithinTeamCityRadius);
}


int CvLuaPlot::lIsLake(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isLake);
}


int CvLuaPlot::lIsFreshWater(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isFreshWater);
}



int CvLuaPlot::lIsRiverSide(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isRiverSide);
}


int CvLuaPlot::lIsRiverConnection(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isRiverConnection);
}

int CvLuaPlot::lIsRiverCrossingFlowClockwise(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const DirectionTypes eDirection = (DirectionTypes)lua_tointeger(L, 2);
	const bool bResult = pkPlot->isRiverCrossingFlowClockwise(eDirection);
	lua_pushboolean(L, bResult);
	return 1;
}


int CvLuaPlot::lGetNearestLandArea(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getNearestLandArea);
}


int CvLuaPlot::lSeeFromLevel(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::seeFromLevel);
}


int CvLuaPlot::lGetNearestLandPlot(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	CvPlot* pkNearestPlot = pkPlot->getNearestLandPlot();
	CvLuaPlot::Push(L, pkNearestPlot);
	return 1;
}


int CvLuaPlot::lSeeThroughLevel(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::seeThroughLevel);
}


int CvLuaPlot::lCanHaveResource(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::canHaveResource);
}


int CvLuaPlot::lCanHaveImprovement(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::canHaveImprovement);
}


int CvLuaPlot::lCanBuild(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::canBuild);
}


int CvLuaPlot::lGetBuildTime(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getBuildTime);
}


int CvLuaPlot::lGetBuildTurnsLeft(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getBuildTurnsLeft);
}


int CvLuaPlot::lGetBuildTurnsTotal(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getBuildTurnsTotal);
}


int CvLuaPlot::lGetFeatureProduction(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const BuildTypes eBuild = (BuildTypes)lua_tointeger(L, 2);
	const PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 3);
	CvCity* pCity;

	const int iResult = pkPlot->getFeatureProduction(eBuild, ePlayer, &pCity);

	lua_pushinteger(L, iResult);
	return 1;
}
#ifdef NQ_FOOD_FROM_CHOPS


int CvLuaPlot::lGetFeatureFood(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const BuildTypes eBuild = (BuildTypes)lua_tointeger(L, 2);
	const PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 3);
	CvCity* pCity;

	const int iResult = pkPlot->getFeatureFood(eBuild, ePlayer, &pCity);

	lua_pushinteger(L, iResult);
	return 1;
}
#endif


int CvLuaPlot::lGetBestDefender(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const PlayerTypes eOwner = (PlayerTypes)lua_tointeger(L, 2);
	const PlayerTypes eAttackingPlayer = (PlayerTypes)lua_tointeger(L, 3);
	CvUnit* pkAttacker = CvLuaUnit::GetInstance(L, 4, false);
	const bool bTestAtWar = luaL_optint(L, 5, 0);
	const bool bTestPotentialEnemy = luaL_optint(L, 6, 0);
	const bool bTestCanMove = luaL_optint(L, 7, 0);
	UnitHandle pkUnit = pkPlot->getBestDefender(eOwner, eAttackingPlayer, pkAttacker, bTestAtWar, bTestPotentialEnemy, bTestCanMove);
	CvLuaUnit::Push(L, pkUnit.pointer());
	return 1;
}


int CvLuaPlot::lGetSelectedUnit(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	CvUnit* pkUnit = pkPlot->getSelectedUnit();
	CvLuaUnit::Push(L, pkUnit);
	return 1;
}


int CvLuaPlot::lGetUnitPower(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getUnitPower);
}



int CvLuaPlot::lDefenseModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::defenseModifier);
}


int CvLuaPlot::lMovementCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::movementCost);
}



int CvLuaPlot::lGetExtraMovePathCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getExtraMovePathCost);
}


int CvLuaPlot::lChangeExtraMovePathCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::changeExtraMovePathCost);
}



int CvLuaPlot::lIsAdjacentOwned(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isAdjacentOwned);
}


int CvLuaPlot::lIsAdjacentPlayer(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isAdjacentPlayer);
}


int CvLuaPlot::lIsAdjacentTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isAdjacentTeam);
}


int CvLuaPlot::lIsOwned(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isOwned);
}


int CvLuaPlot::lIsBarbarian(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isBarbarian);
}


int CvLuaPlot::lIsRevealedBarbarian(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isRevealedBarbarian);
}

int CvLuaPlot::lHasBarbarianCamp(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::HasBarbarianCamp);
}


int CvLuaPlot::lIsVisible(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
	const bool bDebug = luaL_optbool(L, 3, false);

	lua_pushboolean(L, pkPlot->isVisible(eTeam, bDebug));
	return 1;
}


int CvLuaPlot::lIsActiveVisible(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const bool bDebug = luaL_optbool(L, 2, false);

	if (!bDebug)
		lua_pushboolean(L, pkPlot->isActiveVisible());
	else
		lua_pushboolean(L, pkPlot->isActiveVisible(true));

	return 1;
}


int CvLuaPlot::lIsVisibleToWatchingHuman(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isVisibleToWatchingHuman);
}


int CvLuaPlot::lIsAdjacentVisible(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
	const bool bDebug = luaL_optbool(L, 3, false);

	if (!bDebug)
		lua_pushboolean(L, pkPlot->isAdjacentVisible(eTeam));
	else
		lua_pushboolean(L, pkPlot->isAdjacentVisible(eTeam, true));

	return 1;
}


int CvLuaPlot::lIsAdjacentNonvisible(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isAdjacentNonvisible);
}


int CvLuaPlot::lIsAdjacentRevealed(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isAdjacentRevealed);
}


int CvLuaPlot::lIsAdjacentNonrevealed(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isAdjacentNonrevealed);
}



int CvLuaPlot::lRemoveGoody(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::removeGoody);
}


int CvLuaPlot::lIsGoody(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isGoody);
}


int CvLuaPlot::lIsRevealedGoody(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isRevealedGoody);
}


int CvLuaPlot::lIsFriendlyTerritory(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlot->IsFriendlyTerritory(ePlayer);
	lua_pushboolean(L, bResult);
	return 1;
}



int CvLuaPlot::lIsCity(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isCity);
}


int CvLuaPlot::lIsFriendlyCity(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	CvUnit* pkUnit = CvLuaUnit::GetInstance(L, 2);
	const bool bCheckImprovement = lua_toboolean(L, 3);

	const bool bResult = pkPlot->isFriendlyCity(*pkUnit, bCheckImprovement);
	lua_pushboolean(L, bResult);
	return 1;
}


int CvLuaPlot::lIsEnemyCity(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	CvUnit* pkUnit = CvLuaUnit::GetInstance(L, 2);
	const bool bResult = pkPlot->isEnemyCity(*pkUnit);
	lua_pushboolean(L, bResult);
	return 1;
}


int CvLuaPlot::lIsBeingWorked(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isBeingWorked);
}



int CvLuaPlot::lIsUnit(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isUnit);
}


int CvLuaPlot::lIsVisibleEnemyDefender(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	CvUnit* pkUnit = CvLuaUnit::GetInstance(L, 2);
	const bool bResult = pkPlot->isVisibleEnemyDefender(pkUnit);
	lua_pushboolean(L, bResult);

	return 1;
}


int CvLuaPlot::lGetNumDefenders(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getNumDefenders);
}


int CvLuaPlot::lGetNumVisibleEnemyDefenders(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getNumVisibleEnemyDefenders);
}


int CvLuaPlot::lGetNumVisiblePotentialEnemyDefenders(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getNumVisiblePotentialEnemyDefenders);
}


int CvLuaPlot::lIsVisibleEnemyUnit(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	const bool bResult = pkPlot->isVisibleEnemyUnit(ePlayer);
	lua_pushboolean(L, bResult);
	return 1;
}


int CvLuaPlot::lIsVisibleOtherUnit(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isVisibleOtherUnit);
}


int CvLuaPlot::lGetNumFriendlyUnitsOfType(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	CvUnit* pkUnit = CvLuaUnit::GetInstance(L, 2);

	bool bBreakOnUnitLimit = luaL_optbool(L, 3, true);
	int iResult = pkPlot->getNumFriendlyUnitsOfType(pkUnit, bBreakOnUnitLimit);

	lua_pushinteger(L, iResult);
	return 1;
}


int CvLuaPlot::lIsFighting(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isFighting);
}


int CvLuaPlot::lIsRoute(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isRoute);
}

int CvLuaPlot::lIsTradeRoute(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)luaL_optint(L, 2, -1);
	bool bResult = pkPlot->IsTradeRoute(ePlayer);
	lua_pushboolean(L, bResult);
	return 1;
}


int CvLuaPlot::lIsValidDomainForLocation(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	CvUnit* pkUnit = CvLuaUnit::GetInstance(L, 2);

	const bool bResult = pkPlot->isValidDomainForLocation(*pkUnit);
	lua_pushboolean(L, bResult);
	return 1;
}


int CvLuaPlot::lIsValidDomainForAction(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	CvUnit* pkUnit = CvLuaUnit::GetInstance(L, 2);

	const bool bResult = pkPlot->isValidDomainForAction(*pkUnit);
	lua_pushboolean(L, bResult);
	return 1;
}


int CvLuaPlot::lIsImpassable(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isImpassable);
}



int CvLuaPlot::lGetX(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getX);
}


int CvLuaPlot::lGetY(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getY);
}


int CvLuaPlot::lAt(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::at);
}


int CvLuaPlot::lGetPlotIndex(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::GetPlotIndex);
}


int CvLuaPlot::lGetLatitude(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getLatitude);
}


int CvLuaPlot::lArea(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	CvArea* pkArea = pkPlot->area();
	CvLuaArea::Push(L, pkArea);
	return 1;
}


int CvLuaPlot::lWaterArea(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	CvArea* pkArea = pkPlot->waterArea();
	CvLuaArea::Push(L, pkArea);
	return 1;
}


int CvLuaPlot::lGetArea(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getArea);
}


int CvLuaPlot::lSetArea(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setArea);
}


int CvLuaPlot::lGetFeatureVariety(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getFeatureVariety);
}



int CvLuaPlot::lGetOwnershipDuration(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getOwnershipDuration);
}


int CvLuaPlot::lIsOwnershipScore(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isOwnershipScore);
}


int CvLuaPlot::lSetOwnershipDuration(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setOwnershipDuration);
}


int CvLuaPlot::lChangeOwnershipDuration(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::changeOwnershipDuration);
}


int CvLuaPlot::lGetInlandCorner(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	CvPlot* pkInlandCorner = pkPlot->getInlandCorner();

	CvLuaPlot::Push(L, pkInlandCorner);
	return 1;
}


int CvLuaPlot::lGetImprovementDuration(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getImprovementDuration);
}


int CvLuaPlot::lSetImprovementDuration(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setImprovementDuration);
}


int CvLuaPlot::lChangeImprovementDuration(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::changeImprovementDuration);
}



int CvLuaPlot::lGetUpgradeProgress(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getUpgradeProgress);
}


int CvLuaPlot::lGetUpgradeTimeLeft(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getUpgradeTimeLeft);
}



int CvLuaPlot::lSetUpgradeProgress(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setUpgradeProgress);
}


int CvLuaPlot::lChangeUpgradeProgress(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::changeUpgradeProgress);
}


int CvLuaPlot::lGetCityRadiusCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getCityRadiusCount);
}


int CvLuaPlot::lIsCityRadius(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isCityRadius);
}



int CvLuaPlot::lIsStartingPlot(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isStartingPlot);
}


int CvLuaPlot::lSetStartingPlot(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setStartingPlot);
}


int CvLuaPlot::lIsNEOfRiver(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isNEOfRiver);
}


int CvLuaPlot::lSetNEOfRiver(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setNEOfRiver);
}


int CvLuaPlot::lIsWOfRiver(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isWOfRiver);
}


int CvLuaPlot::lSetWOfRiver(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setWOfRiver);
}


int CvLuaPlot::lIsNWOfRiver(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isNWOfRiver);
}


int CvLuaPlot::lSetNWOfRiver(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setNWOfRiver);
}


int CvLuaPlot::lGetRiverEFlowDirection(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getRiverEFlowDirection);
}


int CvLuaPlot::lGetRiverSEFlowDirection(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getRiverSEFlowDirection);
}


int CvLuaPlot::lGetRiverSWFlowDirection(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getRiverSWFlowDirection);
}



int CvLuaPlot::lIsPotentialCityWork(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isPotentialCityWork);
}


int CvLuaPlot::lIsPotentialCityWorkForArea(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isPotentialCityWorkForArea);
}



int CvLuaPlot::lGetOwner(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getOwner);
}


int CvLuaPlot::lSetOwner(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setOwner);
}


int CvLuaPlot::lGetPlotType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getPlotType);
}


int CvLuaPlot::lIsFlatlands(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isFlatlands);
}


int CvLuaPlot::lIsHills(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isHills);
}


int CvLuaPlot::lIsOpenGround(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isOpenGround);
}


int CvLuaPlot::lIsRoughGround(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isRoughGround);
}


int CvLuaPlot::lIsMountain(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isMountain);
}



int CvLuaPlot::lSetPlotType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setPlotType);
}


int CvLuaPlot::lGetResourceType(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	if(pkPlot != NULL)
	{
		if(lua_gettop(L) >= 2)
		{
			TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
			ResourceTypes eResource = pkPlot->getResourceType(eTeam);
			lua_pushinteger(L, eResource);
			return 1;
		}
		else
		{
			ResourceTypes eResource = pkPlot->getResourceType(NO_TEAM);
			lua_pushinteger(L, eResource);
			return 1;
		}
	}
	else
	{
		lua_pushinteger(L, -1);
		return 1;
	}
}


int CvLuaPlot::lGetNonObsoleteResourceType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getNonObsoleteResourceType);
}


int CvLuaPlot::lSetResourceType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setResourceType);
}


int CvLuaPlot::lGetNumResource(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getNumResource);
}


int CvLuaPlot::lSetNumResource(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setNumResource);
}


int CvLuaPlot::lChangeNumResource(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::changeNumResource);
}



int CvLuaPlot::lGetImprovementType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getImprovementType);
}


int CvLuaPlot::lSetImprovementType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setImprovementType);
}


int CvLuaPlot::lSetImprovementPillaged(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::SetImprovementPillaged);
}


int CvLuaPlot::lGetRouteType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getRouteType);
}


int CvLuaPlot::lSetRouteType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setRouteType);
}

int CvLuaPlot::lIsRoutePillaged(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	lua_pushboolean(L, pkPlot->IsRoutePillaged());
	return 1;
}


int CvLuaPlot::lGetPlotCity(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	CvCity* pkCity = pkPlot->getPlotCity();
	CvLuaCity::Push(L, pkCity);
	return 1;
}


int CvLuaPlot::lGetWorkingCity(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	CvCity* pkCity = pkPlot->getWorkingCity();
	CvLuaCity::Push(L, pkCity);
	return 1;
}


int CvLuaPlot::lGetWorkingCityOverride(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	CvCity* pkCity = pkPlot->getWorkingCityOverride();
	CvLuaCity::Push(L, pkCity);
	return 1;
}


int CvLuaPlot::lGetReconCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getReconCount);
}


int CvLuaPlot::lGetRiverCrossingCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getRiverCrossingCount);
}


int CvLuaPlot::lGetYield(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 2);
	const int iResult = pkPlot->getYield(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}


int CvLuaPlot::lCalculateNatureYield(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::calculateNatureYield);
}


int CvLuaPlot::lCalculateBestNatureYield(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::calculateBestNatureYield);
}


int CvLuaPlot::lCalculateTotalBestNatureYield(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::calculateTotalBestNatureYield);
}


int CvLuaPlot::lCalculateImprovementYieldChange(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const ImprovementTypes eImprovement = (ImprovementTypes)lua_tointeger(L, 2);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L,3);
	const PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 4);
	const bool bOptional = luaL_optbool(L, 5, false);
	const RouteTypes eRoute = (RouteTypes)luaL_optint(L, 5, NUM_ROUTE_TYPES);

	const int iResult = pkPlot->calculateImprovementYieldChange(eImprovement, eYield, ePlayer, bOptional, eRoute);
	lua_pushinteger(L, iResult);
	return 1;
}


int CvLuaPlot::lCalculateYield(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::calculateYield);
}


int CvLuaPlot::lHasYield(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::hasYield);
}


int CvLuaPlot::lGetYieldWithBuild(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getYieldWithBuild);
}


int CvLuaPlot::lCountNumAirUnits(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::countNumAirUnits);
}


int CvLuaPlot::lGetFoundValue(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getFoundValue);
}


int CvLuaPlot::lIsBestAdjacentFound(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isBestAdjacentFound);
}


int CvLuaPlot::lGetPlayerCityRadiusCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getPlayerCityRadiusCount);
}


int CvLuaPlot::lIsPlayerCityRadius(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isPlayerCityRadius);
}


int CvLuaPlot::lGetVisibilityCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getVisibilityCount);
}


int CvLuaPlot::lChangeVisibilityCount(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
	const int iChange = lua_toboolean(L, 3);
	const int eSeeInvisible = lua_tointeger(L, 4);
	const bool bInformExplorationTracking = lua_toboolean(L, 5);
	const bool bAlwaysSeeInvisible = lua_toboolean(L, 6);

	pkPlot->changeVisibilityCount(eTeam, iChange, static_cast<InvisibleTypes>(eSeeInvisible), bInformExplorationTracking, bAlwaysSeeInvisible);

	return 0;
}


int CvLuaPlot::lGetRevealedOwner(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
	const bool bDebug = luaL_optbool(L, 3, false);

	if (!bDebug)
		lua_pushinteger(L, pkPlot->getRevealedOwner(eTeam));
	else
		lua_pushinteger(L, pkPlot->getRevealedOwner(eTeam, true));

	return 1;
}


int CvLuaPlot::lGetRevealedTeam(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
	const bool bDebug = luaL_optbool(L, 3, false);

	if (!bDebug)
		lua_pushinteger(L, pkPlot->getRevealedTeam(eTeam));
	else
		lua_pushinteger(L, pkPlot->getRevealedTeam(eTeam, true));

	return 1;
}


int CvLuaPlot::lIsRiverCrossing(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isRiverCrossing);
}


int CvLuaPlot::lIsRiverCrossingToPlot(lua_State* L)
{
	bool bIsCrossingRiver = false;
	CvPlot* pkMyPlot = GetInstance(L);
	CvPlot* pkOtherPlot = GetInstance(L, 2);

	if(pkMyPlot->isRiverCrossing(directionXY(pkMyPlot, pkOtherPlot)))
	{
		bIsCrossingRiver = true;
	}

	lua_pushboolean(L, bIsCrossingRiver);
	return 1;
}


int CvLuaPlot::lIsRevealed(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
	const bool bDebug = luaL_optbool(L, 3, false);

	if (!bDebug)
		lua_pushboolean(L, pkPlot->isRevealed(eTeam));
	else
		lua_pushboolean(L, pkPlot->isRevealed(eTeam, true));

	return 1;
}


int CvLuaPlot::lSetRevealed(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
	const bool bNewValue = lua_toboolean(L, 3);
	const bool bTerrainOnly = luaL_optint(L, 4, 0);
	const TeamTypes eFromTeam = (TeamTypes)luaL_optint(L, 5, NO_TEAM);
	pkPlot->setRevealed(eTeam, bNewValue, bTerrainOnly, eFromTeam);

	return 0;
}


int CvLuaPlot::lGetRevealedImprovementType(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
	const bool bDebug = luaL_optbool(L, 3, false);

	if (!bDebug)
		lua_pushinteger(L, pkPlot->getRevealedImprovementType(eTeam));
	else
		lua_pushinteger(L, pkPlot->getRevealedImprovementType(eTeam, true));

	return 1;
}


int CvLuaPlot::lGetRevealedRouteType(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
	const bool bDebug = luaL_optbool(L, 3, false);

	if (!bDebug)
		lua_pushinteger(L, pkPlot->getRevealedRouteType(eTeam));
	else
		lua_pushinteger(L, pkPlot->getRevealedRouteType(eTeam, true));

	return 1;
}


int CvLuaPlot::lGetBuildProgress(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getBuildProgress);
}


int CvLuaPlot::lChangeBuildProgress(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::changeBuildProgress);
}


int CvLuaPlot::lGetInvisibleVisibilityCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getInvisibleVisibilityCount);
}


int CvLuaPlot::lIsInvisibleVisible(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isInvisibleVisible);
}


int CvLuaPlot::lChangeInvisibleVisibilityCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::changeInvisibleVisibilityCount);
}


int CvLuaPlot::lGetNumUnits(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getNumUnits);
}


int CvLuaPlot::lGetUnit(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const int iIndex = lua_tointeger(L, 2);

	CvUnit* pkUnit = pkPlot->getUnitByIndex(iIndex);
	CvLuaUnit::Push(L, pkUnit);
	return 1;
}


int CvLuaPlot::lGetNumLayerUnits(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const int iLayerID = luaL_optinteger(L, 2, -1);

	lua_pushinteger(L, pkPlot->getNumLayerUnits(iLayerID));
	return 1;
}


int CvLuaPlot::lGetLayerUnit(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const int iIndex = lua_tointeger(L, 2);
	const int iLayerID = luaL_optinteger(L, 3, -1);

	CvUnit* pkUnit = pkPlot->getLayerUnit(iIndex, iLayerID);
	CvLuaUnit::Push(L, pkUnit);
	return 1;
}


int CvLuaPlot::lGetScriptData(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const CvString strResult = pkPlot->getScriptData();
	lua_pushstring(L, strResult.c_str());
	return 1;
}


int CvLuaPlot::lSetScriptData(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);
	const char* strNewValue = lua_tostring(L, 2);

	pkPlot->setScriptData(strNewValue);
	return 0;
}


int CvLuaPlot::lGetActiveFogOfWarMode(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L);

	int fow = 0;

	switch(pkPlot->GetActiveFogOfWarMode())
	{
	case FOGOFWARMODE_OFF:
		fow = 2;
#ifdef AUI_WARNING_FIXES
		break;
#else
	case FOGOFWARMODE_UNEXPLORED:
		fow = 0;
#endif
	case FOGOFWARMODE_NOVIS:
		fow = 1;
#ifdef AUI_WARNING_FIXES
		break;
#endif
	}

	lua_pushinteger(L, fow);
	return 1;
}


int CvLuaPlot::lIsImprovementPillaged(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::IsImprovementPillaged);
}



int CvLuaPlot::lCanSeePlot(lua_State* L)
{
	CvPlot* pkThisPlot = GetInstance(L);
	CvPlot* pkThatPlot = GetInstance(L, 2);
	TeamTypes eTeam = (TeamTypes) lua_tointeger(L, 3);
	int iRange = lua_tointeger(L, 4);
	DirectionTypes eFacingDirection = (DirectionTypes) lua_tointeger(L, 5);

	bool bCanSee = false;
	if(pkThisPlot)
	{
		bCanSee = pkThisPlot->canSeePlot(pkThatPlot, eTeam, iRange, eFacingDirection);
	}

	lua_pushboolean(L, bCanSee);
	return 1;

}


int CvLuaPlot::lGetContinentArtType(lua_State* L)
{
	CvPlot* pkThisPlot = GetInstance(L);
	if(pkThisPlot)
	{
		lua_pushinteger(L, pkThisPlot->GetContinentType());
	}
	else
	{
		lua_pushinteger(L, 0);
	}
	return 1;
}


int CvLuaPlot::lSetContinentArtType(lua_State* L)
{
	CvPlot* pkThisPlot = GetInstance(L);
	if(pkThisPlot)
	{
		const int iContinentType = lua_tointeger(L, 2);
		pkThisPlot->SetContinentType((char) iContinentType);
	}
	return 0;
}



int CvLuaPlot::lIsResourceConnectedByImprovement(lua_State* L)
{
	bool bResult = false;
	CvPlot* kPlot = GetInstance(L);
	const ImprovementTypes eImprovement = (ImprovementTypes) lua_tointeger(L, 2);
	CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
	if(pkImprovementInfo)
	{
		bResult = pkImprovementInfo->IsImprovementResourceTrade(kPlot->getResourceType(GC.getGame().getActiveTeam()));
	}

	lua_pushboolean(L, bResult);
	return 1;
}


int CvLuaPlot::lIsBuildRemovesFeature(lua_State* L)
{
	CvPlot* kPlot = GetInstance(L);
	const BuildTypes eBuild = (BuildTypes) lua_tointeger(L, 2);
	bool bFeatureRemoved = false;

	CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
	if(NULL != pkBuildInfo && kPlot->getFeatureType() != NO_FEATURE)
		bFeatureRemoved = pkBuildInfo->isFeatureRemove(kPlot->getFeatureType());

	lua_pushboolean(L, bFeatureRemoved);
	return 1;
}


int CvLuaPlot::lGetArchaeologyArtifactType(lua_State* L)
{
	CvPlot* kPlot = GetInstance(L);
	int iType = kPlot->GetArchaeologicalRecord().m_eArtifactType;
	lua_pushinteger(L, iType);
	return 1;
}


int CvLuaPlot::lGetArchaeologyArtifactEra(lua_State* L)
{
	CvPlot* kPlot = GetInstance(L);
	int iEra = kPlot->GetArchaeologicalRecord().m_eEra;
	lua_pushinteger(L, iEra);
	return 1;
}


int CvLuaPlot::lGetArchaeologyArtifactPlayer1(lua_State* L)
{
	CvPlot* kPlot = GetInstance(L);
	int iPlayer = kPlot->GetArchaeologicalRecord().m_ePlayer1;
	lua_pushinteger(L, iPlayer);
	return 1;
}


int CvLuaPlot::lGetArchaeologyArtifactPlayer2(lua_State* L)
{
	CvPlot* kPlot = GetInstance(L);
	int iPlayer = kPlot->GetArchaeologicalRecord().m_ePlayer2;
	lua_pushinteger(L, iPlayer);
	return 1;
}


int CvLuaPlot::lGetArchaeologyArtifactWork(lua_State* L)
{
	CvPlot* kPlot = GetInstance(L);
	int iWork = (int)kPlot->GetArchaeologicalRecord().m_eWork;
	lua_pushinteger(L, iWork);
	return 1;
}



int CvLuaPlot::lHasWrittenArtifact(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::HasWrittenArtifact);
}



int CvLuaPlot::lGetCityPurchaseID(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::GetCityPurchaseID);
}



int CvLuaPlot::lSetCityPurchaseID(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::SetCityPurchaseID);
}

#ifdef LEKMOD_NEW_LUA_METHODS


int CvLuaPlot::lSetFreshWater(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setFreshWater);
}
#endif
