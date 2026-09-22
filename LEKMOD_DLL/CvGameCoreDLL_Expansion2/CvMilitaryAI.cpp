






#include "CvGameCoreDLLPCH.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvMinorCivAI.h"
#include "CvDiplomacyAI.h"
#include "CvEconomicAI.h"
#include "CvMilitaryAI.h"
#include "CvGrandStrategyAI.h"
#include "CvCitySpecializationAI.h"
#include "Fireworks/FVariableSystem.h"
#include "cvStopWatch.h"


#include "LintFree.h"

CvMilitaryAIStrategyXMLEntry::CvMilitaryAIStrategyXMLEntry(void):
	m_piPlayerFlavorValue(NULL),
	m_piCityFlavorValue(NULL),
	m_iWeightThreshold(0),
	m_piPersonalityFlavorThresholdMod(NULL),
	m_bNoMinorCivs(false),
	m_bOnlyMinorCivs(false),
	m_bUpdateCitySpecializations(false),
	m_iTechPrereq(NO_TECH),
	m_iTechObsolete(NO_TECH),
	m_iMinimumNumTurnsExecuted(0),
	m_iCheckTriggerTurnCount(0),
	m_iFirstTurnExecuted(0),
	m_eAdvisor(NO_ADVISOR_TYPE),
	m_iAdvisorCounselImportance(1)
{
}

CvMilitaryAIStrategyXMLEntry::~CvMilitaryAIStrategyXMLEntry(void)
{
	SAFE_DELETE_ARRAY(m_piPlayerFlavorValue);
	SAFE_DELETE_ARRAY(m_piCityFlavorValue);
	SAFE_DELETE_ARRAY(m_piPersonalityFlavorThresholdMod);
}

bool CvMilitaryAIStrategyXMLEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;


	m_bNoMinorCivs				 = kResults.GetBool("NoMinorCivs");
	m_bOnlyMinorCivs		     = kResults.GetBool("OnlyMinorCivs");
	m_bUpdateCitySpecializations = kResults.GetBool("UpdateCitySpecializations");
	m_iCheckTriggerTurnCount	 = kResults.GetInt("CheckTriggerTurnCount");
	m_iFirstTurnExecuted            = kResults.GetInt("FirstTurnExecuted");
	m_iMinimumNumTurnsExecuted	 = kResults.GetInt("MinimumNumTurnsExecuted");
	m_iWeightThreshold			 = kResults.GetInt("WeightThreshold");

	const char* strAdvisor = kResults.GetText("Advisor");
	m_eAdvisor = NO_ADVISOR_TYPE;
	if(strAdvisor)
	{
		if(strcmp(strAdvisor, "MILITARY") == 0)
		{
			m_eAdvisor = ADVISOR_MILITARY;
		}
		else if(strcmp(strAdvisor, "ECONOMIC") == 0)
		{
			m_eAdvisor = ADVISOR_ECONOMIC;
		}
		else if(strcmp(strAdvisor, "FOREIGN") == 0)
		{
			m_eAdvisor = ADVISOR_FOREIGN;
		}
		else if(strcmp(strAdvisor, "SCIENCE") == 0)
		{
			m_eAdvisor = ADVISOR_SCIENCE;
		}
	}

	m_strAdvisorCounselText = kResults.GetText("AdvisorCounsel");


	const char* szTextVal = NULL;
	szTextVal = kResults.GetText("TechObsolete");
	m_iTechObsolete = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("TechPrereq");
	m_iTechPrereq = GC.getInfoTypeForString(szTextVal, true);


	const char* szType = GetType();
	kUtility.SetFlavors(m_piPlayerFlavorValue, "AIMilitaryStrategy_Player_Flavors",
	                    "AIMilitaryStrategyType", szType);
	kUtility.SetFlavors(m_piCityFlavorValue, "AIMilitaryStrategy_City_Flavors",
		"AIMilitaryStrategyType", szType);


	kUtility.SetFlavors(m_piPersonalityFlavorThresholdMod,
	                    "AIMilitaryStrategy_PersonalityFlavorThresholdMods",
	                    "AIMilitaryStrategyType", szType);

	return true;
}


int CvMilitaryAIStrategyXMLEntry::GetPlayerFlavorValue(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piPlayerFlavorValue ? m_piPlayerFlavorValue[i] : -1;
}


int CvMilitaryAIStrategyXMLEntry::GetCityFlavorValue(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piCityFlavorValue ? m_piCityFlavorValue[i] : -1;
}


int CvMilitaryAIStrategyXMLEntry::GetWeightThreshold() const
{
	return m_iWeightThreshold;
}


int CvMilitaryAIStrategyXMLEntry::GetPersonalityFlavorThresholdMod(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piPersonalityFlavorThresholdMod ? m_piPersonalityFlavorThresholdMod[i] : -1;
}


bool CvMilitaryAIStrategyXMLEntry::IsNoMinorCivs() const
{
	return m_bNoMinorCivs;
}


bool CvMilitaryAIStrategyXMLEntry::IsOnlyMinorCivs() const
{
	return m_bOnlyMinorCivs;
}


bool CvMilitaryAIStrategyXMLEntry::RequiresCitySpecializationUpdate() const
{
	return m_bUpdateCitySpecializations;
}


int CvMilitaryAIStrategyXMLEntry::GetTechPrereq() const
{
	return m_iTechPrereq;
}


int CvMilitaryAIStrategyXMLEntry::GetTechObsolete() const
{
	return m_iTechObsolete;
}


int CvMilitaryAIStrategyXMLEntry::GetMinimumNumTurnsExecuted() const
{
	return m_iMinimumNumTurnsExecuted;
}


int CvMilitaryAIStrategyXMLEntry::GetCheckTriggerTurnCount() const
{
	return m_iCheckTriggerTurnCount;
}


int CvMilitaryAIStrategyXMLEntry::GetFirstTurnExecuted() const
{
	return m_iFirstTurnExecuted;
}

AdvisorTypes CvMilitaryAIStrategyXMLEntry::GetAdvisor() const
{
	return m_eAdvisor;
}

const char* CvMilitaryAIStrategyXMLEntry::GetAdvisorCounselText() const
{
	return m_strAdvisorCounselText.c_str();
}

int CvMilitaryAIStrategyXMLEntry::GetAdvisorCounselImportance() const
{
	return m_iAdvisorCounselImportance;
}





CvMilitaryAIStrategyXMLEntries::CvMilitaryAIStrategyXMLEntries(void)
{

}


CvMilitaryAIStrategyXMLEntries::~CvMilitaryAIStrategyXMLEntries(void)
{
	DeleteArray();
}


std::vector<CvMilitaryAIStrategyXMLEntry*>& CvMilitaryAIStrategyXMLEntries::GetMilitaryAIStrategyEntries()
{
	return m_paAIStrategyEntries;
}


int CvMilitaryAIStrategyXMLEntries::GetNumMilitaryAIStrategies()
{
	return m_paAIStrategyEntries.size();
}


void CvMilitaryAIStrategyXMLEntries::DeleteArray()
{
	for(std::vector<CvMilitaryAIStrategyXMLEntry*>::iterator it = m_paAIStrategyEntries.begin(); it != m_paAIStrategyEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paAIStrategyEntries.clear();
}


_Ret_maybenull_ CvMilitaryAIStrategyXMLEntry* CvMilitaryAIStrategyXMLEntries::GetEntry(int index)
{
	return m_paAIStrategyEntries[index];
}







CvMilitaryAI::CvMilitaryAI():
	m_pabUsingStrategy(NULL),
	m_paiTurnStrategyAdopted(NULL),
	m_aiTempFlavors(NULL),
	m_paeLastTurnWarState(NULL),
	m_paeLastTurnMilitaryThreat(NULL),
	m_paeLastTurnMilitaryStrength(NULL),
	m_paeLastTurnTargetValue(NULL),
	m_iTotalThreatWeight(1),
	m_iNumberOfTimesOpsBuildSkippedOver(0),
	m_eArmyTypeBeingBuilt(NO_ARMY_TYPE)
{
}


CvMilitaryAI::~CvMilitaryAI(void)
{
	Uninit();
}


void CvMilitaryAI::Init(CvMilitaryAIStrategyXMLEntries* pAIStrategies, CvPlayer* pPlayer, CvDiplomacyAI* pDiplomacyAI)
{

	m_pAIStrategies = pAIStrategies;
	m_pPlayer = pPlayer;
	m_pDiplomacyAI = pDiplomacyAI;


	CvAssertMsg(m_pabUsingStrategy==NULL, "about to leak memory, CvMilitaryAI::m_pabUsingStrategy");
	m_pabUsingStrategy = FNEW(bool[m_pAIStrategies->GetNumMilitaryAIStrategies()], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paiTurnStrategyAdopted==NULL, "about to leak memory, CvMilitaryAI::m_paiTurnStrategyAdopted");
	m_paiTurnStrategyAdopted = FNEW(int[m_pAIStrategies->GetNumMilitaryAIStrategies()], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_aiTempFlavors==NULL, "about to leak memory, CvMilitaryAI::m_aiTempFlavors");
	m_aiTempFlavors = FNEW(int[GC.getNumFlavorTypes()], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paeLastTurnWarState==NULL, "about to leak memory, CvMilitaryAI::m_paeLastTurnWarState");
	m_paeLastTurnWarState = FNEW(int[MAX_CIV_PLAYERS], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paeLastTurnMilitaryThreat==NULL, "about to leak memory, CvMilitaryAI::m_paeLastTurnMilitaryThreat");
	m_paeLastTurnMilitaryThreat = FNEW(int[MAX_CIV_PLAYERS], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paeLastTurnMilitaryStrength==NULL, "about to leak memory, CvMilitaryAI::m_paeLastTurnMilitaryStrength");
	m_paeLastTurnMilitaryStrength = FNEW(int[MAX_CIV_PLAYERS], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paeLastTurnTargetValue==NULL, "about to leak memory, CvMilitaryAI::m_paeLastTurnTargetValue");
	m_paeLastTurnTargetValue = FNEW(int[MAX_CIV_PLAYERS], c_eCiv5GameplayDLL, 0);

	Reset();
}


void CvMilitaryAI::Uninit()
{
	SAFE_DELETE_ARRAY(m_pabUsingStrategy);
	SAFE_DELETE_ARRAY(m_paiTurnStrategyAdopted);
	SAFE_DELETE_ARRAY(m_aiTempFlavors);
	SAFE_DELETE_ARRAY(m_paeLastTurnWarState);
	SAFE_DELETE_ARRAY(m_paeLastTurnMilitaryThreat);
	SAFE_DELETE_ARRAY(m_paeLastTurnMilitaryStrength);
	SAFE_DELETE_ARRAY(m_paeLastTurnTargetValue);
}


void CvMilitaryAI::Reset()
{
	int iI;

	m_iTotalThreatWeight = 1;
	m_eArmyTypeBeingBuilt = NO_ARMY_TYPE;

	m_iNumLandUnits = 0;
	m_iNumRangedLandUnits = 0;
	m_iNumMobileLandUnits = 0;
	m_iNumAirUnits = 0;
	m_iNumAntiAirUnits = 0;
	m_iNumMeleeLandUnits = 0;
	m_iNumNavalUnits = 0;
	m_iNumLandUnitsInArmies = 0;
	m_iNumNavalUnitsInArmies = 0;
	m_iBarbarianCampCount = 0;
	m_iVisibleBarbarianCount = 0;
	m_iRecommendedMilitarySize = 0;
	m_iMandatoryReserveSize = 0;
	m_eLandDefenseState = NO_DEFENSE_STATE;
	m_eNavalDefenseState = NO_DEFENSE_STATE;
	m_iNumberOfTimesOpsBuildSkippedOver = 0;

	for(iI = 0; iI < m_pAIStrategies->GetNumMilitaryAIStrategies(); iI++)
	{
		m_pabUsingStrategy[iI] = false;
		m_paiTurnStrategyAdopted[iI] = -1;
		m_paeLastTurnWarState[iI] = NO_WAR_STATE_TYPE;
		m_paeLastTurnMilitaryThreat[iI] = NO_THREAT_VALUE;
		m_paeLastTurnMilitaryStrength[iI] = NO_STRENGTH_VALUE;
		m_paeLastTurnTargetValue[iI] = NO_TARGET_VALUE;
	}
}


void CvMilitaryAI::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	FAssertMsg(m_pAIStrategies != NULL && m_pAIStrategies->GetNumMilitaryAIStrategies() > 0, "Number of AIStrategies to serialize is expected to greater than 0");
	kStream >> m_iTotalThreatWeight;

	int temp;
	kStream >> temp;
	m_eArmyTypeBeingBuilt = (ArmyType)temp;

	kStream >> m_iNumberOfTimesOpsBuildSkippedOver;

	int iNumStrategies;
	kStream >> iNumStrategies;
#ifdef _MSC_VER
#pragma warning ( push )
#pragma warning ( disable : 6011 )
#endif
	ArrayWrapper<bool> wrapm_pabUsingStrategy(iNumStrategies, m_pabUsingStrategy);
#ifdef _MSC_VER
#pragma warning ( pop )
#endif
	kStream >> wrapm_pabUsingStrategy;

	ArrayWrapper<int> wrapm_paiTurnStrategyAdopted(iNumStrategies, m_paiTurnStrategyAdopted);
	kStream >> wrapm_paiTurnStrategyAdopted;

	ArrayWrapper<int> wrapm_paeLastTurnWarState(MAX_CIV_PLAYERS, m_paeLastTurnWarState);
	kStream >> wrapm_paeLastTurnWarState;

	ArrayWrapper<int> wrapm_paeLastTurnMilitaryThreat(MAX_CIV_PLAYERS, m_paeLastTurnMilitaryThreat);
	kStream >> wrapm_paeLastTurnMilitaryThreat;

	ArrayWrapper<int> wrapm_paeLastTurnMilitaryStrength(MAX_CIV_PLAYERS, m_paeLastTurnMilitaryStrength);
	kStream >> wrapm_paeLastTurnMilitaryStrength;

	ArrayWrapper<int> wrapm_paeLastTurnTargetValue(MAX_CIV_PLAYERS, m_paeLastTurnTargetValue);
	kStream >> wrapm_paeLastTurnTargetValue;
}


void CvMilitaryAI::Write(FDataStream& kStream)
{

	uint uiVersion = 1;
	kStream << uiVersion;

	FAssertMsg(GC.getNumMilitaryAIStrategyInfos() > 0, "Number of AIStrategies to serialize is expected to greater than 0");
	kStream << m_iTotalThreatWeight;
	kStream << (int)m_eArmyTypeBeingBuilt;
	kStream << m_iNumberOfTimesOpsBuildSkippedOver;
	kStream << GC.getNumMilitaryAIStrategyInfos();
	kStream << ArrayWrapper<bool>(m_pAIStrategies->GetNumMilitaryAIStrategies(), m_pabUsingStrategy);
	kStream << ArrayWrapper<int>(m_pAIStrategies->GetNumMilitaryAIStrategies(), m_paiTurnStrategyAdopted);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeLastTurnWarState);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeLastTurnMilitaryThreat);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeLastTurnMilitaryStrength);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeLastTurnTargetValue);
}


CvPlayer* CvMilitaryAI::GetPlayer()
{
	return m_pPlayer;
}


CvMilitaryAIStrategyXMLEntries* CvMilitaryAI::GetMilitaryAIStrategies()
{
	return m_pAIStrategies;
}


bool CvMilitaryAI::IsUsingStrategy(MilitaryAIStrategyTypes eStrategy)
{
	return m_pabUsingStrategy[(int) eStrategy];
}


void CvMilitaryAI::SetUsingStrategy(MilitaryAIStrategyTypes eStrategy, bool bValue)
{
	if(m_pabUsingStrategy[eStrategy] != bValue)
	{
		m_pabUsingStrategy[eStrategy] = bValue;

		if(bValue)
		{
			SetTurnStrategyAdopted(eStrategy, GC.getGame().getGameTurn());
		}
		else
		{
			SetTurnStrategyAdopted(eStrategy, -1);
		}

		LogStrategy(eStrategy, bValue);
	}
}


int CvMilitaryAI::GetTurnStrategyAdopted(MilitaryAIStrategyTypes eStrategy)
{
	return m_paiTurnStrategyAdopted[(int) eStrategy];
}


void CvMilitaryAI::SetTurnStrategyAdopted(MilitaryAIStrategyTypes eStrategy, int iValue)
{
	if(m_paiTurnStrategyAdopted[(int) eStrategy] != iValue)
	{
		m_paiTurnStrategyAdopted[(int) eStrategy] = iValue;
	}
}


