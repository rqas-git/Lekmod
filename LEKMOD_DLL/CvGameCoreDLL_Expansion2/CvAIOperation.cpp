







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
#include "CvAIOperation.h"
#include "CvEconomicAI.h"
#include "CvDiplomacyAIEnums.h"
#include "CvMilitaryAI.h"
#include "CvEnumSerialization.h"
#include "FStlContainerSerialization.h"
#include "CvDiplomacyAI.h"
#include "CvTypes.h"

#define LINT_WARNINGS_ONLY
#include "LintFree.h"



CvAIOperation::CvAIOperation():m_eOwner(NO_PLAYER)
{
	Reset();
}


CvAIOperation::~CvAIOperation()
{
	Uninit();
}


void CvAIOperation::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea, CvCity*            , CvCity* pMuster)
{
	Uninit();
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;
	m_iDefaultArea = iDefaultArea;
#ifndef AUI_WARNING_FIXES
	m_bShouldReplaceLossesWithReinforcements = false;
#endif

	SetStartCityPlot(pMuster->plot());


	BuildListOfUnitsWeStillNeedToBuild();
	GrabUnitsFromTheReserves(NULL, NULL);
}


void CvAIOperation::Uninit()
{
	if(m_eOwner != NO_PLAYER)
	{
		CvPlayer& thisPlayer = GET_PLAYER(m_eOwner);


		for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* thisArmy = thisPlayer.getArmyAI(m_viArmyIDs[uiI]);
			if(thisArmy)
			{
				thisArmy->Kill();
				thisPlayer.deleteArmyAI(m_viArmyIDs[uiI]);
				DeleteArmyAI(m_viArmyIDs[uiI]);
			}
		}
	}


	m_viArmyIDs.clear();
	m_viListOfUnitsWeStillNeedToBuild.clear();
	m_viListOfUnitsCitiesHaveCommittedToBuild.clear();

	Reset();
}


void CvAIOperation::Reset()
{
	m_iID = 0;
	m_eOwner = NO_PLAYER;
	m_eEnemy = NO_PLAYER;
	m_iDefaultArea = FFreeList::INVALID_INDEX;
	m_eCurrentState = AI_OPERATION_STATE_ABORTED;
	m_eAbortReason = NO_ABORT_REASON;
	m_iTargetX = 0;
	m_iTargetY = 0;
	m_iMusterX = 0;
	m_iMusterY = 0;
	m_iStartCityX = -1;
	m_iStartCityY = -1;
	m_eMoveType = INVALID_AI_OPERATION_MOVE_TYPE;
#ifdef AUI_WARNING_FIXES
	m_bShouldReplaceLossesWithReinforcements = false;
#endif
	m_iLastTurnMoved = -1;
	m_viArmyIDs.clear();
}


int CvAIOperation::GetMaximumRecruitTurns() const
{
	return GC.getAI_OPERATIONAL_MAX_RECRUIT_TURNS_DEFAULT();
}


int CvAIOperation::GetID() const
{
	return m_iID;
}


void CvAIOperation::SetID(int iID)
{
	m_iID = iID;
}


void CvAIOperation::SetOwner(PlayerTypes ePlayer)
{
	m_eOwner = ePlayer;
}


PlayerTypes CvAIOperation::GetEnemy() const
{
	return m_eEnemy;
}


void CvAIOperation::SetEnemy(PlayerTypes eEnemy)
{
	m_eEnemy = eEnemy;
}


int CvAIOperation::GetDefaultArea() const
{
	return m_iDefaultArea;
}


void CvAIOperation::SetDefaultArea(int iDefaultArea)
{
	m_iDefaultArea = iDefaultArea;
}


int CvAIOperation::GetFirstArmyID()
{
	if(!m_viArmyIDs.empty())
	{
		return m_viArmyIDs[0];
	}
	else
	{
		return -1;
	}
}


CvPlot* CvAIOperation::GetTargetPlot() const
{
	CvPlot* rtnValue = NULL;
	rtnValue =  GC.getMap().plot(m_iTargetX, m_iTargetY);
	return rtnValue;
}


void CvAIOperation::SetTargetPlot(CvPlot* pTarget)
{
	m_iTargetX = pTarget->getX();
	m_iTargetY = pTarget->getY();
}


CvPlot* CvAIOperation::GetMusterPlot() const
{
	CvPlot* rtnValue = NULL;
	rtnValue =  GC.getMap().plot(m_iMusterX, m_iMusterY);
	return rtnValue;
}


void CvAIOperation::SetMusterPlot(CvPlot* pMuster)
{
	m_iMusterX = pMuster->getX();
	m_iMusterY = pMuster->getY();
}


CvPlot* CvAIOperation::GetStartCityPlot() const
{
	CvPlot* rtnValue = NULL;
	rtnValue =  GC.getMap().plot(m_iStartCityX, m_iStartCityY);
	return rtnValue;
}


void CvAIOperation::SetStartCityPlot(CvPlot* pStartCity)
{
	m_iStartCityX = pStartCity->getX();
	m_iStartCityY = pStartCity->getY();
}

int CvAIOperation::GetGatherTolerance(CvArmyAI* pArmy, CvPlot* pPlot) const
{
	CvTacticalAnalysisCell* pCell;
	CvTacticalAnalysisMap* pMap = GC.getGame().GetTacticalAnalysisMap();
	int iRtnValue = 1;
	int iValidPlotsNearby = 0;


	int iNumUnits = pArmy->GetNumSlotsFilled();


	if(iNumUnits < 1)
	{
		iRtnValue = 0;
	}
	else
	{
		int iRange = OperationalAIHelpers::GetGatherRangeForXUnits(iNumUnits);
		for(int iX = -iRange; iX <= iRange; iX++)
		{
			for(int iY = -iRange; iY <= iRange; iY++)
			{
				int iPlotIndex = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
				pCell = pMap->GetCell(iPlotIndex);

				if((IsMixedLandNavalOperation() || IsAllNavalOperation()) && !pCell->CanUseForOperationGatheringCheckWater(true))
				{
					continue;
				}

				if((IsMixedLandNavalOperation() || IsAllNavalOperation()) && !pArmy->IsAllOceanGoing() && pCell->IsOcean())
				{
					continue;
				}

				iValidPlotsNearby++;
			}
		}


		if(iValidPlotsNearby > iNumUnits)
		{

			iRtnValue = iRange;
		}


		else
		{
			iRtnValue = 3;
		}
	}

	return iRtnValue;
}


