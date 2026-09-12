







#include "CvGameCoreDLLPCH.h"
#include "CvArea.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvGlobals.h"
#include "CvPlayerAI.h"
#include "CvTeam.h"
#include "CvGameCoreUtils.h"
#include "CvInfos.h"

#include "ICvDLLUserInterface.h"
#include "CvEnumSerialization.h"

#include "CvInfosSerializationHelper.h"

#include "LintFree.h"



CvArea::CvArea()
{
	m_paiNumResources = NULL;
	m_paiNumImprovements = NULL;
	reset(0, false, true);
}



CvArea::~CvArea()
{
	uninit();
}



void CvArea::init(int iID, bool bWater)
{


	reset(iID, bWater);






}



void CvArea::uninit()
{
	SAFE_DELETE_ARRAY(m_paiNumResources);
	SAFE_DELETE_ARRAY(m_paiNumImprovements);
}





void CvArea::reset(int iID, bool bWater, bool bConstructorCall)
{
	int iI, iJ;



	uninit();

	m_iID = iID;
	m_iNumTiles = 0;
	m_iNumOwnedTiles = 0;
	m_iNumRiverEdges = 0;
	m_iNumUnits = 0;
	m_iNumCities = 0;
	m_iTotalPopulation = 0;
	m_iNumStartingPlots = 0;
	m_iNumNaturalWonders = 0;
	m_iTotalFoundValue = 0;
	m_Boundaries.m_iNorthEdge = 0;
	m_Boundaries.m_iSouthEdge = 0;
	m_Boundaries.m_iEastEdge = 0;
	m_Boundaries.m_iWestEdge = 0;

	m_bWater = bWater;
	m_bMountains = false;

	for(iI = 0; iI < REALLY_MAX_PLAYERS; iI++)
	{
		m_aiUnitsPerPlayer[iI] = 0;
		m_aiCitiesPerPlayer[iI] = 0;
		m_aiPopulationPerPlayer[iI] = 0;
		m_aiFreeSpecialist[iI] = 0;
	}

	for(iI = 0; iI < REALLY_MAX_TEAMS; iI++)
	{
		m_aiNumRevealedTiles[iI] = 0;
	}

	for(iI = 0; iI < REALLY_MAX_PLAYERS; iI++)
	{
		m_aTargetCities[iI].reset();
	}

	for(iI = 0; iI < REALLY_MAX_PLAYERS; iI++)
	{
		for(iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			m_aaiYieldRateModifier[iI][iJ] = 0;
#if defined(LEKMOD_AREA_BASED_CITY_YIELD)
			m_aaiCityYieldChange[iI][iJ] = 0;
#endif
		}
	}

	if(!bConstructorCall)
	{
		CvAssertMsg((0 < GC.getNumResourceInfos()) && "GC.getNumResourceInfos() is not greater than zero but an array is being allocated in CvArea::reset", "GC.getNumResourceInfos() is not greater than zero but an array is being allocated in CvArea::reset");
		int numRIs = GC.getNumResourceInfos();
		m_paiNumResources = FNEW(int[numRIs], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < numRIs; iI++)
		{
			m_paiNumResources[iI] = 0;
		}

		CvAssertMsg((0 < GC.getNumImprovementInfos()) && "GC.getNumImprovementInfos() is not greater than zero but an array is being allocated in CvArea::reset", "GC.getNumImprovementInfos() is not greater than zero but an array is being allocated in CvArea::reset");
		int numIIs = GC.getNumImprovementInfos();
		m_paiNumImprovements = FNEW(int[numIIs], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < numIIs; iI++)
		{
			m_paiNumImprovements[iI] = 0;
		}
	}
}


void CvArea::SetID(int iID)
{
	m_iID = iID;
}


int CvArea::calculateTotalBestNatureYield() const
{
	CvPlot* pLoopPlot;
	int iCount;
	int iI;

	iCount = 0;

	CvMap& theMap = GC.getMap();
	int iNumPlots = theMap.numPlots();
	for(iI = 0; iI < iNumPlots; iI++)
	{
		pLoopPlot = theMap.plotByIndexUnchecked(iI);

		if(pLoopPlot->getArea() == GetID())
		{
			iCount += pLoopPlot->calculateTotalBestNatureYield(NO_TEAM);
		}
	}

	return iCount;
}