void CvMilitaryAI::DoTurn()
{
	AI_PERF_FORMAT("AI-perf.csv", ("MilitaryAI DoTurn, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	ScanForBarbarians();
	UpdateThreats();
	UpdateWars();
	UpdateBaseData();
	UpdateDefenseState();
	UpdateMilitaryStrategies();

	if(!m_pPlayer->isHuman())
	{
		UpdateOperations();
		MakeEmergencyPurchases();
		MakeOffensivePurchases();
		RequestImprovements();
		DisbandObsoleteUnits();
	}

	LogMilitaryStatus();

	if(GetArmyBeingBuilt() != NO_ARMY_TYPE)
	{
		LogAvailableForces();
	}
}


bool CvMilitaryAI::RequestSneakAttack(PlayerTypes eEnemy)
{
	CvMilitaryTarget target;
	CvAIOperation* pOperation = 0;
	int iOperationID;


	if (m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_SNEAK_ATTACK, &iOperationID))
	{
		return false;
	}
	if (m_pPlayer->haveAIOperationOfType(AI_OPERATION_SNEAK_CITY_ATTACK, &iOperationID))
	{
		return false;
	}

	target = FindBestAttackTarget(AI_OPERATION_SNEAK_CITY_ATTACK, eEnemy);
	if(target.m_pTargetCity)
	{
		if(target.m_bAttackBySea)
		{
			if(IsAttackReady(MUFORMATION_NAVAL_INVASION, AI_OPERATION_NAVAL_SNEAK_ATTACK))
			{
				pOperation = m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_SNEAK_ATTACK, eEnemy, target.m_pTargetCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
				if(pOperation != NULL && !pOperation->ShouldAbort())
				{
					return true;
				}
			}
			else
			{
				m_iNumNavalAttacksRequested++;
				m_eArmyTypeBeingBuilt = (m_iNumLandAttacksRequested > m_iNumNavalAttacksRequested) ? ARMY_TYPE_LAND : ARMY_TYPE_NAVAL_INVASION;
			}
		}
		else
		{
			if(IsAttackReady((GC.getGame().getHandicapInfo().GetID() > 4 && !(GC.getMap().GetAIMapHint() & 1)) ? MUFORMATION_BIGGER_CITY_ATTACK_FORCE : MUFORMATION_BASIC_CITY_ATTACK_FORCE, AI_OPERATION_SNEAK_CITY_ATTACK))
			{
				pOperation = m_pPlayer->addAIOperation(AI_OPERATION_SNEAK_CITY_ATTACK, eEnemy, target.m_pTargetCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
				if(pOperation != NULL && !pOperation->ShouldAbort())
				{
					return true;
				}
			}
			else
			{
				m_iNumLandAttacksRequested++;
				m_eArmyTypeBeingBuilt = (m_iNumLandAttacksRequested > m_iNumNavalAttacksRequested) ? ARMY_TYPE_LAND : ARMY_TYPE_NAVAL_INVASION;
			}
		}
	}
	return false;
}


bool CvMilitaryAI::RequestShowOfForce(PlayerTypes eEnemy)
{
	CvMilitaryTarget target;

	target = FindBestAttackTarget(AI_OPERATION_SNEAK_CITY_ATTACK, eEnemy);
	if(target.m_pTargetCity)
	{
		if (target.m_bAttackBySea)
		{
			CvAIOperation* pOperation = m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_SNEAK_ATTACK, eEnemy, target.m_pTargetCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
			if(pOperation != NULL && !pOperation->ShouldAbort())
			{
				return true;
			}
		}
		else
		{
			CvAIOperation* pOperation = m_pPlayer->addAIOperation(AI_OPERATION_SMALL_CITY_ATTACK, eEnemy, target.m_pTargetCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
			if(pOperation != NULL && !pOperation->ShouldAbort())
			{
				return true;
			}
		}
	}

	return false;
}



bool CvMilitaryAI::RequestNukeAttack(PlayerTypes eEnemy)
{
	if(m_pPlayer->getNumNukeUnits() > 0)
	{
		CvAIOperation* pOperation = m_pPlayer->addAIOperation(AI_OPERATION_NUKE_ATTACK, eEnemy);
		if(pOperation != NULL && pOperation->GetOperationState() != AI_OPERATION_STATE_ABORTED)
		{
			return true;
		}
	}

	return false;
}



bool CvMilitaryAI::RequestPillageAttack(PlayerTypes eEnemy)
{
	int iNumRequiredSlots = 0;
	int iLandReservesUsed = 0;
	int iFilledSlots = 0;
	iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_FAST_PILLAGERS, false, &iNumRequiredSlots, &iLandReservesUsed);
	if(iFilledSlots >= iNumRequiredSlots && iLandReservesUsed <= GetLandReservesAvailable())
	{
		CvAIOperation* pOperation = m_pPlayer->addAIOperation(AI_OPERATION_PILLAGE_ENEMY, eEnemy);
		if(pOperation != NULL && !pOperation->ShouldAbort())
		{
			return true;
		}
	}

	return false;
}



bool CvMilitaryAI::RequestBasicAttack(PlayerTypes eEnemy, int iNumUnitsWillingBuild)
{
	CvMilitaryTarget target;

	target = FindBestAttackTarget(AI_OPERATION_BASIC_CITY_ATTACK, eEnemy);
	return RequestSpecificAttack(target, iNumUnitsWillingBuild);
}


bool CvMilitaryAI::RequestPureNavalAttack(PlayerTypes eEnemy, int iNumUnitsWillingBuild)
{
	CvAIOperation* pOperation = NULL;
	CvMilitaryTarget target;
	int iNumRequiredSlots = 0;
	int iLandReservesUsed = 0;
	int iFilledSlots = 0;

	target = FindBestAttackTarget(AI_OPERATION_PURE_NAVAL_CITY_ATTACK, eEnemy);
	if(target.m_pTargetCity)
	{
		iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_PURE_NAVAL_CITY_ATTACK, true, &iNumRequiredSlots, &iLandReservesUsed);
		if((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingBuild)
		{
			pOperation = m_pPlayer->addAIOperation(AI_OPERATION_PURE_NAVAL_CITY_ATTACK, eEnemy, target.m_pTargetCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
		}

		if(pOperation != NULL && !pOperation->ShouldAbort())
		{
			return true;
		}
	}
	return false;
}


bool CvMilitaryAI::RequestCityStateAttack(PlayerTypes eEnemy)
{
	CvMilitaryTarget target;
	CvAIOperation* pOperation = 0;

	target = FindBestAttackTarget(AI_OPERATION_CITY_STATE_ATTACK, eEnemy);
	if(target.m_pTargetCity)
	{
		if(target.m_bAttackBySea)
		{
			if(IsAttackReady(MUFORMATION_CITY_STATE_INVASION, AI_OPERATION_CITY_STATE_NAVAL_ATTACK))
			{
				pOperation = m_pPlayer->addAIOperation(AI_OPERATION_CITY_STATE_NAVAL_ATTACK, eEnemy, target.m_pTargetCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
				if(pOperation != NULL && !pOperation->ShouldAbort())
				{
					return true;
				}
			}
			else
			{
				m_iNumNavalAttacksRequested++;
				m_eArmyTypeBeingBuilt = (m_iNumLandAttacksRequested > m_iNumNavalAttacksRequested) ? ARMY_TYPE_LAND : ARMY_TYPE_NAVAL_INVASION;
			}
		}
		else
		{
			if(IsAttackReady(MUFORMATION_CITY_STATE_ATTACK_FORCE, AI_OPERATION_CITY_STATE_ATTACK))
			{
				pOperation = m_pPlayer->addAIOperation(AI_OPERATION_CITY_STATE_ATTACK, eEnemy, target.m_pTargetCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
				if(pOperation != NULL && !pOperation->ShouldAbort())
				{
					return true;
				}
			}
			else
			{
				m_iNumLandAttacksRequested++;
				m_eArmyTypeBeingBuilt = (m_iNumLandAttacksRequested > m_iNumNavalAttacksRequested) ? ARMY_TYPE_LAND : ARMY_TYPE_NAVAL_INVASION;
			}
		}

		return true;
	}
	return false;
}

bool CvMilitaryAI::RequestSpecificAttack(CvMilitaryTarget kTarget, int iNumUnitsWillingToBuild)
{
	CvAIOperation* pOperation = NULL;
	int iNumRequiredSlots = 0;
	int iLandReservesUsed = 0;
	int iFilledSlots = 0;

	if(kTarget.m_pTargetCity)
	{
		if(kTarget.m_bAttackBySea)
		{
			iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_NAVAL_INVASION, true, &iNumRequiredSlots, &iLandReservesUsed);
			if((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingToBuild && iLandReservesUsed <= GetLandReservesAvailable())
			{
				pOperation = m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_ATTACK, kTarget.m_pTargetCity->getOwner(), kTarget.m_pTargetCity->getArea(), kTarget.m_pTargetCity, kTarget.m_pMusterCity);
			}
		}
		else
		{
			iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer,(GC.getGame().getHandicapInfo().GetID() > 4 && !(GC.getMap().GetAIMapHint() & 1)) ? MUFORMATION_BIGGER_CITY_ATTACK_FORCE : MUFORMATION_BASIC_CITY_ATTACK_FORCE, false, &iNumRequiredSlots, &iLandReservesUsed);
			if((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingToBuild && iLandReservesUsed <= GetLandReservesAvailable())
			{
				pOperation = m_pPlayer->addAIOperation(AI_OPERATION_BASIC_CITY_ATTACK, kTarget.m_pTargetCity->getOwner(), kTarget.m_pTargetCity->getArea(), kTarget.m_pTargetCity, kTarget.m_pMusterCity);
				if (pOperation != NULL && !pOperation->ShouldAbort() && kTarget.m_pTargetCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
				{
					int iFlavorNaval = m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL"));
					int iNumSuperiority = m_pPlayer->numOperationsOfType(AI_OPERATION_NAVAL_SUPERIORITY);
					int iNumBombard = m_pPlayer->numOperationsOfType(AI_OPERATION_NAVAL_BOMBARDMENT);
					int iMaxOperations = iFlavorNaval / 2;
					if(GC.getMap().GetAIMapHint() & 1)
					{
						iMaxOperations *= 2;
					}
					if (iNumSuperiority+iNumBombard <= iMaxOperations)
						m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_SUPERIORITY, NO_PLAYER);
				}
			}
		}

		if(pOperation != NULL && !pOperation->ShouldAbort())
		{
			return true;
		}
	}
	return false;
}


CvAIOperation* CvMilitaryAI::GetSneakAttackOperation(PlayerTypes eEnemy)
{
	int iOperationID;
	bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_SNEAK_CITY_ATTACK, &iOperationID, eEnemy);

	if(bHasOperationUnderway)
	{
		return m_pPlayer->getAIOperation(iOperationID);
	}
	else
	{
		bool bHasOperationOfType = m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_SNEAK_ATTACK, &iOperationID, eEnemy);

		if(bHasOperationOfType)
		{
			return m_pPlayer->getAIOperation(iOperationID);
		}
	}

	return NULL;
}


CvAIOperation* CvMilitaryAI::GetShowOfForceOperation(PlayerTypes eEnemy)
{
	int iOperationID;
	bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_SMALL_CITY_ATTACK, &iOperationID, eEnemy);

	if(bHasOperationUnderway)
	{
		return m_pPlayer->getAIOperation(iOperationID);
	}

	return NULL;
}


CvAIOperation* CvMilitaryAI::GetBasicAttackOperation(PlayerTypes eEnemy)
{
	int iOperationID;
	bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_BASIC_CITY_ATTACK, &iOperationID, eEnemy);

	if(bHasOperationUnderway)
	{
		return m_pPlayer->getAIOperation(iOperationID);
	}
	else
	{
		bool bHasOperationOfType = m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_ATTACK, &iOperationID, eEnemy);

		if(bHasOperationOfType)
		{
			return m_pPlayer->getAIOperation(iOperationID);
		}
	}

	return NULL;
}


CvAIOperation* CvMilitaryAI::GetCityStateAttackOperation(PlayerTypes eEnemy)
{
	int iOperationID;
	bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_STATE_ATTACK, &iOperationID, eEnemy);

	if(bHasOperationUnderway)
	{
		return m_pPlayer->getAIOperation(iOperationID);
	}
	else
	{
		bool bHasOperationOfType = m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_STATE_NAVAL_ATTACK, &iOperationID, eEnemy);

		if(bHasOperationOfType)
		{
			return m_pPlayer->getAIOperation(iOperationID);
		}
	}

	return NULL;
}


CvAIOperation* CvMilitaryAI::GetPureNavalAttackOperation(PlayerTypes eEnemy)
{
	int iOperationID;
	bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_PURE_NAVAL_CITY_ATTACK, &iOperationID, eEnemy);

	if(bHasOperationUnderway)
	{
		return m_pPlayer->getAIOperation(iOperationID);
	}

	return NULL;
}


CvUnit* CvMilitaryAI::BuyEmergencyUnit(UnitAITypes eUnitType, CvCity* pCity)
{
	bool bIsVenice = m_pPlayer->GetPlayerTraits()->IsNoAnnexing();


	if(pCity->IsPuppet() && !bIsVenice)
	{
		return NULL;
	}


	UnitTypes eType = pCity->GetCityStrategyAI()->GetUnitProductionAI()->RecommendUnit(eUnitType);
	if(eType != NO_UNIT)
	{

		if(pCity->IsCanPurchase(                      true,                    true, eType, NO_BUILDING, NO_PROJECT, YIELD_GOLD))
		{
			int iGoldCost = pCity->GetPurchaseCost(eType);
			int iPriority = GC.getAI_GOLD_PRIORITY_UNIT();
			if(m_pPlayer->GetEconomicAI()->CanWithdrawMoneyForPurchase(PURCHASE_TYPE_UNIT, iGoldCost, iPriority))
			{
				if(pCity->getOwner() == m_pPlayer->GetID())
				{

					int iResult = pCity->CreateUnit(eType, NO_UNITAI, false                           );

					CvAssertMsg(iResult != FFreeList::INVALID_INDEX, "Unable to create unit");

					if (iResult != FFreeList::INVALID_INDEX)
					{
						CvUnit* pUnit = m_pPlayer->getUnit(iResult);
						m_pPlayer->GetTreasury()->LogExpenditure((CvString)pUnit->getUnitInfo().GetText(), iGoldCost, 7);
						m_pPlayer->GetTreasury()->ChangeGold(-iGoldCost);

						pUnit->setMoves(0);

						CvString szMsg;
						szMsg.Format("Emergency Unit Purchased: %s, ", pUnit->getUnitInfo().GetDescription());
						szMsg += pCity->getName();
						m_pPlayer->GetTacticalAI()->LogTacticalMessage(szMsg);

						return pUnit;
					}
					else
					{
						return NULL;
					}
				}
			}
		}


		if(pCity->IsCanPurchase(                      true,                    true, eType, NO_BUILDING, NO_PROJECT, YIELD_FAITH))
		{
			int iFaithCost = pCity->GetFaithPurchaseCost(eType, false                            );

			if(pCity->getOwner() == m_pPlayer->GetID())
			{
				m_pPlayer->ChangeFaith(-iFaithCost);


				int iResult = pCity->CreateUnit(eType, NO_UNITAI, false                           );

				CvAssertMsg(iResult != FFreeList::INVALID_INDEX, "Unable to create unit");
				CvUnit* pUnit = m_pPlayer->getUnit(iResult);
				pUnit->setMoves(0);

				CvString szMsg;
				szMsg.Format("Emergency Faith Unit Purchase: %s, ", pUnit->getUnitInfo().GetDescription());
				szMsg += pCity->getName();
				m_pPlayer->GetTacticalAI()->LogTacticalMessage(szMsg);

				return pUnit;
			}
		}
	}
	return NULL;
}


bool CvMilitaryAI::BuyEmergencyBuilding(CvCity* pCity)
{

#ifdef AUI_WARNING_FIXES
	for (uint iBldgLoop = 0; iBldgLoop < GC.GetGameBuildings()->GetNumBuildings(); iBldgLoop++)
#else
	for(int iBldgLoop = 0; iBldgLoop < GC.GetGameBuildings()->GetNumBuildings(); iBldgLoop++)
#endif
	{
		const BuildingTypes eBldg = static_cast<BuildingTypes>(iBldgLoop);
		CvBuildingEntry* pkBuildingInfo = GC.GetGameBuildings()->GetEntry(eBldg);
		if(pkBuildingInfo)
		{

#if defined(NQ_BUILDING_DEFENSE_FROM_CITIZENS) || defined(LEKMOD_GARRISON_YIELD_EFFECTS)
			if(pCity->canConstruct(eBldg) && (pkBuildingInfo->GetDefenseModifier() > 0 || pkBuildingInfo->GetDefensePerCitizen() > 0 || pkBuildingInfo->GetGarrisonStrengthBonus() > 0))
#else
			if(pCity->canConstruct(eBldg) && pkBuildingInfo->GetDefenseModifier() > 0)
#endif
			{
				if(pCity->IsCanPurchase(                      true,                    true, NO_UNIT, eBldg, NO_PROJECT, YIELD_GOLD))
				{
					int iGoldCost = pCity->GetPurchaseCost(eBldg);
					int iPriority = GC.getAI_GOLD_PRIORITY_DEFENSIVE_BUILDING();
					if(m_pPlayer->GetEconomicAI()->CanWithdrawMoneyForPurchase(PURCHASE_TYPE_BUILDING, iGoldCost, iPriority))
					{
						m_pPlayer->GetTreasury()->LogExpenditure((CvString)pkBuildingInfo->GetText(), iGoldCost, 8);
						m_pPlayer->GetTreasury()->ChangeGold(-iGoldCost);
#ifdef CVASSERT_ENABLE
						int iResult = pCity->CreateBuilding(eBldg);

						DEBUG_VARIABLE(iResult);
						CvAssertMsg(iResult != FFreeList::INVALID_INDEX, "Unable to create building");
#else
						pCity->CreateBuilding(eBldg);
#endif

						CvString szMsg;
						szMsg.Format("Emergency Building Purchased: %s, ", pkBuildingInfo->GetDescription());
						szMsg += pCity->getName();
						m_pPlayer->GetTacticalAI()->LogTacticalMessage(szMsg);
					}
				}
			}
		}
	}
	return false;
}




