







#include "CvGameCoreDLLPCH.h"
#include "CvPlayerAI.h"
#include "CvRandom.h"
#include "CvGlobals.h"
#include "CvGameCoreUtils.h"
#include "CvMap.h"
#include "CvArea.h"
#include "CvPlot.h"
#include "CvTeam.h"
#include "CvGameCoreUtils.h"
#include "ICvDLLUserInterface.h"
#include "CvInfos.h"
#include "CvAStar.h"
#include "CvDiplomacyAI.h"
#include "CvGrandStrategyAI.h"
#include "CvTechAI.h"
#include "CvDangerPlots.h"
#include "CvImprovementClasses.h"
#include "CvTacticalAnalysisMap.h"
#include "CvMilitaryAI.h"
#include "CvWonderProductionAI.h"
#include "CvCitySpecializationAI.h"
#include "cvStopWatch.h"
#include "CvEconomicAI.h"


#include "LintFree.h"

#define DANGER_RANGE				(6)



CvPlayerAI* CvPlayerAI::m_aPlayers = NULL;

void CvPlayerAI::initStatics()
{
	m_aPlayers = FNEW(CvPlayerAI[MAX_PLAYERS], c_eCiv5GameplayDLL, 0);
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aPlayers[iI].m_eID = ((PlayerTypes)iI);
	}
}

void CvPlayerAI::freeStatics()
{
	SAFE_DELETE_ARRAY(m_aPlayers);
}


CvPlayerAI::CvPlayerAI()
{
	AI_reset();
}


CvPlayerAI::~CvPlayerAI()
{
	AI_uninit();
}


void CvPlayerAI::AI_init()
{
	AI_reset();
}


void CvPlayerAI::AI_uninit()
{
}


void CvPlayerAI::AI_reset()
{
	AI_uninit();
}

