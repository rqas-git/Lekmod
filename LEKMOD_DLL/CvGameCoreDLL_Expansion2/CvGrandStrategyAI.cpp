






#include "CvGameCoreDLLPCH.h"
#include "CvGrandStrategyAI.h"
#include "CvEconomicAI.h"
#include "CvCitySpecializationAI.h"
#include "CvDiplomacyAI.h"
#include "CvMinorCivAI.h"
#include "ICvDLLUserInterface.h"


#include "LintFree.h"


CvAIGrandStrategyXMLEntry::CvAIGrandStrategyXMLEntry(void):
	m_piFlavorValue(NULL),
	m_piSpecializationBoost(NULL),
	m_piFlavorModValue(NULL)
{
}

CvAIGrandStrategyXMLEntry::~CvAIGrandStrategyXMLEntry(void)
{
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piSpecializationBoost);
	SAFE_DELETE_ARRAY(m_piFlavorModValue);
}

bool CvAIGrandStrategyXMLEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;


	const char* szType = GetType();
	kUtility.SetFlavors(m_piFlavorValue, "AIGrandStrategy_Flavors", "AIGrandStrategyType", szType);
	kUtility.SetYields(m_piSpecializationBoost, "AIGrandStrategy_Yields", "AIGrandStrategyType", szType);
	kUtility.SetFlavors(m_piFlavorModValue, "AIGrandStrategy_FlavorMods", "AIGrandStrategyType", szType);

	return true;
}


#ifdef AUI_WARNING_FIXES
int CvAIGrandStrategyXMLEntry::GetFlavorValue(uint i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
#else
int CvAIGrandStrategyXMLEntry::GetFlavorValue(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
#endif
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : -1;
}


int CvAIGrandStrategyXMLEntry::GetSpecializationBoost(YieldTypes eYield) const
{
	FAssertMsg(eYield < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(eYield > -1, "Index out of bounds");
	return m_piSpecializationBoost ? m_piSpecializationBoost[(int)eYield] : 0;
}


#ifdef AUI_WARNING_FIXES
int CvAIGrandStrategyXMLEntry::GetFlavorModValue(uint i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
#else
int CvAIGrandStrategyXMLEntry::GetFlavorModValue(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
#endif
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorModValue ? m_piFlavorModValue[i] : 0;
}







CvAIGrandStrategyXMLEntries::CvAIGrandStrategyXMLEntries(void)
{

}


CvAIGrandStrategyXMLEntries::~CvAIGrandStrategyXMLEntries(void)
{
	DeleteArray();
}


std::vector<CvAIGrandStrategyXMLEntry*>& CvAIGrandStrategyXMLEntries::GetAIGrandStrategyEntries()
{
	return m_paAIGrandStrategyEntries;
}


#ifdef AUI_WARNING_FIXES
uint CvAIGrandStrategyXMLEntries::GetNumAIGrandStrategies() const
#else
int CvAIGrandStrategyXMLEntries::GetNumAIGrandStrategies()
#endif
{
	return m_paAIGrandStrategyEntries.size();
}


void CvAIGrandStrategyXMLEntries::DeleteArray()
{
	for(std::vector<CvAIGrandStrategyXMLEntry*>::iterator it = m_paAIGrandStrategyEntries.begin(); it != m_paAIGrandStrategyEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paAIGrandStrategyEntries.clear();
}


#ifdef AUI_WARNING_FIXES
CvAIGrandStrategyXMLEntry* CvAIGrandStrategyXMLEntries::GetEntry(uint index) const
#else
CvAIGrandStrategyXMLEntry* CvAIGrandStrategyXMLEntries::GetEntry(int index)
#endif
{
	return m_paAIGrandStrategyEntries[index];
}







CvGrandStrategyAI::CvGrandStrategyAI():
#ifdef AUI_WARNING_FIXES
	m_pPlayer(NULL),
	m_pAIGrandStrategies(NULL),
	m_iNumTurnsSinceActiveSet(0),
	m_eActiveGrandStrategy(NO_AIGRANDSTRATEGY),
#endif
	m_paiGrandStrategyPriority(NULL),
	m_eGuessOtherPlayerActiveGrandStrategy(NULL),
	m_eGuessOtherPlayerActiveGrandStrategyConfidence(NULL)
{
}


CvGrandStrategyAI::~CvGrandStrategyAI(void)
{
}


void CvGrandStrategyAI::Init(CvAIGrandStrategyXMLEntries* pAIGrandStrategies, CvPlayer* pPlayer)
{

	m_pAIGrandStrategies = pAIGrandStrategies;

	m_pPlayer = pPlayer;


	FAssertMsg(m_paiGrandStrategyPriority==NULL, "about to leak memory, CvGrandStrategyAI::m_paiGrandStrategyPriority");
	m_paiGrandStrategyPriority = FNEW(int[m_pAIGrandStrategies->GetNumAIGrandStrategies()], c_eCiv5GameplayDLL, 0);

	FAssertMsg(m_eGuessOtherPlayerActiveGrandStrategy==NULL, "about to leak memory, CvGrandStrategyAI::m_eGuessOtherPlayerActiveGrandStrategy");
	m_eGuessOtherPlayerActiveGrandStrategy = FNEW(int[MAX_MAJOR_CIVS], c_eCiv5GameplayDLL, 0);

	FAssertMsg(m_eGuessOtherPlayerActiveGrandStrategyConfidence==NULL, "about to leak memory, CvGrandStrategyAI::m_eGuessOtherPlayerActiveGrandStrategyConfidence");
	m_eGuessOtherPlayerActiveGrandStrategyConfidence = FNEW(int[MAX_MAJOR_CIVS], c_eCiv5GameplayDLL, 0);

	Reset();
}


void CvGrandStrategyAI::Uninit()
{
	SAFE_DELETE_ARRAY(m_paiGrandStrategyPriority);
	SAFE_DELETE_ARRAY(m_eGuessOtherPlayerActiveGrandStrategy);
	SAFE_DELETE_ARRAY(m_eGuessOtherPlayerActiveGrandStrategyConfidence);
}


void CvGrandStrategyAI::Reset()
{
#ifdef AUI_WARNING_FIXES
	uint iI;
#else
	int iI;
#endif

	m_iNumTurnsSinceActiveSet = 0;

	m_eActiveGrandStrategy = NO_AIGRANDSTRATEGY;

	for(iI = 0; iI < m_pAIGrandStrategies->GetNumAIGrandStrategies(); iI++)
	{
		m_paiGrandStrategyPriority[iI] = -1;
	}

	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		m_eGuessOtherPlayerActiveGrandStrategy[iI] = NO_AIGRANDSTRATEGY;
		m_eGuessOtherPlayerActiveGrandStrategyConfidence[iI] = NO_GUESS_CONFIDENCE_TYPE;
	}
}


void CvGrandStrategyAI::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_iNumTurnsSinceActiveSet;
	kStream >> (int&)m_eActiveGrandStrategy;

	FAssertMsg(m_pAIGrandStrategies != NULL && m_pAIGrandStrategies->GetNumAIGrandStrategies() > 0, "Number of AIGrandStrategies to serialize is expected to greater than 0");
#ifdef _MSC_VER



#pragma warning ( push )
#pragma warning ( disable : 6011 )
#endif
	ArrayWrapper<int> wrapm_paiGrandStrategyPriority(m_pAIGrandStrategies->GetNumAIGrandStrategies(), m_paiGrandStrategyPriority);
#ifdef _MSC_VER
#pragma warning ( pop )
#endif

	kStream >> wrapm_paiGrandStrategyPriority;

	ArrayWrapper<int> wrapm_eGuessOtherPlayerActiveGrandStrategy(MAX_MAJOR_CIVS, m_eGuessOtherPlayerActiveGrandStrategy);
	kStream >> wrapm_eGuessOtherPlayerActiveGrandStrategy;

	ArrayWrapper<int> wrapm_eGuessOtherPlayerActiveGrandStrategyConfidence(MAX_MAJOR_CIVS, m_eGuessOtherPlayerActiveGrandStrategyConfidence);
	kStream >> wrapm_eGuessOtherPlayerActiveGrandStrategyConfidence;

}


void CvGrandStrategyAI::Write(FDataStream& kStream)
{

	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << m_iNumTurnsSinceActiveSet;
	kStream << m_eActiveGrandStrategy;

	FAssertMsg(GC.getNumAIGrandStrategyInfos() > 0, "Number of AIStrategies to serialize is expected to greater than 0");
	kStream << ArrayWrapper<int>(m_pAIGrandStrategies->GetNumAIGrandStrategies(), m_paiGrandStrategyPriority);

	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_eGuessOtherPlayerActiveGrandStrategy);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_eGuessOtherPlayerActiveGrandStrategyConfidence);
}


