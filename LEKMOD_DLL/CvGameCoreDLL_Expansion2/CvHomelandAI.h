






#pragma once

#ifndef CIV5_HOMELAND_AI_H
#define CIV5_HOMELAND_AI_H

#define UPGRADE_THIS_TURN_PRIORITY_BOOST 1000
#define UPGRADE_IN_TERRITORY_PRIORITY_BOOST 500

enum AIHomelandTargetType
{
    AI_HOMELAND_TARGET_NONE,
    AI_HOMELAND_TARGET_CITY,
    AI_HOMELAND_TARGET_SENTRY_POINT,
    AI_HOMELAND_TARGET_FORT,
    AI_HOMELAND_TARGET_NAVAL_RESOURCE,
    AI_HOMELAND_TARGET_HOME_ROAD,
    AI_HOMELAND_TARGET_ANCIENT_RUIN,
	AI_HOMELAND_TARGET_ANTIQUITY_SITE,
};


class CvHomelandMove
{
public:
	CvHomelandMove()
	{
		m_eMoveType = AI_HOMELAND_MOVE_NONE;
		m_iPriority = 0;
	}

	bool operator<(const CvHomelandMove& move) const
	{
		return (m_iPriority > move.m_iPriority);
	}

	AIHomelandMove m_eMoveType;
	int m_iPriority;
};


class CvHomelandUnit
{
public:
	CvHomelandUnit();

	bool operator<(const CvHomelandUnit& unit) const
	{
		return (GetMovesToTarget() < unit.GetMovesToTarget());
	}


	void SetID(int iID)
	{
		m_iID = iID;
	};
	int GetID() const
	{
		return m_iID;
	};
	void SetMovesToTarget(int iMoves)
	{
		m_iMovesToTarget = iMoves;
	};
	int GetMovesToTarget() const
	{
		return m_iMovesToTarget;
	};
	void SetTarget(CvPlot* pPlot)
	{
		m_pTarget = pPlot;
	};
	CvPlot* GetTarget() const
	{
		return m_pTarget;
	};



	int GetAuxIntData()
	{
		return m_iAuxData;
	}
	void SetAuxIntData(int iAuxData)
	{
		m_iAuxData = iAuxData;
	}

private:
	int m_iID;
	int m_iAuxData;
	int m_iMovesToTarget;
	CvPlot* m_pTarget;
};










class CvHomelandTarget
{
public:

	CvHomelandTarget()
		: m_eTargetType(AI_HOMELAND_TARGET_CITY)
		, m_iTargetX(-1)
		, m_iTargetY(-1)
		, m_pAuxData(NULL)
		, m_iAuxData(0)
	{
	};

	bool operator<(const CvHomelandTarget& target) const
	{
		return (m_iAuxData > target.m_iAuxData);
	}
	inline AIHomelandTargetType GetTargetType()
	{
		return m_eTargetType;
	}
	inline void SetTargetType(AIHomelandTargetType eTargetType)
	{
		m_eTargetType = eTargetType;
	}
	inline int GetTargetX()
	{
		return m_iTargetX;
	}
	inline void SetTargetX(int iValue)
	{
		m_iTargetX = iValue;
	}
	inline int GetTargetY()
	{
		return m_iTargetY;
	}
	inline void SetTargetY(int iValue)
	{
		m_iTargetY = iValue;
	}



	inline void* GetAuxData()
	{
		return m_pAuxData;
	}
	inline void SetAuxData(void* pAuxData)
	{
		m_pAuxData = pAuxData;
	}





	inline int GetAuxIntData()
	{
		return m_iAuxData;
	}
	inline void SetAuxIntData(int iAuxData)
	{
		m_iAuxData = iAuxData;
	}

private:
	AIHomelandTargetType m_eTargetType;
	int m_iTargetX;
	int m_iTargetY;
	void* m_pAuxData;
	int m_iAuxData;
};









class CvHomelandAI
{
public:
	CvHomelandAI(void);
	~CvHomelandAI(void);
	void Init(CvPlayer* pPlayer);
	void Uninit();
	void Reset();


	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);


	void RecruitUnits();
	void FindAutomatedUnits();
	void DoTurn();
	void Update();


	bool IsAnyValidExploreMoves(const CvUnit* pUnit) const;
	bool ExecuteSpecialExploreMove(CvUnit* pUnit, CvPlot* pPlot);


	void LogHomelandMessage(CvString& strMsg);
	void LogPatrolMessage(CvString& strMsg, CvUnit* pPatrolUnit);