CvAIOperation* CvAIOperation::CreateOperation(AIOperationTypes eAIOperationType, PlayerTypes ePlayer)
{
	switch(eAIOperationType)
	{
	case AI_OPERATION_BASIC_CITY_ATTACK:
		return FNEW(CvAIOperationBasicCityAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_DESTROY_BARBARIAN_CAMP:
		return FNEW(CvAIOperationDestroyBarbarianCamp(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_FOUND_CITY:
		return FNEW(CvAIOperationFoundCity(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_MERCHANT_DELEGATION:
		return FNEW(CvAIOperationMerchantDelegation(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_CONCERT_TOUR:
		return FNEW(CvAIOperationConcertTour(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NAVAL_BOMBARDMENT:
		return FNEW(CvAIOperationNavalBombardment(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NAVAL_SUPERIORITY:
		return FNEW(CvAIOperationNavalSuperiority(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_PURE_NAVAL_CITY_ATTACK:
		return FNEW(CvAIOperationPureNavalCityAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_COLONIZE:
		return FNEW(CvAINavalEscortedOperation(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_QUICK_COLONIZE:
		return FNEW(CvAIOperationQuickColonize(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_PILLAGE_ENEMY:
		return FNEW(CvAIOperationPillageEnemy(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_CITY_CLOSE_DEFENSE:
		return FNEW(CvAIOperationCityCloseDefense(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_RAPID_RESPONSE:
		return FNEW(CvAIOperationRapidResponse(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_SNEAK_CITY_ATTACK:
	{
		if(GC.getGame().getGameTurn() < 50 && GET_PLAYER(ePlayer).GetDiplomacyAI()->GetBoldness() >= 5)
		{
			return FNEW(CvAIOperationQuickSneakCityAttack(), c_eCiv5GameplayDLL, 0);
		}
		return FNEW(CvAIOperationSneakCityAttack(), c_eCiv5GameplayDLL, 0);
	}
	case AI_OPERATION_SMALL_CITY_ATTACK:
		return FNEW(CvAIOperationSmallCityAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_CITY_STATE_ATTACK:
		return FNEW(CvAIOperationCityStateAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NAVAL_ATTACK:
		return FNEW(CvAIOperationNavalAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NAVAL_SNEAK_ATTACK:
		return FNEW(CvAIOperationNavalSneakAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_CITY_STATE_NAVAL_ATTACK:
		return FNEW(CvAIOperationNavalCityStateAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NUKE_ATTACK:
		return FNEW(CvAIOperationNukeAttack(), c_eCiv5GameplayDLL, 0);
	}

	return 0;
}


void CvAIOperation::BuildListOfUnitsWeStillNeedToBuild()
{
	m_viListOfUnitsCitiesHaveCommittedToBuild.clear();
	m_viListOfUnitsWeStillNeedToBuild.clear();
	CvPlayer& thisPlayer = GET_PLAYER(m_eOwner);
	for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
	{
		CvArmyAI* pThisArmy = thisPlayer.getArmyAI(m_viArmyIDs[uiI]);

		if(pThisArmy)
		{
			if(pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE)
			{
				int iThisFormationIndex = pThisArmy->GetFormationIndex();
				if(iThisFormationIndex != NO_MUFORMATION)
				{
					CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(iThisFormationIndex);
					if(thisFormation)
					{
						for(int iThisSlotIndex = 0; iThisSlotIndex < thisFormation->getNumFormationSlotEntries(); iThisSlotIndex++)
						{
							OperationSlot thisOperationSlot;
							thisOperationSlot.m_iOperationID = m_iID;
							thisOperationSlot.m_iArmyID = m_viArmyIDs[uiI];
							thisOperationSlot.m_iSlotID = iThisSlotIndex;
							m_viListOfUnitsWeStillNeedToBuild.push_back(thisOperationSlot);
						}
					}
				}
				else
				{

					CvAssertMsg(false,"A sub-class should have dealt with this");
				}
			}
		}
	}
}


OperationSlot CvAIOperation::PeekAtNextUnitToBuild(int iAreaID)
{
	OperationSlot thisSlot;
	if(iAreaID == m_iDefaultArea && !m_viListOfUnitsWeStillNeedToBuild.empty())
	{
		thisSlot = m_viListOfUnitsWeStillNeedToBuild.back();
	}
	return thisSlot;
}


OperationSlot CvAIOperation::CommitToBuildNextUnit(int iAreaID, int iTurns, CvCity* pCity)
{
	OperationSlot thisSlot;
	if(iAreaID == m_iDefaultArea && !m_viListOfUnitsWeStillNeedToBuild.empty())
	{
		thisSlot = m_viListOfUnitsWeStillNeedToBuild.back();
		m_viListOfUnitsWeStillNeedToBuild.pop_back();
		m_viListOfUnitsCitiesHaveCommittedToBuild.push_back(thisSlot);

		CvArmyAI* pArmy = GET_PLAYER(m_eOwner).getArmyAI(thisSlot.m_iArmyID);
		if(pArmy)
		{
			int iTurnsFromCityToMusterGuess;
			iTurnsFromCityToMusterGuess = plotDistance(pCity->getX(), pCity->getY(), pArmy->GetX(), pArmy->GetY()) / 2;
			pArmy->SetEstimatedTurn(thisSlot.m_iSlotID, iTurns + iTurnsFromCityToMusterGuess);
		}
	}
	return thisSlot;
}


#ifdef AUI_WARNING_FIXES
bool CvAIOperation::UncommitToBuild(const OperationSlot& thisOperationSlot)
#else
bool CvAIOperation::UncommitToBuild(OperationSlot thisOperationSlot)
#endif
{

	std::vector<OperationSlot>::iterator iter = find(m_viListOfUnitsCitiesHaveCommittedToBuild.begin(),m_viListOfUnitsCitiesHaveCommittedToBuild.end(),thisOperationSlot);

	if(iter != m_viListOfUnitsCitiesHaveCommittedToBuild.end())
	{

		m_viListOfUnitsWeStillNeedToBuild.push_back(*iter);

		m_viListOfUnitsCitiesHaveCommittedToBuild.erase(iter);
		return true;
	}
	return false;
}


#ifdef AUI_WARNING_FIXES
bool CvAIOperation::FinishedBuilding(const OperationSlot& thisOperationSlot)
#else
bool CvAIOperation::FinishedBuilding(OperationSlot thisOperationSlot)
#endif
{

	std::vector<OperationSlot>::iterator iter = find(m_viListOfUnitsCitiesHaveCommittedToBuild.begin(),m_viListOfUnitsCitiesHaveCommittedToBuild.end(),thisOperationSlot);

	if(iter != m_viListOfUnitsCitiesHaveCommittedToBuild.end())
	{

		m_viListOfUnitsCitiesHaveCommittedToBuild.erase(iter);


		if(m_viListOfUnitsWeStillNeedToBuild.empty() && m_eCurrentState  == AI_OPERATION_STATE_RECRUITING_UNITS)
		{
			m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;

			for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
				if(pThisArmy)
				{
					pThisArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
				}
			}
		}
		return true;
	}
	return false;
}


bool CvAIOperation::GrabUnitsFromTheReserves(CvPlot* pMusterPlot, CvPlot* pTargetPlot)
{
	bool rtnValue = true;
	bool success;
	std::vector<OperationSlot>::iterator it;
	bool bRequired;


	std::vector<OperationSlot> secondList;
	for(it = m_viListOfUnitsWeStillNeedToBuild.begin(); it != m_viListOfUnitsWeStillNeedToBuild.end(); ++it)
	{
		secondList.push_back(*it);
	}


	m_viListOfUnitsWeStillNeedToBuild.clear();

	for(it = secondList.begin(); it != secondList.end(); ++it)
	{
		success = FindBestFitReserveUnit(*it, pMusterPlot, pTargetPlot, &bRequired);


		if(!success)
		{
			if(bRequired)
			{

				rtnValue = false;


				m_viListOfUnitsWeStillNeedToBuild.push_back(*it);
			}
			else
			{
				CvArmyAI* pArmy;
				pArmy = GET_PLAYER(m_eOwner).getArmyAI(it->m_iArmyID);
				if(pArmy)
				{
					pArmy->SetEstimatedTurn(it->m_iSlotID, ARMYSLOT_NOT_INCLUDING_IN_OPERATION);
				}
			}
		}
	}

	return rtnValue;
}


bool CvAIOperation::CheckOnTarget()
{
	int iUnitID;
	CvUnit* pCivilian;
	CvPlot* pCivilianPlot = NULL;
	CvPlot* pEscortPlot;

	if(GetFirstArmyID() == -1)
	{
		return false;
	}

	switch(m_eMoveType)
	{
		{
		case AI_OPERATION_MOVETYPE_SINGLE_HEX:
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[0]);
			if(pThisArmy->GetNumSlotsFilled() >= 1)
			{
				iUnitID = pThisArmy->GetFirstUnitID();
				if(iUnitID != -1)
				{
					pCivilian = GET_PLAYER(m_eOwner).getUnit(iUnitID);
					pCivilianPlot = pCivilian->plot();
				}
				if(m_eCurrentState == AI_OPERATION_STATE_MOVING_TO_TARGET && pCivilianPlot == GetTargetPlot())
				{
					ArmyInPosition(pThisArmy);
					return true;
				}
				else if(m_eCurrentState == AI_OPERATION_STATE_GATHERING_FORCES)
				{
					if(pThisArmy->GetNumSlotsFilled() == 1)
					{
						ArmyInPosition(pThisArmy);
						return true;
					}
					else
					{
						pEscortPlot = GET_PLAYER(m_eOwner).getUnit(pThisArmy->GetNextUnitID())->plot();
						if(pCivilianPlot == pEscortPlot)
						{
							ArmyInPosition(pThisArmy);
							return true;
						}
					}
				}
			}
			else
			{
				CvAssertMsg(m_eCurrentState == AI_OPERATION_STATE_RECRUITING_UNITS || m_eCurrentState == AI_OPERATION_STATE_ABORTED || m_eCurrentState == AI_OPERATION_STATE_SUCCESSFUL_FINISH,
				            "Found an escort operation with no units in it.  Show Ed and send save.");
			}
			break;
		}

	case AI_OPERATION_MOVETYPE_ENEMY_TERRITORY:
	{

		for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
			CvPlot* pCenterOfMass;

			if(pThisArmy->GetNumSlotsFilled() >= 1)
			{
				switch(m_eCurrentState)
				{
				case AI_OPERATION_STATE_GATHERING_FORCES:
					{
						int iGatherTolerance = GetGatherTolerance(pThisArmy, GetMusterPlot());
						pCenterOfMass = pThisArmy->GetCenterOfMass(IsAllNavalOperation() || IsMixedLandNavalOperation() ? DOMAIN_SEA : DOMAIN_LAND);
						if(pCenterOfMass &&
							plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), GetMusterPlot()->getX(), GetMusterPlot()->getY()) <= iGatherTolerance &&
							pThisArmy->GetFurthestUnitDistance(GetMusterPlot()) <= (iGatherTolerance * 3 / 2))
						{
							ArmyInPosition(pThisArmy);
							return true;
						}
					}
					break;
				case AI_OPERATION_STATE_MOVING_TO_TARGET:
					{
						int iTargetTolerance = GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE();
						pCenterOfMass = pThisArmy->GetCenterOfMass(IsAllNavalOperation() || IsMixedLandNavalOperation() ? DOMAIN_SEA : DOMAIN_LAND);
						if(pCenterOfMass &&
							plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), m_iTargetX, m_iTargetY) <= iTargetTolerance &&
							pThisArmy->GetFurthestUnitDistance(GetTargetPlot()) <= (iTargetTolerance * 3 / 2))
						{
							ArmyInPosition(pThisArmy);
							return true;
						}
					}
					break;
				}
			}
			else
			{
				CvAssertMsg(m_eCurrentState == AI_OPERATION_STATE_RECRUITING_UNITS || m_eCurrentState == AI_OPERATION_STATE_ABORTED || m_eCurrentState == AI_OPERATION_STATE_SUCCESSFUL_FINISH,
				            "Found an army operation with no units in it.  Show Ed and send save.");
			}
		}
		break;
	}

	case AI_OPERATION_MOVETYPE_NAVAL_ESCORT:
	{

		for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
			CvPlot* pCenterOfMass;

			if(pThisArmy->GetNumSlotsFilled() >= 1)
			{
				switch(m_eCurrentState)
				{
				case AI_OPERATION_STATE_GATHERING_FORCES:
					{
						int iGatherTolerance = GetGatherTolerance(pThisArmy, GetMusterPlot());
						pCenterOfMass = pThisArmy->GetCenterOfMass(DOMAIN_SEA);
						if(pCenterOfMass &&
							plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), GetMusterPlot()->getX(), GetMusterPlot()->getY()) <= iGatherTolerance &&
							pThisArmy->GetFurthestUnitDistance(GetMusterPlot()) <= (iGatherTolerance * 3))
						{
							ArmyInPosition(pThisArmy);
							return true;
						}
					}
					break;
				case AI_OPERATION_STATE_MOVING_TO_TARGET:
					{
						int iTargetTolerance = GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE();
						pCenterOfMass = pThisArmy->GetCenterOfMass(DOMAIN_SEA);
						if(pCenterOfMass &&
							plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), m_iTargetX, m_iTargetY) <= (iTargetTolerance  * 2) &&
							pThisArmy->GetFurthestUnitDistance(GetTargetPlot()) <= (iTargetTolerance * 3))
						{
							ArmyInPosition(pThisArmy);
							return true;
						}
					}
					break;
				}
			}
			else
			{
				CvAssertMsg(m_eCurrentState == AI_OPERATION_STATE_RECRUITING_UNITS || m_eCurrentState == AI_OPERATION_STATE_ABORTED || m_eCurrentState == AI_OPERATION_STATE_SUCCESSFUL_FINISH,
				            "Found an army operation with no units in it.  Show Ed and send save.");
			}
		}
		break;
	}

	case AI_OPERATION_MOVETYPE_FREEFORM_NAVAL:
	{

		for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
			CvPlot* pCenterOfMass;
			int iGatherTolerance = GetGatherTolerance(pThisArmy, GetMusterPlot());

			if(pThisArmy->GetNumSlotsFilled() >= 1)
			{
				switch(m_eCurrentState)
				{
				case AI_OPERATION_STATE_GATHERING_FORCES:
				case AI_OPERATION_STATE_MOVING_TO_TARGET:


					ArmyInPosition(pThisArmy);

					pCenterOfMass = pThisArmy->GetCenterOfMass(DOMAIN_SEA);
					if(pCenterOfMass &&
					        plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), GetTargetPlot()->getX(), GetTargetPlot()->getY()) <= iGatherTolerance &&
					        pThisArmy->GetFurthestUnitDistance(GetMusterPlot()) <= (iGatherTolerance * 3 / 2))
					{
						return true;
					}
					break;
				}
			}
			else
			{
				CvAssertMsg(m_eCurrentState == AI_OPERATION_STATE_RECRUITING_UNITS || m_eCurrentState == AI_OPERATION_STATE_ABORTED || m_eCurrentState == AI_OPERATION_STATE_SUCCESSFUL_FINISH,
				            "Found an army operation with no units in it.  Show Ed and send save.");
			}
		}
		break;
	}
	}
	return false;
}


bool CvAIOperation::ArmyInPosition(CvArmyAI* pArmy)
{
	bool bStateChanged = false;

	switch(m_eCurrentState)
	{

	case AI_OPERATION_STATE_GATHERING_FORCES:
	{
		m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
		bStateChanged = true;
		pArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
	}
	break;


	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{
		if(pArmy->Plot() == GetTargetPlot())
		{
			m_eCurrentState = AI_OPERATION_STATE_AT_TARGET;
			bStateChanged = true;
		}
	}
	break;

	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
	case AI_OPERATION_STATE_SUCCESSFUL_FINISH:
		break;
	};

	return bStateChanged;
}


int CvAIOperation::PercentFromMusterPointToTarget()
{
	int iRtnValue = 0;

	switch(m_eCurrentState)
	{
	case AI_OPERATION_STATE_GATHERING_FORCES:
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
		return 0;

	case AI_OPERATION_STATE_AT_TARGET:
	case AI_OPERATION_STATE_SUCCESSFUL_FINISH:
		return 100;

	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{

		for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* pArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);

			if (pArmy->GetGoalPlot())
			{
				int iDistanceMusterToTarget;
				int iDistanceCurrentToTarget;

				CvPlot *pCenterOfMass = pArmy->GetCenterOfMass(IsAllNavalOperation() || IsMixedLandNavalOperation() ? DOMAIN_SEA : DOMAIN_LAND);


				iDistanceMusterToTarget = GC.getStepFinder().GetStepDistanceBetweenPoints(m_eOwner, m_eEnemy, GetMusterPlot(), pArmy->GetGoalPlot());
				iDistanceCurrentToTarget = GC.getStepFinder().GetStepDistanceBetweenPoints(m_eOwner, m_eEnemy, pCenterOfMass, pArmy->GetGoalPlot());

				if(iDistanceMusterToTarget <= 0)
				{
					return 0;
				}
	

				else if (iDistanceCurrentToTarget <= 2)
				{
					return 100;
				}

				else
				{
					int iTempValue = 100 - (100 * iDistanceCurrentToTarget / iDistanceMusterToTarget);
					if(iTempValue > iRtnValue)
					{
						iRtnValue = iTempValue;
					}
				}
			}
			else 
			{
				CvAssertMsg (false, "Operational AI army without a goal plot!  Send save to Ed");
			}
		}
	}
	break;
	}

	return iRtnValue;
}



bool CvAIOperation::ShouldAbort()
{
	UnitHandle pUnit;


	if(m_eCurrentState == AI_OPERATION_STATE_SUCCESSFUL_FINISH)
	{
		for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* pArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);

			pUnit = pArmy->GetFirstUnit();
			while(pUnit)
			{
				pUnit->SetDeployFromOperationTurn(GC.getGame().getGameTurn());
				pUnit = pArmy->GetNextUnit();
			}
		}
	}

	return (m_eCurrentState == AI_OPERATION_STATE_ABORTED || m_eCurrentState == AI_OPERATION_STATE_SUCCESSFUL_FINISH);
}


void CvAIOperation::SetToAbort(AIOperationAbortReason eReason)
{
	m_eCurrentState = AI_OPERATION_STATE_ABORTED;
	m_eAbortReason = eReason;
}


void CvAIOperation::Kill(AIOperationAbortReason eReason)
{
	int iID = m_iID;
	PlayerTypes eOwner = GetOwner();

	if (m_eAbortReason == NO_ABORT_REASON)
	{
		m_eAbortReason = eReason;
	}

	LogOperationEnd();
	Uninit();
	GET_PLAYER(eOwner).deleteAIOperation(iID);
}


void CvAIOperation::DoTurn()
{
	LogOperationStatus();
}


bool CvAIOperation::DoDelayedDeath()
{
	if(ShouldAbort())
	{
		if (m_eCurrentState == AI_OPERATION_STATE_SUCCESSFUL_FINISH)
		{
			Kill(AI_ABORT_SUCCESS);
		}
		else
		{
			Kill(AI_ABORT_KILLED);
		}
		return true;
	}

	return false;
}


CvCity* CvAIOperation::GetOperationStartCity() const
{
	if(GetStartCityPlot())
	{
		return GetStartCityPlot()->getPlotCity();
	}

	CvCity* pCapitalCity;
	CvArea* pLoopArea;
	int iLoop;
	int iMyCities;
	int iEnemyCities;
	int iBestTotal = 0;
	CvArea* pBestArea = NULL;
	CvCity* pBestCity = NULL;

	CvPlayer& thisPlayer = GET_PLAYER(m_eOwner);


	pCapitalCity = thisPlayer.getCapitalCity();

	if(pCapitalCity != NULL)
	{
		return pCapitalCity;
	}


	for(pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
	{
		if(pLoopArea->isWater())
		{
			continue;
		}

		iMyCities = pLoopArea->getCitiesPerPlayer(GetOwner());
		if(iMyCities > 0)
		{
			if(m_eEnemy != NO_PLAYER && m_eEnemy != BARBARIAN_PLAYER)
			{
				iEnemyCities = pLoopArea->getCitiesPerPlayer(m_eEnemy);
				if(iEnemyCities == 0)
				{
					continue;
				}
			}
			else
			{
				iEnemyCities = 0;
			}

			if((iMyCities + iEnemyCities) > iBestTotal)
			{
				iBestTotal = iMyCities + iEnemyCities;
				pBestArea = pLoopArea;
			}
		}
	}

	if(pBestArea != NULL)
	{

		CvCity* pCity;
		iBestTotal = 0;
		for(pCity = thisPlayer.firstCity(&iLoop); pCity != NULL; pCity = thisPlayer.nextCity(&iLoop))
		{
			if(pCity->getArea() == pBestArea->GetID())
			{
				if(pCity->getPopulation() > iBestTotal)
				{
					iBestTotal = pCity->getPopulation();
					pBestCity = pCity;
				}
			}
		}
		return pBestCity;
	}
	else
	{
		return NULL;
	}
}


bool CvAIOperation::DeleteArmyAI(int iID)
{
	std::vector<int>::iterator iter;
	for(iter = m_viArmyIDs.begin(); iter != m_viArmyIDs.end(); ++iter)
	{
		if(*iter == iID)
		{
			m_viArmyIDs.erase(iter);
			return true;
		}
	}
	return false;
}


void CvAIOperation::UnitWasRemoved(int iArmyID, int iSlotID)
{

	switch(m_eCurrentState)
	{
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_SUCCESSFUL_FINISH:
		break;

	case AI_OPERATION_STATE_RECRUITING_UNITS:
	{

		OperationSlot slotToFill;

		slotToFill.m_iOperationID = m_iID;
		slotToFill.m_iArmyID = iArmyID;
		slotToFill.m_iSlotID = iSlotID;
		m_viListOfUnitsWeStillNeedToBuild.push_back(slotToFill);
	}
	break;

	case AI_OPERATION_STATE_GATHERING_FORCES:
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	case AI_OPERATION_STATE_AT_TARGET:
	{

		CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(iArmyID);
		CvMultiUnitFormationInfo* pkFormation = GC.getMultiUnitFormationInfo(pThisArmy->GetFormationIndex());
		if(pkFormation)
		{
			if(pThisArmy->GetNumSlotsFilled() <=0 ||
			        pThisArmy->GetNumSlotsFilled() < pkFormation->getNumFormationSlotEntries() / 2)
			{

				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_HALF_STRENGTH;
			}
		}
	}
	break;
	};
}


CvPlot* CvAIOperation::ComputeCenterOfMassForTurn(CvArmyAI* pArmy, CvPlot **ppClosestCurrentCOMonPath) const
{
	CvPlot* pRtnValue = NULL;
	CvPlayer &kPlayer = GET_PLAYER(m_eOwner);

	switch(m_eCurrentState)
	{
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_AT_TARGET:
	case AI_OPERATION_STATE_SUCCESSFUL_FINISH:
		break;

	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_GATHERING_FORCES:

		pRtnValue = GetMusterPlot();
		break;

	case AI_OPERATION_STATE_MOVING_TO_TARGET:
		{
			CvPlot* pCenterOfMass = 0;
			CvPlot* pLastTurnArmyPlot = 0;
			CvAStarNode* pNode1 = 0;
			CvAStarNode* pNode2 = 0;
			int iLastNodeIndex = 0;
			FStaticVector<CvAStarNode*, SAFE_ESTIMATE_MAX_PATH_LEN, true, c_eCiv5GameplayDLL, 0> m_NodesOnPath;

			m_NodesOnPath.clear();


			CvPlot *pGoalPlot = pArmy->GetGoalPlot();
			if (!pGoalPlot->isWater() && IsAllNavalOperation())
			{
				pGoalPlot = kPlayer.GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pGoalPlot, pArmy);
			}

			pLastTurnArmyPlot = pArmy->Plot();
			pCenterOfMass = pArmy->GetCenterOfMass(IsAllNavalOperation() || IsMixedLandNavalOperation() ? DOMAIN_SEA : DOMAIN_LAND);
			if (pLastTurnArmyPlot && pCenterOfMass && pGoalPlot)
			{

#ifdef AUI_WARNING_FIXES
				CvStepPathFinder& kStepFinder = GC.getStepFinder();
				kStepFinder.SetData(&m_eEnemy);
				kStepFinder.SetDestValidFunc(NULL);
				kStepFinder.SetValidFunc(StepValidAnyArea);
				bool bFound = kStepFinder.GeneratePath(pCenterOfMass->getX(), pCenterOfMass->getY(), pGoalPlot->getX(), pGoalPlot->getY(), m_eOwner, false);
				kStepFinder.SetValidFunc(StepValid);
				kStepFinder.SetDestValidFunc(StepDestValid);
				if (bFound)
				{
					pNode1 = kStepFinder.GetLastNode();
#else
				GC.getStepFinder().SetData(&m_eEnemy);
				GC.getStepFinder().SetDestValidFunc(NULL);
				GC.getStepFinder().SetValidFunc(StepValidAnyArea);
				bool bFound = GC.getStepFinder().GeneratePath(pCenterOfMass->getX(), pCenterOfMass->getY(), pGoalPlot->getX(), pGoalPlot->getY(), m_eOwner, false);
				GC.getStepFinder().SetValidFunc(StepValid);
				GC.getStepFinder().SetDestValidFunc(StepDestValid);
				if (bFound)
				{
					pNode1 = GC.getStepFinder().GetLastNode();
#endif


					while (pNode1)
					{
						m_NodesOnPath.push_back(pNode1);
						pNode1 = pNode1->m_pParent;
					}

					iLastNodeIndex = m_NodesOnPath.size() - 1;


					int iJumpAhead = pArmy->GetMovementRate() + 1;
					int iNode1Index = max(0, iLastNodeIndex - iJumpAhead);
					int iNode2Index = min(iNode1Index + 2, iLastNodeIndex);
					pNode1 = m_NodesOnPath[iNode1Index];
					pNode2 = m_NodesOnPath[iNode2Index];
					
					pRtnValue = GC.getMap().plot(pNode1->m_iX, pNode1->m_iY);
					*ppClosestCurrentCOMonPath = GC.getMap().plot(pNode2->m_iX, pNode2->m_iY);
				}
				else
				{

					return NULL;
				}
			}
		}
		break;
	}
	return pRtnValue;
}

bool CvAIOperation::HasOneMoreSlotToFill() const
{
	if(m_viListOfUnitsWeStillNeedToBuild.size() == 1 && m_eCurrentState  == AI_OPERATION_STATE_RECRUITING_UNITS)
	{
		return true;
	}

	return false;
}

bool CvAIOperation::BuyFinalUnit()
{
	CvCity* pCity = GetOperationStartCity();

	if(!m_viListOfUnitsWeStillNeedToBuild.empty() && pCity != NULL && pCity->getOwner() == m_eOwner)
	{
		OperationSlot thisSlot = m_viListOfUnitsWeStillNeedToBuild.back();
		CvArmyAI* pArmy = GET_PLAYER(m_eOwner).getArmyAI(thisSlot.m_iArmyID);
		CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(pArmy->GetFormationIndex());
#ifdef AUI_WARNING_FIXES
		if (!thisFormation)
			return false;
#endif
		const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(thisSlot.m_iSlotID);

		CvUnit* pUnit = GET_PLAYER(m_eOwner).GetMilitaryAI()->BuyEmergencyUnit((UnitAITypes)thisSlotEntry.m_primaryUnitType, pCity);
		if(pUnit != NULL)
		{
			pArmy->AddUnit(pUnit->GetID(), thisSlot.m_iSlotID);
			m_viListOfUnitsWeStillNeedToBuild.pop_back();
			if (m_viListOfUnitsWeStillNeedToBuild.size() == 0 && m_viListOfUnitsCitiesHaveCommittedToBuild.size() == 0 && m_eCurrentState == AI_OPERATION_STATE_RECRUITING_UNITS)
			{
				m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
				for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
				{
					CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
					if(pThisArmy)
					{
						pThisArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
					}
				}
			}
			return true;
		}
	}

	return false;
}


void CvAIOperation::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_iID;
	kStream >> m_eOwner;
	kStream >> m_eEnemy;
	kStream >> m_eCurrentState;
	kStream >> m_iDefaultArea;
	kStream >> m_bShouldReplaceLossesWithReinforcements;
	kStream >> m_eMoveType;
	kStream >> m_iTargetX;
	kStream >> m_iTargetY;
	kStream >> m_iMusterX;
	kStream >> m_iMusterY;
	kStream >> m_iStartCityX;
	kStream >> m_iStartCityY;
	kStream >> m_iLastTurnMoved;
	kStream >> m_viArmyIDs;
	kStream >> m_viListOfUnitsWeStillNeedToBuild;
	kStream >> m_viListOfUnitsCitiesHaveCommittedToBuild;
}


void CvAIOperation::Write(FDataStream& kStream) const
{

	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << m_iID;
	kStream << m_eOwner;
	kStream << m_eEnemy;
	kStream << m_eCurrentState;
	kStream << m_iDefaultArea;
	kStream << m_bShouldReplaceLossesWithReinforcements;
	kStream << m_eMoveType;
	kStream << m_iTargetX;
	kStream << m_iTargetY;
	kStream << m_iMusterX;
	kStream << m_iMusterY;
	kStream << m_iStartCityX;
	kStream << m_iStartCityY;
	kStream << m_iLastTurnMoved;
	kStream << m_viArmyIDs;
	kStream << m_viListOfUnitsWeStillNeedToBuild;
	kStream << m_viListOfUnitsCitiesHaveCommittedToBuild;
}




void CvAIOperation::LogOperationStart()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp1;
		CvString strTemp2;

		CvString strPlayerName = GET_PLAYER(m_eOwner).getCivilizationShortDescription();
		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", " + GetOperationName() + ", ";

		for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
			strTemp1.Format("Started, Army: %d, Units Recruited: %d, Max Formation Size: %d, ", pThisArmy->GetID(), pThisArmy->GetNumSlotsFilled(), pThisArmy->GetNumFormationEntries());
		}

		strOutBuf = strBaseString + strTemp1;
		switch(m_eCurrentState)
		{
		case AI_OPERATION_STATE_ABORTED:
			strTemp2 = "Aborted";
			break;
		case AI_OPERATION_STATE_RECRUITING_UNITS:
			strTemp2 = "Recruiting Units";
			break;
		case AI_OPERATION_STATE_GATHERING_FORCES:
			strTemp2 = "Gathering Forces";
			break;
		case AI_OPERATION_STATE_MOVING_TO_TARGET:
			strTemp2 = "Moving To Target";
			break;
		case AI_OPERATION_STATE_AT_TARGET:
			strTemp2 = "At Target";
			break;
		case AI_OPERATION_STATE_SUCCESSFUL_FINISH:
			strTemp2 = "Completed";
			break;
		};

		strOutBuf += strTemp2;

		pLog->Msg(strOutBuf);


		switch (GetOperationType())
		{
		case AI_OPERATION_BASIC_CITY_ATTACK:
		case AI_OPERATION_PILLAGE_ENEMY:
		case AI_OPERATION_SNEAK_CITY_ATTACK:
		case AI_OPERATION_SMALL_CITY_ATTACK:
		case AI_OPERATION_NAVAL_BOMBARDMENT:
		case AI_OPERATION_NAVAL_SUPERIORITY:
		case AI_OPERATION_COLONIZE:
		case AI_OPERATION_NAVAL_ATTACK:
		case AI_OPERATION_NAVAL_SNEAK_ATTACK:
		case AI_OPERATION_CITY_STATE_ATTACK:
		case AI_OPERATION_CITY_STATE_NAVAL_ATTACK:
		case AI_OPERATION_NUKE_ATTACK:
		case AI_OPERATION_PURE_NAVAL_CITY_ATTACK:
			
			strOutBuf = GetOperationName() + ", ";
			strOutBuf += strTemp1 + strTemp2;
			if (m_eEnemy != NO_PLAYER)
			{
				strPlayerName = GET_PLAYER(m_eEnemy).getCivilizationShortDescription();
				strOutBuf += ", vs. " + strPlayerName;
			}
			GET_PLAYER(m_eOwner).GetMilitaryAI()->LogMilitarySummaryMessage(strOutBuf);
		}
	}
}


void CvAIOperation::LogOperationStatus()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp, szTemp2, szTemp3;
		CvString strPlayerName;
		FILogFile* pLog;

		strPlayerName = GET_PLAYER(m_eOwner).getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", " + GetOperationName() + ", ";

		switch(m_eCurrentState)
		{
		case AI_OPERATION_STATE_ABORTED:
			strTemp = "Aborted";
			break;
		case AI_OPERATION_STATE_RECRUITING_UNITS:
			strTemp = "";
			for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
				szTemp2.Format("Recruiting Units, Army: %d, Muster Turn: %d, SLOT DETAIL:, ", pThisArmy->GetID(), pThisArmy->GetTurnAtNextCheckpoint());
				strTemp += szTemp2;

				for(int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
				{
					CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(iI);
					if(pSlot->GetTurnAtCheckpoint() == ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT)
					{
						szTemp2 = "No Info, ";
					}
					else if(pSlot->GetTurnAtCheckpoint() == ARMYSLOT_NOT_INCLUDING_IN_OPERATION)
					{
						szTemp2 = "Skipping, ";
					}
					else if(pSlot->GetUnitID() == NO_UNIT)
					{
						szTemp3.Format("Turn %d, ", pSlot->GetTurnAtCheckpoint());
						szTemp2 = "Training - " + szTemp3;
					}
					else if(pSlot->HasStartedOnOperation())
					{
						UnitHandle pThisUnit = GET_PLAYER(m_eOwner).getUnit(pSlot->GetUnitID());
						if(pThisUnit)
						{
							szTemp2.Format("Gathering at (%d-%d), ", pThisUnit->getX(), pThisUnit->getY());
						}
					}
					else
					{
						UnitHandle pThisUnit = GET_PLAYER(m_eOwner).getUnit(pSlot->GetUnitID());
						if(pThisUnit)
						{
							szTemp2.Format("%s - Turn %d, ", pThisUnit->getName().GetCString(), pSlot->GetTurnAtCheckpoint());
						}
					}
					strTemp += szTemp2;
				}
			}
			break;
		case AI_OPERATION_STATE_GATHERING_FORCES:
			strTemp = "";
			for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
				szTemp2.Format("Gathering Forces, Army: %d, Gather X: %d, Gather Y: %d, ", pThisArmy->GetID(), pThisArmy->GetX(), pThisArmy->GetY());
				strTemp += szTemp2;
				int iUnitID;
				iUnitID = pThisArmy->GetFirstUnitID();
				while(iUnitID != ARMY_NO_UNIT)
				{

					UnitHandle pThisUnit = GET_PLAYER(m_eOwner).getUnit(iUnitID);
					if(pThisUnit)
					{
						szTemp2.Format("%s at (%d-%d),", pThisUnit->getName().GetCString(), pThisUnit->getX(), pThisUnit->getY());
						strTemp += szTemp2;
					}
					iUnitID = pThisArmy->GetNextUnitID();
				}
			}
			break;
		case AI_OPERATION_STATE_MOVING_TO_TARGET:
			strTemp = "";
			for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
				szTemp2.Format("Moving To Target, Army: %d, At X: %d, At Y: %d, To X: %d, To Y: %d, ", pThisArmy->GetID(), pThisArmy->GetX(), pThisArmy->GetY(), m_iTargetX, m_iTargetY);
				strTemp += szTemp2;
				int iUnitID;
				iUnitID = pThisArmy->GetFirstUnitID();
				while(iUnitID != ARMY_NO_UNIT)
				{

					UnitHandle pThisUnit = GET_PLAYER(m_eOwner).getUnit(iUnitID);
					if(pThisUnit)
					{
						szTemp2.Format("%s at (%d-%d),", pThisUnit->getName().GetCString(), pThisUnit->getX(), pThisUnit->getY());
						strTemp += szTemp2;
					}
					iUnitID = pThisArmy->GetNextUnitID();
				}
			}
			break;
		case AI_OPERATION_STATE_AT_TARGET:
			strTemp = "At Target";
			break;
		case AI_OPERATION_STATE_SUCCESSFUL_FINISH:
			strTemp = "Completed";
			break;
		};

		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}
}

void CvAIOperation::LogOperationSpecialMessage(const CvString& strMsg)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;

		CvString strPlayerName = GET_PLAYER(m_eOwner).getCivilizationShortDescription();
		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", " + GetOperationName() + ", ";
		strOutBuf = strBaseString + strMsg;
		pLog->Msg(strOutBuf);
	}
}


void CvAIOperation::LogOperationEnd()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;

		CvString strPlayerName = GET_PLAYER(m_eOwner).getCivilizationShortDescription();
		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", " + GetOperationName() + ", ";

		strTemp = "Ended, ";

		switch(m_eAbortReason)
		{
		case AI_ABORT_SUCCESS:
			strTemp += "Success";
			break;
		case AI_ABORT_NO_TARGET:
			strTemp += "No target";
			break;
		case AI_ABORT_REPEAT_TARGET:
			strTemp += "Repeat target";
			break;
		case AI_ABORT_LOST_TARGET:
			strTemp += "Lost target";
			break;
		case AI_ABORT_TARGET_ALREADY_CAPTURED:
			strTemp += "Target already captured";
			break;
		case AI_ABORT_NO_ROOM_DEPLOY:
			strTemp += "No room to deploy";
			break;
		case AI_ABORT_HALF_STRENGTH:
			strTemp += "Half strength";
			break;
		case AI_ABORT_NO_MUSTER:
			strTemp += "No muster point";
			break;
		case AI_ABORT_LOST_CIVILIAN:
			strTemp += "Lost civilian";
			break;
		case AI_ABORT_ESCORT_DIED:
			strTemp += "Escort died";
			break;
		case AI_ABORT_NO_NUKES:
			strTemp += "No nukes";
			break;
		case AI_ABORT_KILLED:
			strTemp += "Killed";
			break;
		case AI_ABORT_WAR_STATE_CHANGE:
			strTemp += "War State Change";
			break;
		case AI_ABORT_DIPLO_OPINION_CHANGE:
			strTemp += "Diplo Opinion Change";
			break;
		case AI_ABORT_LOST_PATH:
			strTemp += "Lost Path to Target";
			break;
		}

		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);


		switch (GetOperationType())
		{
		case AI_OPERATION_BASIC_CITY_ATTACK:
		case AI_OPERATION_PILLAGE_ENEMY:
		case AI_OPERATION_SNEAK_CITY_ATTACK:
		case AI_OPERATION_SMALL_CITY_ATTACK:
		case AI_OPERATION_NAVAL_BOMBARDMENT:
		case AI_OPERATION_NAVAL_SUPERIORITY:
		case AI_OPERATION_COLONIZE:
		case AI_OPERATION_NAVAL_ATTACK:
		case AI_OPERATION_NAVAL_SNEAK_ATTACK:
		case AI_OPERATION_CITY_STATE_ATTACK:
		case AI_OPERATION_CITY_STATE_NAVAL_ATTACK:
		case AI_OPERATION_NUKE_ATTACK:
		case AI_OPERATION_PURE_NAVAL_CITY_ATTACK:
			strOutBuf = GetOperationName() + ", ";
			strOutBuf += strTemp;
			GET_PLAYER(m_eOwner).GetMilitaryAI()->LogMilitarySummaryMessage(strOutBuf);
		}
	}
}