CvMilitaryTarget CvMilitaryAI::FindBestAttackTarget(AIOperationTypes eAIOperationType, PlayerTypes eEnemy, int* piWinningScore)
{
	int iFriendlyLoop;
	int iEnemyLoop;
	int iUnitLoop;
	CvCity* pFriendlyCity;
	CvCity* pEnemyCity;
	CvUnit* pLoopUnit;
	static CvWeightedVector<CvMilitaryTarget, SAFE_ESTIMATE_NUM_CITIES* 10, true> weightedTargetList;
	CvMilitaryTarget chosenTarget;
	CvPlayer &kEnemy = GET_PLAYER(eEnemy);


	for (pFriendlyCity = m_pPlayer->firstCity(&iFriendlyLoop); pFriendlyCity != NULL; pFriendlyCity = m_pPlayer->nextCity(&iFriendlyLoop))
	{
		CvPlot* pPlot = pFriendlyCity->plot();
		int iX = pPlot->getX();
		int iY = pPlot->getY();
		bool bGeneralInTheVicinity = false;
		int iPower = 0;
		for (pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
		{
			if (pLoopUnit->IsCombatUnit())
			{
				int iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), iX, iY);
				if (iDistance <= 5)
				{
					iPower += pLoopUnit->GetPower();
				}
			}
			if (!bGeneralInTheVicinity && pLoopUnit->IsGreatGeneral())
			{
				int iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), iX, iY);
				if (iDistance <= 5)
				{
					bGeneralInTheVicinity = true;
				}
			}
		}
		if (bGeneralInTheVicinity)
		{
			iPower *= 11;
			iPower /= 10;
		}
		pFriendlyCity->iScratch = iPower;
	}
	for(pEnemyCity = kEnemy.firstCity(&iEnemyLoop); pEnemyCity != NULL; pEnemyCity = kEnemy.nextCity(&iEnemyLoop))
	{
		CvPlot* pPlot = pEnemyCity->plot();
		if(pPlot->isRevealed(m_pPlayer->getTeam()))
		{
			int iX = pPlot->getX();
			int iY = pPlot->getY();
			bool bGeneralInTheVicinity = false;
			int iPower = 0;
			for (pLoopUnit = kEnemy.firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = kEnemy.nextUnit(&iUnitLoop))
			{
				if (pLoopUnit->IsCombatUnit())
				{
					int iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), iX, iY);
					if (iDistance <= 5)
					{
						iPower += pLoopUnit->GetPower();
					}
				}
				if (!bGeneralInTheVicinity && pLoopUnit->IsGreatGeneral())
				{
					int iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), iX, iY);
					if (iDistance <= 5)
					{
						bGeneralInTheVicinity = true;
					}
				}
			}
			if (bGeneralInTheVicinity)
			{
				iPower *= 11;
				iPower /= 10;
			}
			pEnemyCity->iScratch = iPower;
		}
	}


	static CvWeightedVector<CvMilitaryTarget, SAFE_ESTIMATE_NUM_CITIES* 10, true> prelimWeightedTargetList;
	prelimWeightedTargetList.clear();
	for(pFriendlyCity = m_pPlayer->firstCity(&iFriendlyLoop); pFriendlyCity != NULL; pFriendlyCity = m_pPlayer->nextCity(&iFriendlyLoop))
	{
		for(pEnemyCity = GET_PLAYER(eEnemy).firstCity(&iEnemyLoop); pEnemyCity != NULL; pEnemyCity = GET_PLAYER(eEnemy).nextCity(&iEnemyLoop))
		{
			if(pEnemyCity->plot()->isRevealed(m_pPlayer->getTeam()))
			{
				CvMilitaryTarget target;
				int iWeight;
				target.m_pMusterCity = pFriendlyCity;
				target.m_pTargetCity = pEnemyCity;
				target.iMusterNearbyUnitPower = pFriendlyCity->iScratch;
				target.iTargetNearbyUnitPower = pEnemyCity->iScratch;

				if (eAIOperationType == AI_OPERATION_PURE_NAVAL_CITY_ATTACK)
				{
					target.m_bAttackBySea = true;
					if (target.m_pMusterCity->isCoastal() && target.m_pTargetCity->isCoastal())
					{
						target.m_iPathLength = plotDistance(target.m_pMusterCity->getX(), target.m_pMusterCity->getY(), target.m_pTargetCity->getX(), target.m_pTargetCity->getY());
					}
				}

				else
				{
					ShouldAttackBySea(eEnemy, target);

					if (!pFriendlyCity->isCoastal() && target.m_bAttackBySea)
					{
						continue;
					}
				}

				if(target.m_iPathLength > 0)
				{
					iWeight = (10000 - target.m_iPathLength);
					prelimWeightedTargetList.push_back(target, iWeight);
				}
			}
		}
	}


	prelimWeightedTargetList.SortItems();
	weightedTargetList.clear();
	int iTargetsConsidered = 0;
	for (int iI = 0; iI < prelimWeightedTargetList.size() && iTargetsConsidered < 25; iI++)
	{
		CvMilitaryTarget target = prelimWeightedTargetList.GetElement(iI);
		int iWeight;


		if (target.m_bAttackBySea)
		{
			if (!target.m_pMusterCity->isCoastal())
			{
				continue;
			}
			if (!target.m_pTargetCity->isCoastal())
			{
				continue;
			}
			CvPlot *pSeaPlotNearMuster = GetCoastalPlotAdjacentToTarget(target.m_pMusterCity->plot(), NULL);
			CvPlot *pSeaPlotNearTarget = GetCoastalPlotAdjacentToTarget(target.m_pTargetCity->plot(), NULL);
			if(!GC.getStepFinder().DoesPathExist(m_pPlayer->GetID(), eEnemy, pSeaPlotNearMuster, pSeaPlotNearTarget))
			{
				continue;
			}
		}

		iWeight = ScoreTarget(target, eAIOperationType);
		weightedTargetList.push_back(target, iWeight);
		iTargetsConsidered++;
	}


	if(weightedTargetList.size() == 0)
	{
		chosenTarget.m_pTargetCity = NULL;
		if (piWinningScore)
		{
			*piWinningScore = -1;
		}
		return chosenTarget;
	}

	weightedTargetList.SortItems();
	LogAttackTargets(eAIOperationType, eEnemy, weightedTargetList);

	if(weightedTargetList.GetTotalWeight() > 0)
	{
		RandomNumberDelegate fcn;
		fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
		int iNumChoices = max (1, (weightedTargetList.size() * 25 / 100));
		chosenTarget = weightedTargetList.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing attack target from top 25%% of choices");

		if (piWinningScore)
		{
			*piWinningScore = ScoreTarget(chosenTarget, eAIOperationType);
		}
		LogChosenTarget(eAIOperationType, eEnemy, chosenTarget);
	}
	else
	{
		chosenTarget.m_pTargetCity = NULL;
		if (piWinningScore)
		{
			*piWinningScore = -1;
		}
	}

	return chosenTarget;
}


void CvMilitaryAI::ShouldAttackBySea(PlayerTypes eEnemy, CvMilitaryTarget& target)
{
	CvAStarNode* pPathfinderNode;
	int iPathLength = 0;
	int iPlotDistance = plotDistance(target.m_pMusterCity->getX(), target.m_pMusterCity->getY(), target.m_pTargetCity->getX(), target.m_pTargetCity->getY());


	if(GET_TEAM(m_pPlayer->getTeam()).canEmbark())
	{

		if(target.m_pMusterCity->getArea() != target.m_pTargetCity->getArea())
		{
			target.m_bAttackBySea = true;
			target.m_iPathLength = iPlotDistance;
			return;
		}


		if(!GC.getStepFinder().DoesPathExist(m_pPlayer->GetID(), eEnemy, target.m_pMusterCity->plot(), target.m_pTargetCity->plot()))
		{
			target.m_bAttackBySea = true;
			target.m_iPathLength = iPlotDistance;
			return;
		}


		pPathfinderNode = GC.getStepFinder().GetLastNode();
		if(pPathfinderNode != NULL)
		{
			iPathLength = pPathfinderNode->m_iData1;
			if(iPathLength > (2 * iPlotDistance))
			{
				target.m_bAttackBySea = true;
				target.m_iPathLength = iPlotDistance;
				return;
			}
		}
	}


	else
	{
		if(!GC.getStepFinder().DoesPathExist(m_pPlayer->GetID(), eEnemy, target.m_pMusterCity->plot(), target.m_pTargetCity->plot()))
		{
			target.m_iPathLength = -1;
			return;
		}

		else
		{
			pPathfinderNode = GC.getStepFinder().GetLastNode();
			if(pPathfinderNode != NULL)
			{
				iPathLength = pPathfinderNode->m_iData1;
			}
		}
	}

	target.m_bAttackBySea = false;
	target.m_iPathLength = iPathLength;
}


int CvMilitaryAI::ScoreTarget(CvMilitaryTarget& target, AIOperationTypes eAIOperationType)
{
	unsigned long int uliRtnValue = 1;


	if(!target.m_bAttackBySea)
	{
		if(target.m_iPathLength < 10)
		{
			uliRtnValue *= 16;
		}
		else if(target.m_iPathLength < 15)
		{
			uliRtnValue *= 8;
		}
		else if(target.m_iPathLength < 20)
		{
			uliRtnValue *= 4;
		}
		else
		{
			uliRtnValue *= 2;
		}


		if (target.m_pMusterCity->CanAirlift())
		{
			uliRtnValue *= 2;
		}
	}
	else
	{
		if(target.m_iPathLength < 12)
		{
			uliRtnValue *= 5;
		}
		else if(target.m_iPathLength < 20)
		{
			uliRtnValue *= 3;
		}
		else if(target.m_iPathLength < 30)
		{
			uliRtnValue *= 2;
		}


		if(!target.m_pTargetCity->plot()->isCoastalLand())
		{
			uliRtnValue /= 2;
		}
	}



	if (eAIOperationType == AI_OPERATION_SNEAK_CITY_ATTACK || eAIOperationType == AI_OPERATION_NAVAL_SNEAK_ATTACK)
	{
		uliRtnValue = uliRtnValue * uliRtnValue * uliRtnValue;
	}

	CityAttackApproaches eApproaches;
	int iApproachMultiplier = 0;
	eApproaches = EvaluateMilitaryApproaches(target.m_pTargetCity, true                                               , target.m_bAttackBySea);
	switch(eApproaches)
	{
	case ATTACK_APPROACH_UNRESTRICTED:
		iApproachMultiplier = 10;
		break;

	case ATTACK_APPROACH_OPEN:
		iApproachMultiplier = 8;
		break;

	case ATTACK_APPROACH_NEUTRAL:
		iApproachMultiplier = 4;
		break;

	case ATTACK_APPROACH_LIMITED:
		iApproachMultiplier = 2;
		break;

	case ATTACK_APPROACH_RESTRICTED:
		iApproachMultiplier = 1;
		break;

	case ATTACK_APPROACH_NONE:
		iApproachMultiplier = 0;
		break;
	}

	uliRtnValue *= iApproachMultiplier;




	if (!target.m_pMusterCity->IsRouteToCapitalConnected() && !target.m_pMusterCity->isCapital())
	{
		uliRtnValue /= 4;
	}




	int iFriendlyStrength = target.iMusterNearbyUnitPower;
	int iEnemyStrength = target.iTargetNearbyUnitPower + (target.m_pTargetCity->getStrengthValue() / 50);
	iFriendlyStrength = max(1, iFriendlyStrength);
	iEnemyStrength = max(1, iEnemyStrength);
	int iRatio = 1;
	iRatio = (iFriendlyStrength * 100) / iEnemyStrength;
	iRatio = min(1000, iRatio);
	uliRtnValue *= iRatio;

	if (target.m_pTargetCity->IsOriginalCapital())
	{
		uliRtnValue *= GC.getAI_MILITARY_CAPTURING_ORIGINAL_CAPITAL();
		uliRtnValue /= 100;
	}

	if (target.m_pTargetCity->getOriginalOwner() == m_pPlayer->GetID())
	{
		uliRtnValue *= GC.getAI_MILITARY_RECAPTURING_OWN_CITY();
		uliRtnValue /= 100;
	}


	if(m_pPlayer->IsCityAlreadyTargeted(target.m_pTargetCity, NO_DOMAIN, 50))
	{
		uliRtnValue /= 10;
	}

	uliRtnValue /= 1000;


	unsigned long int iEconomicValue = 1 + (target.m_pTargetCity->getPopulation() / 3);


	iEconomicValue += target.m_pTargetCity->getYieldRateTimes100(YIELD_FOOD, false) / 10;
	iEconomicValue += target.m_pTargetCity->getYieldRateTimes100(YIELD_PRODUCTION, false) / 10;
	iEconomicValue += target.m_pTargetCity->getYieldRateTimes100(YIELD_SCIENCE, false) / 10;
	iEconomicValue += target.m_pTargetCity->getYieldRateTimes100(YIELD_GOLD, false) / 10;
	iEconomicValue += target.m_pTargetCity->getYieldRateTimes100(YIELD_CULTURE, false) / 10;
	iEconomicValue += target.m_pTargetCity->getYieldRateTimes100(YIELD_FAITH, false) / 10;
	uliRtnValue *= iEconomicValue;

	uliRtnValue /= 10;

	return min(10000000, (int)uliRtnValue & 0x7fffffff);
}


CityAttackApproaches CvMilitaryAI::EvaluateMilitaryApproaches(CvCity* pCity, bool bAttackByLand, bool bAttackBySea)
{
	CvPlot* pLoopPlot;
	CityAttackApproaches eRtnValue = ATTACK_APPROACH_UNRESTRICTED;
	int iNumBlocked = 0;


	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(pCity->getX(), pCity->getY(), ((DirectionTypes)iI));


		if(pLoopPlot == NULL)
		{
			iNumBlocked++;
		}
		else
		{

			if(pLoopPlot->isLake())
			{
				iNumBlocked++;
			}

			else if(pLoopPlot->isWater() && !bAttackBySea)
			{
				iNumBlocked++;
			}

			else if(!pLoopPlot->isWater())
			{
				if(!bAttackByLand)
				{
					iNumBlocked++;
				}
				else
				{
					if(pLoopPlot->isImpassable() || pLoopPlot->isMountain())
					{
						iNumBlocked++;
					}
				}
			}
		}
	}

	switch(iNumBlocked)
	{
	case 0:
		eRtnValue = ATTACK_APPROACH_UNRESTRICTED;
		break;
	case 1:
	case 2:
		eRtnValue = ATTACK_APPROACH_OPEN;
		break;
	case 3:
		eRtnValue = ATTACK_APPROACH_NEUTRAL;
		break;
	case 4:
		eRtnValue = ATTACK_APPROACH_LIMITED;
		break;
	case 5:
		eRtnValue = ATTACK_APPROACH_RESTRICTED;
		break;
	case 6:
		eRtnValue = ATTACK_APPROACH_NONE;
		break;
	}

	return eRtnValue;
}


CvCity* CvMilitaryAI::GetNearestCoastalCity(PlayerTypes eEnemy) const
{
	CvCity* pBestCoastalCity = NULL;
	CvCity* pLoopCity, *pEnemyCity;
	int iLoop, iEnemyLoop;
	int iBestDistance = MAX_INT;

	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity->isCoastal())
		{
			for(pEnemyCity = GET_PLAYER(eEnemy).firstCity(&iEnemyLoop); pEnemyCity != NULL; pEnemyCity = GET_PLAYER(eEnemy).nextCity(&iEnemyLoop))
			{

				if(pEnemyCity->isCoastal() && pEnemyCity->plot()->isRevealed(m_pPlayer->getTeam()))
				{

					if(OnSameBodyOfWater(pLoopCity, pEnemyCity))
					{
						int iDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pEnemyCity->getX(), pEnemyCity->getY());
						if(iDistance < iBestDistance)
						{
							iBestDistance = iDistance;
							pBestCoastalCity = pLoopCity;
						}
					}
				}
			}
		}
	}

	return pBestCoastalCity;
}

CvPlot* CvMilitaryAI::GetCoastalPlotAdjacentToTarget(CvPlot *pTarget, CvArmyAI *pArmy) const
{
	CvPlot *pCoastalPlot = NULL;
	UnitHandle pInitialUnit;
	int iBestDistance = MAX_INT;
	int iUnitID = -1;
	if (pArmy)
	{
		iUnitID = pArmy->GetFirstUnitID();
		pInitialUnit = m_pPlayer->getUnit(iUnitID);
	}


	for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
	{
		CvPlot* pAdjacentPlot = plotDirection(pTarget->getX(), pTarget->getY(), ((DirectionTypes)iDirectionLoop));
		if(pAdjacentPlot != NULL && pAdjacentPlot->isWater() && pAdjacentPlot->isShallowWater())
		{

			if(pInitialUnit)
			{
				if (TurnsToReachTarget(pInitialUnit, pAdjacentPlot, false                , true                 , true                    ) < MAX_INT)
				{
					int iDistance = plotDistance(pInitialUnit->getX(), pInitialUnit->getY(), pTarget->getX(), pTarget->getY());
					if (iDistance < iBestDistance)
					{
						iBestDistance = iDistance;
						pCoastalPlot = pAdjacentPlot;
					}
				}
			}
			else
			{
				return pAdjacentPlot;
			}
		}
	}

	return pCoastalPlot;
}




ThreatTypes CvMilitaryAI::GetHighestThreat()
{
	ThreatTypes eHighestThreat = THREAT_NONE;
	ThreatTypes eMilitaryThreatType;

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;


		if(eLoopPlayer != m_pPlayer->GetID() && m_pDiplomacyAI->IsPlayerValid(eLoopPlayer))
		{
			eMilitaryThreatType = m_pDiplomacyAI->GetMilitaryThreat(eLoopPlayer);
			if((int)eMilitaryThreatType > (int)eHighestThreat)
			{
				eHighestThreat = eMilitaryThreatType;
			}
		}
	}
	return eHighestThreat;
}


int CvMilitaryAI::GetBarbarianThreatTotal()
{
	int iRtnValue = 0;

	ScanForBarbarians();


	iRtnValue += GC.getAI_MILITARY_THREAT_WEIGHT_MAJOR() * m_iBarbarianCampCount;


#ifdef AUI_MILITARY_FIX_BARBARIAN_THREAT
	iRtnValue += m_iVisibleBarbarianCount * GC.getAI_MILITARY_BARBARIANS_FOR_MINOR_THREAT();
#else
	iRtnValue += m_iVisibleBarbarianCount / GC.getAI_MILITARY_BARBARIANS_FOR_MINOR_THREAT();
#endif

	return iRtnValue;
}


int CvMilitaryAI::GetThreatWeight(ThreatTypes eThreat)
{
	int iRtnValue = 0;

	switch(eThreat)
	{
	case THREAT_MINOR:
		iRtnValue = GC.getAI_MILITARY_THREAT_WEIGHT_MINOR();
		break;
	case THREAT_MAJOR:
		iRtnValue = GC.getAI_MILITARY_THREAT_WEIGHT_MAJOR();
		break;
	case THREAT_SEVERE:
		iRtnValue = GC.getAI_MILITARY_THREAT_WEIGHT_SEVERE();
		break;
	case THREAT_CRITICAL:
		iRtnValue = GC.getAI_MILITARY_THREAT_WEIGHT_CRITICAL();
		break;
	}

	return iRtnValue;
}


