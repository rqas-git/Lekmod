






#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreUtils.h"
#include "CvSiteEvaluationClasses.h"
#include "CvImprovementClasses.h"
#include "CvCitySpecializationAI.h"
#include "CvDiplomacyAI.h"
#include "CvGrandStrategyAI.h"


#include "LintFree.h"





CvCitySiteEvaluator::CvCitySiteEvaluator(void)
{
	m_iExpansionIndex = 12;
	m_iGrowthIndex = 13;
}


CvCitySiteEvaluator::~CvCitySiteEvaluator(void)
{
}


void CvCitySiteEvaluator::Init()
{

	m_iRingModifier[0] = 1;
	m_iRingModifier[1] =       GC.getCITY_RING_1_MULTIPLIER();
	m_iRingModifier[2] =       GC.getCITY_RING_2_MULTIPLIER();
	m_iRingModifier[3] =       GC.getCITY_RING_3_MULTIPLIER();
	m_iRingModifier[4] =       GC.getCITY_RING_4_MULTIPLIER();
	m_iRingModifier[5] =       GC.getCITY_RING_5_MULTIPLIER();
	m_iRingModifier[6] = 0;
	m_iRingModifier[7] = 0;

	m_iGrowthIndex = GC.getInfoTypeForString("FLAVOR_GROWTH");
	m_iExpansionIndex = GC.getInfoTypeForString("FLAVOR_EXPANSION");
	m_iNavalIndex = GC.getInfoTypeForString("FLAVOR_NAVAL");

	m_iBrazilMultiplier = 1000;
	m_iSpainMultiplier = 55000;
	m_iMorrocoMultiplier = 1000;
	m_iNetherlandsMultiplier = 2000;
	m_iIncaMultiplier = 500;
}


