#include "CvGameCoreDLLPCH.h"
#include "CvPlot.h"
#include "CvCity.h"
#include "CvUnit.h"
#include "CvGlobals.h"
#include "CvUnitMovement.h"
#include "CvGameCoreUtils.h"

void CvUnitMovement::GetCostsForMove(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iBaseMoves, int& iRegularCost, int& iRouteCost, int& iRouteFlatCost)
{
	CvPlayerAI& kPlayer = GET_PLAYER(pUnit->getOwner());
	CvPlayerTraits* pTraits = kPlayer.GetPlayerTraits();
	bool bFasterAlongRiver = pTraits->IsFasterAlongRiver();
	bool bFasterInHills = pTraits->IsFasterInHills();
	bool bIgnoreTerrainCost = pUnit->ignoreTerrainCost();

	TeamTypes eUnitTeam = pUnit->getTeam();
	CvTeam& kUnitTeam = GET_TEAM(eUnitTeam);
	int iMoveDenominator = GC.getMOVE_DENOMINATOR();
	bool bRiverCrossing = pFromPlot->isRiverCrossing(directionXY(pFromPlot, pToPlot));
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)


	if (bRiverCrossing && (pFromPlot->IsAllowsWalkWater() || pToPlot->IsAllowsWalkWater()))
	{
		bRiverCrossing = false;
	}
#endif
	FeatureTypes eFeature = pToPlot->getFeatureType();
	CvFeatureInfo* pFeatureInfo = (eFeature > NO_FEATURE) ? GC.getFeatureInfo(eFeature) : 0;
	TerrainTypes eTerrain = pToPlot->getTerrainType();
	CvTerrainInfo* pTerrainInfo = (eTerrain > NO_TERRAIN) ? GC.getTerrainInfo(eTerrain) : 0;
	
	
	


	if(bIgnoreTerrainCost || (bFasterAlongRiver && pToPlot->isRiver() && pFromPlot->isRiver()) || (bFasterInHills && pToPlot->isHills()))
	{
		iRegularCost = 1;
	}
	else
	{
		iRegularCost = ((eFeature == NO_FEATURE) ? (pTerrainInfo ? pTerrainInfo->getMovementCost() : 0) : (pFeatureInfo ? pFeatureInfo->getMovementCost() : 0));


		if((PlotTypes)pToPlot->getPlotType() == PLOT_HILLS && !pToPlot->isCity())
		{
			iRegularCost += GC.getHILLS_EXTRA_MOVEMENT();
		}

		if(iRegularCost > 0)
		{
			iRegularCost = std::max(1, (iRegularCost - pUnit->getExtraMoveDiscount()));
		}

		
	}


	if ((pToPlot->isRoughGround() && pUnit->IsRoughTerrainEndsTurn()) || (!(bIgnoreTerrainCost || bFasterAlongRiver) && bRiverCrossing))
	{
		iRegularCost = INT_MAX;
	}
	else
	{
		iRegularCost *= iMoveDenominator;

		if(pToPlot->isHills() && pUnit->isHillsDoubleMove())
		{
			iRegularCost /= 2;
		}

		else if((eFeature == NO_FEATURE) ? pUnit->isTerrainDoubleMove(eTerrain) : pUnit->isFeatureDoubleMove(eFeature))
		{
			iRegularCost /= 2;
		}
	}

	if(pFromPlot->isValidRoute(pUnit) && pToPlot->isValidRoute(pUnit) && (kUnitTeam.isBridgeBuilding() || !bRiverCrossing))
	{
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
		RouteTypes eFromPlotRoute = pFromPlot->GetEffectiveRouteType(pUnit);
		RouteTypes eToPlotRoute = pToPlot->GetEffectiveRouteType(pUnit);
#else
		RouteTypes eFromPlotRoute = pFromPlot->getRouteType();
		RouteTypes eToPlotRoute = pToPlot->getRouteType();
#endif
		if (pTraits->IsMoveFriendlyWoodsAsRoad())
		{
			if (eFromPlotRoute == NO_ROUTE)
				eFromPlotRoute = ROUTE_ROAD;
			if (eToPlotRoute == NO_ROUTE)
				eToPlotRoute = ROUTE_ROAD;
		}
		CvRouteInfo* pFromRouteInfo = GC.getRouteInfo(eFromPlotRoute);

		CvAssert(pFromRouteInfo != NULL);

		int iFromMovementCost = (pFromRouteInfo != NULL)? pFromRouteInfo->getMovementCost() : 0;
		int iFromFlatMovementCost = (pFromRouteInfo != NULL)? pFromRouteInfo->getFlatMovementCost() : 0;

		CvRouteInfo* pRouteInfo = GC.getRouteInfo(eToPlotRoute);

		CvAssert(pRouteInfo != NULL);

		int iMovementCost = (pRouteInfo != NULL)? pRouteInfo->getMovementCost() : 0;
		int iFlatMovementCost = (pRouteInfo != NULL)? pRouteInfo->getFlatMovementCost() : 0;



		iRouteCost = std::max(iFromMovementCost + kUnitTeam.getRouteChange(eFromPlotRoute), iMovementCost + kUnitTeam.getRouteChange(eToPlotRoute));

		iRouteFlatCost = std::max(iFromFlatMovementCost * iBaseMoves, iFlatMovementCost * iBaseMoves);
	}
	else if (pTraits->IsMoveFriendlyWoodsAsRoad() && pUnit->getOwner() == pToPlot->getOwner() && (eFeature == FEATURE_FOREST || eFeature == FEATURE_JUNGLE))
	{
		CvRouteInfo* pRoadInfo = GC.getRouteInfo(ROUTE_ROAD);
		iRouteCost = pRoadInfo->getMovementCost() + kUnitTeam.getRouteChange(ROUTE_ROAD);

		iRouteFlatCost = pRoadInfo->getFlatMovementCost() * iBaseMoves;
	}
	else
	{
		iRouteCost = INT_MAX;
		iRouteFlatCost = INT_MAX;
	}

	if(pUnit->getDomainType() == DOMAIN_SEA && pToPlot->IsAllowsSailLand())
	{
			iRegularCost = iMoveDenominator*3;
			iRouteCost = iRegularCost;
			iRouteFlatCost = iRegularCost;
	}
	if(pUnit->getDomainType() == DOMAIN_LAND && pToPlot->IsAllowsSailLand() && (!bIgnoreTerrainCost))
	{ 
			iRegularCost = iMoveDenominator;
			iRouteCost = iRegularCost;
			iRouteFlatCost = iRegularCost;
	}
}