int CvMilitaryAI::GetNumberCivsAtWarWith() const
{
	PlayerTypes eLoopPlayer;
	WarStateTypes eWarState;
	int iRtnValue = 0;


	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;


		if(eLoopPlayer != m_pPlayer->GetID() && m_pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			eWarState = m_pPlayer->GetDiplomacyAI()->GetWarState(eLoopPlayer);
			if(eWarState != NO_WAR_STATE_TYPE)
			{
				iRtnValue++;
			}
		}
	}

	return iRtnValue;
}


CvCity* CvMilitaryAI::GetMostThreatenedCity(int iOrder)
{


	FFastVector<CvCity*> m_apCities;
	m_apCities.push_back_copy(NULL, iOrder + 1);

	for(int iCycle = 0; iCycle < iOrder + 1; iCycle++)
	{
		CvCity* pCity = NULL;
		int iHighestThreatValue = 0;

		CvCity* pLoopCity;
		int iLoopCity = 0;
		for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
		{
			bool bCityAlreadyExamined = false;
			for(uint ui = 0; ui < m_apCities.size(); ui++)
			{
				if(pLoopCity == m_apCities[ui])
				{
					bCityAlreadyExamined = true;
					break;
				}
			}
			if(bCityAlreadyExamined)
			{
				continue;
			}

			int iThreatValue = pLoopCity->getThreatValue();
			iThreatValue = iThreatValue * pLoopCity->getPopulation();

			if(pLoopCity->isCapital())
			{
				iThreatValue = (iThreatValue * GC.getAI_MILITARY_CITY_THREAT_WEIGHT_CAPITAL()) / 100;
			}

			if(iThreatValue > iHighestThreatValue)
			{
				pCity = pLoopCity;
				iHighestThreatValue = iThreatValue;
			}
		}

		if(pCity)
		{
			m_apCities[iCycle] = pCity;
		}
		else
		{

			break;
		}
	}

	return m_apCities[iOrder];
}


int CvMilitaryAI::GetPercentOfRecommendedMilitarySize() const
{
	if(m_iRecommendedMilitarySize <= 0)
	{
		return 100;
	}
	else
	{
		return m_iNumLandUnits * 100 / m_iRecommendedMilitarySize;
	}
}


void CvMilitaryAI::LogAttackTargets(AIOperationTypes eAIOperationType, PlayerTypes eEnemy, CvWeightedVector<CvMilitaryTarget, SAFE_ESTIMATE_NUM_CITIES* 10, true>& weightedTargetList)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strLogName;


		playerName = m_pPlayer->getCivilizationShortDescription();


		FILogFile* pLog;
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "OperationalAILog_" + playerName + ".csv";
		}
		else
		{
			strLogName = "OperationalAILog.csv";
		}
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";
		if(eAIOperationType == AI_OPERATION_BASIC_CITY_ATTACK)
		{
			strBaseString += "Basic Attack, ";
		}
		else if(eAIOperationType == AI_OPERATION_SNEAK_CITY_ATTACK)
		{
			strBaseString += "Sneak Attack, ";
		}
		else if(eAIOperationType == AI_OPERATION_PURE_NAVAL_CITY_ATTACK)
		{
			strBaseString += "Pure Naval Attack, ";
		}
		else
		{
			strBaseString += "City State Attack, ";
		}
		strBaseString += GET_PLAYER(eEnemy).getCivilizationShortDescription();
		strBaseString += ", ";


		for(int iI = 0; iI < weightedTargetList.size(); iI++)
		{
			CvMilitaryTarget target = weightedTargetList.GetElement(iI);
			int iWeight = weightedTargetList.GetWeight(iI);
			strTemp.Format("Target: %s, Muster: %s, %d, Distance: %d", target.m_pTargetCity->getName().GetCString(), target.m_pMusterCity->getName().GetCString(), iWeight, target.m_iPathLength);
			strOutBuf = strBaseString + strTemp;
			if(target.m_bAttackBySea)
			{
				strOutBuf += ", Sea";
			}
			else
			{
				strOutBuf += ", Land";
			}
			pLog->Msg(strOutBuf);
		}
	}
}


void CvMilitaryAI::LogChosenTarget(AIOperationTypes eAIOperationType, PlayerTypes eEnemy, CvMilitaryTarget& target)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strLogName;


		playerName = m_pPlayer->getCivilizationShortDescription();


		FILogFile* pLog;
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "OperationalAILog_" + playerName + ".csv";
		}
		else
		{
			strLogName = "OperationalAILog.csv";
		}
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";
		if(eAIOperationType == AI_OPERATION_BASIC_CITY_ATTACK)
		{
			strBaseString += "Basic Attack, ";
		}
		else if(eAIOperationType == AI_OPERATION_SNEAK_CITY_ATTACK)
		{
			strBaseString += "Sneak Attack, ";
		}
		else if(eAIOperationType == AI_OPERATION_PURE_NAVAL_CITY_ATTACK)
		{
			strBaseString += "Pure Naval Attack, ";
		}
		else
		{
			strBaseString += "City State Attack, ";
		}
		strBaseString += GET_PLAYER(eEnemy).getCivilizationShortDescription();
		strBaseString += ", ";

		strTemp.Format("CHOSEN: %s, Muster: %s", target.m_pTargetCity->getName().GetCString(), target.m_pMusterCity->getName().GetCString());
		strOutBuf = strBaseString + strTemp;
		if(target.m_bAttackBySea)
		{
			strOutBuf += ", Sea";
		}
		else
		{
			strOutBuf += ", Land";
		}
		pLog->Msg(strOutBuf);
	}
}


void CvMilitaryAI::LogDeclarationOfWar(PlayerTypes eOpponent)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString opponentName;
		CvString playerName;
		CvString strTemp;
		FILogFile* pLog;


		playerName = GetPlayer()->getCivilizationShortDescription();
		opponentName = GET_PLAYER(eOpponent).getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";


		strTemp.Format("Declared War on: %s", opponentName.GetCString());
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);

		LogMilitarySummaryMessage(strTemp);
	}
}


void CvMilitaryAI::LogCivilizationDestroyed()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString strTemp;
		FILogFile* pLog;


		playerName = GetPlayer()->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";


		strTemp.Format("DESTROYED!");
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);

		LogMilitarySummaryMessage(strTemp);
	}
}


void CvMilitaryAI::LogCityCaptured(CvCity* pCity, PlayerTypes eOldOwner)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strPlayerName;
		CvString strOldOwnerName;
		CvString strCityName;
		CvString strTemp;
		FILogFile* pLog;


		strPlayerName = GetPlayer()->getCivilizationShortDescription();
		strOldOwnerName = GET_PLAYER(eOldOwner).getCivilizationShortDescription();
		strCityName = pCity->getName();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";


		strTemp = "Captured: " + strCityName + ", ";
		strTemp += "From: " + strOldOwnerName;
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);

		LogMilitarySummaryMessage(strTemp);
	}
}


void CvMilitaryAI::LogCityRazed(CvCity* pCity, PlayerTypes eOldOwner)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strPlayerName;
		CvString strOldOwnerName;
		CvString strCityName;
		CvString strTemp;
		FILogFile* pLog;


		strPlayerName = GetPlayer()->getCivilizationShortDescription();
		strOldOwnerName = GET_PLAYER(eOldOwner).getCivilizationShortDescription();
		strCityName = pCity->getName();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";


		strTemp = "Razed: " + strCityName + ", ";
		strTemp += "From: " + strOldOwnerName;
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);

		LogMilitarySummaryMessage(strTemp);
	}
}


void CvMilitaryAI::LogPeace(PlayerTypes eOpponent)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strOpponentName;
		CvString strPlayerName;
		CvString strTemp;
		FILogFile* pLog;


		strPlayerName = GetPlayer()->getCivilizationShortDescription();
		strOpponentName = GET_PLAYER(eOpponent).getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";


		strTemp.Format("Made peace with: %s", strOpponentName.GetCString());
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);

		LogMilitarySummaryMessage(strTemp);
	}
}


void CvMilitaryAI::LogPeace(TeamTypes eOpponentTeam)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strOpponentName;
		CvString strPlayerName;
		CvString strTemp;
		FILogFile* pLog;


		strPlayerName = GetPlayer()->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";


		for(int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
			if(kPlayer.isAlive() && kPlayer.getTeam() == eOpponentTeam)
			{
				if(strOpponentName.GetLength() != 0)
					strOpponentName += ", ";

				strOpponentName += kPlayer.getCivilizationShortDescription();
			}
		}

		strTemp.Format("Made peace with team: %d (%s)", (int)eOpponentTeam, strOpponentName.GetCString());
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);

		LogMilitarySummaryMessage(strTemp);
	}
}



void CvMilitaryAI::LogDeficitScrapUnit(UnitHandle pUnit)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strTemp;
		CvString playerName;
		FILogFile* pLog;


		playerName = m_pPlayer->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		strOutBuf.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strOutBuf += playerName + ", ";
		strTemp.Format("Scrapping %s, X: %d, Y: %d, ", pUnit->getUnitInfo().GetDescription(), pUnit->getX(), pUnit->getY());
		strOutBuf += strTemp;
		strOutBuf += "by DoDeficit(), , ";
		if(pUnit->getDomainType() == DOMAIN_LAND)
		{
			strTemp.Format("Num Land Units: %d, In Armies %d, Rec Size: %d, ", m_iNumLandUnits, m_iNumLandUnitsInArmies, m_iRecommendedMilitarySize);
		}
		else
		{
			strTemp.Format("Num Naval Units: %d, In Armies %d, ", m_iNumNavalUnits, m_iNumNavalUnitsInArmies);
		}
		strOutBuf += strTemp;
		pLog->Msg(strOutBuf);
	}
}




void CvMilitaryAI::UpdateBaseData()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("UpdateBaseData, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );


	CvUnit* pLoopUnit;
	int iLoop;

	m_iNumLandUnits = 0;
	m_iNumRangedLandUnits = 0;
	m_iNumMobileLandUnits = 0;
	m_iNumAirUnits = 0;
	m_iNumAntiAirUnits = 0;
	m_iNumMeleeLandUnits = 0;
	m_iNumNavalUnits = 0;
	m_iNumLandUnitsInArmies = 0;
	m_iNumNavalUnitsInArmies = 0;

	for(pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{

		if(pLoopUnit->IsCanAttack() && pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE && pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE_SEA)
		{
			if(pLoopUnit->getDomainType() == DOMAIN_LAND)
			{
				m_iNumLandUnits++;

				if(pLoopUnit->getArmyID() != FFreeList::INVALID_INDEX)
				{
					m_iNumLandUnitsInArmies++;
				}

				if(pLoopUnit->IsCanAttackRanged())
				{
					m_iNumRangedLandUnits++;
				}
				else if(pLoopUnit->getExtraIntercept() > 0)
				{

					m_iNumAntiAirUnits++;
				}
				else if(pLoopUnit->getUnitInfo().GetMoves() > 2)
				{
					m_iNumMobileLandUnits++;
				}
				else
				{
					m_iNumMeleeLandUnits++;
				}
			}
			else if(pLoopUnit->getDomainType() == DOMAIN_SEA)
			{
				m_iNumNavalUnits++;

				if(pLoopUnit->getArmyID() != FFreeList::INVALID_INDEX)
				{
					m_iNumNavalUnitsInArmies++;
				}
			}
			else if(pLoopUnit->getDomainType() == DOMAIN_AIR)
			{
				m_iNumAirUnits++;
			}
		}
	}

	float fMultiplier;
	int iNumUnitsWanted = 0;
	bool bNavalMap = false;
	EconomicAIStrategyTypes eStrategyNavalMap = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NAVAL_MAP");
	if (m_pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyNavalMap))
	{
		bNavalMap = true;
	}
	int iFlavorOffense = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE"));
	int iFlavorDefense = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DEFENSE"));


	fMultiplier = (float)0.40 + (((float)(m_pPlayer->GetMilitaryAI()->GetHighestThreat() + iFlavorOffense + iFlavorDefense)) / (float)100.0);




	iNumUnitsWanted = GC.getAI_STRATEGY_DEFEND_MY_LANDS_BASE_UNITS();


	iNumUnitsWanted += (int)(m_pPlayer->getNumCities() *         GC.getAI_STRATEGY_DEFEND_MY_LANDS_UNITS_PER_CITY());
	iNumUnitsWanted += m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_SETTLE, true);

	m_iMandatoryReserveSize = (int)((float)iNumUnitsWanted * fMultiplier);


	int iDifficulty = max(0,GC.getGame().getHandicapInfo().GetID() - 1);
	m_iMandatoryReserveSize += iDifficulty;

	m_iMandatoryReserveSize = max(1,m_iMandatoryReserveSize);


	if(m_pPlayer->isMinorCiv())
	{
		m_iMandatoryReserveSize = max(1,m_iMandatoryReserveSize/2);
		iNumUnitsWanted = 0;
	}
	else
	{
		iNumUnitsWanted = 7;


		bool bConquestGrandStrategy = false;
		AIGrandStrategyTypes eConquestGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST");
		if(eConquestGrandStrategy != NO_AIGRANDSTRATEGY)
		{
			if(m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == eConquestGrandStrategy)
			{
				bConquestGrandStrategy = true;
			}
		}
		if(bConquestGrandStrategy)
		{
			iNumUnitsWanted *= 2;
		}


		iNumUnitsWanted += m_pPlayer->GetDiplomacyAI()->GetBoldness();


		iNumUnitsWanted += iDifficulty;

		iNumUnitsWanted = (int)((float)iNumUnitsWanted * fMultiplier);

		iNumUnitsWanted = max(1,iNumUnitsWanted);
	}

	if (bNavalMap)
	{
		iNumUnitsWanted *= 2;
		iNumUnitsWanted /= 3;
		m_iMandatoryReserveSize *= 2;
		m_iMandatoryReserveSize /= 3;
	}

	m_iRecommendedMilitarySize = m_iMandatoryReserveSize + iNumUnitsWanted;
}


void CvMilitaryAI::UpdateDefenseState()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("UpdateDefenseState, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	int iLandUnitsNotInArmies;
	int iNavalUnitsNotInArmies;


	iLandUnitsNotInArmies = m_iNumLandUnits;
	iNavalUnitsNotInArmies = m_iNumNavalUnits;

	if(iLandUnitsNotInArmies < m_iMandatoryReserveSize)
	{
		m_eLandDefenseState = DEFENSE_STATE_CRITICAL;
	}
	else if(iLandUnitsNotInArmies < m_iRecommendedMilitarySize)
	{
		m_eLandDefenseState = DEFENSE_STATE_NEEDED;
	}
	else if(iLandUnitsNotInArmies < m_iRecommendedMilitarySize * 5 / 4)
	{
		m_eLandDefenseState = DEFENSE_STATE_NEUTRAL;
	}
	else
	{
		m_eLandDefenseState = DEFENSE_STATE_ENOUGH;
	}

	int iNavySize = MilitaryAIHelpers::ComputeRecommendedNavySize(m_pPlayer);

	if(iNavalUnitsNotInArmies <= (iNavySize / 2))
	{
		m_eNavalDefenseState = DEFENSE_STATE_CRITICAL;
	}
	else if(iNavalUnitsNotInArmies <= iNavySize)
	{
		m_eNavalDefenseState = DEFENSE_STATE_NEEDED;
	}
	else if(iNavalUnitsNotInArmies <= iNavySize * 5 / 4)
	{
		m_eNavalDefenseState = DEFENSE_STATE_NEUTRAL;
	}
	else
	{
		m_eNavalDefenseState = DEFENSE_STATE_ENOUGH;
	}
}


void CvMilitaryAI::ScanForBarbarians()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("ScanForBarbarians, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

#ifdef AUI_WARNING_FIXES
	uint iPlotLoop;
#else
	int iPlotLoop;
#endif
	CvPlot* pPlot;

	m_iBarbarianCampCount = 0;
	m_iVisibleBarbarianCount = 0;

	TeamTypes eTeam = m_pPlayer->getTeam();


	for(iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

		if(pPlot->isRevealed(eTeam))
		{
			if(pPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
			{
				m_iBarbarianCampCount++;


				if(pPlot->getOwner() == m_pPlayer->GetID())
				{
					m_iBarbarianCampCount += 9;
				}


				else
				{
					int iTolerableDistance = 10;
					int iCityLoop;
					CvCity *pLoopCity;
					for(pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
					{
						if (pLoopCity)
						{
							int iDist = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pPlot->getX(), pPlot->getY());
							if (iDist < iTolerableDistance)
							{
								m_iBarbarianCampCount += 4;
								break;
							}
						}
					}
				}
			}
		}

		if(pPlot->isVisible(eTeam))
		{
			if(pPlot->plotCount(NULL, -1, -1, BARBARIAN_PLAYER, BARBARIAN_TEAM))
			{
				m_iVisibleBarbarianCount++;
			}
		}
	}
}


void CvMilitaryAI::UpdateThreats()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("UpdateThreats, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	ThreatTypes eMilitaryThreatType;

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;


		if(eLoopPlayer != m_pPlayer->GetID() && m_pDiplomacyAI->IsPlayerValid(eLoopPlayer))
		{
			eMilitaryThreatType = m_pDiplomacyAI->GetMilitaryThreat(eLoopPlayer);
			if(eMilitaryThreatType != m_paeLastTurnMilitaryThreat[eLoopPlayer])
			{

				if(eMilitaryThreatType > m_paeLastTurnMilitaryThreat[eLoopPlayer])
				{
					ThreatIncrease(eMilitaryThreatType, (ThreatTypes)m_paeLastTurnMilitaryThreat[eLoopPlayer]);
				}
				else
				{
					ThreatDecrease(eMilitaryThreatType, (ThreatTypes)m_paeLastTurnMilitaryThreat[eLoopPlayer]);
				}


				m_paeLastTurnMilitaryThreat[eLoopPlayer] = eMilitaryThreatType;
			}
		}
	}
}