#ifdef AUI_CONSTIFY
CvPlayer* CvGrandStrategyAI::GetPlayer() const
#else
CvPlayer* CvGrandStrategyAI::GetPlayer()
#endif
{
	return m_pPlayer;
}


#ifdef AUI_CONSTIFY
CvAIGrandStrategyXMLEntries* CvGrandStrategyAI::GetAIGrandStrategies() const
#else
CvAIGrandStrategyXMLEntries* CvGrandStrategyAI::GetAIGrandStrategies()
#endif
{
	return m_pAIGrandStrategies;
}


void CvGrandStrategyAI::DoTurn()
{
	DoGuessOtherPlayersActiveGrandStrategy();

#ifdef AUI_GS_FIX_NO_ACTIVE_GS_FOR_HUMANS
	if (GetPlayer()->isHuman())
		return;
#endif

#ifdef AUI_WARNING_FIXES
	uint iGrandStrategiesLoop;
#else
	int iGrandStrategiesLoop;
#endif
	AIGrandStrategyTypes eGrandStrategy;
	CvAIGrandStrategyXMLEntry* pGrandStrategy;
	CvString strGrandStrategyName;


	for(iGrandStrategiesLoop = 0; iGrandStrategiesLoop < GetAIGrandStrategies()->GetNumAIGrandStrategies(); iGrandStrategiesLoop++)
	{
		eGrandStrategy = (AIGrandStrategyTypes) iGrandStrategiesLoop;
		pGrandStrategy = GetAIGrandStrategies()->GetEntry(iGrandStrategiesLoop);
		strGrandStrategyName = (CvString) pGrandStrategy->GetType();



		int iPriority = GetBaseGrandStrategyPriority(eGrandStrategy);

		if(strGrandStrategyName == "AIGRANDSTRATEGY_CONQUEST")
		{
			iPriority += GetConquestPriority();
		}
		else if(strGrandStrategyName == "AIGRANDSTRATEGY_CULTURE")
		{
			iPriority += GetCulturePriority();
		}
		else if(strGrandStrategyName == "AIGRANDSTRATEGY_UNITED_NATIONS")
		{
			iPriority += GetUnitedNationsPriority();
		}
		else if(strGrandStrategyName == "AIGRANDSTRATEGY_SPACESHIP")
		{
			iPriority += GetSpaceshipPriority();
		}


		iPriority += GC.getGame().getJonRandNum(       GC.getAI_GS_RAND_ROLL(), "Grand Strategy AI: GS rand roll.");


		if(GetActiveGrandStrategy() == eGrandStrategy && GetActiveGrandStrategy() != NO_AIGRANDSTRATEGY)
		{
			iPriority +=        GC.getAI_GRAND_STRATEGY_CURRENT_STRATEGY_WEIGHT();
		}

		SetGrandStrategyPriority(eGrandStrategy, iPriority);
	}


	int iNumPlayersAliveAndMet = 0;

	int iMajorLoop;

	for(iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		if(GET_PLAYER((PlayerTypes) iMajorLoop).isAlive())
		{
			if(GET_TEAM(GetPlayer()->getTeam()).isHasMet(GET_PLAYER((PlayerTypes) iMajorLoop).getTeam()))
			{
				iNumPlayersAliveAndMet++;
			}
		}
	}

	FStaticVector< int, 5, true, c_eCiv5GameplayDLL > viNumGrandStrategiesAdopted;
	int iNumPlayers;


	for(iGrandStrategiesLoop = 0; iGrandStrategiesLoop < GetAIGrandStrategies()->GetNumAIGrandStrategies(); iGrandStrategiesLoop++)
	{
		iNumPlayers = 0;


		for(iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
		{
			if(GetGuessOtherPlayerActiveGrandStrategy((PlayerTypes) iMajorLoop) == (AIGrandStrategyTypes) iGrandStrategiesLoop)
			{
				iNumPlayers++;
			}
		}

		viNumGrandStrategiesAdopted.push_back(iNumPlayers);
	}

	FStaticVector< int, 5, true, c_eCiv5GameplayDLL > viGrandStrategyChangeForLogging;

	int iChange;


	for(iGrandStrategiesLoop = 0; iGrandStrategiesLoop < GetAIGrandStrategies()->GetNumAIGrandStrategies(); iGrandStrategiesLoop++)
	{
		eGrandStrategy = (AIGrandStrategyTypes) iGrandStrategiesLoop;

		iChange = GetGrandStrategyPriority(eGrandStrategy) *        GC.getAI_GRAND_STRATEGY_OTHER_PLAYERS_GS_MULTIPLIER();
		iChange = iChange * viNumGrandStrategiesAdopted[eGrandStrategy] / iNumPlayersAliveAndMet;
		iChange /= 100;

		ChangeGrandStrategyPriority(eGrandStrategy, -iChange);

		viGrandStrategyChangeForLogging.push_back(-iChange);
	}

	ChangeNumTurnsSinceActiveSet(1);



	if(GetActiveGrandStrategy() == NO_AIGRANDSTRATEGY || GetNumTurnsSinceActiveSet() >=        GC.getAI_GRAND_STRATEGY_NUM_TURNS_STRATEGY_MUST_BE_ACTIVE())
	{
		int iBestPriority = -1;
		int iPriority;

		AIGrandStrategyTypes eBestGrandStrategy = NO_AIGRANDSTRATEGY;

		for(iGrandStrategiesLoop = 0; iGrandStrategiesLoop < GetAIGrandStrategies()->GetNumAIGrandStrategies(); iGrandStrategiesLoop++)
		{
			eGrandStrategy = (AIGrandStrategyTypes) iGrandStrategiesLoop;

			iPriority = GetGrandStrategyPriority(eGrandStrategy);

			if(iPriority > iBestPriority)
			{
				iBestPriority = iPriority;
				eBestGrandStrategy = eGrandStrategy;
			}
		}

		if(eBestGrandStrategy != GetActiveGrandStrategy())
		{
			SetActiveGrandStrategy(eBestGrandStrategy);
			m_pPlayer->GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_NEW_GRAND_STRATEGY);
		}
	}

	LogGrandStrategies(viGrandStrategyChangeForLogging);
}


