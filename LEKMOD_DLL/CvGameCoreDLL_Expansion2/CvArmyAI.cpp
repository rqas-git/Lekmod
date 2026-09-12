







#include "CvGameCoreDLLPCH.h"
#include "CvGlobals.h"
#include "CvPlayerAI.h"
#include "CvTeam.h"
#include "CvArmyAI.h"
#include "CvUnit.h"
#include "CvGameCoreUtils.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "ICvDLLUserInterface.h"
#include "CvAStar.h"
#include "CvInfos.h"
#include "CvEnumSerialization.h"
#include "FStlContainerSerialization.h"


#include "LintFree.h"




CvArmyAI::CvArmyAI()
{
#ifdef AUI_WARNING_FIXES
	Reset(0, NO_PLAYER, -1, true);
#else
	Reset(0, NO_PLAYER, true);
#endif
}


CvArmyAI::~CvArmyAI()
{
	Uninit();
}


void CvArmyAI::Init(int iID, PlayerTypes eOwner, int iOperationID)
{


	Reset(iID, eOwner, iOperationID);
}


void CvArmyAI::Uninit()
{
	m_FormationEntries.clear();
}


void CvArmyAI::Reset(int iID, PlayerTypes eOwner, int iOperationID, bool                       )
{


	Uninit();

	m_iID = iID;
	m_eOwner = eOwner;
	m_iOperationID = iOperationID;
	m_iCurrentX = -1;
	m_iCurrentY = -1;
	m_iGoalX = -1;
	m_iGoalY = -1;
	m_eDomainType = DOMAIN_LAND;
	m_iFormationIndex = NO_MUFORMATION;
	m_eAIState = NO_ARMYAISTATE;

	m_FormationEntries.clear();
}


void CvArmyAI::Kill()
{
	CvAssert(GetOwner() != NO_PLAYER);
	CvAssertMsg(GetID() != FFreeList::INVALID_INDEX, "GetID() is not expected to be equal with FFreeList::INVALID_INDEX");

	int iUnitID;
	iUnitID = GetFirstUnitID();

	while(iUnitID != ARMY_NO_UNIT)
	{
		UnitHandle pThisUnit = GET_PLAYER(GetOwner()).getUnit(iUnitID);
		if(pThisUnit)
		{
			pThisUnit->setArmyID(FFreeList::INVALID_INDEX);
		}
		iUnitID = GetNextUnitID();
	}

	m_FormationEntries.clear();
}


void CvArmyAI::read(FDataStream& kStream)
{

	Reset();


	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_iID;
	kStream >> m_eOwner;
	kStream >> m_iCurrentX;
	kStream >> m_iCurrentY;
	kStream >> m_iGoalX;
	kStream >> m_iGoalY;
	kStream >> m_eDomainType;
	kStream >> m_iFormationIndex;
	kStream >> m_eAIState;
	kStream >> m_iOperationID;

	int iEntriesToRead;
	kStream >> iEntriesToRead;
	for(int iI = 0; iI < iEntriesToRead; iI++)
	{
		CvArmyFormationSlot slot;
		kStream >> slot;
		m_FormationEntries.push_back(slot);
	}
}


void CvArmyAI::write(FDataStream& kStream) const
{

	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << m_iID;
	kStream << m_eOwner;
	kStream << m_iCurrentX;
	kStream << m_iCurrentY;
	kStream << m_iGoalX;
	kStream << m_iGoalY;
	kStream << m_eDomainType;
	kStream << m_iFormationIndex;
	kStream << m_eAIState;
	kStream << m_iOperationID;

	kStream << (int)m_FormationEntries.size();
	for(uint ui = 0; ui < m_FormationEntries.size(); ui++)
	{
		kStream << m_FormationEntries[ui];
	}
}




int CvArmyAI::GetID()
{
	return m_iID;
}


void CvArmyAI::SetID(int iID)
{
	m_iID = iID;
}


TeamTypes CvArmyAI::GetTeam() const
{
	if(GetOwner() != NO_PLAYER)
	{
		return GET_PLAYER(GetOwner()).getTeam();
	}

	return NO_TEAM;
}


ArmyAIState CvArmyAI::GetArmyAIState() const
{
	return (ArmyAIState) m_eAIState;
}


void CvArmyAI::SetArmyAIState(ArmyAIState eNewArmyAIState)
{
	m_eAIState = (int) eNewArmyAIState;
}