CvString CvAIOperation::GetLogFileName(CvString& playerName) const
{
	CvString strLogName;


	if(GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "OperationalAILog_" + playerName + ".csv";
	}
	else
	{
		strLogName = "OperationalAILog.csv";
	}

	return strLogName;
}


CvPlot* CvAIOperation::SelectInitialMusterPoint(CvArmyAI* pThisArmy)
{
	CvPlot* pMusterPt = NULL;
	CvPlot* pStartCityPlot;
	CvCity* pStartCity;

	pStartCity = GetOperationStartCity();
	if(pStartCity)
	{
		pStartCityPlot = pStartCity->plot();

		if(pStartCityPlot != NULL)
		{

			pMusterPt = GC.getStepFinder().GetLastOwnedPlot(GetOwner(), GetEnemy(), pStartCityPlot, pThisArmy->GetGoalPlot());


			if(pMusterPt == NULL)
			{
				return NULL;
			}
			else
			{
				SetMusterPlot(pMusterPt);
			}
		}
	}

	return pMusterPt;
}

class CvOperationSearchUnit
{
public:
	CvOperationSearchUnit();
	CvOperationSearchUnit(CvUnit* pkUnit, int iDistance) { m_pkUnit = pkUnit; m_iDistance = iDistance; };

	bool operator<(const CvOperationSearchUnit& kUnit) const
	{
		return (GetDistance() < kUnit.GetDistance());
	}


	void SetUnit(CvUnit* pkUnit)
	{
		m_pkUnit = pkUnit;
	};
	CvUnit* GetUnit() const
	{
		return m_pkUnit;
	};
	void SetDistance(int iDistance)
	{
		m_iDistance = iDistance;
	};
	int GetDistance() const
	{
		return m_iDistance;
	};
private:
	int		m_iDistance;
	CvUnit* m_pkUnit;
};

typedef FStaticVector< CvOperationSearchUnit, 128, true, c_eCiv5GameplayDLL > CvOperationSearchUnitList;

static CvUnit* GetClosestUnit(CvOperationSearchUnitList& kSearchList, CvPlot* pkMusterPlot, CvPlot* pkTarget, bool bNeedToCheckTarget)
{
	CvUnit* pkBestUnit = NULL;
	if (kSearchList.size())
	{
		std::stable_sort(kSearchList.begin(), kSearchList.end());

		int iBestDistance = MAX_INT;
		CvIgnoreUnitsPathFinder& kPathFinder = GC.getIgnoreUnitsPathFinder();
		for (CvOperationSearchUnitList::iterator itr = kSearchList.begin(); itr != kSearchList.end(); ++itr)
		{
			CvUnit *pkLoopUnit = (*itr).GetUnit();
#ifdef AUI_WARNING_FIXES
			if (!pkLoopUnit)
				continue;
#endif
			int iDistance = (*itr).GetDistance();

			int iPathDistance = MAX_INT;

			if (pkMusterPlot != NULL)
			{
#ifdef AUI_ASTAR_MINOR_OPTIMIZATION
				if (!kPathFinder.DoesPathExist(pkLoopUnit, pkLoopUnit->plot(), pkMusterPlot))
#else
				if (!kPathFinder.DoesPathExist(*pkLoopUnit, pkLoopUnit->plot(), pkMusterPlot))
#endif
					continue;

				iPathDistance = kPathFinder.GetPathLength();
			}

			if(pkTarget != NULL && bNeedToCheckTarget)
			{
#ifdef AUI_ASTAR_MINOR_OPTIMIZATION
				if (!kPathFinder.DoesPathExist(pkLoopUnit, pkLoopUnit->plot(), pkTarget))
#else
				if (!kPathFinder.DoesPathExist(*pkLoopUnit, pkLoopUnit->plot(), pkTarget))
#endif
					continue;

				if (pkMusterPlot == NULL)
					iPathDistance = kPathFinder.GetPathLength();
			}


			if (iPathDistance <= iDistance && iPathDistance <= iBestDistance)
			{
				pkBestUnit = pkLoopUnit;
				break;
			}

			if (iPathDistance < iBestDistance)
			{
				pkBestUnit = pkLoopUnit;
				iBestDistance = iPathDistance;
			}


			if (iDistance >= GC.getAI_HOMELAND_ESTIMATE_TURNS_DISTANCE())
				break;
		}
	}

	return pkBestUnit;
}


#ifdef AUI_WARNING_FIXES
bool CvAIOperation::FindBestFitReserveUnit(const OperationSlot& thisOperationSlot, CvPlot* pMusterPlot, CvPlot* pTargetPlot, bool* bRequired)
#else
bool CvAIOperation::FindBestFitReserveUnit(OperationSlot thisOperationSlot, CvPlot* pMusterPlot, CvPlot* pTargetPlot, bool* bRequired)
#endif
{
	CvUnit* pBestUnit = NULL;
	CvPlayerAI& ownerPlayer = GET_PLAYER(m_eOwner);
	CvArmyAI* pThisArmy = ownerPlayer.getArmyAI(thisOperationSlot.m_iArmyID);
	CvString strMsg;
	CvPlot *pTargetToUse = pTargetPlot;

	CvOperationSearchUnitList kSearchList;

	*bRequired = true;


	if (pTargetToUse && !pTargetToUse->isWater() && IsAllNavalOperation())
	{
		pTargetToUse = ownerPlayer.GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pTargetToUse, pThisArmy);
	}

	int iThisFormationIndex = pThisArmy->GetFormationIndex();
	if(iThisFormationIndex != NO_MUFORMATION)
	{
		CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(iThisFormationIndex);
		if(thisFormation)
		{
			const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(thisOperationSlot.m_iSlotID);
			bool bMustBeDeepWaterNaval = GET_TEAM(ownerPlayer.getTeam()).canEmbarkAllWaterPassage() && thisFormation->IsRequiresNavalUnitConsistency();

			int iLoop = 0;
			for(CvUnit* pLoopUnit = ownerPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = ownerPlayer.nextUnit(&iLoop))
			{
				const UnitAITypes eLoopUnitAIType = pLoopUnit->AI_getUnitAIType();


				if(pLoopUnit->canRecruitFromTacticalAI() && pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX &&
				        eLoopUnitAIType != UNITAI_EXPLORE && eLoopUnitAIType != UNITAI_EXPLORE_SEA && pLoopUnit->getDropRange() == 0                      )
				{

					CvUnitEntry* unitInfo = GC.getUnitInfo(pLoopUnit->getUnitType());
					if(unitInfo == NULL)
						continue;



					if(unitInfo->GetUnitAIType((UnitAITypes)thisSlotEntry.m_primaryUnitType))
					{

						if(!pLoopUnit->IsCombatUnit() || pLoopUnit->GetCurrHitPoints() >= pLoopUnit->GetMaxHitPoints() * GC.getAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION() / 100)
						{

							if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() < GC.getGame().getGameTurn())
							{
								if((!IsAllNavalOperation() && !IsMixedLandNavalOperation()) || pLoopUnit->getDomainType() == DOMAIN_SEA || pLoopUnit->CanEverEmbark())
								{
									if (!bMustBeDeepWaterNaval || pLoopUnit->getDomainType() != DOMAIN_SEA || !pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
									{

										CvPlot* pkLoopUnitPlot = pLoopUnit->plot();
										int iDistance;
										if(pMusterPlot != NULL)
										{
											iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pMusterPlot->getX(), pMusterPlot->getY());

											if(pMusterPlot != NULL && pLoopUnit->getDomainType() == DOMAIN_LAND && pkLoopUnitPlot->getArea() != pMusterPlot->getArea())
											{
												iDistance *= 2;
											}
										}
										else
										{
											if (pTargetToUse != NULL)
												iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pTargetToUse->getX(), pTargetToUse->getY());
											else
											{
												CvAssertMsg(0, "No muster or target!");
												iDistance = MAX_INT;
											}
										}

										kSearchList.push_back(CvOperationSearchUnit(pLoopUnit, iDistance));
									}
								}
							}
						}
					}
				}
			}

			pBestUnit = GetClosestUnit(kSearchList, pMusterPlot, pTargetToUse, NeedToCheckPathToTarget());


			if(pBestUnit != NULL)
			{
				pThisArmy->AddUnit(pBestUnit->GetID(), thisOperationSlot.m_iSlotID);
				return true;
			}
			else
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("Could not recruit from primary units for muster at x=%d y=%d, target of x=%d y=%d", (pMusterPlot)?pMusterPlot->getX():-1, (pMusterPlot)?pMusterPlot->getY():-1, (pTargetToUse)?pTargetToUse->getX():-1, (pTargetToUse)?pTargetToUse->getY():-1);
					LogOperationSpecialMessage(strMsg);
				}
			}

			kSearchList.clear();

			for(CvUnit* pLoopUnit = ownerPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = ownerPlayer.nextUnit(&iLoop))
			{
				const UnitAITypes eLoopUnitAIType = pLoopUnit->AI_getUnitAIType();


				if(pLoopUnit->canRecruitFromTacticalAI() && pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX &&
				        eLoopUnitAIType != UNITAI_EXPLORE && eLoopUnitAIType != UNITAI_EXPLORE_SEA && pLoopUnit->getDropRange() == 0                      )
				{

					CvUnitEntry* unitInfo = GC.getUnitInfo(pLoopUnit->getUnitType());
					if(unitInfo == NULL)
						continue;



					if(unitInfo->GetUnitAIType((UnitAITypes)thisSlotEntry.m_secondaryUnitType))
					{

						if(!pLoopUnit->IsCombatUnit() || pLoopUnit->GetCurrHitPoints() >= pLoopUnit->GetMaxHitPoints() * GC.getAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION() / 100)
						{

							if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() < GC.getGame().getGameTurn())
							{
								if((!IsAllNavalOperation() && !IsMixedLandNavalOperation()) || pLoopUnit->getDomainType() == DOMAIN_SEA || pLoopUnit->CanEverEmbark())
								{
									if (!bMustBeDeepWaterNaval || pLoopUnit->getDomainType() != DOMAIN_SEA || !pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
									{

										CvPlot* pkLoopUnitPlot = pLoopUnit->plot();
										int iDistance;
										if(pMusterPlot != NULL)
										{
											iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pMusterPlot->getX(), pMusterPlot->getY());

											if(pMusterPlot != NULL && pLoopUnit->getDomainType() == DOMAIN_LAND && pkLoopUnitPlot->getArea() != pMusterPlot->getArea())
											{
												iDistance *= 2;
											}
										}
										else
										{
											if (pTargetToUse != NULL)
												iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pTargetToUse->getX(), pTargetToUse->getY());
											else
											{
												CvAssertMsg(0, "No muster or target!");
												iDistance = MAX_INT;
											}
										}

										kSearchList.push_back(CvOperationSearchUnit(pLoopUnit, iDistance));
									}
								}
							}
						}
					}
				}
			}

			pBestUnit = GetClosestUnit(kSearchList, pMusterPlot, pTargetToUse, NeedToCheckPathToTarget());


			if(pBestUnit != NULL)
			{
				pThisArmy->AddUnit(pBestUnit->GetID(), thisOperationSlot.m_iSlotID);
				return true;
			}
			else
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("Could not recruit from secondary units for muster at x=%d y=%d, target of x=%d y=%d", (pMusterPlot)?pMusterPlot->getX():-1, (pMusterPlot)?pMusterPlot->getY():-1, (pTargetToUse)?pTargetToUse->getX():-1, (pTargetToUse)?pTargetToUse->getY():-1);
					LogOperationSpecialMessage(strMsg);
				}
			}


			if(!thisSlotEntry.m_requiredSlot)
			{
				*bRequired = false;
			}
		}
	}

	return false;
}