#ifdef AUI_CONSTIFY
int CvGrandStrategyAI::GetConquestPriority() const
#else
int CvGrandStrategyAI::GetConquestPriority()
#endif
{
	int iPriority = 0;


	VictoryTypes eVictory = (VictoryTypes) GC.getInfoTypeForString("VICTORY_DOMINATION", true);
	if(eVictory == NO_VICTORY || !GC.getGame().isVictoryValid(eVictory))
	{
		if(!GC.getGame().areNoVictoriesValid())
		{
			return -100;
		}
	}

	int iGeneralWarlikeness = GetPlayer()->GetDiplomacyAI()->GetPersonalityMajorCivApproachBias(MAJOR_CIV_APPROACH_WAR);
	int iGeneralHostility = GetPlayer()->GetDiplomacyAI()->GetPersonalityMajorCivApproachBias(MAJOR_CIV_APPROACH_HOSTILE);
	int iGeneralDeceptiveness = GetPlayer()->GetDiplomacyAI()->GetPersonalityMajorCivApproachBias(MAJOR_CIV_APPROACH_DECEPTIVE);
	int iGeneralFriendliness = GetPlayer()->GetDiplomacyAI()->GetPersonalityMajorCivApproachBias(MAJOR_CIV_APPROACH_FRIENDLY);

	int iGeneralApproachModifier = max(max(iGeneralDeceptiveness, iGeneralHostility),iGeneralWarlikeness) - iGeneralFriendliness;

	iPriority += ((GetPlayer()->GetDiplomacyAI()->GetBoldness() + iGeneralApproachModifier) * (12 - m_pPlayer->GetCurrentEra()));

	CvTeam& pTeam = GET_TEAM(GetPlayer()->getTeam());


	if(GC.getGame().getElapsedGameTurns() >=        GC.getAI_GS_CONQUEST_NOBODY_MET_FIRST_TURN())
	{

		bool bHasMetMajor = false;

		for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
		{
			if(pTeam.GetID() != iTeamLoop && !GET_TEAM((TeamTypes) iTeamLoop).isMinorCiv())
			{
				if(pTeam.isHasMet((TeamTypes) iTeamLoop))
				{
					bHasMetMajor = true;
					break;
				}
			}
		}
		if(!bHasMetMajor)
		{
			iPriority +=         GC.getAI_GRAND_STRATEGY_CONQUEST_NOBODY_MET_WEIGHT();
		}
	}


	if(GC.getGame().getElapsedGameTurns() >=        GC.getAI_GS_CONQUEST_MILITARY_STRENGTH_FIRST_TURN())
	{

		int iWorldMilitaryStrength = GC.getGame().GetWorldMilitaryStrengthAverage(GetPlayer()->GetID(), true, true);

		if(iWorldMilitaryStrength > 0)
		{
			int iMilitaryRatio = (GetPlayer()->GetMilitaryMight() - iWorldMilitaryStrength) *         GC.getAI_GRAND_STRATEGY_CONQUEST_POWER_RATIO_MULTIPLIER() / iWorldMilitaryStrength;


			if(iMilitaryRatio > 0)
				iMilitaryRatio /= 2;

			iPriority += iMilitaryRatio;
		}
	}


	if(pTeam.getAtWarCount(                  false) > 0)
	{
		iPriority +=        GC.getAI_GRAND_STRATEGY_CONQUEST_AT_WAR_WEIGHT();
	}


	if(GetPlayer()->IsCramped())
	{
		PlayerTypes ePlayer;
		int iNumPlayersMet = 1;
		int iTotalLandMe = 0;
		int iTotalLandPlayersMet = 0;


		for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
		{
			ePlayer = (PlayerTypes) iMajorLoop;

			if(GET_PLAYER(ePlayer).isAlive() && iMajorLoop != GetPlayer()->GetID())
			{
				if(pTeam.isHasMet(GET_PLAYER(ePlayer).getTeam()))
				{
					iNumPlayersMet++;
				}
			}
		}

		if(iNumPlayersMet > 0)
		{

#ifdef AUI_WARNING_FIXES
			for (uint iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
#else
			for(int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
#endif
			{
				if(GC.getMap().plotByIndexUnchecked(iPlotLoop)->isOwned())
				{
					ePlayer = GC.getMap().plotByIndexUnchecked(iPlotLoop)->getOwner();

					if(ePlayer == GetPlayer()->GetID())
					{
						iTotalLandPlayersMet++;
						iTotalLandMe++;
					}
					else if(!GET_PLAYER(ePlayer).isMinorCiv() && pTeam.isHasMet(GET_PLAYER(ePlayer).getTeam()))
					{
						iTotalLandPlayersMet++;
					}
				}
			}

			iTotalLandPlayersMet /= iNumPlayersMet;

			if(iTotalLandMe > 0)
			{
				if(iTotalLandPlayersMet / iTotalLandMe > 0)
				{
					iPriority +=        GC.getAI_GRAND_STRATEGY_CONQUEST_CRAMPED_WEIGHT();
				}
			}
		}
	}


	if(GetPlayer()->getNumNukeUnits() == 0)
	{
		for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
		{
			PlayerTypes ePlayer = (PlayerTypes) iMajorLoop;

			if(GET_PLAYER(ePlayer).isAlive() && iMajorLoop != GetPlayer()->GetID())
			{
				if(pTeam.isHasMet(GET_PLAYER(ePlayer).getTeam()))
				{
					if (GET_PLAYER(ePlayer).getNumNukeUnits() > 0)
					{
						iPriority -= 50; 
						break;
					}
				}
			}
		}
	}

	return iPriority;
}


#ifdef AUI_CONSTIFY
int CvGrandStrategyAI::GetCulturePriority() const
#else
int CvGrandStrategyAI::GetCulturePriority()
#endif
{
	int iPriority = 0;


	VictoryTypes eVictory = (VictoryTypes) GC.getInfoTypeForString("VICTORY_CULTURAL", true);
	if(eVictory == NO_VICTORY || !GC.getGame().isVictoryValid(eVictory))
	{
		return -100;
	}


	int iFlavorCulture =  m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CULTURE"));
	iPriority += (10 - m_pPlayer->GetCurrentEra()) * iFlavorCulture * 200 / 100;


	PlayerTypes eLoopPlayer;
	int iOurCulture = m_pPlayer->GetTotalJONSCulturePerTurn();
	int iOurTourism = m_pPlayer->GetCulture()->GetTourism();
	int iNumCivsBehindCulture = 0;
	int iNumCivsAheadCulture = 0;
	int iNumCivsBehindTourism = 0;
	int iNumCivsAheadTourism = 0;
	int iNumCivsAlive = 0;

	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		CvPlayer &kPlayer = GET_PLAYER(eLoopPlayer);

		if (kPlayer.isAlive() && !kPlayer.isMinorCiv() && !kPlayer.isBarbarian() && iPlayerLoop != m_pPlayer->GetID())
		{
			if (iOurCulture > kPlayer.GetTotalJONSCulturePerTurn())
			{
				iNumCivsAheadCulture++;
			}
			else
			{
				iNumCivsBehindCulture++;
			}
			if (iOurTourism > kPlayer.GetCulture()->GetTourism())
			{
				iNumCivsAheadTourism++;
			}
			else
			{
				iNumCivsBehindTourism++;
			}
			iNumCivsAlive++;
		}
	}

	if (iNumCivsAlive > 0 && iNumCivsAheadCulture > iNumCivsBehindCulture)
	{
		iPriority += (GC.getAI_GS_CULTURE_AHEAD_WEIGHT() * (iNumCivsAheadCulture - iNumCivsBehindCulture) / iNumCivsAlive);
	}
	if (iNumCivsAlive > 0 && iNumCivsAheadTourism > iNumCivsBehindTourism)
	{
		iPriority += (GC.getAI_GS_CULTURE_TOURISM_AHEAD_WEIGHT() * (iNumCivsAheadTourism - iNumCivsBehindTourism) / iNumCivsAlive);
	}


	int iNumInfluential = m_pPlayer->GetCulture()->GetNumCivsInfluentialOn();
	iPriority += iNumInfluential * GC.getAI_GS_CULTURE_INFLUENTIAL_CIV_MOD();

	return iPriority;
}


#ifdef AUI_CONSTIFY
int CvGrandStrategyAI::GetUnitedNationsPriority() const
#else
int CvGrandStrategyAI::GetUnitedNationsPriority()
#endif
{
	int iPriority = 0;
	PlayerTypes ePlayer = m_pPlayer->GetID();


	VictoryTypes eVictory = (VictoryTypes) GC.getInfoTypeForString("VICTORY_DIPLOMATIC", true);
	if(eVictory == NO_VICTORY || !GC.getGame().isVictoryValid(eVictory))
	{
		return -100;
	}

	int iNumMinorsAttacked = GET_TEAM(GetPlayer()->getTeam()).GetNumMinorCivsAttacked();
	iPriority += (iNumMinorsAttacked*         GC.getAI_GRAND_STRATEGY_UN_EACH_MINOR_ATTACKED_WEIGHT());

	int iVotesNeededToWin = GC.getGame().GetVotesNeededForDiploVictory();

	int iVotesControlled = 0;
	int iVotesControlledDelta = 0;
	int iUnalliedCityStates = 0;
#ifdef AUI_LEAGUES_FIX_POSSIBLE_DEALLOCATION_CRASH
	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	if (pLeague == NULL)
#else
	if (GC.getGame().GetGameLeagues()->GetNumActiveLeagues() == 0)
#endif
	{

		int iFlavorDiplo =  m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DIPLOMACY"));
		iPriority += (10 - m_pPlayer->GetCurrentEra()) * iFlavorDiplo * 150 / 100;
	}
	else
	{
#ifndef AUI_LEAGUES_FIX_POSSIBLE_DEALLOCATION_CRASH
		CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
		CvAssert(pLeague != NULL);
		if (pLeague != NULL)
#endif
		{

			iVotesControlled += pLeague->CalculateStartingVotesForMember(ePlayer);


			int iHighestOtherPlayerVotes = 0;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
			{
				PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;

				if(eLoopPlayer != ePlayer && GET_PLAYER(eLoopPlayer).isAlive())
				{
					if (GET_PLAYER(eLoopPlayer).isMinorCiv())
					{
						if (GET_PLAYER(eLoopPlayer).GetMinorCivAI()->GetAlly() == NO_PLAYER)
						{
							iUnalliedCityStates++;
						}
					}
					else
					{
						int iOtherPlayerVotes = pLeague->CalculateStartingVotesForMember(eLoopPlayer);
						if (iOtherPlayerVotes > iHighestOtherPlayerVotes)
						{
							iHighestOtherPlayerVotes = iOtherPlayerVotes;
						}
					}
				}
			}


			iVotesControlledDelta = iVotesControlled - iHighestOtherPlayerVotes;
		}
	}


	if (iVotesControlled >= iVotesNeededToWin)
	{
		return 1000;
	}
	else if (iVotesControlled >= ((iVotesNeededToWin * 3) / 4))
	{
		iPriority += 40;
	}


	if (iVotesControlledDelta > 0)
	{
		iPriority += MAX(40, iVotesControlledDelta * 5);
	}

	else
	{

		int iPotentialCityStateVotes = iUnalliedCityStates * 2;
		int iPotentialVotesDelta = iPotentialCityStateVotes + iVotesControlledDelta;
		if (iPotentialVotesDelta > 0)
		{
			iPriority += MAX(20, iPotentialVotesDelta * 5);
		}
		else if (iPotentialVotesDelta < 0)
		{
			iPriority += MIN(-40, iPotentialVotesDelta * -5);
		}
	}


	iPriority += m_pPlayer->GetPlayerTraits()->GetCityStateFriendshipModifier();
	iPriority += m_pPlayer->GetPlayerTraits()->GetCityStateBonusModifier();
	iPriority -= m_pPlayer->GetPlayerTraits()->GetCityStateCombatModifier();

	return iPriority;
}


#ifdef AUI_CONSTIFY
int CvGrandStrategyAI::GetSpaceshipPriority() const
#else
int CvGrandStrategyAI::GetSpaceshipPriority()
#endif
{
	int iPriority = 0;


	VictoryTypes eVictory = (VictoryTypes) GC.getInfoTypeForString("VICTORY_SPACE_RACE", true);
	if(eVictory == NO_VICTORY || !GC.getGame().isVictoryValid(eVictory))
	{
		return -100;
	}

	int iFlavorScience =  m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SCIENCE"));


	iPriority += m_pPlayer->GetCurrentEra() * iFlavorScience * 150 / 100;


	ProjectTypes eApolloProgram = (ProjectTypes) GC.getInfoTypeForString("PROJECT_APOLLO_PROGRAM", true);
	if(eApolloProgram != NO_PROJECT)
	{
		if(GET_TEAM(m_pPlayer->getTeam()).getProjectCount(eApolloProgram) > 0)
		{
			iPriority +=         GC.getAI_GS_SS_HAS_APOLLO_PROGRAM();
		}
	}

	return iPriority;
}


#ifdef AUI_CONSTIFY
int CvGrandStrategyAI::GetBaseGrandStrategyPriority(AIGrandStrategyTypes eGrandStrategy) const
#else
int CvGrandStrategyAI::GetBaseGrandStrategyPriority(AIGrandStrategyTypes eGrandStrategy)
#endif
{
	CvAIGrandStrategyXMLEntry* pGrandStrategy = GetAIGrandStrategies()->GetEntry(eGrandStrategy);

	int iPriority = 0;


	for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		if(pGrandStrategy->GetFlavorValue(iFlavorLoop) != 0)
		{
			iPriority += (pGrandStrategy->GetFlavorValue(iFlavorLoop) * GetPlayer()->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes) iFlavorLoop));
		}
	}

	return iPriority;
}


