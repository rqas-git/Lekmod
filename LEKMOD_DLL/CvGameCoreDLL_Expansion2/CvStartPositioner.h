






#pragma once

#ifndef CIV5_START_POSITIONER_H
#define CIV5_START_POSITIONER_H









class CvContinent
{
public:
	CvContinent()
	{
		m_uiFertility = 0;
		m_iRegionsAssigned = 0;
		m_uiFertilityNextRegion = 0;
		m_iAreaID = FFreeList::INVALID_INDEX;
	}

	CvContinent(const CvContinent& source)
	{
		m_uiFertility = source.m_uiFertility;
		m_iRegionsAssigned = source.m_iRegionsAssigned;
		m_uiFertilityNextRegion = source.m_uiFertilityNextRegion;
		m_iAreaID = source.m_iAreaID;
	}

#if defined(LEKMOD_MACOS)
	bool operator<(const CvContinent& continent) const
#else
	bool operator<(const CvContinent& continent)
#endif
	{
		return (m_uiFertilityNextRegion > continent.m_uiFertilityNextRegion);
	}

	void AddRegion()
	{
		m_iRegionsAssigned++;
		m_uiFertilityNextRegion = m_uiFertility / (m_iRegionsAssigned + 1);
	}

	int GetNumRegions() const
	{
		return m_iRegionsAssigned;
	}

	void SetFertility(int uiFertility)
	{
		m_uiFertility = uiFertility;
		m_uiFertilityNextRegion = uiFertility;
	}

	int GetFertility() const
	{
		return m_uiFertility;
	}

	void SetArea(int iAreaID)
	{
		m_iAreaID = iAreaID;
	}

	int GetArea() const
	{
		return m_iAreaID;
	}

private:
	int m_uiFertility;
	int m_uiFertilityNextRegion;
	int m_iRegionsAssigned;
	int m_iAreaID;
};












class CvStartRegion
{
public:
	CvStartRegion()
	{
		m_uiFertility = 0;
		m_iAreaID = FFreeList::INVALID_INDEX;
		m_bLargeEnoughForMajorCiv = true;
		m_iNumCivsPlaced = 0;
	}

	CvStartRegion(const CvStartRegion& source)
	{
		m_Boundaries = source.m_Boundaries;
		m_uiFertility = source.m_uiFertility;
		m_iAreaID = source.m_iAreaID;
		m_bLargeEnoughForMajorCiv = source.m_bLargeEnoughForMajorCiv;
		m_iNumCivsPlaced = source.m_iNumCivsPlaced;
	}

#if defined(LEKMOD_MACOS)
	bool operator<(const CvStartRegion& continent) const
#else
	bool operator<(const CvStartRegion& continent)
#endif
	{
		return (m_uiFertility / (m_iNumCivsPlaced + 1) >
		        continent.m_uiFertility / (continent.m_iNumCivsPlaced + 1));
	}

	bool Contains(int iX, int iY)
	{
		if(iX > m_Boundaries.m_iEastEdge ||
		        iX < m_Boundaries.m_iWestEdge ||
		        iY > m_Boundaries.m_iNorthEdge ||
		        iY < m_Boundaries.m_iSouthEdge)
		{
			return false;
		}
		return true;
	}

	CvAreaBoundaries m_Boundaries;
	int m_uiFertility;
	int m_iAreaID;
	bool m_bLargeEnoughForMajorCiv;
	int m_iNumCivsPlaced;
};










class CvPlayerStartRank
{
public:
	CvPlayerStartRank()
	{
		m_iPlayerID = NO_PLAYER;
		m_iRank = 0;
	}

	CvPlayerStartRank(const CvPlayerStartRank& startRankObj)
	{
		m_iPlayerID = startRankObj.m_iPlayerID;
		m_iRank = startRankObj.m_iRank;
	}

#if defined(LEKMOD_MACOS)
	bool operator<(const CvPlayerStartRank& startRankObj) const
#else
	bool operator<(const CvPlayerStartRank& startRankObj)
#endif
	{
		return (m_iRank < startRankObj.m_iRank);
	}

	int m_iPlayerID;
	int m_iRank;
};










class CvStartPositioner
{
public:
	CvStartPositioner(void);
	~CvStartPositioner(void);

	virtual void Init(CvSiteEvaluatorForStart* pSiteEvaluator);

	void DivideMapIntoRegions(int iNumRegions);
	void ComputeFoundValues();
	void RankPlayerStartOrder();
	void AssignStartingLocations();
	int GetRegion(int iX, int iY);

private:
	void DivideContinentIntoRegions(CvContinent continent);
	void ComputeTileFertilityValues();
	void SubdivideRegion(CvStartRegion region, int numDivisions);
	void ChopIntoTwoRegions(bool bTaller, CvStartRegion* region, CvStartRegion* secondRegion, int iChopPercent);
	void ChopIntoThreeRegions(bool bTaller, CvStartRegion* region, CvStartRegion* secondRegion, CvStartRegion* thirdRegion);
	int ComputeRowFertility(int iAreaID, int xMin, int xMax, int yMin, int yMax);
	bool AddCivToRegion(int iPlayerIndex, CvStartRegion region, bool bRelaxFoodReq);
	bool TooCloseToAnotherCiv(CvPlot* pPlot);
	bool MeetsFoodRequirement(CvPlot* pPlot, TeamTypes eTeam, int iFoodRequirement);
	int StartingPlotRange() const;


	void LogStartPositionMessage(CvString strMsg);


	CvSiteEvaluatorForStart* m_pSiteEvaluator;


	vector<CvContinent> m_ContinentVector;
	vector<CvStartRegion> m_StartRegionVector;
	vector<CvPlayerStartRank> m_PlayerOrder;
	vector<CvPlayerStartRank>::iterator m_PlayerOrderIter;
	int m_iRequiredSeparation;
	int m_iBestFoundValueOnMap;
};

#endif
