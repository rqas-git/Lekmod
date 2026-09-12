







#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreUtils.h"
#include "CvInternalGameCoreUtils.h"
#include "CvGame.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvPlayerAI.h"
#include "CvRandom.h"
#include "CvTeam.h"
#include "CvGlobals.h"
#include "CvMapGenerator.h"
#include "CvReplayMessage.h"
#include "CvInfos.h"
#include "CvReplayInfo.h"
#include "CvGameTextMgr.h"
#include "CvSiteEvaluationClasses.h"
#include "CvImprovementClasses.h"
#include "CvStartPositioner.h"
#include "CvTacticalAnalysisMap.h"
#include "CvGrandStrategyAI.h"
#include "CvMinorCivAI.h"
#include "CvDiplomacyAI.h"
#include "CvNotifications.h"
#include "CvAdvisorCounsel.h"
#include "CvAdvisorRecommender.h"
#include "CvWorldBuilderMapLoader.h"
#include "CvTypes.h"

#include "cvStopWatch.h"
#include "CvUnitMission.h"

#include "CvDLLUtilDefines.h"
#include "CvAchievementUnlocker.h"


#include "ICvDLLUserInterface.h"
#include "CvEnumSerialization.h"
#include "FStlContainerSerialization.h"
#include "FAutoVariableBase.h"
#include "CvStringUtils.h"
#include "CvBarbarians.h"
#include "CvGoodyHuts.h"

#include <sstream>

#include "FTempHeap.h"
#include "CvDiplomacyRequests.h"

#include "CvDllPlot.h"
#include "FFileSystem.h"

#include "CvInfosSerializationHelper.h"
#include "CvCityManager.h"

#if defined (DEV_RECORDING_STATISTICS) || defined (REPLAY_EVENTS)
# include <winsqlite3.h>
# pragma comment(lib, "winsqlite3.lib")
#endif



#include "LintFree.h"






const int g_CurrentCvGameVersion = 1;


CvGameInitialItemsOverrides::CvGameInitialItemsOverrides()
{


	GrantInitialFreeTechsPerTeam.resize(MAX_TEAMS, true);
	GrantInitialGoldPerPlayer.resize(MAX_PLAYERS, true);
	GrantInitialCulturePerPlayer.resize(MAX_PLAYERS, true);
	ClearResearchQueuePerPlayer.resize(MAX_PLAYERS, true);
	GrantInitialUnitsPerPlayer.resize(MAX_PLAYERS, true);
}


CvGame::CvGame() :
	m_jonRand(false)
	, m_endTurnTimer()
	, m_endTurnTimerSemaphore(0)
	, m_curTurnTimer()
#ifndef AUI_GAME_PLAYER_BASED_TURN_LENGTH
	, m_timeSinceGameTurnStart()
#endif
	, m_fCurrentTurnTimerPauseDelta(0.f)
#ifdef TURN_TIMER_PAUSE_BUTTON
	, m_bIsPaused(false)
#endif
	, m_sentAutoMoves(false)
	, m_bForceEndingTurn(false)
	, m_pDiploResponseQuery(NULL)
	, m_bFOW(true)
	, m_bArchaeologyTriggered(false)
	, m_lastTurnAICivsProcessed(-1)
{
	m_aiEndTurnMessagesReceived = FNEW(int[MAX_PLAYERS], c_eCiv5GameplayDLL, 0);
	m_aiRankPlayer = FNEW(int[MAX_PLAYERS], c_eCiv5GameplayDLL, 0);
	m_aiPlayerRank = FNEW(int[MAX_PLAYERS], c_eCiv5GameplayDLL, 0);
	m_aiPlayerScore = FNEW(int[MAX_PLAYERS], c_eCiv5GameplayDLL, 0);
	m_aiRankTeam = FNEW(int[MAX_TEAMS], c_eCiv5GameplayDLL, 0);
	m_aiTeamRank = FNEW(int[MAX_TEAMS], c_eCiv5GameplayDLL, 0);
	m_aiTeamScore = FNEW(int[MAX_TEAMS], c_eCiv5GameplayDLL, 0);

	m_paiUnitCreatedCount = NULL;
	m_paiUnitClassCreatedCount = NULL;
	m_paiBuildingClassCreatedCount = NULL;
	m_paiProjectCreatedCount = NULL;
	m_paiVoteOutcome = NULL;
	m_aiSecretaryGeneralTimer = NULL;
	m_aiVoteTimer = NULL;
	m_aiDiploVote = NULL;
	m_aiVotesCast = NULL;
	m_aiPreviousVotesCast = NULL;
	m_aiNumVotesForTeam = NULL;
	m_aiTeamCompetitionWinnersScratchPad = NULL;

	m_pabSpecialUnitValid = NULL;

	m_ppaaiTeamVictoryRank = NULL;

	m_pSettlerSiteEvaluator = NULL;
	m_pStartSiteEvaluator = NULL;
	m_pStartPositioner = NULL;
	m_pGameReligions = NULL;
	m_pGameCulture = NULL;
	m_pGameLeagues = NULL;
	m_pGameTrade = NULL;
	m_pTacticalMap = NULL;
#ifdef MP_PLAYERS_VOTING_SYSTEM
	m_pMPVotingSystem = NULL;
#endif
	m_pAdvisorCounsel = NULL;
	m_pAdvisorRecommender = NULL;

	m_endTurnTimer.Start();
	m_endTurnTimer.Stop();

	reset(NO_HANDICAP, true);
}



CvGame::~CvGame()
{
	uninit();

	SAFE_DELETE_ARRAY(m_aiEndTurnMessagesReceived);
	SAFE_DELETE_ARRAY(m_aiRankPlayer);
	SAFE_DELETE_ARRAY(m_aiPlayerRank);
	SAFE_DELETE_ARRAY(m_aiPlayerScore);
	SAFE_DELETE_ARRAY(m_aiRankTeam);
	SAFE_DELETE_ARRAY(m_aiTeamRank);
	SAFE_DELETE_ARRAY(m_aiTeamScore);
}


void CvGame::init(HandicapTypes eHandicap)
{
	bool bValid;
	int iStartTurn;
	int iEstimateEndTurn;
#ifdef AUI_WARNING_FIXES
	uint iI;
#else
	int iI;
#endif



	reset(eHandicap);



	m_voteSelections.Init();
	m_votesTriggered.Init();

	if(!isGameMultiPlayer())
	{
		m_jonRand.setCallStackDebuggingEnabled(false);
	}

	m_mapRand.init(CvPreGame::mapRandomSeed() % 73637381);
	m_jonRand.init(CvPreGame::syncRandomSeed() % 52319761);





	if(!GC.getEraInfo(getStartEra())){
		for(int eraIdx = ((int)CvPreGame::era())-1; eraIdx >= 0; --eraIdx){
			CvEraInfo* curEraInfo = GC.getEraInfo((EraTypes)eraIdx);
			if(curEraInfo){
				CvPreGame::setEra((EraTypes)eraIdx);
				break;
			}
		}
	}








	GameTypes g = CvPreGame::gameType();
	if(g == GAME_SINGLE_PLAYER)
	{
		for(iI = 0; iI < NUM_MPOPTION_TYPES; ++iI)
		{
			setMPOption((MultiplayerOptionTypes)iI, false);
		}

		setOption(GAMEOPTION_SIMULTANEOUS_TURNS, false);
		setOption(GAMEOPTION_DYNAMIC_TURNS, false);
		setOption(GAMEOPTION_PITBOSS, false);
	}


	if(isHotSeat() || isPbem())
	{
		setOption(GAMEOPTION_SIMULTANEOUS_TURNS, false);
		setOption(GAMEOPTION_DYNAMIC_TURNS, false);
		setOption(GAMEOPTION_PITBOSS, false);
	}

	if(isMPOption(MPOPTION_SHUFFLE_TEAMS))
	{
		int aiTeams[MAX_CIV_PLAYERS];

		int iNumPlayers = 0;
		for(int i = 0; i < MAX_CIV_PLAYERS; i++)
		{
			if(CvPreGame::slotStatus((PlayerTypes)i) == SS_TAKEN)
			{
				aiTeams[iNumPlayers] = CvPreGame::teamType((PlayerTypes)i);
				++iNumPlayers;
			}
		}

		for(int i = 0; i < iNumPlayers; i++)
		{
			int j = (getJonRand().get(iNumPlayers - i, NULL) + i);

			if(i != j)
			{
				int iTemp = aiTeams[i];
				aiTeams[i] = aiTeams[j];
				aiTeams[j] = iTemp;
			}
		}

		iNumPlayers = 0;
		for(int i = 0; i < MAX_CIV_PLAYERS; i++)
		{
			if(CvPreGame::slotStatus((PlayerTypes)i) == SS_TAKEN)
			{
				CvPreGame::setTeamType((PlayerTypes)i, (TeamTypes)aiTeams[iNumPlayers]);
				++iNumPlayers;
			}
		}
	}

	if(isOption(GAMEOPTION_LOCK_MODS))
	{
		if(isGameMultiPlayer())
		{
			setOption(GAMEOPTION_LOCK_MODS, false);
		}
		else
		{
			static const int iPasswordSize = 8;
			char szRandomPassword[iPasswordSize];
			for(int i = 0; i < iPasswordSize-1; i++)
			{
#ifdef AUI_WARNING_FIXES
				szRandomPassword[i] = char(getJonRandNum(128, "Random Keyword"));
#else
				szRandomPassword[i] = getJonRandNum(128, "Random Keyword");
#endif
			}
			szRandomPassword[iPasswordSize-1] = 0;

			CvString strRandomPassword = szRandomPassword;
			CvPreGame::setAdminPassword(strRandomPassword);
		}
	}

	CvGameSpeedInfo& kGameSpeedInfo = getGameSpeedInfo();
	if(getGameTurn() == 0)
	{
		iStartTurn = 0;

#ifdef AUI_WARNING_FIXES
		for (int iJ = 0; iJ < kGameSpeedInfo.getNumTurnIncrements(); iJ++)
		{
			iStartTurn += kGameSpeedInfo.getGameTurnInfo(iJ).iNumGameTurnsPerIncrement;
#else
		for(iI = 0; iI < kGameSpeedInfo.getNumTurnIncrements(); iI++)
		{
			iStartTurn += kGameSpeedInfo.getGameTurnInfo(iI).iNumGameTurnsPerIncrement;
#endif
		}

		CvEraInfo& kEraInfo = getStartEraInfo();

		iStartTurn *= kEraInfo.getStartPercent();
		iStartTurn /= 100;

		setGameTurn(iStartTurn);

		if (kEraInfo.isNoReligion())
		{
#ifdef AUI_WARNING_FIXES
			CvPreGame::SetGameOption(GAMEOPTION_NO_RELIGION, 1);
#else
			CvPreGame::SetGameOption(GAMEOPTION_NO_RELIGION, true);
#endif
		}
	}

	setStartTurn(getGameTurn());

	iEstimateEndTurn = 0;

#ifdef AUI_WARNING_FIXES
	for (int iJ = 0; iJ < kGameSpeedInfo.getNumTurnIncrements(); iJ++)
	{
		iEstimateEndTurn += kGameSpeedInfo.getGameTurnInfo(iJ).iNumGameTurnsPerIncrement;
#else
	for(iI = 0; iI < kGameSpeedInfo.getNumTurnIncrements(); iI++)
	{
		iEstimateEndTurn += kGameSpeedInfo.getGameTurnInfo(iI).iNumGameTurnsPerIncrement;
#endif
	}

	setDefaultEstimateEndTurn(iEstimateEndTurn);

	if(getMaxTurns() == 0)
	{

		setEstimateEndTurn(iEstimateEndTurn);

		if(getEstimateEndTurn() > getGameTurn())
		{
			bValid = false;

			for(iI = 0; iI < GC.getNumVictoryInfos(); iI++)
			{
				VictoryTypes eVictory = static_cast<VictoryTypes>(iI);
				CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eVictory);
				if(pkVictoryInfo)
				{
					if(isVictoryValid(eVictory))
					{
						if(pkVictoryInfo->isEndScore())
						{
							bValid = true;
							break;
						}
					}
				}

			}

			if(bValid)
			{
				setMaxTurns(getEstimateEndTurn() - getGameTurn());
			}
		}
	}
	else
	{
		setEstimateEndTurn(getGameTurn() + getMaxTurns());
	}

	setStartYear(GC.getSTART_YEAR());

	for(iI = 0; iI < GC.getNumSpecialUnitInfos(); iI++)
	{
		SpecialUnitTypes eSpecialUnit = static_cast<SpecialUnitTypes>(iI);
		CvSpecialUnitInfo* pkSpecialUnitInfo = GC.getSpecialUnitInfo(eSpecialUnit);
		if(pkSpecialUnitInfo)
		{
			if(pkSpecialUnitInfo->isValid())
			{
				makeSpecialUnitValid(eSpecialUnit);
			}
		}
	}

	if(isOption(GAMEOPTION_QUICK_COMBAT))
	{
		CvPreGame::setQuickCombat(true);
	}

	m_bArchaeologyTriggered = false;
	CvGoodyHuts::Reset();

#ifdef AUI_GAME_BETTER_HYBRID_MODE
	m_iCurrentTurnOrderActive = 0;
	m_iLastTurnOrderID = 0;
#ifdef AUI_GAME_PLAYER_BASED_TURN_LENGTH
	m_aiMaxTurnLengths.clear();
	m_aiMaxTurnLengths.reserve(MAX_TEAMS);
#endif
#endif
#ifdef GAME_ALLOW_ONLY_ONE_UNIT_MOVE_ON_TURN_LOADING
	if (GC.getGame().isOption("GAMEOPTION_FIRSTMOVE"))
	{
		setMPOrderedMoveOnTurnLoading(false);
	}
#endif
	doUpdateCacheOnTurn();
}


bool CvGame::init2()
{
	InitPlayers();

	CvGameInitialItemsOverrides kItemOverrides;
	if(!InitMap(kItemOverrides))
	{
		return false;
	}

	initDiplomacy();
	setInitialItems(kItemOverrides);

	if(CvPreGame::isWBMapScript() && !CvPreGame::mapNoPlayers())
	{
		CvWorldBuilderMapLoader::SetInitialItems(false);
	}

	CheckGenerateArchaeology();

	initScoreCalculation();
	setFinalInitialized(true);

	return true;
}





int WorldBuilderMapLoaderAddRandomItems(lua_State* L)
{
	return CvWorldBuilderMapLoader::AddRandomItems(L);
}

int WorldBuilderMapLoaderLoadModData(lua_State* L)
{
	return CvWorldBuilderMapLoader::LoadModData(L);
}

int WorldBuilderMapLoaderRunPostProcessScript(lua_State* L)
{
	return CvWorldBuilderMapLoader::RunPostProcessScript(L);
}

bool CvGame::InitMap(CvGameInitialItemsOverrides& kGameInitialItemsOverrides)
{
	CvMap& kMap = GC.getMap();
	const bool bWBSave = CvPreGame::isWBMapScript();
	if(bWBSave)
	{
		const CvWorldBuilderMapLoaderMapInfo& kWBMapInfo =
		    CvWorldBuilderMapLoader::GetCurrentMapInfo();

		if(kWBMapInfo.uiWidth * kWBMapInfo.uiHeight != 0)
		{
			CvMapInitData kMapInitData;
			kMapInitData.m_bWrapX = kWBMapInfo.bWorldWrap;
			kMapInitData.m_bWrapY = false;
			kMapInitData.m_iGridW = kWBMapInfo.uiWidth;
			kMapInitData.m_iGridH = kWBMapInfo.uiHeight;
			kMap.init(&kMapInitData);

			CvBarbarians::MapInit(kMap.numPlots());

			CvWorldBuilderMapLoader::InitMap();
			CvWorldBuilderMapLoader::ValidateTerrain();

			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if(pkScriptSystem != NULL)
			{
				lua_State* L = pkScriptSystem->CreateLuaThread("WorldBuilderMapLoader");
				if(L != NULL)
				{
					lua_cpcall(L, &WorldBuilderMapLoaderAddRandomItems, 0);
					lua_cpcall(L, &WorldBuilderMapLoaderLoadModData, 0);
					lua_cpcall(L, &WorldBuilderMapLoaderRunPostProcessScript, 0);

					pkScriptSystem->FreeLuaThread(L);
				}
			}
		}
		else
		{

			FAssertMsg(0, "Empty World Builder Map!");


			CvMapInitData kMapInitData;
			kMapInitData.m_bWrapX = false;
			kMapInitData.m_bWrapY = false;
			kMapInitData.m_iGridW = 1;
			kMapInitData.m_iGridH = 1;
			kMap.init(&kMapInitData);

			CvBarbarians::MapInit(kMap.numPlots());

			CvWorldBuilderMapLoader::ValidateTerrain();
		}
	}
	else
	{
		const CvString& strMapName = CvPreGame::mapScriptName();

		CvMapGenerator* pGenerator = FNEW(CvMapGenerator(strMapName.c_str()), c_eMPoolTypeGame, 0);

		CvMapInitData kData;
		if(pGenerator->GetMapInitData(kData, CvPreGame::worldSize()))
			kMap.init(&kData);
		else
			kMap.init();

		CvBarbarians::MapInit(kMap.numPlots());

		pGenerator->GenerateRandomMap();
		pGenerator->GetGameInitialItemsOverrides(kGameInitialItemsOverrides);

		delete pGenerator;
	}

#ifdef AUI_WARNING_FIXES
	CvBarbarians::MapInit(kMap.numPlots());
#else
	CvBarbarians::MapInit(GC.getMap().numPlots());
#endif



	CvWorldBuilderMapLoader::ValidateCoast();


#ifdef AUI_WARNING_FIXES
	kMap.updateAdjacency();
#else
	GC.getMap().updateAdjacency();
#endif

#ifndef AUI_PLOT_OBSERVER_SEE_ALL_PLOTS

	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (CvPreGame::slotStatus((PlayerTypes)iI) == SS_OBSERVER)
		{
			CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
			TeamTypes eTeam = kPlayer.getTeam();

			if (eTeam != NO_TEAM)
			{
				const int iNumInvisibleInfos = NUM_INVISIBLE_TYPES;
#endif
#ifdef AUI_WARNING_FIXES
				for (uint plotID = 0; plotID < kMap.numPlots(); plotID++)
				{
					CvPlot* pLoopPlot = kMap.plotByIndexUnchecked(plotID);
#else
				for(int plotID = 0; plotID < GC.getMap().numPlots(); plotID++)
				{
					CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(plotID);
#endif

#ifdef AUI_PLOT_OBSERVER_SEE_ALL_PLOTS
					pLoopPlot->setRevealed(OBSERVER_TEAM, true, false);
					pLoopPlot->changeVisibilityCount(OBSERVER_TEAM, pLoopPlot->getVisibilityCount(OBSERVER_TEAM) + 1, NO_INVISIBLE, true, true);
#else
					pLoopPlot->changeVisibilityCount(eTeam, pLoopPlot->getVisibilityCount(eTeam) + 1, NO_INVISIBLE, true, false);

					for(int iJ = 0; iJ < iNumInvisibleInfos; iJ++)
					{
						pLoopPlot->changeInvisibleVisibilityCount(eTeam, ((InvisibleTypes)iJ), 1);
					}

					pLoopPlot->setRevealed(eTeam, true, false);
#endif
				}
#ifdef AUI_GAME_OBSERVER_MEET_ALL_TEAMS
				for (int iJ = 0; iJ < MAX_TEAMS; iJ++)
				{
					GET_TEAM(OBSERVER_TEAM).makeHasMet(static_cast<TeamTypes>(iJ), true, true);
				}
#endif
#ifndef AUI_PLOT_OBSERVER_SEE_ALL_PLOTS
			}
		}
	}
#endif

	return true;
}

void CvGame::InitPlayers()
{
	PlayerColorTypes aePlayerColors[REALLY_MAX_PLAYERS];
	bool bValid;
	int iI, iJ, iK, iL;

	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		const TeamTypes eTeam(static_cast<TeamTypes>(iI));
		CvTeam& kTeam = GET_TEAM(eTeam);
		kTeam.init(eTeam);
	}

	for(iI = 0; iI < REALLY_MAX_PLAYERS; iI++)
	{
		aePlayerColors[iI] = NO_PLAYERCOLOR;
	}

	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		const PlayerTypes eLoopPlayer(static_cast<PlayerTypes>(iI));
		aePlayerColors[iI] = CvPreGame::playerColor(eLoopPlayer);

		if(aePlayerColors[iI] == NO_PLAYERCOLOR)
		{
			SlotStatus eStatus = CvPreGame::slotStatus(eLoopPlayer);
			if(eStatus == SS_TAKEN || eStatus == SS_COMPUTER || eStatus == SS_OBSERVER)
			{
				CvCivilizationInfo* pCivilizationInfo = GC.getCivilizationInfo(CvPreGame::civilization(eLoopPlayer));
				aePlayerColors[iI] = ((PlayerColorTypes)(pCivilizationInfo->getDefaultPlayerColor()));
			}
		}
	}

	CivilizationTypes eBarbCiv = (CivilizationTypes)GC.getBARBARIAN_CIVILIZATION();
	CivilizationTypes eMinorCiv = (CivilizationTypes)GC.getMINOR_CIVILIZATION();

	CvCivilizationInfo* pBarbarianCivilizationInfo = GC.getCivilizationInfo(eBarbCiv);
#ifdef AUI_WARNING_FIXES
	int barbarianPlayerColor = (pBarbarianCivilizationInfo ? pBarbarianCivilizationInfo->getDefaultPlayerColor() : 0);
#else
	int barbarianPlayerColor = pBarbarianCivilizationInfo->getDefaultPlayerColor();
#endif

	const int iNumPlayerColorInfos = GC.GetNumPlayerColorInfos();
	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		if(aePlayerColors[iI] != NO_PLAYERCOLOR)
		{
			for(iJ = 0; iJ < iI; iJ++)
			{
				if(aePlayerColors[iI] == aePlayerColors[iJ])
				{
					for(iK = 0; iK < iNumPlayerColorInfos; iK++)
					{
						if(iK != barbarianPlayerColor)
						{
							bValid = true;

							for(iL = 0; iL < MAX_MAJOR_CIVS; iL++)
							{
								if(aePlayerColors[iL] == iK)
								{
									bValid = false;
									break;
								}
							}

							if(bValid)
							{
								aePlayerColors[iI] = ((PlayerColorTypes)iK);
							}
						}
					}

					break;
				}
			}
		}
	}

	int iNumMinors = CvPreGame::numMinorCivs();


	if(iNumMinors < 0)
	{
		const CvWorldInfo& kWorldInfo = CvPreGame::worldInfo();
		iNumMinors = kWorldInfo.getDefaultMinorCivs();
	}

	PlayerTypes eMinorPlayer;


	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{

		if(iI == BARBARIAN_PLAYER)
		{
			CvPreGame::setTeamType(BARBARIAN_PLAYER, BARBARIAN_TEAM);
			CvPreGame::setSlotStatus(BARBARIAN_PLAYER, SS_COMPUTER);
			CvPreGame::setNetID(BARBARIAN_PLAYER, -1);
			CvPreGame::setHandicap(BARBARIAN_PLAYER, (HandicapTypes)GC.getBARBARIAN_HANDICAP());
			CvPreGame::setCivilization(BARBARIAN_PLAYER, eBarbCiv);
			CvPreGame::setLeaderHead(BARBARIAN_PLAYER, (LeaderHeadTypes)GC.getBARBARIAN_LEADER());
			CvPreGame::setPlayerColor(BARBARIAN_PLAYER, ((PlayerColorTypes)barbarianPlayerColor));
			CvPreGame::setMinorCiv(BARBARIAN_PLAYER, false);
		}

		else if(iI < MAX_MAJOR_CIVS)
		{
			CvPreGame::setPlayerColor((PlayerTypes)iI, aePlayerColors[iI]);

			if(CvPreGame::slotStatus((PlayerTypes)iI) == SS_COMPUTER)
			{
				CvPreGame::setHandicap((PlayerTypes)iI, (HandicapTypes)GC.getAI_HANDICAP());
			}
			else if(CvPreGame::slotStatus((PlayerTypes)iI) == SS_OBSERVER)
			{
				CvPreGame::setTeamType((PlayerTypes)iI, OBSERVER_TEAM);
			}
		}

		else if(iI < MAX_CIV_PLAYERS)
		{
			eMinorPlayer = (PlayerTypes) iI;

			if(iI < MAX_MAJOR_CIVS + iNumMinors)
			{
				CvMinorCivInfo* pMinorCivInfo = GC.getMinorCivInfo(CvPreGame::minorCivType(eMinorPlayer));

				CvPreGame::setSlotStatus(eMinorPlayer, SS_COMPUTER);
				CvPreGame::setNetID(eMinorPlayer, -1);
				CvPreGame::setHandicap(eMinorPlayer, (HandicapTypes)GC.getMINOR_CIV_HANDICAP());
				CvPreGame::setCivilization(eMinorPlayer, eMinorCiv);
				CvPreGame::setLeaderHead(eMinorPlayer, (LeaderHeadTypes)GC.getBARBARIAN_LEADER());
				CvPreGame::setPlayerColor(eMinorPlayer, (PlayerColorTypes)pMinorCivInfo->getDefaultPlayerColor());
				CvPreGame::setMinorCiv(eMinorPlayer, true);
			}
		}
	}

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const PlayerTypes ePlayer = static_cast<PlayerTypes>(iI);
		CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

		kPlayer.init(ePlayer);
	}
}





void CvGame::setInitialItems(CvGameInitialItemsOverrides& kInitialItemOverrides)
{
	initFreeState(kInitialItemOverrides);

	if(CvPreGame::isWBMapScript())
		assignStartingPlots();


	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayerLoop);

		if(kPlayer.isAlive() && !kPlayer.isMinorCiv() && !kPlayer.isBarbarian())
		{
			kPlayer.GetFlavorManager()->AdjustWeightsForMap();
		}
	}

	initFreeUnits(kInitialItemOverrides);

	m_iEarliestBarbarianReleaseTurn = getHandicapInfo().getEarliestBarbarianReleaseTurn() + GC.getGame().getJonRandNum(GC.getAI_TACTICAL_BARBARIAN_RELEASE_VARIATION(), "Barbarian Release Turn") + 1;


	DoUpdateIndustrialRoute();

	bool bCanWorkWater = GC.getCAN_WORK_WATER_FROM_GAME_START();


	TeamTypes eTeam;
	for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
	{
		eTeam = (TeamTypes) iTeamLoop;

		if(bCanWorkWater)
		{
			GET_TEAM(eTeam).changeWaterWorkCount(1);
		}

		GET_TEAM(eTeam).DoUpdateBestRoute();
	}


	PlayerTypes ePlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(GET_PLAYER(ePlayer).isAlive())
		{

			if(!GET_PLAYER(ePlayer).isMinorCiv())
			{
				GET_PLAYER(ePlayer).GetDiplomacyAI()->DoInitializePersonality();
			}

			else
			{
				GET_PLAYER(ePlayer).GetMinorCivAI()->DoPickInitialItems();
			}


			GET_PLAYER(ePlayer).DoUpdateNextPolicyCost();
		}
	}


#ifdef AUI_WARNING_FIXES
	for (uint iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
#else
	for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
#endif
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if(pkBuildingInfo)
		{
			if(pkBuildingInfo->IsFoundsReligion())
			{
				const TechTypes eReligionTech = (TechTypes) pkBuildingInfo->GetPrereqAndTech();
				SetReligionTech(eReligionTech);
				break;
			}
		}
	}

	DoUpdateTotalReligionTechCost();
	DoCacheMapScoreMod();


	DoInitDiploVictory();

	LogGameState();
}

void CvGame::CheckGenerateArchaeology()
{
	bool bTriggered = false;


	for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS && !bTriggered; iTeamLoop++)
	{
		const TeamTypes eTeam1 = (TeamTypes)iTeamLoop;
		CvTeam& kTeam1 = GET_TEAM(eTeam1);

		if (kTeam1.isAlive() && !kTeam1.isMinorCiv())
		{
#ifdef AUI_WARNING_FIXES
			for (uint iTech = 0; iTech < GC.getNumTechInfos() && !bTriggered; iTech++)
#else
			for (int iTech = 0; iTech < GC.getNumTechInfos() && !bTriggered; iTech++)
#endif
			{
				CvTechEntry *pkTech = GC.getTechInfo((TechTypes)iTech);
				if (pkTech)
				{
					if (kTeam1.GetTeamTechs()->HasTech((TechTypes)pkTech->GetID()))
					{
						if (pkTech->IsTriggersArchaeologicalSites())
						{
							GC.getGame().TriggerArchaeologySiteCreation(false                      );
							bTriggered = true;
						}
					}
				}
			}
		}
	}
}


void CvGame::regenerateMap()
{
	int iI;

	if(CvPreGame::isWBMapScript())
	{
		return;
	}

	setFinalInitialized(false);

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		GET_PLAYER((PlayerTypes)iI).killUnits();
	}

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		GET_PLAYER((PlayerTypes)iI).killCities();
	}

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		GC.getGame().GetGameDeals()->DoCancelAllDealsWithPlayer((PlayerTypes) iI);
	}

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		GET_PLAYER((PlayerTypes)iI).setFoundedFirstCity(false);
		GET_PLAYER((PlayerTypes)iI).setStartingPlot(NULL);
	}

	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		GC.getMap().setRevealedPlots(((TeamTypes)iI), false);
	}

	GC.getMap().erasePlots();

	CvGameInitialItemsOverrides kOverrides;

	setInitialItems(kOverrides);
	CheckGenerateArchaeology();
	initScoreCalculation();
	setFinalInitialized(true);

	GC.getMap().setupGraphical();
	GC.GetEngineUserInterface()->setDirty(ColoredPlots_DIRTY_BIT, true);

	GC.GetEngineUserInterface()->setCycleSelectionCounter(1);

	gDLL->AutoSave(true);
}



void CvGame::DoGameStarted()
{

	BuildTypes eBuild;
#ifdef AUI_WARNING_FIXES
	uint iBuildLoop;
#else
	int iBuildLoop;
#endif

	bool bTempClearable;

	FeatureTypes eFeature;
#ifdef AUI_WARNING_FIXES
	for (uint iFeatureLoop = 0; iFeatureLoop < GC.getNumFeatureInfos(); iFeatureLoop++)
#else
	for(int iFeatureLoop = 0; iFeatureLoop < GC.getNumFeatureInfos(); iFeatureLoop++)
#endif
	{
		eFeature = (FeatureTypes) iFeatureLoop;

		bTempClearable = false;


		for(iBuildLoop = 0; iBuildLoop < GC.getNumBuildInfos(); iBuildLoop++)
		{
			eBuild = (BuildTypes) iBuildLoop;
			CvBuildInfo* pBuildInfo = GC.getBuildInfo(eBuild);


			if(NULL != pBuildInfo && pBuildInfo->isFeatureRemove(eFeature))
			{
				GC.getFeatureInfo(eFeature)->SetClearable(true);

				bTempClearable = true;
				break;
			}

			if(bTempClearable)
				break;
		}
	}

	GET_PLAYER(getActivePlayer()).GetUnitCycler().Rebuild();
}




void CvGame::uninit()
{
	CvGoodyHuts::Uninit();
	CvBarbarians::Uninit();

	SAFE_DELETE_ARRAY(m_paiUnitCreatedCount);
	SAFE_DELETE_ARRAY(m_paiUnitClassCreatedCount);
	SAFE_DELETE_ARRAY(m_paiBuildingClassCreatedCount);
	SAFE_DELETE_ARRAY(m_paiProjectCreatedCount);
	SAFE_DELETE_ARRAY(m_paiVoteOutcome);
	SAFE_DELETE_ARRAY(m_aiSecretaryGeneralTimer);
	SAFE_DELETE_ARRAY(m_aiVoteTimer);
	SAFE_DELETE_ARRAY(m_aiDiploVote);
	SAFE_DELETE_ARRAY(m_aiVotesCast);
	SAFE_DELETE_ARRAY(m_aiPreviousVotesCast);
	SAFE_DELETE_ARRAY(m_aiNumVotesForTeam);
	SAFE_DELETE_ARRAY(m_aiTeamCompetitionWinnersScratchPad);

	SAFE_DELETE_ARRAY(m_pabSpecialUnitValid);

	if(m_ppaaiTeamVictoryRank != NULL)
	{
#ifdef AUI_WARNING_FIXES
		for (uint iI = 0; iI < GC.getNumVictoryInfos(); iI++)
#else
		for(int iI = 0; iI < GC.getNumVictoryInfos(); iI++)
#endif
		{
			SAFE_DELETE_ARRAY(m_ppaaiTeamVictoryRank[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaaiTeamVictoryRank);
	}

	m_aszDestroyedCities.clear();
	m_aszGreatPeopleBorn.clear();

	m_voteSelections.Uninit();
	m_votesTriggered.Uninit();

	m_mapRand.uninit();
	m_jonRand.uninit();

	clearReplayMessageMap();
#ifdef REPLAY_EVENTS
	clearReplayEventMap();
#endif

	m_aPlotExtraYields.clear();
	m_aPlotExtraCosts.clear();

	SAFE_DELETE(m_pDiploResponseQuery);

	SAFE_DELETE(m_pSettlerSiteEvaluator);
	SAFE_DELETE(m_pStartSiteEvaluator);
	SAFE_DELETE(m_pStartPositioner);
	SAFE_DELETE(m_pGameReligions);
	SAFE_DELETE(m_pGameCulture);
	SAFE_DELETE(m_pGameLeagues);
	SAFE_DELETE(m_pGameTrade);
	SAFE_DELETE(m_pTacticalMap);
#ifdef MP_PLAYERS_VOTING_SYSTEM
	SAFE_DELETE(m_pMPVotingSystem);
#endif
	SAFE_DELETE(m_pAdvisorCounsel);
	SAFE_DELETE(m_pAdvisorRecommender);

	m_bForceEndingTurn = false;
#if defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
	m_iCombatModListLength = 0;
#endif
	m_lastTurnAICivsProcessed = -1;
	m_iEndTurnMessagesSent = 0;
	m_iElapsedGameTurns = 0;
	m_iStartTurn = 0;
	m_iWinningTurn = 0;
	m_iStartYear = 0;
	m_iEstimateEndTurn = 0;
	m_iDefaultEstimateEndTurn = 0;
	m_iTurnSlice = 0;
	m_iCutoffSlice = 0;
	m_iNumCities = 0;
	m_iTotalPopulation = 0;
	m_iNoNukesCount = 0;
	m_iNukesExploded = 0;
	m_iMaxPopulation = 0;
	m_iUnused1 = 0;
	m_iUnused2 = 0;
	m_iUnused3 = 0;
	m_iInitPopulation = 0;
	m_iInitLand = 0;
	m_iInitTech = 0;
	m_iInitWonders = 0;
	m_iAIAutoPlay = 0;
	m_iTotalReligionTechCost = 0;
	m_iCachedWorldReligionTechProgress = 0;
	m_iUnitedNationsCountdown = 0;
	m_iNumVictoryVotesTallied = 0;
	m_iNumVictoryVotesExpected = 0;
	m_iVotesNeededForDiploVictory = 0;
	m_iMapScoreMod = 0;

#if defined(LEKMOD_WC_RESPECT_ACTIVATION_ORDER)
	for (int iActivation = 0; iActivation < MAX_PLAYERS; iActivation++)
	{
		m_aiTurnActivationOrder[iActivation] = iActivation;
	}
#endif

	m_uiInitialTime = 0;
#ifdef GAME_UPDATE_TURN_TIMER_ONCE_PER_TURN
	m_fPreviousTurnLen = 0.0f;
#endif
#ifdef TURN_TIMER_PAUSE_BUTTON
	m_fTimeElapsed = 0.0f;
#endif
	m_bScoreDirty = false;
	m_bCircumnavigated = false;
	m_bDebugMode = false;
	m_bDebugModeCache = false;
	m_bFOW = true;
	m_bFinalInitialized = false;
	m_eWaitDiploPlayer = NO_PLAYER;
	m_bPbemTurnSent = false;
	m_bHotPbemBetweenTurns = false;
	m_bPlayerOptionsSent = false;
	m_bNukesValid = false;
	m_bEndGameTechResearched = false;
	m_bTunerEverConnected = false;
	m_bTutorialEverAttacked = false;
	m_bStaticTutorialActive = false;
	m_bEverRightClickMoved = false;
	m_bCombatWarned = false;
	m_bArchaeologyTriggered = false;

	m_eHandicap = NO_HANDICAP;
	m_ePausePlayer = NO_PLAYER;
	m_eAIAutoPlayReturnPlayer = NO_PLAYER;
	m_eBestLandUnit = NO_UNIT;
	m_eWinner = NO_TEAM;
	m_eVictory = NO_VICTORY;
	m_eGameState = GAMESTATE_ON;
	m_eBestWondersPlayer = NO_PLAYER;
	m_eBestPoliciesPlayer = NO_PLAYER;
	m_eBestGreatPeoplePlayer = NO_PLAYER;
	m_eReligionTech = NO_TECH;
	m_eIndustrialRoute = NO_ROUTE;

	m_strScriptData = "";
	m_iEarliestBarbarianReleaseTurn = 0;

	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aiEndTurnMessagesReceived[iI] = 0;
		m_aiRankPlayer[iI] = 0;
		m_aiPlayerRank[iI] = 0;
		m_aiPlayerScore[iI] = 0;
	}

	for(int iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiRankTeam[iI] = 0;
		m_aiTeamRank[iI] = 0;
		m_aiTeamScore[iI] = 0;
	}

	m_iLastMouseoverUnitID = 0;

#ifdef AUI_GAME_BETTER_HYBRID_MODE
	m_iCurrentTurnOrderActive = 0;
	m_iLastTurnOrderID = 0;
#ifdef AUI_GAME_PLAYER_BASED_TURN_LENGTH
	m_aiMaxTurnLengths.clear();
	m_aiMaxTurnLengths.reserve(MAX_TEAMS);
#endif
#endif

	CvCityManager::Shutdown();
}




void CvGame::reset(HandicapTypes eHandicap, bool bConstructorCall)
{
#ifdef AUI_WARNING_FIXES
	uint iI;
#else
	int iI;
#endif



	uninit();

	m_fCurrentTurnTimerPauseDelta = 0.f;

	CvString strUTF8DatabasePath = gDLL->GetCacheFolderPath();
	strUTF8DatabasePath += "Civ5SavedGameDatabase.db";

	std::wstring wstrDatabasePath = CvStringUtils::FromUTF8ToUTF16(strUTF8DatabasePath);

	if(DeleteFileW(wstrDatabasePath.c_str()) == FALSE)
	{
		if(GetLastError() != ERROR_FILE_NOT_FOUND)
		{
			CvAssertMsg(false, "Warning! Cannot delete existing Civ5SavedGameDatabase! Does something have it opened?");
		}
	}

	Database::Connection db;
	if(db.Open(strUTF8DatabasePath.c_str(), Database::OPEN_CREATE | Database::OPEN_READWRITE | Database::OPEN_FULLMUTEX))
	{
		db.Execute("CREATE TABLE SimpleValues(Name TEXT Primary Key, Value VARIANT)");
	}
	else
	{
		CvAssertMsg(false, "Warning! Cannot create new Civ5SavedGameDatabase.");
	}


	m_eHandicap = eHandicap;

	if(!bConstructorCall)
	{
		CvAssertMsg(m_paiUnitCreatedCount==NULL, "about to leak memory, CvGame::m_paiUnitCreatedCount");
		m_paiUnitCreatedCount = FNEW(int[GC.getNumUnitInfos()], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < GC.getNumUnitInfos(); iI++)
		{
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo((UnitTypes)iI);
			if(!pkUnitInfo)
			{
				continue;
			}
			m_paiUnitCreatedCount[iI] = 0;
		}

		CvAssertMsg(m_paiUnitClassCreatedCount==NULL, "about to leak memory, CvGame::m_paiUnitClassCreatedCount");
		m_paiUnitClassCreatedCount = FNEW(int[GC.getNumUnitClassInfos()], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo((UnitClassTypes)iI);
			if(!pkUnitClassInfo)
			{
				continue;
			}
			m_paiUnitClassCreatedCount[iI] = 0;
		}

		CvAssertMsg(m_paiBuildingClassCreatedCount==NULL, "about to leak memory, CvGame::m_paiBuildingClassCreatedCount");
		m_paiBuildingClassCreatedCount = FNEW(int[GC.getNumBuildingClassInfos()], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo((BuildingClassTypes)iI);
			if(!pkBuildingClassInfo)
			{
				continue;
			}

			m_paiBuildingClassCreatedCount[iI] = 0;
		}

		CvAssertMsg(m_paiProjectCreatedCount==NULL, "about to leak memory, CvGame::m_paiProjectCreatedCount");
		m_paiProjectCreatedCount = FNEW(int[GC.getNumProjectInfos()], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < GC.getNumProjectInfos(); iI++)
		{
			m_paiProjectCreatedCount[iI] = 0;
		}


		CvAssertMsg(0 < GC.getNumVoteInfos(), "GC.getNumVoteInfos() is not greater than zero in CvGame::reset");
		CvAssertMsg(m_paiVoteOutcome==NULL, "about to leak memory, CvGame::m_paiVoteOutcome");
		m_paiVoteOutcome = FNEW(PlayerVoteTypes[GC.getNumVoteInfos()], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < GC.getNumVoteInfos(); iI++)
		{
			m_paiVoteOutcome[iI] = NO_PLAYER_VOTE;
		}

		CvAssertMsg(0 < GC.getNumVoteSourceInfos(), "GC.getNumVoteSourceInfos() is not greater than zero in CvGame::reset");
		CvAssertMsg(m_aiDiploVote==NULL, "about to leak memory, CvGame::m_aiDiploVote");
		m_aiDiploVote = FNEW(int[GC.getNumVoteSourceInfos()], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < GC.getNumVoteSourceInfos(); iI++)
		{
			m_aiDiploVote[iI] = 0;
		}

		CvAssertMsg(0 < MAX_CIV_TEAMS, "MAX_CIV_TEAMS is not greater than zero in CvGame::reset");
		CvAssertMsg(m_aiVotesCast==NULL, "about to leak memory, CvGame::m_aiVotesCast");
		m_aiVotesCast = FNEW(int[MAX_CIV_TEAMS], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			m_aiVotesCast[iI] = NO_TEAM;
		}

		CvAssertMsg(0 < MAX_CIV_TEAMS, "MAX_CIV_TEAMS is not greater than zero in CvGame::reset");
		CvAssertMsg(m_aiPreviousVotesCast == NULL, "About to leak memory, CvGame::m_aiPreviousVotesCast. Please send Anton your save file and version.");
		m_aiPreviousVotesCast = FNEW(int[MAX_CIV_TEAMS], c_eCiv5GameplayDLL, 0);
		for (iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			m_aiPreviousVotesCast[iI] = NO_TEAM;
		}

		CvAssertMsg(0 < MAX_CIV_TEAMS, "MAX_CIV_TEAMS is not greater than zero in CvGame::reset");
		CvAssertMsg(m_aiNumVotesForTeam==NULL, "about to leak memory, CvGame::m_aiNumVotesForTeam");
		m_aiNumVotesForTeam = FNEW(int[MAX_CIV_TEAMS], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			m_aiNumVotesForTeam[iI] = 0;
		}

		CvAssertMsg(0 < MAX_CIV_TEAMS, "MAX_CIV_TEAMS is not greater than zero in CvGame::reset");
		CvAssertMsg(m_aiTeamCompetitionWinnersScratchPad==NULL, "about to leak memory, CvGame::m_aiTeamCompetitionWinnersScratchPad");
		m_aiTeamCompetitionWinnersScratchPad = FNEW(int[MAX_CIV_TEAMS], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			m_aiTeamCompetitionWinnersScratchPad[iI] = 0;
		}

		CvAssertMsg(m_pabSpecialUnitValid==NULL, "about to leak memory, CvGame::m_pabSpecialUnitValid");
		m_pabSpecialUnitValid = FNEW(bool[GC.getNumSpecialUnitInfos()], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < GC.getNumSpecialUnitInfos(); iI++)
		{
			m_pabSpecialUnitValid[iI] = false;
		}

		CvAssertMsg(m_aiSecretaryGeneralTimer==NULL, "about to leak memory, CvGame::m_aiSecretaryGeneralTimer");
		CvAssertMsg(m_aiVoteTimer==NULL, "about to leak memory, CvGame::m_aiVoteTimer");
		m_aiSecretaryGeneralTimer = FNEW(int[GC.getNumVoteSourceInfos()], c_eCiv5GameplayDLL, 0);
		m_aiVoteTimer = FNEW(int[GC.getNumVoteSourceInfos()], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < GC.getNumVoteSourceInfos(); iI++)
		{
			m_aiSecretaryGeneralTimer[iI] = 0;
			m_aiVoteTimer[iI] = 0;
		}

		CvAssertMsg(m_ppaaiTeamVictoryRank==NULL, "about to leak memory, CvGame::m_ppaaiTeamVictoryRank");
		m_ppaaiTeamVictoryRank = FNEW(int*[GC.getNumVictoryInfos()], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < GC.getNumVictoryInfos(); iI++)
		{
			m_ppaaiTeamVictoryRank[iI] = FNEW(int[GC.getNUM_VICTORY_POINT_AWARDS()], c_eCiv5GameplayDLL, 0);
			for(int iJ = 0; iJ < GC.getNUM_VICTORY_POINT_AWARDS(); iJ++)
			{
				m_ppaaiTeamVictoryRank[iI][iJ] = NO_TEAM;
			}
		}

		CvAssertMsg(m_pSettlerSiteEvaluator==NULL, "about to leak memory, CvGame::m_pSettlerSiteEvaluator");
		m_pSettlerSiteEvaluator = FNEW(CvSiteEvaluatorForSettler, c_eCiv5GameplayDLL, 0);
		m_pSettlerSiteEvaluator->Init();

		CvAssertMsg(m_pStartSiteEvaluator==NULL, "about to leak memory, CvGame::m_pStartSiteEvaluator");
		m_pStartSiteEvaluator = FNEW(CvSiteEvaluatorForStart, c_eCiv5GameplayDLL, 0);
		m_pStartSiteEvaluator->Init();

		CvAssertMsg(m_pStartPositioner==NULL, "about to leak memory, CvGame::m_pStartPositioner");
		m_pStartPositioner = FNEW(CvStartPositioner, c_eCiv5GameplayDLL, 0);
		m_pStartPositioner->Init(m_pStartSiteEvaluator);

		m_kGameDeals.Init();

		CvAssertMsg(m_pGameReligions==NULL, "about to leak memory, CvGame::m_pGameReligions");
		m_pGameReligions = FNEW(CvGameReligions, c_eCiv5GameplayDLL, 0);
		m_pGameReligions->Init();

		CvAssertMsg(m_pGameCulture==NULL, "about to leak memory, CvGame::m_pGameCulture");
		m_pGameCulture = FNEW(CvGameCulture, c_eCiv5GameplayDLL, 0);

		CvAssertMsg(m_pGameLeagues==NULL, "about to leak memory, CvGame::m_pGameLeagues");
		m_pGameLeagues = FNEW(CvGameLeagues, c_eCiv5GameplayDLL, 0);
		m_pGameLeagues->Init();

		CvAssertMsg(m_pGameTrade==NULL, "about to leak memory, CvGame::m_pGameTrade");
		m_pGameTrade = FNEW(CvGameTrade, c_eCiv5GameplayDLL, 0);
		m_pGameTrade->Init();

		CvAssertMsg(m_pTacticalMap==NULL, "about to leak memory, CvGame::m_pTacticalMap");
		m_pTacticalMap = FNEW(CvTacticalAnalysisMap, c_eCiv5GameplayDLL, 0);
#ifdef MP_PLAYERS_VOTING_SYSTEM
		CvAssertMsg(m_pMPVotingSystem == NULL, "about to leak memory, CvGame::m_pMPVotingSystem");
		m_pMPVotingSystem = FNEW(CvMPVotingSystem, c_eCiv5GameplayDLL, 0);

#endif
		CvAssertMsg(m_pAdvisorCounsel==NULL, "about to leak memory, CvGame::m_pAdvisorCounsel");
		m_pAdvisorCounsel = FNEW(CvAdvisorCounsel, c_eCiv5GameplayDLL, 0);

		CvAssertMsg(m_pAdvisorRecommender==NULL, "about to leak memory, CvGame::m_pAdvisorRecommender");
		m_pAdvisorRecommender = FNEW(CvAdvisorRecommender, c_eCiv5GameplayDLL, 0);
	}

	m_voteSelections.RemoveAll();
	m_votesTriggered.RemoveAll();

	m_mapRand.reset();
	m_jonRand.reset();

	m_iNumSessions = 1;

	m_AdvisorMessagesViewed.clear();

	CvCityManager::Reset();
}



void CvGame::initDiplomacy()
{
	for(int iI = 0; iI < MAX_TEAMS; iI++)
	{
		const TeamTypes eTeamA = static_cast<TeamTypes>(iI);
		CvTeam& kTeamA = GET_TEAM(eTeamA);
		kTeamA.meet(eTeamA, false);

		if(kTeamA.isBarbarian())
		{
			for(int iJ = 0; iJ < MAX_CIV_TEAMS; iJ++)
			{
				const TeamTypes eTeamB = static_cast<TeamTypes>(iJ);
				if(iI != iJ)
				{
					kTeamA.declareWar(eTeamB);
				}
			}
		}
	}
}



void CvGame::initFreeState(CvGameInitialItemsOverrides& kOverrides)
{
#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumTechInfos(); iI++)
#else
	for(int iI = 0; iI < GC.getNumTechInfos(); iI++)
#endif
	{
		const TechTypes eTech = static_cast<TechTypes>(iI);
		CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
		if(pkTechInfo)
		{
			for(int iJ = 0; iJ < MAX_TEAMS; iJ++)
			{
				const TeamTypes eTeam = static_cast<TeamTypes>(iJ);
				const bool bGrantFreeTechs = kOverrides.GrantInitialFreeTechsPerTeam[eTeam];

				if(bGrantFreeTechs)
				{
					CvTeam& kTeam = GET_TEAM(eTeam);
					if(kTeam.isAlive())
					{

						if (kTeam.GetTeamTechs()->HasTech(eTech))
						{
							continue;
						}

						bool bValid = false;

					#ifdef NQ_AI_HANDICAP_START
						if(!GC.getGame().isOption("GAMEOPTION_AI_HANDICAP_START"))
						{
							if(!bValid)
							{
								if((getHandicapInfo().isFreeTechs(iI)) ||
										(!(kTeam.isHuman())&& getHandicapInfo().isAIFreeTechs(iI)) ||
										(pkTechInfo->GetEra() < getStartEra()))
								{
									bValid = true;
								}
							}
						}
						else
						{
							if(!bValid)
							{
								if((getHandicapInfo().isFreeTechs(iI)) ||
										(pkTechInfo->GetEra() < getStartEra()))
								{
									bValid = true;
								}
							}
						}
#else
						if(!bValid)
						{
							if((getHandicapInfo().isFreeTechs(iI)) ||
									(!(kTeam.isHuman())&& getHandicapInfo().isAIFreeTechs(iI)) ||
									(pkTechInfo->GetEra() < getStartEra()))
							{
								bValid = true;
							}
						}
#endif



						if(!bValid)
						{
							for(int iK = 0; iK < MAX_PLAYERS; iK++)
							{
								CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iK);
								if(kPlayer.isAlive())
								{
									if(kPlayer.getTeam() == eTeam)
									{
										if(kPlayer.getCivilizationInfo().isCivilizationFreeTechs(iI))
										{
											bValid = true;
											break;
										}
									}
								}
							}
						}

						kTeam.setHasTech(eTech, bValid, NO_PLAYER, false, false);
						if(bValid && pkTechInfo->IsMapVisible())
						{
							GC.getMap().setRevealedPlots(eTeam, true, true);
							GC.getMap().updateDeferredFog();
						}
					}
				}
			}
		}
	}

	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const PlayerTypes ePlayer = static_cast<PlayerTypes>(iI);
		CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
		if(kPlayer.isAlive())
		{
			kPlayer.initFreeState(kOverrides);
		}
	}
}



void CvGame::initFreeUnits(CvGameInitialItemsOverrides& kOverrides)
{
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const PlayerTypes ePlayer = static_cast<PlayerTypes>(iI);
		CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

		if(kOverrides.GrantInitialUnitsPerPlayer[ePlayer])
		{
			if(kPlayer.isAlive())
			{
				if((kPlayer.GetNumUnitsWithUnitAI(UNITAI_SETTLE,false,false) == 0) && (kPlayer.getNumCities() == 0))
				{
					kPlayer.initFreeUnits(kOverrides);
				}
			}
		}
	}
}


void CvGame::assignStartingPlots()
{

	CvStartPositioner* pPositioner = GetStartPositioner();


	pPositioner->DivideMapIntoRegions(countMajorCivsAlive());


	pPositioner->ComputeFoundValues();


	pPositioner->RankPlayerStartOrder();
	pPositioner->AssignStartingLocations();
}


void CvGame::update()
{
	if(IsWaitingForBlockingInput())
	{
		if(!GC.GetEngineUserInterface()->isDiploActive())
		{
			GET_PLAYER(m_eWaitDiploPlayer).doTurnPostDiplomacy();
			SetWaitingForBlockingInput(NO_PLAYER);
		}
		else
		{
			return;
		}
	}


	{
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if(pkScriptSystem)
		{
			CvLuaArgsHandle args;
			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "GameCoreUpdateBegin", args.get(), bResult);
		}
	}



	if(getGameState() == GAMESTATE_OVER && !CvPreGame::isNetworkMultiplayerGame())
	{
		testExtendedGame();
	}
	else
	{





		if(getGameState() == GAMESTATE_OVER && CvPreGame::isNetworkMultiplayerGame())
		{
			testExtendedGame();
		}

		{
			sendPlayerOptions();

			if(getTurnSlice() == 0 && !isPaused())
			{
				gDLL->AutoSave(true);
			}


#ifdef AUI_GAME_BETTER_HYBRID_MODE
			if (isNoPlayerActive())
			{
				bool bDoAI = true;
				if (isAnySimultaneousTurns())
				{
					while (m_iCurrentTurnOrderActive < m_iLastTurnOrderID)
					{
						m_iCurrentTurnOrderActive++;


						for (int iI = 0; iI < MAX_PLAYERS; iI++)
						{
							CvPlayer& player = GET_PLAYER((PlayerTypes)iI);
							if (!player.isTurnActive() && player.isHuman() && player.isAlive() && (player.getTurnOrder() == m_iCurrentTurnOrderActive))
							{
								bDoAI = false;
								goto TurnAdvanceCheck;
							}
						}
					}
				}

			TurnAdvanceCheck:;
				if (bDoAI)
				{
					if (gDLL->CanAdvanceTurn())
					{
						doTurn();
					}
				}
				else
					resetTurnTimer(false);
#else
			if(getNumGameTurnActive() == 0)
			{
				if(gDLL->CanAdvanceTurn())
					doTurn();
#endif
			}

			if(!isPaused())
			{
				updateScore();

				updateWar();

				updateMoves();

				if(!isPaused())
				{
					updateTimers();

					UpdatePlayers();

					testAlive();

					if((getAIAutoPlay() == 0) && !(gDLL->GetAutorun()) && GAMESTATE_EXTENDED != getGameState())
					{
						if(CvPreGame::slotStatus(getActivePlayer()) != SS_OBSERVER && !GET_PLAYER(getActivePlayer()).isAlive())
						{
							setGameState(GAMESTATE_OVER);
						}
					}

					CheckPlayerTurnDeactivate();

					changeTurnSlice(1);

					gDLL->FlushTurnReminders();
				}
			}

			PlayerTypes activePlayerID = getActivePlayer();
			const CvPlayer& activePlayer = GET_PLAYER(activePlayerID);
			if(NO_PLAYER != activePlayerID && activePlayer.getAdvancedStartPoints() >= 0 && !GC.GetEngineUserInterface()->isInAdvancedStart())
			{
				GC.GetEngineUserInterface()->setInAdvancedStart(true);
			}
		}
	}


	{
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if(pkScriptSystem)
		{
			CvLuaArgsHandle args;
			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "GameCoreUpdateEnd", args.get(), bResult);
		}
	}
}





void CvGame::CheckPlayerTurnDeactivate()
{
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);

		if(kPlayer.isAlive() && kPlayer.isTurnActive())
		{
			if(kPlayer.isEndTurn() || (!kPlayer.isHuman() && !kPlayer.HasActiveDiplomacyRequests()))
			{
				if(kPlayer.hasProcessedAutoMoves())
				{
					bool bAutoMovesComplete = false;
					if(!(kPlayer.hasBusyUnitOrCity()))
					{
						bAutoMovesComplete = true;

						NET_MESSAGE_DEBUG_OSTR_ALWAYS("CheckPlayerTurnDeactivate() : auto-moves complete for " << kPlayer.getName());
					}
					else if(gDLL->HasReceivedTurnComplete(kPlayer.GetID()))
					{
						bAutoMovesComplete = true;
					}

					if(bAutoMovesComplete)
					{
#ifdef AUTOSAVE_END_OF_TURN
						int iFirstAlivePlayer = -1;
						for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
						{
							CvPlayer& kItPlayer = GET_PLAYER((PlayerTypes)iJ);
							if (kItPlayer.isAlive() && kItPlayer.isHuman())
							{
								iFirstAlivePlayer = iJ;
								break;
							}
						}
						if (iI == iFirstAlivePlayer)
						{
							gDLL->AutoSave(false, true);
						}
#endif
						kPlayer.setTurnActive(false);






#ifdef AUI_GAME_BETTER_HYBRID_MODE
						if (!isAnySimultaneousTurns() || !kPlayer.isHuman())
#else
						if(!kPlayer.isSimultaneousTurns())
#endif
						{
							if((isPbem() || isHotSeat()) && kPlayer.isHuman() && countHumanPlayersAlive() > 1)
							{
								setHotPbemBetweenTurns(true);
							}

#ifndef AUI_GAME_BETTER_HYBRID_MODE
							if(isSimultaneousTeamTurns())
							{
								if(!GET_TEAM(kPlayer.getTeam()).isTurnActive())
								{
									for(int iJ = (kPlayer.getTeam() + 1); iJ < MAX_TEAMS; iJ++)
									{
										CvTeam& kTeam = GET_TEAM((TeamTypes)iJ);
										if(kTeam.isAlive() && !kTeam.isSimultaneousTurns())
										{
											kTeam.setTurnActive(true);
											resetTurnTimer(false);
											break;
										}
									}
								}
								else
								{
									CvString logOutput;
									logOutput.Format("CheckPlayerTurnDeactivate(): Next sequential player not set TurnActive due to player(%i)'s team(%i) being TurnActive.", kPlayer.GetID(), kPlayer.getTeam());
									gDLL->netMessageDebugLog(logOutput);
								}
							}
							else
#endif
							{
								if(!GC.GetEngineUserInterface()->isDiploActive())
								{
									if(!isHotSeat() || kPlayer.isAlive() || !kPlayer.isHuman())
									{
										for(int iJ = (kPlayer.GetID() + 1); iJ < MAX_PLAYERS; iJ++)
										{
											CvPlayer& kNextPlayer = GET_PLAYER((PlayerTypes)iJ);
#ifdef AUI_GAME_BETTER_HYBRID_MODE
											if (kNextPlayer.isAlive() && (!isAnySimultaneousTurns() || !kNextPlayer.isHuman()))
#else
											if(kNextPlayer.isAlive() && !kNextPlayer.isSimultaneousTurns())
#endif
											{
												if(isPbem() && kNextPlayer.isHuman())
												{
													if(!getPbemTurnSent())
													{
														gDLL->sendPbemTurn((PlayerTypes)iJ);
													}
												}
												else
												{
													kNextPlayer.setTurnActive(true);
													resetTurnTimer(false);
												}
												break;
											}
										}
									}
								}
								else
								{

									changeNumGameTurnActive(1, std::string("Because the diplo screen is blocking I am bumping this up for player ") + getName());
								}
							}
						}
					}
				}
			}
		}
	}
}


void CvGame::updateScore(bool bForce)
{
	if(!isScoreDirty() && !bForce)
		return;

	setScoreDirty(false);

	bool abPlayerScored[MAX_CIV_PLAYERS];
	bool abTeamScored[MAX_CIV_TEAMS];
	int iScore;
	int iBestScore;
	PlayerTypes eBestPlayer;
	TeamTypes eBestTeam;
	int iI, iJ, iK;

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		abPlayerScored[iI] = false;
	}

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		iBestScore = MIN_INT;
		eBestPlayer = NO_PLAYER;

		for(iJ = 0; iJ < MAX_CIV_PLAYERS; iJ++)
		{
			if(!abPlayerScored[iJ])
			{
				iScore = GET_PLAYER((PlayerTypes)iJ).GetScore(false);

				if(iScore >= iBestScore)
				{
					iBestScore = iScore;
					eBestPlayer = (PlayerTypes)iJ;
				}
			}
		}

		abPlayerScored[eBestPlayer] = true;

		setRankPlayer(iI, eBestPlayer);
		setPlayerRank(eBestPlayer, iI);
		setPlayerScore(eBestPlayer, iBestScore);

		CvPlayerAI& player = GET_PLAYER(eBestPlayer);

		unsigned int uiDataSetIndex = player.getReplayDataSetIndex("REPLAYDATASET_SCORE");
		player.setReplayDataValue(uiDataSetIndex, getGameTurn(), iBestScore);
	}

	for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		abTeamScored[iI] = false;
	}

	for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		iBestScore = MIN_INT;
		eBestTeam = NO_TEAM;

		for(iJ = 0; iJ < MAX_CIV_TEAMS; iJ++)
		{
			if(!abTeamScored[iJ])
			{
				iScore = GET_TEAM((TeamTypes) iJ).GetScore();

				for(iK = 0; iK < MAX_CIV_PLAYERS; iK++)
				{
					if(GET_PLAYER((PlayerTypes)iK).getTeam() == iJ)
					{
						iScore += getPlayerScore((PlayerTypes)iK);
					}
				}

				if(iScore >= iBestScore)
				{
					iBestScore = iScore;
					eBestTeam = (TeamTypes)iJ;
				}
			}
		}

		abTeamScored[eBestTeam] = true;

		setRankTeam(iI, eBestTeam);
		setTeamRank(eBestTeam, iI);
		setTeamScore(eBestTeam, iBestScore);
	}
}



int CvGame::GetMapScoreMod() const
{
	return m_iMapScoreMod;
}


void CvGame::DoCacheMapScoreMod()
{

	int iBaseNumTiles = 4160;


	WorldSizeTypes eStandardWorld = (WorldSizeTypes) GC.getInfoTypeForString("WORLDSIZE_STANDARD", true);
	if(eStandardWorld == NO_WORLDSIZE)
	{
		Database::SingleResult kResult;
		CvWorldInfo kWorldInfo;
		const bool bResult = DB.SelectAt(kResult, "Worlds", eStandardWorld);
		DEBUG_VARIABLE(bResult);
		CvAssertMsg(bResult, "Cannot find world info.");
		kWorldInfo.CacheResult(kResult);

		iBaseNumTiles = kWorldInfo.getGridWidth();
		iBaseNumTiles *= kWorldInfo.getGridHeight();
	}

	int iNumTilesOnThisMap = GC.getMap().numPlots();

	int iScoreMod = 100 * iBaseNumTiles / iNumTilesOnThisMap;


	if(iScoreMod > 100)
	{
		iScoreMod -= 100;
		iScoreMod /= 3;
		iScoreMod += 100;
	}

	m_iMapScoreMod = iScoreMod;
}



void CvGame::updateCitySight(bool bIncrement)
{
	int iI;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive())
		{
			kPlayer.updateCitySight(bIncrement);
		}
	}
}


void CvGame::updateSelectionList()
{
#ifdef AUI_WARNING_FIXES
	ICvUserInterface2& kEngineUserInterface = *GC.GetEngineUserInterface();
	if (!kEngineUserInterface.DoAutoUnitCycle())
#else
	if(!GC.GetEngineUserInterface()->DoAutoUnitCycle())
#endif
	{
		return;
	}

#ifdef AUI_WARNING_FIXES
	auto_ptr<ICvUnit1> pDllHeadSelectedUnit(kEngineUserInterface.GetHeadSelectedUnit());
#else
	auto_ptr<ICvUnit1> pDllHeadSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
#endif
	CvUnit* pkHeadSelectedUnit = GC.UnwrapUnitPointer(pDllHeadSelectedUnit.get());

	if((pkHeadSelectedUnit == NULL) || !(pkHeadSelectedUnit->ReadyToSelect()))
	{
		if(getGameTurn() == 0)
		{
			SelectSettler();
		}
	}

#ifdef AUI_WARNING_FIXES
	pDllHeadSelectedUnit.reset(kEngineUserInterface.GetHeadSelectedUnit());
#else
	pDllHeadSelectedUnit.reset(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
#endif
	pkHeadSelectedUnit = GC.UnwrapUnitPointer(pDllHeadSelectedUnit.get());

	if((pkHeadSelectedUnit == NULL) || !(pkHeadSelectedUnit->ReadyToSelect()))
	{
#ifdef AUI_WARNING_FIXES
		int iOriginalPlotIndex = kEngineUserInterface.getOriginalPlotIndex();
		CvPlot* pkOriginalPlot = (iOriginalPlotIndex != -1) ? GC.getMap().plotByIndex(iOriginalPlotIndex) : NULL;

		if ((pkOriginalPlot == NULL) || !(cyclePlotUnits(pkOriginalPlot, true, true, kEngineUserInterface.getOriginalPlotCount())))
		{
			auto_ptr<ICvPlot1> pSelectionPlot(kEngineUserInterface.getSelectionPlot());
#else
		int iOriginalPlotIndex = GC.GetEngineUserInterface()->getOriginalPlotIndex();
		CvPlot* pkOriginalPlot = (iOriginalPlotIndex != -1)? GC.getMap().plotByIndex(iOriginalPlotIndex) : NULL;

		if((pkOriginalPlot == NULL) || !(cyclePlotUnits(pkOriginalPlot, true, true, GC.GetEngineUserInterface()->getOriginalPlotCount())))
		{
			auto_ptr<ICvPlot1> pSelectionPlot(GC.GetEngineUserInterface()->getSelectionPlot());
#endif
			CvPlot* pkSelectionPlot = GC.UnwrapPlotPointer(pSelectionPlot.get());
			if((pkSelectionPlot == NULL) || !(cyclePlotUnits(pkSelectionPlot, true, true)))
			{
				cycleUnits(true);
			}
		}

#ifdef AUI_WARNING_FIXES
		pDllHeadSelectedUnit.reset(kEngineUserInterface.GetHeadSelectedUnit());
#else
		pDllHeadSelectedUnit.reset(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
#endif
		pkHeadSelectedUnit = GC.UnwrapUnitPointer(pDllHeadSelectedUnit.get());

		if(pkHeadSelectedUnit != NULL)
		{
			if(!(pkHeadSelectedUnit->ReadyToSelect()))
			{
#ifdef AUI_WARNING_FIXES
				kEngineUserInterface.ClearSelectionList();
#else
				GC.GetEngineUserInterface()->ClearSelectionList();
#endif
			}
		}
	}
}


int s_unitMoveTurnSlice = 0;

bool CvGame::hasTurnTimerExpired(PlayerTypes playerID)
{



	bool gameTurnTimerExpired = false;
	bool isLocalPlayer = getActivePlayer() == playerID;
	if(isOption(GAMEOPTION_END_TURN_TIMER_ENABLED) && !isPaused() && GC.getGame().getGameState() == GAMESTATE_ON)
	{
		ICvUserInterface2* iface = GC.GetEngineUserInterface();
#ifdef TURN_TIMER_PAUSE_BUTTON
		if (getElapsedGameTurns() > 0 && !GC.getGame().m_bIsPaused)
#else
		if(getElapsedGameTurns() > 0)
#endif
		{
#if defined GAME_AUTOPAUSE_ON_ACTIVE_DISCONNECT_IF_NOT_SEQUENTIAL && defined NQM_GAME_FIX_TURN_TIMER_RESET_ON_AUTOMATION
			PlayerTypes ePausePlayer = NO_PLAYER;
			for (int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
			{
				CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
				if (kPlayer.isAlive() && kPlayer.isHuman() && !kPlayer.isConnected())
				{

					break;
				}
		}
			if (isLocalPlayer && (!gDLL->allAICivsProcessedThisTurn() || !allUnitAIProcessed()) && ePausePlayer == NO_PLAYER || (isOption("GAMEOPTION_AUTOMATION_RESETS_TIMER") && !allUnitAIProcessed()))
#elif defined NQM_GAME_FIX_TURN_TIMER_RESET_ON_AUTOMATION
			if (isLocalPlayer && ((!gDLL->allAICivsProcessedThisTurn() && allUnitAIProcessed()) || (isOption("GAMEOPTION_AUTOMATION_RESETS_TIMER") && !allUnitAIProcessed())))
#else
			if(isLocalPlayer && (!gDLL->allAICivsProcessedThisTurn() || !allUnitAIProcessed()))
#endif
			{
				resetTurnTimer(true);


				CvPreGame::setEndTurnTimerLength(0.0f);
				iface->updateEndTurnTimer(0.0f);
			}
			else
			{
				if(playerID == NO_PLAYER)
				{
					return false;
				}
				CvPlayer& curPlayer = GET_PLAYER(playerID);


#ifdef GAME_UPDATE_TURN_TIMER_ONCE_PER_TURN
				float gameTurnEnd = getPreviousTurnLen();
#else
				float gameTurnEnd = static_cast<float>(getMaxTurnLen());
#endif



				float timeSinceCurrentTurnStart = m_curTurnTimer.Peek() + m_fCurrentTurnTimerPauseDelta; 

				float timeSinceGameTurnStart = m_timeSinceGameTurnStart.Peek() + m_fCurrentTurnTimerPauseDelta;
				
#ifdef AUI_GAME_PLAYER_BASED_TURN_LENGTH
				float fTimeElapsed = m_curTurnTimer.Peek() + m_fCurrentTurnTimerPauseDelta;
#elif defined(AUI_GAME_BETTER_HYBRID_MODE)
				float timeElapsed = timeSinceCurrentTurnStart;
#else
#ifdef TURN_TIMER_PAUSE_BUTTON
				setTimeElapsed((curPlayer.isSimultaneousTurns() ? timeSinceGameTurnStart : timeSinceCurrentTurnStart));
				float timeElapsed = getTimeElapsed();
#else
				float timeElapsed = (curPlayer.isSimultaneousTurns() ? timeSinceGameTurnStart : timeSinceCurrentTurnStart);
#endif
#endif
				if(curPlayer.isTurnActive())
				{
#ifdef AUI_GAME_PLAYER_BASED_TURN_LENGTH
					if (fTimeElapsed > fGameTurnEnd)
#else
					if(timeElapsed > gameTurnEnd)
#endif
					{
						if(s_unitMoveTurnSlice == 0)
						{
							gameTurnTimerExpired = true;
						}
#ifdef NQM_GAME_MORE_RELAXED_TURN_SLICE_LIMIT_FOR_NETWORK_MULTIPLAYER
						else if (s_unitMoveTurnSlice + (CvPreGame::isNetworkMultiplayerGame() ? 100 : 10) < getTurnSlice())
#else
						else if(s_unitMoveTurnSlice + 10 < getTurnSlice())
#endif
						{
							gameTurnTimerExpired = true;
						}
					}
				}

#ifndef AUI_GAME_PLAYER_BASED_TURN_LENGTH
				if((!curPlayer.isTurnActive() || gDLL->HasReceivedTurnComplete(playerID))
					&& getNumSequentialHumans() > 1)
				{





					int playersInSeq = getNumSequentialHumans(playerID);


					gameTurnEnd *= playersInSeq;

					float timePerPlayer = gameTurnEnd / playersInSeq;

					int humanTurnsUntilMe = countSeqHumanTurnsUntilPlayerTurn(playerID);
					int humanTurnsCompleted = playersInSeq - humanTurnsUntilMe;

					if(humanTurnsUntilMe)
					{
						timeElapsed =  timeSinceCurrentTurnStart + humanTurnsCompleted*timePerPlayer;
					}
					else
					{





						timeElapsed = timeSinceGameTurnStart + (humanTurnsCompleted-1)*timePerPlayer;
					}
				}
#endif

				if(isLocalPlayer)
				{
#ifdef AUI_GAME_PLAYER_BASED_TURN_LENGTH
					CvPreGame::setEndTurnTimerLength(fGameTurnEnd);
					iface->updateEndTurnTimer(fTimeElapsed / fGameTurnEnd);
#else
					CvPreGame::setEndTurnTimerLength(gameTurnEnd);
					iface->updateEndTurnTimer(timeElapsed / gameTurnEnd);
#endif
				}
			}
		}
#ifdef TURN_TIMER_PAUSE_BUTTON
		else if (getElapsedGameTurns() > 0 && GC.getGame().m_bIsPaused)
		{
			if (!(isLocalPlayer && (!gDLL->allAICivsProcessedThisTurn() || !allUnitAIProcessed())))
			{


#ifdef GAME_UPDATE_TURN_TIMER_ONCE_PER_TURN
				float gameTurnEnd = getPreviousTurnLen();
#else
				float gameTurnEnd = static_cast<float>(getMaxTurnLen());
#endif


				float timeElapsed = getTimeElapsed();

				if (isLocalPlayer)
				{
					CvPreGame::setEndTurnTimerLength(gameTurnEnd);
					iface->updateEndTurnTimer(timeElapsed / gameTurnEnd);
				}
			}
		}
#endif
		else if(isLocalPlayer){

			CvPreGame::setEndTurnTimerLength(0.0f);
			iface->updateEndTurnTimer(0.0f);
		}
	}

	return gameTurnTimerExpired;
}


#ifdef AUI_GAME_PLAYER_BASED_TURN_LENGTH
void CvGame::TurnTimerSync(float fCurTurnTime, float                   )
#else
void CvGame::TurnTimerSync(float fCurTurnTime, float fTurnStartTime)
#endif
{
	m_curTurnTimer.StartWithOffset(fCurTurnTime);
#ifndef AUI_GAME_PLAYER_BASED_TURN_LENGTH
	m_timeSinceGameTurnStart.StartWithOffset(fTurnStartTime);
#endif
}


#ifdef AUI_GAME_PLAYER_BASED_TURN_LENGTH
void CvGame::GetTurnTimerData(float& fCurTurnTime, float&                   )
#else
void CvGame::GetTurnTimerData(float& fCurTurnTime, float& fTurnStartTime)
#endif
{
	fCurTurnTime = m_curTurnTimer.Peek();
#ifndef AUI_GAME_PLAYER_BASED_TURN_LENGTH
	fTurnStartTime = m_timeSinceGameTurnStart.Peek();
#endif
}


void CvGame::updateTestEndTurn()
{
	PlayerTypes activePlayerID = getActivePlayer();
	CvPlayer& activePlayer = GET_PLAYER(activePlayerID);

	ICvUserInterface2* pkIface = GC.GetEngineUserInterface();
	if(pkIface != NULL)
	{
		bool automaticallyEndTurns = (isGameMultiPlayer())? pkIface->IsMPAutoEndTurnEnabled() : pkIface->IsSPAutoEndTurnEnabled();
		if(automaticallyEndTurns && s_unitMoveTurnSlice != 0)
#ifdef NQM_GAME_MORE_RELAXED_TURN_SLICE_LIMIT_FOR_NETWORK_MULTIPLAYER
			automaticallyEndTurns = s_unitMoveTurnSlice + (CvPreGame::isNetworkMultiplayerGame() ? 100 : 10) < getTurnSlice();
#else
			automaticallyEndTurns = s_unitMoveTurnSlice + 10 < getTurnSlice();
#endif

		if(automaticallyEndTurns)
		{
			bool hasSelection = false;

			auto_ptr<ICvUnit1> pDllHeadSelectedUnit(pkIface->GetHeadSelectedUnit());
			if(pDllHeadSelectedUnit.get() != NULL)
			{
				hasSelection = pDllHeadSelectedUnit->IsSelected();
			}

			if(!hasSelection && !pkIface->waitingForRemotePlayers())
			{
				if(m_endTurnTimerSemaphore < 1)
				{
					if(pkIface->canEndTurn() && gDLL->allAICivsProcessedThisTurn() && allUnitAIProcessed() && !gDLL->HasSentTurnComplete())
					{
						activePlayer.GetPlayerAchievements().EndTurn();
						gDLL->sendTurnComplete();
						CvAchievementUnlocker::EndTurn();
						m_endTurnTimer.Start();
					}
				}
			}
		}
	}

	if(activePlayer.isTurnActive())
	{

		EndTurnBlockingTypes eEndTurnBlockingType = NO_ENDTURN_BLOCKING_TYPE;
		int iNotificationIndex = -1;
		activePlayer.GetNotifications()->GetEndTurnBlockedType(eEndTurnBlockingType, iNotificationIndex);

		if(eEndTurnBlockingType == NO_ENDTURN_BLOCKING_TYPE)
		{

			if(activePlayer.hasPromotableUnit() && !GC.getGame().isOption(GAMEOPTION_PROMOTION_SAVING))
			{
				eEndTurnBlockingType = ENDTURN_BLOCKING_UNIT_PROMOTION;
			}
			else if(activePlayer.hasReadyUnit())
			{
				const CvUnit* pUnit = activePlayer.GetFirstReadyUnit();
				CvAssertMsg(pUnit, "GetFirstReadyUnit is returning null");
				if(pUnit)
				{
					if(!pUnit->canHold(pUnit->plot()))
					{
						eEndTurnBlockingType = ENDTURN_BLOCKING_STACKED_UNITS;
					}
					else
					{
						eEndTurnBlockingType = ENDTURN_BLOCKING_UNITS;
					}
				}
			}
		}

		if(eEndTurnBlockingType == NO_ENDTURN_BLOCKING_TYPE)
		{
			if(!(activePlayer.hasBusyUnitOrCity()) && !(activePlayer.hasReadyUnit()))
			{



				if(!isGameMultiPlayer())
				{
					if((activePlayer.isOption(PLAYEROPTION_WAIT_END_TURN) && !isGameMultiPlayer()) || !(GC.GetEngineUserInterface()->isHasMovedUnit()) || isHotSeat() || isPbem())
					{
						GC.GetEngineUserInterface()->setCanEndTurn(true);
					}
				}
				else
				{
					if(activePlayer.hasAutoUnit() && !m_sentAutoMoves)
					{
						if(!(gDLL->shiftKey()))
						{
							gDLL->sendAutoMoves();
							m_sentAutoMoves = true;
						}
					}
					else
					{
						if((activePlayer.isOption(PLAYEROPTION_WAIT_END_TURN) && !isGameMultiPlayer()) || !(GC.GetEngineUserInterface()->isHasMovedUnit()) || isHotSeat() || isPbem())
						{
							GC.GetEngineUserInterface()->setCanEndTurn(true);
						}
						else
						{
							if(GC.GetEngineUserInterface()->getEndTurnCounter() > 0)
							{
								GC.GetEngineUserInterface()->changeEndTurnCounter(-1);
							}
							else
							{
								if(!gDLL->HasSentTurnComplete() && gDLL->allAICivsProcessedThisTurn() && allUnitAIProcessed() && pkIface && pkIface->IsMPAutoEndTurnEnabled())
								{
									activePlayer.GetPlayerAchievements().EndTurn();
									gDLL->sendTurnComplete();
									CvAchievementUnlocker::EndTurn();
								}

								GC.GetEngineUserInterface()->setEndTurnCounter(3);
								if(isGameMultiPlayer())
								{
									GC.GetEngineUserInterface()->setCanEndTurn(true);
									m_endTurnTimer.Start();
								}
							}
						}
					}
				}
			}
		}

		activePlayer.SetEndTurnBlocking(eEndTurnBlockingType, iNotificationIndex);
	}
}


void CvGame::testExtendedGame()
{
	int iI;

	if(getGameState() != GAMESTATE_OVER)
	{
		return;
	}

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if(GET_PLAYER((PlayerTypes)iI).isHuman())
			{
				if(GET_PLAYER((PlayerTypes)iI).isExtendedGame())
				{
					setGameState(GAMESTATE_EXTENDED);
					break;
				}
			}
		}
	}
}



CvUnit* CvGame::getPlotUnit(CvPlot* pPlot, int iIndex)
{
	IDInfo* pUnitNode1;
	IDInfo* pUnitNode2;
	CvUnit* pLoopUnit1;
	CvUnit* pLoopUnit2;
	int iCount;
	int iPass;
	PlayerTypes activePlayer = getActivePlayer();
	TeamTypes activeTeam = getActiveTeam();

	if(pPlot != NULL)
	{
		iCount = 0;

		for(iPass = 0; iPass < 2; iPass++)
		{
			pUnitNode1 = pPlot->headUnitNode();

			while(pUnitNode1 != NULL)
			{
				pLoopUnit1 = ::getUnit(*pUnitNode1);
				pUnitNode1 = pPlot->nextUnitNode(pUnitNode1);

				if(!(pLoopUnit1->isInvisible(activeTeam, true)))
				{
					if(!(pLoopUnit1->isCargo()))
					{
						if((pLoopUnit1->getOwner() == activePlayer) == (iPass == 0))
						{
							if(iCount == iIndex)
							{
								return pLoopUnit1;
							}

							iCount++;

							if(pLoopUnit1->hasCargo())
							{
								pUnitNode2 = pPlot->headUnitNode();

								while(pUnitNode2 != NULL)
								{
									pLoopUnit2 = ::getUnit(*pUnitNode2);
									pUnitNode2 = pPlot->nextUnitNode(pUnitNode2);

									if(!(pLoopUnit2->isInvisible(activeTeam, true)))
									{
										if(pLoopUnit2->getTransportUnit() == pLoopUnit1)
										{
											if(iCount == iIndex)
											{
												return pLoopUnit2;
											}

											iCount++;
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

	return NULL;
}


void CvGame::getPlotUnits(CvPlot* pPlot, std::vector<CvUnit*>& plotUnits)
{
	plotUnits.erase(plotUnits.begin(), plotUnits.end());

	IDInfo* pUnitNode1;
	IDInfo* pUnitNode2;
	CvUnit* pLoopUnit1;
	CvUnit* pLoopUnit2;
	int iPass;
	PlayerTypes activePlayer = getActivePlayer();
	TeamTypes activeTeam = getActiveTeam();

	if(pPlot != NULL)
	{
		for(iPass = 0; iPass < 2; iPass++)
		{
			pUnitNode1 = pPlot->headUnitNode();

			while(pUnitNode1 != NULL)
			{
				pLoopUnit1 = ::getUnit(*pUnitNode1);
				pUnitNode1 = pPlot->nextUnitNode(pUnitNode1);

				if(!(pLoopUnit1->isInvisible(activeTeam, true)))
				{
					if(!(pLoopUnit1->isCargo()))
					{
						if((pLoopUnit1->getOwner() == activePlayer) == (iPass == 0))
						{
							plotUnits.push_back(pLoopUnit1);

							if(pLoopUnit1->hasCargo())
							{
								pUnitNode2 = pPlot->headUnitNode();

								while(pUnitNode2 != NULL)
								{
									pLoopUnit2 = ::getUnit(*pUnitNode2);
									pUnitNode2 = pPlot->nextUnitNode(pUnitNode2);

									if(!(pLoopUnit2->isInvisible(activeTeam, true)))
									{
										if(pLoopUnit2->getTransportUnit() == pLoopUnit1)
										{
											plotUnits.push_back(pLoopUnit2);
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


void CvGame::cycleCities(bool bForward, bool bAdd)
{
	CvCity* pSelectCity;
	CvCity* pLoopCity;
	int iLoop;

	pSelectCity = NULL;

	auto_ptr<ICvCity1> pHeadSelectedCity(GC.GetEngineUserInterface()->getHeadSelectedCity());

	CvCity* pkHeadSelectedCity = GC.UnwrapCityPointer(pHeadSelectedCity.get());

	if((pkHeadSelectedCity != NULL) && ((pkHeadSelectedCity->getTeam() == getActiveTeam()) || isDebugMode()))
	{
		iLoop = pkHeadSelectedCity->getIndex();

		iLoop += (bForward ? 1 : -1);

		do
		{
			pLoopCity = GET_PLAYER(pkHeadSelectedCity->getOwner()).nextCity(&iLoop, !bForward);

			if(pLoopCity == NULL)
			{
				pLoopCity = GET_PLAYER(pkHeadSelectedCity->getOwner()).firstCity(&iLoop, !bForward);
			}

			if((pLoopCity != NULL) && (pLoopCity != pkHeadSelectedCity) && !pLoopCity->IsPuppet())
			{
				pSelectCity = pLoopCity;
			}

		}
		while((pLoopCity != pkHeadSelectedCity) && !pSelectCity);

	}
	else
	{
		pSelectCity = GET_PLAYER(getActivePlayer()).firstCity(&iLoop, !bForward);
	}

	if(pSelectCity != NULL)
	{
		auto_ptr<ICvCity1> pDllSelectedCity = GC.WrapCityPointer(pSelectCity);
		if(bAdd)
		{
			GC.GetEngineUserInterface()->clearSelectedCities();
			GC.GetEngineUserInterface()->addSelectedCity(pDllSelectedCity.get());
		}
		else
		{
			GC.GetEngineUserInterface()->selectCity(pDllSelectedCity.get());
		}
	}
}


void CvGame::cycleUnits(bool bClear, bool bForward, bool bWorkers)
{
	CvUnit* pNextUnit;
#ifndef AUI_WARNING_FIXES
	CvCity* pCycleCity = NULL;
#endif
	bool bWrap = false;
	bool bProcessed = false;
	PlayerTypes eActivePlayer = getActivePlayer();
	ICvUserInterface2* pUI = GC.GetEngineUserInterface();
	CvPlayerAI& theActivePlayer = GET_PLAYER(eActivePlayer);

	auto_ptr<ICvUnit1> pDllSelectedUnit(pUI->GetHeadSelectedUnit());
	CvUnit* pCycleUnit = GC.UnwrapUnitPointer(pDllSelectedUnit.get());

	if(pCycleUnit != NULL)
	{
		if(pCycleUnit->getOwner() != eActivePlayer)
		{
			pCycleUnit = NULL;
		}

		pNextUnit = theActivePlayer.GetUnitCycler().Cycle(pCycleUnit, bForward, bWorkers, &bWrap);

		if(bWrap)
		{
			if(theActivePlayer.hasAutoUnit())
			{
				gDLL->sendAutoMoves();
			}
		}
	}
	else
	{
		pNextUnit = GC.getMap().findUnit(0, 0, eActivePlayer, true, bWorkers);
	}

	if(pNextUnit != NULL && !bWrap)
	{
		CvAssert(pNextUnit->getOwner() == eActivePlayer);
		selectUnit(pNextUnit, bClear);
		bProcessed = true;
	}

#ifdef AUI_WARNING_FIXES
	if (pNextUnit != NULL                              && !bProcessed)
#else
	if(pNextUnit != NULL              && !pCycleCity && !bProcessed)
#endif
	{
		CvAssert(pNextUnit->getOwner() == eActivePlayer);
		selectUnit(pNextUnit, bClear);
	}

	if(pNextUnit == NULL && pCycleUnit != NULL && pCycleUnit->getOwner() == eActivePlayer)
	{
		pUI->ClearSelectionList();
		pCycleUnit->plot()->updateCenterUnit();
	}

	pDllSelectedUnit.reset(pUI->GetHeadSelectedUnit());
	CvUnit* pCurrentSelectedUnit = GC.UnwrapUnitPointer(pDllSelectedUnit.get());
#ifdef AUI_WARNING_FIXES
	if ((pCycleUnit != pCurrentSelectedUnit) || ((pCycleUnit != NULL) && pCycleUnit->ReadyToSelect())                  )
#else
	if((pCycleUnit != pCurrentSelectedUnit) || ((pCycleUnit != NULL) && pCycleUnit->ReadyToSelect()) || pCycleCity)
#endif
	{
		pUI->lookAtSelectionPlot();
	}
}



bool CvGame::cyclePlotUnits(CvPlot* pPlot, bool bForward, bool bAuto, int iCount)
{
	IDInfo* pUnitNode;
	CvUnit* pSelectedUnit;
	CvUnit* pLoopUnit = NULL;

	CvAssertMsg(iCount >= -1, "iCount expected to be >= -1");

	if(iCount == -1)
	{
		pUnitNode = pPlot->headUnitNode();

		while(pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(*pUnitNode);

			if(NULL != pLoopUnit && pLoopUnit->IsSelected())
			{
				break;
			}

			pUnitNode = pPlot->nextUnitNode(pUnitNode);
		}
	}
	else
	{
		pUnitNode = pPlot->headUnitNode();

		while(pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(*pUnitNode);

			if((iCount - 1) == 0)
			{
				break;
			}

			if(iCount > 0)
			{
				iCount--;
			}

			pUnitNode = pPlot->nextUnitNode(pUnitNode);
		}

		if(pUnitNode == NULL)
		{
			pUnitNode = pPlot->tailUnitNode();

			if(pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(*pUnitNode);
			}
		}
	}

	if(pUnitNode != NULL)
	{
		pSelectedUnit = pLoopUnit;

		while(true)
		{
			if(bForward)
			{
				pUnitNode = pPlot->nextUnitNode(pUnitNode);
				if(pUnitNode == NULL)
				{
					pUnitNode = pPlot->headUnitNode();
				}
			}
			else
			{
				pUnitNode = pPlot->prevUnitNode(pUnitNode);
				if(pUnitNode == NULL)
				{
					pUnitNode = pPlot->tailUnitNode();
				}
			}

			pLoopUnit = ::getUnit(*pUnitNode);

			if(iCount == -1)
			{
				if(pLoopUnit == pSelectedUnit)
				{
					break;
				}
			}

			if(NULL != pLoopUnit && pLoopUnit->getOwner() == getActivePlayer())
			{
				if(bAuto)
				{
					if(pLoopUnit->ReadyToSelect())
					{
						                                 selectUnit(pLoopUnit, true);
						return true;
					}
				}
				else
				{
					auto_ptr<ICvUnit1> pDllLoopUnit = GC.WrapUnitPointer(pLoopUnit);
					GC.GetEngineUserInterface()->InsertIntoSelectionList(pDllLoopUnit.get(), true, false);
					return true;
				}
			}

			if(pLoopUnit == pSelectedUnit)
			{
				break;
			}
		}
	}

	return false;
}



void CvGame::selectionListMove(CvPlot* pPlot, bool bShift)
{
	if(pPlot == NULL)
	{
		return;
	}

	auto_ptr<ICvUnit1> pSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
	CvUnit* pkSelectedUnit = GC.UnwrapUnitPointer(pSelectedUnit.get());

	if((pkSelectedUnit == NULL) || (pkSelectedUnit->getOwner() != getActivePlayer()))
	{
		return;
	}

	if(pkSelectedUnit != NULL)
	{
		if(pkSelectedUnit->CanSwapWithUnitHere(*pPlot))
		{
			selectionListGameNetMessage(GAMEMESSAGE_SWAP_UNITS, CvTypes::getMISSION_SWAP_UNITS(), pPlot->getX(), pPlot->getY(), 0, false, bShift);
		}
		else
		{
			selectionListGameNetMessage(GAMEMESSAGE_PUSH_MISSION, CvTypes::getMISSION_MOVE_TO(), pPlot->getX(), pPlot->getY(), 0, false, bShift);
		}
	}
}



void CvGame::selectionListGameNetMessage(int eMessage, int iData2, int iData3, int iData4, int iFlags, bool bAlt, bool bShift)
{
	auto_ptr<ICvUnit1> pSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
	CvUnit* pkSelectedUnit = GC.UnwrapUnitPointer(pSelectedUnit.get());

	if(pkSelectedUnit != NULL)
	{
		if(pkSelectedUnit->getOwner() == getActivePlayer() && !pSelectedUnit->IsBusy())
		{
#ifdef GAME_ALLOW_ONLY_ONE_UNIT_MOVE_ON_TURN_LOADING
			if (GC.getGame().isOption("GAMEOPTION_FIRSTMOVE"))
			{
				if (isGameMultiPlayer())
				{
					float t1;
					float t2;
					GetTurnTimerData(t1, t2);

					bool bAllComplete = true;
					for (uint i = 0; i < MAX_CIV_PLAYERS; i++)
					{
						CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)i);
						if (kPlayer.isHuman() && kPlayer.isAlive()) {
							if (!gDLL->HasReceivedTurnComplete((PlayerTypes)i))
								bAllComplete = false;
						}
					}



					if (bAllComplete == getHasReceivedFirstMission()) {
						if (isMPOrderedMoveOnTurnLoading()) {


							return;
						}
						else {

							setMPOrderedMoveOnTurnLoading(true);
						}
					}



				}
			}
#endif
			if(eMessage == GAMEMESSAGE_DO_COMMAND)
			{
				gDLL->sendDoCommand(pkSelectedUnit->GetID(), ((CommandTypes)iData2), iData3, iData4, bAlt);
			}
			else if((eMessage == GAMEMESSAGE_PUSH_MISSION) || (eMessage == GAMEMESSAGE_AUTO_MISSION))
			{
				if(eMessage == GAMEMESSAGE_PUSH_MISSION)
				{
					MissionTypes eMission = (MissionTypes)iData2;
					CvPlot* pPlot = GC.getMap().plot(iData3, iData4);
					if(pPlot && pkSelectedUnit->CanSwapWithUnitHere(*pPlot) && eMission != CvTypes::getMISSION_ROUTE_TO())
					{
						gDLL->sendSwapUnits(pkSelectedUnit->GetID(), ((MissionTypes)iData2), iData3, iData4, iFlags, bShift);
					}
					else
					{
						gDLL->sendPushMission(pkSelectedUnit->GetID(), ((MissionTypes)iData2), iData3, iData4, iFlags, bShift);
					}
				}
				else
				{
					gDLL->sendAutoMission(pkSelectedUnit->GetID());
				}
			}
			else if((eMessage == GAMEMESSAGE_SWAP_UNITS))
			{
					gDLL->sendSwapUnits(pkSelectedUnit->GetID(), ((MissionTypes)iData2), iData3, iData4, iFlags, bShift);
			}
			else
			{
				CvAssert(false);
			}
		}
	}
}



void CvGame::selectedCitiesGameNetMessage(int eMessage, int iData2, int iData3, int iData4, bool bOption, bool bAlt, bool bShift, bool bCtrl)
{
	const IDInfo* pSelectedCityNode;
	CvCity* pSelectedCity;

	pSelectedCityNode = GC.GetEngineUserInterface()->headSelectedCitiesNode();

	while(pSelectedCityNode != NULL)
	{
		pSelectedCity = ::getCity(*pSelectedCityNode);
		pSelectedCityNode = GC.GetEngineUserInterface()->nextSelectedCitiesNode(pSelectedCityNode);
		CvAssert(pSelectedCity);

		if(pSelectedCity != NULL)
		{
			if(pSelectedCity->getOwner() == getActivePlayer())
			{
#ifdef GAME_ALLOW_ONLY_ONE_UNIT_MOVE_ON_TURN_LOADING
				if (GC.getGame().isOption("GAMEOPTION_FIRSTMOVE"))
				{
					if ((eMessage == GAMEMESSAGE_DO_TASK) && (iData2 == TASK_RANGED_ATTACK))
					{
						if (isGameMultiPlayer())
						{
							float t1;
							float t2;
							GetTurnTimerData(t1, t2);

							bool bAllComplete = true;
							for (uint i = 0; i < MAX_CIV_PLAYERS; i++)
							{
								CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)i);
								if (kPlayer.isHuman() && kPlayer.isAlive()) {
									if (!gDLL->HasReceivedTurnComplete((PlayerTypes)i))
										bAllComplete = false;
								}
							}



							if (bAllComplete == getHasReceivedFirstMission()) {
								if (isMPOrderedMoveOnTurnLoading()) {


									return;
								}
								else {

									setMPOrderedMoveOnTurnLoading(true);
								}
							}



						}
					}
				}
#endif
				switch(eMessage)
				{
				case GAMEMESSAGE_PUSH_ORDER:
					cityPushOrder(pSelectedCity, ((OrderTypes)iData2), iData3, bAlt, bShift, bCtrl);
					break;

				case GAMEMESSAGE_POP_ORDER:
					if(pSelectedCity->getOrderQueueLength() >= 1)
					{
						gDLL->sendPopOrder(pSelectedCity->GetID(), iData2);
					}
					break;

				case GAMEMESSAGE_SWAP_ORDER:
					if(pSelectedCity->getOrderQueueLength() >= 2)
					{
						gDLL->sendSwapOrder(pSelectedCity->GetID(), iData2);
					}
					break;

				case GAMEMESSAGE_DO_TASK:
					gDLL->sendDoTask(pSelectedCity->GetID(), ((TaskTypes)iData2), iData3, iData4, bOption, bAlt, bShift, bCtrl);
					break;

				default:
					CvAssert(false);
					break;
				}
			}
		}
	}
}



void CvGame::cityPushOrder(CvCity* pCity, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl)
{
	CvAssert(pCity);
	if(!pCity) return;
	gDLL->sendPushOrder(pCity->GetID(), eOrder, iData, bAlt, bShift, bCtrl);
}


void CvGame::CityPurchase(CvCity* pCity, UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectType, YieldTypes ePurchaseYield)
{
	CvAssert(pCity);
	if(!pCity) return;


	if(eUnitType >= 0)
	{

		if(!pCity->CanPlaceUnitHere(eUnitType))
		{
			return;
		}
	}

	gDLL->sendPurchase(pCity->GetID(), eUnitType, eBuildingType, eProjectType, ePurchaseYield);
}


void CvGame::selectUnit(CvUnit* pUnit, bool bClear, bool bToggle, bool bSound)
{
	auto_ptr<ICvUnit1> pOldSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
	CvUnit* pkOldSelectedUnit = GC.UnwrapUnitPointer(pOldSelectedUnit.get());

	GC.GetEngineUserInterface()->clearSelectedCities();

	bool bGroup = true;
	if(bClear)
	{
		GC.GetEngineUserInterface()->ClearSelectionList();
		bGroup = false;
	}

	pUnit->IncrementFirstTimeSelected();

	auto_ptr<ICvUnit1> pDllUnit = GC.WrapUnitPointer(pUnit);
	GC.GetEngineUserInterface()->InsertIntoSelectionList(pDllUnit.get(), true, bToggle, bGroup, bSound);

	gDLL->GameplayMinimapUnitSelect(pUnit->getX(), pUnit->getY());

	GC.GetEngineUserInterface()->makeSelectionListDirty();

	bool bHighlightTrade = false;
	int iRouteIndex = -1;
	if(GC.getGame().getActivePlayer() == pUnit->getOwner())
	{
		CvMap& theMap = GC.getMap();
		theMap.updateDeferredFog();
		if(pkOldSelectedUnit)
		{
			pkOldSelectedUnit->plot()->updateCenterUnit();
		}
		pUnit->plot()->updateCenterUnit();

		if (pUnit->isTrade())
		{
			iRouteIndex = GC.getGame().GetGameTrade()->GetIndexFromUnitID( pUnit->GetID(), pUnit->getOwner() );
			if (iRouteIndex != -1)
				bHighlightTrade = true;
		}
	}
	
	if (bHighlightTrade)
		gDLL->TradeVisuals_ActivatePopupRoute(iRouteIndex);
	else
		gDLL->TradeVisuals_DeactivatePopupRoute();
}

static void IfTradeUnit_DisplayPopupTradeRoute(CvUnit *pUnit)
{
	int iRouteIndex;

	iRouteIndex = -1;
	if (pUnit && pUnit->isTrade())
	{
		iRouteIndex = GC.getGame().GetGameTrade()->GetIndexFromUnitID(pUnit->GetID(),pUnit->getOwner());
		if (iRouteIndex != -1) {
			gDLL->TradeVisuals_ActivatePopupRoute(iRouteIndex);
		}
	}	
}
void CvGame::mouseoverUnit(CvUnit *pUnit, bool bEnter)
{
	CvUnit *pkSelectedUnit;

	if (pUnit)
	{
		if (bEnter) {
			IfTradeUnit_DisplayPopupTradeRoute(pUnit);
			m_iLastMouseoverUnitID = pUnit->GetID();
		} else {
			if (pUnit->GetID() == m_iLastMouseoverUnitID) {
				gDLL->TradeVisuals_DeactivatePopupRoute();


				auto_ptr<ICvUnit1> pSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
				pkSelectedUnit = GC.UnwrapUnitPointer(pSelectedUnit.get());
				IfTradeUnit_DisplayPopupTradeRoute(pkSelectedUnit);
			}
		}
	}
}


void CvGame::selectGroup(CvUnit* pUnit, bool bShift, bool bCtrl, bool bAlt)
{
	IDInfo* pUnitNode;
	CvPlot* pUnitPlot;
	bool bGroup;

	CvAssertMsg(pUnit != NULL, "pUnit == NULL unexpectedly");

	if(bAlt || bCtrl)
	{
		GC.GetEngineUserInterface()->clearSelectedCities();

		if(!bShift)
		{
			GC.GetEngineUserInterface()->ClearSelectionList();
			bGroup = true;
		}
		else
		{
			bGroup = true;
		}

		if(!pUnit) return;

		pUnitPlot = pUnit->plot();

		pUnitNode = pUnitPlot->headUnitNode();

		while(pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(*pUnitNode);
			pUnitNode = pUnitPlot->nextUnitNode(pUnitNode);

			if(NULL != pLoopUnit && pLoopUnit->getOwner() == getActivePlayer())
			{
				if(pLoopUnit->canMove())
				{
					CvPlayerAI* pOwnerPlayer = &(GET_PLAYER(pLoopUnit->getOwner()));
#ifdef AUI_GAME_BETTER_HYBRID_MODE
					if (!pOwnerPlayer->isHuman() || !isAnySimultaneousTurns() || !pOwnerPlayer->isTurnActive() || getTurnSlice() - pLoopUnit->getLastMoveTurn() > GC.getMIN_TIMER_UNIT_DOUBLE_MOVES())
#else
					if( !pOwnerPlayer->isSimultaneousTurns() || getTurnSlice() - pLoopUnit->getLastMoveTurn() > GC.getMIN_TIMER_UNIT_DOUBLE_MOVES())
#endif
					{
						if(bAlt || (pLoopUnit->getUnitType() == pUnit->getUnitType()))
						{
							auto_ptr<ICvUnit1> pDllLoopUnit = GC.WrapUnitPointer(pLoopUnit);
							GC.GetEngineUserInterface()->InsertIntoSelectionList(pDllLoopUnit.get(), true, false, bGroup, false, true);
						}
					}
				}
			}
		}
	}
	else
	{
		auto_ptr<ICvUnit1> pDllUnit = GC.WrapUnitPointer(pUnit);
		GC.GetEngineUserInterface()->selectUnit(pDllUnit.get(), !bShift, bShift, true);
	}
}



void CvGame::selectAll(CvPlot* pPlot)
{
	CvUnit* pSelectUnit = NULL;

	if(pPlot != NULL)
	{
		CvUnit* pCenterUnit = pPlot->getCenterUnit().pointer();

		if((pCenterUnit != NULL) && (pCenterUnit->getOwner() == getActivePlayer()))
		{
			pSelectUnit = pCenterUnit;
		}
	}

	if(pSelectUnit != NULL)
	{
		auto_ptr<ICvUnit1> pDllSelectUnit = GC.WrapUnitPointer(pSelectUnit);
		GC.GetEngineUserInterface()->selectGroup(pDllSelectUnit.get(), false, false, true);
	}
}


void CvGame::SelectSettler(void)
{
	CvUnit* pSettlerUnit = NULL;
	CvPlayerAI* pActivePlayer = &(GET_PLAYER(getActivePlayer()));

	CvUnit* pLoopUnit = NULL;
	int iUnitIndex;
	for(pLoopUnit = pActivePlayer->firstUnit(&iUnitIndex); pLoopUnit != NULL; pLoopUnit = pActivePlayer->nextUnit(&iUnitIndex))
	{
		if(pLoopUnit->isFound())
		{
			pSettlerUnit = pLoopUnit;
			break;
		}
	}

	if(pSettlerUnit && pSettlerUnit->ReadyToSelect())
	{
		selectUnit(pSettlerUnit, true, false, true);
	}
}



bool CvGame::selectionListIgnoreBuildingDefense()
{
	bool bIgnoreBuilding = false;
	bool bAttackLandUnit = false;
	auto_ptr<ICvUnit1> pSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
	CvUnit* pkSelectedUnit = GC.UnwrapUnitPointer(pSelectedUnit.get());

	if(pkSelectedUnit != NULL)
	{
		if(pkSelectedUnit->ignoreBuildingDefense())
		{
			bIgnoreBuilding = true;
		}

		if((pkSelectedUnit->getDomainType() == DOMAIN_LAND) && pkSelectedUnit->IsCanAttack())
		{
			bAttackLandUnit = true;
		}
	}

	if(!bIgnoreBuilding && !bAttackLandUnit)
	{
		const UnitTypes eBestLandUnit = getBestLandUnit();
		if(eBestLandUnit != NO_UNIT)
		{
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eBestLandUnit);
			if(pkUnitInfo)
			{
				bIgnoreBuilding = pkUnitInfo->IsIgnoreBuildingDefense();
			}
		}
	}

	return bIgnoreBuilding;
}



bool CvGame::canHandleAction(int iAction, CvPlot* pPlot, bool bTestVisible)
{
	CvPlot* pMissionPlot;
	bool bShift = gDLL->shiftKey();

	CvActionInfo* pActionInfo = GC.getActionInfo(iAction);
	CvAssert(pActionInfo != NULL);
	if(!pActionInfo) return false;

	if(pActionInfo->getControlType() != NO_CONTROL)
	{
		if(canDoControl((ControlTypes)(GC.getActionInfo(iAction)->getControlType())))
		{
			return true;
		}
	}

	if(GC.GetEngineUserInterface()->isCitySelection())
	{
		return false;
	}

	auto_ptr<ICvUnit1> pHeadSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
	CvUnit* pkHeadSelectedUnit = GC.UnwrapUnitPointer(pHeadSelectedUnit.get());

	if(pkHeadSelectedUnit != NULL)
	{
		if(pkHeadSelectedUnit->getOwner() == getActivePlayer())
		{
#ifdef AUI_GAME_BETTER_HYBRID_MODE
			if (GET_PLAYER(pkHeadSelectedUnit->getOwner()).isTurnActive())
#else
			if(GET_PLAYER(pkHeadSelectedUnit->getOwner()).isSimultaneousTurns() || GET_PLAYER(pkHeadSelectedUnit->getOwner()).isTurnActive())
#endif
			{
				if(GC.getActionInfo(iAction)->getMissionType() != NO_MISSION)
				{
					if(pPlot != NULL)
					{
						pMissionPlot = pPlot;
					}
					else if(bShift)
					{
						pMissionPlot = pkHeadSelectedUnit->LastMissionPlot();
					}
					else
					{
						pMissionPlot = NULL;
					}

					if((pMissionPlot == NULL) || !(pMissionPlot->isVisible(pkHeadSelectedUnit->getTeam())))
					{
						pMissionPlot = pkHeadSelectedUnit->plot();
					}

					if(pkHeadSelectedUnit->CanStartMission(pActionInfo->getMissionType(), pActionInfo->getMissionData(), -1, pMissionPlot, bTestVisible))
					{
						return true;
					}
				}

				if(GC.getActionInfo(iAction)->getCommandType() != NO_COMMAND)
				{
					if(pkHeadSelectedUnit->canDoCommand(((CommandTypes)(pActionInfo->getCommandType())), pActionInfo->getCommandData(), -1, bTestVisible))
					{
						return true;
					}
				}

				if(GC.GetEngineUserInterface()->CanDoInterfaceMode((InterfaceModeTypes)pActionInfo->getInterfaceModeType(), bTestVisible))
				{
					return true;
				}
			}
		}
	}

	return false;
}



void CvGame::handleAction(int iAction)
{
	bool bAlt;
	bool bShift;
	bool bSkip;

	bAlt = gDLL->altKey();
	bShift = false;

	auto_ptr<ICvUnit1> pHeadSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());

	if(!(canHandleAction(iAction)))
	{
		return;
	}


	CvActionInfo* pkActionInfo = GC.getActionInfo(iAction);
	if(pkActionInfo->getControlType() != NO_CONTROL)
	{
		doControl((ControlTypes)(pkActionInfo->getControlType()));
	}


	if(GC.GetEngineUserInterface()->CanDoInterfaceMode((InterfaceModeTypes)pkActionInfo->getInterfaceModeType()))
	{
		if(pHeadSelectedUnit.get() != NULL)
		{
			if(GC.getInterfaceModeInfo((InterfaceModeTypes)pkActionInfo->getInterfaceModeType())->getSelectAll())
			{
				GC.GetEngineUserInterface()->selectGroup(pHeadSelectedUnit.get(), false, false, true);
			}
			else if(GC.getInterfaceModeInfo((InterfaceModeTypes)pkActionInfo->getInterfaceModeType())->getSelectType())
			{
				GC.GetEngineUserInterface()->selectGroup(pHeadSelectedUnit.get(), false, true, false);
			}
		}

		GC.GetEngineUserInterface()->setInterfaceMode((InterfaceModeTypes)pkActionInfo->getInterfaceModeType());
	}


	int iMissionType = pkActionInfo->getMissionType();
	if(iMissionType != NO_MISSION)
	{
		bool bSkipMissionAdd = false;

		if(iMissionType == CvTypes::getMISSION_BUILD() || iMissionType == CvTypes::getMISSION_FOUND())
		{
			bool bShowConfirmPopup = false;

			if(iMissionType == CvTypes::getMISSION_BUILD())
			{
				int iBuild = pkActionInfo->getMissionData();
				CvBuildInfo* pBuildInfo = GC.getBuildInfo((BuildTypes)iBuild);


				if(!pBuildInfo->isRepair())
				{

					if(pBuildInfo->getRoute() == NO_ROUTE)
					{

						CvImprovementEntry *pImprovementInfo = GC.getImprovementInfo((ImprovementTypes)pBuildInfo->getImprovement());
						if (!pImprovementInfo || !pImprovementInfo->IsRequiresImprovement())
						{
							if(pHeadSelectedUnit.get() != NULL)
							{
								CvUnit* pkHeadSelectedUnit = GC.UnwrapUnitPointer(pHeadSelectedUnit.get());
								CvPlot* pPlot = pkHeadSelectedUnit->plot();
								if(pPlot != NULL)
								{
									if(pPlot->getImprovementType() != NO_IMPROVEMENT && !(pPlot->getFeatureType() == FEATURE_FALLOUT && pBuildInfo->isFeatureRemove(FEATURE_FALLOUT)))
									{
										bShowConfirmPopup = true;
									}
								}
							}
						}
					}
				}
			}
			else
			if(iMissionType == CvTypes::getMISSION_FOUND())
			{
				if(pHeadSelectedUnit.get() != NULL)
				{
					CvUnit* pkHeadSelectedUnit = GC.UnwrapUnitPointer(pHeadSelectedUnit.get());
					CvPlot* pPlot = pkHeadSelectedUnit->plot();
					if(pPlot != NULL)
					{
						ResourceTypes eArtifactResourceType = static_cast<ResourceTypes>(GC.getARTIFACT_RESOURCE());
						ResourceTypes eHiddenArtifactResourceType = static_cast<ResourceTypes>(GC.getARTIFACT_RESOURCE());
						if (pPlot->getResourceType() == eArtifactResourceType || pPlot->getResourceType() == eHiddenArtifactResourceType)
						{
							bShowConfirmPopup = true;
						}
					}
				}
			}

			if(bShowConfirmPopup)
			{
				int iBuild = pkActionInfo->getMissionData();
				CvPopupInfo kPopupInfo(BUTTONPOPUP_CONFIRM_IMPROVEMENT_REBUILD, iAction, iBuild);
				kPopupInfo.bOption1 = bAlt;
				GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
				bSkipMissionAdd = true;
			}
		}
		else
		if (iMissionType == CvTypes::getMISSION_ESTABLISH_TRADE_ROUTE())
		{
			if(pHeadSelectedUnit.get() != NULL)
			{
				CvUnit* pkHeadSelectedUnit = GC.UnwrapUnitPointer(pHeadSelectedUnit.get());
				if (pkHeadSelectedUnit)
				{
					CvPopupInfo kPopup(BUTTONPOPUP_CHOOSE_INTERNATIONAL_TRADE_ROUTE, pkHeadSelectedUnit->getOwner());
					kPopup.iData2 = pkHeadSelectedUnit->getIndex();
					GC.GetEngineUserInterface()->AddPopup(kPopup);
				}
			}
			bSkipMissionAdd = true;
		}
		else
		if (iMissionType == CvTypes::getMISSION_CHANGE_TRADE_UNIT_HOME_CITY())
		{
			if(pHeadSelectedUnit.get() != NULL)
			{
				CvUnit* pkHeadSelectedUnit = GC.UnwrapUnitPointer(pHeadSelectedUnit.get());
				if (pkHeadSelectedUnit)
				{
					CvPopupInfo kPopup(BUTTONPOPUP_CHOOSE_TRADE_UNIT_NEW_HOME);
					kPopup.iData1 = pkHeadSelectedUnit->getIndex();
					GC.GetEngineUserInterface()->AddPopup(kPopup);
				}
			}
			bSkipMissionAdd = true;
		}
		if (iMissionType == CvTypes::getMISSION_CHANGE_ADMIRAL_PORT())
		{
			if(pHeadSelectedUnit.get() != NULL)
			{
				CvUnit* pkHeadSelectedUnit = GC.UnwrapUnitPointer(pHeadSelectedUnit.get());
				if (pkHeadSelectedUnit)
				{
					CvPopupInfo kPopup(BUTTONPOPUP_CHOOSE_ADMIRAL_PORT);
					kPopup.iData1 = pkHeadSelectedUnit->getIndex();
					GC.GetEngineUserInterface()->AddPopup(kPopup);
				}
			}
			bSkipMissionAdd = true;
		}

		if (!bSkipMissionAdd)
		{
			selectionListGameNetMessage(GAMEMESSAGE_PUSH_MISSION, iMissionType, pkActionInfo->getMissionData(), -1, 0, false, bShift);
			GC.GetEngineUserInterface()->setInterfaceMode(INTERFACEMODE_SELECTION);
		}
	}


	if(pkActionInfo->getCommandType() != NO_COMMAND)
	{
		bSkip = false;

		if(!bSkip)
		{
			if(pkActionInfo->isConfirmCommand())
			{
				CvPopupInfo kPopupInfo(BUTTONPOPUP_CONFIRMCOMMAND, iAction);
				kPopupInfo.bOption1 = bAlt;
				GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
			}
			else
			{
				selectionListGameNetMessage(GAMEMESSAGE_DO_COMMAND, pkActionInfo->getCommandType(), pkActionInfo->getCommandData(), -1, 0, bAlt);
			}
		}
	}
}



bool CvGame::canDoControl(ControlTypes eControl)
{
	switch(eControl)
	{
	case CONTROL_SELECTYUNITTYPE:
	case CONTROL_SELECTYUNITALL:
	case CONTROL_SELECT_HEALTHY:
	case CONTROL_SELECTCITY:
	case CONTROL_SELECTCAPITAL:
	case CONTROL_NEXTUNIT:
	case CONTROL_PREVUNIT:
	case CONTROL_CYCLEUNIT:
	case CONTROL_CYCLEUNIT_ALT:
	case CONTROL_CYCLEWORKER:
	case CONTROL_LASTUNIT:
#ifndef AUI_GAME_FIX_CONTROL_FORCE_END_TURN_CHECKS_FOR_BLOCKING
	case CONTROL_FORCEENDTURN:
#endif
	case CONTROL_AUTOMOVES:
	case CONTROL_SAVE_GROUP:
	case CONTROL_QUICK_SAVE:
	case CONTROL_QUICK_LOAD:
	case CONTROL_TURN_LOG:
		if(!GC.GetEngineUserInterface()->isFocused())
		{
			return true;
		}
		break;

	case CONTROL_PING:
	case CONTROL_YIELDS:
	case CONTROL_RESOURCE_ALL:
	case CONTROL_UNIT_ICONS:
	case CONTROL_SCORES:
	case CONTROL_OPTIONS_SCREEN:
	case CONTROL_DOMESTIC_SCREEN:
	case CONTROL_CIVILOPEDIA:
	case CONTROL_POLICIES_SCREEN:
	case CONTROL_FOREIGN_SCREEN:
	case CONTROL_MILITARY_SCREEN:
	case CONTROL_TECH_CHOOSER:
	case CONTROL_INFO:
	case CONTROL_SAVE_NORMAL:
	case CONTROL_ADVISOR_COUNSEL:
	case CONTROL_NEXTCITY:
	case CONTROL_PREVCITY:
	case CONTROL_RELIGION_OVERVIEW:
	case CONTROL_ESPIONAGE_OVERVIEW:
		return true;
		break;

	case CONTROL_VICTORY_SCREEN:
		if(getGameState() == GAMESTATE_ON)
		{
			return true;
		}
		break;


	case CONTROL_CENTERONSELECTION:
	{
		auto_ptr<ICvPlot1> pSelectionPlot(GC.GetEngineUserInterface()->getSelectionPlot());
		if(pSelectionPlot.get() != NULL)
		{
			return true;
		}
	}
	break;

	case CONTROL_LOAD_GAME:
		if(!(isNetworkMultiPlayer()))
		{
			return true;
		}
		break;

	case CONTROL_RETIRE:
		if((getGameState() == GAMESTATE_ON) || isGameMultiPlayer())
		{
			if(GET_PLAYER(getActivePlayer()).isAlive())
			{
				if(isPbem() || isHotSeat())
				{
					if(!GET_PLAYER(getActivePlayer()).isEndTurn())
					{
						return true;
					}
				}
				else
				{
					return true;
				}
			}
		}
		break;

	case CONTROL_ENDTURN:
	case CONTROL_ENDTURN_ALT:
		if(GC.GetEngineUserInterface()->canEndTurn() && !GC.GetEngineUserInterface()->isFocused())
		{
			return true;
		}
		break;

#ifdef AUI_GAME_FIX_CONTROL_FORCE_END_TURN_CHECKS_FOR_BLOCKING
	case CONTROL_FORCEENDTURN:
		{
			EndTurnBlockingTypes eBlock = GET_PLAYER(getActivePlayer()).GetEndTurnBlockingType();
			if ((eBlock == NO_ENDTURN_BLOCKING_TYPE || eBlock == ENDTURN_BLOCKING_UNITS) && !GC.GetEngineUserInterface()->isFocused())
			{
				return true;
			}
		}
		break;
#endif

	case CONTROL_TOGGLE_STRATEGIC_VIEW:
		GC.GetEngineUserInterface()->ToggleStrategicView();
		break;

	case CONTROL_RESTART_GAME:
		{
			if(!isGameMultiPlayer() && getGameTurn() == getStartTurn())
			{
				return true;
			}
		}
		break;

	default:
		CvAssertMsg(false, "eControl did not match any valid options");
		break;
	}

	return false;
}



void CvGame::doControl(ControlTypes eControl)
{
	if(!canDoControl(eControl))
	{
		return;
	}

	switch(eControl)
	{
	case CONTROL_CENTERONSELECTION:
		GC.GetEngineUserInterface()->lookAtSelectionPlot();
		break;

	case CONTROL_SELECTYUNITTYPE:
	{
		auto_ptr<ICvUnit1> pHeadSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
		if(pHeadSelectedUnit.get() != NULL)
		{
			GC.GetEngineUserInterface()->selectGroup(pHeadSelectedUnit.get(), false, true, false);
		}
	}
	break;

	case CONTROL_SELECTYUNITALL:
	{
		auto_ptr<ICvUnit1> pHeadSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
		if(pHeadSelectedUnit.get() != NULL)
		{
			GC.GetEngineUserInterface()->selectGroup(pHeadSelectedUnit.get(), false, false, true);
		}
	}
	break;

	case CONTROL_SELECT_HEALTHY:
	{
		auto_ptr<ICvUnit1> pHeadSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
		CvUnit* pkHeadSelectedUnit = GC.UnwrapUnitPointer(pHeadSelectedUnit.get());
		GC.GetEngineUserInterface()->ClearSelectionList();
		if(pkHeadSelectedUnit != NULL)
		{
			CvPlot* pHeadPlot = pkHeadSelectedUnit->plot();
			std::vector<CvUnit*> plotUnits;
			getPlotUnits(pHeadPlot, plotUnits);
			for(int iI = 0; iI < (int) plotUnits.size(); iI++)
			{
				CvUnit* pUnit = plotUnits[iI];

				if(pUnit->getOwner() == getActivePlayer())
				{
#ifdef AUI_GAME_BETTER_HYBRID_MODE
					CvPlayer& kUnitOwner = GET_PLAYER(pUnit->getOwner());
					if (!kUnitOwner.isHuman() || !isAnySimultaneousTurns() || !kUnitOwner.isTurnActive() || getTurnSlice() - pUnit->getLastMoveTurn() > GC.getMIN_TIMER_UNIT_DOUBLE_MOVES())
#else
					if(!GET_PLAYER(pUnit->getOwner()).isSimultaneousTurns() || getTurnSlice() - pUnit->getLastMoveTurn() > GC.getMIN_TIMER_UNIT_DOUBLE_MOVES())
#endif
					{
						if(pUnit->IsHurt())
						{
							auto_ptr<ICvUnit1> pDllUnit = GC.WrapUnitPointer(pUnit);
							GC.GetEngineUserInterface()->InsertIntoSelectionList(pDllUnit.get(), true, false, true, true, true);
						}
					}
				}
			}
		}
	}
	break;

	case CONTROL_SELECTCITY:
		if(GC.GetEngineUserInterface()->isCityScreenUp())
		{
			cycleCities();
		}
		else
		{
			GC.GetEngineUserInterface()->selectLookAtCity();
		}
		GC.GetEngineUserInterface()->lookAtSelectionPlot();
		break;

	case CONTROL_SELECTCAPITAL:
	{
		CvCity* pCapitalCity = GET_PLAYER(getActivePlayer()).getCapitalCity();
		if(pCapitalCity != NULL)
		{
			auto_ptr<ICvCity1> pDllCapitalCity = GC.WrapCityPointer(pCapitalCity);
			GC.GetEngineUserInterface()->selectCity(pDllCapitalCity.get());
		}
		GC.GetEngineUserInterface()->lookAtSelectionPlot();
	}
	break;

	case CONTROL_NEXTCITY:
		if(GC.GetEngineUserInterface()->isCitySelection())
		{
			cycleCities(true, !(GC.GetEngineUserInterface()->isCityScreenUp()));
		}
		else
		{
			GC.GetEngineUserInterface()->selectLookAtCity(true);
		}
		GC.GetEngineUserInterface()->lookAtSelectionPlot();
		break;

	case CONTROL_PREVCITY:
		if(GC.GetEngineUserInterface()->isCitySelection())
		{
			cycleCities(false, !(GC.GetEngineUserInterface()->isCityScreenUp()));
		}
		else
		{
			GC.GetEngineUserInterface()->selectLookAtCity(true);
		}
		GC.GetEngineUserInterface()->lookAtSelectionPlot();
		break;

	case CONTROL_NEXTUNIT:
	{
		auto_ptr<ICvPlot1> pSelectionPlot(GC.GetEngineUserInterface()->getSelectionPlot());
		CvPlot* pkSelectionPlot = GC.UnwrapPlotPointer(pSelectionPlot.get());
		if(pkSelectionPlot != NULL)
		{
			cyclePlotUnits(pkSelectionPlot);
		}
		break;
	}

	case CONTROL_PREVUNIT:
	{
		auto_ptr<ICvPlot1> pSelectionPlot(GC.GetEngineUserInterface()->getSelectionPlot());
		CvPlot* pkSelectionPlot = GC.UnwrapPlotPointer(pSelectionPlot.get());
		if(pkSelectionPlot != NULL)
		{
			cyclePlotUnits(pkSelectionPlot, false);
		}
		break;
	}

	case CONTROL_CYCLEUNIT:
	case CONTROL_CYCLEUNIT_ALT:
		cycleUnits(true);
		break;

	case CONTROL_CYCLEWORKER:
		cycleUnits(true, true, true);
		break;

	case CONTROL_LASTUNIT:
	{
		ICvUserInterface2* UI = GC.GetEngineUserInterface();
		auto_ptr<ICvUnit1> pUnit(UI->getLastSelectedUnit());

		if(pUnit.get() != NULL)
		{
			UI->selectUnit(pUnit.get(), true);
			UI->lookAtSelectionPlot();
		}
		else
		{
			cycleUnits(true, false);
		}

		UI->setLastSelectedUnit(NULL);
	}
	break;

	case CONTROL_ENDTURN:
	case CONTROL_ENDTURN_ALT:
		if(GC.GetEngineUserInterface()->canEndTurn() && gDLL->allAICivsProcessedThisTurn() && allUnitAIProcessed())
		{
			CvPlayerAI& kActivePlayer = GET_PLAYER(getActivePlayer());
			if (!isNetworkMultiPlayer() && kActivePlayer.isHuman() && GC.GetPostTurnAutosaves())
			{
				gDLL->AutoSave(false, true);
			}
			kActivePlayer.GetPlayerAchievements().EndTurn();
			gDLL->sendTurnComplete();
			CvAchievementUnlocker::EndTurn();
			GC.GetEngineUserInterface()->setInterfaceMode(INTERFACEMODE_SELECTION);
		}
		break;

	case CONTROL_FORCEENDTURN:
	{
		EndTurnBlockingTypes eBlock = GET_PLAYER(getActivePlayer()).GetEndTurnBlockingType();
		if(gDLL->allAICivsProcessedThisTurn() && allUnitAIProcessed() && (eBlock == NO_ENDTURN_BLOCKING_TYPE || eBlock == ENDTURN_BLOCKING_UNITS))
		{
			CvPlayerAI& kActivePlayer = GET_PLAYER(getActivePlayer());
			kActivePlayer.GetPlayerAchievements().EndTurn();
			gDLL->sendTurnComplete();
			CvAchievementUnlocker::EndTurn();
			SetForceEndingTurn(true);
			GC.GetEngineUserInterface()->setInterfaceMode(INTERFACEMODE_SELECTION);
		}
		break;
	}

	case CONTROL_AUTOMOVES:
		gDLL->sendAutoMoves();
		break;

	case CONTROL_PING:
		GC.GetEngineUserInterface()->setInterfaceMode(INTERFACEMODE_PING);
		break;

	case CONTROL_YIELDS:
		GC.GetEngineUserInterface()->toggleYieldVisibleMode();
		break;

	case CONTROL_RESOURCE_ALL:
		GC.GetEngineUserInterface()->toggleResourceVisibleMode();
		break;

	case CONTROL_UNIT_ICONS:
#ifdef TURN_TIMER_RESET_BUTTON
    {
        if(isOption(GAMEOPTION_END_TURN_TIMER_ENABLED) && !isPaused() && GC.getGame().getGameState() == GAMESTATE_ON)
        {
            if ((getElapsedGameTurns() > 0) && GET_PLAYER(getActivePlayer()).isTurnActive())
            {


                resetTurnTimer(true);
                gDLL->sendGiftUnit(NO_PLAYER, -1);
            }
        }
    }
#endif
		break;

	case CONTROL_SCORES:
#ifdef TURN_TIMER_PAUSE_BUTTON
	{
		if (isOption(GAMEOPTION_END_TURN_TIMER_ENABLED) && !isPaused() && GC.getGame().getGameState() == GAMESTATE_ON)
		{
			if ((getElapsedGameTurns() > 0) && GET_PLAYER(getActivePlayer()).isTurnActive())
			{



				gDLL->sendGiftUnit(NO_PLAYER, -7);
			}
		}
	}
#endif
		break;

	case CONTROL_LOAD_GAME:
		gDLL->LoadGame();
		break;

	case CONTROL_OPTIONS_SCREEN:
		gDLL->GameplayOpenOptionsScreen();
		break;

	case CONTROL_RETIRE:
		if(!isGameMultiPlayer() || countHumanPlayersAlive() == 1)
		{
			setGameState(GAMESTATE_OVER);
			GC.GetEngineUserInterface()->setDirty(Soundtrack_DIRTY_BIT, true);
		}
		else
		{
			if(isNetworkMultiPlayer())
			{
				GC.GetEngineUserInterface()->exitingToMainMenu();
			}
		}
		break;

	case CONTROL_SAVE_GROUP:
		gDLL->SaveGame(SAVEGAME_GROUP);
		break;

	case CONTROL_SAVE_NORMAL:
		gDLL->SaveGame(SAVEGAME_NORMAL);
		break;

	case CONTROL_QUICK_SAVE:
		if(!(isNetworkMultiPlayer()))
		{
			gDLL->QuickSave();
		}
		break;

	case CONTROL_QUICK_LOAD:
		if(!(isNetworkMultiPlayer()))
		{
			gDLL->QuickLoad();
		}
		break;

	case CONTROL_CIVILOPEDIA:
		gDLL->GameplaySearchForPediaEntry("OPEN_VIA_HOTKEY");
		break;

	case CONTROL_POLICIES_SCREEN:
	{
		CvPopupInfo kPopup(BUTTONPOPUP_CHOOSEPOLICY, getActivePlayer());
		kPopup.iData1 = 1;
		GC.GetEngineUserInterface()->AddPopup(kPopup);
	}
	break;

	case CONTROL_FOREIGN_SCREEN:
	{
		CvPopupInfo kPopup(BUTTONPOPUP_DIPLOMATIC_OVERVIEW, getActivePlayer());
		kPopup.iData1 = 1;
		GC.GetEngineUserInterface()->AddPopup(kPopup);
	}
	break;

	case CONTROL_MILITARY_SCREEN:
	{
		CvPopupInfo kPopup(BUTTONPOPUP_MILITARY_OVERVIEW, getActivePlayer());
		kPopup.iData1 = 1;
		GC.GetEngineUserInterface()->AddPopup(kPopup);
	}
	break;

	case CONTROL_TECH_CHOOSER:
	{
		CvPopupInfo kPopup(BUTTONPOPUP_TECH_TREE, getActivePlayer());


		if(!GC.GetEngineUserInterface()->IsPopupQueueEmpty())
			kPopup.iData1 = 1;

		GC.GetEngineUserInterface()->AddPopup(kPopup);
	}
	break;

	case CONTROL_TURN_LOG:
	{
		CvPopupInfo kPopup(BUTTONPOPUP_NOTIFICATION_LOG, getActivePlayer());
		kPopup.iData1 = 1;
		GC.GetEngineUserInterface()->AddPopup(kPopup);
	}
	break;

	case CONTROL_DOMESTIC_SCREEN:
	{
		CvPopupInfo kPopup(BUTTONPOPUP_ECONOMIC_OVERVIEW, getActivePlayer());
		kPopup.iData1 = 1;
		GC.GetEngineUserInterface()->AddPopup(kPopup);
	}
	break;

	case CONTROL_VICTORY_SCREEN:
	{
		CvPopupInfo kPopup(BUTTONPOPUP_VICTORY_INFO, getActivePlayer());
		kPopup.iData1 = 1;;
		GC.GetEngineUserInterface()->AddPopup(kPopup);
	}
	break;

	case CONTROL_INFO:
	{
		CvPopupInfo kPopup(BUTTONPOPUP_DEMOGRAPHICS, getActivePlayer());
		kPopup.iData1 = 1;
		GC.GetEngineUserInterface()->AddPopup(kPopup);
	}
	break;

	case CONTROL_ADVISOR_COUNSEL:
	{
		CvPopupInfo kPopup(BUTTONPOPUP_ADVISOR_COUNSEL);
		kPopup.iData1 = 1;
		GC.GetEngineUserInterface()->AddPopup(kPopup);
	}
	break;

	case CONTROL_ESPIONAGE_OVERVIEW:
	{
		CvPopupInfo kPopup(BUTTONPOPUP_ESPIONAGE_OVERVIEW, getActivePlayer());
		kPopup.iData1 = 1;
		GC.GetEngineUserInterface()->AddPopup(kPopup);
	}
	break;

	case CONTROL_RELIGION_OVERVIEW:
	{
		CvPopupInfo kPopup(BUTTONPOPUP_RELIGION_OVERVIEW, getActivePlayer());
		kPopup.iData1 = 1;
		GC.GetEngineUserInterface()->AddPopup(kPopup);
	}
	break;

	case CONTROL_RESTART_GAME:
	{
		gDLL->RestartGame();
	}
	break;

	default:
		CvAssertMsg(false, "eControl did not match any valid options");
		break;
	}
}


bool CvGame::IsForceEndingTurn() const
{
	return m_bForceEndingTurn;
}


void CvGame::SetForceEndingTurn(bool bValue)
{
	m_bForceEndingTurn = bValue;
}


int CvGame::getAdjustedPopulationPercent(VictoryTypes eVictory) const
{
	int iPopulation;
	int iBestPopulation;
	int iNextBestPopulation;
	int iI;

	CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eVictory);
	if(pkVictoryInfo == NULL)
	{
		return 0;
	}

	if(pkVictoryInfo->getPopulationPercentLead() == 0)
	{
		return 0;
	}

	if(getTotalPopulation() == 0)
	{
		return 100;
	}

	iBestPopulation = 0;
	iNextBestPopulation = 0;

	for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		if(GET_TEAM((TeamTypes)iI).isAlive())
		{
			iPopulation = GET_TEAM((TeamTypes)iI).getTotalPopulation();

			if(iPopulation > iBestPopulation)
			{
				iNextBestPopulation = iBestPopulation;
				iBestPopulation = iPopulation;
			}
			else if(iPopulation > iNextBestPopulation)
			{
				iNextBestPopulation = iPopulation;
			}
		}
	}

	return std::min(100, (((iNextBestPopulation * 100) / getTotalPopulation()) + pkVictoryInfo->getPopulationPercentLead()));
}



int CvGame::getProductionPerPopulation(HurryTypes eHurry)
{
	if(NO_HURRY == eHurry)
		return 0;

	CvHurryInfo* pkHurryInfo = GC.getHurryInfo(eHurry);
	if(pkHurryInfo == NULL)
		return 0;

	return (pkHurryInfo->getProductionPerPopulation());
}



int CvGame::getAdjustedLandPercent(VictoryTypes eVictory) const
{
	CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eVictory);
	if(pkVictoryInfo == NULL)
		return 0;

	if(pkVictoryInfo->getLandPercent() == 0)
	{
		return 0;
	}

	int iPercent = pkVictoryInfo->getLandPercent();

	iPercent -= (countCivTeamsEverAlive() * 2);

	return std::max(iPercent, pkVictoryInfo->getMinLandPercent());
}


int CvGame::countCivPlayersAlive() const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iCount++;
		}
	}

	return iCount;
}



int CvGame::countCivPlayersEverAlive() const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isEverAlive())
		{
			iCount++;
		}
	}

	return iCount;
}



int CvGame::countCivTeamsAlive() const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		if(GET_TEAM((TeamTypes)iI).isAlive())
		{
			iCount++;
		}
	}

	return iCount;
}



int CvGame::countCivTeamsEverAlive() const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		if(GET_TEAM((TeamTypes)iI).isEverAlive())
		{
			iCount++;
		}
	}

	return iCount;
}



int CvGame::countHumanPlayersAlive() const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if(GET_PLAYER((PlayerTypes)iI).isHuman())
			{
				iCount++;
			}
		}
	}

	return iCount;
}



int CvGame::countHumanPlayersEverAlive() const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isEverAlive())
		{
			if(GET_PLAYER((PlayerTypes)iI).isHuman())
			{
				iCount++;
			}
		}
	}

	return iCount;
}

#ifndef AUI_GAME_PLAYER_BASED_TURN_LENGTH

int CvGame::countSeqHumanTurnsUntilPlayerTurn( PlayerTypes playerID ) const
{
	int humanTurnsUntilMe = 0;
#ifdef AUI_GAME_BETTER_HYBRID_MODE
	const CvPlayer& kTargetPlayer = GET_PLAYER(playerID);
	int iTargetPlayerTurnOrder = kTargetPlayer.getTurnOrder();
	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		const CvPlayer& kCurrentPlayer = GET_PLAYER((PlayerTypes)i);
		if (kCurrentPlayer.isHuman() && kCurrentPlayer.isAlive() &&
			kCurrentPlayer.getTurnOrder() >= m_iCurrentTurnOrderActive && kCurrentPlayer.getTurnOrder() < iTargetPlayerTurnOrder)
		{
			++humanTurnsUntilMe;
		}
	}
#else
	bool startCountingPlayers = false;
	CvPlayer& targetPlayer = GET_PLAYER(playerID);
	if(targetPlayer.isSimultaneousTurns())
	{

		for(int i = 0; i < MAX_PLAYERS; ++i)
		{
			CvPlayer& kCurrentPlayer = GET_PLAYER((PlayerTypes)i);
			if(kCurrentPlayer.isHuman() 
				&& kCurrentPlayer.isAlive() 
				&& !kCurrentPlayer.isSimultaneousTurns())
			{
				if(kCurrentPlayer.isTurnActive())
				{
					startCountingPlayers = true;
					humanTurnsUntilMe++;
				}
				else if(startCountingPlayers)
				{
					humanTurnsUntilMe++;
				}
			}
		}	
	}
	else
	{




		startCountingPlayers = false;
		int curPlayerIdx = (targetPlayer.GetID()+1)%MAX_PLAYERS;
		for(int i = 0; i < MAX_PLAYERS; curPlayerIdx = ++curPlayerIdx%MAX_PLAYERS, ++i)
		{
			CvPlayer& kCurrentPlayer = GET_PLAYER((PlayerTypes)curPlayerIdx);
			if(kCurrentPlayer.GetID() == targetPlayer.GetID())
			{
				break;
			}
			else if(kCurrentPlayer.isHuman() 
				&& kCurrentPlayer.isAlive() 
				&& !kCurrentPlayer.isSimultaneousTurns())
			{
				if(kCurrentPlayer.isTurnActive())
				{
					startCountingPlayers = true;
					humanTurnsUntilMe++;
				}
				else if(startCountingPlayers)
				{
					humanTurnsUntilMe++;
				}
			}
		}	
	}
#endif

	return humanTurnsUntilMe;
}
#endif


int CvGame::countMajorCivsAlive() const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if(!GET_PLAYER((PlayerTypes)iI).isMinorCiv())
			{
				iCount++;
			}
		}
	}

	return iCount;
}


int CvGame::countMajorCivsEverAlive() const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isEverAlive())
		{
			if(!GET_PLAYER((PlayerTypes)iI).isMinorCiv())
			{
				iCount++;
			}
		}
	}

	return iCount;
}


int CvGame::countTotalCivPower()
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iCount += GET_PLAYER((PlayerTypes)iI).getPower();
		}
	}

	return iCount;
}



int CvGame::countTotalNukeUnits()
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iCount += GET_PLAYER((PlayerTypes)iI).getNumNukeUnits();
		}
	}

	return iCount;
}



int CvGame::countKnownTechNumTeams(TechTypes eTech)
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		if(GET_TEAM((TeamTypes)iI).isEverAlive())
		{
			if(GET_TEAM((TeamTypes)iI).GetTeamTechs()->HasTech(eTech))
			{
				iCount++;
			}
		}
	}

	return iCount;
}


int CvGame::goldenAgeLength() const
{
	int iLength;

	iLength =        GC.getGOLDEN_AGE_LENGTH();

	iLength *= getGameSpeedInfo().getGoldenAgePercent();
	iLength /= 100;

	return iLength;
}


int CvGame::victoryDelay(VictoryTypes eVictory) const
{
	CvAssert(eVictory >= 0 && eVictory < GC.getNumVictoryInfos());

	CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eVictory);
	CvAssert(pkVictoryInfo);

	if(pkVictoryInfo == NULL)
		return 0;

	int iLength = pkVictoryInfo->getVictoryDelayTurns();

	iLength *= getGameSpeedInfo().getVictoryDelayPercent();
	iLength /= 100;

	return iLength;
}



int CvGame::getImprovementUpgradeTimeMod(ImprovementTypes eImprovement, const CvPlot* pPlot) const
{
	int iTime = 100;

	CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
	if(pPlot != NULL && NULL != pkImprovementInfo)
	{
		if(pPlot->isRiverSide())
		{
			if(pkImprovementInfo->GetRiverSideUpgradeMod() > 0)
			{
				iTime *= pkImprovementInfo->GetRiverSideUpgradeMod();
				iTime /= 100;
			}
		}

		if(pPlot->isCoastalLand())
		{
			if(pkImprovementInfo->GetCoastalLandUpgradeMod() > 0)
			{
				iTime *= pkImprovementInfo->GetCoastalLandUpgradeMod();
				iTime /= 100;
			}
		}

		if(pPlot->isHills())
		{
			if(pkImprovementInfo->GetHillsUpgradeMod() > 0)
			{
				iTime *= pkImprovementInfo->GetHillsUpgradeMod();
				iTime /= 100;
			}
		}
	}

	return iTime;
}


int CvGame::getImprovementUpgradeTime(ImprovementTypes eImprovement, const CvPlot* pPlot) const
{
	CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
	CvAssert(pkImprovementInfo);

	if(pkImprovementInfo == NULL)
		return 0;

	int iTime = pkImprovementInfo->GetUpgradeTime();

	iTime *= getImprovementUpgradeTimeMod(eImprovement, pPlot);
	iTime /= 100;

	iTime *= getGameSpeedInfo().getImprovementPercent();
	iTime /= 100;

	iTime *= getStartEraInfo().getImprovementPercent();
	iTime /= 100;

	return iTime;
}




bool CvGame::canTrainNukes() const
{
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const PlayerTypes ePlayer = static_cast<PlayerTypes>(iI);
		if(GET_PLAYER(ePlayer).isAlive())
		{
#ifdef AUI_WARNING_FIXES
			for (uint iJ = 0; iJ < GC.getNumUnitInfos(); iJ++)
#else
			for(int iJ = 0; iJ < GC.getNumUnitInfos(); iJ++)
#endif
			{
				const UnitTypes eUnit = static_cast<UnitTypes>(iJ);
				CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
				if(pkUnitInfo)
				{
					if(pkUnitInfo->GetNukeDamageLevel() != -1)
					{
						if(GET_PLAYER(ePlayer).canTrain(eUnit))
						{
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}



EraTypes CvGame::getCurrentEra() const
{
	int iEra;
	int iCount;
	int iI;

	iEra = 0;
	iCount = 0;

	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		if(GET_TEAM((TeamTypes)iI).isAlive())
		{
			iEra += GET_TEAM((TeamTypes)iI).GetCurrentEra();
			iCount++;
		}
	}

	if(iCount > 0)
	{
		return ((EraTypes)(iEra / iCount));
	}

	return NO_ERA;
}



TeamTypes CvGame::getActiveTeam()
{
	PlayerTypes eActivePlayer = getActivePlayer();
	if(eActivePlayer == NO_PLAYER)
	{
		return NO_TEAM;
	}
	else
	{
		return (TeamTypes)GET_PLAYER(eActivePlayer).getTeam();
	}
}



CivilizationTypes CvGame::getActiveCivilizationType()
{
	PlayerTypes eActivePlayer = getActivePlayer();
	if(eActivePlayer == NO_PLAYER)
	{
		return NO_CIVILIZATION;
	}
	else
	{
		return (CivilizationTypes)GET_PLAYER(eActivePlayer).getCivilizationType();
	}
}



bool CvGame::isNetworkMultiPlayer() const
{
	return CvPreGame::isNetworkMultiplayerGame();
}



bool CvGame::isGameMultiPlayer() const
{
	return (isNetworkMultiPlayer() || isPbem() || isHotSeat());
}



bool CvGame::isTeamGame() const
{
	CvAssert(countCivPlayersAlive() >= countCivTeamsAlive());
	return (countCivPlayersAlive() > countCivTeamsAlive());
}



void CvGame::ReviveActivePlayer()
{
	if(!(GET_PLAYER(getActivePlayer()).isAlive()))
	{
		setAIAutoPlay(0, m_eAIAutoPlayReturnPlayer);


		if(m_eAIAutoPlayReturnPlayer == NO_PLAYER)
		{
			CvPreGame::setSlotClaim(getActivePlayer(), SLOTCLAIM_ASSIGNED);
			CvPreGame::setSlotStatus(getActivePlayer(), SS_OBSERVER);
		}


		else
		{

			CvPreGame::setSlotClaim(getActivePlayer(), SLOTCLAIM_UNASSIGNED);
			CvPreGame::setSlotStatus(getActivePlayer(), SS_OBSERVER);


			CvPreGame::setSlotStatus(m_eAIAutoPlayReturnPlayer, SS_TAKEN);
			setActivePlayer(m_eAIAutoPlayReturnPlayer, false                  , true                    );
		}
	}
}



bool CvGame::CanMoveActivePlayerToObserver()
{
	int iObserver = -1;
	PlayerTypes activePlayer = CvPreGame::activePlayer();


	if(CvPreGame::slotStatus(activePlayer) == SS_OBSERVER)
	{
		return false;
	}

	for(int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{

		if(CvPreGame::slotStatus((PlayerTypes)iI) == SS_OBSERVER && (CvPreGame::slotClaim((PlayerTypes)iI) == SLOTCLAIM_UNASSIGNED || CvPreGame::slotClaim((PlayerTypes)iI) == SLOTCLAIM_RESERVED))
		{
			iObserver = iI;
			break;
		}
	}


	return iObserver != -1 && activePlayer != iObserver;
}



void CvGame::ActivateObserverSlot()
{
	for(int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		if(CvPreGame::slotStatus((PlayerTypes)iI) == SS_OBSERVER && (CvPreGame::slotClaim((PlayerTypes)iI) == SLOTCLAIM_UNASSIGNED || CvPreGame::slotClaim((PlayerTypes)iI) == SLOTCLAIM_RESERVED))
		{

			CvPreGame::setSlotStatus(CvPreGame::activePlayer(), SS_COMPUTER);


			CvPreGame::setSlotClaim((PlayerTypes)iI, SLOTCLAIM_ASSIGNED);

			setActivePlayer((PlayerTypes)iI, false                  , true                    );

			break;
		}
	}
}


int CvGame::getNumHumanPlayers()
{
	return CvPreGame::numHumans();
}



int CvGame::GetNumMinorCivsEver()
{
	int iNumCivs = 0;

	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		if(GET_PLAYER((PlayerTypes) iMinorLoop).isEverAlive())
		{
			iNumCivs++;
		}
	}

	return iNumCivs;
}


#ifdef AUI_GAME_BETTER_HYBRID_MODE
int CvGame::getCurrentTurnOrderActive() const
{
	return m_iCurrentTurnOrderActive;
}
#else
int CvGame::getNumHumansInHumanWars(PlayerTypes ignorePlayer)
{
	int humansWarringHumans = 0;
	for(int i = 0; i < MAX_CIV_PLAYERS; ++i)
	{
		const CvPlayer& curPlayer = GET_PLAYER((PlayerTypes)i);
		if(curPlayer.isAlive() 
			&& curPlayer.isHuman() 
			&& (ignorePlayer == NO_PLAYER || curPlayer.GetID() != ignorePlayer)
			&& GET_TEAM(curPlayer.getTeam()).isAtWarWithHumans())
		{
			++humansWarringHumans;
		}
	}
	return humansWarringHumans;
}
#endif


int CvGame::getNumSequentialHumans(PlayerTypes ignorePlayer)
{
	int seqHumans = 0;
#ifdef AUI_GAME_BETTER_HYBRID_MODE
	if (isAnySimultaneousTurns())
	{
		bool bHasHuman;
		for (int iJ = 0; iJ <= m_iLastTurnOrderID; iJ++)
		{
			bHasHuman = false;
			for (int i = 0; i < MAX_CIV_PLAYERS; ++i)
			{
				const CvPlayer& curPlayer = GET_PLAYER((PlayerTypes)i);
				if (curPlayer.isAlive() && curPlayer.isHuman() && (ignorePlayer == NO_PLAYER || curPlayer.GetID() != ignorePlayer)
					&& curPlayer.getTurnOrder() == iJ)
				{
					bHasHuman = true;
					break;
				}
			}
			if (bHasHuman)
				seqHumans++;
		}
	}
	else
		seqHumans = getNumHumanPlayers();
#else
	for(int i = 0; i < MAX_CIV_PLAYERS; ++i)
	{
		const CvPlayer& curPlayer = GET_PLAYER((PlayerTypes)i);
		if(curPlayer.isAlive() 
			&& curPlayer.isHuman() 
			&& !curPlayer.isSimultaneousTurns()
			&& (ignorePlayer == NO_PLAYER || curPlayer.GetID() != ignorePlayer))
		{
			++seqHumans;
		}
	}
#endif
	return seqHumans;
}


#if defined(AUI_WARNING_FIXES) || defined(AUI_CONSTIFY)
int CvGame::getGameTurn() const
#else
int CvGame::getGameTurn()
#endif
{
	return CvPreGame::gameTurn();
}


void CvGame::setGameTurn(int iNewValue)
{
	if(getGameTurn() != iNewValue)
	{
		std::string turnMessage = std::string("Game Turn ") + FSerialization::toString(iNewValue) + std::string("\n");
		gDLL->netMessageDebugLog(turnMessage);

		CvPreGame::setGameTurn(iNewValue);
		CvAssert(getGameTurn() >= 0);

		setScoreDirty(true);

		GC.GetEngineUserInterface()->setDirty(TurnTimer_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		m_sentAutoMoves = false;
		gDLL->GameplayTurnChanged(iNewValue);
		endTurnTimerReset();
	}
}


void CvGame::incrementGameTurn()
{
	setGameTurn(getGameTurn() + 1);
}



int CvGame::getTurnYear(int iGameTurn)
{

	return getTurnYearForGame(iGameTurn, getStartYear(), getCalendar(), getGameSpeedType());
}



int CvGame::getGameTurnYear()
{
	return getTurnYear(getGameTurn());
}



int CvGame::getElapsedGameTurns() const
{
	return m_iElapsedGameTurns;
}



void CvGame::incrementElapsedGameTurns()
{
	m_iElapsedGameTurns++;
}



int CvGame::getMaxTurns() const
{
	return CvPreGame::maxTurns();
}



void CvGame::setMaxTurns(int iNewValue)
{
	CvPreGame::setMaxTurns(iNewValue);
	CvAssert(getMaxTurns() >= 0);
}



void CvGame::changeMaxTurns(int iChange)
{
	setMaxTurns(getMaxTurns() + iChange);
}



int CvGame::getMaxCityElimination() const
{
	return CvPreGame::maxCityElimination();
}



void CvGame::setMaxCityElimination(int iNewValue)
{
	CvPreGame::setMaxCityElimination(iNewValue);
	CvAssert(getMaxCityElimination() >= 0);
}


int CvGame::getNumAdvancedStartPoints() const
{
	return CvPreGame::advancedStartPoints();
}



void CvGame::setNumAdvancedStartPoints(int iNewValue)
{
	CvPreGame::setAdvancedStartPoints(iNewValue);
	CvAssert(getNumAdvancedStartPoints() >= 0);
}


int CvGame::getStartTurn() const
{
	return m_iStartTurn;
}


void CvGame::setStartTurn(int iNewValue)
{
	m_iStartTurn = iNewValue;
}


int CvGame::GetWinningTurn() const
{
	return m_iWinningTurn;
}


void CvGame::SetWinningTurn(int iNewValue)
{
	m_iWinningTurn = iNewValue;
}



int CvGame::getStartYear() const
{
	return m_iStartYear;
}



void CvGame::setStartYear(int iNewValue)
{
	m_iStartYear = iNewValue;
}



int CvGame::getEstimateEndTurn() const
{
	return m_iEstimateEndTurn;
}



void CvGame::setEstimateEndTurn(int iNewValue)
{
	m_iEstimateEndTurn = iNewValue;
}


int CvGame::getDefaultEstimateEndTurn() const
{
	return m_iDefaultEstimateEndTurn;
}



void CvGame::setDefaultEstimateEndTurn(int iNewValue)
{
	m_iDefaultEstimateEndTurn = iNewValue;
}



int CvGame::getTurnSlice() const
{
	return m_iTurnSlice;
}



int CvGame::getMinutesPlayed() const
{
	return (getTurnSlice() / gDLL->getTurnsPerMinute());
}



void CvGame::setTurnSlice(int iNewValue)
{
	m_iTurnSlice = iNewValue;
}



void CvGame::changeTurnSlice(int iChange)
{
	setTurnSlice(getTurnSlice() + iChange);
}


#ifdef AUI_GAME_PLAYER_BASED_TURN_LENGTH
void CvGame::resetTurnTimer(bool                       )
#else
void CvGame::resetTurnTimer(bool resetGameTurnStart)
#endif
{
	m_curTurnTimer.Start();
	m_fCurrentTurnTimerPauseDelta = 0;
#ifndef AUI_GAME_PLAYER_BASED_TURN_LENGTH
	if(resetGameTurnStart)
	{
		m_timeSinceGameTurnStart.Start();
	}
#ifdef TURN_TIMER_PAUSE_BUTTON
	m_bIsPaused = false;
#endif
#endif
}

#ifndef AUI_GAME_PLAYER_BASED_TURN_LENGTH

int CvGame::getMaxTurnLen()
{
#ifdef AUI_GAME_RELATIVE_TURN_TIMERS
	if (getPitbossTurnTime() != 0 && !isOption("GAMEOPTION_RELATIVE_TURN_TIMER"))
#else
	if(getPitbossTurnTime() != 0)
#endif
	{
		if(isPitboss())
		{
			return (getPitbossTurnTime() * 3600);
		}
		else
		{
			return getPitbossTurnTime();
		}
	}
	else
	{
		int iMaxUnits = 0;
		int iMaxCities = 0;



		for(int i = 0; i < MAX_CIV_PLAYERS; ++i)
		{
			if(GET_PLAYER((PlayerTypes)i).isAlive())
			{
#ifdef GAME_UPDATE_TURN_TIMER_ONCE_PER_TURN
				if (GET_PLAYER((PlayerTypes)i).isHuman() && GET_PLAYER((PlayerTypes)i).getNumUnits() > iMaxUnits)
				{
					iMaxUnits = GET_PLAYER((PlayerTypes)i).getNumUnits();
				}
				if (GET_PLAYER((PlayerTypes)i).isHuman() && GET_PLAYER((PlayerTypes)i).getNumCities() > iMaxCities)
				{
					iMaxCities = GET_PLAYER((PlayerTypes)i).getNumCities();
				}
#else
				if(GET_PLAYER((PlayerTypes)i).getNumUnits() > iMaxUnits)
				{
					iMaxUnits = GET_PLAYER((PlayerTypes)i).getNumUnits();
				}
				if(GET_PLAYER((PlayerTypes)i).getNumCities() > iMaxCities)
				{
					iMaxCities = GET_PLAYER((PlayerTypes)i).getNumCities();
				}
#endif
			}
		}


		const CvTurnTimerInfo& kTurnTimer = CvPreGame::turnTimerInfo();
#ifdef AUI_GAME_RELATIVE_TURN_TIMERS
		int baseTurnTime = kTurnTimer.getBaseTime();
		int iExtraTurnTime = (kTurnTimer.getCityResource() * iMaxCities) + (kTurnTimer.getUnitResource() * iMaxUnits);
		if (getPitbossTurnTime() != 0 && isOption("GAMEOPTION_RELATIVE_TURN_TIMER"))
		{
			iExtraTurnTime *= getPitbossTurnTime();
			iExtraTurnTime /= 100;
		}
		baseTurnTime += iExtraTurnTime;
#else
		int baseTurnTime = (kTurnTimer.getBaseTime() +
		        (kTurnTimer.getCityResource() * iMaxCities) +
		        (kTurnTimer.getUnitResource() * iMaxUnits));
#endif

		return baseTurnTime;
	}
}
#endif


bool CvGame::IsStaticTutorialActive() const
{
	return m_bStaticTutorialActive;
}


void CvGame::SetStaticTutorialActive(bool bStaticTutorialActive)
{
	m_bStaticTutorialActive = bStaticTutorialActive;
}


#ifdef AUI_WARNING_FIXES
bool CvGame::HasAdvisorMessageBeenSeen(_In_z_ const char* szAdvisorMessageName)
#else
bool CvGame::HasAdvisorMessageBeenSeen(const char* szAdvisorMessageName)
#endif
{
	std::string strAdvisorMessageName = szAdvisorMessageName;
	std::tr1::unordered_set<std::string>::iterator it = m_AdvisorMessagesViewed.find(strAdvisorMessageName);
	return it != m_AdvisorMessagesViewed.end();
}


#ifdef AUI_WARNING_FIXES
void CvGame::SetAdvisorMessageHasBeenSeen(_In_z_ const char* szAdvisorMessageName, bool bSeen)
#else
void CvGame::SetAdvisorMessageHasBeenSeen(const char* szAdvisorMessageName, bool bSeen)
#endif
{
	std::string strAdvisorMessageName = szAdvisorMessageName;
	if(bSeen)
	{
		m_AdvisorMessagesViewed.insert(strAdvisorMessageName);
	}
	else
	{
		m_AdvisorMessagesViewed.erase(strAdvisorMessageName);
	}
}


bool CvGame::IsCityScreenBlocked()
{
	return CvPreGame::IsCityScreenBlocked();
}


bool CvGame::CanOpenCityScreen(PlayerTypes eOpener, CvCity* pCity)
{
	if(eOpener == pCity->getOwner())
	{
		return true;
	}
#ifdef AUI_GAME_OBSERVER_CAN_OPEN_CITIES
	else if (GET_PLAYER(eOpener).isObserver())
	{
		return true;
	}
#endif
	else if (!GET_PLAYER(pCity->getOwner()).isMinorCiv() && (GET_PLAYER(eOpener).GetEspionage()->HasEstablishedSurveillanceInCity(pCity) || GET_PLAYER(eOpener).GetEspionage()->IsAnySchmoozing(pCity)))
	{
		return true;
	}

	return false;
}


int CvGame::getTargetScore() const
{
	return CvPreGame::targetScore();
}



void CvGame::setTargetScore(int iNewValue)
{
	CvPreGame::setTargetScore(iNewValue);
	CvAssert(getTargetScore() >= 0);
}



#ifdef AUI_CONSTIFY
int CvGame::getNumGameTurnActive() const
#else
int CvGame::getNumGameTurnActive()
#endif
{
	int numActive = 0;
	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		if(GET_PLAYER((PlayerTypes)i).isAlive() && GET_PLAYER((PlayerTypes)i).isTurnActive())
		{
			++numActive;
		}
	}
	return numActive;
}



#ifdef AUI_CONSTIFY
int CvGame::countNumHumanGameTurnActive() const
#else
int CvGame::countNumHumanGameTurnActive()
#endif
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isHuman())
		{
			if(GET_PLAYER((PlayerTypes)iI).isTurnActive())
			{
				iCount++;
			}
		}
	}

	return iCount;
}

#ifdef AUI_GAME_BETTER_HYBRID_MODE
bool CvGame::isNoPlayerActive() const
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (GET_PLAYER((PlayerTypes)i).isAlive() && GET_PLAYER((PlayerTypes)i).isTurnActive())
		{
			return false;
		}
	}
	return true;
}
#endif


void CvGame::changeNumGameTurnActive(int iChange, const std::string& why)
{
	char changeBuf[8] = {0};
	char activeBuf[8] = {0};
	sprintf_s(changeBuf, "%d", iChange);
	sprintf_s(activeBuf, "%d", getNumGameTurnActive());
	std::string output = "changeNumGameTurnActive(";
	output += changeBuf;
	output += ") m_iNumActive=";
	output += activeBuf;
	output += " : " + why;
	gDLL->netMessageDebugLog(output);
	CvAssert(getNumGameTurnActive() >= 0);
}



int CvGame::getNumCities() const
{
	return m_iNumCities;
}



int CvGame::getNumCivCities() const
{
	return (getNumCities() - GET_PLAYER(BARBARIAN_PLAYER).getNumCities());
}



void CvGame::changeNumCities(int iChange)
{
	m_iNumCities = (m_iNumCities + iChange);
	CvAssert(getNumCities() >= 0);
}



int CvGame::getTotalPopulation() const
{
	return m_iTotalPopulation;
}



void CvGame::changeTotalPopulation(int iChange)
{
	m_iTotalPopulation = (m_iTotalPopulation + iChange);
	CvAssert(getTotalPopulation() >= 0);
}


int CvGame::getNoNukesCount() const
{
	return m_iNoNukesCount;
}



bool CvGame::isNoNukes() const
{
	return (getNoNukesCount() > 0);
}



void CvGame::changeNoNukesCount(int iChange)
{
	m_iNoNukesCount = (m_iNoNukesCount + iChange);
	CvAssert(getNoNukesCount() >= 0);
}


int CvGame::getNukesExploded() const
{
	return m_iNukesExploded;
}



void CvGame::changeNukesExploded(int iChange)
{
	m_iNukesExploded = (m_iNukesExploded + iChange);
}



int CvGame::getMaxPopulation() const
{
	return m_iMaxPopulation;
}



int CvGame::getInitPopulation() const
{
	return m_iInitPopulation;
}



int CvGame::getInitLand() const
{
	return m_iInitLand;
}



int CvGame::getInitTech() const
{
	return m_iInitTech;
}



int CvGame::getInitWonders() const
{
	return m_iInitWonders;
}



void CvGame::initScoreCalculation()
{

	int iMaxFood = 0;
#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < GC.getMap().numPlots(); i++)
#else
	for(int i = 0; i < GC.getMap().numPlots(); i++)
#endif
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(i);
		if(!pPlot->isWater() || pPlot->isAdjacentToLand())
		{
			iMaxFood += pPlot->calculateBestNatureYield(YIELD_FOOD, NO_TEAM);
		}
	}
	m_iMaxPopulation = getPopulationScore(iMaxFood / std::max(1, GC.getFOOD_CONSUMPTION_PER_POPULATION()));
	if(NO_ERA != getStartEra())
	{
		CvEraInfo& kStartEra = getStartEraInfo();
		int iNumSettlers = kStartEra.getStartingUnitMultiplier();
		m_iInitPopulation = getPopulationScore(iNumSettlers * (kStartEra.getFreePopulation() + 1));
		m_iInitLand = getLandPlotsScore(iNumSettlers *  NUM_CITY_PLOTS);
	}
	else
	{
		m_iInitPopulation = 0;
		m_iInitLand = 0;
	}

	m_iInitTech = 0;
#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < GC.getNumTechInfos(); i++)
#else
	for(int i = 0; i < GC.getNumTechInfos(); i++)
#endif
	{
		const TechTypes eTech = static_cast<TechTypes>(i);
		CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
		if(pkTechInfo)
		{
			if(pkTechInfo->GetEra() < getStartEra())
			{
				m_iInitTech += getTechScore(eTech);
			}
			else
			{

#ifdef AUI_WARNING_FIXES
				for (uint iCiv = 0; iCiv < GC.getNumCivilizationInfos(); iCiv++)
#else
				for(int iCiv = 0; iCiv < GC.getNumCivilizationInfos(); iCiv++)
#endif
				{
					const CivilizationTypes eCivilization = static_cast<CivilizationTypes>(iCiv);
					CvCivilizationInfo* pkCivilizationInfo = GC.getCivilizationInfo(eCivilization);
					if(pkCivilizationInfo)
					{
						if(pkCivilizationInfo->isPlayable())
						{
							if(pkCivilizationInfo->isCivilizationFreeTechs(i))
							{
								m_iInitTech += getTechScore(eTech);
								break;
							}
						}
					}
				}
			}
		}
	}
	m_iInitWonders = 0;
}



int CvGame::getAIAutoPlay()
{
	return m_iAIAutoPlay;
}



void CvGame::setAIAutoPlay(int iNewValue, PlayerTypes eReturnAsPlayer)
{
	int iOldValue;

	iOldValue = getAIAutoPlay();

	if(iOldValue != iNewValue)
	{
		m_iAIAutoPlay = std::max(0, iNewValue);
		m_eAIAutoPlayReturnPlayer = eReturnAsPlayer;

		if((iOldValue == 0) && (getAIAutoPlay() > 0))
		{
			if(CanMoveActivePlayerToObserver())
			{
				ActivateObserverSlot();
			}

			else
			{
				GET_PLAYER(getActivePlayer()).killUnits();
				GET_PLAYER(getActivePlayer()).killCities();
				CvPreGame::setSlotStatus(getActivePlayer(), SS_OBSERVER);
			}
		}
	}
}



void CvGame::changeAIAutoPlay(int iChange)
{
	setAIAutoPlay(getAIAutoPlay() + iChange, m_eAIAutoPlayReturnPlayer);
}



unsigned int CvGame::getInitialTime()
{
	return m_uiInitialTime;
}



void CvGame::setInitialTime(unsigned int uiNewValue)
{
	m_uiInitialTime = uiNewValue;
}


#ifdef GAME_UPDATE_TURN_TIMER_ONCE_PER_TURN

float CvGame::getPreviousTurnLen()
{
	return m_fPreviousTurnLen;
}



void CvGame::setPreviousTurnLen(float fNewValue)
{
	m_fPreviousTurnLen = fNewValue;
}


#endif
#ifdef TURN_TIMER_PAUSE_BUTTON

float CvGame::getTimeElapsed()
{
	return m_fTimeElapsed;
}



void CvGame::setTimeElapsed(float fNewValue)
{
	m_fTimeElapsed = fNewValue;
}
#endif
#ifdef GAME_ALLOW_ONLY_ONE_UNIT_MOVE_ON_TURN_LOADING

bool CvGame::isMPOrderedMoveOnTurnLoading() const
{
	return m_bMPOrderedMoveOnTurnLoading;
}



void CvGame::setMPOrderedMoveOnTurnLoading(bool bNewValue)
{
	m_bMPOrderedMoveOnTurnLoading = bNewValue;
}



bool CvGame::getHasReceivedFirstMission()
{
	return m_bReceivedFirstMission;
}



void CvGame::setHasReceivedFirstMission(bool bNewValue)
{
	m_bReceivedFirstMission = bNewValue;
}


#endif

bool CvGame::isScoreDirty() const
{
	return m_bScoreDirty;
}



void CvGame::setScoreDirty(bool bNewValue)
{
	m_bScoreDirty = bNewValue;
}



bool CvGame::isCircumnavigated() const
{
	return m_bCircumnavigated;
}



void CvGame::makeCircumnavigated()
{
	m_bCircumnavigated = true;
}


bool CvGame::circumnavigationAvailable() const
{
	if(isCircumnavigated())
	{
		return false;
	}

	CvMap& kMap = GC.getMap();

	if(!(kMap.isWrapX()) && !(kMap.isWrapY()))
	{
		return false;
	}

	if(kMap.getLandPlots() > ((kMap.numPlots() * 2) / 3))
	{
		return false;
	}

	return true;
}



void CvGame::DoFromUIDiploEvent(FromUIDiploEventTypes eEvent, PlayerTypes eAIPlayer, int iArg1, int iArg2)
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(eEvent);
		args->Push(eAIPlayer);
		args->Push(iArg1);
		args->Push(iArg2);

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "UiDiploEvent", args.get(), bResult);
	}

	gDLL->sendFromUIDiploEvent(eAIPlayer, eEvent, iArg1, iArg2);
}




void CvGame::DoInitDiploVictory()
{
	DoUpdateDiploVictory();
}



void CvGame::DoUpdateDiploVictory()
{
	int iVotesForHost = 1;
	int iVotesPerCiv = 1;
	int iVotesPerCityState = 1;
#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < GC.getNumLeagueSpecialSessionInfos(); i++)
#else
	for (int i = 0; i < GC.getNumLeagueSpecialSessionInfos(); i++)
#endif
	{
		LeagueSpecialSessionTypes e = (LeagueSpecialSessionTypes)i;
		CvLeagueSpecialSessionEntry* pInfo = GC.getLeagueSpecialSessionInfo(e);
		CvAssert(pInfo != NULL);
		if (pInfo != NULL)
		{
			if (pInfo->IsUnitedNations())
			{
				iVotesForHost = pInfo->GetHostDelegates();
				iVotesPerCiv = pInfo->GetCivDelegates();
				iVotesPerCityState = pInfo->GetCityStateDelegates();
			}
		}
	}

	float fCivsToCount = 0.0f;
	float fCityStatesToCount = 0.0f;
	for (uint i = 0; i < MAX_CIV_PLAYERS; i++)
	{
		PlayerTypes e = (PlayerTypes)i;
		CvPlayer* pPlayer = &GET_PLAYER(e);
		if (pPlayer != NULL && pPlayer->isEverAlive())
		{

			if (pPlayer->isMinorCiv())
			{

				if (!pPlayer->GetMinorCivAI()->IsBoughtOut())
				{
					if (pPlayer->isAlive())
					{
						fCityStatesToCount += 1.0f;
					}
					else
					{
						fCityStatesToCount += 0.5f;
					}
				}
			}

			else
			{
				if (pPlayer->isAlive())
				{
					fCivsToCount += 1.0f;
				}
				else
				{
					fCivsToCount += 0.5f;
				}
			}
		}
	}




	float fCivVotesPortion = (GC.getDIPLO_VICTORY_CIV_DELEGATES_COEFFICIENT() * (float)log(fCivsToCount)) + GC.getDIPLO_VICTORY_CIV_DELEGATES_CONSTANT();
	if (fCivVotesPortion < 0.0f)
	{
		fCivVotesPortion = 0.0f;
	}
	float fCityStateVotesPortion = (GC.getDIPLO_VICTORY_CS_DELEGATES_COEFFICIENT() * (float)log(fCityStatesToCount)) + GC.getDIPLO_VICTORY_CS_DELEGATES_CONSTANT();
	if (fCityStateVotesPortion < 0.0f)
	{
		fCityStateVotesPortion = 0.0f;
	}

	int iVotesToWin = (int)floor(fCivVotesPortion + fCityStateVotesPortion);
	iVotesToWin = MAX(iVotesForHost + iVotesPerCiv + 1, iVotesToWin);
	iVotesToWin = MIN(iVotesForHost + (iVotesPerCiv * (int)fCivsToCount) + (iVotesPerCityState * (int)fCityStatesToCount), iVotesToWin);

	SetVotesNeededForDiploVictory(iVotesToWin);
	GC.GetEngineUserInterface()->setDirty(LeagueScreen_DIRTY_BIT, true);
}



int CvGame::GetVotesNeededForDiploVictory() const
{
	return m_iVotesNeededForDiploVictory;
}



void CvGame::SetVotesNeededForDiploVictory(int iValue)
{
	if(iValue != GetVotesNeededForDiploVictory())
	{
		m_iVotesNeededForDiploVictory = iValue;
	}
}



bool CvGame::IsUnitedNationsActive()
{
	if (!GC.getGame().isOption(GAMEOPTION_NO_LEAGUES))
	{
#ifndef AUI_LEAGUES_FIX_POSSIBLE_DEALLOCATION_CRASH
		if (GetGameLeagues()->GetNumActiveLeagues() > 0)
#endif
		{
			CvLeague* pLeague = GetGameLeagues()->GetActiveLeague();
			if (pLeague != NULL)
			{
				if (pLeague->IsUnitedNations())
				{
					return true;
				}
			}
		}
	}

	return false;
}



int CvGame::GetUnitedNationsCountdown() const
{
	return m_iUnitedNationsCountdown;
}



void CvGame::SetUnitedNationsCountdown(int iValue)
{
	if(iValue != GetUnitedNationsCountdown())
	{
		m_iUnitedNationsCountdown = iValue;
	}
}



void CvGame::ChangeUnitedNationsCountdown(int iChange)
{
	if(iChange != 0)
	{
		SetUnitedNationsCountdown(GetUnitedNationsCountdown() + iChange);
	}
}



int CvGame::GetNumVictoryVotesTallied() const
{
	return m_iNumVictoryVotesTallied;
}




void CvGame::SetNumVictoryVotesTallied(int iValue, bool                     )
{
	m_iNumVictoryVotesTallied = iValue;


	if(iValue > 0)
	{
		if(iValue == GetNumVictoryVotesExpected())
		{
		}
	}
}




void CvGame::ChangeNumVictoryVotesTallied(int iChange, bool bPreliminaryVote)
{
	SetNumVictoryVotesTallied(GetNumVictoryVotesTallied() + iChange, bPreliminaryVote);
}



int CvGame::GetNumVictoryVotesExpected() const
{
	return m_iNumVictoryVotesExpected;
}



void CvGame::SetNumVictoryVotesExpected(int iValue)
{
	m_iNumVictoryVotesExpected = iValue;
}


TeamTypes CvGame::GetVoteCast(TeamTypes eVotingTeam) const
{
	CvAssertMsg(eVotingTeam >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eVotingTeam < MAX_CIV_TEAMS, "eMajor is expected to be within maximum bounds (invalid Index)");

	return (TeamTypes) m_aiVotesCast[eVotingTeam];
}




void CvGame::SetVoteCast(TeamTypes eVotingTeam, TeamTypes eVote, bool bPreliminaryVote)
{
	CvAssertMsg(eVotingTeam >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eVotingTeam < MAX_CIV_TEAMS, "eMajor is expected to be within maximum bounds (invalid Index)");

	if(eVote != GetVoteCast(eVotingTeam))
	{
		m_aiVotesCast[eVotingTeam] = eVote;


		if(eVote != NO_TEAM)
		{
			int iNumVotes = 1;

			ChangeNumVotesForTeam(eVote, iNumVotes);


			ChangeNumVictoryVotesTallied(1, bPreliminaryVote);
		}
	}
}



TeamTypes CvGame::GetPreviousVoteCast(TeamTypes eVotingTeam) const
{
	CvAssertMsg(eVotingTeam >= 0, "eVotingTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eVotingTeam < MAX_CIV_TEAMS, "eVotingTeam is expected to be within maximum bounds (invalid Index)");
	if (eVotingTeam < 0 || eVotingTeam >= MAX_CIV_TEAMS) return NO_TEAM;

	return (TeamTypes) m_aiPreviousVotesCast[eVotingTeam];
}



void CvGame::SetPreviousVoteCast(TeamTypes eVotingTeam, TeamTypes eVotingTarget)
{
	CvAssertMsg(eVotingTeam >= 0, "eVotingTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eVotingTeam < MAX_CIV_TEAMS, "eVotingTeam is expected to be within maximum bounds (invalid Index)");
	if (eVotingTeam < 0 || eVotingTeam >= MAX_CIV_TEAMS) return;

	if (eVotingTarget != GetPreviousVoteCast(eVotingTeam))
	{
		m_aiPreviousVotesCast[eVotingTeam] = eVotingTarget;
	}
}


int CvGame::GetNumVotesForTeam(TeamTypes eTeam) const
{
	CvAssertMsg(eTeam >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_CIV_TEAMS, "eMajor is expected to be within maximum bounds (invalid Index)");

	return m_aiNumVotesForTeam[eTeam];
}


void CvGame::SetNumVotesForTeam(TeamTypes eTeam, int iValue)
{
	CvAssertMsg(eTeam >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_CIV_TEAMS, "eMajor is expected to be within maximum bounds (invalid Index)");

	if(iValue != GetNumVotesForTeam(eTeam))
	{
		m_aiNumVotesForTeam[eTeam] = iValue;
	}
}


void CvGame::ChangeNumVotesForTeam(TeamTypes eTeam, int iChange)
{
	SetNumVotesForTeam(eTeam, GetNumVotesForTeam(eTeam) + iChange);
}



Localization::String CvGame::GetDiploResponse(const char* szLeader, const char* szResponse, const Localization::String& strOptionalKey1, const Localization::String& strOptionalKey2)
{


	Localization::String response;

	if(m_pDiploResponseQuery == NULL)
	{


		const char* szSQL = "select Tag, Bias from Diplomacy_Responses, Language_en_US where (LeaderType = ? or LeaderType = 'GENERIC') and ResponseType = ? and Tag like Response";
		m_pDiploResponseQuery = new Database::Results();
		if(!GC.GetGameDatabase()->Execute(*m_pDiploResponseQuery, szSQL, strlen(szSQL)))
		{
			CvAssertMsg(false, "Failed to generate diplo response query.");
		}
	}











	std::vector<string> probabilities;
	probabilities.reserve(512);

	m_pDiploResponseQuery->Bind(1, szLeader);
	m_pDiploResponseQuery->Bind(2, szResponse);

	while(m_pDiploResponseQuery->Step())
	{
		const char* szTag = m_pDiploResponseQuery->GetText(0);
		int bias = m_pDiploResponseQuery->GetInt(1);
		for(int i = 0; i < bias; i++)
		{
			probabilities.push_back(szTag);
		}
	}

	m_pDiploResponseQuery->Reset();

	if(!probabilities.empty())
	{
		response = Localization::Lookup(probabilities[getAsyncRandNum(probabilities.size(), "Diplomacy Rand")].c_str());
		response << strOptionalKey1 << strOptionalKey2;
	}

	if(response.IsEmpty())
	{
		char szMessage[256];
		sprintf_s(szMessage, "Please send Jon this with your last 5 autosaves and what changelist # you're playing. Could not find diplomacy response. Leader - %s, Response - %s", szLeader, szResponse);
		CvAssertMsg(false, szMessage);
	}

	return response;
}



bool CvGame::isDebugMode() const
{
	return m_bDebugModeCache;
}


void CvGame::setFOW(bool bMode)
{
	m_bFOW = bMode;
}


bool CvGame::getFOW()
{
	return m_bFOW;
}


void CvGame::setDebugMode(bool bDebugMode)
{
	if(m_bDebugMode != bDebugMode)
		toggleDebugMode();
}


void CvGame::toggleDebugMode()
{
#ifdef AUI_WARNING_FIXES
	m_bDebugMode = !m_bDebugMode;
#else
	m_bDebugMode = ((m_bDebugMode) ? false : true);
#endif
	updateDebugModeCache();

	GC.getMap().updateVisibility();

#ifdef AUI_WARNING_FIXES
	ICvUserInterface2& kEngineUserInterface = *GC.GetEngineUserInterface();
	kEngineUserInterface.setDirty(GameData_DIRTY_BIT, true);
	kEngineUserInterface.setDirty(Score_DIRTY_BIT, true);
	kEngineUserInterface.setDirty(MinimapSection_DIRTY_BIT, true);
	kEngineUserInterface.setDirty(UnitInfo_DIRTY_BIT, true);
	kEngineUserInterface.setDirty(CityInfo_DIRTY_BIT, true);
#else
	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	GC.GetEngineUserInterface()->setDirty(Score_DIRTY_BIT, true);
	GC.GetEngineUserInterface()->setDirty(MinimapSection_DIRTY_BIT, true);
	GC.GetEngineUserInterface()->setDirty(UnitInfo_DIRTY_BIT, true);
	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
#endif
}


void CvGame::updateDebugModeCache()
{
	m_bDebugModeCache = m_bDebugMode;
}


int CvGame::getPitbossTurnTime() const
{
	return CvPreGame::pitBossTurnTime();
}


void CvGame::setPitbossTurnTime(int iHours)
{
	CvPreGame::setPitBossTurnTime(iHours);
}



bool CvGame::isHotSeat() const
{
	return CvPreGame::isHotSeat();
}


bool CvGame::isPbem() const
{
	return CvPreGame::isPlayByEmail();
}


bool CvGame::isPitboss() const
{
	return CvPreGame::isPitBoss();
}


#ifdef AUI_GAME_BETTER_HYBRID_MODE
bool CvGame::isAnySimultaneousTurns() const
{
	return isNetworkMultiPlayer() && (isOption(GAMEOPTION_DYNAMIC_TURNS) || isOption(GAMEOPTION_SIMULTANEOUS_TURNS));
}

bool CvGame::isAllActivePlayersTurnAllComplete() const
{
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kCurPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kCurPlayer.isHuman() && kCurPlayer.isAlive() && (kCurPlayer.getTurnOrder() == m_iCurrentTurnOrderActive || !isAnySimultaneousTurns()))
		{
			if (!gDLL->HasReceivedTurnAllComplete((PlayerTypes)iI))
			{
				return false;
			}
		}
	}

	return true;
}
#else
bool CvGame::isSimultaneousTeamTurns() const
{


	if(!isNetworkMultiPlayer())
	{
		return false;
	}

	if(!isOption(GAMEOPTION_DYNAMIC_TURNS) && isOption(GAMEOPTION_SIMULTANEOUS_TURNS))
	{
		return false;
	}

	return true;
}
#endif


bool CvGame::isFinalInitialized() const
{
	return m_bFinalInitialized;
}



void CvGame::setFinalInitialized(bool bNewValue)
{
	if(isFinalInitialized() != bNewValue)
	{
		m_bFinalInitialized = bNewValue;
	}
}



bool CvGame::getPbemTurnSent() const
{
	return m_bPbemTurnSent;
}



void CvGame::setPbemTurnSent(bool bNewValue)
{
	m_bPbemTurnSent = bNewValue;
}



bool CvGame::getHotPbemBetweenTurns() const
{
	return m_bHotPbemBetweenTurns;
}



void CvGame::setHotPbemBetweenTurns(bool bNewValue)
{
	m_bHotPbemBetweenTurns = bNewValue;
}



bool CvGame::isPlayerOptionsSent() const
{
	return m_bPlayerOptionsSent;
}



void CvGame::sendPlayerOptions(bool bForce)
{
	if(getActivePlayer() == NO_PLAYER)
	{
		return;
	}

	if(!isPlayerOptionsSent() || bForce)
	{
		m_bPlayerOptionsSent = true;

		gDLL->BeginSendBundle();
		for(int iI = 0; iI < GC.getNumPlayerOptionInfos(); iI++)
		{
			const PlayerOptionTypes eOption = static_cast<PlayerOptionTypes>(iI);

			CvPlayerOptionInfo* pkInfo = GC.getPlayerOptionInfo(eOption);
			if (pkInfo)
			{
				uint uiID = FString::Hash( pkInfo->GetType() );
				gDLL->sendPlayerOption(static_cast<PlayerOptionTypes>(uiID), gDLL->getPlayerOption(static_cast<PlayerOptionTypes>(uiID)));
			}
		}
		gDLL->EndSendBundle();
	}
}



PlayerTypes CvGame::getActivePlayer() const
{
	return CvPreGame::activePlayer();
}



void CvGame::setActivePlayer(PlayerTypes eNewValue, bool bForceHotSeat, bool bAutoplaySwitch)
{
	PlayerTypes eOldActivePlayer = getActivePlayer();
	if(eOldActivePlayer != eNewValue)
	{
		int iActiveNetId = ((NO_PLAYER != eOldActivePlayer) ? GET_PLAYER(eOldActivePlayer).getNetID() : -1);
		CvPreGame::setActivePlayer(eNewValue);
		gDLL->GameplayActivePlayerChanged(eNewValue);

		if(GET_PLAYER(eNewValue).isHuman() && (isHotSeat() || isPbem() || bForceHotSeat))
		{
			if(isHotSeat())
			{
				GC.GetEngineUserInterface()->setDirty(TurnTimer_DIRTY_BIT, true);
				GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
				m_sentAutoMoves = false;
				resetTurnTimer();
				endTurnTimerReset();
			}

			gDLL->getPassword(eNewValue);
			setHotPbemBetweenTurns(false);

			if(NO_PLAYER != eOldActivePlayer)
			{
				int iInactiveNetId = GET_PLAYER(eNewValue).getNetID();
				GET_PLAYER(eNewValue).setNetID(iActiveNetId);
				GET_PLAYER(eOldActivePlayer).setNetID(iInactiveNetId);
			}

			if(countHumanPlayersAlive() == 1 && isPbem())
			{

				CvPreGame::setGameType(GAME_HOTSEAT_MULTIPLAYER);
			}
		}

		if(isHotSeat() || bForceHotSeat || bAutoplaySwitch)
		{
			sendPlayerOptions(true);
		}

		if(GC.IsGraphicsInitialized())
		{



			gDLL->PublishActivePlayer(eNewValue, eOldActivePlayer);

			CvMap& theMap = GC.getMap();
			theMap.updateFog();
			theMap.updateVisibility();

			ICvUserInterface2* theUI = GC.GetEngineUserInterface();
			theUI->setCanEndTurn(false);

			theUI->clearSelectedCities();
			theUI->ClearSelectionList();

			theUI->setDirty(PercentButtons_DIRTY_BIT, true);
			theUI->setDirty(ResearchButtons_DIRTY_BIT, true);
			theUI->setDirty(GameData_DIRTY_BIT, true);
			theUI->setDirty(MinimapSection_DIRTY_BIT, true);
			theUI->setDirty(CityInfo_DIRTY_BIT, true);
			theUI->setDirty(UnitInfo_DIRTY_BIT, true);


			theUI->setDirty(BlockadedPlots_DIRTY_BIT, true);
		}
	}
}


CvHandicapInfo& CvGame::getHandicapInfo() const
{
	CvHandicapInfo* pkHandicapInfo = GC.getHandicapInfo(getHandicapType());
	if(pkHandicapInfo == NULL)
	{
		const char* szError = "ERROR: Game does not contain valid handicap!!";
		GC.LogMessage(szError);
		CvAssertMsg(false, szError);
	}

#pragma warning ( push )
#pragma warning ( disable : 6011 )
	return *pkHandicapInfo;
#pragma warning ( pop )
}

HandicapTypes CvGame::getHandicapType() const
{
	return m_eHandicap;
}

void CvGame::setHandicapType(HandicapTypes eHandicap)
{
	m_eHandicap = eHandicap;
}


PlayerTypes CvGame::getPausePlayer()
{
	return m_ePausePlayer;
}


bool CvGame::isPaused()
{
	return (getPausePlayer() != NO_PLAYER);
}


void CvGame::setPausePlayer(PlayerTypes eNewValue)
{
#ifndef AUI_GAME_SET_PAUSED_TURN_TIMERS_PAUSE_ON_RECONNECT
	if(!isNetworkMultiPlayer())
#endif
	{

		if(isOption(GAMEOPTION_END_TURN_TIMER_ENABLED))
		{
			if(eNewValue != NO_PLAYER && m_ePausePlayer == NO_PLAYER)
			{
				m_fCurrentTurnTimerPauseDelta += m_curTurnTimer.Stop();
#ifndef AUI_GAME_PLAYER_BASED_TURN_LENGTH
				m_timeSinceGameTurnStart.Stop();
#endif
			}
			else if(eNewValue == NO_PLAYER && m_ePausePlayer != NO_PLAYER)
			{
#ifndef AUI_GAME_PLAYER_BASED_TURN_LENGTH
				m_timeSinceGameTurnStart.Start();
#endif
				m_curTurnTimer.Start();
			}
		}
	}

	m_ePausePlayer = eNewValue;
}


UnitTypes CvGame::getBestLandUnit()
{
	return m_eBestLandUnit;
}


int CvGame::getBestLandUnitCombat()
{
	const UnitTypes eBestLandUnit = getBestLandUnit();
	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eBestLandUnit);
	if(pkUnitInfo)
	{
		return std::max(1, pkUnitInfo->GetCombat());
	}

	return 1;
}



void CvGame::setBestLandUnit(UnitTypes eNewValue)
{
	if(getBestLandUnit() != eNewValue)
	{
		m_eBestLandUnit = eNewValue;

		GC.GetEngineUserInterface()->setDirty(UnitInfo_DIRTY_BIT, true);
	}
}



int CvGame::GetFaithCost(CvUnitEntry *pkUnit) const
{
	int iRtnValue = pkUnit->GetFaithCost();
	iRtnValue *= getGameSpeedInfo().getTrainPercent();
	iRtnValue /= 1000;
	iRtnValue *= 10;

	return iRtnValue;
}


TeamTypes CvGame::getWinner() const
{
	return m_eWinner;
}



VictoryTypes CvGame::getVictory() const
{
	return m_eVictory;
}



void CvGame::setWinner(TeamTypes eNewWinner, VictoryTypes eNewVictory)
{
	if((getWinner() != eNewWinner) || (getVictory() != eNewVictory))
	{
		m_eWinner = eNewWinner;
		m_eVictory = eNewVictory;
		SetWinningTurn(getElapsedGameTurns());


		SetUnitedNationsCountdown(0);

		if(getVictory() != NO_VICTORY && !IsStaticTutorialActive())
		{
			CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(getVictory());
			CvAssert(pkVictoryInfo);
			if(pkVictoryInfo == NULL)
				return;

			const char* szVictoryTextKey = pkVictoryInfo->GetTextKey();

			if(getWinner() != NO_TEAM)
			{
				const PlayerTypes winningTeamLeaderID = GET_TEAM(getWinner()).getLeaderID();
				CvPlayerAI& kWinningTeamLeader = GET_PLAYER(winningTeamLeaderID);
				const char* szWinningTeamLeaderNameKey = kWinningTeamLeader.getNameKey();

				Localization::String localizedText = Localization::Lookup("TXT_KEY_GAME_WON");
				localizedText << GET_TEAM(getWinner()).getName().GetCString() << szVictoryTextKey;
#ifdef MP_PLAYERS_VOTING_SYSTEM
				if (strcmp(pkVictoryInfo->GetType(), "VICTORY_SCRAP") == 0)
				{
					localizedText = Localization::Lookup(pkVictoryInfo->GetDescriptionKey());
				}
#endif
				addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, winningTeamLeaderID, localizedText.toUTF8(), -1, -1);


				localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_VICTORY_WINNER");
				localizedText << szWinningTeamLeaderNameKey << szVictoryTextKey;

				Localization::String localizedSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_VICTORY_WINNER");
				localizedSummary << szWinningTeamLeaderNameKey;
#ifdef MP_PLAYERS_VOTING_SYSTEM
				if (strcmp(pkVictoryInfo->GetType(), "VICTORY_SCRAP") == 0)
				{
					localizedText = Localization::Lookup(pkVictoryInfo->GetDescriptionKey());
					localizedSummary = Localization::Lookup(pkVictoryInfo->GetTextKey());
				}
#endif

				for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
					PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
					CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
					CvNotifications* pNotifications = kCurNotifyPlayer.GetNotifications();
					if(pNotifications){
						pNotifications->Add(NOTIFICATION_VICTORY, localizedText.toUTF8(), localizedSummary.toUTF8(), -1, -1, -1);
					}
				}



				if(!GC.getGame().isGameMultiPlayer() && kWinningTeamLeader.isHuman() && kWinningTeamLeader.isLocalPlayer())
				{
					const bool bUsingDLC1Scenario = gDLL->IsModActivated(CIV5_DLC_01_SCENARIO_MODID);
					const bool bUsingDLC2Scenario = gDLL->IsModActivated(CIV5_DLC_02_SCENARIO_MODID) || gDLL->IsModActivated(CIV5_COMPLETE_SCENARIO1_MODID);
					const bool bUsingDLC3Scenario = gDLL->IsModActivated(CIV5_DLC_03_SCENARIO_MODID);
					const bool bUsingDLC4Scenario = gDLL->IsModActivated(CIV5_DLC_04_SCENARIO_MODID);
					const bool bUsingDLC5Scenario = gDLL->IsModActivated(CIV5_DLC_05_SCENARIO_MODID);
					const bool bUsingDLC6Scenario = gDLL->IsModActivated(CIV5_DLC_06_SCENARIO_MODID);

					const bool bUsingXP1Scenario1 = gDLL->IsModActivated(CIV5_XP1_SCENARIO1_MODID);
					const bool bUsingXP1Scenario2 = gDLL->IsModActivated(CIV5_XP1_SCENARIO2_MODID);
					const bool bUsingXP1Scenario3 = gDLL->IsModActivated(CIV5_XP1_SCENARIO3_MODID);

					const bool bUsingXP2Scenario1 = gDLL->IsModActivated(CIV5_XP2_SCENARIO1_MODID);
					const bool bUsingXP2Scenario2 = gDLL->IsModActivated(CIV5_XP2_SCENARIO2_MODID);

					const VictoryTypes eVictorySpaceRace = static_cast<VictoryTypes>(1);
					const VictoryTypes eVictoryDomination = static_cast<VictoryTypes>(2);
					const VictoryTypes eVictoryCultural = static_cast<VictoryTypes>(3);
					const VictoryTypes eVictoryDiplomatic = static_cast<VictoryTypes>(4);

					const PolicyBranchTypes ePolicyBranchFreedom = static_cast<PolicyBranchTypes>(9);
					const PolicyBranchTypes ePolicyBranchOrder = static_cast<PolicyBranchTypes>(10);
					const PolicyBranchTypes ePolicyBranchAutocracy = static_cast<PolicyBranchTypes>(11);

					const HandicapTypes eHandicapDeity = static_cast<HandicapTypes>(7);


					gDLL->IncrementSteamStat(ESTEAMSTAT_TOTAL_WINS);


					WorldSizeTypes	winnerMapSize = GC.getMap().getWorldSize();
					switch(winnerMapSize)
					{
					case WORLDSIZE_DUEL:
						gDLL->UnlockAchievement(ACHIEVEMENT_MAPSIZE_DUEL);
						break;
					case WORLDSIZE_TINY:
						gDLL->UnlockAchievement(ACHIEVEMENT_MAPSIZE_TINY);
						break;
					case WORLDSIZE_SMALL:
						gDLL->UnlockAchievement(ACHIEVEMENT_MAPSIZE_SMALL);
						break;
					case WORLDSIZE_STANDARD:
						if(!bUsingXP2Scenario2 && !bUsingXP2Scenario1)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_MAPSIZE_STANDARD);
						}
						break;
					case WORLDSIZE_LARGE:
						gDLL->UnlockAchievement(ACHIEVEMENT_MAPSIZE_LARGE);
						break;
					case WORLDSIZE_HUGE:
						gDLL->UnlockAchievement(ACHIEVEMENT_MAPSIZE_HUGE);
						break;
					default:
						OutputDebugString("Playing on some other kind of world size.");
					}


					CvString winnerMapName = CvPreGame::mapScriptName();



					if(winnerMapName == "Assets\\Maps\\Continents.lua")
						gDLL->UnlockAchievement(ACHIEVEMENT_MAPTYPE_CONTINENTS);
					else if(winnerMapName == "Assets\\Maps\\Pangaea.lua")
						gDLL->UnlockAchievement(ACHIEVEMENT_MAPTYPE_PANGAEA);
					else if(winnerMapName == "Assets\\Maps\\Archipelago.lua")
						gDLL->UnlockAchievement(ACHIEVEMENT_MAPTYPE_ARCHIPELAGO);
					else if(winnerMapName == "Assets\\Maps\\Earth_Duel.Civ5Map" || winnerMapName == "Assets\\Maps\\Earth_Huge.Civ5Map"
						|| winnerMapName == "Assets\\Maps\\Earth_Large.Civ5Map" || winnerMapName == "Assets\\Maps\\Earth_Small.Civ5Map"
						|| winnerMapName == "Assets\\Maps\\Earth_Standard.Civ5Map" || winnerMapName == "Assets\\Maps\\Earth_Tiny.Civ5Map")
						gDLL->UnlockAchievement(ACHIEVEMENT_MAPTYPE_EARTH);
					else
						OutputDebugString("\n Playing some other map. \n\n");



					HandicapTypes winnerHandicapType = getHandicapType();
					if(!bUsingXP2Scenario1 && !bUsingXP2Scenario2) 
					{
#ifdef AUI_WARNING_FIXES
						switch (static_cast<int>(winnerHandicapType))
#else
						switch(winnerHandicapType)
#endif
						{
						case 0:
							gDLL->UnlockAchievement(ACHIEVEMENT_DIFLEVEL_SETTLER);
							break;
						case 1:
							gDLL->UnlockAchievement(ACHIEVEMENT_DIFLEVEL_CHIEFTAIN);
							break;
						case 2:
							gDLL->UnlockAchievement(ACHIEVEMENT_DIFLEVEL_WARLORD);
							break;
						case 3:
							gDLL->UnlockAchievement(ACHIEVEMENT_DIFLEVEL_PRINCE);
							break;
						case 4:
							gDLL->UnlockAchievement(ACHIEVEMENT_DIFLEVEL_KING);
							break;
						case 5:
							gDLL->UnlockAchievement(ACHIEVEMENT_DIFLEVEL_EMPEROR);
							break;
						case 6:
							gDLL->UnlockAchievement(ACHIEVEMENT_DIFLEVEL_IMMORTAL);
							break;
						case 7:
							gDLL->UnlockAchievement(ACHIEVEMENT_DIFLEVEL_DEITY);
							break;
						default:
							OutputDebugString("Playing on some non-existant dif level.");
						}
					}


					if(!bUsingXP2Scenario1 && !bUsingXP2Scenario2)
					{
#ifdef AUI_WARNING_FIXES
						switch (static_cast<int>(eNewVictory))
#else
						switch(eNewVictory)
#endif
						{
						case 0:
							OutputDebugString("No current Achievement for a time victory");
							break;
						case 1:
							gDLL->UnlockAchievement(ACHIEVEMENT_VICTORY_SPACE);
							break;
						case 2:
							{
								if(!bUsingXP1Scenario1)
								{
									gDLL->UnlockAchievement(ACHIEVEMENT_VICTORY_DOMINATION);
								}
							}
							break;
						case 3:
							gDLL->UnlockAchievement(ACHIEVEMENT_VICTORY_CULTURE);
							break;
						case 4:
							gDLL->UnlockAchievement(ACHIEVEMENT_VICTORY_DIPLO);
							break;
						default:
							OutputDebugString("Your l33t victory skills allowed you to win in some other way.");
						}
					}



					CvString pLeader =  kWinningTeamLeader.getLeaderTypeKey();

					if(!bUsingDLC6Scenario && pLeader == "LEADER_ALEXANDER")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_ALEXANDER);
					else if(pLeader == "LEADER_WASHINGTON" && !bUsingXP2Scenario1)
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_WASHINGTON);
					else if(!bUsingXP1Scenario3 && !bUsingDLC4Scenario && !bUsingXP2Scenario1 && pLeader == "LEADER_ELIZABETH")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_ELIZABETH);
					else if(!bUsingXP1Scenario2 && !bUsingXP1Scenario3 && !bUsingDLC4Scenario && pLeader == "LEADER_NAPOLEON")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_NAPOLEON);
					else if(!bUsingXP1Scenario2 && !bUsingXP1Scenario3 && !bUsingDLC4Scenario && pLeader == "LEADER_BISMARCK")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_BISMARCK);
					else if(!bUsingXP1Scenario3 && pLeader == "LEADER_CATHERINE")
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_CATHERINE);

						if(bUsingXP1Scenario1 && winnerHandicapType >= 5)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP1_42);
						}
					}
					else if(!bUsingXP1Scenario2 && !bUsingXP2Scenario2 && pLeader == "LEADER_AUGUSTUS")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_CAESAR);
					else if(!bUsingDLC6Scenario && pLeader == "LEADER_RAMESSES")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_RAMESSES);
					else if(!bUsingXP1Scenario2 && !bUsingXP1Scenario1 && pLeader == "LEADER_ASKIA")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_ASKIA);
					else if(!bUsingXP1Scenario1 && !bUsingDLC6Scenario && pLeader == "LEADER_HARUN_AL_RASHID")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_HARUN);
					else if(!bUsingDLC6Scenario && pLeader == "LEADER_DARIUS")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_DARIUS);
					else if(!bUsingDLC3Scenario && pLeader == "LEADER_GANDHI")
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_GANDHI);
						if(eNewVictory == 3 && kWinningTeamLeader.getNumCities() <= 3)
							gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_BOLLYWOOD);
					}
					else if(pLeader == "LEADER_RAMKHAMHAENG")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_RAMKHAMHAENG);
					else if(!bUsingDLC5Scenario && pLeader == "LEADER_WU_ZETIAN")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_WU);
					else if(!bUsingDLC5Scenario && pLeader == "LEADER_ODA_NOBUNAGA")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_ODA);
					else if(!bUsingDLC3Scenario && pLeader == "LEADER_HIAWATHA")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_HIAWATHA);
					else if(!bUsingDLC3Scenario && pLeader == "LEADER_MONTEZUMA")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_MONTEZUMA);
					else if(!bUsingXP1Scenario1 && !bUsingDLC6Scenario && pLeader == "LEADER_SULEIMAN")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_SULEIMAN);
					else if(pLeader == "LEADER_NEBUCHADNEZZAR")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_NEBUCHADNEZZAR);
					else if(!bUsingDLC5Scenario && pLeader == "LEADER_GENGHIS_KHAN")
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_GENGHIS);
					}
					else if(pLeader == "LEADER_ISABELLA")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_ISABELLA);
					else if(pLeader == "LEADER_PACHACUTI")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_PACHACUTI);
					else if(!bUsingDLC3Scenario && pLeader == "LEADER_KAMEHAMEHA")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_KAMEHAMEHA);
					else if(pLeader == "LEADER_HARALD")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_BLUETOOTH);
					else if(!bUsingDLC5Scenario && pLeader == "LEADER_SEJONG")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_SEJONG);
					else if(pLeader == "LEADER_MARIA")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_01);
					else if(!bUsingXP1Scenario2 && pLeader == "LEADER_THEODORA")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_02);
					else if(pLeader == "LEADER_DIDO")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_03);
					else if(pLeader == "LEADER_BOUDICCA")
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_04);

						if(bUsingXP1Scenario1 && winnerHandicapType >= 5)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP1_41);
						}
					}
					else if(pLeader == "LEADER_SELASSIE")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_05);
					else if(pLeader == "LEADER_ATTILA")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_06);
					else if(pLeader == "LEADER_PACAL")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_07);
					else if(pLeader == "LEADER_WILLIAM")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_08);
					else if(!bUsingXP1Scenario3 && pLeader == "LEADER_GUSTAVUS_ADOLPHUS")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_09);
					else if(pLeader == "LEADER_ASHURBANIPAL" )
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_01);
					else if(pLeader == "LEADER_PEDRO")
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_02);


						if(eNewVictory == eVictoryDiplomatic)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP2_20);
						}
					}
					else if(pLeader == "LEADER_GAJAH_MADA" )
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_03);
					else if(pLeader == "LEADER_AHMAD_ALMANSUR")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_04);
					else if(pLeader == "LEADER_CASIMIR")
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_05);


						if(eNewVictory == eVictorySpaceRace)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP2_23);
						}
					}
					else if(pLeader == "LEADER_MARIA_I")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_06);
					else if(pLeader == "LEADER_POCATELLO")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_07);
					else if(pLeader == "LEADER_ENRICO_DANDOLO")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_08);
					else if(pLeader == "LEADER_SHAKA")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_09);
					else
						OutputDebugString("\nPlaying with a non-standard leader.\n");


					if(kWinningTeamLeader.getNumCities() == 1)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_ONECITY);
					}


					if(gDLL->IsAchievementUnlocked(ACHIEVEMENT_MAPSIZE_DUEL) &&  gDLL->IsAchievementUnlocked(ACHIEVEMENT_MAPSIZE_TINY) &&  gDLL->IsAchievementUnlocked(ACHIEVEMENT_MAPSIZE_SMALL) &&  gDLL->IsAchievementUnlocked(ACHIEVEMENT_MAPSIZE_STANDARD) &&  gDLL->IsAchievementUnlocked(ACHIEVEMENT_MAPSIZE_LARGE) &&  gDLL->IsAchievementUnlocked(ACHIEVEMENT_MAPSIZE_HUGE) &&  gDLL->IsAchievementUnlocked(ACHIEVEMENT_MAPTYPE_ARCHIPELAGO) &&  gDLL->IsAchievementUnlocked(ACHIEVEMENT_MAPTYPE_CONTINENTS) &&  gDLL->IsAchievementUnlocked(ACHIEVEMENT_MAPTYPE_EARTH) &&  gDLL->IsAchievementUnlocked(ACHIEVEMENT_MAPTYPE_PANGAEA))
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_MAPS_ALL);
					}
					if(gDLL->IsAchievementUnlocked(ACHIEVEMENT_VICTORY_CULTURE) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_VICTORY_SPACE) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_VICTORY_DIPLO) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_VICTORY_DOMINATION))
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_VICTORY_ALL);
					}
					if(gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_WASHINGTON) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_ELIZABETH) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_NAPOLEON)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_BISMARCK)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_CATHERINE)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_CAESAR)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_ALEXANDER)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_RAMESSES)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_ASKIA)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_HARUN)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_DARIUS)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_GANDHI)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_RAMKHAMHAENG)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_WU)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_ODA)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_HIAWATHA)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_MONTEZUMA)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_SULEIMAN))
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_ALLBASELEADERS);
					}


					const PolicyBranchTypes eBranch =kWinningTeamLeader.GetPlayerPolicies()->GetLateGamePolicyTree();
					if(eBranch != NO_POLICY_BRANCH_TYPE)
					{
						if(eNewVictory == eVictorySpaceRace)
						{
							if(eBranch == ePolicyBranchFreedom)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_10);
							}
							else if(eBranch == ePolicyBranchOrder)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_11);
							}
						}
						else if(eNewVictory == eVictoryDomination)
						{
							if(eBranch == ePolicyBranchOrder)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_17);
							}
							else if(eBranch == ePolicyBranchAutocracy)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_18);
							}
						}
						else if(eNewVictory == eVictoryCultural)
						{
							if(eBranch == ePolicyBranchFreedom)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_12);
							}
							else if(eBranch == ePolicyBranchOrder)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_13);
							}
							else if(eBranch == ePolicyBranchAutocracy)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_14);
							}
						}
						else if(eNewVictory == eVictoryDiplomatic)
						{
							if(eBranch == ePolicyBranchFreedom)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_15);
							}
							else if(eBranch == ePolicyBranchAutocracy)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_16);
							}
						}
					}


#ifndef AUI_LEAGUES_FIX_POSSIBLE_DEALLOCATION_CRASH
					if (GetGameLeagues()->GetNumActiveLeagues() > 0)
#endif
					{
						CvLeague* pLeague = GetGameLeagues()->GetActiveLeague();
						if (pLeague)
						{

							if (eNewVictory == eVictoryDiplomatic)
							{
								if (!pLeague->HasMemberEverBeenHost(kWinningTeamLeader.GetID()))
								{
									gDLL->UnlockAchievement(ACHIEVEMENT_XP2_43);
								}
							}
						}
					}


					CvAchievementUnlocker::Check_PSG();


					if(bUsingDLC1Scenario)
					{
						if(eNewVictory == 2)
						{
							CvString strHandicapType = this->getHandicapInfo().GetType();


							bool bBeatOnHarderDifficulty = false;

							if(strHandicapType == "HANDICAP_DEITY")
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_WIN_SCENARIO_01_DEITY);
								bBeatOnHarderDifficulty = true;
							}

							if(bBeatOnHarderDifficulty || strHandicapType == "HANDICAP_IMMORTAL")
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_WIN_SCENARIO_01_IMMORTAL);
								bBeatOnHarderDifficulty = true;
							}

							if(bBeatOnHarderDifficulty || strHandicapType == "HANDICAP_EMPEROR")
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_WIN_SCENARIO_01_EMPEROR);
								bBeatOnHarderDifficulty = true;
							}

							if(bBeatOnHarderDifficulty || strHandicapType == "HANDICAP_KING")
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_WIN_SCENARIO_01_KING);
								bBeatOnHarderDifficulty = true;
							}


							gDLL->UnlockAchievement(ACHIEVEMENT_WIN_SCENARIO_01_PRINCE_OR_BELOW);
						}
					}


					if(bUsingDLC2Scenario)
					{
						CvString strCivType = kWinningTeamLeader.getCivilizationInfo().GetType();
						if(strCivType == "CIVILIZATION_SPAIN")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_02_WIN_SPAIN);
						else if(strCivType == "CIVILIZATION_FRANCE")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_02_WIN_FRANCE);
						else if(strCivType == "CIVILIZATION_ENGLAND")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_02_WIN_ENGLAND);
						else if(strCivType == "CIVILIZATION_INCA")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_02_WIN_INCA);
						else if(strCivType == "CIVILIZATION_AZTEC")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_02_WIN_AZTECS);
						else if(strCivType == "CIVILIZATION_IROQUOIS")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_02_WIN_IROQUOIS);
					}


					if(bUsingDLC3Scenario)
					{
						CvString strCivType = kWinningTeamLeader.getCivilizationInfo().GetType();
						if(strCivType == "CIVILIZATION_POLYNESIA")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_03_WIN_HIVA);
						else if(strCivType == "CIVILIZATION_IROQUOIS")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_03_WIN_TAHITI);
						else if(strCivType == "CIVILIZATION_INDIA")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_03_WIN_SAMOA);
						else if(strCivType == "CIVILIZATION_AZTEC")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_03_WIN_TONGA);
					}


					if(bUsingDLC4Scenario)
					{
						CvString strCivType = kWinningTeamLeader.getCivilizationInfo().GetType();
						if(strCivType == "CIVILIZATION_DENMARK")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_04_WIN_DENMARK);
						else if(strCivType == "CIVILIZATION_ENGLAND")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_04_WIN_ENGLAND);
						else if(strCivType == "CIVILIZATION_GERMANY")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_04_WIN_NORWAY);
						else if(strCivType == "CIVILIZATION_FRANCE")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_04_NORMANDY);

#ifdef AUI_WARNING_FIXES
						switch (static_cast<int>(winnerHandicapType))
#else
						switch(winnerHandicapType)
#endif
						{
						case 5:
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_04_WIN_EMPEROR);
							break;
						case 6:
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_04_WIN_IMMORTAL);
							break;
						case 7:
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_04_WIN_DEITY);
							break;
						}
					}


					if(bUsingDLC5Scenario)
					{

						CvString strCivType = kWinningTeamLeader.getCivilizationInfo().GetType();
						if(strCivType == "CIVILIZATION_JAPAN")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_05_WIN_JAPAN);
						else if(strCivType == "CIVILIZATION_KOREA")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_05_WIN_KOREA);
						else if(strCivType == "CIVILIZATION_CHINA")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_05_WIN_CHINA);
						else if(strCivType == "CIVILIZATION_MONGOL")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_05_WIN_MANCHU);


#ifdef AUI_WARNING_FIXES
						switch (static_cast<int>(winnerHandicapType))
#else
						switch(winnerHandicapType)
#endif
						{
						case 5:
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_05_WIN_EMPEROR);
							break;
						case 6:
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_05_WIN_IMMORTAL);
							break;
						case 7:
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_05_WIN_DEITY);
							break;
						}


						if(getGameTurn() >= 0 && getGameTurn() < 100)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_05_WIN_100TURNS);
						}

					}


					if(bUsingDLC6Scenario)
					{

						CvString strCivType = kWinningTeamLeader.getCivilizationInfo().GetType();
						if(strCivType == "CIVILIZATION_OTTOMAN")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_06_WIN_HITTITES);
						else if(strCivType == "CIVILIZATION_GREECE")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_06_WIN_GREECE);
						else if(strCivType == "CIVILIZATION_ARABIA")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_06_WIN_SUMER);
						else if(strCivType == "CIVILIZATION_EGYPT")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_06_WIN_EGYPT);
						else if(strCivType == "CIVILIZATION_PERSIA")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_06_WIN_PERSIA);


#ifdef AUI_WARNING_FIXES
						switch (static_cast<int>(winnerHandicapType))
#else
						switch(winnerHandicapType)
#endif
						{
						case 3:
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_06_WIN_PRINCE);
							break;
						case 4:
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_06_WIN_KING);
							break;
						case 5:
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_06_WIN_EMPEROR);
							break;
						case 6:
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_06_WIN_IMMORTAL);
							break;
						case 7:
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_06_WIN_DEITY);
							break;
						}

					}

					if(bUsingXP1Scenario1)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_21);

						if(pLeader == "LEADER_HARUN_AL_RASHID" && winnerHandicapType >= 5)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP1_43);
						}
					}

					if(bUsingXP1Scenario2)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_22);

						if(winnerHandicapType >= 7)
						{
							if(pLeader == "LEADER_THEODORA")
							{
								typedef std::pair<int,int> Location;
								typedef std::tr1::array<Location, 27> OriginalCitiesArray;
								OriginalCitiesArray OriginalCities = {
									Location(65,6),
									Location(48,7),
									Location(71,9),
									Location(79,13),
									Location(74,14),
									Location(49,18),
									Location(68,20),
									Location(73,20),
									Location(63,21),
									Location(76,21),
									Location(51,22),
									Location(47,23),
									Location(57,23),
									Location(67,24),
									Location(75,26),
									Location(63,27),
									Location(50,28),
									Location(58,29),
									Location(45,30),
									Location(71,30),
									Location(54,31),
									Location(64,32),
									Location(78,32),
									Location(49,34),
									Location(69,34),
									Location(75,34),
									Location(45,35),

								};					

								CvMap& kMap = GC.getMap();
								const PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
								bool bHasAllCities = true;


								for(OriginalCitiesArray::iterator it = OriginalCities.begin(); it != OriginalCities.end(); ++it)
								{
									CvPlot* pkPlot = kMap.plot(it->first, it->second);
									if(pkPlot != NULL)
									{
										CvCity* pkCity = pkPlot->getPlotCity();
										if(pkCity != NULL)
										{
											if(pkCity->getOwner() == eActivePlayer)
											{
												continue;
											}
										}
									}

									bHasAllCities = false;
								}

								if(bHasAllCities)
								{
									gDLL->UnlockAchievement(ACHIEVEMENT_XP1_50);
								}
							}
							else if(pLeader == "LEADER_AUGUSTUS")
							{
								typedef std::pair<int,int> Location;
								typedef std::tr1::array<Location, 29> OriginalCitiesArray;
								OriginalCitiesArray OriginalCities = {
									Location(10,16),
									Location(15,18),
									Location(30,18),
									Location(37,19),
									Location(4,23),
									Location(13,25),
									Location(37,27),
									Location(41,27),
									Location(1,29),
									Location(6,29),
									Location(15,30),
									Location(33,31),
									Location(14,34),
									Location(20,36),
									Location(24,36),
									Location(40,36),
									Location(28,37),
									Location(34,37),
									Location(3,39),
									Location(30,40),
									Location(16,42),
									Location(22,42),
									Location(32,45),
									Location(41,45),
									Location(14,47),
									Location(21,48),
									Location(26,49),
									Location(17,52),
									Location(22,53)
								};

								CvMap& kMap = GC.getMap();
								const PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
								bool bHasAllCities = true;


								for(OriginalCitiesArray::iterator it = OriginalCities.begin(); it != OriginalCities.end(); ++it)
								{
									CvPlot* pkPlot = kMap.plot(it->first, it->second);
									if(pkPlot != NULL)
									{
										CvCity* pkCity = pkPlot->getPlotCity();
										if(pkCity != NULL)
										{
											if(pkCity->getOwner() == eActivePlayer)
											{
												continue;
											}
										}
									}

									bHasAllCities = false;
								}

								if(bHasAllCities)
								{
									gDLL->UnlockAchievement(ACHIEVEMENT_XP1_50);
								}
							}
						}
					}

					if(bUsingXP1Scenario3)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_23);
					}


					if(bUsingXP2Scenario1)
					{
						CvString strCivType = kWinningTeamLeader.getCivilizationInfo().GetType();
						if(strCivType == "CIVILIZATION_AMERICA")
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP2_56);
						}
						else
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP2_57);
						}


						if (winnerHandicapType == eHandicapDeity)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP2_58);
						}
					}

					if(bUsingXP2Scenario2)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_49);


						if (winnerHandicapType == eHandicapDeity)
						{
							CvString strCivType = kWinningTeamLeader.getCivilizationInfo().GetType();
							if(strCivType == "CIVILIZATION_AMERICA")
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_50);
							}
							else if(strCivType == "CIVILIZATION_OTTOMAN")
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_51);
							}
						}
					}
				}

				if(GC.getGame().isGameMultiPlayer() && kWinningTeamLeader.isHuman() && (GET_PLAYER(GC.getGame().getActivePlayer()).GetID() == kWinningTeamLeader.GetID()))
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_WIN_MULTIPLAYER);
				}

			}

			if((getAIAutoPlay() > 0) || gDLL->GetAutorun())
			{
				setGameState(GAMESTATE_EXTENDED);
			}
			else
			{
				setGameState(GAMESTATE_OVER);
			}
		}

		GC.GetEngineUserInterface()->setDirty(Center_DIRTY_BIT, true);

		GC.GetEngineUserInterface()->setDirty(Soundtrack_DIRTY_BIT, true);
	}
}



bool CvGame::isVictoryAvailable(VictoryTypes eVictory) const
{
	if(getTeamVictoryRank(eVictory, GC.getNUM_VICTORY_POINT_AWARDS() - 1) == NO_TEAM)
	{
		return true;
	}

	return false;
}



int CvGame::GetNextAvailableVictoryCompetitionRank(VictoryTypes eVictory) const
{
	for(int iRankLoop = 0; iRankLoop < GC.getNUM_VICTORY_POINT_AWARDS(); iRankLoop++)
	{
		if(getTeamVictoryRank(eVictory, iRankLoop) == NO_TEAM)
		{
			return iRankLoop;
		}
	}

	return -1;
}



void CvGame::DoPlaceTeamInVictoryCompetition(VictoryTypes eNewVictory, TeamTypes eTeam)
{
	CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eNewVictory);
	if(pkVictoryInfo == NULL)
		return;


	if(isVictoryAvailable(eNewVictory))
	{
		CvTeam& kTeam = GET_TEAM(eTeam);


		for(int iSlotLoop = 0; iSlotLoop < GC.getNUM_VICTORY_POINT_AWARDS(); iSlotLoop++)
		{
			if(getTeamVictoryRank(eNewVictory, iSlotLoop) == NO_TEAM)
			{
				setTeamVictoryRank(eNewVictory, iSlotLoop, eTeam);
				int iNumPoints = pkVictoryInfo->GetVictoryPointAward(iSlotLoop);
				kTeam.changeVictoryPoints(iNumPoints);
				kTeam.setVictoryAchieved(eNewVictory, true);

				Localization::String youWonInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_VICTORY_RACE_WON_YOU");
				Localization::String youWonSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_VICTORY_RACE_WON_YOU");
				Localization::String someoneWonInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_VICTORY_RACE_WON_SOMEBODY");
				Localization::String someoneWonSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_VICTORY_RACE_WON_SOMEBODY");
				Localization::String unmetWonInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_VICTORY_RACE_WON_UNMET");
				Localization::String unmetWonSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_VICTORY_RACE_WON_UNMET");

				for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
					PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
					CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
					CvNotifications* pNotification = kCurNotifyPlayer.GetNotifications();
					if(pNotification)
					{
						Localization::String localizedText;
						Localization::String localizedSummary;

						const char* szVictoryTextKey = pkVictoryInfo->GetTextKey();

						if(eTeam == kCurNotifyPlayer.getTeam())
						{
							localizedText = youWonInfo;
							localizedText << iSlotLoop+1 << szVictoryTextKey << iNumPoints;
							localizedSummary = youWonSummary;
							localizedSummary << iSlotLoop+1 << szVictoryTextKey;
						}

						else if(GET_TEAM(kCurNotifyPlayer.getTeam()).isHasMet(eTeam))
						{
							const char* szTeamLeaderNameKey = GET_PLAYER(kTeam.getLeaderID()).getNameKey();

							localizedText = someoneWonInfo;
							localizedText << szTeamLeaderNameKey << iSlotLoop+1 << szVictoryTextKey << iNumPoints;
							localizedSummary = someoneWonSummary;
							localizedSummary << szTeamLeaderNameKey << iSlotLoop+1 << szVictoryTextKey;
						}

						else
						{
							localizedText = unmetWonInfo;
							localizedText << iSlotLoop+1 << szVictoryTextKey << iNumPoints;
							localizedSummary = unmetWonSummary;
							localizedSummary << iSlotLoop+1 << szVictoryTextKey;
						}
						pNotification->Add(NOTIFICATION_VICTORY, localizedText.toUTF8(), localizedSummary.toUTF8(), -1, -1, -1);
					}
				}

				break;
			}
		}
	}
}


TeamTypes CvGame::getTeamVictoryRank(VictoryTypes eNewVictory, int iRank) const
{
	CvAssert(iRank >= 0);
	CvAssert(iRank < GC.getNUM_VICTORY_POINT_AWARDS());

	return (TeamTypes) m_ppaaiTeamVictoryRank[eNewVictory][iRank];
}



void CvGame::setTeamVictoryRank(VictoryTypes eNewVictory, int iRank, TeamTypes eTeam)
{
	CvAssert(iRank >= 0);
	CvAssert(iRank < GC.getNUM_VICTORY_POINT_AWARDS());

	m_ppaaiTeamVictoryRank[eNewVictory][iRank] = eTeam;
}



int CvGame::GetWorldMilitaryStrengthAverage(PlayerTypes ePlayer, bool bIncludeMe, bool bIncludeOnlyKnown)
{
	int iWorldMilitaryStrength = 0;
	int iNumAlivePlayers = 0;


	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		if(GET_PLAYER((PlayerTypes) iMajorLoop).isAlive())
		{
			if(bIncludeMe || iMajorLoop != ePlayer)
			{
				if(!bIncludeOnlyKnown || (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GET_PLAYER((PlayerTypes) iMajorLoop).getTeam())))
				{
					iNumAlivePlayers++;
					iWorldMilitaryStrength += GET_PLAYER((PlayerTypes) iMajorLoop).GetMilitaryMight();
				}
			}
		}
	}

	if(iNumAlivePlayers > 0)
	{
		iWorldMilitaryStrength /= iNumAlivePlayers;
	}

	return iWorldMilitaryStrength;
}



int CvGame::GetTotalReligionTechCost() const
{
	return m_iTotalReligionTechCost;
}



void CvGame::DoUpdateTotalReligionTechCost()
{
	TechTypes ePrereqTech = GetReligionTech();


	if(ePrereqTech == NO_TECH)
	{
		m_iTotalReligionTechCost = -1;
		return;
	}

	if(ePrereqTech != NO_TECH)
	{
		m_iTotalReligionTechCost = GetResearchLeftToTech(NO_TEAM, ePrereqTech);
	}
}



int CvGame::GetCachedWorldReligionTechProgress() const
{
	return m_iCachedWorldReligionTechProgress;
}



void CvGame::DoUpdateCachedWorldReligionTechProgress()
{
	TechTypes ePrereqTech = GetReligionTech();


	if(ePrereqTech == NO_TECH)
	{
		m_iCachedWorldReligionTechProgress = -1;
		return;
	}

	TeamTypes eTeam;
	int iNumTeams = 0;

	int iResearchLeftToReligionTech = 0;


	for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
	{
		eTeam = (TeamTypes) iTeamLoop;

		if(GET_TEAM(eTeam).isAlive() && !GET_TEAM(eTeam).isMinorCiv())
		{
			iNumTeams++;
			iResearchLeftToReligionTech += GetResearchLeftToReligionTech(eTeam);
		}
	}

	CvAssert(iNumTeams > 0);
	m_iCachedWorldReligionTechProgress = iResearchLeftToReligionTech / iNumTeams;
}



TechTypes CvGame::GetReligionTech() const
{
	return m_eReligionTech;
}



void CvGame::SetReligionTech(TechTypes eTech)
{

	CvAssert(m_eReligionTech == NO_TECH);

	m_eReligionTech = eTech;
}



int CvGame::GetResearchLeftToReligionTech(TeamTypes eTeam)
{
	TechTypes eReligionTech = GetReligionTech();


	if(eReligionTech == NO_TECH)
	{
		return -1;
	}

	int iResearchLeft = GetResearchLeftToTech(eTeam, eReligionTech);

	return iResearchLeft;
}




int CvGame::GetResearchLeftToTech(TeamTypes eTeam, TechTypes eTech)
{
	CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);


	if(pkTechInfo == NULL)
		return 0;


	if(pkTechInfo->GetPrereqAndTechs(0) == NO_TECH)
	{
		return pkTechInfo->GetResearchCost();
	}


	if(eTeam != NO_TEAM && GET_TEAM(eTeam).GetTeamTechs()->HasTech(eTech))
	{
		return 0;
	}

	int iPrereqTechCost = 0;
	TechTypes ePreReq;

	for(int i = 0; i < GC.getNUM_AND_TECH_PREREQS(); i++)
	{
		ePreReq = (TechTypes) pkTechInfo->GetPrereqAndTechs(i);

		if(ePreReq != NO_TECH)
		{
			iPrereqTechCost += GetResearchLeftToTech(eTeam, ePreReq);
		}
	}

	return pkTechInfo->GetResearchCost() + iPrereqTechCost;
}


GameStateTypes CvGame::getGameState()
{
	return m_eGameState;
}



void CvGame::setGameState(GameStateTypes eNewValue)
{
	if(getGameState() != eNewValue)
	{
		m_eGameState = eNewValue;

		if(eNewValue == GAMESTATE_OVER || eNewValue == GAMESTATE_EXTENDED)
		{
			if (!isGameMultiPlayer())
			{
#ifndef AUI_LEAGUES_FIX_POSSIBLE_DEALLOCATION_CRASH
				if (GetGameLeagues()->GetNumActiveLeagues() > 0)
#endif
				{
					CvLeague* pLeague = GetGameLeagues()->GetActiveLeague();
					if (pLeague)
					{
						if (pLeague->HasMemberAlwaysBeenHost(getActivePlayer()))
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP2_41);
						}
					}
				}
			}
		}

		if(eNewValue == GAMESTATE_OVER)
		{

			if(!isGameMultiPlayer())
			{
				bool bLocalPlayerLost = true;

				const TeamTypes eWinningTeam = getWinner();
				if(eWinningTeam != NO_TEAM)
				{
					CvPlayerAI& kLocalPlayer = GET_PLAYER(getActivePlayer());
					if(eWinningTeam == kLocalPlayer.getTeam())
						bLocalPlayerLost = false;
				}
				if(bLocalPlayerLost)
				{



					bool bUsingDLC1MongolScenario = gDLL->IsModActivated(CIV5_DLC_01_SCENARIO_MODID);
					if(bUsingDLC1MongolScenario && getGameTurn() == 100)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_LOSE_SCENARIO_01);
					}


					bool bUsingDLC5Scenario = gDLL->IsModActivated(CIV5_DLC_05_SCENARIO_MODID);
					if(bUsingDLC5Scenario)
					{
						CvPlayerAI& kLocalPlayer = GET_PLAYER(getActivePlayer());
						CvString strCivType = kLocalPlayer.getCivilizationInfo().GetType();
						if(strCivType == "CIVILIZATION_KOREA")
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_05_LOSE_KOREA);
						}
					}
				}
			}


			long iHours = getMinutesPlayed() / 60;
			long iMinutes = getMinutesPlayed() % 60;

			for(int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
			{
				CvPlayer& player = GET_PLAYER((PlayerTypes)iI);
				if(player.isHuman())
				{
					addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, (PlayerTypes)iI, GetLocalizedText("TXT_KEY_MISC_TIME_SPENT", iHours, iMinutes));

				}
			}

			saveReplay();
			showEndGameSequence();
		}

		GC.GetEngineUserInterface()->setDirty(Cursor_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	}
}


CvGameSpeedInfo& CvGame::getGameSpeedInfo() const
{
	CvGameSpeedInfo* pkGameSpeedInfo = GC.getGameSpeedInfo(getGameSpeedType());
	if(pkGameSpeedInfo == NULL)
	{
		const char* szError = "ERROR: Game does not contain valid game speed!!";
		GC.LogMessage(szError);
		CvAssertMsg(false, szError);
	}

#pragma warning ( push )
#pragma warning ( disable : 6011 )
	return *pkGameSpeedInfo;
#pragma warning ( pop )
}


GameSpeedTypes CvGame::getGameSpeedType() const
{
	return CvPreGame::gameSpeed();
}


CvEraInfo& CvGame::getStartEraInfo() const
{
	CvEraInfo* pkStartEraInfo = GC.getEraInfo(getStartEra());
	if(pkStartEraInfo == NULL)
	{
		const char* szError = "ERROR: Game does not contain valid start era!!";
		GC.LogMessage(szError);
		CvAssertMsg(false, szError);
	}

#pragma warning ( push )
#pragma warning ( disable : 6011 )
	return *pkStartEraInfo;
#pragma warning ( pop )
}


EraTypes CvGame::getStartEra() const
{
	return CvPreGame::era();
}


CalendarTypes CvGame::getCalendar() const
{
	return CvPreGame::calendar();
}



int CvGame::getEndTurnMessagesReceived(int iIndex)
{
	CvAssertMsg(iIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(iIndex < MAX_PLAYERS, "iIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiEndTurnMessagesReceived[iIndex];
}



void CvGame::incrementEndTurnMessagesReceived(int iIndex)
{
	CvAssertMsg(iIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(iIndex < MAX_PLAYERS, "iIndex is expected to be within maximum bounds (invalid Index)");
	m_aiEndTurnMessagesReceived[iIndex]++;
}



PlayerTypes CvGame::getRankPlayer(int iRank)
{
	CvAssertMsg(iRank >= 0, "iRank is expected to be non-negative (invalid Rank)");
	CvAssertMsg(iRank < MAX_PLAYERS, "iRank is expected to be within maximum bounds (invalid Rank)");
	return (PlayerTypes)m_aiRankPlayer[iRank];
}



void CvGame::setRankPlayer(int iRank, PlayerTypes ePlayer)
{
	CvAssertMsg(iRank >= 0, "iRank is expected to be non-negative (invalid Rank)");
	CvAssertMsg(iRank < MAX_PLAYERS, "iRank is expected to be within maximum bounds (invalid Rank)");

	if(getRankPlayer(iRank) != ePlayer)
	{
		m_aiRankPlayer[iRank] = ePlayer;
	}
}



int CvGame::getPlayerRank(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	return m_aiPlayerRank[ePlayer];
}



void CvGame::setPlayerRank(PlayerTypes ePlayer, int iRank)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	m_aiPlayerRank[ePlayer] = iRank;
	CvAssert(getPlayerRank(ePlayer) >= 0);
}



int CvGame::getPlayerScore(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	return m_aiPlayerScore[ePlayer];
}



void CvGame::setPlayerScore(PlayerTypes ePlayer, int iScore)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	if(getPlayerScore(ePlayer) != iScore)
	{
		m_aiPlayerScore[ePlayer] = iScore;
		CvAssert(getPlayerScore(ePlayer) >= 0);

		GC.GetEngineUserInterface()->setDirty(Score_DIRTY_BIT, true);
	}
}



TeamTypes CvGame::getRankTeam(int iRank)
{
	CvAssertMsg(iRank >= 0, "iRank is expected to be non-negative (invalid Rank)");
	CvAssertMsg(iRank < MAX_TEAMS, "iRank is expected to be within maximum bounds (invalid Index)");
	return (TeamTypes)m_aiRankTeam[iRank];
}



void CvGame::setRankTeam(int iRank, TeamTypes eTeam)
{
	CvAssertMsg(iRank >= 0, "iRank is expected to be non-negative (invalid Rank)");
	CvAssertMsg(iRank < MAX_TEAMS, "iRank is expected to be within maximum bounds (invalid Index)");

	if(getRankTeam(iRank) != eTeam)
	{
		m_aiRankTeam[iRank] = eTeam;
	}
}



int CvGame::getTeamRank(TeamTypes eTeam)
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	return m_aiTeamRank[eTeam];
}



void CvGame::setTeamRank(TeamTypes eTeam, int iRank)
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	m_aiTeamRank[eTeam] = iRank;
	CvAssert(getTeamRank(eTeam) >= 0);
}



int CvGame::getTeamScore(TeamTypes eTeam) const
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	return m_aiTeamScore[eTeam];
}



void CvGame::setTeamScore(TeamTypes eTeam, int iScore)
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	m_aiTeamScore[eTeam] = iScore;
	CvAssert(getTeamScore(eTeam) >= 0);
}



bool CvGame::isOption(GameOptionTypes eIndex) const
{
	int i = 0;
	CvPreGame::GetGameOption(eIndex, i);

	return (i == 1);
}


bool CvGame::isOption(const char* pszOption) const
{
	int i = 0;
	CvPreGame::GetGameOption(pszOption, i);

	return (i == 1);
}


void CvGame::setOption(GameOptionTypes eIndex, bool bEnabled)
{
	CvPreGame::SetGameOption(eIndex, (int)bEnabled);
}


void CvGame::setOption(const char* pszOption, bool bEnabled)
{
	CvPreGame::SetGameOption(pszOption, (int)bEnabled);
}

#if defined(LEKMOD_WC_RESPECT_ACTIVATION_ORDER)

void CvGame::StoreTurnActivationOrder(const int* aiShuffle)
{
	if (aiShuffle == NULL)
	{
		return;
	}
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		m_aiTurnActivationOrder[i] = aiShuffle[i];
	}
}


int CvGame::GetTurnActivationOrderIndex(PlayerTypes ePlayer) const
{
	if (ePlayer < 0 || ePlayer >= MAX_PLAYERS)
	{
		return MAX_PLAYERS;
	}
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (m_aiTurnActivationOrder[i] == (int)ePlayer)
		{
			return i;
		}
	}
	return (int)ePlayer;
}


bool CvGame::IsRandomizedTurnActivationOrderEnabled() const
{
	return isOption("GAMEOPTION_SIMULTANEOUS_PLAYER_TURN_ACTIVATION_ORDER_RANDOMIZED");
}


bool CvGame::IsPreferredByTurnActivationOrder(PlayerTypes eA, PlayerTypes eB) const
{
	if (eB == NO_PLAYER)
	{
		return true;
	}
	if (eA == NO_PLAYER)
	{
		return false;
	}
	if (IsRandomizedTurnActivationOrderEnabled())
	{
		return GetTurnActivationOrderIndex(eA) < GetTurnActivationOrderIndex(eB);
	}
	return ((int)eA < (int)eB);
}
#endif



bool CvGame::isMPOption(MultiplayerOptionTypes eIndex) const
{
	return CvPreGame::multiplayerOptionEnabled(eIndex);
}



void CvGame::setMPOption(MultiplayerOptionTypes eIndex, bool bEnabled)
{
	CvPreGame::setMultiplayerOption(eIndex, bEnabled);
}


int CvGame::getUnitCreatedCount(UnitTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiUnitCreatedCount[eIndex];
}



void CvGame::incrementUnitCreatedCount(UnitTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiUnitCreatedCount[eIndex]++;
}



int CvGame::getUnitClassCreatedCount(UnitClassTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiUnitClassCreatedCount[eIndex];
}



bool CvGame::isUnitClassMaxedOut(UnitClassTypes eIndex, int iExtra)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eIndex);
	if(pkUnitClassInfo == NULL)
	{
		return false;
	}

	if(!isWorldUnitClass(eIndex))
	{
		return false;
	}

	CvAssertMsg(getUnitClassCreatedCount(eIndex) <= pkUnitClassInfo->getMaxGlobalInstances(), "Index is expected to be within maximum bounds (invalid Index)");

	return ((getUnitClassCreatedCount(eIndex) + iExtra) >= pkUnitClassInfo->getMaxGlobalInstances());
}



void CvGame::incrementUnitClassCreatedCount(UnitClassTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiUnitClassCreatedCount[eIndex]++;
}



int CvGame::getBuildingClassCreatedCount(BuildingClassTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiBuildingClassCreatedCount[eIndex];
}



bool CvGame::isBuildingClassMaxedOut(BuildingClassTypes eIndex, int iExtra)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eIndex);
	if(pkBuildingClassInfo == NULL)
	{
		CvAssertMsg(false, "BuildingClassInfo does not exist for type.  NOT GOOD");
		return false;
	}


	if(!isWorldWonderClass(*pkBuildingClassInfo))
	{
		return false;
	}

	CvAssertMsg(getBuildingClassCreatedCount(eIndex) <= pkBuildingClassInfo->getMaxGlobalInstances(), "Index is expected to be within maximum bounds (invalid Index)");

	return ((getBuildingClassCreatedCount(eIndex) + iExtra) >= pkBuildingClassInfo->getMaxGlobalInstances());
}



void CvGame::incrementBuildingClassCreatedCount(BuildingClassTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiBuildingClassCreatedCount[eIndex]++;
}



int CvGame::getProjectCreatedCount(ProjectTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiProjectCreatedCount[eIndex];
}



bool CvGame::isProjectMaxedOut(ProjectTypes eIndex, int iExtra)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(!isWorldProject(eIndex))
	{
		return false;
	}

	CvAssertMsg(getProjectCreatedCount(eIndex) <= GC.getProjectInfo(eIndex)->GetMaxGlobalInstances(), "Index is expected to be within maximum bounds (invalid Index)");

	return ((getProjectCreatedCount(eIndex) + iExtra) >= GC.getProjectInfo(eIndex)->GetMaxGlobalInstances());
}



void CvGame::incrementProjectCreatedCount(ProjectTypes eIndex, int iExtra)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiProjectCreatedCount[eIndex] += iExtra;
}


bool CvGame::isVictoryValid(VictoryTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumVictoryInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return CvPreGame::isVictory(eIndex);
}


void CvGame::setVictoryValid(VictoryTypes eIndex, bool bValid)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumVictoryInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	CvPreGame::setVictory(eIndex, bValid);
}


bool CvGame::areNoVictoriesValid() const
{
	bool bRtnValue = true;

#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumVictoryInfos(); iI++)
#else
	for(int iI = 0; iI < GC.getNumVictoryInfos(); iI++)
#endif
	{
		VictoryTypes eVictory = static_cast<VictoryTypes>(iI);
		CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eVictory);
		if(pkVictoryInfo)
		{
			if(isVictoryValid(eVictory))
			{
				bRtnValue = false;
				break;
			}
		}
	}

	return bRtnValue;
}


bool CvGame::isSpecialUnitValid(SpecialUnitTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumSpecialUnitInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabSpecialUnitValid[eIndex];
}



void CvGame::makeSpecialUnitValid(SpecialUnitTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumSpecialUnitInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_pabSpecialUnitValid[eIndex] = true;
}



bool CvGame::isNukesValid() const
{
	return m_bNukesValid;
}



void CvGame::makeNukesValid(bool bValid)
{
	m_bNukesValid = bValid;
}


bool CvGame::isInAdvancedStart() const
{
	for(int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		if((GET_PLAYER((PlayerTypes)iPlayer).getAdvancedStartPoints() >= 0) && GET_PLAYER((PlayerTypes)iPlayer).isHuman())
		{
			return true;
		}
	}

	return false;
}


const CvString& CvGame::getName()
{
	return CvPreGame::gameName();
}



void CvGame::setName(const char* szName)
{
	CvPreGame::setGameName(szName);
}


bool CvGame::isDestroyedCityName(CvString& szName) const
{
	std::vector<CvString>::const_iterator it;
	for(it = m_aszDestroyedCities.begin(); it != m_aszDestroyedCities.end(); ++it)
	{
		if(*it == szName)
		{
			return true;
		}
	}

	return false;
}


void CvGame::addDestroyedCityName(const CvString& szName)
{
	m_aszDestroyedCities.push_back(szName);
}


bool CvGame::isGreatPersonBorn(CvString& szName) const
{
	std::vector<CvString>::const_iterator it;
	for(it = m_aszGreatPeopleBorn.begin(); it != m_aszGreatPeopleBorn.end(); ++it)
	{
		if(*it == szName)
		{
			return true;
		}
	}

	return false;
}


void CvGame::addGreatPersonBornName(const CvString& szName)
{
	m_aszGreatPeopleBorn.push_back(szName);
}





void CvGame::doTurn()
{
#ifndef FINAL_RELEASE
	char temp[256];
	sprintf_s(temp, "Turn %i\n", getGameTurn());
	OutputDebugString(temp);
#endif

	int aiShuffle[MAX_PLAYERS];
	int iLoopPlayer;
	int iI;

	if (getAIAutoPlay())
	{
		gDLL->AutoSave(false);
	}



#ifdef GAME_UPDATE_TURN_TIMER_ONCE_PER_TURN
	setPreviousTurnLen(static_cast<float>(getMaxTurnLen()));
#endif
#ifdef TURN_TIMER_PAUSE_BUTTON
	GC.getGame().m_bIsPaused = false;
#endif
#ifdef CS_ALLYING_WAR_RESCTRICTION
	if (GC.getGame().isOption(GAMEOPTION_END_TURN_TIMER_ENABLED))
	{
		CvGame& kGame = GC.getGame();
#ifdef GAME_UPDATE_TURN_TIMER_ONCE_PER_TURN
		float fGameTurnEnd = kGame.getPreviousTurnLen();
#else
		float fGameTurnEnd = static_cast<float>(kGame.getMaxTurnLen());
#endif
		float fTimeElapsed = kGame.getTimeElapsed();
		for (int jJ = MAX_MAJOR_CIVS; jJ < MAX_MINOR_CIVS; jJ++)
		{
			PlayerTypes eMinor = (PlayerTypes)jJ;
			GET_PLAYER(eMinor).GetMinorCivAI()->RecalculateMajorPriority();
			for (int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
			{
				if (kGame.getGameTurn() == GET_PLAYER((PlayerTypes)iI).getPriorityTurn(eMinor))
				{
					if (fTimeElapsed < GET_PLAYER((PlayerTypes)iI).getPriorityTime(eMinor))
					{
						GET_PLAYER((PlayerTypes)iI).setPriorityTime(eMinor, GET_PLAYER((PlayerTypes)iI).getPriorityTime(eMinor) - fTimeElapsed);
					}
					else
					{
						GET_PLAYER((PlayerTypes)iI).setPriorityTurn(eMinor, -1);
						GET_PLAYER((PlayerTypes)iI).setPriorityTime(eMinor, 0.f);
					}
				}
				if (kGame.getGameTurn() < GET_PLAYER((PlayerTypes)iI).getPriorityTurn(eMinor))
				{
					GET_PLAYER((PlayerTypes)iI).setPriorityTurn(eMinor, kGame.getGameTurn());
					GET_PLAYER((PlayerTypes)iI).setPriorityTime(eMinor, GET_PLAYER((PlayerTypes)iI).getPriorityTime(eMinor) + (fGameTurnEnd - fTimeElapsed));
				}
			}
		}
	}
#endif


	resetTurnTimer(true);


#ifdef AUI_WARNING_FIXES
	ICvUserInterface2& kEngineUserInterface = *GC.GetEngineUserInterface();
	kEngineUserInterface.setNoSelectionListCycle(false);
#else
	GC.GetEngineUserInterface()->setNoSelectionListCycle(false);
#endif

	gDLL->DoTurn();

	CvBarbarians::BeginTurn();

	doUpdateCacheOnTurn();

	DoUpdateCachedWorldReligionTechProgress();

	updateScore();

#ifndef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
#ifndef LEKMOD_v34
	m_kGameDeals.DoTurn();
#endif
#endif

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			GET_TEAM((TeamTypes)iI).doTurn();
		}
	}

	GC.getMap().doTurn();

#ifdef AUI_WARNING_FIXES
	kEngineUserInterface.doTurn();
#else
	GC.GetEngineUserInterface()->doTurn();
#endif

#ifdef AUI_GAME_FIX_MULTIPLAYER_BARBARIANS_SPAWN_AFTER_MOVING
#ifdef AUI_GAME_BETTER_HYBRID_MODE
	if (!isAnySimultaneousTurns())
#else
	if (!isOption(GAMEOPTION_DYNAMIC_TURNS) && !isOption(GAMEOPTION_SIMULTANEOUS_TURNS))
#endif
	{
#endif
		CvBarbarians::DoCamps();

		CvBarbarians::DoUnits();
#ifdef AUI_GAME_FIX_MULTIPLAYER_BARBARIANS_SPAWN_AFTER_MOVING
	}
#endif

#ifndef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
	GetGameReligions()->DoTurn();
	GetGameTrade()->DoTurn();
#endif
	GetGameLeagues()->DoTurn();
#ifndef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
	GetGameCulture()->DoTurn();
#endif
#ifdef MP_PLAYERS_VOTING_SYSTEM
	GetMPVotingSystem()->DoTurn();
#endif

#ifdef AUI_WARNING_FIXES
	kEngineUserInterface.setCanEndTurn(false);
	kEngineUserInterface.setHasMovedUnit(false);
#else
	GC.GetEngineUserInterface()->setCanEndTurn(false);
	GC.GetEngineUserInterface()->setHasMovedUnit(false);
#endif

	if (getAIAutoPlay() > 0)
	{
		changeAIAutoPlay(-1);

		if (getAIAutoPlay() == 0)
		{
			ReviveActivePlayer();
		}
	}
#ifdef GAME_ALLOW_ONLY_ONE_UNIT_MOVE_ON_TURN_LOADING
	if (GC.getGame().isOption("GAMEOPTION_FIRSTMOVE"))
	{
		setHasReceivedFirstMission(false);
	}
#endif
	incrementGameTurn();
	incrementElapsedGameTurns();
#ifdef AVOID_UNIT_SPLIT_MID_TURN
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& player = GET_PLAYER((PlayerTypes)iI);
		if (player.isAlive())
		{
			for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
			{
				FStaticVector<IDInfo, 50, true, c_eCiv5GameplayDLL, 0> oldUnitList;

				IDInfo* pUnitNode;
				CvUnit* pLoopUnit;

				oldUnitList.clear();

				pUnitNode = GC.getMap().plotByIndexUnchecked(iI)->headUnitNode();

				while (pUnitNode != NULL)
				{
					oldUnitList.push_back(*pUnitNode);
					pUnitNode = GC.getMap().plotByIndexUnchecked(iI)->nextUnitNode(pUnitNode);
				}
				int iUnitListSize = (int)oldUnitList.size();
				for (int iVectorLoop = 0; iVectorLoop < (int)iUnitListSize; ++iVectorLoop)
				{
					pLoopUnit = GetPlayerUnit(oldUnitList[iVectorLoop]);
					if (pLoopUnit != NULL)
					{
						if (!pLoopUnit->isDelayedDeath())
						{
							if (pLoopUnit->atPlot(*(GC.getMap().plotByIndexUnchecked(iI))))
							{
								if (!(pLoopUnit->isCargo()))
								{
									if (!(pLoopUnit->isInCombat()))
									{

										if (GC.getMap().plotByIndexUnchecked(iI)->getNumFriendlyUnitsOfType(pLoopUnit) >       GC.getPLOT_UNIT_LIMIT())
										{
											if (!pLoopUnit->jumpToNearestValidPlot())
											{
												pLoopUnit->kill(false);
												pLoopUnit = NULL;
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
#endif


#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE

	testVictory();
#endif

#ifdef AUI_GAME_BETTER_HYBRID_MODE
	constructTurnOrders();
#else
	if(isOption(GAMEOPTION_DYNAMIC_TURNS))
	{
		for(int teamIdx = 0; teamIdx < MAX_TEAMS; ++teamIdx)
		{
			CvTeam& curTeam = GET_TEAM((TeamTypes)teamIdx);
			curTeam.setDynamicTurnsSimultMode(!curTeam.isHuman() || !curTeam.isAtWarWithHumans());
		}
	}
#endif


#ifdef AUI_GAME_BETTER_HYBRID_MODE
	if (isAnySimultaneousTurns())
#else
	if(isOption(GAMEOPTION_DYNAMIC_TURNS) || isOption(GAMEOPTION_SIMULTANEOUS_TURNS))
#endif
	{


		shuffleArray(aiShuffle, MAX_PLAYERS, getJonRand());

		for(iI = 0; iI < MAX_PLAYERS; iI++)
		{
			iLoopPlayer = aiShuffle[iI];
			CvPlayer& player = GET_PLAYER((PlayerTypes)iLoopPlayer);


			if(player.isAlive() && !player.isHuman())
			{
				player.setTurnActive(true);
			}
		}
	}

#ifdef AUI_GAME_BETTER_HYBRID_MODE
	else
#else
	if(isSimultaneousTeamTurns())
	{
		for(iI = 0; iI < MAX_TEAMS; iI++)
		{
			CvTeam& kTeam = GET_TEAM((TeamTypes)iI);
			if(kTeam.isAlive() && !kTeam.isSimultaneousTurns()) 
			{
				kTeam.setTurnActive(true);
				break;
			}
		}
	}
	else if(!isOption(GAMEOPTION_SIMULTANEOUS_TURNS))
#endif
	{

		for(iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if(GET_PLAYER((PlayerTypes)iI).isAlive() 
#ifdef AUI_GAME_BETTER_HYBRID_MODE
				)
#else
				&& !GET_PLAYER((PlayerTypes)iI).isSimultaneousTurns())
#endif
			{
				if(isPbem() && GET_PLAYER((PlayerTypes)iI).isHuman())
				{
					if(iI == getActivePlayer())
					{

						CvPreGame::setGameType(GAME_HOTSEAT_MULTIPLAYER);
						GET_PLAYER((PlayerTypes)iI).setTurnActive(true);
					}
					else if(!getPbemTurnSent())
					{
						gDLL->sendPbemTurn((PlayerTypes)iI);
					}
				}
				else
				{
					GET_PLAYER((PlayerTypes)iI).setTurnActive(true);
					CvAssert(getNumGameTurnActive() == 1);
				}

				break;
			}
		}
	}

#ifndef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE

	testVictory();
#endif


	if(GET_PLAYER(getActivePlayer()).isAlive() && !IsStaticTutorialActive())
	{

		if(!isGameMultiPlayer())
		{
			int iTurnFrequency =        GC.getPROGRESS_POPUP_TURN_FREQUENCY();


			iTurnFrequency *= getGameSpeedInfo().getTrainPercent();
			iTurnFrequency /= 100;

			if(getElapsedGameTurns() % iTurnFrequency == 0)
			{

				CvPopupInfo kPopupInfo(BUTTONPOPUP_WHOS_WINNING);
#ifdef AUI_WARNING_FIXES
				kEngineUserInterface.AddPopup(kPopupInfo);
#else
				GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
#endif
			}
		}
	}
#ifdef LEKMOD_v34

	if (isHotSeat() || !isGameMultiPlayer())
	{
		m_kGameDeals.DoTurn();
	}
#endif
	LogGameState();
#ifndef AUTOSAVE_END_OF_TURN
	if(isNetworkMultiPlayer())
	{
		gDLL->AutoSave(false);
	}
#endif
	gDLL->PublishNewGameTurn(getGameTurn());
}

#ifdef AUI_GAME_BETTER_HYBRID_MODE
void CvGame::constructTurnOrders()
{
	m_iCurrentTurnOrderActive = 0;
	m_iLastTurnOrderID = 0;
	int iTeamIdx;

	if (isOption(GAMEOPTION_DYNAMIC_TURNS) || isOption(GAMEOPTION_SIMULTANEOUS_TURNS))
	{

		for (iTeamIdx = 0; iTeamIdx < MAX_TEAMS; ++iTeamIdx)
		{
			CvTeam& kCurTeam = GET_TEAM((TeamTypes)iTeamIdx);
			if (kCurTeam.isAlive())
				kCurTeam.setTurnOrder(0);
		}

		if (isOption(GAMEOPTION_DYNAMIC_TURNS))
		{
			bool abIsAliveHumanTeam[MAX_TEAMS] = {};
			for (iTeamIdx = 0; iTeamIdx < MAX_TEAMS; ++iTeamIdx)
			{
				CvTeam& kCurTeam = GET_TEAM((TeamTypes)iTeamIdx);
				if (kCurTeam.isAlive() && kCurTeam.isHuman())
				{
					abIsAliveHumanTeam[iTeamIdx] = true;
				}
			}

			for (iTeamIdx = 0; iTeamIdx < MAX_TEAMS - 1; ++iTeamIdx)
			{
				if (abIsAliveHumanTeam[iTeamIdx])
				{
					CvTeam& kCurTeam = GET_TEAM((TeamTypes)iTeamIdx);
					int iCurTeamOrder = kCurTeam.getTurnOrder();
					for (int iTargetTeamIdx = iTeamIdx + 1; iTargetTeamIdx < MAX_TEAMS; ++iTargetTeamIdx)
					{
						if (abIsAliveHumanTeam[iTargetTeamIdx])
						{
							CvTeam& kTargetTeam = GET_TEAM((TeamTypes)iTargetTeamIdx);
							int iTargetTeamOrder = kCurTeam.getTurnOrder();
							if (iTargetTeamOrder == iCurTeamOrder && kCurTeam.isAtWar((TeamTypes)iTeamIdx))
							{
								m_iLastTurnOrderID = iTargetTeamOrder + 1;
								kTargetTeam.setTurnOrder(m_iLastTurnOrderID);
							}
						}
					}
				}
			}
		}
	}

	else
	{
		m_iLastTurnOrderID = MAX_TEAMS - 1;
		for (iTeamIdx = 0; iTeamIdx < MAX_TEAMS; ++iTeamIdx)
		{
			CvTeam& kCurTeam = GET_TEAM((TeamTypes)iTeamIdx);
			kCurTeam.setTurnOrder(iTeamIdx);
		}
	}
#ifdef AUI_GAME_PLAYER_BASED_TURN_LENGTH
	calculateMaxTurnLengths();
#endif
}

#ifdef AUI_GAME_PLAYER_BASED_TURN_LENGTH
void CvGame::calculateMaxTurnLengths()
{
	m_aiMaxTurnLengths.clear();

#ifdef AUI_GAME_RELATIVE_TURN_TIMERS
	if (getPitbossTurnTime() != 0 && !isOption("GAMEOPTION_RELATIVE_TURN_TIMER"))
#else
	if (getPitbossTurnTime() != 0)
#endif
	{
		if (isPitboss())
		{
			for (int iI = 0; iI <= m_iLastTurnOrderID; ++iI)
			{
				m_aiMaxTurnLengths.push_back(getPitbossTurnTime() * 3600);
			}
		}
		else
		{
			for (int iI = 0; iI <= m_iLastTurnOrderID; ++iI)
			{
				m_aiMaxTurnLengths.push_back(getPitbossTurnTime());
			}
		}
	}
	else
	{
		const CvTurnTimerInfo& kTurnTimer = CvPreGame::turnTimerInfo();
		for (int iTurnOrder = 0; iTurnOrder <= m_iLastTurnOrderID; ++iTurnOrder)
		{
			int iMaxUnits = 0;
			int iMaxCities = 0;



			for (int iI = 0; iI < MAX_CIV_PLAYERS; ++iI)
			{
				const CvPlayer& kCurPlayer = GET_PLAYER((PlayerTypes)iI);
				if (kCurPlayer.isAlive() && kCurPlayer.isHuman() && kCurPlayer.getTurnOrder() == iTurnOrder)
				{
					if (kCurPlayer.getNumUnits() > iMaxUnits)
					{
						iMaxUnits = kCurPlayer.getNumUnits();
					}
					if (kCurPlayer.getNumCities() > iMaxCities)
					{
						iMaxCities = kCurPlayer.getNumCities();
					}
				}
			}


#ifdef AUI_GAME_RELATIVE_TURN_TIMERS
			int iBaseTurnTime = kTurnTimer.getBaseTime();
			int iExtraTurnTime = (kTurnTimer.getCityResource() * iMaxCities) + (kTurnTimer.getUnitResource() * iMaxUnits);
			if (getPitbossTurnTime() != 0 && isOption("GAMEOPTION_RELATIVE_TURN_TIMER"))
			{
				iExtraTurnTime *= getPitbossTurnTime();
				iExtraTurnTime /= 100;
			}
			iBaseTurnTime += iExtraTurnTime;
#else
			int iBaseTurnTime = kTurnTimer.getBaseTime() + (kTurnTimer.getCityResource() * iMaxCities) + (kTurnTimer.getUnitResource() * iMaxUnits);
#endif

			m_aiMaxTurnLengths.push_back(iBaseTurnTime);
		}
	}
}
#endif
#endif


ImprovementTypes CvGame::GetBarbarianCampImprovementType()
{
	return (ImprovementTypes)GC.getBARBARIAN_CAMP_IMPROVEMENT();
}



void CvGame::SetBarbarianReleaseTurn(int iValue)
{
	m_iEarliestBarbarianReleaseTurn = iValue;
}




UnitTypes CvGame::GetRandomSpawnUnitType(PlayerTypes ePlayer, bool bIncludeUUs, bool bIncludeRanged)
{
	UnitTypes eBestUnit = NO_UNIT;
	int iBestValue = 0;
	int iValue = 0;
	int iBonusValue;


#ifdef AUI_WARNING_FIXES
	for (uint iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
#else
	for(int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
#endif
	{
		bool bValid = false;
		const UnitTypes eLoopUnit = static_cast<UnitTypes>(iUnitLoop);
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eLoopUnit);

		if(pkUnitInfo != NULL)
		{
			iBonusValue = 0;

			CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo((UnitClassTypes)pkUnitInfo->GetUnitClassType());
			if(pkUnitClassInfo)
			{

				if(eLoopUnit == pkUnitClassInfo->getDefaultUnitIndex())
					iBonusValue += 700;
				else if(!bIncludeUUs)
					continue;
			}

			bValid = (pkUnitInfo->GetCombat() > 0);
			if(bValid)
			{

				if(pkUnitInfo->GetRange() == 0)
				{
#ifdef AUI_WARNING_FIXES
					for (uint iLoop = 0; iLoop < GC.getNumPromotionInfos(); iLoop++)
#else
					for(int iLoop = 0; iLoop < GC.getNumPromotionInfos(); iLoop++)
#endif
					{
						const PromotionTypes ePromotion = static_cast<PromotionTypes>(iLoop);
						CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
						if(pkPromotionInfo)
						{
							if(pkUnitInfo->GetFreePromotions(iLoop))
							{
								if(pkPromotionInfo->IsOnlyDefensive())
								{
									bValid = false;
									break;
								}
							}
						}
					}
				}
			}

			if(!bValid)
				continue;


			if(pkUnitInfo->GetDefaultUnitAIType() == UNITAI_EXPLORE)
				continue;


			if(!bIncludeRanged && pkUnitInfo->GetRangedCombat() > 0)
				continue;


			if(pkUnitInfo->GetDomainType() != DOMAIN_LAND)
				continue;


			if(!GET_PLAYER(ePlayer).canTrain(eLoopUnit, false, false, false,                             true))
				continue;


			iValue = (1 + GC.getGame().getJonRandNum(1000, "Minor Civ Unit spawn Selection"));
			iValue += iBonusValue;

			if(iValue > iBestValue)
			{
				eBestUnit = eLoopUnit;
				iBestValue = iValue;
			}
		}
	}

	return eBestUnit;
}



UnitTypes CvGame::GetCompetitiveSpawnUnitType(PlayerTypes ePlayer, bool bIncludeUUs, bool bIncludeRanged)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	CvWeightedVector<UnitTypes, SAFE_ESTIMATE_NUM_UNITS, true> veUnitRankings;


#ifdef AUI_WARNING_FIXES
	for (uint iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
#else
	for(int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
#endif
	{
		const UnitTypes eLoopUnit = (UnitTypes) iUnitLoop;
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eLoopUnit);
		if(pkUnitInfo == NULL)
		{
			continue;
		}

		bool bValid = (pkUnitInfo->GetCombat() > 0);


		if(bValid && pkUnitInfo->GetRange() == 0)
		{
#ifdef AUI_WARNING_FIXES
			for (uint iPromotionLoop = 0; iPromotionLoop < GC.getNumPromotionInfos(); iPromotionLoop++)
#else
			for(int iPromotionLoop = 0; iPromotionLoop < GC.getNumPromotionInfos(); iPromotionLoop++)
#endif
			{
				const PromotionTypes ePromotion = (PromotionTypes) iPromotionLoop;
				CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkPromotionInfo)
				{
					if(pkUnitInfo->GetFreePromotions(iPromotionLoop))
					{
						if(pkPromotionInfo->IsOnlyDefensive())
						{
							bValid = false;
							break;
						}
					}
				}
			}
		}
		if(!bValid)
			continue;

		UnitClassTypes eLoopUnitClass = (UnitClassTypes) pkUnitInfo->GetUnitClassType();
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eLoopUnitClass);

		if(pkUnitClassInfo == NULL)
		{
			CvAssertMsg(false, "UnitClassInfo is NULL. Please send Anton your save file and version.");
			continue;
		}


		if(eLoopUnit == pkUnitClassInfo->getDefaultUnitIndex())
		{

		}

		else
		{
			if(!bIncludeUUs)
			{
				continue;
			}
			else
			{

				for(int iMajorLoop = 0; iMajorLoop < MAX_PLAYERS; iMajorLoop++)
				{
					PlayerTypes eMajorLoop = (PlayerTypes) iMajorLoop;
					if(GET_PLAYER(eMajorLoop).isAlive())
					{
						UnitTypes eUniqueUnitInGame = (UnitTypes) GET_PLAYER(eMajorLoop).getCivilizationInfo().getCivilizationUnits(eLoopUnitClass);
						if(eLoopUnit == eUniqueUnitInGame)
						{
							bValid = false;
							break;
						}
					}
				}
				if(!bValid)
					continue;
			}
		}


		if(pkUnitInfo->GetDefaultUnitAIType() == UNITAI_EXPLORE)
			continue;


		if(!bIncludeRanged && pkUnitInfo->GetRangedCombat() > 0)
			continue;


		if(pkUnitInfo->GetDomainType() != DOMAIN_LAND)
			continue;


		if(!GET_PLAYER(ePlayer).canTrain(eLoopUnit, false, false, false,                             true))
			continue;

		veUnitRankings.push_back(eLoopUnit, pkUnitInfo->GetPower());
	}


	veUnitRankings.SortItems();
	int iNumChoices = GC.getUNIT_SPAWN_NUM_CHOICES();
	RandomNumberDelegate randFn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
	UnitTypes eChosenUnit = veUnitRankings.ChooseFromTopChoices(iNumChoices, &randFn, "Choosing competitive unit from top choices");

	return eChosenUnit;
}


UnitTypes CvGame::GetRandomUniqueUnitType(bool bIncludeCivsInGame, bool bIncludeStartEra, bool bIncludeOldEras, bool bIncludeRanged)
{

	std::set<UnitTypes> setUniquesAlreadyAssigned;
	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		PlayerTypes eMinorLoop = (PlayerTypes) iMinorLoop;
		CvPlayer* pMinorLoop = &GET_PLAYER(eMinorLoop);
		if(pMinorLoop && pMinorLoop->isEverAlive())
		{
			UnitTypes eUniqueUnit = pMinorLoop->GetMinorCivAI()->GetUniqueUnit();
			if(eUniqueUnit != NO_UNIT)
			{
				setUniquesAlreadyAssigned.insert(eUniqueUnit);
			}
		}
	}
	
	CvWeightedVector<UnitTypes, SAFE_ESTIMATE_NUM_UNITS, true> veUnitRankings;


#ifdef AUI_WARNING_FIXES
	for (uint iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
#else
	for(int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
#endif
	{
		const UnitTypes eLoopUnit = (UnitTypes) iUnitLoop;
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eLoopUnit);
		if(pkUnitInfo == NULL)
		{
			continue;
		}

		bool bValid = (pkUnitInfo->GetCombat() > 0);


		if(bValid && pkUnitInfo->GetRange() == 0)
		{
#ifdef AUI_WARNING_FIXES
			for (uint iPromotionLoop = 0; iPromotionLoop < GC.getNumPromotionInfos(); iPromotionLoop++)
#else
			for(int iPromotionLoop = 0; iPromotionLoop < GC.getNumPromotionInfos(); iPromotionLoop++)
#endif
			{
				const PromotionTypes ePromotion = (PromotionTypes) iPromotionLoop;
				CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkPromotionInfo)
				{
					if(pkUnitInfo->GetFreePromotions(iPromotionLoop))
					{
						if(pkPromotionInfo->IsOnlyDefensive())
						{
							bValid = false;
							break;
						}
					}
				}
			}
		}
		if(!bValid)
			continue;

		UnitClassTypes eLoopUnitClass = (UnitClassTypes) pkUnitInfo->GetUnitClassType();
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eLoopUnitClass);

		if(pkUnitClassInfo == NULL)
		{
			CvAssertMsg(false, "UnitClassInfo is NULL. Please send Anton your save file and version.");
			continue;
		}


		if(eLoopUnit == pkUnitClassInfo->getDefaultUnitIndex())
			continue;


		if (!bIncludeCivsInGame)
		{
			for(int iMajorLoop = 0; iMajorLoop < MAX_PLAYERS; iMajorLoop++)
			{
				PlayerTypes eMajorLoop = (PlayerTypes) iMajorLoop;
				if(GET_PLAYER(eMajorLoop).isAlive())
				{
					UnitTypes eUniqueUnitInGame = (UnitTypes) GET_PLAYER(eMajorLoop).getCivilizationInfo().getCivilizationUnits(eLoopUnitClass);
					if(eLoopUnit == eUniqueUnitInGame)
					{
						bValid = false;
						break;
					}
				}
			}
		}
		if(!bValid)
			continue;


		if(pkUnitInfo->GetDefaultUnitAIType() == UNITAI_EXPLORE)
			continue;


		if(!bIncludeRanged && pkUnitInfo->GetRangedCombat() > 0)
			continue;


		if(pkUnitInfo->GetDomainType() != DOMAIN_LAND)
			continue;


		TechTypes ePrereqTech = (TechTypes) pkUnitInfo->GetPrereqAndTech();
		EraTypes ePrereqEra = NO_ERA;
		if (ePrereqTech != NO_TECH)
		{
			CvTechEntry* pkTechInfo = GC.getTechInfo(ePrereqTech);
			CvAssertMsg(pkTechInfo, "Tech info not found when picking unique unit for minor civ. Please send Anton your save file and version!");
			if (pkTechInfo)
			{
				ePrereqEra = (EraTypes) pkTechInfo->GetEra();
			}
		}

		if (ePrereqEra == getStartEra())
		{
			if (!bIncludeStartEra)
				continue;
		}
		else if (ePrereqEra < getStartEra())
		{
			if (!bIncludeOldEras)
				continue;
		}


		if (setUniquesAlreadyAssigned.count(eLoopUnit) > 0)
			continue;

		veUnitRankings.push_back(eLoopUnit, 1);
	}

	UnitTypes eChosenUnit = NO_UNIT;

	if (veUnitRankings.size() > 0)
	{
		veUnitRankings.SortItems();
		RandomNumberDelegate randFn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
		eChosenUnit = veUnitRankings.ChooseByWeight(&randFn, "Choosing random unique unit for minor civ");
	}

	return eChosenUnit;
}


void CvGame::updateWar()
{
	int iI, iJ;

	if(isOption(GAMEOPTION_ALWAYS_WAR))
	{
		for(iI = 0; iI < MAX_TEAMS; iI++)
		{
			CvTeam& teamI = GET_TEAM((TeamTypes)iI);
			if(teamI.isHuman())
			{
				if(teamI.isAlive())
				{
					for(iJ = 0; iJ < MAX_TEAMS; iJ++)
					{
						CvTeam& teamJ = GET_TEAM((TeamTypes)iJ);
						if(!(teamJ.isHuman()))
						{
							if(teamJ.isAlive())
							{
								if(iI != iJ)
								{
									if(teamI.isHasMet((TeamTypes)iJ))
									{
										if(!atWar(((TeamTypes)iI), ((TeamTypes)iJ)))
										{
											teamI.declareWar(((TeamTypes)iJ));
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


void CvGame::updateMoves()
{
	UnitHandle pLoopUnit;
	pLoopUnit.ignoreDestruction(true);

	int iLoop;
	int iI;

	static bool processPlayerAutoMoves = false;





	FStaticVector<PlayerTypes, MAX_PLAYERS, true, c_eCiv5GameplayDLL, 0> playersToProcess;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& player = GET_PLAYER((PlayerTypes)iI);
		if(player.isAlive() && player.isTurnActive() && !player.isHuman())
		{
			playersToProcess.push_back(static_cast<PlayerTypes>(iI));
			processPlayerAutoMoves = false;

			break;
		}
	}


#ifndef AUI_GAME_BETTER_HYBRID_MODE
	int currentTurn = getGameTurn();
	bool activatePlayers = playersToProcess.empty() && m_lastTurnAICivsProcessed != currentTurn;
#endif

	if(playersToProcess.empty())
	{
		SetLastTurnAICivsProcessed();
		if(gDLL->allAICivsProcessedThisTurn())
		{
			PlayerTypes eActivePlayer = getActivePlayer();
			if(eActivePlayer != NO_PLAYER && CvPreGame::slotStatus(eActivePlayer) == SS_OBSERVER)
			{


				CvPlayer& kActivePlayer = GET_PLAYER(eActivePlayer);
				kActivePlayer.GetPlayerAchievements().EndTurn();
				gDLL->sendTurnComplete();
				CvAchievementUnlocker::EndTurn();
			}

			if(!processPlayerAutoMoves)
			{
#ifdef AUI_GAME_BETTER_HYBRID_MODE
				if (isAnySimultaneousTurns())
				{

					bool readyForAutoMoves = true;
					for (iI = 0; iI < MAX_PLAYERS; iI++)
					{
						CvPlayer& player = GET_PLAYER((PlayerTypes)iI);
						if (player.isHuman() && player.getTurnOrder() == m_iCurrentTurnOrderActive && !player.isObserver() && !player.isAutoMoves())
#else
				if(!GC.getGame().isOption(GAMEOPTION_DYNAMIC_TURNS) && GC.getGame().isOption(GAMEOPTION_SIMULTANEOUS_TURNS))
				{

					bool readyForAutoMoves = true;
					for(iI = 0; iI < MAX_PLAYERS; iI++)
					{
						CvPlayer& player = GET_PLAYER((PlayerTypes)iI);
						if(player.isHuman() && !player.isObserver() && !player.isAutoMoves())
#endif
							readyForAutoMoves = false;
					}
					processPlayerAutoMoves = readyForAutoMoves;
				}
				else
					processPlayerAutoMoves = true;
			}

			for(iI = 0; iI < MAX_PLAYERS; iI++)
			{
				CvPlayer& player = GET_PLAYER((PlayerTypes)iI);

				player.checkInitialTurnAIProcessed();
				if(player.isTurnActive() && player.isHuman())
				{
					playersToProcess.push_back(static_cast<PlayerTypes>(iI));
				}
			}
		}
	}

#ifdef AUI_GAME_BETTER_HYBRID_MODE

	bool activatePlayers = playersToProcess.empty() && m_lastTurnAICivsProcessed != getGameTurn();
#endif

	FStaticVector<PlayerTypes, MAX_PLAYERS, true, c_eCiv5GameplayDLL, 0>::const_iterator i;

	for(i = playersToProcess.begin(); i != playersToProcess.end(); ++i)
	{
		GC.getPathFinder().ForceReset();
		CvPlayer& player = GET_PLAYER((PlayerTypes)*i);
		int iReadyUnitsBeforeMoves = player.GetCountReadyUnits();

		if(player.isAlive())
		{
			bool needsAIUpdate = player.hasUnitsThatNeedAIUpdate();
			if(player.isTurnActive() || needsAIUpdate)
			{
				if(!(player.isAutoMoves()) || needsAIUpdate)
				{
					if(needsAIUpdate || !player.isHuman())
					{
						player.AI_unitUpdate();

						NET_MESSAGE_DEBUG_OSTR_ALWAYS("UpdateMoves() : player.AI_unitUpdate() called for player " << player.GetID() << " " << player.getName()); 
					}

					int iReadyUnitsNow = player.GetCountReadyUnits();


					if(iReadyUnitsNow < iReadyUnitsBeforeMoves)
					{
						player.SetLastSliceMoved(m_iTurnSlice);
					}

					if(!(player.isHuman()) && !(player.hasBusyUnitOrCity()))
					{
						if(iReadyUnitsNow == 0)
						{
							player.setAutoMoves(true);
							NET_MESSAGE_DEBUG_OSTR_ALWAYS("UpdateMoves() : player.setAutoMoves(true) called for player " << player.GetID() << " " << player.getName()); 
						}
						else
						{
							const CvUnit* pReadyUnit = player.GetFirstReadyUnit();
							if(pReadyUnit && !player.GetTacticalAI()->IsInQueuedAttack(pReadyUnit))
							{
								int iWaitTime = 100;
								if(!isNetworkMultiPlayer())
								{
									iWaitTime = 10;
								}
								if(m_iTurnSlice - player.GetLastSliceMoved() > iWaitTime)
								{
									CvUnitEntry* entry = GC.getUnitInfo(pReadyUnit->getUnitType());
									if(entry)
									{
										CvString strTemp = entry->GetDescription();
										CvString szAssertMessage;
										szAssertMessage.Format(
										    "GAME HANG - Please show Ed and send save. Stuck units will have their turn ended so game can advance. [DETAILS: Player %i %s. First stuck unit is %s at (%d, %d)]",
										    player.GetID(), player.getName(), strTemp.GetCString(), pReadyUnit->getX(), pReadyUnit->getY());
										CvAssertMsg(false, szAssertMessage);
										NET_MESSAGE_DEBUG_OSTR_ALWAYS(szAssertMessage);
									}
									player.EndTurnsForReadyUnits();
								}
							}
						}
					}
				}



				if(player.isAutoMoves() && (!player.isHuman() || processPlayerAutoMoves))
				{
					bool bRepeatAutomoves = false;
					int iRepeatPassCount = 2;
					do
					{
						for(pLoopUnit = player.firstUnit(&iLoop); pLoopUnit; pLoopUnit = player.nextUnit(&iLoop))
						{
							CvString tempString;
							getMissionAIString(tempString, pLoopUnit->GetMissionAIType());
							NET_MESSAGE_DEBUG_OSTR_ALWAYS("UpdateMoves() : player " << player.GetID() << " " << player.getName()
																							<< " running AutoMission (" << tempString << ") on " 
																							<< pLoopUnit->getName() << " id=" << pLoopUnit->GetID());

							pLoopUnit->AutoMission();


							if(player.isHuman() && CvUnitMission::HasCompletedMoveMission(pLoopUnit) && pLoopUnit->canMove() && !pLoopUnit->IsDoingPartialMove() && !pLoopUnit->IsAutomated())
							{
								if(player.isEndTurn())
								{
									bRepeatAutomoves = true;
									NET_MESSAGE_DEBUG_OSTR_ALWAYS("UpdateMoves() : player " << player.GetID() << " " << player.getName()
																									<< " AutoMission did not use up all movement points for " 
																									<< pLoopUnit->getName() << " id=" << pLoopUnit->GetID());

									if(player.isLocalPlayer() && gDLL->sendTurnUnready())
										player.setEndTurn(false);
								}
							}
















							if(!isNetworkMultiPlayer() && !isOption(GAMEOPTION_END_TURN_TIMER_ENABLED))
							{
								if(pLoopUnit && player.isEndTurn() && pLoopUnit->GetLengthMissionQueue() == 0 && pLoopUnit->GetActivityType() == ACTIVITY_AWAKE && pLoopUnit->canMove() && !pLoopUnit->IsDoingPartialMove() && !pLoopUnit->IsAutomated())
								{
									if(IsForceEndingTurn())
									{
										SetForceEndingTurn(false);
									}
									else
									{
										CvAssertMsg(GC.getGame().getActivePlayer() == player.GetID(), "slewis - We should not need to resolve ambiguous end turns for the AI or remotely.");
										if(player.isLocalPlayer() && gDLL->sendTurnUnready())
											player.setEndTurn(false);
									}
								}
							}
						}
					}
					while(bRepeatAutomoves && iRepeatPassCount--);




					if(!player.isHuman())
					{
						for(pLoopUnit = player.firstUnit(&iLoop); pLoopUnit; pLoopUnit = player.nextUnit(&iLoop))
						{
							if(pLoopUnit)
							{
								bool bMoveMe  = false;
								int iNumTurnsFortified = pLoopUnit->getFortifyTurns();
								IDInfo* pUnitNodeInner;
								pUnitNodeInner = pLoopUnit->plot()->headUnitNode();
								while(pUnitNodeInner != NULL && !bMoveMe)
								{
									CvUnit* pLoopUnitInner = ::getUnit(*pUnitNodeInner);
									if(pLoopUnitInner && pLoopUnit != pLoopUnitInner)
									{
										if(pLoopUnit->getOwner() == pLoopUnitInner->getOwner())
										{
											if(pLoopUnit->AreUnitsOfSameType(*pLoopUnitInner) && pLoopUnit->plot()->getNumFriendlyUnitsOfType(pLoopUnit.pointer()) > GC.getPLOT_UNIT_LIMIT())
											{
												if(pLoopUnitInner->getFortifyTurns() >= iNumTurnsFortified)
												{
													bMoveMe = true;
												}
											}
										}
									}
									pUnitNodeInner = pLoopUnit->plot()->nextUnitNode(pUnitNodeInner);
								}
								if(bMoveMe)
								{
									if (!pLoopUnit->jumpToNearestValidPlotWithinRange(1))
										pLoopUnit->kill(false);
									break;
								}
								pLoopUnit->doDelayedDeath();
							}
						}
					}


					if(player.isEndTurn() || !player.isHuman())
					{
						player.setProcessedAutoMoves(true);
					}
				}


#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
				if (!player.isEndTurn() && (!player.isHuman() || gDLL->HasReceivedTurnComplete(player.GetID())))
#else
				if(!player.isEndTurn() && gDLL->HasReceivedTurnComplete(player.GetID()) && player.isHuman()                                                                                                      )
#endif
				{
					if(!player.hasBusyUnitOrCity())
					{
						player.setEndTurn(true);
						if(player.isEndTurn())
						{
							NET_MESSAGE_DEBUG_OSTR_ALWAYS("UpdateMoves() : player.setEndTurn(true) called for player " << player.GetID() << " " << player.getName());
						}
					}
					else
					{
						if(!player.hasBusyUnitUpdatesRemaining())
						{
							NET_MESSAGE_DEBUG_OSTR_ALWAYS("Received turn complete for player "  << player.GetID() << " " << player.getName() << " but there is a busy unit. Forcing the turn to advance");
							player.setEndTurn(true);
						}
					}
				}
			}
		}
	}

	if(activatePlayers)
	{
#ifdef AUI_GAME_BETTER_HYBRID_MODE
		if (isAnySimultaneousTurns())
#else
		if (isOption(GAMEOPTION_DYNAMIC_TURNS) || isOption(GAMEOPTION_SIMULTANEOUS_TURNS))
#endif
		{
#ifdef AUI_GAME_FIX_MULTIPLAYER_BARBARIANS_SPAWN_AFTER_MOVING

			if (GC.getGame().getElapsedGameTurns() > 0)
			{
			CvBarbarians::DoCamps();
			CvBarbarians::DoUnits();
			}
#endif

#ifdef NQM_GAME_RANDOMIZE_TURN_ACTIVATION_ORDER_IN_SIMULTANEOUS
			int aiShuffle[MAX_PLAYERS];
			if (GC.getGame().isOption("GAMEOPTION_SIMULTANEOUS_PLAYER_TURN_ACTIVATION_ORDER_RANDOMIZED"))
			{
				shuffleArray(aiShuffle, MAX_PLAYERS, getJonRand());
			}
			else
			{
				for (iI = 0; iI < MAX_PLAYERS; iI++)
				{
					aiShuffle[iI] = iI;
				}
			}
#if defined(LEKMOD_WC_RESPECT_ACTIVATION_ORDER)
			StoreTurnActivationOrder(aiShuffle);
#endif

			for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
			{
				iI = aiShuffle[iJ];
#else
			for(iI = 0; iI < MAX_PLAYERS; iI++)
			{
#endif
				CvPlayer& player = GET_PLAYER((PlayerTypes)iI);
#ifdef AUI_GAME_BETTER_HYBRID_MODE
				if (!player.isTurnActive() && player.isHuman() && player.isAlive() && (player.getTurnOrder() == m_iCurrentTurnOrderActive))
#else
				if(!player.isTurnActive() && player.isHuman() && player.isAlive() && player.isSimultaneousTurns())
#endif
				{
					player.setTurnActive(true);
				}
			}
#ifndef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
#ifdef LEKMOD_v34
			m_kGameDeals.DoTurn();
#endif
#endif
		}
	}
}


void CvGame::updateTimers()
{
	int iI;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive())
		{
			kPlayer.updateTimers();
		}
	}

	if(isHotSeat())
	{

		PlayerTypes eActivePlayer = getActivePlayer();
		if(eActivePlayer != NO_PLAYER)
		{
			CvPlayer& kActivePlayer = GET_PLAYER(eActivePlayer);
			if(kActivePlayer.isAlive() && kActivePlayer.isHuman() && kActivePlayer.isTurnActive())
				CvDiplomacyRequests::DoAIDiplomacy(eActivePlayer);
		}
	}
}


void CvGame::UpdatePlayers()
{
	int numActive = 0;
	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)i);

		if(kPlayer.isAlive() && kPlayer.isTurnActive())
		{
			kPlayer.UpdateNotifications();
			++numActive;
		}
	}
	CvAssert(numActive == getNumGameTurnActive());
}


void CvGame::testAlive()
{
	int iI;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		GET_PLAYER((PlayerTypes)iI).verifyAlive();
	}
}

bool CvGame::testVictory(VictoryTypes eVictory, TeamTypes eTeam, bool* pbEndScore) const
{
	CvAssert(eVictory >= 0 && eVictory < GC.getNumVictoryInfos());
	CvAssert(eTeam >=0 && eTeam < MAX_CIV_TEAMS);
	CvAssert(GET_TEAM(eTeam).isAlive());

	CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eVictory);
	if(pkVictoryInfo == NULL)
	{
		return false;
	}


	if(GET_TEAM(eTeam).isVictoryAchieved(eVictory))
	{
		return false;
	}

	bool bValid = isVictoryValid(eVictory);
	if(pbEndScore)
	{
		*pbEndScore = false;
	}


	if(getElapsedGameTurns() <=        GC.getMIN_GAME_TURNS_ELAPSED_TO_TEST_VICTORY() && (GC.getMap().getWorldSize() != WORLDSIZE_DEBUG))
	{
		return false;
	}


	if(bValid)
	{
		if(pkVictoryInfo->isEndScore())
		{
			if(pbEndScore)
			{
				*pbEndScore = true;
			}

			if(getMaxTurns() == 0)
			{
				bValid = false;
			}
			else if(getElapsedGameTurns() < getMaxTurns())
			{
				bValid = false;
			}
			else
			{
				bool bFound = false;

				for(int iK = 0; iK < MAX_CIV_TEAMS; iK++)
				{
					if(GET_TEAM((TeamTypes)iK).isAlive())
					{
						if(iK != eTeam)
						{
							if(getTeamScore((TeamTypes)iK) >= getTeamScore(eTeam))
							{
								bFound = true;
								break;
							}
						}
					}
				}

				if(bFound)
				{
					bValid = false;
				}
			}
		}
	}


	if(bValid)
	{
		if(pkVictoryInfo->isTargetScore())
		{
			if(getTargetScore() == 0)
			{
				bValid = false;
			}
			else if(getTeamScore(eTeam) < getTargetScore())
			{
				bValid = false;
			}
			else
			{
				bool bFound = false;

				for(int iK = 0; iK < MAX_CIV_TEAMS; iK++)
				{
					if(GET_TEAM((TeamTypes)iK).isAlive())
					{
						if(iK != eTeam)
						{
							if(getTeamScore((TeamTypes)iK) >= getTeamScore(eTeam))
							{
								bFound = true;
								break;
							}
						}
					}
				}

				if(bFound)
				{
					bValid = false;
				}
			}
		}
	}


	if(bValid)
	{
		if(pkVictoryInfo->isConquest())
		{
			if(GET_TEAM(eTeam).getNumCities() == 0)
			{
				bValid = false;
			}
			else
			{
				bool bFound = false;

				for(int iK = 0; iK < MAX_CIV_TEAMS; iK++)
				{
					if(GET_TEAM((TeamTypes)iK).isAlive())
					{
						if(iK != eTeam)
						{
							if(GET_TEAM((TeamTypes)iK).getNumCities() > 0)
							{
								bFound = true;
								break;
							}
						}
					}
				}

				if(bFound)
				{
					bValid = false;
				}
			}
		}
	}


	if(bValid)
	{
		if(pkVictoryInfo->isDiploVote())
		{
			bValid = false;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
			{
				PlayerTypes ePlayerLoop = (PlayerTypes) iPlayerLoop;
				if (GET_PLAYER(ePlayerLoop).getTeam() == eTeam)
				{
					if (m_pGameLeagues->GetDiplomaticVictor() == ePlayerLoop)
					{
						bValid = true;
					}
				}
			}
		}
	}


	if(bValid)
	{
		if(pkVictoryInfo->isInfluential())
		{

			bValid = false;
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
			{
				CvPlayer &kPlayer = GET_PLAYER((PlayerTypes)iPlayerLoop);
				if (kPlayer.isAlive())
				{
					if (kPlayer.getTeam() == eTeam)
					{
						if (kPlayer.GetCulture()->GetNumCivsInfluentialOn() >= m_pGameCulture->GetNumCivsInfluentialForWin())
						{

							bValid = true;
							break;
						}
					}
				}
			}		
		}
	}


	if(bValid)
	{
		if(pkVictoryInfo->IsReligionInAllCities())
		{
			bool bReligionInAllCities = true;

			CvCity* pLoopCity;
			int iLoop;

			PlayerTypes eLoopPlayer;


			for(int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				if(GET_PLAYER(eLoopPlayer).isAlive())
				{
					if(GET_PLAYER(eLoopPlayer).getTeam() == eTeam)
					{
						for(pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iLoop))
						{

							if(pLoopCity->GetPlayersReligion() != eLoopPlayer)
							{
								bReligionInAllCities = false;
								break;
							}
						}

						if(!bReligionInAllCities)
						{
							break;
						}
					}
				}
			}

			if(!bReligionInAllCities)
			{
				bValid = false;
			}
		}
	}


	if(bValid)
	{
		if(pkVictoryInfo->IsFindAllNaturalWonders())
		{
			int iWorldNumNaturalWonders = GC.getMap().GetNumNaturalWonders();

			if(iWorldNumNaturalWonders == 0 || GET_TEAM(eTeam).GetNumNaturalWondersDiscovered() < iWorldNumNaturalWonders)
			{
				bValid = false;
			}
		}
	}


	if(bValid)
	{
		if(getAdjustedPopulationPercent(eVictory) > 0)
		{
			if(100 * GET_TEAM(eTeam).getTotalPopulation() < getTotalPopulation() * getAdjustedPopulationPercent(eVictory))
			{
				bValid = false;
			}
		}
	}


	if(bValid)
	{
		if(getAdjustedLandPercent(eVictory) > 0)
		{
#ifdef AUI_WARNING_FIXES
			if (100 * GET_TEAM(eTeam).getTotalLand() < (int)GC.getMap().getLandPlots() * getAdjustedLandPercent(eVictory))
#else
			if(100 * GET_TEAM(eTeam).getTotalLand() < GC.getMap().getLandPlots() * getAdjustedLandPercent(eVictory))
#endif
			{
				bValid = false;
			}
		}
	}


	if(bValid)
	{
#ifdef AUI_WARNING_FIXES
		for (uint iK = 0; iK < GC.getNumBuildingClassInfos(); iK++)
#else
		for(int iK = 0; iK < GC.getNumBuildingClassInfos(); iK++)
#endif
		{
			BuildingClassTypes eBuildingClass = static_cast<BuildingClassTypes>(iK);
			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
			if(pkBuildingClassInfo)
			{
				if(pkBuildingClassInfo->getVictoryThreshold(eVictory) > GET_TEAM(eTeam).getBuildingClassCount(eBuildingClass))
				{
					bValid = false;
					break;
				}
			}

		}
	}


	if(bValid)
	{
#ifdef AUI_WARNING_FIXES
		for (uint iK = 0; iK < GC.getNumProjectInfos(); iK++)
#else
		for(int iK = 0; iK < GC.getNumProjectInfos(); iK++)
#endif
		{
			const ProjectTypes eProject = static_cast<ProjectTypes>(iK);
			CvProjectEntry* pkProjectInfo = GC.getProjectInfo(eProject);
			if(pkProjectInfo)
			{
				if(pkProjectInfo->GetVictoryMinThreshold(eVictory) > GET_TEAM(eTeam).getProjectCount(eProject))
				{
					bValid = false;
					break;
				}
			}
		}
	}

	return bValid;
}


void CvGame::testVictory()
{
	bool bEndScore = false;


	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "GameCoreTestVictory", args.get(), bResult);
	}

	if(getVictory() != NO_VICTORY)
	{
		return;
	}

	if(getGameState() == GAMESTATE_EXTENDED)
	{
		return;
	}

	updateScore();

	bool bEndGame = false;

	std::vector<std::vector<int> > aaiGameWinners;
	int iTeamLoop = 0;
#ifdef AUI_WARNING_FIXES
	uint iVictoryLoop = 0;
#else
	int iVictoryLoop = 0;
#endif

	int iNumCompetitionWinners = 0;
	for(iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
	{
		m_aiTeamCompetitionWinnersScratchPad[iTeamLoop] = 0;
	}


	for(iVictoryLoop = 0; iVictoryLoop < GC.getNumVictoryInfos(); iVictoryLoop++)
	{
		const VictoryTypes eVictory = static_cast<VictoryTypes>(iVictoryLoop);
		CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eVictory);
		if(pkVictoryInfo == NULL)
			continue;

		for(iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
		{
			CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iTeamLoop);
			if(kLoopTeam.isAlive())
			{
				if(!(kLoopTeam.isMinorCiv()))
				{
					if(testVictory(eVictory, (TeamTypes)iTeamLoop, &bEndScore))
					{

						if(pkVictoryInfo->IsWinsGame())
						{
							std::vector<int> aWinner;
							aWinner.push_back(iTeamLoop);
							aWinner.push_back(iVictoryLoop);
							aaiGameWinners.push_back(aWinner);

							bEndGame = true;
						}

						else
						{
							m_aiTeamCompetitionWinnersScratchPad[iNumCompetitionWinners] = iTeamLoop;
							iNumCompetitionWinners++;
						}
					}
				}
			}
		}


		if(iNumCompetitionWinners > 0)
		{
			int iRand;

			do
			{
				if(isVictoryAvailable(eVictory))
				{
					iRand = GC.getGame().getJonRandNum(iNumCompetitionWinners, "Victory Competition tiebreaker");
					iTeamLoop = m_aiTeamCompetitionWinnersScratchPad[iRand];

					DoPlaceTeamInVictoryCompetition(eVictory, (TeamTypes) iTeamLoop);

					iNumCompetitionWinners--;
				}
				else
				{
					iNumCompetitionWinners = 0;
				}
			}
			while(iNumCompetitionWinners > 0);
		}
	}


	if(!bEndGame)
	{
		bEndGame = IsEndGameTechResearched();

		if(bEndGame)
		{
			VictoryTypes eScoreVictory = NO_VICTORY;
			for(iVictoryLoop = 0; iVictoryLoop < GC.getNumVictoryInfos(); iVictoryLoop++)
			{
				VictoryTypes eVictory = static_cast<VictoryTypes>(iVictoryLoop);
				CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eVictory);
				if(pkVictoryInfo)
				{
					if(pkVictoryInfo->isTargetScore())
					{
						eScoreVictory = eVictory;
						break;
					}
				}
			}

			aaiGameWinners.clear();


			int iBestVPNum = 0;
			int iVPs;
			for(iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
			{
				iVPs = GET_TEAM((TeamTypes) iTeamLoop).getVictoryPoints();

				if(iVPs > iBestVPNum)
				{
					iBestVPNum = iVPs;
				}
			}


			for(iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
			{
				iVPs = GET_TEAM((TeamTypes) iTeamLoop).getVictoryPoints();

				if(iVPs == iBestVPNum)
				{
					std::vector<int> aWinner;
					aWinner.push_back(iTeamLoop);
					aWinner.push_back(eScoreVictory);
					aaiGameWinners.push_back(aWinner);
				}
			}
		}
	}

	if(!bEndGame)
	{
		bEndGame = (getMaxTurns() > 0 && getElapsedGameTurns() >= getMaxTurns());

		if(bEndGame)
		{
			VictoryTypes eScoreVictory = NO_VICTORY;
			for(iVictoryLoop = 0; iVictoryLoop < GC.getNumVictoryInfos(); iVictoryLoop++)
			{
				VictoryTypes eVictory = static_cast<VictoryTypes>(iVictoryLoop);
				CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eVictory);
				if(pkVictoryInfo)
				{
					if(pkVictoryInfo->isEndScore())
					{
						eScoreVictory = eVictory;
						break;
					}
				}
			}

			aaiGameWinners.clear();


			int iBestVPNum = 0;
			int iVPs;
			for(iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
			{
				iVPs = GET_TEAM((TeamTypes) iTeamLoop).GetScore();

				if(iVPs > iBestVPNum)
				{
					iBestVPNum = iVPs;
				}
			}


			for(iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
			{
				iVPs = GET_TEAM((TeamTypes) iTeamLoop).GetScore();

				if(iVPs == iBestVPNum)
				{
					std::vector<int> aWinner;
					aWinner.push_back(iTeamLoop);
					aWinner.push_back(eScoreVictory);
					aaiGameWinners.push_back(aWinner);
				}
			}
		}
	}



	if(bEndGame && !aaiGameWinners.empty())
	{
		int iWinner = getJonRandNum(aaiGameWinners.size(), "Victory tie breaker");
		setWinner(((TeamTypes)aaiGameWinners[iWinner][0]), ((VictoryTypes)aaiGameWinners[iWinner][1]));
	}

	if(getVictory() == NO_VICTORY)
	{
		if(getMaxTurns() > 0)
		{
			if(getElapsedGameTurns() >= getMaxTurns())
			{
				if(!bEndScore)
				{
					if((getAIAutoPlay() > 0) || gDLL->GetAutorun())
					{
						setGameState(GAMESTATE_EXTENDED);
					}
					else
					{
						setGameState(GAMESTATE_OVER);
					}
				}
			}
		}
	}
}


CvRandom& CvGame::getMapRand()
{
	return m_mapRand;
}



int CvGame::getMapRandNum(int iNum, const char* pszLog)
{
	return m_mapRand.get(iNum, pszLog);
}



CvRandom& CvGame::getJonRand()
{
	return m_jonRand;
}





int CvGame::getJonRandNum(int iNum, const char* pszLog)
{
	return m_jonRand.get(iNum, pszLog);
}

#ifdef AUI_BINOM_RNG



int CvGame::getJonRandNumBinom(int iNum, const char* pszLog)
{
	return m_jonRand.getBinom(iNum, pszLog);
}
#endif






int CvGame::getJonRandNumVA(int iNum, const char* pszLog, ...)
{
	if (pszLog)
	{
		const size_t uiOutputSize = 512;
		char szOutput[uiOutputSize];

		va_list vl;
		va_start(vl, pszLog);
		vsprintf_s(szOutput, uiOutputSize, pszLog, vl);
		va_end(vl);

		return m_jonRand.get(iNum, szOutput);
	}
	else
		return m_jonRand.get(iNum);
}




int CvGame::getAsyncRandNum(int iNum, const char* pszLog)
{
	return GC.getASyncRand().get(iNum, pszLog);
}


int CvGame::calculateSyncChecksum()
{
	CvUnit* pLoopUnit;
#if defined(AUI_WARNING_FIXES) || defined(AUI_GAME_FIX_SYNC_CHECKSUM_USE_UNSIGNED)
	uint uiMultiplier;
	uint uiValue = 0;
	int iLoop;
	uint iJ;

#ifdef AUI_USE_SFMT_RNG
	uiValue += getMapRand().getSeed().first;
	uiValue += getMapRand().getSeed().second;
	uiValue += getJonRand().getSeed().first;
	uiValue += getJonRand().getSeed().second;
#else
	uiValue += getMapRand().getSeed();
	uiValue += getJonRand().getSeed();
#endif

	uiValue += getNumCities();
	uiValue += getTotalPopulation();

	uiValue += GC.getMap().getOwnedPlots();
	uiValue += GC.getMap().getNumAreas();

	int iTurnSlice = getTurnSlice() % 4;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		PlayerTypes ePlayer = static_cast<PlayerTypes>(iI);
		CvPlayer& kPlayer = GET_PLAYER(ePlayer);
		if (kPlayer.isEverAlive())
		{
			uiMultiplier = getPlayerScore((PlayerTypes)iI);

			switch (iTurnSlice)
			{
			case 0:
				uiMultiplier += kPlayer.getTotalPopulation() * 543271;
				uiMultiplier += kPlayer.getTotalLand() * 327382;
				uiMultiplier += kPlayer.GetTreasury()->GetGold() * 107564;
				uiMultiplier += kPlayer.getPower() * 135647;
				uiMultiplier += kPlayer.getNumCities() * 436432;
				uiMultiplier += kPlayer.getNumUnits() * 324111;
				break;

			case 1:
				for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					uiMultiplier += kPlayer.calculateTotalYield((YieldTypes)iJ) * 432754;
				}
				break;

			case 2:
				for (iJ = 0; iJ < GC.getNumImprovementInfos(); iJ++)
				{
					uiMultiplier += kPlayer.getImprovementCount((ImprovementTypes)iJ) * 883422;
				}

				for (iJ = 0; iJ < GC.getNumBuildingClassInfos(); iJ++)
				{
					CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo((BuildingClassTypes)iJ);
					if (pkBuildingClassInfo)
					{
						uiMultiplier += kPlayer.getBuildingClassCountPlusMaking((BuildingClassTypes)iJ) * 954531;
					}
				}

				for (iJ = 0; iJ < GC.getNumUnitClassInfos(); iJ++)
				{
					CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo((UnitClassTypes)iJ);
					if (pkUnitClassInfo)
					{
						uiMultiplier += kPlayer.getUnitClassCountPlusMaking((UnitClassTypes)iJ) * 754843;
					}
				}
				break;

			case 3:
				for (pLoopUnit = GET_PLAYER((PlayerTypes)iI).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER((PlayerTypes)iI).nextUnit(&iLoop))
				{
					uiMultiplier += pLoopUnit->getX() * 876543;
					uiMultiplier += pLoopUnit->getY() * 985310;
					uiMultiplier += pLoopUnit->getDamage() * 736373;
					uiMultiplier += pLoopUnit->getExperience() * 820622;
					uiMultiplier += pLoopUnit->getLevel() * 367291;
				}
				break;
			}

			if (uiMultiplier != 0)
			{
				uiValue *= uiMultiplier;
			}
		}
	}

	return (int)uiValue;
#else
	int iMultiplier;
	int iValue;
	int iLoop;
	int iI, iJ;

	iValue = 0;

#ifdef AUI_USE_SFMT_RNG
	iValue += getMapRand().getSeed().first;
	iValue += getMapRand().getSeed().second;
	iValue += getJonRand().getSeed().first;
	iValue += getJonRand().getSeed().second;
#else
	iValue += getMapRand().getSeed();
	iValue += getJonRand().getSeed();
#endif

	iValue += getNumCities();
	iValue += getTotalPopulation();

	iValue += GC.getMap().getOwnedPlots();
	iValue += GC.getMap().getNumAreas();

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isEverAlive())
		{
			iMultiplier = getPlayerScore((PlayerTypes)iI);

			switch(getTurnSlice() % 4)
			{
			case 0:
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).getTotalPopulation() * 543271);
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).getTotalLand() * 327382);
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).GetTreasury()->GetGold() * 107564);
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).getPower() * 135647);
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).getNumCities() * 436432);
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).getNumUnits() * 324111);
				break;

			case 1:
				for(iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					iMultiplier += (GET_PLAYER((PlayerTypes)iI).calculateTotalYield((YieldTypes)iJ) * 432754);
				}
				break;

			case 2:
				for(iJ = 0; iJ < GC.getNumImprovementInfos(); iJ++)
				{
					iMultiplier += (GET_PLAYER((PlayerTypes)iI).getImprovementCount((ImprovementTypes)iJ) * 883422);
				}

				for(iJ = 0; iJ < GC.getNumBuildingClassInfos(); iJ++)
				{
					CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo((BuildingClassTypes)iJ);
					if(!pkBuildingClassInfo)
					{
						continue;
					}

					iMultiplier += (GET_PLAYER((PlayerTypes)iI).getBuildingClassCountPlusMaking((BuildingClassTypes)iJ) * 954531);
				}

				for(iJ = 0; iJ < GC.getNumUnitClassInfos(); iJ++)
				{
					CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo((UnitClassTypes)iJ);
					if(!pkUnitClassInfo)
					{
						continue;
					}

					iMultiplier += (GET_PLAYER((PlayerTypes)iI).getUnitClassCountPlusMaking((UnitClassTypes)iJ) * 754843);
				}
				break;

			case 3:
				for(pLoopUnit = GET_PLAYER((PlayerTypes)iI).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER((PlayerTypes)iI).nextUnit(&iLoop))
				{
					iMultiplier += (pLoopUnit->getX() * 876543);
					iMultiplier += (pLoopUnit->getY() * 985310);
					iMultiplier += (pLoopUnit->getDamage() * 736373);
					iMultiplier += (pLoopUnit->getExperience() * 820622);
					iMultiplier += (pLoopUnit->getLevel() * 367291);
				}
				break;
			}

			if(iMultiplier != 0)
			{
				iValue *= iMultiplier;
			}
		}
	}

	return iValue;
#endif
}



int CvGame::calculateOptionsChecksum()
{
	int iValue;
	int iI, iJ;

	iValue = 0;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);

		for(iJ = 0; iJ < GC.getNumPlayerOptionInfos(); iJ++)
		{
			CvPlayerOptionInfo* pkInfo = GC.getPlayerOptionInfo((PlayerOptionTypes)iJ);
			if (pkInfo)
			{
				uint uiID = FString::Hash( pkInfo->GetType() );
				if(kPlayer.isOption((PlayerOptionTypes)uiID))
				{
					iValue += (iI * 943097);
					iValue += (iJ * 281541);
				}
			}
		}
	}

	return iValue;
}



void CvGame::addReplayMessage(ReplayMessageTypes eType, PlayerTypes ePlayer, const CvString& pszText, int iPlotX, int iPlotY)
{
	int iGameTurn = getGameTurn();


	if(iPlotX != -1 || iPlotY != -1)
	{
		for(ReplayMessageList::iterator it = m_listReplayMessages.begin(); it != m_listReplayMessages.end(); ++it)
		{
			CvReplayMessage& msg = (*it);
			if(msg.getType() == eType && msg.getTurn() == iGameTurn && msg.getPlayer() == ePlayer && msg.getText() == pszText)
			{
				msg.addPlot(iPlotX, iPlotY);
				return;
			}
		}
	}

	CvReplayMessage message(iGameTurn, eType, ePlayer);
	message.addPlot(iPlotX, iPlotY);
	message.setText(pszText);
	m_listReplayMessages.push_back(message);
}

#ifdef REPLAY_MESSAGE_EXTENDED

void CvGame::addReplayMessage(ReplayMessageTypes eType, PlayerTypes ePlayer, const CvString& pszText, int iData1, int iData2, int iPlotX, int iPlotY)
{
	int iGameTurn = getGameTurn();


	if (iPlotX != -1 || iPlotY != -1)
	{
		for (ReplayMessageList::iterator it = m_listReplayMessages.begin(); it != m_listReplayMessages.end(); ++it)
		{
			CvReplayMessage& msg = (*it);
			if (msg.getType() == eType && msg.getTurn() == iGameTurn && msg.getPlayer() == ePlayer && msg.getText() == pszText)
			{
				msg.addPlot(iPlotX, iPlotY);
				return;
			}
		}
	}

	CvReplayMessage message(iGameTurn, iData1, iData2, eType, ePlayer);
	message.addPlot(iPlotX, iPlotY);
	message.setText(pszText);
	m_listReplayMessages.push_back(message);
}
#endif


void CvGame::clearReplayMessageMap()
{
	m_listReplayMessages.clear();
}


uint CvGame::getNumReplayMessages() const
{
	return m_listReplayMessages.size();
}


const CvReplayMessage* CvGame::getReplayMessage(uint i) const
{
	if(i < m_listReplayMessages.size())
	{
		return &(m_listReplayMessages[i]);
	}

	return NULL;
}
#ifdef REPLAY_EVENTS


void CvGame::clearReplayEventMap()
{
	m_listReplayEvents.clear();
}


void CvGame::addReplayEvent(int eType, std::vector<int> vNumArgs, CvString strArg)
{
	CvReplayEvent event(eType, vNumArgs, strArg);
	m_listReplayEvents.push_back(event);
}

void CvGame::addReplayEvent(int eType, PlayerTypes ePlayer, std::vector<int> vNumArgs, CvString strArg)
{
	CvReplayEvent event(eType, ePlayer, vNumArgs, strArg);
	m_listReplayEvents.push_back(event);
}


uint CvGame::getNumReplayEvents() const
{
	return m_listReplayEvents.size();
}


const CvReplayEvent* CvGame::getReplayEvent(uint i) const
{
	if (i < m_listReplayEvents.size())
	{
		return &(m_listReplayEvents[i]);
	}

	return NULL;
}
#endif




void CvGame::Read(FDataStream& kStream)
{
	int iI;

	reset(NO_HANDICAP);


	uint uiVersion;
	kStream >> uiVersion;
	{
		FILogFile* pDbg = LOGFILEMGR.GetLog("LoadDebug.log", FILogFile::kDontTimeStamp);
		pDbg->Msg("CvGame::Read TOP uiVersion=%u (expected %d) endian=%d", uiVersion, g_CurrentCvGameVersion, (int)kStream.GetDesiredEndianNess());
	}
#if defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
	kStream >> m_iCombatModListLength;
	{
		FILogFile* pDbg = LOGFILEMGR.GetLog("LoadDebug.log", FILogFile::kDontTimeStamp);
		pDbg->Msg("CvGame::Read after m_iCombatModListLength=%d", m_iCombatModListLength);
	}
#endif
	kStream >> m_iEndTurnMessagesSent;
	kStream >> m_iElapsedGameTurns;
	kStream >> m_iStartTurn;
	kStream >> m_iWinningTurn;
	kStream >> m_iStartYear;
	kStream >> m_iEstimateEndTurn;
	m_iDefaultEstimateEndTurn = m_iEstimateEndTurn;
	kStream >> m_iDefaultEstimateEndTurn;
	kStream >> m_iTurnSlice;
	kStream >> m_iCutoffSlice;
	kStream >> m_iNumCities;
	kStream >> m_iTotalPopulation;
	kStream >> m_iNoNukesCount;
	kStream >> m_iNukesExploded;
	kStream >> m_iMaxPopulation;
	kStream >> m_iUnused1;
	kStream >> m_iUnused2;
	kStream >> m_iUnused3;
	kStream >> m_iInitPopulation;
	kStream >> m_iInitLand;
	kStream >> m_iInitTech;
	kStream >> m_iInitWonders;
	kStream >> m_iAIAutoPlay;

	if(m_iAIAutoPlay > 0)
	{
		m_iAIAutoPlay = 1;
	}

	kStream >> m_iTotalReligionTechCost;
	kStream >> m_iCachedWorldReligionTechProgress;
	kStream >> m_iUnitedNationsCountdown;
	kStream >> m_iNumVictoryVotesTallied;
	kStream >> m_iNumVictoryVotesExpected;
	kStream >> m_iVotesNeededForDiploVictory;
	kStream >> m_iMapScoreMod;

#ifdef GAME_UPDATE_TURN_TIMER_ONCE_PER_TURN
	kStream >> m_fPreviousTurnLen;
#endif
#ifdef TURN_TIMER_PAUSE_BUTTON
	kStream >> m_fTimeElapsed;
	kStream >> m_bIsPaused;
#endif

	kStream >> m_bScoreDirty;
	kStream >> m_bCircumnavigated;

	kStream >> m_bFinalInitialized;

	kStream >> m_bHotPbemBetweenTurns;

	kStream >> m_bNukesValid;
	kStream >> m_bEndGameTechResearched;
	kStream >> m_bTunerEverConnected;
	kStream >> m_bTutorialEverAttacked;
	kStream >> m_bStaticTutorialActive;
	kStream >> m_bEverRightClickMoved;
	kStream >> m_AdvisorMessagesViewed;
	kStream >> m_eHandicap;
	kStream >> m_ePausePlayer;
	kStream >> m_eAIAutoPlayReturnPlayer;
	kStream >> m_eBestLandUnit;
	kStream >> m_eWinner;
	kStream >> m_eVictory;
	kStream >> m_eGameState;
	if(m_eGameState == GAMESTATE_OVER)
	{
		m_eGameState = GAMESTATE_EXTENDED;
	}
	kStream >> m_eBestWondersPlayer;
	kStream >> m_eBestPoliciesPlayer;
	kStream >> m_eBestGreatPeoplePlayer;
	kStream >> m_eReligionTech;
	kStream >> m_eIndustrialRoute;

	kStream >> m_strScriptData;

	ArrayWrapper<int> wrapm_aiEndTurnMessagesReceived(MAX_PLAYERS, m_aiEndTurnMessagesReceived);
	kStream >> wrapm_aiEndTurnMessagesReceived;

	ArrayWrapper<int> wrapm_aiRankPlayer(MAX_PLAYERS, m_aiRankPlayer);
	kStream >> wrapm_aiRankPlayer;

	ArrayWrapper<int> wrapm_aiPlayerRank(MAX_PLAYERS, m_aiPlayerRank);
	kStream >> wrapm_aiPlayerRank;

	ArrayWrapper<int> wrapm_aiPlayerScore(MAX_PLAYERS, m_aiPlayerScore);
	kStream >> wrapm_aiPlayerScore;

	ArrayWrapper<int> wrapm_aiRankTeam(MAX_TEAMS, m_aiRankTeam);
	kStream >> wrapm_aiRankTeam;

	ArrayWrapper<int> wrapm_aiTeamRank(MAX_TEAMS, m_aiTeamRank);
	kStream >> wrapm_aiTeamRank;

	ArrayWrapper<int> wrapm_aiTeamScore(MAX_TEAMS, m_aiTeamScore);
	kStream >> wrapm_aiTeamScore;

	UnitArrayHelpers::Read(kStream, m_paiUnitCreatedCount);
	UnitClassArrayHelpers::Read(kStream, m_paiUnitClassCreatedCount);
	BuildingClassArrayHelpers::Read(kStream, m_paiBuildingClassCreatedCount);

	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiProjectCreatedCount, GC.getNumProjectInfos());
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiVoteOutcome, GC.getNumVoteInfos());

	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_aiSecretaryGeneralTimer, GC.getNumVoteSourceInfos());
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_aiVoteTimer, GC.getNumVoteSourceInfos());
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_aiDiploVote, GC.getNumVoteSourceInfos());

	ArrayWrapper<int> wrapm_aiVotesCast(MAX_CIV_TEAMS, m_aiVotesCast);
	kStream >> wrapm_aiVotesCast;

	ArrayWrapper<int> wrapm_aiPreviousVotesCast(MAX_CIV_TEAMS, m_aiPreviousVotesCast);
	kStream >> wrapm_aiPreviousVotesCast;

	ArrayWrapper<int> wrapm_aiNumVotesForTeam(MAX_CIV_TEAMS, m_aiNumVotesForTeam);
	kStream >> wrapm_aiNumVotesForTeam;

	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabSpecialUnitValid, GC.getNumSpecialUnitInfos());

	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_ppaaiTeamVictoryRank, GC.getNUM_VICTORY_POINT_AWARDS(), GC.getNumVictoryInfos());

	kStream >> m_aszDestroyedCities;
	kStream >> m_aszGreatPeopleBorn;
	kStream >> m_voteSelections;
	kStream >> m_votesTriggered;

	m_mapRand.read(kStream);
	bool wasCallStackDebuggingEnabled = m_jonRand.callStackDebuggingEnabled();
	m_jonRand.read(kStream);
	m_jonRand.clearCallstacks();
	m_jonRand.setCallStackDebuggingEnabled(wasCallStackDebuggingEnabled);

	{
		clearReplayMessageMap();

		unsigned int uiReplayMessageVersion = 1;
		int iSize = 0;

		kStream >> uiReplayMessageVersion;

		kStream >> iSize;
		for(int i = 0; i < iSize; i++)
		{
			CvReplayMessage message;
			message.read(kStream, uiReplayMessageVersion);
			m_listReplayMessages.push_back(message);
		}
#ifdef REPLAY_EVENTS
		clearReplayEventMap();

		unsigned int uiReplayEventVersion = 1;
		iSize = 0;

		kStream >> uiReplayEventVersion;

		kStream >> iSize;
		for (int i = 0; i < iSize; i++)
		{
			CvReplayEvent event;
			event.read(kStream, uiReplayEventVersion);
			m_listReplayEvents.push_back(event);
		}
#endif
	}

	kStream >> m_iNumSessions;
	if(!isNetworkMultiPlayer())
	{
		++m_iNumSessions;
	}

	kStream >> m_aPlotExtraYields;
	kStream >> m_aPlotExtraCosts;



	if(!isGameMultiPlayer())
	{
		for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
		{
			if(GET_PLAYER((PlayerTypes)iI).isHuman())
			{
				setActivePlayer((PlayerTypes)iI);
				break;
			}
		}
		Localization::String localizedText = Localization::Lookup("TXT_KEY_MISC_RELOAD");
		localizedText << m_iNumSessions;
		addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getActivePlayer(), localizedText.toUTF8());
	}

	if(isOption(GAMEOPTION_NEW_RANDOM_SEED))
	{
		if(!isNetworkMultiPlayer())
		{
			m_jonRand.reseed(timeGetTime());
		}
	}

	kStream >> m_bArchaeologyTriggered;

	kStream >> m_iEarliestBarbarianReleaseTurn;
	kStream >> m_kGameDeals;
	kStream >> *m_pGameReligions;
	{ FILogFile* pDbg = LOGFILEMGR.GetLog("LoadDebug.log", FILogFile::kDontTimeStamp); pDbg->Msg("CvGame::Read before GameCulture used=%u, NumGreatWorks=%d", kStream.GetSizeLeft(), m_pGameCulture ? m_pGameCulture->GetNumGreatWorks() : -1); }
	kStream >> *m_pGameCulture;
	{ FILogFile* pDbg = LOGFILEMGR.GetLog("LoadDebug.log", FILogFile::kDontTimeStamp); pDbg->Msg("CvGame::Read after GameCulture used=%u, NumGreatWorks=%d", kStream.GetSizeLeft(), m_pGameCulture->GetNumGreatWorks()); }
	kStream >> *m_pGameLeagues;
	kStream >> *m_pGameTrade;
#ifdef MP_PLAYERS_VOTING_SYSTEM
	kStream >> *m_pMPVotingSystem;
#endif

	unsigned int lSize = 0;
	kStream >> lSize;
	if(lSize > 0)
	{

		CvString strUTF8DatabasePath = gDLL->GetCacheFolderPath();
		strUTF8DatabasePath += "Civ5SavedGameDatabase.db";


		std::wstring wstrDatabasePath = CvStringUtils::FromUTF8ToUTF16(strUTF8DatabasePath);

		FIFile* pkFile = FFILESYSTEM.Create(wstrDatabasePath.c_str(), FIFile::modeWrite);
		if (pkFile != NULL)
		{
			byte* szBuffer = GetTempHeap()->Allocate(sizeof(char) * lSize);
			ZeroMemory((void*)szBuffer, lSize);

			kStream.ReadIt(lSize, szBuffer);

			pkFile->Write(szBuffer, lSize);

			pkFile->Close();

			GetTempHeap()->DeAllocate(szBuffer);
		}
		else
		{
			CvAssertMsg(false, "Cannot open Civ5SavedGameDatabase.db for write! Does something have this opened?");
		}
	}

#ifdef AUI_GAME_BETTER_HYBRID_MODE
	kStream >> m_iCurrentTurnOrderActive;
	kStream >> m_iLastTurnOrderID;
#ifdef AUI_GAME_PLAYER_BASED_TURN_LENGTH
	kStream >> m_aiMaxTurnLengths;
#endif
#endif



	m_lastTurnAICivsProcessed = -1;
}


void CvGame::ReadSupportingClassData(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;	

	CvBarbarians::Read(kStream, uiVersion);
	CvGoodyHuts::Read(kStream, uiVersion);
}


void CvGame::Write(FDataStream& kStream) const
{

	kStream << g_CurrentCvGameVersion;
#if defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
	kStream << m_iCombatModListLength;
#endif
	kStream << m_iEndTurnMessagesSent;
	kStream << m_iElapsedGameTurns;
	kStream << m_iStartTurn;
	kStream << m_iWinningTurn;
	kStream << m_iStartYear;
	kStream << m_iEstimateEndTurn;
	kStream << m_iDefaultEstimateEndTurn;
	kStream << m_iTurnSlice;
	kStream << m_iCutoffSlice;
	kStream << m_iNumCities;
	kStream << m_iTotalPopulation;
	kStream << m_iNoNukesCount;
	kStream << m_iNukesExploded;
	kStream << m_iMaxPopulation;
	kStream << m_iUnused1;
	kStream << m_iUnused2;
	kStream << m_iUnused3;
	kStream << m_iInitPopulation;
	kStream << m_iInitLand;
	kStream << m_iInitTech;
	kStream << m_iInitWonders;
	kStream << m_iAIAutoPlay;
	kStream << m_iTotalReligionTechCost;
	kStream << m_iCachedWorldReligionTechProgress;
	kStream << m_iUnitedNationsCountdown;
	kStream << m_iNumVictoryVotesTallied;
	kStream << m_iNumVictoryVotesExpected;
	kStream << m_iVotesNeededForDiploVictory;
	kStream << m_iMapScoreMod;


#ifdef GAME_UPDATE_TURN_TIMER_ONCE_PER_TURN
	kStream << m_fPreviousTurnLen;
#endif
#ifdef TURN_TIMER_PAUSE_BUTTON
	kStream << m_fTimeElapsed;
	kStream << m_bIsPaused;
#endif

	kStream << m_bScoreDirty;
	kStream << m_bCircumnavigated;

	kStream << m_bFinalInitialized;

	kStream << m_bHotPbemBetweenTurns;

	kStream << m_bNukesValid;
	kStream << m_bEndGameTechResearched;
	kStream << TunerEverConnected();
	kStream << m_bTutorialEverAttacked;
	kStream << m_bStaticTutorialActive;
	kStream << m_bEverRightClickMoved;
	kStream << m_AdvisorMessagesViewed;

	kStream << m_eHandicap;
	kStream << m_ePausePlayer;
	kStream << m_eAIAutoPlayReturnPlayer;
	kStream << m_eBestLandUnit;
	kStream << m_eWinner;
	kStream << m_eVictory;
	kStream << m_eGameState;
	kStream << m_eBestWondersPlayer;
	kStream << m_eBestPoliciesPlayer;
	kStream << m_eBestGreatPeoplePlayer;
	kStream << m_eReligionTech;
	kStream << m_eIndustrialRoute;

	kStream << m_strScriptData;

	kStream << ArrayWrapper<int>(MAX_PLAYERS, m_aiEndTurnMessagesReceived);
	kStream << ArrayWrapper<int>(MAX_PLAYERS, m_aiRankPlayer);
	kStream << ArrayWrapper<int>(MAX_PLAYERS, m_aiPlayerRank);
	kStream << ArrayWrapper<int>(MAX_PLAYERS, m_aiPlayerScore);
	kStream << ArrayWrapper<int>(MAX_TEAMS, m_aiRankTeam);
	kStream << ArrayWrapper<int>(MAX_TEAMS, m_aiTeamRank);
	kStream << ArrayWrapper<int>(MAX_TEAMS, m_aiTeamScore);

	UnitArrayHelpers::Write(kStream, m_paiUnitCreatedCount, GC.getNumUnitInfos());

	UnitClassArrayHelpers::Write(kStream, m_paiUnitClassCreatedCount, GC.getNumUnitClassInfos());
	BuildingClassArrayHelpers::Write(kStream, m_paiBuildingClassCreatedCount, GC.getNumBuildingClassInfos());

	CvInfosSerializationHelper::WriteHashedDataArray<ProjectTypes, int>(kStream, m_paiProjectCreatedCount, GC.getNumProjectInfos());
	CvInfosSerializationHelper::WriteHashedDataArray<VoteTypes, PlayerVoteTypes>(kStream, m_paiVoteOutcome, GC.getNumVoteInfos());

	CvInfosSerializationHelper::WriteHashedDataArray<VoteSourceTypes, int>(kStream, m_aiSecretaryGeneralTimer, GC.getNumVoteSourceInfos());
	CvInfosSerializationHelper::WriteHashedDataArray<VoteSourceTypes, int>(kStream, m_aiVoteTimer, GC.getNumVoteSourceInfos());
	CvInfosSerializationHelper::WriteHashedDataArray<VoteSourceTypes, int>(kStream, m_aiDiploVote, GC.getNumVoteSourceInfos());

	kStream << ArrayWrapper<int>(MAX_CIV_TEAMS, m_aiVotesCast);
	kStream << ArrayWrapper<int>(MAX_CIV_TEAMS, m_aiPreviousVotesCast);
	kStream << ArrayWrapper<int>(MAX_CIV_TEAMS, m_aiNumVotesForTeam);

	CvInfosSerializationHelper::WriteHashedDataArray<SpecialUnitTypes, bool>(kStream, m_pabSpecialUnitValid, GC.getNumSpecialUnitInfos());

	CvInfosSerializationHelper::WriteHashedDataArray<VictoryTypes, int>(kStream, m_ppaaiTeamVictoryRank, GC.getNUM_VICTORY_POINT_AWARDS(), GC.getNumVictoryInfos());

	kStream << m_aszDestroyedCities;
	kStream << m_aszGreatPeopleBorn;
	kStream << m_voteSelections;
	kStream << m_votesTriggered;

	m_mapRand.write(kStream);
	m_jonRand.write(kStream);

	const int iSize = m_listReplayMessages.size();
	kStream << CvReplayMessage::Version();
	kStream << iSize;

	ReplayMessageList::const_iterator it;
	for(it = m_listReplayMessages.begin(); it != m_listReplayMessages.end(); ++it)
	{
		(*it).write(kStream);
	}
#ifdef REPLAY_EVENTS
	const int iSize2 = m_listReplayEvents.size();
	kStream << CvReplayEvent::Version();
	kStream << iSize2;

	ReplayEventList::const_iterator it2;
	for (it2 = m_listReplayEvents.begin(); it2 != m_listReplayEvents.end(); ++it2)
	{
		(*it2).write(kStream);
	}
#endif

	kStream << m_iNumSessions;

	kStream << m_aPlotExtraYields;
	kStream << m_aPlotExtraCosts;

	kStream << m_bArchaeologyTriggered;

	kStream << m_iEarliestBarbarianReleaseTurn;

	kStream << m_kGameDeals;
	kStream << *m_pGameReligions;
	{ FILogFile* pDbg = LOGFILEMGR.GetLog("SaveDebug.log", FILogFile::kDontTimeStamp); pDbg->Msg("CvGame::Write before GameCulture used=%u, NumGreatWorks=%d", kStream.GetSizeLeft(), m_pGameCulture->GetNumGreatWorks()); }
	kStream << *m_pGameCulture;
	{ FILogFile* pDbg = LOGFILEMGR.GetLog("SaveDebug.log", FILogFile::kDontTimeStamp); pDbg->Msg("CvGame::Write after GameCulture used=%u", kStream.GetSizeLeft()); }
	kStream << *m_pGameLeagues;
	kStream << *m_pGameTrade;
#ifdef MP_PLAYERS_VOTING_SYSTEM
	kStream << *m_pMPVotingSystem;
#endif


	CvString strPath = gDLL->GetCacheFolderPath();
	strPath += "Civ5SavedGameDatabase.db";


	wchar_t savePath[MAX_PATH] = {0};
	MultiByteToWideChar(CP_UTF8, 0, strPath.c_str(), -1, savePath, MAX_PATH);

	HANDLE hFile = CreateFileW(savePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwSize = GetFileSize(hFile, NULL);
		if(dwSize != INVALID_FILE_SIZE)
		{
			byte* szBuffer = GetTempHeap()->Allocate(sizeof(char) * dwSize);
			ZeroMemory((void*)szBuffer, dwSize);

			DWORD dwBytesRead = 0;
#ifdef AUI_WARNING_FIXES
			if (ReadFile(hFile, szBuffer, dwSize, &dwBytesRead, NULL) != FALSE)
#else
			if(ReadFile(hFile, szBuffer, dwSize, &dwBytesRead, NULL) == TRUE)
#endif
			{

				kStream << dwBytesRead;
				kStream.WriteIt(dwBytesRead, szBuffer);
			}

			GetTempHeap()->DeAllocate(szBuffer);
		}
		else
		{
			CvAssertMsg(false, "Saved game database exists, but could not get file size???");
		}
	
		if(CloseHandle(hFile) == FALSE)
		{
			CvAssertMsg(false, "Could not close file handle to saved game database!");
		}
	}
	else
	{
		WIN32_FILE_ATTRIBUTE_DATA fileAttributes;
		if(GetFileAttributesExW(savePath, GetFileExInfoStandard, &fileAttributes) != INVALID_FILE_ATTRIBUTES)
		{
			CvAssertMsg(false,"Saved game database exists, but could not open it!");
		}

#if defined(LEKMOD_MACOS)

		int nilSize = 0;
#else
		long nilSize = 0;
#endif
		kStream << nilSize;
	}

#ifdef AUI_GAME_BETTER_HYBRID_MODE
	kStream << m_iCurrentTurnOrderActive;
	kStream << m_iLastTurnOrderID;
#ifdef AUI_GAME_PLAYER_BASED_TURN_LENGTH
	kStream << m_aiMaxTurnLengths;
#endif
#endif
}


void CvGame::WriteSupportingClassData(FDataStream& kStream)
{

	uint uiVersion = 1;
	kStream << uiVersion;

	CvBarbarians::Write(kStream);
	CvGoodyHuts::Write(kStream);
}


void CvGame::writeReplay(FDataStream& kStream)
{
	CvReplayInfo replayInfo;
	replayInfo.createInfo();
	replayInfo.write(kStream);
}


void CvGame::saveReplay()
{
	gDLL->saveReplay();

	CvPlayerAI& activePlayer = GET_PLAYER(getActivePlayer());

	bool playerTeamWon = (getActiveTeam() == getWinner());
	int score = activePlayer.GetScore(true, playerTeamWon);

	if(!isHotSeat())
	{
		gDLL->RecordVictoryInformation(score);
		gDLL->RecordLeaderboardScore(score);
	}
}


void CvGame::showEndGameSequence()
{

	GC.GetEngineUserInterface()->OpenEndGameMenu();
}


void CvGame::addPlayer(PlayerTypes eNewPlayer, LeaderHeadTypes eLeader, CivilizationTypes eCiv)
{
	CvCivilizationInfo* pkCivilizationInfo = GC.getCivilizationInfo(eCiv);
	CvCivilizationInfo* pkBarbarianCivInfo = GC.getCivilizationInfo(static_cast<CivilizationTypes>(GC.getBARBARIAN_CIVILIZATION()));

	if(pkCivilizationInfo == NULL || pkBarbarianCivInfo == NULL)
	{

		CvAssert(false);
		return;
	}

	PlayerColorTypes eColor = (PlayerColorTypes)pkCivilizationInfo->getDefaultPlayerColor();

	for(int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if(eColor == NO_PLAYERCOLOR || GET_PLAYER((PlayerTypes)iI).getPlayerColor() == eColor)
		{
#ifdef AUI_WARNING_FIXES
			for (uint iK = 0; iK < GC.GetNumPlayerColorInfos(); iK++)
#else
			for(int iK = 0; iK < GC.GetNumPlayerColorInfos(); iK++)
#endif
			{
				const PlayerColorTypes ePlayerColor = static_cast<PlayerColorTypes>(iK);
				CvPlayerColorInfo* pkPlayerColorInfo = GC.GetPlayerColorInfo(ePlayerColor);
				if(pkPlayerColorInfo)
				{
#ifdef AUI_WARNING_FIXES
					if (iK != uint(pkBarbarianCivInfo->getDefaultPlayerColor()))
#else
					if(iK != pkBarbarianCivInfo->getDefaultPlayerColor())
#endif
					{
						bool bValid = true;

						for(int iL = 0; iL < MAX_CIV_PLAYERS; iL++)
						{
							if(GET_PLAYER((PlayerTypes)iL).getPlayerColor() == ePlayerColor)
							{
								bValid = false;
								break;
							}
						}

						if(bValid)
						{
							eColor = ePlayerColor;
							iI = MAX_CIV_PLAYERS;
							break;
						}
					}
				}
			}
		}
	}

	CvPreGame::setLeaderHead(eNewPlayer, eLeader);
	CvPreGame::setCivilization(eNewPlayer, eCiv);
	CvPreGame::setSlotStatus(eNewPlayer, SS_COMPUTER);
	CvPreGame::setPlayerColor(eNewPlayer, eColor);
	GET_PLAYER(eNewPlayer).init(eNewPlayer);
}


int CvGame::getPlotExtraYield(int iX, int iY, YieldTypes eYield) const
{
	for(std::vector<PlotExtraYield>::const_iterator it = m_aPlotExtraYields.begin(); it != m_aPlotExtraYields.end(); ++it)
	{
		if((*it).m_iX == iX && (*it).m_iY == iY)
		{
			return (*it).m_aeExtraYield[eYield];
		}
	}

	return 0;
}


void CvGame::setPlotExtraYield(int iX, int iY, YieldTypes eYield, int iExtraYield)
{
	bool bFound = false;

	for(std::vector<PlotExtraYield>::iterator it = m_aPlotExtraYields.begin(); it != m_aPlotExtraYields.end(); ++it)
	{
		if((*it).m_iX == iX && (*it).m_iY == iY)
		{
			(*it).m_aeExtraYield[eYield] += iExtraYield;
			bFound = true;
			break;
		}
	}

	if(!bFound)
	{
		PlotExtraYield kExtraYield;
		kExtraYield.m_iX = iX;
		kExtraYield.m_iY = iY;
		for(int i = 0; i < NUM_YIELD_TYPES; ++i)
		{
			if(eYield == i)
			{
				kExtraYield.m_aeExtraYield.push_back(iExtraYield);
			}
			else
			{
				kExtraYield.m_aeExtraYield.push_back(0);
			}
		}
		m_aPlotExtraYields.push_back(kExtraYield);
	}

	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	if(NULL != pPlot)
	{
		pPlot->updateYield();
	}
}


void CvGame::removePlotExtraYield(int iX, int iY)
{
	for(std::vector<PlotExtraYield>::iterator it = m_aPlotExtraYields.begin(); it != m_aPlotExtraYields.end(); ++it)
	{
		if((*it).m_iX == iX && (*it).m_iY == iY)
		{
			m_aPlotExtraYields.erase(it);
			break;
		}
	}

	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	if(NULL != pPlot)
	{
		pPlot->updateYield();
	}
}


int CvGame::getPlotExtraCost(int iX, int iY) const
{
	for(std::vector<PlotExtraCost>::const_iterator it = m_aPlotExtraCosts.begin(); it != m_aPlotExtraCosts.end(); ++it)
	{
		if((*it).m_iX == iX && (*it).m_iY == iY)
		{
			return (*it).m_iCost;
		}
	}

	return 0;
}


void CvGame::changePlotExtraCost(int iX, int iY, int iCost)
{
	bool bFound = false;

	for(std::vector<PlotExtraCost>::iterator it = m_aPlotExtraCosts.begin(); it != m_aPlotExtraCosts.end(); ++it)
	{
		if((*it).m_iX == iX && (*it).m_iY == iY)
		{
			(*it).m_iCost += iCost;
			bFound = true;
			break;
		}
	}

	if(!bFound)
	{
		PlotExtraCost kExtraCost;
		kExtraCost.m_iX = iX;
		kExtraCost.m_iY = iY;
		kExtraCost.m_iCost = iCost;
		m_aPlotExtraCosts.push_back(kExtraCost);
	}
}


void CvGame::removePlotExtraCost(int iX, int iY)
{
	for(std::vector<PlotExtraCost>::iterator it = m_aPlotExtraCosts.begin(); it != m_aPlotExtraCosts.end(); ++it)
	{
		if((*it).m_iX == iX && (*it).m_iY == iY)
		{
			m_aPlotExtraCosts.erase(it);
			break;
		}
	}
}






void CvGame::doUpdateCacheOnTurn()
{

}


bool CvGame::isCivEverActive(CivilizationTypes eCivilization) const
{
	for(int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if(kLoopPlayer.isEverAlive())
		{
			if(kLoopPlayer.getCivilizationType() == eCivilization)
			{
				return true;
			}
		}
	}

	return false;
}


bool CvGame::isLeaderEverActive(LeaderHeadTypes eLeader) const
{
	for(int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if(kLoopPlayer.isEverAlive())
		{
			if(kLoopPlayer.getLeaderType() == eLeader)
			{
				return true;
			}
		}
	}

	return false;
}


bool CvGame::isUnitEverActive(UnitTypes eUnit) const
{
	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	if(pkUnitInfo == NULL)
		return false;

#ifdef AUI_WARNING_FIXES
	for (uint iCiv = 0; iCiv < GC.getNumCivilizationInfos(); ++iCiv)
#else
	for(int iCiv = 0; iCiv < GC.getNumCivilizationInfos(); ++iCiv)
#endif
	{
		const CivilizationTypes eCiv = static_cast<CivilizationTypes>(iCiv);
		CvCivilizationInfo* pkCivilizationInfo = GC.getCivilizationInfo(eCiv);
		if(pkCivilizationInfo)
		{
			if(isCivEverActive(eCiv))
			{
				if(eUnit == pkCivilizationInfo->getCivilizationUnits(pkUnitInfo->GetUnitClassType()))
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool CvGame::isBuildingEverActive(BuildingTypes eBuilding) const
{
	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pkBuildingInfo)
	{
#ifdef AUI_WARNING_FIXES
		for (uint iCiv = 0; iCiv < GC.getNumCivilizationInfos(); ++iCiv)
#else
		for(int iCiv = 0; iCiv < GC.getNumCivilizationInfos(); ++iCiv)
#endif
		{
			const CivilizationTypes eCivilization = static_cast<CivilizationTypes>(iCiv);
			CvCivilizationInfo* pkCivilizationInfo = GC.getCivilizationInfo(eCivilization);
			if(pkCivilizationInfo)
			{
				if(isCivEverActive(eCivilization))
				{
					if(eBuilding == pkCivilizationInfo->getCivilizationBuildings(pkBuildingInfo->GetBuildingClassType()))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}



RouteTypes CvGame::GetIndustrialRoute() const
{
	return m_eIndustrialRoute;
}



void CvGame::DoUpdateIndustrialRoute()
{
	RouteTypes eIndustrialRoute = NO_ROUTE;

#ifdef AUI_WARNING_FIXES
	for (uint iRouteLoop = 0; iRouteLoop < GC.getNumRouteInfos(); iRouteLoop++)
#else
	for(int iRouteLoop = 0; iRouteLoop < GC.getNumRouteInfos(); iRouteLoop++)
#endif
	{
		const RouteTypes eRoute = static_cast<RouteTypes>(iRouteLoop);
		CvRouteInfo* pkRouteInfo = GC.getRouteInfo(eRoute);
		if(pkRouteInfo)
		{
			if(pkRouteInfo->IsIndustrial())
			{
				eIndustrialRoute = eRoute;
				break;
			}
		}
	}

	m_eIndustrialRoute = eIndustrialRoute;
}


CvSiteEvaluatorForSettler* CvGame::GetSettlerSiteEvaluator()
{
	return m_pSettlerSiteEvaluator;
}


CvSiteEvaluatorForStart* CvGame::GetStartSiteEvaluator()
{
	return m_pStartSiteEvaluator;
}


CvStartPositioner* CvGame::GetStartPositioner()
{
	return m_pStartPositioner;
}


CvGameDeals* CvGame::GetGameDeals()
{
	return &m_kGameDeals;
}


CvGameReligions* CvGame::GetGameReligions()
{
	return m_pGameReligions;
}


CvGameCulture* CvGame::GetGameCulture()
{
	return m_pGameCulture;
}


CvGameLeagues* CvGame::GetGameLeagues()
{
	return m_pGameLeagues;
}


CvGameTrade* CvGame::GetGameTrade()
{
	return m_pGameTrade;
}


CvTacticalAnalysisMap* CvGame::GetTacticalAnalysisMap()
{
	return m_pTacticalMap;
}

#ifdef MP_PLAYERS_VOTING_SYSTEM

CvMPVotingSystem* CvGame::GetMPVotingSystem()
{
	return m_pMPVotingSystem;
}

#endif

CvAdvisorCounsel* CvGame::GetAdvisorCounsel()
{
	return m_pAdvisorCounsel;
}


CvAdvisorRecommender* CvGame::GetAdvisorRecommender()
{
	return m_pAdvisorRecommender;
}


#if defined(AUI_WARNING_FIXES) || defined(AUI_CONSTIFY)
int CvGame::GetTurnsBetweenMinorCivElections() const
#else
int CvGame::GetTurnsBetweenMinorCivElections()
#endif
{
	int iTurnsBetweenCityStateElections = GC.getESPIONAGE_TURNS_BETWEEN_CITY_STATE_ELECTIONS();
	iTurnsBetweenCityStateElections *= GC.getGame().getGameSpeedInfo().getMinorCivElectionFreqMod();
	iTurnsBetweenCityStateElections /= 100;
	return iTurnsBetweenCityStateElections;
}


#if defined(AUI_WARNING_FIXES) || defined(AUI_CONSTIFY)
int CvGame::GetTurnsUntilMinorCivElection() const
#else
int CvGame::GetTurnsUntilMinorCivElection()
#endif
{
	int iMod = 0;
	int iTurnsBetweenCityStateElections = GetTurnsBetweenMinorCivElections();

	if(iTurnsBetweenCityStateElections != 0)
	{
		iMod = getGameTurn() % iTurnsBetweenCityStateElections;
	}
	if(iMod == 0)
	{
		return 0;
	}
	else
	{
		return iTurnsBetweenCityStateElections - iMod;
	}
}





int CvGame::GetAction(int iKeyStroke, bool bAlt, bool bShift, bool bCtrl)
{
#ifdef AUI_WARNING_FIXES
	uint i;
#else
	int i;
#endif
	int iActionIndex = -1;
	int iPriority = -1;


	for(i=0; i<GC.getNumActionInfos(); i++)
	{
		CvActionInfo& thisActionInfo = *GC.getActionInfo(i);
		if((((thisActionInfo.getHotKeyVal() == iKeyStroke) &&
		        (thisActionInfo.getHotKeyPriority() > iPriority) &&
		        (bAlt == thisActionInfo.isAltDown()) &&
		        (bShift == thisActionInfo.isShiftDown()) &&
		        (bCtrl == thisActionInfo.isCtrlDown())
		    )
		        ||
		        ((thisActionInfo.getHotKeyValAlt() == iKeyStroke) &&
		         (thisActionInfo.getHotKeyPriorityAlt() > iPriority) &&
		         (bAlt == thisActionInfo.isAltDownAlt()) &&
		         (bShift == thisActionInfo.isShiftDownAlt()) &&
		         (bCtrl == thisActionInfo.isCtrlDownAlt())
		        )
		   )
		        &&
		        (canHandleAction(i))
		  )
		{
			iPriority = thisActionInfo.getHotKeyPriority();
			iActionIndex = i;
		}
	}

	return iActionIndex;
}





int CvGame::IsAction(int iKeyStroke, bool bAlt, bool bShift, bool bCtrl)
{
#ifdef AUI_WARNING_FIXES
	uint i;
#else
	int i;
#endif
	int iActionIndex = -1;
	int iPriority = -1;


	for(i=0; i<GC.getNumActionInfos(); i++)
	{
		CvActionInfo& thisActionInfo = *GC.getActionInfo(i);
		if((((thisActionInfo.getHotKeyVal() == iKeyStroke) &&
			(thisActionInfo.getHotKeyPriority() > iPriority) &&
			(bAlt == thisActionInfo.isAltDown()) &&
			(bShift == thisActionInfo.isShiftDown()) &&
			(bCtrl == thisActionInfo.isCtrlDown())
			)
			||
			((thisActionInfo.getHotKeyValAlt() == iKeyStroke) &&
			(thisActionInfo.getHotKeyPriorityAlt() > iPriority) &&
			(bAlt == thisActionInfo.isAltDownAlt()) &&
			(bShift == thisActionInfo.isShiftDownAlt()) &&
			(bCtrl == thisActionInfo.isCtrlDownAlt())
			)
			)			
			)
		{
			iPriority = thisActionInfo.getHotKeyPriority();
			iActionIndex = i;
		}
	}

	return iActionIndex;
}


void CvGame::endTurnTimerSemaphoreIncrement()
{
	++m_endTurnTimerSemaphore;
}


void CvGame::endTurnTimerSemaphoreDecrement()
{
	--m_endTurnTimerSemaphore;
	if(m_endTurnTimerSemaphore <= 0)
	{
		m_endTurnTimerSemaphore = 0;
		m_endTurnTimer.Start();
	}
}


void CvGame::endTurnTimerReset()
{
	m_endTurnTimerSemaphore = 0;
	m_endTurnTimer.Start();
}



void CvGame::DoMinorPledgeProtection(PlayerTypes eMajor, PlayerTypes eMinor, bool bProtect, bool bPledgeNowBroken)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eMinor >= MAX_MAJOR_CIVS, "eMinor is not in expected range (invalid Index)");
	CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "eMinor is not in expected range (invalid Index)");

	if(bProtect)
	{
		CvAssertMsg(GET_PLAYER(eMinor).GetMinorCivAI()->CanMajorProtect(eMajor), "eMajor is not allowed to protect this minor! Please send Anton your save file and version.");
	}

	gDLL->sendMinorPledgeProtection(eMajor, eMinor, bProtect, bPledgeNowBroken);
}



void CvGame::DoMinorGiftGold(PlayerTypes eMinor, int iNumGold)
{
	CvAssertMsg(eMinor >= MAX_MAJOR_CIVS, "eMinor is not in expected range (invalid Index)");
	CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "eMinor is not in expected range (invalid Index)");

	gDLL->sendMinorGiftGold(eMinor, iNumGold);
}



void CvGame::DoMinorGiftTileImprovement(PlayerTypes eMajor, PlayerTypes eMinor, int iPlotX, int iPlotY)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eMinor >= MAX_MAJOR_CIVS, "eMinor is not in expected range (invalid Index)");
	CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "eMinor is not in expected range (invalid Index)");

	gDLL->sendMinorGiftTileImprovement(eMajor, eMinor, iPlotX, iPlotY);
}




void CvGame::DoMinorBullyGold(PlayerTypes eBully, PlayerTypes eMinor)
{
	CvAssertMsg(eBully >= 0, "eBully is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBully < MAX_MAJOR_CIVS, "eBully is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eMinor >= MAX_MAJOR_CIVS, "eMinor is not in expected range (invalid Index)");
	CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "eMinor is not in expected range (invalid Index)");

	int iGold = GET_PLAYER(eMinor).GetMinorCivAI()->GetBullyGoldAmount(eBully);

	gDLL->sendMinorBullyGold(eBully, eMinor, iGold);
}



void CvGame::DoMinorBullyUnit(PlayerTypes eBully, PlayerTypes eMinor)
{
	CvAssertMsg(eBully >= 0, "eBully is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBully < MAX_MAJOR_CIVS, "eBully is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eMinor >= MAX_MAJOR_CIVS, "eMinor is not in expected range (invalid Index)");
	CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "eMinor is not in expected range (invalid Index)");

	UnitTypes eUnitType = static_cast<UnitTypes>(GET_PLAYER(eBully).getCivilizationInfo().getCivilizationUnits(GC.getInfoTypeForString("UNITCLASS_WORKER")));

	gDLL->sendMinorBullyUnit(eBully, eMinor, eUnitType);
}



void CvGame::DoMinorBuyout(PlayerTypes eMajor, PlayerTypes eMinor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eMinor >= MAX_MAJOR_CIVS, "eMinor is not in expected range (invalid Index)");
	CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "eMinor is not in expected range (invalid Index)");

	gDLL->sendMinorBuyout(eMajor, eMinor);
}





void CvGame::DoResearchAgreementNotification(TeamTypes eTeam1, TeamTypes eTeam2)
{

	for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
		PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
		CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
		TeamTypes eCurNotifyTeam = kCurNotifyPlayer.getTeam();


		if(eCurNotifyTeam != eTeam1 && eCurNotifyTeam != eTeam2)
		{
			CvTeam* pCurTeam = &GET_TEAM(eCurNotifyTeam);

			if(pCurTeam->isHasMet(eTeam1) && pCurTeam->isHasMet(eTeam2))
			{
				CvNotifications* pNotifications = kCurNotifyPlayer.GetNotifications();
				if(pNotifications)
				{
					const char* strLeaderName = GET_PLAYER(GET_TEAM(eTeam1).getLeaderID()).getCivilizationShortDescriptionKey();
					const char* strOtherLeaderName = GET_PLAYER(GET_TEAM(eTeam2).getLeaderID()).getCivilizationShortDescriptionKey();

					Localization::String strText;
					strText = Localization::Lookup("TXT_KEY_NOTIFICATION_RESEARCH_AGREEMENT");
					strText << strLeaderName << strOtherLeaderName;

					Localization::String strSummary;
					strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_RESEARCH_AGREEMENT");

					pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
				}
			}
		}
	}
}


int CvGame::GetResearchAgreementCost(PlayerTypes ePlayer1, PlayerTypes ePlayer2)
{
	CvAssertMsg(ePlayer1 > NO_PLAYER, "Invalid player. Please show Jon this.");
	CvAssertMsg(ePlayer1 <= MAX_MAJOR_CIVS, "Invalid player. Please show Jon this.");
	CvAssertMsg(ePlayer2 > NO_PLAYER, "Invalid player. Please show Jon this.");
	CvAssertMsg(ePlayer2 <= MAX_MAJOR_CIVS, "Invalid player. Please show Jon this.");

	EraTypes ePlayer1Era = GET_TEAM(GET_PLAYER(ePlayer1).getTeam()).GetCurrentEra();
	EraTypes ePlayer2Era = GET_TEAM(GET_PLAYER(ePlayer2).getTeam()).GetCurrentEra();
	EraTypes eHighestEra = max(ePlayer1Era, ePlayer2Era);

	int iCost = GC.getEraInfo(eHighestEra)->getResearchAgreementCost();

	iCost *= getGameSpeedInfo().getGoldPercent();
	iCost /= 100;

	return iCost;
}





void CvGame::DoTestConquestVictory()
{
	TeamTypes eTeamWhoWon = NO_TEAM;

	PlayerTypes eLoopPlayer;


	int iNumOriginalCapitals = 0;

	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if(GET_PLAYER(eLoopPlayer).isAlive())
		{
			int iCityLoop;
			CvCity* pLoopCity = NULL;
			for(pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iCityLoop))
			{
				if (pLoopCity->getOriginalOwner() < MAX_MAJOR_CIVS && pLoopCity->IsOriginalCapital())
				{
					iNumOriginalCapitals += 1;
				}
			}
		}
	}

	CvAssertMsg(iNumOriginalCapitals > 0, "No one controls an original capital. This is wrong.");


	for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
	{
		int iNumCapitalsControlled = 0;
		if(GET_TEAM((TeamTypes)iTeamLoop).isAlive())
		{
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				if(GET_PLAYER(eLoopPlayer).isAlive() && GET_PLAYER(eLoopPlayer).getTeam() == (TeamTypes)iTeamLoop)
				{
					int iCityLoop;
					CvCity* pLoopCity = NULL;
					for(pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iCityLoop))
					{
						if (pLoopCity->getOriginalOwner() < MAX_MAJOR_CIVS && pLoopCity->IsOriginalCapital())
						{
							iNumCapitalsControlled += 1;
						}
					}
				}
			}

			if (iNumCapitalsControlled == iNumOriginalCapitals)
			{
				eTeamWhoWon = (TeamTypes)iTeamLoop;
				break;
			}
		}
	}


	if (eTeamWhoWon != NO_TEAM)
	{
#ifdef AUI_WARNING_FIXES
		for (uint iVictoryLoop = 0; iVictoryLoop < GC.getNumVictoryInfos(); iVictoryLoop++)
#else
		for(int iVictoryLoop = 0; iVictoryLoop < GC.getNumVictoryInfos(); iVictoryLoop++)
#endif
		{
			VictoryTypes eVictory = static_cast<VictoryTypes>(iVictoryLoop);
			CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eVictory);
			if(pkVictoryInfo)
			{
				if(pkVictoryInfo->isConquest() && isVictoryValid(eVictory))
				{
					setWinner(eTeamWhoWon, eVictory);
				}
			}
		}
	}
}



PlayerTypes CvGame::GetBestWondersPlayer()
{
	return m_eBestWondersPlayer;
}



void CvGame::SetBestWondersPlayer(PlayerTypes ePlayer, int iWonderCount)
{
	int iVictoryPointChange =       GC.getZERO_SUM_COMPETITION_WONDERS_VICTORY_POINTS();


	if(GetBestWondersPlayer() != NO_PLAYER)
	{
		GET_TEAM(GET_PLAYER(GetBestWondersPlayer()).getTeam()).changeVictoryPoints(-iVictoryPointChange);
	}

	m_eBestWondersPlayer = ePlayer;

	if(ePlayer != NO_PLAYER)
	{
		GET_TEAM(GET_PLAYER(ePlayer).getTeam()).changeVictoryPoints(iVictoryPointChange);


		for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
			PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
			CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
			TeamTypes eCurNotifyTeam = kCurNotifyPlayer.getTeam();

			CvNotifications* pNotifications = kCurNotifyPlayer.GetNotifications();
			if(pNotifications)
			{
				CvString strBuffer;
				CvString strSummary;


				if(kCurNotifyPlayer.GetID() == ePlayer)
				{
					strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BEST_IN_WONDERS_YOU", iVictoryPointChange, iWonderCount+1);
					strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BEST_IN_WONDERS_YOU", iVictoryPointChange, iWonderCount+1);
				}

				else if(!GET_TEAM(eCurNotifyTeam).isHasMet(GET_PLAYER(ePlayer).getTeam()))
				{
					strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BEST_IN_WONDERS_UNMET", iVictoryPointChange, iWonderCount+1);
					strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BEST_IN_WONDERS_UNMET", iVictoryPointChange, iWonderCount+1);
				}

				else
				{
					strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BEST_IN_WONDERS_ANOTHER", GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey(), iVictoryPointChange, iWonderCount+1);
					strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BEST_IN_WONDERS_ANOTHER", GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey());
				}

				pNotifications->Add(NOTIFICATION_VICTORY, strBuffer, strSummary, -1, -1, -1);
			}
		}
	}
}



PlayerTypes CvGame::GetBestPoliciesPlayer()
{
	return m_eBestPoliciesPlayer;
}



void CvGame::SetBestPoliciesPlayer(PlayerTypes ePlayer, int iPolicyCount)
{
	int iVictoryPointChange =       GC.getZERO_SUM_COMPETITION_POLICIES_VICTORY_POINTS();


	if(GetBestPoliciesPlayer() != NO_PLAYER)
	{
		GET_TEAM(GET_PLAYER(GetBestPoliciesPlayer()).getTeam()).changeVictoryPoints(-iVictoryPointChange);
	}

	m_eBestPoliciesPlayer = ePlayer;

	if(ePlayer != NO_PLAYER)
	{
		CvString strBuffer;
		CvString strSummary;

		GET_TEAM(GET_PLAYER(ePlayer).getTeam()).changeVictoryPoints(iVictoryPointChange);


		for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
			PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
			CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
			TeamTypes eCurNotifyTeam = kCurNotifyPlayer.getTeam();


			if(eNotifyPlayer == ePlayer)
			{
				strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BEST_IN_POLICIES_YOU", iVictoryPointChange, iPolicyCount+1);
				strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BEST_IN_POLICIES_YOU", iVictoryPointChange, iPolicyCount+1);
			}

			else if(!GET_TEAM(eCurNotifyTeam).isHasMet(GET_PLAYER(ePlayer).getTeam()))
			{
				strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BEST_IN_POLICIES_UNMET", iVictoryPointChange, iPolicyCount+1);
				strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BEST_IN_POLICIES_UNMET", iVictoryPointChange, iPolicyCount+1);
			}

			else
			{
				strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BEST_IN_POLICIES_ANOTHER", GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey(), iVictoryPointChange, iPolicyCount+1);
				strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BEST_IN_POLICIES_ANOTHER", GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey(), iVictoryPointChange, iPolicyCount+1);
			}

			CvNotifications* pNotifications = kCurNotifyPlayer.GetNotifications();
			if(pNotifications)
			{
				pNotifications->Add(NOTIFICATION_VICTORY, strBuffer, strSummary, -1, -1, -1);
			}
		}
	}
}



PlayerTypes CvGame::GetBestGreatPeoplePlayer()
{
	return m_eBestGreatPeoplePlayer;
}



void CvGame::SetBestGreatPeoplePlayer(PlayerTypes ePlayer, int iGreatPeopleCount)
{
	int iVictoryPointChange =       GC.getZERO_SUM_COMPETITION_GREAT_PEOPLE_VICTORY_POINTS();


	if(GetBestGreatPeoplePlayer() != NO_PLAYER)
	{
		GET_TEAM(GET_PLAYER(GetBestGreatPeoplePlayer()).getTeam()).changeVictoryPoints(-iVictoryPointChange);
	}

	m_eBestGreatPeoplePlayer = ePlayer;

	if(ePlayer != NO_PLAYER)
	{
		GET_TEAM(GET_PLAYER(ePlayer).getTeam()).changeVictoryPoints(iVictoryPointChange);

		for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
			PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
			CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
			TeamTypes eCurNotifyTeam = kCurNotifyPlayer.getTeam();

			CvNotifications* pNotifications =kCurNotifyPlayer.GetNotifications();
			if(pNotifications)
			{
				CvString strBuffer;
				CvString strSummary;


				if(eNotifyPlayer == ePlayer)
				{
					strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BEST_IN_GREAT_PEOPLE_YOU", iVictoryPointChange, iGreatPeopleCount+1);
					strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BEST_IN_GREAT_PEOPLE_YOU", iVictoryPointChange, iGreatPeopleCount+1);
				}

				else if(!GET_TEAM(eCurNotifyTeam).isHasMet(GET_PLAYER(ePlayer).getTeam()))
				{
					strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BEST_IN_GREAT_PEOPLE_UNMET", iVictoryPointChange, iGreatPeopleCount+1);
					strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BEST_IN_GREAT_PEOPLE_UNMET", iVictoryPointChange, iGreatPeopleCount+1);
				}

				else
				{
					strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BEST_IN_GREAT_PEOPLE_ANOTHER", GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey(), iVictoryPointChange, iGreatPeopleCount+1);
					strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BEST_IN_GREAT_PEOPLE_ANOTHER", GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey(), iVictoryPointChange, iGreatPeopleCount+1);
				}

				pNotifications->Add(NOTIFICATION_VICTORY, strBuffer, strSummary, -1, -1, -1);
			}
		}
	}
}



bool CvGame::IsEndGameTechResearched() const
{
	return m_bEndGameTechResearched;
}



void CvGame::SetEndGameTechResearched(bool bValue)
{
	m_bEndGameTechResearched = bValue;
}


bool CvGame::TunerEverConnected() const
{
	return m_bTunerEverConnected || gDLL->TunerEverConnected();
}


bool CvGame::IsEverAttackedTutorial() const
{
	return m_bTutorialEverAttacked;
}


void CvGame::SetEverAttackedTutorial(bool bValue)
{
	m_bTutorialEverAttacked = bValue;
}


bool CvGame::IsEverRightClickMoved() const
{
	return m_bEverRightClickMoved;
}


void CvGame::SetEverRightClickMoved(bool bValue)
{
	m_bEverRightClickMoved = bValue;
}


bool CvGame::IsCombatWarned() const
{
	return m_bCombatWarned;
}


void CvGame::SetCombatWarned(bool bValue)
{
	m_bCombatWarned = bValue;
}



void CvGame::BuildProdModHelpText(CvString* toolTipSink, const char* strTextKey, int iMod, const char* strExtraKey) const
{
	if(iMod != 0 && toolTipSink != NULL)
	{
		Localization::String localizedText = Localization::Lookup(strTextKey);
		localizedText << iMod;

		if(strExtraKey)
		{
			std::string extraKey(strExtraKey);
			if(!extraKey.empty())
				localizedText << strExtraKey;

			const char* const localized = localizedText.toUTF8();
			if(localized)
				(*toolTipSink) += localized;
		}
	}
}



void CvGame::BuildCannotPerformActionHelpText(CvString* toolTipSink, const char* strTextKey, const char* strExtraKey1, const char* strExtraKey2, int iValue) const
{
	if(toolTipSink != NULL)
	{
		Localization::String localizedText = Localization::Lookup(strTextKey);

		if(iValue != -666)
			localizedText << iValue;

		if(strExtraKey1)
		{
			std::string extraKey1(strExtraKey1);
			if(!extraKey1.empty())
				localizedText << strExtraKey1;
		}

		if(strExtraKey2)
		{
			std::string extraKey2(strExtraKey2);
			if(!extraKey2.empty())
				localizedText << strExtraKey2;
		}

		const char* const localized = localizedText.toUTF8();
		if(localized)
			(*toolTipSink) += localized;
	}
}


void CvGame::LogGameState(bool bLogHeaders)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutput;

		CvString playerName;
		CvString otherPlayerName;
		CvString strMinorString;
		CvString strDesc;
		CvString strLogName;
		CvString strTemp;

		strLogName = "WorldState_Log.csv";

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strOutput.Format("%03d", GC.getGame().getElapsedGameTurns());

		AIGrandStrategyTypes eGrandStrategy;
		int iGSConquest = 0;
		int iGSSpaceship = 0;
		int iGSUN = 0;
		int iGSCulture = 0;

		int iAlly = 0;
		int iFriend = 0;
		int iFavorable = 0;
		int iNeutral = 0;
		int iCompetitor = 0;
		int iEnemy = 0;
		int iUnforgivable = 0;

		int iMajorWar = 0;
		int iMajorHostile = 0;
		int iMajorDeceptive = 0;
		int iMajorGuarded = 0;
		int iMajorAfraid = 0;
		int iMajorFriendly = 0;
		int iMajorNeutral = 0;

		int iMinorIgnore = 0;
		int iMinorFriendly = 0;
		int iMinorProtective = 0;
		int iMinorConquest = 0;
		int iMinorBully = 0;

		int iPlayerLoop2;
		PlayerTypes eLoopPlayer2;


		PlayerTypes eLoopPlayer;
		CvPlayer* pPlayer;
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;
			pPlayer = &GET_PLAYER(eLoopPlayer);

			if(pPlayer->isAlive())
			{
				eGrandStrategy = pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy();

				if(eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST"))
				{
					iGSConquest++;
				}
				else if(eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_SPACESHIP"))
				{
					iGSSpaceship++;
				}
				else if(eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_UNITED_NATIONS"))
				{
					iGSUN++;
				}
				else if(eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE"))
				{
					iGSCulture++;
				}


				for(iPlayerLoop2 = 0; iPlayerLoop2 < MAX_CIV_PLAYERS; iPlayerLoop2++)
				{
					eLoopPlayer2 = (PlayerTypes) iPlayerLoop2;

					if(GET_PLAYER(eLoopPlayer2).isAlive())
					{

						if(eLoopPlayer2 < MAX_MAJOR_CIVS)
						{
							switch(pPlayer->GetDiplomacyAI()->GetMajorCivOpinion(eLoopPlayer2))
							{
							case MAJOR_CIV_OPINION_ALLY:
								iAlly++;
								break;
							case MAJOR_CIV_OPINION_FRIEND:
								iFriend++;
								break;
							case MAJOR_CIV_OPINION_FAVORABLE:
								iFavorable++;
								break;
							case MAJOR_CIV_OPINION_NEUTRAL:
								iNeutral++;
								break;
							case MAJOR_CIV_OPINION_COMPETITOR:
								iCompetitor++;
								break;
							case MAJOR_CIV_OPINION_ENEMY:
								iEnemy++;
								break;
							case MAJOR_CIV_OPINION_UNFORGIVABLE:
								iUnforgivable++;
								break;
							}

							switch(pPlayer->GetDiplomacyAI()->GetMajorCivApproach(eLoopPlayer2, false))
							{
							case MAJOR_CIV_APPROACH_WAR:
								iMajorWar++;
								break;
							case MAJOR_CIV_APPROACH_HOSTILE:
								iMajorHostile++;
								break;
							case MAJOR_CIV_APPROACH_DECEPTIVE:
								iMajorDeceptive++;
								break;
							case MAJOR_CIV_APPROACH_GUARDED:
								iMajorGuarded++;
								break;
							case MAJOR_CIV_APPROACH_AFRAID:
								iMajorAfraid++;
								break;
							case MAJOR_CIV_APPROACH_FRIENDLY:
								iMajorFriendly++;
								break;
							case MAJOR_CIV_APPROACH_NEUTRAL:
								iMajorNeutral++;
								break;
							}
						}

						else
						{
							switch(pPlayer->GetDiplomacyAI()->GetMinorCivApproach(eLoopPlayer2))
							{
							case MINOR_CIV_APPROACH_IGNORE:
								iMinorIgnore++;
								break;
							case MINOR_CIV_APPROACH_FRIENDLY:
								iMinorFriendly++;
								break;
							case MINOR_CIV_APPROACH_PROTECTIVE:
								iMinorProtective++;
								break;
							case MINOR_CIV_APPROACH_CONQUEST:
								iMinorConquest++;
								break;
							case MINOR_CIV_APPROACH_BULLY:
								iMinorBully++;
								break;
							}
						}
					}
				}
			}
		}

		bool bFirstTurn = bLogHeaders || getElapsedGameTurns() == 0;


		if(bFirstTurn)
		{
			strOutput += ", Conquest";
			strOutput += ", Spaceship";
			strOutput += ", United Nations";
			strOutput += ", Culture";
		}
		else
		{
			strTemp.Format("%d", iGSConquest);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iGSSpaceship);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iGSUN);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iGSCulture);
			strOutput += ", " + strTemp;
		}

		strOutput += ", ";


		if(bFirstTurn)
		{
			strOutput += ", Ally";
			strOutput += ", Friend";
			strOutput += ", Favorable";
			strOutput += ", Neutral";
			strOutput += ", Competitor";
			strOutput += ", Enemy";
			strOutput += ", Unforgivable";
		}
		else
		{
			strTemp.Format("%d", iAlly);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iFriend);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iFavorable);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iNeutral);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iCompetitor);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iEnemy);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iUnforgivable);
			strOutput += ", " + strTemp;
		}

		strOutput += ", ";


		if(bFirstTurn)
		{
			strOutput += ", War";
			strOutput += ", Hostile";
			strOutput += ", Deceptive";
			strOutput += ", Guarded";
			strOutput += ", Afraid";
			strOutput += ", Friendly";
			strOutput += ", Neutral";
		}
		else
		{
			strTemp.Format("%d", iMajorWar);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iMajorHostile);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iMajorDeceptive);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iMajorGuarded);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iMajorAfraid);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iMajorFriendly);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iMajorNeutral);
			strOutput += ", " + strTemp;
		}

		strOutput += ", ";


		if(bFirstTurn)
		{
			strOutput += ", Ignore";
			strOutput += ", Friendly";
			strOutput += ", Protective";
			strOutput += ", Conquest";
			strOutput += ", Bully";
		}
		else
		{
			strTemp.Format("%d", iMinorIgnore);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iMinorFriendly);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iMinorProtective);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iMinorConquest);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iMinorBully);
			strOutput += ", " + strTemp;
		}

		pLog->Msg(strOutput);
	}
}


void CvGame::unitIsMoving()
{
	s_unitMoveTurnSlice = getTurnSlice();
}


bool CvGame::allUnitAIProcessed() const
{
	int i = 0;
	for(i = 0; i < MAX_PLAYERS; i++)
	{
		const CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));
		if(player.isTurnActive() && player.hasUnitsThatNeedAIUpdate())
			return false;
	}
	return true;
}



int CvGame::GetDealDuration()
{
	return getGameSpeedInfo().GetDealDuration();
}


int CvGame::GetPeaceDuration()
{
	return getGameSpeedInfo().getPeaceDealDuration();
}


bool CvGame::IsArchaeologyTriggered() const
{
	return m_bArchaeologyTriggered;
}


void CvGame::TriggerArchaeologySiteCreation(bool bCheckInitialized)
{
	if (!m_bArchaeologyTriggered)
	{
		if (!bCheckInitialized || isFinalInitialized())
		{
			SpawnArchaeologySitesHistorically();
			m_bArchaeologyTriggered = true;
		}
	}
}



int CalculateDigSiteWeight(int iIndex, FFastVector<CvArchaeologyData, true, c_eCiv5GameplayDLL, 0>& inputData, FFastVector<CvArchaeologyData, true, c_eCiv5GameplayDLL, 0>& chosenDigSites)
{
	CvMap& theMap = GC.getMap();
	int iGridWidth = theMap.getGridWidth();
	int iBaseWeight = 0;
	if (chosenDigSites[iIndex].m_eArtifactType == NO_GREAT_WORK_ARTIFACT_CLASS)
	{
		iBaseWeight = inputData[iIndex].m_eArtifactType + 1;
		iBaseWeight *= (10 - inputData[iIndex].m_eEra);

		int iPlotX = iIndex % iGridWidth;
		int iPlotY = iIndex / iGridWidth;

		CvPlot* pPlot = theMap.plotByIndexUnchecked(iIndex);


		if (pPlot->getResourceType() != NO_RESOURCE || pPlot->isWater() || pPlot->getFeatureType() == FEATURE_ICE || pPlot->isMountain() || pPlot->IsNaturalWonder())
			iBaseWeight = 0;


		if (iBaseWeight && pPlot->getFeatureType() != NO_FEATURE)
		{
			if (GC.getFeatureInfo(pPlot->getFeatureType())->isNoImprovement())
			{
				iBaseWeight = 0;
			}
		}


		if (iBaseWeight && pPlot->getImprovementType() != NO_IMPROVEMENT)
		{
			if (GC.getImprovementInfo(pPlot->getImprovementType())->IsCreatedByGreatPerson())
			{
				iBaseWeight = 0;
			}
		}

		if (iBaseWeight > 0)
		{

			iBaseWeight += 10 + GC.getGame().getJonRandNum(10, "random factor on dig sites");


			iBaseWeight *= (pPlot->getOwner() == NO_PLAYER) ? 9 : 8;
			iBaseWeight /= 8;


			iBaseWeight *= (pPlot->getOwner() > NO_PLAYER && pPlot->getOwner() < MAX_MAJOR_CIVS) ? 11 : 12;
			iBaseWeight /= 12;


			iBaseWeight *= (pPlot->getImprovementType() != NO_IMPROVEMENT || pPlot->getRouteType() != NO_ROUTE) ? 7 : 8;
			iBaseWeight /= 8;


			iBaseWeight *= (pPlot->isCity()) ? 1 : 5;
			iBaseWeight /= 5;


			iBaseWeight *= (pPlot->getTerrainType() == TERRAIN_DESERT) ? 3 : 2;
			iBaseWeight *= (pPlot->getFeatureType() == FEATURE_JUNGLE) ? 3 : 2;
			CvArea* pArea = theMap.getArea(pPlot->getArea());
			iBaseWeight *= (pArea->getNumTiles() <= 4) ? 3 : 2;


			int iDivisor = 1;

			int iRange = 3;
#ifdef AUI_HEXSPACE_DX_LOOPS
			int iMaxDX, iDX;
			CvPlot* pLoopPlot;
			for (int iDY = -iRange; iDY <= iRange; iDY++)
			{
				iMaxDX = iRange - MAX(0, iDY);
				for (iDX = -iRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
				{

					pLoopPlot = plotXY(iPlotX, iPlotY, iDX, iDY);

					if (pLoopPlot)
					{
						if (chosenDigSites[pLoopPlot->GetPlotIndex()].m_eArtifactType != NO_GREAT_WORK_ARTIFACT_CLASS)
						{
							if (hexDistance(iDX, iDY) == 1)
							{
								iDivisor += 3;
							}
							else if (hexDistance(iDX, iDY) == 2)
							{
								iDivisor += 2;
							}
							else
							{
								iDivisor++;
							}
#else
			for (int iDX = -iRange; iDX <= iRange; iDX++)
			{
				for (int iDY = -iRange; iDY <= iRange; iDY++)
				{
					CvPlot* pLoopPlot = plotXYWithRangeCheck(iPlotX, iPlotY, iDX, iDY, iRange);
					if (pLoopPlot)
					{
						if (chosenDigSites[pLoopPlot->GetPlotIndex()].m_eArtifactType != NO_GREAT_WORK_ARTIFACT_CLASS)
						{
							iDivisor++;
						}
					}
				}
			}
			iRange = 2;
			for (int iDX = -iRange; iDX <= iRange; iDX++)
			{
				for (int iDY = -iRange; iDY <= iRange; iDY++)
				{
					CvPlot* pLoopPlot = plotXYWithRangeCheck(iPlotX, iPlotY, iDX, iDY, iRange);
					if (pLoopPlot)
					{
						if (chosenDigSites[pLoopPlot->GetPlotIndex()].m_eArtifactType != NO_GREAT_WORK_ARTIFACT_CLASS)
						{
							iDivisor++;
						}
					}
				}
			}
			iRange = 1;
			for (int iDX = -iRange; iDX <= iRange; iDX++)
			{
				for (int iDY = -iRange; iDY <= iRange; iDY++)
				{
					CvPlot* pLoopPlot = plotXYWithRangeCheck(iPlotX, iPlotY, iDX, iDY, iRange);
					if (pLoopPlot)
					{
						if (chosenDigSites[pLoopPlot->GetPlotIndex()].m_eArtifactType != NO_GREAT_WORK_ARTIFACT_CLASS)
						{
							iDivisor++;
#endif
						}
					}
				}
			}
			iBaseWeight /= iDivisor;
		}
	}
	return iBaseWeight;
}



void CalculateDigSiteWeights(int iGridSize, FFastVector<CvArchaeologyData, true, c_eCiv5GameplayDLL, 0>& inputData, FFastVector<CvArchaeologyData, true, c_eCiv5GameplayDLL, 0>& chosenDigSites, FFastVector<int, true, c_eCiv5GameplayDLL, 0>& currentWeights)
{
	CvAssertMsg(NO_GREAT_WORK_ARTIFACT_CLASS == 0, "Value of NO_ARTIFACT has changed");
	for (int i = 0; i < iGridSize; i++)
	{
		currentWeights[i] = CalculateDigSiteWeight(i, inputData, chosenDigSites);
	}
}


int CvGame::GetNumArchaeologySites() const
{
	if (!IsArchaeologyTriggered())
	{
		return -1;
	}

	int iRtnValue = 0;
#ifdef AUI_WARNING_FIXES
	uint iPlotLoop;
#else
	int iPlotLoop;
#endif
	CvPlot *pPlot;
	for (iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);
		if (pPlot->getResourceType() == GC.getARTIFACT_RESOURCE())
		{
			iRtnValue++;
		}
	}
	return iRtnValue;
}

int CvGame::GetNumHiddenArchaeologySites() const
{
	if (!IsArchaeologyTriggered())
	{
		return -1;
	}

	int iRtnValue = 0;
#ifdef AUI_WARNING_FIXES
	uint iPlotLoop;
#else
	int iPlotLoop;
#endif
	CvPlot *pPlot;
	for (iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);
		if (pPlot->getResourceType() == GC.getHIDDEN_ARTIFACT_RESOURCE())
		{
			iRtnValue++;
		}
	}
	return iRtnValue;
}
#if defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
void CvGame::getCombatDamage(CvCombatInfo& kInfo)
{
	int iMaxHP = GC.getMAX_HIT_POINTS();





	if (kInfo.getUnit(BATTLE_UNIT_ATTACKER) != NULL)
	{
		CvUnit& attacker = *kInfo.getUnit(BATTLE_UNIT_ATTACKER);
		int iAttackerStrength = attacker.GetMaxAttackStrength(kInfo);
		int iDefenderStrength = 0, iDamage = 0, iWoundedRatio = 0;

		if (kInfo.getUnit(BATTLE_UNIT_INTERCEPTOR) != NULL)
		{
			const CvUnit& interceptor = *kInfo.getUnit(BATTLE_UNIT_INTERCEPTOR);
			int iInterceptorStrength = interceptor.GetMaxDefenseStrength(kInfo);
			iWoundedRatio = interceptor.getWoundedRatio(kInfo.getExtraDamageTaken(BATTLE_UNIT_INTERCEPTOR));
			iDamage = GC.getINTERCEPTION_SAME_STRENGTH_MIN_DAMAGE() * iWoundedRatio / iMaxHP;

			kInfo.doRandomness(BATTLE_UNIT_INTERCEPTOR, iWoundedRatio);
			iDamage += kInfo.getCombatSeed(BATTLE_UNIT_INTERCEPTOR);

			double fStrengthRatio = kInfo.doStrengthRatio(iInterceptorStrength, iAttackerStrength);
			iDamage = static_cast<int>(iDamage * fStrengthRatio);


			iDamage *= (100 + attacker.GetInterceptionDefenseDamageModifier());
			iDamage /= 10000;


			iDamage = range(iDamage, 1, iMaxHP - 1);
			kInfo.setDamageInflicted(BATTLE_UNIT_INTERCEPTOR, iDamage);

		}
		else if (kInfo.getCity(BATTLE_UNIT_INTERCEPTOR) != NULL)
		{
			const CvCity& interceptor = *kInfo.getCity(BATTLE_UNIT_INTERCEPTOR);
			const int iInterceptorStrength = interceptor.getStrengthValue();
			iDamage = GC.getINTERCEPTION_SAME_STRENGTH_MIN_DAMAGE();
			iWoundedRatio = iMaxHP;

			kInfo.doRandomness(BATTLE_UNIT_INTERCEPTOR, iWoundedRatio);
			iDamage += kInfo.getCombatSeed(BATTLE_UNIT_INTERCEPTOR);

			double fStrengthRatio = kInfo.doStrengthRatio(iInterceptorStrength, iAttackerStrength);
			iDamage = static_cast<int>(iDamage * fStrengthRatio);
			iDamage /= 100;

			iDamage = range(iDamage, GC.getMIN_CITY_STRIKE_DAMAGE(), iMaxHP - 1);
			kInfo.setDamageInflicted(BATTLE_UNIT_INTERCEPTOR, iDamage);
		}
		int iExtraDamage = kInfo.getExtraDamageTaken(BATTLE_UNIT_ATTACKER) + kInfo.getDamageInflicted(BATTLE_UNIT_INTERCEPTOR);
		if (kInfo.getUnit(BATTLE_UNIT_DEFENDER) != NULL)
		{
			CvUnit& defender = *kInfo.getUnit(BATTLE_UNIT_DEFENDER);
			iDefenderStrength = defender.GetMaxDefenseStrength(kInfo);
			int iDamage = 0;
			std::pair<int, int> damagePair(-1, -1);
			if (kInfo.getAttackIsRanged() && !defender.IsCanDefend())
			{
				damagePair.first = GC.getNONCOMBAT_UNIT_RANGED_DAMAGE();
			}
			const bool bRangedAttack = kInfo.getAttackIsRanged() || kInfo.getAttackIsBombingMission();
			damagePair.second = kInfo.getDefenderRetaliates() ? -1 : 0;
			do
			{
				if (damagePair.first != -1 && damagePair.second != -1)
					break;
				const bool bAttacker = (damagePair.first == -1);
				
				CvUnit& unit		= bAttacker ? attacker : defender;


				const int iUnitStrength		= bAttacker ? iAttackerStrength : iDefenderStrength;
				const int iOpponentStrength = bAttacker ? iDefenderStrength : iAttackerStrength;

				const int iWoundedRatio = unit.getWoundedRatio(bAttacker ? iExtraDamage : kInfo.getExtraDamageTaken(BATTLE_UNIT_DEFENDER));
				const int iAttackSameStrengthMinDamage = bRangedAttack ?         GC.getRANGE_ATTACK_SAME_STRENGTH_MIN_DAMAGE() :         GC.getATTACK_SAME_STRENGTH_MIN_DAMAGE();

				iDamage = iAttackSameStrengthMinDamage * iWoundedRatio / iMaxHP;
				kInfo.doRandomness(bAttacker ? BATTLE_UNIT_ATTACKER : BATTLE_UNIT_DEFENDER, iWoundedRatio);
				iDamage += kInfo.getCombatSeed(bAttacker ? BATTLE_UNIT_ATTACKER : BATTLE_UNIT_DEFENDER);
				double fStrengthRatio = kInfo.doStrengthRatio(iUnitStrength, iOpponentStrength);
				iDamage = static_cast<int>(iDamage * fStrengthRatio);
				iDamage /= 100;
				
				iDamage = std::max(1, iDamage);

				bAttacker ? damagePair.first = iDamage : damagePair.second = iDamage;
			} while (damagePair.first == -1 || damagePair.second == -1);
			int iAttackerDamageDealt = damagePair.first;
			int iDefenderDamageDealt = damagePair.second;

			if (iDefenderDamageDealt < 0)
			{
				iDefenderDamageDealt = 0;
			}

			if (kInfo.getAttackIsAirSweep() && defender.getDomainType() != DOMAIN_AIR)
			{
				iAttackerDamageDealt *=       GC.getAIR_SWEEP_INTERCEPTION_DAMAGE_MOD();
				iAttackerDamageDealt /= 100;
				iAttackerDamageDealt = std::max(0, iAttackerDamageDealt);
				iDefenderDamageDealt *=       GC.getAIR_SWEEP_INTERCEPTION_DAMAGE_MOD();
				iDefenderDamageDealt /= 100;
				iDefenderDamageDealt = std::max(0, iDefenderDamageDealt);
			}
			
			int iDefenderFinalDamage = iAttackerDamageDealt + defender.getDamage();
			int iAttackerFinalDamage = iDefenderDamageDealt + attacker.getDamage() + kInfo.getDamageInflicted(BATTLE_UNIT_INTERCEPTOR);

			if (kInfo.getDefenderRetaliates() && iDefenderFinalDamage >= iMaxHP && iAttackerFinalDamage >= iMaxHP)
			{

				const bool bAttackerCanStillSurvive = attacker.getDamage() + kInfo.getDamageInflicted(BATTLE_UNIT_INTERCEPTOR) < iMaxHP;
				if (iDefenderFinalDamage > iAttackerFinalDamage && bAttackerCanStillSurvive)
				{
					iDefenderDamageDealt = iMaxHP - attacker.getDamage() - kInfo.getDamageInflicted(BATTLE_UNIT_INTERCEPTOR) - 1;
					iAttackerFinalDamage = iMaxHP - 1;
					iDefenderFinalDamage = iMaxHP;
				}
				else
				{
					iAttackerDamageDealt = iMaxHP - defender.getDamage() - 1;
					iDefenderFinalDamage = iMaxHP - 1;
					iAttackerFinalDamage = iMaxHP;
				}
			}

			if (bRangedAttack && iDefenderFinalDamage >= iMaxHP)
			{
				iAttackerDamageDealt = defender.GetMaxHitPoints() - defender.getDamage();
				iDefenderFinalDamage = iMaxHP;
			}
			if (iDefenderFinalDamage > iMaxHP)
			{
				iAttackerDamageDealt = iMaxHP - defender.getDamage();
				iDefenderFinalDamage = iMaxHP;
			}
			if (iAttackerFinalDamage > iMaxHP)
			{
				iDefenderDamageDealt = iMaxHP - attacker.getDamage() - kInfo.getDamageInflicted(BATTLE_UNIT_INTERCEPTOR);
				iAttackerFinalDamage = iMaxHP;
			}

			kInfo.setFinalDamage(BATTLE_UNIT_ATTACKER, iAttackerFinalDamage);
			kInfo.setDamageInflicted(BATTLE_UNIT_ATTACKER, iAttackerDamageDealt);
			kInfo.setFinalDamage(BATTLE_UNIT_DEFENDER, iDefenderFinalDamage);
			kInfo.setDamageInflicted(BATTLE_UNIT_DEFENDER, iDefenderDamageDealt);
			kInfo.doExperience();




		}
		else if (kInfo.getCity(BATTLE_UNIT_DEFENDER) != NULL)
		{
			CvCity& defender = *kInfo.getCity(BATTLE_UNIT_DEFENDER);
			iDefenderStrength = defender.getStrengthValue();
			int iWoundedRatio = 0, iDamage = 0;
			const bool bRangedAttack = kInfo.getAttackIsRanged() || kInfo.getAttackIsBombingMission();
			std::pair<int, int> damagePair(-1, -1);
			if (kInfo.getDefenderRetaliates())
			{
				if (iDefenderStrength <= 0)
					damagePair.second = 0;
			}
			else
			{
				damagePair.second = 0;
			}
			do
			{
				const bool bAttacker = (damagePair.first == -1);
				iWoundedRatio = bAttacker ? attacker.getWoundedRatio(iExtraDamage) : iMaxHP;

				const int iStrength = bAttacker ? iAttackerStrength : iDefenderStrength;
				const int iOpponentStrength = bAttacker ? iDefenderStrength : iAttackerStrength;

				const int iAttackSameStrengthMinDamage = bRangedAttack ?         GC.getRANGE_ATTACK_SAME_STRENGTH_MIN_DAMAGE() :         GC.getATTACK_SAME_STRENGTH_MIN_DAMAGE();
				iDamage = iAttackSameStrengthMinDamage * iWoundedRatio / iMaxHP;
				kInfo.doRandomness(bAttacker ? BATTLE_UNIT_ATTACKER : BATTLE_UNIT_DEFENDER, iWoundedRatio);
				iDamage += kInfo.getCombatSeed(bAttacker ? BATTLE_UNIT_ATTACKER : BATTLE_UNIT_DEFENDER);
				double fStrengthRatio = kInfo.doStrengthRatio(iStrength, iOpponentStrength);
				iDamage = static_cast<int>(iDamage * fStrengthRatio);

				if (bAttacker && !bRangedAttack)
				{
					iDamage *=         GC.getATTACKING_CITY_MELEE_DAMAGE_MOD();
					iDamage /= 100;
				}
				if (!bAttacker && !bRangedAttack)
				{
					iDamage *=         GC.getCITY_ATTACKING_DAMAGE_MOD();
					iDamage /= 100;
				}
				iDamage /= 100;
				iDamage = iDamage > 0 ? iDamage : 1;

				bAttacker ? damagePair.first = iDamage : damagePair.second = iDamage;
			} while (damagePair.first == -1 || damagePair.second == -1);
			int iAttackerDamageDealt = damagePair.first;
			int iDefenderDamageDealt = damagePair.second;
			if (!kInfo.getDefenderRetaliates() || iDefenderStrength <= 0)
			{
				iDefenderDamageDealt = 0;
			}
			else if (iDefenderDamageDealt < GC.getMIN_CITY_STRIKE_DAMAGE())
			{

				iDefenderDamageDealt = GC.getMIN_CITY_STRIKE_DAMAGE();
			}

			int iDefenderFinalDamage = iAttackerDamageDealt + defender.getDamage();
			int iAttackerFinalDamage = iDefenderDamageDealt + attacker.getDamage() + kInfo.getDamageInflicted(BATTLE_UNIT_INTERCEPTOR);
			if (!bRangedAttack)
			{

				if (iDefenderFinalDamage >= defender.GetMaxHitPoints() && iAttackerFinalDamage >= iMaxHP)
				{
					iDefenderDamageDealt = iMaxHP - attacker.getDamage() - 1;
					iAttackerFinalDamage = iMaxHP - 1;
					iDefenderFinalDamage = iMaxHP;
				}
			}
			else
			{

				if (iDefenderFinalDamage >= defender.GetMaxHitPoints())
				{
					iAttackerDamageDealt = defender.GetMaxHitPoints() - defender.getDamage() - 1;
					iDefenderFinalDamage = defender.GetMaxHitPoints() - 1;
				}
			}
			if (iDefenderFinalDamage > defender.GetMaxHitPoints())
			{
				iAttackerDamageDealt = defender.GetMaxHitPoints() - defender.getDamage();
				iDefenderFinalDamage = defender.GetMaxHitPoints();
			}
			if (iAttackerFinalDamage > iMaxHP)
			{
				iDefenderDamageDealt = iMaxHP - attacker.getDamage() - kInfo.getDamageInflicted(BATTLE_UNIT_INTERCEPTOR);
				iAttackerFinalDamage = iMaxHP;
			}


			kInfo.setFinalDamage(BATTLE_UNIT_ATTACKER, iAttackerFinalDamage );
			kInfo.setDamageInflicted(BATTLE_UNIT_ATTACKER, iAttackerDamageDealt);
			kInfo.setFinalDamage(BATTLE_UNIT_DEFENDER, iDefenderFinalDamage);
			kInfo.setDamageInflicted(BATTLE_UNIT_DEFENDER, iDefenderDamageDealt);
			kInfo.doExperience();
		}
		else
		{
			CvAssertMsg(false, "Defender is neither Unit nor City");
		}
		
	}
	else if (kInfo.getCity(BATTLE_UNIT_ATTACKER) != NULL)
	{
		CvCity& attacker = *kInfo.getCity(BATTLE_UNIT_ATTACKER);
		int iAttackerStrength = attacker.getStrengthValue(true);
		int iDefenderStrength = 0;
		if (kInfo.getUnit(BATTLE_UNIT_DEFENDER) != NULL)
		{
			CvUnit& defender = *kInfo.getUnit(BATTLE_UNIT_DEFENDER);
			iDefenderStrength = defender.GetMaxDefenseStrength(kInfo);
			int iWoundedRatio = 0, iDamage = 0;
			std::pair<int, int> damagePair(-1, -1);
			if (kInfo.getAttackIsRanged() && !defender.IsCanDefend())
			{
				damagePair.first = GC.getNONCOMBAT_UNIT_RANGED_DAMAGE();
			}
			damagePair.second = kInfo.getDefenderRetaliates() ? -1 : 0;
			do
			{
				if (damagePair.first != -1 && damagePair.second != -1)
					break;
				const bool bAttacker = (damagePair.first == -1);
				const bool bRangedAttack = kInfo.getAttackIsRanged();
				iWoundedRatio = bAttacker ? iMaxHP : defender.getWoundedRatio(kInfo.getExtraDamageTaken(BATTLE_UNIT_DEFENDER));
				const int iStrength = bAttacker ? iAttackerStrength : iDefenderStrength;
				const int iOpponentStrength = bAttacker ? iDefenderStrength : iAttackerStrength;
				const int iAttackSameStrengthMinDamage = bRangedAttack ?         GC.getRANGE_ATTACK_SAME_STRENGTH_MIN_DAMAGE() :         GC.getATTACK_SAME_STRENGTH_MIN_DAMAGE();
				iDamage = iAttackSameStrengthMinDamage * iWoundedRatio / iMaxHP;
				kInfo.doRandomness(bAttacker ? BATTLE_UNIT_ATTACKER : BATTLE_UNIT_DEFENDER, iWoundedRatio);
				iDamage += kInfo.getCombatSeed(bAttacker ? BATTLE_UNIT_ATTACKER : BATTLE_UNIT_DEFENDER);
				double fStrengthRatio = kInfo.doStrengthRatio(iStrength, iOpponentStrength);
				iDamage = static_cast<int>(iDamage * fStrengthRatio);

				if (bAttacker && !bRangedAttack)
				{
					iDamage *=         GC.getATTACKING_CITY_MELEE_DAMAGE_MOD();
					iDamage /= 100;
				}
				if (bAttacker)
				{
					iDamage *=         GC.getCITY_ATTACKING_DAMAGE_MOD();
					iDamage /= 100;
				}
				iDamage /= 100;
				iDamage = iDamage > 0 ? iDamage : 1;

				bAttacker ? damagePair.first = iDamage : damagePair.second = iDamage;
			} while (damagePair.first == -1 || damagePair.second == -1);
			int iAttackerDamageDealt = damagePair.first;
			if (iAttackerDamageDealt < GC.getMIN_CITY_STRIKE_DAMAGE())
			{
				iAttackerDamageDealt = GC.getMIN_CITY_STRIKE_DAMAGE();
			}
			int iDefenderDamageDealt = damagePair.second;
			if (iDefenderDamageDealt < 0)
			{
				iDefenderDamageDealt = 0;
			}

			int iDefenderFinalDamage = iAttackerDamageDealt + defender.getDamage();
			int iAttackerFinalDamage = iDefenderDamageDealt + attacker.getDamage();
			if (iDefenderFinalDamage > iMaxHP)
			{
				iAttackerDamageDealt = iMaxHP - defender.getDamage();
				iDefenderFinalDamage = iMaxHP;
			}
			if (iAttackerFinalDamage > iMaxHP)
			{
				iDefenderDamageDealt = iMaxHP - attacker.getDamage();
				iAttackerFinalDamage = iMaxHP;
			}

			kInfo.setFinalDamage(BATTLE_UNIT_ATTACKER, iAttackerFinalDamage);
			kInfo.setDamageInflicted(BATTLE_UNIT_ATTACKER, iAttackerDamageDealt);
			kInfo.setFinalDamage(BATTLE_UNIT_DEFENDER, iDefenderFinalDamage);
			kInfo.setDamageInflicted(BATTLE_UNIT_DEFENDER, iDefenderDamageDealt);
			kInfo.doExperience();
		}
		else if (kInfo.getCity(BATTLE_UNIT_DEFENDER) != NULL)
		{
			CvCity& defender = *kInfo.getCity(BATTLE_UNIT_DEFENDER);
			iDefenderStrength = defender.getStrengthValue();
			int iDamage = 0;
			std::pair<int, int> damagePair(-1, -1);
			damagePair.second = kInfo.getDefenderRetaliates() ? -1 : 0;
			do
			{
				const bool bAttacker = (damagePair.first == -1);
				const bool bRangedAttack = kInfo.getAttackIsRanged();
				const int iStrength = bAttacker ? iAttackerStrength : iDefenderStrength;
				const int iOpponentStrength = bAttacker ? iDefenderStrength : iAttackerStrength;
				const int iAttackSameStrengthMinDamage = bRangedAttack ?         GC.getRANGE_ATTACK_SAME_STRENGTH_MIN_DAMAGE() :         GC.getATTACK_SAME_STRENGTH_MIN_DAMAGE();;
				iDamage = iAttackSameStrengthMinDamage;
				kInfo.doRandomness(bAttacker ? BATTLE_UNIT_ATTACKER : BATTLE_UNIT_DEFENDER, iMaxHP);
				iDamage += kInfo.getCombatSeed(bAttacker ? BATTLE_UNIT_ATTACKER : BATTLE_UNIT_DEFENDER);
				double fStrengthRatio = kInfo.doStrengthRatio(iStrength, iOpponentStrength);
				iDamage = static_cast<int>(iDamage * fStrengthRatio);

				if (bAttacker && !bRangedAttack)
				{
					iDamage *=         GC.getATTACKING_CITY_MELEE_DAMAGE_MOD();
					iDamage /= 100;
				}
				if (bAttacker)
				{
					iDamage *=         GC.getCITY_ATTACKING_DAMAGE_MOD();
					iDamage /= 100;
				}
				iDamage /= 100;
				iDamage = iDamage > 0 ? iDamage : 1;

				bAttacker ? damagePair.first = iDamage : damagePair.second = iDamage;
			} while (damagePair.first == -1 || damagePair.second == -1);
			int iAttackerDamageDealt = damagePair.first;
			if (iAttackerDamageDealt < GC.getMIN_CITY_STRIKE_DAMAGE())
			{
				iAttackerDamageDealt = GC.getMIN_CITY_STRIKE_DAMAGE();
			}
			int iDefenderDamageDealt = damagePair.second;
			if (!kInfo.getDefenderRetaliates())
			{
				iDefenderDamageDealt = 0;
			}
			else if (iDefenderDamageDealt < GC.getMIN_CITY_STRIKE_DAMAGE())
			{
				iDefenderDamageDealt = GC.getMIN_CITY_STRIKE_DAMAGE();
			}

			int iDefenderFinalDamage = iAttackerDamageDealt + defender.getDamage();
			int iAttackerFinalDamage = iDefenderDamageDealt + attacker.getDamage();
			if (iDefenderFinalDamage > defender.GetMaxHitPoints() - 1)
			{
				iAttackerDamageDealt = defender.GetMaxHitPoints() - 1 - defender.getDamage();
				iDefenderFinalDamage = defender.GetMaxHitPoints() - 1;
			}
			if (iAttackerFinalDamage > attacker.GetMaxHitPoints() - 1)
			{
				iDefenderDamageDealt = attacker.GetMaxHitPoints() - 1 - attacker.getDamage();
				iAttackerFinalDamage = attacker.GetMaxHitPoints() - 1;
			}

			kInfo.setFinalDamage(BATTLE_UNIT_ATTACKER, iAttackerFinalDamage);
			kInfo.setDamageInflicted(BATTLE_UNIT_ATTACKER, iAttackerDamageDealt);
			kInfo.setFinalDamage(BATTLE_UNIT_DEFENDER, iDefenderFinalDamage);
			kInfo.setDamageInflicted(BATTLE_UNIT_DEFENDER, iDefenderDamageDealt);
			kInfo.doExperience();
		}
		else
		{
			CvAssertMsg(false, "Defender is neither Unit nor City");
		}
	}
	else
	{
		CvAssertMsg(false, "Attacker is neither Unit nor City");
	}

	return;
}


void CvGame::BuildCombatModHelpText(CvCombatModifierList& kModifierList, const char* szTextKey, int iModifier, const char* szExtraKey) const
{
	if (iModifier == 0)
		return;

	Localization::String localizedText = Localization::Lookup(szTextKey);

	if (szExtraKey != NULL)
	{
		localizedText << szExtraKey;
	}

	kModifierList.AddEntry(localizedText.toUTF8(), iModifier);
}


void CvGame::BuildCombatModHelpText(CvCombatModifierList& kModifierList, const char* szTextKey, int iModifier, int iExtraInt) const
{
	if (iModifier == 0)
		return;

	Localization::String localizedText = Localization::Lookup(szTextKey);

	localizedText << iExtraInt;

	kModifierList.AddEntry(localizedText.toUTF8(), iModifier);
}
void CvGame::BuildCombatStrengthHelpText(CvCombatModifierList& kModifierList, const char* szTextKey, int iValue, const char* szExtraKey) const
{
	if (iValue == 0)
		return;

	Localization::String strText = Localization::Lookup(szTextKey);

	if (szExtraKey != NULL && szExtraKey[0] != '\0')
		strText << szExtraKey;

	kModifierList.AddEntry(strText.toUTF8(), iValue, false);
}

void CvGame::BuildCombatStrengthHelpText(CvCombatModifierList& kModifierList, const char* szTextKey, int iValue, int iExtraValue) const
{
	if (iValue == 0)
		return;

	Localization::String strText = Localization::Lookup(szTextKey);
	strText << iExtraValue;

	kModifierList.AddEntry(strText.toUTF8(), iValue, false);
}
#endif

PlayerTypes GetRandomMajorPlayer()
{
	PlayerTypes ePlayer = NO_PLAYER;
	do 
	{
		ePlayer = static_cast<PlayerTypes>(GC.getGame().getJonRandNum(MAX_MAJOR_CIVS, "Random Major Civ"));
	} while (!GET_PLAYER(ePlayer).isEverAlive());
	return ePlayer;
}



PlayerTypes GetRandomPlayer()
{
	PlayerTypes ePlayer = NO_PLAYER;
	do 
	{
		ePlayer = static_cast<PlayerTypes>(GC.getGame().getJonRandNum(MAX_CIV_PLAYERS, "Random Player"));
	} while (!GET_PLAYER(ePlayer).isEverAlive());
	return ePlayer;
}


void CvGame::PopulateDigSite(CvPlot& kPlot, EraTypes eEra, GreatWorkArtifactClass eArtifact)
{
	CvMap& theMap = GC.getMap();
	CvArchaeologyData digSite;

	const int iPlotX = kPlot.getX();
	const int iPlotY = kPlot.getY();

	eEra = eEra > static_cast<EraTypes>(0) ? eEra : static_cast<EraTypes>(0);
	digSite.m_eArtifactType = eArtifact;
	digSite.m_eEra = eEra;


	CvCity* pNearestCity = theMap.findCity(iPlotX, iPlotY, NO_PLAYER, NO_TEAM, true                );
	pNearestCity = pNearestCity ? pNearestCity : theMap.findCity(iPlotX, iPlotY, NO_PLAYER, NO_TEAM, false                );
	if (pNearestCity)
	{
		digSite.m_ePlayer1 = pNearestCity->getOriginalOwner();
	}
	else
	{

		CvUnit* pUnit = theMap.findUnit(iPlotX, iPlotY);
		if (pUnit)
		{
			digSite.m_ePlayer1 = pUnit->GetOriginalOwner();
		}
		else
		{

			PlayerTypes thisPlayer;
			if (theMap.findNearestStartPlot(iPlotX, iPlotY, thisPlayer))
			{
				digSite.m_ePlayer1 = thisPlayer;
			}
			else
			{
				digSite.m_ePlayer1 = GetRandomMajorPlayer();
			}
		}
	}

	if (eArtifact == CvTypes::getARTIFACT_BATTLE_MELEE() || eArtifact == CvTypes::getARTIFACT_BATTLE_RANGED() || eArtifact == CvTypes::getARTIFACT_RAZED_CITY())
	{
		PlayerTypes ePlayer2 = NO_PLAYER;
		do 
		{
			ePlayer2 = GetRandomPlayer();
		} while (ePlayer2 == digSite.m_ePlayer1);
		digSite.m_ePlayer2 = ePlayer2;
	}

	kPlot.AddArchaeologicalRecord(digSite.m_eArtifactType, digSite.m_eEra, digSite.m_ePlayer1, digSite.m_ePlayer2);
}

void CvGame::SpawnArchaeologySitesHistorically()
{
	CvMap& theMap = GC.getMap();
	const int iGridWidth = theMap.getGridWidth();



	const ResourceTypes eArtifactResourceType = static_cast<ResourceTypes>(GC.getARTIFACT_RESOURCE());
	const ResourceTypes eHiddenArtifactResourceType = static_cast<ResourceTypes>(GC.getHIDDEN_ARTIFACT_RESOURCE());

	const size_t aRandomArtifactsCount = 7;
	GreatWorkArtifactClass aRandomArtifacts[aRandomArtifactsCount] = { 
		CvTypes::getARTIFACT_ANCIENT_RUIN(), 
		CvTypes::getARTIFACT_ANCIENT_RUIN(), 
		CvTypes::getARTIFACT_RAZED_CITY(), 
		CvTypes::getARTIFACT_BARBARIAN_CAMP(), 
		CvTypes::getARTIFACT_BARBARIAN_CAMP(), 
		CvTypes::getARTIFACT_BATTLE_MELEE(), 
		CvTypes::getARTIFACT_BATTLE_RANGED() 
	};


	const int iNumMajorCivs = countMajorCivsEverAlive();
	const int iMinDigSites = GC.getMIN_DIG_SITES_PER_MAJOR_CIV() * iNumMajorCivs;
	const int iMaxDigSites = GC.getMAX_DIG_SITES_PER_MAJOR_CIV() * iNumMajorCivs;
	const int iIdealNumDigSites = iMinDigSites + getJonRandNum((iMaxDigSites - iMinDigSites) / 2, "Num dig sites") + getJonRandNum((iMaxDigSites - iMinDigSites) / 2, "Num dig sites");


	EraTypes eHighestEra = NO_ERA;
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;


		if(!GET_PLAYER(eLoopPlayer).isEverAlive())
			continue;

		if (GET_PLAYER(eLoopPlayer).GetCurrentEra() > eHighestEra)
		{
			eHighestEra = GET_PLAYER(eLoopPlayer).GetCurrentEra();
		}
	}

	CvWeightedVector<int, 64, true> eEraWeights;
	eEraWeights.clear();
	int iMaxEraWeight = 0;
	for (int i=0; i < static_cast<int>(eHighestEra); i++)
	{
		int iWeight = static_cast<int>(eHighestEra) - i;
		eEraWeights.push_back(i,iWeight);
		iMaxEraWeight += iWeight;
	}

	RandomNumberDelegate fcn;
	fcn = MakeDelegate(this, &CvGame::getJonRandNum);


	int iHowManyChosenDigSites = 0;


	FFastVector<CvArchaeologyData, true, c_eCiv5GameplayDLL, 0> historicalDigSites;
	FFastVector<CvArchaeologyData, true, c_eCiv5GameplayDLL, 0> scratchDigSites;
	int iGridSize = theMap.numPlots();
	CvAssertMsg(iGridSize > 0, "iGridSize is zero");
	historicalDigSites.resize(iGridSize);
	scratchDigSites.resize(iGridSize);
	for (int i = 0; i < iGridSize; i++)
	{
		scratchDigSites[i].m_eArtifactType = NO_GREAT_WORK_ARTIFACT_CLASS;
		scratchDigSites[i].m_eEra = NO_ERA;
		scratchDigSites[i].m_ePlayer1 = NO_PLAYER;
		scratchDigSites[i].m_ePlayer2 = NO_PLAYER;

		CvPlot* pPlot = theMap.plotByIndexUnchecked(i);
		const ResourceTypes eResource = pPlot->getResourceType();

		if (pPlot->isWater() || pPlot->isImpassable())
		{
			historicalDigSites[i].m_eArtifactType = NO_GREAT_WORK_ARTIFACT_CLASS;
			historicalDigSites[i].m_eEra = NO_ERA;
			historicalDigSites[i].m_ePlayer1 = NO_PLAYER;
			historicalDigSites[i].m_ePlayer2 = NO_PLAYER;


			if(eResource == eArtifactResourceType || eResource == eHiddenArtifactResourceType)
			{
				pPlot->setResourceType(NO_RESOURCE, 0, true);
			}
		}
		else
		{

			if(eResource == eArtifactResourceType || eResource == eHiddenArtifactResourceType)
			{
				if(pPlot->GetArchaeologicalRecord().m_eArtifactType == NO_GREAT_WORK_ARTIFACT_CLASS)
				{

					EraTypes eEra = static_cast<EraTypes>(eEraWeights.ChooseByWeight(&fcn, "Choosing an era by weight"));
					eEra = eEra > static_cast<EraTypes>(0) ? eEra : static_cast<EraTypes>(0);


					GreatWorkArtifactClass eArtifact = aRandomArtifacts[getJonRandNum(aRandomArtifactsCount, "Artifact type for non-historical dig site")];

					PopulateDigSite(*pPlot, eEra, eArtifact);


					scratchDigSites[i] = pPlot->GetArchaeologicalRecord();
				}

				iHowManyChosenDigSites++;
			}

			historicalDigSites[i] = pPlot->GetArchaeologicalRecord();
		}
	}


	FFastVector<int, true, c_eCiv5GameplayDLL, 0> digSiteWeights;
	digSiteWeights.resize(iGridSize);
	for (int i = 0; i < iGridSize; i++)
	{
		digSiteWeights[i] = 0;
	}
	CalculateDigSiteWeights(iGridSize, historicalDigSites, scratchDigSites, digSiteWeights);


	static CvWeightedVector<int, 128*80, true> aDigSiteWeights;
	aDigSiteWeights.resize(iGridSize);

	vector<GreatWorkType> aWorksWriting;
	Database::Connection* db = GC.GetGameDatabase();
	if(db != NULL)
	{
		Database::Results kQuery;
		if(db->Execute(kQuery, "SELECT ID from GreatWorks WHERE ArchaeologyOnly = '1'"))
		{
			while(kQuery.Step())
			{
				const GreatWorkType eWork = static_cast<GreatWorkType>(kQuery.GetInt(0));
				aWorksWriting.push_back(eWork);
			}
		}
	}

	int iApproxNumHiddenSites = iIdealNumDigSites * GC.getPERCENT_SITES_HIDDEN() / 100;
	int iNumDesiredWritingSites = iApproxNumHiddenSites * GC.getPERCENT_HIDDEN_SITES_WRITING() / 100;
	int iNumWritingSites = min((int)aWorksWriting.size(), iNumDesiredWritingSites);


	while (iHowManyChosenDigSites < iIdealNumDigSites)
	{

		aDigSiteWeights.clear();
		for (int i = 0; i < iGridSize; i++)
		{
			if (digSiteWeights[i] > 0)
			{
				aDigSiteWeights.push_back(i, digSiteWeights[i]);
			}
		}


		aDigSiteWeights.SortItems();


		int iBestSite = aDigSiteWeights.GetElement(0);
		CvPlot* pPlot = theMap.plotByIndexUnchecked(iBestSite);


		bool bHiddenSite = GC.getGame().getJonRandNum(100, "Hidden antiquity site roll") < GC.getPERCENT_SITES_HIDDEN();
		if (bHiddenSite)
		{
			pPlot->setResourceType(eHiddenArtifactResourceType, 1);
		}
		else
		{
			pPlot->setResourceType(eArtifactResourceType, 1);
		}


		if (scratchDigSites[iBestSite].m_eArtifactType == NO_GREAT_WORK_ARTIFACT_CLASS)
		{


			EraTypes eEra = static_cast<EraTypes>(eEraWeights.ChooseByWeight(&fcn, "Choosing an era by weight"));
			eEra = eEra > static_cast<EraTypes>(0) ? eEra : static_cast<EraTypes>(0);


			GreatWorkArtifactClass eArtifact;
			eArtifact = aRandomArtifacts[getJonRandNum(aRandomArtifactsCount, "Artifact type for non-historical dig site")];

			PopulateDigSite(*pPlot, eEra, eArtifact);
		}


		if (bHiddenSite && iNumWritingSites > 0)
		{

			pPlot->SetArtifactType(CvTypes::getARTIFACT_WRITING());


			int iIndex = getJonRandNum(aWorksWriting.size(), "");
			GreatWorkType eWrittenGreatWork = aWorksWriting[iIndex];
			pPlot->SetArtifactGreatWork((GreatWorkType)eWrittenGreatWork);


			vector<GreatWorkType>::const_iterator it;
			it = std::find (aWorksWriting.begin(), aWorksWriting.end(), eWrittenGreatWork);
			aWorksWriting.erase(it);


			iNumWritingSites--;
		}

		scratchDigSites[iBestSite] = pPlot->GetArchaeologicalRecord();

		iHowManyChosenDigSites++;


		const int iRange = 3;
		int iPlotX = iBestSite % iGridWidth;
		int iPlotY = iBestSite / iGridWidth;
#ifdef AUI_HEXSPACE_DX_LOOPS
		int iMaxDX, iDX;
		CvPlot* pLoopPlot;
		for (int iDY = -iRange; iDY <= iRange; iDY++)
		{
			iMaxDX = iRange - MAX(0, iDY);
			for (iDX = -iRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
			{

				pLoopPlot = plotXY(iPlotX, iPlotY, iDX, iDY);
#else
		for (int iDX = -iRange; iDX <= iRange; iDX++)
		{
			for (int iDY = -iRange; iDY <= iRange; iDY++)
			{
				CvPlot* pLoopPlot = plotXYWithRangeCheck(iPlotX, iPlotY, iDX, iDY, iRange);
#endif
				if (pLoopPlot)
				{
					int iIndex = pLoopPlot->GetPlotIndex();
					digSiteWeights[iIndex] = CalculateDigSiteWeight(iIndex, historicalDigSites, scratchDigSites);				
				}
			}
		}
	}
}



CombatPredictionTypes CvGame::GetCombatPrediction(const CvUnit* pAttackingUnit, const CvUnit* pDefendingUnit)
{
	if(!pAttackingUnit || !pDefendingUnit)
	{
		return NO_COMBAT_PREDICTION;
	}

	CombatPredictionTypes ePrediction = NO_COMBAT_PREDICTION;
	bool bRanged = pAttackingUnit->GetBaseRangedCombatStrength() > 0 && (pAttackingUnit->isOnlyDefensive() || pAttackingUnit->isMustSetUpToRangedAttack() && pAttackingUnit->isSetUpForRangedAttack());
	if(bRanged)
	{
		return COMBAT_PREDICTION_RANGED;
	}

	CvPlot* pFromPlot = pAttackingUnit->plot();
	CvPlot* pToPlot = pDefendingUnit->plot();

#if defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
	CvCombatInfo kInfo;
	kInfo.setUnit(BATTLE_UNIT_ATTACKER, const_cast<CvUnit*>(pAttackingUnit));
	kInfo.setUnit(BATTLE_UNIT_DEFENDER, const_cast<CvUnit*>(pDefendingUnit));
	kInfo.setPlot(pToPlot);
	kInfo.setCombatPrediction(true);

	int iAttackingStrength = pAttackingUnit->GetMaxAttackStrength(kInfo);
	int iDefenderStrength = pDefendingUnit->GetMaxDefenseStrength(kInfo);

	getCombatDamage(kInfo);
#else
	int iAttackingStrength = pAttackingUnit->GetMaxAttackStrength(pFromPlot, pToPlot, pDefendingUnit);
	if(iAttackingStrength == 0)
	{
		return NO_COMBAT_PREDICTION;
	}

	int iDefenderStrength = pDefendingUnit->GetMaxDefenseStrength(pToPlot, pAttackingUnit, false);
#endif
	int iAttackingDamageInflicted = kInfo.getDamageInflicted(BATTLE_UNIT_ATTACKER);
	int iDefenderDamageInflicted = kInfo.getDamageInflicted(BATTLE_UNIT_DEFENDER);

	int iMaxUnitHitPoints = GC.getMAX_HIT_POINTS();
	if(iAttackingDamageInflicted > iMaxUnitHitPoints)
	{
		iAttackingDamageInflicted = iMaxUnitHitPoints;
	}
	if(iDefenderDamageInflicted > iMaxUnitHitPoints)
	{
		iDefenderDamageInflicted = iMaxUnitHitPoints;
	}

	bool bAttackerDies = false;
	bool bDefenderDies = false;

	if(pAttackingUnit->getDamage() + iDefenderDamageInflicted >= iMaxUnitHitPoints)
	{
		bAttackerDies = true;
	}

	if(pDefendingUnit->getDamage() + iAttackingDamageInflicted >= iMaxUnitHitPoints)
	{
		bDefenderDies = true;
	}

	if(bAttackerDies && bDefenderDies)
	{
		ePrediction = COMBAT_PREDICTION_STALEMATE;
	}
	else if(bAttackerDies)
	{
		ePrediction = COMBAT_PREDICTION_TOTAL_DEFEAT;
	}
	else if(bDefenderDies)
	{
		ePrediction = COMBAT_PREDICTION_TOTAL_VICTORY;
	}
	else if(iAttackingDamageInflicted - iDefenderDamageInflicted > 30)
	{
		ePrediction = COMBAT_PREDICTION_MAJOR_VICTORY;
	}
	else if(iAttackingDamageInflicted > iDefenderDamageInflicted)
	{
		ePrediction = COMBAT_PREDICTION_SMALL_VICTORY;
	}
	else if(iDefenderDamageInflicted - iAttackingDamageInflicted > 30)
	{
		ePrediction = COMBAT_PREDICTION_MAJOR_DEFEAT;
	}
	else if(iAttackingDamageInflicted < iDefenderDamageInflicted)
	{
		ePrediction = COMBAT_PREDICTION_SMALL_DEFEAT;
	}
	else
	{
		ePrediction = COMBAT_PREDICTION_STALEMATE;
	}

	return ePrediction;
}








const unsigned int CvGame::ms_aiSizes[10] = { 2, 3, 5, 7, 9, 11, 13, 15, 17, UINT_MAX };



unsigned int CvGame::GetVariableCitySizeFromPopulation(unsigned int nPopulation)
{
	for(unsigned int i = 0; i < 10; ++i)
	{
		if(nPopulation < ms_aiSizes[i])
		{
			return i;
		}
	}
	return 4;
};






void CvGame::NetMessageStaticsReset()
{
}


void CvGame::SetLastTurnAICivsProcessed()
{
	if(m_lastTurnAICivsProcessed != getGameTurn()){
		gDLL->SendAICivsProcessed();
		m_lastTurnAICivsProcessed = getGameTurn();
	}
}