#ifdef AUI_CONSTIFY
int CvGrandStrategyAI::GetPersonalityAndGrandStrategy(FlavorTypes eFlavorType) const
#else
int CvGrandStrategyAI::GetPersonalityAndGrandStrategy(FlavorTypes eFlavorType)
#endif
{
	if(m_eActiveGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		CvAIGrandStrategyXMLEntry* pGrandStrategy = GetAIGrandStrategies()->GetEntry(m_eActiveGrandStrategy);
		int iModdedFlavor = pGrandStrategy->GetFlavorModValue(eFlavorType) + m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavorType);
		iModdedFlavor = max(0,iModdedFlavor);
		return iModdedFlavor;
	}
	return m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavorType);
}


AIGrandStrategyTypes CvGrandStrategyAI::GetActiveGrandStrategy() const
{
	return m_eActiveGrandStrategy;
}


void CvGrandStrategyAI::SetActiveGrandStrategy(AIGrandStrategyTypes eGrandStrategy)
{
	if(eGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		m_eActiveGrandStrategy = eGrandStrategy;

		SetNumTurnsSinceActiveSet(0);
	}
}


int CvGrandStrategyAI::GetNumTurnsSinceActiveSet() const
{
	return m_iNumTurnsSinceActiveSet;
}


void CvGrandStrategyAI::SetNumTurnsSinceActiveSet(int iValue)
{
	m_iNumTurnsSinceActiveSet = iValue;
	FAssert(m_iNumTurnsSinceActiveSet >= 0);
}


