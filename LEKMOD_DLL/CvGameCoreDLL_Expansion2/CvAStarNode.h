






#pragma once









#ifndef		CVASTARNODE_H
#define		CVASTARNODE_H
#pragma		once

#define ASNL_ADDOPEN		0
#define ASNL_STARTOPEN		1
#define ASNL_DELETEOPEN		2
#define ASNL_ADDCLOSED		3

#define ASNC_INITIALADD		0
#define ASNC_OPENADD_UP		1
#define ASNC_CLOSEDADD_UP	2
#define ASNC_PARENTADD_UP	3
#define ASNC_NEWADD			4

enum CvAStarListType
{
	NO_CVASTARLIST = -1,

	CVASTARLIST_OPEN,
	CVASTARLIST_CLOSED,

	NUM_CVASTARLIST_TYPES
};


#ifndef AUI_ASTAR_MINOR_OPTIMIZATION










struct CvPathNodeCacheData
{
#ifdef AUI_ASTAR_FIX_CAN_ENTER_TERRAIN_NO_DUPLICATE_CALLS
	bool bIsCalculated : 1;
#endif
	bool bPlotVisibleToTeam:1;
	bool bIsMountain:1;
	bool bIsWater:1;
	bool bCanEnterTerrain:1;
	bool bIsRevealedToTeam:1;
	bool bContainsOtherFriendlyTeamCity:1;
	bool bContainsEnemyCity:1;
	bool bContainsVisibleEnemy:1;
	bool bContainsVisibleEnemyDefender:1;
	int	iNumFriendlyUnitsOfType;
};
#endif








class CvAStarNode
{
public:
	CvAStarNode()
	{
		m_iX = -1;
		m_iY = -1;
#ifdef AUI_ASTAR_CACHE_PLOTS_AT_NODES
		m_pPlot = NULL;
#endif
#ifdef AUI_ASTAR_MINOR_OPTIMIZATION
		clear();
#else
		m_iTotalCost = 0;
		m_iKnownCost = 0;
		m_iHeuristicCost = 0;
		m_iNumChildren = 0;
		m_iData1 = 0;
		m_iData2 = 0;

		m_bOnStack = false;

		m_eCvAStarListType = NO_CVASTARLIST;

		m_pParent = NULL;
		m_pNext = NULL;
		m_pPrev = NULL;
		m_pStack = NULL;
#endif
#ifdef AUI_ASTAR_PRECALCULATE_NEIGHBORS_ON_INITIALIZE
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			m_apNeighbors[iI] = NULL;
		}
#endif
	}

	void clear()
	{
		m_iTotalCost = 0;
		m_iKnownCost = 0;
		m_iHeuristicCost = 0;
#ifdef AUI_ASTAR_MINOR_OPTIMIZATION
		bPlotVisibleToTeam = false;
		bIsMountain = false;
		bIsWater = false;
		bCanEnterTerrain = false;
		bIsRevealedToTeam = false;
		bContainsOtherFriendlyTeamCity = false;
		bContainsEnemyCity = false;
		bContainsVisibleEnemy = false;
		bContainsVisibleEnemyDefender = false;
#else
		m_iNumChildren = 0;
#endif
		m_iData1 = 0;
		m_iData2 = 0;

		m_bOnStack = false;

		m_eCvAStarListType = NO_CVASTARLIST;

		m_pParent = NULL;
		m_pNext = NULL;
		m_pPrev = NULL;
		m_pStack = NULL;

#ifdef AUI_ASTAR_FIX_CAN_ENTER_TERRAIN_NO_DUPLICATE_CALLS
#ifdef AUI_ASTAR_MINOR_OPTIMIZATION
		bIsCalculated = false;
#else
		m_kCostCacheData.bIsCalculated = false;
#endif
#endif

		m_apChildren.clear();
	}

	int m_iTotalCost;
	int m_iKnownCost;
	int m_iHeuristicCost;
	int m_iData1;
	int m_iData2;
#ifdef AUI_ASTAR_MINOR_OPTIMIZATION
	int	iNumFriendlyUnitsOfType;
#endif
#ifdef AUI_WARNING_FIXES
	int m_iX, m_iY;
#endif

	CvAStarListType m_eCvAStarListType;

	CvAStarNode* m_pParent;
	CvAStarNode* m_pNext;
	CvAStarNode* m_pPrev;
	CvAStarNode* m_pStack;

#ifdef AUI_ASTAR_MINOR_OPTIMIZATION
	FStaticVector<CvAStarNode*, NUM_DIRECTION_TYPES, true, c_eCiv5GameplayDLL, 0> m_apChildren;
#else
	FStaticVector<CvAStarNode*, 6, true, c_eCiv5GameplayDLL, 0> m_apChildren;
#endif

#ifndef AUI_WARNING_FIXES
	short m_iX, m_iY;
#endif
#ifdef AUI_ASTAR_CACHE_PLOTS_AT_NODES
	CvPlot* m_pPlot;
#endif
#ifndef AUI_ASTAR_MINOR_OPTIMIZATION
	short m_iNumChildren;
#endif
	bool m_bOnStack;
#ifdef AUI_ASTAR_MINOR_OPTIMIZATION
#ifdef AUI_ASTAR_FIX_CAN_ENTER_TERRAIN_NO_DUPLICATE_CALLS
	bool bIsCalculated;
#endif
	bool bPlotVisibleToTeam;
	bool bIsMountain;
	bool bIsWater;
	bool bCanEnterTerrain;
	bool bIsRevealedToTeam;
	bool bContainsOtherFriendlyTeamCity;
	bool bContainsEnemyCity;
	bool bContainsVisibleEnemy;
	bool bContainsVisibleEnemyDefender;
#endif

#ifdef AUI_ASTAR_PRECALCULATE_NEIGHBORS_ON_INITIALIZE

	CvAStarNode* m_apNeighbors[NUM_DIRECTION_TYPES];
#endif

#ifndef AUI_ASTAR_MINOR_OPTIMIZATION
	CvPathNodeCacheData m_kCostCacheData;
#endif
};








class CvPathNode
{
public:
	CvPathNode()
	{
		clear();
	}

	void clear()
	{
		m_iX = -1;
		m_iY = -1;
		m_iData1 = 0;
		m_iData2 = 0;
		m_iFlags = 0;
	}

	enum Flags
	{


		PLOT_INVISIBLE						= (1 << 0),



		PLOT_ADJACENT_INVISIBLE				= (1 << 1)
	};

	int m_iData1;
	int m_iData2;
	int m_iFlags;
#ifdef AUI_WARNING_FIXES
	int m_iX, m_iY;
#else
	short m_iX, m_iY;
#endif

	bool GetFlag(int eFlag) const { return (m_iFlags & eFlag) != 0; }
	void SetFlag(int eFlag) { m_iFlags |= eFlag; }
	void ClearFlag(int eFlag) { m_iFlags = (m_iFlags & ~eFlag); }

	CvPathNode& operator =(const CvAStarNode& rhs)
	{
		m_iX = rhs.m_iX;
		m_iY = rhs.m_iY;
		m_iData1 = rhs.m_iData1;
		m_iData2 = rhs.m_iData2;
		m_iFlags = 0;
		return *this;
	}
};

class CvPathNodeArray : public FFastVector< CvPathNode, true, c_eMPoolTypeContainer >
{
public:

	const CvPathNode* GetTurnDest(int iTurn);
};

#endif