bool CvCitySiteEvaluator::CanFound(CvPlot* pPlot, const CvPlayer* pPlayer, bool bTestVisible) const
{
	CvAssert(pPlot);
	if(!pPlot)
		return false;

	CvPlot* pLoopPlot(NULL);
	bool bValid(false);
	int iRange(0), iDX(0), iDY(0);



	if(GC.getGame().isFinalInitialized())
	{
#ifdef NQM_AI_GIMP_NO_BUILDING_SETTLERS
		if (pPlayer && (pPlayer->isHuman() && GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE)) || (!pPlayer->isHuman() && (GC.getGame().isOption("GAMEOPTION_AI_TWEAKS") || GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_BUILDING_SETTLERS"))))
#else
		if(GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && pPlayer && pPlayer->isHuman())
#endif
		{
			if(pPlayer->getNumCities() > 0)
			{
				return false;
			}
		}
	}

	if(pPlot->isImpassable() || pPlot->isMountain())
	{
		return false;
	}

	if(pPlot->getFeatureType() != NO_FEATURE)
	{
		if(GC.getFeatureInfo(pPlot->getFeatureType())->isNoCity())
		{
			return false;
		}
	}

	if(pPlayer)
	{
		if(pPlot->isOwned() && (pPlot->getOwner() != pPlayer->GetID()))
		{
			return false;
		}
	}

	CvTerrainInfo* pTerrainInfo = GC.getTerrainInfo(pPlot->getTerrainType());

	if(!bValid)
	{
		if(pTerrainInfo->isFound())
		{
			bValid = true;
		}
	}

	if(!bValid)
	{
		if(pTerrainInfo->isFoundCoast())
		{
			if(pPlot->isCoastalLand())
			{
				bValid = true;
			}
		}
	}

	if(!bValid)
	{
		if(pTerrainInfo->isFoundFreshWater())
		{
			if(pPlot->isFreshWater())
			{
				bValid = true;
			}
		}
	}



	if(pPlot->isWater())
	{
		return false;
	}

	if(!bValid)
	{
		return false;
	}

	if(!bTestVisible)
	{

		iRange = GC.getMIN_CITY_RANGE();

#ifdef AUI_HEXSPACE_DX_LOOPS
		int iMaxDX;
		for (iDY = -iRange; iDY <= iRange; iDY++)
		{
			iMaxDX = iRange - MAX(0, iDY);
			for (iDX = -iRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
			{
				pLoopPlot = plotXY(pPlot->getX(), pPlot->getY(), iDX, iDY);
#else
		for(iDX = -(iRange); iDX <= iRange; iDX++)
		{
			for(iDY = -(iRange); iDY <= iRange; iDY++)
			{
				pLoopPlot = plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iDX, iDY, iRange);
#endif

				if(pLoopPlot != NULL)
				{
					if(pLoopPlot->isCity())
					{
						if(pLoopPlot->getLandmass() == pPlot->getLandmass())
						{
							return false;
						}
						else if(hexDistance(iDX, iDY) < iRange)
						{
							return false;
						}
					}
				}
			}
		}
	}

	return true;
}


void CvCitySiteEvaluator::ComputeFlavorMultipliers(CvPlayer* pPlayer)
{

	for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_iFlavorMultiplier[iI] = 0;
	}

	m_iFlavorMultiplier[SITE_EVALUATION_HAPPINESS] = 0;


	CitySpecializationTypes eNextSpecialization = pPlayer->GetCitySpecializationAI()->GetNextSpecializationDesired();
	CvCitySpecializationXMLEntry* pkCitySpecializationEntry = NULL;
	if(eNextSpecialization != NO_CITY_SPECIALIZATION)
		pkCitySpecializationEntry = GC.getCitySpecializationInfo(eNextSpecialization);


	for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		const FlavorTypes eFlavor = static_cast<FlavorTypes>(iFlavorLoop);
		const CvString& strFlavor = GC.getFlavorTypes(eFlavor);
		if(strFlavor == "FLAVOR_GROWTH" ||
		        strFlavor == "FLAVOR_EXPANSION")
		{
			m_iFlavorMultiplier[YIELD_FOOD] += pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor);
			if(pkCitySpecializationEntry)
			{
				m_iFlavorMultiplier[YIELD_FOOD] += pkCitySpecializationEntry->GetFlavorValue(eFlavor);
			}
		}
		else if(strFlavor == "FLAVOR_GOLD" ||
		        strFlavor == "FLAVOR_TILE_IMPROVEMENT")
		{
			m_iFlavorMultiplier[YIELD_GOLD] += pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor);
			if(pkCitySpecializationEntry)
			{
				m_iFlavorMultiplier[YIELD_GOLD] += pkCitySpecializationEntry->GetFlavorValue(eFlavor);
			}
		}
		else if(strFlavor == "FLAVOR_PRODUCTION" ||
		        strFlavor == "FLAVOR_WONDER")
		{
			m_iFlavorMultiplier[YIELD_PRODUCTION] += pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor);
			if(pkCitySpecializationEntry)
			{
				m_iFlavorMultiplier[YIELD_PRODUCTION] += pkCitySpecializationEntry->GetFlavorValue(eFlavor);
			}
		}
		else if(strFlavor == "FLAVOR_SCIENCE")
		{

			m_iFlavorMultiplier[YIELD_SCIENCE] += pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor) * 2;
			if(pkCitySpecializationEntry)
			{
				m_iFlavorMultiplier[YIELD_SCIENCE] += pkCitySpecializationEntry->GetFlavorValue(eFlavor) * 2;
			}
		}
		else if(strFlavor == "FLAVOR_HAPPINESS")
		{

			m_iFlavorMultiplier[SITE_EVALUATION_HAPPINESS] += pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor) * 2;
			if(pkCitySpecializationEntry)
			{
				m_iFlavorMultiplier[SITE_EVALUATION_HAPPINESS] += pkCitySpecializationEntry->GetFlavorValue(eFlavor) * 2;
			}
		}
		else if(strFlavor == "FLAVOR_RELIGION")
		{

			m_iFlavorMultiplier[YIELD_FAITH] += pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor) * 2;
			if (pkCitySpecializationEntry)
			{
				m_iFlavorMultiplier[YIELD_FAITH] += pkCitySpecializationEntry->GetFlavorValue(eFlavor) * 2;
			}
		}
	}


	for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if(m_iFlavorMultiplier[iI] < 0)
		{
			m_iFlavorMultiplier[iI] = 0;
		}
	}


	m_iFlavorMultiplier[SITE_EVALUATION_RESOURCES] = 10;
	m_iFlavorMultiplier[SITE_EVALUATION_STRATEGIC] = 10;
}