FDataStream& operator<<(FDataStream& saveTo, const AIOperationMovementType& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, AIOperationMovementType& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<AIOperationMovementType>(v);
	return loadFrom;
}




CvAIEnemyTerritoryOperation::CvAIEnemyTerritoryOperation()
{
}

CvAIEnemyTerritoryOperation::~CvAIEnemyTerritoryOperation()
{
}


int CvAIEnemyTerritoryOperation::GetMaximumRecruitTurns() const
{
	return GC.getAI_OPERATIONAL_MAX_RECRUIT_TURNS_ENEMY_TERRITORY();
}


void CvAIEnemyTerritoryOperation::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int                   , CvCity*, CvCity*)
{
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_ENEMY_TERRITORY;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;

	if(iID != -1)
	{

		CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
		CvArmyAI* pArmyAI = kPlayer.addArmyAI();
		if(pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());


			CvPlot* pTargetPlot = FindBestTarget();
			if(pTargetPlot != NULL)
			{
				SetTargetPlot(pTargetPlot);
				pArmyAI->SetGoalPlot(pTargetPlot);
				if(SelectInitialMusterPoint(pArmyAI) != NULL)
				{
					pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
					SetDefaultArea(GetMusterPlot()->getArea());

					if (GetDefaultArea() != pTargetPlot->getArea())
					{
						pArmyAI->SetGoalPlot(pTargetPlot);
					}
					else
					{
						CvPlot* pDeployPt;
						pDeployPt = GC.getStepFinder().GetXPlotsFromEnd(GetOwner(), GetEnemy(), GetMusterPlot(), GetTargetPlot(), (GetDeployRange() / 2), true);
						if (pDeployPt)
						{
							pArmyAI->SetGoalPlot(pDeployPt);
						}
						else
						{

							m_eCurrentState = AI_OPERATION_STATE_ABORTED;
							m_eAbortReason = AI_ABORT_LOST_PATH;
						}
					}


					BuildListOfUnitsWeStillNeedToBuild();


					if(GrabUnitsFromTheReserves(GetMusterPlot(), pTargetPlot))
					{
						pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
						m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
					}
					else
					{
						m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
					}

					LogOperationStart();
				}
				else
				{

					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_NO_MUSTER;
				}
			}

			else
			{

				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_LOST_TARGET;
			}
		}
	}
}


int CvAIEnemyTerritoryOperation::GetDeployRange() const
{
	return GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE();
}


CvPlot* CvAIEnemyTerritoryOperation::SelectInitialMusterPoint(CvArmyAI* pThisArmy)
{
	CvPlot* pMusterPt = NULL;
	CvPlot* pStartCityPlot;
	CvCity* pStartCity;
	CvPlot* pCurrentPlot;
	int iSpacesFromTarget = 0;
	CvPlot* pDeployPlot = NULL;
	int iDangerousPlots = 0;

	pStartCity = GetOperationStartCity();
	if(pStartCity)
	{
		pStartCityPlot = pStartCity->plot();

		if(pStartCityPlot != NULL)
		{

			if (pStartCityPlot->getArea() != pThisArmy->GetGoalPlot()->getArea())
			{
				SetMusterPlot(pStartCityPlot);
				return pStartCityPlot;
			}

			CvAStarNode* pNode;


			GC.getStepFinder().SetData(&m_eEnemy);
			if(GC.getStepFinder().GeneratePath(pStartCityPlot->getX(), pStartCityPlot->getY(), pThisArmy->GetGoalPlot()->getX(), pThisArmy->GetGoalPlot()->getY(), m_eOwner, false))
			{
				pNode = GC.getStepFinder().GetLastNode();


				while(pNode != NULL)
				{
					pCurrentPlot = GC.getMap().plotCheckInvalid(pNode->m_iX, pNode->m_iY);


					if(iSpacesFromTarget == GetDeployRange())
					{
						pDeployPlot = pCurrentPlot;
					}


					if(pCurrentPlot->getOwner() == GetOwner())
					{
						pMusterPt = pCurrentPlot;
						break;
					}

					else
					{

						if(GET_PLAYER(m_eOwner).GetPlotDanger(*pCurrentPlot) > 0)
						{
							iDangerousPlots++;
						}
					}


					iSpacesFromTarget++;
					pNode = pNode->m_pParent;
				}


				if(iSpacesFromTarget > 0 && (iDangerousPlots * 100 / iSpacesFromTarget) < GC.getAI_OPERATIONAL_PERCENT_DANGER_FOR_FORWARD_MUSTER())
				{
					if(pDeployPlot)
					{
						pMusterPt = pDeployPlot;
					}
				}
			}
		}
	}

	if(pMusterPt != NULL)
	{
		SetMusterPlot(pMusterPt);
	}
	else
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString szMsg;
			szMsg.Format("No muster point found, Operation aborting, Target was, X: %d, Y: %d", pThisArmy->GetGoalPlot()->getX(), pThisArmy->GetGoalPlot()->getY());
			LogOperationSpecialMessage(szMsg);
		}
	}

	return pMusterPt;
}






CvAIOperationBasicCityAttack::CvAIOperationBasicCityAttack()
{
}


CvAIOperationBasicCityAttack::~CvAIOperationBasicCityAttack()
{
}


void CvAIOperationBasicCityAttack::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int                   , CvCity* pTarget, CvCity* pMuster)
{
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_ENEMY_TERRITORY;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;
	SetStartCityPlot(pMuster->plot());

	if(iID != -1)
	{

		CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
		CvArmyAI* pArmyAI = kPlayer.addArmyAI();
		if(pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());

			if(pTarget)
			{
				SetTargetPlot(pTarget->plot());
				pArmyAI->SetGoalPlot(GetTargetPlot());
				SetMusterPlot(GetStartCityPlot());
				pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
				SetDefaultArea(GetMusterPlot()->getArea());


				CvPlot* pDeployPt;
				pDeployPt = GC.getStepFinder().GetXPlotsFromEnd(GetOwner(), GetEnemy(), GetMusterPlot(), GetTargetPlot(), (GetDeployRange() / 2), true);
				if (pDeployPt)
				{
					pArmyAI->SetGoalPlot(pDeployPt);


					BuildListOfUnitsWeStillNeedToBuild();


					if(GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot()))
					{
						pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
						m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
					}
					else
					{
						m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
					}

					LogOperationStart();
				}
				else
				{
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_LOST_PATH;
				}
			}

			else
			{

				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_LOST_TARGET;
			}
		}
	}
}


void CvAIOperationBasicCityAttack::Read(FDataStream& kStream)
{

	CvAIOperation::Read(kStream);


	uint uiVersion;
	kStream >> uiVersion;
}


void CvAIOperationBasicCityAttack::Write(FDataStream& kStream) const
{

	CvAIOperation::Write(kStream);


	uint uiVersion = 1;
	kStream << uiVersion;
}

MultiunitFormationTypes CvAIOperationBasicCityAttack::GetFormation() const
{
	return (GC.getGame().getHandicapInfo().GetID() > 4 && !(GC.getMap().GetAIMapHint() & 1)) ? MUFORMATION_BIGGER_CITY_ATTACK_FORCE : MUFORMATION_BASIC_CITY_ATTACK_FORCE;
}


bool CvAIOperationBasicCityAttack::ArmyInPosition(CvArmyAI* pArmy)
{
	bool bStateChanged = false;

	switch(m_eCurrentState)
	{

	case AI_OPERATION_STATE_GATHERING_FORCES:
	{

		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);


		CvPlot* pTarget = GetTargetPlot();
		if(pTarget->getOwner() != m_eEnemy)
		{
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_TARGET_ALREADY_CAPTURED;
		}
	}
	break;


	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{
		CvPlot *pCenterOfMass = pArmy->GetCenterOfMass(DOMAIN_LAND);


		if(pCenterOfMass && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), m_iTargetX, m_iTargetY) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE())
		{

			GET_PLAYER(GetOwner()).GetDiplomacyAI()->SetMusteringForAttack(GetEnemy(), true);


			CvTemporaryZone zone;
			zone.SetX(GetTargetPlot()->getX());
			zone.SetY(GetTargetPlot()->getY());
			zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
			zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
			GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);

			m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
		}
	}
	break;


	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}


bool CvAIOperationBasicCityAttack::ShouldAbort()
{

	bool rtnValue = CvAIOperation::ShouldAbort();

	if(!rtnValue)
	{

		if(GetTargetPlot()->getPlotCity() == NULL || GetTargetPlot()->getOwner() != m_eEnemy)
		{

			return true;
		}
	}

	return rtnValue;
}


CvPlot* CvAIOperationBasicCityAttack::FindBestTarget()
{
	CvAssertMsg(false, "Obsolete function called CvAIOperationBasicCityAttack::FindBestTarget()");

	return NULL;
}






CvAIOperationSneakCityAttack::CvAIOperationSneakCityAttack()
{
}
MultiunitFormationTypes CvAIOperationSneakCityAttack::GetFormation() const
{
	return (GC.getGame().getHandicapInfo().GetID() > 4 && !(GC.getMap().GetAIMapHint() & 1)) ? MUFORMATION_BIGGER_CITY_ATTACK_FORCE : MUFORMATION_BASIC_CITY_ATTACK_FORCE;
}

CvAIOperationQuickSneakCityAttack::CvAIOperationQuickSneakCityAttack()
{
}






CvAIOperationSmallCityAttack::CvAIOperationSmallCityAttack()
{
}






CvAIOperationCityStateAttack::CvAIOperationCityStateAttack()
{
}
MultiunitFormationTypes CvAIOperationCityStateAttack::GetFormation() const
{
	return (GC.getGame().getHandicapInfo().GetID() > 4 && !(GC.getMap().GetAIMapHint() & 1)) ? MUFORMATION_BIGGER_CITY_ATTACK_FORCE : MUFORMATION_CITY_STATE_ATTACK_FORCE;
}






CvAIOperationDestroyBarbarianCamp::CvAIOperationDestroyBarbarianCamp()
{
	m_bCivilianRescue = false;
	m_iUnitToRescue = NULL;
}


CvAIOperationDestroyBarbarianCamp::~CvAIOperationDestroyBarbarianCamp()
{
}


int CvAIOperationDestroyBarbarianCamp::GetDeployRange() const
{
	return GC.getAI_OPERATIONAL_BARBARIAN_CAMP_DEPLOY_RANGE();
}


void CvAIOperationDestroyBarbarianCamp::Read(FDataStream& kStream)
{

	CvAIOperation::Read(kStream);


	uint uiVersion;
	kStream >> uiVersion;
	m_bCivilianRescue = false;
	kStream >> m_bCivilianRescue;
	kStream >> m_iUnitToRescue;
}


void CvAIOperationDestroyBarbarianCamp::Write(FDataStream& kStream) const
{

	CvAIOperation::Write(kStream);


	uint uiVersion = 1;
	kStream << uiVersion;
	kStream << m_bCivilianRescue;
	kStream << m_iUnitToRescue;
}


bool CvAIOperationDestroyBarbarianCamp::ArmyInPosition(CvArmyAI* pArmy)
{
	bool bStateChanged = false;

	switch(m_eCurrentState)
	{

	case AI_OPERATION_STATE_GATHERING_FORCES:
	{

		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);


		CvPlot* possibleBetterTarget;
		possibleBetterTarget = FindBestTarget();


		if(possibleBetterTarget == NULL)
		{
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_LOST_TARGET;
		}


		else if(possibleBetterTarget != GetTargetPlot())
		{

			if (pArmy->GetArea() == possibleBetterTarget->getArea())
			{
				CvPlot* pDeployPt = GC.getStepFinder().GetXPlotsFromEnd(GetOwner(), GetEnemy(), pArmy->Plot(), possibleBetterTarget, GC.getAI_OPERATIONAL_BARBARIAN_CAMP_DEPLOY_RANGE(), false);
				if(pDeployPt != NULL)
				{
					pArmy->SetGoalPlot(pDeployPt);
					SetTargetPlot(possibleBetterTarget);
				}
			}


			else
			{
				pArmy->SetGoalPlot(possibleBetterTarget);
				SetTargetPlot(possibleBetterTarget);
			}
		}
	}
	break;


	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{
		if (plotDistance(pArmy->GetX(), pArmy->GetY(), pArmy->GetGoalX(), pArmy->GetGoalY()) <= 1)
		{

			CvTemporaryZone zone;
			zone.SetX(GetTargetPlot()->getX());
			zone.SetY(GetTargetPlot()->getY());
			zone.SetTargetType(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
			zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
			GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);

			m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
		}
	}
	break;


	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}


bool CvAIOperationDestroyBarbarianCamp::ShouldAbort()
{
	CvString strMsg;


	bool rtnValue = CvAIOperation::ShouldAbort();

	if(!rtnValue)
	{

		if (!m_bCivilianRescue && GetTargetPlot()->getImprovementType() != GC.getBARBARIAN_CAMP_IMPROVEMENT())
		{

			if(GC.getLogging() && GC.getAILogging())
			{
				strMsg.Format("Barbarian camp at (x=%d y=%d) no longer exists. Aborting", GetTargetPlot()->getX(), GetTargetPlot()->getY());
				LogOperationSpecialMessage(strMsg);
			}
			return true;
		}

		else if (m_bCivilianRescue)
		{

			CvPlayerAI& BarbPlayer = GET_PLAYER(BARBARIAN_PLAYER);
			CvUnit* pUnitToRescue = BarbPlayer.getUnit(m_iUnitToRescue);
			if (!pUnitToRescue)
			{
				if (GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format ("Civilian can no longer be rescued from barbarians. Aborting");
					LogOperationSpecialMessage(strMsg);
				}
				return true;
			}
			else
			{
				if (pUnitToRescue->GetOriginalOwner() != m_eOwner || (pUnitToRescue->AI_getUnitAIType() != UNITAI_SETTLE && pUnitToRescue->AI_getUnitAIType() != UNITAI_WORKER))
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						strMsg.Format ("Civilian can no longer be rescued from barbarians. Aborting");
						LogOperationSpecialMessage(strMsg);
					}
					return true;
				}
			}
		}

		else if(m_eCurrentState != AI_OPERATION_STATE_RECRUITING_UNITS)
		{

			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[0]);
			CvPlot* pTarget = GetTargetPlot();
			UnitHandle pCampDefender = pTarget->getBestDefender(NO_PLAYER);
			if(pCampDefender && pThisArmy->GetTotalPower() < pCampDefender->GetPower())
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("Barbarian camp stronger (%d) than our units (%d). Aborting", pCampDefender->GetPower(), pThisArmy->GetTotalPower());
					LogOperationSpecialMessage(strMsg);
				}
				return true;
			}
		}
	}

	return rtnValue;
}