int CvArmyAI::GetMovementRate()
{
	int iMovementAverage = 2;
	int iNumUnits = 0;
	int iTotalMovementAllowance = 0;
	UnitHandle pUnit;

	pUnit = GetFirstUnit();
	while(pUnit)
	{
		iNumUnits++;
		iTotalMovementAllowance += pUnit->baseMoves();
		pUnit = GetNextUnit();
	}

	if(iNumUnits > 0)
	{
		iMovementAverage = (iTotalMovementAllowance + (iNumUnits / 2)) / iNumUnits;
	}

	return iMovementAverage;
}


CvPlot* CvArmyAI::GetCenterOfMass(DomainTypes eDomainRequired)
{
	CvPlot* pRtnValue = NULL;
	int iTotalX = 0;
	int iTotalY = 0;
	int iNumUnits = 0;
	UnitHandle pUnit;
	int iReferenceUnitX = -1;
	int iWorldWidth = GC.getMap().getGridWidth();

	pUnit = GetFirstUnit();
	if(pUnit)
	{
		iReferenceUnitX = pUnit->getX();
	}

	while(pUnit)
	{
		int iUnitX = pUnit->getX();

		bool bWorldWrapAdjust = false;
		int iDiff = iUnitX - iReferenceUnitX;
		if(abs(iDiff) > (iWorldWidth / 2))
		{
			bWorldWrapAdjust = true;
		}

		if(bWorldWrapAdjust)
		{
			iTotalX += iUnitX + iWorldWidth;
		}
		else
		{
			iTotalX += iUnitX;
		}
		iTotalY += pUnit->getY();
		iNumUnits++;
		pUnit = GetNextUnit();
	}

	if(iNumUnits > 0)
	{
		int iAverageX = (iTotalX + (iNumUnits / 2)) / iNumUnits;
		if(iAverageX >= iWorldWidth)
		{
			iAverageX = iAverageX - iWorldWidth;
		}
		int iAverageY = (iTotalY + (iNumUnits / 2)) / iNumUnits;
		pRtnValue = GC.getMap().plot(iAverageX, iAverageY);
	}


	if (eDomainRequired != NO_DOMAIN && pRtnValue)
	{
		if (pRtnValue->isWater() && eDomainRequired == DOMAIN_LAND || !pRtnValue->isWater() && eDomainRequired == DOMAIN_SEA)
		{

			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				CvPlot *pLoopPlot = plotDirection(pRtnValue->getX(), pRtnValue->getY(), ((DirectionTypes)iI));
				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->isWater() && eDomainRequired == DOMAIN_SEA || !pLoopPlot->isWater() && eDomainRequired == DOMAIN_LAND)
					{
						return pLoopPlot;
					}
				}
			}


#ifdef AUI_HEXSPACE_DX_LOOPS
			int iMaxDX, iDX;
			CvPlot* pLoopPlot;
			for (int iDY = -2; iDY <= 2; iDY++)
			{
				iMaxDX = 2 - MAX(0, iDY);
				for (iDX = -2 - MIN(0, iDY); iDX <= iMaxDX; iDX++)
				{

					pLoopPlot = plotXY(pRtnValue->getX(), pRtnValue->getY(), iDX, iDY);
					if (pLoopPlot)
					{
						if (hexDistance(iDX, iDY) == 2)
#else
			for (int iDX = -2; iDX <= 2; iDX++)
			{
				for (int iDY = -2; iDY <= 2; iDY++)
				{
					CvPlot *pLoopPlot = plotXYWithRangeCheck(pRtnValue->getX(), pRtnValue->getY(), iDX, iDY, 2);
					if (pLoopPlot)
					{
#ifdef AUI_FIX_HEX_DISTANCE_INSTEAD_OF_PLOT_DISTANCE
						if (hexDistance(iDX, iDY) == 2)
#else
						if (plotDistance(pRtnValue->getX(), pRtnValue->getY(), pLoopPlot->getX(), pLoopPlot->getY()) == 2)
#endif
#endif
						{
							if (pLoopPlot->isWater() && eDomainRequired == DOMAIN_SEA || !pLoopPlot->isWater() && eDomainRequired == DOMAIN_LAND)
							{
								return pLoopPlot;
							}
						}
					}
				}
			}


			pUnit = GetFirstUnit();
			pRtnValue = pUnit->plot();
		}
	}

	return pRtnValue;
}


int CvArmyAI::GetFurthestUnitDistance(CvPlot* pPlot)
{
	int iLargestDistance = 0;
	UnitHandle pUnit;
	int iNewDistance;

	pUnit = GetFirstUnit();
	while(pUnit)
	{
		iNewDistance = plotDistance(pUnit->getX(), pUnit->getY(), pPlot->getX(), pPlot->getY());
		if(iNewDistance > iLargestDistance)
		{
			iLargestDistance = iNewDistance;
		}
		pUnit = GetNextUnit();
	}
	return iLargestDistance;
}