void CvPlayerAI::AI_doTurnPre()
{
	AI_PERF_FORMAT("AI-perf.csv", ("CvPlayerAI::AI_doTurnPre, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), getCivilizationShortDescription()) );
	CvAssertMsg(getPersonalityType() != NO_LEADER, "getPersonalityType() is not expected to be equal with NO_LEADER");
	CvAssertMsg(getLeaderType() != NO_LEADER, "getLeaderType() is not expected to be equal with NO_LEADER");
	CvAssertMsg(getCivilizationType() != NO_CIVILIZATION, "getCivilizationType() is not expected to be equal with NO_CIVILIZATION");

	if(isHuman())
	{
		return;
	}

	AI_updateFoundValues();

	AI_doResearch();
	AI_considerAnnex();
}


void CvPlayerAI::AI_doTurnPost()
{
	AI_PERF_FORMAT("AI-perf.csv", ("CvPlayerAI::AI_doTurnPost, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), getCivilizationShortDescription()) );
	if(isHuman())
	{
		return;
	}

	if(isBarbarian())
	{
		return;
	}

	if(isMinorCiv())
	{
		return;
	}

#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < GC.getNumVictoryInfos(); ++i)
#else
	for(int i = 0; i < GC.getNumVictoryInfos(); ++i)
#endif
	{
		AI_launch((VictoryTypes)i);
	}

	ProcessGreatPeople();
	GetEspionageAI()->DoTurn();
	GetTradeAI()->DoTurn();
}


void CvPlayerAI::AI_doTurnUnitsPre()
{
	GetTacticalAI()->InitializeQueuedAttacks();

	if(isHuman())
	{
		return;
	}

	if(isBarbarian())
	{
		return;
	}
}


void CvPlayerAI::AI_doTurnUnitsPost()
{
	CvUnit* pLoopUnit;
	int iLoop;

	if(!isHuman())
	{
		for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
		{
			pLoopUnit->AI_promote();
		}
	}
}

void CvPlayerAI::AI_updateFoundValues(bool bStartingLoc)
{
	int iGoodEnoughToBeWorthOurTime = GC.getAI_STRATEGY_MINIMUM_SETTLE_FERTILITY();
	int iLoop;
	const int iNumPlots = GC.getMap().numPlots();
	for(CvArea* pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
	{
		pLoopArea->setTotalFoundValue(0);
	}

	const PlayerTypes eID = GetID();
	if(bStartingLoc)
	{
		for(int iI = 0; iI < iNumPlots; iI++)
		{
			GC.getMap().plotByIndexUnchecked(iI)->setFoundValue(eID, -1);
		}
	}
	else
	{
		const TeamTypes eTeam = getTeam();
		GC.getGame().GetSettlerSiteEvaluator()->ComputeFlavorMultipliers(this);
		for (int iI = 0; iI < iNumPlots; iI++)
		{
			CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);

			if (pLoopPlot->isRevealed(eTeam))
			{
				const int iValue = GC.getGame().GetSettlerSiteEvaluator()->PlotFoundValue(pLoopPlot, this, NO_YIELD, false);
				pLoopPlot->setFoundValue(eID, iValue);
				if (iValue >= iGoodEnoughToBeWorthOurTime)
				{
					CvArea* pLoopArea = GC.getMap().getArea(pLoopPlot->getArea());
					if(pLoopArea && !pLoopArea->isWater())
					{
						pLoopArea->setTotalFoundValue(pLoopArea->getTotalFoundValue() + iValue);
					}
				}
			}
			else
			{
				pLoopPlot->setFoundValue(eID, -1);
			}
		}
	}
}


void CvPlayerAI::AI_unitUpdate()
{
	GC.getPathFinder().ForceReset();
	GC.getIgnoreUnitsPathFinder().ForceReset();
	GC.getRouteFinder().ForceReset();
	GC.GetWaterRouteFinder().ForceReset();



	bool bCommonPathFinderMPCaching = GC.getPathFinder().SetMPCacheSafe(true);
	bool bIgnoreUnitsPathFinderMPCaching = GC.getIgnoreUnitsPathFinder().SetMPCacheSafe(true);
	bool bTacticalPathFinderMPCaching = GC.GetTacticalAnalysisMapFinder().SetMPCacheSafe(true);
	bool bInfluencePathFinderMPCaching = GC.getInfluenceFinder().SetMPCacheSafe(true);
	bool bRoutePathFinderMPCaching = GC.getRouteFinder().SetMPCacheSafe(true);
	bool bWaterRoutePathFinderMPCaching = GC.GetWaterRouteFinder().SetMPCacheSafe(true);

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "PlayerPreAIUnitUpdate", args.get(), bResult);
	}




	if(hasBusyUnitOrCity())
	{
		return;
	}

	if(isHuman())
	{
		CvUnit::dispatchingNetMessage(true);

		GetHomelandAI()->FindAutomatedUnits();
		GetHomelandAI()->Update();
		CvUnit::dispatchingNetMessage(false);
	}
	else
	{

		GetTacticalAI()->CommandeerUnits();




		GetTacticalAI()->Update();


		if(m_eID != BARBARIAN_PLAYER)
		{

			GetHomelandAI()->RecruitUnits();
			GetHomelandAI()->Update();
		}
	}

	GC.getPathFinder().SetMPCacheSafe(bCommonPathFinderMPCaching);
	GC.getIgnoreUnitsPathFinder().SetMPCacheSafe(bIgnoreUnitsPathFinderMPCaching);
	GC.GetTacticalAnalysisMapFinder().SetMPCacheSafe(bTacticalPathFinderMPCaching);
	GC.getInfluenceFinder().SetMPCacheSafe(bInfluencePathFinderMPCaching);
	GC.getRouteFinder().SetMPCacheSafe(bRoutePathFinderMPCaching);
	GC.GetWaterRouteFinder().SetMPCacheSafe(bWaterRoutePathFinderMPCaching);
}


void CvPlayerAI::AI_conquerCity(CvCity* pCity, PlayerTypes eOldOwner)
{
	PlayerTypes eOriginalOwner = pCity->getOriginalOwner();
	TeamTypes eOldOwnerTeam = GET_PLAYER(eOldOwner).getTeam();


	if(eOriginalOwner != eOldOwner && eOriginalOwner != GetID() && CanLiberatePlayerCity(eOriginalOwner))
	{

		if(GET_PLAYER(eOriginalOwner).isMinorCiv())
		{
			if(GetDiplomacyAI()->DoPossibleMinorLiberation(eOriginalOwner, pCity->GetID()))
				return;
		}
		else
		{
			bool bLiberate = false;
			if (GET_PLAYER(eOriginalOwner).isAlive())
			{



				TeamTypes eOriginalOwnerTeam = GET_PLAYER(eOriginalOwner).getTeam();
				if (GET_TEAM(getTeam()).IsHasDefensivePact(eOriginalOwnerTeam) && GET_TEAM(getTeam()).isAtWar(eOldOwnerTeam) && GET_TEAM(eOriginalOwnerTeam).isAtWar(eOldOwnerTeam))
				{
					bLiberate = true;
				}

				else if (GetDiplomacyAI()->IsDoFAccepted(eOriginalOwner) && GetDiplomacyAI()->IsGoingForDiploVictory())
				{
					bLiberate = true;
				}
			}

			else if (!GET_PLAYER(eOriginalOwner).isHuman() && GetDiplomacyAI()->IsGoingForDiploVictory())
			{
				bLiberate = true;
			}

			if (bLiberate)
			{
				DoLiberatePlayer(eOriginalOwner, pCity->GetID());
				return;
			}
		}
	}


	if(canRaze(pCity))
	{


		if (IsEmpireUnhappy() || (GC.getMap().GetAIMapHint() & 2) || (GetPlayerTraits()->GetRazeSpeedModifier() > 0 && getNumCities() >= 3 + (GC.getGame().getGameTurn() / 100)) )
		{
			pCity->doTask(TASK_RAZE);
			return;
		}
	}


	if(pCity->getOriginalOwner() != GetID() || GET_PLAYER(m_eID).GetPlayerTraits()->IsNoAnnexing())
	{
#ifdef AUI_CITIZENS_MID_TURN_ASSIGN_RUNS_SELF_CONSISTENCY
		pCity->DoCreatePuppet(false);
#else
		pCity->DoCreatePuppet();
#endif
	}
}

bool CvPlayerAI::AI_captureUnit(UnitTypes, CvPlot* pPlot)
{
	CvCity* pNearestCity;

	CvAssert(!isHuman());


	if (isBarbarian())
		return true;


	if (pPlot->getTeam() == getTeam())
		return true;


	if (pPlot->getTeam() == NO_TEAM)
		return true;


	if (pPlot->IsFriendlyTerritory(GetID()))
		return true;


	pNearestCity = GC.getMap().findCity(pPlot->getX(), pPlot->getY(), NO_PLAYER, getTeam());
	if (pNearestCity != NULL)
	{
		if (plotDistance(pPlot->getX(), pPlot->getY(), pNearestCity->getX(), pNearestCity->getY()) <= 7)
			return true;
	}


	pNearestCity = GC.getMap().findCity(pPlot->getX(), pPlot->getY());
	if (pNearestCity != NULL)
	{
		if (plotDistance(pPlot->getX(), pPlot->getY(), pNearestCity->getX(), pNearestCity->getY()) <= 4)
			return false;
	}


	return true;
}

int CvPlayerAI::AI_foundValue(int iX, int iY, int, bool bStartingLoc)
{

	CvPlot* pPlot;
	int rtnValue = 0;

	pPlot = GC.getMap().plot(iX, iY);

	if(bStartingLoc)
	{
		rtnValue =  GC.getGame().GetStartSiteEvaluator()->PlotFoundValue(pPlot, this);
	}
	else
	{
		GC.getGame().GetSettlerSiteEvaluator()->ComputeFlavorMultipliers(this);
		rtnValue =  GC.getGame().GetSettlerSiteEvaluator()->PlotFoundValue(pPlot, this, NO_YIELD, false);
	}

	return rtnValue;
}

void CvPlayerAI::AI_chooseFreeGreatPerson()
{
	while(GetNumFreeGreatPeople() > 0)
	{
		UnitTypes eDesiredGreatPerson = NO_UNIT;


		if(GetDiplomacyAI()->GetWonderCompetitiveness() >= 8 && GC.getGame().getGameTurn() <= (GC.getGame().getEstimateEndTurn() / 2))
		{
			eDesiredGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_ENGINEER");
		}
		else
		{

			AIGrandStrategyTypes eVictoryStrategy = GetGrandStrategyAI()->GetActiveGrandStrategy();
			if(eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST"))
			{
				eDesiredGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_GREAT_GENERAL");
			}
			else if(eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE"))
			{
				eDesiredGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_ARTIST");
			}
			else if(eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_UNITED_NATIONS"))
			{
				eDesiredGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_MERCHANT");
			}
			else if(eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_SPACESHIP"))
			{
				eDesiredGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_SCIENTIST");
			}
		}

		if(eDesiredGreatPerson != NO_UNIT)
		{
			CvCity* pCapital = getCapitalCity();
			if(pCapital)
			{

				pCapital->GetCityCitizens()->DoSpawnGreatPerson(eDesiredGreatPerson, false, false);
			}
			ChangeNumFreeGreatPeople(-1);
		}
		else
		{
			break;
		}
	}
}

void CvPlayerAI::AI_chooseFreeTech()
{
	TechTypes eBestTech = NO_TECH;

	clearResearchQueue();



	if(eBestTech == NO_TECH)
	{
		eBestTech = GetPlayerTechs()->GetTechAI()->ChooseNextTech(this,               true);
	}

	if(eBestTech != NO_TECH)
	{
		GET_TEAM(getTeam()).setHasTech(eBestTech, true, GetID(), true, true);
	}
}


void CvPlayerAI::AI_chooseResearch()
{
#ifdef AUI_PERF_LOGGING_FORMATTING_TWEAKS
	AI_PERF_FORMAT("AI-perf.csv", ("AI_chooseResearch, Turn %03d, %s", GC.getGame().getGameTurn(), getCivilizationShortDescription()));
#else
	AI_PERF("AI-perf.csv", "AI_chooseResearch");
#endif

	TechTypes eBestTech = NO_TECH;
	int iI;

	clearResearchQueue();

	if(GetPlayerTechs()->GetCurrentResearch() == NO_TECH)
	{
		for(iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if(GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if((iI != GetID()) && (GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam()))
				{
					if(GET_PLAYER((PlayerTypes)iI).GetPlayerTechs()->GetCurrentResearch() != NO_TECH)
					{
						if(GetPlayerTechs()->CanResearch(GET_PLAYER((PlayerTypes)iI).GetPlayerTechs()->GetCurrentResearch()))
						{
							pushResearch(GET_PLAYER((PlayerTypes)iI).GetPlayerTechs()->GetCurrentResearch());
						}
					}
				}
			}
		}
	}

	if(GetPlayerTechs()->GetCurrentResearch() == NO_TECH)
	{


		if(eBestTech == NO_TECH)
		{
			eBestTech = GetPlayerTechs()->GetTechAI()->ChooseNextTech(this);
		}

		if(eBestTech != NO_TECH)
		{
			pushResearch(eBestTech);
		}
	}
}


struct CityAndProduction
{
	CvCity* pCity;
	int iProduction;
};

struct CityAndProductionEval
{
	bool operator()(CityAndProduction const& a, CityAndProduction const& b) const
	{
		return (a.iProduction > b.iProduction);
	}
};

void CvPlayerAI::AI_considerAnnex()
{
#ifdef AUI_PERF_LOGGING_FORMATTING_TWEAKS
	AI_PERF_FORMAT("AI-perf.csv", ("AI_considerAnnex, Turn %03d, %s", GC.getGame().getGameTurn(), getCivilizationShortDescription()));
#else
	AI_PERF("AI-perf.csv", "AI_ considerAnnex");
#endif


	if (IsEmpireUnhappy())
	{
		return;
	}


	if (GetDiplomacyAI()->IsGoingForCultureVictory())
	{
		return;
	}


	if (GetPlayerTraits()->IsNoAnnexing())
	{
		return;
	}


	CvCity* pCity = getCapitalCity();
	if (pCity && pCity->IsPuppet())
	{

		pCity->DoAnnex();
		return;
	}

	std::vector<CityAndProduction> aCityAndProductions;
	int iLoop = 0;
	pCity = NULL;


	for(pCity = firstCity(&iLoop); pCity != NULL; pCity = nextCity(&iLoop))
	{
		CityAndProduction kEval;
		kEval.pCity = pCity;
		kEval.iProduction = pCity->getYieldRateTimes100(YIELD_PRODUCTION, false);
		aCityAndProductions.push_back(kEval);
	}
	
	std::stable_sort(aCityAndProductions.begin(), aCityAndProductions.end(), CityAndProductionEval());
	
	CvCity* pTargetCity = NULL;
	float fCutoffValue = GC.getNORMAL_ANNEX();
	BuildingClassTypes eCourthouseType = NO_BUILDINGCLASS;

#ifdef AUI_WARNING_FIXES
	for (uint eBuildingType = 0; eBuildingType < GC.getNumBuildingInfos(); eBuildingType++)
#else
	for(int eBuildingType = 0; eBuildingType < GC.getNumBuildingInfos(); eBuildingType++)
#endif
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(eBuildingType);
		CvBuildingEntry* buildingInfo = GC.getBuildingInfo(eBuilding);

		if(buildingInfo)
		{
			if (buildingInfo->IsNoOccupiedUnhappiness())
			{
				eCourthouseType = (BuildingClassTypes)buildingInfo->GetBuildingClassType();
				break;
			}
		}
	}

	bool bCourthouseImprovement = false;
	if (eCourthouseType != NO_BUILDINGCLASS)
	{
		if (GetPlayerPolicies()->GetBuildingClassProductionModifier(eCourthouseType) > 0)
		{
			bCourthouseImprovement = true;
		}
#if defined(LEKMOD_TRAIT_BUILDING_CLASS_PRODUCTION_MODIFIERS)
		else if (GetPlayerTraits()->GetBuildingClassProductionModifier(eCourthouseType) > 0)
		{
			bCourthouseImprovement = true;
		}
#endif
	}

	if (bCourthouseImprovement)
	{
		fCutoffValue = GC.getAGGRESIVE_ANNEX();
	}

	uint uiCutOff = (uint)(aCityAndProductions.size() * fCutoffValue);
	for (uint ui = 0; ui < uiCutOff; ui++)
	{
		if (aCityAndProductions[ui].pCity->IsPuppet())
		{
			pTargetCity = aCityAndProductions[ui].pCity;
			break;
		}
	}

	if (pTargetCity)
	{
		if (!pTargetCity->IsResistance())
		{
			pTargetCity->DoAnnex();
		}
	}
}

int CvPlayerAI::AI_plotTargetMissionAIs(CvPlot* pPlot, MissionAITypes eMissionAI, int iRange)
{
	int iCount = 0;

	int iLoop;
	for(CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		CvPlot* pMissionPlot = pLoopUnit->GetMissionAIPlot();
		if(!pMissionPlot)
		{
			continue;
		}

		MissionAITypes eGroupMissionAI = pLoopUnit->GetMissionAIType();
		if(eGroupMissionAI != eMissionAI)
		{
			continue;
		}

		int iDistance = plotDistance(pPlot->getX(), pPlot->getY(), pMissionPlot->getX(), pMissionPlot->getY());
		if(iDistance == iRange)
		{
			iCount++;
		}
	}

	return iCount;
}



void CvPlayerAI::AI_doResearch()
{
	CvAssertMsg(!isHuman(), "isHuman did not return false as expected");

	if(GetPlayerTechs()->GetCurrentResearch() == NO_TECH)
	{
		AI_chooseResearch();

	}
}






void CvPlayerAI::Read(FDataStream& kStream)
{
	CvPlayer::Read(kStream);


	uint uiVersion;
	kStream >> uiVersion;
}






void CvPlayerAI::Write(FDataStream& kStream) const
{
	CvPlayer::Write(kStream);


	uint uiVersion = 1;
	kStream << uiVersion;
}

void CvPlayerAI::AI_launch(VictoryTypes eVictory)
{
	if(GET_TEAM(getTeam()).isHuman())
	{
		return;
	}

	if(!GET_TEAM(getTeam()).canLaunch(eVictory))
	{
		return;
	}

	launch(eVictory);
}

OperationSlot CvPlayerAI::PeekAtNextUnitToBuildForOperationSlot(int iAreaID)
{
	OperationSlot thisSlot;


	std::map<int, CvAIOperation*>::iterator iter;
	for(iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		CvAIOperation* pThisOperation = iter->second;
		if(pThisOperation)
		{
			thisSlot = pThisOperation->PeekAtNextUnitToBuild(iAreaID);
			if(thisSlot.IsValid())
			{
				break;
			}
		}
	}

	return thisSlot;
}


OperationSlot CvPlayerAI::CityCommitToBuildUnitForOperationSlot(int iAreaID, int iTurns, CvCity* pCity)
{
	OperationSlot thisSlot;


	std::map<int, CvAIOperation*>::iterator iter;
	for(iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		CvAIOperation* pThisOperation = iter->second;
		if(pThisOperation)
		{
			thisSlot = pThisOperation->CommitToBuildNextUnit(iAreaID, iTurns, pCity);
			if(thisSlot.IsValid())
			{
				break;
			}
		}
	}

	return thisSlot;
}

#ifdef AUI_WARNING_FIXES
void CvPlayerAI::CityUncommitToBuildUnitForOperationSlot(const OperationSlot& thisSlot)
#else
void CvPlayerAI::CityUncommitToBuildUnitForOperationSlot(OperationSlot thisSlot)
#endif
{

	CvAIOperation* pThisOperation = getAIOperation(thisSlot.m_iOperationID);
	if(pThisOperation)
	{
		pThisOperation->UncommitToBuild(thisSlot);
	}
}

#ifdef AUI_WARNING_FIXES
void CvPlayerAI::CityFinishedBuildingUnitForOperationSlot(const OperationSlot& thisSlot, CvUnit* pThisUnit)
#else
void CvPlayerAI::CityFinishedBuildingUnitForOperationSlot(OperationSlot thisSlot, CvUnit* pThisUnit)
#endif
{

	CvAIOperation* pThisOperation = getAIOperation(thisSlot.m_iOperationID);
	CvArmyAI* pThisArmy = getArmyAI(thisSlot.m_iArmyID);
	if(pThisOperation && pThisArmy && pThisUnit)
	{
		pThisArmy->AddUnit(pThisUnit->GetID(), thisSlot.m_iSlotID);
		pThisOperation->FinishedBuilding(thisSlot);
	}
}

int CvPlayerAI::GetNumUnitsNeededToBeBuilt()
{
	int iRtnValue = 0;

	std::map<int, CvAIOperation*>::iterator iter;
	for(iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		CvAIOperation* pThisOperation = iter->second;
		if(pThisOperation)
		{
			iRtnValue += pThisOperation->GetNumUnitsNeededToBeBuilt();
		}
	}

	return iRtnValue;
}

void CvPlayerAI::ProcessGreatPeople(void)
{
	SpecialUnitTypes eSpecialUnitGreatPerson = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_PEOPLE");

	CvAssert(isAlive());

	if(!isAlive())
		return;

	int iLoop;
	for(CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if(pLoopUnit->getSpecialUnitType() != eSpecialUnitGreatPerson)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;
		switch(pLoopUnit->AI_getUnitAIType())
		{
		case UNITAI_WRITER:
			eDirective = GetDirectiveWriter(pLoopUnit);
			break;
		case UNITAI_ARTIST:
			eDirective = GetDirectiveArtist(pLoopUnit);
			break;
		case UNITAI_MUSICIAN:
			eDirective = GetDirectiveMusician(pLoopUnit);
			break;
		case UNITAI_ENGINEER:
			eDirective = GetDirectiveEngineer(pLoopUnit);
			break;
		case UNITAI_MERCHANT:
			eDirective = GetDirectiveMerchant(pLoopUnit);
			break;
		case UNITAI_SCIENTIST:
			eDirective = GetDirectiveScientist(pLoopUnit);
			break;
		case UNITAI_GENERAL:
			eDirective = GetDirectiveGeneral(pLoopUnit);
			break;
		case UNITAI_PROPHET:
			eDirective = GetDirectiveProphet(pLoopUnit);
			break;
		case UNITAI_ADMIRAL:
			eDirective = GetDirectiveAdmiral(pLoopUnit);
			break;
		}

		pLoopUnit->SetGreatPeopleDirective(eDirective);
	}
}

bool PreparingForWar(CvPlayerAI* pPlayer)
{
	CvAssertMsg(pPlayer, "Need a player");
	if(!pPlayer)
	{
		return false;
	}
	CvMilitaryAI* pMilitaryAI = pPlayer->GetMilitaryAI();
	CvAssertMsg(pMilitaryAI, "No military AI");
	if(!pMilitaryAI)
	{
		return false;
	}

	MilitaryAIStrategyTypes eWarMobilizationStrategy = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_WAR_MOBILIZATION");
	if(pMilitaryAI->IsUsingStrategy(eWarMobilizationStrategy))
	{
		return true;
	}

	return false;
}

bool IsSafe(CvPlayerAI* pPlayer)
{
	CvAssertMsg(pPlayer, "Need a player");
	if(!pPlayer)
	{
		return false;
	}

	if(pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_WINNING)
	{
		return true;
	}
	else
	{
		CvMilitaryAI* pMilitaryAI = pPlayer->GetMilitaryAI();
		CvAssertMsg(pMilitaryAI, "No military AI");
		if(!pMilitaryAI)
		{
			return false;
		}

		MilitaryAIStrategyTypes eAtWarStrategy = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
		if(!pMilitaryAI->IsUsingStrategy(eAtWarStrategy))
		{
			return true;
		}

		return false;
	}
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveWriter(CvUnit* pGreatWriter)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;


#ifdef AUI_WARNING_FIXES
	if (!GetDiplomacyAI()->IsGoingForCultureVictory() && GetCulture()->GetPublicOpinionUnhappiness() > 10)
#else
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && !GetDiplomacyAI()->IsGoingForCultureVictory() && GetCulture()->GetPublicOpinionUnhappiness() > 10)
#endif
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_CULTURE_BLAST;
	}


	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && !GetDiplomacyAI()->IsGoingForCultureVictory() && GetPlayerPolicies()->CanGetAdvancedTenet())
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_CULTURE_BLAST;
	}


	GreatWorkType eGreatWork = pGreatWriter->GetGreatWork();
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GetEconomicAI()->GetBestGreatWorkCity(pGreatWriter->plot(), eGreatWork))
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}
	else
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_CULTURE_BLAST;
	}

	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveArtist(CvUnit* pGreatArtist)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;