int CvCitySiteEvaluator::PlotFoundValue(CvPlot* pPlot, CvPlayer* pPlayer, YieldTypes eYield, bool)
{
	CvAssert(pPlot);
	if(!pPlot)
		return 0;
#ifdef AUI_WARNING_FIXES
	if (!pPlayer)
		return 0;
#endif


	if(!CanFound(pPlot, pPlayer, false))
	{
		return 0;
	}

	int rtnValue = 0;

	int iFoodValue = 0;
	int iHappinessValue = 0;
	int iProductionValue = 0;
	int iGoldValue = 0;
	int iScienceValue = 0;
	int iFaithValue = 0;
	int iResourceValue = 0;
	int iStrategicValue = 0;

	int iCelticForestCount = 0;
	int iIroquoisForestCount = 0;
	int iBrazilJungleCount = 0;
	int iNaturalWonderCount = 0;
	int iDesertCount = 0;
	int iWetlandsCount = 0;

	int iTotalFoodValue = 0;
	int iTotalHappinessValue = 0;
	int iTotalProductionValue = 0;
	int iTotalGoldValue = 0;
	int iTotalScienceValue = 0;
	int iTotalFaithValue = 0;
	int iTotalResourceValue = 0;
	int iTotalStrategicValue = 0;

	int iClosestCityOfMine = 999;
	int iClosestEnemyCity = 999;

	int iCapitalArea = NULL;

	bool bIsInca = false;
	int iAdjacentMountains = 0;

	if ( pPlayer->getCapitalCity() )
		iCapitalArea = pPlayer->getCapitalCity()->getArea();


	ImprovementTypes eIncaImprovement = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_TERRACE_FARM", true);  
	if(eIncaImprovement != NO_IMPROVEMENT)
	{
		CvImprovementEntry* pkEntry = GC.getImprovementInfo(eIncaImprovement);
		if(pkEntry != NULL && pkEntry->IsSpecificCivRequired())
		{
			CivilizationTypes eCiv = pkEntry->GetRequiredCivilization();
			if(eCiv == pPlayer->getCivilizationType())
			{
				bIsInca = true;
			}
		}
	}

#ifdef AUI_HEXSPACE_DX_LOOPS
	int iDX, iMaxDX;
	for (int iDY = -7; iDY <= 7; iDY++)
	{
		iMaxDX = 7 - MAX(0, iDY);
		for (iDX = -7 - MIN(0, iDY); iDX <= iMaxDX; iDX++)
#else
	for (int iDX = -7; iDX <= 7; iDX++)
	{
		for (int iDY = -7; iDY <= 7; iDY++)
#endif
		{
			CvPlot* pLoopPlot = plotXY(pPlot->getX(), pPlot->getY(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
#ifdef AUI_FIX_HEX_DISTANCE_INSTEAD_OF_PLOT_DISTANCE
				int iDistance = hexDistance(iDX, iDY);
#else
				int iDistance = plotDistance(pPlot->getX(), pPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY());
#endif
#ifndef AUI_HEXSPACE_DX_LOOPS
				if (iDistance <= 7)
#endif
				{
					if ((pLoopPlot->getOwner() == NO_PLAYER) || (pLoopPlot->getOwner() == pPlayer->GetID()))
					{

						if (iClosestCityOfMine > iDistance)
						{
							if (pLoopPlot->isCity())
							{
								iClosestCityOfMine = iDistance;
							}
						}


						if (iDistance <= 5)
						{
							int iRingModifier = m_iRingModifier[iDistance];

							iFoodValue = 0;
							iProductionValue = 0;
							iGoldValue = 0;
							iScienceValue = 0;
							iHappinessValue = 0;
							iResourceValue = 0;
							iStrategicValue = 0;

							if (iDistance > 0 && iDistance <= NUM_CITY_RINGS)
							{
								if (eYield == NO_YIELD || eYield == YIELD_FOOD)
								{
									iFoodValue = iRingModifier * ComputeFoodValue(pLoopPlot, pPlayer) *        GC.getSETTLER_FOOD_MULTIPLIER();
								}
								if (eYield == NO_YIELD || eYield == YIELD_PRODUCTION)
								{
									iProductionValue = iRingModifier * ComputeProductionValue(pLoopPlot, pPlayer) *       GC.getSETTLER_PRODUCTION_MULTIPLIER();
								}
								if (eYield == NO_YIELD || eYield == YIELD_GOLD)
								{
									iGoldValue = iRingModifier * ComputeGoldValue(pLoopPlot, pPlayer) *       GC.getSETTLER_GOLD_MULTIPLIER();
								}
								if (eYield == NO_YIELD || eYield == YIELD_SCIENCE)
								{
									iScienceValue = iRingModifier * ComputeScienceValue(pLoopPlot, pPlayer) *       GC.getSETTLER_SCIENCE_MULTIPLIER();
								}
								if (eYield == NO_YIELD || eYield == YIELD_FAITH)
								{
									iFaithValue = iRingModifier * ComputeFaithValue(pLoopPlot, pPlayer) *       GC.getSETTLER_FAITH_MULTIPLIER();
								}
							}


							if (iDistance <= 5 && pLoopPlot->getOwner() == NO_PLAYER)
							{
								iHappinessValue = iRingModifier * ComputeHappinessValue(pLoopPlot, pPlayer) *       GC.getSETTLER_HAPPINESS_MULTIPLIER();
								iResourceValue = iRingModifier * ComputeTradeableResourceValue(pLoopPlot, pPlayer) *       GC.getSETTLER_RESOURCE_MULTIPLIER();
								if (iDistance)
									iStrategicValue = ComputeStrategicValue(pLoopPlot, pPlayer, iDistance) *       GC.getSETTLER_STRATEGIC_MULTIPLIER();
							}

							iTotalFoodValue += iFoodValue;
							iTotalHappinessValue += iHappinessValue;
							iTotalProductionValue += iProductionValue;
							iTotalGoldValue += iGoldValue;
							iTotalScienceValue += iScienceValue;
							iTotalFaithValue += iFaithValue;
							iTotalResourceValue += iResourceValue;
							iTotalStrategicValue += iStrategicValue;

							int iPlotValue = iFoodValue + iHappinessValue + iProductionValue + iGoldValue + iScienceValue + iFaithValue + iResourceValue;
							
							if (iPlotValue == 0)
							{

								iPlotValue -= iRingModifier * GC.getSETTLER_FOOD_MULTIPLIER() * 2;
							}
							iPlotValue += iStrategicValue;


							if (pLoopPlot->IsNaturalWonder() && iDistance > 0 && iDistance <= NUM_CITY_RINGS)
							{

								iPlotValue += iPlotValue * 2 + 500;
							}


							if (iPlotValue > 0 && pLoopPlot->getOwner() == pPlayer->GetID())
							{
								iPlotValue /= 4;
							}


							rtnValue += iPlotValue;

							FeatureTypes ePlotFeature = pLoopPlot->getFeatureType();
							ImprovementTypes ePlotImprovement = pLoopPlot->getImprovementType();
							ResourceTypes ePlotResource = pLoopPlot->getResourceType();

							if (ePlotFeature == FEATURE_FOREST)
							{
								if (iDistance <= 5)
								{
									++iIroquoisForestCount;
									if (iDistance == 1)
									{
										if (ePlotImprovement == NO_IMPROVEMENT)
										{
											++iCelticForestCount;
										}
									}
								}
							}
							else if (ePlotFeature == FEATURE_JUNGLE)
							{
								if (iDistance <= NUM_CITY_RINGS)
								{
									++iBrazilJungleCount;
								}
							}
							else if (ePlotFeature == FEATURE_MARSH || ePlotFeature == FEATURE_FLOOD_PLAINS)
							{
								if (iDistance <= NUM_CITY_RINGS)
								{
									++iWetlandsCount;
								}
							}

							if (pLoopPlot->IsNaturalWonder())
							{
								if (iDistance <= 1)
								{
									++iNaturalWonderCount;
								}
							}

							if (pLoopPlot->getTerrainType() == TERRAIN_DESERT)
							{
								if (iDistance <= NUM_CITY_RINGS)
								{
									if (ePlotResource == NO_RESOURCE)
									{
										++iDesertCount;
									}
								}
							}

							if (bIsInca)
							{
								if (pLoopPlot->isHills())
								{
									if (iDistance <= NUM_CITY_RINGS)
									{
										iAdjacentMountains = pLoopPlot->GetNumAdjacentMountains();
										if (iAdjacentMountains > 0 && iAdjacentMountains < 6)
										{

											rtnValue += m_iIncaMultiplier + (iAdjacentMountains * m_iIncaMultiplier);
										}
									}
									
								}
							}
						}
					}
					else
					{

						if (iClosestEnemyCity > iDistance)
						{
							if (pLoopPlot->isCity() && (pLoopPlot->getOwner() < MAX_MAJOR_CIVS))
							{
								iClosestEnemyCity = iDistance;
							}
						}
					}
				}
			}
		}
	}

	if (pPlayer->GetPlayerTraits()->IsFaithFromUnimprovedForest())
	{
		if (iCelticForestCount >= 3)
		{
			rtnValue += 2 * 1000 * m_iFlavorMultiplier[YIELD_FAITH];
		}
		else if (iCelticForestCount >= 1)
		{
			rtnValue += 1 * 1000 * m_iFlavorMultiplier[YIELD_FAITH];
		}
	}
	else if (pPlayer->GetPlayerTraits()->IsMoveFriendlyWoodsAsRoad())
	{
		rtnValue += iIroquoisForestCount * 10;	
	}
	else if (pPlayer->GetPlayerTraits()->GetNaturalWonderYieldModifier() > 0)
	{
		rtnValue += iNaturalWonderCount * m_iSpainMultiplier;	
	}


	ImprovementTypes eBrazilImprovement = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_BRAZILWOOD_CAMP", true);  
	if(eBrazilImprovement != NO_IMPROVEMENT)
	{
		CvImprovementEntry* pkEntry = GC.getImprovementInfo(eBrazilImprovement);
		if(pkEntry != NULL && pkEntry->IsSpecificCivRequired())
		{
			CivilizationTypes eCiv = pkEntry->GetRequiredCivilization();
			if(eCiv == pPlayer->getCivilizationType())
			{
				rtnValue += iBrazilJungleCount * m_iBrazilMultiplier;
			}
		}
	}


	ImprovementTypes eMoroccoImprovement = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_KASBAH", true);  
	if(eMoroccoImprovement != NO_IMPROVEMENT)
	{
		CvImprovementEntry* pkEntry = GC.getImprovementInfo(eMoroccoImprovement);
		if(pkEntry != NULL && pkEntry->IsSpecificCivRequired())
		{
			CivilizationTypes eCiv = pkEntry->GetRequiredCivilization();
			if(eCiv == pPlayer->getCivilizationType())
			{
				rtnValue += iDesertCount * m_iMorrocoMultiplier;
			}
		}
	}


	ImprovementTypes ePolderImprovement = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_POLDER", true);  
	if(ePolderImprovement != NO_IMPROVEMENT)
	{
		CvImprovementEntry* pkEntry = GC.getImprovementInfo(ePolderImprovement);
		if(pkEntry != NULL && pkEntry->IsSpecificCivRequired())
		{
			CivilizationTypes eCiv = pkEntry->GetRequiredCivilization();
			if(eCiv == pPlayer->getCivilizationType())
			{
				rtnValue += iWetlandsCount * m_iNetherlandsMultiplier;
			}
		}
	}

	if (rtnValue < 0) rtnValue = 0;


	if (pPlot->getResourceType(pPlayer->getTeam()) != NO_RESOURCE)
	{
		rtnValue += (int)rtnValue *         GC.getBUILD_ON_RESOURCE_PERCENT() / 100;
	}

	if (pPlot->isRiver())
	{
		rtnValue += (int)rtnValue *        GC.getBUILD_ON_RIVER_PERCENT() / 100;
	}

	if (pPlot->isCoastalLand(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
	{



		rtnValue += (int)rtnValue *        GC.getSETTLER_BUILD_ON_COAST_PERCENT() / 100;
		int iNavalFlavor = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)m_iNavalIndex);
		if (iNavalFlavor > 7)
		{
			rtnValue += (int)rtnValue *        GC.getSETTLER_BUILD_ON_COAST_PERCENT() / 100;
		}
		if (pPlayer->getCivilizationInfo().isCoastalCiv())
		{
			rtnValue += rtnValue > 0 ? 25 : 0;
			rtnValue *= 2;
		}
	}




#ifdef AUI_WARNING_FIXES
	if (pPlayer->isHuman())
#else
	if (pPlayer != NULL && pPlayer->isHuman())
#endif
	{
		if (iClosestCityOfMine == 3)
		{
			rtnValue /= 2;
		}
	}

	else
	{
		int iGrowthFlavor = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)m_iGrowthIndex);
		int iExpansionFlavor = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)m_iExpansionIndex);

		int iSweetSpot = 5;
		iSweetSpot += (iGrowthFlavor > 7) ?  1 : 0;
		iSweetSpot += (iExpansionFlavor > 7) ?  -1 : 0;
		iSweetSpot += (iGrowthFlavor < 4) ?  -1 : 0;
		iSweetSpot += (iExpansionFlavor < 4) ?  1 : 0;
		iSweetSpot = max(4,iSweetSpot);
		iSweetSpot = min(6,iSweetSpot);

		if (iClosestCityOfMine == iSweetSpot) 
		{

			rtnValue *= 175;
			rtnValue /= 100;
		}
		else if (iClosestCityOfMine < iSweetSpot)
		{
			rtnValue /= 2;
		}
		else if (iClosestCityOfMine > 7)
		{
			rtnValue *= 2;
			rtnValue /= 3;
		}


		int iBoldness = pPlayer->GetDiplomacyAI()->GetBoldness();
		if (iBoldness < 4)
		{
			if (iClosestEnemyCity <= 4)
			{
				rtnValue /= 4;
			}
			else if (iClosestEnemyCity == 5)
			{
				rtnValue /= 2;
			}
		}
		else if (iBoldness > 7)
		{
			if (iClosestEnemyCity <= 5 && iClosestCityOfMine < 8)
			{
				rtnValue *= 3;
				rtnValue /= 2;
			}
		}
		else
		{
			if (iClosestEnemyCity < 5)
			{
				rtnValue *= 2;
				rtnValue /= 3;
			}
		}


		if (iCapitalArea != pPlot->getArea())
		{
			if (iClosestCityOfMine < 7)
			{
				rtnValue *= 3;
				rtnValue /= 2;
			}
		}
	}

	rtnValue = (rtnValue > 0) ? rtnValue : 0;

	return rtnValue;
}


