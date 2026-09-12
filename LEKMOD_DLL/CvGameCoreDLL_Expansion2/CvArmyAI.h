






#pragma once



#ifndef CIV5_ARMYAI_H
#define CIV5_ARMYAI_H

class CvPlot;
class CvArea;
class CvAStarNode;
class CvUnit;
class CvMultiUnitFormationInfo;
class CvAIOperation;

#define SAFE_ESTIMATE_NUM_MULTIUNITFORMATION_ENTRIES 20

#define ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT -1
#define ARMYSLOT_NOT_INCLUDING_IN_OPERATION -2

#define ARMY_NO_UNIT -1

class CvArmyFormationSlot
{
public:
	CvArmyFormationSlot()
	{
		m_iUnitID = ARMY_NO_UNIT;
		m_iEstimatedTurnAtCheckpoint = ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT;
		m_bStartedOnOperation = false;
	};

	int GetUnitID()
	{
		return m_iUnitID;
	};
	void SetUnitID(int iValue)
	{
		m_iUnitID = iValue;
	};
	int GetTurnAtCheckpoint()
	{
		return m_iEstimatedTurnAtCheckpoint;
	};
	void SetTurnAtCheckpoint(int iValue)
	{
		m_iEstimatedTurnAtCheckpoint = iValue;
	};
	bool HasStartedOnOperation()
	{
		return m_bStartedOnOperation;
	};
	void SetStartedOnOperation(bool bValue)
	{
		m_bStartedOnOperation = bValue;
	};

	int m_iUnitID;
	int m_iEstimatedTurnAtCheckpoint;
#ifdef AUI_WARNING_FIXES
	bool m_bStartedOnOperation;
#else
	int m_bStartedOnOperation;
#endif
};

enum ArmyAIState
{
    NO_ARMYAISTATE = -1,
    ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE,
    ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP,
    ARMYAISTATE_MOVING_TO_DESTINATION,
    ARMYAISTATE_AT_DESTINATION,
};











class CvArmyAI
{

public:

	CvArmyAI();
	virtual ~CvArmyAI();


	void Init(int iID, PlayerTypes eOwner, int iOperationID);
	void Uninit();
	void Reset(int iID = 0, PlayerTypes eOwner = NO_PLAYER, int iOperationID = -1, bool bConstructorCall = false);
	void Kill();


	virtual void read(FDataStream& kStream);
	virtual void write(FDataStream& kStream) const;



	int GetID();
	void SetID(int iID);

	TeamTypes GetTeam() const;

	ArmyAIState GetArmyAIState() const;
	void SetArmyAIState(ArmyAIState eNewArmyAIState);

	int GetMovementRate();
	CvPlot* GetCenterOfMass(DomainTypes eDomainRequired);
	int GetFurthestUnitDistance(CvPlot* pPlot);

	inline PlayerTypes GetOwner() const
	{
		return m_eOwner;
	}

	inline int GetOperationID() const
	{
		return m_iOperationID;
	}


	int GetFormationIndex() const;
	void SetFormationIndex(int iFormationIndex);
	int GetNumFormationEntries() const;
	int GetNumSlotsFilled() const;
	void SetEstimatedTurn(int iSlotID, int iTurns);
	CvArmyFormationSlot* GetFormationSlot(int iSlotID)
	{
		return &m_FormationEntries[iSlotID];
	};
	int GetTurnAtNextCheckpoint() const;
	void UpdateCheckpointTurns();
	int GetUnitsOfType(MultiunitPositionTypes ePosition) const;
	bool IsAllOceanGoing();


	int GetTotalPower();


	int GetX() const;
	int GetY() const;
	void SetX(int iX);
	void SetY(int iY);
	void SetXY(int iX, int iY);
	CvPlot* Plot() const;
	int GetArea() const;
	DomainTypes GetDomainType() const;
	void SetDomainType(DomainTypes domainType);
	bool AreAllInWater();


	void SetGoalPlot(CvPlot* pGoalPlot);
	CvPlot* GetGoalPlot() const;
	int GetGoalX() const;
	int GetGoalY() const;
	void SetGoalXY(int iX, int iY);


	void AddUnit(int iUnitId, int iSlotNum);
	bool RemoveUnit(int iUnitId);
	bool CanTacticalAIInterruptUnit(int iUnitId) const;
	int GetFirstUnitID();
	int GetNextUnitID();
	UnitHandle GetFirstUnit();
	UnitHandle GetNextUnit();
	UnitHandle GetFirstUnitInDomain(DomainTypes eDomain);


	void DoTurn();
	bool DoDelayedDeath();

protected:
	int m_iID;
	PlayerTypes m_eOwner;
	int m_iOperationID;
	int m_iCurrentX;
	int m_iCurrentY;
	int m_iGoalX;
	int m_iGoalY;
	int m_eDomainType;
	int m_iFormationIndex;
	int m_eAIState;
	FStaticVector<CvArmyFormationSlot, SAFE_ESTIMATE_NUM_MULTIUNITFORMATION_ENTRIES, true, c_eCiv5GameplayDLL, 0> m_FormationEntries;
	FStaticVector<CvArmyFormationSlot, SAFE_ESTIMATE_NUM_MULTIUNITFORMATION_ENTRIES, true, c_eCiv5GameplayDLL, 0>::iterator m_CurUnitIter;
};

FDataStream& operator<<(FDataStream&, const CvArmyAI&);
FDataStream& operator>>(FDataStream&, CvArmyAI&);

FDataStream& operator<<(FDataStream&, const CvArmyFormationSlot&);
FDataStream& operator>>(FDataStream&, CvArmyFormationSlot&);

#endif