void CvGrandStrategyAI::ChangeNumTurnsSinceActiveSet(int iChange)
{
	if(iChange != 0)
	{
		m_iNumTurnsSinceActiveSet += iChange;
	}

	FAssert(m_iNumTurnsSinceActiveSet >= 0);
}


int CvGrandStrategyAI::GetGrandStrategyPriority(AIGrandStrategyTypes eGrandStrategy) const
{
	FAssert(eGrandStrategy != NO_AIGRANDSTRATEGY);
	return m_paiGrandStrategyPriority[eGrandStrategy];
}


void CvGrandStrategyAI::SetGrandStrategyPriority(AIGrandStrategyTypes eGrandStrategy, int iValue)
{
	FAssert(eGrandStrategy != NO_AIGRANDSTRATEGY);
	m_paiGrandStrategyPriority[eGrandStrategy] = iValue;
}


void CvGrandStrategyAI::ChangeGrandStrategyPriority(AIGrandStrategyTypes eGrandStrategy, int iChange)
{
	FAssert(eGrandStrategy != NO_AIGRANDSTRATEGY);

	if(iChange != 0)
	{
		m_paiGrandStrategyPriority[eGrandStrategy] += iChange;
	}
}










void CvGrandStrategyAI::DoGuessOtherPlayersActiveGrandStrategy()
{
	CvWeightedVector<int, 5, true> vGrandStrategyPriorities;
	FStaticVector< int, 5, true, c_eCiv5GameplayDLL >  vGrandStrategyPrioritiesForLogging;

	GuessConfidenceTypes eGuessConfidence = NO_GUESS_CONFIDENCE_TYPE;

#ifdef AUI_WARNING_FIXES
	uint iGrandStrategiesLoop = 0;
#else
	int iGrandStrategiesLoop = 0;
#endif
	AIGrandStrategyTypes eGrandStrategy = NO_AIGRANDSTRATEGY;
	CvAIGrandStrategyXMLEntry* pGrandStrategy = 0;
	CvString strGrandStrategyName;

	CvTeam& pTeam = GET_TEAM(GetPlayer()->getTeam());

	int iMajorLoop = 0;
	PlayerTypes eMajor = NO_PLAYER;

	int iPriority = 0;


	int iWorldMilitaryAverage = GC.getGame().GetWorldMilitaryStrengthAverage(GetPlayer()->GetID(), true, true);


	int iNumPlayersAlive = 0;
	int iWorldCultureAverage = 0;
	int iWorldTourismAverage = 0;
	for(iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;

		if(GET_PLAYER(eMajor).isAlive())
		{
			iWorldCultureAverage += GET_PLAYER(eMajor).GetJONSCultureEverGenerated();
			iWorldTourismAverage += GET_PLAYER(eMajor).GetCulture()->GetTourism();
			iNumPlayersAlive++;
		}
	}
	iWorldCultureAverage /= iNumPlayersAlive;
	iWorldTourismAverage /= iNumPlayersAlive;


	iNumPlayersAlive = 0;
	int iWorldNumTechsAverage = 0;
	TeamTypes eTeam;
	for(int iTeamLoop = 0; iTeamLoop < MAX_MAJOR_CIVS; iTeamLoop++)
	{
		eTeam = (TeamTypes) iTeamLoop;

		if(GET_TEAM(eTeam).isAlive())
		{
			iWorldNumTechsAverage += GET_TEAM(eTeam).GetTeamTechs()->GetNumTechsKnown();
			iNumPlayersAlive++;
		}
	}
	iWorldNumTechsAverage /= iNumPlayersAlive;


	for(iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;

		if(GET_PLAYER(eMajor).isAlive() && iMajorLoop != GetPlayer()->GetID())
		{
			if(pTeam.isHasMet(GET_PLAYER(eMajor).getTeam()))
			{
				for(iGrandStrategiesLoop = 0; iGrandStrategiesLoop < GetAIGrandStrategies()->GetNumAIGrandStrategies(); iGrandStrategiesLoop++)
				{
					eGrandStrategy = (AIGrandStrategyTypes) iGrandStrategiesLoop;
					pGrandStrategy = GetAIGrandStrategies()->GetEntry(iGrandStrategiesLoop);
					strGrandStrategyName = (CvString) pGrandStrategy->GetType();

					if(strGrandStrategyName == "AIGRANDSTRATEGY_CONQUEST")
					{
						iPriority = GetGuessOtherPlayerConquestPriority(eMajor, iWorldMilitaryAverage);
					}
					else if(strGrandStrategyName == "AIGRANDSTRATEGY_CULTURE")
					{
						iPriority = GetGuessOtherPlayerCulturePriority(eMajor, iWorldCultureAverage, iWorldTourismAverage);
					}
					else if(strGrandStrategyName == "AIGRANDSTRATEGY_UNITED_NATIONS")
					{
						iPriority = GetGuessOtherPlayerUnitedNationsPriority(eMajor);
					}
					else if(strGrandStrategyName == "AIGRANDSTRATEGY_SPACESHIP")
					{
						iPriority = GetGuessOtherPlayerSpaceshipPriority(eMajor, iWorldNumTechsAverage);
					}

					vGrandStrategyPriorities.push_back(iGrandStrategiesLoop, iPriority);
					vGrandStrategyPrioritiesForLogging.push_back(iPriority);
				}

				if(vGrandStrategyPriorities.size() > 0)
				{

					iPriority =        GC.getAI_GRAND_STRATEGY_GUESS_NO_CLUE_WEIGHT();

					vGrandStrategyPriorities.push_back(NO_AIGRANDSTRATEGY, iPriority);
					vGrandStrategyPrioritiesForLogging.push_back(iPriority);

					vGrandStrategyPriorities.SortItems();

					eGrandStrategy = (AIGrandStrategyTypes) vGrandStrategyPriorities.GetElement(0);
					iPriority = vGrandStrategyPriorities.GetWeight(0);
					eGuessConfidence = NO_GUESS_CONFIDENCE_TYPE;


					if(eGrandStrategy != NO_AIGRANDSTRATEGY)
					{
						if(iPriority >=         GC.getAI_GRAND_STRATEGY_GUESS_POSITIVE_THRESHOLD())
						{
							eGuessConfidence = GUESS_CONFIDENCE_POSITIVE;
						}
						else if(iPriority >=        GC.getAI_GRAND_STRATEGY_GUESS_LIKELY_THRESHOLD())
						{
							eGuessConfidence = GUESS_CONFIDENCE_LIKELY;
						}
						else
						{
							eGuessConfidence = GUESS_CONFIDENCE_UNSURE;
						}
					}

					SetGuessOtherPlayerActiveGrandStrategy(eMajor, eGrandStrategy, eGuessConfidence);

					LogGuessOtherPlayerGrandStrategy(vGrandStrategyPrioritiesForLogging, eMajor);
				}

				vGrandStrategyPriorities.clear();
				vGrandStrategyPrioritiesForLogging.clear();
			}
		}
	}
}