int CvArmyAI::GetFormationIndex() const
{
	return m_iFormationIndex;
}


void CvArmyAI::SetFormationIndex(int iFormationIndex)
{
	CvArmyFormationSlot slot;

	if(m_iFormationIndex != iFormationIndex)
	{
		m_iFormationIndex = iFormationIndex;

		CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(m_iFormationIndex);
		if(thisFormation)
		{
			int iNumSlots = thisFormation->getNumFormationSlotEntries();


			m_FormationEntries.clear();
			for(int iI = 0; iI < iNumSlots; iI++)
			{
				slot.SetUnitID(ARMY_NO_UNIT);
				slot.SetTurnAtCheckpoint(ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT);
				m_FormationEntries.push_back(slot);
			}
		}
	}
}


int CvArmyAI::GetNumFormationEntries() const
{
	return m_FormationEntries.size();
}


int CvArmyAI::GetNumSlotsFilled() const
{
	int iRtnValue = 0;

	for(unsigned int iI = 0; iI < m_FormationEntries.size(); iI++)
	{
		if(m_FormationEntries[iI].m_iUnitID != ARMY_NO_UNIT)
		{
			iRtnValue++;
		}
	}
	return iRtnValue;
}


void CvArmyAI::SetEstimatedTurn(int iSlotID, int iTurns)
{
	int iTurnAtCheckpoint;

	if(iTurns == ARMYSLOT_NOT_INCLUDING_IN_OPERATION || iTurns == ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT)
	{
		iTurnAtCheckpoint = iTurns;
	}
	else
	{
		iTurnAtCheckpoint = GC.getGame().getGameTurn() + iTurns;
	}

	m_FormationEntries[iSlotID].SetTurnAtCheckpoint(iTurnAtCheckpoint);
}


int CvArmyAI::GetTurnAtNextCheckpoint() const
{
	int iRtnValue = ARMYSLOT_NOT_INCLUDING_IN_OPERATION;

	for(unsigned int iI = 0; iI < m_FormationEntries.size(); iI++)
	{
		if(m_FormationEntries[iI].m_iEstimatedTurnAtCheckpoint == ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT)
		{
			return ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT;
		}
		else if(m_FormationEntries[iI].m_iEstimatedTurnAtCheckpoint > iRtnValue)
		{
			iRtnValue = m_FormationEntries[iI].m_iEstimatedTurnAtCheckpoint;
		}
	}

	return iRtnValue;
}


void CvArmyAI::UpdateCheckpointTurns()
{
	for(unsigned int iI = 0; iI < m_FormationEntries.size(); iI++)
	{

		if(m_FormationEntries[iI].GetUnitID() != ARMY_NO_UNIT)
		{
			CvUnit* pUnit = GET_PLAYER(m_eOwner).getUnit(m_FormationEntries[iI].GetUnitID());
			CvPlot* pMusterPlot = GC.getMap().plot(GetX(), GetY());
			if(pUnit && pMusterPlot)
			{
				int iTurnsToReachCheckpoint = TurnsToReachTarget(pUnit, pMusterPlot, true                , true, true);
				if(iTurnsToReachCheckpoint < MAX_INT)
				{
					SetEstimatedTurn(iI, iTurnsToReachCheckpoint);
				}
			}
		}
	}
}


int CvArmyAI::GetUnitsOfType(MultiunitPositionTypes ePosition) const
{
	int iRtnValue = 0;

	for(unsigned int iI = 0; iI < m_FormationEntries.size(); iI++)
	{
		if(m_FormationEntries[iI].m_iUnitID != ARMY_NO_UNIT)
		{
			CvUnit* pUnit = GET_PLAYER(m_eOwner).getUnit(m_FormationEntries[iI].m_iUnitID);
			if(pUnit->getMoves() > 0)
			{
				CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(m_iFormationIndex);
				if(thisFormation)
				{
					const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(iI);
					if(thisSlotEntry.m_ePositionType == ePosition)
					{
						iRtnValue++;
					}
				}
			}
		}
	}
	return iRtnValue;
}


bool CvArmyAI::IsAllOceanGoing()
{
	UnitHandle pUnit;

	pUnit = GetFirstUnit();
	while(pUnit)
	{
		if(pUnit->getDomainType() != DOMAIN_SEA && !pUnit->IsHasEmbarkAbility())
		{
			return false;
		}


		if(pUnit->isTerrainImpassable(TERRAIN_OCEAN))
		{
			return false;
		}

		pUnit = GetNextUnit();
	}

	return true;
}