int CvArea::countCoastalLand() const
{
	CvPlot* pLoopPlot;
	int iCount;
	int iI;

	if(isWater())
	{
		return 0;
	}

	iCount = 0;

	CvMap& theMap = GC.getMap();
	int iNumPlots = theMap.numPlots();
	for(iI = 0; iI < iNumPlots; iI++)
	{
		pLoopPlot = theMap.plotByIndexUnchecked(iI);

		if(pLoopPlot->getArea() == GetID())
		{
			if(pLoopPlot->isCoastalLand())
			{
				iCount++;
			}
		}
	}

	return iCount;
}


int CvArea::countNumUniqueResourceTypes() const
{
	int iCount = 0;
	int numRIs = GC.getNumResourceInfos();

	for(int iI = 0; iI < numRIs; iI++)
	{
		CvResourceInfo* resource = GC.getResourceInfo((ResourceTypes)iI);
		if(NULL != resource && resource->isOneArea())
		{
			iCount++;
		}
	}

	return iCount;
}


int CvArea::getNumTiles() const
{
	return m_iNumTiles;
}


void CvArea::changeNumTiles(int iChange)
{
	m_iNumTiles = (m_iNumTiles + iChange);
	CvAssert(m_iNumTiles >= 0);
}



int CvArea::getNumOwnedTiles() const
{
	return m_iNumOwnedTiles;
}



int CvArea::getNumUnownedTiles() const
{
	return (m_iNumTiles - getNumOwnedTiles());
}



void CvArea::changeNumOwnedTiles(int iChange)
{
	m_iNumOwnedTiles = (m_iNumOwnedTiles + iChange);
	CvAssert(getNumOwnedTiles() >= 0);
	CvAssert(getNumUnownedTiles() >= 0);
}



int CvArea::getNumRiverEdges() const
{
	return m_iNumRiverEdges;
}



void CvArea::changeNumRiverEdges(int iChange)
{
	m_iNumRiverEdges = (m_iNumRiverEdges + iChange);
	CvAssert(getNumRiverEdges() >= 0);
}



int CvArea::getNumUnits() const
{
	return m_iNumUnits;
}



int CvArea::getNumCities() const
{
	return m_iNumCities;
}



int CvArea::getTotalPopulation() const
{
	return m_iTotalPopulation;
}



int CvArea::getNumStartingPlots() const
{
	return m_iNumStartingPlots;
}



void CvArea::changeNumStartingPlots(int iChange)
{
	m_iNumStartingPlots = m_iNumStartingPlots + iChange;
	CvAssert(getNumStartingPlots() >= 0);
}




int CvArea::GetNumNaturalWonders() const
{
	return m_iNumNaturalWonders;
}



void CvArea::ChangeNumNaturalWonders(int iChange)
{
	m_iNumNaturalWonders += iChange;
}



bool CvArea::isWater() const
{
	return m_bWater;
}



bool CvArea::IsMountains() const
{
	return m_bMountains;
}


void CvArea::SetMountains(bool bValue)
{
	if(m_bMountains != bValue)
	{
		m_bMountains = bValue;
	}
}




int CvArea::getUnitsPerPlayer(PlayerTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	CvAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	if(eIndex < 0 || eIndex >= MAX_PLAYERS) return 0;

	return m_aiUnitsPerPlayer[eIndex];
}



void CvArea::changeUnitsPerPlayer(PlayerTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	CvAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	if(eIndex < 0 || eIndex >= MAX_PLAYERS) return;
	m_iNumUnits = (m_iNumUnits + iChange);
	CvAssert(getNumUnits() >= 0);
	m_aiUnitsPerPlayer[eIndex] = (m_aiUnitsPerPlayer[eIndex] + iChange);
	CvAssert(getUnitsPerPlayer(eIndex) >= 0);
}



int CvArea::getEnemyUnits(PlayerTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	CvAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	int iRtnValue = 0;

	TeamTypes eTeam = GET_PLAYER(eIndex).getTeam();


	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(atWar(kPlayer.getTeam(), eTeam))
		{
			iRtnValue += getUnitsPerPlayer((PlayerTypes)iI);
		}
	}

	return iRtnValue;
}



int CvArea::getCitiesPerPlayer(PlayerTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	CvAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	if(eIndex < 0 || eIndex >= MAX_PLAYERS) return 0;

	return m_aiCitiesPerPlayer[eIndex];
}



void CvArea::changeCitiesPerPlayer(PlayerTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	CvAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	if(eIndex < 0 || eIndex >= MAX_PLAYERS) return;

	m_iNumCities = (m_iNumCities + iChange);
	CvAssert(getNumCities() >= 0);
	m_aiCitiesPerPlayer[eIndex] = (m_aiCitiesPerPlayer[eIndex] + iChange);
	CvAssert(getCitiesPerPlayer(eIndex) >= 0);
}