AIGrandStrategyTypes CvGrandStrategyAI::GetGuessOtherPlayerActiveGrandStrategy(PlayerTypes ePlayer) const
{
	FAssert(ePlayer < MAX_MAJOR_CIVS);
	return (AIGrandStrategyTypes) m_eGuessOtherPlayerActiveGrandStrategy[ePlayer];
}


GuessConfidenceTypes CvGrandStrategyAI::GetGuessOtherPlayerActiveGrandStrategyConfidence(PlayerTypes ePlayer) const
{
	FAssert(ePlayer < MAX_MAJOR_CIVS);
	return (GuessConfidenceTypes) m_eGuessOtherPlayerActiveGrandStrategyConfidence[ePlayer];
}


void CvGrandStrategyAI::SetGuessOtherPlayerActiveGrandStrategy(PlayerTypes ePlayer, AIGrandStrategyTypes eGrandStrategy, GuessConfidenceTypes eGuessConfidence)
{
	FAssert(ePlayer < MAX_MAJOR_CIVS);
	m_eGuessOtherPlayerActiveGrandStrategy[ePlayer] = eGrandStrategy;
	m_eGuessOtherPlayerActiveGrandStrategyConfidence[ePlayer] = eGuessConfidence;
}


int CvGrandStrategyAI::GetGuessOtherPlayerConquestPriority(PlayerTypes ePlayer, int iWorldMilitaryAverage)
{
	int iConquestPriority = 0;


	if(iWorldMilitaryAverage > 0)
	{
		iConquestPriority += (GET_PLAYER(ePlayer).GetMilitaryMight() - iWorldMilitaryAverage) *         GC.getAI_GRAND_STRATEGY_CONQUEST_POWER_RATIO_MULTIPLIER() / iWorldMilitaryAverage;
	}


	iConquestPriority += (GetPlayer()->GetDiplomacyAI()->GetOtherPlayerNumMinorsAttacked(ePlayer) *       GC.getAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MINOR_ATTACKED());


	iConquestPriority += (GetPlayer()->GetDiplomacyAI()->GetOtherPlayerNumMinorsConquered(ePlayer) *        GC.getAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MINOR_CONQUERED());


	iConquestPriority += (GetPlayer()->GetDiplomacyAI()->GetOtherPlayerNumMajorsAttacked(ePlayer) *        GC.getAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MAJOR_ATTACKED());


	iConquestPriority += (GetPlayer()->GetDiplomacyAI()->GetOtherPlayerNumMajorsConquered(ePlayer) *        GC.getAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MAJOR_CONQUERED());

	return iConquestPriority;
}