#ifdef AUI_WARNING_FIXES
	if (!GetDiplomacyAI()->IsGoingForCultureVictory() && GetCulture()->GetPublicOpinionUnhappiness() > 10)
#else
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && !GetDiplomacyAI()->IsGoingForCultureVictory() && GetCulture()->GetPublicOpinionUnhappiness() > 10)
#endif
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}


	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && !GetDiplomacyAI()->IsGoingForCultureVictory() && PreparingForWar(this))
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}


	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GetDiplomacyAI()->IsGoingForSpaceshipVictory() && EconomicAIHelpers::IsTestStrategy_GS_SpaceshipHomestretch(this))
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}


	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GetPlayerTraits()->GetGoldenAgeMoveChange() > 0 && GetMilitaryAI()->GetNumberCivsAtWarWith() > 1 && !isGoldenAge())
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}


	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GetPlayerTraits()->GetGoldenAgeTourismModifier() > 0 && GetCulture()->GetNumCivsInfluentialOn() > 0)
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}


	GreatWorkType eGreatWork = pGreatArtist->GetGreatWork();
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GetEconomicAI()->GetBestGreatWorkCity(pGreatArtist->plot(), eGreatWork))
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}

	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && !isGoldenAge())
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}

	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveMusician(CvUnit* pGreatMusician)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;


	if (pGreatMusician->getArmyID() != FFreeList::INVALID_INDEX)
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_TOURISM_BLAST;
	}


	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GetDiplomacyAI()->IsGoingForCultureVictory() && GetCulture()->GetNumCivsInfluentialOn() > (GC.getGame().GetGameCulture()->GetNumCivsInfluentialForWin() / 2))
	{		
		CvPlot* pTarget = FindBestMusicianTargetPlot(pGreatMusician, true);
		if(pTarget)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_TOURISM_BLAST;
		}
	}


	GreatWorkType eGreatWork = pGreatMusician->GetGreatWork();
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GetEconomicAI()->GetBestGreatWorkCity(pGreatMusician->plot(), eGreatWork))
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}
	else
	{
		CvPlot* pTarget = FindBestMusicianTargetPlot(pGreatMusician, true);
		if(pTarget)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_TOURISM_BLAST;
		}
	}

	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveEngineer(CvUnit* pGreatEngineer)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;


	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
	{
		int iNextWonderWeight;
		BuildingTypes eNextWonderDesired = GetWonderProductionAI()->ChooseWonder(false                    , false                           , iNextWonderWeight);
		if(eNextWonderDesired != NO_BUILDING)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
		}
	}

	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GC.getGame().getGameTurn() <= ((GC.getGame().getEstimateEndTurn() * 3) / 4))
	{
		if (GetDiplomacyAI()->IsGoingForWorldConquest())
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
	}

	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && (GC.getGame().getGameTurn() - pGreatEngineer->getGameTurnCreated()) >= GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT())
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
	}

	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveMerchant(CvUnit* pGreatMerchant)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	bool bTheVeniceException = false;
	if (GetPlayerTraits()->IsNoAnnexing())
	{
		bTheVeniceException = true;
	}


	if(pGreatMerchant->getArmyID() != FFreeList::INVALID_INDEX)
	{
		return NO_GREAT_PEOPLE_DIRECTIVE_TYPE;
	}