CvPlot* CvAIOperationDestroyBarbarianCamp::FindBestTarget()
{
#ifdef AUI_WARNING_FIXES
	uint iPlotLoop;
#else
	int iPlotLoop;
#endif
	CvPlot* pBestPlot = NULL;
	CvPlot* pPlot;
	int iBestPlotDistance = MAX_INT;
	int iCurPlotDistance;

	m_bCivilianRescue = false;

	TeamTypes eTeam = GET_PLAYER(m_eOwner).getTeam();
	ImprovementTypes eBarbCamp = (ImprovementTypes) GC.getBARBARIAN_CAMP_IMPROVEMENT();

	CvCity* pStartCity;
	pStartCity = GetOperationStartCity();
	if(pStartCity != NULL)
	{




		CvPlayerAI& BarbPlayer = GET_PLAYER(BARBARIAN_PLAYER);

		CvUnit* pLoopUnit = NULL;
		int iLoop;
		for (pLoopUnit = BarbPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = BarbPlayer.nextUnit(&iLoop))
		{
			if (pLoopUnit->GetOriginalOwner() == m_eOwner && (pLoopUnit->AI_getUnitAIType() == UNITAI_SETTLE || pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER || pLoopUnit->AI_getUnitAIType() == UNITAI_ARCHAEOLOGIST))
			{
				iCurPlotDistance = GC.getStepFinder().GetStepDistanceBetweenPoints(m_eOwner, m_eEnemy, pLoopUnit->plot(), pStartCity->plot());
				if (iCurPlotDistance < iBestPlotDistance)
				{
					pBestPlot = pLoopUnit->plot();
					iBestPlotDistance = iCurPlotDistance;
					m_bCivilianRescue = true;
					m_iUnitToRescue = pLoopUnit->GetID();
				}
			}
		}

		if (!pBestPlot)
		{

			for (iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
			{
				pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

				if (pPlot->isRevealed(eTeam))
				{
					if (pPlot->getRevealedImprovementType(eTeam) == eBarbCamp)
					{


						{
							iCurPlotDistance = GC.getStepFinder().GetStepDistanceBetweenPoints(m_eOwner, m_eEnemy, pPlot, pStartCity->plot());

							if (iCurPlotDistance < iBestPlotDistance)
							{
								pBestPlot = pPlot;
								iBestPlotDistance = iCurPlotDistance;
							}
						}
					}
				}
			}
		}

	}

	return pBestPlot;
}






CvAIOperationPillageEnemy::CvAIOperationPillageEnemy()
{
}


CvAIOperationPillageEnemy::~CvAIOperationPillageEnemy()
{
}


int CvAIOperationPillageEnemy::GetDeployRange() const
{
	return GC.getAI_OPERATIONAL_PILLAGE_ENEMY_DEPLOY_RANGE();
}


void CvAIOperationPillageEnemy::Read(FDataStream& kStream)
{

	CvAIOperation::Read(kStream);


	uint uiVersion;
	kStream >> uiVersion;
}


void CvAIOperationPillageEnemy::Write(FDataStream& kStream) const
{

	CvAIOperation::Write(kStream);


	uint uiVersion = 1;
	kStream << uiVersion;
}


bool CvAIOperationPillageEnemy::ArmyMoved(CvArmyAI* pArmy)
{
	bool bStateChanged = false;
	CvPlot* pBetterTarget;

	switch(m_eCurrentState)
	{
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{

		pBetterTarget = FindBestTarget();


		if(pBetterTarget == NULL)
		{
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_NO_TARGET;
		}


		else if(pBetterTarget != GetTargetPlot())
		{
			SetTargetPlot(pBetterTarget);
			pArmy->SetGoalPlot(pBetterTarget);


			CvPlot* pDeployPt;
			pDeployPt = GC.getStepFinder().GetXPlotsFromEnd(GetOwner(), GetEnemy(), pArmy->Plot(), pBetterTarget, GC.getAI_OPERATIONAL_PILLAGE_ENEMY_DEPLOY_RANGE(), false);
			pArmy->SetGoalPlot(pDeployPt);
		}
	}
	break;


	case AI_OPERATION_STATE_AT_TARGET:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_GATHERING_FORCES:
	case AI_OPERATION_STATE_ABORTED:
		return CvAIOperation::ArmyMoved(pArmy);
		break;
	};

	return bStateChanged;
}


bool CvAIOperationPillageEnemy::ArmyInPosition(CvArmyAI* pArmy)
{
	bool bStateChanged = false;

	switch(m_eCurrentState)
	{

	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{
		if(pArmy->Plot() == pArmy->GetGoalPlot())
		{
			m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
		}
	}
	break;


	case AI_OPERATION_STATE_GATHERING_FORCES:
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}


CvPlot* CvAIOperationPillageEnemy::FindBestTarget()
{
	CvCity* pBestTargetCity = NULL;
	int iValue;
	int iBestValue;
	CvCity* pLoopCity;
	CvCity* pStartCity;
	int iDistance;
	int iLoop;

	CvPlayerAI& kEnemyPlayer = GET_PLAYER(m_eEnemy);

	if(!kEnemyPlayer.isAlive())
	{
		return NULL;
	}

	iBestValue = 0;
	pStartCity = GetOperationStartCity();
	if(pStartCity != NULL)
	{
		for(pLoopCity = kEnemyPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kEnemyPlayer.nextCity(&iLoop))
		{

			if(pLoopCity->getArea() == pStartCity->getArea())
			{

				iValue = pLoopCity->countNumImprovedPlots();


				iDistance = GC.getStepFinder().GetStepDistanceBetweenPoints(m_eOwner, m_eEnemy, pLoopCity->plot(), pStartCity->plot());
				if(iDistance > 0)
				{
					iValue = iValue * 100 / iDistance;
				}

				if(iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestTargetCity = pLoopCity;
				}
			}
		}
	}

	if(pBestTargetCity == NULL)
	{
		return NULL;
	}
	else
	{
		return pBestTargetCity->plot();
	}
}




CvAIEscortedOperation::CvAIEscortedOperation()
{
	m_bEscorted = true;
	m_iTargetArea = -1;
#ifdef AUI_WARNING_FIXES
	m_eCivilianType = NO_UNITAI;
#endif
}

CvAIEscortedOperation::~CvAIEscortedOperation()
{
}


void CvAIEscortedOperation::Init(int iID, PlayerTypes eOwner, PlayerTypes             , int                   , CvCity*            , CvCity*            )
{
	CvUnit* pOurCivilian;
	CvPlot* pTargetSite, *pNewTarget;

	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_SINGLE_HEX;
	m_iID = iID;
	m_eOwner = eOwner;


	pOurCivilian = FindBestCivilian();

	if(pOurCivilian != NULL && iID != -1)
	{

		pTargetSite = FindBestTarget(pOurCivilian, false);

		if(pTargetSite != NULL)
		{
			SetTargetPlot(pTargetSite);


			CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
			if(pArmyAI)
			{
				m_viArmyIDs.push_back(pArmyAI->GetID());
				pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
				pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
				pArmyAI->SetFormationIndex(GetFormation());


				pArmyAI->SetGoalPlot(pTargetSite);
				CvPlot* pMusterPt = pOurCivilian->plot();
				SetMusterPlot(pMusterPt);
				pArmyAI->SetXY(pMusterPt->getX(), pMusterPt->getY());
				SetDefaultArea(pMusterPt->getArea());


				pArmyAI->AddUnit(pOurCivilian->GetID(), 0);


				if (pTargetSite->getArea() != pOurCivilian->getArea())
				{
					m_bEscorted = false;
				}

				if (m_bEscorted)
				{
					m_viListOfUnitsWeStillNeedToBuild.clear();
					OperationSlot thisOperationSlot;
					thisOperationSlot.m_iOperationID = m_iID;
					thisOperationSlot.m_iArmyID = pArmyAI->GetID();
					thisOperationSlot.m_iSlotID = 1;
					m_viListOfUnitsWeStillNeedToBuild.push_back(thisOperationSlot);


					GrabUnitsFromTheReserves(pMusterPt, pTargetSite);
					if(pArmyAI->GetNumSlotsFilled() > 1)
					{
						pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
						m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
					}
					else
					{
						m_bEscorted = false;
					}
				}

				if (!m_bEscorted)
				{

					pNewTarget = FindBestTarget(pOurCivilian, true);


					if(pNewTarget == NULL)
					{
						m_bEscorted = true;


						m_viListOfUnitsWeStillNeedToBuild.clear();
						OperationSlot thisOperationSlot2;
						thisOperationSlot2.m_iOperationID = m_iID;
						thisOperationSlot2.m_iArmyID = pArmyAI->GetID();
						thisOperationSlot2.m_iSlotID = 1;
						m_viListOfUnitsWeStillNeedToBuild.push_back(thisOperationSlot2);
						m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
					}


					else
					{

						m_viListOfUnitsWeStillNeedToBuild.clear();


						pArmyAI->SetGoalPlot(pNewTarget);
						SetMusterPlot(pOurCivilian->plot());
						pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());


						pArmyAI->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
						m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
					}
				}
				LogOperationStart();
			}
		}

		else
		{

			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_LOST_TARGET;
		}
	}
}


void CvAIEscortedOperation::Read(FDataStream& kStream)
{

	CvAIOperation::Read(kStream);


	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_bEscorted;
	kStream >> m_eCivilianType;
	m_iTargetArea = -1;
	kStream >> m_iTargetArea;
}


void CvAIEscortedOperation::Write(FDataStream& kStream) const
{

	CvAIOperation::Write(kStream);


	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << m_bEscorted;
	kStream << m_eCivilianType;
	kStream << m_iTargetArea;
}


void CvAIEscortedOperation::UnitWasRemoved(int            , int iSlotID)
{

	if(iSlotID == 0)
	{
		m_eCurrentState = AI_OPERATION_STATE_ABORTED;
		m_eAbortReason = AI_ABORT_LOST_CIVILIAN;
	}
	else
	{
		m_bEscorted = false;
	}
}


CvUnit* CvAIEscortedOperation::FindBestCivilian()
{
	int iUnitLoop;
	CvUnit* pLoopUnit;

	for(pLoopUnit = GET_PLAYER(m_eOwner).firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(m_eOwner).nextUnit(&iUnitLoop))
	{
		if(pLoopUnit != NULL)
		{
			if(pLoopUnit->AI_getUnitAIType() == m_eCivilianType)
			{
				if(pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX)
				{
					return pLoopUnit;
				}
			}
		}
	}

	return NULL;
}


bool CvAIEscortedOperation::RetargetCivilian(CvUnit* pCivilian, CvArmyAI* pArmy)
{
	CvPlot* pBetterTarget;


	pBetterTarget = FindBestTarget(pCivilian, !m_bEscorted);


	if(pBetterTarget == NULL)
	{
		m_eCurrentState = AI_OPERATION_STATE_ABORTED;
		m_eAbortReason = AI_ABORT_NO_TARGET;
		return false;
	}

	else if(pBetterTarget != GetTargetPlot())
	{
		SetTargetPlot(pBetterTarget);
		pArmy->SetGoalPlot(pBetterTarget);
	}
	else
	{
		SetToAbort(AI_ABORT_REPEAT_TARGET);
		return false;
	}

	pArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
	m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;

	return true;
}






CvAIOperationFoundCity::CvAIOperationFoundCity()
{
	m_eCivilianType = UNITAI_SETTLE;
}


CvAIOperationFoundCity::~CvAIOperationFoundCity()
{
}


void CvAIOperationFoundCity::Init(int iID, PlayerTypes eOwner, PlayerTypes           , int iDefaultArea, CvCity*            , CvCity*            )
{
	m_iTargetArea = iDefaultArea;
	CvUnit* pOurCivilian;
	CvPlot* pTargetSite = NULL;
	CvPlot* pNewTarget = NULL;

	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_SINGLE_HEX;
	m_iID = iID;
	m_eOwner = eOwner;


	pOurCivilian = FindBestCivilian();

	if(pOurCivilian != NULL && iID != -1)
	{

		pTargetSite = FindBestTarget(pOurCivilian, false);

		if(pTargetSite != NULL)
		{
			SetTargetPlot(pTargetSite);


			CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
			if(pArmyAI)
			{
				m_viArmyIDs.push_back(pArmyAI->GetID());
				pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
				pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
				pArmyAI->SetFormationIndex(GetFormation());


				pArmyAI->SetGoalPlot(pTargetSite);
				CvPlot* pMusterPt = pOurCivilian->plot();
				SetMusterPlot(pMusterPt);
				pArmyAI->SetXY(pMusterPt->getX(), pMusterPt->getY());
				SetDefaultArea(pMusterPt->getArea());


				pArmyAI->AddUnit(pOurCivilian->GetID(), 0);


				m_viListOfUnitsWeStillNeedToBuild.clear();
				OperationSlot thisOperationSlot;
				thisOperationSlot.m_iOperationID = m_iID;
				thisOperationSlot.m_iArmyID = pArmyAI->GetID();
				thisOperationSlot.m_iSlotID = 1;
				m_viListOfUnitsWeStillNeedToBuild.push_back(thisOperationSlot);


				GrabUnitsFromTheReserves(pMusterPt, pTargetSite);
				if(pArmyAI->GetNumSlotsFilled() > 1)
				{
					pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
					m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
				}
				else
				{


					if (eOwner == -1 || GET_PLAYER(eOwner).getNumCities() > 1 || GET_PLAYER(eOwner).GetDiplomacyAI()->GetBoldness() > 5)
					{
						pNewTarget = FindBestTarget(pOurCivilian, true);
					}


					if(pNewTarget == NULL)
					{

						m_viListOfUnitsWeStillNeedToBuild.clear();
						OperationSlot thisOperationSlot2;
						thisOperationSlot2.m_iOperationID = m_iID;
						thisOperationSlot2.m_iArmyID = pArmyAI->GetID();
						thisOperationSlot2.m_iSlotID = 1;
						m_viListOfUnitsWeStillNeedToBuild.push_back(thisOperationSlot2);
						m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
					}


					else
					{
						m_bEscorted = false;


						m_viListOfUnitsWeStillNeedToBuild.clear();


						pArmyAI->SetGoalPlot(pNewTarget);
						SetMusterPlot(pOurCivilian->plot());
						pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());


						pArmyAI->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
						m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
					}
				}
				LogOperationStart();
			}
		}

		else
		{

			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_LOST_TARGET;
		}
	}
}



bool CvAIOperationFoundCity::ArmyInPosition(CvArmyAI* pArmy)
{
	int iUnitID = 0;
	bool bStateChanged = false;
	CvUnit* pSettler = 0, *pEscort = 0;
	CvString strMsg;

	switch(m_eCurrentState)
	{


	case AI_OPERATION_STATE_GATHERING_FORCES:


		if(!m_bEscorted)
		{
			return CvAIOperation::ArmyInPosition(pArmy);
		}


		else
		{
			iUnitID = pArmy->GetFirstUnitID();
			if(iUnitID != -1)
			{
				pSettler = GET_PLAYER(m_eOwner).getUnit(iUnitID);
			}
			iUnitID = pArmy->GetNextUnitID();
			if(iUnitID != -1)
			{
				pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
			}
			else
			{

				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_ESCORT_DIED;
				return true;
			}
			if(pSettler != NULL && pEscort != NULL && pSettler->plot() == pEscort->plot())
			{

				CvPlot* pBetterTarget = FindBestTarget(pSettler, true);


				if(pBetterTarget == NULL)
				{
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_NO_TARGET;
					return false;
				}

				else
				{
					SetTargetPlot(pBetterTarget);
					pArmy->SetGoalPlot(pBetterTarget);
				}
				return CvAIOperation::ArmyInPosition(pArmy);
			}
		}
		break;

	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	case AI_OPERATION_STATE_AT_TARGET:


		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);


		iUnitID = pArmy->GetFirstUnitID();
		if(iUnitID != -1)
		{
			pSettler = GET_PLAYER(m_eOwner).getUnit(iUnitID);
		}

		if(pSettler != NULL)
		{
			if((GetTargetPlot()->getOwner() != NO_PLAYER && GetTargetPlot()->getOwner() != m_eOwner) || GetTargetPlot()->IsAdjacentOwnedByOtherTeam(pSettler->getTeam()))
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("Now at target but can no longer settle here. Target was (X=%d Y=%d)", GetTargetPlot()->getX(), GetTargetPlot()->getY());
					LogOperationSpecialMessage(strMsg);
				}
				RetargetCivilian(pSettler, pArmy);
				pSettler->finishMoves();
				iUnitID = pArmy->GetNextUnitID();
				if(iUnitID != -1)
				{
					pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
					pEscort->finishMoves();
				}
			}

			else if(pSettler->plot() == GetTargetPlot() && pSettler->canMove() && pSettler->canFound(pSettler->plot()))
			{
				CvPlot* pCityPlot = pSettler->plot();
				int iPlotValue = GC.getGame().GetSettlerSiteEvaluator()->PlotFoundValue(pCityPlot, &GET_PLAYER(m_eOwner), NO_YIELD, false);

				pSettler->PushMission(CvTypes::getMISSION_FOUND());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvArea* pArea = pCityPlot->area();
					CvCity* pCity = pCityPlot->getPlotCity();

					if (pCity != NULL)
					{
						strMsg.Format("City founded, At X=%d, At Y=%d, %s, %d, %d", pCityPlot->getX(), pCityPlot->getY(), pCity->getName().GetCString(), iPlotValue, pArea->getTotalFoundValue());
						LogOperationSpecialMessage(strMsg);
					}
				}
				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			}



			else if(pSettler->plot() == GetTargetPlot() && !pSettler->canFound(pSettler->plot()))
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("At target but can no longer settle here. Target was (X=%d Y=%d)", GetTargetPlot()->getX(), GetTargetPlot()->getY());
					LogOperationSpecialMessage(strMsg);
				}
				RetargetCivilian(pSettler, pArmy);
				pSettler->finishMoves();
				iUnitID = pArmy->GetNextUnitID();
				if(iUnitID != -1)
				{
					pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
					pEscort->finishMoves();
				}
			}
		}
		break;


	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}


CvPlot* CvAIOperationFoundCity::FindBestTarget(CvUnit* pUnit, bool bOnlySafePaths)
{
	CvPlot* pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, bOnlySafePaths                , m_iTargetArea);
	if (pResult == NULL)
	{
		m_iTargetArea = -1;
		pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, bOnlySafePaths                , -1);
	}
	return pResult;
}



bool CvAIOperationFoundCity::ShouldAbort()
{

	bool rtnValue = CvAIOperation::ShouldAbort();

	if(!rtnValue)
	{
		if(m_eCurrentState == AI_OPERATION_STATE_RECRUITING_UNITS && m_bEscorted)
		{
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[0]);
			CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(0);

			if (GetMusterPlot()->getOwner() != m_eOwner || GC.getGame().getGameTurn() - pSlot->GetTurnAtCheckpoint() > 15)
			{
				if (m_viListOfUnitsCitiesHaveCommittedToBuild.size() == 0)
				{

					GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot());
					if(pThisArmy->GetNumSlotsFilled() > 1)
					{
						pThisArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
						m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
					}
					else
					{

						m_bEscorted = false;

						m_viListOfUnitsWeStillNeedToBuild.clear();

						pThisArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
						m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
					}
				}
			}
		}
		else if (m_eCurrentState == AI_OPERATION_STATE_GATHERING_FORCES && m_bEscorted)
		{
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[0]);
			CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(0);
			if (pSlot->GetUnitID() != NO_UNIT)
			{
				UnitHandle pUnit = GET_PLAYER(m_eOwner).getUnit(pSlot->GetUnitID());
				if (pUnit->plot()->isWater())
				{

					m_bEscorted = false;
					pThisArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
					m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
				}
			}
		}
	}

	return rtnValue;
}







CvAIOperationQuickColonize::CvAIOperationQuickColonize()
{
}


CvAIOperationQuickColonize::~CvAIOperationQuickColonize()
{
}


