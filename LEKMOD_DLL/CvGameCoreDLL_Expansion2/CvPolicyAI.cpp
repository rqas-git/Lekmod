






#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "CvPolicyAI.h"
#include "CvGrandStrategyAI.h"
#include "CvInfosSerializationHelper.h"


#include "LintFree.h"


CvPolicyAI::CvPolicyAI(CvPlayerPolicies* currentPolicies):
	m_pCurrentPolicies(currentPolicies)
{
#ifdef AUI_WARNING_FIXES
	m_iPolicyWeightPropagationLevels = 0;
	m_iPolicyWeightPercentDropNewBranch = 0;
#endif	
}


CvPolicyAI::~CvPolicyAI(void)
{
}


void CvPolicyAI::Reset()
{
	m_PolicyAIWeights.clear();
	m_iPolicyWeightPropagationLevels = GC.getPOLICY_WEIGHT_PROPAGATION_LEVELS();
	m_iPolicyWeightPercentDropNewBranch = GC.getPOLICY_WEIGHT_PERCENT_DROP_NEW_BRANCH();

	CvAssertMsg(m_pCurrentPolicies != NULL, "Policy AI init failure: player policy data is NULL");
	if(m_pCurrentPolicies != NULL)
	{
		CvPolicyXMLEntries* pPolicyEntries = m_pCurrentPolicies->GetPolicies();
		CvAssertMsg(pPolicyEntries != NULL, "Policy AI init failure: no policy data");
		if(pPolicyEntries != NULL)
		{

			const int nPolicyEntries = pPolicyEntries->GetNumPolicies();
			for(int i = 0; i < nPolicyEntries; i++)
			{
				m_PolicyAIWeights.push_back(i, 0);
			}
		}
	}
}


void CvPolicyAI::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	int iWeight;

	CvAssertMsg(m_pCurrentPolicies->GetPolicies() != NULL, "Policy AI serialization failure: no policy data");
	CvAssertMsg(m_pCurrentPolicies->GetPolicies()->GetNumPolicies() > 0, "Policy AI serialization failure: number of policies not greater than 0");


	m_PolicyAIWeights.clear();

	uint uiPolicyArraySize = m_pCurrentPolicies->GetPolicies()->GetNumPolicies();

	m_PolicyAIWeights.resize(uiPolicyArraySize);

	for(uint uiIndex = 0; uiIndex < uiPolicyArraySize; ++uiIndex)
		m_PolicyAIWeights.SetWeight(uiIndex, 0);

	uint uiPolicyCount;
	kStream >> uiPolicyCount;

	for(uint uiIndex = 0; uiIndex < uiPolicyCount; ++uiIndex)
	{
		PolicyTypes ePolicy = (PolicyTypes)CvInfosSerializationHelper::ReadHashed(kStream);
		kStream >> iWeight;
		if(ePolicy != NO_POLICY && (uint)ePolicy < uiPolicyArraySize)
			m_PolicyAIWeights.SetWeight((uint)ePolicy, iWeight);
	}
}


void CvPolicyAI::Write(FDataStream& kStream)
{

	uint uiVersion = 1;
	kStream << uiVersion;

	CvAssertMsg(m_pCurrentPolicies->GetPolicies() != NULL, "Policy AI serialization failure: no policy data");
	CvAssertMsg(m_pCurrentPolicies->GetPolicies()->GetNumPolicies() > 0, "Policy AI serialization failure: number of policies not greater than 0");


	uint uiPolicyCount = m_pCurrentPolicies->GetPolicies()->GetNumPolicies();
	kStream << uiPolicyCount;

#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < m_pCurrentPolicies->GetPolicies()->GetNumPolicies(); i++)
#else
	for(int i = 0; i < m_pCurrentPolicies->GetPolicies()->GetNumPolicies(); i++)
#endif
	{
		CvInfosSerializationHelper::WriteHashed(kStream, static_cast<const PolicyTypes>(i));
		kStream << m_PolicyAIWeights.GetWeight(i);
	}
}


void CvPolicyAI::AddFlavorWeights(FlavorTypes eFlavor, int iWeight, int iPropagationPercent)
{
#ifdef AUI_WARNING_FIXES
	uint iPolicy;
#else
	int iPolicy;
#endif
	CvPolicyEntry* entry;
#ifndef AUI_WARNING_FIXES
	int* paiTempWeights;
#endif

	CvPolicyXMLEntries* pkPolicyEntries = m_pCurrentPolicies->GetPolicies();

#ifdef AUI_WARNING_FIXES
	FFastVector<int, true> paiTempWeights;
	paiTempWeights.reserve(pkPolicyEntries->GetNumPolicies());
#else
	paiTempWeights = (int*)_alloca(sizeof(int*) * pkPolicyEntries->GetNumPolicies());
#endif


	for(iPolicy = 0; iPolicy < pkPolicyEntries->GetNumPolicies(); iPolicy++)
	{
		entry = pkPolicyEntries->GetPolicyEntry(iPolicy);


		if(entry)
#ifdef AUI_WARNING_FIXES
			paiTempWeights.push_back(entry->GetFlavorValue(eFlavor) * iWeight);
		else
			paiTempWeights.push_back(0);
#else
			paiTempWeights[iPolicy] = entry->GetFlavorValue(eFlavor) * iWeight;
		else
			paiTempWeights[iPolicy] = 0;
#endif
	}


	if(iPropagationPercent > 0)
	{
		WeightPrereqs(paiTempWeights, iPropagationPercent);
	}


	for(iPolicy = 0; iPolicy < m_pCurrentPolicies->GetPolicies()->GetNumPolicies(); iPolicy++)
	{
		m_PolicyAIWeights.IncreaseWeight(iPolicy, paiTempWeights[iPolicy]);
	}
}