#ifdef AUI_WARNING_FIXES
	if (GC.getGame().getGameTurn() <= ((GC.getGame().getEstimateEndTurn() * 2) / 4))
#else
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GC.getGame().getGameTurn() <= ((GC.getGame().getEstimateEndTurn() * 2) / 4))
#endif
	{
		if (GetDiplomacyAI()->IsGoingForDiploVictory() && !bTheVeniceException)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
	}


	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && IsSafe(this))
	{
		CvPlot* pTarget = FindBestMerchantTargetPlot(pGreatMerchant, true);
		if(pTarget)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
		}
	}

	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && (GC.getGame().getGameTurn() - pGreatMerchant->getGameTurnCreated()) >= GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT() && !bTheVeniceException)
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
	}

	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveScientist(CvUnit*                    )
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;


#ifdef AUI_WARNING_FIXES
	if (!IsSafe(this))
#else
	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && !IsSafe(this))
#endif
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}

	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GC.getGame().getGameTurn() <= ((GC.getGame().getEstimateEndTurn() * 1) / 4))
	{
		if(GetDiplomacyAI()->IsGoingForSpaceshipVictory())
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
	}

	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
	{

		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}

	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveGeneral(CvUnit* pGreatGeneral)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	SpecialUnitTypes eSpecialUnitGreatPerson = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_PEOPLE");

	int iGreatGeneralCount = 0;

	int iLoop;
	for(CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if(pLoopUnit->getSpecialUnitType() != eSpecialUnitGreatPerson)
		{
			continue;
		}

		if(pLoopUnit->AI_getUnitAIType() == UNITAI_GENERAL && pLoopUnit->GetGreatPeopleDirective() != GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE)
		{
			iGreatGeneralCount++;
		}
	}

	if(iGreatGeneralCount > 2 && pGreatGeneral->plot()->getOwner() == pGreatGeneral->getOwner())
	{

		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}

	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveProphet(CvUnit*)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	ReligionTypes eReligion = GetReligions()->GetReligionCreatedByPlayer();
	const CvReligion* pMyReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, GetID());


	if (pMyReligion && pMyReligion->m_bEnhanced)
	{

		if (GetReligionAI()->ChooseProphetConversionCity(false                                    ))
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_SPREAD_RELIGION;
		}
		else
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
	}



	else if (pMyReligion)
	{

		if (GetReligionAI()->ChooseProphetConversionCity(true                                    ))
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_SPREAD_RELIGION;
		}