int CvArea::getPopulationPerPlayer(PlayerTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	CvAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	if(eIndex < 0 || eIndex >= MAX_PLAYERS) return 0;

	return m_aiPopulationPerPlayer[eIndex];
}



void CvArea::changePopulationPerPlayer(PlayerTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	CvAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	if(eIndex < 0 || eIndex >= MAX_PLAYERS) return;

	m_iTotalPopulation = (m_iTotalPopulation + iChange);
	CvAssert(getTotalPopulation() >= 0);
	m_aiPopulationPerPlayer[eIndex] = (m_aiPopulationPerPlayer[eIndex] + iChange);
	CvAssert(getPopulationPerPlayer(eIndex) >= 0);
}




int CvArea::getFreeSpecialist(PlayerTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	CvAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	if(eIndex < 0 || eIndex >= MAX_PLAYERS) return 0;

	return m_aiFreeSpecialist[eIndex];
}



void CvArea::changeFreeSpecialist(PlayerTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	CvAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	if(eIndex < 0 || eIndex >= MAX_PLAYERS) return;

	if(iChange != 0)
	{
		m_aiFreeSpecialist[eIndex] = (m_aiFreeSpecialist[eIndex] + iChange);
		CvAssert(getFreeSpecialist(eIndex) >= 0);
	}
}




int CvArea::getTotalFoundValue() const
{
	return m_iTotalFoundValue;
}



void CvArea::setTotalFoundValue(int iNewValue)
{
	m_iTotalFoundValue = iNewValue;
	CvAssert(m_iTotalFoundValue >= 0);
}



int CvArea::getNumRevealedTiles(TeamTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	CvAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	if(eIndex < 0 || eIndex >= MAX_PLAYERS) return 0;

	return m_aiNumRevealedTiles[eIndex];
}



int CvArea::getNumUnrevealedTiles(TeamTypes eIndex) const
{
	return (m_iNumTiles - getNumRevealedTiles(eIndex));
}



void CvArea::changeNumRevealedTiles(TeamTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	CvAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	if(eIndex < 0 || eIndex >= MAX_PLAYERS) return;

	m_aiNumRevealedTiles[eIndex] = (m_aiNumRevealedTiles[eIndex] + iChange);
	CvAssert(getNumRevealedTiles(eIndex) >= 0);
}



CvCity* CvArea::getTargetCity(PlayerTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	CvAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	if(eIndex < 0 || eIndex >= MAX_PLAYERS) return 0;

	return getCity(m_aTargetCities[eIndex]);
}



void CvArea::setTargetCity(PlayerTypes eIndex, CvCity* pNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	CvAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	if(eIndex < 0 || eIndex >= MAX_PLAYERS) return;

	if(pNewValue != NULL)
	{
		m_aTargetCities[eIndex] = pNewValue->GetIDInfo();
	}
	else
	{
		m_aTargetCities[eIndex].reset();
	}
}



int CvArea::getYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2) const
{
	CvAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be >= 0");
	CvAssertMsg(eIndex1 < MAX_PLAYERS, "eIndex1 is expected to be < MAX_PLAYERS");
	if(eIndex1 < 0 || eIndex1 >= MAX_PLAYERS) return 0;

	CvAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be >= 0");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be < NUM_YIELD_TYPES");
	if(eIndex2 < 0 || eIndex2 >= NUM_YIELD_TYPES) return 0;

	return m_aaiYieldRateModifier[eIndex1][eIndex2];
}



