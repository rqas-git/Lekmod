






#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreUtils.h"
#include "CvStartPositioner.h"
#include "CvMinorCivAI.h"


#include "LintFree.h"





CvStartPositioner::CvStartPositioner(void)
{
#ifdef AUI_WARNING_FIXES
	m_pSiteEvaluator = NULL;
#endif
	m_iRequiredSeparation = 0;
	m_iBestFoundValueOnMap = 0;
}


CvStartPositioner::~CvStartPositioner(void)
{
}


void CvStartPositioner::Init(CvSiteEvaluatorForStart* pSiteEvaluator)
{
	CvAssert(pSiteEvaluator);
	if(!pSiteEvaluator)
		return;

	m_pSiteEvaluator = pSiteEvaluator;
	m_pSiteEvaluator->ComputeFlavorMultipliers(NULL);
}


void CvStartPositioner::DivideMapIntoRegions(int iNumRegions)
{
	CvArea* pLoopArea(NULL);
	int iLoop;
	int iNumRegionsPlaced = 0;


	m_ContinentVector.clear();
	m_StartRegionVector.clear();


	ComputeTileFertilityValues();


	for(pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
	{

		if(pLoopArea->getTotalFoundValue() > 0 && pLoopArea->getNumTiles() >= GC.getMIN_START_AREA_TILES())
		{
			CvContinent continent;
			continent.SetFertility(pLoopArea->getTotalFoundValue());
			continent.SetArea(pLoopArea->GetID());
			m_ContinentVector.push_back(continent);
		}
	}

	if(m_ContinentVector.size() > 0)
	{

		while(iNumRegionsPlaced < iNumRegions)
		{
			std::sort(m_ContinentVector.begin(), m_ContinentVector.end());


			m_ContinentVector[0].AddRegion();
			iNumRegionsPlaced++;
		}
	}


	for(unsigned int iI=0; iI < m_ContinentVector.size(); iI++)
	{
		DivideContinentIntoRegions(m_ContinentVector[iI]);
	}


	std::sort(m_StartRegionVector.begin(), m_StartRegionVector.end());
}


void CvStartPositioner::ComputeFoundValues()
{
	CvPlot* pLoopPlot(NULL);


#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getMap().numPlots(); iI++)
#else
	for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
#endif
	{
#ifdef AUI_STARTPOSITIONER_FLAVORED_STARTS

#else



#endif
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
		CvAssert(pLoopPlot);
		if(!pLoopPlot) continue;
		int iValue = m_pSiteEvaluator->PlotFoundValue(pLoopPlot, NULL);
#ifndef AUI_STARTPOSITIONER_FLAVORED_STARTS
		pLoopPlot->setFoundValue((PlayerTypes)1, iValue);
#endif

		if(iValue > m_iBestFoundValueOnMap)
		{
			m_iBestFoundValueOnMap = iValue;
		}
	}
}


void CvStartPositioner::RankPlayerStartOrder()
{

	m_PlayerOrder.clear();


	for(int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		CvPlayerAI& player = GET_PLAYER((PlayerTypes)iI);

		if(player.isAlive())
		{
			if(player.getStartingPlot() == NULL)
			{
				CvPlayerStartRank playerRank;
				playerRank.m_iPlayerID = iI;




				playerRank.m_iRank = player.getHandicapInfo().getStartingLocationPercent();
				if(player.isMinorCiv())
				{
					playerRank.m_iRank *= 10;
				}
				playerRank.m_iRank += GC.getGame().getJonRandNum(10, "Player Start Plot Draft Order");
				m_PlayerOrder.push_back(playerRank);
			}
		}
	}


	std::sort(m_PlayerOrder.begin(), m_PlayerOrder.end());
}


void CvStartPositioner::AssignStartingLocations()
{
	CvString strString;
	unsigned int iNextRegion = 0;
	int iPlayersPlaced = 0;
	int iMajorCivs = 0;
	for(size_t i = 0; i < m_PlayerOrder.size(); ++i)
	{
		if(!GET_PLAYER((PlayerTypes)m_PlayerOrder[i].m_iPlayerID).isMinorCiv())
			iMajorCivs++;
	}


	m_iRequiredSeparation = StartingPlotRange();


	if(GC.getMap().getWorldSize() == WORLDSIZE_DEBUG)
	{
		m_iRequiredSeparation = 1;
	}

	while(iPlayersPlaced < iMajorCivs && iNextRegion < m_StartRegionVector.size())
	{
		if(m_StartRegionVector[iNextRegion].m_bLargeEnoughForMajorCiv)
		{
			strString.Format("Trying to place major civ with full separation of %d", m_iRequiredSeparation);
			LogStartPositionMessage(strString);
			bool success = AddCivToRegion(m_PlayerOrder[iPlayersPlaced].m_iPlayerID, m_StartRegionVector[iNextRegion], false);
			if(success)
			{
				iPlayersPlaced++;
				m_StartRegionVector[iNextRegion].m_iNumCivsPlaced++;
			}
		}
		iNextRegion++;
	}



	int iHalfMinimumDist = GC.getMIN_CIV_STARTING_DISTANCE() / 2;
	while(iPlayersPlaced < iMajorCivs && m_iRequiredSeparation >= 0)
	{

		std::sort(m_StartRegionVector.begin(), m_StartRegionVector.end());


		m_iRequiredSeparation--;
		iNextRegion = 0;

		while(iPlayersPlaced < iMajorCivs && iNextRegion < m_StartRegionVector.size())
		{
			if(m_StartRegionVector[iNextRegion].m_bLargeEnoughForMajorCiv)
			{
				strString.Format("Trying to place major civ with reduced separation of %d", m_iRequiredSeparation);
				LogStartPositionMessage(strString);
				bool success = AddCivToRegion(m_PlayerOrder[iPlayersPlaced].m_iPlayerID, m_StartRegionVector[iNextRegion], m_iRequiredSeparation < iHalfMinimumDist);
				if(success)
				{
					iPlayersPlaced++;
					m_StartRegionVector[iNextRegion].m_iNumCivsPlaced++;
				}
			}
			iNextRegion++;
		}
	}

	CvAssertMsg(iPlayersPlaced == iMajorCivs, "AssignStartingLocations() can't find enough starting locations for major civs");


	m_iRequiredSeparation = StartingPlotRange();


	std::sort(m_StartRegionVector.begin(), m_StartRegionVector.end());
	while(iPlayersPlaced < (int)m_PlayerOrder.size() && iNextRegion < m_StartRegionVector.size())
	{
		strString.Format("Trying to place minor civ with full separation of %d", m_iRequiredSeparation);
		LogStartPositionMessage(strString);
		bool success = AddCivToRegion(m_PlayerOrder[iPlayersPlaced].m_iPlayerID, m_StartRegionVector[iNextRegion], false);
		if(success)
		{
			iPlayersPlaced++;
			m_StartRegionVector[iNextRegion].m_iNumCivsPlaced++;


			std::sort(m_StartRegionVector.begin(), m_StartRegionVector.end());
			iNextRegion = 0;
		}


		else
		{
			iNextRegion++;
		}
	}


	while(iPlayersPlaced < (int)m_PlayerOrder.size() && m_iRequiredSeparation >= 0)
	{

		std::sort(m_StartRegionVector.begin(), m_StartRegionVector.end());


		m_iRequiredSeparation--;
		iNextRegion = 0;

		while(iPlayersPlaced < (int)m_PlayerOrder.size() && iNextRegion < m_StartRegionVector.size())
		{
			strString.Format("Trying to place minor civ with reduced separation of %d", m_iRequiredSeparation);
			LogStartPositionMessage(strString);
			bool success = AddCivToRegion(m_PlayerOrder[iPlayersPlaced].m_iPlayerID, m_StartRegionVector[iNextRegion], false);
			if(success)
			{
				iPlayersPlaced++;
				m_StartRegionVector[iNextRegion].m_iNumCivsPlaced++;


				std::sort(m_StartRegionVector.begin(), m_StartRegionVector.end());
				iNextRegion = 0;
			}


			else
			{
				iNextRegion++;
			}
		}
	}
}


int CvStartPositioner::GetRegion(int iX, int iY)
{
	int iRegion = 0;

	while(iRegion < (int)m_StartRegionVector.size())
	{
		if(m_StartRegionVector[iRegion].Contains(iX, iY))
		{
			return iRegion;
		}
		iRegion++;
	}

	return -1;
}




void CvStartPositioner::DivideContinentIntoRegions(CvContinent continent)
{

	CvStartRegion region;
	region.m_uiFertility = continent.GetFertility();
	region.m_iAreaID = continent.GetArea();
	region.m_Boundaries = GC.getMap().getArea(continent.GetArea())->getAreaBoundaries();


	if(continent.GetNumRegions() > 0)
	{

		SubdivideRegion(region, continent.GetNumRegions());
	}


	else
	{
		region.m_bLargeEnoughForMajorCiv = false;
		m_StartRegionVector.push_back(region);
	}
}


void CvStartPositioner::ComputeTileFertilityValues()
{
	CvArea* pLoopArea(NULL);
	int iLoop;
	CvPlot* pLoopPlot(NULL);
#ifdef AUI_WARNING_FIXES
	uint iI;
#else
	int iI;
#endif
	int uiFertility;


	for(pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
	{
		pLoopArea->setTotalFoundValue(0);
	}


	for(iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
		CvAssert(pLoopPlot);
		if(!pLoopPlot) continue;




		uiFertility = m_pSiteEvaluator->PlotFertilityValue(pLoopPlot);
		pLoopPlot->setFoundValue((PlayerTypes)0, uiFertility);

		if(uiFertility > 0)
		{

			CvArea* pArea = GC.getMap().getArea(pLoopPlot->getArea());
			CvAssert(pArea);
			if(!pArea) continue;
			pArea->setTotalFoundValue(pArea->getTotalFoundValue() + uiFertility);
		}
	}
}


void CvStartPositioner::SubdivideRegion(CvStartRegion region, int iNumDivisions)
{
	int iNumDivides = 0;
	int iLaterSubdivisions = 0;


	if(iNumDivisions == 1)
	{

		m_StartRegionVector.push_back(region);
	}


	else if(iNumDivisions > 1)
	{

		bool bTaller = true;
		if((region.m_Boundaries.m_iNorthEdge - region.m_Boundaries.m_iSouthEdge) <
		        (region.m_Boundaries.m_iEastEdge - region.m_Boundaries.m_iWestEdge))
		{
			bTaller = false;
		}














		switch(iNumDivisions)
		{
		case 2:
			iNumDivides = 2;
			iLaterSubdivisions = 1;
			break;
		case 3:
			iNumDivides = 3;
			iLaterSubdivisions = 1;
			break;
		case 4:
			iNumDivides = 2;
			iLaterSubdivisions = 2;
			break;
		case 5:
		case 6:
			iNumDivides = 3;
			iLaterSubdivisions = 2;
			break;
		case 7:
		case 8:
			iNumDivides = 2;
			iLaterSubdivisions = 4;
			break;
		case 9:
			iNumDivides = 3;
			iLaterSubdivisions = 3;
			break;
		case 10:
		case 11:
		case 12:
			iNumDivides = 3;
			iLaterSubdivisions = 4;
			break;
		case 13:
		case 14:
		case 15:
		case 16:
			iNumDivides = 2;
			iLaterSubdivisions = 8;
			break;
		case 17:
		case 18:
			iNumDivides = 3;
			iLaterSubdivisions = 6;
			break;
		case 19:
		case 20:
			iNumDivides = 2;
			iLaterSubdivisions = 10;
			break;
		case 21:
		case 22:
			iNumDivides = 3;
			iLaterSubdivisions = 8;
			break;
		default:
			CvAssertMsg(false, "Trying to create regions for more than 18 major civs.");
		}

		if(iNumDivides == 2)
		{
			CvStartRegion secondRegion;
			ChopIntoTwoRegions(bTaller, &region, &secondRegion, 50);
			SubdivideRegion(region, iLaterSubdivisions);
			SubdivideRegion(secondRegion, iLaterSubdivisions);
		}
		else if(iNumDivides == 3)
		{
			CvStartRegion secondRegion;
			CvStartRegion thirdRegion;
			ChopIntoThreeRegions(bTaller, &region, &secondRegion, &thirdRegion);
			SubdivideRegion(region, iLaterSubdivisions);
			SubdivideRegion(secondRegion, iLaterSubdivisions);
			SubdivideRegion(thirdRegion, iLaterSubdivisions);
		}
	}
}


void CvStartPositioner::ChopIntoTwoRegions(bool bTaller, CvStartRegion* region, CvStartRegion* secondRegion, int iChopPercent)
{
	CvAssert(region);
	if(!region) return;
	CvAssert(secondRegion);
	if(!secondRegion) return;

	int uiTargetFertility;
	int uiFertilitySoFar = 0;


	secondRegion->m_iAreaID = region->m_iAreaID;


	uiTargetFertility = region->m_uiFertility * iChopPercent / 100;


	if(bTaller)
	{

		secondRegion->m_Boundaries.m_iEastEdge = region->m_Boundaries.m_iEastEdge;
		secondRegion->m_Boundaries.m_iWestEdge = region->m_Boundaries.m_iWestEdge;
		secondRegion->m_Boundaries.m_iNorthEdge = region->m_Boundaries.m_iNorthEdge;


		int iNorthEdge = region->m_Boundaries.m_iSouthEdge;
		while(uiFertilitySoFar < uiTargetFertility)
		{
			uiFertilitySoFar += ComputeRowFertility(region->m_iAreaID,
			                                        region->m_Boundaries.m_iWestEdge, region->m_Boundaries.m_iEastEdge, iNorthEdge, iNorthEdge);
			iNorthEdge++;
		}


		region->m_Boundaries.m_iNorthEdge = iNorthEdge - 1;
		secondRegion->m_Boundaries.m_iSouthEdge = iNorthEdge;
	}


	else
	{

		secondRegion->m_Boundaries.m_iNorthEdge = region->m_Boundaries.m_iNorthEdge;
		secondRegion->m_Boundaries.m_iSouthEdge = region->m_Boundaries.m_iSouthEdge;
		secondRegion->m_Boundaries.m_iEastEdge = region->m_Boundaries.m_iEastEdge;


		int iEastEdge = region->m_Boundaries.m_iWestEdge;
		while(uiFertilitySoFar < uiTargetFertility)
		{
			uiFertilitySoFar += ComputeRowFertility(region->m_iAreaID,
			                                        iEastEdge, iEastEdge, region->m_Boundaries.m_iSouthEdge, region->m_Boundaries.m_iNorthEdge);
			iEastEdge++;
		}


		region->m_Boundaries.m_iEastEdge = iEastEdge - 1;
		secondRegion->m_Boundaries.m_iWestEdge = iEastEdge;
	}


	secondRegion->m_uiFertility = region->m_uiFertility - uiFertilitySoFar;
	region->m_uiFertility = uiFertilitySoFar;
}


void CvStartPositioner::ChopIntoThreeRegions(bool bTaller, CvStartRegion* region, CvStartRegion* secondRegion, CvStartRegion* thirdRegion)
{
	CvAssert(region);
	if(!region) return;
	CvAssert(secondRegion);
	if(!secondRegion) return;
	CvAssert(thirdRegion);
	if(!thirdRegion) return;


	ChopIntoTwoRegions(bTaller, region, secondRegion, 33);


	ChopIntoTwoRegions(bTaller, secondRegion, thirdRegion, 50);
}


int CvStartPositioner::ComputeRowFertility(int iAreaID, int xMin, int xMax, int yMin, int yMax)
{
	int rtnValue = 0;

	for(int iRow = yMin; iRow <= yMax; iRow++)
	{
		for(int iCol = xMin; iCol <= xMax; iCol++)
		{
			CvPlot* pPlot = GC.getMap().plot(iCol, iRow);

			if(pPlot && pPlot->getArea() == iAreaID)
			{



				rtnValue += pPlot->getFoundValue((PlayerTypes)0);
			}
		}
	}

	return rtnValue;
}


bool CvStartPositioner::AddCivToRegion(int iPlayerIndex, CvStartRegion region, bool bRelaxFoodReq)
{
	CvString strString;
	int uiBestFoundValue = 0;
	int uiPlotFoundValue;
	CvPlot* pBestPlot = NULL;
	CvPlot* pLoopPlot = NULL;
	int iMinorFoodReq = GC.getMINOR_CIV_FOOD_REQUIREMENT();
	int iMajorFoodReq = GC.getMAJOR_CIV_FOOD_REQUIREMENT();
	bool bIsMinorCiv = GET_PLAYER((PlayerTypes)iPlayerIndex).isMinorCiv();
	TeamTypes eTeam = GET_PLAYER((PlayerTypes)iPlayerIndex).getTeam();
	int iPercentOfBest = GC.getMIN_START_FOUND_VALUE_AS_PERCENT_OF_BEST();

	MinorCivTypes eMinorCivType = NO_MINORCIV;
#ifdef AUI_STARTPOSITIONER_COASTAL_CIV_WATER_BIAS
	CvMinorCivInfo* pMinorCivInfo = NULL;
#endif
	if(bIsMinorCiv)
	{
		eMinorCivType =GET_PLAYER((PlayerTypes) iPlayerIndex).GetMinorCivAI()->GetMinorCivType();
#ifdef AUI_STARTPOSITIONER_COASTAL_CIV_WATER_BIAS
		pMinorCivInfo = GC.getMinorCivInfo(eMinorCivType);
#endif
	}

	bool bDebugMap = GC.getMap().getWorldSize() == WORLDSIZE_DEBUG;


	if(bDebugMap)
	{
		iMinorFoodReq = 0;
		iMajorFoodReq = 0;
	}

#ifdef AUI_STARTPOSITIONER_COASTAL_CIV_WATER_BIAS
	CvCivilizationInfo* pCivInfo = GC.getCivilizationInfo(GET_PLAYER((PlayerTypes)iPlayerIndex).getCivilizationType());
#endif


	for(int iRow = region.m_Boundaries.m_iSouthEdge; iRow <= region.m_Boundaries.m_iNorthEdge; iRow++)
	{
		for(int iCol = region.m_Boundaries.m_iWestEdge; iCol <= region.m_Boundaries.m_iEastEdge; iCol++)
		{
			pLoopPlot = GC.getMap().plot(iCol, iRow);


			if(pLoopPlot && pLoopPlot->getArea() == region.m_iAreaID)
			{

				if((bRelaxFoodReq && MeetsFoodRequirement(pLoopPlot, eTeam, iMinorFoodReq))
				        || (bIsMinorCiv && MeetsFoodRequirement(pLoopPlot, eTeam, iMinorFoodReq))
				        || MeetsFoodRequirement(pLoopPlot, eTeam, iMajorFoodReq))
				{
#ifdef AUI_STARTPOSITIONER_FLAVORED_STARTS

					uiPlotFoundValue = m_pSiteEvaluator->PlotFoundValue(pLoopPlot, &(GET_PLAYER((PlayerTypes)iPlayerIndex)));
#else



					uiPlotFoundValue = pLoopPlot->getFoundValue((PlayerTypes)1);
#endif


#ifdef AUI_STARTPOSITIONER_COASTAL_CIV_WATER_BIAS
					if ((pMinorCivInfo && pMinorCivInfo->GetMinorCivTrait() == MINOR_CIV_TRAIT_MARITIME) ||
						(!pMinorCivInfo && pCivInfo && pCivInfo->isCoastalCiv()))
					{
#else
					if(bIsMinorCiv)
					{
						if(GC.getMinorCivInfo(eMinorCivType)->GetMinorCivTrait() == MINOR_CIV_TRAIT_MARITIME)
#endif
						{
							if(!pLoopPlot->isCoastalLand(GC.getLAKE_MAX_AREA_SIZE()))
							{
								uiPlotFoundValue /= 2;
							}
						}
					}


					if(bDebugMap || ((int)uiPlotFoundValue > (int)m_iBestFoundValueOnMap * iPercentOfBest / 100))
					{

						if(!TooCloseToAnotherCiv(pLoopPlot))
						{

							if(bDebugMap || uiPlotFoundValue > uiBestFoundValue)
							{
								uiBestFoundValue = uiPlotFoundValue;
								pBestPlot = pLoopPlot;
							}
						}
					}
				}
			}
		}
	}

	if(pBestPlot != NULL)
	{
		GET_PLAYER((PlayerTypes)iPlayerIndex).setStartingPlot(pBestPlot);
		strString.Format("Adding player, id = %d, plot x = %d, y = %d, fertility = %u",
		                 iPlayerIndex, pBestPlot->getX(), pBestPlot->getY(), uiBestFoundValue);
		LogStartPositionMessage(strString);
		return true;
	}
	else
	{
		return false;
	}
}


bool CvStartPositioner::TooCloseToAnotherCiv(CvPlot* pPlot)
{
	int iI;
	bool rtnValue = false;
	CvPlot* pStartPlot(NULL);

	CvAssert(pPlot);
	if(!pPlot)
		return rtnValue;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			pStartPlot = GET_PLAYER((PlayerTypes)iI).getStartingPlot();
			if(pStartPlot != NULL)
			{

				if(pStartPlot->getArea() == pPlot->getArea())
				{
					if(plotDistance(pPlot->getX(), pPlot->getY(),
					                pStartPlot->getX(), pStartPlot->getY()) < m_iRequiredSeparation)
					{
						return true;
					}
				}


				else
				{
					int iSeparationIfOnAnotherContinent = m_iRequiredSeparation * GC.getMIN_DISTANCE_OTHER_AREA_PERCENT() / 100;
					if(plotDistance(pPlot->getX(), pPlot->getY(),
					                pStartPlot->getX(), pStartPlot->getY()) < iSeparationIfOnAnotherContinent)
					{
						return true;
					}
				}
			}
		}
	}

	return rtnValue;
}


bool CvStartPositioner::MeetsFoodRequirement(CvPlot* pPlot, TeamTypes eTeam, int iFoodRequirement)
{
	CvPlot* pLoopPlot(NULL);
	bool bFoundFoodPlot = false;

	CvAssert(pPlot);
	if(!pPlot)
		return bFoundFoodPlot;

	for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));
		if(pLoopPlot == NULL)
		{
			return false;
		}
		else
		{
			if(pLoopPlot->calculateNatureYield(YIELD_FOOD, eTeam, false) >= iFoodRequirement)
			{
				bFoundFoodPlot = true;
				break;
			}
		}
	}
	return bFoundFoodPlot;
}


int CvStartPositioner::StartingPlotRange() const
{
	int iRange;
	int iNumMinors, iNumMajors;
	int iExpectedCities;


	iRange = (GC.getMap().maxPlotDistance() + 10);
	iRange *= GC.getSTARTING_DISTANCE_PERCENT();
	iRange /= 100;


	iNumMajors = GC.getGame().countMajorCivsAlive();
	iNumMinors = GC.getGame().countCivPlayersAlive() - iNumMajors;
	iExpectedCities = GC.getMap().getWorldInfo().getTargetNumCities() * iNumMajors;
	iExpectedCities += iNumMinors;
	iExpectedCities = std::max(1,iExpectedCities);


	iRange *= GC.getMap().getLandPlots();
	iRange /= (NUM_CITY_PLOTS / 2);
	iRange /= iExpectedCities;



	return std::max(iRange, GC.getMIN_CIV_STARTING_DISTANCE());
}


void CvStartPositioner::LogStartPositionMessage(CvString strMsg)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog("StartPositionLog.csv", FILogFile::kDontTimeStamp);
		pLog->Msg(strMsg);
	}
}