int CvArmyAI::GetTotalPower()
{
	int iRtnValue = 0;
	int iUnitID;
	iUnitID = GetFirstUnitID();

	while(iUnitID != ARMY_NO_UNIT)
	{
		UnitHandle pThisUnit = GET_PLAYER(GetOwner()).getUnit(iUnitID);
		if(pThisUnit)
		{
			iRtnValue += pThisUnit->GetPower();
		}
		iUnitID = GetNextUnitID();
	}

	return iRtnValue;
}




int CvArmyAI::GetX() const
{
	return m_iCurrentX;
}


int CvArmyAI::GetY() const
{
	return m_iCurrentY;
}


void CvArmyAI::SetX(int iX)
{
	m_iCurrentX = iX;
}


void CvArmyAI::SetY(int iY)
{
	m_iCurrentY = iY;
}


void CvArmyAI::SetXY(int iX, int iY)
{
	m_iCurrentX = iX;
	m_iCurrentY = iY;
}


CvPlot* CvArmyAI::Plot() const
{
	return GC.getMap().plotCheckInvalid(m_iCurrentX, m_iCurrentY);
}


int CvArmyAI::GetArea() const
{

	CvPlot* pPlot = GC.getMap().plotCheckInvalid(m_iCurrentX, m_iCurrentY);
	if(pPlot != NULL)
	{
		return pPlot->getArea();
	}
	else
	{

		return FFreeList::INVALID_INDEX;
	}
}


DomainTypes CvArmyAI::GetDomainType() const
{
	return (DomainTypes) m_eDomainType;
}


void CvArmyAI::SetDomainType(DomainTypes domainType)
{
	m_eDomainType = domainType;
}


bool CvArmyAI::AreAllInWater()
{
	UnitHandle pUnit;

	pUnit = GetFirstUnit();
	while(pUnit)
	{
		if(!pUnit->plot()->isWater())
		{
			return false;
		}
		pUnit = GetNextUnit();
	}

	return true;
}




CvPlot* CvArmyAI::GetGoalPlot() const
{
	return GC.getMap().plotCheckInvalid(m_iGoalX, m_iGoalY);
}


void CvArmyAI::SetGoalPlot(CvPlot* pGoalPlot)
{
	CvAssertMsg(pGoalPlot, "Setting army goal to a NULL plot - please show Ed and send save.");

	if(pGoalPlot)
	{
		m_iGoalX = pGoalPlot->getX();
		m_iGoalY = pGoalPlot->getY();
	}
	else
	{
		m_iGoalX = -1;
		m_iGoalY = -1;
	}
}


int CvArmyAI::GetGoalX() const
{
	return m_iGoalX;
}


int CvArmyAI::GetGoalY() const
{
	return m_iGoalY;
}


void CvArmyAI::SetGoalXY(int iX, int iY)
{
	m_iGoalX = iX;
	m_iGoalY = iY;
}




void CvArmyAI::AddUnit(int iUnitID, int iSlotNum)
{
	CvAssertMsg(iUnitID != ARMY_NO_UNIT,"Expect unit to be non-NULL");

	CvPlayer& thisPlayer = GET_PLAYER(m_eOwner);
	UnitHandle pThisUnit = thisPlayer.getUnit(iUnitID);


	thisPlayer.removeFromArmy(pThisUnit->getArmyID(), GetID());

	m_FormationEntries[iSlotNum].SetUnitID(iUnitID);
	pThisUnit->setArmyID(GetID());


	CvPlot* pMusterPlot = GC.getMap().plot(GetX(), GetY());
	if(pMusterPlot)
	{
		int iTurnsToReachCheckpoint = TurnsToReachTarget(pThisUnit, pMusterPlot, true                , true, true);
		if(iTurnsToReachCheckpoint < MAX_INT)
		{
			SetEstimatedTurn(iSlotNum, iTurnsToReachCheckpoint);
		}
	}
}


bool CvArmyAI::RemoveUnit(int iUnitToRemoveID)
{
	bool bWasOneOrMoreRemoved = false;
	CvArmyFormationSlot slot;

	for(int iI = 0; iI < (int)m_FormationEntries.size(); iI++)
	{
		slot = m_FormationEntries[iI];
		if(slot.GetUnitID() == iUnitToRemoveID)
		{
			UnitHandle pThisUnit = GET_PLAYER(GetOwner()).getUnit(iUnitToRemoveID);
			if(pThisUnit)
			{

				pThisUnit->setArmyID(FFreeList::INVALID_INDEX);
				m_FormationEntries[iI].SetUnitID(ARMY_NO_UNIT);
				bWasOneOrMoreRemoved = true;


				CvAIOperation* pThisOperation = GET_PLAYER(GetOwner()).getAIOperation(m_iOperationID);
				if(pThisOperation)
				{
					pThisOperation->UnitWasRemoved(GetID(), iI);
				}
			}
		}
	}

	return bWasOneOrMoreRemoved;
}