void CvMilitaryAI::ThreatIncrease(ThreatTypes eNewThreat, ThreatTypes eOldThreat)
{

	m_iTotalThreatWeight -= GetThreatWeight(eOldThreat);


	m_iTotalThreatWeight += GetThreatWeight(eNewThreat);


}


void CvMilitaryAI::ThreatDecrease(ThreatTypes eNewThreat, ThreatTypes eOldThreat)
{

	m_iTotalThreatWeight -= GetThreatWeight(eOldThreat);


	m_iTotalThreatWeight += GetThreatWeight(eNewThreat);


}


void CvMilitaryAI::UpdateWars()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("UpdateWars, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	WarStateTypes eWarState;

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;


		if(eLoopPlayer != m_pPlayer->GetID() && m_pDiplomacyAI->IsPlayerValid(eLoopPlayer))
		{
			eWarState = m_pDiplomacyAI->GetWarState(eLoopPlayer);
			if(eWarState != m_paeLastTurnWarState[eLoopPlayer])
			{
				WarStateChange(eLoopPlayer, eWarState, (WarStateTypes)m_paeLastTurnWarState[eLoopPlayer]);


				m_paeLastTurnWarState[eLoopPlayer] = eWarState;
			}
		}
	}
}


void CvMilitaryAI::WarStateChange(PlayerTypes ePlayer, WarStateTypes eNewWarState, WarStateTypes eOldWarState)
{
	LogWarStateChange(ePlayer, eNewWarState, eOldWarState);
}


void CvMilitaryAI::UpdateMilitaryStrategies()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("UpdateMilitaryStrategies, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	int iStrategiesLoop = 0;


	for(iStrategiesLoop = 0; iStrategiesLoop < GetMilitaryAIStrategies()->GetNumMilitaryAIStrategies(); iStrategiesLoop++)
	{
		MilitaryAIStrategyTypes eStrategy = (MilitaryAIStrategyTypes) iStrategiesLoop;
		CvMilitaryAIStrategyXMLEntry* pStrategy = GetMilitaryAIStrategies()->GetEntry(iStrategiesLoop);

		if(pStrategy == NULL)
			continue;


		if(GetPlayer()->isMinorCiv() && pStrategy->IsNoMinorCivs())
			continue;


		if(!GetPlayer()->isMinorCiv() && pStrategy->IsOnlyMinorCivs())
			continue;

		bool bTestStrategyStart = true;


		if(IsUsingStrategy(eStrategy))
			bTestStrategyStart = false;

		else
		{

			if(pStrategy->GetTechPrereq() != NO_TECH && !GET_TEAM(GetPlayer()->getTeam()).GetTeamTechs()->HasTech((TechTypes) pStrategy->GetTechPrereq()))
			{
				bTestStrategyStart = false;
			}


			if(bTestStrategyStart && pStrategy->GetTechObsolete() != NO_TECH && GET_TEAM(GetPlayer()->getTeam()).GetTeamTechs()->HasTech((TechTypes) pStrategy->GetTechObsolete()))
			{
				bTestStrategyStart = false;
			}


			if(GC.getGame().getGameTurn() < pStrategy->GetFirstTurnExecuted())
			{
				bTestStrategyStart = false;
			}
		}

		bool bTestStrategyEnd = false;


		if(IsUsingStrategy(eStrategy))
		{
			if(pStrategy->GetCheckTriggerTurnCount() > 0)
			{

				if((GC.getGame().getGameTurn() - GetTurnStrategyAdopted(eStrategy)) % pStrategy->GetCheckTriggerTurnCount() == 0)
					bTestStrategyEnd = true;
			}

			if(bTestStrategyEnd && pStrategy->GetMinimumNumTurnsExecuted() > 0)
			{

				if(GC.getGame().getGameTurn() < GetTurnStrategyAdopted(eStrategy) + pStrategy->GetMinimumNumTurnsExecuted())
					bTestStrategyEnd = false;
			}
		}




		if(bTestStrategyStart || bTestStrategyEnd)
		{
			bool bStrategyShouldBeActive = false;


			if(pStrategy->GetTechObsolete() != NO_TECH && GET_TEAM(GetPlayer()->getTeam()).GetTeamTechs()->HasTech((TechTypes) pStrategy->GetTechObsolete()))
			{
				bStrategyShouldBeActive = false;
			}


			else
			{
				CvString strStrategyName = (CvString) pStrategy->GetType();


				if(strStrategyName == "MILITARYAISTRATEGY_ENOUGH_MILITARY_UNITS")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EnoughMilitaryUnits(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_EMPIRE_DEFENSE")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EmpireDefense(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EmpireDefenseCritical(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_ENOUGH_NAVAL_UNITS")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EnoughNavalUnits(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_NEED_NAVAL_UNITS")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedNavalUnits(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_NEED_NAVAL_UNITS_CRITICAL")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedNavalUnitsCritical(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_WAR_MOBILIZATION")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_WarMobilization(eStrategy, m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_AT_WAR")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_AtWar(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_MINOR_CIV_GENERAL_DEFENSE")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_MinorCivGeneralDefense();
				else if(strStrategyName == "MILITARYAISTRATEGY_MINOR_CIV_THREAT_ELEVATED")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_MinorCivThreatElevated(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_MINOR_CIV_THREAT_CRITICAL")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_MinorCivThreatCritical(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_ERADICATE_BARBARIANS")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EradicateBarbarians(eStrategy, m_pPlayer, m_iBarbarianCampCount, m_iVisibleBarbarianCount);
				else if(strStrategyName == "MILITARYAISTRATEGY_WINNING_WARS")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_WinningWars(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_LOSING_WARS")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_LosingWars(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_NEED_RANGED")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedRangedUnits(m_pPlayer, m_iNumRangedLandUnits, m_iNumMeleeLandUnits);
				else if(strStrategyName == "MILITARYAISTRATEGY_ENOUGH_RANGED")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EnoughRangedUnits(m_pPlayer, m_iNumRangedLandUnits, m_iNumMeleeLandUnits);
				else if(strStrategyName == "MILITARYAISTRATEGY_NEED_RANGED_EARLY")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedRangedDueToEarlySneakAttack(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_NEED_MOBILE")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedMobileUnits(m_pPlayer, m_iNumMobileLandUnits, m_iNumMeleeLandUnits);
				else if(strStrategyName == "MILITARYAISTRATEGY_ENOUGH_MOBILE")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EnoughMobileUnits(m_pPlayer, m_iNumMobileLandUnits, m_iNumMeleeLandUnits);
				else if(strStrategyName == "MILITARYAISTRATEGY_NEED_AIR")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedAirUnits(m_pPlayer, m_iNumAirUnits, m_iNumMeleeLandUnits);
				else if(strStrategyName == "MILITARYAISTRATEGY_ENOUGH_AIR")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EnoughAirUnits(m_pPlayer, m_iNumAirUnits, m_iNumMeleeLandUnits);
				else if(strStrategyName == "MILITARYAISTRATEGY_NEED_NUKE")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedANuke(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_NEED_ANTIAIR")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedAntiAirUnits(m_pPlayer, m_iNumAntiAirUnits, m_iNumMeleeLandUnits);
				else if(strStrategyName == "MILITARYAISTRATEGY_ENOUGH_ANTIAIR")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EnoughAntiAirUnits(m_pPlayer, m_iNumAntiAirUnits, m_iNumMeleeLandUnits);
				else if(strStrategyName == "MILITARYAISTRATEGY_NEED_AIR_CARRIER")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedAirCarriers(m_pPlayer);


				else
				{
					bStrategyShouldBeActive = true;
				}


				ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
				if(pkScriptSystem && bStrategyShouldBeActive)
				{
					CvLuaArgsHandle args;
					args->Push(iStrategiesLoop);
					args->Push(m_pPlayer->GetID());



					bool bResult = false;
					if(LuaSupport::CallTestAll(pkScriptSystem, "MilitaryStrategyCanActivate", args.get(), bResult))
					{

						if(bResult == false)
						{
							bStrategyShouldBeActive = false;
						}
					}
				}
			}


			bool bAdoptOrEndStrategy = false;


			if(bStrategyShouldBeActive)
			{
				if(bTestStrategyStart)
					bAdoptOrEndStrategy = true;
				else if(bTestStrategyEnd)
					bAdoptOrEndStrategy = false;
			}

			else
			{
				if(bTestStrategyStart)
					bAdoptOrEndStrategy = false;
				else if(bTestStrategyEnd)
					bAdoptOrEndStrategy = true;
			}


			if(bAdoptOrEndStrategy)
			{
				int iFlavorLoop;


				if(bTestStrategyStart)
				{
					SetUsingStrategy(eStrategy, true);

					for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
					{
						m_aiTempFlavors[iFlavorLoop] = pStrategy->GetPlayerFlavorValue(iFlavorLoop);
					}

					GetPlayer()->GetFlavorManager()->ChangeFlavors(m_aiTempFlavors, true);

					for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
					{
						m_aiTempFlavors[iFlavorLoop] = pStrategy->GetCityFlavorValue(iFlavorLoop);
					}

					GetPlayer()->GetFlavorManager()->ChangeFlavors(m_aiTempFlavors, false);

					if(pStrategy->RequiresCitySpecializationUpdate())
						GetPlayer()->GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_STRATEGY_NOW_ON);
				}

				else if(bTestStrategyEnd)
				{
					SetUsingStrategy(eStrategy, false);

					for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
					{
						m_aiTempFlavors[iFlavorLoop] = -pStrategy->GetPlayerFlavorValue(iFlavorLoop);
					}

					GetPlayer()->GetFlavorManager()->ChangeFlavors(m_aiTempFlavors, true);

					for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
					{
						m_aiTempFlavors[iFlavorLoop] = -pStrategy->GetCityFlavorValue(iFlavorLoop);
					}

					GetPlayer()->GetFlavorManager()->ChangeFlavors(m_aiTempFlavors, false);

					if(pStrategy->RequiresCitySpecializationUpdate())
						GetPlayer()->GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_STRATEGY_NOW_OFF);
				}
			}
		}
	}
}


void CvMilitaryAI::UpdateOperations()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("UpdateOperations, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	int iOperationID;
	int iNumRequiredSlots;
	int iLandReservesUsed;
	int iFilledSlots;

	int iPlayerLoop;
	PlayerTypes eLoopPlayer;
	WarStateTypes eWarState;
	CvAIOperation* pOperation;

	MilitaryAIStrategyTypes eStrategyBarbs = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_ERADICATE_BARBARIANS");
	MilitaryAIStrategyTypes eStrategyFightAWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
	MilitaryAIStrategyTypes eBuildCriticalDefenses = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL");




	bool bWillingToAcceptRisk = (m_iTotalThreatWeight / 2) < GetBarbarianThreatTotal();
	if(m_pPlayer->GetPlayerTraits()->GetLandBarbarianConversionPercent() > 0)
	{
		bWillingToAcceptRisk = true;
	}


	CvPlayerAI& BarbPlayer = GET_PLAYER(BARBARIAN_PLAYER);
	CvUnit* pLoopUnit = NULL;
	int iBarbLoop;
	for (pLoopUnit = BarbPlayer.firstUnit(&iBarbLoop); pLoopUnit != NULL; pLoopUnit = BarbPlayer.nextUnit(&iBarbLoop))
	{
		if (pLoopUnit->GetOriginalOwner() == m_pPlayer->GetID() && (pLoopUnit->AI_getUnitAIType() == UNITAI_SETTLE || pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER))
		{
			bWillingToAcceptRisk = true;
			break;
		}
	}





	if(!IsUsingStrategy(eStrategyBarbs) || IsUsingStrategy(eStrategyFightAWar) || !bWillingToAcceptRisk)
	{
		bool bFoundOneToDelete = true;
		while(bFoundOneToDelete)
		{
			bFoundOneToDelete = false;
			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_DESTROY_BARBARIAN_CAMP, &iOperationID))
			{
				m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
				bFoundOneToDelete = true;
			}
		}
	}





	if(!IsUsingStrategy(eStrategyFightAWar))
	{
		bool bFoundOneToDelete = true;
		while(bFoundOneToDelete)
		{
			bFoundOneToDelete = false;
			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_BASIC_CITY_ATTACK, &iOperationID))
			{
				m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
				bFoundOneToDelete = true;
			}
			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_PILLAGE_ENEMY, &iOperationID))
			{
				m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
				bFoundOneToDelete = true;
			}
			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_RAPID_RESPONSE, &iOperationID))
			{
				m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
				bFoundOneToDelete = true;
			}
			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_CLOSE_DEFENSE, &iOperationID))
			{
				m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
				bFoundOneToDelete = true;
			}
			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_ATTACK, &iOperationID))
			{
				m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
				bFoundOneToDelete = true;
			}
		}
	}

	else
	{

		for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;


			if(eLoopPlayer != m_pPlayer->GetID() && m_pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
			{


				if(GET_TEAM(m_pPlayer->getTeam()).isForcePeace(GET_PLAYER(eLoopPlayer).getTeam()) || m_pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
				{
					pOperation = GetSneakAttackOperation(eLoopPlayer);
					if(pOperation)
					{
						pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
					}
					pOperation = GetBasicAttackOperation(eLoopPlayer);
					if(pOperation)
					{
						pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
					}
					pOperation = GetShowOfForceOperation(eLoopPlayer);
					if(pOperation)
					{
						pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
					}
					pOperation = GetPureNavalAttackOperation(eLoopPlayer);
					if(pOperation)
					{
						pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
					}
				}

				eWarState = m_pPlayer->GetDiplomacyAI()->GetWarState(eLoopPlayer);
				switch(eWarState)
				{

				case WAR_STATE_NEARLY_WON:
				case WAR_STATE_OFFENSIVE:
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_RAPID_RESPONSE, &iOperationID))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation->GetEnemy() == eLoopPlayer)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_CLOSE_DEFENSE, &iOperationID))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation->GetEnemy() == eLoopPlayer)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					break;


				case WAR_STATE_DEFENSIVE:
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_BASIC_CITY_ATTACK, &iOperationID))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation->GetEnemy() == eLoopPlayer)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_PILLAGE_ENEMY, &iOperationID))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation->GetEnemy() == eLoopPlayer)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_ATTACK, &iOperationID))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation->GetEnemy() == eLoopPlayer)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					break;


				case WAR_STATE_NEARLY_DEFEATED:
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_BASIC_CITY_ATTACK, &iOperationID))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation->GetEnemy() == eLoopPlayer)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_PILLAGE_ENEMY, &iOperationID))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation->GetEnemy() == eLoopPlayer)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_ATTACK, &iOperationID))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation->GetEnemy() == eLoopPlayer)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					break;

				case WAR_STATE_CALM:
				case WAR_STATE_STALEMATE:
				default:
					break;
				}
			}
		}


		CvCity* pLoopCity;
		int iLoop;
		for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			if(pLoopCity->getThreatValue() == 0)
			{
				if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_CLOSE_DEFENSE, &iOperationID, NO_PLAYER, pLoopCity->plot()))
				{
					pOperation = m_pPlayer->getAIOperation(iOperationID);
					pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
				}
			}
		}


		if(m_iTotalThreatWeight <= GC.getAI_MILITARY_THREAT_WEIGHT_MAJOR())
		{
			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_RAPID_RESPONSE, &iOperationID))
			{
				pOperation = m_pPlayer->getAIOperation(iOperationID);
				pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
			}
		}
	}







	if(IsUsingStrategy(eStrategyBarbs) && !IsUsingStrategy(eStrategyFightAWar) && !IsUsingStrategy(eBuildCriticalDefenses) && !m_pPlayer->haveAIOperationOfType(AI_OPERATION_DESTROY_BARBARIAN_CAMP)
	        && bWillingToAcceptRisk)
	{

		m_pPlayer->addAIOperation(AI_OPERATION_DESTROY_BARBARIAN_CAMP);
	}





	if(IsUsingStrategy(eStrategyFightAWar))
	{


		for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;


			if(eLoopPlayer != m_pPlayer->GetID() && m_pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
			{
				bool bLaunchNuke = false;

				if (m_pPlayer->getNumNukeUnits() > 0 && GET_TEAM(m_pPlayer->getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam())) 
				{

					if (m_pPlayer->GetDiplomacyAI()->GetNumTimesNuked(eLoopPlayer) > 0)
					{	
						bLaunchNuke = true;
					}

					else if (GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->GetNumTimesNuked(m_pPlayer->GetID()) > 0)
					{
						bLaunchNuke = true;
					}
					else 
					{
						bool bRollForNuke = false;
						WarProjectionTypes eLastWarProjection = m_pPlayer->GetDiplomacyAI()->GetLastWarProjection(eLoopPlayer);
						WarProjectionTypes eCurrentWarProjection = m_pPlayer->GetDiplomacyAI()->GetWarProjection(eLoopPlayer);
						if (eCurrentWarProjection == WAR_PROJECTION_DESTRUCTION)
						{

							bRollForNuke = true;
						}
						else if (eCurrentWarProjection != WAR_PROJECTION_UNKNOWN && eCurrentWarProjection != NO_WAR_PROJECTION_TYPE && 
							eLastWarProjection != WAR_PROJECTION_UNKNOWN && eLastWarProjection != NO_WAR_PROJECTION_TYPE && 
							eCurrentWarProjection < eLastWarProjection)
						{

							bRollForNuke = true;
						}

						if (bRollForNuke)
						{
							int iFlavorNuke = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_USE_NUKE"));
							int iRoll  = GC.getGame().getJonRandNum(10, "Roll to see if we're going to nuke!");
							int iRoll2 = GC.getGame().getJonRandNum(10, "Second roll to see if we're going to nuke!");
							if (iRoll < iFlavorNuke && iRoll2 < iFlavorNuke)
							{
								bLaunchNuke = true;
							}
						}
					}
				}

				if (bLaunchNuke)
				{
					RequestNukeAttack(eLoopPlayer);
				}

			}
		}


		if (m_pPlayer->GetDiplomacyAI()->GetStateAllWars() != STATE_ALL_WARS_LOSING)
		{
			static CvWeightedVector<CvMilitaryTarget, SAFE_ESTIMATE_NUM_CITIES* 10, true> weightedTargetList;
			weightedTargetList.clear();


			for (iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes)iPlayerLoop;
				if (eLoopPlayer == m_pPlayer->GetID() || !m_pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
				{
					continue;
				}

				eWarState = m_pPlayer->GetDiplomacyAI()->GetWarState(eLoopPlayer);


				if (eWarState == NO_WAR_STATE_TYPE)
				{
					continue;
				}


				if (eWarState == WAR_STATE_DEFENSIVE || eWarState == WAR_STATE_NEARLY_DEFEATED)
				{
					continue;
				}

				bool bRequestAttack = false;

				StrengthTypes eMilitaryStrength = m_pPlayer->GetDiplomacyAI()->GetPlayerMilitaryStrengthComparedToUs(eLoopPlayer);
				TargetValueTypes eTargetType = NO_TARGET_VALUE;

				if(eMilitaryStrength <= STRENGTH_AVERAGE || (GC.getAI_MILITARY_IGNORE_BAD_ODDS() == 1))
				{
					eTargetType = m_pPlayer->GetDiplomacyAI()->GetPlayerTargetValue(eLoopPlayer);
					if(eTargetType > TARGET_VALUE_IMPOSSIBLE)
					{
						bRequestAttack = true;
					}
				}

				if (!bRequestAttack)
				{
					continue;
				}

				CvMilitaryTarget target;
				int iScore;
				target = FindBestAttackTarget(AI_OPERATION_BASIC_CITY_ATTACK, eLoopPlayer, &iScore);
				if(target.m_pTargetCity)
				{
					int iNumUnitsWillingBuild = 1;
					if(target.m_bAttackBySea)
					{
						iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_NAVAL_INVASION, true, &iNumRequiredSlots, &iLandReservesUsed);
						if((iNumRequiredSlots - iFilledSlots) > iNumUnitsWillingBuild || iLandReservesUsed > GetLandReservesAvailable())
						{
							continue;
						}
					}
					else
					{
						iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, (GC.getGame().getHandicapInfo().GetID() > 4 && !(GC.getMap().GetAIMapHint() & 1)) ? MUFORMATION_BIGGER_CITY_ATTACK_FORCE : MUFORMATION_BASIC_CITY_ATTACK_FORCE, false, &iNumRequiredSlots, &iLandReservesUsed);
						if((iNumRequiredSlots - iFilledSlots) > iNumUnitsWillingBuild || iLandReservesUsed > GetLandReservesAvailable())
						{
							continue;
						}
					}
				}
				else
				{
					continue;
				}


				int iAdjustedByTargetValueScore = iScore * eTargetType;
				weightedTargetList.push_back(target, iAdjustedByTargetValueScore);
			}



			if(weightedTargetList.size() != 0)
			{
				weightedTargetList.SortItems();
				LogAttackTargets(AI_OPERATION_BASIC_CITY_ATTACK, m_pPlayer->GetID(), weightedTargetList);

				if(weightedTargetList.GetTotalWeight() > 0)
				{
					RandomNumberDelegate fcn;
					fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
					CvMilitaryTarget chosenTarget = weightedTargetList.ChooseByWeight(&fcn, "Choosing attack target by weight");


					RequestSpecificAttack(chosenTarget, 1);

					LogChosenTarget(AI_OPERATION_BASIC_CITY_ATTACK, m_pPlayer->GetID(), chosenTarget);
				}
			}













































































































		}
	

		for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if(eLoopPlayer != m_pPlayer->GetID() && m_pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
			{
				eWarState = m_pPlayer->GetDiplomacyAI()->GetWarState(eLoopPlayer);


				if (eWarState == NO_WAR_STATE_TYPE)
				{
					continue;
				}


				bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_PURE_NAVAL_CITY_ATTACK, &iOperationID, NO_PLAYER);
				if (!bHasOperationUnderway)
				{
					RequestPureNavalAttack(eLoopPlayer, 1);
				}
			}
		}
	}





	int iFlavorNaval = m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL"));
	int iNumSuperiority = m_pPlayer->numOperationsOfType(AI_OPERATION_NAVAL_SUPERIORITY);
	int iNumBombard = m_pPlayer->numOperationsOfType(AI_OPERATION_NAVAL_BOMBARDMENT);
	int iMaxOperations = iFlavorNaval / 2;
	if(GC.getMap().GetAIMapHint() & 1)
	{
		iMaxOperations *= 2;
	}

	if((iNumSuperiority + iNumBombard) <= iMaxOperations)
	{
		iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_NAVAL_SQUADRON, true, &iNumRequiredSlots);


		if(iFilledSlots >= iNumRequiredSlots)
		{

			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_QUICK_COLONIZE, &iOperationID))
			{
				m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_SUPERIORITY, NO_PLAYER);
				return;
			}
			else if (IsUsingStrategy(eStrategyFightAWar))
			{
				m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_SUPERIORITY, NO_PLAYER);
				return;
			}
		}

		iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_NAVAL_BOMBARDMENT, true, &iNumRequiredSlots);


		if(iFilledSlots >= iNumRequiredSlots)
		{

			if(iNumBombard == 0 && IsUsingStrategy(eStrategyBarbs))
			{
				m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_BOMBARDMENT, NO_PLAYER);
				return;
			}
		}
	}
}