int CvCitySiteEvaluator::PlotFertilityValue(CvPlot* pPlot)
{
	int rtnValue = 0;

	if(!pPlot->isWater() && !pPlot->isImpassable() && !pPlot->isMountain())
	{
		rtnValue += ComputeFoodValue(pPlot, NULL);
		rtnValue += ComputeProductionValue(pPlot, NULL);
		rtnValue += ComputeGoldValue(pPlot, NULL);
		rtnValue += ComputeScienceValue(pPlot, NULL);
		rtnValue += ComputeTradeableResourceValue(pPlot, NULL);
	}

	if(rtnValue < 0) rtnValue = 0;

	return rtnValue;
}


int CvCitySiteEvaluator::BestFoundValueForSpecificYield(CvPlayer* pPlayer, YieldTypes eYield)
{
	pPlayer;
	eYield;
	return 0;
}




int CvCitySiteEvaluator::ComputeFoodValue(CvPlot* pPlot, CvPlayer* pPlayer)
{
	int rtnValue = 0;


	if(pPlayer == NULL)
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_FOOD, NO_TEAM);
	}
	else
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_FOOD, pPlayer->getTeam());
	}


	TeamTypes eTeam = NO_TEAM;
	if(pPlayer != NULL)
	{
		eTeam = pPlayer->getTeam();
	}

	ResourceTypes eResource;
	eResource = pPlot->getResourceType(eTeam);
	if(eResource != NO_RESOURCE)
	{
		rtnValue += GC.getResourceInfo(eResource)->getYieldChange(YIELD_FOOD);

		CvImprovementEntry* pImprovement = GC.GetGameImprovements()->GetImprovementForResource(eResource);
		if(pImprovement)
		{
			rtnValue += pImprovement->GetImprovementResourceYield(eResource, YIELD_FOOD);
		}
	}

	return rtnValue * m_iFlavorMultiplier[YIELD_FOOD];
}