void CvAIOperationQuickColonize::Init(int iID, PlayerTypes eOwner, PlayerTypes           , int iDefaultArea, CvCity*            , CvCity*            )
{
	CvUnit* pOurCivilian;
	CvPlot* pTargetSite;

	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_SINGLE_HEX;
	m_iID = iID;
	m_eOwner = eOwner;
	m_iTargetArea = iDefaultArea;


	pOurCivilian = FindBestCivilian();

	if(pOurCivilian != NULL && iID != -1)
	{

		pTargetSite = FindBestTarget(pOurCivilian, false);

		if(pTargetSite != NULL)
		{
			SetTargetPlot(pTargetSite);

			CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
			if(pArmyAI)
			{
				m_viArmyIDs.push_back(pArmyAI->GetID());
				pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
				pArmyAI->SetFormationIndex(GetFormation());


				pArmyAI->SetGoalPlot(pTargetSite);
				CvPlot* pMusterPt = pOurCivilian->plot();
				SetMusterPlot(pMusterPt);
				pArmyAI->SetXY(pMusterPt->getX(), pMusterPt->getY());
				pArmyAI->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
				SetDefaultArea(pMusterPt->getArea());


				pArmyAI->AddUnit(pOurCivilian->GetID(), 0);
				m_bEscorted = false;

				m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
				LogOperationStart();
			}
		}

		else
		{

			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_LOST_TARGET;
		}
	}
}


CvUnit* CvAIOperationQuickColonize::FindBestCivilian()
{
	int iUnitLoop;
	CvUnit* pLoopUnit;

	for(pLoopUnit = GET_PLAYER(m_eOwner).firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(m_eOwner).nextUnit(&iUnitLoop))
	{
		if(pLoopUnit != NULL)
		{
			if(pLoopUnit->AI_getUnitAIType() == m_eCivilianType)
			{
				if(pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX)
				{
					return pLoopUnit;
				}
			}
		}
	}
	return NULL;
}


CvPlot* CvAIOperationQuickColonize::FindBestTarget(CvUnit* pUnit, bool                   )
{
	CvPlot* pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, false, m_iTargetArea);
	if (pResult == NULL)
	{
		m_iTargetArea = -1;
		pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, false, -1);
	}
	return pResult;
}






CvAIOperationMerchantDelegation::CvAIOperationMerchantDelegation()
{
	m_eCivilianType = UNITAI_MERCHANT;
}


CvAIOperationMerchantDelegation::~CvAIOperationMerchantDelegation()
{
}


bool CvAIOperationMerchantDelegation::ArmyInPosition(CvArmyAI* pArmy)
{
	int iUnitID = 0;
	bool bStateChanged = false;
	CvUnit* pMerchant = 0, *pEscort = 0;
	CvString strMsg;

	switch(m_eCurrentState)
	{


	case AI_OPERATION_STATE_GATHERING_FORCES:


		if(!m_bEscorted)
		{
			return CvAIOperation::ArmyInPosition(pArmy);
		}


		else
		{
			iUnitID = pArmy->GetFirstUnitID();
			if(iUnitID != -1)
			{
				pMerchant = GET_PLAYER(m_eOwner).getUnit(iUnitID);
			}
			iUnitID = pArmy->GetNextUnitID();
			if(iUnitID != -1)
			{
				pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
			}
			else
			{

				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_ESCORT_DIED;
				return true;
			}
			if(pMerchant != NULL && pEscort != NULL && pMerchant->plot() == pEscort->plot())
			{

				CvPlot* pBetterTarget = FindBestTarget(pMerchant, true);


				if(pBetterTarget == NULL)
				{
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_NO_TARGET;
					return false;
				}

				else
				{
					SetTargetPlot(pBetterTarget);
					pArmy->SetGoalPlot(pBetterTarget);
				}
				return CvAIOperation::ArmyInPosition(pArmy);
			}
		}
		break;

	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	case AI_OPERATION_STATE_AT_TARGET:


		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);


		iUnitID = pArmy->GetFirstUnitID();
		if(iUnitID != -1)
		{
			pMerchant = GET_PLAYER(m_eOwner).getUnit(iUnitID);
		}

		if(pMerchant != NULL)
		{

			if(pMerchant->plot() == GetTargetPlot() && pMerchant->canMove() && pMerchant->canTrade(pMerchant->plot()))
			{
				if (pMerchant->canBuyCityState(pMerchant->plot()) && !GET_PLAYER(m_eOwner).GreatMerchantWantsCash())
				{
					pMerchant->PushMission(CvTypes::getMISSION_BUY_CITY_STATE());
					if(GC.getLogging() && GC.getAILogging())
					{
						strMsg.Format("Great Merchant buying city-state, At X=%d, At Y=%d", pMerchant->plot()->getX(), pMerchant->plot()->getY());
						LogOperationSpecialMessage(strMsg);
					}
				}
				else
				{
					pMerchant->PushMission(CvTypes::getMISSION_TRADE());
					if(GC.getLogging() && GC.getAILogging())
					{
						strMsg.Format("Great Merchant finishing trade mission, At X=%d, At Y=%d", pMerchant->plot()->getX(), pMerchant->plot()->getY());
						LogOperationSpecialMessage(strMsg);
					}
				}

				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			}


			else if(pMerchant->plot() == GetTargetPlot())
			{

				if(!pMerchant->canTrade(pMerchant->plot()))
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						strMsg.Format("At target but can no longer trade here. Target was (X=%d Y=%d)", GetTargetPlot()->getX(), GetTargetPlot()->getY());
						LogOperationSpecialMessage(strMsg);
					}
					RetargetCivilian(pMerchant, pArmy);
					pMerchant->finishMoves();
					iUnitID = pArmy->GetNextUnitID();
					if(iUnitID != -1)
					{
						pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
						pEscort->finishMoves();
					}
				}
			}
		}
		break;


	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}


CvPlot* CvAIOperationMerchantDelegation::FindBestTarget(CvUnit* pUnit, bool bOnlySafePaths)
{
	CvAssertMsg(pUnit, "pUnit cannot be null");
	if(!pUnit)
	{
		return NULL;
	}

	return GET_PLAYER(pUnit->getOwner()).FindBestMerchantTargetPlot(pUnit, !bOnlySafePaths                );
}






CvAIOperationConcertTour::CvAIOperationConcertTour()
{
	m_eCivilianType = UNITAI_MUSICIAN;
}


CvAIOperationConcertTour::~CvAIOperationConcertTour()
{
}


bool CvAIOperationConcertTour::ArmyInPosition(CvArmyAI* pArmy)
{
	int iUnitID = 0;
	bool bStateChanged = false;
	CvUnit* pMusician = 0, *pEscort = 0;
	CvString strMsg;

	switch(m_eCurrentState)
	{


	case AI_OPERATION_STATE_GATHERING_FORCES:


		if(!m_bEscorted)
		{
			return CvAIOperation::ArmyInPosition(pArmy);
		}


		else
		{
			iUnitID = pArmy->GetFirstUnitID();
			if(iUnitID != -1)
			{
				pMusician = GET_PLAYER(m_eOwner).getUnit(iUnitID);
			}
			iUnitID = pArmy->GetNextUnitID();
			if(iUnitID != -1)
			{
				pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
			}
			else
			{

				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_ESCORT_DIED;
				return true;
			}
			if(pMusician != NULL && pEscort != NULL && pMusician->plot() == pEscort->plot())
			{

				CvPlot* pBetterTarget = FindBestTarget(pMusician, true);


				if(pBetterTarget == NULL)
				{
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_NO_TARGET;
					return false;
				}

				else
				{
					SetTargetPlot(pBetterTarget);
					pArmy->SetGoalPlot(pBetterTarget);
				}
				return CvAIOperation::ArmyInPosition(pArmy);
			}
		}
		break;

	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	case AI_OPERATION_STATE_AT_TARGET:


		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);


		iUnitID = pArmy->GetFirstUnitID();
		if(iUnitID != -1)
		{
			pMusician = GET_PLAYER(m_eOwner).getUnit(iUnitID);
		}

		if(pMusician != NULL)
		{

			if(pMusician->plot() == GetTargetPlot() && pMusician->canMove() && pMusician->canBlastTourism(pMusician->plot()))
			{
				pMusician->PushMission(CvTypes::getMISSION_ONE_SHOT_TOURISM());
				if(GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("Great Musician performing concert tour, At X=%d, At Y=%d", pMusician->plot()->getX(), pMusician->plot()->getY());
					LogOperationSpecialMessage(strMsg);
				}

				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			}


			else if(pMusician->plot() == GetTargetPlot())
			{

				if(!pMusician->canBlastTourism(pMusician->plot()))
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						strMsg.Format("At target but can no longer hold concert here. Target was (X=%d Y=%d)", GetTargetPlot()->getX(), GetTargetPlot()->getY());
						LogOperationSpecialMessage(strMsg);
					}
					RetargetCivilian(pMusician, pArmy);
					pMusician->finishMoves();
					iUnitID = pArmy->GetNextUnitID();
					if(iUnitID != -1)
					{
						pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
						pEscort->finishMoves();
					}
				}
			}
		}
		break;


	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}


CvPlot* CvAIOperationConcertTour::FindBestTarget(CvUnit* pUnit, bool bOnlySafePaths)
{
	CvAssertMsg(pUnit, "pUnit cannot be null");
	if(!pUnit)
	{
		return NULL;
	}

	return GET_PLAYER(pUnit->getOwner()).FindBestMusicianTargetPlot(pUnit, !bOnlySafePaths                );
}






CvAINavalOperation::CvAINavalOperation()
{
}


CvAINavalOperation::~CvAINavalOperation()
{
}


void CvAINavalOperation::Read(FDataStream& kStream)
{

	CvAIOperation::Read(kStream);


	uint uiVersion;
	kStream >> uiVersion;
}


void CvAINavalOperation::Write(FDataStream& kStream) const
{

	CvAIOperation::Write(kStream);


	uint uiVersion = 1;
	kStream << uiVersion;
}


int CvAINavalOperation::GetDeployRange() const
{
	return GC.getAI_OPERATIONAL_NAVAL_BOMBARDMENT_DEPLOY_RANGE();
}


CvCity* CvAINavalOperation::GetOperationStartCity() const
{
	if(GetStartCityPlot())
	{
		return GetStartCityPlot()->getPlotCity();
	}

	int iLoop;
	CvCity* pCity;


	for(pCity = GET_PLAYER(m_eOwner).firstCity(&iLoop); pCity != NULL; pCity = GET_PLAYER(m_eOwner).nextCity(&iLoop))
	{
		if(pCity->isCoastal())
		{
			return pCity;
		}
	}

	return NULL;
}


CvPlot* CvAINavalOperation::SelectInitialMusterPoint(CvArmyAI* pThisArmy)
{
	CvPlot* pMusterPt = NULL;
	CvPlot* pStartCityPlot;
	CvCity* pStartCity;
	CvPlot* pCurrentPlot;
	CvPlot* pAdjacentPlot;
	int iSpacesFromTarget = 0;
	int iDirectionLoop;
	CvAStarNode* pNode;
	bool bMusterPointFound = false;

	pStartCity = GetOperationStartCity();
	if(pStartCity)
	{
		pStartCityPlot = pStartCity->plot();

		if(pStartCityPlot != NULL)
		{

			for(iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES && !bMusterPointFound; ++iDirectionLoop)
			{
				pAdjacentPlot = plotDirection(pStartCityPlot->getX(), pStartCityPlot->getY(), ((DirectionTypes)iDirectionLoop));
				if(pAdjacentPlot != NULL && pAdjacentPlot->isWater())
				{

					GC.getStepFinder().SetData(&m_eEnemy);
					if(GC.getStepFinder().GeneratePath(pAdjacentPlot->getX(), pAdjacentPlot->getY(), pThisArmy->GetGoalPlot()->getX(), pThisArmy->GetGoalPlot()->getY(), m_eOwner, false))
					{
						pNode = GC.getStepFinder().GetLastNode();


						while(pNode != NULL)
						{
							pCurrentPlot = GC.getMap().plotCheckInvalid(pNode->m_iX, pNode->m_iY);


							if(iSpacesFromTarget == GetDeployRange())
							{
								pMusterPt = pCurrentPlot;
								bMusterPointFound = true;
								break;
							}


							iSpacesFromTarget++;
							pNode = pNode->m_pParent;
						}
					}
				}
			}
		}
	}

	if(pMusterPt != NULL)
	{
		SetMusterPlot(pMusterPt);
	}
	else
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString szMsg;
			szMsg.Format("No muster point found, Naval operation aborting, Target was, X: %d, Y: %d", pThisArmy->GetGoalPlot()->getX(), pThisArmy->GetGoalPlot()->getY());
			LogOperationSpecialMessage(szMsg);
		}
	}

	return pMusterPt;
}


CvUnit* CvAINavalOperation::FindInitialUnit()
{
	int iUnitLoop;
	CvUnit* pLoopUnit;

	for(pLoopUnit = GET_PLAYER(m_eOwner).firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(m_eOwner).nextUnit(&iUnitLoop))
	{
		if(pLoopUnit != NULL)
		{
			if(pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE_SEA)
			{
				CvUnitEntry* pkUnitEntry = GC.getUnitInfo(pLoopUnit->getUnitType());
				if(pkUnitEntry && pkUnitEntry->GetUnitAIType(UNITAI_ATTACK_SEA))
				{
					if(pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX)
					{
						return pLoopUnit;
					}
				}
			}
		}
	}
	return NULL;
}






CvAIOperationNavalBombardment::CvAIOperationNavalBombardment()
{
}


CvAIOperationNavalBombardment::~CvAIOperationNavalBombardment()
{
}


void CvAIOperationNavalBombardment::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int                 , CvCity*            , CvCity*            )
{
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_ENEMY_TERRITORY;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;

	if(iID != -1 && GetOperationStartCity())
	{

		CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
		CvArmyAI* pArmyAI = kPlayer.addArmyAI();
		if(pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());


			CvPlot* pTargetPlot = FindBestTarget();
			if(pTargetPlot != NULL)
			{
				SetTargetPlot(pTargetPlot);
				pArmyAI->SetGoalPlot(pTargetPlot);
				if(SelectInitialMusterPoint(pArmyAI) != NULL)
				{
					pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
					SetDefaultArea(GetMusterPlot()->getArea());


					BuildListOfUnitsWeStillNeedToBuild();


					if(GrabUnitsFromTheReserves(GetMusterPlot(), GetMusterPlot()))
					{
						pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
						m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
					}
					else
					{
						m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
					}

					LogOperationStart();
				}
				else
				{

					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_NO_MUSTER;
				}
			}

			else
			{

				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_LOST_TARGET;
			}
		}
	}
}


void CvAIOperationNavalBombardment::Read(FDataStream& kStream)
{

	CvAINavalOperation::Read(kStream);


	uint uiVersion;
	kStream >> uiVersion;
}


void CvAIOperationNavalBombardment::Write(FDataStream& kStream) const
{

	CvAINavalOperation::Write(kStream);


	uint uiVersion = 1;
	kStream << uiVersion;
}


bool CvAIOperationNavalBombardment::ArmyInPosition(CvArmyAI* pArmy)
{
	bool bStateChanged = false;

	switch(m_eCurrentState)
	{

	case AI_OPERATION_STATE_GATHERING_FORCES:
	{

		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);


		CvPlot* possibleBetterTarget;
		possibleBetterTarget = FindBestTarget();


		if(possibleBetterTarget == NULL)
		{
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_NO_TARGET;
		}


		else if(possibleBetterTarget != GetTargetPlot())
		{

			CvPlot* pDeployPt = GC.getStepFinder().GetXPlotsFromEnd(GetOwner(), GetEnemy(), pArmy->Plot(), possibleBetterTarget, GetDeployRange(), false);
			if(pDeployPt != NULL)
			{
				pArmy->SetGoalPlot(pDeployPt);
				SetTargetPlot(possibleBetterTarget);
			}
		}
	}
	break;


	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{
		if(pArmy->Plot() == pArmy->GetGoalPlot())
		{

			CvTemporaryZone zone;
			zone.SetX(GetTargetPlot()->getX());
			zone.SetY(GetTargetPlot()->getY());
			zone.SetTargetType(AI_TACTICAL_TARGET_BOMBARDMENT_ZONE);
			zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_BOMBARDMENT_ZONE_TURNS());
			GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);

			m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
		}
	}
	break;


	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}