int CvUnitMovement::MovementCost(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iBaseMoves, int iMaxMoves, int iMovesRemaining        )
{
	int iRegularCost;
	int iRouteCost;
	int iRouteFlatCost;

	CvAssertMsg(pToPlot->getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	if(ConsumesAllMoves(pUnit, pFromPlot, pToPlot))
	{
		if (iMovesRemaining > 0)
			return iMovesRemaining;
		else
			return iMaxMoves;
	}
	else if(CostsOnlyOne(pUnit, pFromPlot, pToPlot))
	{
		return GC.getMOVE_DENOMINATOR();
	}
	else if(IsSlowedByZOC(pUnit, pFromPlot, pToPlot))
	{
		if (iMovesRemaining > 0)
			return iMovesRemaining;
		else
			return iMaxMoves;
	}

	GetCostsForMove(pUnit, pFromPlot, pToPlot, iBaseMoves, iRegularCost, iRouteCost, iRouteFlatCost);

	return std::max(1, std::min(iRegularCost, std::min(iRouteCost, iRouteFlatCost)));
}


int CvUnitMovement::MovementCostNoZOC(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iBaseMoves, int iMaxMoves, int iMovesRemaining        )
{
	int iRegularCost;
	int iRouteCost;
	int iRouteFlatCost;

	CvAssertMsg(pToPlot->getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	if(ConsumesAllMoves(pUnit, pFromPlot, pToPlot))
	{
		if (iMovesRemaining > 0)
			return iMovesRemaining;
		else
			return iMaxMoves;
	}
	else if(CostsOnlyOne(pUnit, pFromPlot, pToPlot))
	{
		return GC.getMOVE_DENOMINATOR();
	}

	GetCostsForMove(pUnit, pFromPlot, pToPlot, iBaseMoves, iRegularCost, iRouteCost, iRouteFlatCost);

	return std::max(1, std::min(iRegularCost, std::min(iRouteCost, iRouteFlatCost)));
}


bool CvUnitMovement::ConsumesAllMoves(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot)
{

	

	if(!pToPlot->isRevealed(pUnit->getTeam()) && pUnit->isHuman())
	{
		return true;
	}

#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)


	if (pUnit->CanEverEmbark() && !pUnit->IsHoveringUnit() && !pUnit->canMoveAllTerrain())
	{
		const bool bFromLandLike = !pFromPlot->isWater() || pFromPlot->IsAllowsWalkWater();
		const bool bToLandLike = !pToPlot->isWater() || pToPlot->IsAllowsWalkWater();

		if (bFromLandLike != bToLandLike)
		{

			if (bToLandLike && !bFromLandLike && GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->IsEmbarkedToLandFlatCost())
			{
				return false;
			}
#ifdef LEKMOD_TRAIT_CIVILIAN_EMBARK_ONE_MOVE

			if (!bToLandLike && bFromLandLike && !pUnit->IsCombatUnit() &&
				GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->IsCiviliansEmbarkOneMove())
			{
				return false;
			}
#endif
			return true;
		}


		if (pFromPlot->IsAllowsWalkWater() || pToPlot->IsAllowsWalkWater())
		{
			return false;
		}
	}
#else
	if (!pUnit->isEmbarked() && (pToPlot->IsAllowsWalkWater() || pFromPlot->IsAllowsWalkWater()))
	{
		return false;
	}
#endif


	if(!pFromPlot->isValidDomainForLocation(*pUnit))
	{

		if(pUnit->getDomainType() != DOMAIN_LAND || pUnit->IsHoveringUnit() || pUnit->canMoveAllTerrain() || !pUnit->CanEverEmbark())
			return true;
	}

#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
	const bool bFromWaterForEmbark = pFromPlot->isWater() && !pFromPlot->IsAllowsWalkWater();
	const bool bToWaterForEmbark = pToPlot->isWater() && !pToPlot->IsAllowsWalkWater();
	if (bFromWaterForEmbark != bToWaterForEmbark && pUnit->CanEverEmbark())
#else
	if(pToPlot->isWater() != pFromPlot->isWater() && pUnit->CanEverEmbark())
#endif

	{

		

#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
		if (bFromWaterForEmbark && !bToWaterForEmbark && GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->IsEmbarkedToLandFlatCost())
#else
		if (!pToPlot->isWater() && pFromPlot->isWater() && GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->IsEmbarkedToLandFlatCost())
#endif
		{
			return false;
		}

#ifdef LEKMOD_TRAIT_CIVILIAN_EMBARK_ONE_MOVE

#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
    if (bToWaterForEmbark && !bFromWaterForEmbark)
#else
    if (pToPlot->isWater() && !pFromPlot->isWater())
#endif
    {
        if (!pUnit->IsCombatUnit() && GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->IsCiviliansEmbarkOneMove())
        {
            return false;
        }
    }
#endif

		
		
		if(!pUnit->canMoveAllTerrain())
		{
			return true;
		}
	}
	

	return false;
}


bool CvUnitMovement::CostsOnlyOne(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot)
{
	
	if(!pToPlot->isValidDomainForAction(*pUnit))
	{

		if(pUnit->getDomainType() != DOMAIN_LAND || pUnit->IsHoveringUnit() || pUnit->canMoveAllTerrain() || !pUnit->CanEverEmbark())
			return true;
	}

	CvAssert(!pUnit->IsImmobile());

	
	if(pUnit->flatMovementCost() || pUnit->getDomainType() == DOMAIN_AIR)
	{
		return true;
	}



	if (pUnit->CanEverEmbark() && GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->IsEmbarkedToLandFlatCost())
	{
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
		const bool bFromWaterForEmbark = pFromPlot->isWater() && !pFromPlot->IsAllowsWalkWater();
		const bool bToLandLike = !pToPlot->isWater() || pToPlot->IsAllowsWalkWater();
		if (bFromWaterForEmbark && bToLandLike)
		{
			return true;
		}
#else
		if (!pToPlot->isWater() && pFromPlot->isWater())
		{
			return true;
		}
#endif
	}
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)


#ifdef LEKMOD_TRAIT_CIVILIAN_EMBARK_ONE_MOVE
	if (!pToPlot->IsAllowsWalkWater() && pToPlot->isWater() && pFromPlot->IsAllowsWalkWater() &&
		pUnit->CanEverEmbark() && !pUnit->IsCombatUnit() &&
		GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->IsCiviliansEmbarkOneMove())
	{
		return true;
	}
#endif
#endif
#ifdef LEKMOD_TRAIT_CIVILIAN_EMBARK_ONE_MOVE

#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
    if (pToPlot->isWater() && !pToPlot->IsAllowsWalkWater() && !pFromPlot->isWater() && pUnit->CanEverEmbark())
#else
    if (pToPlot->isWater() && !pFromPlot->isWater() && pUnit->CanEverEmbark())
#endif
    {
        if (!pUnit->IsCombatUnit() && GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->IsCiviliansEmbarkOneMove())
        {
            return true;
        }
    }
#endif

	return false;
}


bool CvUnitMovement::IsSlowedByZOC(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot)
{
	if (pUnit->IsIgnoreZOC() || CostsOnlyOne(pUnit, pFromPlot, pToPlot))
	{
		return false;
	}


	if(GC.getZONE_OF_CONTROL_ENABLED() > 0)
	{
		IDInfo* pAdjUnitNode;
		CvUnit* pLoopUnit;

		int iFromPlotX = pFromPlot->getX();
		int iFromPlotY = pFromPlot->getY();
		int iToPlotX = pToPlot->getX();
		int iToPlotY = pToPlot->getY();
		TeamTypes unit_team_type     = pUnit->getTeam();
		DomainTypes unit_domain_type = pUnit->getDomainType();
		bool bIsVisibleEnemyUnit     = pToPlot->isVisibleEnemyUnit(pUnit);
		CvTeam& kUnitTeam = GET_TEAM(unit_team_type);

		for(int iDirection0 = 0; iDirection0 < NUM_DIRECTION_TYPES; iDirection0++)
		{
			CvPlot* pAdjPlot = plotDirection(iFromPlotX, iFromPlotY, ((DirectionTypes)iDirection0));
			if(NULL != pAdjPlot)
			{


				if (pAdjPlot->isEnemyCity(*pUnit) && (pAdjPlot->isRevealed(pUnit->getTeam()) || pUnit->plot() == pFromPlot))
				{

					for(int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++)
					{
						CvPlot* pEnemyAdjPlot = plotDirection(pAdjPlot->getX(), pAdjPlot->getY(), ((DirectionTypes)iDirection));
						if(NULL != pEnemyAdjPlot)
						{

							if(pEnemyAdjPlot->getX() == iToPlotX && pEnemyAdjPlot->getY() == iToPlotY)
							{
								return true;
							}
						}
					}
				}
				if (!pAdjPlot->isVisible(pUnit->getTeam()) && pUnit->plot() != pFromPlot)
					continue;

				pAdjUnitNode = pAdjPlot->headUnitNode();

				while(pAdjUnitNode != NULL)
				{
					if((pAdjUnitNode->eOwner >= 0) && pAdjUnitNode->eOwner < MAX_PLAYERS)
					{
						pLoopUnit = (GET_PLAYER(pAdjUnitNode->eOwner).getUnit(pAdjUnitNode->iID));
					}
					else
					{
						pLoopUnit = NULL;
					}

					pAdjUnitNode = pAdjPlot->nextUnitNode(pAdjUnitNode);

					if(!pLoopUnit) continue;

					if (pLoopUnit->isDelayedDeath())
						continue;


					TeamTypes unit_loop_team_type = pLoopUnit->getTeam();

					if(pLoopUnit->isInvisible(unit_team_type,false)) continue;


					if(!pLoopUnit->IsCombatUnit())
					{
						continue;
					}


					if(unit_loop_team_type == BARBARIAN_TEAM || kUnitTeam.isAtWar(unit_loop_team_type))
					{



						DomainTypes loop_unit_domain_type = pLoopUnit->getDomainType();
						if(loop_unit_domain_type != unit_domain_type)
						{

							if(loop_unit_domain_type == DOMAIN_SEA && unit_domain_type)
							{

							}
							else
							{
								continue;
							}
						}


						if(unit_domain_type == DOMAIN_LAND && pLoopUnit->isEmbarked())
						{
							continue;
						}


						for(int iDirection2 = 0; iDirection2 < NUM_DIRECTION_TYPES; iDirection2++)
						{
							CvPlot* pEnemyAdjPlot = plotDirection(pAdjPlot->getX(), pAdjPlot->getY(), ((DirectionTypes)iDirection2));
							if(!pEnemyAdjPlot)
							{
								continue;
							}


							if(!bIsVisibleEnemyUnit)
							{

								if(pEnemyAdjPlot->getX() == iToPlotX && pEnemyAdjPlot->getY() == iToPlotY)
								{
									return true;
								}
							}
						}
					}
				}
			}
		}
	}
	return false;
}
