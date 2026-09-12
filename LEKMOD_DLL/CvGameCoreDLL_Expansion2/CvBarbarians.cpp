







#include "CvGameCoreDLLPCH.h"
#include "CvBarbarians.h"
#include "CvGameCoreUtils.h"
#include "CvTypes.h"


#ifdef AUI_WARNING_FIXES
int* CvBarbarians::m_aiPlotBarbCampSpawnCounter = NULL;
int* CvBarbarians::m_aiPlotBarbCampNumUnitsSpawned = NULL;
#else
short* CvBarbarians::m_aiPlotBarbCampSpawnCounter = NULL;
short* CvBarbarians::m_aiPlotBarbCampNumUnitsSpawned = NULL;
#endif
FStaticVector<DirectionTypes, 6, true, c_eCiv5GameplayDLL, 0> CvBarbarians::m_aeValidBarbSpawnDirections;


bool CvBarbarians::IsPlotValidForBarbCamp(CvPlot* pPlot)
{
	int iRange = 4;
	int iDY;

#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX, iDX;
	CvPlot* pLoopPlot;
	for (iDY = -iRange; iDY <= iRange; iDY++)
	{
		iMaxDX = iRange - MAX(0, iDY);
		for (iDX = -iRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
		{

			pLoopPlot = plotXY(pPlot->getX(), pPlot->getY(), iDX, iDY);
#else
	int iPlotX = pPlot->getX();
	int iPlotY = pPlot->getY();

	CvMap& kMap = GC.getMap();
	for (int iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for (iDY = -(iRange); iDY <= iRange; iDY++)
		{
			int iLoopPlotX = iPlotX + iDX;
			int iLoopPlotY = iPlotY + iDY;


			if (plotDistance(iPlotX, iPlotY, iLoopPlotX, iLoopPlotY) > iRange)
				continue;


			CvPlot* pLoopPlot = kMap.plot(iLoopPlotX, iLoopPlotY);
#endif
			if (pLoopPlot)
			{
				if (m_aiPlotBarbCampSpawnCounter[pLoopPlot->GetPlotIndex()] < -1)
					return false;
			}
		}
	}

	return true;
}



void CvBarbarians::DoBarbCampCleared(CvPlot* pPlot, PlayerTypes ePlayer)
{
	m_aiPlotBarbCampSpawnCounter[pPlot->GetPlotIndex()] = -16;

	pPlot->AddArchaeologicalRecord(CvTypes::getARTIFACT_BARBARIAN_CAMP(), ePlayer, NO_PLAYER);
}



bool CvBarbarians::CanBarbariansSpawn()
{
	CvGame& kGame = GC.getGame();
	if (kGame.getElapsedGameTurns() < 10)
	{
		return false;
	}

	return true;
}



bool CvBarbarians::ShouldSpawnBarbFromCamp(CvPlot* pPlot)
{
	if (m_aiPlotBarbCampSpawnCounter[pPlot->GetPlotIndex()] == 0)
	{
		return true;
	}

	return false;
}



void CvBarbarians::DoCampActivationNotice(CvPlot* pPlot)
{
	CvGame& kGame = GC.getGame();

	int iNumTurnsToSpawn = 8 + kGame.getJonRandNum(5, "Barb Spawn Rand call");


	if (kGame.isOption(GAMEOPTION_RAGING_BARBARIANS))
		iNumTurnsToSpawn /= 2;


	int iNumUnitsSpawned = m_aiPlotBarbCampNumUnitsSpawned[pPlot->GetPlotIndex()];


	iNumTurnsToSpawn -= min(3, iNumUnitsSpawned);


	m_aiPlotBarbCampNumUnitsSpawned[pPlot->GetPlotIndex()]++;









	CvHandicapInfo* pHandicapInfo = GC.getHandicapInfo(kGame.getHandicapType());
	if (pHandicapInfo)
		iNumTurnsToSpawn += pHandicapInfo->getBarbSpawnMod();


	CvGameSpeedInfo* pGameSpeedInfo = GC.getGameSpeedInfo(kGame.getGameSpeedType());
	if (pGameSpeedInfo)
	{
		iNumTurnsToSpawn *= pGameSpeedInfo->getBarbPercent();
		iNumTurnsToSpawn /= 100;
	}

	m_aiPlotBarbCampSpawnCounter[pPlot->GetPlotIndex()] = iNumTurnsToSpawn;
}



void CvBarbarians::DoCampAttacked(CvPlot* pPlot)
{
	int iCounter = m_aiPlotBarbCampSpawnCounter[pPlot->GetPlotIndex()];


	int iNewValue = iCounter / 2;

	m_aiPlotBarbCampSpawnCounter[pPlot->GetPlotIndex()] = iNewValue;
}



void CvBarbarians::BeginTurn()
{
	CvGame &kGame = GC.getGame();
	const ImprovementTypes eCamp = kGame.GetBarbarianCampImprovementType();

	CvMap& kMap = GC.getMap();
	int iWorldNumPlots = kMap.numPlots();
	for (int iPlotLoop = 0; iPlotLoop < iWorldNumPlots; iPlotLoop++)
	{
		if (m_aiPlotBarbCampSpawnCounter[iPlotLoop] > 0)
		{

			CvPlot* pPlot = kMap.plotByIndex(iPlotLoop);

			if (pPlot->getImprovementType() != eCamp)
			{
				m_aiPlotBarbCampSpawnCounter[iPlotLoop] = -1;
				m_aiPlotBarbCampNumUnitsSpawned[iPlotLoop] = -1;
			}
			else
			{
				m_aiPlotBarbCampSpawnCounter[iPlotLoop]--;
			}
		}


		else if (m_aiPlotBarbCampSpawnCounter[iPlotLoop] < -1)
		{
			m_aiPlotBarbCampSpawnCounter[iPlotLoop]++;
		}
	}
}


void CvBarbarians::MapInit(int iWorldNumPlots)
{
	if (m_aiPlotBarbCampSpawnCounter != NULL)
	{
		SAFE_DELETE_ARRAY(m_aiPlotBarbCampSpawnCounter);
	}
	if (m_aiPlotBarbCampNumUnitsSpawned != NULL)
	{
		SAFE_DELETE_ARRAY(m_aiPlotBarbCampNumUnitsSpawned);
	}
	
	int iI;

	if (iWorldNumPlots > 0)
	{
		if (m_aiPlotBarbCampSpawnCounter == NULL)
		{
#ifdef AUI_WARNING_FIXES
			m_aiPlotBarbCampSpawnCounter = FNEW(int[iWorldNumPlots], c_eCiv5GameplayDLL, 0);
#else
			m_aiPlotBarbCampSpawnCounter = FNEW(short[iWorldNumPlots], c_eCiv5GameplayDLL, 0);
#endif
		}
		if (m_aiPlotBarbCampNumUnitsSpawned == NULL)
		{
#ifdef AUI_WARNING_FIXES
			m_aiPlotBarbCampNumUnitsSpawned = FNEW(int[iWorldNumPlots], c_eCiv5GameplayDLL, 0);
#else
			m_aiPlotBarbCampNumUnitsSpawned = FNEW(short[iWorldNumPlots], c_eCiv5GameplayDLL, 0);
#endif
		}


		for (iI = 0; iI < iWorldNumPlots; ++iI)
		{
			m_aiPlotBarbCampSpawnCounter[iI] = -1;
			m_aiPlotBarbCampNumUnitsSpawned[iI] = -1;
		}
	}
}



void CvBarbarians::Uninit()
{
	if (m_aiPlotBarbCampSpawnCounter != NULL)
	{
		SAFE_DELETE_ARRAY(m_aiPlotBarbCampSpawnCounter);
	}

	if (m_aiPlotBarbCampNumUnitsSpawned != NULL)
	{
		SAFE_DELETE_ARRAY(m_aiPlotBarbCampNumUnitsSpawned);
	}
}



#ifdef AUI_WARNING_FIXES
void CvBarbarians::Read(FDataStream& kStream, uint                    )
#else
void CvBarbarians::Read(FDataStream& kStream, uint uiParentVersion)
#endif
{

	uint uiVersion = 0;

	kStream >> uiVersion;	

	int iWorldNumPlots = GC.getMap().numPlots();
	MapInit(iWorldNumPlots);

#ifdef AUI_WARNING_FIXES
	ArrayWrapper<int> kWrapper1(iWorldNumPlots, m_aiPlotBarbCampSpawnCounter);
	kStream >> kWrapper1;
	ArrayWrapper<int> kWrapper2(iWorldNumPlots, m_aiPlotBarbCampNumUnitsSpawned);
	kStream >> kWrapper2;
#else
	kStream >> ArrayWrapper<short>(iWorldNumPlots, m_aiPlotBarbCampSpawnCounter);
	kStream >> ArrayWrapper<short>(iWorldNumPlots, m_aiPlotBarbCampNumUnitsSpawned);
#endif
}



void CvBarbarians::Write(FDataStream& kStream)
{

	uint uiVersion = 1;
	kStream << uiVersion;

	int iWorldNumPlots = GC.getMap().numPlots();
#ifdef AUI_WARNING_FIXES
	ArrayWrapper<int> kWrapper1(iWorldNumPlots, m_aiPlotBarbCampSpawnCounter);
	kStream << kWrapper1;
	ArrayWrapper<int> kWrapper2(iWorldNumPlots, m_aiPlotBarbCampNumUnitsSpawned);
	kStream << kWrapper2;
#else
	kStream << ArrayWrapper<short>(iWorldNumPlots, m_aiPlotBarbCampSpawnCounter);
	kStream << ArrayWrapper<short>(iWorldNumPlots, m_aiPlotBarbCampNumUnitsSpawned);
#endif
}


void CvBarbarians::DoCamps()
{
	CvGame& kGame = GC.getGame();

	if(kGame.isOption(GAMEOPTION_NO_BARBARIANS))
	{
		return;
	}

	int iNumNotVisiblePlots = 0;
	int iNumCampsInExistence = 0;
	CvPlot* pLoopPlot;

	ImprovementTypes eCamp = (ImprovementTypes)GC.getBARBARIAN_CAMP_IMPROVEMENT();

	bool bAlwaysRevealedBarbCamp = false;


	if(eCamp != NO_IMPROVEMENT)
	{
		CvMap& kMap = GC.getMap();

#ifdef AUI_WARNING_FIXES
		for (uint iI = 0; iI < kMap.numPlots(); iI++)
#else
		for(int iI = 0; iI < kMap.numPlots(); iI++)
#endif
		{
			pLoopPlot = kMap.plotByIndexUnchecked(iI);


			if(pLoopPlot->getImprovementType() == eCamp)
			{
				iNumCampsInExistence++;
			}

			if(!pLoopPlot->isWater())
			{
				if(!pLoopPlot->isVisibleToCivTeam())
				{
					iNumNotVisiblePlots++;
				}
			}
		}

		int iNumValidCampPlots;

		iNumValidCampPlots = iNumNotVisiblePlots;

		int iFogTilesPerBarbarianCamp = kMap.getWorldInfo().getFogTilesPerBarbarianCamp();
		int iCampTargetNum = (iFogTilesPerBarbarianCamp != 0)? iNumValidCampPlots / iFogTilesPerBarbarianCamp : 0;
		int iNumCampsToAdd = iCampTargetNum - iNumCampsInExistence;

		int iMaxCampsThisArea;

		if(iNumCampsToAdd > 0 && GC.getBARBARIAN_CAMP_ODDS_OF_NEW_CAMP_SPAWNING() > 0)
		{

			if(kGame.getElapsedGameTurns() == 0)
			{
				iNumCampsToAdd *=        GC.getBARBARIAN_CAMP_FIRST_TURN_PERCENT_OF_TARGET_TO_ADD();
				iNumCampsToAdd /= 100;
			}

			else
			{
				if(kGame.getJonRandNum(      GC.getBARBARIAN_CAMP_ODDS_OF_NEW_CAMP_SPAWNING(), "Random roll to see if Barb Camp spawns this turn") > 0)
				{
					iNumCampsToAdd = 1;
				}
				else
				{
					iNumCampsToAdd = 0;
				}
			}


			int iCount = 0;
			int iPlotIndex = -1;
			int iNumPlots = kMap.numPlots();

			UnitTypes eBestUnit;

			int iNumLandPlots = kMap.getLandPlots();


			bool bWantsCoastal = kGame.getJonRandNum(      GC.getBARBARIAN_CAMP_COASTAL_SPAWN_ROLL(), "Barb Camp Plot-Finding Roll - Coastal Bias 1") == 0 ? true : false;

			int iPlayerCapitalMinDistance =       GC.getBARBARIAN_CAMP_MINIMUM_DISTANCE_CAPITAL();
			int iBarbCampMinDistance =       GC.getBARBARIAN_CAMP_MINIMUM_DISTANCE_ANOTHER_CAMP();
			int iMaxDistanceToLook = iPlayerCapitalMinDistance > iBarbCampMinDistance ? iPlayerCapitalMinDistance : iBarbCampMinDistance;
			int iPlotDistance;

			int iDX, iDY;
			CvPlot* pNearbyCampPlot;
			bool bSomethingTooClose;

			CvString strBuffer;

			int iPlayerLoop;


			do
			{
				iCount++;

				iPlotIndex = kGame.getJonRandNum(iNumPlots, "Barb Camp Plot-Finding Roll");

				pLoopPlot = kMap.plotByIndex(iPlotIndex);


				if(!pLoopPlot->isWater())
				{
					if(!pLoopPlot->isImpassable() && !pLoopPlot->isMountain())
					{
						if(!pLoopPlot->isOwned() && !pLoopPlot->isVisibleToCivTeam())
						{

							if(pLoopPlot->getResourceType() == NO_RESOURCE)
							{

								if(kMap.getArea(pLoopPlot->getArea())->getNumTiles() > 1)
								{
									if(pLoopPlot->isCoastalLand() || !bWantsCoastal)
									{

										iMaxCampsThisArea = iCampTargetNum * pLoopPlot->area()->getNumTiles() / iNumLandPlots;

										iMaxCampsThisArea++;


										if(pLoopPlot->area()->getNumImprovements(eCamp) <= iMaxCampsThisArea)
										{

											if(pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
											{

												if(pLoopPlot->getFeatureType() == NO_FEATURE || !GC.getFeatureInfo(pLoopPlot->getFeatureType())->isNoImprovement())
												{
													bSomethingTooClose = false;

#ifdef AUI_HEXSPACE_DX_LOOPS
													int iMaxDX;
													for (iDY = -(iMaxDistanceToLook); iDY <= iMaxDistanceToLook; iDY++)
													{
														iMaxDX = iMaxDistanceToLook - MAX(0, iDY);
														for (iDX = -(iMaxDistanceToLook) - MIN(0, iDY); iDX <= iMaxDX; iDX++)
#else

													for(iDX = -(iMaxDistanceToLook); iDX <= iMaxDistanceToLook; iDX++)
													{
														for(iDY = -(iMaxDistanceToLook); iDY <= iMaxDistanceToLook; iDY++)
#endif
														{
															pNearbyCampPlot = plotXY(pLoopPlot->getX(), pLoopPlot->getY(), iDX, iDY);

															if(pNearbyCampPlot != NULL)
															{
#ifdef AUI_FIX_HEX_DISTANCE_INSTEAD_OF_PLOT_DISTANCE
																iPlotDistance = hexDistance(iDX, iDY);
#else
																iPlotDistance = plotDistance(pNearbyCampPlot->getX(), pNearbyCampPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY());
#endif


																if(iPlotDistance <= iPlayerCapitalMinDistance)
																{
																	if(pNearbyCampPlot->isCity())
																	{
																		if(pNearbyCampPlot->getPlotCity()->isCapital())
																		{

																			if(pNearbyCampPlot->getPlotCity()->getOwner() < MAX_MAJOR_CIVS)
																			{
																				bSomethingTooClose = true;
																				break;
																			}
																		}
																	}
																}


																if(iPlotDistance <= iBarbCampMinDistance)
																{
																	if(pNearbyCampPlot->getImprovementType() == eCamp)
																	{
																		bSomethingTooClose = true;
																		break;
																	}
																}
															}
														}
														if(bSomethingTooClose)
														{
															break;
														}
													}


													if(bSomethingTooClose)
														continue;


													if(!CvBarbarians::IsPlotValidForBarbCamp(pLoopPlot))
														continue;

													pLoopPlot->setImprovementType(eCamp);
													DoCampActivationNotice(pLoopPlot);

													eBestUnit = GetRandomBarbarianUnitType(kMap.getArea(pLoopPlot->getArea()), UNITAI_DEFENSE);

													if(eBestUnit != NO_UNIT)
													{
#ifdef AUI_WARNING_FIXES
														CvUnitEntry* pUnitInfo = GC.getUnitInfo(eBestUnit);
														if (pUnitInfo)
															GET_PLAYER(BARBARIAN_PLAYER).initUnit(eBestUnit, pLoopPlot->getX(), pLoopPlot->getY(), (UnitAITypes)pUnitInfo->GetDefaultUnitAIType());
#else
														GET_PLAYER(BARBARIAN_PLAYER).initUnit(eBestUnit, pLoopPlot->getX(), pLoopPlot->getY(), (UnitAITypes) GC.getUnitInfo(eBestUnit)->GetDefaultUnitAIType());
#endif
													}


													PlayerTypes ePlayer;
													TeamTypes eTeam;
													for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
													{
														ePlayer = (PlayerTypes) iPlayerLoop;
														eTeam = GET_PLAYER(ePlayer).getTeam();

														if(GET_PLAYER(ePlayer).IsAlwaysSeeBarbCamps())
														{
															if(pLoopPlot->isRevealed(eTeam))
															{
																pLoopPlot->setRevealedImprovementType(eTeam, eCamp);
																if(GC.getGame().getActivePlayer() == ePlayer)
																	bAlwaysRevealedBarbCamp = true;
															}
														}
													}




													iNumCampsToAdd--;


													bWantsCoastal = kGame.getJonRandNum(      GC.getBARBARIAN_CAMP_COASTAL_SPAWN_ROLL(), "Barb Camp Plot-Finding Roll - Coastal Bias 2") == 0 ? true : false;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			while(iNumCampsToAdd > 0 && iCount < iNumLandPlots);
		}
	}

	if(bAlwaysRevealedBarbCamp)
		GC.getMap().updateDeferredFog();
}


UnitTypes CvBarbarians::GetRandomBarbarianUnitType(CvArea* pArea, UnitAITypes eUnitAI)
{
	UnitTypes eBestUnit = NO_UNIT;
	int iBestValue = 0;
	int iValue = 0;

	CvPlayerAI& kBarbarianPlayer = GET_PLAYER(BARBARIAN_PLAYER);

	CvGame &kGame = GC.getGame();

#ifdef AUI_WARNING_FIXES
	for (uint iUnitClassLoop = 0; iUnitClassLoop < GC.getNumUnitClassInfos(); iUnitClassLoop++)
#else
	for(int iUnitClassLoop = 0; iUnitClassLoop < GC.getNumUnitClassInfos(); iUnitClassLoop++)
#endif
	{
		bool bValid = false;
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo((UnitClassTypes)iUnitClassLoop);
		if(pkUnitClassInfo == NULL)
			continue;

		const UnitTypes eLoopUnit = ((UnitTypes)(kBarbarianPlayer.getCivilizationInfo().getCivilizationUnits(iUnitClassLoop)));
		if(eLoopUnit != NO_UNIT)
		{
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eLoopUnit);
			if(pkUnitInfo == NULL)
			{
				continue;
			}

			CvUnitEntry& kUnit = *pkUnitInfo;

			bValid = (kUnit.GetCombat() > 0);
			if(bValid)
			{

				if(kUnit.GetRange() == 0)
				{
#ifdef AUI_WARNING_FIXES
					for (uint iLoop = 0; iLoop < GC.getNumPromotionInfos(); iLoop++)
#else
					for(int iLoop = 0; iLoop < GC.getNumPromotionInfos(); iLoop++)
#endif
					{
						const PromotionTypes ePromotion = static_cast<PromotionTypes>(iLoop);
						CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
						if(pkPromotionInfo)
						{
							if(kUnit.GetFreePromotions(iLoop))
							{
								if(pkPromotionInfo->IsOnlyDefensive())
								{
									bValid = false;
									break;
								}
							}
						}
					}
				}
			}

			if(bValid)
			{
				if(pArea->isWater() && kUnit.GetDomainType() != DOMAIN_SEA)
				{
					bValid = false;
				}
				else if(!pArea->isWater() && kUnit.GetDomainType() != DOMAIN_LAND)
				{
					bValid = false;
				}
			}

			if(bValid)
			{
				if(!GET_PLAYER(BARBARIAN_PLAYER).canTrain(eLoopUnit))
				{
					bValid = false;
				}
			}

			if(bValid)
			{
				bool bFound = false;
				bool bRequires = false;
				for(int i = 0; i < GC.getNUM_UNIT_AND_TECH_PREREQS(); ++i)
				{
					TechTypes eTech = (TechTypes) kUnit.GetPrereqAndTechs(i);

					if(NO_TECH != eTech)
					{
						bRequires = true;

						if(GET_TEAM(BARBARIAN_TEAM).GetTeamTechs()->HasTech(eTech))
						{
							bFound = true;
							break;
						}
					}
				}

				if(bRequires && !bFound)
				{
					bValid = false;
				}
			}

			if(bValid)
			{
				iValue = (1 + kGame.getJonRandNum(1000, "Barb Unit Selection"));

				if(kUnit.GetUnitAIType(eUnitAI))
				{
					iValue += 200;
				}

				if(iValue > iBestValue)
				{
					eBestUnit = eLoopUnit;
					iBestValue = iValue;
				}
			}
		}
	}

	return eBestUnit;
}


void CvBarbarians::DoUnits()
{
	CvGame& kGame = GC.getGame();

	if(kGame.isOption(GAMEOPTION_NO_BARBARIANS))
	{
		return;
	}

	if(!CanBarbariansSpawn())
	{
		return;
	}

	ImprovementTypes eCamp = (ImprovementTypes)GC.getBARBARIAN_CAMP_IMPROVEMENT();

	CvMap& kMap = GC.getMap();
#ifdef AUI_WARNING_FIXES
	for (uint iPlotLoop = 0; iPlotLoop < kMap.numPlots(); iPlotLoop++)
#else
	for(int iPlotLoop = 0; iPlotLoop < kMap.numPlots(); iPlotLoop++)
#endif
	{
		CvPlot* pLoopPlot = kMap.plotByIndexUnchecked(iPlotLoop);


		if(pLoopPlot->getImprovementType() == eCamp)
		{
			if(ShouldSpawnBarbFromCamp(pLoopPlot))
			{
				DoSpawnBarbarianUnit(pLoopPlot, false, false);
				DoCampActivationNotice(pLoopPlot);
			}
		}
	}
}



void CvBarbarians::DoSpawnBarbarianUnit(CvPlot* pPlot, bool bIgnoreMaxBarbarians, bool bFinishMoves)
{
	int iNumNearbyUnits;
#ifdef AUI_WARNING_FIXES
	uint iNearbyUnitLoop;
#else
	int iNearbyUnitLoop;
#endif
	int iRange = GC.getMAX_BARBARIANS_FROM_CAMP_NEARBY_RANGE();
	int iX;
	int iY;
	CvPlot* pNearbyPlot;
	DirectionTypes eDirection;

	CvGame& kGame = GC.getGame();

	if (pPlot == 0)
		return;


	if (pPlot && pPlot->GetNumCombatUnits() == 0)
	{
		UnitTypes eUnit;
		eUnit = GetRandomBarbarianUnitType(GC.getMap().getArea(pPlot->getArea()), UNITAI_FAST_ATTACK);

		if (eUnit != NO_UNIT)
		{
			CvUnit* pUnit = GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pPlot->getX(), pPlot->getY(), UNITAI_FAST_ATTACK);
			pUnit->finishMoves();
			return;
		}
	}

	m_aeValidBarbSpawnDirections.clear();


	iNumNearbyUnits = 0;

#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX;
	for (iY = -iRange; iY <= iRange; iY++)
	{
		iMaxDX = iRange - MAX(0, iY);
		for (iX = -iRange - MIN(0, iY); iX <= iMaxDX; iX++)
		{

			pNearbyPlot = plotXY(pPlot->getX(), pPlot->getY(), iX, iY);
#else
	for(iX = -iRange; iX <= iRange; iX++)
	{
		for(iY = -iRange; iY <= iRange; iY++)
		{

			pNearbyPlot = plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iX, iY, iRange);
#endif

			if(pNearbyPlot != NULL)
			{
				if(pNearbyPlot->getNumUnits() > 0)
				{
					for(iNearbyUnitLoop = 0; iNearbyUnitLoop < pNearbyPlot->getNumUnits(); iNearbyUnitLoop++)
					{
						const CvUnit* const unit = pNearbyPlot->getUnitByIndex(iNearbyUnitLoop);
#ifdef AUI_WARNING_FIXES
						if (unit && unit->isBarbarian())
#else
						if(unit && pNearbyPlot->getUnitByIndex(iNearbyUnitLoop)->isBarbarian())
#endif
						{
							iNumNearbyUnits++;
						}
					}
				}
			}
		}
	}

	if(iNumNearbyUnits <=       GC.getMAX_BARBARIANS_FROM_CAMP_NEARBY() || bIgnoreMaxBarbarians)
	{
		CvPlot* pLoopPlot;


		bool bCanSpawnBoats = kGame.getElapsedGameTurns() >        GC.getBARBARIAN_NAVAL_UNIT_START_TURN_SPAWN();


		for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; iDirectionLoop++)
		{
			eDirection = (DirectionTypes) iDirectionLoop;
			pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), eDirection);

			if(pLoopPlot != NULL)
			{
				if(pLoopPlot->getNumUnits() == 0)
				{
					if(!pLoopPlot->isImpassable() && !pLoopPlot->isMountain())
					{
						if(!pLoopPlot->isCity())
						{
							if(!pLoopPlot->isLake())
							{

								if(!pLoopPlot->isWater() || bCanSpawnBoats)
								{
									m_aeValidBarbSpawnDirections.push_back(eDirection);
								}
							}
						}
					}
				}
			}
		}


		if(m_aeValidBarbSpawnDirections.size() > 0)
		{
			int iIndex = kGame.getJonRandNum(m_aeValidBarbSpawnDirections.size(), "Barb Unit Location Spawn Roll");
			eDirection = (DirectionTypes) m_aeValidBarbSpawnDirections[iIndex];
			CvPlot* pSpawnPlot = plotDirection(pPlot->getX(), pPlot->getY(), eDirection);
			UnitAITypes eUnitAI;
			UnitTypes eUnit;


			if(pSpawnPlot->isWater())
			{
				eUnitAI = UNITAI_ATTACK_SEA;
			}

			else
			{
				eUnitAI = UNITAI_FAST_ATTACK;
			}

			eUnit = GetRandomBarbarianUnitType(GC.getMap().getArea(pSpawnPlot->getArea()), eUnitAI);

			if(eUnit != NO_UNIT)
			{
				CvUnit* pUnit = GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pSpawnPlot->getX(), pSpawnPlot->getY(), eUnitAI);
				if (bFinishMoves)
				{
					pUnit->finishMoves();
				}
			}
		}
	}
}