CvPlot* CvAIOperationNavalBombardment::FindBestTarget()
{
#ifdef AUI_WARNING_FIXES
	uint iPlotLoop;
	int iDirectionLoop;
#else
	int iPlotLoop, iDirectionLoop;
#endif
	CvPlot* pBestPlot = NULL;
	CvPlot* pPlot;
	CvPlot* pAdjacentPlot;
	int iBestTurns = MAX_INT;
	int iCurrentTurns;
	CvUnit* pInitialUnit;

	CvPlayer& owningPlayer = GET_PLAYER(m_eOwner);

	if(GetFirstArmyID() == -1)
	{
		pInitialUnit = FindInitialUnit();
	}
	else
	{
		CvArmyAI* pThisArmy = owningPlayer.getArmyAI(m_viArmyIDs[0]);
		int iUnitID = pThisArmy->GetFirstUnitID();
		if(iUnitID != -1)
		{
			pInitialUnit = owningPlayer.getUnit(iUnitID);
		}
		else
		{
			pInitialUnit = FindInitialUnit();
		}
	}

	if(pInitialUnit != NULL)
	{

		for(iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
		{
			pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

			if(pPlot->isRevealed(owningPlayer.getTeam()))
			{
				if(pPlot->isCoastalLand())
				{

					CvUnit* pUnit = pPlot->getVisibleEnemyDefender(m_eOwner);
					if(pUnit)
					{

						for(iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
						{
							pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iDirectionLoop));
							if(pAdjacentPlot != NULL && pAdjacentPlot->isWater() && pAdjacentPlot->isShallowWater())
							{
								if(pInitialUnit->GeneratePath(pAdjacentPlot, 0, false, &iCurrentTurns))
								{
									if(iCurrentTurns < iBestTurns)
									{
										iBestTurns = iCurrentTurns;
										pBestPlot = pAdjacentPlot;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return pBestPlot;
}






CvAIOperationNavalSuperiority::CvAIOperationNavalSuperiority()
{
}


CvAIOperationNavalSuperiority::~CvAIOperationNavalSuperiority()
{
}


void CvAIOperationNavalSuperiority::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int                 , CvCity*            , CvCity*            )
{
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_FREEFORM_NAVAL;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;

	if(iID != -1 && GetOperationStartCity())
	{

		CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
		CvArmyAI* pArmyAI = kPlayer.addArmyAI();
		if(pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());


			CvPlot* pTargetPlot = FindBestTarget();
			if(pTargetPlot != NULL)
			{
				SetTargetPlot(pTargetPlot);
				pArmyAI->SetGoalPlot(pTargetPlot);
				if(SelectInitialMusterPoint(pArmyAI) != NULL)
				{
					pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
					SetDefaultArea(GetMusterPlot()->getArea());


					BuildListOfUnitsWeStillNeedToBuild();


					if(GrabUnitsFromTheReserves(GetMusterPlot(), GetMusterPlot()))
					{
						pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
						m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
					}
					else
					{
						m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
					}

					LogOperationStart();
				}
				else
				{

					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_NO_MUSTER;
				}
			}

			else
			{

				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_LOST_TARGET;
			}
		}
	}
}


void CvAIOperationNavalSuperiority::Read(FDataStream& kStream)
{

	CvAINavalOperation::Read(kStream);


	uint uiVersion;
	kStream >> uiVersion;
}


void CvAIOperationNavalSuperiority::Write(FDataStream& kStream) const
{

	CvAINavalOperation::Write(kStream);


	uint uiVersion = 1;
	kStream << uiVersion;
}


bool CvAIOperationNavalSuperiority::ArmyInPosition(CvArmyAI* pArmy)
{
	bool bStateChanged = false;

	switch(m_eCurrentState)
	{

	case AI_OPERATION_STATE_GATHERING_FORCES:
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{

		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);


		CvPlot* possibleBetterTarget;
		possibleBetterTarget = FindBestTarget();


		if(possibleBetterTarget == NULL)
		{
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_NO_TARGET;
		}


		else if(possibleBetterTarget != GetTargetPlot())
		{

			CvPlot* pDeployPt = GC.getStepFinder().GetXPlotsFromEnd(GetOwner(), GetEnemy(), pArmy->Plot(), possibleBetterTarget, GetDeployRange(), false);
			if(pDeployPt != NULL)
			{
				pArmy->SetGoalPlot(pDeployPt);
				SetTargetPlot(possibleBetterTarget);
			}
		}
	}
	break;


	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}

typedef CvWeightedVector<CvPlot*, 1, true> WeightedPlotVector;




static CvPlot* GetReachablePlot(UnitHandle pUnit, WeightedPlotVector& aPlots, int *iTurns)
{
	CvPlot *pFoundPlot = NULL;
	int iFoundWeight = 0;
	int iFoundTurns = 0;
	uint uiListSize;	
	if ((uiListSize = aPlots.size()) > 0)
	{
		aPlots.SortItems();



		for (uint i = uiListSize; i--; )
		{
			CvPlot* pPlot = aPlots.GetElement(i);
			int iWeight = aPlots.GetWeight(i);

			if (pFoundPlot)
			{
				if (iWeight > iFoundWeight)
					break;
			
				int iTurnsCalculated = TurnsToReachTarget(pUnit, pPlot, true                , false);
				if (iTurnsCalculated != MAX_INT)
				{
					if (iTurnsCalculated < iFoundTurns)
					{
						iFoundWeight = iWeight;
						pFoundPlot = pPlot;
						iFoundTurns = iTurnsCalculated;
						if (iFoundTurns == 1)
							break;
					}
				}
			}
			else
			{
				int iTurnsCalculated = TurnsToReachTarget(pUnit, pPlot, true                , false);
				if (iTurnsCalculated != MAX_INT)
				{
					iFoundWeight = iWeight;
					pFoundPlot = pPlot;
					iFoundTurns = iTurnsCalculated;
					if (iFoundTurns == 1)
						break;
				}
			}
		}
	}

	if (pFoundPlot)
	{
		if (iTurns)
			*iTurns = iFoundTurns;
		return pFoundPlot;
	}

	return NULL;
}


CvPlot* CvAIOperationNavalSuperiority::FindBestTarget()
{
#ifdef AUI_WARNING_FIXES
	uint iPlotLoop, iUnitLoop;
#else
	int iPlotLoop, iUnitLoop;
#endif
	CvPlot* pPlot;
	CvPlot* pBestPlot = NULL;
	CvUnit* pInitialUnit;
	CvCity* pCity;
	CvCity* pEnemyCoastalCity = NULL;
	CvPlot* pCoastalBarbarianCamp = NULL;
	int iClosestEnemyDistance = MAX_INT;
	int iClosestCampDistance = MAX_INT;
	int iDistance;
	CvPlayer& owningPlayer = GET_PLAYER(m_eOwner);

	if(GetFirstArmyID() == -1)
	{
		pInitialUnit = FindInitialUnit();
	}
	else
	{
		CvArmyAI* pThisArmy = owningPlayer.getArmyAI(m_viArmyIDs[0]);
		int iUnitID = pThisArmy->GetFirstUnitID();
		if(iUnitID != -1)
		{
			pInitialUnit = owningPlayer.getUnit(iUnitID);
		}
		else
		{
			pInitialUnit = FindInitialUnit();
		}
	}

	if(pInitialUnit != NULL)
	{
		WeightedPlotVector aPlotList;
		aPlotList.reserve(64);

		int iUnitX = pInitialUnit->getX();
		int iUnitY = pInitialUnit->getY();
		int iBaseMoves = pInitialUnit->baseMoves();


		for(iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
		{
			pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

			if(pPlot->isRevealed(owningPlayer.getTeam()))
			{
				if(pPlot->isWater())
				{
					if(pPlot->getNumUnits() > 0)
					{
						int iPlotDistance = plotDistance(iUnitX, iUnitY, pPlot->getX(), pPlot->getY());
						for(iUnitLoop = 0; iUnitLoop < pPlot->getNumUnits(); iUnitLoop++)
						{
							CvUnit* pLoopUnit = pPlot->getUnitByIndex(iUnitLoop);
							{
								if(pLoopUnit->isEnemy(owningPlayer.getTeam()))
								{
									int iScore = iBaseMoves * iPlotDistance;
									if (pLoopUnit->isTrade())
									{
										iScore /= 3;
									}
									if (pLoopUnit->isEmbarked())
									{
										iScore = (iScore * 2) / 3;
									}

									aPlotList.push_back(pPlot, iScore);
								}
							}
						}
					}
				}


				else if(pPlot->isCity() && pPlot->isCoastalLand())
				{
					pCity = pPlot->getPlotCity();
					if(pCity)
					{
						if(atWar(owningPlayer.getTeam(), pCity->getTeam()))
						{
							iDistance = plotDistance(pInitialUnit->getX(), pInitialUnit->getY(), pCity->getX(), pCity->getY());
							if(iDistance < iClosestEnemyDistance)
							{
								iClosestEnemyDistance = iDistance;
								pEnemyCoastalCity = pCity;
							}
						}
					}
				}

				else if(pPlot->isCoastalLand() && pPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
				{
					iDistance = plotDistance(pInitialUnit->getX(), pInitialUnit->getY(), pPlot->getX(), pPlot->getY());
					if(iDistance < iClosestCampDistance)
					{
						iClosestCampDistance = iDistance;
						pCoastalBarbarianCamp = pPlot;
					}
				}
			}
		}

		int iBestTurns;
		pBestPlot = GetReachablePlot(pInitialUnit, aPlotList, &iBestTurns);


		if(pBestPlot == NULL)
		{
			if(pEnemyCoastalCity != NULL)
			{

				for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
				{
					CvPlot* pAdjacentPlot = plotDirection(pEnemyCoastalCity->plot()->getX(), pEnemyCoastalCity->plot()->getY(), ((DirectionTypes)iDirectionLoop));
					if(pAdjacentPlot != NULL && pAdjacentPlot->isWater() && pAdjacentPlot->isShallowWater())
					{
						if(pInitialUnit->GeneratePath(pAdjacentPlot))
						{
							pBestPlot = pAdjacentPlot;
						}
					}
				}
			}

			else
			{
				if(pCoastalBarbarianCamp != NULL)
				{

					for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
					{
						CvPlot* pAdjacentPlot = plotDirection(pCoastalBarbarianCamp->getX(), pCoastalBarbarianCamp->getY(), ((DirectionTypes)iDirectionLoop));
						if(pAdjacentPlot != NULL && pAdjacentPlot->isWater() && pAdjacentPlot->isShallowWater())
						{
							if(pInitialUnit->GeneratePath(pAdjacentPlot))
							{
								pBestPlot = pAdjacentPlot;
							}
						}
					}
				}
			}
		}
	}

	return pBestPlot;
}






CvAIOperationPureNavalCityAttack::CvAIOperationPureNavalCityAttack()
{
}


CvAIOperationPureNavalCityAttack::~CvAIOperationPureNavalCityAttack()
{
}


void CvAIOperationPureNavalCityAttack::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int                 , CvCity* pTarget, CvCity* pMuster)
{
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_ENEMY_TERRITORY;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;

	if(iID != -1)
	{

		CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
		CvArmyAI* pArmyAI = kPlayer.addArmyAI();
		if(pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());

			if(pTarget)
			{
				SetTargetPlot(pTarget->plot());
				pArmyAI->SetGoalPlot(GetTargetPlot());


				CvPlot *pCoastalMuster = kPlayer.GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMuster->plot(), pArmyAI);
				if (pCoastalMuster)
				{
					SetDefaultArea(pCoastalMuster->getArea());
					SetStartCityPlot(pCoastalMuster);
					SetMusterPlot(GetStartCityPlot());
					pArmyAI->SetXY(GetStartCityPlot()->getX(), GetStartCityPlot()->getY());


					BuildListOfUnitsWeStillNeedToBuild();


					if(GrabUnitsFromTheReserves(GetMusterPlot(), GetMusterPlot()))
					{
						pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
						m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
					}
					else
					{
						m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
					}

					LogOperationStart();
				}
				else
				{

					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_NO_MUSTER;
				}
			}

			else
			{

				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_LOST_TARGET;
			}
		}
	}
}


void CvAIOperationPureNavalCityAttack::Read(FDataStream& kStream)
{

	CvAINavalOperation::Read(kStream);


	uint uiVersion;
	kStream >> uiVersion;
}


void CvAIOperationPureNavalCityAttack::Write(FDataStream& kStream) const
{

	CvAINavalOperation::Write(kStream);


	uint uiVersion = 1;
	kStream << uiVersion;
}


int CvAIOperationPureNavalCityAttack::GetDeployRange() const
{
	return GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE();
}


bool CvAIOperationPureNavalCityAttack::ArmyInPosition(CvArmyAI* pArmy)
{
	bool bStateChanged = false;

	switch(m_eCurrentState)
	{

	case AI_OPERATION_STATE_GATHERING_FORCES:
		{

			bStateChanged = CvAIOperation::ArmyInPosition(pArmy);


			CvPlot* pTarget = GetTargetPlot();
			if(pTarget->getOwner() != m_eEnemy)
			{
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_TARGET_ALREADY_CAPTURED;
			}
		}
		break;


	case AI_OPERATION_STATE_MOVING_TO_TARGET:
		{
			if (plotDistance(pArmy->Plot()->getX(), pArmy->Plot()->getY(), GetTargetPlot()->getX(), GetTargetPlot()->getY()) < 2)
			{

				CvTemporaryZone zone;
				zone.SetX(GetTargetPlot()->getX());
				zone.SetY(GetTargetPlot()->getY());
				zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
				zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
				GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);

				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			}
		}
		break;


	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}


bool CvAIOperationPureNavalCityAttack::ShouldAbort()
{

	bool rtnValue = CvAIOperation::ShouldAbort();

	if(!rtnValue)
	{

		if(GetTargetPlot()->getOwner() != m_eEnemy)
		{

			return true;
		}
	}

	return rtnValue;
}


CvPlot* CvAIOperationPureNavalCityAttack::FindBestTarget()
{
	CvAssertMsg(false, "Obsolete function called CvAIOperationPureNavalCityAttack::FindBestTarget()");

	return NULL;
}






CvAIOperationCityCloseDefense::CvAIOperationCityCloseDefense()
{
}


CvAIOperationCityCloseDefense::~CvAIOperationCityCloseDefense()
{
}


void CvAIOperationCityCloseDefense::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int                 , CvCity*            , CvCity*            )
{
	Reset();
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;

	if(iID != -1)
	{

		CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
		if(pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());

			CvPlot* pTargetPlot = FindBestTarget();
			if(pTargetPlot != NULL)
			{
				SetTargetPlot(pTargetPlot);
				pArmyAI->SetGoalPlot(pTargetPlot);
				SetMusterPlot(pTargetPlot);
				pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
				SetDefaultArea(GetMusterPlot()->getArea());


				BuildListOfUnitsWeStillNeedToBuild();


				if(GrabUnitsFromTheReserves(GetMusterPlot(), NULL))
				{
					pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
					m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
				}
				else
				{
					m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
				}

				LogOperationStart();
			}
		}
	}
}


void CvAIOperationCityCloseDefense::Read(FDataStream& kStream)
{

	CvAIOperation::Read(kStream);


	uint uiVersion;
	kStream >> uiVersion;
}


void CvAIOperationCityCloseDefense::Write(FDataStream& kStream) const
{

	CvAIOperation::Write(kStream);


	uint uiVersion = 1;
	kStream << uiVersion;
}


CvPlot* CvAIOperationCityCloseDefense::FindBestTarget()
{
	CvCity* pCity;
	CvPlot* pPlot = NULL;
	CvCity* pEnemyCapital;
	int iLoop;


	pCity = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetMostThreatenedCity();


	if(pCity == NULL)
	{
		pEnemyCapital = GET_PLAYER(m_eEnemy).getCapitalCity();
		if(pEnemyCapital == NULL)
		{
			pEnemyCapital = GET_PLAYER(m_eEnemy).firstCity(&iLoop);
		}
		pCity = GC.getMap().findCity(pEnemyCapital->getX(), pEnemyCapital->getY(), m_eOwner, NO_TEAM, true, false, NO_TEAM, NO_DIRECTION, NULL);
	}

	if(pCity != NULL)
	{
		pPlot = pCity->plot();
	}

	return pPlot;
}






CvAIOperationRapidResponse::CvAIOperationRapidResponse()
{
}


CvAIOperationRapidResponse::~CvAIOperationRapidResponse()
{
}


void CvAIOperationRapidResponse::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int                 , CvCity*            , CvCity*            )
{
	Reset();
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;

	if(iID != -1)
	{

		CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
		if(pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());

			CvPlot* pTargetPlot = FindBestTarget();
			if(pTargetPlot != NULL)
			{
				SetTargetPlot(pTargetPlot);
				pArmyAI->SetGoalPlot(pTargetPlot);
				SetMusterPlot(pTargetPlot);
				pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
				SetDefaultArea(GetMusterPlot()->getArea());


				BuildListOfUnitsWeStillNeedToBuild();


				if(GrabUnitsFromTheReserves(GetMusterPlot(), NULL))
				{
					pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
					m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
				}
				else
				{
					m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
				}

				LogOperationStart();
			}
		}
	}
}


void CvAIOperationRapidResponse::Read(FDataStream& kStream)
{

	CvAIOperation::Read(kStream);


	uint uiVersion;
	kStream >> uiVersion;
}


void CvAIOperationRapidResponse::Write(FDataStream& kStream) const
{

	CvAIOperation::Write(kStream);


	uint uiVersion = 1;
	kStream << uiVersion;
}


bool CvAIOperationRapidResponse::ArmyInPosition(CvArmyAI* pArmy)
{
	bool bStateChanged = false;

	switch(m_eCurrentState)
	{

	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{

		bStateChanged = false;


		RetargetDefensiveArmy(pArmy);
	}
	break;


	case AI_OPERATION_STATE_GATHERING_FORCES:
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}


bool CvAIOperationRapidResponse::ArmyMoved(CvArmyAI* pArmy)
{
	bool bStateChanged = false;

	switch(m_eCurrentState)
	{
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{
		RetargetDefensiveArmy(pArmy);
	}
	break;


	case AI_OPERATION_STATE_AT_TARGET:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_GATHERING_FORCES:
	case AI_OPERATION_STATE_ABORTED:
		return CvAIOperation::ArmyMoved(pArmy);
		break;
	};

	return bStateChanged;
}


bool CvAIOperationRapidResponse::RetargetDefensiveArmy(CvArmyAI* pArmy)
{
	CvPlot* pBetterTarget;


	pBetterTarget = FindBestTarget();


	if(pBetterTarget == NULL)
	{
		m_eCurrentState = AI_OPERATION_STATE_ABORTED;
		m_eAbortReason = AI_ABORT_NO_TARGET;
		return false;
	}


	else if(pBetterTarget != GetTargetPlot())
	{
		SetTargetPlot(pBetterTarget);
		pArmy->SetGoalPlot(pBetterTarget);
	}

	pArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
	m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;

	return true;
}


CvPlot* CvAIOperationRapidResponse::FindBestTarget()
{
	CvCity* pCity;
	CvPlot* pPlot = NULL;
	CvCity* pEnemyCapital;
	int iLoop;


	pCity = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetMostThreatenedCity();


	if(pCity == NULL)
	{
		pEnemyCapital = GET_PLAYER(m_eEnemy).getCapitalCity();
		if(pEnemyCapital == NULL)
		{
			pEnemyCapital = GET_PLAYER(m_eEnemy).firstCity(&iLoop);
		}
		pCity = GC.getMap().findCity(pEnemyCapital->getX(), pEnemyCapital->getY(), m_eOwner, NO_TEAM, true, false, NO_TEAM, NO_DIRECTION, NULL);
	}

	if(pCity != NULL)
	{
		pPlot = pCity->plot();
	}

	return pPlot;
}




CvAINavalEscortedOperation::CvAINavalEscortedOperation()
{

	m_eCivilianType = UNITAI_SETTLE;
	m_iInitialAreaID = -1;
}

CvAINavalEscortedOperation::~CvAINavalEscortedOperation()
{
}


void CvAINavalEscortedOperation::Init(int iID, PlayerTypes eOwner, PlayerTypes           , int iDefaultArea, CvCity*            , CvCity*            )
{
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_NAVAL_ESCORT;
	m_iID = iID;
	m_eOwner = eOwner;
	SetDefaultArea(iDefaultArea);


	CvUnit* pOurCivilian = FindBestCivilian();
	m_iInitialAreaID = (pOurCivilian != NULL)? pOurCivilian->getArea() : -1;

	CvCity* pStartCity = GetOperationStartCity();

	if(pOurCivilian != NULL && iID != -1 && pStartCity)
	{

		CvPlot* pTargetSite = FindBestTarget(pOurCivilian);

		if(pTargetSite != NULL)
		{
			SetTargetPlot(pTargetSite);


			CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
			if(pArmyAI)
			{
				m_viArmyIDs.push_back(pArmyAI->GetID());
				pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
				pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
				pArmyAI->SetFormationIndex(GetFormation());

				pArmyAI->SetGoalPlot(pTargetSite);
				CvPlot* pMusterPt = pStartCity->plot();
				SetMusterPlot(pMusterPt);
				pArmyAI->SetXY(pMusterPt->getX(), pMusterPt->getY());


				pArmyAI->AddUnit(pOurCivilian->GetID(), 0);


				BuildListOfUnitsWeStillNeedToBuild();


				if(GrabUnitsFromTheReserves(GetMusterPlot(), NULL))
				{
					pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
					m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
				}
				else
				{
					m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
				}
				LogOperationStart();
			}
		}

		else
		{

			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_LOST_TARGET;
		}
	}
}


CvCity* CvAINavalEscortedOperation::GetOperationStartCity() const
{
	if(GetStartCityPlot())
	{
		return GetStartCityPlot()->getPlotCity();
	}

	CvPlayerAI& kOwner = GET_PLAYER(m_eOwner);

	int iLoop = 0;
	CvCity* pCity = NULL;


	for(pCity = kOwner.firstCity(&iLoop); pCity != NULL; pCity = kOwner.nextCity(&iLoop))
	{
		if(pCity->isCoastal())
		{
			if(pCity->getArea() == m_iInitialAreaID)
			{
				return pCity;
			}
		}
	}

	return NULL;
}


void CvAINavalEscortedOperation::UnitWasRemoved(int            , int iSlotID)
{

	if(iSlotID == 0)
	{
		m_eCurrentState = AI_OPERATION_STATE_ABORTED;
		m_eAbortReason = AI_ABORT_LOST_CIVILIAN;
	}
}


CvUnit* CvAINavalEscortedOperation::FindBestCivilian()
{
	int iUnitLoop = 0;
	CvUnit* pLoopUnit = NULL;

	CvPlayerAI& kOwner = GET_PLAYER(m_eOwner);

	for(pLoopUnit = kOwner.firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = kOwner.nextUnit(&iUnitLoop))
	{
		if(pLoopUnit != NULL)
		{
			if(pLoopUnit->AI_getUnitAIType() == m_eCivilianType)
			{
				if(pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX)
				{
					return pLoopUnit;
				}
			}
		}
	}
	return NULL;
}


void CvAINavalEscortedOperation::Read(FDataStream& kStream)
{

	CvAIOperation::Read(kStream);


	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_eCivilianType;
}


void CvAINavalEscortedOperation::Write(FDataStream& kStream) const
{

	CvAIOperation::Write(kStream);


	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << m_eCivilianType;
}


bool CvAINavalEscortedOperation::ArmyInPosition(CvArmyAI* pArmy)
{
	int iUnitID = 0;
	bool bStateChanged = false;
	CvUnit* pSettler = 0, *pEscort = 0;
	CvString strMsg;

	switch(m_eCurrentState)
	{
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	case AI_OPERATION_STATE_AT_TARGET:


		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);


		iUnitID = pArmy->GetFirstUnitID();
		if(iUnitID != -1)
		{
			pSettler = GET_PLAYER(m_eOwner).getUnit(iUnitID);
		}

		if(pSettler != NULL)
		{
			CvPlot* pTargetPlot = GetTargetPlot();
			const PlayerTypes pTargetPlotOwner = pTargetPlot->getOwner();

			CvPlot* pSettlerPlot = pSettler->plot();

			if((pTargetPlotOwner != NO_PLAYER && pTargetPlotOwner != m_eOwner) || pTargetPlot->IsAdjacentOwnedByOtherTeam(pSettler->getTeam()))
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("Not at target but can no longer settle here. Target was (X=%d Y=%d)", pTargetPlot->getX(), pTargetPlot->getY());
					LogOperationSpecialMessage(strMsg);
				}
				RetargetCivilian(pSettler, pArmy);
				pSettler->finishMoves();
				iUnitID = pArmy->GetNextUnitID();
				if(iUnitID != -1)
				{
					pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
					pEscort->finishMoves();
				}
			}

			else if(pSettlerPlot == pTargetPlot && pSettler->canMove() && pSettler->canFound(pSettlerPlot))
			{
				pSettler->PushMission(CvTypes::getMISSION_FOUND());
				if(GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("City founded, At X=%d, At Y=%d", pSettlerPlot->getX(), pSettlerPlot->getY());
					LogOperationSpecialMessage(strMsg);
				}
				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			}



			else if(pSettlerPlot == GetTargetPlot() && !pSettler->canFound(pSettlerPlot))
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("At target but can no longer settle here. Target was (X=%d Y=%d)", pTargetPlot->getX(), pTargetPlot->getY());
					LogOperationSpecialMessage(strMsg);
				}
				RetargetCivilian(pSettler, pArmy);
				pSettler->finishMoves();
				iUnitID = pArmy->GetNextUnitID();
				if(iUnitID != -1)
				{
					pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
					pEscort->finishMoves();
				}
			}
		}
		break;


	case AI_OPERATION_STATE_GATHERING_FORCES:
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}