int CvCitySiteEvaluator::ComputeHappinessValue(CvPlot* pPlot, CvPlayer* pPlayer)
{
	int rtnValue = 0;


	TeamTypes eTeam = NO_TEAM;
	if(pPlayer != NULL)
	{
		eTeam = pPlayer->getTeam();
	}

	ResourceTypes eResource;
	eResource = pPlot->getResourceType(eTeam);
	if(eResource != NO_RESOURCE)
	{

		if(!pPlot->isOwned())
		{
			rtnValue += GC.getResourceInfo(eResource)->getHappiness();
		}


		if(pPlayer)
		{
			if(pPlayer->getNumResourceTotal(eResource) == 0)
				rtnValue *= 5;
		}
	}

	return rtnValue * m_iFlavorMultiplier[SITE_EVALUATION_HAPPINESS];
}


int CvCitySiteEvaluator::ComputeProductionValue(CvPlot* pPlot, CvPlayer* pPlayer)
{
	int rtnValue = 0;


	if(pPlayer == NULL)
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_PRODUCTION, NO_TEAM);
	}
	else
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_PRODUCTION, pPlayer->getTeam());
	}


	TeamTypes eTeam = NO_TEAM;
	if(pPlayer != NULL)
	{
		eTeam = pPlayer->getTeam();
	}

	ResourceTypes eResource;
	eResource = pPlot->getResourceType(eTeam);
	if(eResource != NO_RESOURCE)
	{
		rtnValue += GC.getResourceInfo(eResource)->getYieldChange(YIELD_PRODUCTION);

		CvImprovementEntry* pImprovement = GC.GetGameImprovements()->GetImprovementForResource(eResource);
		if(pImprovement)
		{
			rtnValue += pImprovement->GetImprovementResourceYield(eResource, YIELD_PRODUCTION);
		}
	}

	return rtnValue * m_iFlavorMultiplier[YIELD_PRODUCTION];
}


