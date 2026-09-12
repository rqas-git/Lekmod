






#include "CvGameCoreDLLPCH.h"
#include "CvMinorCivAI.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvInternalGameCoreUtils.h"
#include "CvEnumSerialization.h"
#include "CvNotifications.h"
#include "CvDiplomacyAI.h"
#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
#include "CvFlavorManager.h"
#include <map>
#endif
#include "CvDllInterfaces.h"
#include "CvDllPlot.h"
#include "cvStopWatch.h"
#include "CvReligionClasses.h"


#include "LintFree.h"






CvMinorCivQuest::CvMinorCivQuest()
{
	m_eMinor = NO_PLAYER;
	m_eAssignedPlayer = NO_PLAYER;
	m_eType = NO_MINOR_CIV_QUEST_TYPE;
	m_iStartTurn = NO_TURN;
	m_iData1 = NO_QUEST_DATA;
	m_iData2 = NO_QUEST_DATA;
	m_bHandled = false;
}


CvMinorCivQuest::CvMinorCivQuest(PlayerTypes eMinor, PlayerTypes eAssignedPlayer, MinorCivQuestTypes eType)
{
	m_eMinor = eMinor;
	m_eAssignedPlayer = eAssignedPlayer;
	m_eType = eType;
	m_iStartTurn = NO_TURN;
	m_iData1 = NO_QUEST_DATA;
	m_iData2 = NO_QUEST_DATA;
	m_bHandled = false;
}

CvMinorCivQuest::~CvMinorCivQuest()
{
}


PlayerTypes CvMinorCivQuest::GetMinor() const
{
	return m_eMinor;
}


PlayerTypes CvMinorCivQuest::GetPlayerAssignedTo() const
{
	return m_eAssignedPlayer;
}


MinorCivQuestTypes CvMinorCivQuest::GetType() const
{
	return m_eType;
}


int CvMinorCivQuest::GetStartTurn() const
{
	return m_iStartTurn;
}


int CvMinorCivQuest::GetEndTurn() const
{
	CvAssertMsg(m_iStartTurn != NO_TURN, "GetEndTurn called for a quest, but the start turn was not initialized!");

	int iLength = 0;

	if(m_eType == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{
		iLength = GC.getMINOR_QUEST_STANDARD_CONTEST_LENGTH();
	}

	else if(m_eType == MINOR_CIV_QUEST_CONTEST_FAITH)
	{
		iLength = GC.getMINOR_QUEST_STANDARD_CONTEST_LENGTH();
	}

	else if(m_eType == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		iLength = GC.getMINOR_QUEST_STANDARD_CONTEST_LENGTH();
	}

	else if(m_eType == MINOR_CIV_QUEST_INVEST)
	{
		iLength = GC.getMINOR_QUEST_STANDARD_CONTEST_LENGTH();
	}

	else if(m_eType == MINOR_CIV_QUEST_GIVE_GOLD)
	{
		iLength = GC.getMINOR_QUEST_STANDARD_CONTEST_LENGTH();
	}

	else if(m_eType == MINOR_CIV_QUEST_PLEDGE_TO_PROTECT)
	{
		iLength = GC.getMINOR_QUEST_STANDARD_CONTEST_LENGTH();
	}

	else if(m_eType == MINOR_CIV_QUEST_BULLY_CITY_STATE)
	{
		iLength = GC.getMINOR_QUEST_STANDARD_CONTEST_LENGTH();
	}

	else if(m_eType == MINOR_CIV_QUEST_DENOUNCE_MAJOR)
	{
		iLength = GC.getMINOR_QUEST_STANDARD_CONTEST_LENGTH();
	}


	else
	{
		return NO_TURN;
	}


	iLength *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iLength /= 100;

	return m_iStartTurn + iLength;
}


int CvMinorCivQuest::GetTurnsRemaining(int iCurrentTurn) const
{
	int iStartTurn = GetStartTurn();
	int iEndTurn = GetEndTurn();

	if (iStartTurn == NO_TURN)
		return NO_TURN;

	if (iEndTurn == NO_TURN)
		return NO_TURN;

	return (iEndTurn - iCurrentTurn);
}


int CvMinorCivQuest::GetPrimaryData() const
{
	return m_iData1;
}


int CvMinorCivQuest::GetSecondaryData() const
{
	return m_iData2;
}


int CvMinorCivQuest::GetInfluenceReward() const
{
	int iReward = 0;

	switch(m_eType)
	{
	case MINOR_CIV_QUEST_ROUTE:
		iReward =        GC.getMINOR_QUEST_FRIENDSHIP_ROUTE();
		break;
	case MINOR_CIV_QUEST_KILL_CAMP:
		iReward =        GC.getMINOR_QUEST_FRIENDSHIP_KILL_CAMP();
		break;
	case MINOR_CIV_QUEST_CONNECT_RESOURCE:
		iReward =        GC.getMINOR_QUEST_FRIENDSHIP_CONNECT_RESOURCE();
		break;
	case MINOR_CIV_QUEST_CONSTRUCT_WONDER:
		iReward =        GC.getMINOR_QUEST_FRIENDSHIP_CONSTRUCT_WONDER();
		break;
	case MINOR_CIV_QUEST_GREAT_PERSON:
		iReward =        GC.getMINOR_QUEST_FRIENDSHIP_GREAT_PERSON();
		break;
	case MINOR_CIV_QUEST_KILL_CITY_STATE:
		iReward =        GC.getMINOR_QUEST_FRIENDSHIP_KILL_CITY_STATE();
		break;
	case MINOR_CIV_QUEST_FIND_PLAYER:
		iReward =        GC.getMINOR_QUEST_FRIENDSHIP_FIND_PLAYER();
		break;
	case MINOR_CIV_QUEST_FIND_NATURAL_WONDER:
		iReward =        GC.getMINOR_QUEST_FRIENDSHIP_FIND_NATURAL_WONDER();
		break;
	case MINOR_CIV_QUEST_GIVE_GOLD:
		iReward =        GC.getMINOR_QUEST_FRIENDSHIP_GIVE_GOLD();
		break;
	case MINOR_CIV_QUEST_PLEDGE_TO_PROTECT:
		iReward =        GC.getMINOR_QUEST_FRIENDSHIP_PLEDGE_TO_PROTECT();
		break;
	case MINOR_CIV_QUEST_CONTEST_CULTURE:
		iReward =        GC.getMINOR_QUEST_FRIENDSHIP_CONTEST_CULTURE();
		break;
	case MINOR_CIV_QUEST_CONTEST_FAITH:
		iReward =        GC.getMINOR_QUEST_FRIENDSHIP_CONTEST_FAITH();
		break;
	case MINOR_CIV_QUEST_CONTEST_TECHS:
		iReward =        GC.getMINOR_QUEST_FRIENDSHIP_CONTEST_TECHS();
		break;
	case MINOR_CIV_QUEST_INVEST:

		iReward =       GC.getMINOR_QUEST_FRIENDSHIP_INVEST();
		break;
	case MINOR_CIV_QUEST_BULLY_CITY_STATE:
		iReward =        GC.getMINOR_QUEST_FRIENDSHIP_BULLY_CITY_STATE();
		break;
	case MINOR_CIV_QUEST_DENOUNCE_MAJOR:
		iReward =        GC.getMINOR_QUEST_FRIENDSHIP_DENOUNCE_MAJOR();
		break;
	case MINOR_CIV_QUEST_SPREAD_RELIGION:
		iReward =        GC.getMINOR_QUEST_FRIENDSHIP_SPREAD_RELIGION();
		break;
	case MINOR_CIV_QUEST_TRADE_ROUTE:
		iReward =        GC.getMINOR_QUEST_FRIENDSHIP_TRADE_ROUTE();
		break;
	default:
		iReward = 0;
		break;
	}

	return iReward;
}



int CvMinorCivQuest::GetContestValueForPlayer(PlayerTypes ePlayer)
{
	MinorCivQuestTypes eType = GetType();
	int iValue = -1;
	CvPlayer* pMinor = &GET_PLAYER(GetMinor());
	if(!pMinor)
	{
		CvAssertMsg(false, "Error when calculating contest standings - could not find minor civ! Please send Anton your save file and version.");
		return iValue;
	}


	if(!pMinor->GetMinorCivAI()->IsActiveQuestForPlayer(ePlayer, eType))
		return iValue;

	if(eType == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{
		int iStartCulture = pMinor->GetMinorCivAI()->GetQuestData1(ePlayer, eType);
		int iEndCulture = GET_PLAYER(ePlayer).GetJONSCultureEverGenerated();
		iValue = iEndCulture - iStartCulture;
	}
	else if(eType == MINOR_CIV_QUEST_CONTEST_FAITH)
	{
		int iStartFaith = pMinor->GetMinorCivAI()->GetQuestData1(ePlayer, eType);
		int iEndFaith = GET_PLAYER(ePlayer).GetFaithEverGenerated();
		iValue = iEndFaith - iStartFaith;
	}
	else if(eType == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		int iStartTechs = pMinor->GetMinorCivAI()->GetQuestData1(ePlayer, eType);
		int iEndTechs = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetTeamTechs()->GetNumTechsKnown();
		iValue = iEndTechs - iStartTechs;
	}

	return iValue;
}

int CvMinorCivQuest::GetContestValueForLeader()
{
	MinorCivQuestTypes eType = GetType();
	int iHighestValue = -1;

	if(eType == MINOR_CIV_QUEST_CONTEST_CULTURE ||
	        eType == MINOR_CIV_QUEST_CONTEST_FAITH ||
	        eType == MINOR_CIV_QUEST_CONTEST_TECHS)
	{

		for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes ePlayerLoop = (PlayerTypes) iPlayerLoop;

			int iPlayerValue = GetContestValueForPlayer(ePlayerLoop);
			if(iPlayerValue > iHighestValue)
			{
				iHighestValue = iPlayerValue;
			}
		}
	}

	return iHighestValue;
}


CivsList CvMinorCivQuest::GetContestLeaders()
{
	MinorCivQuestTypes eType = GetType();
	CivsList veTiedForLead;
	int iHighestValue = GetContestValueForLeader();

	if(eType == MINOR_CIV_QUEST_CONTEST_CULTURE ||
	        eType == MINOR_CIV_QUEST_CONTEST_FAITH ||
	        eType == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes ePlayerLoop = (PlayerTypes) iPlayerLoop;
			int iPlayerValue = GetContestValueForPlayer(ePlayerLoop);
			CvAssertMsg(iPlayerValue <= iHighestValue, "Calculation error for calculating the leaders of a contest! Please send Anton your save file and version.");
			if(iPlayerValue == iHighestValue)
				veTiedForLead.push_back(ePlayerLoop);
		}
	}

	return veTiedForLead;
}


bool CvMinorCivQuest::IsContestLeader(PlayerTypes ePlayer)
{
	if(ePlayer == NO_PLAYER)
		ePlayer = GetPlayerAssignedTo();

	MinorCivQuestTypes eType = GetType();
	CvPlayer* pMinor = &GET_PLAYER(GetMinor());
	if(!pMinor)
	{
		CvAssertMsg(false, "Error when calculating contest standings - could not find minor civ! Please send Anton your save file and version.");
		return false;
	}

	if(!pMinor->GetMinorCivAI()->IsActiveQuestForPlayer(ePlayer, eType))
		return false;

	if(eType == MINOR_CIV_QUEST_CONTEST_CULTURE ||
	        eType == MINOR_CIV_QUEST_CONTEST_FAITH ||
	        eType == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		CivsList veTiedForLead = GetContestLeaders();
		for(uint ui = 0; ui < veTiedForLead.size(); ui++)
		{
			if(ePlayer == veTiedForLead[ui])
			{
				return true;
			}
		}
	}

	return false;
}



bool CvMinorCivQuest::IsComplete()
{
	CvPlayer* pMinor = &GET_PLAYER(m_eMinor);
	CvPlayer* pAssignedPlayer = &GET_PLAYER(m_eAssignedPlayer);

	if(!pMinor)
	{
		CvAssertMsg(false, "Minor player not found! Please send Anton your save file and version.");
		return false;
	}
	if(!pAssignedPlayer)
	{
		CvAssertMsg(false, "Assigned player not found! Please send Anton your save file and version.");
		return false;
	}

	if(m_eType == MINOR_CIV_QUEST_ROUTE)
	{
		if (pAssignedPlayer->IsCapitalConnectedToPlayer(m_eMinor))
		{
			return true;
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_KILL_CAMP)
	{
		int iX = m_iData1;
		int iY = m_iData2;
		CvPlot* pPlot = GC.getMap().plot(iX, iY);

		if(pPlot)
		{

			if(pPlot->getImprovementType() != GC.getBARBARIAN_CAMP_IMPROVEMENT())
			{

				if(pPlot->GetPlayerThatClearedBarbCampHere() == m_eAssignedPlayer)
				{
					return true;
				}
			}
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_CONNECT_RESOURCE)
	{
		ResourceTypes eResource = (ResourceTypes) m_iData1;


		if(pAssignedPlayer->getNumResourceTotal(eResource,                    true) > 0)
		{
			return true;
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
	{
		BuildingTypes eWonder = (BuildingTypes) m_iData1;


		if(pAssignedPlayer->countNumBuildings(eWonder) > 0)
		{
			return true;
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_GREAT_PERSON)
	{
		UnitTypes eUnit = (UnitTypes) m_iData1;


		int iLoop;
		for(CvUnit* pLoopUnit = pAssignedPlayer->firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = pAssignedPlayer->nextUnit(&iLoop))
		{
			if(pLoopUnit->getUnitType() == eUnit)
			{
				return true;
			}
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_KILL_CITY_STATE)
	{
		PlayerTypes eTargetCityState = (PlayerTypes) m_iData1;
		CvPlayer* pTargetCityState = &GET_PLAYER(eTargetCityState);
		if(pTargetCityState)
		{
			CvTeam* pTargetCityStateTeam = &GET_TEAM(pTargetCityState->getTeam());
			if(pTargetCityStateTeam)
			{

				if(pTargetCityStateTeam->GetKilledByTeam() == pAssignedPlayer->getTeam())
				{
					return true;
				}
			}
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_FIND_PLAYER)
	{
		PlayerTypes ePlayerToFind = (PlayerTypes) m_iData1;


		if(GET_TEAM(pAssignedPlayer->getTeam()).IsHasFoundPlayersTerritory(ePlayerToFind))
		{
			return true;
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_FIND_NATURAL_WONDER)
	{
		int iNumWondersFoundBefore = m_iData1;


		if(GET_TEAM(pAssignedPlayer->getTeam()).GetNumNaturalWondersDiscovered() > iNumWondersFoundBefore)
		{
			return true;
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_GIVE_GOLD)
	{
		int iGoldGiftedBefore = m_iData2;


		if(pMinor->GetMinorCivAI()->GetNumGoldGifted(m_eAssignedPlayer) > iGoldGiftedBefore)
		{
			return true;
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_PLEDGE_TO_PROTECT)
	{

		if(pMinor->GetMinorCivAI()->IsProtectedByMajor(m_eAssignedPlayer))
		{
			return true;
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{

		if(GetEndTurn() == GC.getGame().getGameTurn())
			if(IsContestLeader(GetPlayerAssignedTo()))
				return true;
	}
	else if(m_eType == MINOR_CIV_QUEST_CONTEST_FAITH)
	{

		if(GetEndTurn() == GC.getGame().getGameTurn())
			if(IsContestLeader(GetPlayerAssignedTo()))
				return true;
	}
	else if(m_eType == MINOR_CIV_QUEST_CONTEST_TECHS)
	{

		if(GetEndTurn() == GC.getGame().getGameTurn())
			if(IsContestLeader(GetPlayerAssignedTo()))
				return true;
	}
	else if(m_eType == MINOR_CIV_QUEST_INVEST)
	{
		if(GetEndTurn() == GC.getGame().getGameTurn())
			return true;
	}
	else if(m_eType == MINOR_CIV_QUEST_BULLY_CITY_STATE)
	{

		PlayerTypes eTargetMinor = (PlayerTypes) m_iData1;
		CvPlayer* pTargetMinor = &GET_PLAYER(eTargetMinor);
		if(pTargetMinor)
		{
			int iMostRecentBullyTurn = pTargetMinor->GetMinorCivAI()->GetTurnLastBulliedByMajor(m_eAssignedPlayer);
			if(iMostRecentBullyTurn > m_iData2)
				return true;
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_DENOUNCE_MAJOR)
	{

		PlayerTypes eTargetMajor = (PlayerTypes) m_iData1;
		CvPlayer* pTargetMajor = &GET_PLAYER(eTargetMajor);
		if(pTargetMajor)
		{
			if(pAssignedPlayer->GetDiplomacyAI()->IsDenouncedPlayer(eTargetMajor))
				return true;
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_SPREAD_RELIGION)
	{

		ReligionTypes eReligion = (ReligionTypes) m_iData1;
		if(pMinor->getCapitalCity())
		{
			if(pMinor->getCapitalCity()->GetCityReligions()->GetReligiousMajority() == eReligion)
				return true;
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_TRADE_ROUTE)
	{
		if(GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(m_eMinor,m_eAssignedPlayer))
			return true;
	}

	return false;
}


bool CvMinorCivQuest::IsRevoked()
{
	if(GET_PLAYER(m_eMinor).GetMinorCivAI()->IsRecentlyBulliedByMajor(m_eAssignedPlayer))
	{
		if(m_eType == MINOR_CIV_QUEST_ROUTE)
			return true;
		if(m_eType == MINOR_CIV_QUEST_CONNECT_RESOURCE)
			return true;
		if(m_eType == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
			return true;
		if(m_eType == MINOR_CIV_QUEST_GREAT_PERSON)
			return true;
		if(m_eType == MINOR_CIV_QUEST_FIND_PLAYER)
			return true;
		if(m_eType == MINOR_CIV_QUEST_FIND_NATURAL_WONDER)
			return true;
		if(m_eType == MINOR_CIV_QUEST_GIVE_GOLD)
			return true;
		if(m_eType == MINOR_CIV_QUEST_PLEDGE_TO_PROTECT)
			return true;
		if(m_eType == MINOR_CIV_QUEST_INVEST)
			return true;
		if(m_eType == MINOR_CIV_QUEST_BULLY_CITY_STATE)
			return true;
		if(m_eType == MINOR_CIV_QUEST_DENOUNCE_MAJOR)
			return true;
		if(m_eType == MINOR_CIV_QUEST_SPREAD_RELIGION)
			return true;
		if(m_eType == MINOR_CIV_QUEST_TRADE_ROUTE)
			return true;
	}

	return false;
}


bool CvMinorCivQuest::IsExpired()
{

	if(GetEndTurn() != NO_TURN && GC.getGame().getGameTurn() > GetEndTurn())
	{
		return true;
	}


	if(m_eType == MINOR_CIV_QUEST_ROUTE)
	{
	}


	if(m_eType == MINOR_CIV_QUEST_KILL_CAMP)
	{
		int iX = GetPrimaryData();
		int iY = GetSecondaryData();
		CvPlot* pPlot = GC.getMap().plot(iX, iY);

		if(pPlot)
		{

			if(!pPlot->HasBarbarianCamp())
			{

				if(pPlot->GetPlayerThatClearedBarbCampHere() != NO_PLAYER && pPlot->GetPlayerThatClearedBarbCampHere() != m_eAssignedPlayer)
				{
					return true;
				}
			}
		}
	}


	if(m_eType == MINOR_CIV_QUEST_CONNECT_RESOURCE)
	{
	}


	else if(m_eType == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
	{
		BuildingTypes eWonder = (BuildingTypes) GetPrimaryData();
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eWonder);
		CvAssertMsg(pkBuildingInfo, "Building info not expected to be FALSE! Please send Anton your save file and version.");
		if(!pkBuildingInfo) return false;

		for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			const PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
			CvPlayer* pLoopPlayer = &GET_PLAYER(eLoopPlayer);

			if(pLoopPlayer)
			{

				if(m_eAssignedPlayer != eLoopPlayer && pLoopPlayer->countNumBuildings(eWonder) > 0)
				{
					return true;
				}
#if defined(LEKMOD_CITYSTATE_QUEST_CHANGES)
				const TechTypes eRequiredTech = (TechTypes)pkBuildingInfo->GetPrereqAndTech();
				const int iWonderEra = GC.getTechInfo(eRequiredTech)->GetEra();
				if ((iWonderEra + 1) < pLoopPlayer->GetCurrentEra() && m_eAssignedPlayer == eLoopPlayer)
					return true;
#endif
			}
		}
	}


	else if(m_eType == MINOR_CIV_QUEST_GREAT_PERSON)
	{
	}


	else if(m_eType == MINOR_CIV_QUEST_KILL_CITY_STATE)
	{
		PlayerTypes eTargetCityState = (PlayerTypes) GetPrimaryData();
		CvPlayer* pTargetCityState = &GET_PLAYER(eTargetCityState);

		if(pTargetCityState)
		{
			CvTeam* pTargetCityStateTeam = &GET_TEAM(pTargetCityState->getTeam());
			if(pTargetCityStateTeam)
			{

				if(!pTargetCityState->isAlive() && pTargetCityStateTeam->GetKilledByTeam() != GET_PLAYER(m_eAssignedPlayer).getTeam())
				{
					return true;
				}
			}
		}
	}


	else if(m_eType == MINOR_CIV_QUEST_FIND_PLAYER)
	{
		PlayerTypes eTargetPlayer = (PlayerTypes) GetPrimaryData();
		CvPlayer* pTargetPlayer = &GET_PLAYER(eTargetPlayer);

		if(pTargetPlayer)
		{

			if(!GET_PLAYER(eTargetPlayer).isAlive())
				return true;
		}
	}


	else if(m_eType == MINOR_CIV_QUEST_FIND_NATURAL_WONDER)
	{
	}


	else if(m_eType == MINOR_CIV_QUEST_GIVE_GOLD)
	{
	}


	else if(m_eType == MINOR_CIV_QUEST_PLEDGE_TO_PROTECT)
	{
	}


	else if(m_eType == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{
		if(GC.getGame().getGameTurn() == GetEndTurn() && !IsComplete())
			return true;
	}


	else if(m_eType == MINOR_CIV_QUEST_CONTEST_FAITH)
	{
		if(GC.getGame().getGameTurn() == GetEndTurn() && !IsComplete())
			return true;
	}


	else if(m_eType == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		if(GC.getGame().getGameTurn() == GetEndTurn() && !IsComplete())
			return true;
	}


	else if(m_eType == MINOR_CIV_QUEST_INVEST)
	{
	}


	else if(m_eType == MINOR_CIV_QUEST_BULLY_CITY_STATE)
	{
		PlayerTypes eTargetCityState = (PlayerTypes) GetPrimaryData();
		CvPlayer* pTargetCityState = &GET_PLAYER(eTargetCityState);

		if(pTargetCityState)
		{

			if(!pTargetCityState->isAlive())
				return true;
		}
	}


	else if(m_eType == MINOR_CIV_QUEST_DENOUNCE_MAJOR)
	{
		PlayerTypes eTargetPlayer = (PlayerTypes) GetPrimaryData();
		CvPlayer* pTargetPlayer = &GET_PLAYER(eTargetPlayer);

		if(pTargetPlayer)
		{

			if(!pTargetPlayer->isAlive())
				return true;
		}
	}


	else if(m_eType == MINOR_CIV_QUEST_SPREAD_RELIGION)
	{

	}
	

	else if(m_eType == MINOR_CIV_QUEST_TRADE_ROUTE)
	{
	}

	return false;
}

bool CvMinorCivQuest::IsObsolete()
{
	return (IsRevoked() || IsExpired());
}


bool CvMinorCivQuest::IsHandled()
{
	return m_bHandled;
}

void CvMinorCivQuest::SetHandled(bool bValue)
{
	m_bHandled = bValue;
}




void CvMinorCivQuest::DoStartQuest(int iStartTurn)
{
	m_iStartTurn = iStartTurn;

	CvPlayer* pMinor = &GET_PLAYER(m_eMinor);
	CvPlayer* pAssignedPlayer = &GET_PLAYER(m_eAssignedPlayer);

	Localization::String strMessage;
	Localization::String strSummary;
	int iNotificationX = -1;
	int iNotificationY = -1;


	if(m_eType == MINOR_CIV_QUEST_ROUTE)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_START_ROUTE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_START_ROUTE");
	}

	else if(m_eType == MINOR_CIV_QUEST_KILL_CAMP)
	{
		CvPlot* pPlot = pMinor->GetMinorCivAI()->GetBestNearbyCampToKill();

		FAssertMsg(pPlot != NULL, "MINOR CIV AI: Somehow we're starting a quest to kill a barb camp but we can't find one nearby. Please send Jon this with your last 5 autosaves and what changelist # you're playing. Oh, and you're about to crash.");

		m_iData1 = pPlot->getX();
		m_iData2 = pPlot->getY();

		pPlot->setRevealed(pAssignedPlayer->getTeam(), true);
		pPlot->setRevealedImprovementType(pAssignedPlayer->getTeam(), pPlot->getImprovementType());

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_KILL_CAMP");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_KILL_CAMP");
		iNotificationX = pPlot->getX();
		iNotificationY = pPlot->getY();
	}

	else if(m_eType == MINOR_CIV_QUEST_CONNECT_RESOURCE)
	{
		ResourceTypes eResource = pMinor->GetMinorCivAI()->GetNearbyResourceForQuest(m_eAssignedPlayer);

		FAssertMsg(eResource != NO_RESOURCE, "MINOR CIV AI: For some reason we got NO_RESOURCE when starting a quest for a major to find a Resource. Please send Jon this with your last 5 autosaves and what changelist # you're playing. Bad things are probably going to happen.");

		m_iData1 = eResource;

		const char* strResourceName = GC.getResourceInfo(eResource)->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_CONNECT_RESOURCE");
		strMessage << strResourceName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CONNECT_RESOURCE");
		strSummary << strResourceName;
	}

	else if(m_eType == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
	{
		BuildingTypes eWonder = pMinor->GetMinorCivAI()->GetBestWonderForQuest(m_eAssignedPlayer);

		FAssertMsg(eWonder != NO_BUILDING, "MINOR CIV AI: For some reason we got NO_BUILDING when starting a quest for a major to find a Wonder. Please send Jon this with your last 5 autosaves and what changelist # you're playing. Bad things are probably going to happen.");

		m_iData1 = eWonder;

#ifdef AUI_WARNING_FIXES
		const char* strBuildingName;
		CvBuildingEntry* pWonderInfo = GC.getBuildingInfo(eWonder);
		if (pWonderInfo)
			strBuildingName = pWonderInfo->GetDescriptionKey();
#else
		const char* strBuildingName = GC.getBuildingInfo(eWonder)->GetDescriptionKey();
#endif

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_CONSTRUCT_WONDER");
		strMessage << strBuildingName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CONSTRUCT_WONDER");
		strSummary << strBuildingName;
	}

	else if(m_eType == MINOR_CIV_QUEST_GREAT_PERSON)
	{
		UnitTypes eUnit = pMinor->GetMinorCivAI()->GetBestGreatPersonForQuest(m_eAssignedPlayer);

		FAssertMsg(eUnit != NO_UNIT, "MINOR CIV AI: For some reason we got NO_UNIT when starting a quest for a major to find a Great Person. Please send Jon this with your last 5 autosaves and what changelist # you're playing. Bad things are probably going to happen.");

		m_iData1 = eUnit;

#ifdef AUI_WARNING_FIXES
		const char* strUnitName;
		CvUnitEntry* pUnitInfo = GC.getUnitInfo(eUnit);
		if (pUnitInfo)
			strUnitName = pUnitInfo->GetDescriptionKey();
#else
		const char* strUnitName = GC.getUnitInfo(eUnit)->GetDescriptionKey();
#endif

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_GREAT_PERSON");
		strMessage << strUnitName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_GREAT_PERSON");
		strSummary << strUnitName;
	}

	else if(m_eType == MINOR_CIV_QUEST_KILL_CITY_STATE)
	{
		PlayerTypes eTargetCityState = pMinor->GetMinorCivAI()->GetBestCityStateTarget(m_eAssignedPlayer);

		FAssertMsg(eTargetCityState != NO_PLAYER, "MINOR CIV AI: For some reason we got NO_PLAYER when starting a quest for a major to kill a City State. Please send Jon this with your last 5 autosaves and what changelist # you're playing. Bad things are probably going to happen.");

		m_iData1 = eTargetCityState;

		const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_KILL_CITY_STATE");
		strMessage << strTargetNameKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_KILL_CITY_STATE");
		strSummary << strTargetNameKey;
	}

	else if(m_eType == MINOR_CIV_QUEST_FIND_PLAYER)
	{
		PlayerTypes ePlayerToFind = pMinor->GetMinorCivAI()->GetBestPlayerToFind(m_eAssignedPlayer);

		FAssertMsg(ePlayerToFind != NO_PLAYER, "MINOR CIV AI: For some reason we got NO_PLAYER when starting a quest for a major to find a player. Please send Jon this with your last 5 autosaves and what changelist # you're playing. Bad things are probably going to happen.");

		m_iData1 = ePlayerToFind;

		const char* strCivKey = GET_PLAYER(ePlayerToFind).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_FIND_PLAYER");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_FIND_PLAYER");
		strSummary << strCivKey;
	}

	else if(m_eType == MINOR_CIV_QUEST_FIND_NATURAL_WONDER)
	{
		int iNumWondersAlreadyFound = GET_TEAM(pAssignedPlayer->getTeam()).GetNumNaturalWondersDiscovered();

		m_iData1 = iNumWondersAlreadyFound;

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_FIND_NATURAL_WONDER");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_FIND_NATURAL_WONDER");
	}

	else if(m_eType == MINOR_CIV_QUEST_GIVE_GOLD)
	{
		PlayerTypes eMostRecentBully = pMinor->GetMinorCivAI()->GetMostRecentBullyForQuest();
		int iGoldAlreadyGiven = pMinor->GetMinorCivAI()->GetNumGoldGifted(m_eAssignedPlayer);

		CvAssertMsg(eMostRecentBully != NO_PLAYER, "MINOR CIV AI: eMostRecentBully should not be NO_PLAYER when giving a Give Gold quest! Please send Anton your save file and version.");

		m_iData1 = eMostRecentBully;
		m_iData2 = iGoldAlreadyGiven;

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_GIVE_GOLD");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_GIVE_GOLD");

		if(eMostRecentBully != NO_PLAYER)
		{
			const char* strCivKey = GET_PLAYER(eMostRecentBully).getCivilizationShortDescriptionKey();
			strMessage << strCivKey;
		}
		else
		{
			const char* strCivKey = "Nobody";
			strMessage << strCivKey;
		}
	}

	else if(m_eType == MINOR_CIV_QUEST_PLEDGE_TO_PROTECT)
	{
		PlayerTypes eMostRecentBully = pMinor->GetMinorCivAI()->GetMostRecentBullyForQuest();

		CvAssertMsg(eMostRecentBully != NO_PLAYER, "MINOR CIV AI: eMostRecentBully should not be NO_PLAYER when giving a Pledge to Protect quest! Please send Anton your save file and version.");

		m_iData1 = eMostRecentBully;

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_PLEDGE_TO_PROTECT");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_PLEDGE_TO_PROTECT");

		if(eMostRecentBully != NO_PLAYER)
		{
			const char* strCivKey = GET_PLAYER(eMostRecentBully).getCivilizationShortDescriptionKey();
			strMessage << strCivKey;
		}
		else
		{
			const char* strCivKey = "Nobody";
			strMessage << strCivKey;
		}
	}

	else if(m_eType == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{
		int iStartingCulture = pAssignedPlayer->GetJONSCultureEverGenerated();

		m_iData1 = iStartingCulture;

		int iTurnsRemaining = GetEndTurn() - GC.getGame().getGameTurn();
		int iTurnsDuration = GetEndTurn() - GetStartTurn();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_CONTEST_CULTURE");
		strMessage << iTurnsRemaining;
		strMessage << iTurnsDuration;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CONTEST_CULTURE");
	}

	else if(m_eType == MINOR_CIV_QUEST_CONTEST_FAITH)
	{
		int iStartingFaith = pAssignedPlayer->GetFaithEverGenerated();

		m_iData1 = iStartingFaith;

		int iTurnsRemaining = GetEndTurn() - GC.getGame().getGameTurn();
		int iTurnsDuration = GetEndTurn() - GetStartTurn();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_CONTEST_FAITH");
		strMessage << iTurnsRemaining;
		strMessage << iTurnsDuration;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CONTEST_FAITH");
	}

	else if(m_eType == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		int iStartingTechs = GET_TEAM(pAssignedPlayer->getTeam()).GetTeamTechs()->GetNumTechsKnown();

		m_iData1 = iStartingTechs;

		int iTurnsRemaining = GetEndTurn() - GC.getGame().getGameTurn();
		int iTurnsDuration = GetEndTurn() - GetStartTurn();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_CONTEST_TECHS");
		strMessage << iTurnsRemaining;
		strMessage << iTurnsDuration;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CONTEST_TECHS");
	}

	else if(m_eType == MINOR_CIV_QUEST_INVEST)
	{
		int iGoldAlreadyGiven = pMinor->GetMinorCivAI()->GetNumGoldGifted(m_eAssignedPlayer);

		m_iData1 = iGoldAlreadyGiven;

		int iTurnsRemaining = GetEndTurn() - GC.getGame().getGameTurn();
		int iBoostPercentage = 50;

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_INVEST");
		strMessage << iTurnsRemaining;
		strMessage << iBoostPercentage;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_INVEST");
	}

	else if(m_eType == MINOR_CIV_QUEST_BULLY_CITY_STATE)
	{
		PlayerTypes eTargetMinor = NO_PLAYER;
#if defined(LEKMOD_CITYSTATE_QUEST_CHANGES)
		eTargetMinor = pMinor->GetMinorCivAI()->GetBestBullyQuestTarget(m_eAssignedPlayer);
#else
		eTargetMinor = pMinor->GetMinorCivAI()->GetBestCityStateTarget(m_eAssignedPlayer);
#endif
#if defined(LEKMOD_CITYSTATE_QUEST_CHANGES)
		if(eTargetMinor == NO_PLAYER)
		{
			return;
		}
#endif
		CvAssertMsg(eTargetMinor != NO_PLAYER, "MINOR CIV AI: eTargetMinor should not be NO_PLAYER when giving a Bully CS quest! Please send Anton your save file and version.");
		int iLastBullyTurn = GET_PLAYER(eTargetMinor).GetMinorCivAI()->GetTurnLastBulliedByMajor(m_eAssignedPlayer);

		m_iData1 = eTargetMinor;
		m_iData2 = iLastBullyTurn;

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_BULLY_CITY_STATE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_BULLY_CITY_STATE");

		if(eTargetMinor != NO_PLAYER)
		{
			const char* strCivKey = GET_PLAYER(eTargetMinor).getCivilizationShortDescriptionKey();
			strMessage << strCivKey;
			strSummary << strCivKey;
		}
		else
		{
			const char* strCivKey = "Nobody";
			strMessage << strCivKey;
			strSummary << strCivKey;
		}
	}

	else if(m_eType == MINOR_CIV_QUEST_DENOUNCE_MAJOR)
	{
		PlayerTypes eMostRecentBully = pMinor->GetMinorCivAI()->GetMostRecentBullyForQuest();

		CvAssertMsg(eMostRecentBully != NO_PLAYER, "MINOR CIV AI: eMostRecentBully should not be NO_PLAYER when giving a Denounce Major quest! Please send Anton your save file and version.");

		m_iData1 = eMostRecentBully;

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_DENOUNCE_MAJOR");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_DENOUNCE_MAJOR");

		if(eMostRecentBully != NO_PLAYER)
		{
			const char* strCivKey = GET_PLAYER(eMostRecentBully).getCivilizationShortDescriptionKey();
			strMessage << strCivKey;
			strSummary << strCivKey;
		}
		else
		{
			const char* strCivKey = "Nobody";
			strMessage << strCivKey;
			strSummary << strCivKey;
		}
	}

	else if(m_eType == MINOR_CIV_QUEST_SPREAD_RELIGION)
	{
		ReligionTypes eReligion = GC.getGame().GetGameReligions()->GetReligionCreatedByPlayer(m_eAssignedPlayer);

		CvAssertMsg(eReligion != NO_RELIGION, "MINOR CIV AI: eReligion should not be NO_RELIGION when giving a Spread Religion quest! Please send Anton your save file and version.");

		m_iData1 = eReligion;

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_SPREAD_RELIGION");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_SPREAD_RELIGION");

		if(eReligion != NO_RELIGION)
		{
			const CvReligion* pkReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);
			CvString strReligion = pkReligion->GetName();
			strMessage << strReligion;
			strSummary << strReligion;
		}
		else
		{
			const char* strReligion = "No Religion";
			strMessage << strReligion;
			strSummary << strReligion;
		}
	}

	else if(m_eType == MINOR_CIV_QUEST_TRADE_ROUTE)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_START_TRADE_ROUTE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_START_TRADE_ROUTE");
	}

	strMessage << pMinor->getNameKey();
	strSummary << pMinor->getNameKey();
	pMinor->GetMinorCivAI()->AddQuestNotification(strMessage.toUTF8(), strSummary.toUTF8(), m_eAssignedPlayer, iNotificationX, iNotificationY);
}



void CvMinorCivQuest::DoStartQuestUsingExistingData(CvMinorCivQuest* pExistingQuest)
{
	CvPlayer* pMinor = &GET_PLAYER(m_eMinor);
	CvPlayer* pAssignedPlayer = &GET_PLAYER(m_eAssignedPlayer);

	CvAssertMsg(pMinor != NULL, "pMinor should not be NULL. Please send Anton your save file and version.");
	CvAssertMsg(pAssignedPlayer != NULL, "pAssignedPlayer should not be NULL. Please send Anton your save file and version.");
	CvAssertMsg(pExistingQuest != NULL, "pExistingQuest should not be NULL. Please send Anton your save file and version.");
	if(pMinor == NULL || pAssignedPlayer == NULL || pExistingQuest == NULL) return;

	Localization::String strMessage;
	Localization::String strSummary;
	int iNotificationX = -1;
	int iNotificationY = -1;


	if(m_eType == MINOR_CIV_QUEST_KILL_CAMP)
	{
		m_iStartTurn = pExistingQuest->GetStartTurn();

		int iCampX = pExistingQuest->GetPrimaryData();
		int iCampY = pExistingQuest->GetSecondaryData();

		CvPlot* pPlot = GC.getMap().plot(iCampX, iCampY);

		if(!pPlot)
		{
			CvAssertMsg(false, "We're starting a quest to kill a barb camp using an existing quest's barb camp data, but the data is bad. Please send Anton your save file and version.");
			return;
		}
		if(pPlot->getImprovementType() != GC.getBARBARIAN_CAMP_IMPROVEMENT())
		{
			CvAssertMsg(false, "We're starting a quest to kill a barb camp using an existing quest's barb camp data, but there's no camp there anymore. Please send Anton your save file and version.");
			return;
		}

		m_iData1 = iCampX;
		m_iData2 = iCampY;

		pPlot->setRevealed(pAssignedPlayer->getTeam(), true);
		pPlot->setRevealedImprovementType(pAssignedPlayer->getTeam(), pPlot->getImprovementType());

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_KILL_CAMP");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_KILL_CAMP");
		iNotificationX = pPlot->getX();
		iNotificationY = pPlot->getY();

		strMessage << pMinor->getNameKey();
		strSummary << pMinor->getNameKey();
		pMinor->GetMinorCivAI()->AddQuestNotification(strMessage.toUTF8(), strSummary.toUTF8(), m_eAssignedPlayer, iNotificationX, iNotificationY);
	}


	else if(pMinor->GetMinorCivAI()->IsGlobalQuest(pExistingQuest->GetType()))
	{
		DoStartQuest(pExistingQuest->GetStartTurn());
	}


	else
	{
		CvAssertMsg(false, "Trying to start a personal quest using existing quest data.  This should not be done.  Please send Anton your save file and version.");
		DoStartQuest(pExistingQuest->GetStartTurn());
	}
}



bool CvMinorCivQuest::DoFinishQuest()
{
	if (!IsComplete())
		return false;

	if (IsHandled())
		return false;

	SetHandled(true);

	CvPlayer* pMinor = &GET_PLAYER(m_eMinor);

	bool bWasFriends = pMinor->GetMinorCivAI()->IsFriends(m_eAssignedPlayer);
	bool bWasAllies = pMinor->GetMinorCivAI()->IsAllies(m_eAssignedPlayer);
	PlayerTypes eOldAlly = pMinor->GetMinorCivAI()->GetAlly();
	int iOldInf = pMinor->GetMinorCivAI()->GetEffectiveFriendshipWithMajor(m_eAssignedPlayer);


	pMinor->GetMinorCivAI()->ChangeFriendshipWithMajor(m_eAssignedPlayer, GetInfluenceReward(),                true);
	
	bool bNowFriends = pMinor->GetMinorCivAI()->IsFriends(m_eAssignedPlayer);
	bool bNowAllies = pMinor->GetMinorCivAI()->IsAllies(m_eAssignedPlayer);
	PlayerTypes eNewAlly = pMinor->GetMinorCivAI()->GetAlly();
	int iNewInf = pMinor->GetMinorCivAI()->GetEffectiveFriendshipWithMajor(m_eAssignedPlayer);
	int iInfChange = iNewInf - iOldInf;

	Localization::String strMessage;
	Localization::String strSummary;
	CivsList veNamesToShow;


	if(m_eType == MINOR_CIV_QUEST_ROUTE)
	{

		pMinor->GetMinorCivAI()->SetRouteConnectionEstablished(m_eAssignedPlayer, true);

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_ROUTE_CONNECTION");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_ROUTE_CONNECTION");
	}


	else if(m_eType == MINOR_CIV_QUEST_KILL_CAMP)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_KILL_CAMP");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_KILL_CAMP");
	}


	else if(m_eType == MINOR_CIV_QUEST_CONNECT_RESOURCE)
	{
		ResourceTypes eResource = (ResourceTypes) GetPrimaryData();
		const char* strResourceName = GC.getResourceInfo(eResource)->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONNECT_RESOURCE");
		strMessage << strResourceName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_CONNECT_RESOURCE");
		strSummary << strResourceName;
	}


	else if(m_eType == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
	{
		BuildingTypes eWonder = (BuildingTypes) GetPrimaryData();
		const char* strBuildingName = GC.getBuildingInfo(eWonder)->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONSTRUCT_WONDER");
		strMessage << strBuildingName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_CONSTRUCT_WONDER");
		strSummary << strBuildingName;
	}


	else if(m_eType == MINOR_CIV_QUEST_GREAT_PERSON)
	{
		UnitTypes eUnit = (UnitTypes) GetPrimaryData();
		const char* strUnitName = GC.getUnitInfo(eUnit)->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_GREAT_PERSON");
		strMessage << strUnitName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_GREAT_PERSON");
		strSummary << strUnitName;
	}


	else if(m_eType == MINOR_CIV_QUEST_KILL_CITY_STATE)
	{
		PlayerTypes eTargetCityState = (PlayerTypes) GetPrimaryData();
		const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_KILL_CITY_STATE");
		strMessage << strTargetNameKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_KILL_CITY_STATE");
		strSummary << strTargetNameKey;
	}


	else if(m_eType == MINOR_CIV_QUEST_FIND_PLAYER)
	{
		PlayerTypes ePlayerFound = (PlayerTypes) GetPrimaryData();
		const char* strCivKey = GET_PLAYER(ePlayerFound).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_FIND_PLAYER");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_FIND_PLAYER");
		strSummary << strCivKey;
	}


	else if(m_eType == MINOR_CIV_QUEST_FIND_NATURAL_WONDER)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_FIND_NATURAL_WONDER");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_FIND_NATURAL_WONDER");
	}


	else if(m_eType == MINOR_CIV_QUEST_GIVE_GOLD)
	{
		PlayerTypes eMostRecentBully = (PlayerTypes) GetPrimaryData();
		const char* strCivKey = "Nobody";
		if(eMostRecentBully != NO_PLAYER)
			strCivKey = GET_PLAYER(eMostRecentBully).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_GIVE_GOLD");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_GIVE_GOLD");
	}


	else if(m_eType == MINOR_CIV_QUEST_PLEDGE_TO_PROTECT)
	{
		PlayerTypes eMostRecentBully = (PlayerTypes) GetPrimaryData();
		const char* strCivKey = "Nobody";
		if(eMostRecentBully != NO_PLAYER)
			strCivKey = GET_PLAYER(eMostRecentBully).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_PLEDGE_TO_PROTECT");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_PLEDGE_TO_PROTECT");
	}


	else if(m_eType == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONTEST_CULTURE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_CONTEST_CULTURE");
		veNamesToShow = GetContestLeaders();
	}


	else if(m_eType == MINOR_CIV_QUEST_CONTEST_FAITH)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONTEST_FAITH");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_CONTEST_FAITH");
		veNamesToShow = GetContestLeaders();
	}


	else if(m_eType == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONTEST_TECHS");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_CONTEST_TECHS");
		veNamesToShow = GetContestLeaders();
	}


	else if(m_eType == MINOR_CIV_QUEST_INVEST)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_INVEST");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_INVEST");
	}


	else if(m_eType == MINOR_CIV_QUEST_BULLY_CITY_STATE)
	{
		PlayerTypes eTargetMinor = (PlayerTypes) GetPrimaryData();
		const char* strCivKey = "Nobody";
		if(eTargetMinor != NO_PLAYER)
			strCivKey = GET_PLAYER(eTargetMinor).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_BULLY_CITY_STATE");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_BULLY_CITY_STATE");
		strSummary << strCivKey;
	}


	else if(m_eType == MINOR_CIV_QUEST_DENOUNCE_MAJOR)
	{
		PlayerTypes eTargetMajor = (PlayerTypes) GetPrimaryData();
		const char* strCivKey = "Nobody";
		if(eTargetMajor != NO_PLAYER)
			strCivKey = GET_PLAYER(eTargetMajor).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_DENOUNCE_MAJOR");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_DENOUNCE_MAJOR");
		strSummary << strCivKey;
	}


	else if(m_eType == MINOR_CIV_QUEST_SPREAD_RELIGION)
	{
		ReligionTypes eReligion = (ReligionTypes) GetPrimaryData();
		CvString strReligionKey = "Atheism";
		if(eReligion != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);
			if(pReligion != NULL)
			{
				strReligionKey = pReligion->GetName();
			}	
		}

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_SPREAD_RELIGION");
		strMessage << strReligionKey.c_str();
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_SPREAD_RELIGION");
		strSummary << strReligionKey.c_str();
	}


	if(m_eType == MINOR_CIV_QUEST_TRADE_ROUTE)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_TRADE_ROUTE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_TRADE_ROUTE");
	}


	if(m_eAssignedPlayer == GC.getGame().getActivePlayer())
	{
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	}

	strMessage << pMinor->getNameKey();
	strMessage << iInfChange;
	strSummary << pMinor->getNameKey();

	CvString sMessage = strMessage.toUTF8();
	CvString sSummary = strSummary.toUTF8();


	if(veNamesToShow.size() > 0)
	{
		sMessage = sMessage + pMinor->GetMinorCivAI()->GetNamesListAsString(veNamesToShow);
	}


	if ((!bWasFriends && bNowFriends) || (!bWasAllies && bNowAllies))
	{
		pair<CvString, CvString> statusChangeStrings = pMinor->GetMinorCivAI()->GetStatusChangeNotificationStrings(m_eAssignedPlayer,         true, bNowFriends, bNowAllies, eOldAlly, eNewAlly);
		sMessage = sMessage + "[NEWLINE][NEWLINE]" + statusChangeStrings.first;
	}

	pMinor->GetMinorCivAI()->AddQuestNotification(sMessage, sSummary, m_eAssignedPlayer);

	return true;
}


bool CvMinorCivQuest::DoCancelQuest()
{
	if (IsHandled())
		return false;

	SetHandled(true);

	CvPlayer* pMinor = &GET_PLAYER(m_eMinor);


	bool bRevoked = IsRevoked();
	bool bExpired = IsExpired();

	Localization::String strMessage;
	Localization::String strSummary;
	CivsList veNamesToShow;


	if(bRevoked)
	{
	}

	else if(bExpired)
	{

		if(m_eType == MINOR_CIV_QUEST_KILL_CAMP)
		{
			strMessage = Localization::Lookup("TXT_KEY_NTFN_QUEST_ENDED_KILL_CAMP");
			strSummary = Localization::Lookup("TXT_KEY_NTFN_QUEST_ENDED_KILL_CAMP_S");
		}

		else if(m_eType == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
		{
			BuildingTypes eWonder = (BuildingTypes) GetPrimaryData();
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eWonder);
			CvAssertMsg(pkBuildingInfo, "Building info not expected to be FALSE! Please send Anton your save file and version.");
			if(!pkBuildingInfo) 
				return false;

			const char* strBuildingName = pkBuildingInfo->GetDescriptionKey();

			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_CONSTRUCT_WONDER");
			strMessage << strBuildingName;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_CONSTRUCT_WONDER");
			strSummary << strBuildingName;
		}


		else if(m_eType == MINOR_CIV_QUEST_KILL_CITY_STATE)
		{
			PlayerTypes eTargetCityState = (PlayerTypes) GetPrimaryData();

			const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_KILL_CITY_STATE");
			strMessage << strTargetNameKey;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_KILL_CITY_STATE");
			strSummary << strTargetNameKey;
		}


		else if(m_eType == MINOR_CIV_QUEST_CONTEST_CULTURE)
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_CULTURE");
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_CONTEST_CULTURE");
			veNamesToShow = GetContestLeaders();
		}


		else if(m_eType == MINOR_CIV_QUEST_CONTEST_FAITH)
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_FAITH");
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_CONTEST_FAITH");
			veNamesToShow = GetContestLeaders();
		}


		else if(m_eType == MINOR_CIV_QUEST_CONTEST_TECHS)
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_TECHS");
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_CONTEST_TECHS");
			veNamesToShow = GetContestLeaders();
		}


		else
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_OTHER");
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_OTHER");
		}

		strMessage << pMinor->getNameKey();
		strSummary << pMinor->getNameKey();

		CvString sMessage = strMessage.toUTF8();
		CvString sSummary = strSummary.toUTF8();


		if(veNamesToShow.size() > 0)
		{
			sMessage = sMessage + pMinor->GetMinorCivAI()->GetNamesListAsString(veNamesToShow);
		}

		pMinor->GetMinorCivAI()->AddQuestNotification(sMessage, sSummary, m_eAssignedPlayer);
	}

	return true;
}


FDataStream& operator>>(FDataStream& loadFrom, CvMinorCivQuest& writeTo)
{
	uint uiVersion;
	loadFrom >> uiVersion;

	loadFrom >> writeTo.m_eType;
	loadFrom >> writeTo.m_iStartTurn;
	loadFrom >> writeTo.m_iData1;
	loadFrom >> writeTo.m_iData2;
	if (uiVersion >= 2)
	{
		loadFrom >> writeTo.m_bHandled;
	}
	else
	{
		writeTo.m_bHandled = false;
	}

	return loadFrom;
}


FDataStream& operator<<(FDataStream& saveTo, const CvMinorCivQuest& readFrom)
{
	uint uiVersion = 2;
	saveTo << uiVersion;

	saveTo << readFrom.m_eType;
	saveTo << readFrom.m_iStartTurn;
	saveTo << readFrom.m_iData1;
	saveTo << readFrom.m_iData2;
	saveTo << readFrom.m_bHandled;

	return saveTo;
}





CvMinorCivAI::CvMinorCivAI()
{
}

CvMinorCivAI::~CvMinorCivAI(void)
{
	Uninit();
}


void CvMinorCivAI::Init(CvPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
	m_minorCivType = CvPreGame::minorCivType(m_pPlayer->GetID());

	Reset();
}


void CvMinorCivAI::Uninit()
{
}


void CvMinorCivAI::Reset()
{
	m_ePersonality = NO_MINOR_CIV_PERSONALITY_TYPE;
	m_eStatus = NO_MINOR_CIV_STATUS_TYPE;
	m_eUniqueUnit = NO_UNIT;

	m_iTurnsSinceThreatenedByBarbarians = -1;
	m_iGlobalQuestCountdown = -1;

	m_eAlly = NO_PLAYER;
	m_iTurnAllied = -1;
	m_eMajorBoughtOutBy = NO_PLAYER;
	m_bDisableNotifications = false;

	int iI, iJ;

	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		m_abWarQuestAgainstMajor[iI] = false;

		for(iJ = 0; iJ < MAX_MAJOR_CIVS; iJ++)
		{
			m_aaiNumEnemyUnitsLeftToKillByMajor[iI][iJ] = -1;
		}

		m_abRouteConnectionEstablished[iI] = false;

#ifdef ENHANCED_GRAPHS
		m_aiBullyGoldAmountTotalByPlayer[iI] = 0;
		m_aiBullyWorkersAmountTotalByPlayer[iI] = 0;
#endif
#if defined(LEKMOD_MERCHANT_BUYOUT_NOT_NOANNEXING)
		m_aiLastAllyTurnWithMajor[iI] = -1;
#endif
		m_aiFriendshipWithMajorTimes100[iI] = 0;
		m_aiAngerFreeIntrusionCounter[iI] = 0;
		m_aiPlayerQuests[iI] = NO_MINOR_CIV_QUEST_TYPE;
		m_aiQuestData1[iI] = -1;
		m_aiQuestData2[iI] = -1;
		m_aiQuestCountdown[iI] = -1;
		m_aiUnitSpawnCounter[iI] = -1;
		m_aiNumUnitsGifted[iI] = 0;
		m_aiNumGoldGifted[iI] = 0;
		m_aiTurnLastBullied[iI] = -1;
		m_aiTurnLastPledged[iI] = -1;
		m_aiTurnLastBrokePledge[iI] = -1;
		m_abUnitSpawningDisabled[iI] = false;
		m_abMajorIntruding[iI] = false;
		m_abEverFriends[iI] = false;
		m_abPledgeToProtect[iI] = false;
		m_aiMajorScratchPad[iI] = 0;

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
		m_abReligionSpreadInfluenceBurstGranted[iI] = false;
#endif

#ifdef CS_ALLYING_WAR_RESCTRICTION
		m_aiMajorPriority[iI] = MAX_MAJOR_CIVS;
#endif
	}

	for(iI = 0; iI < REALLY_MAX_TEAMS; iI++)
	{
		m_abPermanentWar[iI] = false;
		m_abWaryOfTeam[iI] = false;
	}

	ResetQuestList();

	if(GetPlayer()->isMinorCiv())
	{
		CvPlot* pLoopPlot;
		TeamTypes eTeam = GetPlayer()->getTeam();
		int iNumPlotsInEntireWorld = GC.getMap().numPlots();
		for(int iLoopPlot = 0; iLoopPlot < iNumPlotsInEntireWorld; iLoopPlot++)
		{
			pLoopPlot = GC.getMap().plotByIndexUnchecked(iLoopPlot);
			if(pLoopPlot)
				pLoopPlot->setRevealed(eTeam, true);
		}
	}

}

void CvMinorCivAI::ResetQuestList()
{


	for(uint iPlayerEntry = 0; iPlayerEntry < m_QuestsGiven.size(); iPlayerEntry++)
	{
		m_QuestsGiven[iPlayerEntry].clear();
	}

	m_QuestsGiven.clear();

	for(int iNewEntry = 0; iNewEntry < MAX_MAJOR_CIVS; iNewEntry++)
	{
		QuestListForPlayer vQuestList;
		m_QuestsGiven.push_back(vQuestList);
	}
}


void CvMinorCivAI::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_ePersonality;
	kStream >> m_eStatus;
	kStream >> m_eUniqueUnit;

	kStream >> m_iTurnsSinceThreatenedByBarbarians;

	kStream >> m_iGlobalQuestCountdown;

	kStream >> m_eAlly;

	kStream >> m_iTurnAllied;

	kStream >> m_eMajorBoughtOutBy;

	kStream >> m_abWarQuestAgainstMajor;

	kStream >> m_aaiNumEnemyUnitsLeftToKillByMajor;

	kStream >> m_abRouteConnectionEstablished;

#ifdef ENHANCED_GRAPHS
	kStream >> m_aiBullyGoldAmountTotalByPlayer;
	kStream >> m_aiBullyWorkersAmountTotalByPlayer;
#endif
#if defined(LEKMOD_MERCHANT_BUYOUT_NOT_NOANNEXING)
	kStream >> m_aiLastAllyTurnWithMajor;
#endif
	kStream >> m_aiFriendshipWithMajorTimes100;

	kStream >> m_aiAngerFreeIntrusionCounter;

	kStream >> m_aiPlayerQuests;
	kStream >> m_aiQuestData1;
	kStream >> m_aiQuestData2;
	kStream >> m_aiQuestCountdown;
	kStream >> m_aiUnitSpawnCounter;

	kStream >> m_aiNumUnitsGifted;

	kStream >> m_aiNumGoldGifted;

	kStream >> m_aiTurnLastBullied;

	kStream >> m_aiTurnLastPledged;
	kStream >> m_aiTurnLastBrokePledge;

	kStream >> m_abUnitSpawningDisabled;
	kStream >> m_abMajorIntruding;
	kStream >> m_abEverFriends;

	kStream >> m_abPledgeToProtect;

	kStream >> m_abPermanentWar;

	kStream >> m_abWaryOfTeam;


	ResetQuestList();

	int iPlayerEntriesToRead;

	iPlayerEntriesToRead = MAX_MAJOR_CIVS;

	for(int iPlayerLoop = 0; iPlayerLoop < iPlayerEntriesToRead; iPlayerLoop++)
	{
		int iQuestEntriesToRead;
		kStream >> iQuestEntriesToRead;
		CvMinorCivQuest tempQuest;
		for(int iQuestLoop = 0; iQuestLoop < iQuestEntriesToRead; iQuestLoop++)
		{
			kStream >> tempQuest;

			tempQuest.m_eMinor = GetPlayer()->GetID();
			tempQuest.m_eAssignedPlayer = (PlayerTypes) iPlayerLoop;

			m_QuestsGiven[iPlayerLoop].push_back(tempQuest);
		}
	}
	CvAssertMsg(m_QuestsGiven.size() == MAX_MAJOR_CIVS, "Number of entries in minor's quest list does not match MAX_MAJOR_CIVS when read from memory!");

	kStream >> m_bDisableNotifications;
#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	kStream >> m_abReligionSpreadInfluenceBurstGranted;
#endif
#ifdef CS_ALLYING_WAR_RESCTRICTION
	kStream >> m_aiMajorPriority;
#endif
}


void CvMinorCivAI::Write(FDataStream& kStream) const
{

	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << m_ePersonality;
	kStream << m_eStatus;
	kStream << m_eUniqueUnit;

	kStream << m_iTurnsSinceThreatenedByBarbarians;

	kStream << m_iGlobalQuestCountdown;

	kStream << m_eAlly;
	kStream << m_iTurnAllied;

	kStream << m_eMajorBoughtOutBy;

	kStream << m_abWarQuestAgainstMajor;

	kStream << m_aaiNumEnemyUnitsLeftToKillByMajor;

	kStream << m_abRouteConnectionEstablished;

#ifdef ENHANCED_GRAPHS
	kStream << m_aiBullyGoldAmountTotalByPlayer;
	kStream << m_aiBullyWorkersAmountTotalByPlayer;
#endif
#if defined(LEKMOD_MERCHANT_BUYOUT_NOT_NOANNEXING)
	kStream << m_aiLastAllyTurnWithMajor;
#endif
	kStream << m_aiFriendshipWithMajorTimes100;
	kStream << m_aiAngerFreeIntrusionCounter;
	kStream << m_aiPlayerQuests;
	kStream << m_aiQuestData1;
	kStream << m_aiQuestData2;
	kStream << m_aiQuestCountdown;
	kStream << m_aiUnitSpawnCounter;
	kStream << m_aiNumUnitsGifted;
	kStream << m_aiNumGoldGifted;
	kStream << m_aiTurnLastBullied;
	kStream << m_aiTurnLastPledged;
	kStream << m_aiTurnLastBrokePledge;
	kStream << m_abUnitSpawningDisabled;
	kStream << m_abMajorIntruding;
	kStream << m_abEverFriends;
	kStream << m_abPledgeToProtect;
	kStream << m_abPermanentWar;
	kStream << m_abWaryOfTeam;


	CvAssertMsg(m_QuestsGiven.size() == MAX_MAJOR_CIVS, "Number of entries in minor's quest list does not match MAX_MAJOR_CIVS when writing to memory!");
	QuestListForAllPlayers::const_iterator itr_player;
	for(itr_player = m_QuestsGiven.begin(); itr_player != m_QuestsGiven.end(); itr_player++)
	{
		QuestListForPlayer* pvQuestList = (QuestListForPlayer*)itr_player;
		int iQuestEntriesToWrite = pvQuestList->size();
		kStream << iQuestEntriesToWrite;
		QuestListForPlayer::const_iterator itr_quest;
		for(itr_quest = pvQuestList->begin(); itr_quest != pvQuestList->end(); itr_quest++)
		{
			kStream << *itr_quest;
		}
	}

	kStream << m_bDisableNotifications;
#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	kStream << m_abReligionSpreadInfluenceBurstGranted;
#endif
#ifdef CS_ALLYING_WAR_RESCTRICTION
	kStream << m_aiMajorPriority;
#endif
}


void CvMinorCivAI::DoPickInitialItems()
{

	DoPickPersonality();


	DoPickUniqueUnit();
}


CvPlayer* CvMinorCivAI::GetPlayer()
{
	return m_pPlayer;
}


MinorCivTypes CvMinorCivAI::GetMinorCivType() const
{

	return CvPreGame::minorCivType(m_pPlayer->GetID());
}


MinorCivPersonalityTypes CvMinorCivAI::GetPersonality() const
{
	return m_ePersonality;
}


#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
void CvMinorCivAI::DoPickPersonality(bool bEraTransformRepick)
{
	static std::map<int, int> s_personalityAssignmentCounts;

	if(GC.getGame().getElapsedGameTurns() == 0 && GetPlayer()->GetID() == MAX_MAJOR_CIVS)
	{
		s_personalityAssignmentCounts.clear();
	}

	const int iNumPersonalities = GC.getNumMinorCivPersonalityInfos();
	if(iNumPersonalities <= 0)
	{
		m_ePersonality = MINOR_CIV_PERSONALITY_NEUTRAL;
		return;
	}

	int iNumAliveMinors = 0;
	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		if(GET_PLAYER((PlayerTypes)iMinorLoop).isAlive() && GET_PLAYER((PlayerTypes)iMinorLoop).isMinorCiv())
		{
			iNumAliveMinors++;
		}
	}

	const int iPreviousPersonality = (int)m_ePersonality;

	FStaticVector<int, 16, true, c_eCiv5GameplayDLL> veEligiblePersonalities;
	for(int iPersonalityLoop = 0; iPersonalityLoop < iNumPersonalities; iPersonalityLoop++)
	{
		CvMinorCivPersonalityInfo* pkPersonalityInfo = GC.getMinorCivPersonalityInfo(iPersonalityLoop);
		if(bEraTransformRepick)
		{
			if(iPersonalityLoop == iPreviousPersonality)
			{
				continue;
			}

			if(pkPersonalityInfo != NULL && pkPersonalityInfo->GetTransformsAtEra() != NO_ERA)
			{
				continue;
			}
		}

		if(CanAssignPersonality(pkPersonalityInfo, s_personalityAssignmentCounts, iNumAliveMinors))
		{
			veEligiblePersonalities.push_back(iPersonalityLoop);
		}
	}

	if(veEligiblePersonalities.size() == 0)
	{
		m_ePersonality = MINOR_CIV_PERSONALITY_NEUTRAL;
		return;
	}

	const int iRandPick = GC.getGame().getJonRandNum(veEligiblePersonalities.size(), "Minor Civ AI: Picking Personality for this Game (should happen only once per player)");
	m_ePersonality = (MinorCivPersonalityTypes)veEligiblePersonalities[iRandPick];

	s_personalityAssignmentCounts[(int)m_ePersonality]++;

	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	if(pkPersonalityInfo)
	{
		pkPersonalityInfo->ApplyFlavorChanges(m_pPlayer->GetFlavorManager());
	}
}
#else
void CvMinorCivAI::DoPickPersonality()
{
	FlavorTypes eFlavorCityDefense = NO_FLAVOR;
	FlavorTypes eFlavorDefense = NO_FLAVOR;
	FlavorTypes eFlavorOffense = NO_FLAVOR;
	for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_CITY_DEFENSE")
		{
			eFlavorCityDefense = (FlavorTypes)iFlavorLoop;
		}
		if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_DEFENSE")
		{
			eFlavorDefense = (FlavorTypes)iFlavorLoop;
		}
		if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_OFFENSE")
		{
			eFlavorOffense = (FlavorTypes)iFlavorLoop;
		}
	}

	CvFlavorManager* pFlavorManager = m_pPlayer->GetFlavorManager();
	int* pFlavors = pFlavorManager->GetAllPersonalityFlavors();

	MinorCivPersonalityTypes eRandPersonality = (MinorCivPersonalityTypes) GC.getGame().getJonRandNum(NUM_MINOR_CIV_PERSONALITY_TYPES, "Minor Civ AI: Picking Personality for this Game (should happen only once per player)");
	m_ePersonality = eRandPersonality;

	switch(eRandPersonality)
	{
	case MINOR_CIV_PERSONALITY_FRIENDLY:
		pFlavors[eFlavorCityDefense] = pFlavorManager->GetAdjustedValue(pFlavors[eFlavorCityDefense], -2, 0, 10);
		pFlavors[eFlavorDefense] = pFlavorManager->GetAdjustedValue(pFlavors[eFlavorDefense], -2, 0, 10);
		pFlavors[eFlavorOffense] = pFlavorManager->GetAdjustedValue(pFlavors[eFlavorOffense], -2, 0, 10);
		pFlavorManager->ResetToBasePersonality();
		break;
	case MINOR_CIV_PERSONALITY_HOSTILE:
		pFlavors[eFlavorCityDefense] = pFlavorManager->GetAdjustedValue(pFlavors[eFlavorCityDefense], 2, 0, 10);
		pFlavors[eFlavorDefense] = pFlavorManager->GetAdjustedValue(pFlavors[eFlavorDefense], 2, 0, 10);
		pFlavors[eFlavorOffense] = pFlavorManager->GetAdjustedValue(pFlavors[eFlavorOffense], 2, 0, 10);
		pFlavorManager->ResetToBasePersonality();
		break;
	}
}
#endif

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
CvMinorCivPersonalityInfo* CvMinorCivAI::GetPersonalityInfo() const
{
	return GC.getMinorCivPersonalityInfo((int)m_ePersonality);
}

const char* CvMinorCivAI::GetPersonalityTypeString() const
{
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	if(pkPersonalityInfo == NULL)
	{
		return "";
	}

	return pkPersonalityInfo->GetType();
}

bool CvMinorCivAI::IsBlocksWarDeclarationPenalty() const
{
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	return (pkPersonalityInfo != NULL && pkPersonalityInfo->IsBlocksWarDeclarationPenalty());
}

bool CvMinorCivAI::IsNeverAlliedWarSupport() const
{
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	return (pkPersonalityInfo != NULL && pkPersonalityInfo->IsNeverAlliedWarSupport());
}

bool CvMinorCivAI::IsIgnoreBulliedForGoldQuest() const
{
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	return (pkPersonalityInfo != NULL && pkPersonalityInfo->IsIgnoreBulliedForGoldQuest());
}

bool CvMinorCivAI::IsMajorBlockedByAlliedWar(PlayerTypes eMajor) const
{
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS)
	{
		return false;
	}

	if(!IsNeverAlliedWarSupport())
	{
		return false;
	}

	if(m_pPlayer == NULL)
	{
		return false;
	}

	if(GET_TEAM(GET_PLAYER(eMajor).getTeam()).isAtWar(m_pPlayer->getTeam()))
	{
		return false;
	}

	const PlayerTypes eAlly = GetAlly();
	if(eAlly == NO_PLAYER || eAlly == eMajor)
	{
		return false;
	}

	if(!GET_PLAYER(eAlly).isAlive())
	{
		return false;
	}

	return GET_TEAM(GET_PLAYER(eMajor).getTeam()).isAtWar(GET_PLAYER(eAlly).getTeam());
}

bool CvMinorCivAI::IsCoastalMinor() const
{
	CvCity* pCapital = m_pPlayer->getCapitalCity();
	return (pCapital != NULL && pCapital->isCoastal(GC.getLAKE_MAX_AREA_SIZE()));
}

bool CvMinorCivAI::CanAssignPersonality(const CvMinorCivPersonalityInfo* pkPersonalityInfo, const std::map<int, int>& personalityAssignmentCounts, int iNumAliveMinors) const
{
	if(pkPersonalityInfo == NULL || pkPersonalityInfo->IsDisabled())
	{
		return false;
	}

	if(pkPersonalityInfo->GetRequiredMinorCivTrait() != NO_MINOR_CIV_TRAIT_TYPE && GetTrait() != (MinorCivTraitTypes)pkPersonalityInfo->GetRequiredMinorCivTrait())
	{
		return false;
	}

	if(pkPersonalityInfo->GetForbiddenMinorCivTrait() != NO_MINOR_CIV_TRAIT_TYPE && GetTrait() == (MinorCivTraitTypes)pkPersonalityInfo->GetForbiddenMinorCivTrait())
	{
		return false;
	}

	if(pkPersonalityInfo->RequiresCoastal() && !IsCoastalMinor())
	{
		return false;
	}

	if(pkPersonalityInfo->GetMaxGlobalCount() > 0)
	{
		int iMaxAllowed = pkPersonalityInfo->GetMaxGlobalCount();
		if(pkPersonalityInfo->GetMaxGlobalCountDivisor() > 0 && iNumAliveMinors > 0)
		{
			iMaxAllowed = (iNumAliveMinors * pkPersonalityInfo->GetMaxGlobalCount() + pkPersonalityInfo->GetMaxGlobalCountDivisor() - 1) / pkPersonalityInfo->GetMaxGlobalCountDivisor();
		}

		std::map<int, int>::const_iterator it = personalityAssignmentCounts.find(pkPersonalityInfo->GetID());
		if(it != personalityAssignmentCounts.end() && it->second >= iMaxAllowed)
		{
			return false;
		}
	}

	return true;
}

bool CvMinorCivAI::IsQuestBlockedByPersonality(MinorCivQuestTypes eQuest) const
{
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	if(pkPersonalityInfo == NULL || !pkPersonalityInfo->IsBlocksQuests())
	{
		return false;
	}

	if(eQuest == MINOR_CIV_QUEST_KILL_CAMP && GetTurnsSinceThreatenedAnnouncement() >= 0)
	{
		return false;
	}

	return true;
}

int CvMinorCivAI::GetPersonalityQuestInfluenceModifierPercent(PlayerTypes ePlayer) const
{
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	if(pkPersonalityInfo == NULL)
	{
		return 100;
	}

	if(IsMajorExcludedFromPersonalityBonuses(ePlayer))
	{
		return 100;
	}

	const int iModifier = pkPersonalityInfo->GetQuestInfluenceModifierPercent();
	return (iModifier > 0) ? iModifier : 100;
}

bool CvMinorCivAI::IsMajorExcludedFromPersonalityBonuses(PlayerTypes ePlayer) const
{
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	if(pkPersonalityInfo == NULL || !pkPersonalityInfo->IsStripPersonalityBonusesIfAttackedMinor())
	{
		return false;
	}

	return GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetNumMinorCivsAttacked() > 0;
}

bool CvMinorCivAI::DoesMajorHaveOceanCoastalCity(PlayerTypes eMajor) const
{
	int iLoop;
	CvCity* pLoopCity;
	for(pLoopCity = GET_PLAYER(eMajor).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eMajor).nextCity(&iLoop))
	{
		if(pLoopCity->isCoastal(GC.getLAKE_MAX_AREA_SIZE()))
		{
			return true;
		}
	}

	return false;
}

void CvMinorCivAI::DoPersonalityReligionSpreadInfluence(PlayerTypes eMajor, bool bHadReligionBefore)
{
	if(bHadReligionBefore || eMajor < 0 || eMajor >= MAX_MAJOR_CIVS)
	{
		return;
	}

	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	if(pkPersonalityInfo == NULL || pkPersonalityInfo->GetReligionSpreadInfluenceBurst() <= 0)
	{
		return;
	}

	if(m_abReligionSpreadInfluenceBurstGranted[eMajor])
	{
		return;
	}

	m_abReligionSpreadInfluenceBurstGranted[eMajor] = true;
	ChangeFriendshipWithMajor(eMajor, pkPersonalityInfo->GetReligionSpreadInfluenceBurst());
}

void CvMinorCivAI::DoTestPersonalityEraTransform()
{
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	if(pkPersonalityInfo == NULL || pkPersonalityInfo->GetTransformsAtEra() == NO_ERA)
	{
		return;
	}

	const EraTypes eTransformEra = (EraTypes)pkPersonalityInfo->GetTransformsAtEra();
	bool bEraReached = false;
	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		const PlayerTypes eMajor = (PlayerTypes)iMajorLoop;
		if(GET_PLAYER(eMajor).isAlive() && GET_PLAYER(eMajor).GetCurrentEra() >= eTransformEra)
		{
			bEraReached = true;
			break;
		}
	}

	if(bEraReached)
	{
		DoPickPersonality(                        true);
	}
}

namespace
{
UnitTypes GetCompetitiveSpawnNavalUnitType(PlayerTypes ePlayer, bool bIncludeUUs, bool bIncludeRanged)
{
	CvWeightedVector<UnitTypes, SAFE_ESTIMATE_NUM_UNITS, true> veUnitRankings;

#ifdef AUI_WARNING_FIXES
	for(uint iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
#else
	for(int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
#endif
	{
		const UnitTypes eLoopUnit = (UnitTypes)iUnitLoop;
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eLoopUnit);
		if(pkUnitInfo == NULL)
		{
			continue;
		}

		if(pkUnitInfo->GetDomainType() != DOMAIN_SEA)
		{
			continue;
		}

		bool bValid = (pkUnitInfo->GetCombat() > 0 || pkUnitInfo->GetRangedCombat() > 0);
		if(!bValid)
		{
			continue;
		}

		if(!bIncludeRanged && pkUnitInfo->GetRangedCombat() > 0)
		{
			continue;
		}

		if(!GET_PLAYER(ePlayer).canTrain(eLoopUnit, false, false, false, true))
		{
			continue;
		}

		UnitClassTypes eLoopUnitClass = (UnitClassTypes)pkUnitInfo->GetUnitClassType();
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eLoopUnitClass);
		if(pkUnitClassInfo == NULL)
		{
			continue;
		}

		if(eLoopUnit != pkUnitClassInfo->getDefaultUnitIndex())
		{
			if(!bIncludeUUs)
			{
				continue;
			}
		}

		veUnitRankings.push_back(eLoopUnit, pkUnitInfo->GetPower());
	}

	if(veUnitRankings.size() == 0)
	{
		return NO_UNIT;
	}

	veUnitRankings.SortItems();
	RandomNumberDelegate randFn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
	return veUnitRankings.ChooseFromTopChoices(GC.getUNIT_SPAWN_NUM_CHOICES(), &randFn, "Choosing competitive naval unit from top choices");
}
}
#endif


MinorCivTraitTypes CvMinorCivAI::GetTrait() const
{
	CvMinorCivInfo* pkMinorCivInfo = GC.getMinorCivInfo(GetMinorCivType());
	if(pkMinorCivInfo)
	{
		return (MinorCivTraitTypes) pkMinorCivInfo->GetMinorCivTrait();
	}

	return NO_MINOR_CIV_TRAIT_TYPE;
}


bool CvMinorCivAI::IsHasUniqueUnit() const
{
	return (m_eUniqueUnit != NO_UNIT);
}


UnitTypes CvMinorCivAI::GetUniqueUnit() const
{
	return m_eUniqueUnit;
}


void CvMinorCivAI::SetUniqueUnit(UnitTypes eUnit)
{
	CvAssertMsg(GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC, "Setting a unique unit for a non-Militaristic City-State.  It will never be spawned without additional scripting.");
	CvUnitEntry* pInfo = GC.getUnitInfo(eUnit);
	CvAssertMsg(pInfo, "Setting a unique unit for a City-State that the game core could not identify. Please send Anton your save file and version.");
	if (pInfo)
	{
		m_eUniqueUnit = eUnit;
	}
}


void CvMinorCivAI::DoPickUniqueUnit()
{
	m_eUniqueUnit = NO_UNIT;
	if (GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
	{

		m_eUniqueUnit = GC.getGame().GetRandomUniqueUnitType(                      false,                          false,                    false,                   true);
	}
}








void CvMinorCivAI::DoTurn()
{
	AI_PERF_FORMAT("AI-perf.csv", ("CvMinorCivAI::DoTurn, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), GetPlayer()->getCivilizationShortDescription()) );
	if(GetPlayer()->isMinorCiv())
	{
		DoTurnStatus();

		DoElection();
		DoFriendship();
#if defined(LEKMOD_MERCHANT_BUYOUT_NOT_NOANNEXING)
		PlayerTypes eAlly = GetAlly();
		if (eAlly != NO_PLAYER)
			SetLastAllyTurnWithMajor(eAlly, GC.getGame().getGameTurn());
#endif
		DoTestThreatenedAnnouncement();
		DoTestProxyWarAnnouncement();

		DoTurnQuests();

		DoUnitSpawnTurn();

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
		DoTestPersonalityEraTransform();
#endif

		DoIntrusion();
	}
}



void CvMinorCivAI::DoChangeAliveStatus(bool bAlive)
{
	if (!bAlive)
	{

		DoTestActiveQuests(                  true,                   true);

		std::vector<int> vNewInfluence;
		for (int i = 0; i < MAX_MAJOR_CIVS; ++i)
		{
			PlayerTypes e = (PlayerTypes)i;


			DoChangeProtectionFromMajor(e, false);
			EndAllActiveQuestsForPlayer(e);


			int iOldInfluence = GetBaseFriendshipWithMajor(e);
			int iNewInfluence = iOldInfluence;
			if (IsFriendshipAboveAlliesThreshold(iOldInfluence))
			{
				iNewInfluence = GC.getFRIENDSHIP_ALLIES_ON_DEATH();
			}
			else if (IsFriendshipAboveFriendsThreshold(iOldInfluence))
			{
				iNewInfluence = GC.getFRIENDSHIP_FRIENDS_ON_DEATH();
			}
			else if (iOldInfluence > GC.getFRIENDSHIP_THRESHOLD_NEUTRAL())
			{
				iNewInfluence = GC.getFRIENDSHIP_NEUTRAL_ON_DEATH();
			}
			vNewInfluence.push_back(iNewInfluence);
		}


		SetDisableNotifications(true);
		for (unsigned int i = 0; i < vNewInfluence.size(); ++i)
		{
			PlayerTypes e = (PlayerTypes)i;

			DoFriendshipChangeEffects(e, GetEffectiveFriendshipWithMajor(e), vNewInfluence.at(i),               false,                      true);
			SetFriendshipWithMajor(e, vNewInfluence.at(i));
		}
		SetDisableNotifications(false);
	}


	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iPlayerLoop;

		bool bFriends = false;
		bool bAllies = false;
		if(IsFriendshipAboveFriendsThreshold(GetEffectiveFriendshipWithMajor(ePlayer)))
		{
			bFriends = true;
		}
		if(GetAlly() == ePlayer)
		{
			CvAssertMsg(bAlive, "A Minor about to die still has an Ally, when it should have none.");
			bAllies = true;
		}
		if(bFriends || bAllies)
		{
			DoSetBonus(ePlayer, bAlive, bFriends, bAllies);
		}
	}


	if (!bAlive)
	{
		bool bHasAlly = GetAlly() != NO_PLAYER;
		CvAssertMsg(!bHasAlly, "A Minor about to die still has an Ally, when it should have none.");
		if(bHasAlly)
		{
			SetAlly(NO_PLAYER);
		}

		SetTurnsSinceThreatenedAnnouncement(-1);
	}
}



void CvMinorCivAI::DoFirstContactWithMajor(TeamTypes eTeam, bool bSuppressMessages)
{

	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iPlayerLoop;
		if(GET_PLAYER(ePlayer).getTeam() == eTeam)
		{
			if(GET_PLAYER(ePlayer).isHuman())
			{

				SetMajorIntruding(ePlayer, true);
			}
			else
			{
				SetMajorIntruding(ePlayer, false);
			}
		}
	}


	if(IsPeaceBlocked(eTeam))
	{
#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
		if(!IsNeverAlliedWarSupport())
#endif
		{
			GET_TEAM(GetPlayer()->getTeam()).declareWar(eTeam);
		}
	}

	else
	{
		int iGoldGift = 0;
		int iFaithGift = 0;
		bool bFirstMajorCiv = false;


		if(!GET_TEAM(eTeam).IsMinorCivAggressor())
		{

			if(GET_TEAM(GetPlayer()->getTeam()).getHasMetCivCount(true) == 0)
			{
				iGoldGift =        GC.getMINOR_CIV_CONTACT_GOLD_FIRST();
				if(GetTrait() == MINOR_CIV_TRAIT_RELIGIOUS)
					iFaithGift = 0;
				bFirstMajorCiv = true;
			}
			else
			{
				iGoldGift =        GC.getMINOR_CIV_CONTACT_GOLD_OTHER();
				if(GetTrait() == MINOR_CIV_TRAIT_RELIGIOUS)
					iFaithGift = 0;
			}

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
			CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
			if(pkPersonalityInfo && bFirstMajorCiv)
			{
				iGoldGift += pkPersonalityInfo->GetFirstMeetGoldModifier();
				if(iGoldGift < 0)
				{
					iGoldGift = 0;
				}
			}
#endif
		}

		PlayerTypes ePlayer;
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			ePlayer = (PlayerTypes) iPlayerLoop;

			if(GET_PLAYER(ePlayer).getTeam() == eTeam)
			{

				GET_PLAYER(ePlayer).GetTreasury()->ChangeGold(iGoldGift);


				if(iFaithGift > 0)
					GET_PLAYER(ePlayer).ChangeFaith(iFaithGift);


				if(GC.getGame().getElapsedGameTurns() > GetFirstPossibleTurnForPersonalQuests())
				{
					DoTestSeedQuestCountdownForPlayer(ePlayer);
				}


				if(GetTurnsSinceThreatenedAnnouncement() >= 0 && GetTurnsSinceThreatenedAnnouncement() < 10)
				{
					DoTestThreatenedAnnouncementForPlayer(ePlayer);
				}


				DoTestProxyWarAnnouncementOnFirstContact(ePlayer);


				DoTestQuestsOnFirstContact(ePlayer);


				if(ePlayer == GC.getGame().getActivePlayer() && !bSuppressMessages)
				{
					if(!GC.getGame().isNetworkMultiPlayer())
					{
						CvPopupInfo kPopupInfo(BUTTONPOPUP_CITY_STATE_GREETING, GetPlayer()->GetID(), iGoldGift, iFaithGift, 0, bFirstMajorCiv);
						GC.GetEngineUserInterface()->AddPopup(kPopupInfo);

						CancelActivePlayerEndTurn();
					}


					int iLoop = 0;
					CvCity* pLoopCity = NULL;
					for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
					{
						if(pLoopCity->plot()->isRevealed(eTeam))
						{
							auto_ptr<ICvCity1> pDllLoopCity = GC.WrapCityPointer(pLoopCity);
							GC.GetEngineUserInterface()->SetSpecificCityInfoDirty(pDllLoopCity.get(), CITY_UPDATE_TYPE_BANNER);
						}
					}
				}
			}
		}
	}
}


void CvMinorCivAI::DoTestEndWarsVSMinors(PlayerTypes eOldAlly, PlayerTypes eNewAlly)
{
	if(eOldAlly == NO_PLAYER)
		return;

	if(!GetPlayer()->isAlive())
		return;

	PlayerTypes eOtherMinor;
	int iOtherMinorLoop;
	PlayerTypes eOtherAlly;
	bool bForcedWar;

	TeamTypes eLoopTeam;
	for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
	{
		eLoopTeam = (TeamTypes) iTeamLoop;


		if(!GET_TEAM(eLoopTeam).isMinorCiv())
			continue;


		if(!GET_TEAM(eLoopTeam).isAlive())
			continue;


		if(!GET_TEAM(GetPlayer()->getTeam()).isAtWar(eLoopTeam))
			continue;

		if(eOldAlly != NO_PLAYER)
		{

			if(!GET_TEAM(GET_PLAYER(eOldAlly).getTeam()).isAtWar(eLoopTeam))
				continue;
		}

		if(eNewAlly != NO_PLAYER)
		{

			if(GET_TEAM(GET_PLAYER(eNewAlly).getTeam()).isAtWar(eLoopTeam))
				continue;
		}


		bForcedWar = false;

		for(iOtherMinorLoop = 0; iOtherMinorLoop < MAX_CIV_TEAMS; iOtherMinorLoop++)
		{
			eOtherMinor = (PlayerTypes) iOtherMinorLoop;


			if(GET_PLAYER(eOtherMinor).getTeam() == eLoopTeam)
			{
				eOtherAlly = GET_PLAYER(eOtherMinor).GetMinorCivAI()->GetAlly();
				if(eOtherAlly != NO_PLAYER)
				{

					if(GET_TEAM(GET_PLAYER(eOtherAlly).getTeam()).isAtWar(GetPlayer()->getTeam()))
					{
						bForcedWar = true;
						break;
					}
				}
			}

		}

		if(bForcedWar)
			continue;

		if(IsPermanentWar(eLoopTeam))
			continue;

		GET_TEAM(GetPlayer()->getTeam()).makePeace(eLoopTeam);
	}
}


void CvMinorCivAI::DoTurnStatus()
{
	int iWeight = 0;

	PlayerProximityTypes eProximity;

	CvPlayer* pPlayer;
	CvTeam* pTeam;
	PlayerTypes ePlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;
		pPlayer = &GET_PLAYER(ePlayer);
		pTeam = &GET_TEAM(pPlayer->getTeam());

		eProximity = pPlayer->GetProximityToPlayer(GetPlayer()->GetID());


		switch(eProximity)
		{

		case PLAYER_PROXIMITY_DISTANT:
			break;






		case PLAYER_PROXIMITY_FAR:
			break;










		case PLAYER_PROXIMITY_CLOSE:
			if(pTeam->IsMinorCivAggressor())
			{
				iWeight += 10;
			}
			if(pTeam->IsMinorCivWarmonger())
			{
				iWeight += 20;
			}
			break;


		case PLAYER_PROXIMITY_NEIGHBORS:
			if(pTeam->IsMinorCivAggressor())
			{
				iWeight += 20;
			}
			if(IsAtWarWithPlayersTeam(ePlayer))
			{
				iWeight += 20;
			}
			break;
		default:
			break;
		}
	}


	if(iWeight >= 20)
	{
		m_eStatus = MINOR_CIV_STATUS_CRITICAL;
	}
	else if(iWeight >= 10)
	{
		m_eStatus = MINOR_CIV_STATUS_ELEVATED;
	}
	else
	{
		m_eStatus = MINOR_CIV_STATUS_NORMAL;
	}
}


MinorCivStatusTypes CvMinorCivAI::GetStatus() const
{
	return m_eStatus;
}


void CvMinorCivAI::DoAddStartingResources(CvPlot* pCityPlot)
{
	CvAssertMsg(pCityPlot != NULL, "City's plot should not be NULL. Please send Anton your save file and version.");
	if (pCityPlot == NULL) return;

	MinorCivTraitTypes eTrait = GetTrait();


	if (eTrait == MINOR_CIV_TRAIT_MERCANTILE)
	{
		bool bAddUniqueLuxury = true;
		

		if (GetPlayer()->getNumCities() > 1)
		{
			bAddUniqueLuxury = false;
		}
	

		ResourceTypes ePlotResource = pCityPlot->getResourceType();
		if (ePlotResource != NO_RESOURCE)
		{
			CvResourceInfo* pkPlotResourceInfo = GC.getResourceInfo(ePlotResource);
			if (pkPlotResourceInfo && pkPlotResourceInfo->isOnlyMinorCivs())
			{
				bAddUniqueLuxury = false;
			}
		}


		if (bAddUniqueLuxury)
		{
			FStaticVector< ResourceTypes, 64, true, c_eCiv5GameplayDLL > veUniqueLuxuries;
#ifdef AUI_WARNING_FIXES
			for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
			for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
			{
				const ResourceTypes eResourceLoop = (ResourceTypes) iResourceLoop;
				CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResourceLoop);
				if(pkResourceInfo)
				{
					if(pkResourceInfo->isOnlyMinorCivs())
					{
						veUniqueLuxuries.push_back(eResourceLoop);
					}
				}
			}

			pCityPlot->setResourceType(NO_RESOURCE, 0, true);
			if (veUniqueLuxuries.size() > 0)
			{
				int iRoll = GC.getGame().getJonRandNum(veUniqueLuxuries.size(), "Rolling for Mercantile city-state special luxury");
				int iQuantity = GC.getMINOR_CIV_MERCANTILE_RESOURCES_QUANTITY();
				ResourceTypes eSpecialLuxury = veUniqueLuxuries[iRoll];

				pCityPlot->setResourceType(eSpecialLuxury, iQuantity, true);
			}
		}
	}
}


void CvMinorCivAI::DoRemoveStartingResources(CvPlot* pCityPlot)
{
	CvAssertMsg(pCityPlot != NULL, "City's plot should not be NULL. Please send Anton your save file and version.");
	if (pCityPlot == NULL) return;

	MinorCivTraitTypes eTrait = GetTrait();


	if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
	{
		bool bRemoveUniqueLuxury = false;

		if (GC.getMINOR_CIV_MERCANTILE_RESOURCES_KEEP_ON_CAPTURE_DISABLED() == 1)
			bRemoveUniqueLuxury = true;

		if (bRemoveUniqueLuxury)
		{
			ResourceTypes eOldResource = pCityPlot->getResourceType();
			if (eOldResource != NO_RESOURCE)
			{
				CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eOldResource);
				if (pkResourceInfo && pkResourceInfo->isOnlyMinorCivs())
				{
					pCityPlot->setResourceType(NO_RESOURCE, 0, true);
				}
			}
		}
	}
}


void CvMinorCivAI::AddNotification(CvString sString, CvString sSummaryString, PlayerTypes ePlayer, int iX, int iY)
{
	if(iX == -1 && iY == -1)
	{
		CvCity* capCity = GetPlayer()->getCapitalCity();

		if(capCity != NULL)
		{
			iX = capCity->getX();
			iY = capCity->getY();
		}
	}

	CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
	if(pNotifications)
	{
		pNotifications->Add(NOTIFICATION_MINOR, sString, sSummaryString, iX, iY, GetPlayer()->GetID());
	}
}


void CvMinorCivAI::AddQuestNotification(CvString sString, CvString sSummaryString, PlayerTypes ePlayer, int iX, int iY, bool bNewQuest)
{
	CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
	if(pNotifications)
	{
		sString += "[NEWLINE][NEWLINE]";
		sString += Localization::Lookup("TXT_KEY_MINOR_QUEST_BLOCKING_TT").toUTF8();

		if (bNewQuest)
			pNotifications->Add(NOTIFICATION_MINOR_QUEST, sString, sSummaryString, iX, iY, GetPlayer()->GetID(), 1);
		else
			pNotifications->Add(NOTIFICATION_MINOR_QUEST, sString, sSummaryString, iX, iY, GetPlayer()->GetID());
	}
}


void CvMinorCivAI::AddBuyoutNotification(CvString sString, CvString sSummaryString, PlayerTypes ePlayer, int iX, int iY)
{
	if(iX == -1 && iY == -1)
	{
		CvCity* capCity = GetPlayer()->getCapitalCity();

		if(capCity != NULL)
		{
			iX = capCity->getX();
			iY = capCity->getY();
		}
	}

	CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
	if(pNotifications)
	{
		pNotifications->Add(NOTIFICATION_MINOR_BUYOUT, sString, sSummaryString, iX, iY, GetPlayer()->GetID());
	}
}







bool CvMinorCivAI::IsThreateningBarbariansEventActiveForPlayer(PlayerTypes ePlayer)
{
	if (GetPlayer()->isAlive())
	{
		if (IsHasMetPlayer(ePlayer) && !IsAtWarWithPlayersTeam(ePlayer))
		{
			if (GetNumThreateningBarbarians() > 0)
			{
				return true;
			}
		}
	}
	
	return false;
}


int CvMinorCivAI::GetNumThreateningBarbarians()
{
	int iCount = 0;

	int iLoop;
	for(CvUnit* pLoopUnit = GET_PLAYER(BARBARIAN_PLAYER).firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = GET_PLAYER(BARBARIAN_PLAYER).nextUnit(&iLoop))
	{
		if(pLoopUnit->IsBarbarianUnitThreateningMinor(GetPlayer()->GetID()))
			iCount++;
	}

	return iCount;
}


void CvMinorCivAI::DoTestThreatenedAnnouncement()
{

	if(GetTurnsSinceThreatenedAnnouncement() >= 0)
	{
		ChangeTurnsSinceThreatenedAnnouncement(1);


		if(GetTurnsSinceThreatenedAnnouncement() >= 30)
			SetTurnsSinceThreatenedAnnouncement(-1);
	}


	if(GetTurnsSinceThreatenedAnnouncement() == -1)
	{
		if(GetNumThreateningBarbarians() >= GC.getMINOR_CIV_THREATENED_BY_BARBS_NOTIF_THRESHOLD())
		{

			SetTurnsSinceThreatenedAnnouncement(0);

			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				DoTestThreatenedAnnouncementForPlayer((PlayerTypes) iPlayerLoop);
			}
		}
	}
}


int CvMinorCivAI::GetTurnsSinceThreatenedAnnouncement() const
{
	return m_iTurnsSinceThreatenedByBarbarians;
}


void CvMinorCivAI::SetTurnsSinceThreatenedAnnouncement(int iValue)
{
	if(GetTurnsSinceThreatenedAnnouncement() != iValue)
		m_iTurnsSinceThreatenedByBarbarians = iValue;
}


void CvMinorCivAI::ChangeTurnsSinceThreatenedAnnouncement(int iChange)
{
	SetTurnsSinceThreatenedAnnouncement(GetTurnsSinceThreatenedAnnouncement() + iChange);
}


void CvMinorCivAI::DoTestThreatenedAnnouncementForPlayer(PlayerTypes ePlayer)
{
	if (GetPlayer()->isAlive())
	{
		CvCity* pCapital = GetPlayer()->getCapitalCity();
		if (pCapital)
		{
			int iX = pCapital->getX();
			int iY = pCapital->getY();
			if (IsThreateningBarbariansEventActiveForPlayer(ePlayer))
			{

				if (IsPlayerCloseEnoughForThreatenedAnnouncement(ePlayer))
				{

					if (GetNumThreateningBarbarians() >= GC.getMINOR_CIV_THREATENED_BY_BARBS_NOTIF_THRESHOLD())
					{
						Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BARBS_QUEST");
						strMessage << GetPlayer()->getNameKey();
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_BARBS_QUEST");
						strSummary << GetPlayer()->getNameKey();
						AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), ePlayer, iX, iY);
					}
				}
			}
		}
	}
}


bool CvMinorCivAI::IsPlayerCloseEnoughForThreatenedAnnouncement(PlayerTypes eMajor)
{
	CvCity* pCapital = GetPlayer()->getCapitalCity();


	if(pCapital == NULL)
	{
		return false;
	}


	if(IsHasMetPlayer(eMajor))
	{
		bool bCloseEnoughForQuest = false;

		CvCity* pMajorsCapital = GET_PLAYER(eMajor).getCapitalCity();

		if(pMajorsCapital != NULL)
		{
			if(pCapital->getArea() == pMajorsCapital->getArea())
			{
				return true;
			}

			if(!bCloseEnoughForQuest)
			{
				int iDistance = plotDistance(pCapital->getX(), pCapital->getY(), pMajorsCapital->getX(), pMajorsCapital->getY());

				if(iDistance <=        GC.getMAX_DISTANCE_MINORS_BARB_QUEST())
				{
					return true;
				}
			}
		}
	}

	return false;
}



void CvMinorCivAI::DoThreateningBarbKilled(PlayerTypes eKillingPlayer, int iX, int iY)
{
	CvAssertMsg(eKillingPlayer >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eKillingPlayer < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");

	if (IsThreateningBarbariansEventActiveForPlayer(eKillingPlayer))
	{
		ChangeFriendshipWithMajor(eKillingPlayer,        GC.getFRIENDSHIP_PER_BARB_KILLED());

		ChangeAngerFreeIntrusionCounter(eKillingPlayer, 5);

		Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BARB_KILLED");
		strMessage << GetPlayer()->getNameKey();
		Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SM_MINOR_BARB_KILLED");
		strSummary << GetPlayer()->getNameKey();

		AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), eKillingPlayer, iX, iY);
	}
}







void CvMinorCivAI::DoTestProxyWarAnnouncement()
{
	for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
		PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
		CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
		CvTeam* pNotifyTeam = &GET_TEAM(kCurNotifyPlayer.getTeam());
		if (!pNotifyTeam->isHasMet(GetPlayer()->getTeam()))
		{
			continue;
		}

		for (int i = 0; i < MAX_MAJOR_CIVS; i++)
		{
			PlayerTypes eOtherMajor = (PlayerTypes) i;
			if (IsProxyWarActiveForMajor(eNotifyPlayer, eOtherMajor))
			{
				CvPlayer* pOtherMajor = &GET_PLAYER(eOtherMajor);
				CvAssertMsg(pOtherMajor, "Error sending out proxy war notification from a city-state. Please send Anton your save file and version.");
				if (pOtherMajor)
				{
					TeamTypes eEnemyTeam = pOtherMajor->getTeam();
					PlayerTypes eEnemyTeamLeader = GET_TEAM(eEnemyTeam).getLeaderID();
					CvPlayer* pEnemyTeamLeader = &GET_PLAYER(eEnemyTeamLeader);
					CvAssert(pEnemyTeamLeader);
					if (pEnemyTeamLeader && GET_TEAM(GetPlayer()->getTeam()).GetNumTurnsAtWar(eEnemyTeam) == 1)
					{
						Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_WAR_UNIT_HELP");
						strMessage << GetPlayer()->getCivilizationShortDescriptionKey() << pEnemyTeamLeader->getCivilizationShortDescriptionKey();
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_WAR_UNIT_HELP");
						strSummary << GetPlayer()->getCivilizationShortDescriptionKey();

						AddQuestNotification(strMessage.toUTF8(), strSummary.toUTF8(), eNotifyPlayer);

						break;
					}
				}
			}
		}
	}
}

void CvMinorCivAI::DoTestProxyWarAnnouncementOnFirstContact(PlayerTypes eMajor)
{
	for (int i = 0; i < MAX_MAJOR_CIVS; i++)
	{
		PlayerTypes eOtherMajor = (PlayerTypes) i;
		if (IsProxyWarActiveForMajor(eMajor, eOtherMajor))
		{
			CvPlayer* pOtherMajor = &GET_PLAYER(eOtherMajor);
			CvAssertMsg(pOtherMajor, "Error sending out proxy war notification from a city-state. Please send Anton your save file and version.");
			if (pOtherMajor)
			{
				TeamTypes eEnemyTeam = pOtherMajor->getTeam();
				PlayerTypes eEnemyTeamLeader = GET_TEAM(eEnemyTeam).getLeaderID();
				CvPlayer* pEnemyTeamLeader = &GET_PLAYER(eEnemyTeamLeader);
				CvAssert(pEnemyTeamLeader);
				if (pEnemyTeamLeader)
				{
					Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_WAR_UNIT_HELP");
					strMessage << GetPlayer()->getCivilizationShortDescriptionKey() << pEnemyTeamLeader->getCivilizationShortDescriptionKey();
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_WAR_UNIT_HELP");
					strSummary << GetPlayer()->getCivilizationShortDescriptionKey();

					AddQuestNotification(strMessage.toUTF8(), strSummary.toUTF8(), eMajor);

					break;
				}
			}
		}
	}
}

bool CvMinorCivAI::IsProxyWarActiveForMajor(PlayerTypes eMajor, PlayerTypes eOtherMajor)
{
	CvAssert(eMajor >= 0 && eMajor < MAX_MAJOR_CIVS);
	CvAssert(eOtherMajor >= 0 && eOtherMajor < MAX_MAJOR_CIVS);
	if (eMajor != eOtherMajor && eMajor >= 0 && eMajor < MAX_MAJOR_CIVS && eOtherMajor >= 0 && eOtherMajor < MAX_MAJOR_CIVS)
	{
		TeamTypes eOtherTeam = GET_PLAYER(eOtherMajor).getTeam();
		CvAssert(eOtherTeam != NO_TEAM);
		if (eOtherTeam != NO_TEAM)
		{

			if (GET_PLAYER(eMajor).isAlive() && !IsAtWarWithPlayersTeam(eMajor) && !GET_TEAM(eOtherTeam).isAtWar(GET_PLAYER(eMajor).getTeam()))
			{

				if (GET_PLAYER(eOtherMajor).isAlive() && IsAtWarWithPlayersTeam(eOtherMajor) && !IsPeaceBlocked(eOtherTeam))
				{

					PlayerTypes eOtherTeamLeader = GET_TEAM(eOtherTeam).getLeaderID();
					CvPlayer* pOtherTeamLeader = &GET_PLAYER(eOtherTeamLeader);
					if (pOtherTeamLeader && !pOtherTeamLeader->isMinorCiv() && pOtherTeamLeader->isAlive())
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool CvMinorCivAI::IsProxyWarActiveForMajor(PlayerTypes eMajor)
{
	CvAssert(eMajor >= 0 && eMajor < MAX_MAJOR_CIVS);
	for (int i = 0; i < MAX_MAJOR_CIVS; i++)
	{
		if (IsProxyWarActiveForMajor(eMajor, (PlayerTypes)i))
		{
			return true;
		}
	}
	return false;
}







void CvMinorCivAI::DoTurnQuests()
{



	DoTestActiveQuests(                  true,                   true);
	DoQuestsCleanup();




	int iFirstTurnForGlobalQuests = GetFirstPossibleTurnForGlobalQuests();


	if(GetGlobalQuestCountdown() > 0)
	{
		ChangeGlobalQuestCountdown(-1);
	}


	if(GC.getGame().getElapsedGameTurns() == iFirstTurnForGlobalQuests)
	{
		DoTestSeedGlobalQuestCountdown();
	}

	if(GC.getGame().getElapsedGameTurns() > iFirstTurnForGlobalQuests || ENABLE_QUESTS_AT_START)
	{
		DoTestStartGlobalQuest();
	}




	int iFirstTurnForPersonalQuests = GetFirstPossibleTurnForPersonalQuests();

	PlayerTypes ePlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(IsHasMetPlayer(ePlayer))
		{

			if(GetQuestCountdownForPlayer(ePlayer) > 0)
			{
				ChangeQuestCountdownForPlayer(ePlayer, -1);
			}


			if(GC.getGame().getElapsedGameTurns() == iFirstTurnForPersonalQuests)
			{
				DoTestSeedQuestCountdownForPlayer(ePlayer);
			}

			else if(GC.getGame().getElapsedGameTurns() > iFirstTurnForPersonalQuests || ENABLE_QUESTS_AT_START)
			{
				if(!IsAtWarWithPlayersTeam(ePlayer))
				{
					DoTestStartPersonalQuest(ePlayer);
				}
			}
		}
	}
}


int CvMinorCivAI::GetFirstPossibleTurnForPersonalQuests() const
{
	return        GC.getMINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN();
}


int CvMinorCivAI::GetFirstPossibleTurnForGlobalQuests() const
{
	return        GC.getMINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN();
}


int CvMinorCivAI::GetMaxActivePersonalQuestsForPlayer() const
{
	return 2;
}


int CvMinorCivAI::GetMaxActiveGlobalQuests() const
{
	return 1;
}


void CvMinorCivAI::DoTestStartGlobalQuest()
{
	if(GetGlobalQuestCountdown() > 0)
	{
		return;
	}

	if(GetNumActiveGlobalQuests() >= GetMaxActiveGlobalQuests())
	{
		return;
	}


	FStaticVector<MinorCivQuestTypes, NUM_MINOR_CIV_QUEST_TYPES, true, c_eCiv5GameplayDLL, 0> veValidQuests;
	MinorCivQuestTypes eQuest;
	PlayerTypes ePlayer;
	for(int iQuestLoop = 0; iQuestLoop < NUM_MINOR_CIV_QUEST_TYPES; iQuestLoop++)
	{
		eQuest = (MinorCivQuestTypes) iQuestLoop;
		if(IsEnabledQuest(eQuest))
		{
			if(IsGlobalQuest(eQuest))
			{

				int iNumValidPlayers = 0;
				for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
				{
					ePlayer = (PlayerTypes) iPlayerLoop;
					if(IsValidQuestForPlayer(ePlayer, eQuest))
						iNumValidPlayers++;
				}
				if(iNumValidPlayers < GetMinPlayersNeededForQuest(eQuest))
					continue;


				int iCount = GetPersonalityQuestBias(eQuest);
				for(int iCountLoop = 0; iCountLoop < iCount; iCountLoop++)
				{
					veValidQuests.push_back(eQuest);
				}
			}
		}
	}


	if(veValidQuests.size() == 0)
	{
		return;
	}


	int iRandIndex = GC.getGame().getJonRandNum(veValidQuests.size(), "Picking random global quest for Minor to give to players.");
	eQuest = veValidQuests[iRandIndex];


	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;
		if(IsValidQuestForPlayer(ePlayer, eQuest))
		{

			AddQuestForPlayer(ePlayer, eQuest, GC.getGame().getGameTurn());
		}
	}


	DoTestSeedGlobalQuestCountdown();
}


void CvMinorCivAI::DoTestStartPersonalQuest(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");


	if(GetQuestCountdownForPlayer(ePlayer) > 0)
	{
		return;
	}


	if(GetNumActivePersonalQuestsForPlayer(ePlayer) >= GetMaxActivePersonalQuestsForPlayer())
	{
		return;
	}

	FStaticVector<MinorCivQuestTypes, NUM_MINOR_CIV_QUEST_TYPES, true, c_eCiv5GameplayDLL, 0> veValidQuests;

	int iCount, iCountLoop;

	MinorCivQuestTypes eQuest;
	for(int iQuestLoop = 0; iQuestLoop < NUM_MINOR_CIV_QUEST_TYPES; iQuestLoop++)
	{
		eQuest = (MinorCivQuestTypes) iQuestLoop;

		if(IsValidQuestForPlayer(ePlayer, eQuest) && IsPersonalQuest(eQuest))
		{

			iCount = GetPersonalityQuestBias(eQuest);

			for(iCountLoop = 0; iCountLoop < iCount; iCountLoop++)
			{
				veValidQuests.push_back(eQuest);
			}
		}
	}


	if(veValidQuests.size() == 0)
	{
		return;
	}

	int iRandIndex = GC.getGame().getJonRandNum(veValidQuests.size(), "Picking random quest for Minor to give to a player.");
	eQuest = veValidQuests[iRandIndex];

	AddQuestForPlayer(ePlayer, eQuest, GC.getGame().getGameTurn());


	DoTestSeedQuestCountdownForPlayer(ePlayer);
}


void CvMinorCivAI::AddQuestForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eType, int iStartTurn)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	CvAssertMsg(eType >= NO_MINOR_CIV_QUEST_TYPE, "eType is expected to be non-negative (invalid Index)");
	CvAssertMsg(eType < NUM_MINOR_CIV_QUEST_TYPES, "eType is expected to be within maximum bounds (invalid Index)");
	if(eType < NO_MINOR_CIV_QUEST_TYPE || eType >= NUM_MINOR_CIV_QUEST_TYPES) return;


	CvAssertMsg(!IsActiveQuestForPlayer(ePlayer, eType), "ePlayer already has a quest of this type!");
	if(IsActiveQuestForPlayer(ePlayer, eType)) return;

	CvMinorCivQuest newQuest(GetPlayer()->GetID(), ePlayer, eType);
	newQuest.DoStartQuest(iStartTurn);
	m_QuestsGiven[ePlayer].push_back(newQuest);


	GET_PLAYER(ePlayer).GetDiplomacyAI()->LogMinorCivQuestReceived(GetPlayer()->GetID(), GetEffectiveFriendshipWithMajorTimes100(ePlayer), GetEffectiveFriendshipWithMajorTimes100(ePlayer), eType);
}


void CvMinorCivAI::AddQuestCopyForPlayer(PlayerTypes ePlayer, CvMinorCivQuest* pQuest)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	CvAssertMsg(pQuest != NULL, "pQuest should not be NULL. Please send Anton your save file and version.");
	if(pQuest == NULL) return;


	CvAssertMsg(!IsActiveQuestForPlayer(ePlayer, pQuest->GetType()), "ePlayer already has a quest of this type!");
	if(IsActiveQuestForPlayer(ePlayer, pQuest->GetType())) return;

	CvMinorCivQuest newQuest(GetPlayer()->GetID(), ePlayer, pQuest->GetType());
	newQuest.DoStartQuestUsingExistingData(pQuest);
	m_QuestsGiven[ePlayer].push_back(newQuest);


	GET_PLAYER(ePlayer).GetDiplomacyAI()->LogMinorCivQuestReceived(GetPlayer()->GetID(), GetEffectiveFriendshipWithMajorTimes100(ePlayer), GetEffectiveFriendshipWithMajorTimes100(ePlayer), pQuest->GetType());
}





void CvMinorCivAI::DoTestQuestsOnFirstContact(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;


	if(GetNumActiveGlobalQuests() > 0)
	{
		std::set<MinorCivQuestTypes> setGlobalQuests;

		MinorCivQuestTypes eQuestType;
		for(uint iPlayerLoop = 0; iPlayerLoop < m_QuestsGiven.size(); iPlayerLoop++)
		{
			for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[iPlayerLoop].size(); iQuestLoop++)
			{
				CvMinorCivQuest quest = m_QuestsGiven[iPlayerLoop][iQuestLoop];
				CvMinorCivQuest* pQuest = &quest;
				if(!pQuest)
				{
					CvAssertMsg(false, "pQuest should not be NULL. Please send Anton your save file and version.");
					continue;
				}
				eQuestType = pQuest->GetType();
				if(IsGlobalQuest(eQuestType))
				{

					if(setGlobalQuests.find(eQuestType) == setGlobalQuests.end())
					{

						if(IsValidQuestCopyForPlayer(eMajor, pQuest))
						{
							AddQuestCopyForPlayer(eMajor, pQuest);
							setGlobalQuests.insert(eQuestType);
						}
					}
				}
			}
		}
	}
}


void CvMinorCivAI::DoTestActiveQuests(bool bTestComplete, bool bTestObsolete)
{
	if (bTestComplete)
		DoCompletedQuests();
	
	if (bTestObsolete)
		DoObsoleteQuests();
}



void CvMinorCivAI::DoTestActiveQuestsForPlayer(PlayerTypes ePlayer, bool bTestComplete, bool bTestObsolete, MinorCivQuestTypes eQuest)
{
	if (bTestComplete)
		DoCompletedQuestsForPlayer(ePlayer, eQuest);
	
	if (bTestObsolete)
		DoObsoleteQuestsForPlayer(ePlayer, eQuest);
}


void CvMinorCivAI::DoCompletedQuests()
{
	WeightedCivsList vePlayers = CalculateFriendshipFromQuests();


	for (int iMajorIndex = 0; iMajorIndex < vePlayers.size(); iMajorIndex++)
	{
		PlayerTypes eMajor = vePlayers.GetElement(iMajorIndex);
		DoCompletedQuestsForPlayer(eMajor);
	}
}



WeightedCivsList CvMinorCivAI::CalculateFriendshipFromQuests()
{
	WeightedCivsList vePlayerInfluences;
	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		PlayerTypes eMajorLoop = (PlayerTypes) iMajorLoop;
		int iInfTimes100 = GetEffectiveFriendshipWithMajorTimes100(eMajorLoop);
		if (IsHasMetPlayer(eMajorLoop) && !IsAtWarWithPlayersTeam(eMajorLoop))
		{
			QuestListForPlayer::iterator itr_quest;
			for (itr_quest = m_QuestsGiven[eMajorLoop].begin(); itr_quest != m_QuestsGiven[eMajorLoop].end(); itr_quest++)
			{
				if (itr_quest->IsComplete())
				{
					iInfTimes100 += (itr_quest->GetInfluenceReward() * 100);
				}
			}
		}
		int iWeight = iInfTimes100 + abs(GC.getMINOR_FRIENDSHIP_AT_WAR() * 100);
		vePlayerInfluences.push_back(eMajorLoop, iWeight);
	}

	vePlayerInfluences.SortItems();
	return vePlayerInfluences;
}



void CvMinorCivAI::DoCompletedQuestsForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eSpecifyQuestType)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	bool bCheckAllQuests = true;
	if (eSpecifyQuestType > NO_MINOR_CIV_QUEST_TYPE && eSpecifyQuestType < NUM_MINOR_CIV_QUEST_TYPES)
		bCheckAllQuests = false;

	QuestListForPlayer::iterator itr_quest;
	for (itr_quest = m_QuestsGiven[ePlayer].begin(); itr_quest != m_QuestsGiven[ePlayer].end(); itr_quest++)
	{
		if (bCheckAllQuests || itr_quest->GetType() == eSpecifyQuestType)
		{
			if (itr_quest->IsComplete())
			{
				int iOldFriendshipTimes100 = GetEffectiveFriendshipWithMajorTimes100(ePlayer);
				bool bCompleted = itr_quest->DoFinishQuest();
				int iNewFriendshipTimes100 = GetEffectiveFriendshipWithMajorTimes100(ePlayer);
				
				if (bCompleted)
				{
					GET_PLAYER(ePlayer).GetDiplomacyAI()->LogMinorCivQuestFinished(GetPlayer()->GetID(), iOldFriendshipTimes100, iNewFriendshipTimes100, itr_quest->GetType());
				}
			}
		}
	}
}


void CvMinorCivAI::DoObsoleteQuests()
{
	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		PlayerTypes eMajorLoop = (PlayerTypes) iMajorLoop;
		DoObsoleteQuestsForPlayer(eMajorLoop);
	}
}



void CvMinorCivAI::DoObsoleteQuestsForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eSpecifyQuestType)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	bool bCheckAllQuests = true;
	if (eSpecifyQuestType > NO_MINOR_CIV_QUEST_TYPE && eSpecifyQuestType < NUM_MINOR_CIV_QUEST_TYPES)
		bCheckAllQuests = false;

	bool bQuestRevokedFromBullying = false;

	QuestListForPlayer::iterator itr_quest;
	for(itr_quest = m_QuestsGiven[ePlayer].begin(); itr_quest != m_QuestsGiven[ePlayer].end(); itr_quest++)
	{
		if(bCheckAllQuests || itr_quest->GetType() == eSpecifyQuestType)
		{
			if(itr_quest->IsObsolete())
			{
				int iOldFriendshipTimes100 = GetEffectiveFriendshipWithMajorTimes100(ePlayer);
				bool bCancelled = itr_quest->DoCancelQuest();
				int iNewFriendshipTimes100 = GetEffectiveFriendshipWithMajorTimes100(ePlayer);
				
				if (bCancelled)
				{
					if(itr_quest->IsRevoked())
						bQuestRevokedFromBullying = true;

					GET_PLAYER(ePlayer).GetDiplomacyAI()->LogMinorCivQuestCancelled(GetPlayer()->GetID(), iOldFriendshipTimes100, iNewFriendshipTimes100, itr_quest->GetType());
				}
			}
		}
	}


	if(bQuestRevokedFromBullying)
	{
		Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_REVOKED");
		Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_REVOKED");
		strMessage << GetPlayer()->getNameKey();
		strSummary << GetPlayer()->getNameKey();
		AddQuestNotification(strMessage.toUTF8(), strSummary.toUTF8(), ePlayer);
	}
}

void CvMinorCivAI::DoQuestsCleanup()
{
	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		PlayerTypes eMajorLoop = (PlayerTypes) iMajorLoop;
		DoQuestsCleanupForPlayer(eMajorLoop);
	}
}


void CvMinorCivAI::DoQuestsCleanupForPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	bool bPersonalQuestDone = false;
	bool bGlobalQuestDone = false;

	QuestListForPlayer::iterator itr_quest;
	for (itr_quest = m_QuestsGiven[ePlayer].begin(); itr_quest != m_QuestsGiven[ePlayer].end(); itr_quest++)
	{
		if (itr_quest->IsHandled())
		{
			MinorCivQuestTypes eQuestType = itr_quest->GetType();
			if (IsPersonalQuest(eQuestType))
				bPersonalQuestDone = true;
			if (IsGlobalQuest(eQuestType))
				bGlobalQuestDone = true;

			m_QuestsGiven[ePlayer].erase(itr_quest);
			itr_quest--;
		}
	}


	if (bPersonalQuestDone)
		DoTestSeedQuestCountdownForPlayer(ePlayer);
	if (bGlobalQuestDone)
		DoTestSeedGlobalQuestCountdown();
}


bool CvMinorCivAI::IsEnabledQuest(MinorCivQuestTypes eQuest)
{

	if(eQuest == MINOR_CIV_QUEST_ROUTE)
	{
		if(GC.getQUEST_DISABLED_ROUTE() == 1)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_KILL_CAMP)
	{
		if(GC.getQUEST_DISABLED_KILL_CAMP() == 1)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_CONNECT_RESOURCE)
	{
		if(GC.getQUEST_DISABLED_CONNECT_RESOURCE() == 1)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
	{
		if(GC.getQUEST_DISABLED_CONSTRUCT_WONDER() == 1)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_GREAT_PERSON)
	{
		if(GC.getQUEST_DISABLED_GREAT_PERSON() == 1)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_KILL_CITY_STATE)
	{
		if(GC.getQUEST_DISABLED_KILL_CITY_STATE() == 1)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_FIND_PLAYER)
	{
		if(GC.getQUEST_DISABLED_FIND_PLAYER() == 1)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_FIND_NATURAL_WONDER)
	{
		if(GC.getQUEST_DISABLED_NATURAL_WONDER() == 1)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_GIVE_GOLD)
	{
		if(GC.getQUEST_DISABLED_GIVE_GOLD() == 1)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_PLEDGE_TO_PROTECT)
	{
		if(GC.getQUEST_DISABLED_PLEDGE_TO_PROTECT() == 1)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{
		if(GC.getGame().isOption(GAMEOPTION_NO_POLICIES))
			return false;

		if(GC.getQUEST_DISABLED_CONTEST_CULTURE() == 1)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_CONTEST_FAITH)
	{
		if(GC.getGame().isOption(GAMEOPTION_NO_RELIGION))
			return false;

		if(GC.getQUEST_DISABLED_CONTEST_FAITH() == 1)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		if(GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
			return false;

		if(GC.getQUEST_DISABLED_CONTEST_TECHS() == 1)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_INVEST)
	{
		if(GC.getQUEST_DISABLED_INVEST() == 1)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_BULLY_CITY_STATE)
	{
		if(GC.getQUEST_DISABLED_BULLY_CITY_STATE() == 1)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_DENOUNCE_MAJOR)
	{
		if(GC.getQUEST_DISABLED_DENOUNCE_MAJOR() == 1)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_SPREAD_RELIGION)
	{
		if(GC.getGame().isOption(GAMEOPTION_NO_RELIGION))
			return false;

		if(GC.getQUEST_DISABLED_SPREAD_RELIGION() == 1)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_TRADE_ROUTE)
	{
		if(GC.getQUEST_DISABLED_TRADE_ROUTE() == 1)
			return false;
	}

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	if(IsQuestBlockedByPersonality(eQuest))
	{
		return false;
	}
#endif

	return true;
}


bool CvMinorCivAI::IsValidQuestForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eQuest)
{

	if(!IsEnabledQuest(eQuest))
		return false;


	if(!GET_PLAYER(ePlayer).isAlive() || !GetPlayer()->isAlive())
		return false;


	if(IsAtWarWithPlayersTeam(ePlayer))
		return false;


	if(!IsHasMetPlayer(ePlayer))
		return false;


	if(IsActiveQuestForPlayer(ePlayer, eQuest))
		return false;


	CvCity* pMinorsCapital = GetPlayer()->getCapitalCity();
	CvCity* pMajorsCapital = GET_PLAYER(ePlayer).getCapitalCity();
	if(pMinorsCapital == NULL || pMajorsCapital == NULL)
		return false;


	if(eQuest == MINOR_CIV_QUEST_ROUTE)
	{

		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;
#if !defined(LEKMOD_CITYSTATE_QUEST_CHANGES)

		if(!IsEverFriends(ePlayer))
			return false;
#endif

		if (GET_PLAYER(ePlayer).IsCapitalConnectedToPlayer(GetPlayer()->GetID()))
			return false;


		if(IsRouteConnectionEstablished(ePlayer))
			return false;


		const int iMaxRouteDistance = 8;
		CvPlot* pMinorsPlot = pMinorsCapital->plot();
		if(pMinorsPlot == NULL)
			return false;

		int iLoop;
		CvCity* pLoopCity;
		bool bInRange = false;
		for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
		{
			if(pMinorsPlot->getArea() != pLoopCity->getArea())
				continue;
			int iDistance = plotDistance(pMinorsPlot->getX(), pMinorsPlot->getY(), pLoopCity->getX(), pLoopCity->getY());
			if(iDistance < iMaxRouteDistance)
				bInRange = true;
		}

		if(!bInRange)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_KILL_CAMP)
	{

		if(GetBestNearbyCampToKill() == NULL)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_CONNECT_RESOURCE)
	{

		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;

		ResourceTypes eResource = GetNearbyResourceForQuest(ePlayer);

		if(eResource == NO_RESOURCE)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
	{

		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;

		BuildingTypes eWonder = GetBestWonderForQuest(ePlayer);

		if(eWonder == NO_BUILDING)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_GREAT_PERSON)
	{

		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;
		UnitTypes eUnit = GetBestGreatPersonForQuest(ePlayer);

		if(eUnit == NO_UNIT)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_KILL_CITY_STATE)
	{
#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
		CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
		if(pkPersonalityInfo && pkPersonalityInfo->IsBlocksKillCityStateQuest())
			return false;
#else

		if(GetPersonality() == MINOR_CIV_PERSONALITY_FRIENDLY)
			return false;
#endif

		PlayerTypes eTargetCityState = GetBestCityStateTarget(ePlayer);

		if(eTargetCityState == NO_PLAYER)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_FIND_PLAYER)
	{

		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;

		PlayerTypes eTarget = GetBestPlayerToFind(ePlayer);

		if(eTarget == NO_PLAYER)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_FIND_NATURAL_WONDER)
	{

		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;

		if(!IsGoodTimeForNaturalWonderQuest(ePlayer))
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_GIVE_GOLD)
	{
#ifdef LEKMOD_MINOR_CIV_PERSONALITIES

		if(!IsIgnoreBulliedForGoldQuest() && !IsEverBulliedByAnyMajor())
			return false;
#else

		if(!IsEverBulliedByAnyMajor())
			return false;
#endif


		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;


		if(GetMostRecentBullyForQuest() == ePlayer)
			return false;

		if(!IsGoodTimeForGiveGoldQuest())
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_PLEDGE_TO_PROTECT)
	{

		if(!IsEverBulliedByAnyMajor())
			return false;


		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;


		if(GetMostRecentBullyForQuest() == ePlayer)
			return false;


		if(!CanMajorStartProtection(ePlayer))
			return false;

		if(!IsGoodTimeForPledgeToProtectQuest())
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{
	}

	else if(eQuest == MINOR_CIV_QUEST_CONTEST_FAITH)
	{
	}

	else if(eQuest == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
	}

	else if(eQuest == MINOR_CIV_QUEST_INVEST)
	{

		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_BULLY_CITY_STATE)
	{

		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;

		PlayerTypes eTargetCityState = NO_PLAYER;
#if defined(LEKMOD_CITYSTATE_QUEST_CHANGES)
		eTargetCityState = GetBestBullyQuestTarget(ePlayer);
#else
		eTargetCityState = GetBestCityStateTarget(ePlayer);
#endif

		if(eTargetCityState == NO_PLAYER)
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_DENOUNCE_MAJOR)
	{

		if(!IsEverBulliedByAnyMajor())
			return false;


		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;


		if(!IsGoodTimeForDenounceMajorQuest())
			return false;


		if(GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR))
			return false;
		
		PlayerTypes eMostRecentBully = GetMostRecentBullyForQuest();

		if(eMostRecentBully == NO_PLAYER)
			return false;


		if(eMostRecentBully == ePlayer)
			return false;


		if(GET_PLAYER(ePlayer).getTeam() == GET_PLAYER(eMostRecentBully).getTeam())
			return false;


		if(!GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GET_PLAYER(eMostRecentBully).getTeam()))
			return false;


		if(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(GET_PLAYER(eMostRecentBully).getTeam()))
			return false;


		if(GET_PLAYER(ePlayer).isHuman() && GET_PLAYER(eMostRecentBully).isHuman())
			return false;


		if(GET_PLAYER(ePlayer).GetDiplomacyAI()->IsDenouncedPlayer(GetMostRecentBullyForQuest()))
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_SPREAD_RELIGION)
	{

		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;


		if(!GC.getGame().GetGameReligions()->HasCreatedReligion(ePlayer))
			return false;


		if(IsSameReligionAsMajor(ePlayer))
			return false;
	}

	else if(eQuest == MINOR_CIV_QUEST_TRADE_ROUTE)
	{

		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;


		if(GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(ePlayer, GetPlayer()->GetID()))
			return false;


		if(!GC.getGame().GetGameTrade()->CanCreateTradeRoute(ePlayer,GetPlayer()->GetID(), DOMAIN_LAND) &&
			!GC.getGame().GetGameTrade()->CanCreateTradeRoute(ePlayer,GetPlayer()->GetID(), DOMAIN_SEA))
			return false;
	}
	return true;
}


bool CvMinorCivAI::IsValidQuestCopyForPlayer(PlayerTypes ePlayer, CvMinorCivQuest* pQuest)
{
	if(!pQuest)
	{
		CvAssertMsg(false, "Checking validity of copying an existing quest, but pQuest is NULL. Please send Anton your save file and version.");
		return false;
	}

	MinorCivQuestTypes eQuestType = pQuest->GetType();


	if(!IsEnabledQuest(eQuestType))
		return false;


	if(!GET_PLAYER(ePlayer).isAlive() || !GetPlayer()->isAlive())
		return false;


	if(IsAtWarWithPlayersTeam(ePlayer))
		return false;


	if(!IsHasMetPlayer(ePlayer) && !GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GET_TEAM(GetPlayer()->getTeam()).GetID()))
		return false;


	if(IsActiveQuestForPlayer(ePlayer, eQuestType))
		return false;


	CvCity* pMinorsCapital = GetPlayer()->getCapitalCity();
	CvCity* pMajorsCapital = GET_PLAYER(ePlayer).getCapitalCity();
	if(pMinorsCapital == NULL || pMajorsCapital == NULL)
		return false;


	if(eQuestType == MINOR_CIV_QUEST_KILL_CAMP)
	{
		int iCampX = pQuest->GetPrimaryData();
		int iCampY = pQuest->GetSecondaryData();
		CvPlot* pPlot = GC.getMap().plot(iCampX, iCampY);
		if(!pPlot)
			return false;
		if(pPlot->getImprovementType() != GC.getBARBARIAN_CAMP_IMPROVEMENT())
			return false;
	}

	else if(eQuestType == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{
	}

	else if(eQuestType == MINOR_CIV_QUEST_CONTEST_FAITH)
	{
	}

	else if(eQuestType == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
	}

	else if(eQuestType == MINOR_CIV_QUEST_INVEST)
	{

		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;
	}

	else
	{
		CvAssertMsg(false, "Checking validity of copying a personal quest using existing quest data.  This should not be done.  Please send Anton your save file and version.");
		return IsValidQuestForPlayer(ePlayer, eQuestType);
	}

	return true;
}


bool CvMinorCivAI::IsGlobalQuest(MinorCivQuestTypes eQuest) const
{
	if(eQuest == MINOR_CIV_QUEST_KILL_CAMP)
		return true;

	if(eQuest == MINOR_CIV_QUEST_CONTEST_CULTURE)
		return true;

	if(eQuest == MINOR_CIV_QUEST_CONTEST_FAITH)
		return true;

	if(eQuest == MINOR_CIV_QUEST_CONTEST_TECHS)
		return true;

	if(eQuest == MINOR_CIV_QUEST_INVEST)
		return true;

	return false;
}

bool CvMinorCivAI::IsPersonalQuest(MinorCivQuestTypes eQuest) const
{
	return (!IsGlobalQuest(eQuest));
}

int CvMinorCivAI::GetMinPlayersNeededForQuest(MinorCivQuestTypes eQuest) const
{
	int iPlayersNeeded = 1;

	if(eQuest == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{
		iPlayersNeeded = 3;
	}

	else if(eQuest == MINOR_CIV_QUEST_CONTEST_FAITH)
	{
		iPlayersNeeded = 3;
	}

	else if(eQuest == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		iPlayersNeeded = 3;
	}

	else if(eQuest == MINOR_CIV_QUEST_INVEST)
	{
		iPlayersNeeded = 2;
	}

	int iMajorsEverAlive = GC.getGame().countMajorCivsEverAlive();
	iPlayersNeeded = min(iPlayersNeeded, iMajorsEverAlive);

	return iPlayersNeeded;
}


int CvMinorCivAI::GetPersonalityQuestBias(MinorCivQuestTypes eQuest)
{

	int iCount = 10 *        GC.getMINOR_CIV_QUEST_WEIGHT_DEFAULT();

	MinorCivPersonalityTypes ePersonality = GetPersonality();
	MinorCivTraitTypes eTrait = GetTrait();






	if(eQuest == MINOR_CIV_QUEST_ROUTE)
	{
#ifndef LEKMOD_MINOR_CIV_PERSONALITIES
		if(ePersonality == MINOR_CIV_PERSONALITY_FRIENDLY)
		{
			iCount *=         GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_ROUTE();
			iCount /= 100;
		}
		if(ePersonality == MINOR_CIV_PERSONALITY_HOSTILE)
		{
			iCount *=        GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_ROUTE();
			iCount /= 100;
		}
#endif
		if(eTrait == MINOR_CIV_TRAIT_MARITIME)
		{
			iCount *=         GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_ROUTE();
			iCount /= 100;
		}
		if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
		{
			iCount *=         GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_ROUTE();
			iCount /= 100;
		}
	}


	else if(eQuest == MINOR_CIV_QUEST_CONNECT_RESOURCE)
	{
		if(eTrait == MINOR_CIV_TRAIT_MARITIME)
		{
			iCount *=         GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_CONNECT_RESOURCE();
			iCount /= 100;
		}
		if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
		{
			iCount *=         GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_CONNECT_RESOURCE();
			iCount /= 100;
		}
	}


	else if(eQuest == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
	{
		if(eTrait == MINOR_CIV_TRAIT_CULTURED)
		{
			iCount *=         GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_CONSTRUCT_WONDER();
			iCount /= 100;
		}
	}


	else if(eQuest == MINOR_CIV_QUEST_GREAT_PERSON)
	{
		if(eTrait == MINOR_CIV_TRAIT_CULTURED)
		{
			iCount *=         GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_GREAT_PERSON();
			iCount /= 100;
		}
	}


	else if(eQuest == MINOR_CIV_QUEST_KILL_CITY_STATE)
	{
		if(eTrait == MINOR_CIV_TRAIT_MILITARISTIC)
		{
			iCount *=         GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_KILL_CITY_STATE();
			iCount /= 100;
		}
#ifndef LEKMOD_MINOR_CIV_PERSONALITIES
		if(ePersonality == MINOR_CIV_PERSONALITY_HOSTILE)
		{
			iCount *=         GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_KILL_CITY_STATE();
			iCount /= 100;
		}
		if(ePersonality == MINOR_CIV_PERSONALITY_NEUTRAL)
		{
			iCount *=        GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_NEUTRAL_KILL_CITY_STATE();
			iCount /= 100;
		}
#endif
	}


	else if(eQuest == MINOR_CIV_QUEST_FIND_PLAYER)
	{
		if(eTrait == MINOR_CIV_TRAIT_MARITIME)
		{
			iCount *=         GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_FIND_PLAYER();
			iCount /= 100;
		}
		if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
		{
			iCount *=         GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_FIND_PLAYER();
			iCount /= 100;
		}
	}


	else if(eQuest == MINOR_CIV_QUEST_FIND_NATURAL_WONDER)
	{
		if(eTrait == MINOR_CIV_TRAIT_MILITARISTIC)
		{
			iCount *=        GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_FIND_NATURAL_WONDER();
			iCount /= 100;
		}
#ifndef LEKMOD_MINOR_CIV_PERSONALITIES
		if(ePersonality == MINOR_CIV_PERSONALITY_HOSTILE)
		{
			iCount *=        GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_FIND_NATURAL_WONDER();
			iCount /= 100;
		}
#endif
	}


	else if(eQuest == MINOR_CIV_QUEST_GIVE_GOLD)
	{


		if(eTrait == MINOR_CIV_TRAIT_MILITARISTIC)
		{
			iCount *=         GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_GIVE_GOLD();
			iCount /= 100;
		}
		else if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
		{
			iCount *=         GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_GIVE_GOLD();
			iCount /= 100;
		}
		else
		{
			iCount *=         GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_GIVE_GOLD();
			iCount /= 100;
		}
	}


	else if(eQuest == MINOR_CIV_QUEST_PLEDGE_TO_PROTECT)
	{
		if(eTrait == MINOR_CIV_TRAIT_MILITARISTIC)
		{
			iCount *=         GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_PLEDGE_TO_PROTECT();
			iCount /= 100;
		}
		else if(eTrait == MINOR_CIV_TRAIT_CULTURED)
		{
			iCount *=         GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_PLEDGE_TO_PROTECT();
			iCount /= 100;
		}
		else
		{
			iCount *=         GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_PLEDGE_TO_PROTECT();
			iCount /= 100;
		}
	}


	else if(eQuest == MINOR_CIV_QUEST_BULLY_CITY_STATE)
	{
#ifndef LEKMOD_MINOR_CIV_PERSONALITIES
		if(ePersonality == MINOR_CIV_PERSONALITY_HOSTILE)
		{
			iCount *= 200;
			iCount /= 100;
		}
		else if(ePersonality == MINOR_CIV_PERSONALITY_IRRATIONAL)
		{
			iCount *= 150;
			iCount /= 100;
		}
		else if(ePersonality == MINOR_CIV_PERSONALITY_FRIENDLY)
		{
			iCount *= 30;
			iCount /= 100;
		}
#endif
	}


	else if(eQuest == MINOR_CIV_QUEST_DENOUNCE_MAJOR)
	{
		if(eTrait == MINOR_CIV_TRAIT_RELIGIOUS)
		{
			iCount *= 250;
			iCount /= 100;
		}
		else if(eTrait == MINOR_CIV_TRAIT_MARITIME)
		{
			iCount *= 200;
			iCount /= 100;
		}
		else
		{
			iCount *= 150;
			iCount /= 100;
		}
	}


	else if(eQuest == MINOR_CIV_QUEST_SPREAD_RELIGION)
	{
		if(eTrait == MINOR_CIV_TRAIT_RELIGIOUS)
		{
			iCount *= 300;
			iCount /= 100;
		}
	}


	else if(eQuest == MINOR_CIV_QUEST_TRADE_ROUTE)
	{
		if(eTrait == MINOR_CIV_TRAIT_MARITIME)
		{
			iCount *= 200;
			iCount /= 100;
		}
		else if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
		{
			iCount *= 200;
			iCount /= 100;
		}
	}






	else if(eQuest == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{
		if(eTrait == MINOR_CIV_TRAIT_CULTURED)
		{
			iCount *= 200;
			iCount /= 100;
		}
	}

	else if(eQuest == MINOR_CIV_QUEST_CONTEST_FAITH)
	{
		if(eTrait == MINOR_CIV_TRAIT_RELIGIOUS)
		{
			iCount *= 200;
			iCount /= 100;
		}
		else
		{
			iCount *= 50;
			iCount /= 100;
		}
	}

	else if(eQuest == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		if(eTrait == MINOR_CIV_TRAIT_RELIGIOUS)
		{
			iCount += 50;
			iCount /= 100;
		}
	}


	else if(eQuest == MINOR_CIV_QUEST_KILL_CAMP)
	{
		iCount *= 300;
		iCount /= 100;

		if(eTrait == MINOR_CIV_TRAIT_MILITARISTIC)
		{
			iCount *=         GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_KILL_CAMP();
			iCount /= 100;
		}
	}


	else if(eQuest == MINOR_CIV_QUEST_INVEST)
	{
		if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
		{
			iCount *= 150;
			iCount /= 100;
		}
	}

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	{
		CvMinorCivPersonalityInfo* pkPersonalityInfo = GC.getMinorCivPersonalityInfo((int)ePersonality);
		if(pkPersonalityInfo)
		{
			const int iPersonalityMultiplier = pkPersonalityInfo->GetQuestWeightMultiplier(eQuest);
			if(iPersonalityMultiplier != 100)
			{
				iCount *= iPersonalityMultiplier;
				iCount /= 100;
			}
		}
	}
#endif

	return iCount / 10;
}


int CvMinorCivAI::GetNumActiveGlobalQuests() const
{


	std::set<MinorCivQuestTypes> setGlobalQuests;

	for(uint iPlayerLoop = 0; iPlayerLoop < m_QuestsGiven.size(); iPlayerLoop++)
	{
		for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[iPlayerLoop].size(); iQuestLoop++)
		{
			if(IsGlobalQuest(m_QuestsGiven[iPlayerLoop][iQuestLoop].GetType()))
			{
				setGlobalQuests.insert(m_QuestsGiven[iPlayerLoop][iQuestLoop].GetType());
			}
		}
	}

	return setGlobalQuests.size();
}

int CvMinorCivAI::GetNumActiveQuestsForPlayer(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0;

	return m_QuestsGiven[ePlayer].size();
}

int CvMinorCivAI::GetNumActivePersonalQuestsForPlayer(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0;

	int iCount = 0;
	for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(IsPersonalQuest(m_QuestsGiven[ePlayer][iQuestLoop].GetType()))
		{
			iCount++;
		}
	}

	return iCount;
}

bool CvMinorCivAI::IsActiveQuestForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eType)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	CvAssertMsg(eType >= NO_MINOR_CIV_QUEST_TYPE, "eType is expected to be non-negative (invalid Index)");
	CvAssertMsg(eType < NUM_MINOR_CIV_QUEST_TYPES, "eType is expected to be within maximum bounds (invalid Index)");
	if(eType < NO_MINOR_CIV_QUEST_TYPE || eType >= NUM_MINOR_CIV_QUEST_TYPES) return false;

	for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			return true;
		}
	}

	return false;
}


void CvMinorCivAI::EndAllActiveQuestsForPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;


	if(GetNumActiveQuestsForPlayer(ePlayer) > 0)
		m_QuestsGiven[ePlayer].clear();
}

int CvMinorCivAI::GetNumDisplayedQuestsForPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0;

	int iValue = 0;
	
	for (uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(!m_QuestsGiven[ePlayer][iQuestLoop].IsHandled())
		{
			iValue++;
		}
	}

	return iValue;
}

bool CvMinorCivAI::IsDisplayedQuestForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eType)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	CvAssertMsg(eType >= NO_MINOR_CIV_QUEST_TYPE, "eType is expected to be non-negative (invalid Index)");
	CvAssertMsg(eType < NUM_MINOR_CIV_QUEST_TYPES, "eType is expected to be within maximum bounds (invalid Index)");
	if(eType < NO_MINOR_CIV_QUEST_TYPE || eType >= NUM_MINOR_CIV_QUEST_TYPES) return false;

	for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			if(!m_QuestsGiven[ePlayer][iQuestLoop].IsHandled())
			{
				return true;
			}
		}
	}

	return false;
}

void CvMinorCivAI::DoTestSeedGlobalQuestCountdown(bool bForceSeed)
{
	if(!bForceSeed)
	{

		if(GetNumActiveGlobalQuests() >= GetMaxActiveGlobalQuests())
		{
			return;
		}


		if(GetGlobalQuestCountdown() > 0)
		{
			return;
		}
	}

	int iNumTurns = 0;


	if(GC.getGame().getElapsedGameTurns() == GetFirstPossibleTurnForGlobalQuests())
	{
		int iRand =        GC.getMINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN_RAND();
		iNumTurns += GC.getGame().getJonRandNum(iRand, "Random # of turns for Minor Civ global quest counter - first time.");
	}
	else
	{
		iNumTurns +=        GC.getMINOR_CIV_GLOBAL_QUEST_MIN_TURNS_BETWEEN();

		int iRand =        GC.getMINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN();
#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
		CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
		if(pkPersonalityInfo)
		{
			const int iMultiplier = pkPersonalityInfo->GetGlobalQuestRandTurnsMultiplier();
			if(iMultiplier != 100)
			{
				iRand *= iMultiplier;
				iRand /= 100;
			}
		}
#else
		if(GetPersonality() == MINOR_CIV_PERSONALITY_HOSTILE)
		{
			iRand *=         GC.getMINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER();
			iRand /= 100;
		}
#endif
		iNumTurns += GC.getGame().getJonRandNum(iRand, "Random # of turns for Minor Civ global quest counter.");
	}


	iNumTurns *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iNumTurns /= 100;

	SetGlobalQuestCountdown(iNumTurns);
}

int CvMinorCivAI::GetGlobalQuestCountdown()
{
	return m_iGlobalQuestCountdown;
}

void CvMinorCivAI::SetGlobalQuestCountdown(int iValue)
{
	m_iGlobalQuestCountdown = iValue;
}

void CvMinorCivAI::ChangeGlobalQuestCountdown(int iChange)
{
	SetGlobalQuestCountdown(GetGlobalQuestCountdown() + iChange);
}


void CvMinorCivAI::DoTestSeedQuestCountdownForPlayer(PlayerTypes ePlayer, bool bForceSeed)
{
	if(!bForceSeed)
	{

		if(GetNumActivePersonalQuestsForPlayer(ePlayer) >= GetMaxActivePersonalQuestsForPlayer())
		{
			return;
		}


		if(GetQuestCountdownForPlayer(ePlayer) > 0)
		{
			return;
		}
	}

	int iNumTurns = 0;


	if(GC.getGame().getElapsedGameTurns() == GetFirstPossibleTurnForPersonalQuests())
	{
		int iRand =        GC.getMINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN_RAND();
		iNumTurns += GC.getGame().getJonRandNum(iRand, "Random # of turns for Minor Civ personal quest counter - first time.");
	}
	else
	{
		iNumTurns +=        GC.getMINOR_CIV_PERSONAL_QUEST_MIN_TURNS_BETWEEN();

		int iRand =        GC.getMINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN();
#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
		CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
		if(pkPersonalityInfo)
		{
			const int iMultiplier = pkPersonalityInfo->GetPersonalQuestRandTurnsMultiplier();
			if(iMultiplier != 100)
			{
				iRand *= iMultiplier;
				iRand /= 100;
			}
		}
#else
		if(GetPersonality() == MINOR_CIV_PERSONALITY_HOSTILE)
		{
			iRand *=         GC.getMINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER();
			iRand /= 100;
		}
#endif
		iNumTurns += GC.getGame().getJonRandNum(iRand, "Random # of turns for Minor Civ personal quest counter.");
	}


	iNumTurns *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iNumTurns /= 100;

	SetQuestCountdownForPlayer(ePlayer, iNumTurns);
}


int CvMinorCivAI::GetQuestCountdownForPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1;

	return m_aiQuestCountdown[ePlayer];
}


void CvMinorCivAI::SetQuestCountdownForPlayer(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_aiQuestCountdown[ePlayer] = iValue;
}


void CvMinorCivAI::ChangeQuestCountdownForPlayer(PlayerTypes ePlayer, int iChange)
{
	SetQuestCountdownForPlayer(ePlayer, GetQuestCountdownForPlayer(ePlayer) + iChange);
}


bool CvMinorCivAI::AddQuestIfAble(PlayerTypes eMajor, MinorCivQuestTypes eQuest)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;

	CvAssertMsg(eQuest >= NO_MINOR_CIV_QUEST_TYPE, "eQuest is expected to be non-negative (invalid Index)");
	CvAssertMsg(eQuest < NUM_MINOR_CIV_QUEST_TYPES, "eQuest is expected to be within maximum bounds (invalid Index)");
	if(eQuest < NO_MINOR_CIV_QUEST_TYPE || eQuest >= NUM_MINOR_CIV_QUEST_TYPES) return false;

	if(!IsHasMetPlayer(eMajor))
		return false;

	if(IsAtWarWithPlayersTeam(eMajor))
		return false;

	if (!IsValidQuestForPlayer(eMajor, eQuest))
		return false;

	if (IsActiveQuestForPlayer(eMajor, eQuest))
		return false;

	CvAssert(IsPersonalQuest(eQuest) || IsGlobalQuest(eQuest));
	if (IsPersonalQuest(eQuest))
	{
		if(GetNumActivePersonalQuestsForPlayer(eMajor) >= GetMaxActivePersonalQuestsForPlayer())
		{
			return false;
		}

		AddQuestForPlayer(eMajor, eQuest, GC.getGame().getGameTurn());
		return true;
	}
	else if (IsGlobalQuest(eQuest))
	{
		if(GetNumActiveGlobalQuests() >= GetMaxActiveGlobalQuests())
		{
			return false;
		}

		int iNumValidPlayers = 0;
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes ePlayerLoop = (PlayerTypes) iPlayerLoop;
			if(IsValidQuestForPlayer(ePlayerLoop, eQuest))
				iNumValidPlayers++;
		}
		if(iNumValidPlayers < GetMinPlayersNeededForQuest(eQuest))
			return false;

		for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes ePlayerLoop = (PlayerTypes) iPlayerLoop;
			if(IsValidQuestForPlayer(ePlayerLoop, eQuest))
				AddQuestForPlayer(ePlayerLoop, eQuest, GC.getGame().getGameTurn());
		}
		return true;
	}

	return false;
}


int CvMinorCivAI::GetQuestData1(PlayerTypes ePlayer, MinorCivQuestTypes eType) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) CvMinorCivQuest::NO_QUEST_DATA;

	for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			return m_QuestsGiven[ePlayer][iQuestLoop].GetPrimaryData();
		}
	}

	return CvMinorCivQuest::NO_QUEST_DATA;
}


int CvMinorCivAI::GetQuestData2(PlayerTypes ePlayer, MinorCivQuestTypes eType) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return CvMinorCivQuest::NO_QUEST_DATA;

	for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			return m_QuestsGiven[ePlayer][iQuestLoop].GetSecondaryData();
		}
	}

	return CvMinorCivQuest::NO_QUEST_DATA;
}

int CvMinorCivAI::GetQuestTurnsRemaining(PlayerTypes ePlayer, MinorCivQuestTypes eType, int iGameTurn) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return CvMinorCivQuest::NO_TURN;

	for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			return m_QuestsGiven[ePlayer][iQuestLoop].GetTurnsRemaining(iGameTurn);
		}
	}

	return CvMinorCivQuest::NO_TURN;
}

bool CvMinorCivAI::IsContestLeader(PlayerTypes ePlayer, MinorCivQuestTypes eType)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	for (uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if (m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			return m_QuestsGiven[ePlayer][iQuestLoop].IsContestLeader(ePlayer);
		}
	}

	return false;
}

int CvMinorCivAI::GetContestValueForLeader(MinorCivQuestTypes eType)
{
	PlayerTypes eParticipant;
	for (int iMajor = 0; iMajor < MAX_MAJOR_CIVS; iMajor++)
	{
		eParticipant = (PlayerTypes) iMajor;
		for (uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[eParticipant].size(); iQuestLoop++)
		{
			if (m_QuestsGiven[eParticipant][iQuestLoop].GetType() == eType)
			{
				return m_QuestsGiven[eParticipant][iQuestLoop].GetContestValueForLeader();
			}
		}
	}

	return -1;
}

int CvMinorCivAI::GetContestValueForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eType)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
#ifdef AUI_WARNING_FIXES
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1;
#else
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;
#endif

	for (uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if (m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			return m_QuestsGiven[ePlayer][iQuestLoop].GetContestValueForPlayer(ePlayer);
		}
	}

	return -1;
}


bool CvMinorCivAI::IsRouteConnectionEstablished(PlayerTypes eMajor) const
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;

	return m_abRouteConnectionEstablished[eMajor];
}


void CvMinorCivAI::SetRouteConnectionEstablished(PlayerTypes eMajor, bool bValue)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;

	if(m_abRouteConnectionEstablished[eMajor] != bValue)
	{
		m_abRouteConnectionEstablished[eMajor] = bValue;
	}
}



CvPlot* CvMinorCivAI::GetBestNearbyCampToKill()
{
	CvCity* pCapital = GetPlayer()->getCapitalCity();


	if(pCapital == NULL)
	{
		return NULL;
	}

	CvWeightedVector<int, 64, true> viPlotIndexes;

	int iRange =        GC.getMINOR_CIV_QUEST_KILL_CAMP_RANGE();

	CvPlot* pLoopPlot;


	int iDX, iDY;
#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX;
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
			pLoopPlot = plotXY(pCapital->getX(), pCapital->getY(), iDX, iDY);

			if(pLoopPlot != NULL)
			{

				if(pLoopPlot->getArea() != pCapital->getArea())
				{
					continue;
				}

#ifndef AUI_HEXSPACE_DX_LOOPS
				int iDistance = plotDistance(pCapital->getX(), pCapital->getY(), pLoopPlot->getX(), pLoopPlot->getY());

				if(iDistance <= iRange)
#endif
				{

					if(pLoopPlot->getOwner() == NO_PLAYER)
					{

						if(pLoopPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
						{
#ifdef AUI_HEXSPACE_DX_LOOPS
							int iWeight = 1 + (iRange - hexDistance(iDX, iDY));
#else
							int iWeight = 1 + (iRange - iDistance);
#endif
							viPlotIndexes.push_back(pLoopPlot->GetPlotIndex(), iWeight);
						}
					}
				}
			}
		}
	}


	if(viPlotIndexes.size() == 0)
	{
		return NULL;
	}


	viPlotIndexes.SortItems();
	CvPlot* pBestPlot = GC.getMap().plotByIndex(viPlotIndexes.GetElement(0));

	return pBestPlot;
}


ResourceTypes CvMinorCivAI::GetNearbyResourceForQuest(PlayerTypes ePlayer)
{
	ResourceTypes eBestResource = NO_RESOURCE;

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	if(GET_PLAYER(ePlayer).getStartingPlot() != NULL)
	{
		CvArea* pPlayerArea = GC.getMap().getArea(GET_PLAYER(ePlayer).getStartingPlot()->getArea());

		FStaticVector<ResourceTypes, 64, true, c_eCiv5GameplayDLL, 0> veValidResources;
		TechTypes eRevealTech;
		TechTypes eConnectTech;


		ResourceTypes eResource;
#ifdef AUI_WARNING_FIXES
		for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
		{
			eResource = (ResourceTypes) iResourceLoop;

			const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);

			if(pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_BONUS)
			{
				continue;
			}


			if(GetPlayer()->getNumResourceTotal(eResource,                    true) > 0)
			{
				continue;
			}


			if(GET_PLAYER(ePlayer).getNumResourceTotal(eResource,                    true) > 0)
			{
				continue;
			}


			eRevealTech = (TechTypes) pkResourceInfo->getTechReveal();
			if(!GET_TEAM(eTeam).GetTeamTechs()->HasTech(eRevealTech))
			{
				continue;
			}
			int iRevealPolicy = pkResourceInfo->getPolicyReveal();
			if (iRevealPolicy != NO_POLICY && !(GET_PLAYER(ePlayer).GetPlayerPolicies()->HasPolicy((PolicyTypes)iRevealPolicy)))
			{
				continue;
			}


			eConnectTech = (TechTypes) pkResourceInfo->getTechCityTrade();
			if(!GET_TEAM(eTeam).GetTeamTechs()->HasTech(eConnectTech))
			{
				continue;
			}

#ifdef NQ_NO_UNIQUE_LUX_REQUESTS

			if(pkResourceInfo->GetRequiredCivilization() != NULL && pkResourceInfo->GetRequiredCivilization() != NO_CIVILIZATION)
			{
				continue;
			}
#endif

			if(pPlayerArea->getNumResources(eResource) == 0)
			{
				continue;
			}

			veValidResources.push_back(eResource);
		}


		if(veValidResources.size() == 0)
		{
			return NO_RESOURCE;
		}

		int iRandIndex = GC.getGame().getJonRandNum(veValidResources.size(), "Finding random Resource for Minor to give out a quest to connect.");
		eBestResource = veValidResources[iRandIndex];
	}

	return eBestResource;
}


BuildingTypes CvMinorCivAI::GetBestWonderForQuest(PlayerTypes ePlayer)
{
	BuildingTypes eBestWonder;

	FStaticVector<BuildingTypes, 50, true, c_eCiv5GameplayDLL, 0> veValidBuildings;

	int iWorldPlayerLoop;
	PlayerTypes eWorldPlayer;
	CvCity* pLoopCity;
	int iCityLoop;
	int iWonderProgress;
	int iCompletionThreshold =        GC.getMINOR_CIV_QUEST_WONDER_COMPLETION_THRESHOLD();
	bool bFoundWonderTooFarAlong;


#ifdef AUI_WARNING_FIXES
	for (uint iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
#else
	for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
#endif
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);


		if(pkBuildingInfo == NULL)
			continue;

		bFoundWonderTooFarAlong = false;


		if(!isWorldWonderClass(pkBuildingInfo->GetBuildingClassInfo()))
		{
			continue;
		}


		if(!GET_PLAYER(ePlayer).canConstruct(eBuilding))
		{
			continue;
		}
#if defined(LEKMOD_CITYSTATE_QUEST_CHANGES)
		const TechTypes eRequiredTech = (TechTypes)pkBuildingInfo->GetPrereqAndTech(); 
		const int iWonderEra = GC.getTechInfo(eRequiredTech)->GetEra(); 
		if ((iWonderEra + 1) < GET_PLAYER(ePlayer).GetCurrentEra())
			continue;
#endif


		for(iWorldPlayerLoop = 0; iWorldPlayerLoop < MAX_MAJOR_CIVS; iWorldPlayerLoop++)
		{
			eWorldPlayer = (PlayerTypes) iWorldPlayerLoop;

			for(pLoopCity = GET_PLAYER(eWorldPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eWorldPlayer).nextCity(&iCityLoop))
			{
				iWonderProgress = pLoopCity->GetCityBuildings()->GetBuildingProduction(eBuilding);

				if(iWonderProgress * 100 / pLoopCity->getProductionNeeded(eBuilding) >= iCompletionThreshold)
				{
					bFoundWonderTooFarAlong = true;
					break;
				}
			}
			if(bFoundWonderTooFarAlong)
			{
				break;
			}
		}
		if(bFoundWonderTooFarAlong)
		{
			continue;
		}

		veValidBuildings.push_back(eBuilding);
	}


	if(veValidBuildings.size() == 0)
	{
		return NO_BUILDING;
	}

	int iRandIndex = GC.getGame().getJonRandNum(veValidBuildings.size(), "Finding random Wonder for Minor to give out a quest to construct.");
	eBestWonder = veValidBuildings[iRandIndex];

	return eBestWonder;
}



UnitTypes CvMinorCivAI::GetBestGreatPersonForQuest(PlayerTypes ePlayer)
{
	UnitTypes eBestUnit = NO_UNIT;

	SpecialUnitTypes eSpecialUnitGreatPerson = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_PEOPLE");

	FStaticVector<UnitTypes, 8, true, c_eCiv5GameplayDLL, 0> veValidUnits;


#ifdef AUI_WARNING_FIXES
	for (uint iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
#else
	for(int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
#endif
	{
		const UnitTypes eUnit = static_cast<UnitTypes>(iUnitLoop);
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);

		if(pkUnitInfo == NULL)
			continue;


		if(pkUnitInfo->GetProductionCost() != -1)
		{
			continue;
		}


		if(pkUnitInfo->GetSpecialUnitType() != eSpecialUnitGreatPerson)
		{
			continue;
		}


		if(!GET_PLAYER(ePlayer).canTrain(eUnit, false              , false                 , true                ))
		{
			continue;
		}


		if(GC.getGame().isOption(GAMEOPTION_NO_RELIGION))
		{
			if(pkUnitInfo->IsFoundReligion())
			{
				continue;
			}
		}
		if(GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
		{
			if(pkUnitInfo->GetBaseBeakersTurnsToCount() > 0)
			{
				continue;
			}
		}

		bool bAlreadyHasUnit = false;


		int iLoop;
		for(CvUnit* pLoopUnit = GET_PLAYER(ePlayer).firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = GET_PLAYER(ePlayer).nextUnit(&iLoop))
		{
			if(pLoopUnit->getUnitType() == eUnit)
			{
				bAlreadyHasUnit = true;
				break;
			}
		}
		if(bAlreadyHasUnit)
		{
			continue;
		}

#if defined(LEKMOD_CITYSTATE_QUEST_CHANGES)
		const UnitAITypes eDefaultAI = (UnitAITypes) pkUnitInfo->GetDefaultUnitAIType();
		if(eDefaultAI == UNITAI_MUSICIAN || eDefaultAI == UNITAI_GENERAL || eDefaultAI == UNITAI_ADMIRAL)
		{
			const int iGameSpeedPercent = GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
			const int iEarliestTurn = (90 * iGameSpeedPercent) / 100;
			if(GC.getGame().getGameTurn() < iEarliestTurn)
			{
				continue;
			}
		}
#endif

		veValidUnits.push_back(eUnit);
	}


	if(veValidUnits.size() == 0)
	{
		return NO_UNIT;
	}

	int iRandIndex = GC.getGame().getJonRandNum(veValidUnits.size(), "Finding random Great Person for Minor to give out a quest to spawn.");
	eBestUnit = veValidUnits[iRandIndex];

	return eBestUnit;
}




PlayerTypes CvMinorCivAI::GetBestCityStateTarget(PlayerTypes eForPlayer)
{
	CvAssertMsg(eForPlayer >= 0, "eForPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eForPlayer < MAX_MAJOR_CIVS, "eForPlayer is expected to be within maximum bounds (invalid Index)");

	PlayerTypes eBestCityStateTarget = NO_PLAYER;

	PlayerProximityTypes eClosestProximity = PLAYER_PROXIMITY_DISTANT;


	int iTargetLoop;
	PlayerTypes eTarget;
	for(iTargetLoop = MAX_MAJOR_CIVS; iTargetLoop < MAX_CIV_PLAYERS; iTargetLoop++)
	{
		eTarget = (PlayerTypes) iTargetLoop;

		if(!GET_PLAYER(eTarget).isAlive())
			continue;

		if(GetPlayer()->getTeam() == GET_PLAYER(eTarget).getTeam())
			continue;

		if(GetPlayer()->GetProximityToPlayer(eTarget) > eClosestProximity)
		{
			eClosestProximity = GetPlayer()->GetProximityToPlayer(eTarget);
		}
	}


	if(eClosestProximity == PLAYER_PROXIMITY_DISTANT)
	{
		return NO_PLAYER;
	}

	FStaticVector<PlayerTypes, MAX_CIV_PLAYERS, true, c_eCiv5GameplayDLL, 0> veValidTargets;


	for(iTargetLoop = MAX_MAJOR_CIVS; iTargetLoop < MAX_CIV_PLAYERS; iTargetLoop++)
	{
		eTarget = (PlayerTypes) iTargetLoop;

		if(!GET_PLAYER(eTarget).isAlive())
			continue;

		if(GetPlayer()->getTeam() == GET_PLAYER(eTarget).getTeam())
			continue;

		if(!GET_TEAM(GET_PLAYER(eForPlayer).getTeam()).isHasMet(GET_PLAYER(eTarget).getTeam()))
			continue;

		if(GetPlayer()->GetProximityToPlayer(eTarget) == eClosestProximity)
		{
			veValidTargets.push_back(eTarget);
		}
	}


	if(veValidTargets.size() == 0)
		return NO_PLAYER;

	int iRandIndex = GC.getGame().getJonRandNum(veValidTargets.size(), "Finding random City State Target for Minor to give out a quest to kill.");
	eBestCityStateTarget = veValidTargets[iRandIndex];

	return eBestCityStateTarget;
}

#if defined(LEKMOD_CITYSTATE_QUEST_CHANGES)
PlayerTypes CvMinorCivAI::GetBestBullyQuestTarget(PlayerTypes eForPlayer)
{
	CvAssertMsg(eForPlayer >= 0, "eForPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eForPlayer < MAX_MAJOR_CIVS, "eForPlayer is expected to be within maximum bounds (invalid Index)");

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	if(!pkPersonalityInfo || !pkPersonalityInfo->IsHostileOnlyBullyQuestTarget())
	{
		return NO_PLAYER;
	}
#else
	if(GetPersonality() != MINOR_CIV_PERSONALITY_HOSTILE)
	{
		return NO_PLAYER;
	}
#endif

	CvPlayer* pMajor = &GET_PLAYER(eForPlayer);
	if(!pMajor || !pMajor->isAlive())
	{
		return NO_PLAYER;
	}

	CvCity* pMajorCapital = pMajor->getCapitalCity();
	if(!pMajorCapital)
	{
		return NO_PLAYER;
	}

	CvPlot* pMajorCapitalPlot = pMajorCapital->plot();
	if(!pMajorCapitalPlot)
	{
		return NO_PLAYER;
	}

	PlayerProximityTypes eClosestProximity = PLAYER_PROXIMITY_DISTANT;

	for(int iTargetLoop = MAX_MAJOR_CIVS; iTargetLoop < MAX_CIV_PLAYERS; iTargetLoop++)
	{
		PlayerTypes eTarget = (PlayerTypes) iTargetLoop;

		if(!GET_PLAYER(eTarget).isAlive())
			continue;

		if(GetPlayer()->getTeam() == GET_PLAYER(eTarget).getTeam())
			continue;

		if(!GET_TEAM(pMajor->getTeam()).isHasMet(GET_PLAYER(eTarget).getTeam()))
			continue;

		PlayerProximityTypes eProximity = GetPlayer()->GetProximityToPlayer(eTarget);
		if(eProximity > eClosestProximity)
		{
			eClosestProximity = eProximity;
		}
	}

	if(eClosestProximity == PLAYER_PROXIMITY_DISTANT)
	{
		return NO_PLAYER;
	}

	FStaticVector<PlayerTypes, MAX_CIV_PLAYERS, true, c_eCiv5GameplayDLL, 0> veValidTargets;

	for(int iTargetLoop = MAX_MAJOR_CIVS; iTargetLoop < MAX_CIV_PLAYERS; iTargetLoop++)
	{
		PlayerTypes eTarget = (PlayerTypes) iTargetLoop;
		CvPlayer* pTarget = &GET_PLAYER(eTarget);
		if(!pTarget || !pTarget->isAlive())
			continue;

		if(GetPlayer()->getTeam() == pTarget->getTeam())
			continue;

		if(!GET_TEAM(pMajor->getTeam()).isHasMet(pTarget->getTeam()))
			continue;

		if(GetPlayer()->GetProximityToPlayer(eTarget) != eClosestProximity)
			continue;

		CvCity* pTargetCapital = pTarget->getCapitalCity();
		if(!pTargetCapital)
			continue;

		CvPlot* pTargetPlot = pTargetCapital->plot();
		if(!pTargetPlot)
			continue;

		const int iCapitalDistance = plotDistance(pMajorCapital->getX(), pMajorCapital->getY(), pTargetPlot->getX(), pTargetPlot->getY());
		const bool bWithinCapitalRange = (iCapitalDistance != -1 && iCapitalDistance <= 20);

		int iClosestCityDistance = INT_MAX;
		int iLoop = 0;
		for(CvCity* pLoopCity = pMajor->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pMajor->nextCity(&iLoop))
		{
			CvPlot* pLoopPlot = pLoopCity->plot();
			if(!pLoopPlot)
				continue;

			const int iDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pTargetPlot->getX(), pTargetPlot->getY());
			if(iDistance >= 0 && iDistance < iClosestCityDistance)
			{
				iClosestCityDistance = iDistance;
			}
		}
		const bool bWithinCityRange = (iClosestCityDistance != INT_MAX && iClosestCityDistance <= 10);

		if(!bWithinCapitalRange && !bWithinCityRange)
			continue;

		const bool bPlayerStronger = (pMajor->GetMilitaryMight() > pTarget->GetMilitaryMight());
		const bool bTargetUnderstaffed = (pTarget->getNumMilitaryUnits() < 3);

		if(!bPlayerStronger && !bTargetUnderstaffed)
			continue;

		veValidTargets.push_back(eTarget);
	}

	if(veValidTargets.size() == 0)
	{
		return NO_PLAYER;
	}

	const int iRandIndex = GC.getGame().getJonRandNum(veValidTargets.size(), "Finding random Bully quest target.");
	return veValidTargets[iRandIndex];
}
#endif


PlayerTypes CvMinorCivAI::GetMostRecentBullyForQuest() const
{
	PlayerTypes eBully = NO_PLAYER;
	int iTurn = -1;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{

		if(!GET_PLAYER((PlayerTypes)iPlayerLoop).isAlive())
			continue;

		if(m_aiTurnLastBullied[iPlayerLoop] > iTurn)
		{
			iTurn = m_aiTurnLastBullied[iPlayerLoop];
			eBully = (PlayerTypes)iPlayerLoop;
		}
	}

	return eBully;
}


bool CvMinorCivAI::IsWantsMinorDead(PlayerTypes eMinor)
{
	PlayerTypes eMajor;
	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;


		if(!GET_PLAYER(eMajor).isAlive())
			continue;


		if(!IsActiveQuestForPlayer(eMajor, MINOR_CIV_QUEST_KILL_CITY_STATE))
			continue;


		if(GetQuestData1(eMajor, MINOR_CIV_QUEST_KILL_CITY_STATE) != eMinor)
			continue;

		return true;
	}

	return false;
}


PlayerTypes CvMinorCivAI::GetBestPlayerToFind(PlayerTypes ePlayer)
{
	PlayerTypes eBestTargetPlayer = NO_PLAYER;

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
	CvTeam* pTeam = &GET_TEAM(eTeam);

	FStaticVector<PlayerTypes, MAX_CIV_PLAYERS, true, c_eCiv5GameplayDLL, 0> veValidTargets;


	PlayerTypes eTargetMajor;
	TeamTypes eTargetTeam;
	for(int iTargetMajorLoop = 0; iTargetMajorLoop < MAX_MAJOR_CIVS; iTargetMajorLoop++)
	{
		eTargetMajor = (PlayerTypes) iTargetMajorLoop;
		eTargetTeam = GET_PLAYER(eTargetMajor).getTeam();


		if(!GET_PLAYER(eTargetMajor).isAlive())
		{
			continue;
		}


		if(eTargetTeam == eTeam)
		{
			continue;
		}


		if(!pTeam->isHasMet(eTargetTeam))
		{
			continue;
		}


		if(pTeam->IsHasFoundPlayersTerritory(eTargetMajor))
		{
			continue;
		}

		veValidTargets.push_back(eTargetMajor);
	}


	if(veValidTargets.size() == 0)
	{
		return NO_PLAYER;
	}

	int iRandIndex = GC.getGame().getJonRandNum(veValidTargets.size(), "Finding random Major player Target for Minor to give out a quest to find.");
	eBestTargetPlayer = veValidTargets[iRandIndex];

	return eBestTargetPlayer;
}


bool CvMinorCivAI::IsGoodTimeForNaturalWonderQuest(PlayerTypes ePlayer)
{
	CvPlayer* pPlayer = &GET_PLAYER(ePlayer);
	CvTeam* pTeam = &GET_TEAM(pPlayer->getTeam());


	if(pPlayer->getStartingPlot() == NULL)
	{
		return false;
	}


	if(pTeam->GetNumNaturalWondersDiscovered() == GC.getMap().GetNumNaturalWonders())
	{
		return false;
	}


	int iNumNaturalWondersInStartingArea = GC.getMap().getArea(pPlayer->getStartingPlot()->getArea())->GetNumNaturalWonders();
	if(pPlayer->GetNumNaturalWondersDiscoveredInArea() < iNumNaturalWondersInStartingArea)
	{
		return false;
	}

	return true;
}


bool CvMinorCivAI::IsGoodTimeForGiveGoldQuest()
{
#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	if(IsIgnoreBulliedForGoldQuest())
	{
		return true;
	}
#endif

	if(IsRecentlyBulliedByAnyMajor())
	{
		return true;
	}

	return false;
}


bool CvMinorCivAI::IsGoodTimeForPledgeToProtectQuest()
{
	if(IsRecentlyBulliedByAnyMajor())
	{
		return true;
	}

	return false;
}


bool CvMinorCivAI::IsGoodTimeForDenounceMajorQuest()
{
	if(IsRecentlyBulliedByAnyMajor())
	{
		return true;
	}

	return false;
}









void CvMinorCivAI::DoFriendship()
{
	Localization::String strMessage;
	Localization::String strSummary;
	const char* strMinorsNameKey = GetPlayer()->getNameKey();

	PlayerTypes ePlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(GET_PLAYER(ePlayer).isAlive())
		{



			int iOldFriendship = GetBaseFriendshipWithMajor(ePlayer);
			int iChangeThisTurn = GetFriendshipChangePerTurnTimes100(ePlayer);
			int iFriendshipAnchor = GetFriendshipAnchorWithMajor(ePlayer);
			int iNewFriendship = iOldFriendship + (iChangeThisTurn / 100);
			if(iOldFriendship >= iFriendshipAnchor && iNewFriendship < iFriendshipAnchor)
			{

				SetFriendshipWithMajor(ePlayer, iFriendshipAnchor);
			}
			else if (iChangeThisTurn != 0)
			{
				ChangeFriendshipWithMajorTimes100(ePlayer, iChangeThisTurn);
			}
			else
			{

				DoFriendshipChangeEffects(ePlayer, iOldFriendship, iNewFriendship);
			}


			if(GetPlayer()->isAlive() && IsHasMetPlayer(ePlayer))
			{
				const int iTurnsWarning = 2;
				const int iAlliesThreshold = GetAlliesThreshold() * 100;
				const int iFriendsThreshold = GetFriendsThreshold() * 100;
				int iEffectiveFriendship = GetEffectiveFriendshipWithMajorTimes100(ePlayer);
				if(IsAllies(ePlayer))
				{
					if(iEffectiveFriendship + (iTurnsWarning * iChangeThisTurn) < iAlliesThreshold &&
						iEffectiveFriendship + ((iTurnsWarning-1) * iChangeThisTurn) >= iAlliesThreshold)
					{
						strMessage = Localization::Lookup("TXT_KEY_NTFN_CITY_STATE_ALMOST_NOT_ALLIES");
						strMessage << strMinorsNameKey;
						strSummary = Localization::Lookup("TXT_KEY_NTFN_CITY_STATE_ALMOST_SM");
						strSummary << strMinorsNameKey;

						AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), ePlayer);
					}
					if(!GC.getGame().isGameMultiPlayer() && GET_PLAYER(ePlayer).isHuman())
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_CITYSTATE_ALLY);
					}

				}
				else if(IsFriends(ePlayer))
				{
					if(iEffectiveFriendship + (iTurnsWarning * iChangeThisTurn) < iFriendsThreshold &&
						iEffectiveFriendship + ((iTurnsWarning-1) * iChangeThisTurn) >= iFriendsThreshold)
					{
						strMessage = Localization::Lookup("TXT_KEY_NTFN_CITY_STATE_ALMOST_NOT_FRIENDS");
						strMessage << strMinorsNameKey;
						strSummary = Localization::Lookup("TXT_KEY_NTFN_CITY_STATE_ALMOST_SM");
						strSummary << strMinorsNameKey;

						AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), ePlayer);
					}
				}
			}
		}
	}
}




int CvMinorCivAI::GetFriendshipChangePerTurnTimes100(PlayerTypes ePlayer)
{
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	int iChangeThisTurn = 0;


	int iTraitMod = kPlayer.GetPlayerTraits()->GetCityStateFriendshipModifier();
	int iReligionMod = 0;
#ifdef LEKMOD_BELIEF_CITY_STATE_FOLLOWING_RELIGION_INFLUENCE
	int iBeliefDecayMod = 0;
	int iBeliefRecoveryMod = 0;
#endif
	if (IsSameReligionAsMajor(ePlayer))
	{
		iReligionMod +=        GC.getMINOR_FRIENDSHIP_RATE_MOD_SHARED_RELIGION();
#ifdef LEKMOD_BELIEF_CITY_STATE_FOLLOWING_RELIGION_INFLUENCE
		const ReligionTypes eFounderReligion = GC.getGame().GetGameReligions()->GetReligionCreatedByPlayer(ePlayer);
		if (eFounderReligion != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eFounderReligion, ePlayer);
			if (pReligion)
			{
				iBeliefDecayMod = pReligion->m_Beliefs.GetCityStateFollowingReligionDecayMod();
				iBeliefRecoveryMod = pReligion->m_Beliefs.GetCityStateFollowingReligionRecoveryMod();
			}
		}
#endif
	}

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	if(pkPersonalityInfo)
	{
		if(IsSameReligionAsMajor(ePlayer))
		{
			const int iSharedMod = pkPersonalityInfo->GetSharedReligionDecayRecoveryModifierPercent();
			if(iReligionMod != 0 && iSharedMod != 100)
			{
				iReligionMod *= iSharedMod;
				iReligionMod /= 100;
			}
		}
		else
		{
			ReligionTypes eMinorReligion = NO_RELIGION;
			const ReligionTypes eMajorReligion = GET_PLAYER(ePlayer).GetReligions()->GetReligionInMostCities();
			CvCity* pCapital = GetPlayer()->getCapitalCity();
			if(pCapital)
			{
				eMinorReligion = pCapital->GetCityReligions()->GetReligiousMajority();
			}

			if(eMinorReligion != NO_RELIGION && eMajorReligion != NO_RELIGION && eMinorReligion != eMajorReligion)
			{
				iReligionMod += pkPersonalityInfo->GetDifferentReligionDecayRecoveryModifierPercent();
			}
		}
	}
#endif


	int iBaseFriendship = GetBaseFriendshipWithMajor(ePlayer);
	int iFriendshipAnchor = GetFriendshipAnchorWithMajor(ePlayer);
	if (iBaseFriendship == iFriendshipAnchor)
	{

	}
	else if (iBaseFriendship > iFriendshipAnchor)
	{
#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
		if(pkPersonalityInfo && pkPersonalityInfo->GetFriendshipDropPerTurn() != 0)
			iChangeThisTurn += pkPersonalityInfo->GetFriendshipDropPerTurn();
		else if(GET_TEAM(kPlayer.getTeam()).IsMinorCivAggressor())
			iChangeThisTurn +=          GC.getMINOR_FRIENDSHIP_DROP_PER_TURN_AGGRESSOR();
		else
			iChangeThisTurn +=          GC.getMINOR_FRIENDSHIP_DROP_PER_TURN();
#else

		if(GetPersonality() == MINOR_CIV_PERSONALITY_HOSTILE)
			iChangeThisTurn +=          GC.getMINOR_FRIENDSHIP_DROP_PER_TURN_HOSTILE();

		else if(GET_TEAM(kPlayer.getTeam()).IsMinorCivAggressor())
			iChangeThisTurn +=          GC.getMINOR_FRIENDSHIP_DROP_PER_TURN_AGGRESSOR();

		else
			iChangeThisTurn +=          GC.getMINOR_FRIENDSHIP_DROP_PER_TURN();
#endif


		int iDecayMod = 100;
		iDecayMod += GET_PLAYER(ePlayer).GetMinorFriendshipDecayMod();
		iDecayMod += (-1) * (iTraitMod / 2);
		iDecayMod += (-1) * (iReligionMod / 2);
#ifdef LEKMOD_BELIEF_CITY_STATE_FOLLOWING_RELIGION_INFLUENCE
		iDecayMod += (-1) * (iBeliefDecayMod / 2);
#endif
		if (iDecayMod < 0)
			iDecayMod = 0;

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
		if(pkPersonalityInfo && pkPersonalityInfo->GetFriendshipDecayModifierPercent() != 100 && !IsMajorExcludedFromPersonalityBonuses(ePlayer))
		{
			iDecayMod *= pkPersonalityInfo->GetFriendshipDecayModifierPercent();
			iDecayMod /= 100;
		}
#endif

		iChangeThisTurn *= iDecayMod;
		iChangeThisTurn /= 100;
	}
	else
	{
		iChangeThisTurn +=         GC.getMINOR_FRIENDSHIP_NEGATIVE_INCREASE_PER_TURN();


		int iRecoveryMod = 100;
		iRecoveryMod += iTraitMod;
		iRecoveryMod += iReligionMod;
#ifdef LEKMOD_BELIEF_CITY_STATE_FOLLOWING_RELIGION_INFLUENCE
		iRecoveryMod += iBeliefRecoveryMod;
#endif
		
		if (iRecoveryMod < 0)
			iRecoveryMod = 0;

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
		if(pkPersonalityInfo && pkPersonalityInfo->GetFriendshipRecoveryModifierPercent() != 100)
		{
			iRecoveryMod *= pkPersonalityInfo->GetFriendshipRecoveryModifierPercent();
			iRecoveryMod /= 100;
		}
#endif

		iChangeThisTurn *= iRecoveryMod;
		iChangeThisTurn /= 100;
	}


	if (GET_TEAM(kPlayer.getTeam()).isHasMet(GetPlayer()->getTeam()))
	{
		int iShift = 0;

		if (kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_PROTECTED_MINOR_INFLUENCE) != 0)
		{
			if (GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(ePlayer, GetPlayer()->GetID()))
			{
				iShift += kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_PROTECTED_MINOR_INFLUENCE);
			}
		}

		if (CanMajorBullyGold(ePlayer))
		{
			iShift += kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_AFRAID_INFLUENCE);
			iShift += kPlayer.GetPlayerTraits()->GetAfraidMinorPerTurnInfluence();
		}
		
		if (iShift != 0)
		{
			iChangeThisTurn += iShift;
		}
	}


	iChangeThisTurn *= GC.getGame().getGameSpeedInfo().getGoldGiftMod();
	iChangeThisTurn /= 100;

	return iChangeThisTurn;
}




int CvMinorCivAI::GetEffectiveFriendshipWithMajorTimes100(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0;


	if(IsAtWarWithPlayersTeam(ePlayer))
		return (100 *        GC.getMINOR_FRIENDSHIP_AT_WAR());

	return GetBaseFriendshipWithMajorTimes100(ePlayer);
}


int CvMinorCivAI::GetBaseFriendshipWithMajorTimes100(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0;

	return m_aiFriendshipWithMajorTimes100[ePlayer];
}


void CvMinorCivAI::SetFriendshipWithMajorTimes100(PlayerTypes ePlayer, int iNum, bool bFromQuest)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	int iOldEffectiveFriendship = GetEffectiveFriendshipWithMajorTimes100(ePlayer);

	m_aiFriendshipWithMajorTimes100[ePlayer] = iNum;

	int iMinimumFriendship = GC.getMINOR_FRIENDSHIP_AT_WAR();
	if(GetBaseFriendshipWithMajor(ePlayer) < iMinimumFriendship)
		m_aiFriendshipWithMajorTimes100[ePlayer] = iMinimumFriendship * 100;

	int iNewEffectiveFriendship = GetEffectiveFriendshipWithMajorTimes100(ePlayer);


	if(iOldEffectiveFriendship != iNewEffectiveFriendship)
	{
		DoFriendshipChangeEffects(ePlayer, iOldEffectiveFriendship/100, iNewEffectiveFriendship/100, bFromQuest);
	}


	if(ePlayer == GC.getGame().getActivePlayer())
	{
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
	}
}


void CvMinorCivAI::ChangeFriendshipWithMajorTimes100(PlayerTypes ePlayer, int iChange, bool bFromQuest)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{

		if(bFromQuest && iChange > 0)
		{
			if(GET_PLAYER(ePlayer).getMinorQuestFriendshipMod() != 0)
			{
				iChange *= (100 + GET_PLAYER(ePlayer).getMinorQuestFriendshipMod());
				iChange /= 100;
			}

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
			const int iPersonalityQuestMod = GetPersonalityQuestInfluenceModifierPercent(ePlayer);
			if(iPersonalityQuestMod != 100)
			{
				iChange *= iPersonalityQuestMod;
				iChange /= 100;
			}
#endif
		}
#ifdef AI_CANT_COUP
		if (GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_COUP") && !GET_PLAYER(ePlayer).isHuman())
		{
			if (iChange > 0)
			{
				iChange = 0;
			}
		}
#endif
		SetFriendshipWithMajorTimes100(ePlayer, GetBaseFriendshipWithMajorTimes100(ePlayer) + iChange, bFromQuest);
	}
}




int CvMinorCivAI::GetEffectiveFriendshipWithMajor(PlayerTypes ePlayer)
{
	return GetEffectiveFriendshipWithMajorTimes100(ePlayer) / 100;
}


int CvMinorCivAI::GetBaseFriendshipWithMajor(PlayerTypes ePlayer) const
{
	return GetBaseFriendshipWithMajorTimes100(ePlayer) / 100;
}


void CvMinorCivAI::SetFriendshipWithMajor(PlayerTypes ePlayer, int iNum, bool bFromQuest)
{
	SetFriendshipWithMajorTimes100(ePlayer, iNum * 100, bFromQuest);
}


void CvMinorCivAI::ChangeFriendshipWithMajor(PlayerTypes ePlayer, int iChange, bool bFromQuest)
{
	ChangeFriendshipWithMajorTimes100(ePlayer, iChange * 100, bFromQuest);
}


int CvMinorCivAI::GetFriendshipAnchorWithMajor(PlayerTypes eMajor)
{
	int iAnchor = GC.getMINOR_FRIENDSHIP_ANCHOR_DEFAULT();

	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if (eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return iAnchor;

	CvPlayer* pMajor = &GET_PLAYER(eMajor);
	CvAssertMsg(pMajor, "MINOR CIV AI: pMajor not expected to be NULL.  Please send Anton your save file and version.");
	if (!pMajor) return iAnchor;


	if (IsProtectedByMajor(eMajor))
	{
		iAnchor += GC.getMINOR_FRIENDSHIP_ANCHOR_MOD_PROTECTED();
	}


	if (IsWaryOfTeam(pMajor->getTeam()))
	{
		iAnchor += GC.getMINOR_FRIENDSHIP_ANCHOR_MOD_WARY_OF();
	}


	iAnchor += pMajor->GetMinorFriendshipAnchorMod();


	CvPlayerReligions* pMajorReligions = pMajor->GetReligions();
	CvAssertMsg(pMajorReligions, "MINOR CIV AI: pMajorReligions not expected to be NULL.  Please send Anton your save file and version.");
	if (!pMajorReligions) return iAnchor;
	CvCity* pMinorCapital = GetPlayer()->getCapitalCity();
	if (!pMinorCapital) return iAnchor;
	CvCityReligions* pMinorCapitalReligions = pMinorCapital->GetCityReligions();
	CvAssertMsg(pMinorCapitalReligions, "MINOR CIV AI: pMinorCapitalReligions not expected to be NULL.  Please send Anton your save file and version.");
	if (!pMinorCapitalReligions) return iAnchor;
	iAnchor += pMajorReligions->GetCityStateMinimumInfluence(pMinorCapitalReligions->GetReligiousMajority());

	return iAnchor;
}


void CvMinorCivAI::ResetFriendshipWithMajor(PlayerTypes ePlayer)
{

	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	int iOldFriendship = GetEffectiveFriendshipWithMajor(ePlayer);
	const int iResetFriendship = 0;
	if(GetPlayer()->isAlive())
	{
		SetFriendshipWithMajor(ePlayer, iResetFriendship);
	}
	else
	{

		DoFriendshipChangeEffects(ePlayer, iOldFriendship, iResetFriendship,               false,                      true);
		SetFriendshipWithMajor(ePlayer, iResetFriendship);
	}

}


int CvMinorCivAI::GetAngerFreeIntrusionCounter(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0;

	return m_aiAngerFreeIntrusionCounter[ePlayer];
}


void CvMinorCivAI::SetAngerFreeIntrusionCounter(PlayerTypes ePlayer, int iNum)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_aiAngerFreeIntrusionCounter[ePlayer] = iNum;
}


void CvMinorCivAI::ChangeAngerFreeIntrusionCounter(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	SetAngerFreeIntrusionCounter(ePlayer, GetAngerFreeIntrusionCounter(ePlayer) + iChange);
}


void CvMinorCivAI::DoUpdateAlliesResourceBonus(PlayerTypes eNewAlly, PlayerTypes eOldAlly)
{
	FAssertMsg(eNewAlly != NO_PLAYER || eOldAlly != NO_PLAYER, "MINOR CIV AI: Updating Allied resource bonus and both players are not defined!");


	ResourceTypes eResource;
	ResourceUsageTypes eUsage;
	int iResourceQuantity;
#ifdef AUI_WARNING_FIXES
	for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
	{
		eResource = (ResourceTypes) iResourceLoop;

		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		if (pkResourceInfo == NULL)
			continue;

		eUsage = pkResourceInfo->getResourceUsage();

		if(eUsage == RESOURCEUSAGE_STRATEGIC || eUsage == RESOURCEUSAGE_LUXURY)
		{

			if(eOldAlly != NO_PLAYER)
			{
				iResourceQuantity = GetPlayer()->getResourceExport(eResource);

				if(iResourceQuantity > 0)
				{
					GET_PLAYER(eOldAlly).changeResourceFromMinors(eResource, -iResourceQuantity);
					GetPlayer()->changeResourceExport(eResource, -iResourceQuantity);
				}
			}


			if(eNewAlly != NO_PLAYER)
			{
				iResourceQuantity = GetPlayer()->getNumResourceTotal(eResource);
#ifdef LEKMOD_CS_BUILDING_STRATEGIC_NO_ALLY_SHARE
				if(eUsage == RESOURCEUSAGE_STRATEGIC)
				{
					const int iFromBuildings = GetPlayer()->getNumMinorStrategicResourceFromBuildings(eResource);
					iResourceQuantity -= iFromBuildings;
					if(iResourceQuantity < 0)
					{
						iResourceQuantity = 0;
					}
				}
#endif

				if(iResourceQuantity > 0)
				{
					GET_PLAYER(eNewAlly).changeResourceFromMinors(eResource, iResourceQuantity);
					GetPlayer()->changeResourceExport(eResource, iResourceQuantity);
				}
			}
		}
	}
}


int CvMinorCivAI::GetMostFriendshipWithAnyMajor(PlayerTypes& eBestPlayer)
{
	int iMostFriendship = 0;
	PlayerTypes eMajor;

	int iFriendship;

	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;

		if(IsHasMetPlayer(eMajor))
		{
			iFriendship = GetEffectiveFriendshipWithMajor(eMajor);

			if(iFriendship > iMostFriendship)
			{
				eBestPlayer = eMajor;
				iMostFriendship = iFriendship;
			}
		}
	}

	return iMostFriendship;
}


PlayerTypes CvMinorCivAI::GetAlly() const
{
	return m_eAlly;
}


void CvMinorCivAI::SetAlly(PlayerTypes eNewAlly)
{
	CvAssertMsg(eNewAlly >= NO_PLAYER, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eNewAlly < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	CvMap& theMap = GC.getMap();
#ifdef AUI_WARNING_FIXES
	uint iNumPlots = GC.getMap().numPlots();
#else
	int iNumPlots = GC.getMap().numPlots();
#endif

	PlayerTypes eOldAlly = GetAlly();

#ifdef CS_ALLYING_WAR_RESCTRICTION
	CvGame& kGame = GC.getGame();
	if (GC.getGame().isOption(GAMEOPTION_END_TURN_TIMER_ENABLED) && kGame.isNetworkMultiPlayer())
	{
		RecalculateMajorPriority();

		if (eNewAlly != NO_PLAYER)
		{
			if (GetMajorPriority(eNewAlly) == MAX_MAJOR_CIVS)
			{
				SetMajorPriority(eNewAlly, GetMaxMajorPriority() + 1);
				if (eOldAlly != NO_PLAYER)
				{
#ifdef GAME_UPDATE_TURN_TIMER_ONCE_PER_TURN
					float fGameTurnEnd = kGame.getPreviousTurnLen();
#else
					float fGameTurnEnd = static_cast<float>(kGame.getMaxTurnLen());
#endif
					float fTimeElapsed = kGame.getTimeElapsed();
					float fRestrictionTime = CS_ALLYING_WAR_RESCTRICTION_TIMER;
					if (fGameTurnEnd - fTimeElapsed > fRestrictionTime)
					{
						GET_PLAYER(eOldAlly).setPriorityTurn(GetPlayer()->GetID(), kGame.getGameTurn());
						GET_PLAYER(eOldAlly).setPriorityTime(GetPlayer()->GetID(), fTimeElapsed + fRestrictionTime);
					}
					else
					{
						GET_PLAYER(eOldAlly).setPriorityTurn(GetPlayer()->GetID(), kGame.getGameTurn() + 1);
						GET_PLAYER(eOldAlly).setPriorityTime(GetPlayer()->GetID(), fRestrictionTime - (fGameTurnEnd - fTimeElapsed));
					}
				}
			}
			else
			{
				GET_PLAYER(eNewAlly).setPriorityTurn(GetPlayer()->GetID(), -1);
				GET_PLAYER(eNewAlly).setPriorityTime(GetPlayer()->GetID(), 0.f);
				for (int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
				{
					if (GetMajorPriority((PlayerTypes)iI) > GetMajorPriority(eNewAlly))
					{
						SetMajorPriority((PlayerTypes)iI, MAX_MAJOR_CIVS);
						GET_PLAYER((PlayerTypes)iI).setPriorityTurn(GetPlayer()->GetID(), -1);
						GET_PLAYER((PlayerTypes)iI).setPriorityTime(GetPlayer()->GetID(), 0.f);
					}
				}
			}
		}
		else
		{
			for (int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
			{
				SetMajorPriority((PlayerTypes)iI, MAX_MAJOR_CIVS);
				GET_PLAYER((PlayerTypes)iI).setPriorityTurn(GetPlayer()->GetID(), -1);
				GET_PLAYER((PlayerTypes)iI).setPriorityTime(GetPlayer()->GetID(), 0.f);
			}
		}
	}
#endif

	int iPlotVisRange = GC.getPLOT_VISIBILITY_RANGE();

	if(eOldAlly != NO_PLAYER)
	{
#ifdef AUI_WARNING_FIXES
		for (uint iI = 0; iI < iNumPlots; iI++)
#else
		for(int iI = 0; iI < iNumPlots; iI++)
#endif
		{
			CvPlot* pPlot = theMap.plotByIndexUnchecked(iI);
			if(pPlot->getOwner() == m_pPlayer->GetID())
			{
				pPlot->changeAdjacentSight(GET_PLAYER(eOldAlly).getTeam(), iPlotVisRange, false, NO_INVISIBLE, NO_DIRECTION, false);
			}
		}
		if(eOldAlly == GC.getGame().getActivePlayer())
		{
			theMap.updateDeferredFog();
		}
	}

	m_eAlly = eNewAlly;
	m_iTurnAllied = GC.getGame().getGameTurn();


	if(eNewAlly != NO_PLAYER)
	{
		CvPlayerAI& kNewAlly = GET_PLAYER(eNewAlly);


#ifdef AUI_WARNING_FIXES
		for (uint iI = 0; iI < iNumPlots; iI++)
#else
		for(int iI = 0; iI < iNumPlots; iI++)
#endif
		{
			CvPlot* pPlot = theMap.plotByIndexUnchecked(iI);
			if(pPlot->getOwner() == m_pPlayer->GetID())
			{
				pPlot->changeAdjacentSight(kNewAlly.getTeam(), iPlotVisRange, true, NO_INVISIBLE, NO_DIRECTION, false);
			}
		}

#ifdef AUI_WARNING_FIXES
		for (uint iPolicyLoop = 0; iPolicyLoop < GC.getNumPolicyInfos(); iPolicyLoop++)
#else
		for(int iPolicyLoop = 0; iPolicyLoop < GC.getNumPolicyInfos(); iPolicyLoop++)
#endif
		{
			const PolicyTypes eLoopPolicy = static_cast<PolicyTypes>(iPolicyLoop);
			CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(eLoopPolicy);
			if(pkPolicyInfo)
			{
				if(kNewAlly.GetPlayerPolicies()->HasPolicy(eLoopPolicy) && !kNewAlly.GetPlayerPolicies()->IsPolicyBlocked(eLoopPolicy))
				{

					if(pkPolicyInfo->IsMinorGreatPeopleAllies())
					{
						if(kNewAlly.GetGreatPeopleSpawnCounter() <= 0)
							kNewAlly.DoSeedGreatPeopleSpawnCounter();
						else
							kNewAlly.DoApplyNewAllyGPBonus();
					}
				}
			}
		}


		kNewAlly.GetPlayerAchievements().AlliedWithCityState(GetPlayer()->GetID());;
	}


	DoUpdateAlliesResourceBonus(eNewAlly, eOldAlly);


	if(eNewAlly != NO_PLAYER)
	{
		CvPlayerAI& kNewAlly = GET_PLAYER(eNewAlly);
		CvTeam& kNewAllyTeam = GET_TEAM(kNewAlly.getTeam());
		CvTeam& kOurTeam = GET_TEAM(GetPlayer()->getTeam());

		TeamTypes eLoopTeam;
		for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
		{
			eLoopTeam = (TeamTypes) iTeamLoop;

			if(!GET_TEAM(eLoopTeam).isAlive())
				continue;

			if(kNewAllyTeam.isAtWar(eLoopTeam))
				kOurTeam.declareWar(eLoopTeam);
		}
	}

	DoTestEndWarsVSMinors(eOldAlly, eNewAlly);

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pPlayer->GetID());
		args->Push(eOldAlly);
		args->Push(eNewAlly);

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "SetAlly", args.get(), bResult);
	}
}


int CvMinorCivAI::GetAlliedTurns() const
{
	int iRtnValue = -1;

	if (m_eAlly != NO_PLAYER)
	{
		iRtnValue = GC.getGame().getGameTurn() - m_iTurnAllied;
	}

	return iRtnValue;
}


bool CvMinorCivAI::IsAllies(PlayerTypes ePlayer) const
{
	return m_eAlly == ePlayer;
}


bool CvMinorCivAI::IsFriends(PlayerTypes ePlayer)
{
	return IsFriendshipAboveFriendsThreshold(GetEffectiveFriendshipWithMajor(ePlayer));
}


bool CvMinorCivAI::IsEverFriends(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	return m_abEverFriends[ePlayer];
}


void CvMinorCivAI::SetEverFriends(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_abEverFriends[ePlayer] = bValue;
}
#if defined(LEKMOD_MERCHANT_BUYOUT_NOT_NOANNEXING)
int CvMinorCivAI::GetLastAllyTurnWithMajor(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1;

	return m_aiLastAllyTurnWithMajor[ePlayer];
}
void CvMinorCivAI::SetLastAllyTurnWithMajor(PlayerTypes ePlayer, int iTurn)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	m_aiLastAllyTurnWithMajor[ePlayer] = iTurn;
}
#endif


bool CvMinorCivAI::IsCloseToNotBeingAllies(PlayerTypes ePlayer)
{
	int iBuffer = GetEffectiveFriendshipWithMajor(ePlayer) - GetAlliesThreshold();

	if(iBuffer >= 0 && iBuffer <       GC.getMINOR_FRIENDSHIP_CLOSE_AMOUNT())
		return true;

	return false;
}


bool CvMinorCivAI::IsCloseToNotBeingFriends(PlayerTypes ePlayer)
{
	int iBuffer = GetEffectiveFriendshipWithMajor(ePlayer) - GetFriendsThreshold();

	if(iBuffer >= 0 && iBuffer <       GC.getMINOR_FRIENDSHIP_CLOSE_AMOUNT())
		return true;

	return false;
}


int CvMinorCivAI::GetFriendshipLevelWithMajor(PlayerTypes ePlayer)
{
	if(IsAllies(ePlayer))
	{
		return 2;
	}
	else if(IsFriends(ePlayer))
	{
		return 1;
	}

	return 0;
}



int CvMinorCivAI::GetFriendshipNeededForNextLevel(PlayerTypes ePlayer)
{
	if(!IsFriends(ePlayer))
	{
		return GetFriendsThreshold();
	}
	else if(!IsAllies(ePlayer))
	{
		return GetAlliesThreshold();
	}

	return 0;
}


void CvMinorCivAI::DoFriendshipChangeEffects(PlayerTypes ePlayer, int iOldFriendship, int iNewFriendship, bool bFromQuest, bool bIgnoreMinorDeath)
{

	if(!bIgnoreMinorDeath && !GetPlayer()->isAlive())
		return;

	Localization::String strMessage;
	Localization::String strSummary;

	bool bAdd = false;
	bool bFriends = false;
	bool bAllies = false;

	bool bWasAboveFriendsThreshold = IsFriendshipAboveFriendsThreshold(iOldFriendship);
	bool bNowAboveFriendsThreshold = IsFriendshipAboveFriendsThreshold(iNewFriendship);


	if(bNowAboveFriendsThreshold)
		SetEverFriends(ePlayer, true);


	if(!bWasAboveFriendsThreshold && bNowAboveFriendsThreshold)
	{
		bAdd = true;
		bFriends = true;

		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem) 
		{
			CvLuaArgsHandle args;
			args->Push(m_pPlayer->GetID());
			args->Push(ePlayer);
			args->Push(true);
			args->Push(iOldFriendship);
			args->Push(iNewFriendship);

			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "MinorFriendsChanged", args.get(), bResult);
		}
	}

	else if(bWasAboveFriendsThreshold && !bNowAboveFriendsThreshold)
	{
		bAdd = false;
		bFriends = true;

#ifdef NQ_POLICY_TOGGLE_NO_MINOR_DOW_IF_FRIENDS



		if (GetAlly() != NO_PLAYER)
		{
			TeamTypes ePlayerTeam = GET_PLAYER(ePlayer).getTeam();
			TeamTypes eAllyTeam = GET_PLAYER(GetAlly()).getTeam();
			TeamTypes eOurTeam = GetPlayer()->getTeam();
			if (GET_TEAM(ePlayerTeam).isAtWar(eAllyTeam))
			{
				GET_TEAM(eOurTeam).declareWar(ePlayerTeam);
			}
		}
#endif

		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem) 
		{
			CvLuaArgsHandle args;
			args->Push(m_pPlayer->GetID());
			args->Push(ePlayer);
			args->Push(false);
			args->Push(iOldFriendship);
			args->Push(iNewFriendship);

			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "MinorFriendsChanged", args.get(), bResult);
		}
	}


	bool bWasAboveAlliesThreshold = IsFriendshipAboveAlliesThreshold(iOldFriendship);
	bool bNowAboveAlliesThreshold = IsFriendshipAboveAlliesThreshold(iNewFriendship);

	PlayerTypes eOldAlly = GetAlly();
#ifdef NO_AI_ALLYING_CS
	if (GET_PLAYER(ePlayer).isHuman() || !GC.getGame().isOption("GAMEOPTION_AI_TWEAKS"))
	{
#endif


	if((eOldAlly == NO_PLAYER && bNowAboveAlliesThreshold)
	        || (eOldAlly != NO_PLAYER && GetEffectiveFriendshipWithMajor(ePlayer) > GetEffectiveFriendshipWithMajor(eOldAlly)))
	{
		bAdd = true;
		bAllies = true;

		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem) 
		{
			CvLuaArgsHandle args;
			args->Push(m_pPlayer->GetID());
			args->Push(ePlayer);
			args->Push(true);
			args->Push(iOldFriendship);
			args->Push(iNewFriendship);

			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "MinorAlliesChanged", args.get(), bResult);
		}
	}

	else if(eOldAlly == ePlayer && bWasAboveAlliesThreshold && !bNowAboveAlliesThreshold)
	{
		bAdd = false;
		bAllies = true;

		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem)
		{
			CvLuaArgsHandle args;
			args->Push(m_pPlayer->GetID());
			args->Push(ePlayer);
			args->Push(false);
			args->Push(iOldFriendship);
			args->Push(iNewFriendship);

			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "MinorAlliesChanged", args.get(), bResult);
		}

	}
#ifdef NO_AI_ALLYING_CS
	}
	else
	{
	if(eOldAlly == ePlayer && bWasAboveAlliesThreshold && !bNowAboveAlliesThreshold)
	{
		bAdd = false;
		bAllies = true;

		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem)
		{
			CvLuaArgsHandle args;
			args->Push(m_pPlayer->GetID());
			args->Push(ePlayer);
			args->Push(false);
			args->Push(iOldFriendship);
			args->Push(iNewFriendship);

			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "MinorAlliesChanged", args.get(), bResult);
		}
	}
	}
#endif


	if(bFriends || bAllies)
		DoSetBonus(ePlayer, bAdd, bFriends, bAllies,                            bFromQuest);


	if(bAllies)
	{
		if(bAdd)
			SetAlly(ePlayer);
		else
			SetAlly(NO_PLAYER);
	}
}


bool CvMinorCivAI::IsFriendshipAboveFriendsThreshold(int iFriendship) const
{
	int iFriendshipThresholdFriends = GetFriendsThreshold();

	if(iFriendship >= iFriendshipThresholdFriends)
	{
		return true;
	}

	return false;
}


int CvMinorCivAI::GetFriendsThreshold() const
{
	return        GC.getFRIENDSHIP_THRESHOLD_FRIENDS();
}


bool CvMinorCivAI::IsFriendshipAboveAlliesThreshold(int iFriendship) const
{
	int iFriendshipThresholdAllies = GetAlliesThreshold();

	if(iFriendship >= iFriendshipThresholdAllies)
	{
		return true;
	}

	return false;
}


int CvMinorCivAI::GetAlliesThreshold() const
{
	return        GC.getFRIENDSHIP_THRESHOLD_ALLIES();
}


void CvMinorCivAI::DoSetBonus(PlayerTypes ePlayer, bool bAdd, bool bFriends, bool bAllies, bool bSuppressNotifications, bool bPassedBySomeone, PlayerTypes eNewAlly)
{
	MinorCivTraitTypes eTrait = GetTrait();


	if(eTrait == MINOR_CIV_TRAIT_CULTURED)
	{
	}

	else if(eTrait == MINOR_CIV_TRAIT_MILITARISTIC)
	{
		if(bAdd)
		{

			if(GetUnitSpawnCounter(ePlayer) == -1)
				DoSeedUnitSpawnCounter(ePlayer,           true);
		}
	}

	else if(eTrait == MINOR_CIV_TRAIT_MARITIME)
	{
		int iCapitalFoodTimes100 = 0;
		int iOtherCitiesFoodTimes100 = 0;

		if(bFriends)
		{
			iCapitalFoodTimes100 += GetFriendsCapitalFoodBonus(ePlayer);
			iOtherCitiesFoodTimes100 += GetFriendsOtherCityFoodBonus(ePlayer);
		}
		if(bAllies)
		{
			iCapitalFoodTimes100 += GetAlliesCapitalFoodBonus(ePlayer);
			iOtherCitiesFoodTimes100 += GetAlliesOtherCityFoodBonus(ePlayer);
		}

		if(!bAdd)
		{
			iCapitalFoodTimes100 = -iCapitalFoodTimes100;
			iOtherCitiesFoodTimes100 = -iOtherCitiesFoodTimes100;
		}

		GET_PLAYER(ePlayer).ChangeCapitalYieldChange(YIELD_FOOD, iCapitalFoodTimes100);
		GET_PLAYER(ePlayer).ChangeCityYieldChange(YIELD_FOOD, iOtherCitiesFoodTimes100);
	}

	else if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
	{
		GET_PLAYER(ePlayer).DoUpdateHappiness();
	}

	if(eTrait == MINOR_CIV_TRAIT_RELIGIOUS)
	{
	}

	if(ePlayer == GC.getGame().getActivePlayer())
	{
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	}

	CvString strDetailedInfo = GetStatusChangeDetails(ePlayer, bAdd, bFriends, bAllies);

	PlayerTypes eOldAlly = GetAlly();
	TeamTypes eOldAllyTeam = eOldAlly != NO_PLAYER ? GET_PLAYER(eOldAlly).getTeam() : NO_TEAM;


	if(bAdd && bAllies)
	{
		if(eOldAlly != NO_PLAYER && ePlayer != eOldAlly)
		{
			DoSetBonus(eOldAlly,          false,              false,             true,                            false,                      true, ePlayer);
		}
	}





	if (IsDisableNotifications())
	{
		return;
	}

	if (!bSuppressNotifications)
	{
		pair<CvString, CvString> notifStrings = GetStatusChangeNotificationStrings(ePlayer, bAdd, bFriends, bAllies, eOldAlly, (bAdd && bAllies) ? ePlayer : eNewAlly);
		AddNotification(notifStrings.first, notifStrings.second, ePlayer);
	}

#ifdef AUI_CITIZENS_MID_TURN_ASSIGN_RUNS_SELF_CONSISTENCY
	GET_PLAYER(ePlayer).doSelfConsistencyCheckAllCities();
#endif




	Localization::String strMessageOthers;
	Localization::String strSummaryOthers;


	if(bPassedBySomeone)
		ePlayer = eNewAlly;

	for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
		PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
		CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
		CvTeam* pNotifyTeam = &GET_TEAM(kCurNotifyPlayer.getTeam());
		TeamTypes eNewAllyTeam = GET_PLAYER(ePlayer).getTeam();
		const char* strNewBestPlayersNameKey;


		if(pNotifyTeam->isHasMet(eNewAllyTeam))
			strNewBestPlayersNameKey = GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey();

		else
			strNewBestPlayersNameKey = "TXT_KEY_UNMET_PLAYER";

		const char* strOldBestPlayersNameKey = "";


		if(eOldAlly != NO_PLAYER)
		{

			if(pNotifyTeam->isHasMet(eOldAllyTeam))
				strOldBestPlayersNameKey = GET_PLAYER(eOldAlly).getCivilizationShortDescriptionKey();

			else
				strOldBestPlayersNameKey = "TXT_KEY_UNMET_PLAYER";
		}

		const char* strMinorsNameKey = GetPlayer()->getNameKey();
		TeamTypes eMinorTeam = GetPlayer()->getTeam();


		if(bAdd)
		{

			if(bAllies)
			{
				if(ePlayer != eNotifyPlayer)
				{

					if(pNotifyTeam->isHasMet(eMinorTeam))
					{

						if(eOldAlly != NO_PLAYER && eOldAlly != ePlayer)
						{
							strMessageOthers = Localization::Lookup("TXT_KEY_NTFN_MINOR_NEW_BEST_RELATIONS_ALL");
							strMessageOthers << strNewBestPlayersNameKey << strOldBestPlayersNameKey << strMinorsNameKey;
							strSummaryOthers = Localization::Lookup("TXT_KEY_NTFN_SMMRY_MINOR_BEST_RELATIONS_ALL");
							strSummaryOthers << strMinorsNameKey;
						}

						else
						{
							strMessageOthers = Localization::Lookup("TXT_KEY_NTFN_MINOR_NOW_BEST_RELATIONS_ALL");
							strMessageOthers << strNewBestPlayersNameKey << strMinorsNameKey;
							strSummaryOthers = Localization::Lookup("TXT_KEY_NTFN_SMMRY_MINOR_NOW_ALLIES_ALL");
							strSummaryOthers << strMinorsNameKey << strNewBestPlayersNameKey;
						}


						if(eOldAlly != eNotifyPlayer)
							AddNotification(strMessageOthers.toUTF8(), strSummaryOthers.toUTF8(), eNotifyPlayer);
					}
				}
			}
		}

		else
		{

			if(bAllies)
			{
				if(ePlayer != eNotifyPlayer)
				{
					if(pNotifyTeam->isHasMet(eMinorTeam))
					{

						if(!bPassedBySomeone)
						{
							const char* strOldAllyNameKey;


							if(pNotifyTeam->isHasMet(eOldAllyTeam))
								strOldAllyNameKey = GET_PLAYER(eOldAlly).getCivilizationShortDescriptionKey();

							else
								strOldAllyNameKey = "TXT_KEY_UNMET_PLAYER";

							strMessageOthers = Localization::Lookup("TXT_KEY_NTFN_MINOR_BEST_RELATIONS_LOST_ALL");
							strMessageOthers << strOldAllyNameKey << strMinorsNameKey;
							strSummaryOthers = Localization::Lookup("TXT_KEY_NTFN_SMMRY_MINOR_BEST_RELATIONS_LOST_ALL");
							strSummaryOthers << strMinorsNameKey << strOldAllyNameKey;

							AddNotification(strMessageOthers.toUTF8(), strSummaryOthers.toUTF8(), eNotifyPlayer);
						}
					}
				}
			}
		}
	}
}



void CvMinorCivAI::DoIntrusion()
{
	PlayerTypes eMajor;
	int iMajorLoop;
	for(iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;

		if(GetAngerFreeIntrusionCounter(eMajor) > 0)
		{
			ChangeAngerFreeIntrusionCounter(eMajor, -1);
		}
	}


	if(GetNumThreateningBarbarians() > 0)
		return;

	CvPlot* pLoopPlot;
	const IDInfo* pUnitNode;
	const CvUnit* pLoopUnit;


	for(iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;
		SetMajorScratchPad(eMajor, 0);
	}


#ifdef AUI_WARNING_FIXES
	for (uint iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
#else
	for(int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
#endif
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);


		if(pLoopPlot->getOwner() == GetPlayer()->GetID())
		{
			pUnitNode = pLoopPlot->headUnitNode();

			while(pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(*pUnitNode);
				pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);


				if(pLoopUnit && pLoopUnit->IsAngerFreeUnit())
					continue;














				if(pLoopUnit && pLoopUnit->getOwner() < MAX_MAJOR_CIVS)
				{

					if(!IsPlayerHasOpenBorders(pLoopUnit->getOwner()) && GetAngerFreeIntrusionCounter(pLoopUnit->getOwner()) == 0)
					{

						if(!IsAtWarWithPlayersTeam(pLoopUnit->getOwner()))
						{

							if(!GET_PLAYER(pLoopUnit->getOwner()).GetPlayerTraits()->IsAngerFreeIntrusionOfCityStates())
							{
								ChangeFriendshipWithMajor(pLoopUnit->getOwner(),        GC.getFRIENDSHIP_PER_UNIT_INTRUDING());


								if(!pLoopUnit->IsAutomated() && pLoopUnit->GetLengthMissionQueue() == 0)
								{
									SetMajorScratchPad(pLoopUnit->getOwner(), 1);
								}






							}
						}
					}
				}
			}
		}
	}


	Localization::String strMessage;
	Localization::String strSummary;

	for(iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;

		if(GetMajorScratchPad(eMajor) > 0)
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_INTRUSION");
			strMessage << GetPlayer()->getNameKey();
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_INTRUSION");
			strSummary << GetPlayer()->getNameKey();

			AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), eMajor);
		}
	}
}


bool CvMinorCivAI::IsMajorIntruding(PlayerTypes eMajor) const
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor< 0 || eMajor >= MAX_MAJOR_CIVS) return false;

	return m_abMajorIntruding[eMajor];
}


void CvMinorCivAI::SetMajorIntruding(PlayerTypes eMajor, bool bValue)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;

	if(IsMajorIntruding(eMajor) != bValue)
	{
		m_abMajorIntruding[eMajor] = bValue;
	}
}



bool CvMinorCivAI::IsPlayerHasOpenBorders(PlayerTypes ePlayer)
{

	if(IsPlayerHasOpenBordersAutomatically(ePlayer))
		return true;

	return IsFriends(ePlayer);
}


bool CvMinorCivAI::IsPlayerHasOpenBordersAutomatically(PlayerTypes ePlayer)
{

	if(GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateFriendshipModifier() > 0)
		return true;

	return false;
}



void CvMinorCivAI::DoLiberationByMajor(PlayerTypes eLiberator, TeamTypes eConquerorTeam)
{
	Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LIBERATION");
	strMessage << GetPlayer()->getNameKey() << GET_PLAYER(eLiberator).getNameKey();
	Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_LIBERATION");
	strSummary << GetPlayer()->getNameKey();

	int iHighestOtherMajorInfluence = GC.getMINOR_FRIENDSHIP_AT_WAR();

	PlayerTypes ePlayer;
	for(int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		ePlayer = (PlayerTypes) iI;

		if(ePlayer != eLiberator)
		{
			int iInfluence = GetBaseFriendshipWithMajor(ePlayer);
			if(iInfluence > iHighestOtherMajorInfluence)
				iHighestOtherMajorInfluence = iInfluence;

			if(GET_PLAYER(ePlayer).isAlive())
			{
				if(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GetPlayer()->getTeam()))
				{

					if(GET_PLAYER(ePlayer).getTeam() == eConquerorTeam)
					{
						SetFriendshipWithMajor(ePlayer, GC.getMINOR_FRIENDSHIP_AT_WAR());
					}


					CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
					if(pNotifications)
					{
						pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strMessage.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
					}
				}
			}
		}
	}


	int iNewInfluence = max(iHighestOtherMajorInfluence + GC.getMINOR_LIBERATION_FRIENDSHIP(), GetBaseFriendshipWithMajor(eLiberator) + GC.getMINOR_LIBERATION_FRIENDSHIP());
	iNewInfluence = max(GetAlliesThreshold(), iNewInfluence);
	SetFriendshipWithMajor(eLiberator, iNewInfluence);


	strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LIBERATION_YOU");
	strMessage << GetPlayer()->getNameKey();
	strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_LIBERATION");
	strSummary << GetPlayer()->getNameKey();
	AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), eLiberator);
}

void CvMinorCivAI::DoChangeProtectionFromMajor(PlayerTypes eMajor, bool bProtect, bool bPledgeNowBroken)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;

	if(bProtect == IsProtectedByMajor(eMajor)) return;

	if(bProtect)
	{
		if(!CanMajorProtect(eMajor))
		{
			return;
		}
		SetTurnLastPledgedProtectionByMajor(eMajor, GC.getGame().getGameTurn());
	}
	else
	{
		if(bPledgeNowBroken)
		{
			SetTurnLastPledgeBrokenByMajor(eMajor, GC.getGame().getGameTurn());
			ChangeFriendshipWithMajorTimes100(eMajor, GC.getMINOR_FRIENDSHIP_DROP_DISHONOR_PLEDGE_TO_PROTECT());
		}
	}

	m_abPledgeToProtect[eMajor] = bProtect;


	DoTestActiveQuestsForPlayer(eMajor,                   true,                   false, MINOR_CIV_QUEST_PLEDGE_TO_PROTECT);

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
}

bool CvMinorCivAI::CanMajorProtect(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;


	if(GET_TEAM(GET_PLAYER(eMajor).getTeam()).isAtWar(GetPlayer()->getTeam()))
		return false;


	if(GetEffectiveFriendshipWithMajor(eMajor) <       GC.getFRIENDSHIP_THRESHOLD_CAN_PLEDGE_TO_PROTECT())
		return false;


	int iCurrentTurn = GC.getGame().getGameTurn();
	int iLastPledgeBrokenTurn = GetTurnLastPledgeBrokenByMajor(eMajor);
	const int iGracePeriod = 20;
	if(iLastPledgeBrokenTurn >= 0 && iLastPledgeBrokenTurn + iGracePeriod > iCurrentTurn)
		return false;

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	if(pkPersonalityInfo != NULL && pkPersonalityInfo->IsBlocksPledgeToProtect())
	{
		return false;
	}
#endif

	return true;
}

bool CvMinorCivAI::CanMajorStartProtection(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;

	return (CanMajorProtect(eMajor) && !IsProtectedByMajor(eMajor));
}

bool CvMinorCivAI::CanMajorWithdrawProtection(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;

	if (!IsProtectedByMajor(eMajor))
		return false;


	int iCurrentTurn = GC.getGame().getGameTurn();
	int iLastPledgeTurn = GetTurnLastPledgedProtectionByMajor(eMajor);
	const int iGracePeriod = 10;
	if (iLastPledgeTurn >= 0 && iLastPledgeTurn + iGracePeriod > iCurrentTurn)
		return false;

	return true;
}

bool CvMinorCivAI::IsProtectedByMajor(PlayerTypes eMajor) const
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;

	return m_abPledgeToProtect[eMajor];
}

bool CvMinorCivAI::IsProtectedByAnyMajor() const
{
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		if(IsProtectedByMajor((PlayerTypes)iPlayerLoop))
			return true;
	return false;
}

int CvMinorCivAI::GetTurnLastPledgedProtectionByMajor(PlayerTypes eMajor) const
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return -1;

	return m_aiTurnLastPledged[eMajor];
}

void CvMinorCivAI::SetTurnLastPledgedProtectionByMajor(PlayerTypes eMajor, int iTurn)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;

	m_aiTurnLastPledged[eMajor] = iTurn;
}

int CvMinorCivAI::GetTurnLastPledgeBrokenByMajor(PlayerTypes eMajor) const
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return -1;

	return m_aiTurnLastBrokePledge[eMajor];
}

void CvMinorCivAI::SetTurnLastPledgeBrokenByMajor(PlayerTypes eMajor, int iTurn)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;

	m_aiTurnLastBrokePledge[eMajor] = iTurn;
}










bool CvMinorCivAI::DoMajorCivEraChange(PlayerTypes ePlayer, EraTypes eNewEra)
{
	bool bSomethingChanged = false;

	MinorCivTraitTypes eTrait = GetTrait();


	if(eTrait == MINOR_CIV_TRAIT_MARITIME)
	{

		if(IsFriends(ePlayer))
		{
			int iOldFood, iNewFood;


			iOldFood = GetFriendsCapitalFoodBonus(ePlayer);
			iNewFood = GetFriendsCapitalFoodBonus(ePlayer, eNewEra);

			if(iOldFood != iNewFood)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).ChangeCapitalYieldChange(YIELD_FOOD, iNewFood - iOldFood);
			}


			iOldFood = GetFriendsOtherCityFoodBonus(ePlayer);
			iNewFood = GetFriendsOtherCityFoodBonus(ePlayer, eNewEra);

			if(iOldFood != iNewFood)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).ChangeCityYieldChange(YIELD_FOOD, iNewFood - iOldFood);
			}
		}


		if(IsAllies(ePlayer))
		{
			int iOldFood, iNewFood;


			iOldFood = GetAlliesCapitalFoodBonus(ePlayer);
			iNewFood = GetAlliesCapitalFoodBonus(ePlayer);

			if(iOldFood != iNewFood)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).ChangeCapitalYieldChange(YIELD_FOOD, iNewFood - iOldFood);
			}


			iOldFood = GetAlliesOtherCityFoodBonus(ePlayer);
			iNewFood = GetAlliesOtherCityFoodBonus(ePlayer);

			if(iOldFood != iNewFood)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).ChangeCityYieldChange(YIELD_FOOD, iNewFood - iOldFood);
			}
		}
	}


	else if(eTrait == MINOR_CIV_TRAIT_CULTURED)
	{

		if(IsFriends(ePlayer))
		{
			int iOldCulture = GetCultureFlatFriendshipBonus(ePlayer);
			int iNewCulture = GetCultureFlatFriendshipBonus(ePlayer, eNewEra);

			if(iOldCulture != iNewCulture)
			{
				bSomethingChanged = true;
			}
		}


		if(IsAllies(ePlayer))
		{
			int iOldCulture = GetCultureFlatAlliesBonus(ePlayer);
			int iNewCulture = GetCultureFlatAlliesBonus(ePlayer, eNewEra);

			if(iOldCulture != iNewCulture)
			{
				bSomethingChanged = true;
			}
		}
	}


	else if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
	{

		if(IsFriends(ePlayer))
		{
			int iOldHappiness, iNewHappiness;

			iOldHappiness = GetHappinessFlatFriendshipBonus(ePlayer) + GetHappinessPerLuxuryFriendshipBonus(ePlayer);
			iNewHappiness = GetHappinessFlatFriendshipBonus(ePlayer, eNewEra) + GetHappinessPerLuxuryFriendshipBonus(ePlayer, eNewEra);

			if(iOldHappiness != iNewHappiness)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).DoUpdateHappiness();
			}
		}


		if(IsAllies(ePlayer))
		{
			int iOldHappiness, iNewHappiness;

			iOldHappiness = GetHappinessFlatAlliesBonus(ePlayer) + GetHappinessPerLuxuryAlliesBonus(ePlayer);
			iNewHappiness = GetHappinessFlatAlliesBonus(ePlayer, eNewEra) + GetHappinessPerLuxuryAlliesBonus(ePlayer, eNewEra);

			if(iOldHappiness != iNewHappiness)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).DoUpdateHappiness();
			}
		}
	}


	else if(eTrait == MINOR_CIV_TRAIT_RELIGIOUS)
	{

		if(IsFriends(ePlayer))
		{
			int iOldFaith = GetFaithFlatFriendshipBonus(ePlayer);
			int iNewFaith = GetFaithFlatFriendshipBonus(ePlayer, eNewEra);

			if(iOldFaith != iNewFaith)
			{
				bSomethingChanged = true;
			}
		}


		if(IsAllies(ePlayer))
		{
			int iOldFaith = GetFaithFlatAlliesBonus(ePlayer);
			int iNewFaith = GetFaithFlatAlliesBonus(ePlayer, eNewEra);

			if(iOldFaith != iNewFaith)
			{
				bSomethingChanged = true;
			}
		}
	}

#ifdef AUI_CITIZENS_MID_TURN_ASSIGN_RUNS_SELF_CONSISTENCY
	if(bSomethingChanged)
		GET_PLAYER(ePlayer).doSelfConsistencyCheckAllCities();
#endif

	return bSomethingChanged;
}



#if !defined(LEKMOD_FIX_SCHOLASTICISM)

int CvMinorCivAI::GetScienceFriendshipBonus()
{
	int iResult = GetScienceFriendshipBonusTimes100();
	iResult /= 100;

	return iResult;
}
int CvMinorCivAI::GetScienceFriendshipBonusTimes100()
{
	int iResult = GET_PLAYER(m_pPlayer->GetID()).GetScienceTimes100() *        GC.getMINOR_CIV_SCIENCE_BONUS_MULTIPLIER();
	iResult /= 100;

	return iResult;
}

int CvMinorCivAI::GetCurrentScienceFriendshipBonusTimes100(PlayerTypes ePlayer)
{
	if (GET_PLAYER(ePlayer).IsGetsScienceFromPlayer(GetPlayer()->GetID()))
		return GetScienceFriendshipBonusTimes100();

	return 0;
}
#else

int CvMinorCivAI::GetScienceFriendshipBonus(PlayerTypes eMajor)
{
	int iResult = GetScienceFriendshipBonusTimes100(eMajor);
	iResult /= 100;

	return iResult;
}

int CvMinorCivAI::GetScienceFriendshipBonusTimes100(PlayerTypes eMajor, EraTypes eAssumeEra)
{
	int iResult = 0;

	iResult += (GET_PLAYER(m_pPlayer->GetID()).GetScienceTimes100() *                   GC.getMINOR_CIV_SCIENCE_BONUS_MULTIPLIER()) / 100;

	EraTypes eCurrentEra = (eAssumeEra != NO_ERA) ? eAssumeEra : GET_TEAM(GET_PLAYER(eMajor).getTeam()).GetCurrentEra();
	CvPlayerPolicies* pPolicies = GET_PLAYER(eMajor).GetPlayerPolicies();

	if (IsAllies(eMajor))
		iResult += pPolicies->GetMinorAllyYieldBonus(eCurrentEra, YIELD_SCIENCE);
	else if (IsFriends(eMajor))
		iResult += pPolicies->GetMinorFriendYieldBonus(eCurrentEra, YIELD_SCIENCE);

	return iResult;
}

int CvMinorCivAI::GetCurrentScienceFriendshipBonusTimes100(PlayerTypes ePlayer)
{
	if (GET_PLAYER(ePlayer).IsGetsScienceFromPlayer(GetPlayer()->GetID()))
		return GetScienceFriendshipBonusTimes100(ePlayer);

	return 0;
}
#endif


int CvMinorCivAI::GetCultureFlatFriendshipBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{

	int iCultureBonus = 0;

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);


	if(eCurrentEra >= eIndustrial)
	{
		iCultureBonus +=        GC.getFRIENDS_CULTURE_BONUS_AMOUNT_INDUSTRIAL();
	}


	else if(eCurrentEra >= eMedieval)
	{
		iCultureBonus +=       GC.getFRIENDS_CULTURE_BONUS_AMOUNT_MEDIEVAL();
	}


	else
	{
		iCultureBonus +=       GC.getFRIENDS_CULTURE_BONUS_AMOUNT_ANCIENT();
	}

	return iCultureBonus;
}


int CvMinorCivAI::GetCultureFlatAlliesBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{

	int iCultureBonus = 0;

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);


	if(eCurrentEra >= eIndustrial)
	{
		iCultureBonus +=        GC.getALLIES_CULTURE_BONUS_AMOUNT_INDUSTRIAL();
	}


	else if(eCurrentEra >= eMedieval)
	{
		iCultureBonus +=       GC.getALLIES_CULTURE_BONUS_AMOUNT_MEDIEVAL();
	}


	else
	{
		iCultureBonus +=       GC.getALLIES_CULTURE_BONUS_AMOUNT_ANCIENT();
	}

	return iCultureBonus;
}


int CvMinorCivAI::GetCurrentCultureFlatBonus(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_PLAYERS) return 0;


	if(ePlayer >= MAX_MAJOR_CIVS)
		return 0;


	if(GetTrait() != MINOR_CIV_TRAIT_CULTURED)
		return 0;

	int iAmount = 0;

	if(IsAllies(ePlayer))
		iAmount += GetCultureFlatAlliesBonus(ePlayer);

	if(IsFriends(ePlayer))
		iAmount += GetCultureFlatFriendshipBonus(ePlayer);
#if !defined(LEKMOD_FIX_PATRO_FOOD)

	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier(); 
	

	iModifier += GET_PLAYER(ePlayer).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CITY_STATE_BONUS_MODIFIER);
#else
	int iModifier = GET_PLAYER(ePlayer).GetCityStateBonusModifier();
#endif
	if(iModifier > 0)
	{
		iAmount *= (iModifier + 100);
		iAmount /= 100;
	}

	return iAmount;
}

int CvMinorCivAI::GetCurrentCultureBonus(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_PLAYERS) return 0;

	return GetCurrentCultureFlatBonus(ePlayer);
}


int CvMinorCivAI::GetHappinessFlatFriendshipBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);


	if(eCurrentEra >= eIndustrial)
		return GC.getFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_INDUSTRIAL();


	else if(eCurrentEra >= eMedieval)
		return GC.getFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_MEDIEVAL();


	else
		return GC.getFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_ANCIENT();
}


int CvMinorCivAI::GetHappinessFlatAlliesBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);


	if(eCurrentEra >= eIndustrial)
		return GC.getALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_INDUSTRIAL();


	else if(eCurrentEra >= eMedieval)
		return GC.getALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_MEDIEVAL();


	else
		return GC.getALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_ANCIENT();
}


int CvMinorCivAI::GetCurrentHappinessFlatBonus(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_PLAYERS) return 0;


	if(ePlayer >= MAX_MAJOR_CIVS)
		return 0;


	if(GetTrait() != MINOR_CIV_TRAIT_MERCANTILE)
		return 0;

	int iAmount = 0;
	
	if(IsAllies(ePlayer))
		iAmount += GetHappinessFlatAlliesBonus(ePlayer);
	if(IsFriends(ePlayer))
		iAmount += GetHappinessFlatFriendshipBonus(ePlayer);
#ifdef LEKMOD_SIAM_TRAIT_HAPPINESS
	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();
	if (iModifier > 0)
	{
		iAmount *= (iModifier + 100);
		iAmount /= 100;
	}
#endif
	return iAmount;
}


int CvMinorCivAI::GetHappinessPerLuxuryFriendshipBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{

	int iNumLuxuries = 0;
	ResourceTypes eResource;
#ifdef AUI_WARNING_FIXES
	for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
	{
		eResource = (ResourceTypes) iResourceLoop;

		if(GET_PLAYER(ePlayer).getNumResourceAvailable(eResource) > 0)
		{
			const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			if(pkResourceInfo != NULL && pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
			{
				iNumLuxuries++;
			}
		}
	}

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);


	if(eCurrentEra >= eIndustrial)
		return (iNumLuxuries * GC.getFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_INDUSTRIAL());


	else if(eCurrentEra >= eMedieval)
		return (iNumLuxuries * GC.getFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_MEDIEVAL());


	else
		return (iNumLuxuries * GC.getFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_ANCIENT());
}


int CvMinorCivAI::GetHappinessPerLuxuryAlliesBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{

	int iNumLuxuries = 0;
	ResourceTypes eResource;
#ifdef AUI_WARNING_FIXES
	for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
	{
		eResource = (ResourceTypes) iResourceLoop;

		if(GET_PLAYER(ePlayer).getNumResourceAvailable(eResource) > 0)
		{
			const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			if(pkResourceInfo != NULL && pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
			{
				iNumLuxuries++;
			}
		}
	}

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);


	if(eCurrentEra >= eIndustrial)
		return (iNumLuxuries * GC.getALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_INDUSTRIAL());


	else if(eCurrentEra >= eMedieval)
		return (iNumLuxuries * GC.getALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_MEDIEVAL());


	else
		return (iNumLuxuries * GC.getALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_ANCIENT());
}


int CvMinorCivAI::GetCurrentHappinessPerLuxuryBonus(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_PLAYERS) return 0;


	if(ePlayer >= MAX_MAJOR_CIVS)
		return 0;


	if(GetTrait() != MINOR_CIV_TRAIT_MERCANTILE)
		return 0;

	int iAmount = 0;
	if(IsAllies(ePlayer))
		iAmount += GetHappinessPerLuxuryAlliesBonus(ePlayer);
	if(IsFriends(ePlayer))
		iAmount += GetHappinessPerLuxuryFriendshipBonus(ePlayer);

#ifdef LEKMOD_SIAM_TRAIT_HAPPINESS
	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();
	if (iModifier > 0)
	{
		iAmount *= (iModifier + 100);
		iAmount /= 100;
	}
#endif
	return iAmount;
}


int CvMinorCivAI::GetCurrentHappinessBonus(PlayerTypes ePlayer)
{
	int iValue = 0;

	iValue += GetCurrentHappinessFlatBonus(ePlayer);
	iValue += GetCurrentHappinessPerLuxuryBonus(ePlayer);

	return iValue;
}

int CvMinorCivAI::GetFaithFlatFriendshipBonus(PlayerTypes ePlayer, EraTypes eAssumeEra) const
{
	int iFaithBonus = 0;

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);
	EraTypes eClassical = (EraTypes) GC.getInfoTypeForString("ERA_CLASSICAL", true);


	if(eCurrentEra >= eIndustrial)
	{
		iFaithBonus += GC.getFRIENDS_FAITH_FLAT_BONUS_AMOUNT_INDUSTRIAL();
	}


	else if(eCurrentEra >= eRenaissance)
	{
		iFaithBonus += GC.getFRIENDS_FAITH_FLAT_BONUS_AMOUNT_RENAISSANCE();
	}


	else if(eCurrentEra >= eMedieval)
	{
		iFaithBonus += GC.getFRIENDS_FAITH_FLAT_BONUS_AMOUNT_MEDIEVAL();
	}


	else if(eCurrentEra >= eClassical)
	{
		iFaithBonus += GC.getFRIENDS_FAITH_FLAT_BONUS_AMOUNT_CLASSICAL();
	}


	else
	{
		iFaithBonus += GC.getFRIENDS_FAITH_FLAT_BONUS_AMOUNT_ANCIENT();
	}

	return iFaithBonus;
}

int CvMinorCivAI::GetFaithFlatAlliesBonus(PlayerTypes ePlayer, EraTypes eAssumeEra) const
{
	int iFaithBonus = 0;

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);
	EraTypes eClassical = (EraTypes) GC.getInfoTypeForString("ERA_CLASSICAL", true);


	if(eCurrentEra >= eIndustrial)
	{
		iFaithBonus += GC.getALLIES_FAITH_FLAT_BONUS_AMOUNT_INDUSTRIAL();
	}


	else if(eCurrentEra >= eRenaissance)
	{
		iFaithBonus += GC.getALLIES_FAITH_FLAT_BONUS_AMOUNT_RENAISSANCE();
	}


	else if(eCurrentEra >= eMedieval)
	{
		iFaithBonus += GC.getALLIES_FAITH_FLAT_BONUS_AMOUNT_MEDIEVAL();
	}


	else if(eCurrentEra >= eClassical)
	{
		iFaithBonus += GC.getALLIES_FAITH_FLAT_BONUS_AMOUNT_CLASSICAL();
	}


	else
	{
		iFaithBonus += GC.getALLIES_FAITH_FLAT_BONUS_AMOUNT_ANCIENT();
	}

	return iFaithBonus;
}

int CvMinorCivAI::GetCurrentFaithFlatBonus(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_PLAYERS) return 0;


	if(ePlayer >= MAX_MAJOR_CIVS)
		return 0;


	if(GetTrait() != MINOR_CIV_TRAIT_RELIGIOUS)
		return 0;

	int iAmount = 0;
	if(IsAllies(ePlayer))
		iAmount += GetFaithFlatAlliesBonus(ePlayer);
	if(IsFriends(ePlayer))
		iAmount += GetFaithFlatFriendshipBonus(ePlayer);

#if !defined(LEKMOD_FIX_PATRO_FOOD)

	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();


	iModifier += GET_PLAYER(ePlayer).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CITY_STATE_BONUS_MODIFIER);
#else
	int iModifier = GET_PLAYER(ePlayer).GetCityStateBonusModifier();
#endif

	if(iModifier > 0)
	{
		iAmount *= (iModifier + 100);
		iAmount /= 100;
	}

	return iAmount;
}


int CvMinorCivAI::GetCurrentFaithBonus(PlayerTypes ePlayer)
{
	int iValue = 0;

	iValue += GetCurrentFaithFlatBonus(ePlayer);

	return iValue;
}


int CvMinorCivAI::GetFriendsCapitalFoodBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	int iBonus;

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);


	if(eCurrentEra < eRenaissance)
		iBonus =         GC.getFRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE();


	else
		iBonus =         GC.getFRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_POST_RENAISSANCE();

#if !defined(LEKMOD_FIX_PATRO_FOOD)

	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();


	iModifier += GET_PLAYER(ePlayer).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CITY_STATE_BONUS_MODIFIER);
#else
	int iModifier = GET_PLAYER(ePlayer).GetCityStateBonusModifier();
#endif

	if(iModifier > 0)
	{
		iBonus *= (iModifier + 100);
		iBonus /= 100;
	}

	return iBonus;
}


int CvMinorCivAI::GetFriendsOtherCityFoodBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	int iBonus;

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);


	if(eCurrentEra < eRenaissance)
		iBonus =       GC.getFRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE();


	else
		iBonus =       GC.getFRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_POST_RENAISSANCE();

#if !defined(LEKMOD_FIX_PATRO_FOOD)

	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();


	iModifier += GET_PLAYER(ePlayer).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CITY_STATE_BONUS_MODIFIER);
#else
	int iModifier = GET_PLAYER(ePlayer).GetCityStateBonusModifier();
#endif

	if(iModifier > 0)
	{
		iBonus *= (iModifier + 100);
		iBonus /= 100;
	}

	return iBonus;
}


int CvMinorCivAI::GetAlliesCapitalFoodBonus(PlayerTypes ePlayer)
{
	int iBonus =         GC.getALLIES_CAPITAL_FOOD_BONUS_AMOUNT();

#if !defined(LEKMOD_FIX_PATRO_FOOD)

	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();


	iModifier += GET_PLAYER(ePlayer).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CITY_STATE_BONUS_MODIFIER);
#else
	int iModifier = GET_PLAYER(ePlayer).GetCityStateBonusModifier();
#endif

	if(iModifier > 0)
	{
		iBonus *= (iModifier + 100);
		iBonus /= 100;
	}

	return iBonus;
}


int CvMinorCivAI::GetAlliesOtherCityFoodBonus(PlayerTypes ePlayer)
{
	int iBonus =         GC.getALLIES_OTHER_CITIES_FOOD_BONUS_AMOUNT();

#if !defined(LEKMOD_FIX_PATRO_FOOD)

	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();


	iModifier += GET_PLAYER(ePlayer).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CITY_STATE_BONUS_MODIFIER);
#else
	int iModifier = GET_PLAYER(ePlayer).GetCityStateBonusModifier();
#endif

	if(iModifier > 0)
	{
		iBonus *= (iModifier + 100);
		iBonus /= 100;
	}

	return iBonus;
}


int CvMinorCivAI::GetCurrentCapitalFoodBonus(PlayerTypes ePlayer)
{

	if(GetTrait() != MINOR_CIV_TRAIT_MARITIME)
		return 0;

	int iAmount = 0;

	if(IsAllies(ePlayer))
	{
		iAmount += GetAlliesCapitalFoodBonus(ePlayer);
		iAmount += GetAlliesOtherCityFoodBonus(ePlayer);
	}

	if(IsFriends(ePlayer))
	{
		iAmount += GetFriendsCapitalFoodBonus(ePlayer);
		iAmount += GetFriendsOtherCityFoodBonus(ePlayer);
	}

	return iAmount;
}


int CvMinorCivAI::GetCurrentOtherCityFoodBonus(PlayerTypes ePlayer)
{

	if(GetTrait() != MINOR_CIV_TRAIT_MARITIME)
		return 0;

	int iAmount = 0;

	if(IsAllies(ePlayer))
		iAmount += GetAlliesOtherCityFoodBonus(ePlayer);

	if(IsFriends(ePlayer))
		iAmount += GetFriendsOtherCityFoodBonus(ePlayer);

	return iAmount;
}


void CvMinorCivAI::DoSeedUnitSpawnCounter(PlayerTypes ePlayer, bool bBias)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	int iNumTurns = GetSpawnBaseTurns(ePlayer);


	int iRand =       GC.getFRIENDS_RAND_TURNS_UNIT_SPAWN();
	iNumTurns += GC.getGame().getJonRandNum(iRand, "Rand turns for Friendly Minor unit spawn");


	if(bBias)
	{
		iNumTurns *=        GC.getUNIT_SPAWN_BIAS_MULTIPLIER();
		iNumTurns /= 100;
	}

	SetUnitSpawnCounter(ePlayer, iNumTurns);
}


int CvMinorCivAI::GetUnitSpawnCounter(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1;

	return m_aiUnitSpawnCounter[ePlayer];
}


void CvMinorCivAI::SetUnitSpawnCounter(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_aiUnitSpawnCounter[ePlayer] = iValue;
}


void CvMinorCivAI::ChangeUnitSpawnCounter(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	SetUnitSpawnCounter(ePlayer, GetUnitSpawnCounter(ePlayer) + iChange);
}


bool CvMinorCivAI::IsUnitSpawningAllowed(PlayerTypes ePlayer)
{

	if(!IsHasMetPlayer(ePlayer))
		return false;


	if(GetTrait() != MINOR_CIV_TRAIT_MILITARISTIC)
		return false;


	if(IsAtWarWithPlayersTeam(ePlayer))
		return false;


	if(!IsFriends(ePlayer))
		return false;


	if(!GetPlayer()->isAlive())
		return false;


	if(!GET_PLAYER(ePlayer).isAlive())
		return false;

	return true;
}


bool CvMinorCivAI::IsUnitSpawningDisabled(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	return m_abUnitSpawningDisabled[ePlayer];
}


void CvMinorCivAI::SetUnitSpawningDisabled(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_abUnitSpawningDisabled[ePlayer] = bValue;
}


void CvMinorCivAI::DoSpawnUnit(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;

	if(!IsUnitSpawningDisabled(eMajor))
	{

		CvCity* pMinorCapital = GetPlayer()->getCapitalCity();
		if(pMinorCapital == NULL)
		{
			FAssertMsg(false, "MINOR CIV AI: Trying to spawn a Unit for a major civ but the minor has no capital. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
			return;
		}
		CvPlot* pMinorCapitalPlot = pMinorCapital->plot();
		if(pMinorCapitalPlot == NULL)
		{
			CvAssertMsg(false, "MINOR CIV AI: Trying to spawn a Unit for a major civ but the minor's capital has no plot. Please send Anton your save file and version.");
			return;
		}

		CvCity* pMajorCity = GET_PLAYER(eMajor).GetClosestFriendlyCity(*pMinorCapitalPlot, MAX_INT);

		int iX = pMinorCapital->getX();
		int iY = pMinorCapital->getY();
		if(pMajorCity != NULL)
		{
			iX = pMajorCity->getX();
			iY = pMajorCity->getY();
		}


		UnitTypes eUnit = NO_UNIT;
#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
		const bool bSpawnNavalUnits = (GetPersonalityInfo() != NULL && GetPersonalityInfo()->IsSpawnNavalUnits() && DoesMajorHaveOceanCoastalCity(eMajor));
#else
		const bool bSpawnNavalUnits = false;
#endif
		if (GetAlly() == eMajor)
		{	

			bool bUseUniqueUnit = false;
			UnitTypes eUniqueUnit = GetUniqueUnit();
			if (eUniqueUnit != NO_UNIT)
			{
				CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUniqueUnit);
				if (pkUnitInfo)
				{

					TechTypes ePrereqTech = (TechTypes) pkUnitInfo->GetPrereqAndTech();
					if (ePrereqTech == NO_TECH || GET_TEAM(GET_PLAYER(eMajor).getTeam()).GetTeamTechs()->HasTech(ePrereqTech))
					{

						TechTypes eObsoleteTech = (TechTypes) pkUnitInfo->GetObsoleteTech();
						if (eObsoleteTech == NO_TECH || !GET_TEAM(GET_PLAYER(eMajor).getTeam()).GetTeamTechs()->HasTech(eObsoleteTech))
						{
							bUseUniqueUnit = true;
						}
					}
				}
			}
			
			if (bUseUniqueUnit)
			{
				if(bSpawnNavalUnits)
				{
					CvUnitEntry* pkUniqueUnitInfo = GC.getUnitInfo(eUniqueUnit);
					if(pkUniqueUnitInfo && pkUniqueUnitInfo->GetDomainType() == DOMAIN_SEA)
					{
						eUnit = eUniqueUnit;
					}
					else
					{
						eUnit = GetCompetitiveSpawnNavalUnitType(eMajor,                 false,                    true);
					}
				}
				else
				{
					eUnit = eUniqueUnit;
				}
			}
			else
			{
				if(bSpawnNavalUnits)
				{
					eUnit = GetCompetitiveSpawnNavalUnitType(eMajor,                 false,                    true);
				}
				else
				{
					eUnit = GC.getGame().GetCompetitiveSpawnUnitType(eMajor,                 false,                   true);
				}
			}
		}
		else
		{
			if(bSpawnNavalUnits)
			{
				eUnit = GetCompetitiveSpawnNavalUnitType(eMajor,                 false,                    true);
			}
			else
			{
				eUnit = GC.getGame().GetCompetitiveSpawnUnitType(eMajor,                 false,                   true);
			}
		}


		if(eUnit != NO_UNIT)
		{

			int iUnitsToGift = 1 + GET_PLAYER(eMajor).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_MINOR_MILITARY_NUM_EXTRA_UNITS_TO_GIFT);
			for (int i = 0; i < iUnitsToGift; ++i)
			{

				CvUnit* pNewUnit = GET_PLAYER(eMajor).initUnit(eUnit, iX, iY);

#if defined(v35_TRAITIFY)
				pNewUnit->changeExperience(GET_PLAYER(eMajor).GetPlayerTraits()->GetCityStateUnitGiftExtraExperience());
#else

				if (GET_PLAYER(eMajor).GetPlayerTraits()->GetCityStateBonusModifier() > 0)
				{
					pNewUnit->changeExperience(GC.getMAX_EXPERIENCE_PER_COMBAT());
				}
#endif
				

				if (pNewUnit->jumpToNearestValidPlot())
				{
#if defined(LEKMOD_CITYSTATE_QUEST_CHANGES)
					CvCity* pMinorXpCity = GetPlayer()->getCapitalCity();
					int iMinorXP = (pMinorXpCity != NULL) ? pMinorXpCity->getProductionExperience(pNewUnit->getUnitType()) : 0;

					CvCity* pMajorXpCity = pMajorCity;
					int iMajorXP = (pMajorXpCity != NULL) ? pMajorXpCity->getProductionExperience(pNewUnit->getUnitType()) : 0;

					if(pMajorXpCity != NULL && iMajorXP > iMinorXP)
					{
						pMajorXpCity->addProductionExperience(pNewUnit);
					}
					else if(pMinorXpCity != NULL)
					{
						pMinorXpCity->addProductionExperience(pNewUnit);
					}
#else
					if(GetPlayer()->getCapitalCity())
						GetPlayer()->getCapitalCity()->addProductionExperience(pNewUnit);
#endif

					Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_STATE_UNIT_SPAWN");
					strMessage << GetPlayer()->getNameKey();
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_STATE_UNIT_SPAWN");
					strSummary << GetPlayer()->getNameKey();

					AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), eMajor, pNewUnit->getX(), pNewUnit->getY());
				}
				else
				{
					pNewUnit->kill(false);
				}
			}
		}
	}


	DoSeedUnitSpawnCounter(eMajor);
}


void CvMinorCivAI::DoUnitSpawnTurn()
{

	PlayerTypes eMajor;
	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;

		if(IsUnitSpawningAllowed(eMajor))
		{

			if(GetUnitSpawnCounter(eMajor) > 0)
			{
				ChangeUnitSpawnCounter(eMajor, -1);
			}


			if(GetUnitSpawnCounter(eMajor) == 0)
			{
				DoSpawnUnit(eMajor);
			}
		}
	}
}


int CvMinorCivAI::GetSpawnBaseTurns(PlayerTypes ePlayer)
{

	if(!IsFriends(ePlayer))
		return 0;


	if(GetTrait() != MINOR_CIV_TRAIT_MILITARISTIC)
		return 0;

	int iNumTurns =        GC.getFRIENDS_BASE_TURNS_UNIT_SPAWN() * 100;
	

	if(IsAllies(ePlayer))
		iNumTurns +=        (GC.getALLIES_EXTRA_TURNS_UNIT_SPAWN() * 100);


	iNumTurns *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iNumTurns /= 100;


	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	int iPolicyMod = kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_UNIT_FREQUENCY_MODIFIER);
	if(iPolicyMod > 0)
	{
		if(GET_TEAM(kPlayer.getTeam()).HasCommonEnemy(m_pPlayer->getTeam()))
		{
			iNumTurns *= 100;
			iNumTurns /= (100 + iPolicyMod);
		}
	}

#ifdef LEKMOD_SIAM_TRAIT_MILITARY_UNIT
	int iModifier = kPlayer.GetPlayerTraits()->GetCityStateBonusModifier();
	
	if (iModifier != 0)
	{
		iNumTurns *= 100;
		iNumTurns /= (100 + iModifier);
	}

#endif

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	if(pkPersonalityInfo && pkPersonalityInfo->GetUnitSpawnModifierPercent() != 100)
	{
		iNumTurns *= pkPersonalityInfo->GetUnitSpawnModifierPercent();
		iNumTurns /= 100;
	}
#endif

	return iNumTurns / 100;
	
}


int CvMinorCivAI::GetCurrentSpawnEstimate(PlayerTypes ePlayer)
{

	if(!IsFriends(ePlayer))
		return 0;


	if(GetTrait() != MINOR_CIV_TRAIT_MILITARISTIC)
		return 0;

	int iNumTurns = GetSpawnBaseTurns(ePlayer) * 100;

	int iRand =       GC.getFRIENDS_RAND_TURNS_UNIT_SPAWN() * 100;
	iNumTurns += (iRand / 2);

	return iNumTurns / 100;
}


bool CvMinorCivAI::IsBoughtOut() const
{
	PlayerTypes eBuyoutPlayer = GetMajorBoughtOutBy();
	return (eBuyoutPlayer != NO_PLAYER);
}


PlayerTypes CvMinorCivAI::GetMajorBoughtOutBy() const
{
	return m_eMajorBoughtOutBy;
}


void CvMinorCivAI::SetMajorBoughtOutBy(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");

	m_eMajorBoughtOutBy = eMajor;
}


bool CvMinorCivAI::CanMajorBuyout(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;


	if (!GET_PLAYER(eMajor).isAlive() || !GetPlayer()->isAlive())
		return false;
	

	if(!GET_PLAYER(eMajor).IsAbleToAnnexCityStates())
		return false;


	if(GET_TEAM(GetPlayer()->getTeam()).isAtWar(GET_PLAYER(eMajor).getTeam()))
		return false;


	if(!IsAllies(eMajor))
		return false;


	if (GetAlliedTurns() < GC.getMINOR_CIV_BUYOUT_TURNS())
	{
		return false;
	}


	const int iBuyoutCost = GetBuyoutCost(eMajor);
	if(GET_PLAYER(eMajor).GetTreasury()->GetGold() < iBuyoutCost)
		return false;

	return true;
}

int CvMinorCivAI::GetBuyoutCost(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return -1;

	int iGold = GC.getMINOR_CIV_BUYOUT_COST();


	iGold *= GC.getGame().getGameSpeedInfo().getGoldPercent();
	iGold /= 100;


	int iScrapCost = 0;
	int iUnitLoop;
	CvUnit *pLoopUnit;
	for (pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
	{
		iScrapCost += pLoopUnit->GetScrapGold();
	}
	iGold += iScrapCost;


	int iVisibleDivisor =       GC.getMINOR_CIV_GOLD_GIFT_VISIBLE_DIVISOR();
	iGold /= iVisibleDivisor;
	iGold *= iVisibleDivisor;

	return iGold;
}

void CvMinorCivAI::DoBuyout(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;

	if(!CanMajorBuyout(eMajor))
		return;
	


	MinorCivTypes eBornu =(MinorCivTypes) GC.getInfoTypeForString("MINOR_CIV_BORNU", true                );
	MinorCivTypes  eSokoto =(MinorCivTypes) GC.getInfoTypeForString("MINOR_CIV_SOKOTO", true                );
	bool bUsingXP2Scenario2 = gDLL->IsModActivated(CIV5_XP2_SCENARIO2_MODID);

	if (GET_PLAYER(eMajor).isHuman() && bUsingXP2Scenario2 && (GetPlayer()->GetMinorCivAI()->GetMinorCivType() == eBornu || GetPlayer()->GetMinorCivAI()->GetMinorCivType() == eSokoto ))
		gDLL->UnlockAchievement(ACHIEVEMENT_XP2_54);



	const int iBuyoutCost = GetBuyoutCost(eMajor);
	GET_PLAYER(eMajor).GetTreasury()->LogExpenditure(GetPlayer()->GetMinorCivAI()->GetNamesListAsString(0), iBuyoutCost,6);
	GET_PLAYER(eMajor).GetTreasury()->ChangeGold(-iBuyoutCost);

	int iNumUnits = 0;
	int iCapitalX = 0;
	int iCapitalY = 0;
	DoAcquire(eMajor, iNumUnits, iCapitalX, iCapitalY);

	GET_PLAYER(eMajor).GetDiplomacyAI()->LogMinorCivBuyout(GetPlayer()->GetID(), iBuyoutCost,             false);


	int iCoinToss = GC.getGame().getJonRandNum(2, "Coin toss roll to determine flavor message for minor civ buyout notification.");
	Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BUYOUT_TT_1");
	if (iCoinToss == 0)
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BUYOUT_TT_2");
	strMessage << GET_PLAYER(eMajor).getCivilizationShortDescriptionKey();
	strMessage << GetPlayer()->getCivilizationShortDescriptionKey();

	Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BUYOUT");
	strSummary << GET_PLAYER(eMajor).getCivilizationShortDescriptionKey();
	strSummary << GetPlayer()->getCivilizationShortDescriptionKey();

	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		PlayerTypes eMajorLoop = (PlayerTypes) iMajorLoop;
		if (IsHasMetPlayer(eMajorLoop))
		{
			AddBuyoutNotification(strMessage.toUTF8(), strSummary.toUTF8(), eMajorLoop, iCapitalX, iCapitalY);
		}
	}

	

	CvPlayerAI& kMajorPlayer = GET_PLAYER(eMajor);
	kMajorPlayer.GetPlayerAchievements().BoughtCityState(iNumUnits);
}

void CvMinorCivAI::DoAcquire(PlayerTypes eMajor, int &iNumUnits, int& iCapitalX, int& iCapitalY)
{

	CvUnit* pLoopUnit = NULL;
	int iLoopUnit;
	iNumUnits = 0;
	for(pLoopUnit = GetPlayer()->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = GetPlayer()->nextUnit(&iLoopUnit))
	{
		GET_PLAYER(eMajor).DoDistanceGift(GetPlayer()->GetID(), pLoopUnit);
		iNumUnits++;
	}


	SetDisableNotifications(true);
	iCapitalX = -1;
	iCapitalY = -1;
	FStaticVector<CvCity*, 16, false, c_eCiv5GameplayDLL, 0> vpCitiesToAcquire;
	int iLoopCity;
	for (CvCity* pLoopCity = GetPlayer()->firstCity(&iLoopCity, true); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iLoopCity, true))
	{
		vpCitiesToAcquire.push_back(pLoopCity);
	}
	for (uint iI = 0; iI < vpCitiesToAcquire.size(); iI++)
	{
		CvCity* pCity = vpCitiesToAcquire[iI];
		CvAssertMsg(pCity, "pCity should not be NULL. Please send Anton your save file and version.");
		if (pCity)
		{
			if (pCity->isCapital())
			{
				iCapitalX = pCity->getX();
				iCapitalY = pCity->getY();
			}
#if !defined(LEKMOD_MERCHANT_BUYOUT_NOT_NOANNEXING)
			GET_PLAYER(eMajor).acquireCity(pCity, false, true);
#else
			GET_PLAYER(eMajor).acquireCity(pCity, false              , true          , true               );
#endif
		}
	}
	SetDisableNotifications(false);

	SetMajorBoughtOutBy(eMajor);

	GC.getGame().DoUpdateDiploVictory();

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);

}





int CvMinorCivAI::GetBullyGoldAmount(PlayerTypes                 )
{
	int iGold = GC.getMINOR_BULLY_GOLD();
#if !defined(MISC_CHANGES)
	int iGoldGrowthFactor = 350;
#else

	int iGoldGrowthFactor = GC.getBULLY_GOLD_GROWTH_FACTOR();
#endif


	float fGameProgressFactor = ((float) GC.getGame().getElapsedGameTurns() / (float) GC.getGame().getEstimateEndTurn());
	CvAssertMsg(fGameProgressFactor >= 0.0f, "fGameProgressFactor is not expected to be negative! Please send Anton your save file and version.");
	if(fGameProgressFactor > 1.0f)
		fGameProgressFactor = 1.0f;

	iGold += (int)(fGameProgressFactor * iGoldGrowthFactor);




	iGold *= GC.getGame().getGameSpeedInfo().getGoldGiftMod();
	iGold /= 100;


	int iVisibleDivisor =       GC.getMINOR_CIV_GOLD_GIFT_VISIBLE_DIVISOR();
	iGold /= iVisibleDivisor;
	iGold *= iVisibleDivisor;

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	if(pkPersonalityInfo && pkPersonalityInfo->GetTributeGoldModifierPercent() != 100)
	{
		iGold *= pkPersonalityInfo->GetTributeGoldModifierPercent();
		iGold /= 100;

		iGold /= iVisibleDivisor;
		iGold *= iVisibleDivisor;
	}
#endif

	return iGold;
}


#ifdef ENHANCED_GRAPHS

int CvMinorCivAI::GetBullyGoldAmountTotalByPlayer(PlayerTypes eBullyPlayer)
{
	return m_aiBullyGoldAmountTotalByPlayer[eBullyPlayer];
}

void CvMinorCivAI::ChangeBullyGoldAmountTotalByPlayer(PlayerTypes eBullyPlayer, int iChange)
{
	m_aiBullyGoldAmountTotalByPlayer[eBullyPlayer] = m_aiBullyGoldAmountTotalByPlayer[eBullyPlayer] + iChange;
}
int CvMinorCivAI::GetBullyWorkersAmountTotalByPlayer(PlayerTypes eBullyPlayer)
{
	return m_aiBullyWorkersAmountTotalByPlayer[eBullyPlayer];
}

void CvMinorCivAI::ChangeBullyWorkersAmountTotalByPlayer(PlayerTypes eBullyPlayer, int iChange)
{
	m_aiBullyWorkersAmountTotalByPlayer[eBullyPlayer] = m_aiBullyWorkersAmountTotalByPlayer[eBullyPlayer] + iChange;
}
#endif




int CvMinorCivAI::CalculateBullyMetric(PlayerTypes eBullyPlayer, bool bForUnit, CvString* sTooltipSink)
{
	CvString sFactors = "";

	int iScore = 0;
	const int iFailScore = -300;

	CvAssertMsg(GetPlayer()->GetID() != eBullyPlayer, "Minor civ and bully civ not expected to have the same ID!");
	if(GetPlayer()->GetID() == eBullyPlayer)
		return iFailScore;

	CvAssertMsg(eBullyPlayer >= 0, "eBullyPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBullyPlayer < MAX_MAJOR_CIVS, "eBullyPlayer is expected to be within maximum bounds (invalid Index)");
	if(eBullyPlayer < 0 || eBullyPlayer >= MAX_MAJOR_CIVS)
		return iFailScore;


	if(!GetPlayer()->isAlive())
		return iFailScore;






	CvWeightedVector<PlayerTypes, MAX_MAJOR_CIVS, true> veMilitaryRankings;
	PlayerTypes eMajorLoop;
	int iGlobalMilitaryScore = 0;
	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajorLoop = (PlayerTypes) iMajorLoop;
		if(GET_PLAYER(eMajorLoop).isAlive() && !GET_PLAYER(eMajorLoop).isMinorCiv())
		{
			veMilitaryRankings.push_back(eMajorLoop, GET_PLAYER(eMajorLoop).GetMilitaryMight());
		}
	}
	CvAssertMsg(veMilitaryRankings.size() > 0, "WeightedVector of military might rankings not expected to be size 0");
	veMilitaryRankings.SortItems();
	for(int iRanking = 0; iRanking < veMilitaryRankings.size(); iRanking++)
	{
		if(veMilitaryRankings.GetElement(iRanking) == eBullyPlayer)
		{
			float fRankRatio = (float)(veMilitaryRankings.size() - iRanking) / (float)(veMilitaryRankings.size());
#ifdef NQ_TRIBUTE_EASIER_WITH_LOCAL_POWER
			iGlobalMilitaryScore = (int)(fRankRatio * 50);
#else
			iGlobalMilitaryScore = (int)(fRankRatio * 75);
#endif
			iScore += iGlobalMilitaryScore;
			break;
		}
	}
	
	if (sTooltipSink)
	{
		Localization::String strPositiveFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_POSITIVE");
		strPositiveFactor << iGlobalMilitaryScore;
		strPositiveFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_GLOBAL_MILITARY";
		sFactors += strPositiveFactor.toUTF8();
	}






#ifdef AUI_WARNING_FIXES
	int iComparisonRadius = MAX(GC.getMap().getGridWidth() / 10, uint(5));
#else
	int iComparisonRadius = std::max(GC.getMap().getGridWidth() / 10, 5);
#endif
	CvCity* pMinorCapital = GetPlayer()->getCapitalCity();
	if(pMinorCapital == NULL)
		return iFailScore;
	CvPlot* pMinorCapitalPlot = pMinorCapital->plot();
	if(pMinorCapitalPlot == NULL)
		return iFailScore;
	int iMinorCapitalX = pMinorCapitalPlot->getX();
	int iMinorCapitalY = pMinorCapitalPlot->getY();
	int iMinorLocalPower = 0;
	int iBullyLocalPower = 0;
	CvPlot* pLoopPlot;
	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;


	iMinorLocalPower += pMinorCapital->GetPower();

#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX, iDX;
	for (int iDY = -iComparisonRadius; iDY <= iComparisonRadius; iDY++)
	{
		iMaxDX = iComparisonRadius - MAX(0, iDY);
		for (iDX = -iComparisonRadius - MIN(0, iDY); iDX <= iMaxDX; iDX++)
		{

			pLoopPlot = plotXY(iMinorCapitalX, iMinorCapitalY, iDX, iDY);
#else
	for(int iDX = -iComparisonRadius; iDX <= iComparisonRadius; iDX++)
	{
		for(int iDY = -iComparisonRadius; iDY <= iComparisonRadius; iDY++)
		{
			pLoopPlot = ::plotXYWithRangeCheck(iMinorCapitalX, iMinorCapitalY, iDX, iDY, iComparisonRadius);
#endif

			if(pLoopPlot != NULL)
			{

				if(pLoopPlot->getNumUnits() > 0)
				{
					pUnitNode = pLoopPlot->headUnitNode();

					while(pUnitNode != NULL)
					{
						pLoopUnit = ::getUnit(*pUnitNode);
						pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);


						if(pLoopUnit && pLoopUnit->IsCombatUnit())
						{
							if(pLoopUnit->getOwner() == eBullyPlayer)
							{
								iBullyLocalPower += pLoopUnit->GetPower();
							}
							else if(pLoopUnit->getOwner() == GetPlayer()->GetID())
							{
								iMinorLocalPower += pLoopUnit->GetPower();
							}
						}
					}
				}

			}
		}
	}
	float fLocalPowerRatio = (float)iBullyLocalPower / (float)iMinorLocalPower;
	int iLocalPowerScore = 0;
	if(fLocalPowerRatio >= 3.0)
	{
#ifdef NQ_TRIBUTE_EASIER_WITH_LOCAL_POWER
		iLocalPowerScore += 150;
#else
		iLocalPowerScore += 125;
#endif
	}
	else if(fLocalPowerRatio >= 2.0)
	{
#ifdef NQ_TRIBUTE_EASIER_WITH_LOCAL_POWER
		iLocalPowerScore += 120;
#else
		iLocalPowerScore += 100;
#endif
	}
	else if(fLocalPowerRatio >= 1.5)
	{
#ifdef NQ_TRIBUTE_EASIER_WITH_LOCAL_POWER
		iLocalPowerScore += 90;
#else
		iLocalPowerScore += 75;
#endif
	}
	else if(fLocalPowerRatio >= 1.0)
	{
#ifdef NQ_TRIBUTE_EASIER_WITH_LOCAL_POWER
		iLocalPowerScore += 60;
#else
		iLocalPowerScore += 50;
#endif
	}
	else if(fLocalPowerRatio >= 0.5)
	{
#ifdef NQ_TRIBUTE_EASIER_WITH_LOCAL_POWER
		iLocalPowerScore += 30;
#else
		iLocalPowerScore += 25;
#endif
	}
	iScore += iLocalPowerScore;
	
	if (sTooltipSink)
	{
		Localization::String strPositiveFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_POSITIVE");
		strPositiveFactor << iLocalPowerScore;
		strPositiveFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_MILITARY_PRESENCE";
		sFactors += strPositiveFactor.toUTF8();
	}






	int iPoliciesScore = 0;
	int iPoliciesMod = GET_PLAYER(eBullyPlayer).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_MINOR_BULLY_SCORE_MODIFIER);
	if (iPoliciesMod != 0)
	{
		iPoliciesScore += iGlobalMilitaryScore;
		iPoliciesScore += iLocalPowerScore;

		iPoliciesScore *= iPoliciesMod;
		iPoliciesScore /= 100;
	}
	if (sTooltipSink && iPoliciesScore != 0)
	{
		Localization::String strPositiveFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_POSITIVE");
		strPositiveFactor << iPoliciesScore;
		strPositiveFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_GUNBOAT_DIPLOMACY";
		sFactors += strPositiveFactor.toUTF8();
	}
	iScore += iPoliciesScore;

#if defined(TRAITIFY)





	int iTraitScore = 0;
	int iTraitMod = GET_PLAYER(eBullyPlayer).GetPlayerTraits()->GetMinorBullyModifier();
	if (iTraitMod != 0)
	{
		iTraitScore += iGlobalMilitaryScore;
		iTraitScore += iLocalPowerScore;
		iTraitScore *= iTraitMod;
		iTraitScore /= 100;
	}
	if (sTooltipSink && iTraitScore != 0)
	{
		Localization::String strPositiveFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_POSITIVE");
		strPositiveFactor << iTraitScore;
		strPositiveFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_TRAIT_MODIFIER";
		sFactors += strPositiveFactor.toUTF8();
	}
	iScore += iTraitScore;
#endif
#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	{
		CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
		if(pkPersonalityInfo != NULL)
		{
			const int iPersonalityScore = pkPersonalityInfo->GetBullyScoreModifier();
			if(iPersonalityScore > 0)
			{
				iScore += iPersonalityScore;
				if(sTooltipSink)
				{
					Localization::String strPositiveFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_POSITIVE");
					strPositiveFactor << iPersonalityScore;
					strPositiveFactor << pkPersonalityInfo->GetDescription();
					sFactors += strPositiveFactor.toUTF8();
				}
			}
		}
	}
#endif





	const int iBaseReluctanceScore = -110;
	
	if (sTooltipSink)
	{
		Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
		strNegativeFactor << iBaseReluctanceScore;
		strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_BASE_RELUCTANCE";
		sFactors += strNegativeFactor.toUTF8();
	}

	iScore += iBaseReluctanceScore;






	if(GetEffectiveFriendshipWithMajor(eBullyPlayer) < GC.getFRIENDSHIP_THRESHOLD_CAN_BULLY())
	{
		int iInfluenceScore = iFailScore;
		iScore += iInfluenceScore;
		if (sTooltipSink)
		{
			Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
			strNegativeFactor << iInfluenceScore;
			strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_LOW_INFLUENCE";
			sFactors += strNegativeFactor.toUTF8();
		}
	}






	int iLastBullyTurn = GetTurnLastBulliedByMajor(eBullyPlayer);
	if(iLastBullyTurn >= 0)
	{
		if(iLastBullyTurn + 10 >= GC.getGame().getGameTurn())
		{
			int iBulliedVeryRecentlyScore = iFailScore;
			iScore += iBulliedVeryRecentlyScore;
			if (sTooltipSink)
			{
				Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
				strNegativeFactor << iBulliedVeryRecentlyScore;
				strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_BULLIED_VERY_RECENTLY";
				sFactors += strNegativeFactor.toUTF8();
			}
		}
		else if(iLastBullyTurn + 20 >= GC.getGame().getGameTurn())
		{
			int iBulliedRecentlyScore = -40;
			iScore += iBulliedRecentlyScore;
			if (sTooltipSink)
			{
				Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
				strNegativeFactor << iBulliedRecentlyScore;
				strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_BULLIED_RECENTLY";
				sFactors += strNegativeFactor.toUTF8();
			}
		}
	}






	if (bForUnit)
	{
		int iUnitScore = -30;
		iScore += iUnitScore;
		if (sTooltipSink)
		{
			Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
			strNegativeFactor << iUnitScore;
			strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_UNIT_RELUCTANCE";
			sFactors += strNegativeFactor.toUTF8();
		}
	}






	if (bForUnit)
	{
		if (GetPlayer()->getCapitalCity() == NULL || GetPlayer()->getCapitalCity()->getPopulation() < 4)
		{
			int iPopulationScore = iFailScore;
			iScore += iPopulationScore;
			if (sTooltipSink)
			{
				Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
				strNegativeFactor << iPopulationScore;
				strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_LOW_POPULATION";
				sFactors += strNegativeFactor.toUTF8();
			}
		}
	}






	if(GetAlly() != NO_PLAYER && GetAlly() != eBullyPlayer)
	{
		int iAllyScore = -10;
		iScore += iAllyScore;
		if (sTooltipSink)
		{
			Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
			strNegativeFactor << iAllyScore;
			strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_ALLIES";
			sFactors += strNegativeFactor.toUTF8();
		}
	}






	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		int iProtectionScore = 0;
		eMajorLoop = (PlayerTypes) iMajorLoop;
		if(eMajorLoop != eBullyPlayer && IsProtectedByMajor(eMajorLoop))
		{
			iProtectionScore += -20;
			iScore += iProtectionScore;
			if (sTooltipSink)
			{
				Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
				strNegativeFactor << iProtectionScore;
				strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_PLEDGES_TO_PROTECT";
				sFactors += strNegativeFactor.toUTF8();
			}
			break;
		}
	}






#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	if(pkPersonalityInfo != NULL && pkPersonalityInfo->GetBullyScoreModifier() < 0)
	{
		const int iPersonalityScore = pkPersonalityInfo->GetBullyScoreModifier();
		iScore += iPersonalityScore;
		if(sTooltipSink)
		{
			Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
			strNegativeFactor << iPersonalityScore;
			strNegativeFactor << pkPersonalityInfo->GetDescription();
			sFactors += strNegativeFactor.toUTF8();
		}
	}
#else
	if(GetPersonality() == MINOR_CIV_PERSONALITY_HOSTILE)
	{
		int iHostileScore = -10;
		iScore += iHostileScore;
		if (sTooltipSink)
		{
			Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
			strNegativeFactor << iHostileScore;
			strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_HOSTILE";
			sFactors += strNegativeFactor.toUTF8();
		}
	}
#endif
	if(GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
	{
		int iMilitaristicScore = -10;
		iScore += iMilitaristicScore;
		if (sTooltipSink)
		{
			Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
			strNegativeFactor << iMilitaristicScore;
			strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_MILITARISTIC";
			sFactors += strNegativeFactor.toUTF8();
		}
	}

	if (sTooltipSink != NULL)
	{
		(*sTooltipSink) += sFactors;
	}

	return iScore;
}

bool CvMinorCivAI::CanMajorBullyGold(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;


	if(!GetPlayer()->isAlive())
		return false;

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	if(pkPersonalityInfo != NULL && pkPersonalityInfo->IsBlocksTribute())
	{
		return false;
	}
#endif

	int iScore = CalculateBullyMetric(ePlayer,             false);
	return CanMajorBullyGold(ePlayer, iScore);
}


bool CvMinorCivAI::CanMajorBullyGold(PlayerTypes ePlayer, int iSpecifiedBullyMetric)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;


	if(!GetPlayer()->isAlive())
		return false;

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	if(pkPersonalityInfo != NULL && pkPersonalityInfo->IsBlocksTribute())
	{
		return false;
	}
#endif

	return (iSpecifiedBullyMetric >= 0);
}

CvString CvMinorCivAI::GetMajorBullyGoldDetails(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return "";
	
	CvString sFactors = "";
	int iScore = CalculateBullyMetric(ePlayer,             false, &sFactors);
	bool bCanBully = CanMajorBullyGold(ePlayer, iScore);

	Localization::String sFear = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_AFRAID");
	if (!bCanBully)
	{
		sFear = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_RESILIENT");
	}
	sFear << iScore;

	Localization::String sResult = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_GOLD_TT");
	sResult << sFear.toUTF8() << sFactors;

	return sResult.toUTF8();
}

bool CvMinorCivAI::CanMajorBullyUnit(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;


	if(!GetPlayer()->isAlive())
		return false;

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	if(pkPersonalityInfo != NULL && pkPersonalityInfo->IsBlocksTribute())
	{
		return false;
	}
#endif

	int iScore = CalculateBullyMetric(ePlayer,             true);
	return CanMajorBullyUnit(ePlayer, iScore);
}


bool CvMinorCivAI::CanMajorBullyUnit(PlayerTypes ePlayer, int iSpecifiedBullyMetric)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;


	if(!GetPlayer()->isAlive())
		return false;

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	if(pkPersonalityInfo != NULL && pkPersonalityInfo->IsBlocksTribute())
	{
		return false;
	}
#endif

	return (iSpecifiedBullyMetric >= 0);
}

CvString CvMinorCivAI::GetMajorBullyUnitDetails(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return "";

	CvString sFactors = "";
	int iScore = CalculateBullyMetric(ePlayer,             true, &sFactors);
	bool bCanBully = CanMajorBullyUnit(ePlayer, iScore);
	UnitTypes eUnitType = static_cast<UnitTypes>(GET_PLAYER(ePlayer).getCivilizationInfo().getCivilizationUnits(GC.getInfoTypeForString("UNITCLASS_WORKER")));
	CvUnitEntry* pUnitInfo = GC.getUnitInfo(eUnitType);
	CvAssert(pUnitInfo);
	if (!pUnitInfo)
		return "";

	Localization::String sFear = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_AFRAID");
	if (!bCanBully)
	{
		sFear = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_RESILIENT");
	}
	sFear << iScore;

	Localization::String sResult = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_UNIT_TT");
	sResult << sFear.toUTF8() << sFactors << pUnitInfo->GetDescriptionKey();

	return sResult.toUTF8();
}

void CvMinorCivAI::DoMajorBullyGold(PlayerTypes eBully, int iGold)
{
	CvAssertMsg(eBully >= 0, "eBully is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBully < MAX_MAJOR_CIVS, "eBully is expected to be within maximum bounds (invalid Index)");
	if(eBully < 0 || eBully >= MAX_MAJOR_CIVS) return;

	int iBullyMetric = CalculateBullyMetric(eBully,             false);
	bool bSuccess = CanMajorBullyGold(eBully, iBullyMetric);
	int iOldFriendshipTimes100 = GetEffectiveFriendshipWithMajorTimes100(eBully);

	if(bSuccess)
	{
		CvAssertMsg(iGold >= 0, "iGold is expected to be non-negative. Please send Anton your save file and version.");
	
		if(GC.getGame().getActivePlayer() == eBully)
		{
			int iBullyGold = 0;
			gDLL->GetSteamStat(ESTEAMSTAT_BULLIEDGOLD, &iBullyGold);

			iBullyGold += iGold;

			gDLL->SetSteamStat(ESTEAMSTAT_BULLIEDGOLD, iBullyGold);
		}

		GET_PLAYER(eBully).GetTreasury()->ChangeGold(iGold);
#ifdef NQ_MINOR_FRIENDSHIP_GAIN_BULLY_GOLD_SUCCESS_FROM_POLICIES
		int iInfluenceChange = 0;
		int iBullyInfluenceGain = GET_PLAYER(eBully).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_MINOR_FRIENDSHIP_GAIN_BULLY_GOLD_SUCCESS);
		bool bShouldRemoveQuests;
		if (iBullyInfluenceGain > 0)
		{
			iInfluenceChange = iBullyInfluenceGain;
			bShouldRemoveQuests = false;
		}
		else
		{
			iInfluenceChange = GC.getMINOR_FRIENDSHIP_DROP_BULLY_GOLD_SUCCESS();
			bShouldRemoveQuests = true;
		}
#if defined(LEKMOD_POLICY_MINOR_BULLY_TRIBUTE)
		{
			const int iLekmodTributeRewardTimes100 = GET_PLAYER(eBully).GetPlayerPolicies()->GetMinorBullyInfluenceReward();
			const bool bLekmodTributeNoPenalty = GET_PLAYER(eBully).GetPlayerPolicies()->IsMinorBullyNoPenalty();
			if (bLekmodTributeNoPenalty && iInfluenceChange < 0)
				iInfluenceChange = 0;
			iInfluenceChange += iLekmodTributeRewardTimes100;
			if (bLekmodTributeNoPenalty)
				bShouldRemoveQuests = false;
		}
#endif
		DoBulliedByMajorReaction(eBully, iInfluenceChange, bShouldRemoveQuests);
#else
		DoBulliedByMajorReaction(eBully, GC.getMINOR_FRIENDSHIP_DROP_BULLY_GOLD_SUCCESS());
#endif
#ifdef ENHANCED_GRAPHS
		ChangeBullyGoldAmountTotalByPlayer(eBully, iGold);
#endif
	}


	GET_PLAYER(eBully).GetDiplomacyAI()->LogMinorCivBullyGold(GetPlayer()->GetID(), iOldFriendshipTimes100, GetEffectiveFriendshipWithMajorTimes100(eBully), iGold, bSuccess, iBullyMetric);

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}

void CvMinorCivAI::DoMajorBullyUnit(PlayerTypes eBully, UnitTypes eUnitType)
{
	CvAssertMsg(eBully >= 0, "eBully is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBully < MAX_MAJOR_CIVS, "eBully is expected to be within maximum bounds (invalid Index)");
	if(eBully < 0 || eBully >= MAX_MAJOR_CIVS) return;

	int iBullyMetric = CalculateBullyMetric(eBully,             true);
	bool bSuccess = CanMajorBullyUnit(eBully, iBullyMetric);
	int iOldFriendshipTimes100 = GetEffectiveFriendshipWithMajorTimes100(eBully);
	UnitTypes bullyUnit = NO_UNIT;
	if(bSuccess)
	{
		if(eUnitType == NO_UNIT)
		{
			CvAssertMsg(false, "eUnitType is not expected to be NO_UNIT. Please send Anton your save file and version.");
			return;
		}


		CvCity* pCapital = GetPlayer()->getCapitalCity();
		if(pCapital == NULL)
		{
			CvAssertMsg(false, "Trying to spawn a Unit for a major civ but the minor has no capital. Please send Anton your save file and version.");
			return;
		}

		int iX = pCapital->getX();
		int iY = pCapital->getY();
		bullyUnit = static_cast<UnitTypes>(GET_PLAYER(eBully).getCivilizationInfo().getCivilizationUnits(GC.getUnitInfo(eUnitType)->GetUnitClassType()));
		CvUnit* pNewUnit = GET_PLAYER(eBully).initUnit(bullyUnit, iX, iY);
		if (pNewUnit->jumpToNearestValidPlot())
		{
			pNewUnit->finishMoves();

			if(GetPlayer()->getCapitalCity())
				GetPlayer()->getCapitalCity()->addProductionExperience(pNewUnit);

#ifdef NQ_MINOR_FRIENDSHIP_GAIN_BULLY_GOLD_SUCCESS_FROM_POLICIES
			{
				int iWorkerInfluenceTimes100 = GC.getMINOR_FRIENDSHIP_DROP_BULLY_WORKER_SUCCESS();
				bool bWorkerRemoveQuests = true;
#if defined(LEKMOD_POLICY_MINOR_BULLY_TRIBUTE)
				const int iLekmodTributeRewardTimes100 = GET_PLAYER(eBully).GetPlayerPolicies()->GetMinorBullyInfluenceReward();
				const bool bLekmodTributeNoPenalty = GET_PLAYER(eBully).GetPlayerPolicies()->IsMinorBullyNoPenalty();
				if (bLekmodTributeNoPenalty && iWorkerInfluenceTimes100 < 0)
					iWorkerInfluenceTimes100 = 0;
				iWorkerInfluenceTimes100 += iLekmodTributeRewardTimes100;
				if (bLekmodTributeNoPenalty)
					bWorkerRemoveQuests = false;
#endif
				DoBulliedByMajorReaction(eBully, iWorkerInfluenceTimes100, bWorkerRemoveQuests);
			}
#else
			DoBulliedByMajorReaction(eBully, GC.getMINOR_FRIENDSHIP_DROP_BULLY_WORKER_SUCCESS());
#endif
		}
		else
			pNewUnit->kill(false);
#ifdef ENHANCED_GRAPHS
		ChangeBullyWorkersAmountTotalByPlayer(eBully, 1);
#endif
	}


	GET_PLAYER(eBully).GetDiplomacyAI()->LogMinorCivBullyUnit(GetPlayer()->GetID(), iOldFriendshipTimes100, GetEffectiveFriendshipWithMajorTimes100(eBully), bullyUnit, bSuccess, iBullyMetric);

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}



#ifdef NQ_MINOR_FRIENDSHIP_GAIN_BULLY_GOLD_SUCCESS_FROM_POLICIES
void CvMinorCivAI::DoBulliedByMajorReaction(PlayerTypes eBully, int iInfluenceChangeTimes100, bool bShouldRemoveQuests)
#else
void CvMinorCivAI::DoBulliedByMajorReaction(PlayerTypes eBully, int iInfluenceChangeTimes100)
#endif
{
	CvAssertMsg(eBully >= 0, "eBully is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBully < MAX_MAJOR_CIVS, "eBully is expected to be within maximum bounds (invalid Index)");
	if(eBully < 0 || eBully >= MAX_MAJOR_CIVS) return;

	CvPlayer* pBully = &GET_PLAYER(eBully);
	CvAssertMsg(pBully, "pBully not expected to be NULL. Please send Anton your save file and version.");
	if (!pBully) return;

	SetTurnLastBulliedByMajor(eBully, GC.getGame().getGameTurn());
	ChangeFriendshipWithMajorTimes100(eBully, iInfluenceChangeTimes100);

#ifdef NQ_MINOR_FRIENDSHIP_GAIN_BULLY_GOLD_SUCCESS_FROM_POLICIES
	if (bShouldRemoveQuests)
#endif

	DoTestActiveQuests(                  false,                   true);


	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		PlayerTypes eMajorLoop = (PlayerTypes) iMajorLoop;
		CvPlayer* pMajorLoop = &GET_PLAYER(eMajorLoop);
		if (!pMajorLoop) continue;

		if(pMajorLoop->isAlive())
		{
			if(GET_TEAM(pMajorLoop->getTeam()).isHasMet(pBully->getTeam()))
			{
				pMajorLoop->GetDiplomacyAI()->DoPlayerBulliedSomeone(eBully, GetPlayer()->GetID());
			}
		}
	}


	for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		PlayerTypes eMinorLoop = (PlayerTypes) iMinorLoop;
		if (eMinorLoop == GetPlayer()->GetID()) continue;

		CvPlayer* pMinorLoop = &GET_PLAYER(eMinorLoop);
		if (!pMinorLoop) continue;

		if (pMinorLoop->isAlive() && pMinorLoop->GetMinorCivAI()->IsHasMetPlayer(eBully))
		{
			pMinorLoop->GetMinorCivAI()->DoTestActiveQuestsForPlayer(eBully,                   true,                   false, MINOR_CIV_QUEST_BULLY_CITY_STATE);
		}
	}
}

bool CvMinorCivAI::IsEverBulliedByAnyMajor() const
{
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		if(IsEverBulliedByMajor((PlayerTypes)iPlayerLoop))
			return true;
	return false;
}

bool CvMinorCivAI::IsEverBulliedByMajor(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	return (m_aiTurnLastBullied[ePlayer] >= 0);
}

bool CvMinorCivAI::IsRecentlyBulliedByAnyMajor() const
{
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		if(IsRecentlyBulliedByMajor((PlayerTypes)iPlayerLoop))
			return true;
	return false;
}











bool CvMinorCivAI::IsRecentlyBulliedByMajor(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	const int iRecentlyBulliedTurnInterval = 20;
	return (m_aiTurnLastBullied[ePlayer] >= 0 && m_aiTurnLastBullied[ePlayer] >= (GC.getGame().getGameTurn() - iRecentlyBulliedTurnInterval));
}

int CvMinorCivAI::GetTurnLastBulliedByMajor(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1;

	return m_aiTurnLastBullied[ePlayer];
}

void CvMinorCivAI::SetTurnLastBulliedByMajor(PlayerTypes ePlayer, int iTurn)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_aiTurnLastBullied[ePlayer] = iTurn;
}




void CvMinorCivAI::DoElection()
{

	if(GC.getGame().GetTurnsUntilMinorCivElection() != 0)
	{
		return;
	}

	CvWeightedVector<PlayerTypes, MAX_MAJOR_CIVS, true> wvVotes;
	Firaxis::Array<CvEspionageSpy*, MAX_MAJOR_CIVS> apSpy;
	CvCity* pCapital = GetPlayer()->getCapitalCity();
	if(!pCapital)
	{
		return;
	}

	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		PlayerTypes eEspionagePlayer = (PlayerTypes)ui;
		CvPlayerEspionage* pPlayerEspionage = GET_PLAYER(eEspionagePlayer).GetEspionage();
		int iVotes = 0;
		int iLoop;
		apSpy[ui] = NULL;

		if (!GET_PLAYER(eEspionagePlayer).isAlive())
		{
			continue;
		}

		for(CvCity* pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
		{
			CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
			int iSpyID = pCityEspionage->m_aiSpyAssignment[eEspionagePlayer];

			if(iSpyID == -1)
			{
				continue;
			}


			pPlayerEspionage->m_aSpyList[iSpyID].m_bEvaluateReassignment = true;


			if(pPlayerEspionage->m_aSpyList[iSpyID].m_eSpyState != SPY_STATE_RIG_ELECTION)
			{
				continue;
			}

			apSpy[ui] = &(pPlayerEspionage->m_aSpyList[iSpyID]);

			iVotes += (pCityEspionage->m_aiAmount[eEspionagePlayer] * (100 + m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_RIGGING_ELECTION_MODIFIER))) / 100;


			pCityEspionage->ResetProgress(eEspionagePlayer);

			int iRate = pPlayerEspionage->CalcPerTurn(SPY_STATE_RIG_ELECTION, pCity, iSpyID);
			int iGoal = pPlayerEspionage->CalcRequired(SPY_STATE_RIG_ELECTION, pCity, iSpyID);
			pCityEspionage->SetActivity(eEspionagePlayer, 0, iRate, iGoal);
			pCityEspionage->SetLastProgress(eEspionagePlayer, iRate);
		}

		if(iVotes > 0)
		{
			wvVotes.push_back(eEspionagePlayer, iVotes);
		}
	}

	if(wvVotes.size() > 0)
	{
		RandomNumberDelegate fcn;
		fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
		PlayerTypes eElectionWinner = wvVotes.ChooseByWeight(&fcn, "Choosing CS election winner by weight");

		for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
		{
			PlayerTypes ePlayer = (PlayerTypes)ui;

			if(ePlayer == eElectionWinner)
			{
				CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
				if(pNotifications)
				{
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_SUCCESS_S");
					strSummary << pCapital->getNameKey();
					Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_SUCCESS");
					strNotification << GET_PLAYER(ePlayer).GetEspionage()->GetSpyRankName(apSpy[ui]->m_eRank);
					strNotification << GET_PLAYER(ePlayer).getCivilizationInfo().getSpyNames(apSpy[ui]->m_iName);
					strNotification << pCapital->getNameKey();
					pNotifications->Add(NOTIFICATION_SPY_RIG_ELECTION_SUCCESS, strNotification.toUTF8(), strSummary.toUTF8(), pCapital->getX(), pCapital->getY(), -1);
				}

				int iInfluenceModifier = GET_PLAYER(ePlayer).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_RIGGING_ELECTION_MODIFIER);
				ChangeFriendshipWithMajor(ePlayer, GC.getESPIONAGE_INFLUENCE_GAINED_FOR_RIGGED_ELECTION() * (100 + iInfluenceModifier) / 100, false);


				if(ePlayer == GC.getGame().getActivePlayer())
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_XP1_14);
				}
			}
			else
			{
				int iFriendship = GetEffectiveFriendshipWithMajor(ePlayer);
				int iRelationshipAnchor = GetFriendshipAnchorWithMajor(ePlayer);
				bool bFriends = IsFriends(ePlayer);
				bool bMet = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(m_pPlayer->getTeam());


				if(apSpy[ui] != NULL)
				{
					CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
					if(pNotifications)
					{
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_FAILURE_S");
						strSummary << pCapital->getNameKey();
						Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_FAILURE");
						strNotification << GET_PLAYER(ePlayer).GetEspionage()->GetSpyRankName(apSpy[ui]->m_eRank);
						strNotification << GET_PLAYER(ePlayer).getCivilizationInfo().getSpyNames(apSpy[ui]->m_iName);
						strNotification << pCapital->getNameKey();
						strNotification << GET_PLAYER(eElectionWinner).getCivilizationShortDescriptionKey();
						pNotifications->Add(NOTIFICATION_SPY_RIG_ELECTION_FAILURE, strNotification.toUTF8(), strSummary.toUTF8(), pCapital->getX(), pCapital->getY(), -1);
					}
				}
				else if (bMet && (bFriends || iFriendship > iRelationshipAnchor))
				{

					CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
					if(pNotifications)
					{
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_ALERT_S");
						strSummary << pCapital->getNameKey();
						Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_ALERT");
						strNotification << pCapital->getNameKey();
						pNotifications->Add(NOTIFICATION_SPY_RIG_ELECTION_ALERT, strNotification.toUTF8(), strSummary.toUTF8(), pCapital->getX(), pCapital->getY(), -1);
					}
				}

				if (GetEffectiveFriendshipWithMajorTimes100(ePlayer) > 0)
				{
					int iInfluenceModifier = GET_PLAYER(eElectionWinner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_RIGGING_ELECTION_MODIFIER);
					int iDiminishAmount = min(GC.getESPIONAGE_INFLUENCE_LOST_FOR_RIGGED_ELECTION() * (100 + iInfluenceModifier), GetEffectiveFriendshipWithMajorTimes100(ePlayer));
					ChangeFriendshipWithMajorTimes100(ePlayer, -iDiminishAmount, false);
				}
			}
		}
	}

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
}








int CvMinorCivAI::GetNumUnitsGifted(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1;

	return m_aiNumUnitsGifted[ePlayer];
}


void CvMinorCivAI::SetNumUnitsGifted(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_aiNumUnitsGifted[ePlayer] = iValue;
}


void CvMinorCivAI::ChangeNumUnitsGifted(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	SetNumUnitsGifted(ePlayer, GetNumUnitsGifted(ePlayer) + iChange);
}

void CvMinorCivAI::DoUnitGiftFromMajor(PlayerTypes eFromPlayer, CvUnit* pGiftUnit, bool bDistanceGift)
{
	CvAssertMsg(eFromPlayer >= 0, "eFromPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eFromPlayer < MAX_MAJOR_CIVS, "eFromPlayer is expected to be within maximum bounds (invalid Index)");
	if (eFromPlayer < 0 || eFromPlayer >= MAX_MAJOR_CIVS) return;

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	if(pkPersonalityInfo && pkPersonalityInfo->IsNoGifts())
	{
		return;
	}

	if(IsMajorBlockedByAlliedWar(eFromPlayer))
	{
		return;
	}
#endif

	CvAssertMsg(pGiftUnit != NULL, "pGiftUnit is NULL");
	if (pGiftUnit == NULL) return;

	ChangeNumUnitsGifted(eFromPlayer, 1);


	int iInfluence = GetFriendshipFromUnitGift(eFromPlayer, pGiftUnit->IsGreatPerson(), bDistanceGift);
	ChangeFriendshipWithMajor(eFromPlayer, iInfluence);


	if(pGiftUnit->IsGreatPerson())
	{
		pGiftUnit->kill(false);
	}
}

int CvMinorCivAI::GetFriendshipFromUnitGift(PlayerTypes eFromPlayer, bool bGreatPerson, bool                  )
{
	CvAssertMsg(eFromPlayer >= 0, "eFromPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eFromPlayer < MAX_MAJOR_CIVS, "eFromPlayer is expected to be within maximum bounds (invalid Index)");
	if (eFromPlayer < 0 || eFromPlayer >= MAX_MAJOR_CIVS) return 0;

	int iInfluence = 0;
	CvPlayer& kFromPlayer = GET_PLAYER(eFromPlayer);


	if (bGreatPerson)
	{
		int iGPInfluence = kFromPlayer.GetPlayerTraits()->GetGreatPersonGiftInfluence();
#ifdef LEKMOD_GOLD_FROM_GIFTING_GP_TRAIT

		int iGoldtoGift = (kFromPlayer.GetPlayerTraits()->GetGreatPersonGiftInfluence()) * 2 ;
		kFromPlayer.GetTreasury()->ChangeGold(iGoldtoGift);
		if (iGPInfluence > 0)
		{
			iInfluence += iGPInfluence;
		}
#endif
	}
	else
	{
		iInfluence += GC.getFRIENDSHIP_PER_UNIT_GIFTED();


		if (IsProxyWarActiveForMajor(eFromPlayer))
		{
			iInfluence += GC.getFRIENDSHIP_PER_UNIT_GIFTED();
		}
		

		int iMilitaryInfluence = kFromPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_MILITARY_UNIT_GIFT_INFLUENCE);
		if (iMilitaryInfluence > 0)
		{
			iInfluence += iMilitaryInfluence;
		}
	}

	return iInfluence;
}


int CvMinorCivAI::GetNumGoldGifted(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0;
	return m_aiNumGoldGifted[ePlayer];
}

void CvMinorCivAI::SetNumGoldGifted(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;
	m_aiNumGoldGifted[ePlayer] = iValue;
}

void CvMinorCivAI::ChangeNumGoldGifted(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;
	SetNumGoldGifted(ePlayer, GetNumGoldGifted(ePlayer) + iChange);
}



void CvMinorCivAI::DoGoldGiftFromMajor(PlayerTypes ePlayer, int iGold)
{
#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	if(pkPersonalityInfo && pkPersonalityInfo->IsNoGifts())
	{
		return;
	}

	if(IsMajorBlockedByAlliedWar(ePlayer))
	{
		return;
	}
#endif

#ifdef NQ_NUM_TURNS_BEFORE_MINOR_ALLIES_REFUSE_BRIBES_FROM_TRAIT
	PlayerTypes iAlly = GetAlly();
	if (iAlly != NO_PLAYER && iAlly != ePlayer)
	{
		int iNumTurns = GET_PLAYER(iAlly).GetNumTurnsBeforeMinorAlliesRefuseBribes();
		if (iNumTurns > 0 && GetAlliedTurns() >= iNumTurns)
		{
			return;
		}
	}
#endif
	if(GET_PLAYER(ePlayer).GetTreasury()->GetGold() >= iGold)
	{
		int iFriendshipChange = GetFriendshipFromGoldGift(ePlayer, iGold);
		if(iFriendshipChange > 0)
			GET_PLAYER(ePlayer).GetTreasury()->LogExpenditure(GetPlayer()->GetMinorCivAI()->GetNamesListAsString(0), iGold,4);

		GET_PLAYER(ePlayer).GetTreasury()->ChangeGold(-iGold);
		
		ChangeNumGoldGifted(ePlayer, iGold);
		
		ChangeFriendshipWithMajor(ePlayer, iFriendshipChange);


		DoTestActiveQuestsForPlayer(ePlayer,                   true,                   false, MINOR_CIV_QUEST_GIVE_GOLD);
	}

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}



int CvMinorCivAI::GetFriendshipFromGoldGift(PlayerTypes eMajor, int iGold)
{

	iGold = (int) pow((double) iGold, (double)          GC.getGOLD_GIFT_FRIENDSHIP_EXPONENT());

	int iFriendship = int(iGold /         GC.getGOLD_GIFT_FRIENDSHIP_DIVISOR());


	double fGameProgressFactor = float(GC.getGame().getElapsedGameTurns()) / GC.getGame().getEstimateEndTurn();
	fGameProgressFactor = min(fGameProgressFactor, 1.0);
	

	fGameProgressFactor *=       GC.getMINOR_CIV_GOLD_GIFT_GAME_MULTIPLIER();
	fGameProgressFactor /=       GC.getMINOR_CIV_GOLD_GIFT_GAME_DIVISOR();
	fGameProgressFactor = 1 - fGameProgressFactor;

	iFriendship = (int)(iFriendship * fGameProgressFactor);



	int iFriendshipMod = GET_PLAYER(eMajor).getMinorGoldFriendshipMod();
	iFriendshipMod += GET_PLAYER(eMajor).GetReligions()->GetCityStateInfluenceModifier();
	if(iFriendshipMod != 0)
	{
		iFriendship *= (100 + iFriendshipMod);
		iFriendship /= 100;
	}


	iFriendship *= GC.getGame().getGameSpeedInfo().getGoldGiftMod();
	iFriendship /= 100;


	if(IsActiveQuestForPlayer(eMajor, MINOR_CIV_QUEST_INVEST))
	{
		int iBoostPercentage = 50;
		iFriendship *= 100 + iBoostPercentage;
		iFriendship /= 100;
	}


	iFriendship = max(iFriendship,       GC.getMINOR_CIV_GOLD_GIFT_MINIMUM_FRIENDSHIP_REWARD());


	int iVisibleDivisor =       GC.getMINOR_CIV_GOLD_GIFT_VISIBLE_DIVISOR();
	iFriendship /= iVisibleDivisor;
	iFriendship *= iVisibleDivisor;

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	if(pkPersonalityInfo && pkPersonalityInfo->GetGoldGiftInfluenceModifierPercent() != 100)
	{
		iFriendship *= pkPersonalityInfo->GetGoldGiftInfluenceModifierPercent();
		iFriendship /= 100;

		iFriendship /= iVisibleDivisor;
		iFriendship *= iVisibleDivisor;
	}
#endif

	return iFriendship;
}

#ifdef NQ_BELIEF_TOGGLE_ALLOW_FAITH_GIFTS_TO_MINORS

void CvMinorCivAI::DoFaithGiftFromMajor(PlayerTypes ePlayer, int iFaith)
{
#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	if(pkPersonalityInfo && pkPersonalityInfo->IsNoGifts())
	{
		return;
	}

	if(IsMajorBlockedByAlliedWar(ePlayer))
	{
		return;
	}
#endif

	if(GET_PLAYER(ePlayer).CanFaithGiftMinors() && GET_PLAYER(ePlayer).GetFaith() >= iFaith)
	{
		int iFriendshipChange = GetFriendshipFromFaithGift(ePlayer, iFaith);





		GET_PLAYER(ePlayer).ChangeFaith(-iFaith);
		


		
		ChangeFriendshipWithMajor(ePlayer, iFriendshipChange);




	}

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}


int CvMinorCivAI::GetFriendshipFromFaithGift(PlayerTypes eMajor, int iFaith)
{
	int iFriendship = iFaith * 8;
	iFriendship /= 100;


	iFriendship *= GC.getGame().getGameSpeedInfo().getGoldGiftMod();
	iFriendship /= 100;


	iFriendship = max(iFriendship,       GC.getMINOR_CIV_GOLD_GIFT_MINIMUM_FRIENDSHIP_REWARD());


	int iVisibleDivisor =       GC.getMINOR_CIV_GOLD_GIFT_VISIBLE_DIVISOR();
	iFriendship /= iVisibleDivisor;
	iFriendship *= iVisibleDivisor;


	if (IsSameReligionAsMajor(eMajor))
		iFriendship *= 3;

	return iFriendship;
}
#endif



bool CvMinorCivAI::CanMajorGiftTileImprovement(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	CvMinorCivPersonalityInfo* pkPersonalityInfo = GetPersonalityInfo();
	if(pkPersonalityInfo && pkPersonalityInfo->IsNoGifts())
	{
		return false;
	}

	if(IsMajorBlockedByAlliedWar(eMajor))
	{
		return false;
	}
#endif

	CvPlayer* pPlayer = &GET_PLAYER(eMajor);
	if(pPlayer == NULL)
	{
		CvAssertMsg(false, "pPlayer not expected to be NULL. Please send Anton your save file and version.");
		return false;
	}


	const int iCost = GetGiftTileImprovementCost(eMajor);
	if(pPlayer->GetTreasury()->GetGold() < iCost)
		return false;


	bool bHasValidPlot = false;
	CvPlotsVector& aiPlots = GetPlayer()->GetPlots();
	for(uint ui = 0; ui < aiPlots.size(); ui++)
	{

		if(aiPlots[ui] == -1)
		{
			break;
		}

		CvPlot* pPlot = GC.getMap().plotByIndex(aiPlots[ui]);
		if (IsLackingGiftableTileImprovementAtPlot(eMajor, pPlot->getX(), pPlot->getY()))
		{
			bHasValidPlot = true;
			break;
		}
	}

	if (!bHasValidPlot)
		return false;

	return true;
}


bool CvMinorCivAI::IsLackingGiftableTileImprovementAtPlot(PlayerTypes eMajor, int iPlotX, int iPlotY)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;
	CvPlot* pPlot = GC.getMap().plot(iPlotX, iPlotY);
	if(pPlot == NULL)
	{
		CvAssertMsg(false, "pPlot not expected to be NULL, invalid coordinates. Please send Anton your save file and version.");
		return false;
	}


	if(pPlot->getOwner() != GetPlayer()->GetID())
		return false;


	ResourceTypes eResource = pPlot->getResourceType();
	if(eResource == NO_RESOURCE)
	{
		return false;
	}
	else
	{
		ResourceUsageTypes eUsage = GC.getResourceInfo(eResource)->getResourceUsage();
		if(eUsage != RESOURCEUSAGE_STRATEGIC && eUsage != RESOURCEUSAGE_LUXURY)
		{
			return false;
		}
	}

	ImprovementTypes eImprovement = pPlot->getImprovementTypeNeededToImproveResource(eMajor, false               );


	if(eImprovement == NO_IMPROVEMENT)
		return false;

	return true;
}


bool CvMinorCivAI::CanMajorGiftTileImprovementAtPlot(PlayerTypes eMajor, int iPlotX, int iPlotY)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;

	return (CanMajorGiftTileImprovement(eMajor) && IsLackingGiftableTileImprovementAtPlot(eMajor, iPlotX, iPlotY));
}

int CvMinorCivAI::GetGiftTileImprovementCost(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return -1;

	return GC.getMINOR_CIV_TILE_IMPROVEMENT_GIFT_COST();
}

void CvMinorCivAI::DoTileImprovementGiftFromMajor(PlayerTypes eMajor, int iPlotX, int iPlotY)
{
	if(!CanMajorGiftTileImprovementAtPlot(eMajor, iPlotX, iPlotY))
	{
		return;
	}
	CvPlot* pPlot = GC.getMap().plot(iPlotX, iPlotY);
	if(pPlot == NULL)
	{
		CvAssertMsg(false, "pPlot not expected to be NULL, invalid coordinates. Please send Anton your save file and version.");
		return;
	}
	CvPlayer* pPlayer = &GET_PLAYER(eMajor);
	if(pPlayer == NULL)
	{
		CvAssertMsg(false, "pPlayer not expected to be NULL. Please send Anton your save file and version.");
		return;
	}

	ImprovementTypes eImprovement = pPlot->getImprovementTypeNeededToImproveResource(eMajor,               false);
	if(eImprovement == NO_IMPROVEMENT)
	{
		return;
	}

	pPlot->setImprovementType(eImprovement, eMajor);


	auto_ptr<ICvPlot1> pDllPlot(new CvDllPlot(pPlot));
	gDLL->GameplayDoFX(pDllPlot.get());

	const int iCost = GetGiftTileImprovementCost(eMajor);
	GET_PLAYER(eMajor).GetTreasury()->LogExpenditure(GetPlayer()->GetMinorCivAI()->GetNamesListAsString(0), iCost,5);
	pPlayer->GetTreasury()->ChangeGold(-iCost);
}


void CvMinorCivAI::DoNowAtWarWithTeam(TeamTypes eTeam)
{
	int iOldFriendship;
	int iWarFriendship =         GC.getMINOR_FRIENDSHIP_AT_WAR();

	PlayerTypes ePlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(GET_PLAYER(ePlayer).getTeam() == eTeam)
		{

			iOldFriendship = GetBaseFriendshipWithMajor(ePlayer);
			DoFriendshipChangeEffects(ePlayer, iOldFriendship, iWarFriendship);


			if(IsProtectedByMajor(ePlayer))
			{
				DoChangeProtectionFromMajor(ePlayer, false);
			}


			if(GetNumActiveQuestsForPlayer(ePlayer) > 0)
			{
				EndAllActiveQuestsForPlayer(ePlayer);
			}







		}
		else if (!IsAtWarWithPlayersTeam(ePlayer) && GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(eTeam))
		{

			if (IsFriends(ePlayer) && GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
			{
				int iBaseSpawnTurns = GetSpawnBaseTurns(ePlayer);
				iBaseSpawnTurns = MAX(iBaseSpawnTurns, 1);
				if (iBaseSpawnTurns < GetUnitSpawnCounter(ePlayer))
				{
					SetUnitSpawnCounter(ePlayer, iBaseSpawnTurns);
				}
			}
		}
	}

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}


void CvMinorCivAI::DoNowPeaceWithTeam(TeamTypes eTeam)
{
	int iWarFriendship =         GC.getMINOR_FRIENDSHIP_AT_WAR();
	int iNewFriendship;

	PlayerTypes ePlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(GET_PLAYER(ePlayer).getTeam() == eTeam)
		{

			iNewFriendship = GetBaseFriendshipWithMajor(ePlayer);
			DoFriendshipChangeEffects(ePlayer, iWarFriendship, iNewFriendship);
		}
	}

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}


bool CvMinorCivAI::IsPeaceBlocked(TeamTypes eTeam) const
{

	if(IsPermanentWar(eTeam))
		return true;

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES

	if(IsNeverAlliedWarSupport())
		return false;
#endif


	PlayerTypes eMajor;
	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;


		if(!GET_PLAYER(eMajor).isAlive())
			continue;


		if(!IsAllies(eMajor))
			continue;


		if(!GET_TEAM(GET_PLAYER(eMajor).getTeam()).isAtWar(eTeam))
			continue;

		return true;
	}

	return false;
}

#ifdef NQ_PEACE_BLOCKED_IF_INFLUENCE_TOO_LOW
bool CvMinorCivAI::IsInfluenceTooLowForPeace(PlayerTypes ePlayer)
{
	return GetBaseFriendshipWithMajor(ePlayer) < -50;
}
#endif


void CvMinorCivAI::DoTeamDeclaredWarOnMe(TeamTypes eEnemyTeam)
{
	CvTeam* pEnemyTeam = &GET_TEAM(eEnemyTeam);
	CivsList veMinorsNowWary;
	int iRand;


	for(int iEnemyMajorLoop = 0; iEnemyMajorLoop < MAX_MAJOR_CIVS; iEnemyMajorLoop++)
	{
		PlayerTypes eEnemyMajorLoop = (PlayerTypes) iEnemyMajorLoop;
		if(!GET_PLAYER(eEnemyMajorLoop).isAlive())
			continue;
		if(GET_PLAYER(eEnemyMajorLoop).getTeam() != eEnemyTeam)
			continue;		


		
		SetFriendshipWithMajor(eEnemyMajorLoop, GC.getMINOR_FRIENDSHIP_AT_WAR());
	}

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	if(IsBlocksWarDeclarationPenalty())
	{
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		return;
	}
#endif



	if(pEnemyTeam->IsMinorCivWarmonger())
	{
		if(!IsWaryOfTeam(eEnemyTeam))
		{
			SetWaryOfTeam(eEnemyTeam, true);
			veMinorsNowWary.push_back(GetPlayer()->GetID());
		}
		if(ENABLE_PERMANENT_WAR)
			SetPermanentWar(eEnemyTeam, true);
	}

	else if(pEnemyTeam->IsMinorCivAggressor())
	{
		iRand = GC.getGame().getJonRandNum(100, "MINOR CIV AI: Become Wary Of aggressor roll.");

		if(iRand <        GC.getPERMANENT_WAR_AGGRESSOR_CHANCE())
		{
			if(!IsWaryOfTeam(eEnemyTeam))
			{
				SetWaryOfTeam(eEnemyTeam, true);
				veMinorsNowWary.push_back(GetPlayer()->GetID());
			}
			if(ENABLE_PERMANENT_WAR)
				SetPermanentWar(eEnemyTeam, true);
		}
	}


	if(pEnemyTeam->IsMinorCivAggressor())
	{
		int iChance;

		PlayerProximityTypes eProximity;

		int iAttackingMajorPlayer;
		PlayerTypes eAttackingMajorPlayer;
		bool bAttackerIsAlly;

		CvPlayer* pOtherMinorCiv;
		PlayerTypes eOtherMinorCiv;
		for(int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
		{
			eOtherMinorCiv = (PlayerTypes) iMinorCivLoop;
			pOtherMinorCiv = &GET_PLAYER((eOtherMinorCiv));

			iChance = 0;


			if(!pOtherMinorCiv->isAlive())
				continue;


			if(eOtherMinorCiv == GetPlayer()->GetID())
				continue;


			if(pOtherMinorCiv->GetMinorCivAI()->IsWantsMinorDead(GetPlayer()->GetID()))
				continue;

			bAttackerIsAlly = false;


			for(iAttackingMajorPlayer = 0; iAttackingMajorPlayer < MAX_MAJOR_CIVS; iAttackingMajorPlayer++)
			{
				eAttackingMajorPlayer = (PlayerTypes) iAttackingMajorPlayer;


				if(GET_PLAYER(eAttackingMajorPlayer).getTeam() != eEnemyTeam)
					continue;


				if(!GET_PLAYER(eAttackingMajorPlayer).isAlive())
					continue;

				if(pOtherMinorCiv->GetMinorCivAI()->GetAlly() == eAttackingMajorPlayer)
				{
					bAttackerIsAlly = true;
					break;
				}
			}

			if(bAttackerIsAlly)
				continue;


			eProximity = pOtherMinorCiv->GetProximityToPlayer(GetPlayer()->GetID());


			if(pEnemyTeam->IsMinorCivWarmonger())
			{
				if(eProximity == PLAYER_PROXIMITY_DISTANT)
					iChance +=        GC.getPERMANENT_WAR_OTHER_WARMONGER_CHANCE_DISTANT();
				else if(eProximity == PLAYER_PROXIMITY_FAR)
					iChance +=        GC.getPERMANENT_WAR_OTHER_WARMONGER_CHANCE_FAR();
				else if(eProximity == PLAYER_PROXIMITY_CLOSE)
					iChance +=        GC.getPERMANENT_WAR_OTHER_WARMONGER_CHANCE_CLOSE();
				else if(eProximity == PLAYER_PROXIMITY_NEIGHBORS)
					iChance +=         GC.getPERMANENT_WAR_OTHER_WARMONGER_CHANCE_NEIGHBORS();
			}

			else
			{
				if(eProximity == PLAYER_PROXIMITY_DISTANT)
					iChance +=       GC.getPERMANENT_WAR_OTHER_CHANCE_DISTANT();
				else if(eProximity == PLAYER_PROXIMITY_FAR)
					iChance +=       GC.getPERMANENT_WAR_OTHER_CHANCE_FAR();
				else if(eProximity == PLAYER_PROXIMITY_CLOSE)
					iChance +=        GC.getPERMANENT_WAR_OTHER_CHANCE_CLOSE();
				else if(eProximity == PLAYER_PROXIMITY_NEIGHBORS)
					iChance +=        GC.getPERMANENT_WAR_OTHER_CHANCE_NEIGHBORS();
			}


			if(GET_TEAM(pOtherMinorCiv->getTeam()).isAtWar(eEnemyTeam))
				iChance +=        GC.getPERMANENT_WAR_OTHER_AT_WAR();

			iRand = GC.getGame().getJonRandNum(100, "MINOR CIV AI: Third party minor to become Wary Of aggressor roll.");
			if(iRand < iChance)
			{
				if(!pOtherMinorCiv->GetMinorCivAI()->IsWaryOfTeam(eEnemyTeam))
				{
					pOtherMinorCiv->GetMinorCivAI()->SetWaryOfTeam(eEnemyTeam, true);
					veMinorsNowWary.push_back(eOtherMinorCiv);
				}
				if(ENABLE_PERMANENT_WAR)
					pOtherMinorCiv->GetMinorCivAI()->SetPermanentWar(eEnemyTeam, true);
			}
		}
	}

	if(veMinorsNowWary.size() > 0)
	{
		int iMinimumFriendshipMod = 20;
		Localization::String strTemp;

		strTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_MINORS_NOW_WARY");
		CvString strSummary = strTemp.toUTF8();
		strTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_MINORS_NOW_WARY_TT");
		strTemp << iMinimumFriendshipMod;
		CvString strMessage = strTemp.toUTF8();

		for(int iEnemyMajorLoop = 0; iEnemyMajorLoop < MAX_MAJOR_CIVS; iEnemyMajorLoop++)
		{
			PlayerTypes eEnemyMajorLoop = (PlayerTypes) iEnemyMajorLoop;
			if(!GET_PLAYER(eEnemyMajorLoop).isAlive())
				continue;
			if(GET_PLAYER(eEnemyMajorLoop).getTeam() != eEnemyTeam)
				continue;

			strMessage = strMessage + GetNamesListAsString(veMinorsNowWary);
			AddNotification(strMessage, strSummary, eEnemyMajorLoop);
		}
	}

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}


bool CvMinorCivAI::IsPermanentWar(TeamTypes eTeam) const
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	if(eTeam < 0 || eTeam >= MAX_TEAMS) return false;

	return m_abPermanentWar[eTeam];
}


void CvMinorCivAI::SetPermanentWar(TeamTypes eTeam, bool bValue)
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	if(eTeam < 0 || eTeam >= MAX_TEAMS) return;

	if(ENABLE_PERMANENT_WAR)
		m_abPermanentWar[eTeam] = bValue;
}


bool CvMinorCivAI::IsWaryOfTeam(TeamTypes eTeam) const
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	if(eTeam < 0 || eTeam >= MAX_TEAMS) return false;

	return m_abWaryOfTeam[eTeam];
}


void CvMinorCivAI::SetWaryOfTeam(TeamTypes eTeam, bool bValue)
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	if(eTeam < 0 || eTeam >= MAX_TEAMS) return;

	if(IsWaryOfTeam(eTeam) == bValue)
		return;

	m_abWaryOfTeam[eTeam] = bValue;
}








bool CvMinorCivAI::IsHasMetPlayer(PlayerTypes ePlayer)
{
	return GET_TEAM(GetPlayer()->getTeam()).isHasMet(GET_PLAYER(ePlayer).getTeam());
}


bool CvMinorCivAI::IsAtWarWithPlayersTeam(PlayerTypes ePlayer)
{
	return GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(GetPlayer()->getTeam());
}


int CvMinorCivAI::GetNumResourcesMajorLacks(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	int iNumTheyLack = 0;


	ResourceTypes eResource;
#ifdef AUI_WARNING_FIXES
	for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
	{
		eResource = (ResourceTypes) iResourceLoop;

		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);

		if(pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_BONUS)
			continue;


		int iMinorHas = GetPlayer()->getNumResourceTotal(eResource,                    false);
#ifdef LEKMOD_CS_BUILDING_STRATEGIC_NO_ALLY_SHARE
		if(pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
		{
			iMinorHas -= GetPlayer()->getNumMinorStrategicResourceFromBuildings(eResource);
			if(iMinorHas < 0)
			{
				iMinorHas = 0;
			}
		}
#endif
		if(iMinorHas == 0)
			continue;


		if(GET_PLAYER(eMajor).getNumResourceTotal(eResource,                    false) > 0)
			continue;

		iNumTheyLack++;
	}

	return iNumTheyLack;
}


TechTypes CvMinorCivAI::GetGoodTechPlayerDoesntHave(PlayerTypes ePlayer, int iRoughTechValue) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	CvWeightedVector<int, SAFE_ESTIMATE_NUM_XML_WIDGETS, true> TechVector;
	int iValue, iProgress;


	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvTeam kTeam = GET_TEAM(kPlayer.getTeam());

#ifdef AUI_WARNING_FIXES
	for (uint iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
#else
	for(int iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
#endif
	{
		const TechTypes eTech = static_cast<TechTypes>(iTechLoop);
		CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
		if(pkTechInfo == NULL)
			continue;


		if(!kTeam.GetTeamTechs()->HasTech(eTech))
		{

			if(kPlayer.GetPlayerTechs()->CanResearch(eTech))
			{
				iValue = pkTechInfo->GetResearchCost();


				iProgress = kTeam.GetTeamTechs()->GetResearchProgress(eTech);

				if(iProgress > 0)
				{
					iValue -= iProgress;
				}


				iValue += GC.getGame().getJonRandNum(iValue / 4, "Minor Civ Quest Reward: Tech - Adding random weight to Tech Reward");

				TechVector.push_back(iTechLoop, iValue);
			}
		}
	}


	if(TechVector.size() == 1)
	{
		return (TechTypes) TechVector.GetElement(0);
	}
	else if(TechVector.size() == 0)
	{
		return NO_TECH;
	}

	TechVector.SortItems();


	if(iRoughTechValue > 20)
	{
		iRoughTechValue = 20;
	}

	int iIndex = (TechVector.size() - 1) * iRoughTechValue / 20;

	return (TechTypes) TechVector.GetElement(iIndex);
}


bool CvMinorCivAI::IsSameReligionAsMajor(PlayerTypes eMajor)
{
	CvPlayer* pkPlayer = GetPlayer();
	if(pkPlayer)
	{
		CvCity* pkCity = pkPlayer->getCapitalCity();
		if(pkCity)
		{
			ReligionTypes eMinorReligion = pkCity->GetCityReligions()->GetReligiousMajority();
			ReligionTypes eMajorReligion = GC.getGame().GetGameReligions()->GetReligionCreatedByPlayer(eMajor);

			if(eMinorReligion != NO_RELIGION &&
			        eMajorReligion != NO_RELIGION &&
			        eMinorReligion == eMajorReligion)
			{
				return true;
			}
		}
	}
	return false;
}

CvString CvMinorCivAI::GetStatusChangeDetails(PlayerTypes ePlayer, bool bAdd, bool bFriends, bool bAllies)
{
	Localization::String strDetailedInfo;

	MinorCivTraitTypes eTrait = GetTrait();

	if(eTrait == MINOR_CIV_TRAIT_CULTURED)
	{
		int iCultureBonusAmount = 0;
		if (bFriends)
		{
			iCultureBonusAmount += GetCultureFlatFriendshipBonus(ePlayer);
		}
		if (bAllies)
		{
			iCultureBonusAmount += GetCultureFlatAlliesBonus(ePlayer);
		}
		if (!bAdd)
		{
			iCultureBonusAmount = -iCultureBonusAmount;
		}

		if(bAllies && bAdd)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_CULTURE");
			strDetailedInfo << iCultureBonusAmount;
		}
		else if(bFriends && bAdd)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_CULTURE");
			strDetailedInfo << iCultureBonusAmount;
		}
		else if(!bAdd)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LOST_CULTURE");
			strDetailedInfo << iCultureBonusAmount;
		}
	}
	else if(eTrait == MINOR_CIV_TRAIT_MILITARISTIC)
	{
		if(bAllies && bAdd)
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_MILITARISTIC");
		else if(bFriends && bAdd)
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_MILITARISTIC");
		else if(bFriends && !bAdd)
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LOST_FRIENDS_MILITARISTIC");
		else if(bAllies && !bAdd)
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LOST_ALLIES_MILITARISTIC");
	}
	else if(eTrait == MINOR_CIV_TRAIT_MARITIME)
	{
		int iCapitalFoodTimes100 = 0;
		int iOtherCitiesFoodTimes100 = 0;

		if(bFriends)
		{
			iCapitalFoodTimes100 += GetFriendsCapitalFoodBonus(ePlayer);
			iOtherCitiesFoodTimes100 += GetFriendsOtherCityFoodBonus(ePlayer);
		}
		if(bAllies)
		{
			iCapitalFoodTimes100 += GetAlliesCapitalFoodBonus(ePlayer);
			iOtherCitiesFoodTimes100 += GetAlliesOtherCityFoodBonus(ePlayer);
		}

		if(!bAdd)
		{
			iCapitalFoodTimes100 = -iCapitalFoodTimes100;
			iOtherCitiesFoodTimes100 = -iOtherCitiesFoodTimes100;
		}


		iCapitalFoodTimes100 += iOtherCitiesFoodTimes100;
		float fCapitalFood = float(iCapitalFoodTimes100) / 100;
		float fOtherCitiesFood = float(iOtherCitiesFoodTimes100) / 100;


		if(bAllies && bAdd)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_MARITIME");
			strDetailedInfo << fCapitalFood << fOtherCitiesFood;
		}
		else if(bFriends && bAdd)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_MARITIME");
			strDetailedInfo << fCapitalFood << fOtherCitiesFood;
		}
		else if(!bAdd)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LOST_MARITIME");
			strDetailedInfo << fCapitalFood << fOtherCitiesFood;
		}
	}
	else if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
	{
		int iHappinessBonus = 0;

		if(bFriends)
		{
			iHappinessBonus += GetHappinessFlatFriendshipBonus(ePlayer) + GetHappinessPerLuxuryFriendshipBonus(ePlayer);
		}
		if(bAllies)
		{
			iHappinessBonus += GetHappinessFlatAlliesBonus(ePlayer) + GetHappinessPerLuxuryAlliesBonus(ePlayer);
		}
		if(!bAdd)
		{
			iHappinessBonus = -iHappinessBonus;
		}

		if(bAllies && bAdd)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_MERCANTILE");
			strDetailedInfo << iHappinessBonus;
		}
		else if(bFriends && bAdd)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_MERCANTILE");
			strDetailedInfo << iHappinessBonus;
		}
		else if(!bAdd)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LOST_MERCANTILE");
			strDetailedInfo << iHappinessBonus;
		}
	}


	if(eTrait == MINOR_CIV_TRAIT_RELIGIOUS)
	{
		int iFaithBonusAmount = 0;
		if (bFriends)
		{
			iFaithBonusAmount += GetFaithFlatFriendshipBonus(ePlayer);
		}
		if (bAllies)
		{
			iFaithBonusAmount += GetFaithFlatAlliesBonus(ePlayer);
		}
		if (!bAdd)
		{
			iFaithBonusAmount = -iFaithBonusAmount;
		}

		if(bAllies && bAdd)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_RELIGIOUS");
			strDetailedInfo << iFaithBonusAmount;
		}
		else if(bFriends && bAdd)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_RELIGIOUS");
			strDetailedInfo << iFaithBonusAmount;
		}
		else if(!bAdd)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LOST_RELIGIOUS");
			strDetailedInfo << iFaithBonusAmount;
		}
	}

	return strDetailedInfo.toUTF8();
}

pair<CvString, CvString> CvMinorCivAI::GetStatusChangeNotificationStrings(PlayerTypes ePlayer, bool bAdd, bool bFriends, bool bAllies, PlayerTypes eOldAlly, PlayerTypes eNewAlly)
{
	Localization::String strMessage;
	Localization::String strSummary;

	CvTeam* pTeam = &GET_TEAM(GET_PLAYER(ePlayer).getTeam());
	CvAssertMsg(pTeam, "pTeam not expected to be NULL. Please send Anton your save file and version.");

	const char* strMinorsNameKey = GetPlayer()->getNameKey();


	if(bAdd)
	{

		if(bAllies)
		{



			if(eOldAlly == NO_PLAYER || eOldAlly == ePlayer)
			{
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_BASE");
			}

			else
			{
				CvAssertMsg(eOldAlly != NO_PLAYER, "eOldAlly not expected to be NO_PLAYER here. Please send Anton your save file and version.");
				const char* strOldBestPlayersNameKey = "TXT_KEY_UNMET_PLAYER";
				TeamTypes eOldAllyTeam = GET_PLAYER(eOldAlly).getTeam();
				if(pTeam->isHasMet(eOldAllyTeam))
					strOldBestPlayersNameKey = GET_PLAYER(eOldAlly).getCivilizationShortDescriptionKey();

				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_BASE_PASSED");
				strMessage << strOldBestPlayersNameKey;
			}

			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_ALLIES_STATUS");


			int iNumResourceTypes = 0;
			FStaticVector<ResourceTypes, 64, true, c_eCiv5GameplayDLL, 0> veResources;
			ResourceTypes eResource;
			ResourceUsageTypes eUsage;
			int iResourceQuantity;
#ifdef AUI_WARNING_FIXES
			for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
			for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
			{
				eResource = (ResourceTypes) iResourceLoop;
				iResourceQuantity = GetPlayer()->getNumResourceTotal(eResource);
#ifdef LEKMOD_CS_BUILDING_STRATEGIC_NO_ALLY_SHARE
				{
					const CvResourceInfo* pkResInfoQty = GC.getResourceInfo(eResource);
					if(pkResInfoQty != NULL && pkResInfoQty->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
					{
						const int iFromBuildings = GetPlayer()->getNumMinorStrategicResourceFromBuildings(eResource);
						iResourceQuantity -= iFromBuildings;
						if(iResourceQuantity < 0)
						{
							iResourceQuantity = 0;
						}
					}
				}
#endif

				if(iResourceQuantity > 0)
				{
					const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);					
					if (pkResourceInfo != NULL)
					{
						eUsage = GC.getResourceInfo(eResource)->getResourceUsage();

						if(eUsage == RESOURCEUSAGE_STRATEGIC || eUsage == RESOURCEUSAGE_LUXURY)
						{
							veResources.push_back(eResource);
							iNumResourceTypes++;
						}
					}
				}
			}

			Localization::String strResourceDetails;
			if(iNumResourceTypes == 0)
			{
				strResourceDetails = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_GAINED_BEST_RELATIONS_BONUS_NONE");
				strResourceDetails << strMinorsNameKey;
			}
			else
			{
				CvString strResourceNames = GC.getResourceInfo(veResources[0])->GetDescription();
				int i = 1;
				while(i < iNumResourceTypes)
				{
					strResourceNames += ", ";
					strResourceNames += GC.getResourceInfo(veResources[i++])->GetDescription();
				}

				strResourceDetails = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_GAINED_BEST_RELATIONS_BONUS_SOME");
				strResourceDetails << strResourceNames.c_str();
			}
			strMessage << strResourceDetails.toUTF8();
		}

		else if(bFriends)
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_BASE");
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_FRIENDS_STATUS");
		}
	}

	else
	{

		if(bAllies)
		{

			if(eNewAlly == NO_PLAYER)
			{
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_ALLIES_LOST");
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_ALLIES_STATUS_LOST");
			}

			else
			{
				CvAssertMsg(eNewAlly != NO_PLAYER, "eNewAlly not expected to be NO_PLAYER here. Please send Anton your save file and version.");
				CvAssertMsg(eNewAlly != ePlayer, "eNewAlly not expected to be same as ePlayer here. Please send Anton your save file and version.");
				const char* strNewBestPlayersNameKey = "TXT_KEY_UNMET_PLAYER";
				TeamTypes eNewAllyTeam = GET_PLAYER(eNewAlly).getTeam();
				if(pTeam->isHasMet(eNewAllyTeam))
					strNewBestPlayersNameKey = GET_PLAYER(eNewAlly).getCivilizationShortDescriptionKey();

				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_ALLIES_PASSED");
				strMessage << strNewBestPlayersNameKey;
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_ALLIES_STATUS_PASSED");
				strSummary << strNewBestPlayersNameKey;
			}
		}

		else if(bFriends)
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_FRIENDS_LOST_BASE");
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_FRIENDS_STATUS_LOST");
		}
	}

	CvString strDetailedInfo = GetStatusChangeDetails(ePlayer, bAdd, bFriends, bAllies);
	strMessage << GetPlayer()->getNameKey() << strDetailedInfo;
	strSummary << GetPlayer()->getNameKey();

	pair<CvString, CvString> notifStrings = pair<CvString, CvString>(strMessage.toUTF8(), strSummary.toUTF8());
	return notifStrings;
}


CvString CvMinorCivAI::GetNamesListAsString(CivsList veNames)
{
	CvString s = "";
	if (veNames.size() > 0)
	{
		for (uint ui = 0; ui < veNames.size(); ui++)
		{
			PlayerTypes ePlayerLoop = veNames[ui];
			CvPlayer* pPlayerLoop = &GET_PLAYER(ePlayerLoop);
			if (pPlayerLoop)
			{
				Localization::String sName = Localization::Lookup(pPlayerLoop->getCivilizationShortDescriptionKey());
				s = s + "[NEWLINE]" + sName.toUTF8();
			}
		}
	}
	return s;
}


int CvMinorCivAI::GetMajorScratchPad(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0;
	return m_aiMajorScratchPad[ePlayer];
}


void CvMinorCivAI::SetMajorScratchPad(PlayerTypes ePlayer, int iNum)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_aiMajorScratchPad[ePlayer] = iNum;
}


bool CvMinorCivAI::IsDisableNotifications() const
{
	return m_bDisableNotifications;
}


void CvMinorCivAI::SetDisableNotifications(bool bDisableNotifications)
{
	if (m_bDisableNotifications != bDisableNotifications)
	{
		m_bDisableNotifications = bDisableNotifications;
	}
}

#ifdef CS_ALLYING_WAR_RESCTRICTION

int CvMinorCivAI::GetMajorPriority(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	return m_aiMajorPriority[ePlayer];
}


void CvMinorCivAI::SetMajorPriority(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	m_aiMajorPriority[ePlayer] = iValue;
}


PlayerTypes CvMinorCivAI::GetPriorityPlayer() const
{
	PlayerTypes ePriorityPlayer = NO_PLAYER;
	for (int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		if (GetMajorPriority((PlayerTypes)iI) == 0)
		{
			ePriorityPlayer = (PlayerTypes)iI;
			break;
		}
	}

	return ePriorityPlayer;
}


int CvMinorCivAI::GetMaxMajorPriority() const
{
	int iMaxPriority = -1;
	for (int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		if (GetMajorPriority((PlayerTypes)iI) > iMaxPriority && GetMajorPriority((PlayerTypes)iI) < MAX_MAJOR_CIVS)
		{
			iMaxPriority = GetMajorPriority((PlayerTypes)iI);
		}
	}
	return iMaxPriority;
}


void CvMinorCivAI::RecalculateMajorPriority()
{
	PlayerTypes ePriorityPlayer = NO_PLAYER;
	do
	{
		ePriorityPlayer = GetPriorityPlayer();
		if (ePriorityPlayer != NO_PLAYER)
		{
			if (!(GC.getGame().getGameTurn() < GET_PLAYER(ePriorityPlayer).getPriorityTurn(GetPlayer()->GetID()) ||
				GC.getGame().getGameTurn() == GET_PLAYER(ePriorityPlayer).getPriorityTurn(GetPlayer()->GetID()) && GC.getGame().getTimeElapsed() < GET_PLAYER(ePriorityPlayer).getPriorityTime(GetPlayer()->GetID()))
				&& GetAlly() != ePriorityPlayer)
			{
				SetMajorPriority(ePriorityPlayer, MAX_MAJOR_CIVS);
				GET_PLAYER(ePriorityPlayer).setPriorityTurn(GetPlayer()->GetID(), -1);
				GET_PLAYER(ePriorityPlayer).setPriorityTime(GetPlayer()->GetID(), 0.f);

				for (int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
				{
					if (GetMajorPriority((PlayerTypes)iI) < MAX_MAJOR_CIVS)
					{
						SetMajorPriority((PlayerTypes)iI, GetMajorPriority((PlayerTypes)iI) - 1);
					}
				}
			}
		}
	} while (ePriorityPlayer != GetPriorityPlayer());
}
#endif




#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
namespace
{
MinorCivQuestTypes MinorCivPersonalityQuestTypeFromString(const char* szQuestType)
{
	struct QuestTypePair
	{
		const char* m_szType;
		MinorCivQuestTypes m_eQuest;
	};

	static const QuestTypePair kQuestTypes[] =
	{
		{ "MINOR_CIV_QUEST_ROUTE", MINOR_CIV_QUEST_ROUTE },
		{ "MINOR_CIV_QUEST_KILL_CAMP", MINOR_CIV_QUEST_KILL_CAMP },
		{ "MINOR_CIV_QUEST_CONNECT_RESOURCE", MINOR_CIV_QUEST_CONNECT_RESOURCE },
		{ "MINOR_CIV_QUEST_CONSTRUCT_WONDER", MINOR_CIV_QUEST_CONSTRUCT_WONDER },
		{ "MINOR_CIV_QUEST_GREAT_PERSON", MINOR_CIV_QUEST_GREAT_PERSON },
		{ "MINOR_CIV_QUEST_KILL_CITY_STATE", MINOR_CIV_QUEST_KILL_CITY_STATE },
		{ "MINOR_CIV_QUEST_FIND_PLAYER", MINOR_CIV_QUEST_FIND_PLAYER },
		{ "MINOR_CIV_QUEST_FIND_NATURAL_WONDER", MINOR_CIV_QUEST_FIND_NATURAL_WONDER },
		{ "MINOR_CIV_QUEST_GIVE_GOLD", MINOR_CIV_QUEST_GIVE_GOLD },
		{ "MINOR_CIV_QUEST_PLEDGE_TO_PROTECT", MINOR_CIV_QUEST_PLEDGE_TO_PROTECT },
		{ "MINOR_CIV_QUEST_CONTEST_CULTURE", MINOR_CIV_QUEST_CONTEST_CULTURE },
		{ "MINOR_CIV_QUEST_CONTEST_FAITH", MINOR_CIV_QUEST_CONTEST_FAITH },
		{ "MINOR_CIV_QUEST_CONTEST_TECHS", MINOR_CIV_QUEST_CONTEST_TECHS },
		{ "MINOR_CIV_QUEST_INVEST", MINOR_CIV_QUEST_INVEST },
		{ "MINOR_CIV_QUEST_BULLY_CITY_STATE", MINOR_CIV_QUEST_BULLY_CITY_STATE },
		{ "MINOR_CIV_QUEST_DENOUNCE_MAJOR", MINOR_CIV_QUEST_DENOUNCE_MAJOR },
		{ "MINOR_CIV_QUEST_SPREAD_RELIGION", MINOR_CIV_QUEST_SPREAD_RELIGION },
		{ "MINOR_CIV_QUEST_TRADE_ROUTE", MINOR_CIV_QUEST_TRADE_ROUTE },
	};

	if(szQuestType == NULL)
		return NO_MINOR_CIV_QUEST_TYPE;

	for(uint i = 0; i < sizeof(kQuestTypes) / sizeof(kQuestTypes[0]); i++)
	{
		if(_stricmp(szQuestType, kQuestTypes[i].m_szType) == 0)
			return kQuestTypes[i].m_eQuest;
	}

	return NO_MINOR_CIV_QUEST_TYPE;
}
}

CvMinorCivPersonalityInfo::CvMinorCivPersonalityInfo() :
	m_iFriendshipDropPerTurn(0),
	m_iGlobalQuestRandTurnsMultiplier(100),
	m_iPersonalQuestRandTurnsMultiplier(100),
	m_iBullyScoreModifier(0),
	m_bBlocksKillCityStateQuest(false),
	m_bHostileOnlyBullyQuestTarget(false),
	m_iAIGiftWeightModifier(0),
	m_bDisabled(false),
	m_iFriendshipDecayModifierPercent(100),
	m_iFriendshipRecoveryModifierPercent(100),
	m_iSharedReligionDecayRecoveryModifierPercent(100),
	m_iDifferentReligionDecayRecoveryModifierPercent(0),
	m_iQuestInfluenceModifierPercent(100),
	m_bStripPersonalityBonusesIfAttackedMinor(false),
	m_iFirstMeetGoldModifier(0),
	m_iTradeRouteGoldModifierPercent(100),
	m_iGoldGiftInfluenceModifierPercent(100),
	m_iTributeGoldModifierPercent(100),
	m_bNoGifts(false),
	m_bBlocksPledgeToProtect(false),
	m_bBlocksTribute(false),
	m_bBlocksQuests(false),
	m_bBlocksWarDeclarationPenalty(false),
	m_bNeverAlliedWarSupport(false),
	m_iUnitSpawnModifierPercent(100),
	m_bSpawnNavalUnits(false),
	m_iRequiredMinorCivTrait(NO_MINOR_CIV_TRAIT_TYPE),
	m_iForbiddenMinorCivTrait(NO_MINOR_CIV_TRAIT_TYPE),
	m_bRequiresCoastal(false),
	m_iMaxGlobalCount(0),
	m_iMaxGlobalCountDivisor(0),
	m_iTransformsAtEra(NO_ERA),
	m_iReligionSpreadInfluenceBurst(0),
	m_bIgnoreBulliedForGoldQuest(false),
	m_piQuestWeightMultipliers(NULL)
{
}

CvMinorCivPersonalityInfo::~CvMinorCivPersonalityInfo()
{
	SAFE_DELETE_ARRAY(m_piQuestWeightMultipliers);
}

int CvMinorCivPersonalityInfo::GetFriendshipDropPerTurn() const
{
	return m_iFriendshipDropPerTurn;
}

int CvMinorCivPersonalityInfo::GetGlobalQuestRandTurnsMultiplier() const
{
	return m_iGlobalQuestRandTurnsMultiplier;
}

int CvMinorCivPersonalityInfo::GetPersonalQuestRandTurnsMultiplier() const
{
	return m_iPersonalQuestRandTurnsMultiplier;
}

int CvMinorCivPersonalityInfo::GetBullyScoreModifier() const
{
	return m_iBullyScoreModifier;
}

bool CvMinorCivPersonalityInfo::IsBlocksKillCityStateQuest() const
{
	return m_bBlocksKillCityStateQuest;
}

bool CvMinorCivPersonalityInfo::IsHostileOnlyBullyQuestTarget() const
{
	return m_bHostileOnlyBullyQuestTarget;
}

int CvMinorCivPersonalityInfo::GetAIGiftWeightModifier() const
{
	return m_iAIGiftWeightModifier;
}

bool CvMinorCivPersonalityInfo::IsDisabled() const
{
	return m_bDisabled;
}

int CvMinorCivPersonalityInfo::GetFriendshipDecayModifierPercent() const
{
	return m_iFriendshipDecayModifierPercent;
}

int CvMinorCivPersonalityInfo::GetFriendshipRecoveryModifierPercent() const
{
	return m_iFriendshipRecoveryModifierPercent;
}

int CvMinorCivPersonalityInfo::GetSharedReligionDecayRecoveryModifierPercent() const
{
	return m_iSharedReligionDecayRecoveryModifierPercent;
}

int CvMinorCivPersonalityInfo::GetDifferentReligionDecayRecoveryModifierPercent() const
{
	return m_iDifferentReligionDecayRecoveryModifierPercent;
}

int CvMinorCivPersonalityInfo::GetQuestInfluenceModifierPercent() const
{
	return m_iQuestInfluenceModifierPercent;
}

bool CvMinorCivPersonalityInfo::IsStripPersonalityBonusesIfAttackedMinor() const
{
	return m_bStripPersonalityBonusesIfAttackedMinor;
}

int CvMinorCivPersonalityInfo::GetFirstMeetGoldModifier() const
{
	return m_iFirstMeetGoldModifier;
}

int CvMinorCivPersonalityInfo::GetTradeRouteGoldModifierPercent() const
{
	return m_iTradeRouteGoldModifierPercent;
}

int CvMinorCivPersonalityInfo::GetGoldGiftInfluenceModifierPercent() const
{
	return m_iGoldGiftInfluenceModifierPercent;
}

int CvMinorCivPersonalityInfo::GetTributeGoldModifierPercent() const
{
	return m_iTributeGoldModifierPercent;
}

bool CvMinorCivPersonalityInfo::IsNoGifts() const
{
	return m_bNoGifts;
}

bool CvMinorCivPersonalityInfo::IsBlocksPledgeToProtect() const
{
	return m_bBlocksPledgeToProtect;
}

bool CvMinorCivPersonalityInfo::IsBlocksTribute() const
{
	return m_bBlocksTribute;
}

bool CvMinorCivPersonalityInfo::IsBlocksQuests() const
{
	return m_bBlocksQuests;
}

bool CvMinorCivPersonalityInfo::IsBlocksWarDeclarationPenalty() const
{
	return m_bBlocksWarDeclarationPenalty;
}

bool CvMinorCivPersonalityInfo::IsNeverAlliedWarSupport() const
{
	return m_bNeverAlliedWarSupport;
}

int CvMinorCivPersonalityInfo::GetUnitSpawnModifierPercent() const
{
	return m_iUnitSpawnModifierPercent;
}

bool CvMinorCivPersonalityInfo::IsSpawnNavalUnits() const
{
	return m_bSpawnNavalUnits;
}

int CvMinorCivPersonalityInfo::GetRequiredMinorCivTrait() const
{
	return m_iRequiredMinorCivTrait;
}

int CvMinorCivPersonalityInfo::GetForbiddenMinorCivTrait() const
{
	return m_iForbiddenMinorCivTrait;
}

bool CvMinorCivPersonalityInfo::RequiresCoastal() const
{
	return m_bRequiresCoastal;
}

int CvMinorCivPersonalityInfo::GetMaxGlobalCount() const
{
	return m_iMaxGlobalCount;
}

int CvMinorCivPersonalityInfo::GetMaxGlobalCountDivisor() const
{
	return m_iMaxGlobalCountDivisor;
}

int CvMinorCivPersonalityInfo::GetTransformsAtEra() const
{
	return m_iTransformsAtEra;
}

int CvMinorCivPersonalityInfo::GetReligionSpreadInfluenceBurst() const
{
	return m_iReligionSpreadInfluenceBurst;
}

bool CvMinorCivPersonalityInfo::IsIgnoreBulliedForGoldQuest() const
{
	return m_bIgnoreBulliedForGoldQuest;
}

int CvMinorCivPersonalityInfo::GetQuestWeightMultiplier(MinorCivQuestTypes eQuest) const
{
	if(m_piQuestWeightMultipliers == NULL || eQuest < 0 || eQuest >= NUM_MINOR_CIV_QUEST_TYPES)
		return 100;

	return m_piQuestWeightMultipliers[eQuest];
}

void CvMinorCivPersonalityInfo::ApplyFlavorChanges(CvFlavorManager* pFlavorManager) const
{
	if(pFlavorManager == NULL || m_vFlavorChanges.empty())
		return;

	int* pFlavors = pFlavorManager->GetAllPersonalityFlavors();
	for(std::vector<std::pair<FlavorTypes, int> >::const_iterator it = m_vFlavorChanges.begin(); it != m_vFlavorChanges.end(); ++it)
	{
		const FlavorTypes eFlavor = it->first;
		const int iChange = it->second;
		if(eFlavor != NO_FLAVOR && eFlavor < GC.getNumFlavorTypes())
		{
			pFlavors[eFlavor] = pFlavorManager->GetAdjustedValue(pFlavors[eFlavor], iChange, 0, 10);
		}
	}

	pFlavorManager->ResetToBasePersonality();
}

bool CvMinorCivPersonalityInfo::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	m_iFriendshipDropPerTurn = kResults.GetInt("FriendshipDropPerTurn");
	m_iGlobalQuestRandTurnsMultiplier = kResults.GetInt("GlobalQuestRandTurnsMultiplier");
	m_iPersonalQuestRandTurnsMultiplier = kResults.GetInt("PersonalQuestRandTurnsMultiplier");
	m_iBullyScoreModifier = kResults.GetInt("BullyScoreModifier");
	m_bBlocksKillCityStateQuest = kResults.GetBool("BlocksKillCityStateQuest");
	m_bHostileOnlyBullyQuestTarget = kResults.GetBool("HostileOnlyBullyQuestTarget");
	m_iAIGiftWeightModifier = kResults.GetInt("AIGiftWeightModifier");
	m_bDisabled = kResults.GetBool("Disabled");

	m_iFriendshipDecayModifierPercent = kResults.GetInt("FriendshipDecayModifierPercent");
	m_iFriendshipRecoveryModifierPercent = kResults.GetInt("FriendshipRecoveryModifierPercent");
	m_iSharedReligionDecayRecoveryModifierPercent = kResults.GetInt("SharedReligionDecayRecoveryModifierPercent");
	m_iDifferentReligionDecayRecoveryModifierPercent = kResults.GetInt("DifferentReligionDecayRecoveryModifierPercent");
	m_iQuestInfluenceModifierPercent = kResults.GetInt("QuestInfluenceModifierPercent");
	m_bStripPersonalityBonusesIfAttackedMinor = kResults.GetBool("StripPersonalityBonusesIfAttackedMinor");
	m_iFirstMeetGoldModifier = kResults.GetInt("FirstMeetGoldModifier");
	m_iTradeRouteGoldModifierPercent = kResults.GetInt("TradeRouteGoldModifierPercent");
	m_iGoldGiftInfluenceModifierPercent = kResults.GetInt("GoldGiftInfluenceModifierPercent");
	m_iTributeGoldModifierPercent = kResults.GetInt("TributeGoldModifierPercent");
	m_bNoGifts = kResults.GetBool("NoGifts");
	m_bBlocksPledgeToProtect = kResults.GetBool("BlocksPledgeToProtect");
	m_bBlocksTribute = kResults.GetBool("BlocksTribute");
	m_bBlocksQuests = kResults.GetBool("BlocksQuests");
	m_bBlocksWarDeclarationPenalty = kResults.GetBool("BlocksWarDeclarationPenalty");
	m_bNeverAlliedWarSupport = kResults.GetBool("NeverAlliedWarSupport");
	m_iUnitSpawnModifierPercent = kResults.GetInt("UnitSpawnModifierPercent");
	m_bSpawnNavalUnits = kResults.GetBool("SpawnNavalUnits");
	m_iRequiredMinorCivTrait = GC.getInfoTypeForString(kResults.GetText("RequiredMinorCivTrait"), true);
	m_iForbiddenMinorCivTrait = GC.getInfoTypeForString(kResults.GetText("ForbiddenMinorCivTrait"), true);
	m_bRequiresCoastal = kResults.GetBool("RequiresCoastal");
	m_iMaxGlobalCount = kResults.GetInt("MaxGlobalCount");
	m_iMaxGlobalCountDivisor = kResults.GetInt("MaxGlobalCountDivisor");
	m_iTransformsAtEra = GC.getInfoTypeForString(kResults.GetText("TransformsAtEra"), true);
	m_iReligionSpreadInfluenceBurst = kResults.GetInt("ReligionSpreadInfluenceBurst");
	m_bIgnoreBulliedForGoldQuest = kResults.GetBool("IgnoreBulliedForGoldQuest");

	if(m_iFriendshipDecayModifierPercent == 0)
		m_iFriendshipDecayModifierPercent = 100;
	if(m_iFriendshipRecoveryModifierPercent == 0)
		m_iFriendshipRecoveryModifierPercent = 100;
	if(m_iSharedReligionDecayRecoveryModifierPercent == 0)
		m_iSharedReligionDecayRecoveryModifierPercent = 100;
	if(m_iQuestInfluenceModifierPercent == 0)
		m_iQuestInfluenceModifierPercent = 100;
	if(m_iTradeRouteGoldModifierPercent == 0)
		m_iTradeRouteGoldModifierPercent = 100;
	if(m_iGoldGiftInfluenceModifierPercent == 0)
		m_iGoldGiftInfluenceModifierPercent = 100;
	if(m_iTributeGoldModifierPercent == 0)
		m_iTributeGoldModifierPercent = 100;
	if(m_iUnitSpawnModifierPercent == 0)
		m_iUnitSpawnModifierPercent = 100;

	SAFE_DELETE_ARRAY(m_piQuestWeightMultipliers);
	m_piQuestWeightMultipliers = FNEW(int[NUM_MINOR_CIV_QUEST_TYPES], c_eCiv5GameplayDLL, 0);
	for(int iQuestLoop = 0; iQuestLoop < NUM_MINOR_CIV_QUEST_TYPES; iQuestLoop++)
	{
		m_piQuestWeightMultipliers[iQuestLoop] = 100;
	}

	const char* szType = GetType();
	{
		std::string strKey = "Minor_Civ_Personality - QuestWeights";
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select QuestType, WeightMultiplier from Minor_Civ_Personality_QuestWeights where PersonalityType = ?");

		pResults->Bind(1, szType, -1, false);

		while(pResults->Step())
		{
			const MinorCivQuestTypes eQuest = MinorCivPersonalityQuestTypeFromString(pResults->GetText(0));
			if(eQuest != NO_MINOR_CIV_QUEST_TYPE)
			{
				m_piQuestWeightMultipliers[eQuest] = pResults->GetInt(1);
			}
		}

		pResults->Reset();
	}

	m_vFlavorChanges.clear();
	{
		std::string strKey = "Minor_Civ_Personality - Flavors";
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select FlavorType, Flavor from Minor_Civ_Personality_Flavors where PersonalityType = ?");

		pResults->Bind(1, szType, -1, false);

		while(pResults->Step())
		{
			const FlavorTypes eFlavor = (FlavorTypes)GC.getInfoTypeForString(pResults->GetText(0), true);
			if(eFlavor != NO_FLAVOR)
			{
				m_vFlavorChanges.push_back(std::make_pair(eFlavor, pResults->GetInt(1)));
			}
		}

		pResults->Reset();
	}

	return true;
}
#endif




CvMinorCivInfo::CvMinorCivInfo() :
	m_iDefaultPlayerColor(NO_PLAYERCOLOR),
	m_iArtStyleType(NO_ARTSTYLE),
	m_iMinorCivTrait(NO_MINOR_CIV_TRAIT_TYPE),
#ifdef AUI_WARNING_FIXES
	m_iNumLeaders(0),
	m_bAIPlayable(false),
	m_bPlayable(false),
#endif
	m_piFlavorValue(NULL)
{
}

CvMinorCivInfo::~CvMinorCivInfo()
{
	SAFE_DELETE_ARRAY(m_piFlavorValue);
}

int CvMinorCivInfo::getDefaultPlayerColor() const
{
	return m_iDefaultPlayerColor;
}

int CvMinorCivInfo::getArtStyleType() const
{
	return m_iArtStyleType;
}

const char* CvMinorCivInfo::getArtStylePrefix() const
{
	return m_strArtStylePrefix.c_str();
}

const char* CvMinorCivInfo::getArtStyleSuffix() const
{
	return m_strArtStyleSuffix.c_str();
}

int CvMinorCivInfo::getNumCityNames() const
{
	return m_vCityNames.size();
}

const char* CvMinorCivInfo::getShortDescription() const
{
	return m_wstrShortDescription.c_str();
}

const char* CvMinorCivInfo::getShortDescriptionKey() const
{
	return m_strShortDescriptionKey.c_str();
}

const char* CvMinorCivInfo::getShortDescriptionKeyWide() const
{
	return m_wstrShortDescriptionKey.c_str();
}

void CvMinorCivInfo::setShortDescriptionKey(const char* szVal)
{
	m_strShortDescriptionKey = szVal;
	m_wstrShortDescriptionKey = szVal;
	m_wstrShortDescription = GetLocalizedText(m_strShortDescriptionKey.c_str());
}

const char* CvMinorCivInfo::getAdjective()	const
{
	return m_wstrAdjective.c_str();
}

const char* CvMinorCivInfo::getAdjectiveKey() const
{
	return m_strAdjectiveKey.c_str();
}

const char* CvMinorCivInfo::getAdjectiveKeyWide() const
{
	return m_wstrAdjectiveKey.c_str();
}

void CvMinorCivInfo::setAdjectiveKey(const char* szVal)
{
	m_strAdjectiveKey = szVal;
	m_wstrAdjectiveKey = szVal;
	m_wstrAdjective = GetLocalizedText(m_strAdjectiveKey.c_str());
}

const char* CvMinorCivInfo::getFlagTexture() const
{
	return NULL;
}

const char* CvMinorCivInfo::getArtDefineTag() const
{
	return m_strArtDefineTag;
}

void CvMinorCivInfo::setArtDefineTag(const char* szVal)
{
	m_strArtDefineTag = szVal;
}

int CvMinorCivInfo::GetMinorCivTrait() const
{
	return m_iMinorCivTrait;
}

int CvMinorCivInfo::getFlavorValue(int i) const
{
	CvAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}

const std::string& CvMinorCivInfo::getCityNames(int i) const
{
	return m_vCityNames[i];
}

void CvMinorCivInfo::setArtStylePrefix(const char* szVal)
{
	m_strArtStylePrefix = szVal;
}

void CvMinorCivInfo::setArtStyleSuffix(const char* szVal)
{
	m_strArtStyleSuffix = szVal;
}

bool CvMinorCivInfo::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;


	const char* szTextVal = NULL;

	szTextVal = kResults.GetText("ShortDescription");
	setShortDescriptionKey(szTextVal);

	szTextVal = kResults.GetText("Adjective");
	setAdjectiveKey(szTextVal);

	szTextVal = kResults.GetText("ArtDefineTag");
	setArtDefineTag(szTextVal);


	szTextVal = kResults.GetText("DefaultPlayerColor");
	m_iDefaultPlayerColor = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("ArtStyleType");
	m_iArtStyleType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("ArtStylePrefix");
	setArtStylePrefix(szTextVal);

	szTextVal = kResults.GetText("ArtStyleSuffix");
	setArtStyleSuffix(szTextVal);

	szTextVal = kResults.GetText("MinorCivTrait");
	m_iMinorCivTrait = GC.getInfoTypeForString(szTextVal, true);


	const char* szType = GetType();
	kUtility.SetFlavors(m_piFlavorValue, "MinorCivilization_Flavors", "MinorCivType", szType, -1);


	{
		m_vCityNames.clear();

		std::string strKey = "MinorCiv - CityName";
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select CityName from MinorCivilization_CityNames where MinorCivType = ?");

		pResults->Bind(1, szType, -1, false);

		while(pResults->Step())
		{
			m_vCityNames.push_back(pResults->GetText(0));
		}

		pResults->Reset();
	}

	return true;
}

FDataStream& operator<<(FDataStream& saveTo, const MinorCivStatusTypes& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, MinorCivStatusTypes& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<MinorCivStatusTypes>(v);
	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const MinorCivPersonalityTypes& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, MinorCivPersonalityTypes& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<MinorCivPersonalityTypes>(v);
	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const MinorCivQuestTypes& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, MinorCivQuestTypes& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<MinorCivQuestTypes>(v);
	return loadFrom;
}