void CvArea::changeYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	CvAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be >= 0");
	CvAssertMsg(eIndex1 < MAX_PLAYERS, "eIndex1 is expected to be < MAX_PLAYERS");
	if(eIndex1 < 0 || eIndex1 >= MAX_PLAYERS) return;

	CvAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be >= 0");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be < NUM_YIELD_TYPES");
	if(eIndex2 < 0 || eIndex2 >= NUM_YIELD_TYPES) return;

	if(iChange != 0)
	{
		m_aaiYieldRateModifier[eIndex1][eIndex2] = (m_aaiYieldRateModifier[eIndex1][eIndex2] + iChange);

		CvPlayer& thisPlayer = GET_PLAYER(eIndex1);
		thisPlayer.invalidateYieldRankCache(eIndex2);

		if(thisPlayer.getTeam() == GC.getGame().getActiveTeam())
		{
			GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}
#if defined(LEKMOD_AREA_BASED_CITY_YIELD)
int CvArea::getCityYieldChange(PlayerTypes ePlayer, YieldTypes eYield) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be >= 0");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be < MAX_PLAYERS");
	if(ePlayer < 0 || ePlayer >= MAX_PLAYERS) return 0;
	CvAssertMsg(eYield >= 0, "eYield is expected to be >= 0");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield is expected to be < NUM_YIELD_TYPES");
	if(eYield < 0 || eYield >= NUM_YIELD_TYPES) return 0;
	return m_aaiCityYieldChange[ePlayer][eYield];
}
void CvArea::setCityYieldChange(PlayerTypes ePlayer, YieldTypes eYield, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be >= 0");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be < MAX_PLAYERS");
	if(ePlayer < 0 || ePlayer >= MAX_PLAYERS) return;
	CvAssertMsg(eYield >= 0, "eYield is expected to be >= 0");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield is expected to be < NUM_YIELD_TYPES");
	if(eYield < 0 || eYield >= NUM_YIELD_TYPES) return;
	m_aaiCityYieldChange[ePlayer][eYield] = iChange;
}
void CvArea::changeCityYieldChange(PlayerTypes ePlayer, YieldTypes eYield, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be >= 0");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be < MAX_PLAYERS");
	if(ePlayer < 0 || ePlayer >= MAX_PLAYERS) return;
	CvAssertMsg(eYield >= 0, "eYield is expected to be >= 0");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield is expected to be < NUM_YIELD_TYPES");
	if(eYield < 0 || eYield >= NUM_YIELD_TYPES) return;
	setCityYieldChange(ePlayer, eYield, (getCityYieldChange(ePlayer, eYield) + iChange));
}
#endif


int CvArea::getNumResources(ResourceTypes eResource) const
{
	CvAssertMsg(eResource >= 0, "eResource expected to be >= 0");
	CvAssertMsg(eResource < GC.getNumResourceInfos(), "eResource expected to be < GC.getNumResourceInfos");
	return m_paiNumResources[eResource];
}




int CvArea::getNumTotalResources() const
{
	int iTotal = 0;

	int iNumResourceInfos = GC.getNumResourceInfos();
	for(int iI = 0; iI < iNumResourceInfos; iI++)
	{
		iTotal += m_paiNumResources[iI];
	}

	return iTotal;
}



void CvArea::changeNumResources(ResourceTypes eResource, int iChange)
{
	CvAssertMsg(eResource >= 0, "eResource expected to be >= 0");
	CvAssertMsg(eResource < GC.getNumResourceInfos(), "eResource expected to be < GC.getNumResourceInfos");
	m_paiNumResources[eResource] = (m_paiNumResources[eResource] + iChange);
	CvAssert(getNumResources(eResource) >= 0);
}




int CvArea::getNumImprovements(ImprovementTypes eImprovement) const
{
	CvAssertMsg(eImprovement >= 0, "eImprovement expected to be >= 0");
	CvAssertMsg(eImprovement < GC.getNumImprovementInfos(), "eImprovement expected to be < GC.getNumImprovementInfos");
	return m_paiNumImprovements[eImprovement];
}



void CvArea::changeNumImprovements(ImprovementTypes eImprovement, int iChange)
{
	CvAssertMsg(eImprovement >= 0, "eImprovement expected to be >= 0");
	CvAssertMsg(eImprovement < GC.getNumImprovementInfos(), "eImprovement expected to be < GC.getNumImprovementInfos");
	m_paiNumImprovements[eImprovement] = (m_paiNumImprovements[eImprovement] + iChange);
	CvAssert(getNumImprovements(eImprovement) >= 0);
}


CvAreaBoundaries CvArea::getAreaBoundaries() const
{
	return m_Boundaries;
}


#ifdef AUI_WARNING_FIXES
void CvArea::setAreaBoundaries(const CvAreaBoundaries& newBoundaries)
#else
void CvArea::setAreaBoundaries(CvAreaBoundaries newBoundaries)
#endif
{
	m_Boundaries = newBoundaries;
}