int CvCitySiteEvaluator::ComputeGoldValue(CvPlot* pPlot, CvPlayer* pPlayer)
{
	int rtnValue = 0;


	if(pPlayer == NULL)
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_GOLD, NO_TEAM);
	}
	else
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_GOLD, pPlayer->getTeam());
	}


	TeamTypes eTeam = NO_TEAM;
	if(pPlayer != NULL)
	{
		eTeam = pPlayer->getTeam();
	}

	ResourceTypes eResource;
	eResource = pPlot->getResourceType(eTeam);
	if(eResource != NO_RESOURCE)
	{
		rtnValue += GC.getResourceInfo(eResource)->getYieldChange(YIELD_GOLD);

		CvImprovementEntry* pImprovement = GC.GetGameImprovements()->GetImprovementForResource(eResource);
		if(pImprovement)
		{
			rtnValue += pImprovement->GetImprovementResourceYield(eResource, YIELD_GOLD);
		}
	}

	return rtnValue * m_iFlavorMultiplier[YIELD_GOLD];
}


int CvCitySiteEvaluator::ComputeScienceValue(CvPlot* pPlot, CvPlayer* pPlayer)
{
	int rtnValue = 0;

	CvAssert(pPlot);
	if(!pPlot) return rtnValue;


	if(pPlayer == NULL)
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_SCIENCE, NO_TEAM);
	}
	else
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_SCIENCE, pPlayer->getTeam());
	}


	TeamTypes eTeam = NO_TEAM;
	if(pPlayer != NULL)
	{
		eTeam = pPlayer->getTeam();
	}

	ResourceTypes eResource;
	eResource = pPlot->getResourceType(eTeam);
	if(eResource != NO_RESOURCE)
	{
		rtnValue += GC.getResourceInfo(eResource)->getYieldChange(YIELD_SCIENCE);

		CvImprovementEntry* pImprovement = GC.GetGameImprovements()->GetImprovementForResource(eResource);
		if(pImprovement)
		{
			rtnValue += pImprovement->GetImprovementResourceYield(eResource, YIELD_SCIENCE);
		}
	}

	return rtnValue * m_iFlavorMultiplier[YIELD_SCIENCE];
}