void CvMilitaryAI::MakeEmergencyPurchases()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("MakeEmergencyPurchases, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	CvAIOperation* nextOp;


	MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
	if(!IsUsingStrategy(eStrategyAtWar) || m_pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_WINNING)
	{

		nextOp = m_pPlayer->getFirstAIOperation();
		while(nextOp != NULL)
		{
			if(nextOp->HasOneMoreSlotToFill())
			{

				if(nextOp->BuyFinalUnit())
				{
					break;
				}
			}
			nextOp = m_pPlayer->getNextAIOperation();
		}
	}
}


void CvMilitaryAI::MakeOffensivePurchases()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("MakeOffensivePurchases, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	CvUnit *pUnit;

	if (m_pPlayer->isMinorCiv())
	{
		return;
	}


	MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
	if(!IsUsingStrategy(eStrategyAtWar) || m_pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_WINNING)
	{

		if (m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE")) >= 7 &&
			(GetPercentOfRecommendedMilitarySize() < 100 || m_eNavalDefenseState > DEFENSE_STATE_ENOUGH))
		{

			if (GetArmyBeingBuilt() == NO_ARMY_TYPE)
			{
				CvCity *pCity = m_pPlayer->getCapitalCity();
				if (pCity == NULL)
				{
					return;
				}


				bool bNeedNaval = m_eNavalDefenseState > m_eLandDefenseState;
				if (bNeedNaval)
				{

					if (!pCity->isCoastal())
					{
						CvCity* pLoopCity;
						int iCityLoop;
						bNeedNaval = false;
						for(pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL && !bNeedNaval; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
						{
							if(pLoopCity->isCoastal())
							{
								bNeedNaval = true;
								pCity = pLoopCity;
							}
						}
					}
				}

				if (bNeedNaval)
				{
					pUnit = BuyEmergencyUnit(UNITAI_ASSAULT_SEA, pCity);
					if (!pUnit)
					{
						pUnit = BuyEmergencyUnit(UNITAI_ATTACK_SEA, pCity);
					}
				}
				else
				{
					if (m_iNumMeleeLandUnits <= m_iNumRangedLandUnits)
					{
						pUnit = BuyEmergencyUnit(UNITAI_FAST_ATTACK, pCity);
						if (!pUnit)
						{
							pUnit = BuyEmergencyUnit(UNITAI_ATTACK, pCity);
							if (!pUnit)
							{
								pUnit = BuyEmergencyUnit(UNITAI_DEFENSE, pCity);
							}
						}
					}
					else
					{
						pUnit = BuyEmergencyUnit(UNITAI_CITY_BOMBARD, pCity);
						if (!pUnit)
						{
							pUnit = BuyEmergencyUnit(UNITAI_RANGED, pCity);
						}
					}
				}
			}
		}
	}
}

void CvMilitaryAI::RequestImprovements()
{

}


void CvMilitaryAI::DisbandObsoleteUnits()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("DisbandObsoleteUnits, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	bool bInDeficit = false;
	bool bConquestGrandStrategy = false;
	UnitHandle pNavalUnit;
	UnitHandle pLandUnit;
	int iNavalScore = MAX_INT;
	int iLandScore = MAX_INT;


	if(GetNumberCivsAtWarWith() > 0)
	{
		return;
	}


	if(m_pPlayer->isMinorCiv())
	{
		return;
	}


	EconomicAIStrategyTypes eStrategyLosingMoney = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY");
	bInDeficit = m_pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyLosingMoney);

	int iGoldSpentOnUnits = m_pPlayer->GetTreasury()->GetExpensePerTurnUnitMaintenance();
	int iAverageGoldPerUnit = iGoldSpentOnUnits / (max(1,m_pPlayer->getNumUnits()));


	bInDeficit = bInDeficit || iAverageGoldPerUnit > 5;


	AIGrandStrategyTypes eConquestGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST");
	if(eConquestGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if(m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == eConquestGrandStrategy)
		{
			bConquestGrandStrategy = true;
		}
	}


	if(bInDeficit || (m_eLandDefenseState <= DEFENSE_STATE_NEUTRAL && !bConquestGrandStrategy))
	{
		pLandUnit = FindBestUnitToScrap(true          , false                          , iLandScore);
	}


	if(bInDeficit || (m_eNavalDefenseState <= DEFENSE_STATE_NEUTRAL && !bConquestGrandStrategy))
	{
		pNavalUnit = FindBestUnitToScrap(false          , false                          , iNavalScore);
	}

	if(iLandScore < MAX_INT && (m_eLandDefenseState <= m_eNavalDefenseState || iLandScore <= iNavalScore))
	{
		if(pLandUnit)
		{
			pLandUnit->scrap();
			LogScrapUnit(pLandUnit, bInDeficit, bConquestGrandStrategy);
		}
	}
	else if(iNavalScore < MAX_INT)
	{
		if(pNavalUnit)
		{
			pNavalUnit->scrap();
			LogScrapUnit(pNavalUnit, bInDeficit, bConquestGrandStrategy);
		}
	}
}


bool CvMilitaryAI::IsAttackReady(MultiunitFormationTypes eFormation, AIOperationTypes eOperationType) const
{
	int iOperationID;


	bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(eOperationType, &iOperationID, NO_PLAYER);
	if (bHasOperationUnderway)
	{
		if (m_pPlayer->getAIOperation(iOperationID)->GetOperationState() != AI_OPERATION_STATE_RECRUITING_UNITS)
		{
			return false;
		}
	}

	int iNumRequiredSlots = 0;
	int iLandReservesUsed = 0;
	int iFilledSlots = 0;

	bool bRequiresNavalMoves = false;
	if(eFormation == MUFORMATION_NAVAL_INVASION || eFormation == MUFORMATION_NAVAL_SQUADRON || eFormation == MUFORMATION_CITY_STATE_INVASION || eFormation == MUFORMATION_PURE_NAVAL_CITY_ATTACK)
	{
		bRequiresNavalMoves = true;
	}

	iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, eFormation, bRequiresNavalMoves, &iNumRequiredSlots, &iLandReservesUsed);
	if(iFilledSlots >= iNumRequiredSlots)
	{
		return true;
	}
	else
	{
		return false;
	}
}


UnitHandle CvMilitaryAI::FindBestUnitToScrap(bool bLand, bool bDeficitForcedDisband, int& iReturnedScore)
{
	CvUnit* pLoopUnit;
	int iUnitLoop;
	UnitHandle pBestUnit;
	int iScore;
	int iBestScore = MAX_INT;

	for(pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
	{
		bool bSkipThisOne = false;

		if(!pLoopUnit->IsCombatUnit())
		{
			continue;
		}

		if(bLand && pLoopUnit->getDomainType() != DOMAIN_LAND)
		{
			continue;
		}

		if(!bLand && pLoopUnit->getDomainType() != DOMAIN_SEA)
		{
			continue;
		}


		if(!bDeficitForcedDisband)
		{
			if(bLand && m_eLandDefenseState == DEFENSE_STATE_CRITICAL)
			{
				continue;
			}
			else if(!bLand && m_eNavalDefenseState == DEFENSE_STATE_CRITICAL)
			{
				continue;
			}


			if(pLoopUnit->getArmyID() != FFreeList::INVALID_INDEX)
			{
				continue;
			}


			if(bLand && m_pPlayer->canTrain(pLoopUnit->getUnitType(), false              , false                 , true                ))
			{
				continue;
			}


			CvUnitEntry& pUnitInfo = pLoopUnit->getUnitInfo();
			if((TechTypes)pUnitInfo.GetObsoleteTech() == NO_TECH)
			{
				continue;
			}

			if(!GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech((TechTypes)(pUnitInfo.GetObsoleteTech())))
			{
				continue;
			}


			if((pLoopUnit->getUnitInfo().GetPower() * 2) >= GetPowerOfStrongestBuildableUnit(pLoopUnit->getDomainType()))
			{
				continue;
			}


			UnitTypes eUpgradeUnit = pLoopUnit->GetUpgradeUnitType();
			if(eUpgradeUnit != NO_UNIT)
			{
				CvUnitEntry* pUpgradeUnitInfo = GC.GetGameUnits()->GetEntry(eUpgradeUnit);
				if(pUpgradeUnitInfo != NULL)
				{
#ifdef AUI_WARNING_FIXES
					for(uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos() && !bSkipThisOne; iResourceLoop++)
#else
					for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos() && !bSkipThisOne; iResourceLoop++)
#endif
					{
						ResourceTypes eResource = (ResourceTypes) iResourceLoop;
						int iNumResourceNeeded = pUpgradeUnitInfo->GetResourceQuantityRequirement(eResource);

						if(iNumResourceNeeded > 0)
						{
							if(m_pPlayer->getNumResourceTotal(eResource) > 0)
							{
								if(bLand && m_eLandDefenseState > DEFENSE_STATE_NEUTRAL)
								{
									bSkipThisOne  = true;
								}
								else if(!bLand && m_eNavalDefenseState > DEFENSE_STATE_NEUTRAL)
								{
									bSkipThisOne  = true;
								}
							}
						}
					}
				}
			}
		}


		if(!bSkipThisOne && pLoopUnit->canScrap())
		{
			iScore = pLoopUnit->GetPower();

			if(iScore < iBestScore)
			{
				iBestScore = iScore;
				iReturnedScore = iBestScore;
				pBestUnit = pLoopUnit;
			}
		}
	}

	return pBestUnit;
}


UnitTypes CvMilitaryAI::GetUnitForArmy(CvCity* pCity) const
{
	if(m_eArmyTypeBeingBuilt == NO_ARMY_TYPE)
	{
		return NO_UNIT;
	}


	MultiunitFormationTypes eFormation;
	if(m_eArmyTypeBeingBuilt == ARMY_TYPE_NAVAL_INVASION)
	{
		eFormation = MUFORMATION_NAVAL_INVASION;
	}
	else
	{
		eFormation = (GC.getGame().getHandicapInfo().GetID() > 4 && !(GC.getMap().GetAIMapHint() & 1)) ? MUFORMATION_BIGGER_CITY_ATTACK_FORCE : MUFORMATION_BASIC_CITY_ATTACK_FORCE;
	}
	UnitAITypes eUnitAIType = MilitaryAIHelpers::FirstSlotCityCanFill(m_pPlayer, eFormation, (m_eArmyTypeBeingBuilt == ARMY_TYPE_NAVAL_INVASION), pCity->isCoastal(), false                   );
	UnitTypes eType = pCity->GetCityStrategyAI()->GetUnitProductionAI()->RecommendUnit(eUnitAIType);
	if(eType == NO_UNIT)
	{
		eUnitAIType = MilitaryAIHelpers::FirstSlotCityCanFill(m_pPlayer, eFormation, (m_eArmyTypeBeingBuilt == ARMY_TYPE_NAVAL_INVASION), pCity->isCoastal(), true                   );
		eType = pCity->GetCityStrategyAI()->GetUnitProductionAI()->RecommendUnit(eUnitAIType);
	}
	return eType;
}


bool CvMilitaryAI::WillAirUnitRebase(CvUnit* pUnit) const
{
	CvPlot* pUnitPlot = pUnit->plot();

	CvPlot* pBestPlot = NULL;


	bool bNeedsToMove = false;
	if (pUnitPlot->isCity())
	{
		if (pUnitPlot->getPlotCity()->getDamage() > (pUnitPlot->getPlotCity()->GetMaxHitPoints() / 5))
		{
			bNeedsToMove = true;
		}
	}
	else
	{
		CvUnit *pCarrier = pUnit->getTransportUnit();
		if (pCarrier)
		{
			if (pCarrier->getDamage() > (GC.getMAX_HIT_POINTS() / 5))
			{
				bNeedsToMove = true;
			}
		}
	}


	if (pUnit->canAirPatrol(NULL) || pUnit->canAirSweep())
	{
		int iNumNearbyEnemyAirUnits = GetNumEnemyAirUnitsInRange(pUnitPlot, pUnit->GetRange(), true                   , true                  );
		if (iNumNearbyEnemyAirUnits == 0 && !GetBestAirSweepTarget(pUnit))
		{
			bNeedsToMove = true;
		}
	}

	if (!bNeedsToMove)
	{
		return false;
	}


	int iLoopUnit = 0;
	for(CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoopUnit))
	{
		CvPlot* pLoopUnitPlot = pLoopUnit->plot();

		if(pLoopUnit->getDamage() > (GC.getMAX_HIT_POINTS() / 5))
		{
			continue;
		}

		if (pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX)
		{
			continue;
		}

		if(pBestPlot != pUnitPlot && !pUnit->canRebaseAt(pUnitPlot, pLoopUnitPlot->getX(),pLoopUnitPlot->getY()))
		{
			continue;
		}

		if(!pUnit->canLoadUnit(*pLoopUnit, *pLoopUnitPlot))
		{
			continue;
		}
		

		return true;
	}


	for(CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoopUnit))
	{
		CvPlot* pLoopUnitPlot = pLoopUnit->plot();

		if(pLoopUnit->getDamage() > (GC.getMAX_HIT_POINTS() / 5))
		{
			continue;
		}

		if (pLoopUnit->getArmyID() != FFreeList::INVALID_INDEX)
		{
			continue;
		}

		if(pBestPlot != pUnitPlot && !pUnit->canRebaseAt(pUnitPlot, pLoopUnitPlot->getX(),pLoopUnitPlot->getY()))
		{
			continue;
		}

		if(!pUnit->canLoadUnit(*pLoopUnit, *pLoopUnitPlot))
		{
			continue;
		}


		return true;
	}

	CvCity* pLoopCity;
	int iLoopCity = 0;
	for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
	{
		CvPlot* pTarget = pLoopCity->plot();

#ifdef AUI_WARNING_FIXES
		if (!pTarget)
			continue;
#endif

		if(pLoopCity->getDamage() > (pLoopCity->GetMaxHitPoints() / 5))
		{
			continue;
		}

		if (pTarget != pUnitPlot && !pUnit->canRebaseAt(pUnitPlot, pTarget->getX(),pTarget->getY()))
		{
			continue;
		}


		return true;
	}

	return false;
}