int CvGrandStrategyAI::GetGuessOtherPlayerCulturePriority(PlayerTypes ePlayer, int iWorldCultureAverage, int iWorldTourismAverage)
{
	VictoryTypes eVictory = (VictoryTypes) GC.getInfoTypeForString("VICTORY_CULTURAL", true);


	if(eVictory == NO_VICTORY)
	{
		return -100;
	}

	int iCulturePriority = 0;
	int iRatio;


	if(iWorldCultureAverage > 0)
	{
		iRatio = (GET_PLAYER(ePlayer).GetJONSCultureEverGenerated() - iWorldCultureAverage) *        GC.getAI_GS_CULTURE_RATIO_MULTIPLIER() / iWorldCultureAverage;
		if (iRatio > GC.getAI_GS_CULTURE_RATIO_MULTIPLIER())
		{
			iCulturePriority += GC.getAI_GS_CULTURE_RATIO_MULTIPLIER();
		}
		else if (iRatio < -GC.getAI_GS_CULTURE_RATIO_MULTIPLIER())
		{
			iCulturePriority += -GC.getAI_GS_CULTURE_RATIO_MULTIPLIER();
		}
		iCulturePriority += iRatio;
	}


	if(iWorldTourismAverage > 0)
	{
		iRatio = (GET_PLAYER(ePlayer).GetCulture()->GetTourism() - iWorldTourismAverage) *        GC.getAI_GS_TOURISM_RATIO_MULTIPLIER() / iWorldTourismAverage;
		if (iRatio > GC.getAI_GS_TOURISM_RATIO_MULTIPLIER())
		{
			iCulturePriority += GC.getAI_GS_TOURISM_RATIO_MULTIPLIER();
		}
		else if (iRatio < -GC.getAI_GS_TOURISM_RATIO_MULTIPLIER())
		{
			iCulturePriority += -GC.getAI_GS_TOURISM_RATIO_MULTIPLIER();
		}
		iCulturePriority += iRatio;	}

	return iCulturePriority;
}


int CvGrandStrategyAI::GetGuessOtherPlayerUnitedNationsPriority(PlayerTypes ePlayer)
{
	VictoryTypes eVictory = (VictoryTypes) GC.getInfoTypeForString("VICTORY_DIPLOMATIC", true);


	if(eVictory == NO_VICTORY)
	{
		return -100;
	}

	int iTheirCityStateAllies = 0;
	int iTheirCityStateFriends = 0;
	int iCityStatesAlive = 0;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if (eLoopPlayer != ePlayer && GET_PLAYER(eLoopPlayer).isAlive() && GET_PLAYER(eLoopPlayer).isMinorCiv())
		{
			iCityStatesAlive++;
			if (GET_PLAYER(eLoopPlayer).GetMinorCivAI()->IsAllies(ePlayer))
			{
				iTheirCityStateAllies++;
			}
			else if (GET_PLAYER(eLoopPlayer).GetMinorCivAI()->IsFriends(ePlayer))
			{
				iTheirCityStateFriends++;
			}
		}
	}
	iCityStatesAlive = MAX(iCityStatesAlive, 1);

	int iPriority = iTheirCityStateAllies + (iTheirCityStateFriends / 3);
	iPriority = iPriority * GC.getAI_GS_UN_SECURED_VOTE_MOD();
	iPriority = iPriority / iCityStatesAlive;

	return iPriority;
}