void CvArea::GetTopAndBottomLatitudes(int& iTopLatitude, int& iBottomLatitude)
{
	int iGridHeight = GC.getMap().getGridHeight();
	if(GC.getMap().isWrapX() || !(GC.getMap().isWrapY()))
	{
		iTopLatitude = ((m_Boundaries.m_iNorthEdge * 100) / iGridHeight);
		iBottomLatitude = ((m_Boundaries.m_iSouthEdge * 100) / iGridHeight);
	}
	else
	{
		iTopLatitude = ((m_Boundaries.m_iEastEdge * 100) / iGridHeight);
		iBottomLatitude = ((m_Boundaries.m_iWestEdge * 100) / iGridHeight);
	}

	int iMapTop = GC.getMap().getTopLatitude();
	int iMapBottom = GC.getMap().getBottomLatitude();

	iTopLatitude = ((iTopLatitude * (iMapTop - iMapBottom)) / 100);
	iBottomLatitude = ((iBottomLatitude * (iMapTop - iMapBottom)) / 100);

	iTopLatitude = abs(iTopLatitude + iMapBottom);
	iBottomLatitude = abs(iBottomLatitude + iMapBottom);
}



int CvArea::GetAreaMaxLatitude()
{
	int iTopLatitude, iBottomLatitude;
	GetTopAndBottomLatitudes(iTopLatitude, iBottomLatitude);

	return max(iTopLatitude, iBottomLatitude);
}



int CvArea::GetAreaMinLatitude()
{
	int iTopLatitude, iBottomLatitude;
	GetTopAndBottomLatitudes(iTopLatitude, iBottomLatitude);

	return min(iTopLatitude, iBottomLatitude);
}


void CvArea::read(FDataStream& kStream)
{
	int iI;


	reset();


	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_iID;
	kStream >> m_iNumTiles;
	kStream >> m_iNumOwnedTiles;
	kStream >> m_iNumRiverEdges;
	kStream >> m_iNumUnits;
	kStream >> m_iNumCities;
	kStream >> m_iTotalPopulation;
	kStream >> m_iNumStartingPlots;
	kStream >> m_iNumNaturalWonders;
	kStream >> m_iTotalFoundValue;

	kStream >> m_Boundaries.m_iNorthEdge;
	kStream >> m_Boundaries.m_iSouthEdge;
	kStream >> m_Boundaries.m_iEastEdge;
	kStream >> m_Boundaries.m_iWestEdge;

	kStream >> m_bWater;
	kStream >> m_bMountains;

	kStream >> m_aiUnitsPerPlayer;
	kStream >> m_aiCitiesPerPlayer;
	kStream >> m_aiPopulationPerPlayer;
	kStream >> m_aiFreeSpecialist;
	kStream >> m_aiNumRevealedTiles;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		kStream >> (int&)m_aTargetCities[iI].eOwner;
		kStream >> m_aTargetCities[iI].iID;
		kStream >> m_aaiYieldRateModifier[iI];
#if defined(LEKMOD_AREA_BASED_CITY_YIELD)
		kStream >> m_aaiCityYieldChange[iI];
#endif
	}
	
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiNumResources, GC.getNumResourceInfos());

	ImprovementArrayHelpers::Read(kStream, m_paiNumImprovements);
}



void CvArea::write(FDataStream& kStream) const
{
	int iI;


	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << m_iID;
	kStream << m_iNumTiles;
	kStream << m_iNumOwnedTiles;
	kStream << m_iNumRiverEdges;
	kStream << m_iNumUnits;
	kStream << m_iNumCities;
	kStream << m_iTotalPopulation;
	kStream << m_iNumStartingPlots;
	kStream << m_iNumNaturalWonders;
	kStream << m_iTotalFoundValue;

	kStream << m_Boundaries.m_iNorthEdge;
	kStream << m_Boundaries.m_iSouthEdge;
	kStream << m_Boundaries.m_iEastEdge;
	kStream << m_Boundaries.m_iWestEdge;

	kStream << m_bWater;
	kStream << m_bMountains;

	kStream << m_aiUnitsPerPlayer;
	kStream << m_aiCitiesPerPlayer;
	kStream << m_aiPopulationPerPlayer;
	kStream << m_aiFreeSpecialist;
	kStream << m_aiNumRevealedTiles;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		kStream << m_aTargetCities[iI].eOwner;
		kStream << m_aTargetCities[iI].iID;
		kStream << m_aaiYieldRateModifier[iI];
#if defined(LEKMOD_AREA_BASED_CITY_YIELD)
		kStream << m_aaiCityYieldChange[iI];
#endif
	}

	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes>(kStream, m_paiNumResources, GC.getNumResourceInfos());
	ImprovementArrayHelpers::Write(kStream, m_paiNumImprovements, GC.getNumImprovementInfos());
}


FDataStream& operator<<(FDataStream& saveTo, const CvArea& readFrom)
{
	readFrom.write(saveTo);
	return saveTo;
}


FDataStream& operator>>(FDataStream& loadFrom, CvArea& writeTo)
{
	writeTo.read(loadFrom);
	return loadFrom;
}