int CvMilitaryAI::GetNumEnemyAirUnitsInRange(CvPlot* pCenterPlot, int           , bool bCountFighters, bool bCountBombers) const
{
	int iRtnValue = 0;


	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive() && kPlayer.GetID() != m_pPlayer->GetID())
		{
			if (atWar(kPlayer.getTeam(), m_pPlayer->getTeam()))
			{

				int iLoopUnit = 0;
				for(CvUnit* pLoopUnit = kPlayer.firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoopUnit))
				{
					if (pLoopUnit->getDomainType() == DOMAIN_AIR)
					{
						if ( plotDistance(pCenterPlot->getX(), pCenterPlot->getY(), pLoopUnit->getX(), pLoopUnit->getY()) <= 10 )
						{

							if (pLoopUnit->IsAirSweepCapable() || pLoopUnit->canAirDefend())
							{
								if (bCountFighters) iRtnValue++;
							}
							else
							{
								if (bCountBombers) iRtnValue++;
							}
						}
					}
				}




























			}
		}
	}

	return iRtnValue;
}


CvPlot *CvMilitaryAI::GetBestAirSweepTarget(CvUnit* pFighter) const
{
	CvPlot *pBestTarget = NULL;
	int iBestCount = 0;


	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive() && kPlayer.GetID() != m_pPlayer->GetID())
		{
			if (atWar(kPlayer.getTeam(), m_pPlayer->getTeam()))
			{

				int iLoop;
				CvCity* pLoopCity;
				for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
				{
					CvPlot *pCityPlot = pLoopCity->plot();
					if (pCityPlot->isVisible(m_pPlayer->getTeam()) && plotDistance(pFighter->getX(), pFighter->getY(), pLoopCity->getX(), pLoopCity->getY()) <= pFighter->GetRange())
					{
						if (pFighter->canAirSweepAt(pCityPlot->getX(), pCityPlot->getY()))
						{
							int iCountFighters = 0;

#ifdef AUI_WARNING_FIXES
							for (uint iUnitLoop = 0; iUnitLoop < pCityPlot->getNumUnits(); iUnitLoop++)
#else
							for (int iUnitLoop = 0; iUnitLoop < pCityPlot->getNumUnits(); iUnitLoop++)
#endif
							{
								CvUnit *pUnit = pCityPlot->getUnitByIndex(iUnitLoop);
								{
									if (pUnit->getDomainType() == DOMAIN_AIR)
									{
										if (pUnit->IsAirSweepCapable() || pUnit->canAirDefend())
										{
											iCountFighters++;
										}
									}
								}
							}

							if (iCountFighters > iBestCount)
							{
								iBestCount = iCountFighters;
								pBestTarget = pCityPlot;
							}
						}
					}
				}
			}
		}
	}

	return pBestTarget;
}


int CvMilitaryAI::GetPowerOfStrongestBuildableUnit(DomainTypes eDomain)
{
	int iRtnValue = 0;
#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumUnitInfos(); iI++)
#else
	for(int iI = 0; iI < GC.getNumUnitInfos(); iI++)
#endif
	{
		const UnitTypes eUnit = static_cast<UnitTypes>(iI);
		CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eUnit);
		if(pkUnitEntry != NULL && pkUnitEntry->GetDomainType() == eDomain)
		{
			int iThisPower = pkUnitEntry->GetPower();
			if(iThisPower > iRtnValue)
			{
				if(m_pPlayer->canTrain(eUnit, false              , false                 , true                ))
				{
					iRtnValue = iThisPower;
				}
			}
		}
	}

	return iRtnValue;
}


void CvMilitaryAI::LogStrategy(MilitaryAIStrategyTypes eStrategy, bool bValue)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strLogName;
		FILogFile* pLog;


		playerName = GetPlayer()->getCivilizationShortDescription();
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "FlavorAILog_" + playerName + ".csv";
		}
		else
		{
			strLogName = "FlavorAILog.csv";
		}
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";


		CvMilitaryAIStrategyXMLEntry* pEntry = GC.getMilitaryAIStrategyInfo(eStrategy);
		const char* szType = (pEntry != NULL)? pEntry->GetType() : NULL;

		strTemp.Format("%s, %d", szType, bValue);
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);



		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "TechAILog_" + playerName + ".csv";
			pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);
			pLog->Msg(strOutBuf);

			CvString cityName;

			CvCity* pLoopCity;
			int iLoop;

			for(pLoopCity = GetPlayer()->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iLoop))
			{
				cityName = pLoopCity->getName();
				strLogName= "CityStrategyAILog_" + playerName + "_" + cityName + ".csv";
				pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);
				pLog->Msg(strOutBuf);
			}
		}
		else
		{
			pLog = LOGFILEMGR.GetLog("CityStrategyAILog.csv", FILogFile::kDontTimeStamp);
			pLog->Msg(strOutBuf);
			pLog = LOGFILEMGR.GetLog("TechAILog.csv", FILogFile::kDontTimeStamp);
			pLog->Msg(strOutBuf);
		}
	}
}


void CvMilitaryAI::LogMilitaryStatus()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvCity* pCity;
		CvString cityName;
		FILogFile* pLog;


		playerName = GetPlayer()->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);


		if(GC.getGame().getGameTurn() == 1 && m_pPlayer->GetID() == 0)
		{
			strTemp.Format("Turn, Player, Cities, Settlers, Civ Threat, Barb Threat, Land Units, Land In Armies, Rec Land Size, Land Reserve, Naval Units, Naval In Armies, Rec Naval Size, Most Threatened, Danger");
			pLog->Msg(strTemp);
		}


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";


		strTemp.Format("%d, %d, ", m_pPlayer->getNumCities(), m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_SETTLE, true));
		strOutBuf = strBaseString + strTemp;


		strTemp.Format("%d, %d, ", m_iTotalThreatWeight, GetBarbarianThreatTotal());
		strOutBuf += strTemp;


		int iNavalSize = MilitaryAIHelpers::ComputeRecommendedNavySize(m_pPlayer);
		strTemp.Format("%d, %d, %d, %d, %d, %d, %d, ", m_iNumLandUnits, m_iNumLandUnitsInArmies, m_iRecommendedMilitarySize, m_iMandatoryReserveSize, m_iNumNavalUnits, m_iNumNavalUnitsInArmies, iNavalSize);
		strOutBuf += strTemp;


		pCity = GetMostThreatenedCity();
		if(pCity != NULL)
		{
			cityName = pCity->getName();
			strOutBuf += cityName;
			strTemp.Format(", %d", pCity->getThreatValue());
			strOutBuf += strTemp;
		}

		pLog->Msg(strOutBuf);
	}
}


void CvMilitaryAI::LogAvailableForces()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString strTemp;
		FILogFile* pLog;
		int iCapitalX = 0;
		int iCapitalY = 0;
		CvCity* pCapital = GetPlayer()->getCapitalCity();
		if(pCapital)
		{
			iCapitalX = pCapital->getX();
			iCapitalY = pCapital->getY();
		}


		playerName = GetPlayer()->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		if(m_eArmyTypeBeingBuilt == ARMY_TYPE_LAND)
		{
			strTemp = "Land Army Under Construction";
		}
		else
		{
			strTemp = "Naval Invasion Under Construction";
		}
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);


		CvUnit* pLoopUnit;
		int iLoop;
		for(pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
		{

			if(pLoopUnit->TurnProcessed() || pLoopUnit->isDelayedDeath() || pLoopUnit->AI_getUnitAIType() == UNITAI_UNKNOWN ||  pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE || !pLoopUnit->canMove() || pLoopUnit->isHuman())
			{
				continue;
			}


			else if(!pLoopUnit->IsCombatUnit() && !(pLoopUnit->IsGreatGeneral() || pLoopUnit->IsGreatAdmiral()))
			{
				continue;
			}


			else if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn())
			{
				continue;
			}

			int iArmyID = pLoopUnit->getArmyID();
			int iDist = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), iCapitalX, iCapitalY);
			strTemp.Format("%s, Damage=%d, ArmyID=%d, X=%d, Y=%d, DistFromCap=%d", pLoopUnit->getName().GetCString(), pLoopUnit->getDamage(), iArmyID, pLoopUnit->getX(), pLoopUnit->getY(), iDist);
			strOutBuf = strBaseString + strTemp;

			pLog->Msg(strOutBuf);
		}
	}
}


void CvMilitaryAI::LogWarStateChange(PlayerTypes ePlayer, WarStateTypes eNewWarState, WarStateTypes eOldWarState)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString opponentName;
		FILogFile* pLog;


		playerName = GetPlayer()->getCivilizationShortDescription();
		opponentName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", vs. " + opponentName + ", New War State: ";


		switch(eNewWarState)
		{
		case WAR_STATE_CALM:
			strTemp.Format("Calm");
			break;
		case WAR_STATE_NEARLY_WON:
			strTemp.Format("Nearly Won");
			break;
		case WAR_STATE_OFFENSIVE:
			strTemp.Format("Offensive");
			break;
		case WAR_STATE_STALEMATE:
			strTemp.Format("Stalemate");
			break;
		case WAR_STATE_DEFENSIVE:
			strTemp.Format("Defensive");
			break;
		case WAR_STATE_NEARLY_DEFEATED:
			strTemp.Format("Nearly Defeated");
			break;
		default:
			strTemp.Format("XXX");
			break;
		}
		strOutBuf = strBaseString + strTemp + ", Old War State: ";


		switch(eOldWarState)
		{
		case WAR_STATE_CALM:
			strTemp.Format("Calm");
			break;
		case WAR_STATE_NEARLY_WON:
			strTemp.Format("Nearly Won");
			break;
		case WAR_STATE_OFFENSIVE:
			strTemp.Format("Offensive");
			break;
		case WAR_STATE_STALEMATE:
			strTemp.Format("Stalemate");
			break;
		case WAR_STATE_DEFENSIVE:
			strTemp.Format("Defensive");
			break;
		case WAR_STATE_NEARLY_DEFEATED:
			strTemp.Format("Nearly Defeated");
			break;
		default:
			strTemp.Format("XXX");
			break;
		}
		strOutBuf += strTemp;
		pLog->Msg(strOutBuf);
	}
}


void CvMilitaryAI::LogScrapUnit(UnitHandle pUnit, bool bDeficit, bool bConquest)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strTemp;
		CvString playerName;
		FILogFile* pLog;


		playerName = m_pPlayer->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		strOutBuf.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strOutBuf += playerName + ", ";

		strTemp.Format("Scrapping %s, X: %d, Y: %d, ", pUnit->getUnitInfo().GetDescription(), pUnit->getX(), pUnit->getY());
		strOutBuf += strTemp;
		if(bDeficit)
		{
			strOutBuf += "IN DEFICIT, ";
		}
		else
		{
			strOutBuf += "Finances ok, ";
		}
		if(bConquest)
		{
			strOutBuf += "CONQUEST, ";
		}
		else
		{
			strOutBuf += "Other GS, ";
		}
		if(pUnit->getDomainType() == DOMAIN_LAND)
		{
			strTemp.Format("Num Land Units: %d, In Armies %d, Rec Size: %d, ", m_iNumLandUnits, m_iNumLandUnitsInArmies, m_iRecommendedMilitarySize);
		}
		else
		{
			strTemp.Format("Num Naval Units: %d, In Armies %d, ", m_iNumNavalUnits, m_iNumNavalUnitsInArmies);
		}
		strOutBuf += strTemp;
		pLog->Msg(strOutBuf);
	}
}


void CvMilitaryAI::LogMilitarySummaryMessage(const CvString& strMsg)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strPlayerName = m_pPlayer->getCivilizationShortDescription();
		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName, true            ), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";
		strOutBuf = strBaseString + strMsg;
		pLog->Msg(strOutBuf);
	}
}


CvString CvMilitaryAI::GetLogFileName(CvString& playerName, bool bSummary) const
{
	CvString strLogName;

	if (bSummary)
	{
		strLogName = "MilitarySummary.csv";
	}
	else
	{
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "MilitaryAILog_" + playerName + ".csv";
		}
		else
		{
			strLogName = "MilitaryAILog.csv";
		}
	}

	return strLogName;
}









int MilitaryAIHelpers::GetWeightThresholdModifier(MilitaryAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	int iWeightThresholdModifier = 0;

	CvMilitaryAIStrategyXMLEntries* pkAIStrategies = pPlayer->GetMilitaryAI()->GetMilitaryAIStrategies();
	CvFlavorManager* pkFlavorManager = pPlayer->GetFlavorManager();

	for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		FlavorTypes eFlavor = (FlavorTypes) iFlavorLoop;
		int iPersonalityFlavor = pkFlavorManager->GetPersonalityIndividualFlavor(eFlavor);
		CvMilitaryAIStrategyXMLEntry* pkEntry = pkAIStrategies->GetEntry(eStrategy);
		CvAssert(pkEntry != NULL);
		if(pkEntry)
		{
			int iStrategyFlavorMod = pkEntry->GetPersonalityFlavorThresholdMod(eFlavor);

			iWeightThresholdModifier += (iPersonalityFlavor * iStrategyFlavorMod);
		}
	}

	return iWeightThresholdModifier;
}


bool MilitaryAIHelpers::IsTestStrategy_EnoughMilitaryUnits(CvPlayer* pPlayer)
{

	EconomicAIStrategyTypes eStrategyLosingMoney = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY");
	bool bInDeficit = pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyLosingMoney);


	AIGrandStrategyTypes eConquestGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST");
	if(eConquestGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if(bInDeficit || pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() != eConquestGrandStrategy || pPlayer->GetMilitaryAI()->GetPercentOfRecommendedMilitarySize() > 125)
		{
			if(pPlayer->GetMilitaryAI()->GetLandDefenseState() == DEFENSE_STATE_ENOUGH)
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}

	return false;
}


bool MilitaryAIHelpers::IsTestStrategy_EmpireDefense(CvPlayer* pPlayer)
{
	if(pPlayer->GetMilitaryAI()->GetLandDefenseState() == DEFENSE_STATE_NEEDED)
	{
		return true;
	}

	return false;
}


bool MilitaryAIHelpers::IsTestStrategy_EmpireDefenseCritical(CvPlayer* pPlayer)
{
	if(pPlayer->GetMilitaryAI()->GetLandDefenseState() == DEFENSE_STATE_CRITICAL)
	{
		return true;
	}

	return false;
}


bool MilitaryAIHelpers::IsTestStrategy_EnoughNavalUnits(CvPlayer* pPlayer)
{
	if(pPlayer->GetMilitaryAI()->GetNavalDefenseState() == DEFENSE_STATE_ENOUGH)
	{
		return true;
	}

	return false;
}


bool MilitaryAIHelpers::IsTestStrategy_NeedNavalUnits(CvPlayer* pPlayer)
{
	if(pPlayer->GetMilitaryAI()->GetNavalDefenseState() == DEFENSE_STATE_NEEDED)
	{
		return true;
	}

	return false;
}


bool MilitaryAIHelpers::IsTestStrategy_NeedNavalUnitsCritical(CvPlayer* pPlayer)
{
	if(pPlayer->GetMilitaryAI()->GetNavalDefenseState() == DEFENSE_STATE_CRITICAL)
	{
		return true;
	}

	return false;
}


bool MilitaryAIHelpers::IsTestStrategy_WarMobilization(MilitaryAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	int iCurrentWeight = 0;
	PlayerTypes eOtherPlayer;


	MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");

	if(eStrategyAtWar != NO_MILITARYAISTRATEGY)
	{
		if(pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyAtWar))
		{
			return false;
		}
	}


	AIGrandStrategyTypes eConquestGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST");
	if(eConquestGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if(pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == eConquestGrandStrategy)
		{
			iCurrentWeight += 25;
		}
	}

	CvDiplomacyAI* pkDiplomacyAI = pPlayer->GetDiplomacyAI();
	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eOtherPlayer = (PlayerTypes) iMajorLoop;


		if(pkDiplomacyAI->GetWarGoal(eOtherPlayer) == WAR_GOAL_PREPARE || pPlayer->GetMilitaryAI()->GetArmyBeingBuilt() != NO_ARMY_TYPE)
		{
			iCurrentWeight += 100;
		}


		else if(pkDiplomacyAI->GetMajorCivApproach(eOtherPlayer,                       false) == MAJOR_CIV_APPROACH_WAR ||
		        pkDiplomacyAI->GetMajorCivApproach(eOtherPlayer,                       false) == MAJOR_CIV_APPROACH_HOSTILE ||
		        pkDiplomacyAI->GetMajorCivApproach(eOtherPlayer,                       false) == MAJOR_CIV_APPROACH_AFRAID)
		{
			iCurrentWeight += 50;
		}



		else if(pkDiplomacyAI->GetMajorCivApproach(eOtherPlayer,                       false) == MAJOR_CIV_APPROACH_GUARDED ||
		        pkDiplomacyAI->GetMajorCivApproach(eOtherPlayer,                       false) == MAJOR_CIV_APPROACH_DECEPTIVE)
		{
			iCurrentWeight += 25;
		}
	}

	CvMilitaryAIStrategyXMLEntry* pStrategy = pPlayer->GetMilitaryAI()->GetMilitaryAIStrategies()->GetEntry(eStrategy);
	CvAssert(pStrategy != NULL);
	if(pStrategy)
	{
		int iWeightThreshold = pStrategy->GetWeightThreshold();
		if(iCurrentWeight >= iWeightThreshold)
		{
			return true;
		}
	}

	return false;
}