int CvGrandStrategyAI::GetGuessOtherPlayerSpaceshipPriority(PlayerTypes ePlayer, int iWorldNumTechsAverage)
{
	VictoryTypes eVictory = (VictoryTypes) GC.getInfoTypeForString("VICTORY_SPACE_RACE", true);


	if(eVictory == NO_VICTORY)
	{
		return -100;
	}

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();


	ProjectTypes eApolloProgram = (ProjectTypes) GC.getInfoTypeForString("PROJECT_APOLLO_PROGRAM", true);
	if(eApolloProgram != NO_PROJECT)
	{
		if(GET_TEAM(eTeam).getProjectCount(eApolloProgram) > 0)
		{
			return         GC.getAI_GS_SS_HAS_APOLLO_PROGRAM();
		}
	}

	int iNumTechs = GET_TEAM(eTeam).GetTeamTechs()->GetNumTechsKnown();


	if(iWorldNumTechsAverage == 0)
		iWorldNumTechsAverage = 1;

	int iSSPriority = (iNumTechs - iWorldNumTechsAverage) *         GC.getAI_GS_SS_TECH_PROGRESS_MOD() / iWorldNumTechsAverage;

	return iSSPriority;
}





void CvGrandStrategyAI::LogGrandStrategies(const FStaticVector< int, 5, true, c_eCiv5GameplayDLL >& vModifiedGrandStrategyPriorities)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strDesc;
		CvString strLogName;


		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "GrandStrategyAI_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "GrandStrategyAI_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		AIGrandStrategyTypes eGrandStrategy;


#ifdef AUI_WARNING_FIXES
		for (uint iGrandStrategyLoop = 0; iGrandStrategyLoop < GC.getNumAIGrandStrategyInfos(); iGrandStrategyLoop++)
#else
		for(int iGrandStrategyLoop = 0; iGrandStrategyLoop < GC.getNumAIGrandStrategyInfos(); iGrandStrategyLoop++)
#endif
		{

			strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
			strBaseString += playerName + ", ";

			eGrandStrategy = (AIGrandStrategyTypes) iGrandStrategyLoop;


			CvAIGrandStrategyXMLEntry* pEntry = GC.getAIGrandStrategyInfo(eGrandStrategy);
			const char* szAIGrandStrategyType = (pEntry != NULL)? pEntry->GetType() : "Unknown Type";

			if(GetActiveGrandStrategy() == eGrandStrategy)
			{
				strTemp.Format("*** %s, %d, %d", szAIGrandStrategyType, GetGrandStrategyPriority(eGrandStrategy), vModifiedGrandStrategyPriorities[eGrandStrategy]);
			}
			else
			{
				strTemp.Format("%s, %d, %d", szAIGrandStrategyType, GetGrandStrategyPriority(eGrandStrategy), vModifiedGrandStrategyPriorities[eGrandStrategy]);
			}
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}


void CvGrandStrategyAI::LogGuessOtherPlayerGrandStrategy(const FStaticVector< int, 5, true, c_eCiv5GameplayDLL >& vGrandStrategyPriorities, PlayerTypes ePlayer)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString otherPlayerName;
		CvString strDesc;
		CvString strLogName;


		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "GrandStrategyAI_Guess_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "GrandStrategyAI_Guess_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		AIGrandStrategyTypes eGrandStrategy;
		int iPriority;


#ifdef AUI_WARNING_FIXES
		for (uint iGrandStrategyLoop = 0; iGrandStrategyLoop < GC.getNumAIGrandStrategyInfos(); iGrandStrategyLoop++)
#else
		for(int iGrandStrategyLoop = 0; iGrandStrategyLoop < GC.getNumAIGrandStrategyInfos(); iGrandStrategyLoop++)
#endif
		{

			strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
			strBaseString += playerName + ", ";

			eGrandStrategy = (AIGrandStrategyTypes) iGrandStrategyLoop;
			iPriority = vGrandStrategyPriorities[iGrandStrategyLoop];

			CvAIGrandStrategyXMLEntry* pEntry = GC.getAIGrandStrategyInfo(eGrandStrategy);
			const char* szGrandStrategyType = (pEntry != NULL)? pEntry->GetType() : "Unknown Strategy";


			if(GetActiveGrandStrategy() == eGrandStrategy)
			{
				strTemp.Format("*** %s, %d", szGrandStrategyType, iPriority);
			}
			else
			{
				strTemp.Format("%s, %d", szGrandStrategyType, iPriority);
			}
			otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
			strOutBuf = strBaseString + otherPlayerName + ", " + strTemp;

			if(GetGuessOtherPlayerActiveGrandStrategy(ePlayer) == eGrandStrategy)
			{

				switch(GetGuessOtherPlayerActiveGrandStrategyConfidence(ePlayer))
				{
				case GUESS_CONFIDENCE_POSITIVE:
					strTemp.Format("Positive");
					break;
				case GUESS_CONFIDENCE_LIKELY:
					strTemp.Format("Likely");
					break;
				case GUESS_CONFIDENCE_UNSURE:
					strTemp.Format("Unsure");
					break;
				default:
					strTemp.Format("XXX");
					break;
				}

				strOutBuf += ", " + strTemp;
			}

			pLog->Msg(strOutBuf);
		}



		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		iPriority = vGrandStrategyPriorities[GC.getNumAIGrandStrategyInfos()];


		strTemp.Format("NO_GRAND_STRATEGY, %d", iPriority);
		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strOutBuf = strBaseString + otherPlayerName + ", " + strTemp;
		pLog->Msg(strOutBuf);
	}
}