#ifdef AUI_WARNING_FIXES
uint CvPolicyAI::ChooseNextPolicy(CvPlayer* pPlayer)
#else
int CvPolicyAI::ChooseNextPolicy(CvPlayer* pPlayer)
#endif
{
	RandomNumberDelegate fcn;
	fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
#ifdef AUI_WARNING_FIXES
	uint iRtnValue = (uint)NO_POLICY;
	uint iPolicyLoop;
#else
	int iRtnValue = (int)NO_POLICY;
	int iPolicyLoop;
#endif
	vector<int> aLevel3Tenets;

	bool bMustChooseTenet = (pPlayer->GetNumFreeTenets() > 0);


	m_AdoptablePolicies.clear();


	for(iPolicyLoop = 0; iPolicyLoop < m_pCurrentPolicies->GetPolicies()->GetNumPolicies(); iPolicyLoop++)
	{
		if(m_pCurrentPolicies->CanAdoptPolicy((PolicyTypes) iPolicyLoop) && (!bMustChooseTenet || m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(iPolicyLoop)->GetLevel() > 0))
		{
			int iWeight = 0;

			iWeight += m_PolicyAIWeights.GetWeight(iPolicyLoop);


			if(m_pCurrentPolicies->WillFinishBranchIfAdopted((PolicyTypes) iPolicyLoop))
			{
				int iPolicyBranch = m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(iPolicyLoop)->GetPolicyBranchType();
				if(iPolicyBranch != NO_POLICY_BRANCH_TYPE)
				{
					int iFinisherPolicy = m_pCurrentPolicies->GetPolicies()->GetPolicyBranchEntry(iPolicyBranch)->GetFreeFinishingPolicy();
					if(iFinisherPolicy != NO_POLICY)
					{
						iWeight += m_PolicyAIWeights.GetWeight(iFinisherPolicy);
					}
				}
			}
			m_AdoptablePolicies.push_back(iPolicyLoop + GC.getNumPolicyBranchInfos(), iWeight);

			if (m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(iPolicyLoop)->GetLevel() == 3)
			{
				aLevel3Tenets.push_back(iPolicyLoop);
			}
		}
	}


	bool bStartedAMutuallyExclusiveBranch = false;
#ifdef AUI_WARNING_FIXES
	for (uint iBranchLoop = 0; iBranchLoop < GC.getNumPolicyBranchInfos(); iBranchLoop++)
#else
	for(int iBranchLoop = 0; iBranchLoop < GC.getNumPolicyBranchInfos(); iBranchLoop++)
#endif
	{
		const PolicyBranchTypes ePolicyBranch = static_cast<PolicyBranchTypes>(iBranchLoop);
		CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(ePolicyBranch);
		if(pkPolicyBranchInfo)
		{
			if(pPlayer->GetPlayerPolicies()->IsPolicyBranchUnlocked(ePolicyBranch))
			{
				if(pkPolicyBranchInfo->IsMutuallyExclusive())
				{
					bStartedAMutuallyExclusiveBranch = true;
				}
			}
		}
	}

	AIGrandStrategyTypes eCultureGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");
	AIGrandStrategyTypes eCurrentGrandStrategy = pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy();


	if (!bMustChooseTenet)
	{
#ifdef AUI_WARNING_FIXES
		for (uint iBranchLoop = 0; iBranchLoop < GC.getNumPolicyBranchInfos(); iBranchLoop++)
#else
		for(int iBranchLoop = 0; iBranchLoop < GC.getNumPolicyBranchInfos(); iBranchLoop++)
#endif
		{
			const PolicyBranchTypes ePolicyBranch = static_cast<PolicyBranchTypes>(iBranchLoop);
			CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(ePolicyBranch);
			if(pkPolicyBranchInfo)
			{
				if(bStartedAMutuallyExclusiveBranch && pkPolicyBranchInfo->IsMutuallyExclusive())
				{
					continue;
				}

				if(pPlayer->GetPlayerPolicies()->CanUnlockPolicyBranch(ePolicyBranch) && !pPlayer->GetPlayerPolicies()->IsPolicyBranchUnlocked(ePolicyBranch))
				{
					int iBranchWeight = 0;


					if(IsBranchEffectiveInGame(ePolicyBranch))
					{
						iBranchWeight += WeighBranch(ePolicyBranch);

						iBranchWeight *= (100 - m_iPolicyWeightPercentDropNewBranch);
						iBranchWeight /= 100;
						if(eCurrentGrandStrategy == eCultureGrandStrategy)
						{
							iBranchWeight /= 3;
						}
					}

					m_AdoptablePolicies.push_back(iBranchLoop, iBranchWeight);
				}
			}
		}
	}

	m_AdoptablePolicies.SortItems();
	LogPossiblePolicies();


	if (aLevel3Tenets.size() > 0)
	{
		vector<int>::const_iterator it;
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
		for (it = aLevel3Tenets.begin(); it != aLevel3Tenets.end(); ++it)
#else
		for (it = aLevel3Tenets.begin(); it != aLevel3Tenets.end(); it++)
#endif
		{
			CvPolicyEntry *pEntry;
			pEntry = m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(*it);
			if (pEntry)
			{
				AIGrandStrategyTypes eGrandStrategy = pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy();
				if (eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST"))
				{
					if (pEntry->GetFlavorValue((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE")) > 0)
					{
						LogPolicyChoice((PolicyTypes)*it);
						return (*it) + GC.getNumPolicyBranchInfos();
					}
				}
				else if(eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_SPACESHIP"))
				{
					if (pEntry->GetFlavorValue((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SPACESHIP")) > 0)
					{
						LogPolicyChoice((PolicyTypes)*it);
						return (*it) + GC.getNumPolicyBranchInfos();
					}
				}
				else if(eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_UNITED_NATIONS"))
				{
					if (pEntry->GetFlavorValue((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DIPLOMACY")) > 0)
					{
						LogPolicyChoice((PolicyTypes)*it);
						return (*it) + GC.getNumPolicyBranchInfos();
					}
				}
				else if(eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE"))
				{
					if (pEntry->GetFlavorValue((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CULTURE")) > 0)
					{
						LogPolicyChoice((PolicyTypes)*it);
						return (*it) + GC.getNumPolicyBranchInfos();
					}
				}
			}
		}
	}

	CvAssertMsg(m_AdoptablePolicies.GetTotalWeight() >= 0, "Total weights of considered policies should not be negative! Please send Anton your save file and version.");


	if(m_AdoptablePolicies.GetTotalWeight() > 0)
	{
		int iNumChoices = GC.getGame().getHandicapInfo().GetPolicyNumOptions();
		iRtnValue = m_AdoptablePolicies.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing policy from Top Choices");
	}

	else if(m_AdoptablePolicies.GetTotalWeight() == 0 && m_AdoptablePolicies.size() > 0)
	{
		iRtnValue = m_AdoptablePolicies.ChooseAtRandom(&fcn, "Choosing policy at random (no good choices)");
	}


#ifdef AUI_WARNING_FIXES
	if (iRtnValue != (uint)NO_POLICY)
#else
	if(iRtnValue != (int)NO_POLICY)
#endif
	{
		if(iRtnValue >= GC.getNumPolicyBranchInfos())
		{
			LogPolicyChoice((PolicyTypes)(iRtnValue - GC.getNumPolicyBranchInfos()));
		}
		else
		{
			LogBranchChoice((PolicyBranchTypes)iRtnValue);
		}
	}

	return iRtnValue;
}

void CvPolicyAI::DoChooseIdeology(CvPlayer *pPlayer)
{
	int iFreedomPriority = 0;
	int iAutocracyPriority = 0;
	int iOrderPriority = 0;
	int iFreedomMultiplier = 1;
	int iAutocracyMultiplier = 1;
	int iOrderMultiplier = 1;
	PolicyBranchTypes eFreedomBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_FREEDOM();
	PolicyBranchTypes eAutocracyBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_AUTOCRACY();
	PolicyBranchTypes eOrderBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_ORDER();
	if (eFreedomBranch == NO_POLICY_BRANCH_TYPE || eAutocracyBranch == NO_POLICY_BRANCH_TYPE || eOrderBranch == NO_POLICY_BRANCH_TYPE)
	{
		return;
	}


	int iConquestPriority = max(0, pPlayer->GetGrandStrategyAI()->GetConquestPriority());
	int iDiploPriority = max(0, pPlayer->GetGrandStrategyAI()->GetUnitedNationsPriority());
	int iTechPriority = max(0, pPlayer->GetGrandStrategyAI()->GetSpaceshipPriority());
	int iCulturePriority = max(0, pPlayer->GetGrandStrategyAI()->GetCulturePriority());


	int iClearPrefPercent = GC.getIDEOLOGY_PERCENT_CLEAR_VICTORY_PREF();
	if (iConquestPriority > (iDiploPriority   * (100 + iClearPrefPercent) / 100) &&
		iConquestPriority > (iTechPriority    * (100 + iClearPrefPercent) / 100) &&
		iConquestPriority > (iCulturePriority * (100 + iClearPrefPercent) / 100))
	{
		iFreedomMultiplier = 0;
	}
	else if (iDiploPriority > (iConquestPriority * (100 + iClearPrefPercent) / 100) &&
		iDiploPriority > (iTechPriority     * (100 + iClearPrefPercent) / 100) &&
		iDiploPriority > (iCulturePriority  * (100 + iClearPrefPercent) / 100))
	{
		iOrderMultiplier = 0;
	}
	else if (iTechPriority > (iConquestPriority * (100 + iClearPrefPercent) / 100) &&
		iTechPriority > (iDiploPriority    * (100 + iClearPrefPercent) / 100) &&
		iTechPriority > (iCulturePriority  * (100 + iClearPrefPercent) / 100))
	{
		iAutocracyMultiplier = 0;
	}

	int iFreedomTotal = iDiploPriority + iTechPriority + iCulturePriority;
	int iAutocracyTotal = iDiploPriority + iConquestPriority + iCulturePriority;
	int iOrderTotal = iTechPriority + iConquestPriority + iCulturePriority;
	int iGrandTotal = iFreedomTotal + iAutocracyTotal + iOrderTotal;

	if (iGrandTotal > 0)
	{
		int iPriorityToDivide = GC.getIDEOLOGY_SCORE_GRAND_STRATS();
		iFreedomPriority = (iFreedomTotal * iPriorityToDivide) / iGrandTotal;
		iAutocracyPriority = (iAutocracyTotal * iPriorityToDivide) / iGrandTotal;
		iOrderPriority = (iOrderTotal * iPriorityToDivide) / iGrandTotal;
	}

	CvString stage = "After Grand Strategies";
	LogIdeologyChoice(stage, iFreedomPriority, iAutocracyPriority, iOrderPriority);


	iFreedomPriority += PolicyHelpers::GetNumFreePolicies(eFreedomBranch) * GC.getIDEOLOGY_SCORE_PER_FREE_TENET();
	iAutocracyPriority += PolicyHelpers::GetNumFreePolicies(eAutocracyBranch) * GC.getIDEOLOGY_SCORE_PER_FREE_TENET();
	iOrderPriority += PolicyHelpers::GetNumFreePolicies(eOrderBranch) * GC.getIDEOLOGY_SCORE_PER_FREE_TENET();;

	stage = "After Free Policies";
	LogIdeologyChoice(stage, iFreedomPriority, iAutocracyPriority, iOrderPriority);


	PlayerTypes eLoopPlayer;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if (eLoopPlayer != pPlayer->GetID() && pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			CvPlayer &kOtherPlayer = GET_PLAYER(eLoopPlayer);
			PolicyBranchTypes eOtherPlayerIdeology;
			eOtherPlayerIdeology = kOtherPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();

			switch(pPlayer->GetDiplomacyAI()->GetMajorCivApproach(eLoopPlayer,                       true))
			{
			case MAJOR_CIV_APPROACH_HOSTILE:
				if (eOtherPlayerIdeology == eFreedomBranch)
				{
					iAutocracyPriority += GC.getIDEOLOGY_SCORE_HOSTILE();
					iOrderPriority += GC.getIDEOLOGY_SCORE_HOSTILE();
				}
				else if (eOtherPlayerIdeology == eAutocracyBranch)
				{
					iFreedomPriority += GC.getIDEOLOGY_SCORE_HOSTILE();
					iOrderPriority += GC.getIDEOLOGY_SCORE_HOSTILE();
				}
				else if (eOtherPlayerIdeology == eOrderBranch)
				{
					iAutocracyPriority += GC.getIDEOLOGY_SCORE_HOSTILE();;
					iFreedomPriority += GC.getIDEOLOGY_SCORE_HOSTILE();
				}
				break;
			case MAJOR_CIV_APPROACH_GUARDED:
				if (eOtherPlayerIdeology == eFreedomBranch)
				{
					iAutocracyPriority += GC.getIDEOLOGY_SCORE_GUARDED();
					iOrderPriority += GC.getIDEOLOGY_SCORE_GUARDED();
				}
				else if (eOtherPlayerIdeology == eAutocracyBranch)
				{
					iFreedomPriority += GC.getIDEOLOGY_SCORE_GUARDED();
					iOrderPriority += GC.getIDEOLOGY_SCORE_GUARDED();
				}
				else if (eOtherPlayerIdeology == eOrderBranch)
				{
					iAutocracyPriority += GC.getIDEOLOGY_SCORE_GUARDED();
					iFreedomPriority += GC.getIDEOLOGY_SCORE_GUARDED();
				}
				break;
			case MAJOR_CIV_APPROACH_AFRAID:
				if (eOtherPlayerIdeology == eFreedomBranch)
				{
					iFreedomPriority += GC.getIDEOLOGY_SCORE_AFRAID();
				}
				else if (eOtherPlayerIdeology == eAutocracyBranch)
				{
					iAutocracyPriority += GC.getIDEOLOGY_SCORE_AFRAID();
				}
				else if (eOtherPlayerIdeology == eOrderBranch)
				{
					iOrderPriority += GC.getIDEOLOGY_SCORE_AFRAID();
				}
				break;
			case MAJOR_CIV_APPROACH_FRIENDLY:
				if (eOtherPlayerIdeology == eFreedomBranch)
				{
					iFreedomPriority += GC.getIDEOLOGY_SCORE_FRIENDLY();
				}
				else if (eOtherPlayerIdeology == eAutocracyBranch)
				{
					iAutocracyPriority += GC.getIDEOLOGY_SCORE_FRIENDLY();
				}
				else if (eOtherPlayerIdeology == eOrderBranch)
				{
					iOrderPriority += GC.getIDEOLOGY_SCORE_FRIENDLY();
				}
				break;
			case MAJOR_CIV_APPROACH_NEUTRAL:

				break;
			}
		}
	}

	stage = "After Relations";
	LogIdeologyChoice(stage, iFreedomPriority, iAutocracyPriority, iOrderPriority);


	int iHappinessModifier = GC.getIDEOLOGY_SCORE_HAPPINESS();


	int iHappinessDelta;
	int iHappinessPoliciesInBranch;
	iHappinessDelta = GetBranchBuildingHappiness(pPlayer, eFreedomBranch);
	iHappinessPoliciesInBranch = GetNumHappinessPolicies(pPlayer, eFreedomBranch);
	if (iHappinessPoliciesInBranch > 0)
	{
		iFreedomPriority += iHappinessDelta * iHappinessModifier / iHappinessPoliciesInBranch;		
	}
	iHappinessDelta = GetBranchBuildingHappiness(pPlayer, eAutocracyBranch);
	iHappinessPoliciesInBranch = GetNumHappinessPolicies(pPlayer, eAutocracyBranch);
	if (iHappinessPoliciesInBranch > 0)
	{
		iAutocracyPriority += iHappinessDelta * iHappinessModifier / iHappinessPoliciesInBranch;		
	}
	iHappinessDelta = GetBranchBuildingHappiness(pPlayer, eOrderBranch);
	iHappinessPoliciesInBranch = GetNumHappinessPolicies(pPlayer, eOrderBranch);
	if (iHappinessPoliciesInBranch > 0)
	{
		iOrderPriority += iHappinessDelta * iHappinessModifier / iHappinessPoliciesInBranch;		
	}

	stage = "After Tenet Happiness Boosts";
	LogIdeologyChoice(stage, iFreedomPriority, iAutocracyPriority, iOrderPriority);


	iHappinessDelta = max (0, 100 - pPlayer->GetCulture()->ComputeHypotheticalPublicOpinionUnhappiness(eFreedomBranch));
	iFreedomPriority += iHappinessDelta * iHappinessModifier;
	iHappinessDelta = max (0, 100 - pPlayer->GetCulture()->ComputeHypotheticalPublicOpinionUnhappiness(eAutocracyBranch));
	iAutocracyPriority += iHappinessDelta * iHappinessModifier;
	iHappinessDelta = max (0, 100 - pPlayer->GetCulture()->ComputeHypotheticalPublicOpinionUnhappiness(eOrderBranch));
	iOrderPriority += iHappinessDelta * iHappinessModifier;

	stage = "After Public Opinion Happiness";
	LogIdeologyChoice(stage, iFreedomPriority, iAutocracyPriority, iOrderPriority);


	iFreedomPriority += GC.getGame().getJonRandNum(10, "Freedom random priority bump");
	iAutocracyPriority += GC.getGame().getJonRandNum(10, "Autocracy random priority bump");
	iOrderPriority += GC.getGame().getJonRandNum(10, "Order random priority bump");

	stage = "After Random (1 to 10)";
	LogIdeologyChoice(stage, iFreedomPriority, iAutocracyPriority, iOrderPriority);


	iFreedomPriority = iFreedomPriority * iFreedomMultiplier;
	iAutocracyPriority = iAutocracyPriority * iAutocracyMultiplier;
	iOrderPriority = iOrderPriority * iOrderMultiplier;

	stage = "Final (after Clear Victory Preference)";
	LogIdeologyChoice(stage, iFreedomPriority, iAutocracyPriority, iOrderPriority);


	PolicyBranchTypes eChosenBranch;
	if (iFreedomPriority >= iAutocracyPriority && iFreedomPriority >= iOrderPriority)
	{
		eChosenBranch = eFreedomBranch;
	}
	else if (iAutocracyPriority >= iFreedomPriority && iAutocracyPriority >= iOrderPriority)
	{
		eChosenBranch = eAutocracyBranch;
	}
	else
	{
		eChosenBranch = eOrderBranch;
	}
	pPlayer->GetPlayerPolicies()->SetPolicyBranchUnlocked(eChosenBranch, true, false);
	LogBranchChoice(eChosenBranch);
}


void CvPolicyAI::DoConsiderIdeologySwitch(CvPlayer* pPlayer)
{

	int iCurrentHappiness = pPlayer->GetExcessHappiness();
	int iPublicOpinionUnhappiness = pPlayer->GetCulture()->GetPublicOpinionUnhappiness();
	PolicyBranchTypes ePreferredIdeology = pPlayer->GetCulture()->GetPublicOpinionPreferredIdeology();
	PolicyBranchTypes eCurrentIdeology = pPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
	PlayerTypes eMostPressure = pPlayer->GetCulture()->GetPublicOpinionBiggestInfluence();
	

	if (iCurrentHappiness <= GC.getSUPER_UNHAPPY_THRESHOLD() && iPublicOpinionUnhappiness >= 10)
	{

		int iHappinessCurrentIdeology = GetBranchBuildingHappiness(pPlayer, eCurrentIdeology);
		int iHappinessPreferredIdeology = GetBranchBuildingHappiness(pPlayer, ePreferredIdeology);


		bool bDontSwitchFreedom = false;
		bool bDontSwitchOrder = false;
		bool bDontSwitchAutocracy = false;
		int iConquestPriority = pPlayer->GetGrandStrategyAI()->GetConquestPriority();
		int iDiploPriority = pPlayer->GetGrandStrategyAI()->GetUnitedNationsPriority();
		int iTechPriority = pPlayer->GetGrandStrategyAI()->GetSpaceshipPriority();
		int iCulturePriority = pPlayer->GetGrandStrategyAI()->GetCulturePriority();
		int iClearPrefPercent = GC.getIDEOLOGY_PERCENT_CLEAR_VICTORY_PREF();
		if (iConquestPriority > (iDiploPriority   * (100 + iClearPrefPercent) / 100) &&
			iConquestPriority > (iTechPriority    * (100 + iClearPrefPercent) / 100) &&
			iConquestPriority > (iCulturePriority * (100 + iClearPrefPercent) / 100))
		{
			bDontSwitchFreedom = true;
		}
		else if (iDiploPriority > (iConquestPriority * (100 + iClearPrefPercent) / 100) &&
			iDiploPriority > (iTechPriority     * (100 + iClearPrefPercent) / 100) &&
			iDiploPriority > (iCulturePriority  * (100 + iClearPrefPercent) / 100))
		{
			bDontSwitchOrder = true;
		}
		else if (iTechPriority > (iConquestPriority * (100 + iClearPrefPercent) / 100) &&
			iTechPriority > (iDiploPriority    * (100 + iClearPrefPercent) / 100) &&
			iTechPriority > (iCulturePriority  * (100 + iClearPrefPercent) / 100))
		{
			bDontSwitchAutocracy = true;
		}

		int iTotalHappinessImprovement = iPublicOpinionUnhappiness + iHappinessPreferredIdeology - iHappinessCurrentIdeology;
		if (iTotalHappinessImprovement >= 10)
		{
			if (bDontSwitchFreedom && ePreferredIdeology == GC.getPOLICY_BRANCH_FREEDOM())
			{
				return;
			}
			if (bDontSwitchAutocracy && ePreferredIdeology == GC.getPOLICY_BRANCH_AUTOCRACY())
			{
				return;
			}
			if (bDontSwitchOrder && ePreferredIdeology == GC.getPOLICY_BRANCH_ORDER())
			{
				return;
			}


			pPlayer->SetAnarchyNumTurns(GC.getSWITCH_POLICY_BRANCHES_ANARCHY_TURNS());
			pPlayer->GetPlayerPolicies()->DoSwitchIdeologies(ePreferredIdeology);	

			if (ePreferredIdeology == GC.getPOLICY_BRANCH_FREEDOM() && eCurrentIdeology == GC.getPOLICY_BRANCH_ORDER())
			{
				if (GET_PLAYER(eMostPressure).GetID() == GC.getGame().getActivePlayer())
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_XP2_39);
				}
			}
		}
	}
}


int CvPolicyAI::GetBranchBuildingHappiness(CvPlayer* pPlayer, PolicyBranchTypes eBranch)
{

	int iSpecialPolicyBuildingHappiness = 0;
#ifdef AUI_WARNING_FIXES
	uint iBuildingClassLoop;
#else
	int iBuildingClassLoop;
#endif
	BuildingClassTypes eBuildingClass;
#ifdef AUI_WARNING_FIXES
	for (uint iPolicyLoop = 0; iPolicyLoop < GC.getNumPolicyInfos(); iPolicyLoop++)
#else
	for(int iPolicyLoop = 0; iPolicyLoop < GC.getNumPolicyInfos(); iPolicyLoop++)
#endif
	{
		PolicyTypes ePolicy = (PolicyTypes)iPolicyLoop;
		CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(ePolicy);
		if(pkPolicyInfo)
		{
			if (pkPolicyInfo->GetPolicyBranchType() == eBranch)
			{
				for(iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
				{
					eBuildingClass = (BuildingClassTypes) iBuildingClassLoop;

					CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
					if (!pkBuildingClassInfo)
					{
						continue;
					}

					if (pkPolicyInfo->GetBuildingClassHappiness(eBuildingClass) != 0)
					{
						BuildingTypes eBuilding = (BuildingTypes)pPlayer->getCivilizationInfo().getCivilizationBuildings(eBuildingClass);
						if (eBuilding != NO_BUILDING)
						{
							CvCity *pCity;
							int iLoop;
							for (pCity = pPlayer->firstCity(&iLoop); pCity != NULL; pCity = pPlayer->nextCity(&iLoop))
							{
								if (pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
								{
									iSpecialPolicyBuildingHappiness += pkPolicyInfo->GetBuildingClassHappiness(eBuildingClass);
								}
							}
						}
					}
				}
			}
		}
	}
	return iSpecialPolicyBuildingHappiness;
}


int CvPolicyAI::GetNumHappinessPolicies(CvPlayer* pPlayer, PolicyBranchTypes eBranch)
{
	int iRtnValue = 0;
#ifdef AUI_WARNING_FIXES
	uint iBuildingClassLoop;
	BuildingClassTypes eBuildingClass;
	for (uint iPolicyLoop = 0; iPolicyLoop < GC.getNumPolicyInfos(); iPolicyLoop++)
#else
	int iBuildingClassLoop;
	BuildingClassTypes eBuildingClass;
	for(int iPolicyLoop = 0; iPolicyLoop < GC.getNumPolicyInfos(); iPolicyLoop++)
#endif
	{
		PolicyTypes ePolicy = (PolicyTypes)iPolicyLoop;
		CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(ePolicy);
		if(pkPolicyInfo)
		{
			if (pkPolicyInfo->GetPolicyBranchType() == eBranch)
			{
				for(iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
				{
					eBuildingClass = (BuildingClassTypes) iBuildingClassLoop;

					CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
					if (!pkBuildingClassInfo)
					{
						continue;
					}

					BuildingTypes eBuilding = (BuildingTypes)pPlayer->getCivilizationInfo().getCivilizationBuildings(eBuildingClass);
					if (eBuilding != NO_BUILDING)
					{

						CvBuildingEntry *pkEntry = GC.getBuildingInfo(eBuilding);
						if (!pkEntry || pkEntry->IsNoOccupiedUnhappiness())
						{
							continue;
						}

						if (pkPolicyInfo->GetBuildingClassHappiness(eBuildingClass) != 0)
						{
							iRtnValue++;
							break;
						}
					}
				}
			}
		}
	}
	return iRtnValue;
}





#ifdef AUI_WARNING_FIXES
void CvPolicyAI::WeightPrereqs(FFastVector<int, true> paiTempWeights, int iPropagationPercent)
{
	uint iPolicyLoop;
#else
void CvPolicyAI::WeightPrereqs(int* paiTempWeights, int iPropagationPercent)
{
	int iPolicyLoop;
#endif


	for(iPolicyLoop = 0; iPolicyLoop < m_pCurrentPolicies->GetPolicies()->GetNumPolicies(); iPolicyLoop++)
	{

		if(paiTempWeights[iPolicyLoop] > 0)
		{
			PropagateWeights(iPolicyLoop, paiTempWeights[iPolicyLoop], iPropagationPercent, 0);
		}
	}
}


void CvPolicyAI::PropagateWeights(int iPolicy, int iWeight, int iPropagationPercent, int iPropagationLevel)
{
	if(iPropagationLevel < m_iPolicyWeightPropagationLevels)
	{
		int iPropagatedWeight = iWeight * iPropagationPercent / 100;


		for(int iI = 0; iI < GC.getNUM_OR_TECH_PREREQS(); iI++)
		{

			int iPrereq = m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(iPolicy)->GetPrereqAndPolicies(iI);
			if(iPrereq != NO_POLICY)
			{



				m_PolicyAIWeights.IncreaseWeight(iPrereq, iPropagatedWeight);


				if(iPropagatedWeight > 0)
				{
					PropagateWeights(iPrereq, iPropagatedWeight, iPropagationPercent, iPropagationLevel++);
				}
			}
			else
			{
				break;
			}
		}
	}
}


int CvPolicyAI::WeighBranch(PolicyBranchTypes eBranch)
{
	int iWeight = 0;

	CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(eBranch);
	if(pkPolicyBranchInfo)
	{
#ifdef AUI_WARNING_FIXES
		for (uint iPolicyLoop = 0; iPolicyLoop < m_pCurrentPolicies->GetPolicies()->GetNumPolicies(); iPolicyLoop++)
#else
		for(int iPolicyLoop = 0; iPolicyLoop < m_pCurrentPolicies->GetPolicies()->GetNumPolicies(); iPolicyLoop++)
#endif
		{
			const PolicyTypes ePolicyLoop = static_cast<PolicyTypes>(iPolicyLoop);
			CvPolicyEntry* pkLoopPolicyInfo = GC.getPolicyInfo(ePolicyLoop);
			if(pkLoopPolicyInfo)
			{

				if(!m_pCurrentPolicies->HasPolicy(ePolicyLoop))
				{

					if(pkLoopPolicyInfo->GetPolicyBranchType() == eBranch)
					{

						if(pkLoopPolicyInfo->GetPrereqAndPolicies(0) == NO_POLICY)
						{
							iWeight += m_PolicyAIWeights.GetWeight(iPolicyLoop);
						}
					}
				}
			}
		}


		iWeight += m_PolicyAIWeights.GetWeight(pkPolicyBranchInfo->GetFreePolicy());
	}

	return iWeight;
}


bool CvPolicyAI::IsBranchEffectiveInGame(PolicyBranchTypes eBranch)
{
	CvPolicyBranchEntry* pBranchInfo = GC.getPolicyBranchInfo(eBranch);
	CvAssertMsg(pBranchInfo, "Branch info not found! Please send Anton your save file and version.");
	if (!pBranchInfo) return false;
	
	if (pBranchInfo->IsDelayWhenNoReligion())
		if (GC.getGame().isOption(GAMEOPTION_NO_RELIGION))
			return false;

	if (pBranchInfo->IsDelayWhenNoCulture())
		if (GC.getGame().isOption(GAMEOPTION_NO_POLICIES))
			return false;

	if (pBranchInfo->IsDelayWhenNoScience())
		if (GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
			return false;

	if (pBranchInfo->IsDelayWhenNoCityStates())
		if (GC.getGame().GetNumMinorCivsEver() <= 0)
			return false;

	return true;
}


void CvPolicyAI::LogPossiblePolicies()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strDesc;


		playerName = m_pCurrentPolicies->GetPlayer()->getCivilizationShortDescription();

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

#ifdef AUI_WARNING_FIXES
		uint iNumBranches = GC.getNumPolicyBranchInfos();
#else
		int iNumBranches = GC.getNumPolicyBranchInfos();
#endif


		for(int iI = 0; iI < m_AdoptablePolicies.size(); iI++)
		{
			int iWeight = m_AdoptablePolicies.GetWeight(iI);

			if(m_AdoptablePolicies.GetElement(iI) < iNumBranches)
			{
				strTemp.Format("Branch %d, %d", m_AdoptablePolicies.GetElement(iI), iWeight);
			}
			else
			{

				PolicyTypes ePolicy = (PolicyTypes)(m_AdoptablePolicies.GetElement(iI) - iNumBranches);
				CvPolicyEntry* pPolicyEntry = GC.getPolicyInfo(ePolicy);
				const char* szPolicyType = (pPolicyEntry != NULL)? pPolicyEntry->GetType() : "Unknown";
				strTemp.Format("%s, %d", szPolicyType, iWeight);
			}
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}


void CvPolicyAI::LogPolicyChoice(PolicyTypes ePolicy)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strDesc;


		playerName = m_pCurrentPolicies->GetPlayer()->getCivilizationShortDescription();

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		CvPolicyEntry* pPolicyEntry = GC.getPolicyInfo(ePolicy);
		const char* szPolicyType = (pPolicyEntry != NULL)? pPolicyEntry->GetType() : "Unknown";
		strTemp.Format("CHOSEN, %s", szPolicyType);

		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}
}


void CvPolicyAI::LogBranchChoice(PolicyBranchTypes eBranch)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strDesc;


		playerName = m_pCurrentPolicies->GetPlayer()->getCivilizationShortDescription();

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		strTemp.Format("CHOSEN, Branch %d", eBranch);

		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}
}


void CvPolicyAI::LogIdeologyChoice(CvString &decisionState, int iWeightFreedom, int iWeightAutocracy, int iWeightOrder)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;


		playerName = m_pCurrentPolicies->GetPlayer()->getCivilizationShortDescription();

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		strTemp.Format("%s, Freedom: %d, Order: %d, Autocracy: %d", decisionState.c_str(), iWeightFreedom, iWeightOrder, iWeightAutocracy);

		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}
}


CvString CvPolicyAI::GetLogFileName(CvString& playerName) const
{
	CvString strLogName;


	if(GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "PolicyAILog_" + playerName + ".csv";
	}
	else
	{
		strLogName = "PolicyAILog.csv";
	}

	return strLogName;
}