#if defined(TRAITIFY)
		else if (GetPlayerTraits()->IsBannedUnitMission((MissionTypes)GC.getInfoTypeForString("MISSION_ENHANCE_RELIGION")))
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
#endif
		else
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
		}
	}


	else
	{

#if !defined(TRAITIFY)
		if (GC.getGame().GetGameReligions()->GetNumReligionsStillToFound() <= 0)
#else
		if ((GC.getGame().GetGameReligions()->GetNumReligionsStillToFound() <= 0) || (GetPlayerTraits()->IsBannedUnitMission((MissionTypes)GC.getInfoTypeForString("MISSION_FOUND_RELIGION"))))
#endif
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}


		else
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
		}
	}

	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveAdmiral(CvUnit*                  )
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	return eDirective;
}

bool CvPlayerAI::GreatMerchantWantsCash()
{


	bool bIsVenice = GetPlayerTraits()->IsNoAnnexing();
	if (bIsVenice)
	{
		if (getNumCities() >= 4)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return true;
}

CvPlot* CvPlayerAI::FindBestMerchantTargetPlot(CvUnit* pGreatMerchant, bool bOnlySafePaths)
{
	CvAssertMsg(pGreatMerchant, "pGreatMerchant is null");
	if(!pGreatMerchant)
	{
		return NULL;
	}

	int iBestTurnsToReach = MAX_INT;
	CvPlot* pBestTargetPlot = NULL;
	int iPathTurns;
	UnitHandle pMerchant = UnitHandle(pGreatMerchant);
	CvTeam& kTeam = GET_TEAM(getTeam());





	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if (!kPlayer.isMinorCiv())
		{
			continue;
		}














		CvPlot* pCSPlot = kPlayer.getStartingPlot();
		if (!pCSPlot)
		{
			continue;
		}

		if (!pCSPlot->isRevealed(getTeam()))
		{
			continue;
		}


		bool bMinorCivApproachIsCorrect = (GetDiplomacyAI()->GetMinorCivApproach(kPlayer.GetID()) != MINOR_CIV_APPROACH_CONQUEST);
		bool bNotAtWar = !kTeam.isAtWar(kPlayer.getTeam());
		bool bNotPlanningAWar = GetDiplomacyAI()->GetWarGoal(kPlayer.GetID()) == NO_WAR_GOAL_TYPE;

		if(bMinorCivApproachIsCorrect && bNotAtWar && bNotPlanningAWar)
		{

			for(int jJ = 0; jJ < NUM_DIRECTION_TYPES; jJ++)
			{
				CvPlot* pAdjacentPlot = plotDirection(pCSPlot->getX(), pCSPlot->getY(), ((DirectionTypes)jJ));
				if(pAdjacentPlot != NULL)
				{


					bool bRightOwner = (pAdjacentPlot->getOwner() == (PlayerTypes)iI);
					bool bIsRevealed = pAdjacentPlot->isRevealed(getTeam());
					if(bRightOwner && bIsRevealed)
					{
						iPathTurns = TurnsToReachTarget(pMerchant, pAdjacentPlot, true                , !bOnlySafePaths                );
						if(iPathTurns < iBestTurnsToReach)
						{
							iBestTurnsToReach = iPathTurns;
							pBestTargetPlot = pAdjacentPlot;
						}
					}
				}
			}
		}
	}

	return pBestTargetPlot;
}

