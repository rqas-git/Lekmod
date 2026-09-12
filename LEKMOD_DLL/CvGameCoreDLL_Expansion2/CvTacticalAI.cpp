






#include "CvGameCoreDLLPCH.h"
#include "FFastVector.h"
#include "CvGameCoreUtils.h"
#include "CvAStar.h"
#include "CvEconomicAI.h"
#include "CvEnumSerialization.h"
#include "CvUnitCombat.h"
#include "cvStopWatch.h"
#include "CvMilitaryAI.h"
#include "CvTypes.h"

#include "LintFree.h"

CvTacticalUnit::CvTacticalUnit() :
	m_iID(0)
	, m_iAttackStrength(0)
	, m_iHealthPercent(0)
	, m_iMovesToTarget(0)
	, m_iExpectedTargetDamage(0)
	, m_iExpectedSelfDamage(0)
{
}





CvTacticalMoveXMLEntry::CvTacticalMoveXMLEntry(void)
{
	m_iPriority = -1;
	m_iOffenseWeight = 0;
#ifdef AUI_WARNING_FIXES
	m_eMoveType = NO_TACTICAL_MOVE;
	m_iDefenseWeight = 0;
	m_bDominanceZoneMove = false;
#else
	m_iOffenseWeight = 0;
#endif
	m_bOperationsCanRecruit = false;
}


CvTacticalMoveXMLEntry::~CvTacticalMoveXMLEntry(void)
{
}


bool CvTacticalMoveXMLEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	const char* szMoveType = kResults.GetText("Type");
	m_eMoveType = (TacticalAIMoveTypes)GC.getInfoTypeForString(szMoveType);

	m_iPriority = kResults.GetInt("Priority");

	m_iOffenseWeight = kResults.GetInt("OffenseFlavorWeight");
	m_iDefenseWeight = kResults.GetInt("DefenseFlavorWeight");

	m_bOperationsCanRecruit = kResults.GetInt("OperationsCanRecruit");
	m_bDominanceZoneMove = kResults.GetInt("DominanceZoneMove");

	return true;
}




CvTacticalMoveXMLEntries::CvTacticalMoveXMLEntries(void)
{

}


CvTacticalMoveXMLEntries::~CvTacticalMoveXMLEntries(void)
{
	DeleteArray();
}


std::vector<CvTacticalMoveXMLEntry*>& CvTacticalMoveXMLEntries::GetTacticalMoveEntries()
{
	return m_paTacticalMoveEntries;
}


int CvTacticalMoveXMLEntries::GetNumTacticalMoves()
{
	return m_paTacticalMoveEntries.size();
}


CvTacticalMoveXMLEntry* CvTacticalMoveXMLEntries::GetEntry(int index)
{
	return m_paTacticalMoveEntries[index];
}


void CvTacticalMoveXMLEntries::DeleteArray()
{
	for(std::vector<CvTacticalMoveXMLEntry*>::iterator it = m_paTacticalMoveEntries.begin(); it != m_paTacticalMoveEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paTacticalMoveEntries.clear();
}





bool CvTacticalTarget::IsTargetStillAlive(PlayerTypes m_eAttackingPlayer)
{
	bool bRtnValue = false;

	AITacticalTargetType eType = GetTargetType();
	if(eType == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT ||
	        eType == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
	        eType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
	{
		CvUnit* pUnit;
		CvPlot* pPlot;
		pPlot = GC.getMap().plot(m_iTargetX, m_iTargetY);
		pUnit = pPlot->getVisibleEnemyDefender(m_eAttackingPlayer);
		if(pUnit != NULL && !pUnit->isDelayedDeath())
		{
			bRtnValue = true;
		}
	}
	else if(eType == AI_TACTICAL_TARGET_CITY)
	{
		CvCity* pCity;
		CvPlot* pPlot;
		pPlot = GC.getMap().plot(m_iTargetX, m_iTargetY);
		pCity = pPlot->getPlotCity();
		if(pCity != NULL && pCity->getOwner() == GetTargetPlayer())
		{
			bRtnValue = true;
		}
	}
	return bRtnValue;
}


bool CvTacticalTarget::IsTargetValidInThisDomain(DomainTypes eDomain)
{
	bool bRtnValue = false;

	switch(GetTargetType())
	{
	case AI_TACTICAL_TARGET_DEFENSIVE_BASTION:
	case AI_TACTICAL_TARGET_BARBARIAN_CAMP:
	case AI_TACTICAL_TARGET_IMPROVEMENT:
	case AI_TACTICAL_TARGET_IMPROVEMENT_TO_DEFEND:
	case AI_TACTICAL_TARGET_ANCIENT_RUINS:
	case AI_TACTICAL_TARGET_TRADE_UNIT_LAND:
	case AI_TACTICAL_TARGET_TRADE_UNIT_LAND_PLOT:
	case AI_TACTICAL_TARGET_CITADEL:
	case AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE:
		bRtnValue = (eDomain == DOMAIN_LAND);
		break;

	case AI_TACTICAL_TARGET_BLOCKADE_RESOURCE_POINT:
	case AI_TACTICAL_TARGET_BOMBARDMENT_ZONE:
	case AI_TACTICAL_TARGET_EMBARKED_CIVILIAN:
	case AI_TACTICAL_TARGET_EMBARKED_MILITARY_UNIT:
	case AI_TACTICAL_TARGET_TRADE_UNIT_SEA:
	case AI_TACTICAL_TARGET_TRADE_UNIT_SEA_PLOT:
		bRtnValue = (eDomain == DOMAIN_SEA);
		break;

	case AI_TACTICAL_TARGET_CITY:
	case AI_TACTICAL_TARGET_CITY_TO_DEFEND:
	case AI_TACTICAL_TARGET_LOW_PRIORITY_CIVILIAN:
	case AI_TACTICAL_TARGET_MEDIUM_PRIORITY_CIVILIAN:
	case AI_TACTICAL_TARGET_HIGH_PRIORITY_CIVILIAN:
	case AI_TACTICAL_TARGET_VERY_HIGH_PRIORITY_CIVILIAN:
	case AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT:
	case AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT:
	case AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT:
		bRtnValue = true;
		break;
	}

	return bRtnValue;
}

FDataStream& operator<<(FDataStream& saveTo, const CvTemporaryZone& readFrom)
{
	uint uiVersion = 1;
	saveTo << uiVersion;

	saveTo << (int)readFrom.GetTargetType();
	saveTo << readFrom.GetLastTurn();
	saveTo << readFrom.GetX();
	saveTo << readFrom.GetY();
	saveTo << (int)readFrom.IsNavalInvasion();
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, CvTemporaryZone& writeTo)
{
	uint uiVersion;
	loadFrom >> uiVersion;

	int iTemp;
	loadFrom >> iTemp;
	writeTo.SetTargetType((AITacticalTargetType)iTemp);
	loadFrom >> iTemp;
	writeTo.SetLastTurn(iTemp);
	loadFrom >> iTemp;
	writeTo.SetX(iTemp);
	loadFrom >> iTemp;
	writeTo.SetY(iTemp);
	loadFrom >> iTemp;
	writeTo.SetNavalInvasion(iTemp != 0);
	return loadFrom;
}






CvTacticalAI::CvTacticalAI(void)
{
	m_iCurrentZoneIndex = -1;
}


CvTacticalAI::~CvTacticalAI(void)
{
	Uninit();
}


void CvTacticalAI::Init(CvPlayer* pPlayer)
{

	m_pPlayer = pPlayer;

	Reset();

	m_pMap = GC.getGame().GetTacticalAnalysisMap();


	m_iRecruitRange = GC.getAI_TACTICAL_RECRUIT_RANGE();
	m_iLandBarbarianRange = GC.getGame().getHandicapInfo().getBarbarianLandTargetRange();
	m_iSeaBarbarianRange = GC.getGame().getHandicapInfo().getBarbarianSeaTargetRange();
	m_iRepositionRange = GC.getAI_TACTICAL_REPOSITION_RANGE();
	m_iDeployRadius = GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE();
	m_iRandomRange = GC.getAI_TACTICAL_MOVE_PRIORITY_RANDOMNESS();
	m_fFlavorDampening = GC.getAI_TACTICAL_FLAVOR_DAMPENING_FOR_MOVE_PRIORITIZATION();


	m_CachedInfoTypes[eTACTICAL_UNASSIGNED] = GC.getInfoTypeForString("TACTICAL_UNASSIGNED");
	m_CachedInfoTypes[eTACTICAL_POSTURE_SIT_AND_BOMBARD] = GC.getInfoTypeForString("TACTICAL_POSTURE_SIT_AND_BOMBARD");
	m_CachedInfoTypes[eTACTICAL_POSTURE_ATTRIT_FROM_RANGE] = GC.getInfoTypeForString("TACTICAL_POSTURE_ATTRIT_FROM_RANGE");
	m_CachedInfoTypes[eTACTICAL_POSTURE_EXPLOIT_FLANKS] = GC.getInfoTypeForString("TACTICAL_POSTURE_EXPLOIT_FLANKS");
	m_CachedInfoTypes[eTACTICAL_POSTURE_STEAMROLL] = GC.getInfoTypeForString("TACTICAL_POSTURE_STEAMROLL");
	m_CachedInfoTypes[eTACTICAL_POSTURE_SURGICAL_CITY_STRIKE] = GC.getInfoTypeForString("TACTICAL_POSTURE_SURGICAL_CITY_STRIKE");
	m_CachedInfoTypes[eTACTICAL_POSTURE_COUNTERATTACK] = GC.getInfoTypeForString("TACTICAL_POSTURE_COUNTERATTACK");
	m_CachedInfoTypes[eTACTICAL_MOVE_NONCOMBATANTS_TO_SAFETY] = GC.getInfoTypeForString("TACTICAL_MOVE_NONCOMBATANTS_TO_SAFETY");
	m_CachedInfoTypes[eTACTICAL_CAPTURE_CITY] = GC.getInfoTypeForString("TACTICAL_CAPTURE_CITY");
	m_CachedInfoTypes[eTACTICAL_DAMAGE_CITY] = GC.getInfoTypeForString("TACTICAL_DAMAGE_CITY");
	m_CachedInfoTypes[eTACTICAL_DESTROY_HIGH_UNIT] = GC.getInfoTypeForString("TACTICAL_DESTROY_HIGH_UNIT");
	m_CachedInfoTypes[eTACTICAL_DESTROY_MEDIUM_UNIT] = GC.getInfoTypeForString("TACTICAL_DESTROY_MEDIUM_UNIT");
	m_CachedInfoTypes[eTACTICAL_DESTROY_LOW_UNIT] = GC.getInfoTypeForString("TACTICAL_DESTROY_LOW_UNIT");
	m_CachedInfoTypes[eTACTICAL_TO_SAFETY] = GC.getInfoTypeForString("TACTICAL_TO_SAFETY");
	m_CachedInfoTypes[eTACTICAL_ATTRIT_HIGH_UNIT] = GC.getInfoTypeForString("TACTICAL_ATTRIT_HIGH_UNIT");
	m_CachedInfoTypes[eTACTICAL_ATTRIT_MEDIUM_UNIT] = GC.getInfoTypeForString("TACTICAL_ATTRIT_MEDIUM_UNIT");
	m_CachedInfoTypes[eTACTICAL_ATTRIT_LOW_UNIT] = GC.getInfoTypeForString("TACTICAL_ATTRIT_LOW_UNIT");
	m_CachedInfoTypes[eTACTICAL_REPOSITION] = GC.getInfoTypeForString("TACTICAL_REPOSITION");
	m_CachedInfoTypes[eTACTICAL_BARBARIAN_CAMP] = GC.getInfoTypeForString("TACTICAL_BARBARIAN_CAMP");
	m_CachedInfoTypes[eTACTICAL_PILLAGE] = GC.getInfoTypeForString("TACTICAL_PILLAGE");
	m_CachedInfoTypes[eTACTICAL_ATTACK_VERY_HIGH_PRIORITY_CIVILIAN] = GC.getInfoTypeForString("TACTICAL_ATTACK_VERY_HIGH_PRIORITY_CIVILIAN");
	m_CachedInfoTypes[eTACTICAL_ATTACK_HIGH_PRIORITY_CIVILIAN] = GC.getInfoTypeForString("TACTICAL_ATTACK_HIGH_PRIORITY_CIVILIAN");
	m_CachedInfoTypes[eTACTICAL_ATTACK_MEDIUM_PRIORITY_CIVILIAN] = GC.getInfoTypeForString("TACTICAL_ATTACK_MEDIUM_PRIORITY_CIVILIAN");
	m_CachedInfoTypes[eTACTICAL_ATTACK_LOW_PRIORITY_CIVILIAN] = GC.getInfoTypeForString("TACTICAL_ATTACK_LOW_PRIORITY_CIVILIAN");
	m_CachedInfoTypes[eTACTICAL_SAFE_BOMBARDS] = GC.getInfoTypeForString("TACTICAL_SAFE_BOMBARDS");
	m_CachedInfoTypes[eTACTICAL_HEAL] = GC.getInfoTypeForString("TACTICAL_HEAL");
	m_CachedInfoTypes[eTACTICAL_ANCIENT_RUINS] = GC.getInfoTypeForString("TACTICAL_ANCIENT_RUINS");
	m_CachedInfoTypes[eTACTICAL_GARRISON_TO_ALLOW_BOMBARD] = GC.getInfoTypeForString("TACTICAL_GARRISON_TO_ALLOW_BOMBARD");
	m_CachedInfoTypes[eTACTICAL_GARRISON_ALREADY_THERE] = GC.getInfoTypeForString("TACTICAL_GARRISON_ALREADY_THERE");
	m_CachedInfoTypes[eTACTICAL_BASTION_ALREADY_THERE] = GC.getInfoTypeForString("TACTICAL_BASTION_ALREADY_THERE");
	m_CachedInfoTypes[eTACTICAL_GUARD_IMPROVEMENT_ALREADY_THERE] = GC.getInfoTypeForString("TACTICAL_GUARD_IMPROVEMENT_ALREADY_THERE");
	m_CachedInfoTypes[eTACTICAL_GARRISON_1_TURN] = GC.getInfoTypeForString("TACTICAL_GARRISON_1_TURN");
	m_CachedInfoTypes[eTACTICAL_BASTION_1_TURN] = GC.getInfoTypeForString("TACTICAL_BASTION_1_TURN");
	m_CachedInfoTypes[eTACTICAL_GUARD_IMPROVEMENT_1_TURN] = GC.getInfoTypeForString("TACTICAL_GUARD_IMPROVEMENT_1_TURN");
	m_CachedInfoTypes[eTACTICAL_AIR_INTERCEPT] = GC.getInfoTypeForString("TACTICAL_AIR_INTERCEPT");
	m_CachedInfoTypes[eTACTICAL_AIR_SWEEP] = GC.getInfoTypeForString("TACTICAL_AIR_SWEEP");
	m_CachedInfoTypes[eTACTICAL_POSTURE_HEDGEHOG] = GC.getInfoTypeForString("TACTICAL_POSTURE_HEDGEHOG");
	m_CachedInfoTypes[eTACTICAL_POSTURE_WITHDRAW] = GC.getInfoTypeForString("TACTICAL_POSTURE_WITHDRAW");
	m_CachedInfoTypes[eTACTICAL_POSTURE_SHORE_BOMBARDMENT] = GC.getInfoTypeForString("TACTICAL_POSTURE_SHORE_BOMBARDMENT");
	m_CachedInfoTypes[eTACTICAL_CLOSE_ON_TARGET] = GC.getInfoTypeForString("TACTICAL_CLOSE_ON_TARGET");
	m_CachedInfoTypes[eTACTICAL_MOVE_OPERATIONS] = GC.getInfoTypeForString("TACTICAL_MOVE_OPERATIONS");
	m_CachedInfoTypes[eTACTICAL_EMERGENCY_PURCHASES] = GC.getInfoTypeForString("TACTICAL_EMERGENCY_PURCHASES");
	m_CachedInfoTypes[eTACTICAL_ESCORT_EMBARKED_UNIT] = GC.getInfoTypeForString("TACTICAL_ESCORT_EMBARKED_UNIT");
	m_CachedInfoTypes[eTACTICAL_PLUNDER_TRADE_UNIT] = GC.getInfoTypeForString("TACTICAL_PLUNDER_TRADE_UNIT");
	m_CachedInfoTypes[eTACTICAL_PARK_ON_TRADE_ROUTE] = GC.getInfoTypeForString("TACTICAL_PARK_ON_TRADE_ROUTE");
	m_CachedInfoTypes[eTACTICAL_DEFENSIVE_AIRLIFT] = GC.getInfoTypeForString("TACTICAL_DEFENSIVE_AIRLIFT");
	m_CachedInfoTypes[eTACTICAL_PILLAGE_CITADEL] = GC.getInfoTypeForString("TACTICAL_PILLAGE_CITADEL");
	m_CachedInfoTypes[eTACTICAL_PILLAGE_RESOURCE] = GC.getInfoTypeForString("TACTICAL_PILLAGE_RESOURCE");
	m_CachedInfoTypes[eTACTICAL_PILLAGE_CITADEL_NEXT_TURN] = GC.getInfoTypeForString("TACTICAL_PILLAGE_CITADEL_NEXT_TURN");
	m_CachedInfoTypes[eTACTICAL_PILLAGE_RESOURCE_NEXT_TURN] = GC.getInfoTypeForString("TACTICAL_PILLAGE_RESOURCE_NEXT_TURN");
	m_CachedInfoTypes[eTACTICAL_PILLAGE_NEXT_TURN] = GC.getInfoTypeForString("TACTICAL_PILLAGE_NEXT_TURN");
	m_CachedInfoTypes[eMUPOSITION_CIVILIAN_SUPPORT] = GC.getInfoTypeForString("MUPOSITION_CIVILIAN_SUPPORT");
	m_CachedInfoTypes[eMUPOSITION_NAVAL_ESCORT] = GC.getInfoTypeForString("MUPOSITION_NAVAL_ESCORT");
	m_CachedInfoTypes[eMUPOSITION_BOMBARD] = GC.getInfoTypeForString("MUPOSITION_BOMBARD");
	m_CachedInfoTypes[eMUPOSITION_FRONT_LINE] = GC.getInfoTypeForString("MUPOSITION_FRONT_LINE");

}


void CvTacticalAI::Uninit()
{
}


void CvTacticalAI::Reset()
{
	m_MovePriorityTurn = -1;
	m_pMap = NULL;
}


void CvTacticalAI::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	int phony = 0;
	kStream >> phony;
	kStream >> m_TempZones;
}


void CvTacticalAI::Write(FDataStream& kStream)
{

	uint uiVersion = 1;
	kStream << uiVersion;
	int phony = 0;
	kStream << phony;
	kStream << m_TempZones;
}


void CvTacticalAI::CommandeerUnits()
{
	UnitHandle pLoopUnit;
	int iLoop;

	m_CurrentTurnUnits.clear();


	for(pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{

		if(pLoopUnit->TurnProcessed() || pLoopUnit->isDelayedDeath() || pLoopUnit->AI_getUnitAIType() == UNITAI_UNKNOWN ||  pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE || !pLoopUnit->canMove() || pLoopUnit->isHuman())
		{
			continue;
		}


		else if(pLoopUnit->isBarbarian() || (pLoopUnit->getDomainType() == DOMAIN_AIR && pLoopUnit->getDamage() < 50 && !m_pPlayer->GetMilitaryAI()->WillAirUnitRebase(pLoopUnit.pointer())))
		{
			pLoopUnit->setTacticalMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_UNASSIGNED]);
			m_CurrentTurnUnits.push_back(pLoopUnit->GetID());
		}


		else if(!pLoopUnit->IsCombatUnit() && !pLoopUnit->IsGreatGeneral())
		{
			continue;
		}

		else
		{

			int iArmyID = pLoopUnit->getArmyID();
			const CvArmyAI* army = m_pPlayer->getArmyAI(iArmyID);
			if(iArmyID != FFreeList::INVALID_INDEX && NULL != army && !army->CanTacticalAIInterruptUnit(pLoopUnit->GetID()))
			{
				pLoopUnit->setTacticalMove(NO_TACTICAL_MOVE);
			}

			else
			{

				int iDanger = m_pPlayer->GetPlotDanger(*(pLoopUnit->plot()));
				if(iDanger > 0 || NearVisibleEnemy(pLoopUnit, m_iRecruitRange) ||
				        pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn())
				{
					pLoopUnit->setTacticalMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_UNASSIGNED]);
					m_CurrentTurnUnits.push_back(pLoopUnit->GetID());
				}
				else if (pLoopUnit->canParadrop(pLoopUnit->plot(),false))
				{
					pLoopUnit->setTacticalMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_UNASSIGNED]);
					m_CurrentTurnUnits.push_back(pLoopUnit->GetID());
				}
			}
		}
	}
}


void CvTacticalAI::DoTurn()
{
}


void CvTacticalAI::Update()
{
	AI_PERF_FORMAT("AI-perf.csv", ("Tactical AI, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	FindTacticalTargets();


	ProcessDominanceZones();
}




CvTemporaryZone* CvTacticalAI::GetFirstTemporaryZone()
{
	CvTemporaryZone* pRtnValue = NULL;

	m_iCurrentTempZoneIndex = 0;
	if((int)m_TempZones.size() > m_iCurrentTempZoneIndex)
	{
		pRtnValue = &m_TempZones[m_iCurrentTempZoneIndex];
	}

	return pRtnValue;
}


CvTemporaryZone* CvTacticalAI::GetNextTemporaryZone()
{
	CvTemporaryZone* pRtnValue = NULL;

	m_iCurrentTempZoneIndex++;
	if((int)m_TempZones.size() > m_iCurrentTempZoneIndex)
	{
		pRtnValue = &m_TempZones[m_iCurrentTempZoneIndex];
	}

	return pRtnValue;
}


void CvTacticalAI::AddTemporaryZone(CvTemporaryZone zone)
{
	m_TempZones.push_back(zone);
}


void CvTacticalAI::DeleteTemporaryZone(CvPlot* pPlot)
{
	FStaticVector<CvTemporaryZone, SAFE_ESTIMATE_NUM_TEMP_ZONES, true, c_eCiv5GameplayDLL, 0> zonesCopy;


	zonesCopy = m_TempZones;

	m_TempZones.clear();


	for(unsigned int iI = 0; iI < zonesCopy.size(); iI++)
	{
		if(zonesCopy[iI].GetX() != pPlot->getX() ||
		        zonesCopy[iI].GetY() != pPlot->getY())
		{
			m_TempZones.push_back(zonesCopy[iI]);
		}
	}
}


void CvTacticalAI::DropObsoleteZones()
{
	FStaticVector<CvTemporaryZone, SAFE_ESTIMATE_NUM_TEMP_ZONES, true, c_eCiv5GameplayDLL, 0> zonesCopy;


	zonesCopy = m_TempZones;

	m_TempZones.clear();


	for(unsigned int iI = 0; iI < zonesCopy.size(); iI++)
	{
		if(zonesCopy[iI].GetLastTurn() >= GC.getGame().getGameTurn())
		{
			m_TempZones.push_back(zonesCopy[iI]);
		}
	}
}


bool CvTacticalAI::IsTemporaryZoneCity(CvCity* pCity)
{
	for(unsigned int iI = 0; iI < m_TempZones.size(); iI++)
	{
		if(m_TempZones[iI].GetX() == pCity->getX() &&
		        m_TempZones[iI].GetY() == pCity->getY() &&
		        m_TempZones[iI].GetTargetType() == AI_TACTICAL_TARGET_CITY)
		{
			return true;
		}
	}
	return false;
}


void CvTacticalAI::InitializeQueuedAttacks()
{
	m_QueuedAttacks.clear();
	m_iCurrentSeriesID = 0;
}


bool CvTacticalAI::QueueAttack(void* pAttacker, CvTacticalTarget* pTarget, bool bRanged, bool bCityAttack)
{
	bool bRtnValue = true;
	CvQueuedAttack attack;
	int iSeriesID;
	CvPlot* pTargetPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
	CvPlot* pFromPlot;


	int iTempSeriesID = PlotAlreadyTargeted(pTargetPlot);

	if(iTempSeriesID == -1)
	{
		m_iCurrentSeriesID++;
		iSeriesID = m_iCurrentSeriesID;
	}
	else
	{
		iSeriesID = iTempSeriesID;
		bRtnValue = false;
	}

	attack.SetAttacker(pAttacker);
	attack.SetTarget(pTarget);
	attack.SetSeriesID(iSeriesID);
	attack.SetRanged(bRanged);
	attack.SetCityAttack(bCityAttack);
	m_QueuedAttacks.push_back(attack);

	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strMsg;
		CvString strTemp;
		if(bCityAttack)
		{
			strTemp = ((CvCity*)pAttacker)->getName();
			pFromPlot = ((CvCity*)pAttacker)->plot();
		}
		else
		{
			CvUnit* pkAttackingUnit = static_cast<CvUnit*>(pAttacker);
			strTemp = pkAttackingUnit->getUnitInfo().GetDescription();
			pFromPlot = ((CvUnit*)pAttacker)->plot();
		}
		strMsg.Format("Queued attack with %s, To X: %d, To Y: %d, From X: %d, From Y: %d", strTemp.GetCString(), pTarget->GetTargetX(), pTarget->GetTargetY(), pFromPlot->getX(), pFromPlot->getY());
		LogTacticalMessage(strMsg);
	}

	return bRtnValue;
}


void CvTacticalAI::LaunchAttack(void* pAttacker, CvTacticalTarget* pTarget, bool bFirstAttack, bool bRanged, bool bCityAttack)
{
	CvCity* pCity = NULL;
	UnitHandle pUnit;

	if(bCityAttack)
	{
		pCity = (CvCity*)pAttacker;
	}
	else
	{
		pUnit = UnitHandle((CvUnit*)pAttacker);
	}

	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strMsg;
		CvString strTemp;
		int iX, iY;

		if(bCityAttack)
		{
			strTemp = pCity->getName();
			iX = pCity->getX();
			iY = pCity->getY();
		}
		else
		{
			strTemp = pUnit->getUnitInfo().GetDescription();
			iX = pUnit->getX();
			iY = pUnit->getY();
		}

		if(bRanged)
		{
			if(bFirstAttack)
			{
				strMsg.Format("Made initial ranged attack with %s, at X: %d, Y: %d, vs. target at X: %d, Y: %d", strTemp.GetCString(), iX, iY, pTarget->GetTargetX(), pTarget->GetTargetY());
			}
			else
			{
				strMsg.Format("Made follow-on ranged attack with %s, at X: %d, Y: %d, vs. target at X: %d, Y: %d", strTemp.GetCString(), iX, iY, pTarget->GetTargetX(), pTarget->GetTargetY());
			}
		}
		else
		{
			if(bFirstAttack)
			{
				strMsg.Format("Made initial attack with %s, at X: %d, Y: %d, vs. target at X: %d, Y: %d", strTemp.GetCString(), iX, iY, pTarget->GetTargetX(), pTarget->GetTargetY());
			}
			else
			{
				strMsg.Format("Made follow-on attack with %s, at X: %d, Y: %d, vs. target at X: %d, Y: %d", strTemp.GetCString(), iX, iY, pTarget->GetTargetX(), pTarget->GetTargetY());
			}
		}

		LogTacticalMessage(strMsg);
	}


	if(bCityAttack)
	{
		pCity->doTask(TASK_RANGED_ATTACK, pTarget->GetTargetX(), pTarget->GetTargetY(), 0);
	}
	else
	{
		CvAssertMsg(pUnit->getMoves() > 0, "Tactical AI is going to push a mission on the queue to attack, but the unit is out of moves.");
		CvAssertMsg(!pUnit->isOutOfAttacks(), "Tactical AI is going to push a mission on the queue to attack, but the unit is out of attacks.");

		bool bSendAttack = pUnit->getMoves() > 0 && !pUnit->isOutOfAttacks();
		if(bSendAttack)
		{
			if(bRanged && pUnit->getDomainType() != DOMAIN_AIR)
			{
				pUnit->PushMission(CvTypes::getMISSION_RANGE_ATTACK(), pTarget->GetTargetX(), pTarget->GetTargetY());
			}




			else
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->GetTargetX(), pTarget->GetTargetY());
			}
		}


		if(!bSendAttack || (!pUnit->isOutOfAttacks() && !pUnit->isFighting()))
		{
			pUnit->SetTurnProcessed(false);
			CombatResolved(pAttacker, false, bCityAttack);
		}
	}
}


void CvTacticalAI::CombatResolved(void* pAttacker, bool bVictorious, bool bCityAttack)
{
	int iSeriesID = 0;
	bool bFoundIt = false;

	CvCity* pCity = NULL;
	UnitHandle pUnit;

	if(bCityAttack)
	{
		pCity = (CvCity*)pAttacker;
	}
	else
	{
		pUnit = UnitHandle((CvUnit*)pAttacker);
	}

	if(m_QueuedAttacks.size() > 0)
	{
		std::list<CvQueuedAttack>::iterator nextToErase, nextInList;
		nextToErase = m_QueuedAttacks.begin();


		while(nextToErase != m_QueuedAttacks.end())
		{
			if(nextToErase->GetAttacker() == pAttacker)
			{
				iSeriesID = nextToErase->GetSeriesID();
				bFoundIt = true;
				break;
			}
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
			++nextToErase;
#else
			nextToErase++;
#endif
		}



		if(!bFoundIt)
		{
			return;
		}


		if(!bCityAttack && (pUnit->canMoveAfterAttacking() || !pUnit->isOutOfAttacks()) && pUnit->getMoves() > 0)
		{
			pUnit->SetTurnProcessed(false);
		}


		if(bVictorious)
		{
			bool bFirst = true;
			while(nextToErase != m_QueuedAttacks.end() && nextToErase->GetSeriesID() == iSeriesID)
			{

				if(!bFirst && !nextToErase->IsCityAttack())
				{
					UnitHandle pAbortUnit((CvUnit*)nextToErase->GetAttacker());
					pAbortUnit->SetTurnProcessed(false);
				}
				nextToErase = m_QueuedAttacks.erase(nextToErase);
				bFirst = false;
			}
		}


		else
		{
			nextInList = nextToErase;
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
			++nextInList;
#else
			nextInList++;
#endif


			if(nextInList != m_QueuedAttacks.end() && nextInList->GetSeriesID() == nextToErase->GetSeriesID())
			{




				CvQueuedAttack kAttack(*nextInList);
				m_QueuedAttacks.erase(nextToErase);
				LaunchAttack(kAttack.GetAttacker(), kAttack.GetTarget(), false           , kAttack.IsRanged(), kAttack.IsCityAttack());
			}
			else
			{
				m_QueuedAttacks.erase(nextToErase);
			}
		}
	}

	return;
}


int CvTacticalAI::PlotAlreadyTargeted(CvPlot* pPlot)
{
	if(m_QueuedAttacks.size() > 0)
	{
		std::list<CvQueuedAttack>::iterator it;
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
		for (it = m_QueuedAttacks.begin(); it != m_QueuedAttacks.end(); ++it)
#else
		for(it = m_QueuedAttacks.begin(); it != m_QueuedAttacks.end(); it++)
#endif
		{
			if(it->GetTarget()->GetTargetX() == pPlot->getX() &&
			        it->GetTarget()->GetTargetY() == pPlot->getY())
			{
				return it->GetSeriesID();
			}
		}
	}
	return -1;
}


bool CvTacticalAI::IsInQueuedAttack(const CvUnit* pUnit)
{
	if(m_QueuedAttacks.size() > 0)
	{
		std::list<CvQueuedAttack>::iterator it;
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
		for (it = m_QueuedAttacks.begin(); it != m_QueuedAttacks.end(); ++it)
#else
		for(it = m_QueuedAttacks.begin(); it != m_QueuedAttacks.end(); it++)
#endif
		{
			if(it->GetAttacker() == pUnit)
			{
				return true;
			}
		}
	}
	return false;
}


bool CvTacticalAI::IsCityInQueuedAttack(const CvCity* pAttackCity)
{
	if(m_QueuedAttacks.size() > 0)
	{
		std::list<CvQueuedAttack>::iterator it;
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
		for (it = m_QueuedAttacks.begin(); it != m_QueuedAttacks.end(); ++it)
#else
		for(it = m_QueuedAttacks.begin(); it != m_QueuedAttacks.end(); it++)
#endif
		{
			if(it->IsCityAttack() && it->GetAttacker() == (void*)pAttackCity)
			{
				return true;
			}
		}
	}
	return false;
}


int CvTacticalAI::NearXQueuedAttacks(const CvPlot* pPlot, const int iRange)
{
	int iNearbyAttacks = 0;

	if(m_QueuedAttacks.size() > 0)
	{
		std::list<CvQueuedAttack>::iterator it;
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
		for (it = m_QueuedAttacks.begin(); it != m_QueuedAttacks.end(); ++it)
#else
		for(it = m_QueuedAttacks.begin(); it != m_QueuedAttacks.end(); it++)
#endif
		{
			int iDistance;
			iDistance = plotDistance(pPlot->getX(), pPlot->getY(), it->GetTarget()->GetTargetX(), it->GetTarget()->GetTargetY());
			if(iDistance <= iRange)
			{
				iNearbyAttacks++;
			}
		}
	}
	return iNearbyAttacks;
}




void CvTacticalAI::UpdatePostures()
{
	CvTacticalDominanceZone* pZone;
	AITacticalPosture eLastPosture, eNewPosture;

	m_NewPostures.clear();


	for(int iI = 0; iI < m_pMap->GetNumZones(); iI++)
	{
		pZone = m_pMap->GetZone(iI);


		if(UseThisDominanceZone(pZone))
		{
			int iCityID = -1;
			if(pZone->GetClosestCity() != NULL)
			{
				iCityID = pZone->GetClosestCity()->GetID();
			}

			eLastPosture = FindPosture(pZone);
			eNewPosture = SelectPosture(pZone, eLastPosture);
			m_NewPostures.push_back(CvTacticalPosture(pZone->GetOwner(), pZone->IsWater(), iCityID, eNewPosture));

			if(GC.getLogging() && GC.getAILogging() && eNewPosture != AI_TACTICAL_POSTURE_NONE)
			{
				CvString szPostureMsg;
				szPostureMsg.Format("Zone ID: %d, ", pZone->GetDominanceZoneID());
				switch(eNewPosture)
				{
				case AI_TACTICAL_POSTURE_SIT_AND_BOMBARD:
					szPostureMsg += "Sit and Bombard";
					break;
				case AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE:
					szPostureMsg += "Attrit from Range";
					break;
				case AI_TACTICAL_POSTURE_EXPLOIT_FLANKS:
					szPostureMsg += "Exploit Flanks";
					break;
				case AI_TACTICAL_POSTURE_STEAMROLL:
					szPostureMsg += "Steamroll";
					break;
				case AI_TACTICAL_POSTURE_SURGICAL_CITY_STRIKE:
					szPostureMsg += "Surgical City Strike";
					break;
				case AI_TACTICAL_POSTURE_HEDGEHOG:
					szPostureMsg += "Hedgehog";
					break;
				case AI_TACTICAL_POSTURE_COUNTERATTACK:
					szPostureMsg += "Counterattack";
					break;
				case AI_TACTICAL_POSTURE_WITHDRAW:
					szPostureMsg += "Withdraw";
					break;
				case AI_TACTICAL_POSTURE_SHORE_BOMBARDMENT:
					szPostureMsg += "Shore Bombardment";
					break;
				}
				LogTacticalMessage(szPostureMsg);
			}
		}
	}


	m_Postures.clear();
	m_Postures = m_NewPostures;
}


AITacticalPosture CvTacticalAI::SelectPosture(CvTacticalDominanceZone* pZone, AITacticalPosture eLastPosture)
{
	AITacticalPosture eChosenPosture = AI_TACTICAL_POSTURE_NONE;
	int iDominancePercentage = GC.getGame().GetTacticalAnalysisMap()->GetDominancePercentage();
	eTacticalDominanceFlags eRangedDominance = TACTICAL_DOMINANCE_EVEN;
	eTacticalDominanceFlags eUnitCountDominance = TACTICAL_DOMINANCE_EVEN;
	int iRatio;



	if(pZone->GetEnemyRangedStrength() <= 0)
	{
		eRangedDominance = TACTICAL_DOMINANCE_FRIENDLY;
	}
	else
	{
		iRatio = (pZone->GetFriendlyRangedStrength() * 100) / pZone->GetEnemyRangedStrength();
		if(iRatio > 100 + iDominancePercentage)
		{
			eRangedDominance = TACTICAL_DOMINANCE_FRIENDLY;
		}
		else if(iRatio < 100 - iDominancePercentage)
		{
			eRangedDominance = TACTICAL_DOMINANCE_ENEMY;
		}
	}


	if(pZone->GetEnemyUnitCount() <= 0)
	{
		eUnitCountDominance = TACTICAL_DOMINANCE_FRIENDLY;
	}
	else
	{
		iRatio = (pZone->GetFriendlyUnitCount()  * 100) / pZone->GetEnemyUnitCount();
		if(iRatio > 100 + iDominancePercentage)
		{
			eUnitCountDominance = TACTICAL_DOMINANCE_FRIENDLY;
		}
		else if(iRatio < 100 - iDominancePercentage)
		{
			eUnitCountDominance = TACTICAL_DOMINANCE_ENEMY;
		}
	}


	switch(pZone->GetTerritoryType())
	{
	case TACTICAL_TERRITORY_ENEMY:
	{
		bool bTemporaryZone = false;
		CvCity *pClosestCity = pZone->GetClosestCity();


		if (pClosestCity)
		{
			bTemporaryZone = m_pPlayer->GetTacticalAI()->IsTemporaryZoneCity(pClosestCity);
		}


		if(!bTemporaryZone && (pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_ENEMY || pZone->GetFriendlyRangedUnitCount() == pZone->GetFriendlyUnitCount()))
		{
			eChosenPosture = AI_TACTICAL_POSTURE_WITHDRAW;
		}


		else if (bTemporaryZone)
		{
			if (pZone->GetDominanceFlag() != TACTICAL_DOMINANCE_ENEMY)
			{
				eChosenPosture = AI_TACTICAL_POSTURE_STEAMROLL;
			}
			else
			{
				eChosenPosture = AI_TACTICAL_POSTURE_SURGICAL_CITY_STRIKE;
			}
		}


		else if(pZone->GetEnemyUnitCount() > 0 && pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY &&
		        (eRangedDominance != TACTICAL_DOMINANCE_ENEMY || pZone->GetFriendlyStrength() > pZone->GetEnemyStrength() * 2))
		{
			eChosenPosture = AI_TACTICAL_POSTURE_STEAMROLL;
		}


		else if(eLastPosture == AI_TACTICAL_POSTURE_STEAMROLL && pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY && pZone->GetEnemyUnitCount() > 0)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_STEAMROLL;
		}


		else if(eRangedDominance == TACTICAL_DOMINANCE_FRIENDLY && eUnitCountDominance != TACTICAL_DOMINANCE_ENEMY)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_SIT_AND_BOMBARD;
		}


		else if(eLastPosture == AI_TACTICAL_POSTURE_SIT_AND_BOMBARD && eRangedDominance != TACTICAL_DOMINANCE_ENEMY && eUnitCountDominance != TACTICAL_DOMINANCE_ENEMY)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_SIT_AND_BOMBARD;
		}


		else if(pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_SURGICAL_CITY_STRIKE;
		}


		else if(eUnitCountDominance == TACTICAL_DOMINANCE_FRIENDLY && pZone->GetEnemyUnitCount() > 1)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
		}


		else if(eLastPosture == AI_TACTICAL_POSTURE_EXPLOIT_FLANKS && eUnitCountDominance != TACTICAL_DOMINANCE_ENEMY && pZone->GetEnemyUnitCount() > 1)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
		}


		else
		{
			eChosenPosture = AI_TACTICAL_POSTURE_SURGICAL_CITY_STRIKE;
		}
		break;
	}

	case TACTICAL_TERRITORY_NEUTRAL:
	case TACTICAL_TERRITORY_NO_OWNER:
	{
		if(eRangedDominance == TACTICAL_DOMINANCE_FRIENDLY && eUnitCountDominance != TACTICAL_DOMINANCE_ENEMY)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE;
		}


		else if(eLastPosture == AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE && eRangedDominance != TACTICAL_DOMINANCE_ENEMY)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE;
		}


		else if(eUnitCountDominance == TACTICAL_DOMINANCE_FRIENDLY && pZone->GetEnemyUnitCount() > 0)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
		}


		else if(eLastPosture == AI_TACTICAL_POSTURE_EXPLOIT_FLANKS && eUnitCountDominance != TACTICAL_DOMINANCE_ENEMY && pZone->GetEnemyUnitCount() > 0)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
		}


		else
		{
			eChosenPosture = AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
		}
		break;
#ifndef AUI_WARNING_FIXES

		break;
#endif
	}
	case TACTICAL_TERRITORY_FRIENDLY:
	{
		if(eRangedDominance == TACTICAL_DOMINANCE_FRIENDLY && pZone->GetFriendlyRangedUnitCount() > 1)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE;
		}


		else if(eLastPosture == AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE && pZone->GetFriendlyRangedUnitCount() > 1 && eRangedDominance != TACTICAL_DOMINANCE_ENEMY)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE;
		}


		else if(eUnitCountDominance == TACTICAL_DOMINANCE_FRIENDLY && pZone->GetEnemyUnitCount() > 0)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
		}


		else if(eLastPosture == AI_TACTICAL_POSTURE_EXPLOIT_FLANKS && eUnitCountDominance != TACTICAL_DOMINANCE_ENEMY && pZone->GetEnemyUnitCount() > 0)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
		}


		else if(pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY ||
#ifdef AUI_WARNING_FIXES
				(pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_EVEN && eRangedDominance == TACTICAL_DOMINANCE_ENEMY))
#else
		        pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_EVEN && eRangedDominance == TACTICAL_DOMINANCE_ENEMY)
#endif
		{
			eChosenPosture = AI_TACTICAL_POSTURE_COUNTERATTACK;
		}


		else if(eLastPosture == AI_TACTICAL_POSTURE_COUNTERATTACK && pZone->GetDominanceFlag() != TACTICAL_DOMINANCE_ENEMY)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_COUNTERATTACK;
		}


		else
		{
			eChosenPosture = AI_TACTICAL_POSTURE_HEDGEHOG;
		}
		break;
	}
	case TACTICAL_TERRITORY_TEMP_ZONE:
	{

		if(pZone->IsWater())
		{
			eChosenPosture = AI_TACTICAL_POSTURE_SHORE_BOMBARDMENT;
		}
		else
		{

			eChosenPosture = AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
		}
	}
	}

	return eChosenPosture;
}


AITacticalPosture CvTacticalAI::FindPosture(CvTacticalDominanceZone* pZone)
{
	if(pZone != NULL)
	{
		for(unsigned int iI = 0; iI < m_Postures.size(); iI++)
		{
			if(m_Postures[iI].GetPlayer() == pZone->GetOwner() &&
			        m_Postures[iI].IsWater() == pZone->IsWater())
			{
				int iCityID = -1;
				if(pZone->GetClosestCity() != NULL)
				{
					iCityID = pZone->GetClosestCity()->GetID();
				}
				if(m_Postures[iI].GetCityID() == iCityID)
				{
					return m_Postures[iI].GetPosture();
				}
			}
		}
	}

	return AI_TACTICAL_POSTURE_NONE;
}


void CvTacticalAI::EstablishTacticalPriorities()
{
	CvTacticalMove move;

	m_MovePriorityList.clear();


#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumTacticalMoveInfos(); iI++)
#else
	for(int iI = 0; iI < GC.getNumTacticalMoveInfos(); iI++)
#endif
	{
		const TacticalAIMoveTypes eTacticalAIMove = static_cast<TacticalAIMoveTypes>(iI);
		CvTacticalMoveXMLEntry* pkTacticalMoveInfo = GC.getTacticalMoveInfo(eTacticalAIMove);
		if(pkTacticalMoveInfo)
		{
			move.m_eMoveType = eTacticalAIMove;
			move.m_iPriority = pkTacticalMoveInfo->GetPriority();

			if(move.m_iPriority >= 0)
			{
				m_MovePriorityList.push_back(move);
			}
		}
	}


	std::stable_sort(m_MovePriorityList.begin(), m_MovePriorityList.end());
}



void CvTacticalAI::EstablishBarbarianPriorities()
{
	int iPriority = 0;


	if(GC.getGame().getGameTurn() <= m_MovePriorityTurn)
	{
		return;
	}

	m_MovePriorityList.clear();
	m_MovePriorityTurn = GC.getGame().getGameTurn();


	for(int iI = 0; iI < NUM_AI_BARBARIAN_MOVES; iI++)
	{

		switch((AIBarbarianTacticalMove)iI)
		{
		case AI_TACTICAL_BARBARIAN_CAPTURE_CITY:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_CAPTURE_CITY();
			break;
		case AI_TACTICAL_BARBARIAN_DAMAGE_CITY:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_DAMAGE_CITY();
			break;
		case AI_TACTICAL_BARBARIAN_DESTROY_HIGH_PRIORITY_UNIT:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_HIGH_UNIT();
			break;
		case AI_TACTICAL_BARBARIAN_DESTROY_MEDIUM_PRIORITY_UNIT:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_MEDIUM_UNIT();
			break;
		case AI_TACTICAL_BARBARIAN_DESTROY_LOW_PRIORITY_UNIT:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_LOW_UNIT();
			break;
		case AI_TACTICAL_BARBARIAN_MOVE_TO_SAFETY:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_TO_SAFETY();
			break;
		case AI_TACTICAL_BARBARIAN_ATTRIT_HIGH_PRIORITY_UNIT:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_HIGH_UNIT();
			break;
		case AI_TACTICAL_BARBARIAN_ATTRIT_MEDIUM_PRIORITY_UNIT:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_MEDIUM_UNIT();
			break;
		case AI_TACTICAL_BARBARIAN_ATTRIT_LOW_PRIORITY_UNIT:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_LOW_UNIT();
			break;
		case AI_TACTICAL_BARBARIAN_PILLAGE:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE();
			break;
		case AI_TACTICAL_BARBARIAN_PILLAGE_CITADEL:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE_CITADEL();
			break;
		case AI_TACTICAL_BARBARIAN_PILLAGE_NEXT_TURN:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE_NEXT_TURN();
			break;
		case AI_TACTICAL_BARBARIAN_PRIORITY_BLOCKADE_RESOURCE:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_BLOCKADE_RESOURCE();
			break;
		case AI_TACTICAL_BARBARIAN_CIVILIAN_ATTACK:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_CIVILIAN_ATTACK();
			break;
		case AI_TACTICAL_BARBARIAN_AGGRESSIVE_MOVE:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_AGGRESSIVE_MOVE();
			break;
		case AI_TACTICAL_BARBARIAN_PASSIVE_MOVE:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_PASSIVE_MOVE();
			break;
		case AI_TACTICAL_BARBARIAN_CAMP_DEFENSE:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_CAMP_DEFENSE();
			break;
		case AI_TACTICAL_BARBARIAN_DESPERATE_ATTACK:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_DESPERATE_ATTACK();
			break;
		case AI_TACTICAL_BARBARIAN_ESCORT_CIVILIAN:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_ESCORT_CIVILIAN();
			break;
		case AI_TACTICAL_BARBARIAN_PLUNDER_TRADE_UNIT:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_PLUNDER_TRADE_UNIT();
			break;
		}


		if(iPriority >= 0)
		{

			iPriority += GC.getGame().getJonRandNum(GC.getAI_TACTICAL_MOVE_PRIORITY_RANDOMNESS(), "Tactical AI Move Priority");


			CvTacticalMove move;
			move.m_eMoveType = (TacticalAIMoveTypes)iI;
			move.m_iPriority = iPriority;
			m_MovePriorityList.push_back(move);
		}
	}


	std::stable_sort(m_MovePriorityList.begin(), m_MovePriorityList.end());
}



void CvTacticalAI::FindTacticalTargets()
{
#ifndef AUI_WARNING_FIXES
	int iI;
#endif
	CvPlot* pLoopPlot;
	CvTacticalTarget newTarget;
	bool bValidPlot;
	bool bEnemyDominatedPlot;
	CvPlayerTrade* pPlayerTrade = m_pPlayer->GetTrade();


	m_AllTargets.clear();

	bool bBarbsAllowedYet = GC.getGame().getGameTurn() >= GC.getGame().GetBarbarianReleaseTurn();


#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getMap().numPlots(); iI++)
#else
	for(iI = 0; iI < GC.getMap().numPlots(); iI++)
#endif
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
		bValidPlot = false;

		if(pLoopPlot->isVisible(m_pPlayer->getTeam()))
		{

			if(!m_pPlayer->isBarbarian() || bBarbsAllowedYet)
			{
				bValidPlot = true;
			}
			else
			{
				if(!pLoopPlot->isOwned())
				{
					bValidPlot = true;
				}
			}
			if(bValidPlot)
			{
				if(PlotAlreadyTargeted(pLoopPlot) != -1)
				{
					bValidPlot = false;
				}
			}
		}

		if(bValidPlot)
		{
			newTarget.Clear();
			newTarget.SetTargetX(pLoopPlot->getX());
			newTarget.SetTargetY(pLoopPlot->getY());
			newTarget.SetDominanceZone(GC.getGame().GetTacticalAnalysisMap()->GetCell(iI)->GetDominanceZone());

			bEnemyDominatedPlot = GC.getGame().GetTacticalAnalysisMap()->IsInEnemyDominatedZone(pLoopPlot);



			CvCity* pCity = pLoopPlot->getPlotCity();
			if(pCity != NULL)
			{
				if(m_pPlayer->GetID() == pCity->getOwner())
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_CITY_TO_DEFEND);
					newTarget.SetAuxData((void*)pCity);
					newTarget.SetAuxIntData(pCity->getThreatValue());
					m_AllTargets.push_back(newTarget);
				}


				else if(atWar(m_pPlayer->getTeam(), pCity->getTeam()))
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_CITY);
					newTarget.SetTargetPlayer(pCity->getOwner());
					newTarget.SetAuxData((void*)pCity);
					m_AllTargets.push_back(newTarget);
				}
			}
			else
			{

				CvUnit* pUnit = pLoopPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
				if(pUnit != NULL)
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT);
					newTarget.SetTargetPlayer(pUnit->getOwner());
					newTarget.SetAuxData((void*)pUnit);
					newTarget.SetAuxIntData(pUnit->getDamage());
					m_AllTargets.push_back(newTarget);
				}


				else if(pLoopPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
					newTarget.SetTargetPlayer(BARBARIAN_PLAYER);
					newTarget.SetAuxData((void*)pLoopPlot);
					m_AllTargets.push_back(newTarget);
				}


				else if(!m_pPlayer->isMinorCiv() && pLoopPlot->isGoody())
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_ANCIENT_RUINS);
					newTarget.SetAuxData((void*)pLoopPlot);
					m_AllTargets.push_back(newTarget);
				}


				else if(atWar(m_pPlayer->getTeam(), pLoopPlot->getTeam()) &&
					pLoopPlot->getImprovementType() != NO_IMPROVEMENT &&
					GC.getImprovementInfo(pLoopPlot->getImprovementType())->GetNearbyEnemyDamage() > 0)
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_CITADEL);
					newTarget.SetTargetPlayer(pLoopPlot->getOwner());
					newTarget.SetAuxData((void*)pLoopPlot);
					m_AllTargets.push_back(newTarget);
				}


				else if(atWar(m_pPlayer->getTeam(), pLoopPlot->getTeam()) &&
				        pLoopPlot->getImprovementType() != NO_IMPROVEMENT &&
				        !pLoopPlot->IsImprovementPillaged())
				{
					ResourceUsageTypes eRUT = (ResourceUsageTypes)-1;
					ResourceTypes eResource = pLoopPlot->getResourceType();
					if (eResource != NO_RESOURCE)
					{
						eRUT = GC.getResourceInfo(eResource)->getResourceUsage();
					}


					if (eRUT == RESOURCEUSAGE_STRATEGIC || eRUT == RESOURCEUSAGE_LUXURY || pLoopPlot->isWater() || m_pPlayer->isBarbarian())
					{

						if(m_pPlayer->isBarbarian() && pLoopPlot->isWater())
						{
							continue;
						}
						else
						{
							newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE);
							newTarget.SetTargetPlayer(pLoopPlot->getOwner());
							newTarget.SetAuxData((void*)pLoopPlot);
							m_AllTargets.push_back(newTarget);
						}
					}
					else
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT);
						newTarget.SetTargetPlayer(pLoopPlot->getOwner());
						newTarget.SetAuxData((void*)pLoopPlot);
						m_AllTargets.push_back(newTarget);
					}
				}


				else if(atWar(m_pPlayer->getTeam(), pLoopPlot->getTeam()) &&
				        pLoopPlot->getRouteType() != NO_ROUTE && !pLoopPlot->IsRoutePillaged() && pLoopPlot->IsTradeRoute()                            )
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT);
					newTarget.SetTargetPlayer(pLoopPlot->getOwner());
					newTarget.SetAuxData((void*)pLoopPlot);
					m_AllTargets.push_back(newTarget);
				}


				else if(pLoopPlot->isVisibleOtherUnit(m_pPlayer->GetID()))
				{
					CvUnit* pTargetUnit = pLoopPlot->getUnitByIndex(0);
					if(!pTargetUnit->isDelayedDeath() && atWar(m_pPlayer->getTeam(), pTargetUnit->getTeam()) && !pTargetUnit->IsCanDefend())
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_LOW_PRIORITY_CIVILIAN);
						newTarget.SetTargetPlayer(pTargetUnit->getOwner());
						newTarget.SetAuxData((void*)pTargetUnit);

						if(pTargetUnit->isEmbarked())
						{
							if(pTargetUnit->IsCombatUnit())
							{
								newTarget.SetTargetType(AI_TACTICAL_TARGET_EMBARKED_MILITARY_UNIT);
							}
							else
							{
								newTarget.SetTargetType(AI_TACTICAL_TARGET_EMBARKED_CIVILIAN);
							}
						}
						else
						{
							if(IsVeryHighPriorityCivilianTarget(&newTarget))
							{
								newTarget.SetTargetType(AI_TACTICAL_TARGET_VERY_HIGH_PRIORITY_CIVILIAN);
							}
							else if(IsHighPriorityCivilianTarget(&newTarget))
							{
								newTarget.SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_CIVILIAN);
							}
							else if(IsMediumPriorityCivilianTarget(&newTarget))
							{
								newTarget.SetTargetType(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_CIVILIAN);
							}
						}
						m_AllTargets.push_back(newTarget);
					}
				}


				else if (pPlayerTrade->ContainsEnemyTradeUnit(pLoopPlot))
				{
					if (pLoopPlot->isWater())
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_TRADE_UNIT_SEA);
					}
					else
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_TRADE_UNIT_LAND);
					}

					newTarget.SetAuxData((void*)pLoopPlot);
					m_AllTargets.push_back(newTarget);
				}


				else if(m_pPlayer->GetID() == pLoopPlot->getOwner() &&
				        pLoopPlot->defenseModifier(m_pPlayer->getTeam(), true) > 0 &&
				        m_pPlayer->GetPlotDanger(*pLoopPlot) > 0)
				{
					CvCity* pDefenseCity = pLoopPlot->GetAdjacentFriendlyCity(m_pPlayer->getTeam(), true             );
					if(pDefenseCity)
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_DEFENSIVE_BASTION);
						newTarget.SetAuxData((void*)pLoopPlot);
						newTarget.SetAuxIntData(pDefenseCity->getThreatValue() + m_pPlayer->GetPlotDanger(*pLoopPlot));
						m_AllTargets.push_back(newTarget);
					}
				}


				else if(m_pPlayer->GetID() == pLoopPlot->getOwner() &&
				        pLoopPlot->getImprovementType() != NO_IMPROVEMENT &&
				        !pLoopPlot->IsImprovementPillaged() && !pLoopPlot->isGoody())
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT_TO_DEFEND);
					newTarget.SetAuxData((void*)pLoopPlot);
					m_AllTargets.push_back(newTarget);
				}


				else if (pLoopPlot->isVisible(m_pPlayer->getTeam()) && 
						 pPlayerTrade->ContainsEnemyTradePlot(pLoopPlot))
				{
					if (pLoopPlot->isWater())
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_TRADE_UNIT_SEA_PLOT);
					}
					else
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_TRADE_UNIT_LAND_PLOT);
					}

					newTarget.SetAuxData((void*)pLoopPlot);
					m_AllTargets.push_back(newTarget);
				}
			}
		}
	}




	if(m_pPlayer->isBarbarian())
	{
		IdentifyPriorityBarbarianTargets();
	}
	else
	{
		IdentifyPriorityTargets();
	}


	IdentifyPriorityTargetsByType();


	EliminateNearbyBlockadePoints();


	std::stable_sort(m_AllTargets.begin(), m_AllTargets.end());
}

void CvTacticalAI::ProcessDominanceZones()
{
	CvTacticalDominanceZone* pZone;
	FStaticVector<CvTacticalMove, 256, true, c_eCiv5GameplayDLL >::iterator it;


	if(m_pPlayer->isBarbarian())
	{
		EstablishBarbarianPriorities();
		ExtractTargetsForZone(NULL);
		AssignBarbarianMoves();
	}

	else
	{
		EstablishTacticalPriorities();
		UpdatePostures();


#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
		for (it = m_MovePriorityList.begin(); it != m_MovePriorityList.end(); ++it)
#else
		for(it = m_MovePriorityList.begin(); it != m_MovePriorityList.end(); it++)
#endif
		{
			CvTacticalMove move = *it;

			if(move.m_iPriority >= 0)
			{
				CvTacticalMoveXMLEntry* pkTacticalMoveInfo = GC.getTacticalMoveInfo(move.m_eMoveType);
				if(pkTacticalMoveInfo && pkTacticalMoveInfo->IsDominanceZoneMove())
				{
					for(int iI = 0; iI < m_pMap->GetNumZones(); iI++)
					{
						m_iCurrentZoneIndex = iI;
						pZone = m_pMap->GetZone(iI);
						AITacticalPosture ePosture = FindPosture(pZone);
						CvString strMoveName = (CvString)pkTacticalMoveInfo->GetType();
						CvTacticalMove moveToPassOn = move;


						bool bMatch = false;
						if(strMoveName == "TACTICAL_CLOSE_ON_TARGET")
						{
							bMatch = true;
						}
						else if(ePosture == AI_TACTICAL_POSTURE_WITHDRAW && strMoveName == "TACTICAL_POSTURE_WITHDRAW")
						{
							bMatch = true;
						}
						else if(ePosture == AI_TACTICAL_POSTURE_HEDGEHOG && strMoveName == "TACTICAL_POSTURE_HEDGEHOG")
						{
							bMatch = true;
						}
						else if(ePosture == AI_TACTICAL_POSTURE_SHORE_BOMBARDMENT && strMoveName == "TACTICAL_POSTURE_SHORE_BOMBARDMENT")
						{
							bMatch = true;
						}
						else if(pZone->GetDominanceFlag() != TACTICAL_DOMINANCE_FRIENDLY && pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY && strMoveName == "TACTICAL_EMERGENCY_PURCHASES")
						{
							bMatch = true;
						}
						else if(pZone->GetDominanceFlag() != TACTICAL_DOMINANCE_FRIENDLY && pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY && strMoveName == "TACTICAL_DEFENSIVE_AIRLIFT")
						{
							bMatch = true;
						}
						else if (strMoveName == "TACTICAL_OFFENSIVE_POSTURE_MOVES")
						{



							bMatch = true;
							
							switch (ePosture)
							{
							case AI_TACTICAL_POSTURE_SIT_AND_BOMBARD:
								moveToPassOn.m_eMoveType = (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_SIT_AND_BOMBARD];
								break;
							case AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE:
								moveToPassOn.m_eMoveType = (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_ATTRIT_FROM_RANGE];
								break;
							case AI_TACTICAL_POSTURE_EXPLOIT_FLANKS:
								moveToPassOn.m_eMoveType = (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_EXPLOIT_FLANKS];
								break;
							case AI_TACTICAL_POSTURE_STEAMROLL:
								moveToPassOn.m_eMoveType = (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_STEAMROLL];
								break;
							case AI_TACTICAL_POSTURE_SURGICAL_CITY_STRIKE:
								moveToPassOn.m_eMoveType = (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_SURGICAL_CITY_STRIKE];
								break;
							case AI_TACTICAL_POSTURE_COUNTERATTACK:
								moveToPassOn.m_eMoveType = (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_COUNTERATTACK];
								break;
							default:
								bMatch = false;
								break;
							}
						}

						if(bMatch)
						{
							if(!UseThisDominanceZone(pZone))
							{
								continue;
							}

							ExtractTargetsForZone(pZone);



							if(m_ZoneTargets.size() <= 0 && pZone->GetTerritoryType() != TACTICAL_TERRITORY_TEMP_ZONE && (pZone->GetTerritoryType() != TACTICAL_TERRITORY_ENEMY || pZone->IsWater()))
							{
								continue;
							}

							AssignTacticalMove(moveToPassOn);
						}
					}
				}
				else
				{
					ExtractTargetsForZone(NULL);
					AssignTacticalMove(move);
				}
			}
		}
	}
}


void CvTacticalAI::AssignTacticalMove(CvTacticalMove move)
{
#pragma warning ( push )
#pragma warning ( disable : 6011 )
	AI_PERF_FORMAT("AI-perf-tact.csv", ("Move Type: %s (%d), Turn %03d, %s", GC.getTacticalMoveInfo(move.m_eMoveType)->GetType(), (int)move.m_eMoveType, GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );
#pragma warning ( pop )

	if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_MOVE_NONCOMBATANTS_TO_SAFETY])
	{
		PlotMovesToSafety(false                 );
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_CAPTURE_CITY])
	{
		PlotCaptureCityMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_DAMAGE_CITY])
	{
		PlotDamageCityMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_DESTROY_HIGH_UNIT])
	{
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_DESTROY_MEDIUM_UNIT])
	{
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_DESTROY_LOW_UNIT])
	{
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, true);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_TO_SAFETY])
	{
		PlotMovesToSafety(true                 );
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ATTRIT_HIGH_UNIT])
	{
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ATTRIT_MEDIUM_UNIT])
	{
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, false);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ATTRIT_LOW_UNIT])
	{
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, false);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_REPOSITION])
	{
		PlotRepositionMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_BARBARIAN_CAMP])
	{
		PlotBarbarianCampMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_PILLAGE_CITADEL])
	{
		PlotPillageMoves(AI_TACTICAL_TARGET_CITADEL, true               );
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_PILLAGE_RESOURCE])
	{
		PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE, true               );
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_PILLAGE])
	{
		PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT, true               );
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_PILLAGE_CITADEL_NEXT_TURN])
	{
		PlotPillageMoves(AI_TACTICAL_TARGET_CITADEL, false               );
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_PILLAGE_RESOURCE_NEXT_TURN])
	{
		PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE, false               );
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_PILLAGE_NEXT_TURN])
	{
		PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT, false               );
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_PLUNDER_TRADE_UNIT])
	{
		PlotPlunderTradeUnitMoves(DOMAIN_LAND);
		PlotPlunderTradeUnitMoves(DOMAIN_SEA);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_PARK_ON_TRADE_ROUTE])
	{
		PlotPlunderTradePlotMoves(DOMAIN_LAND);
		PlotPlunderTradePlotMoves(DOMAIN_SEA);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ATTACK_VERY_HIGH_PRIORITY_CIVILIAN])
	{
		PlotCivilianAttackMoves(AI_TACTICAL_TARGET_VERY_HIGH_PRIORITY_CIVILIAN);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ATTACK_HIGH_PRIORITY_CIVILIAN])
	{
		PlotCivilianAttackMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_CIVILIAN);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ATTACK_MEDIUM_PRIORITY_CIVILIAN])
	{
		PlotCivilianAttackMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_CIVILIAN);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ATTACK_LOW_PRIORITY_CIVILIAN])
	{
		PlotCivilianAttackMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_CIVILIAN);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_SAFE_BOMBARDS])
	{
		PlotSafeBombardMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_HEAL])
	{
		PlotHealMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ANCIENT_RUINS])
	{
		PlotAncientRuinMoves(1);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GARRISON_TO_ALLOW_BOMBARD])
	{
		PlotGarrisonMoves(1, true                           );
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GARRISON_ALREADY_THERE])
	{
		PlotGarrisonMoves(0);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_BASTION_ALREADY_THERE])
	{
		PlotBastionMoves(0);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GUARD_IMPROVEMENT_ALREADY_THERE])
	{
		PlotGuardImprovementMoves(0);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GARRISON_1_TURN])
	{
		PlotGarrisonMoves(1);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_BASTION_1_TURN])
	{
		PlotBastionMoves(1);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GUARD_IMPROVEMENT_1_TURN])
	{
		PlotGuardImprovementMoves(1);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_AIR_INTERCEPT])
	{
		PlotAirInterceptMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_AIR_SWEEP])
	{
		PlotAirSweepMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_SIT_AND_BOMBARD])
	{
		PlotSitAndBombardMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_ATTRIT_FROM_RANGE])
	{
		PlotAttritFromRangeMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_EXPLOIT_FLANKS])
	{
		PlotExploitFlanksMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_STEAMROLL])
	{
		PlotSteamrollMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_SURGICAL_CITY_STRIKE])
	{
		PlotSurgicalCityStrikeMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_HEDGEHOG])
	{
		PlotHedgehogMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_COUNTERATTACK])
	{
		PlotCounterattackMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_WITHDRAW])
	{
		PlotWithdrawMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_SHORE_BOMBARDMENT])
	{
		PlotShoreBombardmentMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_CLOSE_ON_TARGET])
	{
		PlotCloseOnTarget(true                    );
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_MOVE_OPERATIONS])
	{
		PlotOperationalArmyMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_EMERGENCY_PURCHASES])
	{
		PlotEmergencyPurchases();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_DEFENSIVE_AIRLIFT])
	{
		PlotDefensiveAirlifts();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ESCORT_EMBARKED_UNIT])
	{
		PlotEscortEmbarkedMoves();
	}
}


void CvTacticalAI::AssignBarbarianMoves()
{
	FStaticVector<CvTacticalMove, 256, true, c_eCiv5GameplayDLL >::iterator it;


#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_MovePriorityList.begin(); it != m_MovePriorityList.end(); ++it)
#else
	for(it = m_MovePriorityList.begin(); it != m_MovePriorityList.end(); it++)
#endif
	{
		CvTacticalMove move = *it;

		AI_PERF_FORMAT("AI-perf-tact.csv", ("Barb Move: %d, Turn %03d, %s", (int)move.m_eMoveType, GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

#ifdef AUI_WARNING_FIXES
		switch (static_cast<AIBarbarianTacticalMove>(move.m_eMoveType))
#else
		switch(move.m_eMoveType)
#endif
		{
		case AI_TACTICAL_BARBARIAN_CAPTURE_CITY:
			PlotCaptureCityMoves();
			break;
		case AI_TACTICAL_BARBARIAN_DAMAGE_CITY:
			PlotDamageCityMoves();
			break;
		case AI_TACTICAL_BARBARIAN_DESTROY_HIGH_PRIORITY_UNIT:
			PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true);
			break;
		case AI_TACTICAL_BARBARIAN_DESTROY_MEDIUM_PRIORITY_UNIT:
			PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);
			break;
		case AI_TACTICAL_BARBARIAN_DESTROY_LOW_PRIORITY_UNIT:
			PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, true);
			break;
		case AI_TACTICAL_BARBARIAN_MOVE_TO_SAFETY:
			PlotMovesToSafety(true                 );
			break;
		case AI_TACTICAL_BARBARIAN_ATTRIT_HIGH_PRIORITY_UNIT:
			PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false);
			break;
		case AI_TACTICAL_BARBARIAN_ATTRIT_MEDIUM_PRIORITY_UNIT:
			PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, false);
			break;
		case AI_TACTICAL_BARBARIAN_ATTRIT_LOW_PRIORITY_UNIT:
			PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, false);
			break;
		case AI_TACTICAL_BARBARIAN_PILLAGE:
			PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE, true              );
			break;
		case AI_TACTICAL_BARBARIAN_PILLAGE_CITADEL:
			PlotPillageMoves(AI_TACTICAL_TARGET_CITADEL, true              );
			break;
		case AI_TACTICAL_BARBARIAN_PILLAGE_NEXT_TURN:
			PlotPillageMoves(AI_TACTICAL_TARGET_CITADEL, false              );
			PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE, false              );
			break;
		case AI_TACTICAL_BARBARIAN_PRIORITY_BLOCKADE_RESOURCE:

			break;
		case AI_TACTICAL_BARBARIAN_CIVILIAN_ATTACK:
			PlotCivilianAttackMoves(AI_TACTICAL_TARGET_VERY_HIGH_PRIORITY_CIVILIAN);
			PlotCivilianAttackMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_CIVILIAN);
			PlotCivilianAttackMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_CIVILIAN);
			PlotCivilianAttackMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_CIVILIAN);
			break;
		case AI_TACTICAL_BARBARIAN_CAMP_DEFENSE:
			PlotCampDefenseMoves();
			break;
		case AI_TACTICAL_BARBARIAN_AGGRESSIVE_MOVE:
			PlotBarbarianMove(true                );
			break;
		case AI_TACTICAL_BARBARIAN_PASSIVE_MOVE:
			PlotBarbarianMove(false                );
			break;
		case AI_TACTICAL_BARBARIAN_DESPERATE_ATTACK:
			PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, false, true);
			break;
		case AI_TACTICAL_BARBARIAN_ESCORT_CIVILIAN:
			PlotBarbarianCivilianEscortMove();
			break;
		case AI_TACTICAL_BARBARIAN_PLUNDER_TRADE_UNIT:
			PlotBarbarianPlunderTradeUnitMove(DOMAIN_LAND);
			PlotBarbarianPlunderTradeUnitMove(DOMAIN_SEA);
			break;
		}
	}

	ReviewUnassignedUnits();
}


bool CvTacticalAI::PlotCaptureCityMoves()
{
	int iRequiredDamage;
	bool bAttackMade = false;
	bool bLog = GC.getLogging() && GC.getAILogging();


	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_CITY);
	while(pTarget != NULL)
	{

		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if(pPlot != NULL)
		{
			m_CurrentMoveCities.clear();
			if(FindUnitsWithinStrikingDistance(pPlot, 1, 0, false                   , false               , false                     , true                         ))
			{

				CvCity* pCity = pPlot->getPlotCity();
				if(pCity != NULL)
				{
					iRequiredDamage = pCity->GetMaxHitPoints() - pCity->getDamage();
					pTarget->SetAuxIntData(iRequiredDamage);
					if(ComputeTotalExpectedDamage(pTarget, pPlot) >= (iRequiredDamage / 2))
					{

						if(bLog)
						{
							CvString strPlayerName, strCityName, strLogString, strTemp;
							strPlayerName = GET_PLAYER(pCity->getOwner()).getCivilizationShortDescription();
							strCityName = pCity->getName();
							strLogString.Format("Attacking city of ");
							strLogString += strCityName;
							strTemp.Format(" to capture, X: %d, Y: %d,", pCity->getX(), pCity->getY());
							strLogString += strTemp + strPlayerName;
							LogTacticalMessage(strLogString);
						}


						ExecuteAttack(pTarget, pPlot, false, false);
						bAttackMade = true;


						if(pPlot->getOwner() == m_pPlayer->GetID())
						{
							DeleteTemporaryZone(pPlot);
						}
					}
				}
			}
		}
		pTarget = GetNextZoneTarget();
	}
	return bAttackMade;
}


bool CvTacticalAI::PlotDamageCityMoves()
{
	int iRequiredDamage;
	bool bAttackMade = false;
	bool bLog = GC.getLogging() && GC.getAILogging();


	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_CITY);
	while(pTarget != NULL)
	{

		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if(pPlot != NULL)
		{
			m_CurrentMoveCities.clear();
			if(FindUnitsWithinStrikingDistance(pPlot, 1, 0, false                   , false               , false                     , true                         ))
			{
				CvCity* pCity = pPlot->getPlotCity();
				if(pCity != NULL)
				{
					iRequiredDamage = pCity->GetMaxHitPoints() - pCity->getDamage();
					pTarget->SetAuxIntData(iRequiredDamage);


					if(ComputeTotalExpectedDamage(pTarget, pPlot) > (iRequiredDamage / 8))
					{

						if(bLog)
						{
							CvString strPlayerName, strCityName, strLogString, strTemp;
							strPlayerName = GET_PLAYER(pCity->getOwner()).getCivilizationShortDescription();
							strCityName = pCity->getName();
							strLogString.Format("Attacking city of ");
							strLogString += strCityName;
							strTemp.Format(" to damage, X: %d, Y: %d,", pCity->getX(), pCity->getY());
							strLogString += strTemp + strPlayerName;
							LogTacticalMessage(strLogString);
						}


						ExecuteAttack(pTarget, pPlot, false, true);
						bAttackMade = true;
					}
				}
			}
		}
		pTarget = GetNextZoneTarget();
	}
	return bAttackMade;
}


void CvTacticalAI::PlotBarbarianCampMoves()
{
	bool bLog = GC.getLogging() && GC.getAILogging();
	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
	while(pTarget != NULL)
	{

		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if(FindUnitsWithinStrikingDistance(pPlot, 1, 0, false                     , false               , false                     , false                         , false                 , true                      ))
		{

			ExecuteBarbarianCampMove(pPlot);
			if(bLog)
			{
				CvString strLogString;
				strLogString.Format("Removing barbarian camp, X: %d, Y: %d", pPlot->getX(), pPlot->getY());
				LogTacticalMessage(strLogString);
			}
			DeleteTemporaryZone(pPlot);
		}
		pTarget = GetNextZoneTarget();
	}
}


void CvTacticalAI::PlotDestroyUnitMoves(AITacticalTargetType targetType, bool bMustBeAbleToKill, bool bAttackAtPoorOdds)
{
	int iRequiredDamage;
	int iExpectedDamage;


	CvTacticalTarget* pTarget = GetFirstZoneTarget(targetType);
	while(pTarget != NULL)
	{
		bool bUnitCanAttack = false;
		bool bCityCanAttack = false;


		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		UnitHandle pDefender = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
		if(pDefender)
		{
			bUnitCanAttack = FindUnitsWithinStrikingDistance(pPlot, 1, 0, false                     );
			bCityCanAttack = FindCitiesWithinStrikingDistance(pPlot);
			if(bUnitCanAttack || bCityCanAttack)
			{
				iExpectedDamage = ComputeTotalExpectedDamage(pTarget, pPlot);
				iExpectedDamage += ComputeTotalExpectedBombardDamage(pDefender);
				iRequiredDamage = pDefender->GetCurrHitPoints();
				pTarget->SetAuxIntData(iRequiredDamage);

				if(!bMustBeAbleToKill)
				{

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;

						CvString strPlayerName = GET_PLAYER(pDefender->getOwner()).getCivilizationShortDescription();

						CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pDefender->getUnitType());
						CvString strTemp = (pkUnitInfo != NULL)? pkUnitInfo->GetDescription() : "Unknown Unit Type";

						switch(targetType)
						{
						case AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT:
							strLogString.Format("Looking at damaging high priority %s, X: %d, Y: %d,", strTemp.GetCString(),
							                    pDefender->getX(), pDefender->getY());
							break;
						case AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT:
							strLogString.Format("Looking at damaging medium priority %s, X: %d, Y: %d,", strTemp.GetCString(),
							                    pDefender->getX(), pDefender->getY());
							break;
						case AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT:
							strLogString.Format("Looking at damaging low priority %s, X: %d, Y: %d,", strTemp.GetCString(),
							                    pDefender->getX(), pDefender->getY());
							break;
						}
						strLogString += strPlayerName;
						LogTacticalMessage(strLogString);
					}


					if(bAttackAtPoorOdds)
					{
						ExecuteAttack(pTarget, pPlot, false, false);
					}

					else
					{
						bool bMustInflictWhatWeTake = true;
						if (iExpectedDamage >= (iRequiredDamage * 40) / 100)
						{
							bMustInflictWhatWeTake = false;
						}
						ExecuteAttack(pTarget, pPlot, bMustInflictWhatWeTake, true);
					}
				}


				else
				{
					if(iExpectedDamage > iRequiredDamage)
					{

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString, strTemp, strPlayerName;
							strPlayerName = GET_PLAYER(pDefender->getOwner()).getCivilizationShortDescription();
#ifdef AUI_WARNING_FIXES
							CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pDefender->getUnitType());
							strTemp = (pkUnitInfo != NULL) ? pkUnitInfo->GetDescription() : "Unknown Unit Type";
#else
							strTemp = GC.getUnitInfo(pDefender->getUnitType())->GetDescription();
#endif
							switch(targetType)
							{
							case AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT:
								strLogString.Format("Looking at killing high priority %s, X: %d, Y: %d,", strTemp.GetCString(),
								                    pDefender->getX(), pDefender->getY());
								break;
							case AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT:
								strLogString.Format("Looking at killing medium priority %s, X: %d, Y: %d,", strTemp.GetCString(),
								                    pDefender->getX(), pDefender->getY());
								break;
							case AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT:
								strLogString.Format("Looking at killing low priority %s, X: %d, Y: %d,", strTemp.GetCString(),
								                    pDefender->getX(), pDefender->getY());
								break;
							}
							strLogString += strPlayerName;
							LogTacticalMessage(strLogString);
						}
						ExecuteAttack(pTarget, pPlot, false, (targetType != AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT));
					}
				}
			}
		}
		pTarget = GetNextZoneTarget();
	}
}


void CvTacticalAI::PlotMovesToSafety(bool bCombatUnits)
{
	int iDangerLevel;

	list<int>::iterator it;
	m_CurrentMoveUnits.clear();


#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
#else
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
#endif
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{

			CvPlot* pPlot = pUnit->plot();

			iDangerLevel = m_pPlayer->GetPlotDanger(*pPlot);
			if(iDangerLevel > 0)
			{
				bool bAddUnit = false;
				if(bCombatUnits)
				{

					if(pUnit->GetCurrHitPoints() < pUnit->GetMaxHitPoints())
					{
						if(pUnit->isBarbarian())
						{

							if(pUnit->getDomainType() == DOMAIN_SEA)
							{
								bAddUnit = true;
							}
						}


						else if (pUnit->IsUnderEnemyRangedAttack() || pUnit->GetBaseCombatStrengthConsideringDamage() * 2 <= pUnit->GetBaseCombatStrength())
						{
							bAddUnit = true;
						}
					}


					else if(!pUnit->isBarbarian())
					{
						int iAcceptableDanger;
						iAcceptableDanger = pUnit->GetBaseCombatStrengthConsideringDamage() * 100;
						if(iDangerLevel > iAcceptableDanger)
						{
							bAddUnit = true;
						}
					}
				}
				else
				{

					if(!pUnit->IsCanDefend())
					{
						bAddUnit = true;
					}
				}

				if(bAddUnit)
				{

					CvTacticalUnit unit;
					unit.SetID(pUnit->GetID());
					m_CurrentMoveUnits.push_back(unit);
				}
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteMovesToSafestPlot();
	}
}


void CvTacticalAI::PlotRepositionMoves()
{
	list<int>::iterator it;
	CvTacticalUnit unit;

	m_CurrentMoveUnits.clear();


#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
#else
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
#endif
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{

			if (pUnit->IsGreatGeneral() || pUnit->IsGreatAdmiral())
			{
				continue;
			}

			unit.SetID(pUnit->GetID());
			m_CurrentMoveUnits.push_back(unit);
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteRepositionMoves();
	}
}


void CvTacticalAI::PlotBarbarianMove(bool bAggressive)
{
	list<int>::iterator it;
	CvTacticalUnit unit;

	if(m_pPlayer->isBarbarian())
	{
		m_CurrentMoveUnits.clear();


#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
		for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
#else
		for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
#endif
		{
			UnitHandle pUnit = m_pPlayer->getUnit(*it);
			if(pUnit)
			{
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}

		if(m_CurrentMoveUnits.size() > 0)
		{
			ExecuteBarbarianMoves(bAggressive);
		}
	}
}


void CvTacticalAI::PlotBarbarianCivilianEscortMove()
{
	list<int>::iterator it;
	CvTacticalUnit unit;

	if(m_pPlayer->isBarbarian())
	{
		m_CurrentMoveUnits.clear();

#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
		for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
#else
		for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
#endif
		{
			UnitHandle pUnit = m_pPlayer->getUnit(*it);


			if(!pUnit->IsCanAttack())
			{
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}

		if(m_CurrentMoveUnits.size() > 0)
		{
			ExecuteBarbarianCivilianEscortMove();
		}
	}
}


void CvTacticalAI::PlotBarbarianPlunderTradeUnitMove(DomainTypes eDomain)
{
	AITacticalTargetType eTargetType = AI_TACTICAL_TARGET_NONE;
	bool bNavalOnly = false;
	if (eDomain == DOMAIN_LAND)
	{
		eTargetType = AI_TACTICAL_TARGET_TRADE_UNIT_LAND;
	}
	else if (eDomain == DOMAIN_SEA)
	{
		eTargetType = AI_TACTICAL_TARGET_TRADE_UNIT_SEA;
		bNavalOnly = true;
	}

	CvAssertMsg(eTargetType != AI_TACTICAL_TARGET_NONE, "Undefined domain type for target");
	if (eTargetType == AI_TACTICAL_TARGET_NONE)
	{
		return;
	}

	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(eTargetType);
	while(pTarget != NULL)
	{

		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());

		if (FindUnitsWithinStrikingDistance(pPlot, 0, 0, false                     , bNavalOnly, true                     , false                         , false                 ))
		{

			ExecutePlunderTradeUnit(pPlot);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Plundering trade unit, X: %d, Y: %d", pTarget->GetTargetX(), pTarget->GetTargetY());
				LogTacticalMessage(strLogString);
			}
		}

		pTarget = GetNextZoneTarget();
	}
}


void CvTacticalAI::PlotOperationalArmyMoves()
{
#ifndef AUI_WARNING_FIXES
	list<int>::iterator it;
#endif


	CvAIOperation* nextOp = NULL;
	nextOp = m_pPlayer->getFirstAIOperation();
	while(nextOp != NULL)
	{
		if(nextOp->GetLastTurnMoved() < GC.getGame().getGameTurn())
		{
			switch(nextOp->GetMoveType())
			{
			case AI_OPERATION_MOVETYPE_SINGLE_HEX:
				PlotSingleHexOperationMoves((CvAIEscortedOperation*)nextOp);
				break;

			case AI_OPERATION_MOVETYPE_ENEMY_TERRITORY:
				PlotEnemyTerritoryOperationMoves((CvAIEnemyTerritoryOperation*)nextOp);
				break;

			case AI_OPERATION_MOVETYPE_NAVAL_ESCORT:
				PlotNavalEscortOperationMoves((CvAINavalEscortedOperation*)nextOp);
				break;

			case AI_OPERATION_MOVETYPE_FREEFORM_NAVAL:
				PlotFreeformNavalOperationMoves((CvAINavalOperation*)nextOp);
				break;
			}

			nextOp->SetLastTurnMoved(GC.getGame().getGameTurn());
			nextOp->CheckOnTarget();
		}


		nextOp = m_pPlayer->getNextAIOperation();
	}

	bool bKilledSomething;
	do
	{
		bKilledSomething = false;
		nextOp = m_pPlayer->getFirstAIOperation();
		while(nextOp != NULL)
		{
			if(nextOp->DoDelayedDeath())
			{
				bKilledSomething = true;
				break;
			}
			nextOp = m_pPlayer->getNextAIOperation();
		}

	}
	while(bKilledSomething);
}


void CvTacticalAI::PlotPillageMoves(AITacticalTargetType eTarget, bool bFirstPass)
{
	int iPillageHeal = GC.getPILLAGE_HEAL_AMOUNT();

	CvString szTargetName = "";
	if(GC.getLogging() && GC.getAILogging())
	{
		if (eTarget == AI_TACTICAL_TARGET_CITADEL)
		{
			szTargetName = "Citadel";
		}
		else if (eTarget == AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE)
		{
			szTargetName = "Improved Resource";
		}
		else
		{
			szTargetName = "Improvement";
		}
	}

	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(eTarget);
	while(pTarget != NULL)
	{

		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());


		if (bFirstPass && FindParatroopersWithinStrikingDistance(pPlot))
		{

			ExecuteParadropPillage(pPlot);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Paratrooping in to pillage %s, X: %d, Y: %d", szTargetName.GetCString(), pTarget->GetTargetX(), pTarget->GetTargetY());
				LogTacticalMessage(strLogString);
			}

		}

		else if (bFirstPass && FindUnitsWithinStrikingDistance(pPlot, 0, iPillageHeal, false                     , false               , true                     , false                         , true                 ))
		{

			ExecutePillage(pPlot);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Pillaging %s, X: %d, Y: %d", szTargetName.GetCString(), pTarget->GetTargetX(), pTarget->GetTargetY());
				LogTacticalMessage(strLogString);
			}
		}


		else if(!bFirstPass && FindUnitsWithinStrikingDistance(pPlot, 2, iPillageHeal, false                     , false               , false                     , false                         , true                 ))
		{
			ExecuteMoveToTarget(pPlot);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Moving toward %s for pillage, X: %d, Y: %d", szTargetName.GetCString(), pTarget->GetTargetX(), pTarget->GetTargetY());
				LogTacticalMessage(strLogString);
			}
		}

		pTarget = GetNextZoneTarget();
	}
}


void CvTacticalAI::PlotPlunderTradePlotMoves (DomainTypes eDomain)
{
	AITacticalTargetType eTargetType = AI_TACTICAL_TARGET_NONE;
	bool bNavalOnly = false;
	if (eDomain == DOMAIN_LAND)
	{
		eTargetType = AI_TACTICAL_TARGET_TRADE_UNIT_LAND_PLOT;
	}
	else if (eDomain == DOMAIN_SEA)
	{
		eTargetType = AI_TACTICAL_TARGET_TRADE_UNIT_SEA_PLOT;
		bNavalOnly = true;
	}

	CvAssertMsg(eTargetType != AI_TACTICAL_TARGET_NONE, "Undefined domain type for target");
	if (eTargetType == AI_TACTICAL_TARGET_NONE)
	{
		return;
	}

	for (uint ui = 0; ui < m_CurrentMoveUnits.size(); ui++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[ui].GetID());
		if (!pUnit)
		{
			continue;
		}

		if (pUnit->getDomainType() != eDomain)
		{
			continue;
		}

		int iClosestTarget = MAX_INT;
		CvPlot* pClosestPlot = NULL;
		CvTacticalTarget* pChosenTarget = NULL;
		CvTacticalTarget* pTarget = GetFirstZoneTarget(eTargetType);
		while(pTarget != NULL)
		{
			if (pTarget->GetAuxIntData() == 0)
			{
				int iDistance = plotDistance(pUnit->getX(), pUnit->getY(), pTarget->GetTargetX(), pTarget->GetTargetY());
				if(iDistance < iClosestTarget)
				{
					pClosestPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
					iClosestTarget = iDistance;
					pChosenTarget = pTarget;
				}
			}

			pTarget = GetNextZoneTarget();
		}

		if (pClosestPlot)
		{
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pClosestPlot->getX(), pClosestPlot->getY());
			pUnit->finishMoves();
			

			UnitProcessed(m_CurrentMoveUnits[ui].GetID());


			pChosenTarget->SetAuxIntData(1);
		}
	}
}


void CvTacticalAI::PlotPlunderTradeUnitMoves (DomainTypes eDomain)
{
	AITacticalTargetType eTargetType = AI_TACTICAL_TARGET_NONE;
	bool bNavalOnly = false;
	if (eDomain == DOMAIN_LAND)
	{
		eTargetType = AI_TACTICAL_TARGET_TRADE_UNIT_LAND;
	}
	else if (eDomain == DOMAIN_SEA)
	{
		eTargetType = AI_TACTICAL_TARGET_TRADE_UNIT_SEA;
		bNavalOnly = true;
	}

	CvAssertMsg(eTargetType != AI_TACTICAL_TARGET_NONE, "Undefined domain type for target");
	if (eTargetType == AI_TACTICAL_TARGET_NONE)
	{
		return;
	}

	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(eTargetType);
	while(pTarget != NULL)
	{

		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());

		if (FindUnitsWithinStrikingDistance(pPlot, 0, 0, false                     , bNavalOnly, true                     , false                         , false                 ))
		{

			ExecutePlunderTradeUnit(pPlot);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Plundering trade unit, X: %d, Y: %d", pTarget->GetTargetX(), pTarget->GetTargetY());
				LogTacticalMessage(strLogString);
			}
		}

		pTarget = GetNextZoneTarget();
	}
}


void CvTacticalAI::PlotBlockadeImprovementMoves()
{
	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BLOCKADE_RESOURCE_POINT);
	while(pTarget != NULL)
	{

		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if(FindUnitsWithinStrikingDistance(pPlot, 2, 0, false                     , true                 , false                     ))
		{

			ExecuteNavalBlockadeMove(pPlot);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Blockading naval resource(s) with move to, X: %d, Y: %d", pTarget->GetTargetX(), pTarget->GetTargetY());
				LogTacticalMessage(strLogString);
			}
		}
		pTarget = GetNextZoneTarget();
	}
}


void CvTacticalAI::PlotCivilianAttackMoves(AITacticalTargetType eTargetType)
{
	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(eTargetType);
	while(pTarget != NULL)
	{

		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if(FindUnitsWithinStrikingDistance(pPlot, 1, 0, false                     , false               , false                     , false                         , false                 , true                      ))
		{

			ExecuteCivilianCapture(pPlot);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				switch(eTargetType)
				{
				case AI_TACTICAL_TARGET_VERY_HIGH_PRIORITY_CIVILIAN:
					strLogString.Format("Attacking very high priority civilian, X: %d, Y: %d", pTarget->GetTargetX(),
					                    pTarget->GetTargetY());
					break;
				case AI_TACTICAL_TARGET_HIGH_PRIORITY_CIVILIAN:
					strLogString.Format("Attacking high priority civilian, X: %d, Y: %d", pTarget->GetTargetX(),
					                    pTarget->GetTargetY());
					break;
				case AI_TACTICAL_TARGET_MEDIUM_PRIORITY_CIVILIAN:
					strLogString.Format("Attacking medium priority civilian, X: %d, Y: %d", pTarget->GetTargetX(),
					                    pTarget->GetTargetY());
					break;
				case AI_TACTICAL_TARGET_LOW_PRIORITY_CIVILIAN:
					strLogString.Format("Attacking low priority civilian, X: %d, Y: %d", pTarget->GetTargetX(),
					                    pTarget->GetTargetY());
					break;
				}
				LogTacticalMessage(strLogString);
			}
		}
		pTarget = GetNextZoneTarget();
	}
}


void CvTacticalAI::PlotSafeBombardMoves()
{
	CvTacticalTarget* pTarget;

	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
	while(pTarget != NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()))
	{
		m_pMap->ClearDynamicFlags();
		CvPlot* pTargetPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		m_pMap->SetTargetBombardCells(pTargetPlot, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

		ExecuteSafeBombards(*pTarget);
		pTarget = GetNextZoneTarget();
	}

	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT);
	while(pTarget != NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()))
	{
		m_pMap->ClearDynamicFlags();
		CvPlot* pTargetPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		m_pMap->SetTargetBombardCells(pTargetPlot, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

		ExecuteSafeBombards(*pTarget);
		pTarget = GetNextZoneTarget();
	}

	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT);
	while(pTarget != NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()))
	{
		m_pMap->ClearDynamicFlags();
		CvPlot* pTargetPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		m_pMap->SetTargetBombardCells(pTargetPlot, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

		ExecuteSafeBombards(*pTarget);
		pTarget = GetNextZoneTarget();
	}

	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_EMBARKED_MILITARY_UNIT);
	while(pTarget != NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()))
	{
		m_pMap->ClearDynamicFlags();
		CvPlot* pTargetPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		m_pMap->SetTargetBombardCells(pTargetPlot, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

		ExecuteSafeBombards(*pTarget);
		pTarget = GetNextZoneTarget();
	}
}


void CvTacticalAI::PlotHealMoves()
{
	list<int>::iterator it;
	m_CurrentMoveUnits.clear();
	CvTacticalUnit unit;


#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
#else
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
#endif
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{

			if(pUnit->GetCurrHitPoints() < pUnit->GetMaxHitPoints() && !pUnit->isEmbarked() && !pUnit->plot()->isCity())
			{

				if(pUnit->getDomainType() != DOMAIN_SEA || pUnit->plot()->IsFriendlyTerritory(m_pPlayer->GetID()))
				{
					if (!pUnit->IsUnderEnemyRangedAttack())
					{
						unit.SetID(pUnit->GetID());
						m_CurrentMoveUnits.push_back(unit);

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Healing at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
							LogTacticalMessage(strLogString);
						}
					}
				}
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteHeals();
	}
}


void CvTacticalAI::PlotCampDefenseMoves()
{
	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
	while(pTarget != NULL)
	{
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if(FindUnitsWithinStrikingDistance(pPlot, 1, 0, true                     , false               , false                     ))
		{
			ExecuteMoveToPlot(pPlot);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Moving to protect camp, X: %d, Y: %d", pTarget->GetTargetX(), pTarget->GetTargetY());
				LogTacticalMessage(strLogString);
			}
		}
		pTarget = GetNextZoneTarget();
	}
}


void CvTacticalAI::PlotGarrisonMoves(int iNumTurnsAway, bool bMustAllowRangedAttack)
{
	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_CITY_TO_DEFEND);
	while(pTarget != NULL)
	{
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		CvCity* pCity = pPlot->getPlotCity();

		if(pCity && pCity->GetLastTurnGarrisonAssigned() < GC.getGame().getGameTurn())
		{

			FindUnitsForThisMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GARRISON_ALREADY_THERE], pPlot, iNumTurnsAway, bMustAllowRangedAttack);

			if(m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
			{
				ExecuteMoveToTarget(pPlot);
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Garrison, X: %d, Y: %d, Priority: %d, Turns Away: %d", pTarget->GetTargetX(), pTarget->GetTargetY(), pTarget->GetAuxIntData(), iNumTurnsAway);
					if(bMustAllowRangedAttack)
					{
						strLogString += ", Allows bombard";
					}
					LogTacticalMessage(strLogString);
				}
				pCity->SetLastTurnGarrisonAssigned(GC.getGame().getGameTurn());
			}
		}
		pTarget = GetNextZoneTarget();
	}
}


void CvTacticalAI::PlotBastionMoves(int iNumTurnsAway)
{
	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_DEFENSIVE_BASTION);
	while(pTarget != NULL)
	{

		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		FindUnitsForThisMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_BASTION_ALREADY_THERE], pPlot, iNumTurnsAway);

		if(m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
		{
			ExecuteMoveToTarget(pPlot);
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Bastion, X: %d, Y: %d, Priority: %d, Turns Away: %d", pTarget->GetTargetX(), pTarget->GetTargetY(), pTarget->GetAuxIntData(), iNumTurnsAway);
				LogTacticalMessage(strLogString);
			}
		}
		pTarget = GetNextZoneTarget();
	}
}


void CvTacticalAI::PlotGuardImprovementMoves(int iNumTurnsAway)
{
	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_IMPROVEMENT_TO_DEFEND);
	while(pTarget != NULL)
	{

		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		FindUnitsForThisMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_BASTION_ALREADY_THERE], pPlot, iNumTurnsAway);

		if(m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
		{
			ExecuteMoveToTarget(pPlot);
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Guard Improvement, X: %d, Y: %d, Turns Away: %d", pTarget->GetTargetX(), pTarget->GetTargetY(), iNumTurnsAway);
				LogTacticalMessage(strLogString);
			}
		}
		pTarget = GetNextZoneTarget();
	}
}


void CvTacticalAI::PlotAncientRuinMoves(int iNumTurnsAway)
{
	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_ANCIENT_RUINS);
	while(pTarget != NULL)
	{

		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		FindUnitsForThisMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ANCIENT_RUINS], pPlot, iNumTurnsAway);

		if(m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
		{
			ExecuteMoveToTarget(pPlot);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Moving to goody hut, X: %d, Y: %d, Turns Away: %d", pTarget->GetTargetX(), pTarget->GetTargetY(), iNumTurnsAway);
				LogTacticalMessage(strLogString);
			}
		}
		pTarget = GetNextZoneTarget();
	}
}


void CvTacticalAI::PlotAirInterceptMoves()
{
	list<int>::iterator it;
	m_CurrentMoveUnits.clear();
	CvTacticalUnit unit;
	CvTacticalDominanceZone *pZone;


#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
#else
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
#endif
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{

			if(pUnit->canAirPatrol(NULL) && !m_pPlayer->GetMilitaryAI()->WillAirUnitRebase(pUnit.pointer()))
			{
				CvPlot* pUnitPlot = pUnit->plot();
				CvCity* pCity = pUnitPlot->getPlotCity();
				pZone = NULL;

				if (pCity)
				{
					pZone = m_pMap->GetZoneByCity(pCity, false);
				}
				int iNumNearbyBombers = m_pPlayer->GetMilitaryAI()->GetNumEnemyAirUnitsInRange(pUnitPlot, m_iRecruitRange, false                  , true                 );


				if (!pCity || !pZone || pZone->GetDominanceFlag() != TACTICAL_DOMINANCE_FRIENDLY)
				{
					if (iNumNearbyBombers > 0)
					{
						unit.SetID(pUnit->GetID());
						m_CurrentMoveUnits.push_back(unit);

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Ready to intercept enemy air units at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
							LogTacticalMessage(strLogString);
						}
					}
				}
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteAirInterceptMoves();
	}
}


void CvTacticalAI::PlotAirSweepMoves()
{
	list<int>::iterator it;
	m_CurrentMoveUnits.clear();
	CvTacticalUnit unit;
	CvTacticalDominanceZone *pZone;


#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
#else
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
#endif
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit && (pUnit->getDamage() * 2) < GC.getMAX_HIT_POINTS())
		{

			if(pUnit->canAirSweep() && !m_pPlayer->GetMilitaryAI()->WillAirUnitRebase(pUnit.pointer()) && m_pPlayer->GetMilitaryAI()->GetBestAirSweepTarget(pUnit.pointer()) != NULL)
			{
				CvPlot* pUnitPlot = pUnit->plot();
				CvCity* pCity = pUnitPlot->getPlotCity();
				pZone = NULL;


				if (pCity)
				{
					pZone = m_pMap->GetZoneByCity(pCity, false);
				}
				if (!pCity || !pZone || pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY)
				{
					unit.SetID(pUnit->GetID());
					m_CurrentMoveUnits.push_back(unit);

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Ready to air sweep enemy air units at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
						LogTacticalMessage(strLogString);
					}
				}
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteAirSweepMoves();
	}
}


void CvTacticalAI::PlotEmergencyPurchases()
{
	CvCity* pCity;
	UnitHandle pCityDefender;
	CvUnit* pUnit;

	if(m_pPlayer->isMinorCiv())
	{
		return;
	}


	CvTacticalDominanceZone* pZone = GC.getGame().GetTacticalAnalysisMap()->GetZone(m_iCurrentZoneIndex);
	pCity = pZone->GetClosestCity();
	if(pCity && pCity->getOwner() == m_pPlayer->GetID() && pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY && pZone->GetEnemyUnitCount() > 0)
	{

		for(unsigned int iI = 0; iI < m_ZoneTargets.size() && !pCity->isCapital(); iI++)
		{
			if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				return;
			}
		}

		m_pPlayer->GetMilitaryAI()->BuyEmergencyBuilding(pCity);


		if (pCity->plot()->getNumDefenders(m_pPlayer->GetID()) < 2)
		{
			bool bBuyNavalUnit = false;
			bool bBuyLandUnit = false;

			pCityDefender = pCity->plot()->getBestDefender(m_pPlayer->GetID());
			if (!pCityDefender)
			{
				bBuyLandUnit = true;
				if (pCity->isCoastal())
				{
					bBuyNavalUnit = true;
				}
			}
			else
			{
				if (pCityDefender->getDomainType() == DOMAIN_LAND)
				{
					if (pCity->isCoastal())
					{
						bBuyNavalUnit = true;
					}
				}
				else
				{
					bBuyLandUnit = true;
				}
			}

			if (bBuyLandUnit)
			{
				pUnit = m_pPlayer->GetMilitaryAI()->BuyEmergencyUnit(UNITAI_CITY_BOMBARD, pCity);
				if(!pUnit)
				{
					pUnit = m_pPlayer->GetMilitaryAI()->BuyEmergencyUnit(UNITAI_RANGED, pCity);
				}
			}

			if (bBuyNavalUnit)
			{
				pUnit = m_pPlayer->GetMilitaryAI()->BuyEmergencyUnit(UNITAI_ASSAULT_SEA, pCity);
				if (pUnit)
				{

					bBuyNavalUnit = false;
				}
			}


			pUnit = m_pPlayer->GetMilitaryAI()->BuyEmergencyUnit(UNITAI_ATTACK_AIR, pCity);
			if (!pUnit)
			{
				pUnit = m_pPlayer->GetMilitaryAI()->BuyEmergencyUnit(UNITAI_DEFENSE_AIR, pCity);
			}






		}
	}
}


void CvTacticalAI::PlotDefensiveAirlifts()
{
	vector<int> aProcessedUnits;
	list<int>::iterator it;
	CvTacticalUnit unit;
	CvCity* pCity;

	if(m_pPlayer->isMinorCiv())
	{
		return;
	}


	CvTacticalDominanceZone* pZone = GC.getGame().GetTacticalAnalysisMap()->GetZone(m_iCurrentZoneIndex);
	pCity = pZone->GetClosestCity();
	if(pCity && pCity->getOwner() == m_pPlayer->GetID() && pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY && pCity->CanAirlift()&& pZone->GetEnemyUnitCount() > 0)
	{

#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
		for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
#else
		for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
#endif
		{
			UnitHandle pUnit = m_pPlayer->getUnit(*it);
			if(pUnit)
			{

				for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
				{
					CvPlot *pLoopPlot = plotDirection(pCity->getX(), pCity->getY(), ((DirectionTypes)iI));
					if (pLoopPlot != NULL)
					{
						if (pUnit->canAirliftAt(pUnit->plot(), pLoopPlot->getX(), pLoopPlot->getY()))
						{
							pUnit->airlift(pLoopPlot->getX(), pLoopPlot->getY());

							aProcessedUnits.push_back(pUnit->GetID());

							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("Airlifting %s to city of %s, Current X: %d, Current Y: %d", pUnit->getName().GetCString(), pCity->getName().GetCString(), pUnit->getX(), pUnit->getY());
								LogTacticalMessage(strLogString, false);
							}

							break;
						}
					}
				}
			}
		}
	}


	vector<int>::const_iterator unitIt;
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (unitIt = aProcessedUnits.begin(); unitIt != aProcessedUnits.end(); ++unitIt)
#else
	for (unitIt = aProcessedUnits.begin(); unitIt != aProcessedUnits.end(); unitIt++)
#endif
	{
		UnitProcessed(*unitIt);
	}
}


void CvTacticalAI::PlotEscortEmbarkedMoves()
{
	list<int>::iterator it;
	m_CurrentMoveUnits.clear();
	CvTacticalUnit unit;


#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
#else
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
#endif
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{

			if(pUnit->getDomainType() == DOMAIN_SEA && pUnit->IsCombatUnit())
			{
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteEscortEmbarkedMoves();
	}
}




void CvTacticalAI::PlotSitAndBombardMoves()
{

	CvTacticalTarget target;
	CvPlot* pTarget;
	bool bAttackUnderway;
	bool bAttackMade;

	CvTacticalDominanceZone* pZone = GC.getGame().GetTacticalAnalysisMap()->GetZone(m_iCurrentZoneIndex);
	if(pZone->GetClosestCity() != NULL)
	{

		TurnOffMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_DAMAGE_CITY]);

		target.SetTargetType(AI_TACTICAL_TARGET_CITY);
		target.SetTargetPlayer(pZone->GetClosestCity()->getOwner());
		target.SetTargetX(pZone->GetClosestCity()->plot()->getX());
		target.SetTargetY(pZone->GetClosestCity()->plot()->getY());
		target.SetDominanceZone(pZone->GetDominanceZoneID());

		m_pMap->ClearDynamicFlags();
		pTarget = GC.getMap().plot(target.GetTargetX(), target.GetTargetY());
		m_pMap->SetTargetBombardCells(pTarget, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());


		bAttackUnderway = ExecuteSafeBombards(target);
		if(target.IsTargetStillAlive(m_pPlayer->GetID()))
		{
			bAttackMade = PlotCaptureCityMoves();
			if(bAttackMade)
			{
				bAttackUnderway = true;
			}
			if(target.IsTargetStillAlive(m_pPlayer->GetID()))
			{
				bAttackMade = ExecuteProtectedBombards(target, bAttackUnderway);
				if(bAttackMade)
				{
					bAttackUnderway = true;
				}
				if(target.IsTargetStillAlive(m_pPlayer->GetID()))
				{
					PlotCaptureCityMoves();
				}
			}
		}


		for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
		{

			if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
			        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
			        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
			{
				if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
				{
					m_pMap->ClearDynamicFlags();
					pTarget = GC.getMap().plot(m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY());
					m_pMap->SetTargetBombardCells(pTarget, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

					bAttackUnderway = ExecuteSafeBombards(m_ZoneTargets[iI]);
					if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
					{
						ExecuteProtectedBombards(m_ZoneTargets[iI], bAttackUnderway);
					}
				}
			}
		}

		PlotCloseOnTarget(true                    );
	}
}


void CvTacticalAI::PlotAttritFromRangeMoves()
{
	CvPlot* pTarget;
	bool bAttackUnderway;


	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{

		if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
			{
				m_pMap->ClearDynamicFlags();
				pTarget = GC.getMap().plot(m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY());
				m_pMap->SetTargetBombardCells(pTarget, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

				bAttackUnderway = ExecuteSafeBombards(m_ZoneTargets[iI]);
				if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
				{
					ExecuteProtectedBombards(m_ZoneTargets[iI], bAttackUnderway);
				}
			}
		}
	}
}


void CvTacticalAI::PlotExploitFlanksMoves()
{
	CvPlot* pTarget;
	bool bAttackUnderway = false;
	bool bAttackMade;

	m_TempTargets.clear();


#ifdef AUI_ITERATORIZE
	for (TacticalList::iterator it = m_ZoneTargets.begin(); it != m_ZoneTargets.end(); ++it)
	{
		if (it->GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
			it->GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
			it->GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			m_pMap->ClearDynamicFlags();
			pTarget = GC.getMap().plot(it->GetTargetX(), it->GetTargetY());
			m_pMap->SetTargetFlankBonusCells(pTarget);

			bAttackMade = ExecuteFlankAttack(*it);
			if (bAttackMade)
			{
				bAttackUnderway = true;
			}


			else
			{
				m_TempTargets.push_back(*it);
#else
	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			m_pMap->ClearDynamicFlags();
			pTarget = GC.getMap().plot(m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY());
			m_pMap->SetTargetFlankBonusCells(pTarget);

			bAttackMade = ExecuteFlankAttack(m_ZoneTargets[iI]);
			if(bAttackMade)
			{
				bAttackUnderway = true;
			}


			else
			{
				m_TempTargets.push_back(m_ZoneTargets[iI]);
#endif
			}
		}
	}
}


void CvTacticalAI::PlotSteamrollMoves()
{
	m_TempTargets.clear();
	bool bAttackMade;


	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
			{
				ExecutePriorityAttacksOnUnitTarget(m_ZoneTargets[iI]);
			}
		}
	}


	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, true);


	bAttackMade = PlotCaptureCityMoves();

	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, false);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, false, true);

	if(!bAttackMade)
	{
		PlotDamageCityMoves();
	}

	PlotCloseOnTarget(false                    );
}


void CvTacticalAI::PlotSurgicalCityStrikeMoves()
{
	CvTacticalTarget target;
	bool bAttackUnderway;

	CvTacticalDominanceZone* pZone = m_pMap->GetZone(m_iCurrentZoneIndex);
	if(pZone->GetClosestCity() != NULL)
	{
		target.SetTargetType(AI_TACTICAL_TARGET_CITY);
		target.SetTargetPlayer(pZone->GetClosestCity()->getOwner());
		target.SetTargetX(pZone->GetClosestCity()->plot()->getX());
		target.SetTargetY(pZone->GetClosestCity()->plot()->getY());
		target.SetDominanceZone(pZone->GetDominanceZoneID());
		bAttackUnderway = PlotCaptureCityMoves();


		for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
		{
			if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
			        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
			        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
			{
				if(plotDistance(m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY(), target.GetTargetX(), target.GetTargetY()) == 1)
				{
					if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
					{
						ExecutePriorityAttacksOnUnitTarget(m_ZoneTargets[iI]);
					}
				}
			}
		}

		if(target.IsTargetStillAlive(m_pPlayer->GetID()))
		{
			PlotDamageCityMoves();
		}


		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true);
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, true);

		PlotCloseOnTarget(false                    );
	}
}


void CvTacticalAI::PlotHedgehogMoves()
{

	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT)
		{
			if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
			{
				ExecutePriorityAttacksOnUnitTarget(m_ZoneTargets[iI]);
			}
		}
	}

	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);


	CvTacticalDominanceZone* pZone = m_pMap->GetZone(m_iCurrentZoneIndex);
	if(pZone->GetClosestCity() != NULL)
	{
		CvTacticalTarget target;
		target.SetTargetType(AI_TACTICAL_TARGET_CITY);
		target.SetTargetPlayer(pZone->GetClosestCity()->getOwner());
		target.SetTargetX(pZone->GetClosestCity()->plot()->getX());
		target.SetTargetY(pZone->GetClosestCity()->plot()->getY());
		target.SetDominanceZone(pZone->GetDominanceZoneID());
		ExecuteHedgehogDefense(target, pZone);
	}
}


void CvTacticalAI::PlotCounterattackMoves()
{

	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT)
		{
			if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
			{
				ExecutePriorityAttacksOnUnitTarget(m_ZoneTargets[iI]);
			}
		}
	}
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);


	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
			{
				ExecutePriorityAttacksOnUnitTarget(m_ZoneTargets[iI]);
			}
		}
	}
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, true);


	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, false);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, false);
}


void CvTacticalAI::PlotWithdrawMoves()
{
	list<int>::iterator it;
	m_CurrentMoveUnits.clear();
	CvTacticalUnit unit;

	CvTacticalDominanceZone* pZone = m_pMap->GetZone(m_iCurrentZoneIndex);


#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
#else
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
#endif
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{

			if(pZone->IsWater() && pUnit->getDomainType() == DOMAIN_SEA || !pZone->IsWater() && pUnit->getDomainType() == DOMAIN_LAND)
			{

				int iPlotIndex = GC.getMap().plotNum(pUnit->getX(), pUnit->getY());
				if(m_pMap->GetCell(iPlotIndex)->GetDominanceZone() == m_pMap->GetZone(m_iCurrentZoneIndex)->GetDominanceZoneID())
				{
					unit.SetID(pUnit->GetID());
					m_CurrentMoveUnits.push_back(unit);
				}


				else if(pZone->GetClosestCity() != NULL)
				{
					if(plotDistance(pZone->GetClosestCity()->getX(), pZone->GetClosestCity()->getY(), pUnit->getX(), pUnit->getY()) <= m_iRecruitRange)
					{
						CvPlot* pPlot = pUnit->plot();
						if(m_pPlayer->GetPlotDanger(*pPlot) > 0)
						{
							if(pUnit->plot()->getOwner() != pUnit->getOwner())
							{
								unit.SetID(pUnit->GetID());
								m_CurrentMoveUnits.push_back(unit);
							}
						}
					}
				}


				else if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_NO_OWNER)
				{
					CvPlot* pPlot = pUnit->plot();
					if(m_pPlayer->GetPlotDanger(*pPlot) > 0)
					{
						if(pUnit->plot()->getOwner() != pUnit->getOwner())
						{
							unit.SetID(pUnit->GetID());
							m_CurrentMoveUnits.push_back(unit);
						}
					}
				}
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteWithdrawMoves();
	}
}


void CvTacticalAI::PlotShoreBombardmentMoves()
{
	CvPlot* pTarget;

	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{

		if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
			{
				m_pMap->ClearDynamicFlags();
				pTarget = GC.getMap().plot(m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY());
				m_pMap->SetTargetBombardCells(pTarget, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

				ExecuteSafeBombards(m_ZoneTargets[iI]);
			}
		}
	}
	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_CITY)
		{
			if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
			{
				m_pMap->ClearDynamicFlags();
				pTarget = GC.getMap().plot(m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY());
				m_pMap->SetTargetBombardCells(pTarget, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

				ExecuteSafeBombards(m_ZoneTargets[iI]);
			}
		}
	}
}


void CvTacticalAI::PlotCloseOnTarget(bool bCheckDominance)
{
	CvTacticalDominanceZone* pZone = GC.getGame().GetTacticalAnalysisMap()->GetZone(m_iCurrentZoneIndex);
	if (bCheckDominance && pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_ENEMY)
	{
		return;
	}


	CvTacticalTarget target;
	if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_TEMP_ZONE)
	{
		target.SetTargetType(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
		target.SetTargetPlayer(BARBARIAN_PLAYER);
		target.SetTargetX(pZone->GetTempZoneCenter()->getX());
		target.SetTargetY(pZone->GetTempZoneCenter()->getY());
		target.SetDominanceZone(pZone->GetDominanceZoneID());

		ExecuteCloseOnTarget(target, pZone);
	}
	else if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY && pZone->GetClosestCity() != NULL)
	{
		bool bCanSeeCity = pZone->GetClosestCity()->plot()->isVisible(m_pPlayer->getTeam());


		if(!bCanSeeCity || pZone->GetFriendlyStrength() > (pZone->GetEnemyStrength() / 2))
		{
			target.SetTargetType(AI_TACTICAL_TARGET_CITY);
			target.SetTargetPlayer(pZone->GetClosestCity()->getOwner());
			target.SetTargetX(pZone->GetClosestCity()->plot()->getX());
			target.SetTargetY(pZone->GetClosestCity()->plot()->getY());
			target.SetDominanceZone(pZone->GetDominanceZoneID());

			ExecuteCloseOnTarget(target, pZone);
		}
	}
}


void CvTacticalAI::ReviewUnassignedUnits()
{
	list<int>::iterator it;


#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
#else
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
#endif
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{

			if(pUnit->isBarbarian() || pUnit->getDomainType() == DOMAIN_AIR)
			{
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
				pUnit->SetTurnProcessed(true);

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp;
					strTemp = pUnit->getUnitInfo().GetDescription();
					strLogString.Format("Unassigned %s at, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
					LogTacticalMessage(strLogString);
				}
			}
		}
	}
}




void CvTacticalAI::PlotSingleHexOperationMoves(CvAIEscortedOperation* pOperation)
{
	int iUnitID;
	UnitHandle pCivilian;
	UnitHandle pEscort;
	CvPlot* pCivilianMove;
	CvPlot* pEscortMove;
	bool bSaveMoves = false;


	if(pOperation->GetFirstArmyID() == -1)
	{
		return;
	}
	CvArmyAI* pThisArmy = m_pPlayer->getArmyAI(pOperation->GetFirstArmyID());
	iUnitID = pThisArmy->GetFirstUnitID();
	if(iUnitID != -1)
	{
		pCivilian = m_pPlayer->getUnit(iUnitID);
	}

	if(!pCivilian)
	{
		return;
	}


	if(pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE ||
	        pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP)
	{
		pEscort = m_pPlayer->getUnit(pThisArmy->GetNextUnitID());
		if(!pEscort || pEscort->TurnProcessed())
		{

			return;
		}


		if(pEscort->GeneratePath(pCivilian->plot()))
		{

			ExecuteMoveToPlot(pCivilian, pCivilian->plot());

			if(pThisArmy->GetNumSlotsFilled() > 1)
			{
				if(pEscort)
				{

					ExecuteMoveToPlot(pEscort, pCivilian->plot());
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strTemp;
						CvString strLogString;
						strTemp = GC.getUnitInfo(pEscort->getUnitType())->GetDescription();
						strLogString.Format("Moving escorting %s to civilian for operation, Civilian X: %d, Civilian Y: %d, X: %d, Y: %d", strTemp.GetCString(), pCivilian->plot()->getX(), pCivilian->plot()->getY(), pEscort->getX(), pEscort->getY());
						LogTacticalMessage(strLogString);
					}
				}
			}
		}


		else
		{
			for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				CvPlot* pPlot = plotDirection(pCivilian->getX(), pCivilian->getY(), ((DirectionTypes)iI));
				if(pPlot != NULL)
				{

					if(pEscort->canEnterTerrain(*pPlot) && pEscort->canEnterTerritory(pPlot->getTeam()))
					{
						if(pPlot->getNumUnits() == 0)
						{
							if(pEscort->GeneratePath(pPlot) && pCivilian->GeneratePath(pPlot))
							{
								ExecuteMoveToPlot(pEscort, pPlot);
								ExecuteMoveToPlot(pCivilian, pPlot);
								if(GC.getLogging() && GC.getAILogging())
								{
#ifdef AUI_WARNING_FIXES
									CvString strTemp = "";
									CvString strLogString;
									CvUnitEntry* pTempUnitInfo = GC.getUnitInfo(pEscort->getUnitType());
									if (pTempUnitInfo)
										strTemp = pTempUnitInfo->GetDescription();
#else
									CvString strTemp;
									CvString strLogString;
									strTemp = GC.getUnitInfo(pEscort->getUnitType())->GetDescription();
#endif
									strLogString.Format("Moving escorting %s to open hex, Open hex X: %d, Open hex Y: %d, X: %d, Y: %d", strTemp.GetCString(), pPlot->getX(), pPlot->getY(), pEscort->getX(), pEscort->getY());
									LogTacticalMessage(strLogString);
#ifdef AUI_WARNING_FIXES
									pTempUnitInfo = GC.getUnitInfo(pCivilian->getUnitType());
									if (pTempUnitInfo)
										strTemp = pTempUnitInfo->GetDescription();
#else
									strTemp = GC.getUnitInfo(pCivilian->getUnitType())->GetDescription();
#endif
									strLogString.Format("Moving %s to open hex, Open hex X: %d, Open hex Y: %d, X: %d, Y: %d", strTemp.GetCString(), pPlot->getX(), pPlot->getY(), pCivilian->getX(), pCivilian->getY());
									LogTacticalMessage(strLogString);
								}
								return;
							}
						}
					}
				}
			}


			pOperation->RetargetCivilian(pCivilian.pointer(), pThisArmy);
			pCivilian->finishMoves();
			pEscort->finishMoves();
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strTemp;
				CvString strLogString;
				strLogString.Format("Retargeting civilian escort operation. No empty tile adjacent to civilian to meet.");
				LogTacticalMessage(strLogString);
			}
		}
	}


	else
	{

		if(pCivilian->plot() != pOperation->GetTargetPlot())
		{

			int iFlags = 0;
			if(pThisArmy->GetNumSlotsFilled() > 1)
			{
				iFlags = MOVE_UNITS_IGNORE_DANGER;
			}


			if(!pCivilian->GeneratePath(pOperation->GetTargetPlot(), iFlags, false           ))
			{
				pOperation->RetargetCivilian(pCivilian.pointer(), pThisArmy);
				pCivilian->finishMoves();
				pEscort = m_pPlayer->getUnit(pThisArmy->GetNextUnitID());
				if(pEscort)
				{
					pEscort->finishMoves();
				}
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Retargeting civilian escort operation (path lost to target), X: %d, Y: %d", pOperation->GetTargetPlot()->getX(), pOperation->GetTargetPlot()->getY());
					LogTacticalMessage(strLogString);
				}
			}

			else
			{
				pCivilianMove = pCivilian->GetPathEndTurnPlot();
				bSaveMoves = (pCivilianMove == pOperation->GetTargetPlot());
				if(pThisArmy->GetNumSlotsFilled() == 1)
				{
					ExecuteMoveToPlot(pCivilian, pCivilianMove, bSaveMoves);
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						CvString strTemp;
						strTemp = pCivilian->getUnitInfo().GetDescription();
						strLogString.Format("Moving %s without escort to target, X: %d, Y: %d", strTemp.GetCString(), pCivilian->plot()->getX(), pCivilian->plot()->getY());
						LogTacticalMessage(strLogString);
					}
				}

				else
				{
					pEscort = m_pPlayer->getUnit(pThisArmy->GetNextUnitID());


					if(TurnsToReachTarget(pEscort, pCivilianMove) <= 1)
					{
						ExecuteMoveToPlot(pCivilian, pCivilianMove, bSaveMoves);
						ExecuteMoveToPlot(pEscort, pCivilianMove);
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							CvString strTemp;
							strTemp = pCivilian->getUnitInfo().GetDescription();
							strLogString.Format("Moving %s to target, X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
							LogTacticalMessage(strLogString);
							strTemp = pEscort->getUnitInfo().GetDescription();
							strLogString.Format("Moving escorting %s to target, X: %d, Y: %d", strTemp.GetCString(), pEscort->getX(), pEscort->getY());
							LogTacticalMessage(strLogString);
						}
					}

					else
					{
						CvTacticalAnalysisCell* pCell = GC.getGame().GetTacticalAnalysisMap()->GetCell(pCivilianMove->GetPlotIndex());
						UnitHandle pBlockingUnit = pCivilianMove->getBestDefender(m_pPlayer->GetID());



						if(pCell->IsFriendlyTurnEndTile() || !pBlockingUnit)
						{
							if(!pEscort->GeneratePath(pOperation->GetTargetPlot(), 0, false           ))
							{
								pOperation->RetargetCivilian(pCivilian.pointer(), pThisArmy);
								pCivilian->finishMoves();
								pEscort->finishMoves();
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Retargeting civilian escort operation (path lost to target), X: %d, Y: %d", pOperation->GetTargetPlot()->getX(), pOperation->GetTargetPlot()->getY());
									LogTacticalMessage(strLogString);
								}
							}
							else
							{
								pEscortMove = pCivilian->GetPathEndTurnPlot();
								bSaveMoves = (pEscortMove == pOperation->GetTargetPlot());


								if(TurnsToReachTarget(pCivilian, pEscortMove) <= 1)
								{
									ExecuteMoveToPlot(pEscort, pEscortMove);
									ExecuteMoveToPlot(pCivilian, pEscortMove, bSaveMoves);
									if(GC.getLogging() && GC.getAILogging())
									{
										CvString strLogString;
										CvString strTemp;
										strTemp = pEscort->getUnitInfo().GetDescription();
										strLogString.Format("Moving escorting %s to target, X: %d, Y: %d", strTemp.GetCString(), pEscort->getX(), pEscort->getY());
										LogTacticalMessage(strLogString);
										strTemp = pCivilian->getUnitInfo().GetDescription();
										strLogString.Format("Moving %s to target, X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
										LogTacticalMessage(strLogString);
									}
								}
								else
								{

									pOperation->RetargetCivilian(pCivilian.pointer(), pThisArmy);
									pCivilian->finishMoves();
									pEscort->finishMoves();
									if(GC.getLogging() && GC.getAILogging())
									{
										CvString strLogString;
										strLogString.Format("Retargeting civilian escort operation. Too many blocking units.");
										LogTacticalMessage(strLogString);
									}
								}
							}
						}


						else
						{
							if(ExecuteMoveOfBlockingUnit(pBlockingUnit))
							{
								ExecuteMoveToPlot(pEscort, pCivilianMove);
								ExecuteMoveToPlot(pCivilian, pCivilianMove, bSaveMoves);
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									CvString strTemp;
									strTemp = pEscort->getUnitInfo().GetDescription();
									strLogString.Format("Moving escorting %s to target, X: %d, Y: %d", strTemp.GetCString(), pEscort->getX(), pEscort->getY());
									LogTacticalMessage(strLogString);
									strTemp = pCivilian->getUnitInfo().GetDescription();
									strLogString.Format("Moving %s to target, X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
									LogTacticalMessage(strLogString);
								}
							}
							else
							{


								pOperation->RetargetCivilian(pCivilian.pointer(), pThisArmy);
								pCivilian->finishMoves();
								pEscort->finishMoves();
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Retargeting civilian escort operation. Could not move blocking unit.");
									LogTacticalMessage(strLogString);
								}
							}
						}
					}
				}
			}
		}
	}
}


void CvTacticalAI::PlotEnemyTerritoryOperationMoves(CvAIEnemyTerritoryOperation* pOperation)
{

	if(pOperation->GetFirstArmyID() == -1)
	{
		return;
	}
	CvArmyAI* pThisArmy = m_pPlayer->getArmyAI(pOperation->GetFirstArmyID());

	m_OperationUnits.clear();
	m_GeneralsToMove.clear();
	m_ParatroopersToMove.clear();
	pThisArmy->UpdateCheckpointTurns();


	if(pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE)
	{

		if(pThisArmy->GetTurnAtNextCheckpoint() == ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT)
		{
			return;
		}
		else
		{
			for(int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
			{
				CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(iI);
				if(pSlot->GetUnitID() != NO_UNIT)
				{

					UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
					if(pUnit && !pUnit->TurnProcessed())
					{

						if(pUnit->IsGreatGeneral() || pUnit->IsGreatAdmiral())
						{
							if(pUnit->getMoves() > 0)
							{
								CvOperationUnit operationUnit;
								operationUnit.SetUnitID(pUnit->GetID());
								operationUnit.SetPosition((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_CIVILIAN_SUPPORT]);
								m_GeneralsToMove.push_back(operationUnit);
							}
						}

						else
						{
							CvMultiUnitFormationInfo* pkMultiUnitFormationInfo = GC.getMultiUnitFormationInfo(pThisArmy->GetFormationIndex());
							if(pkMultiUnitFormationInfo)
							{
								const CvFormationSlotEntry& thisSlotEntry = pkMultiUnitFormationInfo->getFormationSlotEntry(iI);


								if(pSlot->HasStartedOnOperation())
								{
									MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
								}

								else
								{

									int iTurns = TurnsToReachTarget(pUnit, pOperation->GetMusterPlot(), true                , true, true);
									if(iTurns + GC.getGame().getGameTurn() >= pThisArmy->GetTurnAtNextCheckpoint())
									{
										pSlot->SetStartedOnOperation(true);
										MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
									}
								}
							}
						}
					}
				}
			}
			ExecuteGatherMoves(pThisArmy);
		}
	}


	else if(pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP)
	{
		ClearEnemiesNearArmy(pThisArmy);


		for(int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
		{
			CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(iI);
			if(pSlot->GetUnitID() != NO_UNIT)
			{
				UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
				if(pUnit && !pUnit->TurnProcessed())
				{

					if(pUnit->IsGreatGeneral() || pUnit->IsGreatAdmiral())
					{
						if(pUnit->getMoves() > 0)
						{
							CvOperationUnit operationUnit;
							operationUnit.SetUnitID(pUnit->GetID());
							operationUnit.SetPosition((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_CIVILIAN_SUPPORT]);
							m_GeneralsToMove.push_back(operationUnit);
						}
					}

					else
					{
						pSlot->SetStartedOnOperation(true);
						CvMultiUnitFormationInfo* pkMultiUnitFormationInfo = GC.getMultiUnitFormationInfo(pThisArmy->GetFormationIndex());
						if(pkMultiUnitFormationInfo)
						{
							const CvFormationSlotEntry& thisSlotEntry = pkMultiUnitFormationInfo->getFormationSlotEntry(iI);
							MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
						}
					}
				}
			}
		}
			ExecuteGatherMoves(pThisArmy);
	}


	else if(pThisArmy->GetArmyAIState() == ARMYAISTATE_MOVING_TO_DESTINATION)
	{

		CvPlot* pThisTurnTarget;
		CvPlot* pClosestCurrentCOMonPath = NULL;
		pThisTurnTarget = pOperation->ComputeCenterOfMassForTurn(pThisArmy, &pClosestCurrentCOMonPath);
		if(pThisTurnTarget == NULL)
		{
			pOperation->SetToAbort(AI_ABORT_LOST_PATH);
			return;
		}

		pThisArmy->SetXY(pThisTurnTarget->getX(), pThisTurnTarget->getY());
		ClearEnemiesNearArmy(pThisArmy);


		for(int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
		{
			CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(iI);
			if(pSlot->GetUnitID() != NO_UNIT)
			{
				UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
				if(pUnit && !pUnit->TurnProcessed())
				{

					if(pUnit->IsGreatGeneral() || pUnit->IsGreatAdmiral())
					{
						if(pUnit->getMoves() > 0)
						{
							CvOperationUnit operationUnit;
							operationUnit.SetUnitID(pUnit->GetID());
							operationUnit.SetPosition((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_CIVILIAN_SUPPORT]);
							m_GeneralsToMove.push_back(operationUnit);
						}
					}
					else
					{
						CvMultiUnitFormationInfo* pkMultiUnitFormationInfo = GC.getMultiUnitFormationInfo(pThisArmy->GetFormationIndex());
						if(pkMultiUnitFormationInfo)
						{
							const CvFormationSlotEntry& thisSlotEntry = pkMultiUnitFormationInfo->getFormationSlotEntry(iI);
							MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
						}
					}
				}
			}
		}
		ExecuteFormationMoves(pThisArmy, pClosestCurrentCOMonPath);
	}

	if(m_ParatroopersToMove.size() > 0)
	{

	}

	if(m_GeneralsToMove.size() > 0)
	{
		MoveGreatGeneral(pThisArmy);
	}
}


void CvTacticalAI::PlotNavalEscortOperationMoves(CvAINavalEscortedOperation* pOperation)
{
	CvUnit* pCivilian = 0, *pEscort = 0;
	int iUnitID;


	if(pOperation->GetFirstArmyID() == -1)
	{
		return;
	}
	CvArmyAI* pThisArmy = m_pPlayer->getArmyAI(pOperation->GetFirstArmyID());
	iUnitID = pThisArmy->GetFirstUnitID();
	if(iUnitID != -1)
	{
		pCivilian = m_pPlayer->getUnit(iUnitID);

		iUnitID = pThisArmy->GetNextUnitID();
		if(iUnitID != -1)
		{
			pEscort = m_pPlayer->getUnit(iUnitID);
			if(pEscort->getDomainType() != DOMAIN_LAND)
			{
				pEscort = NULL;
			}
		}
	}

	if(!pCivilian && pOperation->IsCivilianRequired())
	{
		return;
	}

	m_OperationUnits.clear();
	pThisArmy->UpdateCheckpointTurns();


	if(pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE)
	{

		if(pThisArmy->GetTurnAtNextCheckpoint() == ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT)
		{
			return;
		}
		else
		{
			for(int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
			{
				CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(iI);
				if(pSlot->GetUnitID() != NO_UNIT)
				{

					UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
					if(pUnit && !pUnit->TurnProcessed())
					{
						CvMultiUnitFormationInfo* pkUnitFormationInfo = GC.getMultiUnitFormationInfo(pThisArmy->GetFormationIndex());
						if (pkUnitFormationInfo)
						{
							const CvFormationSlotEntry& thisSlotEntry = pkUnitFormationInfo->getFormationSlotEntry(iI);


							if(pSlot->HasStartedOnOperation())
							{
								MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
							}

							else
							{

								int iTurns = TurnsToReachTarget(pUnit, pOperation->GetMusterPlot(), true                , true, true);
								if(iTurns + GC.getGame().getGameTurn() >= pThisArmy->GetTurnAtNextCheckpoint())
								{
									pSlot->SetStartedOnOperation(true);
									MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
								}
							}
						}
					}
				}
			}
			ExecuteNavalFormationMoves(pThisArmy, pOperation->GetMusterPlot());
		}
	}


	else if(pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP)
	{
		ExecuteFleetMoveToTarget(pThisArmy, pOperation->GetMusterPlot());
	}


	else if(pThisArmy->GetArmyAIState() == ARMYAISTATE_MOVING_TO_DESTINATION && pOperation->IsCivilianRequired())
	{

		CvPlot* pThisTurnTarget;
		pThisTurnTarget = pThisArmy->GetCenterOfMass(DOMAIN_SEA);
		if(pThisTurnTarget != NULL)
		{
			pThisArmy->SetXY(pThisTurnTarget->getX(), pThisTurnTarget->getY());
		}


		if(!pCivilian->plot()->isWater() && pCivilian->plot() == pThisArmy->GetGoalPlot())
		{
			if(pCivilian->canFound(pCivilian->plot()))
			{
				pCivilian->PushMission(CvTypes::getMISSION_FOUND());
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("City founded, At X=%d, At Y=%d", pCivilian->plot()->getX(), pCivilian->plot()->getY());
					pOperation->LogOperationSpecialMessage(strMsg);
				}
				pOperation->SetToAbort(AI_ABORT_SUCCESS);
			}
		}


		else if(TurnsToReachTarget(pCivilian, pOperation->GetTargetPlot()) <= 1 &&
		        (!pEscort || TurnsToReachTarget(pEscort, pOperation->GetTargetPlot()) <= 1))
		{
			ExecuteMoveToPlot(pCivilian, pOperation->GetTargetPlot(), true);
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strTemp;
				CvString strLogString;
				strTemp = GC.getUnitInfo(pCivilian->getUnitType())->GetDescription();
				strLogString.Format("Moving %s to target, X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
				LogTacticalMessage(strLogString);
			}

			if(pEscort)
			{
				ExecuteMoveToPlot(pEscort, pOperation->GetTargetPlot(), false);
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strTemp;
					CvString strLogString;
					strTemp = GC.getUnitInfo(pEscort->getUnitType())->GetDescription();
					strLogString.Format("Moving %s to target, X: %d, Y: %d", strTemp.GetCString(), pEscort->getX(), pEscort->getY());
					LogTacticalMessage(strLogString);
				}
			}


			for(int iI = 2; iI < pThisArmy->GetNumFormationEntries(); iI++)
			{
				CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(iI);
				if(pSlot->GetUnitID() != NO_UNIT)
				{
					UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
					if(pUnit && !pUnit->TurnProcessed())
					{
						MoveToEmptySpaceNearTarget(pUnit, pOperation->GetTargetPlot(), false           );
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strTemp;
							CvString strLogString;
							strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
							strLogString.Format("Moving %s near target, Now at X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
							LogTacticalMessage(strLogString);
						}
					}
				}
			}
		}


		else
		{
			int iBestDistance = MAX_INT;
			CvPlot* pBestPlot = NULL;


			for(int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
			{
				CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(iI);
				if(pSlot->GetUnitID() != NO_UNIT)
				{
					UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
					if(pUnit && !pUnit->TurnProcessed())
					{
						const CvFormationSlotEntry& thisSlotEntry = GC.getMultiUnitFormationInfo(pThisArmy->GetFormationIndex())->getFormationSlotEntry(iI);
						MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
					}
				}
			}


			UnitHandle pUnitAtSea = pThisArmy->GetFirstUnitInDomain(DOMAIN_SEA);
			if(!pUnitAtSea)
			{
				pOperation->RetargetCivilian(pCivilian, pThisArmy);
				for(int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
				{
					CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(iI);
					if(pSlot->GetUnitID() != NO_UNIT)
					{
						UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
						if(pUnit)
						{
							pUnit->finishMoves();
						}
					}
				}
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Retargeting naval escort operation (no unit in water!), X: %d, Y: %d", pOperation->GetTargetPlot()->getX(), pOperation->GetTargetPlot()->getY());
					LogTacticalMessage(strLogString);
				}
			}
			else
			{
				int iFlags;
				if(pThisArmy->GetUnitsOfType((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_NAVAL_ESCORT]) > 0)
				{
					iFlags = MOVE_UNITS_IGNORE_DANGER;
				}


				for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
				{
					CvPlot* pAdjacentPlot = plotDirection(pThisArmy->GetGoalX(), pThisArmy->GetGoalY(), ((DirectionTypes)iI));
					if(pAdjacentPlot != NULL && pAdjacentPlot->isWater())
					{
						if(pUnitAtSea->plot() == pAdjacentPlot)
						{
							pBestPlot = pAdjacentPlot;
							break;
						}
						else
						{

							int iDistance = GC.getStepFinder().GetStepDistanceBetweenPoints(m_pPlayer->GetID(), pOperation->GetEnemy(), pUnitAtSea->plot(), pAdjacentPlot);
							if(iDistance > 0 && iDistance < iBestDistance)
							{
								iBestDistance = iDistance;
								int iDistanceToMove = min(4, iDistance);
								PlayerTypes eEnemy = pOperation->GetEnemy();
								pBestPlot = GC.getStepFinder().GetXPlotsFromEnd(m_pPlayer->GetID(), eEnemy, pUnitAtSea->plot(), pAdjacentPlot, (iDistance - iDistanceToMove), false);
							}
						}
					}
				}

				if(pBestPlot == NULL)
				{
					pOperation->RetargetCivilian(pCivilian, pThisArmy);
					for(int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
					{
						CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(iI);
						if(pSlot->GetUnitID() != NO_UNIT)
						{
							UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
							if(pUnit)
							{
								pUnit->finishMoves();
							}
						}
					}
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Retargeting naval escort operation (path lost to target), X: %d, Y: %d", pOperation->GetTargetPlot()->getX(), pOperation->GetTargetPlot()->getY());
						LogTacticalMessage(strLogString);
					}
				}
				else
				{
					ExecuteNavalFormationMoves(pThisArmy, pBestPlot);
				}
			}
		}
	}


	else if(pThisArmy->GetArmyAIState() == ARMYAISTATE_MOVING_TO_DESTINATION && !pOperation->IsCivilianRequired())
	{
		int iBestDistance = MAX_INT;
		int iDistance;
		CvPlot *pBestPlot = NULL;
		int iSlowestMovementRate = MAX_INT;
		UnitHandle pClosestUnitAtSea;
		pBestPlot = m_pPlayer->GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pThisArmy->GetGoalPlot(), pThisArmy);


		if (pBestPlot == NULL)
		{
			pOperation->RetargetCivilian(pCivilian, pThisArmy);
			for (int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
			{
				CvArmyFormationSlot *pSlot = pThisArmy->GetFormationSlot(iI);
				if (pSlot->GetUnitID() != NO_UNIT)
				{
					UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
					if (pUnit) 
					{
						pUnit->finishMoves();
					}
				}
			}
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Retargeting naval escort operation (path lost to target), X: %d, Y: %d", pOperation->GetTargetPlot()->getX(), pOperation->GetTargetPlot()->getY());
				LogTacticalMessage(strLogString);
			}
		}

		else
		{

			iBestDistance = MAX_INT;
			for (int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
			{
				CvArmyFormationSlot *pSlot = pThisArmy->GetFormationSlot(iI);
				if (pSlot->GetUnitID() != NO_UNIT)
				{
					UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
					if (pUnit && !pUnit->TurnProcessed())
					{
						CvMultiUnitFormationInfo* pMultiUnitFormationInfo = GC.getMultiUnitFormationInfo(pThisArmy->GetFormationIndex());
						if (pMultiUnitFormationInfo)
						{
							const CvFormationSlotEntry& thisSlotEntry = pMultiUnitFormationInfo->getFormationSlotEntry(iI);
							MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);

							int iMoves = pUnit->getMoves() / GC.getMOVE_DENOMINATOR();
							if (iMoves < iSlowestMovementRate)
							{
								iSlowestMovementRate = iMoves;
							}


							iDistance = GC.getStepFinder().GetStepDistanceBetweenPoints(m_pPlayer->GetID(), pOperation->GetEnemy(), pUnit->plot(), pBestPlot);
							if (iDistance > 0 && iDistance < iBestDistance)
							{
								iBestDistance = iDistance;
								pClosestUnitAtSea = pUnit;
							}
						}
					}
				}
			}	


			if (!pClosestUnitAtSea)
			{
				pOperation->RetargetCivilian(pCivilian, pThisArmy);
				for (int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
				{
					CvArmyFormationSlot *pSlot = pThisArmy->GetFormationSlot(iI);
					if (pSlot->GetUnitID() != NO_UNIT)
					{
						UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
						if (pUnit) 
						{
							pUnit->finishMoves();
						}
					}
				}
				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Retargeting naval escort operation (no unit in water!), X: %d, Y: %d", pOperation->GetTargetPlot()->getX(), pOperation->GetTargetPlot()->getY());
					LogTacticalMessage(strLogString);
				}
			}
			else
			{

				if (iBestDistance > iSlowestMovementRate)
				{
					pBestPlot = GC.getStepFinder().GetXPlotsFromEnd(m_pPlayer->GetID(), pOperation->GetEnemy(), pClosestUnitAtSea->plot(), pBestPlot, (iBestDistance - iSlowestMovementRate), true);					
				}
				if (pBestPlot)
				{
					pThisArmy->SetXY(pBestPlot->getX(), pBestPlot->getY());
					ExecuteNavalFormationMoves(pThisArmy, pBestPlot);
				}
				else
				{
					CvAssertMsg (false, "Naval operation logic error.");
				}
			}
		}
	}
}


void CvTacticalAI::PlotFreeformNavalOperationMoves(CvAINavalOperation* pOperation)
{

	if(pOperation->GetFirstArmyID() == -1)
	{
		return;
	}
	CvArmyAI* pThisArmy = m_pPlayer->getArmyAI(pOperation->GetFirstArmyID());

	m_OperationUnits.clear();
	pThisArmy->UpdateCheckpointTurns();


	if(pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE)
	{

		if(pThisArmy->GetTurnAtNextCheckpoint() == ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT)
		{
			return;
		}
		else
		{
			for(int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
			{
				CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(iI);
				if(pSlot->GetUnitID() != NO_UNIT)
				{

					UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
					if(pUnit && !pUnit->TurnProcessed())
					{
						CvMultiUnitFormationInfo* pkMultiUnitFormationInfo = GC.getMultiUnitFormationInfo(pThisArmy->GetFormationIndex());
						if(pkMultiUnitFormationInfo)
						{
							const CvFormationSlotEntry& thisSlotEntry = pkMultiUnitFormationInfo->getFormationSlotEntry(iI);


							if(pSlot->HasStartedOnOperation())
							{
								MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
							}
							else
							{

								int iTurns = TurnsToReachTarget(pUnit, pOperation->GetMusterPlot(), true                , true, true);
								if(iTurns + GC.getGame().getGameTurn() >= pThisArmy->GetTurnAtNextCheckpoint())
								{
									pSlot->SetStartedOnOperation(true);
									MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
								}
							}
						}
					}
				}
			}
			ExecuteNavalFormationMoves(pThisArmy, pOperation->GetMusterPlot());
		}
	}


	else if(pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP)
	{

		pOperation->ArmyInPosition(pThisArmy);
		ExecuteFleetMoveToTarget(pThisArmy, pOperation->GetTargetPlot());
	}


	else if(pThisArmy->GetArmyAIState() == ARMYAISTATE_MOVING_TO_DESTINATION)
	{

		pOperation->ArmyInPosition(pThisArmy);
		ExecuteFleetMoveToTarget(pThisArmy, pOperation->GetTargetPlot());
	}
}


void CvTacticalAI::ClearEnemiesNearArmy(CvArmyAI* pArmy)
{
	bool bEnemyNear = false;
	CvPlot* pPlot;
	int iRange = 1;
	bool bAttackUnderway;
	bool bAttackMade = false;
	UnitHandle pUnit;


	for(unsigned int iI = 0; iI < m_AllTargets.size(); iI++)
	{

		if(m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
		        m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{

			if(plotDistance(pArmy->GetX(), pArmy->GetY(), m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY()) <= iRange)
			{
				bEnemyNear = true;
				break;
			}
		}
	}

	if(bEnemyNear)
	{

		pUnit = pArmy->GetFirstUnit();
		while(pUnit)
		{
			if(!pUnit->TurnProcessed() && !pUnit->isDelayedDeath() && pUnit->canMove())
			{
				if(std::find(m_CurrentTurnUnits.begin(), m_CurrentTurnUnits.end(), pUnit->GetID()) == m_CurrentTurnUnits.end())
				{
					m_CurrentTurnUnits.push_back(pUnit->GetID());
				}
			}
			pUnit = pArmy->GetNextUnit();
		}


		for(unsigned int iI = 0; iI < m_AllTargets.size(); iI++)
		{

			if(m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
			        m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
			        m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
			{
				if(m_AllTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
				{

					if(plotDistance(pArmy->GetX(), pArmy->GetY(), m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY()) <= iRange)
					{
						pPlot = GC.getMap().plot(m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY());

						m_pMap->ClearDynamicFlags();
						m_pMap->SetTargetBombardCells(pPlot, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

						bAttackUnderway = ExecuteSafeBombards(m_AllTargets[iI]);
						if(m_AllTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
						{
							bAttackMade = ExecuteProtectedBombards(m_AllTargets[iI], bAttackUnderway);
						}
						if(bAttackMade)
						{
							bAttackUnderway = true;
						}
						if(m_AllTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
						{
							UnitHandle pDefender = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
							if(pDefender)
							{
								m_AllTargets[iI].SetAuxIntData(pDefender->GetCurrHitPoints());
								m_CurrentMoveCities.clear();
								if(FindUnitsWithinStrikingDistance(pPlot, 1, 0, false                     ))
								{
									ComputeTotalExpectedDamage(&m_AllTargets[iI], pPlot);
									ExecuteAttack(&m_AllTargets[iI], pPlot, true                       , true                       );
								}
							}
						}
					}
				}
			}
		}
	}
}


void CvTacticalAI::MoveWithFormation(UnitHandle pUnit, MultiunitPositionTypes ePosition)
{
	if(pUnit->getMoves() > 0)
	{
		CvOperationUnit operationUnit;
		operationUnit.SetUnitID(pUnit->GetID());
		operationUnit.SetPosition(ePosition);
		m_OperationUnits.push_back(operationUnit);
	}
}


void CvTacticalAI::ExecuteGatherMoves(CvArmyAI* pArmy)
{
	CvPlot* pTarget;
	UnitHandle pUnit;
	int iUnits;
	AITacticalTargetType eTargetType;
	CvPlot* pLoopPlot;

	if(m_OperationUnits.size() == 0)
	{
		return;
	}

	pTarget = pArmy->Plot();


	iUnits = m_OperationUnits.size();


	int iRange = OperationalAIHelpers::GetGatherRangeForXUnits(iUnits);


	bool bFoundEnoughDeploymentPlots = false;
	if (ScoreDeploymentPlots(pTarget, pArmy, iUnits, 0, iRange))
	{

		if (m_TempTargets.size() >= (unsigned)(iUnits * 2))
		{
			bFoundEnoughDeploymentPlots = true;
		}
		else
		{
			m_TempTargets.clear();
			iRange = 3;
		}
	}

	if (!bFoundEnoughDeploymentPlots)
	{
		if (!ScoreDeploymentPlots(pTarget, pArmy, iUnits, 0, iRange))
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Operation aborting. Army ID: %d. Not enough spaces to deploy near target", pArmy->GetID());
				LogTacticalMessage(strLogString);
			}
			m_pPlayer->getAIOperation(pArmy->GetOperationID())->SetToAbort(AI_ABORT_NO_ROOM_DEPLOY);
			return;
		}
	}


	std::stable_sort(m_TempTargets.begin(), m_TempTargets.end());
	m_PotentialBlocks.clear();
	bool bDone = false;

	int iUnitsToPlace = iUnits;

#ifdef AUI_ITERATORIZE
	for (FStaticVector<CvTacticalTarget, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::const_iterator it = m_TempTargets.begin(); it != m_TempTargets.end() && !bDone; ++it)
	{
		eTargetType = it->GetTargetType();

		pLoopPlot = GC.getMap().plot(it->GetTargetX(), it->GetTargetY());
#else
	for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
	{
		eTargetType = m_TempTargets[iI].GetTargetType();

		pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
#endif
		

		UnitHandle pUnitAlreadyThere = pLoopPlot->getBestDefender(NO_PLAYER);
		if (!pUnitAlreadyThere || pUnitAlreadyThere->getArmyID() == pArmy->GetID())
		{
			if(FindClosestOperationUnit(pLoopPlot, true                   , false                      ))
			{
#ifdef AUI_ITERATORIZE
				for (std::vector<CvTacticalUnit>::const_iterator it2 = m_CurrentMoveUnits.begin(); it2 != m_CurrentMoveUnits.end(); ++it2)
				{
					CvBlockingUnit block;
					block.SetUnitID(it2->GetID());
					block.SetPlot(pLoopPlot);
					block.SetNumChoices((int)m_CurrentMoveUnits.size());
					block.SetDistanceToTarget(it2->GetMovesToTarget());
#else
				for(unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
				{
					CvBlockingUnit block;
					block.SetUnitID(m_CurrentMoveUnits[jJ].GetID());
					block.SetPlot(pLoopPlot);
					block.SetNumChoices((int)m_CurrentMoveUnits.size());
					block.SetDistanceToTarget(m_CurrentMoveUnits[jJ].GetMovesToTarget());
#endif
					m_PotentialBlocks.push_back(block);
				}
				iUnitsToPlace--;
				if(iUnitsToPlace == 0)
				{
					bDone = true;
				}	
			}
		}
	}


	AssignDeployingUnits(iUnits - iUnitsToPlace);

	PerformChosenMoves();


	if(m_ChosenBlocks.size() < (unsigned int)(iUnits))
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("No gather move for %d units", iUnits - m_ChosenBlocks.size());
			LogTacticalMessage(strMsg);
		}
	}
}


void CvTacticalAI::ExecuteFormationMoves(CvArmyAI* pArmy, CvPlot *pClosestCurrentCOMonPath)
{
	CvPlot* pTarget;
	UnitHandle pUnit;
	AITacticalTargetType eTargetType;
	CvPlot* pLoopPlot;
	FStaticVector<CvOperationUnit, SAFE_ESTIMATE_NUM_MULTIUNITFORMATION_ENTRIES, true, c_eCiv5GameplayDLL, 0>::iterator it;

	if(m_OperationUnits.size() == 0)
	{
		return;
	}

	pTarget = pArmy->Plot();

	int iMeleeUnits = 0;
	int iRangedUnits = 0;
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_OperationUnits.begin(); it != m_OperationUnits.end(); ++it)
#else
	for(it = m_OperationUnits.begin(); it != m_OperationUnits.end(); it++)
#endif
	{
		CvUnit *pOpUnit = m_pPlayer->getUnit(it->GetUnitID());
		if (pOpUnit->IsCanAttackRanged())
		{
			iRangedUnits++;
		}
		else
		{
			iMeleeUnits++;
		}
	}


	if(!ScoreFormationPlots(pArmy, pTarget, pClosestCurrentCOMonPath, iMeleeUnits + iRangedUnits))
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Operation aborting. Army ID: %d. Not enough spaces to deploy along formation's path", pArmy->GetID());
			LogTacticalMessage(strLogString);
		}
		m_pPlayer->getAIOperation(pArmy->GetOperationID())->SetToAbort(AI_ABORT_NO_ROOM_DEPLOY);
	}


	else
	{
		std::stable_sort(m_TempTargets.begin(), m_TempTargets.end());


		int iMeleeUnitsToPlace = iMeleeUnits;
		bool bDone = false;
#ifdef AUI_ITERATORIZE
		for (FStaticVector<CvTacticalTarget, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::const_iterator it2 = m_TempTargets.begin(); it2 != m_TempTargets.end() && !bDone; ++it2)
		{
			eTargetType = it2->GetTargetType();

			pLoopPlot = GC.getMap().plot(it2->GetTargetX(), it2->GetTargetY());
#else
		for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			eTargetType = m_TempTargets[iI].GetTargetType();

			pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
#endif


			if (!pLoopPlot->getBestDefender(NO_PLAYER))
			{
				if(FindClosestOperationUnit(pLoopPlot, false                   , false                      ))
				{
#ifdef AUI_ITERATORIZE
					UnitHandle pInnerUnit = m_pPlayer->getUnit(m_CurrentMoveUnits.begin()->GetID());
#else
					UnitHandle pInnerUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
#endif
					bool bMoveWasSafe;
					MoveToUsingSafeEmbark(pInnerUnit, pLoopPlot, bMoveWasSafe);
					pInnerUnit->finishMoves();
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Deploying melee unit, %s, To X: %d, To Y: %d, At X: %d, At Y: %d", pInnerUnit->getName().GetCString(), pLoopPlot->getX(), pLoopPlot->getY(), pInnerUnit->getX(), pInnerUnit->getY());
						LogTacticalMessage(strMsg);
					}
					iMeleeUnitsToPlace--;
				}
			}
			if (iMeleeUnitsToPlace == 0)
			{
				bDone = true;
			}
		}


		if (iMeleeUnitsToPlace > 0)
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format("No army deployment move for %d melee units", iMeleeUnitsToPlace);
				LogTacticalMessage(strMsg);
			}
		}


		int iRangedUnitsToPlace = iRangedUnits;
		bDone = false;
#ifdef AUI_ITERATORIZE
		for (FStaticVector<CvTacticalTarget, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::const_iterator it2 = m_TempTargets.begin(); it2 != m_TempTargets.end() && !bDone; ++it2)
		{
			eTargetType = it2->GetTargetType();

			pLoopPlot = GC.getMap().plot(it2->GetTargetX(), it2->GetTargetY());
#else
		for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			eTargetType = m_TempTargets[iI].GetTargetType();

			pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
#endif
			if (eTargetType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{

				if (!pLoopPlot->getBestDefender(NO_PLAYER))
				{
					if(FindClosestOperationUnit(pLoopPlot, true                   , true                      ))
					{
#ifdef AUI_ITERATORIZE
						UnitHandle pInnerUnit = m_pPlayer->getUnit(m_CurrentMoveUnits.begin()->GetID());
#else
						UnitHandle pInnerUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
#endif
						bool bMoveWasSafe;
						MoveToUsingSafeEmbark(pInnerUnit, pLoopPlot, bMoveWasSafe);
						pInnerUnit->finishMoves();
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Deploying ranged unit, %s, To X: %d, To Y: %d, At X: %d, At Y: %d", pInnerUnit->getName().GetCString(), pLoopPlot->getX(), pLoopPlot->getY(), pInnerUnit->getX(), pInnerUnit->getY());
							LogTacticalMessage(strMsg);
						}
						iRangedUnitsToPlace--;
					}
				}
			}
			if (iRangedUnitsToPlace == 0)
			{
				bDone = true;
			}
		}


#ifdef AUI_ITERATORIZE
		for (FStaticVector<CvTacticalTarget, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::const_iterator it2 = m_TempTargets.begin(); it2 != m_TempTargets.end() && !bDone; ++it2)
		{
			eTargetType = it2->GetTargetType();

			pLoopPlot = GC.getMap().plot(it2->GetTargetX(), it2->GetTargetY());
#else
		for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			eTargetType = m_TempTargets[iI].GetTargetType();

			pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
#endif


			if (!pLoopPlot->getBestDefender(NO_PLAYER))
			{
				if(FindClosestOperationUnit(pLoopPlot, true                   , true                      ))
				{
#ifdef AUI_ITERATORIZE
					UnitHandle pInnerUnit = m_pPlayer->getUnit(m_CurrentMoveUnits.begin()->GetID());
#else
					UnitHandle pInnerUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
#endif
					bool bMoveWasSafe;
					MoveToUsingSafeEmbark(pInnerUnit, pLoopPlot, bMoveWasSafe);
					pInnerUnit->finishMoves();
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Deploying ranged unit (Pass 2), %s, To X: %d, To Y: %d, At X: %d, At Y: %d", pInnerUnit->getName().GetCString(), pLoopPlot->getX(), pLoopPlot->getY(), pInnerUnit->getX(), pInnerUnit->getY());
						LogTacticalMessage(strMsg);
					}
					iRangedUnitsToPlace--;
				}
			}
			if (iRangedUnitsToPlace == 0)
			{
				bDone = true;
			}
		}


		if (iRangedUnitsToPlace > 0)
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format("No army deployment move for %d ranged units", iRangedUnitsToPlace);
				LogTacticalMessage(strMsg);
			}
		}
	}
}


bool CvTacticalAI::ScoreDeploymentPlots(CvPlot* pTarget, CvArmyAI* pArmy, int iNumMeleeUnits, int iNumRangedUnits, int iRange)
{
	int iDX, iDY;
	int iScore;
	CvPlot* pPlot;
	CvTacticalAnalysisCell* pCell;
	bool bSafeForDeployment;
	bool bForcedToUseWater;
	int iNumSafePlotsFound = 0;
	int iNumDeployPlotsFound = 0;
	CvTacticalTarget target;


	m_TempTargets.clear();

	for(iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for(iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pPlot = plotXY(pTarget->getX(), pTarget->getY(), iDX, iDY);
			if(pPlot != NULL)
			{
				bSafeForDeployment = true;
				bForcedToUseWater = false;

				int iPlotDistance = plotDistance(pPlot->getX(), pPlot->getY(), pTarget->getX(), pTarget->getY());
				if(iPlotDistance <= iRange)
				{
					int iPlotIndex = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
					pCell = m_pMap->GetCell(iPlotIndex);
					CvAIOperation* pOperation = m_pPlayer->getAIOperation(pArmy->GetOperationID());

					bool bValid = false;
					if(pOperation->IsMixedLandNavalOperation() && pCell->CanUseForOperationGatheringCheckWater(true           ))
					{
						bValid = true;
					}

					else if(pOperation->IsAllNavalOperation() && pCell->CanUseForOperationGatheringCheckWater(true           ))
					{
						bValid = true;
					}

					else if((!pOperation->IsAllNavalOperation() && !pOperation->IsMixedLandNavalOperation()) && (pCell->CanUseForOperationGatheringCheckWater(false           ) || GC.getMap().GetAIMapHint() & 1))
					{
						bValid = true;
						if (pCell->IsWater())
						{
							bForcedToUseWater = true;
						}
					}

					if(pOperation->IsMixedLandNavalOperation() || pOperation->IsAllNavalOperation())
					{
						if(!pArmy->IsAllOceanGoing() && pCell->IsOcean())
						{
							bValid = false;
						}
					}

					if(bValid)
					{

						CvUnit* pFriendlyUnit;
						pFriendlyUnit = pCell->GetFriendlyMilitaryUnit();
						if(pFriendlyUnit != NULL)
						{
							if(pFriendlyUnit->getArmyID() != pArmy->GetID())
							{
								continue;
							}
						}

						iNumDeployPlotsFound++;
						iScore = 600 - (iPlotDistance * 100);
						if(pCell->IsSubjectToAttack())
						{
							iScore -= 100;
							bSafeForDeployment = false;
						}
						else
						{
							iNumSafePlotsFound++;
						}
						if(pCell->IsEnemyCanMovePast())
						{
							iScore -= 100;
						}
						if(pPlot->isCity() && pPlot->getOwner() == m_pPlayer->GetID())
						{
							iScore += 100;
						}
						else
						{
							iScore += pCell->GetDefenseModifier() * 2;
						}
						if (bForcedToUseWater)
						{
							iScore = 10;
						}

						pCell->SetSafeForDeployment(bSafeForDeployment);
						pCell->SetDeploymentScore(iScore);


						target.SetTargetX(pPlot->getX());
						target.SetTargetY(pPlot->getY());
						target.SetAuxIntData(iScore);


						if(bSafeForDeployment)
						{
							target.SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
						}
						else
						{
							target.SetTargetType(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT);
						}

						m_TempTargets.push_back(target);
					}
				}
			}
		}
	}


	if(iNumSafePlotsFound < iNumRangedUnits || iNumDeployPlotsFound < (iNumMeleeUnits + iNumRangedUnits))
	{
		return false;
	}

	return true;
}


bool CvTacticalAI::ScoreFormationPlots(CvArmyAI* pArmy, CvPlot* pForwardTarget, CvPlot *pCurrentCOM, int iNumUnits)
{
	int iDX, iDY;
	int iScore;
	CvPlot* pPlot;
	CvTacticalAnalysisCell* pCell;
	int iNumDeployPlotsFound = 0;
	CvTacticalTarget target;
	bool bForcedToUseWater;


	m_TempTargets.clear();

	int iRange = 3;
	for(iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for(iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pPlot = plotXY(pForwardTarget->getX(), pForwardTarget->getY(), iDX, iDY);
			if(pPlot != NULL)
			{
				bForcedToUseWater = false;

				int iPlotDistance = plotDistance(pPlot->getX(), pPlot->getY(), pForwardTarget->getX(), pForwardTarget->getY());
				if(iPlotDistance <= iRange)
				{
					int iPlotIndex = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
					pCell = m_pMap->GetCell(iPlotIndex);
					CvAIOperation* pOperation = m_pPlayer->getAIOperation(pArmy->GetOperationID());

					bool bValid = false;
					if(pOperation->IsMixedLandNavalOperation() && pCell->CanUseForOperationGatheringCheckWater(true           ))
					{
						bValid = true;
					}

					else if(pOperation->IsAllNavalOperation() && pCell->CanUseForOperationGatheringCheckWater(true           ))
					{
						bValid = true;
					}

					else if((!pOperation->IsAllNavalOperation() && !pOperation->IsMixedLandNavalOperation()) && (pCell->CanUseForOperationGatheringCheckWater(false           ) || GC.getMap().GetAIMapHint() & 1))
					{
						bValid = true;
						if (pCell->IsWater())
						{
							bForcedToUseWater = true;
						}
					}

					if(pOperation->IsMixedLandNavalOperation() || pOperation->IsAllNavalOperation())
					{
						if(!pArmy->IsAllOceanGoing() && pCell->IsOcean())
						{
							bValid = false;
						}
					}

					if(bValid)
					{

						CvUnit* pFriendlyUnit;
						pFriendlyUnit = pCell->GetFriendlyMilitaryUnit();
						if(pFriendlyUnit != NULL)
						{
							if(pFriendlyUnit->getArmyID() != pArmy->GetID())
							{
								continue;
							}
						}

						iNumDeployPlotsFound++;


						int iPlotDistanceFromCOM;
						if (pForwardTarget == pCurrentCOM)
						{
							iPlotDistanceFromCOM = iPlotDistance;
						}
						else
						{
							iPlotDistanceFromCOM = plotDistance(pPlot->getX(), pPlot->getY(), pCurrentCOM->getX(), pCurrentCOM->getY());
						}

						iScore = 800 - (iPlotDistance * 100) - (iPlotDistanceFromCOM * 100);

						if(pPlot->isCity() && pPlot->getOwner() == m_pPlayer->GetID())
						{
							iScore += 100;
						}
						else
						{
							iScore += pCell->GetDefenseModifier() * 2;
						}
						if (bForcedToUseWater)
						{
							iScore = 10;
						}


						bool bSafeForDeployment = iPlotDistanceFromCOM < iPlotDistance || pForwardTarget == pCurrentCOM;
						pCell->SetSafeForDeployment(bSafeForDeployment);


						if (bSafeForDeployment)
						{
							iScore -= 200;
						}

						pCell->SetDeploymentScore(iScore);


						target.SetTargetX(pPlot->getX());
						target.SetTargetY(pPlot->getY());
						target.SetAuxIntData(iScore);

						if(bSafeForDeployment)
						{
							target.SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
						}
						else
						{
							target.SetTargetType(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT);
						}
						m_TempTargets.push_back(target);
					}
				}
			}
		}
	}


	if(iNumDeployPlotsFound < iNumUnits)
	{
		return false;
	}

	return true;
}


void CvTacticalAI::ExecuteNavalFormationMoves(CvArmyAI* pArmy, CvPlot* pTurnTarget)
{
	UnitHandle pUnit;
	int iMostUnits;
	int iLeastUnits;
	CvPlot* pLoopPlot;
	bool bMoreEscorted = true;
	FStaticVector<CvOperationUnit, SAFE_ESTIMATE_NUM_MULTIUNITFORMATION_ENTRIES, true, c_eCiv5GameplayDLL, 0>::iterator it;

	if(m_OperationUnits.size() == 0)
	{
		return;
	}

	int iNavalUnits = 0;
	int iEscortedUnits = 0;
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_OperationUnits.begin(); it != m_OperationUnits.end(); ++it)
#else
	for(it = m_OperationUnits.begin(); it != m_OperationUnits.end(); it++)
#endif
	{
		CvUnit *pOpUnit = m_pPlayer->getUnit(it->GetUnitID());
		if (pOpUnit)
		{
			if (pOpUnit->getDomainType() == DOMAIN_LAND)
			{
				iEscortedUnits++;
			}
			else
			{
				if (pOpUnit->IsGreatAdmiral())
				{
					iEscortedUnits++;
				}
				else
				{
					iNavalUnits++;
				}
			}
		}
	}

	iMostUnits = max(iNavalUnits, iEscortedUnits);
	iLeastUnits = min(iNavalUnits, iEscortedUnits);
	if(iNavalUnits > iEscortedUnits)
	{
		bMoreEscorted = false;
	}


	int iRange = OperationalAIHelpers::GetGatherRangeForXUnits(iMostUnits);


	if(!ScoreDeploymentPlots(pTurnTarget, pArmy, iMostUnits, 0, iRange) &&
	        !ScoreDeploymentPlots(pTurnTarget, pArmy, iMostUnits, 0, 3))
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Operation aborting. Army ID: %d. Not enough spaces to deploy near turn target", pArmy->GetID());
			LogTacticalMessage(strLogString);
		}
		m_pPlayer->getAIOperation(pArmy->GetOperationID())->SetToAbort(AI_ABORT_NO_ROOM_DEPLOY);
	}


	else
	{
		std::stable_sort(m_TempTargets.begin(), m_TempTargets.end());
		m_PotentialBlocks.clear();
		bool bDone = false;
		int iMostUnitsToPlace = iMostUnits;

#ifdef AUI_ITERATORIZE
		for (FStaticVector<CvTacticalTarget, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::const_iterator it2 = m_TempTargets.begin(); it2 != m_TempTargets.end() && !bDone; ++it2)
		{
			pLoopPlot = GC.getMap().plot(it2->GetTargetX(), it2->GetTargetY());
#else
		for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
#endif
			if(FindClosestNavalOperationUnit(pLoopPlot, bMoreEscorted))
			{
#ifdef AUI_ITERATORIZE
				for (std::vector<CvTacticalUnit>::const_iterator it3 = m_CurrentMoveUnits.begin(); it3 != m_CurrentMoveUnits.end(); ++it3)
				{
					CvBlockingUnit block;
					block.SetUnitID(it3->GetID());
					block.SetPlot(pLoopPlot);
					block.SetNumChoices((int)m_CurrentMoveUnits.size());
					block.SetDistanceToTarget(it3->GetMovesToTarget());
#else
				for(unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
				{
					CvBlockingUnit block;
					block.SetUnitID(m_CurrentMoveUnits[jJ].GetID());
					block.SetPlot(pLoopPlot);
					block.SetNumChoices((int)m_CurrentMoveUnits.size());
					block.SetDistanceToTarget(m_CurrentMoveUnits[jJ].GetMovesToTarget());
#endif
					m_PotentialBlocks.push_back(block);
				}
				iMostUnitsToPlace--;
				if(iMostUnitsToPlace == 0)
				{
					bDone = true;
				}
			}
		}


		AssignDeployingUnits(iMostUnits - iMostUnitsToPlace);
		PerformChosenMoves(2);


		if(m_ChosenBlocks.size() < (unsigned int)iMostUnits)
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format("No naval deployment move for %d units in first pass", iMostUnits - m_ChosenBlocks.size());
				LogTacticalMessage(strMsg);
			}
		}
		
		if(iLeastUnits > 0)
		{

			m_TempTargets.clear();
			CvTacticalTarget temp;

#ifdef AUI_ITERATORIZE
			for (FStaticVector<CvBlockingUnit, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::const_iterator it2 = m_ChosenBlocks.begin(); it2 != m_ChosenBlocks.end(); ++it2)
			{
				temp.SetTargetX(it2->GetPlot()->getX());
				temp.SetTargetY(it2->GetPlot()->getY());
#else
			for(unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
			{
				temp.SetTargetX(m_ChosenBlocks[iI].GetPlot()->getX());
				temp.SetTargetY(m_ChosenBlocks[iI].GetPlot()->getY());
#endif
				m_TempTargets.push_back(temp);
			}

			m_PotentialBlocks.clear();
			bDone = false;
			int iLeastUnitsToPlace = iLeastUnits;

#ifdef AUI_ITERATORIZE
			for (FStaticVector<CvTacticalTarget, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::const_iterator it2 = m_TempTargets.begin(); it2 != m_TempTargets.end() && !bDone; ++it2)
			{
				pLoopPlot = GC.getMap().plot(it2->GetTargetX(), it2->GetTargetY());
#else
			for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
			{
				pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
#endif
				if(FindClosestNavalOperationUnit(pLoopPlot, !bMoreEscorted))
				{
#ifdef AUI_ITERATORIZE
					for (std::vector<CvTacticalUnit>::const_iterator it3 = m_CurrentMoveUnits.begin(); it3 != m_CurrentMoveUnits.end(); ++it3)
					{
						CvBlockingUnit block;
						block.SetUnitID(it3->GetID());
						block.SetPlot(pLoopPlot);
						block.SetNumChoices((int)m_CurrentMoveUnits.size());
						block.SetDistanceToTarget(it3->GetMovesToTarget());
#else
					for(unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
					{
						CvBlockingUnit block;
						block.SetUnitID(m_CurrentMoveUnits[jJ].GetID());
						block.SetPlot(pLoopPlot);
						block.SetNumChoices((int)m_CurrentMoveUnits.size());
						block.SetDistanceToTarget(m_CurrentMoveUnits[jJ].GetMovesToTarget());
#endif
						m_PotentialBlocks.push_back(block);
					}
					iLeastUnitsToPlace--;
					if(iLeastUnitsToPlace == 0)
					{
						bDone = true;
					}
				}
			}


			AssignDeployingUnits(iLeastUnits - iLeastUnitsToPlace);
			PerformChosenMoves(2);

			if(m_ChosenBlocks.size() < (unsigned int)iLeastUnits)
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("No naval deployment move for %d units in second pass", iLeastUnits - m_ChosenBlocks.size());
					LogTacticalMessage(strMsg);
				}
			}
		}
	}
}


bool CvTacticalAI::PlotEscortNavalOperationMoves(CvArmyAI* pArmy)
{
	CvPlot* pTarget = NULL;
	int iOperationID;
	int iArmyID;
	CvArmyAI* pArmyToEscort;


	if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_COLONIZE, &iOperationID))
	{
		CvAIOperation* pOperation = m_pPlayer->getAIOperation(iOperationID);
		iArmyID = pOperation->GetFirstArmyID();
		if(iArmyID != -1)
		{
			pArmyToEscort = m_pPlayer->getArmyAI(iArmyID);
			if(pArmyToEscort)
			{
				pTarget = pArmyToEscort->GetCenterOfMass(DOMAIN_SEA);
			}
		}
	}

	if(pTarget)
	{
		ExecuteFleetMoveToTarget(pArmy, pTarget);
		return true;
	}

	return false;
}


void CvTacticalAI::ExecuteFleetMoveToTarget(CvArmyAI* pArmy, CvPlot* pTarget)
{

	for(int iI = 0; iI < pArmy->GetNumFormationEntries(); iI++)
	{
		CvArmyFormationSlot* pSlot = pArmy->GetFormationSlot(iI);
		if(pSlot->GetUnitID() != NO_UNIT)
		{
			UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
			if(pUnit && !pUnit->TurnProcessed())
			{
				pSlot->SetStartedOnOperation(true);

				CvMultiUnitFormationInfo* pkMultiUnitFormation = GC.getMultiUnitFormationInfo(pArmy->GetFormationIndex());
				if(pkMultiUnitFormation)
				{
					const CvFormationSlotEntry& thisSlotEntry = pkMultiUnitFormation->getFormationSlotEntry(iI);
					MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
				}
			}
		}
	}
	ExecuteNavalFormationMoves(pArmy, pTarget);
}




void CvTacticalAI::IdentifyPriorityTargets()
{
	CvCity* pLoopCity;
	int iCityLoop;
	FFastVector<CvTacticalTarget> possibleAttackers;
	int iExpectedDamage;
	int iExpectedTotalDamage;


	for(pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
	{

		possibleAttackers.clear();
		iExpectedTotalDamage = 0;

		CvTacticalTarget* pTarget;
		pTarget = GetFirstUnitTarget();
		while(pTarget != NULL)
		{
			CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
			UnitHandle pEnemyUnit = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
			if(pEnemyUnit)
			{
				iExpectedDamage = 0;
#if !defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
				if (pEnemyUnit->IsCanAttackRanged() && pEnemyUnit->GetMaxRangedCombatStrength(NULL,           NULL, true, true) > pEnemyUnit->GetMaxAttackStrength(NULL, pLoopCity->plot(), NULL))
#else
				CvCombatInfo kCombatInfo;
				kCombatInfo.setUnit(BATTLE_UNIT_ATTACKER, pEnemyUnit.pointer());
				kCombatInfo.setCity(BATTLE_UNIT_DEFENDER, pLoopCity);
				kCombatInfo.setPlot(pLoopCity->plot());
				kCombatInfo.setAttackIsRanged(true);
				if (pEnemyUnit->IsCanAttackRanged() && pEnemyUnit->GetMaxRangedCombatStrength(kCombatInfo) > pEnemyUnit->GetMaxAttackStrength(kCombatInfo))
#endif
				{
					if(plotDistance(pEnemyUnit->getX(), pEnemyUnit->getY(), pLoopCity->getX(), pLoopCity->getY()) <= pEnemyUnit->GetRange())
					{
						if(pEnemyUnit->canEverRangeStrikeAt(pLoopCity->getX(), pLoopCity->getY()))
						{
							iExpectedDamage = pEnemyUnit->GetRangeCombatDamage(NULL, pLoopCity, false);
						}
					}
				}
				else if(CanReachInXTurns(pEnemyUnit, pLoopCity->plot(), 1))
				{
#if !defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
					int iAttackerStrength = pEnemyUnit->GetMaxAttackStrength(NULL, pLoopCity->plot(), NULL);
#else
					int iAttackerStrength = pEnemyUnit->GetMaxAttackStrength(kCombatInfo);
#endif
					int iDefenderStrength = pLoopCity->getStrengthValue();
					CvUnit* pFireSupportUnit = CvUnitCombat::GetFireSupportUnit(pLoopCity->getOwner(), pLoopCity->getX(), pLoopCity->getY(), pEnemyUnit->getX(), pEnemyUnit->getY());
					int iDefenderFireSupportCombatDamage = 0;
					if(pFireSupportUnit != NULL)
					{
						iDefenderFireSupportCombatDamage = pFireSupportUnit->GetRangeCombatDamage(pEnemyUnit.pointer(), NULL, false);
					}
					iExpectedDamage = pEnemyUnit->getCombatDamage(iAttackerStrength, iDefenderStrength, pEnemyUnit->getDamage() + iDefenderFireSupportCombatDamage,                  false,                     false,                     true);
				}
				if(iExpectedDamage > 0)
				{
					iExpectedTotalDamage += iExpectedDamage;
					possibleAttackers.push_back(*pTarget);
				}
			}
			pTarget = GetNextUnitTarget();
		}


		if(iExpectedTotalDamage > (pLoopCity->GetMaxHitPoints() - pLoopCity->getDamage()))
		{
			unsigned int iAttackerIndex = 0;


			pTarget = GetFirstUnitTarget();
			while(iAttackerIndex < possibleAttackers.size() && pTarget != NULL)
			{

				if(pTarget->GetTargetX() == possibleAttackers[iAttackerIndex].GetTargetX() &&
				        pTarget->GetTargetY() == possibleAttackers[iAttackerIndex].GetTargetY())
				{
					CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
					UnitHandle pEnemyUnit = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
					if(pEnemyUnit && pEnemyUnit->IsCanAttackRanged())
					{
						pTarget->SetTargetType(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT);
					}
					else
					{
						pTarget->SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
					}
					iAttackerIndex++;
				}
				pTarget = GetNextUnitTarget();
			}
		}


		else if(possibleAttackers.size() > 0)
		{
			unsigned int iAttackerIndex = 0;


			pTarget = GetFirstUnitTarget();
			while(iAttackerIndex < possibleAttackers.size() && pTarget != NULL)
			{

				if(pTarget->GetTargetX() == possibleAttackers[iAttackerIndex].GetTargetX() &&
				        pTarget->GetTargetY() == possibleAttackers[iAttackerIndex].GetTargetY())
				{
					if(pTarget->GetTargetType() != AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
					{
						pTarget->SetTargetType(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT);
					}
					iAttackerIndex++;
				}
				pTarget = GetNextUnitTarget();
			}
		}
	}
}


void CvTacticalAI::IdentifyPriorityBarbarianTargets()
{
	CvPlot* pLoopPlot;
	CvTacticalTarget* pTarget;

#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getMap().numPlots(); iI++)
#else
	for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
#endif
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
		if(pLoopPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
		{
			pTarget = GetFirstUnitTarget();
			while(pTarget != NULL)
			{
				bool bPriorityTarget = false;


				if(pTarget->GetTargetType() != AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
				{
					CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
					UnitHandle pEnemyUnit = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
#if !defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
					if(pEnemyUnit->IsCanAttackRanged() && pEnemyUnit->GetMaxRangedCombatStrength(NULL,           NULL, true, true) > pEnemyUnit->GetMaxAttackStrength(NULL, pLoopPlot, NULL))
#else
					CvCombatInfo kCombatInfo;
					kCombatInfo.setUnit(BATTLE_UNIT_ATTACKER, pEnemyUnit.pointer());
					kCombatInfo.setPlot(pLoopPlot);
					kCombatInfo.setAttackIsRanged(true);
					if (pEnemyUnit->IsCanAttackRanged() && pEnemyUnit->GetMaxRangedCombatStrength(kCombatInfo) > pEnemyUnit->GetMaxAttackStrength(kCombatInfo))
#endif
					{
						if(plotDistance(pEnemyUnit->getX(), pEnemyUnit->getY(), pLoopPlot->getX(), pLoopPlot->getY()) <= pEnemyUnit->GetRange())
						{
							if(pEnemyUnit->canEverRangeStrikeAt(pLoopPlot->getX(), pLoopPlot->getY()))
							{
								bPriorityTarget = true;
							}
						}
					}
					else if(CanReachInXTurns(pEnemyUnit, pLoopPlot, 1))
					{
						bPriorityTarget = true;
					}
					if(bPriorityTarget)
					{
						pTarget->SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
					}
				}
				pTarget = GetNextUnitTarget();
			}
		}
	}
}


void CvTacticalAI::IdentifyPriorityTargetsByType()
{

	for(unsigned int iI = 0; iI < m_AllTargets.size(); iI++)
	{

		if(m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{

			CvUnit* pUnit = (CvUnit*)m_AllTargets[iI].GetAuxData();
			if(pUnit->IsCanAttackRanged())
			{
				m_AllTargets[iI].SetTargetType(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT);
			}
		}


		if(m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
		        m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{

			CvUnit* pUnit = (CvUnit*)m_AllTargets[iI].GetAuxData();
			ImprovementTypes eImprovement = pUnit->plot()->getImprovementType();
			if(pUnit->plot()->getOwner() == pUnit->getOwner() &&
			        eImprovement != NO_IMPROVEMENT && GC.getImprovementInfo(eImprovement)->GetNearbyEnemyDamage() > 0)
			{
				m_AllTargets[iI].SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
			}
		}
	}
}


void CvTacticalAI::EliminateNearbyBlockadePoints()
{

	std::stable_sort(m_NavalResourceBlockadePoints.begin(), m_NavalResourceBlockadePoints.end());


	TacticalList tempPoints;
	tempPoints = m_NavalResourceBlockadePoints;


	m_NavalResourceBlockadePoints.clear();


	TacticalList::iterator it, it2;
	for(it = tempPoints.begin(); it != tempPoints.end(); ++it)
	{
		bool bFoundAdjacent = false;


		for(it2 = m_NavalResourceBlockadePoints.begin(); it2 != m_NavalResourceBlockadePoints.end(); ++it2)
		{
			if(plotDistance(it->GetTargetX(), it->GetTargetY(), it2->GetTargetX(), it2->GetTargetY()) <= 2)
			{
				bFoundAdjacent = true;
				break;
			}
		}

		if(!bFoundAdjacent)
		{
			m_NavalResourceBlockadePoints.push_back(*it);
		}
	}


	for(it = m_NavalResourceBlockadePoints.begin(); it != m_NavalResourceBlockadePoints.end(); ++it)
	{
		m_AllTargets.push_back(*it);
	}
}


void CvTacticalAI::ExtractTargetsForZone(CvTacticalDominanceZone* pZone                                 )
{
	TacticalList::iterator it;

	m_ZoneTargets.clear();
	for(it = m_AllTargets.begin(); it != m_AllTargets.end(); ++it)
	{
		bool bValid = false;

		if(pZone == NULL)
		{
			bValid = true;
		}
		else
		{
			DomainTypes eDomain = DOMAIN_LAND;
			if(pZone->IsWater())
			{
				eDomain = DOMAIN_SEA;
			}
			bValid = it->IsTargetValidInThisDomain(eDomain);
		}
		if(bValid)
		{
			if(pZone == NULL || it->GetDominanceZone() == pZone->GetDominanceZoneID())
			{
				m_ZoneTargets.push_back(*it);
			}


			else
			{
				CvCity* pCity;
				pCity = pZone->GetClosestCity();
				if(pCity)
				{
					if(plotDistance(pCity->getX(), pCity->getY(), it->GetTargetX(), it->GetTargetY()) <= 2)
					{
						m_ZoneTargets.push_back(*it);
					}
				}
			}
		}
	}
}


CvTacticalTarget* CvTacticalAI::GetFirstZoneTarget(AITacticalTargetType eType)
{
	m_eCurrentTargetType = eType;
	m_iCurrentTargetIndex = 0;

	while(m_iCurrentTargetIndex < (int)m_ZoneTargets.size())
	{
		if(m_eCurrentTargetType == AI_TACTICAL_TARGET_NONE || m_ZoneTargets[m_iCurrentTargetIndex].GetTargetType() == m_eCurrentTargetType)
		{
			return &m_ZoneTargets[m_iCurrentTargetIndex];
		}
		m_iCurrentTargetIndex++;
	}

	return NULL;
}


CvTacticalTarget* CvTacticalAI::GetNextZoneTarget()
{
	m_iCurrentTargetIndex++;

	while(m_iCurrentTargetIndex < (int)m_ZoneTargets.size())
	{
		if(m_eCurrentTargetType == AI_TACTICAL_TARGET_NONE || m_ZoneTargets[m_iCurrentTargetIndex].GetTargetType() == m_eCurrentTargetType)
		{
			return &m_ZoneTargets[m_iCurrentTargetIndex];
		}
		m_iCurrentTargetIndex++;
	}

	return NULL;
}


CvTacticalTarget* CvTacticalAI::GetFirstUnitTarget()
{
	m_iCurrentUnitTargetIndex = 0;

	while(m_iCurrentUnitTargetIndex < (int)m_AllTargets.size())
	{
		if(m_AllTargets[m_iCurrentUnitTargetIndex].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_AllTargets[m_iCurrentUnitTargetIndex].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
		        m_AllTargets[m_iCurrentUnitTargetIndex].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			return &m_AllTargets[m_iCurrentUnitTargetIndex];
		}
		m_iCurrentUnitTargetIndex++;
	}

	return NULL;
}


CvTacticalTarget* CvTacticalAI::GetNextUnitTarget()
{
	m_iCurrentUnitTargetIndex++;

	while(m_iCurrentUnitTargetIndex < (int)m_AllTargets.size())
	{
		if(m_AllTargets[m_iCurrentUnitTargetIndex].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_AllTargets[m_iCurrentUnitTargetIndex].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
		        m_AllTargets[m_iCurrentUnitTargetIndex].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			return &m_AllTargets[m_iCurrentUnitTargetIndex];
		}
		m_iCurrentUnitTargetIndex++;
	}

	return NULL;
}




void CvTacticalAI::ExecuteBarbarianCampMove(CvPlot* pTargetPlot)
{

	UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if(pUnit)
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
		pUnit->finishMoves();


		UnitProcessed(m_CurrentMoveUnits[0].GetID());
	}
}


void CvTacticalAI::ExecuteCivilianCapture(CvPlot* pTargetPlot)
{

	UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if(pUnit)
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
		pUnit->finishMoves();


		UnitProcessed(m_CurrentMoveUnits[0].GetID());

		pUnit->SetTacticalAIPlot(NULL);
	}
}


void CvTacticalAI::ExecutePillage(CvPlot* pTargetPlot)
{

	UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if(pUnit)
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
		pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
		pUnit->finishMoves();


		UnitProcessed(m_CurrentMoveUnits[0].GetID());
	}
}


void CvTacticalAI::ExecutePlunderTradeUnit(CvPlot* pTargetPlot)
{

	UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if(pUnit)
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
		pUnit->PushMission(CvTypes::getMISSION_PLUNDER_TRADE_ROUTE());
		pUnit->finishMoves();


		UnitProcessed(m_CurrentMoveUnits[0].GetID());
	}
}


void CvTacticalAI::ExecuteParadropPillage(CvPlot* pTargetPlot)
{

	UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if(pUnit)
	{
		pUnit->PushMission(CvTypes::getMISSION_PARADROP(), pTargetPlot->getX(), pTargetPlot->getY());
		pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
		pUnit->finishMoves();


		UnitProcessed(m_CurrentMoveUnits[0].GetID());
	}
}


void CvTacticalAI::ExecuteAttack(CvTacticalTarget* pTarget, CvPlot* pTargetPlot, bool bInflictWhatWeTake, bool bMustSurviveAttack)
{
	void* pFirstAttacker = NULL;
	bool bFirstAttackRanged = false;
	bool bFirstAttackCity = false;

	if(PlotAlreadyTargeted(pTargetPlot) != -1)
	{
		return;
	}


	int iDamageRemaining = (pTarget->GetAuxIntData() * (100 + GC.getAI_TACTICAL_OVERKILL_PERCENT())) / 100;


	for(unsigned int iI = 0; iI < m_CurrentMoveCities.size() && iDamageRemaining > 0; iI++)
	{
		CvCity* pCity = m_pPlayer->getCity(m_CurrentMoveCities[iI].GetID());
		if(pCity != NULL)
		{
			if(QueueAttack(pCity, pTarget, true            , true          ))
			{
				pFirstAttacker = (void*)pCity;
				bFirstAttackRanged = true;
				bFirstAttackCity = true;
			}


			iDamageRemaining -= m_CurrentMoveCities[iI].GetExpectedTargetDamage();
		}
	}


	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size() && iDamageRemaining > 0; iI++)
	{
		if(!bInflictWhatWeTake || m_CurrentMoveUnits[iI].GetExpectedTargetDamage() >= m_CurrentMoveUnits[iI].GetExpectedSelfDamage())
		{
			UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
			if(pUnit)
			{
				if(pUnit->getMoves() > 0)
				{
					if(!bMustSurviveAttack || ((m_CurrentMoveUnits[iI].GetExpectedSelfDamage() + pUnit->getDamage()) < pUnit->GetMaxHitPoints()))
					{

						if(pUnit->IsCanAttackRanged())
						{

							if(plotDistance(pUnit->getX(), pUnit->getY(), pTargetPlot->getX(), pTargetPlot->getY()) <= pUnit->GetRange())
							{

								if(pUnit->canEverRangeStrikeAt(pTargetPlot->getX(), pTargetPlot->getY()))
								{

									if(pUnit->canSetUpForRangedAttack(NULL))
									{
										pUnit->setSetUpForRangedAttack(true);
										if(GC.getLogging() && GC.getAILogging())
										{
											CvString strTemp;
											CvString strMsg;
											strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
											strMsg.Format("Set up %s for ranged attack", strTemp.GetCString());
											LogTacticalMessage(strMsg);
										}
										if(!pUnit->canMove())
										{
											pUnit->SetTacticalAIPlot(NULL);
											UnitProcessed(pUnit->GetID());
										}
									}


									if(pUnit->canMove() && pUnit->canRangeStrikeAt(pTargetPlot->getX(), pTargetPlot->getY()))
									{

										if(QueueAttack((void*)pUnit.pointer(), pTarget, true            , false          ))
										{
											pFirstAttacker = (void*)pUnit.pointer();
											bFirstAttackRanged = true;
										}
										pUnit->SetTacticalAIPlot(NULL);
										UnitProcessed(m_CurrentMoveUnits[iI].GetID());


										iDamageRemaining -= m_CurrentMoveUnits[iI].GetExpectedTargetDamage();
									}
								}
							}
						}
					}
					else
					{
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Not attacking with unit. We'll destroy ourself, %s, X: %d, Y: %d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
							LogTacticalMessage(strMsg);
						}
					}
				}
			}
		}
		else
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format("Not attacking with unit. Can't generate a good damage ratio.");
				LogTacticalMessage(strMsg);
			}
		}
	}


	if(pTarget->GetTargetType() == AI_TACTICAL_TARGET_CITY && iDamageRemaining < 1)
	{
		iDamageRemaining = 1;
	}


	std::vector<CvPlot *> plotList;
	for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
	{
		CvPlot* pAdjacentPlot = plotDirection(pTargetPlot->getX(), pTargetPlot->getY(), ((DirectionTypes)iDirectionLoop));
		if (pAdjacentPlot)
		{
			int iPlotIndex = GC.getMap().plotNum(pAdjacentPlot->getX(), pAdjacentPlot->getY());
			CvTacticalAnalysisCell *pCell = m_pMap->GetCell(iPlotIndex);
			if(pAdjacentPlot != NULL && pAdjacentPlot->getNumDefenders(pTarget->GetTargetPlayer()) == 0 && !pCell->IsEnemyCity())
			{
				plotList.push_back(pAdjacentPlot);
			}
		}
	}


	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size() && iDamageRemaining > 0; iI++)
	{
		if(!bInflictWhatWeTake || m_CurrentMoveUnits[iI].GetExpectedTargetDamage() >= m_CurrentMoveUnits[iI].GetExpectedSelfDamage())
		{
			UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
			if(pUnit)
			{
				if(pUnit->getMoves() > 0 && (!bMustSurviveAttack || ((m_CurrentMoveUnits[iI].GetExpectedSelfDamage() + pUnit->getDamage()) < pUnit->GetMaxHitPoints())))
				{

					if(!pUnit->IsCanAttackRanged())
					{
						bool bQueueAttackNow = false;
						

						if (plotDistance(pUnit->getX(), pUnit->getY(), pTargetPlot->getX(), pTargetPlot->getY()) <= 1)
						{
							bQueueAttackNow = true;
						}
						else
						{

							std::vector<CvPlot *>::iterator it;
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
							for (it = plotList.begin(); it != plotList.end(); ++it)
#else
							for (it = plotList.begin(); it != plotList.end(); it++)
#endif
							{
								if (TurnsToReachTarget(pUnit, *it, false                , false                 , true                    ) == 0)
								{

									pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), (*it)->getX(), (*it)->getY());
									bQueueAttackNow = true;

									plotList.erase(it);
									break;
								}
							}


							if (!bQueueAttackNow)
							{
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strMsg;
									strMsg.Format("Not enough free hexes next for all melee units to attack, Target X: %d, Y: %d", pTargetPlot->getX(), pTargetPlot->getY());
									LogTacticalMessage(strMsg);
								}
								

								if (MoveToEmptySpaceNearTarget(pUnit, pTargetPlot, (pUnit->getDomainType() == DOMAIN_LAND)))
								{
									if(GC.getLogging() && GC.getAILogging())
									{
										CvString strMsg;
										strMsg.Format("Moving adjacent for attack next turn, Now at X: %d, Y: %d", pUnit->getX(), pUnit->getY());
										LogTacticalMessage(strMsg);
									}
								}
							}
						}

						if (bQueueAttackNow)
						{

							if(QueueAttack((void*)pUnit.pointer(), pTarget, false            , false          ))
							{
								pFirstAttacker = (void*)pUnit.pointer();
							}
							pUnit->SetTacticalAIPlot(NULL);
							UnitProcessed(m_CurrentMoveUnits[iI].GetID(), false                     );


							iDamageRemaining -= m_CurrentMoveUnits[iI].GetExpectedTargetDamage();
						}
					}
				}
				else
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Not attacking with unit. We'll destroy ourself, %s, X: %d, Y: %d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
						LogTacticalMessage(strMsg);
					}
				}
			}
		}
		else
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format("Not attacking with unit. Can't generate a good damage ratio.");
				LogTacticalMessage(strMsg);
			}
		}
	}


	if(pFirstAttacker)
	{
		LaunchAttack(pFirstAttacker, pTarget, true           , bFirstAttackRanged, bFirstAttackCity);
	}
}


void CvTacticalAI::ExecuteRepositionMoves()
{
	CvPlot* pBestPlot = NULL;
	CvString strTemp;

	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			strTemp = pUnit->getUnitInfo().GetDescription();


			if(pUnit->getDomainType() == DOMAIN_LAND)
			{
				pBestPlot = FindNearbyTarget(pUnit, m_iRepositionRange);

				if(pBestPlot)
				{
					if(MoveToEmptySpaceNearTarget(pUnit, pBestPlot, (pUnit->getDomainType()==DOMAIN_LAND)))
					{
						pUnit->finishMoves();
						UnitProcessed(m_CurrentMoveUnits[iI].GetID(), pUnit->IsCombatUnit());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("%s moved to empty space near target, X: %d, Y: %d, Current X: %d, Current Y: %d", strTemp.GetCString(),
							                    pBestPlot->getX(), pBestPlot->getY(), pUnit->getX(), pUnit->getY());
							LogTacticalMessage(strLogString);
						}
					}
				}
			}
		}
	}
}

#define PATH_PLAN_LAST
#if defined(PATH_PLAN_LAST)
typedef CvWeightedVector<CvPlot*, 1, true> WeightedPlotVector;



static CvPlot* GetReachablePlot(UnitHandle pUnit, WeightedPlotVector& aPlots, int iTurns)
{
	uint uiListSize;
	if ((uiListSize = aPlots.size()) > 0)
	{
		aPlots.SortItems();
		for (uint i = uiListSize; i--; )
		{
			CvPlot* pPlot = aPlots.GetElement(i);
			if(CanReachInXTurns(pUnit, pPlot, iTurns))
			{
				return pPlot;
			}
		}
	}
	return NULL;
}
#endif


void CvTacticalAI::ExecuteMovesToSafestPlot()
{
	int iDanger;

#if defined(PATH_PLAN_LAST)

	WeightedPlotVector aCityList;
	aCityList.reserve(10);
	WeightedPlotVector aZeroDangerList;
	aZeroDangerList.reserve(100);
	WeightedPlotVector aCoverList;
	aCoverList.reserve(100);
	WeightedPlotVector aDangerList;
	aDangerList.reserve(100);
#endif

	TeamTypes ePlayerTeam = m_pPlayer->getTeam();
	PlayerTypes ePlayerID = m_pPlayer->GetID();

	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
#if defined(PATH_PLAN_LAST)
			aCityList.clear();
			aZeroDangerList.clear();
			aCoverList.clear();
			aDangerList.clear();
#endif
			CvPlot* pBestPlot = NULL;

			int iRange = pUnit->getUnitInfo().GetMoves();
#if !defined(PATH_PLAN_LAST)
			int iLowestDanger = MAX_INT;
			bool bResultHasZeroDangerMove = false;
			bool bResultInTerritory = false;
			bool bResultInCity = false;
			bool bResultInCover = false;
#endif

			CvMap& kMap = GC.getMap();
			int iUnitX = pUnit->getX();
			int iUnitY = pUnit->getY();
			DomainTypes eUnitDomain = pUnit->getDomainType();
			bool bIsSeaUnit = eUnitDomain == DOMAIN_SEA;


			for(int iX = -iRange; iX <= iRange; iX++)
			{
				for(int iY = -iRange; iY <= iRange; iY++)
				{
					CvPlot* pPlot = kMap.plot(iUnitX + iX, iUnitY + iY);
					if(pPlot == NULL)
					{
						continue;
					}


					if(pPlot->getNumUnits() > 0)
					{
						IDInfo* pUnitNode = pPlot->headUnitNode();
						if(pUnitNode)
						{
							CvUnit* pFirstUnit = ::getUnit(*pUnitNode);
							if(pFirstUnit)
							{
								if(pFirstUnit->getOwner() != pUnit->getOwner())
								{
									continue;
								}

								else if(pUnit->AreUnitsOfSameType(*pFirstUnit))
								{
									continue;
								}
							}
						}
					}


					if (bIsSeaUnit && !pPlot->isWater())
					{
						continue;
					}

#if !defined(PATH_PLAN_LAST)
					if(!CanReachInXTurns(pUnit, pPlot, 1))
					{
						continue;
					}
#endif






					iDanger = m_pPlayer->GetPlotDanger(*pPlot);
					bool bIsZeroDanger = (iDanger <= 0);
					bool bIsInCity = pPlot->isFriendlyCity(*pUnit, false);
					bool bIsInCover = (pPlot->getNumDefenders(ePlayerID) > 0) && !pUnit->IsCanDefend(pPlot);
					bool bIsInTerritory = (pPlot->getTeam() == ePlayerTeam);

#if defined(PATH_PLAN_LAST)
					if(bIsInCity)
					{
						aCityList.push_back(pPlot, iDanger);
					}
					else
					if(bIsZeroDanger)
					{
						aZeroDangerList.push_back(pPlot, (bIsInTerritory)?0:1);
					}
					else
					if(bIsInCover)
					{
						aCoverList.push_back(pPlot, iDanger);
					}
					else
					{
						aDangerList.push_back(pPlot, iDanger);
					}
#endif

#if !defined(PATH_PLAN_LAST)
					bool bUpdateBestValue = false;

					if(bIsInCity)
					{
						if(!bResultInCity || iDanger < iLowestDanger)
						{
							bUpdateBestValue = true;
						}
					}
					else if(bIsZeroDanger)
					{
						if(!bResultInCity)
						{
							if(bResultHasZeroDangerMove)
							{
								if(bIsInTerritory && !bResultInTerritory)
								{
									bUpdateBestValue = true;
								}
							}
							else
							{
								bUpdateBestValue = true;
							}
						}
					}
					else if(bIsInCover)
					{
						if(!bResultInCity && !bResultHasZeroDangerMove)
						{
							if(!bResultInCover || iDanger < iLowestDanger)
							{
								bUpdateBestValue = true;
							}
						}
					}
					else if(bIsInTerritory)
					{
						if(!bResultInCity && !bResultInCover && !bResultHasZeroDangerMove)
						{
							if(!bResultInTerritory || iDanger < iLowestDanger)
							{
								bUpdateBestValue = true;
							}
						}
					}

					else if(!bResultInCity && !bResultInCover && !bResultInTerritory && !bResultHasZeroDangerMove)
					{
						if(iDanger < iLowestDanger)
						{
							bUpdateBestValue = true;
						}
					}

					if(bUpdateBestValue)
					{
						pBestPlot = pPlot;
						iLowestDanger = iDanger;

						bResultInTerritory = bIsInTerritory;
						bResultInCity      = bIsInCity;
						bResultInCover     = bIsInCover;
						bResultHasZeroDangerMove = bIsZeroDanger;
					}
#endif
				}
			}

#if defined(PATH_PLAN_LAST)


			if ((pBestPlot = GetReachablePlot(pUnit, aCityList, 1)) == NULL)
				if ((pBestPlot = GetReachablePlot(pUnit, aZeroDangerList, 1)) == NULL)
					if ((pBestPlot = GetReachablePlot(pUnit, aCoverList, 1)) == NULL)
						pBestPlot = GetReachablePlot(pUnit, aDangerList, 1);
#endif

			if(pBestPlot != NULL)
			{

				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY(), MOVE_UNITS_IGNORE_DANGER);
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID(), pUnit->IsCombatUnit());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp;
					strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
					strLogString.Format("Moving %s to safety, To X: %d, Y: %d, From X: %d, Y: %d", strTemp.GetCString(), pBestPlot->getX(), pBestPlot->getY(), pUnit->getX(), pUnit->getY());
					LogTacticalMessage(strLogString);
				}
			}
			else
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp;
					strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
					strLogString.Format("Failed to find destination moving %s to safety from, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
					LogTacticalMessage(strLogString);
				}
			}
		}
	}
}


void CvTacticalAI::ExecuteHeals()
{
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			if(pUnit->isFortifyable())
			{
				pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
				pUnit->SetFortifiedThisTurn(true);
			}
			else
			{
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
			}
			UnitProcessed(m_CurrentMoveUnits[iI].GetID());
		}
	}
}


void CvTacticalAI::ExecuteBarbarianMoves(bool bAggressive)
{
	CvPlot* pBestPlot = NULL;
	CvPlot* pCurrentDestination;
	CvString strTemp;

	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			if(pUnit->isBarbarian())
			{
				strTemp = pUnit->getUnitInfo().GetDescription();


				if(pUnit->getDomainType() == DOMAIN_LAND)
				{
					AI_PERF_FORMAT("AI-perf-tact.csv", ("Barb Land Move, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

					if(bAggressive)
					{
						pBestPlot = FindBestBarbarianLandMove(pUnit);
					}
					else
					{
						pBestPlot = FindPassiveBarbarianLandMove(pUnit);
					}

					if(pBestPlot && MoveToEmptySpaceNearTarget(pUnit, pBestPlot))
					{
						pUnit->finishMoves();
						UnitProcessed(m_CurrentMoveUnits[iI].GetID());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("%s moved to empty space near target, X: %d, Y: %d, Current X: %d, Current Y: %d", strTemp.GetCString(),
							                    pBestPlot->getX(), pBestPlot->getY(), pUnit->getX(), pUnit->getY());
							LogTacticalMessage(strLogString);
						}
					}
					else
					{
						pUnit->finishMoves();
						UnitProcessed(m_CurrentMoveUnits[iI].GetID());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("No target for %s at position, Current X: %d, Current Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
							LogTacticalMessage(strLogString);
						}
					}
				}


				else
				{

					AI_PERF_FORMAT("AI-perf-tact.csv", ("Barb Naval Move, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );


					pCurrentDestination = pUnit->GetTacticalAIPlot();


					if(pCurrentDestination == NULL ||
					        pCurrentDestination == pUnit->plot())
					{
						pBestPlot = FindBestBarbarianSeaMove(pUnit);
					}


					else
					{
						if(TurnsToReachTarget(pUnit, pCurrentDestination, true                ) != MAX_INT)
						{
							pBestPlot = pCurrentDestination;
						}
						else
						{
							pBestPlot = FindBestBarbarianSeaMove(pUnit);
						}
					}

					if(pBestPlot != NULL)
					{
						pUnit->SetTacticalAIPlot(pBestPlot);
						pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY());
						pUnit->finishMoves();
						UnitProcessed(m_CurrentMoveUnits[iI].GetID());

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Moving %s to new position, X: %d, Y: %d, Current X: %d, Current Y: %d", strTemp.GetCString(),
							                    pBestPlot->getX(), pBestPlot->getY(), pUnit->getX(), pUnit->getY());
							LogTacticalMessage(strLogString);
						}
					}
					else
					{
						pUnit->SetTacticalAIPlot(NULL);
						pUnit->finishMoves();
						UnitProcessed(m_CurrentMoveUnits[iI].GetID());

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("No target for %s at position, Current X: %d, Current Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
							LogTacticalMessage(strLogString);
						}
					}
				}
			}
		}
	}
}


void CvTacticalAI::ExecuteBarbarianCivilianEscortMove()
{
	UnitHandle pCivilian;
	UnitHandle pEscort;
	CvPlot* pTarget;
	CvPlot* pCurrent;
	CvPlot* pCivilianMove;
	CvPlot* pEscortMove;
	CvUnit* pLoopUnit;

	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		pCivilian = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pCivilian)
		{
			pTarget = FindNearbyTarget(pCivilian, MAX_INT, AI_TACTICAL_TARGET_BARBARIAN_CAMP, pCivilian.pointer());
			if(pTarget)
			{

				pCurrent = pCivilian->plot();
				if(pCurrent == pTarget)
				{
					pCivilian->finishMoves();
					UnitProcessed(pCivilian->GetID());
				}
#ifdef AUI_WARNING_FIXES
				else if (pCurrent)
#else
				else
#endif
				{
					if(pCurrent->getNumUnits() > 1)
					{
#ifdef AUI_WARNING_FIXES
						for (uint iJ = 0; iJ < pCurrent->getNumUnits(); iJ++)
#else
						for(int iJ = 0; iJ < pCurrent->getNumUnits(); iJ++)
#endif
						{
							pLoopUnit = pCurrent->getUnitByIndex(iJ);
							if(pLoopUnit->GetID() != pCivilian->GetID() &&
							        pLoopUnit->getOwner() == pCivilian->getOwner())
							{
								pEscort = UnitHandle(pLoopUnit);
								break;
							}
						}
					}


					int iFlags = 0;
					if(pEscort)
					{
						iFlags = MOVE_UNITS_IGNORE_DANGER;
					}


					if(!pCivilian->GeneratePath(pTarget, iFlags, false           ))
					{
						pCivilian->finishMoves();
						if(pEscort)
						{
							pEscort->finishMoves();
						}
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Retargeting civilian escort operation (path lost to target), X: %d, Y: %d", pTarget->getX(), pTarget->getY());
							LogTacticalMessage(strLogString);
						}
					}

					else
					{
						pCivilianMove = pCivilian->GetPathEndTurnPlot();


						if(pCivilianMove == pTarget)
						{

							UnitHandle pCampDefender = pCivilianMove->getBestDefender(m_pPlayer->GetID());
							if(!pCampDefender || (pEscort && pEscort->GetPower() > pCampDefender->GetPower()))
							{
								if(pEscort && (!pCampDefender || ExecuteMoveOfBlockingUnit(pCampDefender)))
								{
									ExecuteMoveToPlot(pEscort, pCivilianMove);
									ExecuteMoveToPlot(pCivilian, pCivilianMove);
									if(GC.getLogging() && GC.getAILogging())
									{
										CvString strTemp;
										CvString strLogString;
										strTemp = pEscort->getUnitInfo().GetDescription();
										strLogString.Format("Moving escorting %s to take over camp defense, X: %d, Y: %d", strTemp.GetCString(), pEscort->getX(), pEscort->getY());
										LogTacticalMessage(strLogString);
										strTemp = pCivilian->getUnitInfo().GetDescription();
										strLogString.Format("Moving %s to camp, X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
										LogTacticalMessage(strLogString);
									}
								}
								else
								{
									ExecuteMoveToPlot(pCivilian, pCivilianMove);
									if(GC.getLogging() && GC.getAILogging())
									{
										CvString strTemp;
										CvString strLogString;
										strTemp = pCivilian->getUnitInfo().GetDescription();
										strLogString.Format("Moving %s to camp, X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
										LogTacticalMessage(strLogString);
									}
								}
							}
							else
							{
								ExecuteMoveToPlot(pCivilian, pCivilianMove);
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strTemp;
									CvString strLogString;
									strTemp = pCivilian->getUnitInfo().GetDescription();
									strLogString.Format("Moving %s to camp, X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
									LogTacticalMessage(strLogString);
								}
							}
						}


						else if(!pEscort)
						{
							ExecuteMoveToPlot(pCivilian, pCivilianMove);
							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strTemp;
								CvString strLogString;
								strTemp = pCivilian->getUnitInfo().GetDescription();
								strLogString.Format("Moving %s without escort to target, X: %d, Y: %d", strTemp.GetCString(), pCivilian->plot()->getX(), pCivilian->plot()->getY());
								LogTacticalMessage(strLogString);
							}
						}


						else
						{

							if(TurnsToReachTarget(pEscort, pCivilianMove) <= 1)
							{
								ExecuteMoveToPlot(pEscort, pCivilianMove);
								ExecuteMoveToPlot(pCivilian, pCivilianMove);
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strTemp;
									CvString strLogString;
									strTemp = pEscort->getUnitInfo().GetDescription();
									strLogString.Format("Moving escorting %s to target, X: %d, Y: %d", strTemp.GetCString(), pEscort->getX(), pEscort->getY());
									LogTacticalMessage(strLogString);
									strTemp = pCivilian->getUnitInfo().GetDescription();
									strLogString.Format("Moving %s to target, X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
									LogTacticalMessage(strLogString);
								}
							}

							else
							{
								UnitHandle pBlockingUnit = pCivilianMove->getBestDefender(m_pPlayer->GetID());



								if(!pBlockingUnit || pBlockingUnit->getMoves() == 0)
								{
									if(!pEscort->GeneratePath(pTarget, 0, false           ))
									{
										pCivilian->finishMoves();
										pEscort->finishMoves();
										if(GC.getLogging() && GC.getAILogging())
										{
											CvString strLogString;
											strLogString.Format("Retargeting civilian escort operation (path lost to target), X: %d, Y: %d", pTarget->getX(), pTarget->getY());
											LogTacticalMessage(strLogString);
										}
									}
									else
									{
										pEscortMove = pCivilian->GetPathEndTurnPlot();


										if(TurnsToReachTarget(pCivilian, pEscortMove) <= 1)
										{
											ExecuteMoveToPlot(pEscort, pEscortMove);
											ExecuteMoveToPlot(pCivilian, pEscortMove);
											if(GC.getLogging() && GC.getAILogging())
											{
												CvString strTemp;
												CvString strLogString;
												strTemp = pEscort->getUnitInfo().GetDescription();
												strLogString.Format("Moving escorting %s to target, X: %d, Y: %d", strTemp.GetCString(), pEscort->getX(), pEscort->getY());
												LogTacticalMessage(strLogString);
												strTemp = pCivilian->getUnitInfo().GetDescription();
												strLogString.Format("Moving %s to target, X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
												LogTacticalMessage(strLogString);
											}
										}
										else
										{
											pCivilian->finishMoves();
											pEscort->finishMoves();
											if(GC.getLogging() && GC.getAILogging())
											{
												CvString strLogString;
												strLogString.Format("Retargeting civilian escort operation. Too many blocking units.");
												LogTacticalMessage(strLogString);
											}
										}
									}
								}


								else
								{
									if(ExecuteMoveOfBlockingUnit(pBlockingUnit))
									{
										ExecuteMoveToPlot(pEscort, pCivilianMove);
										ExecuteMoveToPlot(pCivilian, pCivilianMove);
										if(GC.getLogging() && GC.getAILogging())
										{
											CvString strTemp;
											CvString strLogString;
											strTemp = pEscort->getUnitInfo().GetDescription();
											strLogString.Format("Moving escorting %s to target, X: %d, Y: %d", strTemp.GetCString(), pEscort->getX(), pEscort->getY());
											LogTacticalMessage(strLogString);
											strTemp = pCivilian->getUnitInfo().GetDescription();
											strLogString.Format("Moving %s to target, X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
											LogTacticalMessage(strLogString);
										}
									}
									else
									{
										pCivilian->finishMoves();
										pEscort->finishMoves();
										if(GC.getLogging() && GC.getAILogging())
										{
											CvString strLogString;
											strLogString.Format("Retargeting civilian escort operation. Could not move blocking unit.");
											LogTacticalMessage(strLogString);
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
}


void CvTacticalAI::ExecuteMoveToPlot(CvPlot* pTarget, bool bSaveMoves)
{

	UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if(pUnit)
	{
		ExecuteMoveToPlot(pUnit, pTarget, bSaveMoves);
	}
}


void CvTacticalAI::ExecuteMoveToPlot(UnitHandle pUnit, CvPlot* pTarget, bool bSaveMoves)
{

	if(pTarget == pUnit->plot())
	{

		if(pUnit->canFortify(pUnit->plot()))
		{
			pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
			pUnit->SetFortifiedThisTurn(true);
		}
		else
		{
			pUnit->PushMission(CvTypes::getMISSION_SKIP());
			if(!bSaveMoves)
			{
				pUnit->finishMoves();
			}
		}
	}

	else
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY(), MOVE_UNITS_IGNORE_DANGER);
		if(!bSaveMoves)
		{
			pUnit->finishMoves();
		}
	}

	UnitProcessed(pUnit->GetID(), pUnit->IsCombatUnit());
}


bool CvTacticalAI::ExecuteMoveOfBlockingUnit(UnitHandle pBlockingUnit)
{
	if(!pBlockingUnit->canMove() || IsInQueuedAttack(pBlockingUnit.pointer()))
	{
		return false;
	}

	CvPlot* pOldPlot = pBlockingUnit->plot();

	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pPlot = plotDirection(pBlockingUnit->getX(), pBlockingUnit->getY(), ((DirectionTypes)iI));
		if(pPlot != NULL)
		{

			if (!pOldPlot->isWater() && pPlot->isWater() && pBlockingUnit->getDomainType() == DOMAIN_LAND)
			{
				continue;
			}


			if(!pPlot->getVisibleEnemyDefender(m_pPlayer->GetID()) && !pPlot->isEnemyCity(*pBlockingUnit.pointer()) && pBlockingUnit->GeneratePath(pPlot))
			{
				ExecuteMoveToPlot(pBlockingUnit, pPlot);
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strTemp, strLogString;
					strTemp = pBlockingUnit->getUnitInfo().GetDescription();
					strLogString.Format("Moving blocking %s out of way, Leaving X: %d, Y: %d, Now At X: %d, Y: %d", strTemp.GetCString(), pOldPlot->getX(), pOldPlot->getY(), pBlockingUnit->getX(), pBlockingUnit->getY());
					LogTacticalMessage(strLogString);
				}
				return true;
			}
		}
	}
	return false;
}


void CvTacticalAI::ExecuteNavalBlockadeMove(CvPlot* pTarget)
{

	UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if(pUnit)
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
		pUnit->finishMoves();
		UnitProcessed(m_CurrentMoveUnits[0].GetID());
		pUnit->SetTacticalAIPlot(NULL);
	}
}


void CvTacticalAI::ExecuteMoveToTarget(CvPlot* pTarget)
{
	std::vector<CvTacticalUnit>::iterator it;


	for(it = m_CurrentMoveHighPriorityUnits.begin(); it != m_CurrentMoveHighPriorityUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());


		if(m_CurrentMoveUnits.size() > 0 && m_CurrentMoveUnits.begin()->GetMovesToTarget() < it->GetMovesToTarget())
		{
			break;
		}

		if(pUnit->plot() == pTarget && pUnit->canFortify(pUnit->plot()))
		{
			pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
			pUnit->SetFortifiedThisTurn(true);
			UnitProcessed(it->GetID());
			return;
		}

		else if(pUnit && it->GetMovesToTarget() < MAX_INT)
		{
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
			pUnit->finishMoves();
			UnitProcessed(it->GetID());
			return;
		}
	}


	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());

		if(pUnit->plot() == pTarget && pUnit->canFortify(pUnit->plot()))
		{
			pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
			pUnit->SetFortifiedThisTurn(true);
			UnitProcessed(it->GetID());
			return;
		}

		else if(pUnit && it->GetMovesToTarget() < MAX_INT)
		{
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
			pUnit->finishMoves();
			UnitProcessed(it->GetID());
			return;
		}
	}
}


void CvTacticalAI::ExecuteAirInterceptMoves()
{
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			if(pUnit->canAirPatrol(NULL))
			{
				pUnit->PushMission(CvTypes::getMISSION_AIRPATROL());
				UnitProcessed(m_CurrentMoveUnits[iI].GetID(), false                     );
			}
		}
	}
}


void CvTacticalAI::ExecuteAirSweepMoves()
{
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			if(pUnit->canAirSweep())
			{
				CvPlot *pTarget = m_pPlayer->GetMilitaryAI()->GetBestAirSweepTarget(pUnit.pointer());
				if (pTarget)
				{
					pUnit->PushMission(CvTypes::getMISSION_AIR_SWEEP(), pTarget->getX(), pTarget->getY());
					UnitProcessed(m_CurrentMoveUnits[iI].GetID(), false                     );
				}
			}
		}
	}
}


bool CvTacticalAI::ExecuteSafeBombards(CvTacticalTarget& kTarget)
{
	CvTacticalUnit unit;
	void* pFirstAttacker = NULL;
	UnitHandle pDefender;
	int iRequiredDamage = 0;
	bool bCityCanAttack = false;
	bool bFirstAttackCity = false;

	CvPlot* pTargetPlot = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());
	if(PlotAlreadyTargeted(pTargetPlot) != -1)
	{
		return false;
	}


	pDefender = pTargetPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
	if(pDefender)
	{
		iRequiredDamage = pDefender->GetCurrHitPoints();


		bCityCanAttack = FindCitiesWithinStrikingDistance(pTargetPlot);
		if(bCityCanAttack)
		{
			ComputeTotalExpectedBombardDamage(pDefender);


			for(unsigned int iI = 0; iI < m_CurrentMoveCities.size() && iRequiredDamage > 0; iI++)
			{
				CvCity* pCity = m_pPlayer->getCity(m_CurrentMoveCities[iI].GetID());
				if(pCity != NULL)
				{
					if(QueueAttack(pCity, &kTarget, true            , true          ))
					{
						pFirstAttacker = (void*)pCity;
						bFirstAttackCity = true;
					}


					iRequiredDamage -= m_CurrentMoveCities[iI].GetExpectedTargetDamage();
				}
			}
		}
	}


	else
	{
		CvCity* pCity = pTargetPlot->getPlotCity();
		if(pCity != NULL)
		{
			iRequiredDamage = pCity->GetMaxHitPoints() - pCity->getDamage();


			if(iRequiredDamage <= 1)
			{
				return false;
			}
		}
	}


	if(iRequiredDamage <= 0)
	{
		return false;
	}



	m_CurrentMoveUnits.clear();
	list<int>::iterator it;
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
#else
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
#endif
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit && pUnit->IsCanAttackRanged() && !pUnit->isOutOfAttacks())
		{
			int iPlotIndex = GC.getMap().plotNum(pUnit->getX(), pUnit->getY());
			CvTacticalAnalysisCell* pCell = GC.getGame().GetTacticalAnalysisMap()->GetCell(iPlotIndex);
			if(pCell->IsWithinRangeOfTarget() && !pCell->IsSubjectToAttack() && IsExpectedToDamageWithRangedAttack(pUnit, pTargetPlot))
			{
				if(pUnit->canSetUpForRangedAttack(NULL))
				{
					pUnit->setSetUpForRangedAttack(true);
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Set up %s for ranged attack", pUnit->getName().GetCString());
						LogTacticalMessage(strMsg, false);
					}
				}

				if(pUnit->canMove() && pUnit->canRangeStrikeAt(kTarget.GetTargetX(), kTarget.GetTargetY()))
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Making a safe bombard (no move) with %s, Target X: %d, Target Y: %d, At X: %d, At Y: %d", pUnit->getName().GetCString(),
						              kTarget.GetTargetX(), kTarget.GetTargetY(), pUnit->getX(), pUnit->getY());
						LogTacticalMessage(strMsg, false);
					}

					if(QueueAttack((void*)pUnit.pointer(), &kTarget, true            , false          ))
					{
						pFirstAttacker = (void*)pUnit.pointer();
					}


					unit.SetID(pUnit->GetID());
					m_CurrentMoveUnits.push_back(unit);
				}
			}
		}
	}


	std::vector<CvTacticalUnit>::iterator currMoveIt;
	for(currMoveIt = m_CurrentMoveUnits.begin(); currMoveIt != m_CurrentMoveUnits.end(); ++currMoveIt)
	{
		UnitProcessed(currMoveIt->GetID());
	}



	int iDX, iDY;
#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX;
#endif
	CvPlot* pLoopPlot;
	int iPlotIndex;
	CvTacticalAnalysisCell* pCell;

	for(int iTurnsToReach = 0; iTurnsToReach <= 2; iTurnsToReach++)
	{
		int iRange = m_pMap->GetBestFriendlyRange();

#ifdef AUI_HEXSPACE_DX_LOOPS
		for (iDY = -iRange; iDY <= iRange; iDY++)
		{
			iMaxDX = iRange - MAX(0, iDY);
			for (iDX = -iRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
#else
		for(iDX = -(iRange); iDX <= iRange; iDX++)
		{
			for(iDY = -(iRange); iDY <= iRange; iDY++)
#endif
			{
#ifdef AUI_HEXSPACE_DX_LOOPS
				if (iDX == 0 && iDY == 0)
					continue;
#endif
				pLoopPlot = plotXY(kTarget.GetTargetX(), kTarget.GetTargetY(), iDX, iDY);
				if(pLoopPlot != NULL)
				{
#ifdef AUI_FIX_HEX_DISTANCE_INSTEAD_OF_PLOT_DISTANCE
					int iDistance = hexDistance(iDX, iDY);
#else
					int iDistance = plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), kTarget.GetTargetX(), kTarget.GetTargetY());
#endif
#ifndef AUI_HEXSPACE_DX_LOOPS
					if(iDistance > 0 && iDistance <= iRange)
#endif
					{
						iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
						pCell = m_pMap->GetCell(iPlotIndex);
						if(pCell->IsRevealed() && pCell->CanUseForOperationGathering())
						{
							if(pCell->IsWithinRangeOfTarget() && !pCell->IsSubjectToAttack())
							{
								bool bHaveLOS = pLoopPlot->canSeePlot(pTargetPlot, m_pPlayer->getTeam(), iRange, NO_DIRECTION);
								if(FindClosestUnit(pLoopPlot, iTurnsToReach, false                   , true                     , iDistance, !bHaveLOS, false                    , true                , pTargetPlot))
								{
									UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
									if(pUnit)
									{

										UnitHandle pBlockingUnit = pLoopPlot->getBestDefender(m_pPlayer->GetID());
										if(!pBlockingUnit || ExecuteMoveOfBlockingUnit(pBlockingUnit))
										{
											pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pLoopPlot->getX(), pLoopPlot->getY());

											if(GC.getLogging() && GC.getAILogging())
											{
												CvString strMsg;
												strMsg.Format("Moving closer for safe bombard with %s, Target X: %d, Y: %d, Bombard From X: %d, Y: %d, Now At X: %d, Y: %d", pUnit->getName().GetCString(),
												              kTarget.GetTargetX(), kTarget.GetTargetY(), pLoopPlot->getX(), pLoopPlot->getY(), pUnit->getX(), pUnit->getY());
												LogTacticalMessage(strMsg, false);
											}

											UnitProcessed(m_CurrentMoveUnits[0].GetID());

											if(pUnit->canSetUpForRangedAttack(NULL))
											{
												pUnit->setSetUpForRangedAttack(true);
												if(GC.getLogging() && GC.getAILogging())
												{
													CvString strMsg;
													strMsg.Format("Set up %s for ranged attack", pUnit->getName().GetCString());
													LogTacticalMessage(strMsg, false);
												}
											}

											if(pUnit->canMove() && !pUnit->isOutOfAttacks() && pUnit->canRangeStrikeAt(kTarget.GetTargetX(), kTarget.GetTargetY()))
											{
												if(GC.getLogging() && GC.getAILogging())
												{
													CvString strMsg;
													strMsg.Format("Making a safe bombard (half move) with %s, Target X: %d, Target Y: %d, At X: %d, At Y: %d", pUnit->getName().GetCString(),
													              kTarget.GetTargetX(), kTarget.GetTargetY(), pUnit->getX(), pUnit->getY());
													LogTacticalMessage(strMsg, false);
												}

												if(QueueAttack((void*)pUnit.pointer(), &kTarget, true            , false                ))
												{
													pFirstAttacker = (void*)pUnit.pointer();
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
		}
	}


	if(pFirstAttacker)
	{
		LaunchAttack(pFirstAttacker, &kTarget, true           , true                       , bFirstAttackCity);
		return true;
	}
	return false;
}


bool CvTacticalAI::ExecuteProtectedBombards(CvTacticalTarget& kTarget, bool bAttackUnderway)
{
	bool bAttackMade = true;
	bool bAtLeastOneAttackInitiated = false;

	while(bAttackMade && kTarget.IsTargetStillAlive(m_pPlayer->GetID()))
	{
		bAttackMade = ExecuteOneProtectedBombard(kTarget);
		if(bAttackMade)
		{
			bAttackUnderway = true;
			bAtLeastOneAttackInitiated = true;
		}
	}
	return bAtLeastOneAttackInitiated;
}


bool CvTacticalAI::ExecuteOneProtectedBombard(CvTacticalTarget& kTarget)
{
	UnitHandle pFirstAttacker;
	int iDX, iDY;
#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX;
#endif
	CvPlot* pAttackPlot;
	int iPlotIndex;
	CvTacticalAnalysisCell* pCell;
	int iPriority;
	int iNumTurns;
	int iNumUnitsRequiredToCover;

	CvPlot* pTargetPlot = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());
	if(PlotAlreadyTargeted(pTargetPlot) != -1)
	{
		return false;
	}

	CvCity* pCity = pTargetPlot->getPlotCity();
	if(pCity != NULL)
	{
		int iRequiredDamage = pCity->GetMaxHitPoints() - pCity->getDamage();


		if(iRequiredDamage <= 1)
		{
			return false;
		}
	}

	int iRange = m_pMap->GetBestFriendlyRange();
	m_TempTargets.clear();


#ifdef AUI_HEXSPACE_DX_LOOPS
	for (iDY = -iRange; iDY <= iRange; iDY++)
	{
		iMaxDX = iRange - MAX(0, iDY);
		for (iDX = -iRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
#else
	for(iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for(iDY = -(iRange); iDY <= iRange; iDY++)
#endif
		{
#ifdef AUI_HEXSPACE_DX_LOOPS
			if (iDY == 0 && iDX == 0)
				continue;
#endif
			pAttackPlot = plotXY(kTarget.GetTargetX(), kTarget.GetTargetY(), iDX, iDY);
			if(pAttackPlot != NULL)
			{
#ifdef AUI_FIX_HEX_DISTANCE_INSTEAD_OF_PLOT_DISTANCE
				int iPlotDistance = hexDistance(iDX, iDY);
#else
				int iPlotDistance = plotDistance(pAttackPlot->getX(), pAttackPlot->getY(), kTarget.GetTargetX(), kTarget.GetTargetY());
#endif
#ifndef AUI_HEXSPACE_DX_LOOPS
				if(iPlotDistance > 0 && iPlotDistance <= iRange)
#endif
				{
					iPlotIndex = GC.getMap().plotNum(pAttackPlot->getX(), pAttackPlot->getY());
					pCell = m_pMap->GetCell(iPlotIndex);
					if(pCell->IsRevealed() && pCell->CanUseForOperationGathering())
					{
						if(pCell->IsWithinRangeOfTarget())
						{

							CvPlot* pLoopPlot;
							bool bNoEnemyAdjacent = true;
							for(int iI = 0; iI < NUM_DIRECTION_TYPES && bNoEnemyAdjacent; iI++)
							{
								pLoopPlot = plotDirection(pAttackPlot->getX(), pAttackPlot->getY(), ((DirectionTypes)iI));
								if(pLoopPlot != NULL)
								{
									iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
									if(m_pMap->GetCell(iPlotIndex)->GetEnemyMilitaryUnit() != NULL)
									{
										bNoEnemyAdjacent = false;
									}
								}
							}

							if(bNoEnemyAdjacent)
							{

								iNumTurns = -1;
								bool bHaveLOS = pAttackPlot->canSeePlot(pTargetPlot, m_pPlayer->getTeam(), iRange, NO_DIRECTION);
								if(FindClosestUnit(pAttackPlot, 0, false                   , true                      , iPlotDistance, !bHaveLOS, false                    ,  false                , pTargetPlot))
								{
									iNumTurns = 0;
								}

								else if(FindClosestUnit(pAttackPlot, 1, false                   , true                      , iPlotDistance, !bHaveLOS, false                    ,  false                ,  pTargetPlot))
								{
									iNumTurns = 1;
								}


								if(iNumTurns >= 0)
								{
									if(CanCoverFromEnemy(pAttackPlot, iNumUnitsRequiredToCover, m_CurrentMoveUnits[0].GetID()))
									{
										CvTacticalTarget target;
										target.SetTargetX(pAttackPlot->getX());
										target.SetTargetY(pAttackPlot->getY());



										iPriority = 300 - (iNumTurns * 100);
										iPriority -= 40 * iNumUnitsRequiredToCover;
										target.SetAuxIntData(iPriority);
										m_TempTargets.push_back(target);
									}
								}
							}
						}
					}
				}
			}
		}
	}


	if(m_TempTargets.size() == 0)
	{
		return false;
	}


	std::stable_sort(m_TempTargets.begin(), m_TempTargets.end());


#ifdef AUI_ITERATORIZE
	pAttackPlot = GC.getMap().plot(m_TempTargets.begin()->GetTargetX(), m_TempTargets.begin()->GetTargetY());
#else
	pAttackPlot = GC.getMap().plot(m_TempTargets[0].GetTargetX(), m_TempTargets[0].GetTargetY());
#endif
#ifdef AUI_WARNING_FIXES
	if (!pAttackPlot)
		return false;
#endif
	int iPlotDistance = plotDistance(pAttackPlot->getX(), pAttackPlot->getY(), kTarget.GetTargetX(), kTarget.GetTargetY());
	bool bHaveLOS = pAttackPlot->canSeePlot(pTargetPlot, m_pPlayer->getTeam(), iRange, NO_DIRECTION);
	if(FindClosestUnit(pAttackPlot, 0, false                   , true                      , iPlotDistance, !bHaveLOS, false                    ,  false                , pTargetPlot) ||
	        FindClosestUnit(pAttackPlot, 1, false                   , true                      , iPlotDistance, !bHaveLOS, false                    ,  false                , pTargetPlot))
	{
#ifdef AUI_ITERATORIZE
		int iAttackingUnitID = m_CurrentMoveUnits.begin()->GetID();
#else
		int iAttackingUnitID = m_CurrentMoveUnits[0].GetID();
#endif
#ifdef AUI_WARNING_FIXES
		if(CanCoverFromEnemy(pAttackPlot, iNumUnitsRequiredToCover, iAttackingUnitID))
#else
		if(pAttackPlot && CanCoverFromEnemy(pAttackPlot, iNumUnitsRequiredToCover, iAttackingUnitID))
#endif
		{

#ifdef AUI_ITERATORIZE
			for (FStaticVector<CvBlockingUnit, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::iterator it = m_ChosenBlocks.begin(); it != m_ChosenBlocks.end(); ++it)
			{
				CvUnit* pChosenUnit = m_pPlayer->getUnit(it->GetUnitID());
				if (pChosenUnit)
				{
					if (pChosenUnit->plot() != it->GetPlot())
#else
			for(unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
			{
				CvUnit* pChosenUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());
				if(pChosenUnit)
				{
					if (pChosenUnit->plot() != m_ChosenBlocks[iI].GetPlot())
#endif
					{
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Moving to cover a protected bombard with %s, X: %d, Y: %d", pChosenUnit->getName().GetCString(),
#ifdef AUI_ITERATORIZE
								it->GetPlot()->getX(), it->GetPlot()->getY());
#else
								m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
#endif
							LogTacticalMessage(strMsg, false);
						}
#ifdef AUI_ITERATORIZE
						pChosenUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), it->GetPlot()->getX(), it->GetPlot()->getY());
#else
						pChosenUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
#endif
					}
					else if (pChosenUnit->canPillage(pChosenUnit->plot()) && pChosenUnit->getDamage() > 0)
					{
						pChosenUnit->PushMission(CvTypes::getMISSION_PILLAGE());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Pillaging during a protected bombard with %s, X: %d, Y: %d", pChosenUnit->getName().GetCString(),
#ifdef AUI_ITERATORIZE
								it->GetPlot()->getX(), it->GetPlot()->getY());
#else
								m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
#endif
							LogTacticalMessage(strMsg, false);
						}
					}
					else if(pChosenUnit->canFortify(pChosenUnit->plot()))
					{
						pChosenUnit->PushMission(CvTypes::getMISSION_FORTIFY());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Fortifying during a protected bombard with %s, X: %d, Y: %d", pChosenUnit->getName().GetCString(),
#ifdef AUI_ITERATORIZE
								it->GetPlot()->getX(), it->GetPlot()->getY());
#else
								m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
#endif
							LogTacticalMessage(strMsg, false);
						}
					}
					else
					{
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Sitting during a protected bombard with %s, X: %d, Y: %d", pChosenUnit->getName().GetCString(),
#ifdef AUI_ITERATORIZE
								it->GetPlot()->getX(), it->GetPlot()->getY());
#else
								m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
#endif
							LogTacticalMessage(strMsg, false);
						}
					}
					pChosenUnit->finishMoves();
					UnitProcessed(pChosenUnit->GetID());
				}
			}


			CvUnit* pUnit = m_pPlayer->getUnit(iAttackingUnitID);
			if(pUnit)
			{
				if(pAttackPlot != pUnit->plot())
				{
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pAttackPlot->getX(), pAttackPlot->getY());

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Moving closer for protected bombard with %s, Target X: %d, Target Y: %d, At X: %d, At Y: %d", pUnit->getName().GetCString(),
						              pAttackPlot->getX(), pAttackPlot->getY(), pUnit->getX(), pUnit->getY());
						LogTacticalMessage(strMsg, false);
					}
				}

				UnitProcessed(iAttackingUnitID);

				if(pUnit->canSetUpForRangedAttack(NULL))
				{
					pUnit->setSetUpForRangedAttack(true);
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Set up %s for ranged attack", pUnit->getName().GetCString());
						LogTacticalMessage(strMsg, false);
					}
				}

				if(pUnit->canMove() && !pUnit->isOutOfAttacks() && pUnit->canRangeStrikeAt(kTarget.GetTargetX(), kTarget.GetTargetY()))
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Making a protected bombard with %s, Target X: %d, Target Y: %d, At X: %d, At Y: %d", pUnit->getName().GetCString(),
						              kTarget.GetTargetX(), kTarget.GetTargetY(), pUnit->getX(), pUnit->getY());
						LogTacticalMessage(strMsg, false);
					}

					if(QueueAttack((void*)pUnit, &kTarget, true            , false                ))
					{
						pFirstAttacker = pUnit;
					}
				}
			}
		}

		if(pFirstAttacker)
		{
			LaunchAttack((void*)pFirstAttacker.pointer(), &kTarget, true           , true                       , false                );
			return true;
		}
	}

	return false;
}


bool CvTacticalAI::ExecuteFlankAttack(CvTacticalTarget& kTarget)
{
	CvTacticalAnalysisCell* pCell;
	CvPlot* pLoopPlot;
	int iPlotIndex;
	int iPossibleFlankHexes = 0;
	CvTacticalTarget target;


	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(kTarget.GetTargetX(), kTarget.GetTargetY(), ((DirectionTypes)iI));
		if(pLoopPlot != NULL)
		{
			iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
			pCell = m_pMap->GetCell(iPlotIndex);
			if(pCell->IsHelpsProvidesFlankBonus())
			{
				iPossibleFlankHexes++;
			}
		}
	}


	if(iPossibleFlankHexes > 1)
	{
		int iFillableHexes = 0;
		int iNumAttackers = 0;
		m_TempTargets.clear();
		for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			pLoopPlot = plotDirection(kTarget.GetTargetX(), kTarget.GetTargetY(), ((DirectionTypes)iI));
			if(pLoopPlot != NULL)
			{
				iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
				pCell = m_pMap->GetCell(iPlotIndex);
				if(pCell->IsHelpsProvidesFlankBonus())
				{
					if(FindClosestUnit(pLoopPlot, 0, true                   , false                      , 0                   , false                    , true                     ))
					{
						iFillableHexes++;
						iNumAttackers++;
						target.SetTargetX(pLoopPlot->getX());
						target.SetTargetY(pLoopPlot->getY());
						m_TempTargets.push_back(target);
					}

					else if(FindClosestUnit(pLoopPlot, 1, false                   , false                      , 0                   , false                    , true                     ))
					{
						iFillableHexes++;
						target.SetTargetX(pLoopPlot->getX());
						target.SetTargetY(pLoopPlot->getY());
						m_TempTargets.push_back(target);
					}
				}
			}
		}


		if((iNumAttackers >= 1 && iFillableHexes >= 3) || iNumAttackers >= 2)
		{

			m_PotentialBlocks.clear();
#ifdef AUI_ITERATORIZE
			for (FStaticVector<CvTacticalTarget, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::const_iterator it = m_TempTargets.begin(); it != m_TempTargets.end(); ++it)
			{
				pLoopPlot = GC.getMap().plot(it->GetTargetX(), it->GetTargetY());
#else
			for(unsigned int iI = 0; iI < m_TempTargets.size(); iI++)
			{
				pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
#endif
				if(!FindClosestUnit(pLoopPlot, 1, false                   , false                      , 0                   , false                    , true                     ))
				{
					iFillableHexes--;
					if(iFillableHexes < 2)
					{
						return false;
					}
				}
				else
				{

#ifdef AUI_ITERATORIZE
					for (std::vector<CvTacticalUnit>::const_iterator it2 = m_CurrentMoveUnits.begin(); it2 != m_CurrentMoveUnits.end(); ++it2)
					{
						CvBlockingUnit block;
						block.SetUnitID(it2->GetID());
						block.SetPlot(pLoopPlot);
						block.SetNumChoices((int)m_CurrentMoveUnits.size());
						block.SetDistanceToTarget(it2->GetMovesToTarget());
#else
					for(unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
					{
						CvBlockingUnit block;
						block.SetUnitID(m_CurrentMoveUnits[jJ].GetID());
						block.SetPlot(pLoopPlot);
						block.SetNumChoices((int)m_CurrentMoveUnits.size());
						block.SetDistanceToTarget(m_CurrentMoveUnits[jJ].GetMovesToTarget());
#endif
						m_PotentialBlocks.push_back(block);
					}
				}
			}

			if(AssignFlankingUnits(iFillableHexes))
			{

#ifdef AUI_ITERATORIZE
				for (FStaticVector<CvBlockingUnit, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::iterator it = m_ChosenBlocks.begin(); it != m_ChosenBlocks.end(); ++it)
				{
					CvUnit* pUnit = m_pPlayer->getUnit(it->GetUnitID());
					if (pUnit)
					{
						if (pUnit->plot() != it->GetPlot())
#else
				for(unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
				{
					CvUnit* pUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());
					if(pUnit)
					{
						if(pUnit->plot() == m_ChosenBlocks[iI].GetPlot())
#endif
						{
							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strMsg;
								strMsg.Format("Already in a flanking position with %s, X: %d, Y: %d", pUnit->getName().GetCString(),
#ifdef AUI_ITERATORIZE
											  it->GetPlot()->getX(), it->GetPlot()->getY());
#else
								              m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
#endif
								LogTacticalMessage(strMsg, false);
							}
						}
						else
						{
#ifdef AUI_ITERATORIZE
							pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), it->GetPlot()->getX(), it->GetPlot()->getY());
#else
							pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
#endif
							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strMsg;
								strMsg.Format("Moving into a flanking position with %s, X: %d, Y: %d", pUnit->getName().GetCString(),
#ifdef AUI_ITERATORIZE
											  it->GetPlot()->getX(), it->GetPlot()->getY());
#else
								              m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
#endif
								LogTacticalMessage(strMsg, false);
							}

							if(pUnit->getMoves() <= 0)
							{
								UnitProcessed(pUnit->GetID());
							}
						}
					}
				}


				CvPlot* pTargetPlot = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());
				UnitHandle pDefender = pTargetPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
				if(pDefender)
				{
					kTarget.SetAuxIntData(pDefender->GetCurrHitPoints());
					m_CurrentMoveCities.clear();
					if(FindUnitsWithinStrikingDistance(pTargetPlot, 1, 0, false                     ))
					{
						ComputeTotalExpectedDamage(&kTarget, pTargetPlot);
						ExecuteAttack(&kTarget, pTargetPlot, false                      , true                      );
					}
				}
			}
		}
	}

	return false;
}


void CvTacticalAI::ExecuteCloseOnTarget(CvTacticalTarget& kTarget, CvTacticalDominanceZone* pZone)
{
	CvOperationUnit unit;
	int iDistance;
	CvPlot* pTargetPlot;
	int iRangedUnits = 0;
	int iMeleeUnits = 0;
	int iGenerals = 0;
	list<int>::iterator it;
	int iTacticalRadius = GC.getGame().GetTacticalAnalysisMap()->GetTacticalRange();

	pTargetPlot = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());
	m_OperationUnits.clear();
	m_GeneralsToMove.clear();

#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
#else
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
#endif
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{

			if(pZone->IsNavalInvasion() ||
			        (pZone->IsWater() && pUnit->getDomainType() == DOMAIN_SEA || !pZone->IsWater() && pUnit->getDomainType() == DOMAIN_LAND))
			{

				iDistance = plotDistance(pUnit->getX(), pUnit->getY(), kTarget.GetTargetX(), kTarget.GetTargetY());
				if(iDistance <= iTacticalRadius || (iDistance <= (GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE() * 3) && pUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn()))
				{
					unit.SetUnitID(pUnit->GetID());

					if(pUnit->IsCanAttackRanged())
					{
						unit.SetPosition((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_BOMBARD]);
						iRangedUnits++;
						m_OperationUnits.push_back(unit);

					}
					else if(pUnit->IsGreatGeneral() || pUnit->IsGreatAdmiral())
					{
						unit.SetPosition((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_CIVILIAN_SUPPORT]);
						iGenerals++;
						m_GeneralsToMove.push_back(unit);
					}
					else
					{
						unit.SetPosition((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_FRONT_LINE]);
						iMeleeUnits++;
						m_OperationUnits.push_back(unit);
					}
				}
			}
		}
	}


	if(m_OperationUnits.size() > 0)
	{

		bool bLandOnly = true;
		if (pZone->IsNavalInvasion() || pZone->GetEnemyNavalUnitCount() == 0)
		{
			bLandOnly = false;
		}
		ScoreCloseOnPlots(pTargetPlot, bLandOnly);


		std::stable_sort(m_TempTargets.begin(), m_TempTargets.end());
		m_PotentialBlocks.clear();

		int iRangedUnitsToPlace = iRangedUnits;
		int iMeleeUnitsToPlace = iMeleeUnits;


		bool bDone = false;
#ifdef AUI_ITERATORIZE
		for (FStaticVector<CvTacticalTarget, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::const_iterator it3 = m_TempTargets.begin(); it3 != m_TempTargets.end(); ++it3)
		{
			AITacticalTargetType eTargetType = it3->GetTargetType();
			if (eTargetType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				CvPlot* pLoopPlot = GC.getMap().plot(it3->GetTargetX(), it3->GetTargetY());
#else
		for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			AITacticalTargetType eTargetType = m_TempTargets[iI].GetTargetType();
			if (eTargetType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				CvPlot* pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
#endif
				if(FindClosestOperationUnit(pLoopPlot, true                   , true                      ))
				{
#ifdef AUI_ITERATORIZE
					for (std::vector<CvTacticalUnit>::const_iterator it2 = m_CurrentMoveUnits.begin(); it2 != m_CurrentMoveUnits.end(); ++it2)
					{
						CvBlockingUnit block;
						block.SetUnitID(it2->GetID());
						block.SetPlot(pLoopPlot);
						block.SetNumChoices((int)m_CurrentMoveUnits.size());
						block.SetDistanceToTarget(it2->GetMovesToTarget());
#else
					for(unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
					{
						CvBlockingUnit block;
						block.SetUnitID(m_CurrentMoveUnits[jJ].GetID());
						block.SetPlot(pLoopPlot);
						block.SetNumChoices((int)m_CurrentMoveUnits.size());
						block.SetDistanceToTarget(m_CurrentMoveUnits[jJ].GetMovesToTarget());
#endif
						m_PotentialBlocks.push_back(block);
					}

					iRangedUnitsToPlace--;
					if(iRangedUnitsToPlace == 0)
					{
						bDone = true;
					}
				}
			}
		}
		AssignDeployingUnits(iRangedUnits - iRangedUnitsToPlace);
		PerformChosenMoves();


		m_PotentialBlocks.clear();
		iMeleeUnits += iRangedUnitsToPlace;
		iMeleeUnitsToPlace += iRangedUnitsToPlace;
		bDone = false;
#ifdef AUI_ITERATORIZE
		for (FStaticVector<CvTacticalTarget, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::const_iterator it3 = m_TempTargets.begin(); it3 != m_TempTargets.end(); ++it3)
		{
			AITacticalTargetType eTargetType = it3->GetTargetType();
			if (eTargetType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				CvPlot* pLoopPlot = GC.getMap().plot(it3->GetTargetX(), it3->GetTargetY());
#else
		for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			AITacticalTargetType eTargetType = m_TempTargets[iI].GetTargetType();
			if (eTargetType != AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				CvPlot* pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
#endif
				if(FindClosestOperationUnit(pLoopPlot, true                   , false                      ))
				{
#ifdef AUI_ITERATORIZE
					for (std::vector<CvTacticalUnit>::const_iterator it2 = m_CurrentMoveUnits.begin(); it2 != m_CurrentMoveUnits.end(); ++it2)
					{
						CvBlockingUnit block;
						block.SetUnitID(it2->GetID());
						block.SetPlot(pLoopPlot);
						block.SetNumChoices((int)m_CurrentMoveUnits.size());
						block.SetDistanceToTarget(it2->GetMovesToTarget());
#else
					for(unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
					{
						CvBlockingUnit block;
						block.SetUnitID(m_CurrentMoveUnits[jJ].GetID());
						block.SetPlot(pLoopPlot);
						block.SetNumChoices((int)m_CurrentMoveUnits.size());
						block.SetDistanceToTarget(m_CurrentMoveUnits[jJ].GetMovesToTarget());
#endif
						m_PotentialBlocks.push_back(block);
					}

					iMeleeUnitsToPlace--;
					if(iMeleeUnitsToPlace == 0)
					{
						bDone = true;
					}
				}
			}
		}
		AssignDeployingUnits(iMeleeUnits - iMeleeUnitsToPlace);
		PerformChosenMoves();
	}

	if(m_GeneralsToMove.size() > 0)
	{
		MoveGreatGeneral();
	}
}


void CvTacticalAI::ExecuteHedgehogDefense(CvTacticalTarget& kTarget, CvTacticalDominanceZone* pZone)
{
	CvOperationUnit unit;
	int iDistance;
	CvPlot* pTargetPlot;
	int iRangedUnits = 0;
	int iMeleeUnits = 0;
	list<int>::iterator it;
	int iTacticalRadius = GC.getGame().GetTacticalAnalysisMap()->GetTacticalRange();

	pTargetPlot = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());
	m_OperationUnits.clear();
	m_GeneralsToMove.clear();

#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
#else
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
#endif
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{

			if(pZone->IsWater() && pUnit->getDomainType() == DOMAIN_SEA || !pZone->IsWater() && pUnit->getDomainType() == DOMAIN_LAND)
			{

				iDistance = plotDistance(pUnit->getX(), pUnit->getY(), kTarget.GetTargetX(), kTarget.GetTargetY());
				if(iDistance <= iTacticalRadius)
				{
					unit.SetUnitID(pUnit->GetID());

					if (pUnit->IsGreatGeneral())
					{
						if(pUnit->getMoves() > 0)
						{
							CvOperationUnit operationUnit;
							operationUnit.SetUnitID(pUnit->GetID());
							operationUnit.SetPosition((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_CIVILIAN_SUPPORT]);
							m_GeneralsToMove.push_back(operationUnit);
						}
					}
					else if (pUnit->IsCanAttackRanged())
					{
						unit.SetPosition((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_BOMBARD]);
						iRangedUnits++;
						m_OperationUnits.push_back(unit);
					}
					else
					{
						unit.SetPosition((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_FRONT_LINE]);
						iMeleeUnits++;
						m_OperationUnits.push_back(unit);
					}
				}
			}
		}
	}


	if(m_OperationUnits.size() > 0)
	{
		ScoreHedgehogPlots(pTargetPlot);


		std::stable_sort(m_TempTargets.begin(), m_TempTargets.end());
		m_PotentialBlocks.clear();

		int iRangedUnitsToPlace = iRangedUnits;
		int iMeleeUnitsToPlace = iMeleeUnits;


		bool bDone = false;
#ifdef AUI_ITERATORIZE
		for (FStaticVector<CvTacticalTarget, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::const_iterator it3 = m_TempTargets.begin(); it3 != m_TempTargets.end() && !bDone; ++it3)
		{
			AITacticalTargetType eTargetType = it3->GetTargetType();
			if (eTargetType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				CvPlot* pLoopPlot = GC.getMap().plot(it3->GetTargetX(), it3->GetTargetY());
#else
		for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			AITacticalTargetType eTargetType = m_TempTargets[iI].GetTargetType();
			if (eTargetType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				CvPlot* pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
#endif
				if(FindClosestOperationUnit(pLoopPlot, true                   , true                      ))
				{
#ifdef AUI_ITERATORIZE
					for (std::vector<CvTacticalUnit>::const_iterator it2 = m_CurrentMoveUnits.begin(); it2 != m_CurrentMoveUnits.end(); ++it2)
					{
						CvBlockingUnit block;
						block.SetUnitID(it2->GetID());
						block.SetPlot(pLoopPlot);
						block.SetNumChoices((int)m_CurrentMoveUnits.size());
						block.SetDistanceToTarget(it2->GetMovesToTarget());
#else
					for(unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
					{
						CvBlockingUnit block;
						block.SetUnitID(m_CurrentMoveUnits[jJ].GetID());
						block.SetPlot(pLoopPlot);
						block.SetNumChoices((int)m_CurrentMoveUnits.size());
						block.SetDistanceToTarget(m_CurrentMoveUnits[jJ].GetMovesToTarget());
#endif
						m_PotentialBlocks.push_back(block);
					}

					iRangedUnitsToPlace--;
					if(iRangedUnitsToPlace == 0)
					{
						bDone = true;
					}
				}
			}
		}
		AssignDeployingUnits(iRangedUnits - iRangedUnitsToPlace);
		PerformChosenMoves();


		m_PotentialBlocks.clear();
		iMeleeUnits += iRangedUnitsToPlace;
		iMeleeUnitsToPlace += iRangedUnitsToPlace;
		bDone = false;
#ifdef AUI_ITERATORIZE
		for (FStaticVector<CvTacticalTarget, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::const_iterator it3 = m_TempTargets.begin(); it3 != m_TempTargets.end() && !bDone; ++it3)
		{
			AITacticalTargetType eTargetType = it3->GetTargetType();
			if (eTargetType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				CvPlot* pLoopPlot = GC.getMap().plot(it3->GetTargetX(), it3->GetTargetY());
#else
		for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			AITacticalTargetType eTargetType = m_TempTargets[iI].GetTargetType();
			if (eTargetType != AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				CvPlot* pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
#endif
				if(FindClosestOperationUnit(pLoopPlot, true                   , false                      ))
				{
#ifdef AUI_ITERATORIZE
					for (std::vector<CvTacticalUnit>::const_iterator it2 = m_CurrentMoveUnits.begin(); it2 != m_CurrentMoveUnits.end(); ++it2)
					{
						CvBlockingUnit block;
						block.SetUnitID(it2->GetID());
						block.SetPlot(pLoopPlot);
						block.SetNumChoices((int)m_CurrentMoveUnits.size());
						block.SetDistanceToTarget(it2->GetMovesToTarget());
#else
					for(unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
					{
						CvBlockingUnit block;
						block.SetUnitID(m_CurrentMoveUnits[jJ].GetID());
						block.SetPlot(pLoopPlot);
						block.SetNumChoices((int)m_CurrentMoveUnits.size());
						block.SetDistanceToTarget(m_CurrentMoveUnits[jJ].GetMovesToTarget());
#endif
						m_PotentialBlocks.push_back(block);
					}

					iMeleeUnitsToPlace--;
					if(iMeleeUnitsToPlace == 0)
					{
						bDone = true;
					}
				}
			}
		}
		AssignDeployingUnits(iMeleeUnits - iMeleeUnitsToPlace);
		PerformChosenMoves();
	}

	if(m_GeneralsToMove.size() > 0)
	{
		MoveGreatGeneral();
	}
}


void CvTacticalAI::ExecutePriorityAttacksOnUnitTarget(CvTacticalTarget& kTarget)
{
	bool bAttackUnderway = false;
	bool bAttackMade = false;
	CvPlot* pTarget;

	pTarget = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());


	m_pMap->ClearDynamicFlags();
	m_pMap->SetTargetBombardCells(pTarget, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

	if(kTarget.IsTargetStillAlive(m_pPlayer->GetID()))
	{
		bAttackUnderway = ExecuteSafeBombards(kTarget);
	}

	if(kTarget.IsTargetStillAlive(m_pPlayer->GetID()))
	{
		ExecuteProtectedBombards(kTarget, bAttackUnderway);
	}


	if(kTarget.IsTargetStillAlive(m_pPlayer->GetID()))
	{
		m_pMap->ClearDynamicFlags();
		m_pMap->SetTargetFlankBonusCells(pTarget);

		bAttackMade = ExecuteFlankAttack(kTarget);
		if(bAttackMade)
		{
			bAttackUnderway = true;
		}
	}
}


void CvTacticalAI::ExecuteWithdrawMoves()
{
	CvCity* pNearestCity;
	int iTurnsToReachTarget;

#ifdef AUI_ITERATORIZE
	for (std::vector<CvTacticalUnit>::iterator it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if (pUnit)
		{

			pNearestCity = GC.getMap().findCity(pUnit->getX(), pUnit->getY(), m_pPlayer->GetID(), NO_TEAM, true                );
			if (pNearestCity != NULL)
			{
				iTurnsToReachTarget = TurnsToReachTarget(pUnit, pNearestCity->plot());
				it->SetMovesToTarget(iTurnsToReachTarget);
				it->SetAttackStrength(1000 - iTurnsToReachTarget);
				it->SetHealthPercent(10, 10);
#else
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{

			pNearestCity = GC.getMap().findCity(pUnit->getX(), pUnit->getY(), m_pPlayer->GetID(), NO_TEAM, true                );
			if(pNearestCity != NULL)
			{
				iTurnsToReachTarget = TurnsToReachTarget(pUnit, pNearestCity->plot());
				m_CurrentMoveUnits[iI].SetMovesToTarget(iTurnsToReachTarget);
				m_CurrentMoveUnits[iI].SetAttackStrength(1000-iTurnsToReachTarget);
				m_CurrentMoveUnits[iI].SetHealthPercent(10,10);
#endif
			}
		}
	}


	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());


#ifdef AUI_ITERATORIZE
	for (std::vector<CvTacticalUnit>::iterator it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
#else
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
#endif
		if(pUnit)
		{

			pNearestCity = GC.getMap().findCity(pUnit->getX(), pUnit->getY(), m_pPlayer->GetID(), NO_TEAM, true                );
			if(pNearestCity != NULL)
			{
				MoveToEmptySpaceNearTarget(pUnit, pNearestCity->plot(), (pUnit->getDomainType()==DOMAIN_LAND));

				pUnit->finishMoves();
#ifdef AUI_ITERATORIZE
				UnitProcessed(it->GetID(), pUnit->IsCombatUnit());
#else
				UnitProcessed(m_CurrentMoveUnits[iI].GetID(), pUnit->IsCombatUnit());
#endif

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("%s withdrew toward %s, Current X: %d, Current Y: %d", pUnit->getName().GetCString(), pNearestCity->getName().GetCString(),
					                    pUnit->getX(), pUnit->getY());
					LogTacticalMessage(strLogString, false);
				}
			}
		}
	}
}


void CvTacticalAI::ExecuteEscortEmbarkedMoves()
{
#ifdef AUI_ITERATORIZE
	for (std::vector<CvTacticalUnit>::iterator it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
#else
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
#endif
		if(pUnit)
		{
			CvPlot *pBestTarget = NULL;
			int iHighestDanger = -1;
			int iMovementRate = pUnit->getMoves() / GC.getMOVE_DENOMINATOR();


			UnitHandle pLoopUnit;
			int iLoop;
			for(pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
			{
				if (pLoopUnit->getDomainType() != DOMAIN_LAND)
				{
					continue;
				}
				else if (!pLoopUnit->isEmbarked())
				{
					continue;
				}
				else if (pLoopUnit->plot()->getNumUnits() > 1)
				{
					continue;
				}
				else if (pLoopUnit->canMove())
				{
					continue;
				}
				else if (plotDistance(pUnit->getX(), pUnit->getY(), pLoopUnit->getX(), pLoopUnit->getY()) > iMovementRate)
				{
					continue;
				}


				CvPlot *pTarget = pLoopUnit->plot();
				if (TurnsToReachTarget(pUnit, pTarget) <= 1)
				{
					int iDanger = m_pPlayer->GetPlotDanger(*pTarget);
					if (iDanger > iHighestDanger)
					{
						iHighestDanger = iDanger;
						pBestTarget = pTarget;
					}
				}
			}

			if (pBestTarget)
			{
				ExecuteMoveToPlot(pUnit, pBestTarget);
#ifdef AUI_ITERATORIZE
				UnitProcessed(it->GetID());
#else
				UnitProcessed(m_CurrentMoveUnits[iI].GetID());
#endif

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("%s escorted embarked unit at, Current X: %d, Current Y: %d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
					LogTacticalMessage(strLogString, false);
				}
			}
		}
	}
}


void CvTacticalAI::TurnOffMove(TacticalAIMoveTypes eType)
{
	FStaticVector<CvTacticalMove, 256, true, c_eCiv5GameplayDLL >::iterator it;

#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_MovePriorityList.begin(); it != m_MovePriorityList.end(); ++it)
#else
	for(it = m_MovePriorityList.begin(); it != m_MovePriorityList.end(); it++)
#endif
	{
		if(it->m_eMoveType == eType)
		{
			it->m_iPriority = -1;
			return;
		}
	}
}


bool CvTacticalAI::FindUnitsForThisMove(TacticalAIMoveTypes eMove, CvPlot* pTarget, int iNumTurnsAway                                , bool bRangedOnly)
{
	UnitHandle pLoopUnit;
	bool rtnValue = false;

	list<int>::iterator it;
	m_CurrentMoveUnits.clear();
	m_CurrentMoveHighPriorityUnits.clear();


#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
#else
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
#endif
	{
		pLoopUnit = m_pPlayer->getUnit(*it);
		if(pLoopUnit && pLoopUnit->getDomainType() != DOMAIN_AIR && pLoopUnit->IsCombatUnit())
		{

			if(pLoopUnit->getDomainType() == DOMAIN_SEA && !pTarget->isWater() ||
			        pLoopUnit->getDomainType() == DOMAIN_LAND && pTarget->isWater())
			{
				continue;
			}

			bool bSuitableUnit = false;
			bool bHighPriority = false;

			if(eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GARRISON_ALREADY_THERE] ||
			        eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GARRISON_1_TURN])
			{

				if(pLoopUnit->isRanged())
				{
					bSuitableUnit = true;
					bHighPriority = true;
				}

				else if(bRangedOnly)
				{
					continue;
				}


				if(pLoopUnit->getDefenseModifier() == 0 &&
				        pLoopUnit->getAttackModifier() == 0 &&
				        pLoopUnit->getExtraCombatPercent() == 0)
				{
					bSuitableUnit = true;
				}
			}

			else if(eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GUARD_IMPROVEMENT_ALREADY_THERE] ||
			        eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GUARD_IMPROVEMENT_1_TURN] ||
			        eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_BASTION_ALREADY_THERE] ||
			        eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_BASTION_1_TURN])
			{

				if(!pLoopUnit->isRanged() && !pLoopUnit->noDefensiveBonus())
				{
					bSuitableUnit = true;


					if(pLoopUnit->getDefenseModifier() > 0 || pLoopUnit->getExtraCombatPercent() > 0)
					{
						bHighPriority = true;
					}
				}
			}

			else if(eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ANCIENT_RUINS])
			{

				if(pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_FAST_ATTACK))
				{
					bSuitableUnit = true;
					bHighPriority = true;
				}
				else if(pLoopUnit->IsCanAttack())
				{
					bSuitableUnit = true;
				}
			}

			if(bSuitableUnit)
			{

				int iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pTarget->getX(), pTarget->getY());
				if(pLoopUnit->maxMoves() > 0)
				{
					int iMovesPerTurn = pLoopUnit->maxMoves() / GC.getMOVE_DENOMINATOR();
					int iLeastTurns = (iDistance + iMovesPerTurn - 1) / iMovesPerTurn;
					if(iNumTurnsAway == -1 || iLeastTurns <= iNumTurnsAway)
					{

						int iMoves = TurnsToReachTarget(pLoopUnit, pTarget);
						if(iMoves != MAX_INT && (iNumTurnsAway == -1 ||
						                         (iNumTurnsAway == 0 && pLoopUnit->plot() == pTarget) || iMoves <= iNumTurnsAway))
						{
							CvTacticalUnit unit;
							unit.SetID(pLoopUnit->GetID());
							unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
							unit.SetMovesToTarget(iMoves);

							if(bHighPriority)
							{
								m_CurrentMoveHighPriorityUnits.push_back(unit);
							}
							else
							{
								m_CurrentMoveUnits.push_back(unit);
							}
							rtnValue = true;
						}
					}
				}
			}
		}
	}

	return rtnValue;
}


bool CvTacticalAI::FindUnitsWithinStrikingDistance(CvPlot* pTarget, int iNumTurnsAway, int iPreferredDamageLevel, bool bNoRangedUnits, bool bNavalOnly, bool bMustMoveThrough, bool bIncludeBlockedUnits, bool bWillPillage, bool bTargetUndefended)
{
	list<int>::iterator it;
	UnitHandle pLoopUnit;

	bool rtnValue = false;
	m_CurrentMoveUnits.clear();

	bool bIsCityTarget = pTarget->getPlotCity() != NULL;


#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
#else
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
#endif
	{
		pLoopUnit = m_pPlayer->getUnit(*it);
		if(pLoopUnit)
		{
			if(!bNavalOnly || pLoopUnit->getDomainType() == DOMAIN_SEA)
			{

				if(!pLoopUnit->IsCanAttack())
				{
					continue;
				}

				if(pLoopUnit->isOutOfAttacks())
				{
					continue;
				}

				if (!bIsCityTarget && pLoopUnit->IsCityAttackOnly())
				{
					continue;
				}

				if (bWillPillage && !pLoopUnit->canPillage(pTarget))
				{
					continue;
				}
#if !defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
				int iAttackStrength = pLoopUnit->GetMaxAttackStrength(NULL, NULL, NULL);
#else
				CvCombatInfo kCombatInfo;
				kCombatInfo.setUnit(BATTLE_UNIT_ATTACKER, pLoopUnit.pointer());
				int iAttackStrength = pLoopUnit->GetMaxAttackStrength(kCombatInfo);
#endif

				if (iPreferredDamageLevel > 0)
				{
					int iDamage = pLoopUnit->getDamage();
					if (iDamage > iPreferredDamageLevel)
					{
						iAttackStrength *= 3;
					}
				}

				int iTurnsCalculated = -1;
				if (bTargetUndefended && CanReachInXTurns(pLoopUnit, pTarget, iNumTurnsAway, false                 , &iTurnsCalculated))
				{
					CvTacticalUnit unit;
					unit.SetID(pLoopUnit->GetID());
					unit.SetAttackStrength(iAttackStrength / 2);
					unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
					m_CurrentMoveUnits.push_back(unit);
					rtnValue = true;
				}

				else if(!bNoRangedUnits && !bWillPillage && pLoopUnit->IsCanAttackRanged())
				{

					if (pLoopUnit->getDomainType() != DOMAIN_AIR || (pLoopUnit->getDamage() * 2) < GC.getMAX_HIT_POINTS())
					{

						if(plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pTarget->getX(), pTarget->getY()) <= pLoopUnit->GetRange())
						{

							if(pLoopUnit->canEverRangeStrikeAt(pTarget->getX(), pTarget->getY()))
							{

								if(IsExpectedToDamageWithRangedAttack(pLoopUnit, pTarget))
								{
									CvTacticalUnit unit;
									unit.SetID(pLoopUnit->GetID());


#if !defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
									unit.SetAttackStrength(100 * pLoopUnit->GetMaxRangedCombatStrength(NULL,           NULL, true, true));
#else
									kCombatInfo.setAttackIsRanged(true);
									unit.SetAttackStrength(100 * pLoopUnit->GetMaxRangedCombatStrength(kCombatInfo));
#endif
									unit.SetHealthPercent(100, 100);
									m_CurrentMoveUnits.push_back(unit);
									rtnValue = true;
								}
							}
						}
					}
				}
				else
				{
					if (bMustMoveThrough && (iTurnsCalculated == 0 || (iTurnsCalculated == -1 && CanReachInXTurns(pLoopUnit, pTarget, 0, false                 , &iTurnsCalculated))))
					{
						CvTacticalUnit unit;
						unit.SetID(pLoopUnit->GetID());
						unit.SetAttackStrength(iAttackStrength);
						unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
						m_CurrentMoveUnits.push_back(unit);
						rtnValue = true;
					}

					else if ( (iTurnsCalculated != -1 && iTurnsCalculated <= iNumTurnsAway) || (iTurnsCalculated == -1 && CanReachInXTurns(pLoopUnit, pTarget, iNumTurnsAway, false                 )) )
					{
						CvTacticalUnit unit;
						unit.SetID(pLoopUnit->GetID());
						unit.SetAttackStrength(iAttackStrength);
						unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
						m_CurrentMoveUnits.push_back(unit);
						rtnValue = true;
					}


					else if (bIncludeBlockedUnits && CanReachInXTurns(pLoopUnit, pTarget, iNumTurnsAway, true                 ))
					{
						CvTacticalUnit unit;
						unit.SetID(pLoopUnit->GetID());
						unit.SetAttackStrength(iAttackStrength / 2);
						unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
						m_CurrentMoveUnits.push_back(unit);
						rtnValue = true;
					}
				}
			}
		}
	}


	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return rtnValue;
}


bool CvTacticalAI::FindCitiesWithinStrikingDistance(CvPlot* pTargetPlot)
{
#ifndef AUI_WARNING_FIXES
	list<int>::iterator it;
#endif
	CvCity* pLoopCity;
	int iLoop;

	bool rtnValue = false;
	m_CurrentMoveCities.clear();


	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity->canRangeStrikeAt(pTargetPlot->getX(), pTargetPlot->getY()) && !IsCityInQueuedAttack(pLoopCity))
		{
			CvTacticalCity city;
			city.SetID(pLoopCity->GetID());
			m_CurrentMoveCities.push_back(city);
			rtnValue = true;
		}
	}


	std::stable_sort(m_CurrentMoveCities.begin(), m_CurrentMoveCities.end());

	return rtnValue;
}



bool CvTacticalAI::FindParatroopersWithinStrikingDistance(CvPlot* pTarget)
{
	list<int>::iterator it;
	UnitHandle pLoopUnit;

	bool rtnValue = false;
	m_CurrentMoveUnits.clear();


#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
#else
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
#endif
	{
		pLoopUnit = m_pPlayer->getUnit(*it);
		if(pLoopUnit && pLoopUnit->canParadropAt(pLoopUnit->plot(), pTarget->getX(), pTarget->getY()))
		{
			CvTacticalUnit unit;
			unit.SetID(pLoopUnit->GetID());
#if !defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
			unit.SetAttackStrength(pLoopUnit->GetMaxAttackStrength(NULL, NULL, NULL));
#else
			CvCombatInfo kCombatInfo;
			kCombatInfo.setUnit(BATTLE_UNIT_ATTACKER, pLoopUnit.pointer());
			kCombatInfo.setPlot(pTarget);
			unit.SetAttackStrength(pLoopUnit->GetMaxAttackStrength(kCombatInfo));
#endif
			unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
			m_CurrentMoveUnits.push_back(unit);
			rtnValue = true;
		}
	}


	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return rtnValue;
}



bool CvTacticalAI::FindClosestUnit(CvPlot* pTarget, int iNumTurnsAway, bool bMustHaveHalfHP, bool bMustBeRangedUnit, int iRangeRequired, bool bNeedsIgnoreLOS, bool bMustBeMeleeUnit, bool bIgnoreUnits, CvPlot* pRangedAttackTarget)
{
	list<int>::iterator it;
	UnitHandle pLoopUnit;

	bool rtnValue = false;
	m_CurrentMoveUnits.clear();


#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
#else
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
#endif
	{
		pLoopUnit = m_pPlayer->getUnit(*it);
		if(pLoopUnit)
		{
			bool bValidUnit = true;


			if(pLoopUnit->getUnitInfo().GetCombat() == 0)
			{
				bValidUnit = false;
			}

			else if (bMustHaveHalfHP && (pLoopUnit->getDamage() * 2 > GC.getMAX_HIT_POINTS()))
			{
				bValidUnit = false;
			}

			else if(bMustBeRangedUnit && ((pTarget->isWater() && pLoopUnit->getDomainType() == DOMAIN_LAND) ||
			                              (!pTarget->isWater() && !pTarget->isCity() && pLoopUnit->getDomainType() == DOMAIN_SEA)))
			{
				bValidUnit = false;
			}

			else if(bMustBeRangedUnit && !pLoopUnit->IsCanAttackRanged())
			{
				bValidUnit = false;
			}

			else if(bMustBeRangedUnit && pLoopUnit->GetRange() < iRangeRequired)
			{
				bValidUnit = false;
			}

			else if(bMustBeRangedUnit && !pLoopUnit->canEverRangeStrikeAt(pRangedAttackTarget->getX(), pRangedAttackTarget->getY()))
			{
				bValidUnit = false;
			}

			else if(bMustBeRangedUnit && pLoopUnit->isOutOfAttacks())
			{
				bValidUnit = false;
			}

			else if(pRangedAttackTarget != NULL && bMustBeRangedUnit && !IsExpectedToDamageWithRangedAttack(pLoopUnit, pRangedAttackTarget))
			{
				bValidUnit = false;
			}

			else if(bNeedsIgnoreLOS && !pLoopUnit->IsRangeAttackIgnoreLOS())
			{
				bValidUnit = false;
			}

			else if(bMustBeMeleeUnit && pLoopUnit->IsCanAttackRanged())
			{
				bValidUnit = false;
			}

			int iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pTarget->getX(), pTarget->getY());
			if(iNumTurnsAway == 0 && iDistance > (m_iRecruitRange / 2) || iNumTurnsAway == 1 && iDistance > m_iRecruitRange)
			{
				bValidUnit = false;
			}

			if(bValidUnit)
			{
				int iTurns = TurnsToReachTarget(pLoopUnit, pTarget, false                , bIgnoreUnits, (iNumTurnsAway==0));
				if(iTurns <= iNumTurnsAway)
				{
					CvTacticalUnit unit;
					unit.SetID(pLoopUnit->GetID());
					unit.SetAttackStrength(1000-iTurns);
					unit.SetHealthPercent(10,10);
					unit.SetMovesToTarget(plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pTarget->getX(), pTarget->getY()));
					m_CurrentMoveUnits.push_back(unit);
					rtnValue = true;
				}
			}
		}
	}


	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return rtnValue;
}


bool CvTacticalAI::FindClosestOperationUnit(CvPlot* pTarget, bool bSafeForRanged, bool bMustBeRangedUnit)
{
	FStaticVector<CvOperationUnit, SAFE_ESTIMATE_NUM_MULTIUNITFORMATION_ENTRIES, true, c_eCiv5GameplayDLL, 0>::iterator it;
	UnitHandle pLoopUnit;

	bool rtnValue = false;
	m_CurrentMoveUnits.clear();


#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_OperationUnits.begin(); it != m_OperationUnits.end(); ++it)
#else
	for(it = m_OperationUnits.begin(); it != m_OperationUnits.end(); it++)
#endif
	{
		pLoopUnit = m_pPlayer->getUnit(it->GetUnitID());
		if(pLoopUnit)
		{
			bool bValidUnit = true;

			if (pLoopUnit->hasMoved())
			{
				bValidUnit = false;
			}

			else if(!bSafeForRanged && pLoopUnit->IsCanAttackRanged())
			{
				bValidUnit = false;
			}

			else if (bMustBeRangedUnit && !pLoopUnit->IsCanAttackRanged())
			{
				bValidUnit = false;
			}

			if(bValidUnit)
			{
				int iTurns = TurnsToReachTarget(pLoopUnit, pTarget, false                , false                 );

				if(iTurns != MAX_INT)
				{
					CvTacticalUnit unit;
					unit.SetID(pLoopUnit->GetID());
					unit.SetAttackStrength(1000-iTurns);
					unit.SetHealthPercent(10,10);
					unit.SetMovesToTarget(plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pTarget->getX(), pTarget->getY()));
					m_CurrentMoveUnits.push_back(unit);
					rtnValue = true;
				}
			}
		}
	}


	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return rtnValue;
}


bool CvTacticalAI::FindClosestNavalOperationUnit(CvPlot* pTarget, bool bEscortedUnits)
{
	FStaticVector<CvOperationUnit, SAFE_ESTIMATE_NUM_MULTIUNITFORMATION_ENTRIES, true, c_eCiv5GameplayDLL, 0>::iterator it;
	UnitHandle pLoopUnit;

	bool rtnValue = false;
	m_CurrentMoveUnits.clear();


#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_OperationUnits.begin(); it != m_OperationUnits.end(); ++it)
#else
	for(it = m_OperationUnits.begin(); it != m_OperationUnits.end(); it++)
#endif
	{
		pLoopUnit = m_pPlayer->getUnit(it->GetUnitID());
		if(pLoopUnit)
		{
			bool bValidUnit = true;

			if(bEscortedUnits && (!pLoopUnit->IsGreatAdmiral() && pLoopUnit->getDomainType() != DOMAIN_LAND))
			{
				bValidUnit = false;
			}

			if(!bEscortedUnits && (pLoopUnit->getDomainType() != DOMAIN_SEA || pLoopUnit->IsGreatAdmiral()))
			{
				bValidUnit = false;
			}

			if(bValidUnit)
			{
				int iTurns = TurnsToReachTarget(pLoopUnit, pTarget, false                , true                 );

				if(iTurns != MAX_INT)
				{
					CvTacticalUnit unit;
					unit.SetID(pLoopUnit->GetID());
					unit.SetAttackStrength(1000-iTurns);
					unit.SetHealthPercent(10,10);
					unit.SetMovesToTarget(plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pTarget->getX(), pTarget->getY()));
					m_CurrentMoveUnits.push_back(unit);
					rtnValue = true;
				}
			}
		}
	}


	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return rtnValue;
}


int CvTacticalAI::ComputeTotalExpectedDamage(CvTacticalTarget* pTarget, CvPlot* pTargetPlot)
{
	int rtnValue = 0;
	int iExpectedDamage;
	int iExpectedSelfDamage;


	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pAttacker = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());


		switch(pTarget->GetTargetType())
		{
		case AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT:
		case AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT:
		case AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT:
		{
			UnitHandle pDefender = pTargetPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
			if(pDefender)
			{
				if(pAttacker->IsCanAttackRanged())
				{
					iExpectedDamage = pAttacker->GetRangeCombatDamage(pDefender.pointer(), NULL, false);
					iExpectedSelfDamage = 0;
				}
				else
				{
#if !defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
					int iAttackerStrength = pAttacker->GetMaxAttackStrength(NULL, pTargetPlot, NULL);
					int iDefenderStrength = pDefender->GetMaxDefenseStrength(pTargetPlot, pAttacker.pointer());
#else
					CvCombatInfo kCombatInfo;
					kCombatInfo.setUnit(BATTLE_UNIT_ATTACKER, pAttacker.pointer());
					kCombatInfo.setUnit(BATTLE_UNIT_DEFENDER, pDefender.pointer());
					kCombatInfo.setPlot(pTargetPlot);
					int iAttackerStrength = pAttacker->GetMaxAttackStrength(kCombatInfo);
					int iDefenderStrength = pDefender->GetMaxDefenseStrength(kCombatInfo);
#endif
					UnitHandle pFireSupportUnit = CvUnitCombat::GetFireSupportUnit(pDefender->getOwner(), pTargetPlot->getX(), pTargetPlot->getY(), pAttacker->getX(), pAttacker->getY());
					int iDefenderFireSupportCombatDamage = 0;
					if(pFireSupportUnit)
					{
						iDefenderFireSupportCombatDamage = pFireSupportUnit->GetRangeCombatDamage(pAttacker.pointer(), NULL, false);
					}
					iExpectedDamage = pAttacker->getCombatDamage(iAttackerStrength, iDefenderStrength, pAttacker->getDamage() + iDefenderFireSupportCombatDamage,                  false,                     false,                     false);
					iExpectedSelfDamage = pDefender->getCombatDamage(iDefenderStrength, iAttackerStrength, pDefender->getDamage(),                  false,                     false,                     false);
				}
				m_CurrentMoveUnits[iI].SetExpectedTargetDamage(iExpectedDamage);
				m_CurrentMoveUnits[iI].SetExpectedSelfDamage(iExpectedSelfDamage);
				rtnValue += iExpectedDamage;
			}
		}
		break;

		case AI_TACTICAL_TARGET_CITY:
		{
			CvCity* pCity = pTargetPlot->getPlotCity();
			if(pCity != NULL)
			{
#if !defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
				if(pAttacker->IsCanAttackRanged() && pAttacker->GetMaxRangedCombatStrength(NULL,           NULL, true, true) > pAttacker->GetMaxAttackStrength(NULL, pTargetPlot, NULL))
#else
				CvCombatInfo kCombatInfo;
				kCombatInfo.setUnit(BATTLE_UNIT_ATTACKER, pAttacker.pointer());
				kCombatInfo.setCity(BATTLE_UNIT_DEFENDER, pCity);
				kCombatInfo.setPlot(pTargetPlot);
				kCombatInfo.setAttackIsRanged(true);
				if (pAttacker->IsCanAttackRanged() && pAttacker->GetMaxRangedCombatStrength(kCombatInfo) > pAttacker->GetMaxAttackStrength(kCombatInfo))
#endif
				{
					iExpectedDamage = pAttacker->GetRangeCombatDamage(NULL, pCity, false);
					iExpectedSelfDamage = 0;
				}
				else
				{
#if !defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
					int iAttackerStrength = pAttacker->GetMaxAttackStrength(NULL, pTargetPlot, NULL);
#else
					kCombatInfo.setAttackIsRanged(false);
					int iAttackerStrength = pAttacker->GetMaxAttackStrength(kCombatInfo);
#endif
					int iDefenderStrength = pCity->getStrengthValue();
					CvUnit* pFireSupportUnit = CvUnitCombat::GetFireSupportUnit(pCity->getOwner(), pTargetPlot->getX(), pTargetPlot->getY(), pAttacker->getX(), pAttacker->getY());
					int iDefenderFireSupportCombatDamage = 0;
					if(pFireSupportUnit != NULL)
					{
						iDefenderFireSupportCombatDamage = pFireSupportUnit->GetRangeCombatDamage(pAttacker.pointer(), NULL, false);
					}
					iExpectedDamage = pAttacker->getCombatDamage(iAttackerStrength, iDefenderStrength, pAttacker->getDamage() + iDefenderFireSupportCombatDamage,                  false,                     false,                     true);
					iExpectedSelfDamage = pAttacker->getCombatDamage(iDefenderStrength, iAttackerStrength, pCity->getDamage(),                  false,                     true,                     false);
				}
				m_CurrentMoveUnits[iI].SetExpectedTargetDamage(iExpectedDamage);
				m_CurrentMoveUnits[iI].SetExpectedSelfDamage(iExpectedSelfDamage);
				rtnValue += iExpectedDamage;
			}
		}
		break;
		}
	}

	return rtnValue;
}


int CvTacticalAI::ComputeTotalExpectedBombardDamage(UnitHandle pTarget)
{
	int rtnValue = 0;
	int iExpectedDamage;


	for(unsigned int iI = 0; iI < m_CurrentMoveCities.size(); iI++)
	{
		CvCity* pAttackingCity = m_pPlayer->getCity(m_CurrentMoveCities[iI].GetID());
		iExpectedDamage = pAttackingCity->rangeCombatDamage(pTarget.pointer(), NULL, false);
		m_CurrentMoveCities[iI].SetExpectedTargetDamage(iExpectedDamage);
		rtnValue += iExpectedDamage;
	}

	return rtnValue;
}

#ifdef AUI_CONSTIFY
bool CvTacticalAI::IsExpectedToDamageWithRangedAttack(UnitHandle pAttacker, CvPlot* pTargetPlot) const
#else
bool CvTacticalAI::IsExpectedToDamageWithRangedAttack(UnitHandle pAttacker, CvPlot* pTargetPlot)
#endif
{
	int iExpectedDamage = 0;

	if(pTargetPlot->isCity())
	{
		CvCity* pCity = pTargetPlot->getPlotCity();
		iExpectedDamage = pAttacker->GetRangeCombatDamage(NULL, pCity,                  false);
	}
	else
	{
		UnitHandle pDefender = pTargetPlot->getBestDefender(NO_PLAYER, m_pPlayer->GetID());
		if(pDefender)
		{
			iExpectedDamage = pAttacker->GetRangeCombatDamage(pDefender.pointer(), NULL, false);
		}
	}

	return iExpectedDamage > 0;
}



bool CvTacticalAI::MoveToEmptySpaceNearTarget(UnitHandle pUnit, CvPlot* pTarget, bool bLand)
{
	CvPlot* pLoopPlot;


	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(pTarget->getX(), pTarget->getY(), ((DirectionTypes)iI));
		if(pLoopPlot != NULL && pLoopPlot->isWater() != bLand)
		{

			if(!pLoopPlot->getBestDefender(m_pPlayer->GetID()))
			{

				if(!pLoopPlot->getBestDefender(NO_PLAYER, m_pPlayer->GetID()))
				{

					if(pLoopPlot->getPlotCity() == NULL || pLoopPlot->isFriendlyCity(*pUnit, false))
					{

						if(pUnit->GeneratePath(pLoopPlot))
						{

							bool bMoveWasSafe;
							MoveToUsingSafeEmbark(pUnit, pLoopPlot, bMoveWasSafe);
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}


bool CvTacticalAI::MoveToEmptySpaceTwoFromTarget(UnitHandle pUnit, CvPlot* pTarget, bool bLand)
{
	CvPlot* pLoopPlot;


	for(int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		pLoopPlot = plotCity(pTarget->getX(), pTarget->getY(), iI);
		if(pLoopPlot != NULL && pLoopPlot->isWater() != bLand && plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pTarget->getX(), pTarget->getY() == 2))
		{

			if(!pLoopPlot->getBestDefender(m_pPlayer->GetID()))
			{

				if(!pLoopPlot->getBestDefender(NO_PLAYER, m_pPlayer->GetID()))
				{

					if(pLoopPlot->getPlotCity() == NULL || pLoopPlot->isFriendlyCity(*pUnit, false))
					{

						if(pUnit->GeneratePath(pLoopPlot))
						{

							bool bMoveWasSafe;
							MoveToUsingSafeEmbark(pUnit, pLoopPlot, bMoveWasSafe);
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}


bool CvTacticalAI::MoveToUsingSafeEmbark(UnitHandle pUnit, CvPlot* pTargetPlot, bool &bMoveWasSafe)
{
	bMoveWasSafe = true;


	if (pUnit->getDomainType() != DOMAIN_LAND)
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
		return true;
	}


	if(!pUnit->GeneratePath(pTargetPlot))
	{


		bMoveWasSafe = false;
		return false;
	}

	else
	{
		CvPlot *pMovePlot = pUnit->GetPathEndTurnPlot();
		

		if (!pMovePlot->isWater())
		{
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
			return true;
		}


		else
		{
			bool bDangerous = false;

			int iPlotIndex = GC.getMap().plotNum(pTargetPlot->getX(), pTargetPlot->getY());
			CvTacticalAnalysisCell *pCell = m_pMap->GetCell(iPlotIndex);
			if (pCell->IsSubjectToAttack())
			{

				bDangerous = true;
			}

			else
			{
				if (GC.getGame().GetTacticalAnalysisMap()->IsInEnemyDominatedZone(pTargetPlot))
				{

					bDangerous = true;
				}
			}


			if (!bDangerous)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
				return true;	
			}


			else
			{
				if(!pUnit->GeneratePath(pTargetPlot, CvUnit::MOVEFLAG_STAY_ON_LAND))
				{

					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());


					bMoveWasSafe = false;
					return true;	
				}
				else
				{
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY(), CvUnit::MOVEFLAG_STAY_ON_LAND);
					return true;	
				}
			}
		}
	}
}


CvPlot* CvTacticalAI::FindBestBarbarianLandMove(UnitHandle pUnit)
{
	CvPlot* pBestMovePlot = FindNearbyTarget(pUnit, m_iLandBarbarianRange);
	

	if (pBestMovePlot == NULL)
	{
		pBestMovePlot = FindBarbarianGankTradeRouteTarget(pUnit);
	}


	if(pBestMovePlot == NULL)
	{
		pBestMovePlot = FindBarbarianExploreTarget(pUnit);
	}

	return pBestMovePlot;
}


CvPlot* CvTacticalAI::FindPassiveBarbarianLandMove(UnitHandle pUnit)
{
	CvPlot* pBestMovePlot = NULL;
	int iBestValue;
	int iValue;

	iBestValue = MAX_INT;
	pBestMovePlot = NULL;

	for(unsigned int iI = 0; iI < m_AllTargets.size(); iI++)
	{

		if(m_AllTargets[iI].GetTargetType()==AI_TACTICAL_TARGET_BARBARIAN_CAMP)
		{
			iValue = plotDistance(pUnit->getX(), pUnit->getY(), m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY());
			if(iValue < iBestValue)
			{
				iBestValue = iValue;
				pBestMovePlot = GC.getMap().plot(m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY());
			}
		}
	}

	if(pBestMovePlot == NULL)
	{
		pBestMovePlot = FindBarbarianExploreTarget(pUnit);
	}

	return pBestMovePlot;
}


CvPlot* CvTacticalAI::FindBestBarbarianSeaMove(UnitHandle pUnit)
{
	CvPlot* pBestMovePlot = NULL;
	int iBestValue;
	int iValue;
	CvPlot* pPlot;
	CvTacticalTarget* pTarget;
	int iMovementRate;

	iMovementRate = pUnit->baseMoves();
	iBestValue = MAX_INT;
	pBestMovePlot = NULL;


	pTarget = GetFirstUnitTarget();
	while(pTarget != NULL)
	{

		if(plotDistance(pUnit->getX(), pUnit->getY(), pTarget->GetTargetX(), pTarget->GetTargetY()) < m_iSeaBarbarianRange)
		{
			pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
			if(pUnit->getArea() == pPlot->getArea())
			{
				iValue = TurnsToReachTarget(pUnit, pPlot, true                );
				if(iValue < iBestValue)
				{
					iBestValue = iValue;
					pBestMovePlot = pPlot;
				}
			}
		}
		pTarget = GetNextUnitTarget();
	}


	if (pBestMovePlot == NULL)
	{
		pBestMovePlot = FindBarbarianGankTradeRouteTarget(pUnit);
	}


	if(pBestMovePlot == NULL)
	{
		CvPlot* pNearestCamp = NULL;
		int iBestCampDistance = MAX_INT;


		pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
		while(pTarget != NULL)
		{
			int iDistance = plotDistance(pUnit->getX(), pUnit->getY(), pTarget->GetTargetX(), pTarget->GetTargetY());
			if(iDistance < iBestCampDistance)
			{
				pNearestCamp = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
				iBestCampDistance = iDistance;
			}
			pTarget = GetNextZoneTarget();
		}






		for(int jJ = 0; jJ < NUM_DIRECTION_TYPES; jJ++)
		{
			pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
			while(pTarget != NULL)
			{
				CvPlot* pCamp = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
				if(pCamp != pNearestCamp)
				{
					pPlot = plotDirection(pCamp->getX(), pCamp->getY(), ((DirectionTypes)jJ));
					if(pPlot && pPlot->isWater())
					{
						int iDistance = plotDistance(pUnit->getX(), pUnit->getY(), pPlot->getX(), pPlot->getY());


						if(iDistance < iMovementRate * iBestValue && iDistance < (m_iSeaBarbarianRange * 3))
						{
							iValue = TurnsToReachTarget(pUnit, pPlot, true                );
							if(iValue < iBestValue)
							{
								iBestValue = iValue;
								pBestMovePlot = pPlot;
							}
						}
					}
				}
				pTarget = GetNextZoneTarget();
			}
		}
	}


	if(pBestMovePlot == NULL)
	{

		iBestValue = 0;
		int iMovementRange = pUnit->movesLeft() / GC.getMOVE_DENOMINATOR();
#ifdef AUI_HEXSPACE_DX_LOOPS
		int iMaxDX, iX;
		CvPlot* pConsiderPlot;
		for (int iY = -iMovementRange; iY <= iMovementRange; iY++)
		{
			iMaxDX = iMovementRange - MAX(0, iY);
			for (iX = -iMovementRange - MIN(0, iY); iX <= iMaxDX; iX++)
			{
				pConsiderPlot = plotXY(pUnit->getX(), pUnit->getY(), iX, iY);
#else
		for(int iX = -iMovementRange; iX <= iMovementRange; iX++)
		{
			for(int iY = -iMovementRange; iY <= iMovementRange; iY++)
			{
				CvPlot* pConsiderPlot = plotXYWithRangeCheck(pUnit->getX(), pUnit->getY(), iX, iY, iMovementRange);
#endif
				if(!pConsiderPlot)
				{
					continue;
				}

				if(pUnit->atPlot(*pConsiderPlot))
				{
					continue;
				}

				if(!pConsiderPlot->isRevealed(pUnit->getTeam()))
				{
					continue;
				}

				if(pConsiderPlot->area() != pUnit->area())
				{
					continue;
				}

				if(!CanReachInXTurns(pUnit, pConsiderPlot, 1))
				{
					continue;
				}


				DomainTypes eDomain = pUnit->getDomainType();
				iValue = CvEconomicAI::ScoreExplorePlot(pConsiderPlot, pUnit->getTeam(), pUnit->getUnitInfo().GetBaseSightRange(), eDomain);


				if(pConsiderPlot->isAdjacentOwned())
				{
					iValue += 100;
				}
				else if(pConsiderPlot->isOwned())
				{
					iValue += 200;
				}


				if(iValue == 0)
				{
#ifdef AUI_FIX_HEX_DISTANCE_INSTEAD_OF_PLOT_DISTANCE
					iValue = hexDistance(iX, iY);
#else
					iValue = plotDistance(pUnit->getX(), pUnit->getY(), pConsiderPlot->getX(), pConsiderPlot->getY());
#endif
				}

				if(iValue > iBestValue)
				{
					pBestMovePlot = pConsiderPlot;
					iBestValue = iValue;
				}
			}
		}
	}

	return pBestMovePlot;
}


CvPlot* CvTacticalAI::FindBarbarianExploreTarget(UnitHandle pUnit)
{
	CvPlot* pBestMovePlot = NULL;
	int iBestValue;
	int iValue;


	iBestValue = 0;
	int iMovementRange = pUnit->movesLeft() / GC.getMOVE_DENOMINATOR();
#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX, iX;
	CvPlot* pPlot;
	for (int iY = -iMovementRange; iY <= iMovementRange; iY++)
	{
		iMaxDX = iMovementRange - MAX(0, iY);
		for (iX = -iMovementRange - MIN(0, iY); iX <= iMaxDX; iX++)
		{
			pPlot = plotXY(pUnit->getX(), pUnit->getY(), iX, iY);
#else
	for(int iX = -iMovementRange; iX <= iMovementRange; iX++)
	{
		for(int iY = -iMovementRange; iY <= iMovementRange; iY++)
		{
			CvPlot* pPlot = plotXYWithRangeCheck(pUnit->getX(), pUnit->getY(), iX, iY, iMovementRange);
#endif
			if(!pPlot)
			{
				continue;
			}

			if(pUnit->atPlot(*pPlot))
			{
				continue;
			}

			if(!pPlot->isRevealed(pUnit->getTeam()))
			{
				continue;
			}

			if(pPlot->area() != pUnit->area())
			{
				continue;
			}

			if(!CanReachInXTurns(pUnit, pPlot, 1))
			{
				continue;
			}


			DomainTypes eDomain = pUnit->getDomainType();
			iValue = CvEconomicAI::ScoreExplorePlot(pPlot, pUnit->getTeam(), pUnit->getUnitInfo().GetBaseSightRange(), eDomain);


			if(pPlot->isAdjacentOwned())
			{
				iValue += 100;
			}
			else if(pPlot->isOwned())
			{
				iValue += 200;
			}


			if(iValue == 0)
			{
#ifdef AUI_FIX_HEX_DISTANCE_INSTEAD_OF_PLOT_DISTANCE
				iValue = hexDistance(iX, iY);
#else
				iValue = plotDistance(pUnit->getX(), pUnit->getY(), pPlot->getX(), pPlot->getY());
#endif
			}

			if(iValue > iBestValue)
			{
				pBestMovePlot = pPlot;
				iBestValue = iValue;
			}
		}
	}
	return pBestMovePlot;
}


CvPlot* CvTacticalAI::FindBarbarianGankTradeRouteTarget(UnitHandle pUnit)
{
	CvPlot* pBestMovePlot = NULL;
	int iBestValue;
	int iValue;


	iBestValue = 0;
	int iMovementRange = pUnit->movesLeft() / GC.getMOVE_DENOMINATOR();
#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX, iX;
	CvPlot* pPlot;
	for (int iY = -iMovementRange; iY <= iMovementRange; iY++)
	{
		iMaxDX = iMovementRange - MAX(0, iY);
		for (iX = -iMovementRange - MIN(0, iY); iX <= iMaxDX; iX++)
		{
			pPlot = plotXY(pUnit->getX(), pUnit->getY(), iX, iY);
#else
	for(int iX = -iMovementRange; iX <= iMovementRange; iX++)
	{
		for(int iY = -iMovementRange; iY <= iMovementRange; iY++)
		{
			CvPlot* pPlot = plotXYWithRangeCheck(pUnit->getX(), pUnit->getY(), iX, iY, iMovementRange);
#endif
			if(!pPlot)
			{
				continue;
			}

			if(pUnit->atPlot(*pPlot))
			{
				continue;
			}

			if(!pPlot->isRevealed(pUnit->getTeam()))
			{
				continue;
			}

			if(pPlot->area() != pUnit->area())
			{
				continue;
			}

			if(!CanReachInXTurns(pUnit, pPlot, 1))
			{
				continue;
			}

			iValue = GC.getGame().GetGameTrade()->GetNumTradeRoutesInPlot(pPlot);

			if(iValue > iBestValue)
			{
				pBestMovePlot = pPlot;
				iBestValue = iValue;
			}
		}
	}
	return pBestMovePlot;
}



CvPlot* CvTacticalAI::FindNearbyTarget(UnitHandle pUnit, int iRange, AITacticalTargetType eType, CvUnit* pNoLikeUnit)
{
	CvPlot* pBestMovePlot = NULL;
	int iBestValue;
	int iValue;
	CvPlot* pPlot;

	iBestValue = MAX_INT;
	pBestMovePlot = NULL;


	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{

		bool bTypeMatch = false;
		if(eType == AI_TACTICAL_TARGET_NONE)
		{
			if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
			        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
			        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT ||
			        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_CITY ||
			        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_IMPROVEMENT)
			{
				bTypeMatch = true;
			}
		}
		else if(m_ZoneTargets[iI].GetTargetType() ==  eType)
		{
			bTypeMatch = true;
		}


		if(bTypeMatch)
		{
			pPlot = GC.getMap().plot(m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY());
			int iDistance = plotDistance(pUnit->getX(), pUnit->getY(), m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY());
			if(iDistance == 0)
			{
				return pPlot;
			}
			else if(iDistance < iRange)
			{
				if(pUnit->getArea() == pPlot->getArea())
				{
					if(!pNoLikeUnit || pPlot->getNumFriendlyUnitsOfType(pNoLikeUnit) == 0)
					{
						iValue = TurnsToReachTarget(pUnit, pPlot, true                );
						if(iValue < iBestValue)
						{
							pBestMovePlot = pPlot;
							iBestValue = iValue;
						}
					}
				}
			}
		}
	}

	return pBestMovePlot;
}



bool CvTacticalAI::NearVisibleEnemy(UnitHandle pUnit, int iRange)
{
	UnitHandle pLoopUnit;
	CvCity* pLoopCity = NULL;
	int iLoop;


	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive() && atWar(kPlayer.getTeam(), m_pPlayer->getTeam()))
		{

			for(pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit; pLoopUnit = kPlayer.nextUnit(&iLoop))
			{

				if(pLoopUnit->plot()->isVisible(m_pPlayer->getTeam()))
				{

					if(plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pUnit->getX(), pUnit->getY()) <= iRange)
					{
						return true;
					}
				}
			}


			for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{

				if(pLoopCity->plot()->isVisible(m_pPlayer->getTeam()))
				{

					if(plotDistance(pLoopCity->getX(), pLoopCity->getY(), pUnit->getX(), pUnit->getY()) <= iRange)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}




bool CvTacticalAI::CanCoverFromEnemy(CvPlot* pPlot, int& iNumUnitsRequiredToCover, int iAttackingUnitID)
{
	CvPlot* pLoopPlot;
	int iPlotIndex;

	iNumUnitsRequiredToCover = 0;


	if(pPlot->isWater())
	{
		return true;
	}


	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));


		if(pLoopPlot != NULL && !pLoopPlot->isWater())
		{
			iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
			if(m_pMap->GetCell(iPlotIndex)->IsEnemyCanMovePast() && !m_pMap->GetCell(iPlotIndex)->IsFriendlyTurnEndTile())
			{
				iNumUnitsRequiredToCover++;
			}
		}
	}

	if(iNumUnitsRequiredToCover == 0)
	{
		m_ChosenBlocks.clear();
		return true;
	}
	else
	{
		if(iNumUnitsRequiredToCover > (int)m_CurrentTurnUnits.size())
		{
			return false;
		}


		m_PotentialBlocks.clear();
		for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));
			if(pLoopPlot != NULL && !pLoopPlot->isWater())
			{
				iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
				if(m_pMap->GetCell(iPlotIndex)->IsEnemyCanMovePast() && !m_pMap->GetCell(iPlotIndex)->IsFriendlyTurnEndTile())
				{
					if(!FindClosestUnit(pLoopPlot, 1, false                   , false                      , 2                   , false                    , true                     ))
					{
						return false;
					}
					else
					{

#ifdef AUI_ITERATORIZE
						for (std::vector<CvTacticalUnit>::const_iterator it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
						{
							if (it->GetID() != iAttackingUnitID)
							{
								CvBlockingUnit block;
								block.SetUnitID(it->GetID());
								block.SetPlot(pLoopPlot);
								block.SetNumChoices((int)m_CurrentMoveUnits.size());
								block.SetDistanceToTarget(it->GetMovesToTarget());
#else
						for(unsigned int iJ = 0; iJ < m_CurrentMoveUnits.size(); iJ++)
						{
							if(m_CurrentMoveUnits[iJ].GetID() != iAttackingUnitID)
							{
								CvBlockingUnit block;
								block.SetUnitID(m_CurrentMoveUnits[iJ].GetID());
								block.SetPlot(pLoopPlot);
								block.SetNumChoices((int)m_CurrentMoveUnits.size());
								block.SetDistanceToTarget(m_CurrentMoveUnits[iJ].GetMovesToTarget());
#endif
								m_PotentialBlocks.push_back(block);
							}
						}
					}
				}
			}
		}
	}


	return AssignCoveringUnits(iNumUnitsRequiredToCover);
}


bool CvTacticalAI::AssignCoveringUnits(int iNumUnitsRequiredToCover)
{
	bool bChoseOne = true;

	m_TemporaryBlocks.clear();
	m_ChosenBlocks.clear();

	while(bChoseOne)
	{
		bChoseOne = false;
		m_NewlyChosen.clear();


#ifdef AUI_ITERATORIZE
		for (FStaticVector<CvBlockingUnit, SAFE_ESTIMATE_NUM_BLOCKING_UNITS, true, c_eCiv5GameplayDLL, 0>:: const_iterator it = m_PotentialBlocks.begin(); it != m_PotentialBlocks.end(); ++it)
		{
			CvBlockingUnit temp = *it;
#else
		for(unsigned int iI = 0; iI < m_PotentialBlocks.size(); iI++)
		{
			CvBlockingUnit temp = m_PotentialBlocks[iI];
#endif

			if(temp.GetNumChoices() == 1)
			{
				m_NewlyChosen.push_back(temp);
				bChoseOne = true;
			}
		}

		if(bChoseOne)
		{

			if(HaveDuplicateUnit())
			{
				return false;
			}
			else
			{

#ifdef AUI_ITERATORIZE
				for (FStaticVector<CvBlockingUnit, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::const_iterator it = m_NewlyChosen.begin(); it != m_NewlyChosen.end(); ++it)
				{
					m_ChosenBlocks.push_back(*it);
#else
				for(unsigned int jJ = 0; jJ < m_NewlyChosen.size(); jJ++)
				{
					m_ChosenBlocks.push_back(m_NewlyChosen[jJ]);
#endif
				}

				RemoveChosenUnits();


				if(NumUniqueUnitsLeft() < (iNumUnitsRequiredToCover - (int)m_ChosenBlocks.size()))
				{
					return false;
				}
			}
		}
	}

	return ChooseRemainingAssignments(iNumUnitsRequiredToCover, iNumUnitsRequiredToCover);
}


bool CvTacticalAI::AssignFlankingUnits(int iNumUnitsRequiredToFlank)
{
	m_TemporaryBlocks.clear();
	m_ChosenBlocks.clear();

	return ChooseRemainingAssignments(iNumUnitsRequiredToFlank, 2                       );
}


bool CvTacticalAI::AssignDeployingUnits(int iNumUnitsRequiredToDeploy)
{
	bool bChoseOne = true;
	bool bRtnValue = true;

	m_TemporaryBlocks.clear();
	m_ChosenBlocks.clear();


	while(bChoseOne)
	{
		bChoseOne = false;
		m_NewlyChosen.clear();

#ifdef AUI_ITERATORIZE
		for (FStaticVector<CvBlockingUnit, SAFE_ESTIMATE_NUM_BLOCKING_UNITS, true, c_eCiv5GameplayDLL, 0>::const_iterator it = m_PotentialBlocks.begin(); it != m_PotentialBlocks.end(); ++it)
		{
			if (it->GetNumChoices() == 1)
			{
				m_NewlyChosen.push_back(*it);
#else
		for(unsigned int iI = 0; iI < m_PotentialBlocks.size(); iI++)
		{
			if(m_PotentialBlocks[iI].GetNumChoices() == 1)
			{
				m_NewlyChosen.push_back(m_PotentialBlocks[iI]);
#endif
				bChoseOne = true;
			}
		}

		if(bChoseOne)
		{

			if(HaveDuplicateUnit())
			{
				return false;
			}
			else
			{

#ifdef AUI_ITERATORIZE
				for (FStaticVector<CvBlockingUnit, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::const_iterator it = m_NewlyChosen.begin(); it != m_NewlyChosen.end(); ++it)
				{
					m_ChosenBlocks.push_back(*it);
#else
				for(unsigned int jJ = 0; jJ < m_NewlyChosen.size(); jJ++)
				{
					m_ChosenBlocks.push_back(m_NewlyChosen[jJ]);
#endif
				}

				RemoveChosenUnits();


				if(NumUniqueUnitsLeft() < (iNumUnitsRequiredToDeploy - (int)m_ChosenBlocks.size()))
				{
					return false;
				}
			}
		}
	}


	while(m_PotentialBlocks.size() > 0)
	{
		bChoseOne = false;
		m_NewlyChosen.clear();

#ifdef AUI_ITERATORIZE
		m_NewlyChosen.push_back(*m_PotentialBlocks.begin());
		m_ChosenBlocks.push_back(*m_PotentialBlocks.begin());


		RemoveChosenUnits(m_PotentialBlocks.begin()->GetNumChoices());
#else
		m_NewlyChosen.push_back(m_PotentialBlocks[0]);
		m_ChosenBlocks.push_back(m_PotentialBlocks[0]);


		RemoveChosenUnits(m_PotentialBlocks[0].GetNumChoices());
#endif


		if(NumUniqueUnitsLeft() < (iNumUnitsRequiredToDeploy - (int)m_ChosenBlocks.size()))
		{

			bRtnValue = false;
		}
	}

	return bRtnValue;
}


void CvTacticalAI::PerformChosenMoves(int iFallbackMoveRange)
{

	std::stable_sort(m_ChosenBlocks.begin(), m_ChosenBlocks.end(), TacticalAIHelpers::CvBlockingUnitDistanceSort);


#ifdef AUI_ITERATORIZE
	for (FStaticVector<CvBlockingUnit, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::iterator it = m_ChosenBlocks.begin(); it != m_ChosenBlocks.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetUnitID());
		if (pUnit)
		{
			if (pUnit->plot() != it->GetPlot() && IsInChosenMoves(pUnit->plot()) && it->GetPlot()->getNumFriendlyUnitsOfType(pUnit) == 0)
			{
				bool bMoveWasSafe;
				MoveToUsingSafeEmbark(pUnit, it->GetPlot(), bMoveWasSafe);
#else
	for(unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());
		if(pUnit)
		{
			if(pUnit->plot() != m_ChosenBlocks[iI].GetPlot() && IsInChosenMoves(pUnit->plot()) && m_ChosenBlocks[iI].GetPlot()->getNumFriendlyUnitsOfType(pUnit) == 0)
			{
				bool bMoveWasSafe;
				MoveToUsingSafeEmbark(pUnit, m_ChosenBlocks[iI].GetPlot(), bMoveWasSafe);
#endif

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("Deploying %s (to get out of way), To X: %d, To Y: %d, At X: %d, At Y: %d, Distance Before Move: %d",
#ifdef AUI_ITERATORIZE
						pUnit->getName().GetCString(), it->GetPlot()->getX(), it->GetPlot()->getY(),
						pUnit->getX(), pUnit->getY(), it->GetDistanceToTarget());
#else
					              pUnit->getName().GetCString(), m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY(),
					              pUnit->getX(), pUnit->getY(), m_ChosenBlocks[iI].GetDistanceToTarget());
#endif
					LogTacticalMessage(strMsg);
				}


#ifdef AUI_ITERATORIZE
				it->SetNumChoices(-1);
#else
				m_ChosenBlocks[iI].SetNumChoices(-1);
#endif
			}
		}
	}


#ifdef AUI_ITERATORIZE
	for (FStaticVector<CvBlockingUnit, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::iterator it = m_ChosenBlocks.begin(); it != m_ChosenBlocks.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetUnitID());
		if (pUnit)
		{
			if (pUnit->plot() == it->GetPlot())
			{
				it->SetNumChoices(-1);
			}
			else
			{

				if (it->GetNumChoices() != -1)
#else
	for(unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());
		if(pUnit)
		{
			if(pUnit->plot() == m_ChosenBlocks[iI].GetPlot())
			{
				m_ChosenBlocks[iI].SetNumChoices(-1);
			}
			else
			{

				if(m_ChosenBlocks[iI].GetNumChoices() != -1)
#endif
				{
					CvPlot* pPlotBeforeMove = pUnit->plot();
					bool bMoveWasSafe;
#ifdef AUI_ITERATORIZE
					MoveToUsingSafeEmbark(pUnit, it->GetPlot(), bMoveWasSafe);
#else
					MoveToUsingSafeEmbark(pUnit, m_ChosenBlocks[iI].GetPlot(), bMoveWasSafe);
#endif

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Deploying %s, To X: %d, To Y: %d, At X: %d, At Y: %d, Distance Before Move: %d",
#ifdef AUI_ITERATORIZE
							pUnit->getName().GetCString(), it->GetPlot()->getX(), it->GetPlot()->getY(),
							pUnit->getX(), pUnit->getY(), it->GetDistanceToTarget());
#else
						              pUnit->getName().GetCString(), m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY(),
						              pUnit->getX(), pUnit->getY(), m_ChosenBlocks[iI].GetDistanceToTarget());
#endif
						LogTacticalMessage(strMsg);
					}


					if(pPlotBeforeMove != pUnit->plot())
					{
#ifdef AUI_ITERATORIZE
						it->SetNumChoices(-1);
#else
						m_ChosenBlocks[iI].SetNumChoices(-1);
#endif
					}
				}
			}
		}
	}


	if (iFallbackMoveRange > 0)
	{
#ifdef AUI_ITERATORIZE
		for (FStaticVector<CvBlockingUnit, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::iterator it = m_ChosenBlocks.begin(); it != m_ChosenBlocks.end(); ++it)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(it->GetUnitID());
			if (pUnit)
			{

				if (it->GetNumChoices() != -1)
				{
					CvPlot* pPlotBeforeMove = pUnit->plot();
					if (MoveToEmptySpaceNearTarget(UnitHandle(pUnit), it->GetPlot(), (pUnit->getDomainType() == DOMAIN_LAND)))
#else
		for(unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());
			if(pUnit)
			{

				if(m_ChosenBlocks[iI].GetNumChoices() != -1)
				{
					CvPlot* pPlotBeforeMove = pUnit->plot();
					if(MoveToEmptySpaceNearTarget(UnitHandle(pUnit), m_ChosenBlocks[iI].GetPlot(), (pUnit->getDomainType()==DOMAIN_LAND)))
#endif
					{
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Deploying %s to space near target, Target X: %d, Target Y: %d, At X: %d, At Y: %d, Distance Before Move: %d",
#ifdef AUI_ITERATORIZE
								pUnit->getName().GetCString(), it->GetPlot()->getX(), it->GetPlot()->getY(),
								pUnit->getX(), pUnit->getY(), it->GetDistanceToTarget());
#else
								pUnit->getName().GetCString(), m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY(),
								pUnit->getX(), pUnit->getY(), m_ChosenBlocks[iI].GetDistanceToTarget());
#endif
							LogTacticalMessage(strMsg);
						}

						if(pPlotBeforeMove != pUnit->plot())
						{
#ifdef AUI_ITERATORIZE
							it->SetNumChoices(-1);
#else
							m_ChosenBlocks[iI].SetNumChoices(-1);
#endif
						}
					}
				}
			}
		}
	}


	if (iFallbackMoveRange > 1)
	{
#ifdef AUI_ITERATORIZE
		for (FStaticVector<CvBlockingUnit, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::iterator it = m_ChosenBlocks.begin(); it != m_ChosenBlocks.end(); ++it)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(it->GetUnitID());
			if (pUnit)
			{

				if (it->GetNumChoices() != -1)
				{
					if (MoveToEmptySpaceNearTarget(UnitHandle(pUnit), it->GetPlot(), (pUnit->getDomainType() == DOMAIN_LAND)))
#else
		for(unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());
			if(pUnit)
			{

				if(m_ChosenBlocks[iI].GetNumChoices() != -1)
				{
					if (MoveToEmptySpaceTwoFromTarget(UnitHandle(pUnit), m_ChosenBlocks[iI].GetPlot(), (pUnit->getDomainType()==DOMAIN_LAND)))
#endif
					{
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Deploying %s to space within 2 of target, Target X: %d, Target Y: %d, At X: %d, At Y: %d, Distance Before Move: %d",
#ifdef AUI_ITERATORIZE
								pUnit->getName().GetCString(), it->GetPlot()->getX(), it->GetPlot()->getY(),
								pUnit->getX(), pUnit->getY(), it->GetDistanceToTarget());
#else
								pUnit->getName().GetCString(), m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY(),
								pUnit->getX(), pUnit->getY(), m_ChosenBlocks[iI].GetDistanceToTarget());
#endif
							LogTacticalMessage(strMsg);
						}
					}
				}
			}
		}
	}


#ifdef AUI_ITERATORIZE
	for (FStaticVector<CvBlockingUnit, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::iterator it = m_ChosenBlocks.begin(); it != m_ChosenBlocks.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetUnitID());
#else
	for(unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());
#endif
		if(!pUnit->isDelayedDeath())
		{
			if (pUnit->getMoves() > 0)
			{
				if (pUnit->canPillage(pUnit->plot()) && pUnit->getDamage() > 0)
				{
					pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Already in position, will pillage with %s, X: %d, Y: %d", pUnit->getName().GetCString(),
#ifdef AUI_ITERATORIZE
							it->GetPlot()->getX(), it->GetPlot()->getY());
#else
							m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
#endif
						LogTacticalMessage(strMsg);
					}

				}
				else if(pUnit->canFortify(pUnit->plot()))
				{
					pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Already in position, will fortify with %s, X: %d, Y: %d", pUnit->getName().GetCString(),
#ifdef AUI_ITERATORIZE
							it->GetPlot()->getX(), it->GetPlot()->getY());
#else
							m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
#endif
						LogTacticalMessage(strMsg);
					}
				}
				else
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Already in position, no move for %s, X: %d, Y: %d", pUnit->getName().GetCString(),
#ifdef AUI_ITERATORIZE
							it->GetPlot()->getX(), it->GetPlot()->getY());
#else
							m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
#endif
						LogTacticalMessage(strMsg);
					}
				}
				pUnit->finishMoves();
			}
			UnitProcessed(pUnit->GetID());
		}
	}
}


void CvTacticalAI::MoveGreatGeneral(CvArmyAI* pArmyAI)
{
	UnitHandle pGeneral;
	int iRange;

#ifdef AUI_ITERATORIZE
	for (FStaticVector<CvOperationUnit, SAFE_ESTIMATE_NUM_MULTIUNITFORMATION_ENTRIES, true, c_eCiv5GameplayDLL, 0>::iterator it = m_GeneralsToMove.begin(); it != m_GeneralsToMove.end(); ++it)
	{
		CvPlot* pBestPlot = NULL;
		int iBestScore = -1;
		pGeneral = m_pPlayer->getUnit(it->GetUnitID());
#else
	for (unsigned int iI = 0; iI < m_GeneralsToMove.size(); iI++)
	{
		CvPlot* pBestPlot = NULL;
		int iBestScore = -1;
		pGeneral = m_pPlayer->getUnit(m_GeneralsToMove[iI].GetUnitID());
#endif

		if(pGeneral)
		{
			iRange = (pGeneral->maxMoves() * 3) / GC.getMOVE_DENOMINATOR();
#ifdef AUI_HEXSPACE_DX_LOOPS
			int iMaxDX, iX;
			CvPlot* pEvalPlot;
			for (int iY = -iRange; iY <= iRange; iY++)
			{
				iMaxDX = iRange - MAX(0, iY);
				for (iX = -iRange - MIN(0, iY); iX <= iMaxDX; iX++)
				{
					pEvalPlot = plotXY(pGeneral->getX(), pGeneral->getY(), iX, iY);
#else
			for(int iX = -iRange; iX <= iRange; iX++)
			{
				for(int iY = -iRange; iY <= iRange; iY++)
				{
					CvPlot* pEvalPlot = NULL;
					pEvalPlot = plotXYWithRangeCheck(pGeneral->getX(), pGeneral->getY(), iX, iY, iRange);
#endif
					if(!pEvalPlot)
					{
						continue;
					}

					if(CanReachInXTurns(pGeneral, pEvalPlot, 1))
					{
						int iScore = ScoreGreatGeneralPlot(pGeneral, pEvalPlot, pArmyAI);

						if(iScore > iBestScore && iScore > 0)
						{
							iBestScore = iScore;
							pBestPlot = pEvalPlot;
						}
					}

				}
			}

			if(pBestPlot != NULL)
			{
				ExecuteMoveToPlot(pGeneral, pBestPlot);

				if(GC.getLogging() && GC.getAILogging())
				{
					int iDistToOperationCenter = -1;

					if(pArmyAI)
					{
						CvPlot* pCOM = pArmyAI->GetCenterOfMass(NO_DOMAIN);
						if(pCOM)
						{
							iDistToOperationCenter = plotDistance(pBestPlot->getX(), pBestPlot->getY(), pCOM->getX(), pCOM->getY());
						}
					}

					CvString strMsg;
					strMsg.Format("Deploying %s, To X: %d, To Y: %d, At X: %d, At Y: %d, Plot Score: %d, Dist from COM: %d",
					              pGeneral->getName().GetCString(), pBestPlot->getX(), pBestPlot->getY(),
					              pGeneral->getX(), pGeneral->getY(), iBestScore, iDistToOperationCenter);
					LogTacticalMessage(strMsg);
				}
			}
		}
	}

	return;
}


bool CvTacticalAI::IsInChosenMoves(CvPlot* pPlot)
{
#ifdef AUI_ITERATORIZE
	for (FStaticVector<CvBlockingUnit, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::const_iterator it = m_ChosenBlocks.begin(); it != m_ChosenBlocks.end(); ++it)
	{
		if (it->GetPlot() == pPlot)
#else
	for(unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
	{
		if(m_ChosenBlocks[iI].GetPlot() == pPlot)
#endif
		{
			return true;
		}
	}

	return false;
}


bool CvTacticalAI::HaveDuplicateUnit()
{
#ifdef AUI_ITERATORIZE
	for (FStaticVector<CvBlockingUnit, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::const_iterator it = m_NewlyChosen.begin(); it + 1 != m_NewlyChosen.end(); ++it)
	{
		for (FStaticVector<CvBlockingUnit, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::const_iterator it2 = it + 1; it2 != m_NewlyChosen.end(); ++it2)
		{
			if (it->GetUnitID() == it2->GetUnitID())
#else
	for(unsigned int iI = 0; iI < m_NewlyChosen.size()- 1; iI++)
	{
		for(unsigned int jJ = iI + 1; jJ < m_NewlyChosen.size(); jJ++)
		{
			if(m_NewlyChosen[iI].GetUnitID() == m_NewlyChosen[jJ].GetUnitID())
#endif
			{
				return true;
			}
		}
	}
	return false;
}


void CvTacticalAI::RemoveChosenUnits(int iStartIndex)
{
	m_TemporaryBlocks.clear();
	m_TemporaryBlocks = m_PotentialBlocks;
	m_PotentialBlocks.clear();

#ifdef AUI_ITERATORIZE
	for (FStaticVector<CvBlockingUnit, SAFE_ESTIMATE_NUM_BLOCKING_UNITS, true, c_eCiv5GameplayDLL, 0>::iterator it = m_TemporaryBlocks.begin() + iStartIndex; it != m_TemporaryBlocks.end(); ++it)
	{
		bool bCopyIt = true;

		CvBlockingUnit block = *it;


		for (FStaticVector<CvBlockingUnit, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::const_iterator it2 = m_NewlyChosen.begin(); it2 != m_NewlyChosen.end(); ++it2)
		{
			if (block.GetUnitID() == it2->GetUnitID())
#else
	for(unsigned int iI = iStartIndex; iI < m_TemporaryBlocks.size(); iI++)
	{
		bool bCopyIt = true;

		CvBlockingUnit block = m_TemporaryBlocks[iI];


		for(unsigned int jJ = 0; jJ < m_NewlyChosen.size() && bCopyIt; jJ++)
		{
			if(block.GetUnitID() == m_NewlyChosen[jJ].GetUnitID())
#endif
			{
				bCopyIt = false;
			}
		}

		if(bCopyIt)
		{
			m_PotentialBlocks.push_back(block);
		}
	}


#ifdef AUI_ITERATORIZE
	for (FStaticVector<CvBlockingUnit, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::iterator it = m_PotentialBlocks.begin(); it != m_PotentialBlocks.end(); ++it)
	{
		int iNumFound = 0;
		CvPlot* pPlot = it->GetPlot();

		for (FStaticVector<CvBlockingUnit, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::iterator it2 = m_PotentialBlocks.begin(); it2 != m_PotentialBlocks.end(); ++it2)
		{
			if (pPlot == it2->GetPlot())
			{
				iNumFound++;
			}
		}
		it->SetNumChoices(iNumFound);
#else
	for(unsigned int iI = 0; iI < m_PotentialBlocks.size(); iI++)
	{
		int iNumFound = 0;
		CvPlot* pPlot = m_PotentialBlocks[iI].GetPlot();

		for(unsigned int jJ = 0; jJ < m_PotentialBlocks.size(); jJ++)
		{
			if(pPlot == m_PotentialBlocks[jJ].GetPlot())
			{
				iNumFound++;
			}
		}
		m_PotentialBlocks[iI].SetNumChoices(iNumFound);
#endif
	}
}


int CvTacticalAI::NumUniqueUnitsLeft()
{
	int iRtnValue = 1;
	int iCurrentID;

	if(m_PotentialBlocks.size() < 2)
	{
		return m_PotentialBlocks.size();
	}


	m_TemporaryBlocks.clear();
	m_TemporaryBlocks = m_PotentialBlocks;
	std::stable_sort(m_TemporaryBlocks.begin(), m_TemporaryBlocks.end());

#ifdef AUI_ITERATORIZE
	iCurrentID = m_TemporaryBlocks.begin()->GetUnitID();
	for (FStaticVector<CvBlockingUnit, SAFE_ESTIMATE_NUM_BLOCKING_UNITS, true, c_eCiv5GameplayDLL, 0>::iterator it = m_TemporaryBlocks.begin() + 1; it != m_TemporaryBlocks.end(); ++it)
	{
		if (it->GetUnitID() != iCurrentID)
		{
			iRtnValue++;
			iCurrentID = it->GetUnitID();
#else
	iCurrentID = m_TemporaryBlocks[0].GetUnitID();
	for(unsigned int iI = 1; iI < m_TemporaryBlocks.size(); iI++)
	{
		if(m_TemporaryBlocks[iI].GetUnitID() != iCurrentID)
		{
			iRtnValue++;
			iCurrentID = m_TemporaryBlocks[iI].GetUnitID();
#endif
		}
	}

	return iRtnValue;
}


bool CvTacticalAI::ChooseRemainingAssignments(int iNumUnitsDesired, int iNumUnitsAcceptable)
{
	int iBestScore = 0;
	int iScore;
	int iCurrent[NUM_DIRECTION_TYPES];
	int iFirst[NUM_DIRECTION_TYPES];
	int iLast[NUM_DIRECTION_TYPES];

	int iBlocksToCreate = iNumUnitsDesired - (int)m_ChosenBlocks.size();

	if(iBlocksToCreate == 0)
	{
		return true;
	}

	FAssertMsg(iBlocksToCreate <= NUM_DIRECTION_TYPES, "More than NUM_DIRECTION_TYPES hexes to block. Will cause array overflows and performance issues!");

	if(iBlocksToCreate > 0 && iBlocksToCreate < NUM_DIRECTION_TYPES)
	{
		m_NewlyChosen.clear();


		int iLevel = 0;
		int iCurIndex = 0;
		while(iLevel < iBlocksToCreate)
		{
			iFirst[iLevel] = iCurIndex;
			int iNumChoices = m_PotentialBlocks[iCurIndex].GetNumChoices();

			FAssertMsg(iNumChoices > 0, "Invalid number of tactical AI move choices. Will cause array overflows and performance issues!");
			FAssertMsg(iNumChoices + iCurIndex <= (int)m_PotentialBlocks.size(), "Invalid number of tactical AI move choices. Will cause array overflows and performance issues!");

			iLast[iLevel] = iCurIndex + iNumChoices - 1;
			iCurIndex = iLast[iLevel] + 1;
			iLevel++;
		}

		for(int iI = 0; iI < iBlocksToCreate; iI++)
		{
			iCurrent[iI] = iFirst[iI];
		}


		bool bDone = false;
		while(!bDone)
		{
			m_TemporaryBlocks.clear();


			for(int iI = 0; iI < iBlocksToCreate; iI++)
			{
				if(iI >= (int)m_PotentialBlocks.size())
				{
					FAssertMsg(false, "Invalid fast vector index - show Ed");
				}
				m_TemporaryBlocks.push_back(m_PotentialBlocks[iCurrent[iI]]);
			}

			iScore = ScoreAssignments(iNumUnitsDesired != iNumUnitsAcceptable                      );


			if(iScore > iBestScore)
			{
				m_NewlyChosen.clear();
#ifdef AUI_ITERATORIZE
				for (FStaticVector<CvBlockingUnit, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::iterator it = m_TemporaryBlocks.begin(); it != m_TemporaryBlocks.end(); ++it)
				{
					if (it->GetDistanceToTarget() != MAX_INT)
					{
						m_NewlyChosen.push_back(*it);
#else
				for(unsigned int iI = 0; iI < m_TemporaryBlocks.size(); iI++)
				{
					if(m_TemporaryBlocks[iI].GetDistanceToTarget() != MAX_INT)
					{
						m_NewlyChosen.push_back(m_TemporaryBlocks[iI]);
#endif
					}
				}
				iBestScore = iScore;
			}


			bool bIncrementDone = false;
			for(int iLevelIndex = iBlocksToCreate - 1; !bIncrementDone && iLevelIndex >= 0 && iLevelIndex < NUM_DIRECTION_TYPES; iLevelIndex--)
			{

				if(iCurrent[iLevelIndex] + 1 > iLast[iLevelIndex])
				{

					iCurrent[iLevelIndex] = iFirst[iLevelIndex];
				}

				else
				{
					iCurrent[iLevelIndex]++;
					bIncrementDone = true;
				}
			}

			if(!bIncrementDone)
			{
				bDone = true;
			}
		}


#ifdef AUI_ITERATORIZE
		for (FStaticVector<CvBlockingUnit, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0>::iterator it = m_NewlyChosen.begin(); it != m_NewlyChosen.end(); ++it)
		{
			m_ChosenBlocks.push_back(*it);
#else
		for(unsigned int iI = 0; iI < m_NewlyChosen.size(); iI++)
		{
			m_ChosenBlocks.push_back(m_NewlyChosen[iI]);
#endif
		}

		return (int)m_ChosenBlocks.size() >= iNumUnitsAcceptable;
	}

	return false;
}


int CvTacticalAI::ScoreAssignments(bool bCanLeaveOpenings)
{
	int iScore = 0;


#ifdef AUI_ITERATORIZE
	for (FStaticVector<CvBlockingUnit, SAFE_ESTIMATE_NUM_BLOCKING_UNITS, true, c_eCiv5GameplayDLL, 0>::iterator it = m_TemporaryBlocks.begin(); it + 1 != m_TemporaryBlocks.end(); ++it)
	{
		for (FStaticVector<CvBlockingUnit, SAFE_ESTIMATE_NUM_BLOCKING_UNITS, true, c_eCiv5GameplayDLL, 0>::iterator it2 = it + 1; it2 != m_TemporaryBlocks.end(); ++it2)
		{
			if (it->GetUnitID() == it2->GetUnitID())
#else
	for(unsigned int iI = 0; iI < m_TemporaryBlocks.size(); iI++)
	{
		for(unsigned int jJ = iI + 1; jJ < m_TemporaryBlocks.size(); jJ++)
		{
			if(m_TemporaryBlocks[iI].GetUnitID() == m_TemporaryBlocks[jJ].GetUnitID())
#endif
			{
				if(!bCanLeaveOpenings)
				{
					return -1;
				}
				else
				{

#ifdef AUI_ITERATORIZE
					if (it->GetDistanceToTarget() < it2->GetDistanceToTarget())
					{
						it2->SetDistanceToTarget(MAX_INT);
					}
					else
					{
						it->SetDistanceToTarget(MAX_INT);
#else
					if(m_TemporaryBlocks[iI].GetDistanceToTarget() < m_TemporaryBlocks[jJ].GetDistanceToTarget())
					{
						m_TemporaryBlocks[jJ].SetDistanceToTarget(MAX_INT);
					}
					else
					{
						m_TemporaryBlocks[iI].SetDistanceToTarget(MAX_INT);
#endif
					}
				}
			}
		}
	}


#ifdef AUI_ITERATORIZE
	for (FStaticVector<CvBlockingUnit, SAFE_ESTIMATE_NUM_BLOCKING_UNITS, true, c_eCiv5GameplayDLL, 0>::const_iterator it = m_TemporaryBlocks.begin(); it != m_TemporaryBlocks.end(); ++it)
	{
		if (it->GetDistanceToTarget() != MAX_INT)
		{
			iScore += (10000 - (it->GetDistanceToTarget() * 1000));
			iScore += m_pPlayer->getUnit(it->GetUnitID())->GetPower();
#else
	for(unsigned int iI = 0; iI < m_TemporaryBlocks.size(); iI++)
	{
		if(m_TemporaryBlocks[iI].GetDistanceToTarget() != MAX_INT)
		{
			iScore += (10000 - (m_TemporaryBlocks[iI].GetDistanceToTarget() * 1000));
			iScore += m_pPlayer->getUnit(m_TemporaryBlocks[iI].GetUnitID())->GetPower();
#endif
		}
	}

	return iScore;
}


int CvTacticalAI::ScoreCloseOnPlots(CvPlot* pTarget, bool bLandOnly)
{
	int iScore;
	CvPlot* pPlot;
	CvTacticalAnalysisCell* pCell;
	bool bChoiceBombardSpot;
	bool bSafeFromAttack;
	CvTacticalTarget target;
	int iRtnValue = 0;


	m_TempTargets.clear();

	for(int jJ = 0; jJ < NUM_CITY_PLOTS; jJ++)
	{
		pPlot = plotCity(pTarget->getX(), pTarget->getY(), jJ);

		if(pPlot != NULL)
		{
			bChoiceBombardSpot = false;
			bSafeFromAttack = true;

			int iPlotDistance = plotDistance(pPlot->getX(), pPlot->getY(), pTarget->getX(), pTarget->getY());
			int iPlotIndex = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
			pCell = m_pMap->GetCell(iPlotIndex);

			if((bLandOnly && pCell->CanUseForOperationGatheringCheckWater(false           )) || (!bLandOnly && pCell->CanUseForOperationGathering()))
			{
				bool bCloseEnough = false;
				for(unsigned int iI = 0; iI < m_OperationUnits.size() && !bCloseEnough; iI++)
				{
					UnitHandle pUnit = m_pPlayer->getUnit(m_OperationUnits[iI].GetUnitID());
					if(pUnit)
					{
						if(plotDistance(pPlot->getX(), pPlot->getY(), pUnit->getX(), pUnit->getY()) <= m_iDeployRadius)
						{
							bCloseEnough = true;
						}
					}
				}

				if(bCloseEnough)
				{
					iScore = 600 - (iPlotDistance * 100);


					if(pCell->IsWithinRangeOfTarget() && iPlotDistance > 1)
					{
						bChoiceBombardSpot = true;
						iRtnValue++;
					}

					if(pCell->IsSubjectToAttack())
					{
						iScore -= 30;
						bSafeFromAttack = false;
					}

					if(pCell->IsEnemyCanMovePast())
					{
						iScore -= 30;
					}
					if(pPlot->isCity() && pPlot->getOwner() == m_pPlayer->GetID())
					{
						iScore += 100;
					}
					else
					{
						iScore += pCell->GetDefenseModifier();
					}

					pCell->SetSafeForDeployment(bChoiceBombardSpot || bSafeFromAttack);
					pCell->SetDeploymentScore(iScore);


					target.SetTargetX(pPlot->getX());
					target.SetTargetY(pPlot->getY());
					target.SetAuxIntData(iScore);


					if(bChoiceBombardSpot)
					{
						target.SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
					}
					else
					{
						target.SetTargetType(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT);
					}

					m_TempTargets.push_back(target);
				}
			}
		}
	}
	return iRtnValue;
}


void CvTacticalAI::ScoreHedgehogPlots(CvPlot* pTarget)
{
	int iScore;
	CvPlot* pPlot;
	CvTacticalAnalysisCell* pCell;
	bool bChoiceBombardSpot;
	bool bSafeFromAttack;
	CvTacticalTarget target;


	m_TempTargets.clear();

	for(int jJ = 0; jJ < NUM_CITY_PLOTS; jJ++)
	{
		pPlot = plotCity(pTarget->getX(), pTarget->getY(), jJ);

		if(pPlot != NULL)
		{
			bChoiceBombardSpot = false;
			bSafeFromAttack = true;

			int iPlotDistance = plotDistance(pPlot->getX(), pPlot->getY(), pTarget->getX(), pTarget->getY());
			int iPlotIndex = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
			pCell = m_pMap->GetCell(iPlotIndex);

			if(pCell->CanUseForOperationGatheringCheckWater(false           ))
			{
				iScore = 600 - (iPlotDistance * 150);

				if(pCell->IsSubjectToAttack())
				{
					iScore += 100;
					bSafeFromAttack = false;
				}
				if(pCell->IsEnemyCanMovePast())
				{
					iScore += 50;
				}
				if(pPlot->isCity() && pPlot->getOwner() == m_pPlayer->GetID())
				{
					iScore += 100;
				}
				else
				{
					iScore += pCell->GetDefenseModifier() * 4;
				}

				pCell->SetSafeForDeployment(bSafeFromAttack);
				pCell->SetDeploymentScore(iScore);


				target.SetTargetX(pPlot->getX());
				target.SetTargetY(pPlot->getY());
				target.SetAuxIntData(iScore);


				if(bSafeFromAttack)
				{
					target.SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
				}
				else
				{
					target.SetTargetType(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT);
				}

				m_TempTargets.push_back(target);
			}
		}
	}
}


int CvTacticalAI::ScoreGreatGeneralPlot(UnitHandle pGeneral, CvPlot* pTarget, CvArmyAI* pArmyAI)
{

	int iScore = 0;

	UnitHandle pBestDefender;
	PlayerTypes ePlayer = m_pPlayer->GetID();


	bool bFriendlyCity = false;
	int iNearbyQueuedAttacks = 0;
	int iDangerValue = 0;
	int iDangerDivisor = 1;
	int iDistToOperationCenter = MAX_INT;
	int iFriendlyUnitBasePower = 0;
	int iFriendlyUnitFinalPower = 0;
	int iFriendlyCityStrength = 0;




	if(pTarget->isCity() && pTarget->getOwner() != ePlayer)
	{
		return 0;
	}


	CvUnit* pUnit = pTarget->getUnitByIndex(0);
	if(pUnit && pUnit->getOwner() != ePlayer)
	{
		return 0;
	}


	else if (pUnit && pUnit->GetID() != pGeneral->GetID() && !pUnit->IsCanAttack() && pUnit->hasMoved())
	{
		return 0;
	}


	iDangerValue = m_pPlayer->GetPlotDanger(*pTarget);
	pBestDefender = pTarget->getBestDefender(m_pPlayer->GetID());


	if(pTarget->isCity())
	{
		bFriendlyCity = true;
		CvCity* pCity = pTarget->getPlotCity();

		iFriendlyCityStrength = pCity->getStrengthValue() * 100;
		iFriendlyCityStrength *= (pCity->GetMaxHitPoints() - pCity->getDamage());
		iFriendlyCityStrength /= pCity->GetMaxHitPoints();

		if(iDangerValue > (iFriendlyCityStrength * 2))
		{
			iDangerDivisor = 5;
		}
		else if(iDangerValue > iFriendlyCityStrength)
		{
			iDangerDivisor = 3;
		}
		else if(iDangerValue > (iFriendlyCityStrength / 2))
		{
			iDangerDivisor = 2;
		}
	}


	else if(pBestDefender && pBestDefender->GetID() != pGeneral->GetID())
	{
		iFriendlyUnitBasePower = pBestDefender->GetBaseCombatStrengthConsideringDamage() * 250;

		if(iDangerValue > (iFriendlyUnitBasePower * 2))
		{
			iDangerDivisor = 5;
		}
		else if(iDangerValue > iFriendlyUnitBasePower)
		{
			iDangerDivisor = 3;
		}
		else if(iDangerValue > (iFriendlyUnitBasePower / 2))
		{
			iDangerDivisor = 2;
		}

		iFriendlyUnitFinalPower = iFriendlyUnitBasePower;


		if(IsInQueuedAttack(pBestDefender.pointer()) && !pBestDefender->isRanged())
		{

			iFriendlyUnitFinalPower /= 3;
		}

		else if(pBestDefender->getMoves() > 0)
		{

			iFriendlyUnitFinalPower /= 25;
		}


		if(pArmyAI && (pBestDefender->getArmyID() != -1) &&(pBestDefender->getArmyID() == pArmyAI->GetID()))
		{
			iFriendlyUnitFinalPower *= 5;
		}
	}


	else
	{	
		iDangerDivisor = 1000;
	}


	if(pArmyAI)
	{
		CvPlot* pCOM = pArmyAI->GetCenterOfMass(NO_DOMAIN);
		if(pCOM)
		{
			iDistToOperationCenter = plotDistance(pTarget->getX(), pTarget->getY(), pCOM->getX(), pCOM->getY());
		}
	}


	iNearbyQueuedAttacks = NearXQueuedAttacks(pTarget, 2);



	if(bFriendlyCity)
	{
		iScore = iFriendlyCityStrength;


		if(pArmyAI)
		{
			iScore = 1000;
		}
	}

	else if(iFriendlyUnitFinalPower > 0)
	{
		iScore = iFriendlyUnitFinalPower;
	}

	else
	{
		iScore = 10;
	}

	if(iNearbyQueuedAttacks > 0)
	{
		iScore *= (5 * iNearbyQueuedAttacks);
	}

	if(iDistToOperationCenter < 20 && (iFriendlyUnitFinalPower > 0 || bFriendlyCity || iDangerValue == 0))
	{

		if(iDistToOperationCenter <= 3)
		{
			iScore += 5000000;
		}


		else
		{
			iScore += (250000 * (20 - iDistToOperationCenter));
		}
	}

	if(iDangerValue > 0)
	{
		iScore /= iDangerDivisor;
	}

	return iScore;
}


void CvTacticalAI::UnitProcessed(int iID, bool bMarkTacticalMap)
{
	UnitHandle pUnit;
	int iPlotIndex;
	CvTacticalAnalysisCell* pCell;

	m_CurrentTurnUnits.remove(iID);
	pUnit = m_pPlayer->getUnit(iID);

	CvAssert(pUnit);
	pUnit->SetTurnProcessed(true);

	if(bMarkTacticalMap)
	{
		CvTacticalAnalysisMap* pMap = GC.getGame().GetTacticalAnalysisMap();
		if(pMap->IsBuilt())
		{
			iPlotIndex = GC.getMap().plotNum(pUnit->getX(), pUnit->getY());
			pCell = pMap->GetCell(iPlotIndex);
			pCell->SetFriendlyTurnEndTile(true);
		}
	}
}


bool CvTacticalAI::UseThisDominanceZone(CvTacticalDominanceZone* pZone)
{
	bool bIsOurCapital = false;
	bool bTempZone = false;
	int iCityID = -1;
	if(pZone->GetClosestCity() != NULL)
	{
		iCityID = pZone->GetClosestCity()->GetID();
		bIsOurCapital = (pZone->GetClosestCity() == m_pPlayer->getCapitalCity());
		bTempZone = IsTemporaryZoneCity(pZone->GetClosestCity());
	}
	bool bWeHaveUnitsNearEnemy = pZone->GetFriendlyUnitCount() > 0 && pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY;
	bool bTheyHaveUnitsNearUs = pZone->GetDominanceFlag() != TACTICAL_DOMINANCE_NO_UNITS_VISIBLE && pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY;
	bool bBothHaveUnits = pZone->GetFriendlyUnitCount() > 0 && pZone->GetEnemyUnitCount() > 0;

	return (bTempZone || bIsOurCapital || bWeHaveUnitsNearEnemy || bTheyHaveUnitsNearUs || bBothHaveUnits);
}



bool CvTacticalAI::IsVeryHighPriorityCivilianTarget(CvTacticalTarget* pTarget)
{
	bool bRtnValue = false;
	CvUnit* pUnit = (CvUnit*)pTarget->GetAuxData();
	if(pUnit)
	{
		if(pUnit->AI_getUnitAIType() == UNITAI_GENERAL || pUnit->AI_getUnitAIType() == UNITAI_ADMIRAL)
		{
			bRtnValue = true;
		}
	}
	return bRtnValue;
}


bool CvTacticalAI::IsHighPriorityCivilianTarget(CvTacticalTarget* pTarget)
{
	bool bRtnValue = false;
	CvUnit* pUnit = (CvUnit*)pTarget->GetAuxData();
	if(pUnit)
	{
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pUnit->getUnitType());
		int iEstimatedEndTurn = GC.getGame().getEstimateEndTurn();
		if(pkUnitInfo)
		{

			if(pkUnitInfo->GetCivilianAttackPriority() == CIVILIAN_ATTACK_PRIORITY_HIGH)
			{
				bRtnValue = true;
			}
			else if(pkUnitInfo->GetCivilianAttackPriority() == CIVILIAN_ATTACK_PRIORITY_HIGH_EARLY_GAME_ONLY)
			{
				if(GC.getGame().getGameTurn() < (iEstimatedEndTurn / 3))
				{
					bRtnValue =  true;
				}
			}
			if(!bRtnValue && pUnit->AI_getUnitAIType() == UNITAI_SETTLE)
			{

				if(GET_PLAYER(pUnit->getOwner()).getNumCities() < 5)
				{
					bRtnValue = true;
				}
				else if(GC.getGame().getGameTurn() < (iEstimatedEndTurn / 3))
				{
					bRtnValue =  true;
				}
				else if(GET_PLAYER(pUnit->getOwner()).IsCramped())
				{
					bRtnValue = true;
				}
			}
			if(!bRtnValue && m_pPlayer->isBarbarian())
			{
				bRtnValue = true;
			}
		}
	}
	return bRtnValue;
}


bool CvTacticalAI::IsMediumPriorityCivilianTarget(CvTacticalTarget* pTarget)
{
	bool bRtnValue = false;
	CvUnit* pUnit = (CvUnit*)pTarget->GetAuxData();
	if(pUnit)
	{
		int iEstimatedEndTurn = GC.getGame().getEstimateEndTurn();
		if(pUnit->isEmbarked() && !pUnit->IsCombatUnit())
		{
			bRtnValue = true;
		}
		else if(pUnit->AI_getUnitAIType() == UNITAI_SETTLE)
		{
			if(GC.getGame().getGameTurn() >= (iEstimatedEndTurn / 3))
			{
				bRtnValue =  true;
			}
		}
		else if(pUnit->AI_getUnitAIType() == UNITAI_WORKER)
		{
			if(GC.getGame().getGameTurn() < (iEstimatedEndTurn / 3))
			{
				bRtnValue =  true;
			}
		}
	}
	return bRtnValue;
}


void CvTacticalAI::LogTacticalMessage(CvString& strMsg, bool bSkipLogDominanceZone)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strPlayerName;
		CvString strTemp;
		FILogFile* pLog;

		strPlayerName = m_pPlayer->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";
		strTemp = "no zone, ";
		if(!bSkipLogDominanceZone)
		{
			CvTacticalDominanceZone* pZone = GC.getGame().GetTacticalAnalysisMap()->GetZone(m_iCurrentZoneIndex);
			if(pZone != NULL)
			{
				strTemp.Format("Zone ID: %d, ", pZone->GetDominanceZoneID());
			}
		}
		strBaseString += strTemp;
		strOutBuf = strBaseString + strMsg;

		pLog->Msg(strOutBuf);
	}
}


CvString CvTacticalAI::GetLogFileName(CvString& playerName) const
{
	CvString strLogName;


	if(GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "PlayerTacticalAILog_" + playerName + ".csv";
	}
	else
	{
		strLogName = "PlayerTacticalAILog.csv";
	}

	return strLogName;
}


CvString CvTacticalAI::GetTacticalMissionName(AITacticalMission eMission) const
{
	CvString strMissionName;


	switch(eMission)
	{
	case AI_TACTICAL_MISSION_NONE:
		strMissionName = "No Tactic";
		break;
	case AI_TACTICAL_MISSION_ATTACK_STATIONARY_TARGET:
		strMissionName = "Attack Stationary Target";
		break;
	case AI_TACTICAL_MISSION_PILLAGE_ENEMY_IMPROVEMENTS:
		strMissionName = "Pillage Enemy Improvements";
	}

	return strMissionName;
}

FDataStream& operator<<(FDataStream& saveTo, const AITacticalMission& readFrom)
{
	int v = static_cast<int>(readFrom);
	saveTo << v;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, AITacticalMission& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<AITacticalMission>(v);
	return loadFrom;
}




bool TacticalAIHelpers::CvBlockingUnitDistanceSort(CvBlockingUnit obj1, CvBlockingUnit obj2)
{
	return obj1.GetDistanceToTarget() < obj2.GetDistanceToTarget();
}