bool MilitaryAIHelpers::IsTestStrategy_AtWar(CvPlayer* pPlayer)
{
	return (pPlayer->GetMilitaryAI()->GetNumberCivsAtWarWith() > 0);
}


bool MilitaryAIHelpers::IsTestStrategy_MinorCivGeneralDefense()
{
	return true;
}


bool MilitaryAIHelpers::IsTestStrategy_MinorCivThreatElevated(CvPlayer* pPlayer)
{
	if(pPlayer->GetMinorCivAI()->GetStatus() == MINOR_CIV_STATUS_ELEVATED)
	{
		return true;
	}

	return false;
}


bool MilitaryAIHelpers::IsTestStrategy_MinorCivThreatCritical(CvPlayer* pPlayer)
{
	if(pPlayer->GetMinorCivAI()->GetStatus() == MINOR_CIV_STATUS_CRITICAL)
	{
		return true;
	}

	return false;
}


bool MilitaryAIHelpers::IsTestStrategy_EradicateBarbarians(MilitaryAIStrategyTypes eStrategy, CvPlayer* pPlayer, int iBarbarianCampCount, int iVisibleBarbarianCount)
{
	int iStrategyWeight;
	PlayerTypes eOtherPlayer;


	MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
	if(eStrategyAtWar != NO_MILITARYAISTRATEGY)
	{
		if(pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyAtWar))
		{
			if(!pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_WINNING)
			{
				return false;
			}
		}
	}


	CvPlayerAI& BarbPlayer = GET_PLAYER(BARBARIAN_PLAYER);
	CvUnit* pLoopUnit = NULL;
	int iLoop;
	for (pLoopUnit = BarbPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = BarbPlayer.nextUnit(&iLoop))
	{
		if (pLoopUnit->GetOriginalOwner() == pPlayer->GetID() && (pLoopUnit->AI_getUnitAIType() == UNITAI_SETTLE || pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER))
		{
			return true;
		}
	}

	CvMilitaryAIStrategyXMLEntry* pStrategy = pPlayer->GetMilitaryAI()->GetMilitaryAIStrategies()->GetEntry(eStrategy);
	CvAssert(pStrategy != NULL);
	if(pStrategy)
	{
		iStrategyWeight = iBarbarianCampCount * 50 + iVisibleBarbarianCount * 25;
		int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pPlayer);
		int iWeightThreshold = pStrategy->GetWeightThreshold() + iWeightThresholdModifier;

		if(iStrategyWeight >= iWeightThreshold * 2 && iVisibleBarbarianCount > 0)
		{
			return true;
		}
		else if(iStrategyWeight >= iWeightThreshold)
		{

			CvDiplomacyAI* pkDiplomacyAI = pPlayer->GetDiplomacyAI();
			for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
			{
				eOtherPlayer = (PlayerTypes) iMajorLoop;
				if(pkDiplomacyAI->GetWarGoal(eOtherPlayer) == WAR_GOAL_PREPARE)
				{
					return false;
				}
			}

			return true;
		}

		else if(pPlayer->haveAIOperationOfType(AI_OPERATION_DESTROY_BARBARIAN_CAMP))
		{
			return true;
		}
	}

	return false;
}


bool MilitaryAIHelpers::IsTestStrategy_WinningWars(CvPlayer* pPlayer)
{
	if(pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_WINNING)
	{
		return true;
	}
	return false;
}


bool MilitaryAIHelpers::IsTestStrategy_LosingWars(CvPlayer* pPlayer)
{
	if(pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
	{
		return true;
	}
	return false;
}


bool MilitaryAIHelpers::IsTestStrategy_EnoughRangedUnits(CvPlayer* pPlayer, int iNumRanged, int iNumMelee)
{
	int iFlavorRange = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RANGED"));
	int iRatio = iNumRanged * 10 / max(1,iNumMelee+iNumRanged);
	return (iRatio >= iFlavorRange);
}


bool MilitaryAIHelpers::IsTestStrategy_NeedRangedUnits(CvPlayer* pPlayer, int iNumRanged, int iNumMelee)
{
	int iFlavorRange = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RANGED"));
	int iRatio = iNumRanged * 10 / max(1,iNumMelee+iNumRanged);
	return (iRatio <= iFlavorRange / 2);
}


bool MilitaryAIHelpers::IsTestStrategy_NeedRangedDueToEarlySneakAttack(CvPlayer* pPlayer)
{
	MilitaryAIStrategyTypes eStrategyWarMob = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_WAR_MOBILIZATION");
	if(eStrategyWarMob != NO_MILITARYAISTRATEGY)
	{
		if(pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyWarMob))
		{
			return true;
		}
	}
	return false;
}


bool MilitaryAIHelpers::IsTestStrategy_EnoughMobileUnits(CvPlayer* pPlayer, int iNumMobile, int iNumMelee)
{
	int iFlavorMobile = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_MOBILE"));
	int iRatio = iNumMobile * 10 / max(1,iNumMelee+iNumMobile);
	return (iRatio >= iFlavorMobile);
}


bool MilitaryAIHelpers::IsTestStrategy_NeedMobileUnits(CvPlayer* pPlayer, int iNumMobile, int iNumMelee)
{
	int iFlavorMobile = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_MOBILE"));
	int iRatio = iNumMobile * 10 / max(1,iNumMelee+iNumMobile);
	return (iRatio <= iFlavorMobile / 2);
}


bool MilitaryAIHelpers::IsTestStrategy_EnoughAirUnits(CvPlayer* pPlayer, int iNumAir, int iNumMelee)
{
	int iFlavorAir = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_AIR"));
	int iRatio = iNumAir * 10 / max(1,iNumMelee+iNumAir);
	return (iRatio >= iFlavorAir);
}


bool MilitaryAIHelpers::IsTestStrategy_NeedAirUnits(CvPlayer* pPlayer, int iNumAir, int iNumMelee)
{
	int iFlavorAir = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_AIR"));
	int iRatio = iNumAir * 10 / max(1,iNumMelee+iNumAir);
	return (iRatio <= iFlavorAir / 2);
}


bool MilitaryAIHelpers::IsTestStrategy_NeedANuke(CvPlayer* pPlayer)
{
	if(GC.getGame().isNoNukes())
	{
		return false;
	}

	int iFlavorNuke = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NUKE"));
	int iNumNukes = pPlayer->getNumNukeUnits();

	return (iNumNukes < iFlavorNuke / 3);
}


bool MilitaryAIHelpers::IsTestStrategy_EnoughAntiAirUnits(CvPlayer* pPlayer, int iNumAA, int iNumMelee)
{
	bool bAnyAirforce = false;
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if(eLoopPlayer != pPlayer->GetID() && pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			if(GET_PLAYER(eLoopPlayer).GetMilitaryAI()->HasAirforce())
			{
				bAnyAirforce = true;
				break;
			}
		}
	}

	if(bAnyAirforce)
	{
		int iRatio = (iNumAA * 10) / max(1,iNumMelee+iNumAA);
		return (iRatio > 2);
	}
	else
	{
		return true;
	}
}


bool MilitaryAIHelpers::IsTestStrategy_NeedAntiAirUnits(CvPlayer* pPlayer, int iNumAA, int iNumMelee)
{
	bool bAnyAirforce = false;
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if(eLoopPlayer != pPlayer->GetID() && pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			if(GET_PLAYER(eLoopPlayer).GetMilitaryAI()->HasAirforce())
			{
				bAnyAirforce = true;
				break;
			}
		}
	}

	if(bAnyAirforce)
	{
		int iRatio = (iNumAA * 10) / max(1,iNumMelee+iNumAA);
		return (iRatio <= 2);
	}
	else
	{
		return false;
	}
}


bool MilitaryAIHelpers::IsTestStrategy_NeedAirCarriers(CvPlayer* pPlayer)
{
	int iNumLoadableAirUnits = 0;
	int iNumTotalCargoSpace = 0;
	CvUnit* pLoopUnit;
	int iLoop;
	SpecialUnitTypes eSpecialUnitPlane = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_FIGHTER");
	for(pLoopUnit = pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iLoop))
	{

		if(pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE && pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE_SEA)
		{
			if(pLoopUnit->cargoSpace() > 0)
			{
				if(pLoopUnit->specialCargo() != NO_SPECIALUNIT)
				{
					if(pLoopUnit->specialCargo() != eSpecialUnitPlane)
					{
						continue;
					}
				}

				if (pLoopUnit->domainCargo() != NO_DOMAIN)
				{
					if (pLoopUnit->domainCargo() != DOMAIN_AIR)
					{
						continue;
					}
				}
				iNumTotalCargoSpace += pLoopUnit->cargoSpace();
			}
			else if (pLoopUnit->getSpecialUnitType() == eSpecialUnitPlane)
			{
				iNumLoadableAirUnits += 1;
			}
		}
	}

	if (iNumLoadableAirUnits > iNumTotalCargoSpace)
	{
		return true;
	}
	else
	{
		return false;
	}
}




int MilitaryAIHelpers::ComputeRecommendedNavySize(CvPlayer* pPlayer)
{
	int iNumUnitsWanted = 0;
	int iFlavorNaval = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL"));


	double dMultiplier;


	iNumUnitsWanted = 1;

	int iNumCoastalCities = 0;
	int iLoop;
	CvCity* pCity;
	for(pCity = pPlayer->firstCity(&iLoop); pCity != NULL; pCity = pPlayer->nextCity(&iLoop))
	{
		if(pCity->isCoastal(-1))
		{
			iNumCoastalCities++;
		}
	}

	iNumUnitsWanted += iNumCoastalCities;

	dMultiplier = (double)0.75 + ((double)pPlayer->GetMilitaryAI()->GetHighestThreat() / (double)4.0) + ((double)(iFlavorNaval) / (double)40.0);
	iNumUnitsWanted = (int)((double)iNumUnitsWanted * dMultiplier*          GC.getAI_STRATEGY_NAVAL_UNITS_PER_CITY());

	iNumUnitsWanted = max(1,iNumUnitsWanted);

	EconomicAIStrategyTypes eStrategyNavalMap = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NAVAL_MAP");
	EconomicAIStrategyTypes eExpandOtherContinents = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS");
	if (pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyNavalMap) || pPlayer->GetEconomicAI()->IsUsingStrategy(eExpandOtherContinents))
	{
		iNumUnitsWanted *= 3;
		iNumUnitsWanted /= 2;
	}

	if (pPlayer->getCivilizationInfo().isCoastalCiv())
	{
		iNumUnitsWanted *= 3;
		iNumUnitsWanted /= 2;
	}


	int iGT = GC.getGame().getGameTurn();
	iGT = min(iGT,200);
	AIGrandStrategyTypes eConquestGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST");
	if(eConquestGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if(pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == eConquestGrandStrategy)
		{
			iNumUnitsWanted += (10 * iGT) / 200;
		}
	}

	return iNumUnitsWanted;
}


int MilitaryAIHelpers::NumberOfFillableSlots(CvPlayer* pPlayer, MultiunitFormationTypes formation, bool bRequiresNavalMoves, int* piNumberSlotsRequired, int* piNumberLandReservesUsed)
{
	CvUnit* pLoopUnit;
	int iLoop;
	FStaticVector< CvFormationSlotEntry, 10, false, c_eCiv5GameplayDLL > slotsToFill;
	FStaticVector< CvFormationSlotEntry, 10, false, c_eCiv5GameplayDLL >::iterator it;
	int iWillBeFilled = 0;
	int iLandReservesUsed = 0;

	CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(formation);
	for(int iThisSlotIndex = 0; iThisSlotIndex < thisFormation->getNumFormationSlotEntries(); iThisSlotIndex++)
	{
		const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(iThisSlotIndex);
		slotsToFill.push_back(thisSlotEntry);
	}

	bool bMustBeDeepWaterNaval = GET_TEAM(pPlayer->getTeam()).canEmbarkAllWaterPassage() && thisFormation->IsRequiresNavalUnitConsistency();

	for(pLoopUnit = pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iLoop))
	{

		if(pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE && pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE_SEA)
		{

			if(pLoopUnit->GetCurrHitPoints() >= pLoopUnit->GetMaxHitPoints() * GC.getAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION() / 100)
			{
				if(pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX && pLoopUnit->canRecruitFromTacticalAI())
				{
					if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() < GC.getGame().getGameTurn())
					{
						if(!bRequiresNavalMoves || pLoopUnit->getDomainType() == DOMAIN_SEA || pLoopUnit->CanEverEmbark())
						{
							if (!bMustBeDeepWaterNaval || pLoopUnit->getDomainType() != DOMAIN_SEA || !pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
							{
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
								for (it = slotsToFill.begin(); it != slotsToFill.end(); ++it)
#else
								for(it = slotsToFill.begin(); it != slotsToFill.end(); it++)
#endif
								{
									CvFormationSlotEntry slotEntry = *it;
									CvUnitEntry& kUnitInfo = pLoopUnit->getUnitInfo();
									if(kUnitInfo.GetUnitAIType((UnitAITypes)slotEntry.m_primaryUnitType) ||
										kUnitInfo.GetUnitAIType((UnitAITypes)slotEntry.m_secondaryUnitType))
									{
										slotsToFill.erase(it);
										iWillBeFilled++;

										if(pLoopUnit->getDomainType() == DOMAIN_LAND)
										{
											iLandReservesUsed++;
										}
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}


	if(piNumberSlotsRequired != NULL)
	{
		(*piNumberSlotsRequired) = iWillBeFilled;
		for(int iThisSlotIndex = 0; iThisSlotIndex < (int)slotsToFill.size(); iThisSlotIndex++)
		{
			if(slotsToFill[iThisSlotIndex].m_requiredSlot)
			{
				(*piNumberSlotsRequired)++;
			}
		}
	}

	if(piNumberLandReservesUsed != NULL)
	{
		*piNumberLandReservesUsed = iLandReservesUsed;
	}
	return iWillBeFilled;
}


UnitAITypes MilitaryAIHelpers::FirstSlotCityCanFill(CvPlayer* pPlayer, MultiunitFormationTypes formation, bool bRequiresNavalMoves, bool bAtCoastalCity, bool bSecondaryUnit)
{
	CvUnit* pLoopUnit;
	int iLoop;
	FStaticVector< CvFormationSlotEntry, 10, false, c_eCiv5GameplayDLL > slotsToFill;
	FStaticVector< CvFormationSlotEntry, 10, false, c_eCiv5GameplayDLL >::iterator it;

	CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(formation);
	for(int iThisSlotIndex = 0; iThisSlotIndex < thisFormation->getNumFormationSlotEntries(); iThisSlotIndex++)
	{
		const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(iThisSlotIndex);
		slotsToFill.push_back(thisSlotEntry);
	}

	bool bMustBeDeepWaterNaval = GET_TEAM(pPlayer->getTeam()).canEmbarkAllWaterPassage() && thisFormation->IsRequiresNavalUnitConsistency();

	for(pLoopUnit = pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iLoop))
	{

		if(pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE && pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE_SEA)
		{

			if(pLoopUnit->GetCurrHitPoints() >= pLoopUnit->GetMaxHitPoints() * GC.getAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION() / 100)
			{
				if(pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX && pLoopUnit->canRecruitFromTacticalAI())
				{
					if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() < GC.getGame().getGameTurn())
					{
						if(!bRequiresNavalMoves || pLoopUnit->getDomainType() == DOMAIN_SEA || pLoopUnit->CanEverEmbark())
						{
							if (!bMustBeDeepWaterNaval || pLoopUnit->getDomainType() != DOMAIN_SEA || !pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
							{
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
								for (it = slotsToFill.begin(); it != slotsToFill.end(); ++it)
#else
								for(it = slotsToFill.begin(); it != slotsToFill.end(); it++)
#endif
								{
									CvFormationSlotEntry slotEntry = *it;
									CvUnitEntry& kUnitInfo = pLoopUnit->getUnitInfo();
									if(kUnitInfo.GetUnitAIType((UnitAITypes)slotEntry.m_primaryUnitType) ||
										kUnitInfo.GetUnitAIType((UnitAITypes)slotEntry.m_secondaryUnitType))
									{
										slotsToFill.erase(it);
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}


	if(bAtCoastalCity)
	{
		for(int iThisSlotIndex = 0; iThisSlotIndex < (int)slotsToFill.size(); iThisSlotIndex++)
		{
			if(slotsToFill[iThisSlotIndex].m_requiredSlot)
			{
				UnitAITypes eType = (UnitAITypes)slotsToFill[iThisSlotIndex].m_primaryUnitType;
				if(eType == UNITAI_ASSAULT_SEA || eType == UNITAI_ATTACK_SEA || eType == UNITAI_RESERVE_SEA || eType == UNITAI_ESCORT_SEA)
				{
					if(!bSecondaryUnit)
					{
						return (UnitAITypes)slotsToFill[iThisSlotIndex].m_primaryUnitType;
					}
					else
					{
						return (UnitAITypes)slotsToFill[iThisSlotIndex].m_secondaryUnitType;
					}
				}
			}
		}
	}


	for(int iThisSlotIndex = 0; iThisSlotIndex < (int)slotsToFill.size(); iThisSlotIndex++)
	{
		if(slotsToFill[iThisSlotIndex].m_requiredSlot)
		{
			if(!bSecondaryUnit)
			{
				return (UnitAITypes)slotsToFill[iThisSlotIndex].m_primaryUnitType;
			}
			else
			{
				return (UnitAITypes)slotsToFill[iThisSlotIndex].m_secondaryUnitType;
			}
		}
	}

	return NO_UNITAI;
}