CvPlot* CvPlayerAI::FindBestMusicianTargetPlot(CvUnit* pGreatMusician, bool bOnlySafePaths)
{
	CvAssertMsg(pGreatMusician, "pGreatMusician is null");
	if(!pGreatMusician)
	{
		return NULL;
	}

	int iBestTurnsToReach = MAX_INT;
	CvPlot* pBestTargetPlot = NULL;
	CvCity* pBestTargetCity = NULL;
	int iPathTurns;
	UnitHandle pMusician = UnitHandle(pGreatMusician);


	PlayerTypes eTargetPlayer = GetCulture()->GetCivLowestInfluence(true                      );
	if (eTargetPlayer == NO_PLAYER)
	{
		return NULL;
	}

	CvPlayer &kTargetPlayer = GET_PLAYER(eTargetPlayer);


	int iLoop;
	CvCity *pLoopCity;
	for(pLoopCity = kTargetPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kTargetPlayer.nextCity(&iLoop))
	{

		for(int jJ = 0; jJ < NUM_DIRECTION_TYPES; jJ++)
		{
			CvPlot* pAdjacentPlot = plotDirection(pLoopCity->getX(), pLoopCity->getY(), ((DirectionTypes)jJ));
			if(pAdjacentPlot != NULL)
			{

				bool bRightOwner = (pAdjacentPlot->getOwner() == eTargetPlayer);
				bool bIsRevealed = pAdjacentPlot->isRevealed(getTeam());
				if(bRightOwner && bIsRevealed)
				{
					iPathTurns = TurnsToReachTarget(pMusician, pAdjacentPlot, true                , !bOnlySafePaths                );
					if(iPathTurns < iBestTurnsToReach)
					{
						iBestTurnsToReach = iPathTurns;
						pBestTargetCity = pLoopCity;
					}
				}
			}
		}
	}


	if (pBestTargetCity)
	{
		iBestTurnsToReach = MAX_INT;
		CvPlot *pLoopPlot;
		for(int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
		{
			pLoopPlot = plotCity(pBestTargetCity->getX(), pBestTargetCity->getY(), iJ);
			if(pLoopPlot != NULL)
			{

				bool bRightOwner = (pLoopPlot->getOwner() == eTargetPlayer);
				bool bIsRevealed = pLoopPlot->isRevealed(getTeam());
				if(bRightOwner && bIsRevealed)
				{
					iPathTurns = TurnsToReachTarget(pMusician, pLoopPlot, true                , !bOnlySafePaths                );
					if(iPathTurns < iBestTurnsToReach)
					{
						iBestTurnsToReach = iPathTurns;
						pBestTargetPlot = pLoopPlot;
					}
				}
			}	
		}
	}

	return pBestTargetPlot;
}

CvPlot* CvPlayerAI::FindBestArtistTargetPlot(CvUnit* pGreatArtist, int& iResultScore)
{
	CvAssertMsg(pGreatArtist, "pGreatArtist is null");
	if(!pGreatArtist)
	{
		return NULL;
	}

	iResultScore = 0;

#ifndef AUI_WARNING_FIXES
	CvPlotsVector& m_aiPlots = GetPlots();
#endif

	CvPlot* pBestPlot = NULL;
	int iBestScore = 0;


	const uint nPlots = m_aiPlots.size();
	for(uint ui = 0; ui < nPlots; ui++)
	{
		if(m_aiPlots[ui] == -1)
		{
			continue;
		}

		CvPlot* pPlot = GC.getMap().plotByIndex(m_aiPlots[ui]);

		if(pPlot->isWater())
		{
			continue;
		}

		if(!pPlot->IsAdjacentOwnedByOtherTeam(getTeam()))
		{
			continue;
		}


		ResourceTypes eResource = pPlot->getResourceType();
		if(eResource != NO_RESOURCE)
		{
			CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
			if(pkResource != NULL)
			{
				if (pkResource->getResourceUsage() == RESOURCEUSAGE_LUXURY)
				{
					continue;
				}
			}
		}


		FeatureTypes eFeature = pPlot->getFeatureType();
		if (eFeature != NO_FEATURE && GC.getFeatureInfo(eFeature)->isNoImprovement())
		{
			continue;
		}


		ImprovementTypes eImprovement = (ImprovementTypes)pPlot->getImprovementType();
		if (eImprovement != NO_IMPROVEMENT)
		{
			CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
			if(pkImprovementInfo)
			{
				if (pkImprovementInfo->GetCultureBombRadius() > 0)
				{
					continue;
				}
			}
		}

		int iScore = 0;

		for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			CvPlot* pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

			if(pAdjacentPlot == NULL)
			{
				continue;
			}


			if(pAdjacentPlot->getTeam() == NO_TEAM || pAdjacentPlot->getTeam() == getTeam())
			{
				continue;
			}


			if(pAdjacentPlot->getPlotCity())
			{
				continue;
			}

			const PlayerTypes eOtherPlayer = pAdjacentPlot->getOwner();
			if(GET_PLAYER(eOtherPlayer).isMinorCiv())
			{
				MinorCivApproachTypes eMinorApproach = GetDiplomacyAI()->GetMinorCivApproach(eOtherPlayer);

				if(eMinorApproach != MINOR_CIV_APPROACH_CONQUEST && eMinorApproach != MINOR_CIV_APPROACH_IGNORE)
				{
					iScore = 0;
					break;
				}
			}
			else
			{
				MajorCivApproachTypes eMajorApproach = GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, true);
				DisputeLevelTypes eLandDisputeLevel = GetDiplomacyAI()->GetLandDisputeLevel(eOtherPlayer);

				bool bTicked = eMajorApproach == MAJOR_CIV_APPROACH_HOSTILE;
				bool bTickedAboutLand = eMajorApproach == MAJOR_CIV_APPROACH_NEUTRAL && (eLandDisputeLevel == DISPUTE_LEVEL_STRONG || eLandDisputeLevel == DISPUTE_LEVEL_FIERCE);


				if(!bTicked && !bTickedAboutLand)
				{
					iScore = 0;
					break;
				}
			}

			eResource = pAdjacentPlot->getResourceType();
			if(eResource != NO_RESOURCE)
			{
				iScore += GetBuilderTaskingAI()->GetResourceWeight(eResource, NO_IMPROVEMENT, pAdjacentPlot->getNumResource()) * 10;
			}
			else
			{
				for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
				{
					iScore += pAdjacentPlot->getYield((YieldTypes)iYield);
				}
			}
		}

		if(iScore > iBestScore)
		{
			iBestScore = iScore;
			pBestPlot = pPlot;
		}
	}

	iResultScore = iBestScore;
	return pBestPlot;
}