int CvCitySiteEvaluator::ComputeFaithValue(CvPlot* pPlot, CvPlayer* pPlayer)
{
	int rtnValue = 0;

	CvAssert(pPlot);
	if(!pPlot) return rtnValue;


	if(pPlayer == NULL)
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_FAITH, NO_TEAM);
	}
	else
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_FAITH, pPlayer->getTeam());
	}


	TeamTypes eTeam = NO_TEAM;
	if(pPlayer != NULL)
	{
		eTeam = pPlayer->getTeam();
	}

	ResourceTypes eResource;
	eResource = pPlot->getResourceType(eTeam);
	if(eResource != NO_RESOURCE)
	{
		rtnValue += GC.getResourceInfo(eResource)->getYieldChange(YIELD_FAITH);

		CvImprovementEntry* pImprovement = GC.GetGameImprovements()->GetImprovementForResource(eResource);
		if(pImprovement)
		{
			rtnValue += pImprovement->GetImprovementResourceYield(eResource, YIELD_FAITH);
		}
	}

	return rtnValue * m_iFlavorMultiplier[YIELD_FAITH];
}



int CvCitySiteEvaluator::ComputeTradeableResourceValue(CvPlot* pPlot, CvPlayer* pPlayer)
{
	int rtnValue = 0;

	CvAssert(pPlot);
	if(!pPlot) return rtnValue;


	if(pPlot->isOwned())
	{
		return rtnValue;
	}

	TeamTypes eTeam = NO_TEAM;
	if(pPlayer != NULL)
	{
		eTeam = pPlayer->getTeam();
	}

	ResourceTypes eResource;
	eResource = pPlot->getResourceType(eTeam);

	if(eResource != NO_RESOURCE)
	{
		ResourceUsageTypes eResourceUsage = GC.getResourceInfo(eResource)->getResourceUsage();


		if(eResourceUsage == RESOURCEUSAGE_LUXURY || eResourceUsage == RESOURCEUSAGE_STRATEGIC)
		{
			rtnValue += pPlot->getNumResource() * m_iFlavorMultiplier[SITE_EVALUATION_RESOURCES];

			if(pPlayer)
			{

				if(pPlayer->getNumResourceTotal(eResource) == 0)
					rtnValue *= 3;
			}
		}
	}

	return rtnValue;
}


int CvCitySiteEvaluator::ComputeStrategicValue(CvPlot* pPlot, CvPlayer* pPlayer, int iPlotsFromCity)
{
	int rtnValue = 0;

	CvAssert(pPlot);
	if(!pPlot) return rtnValue;


	if(iPlotsFromCity == 2 && (pPlot->isImpassable() || pPlot->isMountain()))
	{
		rtnValue +=       GC.getCHOKEPOINT_STRATEGIC_VALUE();
	}


	if(iPlotsFromCity == 1 && pPlot->isHills())
	{
		rtnValue +=       GC.getHILL_STRATEGIC_VALUE();
	}


	if(pPlot->getFeatureType() != NO_FEATURE)
	{
		int iWeight = GC.getFeatureInfo(pPlot->getFeatureType())->getStartingLocationWeight();
		if(iWeight != 0 && iPlotsFromCity == 1)
		{
			rtnValue += iWeight;
		}
	}


	if(pPlayer != NULL && pPlot->isCity())
	{

		{
			rtnValue +=           GC.getALREADY_OWNED_STRATEGIC_VALUE();
		}
	}



	rtnValue *= m_iFlavorMultiplier[SITE_EVALUATION_STRATEGIC];

	return rtnValue;
}





CvSiteEvaluatorForSettler::CvSiteEvaluatorForSettler(void)
{
}


CvSiteEvaluatorForSettler::~CvSiteEvaluatorForSettler(void)
{
}


int CvSiteEvaluatorForSettler::PlotFoundValue(CvPlot* pPlot, CvPlayer* pPlayer, YieldTypes eYield, bool bCoastOnly)
{
	CvAssert(pPlot);
	if(!pPlot) return 0;

	if(!CanFound(pPlot, pPlayer, true))
	{
		return 0;
	}




	bool bIsCoastal = pPlot->isCoastalLand(GC.getMIN_WATER_SIZE_FOR_OCEAN());
	CvArea* pArea = pPlot->area();
	CvAssert(pArea);
	if(!pArea) return 0;
	int iNumAreaCities = pArea->getCitiesPerPlayer(pPlayer->GetID());
	if(bCoastOnly && !bIsCoastal && iNumAreaCities == 0)
	{
		return 0;
	}


	else
	{


		int iLuxuryModifier = 0;

		if (pPlayer->GetPlayerTraits()->WillGetUniqueLuxury(pArea) && bIsCoastal)
		{
			iLuxuryModifier = CvCitySiteEvaluator::PlotFoundValue(pPlot, pPlayer, eYield) * 2;
			return iLuxuryModifier;
		}
		else
		{
			return CvCitySiteEvaluator::PlotFoundValue(pPlot, pPlayer, eYield);
		}
	}
}