CvPlot* CvAINavalEscortedOperation::FindBestTarget(CvUnit* pUnit)
{
	return GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, true, -1);
}


bool CvAINavalEscortedOperation::RetargetCivilian(CvUnit* pCivilian, CvArmyAI* pArmy)
{
	CvPlot* pBetterTarget;


	pBetterTarget = FindBestTarget(pCivilian);


	if(pBetterTarget == NULL)
	{
		m_eCurrentState = AI_OPERATION_STATE_ABORTED;
		m_eAbortReason = AI_ABORT_NO_TARGET;
		return false;
	}


	else if(pBetterTarget != GetTargetPlot())
	{
		SetTargetPlot(pBetterTarget);
		pArmy->SetGoalPlot(pBetterTarget);
	}

	pArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
	m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;

	return true;
}




CvAIOperationNavalAttack::CvAIOperationNavalAttack()
{
	m_eCivilianType = NO_UNITAI;
}

CvAIOperationNavalAttack::~CvAIOperationNavalAttack()
{
}


void CvAIOperationNavalAttack::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea, CvCity* pTarget, CvCity* pMuster)
{
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_NAVAL_ESCORT;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;
	SetDefaultArea(iDefaultArea);

	SetStartCityPlot(pMuster->plot());

	if(iID != -1)
	{
		if(pTarget)
		{
			SetTargetPlot(pTarget->plot());


			CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
			if(pArmyAI)
			{
				m_viArmyIDs.push_back(pArmyAI->GetID());
				pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
				pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
				pArmyAI->SetFormationIndex(GetFormation());

				pArmyAI->SetGoalPlot(GetTargetPlot());
				SetMusterPlot(GetStartCityPlot());
				pArmyAI->SetXY(GetStartCityPlot()->getX(), GetStartCityPlot()->getY());

				BuildListOfUnitsWeStillNeedToBuild();


				if(GrabUnitsFromTheReserves(GetMusterPlot(), NULL))
				{
					pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
					m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
				}
				else
				{
					m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
				}
				LogOperationStart();
			}
		}

		else
		{

			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_LOST_TARGET;
		}
	}
}


void CvAIOperationNavalAttack::UnitWasRemoved(int iArmyID, int iSlotID)
{

	CvAIOperation::UnitWasRemoved(iArmyID, iSlotID);
}


void CvAIOperationNavalAttack::Read(FDataStream& kStream)
{

	CvAINavalEscortedOperation::Read(kStream);


	uint uiVersion;
	kStream >> uiVersion;
}


void CvAIOperationNavalAttack::Write(FDataStream& kStream) const
{

	CvAINavalEscortedOperation::Write(kStream);


	uint uiVersion = 1;
	kStream << uiVersion;
}


bool CvAIOperationNavalAttack::ArmyInPosition(CvArmyAI* pArmy)
{
	switch(m_eCurrentState)
	{

	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{
		CvPlot *pCenterOfMass = pArmy->GetCenterOfMass(DOMAIN_SEA);


		if(pCenterOfMass && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), m_iTargetX, m_iTargetY) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE() * 2)
		{

			GET_PLAYER(GetOwner()).GetDiplomacyAI()->SetMusteringForAttack(GetEnemy(), true);


			CvTemporaryZone zone;
			zone.SetX(GetTargetPlot()->getX());
			zone.SetY(GetTargetPlot()->getY());
			zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
			zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
			zone.SetNavalInvasion(true);
			GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);

			m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			return true;
		}
	}
	break;


	case AI_OPERATION_STATE_GATHERING_FORCES:
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return false;
}


CvCity* CvAIOperationNavalAttack::GetOperationStartCity() const
{
	if(GetStartCityPlot())
	{
		return GetStartCityPlot()->getPlotCity();
	}

	return GET_PLAYER(m_eOwner).GetMilitaryAI()->GetNearestCoastalCity(m_eEnemy);
}


CvPlot* CvAIOperationNavalAttack::FindBestTarget()
{
	CvAssertMsg(false, "Obsolete function called CvAIOperationNavalAttack::FindBestTarget()");

	return NULL;
}






CvAIOperationNavalSneakAttack::CvAIOperationNavalSneakAttack()
{
}






CvAIOperationNavalCityStateAttack::CvAIOperationNavalCityStateAttack()
{
}


CvAIOperationNukeAttack::CvAIOperationNukeAttack()
{
	m_iBestUnitID = -1;
}

CvAIOperationNukeAttack::~CvAIOperationNukeAttack()
{
}

void CvAIOperationNukeAttack::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int , CvCity*            , CvCity*            )
{
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_STATIC;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;

	if(iID != -1)
	{

		CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
		CvArmyAI* pArmyAI = kPlayer.addArmyAI();
		if(pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(), m_eOwner, m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());


			CvPlot* pTargetPlot = FindBestTarget();
			if(pTargetPlot != NULL)
			{
				SetTargetPlot(pTargetPlot);
				pArmyAI->SetGoalPlot(pTargetPlot);

				BuildListOfUnitsWeStillNeedToBuild();

				if(GrabUnitsFromTheReserves(GC.getMap().plotCheckInvalid(m_iMusterX, m_iMusterY), pTargetPlot))
				{
					pArmyAI->SetArmyAIState(ARMYAISTATE_AT_DESTINATION);
					m_eCurrentState = AI_OPERATION_STATE_AT_TARGET;
					LogOperationStart();
					ArmyInPosition(pArmyAI);
				}
				else
				{

					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_NO_NUKES;
				}
			}

			else
			{

				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_LOST_TARGET;
			}
		}
	}
}



void CvAIOperationNukeAttack::Read(FDataStream& kStream)
{

	CvAIOperation::Read(kStream);


	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_iBestUnitID;
}


void CvAIOperationNukeAttack::Write(FDataStream& kStream) const
{

	CvAIOperation::Write(kStream);


	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << m_iBestUnitID;
}

CvCity* CvAIOperationNukeAttack::GetOperationStartCity() const
{
	if(GetStartCityPlot())
	{
		return GetStartCityPlot()->getPlotCity();
	}

	return GC.getMap().plotCheckInvalid(m_iMusterX, m_iMusterY)->getPlotCity();
}

bool CvAIOperationNukeAttack::ArmyInPosition(CvArmyAI* pArmy)
{
	CvPlot* pTargetPlot = GetTargetPlot();
	if(pTargetPlot)
	{

		int iUnitID = pArmy->GetFirstUnitID();
		CvUnit* pNuke = NULL;
		if(iUnitID != -1)
		{
			pNuke = GET_PLAYER(m_eOwner).getUnit(iUnitID);
		}

		if(pNuke != NULL)
		{
			if(pNuke->canMove() && pNuke->canNukeAt(pNuke->plot(),pTargetPlot->getX(),pTargetPlot->getY()))
			{
				pNuke->PushMission(CvTypes::getMISSION_NUKE(), pTargetPlot->getX(), pTargetPlot->getY());
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("City nuked, At X=%d, At Y=%d", pTargetPlot->getX(), pTargetPlot->getY());
					LogOperationSpecialMessage(strMsg);
				}
				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			}
		}
	}
	return true;
}

CvPlot* CvAIOperationNukeAttack::FindBestTarget()
{
	CvUnit* pLoopUnit;
	CvUnit* pBestUnit = NULL;
	CvCity* pBestCity = NULL;
	int iBestCity = 0;
	int iUnitLoop = 0;
	int iCityLoop = 0;
	CvPlayerAI& ownerPlayer = GET_PLAYER(m_eOwner);
	TeamTypes eTeam = ownerPlayer.getTeam();
	CvTeam& ourTeam = GET_TEAM(eTeam);
	CvPlayerAI& enemyPlayer = GET_PLAYER(m_eEnemy);

	int iBlastRadius = GC.getNUKE_BLAST_RADIUS();


	for(pLoopUnit = ownerPlayer.firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = ownerPlayer.nextUnit(&iUnitLoop))
	{
		if(pLoopUnit && pLoopUnit->canNuke(NULL))
		{
			int iUnitRange = pLoopUnit->GetRange();

			CvCity* pLoopCity;
			for(pLoopCity = enemyPlayer.firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = enemyPlayer.nextCity(&iCityLoop))
			{
				if(pLoopCity)
				{
					if(plotDistance(pLoopUnit->getX(),pLoopUnit->getY(),pLoopCity->getX(),pLoopCity->getY()) <= iUnitRange)
					{
						CvPlot* pCityPlot = pLoopCity->plot();
						int iThisCityValue = pLoopCity->getPopulation();
						iThisCityValue -= pLoopCity->getDamage() / 5;



#ifdef AUI_HEXSPACE_DX_LOOPS
						int iMaxDX, iDX;
						CvPlot* pLoopPlot;
						for (int iDY = -iBlastRadius; iDY <= iBlastRadius; iDY++)
						{
							iMaxDX = iBlastRadius - MAX(0, iDY);
							for (iDX = -iBlastRadius - MIN(0, iDY); iDX <= iMaxDX; iDX++)
							{

								pLoopPlot = plotXY(pCityPlot->getX(), pCityPlot->getY(), iDX, iDY);
#else
						for(int iDX = -iBlastRadius; iDX <= iBlastRadius; iDX++)
						{
							for(int iDY = -iBlastRadius; iDY <= iBlastRadius; iDY++)
							{
								CvPlot* pLoopPlot = plotXYWithRangeCheck(pCityPlot->getX(), pCityPlot->getY(), iDX, iDY, iBlastRadius);
#endif
								if(pLoopPlot)
								{

									PlayerTypes ePlotOwner = pLoopPlot->getOwner();
									TeamTypes ePlotTeam = pLoopPlot->getTeam();

									if(ePlotOwner == m_eOwner)
									{
										iThisCityValue -= 1;
										if(pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
										{
											if(!pLoopPlot->IsImprovementPillaged())
											{
												iThisCityValue -= 5;
												if(pLoopPlot->getResourceType(ePlotTeam) != NO_RESOURCE)
												{
													iThisCityValue -= 1000;
												}
											}
										}
									}
									else if(ePlotTeam != NO_TEAM && ourTeam.isAtWar(ePlotTeam))
									{
										iThisCityValue += 1;
										if(pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
										{
											if(!pLoopPlot->IsImprovementPillaged())
											{
												iThisCityValue += 2;
												if(pLoopPlot->getResourceType(ePlotTeam) != NO_RESOURCE)
												{
													iThisCityValue += 5;
												}
											}
										}
									}
									else if (ePlotOwner != NO_PLAYER)
									{
										iThisCityValue -= 1000;
									}





									const IDInfo* pUnitNode = pLoopPlot->headUnitNode();
									const CvUnit* pInnerLoopUnit;
									while(pUnitNode != NULL)
									{
										pInnerLoopUnit = ::getUnit(*pUnitNode);
										pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);
										if(pInnerLoopUnit != NULL)
										{
											PlayerTypes eUnitOwner = pInnerLoopUnit->getOwner();
											TeamTypes eUnitTeam = pInnerLoopUnit->getTeam();

											if(eUnitOwner == m_eOwner)
											{
												iThisCityValue -= 2;
											}
											else if(ourTeam.isAtWar(eUnitTeam))
											{
												iThisCityValue += 2;
											}
											else if (ePlotOwner != NO_PLAYER)
											{
												iThisCityValue -= 1000;
											}
										}
									}
								}
							}
						}


						if(pLoopCity->isCapital())
						{
							iThisCityValue *= 2;
						}

						if(iThisCityValue > iBestCity)
						{
							pBestUnit = pLoopUnit;
							pBestCity = pLoopCity;
							iBestCity = iThisCityValue;
						}
					}
				}
			}
		}
	}

	if(pBestCity && pBestUnit)
	{
		m_iMusterX = pBestUnit->getX();
		m_iMusterY = pBestUnit->getY();
		m_iBestUnitID = pBestUnit->GetID();
		return pBestCity->plot();
	}

	return NULL;
}


#ifdef AUI_WARNING_FIXES
bool CvAIOperationNukeAttack::FindBestFitReserveUnit(const OperationSlot& thisOperationSlot, CvPlot*                , CvPlot*                , bool* bRequired)
#else
bool CvAIOperationNukeAttack::FindBestFitReserveUnit(OperationSlot thisOperationSlot, CvPlot*                , CvPlot*                , bool* bRequired)
#endif
{

	*bRequired = true;
	CvPlayerAI& ownerPlayer = GET_PLAYER(m_eOwner);
	CvArmyAI* pThisArmy = ownerPlayer.getArmyAI(thisOperationSlot.m_iArmyID);
	pThisArmy->AddUnit(m_iBestUnitID, thisOperationSlot.m_iSlotID);
	return true;
}


FDataStream& operator<<(FDataStream& saveTo, const AIOperationState& readFrom)
{
	int v = static_cast<int>(readFrom);
	saveTo << v;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, AIOperationState& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<AIOperationState>(v);
	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const OperationSlot& readFrom)
{
	saveTo << readFrom.m_iOperationID;
	saveTo << readFrom.m_iArmyID;
	saveTo << readFrom.m_iSlotID;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, OperationSlot& writeTo)
{
	loadFrom >> writeTo.m_iOperationID;
	loadFrom >> writeTo.m_iArmyID;
	loadFrom >> writeTo.m_iSlotID;
	return loadFrom;
}

int OperationalAIHelpers::GetGatherRangeForXUnits(int iTotalUnits)
{
	int iRange = 0;

	if(iTotalUnits <= 2)
	{
		iRange = 1;
	}
	else if(iTotalUnits <= 6)
	{
		iRange = 2;
	}
	else if(iTotalUnits <= 10)
	{
		iRange = 3;
	}
	else
	{
		iRange = 4;
	}

	return iRange;
}