private:

	typedef FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL > MoveUnitsArray;


	void EstablishHomelandPriorities();
	void FindHomelandTargets();
	void AssignHomelandMoves();


	void PlotExplorerMoves();
	void PlotExplorerSeaMoves();
	void PlotFirstTurnSettlerMoves();
	void PlotGarrisonMoves(bool bCityStateOnly = false);
	void PlotHealMoves();
	void PlotMovesToSafety();
	void PlotMobileReserveMoves();
	void PlotSentryMoves();
	void PlotWorkerMoves();
	void PlotWorkerSeaMoves();
	void PlotPatrolMoves();
	void PlotUpgradeMoves();
	void PlotAncientRuinMoves();
	void PlotWriterMoves();
	void PlotArtistMoves();
	void PlotMusicianMoves();
	void PlotScientistMoves();
	void PlotEngineerMoves();
	void PlotGeneralMoves();
	void PlotMerchantMoves();
	void PlotProphetMoves();
	void PlotAdmiralMoves();
	void PlotMissionaryMoves();
	void PlotInquisitorMoves();
	void PlotSSPartAdds();
	void PlotSSPartMoves();
	void PlotTreasureMoves();
	void PlotAircraftMoves();
	void PlotTradeUnitMoves();
	void PlotArchaeologistMoves();
	void PlotAirliftMoves();
	void ReviewUnassignedUnits();


	void ExecuteFirstTurnSettlerMoves();
	void ExecuteExplorerMoves();
	void ExecuteWorkerMoves();
	bool ExecuteWorkerSeaMoves(CvHomelandTarget target, CvPlot* pTarget);
	void ExecuteMovesToSafestPlot();
	void ExecuteHeals();
	void ExecutePatrolMoves();
	void ExecuteMoveToTarget(CvPlot* pTarget);
	void ExecuteWriterMoves();
	void ExecuteArtistMoves();
	void ExecuteMusicianMoves();
	void ExecuteScientistMoves();
	void ExecuteEngineerMoves();
	void ExecuteGeneralMoves();
	void ExecuteMerchantMoves();
	void ExecuteProphetMoves();
	void ExecuteAdmiralMoves();
	void ExecuteMissionaryMoves();
	void ExecuteInquisitorMoves();
	void ExecuteSSPartAdds();
	void ExecuteSSPartMoves();
	void ExecuteTreasureMoves();
	void ExecuteAircraftMoves();
	void ExecuteTradeUnitMoves();
	void ExecuteArchaeologistMoves();


	void EliminateAdjacentSentryPoints();
	void EliminateAdjacentHomelandRoads();
	bool FindWorkersInSameArea(CvPlot* pTarget, BuildTypes eBuild);
	bool FindUnitsForThisMove(AIHomelandMove eMove, bool bFirstTime);
	CvPlot* FindPatrolTarget(CvUnit* pUnit);
	bool GetBestUnitToReachTarget(CvPlot* pTarget, int iMaxTurns);
	bool MoveCivilianToSafety(CvUnit* pUnit, bool bIgnoreUnits = false);
	bool MoveToEmptySpaceNearTarget(CvUnit* pUnit, CvPlot* pTarget, bool bLand=true);
	CvCity* ChooseBestFreeWonderCity(BuildingTypes eWonder, UnitHandle pEngineer);
	CvPlot* FindArchaeologistTarget(CvUnit *pUnit);
	void UnitProcessed(int iID);
	bool ExecuteWorkerMove(CvUnit* pUnit);
	bool ExecuteCultureBlast(CvUnit* pUnit);
	bool ExecuteGoldenAgeMove(CvUnit* pUnit);
	bool IsValidExplorerEndTurnPlot(const CvUnit* pUnit, CvPlot* pPlot) const;
	bool GetClosestUnitByTurnsToTarget(MoveUnitsArray &kMoveUnits, CvPlot* pTarget, int iMaxTurns, CvUnit** ppClosestUnit, int* piClosestTurns);
	void ClearCurrentMoveUnits();
	void ClearCurrentMoveHighPriorityUnits();


	CvString GetLogFileName(CvString& playerName) const;


	CvPlayer* m_pPlayer;
	std::list<int> m_CurrentTurnUnits;

	MoveUnitsArray m_CurrentMoveUnits;
	MoveUnitsArray m_CurrentMoveHighPriorityUnits;

	FStaticVector< CvHomelandMove, 35, true, c_eCiv5GameplayDLL > m_MovePriorityList;
	int m_MovePriorityTurn;

	CvUnit* m_CurrentBestMoveUnit;
	int m_iCurrentBestMoveUnitTurns;

	CvUnit* m_CurrentBestMoveHighPriorityUnit;
	int m_iCurrentBestMoveHighPriorityUnitTurns;


	std::vector<CvHomelandTarget> m_TargetedCities;
	std::vector<CvHomelandTarget> m_TargetedSentryPoints;
	std::vector<CvHomelandTarget> m_TargetedForts;
	std::vector<CvHomelandTarget> m_TargetedNavalResources;
	std::vector<CvHomelandTarget> m_TargetedHomelandRoads;
	std::vector<CvHomelandTarget> m_TargetedAncientRuins;
	std::vector<CvHomelandTarget> m_TargetedAntiquitySites;


	int m_iRandomRange;
	int m_iDefensiveMoveTurns;
	int m_iUpgradeMoveTurns;
	double m_fFlavorDampening;
};

namespace HomelandAIHelpers
{
bool CvHomelandUnitAuxIntSort(CvHomelandUnit obj1, CvHomelandUnit obj2);
bool CvHomelandUnitAuxIntReverseSort(CvHomelandUnit obj1, CvHomelandUnit obj2);
}

#endif