CvSiteEvaluatorForStart::CvSiteEvaluatorForStart(void)
{
}


CvSiteEvaluatorForStart::~CvSiteEvaluatorForStart(void)
{
}


void CvSiteEvaluatorForStart::ComputeFlavorMultipliers(CvPlayer*)
{

	for(int iI = 0; iI < NUM_SITE_EVALUATION_FACTORS; iI++)
	{
		m_iFlavorMultiplier[iI] = 1;
	}
}


int CvSiteEvaluatorForStart::PlotFoundValue(CvPlot* pPlot, CvPlayer* pPlayer, YieldTypes, bool)
{
	int rtnValue = 0;
	int iI;
	CvPlot* pLoopPlot(NULL);
	int iCelticForestCount = 0;

	CvAssert(pPlot);
	if(!pPlot) return rtnValue;

	if(!CanFound(pPlot, pPlayer, false))
	{
		return rtnValue;
	}




	if(pPlot->isGoody())
	{
		return 0;
	}


	for(iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		pLoopPlot = plotCity(pPlot->getX(), pPlot->getY(), iI);


		if(pLoopPlot == NULL)
		{
			return 0;
		}
		else
		{
			int iDistance = plotDistance(pPlot->getX(), pPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY());
			CvAssert(iDistance <= NUM_CITY_RINGS);
			if(iDistance > NUM_CITY_RINGS) continue;
			int iRingModifier = m_iRingModifier[iDistance];


			if(iDistance != 0)
			{
				rtnValue += iRingModifier * ComputeFoodValue(pLoopPlot, pPlayer) *       GC.getSTART_AREA_FOOD_MULTIPLIER();
				rtnValue += iRingModifier * ComputeHappinessValue(pLoopPlot, pPlayer) *        GC.getSTART_AREA_HAPPINESS_MULTIPLIER();
				rtnValue += iRingModifier * ComputeProductionValue(pLoopPlot, pPlayer) *       GC.getSTART_AREA_PRODUCTION_MULTIPLIER();
				rtnValue += iRingModifier * ComputeGoldValue(pLoopPlot, pPlayer) *       GC.getSTART_AREA_GOLD_MULTIPLIER();
				rtnValue += iRingModifier * ComputeScienceValue(pLoopPlot, pPlayer) *       GC.getSTART_AREA_SCIENCE_MULTIPLIER();
				rtnValue += iRingModifier * ComputeFaithValue(pLoopPlot, pPlayer) *       GC.getSTART_AREA_FAITH_MULTIPLIER();
				rtnValue += iRingModifier * ComputeTradeableResourceValue(pLoopPlot, pPlayer) *       GC.getSTART_AREA_RESOURCE_MULTIPLIER();
				rtnValue += iRingModifier * ComputeStrategicValue(pLoopPlot, pPlayer, iDistance) *       GC.getSTART_AREA_STRATEGIC_MULTIPLIER();
			}

			if (pPlayer)
			{
				if (iDistance == 1 && pLoopPlot->getFeatureType() == FEATURE_FOREST)
				{
					if (pLoopPlot->getImprovementType() == NO_IMPROVEMENT && pPlayer->GetPlayerTraits()->IsFaithFromUnimprovedForest())
					{
						iCelticForestCount += 1;
					}
				}
			}
		}
	}

	if (iCelticForestCount >= 3)
	{
		rtnValue += 2 * 1000 * m_iFlavorMultiplier[YIELD_FAITH];
	}
	else if (iCelticForestCount >= 1)
	{
		rtnValue += 1 * 1000 * m_iFlavorMultiplier[YIELD_FAITH];
	}

	if(rtnValue < 0) rtnValue = 0;


	if(pPlot->getResourceType() != NO_RESOURCE)
	{
		rtnValue += rtnValue * GC.getBUILD_ON_RESOURCE_PERCENT() / 100;
	}

	if(pPlot->isRiver())
	{
		rtnValue += rtnValue * GC.getBUILD_ON_RIVER_PERCENT() / 100;
	}

	if(pPlot->isCoastalLand())
	{
		rtnValue += rtnValue * GC.getSTART_AREA_BUILD_ON_COAST_PERCENT() / 100;
	}

	return rtnValue;
}