bool CvArmyAI::CanTacticalAIInterruptUnit(int              ) const
{

	if(m_eAIState == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE ||
	        m_eAIState == ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP)
	{
		return true;
	}

	if(m_eOwner >=0 && m_eOwner < MAX_PLAYERS)
	{
		CvAIOperation* op = GET_PLAYER(m_eOwner).getAIOperation(m_iOperationID);
		if(op)
		{
			return op->CanTacticalAIInterruptOperation();
		}
	}
	return false;
}


int CvArmyAI::GetFirstUnitID()
{
	m_CurUnitIter = m_FormationEntries.begin();

	if(m_CurUnitIter == m_FormationEntries.end())
	{
		return ARMY_NO_UNIT;
	}
	else
	{

		while(m_CurUnitIter != m_FormationEntries.end())
		{
			if(m_CurUnitIter->GetUnitID() != ARMY_NO_UNIT)
			{
				return m_CurUnitIter->GetUnitID();
			}
			++m_CurUnitIter;
		}

		return ARMY_NO_UNIT;
	}
}


int CvArmyAI::GetNextUnitID()
{
	if(m_CurUnitIter != m_FormationEntries.end())
	{
		++m_CurUnitIter;

		while(m_CurUnitIter != m_FormationEntries.end())
		{
			if(m_CurUnitIter->GetUnitID() != ARMY_NO_UNIT)
			{
				return m_CurUnitIter->GetUnitID();
			}
			++m_CurUnitIter;
		}

		return ARMY_NO_UNIT;
	}
	return ARMY_NO_UNIT;
}


UnitHandle CvArmyAI::GetFirstUnit()
{
	UnitHandle pRtnValue;

	int iUnitID = GetFirstUnitID();
	if(iUnitID != ARMY_NO_UNIT)
	{
		pRtnValue = UnitHandle(GET_PLAYER(m_eOwner).getUnit(iUnitID));

		FAssertMsg(pRtnValue, "Bogus unit in army - tell Ed");
	}

	return pRtnValue;
}


UnitHandle CvArmyAI::GetNextUnit()
{
	UnitHandle pRtnValue;

	int iUnitID = GetNextUnitID();
	if(iUnitID != ARMY_NO_UNIT)
	{
		pRtnValue = UnitHandle(GET_PLAYER(m_eOwner).getUnit(iUnitID));

		FAssertMsg(pRtnValue, "Bogus unit in army - tell Ed");
	}

	return pRtnValue;
}


UnitHandle CvArmyAI::GetFirstUnitInDomain(DomainTypes eDomain)
{
	UnitHandle pUnit, pCurrentUnit;

	pCurrentUnit = GetFirstUnit();
	while(pCurrentUnit)
	{
		if(pCurrentUnit->plot()->isWater() && eDomain == DOMAIN_SEA || !pCurrentUnit->plot()->isWater() && eDomain == DOMAIN_LAND)
		{
			return pCurrentUnit;
		}
		pCurrentUnit = GetNextUnit();
	}

	return pUnit;
}




void CvArmyAI::DoTurn()
{

	DoDelayedDeath();
}


bool CvArmyAI::DoDelayedDeath()
{
	if(GetNumSlotsFilled() == 0 && m_eAIState != ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE)
	{
		Kill();
		return true;
	}

	return false;
}

FDataStream& operator<<(FDataStream& saveTo, const CvArmyAI& readFrom)
{
	readFrom.write(saveTo);
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, CvArmyAI& writeTo)
{
	writeTo.read(loadFrom);
	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const CvArmyFormationSlot& readFrom)
{
	saveTo << readFrom.m_iUnitID;
	saveTo << readFrom.m_iEstimatedTurnAtCheckpoint;
	saveTo << readFrom.m_bStartedOnOperation;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, CvArmyFormationSlot& writeTo)
{
	loadFrom >> writeTo.m_iUnitID;
	loadFrom >> writeTo.m_iEstimatedTurnAtCheckpoint;
	loadFrom >> writeTo.m_bStartedOnOperation;
	return loadFrom;
}
