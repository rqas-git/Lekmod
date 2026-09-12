







#include "CvGameCoreDLLPCH.h"
#include "CvGlobals.h"
#include "CvArea.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvRandom.h"
#include "CvTeam.h"
#include "CvGameCoreUtils.h"
#include "CvPlayerAI.h"
#include "CvPlayer.h"
#include "CvGameCoreUtils.h"
#include "CvInfos.h"
#include "CvAStar.h"
#include "CvGameTextMgr.h"
#include "CvDiplomacyAI.h"
#include "CvEconomicAI.h"
#include "CvMilitaryAI.h"
#include "CvCitySpecializationAI.h"
#include "CvWonderProductionAI.h"
#include "CvGrandStrategyAI.h"
#include "CvDiplomacyAI.h"
#include "CvTechAI.h"
#include "CvFlavorManager.h"
#include "CvHomelandAI.h"
#include "CvMinorCivAI.h"
#include "CvDealAI.h"
#include "CvImprovementClasses.h"
#include "CvBuilderTaskingAI.h"
#include "CvDangerPlots.h"
#include "CvCityConnections.h"
#include "CvNotifications.h"
#include "CvDiplomacyRequests.h"
#include "cvStopWatch.h"
#include "CvTypes.h"

#include "ICvDLLUserInterface.h"
#include "CvEnumSerialization.h"
#include "FStlContainerSerialization.h"
#include <sstream>

#include "CvInternalGameCoreUtils.h"
#include "CvAchievementUnlocker.h"
#include "CvInfosSerializationHelper.h"

#include "CvDllCity.h"
#include "CvGoodyHuts.h"
#include "CvVotingClasses.h"

#define LINT_WARNINGS_ONLY
#include "LintFree.h"






const int g_CurrentCvPlayerVersion = 17;


bool isEmpty(const char* szString)
{
	return szString == NULL || szString[0] == '\0';
}



namespace FSerialization
{
void SyncPlayer()
{
	if(GC.getGame().isNetworkMultiPlayer())
	{
		PlayerTypes eAuthoritativePlayerID = GC.getGame().getActivePlayer();
		CvPlayer& authoritativePlayer = GET_PLAYER(eAuthoritativePlayerID);
		const FAutoArchive& archive = authoritativePlayer.getSyncArchive();
		if(archive.hasDeltas())
		{
			FMemoryStream ms;
			std::vector<std::pair<std::string, std::string> > callStacks;
			archive.saveDelta(ms, callStacks);
			gDLL->sendPlayerSyncCheck(eAuthoritativePlayerID, ms, callStacks);
		}



		if(gDLL->IsHost())
		{
			for(int i = 0; i < MAX_PLAYERS; ++i)
			{
				CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));
				if(!player.isHuman() && player.isAlive())
				{
					const FAutoArchive& aiArchive = player.getSyncArchive();
					FMemoryStream ms;
					std::vector<std::pair<std::string, std::string> > callStacks;
					aiArchive.saveDelta(ms, callStacks);
					gDLL->sendPlayerSyncCheck(static_cast<PlayerTypes>(i), ms, callStacks);
				}
			}
		}
	}
}



void ClearPlayerDeltas()
{
	int i = 0;
	for(i = 0; i < MAX_PLAYERS; ++i)
	{
		CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));
		FAutoArchive& archive = player.getSyncArchive();
		archive.clearDelta();
	}
}
}


CvPlayer::CvPlayer() :
	m_syncArchive(*this)
#ifdef GAME_AUTOPAUSE_ON_ACTIVE_DISCONNECT_IF_NOT_SEQUENTIAL
	, m_bIsDisconnected("CvPlayer::m_bIsDisconnected", m_syncArchive)
#endif
	, m_iStartingX("CvPlayer::m_iStartingX", m_syncArchive)
	, m_iStartingY("CvPlayer::m_iStartingY", m_syncArchive)
	, m_iTotalPopulation("CvPlayer::m_iTotalPopulation", m_syncArchive, true)
	, m_iTotalLand("CvPlayer::m_iTotalLand", m_syncArchive)
	, m_iTotalLandScored("CvPlayer::m_iTotalLandScored", m_syncArchive)
	, m_iJONSCulturePerTurnForFree("CvPlayer::m_iJONSCulturePerTurnForFree", m_syncArchive)
	, m_iJONSCulturePerTurnFromMinorCivs("CvPlayer::m_iJONSCulturePerTurnFromMinorCivs", m_syncArchive)
	, m_iJONSCultureCityModifier("CvPlayer::m_iJONSCultureCityModifier", m_syncArchive)
#ifdef AUI_PLAYER_FIX_JONS_CULTURE_IS_T100
	, m_iJONSCultureT100("CvPlayer::m_iJONSCultureT100", m_syncArchive, true)
	, m_iJONSCultureEverGeneratedT100("CvPlayer::m_iJONSCultureEverGeneratedT100", m_syncArchive)
#else
	, m_iJONSCulture("CvPlayer::m_iJONSCulture", m_syncArchive, true)
	, m_iJONSCultureEverGenerated("CvPlayer::m_iJONSCulture", m_syncArchive)
#endif
#if !defined(LEKMOD_EXPERIMENTAL_CHANGES)
	, m_iCulturePerWonder("CvPlayer::m_iCulturePerWonder", m_syncArchive)
#endif
	, m_iCultureWonderMultiplier("CvPlayer::m_iCultureWonderMultiplier", m_syncArchive)
	, m_iCulturePerTechResearched("CvPlayer::m_iCulturePerTechResearched", m_syncArchive)
	, m_iFaith(0)
	, m_iFaithEverGenerated(0)
	, m_iHappiness("CvPlayer::m_iHappiness", m_syncArchive)
	, m_iUprisingCounter("CvPlayer::m_iUprisingCounter", m_syncArchive)
	, m_iExtraHappinessPerLuxury("CvPlayer::m_iExtraHappinessPerLuxury", m_syncArchive)
	, m_iUnhappinessFromUnits("CvPlayer::m_iUnhappinessFromUnits", m_syncArchive)
	, m_iUnhappinessFromUnitsMod("CvPlayer::m_iUnhappinessFromUnitsMod", m_syncArchive)
	, m_iUnhappinessMod("CvPlayer::m_iUnhappinessMod", m_syncArchive)
	, m_iCityCountUnhappinessMod("CvPlayer::m_iCityCountUnhappinessMod", m_syncArchive)
	, m_iOccupiedPopulationUnhappinessMod("CvPlayer::m_iOccupiedPopulationUnhappinessMod", m_syncArchive)
	, m_iCapitalUnhappinessMod("CvPlayer::m_iCapitalUnhappinessMod", m_syncArchive)
	, m_iCityRevoltCounter("CvPlayer::m_iCityRevoltCounter", m_syncArchive)
	, m_iHappinessPerGarrisonedUnitCount("CvPlayer::m_iHappinessPerGarrisonedUnitCount", m_syncArchive)
	, m_iHappinessPerTradeRouteCount("CvPlayer::m_iHappinessPerTradeRouteCount", m_syncArchive)
#ifdef NQ_RAIL_CONNECTION_HAPPINESS_FROM_POLICIES
	, m_iHappinessPerRailConnectionCount("CvPlayer::m_iHappinessPerRailConnectionCount", m_syncArchive)
#endif
	, m_iHappinessPerXPopulation(0)
	, m_iHappinessFromLeagues(0)
	, m_iEspionageModifier(0)
	, m_iSpyStartingRank(0)
#ifdef ENHANCED_GRAPHS
	, m_iNumStolenScience(0)
	, m_iNumTrainedUnits(0)
	, m_iNumKilledUnits(0)
	, m_iNumLostUnits(0)
	, m_iUnitsDamageDealt(0)
	, m_iUnitsDamageTaken(0)
	, m_iCitiesDamageDealt(0)
	, m_iCitiesDamageTaken(0)
	, m_iNumScientistsTotal(0)
	, m_iNumEngineersTotal(0)
	, m_iNumMerchantsTotal(0)
	, m_iNumWritersTotal(0)
	, m_iNumArtistsTotal(0)
	, m_iNumMusiciansTotal(0)
	, m_iNumGeneralsTotal(0)
	, m_iNumAdmiralsTotal(0)
	, m_iNumProphetsTotal(0)
	, m_iProductionGoldFromWonders(0)
	, m_iNumChops(0)
	, m_iNumTimesOpenedDemographics(0)
	, m_iUnitsDamageHealed(0)
	, m_iTurnsStagnated(0)
	, m_iGoldSpentBuys(0)
	, m_iGoldSpentUpgrades(0)
	, m_bMayaBoostScientist(false)
	, m_bMayaBoostEngineers(false)
	, m_bMayaBoostMerchants(false)
	, m_bMayaBoostWriters(false)
	, m_bMayaBoostArtists(false)
	, m_bMayaBoostMusicians(false)
#endif
	, m_iExtraLeagueVotes(0)
#if defined(TRAITIFY)
	, m_iTraitExtraLeagueVotes(0)
#endif
#if defined(MISC_CHANGES)
	, m_iTechExtraLeagueVotes(0)
	, m_iPolicyExtraLeagueVotes(0)
	, m_iMiscTradeRoutes(0)
#endif
	, m_iSpecialPolicyBuildingHappiness("CvPlayer::m_iSpecialPolicyBuildingHappiness", m_syncArchive)
	, m_iWoundedUnitDamageMod("CvPlayer::m_iWoundedUnitDamageMod", m_syncArchive)
	, m_iUnitUpgradeCostMod("CvPlayer::m_iUnitUpgradeCostMod", m_syncArchive)
	, m_iBarbarianCombatBonus("CvPlayer::m_iBarbarianCombatBonus", m_syncArchive)
	, m_iAlwaysSeeBarbCampsCount("CvPlayer::m_iAlwaysSeeBarbCampsCount", m_syncArchive)
	, m_iHappinessFromBuildings("CvPlayer::m_iHappinessFromBuildings", m_syncArchive)
	, m_iHappinessPerCity("CvPlayer::m_iHappinessPerCity", m_syncArchive)
	, m_iHappinessPerXPolicies(0)
	, m_iAdvancedStartPoints("CvPlayer::m_iAdvancedStartPoints", m_syncArchive)
	, m_iAttackBonusTurns("CvPlayer::m_iAttackBonusTurns", m_syncArchive)
	, m_iCultureBonusTurns(0)
	, m_iTourismBonusTurns(0)
	, m_iGoldenAgeProgressMeter("CvPlayer::m_iGoldenAgeProgressMeter", m_syncArchive, true)
	, m_iGoldenAgeMeterMod("CvPlayer::m_iGoldenAgeMeterMod", m_syncArchive)
	, m_iNumGoldenAges("CvPlayer::m_iNumGoldenAges", m_syncArchive)
	, m_iGoldenAgeTurns("CvPlayer::m_iGoldenAgeTurns", m_syncArchive)
	, m_iNumUnitGoldenAges("CvPlayer::m_iNumUnitGoldenAges", m_syncArchive)
	, m_iStrikeTurns("CvPlayer::m_iStrikeTurns", m_syncArchive)
	, m_iGoldenAgeModifier("CvPlayer::m_iGoldenAgeModifier", m_syncArchive)
	, m_iGreatPeopleCreated("CvPlayer::m_iGreatPeopleCreated", m_syncArchive)
	, m_iGreatGeneralsCreated("CvPlayer::m_iGreatGeneralsCreated", m_syncArchive)
	, m_iGreatAdmiralsCreated(0)
	, m_iGreatWritersCreated(0)
	, m_iGreatArtistsCreated(0)
	, m_iGreatMusiciansCreated(0)
#ifdef NQ_FREE_SETTLERS_FROM_BELIEF
	, m_bHasUsedReligiousSettlements(false)
#endif
#ifdef NQ_DEUS_VULT
	, m_bHasUsedDeusVult(false)
#endif
#ifdef NQ_GOLDEN_AGE_TURNS_FROM_BELIEF
	, m_bHasUsedDharma(false)
#endif
	, m_iGreatScientistsCreated(0)
	, m_iGreatEngineersCreated(0)
	, m_iGreatMerchantsCreated(0)
	, m_iGreatProphetsCreated(0)
	, m_iMerchantsFromFaith(0)
	, m_iScientistsFromFaith(0)
	, m_iWritersFromFaith(0)
	, m_iArtistsFromFaith(0)
	, m_iMusiciansFromFaith(0)
	, m_iGeneralsFromFaith(0)
	, m_iAdmiralsFromFaith(0)
	, m_iEngineersFromFaith(0)
	, m_iGreatPeopleThresholdModifier("CvPlayer::m_iGreatPeopleThresholdModifier", m_syncArchive)
	, m_iGreatGeneralsThresholdModifier("CvPlayer::m_iGreatGeneralsThresholdModifier", m_syncArchive)
	, m_iGreatAdmiralsThresholdModifier(0)
	, m_iGreatGeneralCombatBonus(0)
	, m_iAnarchyNumTurns("CvPlayer::m_iAnarchyNumTurns", m_syncArchive)
	, m_iPolicyCostModifier("CvPlayer::m_iPolicyCostModifier", m_syncArchive)
	, m_iGreatPeopleRateModifier("CvPlayer::m_iGreatPeopleRateModifier", m_syncArchive)
	, m_iGreatPeopleRateModFromBldgs("CvPlayer::m_iGreatPeopleRateModFromBldgs", m_syncArchive)
	, m_iGreatGeneralRateModifier("CvPlayer::m_iGreatGeneralRateModifier", m_syncArchive)
	, m_iGreatGeneralRateModFromBldgs("CvPlayer::m_iGreatGeneralRateModFromBldgs", m_syncArchive)
	, m_iDomesticGreatGeneralRateModifier("CvPlayer::m_iDomesticGreatGeneralRateModifier", m_syncArchive)
	, m_iDomesticGreatGeneralRateModFromBldgs("CvPlayer::m_iDomesticGreatGeneralRateModFromBldgs", m_syncArchive)
	, m_iGreatScientistBeakerModifier(0)
	, m_iGreatPersonExpendGold(0)
	, m_iMaxGlobalBuildingProductionModifier("CvPlayer::m_iMaxGlobalBuildingProductionModifier", m_syncArchive)
	, m_iMaxTeamBuildingProductionModifier("CvPlayer::m_iMaxTeamBuildingProductionModifier", m_syncArchive)
	, m_iMaxPlayerBuildingProductionModifier("CvPlayer::m_iMaxPlayerBuildingProductionModifier", m_syncArchive)
	, m_iFreeExperience("CvPlayer::m_iFreeExperience", m_syncArchive)
	, m_iFreeExperienceFromBldgs("CvPlayer::m_iFreeExperienceFromBldgs", m_syncArchive)
	, m_iFreeExperienceFromMinors("CvPlayer::m_iFreeExperienceFromMinors", m_syncArchive)
	, m_iFeatureProductionModifier("CvPlayer::m_iFeatureProductionModifier", m_syncArchive)
	, m_iWorkerSpeedModifier("CvPlayer::m_iWorkerSpeedModifier", m_syncArchive)
	, m_iImprovementCostModifier("CvPlayer::m_iImprovementCostModifier", m_syncArchive)
	, m_iImprovementUpgradeRateModifier("CvPlayer::m_iImprovementUpgradeRateModifier", m_syncArchive)
	, m_iSpecialistProductionModifier("CvPlayer::m_iSpecialistProductionModifier", m_syncArchive)
	, m_iMilitaryProductionModifier("CvPlayer::m_iMilitaryProductionModifier", m_syncArchive)
	, m_iSpaceProductionModifier("CvPlayer::m_iSpaceProductionModifier", m_syncArchive)
	, m_iCityDefenseModifier("CvPlayer::m_iCityDefenseModifier", m_syncArchive)
	, m_iUnitFortificationModifier("CvPlayer::m_iUnitFortificationModifier", m_syncArchive)
	, m_iUnitBaseHealModifier("CvPlayer::m_iUnitBaseHealModifier", m_syncArchive)
	, m_iWonderProductionModifier("CvPlayer::m_iWonderProductionModifier", m_syncArchive)
	, m_iSettlerProductionModifier("CvPlayer::m_iSettlerProductionModifier", m_syncArchive)
	, m_iCapitalSettlerProductionModifier("CvPlayer::m_iCapitalSettlerProductionModifier", m_syncArchive)
	, m_iUnitProductionMaintenanceMod("CvPlayer::m_iUnitProductionMaintenanceMod", m_syncArchive)
	, m_iPolicyCostBuildingModifier("CvPlayer::m_iPolicyCostBuildingModifier", m_syncArchive)
	, m_iPolicyCostMinorCivModifier("CvPlayer::m_iPolicyCostMinorCivModifier", m_syncArchive)
#if !defined(LEK_YIELD_TOURISM)
	, m_iInfluenceSpreadModifier(0)
#endif
	, m_iExtraVotesPerDiplomat(0)
	, m_iNumNukeUnits("CvPlayer::m_iNumNukeUnits", m_syncArchive)
	, m_iNumOutsideUnits("CvPlayer::m_iNumOutsideUnits", m_syncArchive, true)
	, m_iBaseFreeUnits("CvPlayer::m_iBaseFreeUnits", m_syncArchive)
	, m_iBaseFreeMilitaryUnits("CvPlayer::m_iBaseFreeMilitaryUnits", m_syncArchive)
	, m_iFreeUnitsPopulationPercent("CvPlayer::m_iFreeUnitsPopulationPercent", m_syncArchive)
	, m_iFreeMilitaryUnitsPopulationPercent("CvPlayer::m_iFreeMilitaryUnitsPopulationPercent", m_syncArchive)
	, m_iGoldPerUnit("CvPlayer::m_iGoldPerUnit", m_syncArchive)
	, m_iGoldPerMilitaryUnit("CvPlayer::m_iGoldPerMilitaryUnit", m_syncArchive)
	, m_iRouteGoldMaintenanceMod("CvPlayer::m_iRouteGoldMaintenanceMod", m_syncArchive)
	, m_iBuildingGoldMaintenanceMod("CvPlayer::m_iBuildingGoldMaintenanceMod", m_syncArchive)
	, m_iUnitGoldMaintenanceMod("CvPlayer::m_iUnitGoldMaintenanceMod", m_syncArchive)
	, m_iUnitSupplyMod("CvPlayer::m_iUnitSupplyMod", m_syncArchive)
	, m_iExtraUnitCost("CvPlayer::m_iExtraUnitCost", m_syncArchive)
	, m_iNumMilitaryUnits("CvPlayer::m_iNumMilitaryUnits", m_syncArchive)
	, m_iHappyPerMilitaryUnit("CvPlayer::m_iHappyPerMilitaryUnit", m_syncArchive)
	, m_iHappinessToCulture("CvPlayer::m_iHappinessToCulture", m_syncArchive)
	, m_iHappinessToScience("CvPlayer::m_iHappinessToScience", m_syncArchive)
#ifdef NQ_GOLD_TO_SCIENCE_FROM_POLICIES
	, m_iGoldToScience("CvPlayer::m_iGoldToScience", m_syncArchive)
#endif
	, m_iHalfSpecialistUnhappinessCount("CvPlayer::m_iHalfSpecialistUnhappinessCount", m_syncArchive)
	, m_iHalfMoreSpecialistUnhappinessCount("CvPlayer::m_iHalfMoreSpecialistUnhappinessCount", m_syncArchive)
	, m_iHalfSpecialistFoodCount("CvPlayer::m_iHalfSpecialistFoodCount", m_syncArchive)
	, m_iMilitaryFoodProductionCount("CvPlayer::m_iMilitaryFoodProductionCount", m_syncArchive)
	, m_iGoldenAgeCultureBonusDisabledCount(0)
	, m_iSecondReligionPantheonCount(0)
	, m_iEnablesSSPartHurryCount(0)
#ifdef NQ_DIABLE_RESISTANCE_TIME_VIA_POLICIES
	, m_iDisablesResistanceTimeCount(0)
#endif
#ifdef NQ_PATRIOTIC_WAR
	, m_iDoubleTrainedMilitaryLandUnitCount(0)
#endif
#ifdef NQ_WAR_HERO
	, m_iWarHeroCount(0)
#endif
#ifdef NQ_IGNORE_PUPPETS_FOR_RESEARCH_COSTS_FROM_POLICIES
	, m_iIgnorePuppetsForResearchCostsCount(0)
#endif
#ifdef NQ_POLICY_TOGGLE_NO_MINOR_DOW_IF_FRIENDS
	, m_iNoMinorDOWIfFriendsCount(0)
#endif
#ifdef NQ_ALLOW_PUPPET_PURCHASING_FROM_POLICIES
	, m_iAllowPuppetPurchasingCount(0)
#endif
#if defined(LEKMOD_GREAT_FIREWALL_PLAYER_EFFECT)
	, m_iInfluenceNullificationCount(0)
#endif
	, m_iEnablesSSPartPurchaseCount(0)
	, m_iConscriptCount("CvPlayer::m_iConscriptCount", m_syncArchive)
	, m_iMaxConscript("CvPlayer::m_iMaxConscript", m_syncArchive)
	, m_iHighestUnitLevel("CvPlayer::m_iHighestUnitLevel", m_syncArchive)
	, m_iOverflowResearch("CvPlayer::m_iOverflowResearch", m_syncArchive, true)
	, m_iExpModifier("CvPlayer::m_iExpModifier", m_syncArchive)
	, m_iExpInBorderModifier("CvPlayer::m_iExpInBorderModifier", m_syncArchive)
	, m_iLevelExperienceModifier("CvPlayer::m_iLevelExperienceModifier", m_syncArchive)
	, m_iMinorQuestFriendshipMod("CvPlayer::m_iMinorQuestFriendshipMod", m_syncArchive)
	, m_iMinorGoldFriendshipMod("CvPlayer::m_iMinorGoldFriendshipMod", m_syncArchive)
	, m_iMinorFriendshipMinimum("CvPlayer::m_iMinorFriendshipMinimum", m_syncArchive)
	, m_iMinorFriendshipDecayMod("CvPlayer::m_iMinorFriendshipDecayMod", m_syncArchive)
	, m_iMinorScienceAlliesCount("CvPlayer::m_iMinorScienceAlliesCount", m_syncArchive)
	, m_iMinorResourceBonusCount("CvPlayer::m_iMinorResourceBonusCount", m_syncArchive)
	, m_iAbleToAnnexCityStatesCount(0)
	, m_iFreeSpecialist("CvPlayer::m_iFreeSpecialist", m_syncArchive)
	, m_iCultureBombTimer("CvPlayer::m_iCultureBombTimer", m_syncArchive)
	, m_iConversionTimer("CvPlayer::m_iConversionTimer", m_syncArchive)
	, m_iCapitalCityID("CvPlayer::m_iCapitalCityID", m_syncArchive)
	, m_iCitiesLost("CvPlayer::m_iCitiesLost", m_syncArchive)
	, m_iMilitaryMight("CvPlayer::m_iMilitaryMight", m_syncArchive)
	, m_iEconomicMight("CvPlayer::m_iEconomicMight", m_syncArchive)
	, m_iTurnMightRecomputed("CvPlayer::m_iTurnMightRecomputed", m_syncArchive)
	, m_iNewCityExtraPopulation("CvPlayer::m_iNewCityExtraPopulation", m_syncArchive)
	, m_iFreeFoodBox("CvPlayer::m_iFreeFoodBox", m_syncArchive)
	, m_iScenarioScore1("CvPlayer::m_iScenarioScore1", m_syncArchive)
	, m_iScenarioScore2("CvPlayer::m_iScenarioScore2", m_syncArchive)
	, m_iScenarioScore3("CvPlayer::m_iScenarioScore3", m_syncArchive)
	, m_iScenarioScore4("CvPlayer::m_iScenarioScore4", m_syncArchive)
	, m_iScoreFromFutureTech("CvPlayer::m_iScoreFromFutureTech", m_syncArchive)
	, m_iCombatExperience("CvPlayer::m_iCombatExperience", m_syncArchive)
	, m_iLifetimeCombatExperience(0)
	, m_iNavalCombatExperience(0)
	, m_iPopRushHurryCount("CvPlayer::m_iPopRushHurryCount", m_syncArchive)
	, m_iTotalImprovementsBuilt("CvPlayer::m_iTotalImprovementsBuilt", m_syncArchive)
	, m_iNextOperationID("CvPlayer::m_iNextOperationID", m_syncArchive)
	, m_iCostNextPolicy("CvPlayer::m_iCostNextPolicy", m_syncArchive)
	, m_iNumBuilders("CvPlayer::m_iNumBuilders", m_syncArchive, true)
	, m_iMaxNumBuilders("CvPlayer::m_iMaxNumBuilders", m_syncArchive)
	, m_iCityStrengthMod("CvPlayer::m_iCityStrengthMod", m_syncArchive)
	, m_iCityGrowthMod("CvPlayer::m_iCityGrowthMod", m_syncArchive)
	, m_iCapitalGrowthMod("CvPlayer::m_iCapitalGrowthMod", m_syncArchive)
	, m_iNumPlotsBought("CvPlayer::m_iNumPlotsBought", m_syncArchive)
	, m_iPlotGoldCostMod("CvPlayer::m_iPlotGoldCostMod", m_syncArchive)
	, m_iPlotCultureCostModifier("CvPlayer::m_iPlotCultureCostModifier", m_syncArchive)
	, m_iPlotCultureExponentModifier(0)
	, m_iNumCitiesPolicyCostDiscount(0)
	, m_iNumCitiesResearchCostDiscount(0)
	, m_iGarrisonedCityRangeStrikeModifier(0)
	, m_iGarrisonFreeMaintenanceCount(0)
	, m_iNumCitiesFreeAestheticsSchools(0)
	, m_iNumCitiesFreePietyGardens(0)
	, m_iNumCitiesFreeWalls(0)
	, m_iNumCitiesFreeCultureBuilding(0)
	, m_iNumCitiesFreeFoodBuilding(0)
#if defined(LEKMOD_FIX_PATRO_FOOD)
	, m_iCityStateBonusModifier(0)
#endif
	, m_iUnitPurchaseCostModifier("CvPlayer::m_iUnitPurchaseCostModifier", m_syncArchive)
	, m_iAllFeatureProduction("CvPlayer::m_iAllFeatureProduction", m_syncArchive)
	, m_iCityDistanceHighwaterMark("CvPlayer::m_iCityDistanceHighwaterMark", m_syncArchive)
	, m_iOriginalCapitalX("CvPlayer::m_iOriginalCapitalX", m_syncArchive)
	, m_iOriginalCapitalY("CvPlayer::m_iOriginalCapitalY", m_syncArchive)
	, m_iNumWonders("CvPlayer::m_iNumWonders", m_syncArchive)
	, m_iNumPolicies("CvPlayer::m_iNumPolicies", m_syncArchive)
	, m_iNumGreatPeople("CvPlayer::m_iNumGreatPeople", m_syncArchive)
	, m_uiStartTime("CvPlayer::m_uiStartTime", m_syncArchive)
#ifdef NQ_CHEAT_FIRST_ROYAL_LIBRARY_COMES_WITH_GREAT_WORK
	, m_bHasEverBuiltRoyalLibrary("CvPlayer::m_bHasEverBuiltRoyalLibrary", m_syncArchive)
#endif
	, m_bHasBetrayedMinorCiv("CvPlayer::m_bHasBetrayedMinorCiv", m_syncArchive)
	, m_bAlive("CvPlayer::m_bAlive", m_syncArchive)
	, m_bEverAlive("CvPlayer::m_bEverAlive", m_syncArchive)
	, m_bBeingResurrected(false)
	, m_bTurnActive("CvPlayer::m_bTurnActive", m_syncArchive, false, true)
	, m_bAutoMoves("CvPlayer::m_bAutoMoves", m_syncArchive, false, true)
	, m_bEndTurn("CvPlayer::m_bEndTurn", m_syncArchive, false, true)
#ifdef AUI_GAME_BETTER_HYBRID_MODE
	, m_iTurnOrder(0)
#else
	, m_bDynamicTurnsSimultMode(true)
#endif
	, m_bPbemNewTurn("CvPlayer::m_bPbemNewTurn", m_syncArchive)
	, m_bExtendedGame("CvPlayer::m_bExtendedGame", m_syncArchive)
	, m_bFoundedFirstCity("CvPlayer::m_bFoundedFirstCity", m_syncArchive)
	, m_iNumCitiesFounded(0)
	, m_bStrike("CvPlayer::m_bStrike", m_syncArchive)
	, m_bCramped("CvPlayer::m_bCramped", m_syncArchive)
	, m_bLostCapital("CvPlayer::m_bLostCapital", m_syncArchive)
	, m_eConqueror(NO_PLAYER)
	, m_bHasAdoptedStateReligion("CvPlayer::m_bHasAdoptedStateReligion", m_syncArchive)
	, m_bAlliesGreatPersonBiasApplied("CvPlayer::m_bAlliesGreatPersonBiasApplied", m_syncArchive)
#if defined(LEKMOD_PROMO_YIELD_FROM_CONVERSION) && defined(LEKMOD_PROMO_CONVERSION_MAJORITY_ONLY_ONCE)
	, m_aiConversionMajorityOnceUsedKeys("CvPlayer::m_aiConversionMajorityOnceUsedKeys", m_syncArchive)
#endif
	, m_eID("CvPlayer::m_eID", m_syncArchive)
	, m_ePersonalityType("CvPlayer::m_ePersonalityType", m_syncArchive)
#if defined(LEKMOD_EXPERIMENTAL_CHANGES)
	, m_aiWorldWonderYieldChanges("CvPlayer::m_aiWorldWonderYieldChanges", m_syncArchive)
#endif
#if defined(STANDARDIZE_YIELDS)
	, m_aiYieldEverGeneratedTimes100("CvPlayer::m_aiYieldEverGeneratedTimes100", m_syncArchive)
	, m_aiYieldPerTurnFromMisc("CvPlayer::m_aiYieldPerTurnFromMisc", m_syncArchive)
#endif
	, m_aiCityYieldChange("CvPlayer::m_aiCityYieldChange", m_syncArchive)
	, m_aiCoastalCityYieldChange("CvPlayer::m_aiCoastalCityYieldChange", m_syncArchive)
	, m_aiCapitalYieldChange("CvPlayer::m_aiCapitalYieldChange", m_syncArchive)
	, m_aiCapitalYieldPerPopChange("CvPlayer::m_aiCapitalYieldPerPopChange", m_syncArchive)
	, m_aiSeaPlotYield("CvPlayer::m_aiSeaPlotYield", m_syncArchive)
	, m_aiYieldRateModifier("CvPlayer::m_aiYieldRateModifier", m_syncArchive)
	, m_aiCapitalYieldRateModifier("CvPlayer::m_aiCapitalYieldRateModifier", m_syncArchive)
	, m_aiExtraYieldThreshold("CvPlayer::m_aiExtraYieldThreshold", m_syncArchive)
	, m_aiSpecialistExtraYield("CvPlayer::m_aiSpecialistExtraYield", m_syncArchive)
	, m_aiProximityToPlayer("CvPlayer::m_aiProximityToPlayer", m_syncArchive, true)
	, m_aiResearchAgreementCounter("CvPlayer::m_aiResearchAgreementCounter", m_syncArchive)
	, m_aiIncomingUnitTypes("CvPlayer::m_aiIncomingUnitTypes", m_syncArchive, true)
	, m_aiIncomingUnitCountdowns("CvPlayer::m_aiIncomingUnitCountdowns", m_syncArchive, true)
	, m_aiMinorFriendshipAnchors("CvPlayer::m_aiMinorFriendshipAnchors", m_syncArchive, true)
	, m_aOptions("CvPlayer::m_aOptions", m_syncArchive, true)
	, m_strReligionKey("CvPlayer::m_strReligionKey", m_syncArchive)
	, m_strScriptData("CvPlayer::m_strScriptData", m_syncArchive)
	, m_paiNumResourceUsed("CvPlayer::m_paiNumResourceUsed", m_syncArchive)
	, m_paiNumResourceTotal("CvPlayer::m_paiNumResourceTotal", m_syncArchive)
#ifdef LEKMOD_CS_BUILDING_STRATEGIC_NO_ALLY_SHARE
	, m_paiMinorStrategicResourceFromBuildings("CvPlayer::m_paiMinorStrategicResourceFromBuildings", m_syncArchive)
#endif
	, m_paiResourceGiftedToMinors("CvPlayer::m_paiResourceGiftedToMinors", m_syncArchive)
	, m_paiResourceExport("CvPlayer::m_paiResourceExport", m_syncArchive)
	, m_paiResourceImport("CvPlayer::m_paiResourceImport", m_syncArchive)
	, m_paiResourceFromMinors("CvPlayer::m_paiResourceFromMinors", m_syncArchive)
	, m_paiResourcesSiphoned("CvPlayer::m_paiResourcesSiphoned", m_syncArchive)
	, m_paiImprovementCount("CvPlayer::m_paiImprovementCount", m_syncArchive)
	, m_paiFreeBuildingCount("CvPlayer::m_paiFreeBuildingCount", m_syncArchive)
	, m_paiFreePromotionCount("CvPlayer::m_paiFreePromotionCount", m_syncArchive)
#ifdef LEKMOD_UNITCOMBAT_FREE_PROMOTION
	, m_paiUnitCombatFreePromotionCount("CvPlayer::m_paiFreeUnitCombatPromotionCount", m_syncArchive)
#endif
	, m_paiUnitCombatProductionModifiers("CvPlayer::m_paiUnitCombatProductionModifiers", m_syncArchive)
	, m_paiUnitCombatFreeExperiences("CvPlayer::m_paiUnitCombatFreeExperiences", m_syncArchive)
	, m_paiUnitClassCount("CvPlayer::m_paiUnitClassCount", m_syncArchive, true)
	, m_paiUnitClassMaking("CvPlayer::m_paiUnitClassMaking", m_syncArchive, true)
	, m_paiBuildingClassCount("CvPlayer::m_paiBuildingClassCount", m_syncArchive)
	, m_paiBuildingClassMaking("CvPlayer::m_paiBuildingClassMaking", m_syncArchive, true)
	, m_paiProjectMaking("CvPlayer::m_paiProjectMaking", m_syncArchive)
	, m_paiHurryCount("CvPlayer::m_paiHurryCount", m_syncArchive)
	, m_paiHurryModifier("CvPlayer::m_paiHurryModifier", m_syncArchive)
	, m_pabLoyalMember("CvPlayer::m_pabLoyalMember", m_syncArchive)
	, m_pabGetsScienceFromPlayer("CvPlayer::m_pabGetsScienceFromPlayer", m_syncArchive)
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	, m_ppaaiGreatWorkClassYieldChange("CvPlayer::m_ppaaiGreatWorkClassYieldChange", m_syncArchive)
#endif
	, m_ppaaiSpecialistExtraYield("CvPlayer::m_ppaaiSpecialistExtraYield", m_syncArchive)
	, m_ppaaiImprovementYieldChange("CvPlayer::m_ppaaiImprovementYieldChange", m_syncArchive)
	, m_ppaaiBuildingClassYieldMod("CvPlayer::m_ppaaiBuildingClassYieldMod", m_syncArchive)
	, m_UnitCycle(this)
	, m_bEverPoppedGoody("CvPlayer::m_bEverPoppedGoody", m_syncArchive)
	, m_bEverTrainedBuilder("CvPlayer::m_bEverTrainedBuilder", m_syncArchive)
	, m_iCityConnectionHappiness("CvPlayer::m_iCityConnectionHappiness", m_syncArchive)
	, m_iHolyCityID("CvPlayer::m_iHolyCityID", m_syncArchive)
	, m_iTurnsSinceSettledLastCity("CvPlayer::m_iTurnsSinceSettledLastCity", m_syncArchive)
	, m_iNumNaturalWondersDiscoveredInArea("CvPlayer::m_iNumNaturalWondersDiscoveredInArea", m_syncArchive)
	, m_iStrategicResourceMod("CvPlayer::m_iStrategicResourceMod", m_syncArchive)
	, m_iSpecialistCultureChange("CvPlayer::m_iSpecialistCultureChange", m_syncArchive)
	, m_iGreatPeopleSpawnCounter("CvPlayer::m_iGreatPeopleSpawnCounter", m_syncArchive)
	, m_iFreeTechCount("CvPlayer::m_iFreeTechCount", m_syncArchive, true)
	, m_iMedianTechPercentage(50)
	, m_iNumFreePolicies("CvPlayer::m_iNumFreePolicies", m_syncArchive)
	, m_iNumFreePoliciesEver("CvPlayer::m_iNumFreePoliciesEver", m_syncArchive)
	, m_iNumFreeTenets(0)
	, m_iMaxEffectiveCities(1)
	, m_iLastSliceMoved(0)
	, m_eEndTurnBlockingType(NO_ENDTURN_BLOCKING_TYPE)
	, m_iEndTurnBlockingNotificationIndex(0)
	, m_activeWaitingForEndTurnMessage(false)
	, m_endTurnBusyUnitUpdatesLeft(0)
	, m_lastGameTurnInitialAIProcessed(-1)
	, m_iNumFreeGreatPeople(0)
	, m_iNumMayaBoosts(0)
	, m_iNumFaithGreatPeople(0)
	, m_iNumArchaeologyChoices(0)
	, m_eFaithPurchaseType(NO_AUTOMATIC_FAITH_PURCHASE)
	, m_iFaithPurchaseIndex(0)
	, m_bProcessedAutoMoves(false)
	, m_kPlayerAchievements(*this)
#ifdef CS_ALLYING_WAR_RESCTRICTION
	, m_ppaaiTurnCSWarAllowing("CvPlayer::m_ppaaiTurnCSWarAllowing", m_syncArchive)
	, m_ppaafTimeCSWarAllowing("CvPlayer::m_ppaafTimeCSWarAllowing", m_syncArchive)
	, m_paiPriorityTurn("CvPlayer::m_paiPriorityTurn", m_syncArchive)
	, m_piPriorityTime("CvPlayer::m_piPriorityTime", m_syncArchive)
#endif
#ifdef PENALTY_FOR_DELAYING_POLICIES
	, m_bIsDelayedPolicy(false)
#endif


	, m_paiNumCitiesFreeChosenBuilding("CvPlayer::m_paiNumCitiesFreeChosenBuilding", m_syncArchive)
	, m_pabFreeChosenBuildingNewCity("CvPlayer::m_pabFreeChosenBuildingNewCity", m_syncArchive)
	, m_pabAllCityFreeBuilding("CvPlayer::m_pabAllCityFreeBuilding", m_syncArchive)
	, m_pabNewFoundCityFreeUnit("CvPlayer::m_pabNewFoundCityFreeUnit", m_syncArchive)
	, m_pabNewFoundCityFreeBuilding("CvPlayer::m_pabNewFoundCityFreeBuilding", m_syncArchive)

{
	m_pPlayerPolicies = FNEW(CvPlayerPolicies, c_eCiv5GameplayDLL, 0);
	m_pEconomicAI = FNEW(CvEconomicAI, c_eCiv5GameplayDLL, 0);
	m_pMilitaryAI = FNEW(CvMilitaryAI, c_eCiv5GameplayDLL, 0);
	m_pCitySpecializationAI = FNEW(CvCitySpecializationAI, c_eCiv5GameplayDLL, 0);
	m_pWonderProductionAI = FNEW(CvWonderProductionAI(this, GC.GetGameBuildings()), c_eCiv5GameplayDLL, 0);
	m_pGrandStrategyAI = FNEW(CvGrandStrategyAI, c_eCiv5GameplayDLL, 0);
	m_pDiplomacyAI = FNEW(CvDiplomacyAI, c_eCiv5GameplayDLL, 0);
	m_pReligions = FNEW(CvPlayerReligions, c_eCiv5GameplayDLL, 0);
	m_pReligionAI = FNEW(CvReligionAI, c_eCiv5GameplayDLL, 0);
	m_pPlayerTechs = FNEW(CvPlayerTechs, c_eCiv5GameplayDLL, 0);
	m_pFlavorManager = FNEW(CvFlavorManager, c_eCiv5GameplayDLL, 0);
	m_pTacticalAI = FNEW(CvTacticalAI, c_eCiv5GameplayDLL, 0);
	m_pHomelandAI = FNEW(CvHomelandAI, c_eCiv5GameplayDLL, 0);
	m_pMinorCivAI = FNEW(CvMinorCivAI, c_eCiv5GameplayDLL, 0);
	m_pDealAI = FNEW(CvDealAI, c_eCiv5GameplayDLL, 0);
	m_pBuilderTaskingAI = FNEW(CvBuilderTaskingAI, c_eCiv5GameplayDLL, 0);
	m_pDangerPlots = FNEW(CvDangerPlots, c_eCiv5GameplayDLL, 0);
	m_pCityConnections = FNEW(CvCityConnections, c_eCiv5GameplayDLL, 0);
	m_pTreasury = FNEW(CvTreasury, c_eCiv5GameplayDLL, 0);
	m_pTraits = FNEW(CvPlayerTraits, c_eCiv5GameplayDLL, 0);
	m_pEspionage = FNEW(CvPlayerEspionage, c_eCiv5GameplayDLL, 0);
	m_pEspionageAI = FNEW(CvEspionageAI, c_eCiv5GameplayDLL, 0);
	m_pTrade = FNEW(CvPlayerTrade, c_eCiv5GameplayDLL, 0);
	m_pTradeAI = FNEW(CvTradeAI, c_eCiv5GameplayDLL, 0);
	m_pLeagueAI = FNEW(CvLeagueAI, c_eCiv5GameplayDLL, 0);
	m_pCulture = FNEW(CvPlayerCulture, c_eCiv5GameplayDLL, 0);

	m_pNotifications = NULL;
	m_pDiplomacyRequests = NULL;

	m_iNextOperationID = 0;

	m_aiPlots.clear();
	m_bfEverConqueredBy.ClearAll();

	m_aiGreatWorkYieldChange.clear();
	m_aiSiphonLuxuryCount.clear();

	reset(NO_PLAYER, true);
}



CvPlayer::~CvPlayer()
{
	uninit();

	SAFE_DELETE(m_pDangerPlots);
	delete m_pPlayerPolicies;
	delete m_pEconomicAI;
	delete m_pMilitaryAI;
	delete m_pCitySpecializationAI;
	delete m_pWonderProductionAI;
	delete m_pGrandStrategyAI;
	delete m_pDiplomacyAI;
	delete m_pReligions;
	delete m_pReligionAI;
	delete m_pPlayerTechs;
	delete m_pFlavorManager;
	delete m_pTacticalAI;
	delete m_pHomelandAI;
	delete m_pMinorCivAI;
	delete m_pDealAI;
	delete m_pBuilderTaskingAI;
	SAFE_DELETE(m_pCityConnections);
	SAFE_DELETE(m_pNotifications);
	SAFE_DELETE(m_pDiplomacyRequests);
	SAFE_DELETE(m_pTreasury);
	SAFE_DELETE(m_pTraits);
	SAFE_DELETE(m_pEspionage);
	SAFE_DELETE(m_pEspionageAI);
	SAFE_DELETE(m_pTrade);
	SAFE_DELETE(m_pTradeAI);
	SAFE_DELETE(m_pLeagueAI);
}



void CvPlayer::init(PlayerTypes eID)
{
	LeaderHeadTypes eBestPersonality;
	int iValue;
	int iBestValue;
#ifdef AUI_WARNING_FIXES
	uint iI;
	int iJ;
#else
	int iI, iJ;
#endif


	if(eID < MAX_MAJOR_CIVS)
	{
		m_pNotifications = FNEW(CvNotifications, c_eCiv5GameplayDLL, 0);
		m_pDiplomacyRequests = FNEW(CvDiplomacyRequests, c_eCiv5GameplayDLL, 0);
	}



	reset(eID);



	m_cities.Init();

	m_units.Init();

	m_armyAIs.Init();

	m_AIOperations.clear();



	setupGraphical();



	CvAssert(getTeam() != NO_TEAM);
	GET_TEAM(getTeam()).changeNumMembers(1);
	PlayerTypes p = GetID();
	SlotStatus s = CvPreGame::slotStatus(p);
	if((s == SS_TAKEN) || (s == SS_COMPUTER))
	{
		setAlive(true);

		if(GC.getGame().isOption(GAMEOPTION_RANDOM_PERSONALITIES))
		{
			if(!isBarbarian() && !isMinorCiv())
			{
				iBestValue = 0;
				eBestPersonality = NO_LEADER;

				for(iI = 0; iI < GC.getNumLeaderHeadInfos(); iI++)
				{
#ifdef AUI_WARNING_FIXES
					if (iI != (uint)GC.getBARBARIAN_LEADER() && iI != (uint)GC.getMINOR_CIVILIZATION())
#else
					if(iI != GC.getBARBARIAN_LEADER() && iI != GC.getMINOR_CIVILIZATION())
#endif
					{
						iValue = (1 + GC.getGame().getJonRandNum(10000, "Choosing Personality"));

						for(iJ = 0; iJ < MAX_CIV_PLAYERS; iJ++)
						{
							if(GET_PLAYER((PlayerTypes)iJ).isAlive())
							{
								if(GET_PLAYER((PlayerTypes)iJ).getPersonalityType() == ((LeaderHeadTypes)iI))
								{
									iValue /= 2;
								}
							}
						}

						if(iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestPersonality = ((LeaderHeadTypes)iI);
						}
					}
				}

				if(eBestPersonality != NO_LEADER)
				{
					setPersonalityType(eBestPersonality);
				}
			}
		}

		CvAssert(m_pTraits);
		m_pTraits->InitPlayerTraits();


		if(m_pTraits->IsEmbarkedAllWater())
		{
			SetEmbarkedGraphicOverride("ART_DEF_UNIT_U_POLYNESIAN_WAR_CANOE");
		}
		else if(m_pTraits->IsEmbarkedToLandFlatCost())
		{
			SetEmbarkedGraphicOverride("ART_DEF_UNIT_U_DANISH_LONGBOAT");
		}

		changeGoldPerUnitTimes100(GC.getINITIAL_GOLD_PER_UNIT_TIMES_100());

		ChangeMaxNumBuilders(GC.getDEFAULT_MAX_NUM_BUILDERS());

		changeLevelExperienceModifier(GetPlayerTraits()->GetLevelExperienceModifier());
		changeMaxGlobalBuildingProductionModifier(GetPlayerTraits()->GetMaxGlobalBuildingProductionModifier());
		changeMaxTeamBuildingProductionModifier(GetPlayerTraits()->GetMaxTeamBuildingProductionModifier());
		changeMaxPlayerBuildingProductionModifier(GetPlayerTraits()->GetMaxPlayerBuildingProductionModifier());
		ChangePlotGoldCostMod(GetPlayerTraits()->GetPlotBuyCostModifier());
		ChangePlotCultureCostModifier(GetPlayerTraits()->GetPlotCultureCostModifier());
		GetTreasury()->ChangeCityConnectionTradeRouteGoldChange(GetPlayerTraits()->GetCityConnectionTradeRouteChange());
		changeWonderProductionModifier(GetPlayerTraits()->GetWonderProductionModifier());
		ChangeRouteGoldMaintenanceMod(GetPlayerTraits()->GetImprovementMaintenanceModifier());
#if defined(TRAITIFY)
		ChangeTraitExtraLeagueVotes(GetPlayerTraits()->GetNumExtraLeagueVotes());
		changeHalfMoreSpecialistUnhappinessCount(GetPlayerTraits()->IsHalfMoreSpecialistUnhappiness());
		ChangeMinorFriendshipAnchorMod(GetPlayerTraits()->GetMinorFriendshipMinimum());
		ChangeExtraHappinessPerCity(GetPlayerTraits()->GetGlobalHappinessPerCity());
		for (int iR = 0; iR < GC.getNumRouteInfos(); iR++)
		{
			GET_TEAM(getTeam()).changeRouteChange(((RouteTypes)iR), GetPlayerTraits()->GetRouteMovementChange((RouteTypes)iR));
		}
#endif
#if defined(LEKMOD_FIX_PATRO_FOOD)
		ChangeCityStateBonusModifier(GetPlayerTraits()->GetCityStateBonusModifier());
#endif
#if defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
		ChangeGreatGeneralCombatBonus(GetPlayerTraits()->GetGreatGeneralExtraBonus());
#endif
		for(iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			YieldTypes eYield = static_cast<YieldTypes>(iJ);
#if !defined(LEKMOD_CITY_YIELDS_TRAITS)
			ChangeCityYieldChange(eYield, 100 * GetPlayerTraits()->GetFreeCityYield(eYield));
#endif
			changeYieldRateModifier(eYield, GetPlayerTraits()->GetYieldRateModifier(eYield));
#if defined(LEKMOD_EXPERIMENTAL_CHANGES)
			ChangeWorldWonderYieldChanges(eYield, GetPlayerTraits()->GetWorldWonderYieldChange(eYield));
#endif
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
			ChangeGreatWorkYieldChange(eYield, GetPlayerTraits()->GetGreatWorkYieldChange(eYield));
			for (iI = 0; iI < GC.getNumGreatWorkClassInfos(); ++iI)
			{
				ChangeGreatWorkClassYieldChange((GreatWorkClass)iI, eYield, GetPlayerTraits()->GetGreatWorkClassYieldChange((GreatWorkClass)iI, eYield));
			}
#endif
		}

		GetPlayerTraits()->GetUsedGroupAreas().clear();
		GetPlayerTraits()->GetGroupPriority().clear();

		recomputeGreatPeopleModifiers();
		for(iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			updateExtraYieldThreshold((YieldTypes)iJ);
		}
#if !defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
		ChangeGreatWorkYieldChange(YIELD_CULTURE, GC.getBASE_CULTURE_PER_GREAT_WORK());
#endif
		CvCivilizationInfo& playerCivilizationInfo = getCivilizationInfo();
		for(iI = 0; iI < GC.getNumUnitClassInfos(); ++iI)
		{
			const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(iI);
			CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
			if(pkUnitClassInfo != NULL)
			{
				const UnitTypes eUnit = ((UnitTypes)(playerCivilizationInfo.getCivilizationUnits(iI)));
				if(NO_UNIT != eUnit)
				{
					CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
					if(NULL != pkUnitInfo && pkUnitInfo->IsFound())
					{
						setUnitExtraCost(eUnitClass, getNewCityProductionValue());
					}
				}
			}
		}

		BuildingTypes eFreeBuilding = GetPlayerTraits()->GetFreeBuilding();
		if(eFreeBuilding != NO_BUILDING)
		{
			changeFreeBuildingCount(eFreeBuilding, 1);
		}
#if !defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
		SetGreatGeneralCombatBonus(GC.getGREAT_GENERAL_STRENGTH_MOD());
#else
		ChangeGreatGeneralCombatBonus(GC.getGREAT_GENERAL_STRENGTH_MOD());
#endif
	}

	m_aiPlots.clear();
	m_bfEverConqueredBy.ClearAll();

	AI_init();
}



void CvPlayer::uninit()
{
	m_paiNumResourceUsed.clear();
	m_paiNumResourceTotal.clear();
#ifdef LEKMOD_CS_BUILDING_STRATEGIC_NO_ALLY_SHARE
	m_paiMinorStrategicResourceFromBuildings.clear();
#endif
	m_paiResourceGiftedToMinors.clear();
	m_paiResourceExport.clear();
	m_paiResourceImport.clear();
	m_paiResourceFromMinors.clear();
	m_paiResourcesSiphoned.clear();
	m_paiImprovementCount.clear();
	m_paiFreeBuildingCount.clear();
	m_paiFreePromotionCount.clear();
	m_paiUnitCombatProductionModifiers.clear();
#ifdef LEKMOD_UNITCOMBAT_FREE_PROMOTION
	m_paiUnitCombatFreePromotionCount.clear();
#endif
	m_paiUnitCombatFreeExperiences.clear();
	m_paiUnitClassCount.clear();
	m_paiUnitClassMaking.clear();
	m_paiBuildingClassCount.clear();
	m_paiBuildingClassMaking.clear();
	m_paiProjectMaking.clear();
	m_paiHurryCount.clear();
	m_paiHurryModifier.clear();



	m_paiNumCitiesFreeChosenBuilding.clear();
	m_pabFreeChosenBuildingNewCity.clear();
	m_pabAllCityFreeBuilding.clear();
	m_pabNewFoundCityFreeUnit.clear();
	m_pabNewFoundCityFreeBuilding.clear();



	m_pabLoyalMember.clear();
	m_pabGetsScienceFromPlayer.clear();

#ifdef CS_ALLYING_WAR_RESCTRICTION
	m_ppaaiTurnCSWarAllowing.clear();
	m_ppaafTimeCSWarAllowing.clear();
	m_paiPriorityTurn.clear();
	m_piPriorityTime.clear();
#endif

	m_pPlayerPolicies->Uninit();
	m_pEconomicAI->Uninit();
	m_pMilitaryAI->Uninit();
	m_pCitySpecializationAI->Uninit();
	m_pWonderProductionAI->Uninit();
	m_pGrandStrategyAI->Uninit();
	m_pDiplomacyAI->Uninit();
	m_pReligions->Uninit();
	m_pReligionAI->Uninit();
	m_pEspionage->Uninit();
	m_pEspionageAI->Uninit();
	m_pTrade->Uninit();
	m_pTradeAI->Uninit();
	m_pLeagueAI->Uninit();
	m_pPlayerTechs->Uninit();
	m_pFlavorManager->Uninit();
	m_pTacticalAI->Uninit();
	m_pHomelandAI->Uninit();
	m_pMinorCivAI->Uninit();
	m_pDealAI->Uninit();
	m_pBuilderTaskingAI->Uninit();
	m_pCityConnections->Uninit();
	if(m_pNotifications)
	{
		m_pNotifications->Uninit();
	}
	if(m_pDiplomacyRequests)
	{
		m_pDiplomacyRequests->Uninit();
	}
	m_pTreasury->Uninit();
	m_pTraits->Uninit();

	if(m_pDangerPlots)
	{
		m_pDangerPlots->Uninit();
	}
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	m_ppaaiGreatWorkClassYieldChange.clear();
#endif
	m_ppaaiSpecialistExtraYield.clear();
	m_ppiImprovementYieldChange.clear();
	m_ppaaiImprovementYieldChange.clear();

	m_ppiResourceYieldChange.clear();
	m_ppaaiBuildingClassYieldMod.clear();

	m_UnitCycle.Clear();

	m_researchQueue.clear();

	m_cityNames.clear();

	m_cities.Uninit();

	m_units.Uninit();


	std::map<int , CvAIOperation*>::iterator iter;
	for(iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		delete(iter->second);
	}
	m_AIOperations.clear();

	m_aiPlots.clear();
	m_bfEverConqueredBy.ClearAll();

	FAutoArchive& archive = getSyncArchive();
	archive.clearDelta();

#ifdef GAME_AUTOPAUSE_ON_ACTIVE_DISCONNECT_IF_NOT_SEQUENTIAL
	m_bIsDisconnected = false;
#endif
	m_iStartingX = INVALID_PLOT_COORD;
	m_iStartingY = INVALID_PLOT_COORD;
	m_iTotalPopulation = 0;
	m_iTotalLand = 0;
	m_iTotalLandScored = 0;
	m_iCityConnectionHappiness = 0;
	m_iJONSCulturePerTurnForFree = 0;
	m_iJONSCulturePerTurnFromMinorCivs = 0;
	m_iJONSCultureCityModifier = 0;
#ifdef AUI_PLAYER_FIX_JONS_CULTURE_IS_T100
	m_iJONSCultureT100 = 0;
	m_iJONSCultureEverGeneratedT100 = 0;
#else
	m_iJONSCulture = 0;
	m_iJONSCultureEverGenerated = 0;
#endif
#if !defined(LEKMOD_EXPERIMENTAL_CHANGES)
	m_iCulturePerWonder = 0;
#endif
	m_iCultureWonderMultiplier = 0;
	m_iCulturePerTechResearched = 0;
	m_iFaith = 0;
	m_iFaithEverGenerated = 0;
	m_iHappiness = 0;
	m_iUprisingCounter = 0;
	m_iExtraHappinessPerLuxury = 0;
	m_iUnhappinessFromUnits = 0;
	m_iUnhappinessFromUnitsMod = 0;
	m_iUnhappinessMod = 0;
	m_iCityCountUnhappinessMod = 0;
	m_iOccupiedPopulationUnhappinessMod = 0;
	m_iCapitalUnhappinessMod = 0;
	m_iCityRevoltCounter = 0;
	m_iHappinessPerGarrisonedUnitCount = 0;
	m_iHappinessPerTradeRouteCount = 0;
#ifdef NQ_RAIL_CONNECTION_HAPPINESS_FROM_POLICIES
	m_iHappinessPerRailConnectionCount = 0;
#endif
	m_iHappinessPerXPopulation = 0;
	m_iHappinessFromLeagues = 0;
	m_iEspionageModifier = 0;
	m_iSpyStartingRank = 0;
#ifdef ENHANCED_GRAPHS
	m_iNumStolenScience = 0;
	m_iNumTrainedUnits = 0;
	m_iNumKilledUnits = 0;
	m_iNumLostUnits = 0;
	m_iUnitsDamageDealt = 0;
	m_iUnitsDamageTaken = 0;
	m_iCitiesDamageDealt = 0;
	m_iCitiesDamageTaken = 0;
	m_iNumScientistsTotal = 0;
	m_iNumEngineersTotal = 0;
	m_iNumMerchantsTotal = 0;
	m_iNumWritersTotal = 0;
	m_iNumArtistsTotal = 0;
	m_iNumMusiciansTotal = 0;
	m_iNumGeneralsTotal = 0;
	m_iNumAdmiralsTotal = 0;
	m_iNumProphetsTotal = 0;
	m_iProductionGoldFromWonders = 0;
	m_iNumChops = 0;
	m_iNumTimesOpenedDemographics = 0;
	m_iUnitsDamageHealed = 0;
	m_iTurnsStagnated = 0;
	m_iGoldSpentBuys = 0;
	m_iGoldSpentUpgrades = 0;
	m_bMayaBoostScientist = 0;
	m_bMayaBoostEngineers = 0;
	m_bMayaBoostMerchants = 0;
	m_bMayaBoostWriters = 0;
	m_bMayaBoostArtists = 0;
	m_bMayaBoostMusicians = 0;
#endif
	m_iExtraLeagueVotes = 0;
#if defined(TRAITIFY)
	m_iTraitExtraLeagueVotes = 0;
#endif
#if defined(MISC_CHANGES)
	m_iTechExtraLeagueVotes = 0;
	m_iPolicyExtraLeagueVotes = 0;
	m_iMiscTradeRoutes = 0;
#endif

	m_iSpecialPolicyBuildingHappiness = 0;
	m_iWoundedUnitDamageMod = 0;
	m_iUnitUpgradeCostMod = 0;
	m_iBarbarianCombatBonus = 0;
	m_iAlwaysSeeBarbCampsCount = 0;
	m_iHappinessFromBuildings = 0;
	m_iHappinessPerCity = 0;
	m_iHappinessPerXPolicies = 0;
	m_iAdvancedStartPoints = -1;
	m_iAttackBonusTurns = 0;
	m_iCultureBonusTurns = 0;
	m_iTourismBonusTurns = 0;
	m_iGoldenAgeProgressMeter = 0;
	m_iGoldenAgeMeterMod = 0;
	m_iNumGoldenAges = 0;
	m_iGoldenAgeTurns = 0;
	m_iNumUnitGoldenAges = 0;
	m_iStrikeTurns = 0;
	m_iGoldenAgeModifier = 0;
	m_iGreatPeopleCreated = 0;
	m_iGreatGeneralsCreated = 0;
	m_iGreatAdmiralsCreated = 0;
	m_iGreatWritersCreated = 0;
	m_iGreatArtistsCreated = 0;
	m_iGreatMusiciansCreated = 0;
#ifdef NQ_FREE_SETTLERS_FROM_BELIEF
	m_bHasUsedReligiousSettlements = false;
#endif
#ifdef NQ_DEUS_VULT
	m_bHasUsedDeusVult = false;
#endif
#ifdef NQ_GOLDEN_AGE_TURNS_FROM_BELIEF
	m_bHasUsedDharma = false;
#endif
	m_iGreatScientistsCreated = 0;
	m_iGreatEngineersCreated = 0;
	m_iGreatMerchantsCreated = 0;
	m_iGreatProphetsCreated = 0;
	m_iMerchantsFromFaith = 0;
	m_iScientistsFromFaith = 0;
	m_iWritersFromFaith = 0;
	m_iArtistsFromFaith = 0;
	m_iMusiciansFromFaith = 0;
	m_iGeneralsFromFaith = 0;
	m_iAdmiralsFromFaith = 0;
	m_iEngineersFromFaith = 0;
	m_iGreatPeopleThresholdModifier = 0;
	m_iGreatGeneralsThresholdModifier = 0;
	m_iGreatAdmiralsThresholdModifier = 0;
	m_iGreatGeneralCombatBonus = 0;
	m_iAnarchyNumTurns = 0;
	m_iPolicyCostModifier = 0;
	m_iGreatPeopleRateModifier = 0;
	m_iGreatPeopleRateModFromBldgs = 0;
	m_iGreatGeneralRateModifier = 0;
	m_iGreatGeneralRateModFromBldgs = 0;
	m_iDomesticGreatGeneralRateModifier = 0;
	m_iDomesticGreatGeneralRateModFromBldgs = 0;
	m_iGreatAdmiralRateModifier = 0;
	m_iGreatWriterRateModifier = 0;
	m_iGreatArtistRateModifier = 0;
	m_iGreatMusicianRateModifier = 0;
	m_iGreatMerchantRateModifier = 0;
	m_iGreatScientistRateModifier = 0;
	m_iGreatScientistBeakerModifier = 0;
	m_iGreatEngineerRateModifier = 0;
	m_iGreatPersonExpendGold = 0;
	m_iMaxGlobalBuildingProductionModifier = 0;
	m_iMaxTeamBuildingProductionModifier = 0;
	m_iMaxPlayerBuildingProductionModifier = 0;
	m_iFreeExperience = 0;
	m_iFreeExperienceFromBldgs = 0;
	m_iFreeExperienceFromMinors = 0;
	m_iFeatureProductionModifier = 0;
	m_iWorkerSpeedModifier = 0;
	m_iImprovementCostModifier = 0;
	m_iImprovementUpgradeRateModifier = 0;
	m_iSpecialistProductionModifier = 0;
	m_iMilitaryProductionModifier = 0;
	m_iSpaceProductionModifier = 0;
	m_iCityDefenseModifier = 0;
	m_iUnitFortificationModifier = 0;
	m_iUnitBaseHealModifier = 0;
	m_iWonderProductionModifier = 0;
	m_iSettlerProductionModifier = 0;
	m_iCapitalSettlerProductionModifier = 0;
	m_iUnitProductionMaintenanceMod = 0;
	m_iPolicyCostBuildingModifier = 0;
	m_iPolicyCostMinorCivModifier = 0;
#if !defined(LEK_YIELD_TOURISM)
	m_iInfluenceSpreadModifier = 0;
#endif
	m_iExtraVotesPerDiplomat = 0;
	m_iNumNukeUnits = 0;
	m_iNumOutsideUnits = 0;
	m_iBaseFreeUnits = 0;
	m_iBaseFreeMilitaryUnits = 0;
	m_iFreeUnitsPopulationPercent = 0;
	m_iFreeMilitaryUnitsPopulationPercent = 0;
	m_iGoldPerUnit = 0;
	m_iGoldPerMilitaryUnit = 0;
	m_iRouteGoldMaintenanceMod = 0;
	m_iBuildingGoldMaintenanceMod = 0;
	m_iUnitGoldMaintenanceMod = 0;
	m_iUnitSupplyMod = 0;
	m_iExtraUnitCost = 0;
	m_iNumMilitaryUnits = 0;
	m_iHappyPerMilitaryUnit = 0;
	m_iHappinessToCulture = 0;
	m_iHappinessToScience = 0;
#ifdef NQ_GOLD_TO_SCIENCE_FROM_POLICIES
	m_iGoldToScience = 0;
#endif
	m_iHalfSpecialistUnhappinessCount = 0;
	m_iHalfMoreSpecialistUnhappinessCount = 0;
	m_iHalfSpecialistFoodCount = 0;
	m_iMilitaryFoodProductionCount = 0;
	m_iGoldenAgeCultureBonusDisabledCount = 0;
	m_iSecondReligionPantheonCount = 0;
	m_iEnablesSSPartHurryCount = 0;
#ifdef NQ_DIABLE_RESISTANCE_TIME_VIA_POLICIES
	m_iDisablesResistanceTimeCount = 0;
#endif
#ifdef NQ_PATRIOTIC_WAR
	m_iDoubleTrainedMilitaryLandUnitCount = 0;
#endif
#ifdef NQ_WAR_HERO
	m_iWarHeroCount = 0;
#endif
#ifdef NQ_IGNORE_PUPPETS_FOR_RESEARCH_COSTS_FROM_POLICIES
	m_iIgnorePuppetsForResearchCostsCount = 0;
#endif
#ifdef NQ_POLICY_TOGGLE_NO_MINOR_DOW_IF_FRIENDS
	m_iNoMinorDOWIfFriendsCount = 0;
#endif
#ifdef NQ_ALLOW_PUPPET_PURCHASING_FROM_POLICIES
	m_iAllowPuppetPurchasingCount = 0;
#endif
#if defined(LEKMOD_GREAT_FIREWALL_PLAYER_EFFECT)
	m_iInfluenceNullificationCount = 0;
#endif
	m_iEnablesSSPartPurchaseCount = 0;
	m_iConscriptCount = 0;
	m_iMaxConscript = 0;
	m_iHighestUnitLevel = 1;
	m_iOverflowResearch = 0;
	m_iExpModifier = 0;
	m_iExpInBorderModifier = 0;
	m_iLevelExperienceModifier = 0;
	m_iMinorQuestFriendshipMod = 0;
	m_iMinorGoldFriendshipMod = 0;
	m_iMinorFriendshipMinimum = 0;
	m_iMinorFriendshipDecayMod = 0;
	m_iMinorScienceAlliesCount = 0;
	m_iMinorResourceBonusCount = 0;
	m_iAbleToAnnexCityStatesCount = 0;
	m_iCultureBombTimer = 0;
	m_iConversionTimer = 0;
	m_iCapitalCityID = FFreeList::INVALID_INDEX;
	m_iCitiesLost = 0;
	m_iMilitaryMight = 0;
	m_iEconomicMight = 0;
	m_iTurnMightRecomputed = -1;
	m_iNewCityExtraPopulation = 0;
	m_iFreeFoodBox = 0;
	m_iScenarioScore1 = 0;
	m_iScenarioScore2 = 0;
	m_iScenarioScore3 = 0;
	m_iScenarioScore4 = 0;
	m_iScoreFromFutureTech = 0;
	m_iCombatExperience = 0;
	m_iLifetimeCombatExperience = 0;
	m_iNavalCombatExperience = 0;
	m_iBorderObstacleCount = 0;
	m_iPopRushHurryCount = 0;
	m_uiStartTime = 0;
	m_iTotalImprovementsBuilt = 0;
	m_iNextOperationID = 0;
	m_iCostNextPolicy = 0;
	m_iNumBuilders = 0;
	m_iMaxNumBuilders = 0;
	m_iCityStrengthMod = 0;
	m_iCityGrowthMod = 0;
	m_iCapitalGrowthMod = 0;
	m_iNumPlotsBought = 0;
	m_iPlotGoldCostMod = 0;
	m_iPlotCultureCostModifier = 0;
	m_iPlotCultureExponentModifier = 0;
	m_iNumCitiesPolicyCostDiscount = 0;
	m_iNumCitiesResearchCostDiscount = 0;
	m_iGarrisonedCityRangeStrikeModifier = 0;
	m_iGarrisonFreeMaintenanceCount = 0;
	m_iNumCitiesFreeAestheticsSchools = 0;
	m_iNumCitiesFreePietyGardens = 0;
	m_iNumCitiesFreeWalls = 0;
	m_iNumCitiesFreeCultureBuilding = 0;
	m_iNumCitiesFreeFoodBuilding = 0;
#if defined(LEKMOD_FIX_PATRO_FOOD)
	m_iCityStateBonusModifier = 0;
#endif
	m_iUnitPurchaseCostModifier = 0;
	m_iAllFeatureProduction = 0;
	m_iCityDistanceHighwaterMark = 1;
	m_iOriginalCapitalX = -1;
	m_iOriginalCapitalY = -1;
	m_iNumWonders = 0;
	m_iNumPolicies = 0;
	m_iNumGreatPeople = 0;
	m_iHolyCityID = -1;
	m_iTurnsSinceSettledLastCity = -1;
	m_iNumNaturalWondersDiscoveredInArea = 0;
	m_iStrategicResourceMod = 0;
	m_iSpecialistCultureChange = 0;
	m_iGreatPeopleSpawnCounter = 0;
	m_iFreeTechCount = 0;
	m_iMedianTechPercentage = 50;
	m_iNumFreePolicies = 0;
	m_iNumFreePoliciesEver = 0;
	m_iNumFreeTenets = 0;
	m_iNumFreeGreatPeople = 0;
	m_iNumMayaBoosts = 0;
	m_iNumFaithGreatPeople = 0;
	m_iNumArchaeologyChoices = 0;
	m_eFaithPurchaseType = NO_AUTOMATIC_FAITH_PURCHASE;
	m_iFaithPurchaseIndex = 0;
	m_iMaxEffectiveCities = 1;
	m_iLastSliceMoved = 0;
#ifdef PENALTY_FOR_DELAYING_POLICIES
	m_bIsDelayedPolicy = false;
#endif

#ifdef NQ_CHEAT_FIRST_ROYAL_LIBRARY_COMES_WITH_GREAT_WORK
	m_bHasEverBuiltRoyalLibrary = false;
#endif
	m_bHasBetrayedMinorCiv = false;
	m_bAlive = false;
	m_bEverAlive = false;
	m_bBeingResurrected = false;
	m_bTurnActive = false;
	m_bAutoMoves = false;
	m_bProcessedAutoMoves = false;
	m_bEndTurn = false;
#ifdef AUI_GAME_BETTER_HYBRID_MODE
	m_iTurnOrder = 0;
#else
	m_bDynamicTurnsSimultMode = true;
#endif
	m_bPbemNewTurn = false;
	m_bExtendedGame = false;
	m_bFoundedFirstCity = false;
	m_iNumCitiesFounded = 0;
	m_bStrike = false;
	m_bCramped = false;
	m_bLostCapital = false;
	m_eConqueror = NO_PLAYER;
	m_bHasAdoptedStateReligion = false;
	m_bAlliesGreatPersonBiasApplied = false;
	m_lastGameTurnInitialAIProcessed = -1;

#if defined(LEKMOD_PROMO_YIELD_FROM_CONVERSION) && defined(LEKMOD_PROMO_CONVERSION_MAJORITY_ONLY_ONCE)
	m_aiConversionMajorityOnceUsedKeys.clear();
#endif

	m_eID = NO_PLAYER;
}





void CvPlayer::reset(PlayerTypes eID, bool bConstructorCall)
{
	m_syncArchive.reset();


	uninit();

	m_eID = eID;
	if(m_eID != NO_PLAYER)
	{
		m_ePersonalityType = CvPreGame::leaderHead(m_eID);
	}
	else
	{
		m_ePersonalityType = NO_LEADER;
	}


	m_bEverPoppedGoody = false;
#if defined(LEKMOD_EXPERIMENTAL_CHANGES)
	m_aiWorldWonderYieldChanges.clear();
	m_aiWorldWonderYieldChanges.resize(NUM_YIELD_TYPES, 0);
#endif
#if defined(STANDARDIZE_YIELDS)
	m_aiYieldEverGeneratedTimes100.clear();
	m_aiYieldEverGeneratedTimes100.resize(NUM_YIELD_TYPES, 0);
	m_aiYieldPerTurnFromMisc.clear();
	m_aiYieldPerTurnFromMisc.resize(NUM_YIELD_TYPES, 0);
#endif
	m_aiCityYieldChange.clear();
	m_aiCityYieldChange.resize(NUM_YIELD_TYPES, 0);

	m_aiCoastalCityYieldChange.clear();
	m_aiCoastalCityYieldChange.resize(NUM_YIELD_TYPES, 0);

	m_aiCapitalYieldChange.clear();
	m_aiCapitalYieldChange.resize(NUM_YIELD_TYPES, 0);

	m_aiCapitalYieldPerPopChange.clear();
	m_aiCapitalYieldPerPopChange.resize(NUM_YIELD_TYPES, 0);

	m_aiSeaPlotYield.clear();
	m_aiSeaPlotYield.resize(NUM_YIELD_TYPES, 0);

	m_aiYieldRateModifier.clear();
	m_aiYieldRateModifier.resize(NUM_YIELD_TYPES, 0);

	m_aiCapitalYieldRateModifier.clear();
	m_aiCapitalYieldRateModifier.resize(NUM_YIELD_TYPES, 0);

	m_aiGreatWorkYieldChange.clear();
	m_aiGreatWorkYieldChange.resize(NUM_YIELD_TYPES, 0);

	m_aiExtraYieldThreshold.clear();
	m_aiExtraYieldThreshold.resize(NUM_YIELD_TYPES, 0);

	m_aiSpecialistExtraYield.clear();
	m_aiSpecialistExtraYield.resize(NUM_YIELD_TYPES, 0);

	m_aiProximityToPlayer.clear();
	m_aiProximityToPlayer.resize(MAX_PLAYERS, 0);

	m_aiResearchAgreementCounter.clear();
	m_aiResearchAgreementCounter.resize(MAX_PLAYERS, 0);

	m_aiIncomingUnitTypes.clear();
	m_aiIncomingUnitTypes.resize(MAX_PLAYERS, NO_UNIT);

	m_aiIncomingUnitCountdowns.clear();
	m_aiIncomingUnitCountdowns.resize(MAX_PLAYERS, -1);

	m_aiMinorFriendshipAnchors.clear();
	m_aiMinorFriendshipAnchors.resize(MAX_PLAYERS, 0);

	m_aiSiphonLuxuryCount.clear();
	m_aiSiphonLuxuryCount.resize(MAX_PLAYERS, 0);

#if defined(LEKMOD_PROMO_YIELD_FROM_CONVERSION) && defined(LEKMOD_PROMO_CONVERSION_MAJORITY_ONLY_ONCE)
	m_aiConversionMajorityOnceUsedKeys.clear();
#endif

#if defined(LEKMOD_CITY_YIELDS_TRAITS) && defined(LEKMOD_TRACK_CITY_SETTLER_UNITTYPE) && defined(LEKMOD_YIELD_SETTLE_UNIT_NON_CAP_MAX) && (LEKMOD_YIELD_SETTLE_UNIT_NON_CAP_MAX > 0)
	m_aiYieldSettleUnitCityOrder.clear();
#endif

	m_aOptions.clear();

	m_strReligionKey = "";
	m_strScriptData = "";
	m_strEmbarkedGraphicOverride = "";

	if(!bConstructorCall)
	{
		CvAssertMsg(0 < GC.getNumResourceInfos(), "GC.getNumResourceInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		m_paiNumResourceUsed.clear();
		m_paiNumResourceUsed.resize(GC.getNumResourceInfos(), 0);

		m_paiNumResourceTotal.clear();
		m_paiNumResourceTotal.resize(GC.getNumResourceInfos(), 0);

#ifdef LEKMOD_CS_BUILDING_STRATEGIC_NO_ALLY_SHARE
		m_paiMinorStrategicResourceFromBuildings.clear();
		m_paiMinorStrategicResourceFromBuildings.resize(GC.getNumResourceInfos(), 0);
#endif

		m_paiResourceGiftedToMinors.clear();
		m_paiResourceGiftedToMinors.resize(GC.getNumResourceInfos(), 0);

		m_paiResourceExport.clear();
		m_paiResourceExport.resize(GC.getNumResourceInfos(), 0);

		m_paiResourceImport.clear();
		m_paiResourceImport.resize(GC.getNumResourceInfos(), 0);

		m_paiResourceFromMinors.clear();
		m_paiResourceFromMinors.resize(GC.getNumResourceInfos(), 0);

		m_paiResourcesSiphoned.clear();
		m_paiResourcesSiphoned.resize(GC.getNumResourceInfos(), 0);

		CvAssertMsg(0 < GC.getNumImprovementInfos(), "GC.getNumImprovementInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		m_paiImprovementCount.clear();
		m_paiImprovementCount.resize(GC.getNumImprovementInfos(), 0);

		m_paiUnitCombatProductionModifiers.clear();
		m_paiUnitCombatProductionModifiers.resize(GC.getNumUnitCombatClassInfos(), 0);

		m_paiUnitCombatFreeExperiences.clear();
		m_paiUnitCombatFreeExperiences.resize(GC.getNumUnitCombatClassInfos(), 0);

		m_paiFreeBuildingCount.clear();
		m_paiFreeBuildingCount.resize(GC.getNumBuildingInfos(), 0);

		m_paiFreePromotionCount.clear();
		m_paiFreePromotionCount.resize(GC.getNumPromotionInfos(), 0);

#ifdef LEKMOD_UNITCOMBAT_FREE_PROMOTION
		m_paiUnitCombatFreePromotionCount.clear();
		m_paiUnitCombatFreePromotionCount.resize(GC.getNumPromotionInfos(), GC.getNumUnitCombatClassInfos());
		for(int i = 0; i < GC.getNumPromotionInfos(); i++)
		{
			for (int j = 0; j < GC.getNumUnitCombatClassInfos(); j++)
			{

				m_paiUnitCombatFreePromotionCount.setAt2D(i, j, 0);


			}
		}
#endif

		m_paiUnitClassCount.clear();
		m_paiUnitClassCount.resize(GC.getNumUnitClassInfos(), 0);

		m_paiUnitClassMaking.clear();
		m_paiUnitClassMaking.resize(GC.getNumUnitClassInfos(), 0);

		m_paiBuildingClassCount.clear();
		m_paiBuildingClassCount.resize(GC.getNumBuildingClassInfos(), 0);

		m_paiBuildingClassMaking.clear();
		m_paiBuildingClassMaking.resize(GC.getNumBuildingClassInfos(), 0);

		m_paiProjectMaking.clear();
		m_paiProjectMaking.resize(GC.getNumProjectInfos(), 0);

		m_paiHurryCount.clear();
		m_paiHurryCount.resize(GC.getNumHurryInfos(), 0);

		m_paiHurryModifier.clear();
		m_paiHurryModifier.resize(GC.getNumHurryInfos(), 0);




		m_paiNumCitiesFreeChosenBuilding.clear();
		m_paiNumCitiesFreeChosenBuilding.resize(GC.getNumBuildingClassInfos(), 0);

		m_pabFreeChosenBuildingNewCity.clear();
		m_pabFreeChosenBuildingNewCity.resize(GC.getNumBuildingClassInfos(), false);

		m_pabAllCityFreeBuilding.clear();
		m_pabAllCityFreeBuilding.resize(GC.getNumBuildingClassInfos(), false);

		m_pabNewFoundCityFreeUnit.clear();
		m_pabNewFoundCityFreeUnit.resize(GC.getNumUnitClassInfos(), false);

		m_pabNewFoundCityFreeBuilding.clear();
		m_pabNewFoundCityFreeBuilding.resize(GC.getNumBuildingClassInfos(), false);



		m_pabLoyalMember.clear();
		m_pabLoyalMember.resize(GC.getNumVoteSourceInfos(), true);

		m_pabGetsScienceFromPlayer.clear();
		m_pabGetsScienceFromPlayer.resize(MAX_CIV_PLAYERS, false);

#ifdef CS_ALLYING_WAR_RESCTRICTION
		Firaxis::Array< int, MAX_MINOR_CIVS > turn;
		for (unsigned int j = 0; j < MAX_MINOR_CIVS; ++j)
		{
			turn[j] = -1;
		}
		m_ppaaiTurnCSWarAllowing.clear();
		m_ppaaiTurnCSWarAllowing.resize(MAX_MAJOR_CIVS);
		for (unsigned int i = 0; i < m_ppaaiTurnCSWarAllowing.size(); ++i)
		{
			m_ppaaiTurnCSWarAllowing.setAt(i, turn);
		}

		Firaxis::Array< float, MAX_MINOR_CIVS > time;
		for (unsigned int j = 0; j < MAX_MINOR_CIVS; ++j)
		{
			time[j] = 0.f;
		}
		m_ppaafTimeCSWarAllowing.clear();
		m_ppaafTimeCSWarAllowing.resize(MAX_MAJOR_CIVS);
		for (unsigned int i = 0; i < m_ppaafTimeCSWarAllowing.size(); ++i)
		{
			m_ppaafTimeCSWarAllowing.setAt(i, time);
		}

		m_paiPriorityTurn.clear();
		m_paiPriorityTurn.resize(MAX_MAJOR_CIVS, -1);

		m_piPriorityTime.clear();
		m_piPriorityTime.resize(MAX_MAJOR_CIVS, 0.f);
#endif

		m_pEconomicAI->Init(GC.GetGameEconomicAIStrategies(), this);
		m_pMilitaryAI->Init(GC.GetGameMilitaryAIStrategies(), this, GetDiplomacyAI());
		m_pCitySpecializationAI->Init(GC.GetGameCitySpecializations(), this);
		m_pWonderProductionAI->Init(GC.GetGameBuildings(), this, false);
		m_pGrandStrategyAI->Init(GC.GetGameAIGrandStrategies(), this);
		m_pDiplomacyAI->Init(this);
		m_pReligions->Init(this);
		m_pReligionAI->Init(GC.GetGameBeliefs(), this);
		m_pPlayerTechs->Init(GC.GetGameTechs(), this, false);
		m_pPlayerPolicies->Init(GC.GetGamePolicies(), this, false);
		m_pTacticalAI->Init(this);
		m_pHomelandAI->Init(this);
		m_pMinorCivAI->Init(this);
		m_pDealAI->Init(this);
		m_pBuilderTaskingAI->Init(this);
		m_pCityConnections->Init(this);
		if(m_pNotifications)
		{
			m_pNotifications->Init(eID);
		}
		if(m_pDiplomacyRequests)
		{
			m_pDiplomacyRequests->Init(eID);
		}
		m_pDangerPlots->Init(eID, false              );
		m_pTreasury->Init(this);
		m_pTraits->Init(GC.GetGameTraits(), this);
		m_pEspionage->Init(this);
		m_pEspionageAI->Init(this);
		m_pTrade->Init(this);
		m_pTradeAI->Init(this);
		m_pLeagueAI->Init(this);
		m_pCulture->Init(this);


		m_pFlavorManager->Init(this);


		PlayerTypes p = GetID();
		if(p != NO_PLAYER)
		{
			SlotStatus s = CvPreGame::slotStatus(p);
			if((s == SS_TAKEN || s == SS_COMPUTER) && !isBarbarian())
			{
				m_pFlavorManager->AddFlavorRecipient(m_pPlayerTechs);
				m_pFlavorManager->AddFlavorRecipient(m_pPlayerPolicies);
				m_pFlavorManager->AddFlavorRecipient(m_pWonderProductionAI);
			}
		}

		Firaxis::Array< int, NUM_YIELD_TYPES > yield;
		for(unsigned int j = 0; j < NUM_YIELD_TYPES; ++j)
		{
			yield[j] = 0;
		}

		m_ppaaiSpecialistExtraYield.clear();
		m_ppaaiSpecialistExtraYield.resize(GC.getNumSpecialistInfos());
		for(unsigned int i = 0; i < m_ppaaiSpecialistExtraYield.size(); ++i)
		{
			m_ppaaiSpecialistExtraYield.setAt(i, yield);
		}
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
		m_ppaaiGreatWorkClassYieldChange.clear();
		m_ppaaiGreatWorkClassYieldChange.resize(GC.getNumGreatWorkClassInfos());
		for(unsigned int i = 0; i < m_ppaaiGreatWorkClassYieldChange.size(); ++i)
		{
			m_ppaaiGreatWorkClassYieldChange.setAt(i, yield);
		}
#endif
		m_ppaaiImprovementYieldChange.clear();
		m_ppaaiImprovementYieldChange.resize(GC.getNumImprovementInfos());
		for(unsigned int i = 0; i < m_ppaaiImprovementYieldChange.size(); ++i)
		{
			m_ppaaiImprovementYieldChange.setAt(i, yield);
		}

		m_ppiImprovementYieldChange.clear();
		m_ppiImprovementYieldChange.resize(GC.getNumImprovementInfos());
		for(unsigned int i = 0; i < m_ppiImprovementYieldChange.size(); ++i)
		{
			m_ppiImprovementYieldChange[i] = yield;
		}
		
		m_ppiResourceYieldChange.clear();
		m_ppiResourceYieldChange.resize(GC.getNumResourceInfos());
		for(unsigned int i = 0; i < m_ppiResourceYieldChange.size(); ++i)
		{
			m_ppiResourceYieldChange[i] = yield;
		}

		m_ppaaiBuildingClassYieldMod.clear();
		m_ppaaiBuildingClassYieldMod.resize(GC.getNumBuildingClassInfos());
		for(unsigned int i = 0; i < m_ppaaiBuildingClassYieldMod.size(); ++i)
		{
			m_ppaaiBuildingClassYieldMod.setAt(i, yield);
		}
#if defined(STANDARDIZE_YIELDS)
		m_aBuildingYieldChangeCache.clear();
#endif
		m_aVote.clear();
		m_aUnitExtraCosts.clear();
	}

	m_cities.RemoveAll();

	m_units.RemoveAll();

	m_armyAIs.RemoveAll();


	std::map<int , CvAIOperation*>::iterator iter;
	for(iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		delete(iter->second);
	}
	m_AIOperations.clear();

	if(!bConstructorCall)
	{
		AI_reset();
	}
}

#ifdef GAME_AUTOPAUSE_ON_ACTIVE_DISCONNECT_IF_NOT_SEQUENTIAL
bool CvPlayer::isDisconnected() const
{
	return m_bIsDisconnected;
}
void CvPlayer::setIsDisconnected(bool bNewValue)
{
	m_bIsDisconnected = bNewValue;
}
#endif



void CvPlayer::gameStartInit()
{

	if(CvPreGame::gameStartType() != GAME_LOADED)
	{
		if(!GC.GetEngineUserInterface()->IsLoadedGame())
		{
			InitDangerPlots();
		}
	}

	verifyAlive();
	if(!isAlive())
	{
		return;
	}

	if(!GC.GetEngineUserInterface()->IsLoadedGame())
	{
		InitPlots();
		UpdatePlots();
	}
}





void CvPlayer::setupGraphical()
{
	CvCity* pLoopCity;
	CvUnit* pLoopUnit;


	int iLoop;
	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->setupGraphical();
	}


	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		pLoopUnit->setupGraphical();
	}
}



void CvPlayer::initFreeState(CvGameInitialItemsOverrides& kOverrides)
{
	CvHandicapInfo& kHandicapInfo = getHandicapInfo();


	if(kOverrides.GrantInitialGoldPerPlayer[GetID()])
	{
		int iInitialGold = kHandicapInfo.getStartingGold() + GC.getGame().getStartEraInfo().getStartingGold();
		iInitialGold *= GC.getGame().getGameSpeedInfo().getTrainPercent();
		iInitialGold /= 100;
		GetTreasury()->SetGold(iInitialGold);
	}


	if(kOverrides.GrantInitialCulturePerPlayer[GetID()])
	{
		int iInitialCulture = kHandicapInfo.getStartingPolicyPoints() + GC.getGame().getStartEraInfo().getStartingCulture();
		iInitialCulture *= GC.getGame().getGameSpeedInfo().getTrainPercent();
		iInitialCulture /= 100;
		setJONSCulture(iInitialCulture);


		ChangeJONSCulturePerTurnForFree(kHandicapInfo.getFreeCulturePerTurn());
	}

	ChangeExtraHappinessPerLuxury(kHandicapInfo.getExtraHappinessPerLuxury());


#ifdef AUI_WARNING_FIXES
	for (uint iLoop = 0; iLoop < GC.getNumResourceInfos(); iLoop++)
#else
	for(int iLoop = 0; iLoop < GC.getNumResourceInfos(); iLoop++)
#endif
	{
		const ResourceTypes eResource = static_cast<ResourceTypes>(iLoop);
		CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
		if(pkResource)
		{
			if(pkResource->getStartingResourceQuantity() != 0)
			{
				changeNumResourceTotal(eResource, pkResource->getStartingResourceQuantity());
			}
		}

	}

	DoUpdateHappiness();
#ifdef AUI_CITIZENS_MID_TURN_ASSIGN_RUNS_SELF_CONSISTENCY
	doSelfConsistencyCheckAllCities();
#endif

	clearResearchQueue();
}


void CvPlayer::initFreeUnits(CvGameInitialItemsOverrides&               )
{
	UnitTypes eLoopUnit;
	int iFreeCount;
	int iDefaultAI;
#ifdef AUI_WARNING_FIXES
	uint iI;
	int iJ;
#else
	int iI, iJ;
#endif

	CvEraInfo& gameStartEra = GC.getGame().getStartEraInfo();
	CvHandicapInfo& gameHandicap = GC.getGame().getHandicapInfo();
	CvHandicapInfo& playerHandicap = getHandicapInfo();
	CvCivilizationInfo& playerCivilization = getCivilizationInfo();

	for(iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(iI);
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
		if(pkUnitClassInfo)
		{
			eLoopUnit = (UnitTypes)playerCivilization.getCivilizationUnits(iI);

			if(eLoopUnit != NO_UNIT)
			{
				iFreeCount = playerCivilization.getCivilizationFreeUnitsClass(iI);
				iDefaultAI = playerCivilization.getCivilizationFreeUnitsDefaultUnitAI(iI);

#ifdef NQ_AI_HANDICAP_START
				if(!GC.getGame().isOption("GAMEOPTION_AI_HANDICAP_START"))
				{
					iFreeCount *= (gameStartEra.getStartingUnitMultiplier() + ((!isHuman()) ? gameHandicap.getAIStartingUnitMultiplier() : 0));
				}
#else
				iFreeCount *= (gameStartEra.getStartingUnitMultiplier() + ((!isHuman()) ? gameHandicap.getAIStartingUnitMultiplier() : 0));
#endif

				if(isMinorCiv() && iFreeCount > 1)
					iFreeCount = 1;

				for(iJ = 0; iJ < iFreeCount; iJ++)
				{
					addFreeUnit(eLoopUnit,(UnitAITypes)iDefaultAI);
				}
			}
		}
	}


	int iUnitClass = GetPlayerTraits()->GetFirstFreeUnit(NO_TECH);
	while(iUnitClass != NO_UNITCLASS)
	{
		eLoopUnit = (UnitTypes)playerCivilization.getCivilizationUnits(iUnitClass);
		iDefaultAI = GC.GetGameUnits()->GetEntry(eLoopUnit)->GetDefaultUnitAIType();
		addFreeUnit(eLoopUnit,(UnitAITypes)iDefaultAI);


		iUnitClass = GetPlayerTraits()->GetNextFreeUnit();
	}


	iFreeCount = gameStartEra.getStartingDefenseUnits();
	iFreeCount += playerHandicap.getStartingDefenseUnits();

#ifdef NQ_AI_HANDICAP_START
	if(!GC.getGame().isOption("GAMEOPTION_AI_HANDICAP_START"))
	{
		if(!isHuman())
			iFreeCount += gameHandicap.getAIStartingDefenseUnits();
	}
#else

	if(!isHuman())
		iFreeCount += gameHandicap.getAIStartingDefenseUnits();
#endif

	if(iFreeCount > 0 && !isMinorCiv())
		addFreeUnitAI(UNITAI_DEFENSE, iFreeCount);


	iFreeCount = gameStartEra.getStartingWorkerUnits();
	iFreeCount += playerHandicap.getStartingWorkerUnits();

#ifdef NQ_AI_HANDICAP_START
	if(!GC.getGame().isOption("GAMEOPTION_AI_HANDICAP_START"))
	{
		if(!isHuman())
			iFreeCount += gameHandicap.getAIStartingWorkerUnits();
	}
#else
	if(!isHuman())
		iFreeCount += gameHandicap.getAIStartingWorkerUnits();
#endif

	if(iFreeCount > 0 && !isMinorCiv())
		addFreeUnitAI(UNITAI_WORKER, iFreeCount);


	iFreeCount = gameStartEra.getStartingExploreUnits();
	iFreeCount += playerHandicap.getStartingExploreUnits();

#ifdef NQ_AI_HANDICAP_START
	if(!GC.getGame().isOption("GAMEOPTION_AI_HANDICAP_START"))
	{
		if(!isHuman())
			iFreeCount += gameHandicap.getAIStartingExploreUnits();
	}
#else
	if(!isHuman())
		iFreeCount += gameHandicap.getAIStartingExploreUnits();
#endif


	if(iFreeCount > 0 && !isMinorCiv())
		addFreeUnitAI(UNITAI_EXPLORE, iFreeCount);


	if(GetNumUnitsWithUnitAI(UNITAI_EXPLORE) == 0)
	{
		int iLoop;
		CvUnit* pLoopUnit;
		for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
		{
			if(pLoopUnit->AI_getUnitAIType() == UNITAI_DEFENSE)
			{
				pLoopUnit->AI_setUnitAIType(UNITAI_EXPLORE);
				break;
			}
		}
	}
}



void CvPlayer::addFreeUnitAI(UnitAITypes eUnitAI, int iCount)
{
#ifdef AUI_WARNING_FIXES
	uint iI;
#else
	int iI;
#endif

	UnitTypes eBestUnit = NO_UNIT;
	int iBestValue = 0;

	CvCivilizationInfo& playerCivilzationInfo = getCivilizationInfo();
	for(iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(iI);
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
		if(pkUnitClassInfo)
		{
			UnitTypes eLoopUnit = (UnitTypes)playerCivilzationInfo.getCivilizationUnits(iI);
			if(eLoopUnit != NO_UNIT)
			{
				CvUnitEntry* pUnitInfo = GC.getUnitInfo(eLoopUnit);
				if(pUnitInfo != NULL)
				{
					if(canTrain(eLoopUnit))
					{
						bool bValid = true;
#ifdef AUI_WARNING_FIXES
						for (uint iJ = 0; iJ < GC.getNumResourceInfos(); iJ++)
#else
						for(int iJ = 0; iJ < GC.getNumResourceInfos(); iJ++)
#endif
						{
							const ResourceTypes eResource = static_cast<ResourceTypes>(iJ);
							CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
							if(pkResource)
							{
								if(pUnitInfo->GetResourceQuantityRequirement(iJ) > 0)
								{
									bValid = false;
								}
							}
						}

						if(bValid)
						{
							int iValue = 0;


							if(pUnitInfo->GetDefaultUnitAIType() == eUnitAI)
								iValue += (pUnitInfo->GetProductionCost() * 2);

							else if(pUnitInfo->GetUnitAIType(eUnitAI))
								iValue += (pUnitInfo->GetProductionCost());

							if(iValue > iBestValue)
							{
								eBestUnit = eLoopUnit;
								iBestValue = iValue;
							}
						}
					}
				}
			}
		}

	}

	if(eBestUnit != NO_UNIT)
	{
#ifdef AUI_WARNING_FIXES
		for (int iJ = 0; iJ < iCount; iJ++)
#else
		for(iI = 0; iI < iCount; iI++)
#endif
		{
			addFreeUnit(eBestUnit, eUnitAI);
		}
	}
}



#if !defined(TRAITIFY)
CvPlot* CvPlayer::addFreeUnit(UnitTypes eUnit, UnitAITypes eUnitAI)
{
	CvPlot* pStartingPlot;
	CvPlot* pBestPlot;
	CvPlot* pLoopPlot;
	CvPlot* pReturnValuePlot = NULL;

	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	if (pkUnitInfo == NULL)
		return pReturnValuePlot;

	if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		if ((eUnitAI == UNITAI_SETTLE) || (pkUnitInfo->GetDefaultUnitAIType() == UNITAI_SETTLE))
		{
			if (GetNumUnitsWithUnitAI(UNITAI_SETTLE) >= 1)
			{
				return pReturnValuePlot;
			}
		}
	}



	if (GetPlayerTraits()->IsNoAnnexing())
	{

		if ((eUnitAI == UNITAI_SETTLE) || (pkUnitInfo->GetDefaultUnitAIType() == UNITAI_SETTLE))
		{

			if (GetNumUnitsWithUnitAI(UNITAI_SETTLE) >= 1)
			{


#ifdef AUI_WARNING_FIXES
				for (uint iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
#else
				for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
#endif
				{
					const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(iI);
					CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
					if (pkUnitClassInfo)
					{
						const UnitTypes eLocalUnit = (UnitTypes)getCivilizationInfo().getCivilizationUnits(eUnitClass);
						if (eLocalUnit != NO_UNIT)
						{
							CvUnitEntry* pUnitEntry = GC.getUnitInfo(eLocalUnit);
							if (pUnitEntry->IsCanBuyCityState())
							{

								eUnit = eLocalUnit;
								eUnitAI = (UnitAITypes)pkUnitInfo->GetDefaultUnitAIType();
								break;
							}
						}
					}
				}
			}
		}
	}

	CvCity* pCapital = getCapitalCity();

	if (pCapital)
	{
		pStartingPlot = pCapital->plot();
	}
	else
	{
		pStartingPlot = getStartingPlot();
	}

	if (pStartingPlot != NULL)
	{
		pBestPlot = NULL;

		if (isHuman())
		{
			if (!(pkUnitInfo->IsFound()))
			{
				DirectionTypes eDirection;

				bool bDirectionValid;

				int iCount = 0;


				do
				{
					bDirectionValid = true;

					eDirection = (DirectionTypes)GC.getGame().getJonRandNum(NUM_DIRECTION_TYPES, "Placing Starting Units (Human)");

					if (bDirectionValid)
					{
						pLoopPlot = plotDirection(pStartingPlot->getX(), pStartingPlot->getY(), eDirection);

						if (pLoopPlot != NULL && pLoopPlot->getArea() == pStartingPlot->getArea())
						{
							if (!pLoopPlot->isImpassable() && !pLoopPlot->isMountain())
							{
								if (!(pLoopPlot->isUnit()))
								{
									if (!(pLoopPlot->isGoody()))
									{
										pBestPlot = pLoopPlot;
										break;
									}
								}
							}
						}
					}


					iCount++;
				} while (iCount < 1000);
			}
		}

		if (pBestPlot == NULL)
		{
			pBestPlot = pStartingPlot;
		}

		CvUnit* pNewUnit = initUnit(eUnit, pBestPlot->getX(), pBestPlot->getY(), eUnitAI);
		CvAssert(pNewUnit != NULL);
		if (pNewUnit == NULL)
			return NULL;


		if (pBestPlot->getNumUnits() > 1)
		{
			if (!pNewUnit->jumpToNearestValidPlot())
			{

				pNewUnit->kill(false);
				return NULL;
			}
		}
		pReturnValuePlot = pNewUnit->plot();
	}

	return pReturnValuePlot;
}
#else 

CvPlot* CvPlayer::addFreeUnit(UnitTypes eUnit, UnitAITypes eUnitAI)
{
	CvPlot* pStartingPlot;
	CvPlot* pReturnValuePlot = NULL;

	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	if (pkUnitInfo == NULL)
		return pReturnValuePlot;

	if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		if ((eUnitAI == UNITAI_SETTLE) || (static_cast<UnitAITypes>(pkUnitInfo->GetDefaultUnitAIType()) == UNITAI_SETTLE))
		{
			if (GetNumUnitsWithUnitAI(UNITAI_SETTLE) >= 1)
			{
				return pReturnValuePlot;
			}
		}
	}


	if (GetPlayerTraits()->IsNoAnnexing())
	{
		if ((eUnitAI == UNITAI_SETTLE) || (static_cast<UnitAITypes>(pkUnitInfo->GetDefaultUnitAIType()) == UNITAI_SETTLE))
		{
			if (GetNumUnitsWithUnitAI(UNITAI_SETTLE) >= 1)
			{
				for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
				{
					const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(iI);
					CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
					if (pkUnitClassInfo)
					{
						const UnitTypes eLocalUnit = (UnitTypes)getCivilizationInfo().getCivilizationUnits(eUnitClass);
						if (eLocalUnit != NO_UNIT)
						{
							CvUnitEntry* pUnitEntry = GC.getUnitInfo(eLocalUnit);
							if (pUnitEntry->IsCanBuyCityState())
							{
								eUnit = eLocalUnit;
								eUnitAI = static_cast<UnitAITypes>(pkUnitInfo->GetDefaultUnitAIType());
								break;
							}
						}
					}
				}
			}
		}
	}

	CvCity* pCapital = getCapitalCity();

	if (pCapital)
	{
		pStartingPlot = pCapital->plot();
	}
	else
	{
		pStartingPlot = getStartingPlot();
	}


	bool bForceCapitalSpawn = false;
	UnitAITypes eDefaultAI = static_cast<UnitAITypes>(pkUnitInfo->GetDefaultUnitAIType());
	UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(pkUnitInfo->GetUnitClassType());

	if (eDefaultAI == UNITAI_TRADE_UNIT)
	{
		bForceCapitalSpawn = true;
	}


	if (eUnitClass != NO_UNITCLASS && GetPlayerTraits()->IsUnitClassForceSpawnCapital(eUnitClass))
	{
		bForceCapitalSpawn = true;
	}



	if (bForceCapitalSpawn && pCapital)
	{
		pStartingPlot = pCapital->plot();
	}


	if (pStartingPlot != NULL)
	{
		CvUnit* pNewUnit = initUnit(eUnit, pStartingPlot->getX(), pStartingPlot->getY(), eUnitAI);
		CvAssert(pNewUnit != NULL);
		if (pNewUnit == NULL)
			return NULL;


		if (bForceCapitalSpawn)
		{
			return pStartingPlot;
		}

		CvPlot* pBestPlot = NULL;
		if (isHuman() && !(pkUnitInfo->IsFound()))
		{
			DirectionTypes eDirection;
			bool bDirectionValid;
			int iCount = 0;


			do
			{
				bDirectionValid = true;
				eDirection = (DirectionTypes)GC.getGame().getJonRandNum(NUM_DIRECTION_TYPES, "Placing Starting Units (Human)");

				if (bDirectionValid)
				{
					CvPlot* pLoopPlot = plotDirection(pStartingPlot->getX(), pStartingPlot->getY(), eDirection);

					if (pLoopPlot != NULL && pLoopPlot->getArea() == pStartingPlot->getArea())
					{
						if (!pLoopPlot->isImpassable() && !pLoopPlot->isMountain())
						{
							if (!(pLoopPlot->isUnit()))
							{
								if (!(pLoopPlot->isGoody()))
								{
									pBestPlot = pLoopPlot;
									break;
								}
							}
						}
					}
				}
				iCount++;
			} while (iCount < 1000);
		}


		if (pBestPlot == NULL)
		{
			pBestPlot = pStartingPlot;
		}


		if (!bForceCapitalSpawn)
		{
			pNewUnit->setXY(pBestPlot->getX(), pBestPlot->getY(), false, false, false, false);
		}

		pReturnValuePlot = pNewUnit->plot();
	}

	return pReturnValuePlot;
}
#endif

CvCity* CvPlayer::initCity(int iX, int iY, bool bBumpUnits, bool bInitialFounding)
{
	CvCity* pCity = addCity();

	CvAssertMsg(pCity != NULL, "City is not assigned a valid value");
	if(pCity != NULL)
	{
		CvAssertMsg(!(GC.getMap().plot(iX, iY)->isCity()), "No city is expected at this plot when initializing new city");
		pCity->init(pCity->GetID(), GetID(), iX, iY, bBumpUnits, bInitialFounding);
		pCity->GetCityStrategyAI()->UpdateFlavorsForNewCity();
	}

	return pCity;
}



#if !defined(LEKMOD_MERCHANT_BUYOUT_NOT_NOANNEXING)
void CvPlayer::acquireCity(CvCity* pOldCity, bool bConquest, bool bGift)
#else
void CvPlayer::acquireCity(CvCity* pOldCity, bool bConquest, bool bGift, bool bPurchased)
#endif
{
	if(pOldCity == NULL)
		return;

	IDInfo* pUnitNode;
	CvCity* pNewCity;
	CvUnit* pLoopUnit;
	CvPlot* pCityPlot;

	CvString strBuffer;
	CvString strName;
	bool abEverOwned[MAX_PLAYERS];
	PlayerTypes eOldOwner;
#ifdef AUI_PLAYER_FIX_ACQUIRE_CITY_NO_CITY_LOSSES_ON_RECAPTURE
	PlayerTypes eOriginalOwner = pOldCity->getOriginalOwner();
#else
	PlayerTypes eOriginalOwner;
#endif
	BuildingTypes eBuilding;
#ifdef AUI_WARNING_FIXES
	bool bRecapture = false;
	int iCaptureGold = 0;
	int iCaptureCulture = 0;
	int iCaptureGreatWorks = 0;
#else
	bool bRecapture;
	int iCaptureGold;
	int iCaptureCulture;
	int iCaptureGreatWorks;
#endif
	int iGameTurnFounded;
	int iPopulation;
	int iHighestPopulation;
	int iOldPopulation;
	int iBattleDamage;
#ifdef AUI_WARNING_FIXES
	uint iI;
#else
	int iI;
#endif
	FFastSmallFixedList<IDInfo, 25, true, c_eCiv5GameplayDLL > oldUnits;
#ifdef AUI_CITY_FIX_COMPONENT_CONSTRUCTORS_CONTAIN_POINTERS
	CvCityReligions tempReligions(pOldCity);
#else
	CvCityReligions tempReligions;
#endif
#if !defined(LEKMOD_MERCHANT_BUYOUT_NOT_NOANNEXING)
	bool bIsMinorCivBuyout = (pOldCity->GetPlayer()->isMinorCiv() && bGift && (IsAbleToAnnexCityStates() || GetPlayerTraits()->IsNoAnnexing()));
#else
	bool bIsMinorCivBuyout = (pOldCity->GetPlayer()->isMinorCiv() && bGift && (IsAbleToAnnexCityStates() || GetPlayerTraits()->IsNoAnnexing() || bPurchased));
#endif
	pCityPlot = pOldCity->plot();

	pUnitNode = pCityPlot->headUnitNode();

	while(pUnitNode != NULL)
	{
		oldUnits.insertAtEnd(pUnitNode);
		pUnitNode = pCityPlot->nextUnitNode((IDInfo*)pUnitNode);
	}

	pUnitNode = oldUnits.head();

	while(pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = oldUnits.next(pUnitNode);

		if(pLoopUnit && pLoopUnit->getTeam() != getTeam())
		{
			if(pLoopUnit->IsImmobile())
			{
				pLoopUnit->kill(false, GetID());
				DoUnitKilledCombat(pLoopUnit->getOwner(), pLoopUnit->getUnitType());
			}
		}
	}

#ifdef BUILDINGS_DESTROY_ONCE_PER_TURN
	int iTurnsSinceAcquire = GC.getGame().getGameTurn() - pOldCity->getGameTurnAcquired();
#endif

	if(bConquest)
	{
		CvNotifications* pNotifications = GET_PLAYER(pOldCity->getOwner()).GetNotifications();
		if(pNotifications)
		{
			Localization::String locString = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_LOST");
			locString << pOldCity->getNameKey() << getNameKey();
			Localization::String locSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_LOST");
			locSummary << pOldCity->getNameKey();
			pNotifications->Add(NOTIFICATION_CITY_LOST, locString.toUTF8(), locSummary.toUTF8(), pOldCity->getX(), pOldCity->getY(), -1);
		}

		if(!isBarbarian() && !pOldCity->isBarbarian())
		{
			int iDefaultCityValue =         GC.getWAR_DAMAGE_LEVEL_CITY_WEIGHT();


			int iValue = iDefaultCityValue;
			iValue += pOldCity->getPopulation() *         GC.getWAR_DAMAGE_LEVEL_INVOLVED_CITY_POP_MULTIPLIER();
			if (pOldCity->IsOriginalCapital())
			{
				iValue *= 3;
				iValue /= 2;
			}


			GetDiplomacyAI()->ChangeOtherPlayerWarValueLost(pOldCity->getOwner(), GetID(), iValue);

			GET_PLAYER(pOldCity->getOwner()).GetDiplomacyAI()->ChangeWarValueLost(GetID(), iValue);


			PlayerTypes ePlayer;
			CvDiplomacyAI* pOldOwnerDiploAI = GET_PLAYER(pOldCity->getOwner()).GetDiplomacyAI();
			int iNumLiberatedCities = pOldOwnerDiploAI->GetNumCitiesLiberated(GetID());
			pOldOwnerDiploAI->ChangeNumCitiesLiberated(GetID(), -iNumLiberatedCities);

			iValue = iDefaultCityValue;
			iValue += pOldCity->getPopulation() *         GC.getWAR_DAMAGE_LEVEL_UNINVOLVED_CITY_POP_MULTIPLIER();


			for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
			{
				ePlayer = (PlayerTypes) iPlayerLoop;


				if(ePlayer != GetID() && ePlayer != pOldCity->getOwner())
				{
					GET_PLAYER(ePlayer).GetDiplomacyAI()->ChangeOtherPlayerWarValueLost(pOldCity->getOwner(), GetID(), iValue);
				}
			}
		}

		GetMilitaryAI()->LogCityCaptured(pOldCity, pOldCity->getOwner());
	}

	if(pOldCity->getOriginalOwner() == pOldCity->getOwner())
	{
		GET_PLAYER(pOldCity->getOriginalOwner()).changeCitiesLost(1);
	}
	else if(pOldCity->getOriginalOwner() == GetID())
	{
		GET_PLAYER(pOldCity->getOriginalOwner()).changeCitiesLost(-1);
	}

	if(bConquest)
	{
		if(GetID() == GC.getGame().getActivePlayer())
		{
			strBuffer = GetLocalizedText("TXT_KEY_MISC_CAPTURED_CITY", pOldCity->getNameKey()).GetCString();
			GC.GetEngineUserInterface()->AddCityMessage(0, pOldCity->GetIDInfo(), GetID(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer                                                                                                                                                            );
		}

		strName.Format("%s (%s)", pOldCity->getName().GetCString(), GET_PLAYER(pOldCity->getOwner()).getName());

		for(iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if((PlayerTypes)iI == GC.getGame().getActivePlayer())
			{
				if(GET_PLAYER((PlayerTypes)iI).isAlive())
				{
#ifdef AUI_WARNING_FIXES
					if ((PlayerTypes)iI != GetID())
#else
					if(iI != GetID())
#endif
					{
						if(pOldCity->isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_CITY_CAPTURED_BY", strName.GetCString(), getCivilizationShortDescriptionKey());
							GC.GetEngineUserInterface()->AddCityMessage(0, pOldCity->GetIDInfo(), ((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), strBuffer                                                                                                                                                           );
						}
					}
				}
			}
		}

		strBuffer = GetLocalizedText("TXT_KEY_MISC_CITY_WAS_CAPTURED_BY", strName.GetCString(), getCivilizationShortDescriptionKey());
		GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, GetID(), strBuffer, pOldCity->getX(), pOldCity->getY());

#ifndef FINAL_RELEASE
		OutputDebugString("\n"); OutputDebugString(strBuffer); OutputDebugString("\n\n");
#endif
#ifndef AUI_WARNING_FIXES
	}

	iCaptureGold = 0;
	iCaptureCulture = 0;
	iCaptureGreatWorks = 0;

	if(bConquest)
	{
		iCaptureGold = 0;
#endif

		iCaptureGold += GC.getBASE_CAPTURE_GOLD();
		iCaptureGold += (pOldCity->getPopulation() * GC.getCAPTURE_GOLD_PER_POPULATION());
		iCaptureGold += GC.getGame().getJonRandNum(GC.getCAPTURE_GOLD_RAND1(), "Capture Gold 1");
		iCaptureGold += GC.getGame().getJonRandNum(GC.getCAPTURE_GOLD_RAND2(), "Capture Gold 2");

		if(GC.getCAPTURE_GOLD_MAX_TURNS() > 0)
		{
			iCaptureGold *= range((GC.getGame().getGameTurn() - pOldCity->getGameTurnAcquired()), 0, GC.getCAPTURE_GOLD_MAX_TURNS());
			iCaptureGold /= GC.getCAPTURE_GOLD_MAX_TURNS();
		}

		iCaptureGold *= (100 + pOldCity->getCapturePlunderModifier()) / 100;
		iCaptureGold *= (100 + GetPlayerTraits()->GetPlunderModifier()) / 100;
#ifndef AUI_WARNING_FIXES
	}
#endif

	GetTreasury()->ChangeGold(iCaptureGold);

#ifndef AUI_WARNING_FIXES
	if(bConquest)
	{
#endif
#ifdef AUI_PLAYER_FIX_JONS_CULTURE_IS_T100
		iCaptureCulture = pOldCity->getJONSCulturePerTurnTimes100();
		iCaptureCulture *= GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURAL_PLUNDER_MULTIPLIER);

		if (iCaptureCulture > 0)
		{
			changeJONSCultureTimes100(iCaptureCulture);
#else
		iCaptureCulture = pOldCity->getJONSCulturePerTurn();
		iCaptureCulture *= GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURAL_PLUNDER_MULTIPLIER);

		if(iCaptureCulture > 0)
		{
			changeJONSCulture(iCaptureCulture);
#endif
#ifdef UPDATE_CULTURE_NOTIFICATION_DURING_TURN

			if (isAlive() && isHuman() && getNumCities() > 0)
			{
				TestMidTurnPolicyNotification();
			}
#endif
		}
#ifndef AUI_WARNING_FIXES
	}

	if(bConquest)
	{
#endif
#ifdef NQ_SPOILS_OF_WAR




		
		if (!pOldCity->isEverOwned(GetID()) && !GET_PLAYER(pOldCity->getOriginalOwner()).isMinorCiv() && !pOldCity->IsPuppet())
		{
			DoTechFromCityConquer(pOldCity);
		}
		





		

#endif
		if (GetPlayerTraits()->IsTechFromCityConquer())
		{

			if (!pOldCity->isEverOwned(GetID()))
			{
#ifdef NQ_GREAT_WORK_ON_UNIQUE_CONQUEST
				DoGreatWorkFromCityConquer(pOldCity);
#else
				DoTechFromCityConquer(pOldCity);
#endif
			}
		}
#ifndef AUI_WARNING_FIXES
	}


	if (bConquest)
	{
#endif
		if(!isMinorCiv())
		{
#ifdef AUI_PLAYER_FIX_ACQUIRE_CITY_NO_CITY_LOSSES_ON_RECAPTURE
			if (GET_PLAYER(eOriginalOwner).getTeam() != getTeam())
#else
			bool bDoWarmonger = true;


			if (pOldCity->getOriginalOwner() == GetID())
			{
				bDoWarmonger = false;
			}

			if (bDoWarmonger)
#endif
			{
				CvDiplomacyAIHelpers::ApplyWarmongerPenalties(GetID(), pOldCity->getOwner());
			}
		}
	}

	int iNumBuildingInfos = GC.getNumBuildingInfos();
	std::vector<int> paiNumRealBuilding(iNumBuildingInfos, 0);
	std::vector<int> paiBuildingOriginalOwner(iNumBuildingInfos, 0);
	std::vector<int> paiBuildingOriginalTime(iNumBuildingInfos, 0);
	struct CopyGreatWorkData
	{
		int m_iGreatWork;
		BuildingTypes m_eBuildingType;
		int m_iSlot;
		bool m_bTransferred;
	};
	std::vector<CopyGreatWorkData> paGreatWorkData;
	int iOldCityX = pOldCity->getX();
	int iOldCityY = pOldCity->getY();
	eOldOwner = pOldCity->getOwner();
	eOriginalOwner = pOldCity->getOriginalOwner();
	iGameTurnFounded = pOldCity->getGameTurnFounded();
	iPopulation = pOldCity->getPopulation();
	iOldPopulation = iPopulation;
	iHighestPopulation = pOldCity->getHighestPopulation();
	bool bEverCapital = pOldCity->IsEverCapital();
	strName = pOldCity->getNameKey();
	int iOldCultureLevel = pOldCity->GetJONSCultureLevel();
	bool bHasMadeAttack = pOldCity->isMadeAttack();

	tempReligions.Init(pOldCity);
	tempReligions.Copy(pOldCity->GetCityReligions());

	iBattleDamage = pOldCity->getDamage();

#ifdef LEKMOD_TRACK_CITY_SETTLER_UNITTYPE
	UnitTypes eAcquiredCitySettlerUnit = pOldCity->SettlerUnit();
#endif

#if defined(LEKMOD_FREE_RESOURCE_CITY_GRANT)
	const int iNumResourceInfosForFreeGrant = GC.getNumResourceInfos();
	std::vector<int> paiFreeResourceCityGrant(iNumResourceInfosForFreeGrant, 0);
	int aiYieldFromFreeResourceCity[NUM_YIELD_TYPES] = {};
	for (int iResource = 0; iResource < iNumResourceInfosForFreeGrant; ++iResource)
	{
		paiFreeResourceCityGrant[iResource] = pOldCity->GetFreeResource((ResourceTypes)iResource);
	}
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		aiYieldFromFreeResourceCity[iYield] = pOldCity->GetYieldFromFreeResourceCity((YieldTypes)iYield);
	}
#endif


	if(!bGift || !isHuman() || !GET_PLAYER(pOldCity->getOwner()).isHuman())
	{
		int iBattleDamgeThreshold = 200 *        GC.getCITY_CAPTURE_DAMAGE_PERCENT();
		iBattleDamgeThreshold /= 100;

		if(iBattleDamage > iBattleDamgeThreshold)
		{
			iBattleDamage = iBattleDamgeThreshold;
		}
	}

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		abEverOwned[iI] = pOldCity->isEverOwned((PlayerTypes)iI);
	}

	abEverOwned[GetID()] = true;

	for(iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		paiNumRealBuilding[iI] = pOldCity->GetCityBuildings()->GetNumRealBuilding((BuildingTypes)iI);
		paiBuildingOriginalOwner[iI] = pOldCity->GetCityBuildings()->GetBuildingOriginalOwner((BuildingTypes)iI);
		paiBuildingOriginalTime[iI] = pOldCity->GetCityBuildings()->GetBuildingOriginalTime((BuildingTypes)iI);

		if (pOldCity->GetCityBuildings()->GetNumBuilding((BuildingTypes)iI) > 0)
		{
			CvBuildingEntry *pkBuilding = GC.getBuildingInfo((BuildingTypes)iI);
			if (pkBuilding)
			{
#ifdef AUI_WARNING_FIXES
				for (uint jJ = 0; jJ < pkBuilding->GetGreatWorkCount(); jJ++)
				{
					int iGreatWork = pOldCity->GetCityBuildings()->GetBuildingGreatWork((BuildingClassTypes)pkBuilding->GetBuildingClassType(), (int)jJ);
#else
				for (int jJ = 0; jJ < pkBuilding->GetGreatWorkCount(); jJ++)
				{
					int iGreatWork = pOldCity->GetCityBuildings()->GetBuildingGreatWork((BuildingClassTypes)pkBuilding->GetBuildingClassType(), jJ);
#endif
					if (iGreatWork != NO_GREAT_WORK)
					{
						CopyGreatWorkData kData;
						kData.m_iGreatWork = iGreatWork;
						kData.m_eBuildingType = (BuildingTypes)iI;
						kData.m_iSlot = jJ;
						kData.m_bTransferred = false;
						paGreatWorkData.push_back(kData);

						CvPlayer &kOldCityPlayer = GET_PLAYER(pOldCity->getOriginalOwner());
						if (kOldCityPlayer.GetCulture()->GetSwappableWritingIndex() == iGreatWork)
						{
							kOldCityPlayer.GetCulture()->SetSwappableWritingIndex(-1);
						}
						if (kOldCityPlayer.GetCulture()->GetSwappableArtifactIndex() == iGreatWork)
						{
							kOldCityPlayer.GetCulture()->SetSwappableArtifactIndex(-1);
						}
						if (kOldCityPlayer.GetCulture()->GetSwappableArtIndex() == iGreatWork)
						{
							kOldCityPlayer.GetCulture()->SetSwappableArtIndex(-1);
						}
						if (kOldCityPlayer.GetCulture()->GetSwappableMusicIndex() == iGreatWork)
						{
							kOldCityPlayer.GetCulture()->SetSwappableMusicIndex(-1);
						}
					}
				}
			}
		}
	}

	std::vector<BuildingYieldChange> aBuildingYieldChange;
	for(iI = 0; iI < GC.getNumBuildingClassInfos(); ++iI)
	{
		CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo((BuildingClassTypes)iI);
		if(!pkBuildingClassInfo)
		{
			continue;
		}

		for(int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			BuildingYieldChange kChange;
			kChange.eBuildingClass = (BuildingClassTypes)iI;
			kChange.eYield = (YieldTypes)iYield;
			kChange.iChange = pOldCity->GetCityBuildings()->GetBuildingYieldChange((BuildingClassTypes)iI, (YieldTypes)iYield);
			if(0 != kChange.iChange)
			{
				aBuildingYieldChange.push_back(kChange);
			}
		}
	}

#ifdef AUI_PLAYER_FIX_ACQUIRE_CITY_NO_CITY_LOSSES_ON_RECAPTURE
	bRecapture = GET_PLAYER(eOriginalOwner).getTeam() == getTeam();
#else
	bRecapture = false;
#endif


	CvCityEspionage* pOldCityEspionage = pOldCity->GetCityEspionage();
	if(pOldCityEspionage)
	{
		for(int i = 0; i < MAX_MAJOR_CIVS; i++)
		{
			int iAssignedSpy = pOldCityEspionage->m_aiSpyAssignment[i];

			if(iAssignedSpy != -1)
			{
				CvNotifications* pNotifications = GET_PLAYER((PlayerTypes)i).GetNotifications();
				if(pNotifications)
				{
					CvPlayerEspionage* pEspionage = GET_PLAYER((PlayerTypes)i).GetEspionage();
					CvEspionageSpy* pSpy = &(pEspionage->m_aSpyList[iAssignedSpy]);

					Localization::String strSummary;
					Localization::String strNotification;
					if(bConquest)
					{
						strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SPY_EVICTED_CONQUEST_S");
						if(((PlayerTypes)i) == GetID())
						{
							strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_EVICTED_CONQUEST_YOU");
							strNotification << pEspionage->GetSpyRankName(pSpy->m_eRank);
							strNotification << GET_PLAYER((PlayerTypes)i).getCivilizationInfo().getSpyNames(pSpy->m_iName);
							strNotification << pOldCity->getNameKey();
						}
						else
						{
							strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_EVICTED_CONQUEST");
							strNotification << pEspionage->GetSpyRankName(pSpy->m_eRank);
							strNotification << GET_PLAYER((PlayerTypes)i).getCivilizationInfo().getSpyNames(pSpy->m_iName);
							strNotification << pOldCity->getNameKey();
							strNotification << getCivilizationInfo().getShortDescriptionKey();
						}
					}
					else
					{
						strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SPY_EVICTED_TRADE_S");
						if(((PlayerTypes)i) == GetID())
						{
							strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_EVICTED_TRADE_YOU");
							strNotification << pEspionage->GetSpyRankName(pSpy->m_eRank);
							strNotification << GET_PLAYER((PlayerTypes)i).getCivilizationInfo().getSpyNames(pSpy->m_iName);
							strNotification << pOldCity->getNameKey();
						}
						else
						{
							strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_EVICTED_TRADE");
							strNotification << pEspionage->GetSpyRankName(pSpy->m_eRank);
							strNotification << GET_PLAYER((PlayerTypes)i).getCivilizationInfo().getSpyNames(pSpy->m_iName);
							strNotification << pOldCity->getNameKey();
							strNotification << getCivilizationInfo().getShortDescriptionKey();
						}
					}

					pNotifications->Add(NOTIFICATION_SPY_EVICTED, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, pOldCity->getOwner());
				}

				GET_PLAYER((PlayerTypes)i).GetEspionage()->ExtractSpyFromCity(iAssignedSpy);

			}
		}
	}

	GC.getGame().GetGameTrade()->ClearAllCityTradeRoutes(pCityPlot);

	bool bCapital = pOldCity->isCapital();


	FStaticVector<int, 121, true, c_eCiv5GameplayDLL, 0> aiPurchasedPlotX;
	FStaticVector<int, 121, true, c_eCiv5GameplayDLL, 0> aiPurchasedPlotY;
	const int iMaxRange =       GC.getMAXIMUM_ACQUIRE_PLOT_DISTANCE();

#ifdef AUI_WARNING_FIXES
	for (uint iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
#else
	for(int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
#endif
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);
		if(pLoopPlot && pLoopPlot->GetCityPurchaseOwner() == eOldOwner && pLoopPlot->GetCityPurchaseID() == pOldCity->GetID())
		{
			aiPurchasedPlotX.push_back(pLoopPlot->getX());
			aiPurchasedPlotY.push_back(pLoopPlot->getY());
			pLoopPlot->ClearCityPurchaseInfo();
		}
	}

	pOldCity->PreKill();

	{
		auto_ptr<ICvCity1> pkDllOldCity(new CvDllCity(pOldCity));
		gDLL->GameplayCityCaptured(pkDllOldCity.get(), GetID());
	}

	GET_PLAYER(eOldOwner).deleteCity(pOldCity->GetID());


	GC.getGame().addReplayMessage(REPLAY_MESSAGE_CITY_CAPTURED, m_eID, "", pCityPlot->getX(), pCityPlot->getY());

	PlayerTypes ePlayer;

	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(ePlayer != m_eID)
		{
			if(GET_PLAYER(ePlayer).isAlive())
			{
				GET_PLAYER(m_eID).DoUpdateProximityToPlayer(ePlayer);
				GET_PLAYER(ePlayer).DoUpdateProximityToPlayer(m_eID);
			}
		}
	}

	GC.getMap().updateWorkingCity(pCityPlot,NUM_CITY_RINGS*2);


	if(bCapital)
	{
		GET_PLAYER(eOldOwner).findNewCapital();
		GET_TEAM(getTeam()).resetVictoryProgress();
	}

	GC.GetEngineUserInterface()->setDirty(NationalBorders_DIRTY_BIT, true);


	pNewCity = initCity(pCityPlot->getX(), pCityPlot->getY(), !bConquest, (!bConquest && !bGift));

	CvAssertMsg(pNewCity != NULL, "NewCity is not assigned a valid value");

#ifdef _MSC_VER
#pragma warning ( push )
#pragma warning ( disable : 6011 ) 
#endif


	if(bIsMinorCivBuyout)
	{
		pNewCity->setPreviousOwner(NO_PLAYER);
		pNewCity->setOriginalOwner(m_eID);
		pNewCity->setGameTurnFounded(GC.getGame().getGameTurn());
		pNewCity->SetEverCapital(false);
		AwardFreeBuildings(pNewCity);
	}

	else
	{
		pNewCity->setPreviousOwner(eOldOwner);
		pNewCity->setOriginalOwner(eOriginalOwner);
		pNewCity->setGameTurnFounded(iGameTurnFounded);
		pNewCity->SetEverCapital(bEverCapital);
	}

#ifdef LEKMOD_TRACK_CITY_SETTLER_UNITTYPE
	if (!bIsMinorCivBuyout)
	{
		pNewCity->SetSettlerUnit(eAcquiredCitySettlerUnit);
	}
#endif

#if defined(LEKMOD_FREE_RESOURCE_CITY_GRANT)
	if (!bIsMinorCivBuyout)
	{
		for (int iResource = 0; iResource < iNumResourceInfosForFreeGrant; ++iResource)
		{
			const int iQty = paiFreeResourceCityGrant[iResource];
			if (iQty != 0)
			{
				pNewCity->ChangeFreeResource((ResourceTypes)iResource, iQty);
			}
		}
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			pNewCity->SetYieldFromFreeResourceCity((YieldTypes)iYield, aiYieldFromFreeResourceCity[iYield]);
		}
		if (pNewCity->plot())
		{
			pNewCity->plot()->updateYield();
		}
	}
#endif


	if(!bRecapture && bConquest)
	{
		int iPercentPopulationRetained =        GC.getCITY_CAPTURE_POPULATION_PERCENT();
		int iInfluenceReduction = GetCulture()->GetInfluenceCityConquestReduction(eOldOwner);
		iPercentPopulationRetained += (iInfluenceReduction * (100 - iPercentPopulationRetained) / 100);

		iPopulation = max(1, iPopulation * iPercentPopulationRetained / 100);
	}

	pNewCity->setPopulation(iPopulation);
	pNewCity->setHighestPopulation(iHighestPopulation);
	pNewCity->setName(strName);
	pNewCity->setNeverLost(false);
	pNewCity->setDamage(iBattleDamage,true);
	pNewCity->setMadeAttack(bHasMadeAttack);

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		pNewCity->setEverOwned(((PlayerTypes)iI), abEverOwned[iI]);
	}

	pNewCity->SetJONSCultureLevel(iOldCultureLevel);
	pNewCity->GetCityReligions()->Copy(&tempReligions);
	pNewCity->GetCityReligions()->RemoveFormerPantheon();

	if(bCapital)
	{
		GET_PLAYER(eOldOwner).SetHasLostCapital(true, m_eID);
	}

	CvCivilizationInfo& playerCivilizationInfo = getCivilizationInfo();

	if(bConquest && !GC.getGame().isGameMultiPlayer() && isHuman())
	{
		const char* szCivKey = getCivilizationTypeKey();


		if(strcmp(szCivKey, "CIVILIZATION_INDONESIA") == 0)
		{
			CvUnit *pConqueringUnit = pCityPlot->getUnitByIndex(0);
			if (pConqueringUnit->getUnitType() == (UnitTypes)GC.getInfoTypeForString("UNIT_KRIS_SWORDSMAN", true))
			{
				PromotionTypes ePromotion = (PromotionTypes)GC.getInfoTypeForString("PROMOTION_ENEMY_BLADE", true);
				if (pConqueringUnit->isHasPromotion(ePromotion))
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_XP2_21);
				}
			}
		}


		bool bUsingXP1Scenario1 = gDLL->IsModActivated(CIV5_XP1_SCENARIO1_MODID);
		bool bUsingXP1Scenario2 = gDLL->IsModActivated(CIV5_XP1_SCENARIO2_MODID);
		bool bUsingXP2Scenario1 = gDLL->IsModActivated(CIV5_XP2_SCENARIO1_MODID);

		const char* szNameKey = pNewCity->getNameKey();
		if(bUsingXP2Scenario1)
		{
			if(strcmp(szCivKey, "CIVILIZATION_ENGLAND") == 0)
			{
				if(strcmp(szNameKey, "TXT_KEY_CIVIL_WAR_SCENARIO_CITY_NAME_GETTYSBURG") == 0)
				{
					CvUnit *pConqueringUnit = pCityPlot->getUnitByIndex(0);
					PromotionTypes ePromotion = (PromotionTypes)GC.getInfoTypeForString("PROMOTION_PICKETT", true);
					if (pConqueringUnit->isHasPromotion(ePromotion))
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_59);
					}
				}
			}
		}

		if(bUsingXP1Scenario1)
		{
			const HandicapTypes eCurrentHandicap = GC.getGame().getHandicapType();
			HandicapTypes eEmporerHandicap = NO_HANDICAP;
			HandicapTypes eDeityHandicap = NO_HANDICAP;

			const int numHandicapInfos = GC.getNumHandicapInfos();
			for(int i = 0; i < numHandicapInfos; ++i)
			{
				const HandicapTypes eHandicap = static_cast<HandicapTypes>(i);
				CvHandicapInfo* pkInfo = GC.getHandicapInfo(eHandicap);
				if(pkInfo != NULL)
				{
					if(strcmp(pkInfo->GetType(), "HANDICAP_EMPEROR") == 0)
					{
						eEmporerHandicap = eHandicap;
					}
					else if(strcmp(pkInfo->GetType(), "HANDICAP_DEITY") == 0)
					{
						eDeityHandicap = eHandicap;
					}
				}
			}

			if(szCivKey && szNameKey)
			{
				if(strcmp(szCivKey, "CIVILIZATION_ENGLAND") == 0)
				{
					if(strcmp(szNameKey, "TXT_KEY_CITYSTATE_JERUSALEM") == 0)
					{
						if(eCurrentHandicap >= eEmporerHandicap)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP1_39);
						}
					}
				}
				else if(strcmp(szCivKey, "CIVILIZATION_OTTOMAN") == 0)
				{
					if(strcmp(szNameKey, "TXT_KEY_CITY_NAME_CONSTANTINOPLE") == 0)
					{
						if(eCurrentHandicap >= eDeityHandicap)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP1_40);
						}
					}
				}
			}	
		}

		if(bUsingXP1Scenario2)
		{
			bool bHasConstantinople = false;
			bool bHasRome = false;

			if(strcmp(szNameKey, "TXT_KEY_CITY_NAME_CONSTANTINOPLE") == 0)
			{
				bHasConstantinople = true;

				if(pNewCity->getOriginalOwner() != GetID())
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_XP1_47);
				}
			}
			else if(strcmp(szNameKey, "TXT_KEY_CITY_NAME_ROME") == 0)
			{
				bHasRome = true;
			}

			if(bHasConstantinople || bHasRome)
			{
				int iLoop = 0;
				for(CvCity* pCity = firstCity(&iLoop); pCity != NULL; pCity = nextCity(&iLoop))
				{
					const char* szOtherNameKey = pCity->getNameKey();
					if(strcmp(szOtherNameKey, "TXT_KEY_CITY_NAME_CONSTANTINOPLE") == 0)
					{
						bHasConstantinople = true;
					}
					else if(strcmp(szOtherNameKey, "TXT_KEY_CITY_NAME_ROME") == 0)
					{
						bHasRome = true;
					}
				}
			}

			if(bHasRome && bHasConstantinople)
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_XP1_48);
			}

			if(strcmp(getCivilizationTypeKey(), "CIVILIZATION_CELTS") == 0)
			{

				typedef std::pair<int,int> Location;
				typedef std::tr1::array<Location, 7> SassanidCityArray;
				SassanidCityArray SassanidCities = {
					Location(87,17),
					Location(85,20),
					Location(81,21),
					Location(79,24),
					Location(82,28),
					Location(81,33),
					Location(87,33),
				};
				
				int iNewPlotX = pNewCity->getX();
				int iNewPlotY = pNewCity->getY();


				for(SassanidCityArray::iterator it = SassanidCities.begin(); it != SassanidCities.end(); ++it)
				{
					if(it->first == iNewPlotX && it->second == iNewPlotY)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_51);

					}
				}
			}

		}
	}

	std::vector<BuildingTypes> freeConquestBuildings = m_pPlayerPolicies->GetFreeBuildingsOnConquest();
#ifdef AUI_WARNING_FIXES
	for (iI = 0; iI < freeConquestBuildings.size(); iI++)
#else
	for(iI = 0; iI < (int)freeConquestBuildings.size(); iI++)
#endif
	{
		BuildingTypes eLoopBuilding = freeConquestBuildings[iI];
		if (eLoopBuilding != NO_BUILDING)
		{
			CvBuildingEntry* pkLoopBuildingInfo = GC.getBuildingInfo(eLoopBuilding);
			if(pkLoopBuildingInfo)
			{
				if (eLoopBuilding == pkLoopBuildingInfo->GetID())
				{
					pNewCity->GetCityBuildings()->SetNumFreeBuilding(eLoopBuilding, 1);
				}
			}
		}
	}

	BuildingTypes eTraitFreeBuilding = GetPlayerTraits()->GetFreeBuildingOnConquest();
	for(iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		const BuildingTypes eLoopBuilding = static_cast<BuildingTypes>(iI);
		CvBuildingEntry* pkLoopBuildingInfo = GC.getBuildingInfo(eLoopBuilding);
		if(pkLoopBuildingInfo)
		{
			const CvBuildingClassInfo& kLoopBuildingClassInfo = pkLoopBuildingInfo->GetBuildingClassInfo();

			int iNum = 0;

			if(eTraitFreeBuilding == pkLoopBuildingInfo->GetID())
			{
				pNewCity->GetCityBuildings()->SetNumFreeBuilding(eTraitFreeBuilding, 1);
			}

			else if(paiNumRealBuilding[iI] > 0)
			{
				const BuildingClassTypes eBuildingClass = (BuildingClassTypes)pkLoopBuildingInfo->GetBuildingClassType();
				if(::isWorldWonderClass(kLoopBuildingClassInfo))
				{
					eBuilding = eLoopBuilding;
				}
				else
				{
					eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings(eBuildingClass);
				}

				if(eBuilding != NO_BUILDING)
				{
					CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
					if(pkBuildingInfo)
					{
						if(!pkLoopBuildingInfo->IsNeverCapture())
						{
							if(!isProductionMaxedBuildingClass(((BuildingClassTypes)(pkBuildingInfo->GetBuildingClassType())), true))
							{

#ifdef BUILDINGS_DESTROY_ONCE_PER_TURN
								if(!bConquest || bRecapture || !(iTurnsSinceAcquire > 0) || (GC.getGame().getJonRandNum(100, "Capture Probability") < pkLoopBuildingInfo->GetConquestProbability()))
#else
								
								if(!bConquest || bRecapture || (GC.getGame().getJonRandNum(100, "Capture Probability") < pkLoopBuildingInfo->GetConquestProbability()))
#endif
								{
									iNum += paiNumRealBuilding[iI];
								}
							}
						}


						if(bConquest && !GC.getGame().isGameMultiPlayer() && pkLoopBuildingInfo->GetType() && _stricmp(pkLoopBuildingInfo->GetType(), "BUILDING_BURIAL_TOMB") == 0 && isHuman())
						{
							if(iCaptureGold > 0)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_TOMBRAIDER);
							}
						}


						if(bConquest && !GC.getGame().isGameMultiPlayer() && pkLoopBuildingInfo->GetType() && _stricmp(pkLoopBuildingInfo->GetType(), "BUILDING_STATUE_ZEUS") == 0 && isHuman())
						{
							const char* pkCivKey = getCivilizationTypeKey();
							if(pkCivKey && strcmp(pkCivKey, "CIVILIZATION_ROME") == 0)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_ROME_GETS_ZEUS);
							}
						}

						pNewCity->GetCityBuildings()->SetNumRealBuildingTimed(eBuilding, iNum, false, ((PlayerTypes)(paiBuildingOriginalOwner[iI])), paiBuildingOriginalTime[iI]);

						if (iNum > 0)
						{
							if (pkBuildingInfo->GetGreatWorkCount() > 0)
							{
								for (unsigned int jJ=0; jJ < paGreatWorkData.size(); jJ++)
								{
#ifdef AUI_WARNING_FIXES
									if ((uint)paGreatWorkData[jJ].m_eBuildingType == iI)
#else
									if (paGreatWorkData[jJ].m_eBuildingType == iI)
#endif
									{
										pNewCity->GetCityBuildings()->SetBuildingGreatWork(eBuildingClass, paGreatWorkData[jJ].m_iSlot, paGreatWorkData[jJ].m_iGreatWork);
										paGreatWorkData[jJ].m_bTransferred = true;
										iCaptureGreatWorks++;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	for(std::vector<BuildingYieldChange>::iterator it = aBuildingYieldChange.begin(); it != aBuildingYieldChange.end(); ++it)
	{
		pNewCity->GetCityBuildings()->SetBuildingYieldChange((*it).eBuildingClass, (*it).eYield, (*it).iChange);
	}


	for (unsigned int jJ=0; jJ < paGreatWorkData.size(); jJ++)
	{
		if (!paGreatWorkData[jJ].m_bTransferred)
		{
			BuildingClassTypes eBuildingClass = NO_BUILDINGCLASS;
#ifdef AUI_WARNING_FIXES
			uint iSlot = MAX_UNSIGNED_INT;
#else
			int iSlot = -1;
#endif
			GreatWorkType eType = GC.getGame().GetGameCulture()->m_CurrentGreatWorks[paGreatWorkData[jJ].m_iGreatWork].m_eType;
			GreatWorkSlotType eGreatWorkSlot = CultureHelpers::GetGreatWorkSlot(eType);
			if (pNewCity->GetCityBuildings()->GetNextAvailableGreatWorkSlot(eGreatWorkSlot, &eBuildingClass, &iSlot))
			{
				pNewCity->GetCityBuildings()->SetBuildingGreatWork(eBuildingClass, iSlot, paGreatWorkData[jJ].m_iGreatWork);
				paGreatWorkData[jJ].m_bTransferred = true;
				iCaptureGreatWorks++;
			}
			else
			{
				BuildingClassTypes eGWBuildingClass;
#ifdef AUI_WARNING_FIXES
				uint iGWSlot;
#else
				int iGWSlot;
#endif
				CvCity *pGWCity = GetCulture()->GetClosestAvailableGreatWorkSlot(pCityPlot->getX(), pCityPlot->getY(), eGreatWorkSlot, &eGWBuildingClass, &iGWSlot);
				if (pGWCity)
				{
					pGWCity->GetCityBuildings()->SetBuildingGreatWork(eGWBuildingClass, iGWSlot, paGreatWorkData[jJ].m_iGreatWork);
					paGreatWorkData[jJ].m_bTransferred = true;
					iCaptureGreatWorks++;
				}
			}
		}
	}


	if(pCityPlot->getX() == GetOriginalCapitalX() && pCityPlot->getY() == GetOriginalCapitalY())
	{
		SetHasLostCapital(false, NO_PLAYER);

		const BuildingTypes eCapitalBuilding = (BuildingTypes)(getCivilizationInfo().getCivilizationBuildings(GC.getCAPITAL_BUILDINGCLASS()));
		if(eCapitalBuilding != NO_BUILDING)
		{
#ifdef AUI_PLAYER_FIX_VENICE_ONLY_BANS_SETTLERS_NOT_SETTLING
			CvCity* pOldCapital = getCapitalCity();
			if (pOldCapital != NULL)
			{
				pOldCapital->GetCityBuildings()->SetNumRealBuilding(eCapitalBuilding, 0);
#else
			if(getCapitalCity() != NULL)
			{
				getCapitalCity()->GetCityBuildings()->SetNumRealBuilding(eCapitalBuilding, 0);
#endif
			}
			CvAssertMsg(!(pNewCity->GetCityBuildings()->GetNumRealBuilding(eCapitalBuilding)), "(pBestCity->getNumRealBuilding(eCapitalBuilding)) did not return false as expected");
			pNewCity->GetCityBuildings()->SetNumRealBuilding(eCapitalBuilding, 1);
#ifdef AUI_PLAYER_FIX_VENICE_ONLY_BANS_SETTLERS_NOT_SETTLING
			if (GetPlayerTraits()->IsNoAnnexing() && pOldCapital && !pOldCapital->IsPuppet())
#ifdef AUI_CITIZENS_MID_TURN_ASSIGN_RUNS_SELF_CONSISTENCY
				pOldCapital->DoCreatePuppet(false);
#else
				pOldCapital->DoCreatePuppet();
#endif
#endif
		}
	}


	GC.getGame().DoTestConquestVictory();

	GC.getMap().updateWorkingCity(pCityPlot,NUM_CITY_RINGS*2);

	if(bConquest)
	{
#ifdef AUI_HEXSPACE_DX_LOOPS
		int iMaxDX, iDX;
		CvPlot* pLoopPlot;
		for (int iDY = -iMaxRange; iDY <= iMaxRange; iDY++)
		{
			iMaxDX = iMaxRange - MAX(0, iDY);
			for (iDX = -iMaxRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
			{

				pLoopPlot = plotXY(iOldCityX, iOldCityY, iDX, iDY);
#else
		for(int iDX = -iMaxRange; iDX <= iMaxRange; iDX++)
		{
			for(int iDY = -iMaxRange; iDY <= iMaxRange; iDY++)
			{
				CvPlot* pLoopPlot = plotXYWithRangeCheck(iOldCityX, iOldCityY, iDX, iDY, iMaxRange);
#endif
				if(pLoopPlot)
				{
					pLoopPlot->verifyUnitValidPlot();
				}
			}
		}


		if(isHuman() && !CvPreGame::isNetworkMultiplayerGame())
		{
			const char* pkLeaderKey = getLeaderTypeKey();
			if(pkLeaderKey && strcmp(pkLeaderKey, "LEADER_ASKIA") == 0)
			{
				CvCity* pkCaptialCity = getCapitalCity();
				if(pkCaptialCity != NULL)
				{
					CvPlot* pkCapitalPlot = pkCaptialCity->plot();
					CvPlot* pkNewCityPlot = pNewCity->plot();
					if(pkCapitalPlot && pkNewCityPlot)
					{

						CvArea* pkCapitalArea = pkCapitalPlot->area();
						CvArea* pkNewCityArea = pkNewCityPlot->area();

						if(pkCapitalArea && pkNewCityArea)
						{

#define ACHIEVEMENT_MIN_CONTINENT_SIZE	8
							if(pkNewCityArea->GetID() != pkCapitalArea->GetID() && pkNewCityArea->getNumTiles() >= ACHIEVEMENT_MIN_CONTINENT_SIZE)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_WARCANOE);
							}
						}
					}
				}
			}
		}
	}

	pCityPlot->setRevealed(GET_PLAYER(eOldOwner).getTeam(), true);


	if(GET_PLAYER(eOldOwner).getNumCities() == 0 && !GET_PLAYER(eOldOwner).GetPlayerTraits()->IsStaysAliveZeroCities() && !bIsMinorCivBuyout)
	{
		if(!isMinorCiv() && !isBarbarian())
		{
			for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
			{
				if(GetID() != iMajorLoop && GET_PLAYER((PlayerTypes) iMajorLoop).isAlive())
				{

					if(GET_TEAM(GET_PLAYER((PlayerTypes) iMajorLoop).getTeam()).isHasMet(getTeam()))
					{
						GET_PLAYER((PlayerTypes) iMajorLoop).GetDiplomacyAI()->DoPlayerKilledSomeone(GetID(), eOldOwner);
					}
				}
			}
		}
	}

	else
	{
		GET_PLAYER(eOldOwner).GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_MY_CITY_CAPTURED);
	}


	GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_ENEMY_CITY_CAPTURED);

	bool bDisbanded = false;


	if(GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		bDisbanded = true;
		disband(pNewCity);

		pNewCity = NULL;


		for(uint ui = 0; ui < aiPurchasedPlotX.size(); ui++)
		{
			CvPlot* pPlot = GC.getMap().plot(aiPurchasedPlotX[ui], aiPurchasedPlotY[ui]);
			pPlot->setOwner(NO_PLAYER, -1,                 true,                      true);
		}

	}
	else
	{

		for(uint ui = 0; ui < aiPurchasedPlotX.size(); ui++)
		{
			CvPlot* pPlot = GC.getMap().plot(aiPurchasedPlotX[ui], aiPurchasedPlotY[ui]);
			if(pPlot->getOwner() != pNewCity->getOwner())
				pPlot->setOwner(pNewCity->getOwner(),                    pNewCity->GetID(),                 true,                      true);
		}


		if(pNewCity->getOriginalOwner() != GetID())
		{
			pNewCity->SetOccupied(true);

			int iInfluenceReduction = GetCulture()->GetInfluenceCityConquestReduction(eOldOwner);

#ifdef NQ_DIABLE_RESISTANCE_TIME_VIA_POLICIES
			int iResistanceTurns = 0;
			if (!IsDisablesResistanceTime())
			{
				iResistanceTurns = pNewCity->getPopulation() * GC.getGame().getGameSpeedInfo().getVictoryDelayPercent();
				iResistanceTurns *= (100 - iInfluenceReduction);
				if (iResistanceTurns % 20000 != 0)
					iResistanceTurns += 20000;
				iResistanceTurns /= 20000;
			}
#else

			int iResistanceTurns = pNewCity->getPopulation() * GC.getGame().getGameSpeedInfo().getVictoryDelayPercent();
			iResistanceTurns *= (100 - iInfluenceReduction);
			if (iResistanceTurns % 20000 != 0)
				iResistanceTurns += 20000;
			iResistanceTurns /= 20000;

#endif

			if (iResistanceTurns > 0)
			{
				pNewCity->ChangeResistanceTurns(iResistanceTurns);
			}
		}

		long lResult = 0;

		if(lResult == 0)
		{
			PlayerTypes eLiberatedPlayer = NO_PLAYER;


			if(pNewCity->getOriginalOwner() != eOldOwner && pNewCity->getOriginalOwner() != GetID())
			{
				eLiberatedPlayer = pNewCity->getOriginalOwner();
				if(!CanLiberatePlayerCity(eLiberatedPlayer))
				{
					eLiberatedPlayer = NO_PLAYER;
				}
			}


			if(!isHuman())
			{
				AI_conquerCity(pNewCity, eOldOwner);

				CvCity* pkCurrentCity = pCityPlot->getPlotCity();
				if (pkCurrentCity == NULL || pNewCity != pkCurrentCity || pkCurrentCity->getOwner() != GetID())
				{

					pNewCity = NULL;
				}
			}


			else if(!GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
			{

				pNewCity->SetIgnoreCityForHappiness(true);
				if (GetPlayerTraits()->IsNoAnnexing() && bIsMinorCivBuyout)
				{
#ifdef AUI_CITIZENS_MID_TURN_ASSIGN_RUNS_SELF_CONSISTENCY
					pNewCity->DoCreatePuppet(false);
#else
					pNewCity->DoCreatePuppet();
#endif
				}
				else if (pNewCity->getOriginalOwner() != GetID() || GetPlayerTraits()->IsNoAnnexing() || bIsMinorCivBuyout)
				{
					if(GC.getGame().getActivePlayer() == GetID())
					{
						int iTemp[5] = { pNewCity->GetID(), iCaptureGold, iCaptureCulture, iCaptureGreatWorks, eLiberatedPlayer };
						bool bTemp[2] = { bIsMinorCivBuyout, bConquest };
						GC.GetEngineUserInterface()->AddPopup(BUTTONPOPUP_CITY_CAPTURED, POPUP_PARAM_INT_ARRAY(iTemp), POPUP_PARAM_BOOL_ARRAY(bTemp));

						CancelActivePlayerEndTurn();
					}
				}
				else
				{
					pNewCity->SetIgnoreCityForHappiness(false);
				}
			}


			else if(iCaptureGold > 0 || iCaptureCulture > 0 || iCaptureGreatWorks > 0)
			{
				if (iCaptureCulture == 0 && iCaptureGreatWorks == 0)
				{
					strBuffer = GetLocalizedText("TXT_KEY_POPUP_GOLD_CITY_CAPTURE", iCaptureGold, pNewCity->getNameKey());
					GC.GetEngineUserInterface()->AddCityMessage(0, pNewCity->GetIDInfo(), GetID(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer);
				}
				else
				{
					strBuffer = GetLocalizedText("TXT_KEY_POPUP_GOLD_AND_CULTURE_CITY_CAPTURE", iCaptureGold, iCaptureCulture, iCaptureGreatWorks, pNewCity->getNameKey());
					GC.GetEngineUserInterface()->AddCityMessage(0, pNewCity->GetIDInfo(), GetID(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer);

				}
			}
		}
	}


	CheckForMurder(eOldOwner);

#ifdef AUI_CITY_FIX_PUPPET_WORKED_PLOT_OVERRIDE
	GC.getMap().updateWorkingCity(pCityPlot, NUM_CITY_RINGS * 2);
#endif

	if(GC.getGame().getActiveTeam() == GET_PLAYER(eOldOwner).getTeam())
	{
		CvMap& theMap = GC.getMap();
		theMap.updateDeferredFog();
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem && pNewCity != NULL)
	{
		CvLuaArgsHandle args;
		args->Push(eOldOwner);
		args->Push(bCapital);
		args->Push(pNewCity->getX());
		args->Push(pNewCity->getY());
		args->Push(GetID());
		args->Push(iOldPopulation);
		args->Push(bConquest);
		args->Push((int)paGreatWorkData.size());
		args->Push(iCaptureGreatWorks);

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "CityCaptureComplete", args.get(), bResult);
	}

#ifdef AUI_CITIZENS_MID_TURN_ASSIGN_RUNS_SELF_CONSISTENCY
	doSelfConsistencyCheckAllCities();
	GET_PLAYER(eOldOwner).doSelfConsistencyCheckAllCities();
#endif

#if defined(LEKMOD_CITY_YIELDS_TRAITS) && defined(LEKMOD_TRACK_CITY_SETTLER_UNITTYPE) && defined(LEKMOD_YIELD_SETTLE_UNIT_NON_CAP_MAX) && (LEKMOD_YIELD_SETTLE_UNIT_NON_CAP_MAX > 0)
	{
		const bool bYieldSettleKeyed =
			(GetPlayerTraits()->GetYieldSettleUnit() != NO_UNIT) ||
			(eOldOwner != NO_PLAYER && GET_PLAYER(eOldOwner).isAlive() && GET_PLAYER(eOldOwner).GetPlayerTraits()->GetYieldSettleUnit() != NO_UNIT);
		if (bYieldSettleKeyed)
		{
			updateYield();
			if (eOldOwner != NO_PLAYER && GET_PLAYER(eOldOwner).isAlive())
				GET_PLAYER(eOldOwner).updateYield();
			GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
#endif

#ifdef _MSC_VER
#pragma warning ( pop )
#endif
}



void CvPlayer::killCities()
{
	CvCity* pLoopCity;
	int iLoop;

	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->kill();
	}
}

const int RESERVE_TOP_X_NAMES = 5;


CvString CvPlayer::getNewCityName() const
{
	const CLLNode<CvString>* pNode;
	CvString strName;

	for(pNode = headCityNameNode(); (pNode != NULL); pNode = nextCityNameNode(pNode))
	{
		strName = pNode->m_data;
		if(isCityNameValid(strName, true))
		{
			strName = pNode->m_data;
			break;
		}
	}

	if(strName.IsEmpty())
	{
		getCivilizationCityName(strName, getCivilizationType());
	}

	if(strName.IsEmpty())
	{

		int iPlayersAlive = 0;
		for(int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			const PlayerTypes ePlayer = static_cast<PlayerTypes>(iI);
			CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
			if(ePlayer != GetID() && kPlayer.isAlive() && !kPlayer.isMinorCiv() && !kPlayer.isBarbarian())
			{
				iPlayersAlive++;
			}
		}

		int iChosenPlayer = GC.getGame().getJonRandNum(iPlayersAlive, "Random Player To Steal City Name");

		int iPlayersFound = 0;
		for(int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			const PlayerTypes ePlayer = static_cast<PlayerTypes>(iI);
			CvPlayerAI &kPlayer = GET_PLAYER(ePlayer);
			if(ePlayer != GetID() && kPlayer.isAlive() && !kPlayer.isMinorCiv() && !kPlayer.isBarbarian())
			{
				if(iPlayersFound == iChosenPlayer)
				{
					strName = GetBorrowedCityName(kPlayer.getCivilizationType());			
					break;
				}
				else
				{
					iPlayersFound++;
				}
			}
		}
	}

	if(strName.IsEmpty())
	{

		int iCivsInDB = 0;
#ifdef AUI_WARNING_FIXES
		for (uint iI = 0; iI < GC.getNumCivilizationInfos(); iI++)
#else
		for(int iI = 0; iI < GC.getNumCivilizationInfos(); iI++)
#endif
		{
			const CivilizationTypes eCiv = static_cast<CivilizationTypes>(iI);

			CvCivilizationInfo* pkCivilizationInfo = GC.getCivilizationInfo(eCiv);
			if(pkCivilizationInfo != NULL && pkCivilizationInfo->getNumCityNames() > RESERVE_TOP_X_NAMES)
			{
				iCivsInDB++;
			}
		}

		int iChosenCiv = GC.getGame().getJonRandNum(iCivsInDB, "Random Civ To Steal City Name");

		int iCivsFound = 0;
#ifdef AUI_WARNING_FIXES
		for (uint iI = 0; iI < GC.getNumCivilizationInfos(); iI++)
#else
		for(int iI = 0; iI < GC.getNumCivilizationInfos(); iI++)
#endif
		{
			const CivilizationTypes eCiv = static_cast<CivilizationTypes>(iI);

			CvCivilizationInfo* pkCivilizationInfo = GC.getCivilizationInfo(eCiv);
			if (pkCivilizationInfo != NULL && pkCivilizationInfo->getNumCityNames() > RESERVE_TOP_X_NAMES)
			{
				if (iCivsFound == iChosenCiv)
				{
					strName = GetBorrowedCityName(eCiv);
					break;
				}
				else
				{
					iCivsFound++;
				}
			}
		}
	}

	if(strName.IsEmpty())
	{
		strName = "TXT_KEY_CITY";
	}

	return strName;
}


CvString CvPlayer::GetBorrowedCityName(CivilizationTypes eCivToBorrowFrom) const
{
	CvString szRtnValue;
	CvCivilizationInfo *pCivInfo = GC.getCivilizationInfo(eCivToBorrowFrom);

	if (pCivInfo)
	{
		int iRange = pCivInfo->getNumCityNames() - RESERVE_TOP_X_NAMES;
		int iRandOffset = GC.getGame().getJonRandNum(iRange, "Random City Name To Steal");
		for(int iI = 0; iI < iRange; iI++)     
		{
			CvString strCityName = pCivInfo->getCityNames(RESERVE_TOP_X_NAMES + ((iI + iRandOffset) % iRange));
			szRtnValue = GetLocalizedText(strCityName.c_str());

			if(isCityNameValid(szRtnValue, true))
			{
				break;
			}
		}
	}

	return szRtnValue;

}


void CvPlayer::getCivilizationCityName(CvString& szBuffer, CivilizationTypes eCivilization) const
{
	int iRandOffset;
	int iLoopName;

	CvCivilizationInfo* pkCivilizationInfo = GC.getCivilizationInfo(eCivilization);
	if(pkCivilizationInfo == NULL)
	{

		return;
	}

	if(isBarbarian())
	{
		iRandOffset = GC.getGame().getJonRandNum(pkCivilizationInfo->getNumCityNames(), "Random Barb Name");
	}
	else
	{
		iRandOffset = 0;
	}


	if(isMinorCiv())
	{
		CvMinorCivInfo* pkMinorCivInfo = GC.getMinorCivInfo(GetMinorCivAI()->GetMinorCivType());
		if(pkMinorCivInfo)
		{
			CvMinorCivInfo& kMinorCivInfo = *pkMinorCivInfo;
			for(int iI = 0; iI < kMinorCivInfo.getNumCityNames(); iI++)
			{
				iLoopName = ((iI + iRandOffset) % kMinorCivInfo.getNumCityNames());

				const CvString strCityName = kMinorCivInfo.getCityNames(iLoopName);
				CvString strName = GetLocalizedText(strCityName.c_str());

				if(isCityNameValid(strName, true))
				{
					szBuffer = strCityName;
					break;
				}
			}
		}
	}
	else
	{
		CvCivilizationInfo& kCivInfo = *pkCivilizationInfo;
		for(int iI = 0; iI < kCivInfo.getNumCityNames(); iI++)
		{
			iLoopName = ((iI + iRandOffset) % kCivInfo.getNumCityNames());

			const CvString strCityName = kCivInfo.getCityNames(iLoopName);
			CvString strName = GetLocalizedText(strCityName.c_str());

			if(isCityNameValid(strName, true))
			{
				szBuffer = strCityName;
				break;
			}
		}
	}
}



bool CvPlayer::isCityNameValid(CvString& szName, bool bTestDestroyed) const
{
	const CvCity* pLoopCity;
	int iLoop;

	if(bTestDestroyed)
	{
		if(GC.getGame().isDestroyedCityName(szName))
		{
			return false;
		}

		for(int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
			for(pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kLoopPlayer.nextCity(&iLoop))
			{
				if(pLoopCity->getName() == szName)
				{
					return false;
				}
			}
		}
	}
	else
	{
		for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			if(pLoopCity->getName() == szName)
			{
				return false;
			}
		}
	}

	return true;
}



void CvPlayer::DoLiberatePlayer(PlayerTypes ePlayer, int iOldCityID)
{
	CvCity* pCity = getCity(iOldCityID);
	CvAssert(pCity);
	if (!pCity)
		return;

	PlayerTypes eOldOwner = pCity->getOwner();
	CvPlot* pPlot = pCity->plot();


	TeamTypes eTeam = getTeam();
	TeamTypes eLiberatedTeam = GET_PLAYER(ePlayer).getTeam();


	TeamTypes eConquerorTeam = GET_TEAM(eLiberatedTeam).GetKilledByTeam();

	if (!GET_PLAYER(ePlayer).isAlive())
	{
		GET_PLAYER(ePlayer).setBeingResurrected(true);
		GET_TEAM(eLiberatedTeam).SetLiberatedByTeam(eTeam);


		for(int iOtherTeamLoop = 0; iOtherTeamLoop < MAX_CIV_TEAMS; iOtherTeamLoop++)
		{
			if(eLiberatedTeam != iOtherTeamLoop)
			{
				GET_TEAM(eLiberatedTeam).makePeace((TeamTypes) iOtherTeamLoop,               false,                          true);
			}
		}
	
		if (!GET_PLAYER(ePlayer).isMinorCiv())
		{

			Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_CIV_RESURRECTED");
			strMessage << getCivilizationShortDescriptionKey();
			strMessage << pCity->getNameKey();
			strMessage << GET_PLAYER(ePlayer).getCivilizationAdjectiveKey();
			strMessage << GET_PLAYER(ePlayer).getCivilizationDescriptionKey();
			Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CIV_RESURRECTED_SHORT");
			if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(ePlayer).isHuman())
			{
				strSummary << GET_PLAYER(ePlayer).getNickName();
			}
			else
			{
				strSummary << GET_PLAYER(ePlayer).getNameKey();
			}
			if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(m_eID).isHuman())
			{
				strSummary << GET_PLAYER(m_eID).getNickName();
			}
			else
			{
				strSummary << GET_PLAYER(m_eID).getNameKey();
			}			

			for(int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				const PlayerTypes eOtherPlayer = static_cast<PlayerTypes>(iI);
				CvPlayerAI& kOtherPlayer = GET_PLAYER(eOtherPlayer);
				if(kOtherPlayer.isAlive() && kOtherPlayer.GetNotifications() && iI != m_eID)
				{
					kOtherPlayer.GetNotifications()->Add(NOTIFICATION_RESURRECTED_MAJOR_CIV, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1);
				}
			}

			CvString temp = strMessage.toUTF8();
			GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, m_eID, temp);
		}
	}
	else
	{
		GET_PLAYER(ePlayer).GetDiplomacyAI()->ChangeNumCitiesLiberated(m_eID, 1);

		if (!GET_PLAYER(ePlayer).isMinorCiv())
		{

			Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_LIBERATED");
			if(GC.getGame().isGameMultiPlayer() && isHuman())
			{
				strMessage << getNickName();
			}
			else
			{
				strMessage << getNameKey();
			}
			strMessage << pCity->getNameKey();
			strMessage << GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey();
			Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CIV_LIBERATED_SHORT");
			strSummary << pCity->getNameKey();
			if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(ePlayer).isHuman())
			{
				strSummary << GET_PLAYER(ePlayer).getNickName();
			}
			else
			{
				strSummary << GET_PLAYER(ePlayer).getNameKey();
			}

			for(int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				const PlayerTypes eOtherPlayer = static_cast<PlayerTypes>(iI);
				CvPlayerAI& kOtherPlayer = GET_PLAYER(eOtherPlayer);
				if(kOtherPlayer.isAlive() && kOtherPlayer.GetNotifications() && iI != m_eID)
				{
					kOtherPlayer.GetNotifications()->Add(NOTIFICATION_LIBERATED_MAJOR_CITY, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1);
				}
			}
		}
	}


	GET_PLAYER(ePlayer).acquireCity(pCity, false, true);

	if (!GET_PLAYER(ePlayer).isMinorCiv())
	{

		if (!GET_PLAYER(ePlayer).isAlive())
		{
			CvDiplomacyAI* pDiploAI = GET_PLAYER(ePlayer).GetDiplomacyAI();
			PlayerTypes eMePlayer = GetID();
			pDiploAI->SetResurrectedBy(eMePlayer, true);
			pDiploAI->SetLandDisputeLevel(eMePlayer, DISPUTE_LEVEL_NONE);
			pDiploAI->SetWonderDisputeLevel(eMePlayer, DISPUTE_LEVEL_NONE);
			pDiploAI->SetMinorCivDisputeLevel(eMePlayer, DISPUTE_LEVEL_NONE);
			pDiploAI->SetWarmongerThreat(eMePlayer, THREAT_NONE);

			pDiploAI->SetPlayerNoSettleRequestCounter(eMePlayer, -1);
			pDiploAI->SetPlayerStopSpyingRequestCounter(eMePlayer, -1);
			pDiploAI->SetDemandCounter(eMePlayer, -1);
			pDiploAI->ChangeNumTimesCultureBombed(eMePlayer, -pDiploAI->GetNumTimesCultureBombed(eMePlayer));
			pDiploAI->ChangeNegativeReligiousConversionPoints(eMePlayer, -pDiploAI->GetNegativeReligiousConversionPoints(eMePlayer));
			pDiploAI->ChangeNegativeArchaeologyPoints(eMePlayer, -pDiploAI->GetNegativeArchaeologyPoints(eMePlayer));

			pDiploAI->ChangeNumTimesRobbedBy(eMePlayer, -pDiploAI->GetNumTimesRobbedBy(eMePlayer));
			pDiploAI->SetPlayerBrokenMilitaryPromise(eMePlayer, false);
			pDiploAI->SetPlayerIgnoredMilitaryPromise(eMePlayer, false);
			pDiploAI->SetBrokenBorderPromiseValue(eMePlayer, 0);
			pDiploAI->SetIgnoredBorderPromiseValue(eMePlayer, 0);
			pDiploAI->SetBrokenExpansionPromiseValue(eMePlayer, 0);
			pDiploAI->SetIgnoredExpansionPromiseValue(eMePlayer, 0);

			pDiploAI->SetPlayerBrokenAttackCityStatePromise(eMePlayer, false);
			pDiploAI->SetPlayerIgnoredAttackCityStatePromise(eMePlayer, false);
			pDiploAI->SetPlayerBrokenBullyCityStatePromise(eMePlayer, false);
			pDiploAI->SetPlayerIgnoredBullyCityStatePromise(eMePlayer, false);

			pDiploAI->SetPlayerBrokenNoConvertPromise(eMePlayer, false);
			pDiploAI->SetPlayerIgnoredNoConvertPromise(eMePlayer, false);

			pDiploAI->SetPlayerBrokenNoDiggingPromise(eMePlayer, false);
			pDiploAI->SetPlayerIgnoredNoDiggingPromise(eMePlayer, false);

			pDiploAI->SetPlayerBrokenSpyPromise(eMePlayer, false);
			pDiploAI->SetPlayerIgnoredSpyPromise(eMePlayer, false);

			pDiploAI->SetPlayerBrokenCoopWarPromise(eMePlayer, false);

			pDiploAI->SetOtherPlayerNumProtectedMinorsKilled(eMePlayer, 0);
			pDiploAI->SetOtherPlayerNumProtectedMinorsAttacked(eMePlayer, 0);
			pDiploAI->SetOtherPlayerNumProtectedMinorsBullied(eMePlayer, 0);
			pDiploAI->SetOtherPlayerTurnsSinceSidedWithProtectedMinor(eMePlayer, -1);

			pDiploAI->SetFriendDenouncedUs(eMePlayer, false);
			pDiploAI->SetDenouncedPlayer(eMePlayer, false);
			GetDiplomacyAI()->SetDenouncedPlayer(ePlayer, false);
			pDiploAI->SetFriendDeclaredWarOnUs(eMePlayer, false);

			pDiploAI->ChangeNumTimesNuked(eMePlayer, -pDiploAI->GetNumTimesNuked(eMePlayer));
		}
	}


	GET_PLAYER(ePlayer).verifyAlive();
	GET_PLAYER(ePlayer).setBeingResurrected(false);


	if(GET_PLAYER(ePlayer).isMinorCiv())
	{
		GET_PLAYER(ePlayer).GetMinorCivAI()->DoLiberationByMajor(eOldOwner, eConquerorTeam);
	}



	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		PlayerTypes eMajor = (PlayerTypes)iMajorLoop;
		if(GetID() != eMajor && GET_PLAYER(eMajor).isAlive())
		{

			if(GET_TEAM(GET_PLAYER(eMajor).getTeam()).isHasMet(getTeam()))
			{
				int iNumCities = max(GET_PLAYER(ePlayer).getNumCities(), 1);
				int iWarmongerOffset = CvDiplomacyAIHelpers::GetWarmongerOffset(iNumCities, GET_PLAYER(ePlayer).isMinorCiv());
				GET_PLAYER(eMajor).GetDiplomacyAI()->ChangeOtherPlayerWarmongerAmount(GetID(), -iWarmongerOffset);
			}
		}
	}


#ifdef NQ_NEVER_PUSH_OUT_OF_MINORS_ON_PEACE
	if(pPlot && pPlot->getNumUnits() > 0 && !GET_PLAYER(ePlayer).isMinorCiv())
#else
	if(pPlot->getNumUnits() > 0)
#endif
	{

		IDInfoVector currentUnits;
		if (pPlot->getUnits(&currentUnits) > 0)
		{
			for(IDInfoVector::const_iterator itr = currentUnits.begin(); itr != currentUnits.end(); ++itr)
			{
				CvUnit* pLoopUnit = (CvUnit*)GetPlayerUnit(*itr);

				if(pLoopUnit && pLoopUnit->getOwner() == eOldOwner)
				{
					pLoopUnit->finishMoves();
					if (!pLoopUnit->jumpToNearestValidPlot())
						pLoopUnit->kill(false);
				}
			}
		}
	}
}


bool CvPlayer::CanLiberatePlayer(PlayerTypes ePlayer)
{

	if(GET_PLAYER(ePlayer).isAlive())
	{
		return false;
	}

	if(GET_PLAYER(ePlayer).IsEverConqueredBy(m_eID))
	{
		return false;
	}

	if(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetKilledByTeam() == getTeam())
	{
		return false;
	}

	return true;
}

#ifdef NEW_CITIES_LIBERATION

bool CvPlayer::CanLiberatePlayerCity(PlayerTypes ePlayer)
{
	if (GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_LIBERATION"))
	{
		if (GET_PLAYER(ePlayer).isHuman())
		{
			return true;
		}
		if (GET_PLAYER(ePlayer).isMinorCiv())
		{
			return CanLiberatePlayer(ePlayer);
		}

		return false;
	}
	else
	{
		if (!GET_PLAYER(ePlayer).isAlive())
		{
			return CanLiberatePlayer(ePlayer);
		}

		return true;
	}
}

#else

bool CvPlayer::CanLiberatePlayerCity(PlayerTypes ePlayer)
{
	if (!GET_PLAYER(ePlayer).isAlive())
	{
		return CanLiberatePlayer(ePlayer);
	}

	return true;
}
#endif


#ifdef AUI_UNIT_FIX_GIFTED_UNITS_ARE_GIFTED_NOT_CLONED
CvUnit* CvPlayer::initUnit(UnitTypes eUnit, int iX, int iY, UnitAITypes eUnitAI, DirectionTypes eFacingDirection, bool bNoMove, bool bSetupGraphical, int iMapLayer          , int iNumGoodyHutsPopped, bool bIsGifted)
#else
CvUnit* CvPlayer::initUnit(UnitTypes eUnit, int iX, int iY, UnitAITypes eUnitAI, DirectionTypes eFacingDirection, bool bNoMove, bool bSetupGraphical, int iMapLayer          , int iNumGoodyHutsPopped)
#endif
{
	CvAssertMsg(eUnit != NO_UNIT, "Unit is not assigned a valid value");
	if (eUnit == NO_UNIT)
		return NULL;

	CvUnitEntry* pkUnitDef = GC.getUnitInfo(eUnit);
	CvAssertFmt(pkUnitDef != NULL, "Trying to create unit of type %d, which does not exist", eUnit);
	if (pkUnitDef == NULL)
		return NULL;

	CvUnit* pUnit = addUnit();
	CvAssertMsg(pUnit != NULL, "Unit is not assigned a valid value");
	if(NULL != pUnit)
	{
#ifdef AUI_UNIT_FIX_GIFTED_UNITS_ARE_GIFTED_NOT_CLONED
		pUnit->init(pUnit->GetID(), eUnit, ((eUnitAI == NO_UNITAI) ? ((UnitAITypes)(pkUnitDef->GetDefaultUnitAIType())) : eUnitAI), GetID(), iX, iY, eFacingDirection, bNoMove, bSetupGraphical, iMapLayer, iNumGoodyHutsPopped, bIsGifted);
#else
		pUnit->init(pUnit->GetID(), eUnit, ((eUnitAI == NO_UNITAI) ? ((UnitAITypes)(pkUnitDef->GetDefaultUnitAIType())) : eUnitAI), GetID(), iX, iY, eFacingDirection, bNoMove, bSetupGraphical, iMapLayer, iNumGoodyHutsPopped);
#endif


		if(pUnit->getUnitInfo().GetWorkRate() > 0 && pUnit->getUnitInfo().GetDomainType() == DOMAIN_LAND)
		{
			m_bEverTrainedBuilder = true;
		}

	}

	m_kPlayerAchievements.AddUnit(pUnit);

	return pUnit;
}

#ifdef AUI_UNIT_FIX_GIFTED_UNITS_ARE_GIFTED_NOT_CLONED
CvUnit* CvPlayer::initUnitWithNameOffset(UnitTypes eUnit, int nameOffset, int iX, int iY, UnitAITypes eUnitAI, DirectionTypes eFacingDirection, bool bNoMove, bool bSetupGraphical, int iMapLayer          , int iNumGoodyHutsPopped, bool bIsGifted)
#else
CvUnit* CvPlayer::initUnitWithNameOffset(UnitTypes eUnit, int nameOffset, int iX, int iY, UnitAITypes eUnitAI, DirectionTypes eFacingDirection, bool bNoMove, bool bSetupGraphical, int iMapLayer          , int iNumGoodyHutsPopped)
#endif
{
	CvAssertMsg(eUnit != NO_UNIT, "Unit is not assigned a valid value");
	if (eUnit == NO_UNIT)
		return NULL;

	CvUnitEntry* pkUnitDef = GC.getUnitInfo(eUnit);
	CvAssertFmt(pkUnitDef != NULL, "Trying to create unit of type %d, which does not exist", eUnit);
	if (pkUnitDef == NULL)
		return NULL;

	CvUnit* pUnit = addUnit();
	CvAssertMsg(pUnit != NULL, "Unit is not assigned a valid value");
	if(NULL != pUnit)
	{
#ifdef AUI_UNIT_FIX_GIFTED_UNITS_ARE_GIFTED_NOT_CLONED
		pUnit->initWithNameOffset(pUnit->GetID(), eUnit, nameOffset, ((eUnitAI == NO_UNITAI) ? ((UnitAITypes)(pkUnitDef->GetDefaultUnitAIType())) : eUnitAI), GetID(), iX, iY, eFacingDirection, bNoMove, bSetupGraphical, iMapLayer, iNumGoodyHutsPopped, bIsGifted);
#else
		pUnit->initWithNameOffset(pUnit->GetID(), eUnit, nameOffset, ((eUnitAI == NO_UNITAI) ? ((UnitAITypes)(pkUnitDef->GetDefaultUnitAIType())) : eUnitAI), GetID(), iX, iY, eFacingDirection, bNoMove, bSetupGraphical, iMapLayer, iNumGoodyHutsPopped);
#endif


		if(pUnit->getUnitInfo().GetWorkRate() > 0 && pUnit->getUnitInfo().GetDomainType() == DOMAIN_LAND)
		{
			m_bEverTrainedBuilder = true;
		}

	}

	m_kPlayerAchievements.AddUnit(pUnit);

	return pUnit;
}


void CvPlayer::disbandUnit(bool)
{
	CvUnit* pLoopUnit;
	CvUnit* pBestUnit;
	char szBuffer[1024];
	const size_t lenBuffer = 1024;
	int iValue;
	int iBestValue;
	int iLoop;

	iBestValue = INT_MAX;
	pBestUnit = NULL;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if(!(pLoopUnit->hasCargo()))
		{
			if(!(pLoopUnit->isGoldenAge()))
			{
				if(pLoopUnit->getUnitInfo().GetProductionCost() > 0)
				{
					{
						iValue = (10000 + GC.getGame().getJonRandNum(1000, "Disband Unit"));

						iValue += (pLoopUnit->getUnitInfo().GetProductionCost() * 5);

						iValue += (pLoopUnit->getExperience() * 20);
						iValue += (pLoopUnit->getLevel() * 100);

						if(pLoopUnit->IsCanDefend() && pLoopUnit->plot()->isCity())
						{
							iValue *= 2;
						}

						if(pLoopUnit->plot()->getTeam() == pLoopUnit->getTeam())
						{
							iValue *= 3;
						}

						switch(pLoopUnit->AI_getUnitAIType())
						{
						case UNITAI_UNKNOWN:
							break;

						case UNITAI_SETTLE:
							iValue *= 20;
							break;

						case UNITAI_WORKER:
							iValue *= 10;
							break;

						case UNITAI_ATTACK:
						case UNITAI_CITY_BOMBARD:
						case UNITAI_FAST_ATTACK:
						case UNITAI_DEFENSE:
						case UNITAI_COUNTER:
							iValue *= 2;
							break;

						case UNITAI_RANGED:
						case UNITAI_CITY_SPECIAL:
						case UNITAI_PARADROP:
							iValue *= 6;
							break;

						case UNITAI_EXPLORE:
							iValue *= 15;
							break;

						case UNITAI_ARTIST:
						case UNITAI_SCIENTIST:
						case UNITAI_GENERAL:
						case UNITAI_MERCHANT:
						case UNITAI_ENGINEER:
						case UNITAI_SPACESHIP_PART:
						case UNITAI_TREASURE:
						case UNITAI_PROPHET:
						case UNITAI_MISSIONARY:
						case UNITAI_INQUISITOR:
						case UNITAI_ADMIRAL:
						case UNITAI_WRITER:
						case UNITAI_MUSICIAN:
							break;

						case UNITAI_ICBM:
							iValue *= 4;
							break;

						case UNITAI_WORKER_SEA:
							iValue *= 18;
							break;

						case UNITAI_ATTACK_SEA:
						case UNITAI_RESERVE_SEA:
						case UNITAI_ESCORT_SEA:
							break;

						case UNITAI_EXPLORE_SEA:
							iValue *= 25;
							break;

						case UNITAI_ASSAULT_SEA:
						case UNITAI_SETTLER_SEA:
						case UNITAI_CARRIER_SEA:
						case UNITAI_MISSILE_CARRIER_SEA:
							iValue *= 5;
							break;

						case UNITAI_PIRATE_SEA:
						case UNITAI_ATTACK_AIR:
							break;

						case UNITAI_DEFENSE_AIR:
						case UNITAI_CARRIER_AIR:
						case UNITAI_MISSILE_AIR:
							iValue *= 3;
							break;

						default:
							CvAssert(false);
							break;
						}

						if(pLoopUnit->getUnitInfo().GetExtraMaintenanceCost() > 0)
						{
							iValue /= (pLoopUnit->getUnitInfo().GetExtraMaintenanceCost() + 1);
						}

						if(iValue < iBestValue)
						{
							iBestValue = iValue;
							pBestUnit = pLoopUnit;
						}
					}
				}
			}
		}
	}

	if(pBestUnit != NULL)
	{
		if(GetID() == GC.getGame().getActivePlayer())
		{
			sprintf_s(szBuffer, lenBuffer, GetLocalizedText("TXT_KEY_MISC_UNIT_DISBANDED_NO_MONEY", pBestUnit->getNameKey()).GetCString());
			GC.GetEngineUserInterface()->AddUnitMessage(0, pBestUnit->GetIDInfo(), GetID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer);
		}

		CvAssert(!(pBestUnit->isGoldenAge()));

		pBestUnit->kill(false);
	}
}



void CvPlayer::killUnits()
{
	CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		pLoopUnit->kill(false);
	}
}


CvPlot *CvPlayer::GetGreatAdmiralSpawnPlot (CvUnit *pUnit)
{
	CvPlot *pInitialPlot = pUnit->plot();


	CvCity *pInitialCity = pInitialPlot->getPlotCity();
	if (pInitialCity && pInitialCity->isCoastal(GC.getLAKE_MAX_AREA_SIZE()))
	{

		if (pInitialPlot->getNumFriendlyUnitsOfType(pUnit) <= GC.getPLOT_UNIT_LIMIT())
		{
			return pInitialPlot;
		}
	}


	CvCity *pLoopCity;
	int iLoop;
	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity != pInitialCity)
		{
			if (pLoopCity->isCoastal(GC.getLAKE_MAX_AREA_SIZE()))
			{
				if (pLoopCity->plot()->getNumFriendlyUnitsOfType(pUnit) < GC.getPLOT_UNIT_LIMIT())
				{
					return pLoopCity->plot();
				}
			}
		}
	}


	int iCapitalX;
	int iCapitalY;
	CvCity *pCapital = getCapitalCity();
	if (pCapital)
	{
		iCapitalX = pCapital->getX();
		iCapitalY = pCapital->getY();

		CvPlot *pBestPlot = NULL;
		int iBestDistance = MAX_INT;

#ifdef AUI_WARNING_FIXES
		for (uint iI = 0; iI < GC.getMap().numPlots(); iI++)
#else
		for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
#endif
		{
			CvPlot *pPlot = GC.getMap().plotByIndexUnchecked(iI);
			if (pPlot != NULL)
			{
				if (pPlot->isWater() && !pPlot->isLake())
				{
					if (pPlot->IsFriendlyTerritory(GetID()) || !pPlot->isOwned())
					{
						if (pPlot->getNumFriendlyUnitsOfType(pUnit) < GC.getPLOT_UNIT_LIMIT())
						{
							int iDistance = plotDistance(iCapitalX, iCapitalY, pPlot->getX(), pPlot->getY());
							if (iDistance < iBestDistance)
							{
								pBestPlot = pPlot;
								iBestDistance = iDistance;
							}
						}
					}
				}
			}
		}

		if (pBestPlot)
		{
			return pBestPlot;
		}


#ifdef AUI_WARNING_FIXES
		for (uint iI = 0; iI < GC.getMap().numPlots(); iI++)
#else
		for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
#endif
		{
			CvPlot *pPlot = GC.getMap().plotByIndexUnchecked(iI);
			if (pPlot != NULL)
			{
				if (pPlot->isWater())
				{
					if (pPlot->IsFriendlyTerritory(GetID()) || !pPlot->isOwned())
					{
						if (pPlot->getNumFriendlyUnitsOfType(pUnit) < GC.getPLOT_UNIT_LIMIT())
						{
							int iDistance = plotDistance(iCapitalX, iCapitalY, pPlot->getX(), pPlot->getY());
							if (iDistance < iBestDistance)
							{
								pBestPlot = pPlot;
								iBestDistance = iDistance;
							}
						}
					}
				}
			}
		}
		if (pBestPlot)
		{
			return pBestPlot;
		}
	}

	CvAssertMsg (false, "Could not find valid plot for Great Admiral - placing on land");

	return pInitialPlot;
}




int CvPlayer::GetNumBuilders() const
{
	return m_iNumBuilders;
}



void CvPlayer::SetNumBuilders(int iNum)
{
	if(GetNumBuilders() != iNum)
	{
		m_iNumBuilders = iNum;
	}
}



void CvPlayer::ChangeNumBuilders(int iChange)
{
	if(iChange != 0)
	{
		SetNumBuilders(GetNumBuilders() + iChange);
	}
}




int CvPlayer::GetMaxNumBuilders() const
{
	return m_iMaxNumBuilders;
}



void CvPlayer::SetMaxNumBuilders(int iNum)
{
	if(GetMaxNumBuilders() != iNum)
	{
		m_iMaxNumBuilders = iNum;
	}
}



void CvPlayer::ChangeMaxNumBuilders(int iChange)
{
	if(iChange != 0)
	{
		SetMaxNumBuilders(GetMaxNumBuilders() + iChange);
	}
}




int CvPlayer::GetNumUnitsWithUnitAI(UnitAITypes eUnitAIType, bool bIncludeBeingTrained, bool bIncludeWater)
{
	int iNumUnits = 0;

	CvUnit* pLoopUnit;
	CvCity* pLoopCity;
	int iLoop;


	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{

		if(pLoopUnit->getDomainType() != DOMAIN_SEA || bIncludeWater)
		{
			if(pLoopUnit->AI_getUnitAIType() == eUnitAIType)
			{
				iNumUnits++;
			}
		}
	}


	if(bIncludeBeingTrained)
	{
		for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			if(pLoopCity->isProductionUnit())
			{
				CvUnitEntry* pkUnitEntry = GC.getUnitInfo(pLoopCity->getProductionUnit());
				if(pkUnitEntry)
				{

					if(pkUnitEntry->GetDomainType() != DOMAIN_SEA || bIncludeWater)
					{
						if(pkUnitEntry->GetDefaultUnitAIType() == eUnitAIType)
						{
							iNumUnits++;
						}
					}
				}
			}
		}
	}

	return iNumUnits;
}



int CvPlayer::GetNumUnitsWithDomain(DomainTypes eDomain, bool bMilitaryOnly)
{
	int iNumUnits = 0;

	CvUnit* pLoopUnit;
	int iLoop;


	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if(pLoopUnit->getDomainType() == eDomain)
		{
			if(!bMilitaryOnly || pLoopUnit->IsCombatUnit())
			{
				iNumUnits++;
			}
		}
	}

	return iNumUnits;
}


int CvPlayer::GetNumUnitsWithUnitCombat(UnitCombatTypes eUnitCombat)
{
	int iNumUnits = 0;

	CvUnit* pLoopUnit;
	int iLoop;


	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if(pLoopUnit->getUnitCombatType() == eUnitCombat)
		{
			iNumUnits++;
		}
	}

	return iNumUnits;
}



void CvPlayer::InitDangerPlots()
{
	m_pDangerPlots->Init(GetID(), true              );
}


void CvPlayer::UpdateDangerPlots()
{
	m_pDangerPlots->UpdateDanger(false, false);
}


void CvPlayer::SetDangerPlotsDirty()
{
	m_pDangerPlots->SetDirty();
}


bool CvPlayer::isHuman() const
{
	if(GetID() == NO_PLAYER)
	{
		return false;
	}

	return CvPreGame::isHuman(GetID());
}


bool CvPlayer::isObserver() const
{
	if(GetID() == NO_PLAYER)
	{
		return false;
	}

	return CvPreGame::slotStatus(GetID()) == SS_OBSERVER;
}



bool CvPlayer::isBarbarian() const
{
	return (GetID() == BARBARIAN_PLAYER);
}


void CvPlayer::doBarbarianRansom(int iOption, int iUnitID)
{
	UnitHandle pUnit = getUnit(iUnitID);


	if(iOption == 0)
	{
		int iNumGoldStolen = GC.getBARBARIAN_UNIT_GOLD_RANSOM();

		if(iNumGoldStolen > GetTreasury()->GetGold())
		{
			iNumGoldStolen = GetTreasury()->GetGold();
		}


		GetTreasury()->ChangeGold(-iNumGoldStolen);
	}

	else if(iOption == 1)
	{
		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->AddUnitMessage(0, pUnit->GetIDInfo(), GetID(), true, GC.getEVENT_MESSAGE_TIME(), GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_RANSOM_KILL_BY_BARBARIANS", pUnit->getNameKey()));
		}

		pUnit->kill(true, BARBARIAN_PLAYER);
	}
}


const char* CvPlayer::getName() const
{
	if(isMinorCiv())
	{
		return GC.getMinorCivInfo(GetMinorCivAI()->GetMinorCivType())->GetDescription();
	}

	if(GC.getGame().isMPOption(MPOPTION_ANONYMOUS) && isAlive() && GC.getGame().getGameState() == GAMESTATE_ON)
	{
		return getLeaderInfo().GetDescription();
	}

	if(GC.getGame().isGameMultiPlayer() && isHuman())
	{
		const CvString& szDisplayName = CvPreGame::nicknameDisplayed(GetID());
		if(szDisplayName.GetLength())
		{
			return szDisplayName.c_str();
		}
	}
	
	const CvString& szPlayerName = CvPreGame::leaderName(GetID());
	if(szPlayerName.GetLength() == 0)
	{
		return getLeaderInfo().GetDescription();
	}

	return szPlayerName.c_str();
}


const char* CvPlayer::getNameKey() const
{
	if(isMinorCiv())
	{
		return GC.getMinorCivInfo(GetMinorCivAI()->GetMinorCivType())->GetTextKey();
	}

	if(GC.getGame().isMPOption(MPOPTION_ANONYMOUS) && isAlive())
	{
		return getLeaderInfo().GetTextKey();
	}

	if(GC.getGame().isGameMultiPlayer() && isHuman())
	{

		const CvString& szDisplayName = CvPreGame::nicknameDisplayed(GetID());
		if(szDisplayName.GetLength())
			return szDisplayName.c_str();
	}

	const CvString& szPlayerName = CvPreGame::leaderNameKey(GetID());
	if(szPlayerName.GetLength() == 0)
	{
		return getLeaderInfo().GetTextKey();
	}

	return szPlayerName.c_str();
}



const char* CvPlayer::getCivilizationDescription() const
{
	if(isMinorCiv())
	{
		return GC.getMinorCivInfo(GetMinorCivAI()->GetMinorCivType())->GetDescription();
	}
	else if(CvPreGame::civilizationDescription(GetID()).GetLength() == 0)
	{
		return getCivilizationInfo().GetDescription();
	}
	else
	{
		return CvPreGame::civilizationDescription(GetID()).c_str();
	}
}



const char* CvPlayer::getCivilizationDescriptionKey() const
{
	if(isMinorCiv())
	{
		return GC.getMinorCivInfo(GetMinorCivAI()->GetMinorCivType())->GetTextKey();
	}
	else if(CvPreGame::civilizationDescriptionKey(GetID()).GetLength() == 0)
	{
		return getCivilizationInfo().GetTextKey();
	}
	else
	{
		return CvPreGame::civilizationDescriptionKey(GetID()).c_str();
	}
}



const char* CvPlayer::getCivilizationShortDescription() const
{
	if(isMinorCiv())
	{
		return GC.getMinorCivInfo(GetMinorCivAI()->GetMinorCivType())->getShortDescription();
	}
	else if(CvPreGame::civilizationShortDescription(GetID()).GetLength() == 0)
	{
		return getCivilizationInfo().getShortDescription();
	}
	else
	{
		return CvPreGame::civilizationShortDescription(GetID()).c_str();
	}
}



const char* CvPlayer::getCivilizationShortDescriptionKey() const
{
	if(isMinorCiv())
	{
		return GC.getMinorCivInfo(GetMinorCivAI()->GetMinorCivType())->getShortDescriptionKey();
	}
	else if(CvPreGame::civilizationShortDescriptionKey(GetID()).GetLength() == 0)
	{
		return getCivilizationInfo().getShortDescriptionKey();
	}
	else
	{
		return CvPreGame::civilizationShortDescriptionKey(GetID()).c_str();
	}
}



const char* CvPlayer::getCivilizationAdjective() const
{
	if(isMinorCiv())
	{
		return GC.getMinorCivInfo(GetMinorCivAI()->GetMinorCivType())->getAdjective();
	}
	else if(CvPreGame::civilizationAdjective(GetID()).GetLength() == 0)
	{
		return getCivilizationInfo().getAdjective();
	}
	else
	{
		return CvPreGame::civilizationAdjective(GetID()).c_str();
	}
}


const char* CvPlayer::getCivilizationAdjectiveKey() const
{
	if(isMinorCiv())
	{
		return GC.getMinorCivInfo(GetMinorCivAI()->GetMinorCivType())->getAdjectiveKeyWide();
	}
	else if(CvPreGame::civilizationAdjectiveKey(GetID()).GetLength() == 0)
	{
		return getCivilizationInfo().getAdjectiveKey();
	}
	else
	{
		return CvPreGame::civilizationAdjectiveKey(GetID()).c_str();
	}
}


const char* CvPlayer::getCivilizationTypeKey() const
{
	if(isMinorCiv())
	{
		return GC.getMinorCivInfo(GetMinorCivAI()->GetMinorCivType())->GetType();
	}
	else
	{
		return getCivilizationInfo().GetType();
	}
}


const char* CvPlayer::getLeaderTypeKey() const
{
	if(isMinorCiv())
	{
		return GC.getMinorCivInfo(GetMinorCivAI()->GetMinorCivType())->GetType();
	}
	else
	{
		return getLeaderInfo().GetType();
	}
}


bool CvPlayer::isWhiteFlag() const
{
	return CvPreGame::isWhiteFlag(GetID());
}



const char* CvPlayer::GetStateReligionName() const
{
	return GetLocalizedText(m_strReligionKey.get());
}


CvString CvPlayer::GetStateReligionKey() const
{
	return m_strReligionKey.get();
}


void CvPlayer::SetStateReligionKey(const char* strKey)
{
	m_strReligionKey = strKey;
}



const CvString CvPlayer::getWorstEnemyName() const
{
	TeamTypes eWorstEnemy;

	eWorstEnemy = NO_TEAM;

	if(eWorstEnemy != NO_TEAM)
	{
		return GET_TEAM(eWorstEnemy).getName();
	}

	return "";
}



ArtStyleTypes CvPlayer::getArtStyleType() const
{
	if(CvPreGame::artStyle(GetID()) == NO_ARTSTYLE)
	{
		return ((ArtStyleTypes)(getCivilizationInfo().getArtStyleType()));
	}
	else
	{
		return CvPreGame::artStyle(GetID());
	}
}

#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
int CvPlayer::getCachedJONSCultureForThisTurn() const
{
	return m_iCachedJONSCultureForThisTurn;
}

int CvPlayer::getCachedScienceT100ForThisTurn() const
{
	return m_iCachedScienceT100ForThisTurn;
}

int CvPlayer::getCachedFaithForThisTurn() const
{
	return m_iCachedFaithForThisTurn;
}

int CvPlayer::getCachedExcessHappinessForThisTurn() const
{
	return m_iCachedExcessHappinessForThisTurn;
}

int CvPlayer::getCachedSpyStartingRank() const
{
	return m_iCachedSpyStartingRank;
}
#endif


void CvPlayer::doTurn()
{

#ifdef AUI_PERF_LOGGING_FORMATTING_TWEAKS
	AI_PERF_FORMAT("AI-perf.csv", ("CvPlayer::doTurn(), Turn %03d, %s", GC.getGame().getGameTurn(), getCivilizationShortDescription()));
#else
	AI_PERF_FORMAT("AI-perf.csv", ("CvPlayer::doTurn(), Turn %d, %s", GC.getGame().getGameTurn(), getCivilizationShortDescription()));
#endif

	CvAssertMsg(isAlive(), "isAlive is expected to be true");

	doUpdateCacheOnTurn();

	AI_doTurnPre();

#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
	cacheYields();
#endif

	if(getCultureBombTimer() > 0)
		changeCultureBombTimer(-1);

	if(getConversionTimer() > 0)
		changeConversionTimer(-1);

	if(GetTurnsSinceSettledLastCity() >= 0)
		ChangeTurnsSinceSettledLastCity(1);

	setConscriptCount(0);

	DoUpdateCramped();

	DoUpdateUprisings();
	DoUpdateCityRevolts();

	if(GetPlayerTraits()->IsEndOfMayaLongCount())
	{
		ChangeNumMayaBoosts(1);
	}

#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
	GC.getGame().GetGameTrade()->DoTurn(GetID());

	GetCulture()->DoTurn();

	GC.getGame().GetGameDeals()->DoTurn(GetID());
#endif

	bool bHasActiveDiploRequest = false;
	if(isAlive())
	{
		if(!isBarbarian())
		{
			if(!isMinorCiv())
			{
#ifdef DO_CANCEL_DEALS_WITH_AI
				if (!isHuman() && GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_DEALS"))
				{
					GC.getGame().GetGameTrade()->ClearAllCivTradeRoutes(GetID());
					for (int iLoopTeam = 0; iLoopTeam < MAX_CIV_TEAMS; iLoopTeam++)
					{
						TeamTypes eTeam = (TeamTypes)iLoopTeam;
						if (getTeam() != eTeam && GET_TEAM(eTeam).isAlive() && GET_TEAM(eTeam).isHuman())
						{
							GC.getGame().GetGameDeals()->DoCancelDealsBetweenTeams(GET_PLAYER(GetID()).getTeam(), (TeamTypes)iLoopTeam);
							GET_TEAM(getTeam()).CloseEmbassyAtTeam(eTeam);
							GET_TEAM(eTeam).CloseEmbassyAtTeam(getTeam());
							GET_TEAM(getTeam()).CancelResearchAgreement(eTeam);
							GET_TEAM(eTeam).CancelResearchAgreement(getTeam());
							GET_TEAM(getTeam()).EvacuateDiplomatsAtTeam(eTeam);
							GET_TEAM(eTeam).EvacuateDiplomatsAtTeam(getTeam());


							GC.getMap().verifyUnitValidPlot();
						}
					}
				}
#endif
#if !defined(LEKMOD_v34)
				GetTrade()->DoTurn();
#endif
				GetMilitaryAI()->ResetCounters();
				GetGrandStrategyAI()->DoTurn();
				if(GC.getGame().isHotSeat() && !isHuman())
				{


					GetDiplomacyAI()->DoTurn((PlayerTypes)CvDiplomacyAI::DIPLO_AI_PLAYERS);
				}
				else
					GetDiplomacyAI()->DoTurn((PlayerTypes)CvDiplomacyAI::DIPLO_ALL_PLAYERS);

				if (!isHuman())
					bHasActiveDiploRequest = CvDiplomacyRequests::HasActiveDiploRequestWithHuman(GetID());
			}
		}
	}

	if(isHuman() && !GC.getGame().isGameMultiPlayer())
		doArmySize();

	if( (bHasActiveDiploRequest || GC.GetEngineUserInterface()->isDiploActive()) && !GC.getGame().isGameMultiPlayer() && !isHuman())
	{
		GC.getGame().SetWaitingForBlockingInput(m_eID);
	}
	else
	{
		doTurnPostDiplomacy();
	}
#if defined(TRAITIFY)
	updateYield();
#endif
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "PlayerDoTurn", args.get(), bResult);
	}

	m_kPlayerAchievements.StartTurn();
}


void CvPlayer::doTurnPostDiplomacy()
{
	CvGame& kGame = GC.getGame();

#ifdef PENALTY_FOR_DELAYING_POLICIES
	while (IsDelayedPolicy() && GC.getGame().isOption("GAMEOPTION_NO_MP_POLICY_SAVING"))
	{
		FStaticVector<PolicyTypes, 64, true, c_eCiv5GameplayDLL, 0> adoptablePolicies;
		FStaticVector<PolicyBranchTypes, 16, true, c_eCiv5GameplayDLL, 0> adoptablePolicyBranches;

		for (int iI = 0; iI < GC.GetGamePolicies()->GetNumPolicies(); iI++)
		{
			if (canAdoptPolicy((PolicyTypes)iI))
			{
				adoptablePolicies.push_back((PolicyTypes)iI);
			}
		}

		if (adoptablePolicies.size() > 0)
		{
			int iNextPolicy = GC.getGame().getJonRandNum(adoptablePolicies.size(), "Random Policy Pick");
			doAdoptPolicy(adoptablePolicies[iNextPolicy]);
			if (!(getJONSCulture() >= getNextPolicyCost() || GetNumFreePolicies() > 0))
			{
				setIsDelayedPolicy(false);
			}
		}
		else
		{
			for (int iI = 0; iI < GC.GetGamePolicies()->GetNumPolicyBranches(); iI++)
			{
				PolicyBranchTypes eBranch = (PolicyBranchTypes)iI;
				if (GetPlayerPolicies()->CanUnlockPolicyBranch(eBranch))
				{
					adoptablePolicyBranches.push_back((PolicyBranchTypes)iI);
				}
			}

			if (adoptablePolicyBranches.size() > 0)
			{
				int iNextPolicyBranch = GC.getGame().getJonRandNum(adoptablePolicyBranches.size(), "Random Policy Pick");
				GetPlayerPolicies()->DoUnlockPolicyBranch(adoptablePolicyBranches[iNextPolicyBranch]);
				if (!(getJONSCulture() >= getNextPolicyCost() || GetNumFreePolicies() > 0))
				{
					setIsDelayedPolicy(false);
				}
			}
			else
			{
				setIsDelayedPolicy(false);
			}
		}
		adoptablePolicies.clear();
		adoptablePolicyBranches.clear();
	}


	int iNumFreePoliciesFromProjectReward = GetNumFreePolicies() / 1024;
	if (iNumFreePoliciesFromProjectReward > 0)
	{
		ChangeNumFreePolicies(-1024 * iNumFreePoliciesFromProjectReward);
	}
	if (kGame.isOption(GAMEOPTION_END_TURN_TIMER_ENABLED))
	{
		if (getJONSCulture() >= getNextPolicyCost() || GetNumFreePolicies() > 0)
		{
			if (isHuman())
			{
				setIsDelayedPolicy(true);
			}
		}
		else
		{
			setIsDelayedPolicy(false);
		}
	}
	ChangeNumFreePolicies(iNumFreePoliciesFromProjectReward);
#endif
	if(isAlive())
	{
		kGame.GetTacticalAnalysisMap()->RefreshDataForNextPlayer(this);

		{
			AI_PERF_FORMAT("AI-perf.csv", ("Plots/Danger, Turn %03d, %s", kGame.getElapsedGameTurns(), getCivilizationShortDescription()) );

			UpdatePlots();
			m_pDangerPlots->UpdateDanger();
		}

		if(!isBarbarian())
		{
			GetEconomicAI()->DoTurn();
			GetMilitaryAI()->DoTurn();
			GetReligionAI()->DoTurn();
			GetTradeAI()->DoTurn();
			GetCitySpecializationAI()->DoTurn();
			GetLeagueAI()->DoTurn();
		}

		if(isMinorCiv())
		{
			GetMinorCivAI()->DoTurn();
		}
	}


	if(GetAttackBonusTurns() > 0)
	{
		ChangeAttackBonusTurns(-1);
	}
	if(GetCultureBonusTurns() > 0)
	{
		ChangeCultureBonusTurns(-1);
	}
	if(GetTourismBonusTurns() > 0)
	{
		ChangeTourismBonusTurns(-1);
	}


	DoProcessGoldenAge();


	DoGreatPeopleSpawnTurn();

#ifdef AUTOMATICALLY_SPEND_FREE_TECHNOLOGIES
	if (GC.getGame().isOption("GAMEOPTION_NO_TECH_SAVING"))
	{
		FStaticVector<TechTypes, 128, true, c_eCiv5GameplayDLL> vePossibleTechs;
		int iCheapestTechCost = MAX_INT;
		while (GetNumFreeTechs() > 0)
		{
			for (int i = 0; i < GC.getNumTechInfos(); i++)
			{
				TechTypes e = (TechTypes)i;
				CvTechEntry* pInfo = GC.getTechInfo(e);
				if (pInfo)
				{

					if (!GET_TEAM(getTeam()).GetTeamTechs()->HasTech(e))
					{

						if (GetPlayerTechs()->CanResearch(e))
						{
							if (pInfo->GetResearchCost() < iCheapestTechCost)
							{
								iCheapestTechCost = pInfo->GetResearchCost();
								vePossibleTechs.clear();
								vePossibleTechs.push_back(e);
							}
							else if (pInfo->GetResearchCost() == iCheapestTechCost)
							{
								vePossibleTechs.push_back(e);
							}
						}
					}
				}
			}

			if (!vePossibleTechs.empty())
			{
				int iRoll = GC.getGame().getJonRandNum((int)vePossibleTechs.size(), "Rolling to choose free tech from conquering a city");
				TechTypes eFreeTech = vePossibleTechs[iRoll];
				CvAssert(eFreeTech != NO_TECH)
					if (eFreeTech != NO_TECH)
					{
						GET_TEAM(getTeam()).setHasTech(eFreeTech, true, GetID(), true, true);
						GET_TEAM(getTeam()).GetTeamTechs()->SetNoTradeTech(eFreeTech, true);
					}
			}
			SetNumFreeTechs(max(0, GetNumFreeTechs() - 1));
		}
	}
#endif


	{
		AI_PERF_FORMAT("AI-perf.csv", ("Do City Turns, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), getCivilizationShortDescription()) );
		if(getNumCities() > 0)
		{
			int iLoop = 0;
			for(CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
				kGame.GetGameReligions()->SpreadReligionToOneCity(pLoopCity);
#endif
				pLoopCity->doTurn();
			}
		}
	}


	GetTreasury()->DoGold();

#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE

	DoIncomingUnits();


	doResearch();

	GetEspionage()->DoTurn();
#endif




	GetCulture()->SetLastTurnLifetimeCulture(GetJONSCultureEverGenerated());
#ifdef NQM_PRUNING
	if (!kGame.isOption(GAMEOPTION_END_TURN_TIMER_ENABLED) || (getJONSCulture() < getNextPolicyCost()))
	{
#else
	if(kGame.isOption(GAMEOPTION_END_TURN_TIMER_ENABLED))
	{
#ifdef AI_CULTURE_RESTRICTION
		if (getJONSCulture() < getNextPolicyCost())
		{
			if (isHuman() || getNextPolicyCost() < 1000 || !GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_CULTURE"))
			{
#if defined(AUI_PLAYER_FIX_JONS_CULTURE_IS_T100) && defined(AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE)
			changeJONSCultureTimes100(getCachedJONSCultureForThisTurn());
#elif defined(AUI_PLAYER_FIX_JONS_CULTURE_IS_T100)
			changeJONSCultureTimes100(GetTotalJONSCulturePerTurnTimes100());
#elif defined(AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE)
			changeJONSCulture(getCachedJONSCultureForThisTurn());
#else
			changeJONSCulture(GetTotalJONSCulturePerTurn());
#endif
			}
		}
#else
		if(getJONSCulture() < getNextPolicyCost())
#if defined(AUI_PLAYER_FIX_JONS_CULTURE_IS_T100) && defined(AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE)
			changeJONSCultureTimes100(getCachedJONSCultureForThisTurn());
#elif defined(AUI_PLAYER_FIX_JONS_CULTURE_IS_T100)
			changeJONSCultureTimes100(GetTotalJONSCulturePerTurnTimes100());
#elif defined(AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE)
			changeJONSCulture(getCachedJONSCultureForThisTurn());
#else
			changeJONSCulture(GetTotalJONSCulturePerTurn());
#endif
#endif
#endif
	}
#ifndef NQM_PRUNING
	else
	{
#if defined(AUI_PLAYER_FIX_JONS_CULTURE_IS_T100) && defined(AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE)
		changeJONSCultureTimes100(getCachedJONSCultureForThisTurn());
#elif defined(AUI_PLAYER_FIX_JONS_CULTURE_IS_T100)
		changeJONSCultureTimes100(GetTotalJONSCulturePerTurnTimes100());
#elif defined(AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE)
		changeJONSCulture(getCachedJONSCultureForThisTurn());
#else
		changeJONSCulture(GetTotalJONSCulturePerTurn());
#endif
	}
#endif


	DoUpdateNextPolicyCost();


	if(isAlive() && isHuman() && getNumCities() > 0)
	{
		if (!GC.GetEngineUserInterface()->IsPolicyNotificationSeen())
		{
#if !defined(UPDATE_CULTURE_NOTIFICATION_DURING_TURN)
			if (getNextPolicyCost() <= getJONSCulture() && GetPlayerPolicies()->GetNumPoliciesCanBeAdopted() > 0)
			{
				CvNotifications* pNotifications = GetNotifications();
				if (pNotifications)
				{
					CvString strBuffer;

					if (kGame.isOption(GAMEOPTION_POLICY_SAVING))
						strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ENOUGH_CULTURE_FOR_POLICY_DISMISS");
					else
						strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ENOUGH_CULTURE_FOR_POLICY");

					CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ENOUGH_CULTURE_FOR_POLICY");
					pNotifications->Add(NOTIFICATION_POLICY, strBuffer, strSummary, -1, -1, -1);
				}
			}
		}
#else
			TestMidTurnPolicyNotification();
		}
#endif

		if (GetPlayerPolicies()->IsTimeToChooseIdeology() && GetPlayerPolicies()->GetLateGamePolicyTree() == NO_POLICY_BRANCH_TYPE)
		{
			CvNotifications* pNotifications = GetNotifications();
			if(pNotifications)
			{
				CvString strBuffer;
				if (GetCurrentEra() > GC.getInfoTypeForString("ERA_INDUSTRIAL"))
				{
					strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_CHOOSE_IDEOLOGY_ERA");
				}
				else
				{
					strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_CHOOSE_IDEOLOGY_FACTORIES");
				}
				CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_CHOOSE_IDEOLOGY");
				pNotifications->Add(NOTIFICATION_CHOOSE_IDEOLOGY, strBuffer, strSummary, -1, -1, GetID());
			}
		}
	}

	if (isAlive() && getNumCities() > 0 && !isHuman() && !isMinorCiv())
	{
		if (GetPlayerPolicies()->IsTimeToChooseIdeology() && GetPlayerPolicies()->GetLateGamePolicyTree() == NO_POLICY_BRANCH_TYPE)
		{
			AI_PERF_FORMAT("AI-perf.csv", ("DoChooseIdeology, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), getCivilizationShortDescription()) );
			GetPlayerPolicies()->DoChooseIdeology();
		}
	}

	if(!isBarbarian() && !isHuman())
	{
		AI_PERF_FORMAT("AI-perf.csv", ("DoPolicyAI, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), getCivilizationShortDescription()) );
		GetPlayerPolicies()->DoPolicyAI();
	}

#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE

	ChangeFaith(getCachedFaithForThisTurn());
#else

	doResearch();

	GetEspionage()->DoTurn();


	CvGameReligions* pGameReligions = kGame.GetGameReligions();
	pGameReligions->DoPlayerTurn(*this);
#endif


	CvGameLeagues* pGameLeagues = kGame.GetGameLeagues();
	pGameLeagues->DoPlayerTurn(*this);


	if(GetAnarchyNumTurns() > 0)
		ChangeAnarchyNumTurns(-1);

#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
	GetEspionage()->CacheSpyStats();

	int iLoop = 0;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->doResourceDemands();
	}
#else
	DoIncomingUnits();
#endif
#if defined(LEKMOD_v34)
	GetTrade()->DoTurn();
#endif
	const int iGameTurn = kGame.getGameTurn();

	GatherPerTurnReplayStats(iGameTurn);
#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE

	kGame.GetGameReligions()->DoPlayerTurn(*this);
#endif

	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);

	AI_doTurnPost();
}

#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
void CvPlayer::cacheYields()
{

	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->GetCityCitizens()->DoReallocateCitizens();
	}
#ifdef AUI_PLAYER_SELF_CONSISTENCY_SWEEP_AFTER_INITIAL_REALLOCATE
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->GetCityCitizens()->DoSelfConsistencyCheck();
	}
#endif


	m_iCachedExcessHappinessForThisTurn = GetExcessHappiness();


	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->cacheYieldsForTurn();
	}

	m_pTreasury->cacheGoldT100ForThisTurn();


#ifdef AUI_PLAYER_FIX_JONS_CULTURE_IS_T100
	m_iCachedJONSCultureForThisTurn = GetTotalJONSCulturePerTurnTimes100();
#else
	m_iCachedJONSCultureForThisTurn = GetTotalJONSCulturePerTurn();
#endif


	m_iCachedScienceT100ForThisTurn = GetScienceTimes100();


	m_iCachedFaithForThisTurn = GetTotalFaithPerTurn();

	GetEspionage()->CacheSpyStats();

	m_iCachedSpyStartingRank = GetStartingSpyRank();
}
#endif

#ifdef AUI_CITIZENS_MID_TURN_ASSIGN_RUNS_SELF_CONSISTENCY
void CvPlayer::doSelfConsistencyCheckAllCities()
{
	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->GetCityCitizens()->DoSelfConsistencyCheck();
	}

	if (GetID() == GC.getGame().getActivePlayer())
	{
		GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
	}
}
#endif


void CvPlayer::doTurnUnits()
{
	CvArmyAI* pLoopArmyAI;
	CvUnit* pLoopUnit;
	int iLoop;

	AI_doTurnUnitsPre();


	m_pTacticalAI->DoTurn();


	std::map<int, CvAIOperation*>::iterator iter;
	bool bKilledSomething;
	do
	{
		bKilledSomething = false;
		for(iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
		{
			CvAIOperation* pThisOperation = iter->second;
			if(pThisOperation)
			{
				if(pThisOperation->DoDelayedDeath())
				{
					bKilledSomething = true;
					break;
				}
			}
		}

	}
	while(bKilledSomething);

	for(pLoopArmyAI = firstArmyAI(&iLoop); pLoopArmyAI != NULL; pLoopArmyAI = nextArmyAI(&iLoop))
	{
		pLoopArmyAI->DoDelayedDeath();
	}

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		pLoopUnit->doDelayedDeath();
	}

	for(iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		CvAIOperation* pThisOperation = iter->second;
		if(pThisOperation)
		{
			pThisOperation->DoTurn();
		}
	}

	do
	{
		bKilledSomething = false;
		for(iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
		{
			CvAIOperation* pThisOperation = iter->second;
			if(pThisOperation)
			{
				if(pThisOperation->DoDelayedDeath())
				{
					bKilledSomething = true;
					break;
				}
			}
		}

	}
	while(bKilledSomething);

	for(pLoopArmyAI = firstArmyAI(&iLoop); pLoopArmyAI != NULL; pLoopArmyAI = nextArmyAI(&iLoop))
	{
		pLoopArmyAI->DoTurn();
	}


	m_pHomelandAI->DoTurn();


	for(int iPass = 0; iPass < 4; iPass++)
	{
		for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
		{
			switch(pLoopUnit->getDomainType())
			{
			case DOMAIN_AIR:
				if(iPass == 1)
				{
					pLoopUnit->doTurn();
				}
				break;
			case DOMAIN_SEA:
				if(iPass == 2)
				{
					pLoopUnit->doTurn();
				}
				break;
			case DOMAIN_LAND:
				if(iPass == 3)
				{
					pLoopUnit->doTurn();
				}
				break;
			case DOMAIN_IMMOBILE:
				if(iPass == 0)
				{
					pLoopUnit->doTurn();
				}
				break;
			case NO_DOMAIN:
				CvAssertMsg(false, "Unit with no Domain");
				break;
			default:
				if(iPass == 3)
				{
					pLoopUnit->doTurn();
				}
				break;
			}
		}
	}

	if(GetID() == GC.getGame().getActivePlayer())
	{
		GC.GetEngineUserInterface()->setDirty(Waypoints_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(SelectionButtons_DIRTY_BIT, true);
	}

	GC.GetEngineUserInterface()->setDirty(UnitInfo_DIRTY_BIT, true);

	AI_doTurnUnitsPost();
}



void CvPlayer::SetAllUnitsUnprocessed()
{
	CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		pLoopUnit->SetTurnProcessed(false);
	}
}



void CvPlayer::DoUnitReset()
{
	CvUnit* pLoopUnit;
	int iLoop;

	for (pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{

		if (!pLoopUnit->isEmbarked())
		{
			if (pLoopUnit->hasMoved())
			{
				if (pLoopUnit->isAlwaysHeal())
				{
					pLoopUnit->doHeal();
				}
			}
			else
			{
				if (pLoopUnit->IsHurt())
				{
					pLoopUnit->doHeal();
				}
			}
		}

		int iCitadelDamage;
		if (pLoopUnit->IsNearEnemyCitadel(iCitadelDamage))
		{
			pLoopUnit->changeDamage(iCitadelDamage, NO_PLAYER,                          0.5f);
		}
#if defined(v35_TRAITIFY)
		pLoopUnit->setMadeAttackLastTurn(pLoopUnit->isOutOfAttacks());
		pLoopUnit->setKilledUnitLastTurn(pLoopUnit->killedUnit());
		pLoopUnit->setKilledUnit(false);
#endif

		pLoopUnit->setMoves(pLoopUnit->maxMoves());
		if (pLoopUnit->IsGreatGeneral())
		{


			int newValue = pLoopUnit->GetGreatGeneralStackMovement();
			if (newValue > pLoopUnit->maxMoves())
			{
				pLoopUnit->setMoves(newValue);
			}

		}
#ifdef LEKMOD_LONGSHIP_ALL_PROMO

		if (pLoopUnit->isEmbarked())
		{
			
			int newValue = pLoopUnit->GetEmbarkedUnitStackMovement();
			if (newValue > pLoopUnit->maxMoves())
			{
				pLoopUnit->setMoves(newValue);
			}

		}


		else if (pLoopUnit->getDomainType() == DOMAIN_LAND && !pLoopUnit->IsCombatUnit())
		{
			int newValue = pLoopUnit->GetLandUnitStackMovement();
			if (newValue > pLoopUnit->maxMoves())
			{
				pLoopUnit->setMoves(newValue);
			}
		}
#else

		if (pLoopUnit->isEmbarked())
		{

			int newValue = pLoopUnit->GetEmbarkedUnitStackMovement();
			if (newValue > pLoopUnit->maxMoves())
			{
				pLoopUnit->setMoves(newValue);
			}

		}


#endif



		else if (pLoopUnit->AI_getUnitAIType() == UNITAI_ARCHAEOLOGIST)
		{
			CvPlayer &kPlayer = GET_PLAYER(pLoopUnit->getOwner());
			if (kPlayer.GetCulture()->HasDigCompleteHere(pLoopUnit->plot()))
			{
				pLoopUnit->setMoves(0);
			}
		}

		pLoopUnit->SetIgnoreDangerWakeup(false);
		pLoopUnit->setMadeAttack(false);
		pLoopUnit->setMadeInterception(false);
#if defined(NQM_UNIT_FIX_NO_DOUBLE_INSTAHEAL_ON_SAME_TURN) || defined(NQM_UNIT_FIX_NO_INSTAHEAL_AFTER_PARADROP) || defined(NQM_UNIT_FIX_NO_INSTAHEAL_ON_CREATION_TURN)
		pLoopUnit->setCanInstahealThisTurn(true);
#endif

		if(!isHuman())
		{
			const MissionData* pkMissionData = pLoopUnit->GetHeadMissionData();
			if(pkMissionData)
			{
				if(pkMissionData->eMissionType == CvTypes::getMISSION_RANGE_ATTACK() ||
				        pkMissionData->eMissionType == CvTypes::getMISSION_AIRSTRIKE() ||
				        pkMissionData->eMissionType == CvTypes::getMISSION_AIR_SWEEP() ||
				        pkMissionData->eMissionType == CvTypes::getMISSION_NUKE())
				{
					CvAssertMsg(0, "An AI unit has a combat mission queued at the end of its turn.");
					pLoopUnit->ClearMissionQueue();
				}
			}
		}
	}
}



void CvPlayer::DoUnitAttrition()
{
	CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		pLoopUnit->DoAttrition();
	}
}


void CvPlayer::RespositionInvalidUnits()
{
	CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if(!pLoopUnit)
		{
			continue;
		}

		if(pLoopUnit->isDelayedDeath())
		{
			continue;
		}

		if(pLoopUnit->isCargo())
		{
			continue;
		}

		if(pLoopUnit->isInCombat())
		{
			continue;
		}

		CvPlot* pPlot = pLoopUnit->plot();
		if(!pPlot)
		{
			continue;
		}

		if(pPlot->getNumFriendlyUnitsOfType(pLoopUnit) > GC.getPLOT_UNIT_LIMIT())
		{
			if (!pLoopUnit->jumpToNearestValidPlot())
				pLoopUnit->kill(false);
		}
	}
}


void CvPlayer::updateYield()
{





	CvMap& kMap = GC.getMap();
	int iNumPlots = kMap.numPlots();
	PlayerTypes ePlayer = GetID();
	for (int iI = 0; iI < iNumPlots; iI++)
	{
		CvPlot* pkPlot = kMap.plotByIndexUnchecked(iI);
		if (pkPlot->getOwner() == ePlayer)
			pkPlot->updateYield();
	}
}


void CvPlayer::updateExtraSpecialistYield()
{
	CvCity* pLoopCity;
	int iLoop;

	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateExtraSpecialistYield();
	}
}


void CvPlayer::updateCityPlotYield()
{
	CvCity* pLoopCity;
	int iLoop;

	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->plot()->updateYield();
	}
}



void CvPlayer::updateCitySight(bool bIncrement)
{
	CvCity* pLoopCity;
	int iLoop;

	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->plot()->updateSight(bIncrement);
	}
}


void CvPlayer::UpdateNotifications()
{
	if(GetNotifications())
	{
		GetNotifications()->Update();
	}

	if(GetDiplomacyRequests())
	{
		GetDiplomacyRequests()->Update();
	}
}


void CvPlayer::UpdateReligion()
{
	DoUpdateHappiness();
}


void CvPlayer::updateTimers()
{
	CvUnit* pLoopUnit;
	int iLoop;
	m_endTurnBusyUnitUpdatesLeft--;
	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		pLoopUnit->UpdateMission();
		pLoopUnit->doDelayedDeath();
	}

	GetDiplomacyAI()->update();
}


bool CvPlayer::hasPromotableUnit() const
{
	const CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if(pLoopUnit->isPromotionReady() && !pLoopUnit->isDelayedDeath())
		{
			return true;
		}
	}

	return false;
}


bool CvPlayer::hasReadyUnit() const
{
	const CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if(pLoopUnit->ReadyToMove() && !pLoopUnit->isDelayedDeath())
		{
			return true;
		}
	}

	return false;
}


int CvPlayer::GetCountReadyUnits() const
{
	int iRtnValue = 0;
	const CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if(pLoopUnit->ReadyToMove() && !pLoopUnit->isDelayedDeath())
		{
			iRtnValue++;
		}
	}

	return iRtnValue;
}


const CvUnit* CvPlayer::GetFirstReadyUnit() const
{
	const CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if(pLoopUnit->ReadyToMove() && !pLoopUnit->isDelayedDeath())
		{
			return pLoopUnit;
		}
	}

	return NULL;
}


void CvPlayer::EndTurnsForReadyUnits()
{
	CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if(pLoopUnit->ReadyToMove() && !pLoopUnit->isDelayedDeath())
		{
			pLoopUnit->finishMoves();
		}
	}
}


bool CvPlayer::hasAutoUnit() const
{
	const CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if(pLoopUnit->ReadyToAuto())
		{
			return true;
		}
	}

	return false;
}


bool CvPlayer::hasBusyUnit() const
{
	const CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if(pLoopUnit->IsBusy())
		{
			return true;
		}
	}

	return false;
}


bool CvPlayer::hasBusyCity() const
{
	const CvCity* pLoopCity;
	int iLoop;

	for(pLoopCity = firstCity(&iLoop); pLoopCity; pLoopCity = nextCity(&iLoop))
	{
		if(pLoopCity->IsBusy())
		{
			return true;
		}
	}

	return false;
}


const CvCity* CvPlayer::getBusyCity() const
{
	const CvCity* pLoopCity;
	int iLoop;

	for(pLoopCity = firstCity(&iLoop); pLoopCity; pLoopCity = nextCity(&iLoop))
	{
		if(pLoopCity->IsBusy())
		{
			return pLoopCity;
		}
	}

#ifdef AUI_WARNING_FIXES
	return NULL;
#else
#if defined(LEKMOD_MACOS)
	return NULL;
#else
	return false;
#endif
#endif
}


bool CvPlayer::hasBusyUnitOrCity() const
{
	if(hasBusyUnit())
		return true;
	return hasBusyCity();
}


const UnitHandle CvPlayer::getBusyUnit() const
{
	const UnitHandle result;
	const CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if(pLoopUnit->IsBusy())
		{
			result = pLoopUnit;
		}
	}
	return result;
}



void CvPlayer::chooseTech(int iDiscover, const char* strText, TechTypes iTechJustDiscovered)
{
	if(GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
	{
		return;
	}

	if(iDiscover > 0)
	{
		SetNumFreeTechs(GetNumFreeTechs()+iDiscover);
	}

	if(iDiscover > 0)
	{
		CvNotifications* pNotifications = GetNotifications();
		if(pNotifications)
		{
			pNotifications->Add(NOTIFICATION_FREE_TECH, strText, strText, -1, -1, iDiscover, iTechJustDiscovered);
		}
	}
	else if(strText == 0 || strText[0] == 0)
	{
		CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_NEW_RESEARCH");
		CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_NEW_RESEARCH");
		CvNotifications* pNotifications = GetNotifications();
		if(pNotifications)
		{
			pNotifications->Add(NOTIFICATION_TECH, strBuffer, strSummary, -1, -1, iDiscover, iTechJustDiscovered);
		}
	}
	else
	{
		CvNotifications* pNotifications = GetNotifications();
		if(pNotifications)
		{
			pNotifications->Add(NOTIFICATION_TECH, strText, strText, -1, -1, iDiscover, iTechJustDiscovered);
		}
	}
}







int CvPlayer::GetScore(bool bFinal, bool bWinner) const
{
	if(!isAlive())
		return 0;

	if(GET_TEAM(getTeam()).getNumMembers() == 0)
		return 0;

	int iScore = 0;

	iScore += GetScoreFromCities();
	iScore += GetScoreFromPopulation();
	iScore += GetScoreFromLand();
	iScore += GetScoreFromWonders();
	iScore += GetScoreFromPolicies();
	iScore += GetScoreFromGreatWorks();
	iScore += GetScoreFromReligion();
	iScore += GetScoreFromTechs();
	iScore += GetScoreFromFutureTech();
	iScore += GetScoreFromScenario1();
	iScore += GetScoreFromScenario2();
	iScore += GetScoreFromScenario3();
	iScore += GetScoreFromScenario4();


	if(bFinal && bWinner)
	{
		int iGameProgressPercent = 100 * GC.getGame().getGameTurn() / GC.getGame().getEstimateEndTurn();
		iGameProgressPercent = iGameProgressPercent < 1 ? 1 : iGameProgressPercent;
		iScore *= 100;
		iScore /= iGameProgressPercent;
	}

	return iScore;
}



int CvPlayer::GetScoreFromCities() const
{
#ifdef NQM_OPTIONAL_SCORING_TWEAKS
	int iScore = getNumCities();
	if (GC.getGame().isOption("GAMEOPTION_TWEAKED_SCORING"))
		iScore *= GC.getNEW_SCORE_CITY_MULTIPLIER();
	else
		iScore *= GC.getSCORE_CITY_MULTIPLIER();
#else
	int iScore = getNumCities() *        GC.getSCORE_CITY_MULTIPLIER();
#endif

	iScore *= GC.getGame().GetMapScoreMod();
	iScore /= 100;

	return iScore;
}



int CvPlayer::GetScoreFromPopulation() const
{
#ifdef NQM_OPTIONAL_SCORING_TWEAKS
	int iScore = getTotalPopulation();
	if (GC.getGame().isOption("GAMEOPTION_TWEAKED_SCORING"))
		iScore *= GC.getNEW_SCORE_POPULATION_MULTIPLIER();
	else
		iScore *= GC.getSCORE_POPULATION_MULTIPLIER();
#else
	int iScore = getTotalPopulation() *       GC.getSCORE_POPULATION_MULTIPLIER();
#endif

	iScore *= GC.getGame().GetMapScoreMod();
	iScore /= 100;

	return iScore;
}



int CvPlayer::GetScoreFromLand() const
{
	int iScore = getTotalLand() *       GC.getSCORE_LAND_MULTIPLIER();

	iScore *= GC.getGame().GetMapScoreMod();
	iScore /= 100;

	return iScore;
}



int CvPlayer::GetScoreFromWonders() const
{
#ifdef NQM_OPTIONAL_SCORING_TWEAKS
	int iScore = GetNumWonders();
	if (GC.getGame().isOption("GAMEOPTION_TWEAKED_SCORING"))
		iScore *= GC.getNEW_SCORE_WONDER_MULTIPLIER();
	else
		iScore *= GC.getSCORE_WONDER_MULTIPLIER();
#else
	int iScore = GetNumWonders() *        GC.getSCORE_WONDER_MULTIPLIER();
#endif
	return iScore;
}



int CvPlayer::GetScoreFromPolicies() const
{
	if(GC.getGame().isOption(GAMEOPTION_NO_POLICIES))
	{
		return 0;
	}
#ifdef NQM_OPTIONAL_SCORING_TWEAKS
	int iScore = GetPlayerPolicies()->GetNumPoliciesOwned();
	if (GC.getGame().isOption("GAMEOPTION_TWEAKED_SCORING"))
		iScore *= GC.getNEW_SCORE_POLICY_MULTIPLIER();
	else
		iScore *= GC.getSCORE_POLICY_MULTIPLIER();
#else
	int iScore = GetPlayerPolicies()->GetNumPoliciesOwned() *       GC.getSCORE_POLICY_MULTIPLIER();
#endif
	return iScore;
}



int CvPlayer::GetScoreFromGreatWorks() const
{
#ifdef NQM_OPTIONAL_SCORING_TWEAKS
	int iScore = GetCulture()->GetNumGreatWorks();
	if (GC.getGame().isOption("GAMEOPTION_TWEAKED_SCORING"))
		iScore *= GC.getNEW_SCORE_GREAT_WORK_MULTIPLIER();
	else
		iScore *= GC.getSCORE_GREAT_WORK_MULTIPLIER();
#else
	int iScore = GetCulture()->GetNumGreatWorks() *       GC.getSCORE_GREAT_WORK_MULTIPLIER();
#endif
	return iScore;
}



int CvPlayer::GetScoreFromReligion() const
{
	if(GC.getGame().isOption(GAMEOPTION_NO_RELIGION))
	{
		return 0;
	}
	int iScore = 0;
	CvGameReligions *pGameReligions = GC.getGame().GetGameReligions();
	ReligionTypes eReligion = GetReligions()->GetReligionCreatedByPlayer();
	if (eReligion > RELIGION_PANTHEON)
	{
		const CvReligion *pReligion = pGameReligions->GetReligion(eReligion, GetID());
#ifdef NQM_OPTIONAL_SCORING_TWEAKS
		iScore = pReligion->m_Beliefs.GetNumBeliefs();
		if (GC.getGame().isOption("GAMEOPTION_TWEAKED_SCORING"))
			iScore *= GC.getNEW_SCORE_BELIEF_MULTIPLIER();
		else
			iScore *= GC.getSCORE_BELIEF_MULTIPLIER();
#else
		iScore += pReligion->m_Beliefs.GetNumBeliefs() *        GC.getSCORE_BELIEF_MULTIPLIER();
#endif
		iScore += pGameReligions->GetNumCitiesFollowing(eReligion) *       GC.getSCORE_RELIGION_CITIES_MULTIPLIER();
	}
	return iScore;
}



int CvPlayer::GetScoreFromTechs() const
{
	if(GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
	{
		return 0;
	}


#ifdef NQM_OPTIONAL_SCORING_TWEAKS
	int iScore = GET_TEAM(getTeam()).GetTeamTechs()->GetNumTechsKnown();
	if (GC.getGame().isOption("GAMEOPTION_TWEAKED_SCORING"))
		iScore *= GC.getNEW_SCORE_TECH_MULTIPLIER();
	else
		iScore *= GC.getSCORE_TECH_MULTIPLIER();
#else
	int iScore = GET_TEAM(getTeam()).GetTeamTechs()->GetNumTechsKnown() *       GC.getSCORE_TECH_MULTIPLIER();
#endif
	return iScore;
}



int CvPlayer::GetScoreFromFutureTech() const
{
	if(GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
	{
		return 0;
	}

	return m_iScoreFromFutureTech;
}


void CvPlayer::ChangeScoreFromFutureTech(int iChange)
{
	if(iChange != 0)
		m_iScoreFromFutureTech += iChange;
}



int CvPlayer::GetScoreFromScenario1() const
{
	return m_iScenarioScore1;
}
void CvPlayer::ChangeScoreFromScenario1(int iChange)
{
	if(iChange != 0)
		m_iScenarioScore1 += iChange;
}

int CvPlayer::GetScoreFromScenario2() const
{
	return m_iScenarioScore2;
}
void CvPlayer::ChangeScoreFromScenario2(int iChange)
{
	if(iChange != 0)
		m_iScenarioScore2 += iChange;
}

int CvPlayer::GetScoreFromScenario3() const
{
	return m_iScenarioScore3;
}
void CvPlayer::ChangeScoreFromScenario3(int iChange)
{
	if(iChange != 0)
		m_iScenarioScore3 += iChange;
}

int CvPlayer::GetScoreFromScenario4() const
{
	return m_iScenarioScore4;
}
void CvPlayer::ChangeScoreFromScenario4(int iChange)
{
	if(iChange != 0)
		m_iScenarioScore4 += iChange;
}






int CvPlayer::countCityFeatures(FeatureTypes eFeature) const
{
	const CvCity* pLoopCity;
	const CvPlot* pLoopPlot;
	int iCount;
	int iLoop;
	int iI;

	iCount = 0;

	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		for(iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			pLoopPlot = plotCity(pLoopCity->getX(), pLoopCity->getY(), iI);

			if(pLoopPlot != NULL)
			{
				if(pLoopPlot->getFeatureType() == eFeature)
				{
					iCount++;
				}
			}
		}
	}

	return iCount;
}



int CvPlayer::countNumBuildings(BuildingTypes eBuilding) const
{
	const CvCity* pLoopCity;
	int iCount;
	int iLoop;

	iCount = 0;

	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if(pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
		{
			iCount += pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding);
		}
	}

	return iCount;
}



int CvPlayer::countCitiesFeatureSurrounded() const
{
	const CvCity* pLoopCity;
	int iCount;
	int iLoop;

	iCount = 0;

	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if(pLoopCity->IsFeatureSurrounded())
			iCount ++;
	}

	return iCount;
}


bool CvPlayer::IsCityConnectedToCity(CvCity* pCity1, CvCity* pCity2, RouteTypes eRestrictRoute, bool bIgnoreHarbors)
{
	int iPathfinderFlags = GetID() | MOVE_ROUTE_ALLOW_UNEXPLORED;
	if(eRestrictRoute == NO_ROUTE)
	{
		iPathfinderFlags |= MOVE_ANY_ROUTE;
	}
	else
	{

		int iRouteValue = eRestrictRoute + 1;
		iPathfinderFlags |= (iRouteValue << 8);
	}

	if (bIgnoreHarbors)
	{
		GC.getRouteFinder().SetNumExtraChildrenFunc(NULL);
		GC.getRouteFinder().SetExtraChildGetterFunc(NULL);
	}

	GC.getRouteFinder().ForceReset();
	bool bReturnValue = GC.getRouteFinder().GeneratePath(pCity1->getX(), pCity1->getY(), pCity2->getX(), pCity2->getY(), iPathfinderFlags, false);

	if (bIgnoreHarbors)
	{

		GC.getRouteFinder().SetNumExtraChildrenFunc(RouteGetNumExtraChildren);
		GC.getRouteFinder().SetExtraChildGetterFunc(RouteGetExtraChild);
	}

	return bReturnValue;
}


bool CvPlayer::IsCapitalConnectedToPlayer(PlayerTypes ePlayer, RouteTypes eRestrictRoute)
{

	if(!isAlive() || !(GET_PLAYER(ePlayer).isAlive()))
	{
		return false;
	}

	CvCity* pOtherPlayerCapital = GET_PLAYER(ePlayer).getCapitalCity();
	if(pOtherPlayerCapital == NULL)
	{
		return false;
	}

	return IsCapitalConnectedToCity(pOtherPlayerCapital, eRestrictRoute);
}


bool CvPlayer::IsCapitalConnectedToCity(CvCity* pCity, RouteTypes eRestrictRoute)
{
	CvCity* pPlayerCapital = getCapitalCity();
	if(pPlayerCapital == NULL)
	{
		return false;
	}

	return IsCityConnectedToCity(pPlayerCapital, pCity, eRestrictRoute);
}


void CvPlayer::findNewCapital()
{
	CvCity* pOldCapital;
	CvCity* pLoopCity;
	CvCity* pBestCity;
	BuildingTypes eCapitalBuilding;
	int iValue;
	int iBestValue;
	int iLoop;

	eCapitalBuilding = ((BuildingTypes)(getCivilizationInfo().getCivilizationBuildings(GC.getCAPITAL_BUILDINGCLASS())));

	if(eCapitalBuilding == NO_BUILDING)
	{
		return;
	}

	pOldCapital = getCapitalCity();

	iBestValue = 0;
	pBestCity = NULL;

	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if(pLoopCity != pOldCapital)
		{
			if(0 == pLoopCity->GetCityBuildings()->GetNumRealBuilding(eCapitalBuilding))
			{
				iValue = (pLoopCity->getPopulation() * 4);

				int iYieldValueTimes100 = pLoopCity->getYieldRateTimes100(YIELD_FOOD, false);
				iYieldValueTimes100 += (pLoopCity->getYieldRateTimes100(YIELD_PRODUCTION, false) * 3);
				iYieldValueTimes100 += (pLoopCity->getYieldRateTimes100(YIELD_GOLD, false) * 2);
				iValue += (iYieldValueTimes100 / 100);

				iValue += (pLoopCity->getNumGreatPeople() * 2);

				if(iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestCity = pLoopCity;
				}
			}
		}
	}

	if(pBestCity != NULL)
	{
		if(pOldCapital != NULL)
		{
			pOldCapital->GetCityBuildings()->SetNumRealBuilding(eCapitalBuilding, 0);
		}
		CvAssertMsg(!(pBestCity->GetCityBuildings()->GetNumRealBuilding(eCapitalBuilding)), "(pBestCity->getNumRealBuilding(eCapitalBuilding)) did not return false as expected");
		pBestCity->GetCityBuildings()->SetNumRealBuilding(eCapitalBuilding, 1);
	}
}


bool CvPlayer::canRaze(CvCity* pCity, bool bIgnoreCapitals) const
{
	if(GC.getGame().isOption(GAMEOPTION_NO_CITY_RAZING))
	{
		return false;
	}


	if(pCity->getOwner() != GetID())
	{
		return false;
	}


	if(pCity->getOriginalOwner() == GetID())
	{
		return false;
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(pCity->getOwner());
		args->Push(pCity->GetID());

		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "CanRazeOverride", args.get(), bResult))
		{

			if(bResult == true)
			{
				return true;
			}
		}
	}


	CvPlayer* pOriginalOwner = &GET_PLAYER(pCity->getOriginalOwner());
	bool bOriginalCapital =	pCity->getX() == pOriginalOwner->GetOriginalCapitalX() &&
	                        pCity->getY() == pOriginalOwner->GetOriginalCapitalY();

	if(!bIgnoreCapitals && pCity->IsEverCapital() && bOriginalCapital)
	{
		return false;
	}


	if (pCity->GetCityReligions()->IsHolyCityAnyReligion())
	{
		return false;
	}


	ResourceTypes eResource = pCity->plot()->getResourceType();
	if (eResource != NO_RESOURCE)
	{
		CvResourceInfo *pkResource = GC.getResourceInfo(eResource);
		if (pkResource && pkResource->GetRequiredCivilization() != NO_CIVILIZATION)
		{
			return false;
		}
	}

	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(pCity->getOwner());
		args->Push(pCity->GetID());

		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "CanRaze", args.get(), bResult))
		{

			if(bResult == false)
			{
				return false;
			}
		}
	}

	return true;
}



void CvPlayer::raze(CvCity* pCity)
{
	char szBuffer[1024];
	const size_t lenBuffer = 1024;
	int iI;

	if(!canRaze(pCity))
	{
		return;
	}

	CvAssert(pCity->getOwner() == GetID());

	if(GetID() == GC.getGame().getActivePlayer())
	{
		sprintf_s(szBuffer, lenBuffer, GetLocalizedText("TXT_KEY_MISC_DESTROYED_CITY", pCity->getNameKey()).GetCString());
		GC.GetEngineUserInterface()->AddCityMessage(0, pCity->GetIDInfo(), GetID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer                                                                                                                                                   );

	}

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if(iI != GetID() && iI == GC.getGame().getActivePlayer())
			{
				if(pCity->isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
				{
					sprintf_s(szBuffer, lenBuffer, GetLocalizedText("TXT_KEY_MISC_CITY_HAS_BEEN_RAZED_BY", pCity->getNameKey(), getCivilizationDescriptionKey()).GetCString());
					GC.GetEngineUserInterface()->AddCityMessage(0, pCity->GetIDInfo(), ((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer                                                                                                                                                  );
				}
			}
		}
	}

	sprintf_s(szBuffer, lenBuffer, GetLocalizedText("TXT_KEY_MISC_CITY_RAZED_BY", pCity->getNameKey(), getCivilizationShortDescriptionKey()).GetCString());
	GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, GetID(), szBuffer, pCity->getX(), pCity->getY());

	pCity->SetIgnoreCityForHappiness(false);
	DoUpdateHappiness();

	int iPopulationDrop = 1;
	iPopulationDrop *= (100 + GetPlayerTraits()->GetRazeSpeedModifier());
	iPopulationDrop /= 100;
	int iTurnsToRaze = pCity->getPopulation();
	if(iPopulationDrop > 0)
	{
		iTurnsToRaze = (iTurnsToRaze + iPopulationDrop - 1) / iPopulationDrop;
	}

	pCity->ChangeRazingTurns(iTurnsToRaze);

	DoUpdateNextPolicyCost();


	if(GetID() == GC.getGame().getActivePlayer())
	{
		GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
	}
#ifdef AUI_CITIZENS_MID_TURN_ASSIGN_RUNS_SELF_CONSISTENCY
	doSelfConsistencyCheckAllCities();
#endif
}



void CvPlayer::unraze(CvCity* pCity)
{
	if (GetPlayerTraits()->IsNoAnnexing())
	{
		pCity->DoCreatePuppet();
	}
	else
	{
		pCity->DoAnnex();
	}

	pCity->ChangeRazingTurns(-pCity->GetRazingTurns());

	DoUpdateNextPolicyCost();


	if(GetID() == GC.getGame().getActivePlayer())
	{
		GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
	}
}




void CvPlayer::disband(CvCity* pCity)
{
	CvPlot* pPlot = pCity->plot();

	if(getNumCities() == 1)
	{
		setFoundedFirstCity(false);
	}

	GC.getGame().addDestroyedCityName(pCity->getNameKey());

#ifdef AUI_WARNING_FIXES
	for (uint eBuildingType = 0; eBuildingType < GC.getNumBuildingInfos(); eBuildingType++)
#else
	for(int eBuildingType = 0; eBuildingType < GC.getNumBuildingInfos(); eBuildingType++)
#endif
	{
		CvBuildingEntry* buildingInfo = GC.getBuildingInfo((BuildingTypes) eBuildingType);
		if(buildingInfo)
		{

			int iExists = pCity->GetCityBuildings()->GetNumRealBuilding((BuildingTypes) eBuildingType);
			int iPreferredPosition = buildingInfo->GetPreferredDisplayPosition();
			if(iPreferredPosition > 0)
			{
				auto_ptr<ICvCity1> pDllCity(new CvDllCity(pCity));

				if(iExists > 0)
				{

					GC.GetEngineUserInterface()->AddDeferredWonderCommand(WONDER_REMOVED, pDllCity.get(), (BuildingTypes) eBuildingType, 0);
				}
				else
				{

#ifdef AUI_WARNING_FIXES
					if (pCity->getProductionBuilding() == (BuildingTypes)eBuildingType)
#else
					if(pCity->getProductionBuilding() == eBuildingType)
#endif
					{

						if(isWorldWonderClass(buildingInfo->GetBuildingClassInfo()))
						{
							GC.GetEngineUserInterface()->AddDeferredWonderCommand(WONDER_REMOVED, pDllCity.get(), (BuildingTypes) eBuildingType, 0);
						}
					}
				}
			}
		}
	}

	{
		auto_ptr<ICvCity1> pkDllCity(new CvDllCity(pCity));
		gDLL->GameplayCitySetDamage(pkDllCity.get(), 0, pCity->getDamage());
		gDLL->GameplayCityDestroyed(pkDllCity.get(), NO_PLAYER);
	}

	pCity->kill();

	if(pPlot)
	{
		IDInfoVector currentUnits;
		if (pPlot->getUnits(&currentUnits) > 0)
		{
			for (IDInfoVector::const_iterator itr = currentUnits.begin(); itr != currentUnits.end(); ++itr)
			{
				CvUnit* pUnit = ::getUnit(*itr);

				if(pUnit && !pPlot->isValidDomainForLocation(*pUnit))
				{
					if (!pUnit->jumpToNearestValidPlot())
						pUnit->kill(false);
				}
			}
		}

	}
}



bool CvPlayer::canReceiveGoody(CvPlot* pPlot, GoodyTypes eGoody, CvUnit* pUnit) const
{
	CvCity* pCity;
	UnitTypes eUnit;
	bool bTechFound;
	int iI;

	Database::SingleResult kResult;
	CvGoodyInfo kGoodyInfo;
	const bool bResult = DB.SelectAt(kResult, "GoodyHuts", eGoody);
	DEBUG_VARIABLE(bResult);
	CvAssertMsg(bResult, "Cannot find goody info.");
	kGoodyInfo.CacheResult(kResult);

	if(!CvGoodyHuts::IsCanPlayerReceiveGoody(GetID(), eGoody))
	{
		return false;
	}

#ifdef LEKMOD_NEW_ANCIENT_RUIN_REWARDS


	if (kGoodyInfo.isOncePerGame())
	{
		if (CvGoodyHuts::IsGoodyHutOncePerGame(GetID(), eGoody))
		{
			return false;
		}
	}
	
	if(kGoodyInfo.getAfterTurn() > 0)
	{
		if(GC.getGame().getElapsedGameTurns() < kGoodyInfo.getAfterTurn())
		{
			return false;
		}
	}
	if(kGoodyInfo.getBeforeTurn() > 0)
	{
		if(GC.getGame().getElapsedGameTurns() > kGoodyInfo.getBeforeTurn())
		{
			return false;
		}
	}
	

	if(kGoodyInfo.getFoodMin() > 0 || kGoodyInfo.getFoodMax() > 0)
	{
		if (getNumCities() == 0)
		{
			return false;
		}

		if(kGoodyInfo.getFoodMin() > kGoodyInfo.getFoodMax())
		{
			return false;
		}
		
	}
	if(kGoodyInfo.getFaithMin() > 0 || kGoodyInfo.getFaithMax() > 0)
	{
		if(kGoodyInfo.getFaithMin() > kGoodyInfo.getFaithMax())
		{
			return false;
		}
	}

	if(kGoodyInfo.getTileGrowths() > 0)
	{
		if (getNumCities() == 0)
		{
			return false;
		}
	}

	if (kGoodyInfo.getRandomImprovement() > 0)
	{
		if (getNumCities() == 0)
		{
			return false;
		}

		CvCity* pBestCity = findBestCityForGoody(pPlot);

		int iNumTilesToImprove = kGoodyInfo.getRandomImprovement();
		int iNumImprovements = 0;


		for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{


			if (iNumImprovements >= iNumTilesToImprove)
			{
				break;
			}

			CvPlot* pCityPlot = pBestCity->GetCityCitizens()->GetCityPlotFromIndex(iI);	
			if (pCityPlot == NULL || pCityPlot == pBestCity->plot() || pCityPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				continue;
			}


			for (int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
			{
				CvBuildInfo* pkBuildInfo = GC.getBuildInfo((BuildTypes)iJ);
				ImprovementTypes eImprovement = (ImprovementTypes)pkBuildInfo->getImprovement();

				if (!pkBuildInfo || !eImprovement)
				{
					continue;
				}

				if (canGoodyImprovePlot(pCityPlot, ((BuildTypes)iJ)))
				{
					iNumImprovements++;
					break;
				}
			}
		}


		if (iNumImprovements < iNumTilesToImprove)
		{
			return false;
		}

	}

	if (kGoodyInfo.getFreePromotion() != NO_PROMOTION)
	{

		if (pUnit != NULL && !pUnit->IsCombatUnit())
		{
			return false;
		}


		if (kGoodyInfo.getExcludeUnitClass() != NO_UNITCLASS)
		{
			if (pUnit == NULL)
			{
				return false;
			}

			eUnit = pUnit->getUnitType();
			if (eUnit != NO_UNIT)
			{
				if (GC.getUnitInfo(eUnit)->GetUnitClassType() == kGoodyInfo.getExcludeUnitClass())
				{
					return false;
				}
			}
		}


		if (pUnit != NULL && pUnit->isHasPromotion((PromotionTypes)kGoodyInfo.getFreePromotion()))
		{
			return false;
		}
	}

	if (kGoodyInfo.getCityStateInfluence() > 0)
	{

		bool bMetCityState = false;
		for (int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
		{
			PlayerTypes eMinor = (PlayerTypes)iI;
			if (eMinor == NO_PLAYER || eMinor == GetID() || !GET_PLAYER(eMinor).isMinorCiv())
			{
				continue;
			}


			if (GET_TEAM(getTeam()).isAtWar(GET_PLAYER(eMinor).getTeam()))
			{
				continue;
			}
			if (GET_TEAM(getTeam()).isHasMet(GET_PLAYER(eMinor).getTeam()))
			{
				bMetCityState = true;
				break;
			}
		}

		if (!bMetCityState)
		{
			return false;
		}
	}
#else


	if(kGoodyInfo.getExperience() > 0)
	{
		if((pUnit == NULL) || !(pUnit->canAcquirePromotionAny()) || (GC.getGame().getElapsedGameTurns() < 10))
		{
			return false;
		}
	}

	if (kGoodyInfo.getCulture() > 0)
	{
		if (GC.getGame().getElapsedGameTurns() < 12)
		{
			return false;
		}
	}
#endif

	if(kGoodyInfo.getDamagePrereq() > 0)
	{
		if((pUnit == NULL) || (pUnit->getDamage() < ((pUnit->GetMaxHitPoints() * kGoodyInfo.getDamagePrereq()) / 100)))
		{
			return false;
		}
	}

	


	if(kGoodyInfo.isPantheonFaith())
	{
#ifndef LEKMOD_NEW_ANCIENT_RUIN_REWARDS
		if(GC.getGame().getElapsedGameTurns() < 20)
		{
			return false;
		}
		else
		{
			return (!GetReligions()->HasCreatedPantheon() && !GetReligions()->HasCreatedReligion());
		}
#else
		return (!GetReligions()->HasCreatedPantheon() && !GetReligions()->HasCreatedReligion());
#endif
	}

#ifdef LEKMOD_NEW_ANCIENT_RUIN_REWARDS

	if(kGoodyInfo.isReligionFaith())
	{
		return (GetReligions()->HasCreatedPantheon() && !GetReligions()->HasCreatedReligion());
	}
#endif


	if(kGoodyInfo.getProphetPercent() > 0)
	{
		if(GC.getGame().getElapsedGameTurns() < 20)
		{
			return false;
		}
		else
		{
			return (GetReligions()->HasCreatedPantheon() && !GetReligions()->HasCreatedReligion());
		}
	}


	if(kGoodyInfo.getPopulation() > 0)
	{
		if(getNumCities() == 0)
		{
			return false;
		}


		if(IsEmpireUnhappy())
		{
			return false;
		}
	}

























































	if(kGoodyInfo.isRevealUnknownResource())
	{

		if(getCapitalCity() == NULL)
		{
			return false;
		}

		CvResourceInfo* pResource;
		ResourceClassTypes eResourceClassBonus = (ResourceClassTypes) GC.getInfoTypeForString("RESOURCECLASS_BONUS");

		bool bPlayerDoesntKnowOfResource = false;

		int iNumResourceInfos = GC.getNumResourceInfos();
		for(int iResourceLoop = 0; iResourceLoop < iNumResourceInfos; iResourceLoop++)
		{
			pResource = GC.getResourceInfo((ResourceTypes) iResourceLoop);


			if(pResource != NULL && pResource->getResourceClassType() != eResourceClassBonus)
			{
				if(!GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes) pResource->getTechReveal()))
				{
					bPlayerDoesntKnowOfResource = true;
				}
			}
		}


		if(!bPlayerDoesntKnowOfResource)
		{
			return false;
		}
	}


	if(kGoodyInfo.isUpgradeUnit())
	{
		if(pUnit == NULL)
		{
			return false;
		}

		if(pUnit->IsHasBeenPromotedFromGoody())
		{
			return false;
		}

		UnitClassTypes eUpgradeUnitClass = (UnitClassTypes) GC.getUnitInfo(pUnit->getUnitType())->GetGoodyHutUpgradeUnitClass();

		if(eUpgradeUnitClass == NO_UNITCLASS)
		{
			return false;
		}

		UnitTypes eUpgradeUnit = (UnitTypes) getCivilizationInfo().getCivilizationUnits(eUpgradeUnitClass);

		if(eUpgradeUnit == NO_UNIT)
		{
			return false;
		}
	}


	if(kGoodyInfo.isTech())
	{
		bTechFound = false;

		int iNumTechInfos = GC.getNumTechInfos();
		for(iI = 0; iI < iNumTechInfos; iI++)
		{
			const TechTypes eTech = static_cast<TechTypes>(iI);
			CvTechEntry* pkTech = GC.getTechInfo(eTech);
			if(pkTech != NULL && pkTech->IsGoodyTech())
			{
				if(GetPlayerTechs()->CanResearch(eTech))
				{
					bool bUseTech = true;
					ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
					if (pkScriptSystem) 
					{
						CvLuaArgsHandle args;
						args->Push(GetID());
						args->Push(eTech);



						bool bScriptResult = false;
						if (LuaSupport::CallTestAll(pkScriptSystem, "GoodyHutCanResearch", args.get(), bScriptResult)) 
						{
							bUseTech = bResult;
						}
					}

					if(bUseTech)
					{
						bTechFound = true;
					}
					break;
				}
			}
		}

		if(!bTechFound)
		{
			return false;
		}
	}







	if(kGoodyInfo.isBad())
	{
		if((pUnit == NULL) || pUnit->isNoBadGoodies())
		{
			return false;
		}
	}

	if(kGoodyInfo.getUnitClassType() != NO_UNITCLASS)
	{
		eUnit = ((UnitTypes)(getCivilizationInfo().getCivilizationUnits(kGoodyInfo.getUnitClassType())));

		if(eUnit == NO_UNIT)
		{
			return false;
		}

		CvUnitEntry* pUnitInfo = GC.getUnitInfo(eUnit);
		if(pUnitInfo == NULL)
		{
			return false;
		}


		if(pUnitInfo->GetCombat() > 0)
		{
			if(GC.getGame().isGameMultiPlayer() || (GC.getGame().getElapsedGameTurns() < 20))
			{
				return false;
			}
		}


		if(pUnitInfo->GetWorkRate() > 0)
		{

			if(GetMaxNumBuilders() > -1 && GetNumBuilders() >= GetMaxNumBuilders())
			{
				return false;
			}

			bool bHasTechWhichUnlocksImprovement = false;


			int iNumTechInfos = GC.getNumTechInfos();
			int iNumBuildInfos = GC.getNumBuildInfos();
			for(int iTechLoop = 0; iTechLoop < iNumTechInfos; iTechLoop++)
			{
				if(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes) iTechLoop))
				{

					for(int iBuildLoop = 0; iBuildLoop < iNumBuildInfos; iBuildLoop++)
					{
						CvBuildInfo* pkBuildInfo = GC.getBuildInfo((BuildTypes) iBuildLoop);
						if(!pkBuildInfo)
						{
							continue;
						}
						if(pkBuildInfo->getTechPrereq() == (TechTypes) iTechLoop)
						{
							if(pkBuildInfo->getImprovement() != NO_IMPROVEMENT || pkBuildInfo->getRoute() != NO_ROUTE)
							{
								bHasTechWhichUnlocksImprovement = true;
								break;
							}
						}
					}
				}

				if(bHasTechWhichUnlocksImprovement)
				{
					break;
				}
			}


			if(!bHasTechWhichUnlocksImprovement)
			{
				return false;
			}
		}


#ifdef AUI_PLAYER_FIX_VENICE_ONLY_BANS_SETTLERS_NOT_SETTLING
		if ((GetPlayerTraits()->IsNoAnnexing() && pUnitInfo->GetDefaultUnitAIType() == UNITAI_SETTLE) || (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman()))
#else
		if(GetPlayerTraits()->IsNoAnnexing() || (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman()))
#endif
		{
			if(pUnitInfo->IsFound() || pUnitInfo->IsFoundAbroad())
			{
				return false;
			}
		}
	}


	if(kGoodyInfo.getBarbarianUnitClass() != NO_UNITCLASS)
	{
		if(GC.getGame().isOption(GAMEOPTION_NO_BARBARIANS))
		{
			return false;
		}

		if(getNumCities() == 0)
		{
			return false;
		}

		if(getNumCities() == 1)
		{
			pCity = GC.getMap().findCity(pPlot->getX(), pPlot->getY(), NO_PLAYER, getTeam());

			if(pCity != NULL)
			{
				if(plotDistance(pPlot->getX(), pPlot->getY(), pCity->getX(), pCity->getY()) <= (8 - getNumCities()))
				{
					return false;
				}
			}
		}
	}

	return true;
}



void CvPlayer::receiveGoody(CvPlot* pPlot, GoodyTypes eGoody, CvUnit* pUnit)
{
	CvPlot* pLoopPlot;
	CvPlot* pBestPlot = NULL;
	CvString strBuffer;
	CvString strTempBuffer;
	TechTypes eBestTech;
	UnitTypes eUnit;
	int iGold;
	int iOffset;
	int iRange;
	int iBarbCount;
	int iValue;
	int iBestValue;
	int iPass;
	int iDX, iDY;
#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX;
#endif
#ifdef AUI_WARNING_FIXES
	uint iI;
#else
	int iI;
#endif

	CvAssertMsg(canReceiveGoody(pPlot, eGoody, pUnit), "Instance is expected to be able to recieve goody");

	Database::SingleResult kResult;
	CvGoodyInfo kGoodyInfo;
	const bool bResult = DB.SelectAt(kResult, "GoodyHuts", eGoody);
	DEBUG_VARIABLE(bResult);
	CvAssertMsg(bResult, "Cannot find goody info.");
	kGoodyInfo.CacheResult(kResult);

	CvGoodyHuts::DoPlayerReceivedGoody(GetID(), eGoody);

#ifdef AUI_PLAYER_RECEIVE_GOODY_PLOT_MESSAGE_FOR_YIELD
	int iNumYieldBonuses = 0;
#endif

	strBuffer = kGoodyInfo.GetDescription();


	iGold = kGoodyInfo.getGold() + (kGoodyInfo.getNumGoldRandRolls() * GC.getGame().getJonRandNum(kGoodyInfo.getGoldRandAmount(), "Goody Gold Rand"));

	if (iGold != 0)
	{
		GetTreasury()->ChangeGold(iGold);

#ifdef AUI_PLAYER_FIX_RECEIVE_GOODY_MESSAGE
		strBuffer = GetLocalizedText(kGoodyInfo.GetDescriptionKey(), iGold);
#else
		strBuffer += GetLocalizedText("TXT_KEY_MISC_RECEIVED_GOLD", iGold);
#endif
#ifdef AUI_PLAYER_RECEIVE_GOODY_PLOT_MESSAGE_FOR_YIELD
		ReportYieldFromKill(YIELD_GOLD, iGold, pPlot->getX(), pPlot->getY(), iNumYieldBonuses);
		iNumYieldBonuses += 1;

#endif
	}

#ifdef LEKMOD_NEW_ANCIENT_RUIN_REWARDS


	if (kGoodyInfo.isOncePerGame())
	{
		CvGoodyHuts::DoPlayerReceivedGoodyOncePerGame(GetID(), eGoody);
	}



	int iFood = 0;
	if (kGoodyInfo.getFoodMin() > 0 && kGoodyInfo.getFoodMax() > 0)
	{
		int iMinValue = kGoodyInfo.getFoodMin();
		int iMaxValue = kGoodyInfo.getFoodMax();
		iFood = GC.getGame().getJonRandNum(iMaxValue - iMinValue + 1, "Goody Food Rand") + iMinValue;

		if (iFood > 0)
		{

			CvCity* pBestCity = findBestCityForGoody(pPlot);

			if (pBestCity != NULL)
			{

				if (kGoodyInfo.getIncreasePerPop() > 0)
				{
					iFood += kGoodyInfo.getIncreasePerPop() * pBestCity->getPopulation();
				}



				pBestCity->changeFood(iFood);
				if (GetID() == GC.getGame().getActivePlayer())
				{

					strBuffer = GetLocalizedText(kGoodyInfo.GetDescriptionKey(), iFood);


					char text[256] = { 0 };
					float fDelay = GC.getPOST_COMBAT_TEXT_DELAY() * 3;
					text[0] = NULL;
					sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_FOOD]", iFood);
					GC.GetEngineUserInterface()->AddPopupText(pPlot->getX(), pPlot->getY(), text, fDelay);
				}


				TestMidTurnPopGrowth(pBestCity, true                            );
			}
		}
	}


	int iRandomFaith = 0;
	if (kGoodyInfo.getFaithMin() > 0 && kGoodyInfo.getFaithMax() > 0)
	{
		int iMinValue = kGoodyInfo.getFaithMin();
		int iMaxValue = kGoodyInfo.getFaithMax();
		iRandomFaith = GC.getGame().getJonRandNum(iMaxValue - iMinValue + 1, "Goody Faith Rand") + iMinValue;
		if (iRandomFaith > 0)
		{
			ChangeFaith(iRandomFaith);
			if (GetID() == GC.getGame().getActivePlayer())
			{

				char text[256] = { 0 };
				float fDelay = GC.getPOST_COMBAT_TEXT_DELAY() * 3;
				text[0] = NULL;
				sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_PEACE]", iRandomFaith);
				GC.GetEngineUserInterface()->AddPopupText(pPlot->getX(), pPlot->getY(), text, fDelay);
			}

			strBuffer = GetLocalizedText(kGoodyInfo.GetDescriptionKey(), iRandomFaith);
		}
	}


	if (kGoodyInfo.getTileGrowths() > 0)
	{
		int iTiles = kGoodyInfo.getTileGrowths();
		CvCity* pBestCity = findBestCityForGoody(pPlot);

		if (pBestCity != NULL)
		{
			for (int iI = 0; iI < iTiles; iI++)
			{

				CvPlot* pPlotToAcquire = pBestCity->GetNextBuyablePlot();


				if (pPlotToAcquire)
				{
					pBestCity->DoAcquirePlot(pPlotToAcquire->getX(), pPlotToAcquire->getY());
				}
			}
		}
	}


	if (kGoodyInfo.getRandomImprovement() > 0)
	{
		CvCity* pBestCity = findBestCityForGoody(pPlot);
		if (pBestCity != NULL)
		{

			int iNumTilesToImprove = kGoodyInfo.getRandomImprovement();
			int iNumImprovements = 0;




			while (iNumImprovements < iNumTilesToImprove)
			{


				for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
				{
					if (iNumImprovements >= iNumTilesToImprove)
					{
						break;
					}


					int iRandPlot = GC.getGame().getJonRandNum(NUM_CITY_PLOTS, "Goody Improvement Rand");
					CvPlot* pCityPlot = pBestCity->GetCityCitizens()->GetCityPlotFromIndex(iRandPlot);
					if (pCityPlot == NULL || pCityPlot == pBestCity->plot() || pCityPlot->getImprovementType() != NO_IMPROVEMENT)
					{
						continue;
					}


					for (int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
					{
						CvBuildInfo* pkBuildInfo = GC.getBuildInfo((BuildTypes)iJ);
						ImprovementTypes eImprovement = (ImprovementTypes)pkBuildInfo->getImprovement();
						CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);

						if (!pkBuildInfo || !eImprovement)
						{
							continue;
						}

						if (canGoodyImprovePlot(pCityPlot, ((BuildTypes)iJ)))
						{
							pCityPlot->setImprovementType(eImprovement);
							if (kGoodyInfo.getRandomImprovement() == 1)
							{

								strBuffer = GetLocalizedText(kGoodyInfo.GetDescriptionKey(), pkBuildInfo->GetType(), GC.getImprovementInfo(eImprovement)->GetTextKey(), GC.getResourceInfo(pCityPlot->getResourceType())->GetIconString(), GC.getResourceInfo(pCityPlot->getResourceType())->GetTextKey());

							}

							if (pkImprovementInfo->GetCultureBombRadius() > 0)
							{

								UnitTypes eDummyUnit = pUnit->getUnitType();
								CvUnit* pDummyUnit = initUnit(eDummyUnit, pCityPlot->getX(), pCityPlot->getY(), NO_UNITAI, NO_DIRECTION, true, true);
								pDummyUnit->PerformCultureBomb(pkImprovementInfo->GetCultureBombRadius());
								pDummyUnit->kill(true);

							}

							if (pkImprovementInfo->GetCultureBombRadiusNeutral() > 0)
							{
								UnitTypes eDummyUnit = pUnit->getUnitType();
								CvUnit* pDummyUnit = initUnit(eDummyUnit, pCityPlot->getX(), pCityPlot->getY(), NO_UNITAI, NO_DIRECTION, true, true);
								pDummyUnit->PerformNeutralCultureBomb(pkImprovementInfo->GetCultureBombRadiusNeutral());
								pDummyUnit->kill(true);

							}

							iNumImprovements++;
							break;
						}
					}
				}
			}
		}
	}


	if (kGoodyInfo.getFreePromotion() != NO_PROMOTION)
	{
		pUnit->setHasPromotion((PromotionTypes)kGoodyInfo.getFreePromotion(), true);
		strBuffer = GetLocalizedText(kGoodyInfo.GetDescriptionKey(), GC.getPromotionInfo((PromotionTypes)kGoodyInfo.getFreePromotion())->GetTextKey());
	}


	if (kGoodyInfo.getCityStateInfluence() > 0)
	{
		int iDistance = -1;
		PlayerTypes eClosestCityState = NO_PLAYER;

		for (int iPlayerLoop = 0; iPlayerLoop < MAX_MINOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes ePlayer = (PlayerTypes)iPlayerLoop;
			if (ePlayer == NO_PLAYER)
			{
				continue;
			}
			if (GET_PLAYER(ePlayer).isAlive() && GET_PLAYER(ePlayer).isMinorCiv())
			{

				if (!GET_TEAM(getTeam()).isHasMet(GET_PLAYER(ePlayer).getTeam()))
				{
					continue;
				}

				if (GET_TEAM(getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
				{
					continue;
				}


				int iNewDistance = plotDistance(pPlot->getX(), pPlot->getY(), GET_PLAYER(ePlayer).getCapitalCity()->getX(), GET_PLAYER(ePlayer).getCapitalCity()->getY());
				if (iDistance == -1 || iNewDistance < iDistance)
				{
					iDistance = iNewDistance;
					eClosestCityState = ePlayer;
				}
			}
		}

		if (eClosestCityState != NO_PLAYER)
		{
			
			GET_PLAYER(eClosestCityState).GetMinorCivAI()->ChangeFriendshipWithMajor(GetID(), kGoodyInfo.getCityStateInfluence());
			strBuffer = GetLocalizedText(kGoodyInfo.GetDescriptionKey(), GET_PLAYER(eClosestCityState).getCivilizationShortDescriptionKey(), kGoodyInfo.getCityStateInfluence());
		}


	}


#endif


	if(kGoodyInfo.getPopulation() > 0)
	{
		int iDistance;
		int iBestCityDistance = -1;
		CvCity* pBestCity = NULL;

		CvCity* pLoopCity;
		int iLoop;

		for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			iDistance = plotDistance(pPlot->getX(), pPlot->getY(), pLoopCity->getX(), pLoopCity->getY());

			if(iBestCityDistance == -1 || iDistance < iBestCityDistance)
			{
				iBestCityDistance = iDistance;
				pBestCity = pLoopCity;
			}
		}

		if(pBestCity != NULL)
		{
			pBestCity->changePopulation(kGoodyInfo.getPopulation());
		}
	}


	int iCulture = kGoodyInfo.getCulture();
	if(iCulture > 0)
	{

		iCulture *= GC.getGame().getGameSpeedInfo().getCulturePercent();
		iCulture /= 100;

		changeJONSCulture(iCulture);

#ifdef UPDATE_CULTURE_NOTIFICATION_DURING_TURN

		if (isAlive() && isHuman() && getNumCities() > 0)
		{
			TestMidTurnPolicyNotification();
		}
#endif
#ifdef AUI_PLAYER_FIX_RECEIVE_GOODY_MESSAGE
		strBuffer = GetLocalizedText(kGoodyInfo.GetDescriptionKey(), iCulture);
#endif
#ifdef AUI_PLAYER_RECEIVE_GOODY_PLOT_MESSAGE_FOR_YIELD
		ReportYieldFromKill(YIELD_CULTURE, iCulture, pPlot->getX(), pPlot->getY(), iNumYieldBonuses);
		iNumYieldBonuses += 1;
#endif
	}


	int iFaith = kGoodyInfo.getFaith();
	if(iFaith > 0)
	{

		iFaith *= GC.getGame().getGameSpeedInfo().getFaithPercent();
		iFaith /= 100;

		ChangeFaith(iFaith);
#ifdef AUI_PLAYER_FIX_RECEIVE_GOODY_MESSAGE
		strBuffer = GetLocalizedText(kGoodyInfo.GetDescriptionKey(), iFaith);
#endif
#ifdef AUI_PLAYER_RECEIVE_GOODY_PLOT_MESSAGE_FOR_YIELD
		ReportYieldFromKill(YIELD_FAITH, iFaith, pPlot->getX(), pPlot->getY(), iNumYieldBonuses);
		iNumYieldBonuses += 1;
#endif
	}
#ifndef LEKMOD_NEW_ANCIENT_RUIN_REWARDS

	bool bPantheon = kGoodyInfo.isPantheonFaith();
	if(bPantheon)
	{

		iFaith = GC.getGame().GetGameReligions()->GetMinimumFaithNextPantheon() + 2 * GC.getRELIGION_GAME_FAITH_DELTA_NEXT_PANTHEON();
		int iDivisor =        GC.getGOLD_PURCHASE_VISIBLE_DIVISOR();
		iFaith /= iDivisor;
		iFaith *= iDivisor;
		ChangeFaith(iFaith);
#ifdef AUI_PLAYER_FIX_RECEIVE_GOODY_MESSAGE
		strBuffer = GetLocalizedText(kGoodyInfo.GetDescriptionKey(), iFaith);
#endif
#ifdef AUI_PLAYER_RECEIVE_GOODY_PLOT_MESSAGE_FOR_YIELD
		ReportYieldFromKill(YIELD_FAITH, iFaith, pPlot->getX(), pPlot->getY(), iNumYieldBonuses);
		iNumYieldBonuses += 1;
#endif
	}
#endif


	int iProphetPercent = kGoodyInfo.getProphetPercent();
	if(iProphetPercent > 0)
	{
		iFaith = GetReligions()->GetCostNextProphet(false                            , true                              ) * iProphetPercent / 100;
		int iDivisor =        GC.getGOLD_PURCHASE_VISIBLE_DIVISOR();
		iFaith /= iDivisor;
		iFaith *= iDivisor;
		ChangeFaith(iFaith);
#ifdef AUI_PLAYER_FIX_RECEIVE_GOODY_MESSAGE
		strBuffer = GetLocalizedText(kGoodyInfo.GetDescriptionKey(), iFaith);
#endif
#ifdef AUI_PLAYER_RECEIVE_GOODY_PLOT_MESSAGE_FOR_YIELD
		ReportYieldFromKill(YIELD_FAITH, iFaith, pPlot->getX(), pPlot->getY(), iNumYieldBonuses);
		iNumYieldBonuses += 1;
#endif
	}


	if(kGoodyInfo.getRevealNearbyBarbariansRange() > 0)
	{

		const int iBarbCampDistance = kGoodyInfo.getRevealNearbyBarbariansRange();
#ifdef AUI_HEXSPACE_DX_LOOPS
		for (iDY = -iBarbCampDistance; iDY <= iBarbCampDistance; iDY++)
		{
			iMaxDX = iBarbCampDistance - MAX(0, iDY);
			for (iDX = -iBarbCampDistance - MIN(0, iDY); iDX <= iMaxDX; iDX++)
#else
		for(iDX = -(iBarbCampDistance); iDX <= iBarbCampDistance; iDX++)
		{
			for(iDY = -(iBarbCampDistance); iDY <= iBarbCampDistance; iDY++)
#endif
			{
				CvPlot* pNearbyBarbarianPlot = plotXY(pPlot->getX(), pPlot->getY(), iDX, iDY);
				if(pNearbyBarbarianPlot != NULL)
				{
#ifndef AUI_HEXSPACE_DX_LOOPS
#ifdef AUI_FIX_HEX_DISTANCE_INSTEAD_OF_PLOT_DISTANCE
					if (hexDistance(iDX, iDY) <= iBarbCampDistance)
#else
					if(plotDistance(pNearbyBarbarianPlot->getX(), pNearbyBarbarianPlot->getY(), pPlot->getX(), pPlot->getY()) <= iBarbCampDistance)
#endif
#endif
					{
						if(pNearbyBarbarianPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
						{

							pNearbyBarbarianPlot->setRevealed(getTeam(), true);

							pNearbyBarbarianPlot->setRevealedImprovementType(getTeam(), pNearbyBarbarianPlot->getImprovementType());
						}
					}
				}
			}
		}
	}


	iRange = kGoodyInfo.getMapRange();

	if(iRange > 0)
	{
		iOffset = kGoodyInfo.getMapOffset();

		if(iOffset > 0)
		{
			iBestValue = 0;
			pBestPlot = NULL;

			int iRandLimit;

#ifdef AUI_HEXSPACE_DX_LOOPS
			for (iDY = -iOffset; iDY <= iOffset; iDY++)
			{
				iMaxDX = iOffset - MAX(0, iDY);
				for (iDX = -iOffset - MIN(0, iDY); iDX <= iMaxDX; iDX++)
				{
					pLoopPlot = plotXY(pPlot->getX(), pPlot->getY(), iDX, iDY);
#else
			for(iDX = -(iOffset); iDX <= iOffset; iDX++)
			{
				for(iDY = -(iOffset); iDY <= iOffset; iDY++)
				{
					pLoopPlot = plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iDX, iDY, iOffset);
#endif

					if(pLoopPlot != NULL)
					{
						if(!(pLoopPlot->isRevealed(getTeam())))
						{

							if(pPlot->isWater())
								iRandLimit = 10;
							else
								iRandLimit = 10000;

							iValue = (1 + GC.getGame().getJonRandNum(iRandLimit, "Goody Map"));

#ifdef AUI_FIX_HEX_DISTANCE_INSTEAD_OF_PLOT_DISTANCE
							iValue *= hexDistance(iDX, iDY);
#else
							iValue *= plotDistance(pPlot->getX(), pPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY());
#endif

							if(iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = pLoopPlot;
							}
						}
					}
				}
			}
		}

		if(pBestPlot == NULL)
		{
			pBestPlot = pPlot;
		}

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
				pLoopPlot = plotXY(pBestPlot->getX(), pBestPlot->getY(), iDX, iDY);

				if(pLoopPlot != NULL)
				{
#ifndef AUI_HEXSPACE_DX_LOOPS
#ifdef AUI_FIX_HEX_DISTANCE_INSTEAD_OF_PLOT_DISTANCE
					if (hexDistance(iDX, iDY) <= iRange)
#else
					if (plotDistance(pBestPlot->getX(), pBestPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY()) <= iRange)
#endif
#endif
					{
						if(GC.getGame().getJonRandNum(100, "Goody Map") < kGoodyInfo.getMapProb())
						{
							pLoopPlot->setRevealed(getTeam(), true);
						}
					}
				}
			}
		}
	}


	if (pUnit != NULL)
	{
#ifdef LEKMOD_NEW_ANCIENT_RUIN_REWARDS
		if (kGoodyInfo.getExperience() > 0)
		{
			pUnit->changeExperience(kGoodyInfo.getExperience());
			pUnit->testPromotionReady();
			
		}
#else
		pUnit->changeExperience(kGoodyInfo.getExperience());
#endif
	}


	if(pUnit != NULL)
	{
		pUnit->changeDamage(-(kGoodyInfo.getHealing()));
	}


	if(kGoodyInfo.isRevealUnknownResource())
	{
		if(getCapitalCity() != NULL)
		{
			CvCity* pCapital = getCapitalCity();

			CvPlot* pResourcePlot;
			int iResourceDistance;
			TechTypes eRevealTech;
			int iResourceCost;
			int iBestResourceCost = -1;
			ResourceTypes eResource;
			ResourceTypes eBestResource = NO_RESOURCE;
			CvPlot* pBestResourcePlot = NULL;

			ResourceClassTypes eResourceClassBonus;


#ifdef AUI_WARNING_FIXES
			for (uint iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
#else
			for(int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
#endif
			{
				pResourcePlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);
				eResource = pResourcePlot->getResourceType();

				if(eResource != NO_RESOURCE)
				{
					CvResourceInfo& pResource = *GC.getResourceInfo(eResource);
					eResourceClassBonus = (ResourceClassTypes) GC.getInfoTypeForString("RESOURCECLASS_BONUS");


					if(pResource.getResourceClassType() != eResourceClassBonus)
					{

						if(!pResourcePlot->IsResourceForceReveal(getTeam()))
						{

							eRevealTech = (TechTypes) pResource.getTechReveal();
							if(!GET_TEAM(getTeam()).GetTeamTechs()->HasTech(eRevealTech))
							{
								iResourceDistance = plotDistance(pResourcePlot->getX(), pResourcePlot->getY(), pCapital->getX(), pCapital->getY());


								if(iResourceDistance <= 10)
								{
									iResourceCost = GC.getTechInfo(eRevealTech)->GetResearchCost();


									if(iBestResourceCost == -1 || iResourceCost < iBestResourceCost)
									{
										iBestResourceCost = iResourceCost;
										eBestResource = eResource;
										pBestResourcePlot = pResourcePlot;
									}
								}
							}
						}
					}
				}
			}

			CvAssert(pBestResourcePlot);


			if(pBestResourcePlot != NULL)
			{
				pBestResourcePlot->setRevealed(getTeam(), true);
				pBestResourcePlot->SetResourceForceReveal(getTeam(), true);


				if(getTeam() == GC.getGame().getActiveTeam())
				{
					pBestResourcePlot->setLayoutDirty(true);
				}


				CvPlot* pAdjacentPlot;
				for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; iDirectionLoop++)
				{
					pAdjacentPlot = plotDirection(pBestResourcePlot->getX(), pBestResourcePlot->getY(), ((DirectionTypes) iDirectionLoop));

					if(pAdjacentPlot != NULL)
					{
						pAdjacentPlot->setRevealed(getTeam(), true);
					}
				}

				CvString strTempString;
				strTempString.Format(" (%s)", GC.getResourceInfo(eBestResource)->GetDescription());
				strBuffer += strTempString;
			}
		}
	}


	if(kGoodyInfo.isUpgradeUnit())
	{
		UnitClassTypes eUpgradeUnitClass = NO_UNITCLASS;
		UnitTypes eUpgradeUnit = NO_UNIT;

		if(pUnit != NULL)
		{
			eUpgradeUnitClass = (UnitClassTypes) pUnit->getUnitInfo().GetGoodyHutUpgradeUnitClass();
			eUpgradeUnit = (UnitTypes) getCivilizationInfo().getCivilizationUnits(eUpgradeUnitClass);
		}
		
		if(eUpgradeUnit != NO_UNIT)
		{



			UnitAITypes currentAIDefault = pUnit->AI_getUnitAIType();
			UnitAITypes newAIDefault = (UnitAITypes)GC.getUnitInfo(eUpgradeUnit)->GetDefaultUnitAIType();
			if(currentAIDefault == UNITAI_EXPLORE)
			{
				if(newAIDefault == UNITAI_EXPLORE || newAIDefault == UNITAI_ATTACK || newAIDefault == UNITAI_DEFENSE || newAIDefault == UNITAI_FAST_ATTACK || newAIDefault == UNITAI_COUNTER)
				{
					newAIDefault = UNITAI_EXPLORE;
				}
			}

			CvUnit* pNewUnit = initUnit(eUpgradeUnit, pPlot->getX(), pPlot->getY(), newAIDefault, NO_DIRECTION, false, false, 0, pUnit->GetNumGoodyHutsPopped());
			pUnit->finishMoves();
			pUnit->SetBeenPromotedFromGoody(true);

			CvAssert(pNewUnit);
			if (pNewUnit != NULL)
			{
				pNewUnit->convert(pUnit, true);
#if defined(LEKMOD_CONVERT_PROMOTIONS_UPGRADE)
				pUnit->ConvertPromotions(pUnit, pNewUnit);
#endif
				pNewUnit->setupGraphical();

				ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
				if (pkScriptSystem)
				{
					CvLuaArgsHandle args;
					args->Push(GetID());
					args->Push(pUnit->GetID());
					args->Push(pNewUnit->GetID());
					args->Push(true);

					bool bScriptResult;
					LuaSupport::CallHook(pkScriptSystem, "UnitUpgraded", args.get(), bScriptResult);
				}
			}
			else
				pUnit->kill(false);


			GC.GetEngineUserInterface()->SetDontShowPopups(false);
		}
	}


	if(kGoodyInfo.isTech())
	{
		iBestValue = 0;
		eBestTech = NO_TECH;

		for(iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			const TechTypes eTech = static_cast<TechTypes>(iI);
			CvTechEntry* pkTech = GC.getTechInfo(eTech);
			if(pkTech != NULL && pkTech->IsGoodyTech())
			{
				if(GetPlayerTechs()->CanResearch(eTech))
				{
					bool bUseTech = true;

					ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
					if (pkScriptSystem)
					{
						CvLuaArgsHandle args;
						args->Push(GetID());
						args->Push(eTech);



						bool bScriptResult = false;
						if (LuaSupport::CallTestAll(pkScriptSystem, "GoodyHutCanResearch", args.get(), bScriptResult))
						{
							bUseTech = bScriptResult;
						}
					}

					if(bUseTech)
					{
						iValue = (1 + GC.getGame().getJonRandNum(10000, "Goody Tech"));

						if(iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestTech = eTech;
						}
					}
				}
			}
		}

		CvAssertMsg(eBestTech != NO_TECH, "BestTech is not assigned a valid value");

		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem) 
		{
			CvLuaArgsHandle args;
			args->Push(GetID());
			args->Push(eBestTech);

			bool bScriptResult;
			LuaSupport::CallHook(pkScriptSystem, "GoodyHutTechResearched", args.get(), bScriptResult);
		}

		GET_TEAM(getTeam()).setHasTech(eBestTech, true, GetID(), true, true);
		GET_TEAM(getTeam()).GetTeamTechs()->SetNoTradeTech(eBestTech, true);
	}


	if(kGoodyInfo.getUnitClassType() != NO_UNITCLASS)
	{
		eUnit = (UnitTypes)getCivilizationInfo().getCivilizationUnits(kGoodyInfo.getUnitClassType());

		if(eUnit != NO_UNIT)
		{
			CvUnit* pNewUnit = initUnit(eUnit, pPlot->getX(), pPlot->getY());

			if(pNewUnit && pUnit && pUnit->AreUnitsOfSameType(*pNewUnit))
			{
				pBestPlot = NULL;
				iBestValue = INT_MAX;
				const int iPopRange = 2;
#ifdef AUI_HEXSPACE_DX_LOOPS
				for (iDY = -iPopRange; iDY <= iPopRange; iDY++)
				{
					iMaxDX = iPopRange - MAX(0, iDY);
					for (iDX = -iPopRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
					{
						pLoopPlot = plotXY(pPlot->getX(), pPlot->getY(), iDX, iDY);
#else
				for(iDX = -(iPopRange); iDX <= iPopRange; iDX++)
				{
					for(iDY = -(iPopRange); iDY <= iPopRange; iDY++)
					{
						pLoopPlot	= plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iDX, iDY, iPopRange);
#endif
						if(pLoopPlot != NULL)
						{
							if(pLoopPlot->isValidDomainForLocation(*pNewUnit))
							{
								if(pNewUnit->canMoveInto(*pLoopPlot))
								{
									if(pLoopPlot->getNumFriendlyUnitsOfType(pUnit) < GC.getPLOT_UNIT_LIMIT())
									{
										if(pNewUnit->canEnterTerritory(pLoopPlot->getTeam()) && !pNewUnit->isEnemy(pLoopPlot->getTeam(), pLoopPlot))
										{
											if((pNewUnit->getDomainType() != DOMAIN_AIR) || pLoopPlot->isFriendlyCity(*pNewUnit, true))
											{
												if(pLoopPlot->isRevealed(getTeam()))
												{
													iValue = 1 + GC.getGame().getJonRandNum(6, "spawn goody unit that would over-stack");

#ifdef AUI_FIX_HEX_DISTANCE_INSTEAD_OF_PLOT_DISTANCE
													if (hexDistance(iDX, iDY) > 1)
#else
													if(plotDistance(pPlot->getX(),pPlot->getY(),pLoopPlot->getX(),pLoopPlot->getY()) > 1)
#endif
													{
														iValue += 12;
													}

													if(pLoopPlot->area() != pPlot->area())
													{
														iValue *= 10;
													}

													if(iValue < iBestValue)
													{
														iBestValue = iValue;
														pBestPlot = pLoopPlot;
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
				if(pBestPlot != NULL)
				{
					bool bVis = pBestPlot->isVisibleToWatchingHuman();
					pNewUnit->setXY(pBestPlot->getX(), pBestPlot->getY(), false, true, true && bVis, true);
					pNewUnit->SetPosition(pBestPlot);
					pNewUnit->finishMoves();
				}
				else
				{
					pNewUnit->kill(false);
				}
			}
		}
	}


	if(kGoodyInfo.getBarbarianUnitClass() != NO_UNITCLASS)
	{
		iBarbCount = 0;

		eUnit = (UnitTypes)GET_PLAYER(BARBARIAN_PLAYER).getCivilizationInfo().getCivilizationUnits(kGoodyInfo.getBarbarianUnitClass());

		if(eUnit != NO_UNIT)
		{
			for(iPass = 0; iPass < 10; iPass++)
			{
				if(iBarbCount < kGoodyInfo.getMinBarbarians())
				{
					for(iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
					{
						pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

						if(pLoopPlot != NULL)
						{
							if(pLoopPlot->getArea() == pPlot->getArea())
							{
								if(!(pLoopPlot->isImpassable()) && !pLoopPlot->isMountain() && !(pLoopPlot->getPlotCity()))
								{
									if(pLoopPlot->getNumUnits() == 0)
									{
										if((iPass > 0) || (GC.getGame().getJonRandNum(100, "Goody Barbs") < kGoodyInfo.getBarbarianUnitProb()))
										{
											GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pLoopPlot->getX(), pLoopPlot->getY(), ((pLoopPlot->isWater()) ? UNITAI_ATTACK_SEA : UNITAI_ATTACK));
											iBarbCount++;

											if((iPass > 0) && (iBarbCount == kGoodyInfo.getMinBarbarians()))
											{
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
		}
	}

	if(!strBuffer.empty() && GC.getGame().getActivePlayer() == GetID())
	{
		GC.GetEngineUserInterface()->AddPlotMessage(0, pPlot->GetPlotIndex(), GetID(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer);
	}


	if(GetID() == GC.getGame().getActivePlayer())
	{
		GC.getMap().updateDeferredFog();

		bool bDontShowRewardPopup = GC.GetEngineUserInterface()->IsOptionNoRewardPopups();


		if(!GC.getGame().isNetworkMultiPlayer() && !bDontShowRewardPopup)
		{
			int iSpecialValue = 0;

			if (iGold > 0)
				iSpecialValue = iGold;
			else if (iCulture > 0)
				iSpecialValue = iCulture;
			else if (iFaith > 0)
				iSpecialValue = iFaith;
#ifdef LEKMOD_NEW_ANCIENT_RUIN_REWARDS
			else if (iRandomFaith > 0)
				iSpecialValue = iRandomFaith;
			else if (iFood > 0)
				iSpecialValue = iFood;
#endif
			CvPopupInfo kPopupInfo(BUTTONPOPUP_GOODY_HUT_REWARD, eGoody, iSpecialValue);
			GC.GetEngineUserInterface()->AddPopup(kPopupInfo);

			CancelActivePlayerEndTurn();
		}
	}
}
#ifdef LEKMOD_NEW_ANCIENT_RUIN_REWARDS

bool CvPlayer::canGoodyImprovePlot(CvPlot* pPlot, BuildTypes eBuild) const
{
	if (pPlot == NULL)
	{
		return false;
	}


	if (pPlot->getOwner() != GetID())
	{
		return false;
	}


	ResourceTypes eResource = pPlot->getResourceType(getTeam());

	if (!eResource || eResource == NO_RESOURCE)
	{
		return false;
	}

	


	CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
	if (!pkBuildInfo)
	{
		return false;
	}

	ImprovementTypes eImprovement = (ImprovementTypes)pkBuildInfo->getImprovement();
	CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);


	if (pPlot->getImprovementType() != NO_IMPROVEMENT)
	{
		return false;
	}


	if (!pkImprovementInfo)
	{
		return false;
	}

	if (!pkImprovementInfo->IsImprovementResourceTrade(eResource))
	{
		return false;
	}


	if (pkImprovementInfo->IsCreatedByGreatPerson())
	{
		return false;
	}
	

	if (pPlot->getFeatureType() != NO_FEATURE)
	{
		FeatureTypes eFeature = pPlot->getFeatureType();
		CvFeatureInfo* pkFeatureInfo = GC.getFeatureInfo(eFeature);
		if (pkFeatureInfo)
		{
			if (pkBuildInfo->isFeatureRemove(eFeature))
			{
				return false;
			}
		}
	}

	if (canBuildNoTech(pPlot, eBuild))
	{
		return true;
	}

	return false;
}

CvCity* CvPlayer::findBestCityForGoody(CvPlot* pPlot) const
{
	int iDistance;
	int iBestCityDistance = -1;
	CvCity* pBestCity = NULL;


	PlayerTypes ePlayer = GetID();
	CvPlayer* pPlayer = &GET_PLAYER(ePlayer);

	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop))
	{
		iDistance = plotDistance(pPlot->getX(), pPlot->getY(), pLoopCity->getX(), pLoopCity->getY());

		if (iBestCityDistance == -1 || iDistance < iBestCityDistance)
		{
			iBestCityDistance = iDistance;
			pBestCity = pLoopCity;
		}
	}

	return pBestCity;
	
}
#endif

void CvPlayer::doGoody(CvPlot* pPlot, CvUnit* pUnit)
{
	CvHandicapInfo& playerHandicapInfo = getHandicapInfo();

	GoodyTypes eGoody;

	CvAssertMsg(pPlot->isGoody(), "pPlot->isGoody is expected to be true");

	if(!isBarbarian())
	{
		m_bEverPoppedGoody = true;
		pPlot->removeGoody();


		if(isMinorCiv())
		{
			return;
		}


		if(playerHandicapInfo.getNumGoodies() > 0)
		{

			int iValidGoodiesLoop;
			bool bValid;

			std::vector<GoodyTypes> avValidGoodies;
			for(int iGoodyLoop = 0; iGoodyLoop < playerHandicapInfo.getNumGoodies(); iGoodyLoop++)
			{
				eGoody = (GoodyTypes) playerHandicapInfo.getGoodies(iGoodyLoop);
				bValid = false;


				for(iValidGoodiesLoop = 0; iValidGoodiesLoop < (int) avValidGoodies.size(); iValidGoodiesLoop++)
				{
					if(avValidGoodies[iValidGoodiesLoop] == eGoody)
					{
						avValidGoodies.push_back(eGoody);
						bValid = true;
						break;
					}
				}

				if(bValid)
					continue;

				if(canReceiveGoody(pPlot, eGoody, pUnit))
				{
					avValidGoodies.push_back(eGoody);
				}
			}


			if(avValidGoodies.size() > 0)
			{
#ifdef AUI_PLAYER_FIX_GOODY_HUT_PICKER
				if (pUnit && GC.getGame().getActivePlayer() == GetID() && pUnit->isHasPromotion((PromotionTypes)GC.getPROMOTION_GOODY_HUT_PICKER()))
				{
#else
				if (pUnit && pUnit->isHasPromotion((PromotionTypes)GC.getPROMOTION_GOODY_HUT_PICKER()))
				{
					if(GC.getGame().getActivePlayer() == GetID())
#endif
					{
						CvPopupInfo kPopupInfo(BUTTONPOPUP_CHOOSE_GOODY_HUT_REWARD, GetID(), pUnit->GetID());
						GC.GetEngineUserInterface()->AddPopup(kPopupInfo);

						CancelActivePlayerEndTurn();
					}
				}
				else
				{
					int iRand = GC.getGame().getJonRandNum(avValidGoodies.size(), "Picking a Goody result");
					eGoody = (GoodyTypes) avValidGoodies[iRand];
					receiveGoody(pPlot, eGoody, pUnit);
				}
				
				if (pUnit && isHuman() && !GC.getGame().isGameMultiPlayer())
				{
					pUnit->ChangeNumGoodyHutsPopped(pUnit->GetNumGoodyHutsPopped() + 1);
					if (pUnit->isHasPromotion((PromotionTypes)GC.getPROMOTION_GOODY_HUT_PICKER()) && pUnit->GetNumGoodyHutsPopped() >= 5)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_25);
					}
				}
			}

			pPlot->AddArchaeologicalRecord(CvTypes::getARTIFACT_ANCIENT_RUIN(), m_eID, NO_PLAYER);
		}
	}
}



void CvPlayer::AwardFreeBuildings(CvCity* pCity)
{

	int iNumFreePietyGardens = GetNumCitiesFreePietyGardens();
	if (iNumFreePietyGardens > 0)
	{
		BuildingTypes ePietyGarden = pCity->ChooseFreeGardenBuilding();
		if (ePietyGarden != NO_BUILDING)
		{
			pCity->GetCityBuildings()->SetNumFreeBuilding(ePietyGarden, 0);
			pCity->GetCityBuildings()->SetNumFreeBuilding(ePietyGarden, 1);
		}
		ChangeNumCitiesFreePietyGardens(-1);
	}

	int iNumFreeAestheticsSchools = GetNumCitiesFreeAestheticsSchools();
	if (iNumFreeAestheticsSchools > 0)
	{
		BuildingTypes eAestheticsSchool = (BuildingTypes)GC.getInfoTypeForString("BUILDING_SCRIPTORIUM", true);
		if (eAestheticsSchool != NO_BUILDING)
		{
			pCity->GetCityBuildings()->SetNumFreeBuilding(eAestheticsSchool, 1);
		}
		eAestheticsSchool = (BuildingTypes)GC.getInfoTypeForString("BUILDING_GALLERY", true);
		if (eAestheticsSchool != NO_BUILDING)
		{
			pCity->GetCityBuildings()->SetNumFreeBuilding(eAestheticsSchool, 1);
		}
		eAestheticsSchool = (BuildingTypes)GC.getInfoTypeForString("BUILDING_CONSERVATORY", true);
		if (eAestheticsSchool != NO_BUILDING)
		{
			pCity->GetCityBuildings()->SetNumFreeBuilding(eAestheticsSchool, 1);
		}
		ChangeNumCitiesFreeAestheticsSchools(-1);
	}


	int iNumFreeWalls = GetNumCitiesFreeWalls();
	if (iNumFreeWalls > 0)
	{
		BuildingTypes eWalls = pCity->ChooseFreeWallsBuilding();
		if (eWalls != NO_BUILDING)
		{
			pCity->GetCityBuildings()->SetNumRealBuilding(eWalls, 1);
		}
		ChangeNumCitiesFreeWalls(-1);
	}

	int iNumFreeCultureBuildings = GetNumCitiesFreeCultureBuilding();
	if(iNumFreeCultureBuildings > 0)
	{
		BuildingTypes eBuilding = pCity->ChooseFreeCultureBuilding();
		if(eBuilding != NO_BUILDING)
		{
			pCity->GetCityBuildings()->SetNumFreeBuilding(eBuilding, 1);
		}
		else
		{
			pCity->SetOwedCultureBuilding(true);
		}

		ChangeNumCitiesFreeCultureBuilding(-1);
	}

	int iNumFreeFoodBuildings = GetNumCitiesFreeFoodBuilding();
	if(iNumFreeFoodBuildings > 0)
	{
		BuildingTypes eBuilding = pCity->ChooseFreeFoodBuilding();
		if(eBuilding != NO_BUILDING)
		{
#ifdef AQUEDUCT_FIX
			pCity->GetCityBuildings()->SetNumRealBuilding(eBuilding, 0);
#endif
			pCity->GetCityBuildings()->SetNumFreeBuilding(eBuilding, 1);
		}

		ChangeNumCitiesFreeFoodBuilding(-1);
	}
}


bool CvPlayer::canFound(int iX, int iY, bool bTestVisible) const
{
	CvPlot* pPlot;

	pPlot = GC.getMap().plot(iX, iY);


	if(!isMinorCiv() && !isBarbarian())
	{
		if(pPlot->IsNoSettling(GetID()))
			return false;
	}

#ifndef AUI_PLAYER_FIX_VENICE_ONLY_BANS_SETTLERS_NOT_SETTLING

	if (GetPlayerTraits()->IsNoAnnexing() && getCapitalCity())
	{
		return false;
	}
#endif


	if(!bTestVisible)
	{
		if(IsEmpireVeryUnhappy())
			return false;
	}

	return GC.getGame().GetSettlerSiteEvaluator()->CanFound(pPlot, this, bTestVisible);
}



#if !defined(LEKMOD_TRACK_CITY_SETTLER_UNITTYPE)
void CvPlayer::found(int iX, int iY)
#else
void CvPlayer::found(int iX, int iY, UnitTypes eSettlerUnit)
#endif
{
	if(!canFound(iX, iY))
	{
		return;
	}

	SetTurnsSinceSettledLastCity(0);

	CvCity* pCity = initCity(iX, iY);
	CvAssertMsg(pCity != NULL, "City is not assigned a valid value");
	if(pCity == NULL)
		return;

	int iExtraTerritoryClaim = GetPlayerTraits()->GetExtraFoundedCityTerritoryClaimRange();
	iExtraTerritoryClaim += GetPlayerPolicies()->GetNumericModifier(POLICYMOD_EXTRA_TERRITORY_CLAIM);
	for (int i = 0; i < iExtraTerritoryClaim; i++)
	{
		CvPlot* pPlotToAcquire = pCity->GetNextBuyablePlot();


		if(pPlotToAcquire)
		{
			pCity->DoAcquirePlot(pPlotToAcquire->getX(), pPlotToAcquire->getY());
		}
	}

#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#else
	for(int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#endif
	{
		const BuildingClassTypes eBuildingClass = static_cast<BuildingClassTypes>(iI);
		CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
		if(pkBuildingClassInfo)
		{
			const BuildingTypes eLoopBuilding = ((BuildingTypes)(getCivilizationInfo().getCivilizationBuildings(iI)));
			if(eLoopBuilding != NO_BUILDING)
			{
				CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eLoopBuilding);
				if(pkBuildingInfo)
				{
					if(pkBuildingInfo->GetFreeStartEra() != NO_ERA)
					{
						if(GC.getGame().getStartEra() >= pkBuildingInfo->GetFreeStartEra())
						{
							if(pCity->canConstruct(eLoopBuilding))
							{
								pCity->GetCityBuildings()->SetNumRealBuilding(eLoopBuilding, 1);
							}
						}
					}
				}
			}
		}
	}
#if defined(LEKMOD_TRACK_CITY_SETTLER_UNITTYPE)
	if(eSettlerUnit != NO_UNIT)
	{
		pCity->SetSettlerUnit(eSettlerUnit);
	}
#endif
#if defined(TRAITIFY)
	pCity->updateYield();
#endif
	AwardFreeBuildings(pCity);

	DoUpdateNextPolicyCost();

	if(isHuman() && getAdvancedStartPoints() < 0)
	{



		if(GC.getGame().getGameTurn() == 0)
		{
			this->GetEconomicAI()->DoTurn();
			this->GetMilitaryAI()->DoTurn();
			this->GetReligionAI()->DoTurn();
			this->GetEspionageAI()->DoTurn();
			this->GetTradeAI()->DoTurn();
		}
		pCity->GetCityStrategyAI()->DoTurn();
		pCity->chooseProduction();
		pCity->doFoundMessage();


		if(GetPlayerTechs()->GetCurrentResearch() == NO_TECH && GetScienceTimes100() > 0)
		{
			if(GetID() == GC.getGame().getActivePlayer())
			{
				chooseTech();
			}
		}
	}
	else
	{
		pCity->doFoundMessage();


		GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_CITY_FOUNDED);
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());
		args->Push(pCity->getX());
		args->Push(pCity->getY());

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "PlayerCityFounded", args.get(), bResult);
	}
}

#if defined(LEKMOD_CITY_YIELDS_TRAITS) && defined(LEKMOD_TRACK_CITY_SETTLER_UNITTYPE) && defined(LEKMOD_YIELD_SETTLE_UNIT_NON_CAP_MAX) && (LEKMOD_YIELD_SETTLE_UNIT_NON_CAP_MAX > 0)



void CvPlayer::RebuildYieldSettleUnitCityOrder()
{
	m_aiYieldSettleUnitCityOrder.clear();
	UnitTypes eTraitUnit = GetPlayerTraits()->GetYieldSettleUnit();
	if (eTraitUnit == NO_UNIT)
		return;

	std::vector<std::pair<int, int> > v;
	int iLoop;
	for (CvCity* pLoop = firstCity(&iLoop); pLoop != NULL; pLoop = nextCity(&iLoop))
	{
		if (pLoop->isCapital())
			continue;
		if (pLoop->SettlerUnit() != eTraitUnit)
			continue;
		v.push_back(std::make_pair(pLoop->getGameTurnFounded(), pLoop->GetID()));
	}
	std::sort(v.begin(), v.end());
	m_aiYieldSettleUnitCityOrder.reserve(v.size());
	for (size_t i = 0; i < v.size(); ++i)
		m_aiYieldSettleUnitCityOrder.push_back(v[i].second);
}

bool CvPlayer::IsCityReceivingYieldSettleUnitEraBonus(const CvCity* pCity)
{
	CvAssert(pCity != NULL);
	if (!pCity)
		return true;

	UnitTypes eTraitUnit = GetPlayerTraits()->GetYieldSettleUnit();
	if (eTraitUnit == NO_UNIT)
		return true;
	if (pCity->getOwner() != GetID())
		return true;
	if (pCity->SettlerUnit() != eTraitUnit)
		return true;
	if (pCity->isCapital())
		return true;

	RebuildYieldSettleUnitCityOrder();

	const int iMax = LEKMOD_YIELD_SETTLE_UNIT_NON_CAP_MAX;
	int iRemaining = iMax;
	const int iTargetID = pCity->GetID();

	for (size_t i = 0; i < m_aiYieldSettleUnitCityOrder.size(); ++i)
	{
		CvCity* pLoop = getCity(m_aiYieldSettleUnitCityOrder[i]);
		if (pLoop == NULL)
			continue;
		if (pLoop->getOwner() != GetID())
			continue;
		if (pLoop->isCapital())
			continue;
		if (pLoop->SettlerUnit() != eTraitUnit)
			continue;

		if (pLoop->GetID() == iTargetID)
			return (iRemaining > 0);
		if (iRemaining > 0)
			--iRemaining;
	}
	return false;
}
#endif


bool CvPlayer::canTrain(UnitTypes eUnit, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bIgnoreUniqueUnitStatus, CvString* toolTipSink) const
{
	CvUnitEntry* pUnitInfoPtr = GC.getUnitInfo(eUnit);
	if(pUnitInfoPtr == NULL)
		return false;

	CvUnitEntry& pUnitInfo = *pUnitInfoPtr;

	const UnitClassTypes eUnitClass = (UnitClassTypes) pUnitInfo.GetUnitClassType();
	if(eUnitClass == NO_UNITCLASS)
	{
		return false;
	}

	CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
	if(pkUnitClassInfo == NULL)
	{
		return false;
	}

	if (GetPlayerTraits()->NoTrain(eUnitClass))
	{
		return false;
	}


	if(!bIgnoreUniqueUnitStatus)
	{
		UnitTypes eThisPlayersUnitType = (UnitTypes) getCivilizationInfo().getCivilizationUnits(eUnitClass);


		if(eThisPlayersUnitType != eUnit)
		{
			return false;
		}
	}

	if(!bIgnoreCost)
	{
		if(pUnitInfo.GetProductionCost() == -1)
		{
			return false;
		}
	}


	if(pUnitInfo.IsFound() || pUnitInfo.IsFoundAbroad())
	{
#if defined(NQM_AI_GIMP_NO_BUILDING_SETTLERS) && defined(AUI_PLAYER_FIX_ENSURE_NO_CS_SETTLER)
		if (isMinorCiv() || (isHuman() && GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE)) || (!isHuman() && GC.getGame().isOption("GAMEOPTION_AI_TWEAKS")))
#elif defined(NQM_AI_GIMP_NO_BUILDING_SETTLERS)
		if ((isHuman() && GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE)) || (!isHuman() && (GC.getGame().isOption("GAMEOPTION_AI_TWEAKS") || GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_BUILDING_SETTLERS"))))
#elif defined(AUI_PLAYER_FIX_ENSURE_NO_CS_SETTLER)
		if (isMinorCiv() || (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman()))
#else
		if(GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
#endif
		{
			return false;
		}
	}
	

	PolicyTypes ePolicy = (PolicyTypes)pUnitInfo.GetPolicyType();
	if (ePolicy != NO_POLICY)
	{
		if (!GetPlayerPolicies()->HasPolicy(ePolicy))
		{
			return false;
		}
	}

#if defined(MISC_CHANGES)

	PolicyBranchTypes ePolicyBranch = (PolicyBranchTypes)pUnitInfo.GetPolicyBranchType();
	PolicyBranchTypes eIdeology = GetPlayerPolicies()->GetLateGamePolicyTree();
	if (ePolicyBranch != NO_POLICY_BRANCH_TYPE)
	{
		if (!GetPlayerPolicies()->HasPolicyBranch(ePolicyBranch))
		{
			return false;
		}
	}

	if (pUnitInfo.IsAnyIdeologyUnlock())
	{

		if (eIdeology == NO_POLICY_BRANCH_TYPE)
		{
			return false;
		}
	}
#endif

	if (GC.getGame().isOption(GAMEOPTION_NO_RELIGION))
	{
		if (pUnitInfo.IsFoundReligion() || pUnitInfo.IsSpreadReligion() || pUnitInfo.IsRemoveHeresy())
		{
			return false;
		}
	}

	if(!bContinue)
	{
		if(!bTestVisible)
		{

			if(pUnitInfo.GetWorkRate() > 0 && pUnitInfo.GetDomainType() == DOMAIN_LAND)
			{
				if(GetMaxNumBuilders() > -1 && GetNumBuilders() >= GetMaxNumBuilders())
				{
					return false;
				}
			}
		}
	}


	if(!(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)(pUnitInfo.GetPrereqAndTech()))))
	{
		return false;
	}

	int iI;
	for(iI = 0; iI < GC.getNUM_UNIT_AND_TECH_PREREQS(); iI++)
	{
		if(pUnitInfo.GetPrereqAndTechs(iI) != NO_TECH)
		{
			if(!(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)(pUnitInfo.GetPrereqAndTechs(iI)))))
			{
				return false;
			}
		}
	}


	if((TechTypes)pUnitInfo.GetObsoleteTech() != NO_TECH)
	{
		if(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)(pUnitInfo.GetObsoleteTech())))
		{
			return false;
		}
	}


	if(GC.getGame().isUnitClassMaxedOut(eUnitClass))
	{
		return false;
	}


	if(GET_TEAM(getTeam()).isUnitClassMaxedOut(eUnitClass))
	{
		return false;
	}


	if(isUnitClassMaxedOut(eUnitClass))
	{
		return false;
	}


	ProjectTypes eProject = (ProjectTypes) pUnitInfo.GetSpaceshipProject();
	if(eProject != NO_PROJECT)
	{
		if(GET_TEAM(getTeam()).isProjectMaxedOut(eProject))
			return false;

		int iUnitAndProjectCount = GET_TEAM(getTeam()).getProjectCount(eProject) + getUnitClassCount(eUnitClass) + GET_TEAM(getTeam()).getUnitClassMaking(eUnitClass) + ((bContinue) ? -1 : 0);
		if(iUnitAndProjectCount >= pkUnitClassInfo->getMaxPlayerInstances())
		{
			return false;
		}
	}

	if(!bTestVisible)
	{

		if(pUnitInfo.IsFound() || pUnitInfo.IsFoundAbroad())
		{
			if(IsEmpireVeryUnhappy() && GC.getVERY_UNHAPPY_CANT_TRAIN_SETTLERS() == 1)
			{
				GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_VERY_UNHAPPY_SETTLERS");
				if(toolTipSink == NULL)
					return false;
			}
		}


		ProjectTypes ePrereqProject = (ProjectTypes) pUnitInfo.GetProjectPrereq();
		if(ePrereqProject != NO_PROJECT)
		{
			CvProjectEntry* pkProjectInfo = GC.getProjectInfo(ePrereqProject);
			if(pkProjectInfo)
			{
				if(GET_TEAM(getTeam()).getProjectCount(ePrereqProject) == 0)
				{
					GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_UNIT_PROJECT_REQUIRED", pkProjectInfo->GetDescription());
					if(toolTipSink == NULL)
						return false;
				}
			}
		}


#ifdef AUI_WARNING_FIXES
		for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
		{
			const ResourceTypes eResource = static_cast<ResourceTypes>(iResourceLoop);
			CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			if(pkResourceInfo)
			{
				const int iNumResource = pUnitInfo.GetResourceQuantityRequirement(eResource);

				if(iNumResource > 0)
				{

					if(!bContinue && getNumResourceAvailable(eResource) < iNumResource)
					{
						GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_UNIT_LACKS_RESOURCES", pkResourceInfo->GetIconString(), pkResourceInfo->GetTextKey(), iNumResource);
						if(toolTipSink == NULL)
							return false;
					}

					else if(bContinue && (getNumResourceAvailable(eResource) < 0))
					{
						GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_UNIT_LACKS_RESOURCES", pkResourceInfo->GetIconString(), pkResourceInfo->GetTextKey(), iNumResource);
						if(toolTipSink == NULL)
							return false;
					}
				}
			}

		}

		if(GC.getGame().isUnitClassMaxedOut(eUnitClass, (GET_TEAM(getTeam()).getUnitClassMaking(eUnitClass) + ((bContinue) ? -1 : 0))))
		{
			GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_GAME_COUNT_MAX", "", "", pkUnitClassInfo->getMaxTeamInstances());
			if(toolTipSink == NULL)
				return false;
		}

		if(GET_TEAM(getTeam()).isUnitClassMaxedOut(eUnitClass, (GET_TEAM(getTeam()).getUnitClassMaking(eUnitClass) + ((bContinue) ? -1 : 0))))
		{
			GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_TEAM_COUNT_MAX", "", "", pkUnitClassInfo->getMaxTeamInstances());
			if(toolTipSink == NULL)
				return false;
		}

		if(isUnitClassMaxedOut(eUnitClass, (getUnitClassMaking(eUnitClass) + ((bContinue) ? -1 : 0))))
		{
			GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_PLAYER_COUNT_MAX", "", "", pkUnitClassInfo->getMaxPlayerInstances());
			if(toolTipSink == NULL)
				return false;
		}

		if(GC.getGame().isNoNukes() || !GC.getGame().isNukesValid())
		{
			if(pUnitInfo.GetNukeDamageLevel() != -1)
			{
				GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_NUKES");
				if(toolTipSink == NULL)
					return false;
			}
		}

		if(pUnitInfo.GetNukeDamageLevel() != -1)
		{
			if(GC.getGame().GetGameLeagues()->IsNoTrainingNuclearWeapons(GetID()))
			{
				GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_NUKES_BY_RESOLUTION");
				if(toolTipSink == NULL)
					return false;
			}
		}

		if(pUnitInfo.GetSpecialUnitType() != NO_SPECIALUNIT)
		{
			if(!(GC.getGame().isSpecialUnitValid((SpecialUnitTypes)(pUnitInfo.GetSpecialUnitType()))))
			{
				GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_SPECIAL_UNIT");
				if(toolTipSink == NULL)
					return false;
			}
		}

		if (pUnitInfo.IsTrade())
		{
			if (GetTrade()->GetNumTradeRoutesRemaining(bContinue) <= 0)
			{
				GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_TRADE_UNIT_CONSTRUCTION_NO_EXTRA_SLOTS");
				if (toolTipSink == NULL)
					return false;			
			}

			DomainTypes eDomain = (DomainTypes)pUnitInfo.GetDomainType();
			if (!GetTrade()->CanCreateTradeRoute(eDomain))
			{
				if (eDomain == DOMAIN_LAND)
				{
					GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_TRADE_UNIT_CONSTRUCTION_NONE_OF_TYPE_LAND");
				}
				else if (eDomain == DOMAIN_SEA)
				{
					GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_TRADE_UNIT_CONSTRUCTION_NONE_OF_TYPE_SEA");
				}
				if (toolTipSink == NULL)
					return false;
			}
		}
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());
		args->Push(eUnit);



		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "PlayerCanTrain", args.get(), bResult))
		{

			if(bResult == false)
			{
				return false;
			}
		}
	}

	return true;
}



bool CvPlayer::canConstruct(BuildingTypes eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost, CvString* toolTipSink) const
{
	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pkBuildingInfo == NULL)
		return false;


	if(GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE) && pkBuildingInfo->IsEspionage())
	{
		return false;
	}

	CvBuildingEntry& pBuildingInfo = *pkBuildingInfo;

#ifdef AUI_WARNING_FIXES
	uint iI;
	int iJ;
#else
	int iI;
#endif
	CvTeam& currentTeam = GET_TEAM(getTeam());

	const BuildingClassTypes eBuildingClass = ((BuildingClassTypes)(pBuildingInfo.GetBuildingClassType()));
	const CvBuildingClassInfo& kBuildingClass = pkBuildingInfo->GetBuildingClassInfo();


	if(getCivilizationInfo().getCivilizationBuildings(eBuildingClass) != eBuilding)
	{
		return false;
	}
#if !defined(TRAITIFY)
	if (!bIgnoreCost)
	{
		if (pBuildingInfo.GetProductionCost() == -1)
		{
			return false;
		}
	}
#else
	if (!bIgnoreCost)
	{

		int iBaseCost = pBuildingInfo.GetProductionCost();
		int iOverrideCost = GetPlayerTraits()->GetBuildingCostOverride(eBuilding, YIELD_PRODUCTION);


		if (iBaseCost == -1 && iOverrideCost <= 0)
		{
		    return false;
		}

		if (iBaseCost > -1 && iOverrideCost == -1)
		{
		    return false;
		}
	}
#endif

	PolicyBranchTypes eBranch = (PolicyBranchTypes)pBuildingInfo.GetPolicyBranchType();
	if (eBranch != NO_POLICY_BRANCH_TYPE)
	{
		if (!GetPlayerPolicies()->IsPolicyBranchUnlocked(eBranch))
		{
			return false;
		}
	}

	if(!(currentTeam.GetTeamTechs()->HasTech((TechTypes)(pBuildingInfo.GetPrereqAndTech()))))
	{
		return false;
	}

#ifdef AUI_WARNING_FIXES
	for (iJ = 0; iJ < GC.getNUM_BUILDING_AND_TECH_PREREQS(); iJ++)
	{
		if (pBuildingInfo.GetPrereqAndTechs(iJ) != NO_TECH)
		{
			if (!(currentTeam.GetTeamTechs()->HasTech((TechTypes)(pBuildingInfo.GetPrereqAndTechs(iJ)))))
#else
	for(iI = 0; iI < GC.getNUM_BUILDING_AND_TECH_PREREQS(); iI++)
	{
		if(pBuildingInfo.GetPrereqAndTechs(iI) != NO_TECH)
		{
			if(!(currentTeam.GetTeamTechs()->HasTech((TechTypes)(pBuildingInfo.GetPrereqAndTechs(iI)))))
#endif
			{
				return false;
			}
		}
	}

	if(currentTeam.isObsoleteBuilding(eBuilding))
	{
		return false;
	}


	BuildingClassTypes eReplacementBuildingClass = (BuildingClassTypes) pBuildingInfo.GetReplacementBuildingClass();

	if(eReplacementBuildingClass != NO_BUILDINGCLASS)
	{
		BuildingTypes eUpgradeBuilding = ((BuildingTypes)(getCivilizationInfo().getCivilizationBuildings(eReplacementBuildingClass)));

		if(canConstruct(eUpgradeBuilding))
		{
			return false;
		}
	}

	if(pBuildingInfo.GetVictoryPrereq() != NO_VICTORY)
	{
		if(!(GC.getGame().isVictoryValid((VictoryTypes)(pBuildingInfo.GetVictoryPrereq()))))
		{
			return false;
		}

		if(isMinorCiv())
		{
			return false;
		}

		if(currentTeam.getVictoryCountdown((VictoryTypes)pBuildingInfo.GetVictoryPrereq()) >= 0)
		{
			return false;
		}
	}

	if(pBuildingInfo.GetMaxStartEra() != NO_ERA)
	{
		if(GC.getGame().getStartEra() > pBuildingInfo.GetMaxStartEra())
		{
			return false;
		}
	}

	if(GC.getGame().isBuildingClassMaxedOut(eBuildingClass))
	{
		return false;
	}

#ifdef NQM_AI_GIMP_NO_WORLD_WONDERS
	CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
	if (GC.getGame().isBuildingClassMaxedOut(eBuildingClass) ||
		isWorldWonderClass(*pkBuildingClassInfo) && (GC.getGame().isOption("GAMEOPTION_AI_TWEAKS") || GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_WORLD_WONDER")) && !isHuman())
	{
		return false;
	}
#endif

	if(currentTeam.isBuildingClassMaxedOut(eBuildingClass))
	{
		return false;
	}

	if(isBuildingClassMaxedOut(eBuildingClass))
	{
		return false;
	}





	if(!bTestVisible)
	{

		CvCivilizationInfo& civilizationInfo = getCivilizationInfo();
#ifdef AUI_WARNING_FIXES
		uint numBuildingClassInfos = GC.getNumBuildingClassInfos();
#else
		int numBuildingClassInfos = GC.getNumBuildingClassInfos();
#endif

		for(iI = 0; iI < numBuildingClassInfos; iI++)
		{
			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo((BuildingClassTypes)iI);
			if(!pkBuildingClassInfo)
			{
				continue;
			}

			BuildingTypes ePrereqBuilding = (BuildingTypes)civilizationInfo.getCivilizationBuildings(iI);

			if(NO_BUILDING != ePrereqBuilding && currentTeam.isObsoleteBuilding(ePrereqBuilding))
			{
				CvBuildingEntry* pkPrereqBuilding = GC.getBuildingInfo(ePrereqBuilding);
				if(pkPrereqBuilding)
				{
					int iNumHave = getBuildingClassCount((BuildingClassTypes)iI);

					int iNumNeeded = getBuildingClassPrereqBuilding(eBuilding, (BuildingClassTypes)iI, 0);

					if(iNumHave < iNumNeeded)
					{
						GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_BUILDING_COUNT_NEEDED", pkPrereqBuilding->GetTextKey(), "", iNumNeeded - iNumHave);

						if(toolTipSink == NULL)
							return false;
					}
				}
			}
		}


#ifdef AUI_WARNING_FIXES
		for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
		{
			const ResourceTypes eResource = static_cast<ResourceTypes>(iResourceLoop);
			CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
			if(pkResource)
			{
				int iNumResource = pBuildingInfo.GetResourceQuantityRequirement(eResource);
				if(iNumResource > 0)
				{
					if(bContinue)
						iNumResource = 0;

					if(getNumResourceAvailable(eResource) < iNumResource)
					{
						GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_BUILDING_LACKS_RESOURCES", pkResource->GetIconString(), pkResource->GetTextKey(), iNumResource);
						if(toolTipSink == NULL)
							return false;
					}
				}
			}
		}

		if(GC.getGame().isBuildingClassMaxedOut(eBuildingClass, (currentTeam.getBuildingClassMaking(eBuildingClass) + ((bContinue) ? -1 : 0))))
		{
			GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_GAME_COUNT_MAX", "", "", kBuildingClass.getMaxGlobalInstances());
			if(toolTipSink == NULL)
				return false;
		}

		if(currentTeam.isBuildingClassMaxedOut(eBuildingClass, (currentTeam.getBuildingClassMaking(eBuildingClass) + ((bContinue) ? -1 : 0))))
		{
			GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_TEAM_COUNT_MAX", "", "", kBuildingClass.getMaxTeamInstances());
			if(toolTipSink == NULL)
				return false;
		}

		if(isBuildingClassMaxedOut(eBuildingClass, (getBuildingClassMaking(eBuildingClass) + ((bContinue) ? -1 : 0))))
		{
			GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_PLAYER_COUNT_MAX", "", "", kBuildingClass.getMaxPlayerInstances());
			if(toolTipSink == NULL)
				return false;
		}

		if(getNumCities() < pBuildingInfo.GetNumCitiesPrereq())
		{
			return false;
		}

		if(getHighestUnitLevel() < pBuildingInfo.GetUnitLevelPrereq())
		{
			return false;
		}


		BuildingTypes ePrereqBuilding;
		int iNumNeeded;
		for(iI = 0; iI < numBuildingClassInfos; iI++)
		{
			iNumNeeded = getBuildingClassPrereqBuilding(eBuilding, ((BuildingClassTypes)iI), bContinue);

			ePrereqBuilding = (BuildingTypes) civilizationInfo.getCivilizationBuildings(iI);
			if(NO_BUILDING != ePrereqBuilding)
			{
				CvBuildingEntry* pkPrereqBuilding = GC.getBuildingInfo(ePrereqBuilding);
				if(pkPrereqBuilding)
				{
					int iNumHave = 0;
					const CvCity* pLoopCity = NULL;
					int iLoop;
					for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
					{
						if(pLoopCity && !pLoopCity->IsPuppet() && pLoopCity->GetCityBuildings()->GetNumBuilding(ePrereqBuilding) > 0)
						{
							iNumHave++;
						}
					}

					if(iNumHave < iNumNeeded)
					{
						ePrereqBuilding = (BuildingTypes) civilizationInfo.getCivilizationBuildings(iI);

						GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_BUILDING_COUNT_NEEDED", pkPrereqBuilding->GetTextKey(), "", iNumNeeded - iNumHave);

						if(toolTipSink == NULL)
							return false;


						int iNonPuppetCities = getNumCities() - GetNumPuppetCities();
						if(iNumNeeded == iNonPuppetCities && iNumNeeded - iNumHave < 5)
						{
							(*toolTipSink) += "[NEWLINE]";

							for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
							{
								if(pLoopCity && !pLoopCity->IsPuppet() && pLoopCity->GetCityBuildings()->GetNumBuilding(ePrereqBuilding) == 0)
								{
									(*toolTipSink) += pLoopCity->getName();
									(*toolTipSink) += " ";
								}
							}
						}
					}
				}
			}
		}
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());
		args->Push(eBuilding);



		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "PlayerCanConstruct", args.get(), bResult))
		{

			if(bResult == false)
			{
				return false;
			}
		}
	}

	return true;
}



bool CvPlayer::canCreate(ProjectTypes eProject, bool bContinue, bool bTestVisible) const
{
	CvProjectEntry* pkProjectInfo = GC.getProjectInfo(eProject);
	if(!pkProjectInfo)
	{
		return false;
	}

	CvProjectEntry& pProjectInfo = *pkProjectInfo;

#ifdef AUI_WARNING_FIXES
	uint iI;
#else
	int iI;
#endif


	if(isBarbarian())
	{
		return false;
	}


	if(isMinorCiv())
	{
		return false;
	}


	if(pProjectInfo.GetProductionCost() == -1)
	{
		return false;
	}


	if(!(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)(pProjectInfo.GetTechPrereq()))))
	{
		return false;
	}


	if(pProjectInfo.GetCultureBranchesRequired() > 0)
	{
		if(GetPlayerPolicies()->GetNumPolicyBranchesFinished() < pProjectInfo.GetCultureBranchesRequired())
		{
			return false;
		}
	}


	if(pProjectInfo.GetVictoryPrereq() != NO_VICTORY)
	{
		if(!(GC.getGame().isVictoryValid((VictoryTypes)(pProjectInfo.GetVictoryPrereq()))))
		{
			return false;
		}

		if(isMinorCiv())
		{
			return false;
		}

		if(GET_TEAM(getTeam()).getVictoryCountdown((VictoryTypes)pProjectInfo.GetVictoryPrereq()) >= 0)
		{
			return false;
		}
	}

	if(GC.getGame().isProjectMaxedOut(eProject))
	{
		return false;
	}

	if(GET_TEAM(getTeam()).isProjectMaxedOut(eProject))
	{
		return false;
	}

	if(!bTestVisible)
	{

		ResourceTypes eResource;
		int iNumResource;
#ifdef AUI_WARNING_FIXES
		for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
		{
			eResource = (ResourceTypes) iResourceLoop;
			iNumResource = pProjectInfo.GetResourceQuantityRequirement(eResource);

			if(iNumResource > 0)
			{
				if(getNumResourceAvailable(eResource) < iNumResource)
				{
					return false;
				}
			}
		}

		if(GC.getGame().isProjectMaxedOut(eProject, (GET_TEAM(getTeam()).getProjectMaking(eProject) + ((bContinue) ? -1 : 0))))
		{
			return false;
		}

		if(GET_TEAM(getTeam()).isProjectMaxedOut(eProject, (GET_TEAM(getTeam()).getProjectMaking(eProject) + ((bContinue) ? -1 : 0))))
		{
			return false;
		}


		if(GC.getGame().isNoNukes())
		{
			if(pProjectInfo.IsAllowsNukes())
			{
				for(iI = 0; iI < GC.getNumUnitInfos(); iI++)
				{
					CvUnitEntry* pkUnitEntry = GC.getUnitInfo((UnitTypes)iI);
					if(pkUnitEntry && pkUnitEntry->GetNukeDamageLevel() != -1)
					{
						return false;
					}
				}
			}
		}

		if(pProjectInfo.GetAnyoneProjectPrereq() != NO_PROJECT)
		{
			if(GC.getGame().getProjectCreatedCount((ProjectTypes)(pProjectInfo.GetAnyoneProjectPrereq())) == 0)
			{
				return false;
			}
		}

		for(iI = 0; iI < GC.getNumProjectInfos(); iI++)
		{
			if(GET_TEAM(getTeam()).getProjectCount((ProjectTypes)iI) < pProjectInfo.GetProjectsNeeded(iI))
			{
				return false;
			}
		}
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());
		args->Push(eProject);



		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "PlayerCanCreate", args.get(), bResult))
		{

			if(bResult == false)
			{
				return false;
			}
		}
	}

	return true;
}



bool CvPlayer::canPrepare(SpecialistTypes eSpecialist, bool) const
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());
		args->Push(eSpecialist);



		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "PlayerCanPrepare", args.get(), bResult))
		{

			if(bResult == false)
			{
				return false;
			}
		}
	}


	return false;
}



bool CvPlayer::canMaintain(ProcessTypes eProcess, bool) const
{

	const CvProcessInfo* pkProcessInfo = GC.getProcessInfo(eProcess);
	if (!pkProcessInfo)
		return false;

	if(!(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)(pkProcessInfo->getTechPrereq()))))
	{
		return false;
	}

#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumLeagueProjectInfos(); iI++)
#else
	for(int iI = 0; iI < GC.getNumLeagueProjectInfos(); iI++)
#endif
	{
		LeagueProjectTypes eLeagueProject = (LeagueProjectTypes) iI;
		CvLeagueProjectEntry* pInfo = GC.getLeagueProjectInfo(eLeagueProject);
		if (pInfo && pInfo->GetProcess() == eProcess)
		{
			if (!GC.getGame().GetGameLeagues()->CanContributeToLeagueProject(GetID(), eLeagueProject))
			{
				return false;
			}
		}
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());
		args->Push(eProcess);



		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "PlayerCanMaintain", args.get(), bResult))
		{

			if(bResult == false)
			{
				return false;
			}
		}
	}

	return true;
}



bool CvPlayer::IsCanPurchaseAnyCity(bool bTestPurchaseCost, bool bTestTrainable, UnitTypes eUnit, BuildingTypes eBuilding, YieldTypes ePurchaseYield)
{
	int iLoop;
	CvCity *pLoopCity;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->IsCanPurchase(bTestPurchaseCost, bTestTrainable, eUnit, eBuilding, NO_PROJECT, ePurchaseYield))
		{
			return true;
		}
	}

	return false;
}


bool CvPlayer::isProductionMaxedUnitClass(UnitClassTypes eUnitClass) const
{
	if(eUnitClass == NO_UNITCLASS)
	{
		return false;
	}

	if(GC.getGame().isUnitClassMaxedOut(eUnitClass))
	{
		return true;
	}

	if(GET_TEAM(getTeam()).isUnitClassMaxedOut(eUnitClass))
	{
		return true;
	}

	if(isUnitClassMaxedOut(eUnitClass))
	{
		return true;
	}

	return false;
}



bool CvPlayer::isProductionMaxedBuildingClass(BuildingClassTypes eBuildingClass, bool bAcquireCity) const
{
	if(eBuildingClass == NO_BUILDINGCLASS)
	{
		return false;
	}

	CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
	if(pkBuildingClassInfo == NULL)
	{
		return false;
	}

	if(!bAcquireCity)
	{
		if(GC.getGame().isBuildingClassMaxedOut(eBuildingClass))
		{
			return true;
		}
	}

	if(GET_TEAM(getTeam()).isBuildingClassMaxedOut(eBuildingClass))
	{
		return true;
	}

	if(isBuildingClassMaxedOut(eBuildingClass, ((bAcquireCity) ? pkBuildingClassInfo->getExtraPlayerInstances() : 0)))
	{
		return true;
	}

	return false;
}



bool CvPlayer::isProductionMaxedProject(ProjectTypes eProject) const
{
	if(eProject == NO_PROJECT)
	{
		return false;
	}

	if(GC.getGame().isProjectMaxedOut(eProject))
	{
		return true;
	}

	if(GET_TEAM(getTeam()).isProjectMaxedOut(eProject))
	{
		return true;
	}

	return false;
}



int CvPlayer::getProductionNeeded(UnitTypes eUnit) const
{
	CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eUnit);

	CvAssertMsg(pkUnitEntry, "This should never be hit");
	if(pkUnitEntry == NULL)
		return 0;

	UnitClassTypes eUnitClass = (UnitClassTypes)pkUnitEntry->GetUnitClassType();
	CvAssert(NO_UNITCLASS != eUnitClass);

	CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
	CvAssert(pkUnitClassInfo);
	if(pkUnitClassInfo == NULL)
		return 0;

	int iProductionNeeded = pkUnitEntry->GetProductionCost();
	
	iProductionNeeded *= 100 + getUnitClassCount(eUnitClass) * pkUnitClassInfo->getInstanceCostModifier();
	iProductionNeeded /= 100;

	if(isMinorCiv())
	{
		iProductionNeeded *= GC.getMINOR_CIV_PRODUCTION_PERCENT();
		iProductionNeeded /= 100;
	}

	iProductionNeeded *= GC.getUNIT_PRODUCTION_PERCENT();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getGame().getGameSpeedInfo().getTrainPercent();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getGame().getStartEraInfo().getTrainPercent();
	iProductionNeeded /= 100;

	if(!isHuman() && !IsAITeammateOfHuman() && !isBarbarian())
	{
		if(isWorldUnitClass(eUnitClass))
		{
			iProductionNeeded *= GC.getGame().getHandicapInfo().getAIWorldTrainPercent();
			iProductionNeeded /= 100;
		}
		else
		{
			iProductionNeeded *= GC.getGame().getHandicapInfo().getAITrainPercent();
			iProductionNeeded /= 100;
		}

		iProductionNeeded *= std::max(0, ((GC.getGame().getHandicapInfo().getAIPerEraModifier() * GetCurrentEra()) + 100));
		iProductionNeeded /= 100;
	}

	iProductionNeeded += getUnitExtraCost(eUnitClass);

#ifdef LEKMOD_CUSTOM_SETTLERS

	if(pkUnitEntry->IsFound() && pkUnitEntry->GetSettlerCostModifier() != 0)
	{
		iProductionNeeded *= 100 + pkUnitEntry->GetSettlerCostModifier();
		iProductionNeeded /= 100;
	}
#endif

#ifdef NQ_UNIT_FINAL_PRODUCTION_COST_MODIFIER
	iProductionNeeded *= (100 + pkUnitEntry->GetFinalProductionCostModifier());
	iProductionNeeded /= 100;
#endif

	return std::max(1, iProductionNeeded);
}



int CvPlayer::getProductionNeeded(BuildingTypes eBuilding) const
{
	int iProductionNeeded;

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pkBuildingInfo == NULL)
	{

		return 1;
	}

	iProductionNeeded = pkBuildingInfo->GetProductionCost();

#if defined(TRAITIFY)
	int iOverrideProductionCost = GetPlayerTraits()->GetBuildingCostOverride(eBuilding, YIELD_PRODUCTION);
	if (iOverrideProductionCost > 0)
	{
		iProductionNeeded = iOverrideProductionCost;
	}
#endif

	if(pkBuildingInfo->GetNumCityCostMod() > 0 && getNumCities() > 0)
	{
		iProductionNeeded += (pkBuildingInfo->GetNumCityCostMod() * getNumCities());
	}

	if(isMinorCiv())
	{
		iProductionNeeded *= GC.getMINOR_CIV_PRODUCTION_PERCENT();
		iProductionNeeded /= 100;
	}

	iProductionNeeded *= GC.getBUILDING_PRODUCTION_PERCENT();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getGame().getGameSpeedInfo().getConstructPercent();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getGame().getStartEraInfo().getConstructPercent();
	iProductionNeeded /= 100;

	if(pkBuildingInfo->GetPrereqAndTech() != NO_TECH)
	{
		CvTechEntry* pkTechInfo = GC.getTechInfo((TechTypes)pkBuildingInfo->GetPrereqAndTech());
		if(pkTechInfo)
		{

			int iTotalEraMod = 100;
			EraTypes eBuildingUnlockedEra = (EraTypes) pkTechInfo->GetEra();

			if(eBuildingUnlockedEra < GetCurrentEra())
			{
#ifdef AUI_WARNING_FIXES
				for (uint iLoop = uint(eBuildingUnlockedEra); iLoop < uint(GetCurrentEra()); iLoop++)
#else
				for(int iLoop = eBuildingUnlockedEra; iLoop < GetCurrentEra(); iLoop++)
#endif
				{
					CvAssertMsg(iLoop >= 0, "Loop should be within era bounds");
					CvAssertMsg(iLoop <GC.getNumEraInfos(), "Loop should be within era bounds");

#ifdef AUI_WARNING_FIXES
					if (iLoop < GC.getNumEraInfos())
#else
					if(iLoop >= 0 && iLoop < GC.getNumEraInfos())
#endif
					{
						CvEraInfo* pkEraInfo = GC.getEraInfo((EraTypes)iLoop);
						if(pkEraInfo)
						{
							iTotalEraMod += pkEraInfo->getLaterEraBuildingConstructMod();
						}
					}
				}


				if(iTotalEraMod != 100)
				{
					iProductionNeeded *= iTotalEraMod;
					iProductionNeeded /= 100;
				}
			}
		}
	}

	if(!isHuman() && !IsAITeammateOfHuman() && !isBarbarian())
	{
		if(isWorldWonderClass(pkBuildingInfo->GetBuildingClassInfo()))
		{
			iProductionNeeded *= GC.getGame().getHandicapInfo().getAIWorldConstructPercent();
			iProductionNeeded /= 100;
		}
		else
		{
			iProductionNeeded *= GC.getGame().getHandicapInfo().getAIConstructPercent();
			iProductionNeeded /= 100;
		}

		iProductionNeeded *= std::max(0, ((GC.getGame().getHandicapInfo().getAIPerEraModifier() * GetCurrentEra()) + 100));
		iProductionNeeded /= 100;
	}

	return std::max(1, iProductionNeeded);
}



int CvPlayer::getProductionNeeded(ProjectTypes eProject) const
{
	int iProductionNeeded;

	iProductionNeeded = GC.getProjectInfo(eProject)->GetProductionCost();

	if(isMinorCiv())
	{
		iProductionNeeded *= GC.getMINOR_CIV_PRODUCTION_PERCENT();
		iProductionNeeded /= 100;
	}

	iProductionNeeded *= GC.getPROJECT_PRODUCTION_PERCENT();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getGame().getGameSpeedInfo().getCreatePercent();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getGame().getStartEraInfo().getCreatePercent();
	iProductionNeeded /= 100;

	if(!isHuman() && !IsAITeammateOfHuman() && !isBarbarian())
	{
		if(isWorldProject(eProject))
		{
			iProductionNeeded *= GC.getGame().getHandicapInfo().getAIWorldCreatePercent();
			iProductionNeeded /= 100;
		}
		else
		{
			iProductionNeeded *= GC.getGame().getHandicapInfo().getAICreatePercent();
			iProductionNeeded /= 100;
		}

		iProductionNeeded *= std::max(0, ((GC.getGame().getHandicapInfo().getAIPerEraModifier() * GetCurrentEra()) + 100));
		iProductionNeeded /= 100;
	}

	return std::max(1, iProductionNeeded);
}


int CvPlayer::getProductionNeeded(SpecialistTypes eSpecialist) const
{
	CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
	if(pkSpecialistInfo == NULL)
	{

		CvAssert(pkSpecialistInfo);
		return 0;
	}

	int iProductionNeeded;
	iProductionNeeded = pkSpecialistInfo->getCost();

	if(isMinorCiv())
	{
		iProductionNeeded *= GC.getMINOR_CIV_PRODUCTION_PERCENT();
		iProductionNeeded /= 100;
	}

	iProductionNeeded *= GC.getGame().getGameSpeedInfo().getCreatePercent();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getGame().getStartEraInfo().getCreatePercent();
	iProductionNeeded /= 100;

	return std::max(1, iProductionNeeded);
}


int CvPlayer::getProductionModifier(CvString* toolTipSink) const
{
	int iMultiplier = 0;

	int iTempMod;


	iTempMod = GetUnitProductionMaintenanceMod();
	iMultiplier += iTempMod;
	GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_UNIT_SUPPLY", iTempMod);

	return iMultiplier;
}


int CvPlayer::getProductionModifier(UnitTypes eUnit, CvString* toolTipSink) const
{
	int iMultiplier = getProductionModifier(toolTipSink);
	int iTempMod;

	CvUnitEntry* pUnitEntry = GC.getUnitInfo(eUnit);

	if(pUnitEntry)
	{

		if(pUnitEntry->IsMilitaryProduction())
		{
			iTempMod = getMilitaryProductionModifier();
			iMultiplier += iTempMod;
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_MILITARY_PLAYER", iTempMod);
		}


		if(pUnitEntry->IsFound())
		{
			iTempMod = getSettlerProductionModifier();
			iMultiplier += iTempMod;
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_SETTLER_PLAYER", iTempMod);
		}


		if(pUnitEntry->GetUnitCombatType() != NO_UNITCOMBAT)
		{
			iTempMod = getUnitCombatProductionModifiers((UnitCombatTypes) pUnitEntry->GetUnitCombatType());
			iMultiplier += iTempMod;
#if !defined(LEKMOD_v34)
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_UNIT_COBMAT_CLASS_PLAYER", iTempMod);
#else
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_UNIT_COMBAT_CLASS_PLAYER", iTempMod);
#endif
		}


#ifdef AUI_WARNING_FIXES
		CvPlayerTraits* pPlayerTraits = GetPlayerTraits();
		for (uint iI = 0; iI < GC.getNumTraitInfos(); iI++)
#else
		int iNumTraits = GC.getNumTraitInfos();
		CvPlayerTraits* pPlayerTraits = GetPlayerTraits();
		for(int iI = 0; iI < iNumTraits; iI++)
#endif
		{
			if(pPlayerTraits->HasTrait((TraitTypes)iI))
			{
				iMultiplier += pUnitEntry->GetProductionTraits(iI);

				if(pUnitEntry->GetSpecialUnitType() != NO_SPECIALUNIT)
				{
					CvSpecialUnitInfo* pkSpecialUnitInfo = GC.getSpecialUnitInfo((SpecialUnitTypes) pUnitEntry->GetSpecialUnitType());
					if(pkSpecialUnitInfo)
					{
						iTempMod = pkSpecialUnitInfo->getProductionTraits(iI);
						iMultiplier += iTempMod;
						GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_UNIT_TRAIT", iTempMod);
					}
				}
			}
		}
	}

	return iMultiplier;
}


int CvPlayer::getProductionModifier(BuildingTypes eBuilding, CvString* toolTipSink) const
{
	int iMultiplier = getProductionModifier(toolTipSink);
	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pkBuildingInfo == NULL)
	{
		return iMultiplier;
	}

	CvGame& kGame = GC.getGame();
	const CvBuildingClassInfo& kBuildingClassInfo = pkBuildingInfo->GetBuildingClassInfo();

	int iTempMod;

	int iNumTraits = GC.getNumTraitInfos();
	CvPlayerTraits* pPlayerTraits = GetPlayerTraits();
	for(int iI = 0; iI < iNumTraits; iI++)
	{
		if(pPlayerTraits->HasTrait((TraitTypes)iI))
		{
			iTempMod = pkBuildingInfo->GetProductionTraits(iI);
			iMultiplier += iTempMod;
			kGame.BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_BUILDING_TRAIT", iTempMod);
		}
	}


	if(::isWorldWonderClass(kBuildingClassInfo))
	{
		iTempMod = getMaxGlobalBuildingProductionModifier();
		iMultiplier += iTempMod;
		kGame.BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_WORLD_WONDER_PLAYER", iTempMod);
		iTempMod = m_pPlayerPolicies->GetNumericModifier(POLICYMOD_WONDER_PRODUCTION_MODIFIER);
		iMultiplier += iTempMod;
		kGame.BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_WONDER_POLICY", iTempMod);
	}


	else if(::isTeamWonderClass(kBuildingClassInfo))
	{
		iTempMod = getMaxTeamBuildingProductionModifier();
		iMultiplier += iTempMod;
		kGame.BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_TEAM_WONDER_PLAYER", iTempMod);
		iTempMod = m_pPlayerPolicies->GetNumericModifier(POLICYMOD_WONDER_PRODUCTION_MODIFIER);
		iMultiplier += iTempMod;
		kGame.BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_WONDER_POLICY", iTempMod);
	}


	else if(::isNationalWonderClass(kBuildingClassInfo))
	{
		iTempMod = getMaxPlayerBuildingProductionModifier();
		iMultiplier += iTempMod;
		kGame.BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_NATIONAL_WONDER_PLAYER", iTempMod);
		iTempMod = m_pPlayerPolicies->GetNumericModifier(POLICYMOD_WONDER_PRODUCTION_MODIFIER);
		iMultiplier += iTempMod;
		kGame.BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_WONDER_POLICY", iTempMod);
	}


	else
	{
		iTempMod = m_pPlayerPolicies->GetNumericModifier(POLICYMOD_BUILDING_PRODUCTION_MODIFIER);
		iMultiplier += iTempMod;
		kGame.BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_BUILDING_POLICY_PLAYER", iTempMod);
	}


	if(pkBuildingInfo->IsReligious())
	{
		iTempMod = m_pPlayerPolicies->GetNumericModifier(POLICYMOD_RELIGION_PRODUCTION_MODIFIER);
		iMultiplier += iTempMod;
		kGame.BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_RELIGION_PLAYER", iTempMod);
	}

	return iMultiplier;
}


int CvPlayer::getProductionModifier(ProjectTypes eProject, CvString* toolTipSink) const
{
	int iMultiplier = getProductionModifier(toolTipSink);
	int iTempMod;

	if(GC.getProjectInfo(eProject)->IsSpaceship())
	{
		iTempMod = getSpaceProductionModifier();
		iMultiplier += iTempMod;
		GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_SPACE_PLAYER", iTempMod);
	}

	return iMultiplier;
}


int CvPlayer::getProductionModifier(SpecialistTypes, CvString* toolTipSink) const
{
	int iMultiplier = getProductionModifier(toolTipSink);
	int iTempMod;

	iTempMod = getSpecialistProductionModifier();
	iMultiplier += iTempMod;
	GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_SPECIALIST_PLAYER", iTempMod);

	return iMultiplier;
}


int CvPlayer::getProductionModifier(ProcessTypes             , CvString* toolTipSink) const
{
	int iMultiplier = getProductionModifier(toolTipSink);

	return iMultiplier;
}


int CvPlayer::getBuildingClassPrereqBuilding(BuildingTypes eBuilding, BuildingClassTypes ePrereqBuildingClass, int iExtra) const
{
	CvBuildingEntry* pkBuilding = GC.getBuildingInfo(eBuilding);
	if(pkBuilding == NULL)
	{
		CvAssertMsg(pkBuilding, "Should never happen...");
		return -1;
	}

	int iPrereqs = pkBuilding->GetPrereqNumOfBuildingClass(ePrereqBuildingClass);


	if(iPrereqs == 0)
	{
		return 0;
	}

	else if(iPrereqs == -1)
	{
		int iNonPuppetCities = 0;
		int iLoop = 0;
		const CvCity* pLoopCity = NULL;
		for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			if(pLoopCity && !pLoopCity->IsPuppet())
			{
				iNonPuppetCities++;
			}
		}

		return iNonPuppetCities;
	}
	else
	{
		iPrereqs *= std::max(0, GC.getMap().getWorldInfo().getBuildingClassPrereqModifier() + 100);
		iPrereqs /= 100;
	}

	if(!isLimitedWonderClass(pkBuilding->GetBuildingClassInfo()))
	{
		BuildingClassTypes eBuildingClass = (BuildingClassTypes)pkBuilding->GetBuildingClassType();
		iPrereqs *= (getBuildingClassCount(eBuildingClass) + iExtra + 1);
	}

	if(GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		iPrereqs = std::min(1, iPrereqs);
	}

	return iPrereqs;
}



void CvPlayer::removeBuildingClass(BuildingClassTypes eBuildingClass)
{
	CvCity* pLoopCity;
	BuildingTypes eBuilding;
	int iLoop;

	eBuilding = ((BuildingTypes)(getCivilizationInfo().getCivilizationBuildings(eBuildingClass)));

	if(eBuilding != NO_BUILDING)
	{
		for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			if(pLoopCity->GetCityBuildings()->GetNumRealBuilding(eBuilding) > 0)
			{
				pLoopCity->GetCityBuildings()->SetNumRealBuilding(eBuilding, 0);
				break;
			}
		}
	}
}



void CvPlayer::processBuilding(BuildingTypes eBuilding, int iChange, bool bFirst, CvArea* pArea)
{
#ifdef AUI_WARNING_FIXES
	uint iI;
	int iJ;
#else
	int iI, iJ;
#endif

	CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pBuildingInfo == NULL)
		return;
#if defined(LEKMOD_AREA_BASED_CITY_YIELD)
	CvArea* pLoopArea;
	CvMap& map = GC.getMap();
#endif

	if(bFirst && iChange > 0)
	{

		int iFreePolicies = pBuildingInfo->GetFreePolicies();
		if(iFreePolicies > 0)
			ChangeNumFreePolicies(iFreePolicies);

		int iFreeGreatPeople = pBuildingInfo->GetFreeGreatPeople();
		if(iFreeGreatPeople > 0)
			ChangeNumFreeGreatPeople(iFreeGreatPeople);


		int iFreeFlatFaith = pBuildingInfo->GetFreeFlatFaith();
		iFreeFlatFaith *= GC.getGame().getGameSpeedInfo().getTrainPercent();
		iFreeFlatFaith /= 100;
		if (iFreeFlatFaith > 0)
		{
			ChangeFaith(iFreeFlatFaith);
		}



		if(pBuildingInfo->IsGoldenAge())
		{
			int iGoldenAgeTurns = getGoldenAgeLength();
			changeGoldenAgeTurns(iGoldenAgeTurns);
		}


		if(pBuildingInfo->GetGlobalPopulationChange() != 0)
		{
			CvCity* pLoopCity;
			int iLoop;

			for(iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if(GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if(GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
					{
#ifdef AUI_WARNING_FIXES
						if (pBuildingInfo->IsTeamShare() || ((PlayerTypes)iI == GetID()))
#else
						if(pBuildingInfo->IsTeamShare() || (iI == GetID()))
#endif
						{
							for(pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
							{
								pLoopCity->setPopulation(std::max(1, (pLoopCity->getPopulation() + iChange * GC.getBuildingInfo(eBuilding)->GetGlobalPopulationChange())));
							}
						}
					}
				}
			}
		}


		if(pBuildingInfo->GetFreeTechs() > 0)
		{
			if(!isHuman())
			{
#ifdef AUI_WARNING_FIXES
				for (iJ = 0; iJ < pBuildingInfo->GetFreeTechs(); iJ++)
#else
				for(iI = 0; iI < pBuildingInfo->GetFreeTechs(); iI++)
#endif
				{
					for(int iLoop = 0; iLoop < iChange; iLoop++)
						AI_chooseFreeTech();
				}
			}
			else
			{
				Localization::String localizedText = Localization::Lookup("TXT_KEY_MISC_COMPLETED_WONDER_CHOOSE_TECH");
				localizedText << pBuildingInfo->GetTextKey();
				chooseTech(pBuildingInfo->GetFreeTechs() * iChange, localizedText.toUTF8());
			}
		}
		if(pBuildingInfo->GetMedianTechPercentChange() > 0)
		{
			ChangeMedianTechPercentage(pBuildingInfo->GetMedianTechPercentChange());
		}

		if(pBuildingInfo->GetExtraSpies() > 0)
		{
			CvPlayerEspionage* pEspionage = GetEspionage();
			CvAssertMsg(pEspionage, "pEspionage is null! What's up with that?!");
			if(pEspionage)
			{
				int iNumSpies = pBuildingInfo->GetExtraSpies();
				for(int i = 0; i < iNumSpies; i++)
				{
					pEspionage->CreateSpy();
				}
			}
		}

		if(pBuildingInfo->GetInstantSpyRankChange() > 0)
		{
			CvPlayerEspionage* pEspionage = GetEspionage();
			CvAssertMsg(pEspionage, "pEspionage is null! What's up with that?!");
			if(pEspionage)
			{
				for(uint ui = 0; ui < pEspionage->m_aSpyList.size(); ui++)
				{
					pEspionage->LevelUpSpy(ui);
				}
			}
		}

		if(pBuildingInfo->GetSpyRankChange() > 0)
		{
			ChangeStartingSpyRank(pBuildingInfo->GetSpyRankChange());
		}


		if(pBuildingInfo->GetGold() > 0)
			GetTreasury()->ChangeGold(pBuildingInfo->GetGold());


		int iMinorFriendshipChange = pBuildingInfo->GetMinorFriendshipChange();
		if(iMinorFriendshipChange != 0)
		{
			int iNewValue;
			iMinorFriendshipChange += 100;

			for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
			{
				iNewValue = GET_PLAYER((PlayerTypes) iMinorLoop).GetMinorCivAI()->GetBaseFriendshipWithMajorTimes100(GetID());
				iNewValue *= iMinorFriendshipChange;
				iNewValue /= 100;

				GET_PLAYER((PlayerTypes) iMinorLoop).GetMinorCivAI()->SetFriendshipWithMajorTimes100(GetID(), iNewValue);
			}
		}
	}

	if(pBuildingInfo->GetFreeBuildingClass() != NO_BUILDINGCLASS)
	{
		BuildingTypes eFreeBuilding = (BuildingTypes)getCivilizationInfo().getCivilizationBuildings(pBuildingInfo->GetFreeBuildingClass());
		changeFreeBuildingCount(eFreeBuilding, iChange);
	}


	ChangeUnitUpgradeCostMod(pBuildingInfo->GetUnitUpgradeCostMod() * iChange);


	ChangePolicyCostBuildingModifier(pBuildingInfo->GetPolicyCostModifier() * iChange);


	ChangePlotCultureCostModifier(pBuildingInfo->GetGlobalPlotCultureCostModifier() * iChange);
	ChangePlotGoldCostMod(pBuildingInfo->GetGlobalPlotBuyCostModifier() * iChange);


	ChangeJONSCultureCityModifier(pBuildingInfo->GetGlobalCultureRateModifier() * iChange);


	GetTreasury()->ChangeCityConnectionTradeRouteGoldModifier(pBuildingInfo->GetCityConnectionTradeRouteModifier() * iChange);


	PromotionTypes eFreePromotion = (PromotionTypes) pBuildingInfo->GetFreePromotion();
	if(eFreePromotion != NO_PROMOTION)
		ChangeFreePromotionCount(eFreePromotion, iChange);


	PromotionTypes eFreePromotionRemoved = (PromotionTypes) pBuildingInfo->GetFreePromotionRemoved();
	if(eFreePromotionRemoved != NO_PROMOTION)
		ChangeFreePromotionCount(eFreePromotionRemoved, -iChange);


	ChangeExtraHappinessPerCity(pBuildingInfo->GetHappinessPerCity() * iChange);


	ChangeExtraHappinessPerXPolicies(pBuildingInfo->GetHappinessPerXPolicies() * iChange);


	ChangeCityCountUnhappinessMod(pBuildingInfo->GetCityCountUnhappinessMod() * iChange);


	for(iI = 0; iI < GC.getNumHurryInfos(); iI++)
	{
		changeHurryModifier((HurryTypes) iI, (pBuildingInfo->GetHurryModifier(iI) * iChange));
	}

	changeGreatPeopleRateModFromBldgs(pBuildingInfo->GetGlobalGreatPeopleRateModifier() * iChange);
	changeGreatGeneralRateModFromBldgs(pBuildingInfo->GetGreatGeneralRateModifier() * iChange);
	ChangeGreatScientistBeakerMod(pBuildingInfo->GetGreatScientistBeakerModifier() * iChange);
	ChangeGreatPersonExpendGold(pBuildingInfo->GetGreatPersonExpendGold() * iChange);
	recomputeGreatPeopleModifiers();

	changeGoldenAgeModifier(pBuildingInfo->GetGoldenAgeModifier() * iChange);
	changeFreeExperienceFromBldgs(pBuildingInfo->GetGlobalFreeExperience() * iChange);
	changeWorkerSpeedModifier(pBuildingInfo->GetWorkerSpeedModifier() * iChange);
	ChangeSpecialistCultureChange(pBuildingInfo->GetSpecialistExtraCulture() * iChange);
	changeBorderObstacleCount(pBuildingInfo->IsPlayerBorderObstacle() * iChange);

	changeSpaceProductionModifier(pBuildingInfo->GetGlobalSpaceProductionModifier() * iChange);

	for(iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		YieldTypes eYield = static_cast<YieldTypes>(iI);
		pArea->changeYieldRateModifier(GetID(), eYield, (pBuildingInfo->GetAreaYieldModifier(iI) * iChange));
		changeYieldRateModifier(eYield, (pBuildingInfo->GetGlobalYieldModifier(iI) * iChange));
		for (iJ = 0; iJ < GC.getNumResourceInfos(); iJ++)
		{
			changeResourceYieldChange(((ResourceTypes)iJ), eYield, (pBuildingInfo->GetResourceYieldChangeGlobal((ResourceTypes)iJ, eYield) * iChange));
		}
#if defined(LEKMOD_AREA_BASED_CITY_YIELD)
		int iLoop;
		for (pLoopArea = map.firstArea(&iLoop); pLoopArea != NULL; pLoopArea = map.nextArea(&iLoop))
		{
			if (pLoopArea == pArea)
			{
				pLoopArea->changeCityYieldChange(GetID(), eYield, (pBuildingInfo->GetSameLandMassYieldChange(iI) * iChange));
			}
			else
			{
				pLoopArea->changeCityYieldChange(GetID(), eYield, (pBuildingInfo->GetDifferentLandMassYieldChange(iI) * iChange));
			}
		}
#endif
	}

	for(iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		for(iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			changeSpecialistExtraYield(((SpecialistTypes)iI), ((YieldTypes)iJ), (pBuildingInfo->GetSpecialistYieldChange(iI, iJ) * iChange));
		}
	}
	int iOldEspionageModifier = GetEspionageModifier();
	ChangeEspionageModifier(pBuildingInfo->GetGlobalEspionageModifier() * iChange);
	if(iOldEspionageModifier != GetEspionageModifier())
	{
		int iLoop;
		CvCity* pLoopCity;
		for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
			{
				PlayerTypes ePlayer = (PlayerTypes)ui;
				GET_PLAYER(ePlayer).GetEspionage()->UpdateCity(pLoopCity);
			}
		}
	}

	ChangeExtraLeagueVotes(pBuildingInfo->GetExtraLeagueVotes() * iChange);

	for(iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
	{
		YieldTypes eYield = (YieldTypes) iJ;
		for(int iK = 0; iK < GC.getNumImprovementInfos(); iK++)
		{
			ImprovementTypes eImprovement = (ImprovementTypes)iK;
			if(eImprovement != NO_IMPROVEMENT)
			{
				int iYieldChange = pBuildingInfo->GetImprovementYieldChangeGlobal(eImprovement, eYield);
				if(iYieldChange != 0)
				{
					ChangeImprovementExtraYield(eImprovement, eYield, (iYieldChange * iChange));
				}
			}
		}
	}


	int iLoop;
	CvCity* pLoopCity;
	int iBuildingCount;
	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{

		BuildingClassTypes eBuildingClass;
#if !defined(LEKMOD_NONCIV_BUILDINGCLASS_YIELD_CHANGE)
		for(iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			eBuildingClass = (BuildingClassTypes) iI;

			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
			if(!pkBuildingClassInfo)
			{
				continue;
			}

			eBuilding = (BuildingTypes) getCivilizationInfo().getCivilizationBuildings(eBuildingClass);

			if(eBuilding != NO_BUILDING)
			{
				CvBuildingEntry* pkBuilding = GC.getBuildingInfo(eBuilding);
				if(pkBuilding)
				{
#ifdef LEKMOD_FREE_BUILDING_FIX
					iBuildingCount = pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding);
#else
					iBuildingCount = pLoopCity->GetCityBuildings()->GetNumRealBuilding(eBuilding);
#endif
					if(iBuildingCount > 0)
					{
#if !defined(LEKMOD_NONCIV_BUILDINGCLASS_YIELD_CHANGE)

						pLoopCity->ChangeJONSCulturePerTurnFromBuildings(pBuildingInfo->GetBuildingClassYieldChange(eBuildingClass, YIELD_CULTURE) * iBuildingCount * iChange);


						for(iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
						{
							switch(iJ)
							{
							case YIELD_CULTURE:
							{

								break;
							}
							case YIELD_FAITH:
							{
								pLoopCity->ChangeFaithPerTurnFromBuildings(pBuildingInfo->GetBuildingClassYieldChange(eBuildingClass, iJ) * iBuildingCount * iChange);
								break;
							}
							default:
							{
								YieldTypes eYield = (YieldTypes) iJ;
								int iYieldChange = pBuildingInfo->GetBuildingClassYieldChange(eBuildingClass, eYield);
								if(iYieldChange > 0)
								{
									pLoopCity->ChangeBaseYieldRateFromBuildings(eYield, iYieldChange * iBuildingCount * iChange);
								}
							}
							}
						}
#else

						for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
						{
							YieldTypes eYield = (YieldTypes)iJ;
							int iYieldChange = pBuildingInfo->GetBuildingClassYieldChange(eBuildingClass, eYield);
							if (iYieldChange > 0)
							{
								pLoopCity->ChangeBaseYieldRateFromBuildings(eYield, iYieldChange * iBuildingCount * iChange);
							}
						}
#endif
					}
				}
			}
		}
#else
		for (int jJ = 0; jJ < GC.getNumBuildingInfos(); jJ++)
		{
			eBuilding = (BuildingTypes)jJ;
			if (eBuilding == NO_BUILDING)
				continue;
			CvBuildingEntry* pkBuilding = GC.getBuildingInfo(eBuilding);
			if (!pkBuilding)
				continue;
			eBuildingClass = (BuildingClassTypes)pkBuilding->GetBuildingClassType();
			if (eBuildingClass == NO_BUILDINGCLASS)
				continue;
			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
			if (!pkBuildingClassInfo)
				continue;
#ifdef LEKMOD_FREE_BUILDING_FIX
			iBuildingCount = pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding);
#else
			iBuildingCount = pLoopCity->GetCityBuildings()->GetNumRealBuilding(eBuilding);
#endif
			if (iBuildingCount > 0)
			{
#if !defined(STANDARDIZE_YIELDS)

				pLoopCity->ChangeJONSCulturePerTurnFromBuildings(pBuildingInfo->GetBuildingClassYieldChange(eBuildingClass, YIELD_CULTURE) * iBuildingCount * iChange);


				for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					switch (iJ)
					{
					case YIELD_CULTURE:
					{

						break;
					}
					case YIELD_FAITH:
					{
						pLoopCity->ChangeFaithPerTurnFromBuildings(pBuildingInfo->GetBuildingClassYieldChange(eBuildingClass, iJ) * iBuildingCount * iChange);
						break;
					}
					default:
					{
						YieldTypes eYield = (YieldTypes)iJ;
						int iYieldChange = pBuildingInfo->GetBuildingClassYieldChange(eBuildingClass, eYield);
						if (iYieldChange > 0)
							pLoopCity->ChangeBaseYieldRateFromBuildings(eYield, iYieldChange * iBuildingCount * iChange);
					}
					}
				}
#else

				for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					YieldTypes eYield = (YieldTypes)iJ;
					int iYieldChange = pBuildingInfo->GetBuildingClassYieldChange(eBuildingClass, eYield);
					if (iYieldChange > 0)
					{
						pLoopCity->ChangeBaseYieldRateFromBuildings(eYield, iYieldChange * iBuildingCount * iChange);
					}
				}
#endif
			}
		}
#endif
	}
}



int CvPlayer::GetBuildingClassYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYieldType)
{
	int rtnValue = 0;

	CvBuildingXMLEntries* pBuildings = GC.GetGameBuildings();

	if (pBuildings)
	{
#ifdef AUI_WARNING_FIXES
		for (uint i = 0; i < pBuildings->GetNumBuildings(); i++)
#else
		for (int i = 0; i < pBuildings->GetNumBuildings(); i++)
#endif
		{

			if (countNumBuildings((BuildingTypes)i) > 0)
			{
				CvBuildingEntry* pEntry = pBuildings->GetEntry(i);
				if (pEntry)
				{
					rtnValue += pEntry->GetBuildingClassYieldChange(eBuildingClass, eYieldType);
				}
			}
		}
	}

	return rtnValue;
}


bool CvPlayer::canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestEra, bool bTestVisible, bool bTestGold, bool bTestPlotOwner) const
{
	if(!(pPlot->canBuild(eBuild, GetID(), bTestVisible, bTestPlotOwner)))
	{
		return false;
	}

#ifdef AUI_WARNING_FIXES
	CvBuildInfo* pBuildInfo = GC.getBuildInfo(eBuild);
	if (!pBuildInfo)
		return false;

	if (pBuildInfo->getTechPrereq() != NO_TECH)
	{
		if (!(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)pBuildInfo->getTechPrereq())))
		{
			CvTechEntry* pTechInfo = GC.getTechInfo((TechTypes)pBuildInfo->getTechPrereq());
			if ((!bTestEra && !bTestVisible) || (pTechInfo && (GetCurrentEra() + 1) < pTechInfo->GetEra()))
#else
	if(GC.getBuildInfo(eBuild)->getTechPrereq() != NO_TECH)
	{
		if(!(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)GC.getBuildInfo(eBuild)->getTechPrereq())))
		{
			if((!bTestEra && !bTestVisible) || ((GetCurrentEra() + 1) < GC.getTechInfo((TechTypes) GC.getBuildInfo(eBuild)->getTechPrereq())->GetEra()))
#endif
			{
				return false;
			}
		}
	}


	ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(eBuild)->getImprovement();
	if(eImprovement != NO_IMPROVEMENT)
	{
		CvImprovementEntry* pkEntry = GC.getImprovementInfo(eImprovement);
		if(pkEntry->IsSpecificCivRequired())
		{
			CivilizationTypes eCiv = pkEntry->GetRequiredCivilization();
			if(eCiv != getCivilizationType())
			{
				return false;
			}
		}
	}

	if(!bTestVisible)
	{
		if(IsBuildBlockedByFeature(eBuild, pPlot->getFeatureType()))
		{
			return false;
		}

		if(bTestGold)
		{
			if(std::max(0, GetTreasury()->GetGold()) < getBuildCost(pPlot, eBuild))
			{
				return false;
			}
		}
	}

#ifdef LEKMOD_TRAIT_NO_BUILD_IMPROVEMENTS


	if (eImprovement != NO_IMPROVEMENT)
	{


		CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
		if (pkBuildInfo->IsSpecificCivRequired())
		{
			CivilizationTypes eCiv = pkBuildInfo->GetRequiredCivilization();
			if (eCiv != getCivilizationType())
			{
				return false;
			}
			else
			{
				return true;
			}
		}


		if (GetPlayerTraits()->NoBuild(eImprovement))
		{
			return false;
		}

	}
#endif

	return true;
}

#ifdef LEKMOD_NEW_ANCIENT_RUIN_REWARDS


bool CvPlayer::canBuildNoTech(const CvPlot* pPlot, BuildTypes eBuild, bool bTestVisible, bool bTestGold, bool bTestPlotOwner) const
{
	if (!(pPlot->canBuild(eBuild, GetID(), bTestVisible, bTestPlotOwner)))
	{
		return false;
	}


	ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(eBuild)->getImprovement();
	if (eImprovement != NO_IMPROVEMENT)
	{
		CvImprovementEntry* pkEntry = GC.getImprovementInfo(eImprovement);
		if (pkEntry->IsSpecificCivRequired())
		{
			CivilizationTypes eCiv = pkEntry->GetRequiredCivilization();
			if (eCiv != getCivilizationType())
			{
				return false;
			}
		}
	}

	if (!bTestVisible)
	{
		if (IsBuildBlockedByFeature(eBuild, pPlot->getFeatureType()))
		{
			return false;
		}

		if (bTestGold)
		{
			if (std::max(0, GetTreasury()->GetGold()) < getBuildCost(pPlot, eBuild))
			{
				return false;
			}
		}
	}

#ifdef LEKMOD_TRAIT_NO_BUILD_IMPROVEMENTS


	if (eImprovement != NO_IMPROVEMENT)
	{


		CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
		if (pkBuildInfo->IsSpecificCivRequired())
		{
			CivilizationTypes eCiv = pkBuildInfo->GetRequiredCivilization();
			if (eCiv != getCivilizationType())
			{
				return false;
			}
			else
			{
				return true;
			}
		}


		if (GetPlayerTraits()->NoBuild(eImprovement))
		{
			return false;
		}

	}
#endif

		return true;
}
#endif



bool CvPlayer::IsBuildBlockedByFeature(BuildTypes eBuild, FeatureTypes eFeature) const
{

	if(eFeature == NO_FEATURE)
	{
		return false;
	}


	if(!GC.getBuildInfo(eBuild)->isFeatureRemove(eFeature))
	{
		return false;
	}

	TechTypes ePrereqTech = (TechTypes) GC.getBuildInfo(eBuild)->getFeatureTech(eFeature);


	if(ePrereqTech == NO_TECH)
	{
		return false;
	}


	if(GET_TEAM(getTeam()).GetTeamTechs()->HasTech(ePrereqTech))
	{
		return false;
	}


	return true;
}



int CvPlayer::getBuildCost(const CvPlot* pPlot, BuildTypes eBuild) const
{
	CvAssert(eBuild >= 0 && eBuild < GC.getNumBuildInfos());


	CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
	if(pkBuildInfo == NULL)
	{
		return 0;
	}

	if(pPlot->getBuildProgress(eBuild) > 0)
	{
		return 0;
	}

	if(pPlot->getRouteType() != NO_ROUTE && pPlot->getRouteType() == pkBuildInfo->getRoute() && pPlot->IsRoutePillaged())
	{
		return 0;
	}

	int iBuildCost = pkBuildInfo->getCost();


	iBuildCost += (getTotalImprovementsBuilt() * pkBuildInfo->getCostIncreasePerImprovement());

	iBuildCost *= (100 + getImprovementCostModifier());
	iBuildCost /= 100;

	if(pPlot->getFeatureType() != NO_FEATURE)
	{
		iBuildCost += pkBuildInfo->getFeatureCost(pPlot->getFeatureType());
	}

	iBuildCost *= getHandicapInfo().getImprovementCostPercent();
	iBuildCost /= 100;

	iBuildCost *= GC.getGame().getGameSpeedInfo().getImprovementPercent();
	iBuildCost /= 100;

	return std::max(0, iBuildCost);
}



RouteTypes CvPlayer::getBestRoute(CvPlot* pPlot) const
{
	RouteTypes eRoute;
	RouteTypes eBestRoute;
	int iValue;
	int iBestValue;
#ifdef AUI_WARNING_FIXES
	uint iI;
#else
	int iI;
#endif

	iBestValue = 0;
	eBestRoute = NO_ROUTE;

	for(iI = 0; iI < GC.getNumBuildInfos(); iI++)
	{
		const BuildTypes eBuild = static_cast<BuildTypes>(iI);
		CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
		if(pkBuildInfo)
		{
			eRoute = ((RouteTypes)(pkBuildInfo->getRoute()));
			if(eRoute != NO_ROUTE)
			{
				CvRouteInfo* pkRouteInfo = GC.getRouteInfo(eRoute);
				if(pkRouteInfo)
				{
					if((pPlot != NULL) ? ((pPlot->getRouteType() == eRoute) || canBuild(pPlot, eBuild)) : GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)(pkBuildInfo->getTechPrereq())))
					{
						iValue = pkRouteInfo->getValue();

						if(iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestRoute = eRoute;
						}
					}
				}
			}
		}
	}

	return eBestRoute;
}



int CvPlayer::getImprovementUpgradeRate() const
{
	int iRate;

	iRate = 100;

	iRate *= std::max(0, (getImprovementUpgradeRateModifier() + 100));
	iRate /= 100;

	return iRate;
}



int CvPlayer::GetAllFeatureProduction() const
{
	return m_iAllFeatureProduction;
}



void CvPlayer::ChangeAllFeatureProduction(int iChange)
{
	if(iChange != 0)
	{
		m_iAllFeatureProduction += iChange;
	}
}



int CvPlayer::GetCityDistanceHighwaterMark() const
{
	return m_iCityDistanceHighwaterMark;
}



void CvPlayer::SetCityDistanceHighwaterMark(int iNewValue)
{
	m_iCityDistanceHighwaterMark = iNewValue;
}



int CvPlayer::calculateTotalYield(YieldTypes eYield) const
{
	const CvCity* pLoopCity;
	int iTotalYield = 0;
	int iLoop = 0;

	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iTotalYield += pLoopCity->getYieldRateTimes100(eYield, false);
	}

	return iTotalYield / 100;
}



int CvPlayer::GetUnitProductionMaintenanceMod() const
{

	return calculateUnitProductionMaintenanceMod();
}



void CvPlayer::UpdateUnitProductionMaintenanceMod()
{
	m_iUnitProductionMaintenanceMod = calculateUnitProductionMaintenanceMod();

	if(GetID() == GC.getGame().getActivePlayer())
	{
		GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	}
}



int CvPlayer::calculateUnitProductionMaintenanceMod() const
{
	int iPaidUnits = GetNumUnitsOutOfSupply();


	int iMaintenanceMod = min(       GC.getMAX_UNIT_SUPPLY_PRODMOD(), iPaidUnits * 10);
	iMaintenanceMod = -iMaintenanceMod;

	return iMaintenanceMod;
}



int CvPlayer::GetNumUnitsSupplied() const
{
	int iFreeUnits = GetNumUnitsSuppliedByHandicap();
	iFreeUnits += GetNumUnitsSuppliedByCities();
	iFreeUnits += GetNumUnitsSuppliedByPopulation();

	if(!isMinorCiv() && !isHuman() && !IsAITeammateOfHuman())
	{
		int iMod = (100 + GC.getGame().getHandicapInfo().getAIUnitSupplyPercent());
		iFreeUnits *= iMod;
		iFreeUnits /= 100;
	}

	return iFreeUnits;
}



int CvPlayer::GetNumUnitsSuppliedByHandicap() const
{
	return getHandicapInfo().getProductionFreeUnits();
}



int CvPlayer::GetNumUnitsSuppliedByCities() const
{
	return getHandicapInfo().getProductionFreeUnitsPerCity() * getNumCities();
}



int CvPlayer::GetNumUnitsSuppliedByPopulation() const
{
	return getTotalPopulation() * getHandicapInfo().getProductionFreeUnitsPopulationPercent() / 100;
}

#ifdef ENHANCED_GRAPHS


int CvPlayer::GetNumTrainedUnits() const
{
	return m_iNumTrainedUnits;
}
void CvPlayer::ChangeNumTrainedUnits(int iChange)
{
	m_iNumTrainedUnits = (m_iNumTrainedUnits + iChange);
}
int CvPlayer::GetNumKilledUnits() const
{
	return m_iNumKilledUnits;
}
void CvPlayer::ChangeNumKilledUnits(int iChange)
{
	m_iNumKilledUnits = (m_iNumKilledUnits + iChange);
}
int CvPlayer::GetNumLostUnits() const
{
	return m_iNumLostUnits;
}
void CvPlayer::ChangeNumLostUnits(int iChange)
{
	m_iNumLostUnits = (m_iNumLostUnits + iChange);
}
int CvPlayer::GetUnitsDamageDealt() const
{
	return m_iUnitsDamageDealt;
}
void CvPlayer::ChangeUnitsDamageDealt(int iChange)
{
	m_iUnitsDamageDealt = (m_iUnitsDamageDealt + iChange);
}
int CvPlayer::GetUnitsDamageTaken() const
{
	return m_iUnitsDamageTaken;
}
void CvPlayer::ChangeUnitsDamageTaken(int iChange)
{
	m_iUnitsDamageTaken = (m_iUnitsDamageTaken + iChange);
}
int CvPlayer::GetCitiesDamageDealt() const
{
	return m_iCitiesDamageDealt;
}
void CvPlayer::ChangeCitiesDamageDealt(int iChange)
{
	m_iCitiesDamageDealt = (m_iCitiesDamageDealt + iChange);
}
int CvPlayer::GetCitiesDamageTaken() const
{
	return m_iCitiesDamageTaken;
}
void CvPlayer::ChangeCitiesDamageTaken(int iChange)
{
	m_iCitiesDamageTaken = (m_iCitiesDamageTaken + iChange);
}
int CvPlayer::GetNumScientistsTotal() const
{
	return m_iNumScientistsTotal;
}
void CvPlayer::ChangeNumScientistsTotal(int iChange)
{
	m_iNumScientistsTotal = (m_iNumScientistsTotal + iChange);
}
int CvPlayer::GetNumEngineersTotal() const
{
	return m_iNumEngineersTotal;
}
void CvPlayer::ChangeNumEngineersTotal(int iChange)
{
	m_iNumEngineersTotal = (m_iNumEngineersTotal + iChange);
}
int CvPlayer::GetNumMerchantsTotal() const
{
	return m_iNumMerchantsTotal;
}
void CvPlayer::ChangeNumMerchantsTotal(int iChange)
{
	m_iNumMerchantsTotal = (m_iNumMerchantsTotal + iChange);
}
int CvPlayer::GetNumWritersTotal() const
{
	return m_iNumWritersTotal;
}
void CvPlayer::ChangeNumWritersTotal(int iChange)
{
	m_iNumWritersTotal = (m_iNumWritersTotal + iChange);
}
int CvPlayer::GetNumAristsTotal() const
{
	return m_iNumArtistsTotal;
}
void CvPlayer::ChangeNumArtistsTotal(int iChange)
{
	m_iNumArtistsTotal = (m_iNumArtistsTotal + iChange);
}
int CvPlayer::GetNumMusiciansTotal() const
{
	return m_iNumMusiciansTotal;
}
void CvPlayer::ChangeNumMusiciansTotal(int iChange)
{
	m_iNumMusiciansTotal = (m_iNumMusiciansTotal + iChange);
}
int CvPlayer::GetNumGeneralsTotal() const
{
	return m_iNumGeneralsTotal;
}
void CvPlayer::ChangeNumGeneralsTotal(int iChange)
{
	m_iNumGeneralsTotal = (m_iNumGeneralsTotal + iChange);
}
int CvPlayer::GetNumAdmiralsTotal() const
{
	return m_iNumAdmiralsTotal;
}
void CvPlayer::ChangeNumAdmiralsTotal(int iChange)
{
	m_iNumAdmiralsTotal = (m_iNumAdmiralsTotal + iChange);
}
int CvPlayer::GetNumProphetsTotal() const
{
	return m_iNumProphetsTotal;
}
void CvPlayer::ChangeNumProphetsTotal(int iChange)
{
	m_iNumProphetsTotal = (m_iNumProphetsTotal + iChange);
}
int CvPlayer::GetProductionGoldFromWonders() const
{
	return m_iProductionGoldFromWonders;
}
void CvPlayer::ChangeProductionGoldFromWonders(int iChange)
{
	m_iProductionGoldFromWonders = (m_iProductionGoldFromWonders + iChange);
}
int CvPlayer::GetNumChops() const
{
	return m_iNumChops;
}
void CvPlayer::ChangeNumChops(int iChange)
{
	m_iNumChops = (m_iNumChops + iChange);
}
int CvPlayer::GetNumTimesOpenedDemographics() const
{
	return m_iNumTimesOpenedDemographics;
}
void CvPlayer::ChangeNumTimesOpenedDemographics(int iChange)
{
	m_iNumTimesOpenedDemographics = (m_iNumTimesOpenedDemographics + iChange);
}

int CvPlayer::GetUnitsDamageHealed() const
{
	return m_iUnitsDamageHealed;
}

void CvPlayer::ChangeUnitsDamageHealed(int iChange)
{
	m_iUnitsDamageHealed += iChange;
}

int CvPlayer::GetTurnsStagnated() const
{
	return m_iTurnsStagnated;
}

void CvPlayer::ChangeTurnsStagnated(int iChange)
{
	m_iTurnsStagnated += iChange;
}

int CvPlayer::GetGoldSpentBuys() const
{
	return m_iGoldSpentBuys;
}

void CvPlayer::ChangeGoldSpentBuys(int iChange)
{
	m_iGoldSpentBuys += iChange;
}

int CvPlayer::GetGoldSpentUpgrades() const
{
	return m_iGoldSpentUpgrades;
}

void CvPlayer::ChangeGoldSpentUpgrades(int iChange)
{
	m_iGoldSpentUpgrades += iChange;
}

bool CvPlayer::GetMayaBoostScientist() const
{
	return m_bMayaBoostScientist;
}
void CvPlayer::SetMayaBoostScientist(bool bValue)
{
	m_bMayaBoostScientist = bValue;
}
bool CvPlayer::GetMayaBoostEngineers() const
{
	return m_bMayaBoostEngineers;
}
void CvPlayer::SetMayaBoostEngineers(bool bValue)
{
	m_bMayaBoostEngineers = bValue;
}
bool CvPlayer::GetMayaBoostMerchants() const
{
	return m_bMayaBoostMerchants;
}
void CvPlayer::SetMayaBoostMerchants(bool bValue)
{
	m_bMayaBoostMerchants = bValue;
}
bool CvPlayer::GetMayaBoostWriters() const
{
	return m_bMayaBoostWriters;
}
void CvPlayer::SetMayaBoostWriters(bool bValue)
{
	m_bMayaBoostWriters = bValue;
}
bool CvPlayer::GetMayaBoostArtists() const
{
	return m_bMayaBoostArtists;
}
void CvPlayer::SetMayaBoostArtists(bool bValue)
{
	m_bMayaBoostArtists = bValue;
}
bool CvPlayer::GetMayaBoostMusicians() const
{
	return m_bMayaBoostMusicians;
}
void CvPlayer::SetMayaBoostMusicians(bool bValue)
{
	m_bMayaBoostMusicians = bValue;
}
#endif



int CvPlayer::GetNumUnitsOutOfSupply() const
{
	int iFreeUnits = GetNumUnitsSupplied();
	int iNumUnits = getNumUnits();

	int iNumTradeUnits = 0;
	int iLoop = 0;
	const CvUnit* pLoopUnit = NULL;
	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if(pLoopUnit->isTrade())
		{
			iNumTradeUnits++;
		}
	}

	int iNumUnitsToSupply = iNumUnits - iNumTradeUnits;
	return std::max(0, iNumUnitsToSupply - iFreeUnits);
}


int CvPlayer::calculateUnitCost() const
{
	int iFreeUnits;
	int iPaidUnits;
	int iBaseUnitCost;
	int iExtraCost;

	return GetTreasury()->CalculateUnitCost(iFreeUnits, iPaidUnits, iBaseUnitCost, iExtraCost);
}


int CvPlayer::calculateUnitSupply() const
{
	int iPaidUnits;
	int iBaseSupplyCost;

	return GetTreasury()->CalculateUnitSupply(iPaidUnits, iBaseSupplyCost);
}


int CvPlayer::calculateResearchModifier(TechTypes eTech)
{
#ifdef AUI_TECH_FIX_TEAMER_RESEARCH_COSTS
	return GET_TEAM(getTeam()).calculateResearchModifier(eTech);
#else
	int iModifier = 100;

	if(NO_TECH == eTech)
	{
		return iModifier;
	}

#ifdef AUI_TECH_TOGGLEABLE_ALREADY_KNOWN_TECH_COST_DISCOUNT
	if (!GC.getGame().isOption("GAMEOPTION_NO_TECH_COST_TOTAL_KNOWN_TEAM_MODIFIER"))
	{
#endif
	int iLeaguesMod = GC.getGame().GetGameLeagues()->GetResearchMod(GetID(), eTech);
	int iKnownCount = 0;
	int iPossibleKnownCount = 0;
	for(int iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
		if(kLoopTeam.isAlive() && !kLoopTeam.isMinorCiv())
		{
			if(GET_TEAM(getTeam()).isHasMet((TeamTypes)iI))
			{
#ifdef HAS_TECH_BY_HUMAN
				if (GC.getGame().isOption("GAMEOPTION_NO_AI_TECH_DISCOUNT"))
				{
					if (kLoopTeam.GetTeamTechs()->HasTechByHuman(eTech))
#else
					if (kLoopTeam.GetTeamTechs()->HasTech(eTech))
#endif
					{
						iKnownCount++;
					}
#ifdef HAS_TECH_BY_HUMAN
				}
				else
				{
					if (kLoopTeam.GetTeamTechs()->HasTech(eTech))
					{
						iKnownCount++;
					}
				}
#endif
			}
			iPossibleKnownCount++;
		}
	}
	if(iPossibleKnownCount > 0)
	{
		if (iLeaguesMod == 1)
		{	
			iModifier += ((GC.getTECH_COST_TOTAL_KNOWN_TEAM_MODIFIER() * iKnownCount) * 2 ) / iPossibleKnownCount;
		}
		else
			iModifier += (GC.getTECH_COST_TOTAL_KNOWN_TEAM_MODIFIER() * iKnownCount) / iPossibleKnownCount;
	}	
#ifdef AUI_TECH_TOGGLEABLE_ALREADY_KNOWN_TECH_COST_DISCOUNT
	}
#endif

	int iPossiblePaths = 0;
	int iUnknownPaths = 0;
	for(int iI = 0; iI < GC.getNUM_OR_TECH_PREREQS(); iI++)
	{
		if(GC.getTechInfo(eTech)->GetPrereqOrTechs(iI) != NO_TECH)
		{
			if(!(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)(GC.getTechInfo(eTech)->GetPrereqOrTechs(iI)))))
			{
				iUnknownPaths++;
			}

			iPossiblePaths++;
		}
	}
	CvAssertMsg(iPossiblePaths >= iUnknownPaths, "The number of possible paths is expected to match or exceed the number of unknown ones");
	iModifier += (iPossiblePaths - iUnknownPaths) * GC.getTECH_COST_KNOWN_PREREQ_MODIFIER();


	int iLeaguesMod = GC.getGame().GetGameLeagues()->GetResearchMod(GetID(), eTech);
	if (iLeaguesMod != 0)
	{
		iModifier *= 100 + iLeaguesMod;
		iModifier /= 100;
	}

	return iModifier;
#endif
}


int CvPlayer::calculateGoldRate() const
{
	return calculateGoldRateTimes100() / 100;
}



int CvPlayer::calculateGoldRateTimes100() const
{

	if(IsAnarchy())
	{
		return 0;
	}

	int iRate = 0;

	iRate = GetTreasury()->CalculateBaseNetGoldTimes100();

	return iRate;
}


int CvPlayer::unitsRequiredForGoldenAge() const
{
	return (GC.getBASE_GOLDEN_AGE_UNITS() + (getNumUnitGoldenAges() * GC.getGOLDEN_AGE_UNITS_MULTIPLIER()));
}



int CvPlayer::unitsGoldenAgeCapable() const
{
	const CvUnit* pLoopUnit;
	int iCount;
	int iLoop;

	iCount = 0;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if(pLoopUnit->isGoldenAge())
		{
			iCount++;
		}
	}

	return iCount;
}



int CvPlayer::unitsGoldenAgeReady() const
{
	const CvUnit* pLoopUnit;
#ifdef AUI_WARNING_FIXES

	FStaticVector<UnitTypes, 2, true, c_eCiv5GameplayDLL> pabUnitUsed;
	FStaticVector<UnitTypes, 2, true, c_eCiv5GameplayDLL>::iterator it;

	int iCount = 0;
	int iLoop = 0;
	UnitTypes eLoopUnitType = NO_UNIT;
#else
	bool* pabUnitUsed;
	int iCount;
	int iLoop;
	int iI;

	pabUnitUsed = FNEW(bool[GC.getNumUnitInfos()], c_eCiv5GameplayDLL, 0);

	for(iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		pabUnitUsed[iI] = false;
	}

	iCount = 0;
#endif

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
#ifdef AUI_WARNING_FIXES
		if (pLoopUnit->isGoldenAge())
		{
			eLoopUnitType = pLoopUnit->getUnitType();
			for (it = pabUnitUsed.begin(); it != pabUnitUsed.end(); ++it)
			{
				if (*it == eLoopUnitType)
				{
					goto NextUnit;
				}
			}
			pabUnitUsed.push_back(eLoopUnitType);
			iCount++;
		NextUnit:;
#else
		if(!(pabUnitUsed[pLoopUnit->getUnitType()]))
		{
			if(pLoopUnit->isGoldenAge())
			{
				pabUnitUsed[pLoopUnit->getUnitType()] = true;
				iCount++;
			}
#endif
		}
	}

#ifndef AUI_WARNING_FIXES
	SAFE_DELETE_ARRAY(pabUnitUsed);
#endif

	return iCount;
}


int CvPlayer::greatGeneralThreshold() const
{
	int iThreshold;

	iThreshold = ((        GC.getGREAT_GENERALS_THRESHOLD() * std::max(0, (getGreatGeneralsThresholdModifier() + 100))) / 100);

	iThreshold *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iThreshold /= std::max(1, GC.getGame().getGameSpeedInfo().getTrainPercent());

	iThreshold *= GC.getGame().getStartEraInfo().getGreatPeoplePercent();
	iThreshold /= 100;

	return std::max(1, iThreshold);
}


int CvPlayer::greatAdmiralThreshold() const
{
	int iThreshold;

	iThreshold = ((        GC.getGREAT_GENERALS_THRESHOLD() * std::max(0, (getGreatAdmiralsThresholdModifier() + 100))) / 100);

	iThreshold *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iThreshold /= std::max(1, GC.getGame().getGameSpeedInfo().getTrainPercent());

	iThreshold *= GC.getGame().getStartEraInfo().getGreatPeoplePercent();
	iThreshold /= 100;

	return std::max(1, iThreshold);
}
#if !defined(TRAITIFY)

int CvPlayer::specialistYield(SpecialistTypes eSpecialist, YieldTypes eYield) const
{
	CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
	if(pkSpecialistInfo == NULL)
	{

		CvAssert(pkSpecialistInfo);
		return 0;
	}
#ifndef LEK_TRAIT_SPECIALIST_YIELD_MAX_ONE
	int iRtnValue = pkSpecialistInfo->getYieldChange(eYield) + getSpecialistExtraYield(eSpecialist, eYield) + GetPlayerTraits()->GetSpecialistYieldChange(eSpecialist, eYield);
#else
	int iRtnValue = pkSpecialistInfo->getYieldChange(eYield) + getSpecialistExtraYield(eSpecialist, eYield) + GetPlayerTraits()->GetSpecialistYieldChange(eSpecialist, eYield) + GetPlayerTraits()->GetAnySpecificSpecialistYieldChange(eSpecialist, eYield);
#endif
	if (eSpecialist !=                  GC.getDEFAULT_SPECIALIST())
	{
		iRtnValue += getSpecialistExtraYield(eYield);
	}
	return (iRtnValue);
}
#else 

int CvPlayer::specialistYield(SpecialistTypes eSpecialist, YieldTypes eYield, bool bExtraOnly) const
{
	CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
	if (pkSpecialistInfo == NULL)
	{

		CvAssert(pkSpecialistInfo);
		return 0;
	}
	int iRtnValue = 0;
	if(!bExtraOnly)
	{
		iRtnValue = pkSpecialistInfo->getYieldChange(eYield);
	}
#ifndef LEK_TRAIT_SPECIALIST_YIELD_MAX_ONE
	iRtnValue += getSpecialistExtraYield(eSpecialist, eYield) + GetPlayerTraits()->GetSpecialistYieldChange(eSpecialist, eYield);
#else
	iRtnValue += getSpecialistExtraYield(eSpecialist, eYield) + GetPlayerTraits()->GetSpecialistYieldChange(eSpecialist, eYield) + GetPlayerTraits()->GetAnySpecificSpecialistYieldChange(eSpecialist, eYield);
#endif
	if (eSpecialist !=                  GC.getDEFAULT_SPECIALIST())
	{
		iRtnValue += getSpecialistExtraYield(eYield);
	}
	return (iRtnValue);
}
#endif
#if defined(LEKMOD_EXPERIMENTAL_CHANGES)
int CvPlayer::GetWorldWonderYieldChanges(YieldTypes eYield) const
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiWorldWonderYieldChanges[eYield];
}
void CvPlayer::ChangeWorldWonderYieldChanges(YieldTypes eYield, int iChange)
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	if(iChange != 0)
	{
		m_aiWorldWonderYieldChanges.setAt(eYield, m_aiWorldWonderYieldChanges[eYield] + iChange);

		CvCity* pLoopCity;
		int iLoop;
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			int totalChange = iChange * pLoopCity->getNumWorldWonders();
			pLoopCity->ChangeBaseYieldRateFromBuildings(eYield, totalChange);
		}
	}
}
#endif


int CvPlayer::GetCityYieldChange(YieldTypes eYield) const
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCityYieldChange[eYield];
}



void CvPlayer::ChangeCityYieldChange(YieldTypes eYield, int iChange)
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_aiCityYieldChange.setAt(eYield, m_aiCityYieldChange[eYield] + iChange);

		updateYield();
	}
}



int CvPlayer::GetCoastalCityYieldChange(YieldTypes eYield) const
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCoastalCityYieldChange[eYield];
}



void CvPlayer::ChangeCoastalCityYieldChange(YieldTypes eYield, int iChange)
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_aiCoastalCityYieldChange.setAt(eYield, m_aiCoastalCityYieldChange[eYield] + iChange);

		updateYield();
	}
}



int CvPlayer::GetCapitalYieldChange(YieldTypes eYield) const
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCapitalYieldChange[eYield];
}



void CvPlayer::ChangeCapitalYieldChange(YieldTypes eYield, int iChange)
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_aiCapitalYieldChange.setAt(eYield, m_aiCapitalYieldChange[eYield] + iChange);

		updateYield();
	}
}



int CvPlayer::GetCapitalYieldPerPopChange(YieldTypes eYield) const
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCapitalYieldPerPopChange[eYield];
}



void CvPlayer::ChangeCapitalYieldPerPopChange(YieldTypes eYield, int iChange)
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_aiCapitalYieldPerPopChange.setAt(eYield, m_aiCapitalYieldPerPopChange[eYield] + iChange);

		updateYield();
	}
}
#if !defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)


int CvPlayer::GetGreatWorkYieldChange(YieldTypes eYield) const
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiGreatWorkYieldChange[eYield];
}



void CvPlayer::ChangeGreatWorkYieldChange(YieldTypes eYield, int iChange)
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_aiGreatWorkYieldChange[eYield] = m_aiGreatWorkYieldChange[eYield] + iChange;
	}
}
#else


void CvPlayer::ChangeGreatWorkYieldChange(YieldTypes eYield, int iChange)
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	for (int iI = 0; iI < GC.getNumGreatWorkClassInfos(); iI++)
	{
		ChangeGreatWorkClassYieldChange((GreatWorkClass)iI, eYield, iChange);
	}
}


int CvPlayer::GetGreatWorkClassYieldChange(GreatWorkClass eGreatWorkClass, YieldTypes eYield) const
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eGreatWorkClass >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eGreatWorkClass < GC.getNumGreatWorkClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_ppaaiGreatWorkClassYieldChange[eGreatWorkClass][eYield];
}


void CvPlayer::ChangeGreatWorkClassYieldChange(GreatWorkClass eGreatWorkClass, YieldTypes eYield, int iChange)
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eGreatWorkClass >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eGreatWorkClass < GC.getNumGreatWorkClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	if (iChange != 0)
	{
		Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiGreatWorkClassYieldChange[eGreatWorkClass];
		yields[eYield] = (m_ppaaiGreatWorkClassYieldChange[eGreatWorkClass][eYield] + iChange);
		m_ppaaiGreatWorkClassYieldChange.setAt(eGreatWorkClass, yields);
	}
	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->GetCityBuildings()->rebuildGreatWorkYields(eGreatWorkClass);
	}
}
#endif

CvPlot* CvPlayer::getStartingPlot() const
{
	return GC.getMap().plotCheckInvalid(m_iStartingX, m_iStartingY);
}



void CvPlayer::setStartingPlot(CvPlot* pNewValue)
{
	CvPlot* pOldStartingPlot;

	pOldStartingPlot = getStartingPlot();

	if(pOldStartingPlot != pNewValue)
	{
		if(pOldStartingPlot != NULL)
		{
			pOldStartingPlot->area()->changeNumStartingPlots(-1);
		}

		if(pNewValue == NULL)
		{
			m_iStartingX = INVALID_PLOT_COORD;
			m_iStartingY = INVALID_PLOT_COORD;
		}
		else
		{
			m_iStartingX = pNewValue->getX();
			m_iStartingY = pNewValue->getY();

			getStartingPlot()->setStartingPlot(true);

			CvArea* pArea = getStartingPlot()->area();
			if(pArea != NULL)
				pArea->changeNumStartingPlots(1);
		}
	}
}



int CvPlayer::getTotalPopulation() const
{
	return m_iTotalPopulation;
}



int CvPlayer::getAveragePopulation() const
{
	if(getNumCities() == 0)
	{
		return 0;
	}

	return ((getTotalPopulation() / getNumCities()) + 1);
}



void CvPlayer::changeTotalPopulation(int iChange)
{
	m_iTotalPopulation = (m_iTotalPopulation + iChange);
	CvAssert(getTotalPopulation() >= 0);
}



long CvPlayer::getRealPopulation() const
{
	const CvCity* pLoopCity;
	__int64 iTotalPopulation = 0;
	int iLoop = 0;

	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iTotalPopulation += pLoopCity->getRealPopulation();
	}

	if(iTotalPopulation > INT_MAX)
	{
		iTotalPopulation = INT_MAX;
	}

	return ((long)(iTotalPopulation));
}


int CvPlayer::GetNewCityExtraPopulation() const
{
#if !defined(TRAITIFY)
	return m_iNewCityExtraPopulation;
#else
	return m_iNewCityExtraPopulation + GetPlayerTraits()->GetExtraPopulation();
#endif
}


void CvPlayer::ChangeNewCityExtraPopulation(int iChange)
{
	if(iChange != 0)
	{
		m_iNewCityExtraPopulation += iChange;
	}
}


int CvPlayer::GetFreeFoodBox() const
{
	return m_iFreeFoodBox;
}


void CvPlayer::ChangeFreeFoodBox(int iChange)
{
	if(iChange != 0)
	{
		m_iFreeFoodBox += iChange;
	}
}



int CvPlayer::getTotalLand() const
{
	return m_iTotalLand;
}



void CvPlayer::changeTotalLand(int iChange)
{
	m_iTotalLand = (m_iTotalLand + iChange);
	CvAssert(getTotalLand() >= 0);
}



int CvPlayer::getTotalLandScored() const
{
	return m_iTotalLandScored;
}



void CvPlayer::changeTotalLandScored(int iChange)
{
	if(iChange != 0)
	{
		m_iTotalLandScored = (m_iTotalLandScored + iChange);
		CvAssert(getTotalLandScored() >= 0);
	}
}


int CvPlayer::GetTotalJONSCulturePerTurn() const
#ifdef AUI_PLAYER_FIX_JONS_CULTURE_IS_T100
{
	return GetTotalJONSCulturePerTurnTimes100() / 100;
}

int CvPlayer::GetTotalJONSCulturePerTurnTimes100() const
#endif
{
	if(GC.getGame().isOption(GAMEOPTION_NO_POLICIES))
	{
		return 0;
	}


	if(IsAnarchy())
	{
		return 0;
	}
#if !defined(STANDARDIZE_YIELDS)
	int iCulturePerTurn = 0;


#ifdef AUI_PLAYER_FIX_JONS_CULTURE_IS_T100
	iCulturePerTurn += GetJONSCulturePerTurnFromCitiesTimes100();


	iCulturePerTurn += GetJONSCulturePerTurnFromExcessHappinessTimes100();


	iCulturePerTurn += GetJONSCulturePerTurnFromTraits() * 100;


	iCulturePerTurn += GetJONSCulturePerTurnForFree() * 100;


	iCulturePerTurn += GetCulturePerTurnFromMinorCivs() * 100;


	iCulturePerTurn += GetCulturePerTurnFromReligionTimes100();


	iCulturePerTurn += GetCulturePerTurnFromBonusTurnsTimes100();
#else
	iCulturePerTurn += GetJONSCulturePerTurnFromCities();


	iCulturePerTurn += GetJONSCulturePerTurnFromExcessHappiness();


	iCulturePerTurn += GetJONSCulturePerTurnFromTraits();


	iCulturePerTurn += GetJONSCulturePerTurnForFree();


	iCulturePerTurn += GetCulturePerTurnFromMinorCivs();


	iCulturePerTurn += GetCulturePerTurnFromReligion();
	

	iCulturePerTurn += GetCulturePerTurnFromBonusTurns();
#endif

	if (isGoldenAge() && !IsGoldenAgeCultureBonusDisabled())
	{
#if !defined(TRAITIFY)
#if !defined(LEKMOD_PLAYER_GOLDEN_AGE_YIELD_MOD_INFO)
		
		iCulturePerTurn += ((iCulturePerTurn * GC.getGOLDEN_AGE_CULTURE_MODIFIER()) / 100);
#else
		const CvYieldInfo& kYield = *GC.getYieldInfo(YIELD_CULTURE);
		iCulturePerTurn += ((iCulturePerTurn * kYield.getPlayerGoldenAgeYieldMod()) / 100);
#endif
#else
		CvPlayerTraits* pPlayerTraits = GetPlayerTraits();
#if defined(LEKMOD_PLAYER_GOLDEN_AGE_YIELD_MOD_INFO)
		const CvYieldInfo& kYield = *GC.getYieldInfo(YIELD_CULTURE);
		iCulturePerTurn += ((iCulturePerTurn * (kYield.getPlayerGoldenAgeYieldMod() + pPlayerTraits->GetGoldenAgeYieldModifier(YIELD_CULTURE))) / 100);
#else
		iCulturePerTurn += ((iCulturePerTurn * (GC.getGOLDEN_AGE_CULTURE_MODIFIER() + pPlayerTraits->GetGoldenAgeYieldModifier(YIELD_CULTURE))) / 100);
#endif
#endif
	}
	return iCulturePerTurn;
#else
	return getYieldTimes100(YIELD_CULTURE, false                 );
#endif
}



int CvPlayer::GetJONSCulturePerTurnFromCities() const
#ifdef AUI_PLAYER_FIX_JONS_CULTURE_IS_T100
{
#if !defined(STANDARDIZE_YIELDS)
	return GetJONSCulturePerTurnFromCitiesTimes100() / 100;
#else
	return getYieldFromCitiesTimes100(YIELD_CULTURE, false                 ) / 100;
#endif
}

int CvPlayer::GetJONSCulturePerTurnFromCitiesTimes100() const
#endif
{
#if !defined(STANDARDIZE_YIELDS)
	int iCulturePerTurn = 0;


	const CvCity* pLoopCity;
	int iLoop;
	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
#if !defined(STANDARDIZE_YIELDS)
#ifdef AUI_PLAYER_FIX_JONS_CULTURE_IS_T100
		iCulturePerTurn += pLoopCity->getJONSCulturePerTurnTimes100();
#else
		iCulturePerTurn += pLoopCity->getJONSCulturePerTurn();
#endif
#else
		iCulturePerTurn += pLoopCity->getYieldRateTimes100(YIELD_CULTURE, false                 );
#endif
	}

	return iCulturePerTurn;
#else
	return getYieldFromCitiesTimes100(YIELD_CULTURE, false                 );
#endif
}



int CvPlayer::GetJONSCulturePerTurnFromExcessHappiness() const
#ifdef AUI_PLAYER_FIX_JONS_CULTURE_IS_T100
{
	return GetJONSCulturePerTurnFromExcessHappinessTimes100() / 100;
}

int CvPlayer::GetJONSCulturePerTurnFromExcessHappinessTimes100() const
#endif
{
#if !defined(STANDARDIZE_YIELDS)
	if(GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return 0;
	}

	if(getHappinessToCulture() != 0)
	{
		if(GetExcessHappiness() > 0)
		{
			int iFreeCulture = GetExcessHappiness() * getHappinessToCulture();
#ifndef AUI_PLAYER_FIX_JONS_CULTURE_IS_T100
			iFreeCulture /= 100;
#endif

			return iFreeCulture;
		}
	}

	return 0;
#else
	return getYieldFromHappinessTimes100(YIELD_CULTURE);
#endif
}



int CvPlayer::GetJONSCulturePerTurnFromTraits() const
{
#if !defined(STANDARDIZE_YIELDS)

	int bonus = GetPlayerTraits()->GetYieldChangePerTradePartner(YIELD_CULTURE);
	if (bonus > 0)
	{
		bonus += GetPlayerTraits()->GetTradePartnerYieldFlatBonusPerEra() * GetCurrentEra();
		bonus *= GetTrade()->GetNumDifferentTradingPartners();
	}
	return bonus;


#else
	return getYieldFromTraitsTimes100(YIELD_CULTURE) / 100;
#endif
}



int CvPlayer::GetJONSCulturePerTurnForFree() const
{
#if !defined(STANDARDIZE_YIELDS)
	return m_iJONSCulturePerTurnForFree;
#else
	return GetYieldPerTurnFromMisc(YIELD_CULTURE);
#endif
}



void CvPlayer::ChangeJONSCulturePerTurnForFree(int iChange)
{
#if !defined(STANDARDIZE_YIELDS)
	if(iChange != 0)
		m_iJONSCulturePerTurnForFree += iChange;

	if(GC.getGame().getActivePlayer() == GetID())
	{
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	}
#else
	ChangeYieldPerTurnFromMisc(YIELD_CULTURE, iChange);
#endif
}



int CvPlayer::GetJONSCulturePerTurnFromMinorCivs() const
{
	return GetCulturePerTurnFromMinorCivs();
}



void CvPlayer::ChangeJONSCulturePerTurnFromMinorCivs(int            )
{
	CvAssertMsg(false, "ChangeJONSCulturePerTurnFromMinorCivs called, but Anton meant to disable it");
}



int CvPlayer::GetCulturePerTurnFromMinorCivs() const
{
#if !defined(STANDARDIZE_YIELDS)
	int iAmount = 0;
	PlayerTypes eMinor;
	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		eMinor = (PlayerTypes) iMinorLoop;
		iAmount += GetCulturePerTurnFromMinor(eMinor);
	}

	return iAmount;
#else
	return getYieldFromMinorCivsTimes100(YIELD_CULTURE) / 100;
#endif
}



int CvPlayer::GetCulturePerTurnFromMinor(PlayerTypes eMinor) const
{
	int iAmount = 0;

	if(GET_PLAYER(eMinor).isAlive())
	{

		iAmount += GET_PLAYER(eMinor).GetMinorCivAI()->GetCurrentCultureBonus(GetID());
	}

	return iAmount;
}



int CvPlayer::GetCulturePerTurnFromReligion() const
#ifdef AUI_PLAYER_FIX_JONS_CULTURE_IS_T100
{
	return GetCulturePerTurnFromReligionTimes100() / 100;
}

int CvPlayer::GetCulturePerTurnFromReligionTimes100() const
#endif
{
#if !defined(STANDARDIZE_YIELDS)
	int iOtherCulturePerTurn = 0;
	int iReligionCulturePerTurn = 0;


#ifdef AUI_PLAYER_FIX_JONS_CULTURE_IS_T100
	iOtherCulturePerTurn += GetJONSCulturePerTurnFromCitiesTimes100();
	iOtherCulturePerTurn += GetJONSCulturePerTurnFromExcessHappinessTimes100();
	iOtherCulturePerTurn += GetJONSCulturePerTurnForFree() * 100;
	iOtherCulturePerTurn += GetCulturePerTurnFromMinorCivs() * 100;
#else
	iOtherCulturePerTurn += GetJONSCulturePerTurnFromCities();
	iOtherCulturePerTurn += GetJONSCulturePerTurnFromExcessHappiness();
	iOtherCulturePerTurn += GetJONSCulturePerTurnForFree();
	iOtherCulturePerTurn += GetCulturePerTurnFromMinorCivs();
#endif


	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	ReligionTypes eFoundedReligion = pReligions->GetFounderBenefitsReligion(GetID());
	if(eFoundedReligion != NO_RELIGION)
	{
		const CvReligion* pReligion = pReligions->GetReligion(eFoundedReligion, NO_PLAYER);
		if(pReligion)
		{
			iReligionCulturePerTurn += pReligion->m_Beliefs.GetHolyCityYieldChange(YIELD_CULTURE);

			int iTemp = pReligion->m_Beliefs.GetYieldChangePerForeignCity(YIELD_CULTURE);
			if (iTemp > 0)
			{
				iReligionCulturePerTurn += (iTemp * GetReligions()->GetNumForeignCitiesFollowing());
			}

			iTemp = pReligion->m_Beliefs.GetYieldChangePerXForeignFollowers(YIELD_CULTURE);
			if (iTemp > 0)
			{
#ifdef LEK_CULTURE_SCIENCE_SPREAD_BELIEFS_ALL_CITIES
				int iFollowers = pReligions->GetNumFollowers(eFoundedReligion);
#else
				int iFollowers = GetReligions()->GetNumForeignFollowers(false             );
#endif
				if (iFollowers > 0)
				{
					iReligionCulturePerTurn += (iFollowers / iTemp);
				}
			}

			bool bAtPeace = GET_TEAM(getTeam()).getAtWarCount(false) == 0;
			int iMod = pReligion->m_Beliefs.GetPlayerCultureModifier(bAtPeace);
#ifdef AUI_PLAYER_FIX_JONS_CULTURE_IS_T100
			iReligionCulturePerTurn *= 100;
#endif

			if (iMod != 0)
			{
				iReligionCulturePerTurn += ((iReligionCulturePerTurn + iOtherCulturePerTurn) * iMod) / 100;
			}
			return iReligionCulturePerTurn;
		}
	}

	return 0;
#else
	return getYieldFromReligionTimes100(YIELD_CULTURE, getYieldTimes100(YIELD_CULTURE, true));
#endif
}



int CvPlayer::GetCulturePerTurnFromBonusTurns() const
#ifdef AUI_PLAYER_FIX_JONS_CULTURE_IS_T100
{
#if !defined(STANDARDIZE_YIELDS)
	return GetCulturePerTurnFromBonusTurnsTimes100() / 100;
#else
	return 0;
#endif
}

int CvPlayer::GetCulturePerTurnFromBonusTurnsTimes100() const
#endif
{
#if !defined(STANDARDIZE_YIELDS)
	int iValue = 0;

	if (GetCultureBonusTurns() > 0)
	{

#ifdef AUI_PLAYER_FIX_JONS_CULTURE_IS_T100
		iValue += GetJONSCulturePerTurnFromCitiesTimes100();
		iValue += GetJONSCulturePerTurnFromExcessHappinessTimes100();
		iValue += GetJONSCulturePerTurnForFree() * 100;
		iValue += GetCulturePerTurnFromMinorCivs() * 100;
		iValue += GetCulturePerTurnFromReligionTimes100();

		iValue *= GC.getTEMPORARY_CULTURE_BOOST_MOD();
		iValue /= 100;
#else
		int iOtherCulturePerTurn = 0;
		iOtherCulturePerTurn += GetJONSCulturePerTurnFromCities();
		iOtherCulturePerTurn += GetJONSCulturePerTurnFromExcessHappiness();
		iOtherCulturePerTurn += GetJONSCulturePerTurnForFree();
		iOtherCulturePerTurn += GetCulturePerTurnFromMinorCivs();
		iOtherCulturePerTurn += GetCulturePerTurnFromReligion();

		iValue += ((iOtherCulturePerTurn * GC.getTEMPORARY_CULTURE_BOOST_MOD()) / 100);
#endif
	}

	return iValue;
#else
	return 0;
#endif
}



int CvPlayer::GetJONSCultureCityModifier() const
{
#if !defined(STANDARDIZE_YIELDS)
	return m_iJONSCultureCityModifier;
#else
	return getYieldRateModifier(YIELD_CULTURE);
#endif
}



void CvPlayer::ChangeJONSCultureCityModifier(int iChange)
{
#if !defined(STANDARDIZE_YIELDS)
	if(iChange != 0)
	{
		m_iJONSCultureCityModifier += iChange;

		if(GC.getGame().getActivePlayer() == GetID())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
#else
	changeYieldRateModifier(YIELD_CULTURE, iChange);
#endif
}

#ifdef AUI_PLAYER_FIX_JONS_CULTURE_IS_T100
int CvPlayer::getJONSCultureTimes100() const
{

	if (isMinorCiv())
		return 0;

	if (GC.getGame().isOption(GAMEOPTION_NO_POLICIES))
	{
		return 0;
	}

	return m_iJONSCultureT100;
}


void CvPlayer::setJONSCultureTimes100(int iNewValue)
{
	if (getJONSCultureTimes100() != iNewValue)
	{

		if (iNewValue > getJONSCultureTimes100())
		{
			ChangeJONSCultureEverGeneratedTimes100(iNewValue - getJONSCultureTimes100());
		}

		m_iJONSCultureT100 = iNewValue;

		if (GC.getGame().getActivePlayer() == GetID())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

void CvPlayer::changeJONSCultureTimes100(int iChange)
{
	setJONSCultureTimes100(getJONSCultureTimes100() + iChange);
}

int CvPlayer::GetJONSCultureEverGeneratedTimes100() const
{
	return m_iJONSCultureEverGeneratedT100;
}


void CvPlayer::SetJONSCultureEverGeneratedTimes100(int iNewValue)
{
	if (GetJONSCultureEverGeneratedTimes100() != iNewValue)
	{
		m_iJONSCultureEverGeneratedT100 = iNewValue;
	}
}

void CvPlayer::ChangeJONSCultureEverGeneratedTimes100(int iChange)
{
	SetJONSCultureEverGeneratedTimes100(GetJONSCultureEverGeneratedTimes100() + iChange);
}

int CvPlayer::getJONSCulture() const
{
	return getJONSCultureTimes100() / 100;
}
void CvPlayer::setJONSCulture(int iNewValue)
{
	setJONSCultureTimes100(iNewValue * 100);
}
void CvPlayer::changeJONSCulture(int iChange)
{
	changeJONSCultureTimes100(iChange * 100);
}

int CvPlayer::GetJONSCultureEverGenerated() const
{
	return GetJONSCultureEverGeneratedTimes100() / 100;
}
void CvPlayer::SetJONSCultureEverGenerated(int iNewValue)
{
	SetJONSCultureEverGeneratedTimes100(iNewValue * 100);
}
void CvPlayer::ChangeJONSCultureEverGenerated(int iChange)
{
	ChangeJONSCultureEverGeneratedTimes100(iChange * 100);
}
#else

int CvPlayer::getJONSCulture() const
{

	if(isMinorCiv())
		return 0;

	if(GC.getGame().isOption(GAMEOPTION_NO_POLICIES))
	{
		return 0;
	}

	return m_iJONSCulture;
}



void CvPlayer::setJONSCulture(int iNewValue)
{
	if(getJONSCulture() != iNewValue)
	{

		if(iNewValue > m_iJONSCulture)
		{
			ChangeJONSCultureEverGenerated(iNewValue - m_iJONSCulture);
		}

		m_iJONSCulture = iNewValue;

		if(GC.getGame().getActivePlayer() == GetID())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


void CvPlayer::changeJONSCulture(int iChange)
{
	setJONSCulture(getJONSCulture() + iChange);
}



int CvPlayer::GetJONSCultureEverGenerated() const
{
	return m_iJONSCultureEverGenerated;
}



void CvPlayer::SetJONSCultureEverGenerated(int iNewValue)
{
	if(GetJONSCultureEverGenerated() != iNewValue)
	{
		m_iJONSCultureEverGenerated = iNewValue;
	}
}


void CvPlayer::ChangeJONSCultureEverGenerated(int iChange)
{
	SetJONSCultureEverGenerated(GetJONSCultureEverGenerated() + iChange);
}
#endif



int CvPlayer::GetJONSCulturePerCityPerTurn() const
{
#ifdef AUI_PLAYER_FIX_JONS_CULTURE_IS_T100
	int iCulture = GetJONSCultureEverGeneratedTimes100();
#else
	int iCulture = GetJONSCultureEverGenerated();
#endif
	int iNumCities = getNumCities();


	iNumCities -= GetNumPuppetCities();

	int iNumTurns = GC.getGame().getElapsedGameTurns();

	if(iNumTurns == 0)
	{
		iNumTurns = 1;
	}

#ifdef AUI_PLAYER_FIX_JONS_CULTURE_IS_T100
	int iCulturePerCityPerTurn = iCulture / iNumCities / iNumTurns;
#else
	int iCulturePerCityPerTurn = 100 * iCulture / iNumCities / iNumTurns;
#endif
	return iCulturePerCityPerTurn;
}



int CvPlayer::GetCulturePerWonder() const
{
#if !defined(LEKMOD_EXPERIMENTAL_CHANGES)
	return m_iCulturePerWonder;
#else
	return GetWorldWonderYieldChanges(YIELD_CULTURE);
#endif
}



void CvPlayer::ChangeCulturePerWonder(int iChange)
{
#if !defined(LEKMOD_EXPERIMENTAL_CHANGES)
	if(iChange != 0)
	{
		m_iCulturePerWonder += iChange;

		int iTotalCultureChange;


		CvCity* pLoopCity;
		int iLoop;
		for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			iTotalCultureChange = pLoopCity->getNumWorldWonders() * iChange;
#if !defined(STANDARDIZE_YIELDS)
			pLoopCity->ChangeJONSCulturePerTurnFromPolicies(iTotalCultureChange);
#else
			pLoopCity->ChangeBaseYieldRateFromPolicies(YIELD_CULTURE, iTotalCultureChange);
#endif
		}
	}
#else
	ChangeWorldWonderYieldChanges(YIELD_CULTURE, iChange);
#endif
}



int CvPlayer::GetCultureWonderMultiplier() const
{
	return m_iCultureWonderMultiplier;
}



void CvPlayer::ChangeCultureWonderMultiplier(int iChange)
{
	if(iChange != 0)
		m_iCultureWonderMultiplier += iChange;
}



int CvPlayer::GetCulturePerTechResearched() const
{
	return m_iCulturePerTechResearched;
}



void CvPlayer::ChangeCulturePerTechResearched(int iChange)
{
	if(iChange != 0)
	{
		m_iCulturePerTechResearched += iChange;
	}
}



int CvPlayer::GetSpecialistCultureChange() const
{
	return m_iSpecialistCultureChange;
}



void CvPlayer::ChangeSpecialistCultureChange(int iChange)
{
	if(iChange != 0)
	{
		CvCity* pLoopCity;
		int iLoop;

		int iTotalCulture = 0;

		SpecialistTypes eSpecialist;
#ifdef AUI_WARNING_FIXES
		uint iSpecialistLoop;
#else
		int iSpecialistLoop;
#endif
		int iSpecialistCount;


		for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			for(iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
			{
				eSpecialist = (SpecialistTypes) iSpecialistLoop;
				iSpecialistCount = pLoopCity->GetCityCitizens()->GetSpecialistCount(eSpecialist);
				iTotalCulture += (iSpecialistCount * pLoopCity->GetCultureFromSpecialist(eSpecialist));
			}
#if !defined(STANDARDIZE_YIELDS)
			pLoopCity->ChangeJONSCulturePerTurnFromSpecialists(-iTotalCulture);
#else
			pLoopCity->ChangeBaseYieldRateFromSpecialists(YIELD_CULTURE, -iTotalCulture);
#endif
		}


		m_iSpecialistCultureChange += iChange;

		iTotalCulture = 0;


		for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			for(iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
			{
				eSpecialist = (SpecialistTypes) iSpecialistLoop;
				iSpecialistCount = pLoopCity->GetCityCitizens()->GetSpecialistCount(eSpecialist);
				iTotalCulture += (iSpecialistCount * pLoopCity->GetCultureFromSpecialist(eSpecialist));
			}
#if !defined(STANDARDIZE_YIELDS)
			pLoopCity->ChangeJONSCulturePerTurnFromSpecialists(iTotalCulture);
#else
			pLoopCity->ChangeBaseYieldRateFromSpecialists(YIELD_CULTURE, iTotalCulture);
#endif
		}
#ifdef AUI_CITIZENS_MID_TURN_ASSIGN_RUNS_SELF_CONSISTENCY
		doSelfConsistencyCheckAllCities();
#endif
	}
}




int CvPlayer::GetCultureYieldFromPreviousTurns(int iGameTurn, int iNumPreviousTurnsToCount)
{

	int iSum = 0;
	for (int iI = 0; iI < iNumPreviousTurnsToCount; iI++)
	{
		int iTurn = iGameTurn - iI;
		if (iTurn < 0)
		{
			break;
		}

		int iTurnCulture = getReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_CULTUREPERTURN"), iTurn);
		if (iTurnCulture >= 0)
		{
			iSum += iTurnCulture;
		}
		else if (iTurnCulture == -1)
		{
			iSum += (3 * GetTotalJONSCulturePerTurn());
		}
	}

	return iSum;
}
#if defined(STANDARDIZE_YIELDS)
int CvPlayer::GetTotalYieldEverGeneratedTimes100(YieldTypes eYield) const
{
	return m_aiYieldEverGeneratedTimes100[eYield];
}
void CvPlayer::ChangeTotalYieldEverGeneratedTimes100(YieldTypes eYield, int iChange)
{
	if (iChange != 0)
	{
		SetTotalYieldEverGeneratedTimes100(eYield, GetTotalYieldEverGeneratedTimes100(eYield) + iChange);
	}
}
void CvPlayer::SetTotalYieldEverGeneratedTimes100(YieldTypes eYield, int iChange)
{
	if (iChange != 0)
	{
		m_aiYieldEverGeneratedTimes100.setAt(eYield, iChange);
	}
}


int CvPlayer::GetYieldPerTurnFromMisc(YieldTypes eYield) const
{
	CvAssertMsg(eYield >= 0 && eYield < NUM_YIELD_TYPES, "eYield out of bounds");
	return m_aiYieldPerTurnFromMisc[eYield];
}

void CvPlayer::ChangeYieldPerTurnFromMisc(YieldTypes eYield, int iChange)
{
	CvAssertMsg(eYield >= 0 && eYield < NUM_YIELD_TYPES, "eYield out of bounds");
	if (iChange != 0)
	{
		m_aiYieldPerTurnFromMisc.setAt(eYield, m_aiYieldPerTurnFromMisc[eYield] + iChange);

		if (GC.getGame().getActivePlayer() == GetID())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}
#endif

int CvPlayer::GetNumCitiesFreePietyGardens() const
{
	return m_iNumCitiesFreePietyGardens;
}





int CvPlayer::GetNumCitiesFreeAestheticsSchools() const
{
	return m_iNumCitiesFreeAestheticsSchools;
}






int CvPlayer::GetNumCitiesFreeWalls() const
{
	return m_iNumCitiesFreeWalls;
}



void CvPlayer::ChangeNumCitiesFreeAestheticsSchools(int iChange)
{
	if(iChange != 0)
		m_iNumCitiesFreeAestheticsSchools += iChange;
}



void CvPlayer::ChangeNumCitiesFreePietyGardens(int iChange)
{
	if(iChange != 0)
		m_iNumCitiesFreePietyGardens += iChange;
}



void CvPlayer::ChangeNumCitiesFreeWalls(int iChange)
{
	if(iChange != 0)
		m_iNumCitiesFreeWalls += iChange;
}


int CvPlayer::GetNumCitiesFreeCultureBuilding() const
{
	return m_iNumCitiesFreeCultureBuilding;
}


void CvPlayer::ChangeNumCitiesFreeCultureBuilding(int iChange)
{
	if(iChange != 0)
		m_iNumCitiesFreeCultureBuilding += iChange;
}


int CvPlayer::GetNumCitiesFreeFoodBuilding() const
{
	return m_iNumCitiesFreeFoodBuilding;
}


void CvPlayer::ChangeNumCitiesFreeFoodBuilding(int iChange)
{
	if(iChange != 0)
		m_iNumCitiesFreeFoodBuilding += iChange;
}

int CvPlayer::GetNumCitiesFreeChosenBuilding(BuildingClassTypes eBuildingClass) const
{
	CvAssertMsg(eBuildingClass < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(eBuildingClass > -1, "Index out of bounds");
	return m_paiNumCitiesFreeChosenBuilding[eBuildingClass];
}


void CvPlayer::ChangeNumCitiesFreeChosenBuilding(BuildingClassTypes eBuildingClass, int iChange)
{
	m_paiNumCitiesFreeChosenBuilding.setAt(eBuildingClass, (m_paiNumCitiesFreeChosenBuilding[eBuildingClass] + iChange));
}

bool CvPlayer::IsFreeUnitNewFoundCity(UnitClassTypes eUnitClass) const
{
	CvAssertMsg(eUnitClass < GC.getNumUnitClassInfos(), "Index out of bounds");
	CvAssertMsg(eUnitClass > -1, "Index out of bounds");
	return m_pabNewFoundCityFreeUnit[eUnitClass];
}


void CvPlayer::ChangeNewFoundCityFreeUnit(UnitClassTypes eUnitClass, bool bValue)
{
	m_pabNewFoundCityFreeUnit.setAt(eUnitClass, bValue);
}

bool CvPlayer::IsFreeBuildingNewFoundCity(BuildingClassTypes eBuildingClass) const
{
	CvAssertMsg(eBuildingClass < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(eBuildingClass > -1, "Index out of bounds");
	return m_pabNewFoundCityFreeBuilding[eBuildingClass];
}


void CvPlayer::ChangeNewFoundCityFreeBuilding(BuildingClassTypes eBuildingClass, bool bValue)
{
	m_pabNewFoundCityFreeBuilding.setAt(eBuildingClass, bValue);
}

bool CvPlayer::IsFreeChosenBuildingNewCity(BuildingClassTypes eBuildingClass) const
{
	CvAssertMsg(eBuildingClass < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(eBuildingClass > -1, "Index out of bounds");
	return m_pabFreeChosenBuildingNewCity[eBuildingClass];
}


void CvPlayer::ChangeFreeChosenBuildingNewCity(BuildingClassTypes eBuildingClass, bool bValue)
{
	m_pabFreeChosenBuildingNewCity.setAt(eBuildingClass, bValue);
}



bool CvPlayer::IsFreeBuildingAllCity(BuildingClassTypes eBuildingClass) const
{
	CvAssertMsg(eBuildingClass < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(eBuildingClass > -1, "Index out of bounds");
	return m_pabAllCityFreeBuilding[eBuildingClass];
}


void CvPlayer::ChangeAllCityFreeBuilding(BuildingClassTypes eBuildingClass, bool bValue)
{
	m_pabAllCityFreeBuilding.setAt(eBuildingClass, bValue);
}
#if !defined(FULL_YIELD_FROM_KILLS)


void CvPlayer::DoYieldsFromKill(UnitTypes eAttackingUnitType, UnitTypes eKilledUnitType, int iX, int iY, bool bWasBarbarian, int iExistingDelay)
{
	int iNumBonuses = iExistingDelay;
	DoUnresearchedTechBonusFromKill(eKilledUnitType, iX, iY, iNumBonuses);
	for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		DoYieldBonusFromKill((YieldTypes)iYield, eAttackingUnitType, eKilledUnitType, iX, iY, bWasBarbarian, iNumBonuses);
	}
}




void CvPlayer::DoYieldBonusFromKill(YieldTypes eYield, UnitTypes eAttackingUnitType, UnitTypes eKilledUnitType, int iX, int iY, bool bWasBarbarian, int &iNumBonuses)
{
#ifdef UPDATE_CULTURE_NOTIFICATION_DURING_TURN
	CvGame& kGame = GC.getGame();
#endif
	CvAssertMsg(eKilledUnitType != NO_UNIT, "Killed unit's type is NO_TYPE. Please send Anton your save file and version.");
	if (eKilledUnitType == NO_UNIT) return;

	CvUnitEntry* pkKilledUnitInfo = GC.getUnitInfo(eKilledUnitType);
	if(pkKilledUnitInfo)
	{
		int iCombatStrength = max(pkKilledUnitInfo->GetCombat(), pkKilledUnitInfo->GetRangedCombat());
		if(iCombatStrength > 0)
		{	
			int iPolicyValue = 0;
			int iTraitValue = 0;
			int iBeliefValue = 0;
			int iOtherValue = 0;

			switch(eYield)
			{
			case YIELD_FOOD:
			case YIELD_PRODUCTION:

				return;

			case YIELD_GOLD:
				iPolicyValue += GetPlayerPolicies()->GetNumericModifier(POLICYMOD_GOLD_FROM_KILLS);
				break;

			case YIELD_CULTURE:
				iTraitValue += GetPlayerTraits()->GetCultureFromKills();
				iPolicyValue += GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURE_FROM_KILLS);


				if(bWasBarbarian)
				{
					iPolicyValue += GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURE_FROM_BARBARIAN_KILLS);
				}
				break;

			case YIELD_FAITH:
				iTraitValue += GetPlayerTraits()->GetFaithFromKills();

				if (eYield == YIELD_FAITH && (GC.getGame().isOption(GAMEOPTION_NO_RELIGION)))
				{
					return;
				}
				break;
			case YIELD_SCIENCE:
				iPolicyValue += GetPlayerPolicies()->GetNumericModifier(POLICYMOD_SCIENCE_FROM_KILLS);
				break;
			}

			iBeliefValue += GC.getGame().GetGameReligions()->GetBeliefYieldForKill(eYield, iX, iY, GetID());

			if(eAttackingUnitType != NO_UNIT)
			{
				CvUnitEntry* pkAttackingUnitInfo = GC.getUnitInfo(eAttackingUnitType);
				if(pkAttackingUnitInfo)
				{
					iOtherValue += pkAttackingUnitInfo->GetYieldFromKills(eYield);
				}
			}
			

			iPolicyValue = min((iPolicyValue * iCombatStrength) / 100, 30); 
			iTraitValue = (iTraitValue * iCombatStrength) / 100;
			iBeliefValue = min((iBeliefValue * iCombatStrength) / 100, 30);
			iOtherValue = (iOtherValue * iCombatStrength) / 100;
			int iTotalValue = iPolicyValue + iTraitValue + iBeliefValue + iOtherValue;

			if(iTotalValue > 0)
			{
				switch(eYield)
				{
				case YIELD_GOLD:
					GetTreasury()->ChangeGold(iTotalValue);
					break;
				case YIELD_CULTURE:
					changeJONSCulture(iTotalValue);

#ifdef UPDATE_CULTURE_NOTIFICATION_DURING_TURN

					if (isAlive() && isHuman() && getNumCities() > 0)
					{
						if (!GC.GetEngineUserInterface()->IsPolicyNotificationSeen())
						{
							if (getNextPolicyCost() <= getJONSCulture() && GetPlayerPolicies()->GetNumPoliciesCanBeAdopted() > 0)
							{
								CvNotifications* pNotifications = GetNotifications();
								if (pNotifications)
								{
									CvString strBuffer;

									if (kGame.isOption(GAMEOPTION_POLICY_SAVING))
										strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ENOUGH_CULTURE_FOR_POLICY_DISMISS");
									else
										strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ENOUGH_CULTURE_FOR_POLICY");

									CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ENOUGH_CULTURE_FOR_POLICY");
									pNotifications->Add(NOTIFICATION_POLICY, strBuffer, strSummary, -1, -1, -1);
								}
							}
						}
					}
#endif
					break;
				case YIELD_FAITH:
					ChangeFaith(iTotalValue);
					break;
				case YIELD_SCIENCE:
					TechTypes eCurrentTech = GetPlayerTechs()->GetCurrentResearch();
					if(eCurrentTech == NO_TECH)
					{
						changeOverflowResearch(iTotalValue);
					}
					else
					{
						GET_TEAM(getTeam()).GetTeamTechs()->ChangeResearchProgress(eCurrentTech, iTotalValue, GetID());
					}
					break;
				}
				iNumBonuses++;
				ReportYieldFromKill(eYield, iTotalValue, iX, iY, iNumBonuses);
			}
		}
	}
}
#else

void CvPlayer::DoYieldsFromKill(CvUnit* pAttackingUnit, CvUnit* pKilledUnit, int iX, int iY, bool bWasBarbarian, int iExistingDelay)
{
	int iNumBonuses = iExistingDelay;
	CvAssertMsg(pKilledUnit != NULL, "Killed Unit is NULL, Please Report.");
	if (pKilledUnit != NULL)
	{
		UnitTypes eKilledUnitType = pKilledUnit->getUnitType();
		DoUnresearchedTechBonusFromKill(eKilledUnitType, iX, iY, iNumBonuses);

		for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
		{
			DoYieldBonusFromKill((YieldTypes)iYield, pAttackingUnit, pKilledUnit, iX, iY, bWasBarbarian, iNumBonuses);
		}
	}
}
void CvPlayer::DoYieldBonusFromKill(YieldTypes eYield, CvUnit* pAttackingUnit, CvUnit* pKilledUnit, int iX, int iY, bool bWasBarbarian, int& iNumBonuses)
{
	CvGame& kGame = GC.getGame();

	CvAssertMsg(pKilledUnit != NULL, "Killed Unit is NULL, Please Report.");
	if (pKilledUnit == NULL) return;
	UnitTypes eKilledUnitType = pKilledUnit->getUnitType();
	CvAssertMsg(eKilledUnitType != NO_UNIT, "Killed unit's type is NO_TYPE. Please Report.");
	if (eKilledUnitType == NO_UNIT) return;
	CvUnitEntry* pkKilledUnitInfo = GC.getUnitInfo(eKilledUnitType);

	UnitTypes eAttackingUnitType = NO_UNIT;
#if !defined(UNITS_REMEMBER_HOME)
	if (pAttackingUnit != NULL)
		eAttackingUnitType = pAttackingUnit->getUnitType();
#else

	CvCity* pAttackerHomeCity = NULL;
	if (pAttackingUnit != NULL)
	{
		eAttackingUnitType = pAttackingUnit->getUnitType();
		pAttackerHomeCity = pAttackingUnit->GetHomeCity();


		if (pAttackerHomeCity != NULL)
		{
			if (pAttackingUnit->getOwner() != pAttackerHomeCity->getOwner())
				pAttackerHomeCity = NULL;
		}
	}
#endif
	if (pkKilledUnitInfo)
	{
		int iCombatStrength = std::max(pKilledUnit->GetBaseCombatStrength(true), pKilledUnit->GetBaseRangedCombatStrength());
		if (iCombatStrength > 0)
		{
			int iPolicyValue = 0;
			int iTraitValue = 0;
			int iBeliefValue = 0;
			int iPromotionValue = 0;
			int iUnitValue = 0;

			switch (eYield)
			{ 
			case YIELD_GOLD:
				iPolicyValue += GetPlayerPolicies()->GetNumericModifier(POLICYMOD_GOLD_FROM_KILLS);
				break;
			case YIELD_SCIENCE:
				iPolicyValue += GetPlayerPolicies()->GetNumericModifier(POLICYMOD_SCIENCE_FROM_KILLS);
				break;
			case YIELD_CULTURE:
				iPolicyValue += GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURE_FROM_KILLS);
				iTraitValue += GetPlayerTraits()->GetCultureFromKills();

				if (bWasBarbarian)
					iPolicyValue += GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURE_FROM_BARBARIAN_KILLS);
				break;
			case YIELD_FAITH:
				iTraitValue += GetPlayerTraits()->GetFaithFromKills();
				if (kGame.isOption(GAMEOPTION_NO_RELIGION))
					return;
				break;
#if defined(LEKMOD_v34)
			case YIELD_GOLDEN_AGE_POINTS:
				if (pAttackingUnit != NULL)
					iPromotionValue += pAttackingUnit->GetGoldenAgeValueFromKills();

				if (kGame.isOption(GAMEOPTION_NO_HAPPINESS))
					return;
				break;
#endif
			}

			iPolicyValue += GetPlayerPolicies()->GetYieldFromKills(eYield);
			iTraitValue += GetPlayerTraits()->GetYieldFromKills(eYield);
			iBeliefValue += kGame.GetGameReligions()->GetBeliefYieldForKill(eYield, iX, iY, GetID());

			if (eAttackingUnitType != NO_UNIT)
			{
				CvUnitEntry* pkAttackingUnitInfo = GC.getUnitInfo(eAttackingUnitType);
				if (pkAttackingUnitInfo)
					iUnitValue += pkAttackingUnitInfo->GetYieldFromKills(eYield);
			}
			if (pAttackingUnit != NULL)
			{
			    EraTypes eKilledUnitEra = (EraTypes)pkKilledUnitInfo->GetEra();
				bool bValid = pAttackingUnit->IsKillYieldEraValid(eKilledUnitEra);
				if(bValid)
				{
					iPromotionValue += pAttackingUnit->GetYieldFromKills(eYield);
				}
			}



			int iPolicyCap = GC.getPOLICY_YIELD_CAP();
			int iPolicyMax = GetPlayerPolicies()->GetYieldFromKillsMax(eYield);
			if (iPolicyMax > 0)
				iPolicyCap = iPolicyMax;

			int iTraitCap = GC.getTRAIT_YIELD_CAP();
			int iTraitMax = GetPlayerTraits()->GetYieldFromKillsMax(eYield);
			if (iTraitMax > 0)
				iTraitCap = iTraitMax;

			int iBeliefCap = GC.getBELIEF_YIELD_CAP();

			int iPromotionCap = GC.getPROMOTION_YIELD_CAP();
			if (pAttackingUnit != NULL)
			{
				int iCapFromPromotions = pAttackingUnit->GetKillYieldCap(eYield);
				if (iCapFromPromotions > 0)
					iPromotionCap = iCapFromPromotions;
			}

			int iUnitCap = GC.getUNIT_YIELD_CAP();
			if (eAttackingUnitType != NO_UNIT)
			{
				CvUnitEntry* pkAttackingUnitInfo = GC.getUnitInfo(eAttackingUnitType);
				if (pkAttackingUnitInfo)
				{
					int iUnitMax = pkAttackingUnitInfo->GetYieldFromKillsMax(eYield);
					if (iUnitMax > 0)
						iUnitCap = iUnitMax;
				}
			}

			if (iPolicyCap > 0)
				iPolicyValue = min((iPolicyValue * iCombatStrength) / 100, iPolicyCap);
			else 
				iPolicyValue = (iPolicyValue * iCombatStrength) / 100;
			if (iTraitCap > 0)
				iTraitValue = min((iTraitValue * iCombatStrength) / 100, iTraitCap);
			else
				iTraitValue = (iTraitValue * iCombatStrength) / 100;
			if (iBeliefCap > 0)
				iBeliefValue = min((iBeliefValue * iCombatStrength) / 100, iBeliefCap);
			else
				iBeliefValue = (iBeliefValue * iCombatStrength) / 100;
			if (iPromotionCap > 0)
				iPromotionValue = min((iPromotionValue * iCombatStrength) / 100, iPromotionCap);
			else
				iPromotionValue = (iPromotionValue * iCombatStrength) / 100;
			if (iUnitCap > 0)
				iUnitValue = min((iUnitValue * iCombatStrength) / 100, iUnitCap);
			else
				iUnitValue = (iUnitValue * iCombatStrength) / 100;
			
			int iTotalValue = iPolicyValue + iTraitValue + iBeliefValue + iPromotionValue + iUnitValue;

			if (iTotalValue > 0)
			{
				switch (eYield)
				{

#if defined(UNITS_REMEMBER_HOME)
				case YIELD_FOOD:
				{
					if (pAttackerHomeCity != NULL)
					{

						iX = pAttackerHomeCity->getX();
						iY = pAttackerHomeCity->getY();

						pAttackerHomeCity->changeFood(iTotalValue);
						TestMidTurnPopGrowth(pAttackerHomeCity, true                           );
					}
					else
					{

						return;
					}
				}
				break;
				case YIELD_PRODUCTION:
				{
					if (pAttackerHomeCity != NULL)
					{

						iX = pAttackerHomeCity->getX();
						iY = pAttackerHomeCity->getY();

						if (pAttackerHomeCity->isProductionProcess())
							pAttackerHomeCity->changeProduction(iTotalValue);
						else
							pAttackerHomeCity->setOverflowProduction(pAttackerHomeCity->getOverflowProduction() + iTotalValue);
					}
					else
					{

						return;
					}
				}
				break;
#endif
				case YIELD_GOLD:
					GetTreasury()->ChangeGold(iTotalValue);
					break;
				case YIELD_CULTURE:
					changeJONSCulture(iTotalValue);
#if defined(UPDATE_CULTURE_NOTIFICATION_DURING_TURN)
					if (isAlive() && isHuman() && getNumCities() > 0)
						TestMidTurnPolicyNotification();
#endif
					break;
				case YIELD_FAITH:
					ChangeFaith(iTotalValue);
					break;
				case YIELD_SCIENCE:
				{
					TechTypes eCurrentTech = GetPlayerTechs()->GetCurrentResearch();
					if (eCurrentTech == NO_TECH)
						changeOverflowResearch(iTotalValue);
					else
						GET_TEAM(getTeam()).GetTeamTechs()->ChangeResearchProgress(eCurrentTech, iTotalValue, GetID());
				}
				break;
#if defined(LEKMOD_v34)
				case YIELD_GOLDEN_AGE_POINTS:
					ChangeGoldenAgeProgressMeter(iTotalValue);

					break;
#endif
#if defined(LEK_YIELD_TOURISM)
				case YIELD_TOURISM:
					GetCulture()->ChangeInfluenceOn(pKilledUnit->getOwner(), iTotalValue);
					break;
#endif
				}

				iNumBonuses++;
				ReportYieldFromKill(eYield, iTotalValue, iX, iY, iNumBonuses);
			}
		}
	}
}
#endif
#if defined(LEKMOD_PROMO_YIELD_FROM_CONVERSION) && defined(LEKMOD_PROMO_CONVERSION_MAJORITY_ONLY_ONCE)
bool CvPlayer::IsConversionMajorityYieldOnceUsed(int iCityID, PromotionTypes ePromotion, YieldTypes eYield) const
{
	const std::vector<int>& v = m_aiConversionMajorityOnceUsedKeys;
	for (size_t i = 0; i + 2 < v.size(); i += 3)
	{
		if (v[i] == iCityID && v[i + 1] == (int)ePromotion && v[i + 2] == (int)eYield)
			return true;
	}
	return false;
}


void CvPlayer::MarkConversionMajorityYieldOnceUsed(int iCityID, PromotionTypes ePromotion, YieldTypes eYield)
{
	if (IsConversionMajorityYieldOnceUsed(iCityID, ePromotion, eYield))
		return;
	std::vector<int>& v = m_aiConversionMajorityOnceUsedKeys.dirtyGet();
	v.push_back(iCityID);
	v.push_back((int)ePromotion);
	v.push_back((int)eYield);
}
#endif
#if defined(LEKMOD_PROMO_YIELD_FROM_CONVERSION)
void CvPlayer::DoYieldsFromConversion(CvUnit* pConvertingUnit, CvCity* pPressuredCity,int iFollowerDelta, bool bMajority, int iX, int iY, int iExistingDelay)
{
	int iNumBonuses = iExistingDelay;
	if (pPressuredCity != NULL)
	{
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
		{
			DoYieldBonusFromConversion((YieldTypes)iYield, pConvertingUnit, pPressuredCity, iFollowerDelta, bMajority, iX, iY, iNumBonuses);
		}
	}
}
void CvPlayer::DoYieldBonusFromConversion(YieldTypes eYield, CvUnit* pConvertingUnit, CvCity* pPressuredCity, int iFollowerDelta, bool bMajority, int iX, int iY, int& iNumBonuses)
{
	CvGame& kGame = GC.getGame();
	CvAssertMsg(pConvertingUnit != NULL, "Converting Unit is NULL, Please Report.");
	if (pConvertingUnit == NULL) return;
	CvAssertMsg(pPressuredCity != NULL, "Pressured City is NULL, Please Report.");
	if (pPressuredCity == NULL) return;
	if (eYield == YIELD_FAITH && (kGame.isOption(GAMEOPTION_NO_RELIGION)))
		return;
#if defined(LEKMOD_v34)
	if (eYield == YIELD_GOLDEN_AGE_POINTS && kGame.isOption(GAMEOPTION_NO_HAPPINESS))
		return;
#endif
	if (iFollowerDelta == 0)
		return;
	int iPerFollower = 0;
	int iPerFollowerMajority = 0;
#if defined(LEKMOD_PROMO_CONVERSION_MAJORITY_ONLY_ONCE)
	std::vector<PromotionTypes> aePromotionsToMarkOnce;
	const int iCityID = pPressuredCity->GetID();
#endif

	for (int iP = 0; iP < GC.getNumPromotionInfos(); iP++)
	{
		if (pConvertingUnit->isHasPromotion((PromotionTypes)iP))
		{
			const CvPromotionEntry* pEntry = GC.getPromotionInfo((PromotionTypes)iP);
			if (pEntry)
			{
				iPerFollower += pEntry->GetYieldFromFollowerConversion(eYield);
#if defined(LEKMOD_PROMO_CONVERSION_MAJORITY_ONLY_ONCE)
				if (bMajority)
				{
					const int iMajAmt = pEntry->GetYieldFromFollowerConversionMajority(eYield);
					if (iMajAmt > 0)
					{
						if (pEntry->GetYieldFromFollowerConversionMajorityOnlyOnce(eYield))
						{
							if (IsConversionMajorityYieldOnceUsed(iCityID, (PromotionTypes)iP, eYield))
								continue;
							aePromotionsToMarkOnce.push_back((PromotionTypes)iP);
						}
						iPerFollowerMajority += iMajAmt;
					}
				}
#else
				iPerFollowerMajority += pEntry->GetYieldFromFollowerConversionMajority(eYield);
#endif
			}
		}
	}
	int iTotalValue = (iFollowerDelta * iPerFollower) + (bMajority ? iPerFollowerMajority : 0);
	if (iTotalValue > 0)
	{
		switch (eYield)
		{
		case YIELD_GOLD:
			GetTreasury()->ChangeGold(iTotalValue);
			break;
		case YIELD_CULTURE:
			changeJONSCulture(iTotalValue);
#if defined(UPDATE_CULTURE_NOTIFICATION_DURING_TURN)
			if (isAlive() && isHuman() && getNumCities() > 0)
				TestMidTurnPolicyNotification();
#endif
			break;
		case YIELD_FAITH:
			ChangeFaith(iTotalValue);
			break;
		case YIELD_SCIENCE:
		{
			TechTypes eCurrentTech = GetPlayerTechs()->GetCurrentResearch();
			if (eCurrentTech == NO_TECH)
				changeOverflowResearch(iTotalValue);
			else
				GET_TEAM(getTeam()).GetTeamTechs()->ChangeResearchProgress(eCurrentTech, iTotalValue, GetID());
		}
		break;
#if defined(LEKMOD_v34)
		case YIELD_GOLDEN_AGE_POINTS:
			ChangeGoldenAgeProgressMeter(iTotalValue);

			break;
#endif
#if defined(LEK_YIELD_TOURISM)
		case YIELD_TOURISM:
			GetCulture()->ChangeInfluenceOn(pPressuredCity->getOwner(), iTotalValue);
			break;
#endif
		}

		iNumBonuses++;
		ReportYieldFromKill(eYield, iTotalValue, iX, iY, iNumBonuses);
#if defined(LEKMOD_PROMO_CONVERSION_MAJORITY_ONLY_ONCE)
		if (bMajority)
		{
			for (size_t iMu = 0; iMu < aePromotionsToMarkOnce.size(); iMu++)
			{
				MarkConversionMajorityYieldOnceUsed(iCityID, aePromotionsToMarkOnce[iMu], eYield);
			}
		}
#endif
	}
}
#endif



void CvPlayer::DoUnresearchedTechBonusFromKill(UnitTypes eKilledUnitType, int iX, int iY, int &iNumBonuses)
{
	CvAssertMsg(eKilledUnitType != NO_UNIT, "Killed unit's type is NO_TYPE. Please send Anton your save file and version.");
	if (eKilledUnitType == NO_UNIT) return;

	int iPercent = GetPlayerTraits()->GetUnresearchedTechBonusFromKills();

	if (iPercent > 0)
	{
		int iValue = 0;

		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eKilledUnitType);
		if(pkUnitInfo)
		{
			TechTypes ePrereq = (TechTypes) pkUnitInfo->GetPrereqAndTech();
			if (ePrereq != NO_TECH)
			{
				CvTechEntry* pkTechInfo = GC.getTechInfo(ePrereq);
				if (pkTechInfo && !GET_TEAM(getTeam()).GetTeamTechs()->HasTech(ePrereq))
				{
					int iCombatStrength = max(pkUnitInfo->GetCombat(), pkUnitInfo->GetRangedCombat());
					if (iCombatStrength > 0)
					{
						int iTechCost = GetPlayerTechs()->GetResearchCost(ePrereq);
						iValue = (iTechCost * iPercent) / 100;


						int iRemainingCost = iTechCost - GetPlayerTechs()->GetResearchProgress(ePrereq);
						if (iValue > iRemainingCost)
						{
							iValue = iRemainingCost;
						}

						if (iValue > 0)
						{
							GET_TEAM(getTeam()).GetTeamTechs()->ChangeResearchProgress(ePrereq, iValue, GetID());
							iNumBonuses++;
							ReportYieldFromKill(YIELD_SCIENCE, iValue, iX, iY, iNumBonuses);
						}
					}
				}
			}
		}
	}
}



void CvPlayer::ReportYieldFromKill(YieldTypes eYield, int iValue, int iX, int iY, int iDelay)
{
	CvString yieldString;
	if(iValue > 0)
	{
		switch(eYield)
		{
#if defined(UNITS_REMEMBER_HOME)
		case YIELD_FOOD:
			yieldString = "[COLOR_GREEN]+%d[ENDCOLOR][ICON_FOOD]";
			break;
		case YIELD_PRODUCTION:
			yieldString = "[COLOR_ORANGE]+%d[ENDCOLOR][ICON_PRODUCTION]";
			break;
#endif
		case YIELD_GOLD:
			yieldString = "[COLOR_YELLOW]+%d[ENDCOLOR][ICON_GOLD]";
			break;
		case YIELD_CULTURE:
			yieldString = "[COLOR_MAGENTA]+%d[ENDCOLOR][ICON_CULTURE]";
			break;
		case YIELD_FAITH:
			yieldString = "[COLOR_WHITE]+%d[ENDCOLOR][ICON_PEACE]";
			break;
		case YIELD_SCIENCE:
			yieldString = "[COLOR_BLUE]+%d[ENDCOLOR][ICON_RESEARCH]";
			break;
#if defined(FULL_YIELD_FROM_KILLS) && defined(LEKMOD_v34)
		case YIELD_GOLDEN_AGE_POINTS:
			yieldString = "[COLOR_WHITE]+%d[ENDCOLOR][ICON_GOLDEN_AGE]";
			break;
#endif
#if defined(LEK_YIELD_TOURISM)
			case YIELD_TOURISM:
			yieldString = "[COLOR_CYAN]+%d[ENDCOLOR][ICON_TOURISM]";
			break;
#endif
		default:

			return;
		}

		if(GetID() == GC.getGame().getActivePlayer())
		{
			char text[256] = {0};
#if !defined(DISPLAY_GENERAL_ADMIRAL_POINTS)
			float fDelay = GC.getPOST_COMBAT_TEXT_DELAY() * (1 + ((float)iDelay * 0.5f));
#else
			float fDelay = GC.getPOST_COMBAT_TEXT_DELAY() * (1.5f + ((float)iDelay * 0.5f));
#endif
			sprintf_s(text, yieldString, iValue);
			GC.GetEngineUserInterface()->AddPopupText(iX, iY, text, fDelay);
		}
	}
}
#if defined(UPDATE_CULTURE_NOTIFICATION_DURING_TURN)
void CvPlayer::TestMidTurnPolicyNotification()
{
	if (!GC.GetEngineUserInterface()->IsPolicyNotificationSeen())
	{
		if (getNextPolicyCost() <= getJONSCulture() && GetPlayerPolicies()->GetNumPoliciesCanBeAdopted() > 0)
		{
			CvNotifications* pNotifications = GetNotifications();
			if (pNotifications)
			{
				CvString strBuffer;

				if (GC.getGame().isOption(GAMEOPTION_POLICY_SAVING))
					strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ENOUGH_CULTURE_FOR_POLICY_DISMISS");
				else
					strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ENOUGH_CULTURE_FOR_POLICY");

				CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ENOUGH_CULTURE_FOR_POLICY");
				pNotifications->Add(NOTIFICATION_POLICY, strBuffer, strSummary, -1, -1, -1);
			}
		}
	}
}
#endif
void CvPlayer::TestMidTurnPopGrowth(CvCity* pCity, bool bAlwaysShowNotification)
{
	CvAssertMsg(pCity != NULL, "pCity in CvPlayer::TestMidTurnPopGrowth is not assigned a valid value");
	if (pCity != NULL)
	{

		while ((pCity->getFood()) >= pCity->growthThreshold())
		{
			if (pCity->GetCityCitizens()->IsForcedAvoidGrowth())
			{
				pCity->setFood(pCity->growthThreshold());
			}
			else
			{
				pCity->changeFood(-(std::max(0, (pCity->growthThreshold() - pCity->getFoodKept()))));
				pCity->changePopulation(1);


				if (pCity->getPopulation() <= 5 && !bAlwaysShowNotification)
				{
					CvNotifications* pNotifications = GetNotifications();
					if (pNotifications)
					{
						Localization::String localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_GROWTH");
						localizedText << pCity->getNameKey() << pCity->getPopulation();
						Localization::String localizedSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_GROWTH");
						localizedSummary << pCity->getNameKey();
						pNotifications->Add(NOTIFICATION_CITY_GROWTH, localizedText.toUTF8(), localizedSummary.toUTF8(), pCity->getX(), pCity->getY(), pCity->GetID());
					}
				}
			}
		}
	}
}
#ifdef NQ_BELIEF_TOGGLE_ALLOW_FAITH_GIFTS_TO_MINORS
bool CvPlayer::CanFaithGiftMinors()
{
	ReligionTypes eReligion = GC.getGame().GetGameReligions()->GetReligionCreatedByPlayer(GetID());
	if (eReligion != NO_RELIGION)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);
		if (pReligion && pReligion->m_Beliefs.IsAllowsFaithGiftsToMinors())
		{
			return true;
		}
	}

	return false;
}
#endif

#ifdef NQ_ALLOW_RELIGION_ONE_SHOTS 
void CvPlayer::DoReligionOneShots(ReligionTypes eReligion)
{
	bool setUnitReligion = false;
	const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);

#ifdef NQ_FREE_SETTLERS_FROM_BELIEF
	if (!m_bHasUsedReligiousSettlements && pReligion->m_Beliefs.GetNumFreeSettlers() > 0)
	{
		m_bHasUsedReligiousSettlements = true;




		for (int iFreeSettlerLoop = 0; iFreeSettlerLoop < pReligion->m_Beliefs.GetNumFreeSettlers(); iFreeSettlerLoop++)
		{

			addFreeUnit((UnitTypes)getCivilizationInfo().getCivilizationUnits((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_PROPHET")));

			
		}
	}
#endif

#ifdef NQ_DEUS_VULT
	if (!m_bHasUsedDeusVult && pReligion->m_Beliefs.IsDeusVult())
	{
		m_bHasUsedDeusVult = true;
		setUnitReligion = true;


		UnitTypes eBestMountedUnit = (UnitTypes)getCivilizationInfo().getCivilizationUnits((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_CHARIOT_ARCHER"));
		int iBestMountedScore = GC.getUnitInfo(eBestMountedUnit)->GetProductionCost();
		

		UnitTypes eBestRangedUnit = (UnitTypes)getCivilizationInfo().getCivilizationUnits((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_ARCHER"));
		int iBestRangedScore = GC.getUnitInfo(eBestRangedUnit)->GetProductionCost();


		UnitTypes eBestSiegeUnit = (UnitTypes)getCivilizationInfo().getCivilizationUnits((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_CATAPULT"));
		int iBestSiegeScore = GC.getUnitInfo(eBestSiegeUnit)->GetProductionCost();


		UnitTypes eBestMeleeUnit = (UnitTypes)getCivilizationInfo().getCivilizationUnits((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_WARRIOR"));
		int iBestMeleeScore = GC.getUnitInfo(eBestMeleeUnit)->GetProductionCost();

		for(int iUnitClassLoop = 0; iUnitClassLoop < GC.getNumUnitClassInfos(); iUnitClassLoop++)
		{
			bool bValid = false;
			CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo((UnitClassTypes)iUnitClassLoop);
			if(pkUnitClassInfo == NULL)
				continue;

			const UnitTypes eLoopUnit = ((UnitTypes)(getCivilizationInfo().getCivilizationUnits(iUnitClassLoop)));
			if(eLoopUnit != NO_UNIT)
			{
				CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eLoopUnit);
				if(pkUnitInfo == NULL)
					continue;

				CvUnitEntry& kUnit = *pkUnitInfo;


				if (kUnit.GetCombat() <= 0 || kUnit.IsFound() || kUnit.IsFoundAbroad())
					continue;
				

				if(!(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)(kUnit.GetPrereqAndTech()))))
					continue;

				if ((UnitCombatTypes)pkUnitInfo->GetUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_MOUNTED") ||
					(UnitCombatTypes)pkUnitInfo->GetUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_MOUNTED_RANGED"))
				{
					if (pkUnitInfo->GetProductionCost() > iBestMountedScore)
					{
						iBestMountedScore = pkUnitInfo->GetProductionCost();
						eBestMountedUnit = eLoopUnit;
					}
				}
				if ((UnitCombatTypes)pkUnitInfo->GetUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_ARCHER"))
				{
					if (pkUnitInfo->GetProductionCost() > iBestRangedScore)
					{
						iBestRangedScore = pkUnitInfo->GetProductionCost();
						eBestRangedUnit = eLoopUnit;
					}
				}
				if ((UnitCombatTypes)pkUnitInfo->GetUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_SIEGE"))
				{
					if (pkUnitInfo->GetProductionCost() > iBestSiegeScore)
					{
						iBestSiegeScore = pkUnitInfo->GetProductionCost();
						eBestSiegeUnit = eLoopUnit;
					}
				}
				if ((UnitCombatTypes)pkUnitInfo->GetUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_MELEE"))
				{
					if (pkUnitInfo->GetProductionCost() > iBestMeleeScore)
					{
						iBestMeleeScore = pkUnitInfo->GetProductionCost();
						eBestMeleeUnit = eLoopUnit;
					}
				}
			}
		}


		addFreeUnit((UnitTypes)getCivilizationInfo().getCivilizationUnits((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_INQUISITOR")));
		addFreeUnit((UnitTypes)getCivilizationInfo().getCivilizationUnits((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_INQUISITOR")));


		if (eBestMountedUnit)
		{
			addFreeUnit(eBestMountedUnit);
			addFreeUnit(eBestMountedUnit);
		}


		if (eBestRangedUnit)
		{
			addFreeUnit(eBestRangedUnit);
			addFreeUnit(eBestRangedUnit);
		}


		if (eBestSiegeUnit)
		{
			addFreeUnit(eBestSiegeUnit);
			addFreeUnit(eBestSiegeUnit);
		}
	}
#endif

#ifdef NQ_GOLDEN_AGE_TURNS_FROM_BELIEF
	if (!m_bHasUsedDharma)
	{
		int iGoldenAgeTurns = pReligion->m_Beliefs.GetGoldenAgeTurns();
		if (iGoldenAgeTurns > 0)
		{
			m_bHasUsedDharma = true;
			int iLengthModifier = getGoldenAgeModifier() + GetPlayerTraits()->GetGoldenAgeDurationModifier();
			if (iLengthModifier > 0)
			{
				iGoldenAgeTurns = iGoldenAgeTurns * (100 + iLengthModifier) / 100;
			}
			iGoldenAgeTurns = iGoldenAgeTurns * GC.getGame().getGameSpeedInfo().getGoldenAgePercent() / 100;
			changeGoldenAgeTurns(iGoldenAgeTurns);
		}
	}
#endif

	if (setUnitReligion)
	{

		int iLoopUnit;
		CvUnit* pLoopUnit;
		for(pLoopUnit = firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoopUnit))
		{
			if (pLoopUnit->getUnitInfo().IsSpreadReligion() || pLoopUnit->getUnitInfo().IsRemoveHeresy())
			{
				if (pLoopUnit->GetReligionData()->GetReligion() == NO_RELIGION)
				{
					pLoopUnit->GetReligionData()->SetReligion(eReligion);
					pLoopUnit->GetReligionData()->SetSpreadsLeft(pLoopUnit->getUnitInfo().GetReligionSpreads());
					pLoopUnit->GetReligionData()->SetReligiousStrength(pLoopUnit->getUnitInfo().GetReligiousStrength());
				}
			}
		}
	}
}
#endif

#ifdef NQ_GREAT_WORK_ON_UNIQUE_CONQUEST


void CvPlayer::DoGreatWorkFromCityConquer(CvCity* pConqueredCity)
{
	CvGameCulture *pCulture = GC.getGame().GetGameCulture();
	if(pCulture == NULL)
	{
		CvAssertMsg(pCulture != NULL, "This should never happen.");
		return;
	}


	GreatWorkSlotType eGreatWorkSlot = NO_GREAT_WORK_SLOT;
	GreatWorkClass eGreatWorkClass = NO_GREAT_WORK_CLASS;
	UnitTypes eUnitType = NO_UNIT;
	if (this->GetCulture()->GetNumAvailableGreatWorkSlots(CvTypes::getGREAT_WORK_SLOT_LITERATURE()) > 0)
	{
		eGreatWorkSlot = CvTypes::getGREAT_WORK_SLOT_LITERATURE();
		eGreatWorkClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_LITERATURE");
		eUnitType = (UnitTypes)GC.getInfoTypeForString("UNIT_WRITER");
	}
	else if (this->GetCulture()->GetNumAvailableGreatWorkSlots(CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT()) > 0)
	{
		eGreatWorkSlot = CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT();
		eGreatWorkClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_ART");
		eUnitType = (UnitTypes)GC.getInfoTypeForString("UNIT_ARTIST");
	}
	else if (this->GetCulture()->GetNumAvailableGreatWorkSlots(CvTypes::getGREAT_WORK_SLOT_MUSIC()) > 0)
	{
		eGreatWorkSlot = CvTypes::getGREAT_WORK_SLOT_MUSIC();
		eGreatWorkClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_MUSIC");
		eUnitType = (UnitTypes)GC.getInfoTypeForString("UNIT_MUSICIAN");
	}
	

	if (eGreatWorkSlot != NO_GREAT_WORK_SLOT)
	{
#ifdef AUI_WARNING_FIXES
		uint iSlot = MAX_UNSIGNED_INT;
#else
		int iSlot = -1;
#endif
		BuildingClassTypes eBuildingClass = NO_BUILDINGCLASS;
		GreatWorkType eGreatWorkType = NO_GREAT_WORK;


		CvString strName;
		CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eUnitType);
		int iNumUnitCreated = GC.getGame().getUnitCreatedCount(eUnitType);
		int iNumNames = pkUnitEntry->GetNumUnitNames();
		if (iNumUnitCreated < iNumNames)
		{
			int iNameOffset = GC.getGame().getJonRandNum(iNumNames, "Unit name selection");
			int iI;
			for(iI = 0; iI < iNumNames; iI++)
			{
				int iIndex = (iNameOffset + iI) % iNumNames;
				strName = pkUnitEntry->GetUnitNames(iIndex);
				if(!GC.getGame().isGreatPersonBorn(strName))
				{
					eGreatWorkType = pkUnitEntry->GetGreatWorks(iIndex);
					GC.getGame().addGreatPersonBornName(strName);
					break;
				}
			}
		}


		int iGWindex;
		if (eGreatWorkType != NO_GREAT_WORK)
		{
			CvCity *pCity = this->GetCulture()->GetClosestAvailableGreatWorkSlot(pConqueredCity->getX(), pConqueredCity->getY(), eGreatWorkSlot, &eBuildingClass, &iSlot);
			if (pCity)
			{
				Localization::String name = Localization::Lookup(strName);
				CvString strBuffer;
				strBuffer.Format("%s (%s)", name.toUTF8(), pkUnitEntry->GetDescription());
				iGWindex = pCulture->CreateGreatWork(eGreatWorkType, eGreatWorkClass, pCity->getOwner(), this->GetCurrentEra(), strBuffer);
				pCity->GetCityBuildings()->SetBuildingGreatWork(eBuildingClass, iSlot, iGWindex);


				Localization::String localizedText;
				CvNotifications* pNotifications = this->GetNotifications();
				if(pNotifications)
				{
					localizedText = Localization::Lookup("TXT_KEY_GREAT_WORK_FROM_CONQUEST");
					localizedText << this->getNameKey() << pCulture->GetGreatWorkName(iGWindex);
					pNotifications->Add(NOTIFICATION_GREAT_WORK_COMPLETED_ACTIVE_PLAYER, localizedText.toUTF8(), localizedText.toUTF8(), pConqueredCity->getX(), pConqueredCity->getY(), iGWindex, this->GetID());
				}
			}
		}
	}
}
#endif



void CvPlayer::DoTechFromCityConquer(CvCity* pConqueredCity)
{
	PlayerTypes eOpponent = pConqueredCity->getOwner();
	FStaticVector<TechTypes, 128, true, c_eCiv5GameplayDLL> vePossibleTechs;
	int iCheapestTechCost = MAX_INT;
#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < GC.getNumTechInfos(); i++)
#else
	for (int i = 0; i < GC.getNumTechInfos(); i++)
#endif
	{
		TechTypes e = (TechTypes) i;
		CvTechEntry* pInfo = GC.getTechInfo(e);
		if (pInfo)
		{

			if (GET_TEAM(GET_PLAYER(eOpponent).getTeam()).GetTeamTechs()->HasTech(e))
			{

				if (!GET_TEAM(getTeam()).GetTeamTechs()->HasTech(e))
				{

					if (GetPlayerTechs()->CanResearch(e))
					{
						if (pInfo->GetResearchCost() < iCheapestTechCost)
						{
							iCheapestTechCost = pInfo->GetResearchCost();
							vePossibleTechs.clear();
							vePossibleTechs.push_back(e);
						}
						else if (pInfo->GetResearchCost() == iCheapestTechCost)
						{
							vePossibleTechs.push_back(e);
						}
					}
				}
			}
		}
	}

	if (!vePossibleTechs.empty())
	{
		int iRoll = GC.getGame().getJonRandNum((int)vePossibleTechs.size(), "Rolling to choose free tech from conquering a city");
		TechTypes eFreeTech = vePossibleTechs[iRoll];
		CvAssert(eFreeTech != NO_TECH)
		if (eFreeTech != NO_TECH)
		{
#ifdef NQ_SPOILS_OF_WAR

			if (GC.getGame().isOption("GAMEOPTION_SPOILS_OF_WAR"))
			{

				int iCost = GetPlayerTechs()->GetResearchCost(eFreeTech) * 100;
				int iMaxCost = GetPlayerTechs()->GetResearchCost(eFreeTech);
				int iPercentage = 0;

				CvTechEntry* pInfo = GC.getTechInfo(eFreeTech);
				EraTypes eEra = (EraTypes)pInfo->GetEra();
				switch (eEra)
				{
				case 0:
					iPercentage = 100;
					break;
				case 1:
					iPercentage = 100;
					break;
				case 2:
					iPercentage = 100;
					break;
				case 3:
					iPercentage = 100;
					break;
				case 4:
					iPercentage = 100;
					break;
				case 5:
					iPercentage = 90;
					break;
				case 6:
					iPercentage = 60;
					break;
				case 7:
					iPercentage = 30;
					break;
				}
				if (iPercentage > 0 && iPercentage <= 100)
				{
					iCost = (iCost / 100 * iPercentage) / 100;
				}

				else
				{
					iCost = 0;
				}

				if (iPercentage == 100)
				{
					GET_TEAM(getTeam()).setHasTech(eFreeTech, true, GetID(), true, true);
					GET_TEAM(getTeam()).GetTeamTechs()->SetNoTradeTech(eFreeTech, true);
				}
				else if (iCost != 0)
				{

					if ((GET_TEAM(getTeam()).GetTeamTechs()->GetResearchProgress(eFreeTech) + iCost) > iMaxCost)
					{
						GET_TEAM(getTeam()).GetTeamTechs()->SetResearchProgress(eFreeTech, 0, GetID());
						GET_TEAM(getTeam()).setHasTech(eFreeTech, true, GetID(), true, true);
						GET_TEAM(getTeam()).GetTeamTechs()->SetNoTradeTech(eFreeTech, true);
					}
					else
					{
						GET_TEAM(getTeam()).GetTeamTechs()->ChangeResearchProgress(eFreeTech, iCost, GetID());
						GET_TEAM(getTeam()).GetTeamTechs()->SetNoTradeTech(eFreeTech, true);
					}
				}
			}
			else
			{
				GET_TEAM(getTeam()).setHasTech(eFreeTech, true, GetID(), true, true);
				GET_TEAM(getTeam()).GetTeamTechs()->SetNoTradeTech(eFreeTech, true);
			}
#else
			GET_TEAM(getTeam()).setHasTech(eFreeTech, true, GetID(), true, true);
			GET_TEAM(getTeam()).GetTeamTechs()->SetNoTradeTech(eFreeTech, true);
			
#endif
			
		}
	}
}



int CvPlayer::GetTotalFaithPerTurn() const
{
	int iFaithPerTurn = 0;


	if(IsAnarchy())
		return 0;

#if !defined(STANDARDIZE_YIELDS)

	iFaithPerTurn += GetFaithPerTurnFromCities();


	iFaithPerTurn += GetFaithPerTurnFromMinorCivs();


	iFaithPerTurn += GetFaithPerTurnFromReligion();
#else

	iFaithPerTurn = getYieldTimes100(YIELD_FAITH, false                 ) / 100;
#endif

	return iFaithPerTurn;
}



int CvPlayer::GetFaithPerTurnFromCities() const
{
	int iFaithPerTurn = 0;

	const CvCity* pLoopCity;
	int iLoop;
	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
#if !defined(STANDARDIZE_YIELDS)
		iFaithPerTurn += pLoopCity->GetFaithPerTurn();
#else
		iFaithPerTurn += pLoopCity->getYieldRate(YIELD_FAITH, false                 );
#endif
	}

	return iFaithPerTurn;
}



int CvPlayer::GetFaithPerTurnFromMinorCivs() const
{
	int iFaithPerTurn = 0;
	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		iFaithPerTurn += GetFaithPerTurnFromMinor((PlayerTypes)iMinorLoop);
	}
	return iFaithPerTurn;
}



int CvPlayer::GetFaithPerTurnFromMinor(PlayerTypes eMinor) const
{
	int iFaithPerTurn = 0;

	if(GET_PLAYER(eMinor).isAlive())
	{
		iFaithPerTurn += GET_PLAYER(eMinor).GetMinorCivAI()->GetCurrentFaithBonus(GetID());
	}

	return iFaithPerTurn;
}



int CvPlayer::GetFaithPerTurnFromReligion() const
{
#if defined(STANDARDIZE_YIELDS)



	return getYieldFromReligionTimes100(YIELD_FAITH, getYieldTimes100(YIELD_FAITH, true)) / 100;
#else
	int iFaithPerTurn = 0;


	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	ReligionTypes eFoundedReligion = pReligions->GetFounderBenefitsReligion(GetID());
	if(eFoundedReligion != NO_RELIGION)
	{
		const CvReligion* pReligion = pReligions->GetReligion(eFoundedReligion, NO_PLAYER);
		if(pReligion)
		{
			iFaithPerTurn += pReligion->m_Beliefs.GetHolyCityYieldChange(YIELD_FAITH);

#ifdef NQ_FAITH_PER_FOREIGN_TRADE_ROUTE
			iFaithPerTurn += pReligion->m_Beliefs.GetFaithPerForeignTradeRoute() * GetTrade()->GetNumForeignTradeRoutes(GetID());
#endif

#ifdef NQ_FAITH_PER_CITY_STATE_THIS_RELIGION
			int iFaithPerCityStateThisReligionBonus = pReligion->m_Beliefs.GetFaithPerCityStateThisReligion();
			if (iFaithPerCityStateThisReligionBonus > 0)
			{
				int iNumCityStatesThisReligion = 0;
				PlayerTypes ePlayer;
				for (int iPlayerLoop = MAX_MAJOR_CIVS; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
				{
					ePlayer = (PlayerTypes) iPlayerLoop;
					if (GET_PLAYER(ePlayer).isMinorCiv())
					{
						CvCity* pkCity = GET_PLAYER(ePlayer).getCapitalCity();
						if(pkCity)
						{
							ReligionTypes eMinorReligion = pkCity->GetCityReligions()->GetReligiousMajority();
							if(eMinorReligion != NO_RELIGION && eMinorReligion == eFoundedReligion)
							{
								iNumCityStatesThisReligion++;
							}
						}
					}
				}
				iFaithPerTurn += iFaithPerCityStateThisReligionBonus * iNumCityStatesThisReligion;
			}
#endif

			int iTemp = pReligion->m_Beliefs.GetYieldChangePerForeignCity(YIELD_FAITH);
			if (iTemp > 0)
			{
				iFaithPerTurn += (iTemp * GetReligions()->GetNumForeignCitiesFollowing());
			}

			iTemp = pReligion->m_Beliefs.GetYieldChangePerXForeignFollowers(YIELD_FAITH);
			if (iTemp > 0)
			{
				int iFollowers = GetReligions()->GetNumForeignFollowers(false             );
				if (iFollowers > 0)
				{
					iFaithPerTurn += (iTemp / iFollowers);
				}
			}
		}
	}

	return iFaithPerTurn;
#endif
}


int CvPlayer::GetFaith() const
{
	return m_iFaith;
}


void CvPlayer::SetFaith(int iNewValue)
{
	if(GC.getGame().isOption(GAMEOPTION_NO_RELIGION))
	{
		return;
	}

	if(GetFaith() != iNewValue)
	{

		if(iNewValue > m_iFaith)
		{
			ChangeFaithEverGenerated(iNewValue - m_iFaith);
		}

		m_iFaith = iNewValue;

		if(GC.getGame().getActivePlayer() == GetID())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


void CvPlayer::ChangeFaith(int iChange)
{
	if(GC.getGame().isOption(GAMEOPTION_NO_RELIGION))
	{
		return;
	}

	SetFaith(GetFaith() + iChange);
}


int CvPlayer::GetFaithEverGenerated() const
{
	return m_iFaithEverGenerated;
}


void CvPlayer::SetFaithEverGenerated(int iNewValue)
{
	if(m_iFaithEverGenerated != iNewValue)
		m_iFaithEverGenerated = iNewValue;
}


void CvPlayer::ChangeFaithEverGenerated(int iChange)
{
	SetFaithEverGenerated(GetFaithEverGenerated() + iChange);
}



void CvPlayer::DoUpdateHappiness()
{

	m_iHappiness = getHandicapInfo().getHappinessDefault();


	int iNumHappinessFromResources = GetHappinessFromResources();
	m_iHappiness += iNumHappinessFromResources;


	m_iHappiness += GetHappinessFromCities();


	m_iHappiness += GetHappinessFromBuildings();


	m_iHappiness += GetHappinessFromPolicies();


	m_iHappiness += getNumCities() * m_iHappinessPerCity;


	m_iHappiness += GetHappinessFromReligion();


	m_iHappiness += GetHappinessFromNaturalWonders();


	m_iHappiness += GetHappinessFromMinorCivs();


	m_iHappiness += GetHappinessFromLeagues();


	DoUpdateCityConnectionHappiness();
	m_iHappiness += GetHappinessFromTradeRoutes();

	if(isLocalPlayer() && GetExcessHappiness() >= 100)
	{
		gDLL->UnlockAchievement(ACHIEVEMENT_XP2_45);
	}

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);

#ifdef LEKMOD_NEW_LUA_EVENTS

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem)
	{
		CvLuaArgsHandle args;

		args->Push(GetID());



		bool bResult = false;
		LuaSupport::CallHook(pkScriptSystem, "PlayerHappinessChanged", args.get(), bResult);
	}
#endif
}



int CvPlayer::GetHappiness() const
{
	return m_iHappiness;
}



void CvPlayer::SetHappiness(int iNewValue)
{
	if(GetHappiness() != iNewValue)
	{
		m_iHappiness = iNewValue;
	}
}



int CvPlayer::GetExcessHappiness() const
{
	return GetHappiness() - GetUnhappiness();
}



bool CvPlayer::IsEmpireUnhappy() const
{
	if(GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return false;
	}

	if(GetExcessHappiness() < 0)
	{
		return true;
	}

	return false;
}



bool CvPlayer::IsEmpireVeryUnhappy() const
{
	if(GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return false;
	}

	if(GetExcessHappiness() <=         GC.getVERY_UNHAPPY_THRESHOLD())
	{
		return true;
	}

	return false;
}



bool CvPlayer::IsEmpireSuperUnhappy() const
{
	if(GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return false;
	}

	if(GetExcessHappiness() <=         GC.getSUPER_UNHAPPY_THRESHOLD())
	{
		return true;
	}

	return false;
}



void CvPlayer::DoUpdateUprisings()
{
	if(IsEmpireVeryUnhappy())
	{

		if(GetUprisingCounter() > 0)
		{
			ChangeUprisingCounter(-1);


			if(GetUprisingCounter() == 0)
			{
				DoUprising();
				DoResetUprisingCounter(               false);
			}
		}

		else
		{
			DoResetUprisingCounter(               true);
		}
	}
}



int CvPlayer::GetUprisingCounter() const
{
	return m_iUprisingCounter;
}



void CvPlayer::SetUprisingCounter(int iValue)
{
	m_iUprisingCounter = iValue;
}



void CvPlayer::ChangeUprisingCounter(int iChange)
{
	SetUprisingCounter(GetUprisingCounter() + iChange);
}



void CvPlayer::DoResetUprisingCounter(bool bFirstTime)
{
	int iTurns =       GC.getUPRISING_COUNTER_MIN();
	CvGame& theGame = GC.getGame();
	int iExtra = theGame.getJonRandNum(      GC.getUPRISING_COUNTER_POSSIBLE(), "Uprising counter rand");
	iTurns += iExtra;


	int iMod = theGame.getGameSpeedInfo().getTrainPercent();

	if(iMod > 100)
	{
		iTurns *= iMod;
		iTurns /= 100;
	}

	if(bFirstTime)
		iTurns /= 2;

	if(iTurns <= 0)
		iTurns = 1;

	SetUprisingCounter(iTurns);
}



void CvPlayer::DoUprising()
{

	int iNumRebels =         GC.getUPRISING_NUM_BASE();
	int iExtraRoll = (getNumCities() - 1) *        GC.getUPRISING_NUM_CITY_COUNT();
	iExtraRoll += 100;
	iNumRebels += GC.getGame().getJonRandNum(iExtraRoll, "Rebel count rand roll");
	iNumRebels /= 100;


	CvCity* pBestCity = NULL;
	int iBestWeight = 0;

	int iTempWeight;

	CvCity* pLoopCity;
	int iLoop;
	CvGame& theGame = GC.getGame();
	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iTempWeight = pLoopCity->getPopulation();
		iTempWeight += theGame.getJonRandNum(10, "Uprising rand weight.");

		if(iTempWeight > iBestWeight)
		{
			iBestWeight = iTempWeight;
			pBestCity = pLoopCity;
		}
	}


	if(pBestCity != NULL)
	{
		int iBestPlot = -1;
		int iBestPlotWeight = -1;
		CvPlot* pPlot;

		CvCityCitizens* pCitizens = pBestCity->GetCityCitizens();


		for(int iPlotLoop = 1; iPlotLoop < NUM_CITY_PLOTS; iPlotLoop++)
		{
			pPlot = pCitizens->GetCityPlotFromIndex(iPlotLoop);

			if(!pPlot)
				continue;


			if(pPlot->isImpassable() || pPlot->isMountain())
				continue;


			if(pPlot->isWater())
				continue;


			if(pPlot->isCity())
				continue;


			if(pPlot->getNumUnits() > 0)
				continue;

			iTempWeight = theGame.getJonRandNum(10, "Uprising rand plot location.");


			if(pPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				iTempWeight += 4;


				if(pPlot->getResourceType(getTeam()) != NO_RESOURCE)
					iTempWeight += 3;
			}


			if(pPlot->defenseModifier(BARBARIAN_TEAM, false, false))
				iTempWeight += 4;


			if(pPlot->getOwner() != GetID())
				iTempWeight = -1;

			if(iTempWeight > iBestPlotWeight)
			{
				iBestPlotWeight = iTempWeight;
				iBestPlot = iPlotLoop;
			}
		}


		if(iBestPlot != -1)
		{

			theGame.SetBarbarianReleaseTurn(0);

			pPlot = pCitizens->GetCityPlotFromIndex(iBestPlot);


			UnitTypes eUnit = theGame.GetRandomSpawnUnitType(GetID(),                 false,                    false);

			CvNotifications* pNotifications = GetNotifications();
			if(pNotifications)
			{
				Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_REBELS");
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_REBELS_SUMMARY");
				pNotifications->Add(NOTIFICATION_REBELS, strMessage.toUTF8(), strSummary.toUTF8(), pPlot->getX(), pPlot->getY(), eUnit, BARBARIAN_PLAYER);
			}


			GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pPlot->getX(), pPlot->getY());
			iNumRebels--;


			do
			{
				iNumRebels--;


				CvUnit* pUnit = GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pPlot->getX(), pPlot->getY());
				CvAssert(pUnit);
				if (pUnit)
				{
					if (!pUnit->jumpToNearestValidPlotWithinRange(5))
						pUnit->kill(false);
				}
			}
			while(iNumRebels > 0);
		}
	}
}



void CvPlayer::DoUpdateCityRevolts()
{
	if(IsEmpireSuperUnhappy() && GetCulture()->GetPublicOpinionUnhappiness() > 0)
	{
		if(GetCityRevoltCounter() > 0)
		{
			ChangeCityRevoltCounter(-1);


			if(GetCityRevoltCounter() == 0)
			{
				DoCityRevolt();
				SetCityRevoltCounter(0);
			}
		}

		else
		{
			DoResetCityRevoltCounter();
		}
	}
}



int CvPlayer::GetCityRevoltCounter() const
{
	return m_iCityRevoltCounter;
}



void CvPlayer::SetCityRevoltCounter(int iValue)
{
	m_iCityRevoltCounter = iValue;
}



void CvPlayer::ChangeCityRevoltCounter(int iChange)
{
	SetCityRevoltCounter(GetCityRevoltCounter() + iChange);
}



void CvPlayer::DoResetCityRevoltCounter()
{
	int iTurns =       GC.getREVOLT_COUNTER_MIN();
	CvGame& theGame = GC.getGame();


	int iMod = theGame.getGameSpeedInfo().getTrainPercent();

	if(iMod > 100)
	{
		iTurns *= iMod;
		iTurns /= 100;
	}

	if(iTurns <= 0)
		iTurns = 1;

	CvCity *pMostUnhappyCity = GetMostUnhappyCity();
	PlayerTypes eRecipient = GetMostUnhappyCityRecipient();
	if(pMostUnhappyCity && eRecipient != NO_PLAYER)
	{
		SetCityRevoltCounter(iTurns);

		CvNotifications* pNotifications = GetNotifications();
		if(pNotifications && isHuman())
		{
#if defined(LEKMOD_MACOS)
			Localization::String strMessage = Localization::String(GetLocalizedText("TXT_KEY_NOTIFICATION_POSSIBLE_CITY_REVOLT", iTurns, pMostUnhappyCity->getName(), GET_PLAYER(eRecipient).getCivilizationShortDescription()).c_str());
#else
			Localization::String strMessage = GetLocalizedText("TXT_KEY_NOTIFICATION_POSSIBLE_CITY_REVOLT", iTurns, pMostUnhappyCity->getName(), GET_PLAYER(eRecipient).getCivilizationShortDescription());
#endif
			Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_POSSIBLE_CITY_REVOLT_SUMMARY");
			pNotifications->Add(NOTIFICATION_CITY_REVOLT_POSSIBLE, strMessage.toUTF8(), strSummary.toUTF8(), pMostUnhappyCity->getX(), pMostUnhappyCity->getY(), -1);
		}
	}
}



void CvPlayer::DoCityRevolt()
{
	CvCity *pMostUnhappyCity = GetMostUnhappyCity();
	PlayerTypes eRecipient = GetMostUnhappyCityRecipient();
	if(pMostUnhappyCity && eRecipient != NO_PLAYER)
	{
		CvPlayer &kRecipient = GET_PLAYER(eRecipient);
		for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
			PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
			CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
			CvNotifications* pNotifications = kCurNotifyPlayer.GetNotifications();
			if(pNotifications)
			{
				Localization::String strMessage;
				if (eNotifyPlayer == GetID())
				{
					strMessage = GetLocalizedText("TXT_KEY_NOTIFICATION_CITY_REVOLT", pMostUnhappyCity->getName(), kRecipient.getCivilizationShortDescription());
				}
				else
				{
					strMessage = GetLocalizedText("TXT_KEY_NOTIFICATION_OTHER_PLAYER_CITY_REVOLT", getCivilizationAdjective(), pMostUnhappyCity->getName(), kRecipient.getCivilizationShortDescription());
				}
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_REVOLT_SUMMARY");
				pNotifications->Add(NOTIFICATION_CITY_REVOLT, strMessage.toUTF8(), strSummary.toUTF8(), pMostUnhappyCity->getX(), pMostUnhappyCity->getY(), -1);
			}
		}

		kRecipient.acquireCity(pMostUnhappyCity, false             , false         );


		CvPlot *pPlot = pMostUnhappyCity->plot();
		if(pPlot->getNumUnits() > 0)
		{

			IDInfoVector currentUnits;
			if (pPlot->getUnits(&currentUnits) > 0)
			{
				for(IDInfoVector::const_iterator itr = currentUnits.begin(); itr != currentUnits.end(); ++itr)
				{
					CvUnit* pLoopUnit = (CvUnit*)GetPlayerUnit(*itr);

					if(pLoopUnit && pLoopUnit->getOwner() == GetID())
					{
						pLoopUnit->finishMoves();
						if (!pLoopUnit->jumpToNearestValidPlot())
							pLoopUnit->kill(false);
					}
				}
			}
		}
	}
}



CvCity *CvPlayer::GetMostUnhappyCity()
{
	CvCity *pRtnValue = NULL;
	int iHighestUnhappiness = -1;

	if (getNumCities() > 1)
	{
		PolicyBranchTypes ePreferredIdeology = GetCulture()->GetPublicOpinionPreferredIdeology();

		int iLoop;
		CvCity* pLoopCity;
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			if (pLoopCity->isCapital())
			{
				continue;
			}


			int iUnhappiness = pLoopCity->getPopulation();


			iUnhappiness -= pLoopCity->GetLocalHappiness(); 


			for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
			{
				CvPlayer &kPlayer = GET_PLAYER((PlayerTypes)iLoopPlayer);
				if (iLoopPlayer != GetID() && kPlayer.isAlive() && !kPlayer.isMinorCiv())
				{
					PolicyBranchTypes eOtherCivIdeology = kPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();
					if (eOtherCivIdeology == ePreferredIdeology)
					{
						int iCulturalDominanceOverUs = kPlayer.GetCulture()->GetInfluenceLevel(GetID()) - GetCulture()->GetInfluenceLevel((PlayerTypes)iLoopPlayer);
						if (iCulturalDominanceOverUs > 0 && kPlayer.getCapitalCity() != NULL)
						{

							int iCapitalDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), kPlayer.getCapitalCity()->getX(), kPlayer.getCapitalCity()->getY());
							if (iCapitalDistance < 100)
							{
								int iDistanceFactor = 100 - iCapitalDistance;
								iDistanceFactor = (int)sqrt((float)iDistanceFactor);
								iUnhappiness += (iDistanceFactor * iCulturalDominanceOverUs);
							}
						}
					}
				}
			}

			if (iUnhappiness > iHighestUnhappiness)
			{
				iHighestUnhappiness = iUnhappiness;
				pRtnValue = pLoopCity;
			}
		}
	}

	return pRtnValue;
}


PlayerTypes CvPlayer::GetMostUnhappyCityRecipient()
{
	PlayerTypes eRtnValue = NO_PLAYER;
	CvCity *pMostUnhappyCity = GetMostUnhappyCity();
	int iClosestCapital = MAX_INT;

	if (pMostUnhappyCity)
	{
		PolicyBranchTypes ePreferredIdeology = GetCulture()->GetPublicOpinionPreferredIdeology();


		for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
		{
			CvPlayer &kPlayer = GET_PLAYER((PlayerTypes)iLoopPlayer);
			if (iLoopPlayer != GetID() && kPlayer.isAlive() && !kPlayer.isMinorCiv())
			{
				PolicyBranchTypes eOtherCivIdeology = kPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();
				if (eOtherCivIdeology == ePreferredIdeology)
				{
					int iCulturalDominanceOverUs = kPlayer.GetCulture()->GetInfluenceLevel(GetID()) - GetCulture()->GetInfluenceLevel((PlayerTypes)iLoopPlayer);
					if (iCulturalDominanceOverUs > 0 && kPlayer.getCapitalCity() != NULL)
					{

						int iCapitalDistance = plotDistance(pMostUnhappyCity->getX(), pMostUnhappyCity->getY(), kPlayer.getCapitalCity()->getX(), kPlayer.getCapitalCity()->getY());

						if (iCapitalDistance < iClosestCapital)
						{
							iClosestCapital = iCapitalDistance;
							eRtnValue = (PlayerTypes)iLoopPlayer;
						}
					}
				}
			}
		}
	}

	return eRtnValue;
}



int CvPlayer::GetHappinessFromPolicies() const
{
	int iHappiness = m_pPlayerPolicies->GetNumericModifier(POLICYMOD_EXTRA_HAPPINESS);
	iHappiness += (getNumCities() * m_pPlayerPolicies->GetNumericModifier(POLICYMOD_EXTRA_HAPPINESS_PER_CITY));

	int iHappinessPerXPopulation;
	iHappinessPerXPopulation = GetHappinessPerXPopulation();

	if(iHappinessPerXPopulation > 0)
	{
		const CvCity* pLoopCity;
		int iLoop;
		for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			if(pLoopCity && !pLoopCity->IsPuppet())
			{
				int iExtraHappiness = pLoopCity->getPopulation() / iHappinessPerXPopulation;

				iHappiness += iExtraHappiness;
			}
		}
	}

#ifdef NQ_HAPPINESS_FROM_GREAT_IMPROVEMENTS_FROM_POLICIES
	iHappiness += (getGreatPersonImprovementCount() * m_pPlayerPolicies->GetNumericModifier(POLICYMOD_HAPPINESS_FROM_GREAT_IMPROVEMENTS));
#endif

	return iHappiness;
}



int CvPlayer::GetHappinessFromCities() const
{
	int iHappiness = 0;

	const CvCity* pLoopCity;
	int iLoop;
	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iHappiness += pLoopCity->GetLocalHappiness();
	}

	return iHappiness;
}



int CvPlayer::GetHappinessFromBuildings() const
{
	int iHappiness = 0;
	BuildingClassTypes eBuildingClass;


	int iSpecialBuildingHappiness = 0;
#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#else
	for(int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#endif
	{
		eBuildingClass = (BuildingClassTypes) iI;

		CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
		if(!pkBuildingClassInfo)
		{
			continue;
		}

		BuildingTypes eBuilding = (BuildingTypes) getCivilizationInfo().getCivilizationBuildings(eBuildingClass);
		if(eBuilding != NO_BUILDING && countNumBuildings(eBuilding) > 0)
		{
			CvBuildingEntry* pkBuilding = GC.getBuildingInfo(eBuilding);
			if(pkBuilding)
			{
#ifdef AUI_WARNING_FIXES
				for (uint jJ = 0; jJ < GC.getNumBuildingClassInfos(); jJ++)
#else
				for(int jJ = 0; jJ < GC.getNumBuildingClassInfos(); jJ++)
#endif
				{
					BuildingClassTypes eBuildingClassThatGivesHappiness = (BuildingClassTypes) jJ;
					int iHappinessPerBuilding = pkBuilding->GetBuildingClassHappiness(eBuildingClassThatGivesHappiness);
					if(iHappinessPerBuilding > 0)
					{
						BuildingTypes eBuildingThatGivesHappiness = (BuildingTypes) getCivilizationInfo().getCivilizationBuildings(eBuildingClassThatGivesHappiness);
						if(eBuildingThatGivesHappiness != NO_BUILDING)
						{
							iSpecialBuildingHappiness += iHappinessPerBuilding * countNumBuildings(eBuildingThatGivesHappiness);
						}
					}
				}
			}
		}
	}
#if defined(TRAITIFY)

	int iTraitBuildingHappiness = 0;


	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		BuildingClassTypes eBuildingClass = (BuildingClassTypes)iI;
		CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
		if (!pkBuildingClassInfo)
		{
			continue;
		}

		int iTraitHappiness = GetPlayerTraits()->GetBuildingClassGlobalHappiness(eBuildingClass);

		if (iTraitHappiness > 0)
		{
			BuildingTypes eBuilding = (BuildingTypes)getCivilizationInfo().getCivilizationBuildings(eBuildingClass);
			if (eBuilding != NO_BUILDING)
			{
				iTraitBuildingHappiness += iTraitHappiness * countNumBuildings(eBuilding);
			}
		}
	}

	iHappiness += iTraitBuildingHappiness;
#endif
	iHappiness += iSpecialBuildingHappiness;

	const CvCity* pLoopCity;
	int iLoop;
	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iHappiness += pLoopCity->GetHappinessFromBuildings();
	}


	if(m_iHappinessPerXPolicies > 0)
	{
		iHappiness += GetPlayerPolicies()->GetNumPoliciesOwned() / m_iHappinessPerXPolicies;
	}

	return iHappiness;
}



int CvPlayer::GetExtraHappinessPerCity() const
{
	return m_iHappinessPerCity;
}



void CvPlayer::ChangeExtraHappinessPerCity(int iChange)
{
	CvAssertMsg(m_iHappinessPerCity >= 0, "Count of buildings helping Happiness is corrupted");

	if(iChange != 0)
		m_iHappinessPerCity += iChange;
}



int CvPlayer::GetExtraHappinessPerXPolicies() const
{
	return m_iHappinessPerXPolicies;
}



void CvPlayer::ChangeExtraHappinessPerXPolicies(int iChange)
{
	CvAssertMsg(m_iHappinessPerXPolicies >= 0, "Count of extra happiness per buildings is corrupted");

	if(iChange != 0)
		m_iHappinessPerXPolicies += iChange;
}



int CvPlayer::GetHappinessFromResources() const
{
	int iTotalHappiness = 0;

	int iBaseHappiness;


	ResourceTypes eResource;
#ifdef AUI_WARNING_FIXES
	for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
	{
		eResource = (ResourceTypes) iResourceLoop;
#ifdef LEKMOD_SIAM_TRAIT_HAPPINESS

		int iTraitModifier = GetPlayerTraits()->GetCityStateBonusModifier();
		iBaseHappiness = GetHappinessFromLuxury(eResource);
		if (iBaseHappiness)
		{



			if (iTraitModifier != 0 && getResourceFromMinors(eResource) > 0)
			{
				if (IsMinorResourceBonus())
				{

					if (GC.getResourceInfo(eResource)->isOnlyMinorCivs())
					{
						iBaseHappiness *= ((iTraitModifier) + GC.getMINOR_POLICY_RESOURCE_HAPPINESS_MULTIPLIER());
						iBaseHappiness /= 100;
					}
					else
					{
						iBaseHappiness *= GC.getMINOR_POLICY_RESOURCE_HAPPINESS_MULTIPLIER();
						iBaseHappiness /= 100;
					}
				}
				else if (GC.getResourceInfo(eResource)->isOnlyMinorCivs())
				{
					iBaseHappiness *= (iTraitModifier + 100);
					iBaseHappiness /= 100;
				}
			}
			else
			{
				if (IsMinorResourceBonus() && getResourceFromMinors(eResource) > 0)
				{
					iBaseHappiness *=         GC.getMINOR_POLICY_RESOURCE_HAPPINESS_MULTIPLIER();
					iBaseHappiness /= 100;
				}
			}

			iTotalHappiness += iBaseHappiness;
			iTotalHappiness += GetExtraHappinessPerLuxury();
		}
#else
		iBaseHappiness = GetHappinessFromLuxury(eResource);
		if (iBaseHappiness)
		{

			if (IsMinorResourceBonus() && getResourceFromMinors(eResource) > 0)
			{
				iBaseHappiness *=         GC.getMINOR_POLICY_RESOURCE_HAPPINESS_MULTIPLIER();
				iBaseHappiness /= 100;
			}

			iTotalHappiness += iBaseHappiness;
			iTotalHappiness += GetExtraHappinessPerLuxury();
		}
#endif
	}


	iTotalHappiness += GetHappinessFromResourceVariety();

	return iTotalHappiness;
}



int CvPlayer::GetHappinessFromResourceVariety() const
{
	int iHappiness = 0;

	int iMultipleLuxuriesBonus =       GC.getHAPPINESS_PER_EXTRA_LUXURY();


	int iNumHappinessResources = 0;

	ResourceTypes eResource;
#ifdef AUI_WARNING_FIXES
	for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
	{
		eResource = (ResourceTypes) iResourceLoop;

		if(GetHappinessFromLuxury(eResource) > 0)
		{
			iNumHappinessResources++;
		}
	}

	if(iNumHappinessResources > 1)
	{
		iHappiness += (--iNumHappinessResources * iMultipleLuxuriesBonus);
	}

	return iHappiness;
}




int CvPlayer::GetHappinessFromReligion()
{
	int iHappinessFromReligion = 0;
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();


	ReligionTypes eFoundedReligion = pReligions->GetFounderBenefitsReligion(GetID());
	if(eFoundedReligion != NO_RELIGION)
	{
		const CvReligion* pReligion = pReligions->GetReligion(eFoundedReligion, NO_PLAYER);
		if(pReligion)
		{
			bool bAtPeace = GET_TEAM(getTeam()).getAtWarCount(false) == 0;
			iHappinessFromReligion += pReligion->m_Beliefs.GetPlayerHappiness(bAtPeace);

			float iHappinessPerFollowingCity = pReligion->m_Beliefs.GetHappinessPerFollowingCity();
			iHappinessFromReligion += (int)((float)pReligions->GetNumCitiesFollowing(eFoundedReligion) * iHappinessPerFollowingCity);

			int iHappinessPerXPeacefulForeignFollowers = pReligion->m_Beliefs.GetHappinessPerXPeacefulForeignFollowers();
			if (iHappinessPerXPeacefulForeignFollowers > 0)
			{

				iHappinessFromReligion += GetReligions()->GetNumForeignFollowers(false              ) / iHappinessPerXPeacefulForeignFollowers;
			}
		}
	}

	return iHappinessFromReligion;
}



int CvPlayer::GetHappinessFromNaturalWonders() const
{
	int iNumWonders = GET_TEAM(getTeam()).GetNumNaturalWondersDiscovered();

	int iHappiness = iNumWonders*       GC.getHAPPINESS_PER_NATURAL_WONDER();


	if(m_pTraits->GetNaturalWonderHappinessModifier() > 0)
	{
		iHappiness *= (100 + m_pTraits->GetNaturalWonderHappinessModifier());
		iHappiness /= 100;
	}

#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getMap().numPlots(); iI++)
#else
	for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
#endif
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iI);
		if(pPlot == NULL)
		{
			continue;
		}

		if(pPlot->getOwner() != m_eID)
		{
			continue;
		}

		FeatureTypes eFeature = pPlot->getFeatureType();
		if(eFeature == NO_FEATURE)
		{
			continue;
		}

		int iPlotHappiness = GC.getFeatureInfo(eFeature)->getInBorderHappiness();

		if(iPlotHappiness > 0)
		{

			if(m_pTraits->GetNaturalWonderYieldModifier() > 0)
			{
				iPlotHappiness *= (100 + m_pTraits->GetNaturalWonderYieldModifier());
				iPlotHappiness /= 100;
			}

			iHappiness += iPlotHappiness;
		}
	}

	return iHappiness;
}



int CvPlayer::GetExtraHappinessPerLuxury() const
{
	return m_iExtraHappinessPerLuxury;
}



void CvPlayer::ChangeExtraHappinessPerLuxury(int iChange)
{



	m_iExtraHappinessPerLuxury += iChange;
}



int CvPlayer::GetHappinessFromLuxury(ResourceTypes eResource) const
{
	CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
	if(pkResourceInfo)
	{
		int iBaseHappiness = pkResourceInfo->getHappiness();

		if (GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(GetID(), eResource))
		{
			iBaseHappiness = 0;
		}


		if(pkResourceInfo->getResourceUsage() != RESOURCEUSAGE_LUXURY)
		{
			return 0;
		}


		else if(getNumResourceAvailable(eResource,                    true) > 0)
		{

			return iBaseHappiness + GetPlayerTraits()->GetExtraHappinessPerLuxury();

		}
		










	}

#ifdef AUI_WARNING_FIXES
	return 0;
#else
	return false;
#endif
}




int CvPlayer::GetUnhappinessFromUnits() const
{
	int iUnhappinessFromUnits = m_iUnhappinessFromUnits;

	int iFreeUnitUnhappiness =       GC.getFREE_UNIT_HAPPINESS();
	if(iFreeUnitUnhappiness != 0)
	{
		iUnhappinessFromUnits -= iFreeUnitUnhappiness;
	}


	if(iUnhappinessFromUnits < 0)
	{
		iUnhappinessFromUnits = 0;
	}

	if(GetUnhappinessFromUnitsMod() != 0)
	{
		iUnhappinessFromUnits *= (100 + GetUnhappinessFromUnitsMod());
		iUnhappinessFromUnits /= 100;
	}

	return iUnhappinessFromUnits;
}



void CvPlayer::ChangeUnhappinessFromUnits(int iChange)
{
	m_iUnhappinessFromUnits += iChange;
}



int CvPlayer::GetUnhappiness(CvCity* pAssumeCityAnnexed, CvCity* pAssumeCityPuppeted) const
{
	int iUnhappiness = 0;


	iUnhappiness += GetUnhappinessFromCityCount(pAssumeCityAnnexed, pAssumeCityPuppeted);


	iUnhappiness += GetUnhappinessFromCapturedCityCount(pAssumeCityAnnexed, pAssumeCityPuppeted);


	iUnhappiness += GetUnhappinessFromCityPopulation(pAssumeCityAnnexed, pAssumeCityPuppeted);


	iUnhappiness += GetUnhappinessFromOccupiedCities(pAssumeCityAnnexed, pAssumeCityPuppeted);


	iUnhappiness += GetUnhappinessFromUnits();

	iUnhappiness /= 100;

	iUnhappiness += GetCulture()->GetPublicOpinionUnhappiness();


	if(!isHuman() && !IsAITeammateOfHuman())
	{
		iUnhappiness *= GC.getGame().getHandicapInfo().getAIUnhappinessPercent();
		iUnhappiness /= 100;
	}

	return iUnhappiness;
}



int CvPlayer::GetUnhappinessFromCityForUI(CvCity* pCity) const
{
	int iNumCitiesUnhappinessTimes100 = 0;
	int iPopulationUnhappinessTimes100 = 0;

	int iPopulation = pCity->getPopulation() * 100;


	if(isHalfSpecialistUnhappiness())
	{
		int iSpecialistCount = pCity->GetCityCitizens()->GetTotalSpecialistCount() * 100;
		iPopulation -= (iSpecialistCount / 2);
	}

	if(isHalfMoreSpecialistUnhappiness())
	{
		int iSpecialistCount = pCity->GetCityCitizens()->GetTotalSpecialistCount() * 100;
		iPopulation += (iSpecialistCount / 2);
	}


	if(pCity->IsOccupied() && !pCity->IsIgnoreCityForHappiness())
	{


			iNumCitiesUnhappinessTimes100 += (100 *       GC.getUNHAPPINESS_PER_CAPTURED_CITY());
			iPopulationUnhappinessTimes100 += int(iPopulation*           GC.getUNHAPPINESS_PER_OCCUPIED_POPULATION());



		if(GetOccupiedPopulationUnhappinessMod() != 0)
		{
			iPopulationUnhappinessTimes100 *= (100 + GetOccupiedPopulationUnhappinessMod());
			iPopulationUnhappinessTimes100 /= 100;
		}
	}

	else
	{
		iNumCitiesUnhappinessTimes100 += (100 *       GC.getUNHAPPINESS_PER_CITY());
		iPopulationUnhappinessTimes100 += (iPopulation*       GC.getUNHAPPINESS_PER_POPULATION());

		if(pCity->isCapital() && GetCapitalUnhappinessMod() != 0)
		{
			iPopulationUnhappinessTimes100 *= (100 + GetCapitalUnhappinessMod());
			iPopulationUnhappinessTimes100 /= 100;
		}

		iPopulationUnhappinessTimes100 *= (100 + GetUnhappinessMod());
		iPopulationUnhappinessTimes100 /= 100;

		iPopulationUnhappinessTimes100 *= 100 + GetPlayerTraits()->GetPopulationUnhappinessModifier();
		iPopulationUnhappinessTimes100 /= 100;
	}


	iPopulationUnhappinessTimes100 *= getHandicapInfo().getPopulationUnhappinessMod();
	iPopulationUnhappinessTimes100 /= 100;


	int iMod = 0;
	iMod += GetCityCountUnhappinessMod();
	iMod += GetPlayerTraits()->GetCityUnhappinessModifier();

	iNumCitiesUnhappinessTimes100 *= (100 + iMod);
	iNumCitiesUnhappinessTimes100 /= 100;


	iNumCitiesUnhappinessTimes100 *= getHandicapInfo().getNumCitiesUnhappinessMod();
	iNumCitiesUnhappinessTimes100 /= 100;


	iNumCitiesUnhappinessTimes100 *= GC.getMap().getWorldInfo().getNumCitiesUnhappinessPercent();
	iNumCitiesUnhappinessTimes100 /= 100;

	return iNumCitiesUnhappinessTimes100 + iPopulationUnhappinessTimes100;
}



int CvPlayer::GetUnhappinessFromCityCount(CvCity* pAssumeCityAnnexed, CvCity* pAssumeCityPuppeted) const
{
	int iUnhappiness = 0;
	int iUnhappinessPerCity =       GC.getUNHAPPINESS_PER_CITY() * 100;

	bool bCityValid;

	int iLoop;
	for(const CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		bCityValid = false;


		if(pLoopCity == pAssumeCityPuppeted)
			bCityValid = true;

		else if(pLoopCity == pAssumeCityAnnexed)
			bCityValid = false;

		else if(pLoopCity->IsIgnoreCityForHappiness())
			bCityValid = false;

		else if(!pLoopCity->IsOccupied() || pLoopCity->IsNoOccupiedUnhappiness())
			bCityValid = true;

		if(bCityValid)
			iUnhappiness += iUnhappinessPerCity;
	}


	int iMod = 0;
	iMod += GetCityCountUnhappinessMod();
	iMod += GetPlayerTraits()->GetCityUnhappinessModifier();

	iUnhappiness *= (100 + iMod);
	iUnhappiness /= 100;


	iUnhappiness *= getHandicapInfo().getNumCitiesUnhappinessMod();
	iUnhappiness /= 100;


	iUnhappiness *= GC.getMap().getWorldInfo().getNumCitiesUnhappinessPercent();
	iUnhappiness /= 100;

	return iUnhappiness;
}



int CvPlayer::GetUnhappinessFromCapturedCityCount(CvCity* pAssumeCityAnnexed, CvCity* pAssumeCityPuppeted) const
{
	int iUnhappiness = 0;
	int iUnhappinessPerCapturedCity =       GC.getUNHAPPINESS_PER_CAPTURED_CITY() * 100;

	bool bCityValid;

	int iLoop;
	for(const CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		bCityValid = false;


		if(pLoopCity == pAssumeCityPuppeted)
			bCityValid = false;

		else if(pLoopCity == pAssumeCityAnnexed)
			bCityValid = true;

		else if(pLoopCity->IsIgnoreCityForHappiness())
			bCityValid = false;

		else if(pLoopCity->IsOccupied() && !pLoopCity->IsNoOccupiedUnhappiness())
			bCityValid = true;


		if(bCityValid)
			iUnhappiness += iUnhappinessPerCapturedCity;
	}


	int iMod = 0;
	iMod += GetCityCountUnhappinessMod();
	iMod += GetPlayerTraits()->GetCityUnhappinessModifier();

	iUnhappiness *= (100 + iMod);
	iUnhappiness /= 100;


	iUnhappiness *= getHandicapInfo().getNumCitiesUnhappinessMod();
	iUnhappiness /= 100;


	iUnhappiness *= GC.getMap().getWorldInfo().getNumCitiesUnhappinessPercent();
	iUnhappiness /= 100;

	return iUnhappiness;
}



#if defined(AUI_CITIZENS_FIX_FORCED_AVOID_GROWTH_ONLY_WHEN_GROWING_LOWERS_HAPPINESS) || defined(AUI_CITIZENS_UNHARDCODE_SPECIALIST_VALUE_HAPPINESS)
int CvPlayer::GetUnhappinessFromCityPopulation(CvCity* pAssumeCityAnnexed, CvCity* pAssumeCityPuppeted, const CvCity* pAssumeCityGrows, const CvCity* pAssumeCityExtraSpecialist) const
#else
int CvPlayer::GetUnhappinessFromCityPopulation(CvCity* pAssumeCityAnnexed, CvCity* pAssumeCityPuppeted) const
#endif
{
	int iUnhappiness = 0;
	int iUnhappinessFromThisCity;

	int iUnhappinessPerPop =       GC.getUNHAPPINESS_PER_POPULATION() * 100;
	int iPopulation;
	int iSpecialistCount;

	bool bCityValid;

	int iLoop;
	for(const CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		bCityValid = false;


		if(pLoopCity == pAssumeCityAnnexed)
			bCityValid = false;

		else if(pLoopCity == pAssumeCityPuppeted)
			bCityValid = true;

		else if(pLoopCity->IsIgnoreCityForHappiness())
			bCityValid = false;

		else if(!pLoopCity->IsOccupied() || pLoopCity->IsNoOccupiedUnhappiness())
			bCityValid = true;

		if(bCityValid)
		{
			iPopulation = pLoopCity->getPopulation();
#if defined(AUI_CITIZENS_FIX_FORCED_AVOID_GROWTH_ONLY_WHEN_GROWING_LOWERS_HAPPINESS) || defined(AUI_CITIZENS_UNHARDCODE_SPECIALIST_VALUE_HAPPINESS)
			if (pLoopCity == pAssumeCityGrows)
				iPopulation++;
#endif


			if(isHalfSpecialistUnhappiness())
			{
				iSpecialistCount = pLoopCity->GetCityCitizens()->GetTotalSpecialistCount();
				iSpecialistCount++;
#if defined(AUI_CITIZENS_FIX_FORCED_AVOID_GROWTH_ONLY_WHEN_GROWING_LOWERS_HAPPINESS) || defined(AUI_CITIZENS_UNHARDCODE_SPECIALIST_VALUE_HAPPINESS)
				if (pLoopCity == pAssumeCityExtraSpecialist)
					iSpecialistCount++;
#endif
				iPopulation -= (iSpecialistCount / 2);
			}

			if(isHalfMoreSpecialistUnhappiness())
			{
				iSpecialistCount = pLoopCity->GetCityCitizens()->GetTotalSpecialistCount();
				iSpecialistCount++;
#if defined(AUI_CITIZENS_FIX_FORCED_AVOID_GROWTH_ONLY_WHEN_GROWING_LOWERS_HAPPINESS) || defined(AUI_CITIZENS_UNHARDCODE_SPECIALIST_VALUE_HAPPINESS)
				if (pLoopCity == pAssumeCityExtraSpecialist)
					iSpecialistCount++;
#endif
				iPopulation += (iSpecialistCount / 2);
			}

			iUnhappinessFromThisCity = iPopulation * iUnhappinessPerPop;

			if(pLoopCity->isCapital() && GetCapitalUnhappinessMod() != 0)
			{
				iUnhappinessFromThisCity *= (100 + GetCapitalUnhappinessMod());
				iUnhappinessFromThisCity /= 100;
			}
#ifdef TRAITIFY
			if (pLoopCity->IsPuppet())
			{
				int iTraitMod = GetPlayerTraits()->GetUnhappinessModifierForPuppetedCities();
				if (iTraitMod != 0)
				{
					iUnhappinessFromThisCity *= (100 + iTraitMod);
					iUnhappinessFromThisCity /= 100;
				}
			}
#endif

			iUnhappiness += iUnhappinessFromThisCity;
		}
	}

	iUnhappiness *= (100 + GetUnhappinessMod());
	iUnhappiness /= 100;

	iUnhappiness *= 100 + GetPlayerTraits()->GetPopulationUnhappinessModifier();
	iUnhappiness /= 100;


	iUnhappiness *= getHandicapInfo().getPopulationUnhappinessMod();
	iUnhappiness /= 100;

	return iUnhappiness;
}



int CvPlayer::GetUnhappinessFromPuppetCityPopulation() const
{
	int iUnhappiness = 0;
	int iUnhappinessPerPop = GC.getUNHAPPINESS_PER_POPULATION() * 100;

	int iLoop = 0;
	for(const CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		bool bCityValid = false;

		if(pLoopCity->IsPuppet())
			bCityValid = true;


		if(pLoopCity->IsIgnoreCityForHappiness())
			bCityValid = false;

		if(bCityValid)
		{
			int iPopulation = pLoopCity->getPopulation();













			iPopulation -= pLoopCity->GetCityCitizens()->GetTotalSpecialistCount();

			int iUnhappinessFromThisCity = iPopulation * iUnhappinessPerPop;

			if(pLoopCity->isCapital() && GetCapitalUnhappinessMod() != 0)
			{
				iUnhappinessFromThisCity *= (100 + GetCapitalUnhappinessMod());
				iUnhappinessFromThisCity /= 100;
			}

			iUnhappiness += iUnhappinessFromThisCity;
		}
	}

	iUnhappiness *= (100 + GetUnhappinessMod());
	iUnhappiness /= 100;

	iUnhappiness *= 100 + GetPlayerTraits()->GetPopulationUnhappinessModifier();
	iUnhappiness /= 100;
#if defined(TRAITIFY)
	iUnhappiness *= 100 + GetPlayerTraits()->GetUnhappinessModifierForPuppetedCities();
	iUnhappiness /= 100;
#endif

	iUnhappiness *= getHandicapInfo().getPopulationUnhappinessMod();
	iUnhappiness /= 100;

	return iUnhappiness;
}


int CvPlayer::GetUnhappinessFromCitySpecialists(CvCity* pAssumeCityAnnexed, CvCity* pAssumeCityPuppeted) const
{
	int iUnhappiness = 0;
	int iUnhappinessFromThisCity;

	int iUnhappinessPerPop =       GC.getUNHAPPINESS_PER_POPULATION() * 100;
	int iPopulation;

	bool bCityValid;

	int iLoop;
	for(const CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		bCityValid = false;


		if(pLoopCity == pAssumeCityAnnexed)
			bCityValid = false;

		else if(pLoopCity == pAssumeCityPuppeted)
			bCityValid = true;

		else if(pLoopCity->IsIgnoreCityForHappiness())
			bCityValid = false;

		else if(!pLoopCity->IsOccupied() || pLoopCity->IsNoOccupiedUnhappiness())
			bCityValid = true;

		if(bCityValid)
		{
			iPopulation = pLoopCity->GetCityCitizens()->GetTotalSpecialistCount();


			if(isHalfSpecialistUnhappiness())
			{
				iPopulation++;
				iPopulation /= 2;
			}
			if(isHalfMoreSpecialistUnhappiness())
			{
				iPopulation++;
				iPopulation = static_cast<int>(iPopulation * 0.5);
			}

			iUnhappinessFromThisCity = iPopulation * iUnhappinessPerPop;

			if(pLoopCity->isCapital() && GetCapitalUnhappinessMod() != 0)
			{
				iUnhappinessFromThisCity *= (100 + GetCapitalUnhappinessMod());
				iUnhappinessFromThisCity /= 100;
			}

			iUnhappiness += iUnhappinessFromThisCity;
		}
	}

	iUnhappiness *= (100 + GetUnhappinessMod());
	iUnhappiness /= 100;

	iUnhappiness *= 100 + GetPlayerTraits()->GetPopulationUnhappinessModifier();
	iUnhappiness /= 100;


	iUnhappiness *= getHandicapInfo().getPopulationUnhappinessMod();
	iUnhappiness /= 100;

	return iUnhappiness;
}



#if defined(AUI_CITIZENS_FIX_FORCED_AVOID_GROWTH_ONLY_WHEN_GROWING_LOWERS_HAPPINESS) || defined(AUI_CITIZENS_UNHARDCODE_SPECIALIST_VALUE_HAPPINESS)
int CvPlayer::GetUnhappinessFromOccupiedCities(CvCity* pAssumeCityAnnexed, CvCity* pAssumeCityPuppeted, const CvCity* pAssumeCityGrows, const CvCity* pAssumeCityExtraSpecialist) const
#else
int CvPlayer::GetUnhappinessFromOccupiedCities(CvCity* pAssumeCityAnnexed, CvCity* pAssumeCityPuppeted) const
#endif
{
	int iUnhappiness = 0;
	int iUnhappinessFromThisCity;

	double fUnhappinessPerPop =           GC.getUNHAPPINESS_PER_OCCUPIED_POPULATION() * 100;
	int iPopulation;
	int iSpecialistCount;

	bool bCityValid;

	int iLoop;
	for(const CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		bCityValid = false;


		if(pLoopCity == pAssumeCityAnnexed)
			bCityValid = true;

		else if(pLoopCity == pAssumeCityPuppeted)
			bCityValid = false;

		else if(pLoopCity->IsIgnoreCityForHappiness())
			bCityValid = false;


		else if(pLoopCity->IsOccupied() && !pLoopCity->IsNoOccupiedUnhappiness())
			bCityValid = true;

		if(bCityValid)
		{
			iPopulation = pLoopCity->getPopulation();
#if defined(AUI_CITIZENS_FIX_FORCED_AVOID_GROWTH_ONLY_WHEN_GROWING_LOWERS_HAPPINESS) || defined(AUI_CITIZENS_UNHARDCODE_SPECIALIST_VALUE_HAPPINESS)
			if (pLoopCity == pAssumeCityGrows)
				iPopulation++;
#endif


			if(isHalfSpecialistUnhappiness())
			{
				iSpecialistCount = pLoopCity->GetCityCitizens()->GetTotalSpecialistCount();
				iSpecialistCount++;
#if defined(AUI_CITIZENS_FIX_FORCED_AVOID_GROWTH_ONLY_WHEN_GROWING_LOWERS_HAPPINESS) || defined(AUI_CITIZENS_UNHARDCODE_SPECIALIST_VALUE_HAPPINESS)
				if (pLoopCity == pAssumeCityExtraSpecialist)
					iSpecialistCount++;
#endif
				iPopulation -= (iSpecialistCount / 2);
			}
			if(isHalfMoreSpecialistUnhappiness())
			{
				iSpecialistCount = pLoopCity->GetCityCitizens()->GetTotalSpecialistCount();
				iSpecialistCount++;
#if defined(AUI_CITIZENS_FIX_FORCED_AVOID_GROWTH_ONLY_WHEN_GROWING_LOWERS_HAPPINESS) || defined(AUI_CITIZENS_UNHARDCODE_SPECIALIST_VALUE_HAPPINESS)
				if (pLoopCity == pAssumeCityExtraSpecialist)
					iSpecialistCount++;
#endif
				iPopulation += (iSpecialistCount / 2);
			}

			iUnhappinessFromThisCity = int(double(iPopulation) * fUnhappinessPerPop);


			if(GetOccupiedPopulationUnhappinessMod() != 0)
			{
				iUnhappinessFromThisCity *= (100 + GetOccupiedPopulationUnhappinessMod());
				iUnhappinessFromThisCity /= 100;
			}

			iUnhappiness += iUnhappinessFromThisCity;
		}
	}


	iUnhappiness *= getHandicapInfo().getPopulationUnhappinessMod();
	iUnhappiness /= 100;

	return iUnhappiness;
}



int CvPlayer::GetUnhappinessFromUnitsMod() const
{
	return m_iUnhappinessFromUnitsMod;
}



void CvPlayer::ChangeUnhappinessFromUnitsMod(int iChange)
{
	if(iChange != 0)
	{
		m_iUnhappinessFromUnitsMod += iChange;
	}
}



int CvPlayer::GetUnhappinessMod() const
{
	return m_iUnhappinessMod;
}



void CvPlayer::ChangeUnhappinessMod(int iChange)
{
	if(iChange != 0)
	{
		m_iUnhappinessMod += iChange;
	}
}



int CvPlayer::GetCityCountUnhappinessMod() const
{
	return m_iCityCountUnhappinessMod;
}



void CvPlayer::ChangeCityCountUnhappinessMod(int iChange)
{
	if(iChange != 0)
	{
		m_iCityCountUnhappinessMod += iChange;
	}
}



int CvPlayer::GetOccupiedPopulationUnhappinessMod() const
{
	return m_iOccupiedPopulationUnhappinessMod;
}



void CvPlayer::ChangeOccupiedPopulationUnhappinessMod(int iChange)
{
	if(iChange != 0)
	{
		m_iOccupiedPopulationUnhappinessMod += iChange;
	}
}



int CvPlayer::GetCapitalUnhappinessMod() const
{
	return m_iCapitalUnhappinessMod;
}



void CvPlayer::ChangeCapitalUnhappinessMod(int iChange)
{
	if(iChange != 0)
	{
		m_iCapitalUnhappinessMod += iChange;
	}
}



int CvPlayer::GetHappinessPerGarrisonedUnit() const
{
	return m_iHappinessPerGarrisonedUnitCount;
}



void CvPlayer::SetHappinessPerGarrisonedUnit(int iValue)
{
	m_iHappinessPerGarrisonedUnitCount = iValue;
}



void CvPlayer::ChangeHappinessPerGarrisonedUnit(int iChange)
{
	SetHappinessPerGarrisonedUnit(m_iHappinessPerGarrisonedUnitCount + iChange);
}



int CvPlayer::GetHappinessFromTradeRoutes() const
{
	return m_iCityConnectionHappiness;
}



void CvPlayer::DoUpdateCityConnectionHappiness()
{
	int iHappinessPerTradeRoute = GetHappinessPerTradeRoute();
	int iNumCities = 0;
#ifdef NQ_RAIL_CONNECTION_HAPPINESS_FROM_POLICIES
	int iHappinessPerRailConnection = GetHappinessPerRailConnection();
	int iNumCitiesWithRailroads = 0;

	if (iHappinessPerTradeRoute != 0 || iHappinessPerRailConnection != 0)
#else
	if (iHappinessPerTradeRoute != 0)
#endif
	{
		CvCity* pCapitalCity = getCapitalCity();


		if(pCapitalCity != NULL && getNumCities() > 1)
		{
			CvCity* pLoopCity;

			int iLoop;
			for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				if(pLoopCity != pCapitalCity)
				{
					if(GetTreasury()->HasCityConnectionRouteBetweenCities(pCapitalCity, pLoopCity))
					{
						iNumCities++;
					}
				}
#ifdef NQ_RAIL_CONNECTION_HAPPINESS_FROM_POLICIES
				if (pLoopCity != pCapitalCity && pLoopCity->IsIndustrialRouteToCapital())
				{
					iNumCitiesWithRailroads++;
				}
#endif
			}
		}
	}
	m_iCityConnectionHappiness = iHappinessPerTradeRoute * iNumCities / 100;
#ifdef NQ_RAIL_CONNECTION_HAPPINESS_FROM_POLICIES
	m_iCityConnectionHappiness += iHappinessPerRailConnection * iNumCitiesWithRailroads / 100;
#endif
}



int CvPlayer::GetHappinessPerTradeRoute() const
{
	return m_iHappinessPerTradeRouteCount;
}



void CvPlayer::SetHappinessPerTradeRoute(int iValue)
{
	m_iHappinessPerTradeRouteCount = iValue;
}



void CvPlayer::ChangeHappinessPerTradeRoute(int iChange)
{
	SetHappinessPerTradeRoute(m_iHappinessPerTradeRouteCount + iChange);
}

#ifdef NQ_RAIL_CONNECTION_HAPPINESS_FROM_POLICIES


int CvPlayer::GetHappinessPerRailConnection() const
{
	return m_iHappinessPerRailConnectionCount;
}



void CvPlayer::SetHappinessPerRailConnection(int iValue)
{
	m_iHappinessPerRailConnectionCount = iValue;
}



void CvPlayer::ChangeHappinessPerRailConnection(int iChange)
{
	SetHappinessPerRailConnection(m_iHappinessPerRailConnectionCount + iChange);
}
#endif




int CvPlayer::GetHappinessPerXPopulation() const
{
	return m_iHappinessPerXPopulation;
}



void CvPlayer::SetHappinessPerXPopulation(int iValue)
{
	m_iHappinessPerXPopulation = iValue;
}



void CvPlayer::ChangeHappinessPerXPopulation(int iChange)
{
	SetHappinessPerXPopulation(m_iHappinessPerXPopulation + iChange);
}



int CvPlayer::GetHappinessFromMinorCivs() const
{
	int iHappiness = 0;
	PlayerTypes eMinor;
	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		eMinor = (PlayerTypes) iMinorLoop;
		iHappiness += GetHappinessFromMinor(eMinor);
	}
	return iHappiness;
}



int CvPlayer::GetHappinessFromMinor(PlayerTypes eMinor) const
{
	int iAmount = 0;

	if(GET_PLAYER(eMinor).isAlive())
	{

		iAmount += GET_PLAYER(eMinor).GetMinorCivAI()->GetCurrentHappinessBonus(GetID());
	}

	return iAmount;
}



int CvPlayer::GetHappinessFromLeagues() const
{
	return m_iHappinessFromLeagues;
}



void CvPlayer::SetHappinessFromLeagues(int iValue)
{
	m_iHappinessFromLeagues = iValue;
}



void CvPlayer::ChangeHappinessFromLeagues(int iChange)
{
	SetHappinessFromLeagues(GetHappinessFromLeagues() + iChange);
}



int CvPlayer::GetEspionageModifier() const
{
	return m_iEspionageModifier;
}



void CvPlayer::ChangeEspionageModifier(int iChange)
{
	m_iEspionageModifier = (m_iEspionageModifier + iChange);
}



int CvPlayer::GetStartingSpyRank() const
{
	return m_iSpyStartingRank;
}



void CvPlayer::ChangeStartingSpyRank(int iChange)
{
	m_iSpyStartingRank = (m_iSpyStartingRank + iChange);
}

#ifdef ENHANCED_GRAPHS
int CvPlayer::GetNumStolenScience() const
{
	return m_iNumStolenScience;
}
void CvPlayer::ChangeNumStolenScience(int iChange)
{
	m_iNumStolenScience = (m_iNumStolenScience + iChange);
}
#endif



int CvPlayer::GetExtraLeagueVotes() const
{
	return m_iExtraLeagueVotes;
}



void CvPlayer::ChangeExtraLeagueVotes(int iChange)
{
	m_iExtraLeagueVotes += iChange;
	CvAssert(m_iExtraLeagueVotes >= 0);
	if (m_iExtraLeagueVotes < 0)
	{
		m_iExtraLeagueVotes = 0;
	}
}
#if defined(TRAITIFY)


int CvPlayer::GetTraitExtraLeagueVotes() const
{
	return m_iTraitExtraLeagueVotes;
}


void CvPlayer::ChangeTraitExtraLeagueVotes(int iChange)
{
	m_iTraitExtraLeagueVotes += iChange;
	CvAssert(m_iTraitExtraLeagueVotes >= 0);
	if (m_iTraitExtraLeagueVotes < 0)
	{
		m_iTraitExtraLeagueVotes = 0;
	}
}
#endif
#if defined(MISC_CHANGES)


int CvPlayer::GetTechExtraLeagueVotes() const
{
	return m_iTechExtraLeagueVotes;
}


void CvPlayer::ChangeTechExtraLeagueVotes(int iChange)
{
	m_iTechExtraLeagueVotes += iChange;
	CvAssert(m_iTechExtraLeagueVotes >= 0);
	if (m_iTechExtraLeagueVotes < 0)
	{
		m_iTechExtraLeagueVotes = 0;
	}
}


int CvPlayer::GetPolicyExtraLeagueVotes() const
{
	return m_iPolicyExtraLeagueVotes;
}


void CvPlayer::ChangePolicyExtraLeagueVotes(int iChange)
{
	m_iPolicyExtraLeagueVotes += iChange;
	CvAssert(m_iPolicyExtraLeagueVotes >= 0);
	if (m_iPolicyExtraLeagueVotes < 0)
	{
		m_iPolicyExtraLeagueVotes = 0;
	}
}


int CvPlayer::GetNumMiscTradeRoutes() const
{
	return m_iMiscTradeRoutes;
}


void CvPlayer::ChangeNumMiscTradeRoutes(int iChange)
{
	m_iMiscTradeRoutes += iChange;
	CvAssert(m_iMiscTradeRoutes >= 0);
	if (m_iMiscTradeRoutes < 0)
	{
		m_iMiscTradeRoutes = 0;
	}
}
#endif
#if defined(LEKMOD_v34)


bool CvPlayer::CanChooseReformationBelief() const
{
	return m_bCanChooseReformationBelief;
}


void CvPlayer::SetCanChooseReformationBelief(int iValue)
{
	m_bCanChooseReformationBelief = iValue;
}
#endif


int CvPlayer::GetWoundedUnitDamageMod() const
{
	return m_iWoundedUnitDamageMod;
}



void CvPlayer::SetWoundedUnitDamageMod(int iValue)
{
	m_iWoundedUnitDamageMod = iValue;

	if(m_iWoundedUnitDamageMod <        -GC.getWOUNDED_DAMAGE_MULTIPLIER())
	{
		m_iWoundedUnitDamageMod =        -GC.getWOUNDED_DAMAGE_MULTIPLIER();
	}
}



void CvPlayer::ChangeWoundedUnitDamageMod(int iChange)
{
	SetWoundedUnitDamageMod(m_iWoundedUnitDamageMod + iChange);
}



int CvPlayer::GetUnitUpgradeCostMod() const
{
	return m_iUnitUpgradeCostMod;
}



void CvPlayer::SetUnitUpgradeCostMod(int iValue)
{
	m_iUnitUpgradeCostMod = iValue;

	if(m_iUnitUpgradeCostMod <         GC.getUNIT_UPGRADE_COST_DISCOUNT_MAX())
		m_iUnitUpgradeCostMod =         GC.getUNIT_UPGRADE_COST_DISCOUNT_MAX();
}



void CvPlayer::ChangeUnitUpgradeCostMod(int iChange)
{
	SetUnitUpgradeCostMod(m_iUnitUpgradeCostMod + iChange);
}



int CvPlayer::GetBarbarianCombatBonus() const
{
	return m_iBarbarianCombatBonus;
}



void CvPlayer::SetBarbarianCombatBonus(int iValue)
{
	m_iBarbarianCombatBonus = iValue;
}



void CvPlayer::ChangeBarbarianCombatBonus(int iChange)
{
	SetBarbarianCombatBonus(m_iBarbarianCombatBonus + iChange);
}



bool CvPlayer::IsAlwaysSeeBarbCamps() const
{
#ifdef NQ_ALWAYS_SEE_BARB_CAMPS
	return true;
#else
	return m_iAlwaysSeeBarbCampsCount > 0;
#endif
}



#ifdef NQ_ALWAYS_SEE_BARB_CAMPS
void CvPlayer::SetAlwaysSeeBarbCampsCount(int           )
{
#else
void CvPlayer::SetAlwaysSeeBarbCampsCount(int iValue)
{
	m_iAlwaysSeeBarbCampsCount = iValue;
#endif
}



#ifdef NQ_ALWAYS_SEE_BARB_CAMPS
void CvPlayer::ChangeAlwaysSeeBarbCampsCount(int            )
{
#else
void CvPlayer::ChangeAlwaysSeeBarbCampsCount(int iChange)
{
	SetAlwaysSeeBarbCampsCount(m_iAlwaysSeeBarbCampsCount + iChange);
#endif
}


CvPlayerTechs* CvPlayer::GetPlayerTechs() const
{
	return m_pPlayerTechs;
}


CvPlayerPolicies* CvPlayer::GetPlayerPolicies() const
{
	return m_pPlayerPolicies;
}


CvPlayerTraits* CvPlayer::GetPlayerTraits() const
{
	return m_pTraits;
}


CvFlavorManager* CvPlayer::GetFlavorManager() const
{
	return m_pFlavorManager;
}


CvTacticalAI* CvPlayer::GetTacticalAI() const
{
	return m_pTacticalAI;
}


CvHomelandAI* CvPlayer::GetHomelandAI() const
{
	return m_pHomelandAI;
}


void CvPlayer::setHasPolicy(PolicyTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumPolicyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(m_pPlayerPolicies->HasPolicy(eIndex) != bNewValue)
	{
		m_pPlayerPolicies->SetPolicy(eIndex, bNewValue);
		processPolicies(eIndex, bNewValue ? 1 : -1);
	}
}


int CvPlayer::getNextPolicyCost() const
{
	return m_iCostNextPolicy;
}


void CvPlayer::DoUpdateNextPolicyCost()
{
	m_iCostNextPolicy = GetPlayerPolicies()->GetNextPolicyCost();
}


bool CvPlayer::canAdoptPolicy(PolicyTypes eIndex) const
{
	return GetPlayerPolicies()->CanAdoptPolicy(eIndex);
}


void CvPlayer::doAdoptPolicy(PolicyTypes ePolicy)
{
	CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(ePolicy);
	CvAssert(pkPolicyInfo != NULL);
	if(pkPolicyInfo == NULL)
		return;


	if(!canAdoptPolicy(ePolicy))
		return;

	bool bTenet = pkPolicyInfo->GetLevel() > 0;


	if (bTenet && GetNumFreeTenets() > 0)
	{
		ChangeNumFreeTenets(-1, false);
	}
	else if (GetNumFreePolicies() > 0)
	{
		ChangeNumFreePolicies(-1);
	}
	else
	{
		changeJONSCulture(-getNextPolicyCost());
	}

	setHasPolicy(ePolicy, true);


	DoUpdateNextPolicyCost();


	PolicyBranchTypes ePolicyBranch = (PolicyBranchTypes) pkPolicyInfo->GetPolicyBranchType();
	GetPlayerPolicies()->SetPolicyBranchUnlocked(ePolicyBranch, true, false);

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);


	if(GC.getGame().getActivePlayer() == GetID())
	{
		GC.GetEngineUserInterface()->setDirty(Policies_DIRTY_BIT, true);
	}


	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());
		args->Push(ePolicy);



		bool bResult = false;
		LuaSupport::CallHook(pkScriptSystem, "PlayerAdoptPolicy", args.get(), bResult);
	}

	updateYield();
}



bool CvPlayer::IsAnarchy() const
{
	return GetAnarchyNumTurns() > 0;
}



int CvPlayer::GetAnarchyNumTurns() const
{
	return m_iAnarchyNumTurns;
}



void CvPlayer::SetAnarchyNumTurns(int iValue)
{
	if(iValue != GetAnarchyNumTurns())
	{
		m_iAnarchyNumTurns = iValue;

		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}



void CvPlayer::ChangeAnarchyNumTurns(int iChange)
{
	SetAnarchyNumTurns(GetAnarchyNumTurns() + iChange);
}


int CvPlayer::getAdvancedStartPoints() const
{
	return m_iAdvancedStartPoints;
}


void CvPlayer::setAdvancedStartPoints(int iNewValue)
{
	if(getAdvancedStartPoints() != iNewValue)
	{
		m_iAdvancedStartPoints = iNewValue;

		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(MiscButtons_DIRTY_BIT, true);
			GC.GetEngineUserInterface()->setDirty(SelectionButtons_DIRTY_BIT, true);
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


void CvPlayer::changeAdvancedStartPoints(int iChange)
{
	setAdvancedStartPoints(getAdvancedStartPoints() + iChange);
}




int CvPlayer::GetAttackBonusTurns() const
{
	return m_iAttackBonusTurns;
}



void CvPlayer::ChangeAttackBonusTurns(int iChange)
{
	if(iChange != 0)
	{
		m_iAttackBonusTurns += iChange;
	}
}



int CvPlayer::GetCultureBonusTurns() const
{
	return m_iCultureBonusTurns;
}



void CvPlayer::ChangeCultureBonusTurns(int iChange)
{
	if (iChange != 0)
	{
		m_iCultureBonusTurns += iChange;
	}
}



int CvPlayer::GetTourismBonusTurns() const
{
	return m_iTourismBonusTurns;
}



void CvPlayer::ChangeTourismBonusTurns(int iChange)
{
	if (iChange != 0)
	{
		m_iTourismBonusTurns += iChange;
	}
}
#if defined(LEKMOD_v34)


int CvPlayer::GetGoldenAgePointsFromCities()
{
	int iGoldenAgePoints = 0;
	int iLoop = 0;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iGoldenAgePoints += pLoopCity->getYieldRate(YIELD_GOLDEN_AGE_POINTS, false);
	}
	CvAssertMsg(iGoldenAgePoints >= 0, "Total Golden Age Points from cities is unexpectedly negative.");
	return iGoldenAgePoints;
}


int CvPlayer::GetTotalGoldenAgePointsInEmpire()
{
	int iGoldenAgePoints = 0;


	iGoldenAgePoints += GetExcessHappiness();

	iGoldenAgePoints += GetGoldenAgePointsFromCities();


	return iGoldenAgePoints;
}
#endif
#if defined(GAMEOPTION_GOLDENAGE_ALT)


void CvPlayer::DoProcessGoldenAge()
{
	if (GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return;
	}

	if (GC.getGame().isOption("GAMEOPTION_GOLDENAGE_ALT"))
	{

		if (!isMinorCiv() && !isBarbarian())
		{

			if (getGoldenAgeTurns() > 0)
			{

				if (!IsAnarchy())
				{
					changeGoldenAgeTurns(-1);
				}

#if defined(LEKMOD_v34)
				ChangeGoldenAgeProgressMeter(GetTotalGoldenAgePointsInEmpire());
#else
				ChangeGoldenAgeProgressMeter(GetExcessHappiness());
#endif

				if (GetGoldenAgeProgressMeter() >= GetGoldenAgeProgressThreshold())
				{
					int iOverflow = GetGoldenAgeProgressMeter() - GetGoldenAgeProgressThreshold();

					SetGoldenAgeProgressMeter(iOverflow);

					int iLength = getGoldenAgeLength();
					changeGoldenAgeTurns(iLength);


					if (GetID() == GC.getGame().getActivePlayer())
					{

						if (!GC.getGame().isNetworkMultiPlayer())
						{
							CvPopupInfo kPopupInfo(BUTTONPOPUP_GOLDEN_AGE_REWARD);
							GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
						}
					}
				}
			}
			else
			{
				ChangeGoldenAgeProgressMeter(GetTotalGoldenAgePointsInEmpire());


				if (GetGoldenAgeProgressMeter() >= GetGoldenAgeProgressThreshold())
				{
					int iOverflow = GetGoldenAgeProgressMeter() - GetGoldenAgeProgressThreshold();

					SetGoldenAgeProgressMeter(iOverflow);

					int iLength = getGoldenAgeLength();
					changeGoldenAgeTurns(iLength);


					if (GetID() == GC.getGame().getActivePlayer())
					{

						if (!GC.getGame().isNetworkMultiPlayer())
						{
							CvPopupInfo kPopupInfo(BUTTONPOPUP_GOLDEN_AGE_REWARD);
							GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
						}
					}
				}
			}
		}
	}
	else
	{

		if (!isMinorCiv() && !isBarbarian())
		{

			if (getGoldenAgeTurns() > 0)
			{
				if (!IsAnarchy())
				{
					changeGoldenAgeTurns(-1);
				}
			}


			else
			{

#if !defined(LEKMOD_v34)
				ChangeGoldenAgeProgressMeter(GetExcessHappiness());
#else
				ChangeGoldenAgeProgressMeter(GetTotalGoldenAgePointsInEmpire());
#endif

				if (GetGoldenAgeProgressMeter() >= GetGoldenAgeProgressThreshold())
				{
					int iOverflow = GetGoldenAgeProgressMeter() - GetGoldenAgeProgressThreshold();

					SetGoldenAgeProgressMeter(iOverflow);

					int iLength = getGoldenAgeLength();
					changeGoldenAgeTurns(iLength);


					if (GetID() == GC.getGame().getActivePlayer())
					{

						if (!GC.getGame().isNetworkMultiPlayer())
						{
							CvPopupInfo kPopupInfo(BUTTONPOPUP_GOLDEN_AGE_REWARD);
							GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
						}
					}
				}
			}
		}
	}
}
#else


void CvPlayer::DoProcessGoldenAge()
{
	if (GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return;
	}


	if (!isMinorCiv() && !isBarbarian())
	{

		if (getGoldenAgeTurns() > 0)
		{
			if (!IsAnarchy())
			{
				changeGoldenAgeTurns(-1);
			}
		}


		else
		{

#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
			ChangeGoldenAgeProgressMeter(getCachedExcessHappinessForThisTurn());
#else
#if !defined(LEKMOD_v34)
			ChangeGoldenAgeProgressMeter(GetExcessHappiness());
#else
			ChangeGoldenAgeProgressMeter(GetTotalGoldenAgePointsInEmpire());
#endif
#endif


			if (GetGoldenAgeProgressMeter() >= GetGoldenAgeProgressThreshold())
			{
				int iOverflow = GetGoldenAgeProgressMeter() - GetGoldenAgeProgressThreshold();

				SetGoldenAgeProgressMeter(iOverflow);

				int iLength = getGoldenAgeLength();
				changeGoldenAgeTurns(iLength);


				if (GetID() == GC.getGame().getActivePlayer())
				{

					if (!GC.getGame().isNetworkMultiPlayer())
					{
						CvPopupInfo kPopupInfo(BUTTONPOPUP_GOLDEN_AGE_REWARD);
						GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
					}
				}
			}
		}
	}
}
#endif


int CvPlayer::GetGoldenAgeProgressThreshold() const
{
	int iThreshold =         GC.getGOLDEN_AGE_BASE_THRESHOLD_HAPPINESS();
	iThreshold += GetNumGoldenAges() *         GC.getGOLDEN_AGE_EACH_GA_ADDITIONAL_HAPPINESS();


	int iCostExtra = int(iThreshold * (getNumCities() - 1) *          GC.getGOLDEN_AGE_THRESHOLD_CITY_MULTIPLIER());
	iThreshold += iCostExtra;

	if(GetGoldenAgeMeterMod() != 0)
	{
		iThreshold *= (100 + GetGoldenAgeMeterMod());
		iThreshold /= 100;
	}


	iThreshold *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iThreshold /= 100;


	int iVisibleDivisor =       GC.getGOLDEN_AGE_VISIBLE_THRESHOLD_DIVISOR();
	iThreshold /= iVisibleDivisor;
	iThreshold *= iVisibleDivisor;

	return iThreshold;
}



int CvPlayer::GetGoldenAgeProgressMeter() const
{
	return m_iGoldenAgeProgressMeter;
}



void CvPlayer::SetGoldenAgeProgressMeter(int iValue)
{
	m_iGoldenAgeProgressMeter = iValue;

	if(m_iGoldenAgeProgressMeter < 0)
	{
		m_iGoldenAgeProgressMeter = 0;
	}

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}



void CvPlayer::ChangeGoldenAgeProgressMeter(int iChange)
{
	SetGoldenAgeProgressMeter(GetGoldenAgeProgressMeter() + iChange);
}



int CvPlayer::GetGoldenAgeMeterMod() const
{
	return m_iGoldenAgeMeterMod;
}



void CvPlayer::SetGoldenAgeMeterMod(int iValue)
{
	m_iGoldenAgeMeterMod = iValue;
}



void CvPlayer::ChangeGoldenAgeMeterMod(int iChange)
{
	SetGoldenAgeMeterMod(GetGoldenAgeMeterMod() + iChange);
}



int CvPlayer::GetNumGoldenAges() const
{
	return m_iNumGoldenAges;
}



void CvPlayer::SetNumGoldenAges(int iValue)
{
	m_iNumGoldenAges = iValue;

	if(iValue > 0 && isHuman() && !GC.getGame().isGameMultiPlayer()&& GET_PLAYER(GC.getGame().getActivePlayer()).isLocalPlayer())
	{
		gDLL->UnlockAchievement(ACHIEVEMENT_GOLDEN_AGE);

		const char* strLeader = getLeaderTypeKey();
		if(m_iNumGoldenAges >=5 && NULL != strLeader && strcmp(strLeader, "LEADER_DARIUS") == 0)
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_ARCHAEMENNID);
		}
	}
}



void CvPlayer::ChangeNumGoldenAges(int iChange)
{
	SetNumGoldenAges(GetNumGoldenAges() + iChange);
}



int CvPlayer::getGoldenAgeTurns() const
{
	return m_iGoldenAgeTurns;
}


bool CvPlayer::isGoldenAge() const
{
	return (getGoldenAgeTurns() > 0);
}


void CvPlayer::changeGoldenAgeTurns(int iChange)
{
	Localization::String locString;
	Localization::String locSummaryString;

	bool bOldGoldenAge;

	if(iChange != 0)
	{
		bOldGoldenAge = isGoldenAge();

		m_iGoldenAgeTurns = (m_iGoldenAgeTurns + iChange);
		CvAssert(getGoldenAgeTurns() >= 0);

		if(bOldGoldenAge != isGoldenAge())
		{
			GC.getMap().updateYield();

			if(isGoldenAge())
			{
				ChangeNumGoldenAges(1);

				locString = Localization::Lookup("TXT_KEY_NOTIFICATION_GOLDEN_AGE_BEGUN");
				locString << getCivilizationAdjectiveKey();
				GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, GetID(), locString.toUTF8(), -1, -1);

				gDLL->GameplayGoldenAgeStarted();
			}
			else
			{
				gDLL->GameplayGoldenAgeEnded();
			}

			CvNotifications* pNotifications = GetNotifications();
			if(pNotifications)
			{
				NotificationTypes eNotification = NO_NOTIFICATION_TYPE;

				if(isGoldenAge())
				{
					eNotification = NOTIFICATION_GOLDEN_AGE_BEGUN_ACTIVE_PLAYER;
					locString = Localization::Lookup("TXT_KEY_NOTIFICATION_GOLDEN_AGE_BEGUN_ACTIVE_PLAYER");
					locSummaryString = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_GOLDEN_AGE_BEGUN_ACTIVE_PLAYER");
				}
				else
				{
					eNotification = NOTIFICATION_GOLDEN_AGE_ENDED_ACTIVE_PLAYER;
					locString = Localization::Lookup("TXT_KEY_NOTIFICATION_GOLDEN_AGE_ENDED_ACTIVE_PLAYER");
					locSummaryString = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_GOLDEN_AGE_ENDED_ACTIVE_PLAYER");
				}

				pNotifications->Add(eNotification, locString.toUTF8(), locSummaryString.toUTF8(), -1, -1, -1);
			}
#ifdef LEKMOD_NEW_LUA_EVENTS
			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if (pkScriptSystem)
			{
				CvLuaArgsHandle args;
				args->Push(GetID());

				bool bResult;
				LuaSupport::CallHook(pkScriptSystem, "PlayerSetGoldenAge", args.get(), bResult);
			}
#endif
		}

		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getGoldenAgeLength() const
{
	int iTurns = GC.getGame().goldenAgeLength();


	int iLengthModifier = getGoldenAgeModifier();


	iLengthModifier += GetPlayerTraits()->GetGoldenAgeDurationModifier();

	if(iLengthModifier > 0)
	{
		iTurns = iTurns * (100 + iLengthModifier) / 100;
	}

	return iTurns;
}


int CvPlayer::getNumUnitGoldenAges() const
{
	return m_iNumUnitGoldenAges;
}



void CvPlayer::changeNumUnitGoldenAges(int iChange)
{
	m_iNumUnitGoldenAges = (m_iNumUnitGoldenAges + iChange);
	CvAssert(getNumUnitGoldenAges() >= 0);
}



int CvPlayer::getStrikeTurns() const
{
	return m_iStrikeTurns;
}



void CvPlayer::changeStrikeTurns(int iChange)
{
	m_iStrikeTurns = (m_iStrikeTurns + iChange);
	CvAssert(getStrikeTurns() >= 0);
}



int CvPlayer::getGoldenAgeModifier() const
{
	return m_iGoldenAgeModifier;
}


void CvPlayer::changeGoldenAgeModifier(int iChange)
{
	m_iGoldenAgeModifier += iChange;
}


int CvPlayer::getGreatPeopleCreated() const
{
	return m_iGreatPeopleCreated;
}


void CvPlayer::incrementGreatPeopleCreated()
{
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	m_iGreatPeopleCreated += 1;
#else
	m_iGreatPeopleCreated++;
#endif
}


int CvPlayer::getGreatGeneralsCreated() const
{
	return m_iGreatGeneralsCreated;
}


void CvPlayer::incrementGreatGeneralsCreated()
{
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	m_iGreatGeneralsCreated += 1;
#else
	m_iGreatGeneralsCreated++;
#endif
}


int CvPlayer::getGreatAdmiralsCreated() const
{
	return m_iGreatAdmiralsCreated;
}


void CvPlayer::incrementGreatAdmiralsCreated()
{
	m_iGreatAdmiralsCreated++;
}


int CvPlayer::getGreatWritersCreated() const
{
	return m_iGreatWritersCreated;
}


void CvPlayer::incrementGreatWritersCreated()
{
	m_iGreatWritersCreated++;
}


int CvPlayer::getGreatArtistsCreated() const
{
	return m_iGreatArtistsCreated;
}


void CvPlayer::incrementGreatArtistsCreated()
{
	m_iGreatArtistsCreated++;
}


int CvPlayer::getGreatMusiciansCreated() const
{
	return m_iGreatMusiciansCreated;
}


void CvPlayer::incrementGreatMusiciansCreated()
{
	m_iGreatMusiciansCreated++;
}



int CvPlayer::getGreatScientistsCreated() const
{
	return m_iGreatScientistsCreated;
}


void CvPlayer::incrementGreatScientistsCreated()
{
	m_iGreatScientistsCreated++;
}


int CvPlayer::getGreatEngineersCreated() const
{
	return m_iGreatEngineersCreated;
}


void CvPlayer::incrementGreatEngineersCreated()
{
	m_iGreatEngineersCreated++;
}


int CvPlayer::getGreatMerchantsCreated() const
{
	return m_iGreatMerchantsCreated;
}


void CvPlayer::incrementGreatMerchantsCreated()
{
	m_iGreatMerchantsCreated++;
}


int CvPlayer::getGreatProphetsCreated() const
{
	return m_iGreatProphetsCreated;
}


void CvPlayer::incrementGreatProphetsCreated()
{
	m_iGreatProphetsCreated++;
}



int CvPlayer::getMerchantsFromFaith() const
{
	return m_iMerchantsFromFaith;
}


void CvPlayer::incrementMerchantsFromFaith()
{
	m_iMerchantsFromFaith++;
}


int CvPlayer::getScientistsFromFaith() const
{
	return m_iScientistsFromFaith;
}


void CvPlayer::incrementScientistsFromFaith()
{
	m_iScientistsFromFaith++;
}


int CvPlayer::getWritersFromFaith() const
{
	return m_iWritersFromFaith;
}


void CvPlayer::incrementWritersFromFaith()
{
	m_iWritersFromFaith++;
}


int CvPlayer::getArtistsFromFaith() const
{
	return m_iArtistsFromFaith;
}


void CvPlayer::incrementArtistsFromFaith()
{
	m_iArtistsFromFaith++;
}


int CvPlayer::getMusiciansFromFaith() const
{
	return m_iMusiciansFromFaith;
}


void CvPlayer::incrementMusiciansFromFaith()
{
	m_iMusiciansFromFaith++;
}


int CvPlayer::getGeneralsFromFaith() const
{
	return m_iGeneralsFromFaith;
}


void CvPlayer::incrementGeneralsFromFaith()
{
	m_iGeneralsFromFaith++;
}


int CvPlayer::getAdmiralsFromFaith() const
{
	return m_iAdmiralsFromFaith;
}


void CvPlayer::incrementAdmiralsFromFaith()
{
	m_iAdmiralsFromFaith++;
}


int CvPlayer::getEngineersFromFaith() const
{
	return m_iEngineersFromFaith;
}


void CvPlayer::incrementEngineersFromFaith()
{
	m_iEngineersFromFaith++;
}


int CvPlayer::getGreatPeopleThresholdModifier() const
{
	return m_iGreatPeopleThresholdModifier;
}


void CvPlayer::changeGreatPeopleThresholdModifier(int iChange)
{
	m_iGreatPeopleThresholdModifier = (m_iGreatPeopleThresholdModifier + iChange);
}


int CvPlayer::getGreatGeneralsThresholdModifier() const
{
	return m_iGreatGeneralsThresholdModifier;
}


void CvPlayer::changeGreatGeneralsThresholdModifier(int iChange)
{
	m_iGreatGeneralsThresholdModifier += iChange;
}


int CvPlayer::getGreatAdmiralsThresholdModifier() const
{
	return m_iGreatAdmiralsThresholdModifier;
}


void CvPlayer::changeGreatAdmiralsThresholdModifier(int iChange)
{
	m_iGreatAdmiralsThresholdModifier += iChange;
}



int CvPlayer::getPolicyCostModifier() const
{
	return m_iPolicyCostModifier;
}


void CvPlayer::recomputePolicyCostModifier()
{
	int iCost = m_pPlayerPolicies->GetNumericModifier(POLICYMOD_POLICY_COST_MODIFIER);
	iCost += GetPolicyCostBuildingModifier();
	iCost += GetPolicyCostMinorCivModifier();
	iCost += GetPlayerTraits()->GetPolicyCostModifier();

	if(iCost <         GC.getPOLICY_COST_DISCOUNT_MAX())
		iCost =         GC.getPOLICY_COST_DISCOUNT_MAX();

	m_iPolicyCostModifier = iCost;
}


int CvPlayer::getGreatPeopleRateModifier() const
{
	return m_iGreatPeopleRateModifier;
}


int CvPlayer::getGreatGeneralRateModifier() const
{
	return m_iGreatGeneralRateModifier;
}


int CvPlayer::getGreatAdmiralRateModifier() const
{
	return m_iGreatAdmiralRateModifier;
}


int CvPlayer::getGreatWriterRateModifier() const
{
	return m_iGreatWriterRateModifier;
}


int CvPlayer::getGreatArtistRateModifier() const
{
	return m_iGreatArtistRateModifier;
}


int CvPlayer::getGreatMusicianRateModifier() const
{
	return m_iGreatMusicianRateModifier;
}


int CvPlayer::getGreatMerchantRateModifier() const
{
	return m_iGreatMerchantRateModifier;
}


int CvPlayer::getGreatScientistRateModifier() const
{
	return m_iGreatScientistRateModifier;
}


int CvPlayer::getGreatEngineerRateModifier() const
{
	return m_iGreatEngineerRateModifier;
}


int CvPlayer::getDomesticGreatGeneralRateModifier() const
{
	return m_iDomesticGreatGeneralRateModifier;
}


void CvPlayer::changeGreatPeopleRateModFromBldgs(int ichange)
{
	m_iGreatPeopleRateModFromBldgs += ichange;
}


void CvPlayer::changeGreatGeneralRateModFromBldgs(int ichange)
{
	m_iGreatGeneralRateModFromBldgs += ichange;
}



void CvPlayer::DoUnitKilledCombat(PlayerTypes eKilledPlayer, UnitTypes eUnitType)
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());
		args->Push(eKilledPlayer);
		args->Push(eUnitType);

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "UnitKilledInCombat", args.get(), bResult);
	}
}



void CvPlayer::DoGreatPersonExpended(UnitTypes eGreatPersonUnit)
{

	int iExpendGold = GetGreatPersonExpendGold();
	if(iExpendGold > 0)
	{
		GetTreasury()->ChangeGold(iExpendGold);

		if(isHuman() && !GC.getGame().isGameMultiPlayer() && GET_PLAYER(GC.getGame().getActivePlayer()).isLocalPlayer())
		{

			const int HALICARNASSUS_ACHIEVEMENT_GOLD = 1000;
			int iHalicarnassus = GC.getInfoTypeForString("BUILDINGCLASS_MAUSOLEUM_HALICARNASSUS");

			if(iHalicarnassus != -1 && getBuildingClassCount((BuildingClassTypes)iHalicarnassus) >= 1)
			{
				BuildingTypes eHalicarnassus = (BuildingTypes)GC.getInfoTypeForString("BUILDING_MAUSOLEUM_HALICARNASSUS");
				CvBuildingEntry* pHalicarnassusInfo = GC.getBuildingInfo(eHalicarnassus);
#ifdef AUI_WARNING_FIXES
				int iHalicarnassusGold = 0;
				if (pHalicarnassusInfo)
					iHalicarnassusGold = pHalicarnassusInfo->GetGreatPersonExpendGold();
#else
				int iHalicarnassusGold = pHalicarnassusInfo->GetGreatPersonExpendGold();
#endif

				int32 iTotalHalicarnassusGold = 0;
				if(gDLL->GetSteamStat(ESTEAMSTAT_HALICARNASSUSGOLDEARNED, &iTotalHalicarnassusGold))
				{
					iTotalHalicarnassusGold += iHalicarnassusGold;
					gDLL->SetSteamStat(ESTEAMSTAT_HALICARNASSUSGOLDEARNED, iTotalHalicarnassusGold);
					if(iTotalHalicarnassusGold >= HALICARNASSUS_ACHIEVEMENT_GOLD)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_HALICARNASSUS_GOLD);
					}
				}
			}
		}
	}


	ReligionTypes eReligionFounded = GetReligions()->GetReligionCreatedByPlayer();
	if(eReligionFounded > RELIGION_PANTHEON)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligionFounded, GetID());
		if(pReligion)
		{
			int iFaith = pReligion->m_Beliefs.GetGreatPersonExpendedFaith();
			if(iFaith > 0)
			{
				iFaith *= GC.getGame().getGameSpeedInfo().getTrainPercent();
				iFaith /= 100;
				ChangeFaith(iFaith);
			}
		}
	}

#ifdef LEKMOD_BUILDING_GP_EXPEND_YIELD


	for(int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); iBuilding++)
	{
	
		BuildingTypes eBuilding = (BuildingTypes)iBuilding;
		if(eBuilding != NO_BUILDING)
		{
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if(pkBuildingInfo)
			{



				int iNumCities = getNumCities();
				for(int iCityLoop = 0; iCityLoop < iNumCities; iCityLoop++)
				{
					CvCity* pCity = getCity(iCityLoop);
					if(!pCity)
					{
						continue;
					}
					if(pCity->GetCityBuildings()->GetNumBuilding(eBuilding) <= 0)
					{
						continue;
					}


					for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
					{
						YieldTypes eYield = (YieldTypes)iYield;

						int iYieldAmount = pkBuildingInfo->GetGreatPersonExpendYield(eYield);



						if (iYieldAmount <= 0)
						{
							continue;
						}


						iYieldAmount *= GC.getGame().getGameSpeedInfo().getTrainPercent();
						iYieldAmount /= 100;

						switch (eYield)
						{
						case YIELD_FOOD:
						{
							pCity->changeFood(iYieldAmount);

							TestMidTurnPopGrowth(pCity, true                           );
						}
						break;
						case YIELD_PRODUCTION:
						{

							if (pCity->isProductionProcess())
							{
								pCity->changeProduction(iYieldAmount);
							}

							else
							{
								pCity->setOverflowProduction(pCity->getOverflowProduction() + iYieldAmount);
							}	
						}
						break;
						case YIELD_GOLD:
						{

							GetTreasury()->ChangeGold(iYieldAmount);
						}
						break;
						case YIELD_SCIENCE:
						{

							TechTypes eCurrentTech = GetPlayerTechs()->GetCurrentResearch();
							if (eCurrentTech == NO_TECH)
							{
								changeOverflowResearch(iYieldAmount);
							}
							else
							{
								GET_TEAM(getTeam()).GetTeamTechs()->ChangeResearchProgress(eCurrentTech, iYieldAmount, GetID());
							}

						}
						break;
						case YIELD_CULTURE:
						{
							changeJONSCulture(iYieldAmount);
						}
						break;
						case YIELD_FAITH:
						{
							ChangeFaith(iYieldAmount);
						}
						break;
						}

					}
					
				}
				
			}
		}
	}

#endif

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());
		args->Push(eGreatPersonUnit);

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "GreatPersonExpended", args.get(), bResult);
	}
}


int CvPlayer::GetGreatPersonExpendGold() const
{
	return m_iGreatPersonExpendGold;
}


void CvPlayer::ChangeGreatPersonExpendGold(int ichange)
{
	m_iGreatPersonExpendGold += ichange;
}


void CvPlayer::recomputeGreatPeopleModifiers()
{



	m_iGreatPeopleRateModifier = 0;
	m_iGreatGeneralRateModifier = 0;
	m_iGreatAdmiralRateModifier = 0;
	m_iGreatWriterRateModifier = 0;
	m_iGreatArtistRateModifier = 0;
	m_iGreatMusicianRateModifier = 0;
	m_iGreatMerchantRateModifier = 0;
	m_iGreatScientistRateModifier = 0;
	m_iGreatEngineerRateModifier = 0;
	m_iDomesticGreatGeneralRateModifier = 0;


	m_iGreatPeopleRateModifier += m_pTraits->GetGreatPeopleRateModifier();
	m_iGreatGeneralRateModifier += m_pTraits->GetGreatGeneralRateModifier();
	m_iGreatScientistRateModifier += m_pTraits->GetGreatScientistRateModifier();
#if defined(TRAITIFY)
	m_iGreatEngineerRateModifier += m_pTraits->GetGreatEngineerRateModifier();
	m_iGreatMerchantRateModifier += m_pTraits->GetGreatMerchantRateModifier();
#endif


	m_iGreatPeopleRateModifier += m_pPlayerPolicies->GetNumericModifier(POLICYMOD_GREAT_PERSON_RATE);
	m_iGreatGeneralRateModifier += m_pPlayerPolicies->GetNumericModifier(POLICYMOD_GREAT_GENERAL_RATE);
	m_iGreatAdmiralRateModifier += m_pPlayerPolicies->GetNumericModifier(POLICYMOD_GREAT_ADMIRAL_RATE);
	m_iGreatWriterRateModifier += m_pPlayerPolicies->GetNumericModifier(POLICYMOD_GREAT_WRITER_RATE);
	m_iGreatArtistRateModifier += m_pPlayerPolicies->GetNumericModifier(POLICYMOD_GREAT_ARTIST_RATE);
	m_iGreatMusicianRateModifier += m_pPlayerPolicies->GetNumericModifier(POLICYMOD_GREAT_MUSICIAN_RATE);
	m_iGreatMerchantRateModifier += m_pPlayerPolicies->GetNumericModifier(POLICYMOD_GREAT_MERCHANT_RATE);
	m_iGreatScientistRateModifier += m_pPlayerPolicies->GetNumericModifier(POLICYMOD_GREAT_SCIENTIST_RATE);
	m_iDomesticGreatGeneralRateModifier += m_pPlayerPolicies->GetNumericModifier(POLICYMOD_DOMESTIC_GREAT_GENERAL_RATE);


	m_iGreatPeopleRateModifier += m_iGreatPeopleRateModFromBldgs;
	m_iGreatGeneralRateModifier += m_iGreatGeneralRateModFromBldgs;
	m_iDomesticGreatGeneralRateModifier += m_iDomesticGreatGeneralRateModFromBldgs;


	m_iGreatPeopleRateModifier += GetGreatPeopleRateModFromFriendships();


	int iArtsyMod = GC.getGame().GetGameLeagues()->GetArtsyGreatPersonRateModifier(GetID());
	int iScienceyMod = GC.getGame().GetGameLeagues()->GetScienceyGreatPersonRateModifier(GetID());
	if (iArtsyMod != 0)
	{
		m_iGreatWriterRateModifier += iArtsyMod;
		m_iGreatArtistRateModifier += iArtsyMod;
		m_iGreatMusicianRateModifier += iArtsyMod;
	}
	if (iScienceyMod != 0)
	{
		m_iGreatScientistRateModifier += iScienceyMod;
		m_iGreatEngineerRateModifier += iScienceyMod;
		m_iGreatMerchantRateModifier += iScienceyMod;
	}


	m_iDomesticGreatGeneralRateModifier += GC.getCOMBAT_EXPERIENCE_IN_BORDERS_PERCENT();
}



int CvPlayer::GetGreatPeopleRateModFromFriendships() const
{
	int iRtnValue = 0;
	int iTraitMod = GetPlayerTraits()->GetDOFGreatPersonModifier();


	if(iTraitMod > 0)
	{
		iRtnValue = GetDiplomacyAI()->GetNumDoF() * iTraitMod;
	}


	else
	{
		PlayerTypes eLoopPlayer;
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if(GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
			{
				if(GetDiplomacyAI()->IsDoFAccepted(eLoopPlayer))
				{
					int iOthersTraitMod = GET_PLAYER(eLoopPlayer).GetPlayerTraits()->GetDOFGreatPersonModifier();
					if(iOthersTraitMod > 0)
					{
						iRtnValue += iOthersTraitMod;
					}
				}
			}
		}
	}

	return iRtnValue;
}



int CvPlayer::GetGreatScientistBeakerMod() const
{
	return m_iGreatScientistBeakerModifier;
}



void CvPlayer::SetGreatScientistBeakerMod(int iValue)
{
	m_iGreatScientistBeakerModifier = iValue;
}



void CvPlayer::ChangeGreatScientistBeakerMod(int iChange)
{
	SetGreatScientistBeakerMod(GetGreatScientistBeakerMod() + iChange);
}


int CvPlayer::GetGreatGeneralCombatBonus() const
{
	return m_iGreatGeneralCombatBonus;
}


void CvPlayer::SetGreatGeneralCombatBonus(int iValue)
{
	m_iGreatGeneralCombatBonus = iValue;
}
#if defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)

void CvPlayer::ChangeGreatGeneralCombatBonus(int iChange)
{
	SetGreatGeneralCombatBonus(GetGreatGeneralCombatBonus() + iChange);
}
#endif






void CvPlayer::DoSeedGreatPeopleSpawnCounter()
{
	int iNumTurns =        GC.getMINOR_TURNS_GREAT_PEOPLE_SPAWN_BASE();


	int iExtraAllies = -1;

	PlayerTypes eMinor;
	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		eMinor = (PlayerTypes) iMinorLoop;


		if(!GET_PLAYER(eMinor).isAlive())
			continue;

		if(GET_PLAYER(eMinor).GetMinorCivAI()->GetAlly() == GetID())
			iExtraAllies++;
	}

	if(iExtraAllies > 0)
	{
		int iExtraAlliesChange = iExtraAllies*        GC.getMINOR_ADDITIONAL_ALLIES_GP_CHANGE();

		iExtraAlliesChange = max(        GC.getMAX_MINOR_ADDITIONAL_ALLIES_GP_CHANGE(), iExtraAlliesChange);

		iNumTurns += iExtraAlliesChange;
	}

	int iRand =       GC.getMINOR_TURNS_GREAT_PEOPLE_SPAWN_RAND();
	iNumTurns += GC.getGame().getJonRandNum(iRand, "Rand turns for Friendly Minor GreatPeople spawn");


	if(!IsAlliesGreatPersonBiasApplied())
	{
		iNumTurns *=        GC.getMINOR_TURNS_GREAT_PEOPLE_SPAWN_BIAS_MULTIPLY();
		iNumTurns /= 100;

		SetAlliesGreatPersonBiasApplied(true);
	}


	iNumTurns *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iNumTurns /= 100;

	if(iNumTurns < 1)
		iNumTurns = 1;

	SetGreatPeopleSpawnCounter(iNumTurns);
}



void CvPlayer::DoApplyNewAllyGPBonus()
{
	int iChange =        GC.getMINOR_ADDITIONAL_ALLIES_GP_CHANGE();
	ChangeGreatPeopleSpawnCounter(iChange);

	if(GetGreatPeopleSpawnCounter() < 1)
		SetGreatPeopleSpawnCounter(1);
}



int CvPlayer::GetGreatPeopleSpawnCounter()
{
	return m_iGreatPeopleSpawnCounter;
}



void CvPlayer::SetGreatPeopleSpawnCounter(int iValue)
{
	m_iGreatPeopleSpawnCounter = iValue;
}



void CvPlayer::ChangeGreatPeopleSpawnCounter(int iChange)
{
	SetGreatPeopleSpawnCounter(GetGreatPeopleSpawnCounter() + iChange);
}



void CvPlayer::DoSpawnGreatPerson(PlayerTypes eMinor)
{
	CvAssertMsg(eMinor >= MAX_MAJOR_CIVS, "eMinor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "eMinor is expected to be within maximum bounds (invalid Index)");


	CvCity* pMinorCapital = GET_PLAYER(eMinor).getCapitalCity();
	if(pMinorCapital == NULL)
	{
		FAssertMsg(false, "MINOR CIV AI: Trying to spawn a GreatPeople for a major civ but the minor has no capital. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
		return;
	}

	CvPlot* pMinorPlot = pMinorCapital->plot();
	if(pMinorPlot == NULL)
	{
		CvAssertMsg(false, "Plot for minor civ's capital not found! Please send Anton your save file and version.");
		return;
	}



	CvCity* pMajorCity = GetClosestFriendlyCity(*pMinorPlot, MAX_INT);

	int iX = pMinorCapital->getX();
	int iY = pMinorCapital->getY();
	if(pMajorCity != NULL)
	{
		iX = pMajorCity->getX();
		iY = pMajorCity->getY();
	}


	UnitTypes eBestUnit = NO_UNIT;
	int iBestScore = -1;
	SpecialUnitTypes eSpecialUnitGreatPerson = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_PEOPLE");

#ifdef AUI_WARNING_FIXES
	for (uint iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
#else
	for(int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
#endif
	{
		UnitTypes eLoopUnit = (UnitTypes)iUnitLoop;
		CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eLoopUnit);

		if(pkUnitEntry && pkUnitEntry->GetSpecialUnitType() == eSpecialUnitGreatPerson)
		{

#ifdef NQ_PATRONAGE_GREAT_PEOPLE_FIX
			if(eLoopUnit == GC.getInfoTypeForString("UNIT_WRITER") ||
				eLoopUnit == GC.getInfoTypeForString("UNIT_ARTIST") ||
				eLoopUnit == GC.getInfoTypeForString("UNIT_MUSICIAN") ||
				eLoopUnit == GC.getInfoTypeForString("UNIT_MERCHANT") ||
				eLoopUnit == GC.getInfoTypeForString("UNIT_ENGINEER") ||
				eLoopUnit == GC.getInfoTypeForString("UNIT_SCIENTIST") ||
				eLoopUnit == GC.getInfoTypeForString("UNIT_GREAT_GENERAL"))
#else
			if(!pkUnitEntry->IsFoundReligion())
#endif
			{
				int iScore = GC.getGame().getJonRandNum(100, "Rand");

				if(iScore > iBestScore)
				{
					iBestScore = iScore;
					eBestUnit = eLoopUnit;
				}
			}
		}
	}


	if(eBestUnit != NO_UNIT)
	{
		CvUnit* pNewGreatPeople = initUnit(eBestUnit, iX, iY);
		CvAssert(pNewGreatPeople);

		if (pNewGreatPeople)
		{
#ifdef NQ_WAR_HERO
			if(pNewGreatPeople->IsGreatGeneral() && IsWarHero())
			{
				addFreeUnit((UnitTypes)GC.getInfoTypeForString("UNIT_ARTIST"));
			}
#endif
			if (pNewGreatPeople->IsGreatAdmiral())
			{
				CvPlot* pSpawnPlot = GetGreatAdmiralSpawnPlot(pNewGreatPeople);
				if (pNewGreatPeople->plot() != pSpawnPlot && pSpawnPlot != NULL)
				{
					pNewGreatPeople->setXY(pSpawnPlot->getX(), pSpawnPlot->getY());
				}
			}
			else
			{
				if (!pNewGreatPeople->jumpToNearestValidPlot())
					pNewGreatPeople->kill(false);
			}

			CvNotifications* pNotifications = GetNotifications();
			if(pNotifications)
			{
				Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_STATE_UNIT_SPAWN");
				strMessage << GET_PLAYER(eMinor).getNameKey();
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_STATE_UNIT_SPAWN");
				strSummary << GET_PLAYER(eMinor).getNameKey();
				pNotifications->Add(NOTIFICATION_MINOR, strMessage.toUTF8(), strSummary.toUTF8(), iX, iY, eMinor);
			}
		}
	}
}



void CvPlayer::DoGreatPeopleSpawnTurn()
{

	if(GetGreatPeopleSpawnCounter() > 0)
	{
		AI_PERF_FORMAT("AI-perf.csv", ("CvPlayer::DoGreatPeopleSpawnTurn, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), getCivilizationShortDescription()) );
		ChangeGreatPeopleSpawnCounter(-1);


		if(GetGreatPeopleSpawnCounter() == 0)
		{
			PlayerTypes eBestMinor = NO_PLAYER;
			int iBestScore = -1;
			int iScore;

			PlayerTypes eMinor;
			for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
			{
				eMinor = (PlayerTypes) iMinorLoop;


				if(!GET_PLAYER(eMinor).isAlive())
					continue;


				if(GET_PLAYER(eMinor).GetMinorCivAI()->GetAlly() != GetID())
					continue;

				iScore = GC.getGame().getJonRandNum(100, "Random minor great person gift location.");


				if(eBestMinor == NO_PLAYER || iScore > iBestScore)
				{
					eBestMinor = eMinor;
					iBestScore = iScore;
				}
			}

			if(eBestMinor != NO_PLAYER)
				DoSpawnGreatPerson(eBestMinor);


			DoSeedGreatPeopleSpawnCounter();
		}
	}
}


CvCity* CvPlayer::GetGreatPersonSpawnCity(UnitTypes eUnit)
{
	CvCity* pBestCity = getCapitalCity();
	CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eUnit);

	if(pkUnitEntry && pkUnitEntry->GetDomainType() == DOMAIN_SEA)
	{
		int iBestValue = INT_MAX;
		int iLoop;
		for(CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			if(!pLoopCity->isCoastal())
			{
				continue;
			}

			int iValue = 4 * GC.getGame().getJonRandNum(getNumCities(), "Great Admiral City Selection");

			for(int i = 0; i < NUM_YIELD_TYPES; i++)
			{
				iValue += pLoopCity->findYieldRateRank((YieldTypes)i);
			}
			iValue += pLoopCity->findPopulationRank();

			if(iValue < iBestValue)
			{
				pBestCity = pLoopCity;
				iBestValue = iValue;
			}
		}
	}

	return pBestCity;
}


int CvPlayer::getMaxGlobalBuildingProductionModifier() const
{
	return m_iMaxGlobalBuildingProductionModifier;
}



void CvPlayer::changeMaxGlobalBuildingProductionModifier(int iChange)
{
	m_iMaxGlobalBuildingProductionModifier = (m_iMaxGlobalBuildingProductionModifier + iChange);
}



int CvPlayer::getMaxTeamBuildingProductionModifier() const
{
	return m_iMaxTeamBuildingProductionModifier;
}



void CvPlayer::changeMaxTeamBuildingProductionModifier(int iChange)
{
	m_iMaxTeamBuildingProductionModifier = (m_iMaxTeamBuildingProductionModifier + iChange);
}



int CvPlayer::getMaxPlayerBuildingProductionModifier() const
{
	return m_iMaxPlayerBuildingProductionModifier;
}



void CvPlayer::changeMaxPlayerBuildingProductionModifier(int iChange)
{
	m_iMaxPlayerBuildingProductionModifier = (m_iMaxPlayerBuildingProductionModifier + iChange);
}



int CvPlayer::getFreeExperience() const
{
	return m_iFreeExperience;
}


void CvPlayer::changeFreeExperienceFromBldgs(int iChange)
{
	m_iFreeExperienceFromBldgs += iChange;
}


void CvPlayer::changeFreeExperienceFromMinors(int iChange)
{
	m_iFreeExperienceFromMinors += iChange;
}


void CvPlayer::recomputeFreeExperience()
{
	m_iFreeExperience = m_iFreeExperienceFromBldgs;
#ifdef AUI_PLAYER_FIX_RECOMPUTE_FREE_EXPERIENCE_GLOBAL_FREE_EXPERIENCE
	m_iFreeExperience += m_iFreeExperienceFromMinors;
#else
	m_iFreeExperience = m_iFreeExperienceFromMinors;
#endif
	m_iFreeExperience += m_pPlayerPolicies->GetNumericModifier(POLICYMOD_FREE_EXPERIENCE);
}



int CvPlayer::getFeatureProductionModifier() const
{
	return m_iFeatureProductionModifier;
}



void CvPlayer::changeFeatureProductionModifier(int iChange)
{
	m_iFeatureProductionModifier = (m_iFeatureProductionModifier + iChange);
}



int CvPlayer::getWorkerSpeedModifier() const
{
	return m_iWorkerSpeedModifier;
}



void CvPlayer::changeWorkerSpeedModifier(int iChange)
{
	m_iWorkerSpeedModifier = (m_iWorkerSpeedModifier + iChange);
}



int CvPlayer::getImprovementCostModifier() const
{
	return m_iImprovementCostModifier;
}



void CvPlayer::changeImprovementCostModifier(int iChange)
{
	m_iImprovementCostModifier = (m_iImprovementCostModifier + iChange);
}



int CvPlayer::getImprovementUpgradeRateModifier() const
{
	return m_iImprovementUpgradeRateModifier;
}



void CvPlayer::changeImprovementUpgradeRateModifier(int iChange)
{
	m_iImprovementUpgradeRateModifier = (m_iImprovementUpgradeRateModifier + iChange);
}



int CvPlayer::getSpecialistProductionModifier() const
{
	return m_iSpecialistProductionModifier;
}



void CvPlayer::changeSpecialistProductionModifier(int iChange)
{
	m_iSpecialistProductionModifier = (m_iSpecialistProductionModifier + iChange);
}



int CvPlayer::getMilitaryProductionModifier() const
{
	return m_iMilitaryProductionModifier;
}



void CvPlayer::changeMilitaryProductionModifier(int iChange)
{
	m_iMilitaryProductionModifier = (m_iMilitaryProductionModifier + iChange);
}



int CvPlayer::getSpaceProductionModifier() const
{
	return m_iSpaceProductionModifier;
}



void CvPlayer::changeSpaceProductionModifier(int iChange)
{
	m_iSpaceProductionModifier = (m_iSpaceProductionModifier + iChange);
}



int CvPlayer::getCityDefenseModifier() const
{
	return m_iCityDefenseModifier;
}


void CvPlayer::changeCityDefenseModifier(int iChange)
{
	m_iCityDefenseModifier = (m_iCityDefenseModifier + iChange);
}



int CvPlayer::getUnitFortificationModifier() const
{
	return m_iUnitFortificationModifier;
}


void CvPlayer::changeUnitFortificationModifier(int iChange)
{
	m_iUnitFortificationModifier = (m_iUnitFortificationModifier + iChange);
}



int CvPlayer::getUnitBaseHealModifier() const
{
	return m_iUnitBaseHealModifier;
}


void CvPlayer::changeUnitBaseHealModifier(int iChange)
{
	m_iUnitBaseHealModifier = (m_iUnitBaseHealModifier + iChange);
}


int CvPlayer::getWonderProductionModifier() const
{
	return m_iWonderProductionModifier;
}


void CvPlayer::changeWonderProductionModifier(int iChange)
{
	m_iWonderProductionModifier = (m_iWonderProductionModifier + iChange);
}



int CvPlayer::getSettlerProductionModifier() const
{
	return m_iSettlerProductionModifier;
}


void CvPlayer::changeSettlerProductionModifier(int iChange)
{
	m_iSettlerProductionModifier = (m_iSettlerProductionModifier + iChange);
}



int CvPlayer::getCapitalSettlerProductionModifier() const
{
	return m_iCapitalSettlerProductionModifier;
}


void CvPlayer::changeCapitalSettlerProductionModifier(int iChange)
{
	m_iCapitalSettlerProductionModifier = (m_iCapitalSettlerProductionModifier + iChange);
}



int CvPlayer::GetPolicyCostBuildingModifier() const
{
	return m_iPolicyCostBuildingModifier;
}


void CvPlayer::ChangePolicyCostBuildingModifier(int iChange)
{
	if(iChange != 0)
	{
		m_iPolicyCostBuildingModifier = (m_iPolicyCostBuildingModifier + iChange);

		recomputePolicyCostModifier();
		DoUpdateNextPolicyCost();
	}
}


int CvPlayer::GetPolicyCostMinorCivModifier() const
{
	return m_iPolicyCostMinorCivModifier;
}


void CvPlayer::ChangePolicyCostMinorCivModifier(int iChange)
{
	m_iPolicyCostMinorCivModifier = (m_iPolicyCostMinorCivModifier + iChange);
}
#if !defined(LEK_YIELD_TOURISM)

int CvPlayer::GetInfluenceSpreadModifier() const
{
	return m_iInfluenceSpreadModifier;
}


void CvPlayer::ChangeInfluenceSpreadModifier(int iChange)
{
	m_iInfluenceSpreadModifier = (m_iInfluenceSpreadModifier + iChange);
}
#endif

int CvPlayer::GetExtraVotesPerDiplomat() const
{
	return m_iExtraVotesPerDiplomat;
}


void CvPlayer::ChangeExtraVotesPerDiplomat(int iChange)
{
	m_iExtraVotesPerDiplomat += iChange;
}


int CvPlayer::getNumNukeUnits() const
{
	return m_iNumNukeUnits;
}



void CvPlayer::changeNumNukeUnits(int iChange)
{
	m_iNumNukeUnits = (m_iNumNukeUnits + iChange);
	CvAssert(getNumNukeUnits() >= 0);
}



int CvPlayer::getNumOutsideUnits()
{
	int iOutsideUnitCount = 0;

	CvUnit* pLoopUnit;
	int iLoop;
	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if(pLoopUnit->plot() != NULL)
		{
			if(pLoopUnit->plot()->getOwner() != pLoopUnit->getOwner())
			{
				iOutsideUnitCount++;
			}
		}
	}

	return iOutsideUnitCount;
}



void CvPlayer::changeNumOutsideUnits(int iChange)
{
	if(iChange != 0)
	{
		m_iNumOutsideUnits += iChange;
		CvAssert(getNumOutsideUnits() >= 0);

		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getBaseFreeUnits() const
{
	return m_iBaseFreeUnits;
}



void CvPlayer::changeBaseFreeUnits(int iChange)
{
	if(iChange != 0)
	{
		m_iBaseFreeUnits = (m_iBaseFreeUnits + iChange);

		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::GetVerifiedOutsideUnitCount()
{
	int iOutsideUnitCount = 0;

	CvUnit* pLoopUnit;
	int iLoop;
	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if(pLoopUnit->plot() != NULL)
		{
			if(pLoopUnit->plot()->getOwner() != pLoopUnit->getOwner())
			{
				iOutsideUnitCount++;
			}
		}
	}

	return iOutsideUnitCount;
}



int CvPlayer::getGoldPerUnit() const
{
	return getGoldPerUnitTimes100() / 100;
}



void CvPlayer::changeGoldPerUnit(int iChange)
{
	if(iChange != 0)
	{
		changeGoldPerUnitTimes100(iChange * 100);
	}
}


int CvPlayer::getGoldPerUnitTimes100() const
{
	return m_iGoldPerUnit;
}



void CvPlayer::changeGoldPerUnitTimes100(int iChange)
{
	if(iChange != 0)
	{
		m_iGoldPerUnit = (m_iGoldPerUnit + iChange);

		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}



int CvPlayer::getGoldPerMilitaryUnit() const
{
	return m_iGoldPerMilitaryUnit;
}



void CvPlayer::changeGoldPerMilitaryUnit(int iChange)
{
	if(iChange != 0)
	{
		m_iGoldPerMilitaryUnit = (m_iGoldPerMilitaryUnit + iChange);

		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::GetRouteGoldMaintenanceMod() const
{
	return m_iRouteGoldMaintenanceMod;
}


void CvPlayer::ChangeRouteGoldMaintenanceMod(int iChange)
{
	if(iChange != 0)
	{
		m_iRouteGoldMaintenanceMod = (m_iRouteGoldMaintenanceMod + iChange);
	}
}


int CvPlayer::GetBuildingGoldMaintenanceMod() const
{
	return m_iBuildingGoldMaintenanceMod;
}


void CvPlayer::ChangeBuildingGoldMaintenanceMod(int iChange)
{
	if(iChange != 0)
	{
		m_iBuildingGoldMaintenanceMod = (m_iBuildingGoldMaintenanceMod + iChange);
	}
}


int CvPlayer::GetUnitGoldMaintenanceMod() const
{
	return m_iUnitGoldMaintenanceMod;
}


void CvPlayer::ChangeUnitGoldMaintenanceMod(int iChange)
{
	if(iChange != 0)
	{
		m_iUnitGoldMaintenanceMod = (m_iUnitGoldMaintenanceMod + iChange);
	}
}


int CvPlayer::GetUnitSupplyMod() const
{
	return m_iUnitSupplyMod;
}



void CvPlayer::ChangeUnitSupplyMod(int iChange)
{
	if(iChange != 0)
	{
		m_iUnitSupplyMod = (m_iUnitSupplyMod + iChange);
	}
}



int CvPlayer::getExtraUnitCost() const
{
	return m_iExtraUnitCost;
}



void CvPlayer::changeExtraUnitCost(int iChange)
{
	if(iChange != 0)
	{
		m_iExtraUnitCost = (m_iExtraUnitCost + iChange);

		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::GetNumMaintenanceFreeUnits(DomainTypes eDomain, bool bOnlyCombatUnits) const
{
	int iNumFreeUnits = 0;


	const CvUnit* pLoopUnit;
	int iLoop;
	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if (eDomain != NO_DOMAIN)
		{
			if (pLoopUnit->getDomainType() != eDomain)
			{
				continue;
			}
		}

		if (bOnlyCombatUnits)
		{
			if (!pLoopUnit->IsCombatUnit())
			{
				continue;
			}
		}

		if(pLoopUnit->getUnitInfo().IsNoMaintenance())
		{
			iNumFreeUnits++;
		}
		else if(IsGarrisonFreeMaintenance() && pLoopUnit->IsGarrisoned())
		{
			iNumFreeUnits++;
		}
#if defined(LEKMOD_GARRISON_YIELD_EFFECTS)
		else if (pLoopUnit->IsGarrisoned())
		{
			CvCity* pGarrisonedCity = pLoopUnit->plot()->getPlotCity();
			if (pGarrisonedCity != NULL)
			{
				if (pGarrisonedCity->GetCityBuildings()->IsGarrisonMaintenanceFree())
				{
					iNumFreeUnits++;
				}
			}
		}
#endif
	}

	return iNumFreeUnits;
}


int CvPlayer::getNumMilitaryUnits() const
{
	return m_iNumMilitaryUnits;
}



void CvPlayer::changeNumMilitaryUnits(int iChange)
{
	if(iChange != 0)
	{
		m_iNumMilitaryUnits = (m_iNumMilitaryUnits + iChange);
		CvAssert(getNumMilitaryUnits() >= 0);

		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}



int CvPlayer::getHappyPerMilitaryUnit() const
{
	return m_iHappyPerMilitaryUnit;
}



void CvPlayer::changeHappyPerMilitaryUnit(int iChange)
{
	if(iChange != 0)
	{
		m_iHappyPerMilitaryUnit = (m_iHappyPerMilitaryUnit + iChange);
	}
}


int CvPlayer::getHappinessToCulture() const
{
	return m_iHappinessToCulture;
}


void CvPlayer::changeHappinessToCulture(int iChange)
{
	if(iChange != 0)
	{
		m_iHappinessToCulture = (m_iHappinessToCulture + iChange);
		CvAssert(getHappinessToCulture() >= 0);
	}
}


int CvPlayer::getHappinessToScience() const
{
	return m_iHappinessToScience;
}


void CvPlayer::changeHappinessToScience(int iChange)
{
	if(iChange != 0)
	{
		m_iHappinessToScience = (m_iHappinessToScience + iChange);
		CvAssert(getHappinessToScience() >= 0);
	}
}

#ifdef NQ_GOLD_TO_SCIENCE_FROM_POLICIES

int CvPlayer::getGoldToScience() const
{
	return m_iGoldToScience;
}


void CvPlayer::changeGoldToScience(int iChange)
{
	if(iChange != 0)
	{
		m_iGoldToScience = (m_iGoldToScience + iChange);
		CvAssert(getGoldToScience() >= 0);
	}
}
#endif


int CvPlayer::getHalfSpecialistUnhappinessCount() const
{
	return m_iHalfSpecialistUnhappinessCount;
}

int CvPlayer::getHalfMoreSpecialistUnhappinessCount() const
{
	return m_iHalfMoreSpecialistUnhappinessCount;
}




bool CvPlayer::isHalfSpecialistUnhappiness() const
{
	return (getHalfSpecialistUnhappinessCount() > 0);
}

bool CvPlayer::isHalfMoreSpecialistUnhappiness() const
{
	return (getHalfMoreSpecialistUnhappinessCount() > 0);
}


void CvPlayer::changeHalfSpecialistUnhappinessCount(int iChange)
{
	if(iChange != 0)
	{
		m_iHalfSpecialistUnhappinessCount = (m_iHalfSpecialistUnhappinessCount + iChange);
		CvAssert(getHalfSpecialistUnhappinessCount() >= 0);
	}
}

void CvPlayer::changeHalfMoreSpecialistUnhappinessCount(int iChange)
{
	if(iChange != 0)
	{
		m_iHalfMoreSpecialistUnhappinessCount = (m_iHalfMoreSpecialistUnhappinessCount + iChange);
		CvAssert(getHalfMoreSpecialistUnhappinessCount() >= 0);
	}
}


int CvPlayer::getHalfSpecialistFoodCount() const
{
	return m_iHalfSpecialistFoodCount;
}



bool CvPlayer::isHalfSpecialistFood() const
{
	return (getHalfSpecialistFoodCount() > 0);
}



void CvPlayer::changeHalfSpecialistFoodCount(int iChange)
{
	if(iChange != 0)
	{
		m_iHalfSpecialistFoodCount = (m_iHalfSpecialistFoodCount + iChange);
		CvAssert(getHalfSpecialistFoodCount() >= 0);
	}
}



int CvPlayer::getMilitaryFoodProductionCount() const
{
	return m_iMilitaryFoodProductionCount;
}



bool CvPlayer::isMilitaryFoodProduction() const
{
	return (getMilitaryFoodProductionCount() > 0);
}



void CvPlayer::changeMilitaryFoodProductionCount(int iChange)
{
	if(iChange != 0)
	{
		m_iMilitaryFoodProductionCount = (m_iMilitaryFoodProductionCount + iChange);
		CvAssert(getMilitaryFoodProductionCount() >= 0);

		if(getTeam() == GC.getGame().getActiveTeam())
		{
			GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::GetGoldenAgeCultureBonusDisabledCount() const
{
	return m_iGoldenAgeCultureBonusDisabledCount;
}


bool CvPlayer::IsGoldenAgeCultureBonusDisabled() const
{
	return (GetGoldenAgeCultureBonusDisabledCount() > 0);
}


void CvPlayer::ChangeGoldenAgeCultureBonusDisabledCount(int iChange)
{
	if (iChange != 0)
	{
		m_iGoldenAgeCultureBonusDisabledCount = m_iGoldenAgeCultureBonusDisabledCount + iChange;
		CvAssert(GetGoldenAgeCultureBonusDisabledCount() >= 0);
	}
}


int CvPlayer::GetSecondReligionPantheonCount() const
{
	return m_iSecondReligionPantheonCount;
}


bool CvPlayer::IsSecondReligionPantheon() const
{
	return (GetSecondReligionPantheonCount() > 0);
}


void CvPlayer::ChangeSecondReligionPantheonCount(int iChange)
{
	if (iChange != 0)
	{
		m_iSecondReligionPantheonCount = m_iSecondReligionPantheonCount + iChange;
		CvAssert(GetSecondReligionPantheonCount() >= 0);
	}
}


int CvPlayer::GetEnablesSSPartHurryCount() const
{
	return m_iEnablesSSPartHurryCount;
}


bool CvPlayer::IsEnablesSSPartHurry() const
{
	return (GetEnablesSSPartHurryCount() > 0);
}


void CvPlayer::ChangeEnablesSSPartHurryCount(int iChange)
{
	if (iChange != 0)
	{
		m_iEnablesSSPartHurryCount = m_iEnablesSSPartHurryCount + iChange;
		CvAssert(GetEnablesSSPartHurryCount() >= 0);
	}
}

#ifdef NQ_DIABLE_RESISTANCE_TIME_VIA_POLICIES

int CvPlayer::GetDisablesResistanceTimeCount() const
{
	return m_iDisablesResistanceTimeCount;
}


bool CvPlayer::IsDisablesResistanceTime() const
{
	return (GetDisablesResistanceTimeCount() > 0);
}


void CvPlayer::ChangeDisablesResistanceTimeCount(int iChange)
{
	if (iChange != 0)
	{
		m_iDisablesResistanceTimeCount = m_iDisablesResistanceTimeCount + iChange;
		CvAssert(GetDisablesResistanceTimeCount() >= 0);
	}
}
#endif

#ifdef NQ_PATRIOTIC_WAR

int CvPlayer::GetDoubleTrainedMilitaryLandUnitCount() const
{
	return m_iDoubleTrainedMilitaryLandUnitCount;
}


bool CvPlayer::IsDoubleTrainedMilitaryLandUnit() const
{
	return (GetDoubleTrainedMilitaryLandUnitCount() > 0);
}


void CvPlayer::ChangeDoubleTrainedMilitaryLandUnitCount(int iChange)
{
	if (iChange != 0)
	{
		m_iDoubleTrainedMilitaryLandUnitCount = m_iDoubleTrainedMilitaryLandUnitCount + iChange;
		CvAssert(GetDoubleTrainedMilitaryLandUnitCount() >= 0);
	}
}
#endif

#ifdef NQ_WAR_HERO

int CvPlayer::GetWarHeroCount() const
{
	return m_iWarHeroCount;
}


bool CvPlayer::IsWarHero() const
{
	return (GetWarHeroCount() > 0);
}


void CvPlayer::ChangeWarHeroCount(int iChange)
{
	if (iChange != 0)
	{
		m_iWarHeroCount = m_iWarHeroCount + iChange;
		CvAssert(GetWarHeroCount() >= 0);
	}
}
#endif
#ifdef NQ_IGNORE_PUPPETS_FOR_RESEARCH_COSTS_FROM_POLICIES

int CvPlayer::GetIgnorePuppetsForResearchCostsCount() const
{
	return m_iIgnorePuppetsForResearchCostsCount;
}


bool CvPlayer::IsIgnorePuppetsForResearchCosts() const
{
	return (GetIgnorePuppetsForResearchCostsCount() > 0);
}


void CvPlayer::ChangeIgnorePuppetsForResearchCostsCount(int iChange)
{
	if (iChange != 0)
	{
		m_iIgnorePuppetsForResearchCostsCount = m_iIgnorePuppetsForResearchCostsCount + iChange;
		CvAssert(GetIgnorePuppetsForResearchCostsCount() >= 0);
	}
}
#endif

#ifdef NQ_POLICY_TOGGLE_NO_MINOR_DOW_IF_FRIENDS

int CvPlayer::GetNoMinorDOWIfFriendsCount() const
{
	return m_iNoMinorDOWIfFriendsCount;
}


bool CvPlayer::IsNoMinorDOWIfFriends() const
{
	return (GetNoMinorDOWIfFriendsCount() > 0);
}


void CvPlayer::ChangeNoMinorDOWIfFriendsCount(int iChange)
{
	if (iChange != 0)
	{
		m_iNoMinorDOWIfFriendsCount = m_iNoMinorDOWIfFriendsCount + iChange;
		CvAssert(GetNoMinorDOWIfFriendsCount() >= 0);
	}
}
#endif

#ifdef NQ_ALLOW_PUPPET_PURCHASING_FROM_POLICIES

int CvPlayer::GetAllowPuppetPurchasingCount() const
{
	return m_iAllowPuppetPurchasingCount;
}


bool CvPlayer::IsAllowPuppetPurchasing() const
{
	return (GetAllowPuppetPurchasingCount() > 0);
}


void CvPlayer::ChangeAllowPuppetPurchasingCount(int iChange)
{
	if (iChange != 0)
	{
		m_iAllowPuppetPurchasingCount = m_iAllowPuppetPurchasingCount + iChange;
		CvAssert(GetAllowPuppetPurchasingCount() >= 0);
	}
}
#endif


int CvPlayer::GetEnablesSSPartPurchaseCount() const
{
	return m_iEnablesSSPartPurchaseCount;
}


bool CvPlayer::IsEnablesSSPartPurchase() const
{
	return (GetEnablesSSPartPurchaseCount() > 0);
}


void CvPlayer::ChangeEnablesSSPartPurchaseCount(int iChange)
{
	if (iChange != 0)
	{
		m_iEnablesSSPartPurchaseCount = m_iEnablesSSPartPurchaseCount + iChange;
		CvAssert(GetEnablesSSPartPurchaseCount() >= 0);
	}
}


int CvPlayer::getHighestUnitLevel()	const
{
	return m_iHighestUnitLevel;
}



void CvPlayer::setHighestUnitLevel(int iNewValue)
{
	m_iHighestUnitLevel = iNewValue;
	CvAssert(getHighestUnitLevel() >= 0);
}



int CvPlayer::getMaxConscript() const
{
	return m_iMaxConscript;
}



void CvPlayer::changeMaxConscript(int iChange)
{
	m_iMaxConscript = (m_iMaxConscript + iChange);
	CvAssert(getMaxConscript() >= 0);
}



int CvPlayer::getConscriptCount() const
{
	return m_iConscriptCount;
}



void CvPlayer::setConscriptCount(int iNewValue)
{
	m_iConscriptCount = iNewValue;
	CvAssert(getConscriptCount() >= 0);
}



void CvPlayer::changeConscriptCount(int iChange)
{
	setConscriptCount(getConscriptCount() + iChange);
}



int CvPlayer::getOverflowResearch() const
{
	return m_iOverflowResearch / 100;
}



void CvPlayer::setOverflowResearch(int iNewValue)
{
	setOverflowResearchTimes100(iNewValue*100);
}



void CvPlayer::changeOverflowResearch(int iChange)
{
	changeOverflowResearchTimes100(iChange*100);
}



int CvPlayer::getOverflowResearchTimes100() const
{
	return m_iOverflowResearch;
}



void CvPlayer::setOverflowResearchTimes100(int iNewValue)
{
	m_iOverflowResearch = iNewValue;
	CvAssert(getOverflowResearchTimes100() >= 0);
}



void CvPlayer::changeOverflowResearchTimes100(int iChange)
{
	setOverflowResearchTimes100(getOverflowResearchTimes100() + iChange);
}


int CvPlayer::getExpModifier() const
{
	return m_iExpModifier;
}



void CvPlayer::changeExpModifier(int iChange)
{
	if(iChange != 0)
	{
		m_iExpModifier += iChange;
		CvAssert(getExpModifier() >= 0);
	}
}


int CvPlayer::getExpInBorderModifier() const
{
	return m_iExpInBorderModifier;
}



void CvPlayer::changeExpInBorderModifier(int iChange)
{
	if(iChange != 0)
	{
		m_iExpInBorderModifier += iChange;
		CvAssert(getExpInBorderModifier() >= 0);
	}
}



int CvPlayer::getLevelExperienceModifier() const
{
	return m_iLevelExperienceModifier;
}



void CvPlayer::changeLevelExperienceModifier(int iChange)
{
	m_iLevelExperienceModifier += iChange;
}


int CvPlayer::getMinorQuestFriendshipMod() const
{
	return m_iMinorQuestFriendshipMod;
}



void CvPlayer::changeMinorQuestFriendshipMod(int iChange)
{
	if(iChange != 0)
	{
		m_iMinorQuestFriendshipMod += iChange;
		CvAssert(getMinorQuestFriendshipMod() >= 0);
	}
}


int CvPlayer::getMinorGoldFriendshipMod() const
{
	return m_iMinorGoldFriendshipMod;
}



void CvPlayer::changeMinorGoldFriendshipMod(int iChange)
{
	if(iChange != 0)
	{
		m_iMinorGoldFriendshipMod += iChange;
		CvAssert(getMinorGoldFriendshipMod() >= 0);
	}
}




int CvPlayer::GetMinorFriendshipAnchorMod() const
{
	return m_iMinorFriendshipMinimum;
}


void CvPlayer::SetMinorFriendshipAnchorMod(int iValue)
{
	if (iValue < GC.getMINOR_FRIENDSHIP_AT_WAR())
	{
		CvAssertMsg(false, "Minor friendship anchor mod should not be lower than the War friendship level. Please send Anton your save file and version.");
		m_iMinorFriendshipMinimum = GC.getMINOR_FRIENDSHIP_AT_WAR();
		return;
	}

	m_iMinorFriendshipMinimum = iValue;
}


void CvPlayer::ChangeMinorFriendshipAnchorMod(int iChange)
{
	SetMinorFriendshipAnchorMod(GetMinorFriendshipAnchorMod() + iChange);
}



int CvPlayer::GetMinorFriendshipDecayMod() const
{
	return m_iMinorFriendshipDecayMod;
}



void CvPlayer::changeGetMinorFriendshipDecayMod(int iChange)
{
	if(iChange != 0)
	{
		m_iMinorFriendshipDecayMod += iChange;
	}
}


bool CvPlayer::IsMinorScienceAllies() const
{
	return GetMinorScienceAlliesCount() > 0;
}


int CvPlayer::GetMinorScienceAlliesCount() const
{
	return m_iMinorScienceAlliesCount;
}


void CvPlayer::ChangeMinorScienceAlliesCount(int iChange)
{
	if(iChange != 0)
	{
		m_iMinorScienceAlliesCount += iChange;
	}
}


bool CvPlayer::IsMinorResourceBonus() const
{
	return GetMinorResourceBonusCount() > 0;
}


int CvPlayer::GetMinorResourceBonusCount() const
{
	return m_iMinorResourceBonusCount;
}


void CvPlayer::ChangeMinorResourceBonusCount(int iChange)
{
	if(iChange != 0)
	{
		m_iMinorResourceBonusCount += iChange;
	}
}


bool CvPlayer::IsAbleToAnnexCityStates() const
{
	if (GetAbleToAnnexCityStatesCount() > 0)
		return true;

	if (GetPlayerTraits()->IsAbleToAnnexCityStates())
		return true;

	return false;
}

#ifdef NQ_NUM_TURNS_BEFORE_MINOR_ALLIES_REFUSE_BRIBES_FROM_TRAIT

int CvPlayer::GetNumTurnsBeforeMinorAlliesRefuseBribes() const
{
	return GetPlayerTraits()->GetNumTurnsBeforeMinorAlliesRefuseBribes();
}
#endif


int CvPlayer::GetAbleToAnnexCityStatesCount() const
{
	return m_iAbleToAnnexCityStatesCount;
}


void CvPlayer::ChangeAbleToAnnexCityStatesCount(int iChange)
{
	m_iAbleToAnnexCityStatesCount += iChange;
}


int CvPlayer::getCultureBombTimer() const
{
	return m_iCultureBombTimer;
}



void CvPlayer::setCultureBombTimer(int iNewValue)
{
	if(getCultureBombTimer() != iNewValue)
	{
		m_iCultureBombTimer = iNewValue;
		CvAssert(getCultureBombTimer() >= 0);
	}
}



void CvPlayer::changeCultureBombTimer(int iChange)
{
	setCultureBombTimer(getCultureBombTimer() + iChange);
}



int CvPlayer::getConversionTimer() const
{
	return m_iConversionTimer;
}



void CvPlayer::setConversionTimer(int iNewValue)
{
	if(getConversionTimer() != iNewValue)
	{
		m_iConversionTimer = iNewValue;
		CvAssert(getConversionTimer() >= 0);

		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(MiscButtons_DIRTY_BIT, true);
		}
	}
}



void CvPlayer::changeConversionTimer(int iChange)
{
	setConversionTimer(getConversionTimer() + iChange);
}


CvCity* CvPlayer::getCapitalCity()
{
	return getCity(m_iCapitalCityID);
}


const CvCity* CvPlayer::getCapitalCity() const
{
	return getCity(m_iCapitalCityID);
}


void CvPlayer::setCapitalCity(CvCity* pNewCapitalCity)
{
	CvCity* pOldCapitalCity;

	pOldCapitalCity = getCapitalCity();

	if(pOldCapitalCity != pNewCapitalCity)
	{
		if(pNewCapitalCity != NULL)
		{

			if(GetOriginalCapitalX() == -1 || GetOriginalCapitalY() == -1)
			{
				m_iOriginalCapitalX = pNewCapitalCity->getX();
				m_iOriginalCapitalY = pNewCapitalCity->getY();
			}

			m_iCapitalCityID = pNewCapitalCity->GetID();

			pNewCapitalCity->SetEverCapital(true);
#ifdef AUI_PLAYER_FIX_VENICE_ONLY_BANS_SETTLERS_NOT_SETTLING
			if(pNewCapitalCity->IsPuppet())
				pNewCapitalCity->DoAnnex();
#endif
		}
		else
		{
			m_iCapitalCityID = FFreeList::INVALID_INDEX;
		}
	}
}



int CvPlayer::GetOriginalCapitalX() const
{
	return m_iOriginalCapitalX;
}



int CvPlayer::GetOriginalCapitalY() const
{
	return m_iOriginalCapitalY;
}



bool CvPlayer::IsHasLostCapital() const
{
	return m_bLostCapital;
}



void CvPlayer::SetHasLostCapital(bool bValue, PlayerTypes eConqueror)
{
	if(bValue != m_bLostCapital)
	{
		m_bLostCapital = bValue;
		m_eConqueror = eConqueror;


		if(!isMinorCiv())
		{
			int iMostOriginalCapitals = 0;
			TeamTypes eWinningTeam = NO_TEAM;
			PlayerTypes eWinningPlayer = NO_PLAYER;

			{


				typedef std::tr1::array<int, MAX_CIV_TEAMS> CivTeamArray;
				CivTeamArray aTeamCityCount;
#if defined(LEKMOD_MACOS)
				aTeamCityCount.fill(0);
#else
				aTeamCityCount.assign(0);
#endif

				CvMap& kMap = GC.getMap();
				for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; ++iLoopPlayer)
				{
					const PlayerTypes ePlayer = static_cast<PlayerTypes>(iLoopPlayer);
					CvPlayer& kLoopPlayer = GET_PLAYER(ePlayer);
					if(kLoopPlayer.isEverAlive())
					{
						const int iOriginalCapitalX = kLoopPlayer.GetOriginalCapitalX();
						const int iOriginalCapitalY = kLoopPlayer.GetOriginalCapitalY();
						if(iOriginalCapitalX != -1 && iOriginalCapitalY != -1)
						{
							CvPlot* pkPlot = kMap.plot(iOriginalCapitalX, iOriginalCapitalY);
							if(pkPlot != NULL)
							{
								CvCity* pkCapitalCity = pkPlot->getPlotCity();
								if(pkCapitalCity != NULL)
								{
									const PlayerTypes eCapitalOwner = pkCapitalCity->getOwner();
									if(eCapitalOwner != NO_PLAYER)
									{
										CvPlayer& kCapitalOwnerPlayer = GET_PLAYER(eCapitalOwner);
										aTeamCityCount[kCapitalOwnerPlayer.getTeam()]++;
									}
								}
							}	
						}
					}
				}


				CivTeamArray::iterator itMax = max_element(aTeamCityCount.begin(), aTeamCityCount.end());
				if(count(aTeamCityCount.begin(), aTeamCityCount.end(), *itMax) == 1)
				{
					eWinningTeam = static_cast<TeamTypes>(itMax - aTeamCityCount.begin());
					iMostOriginalCapitals = *itMax;

					CvTeam& kTeam = GET_TEAM(eWinningTeam);
					eWinningPlayer = kTeam.getLeaderID();
				}			
			}


			if(bValue)
			{



				Localization::String localizedBuffer;
				Localization::String localizedSummary;
				NotificationTypes eNotificationType = NOTIFICATION_CAPITAL_LOST;

				for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
				{
					PlayerTypes ePlayer = (PlayerTypes)ui;
					CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
					if(!pNotifications)
					{
						continue;
					}


					if(ePlayer == GetID())
					{
						eNotificationType = NOTIFICATION_CAPITAL_LOST_ACTIVE_PLAYER;
						localizedSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_YOU_LOST_CAPITAL");
						if (eWinningPlayer == ePlayer)
						{
							localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_YOU_LOST_CAPITAL_YOU_WINNING");
							localizedBuffer << iMostOriginalCapitals;
						}
						else if (eWinningTeam != NO_TEAM)
						{
							if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(eWinningTeam))
							{
								if (eWinningPlayer != NO_PLAYER)
								{
									localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_YOU_LOST_CAPITAL_OTHER_PLAYER_WINNING");
									if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(eWinningPlayer).isHuman())
									{
										localizedBuffer << GET_PLAYER(eWinningPlayer).getNickName();
									}
									else
									{
										localizedBuffer << GET_PLAYER(eWinningPlayer).getNameKey();
									}
									localizedBuffer << iMostOriginalCapitals;
								}
								else
								{
									localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_YOU_LOST_CAPITAL_TEAM_WINNING");
									localizedBuffer << (int)eWinningTeam;
									localizedBuffer << iMostOriginalCapitals;
								}
							}
							else
							{
								localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_YOU_LOST_CAPITAL_UNMET_WINNING");
								localizedBuffer << iMostOriginalCapitals;
							}
						}
						else
						{
							localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_YOU_LOST_CAPITAL");
						}
					}

					else if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(getTeam()))
					{
						eNotificationType = NOTIFICATION_CAPITAL_LOST;
						localizedSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_PLAYER_LOST_CAPITAL");
						localizedSummary << getCivilizationShortDescriptionKey();

						if (eWinningTeam != NO_TEAM)
						{
							if (GET_TEAM(eWinningTeam).isHasMet(getTeam()))
							{
								if (eWinningPlayer == GetID())
								{
									localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_OTHER_LOST_CAPITAL_YOU_WINNING");

									if (GC.getGame().isGameMultiPlayer() && isHuman())
									{
										localizedBuffer << getNickName();
									}
									else
									{
										localizedBuffer << getNameKey();
									}
									localizedBuffer << iMostOriginalCapitals;
								}
								else if (eWinningPlayer != NO_PLAYER)
								{
									localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_OTHER_LOST_CAPITAL_OTHER_WINNING");
									if (GC.getGame().isGameMultiPlayer() && isHuman())
									{
										localizedBuffer << getNickName();
									}
									else
									{
										localizedBuffer << getNameKey();
									}

									if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(eWinningPlayer).isHuman())
									{
										localizedBuffer << GET_PLAYER(eWinningPlayer).getNickName();
									}
									else
									{
										localizedBuffer << GET_PLAYER(eWinningPlayer).getNameKey();
									}
									localizedBuffer << iMostOriginalCapitals;
								}
								else
								{
									localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_OTHER_LOST_CAPITAL_TEAM_WINNING");
									if (GC.getGame().isGameMultiPlayer() && isHuman())
									{
										localizedBuffer << getNickName();
									}
									else
									{
										localizedBuffer << getNameKey();
									}

									localizedBuffer << (int)eWinningTeam;
									localizedBuffer << iMostOriginalCapitals;
								}
							}
							else
							{
								localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_OTHER_LOST_CAPITAL_UNMET_WINNING");
								if (GC.getGame().isGameMultiPlayer() && isHuman())
								{
									localizedBuffer << getNickName();
								}
								else
								{
									localizedBuffer << getNameKey();
								}
								localizedBuffer << iMostOriginalCapitals;
							}
						}
						else
						{
							localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_OTHER_LOST_CAPITAL");
							if (GC.getGame().isGameMultiPlayer() && isHuman())
							{
								localizedBuffer << getNickName();
							}
							else
							{
								localizedBuffer << getNameKey();
							}
						}
					}
					else
					{
						eNotificationType = NOTIFICATION_CAPITAL_LOST;
						localizedSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_SOMEONE_LOST_CAPITAL");

						if (eWinningTeam != NO_TEAM)
						{
							if (GET_TEAM(eWinningTeam).isHasMet(getTeam()))
							{
								if (eWinningPlayer == GetID())
								{
									localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_UNMET_LOST_CAPITAL_YOU_WINNING");
									localizedBuffer << iMostOriginalCapitals;
								}
								else if (eWinningPlayer != NO_PLAYER)
								{
									localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_UNMET_LOST_CAPITAL_OTHER_WINNING");
									if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(eWinningPlayer).isHuman())
									{
										localizedBuffer << GET_PLAYER(eWinningPlayer).getNickName();
									}
									else
									{
										localizedBuffer << GET_PLAYER(eWinningPlayer).getNameKey();
									}
									localizedBuffer << iMostOriginalCapitals;
								}
								else
								{
									localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_UNMET_LOST_CAPITAL_TEAM_WINNING");
									localizedBuffer << (int)eWinningTeam;
									localizedBuffer << iMostOriginalCapitals;
								}
							}
							else
							{
								localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_UNMET_LOST_CAPITAL_UNMET_WINNING");
								localizedBuffer << iMostOriginalCapitals;
							}
						}
						else
						{
							localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_UNMET_LOST_CAPITAL");
						}
					}

					pNotifications->Add(eNotificationType, localizedBuffer.toUTF8(), localizedSummary.toUTF8(), -1, -1, -1);
				}


				{
					Localization::String message;
					if (eWinningPlayer != NO_PLAYER)
					{
						message = Localization::Lookup("TXT_KEY_NOTIFICATION_OTHER_LOST_CAPITAL_OTHER_WINNING");
						if (GC.getGame().isGameMultiPlayer() && isHuman())
						{
							localizedBuffer << getNickName();
						}
						else
						{
							localizedBuffer << getNameKey();
						}

						if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(eWinningPlayer).isHuman())
						{
							localizedBuffer << GET_PLAYER(eWinningPlayer).getNickName();
						}
						else
						{
							localizedBuffer << GET_PLAYER(eWinningPlayer).getNameKey();
						}

						localizedBuffer << iMostOriginalCapitals;
					}
					else if (eWinningTeam != NO_TEAM)
					{
						message = Localization::Lookup("TXT_KEY_NOTIFICATION_OTHER_LOST_CAPITAL_TEAM_WINNING");
						if (GC.getGame().isGameMultiPlayer() && isHuman())
						{
							localizedBuffer << getNickName();
						}
						else
						{
							localizedBuffer << getNameKey();
						}

						localizedBuffer << (int)eWinningTeam;
						localizedBuffer << iMostOriginalCapitals;
					}
					else
					{
						message = Localization::Lookup("TXT_KEY_NOTIFICATION_OTHER_LOST_CAPITAL");
						if (GC.getGame().isGameMultiPlayer() && isHuman())
						{
							localizedBuffer << getNickName();
						}
						else
						{
							localizedBuffer << getNameKey();
						}
					}

					CvString translatedMessage = message.toUTF8();
					GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, GetID(), translatedMessage, GetOriginalCapitalX(), GetOriginalCapitalY());
				}					
			}

			else
			{
				Localization::String localizedBuffer;
				Localization::String localizedSummary;

				for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
				{
					PlayerTypes ePlayer = (PlayerTypes)ui;
					CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
					if(!pNotifications)
					{
						continue;
					}


					if(ePlayer == GetID())
					{
						localizedSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_YOU_RECOVERED_CAPITAL");
						if (eWinningPlayer == ePlayer)
						{
							localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_YOU_REGAINED_CAPITAL_YOU_WINNING");
							localizedBuffer << iMostOriginalCapitals;
						}
						else if (GET_TEAM(getTeam()).isHasMet(eWinningTeam))
						{
							if (eWinningPlayer != NO_PLAYER)
							{
								localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_YOU_REGAINED_CAPITAL_OTHER_PLAYER_WINNING");
								if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(eWinningPlayer).isHuman())
								{
									localizedBuffer << GET_PLAYER(eWinningPlayer).getNickName();
								}
								else
								{
									localizedBuffer << GET_PLAYER(eWinningPlayer).getNameKey();
								}
								localizedBuffer << iMostOriginalCapitals;
							}
							else
							{
								localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_YOU_REGAINED_CAPITAL_TEAM_WINNING");
								localizedBuffer << (int)eWinningTeam;
								localizedBuffer << iMostOriginalCapitals;
							}
						}
						else if (eWinningTeam != NO_TEAM)
						{
							localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_YOU_REGAINED_CAPITAL_UNMET_WINNING");
							localizedBuffer << iMostOriginalCapitals;
						}
						else
						{
							localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_YOU_RECOVERED_CAPITAL");
						}
					}

					else if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(getTeam()))
					{
						localizedSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_PLAYER_RECOVERED_CAPITAL");
						localizedSummary << getCivilizationShortDescriptionKey();

						if (eWinningTeam != NO_TEAM)
						{
							if (GET_TEAM(eWinningTeam).isHasMet(getTeam()))
							{
								if (eWinningPlayer == ePlayer)
								{
									localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_OTHER_REGAINED_CAPITAL_YOU_WINNING");
									localizedBuffer << iMostOriginalCapitals;
								}
								else if (eWinningPlayer != NO_PLAYER)
								{
									localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_OTHER_REGAINED_CAPITAL_OTHER_WINNING");
									if (GC.getGame().isGameMultiPlayer() && isHuman())
									{
										localizedBuffer << getNickName();
									}
									else
									{
										localizedBuffer << getNameKey();
									}

									if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(eWinningPlayer).isHuman())
									{
										localizedBuffer << GET_PLAYER(eWinningPlayer).getNickName();
									}
									else
									{
										localizedBuffer << GET_PLAYER(eWinningPlayer).getNameKey();
									}
									localizedBuffer << iMostOriginalCapitals;
								}
								else
								{
									localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_OTHER_REGAINED_CAPITAL_TEAM_WINNING");
									if (GC.getGame().isGameMultiPlayer() && isHuman())
									{
										localizedBuffer << getNickName();
									}
									else
									{
										localizedBuffer << getNameKey();
									}

									localizedBuffer << (int)eWinningTeam;
									localizedBuffer << iMostOriginalCapitals;
								}
							}
							else
							{
								localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_OTHER_REGAINED_CAPITAL_UNMET_WINNING");
								if (GC.getGame().isGameMultiPlayer() && isHuman())
								{
									localizedBuffer << getNickName();
								}
								else
								{
									localizedBuffer << getNameKey();
								}
								localizedBuffer << iMostOriginalCapitals;
							}
						}
						else
						{
							localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_OTHER_REGAINED_CAPITAL");
							if (GC.getGame().isGameMultiPlayer() && isHuman())
							{
								localizedBuffer << getNickName();
							}
							else
							{
								localizedBuffer << getNameKey();
							}
						}
					}
					else
					{
						localizedSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_SOMEONE_RECOVERED_CAPITAL");

						if (eWinningTeam != NO_TEAM)
						{
							if (GET_TEAM(eWinningTeam).isHasMet(getTeam()))
							{
								if (eWinningPlayer == ePlayer)
								{
									localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_UNMET_REGAINED_CAPITAL_YOU_WINNING");
									localizedBuffer << iMostOriginalCapitals;
								}
								else if (eWinningPlayer != NO_PLAYER)
								{
									localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_UNMET_REGAINED_CAPITAL_OTHER_WINNING");
									if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(eWinningPlayer).isHuman())
									{
										localizedBuffer << GET_PLAYER(eWinningPlayer).getNickName();
									}
									else
									{
										localizedBuffer << GET_PLAYER(eWinningPlayer).getNameKey();
									}
									localizedBuffer << iMostOriginalCapitals;
								}
								else
								{
									localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_UNMET_REGAINED_CAPITAL_TEAM_WINNING");
									localizedBuffer << (int)eWinningTeam;
									localizedBuffer << iMostOriginalCapitals;
								}
							}
							else
							{
								localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_UNMET_REGAINED_CAPITAL_UNMET_WINNING");
								localizedBuffer << iMostOriginalCapitals;
							}
						}
						else
						{
							localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_UNMET_REGAINED_CAPITAL");
						}
					}

					pNotifications->Add(NOTIFICATION_CAPITAL_RECOVERED, localizedBuffer.toUTF8(), localizedSummary.toUTF8(), -1, -1, -1);
				}


				{
					Localization::String message;
					if (eWinningPlayer != NO_PLAYER)
					{
						message = Localization::Lookup("TXT_KEY_NOTIFICATION_OTHER_REGAINED_CAPITAL_OTHER_WINNING");
						if (GC.getGame().isGameMultiPlayer() && isHuman())
						{
							localizedBuffer << getNickName();
						}
						else
						{
							localizedBuffer << getNameKey();
						}

						if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(eWinningPlayer).isHuman())
						{
							localizedBuffer << GET_PLAYER(eWinningPlayer).getNickName();
						}
						else
						{
							localizedBuffer << GET_PLAYER(eWinningPlayer).getNameKey();
						}

						localizedBuffer << iMostOriginalCapitals;
					}
					else if (eWinningTeam != NO_TEAM)
					{
						message = Localization::Lookup("TXT_KEY_NOTIFICATION_OTHER_REGAINED_CAPITAL_TEAM_WINNING");
						if (GC.getGame().isGameMultiPlayer() && isHuman())
						{
							localizedBuffer << getNickName();
						}
						else
						{
							localizedBuffer << getNameKey();
						}

						localizedBuffer << (int)eWinningTeam;
						localizedBuffer << iMostOriginalCapitals;
					}
					else
					{
						message = Localization::Lookup("TXT_KEY_NOTIFICATION_OTHER_REGAINED_CAPITAL");
						if (GC.getGame().isGameMultiPlayer() && isHuman())
						{
							localizedBuffer << getNickName();
						}
						else
						{
							localizedBuffer << getNameKey();
						}
					}

					CvString translatedMessage = message.toUTF8();
					GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, GetID(), translatedMessage, GetOriginalCapitalX(), GetOriginalCapitalY());
				}					
			}
		}
	}
}



PlayerTypes CvPlayer::GetCapitalConqueror() const
{
	return m_eConqueror;
}


int CvPlayer::getCitiesLost() const
{
	return m_iCitiesLost;
}



void CvPlayer::changeCitiesLost(int iChange)
{
	m_iCitiesLost = (m_iCitiesLost + iChange);
}


int CvPlayer::getPower() const
{
	if(m_iTurnMightRecomputed < GC.getGame().getElapsedGameTurns())
	{

		const_cast<CvPlayer*>(this)->m_iTurnMightRecomputed = GC.getGame().getElapsedGameTurns();
		const_cast<CvPlayer*>(this)->m_iMilitaryMight = calculateMilitaryMight();
		const_cast<CvPlayer*>(this)->m_iEconomicMight = calculateEconomicMight();
	}
	return m_iMilitaryMight + m_iEconomicMight;
}


int CvPlayer::GetMilitaryMight() const
{
	if(m_iTurnMightRecomputed < GC.getGame().getElapsedGameTurns())
	{

		const_cast<CvPlayer*>(this)->m_iTurnMightRecomputed = GC.getGame().getElapsedGameTurns();
		const_cast<CvPlayer*>(this)->m_iMilitaryMight = calculateMilitaryMight();
		const_cast<CvPlayer*>(this)->m_iEconomicMight = calculateEconomicMight();
	}
	return m_iMilitaryMight;
}


int CvPlayer::GetEconomicMight() const
{
	if(m_iTurnMightRecomputed < GC.getGame().getElapsedGameTurns())
	{

		const_cast<CvPlayer*>(this)->m_iTurnMightRecomputed = GC.getGame().getElapsedGameTurns();
		const_cast<CvPlayer*>(this)->m_iMilitaryMight = calculateMilitaryMight();
		const_cast<CvPlayer*>(this)->m_iEconomicMight = calculateEconomicMight();
	}
	return m_iEconomicMight;
}


int CvPlayer::calculateMilitaryMight() const
{
	int rtnValue = 0;
	const CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{

		int iPower =  pLoopUnit->GetPower();
		if (pLoopUnit->getDomainType() == DOMAIN_SEA)
		{
			iPower /= 2;
		}
		rtnValue += iPower;
	}



	float fGoldMultiplier = 1.0f + (sqrt((float)GetTreasury()->GetGold()) / 100.0f);
	if(fGoldMultiplier > 2.0f) fGoldMultiplier = 2.0f;

	rtnValue = (int)(rtnValue * fGoldMultiplier);

	return rtnValue;
}



int CvPlayer::calculateEconomicMight() const
{

	int iEconomicMight = 5;

	iEconomicMight += getTotalPopulation();



	iEconomicMight += calculateTotalYield(YIELD_PRODUCTION);

	iEconomicMight += calculateTotalYield(YIELD_GOLD);



	return iEconomicMight;
}


int CvPlayer::calculateProductionMight() const
{
	int iMight = 0;

	const CvCity* pLoopCity;
	int iLoop;
	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iMight += pLoopCity->getRawProductionDifference(                true,               false);
	}

	return iMight;
}


int CvPlayer::getCombatExperience() const
{
	return m_iCombatExperience;
}


void CvPlayer::setCombatExperience(int iExperience)
{
	CvAssert(iExperience >= 0);

	if(iExperience != getCombatExperience())
	{
		m_iCombatExperience = iExperience;


		if(!isBarbarian())
		{
			int iExperienceThreshold = greatGeneralThreshold();
			if(m_iCombatExperience >= iExperienceThreshold && iExperienceThreshold > 0)
			{

				CvCity* pBestCity = NULL;
				int iBestValue = INT_MAX;
				int iLoop;
				for(CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
				{
					int iValue = 4 * GC.getGame().getJonRandNum(getNumCities(), "Great General City Selection");

					for(int i = 0; i < NUM_YIELD_TYPES; i++)
					{
						iValue += pLoopCity->findYieldRateRank((YieldTypes)i);
					}
					iValue += pLoopCity->findPopulationRank();

					if(iValue < iBestValue)
					{
						pBestCity = pLoopCity;
						iBestValue = iValue;
					}
				}

				if(pBestCity)
				{

					PromotionTypes eGreatGeneralPromotion = NO_PROMOTION;
#ifdef AUI_WARNING_FIXES
					for (uint iI = 0; iI < GC.getNumPromotionInfos(); iI++)
#else
					for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
#endif
					{
						const PromotionTypes eLoopPromotion = static_cast<PromotionTypes>(iI);
						CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eLoopPromotion);
						if(pkPromotionInfo)
						{
							if(pkPromotionInfo->IsGreatGeneral())
							{
								eGreatGeneralPromotion = eLoopPromotion;
								break;
							}
						}
					}


					if(eGreatGeneralPromotion != NO_PROMOTION)
					{
#ifdef AUI_WARNING_FIXES
						for (uint iI = 0; iI < GC.getNumUnitInfos(); iI++)
#else
						for(int iI = 0; iI < GC.getNumUnitInfos(); iI++)
#endif
						{
							const UnitTypes eLoopUnit = static_cast<UnitTypes>(iI);
							CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eLoopUnit);
							if(pkUnitInfo)
							{
								if(pkUnitInfo->GetFreePromotions(eGreatGeneralPromotion))
								{

									const UnitTypes eUnit = (UnitTypes) getCivilizationInfo().getCivilizationUnits((UnitClassTypes)pkUnitInfo->GetUnitClassType());

									if(eUnit == eLoopUnit)
									{
										pBestCity->createGreatGeneral(eUnit);
										setCombatExperience(getCombatExperience() - iExperienceThreshold);
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
}


void CvPlayer::changeCombatExperience(int iChange)
{
	if(getCombatExperience() + iChange < 0)
	{
		setCombatExperience(0);
	}
	else
	{
		setCombatExperience(getCombatExperience() + iChange);
	}

	m_iLifetimeCombatExperience += iChange;
}


int CvPlayer::getNavalCombatExperience() const
{
	return m_iNavalCombatExperience;
}


void CvPlayer::setNavalCombatExperience(int iExperience)
{
	CvAssert(iExperience >= 0);

	if(iExperience != getNavalCombatExperience())
	{
		m_iNavalCombatExperience = iExperience;


		if(!isBarbarian())
		{
			int iExperienceThreshold = greatAdmiralThreshold();
			if(m_iNavalCombatExperience >= iExperienceThreshold && iExperienceThreshold > 0)
			{

				CvCity* pBestCity = NULL;
				int iBestValue = INT_MAX;
				int iLoop;
				for(CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
				{
					if(!pLoopCity->isCoastal())
					{
						continue;
					}

					int iValue = 4 * GC.getGame().getJonRandNum(getNumCities(), "Great Admiral City Selection");

					for(int i = 0; i < NUM_YIELD_TYPES; i++)
					{
						iValue += pLoopCity->findYieldRateRank((YieldTypes)i);
					}
					iValue += pLoopCity->findPopulationRank();

					if(iValue < iBestValue)
					{
						pBestCity = pLoopCity;
						iBestValue = iValue;
					}
				}

				if(pBestCity)
				{

					PromotionTypes eGreatAdmiralPromotion = NO_PROMOTION;
#ifdef AUI_WARNING_FIXES
					for (uint iI = 0; iI < GC.getNumPromotionInfos(); iI++)
#else
					for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
#endif
					{
						const PromotionTypes eLoopPromotion = static_cast<PromotionTypes>(iI);
						CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eLoopPromotion);
						if(pkPromotionInfo)
						{
							if(pkPromotionInfo->IsGreatAdmiral())
							{
								eGreatAdmiralPromotion = eLoopPromotion;
							}
						}
					}


					if(eGreatAdmiralPromotion != NO_PROMOTION)
					{
#ifdef AUI_WARNING_FIXES
						for (uint iI = 0; iI < GC.getNumUnitInfos(); iI++)
#else
						for(int iI = 0; iI < GC.getNumUnitInfos(); iI++)
#endif
						{
							const UnitTypes eLoopUnit = static_cast<UnitTypes>(iI);
							CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eLoopUnit);
							if(pkUnitInfo)
							{
								if(pkUnitInfo->GetFreePromotions(eGreatAdmiralPromotion))
								{

									const UnitTypes eUnit = (UnitTypes) getCivilizationInfo().getCivilizationUnits((UnitClassTypes)pkUnitInfo->GetUnitClassType());

									if(eUnit == eLoopUnit)
									{
										pBestCity->createGreatAdmiral(eUnit);
										setNavalCombatExperience(getNavalCombatExperience() - iExperienceThreshold);
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
}


void CvPlayer::changeNavalCombatExperience(int iChange)
{
	if(getNavalCombatExperience() + iChange < 0)
	{
		setNavalCombatExperience(0);
	}
	else
	{
		setNavalCombatExperience(getNavalCombatExperience() + iChange);
	}

	m_iLifetimeCombatExperience += iChange;
}


int CvPlayer::getLifetimeCombatExperience() const
{
	return m_iLifetimeCombatExperience;
}


int CvPlayer::getBorderObstacleCount() const
{
	return m_iBorderObstacleCount;
}


bool CvPlayer::isBorderObstacle() const
{
	return (getBorderObstacleCount() > 0);
}


void CvPlayer::changeBorderObstacleCount(int iChange)
{
	if(iChange != 0)
	{
		m_iBorderObstacleCount = (m_iBorderObstacleCount + iChange);
		CvAssert(getBorderObstacleCount() >= 0);
	}
}


int CvPlayer::getNetID() const
{
	return CvPreGame::netID(GetID());
}


void CvPlayer::setNetID(int iNetID)
{
	CvPreGame::setNetID(GetID(), iNetID);
}


bool CvPlayer::isConnected() const
{
	return gDLL->IsPlayerConnected(GetID());
}


void CvPlayer::sendTurnReminder()
{

	gDLL->sendTurnReminder(GetID());








}


uint CvPlayer::getStartTime() const
{
	return m_uiStartTime;
}



void CvPlayer::setStartTime(uint uiStartTime)
{
	m_uiStartTime = uiStartTime;
}



uint CvPlayer::getTotalTimePlayed() const
{
	return ((timeGetTime() - m_uiStartTime)/1000);
}



bool CvPlayer::isMinorCiv() const
{
	return CvPreGame::isMinorCiv(m_eID);
}


bool CvPlayer::isMajorCiv() const
{
	return GET_TEAM(getTeam()).isMajorCiv();
}

#ifdef NQ_CHEAT_FIRST_ROYAL_LIBRARY_COMES_WITH_GREAT_WORK

void CvPlayer::SetHasEverBuiltRoyalLibrary(bool bValue)
{
	m_bHasEverBuiltRoyalLibrary = bValue;
}


bool CvPlayer::GetHasEverBuiltRoyalLibrary()
{
	return m_bHasEverBuiltRoyalLibrary;
}
#endif



bool CvPlayer::IsHasBetrayedMinorCiv() const
{
	return m_bHasBetrayedMinorCiv;
}



void CvPlayer::SetHasBetrayedMinorCiv(bool bValue)
{
	if(IsHasBetrayedMinorCiv() != bValue)
	{
		m_bHasBetrayedMinorCiv = bValue;
	}
}


void CvPlayer::setAlive(bool bNewValue, bool bNotify)
{
	CvString strBuffer;
	int iI;

	if(isAlive() != bNewValue)
	{
		m_bAlive = bNewValue;

		GET_TEAM(getTeam()).changeAliveCount((isAlive()) ? 1 : -1);

		GC.getGame().GetGameLeagues()->DoPlayerAliveStatusChanged(GetID());


		if(isMinorCiv())
		{
			GetMinorCivAI()->DoChangeAliveStatus(bNewValue);
		}

		if(isAlive())
		{
			if(!isEverAlive())
			{
				m_bEverAlive = true;

				GET_TEAM(getTeam()).changeEverAliveCount(1);
			}

			if(getNumCities() == 0)
			{
				setFoundedFirstCity(false);
			}

			GET_TEAM(getTeam()).SetKilledByTeam(NO_TEAM);

#ifdef AUI_GAME_BETTER_HYBRID_MODE
			CvGame& kCurGame = GC.getGame();
			if (kCurGame.isNoPlayerActive() || (isSimultaneousTurns() && (GET_TEAM(getTeam()).isTurnActive() || kCurGame.getCurrentTurnOrderActive() == getTurnOrder())))
#else
			if(isSimultaneousTurns() || (GC.getGame().getNumGameTurnActive() == 0) || (GC.getGame().isSimultaneousTeamTurns() && GET_TEAM(getTeam()).isTurnActive()))
#endif
			{
				setTurnActive(true);
			}

			gDLL->openSlot(GetID());
		}
		else
		{
			clearResearchQueue();
			killUnits();
			killCities();
			if(CvPreGame::isNetworkMultiplayerGame() && m_eID == GC.getGame().getActivePlayer())
				gDLL->netDisconnect();

			if (!GET_TEAM(getTeam()).isAlive())
			{
				for (int i = 0; i < MAX_TEAMS; i++)
				{
					TeamTypes eTheirTeam = (TeamTypes)i;
					if (getTeam() != eTheirTeam)
					{

						GET_TEAM(getTeam()).CloseEmbassyAtTeam(eTheirTeam);
						GET_TEAM(eTheirTeam).CloseEmbassyAtTeam(getTeam());


						GET_TEAM(getTeam()).CancelResearchAgreement(eTheirTeam);
						GET_TEAM(eTheirTeam).CancelResearchAgreement(getTeam());
					}
				}
			}


			for(int iLoop = 0; iLoop < MAX_PLAYERS; iLoop++)
			{
				PlayerTypes eLoopPlayer = (PlayerTypes) iLoop;
				SetIncomingUnitCountdown(eLoopPlayer, -1);
				SetIncomingUnitType(eLoopPlayer, NO_UNIT);
			}

			GC.getGame().GetGameDeals()->DoCancelAllDealsWithPlayer(GetID());


			for(int iPlayerLoop = MAX_MAJOR_CIVS; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
			{
				PlayerTypes eOtherPlayer = (PlayerTypes) iPlayerLoop;
				GET_PLAYER(eOtherPlayer).GetMinorCivAI()->ResetFriendshipWithMajor(GetID());
			}

			setTurnActive(false);

			gDLL->closeSlot(GetID());

			if(bNotify && !isBarbarian())
			{
				Localization::String strMessage = Localization::Lookup("TXT_KEY_MISC_CIV_DESTROYED");
				strMessage << getCivilizationAdjectiveKey();
				Localization::String strSummary = Localization::Lookup("TXT_KEY_MISC_CIV_DESTROYED_SHORT");
				strSummary << getCivilizationShortDescriptionKey();

				for(iI = 0; iI < MAX_PLAYERS; iI++)
				{
					const PlayerTypes eOtherPlayer = static_cast<PlayerTypes>(iI);
					CvPlayerAI& kOtherPlayer = GET_PLAYER(eOtherPlayer);

					if(kOtherPlayer.isAlive() && kOtherPlayer.GetNotifications())
					{
						kOtherPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_KILLED, strMessage.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
					}
				}

				GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, GetID(), strMessage.toUTF8(), -1, -1);
			}

			GC.getGame().testVictory();
		}

		GC.getGame().setScoreDirty(true);
	}
}


void CvPlayer::setBeingResurrected(bool bValue)
{
	if (m_bBeingResurrected != bValue)
	{
		m_bBeingResurrected = bValue;
	}
}


void CvPlayer::verifyAlive()
{
	bool bKill;

	if(isAlive())
	{
		bKill = false;

		if(!bKill)
		{
			if(!isBarbarian())
			{
				if(getNumCities() == 0 && getAdvancedStartPoints() < 0)
				{
					if((getNumUnits() == 0) || (!(GC.getGame().isOption(GAMEOPTION_COMPLETE_KILLS)) && isFoundedFirstCity()))
					{
						if(!GetPlayerTraits()->IsStaysAliveZeroCities())
						{
							bKill = true;
						}
					}
				}
			}
		}

		if(!bKill)
		{
			if(!isBarbarian())
			{
				if(GC.getGame().getMaxCityElimination() > 0)
				{
					if(getCitiesLost() >= GC.getGame().getMaxCityElimination())
					{
						bKill = true;
					}
				}
			}
		}

		if(bKill)
		{
			setAlive(false, false);
		}
	}
	else
	{
		if((getNumCities() > 0) || (getNumUnits() > 0))
		{
			setAlive(true);
		}
	}
}



bool CvPlayer::isTurnActive() const
{
	return m_bTurnActive;
}


void CvPlayer::setTurnActiveForPbem(bool bActive)
{
	CvAssertMsg(GC.getGame().isPbem(), "You are using setTurnActiveForPbem. Are you sure you know what you're doing?");




	if(isTurnActive() != bActive)
	{
		m_bTurnActive = bActive;
		GC.getGame().changeNumGameTurnActive(isTurnActive() ? 1 : -1, "setTurnActiveForPlayByEmail");
	}
}



void CvPlayer::setTurnActive(bool bNewValue, bool bDoTurn)
{
#ifdef GAME_ALLOW_ONLY_ONE_UNIT_MOVE_ON_TURN_LOADING
	if (GC.getGame().isOption("GAMEOPTION_FIRSTMOVE")) 
	{
		float t1;
		float t2;
		GC.getGame().GetTurnTimerData(t1, t2);
		if (isHuman() && isAlive())
		{

		}
	}
#endif
	if(isTurnActive() != bNewValue)
	{
		m_bTurnActive = bNewValue;
		DLLUI->PublishEndTurnDirty();

		CvGame& kGame = GC.getGame();





		if(isTurnActive())
		{
			CvAssertMsg(isAlive(), "isAlive is expected to be true");

			setEndTurn(false);

			DoUnitAttrition();

			if(kGame.getActivePlayer() == m_eID)
			{
				CvMap& theMap = GC.getMap();
				theMap.updateDeferredFog();
			}

			if((kGame.isHotSeat() || kGame.isPbem()) && isHuman() && bDoTurn)
			{
				DLLUI->clearEventMessages();

				kGame.setActivePlayer(GetID());
			}

			if(CvPreGame::isPitBoss() && kGame.getActivePlayer() != m_eID && isHuman() && gDLL->IsHost() && !isConnected())
			{
				sendTurnReminder();
			}

			std::ostringstream infoStream;
			infoStream << "setTurnActive() for player ";
			infoStream << (int)GetID();
			infoStream << " ";
			infoStream << getName();
			kGame.changeNumGameTurnActive(1, infoStream.str());

			DLLUI->PublishPlayerTurnStatus(DLLUIClass::TURN_START, GetID());

			if(bDoTurn)
			{
				SetAllUnitsUnprocessed();

				bool bCommonPathFinderMPCaching = GC.getPathFinder().SetMPCacheSafe(true);
				bool bIgnoreUnitsPathFinderMPCaching = GC.getIgnoreUnitsPathFinder().SetMPCacheSafe(true);
				bool bTacticalPathFinderMPCaching = GC.GetTacticalAnalysisMapFinder().SetMPCacheSafe(true);
				bool bInfluencePathFinderMPCaching = GC.getInfluenceFinder().SetMPCacheSafe(true);
				bool bRoutePathFinderMPCaching = GC.getRouteFinder().SetMPCacheSafe(true);
				bool bWaterRoutePathFinderMPCaching = GC.GetWaterRouteFinder().SetMPCacheSafe(true);

				{
					AI_PERF_FORMAT("AI-perf.csv", ("Connections/Gold, Turn %03d, %s", kGame.getElapsedGameTurns(), getCivilizationShortDescription()) );


					m_pCityConnections->Update();
					GetTreasury()->DoUpdateCityConnectionGold();
					DoUpdateHappiness();
				}

				{
					AI_PERF_FORMAT("AI-perf.csv", ("Builder Tasking, Turn %03d, %s", kGame.getElapsedGameTurns(), getCivilizationShortDescription()) );

					m_pBuilderTaskingAI->Update();
				}

				if(kGame.isFinalInitialized())
				{
					if(isAlive())
					{
						if(GetDiplomacyRequests())
						{
							GetDiplomacyRequests()->BeginTurn();
						}

						doTurn();

						doTurnUnits();
					}
				}

				GC.getPathFinder().SetMPCacheSafe(bCommonPathFinderMPCaching);
				GC.getIgnoreUnitsPathFinder().SetMPCacheSafe(bIgnoreUnitsPathFinderMPCaching);
				GC.GetTacticalAnalysisMapFinder().SetMPCacheSafe(bTacticalPathFinderMPCaching);
				GC.getInfluenceFinder().SetMPCacheSafe(bInfluencePathFinderMPCaching);
				GC.getRouteFinder().SetMPCacheSafe(bRoutePathFinderMPCaching);
				GC.GetWaterRouteFinder().SetMPCacheSafe(bWaterRoutePathFinderMPCaching);

				if((GetID() == kGame.getActivePlayer()) && (kGame.getElapsedGameTurns() > 0))
				{
					if(kGame.isNetworkMultiPlayer())
					{
						DLLUI->AddMessage(0, GetID(), true, GC.getEVENT_MESSAGE_TIME(), GetLocalizedText("TXT_KEY_MISC_TURN_BEGINS").GetCString(), "AS2D_NEWTURN", MESSAGE_TYPE_DISPLAY_ONLY);
					}
				}

				doWarnings();
			}

			if(GetID() == kGame.getActivePlayer())
			{
				GetUnitCycler().Rebuild();

				if(DLLUI->GetLengthSelectionList() == 0)
				{
					DLLUI->setCycleSelectionCounter(1);
				}

#ifndef REMOVE_EXCESS_CAMERA_CENTERING
				DLLUI->setDirty(SelectionCamera_DIRTY_BIT, true);
#endif


				DLLUI->PublishActivePlayerTurnStart();
			}
			else if(isHuman() && kGame.isGameMultiPlayer())
			{
				DLLUI->PublishRemotePlayerTurnStart();
			}
		}





		else
		{
			CvAssertFmt(GetEndTurnBlockingType() == NO_ENDTURN_BLOCKING_TYPE, "Expecting the end-turn blocking to be NO_ENDTURN_BLOCKING_TYPE, got %d", GetEndTurnBlockingType());
			SetEndTurnBlocking(NO_ENDTURN_BLOCKING_TYPE, -1);

			DoUnitReset();

			if(!isHuman())
			{
				RespositionInvalidUnits();
			}

			if(GetNotifications())
			{
				GetNotifications()->EndOfTurnCleanup();
			}

			if(GetDiplomacyRequests())
			{
				GetDiplomacyRequests()->EndTurn();
			}

#if defined(NQM_UNIT_FIX_FORTIFY_BONUS_RECEIVED_END_OF_TURN_NOT_INSTANTLY) || defined(NQM_UNIT_NO_AA_INTERCEPT_AFTER_MOVE_BEFORE_TURN_END) || defined(NQM_UNIT_FIGHTER_NO_INTERCEPT_UNTIL_AFTER_TURN_END)
			int iLoop;
			for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
			{
#ifdef NQM_UNIT_FIX_FORTIFY_BONUS_RECEIVED_END_OF_TURN_NOT_INSTANTLY

				if (pLoopUnit->IsFortifiedThisTurn())
				{
					pLoopUnit->changeFortifyTurns(1);
				}
#endif
#if defined(NQM_UNIT_NO_AA_INTERCEPT_AFTER_MOVE_BEFORE_TURN_END) || defined(NQM_UNIT_FIGHTER_NO_INTERCEPT_UNTIL_AFTER_TURN_END)
				pLoopUnit->setIsInterceptBlockedUntilEndTurn(false);
#endif
			}
#endif

			if(GetID() == kGame.getActivePlayer())
			{
				DLLUI->PublishActivePlayerTurnEnd();
			}

			if(!isHuman() || (isHuman() && !isAlive()) || (isHuman() && gDLL->HasReceivedTurnAllComplete(GetID())) || kGame.getAIAutoPlay())
				kGame.changeNumGameTurnActive(-1, std::string("setTurnActive() for player ") + getName());

			DLLUI->PublishPlayerTurnStatus(DLLUIClass::TURN_END, GetID());
		}
	}
	else
	{
		CvString logOutput;
		logOutput.Format("SetTurnActive() called without changing the end turn status. Player(%i) OldTurnActive(%i) NewTurnActive(%i)", GetID(), isTurnActive(), bNewValue);
		gDLL->netMessageDebugLog(logOutput);
	}
}


bool CvPlayer::isSimultaneousTurns() const
{
#ifdef AUI_GAME_BETTER_HYBRID_MODE
	return (GC.getGame().isAnySimultaneousTurns() && isHuman());
#else
	if(GC.getGame().isOption(GAMEOPTION_DYNAMIC_TURNS))
	{
		return m_bDynamicTurnsSimultMode;
	}
	else if(GC.getGame().isOption(GAMEOPTION_SIMULTANEOUS_TURNS))
	{
		return true;
	}

	return false;
#endif
}



#ifdef AUI_GAME_BETTER_HYBRID_MODE
int CvPlayer::getTurnOrder() const
{
	return m_iTurnOrder;
}



void CvPlayer::setTurnOrder(int iTurnOrder)
{
	m_iTurnOrder = iTurnOrder;
}
#else
void CvPlayer::setDynamicTurnsSimultMode(bool simultaneousTurns)
{
	if(simultaneousTurns != m_bDynamicTurnsSimultMode)
	{
		CvNotifications* pNotifications = GetNotifications();
		if (pNotifications)
		{
			NotificationTypes notifyType = NOTIFICATION_TURN_MODE_SEQUENTIAL;
			Localization::String localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_TURN_MODE_SEQUENTIAL");
			Localization::String localizedTextSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_TURN_MODE_SEQUENTIAL_SUMMARY");
			if(simultaneousTurns)
			{
				notifyType = NOTIFICATION_TURN_MODE_SIMULTANEOUS;
				localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_TURN_MODE_SIMULTANEOUS");
				localizedTextSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_TURN_MODE_SIMULTANEOUS_SUMMARY");
			}

			pNotifications->Add(notifyType, localizedText.toUTF8(), localizedTextSummary.toUTF8(), -1, -1, -1);
		}

		m_bDynamicTurnsSimultMode = simultaneousTurns;
	}
	
}
#endif


bool CvPlayer::isAutoMoves() const
{
	return m_bAutoMoves;
}


void CvPlayer::setAutoMoves(bool bNewValue)
{
	if(isAutoMoves() != bNewValue)
	{
		m_bAutoMoves = bNewValue;
		m_bProcessedAutoMoves = false;
	}
}


bool CvPlayer::hasProcessedAutoMoves() const
{
	return m_bProcessedAutoMoves;
}


void CvPlayer::setProcessedAutoMoves(bool bNewValue)
{
	if(hasProcessedAutoMoves() != bNewValue)
	{
		m_bProcessedAutoMoves = bNewValue;
	}
}


bool CvPlayer::isEndTurn() const
{
	return m_bEndTurn;
}


void CvPlayer::setEndTurn(bool bNewValue)
{
	CvGame& game = GC.getGame();

#ifdef AUI_GAME_BETTER_HYBRID_MODE
	if (bNewValue && isSimultaneousTurns() && !game.isAllActivePlayersTurnAllComplete())
#else
	if(isSimultaneousTurns()
		&& bNewValue 
		&& game.isNetworkMultiPlayer() 
		&& !gDLL->HasReceivedTurnAllCompleteFromAllPlayers())
#endif
	{

		return;
	}




	if(!isEndTurn() && isHuman() && GetID() != game.getActivePlayer())
	{
		if(hasBusyUnitOrCity() || (!gDLL->HasReceivedTurnComplete(GetID()) && hasReadyUnit()))
		{
			return;
		}
	}
	else if(!isHuman())
	{
		if(hasBusyUnitOrCity())
		{
			return;
		}
	}

	if(isEndTurn() != bNewValue)
	{

		if(!GC.getGame().isGameMultiPlayer())
		{









		}

		CvAssertMsg(isTurnActive(), "isTurnActive is expected to be true");

		m_bEndTurn = bNewValue;

		if(isEndTurn())
		{
#ifdef AUI_GAME_BETTER_HYBRID_MODE
			if (game.isAnySimultaneousTurns())
#else
			if(!GC.getGame().isOption(GAMEOPTION_DYNAMIC_TURNS) && GC.getGame().isOption(GAMEOPTION_SIMULTANEOUS_TURNS))
#endif
			{
				checkRunAutoMovesForEveryone();
			}
			else
			{
				setAutoMoves(true);
			}
		}
		else
			setAutoMoves(false);
	}
	else
	{


		if(!bNewValue && isAutoMoves())
			setAutoMoves(false);
	}
}


void CvPlayer::checkRunAutoMovesForEveryone()
{
	bool runAutoMovesForEveryone = true;
	int i = 0;
	for(i = 0; i < MAX_PLAYERS; ++i)
	{
		CvPlayer& p = CvPlayerAI::getPlayer((PlayerTypes)i);
		if(p.isHuman() && !p.isObserver() 






			&& (!p.isEndTurn()
#ifdef AUI_GAME_BETTER_HYBRID_MODE
				&& (!GC.getGame().isAllActivePlayersTurnAllComplete() || p.isTurnActive())))
#else
			&& (!gDLL->HasReceivedTurnAllCompleteFromAllPlayers() || p.isTurnActive()))) 
#endif
		{
			runAutoMovesForEveryone = false;
			break;
		}
	}

	if(runAutoMovesForEveryone)
	{
		for(i = 0; i < MAX_PLAYERS; ++i)
		{
			CvPlayer& p = CvPlayerAI::getPlayer((PlayerTypes)i);
#ifdef AUI_GAME_BETTER_HYBRID_MODE
			if (p.isHuman() && p.getTurnOrder() == GC.getGame().getCurrentTurnOrderActive())
#else
			if(p.isHuman())
#endif
			{
				p.setAutoMoves(true);
			}
		}
	}
}


EndTurnBlockingTypes CvPlayer::GetEndTurnBlockingType(void) const
{
	return m_eEndTurnBlockingType;
}


int CvPlayer::GetEndTurnBlockingNotificationIndex(void) const
{
	return m_iEndTurnBlockingNotificationIndex;
}


void CvPlayer::SetEndTurnBlocking(EndTurnBlockingTypes eBlockingType, int iNotificationIndex)
{
	bool bFireEvent = false;
	if(m_eEndTurnBlockingType != eBlockingType || m_iEndTurnBlockingNotificationIndex != iNotificationIndex)
	{
		bFireEvent = true;
	}

	EndTurnBlockingTypes ePrevBlockingType = m_eEndTurnBlockingType;
	m_eEndTurnBlockingType = eBlockingType;
	m_iEndTurnBlockingNotificationIndex = iNotificationIndex;

	if(bFireEvent)
	{
		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->SetEndTurnBlockingChanged( ePrevBlockingType, m_eEndTurnBlockingType );
			GC.GetEngineUserInterface()->UpdateEndTurn();
		}
	}
}


bool CvPlayer::isTurnDone() const
{

	if(!GC.getGame().isPbem() && !GC.getGame().isHotSeat())
	{
		return false;
	}
	if(!isHuman())
	{
		return true;
	}
	if(!isEndTurn())
	{
		return false;
	}
	return (!isAutoMoves());
}


bool CvPlayer::isExtendedGame() const
{
	return m_bExtendedGame;
}



void CvPlayer::makeExtendedGame()
{
	m_bExtendedGame = true;
}



bool CvPlayer::isFoundedFirstCity() const
{
	return m_bFoundedFirstCity;
}



void CvPlayer::setFoundedFirstCity(bool bNewValue)
{
	if(isFoundedFirstCity() != bNewValue)
	{
		m_bFoundedFirstCity = bNewValue;

		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(PercentButtons_DIRTY_BIT, true);
			GC.GetEngineUserInterface()->setDirty(ResearchButtons_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::GetNumCitiesFounded() const
{
	return m_iNumCitiesFounded;
}


void CvPlayer::ChangeNumCitiesFounded(int iValue)
{
	m_iNumCitiesFounded += iValue;
}



void CvPlayer::CheckForMurder(PlayerTypes ePossibleVictimPlayer)
{



	CvPlayer& kPossibleVictimPlayer = GET_PLAYER(ePossibleVictimPlayer);
	bool bPossibileVictimIsHuman = kPossibleVictimPlayer.isHuman();


	kPossibleVictimPlayer.verifyAlive();


	if(!kPossibleVictimPlayer.isAlive())
	{
		GET_TEAM(kPossibleVictimPlayer.getTeam()).SetKilledByTeam(getTeam());
		kPossibleVictimPlayer.SetEverConqueredBy(m_eID, true);


		if(!CvPreGame::isNetworkMultiplayerGame())
		{
			if(!bPossibileVictimIsHuman && !kPossibleVictimPlayer.isMinorCiv() && !kPossibleVictimPlayer.isBarbarian())
				kPossibleVictimPlayer.GetDiplomacyAI()->DoKilledByPlayer(GetID());
		}


		if (!kPossibleVictimPlayer.isMinorCiv())
		{
			for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
			{
				PlayerTypes eCleanupPlayer = (PlayerTypes)ui;
				GET_PLAYER(eCleanupPlayer).GetDiplomacyAI()->KilledPlayerCleanup(kPossibleVictimPlayer.GetID());
			}
		}
	}
}


bool CvPlayer::isStrike() const
{
	return m_bStrike;
}



void CvPlayer::setStrike(bool bNewValue)
{
	if(m_bStrike != bNewValue)
	{
		m_bStrike = bNewValue;

		if(m_bStrike)
		{
			if(GetID() == GC.getGame().getActivePlayer())
			{
				GC.GetEngineUserInterface()->AddMessage(0, GetID(), false, GC.getEVENT_MESSAGE_TIME(), GetLocalizedText("TXT_KEY_MISC_UNITS_ON_STRIKE").GetCString(), "AS2D_STRIKE", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));

				GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
			}
		}
	}
}




bool CvPlayer::IsCramped() const
{
	return m_bCramped;
}



void CvPlayer::DoUpdateCramped()
{
	CvCity* pLoopCity;
	CvPlot* pPlot;

	int iTotalPlotsNearby = 0;
	int iUsablePlotsNearby = 0;

	int iRange = GC.getCRAMPED_RANGE_FROM_CITY();

	int iLoop;
	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
#ifdef AUI_HEXSPACE_DX_LOOPS
		for (int iDY = -iRange; iDY <= iRange; iDY++)
		{
			int iMaxDX = iRange - MAX(0, iDY);
			for (int iDX = -iRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
			{
				pPlot = plotXY(pLoopCity->getX(), pLoopCity->getY(), iDX, iDY);
#else
		for(int iX = -iRange; iX <= iRange; iX++)
		{
			for(int iY = -iRange; iY <= iRange; iY++)
			{
				pPlot = plotXYWithRangeCheck(pLoopCity->getX(), pLoopCity->getY(), iX, iY, iRange);
#endif

				if(pPlot != NULL)
				{

					if(!pPlot->isOwned() || pPlot->getOwner() != GetID())
					{
						iTotalPlotsNearby++;


						if(!pPlot->isOwned() && !pPlot->isImpassable() && !pPlot->isMountain() && !pPlot->isWater())
						{
							iUsablePlotsNearby++;
						}
					}
				}
			}
		}
	}

	if(iTotalPlotsNearby > 0)
	{
		if(100 * iUsablePlotsNearby / iTotalPlotsNearby <= GC.getCRAMPED_USABLE_PLOT_PERCENT())
		{
			m_bCramped = true;
		}
		else
		{
			m_bCramped = false;
		}
	}
}


CvHandicapInfo& CvPlayer::getHandicapInfo() const
{
	CvHandicapInfo* pkHandicapInfo = GC.getHandicapInfo(getHandicapType());
	if(pkHandicapInfo == NULL)
	{
		const char* szError = "ERROR: Player does not contain valid handicap!!";
		GC.LogMessage(szError);
		CvAssertMsg(false, szError);
	}

#pragma warning ( push )
#pragma warning ( disable : 6011 )
	return *pkHandicapInfo;
#pragma warning ( pop )
}


HandicapTypes CvPlayer::getHandicapType() const
{
	return CvPreGame::handicap(GetID());
}


CvCivilizationInfo& CvPlayer::getCivilizationInfo() const
{
	CvCivilizationInfo* pkCivilizationInfo = GC.getCivilizationInfo(getCivilizationType());
	if(pkCivilizationInfo == NULL)
	{
		const char* szError = "ERROR: Player does not contain valid civilization type!!";
		GC.LogMessage(szError);
		CvAssertMsg(false, szError);
	}

#pragma warning ( push )
#pragma warning ( disable : 6011 )
	return *pkCivilizationInfo;
#pragma warning ( pop )
}


CivilizationTypes CvPlayer::getCivilizationType() const
{
	return CvPreGame::civilization(GetID());
}



CvLeaderHeadInfo& CvPlayer::getLeaderInfo() const
{
	CvLeaderHeadInfo* pkLeaderInfo = GC.getLeaderHeadInfo(getLeaderType());
	if(pkLeaderInfo == NULL)
	{
		const char* szError = "ERROR: Player does not contain valid leader type!!";
		GC.LogMessage(szError);
		CvAssertMsg(false, szError);
	}

#pragma warning ( push )
#pragma warning ( disable : 6011 )
	return *pkLeaderInfo;
#pragma warning ( pop )
}


LeaderHeadTypes CvPlayer::getLeaderType() const
{
	return CvPreGame::leaderHead(GetID());
}



LeaderHeadTypes CvPlayer::getPersonalityType() const
{
	return m_ePersonalityType;
}



void CvPlayer::setPersonalityType(LeaderHeadTypes eNewValue)
{
	m_ePersonalityType = eNewValue;
}



EraTypes CvPlayer::GetCurrentEra() const
{
	return GET_TEAM(getTeam()).GetCurrentEra();
}


void CvPlayer::setTeam(TeamTypes eTeam)
{
	CvAssert(eTeam != NO_TEAM);
	CvAssert(getTeam() != NO_TEAM);

	GET_TEAM(getTeam()).changeNumMembers(-1);
	if(isAlive())
	{
		GET_TEAM(getTeam()).changeAliveCount(-1);
	}
	if(isEverAlive())
	{
		GET_TEAM(getTeam()).changeEverAliveCount(-1);
	}
	GET_TEAM(getTeam()).changeNumCities(-(getNumCities()));
	GET_TEAM(getTeam()).changeTotalPopulation(-(getTotalPopulation()));
	GET_TEAM(getTeam()).changeTotalLand(-(getTotalLand()));

	CvPreGame::setTeamType(GetID(), eTeam);

	GET_TEAM(getTeam()).changeNumMembers(1);
	if(isAlive())
	{
		GET_TEAM(getTeam()).changeAliveCount(1);
	}
	if(isEverAlive())
	{
		GET_TEAM(getTeam()).changeEverAliveCount(1);
	}
	GET_TEAM(getTeam()).changeNumCities(getNumCities());
	GET_TEAM(getTeam()).changeTotalPopulation(getTotalPopulation());
	GET_TEAM(getTeam()).changeTotalLand(getTotalLand());
}


bool CvPlayer::IsAITeammateOfHuman() const
{
	bool bRtnValue = false;

	for(int i = 0; i < MAX_PLAYERS; ++i)
	{
		CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));
		if(player.isHuman() && player.isAlive())
		{
			if(player.getTeam() == getTeam())
			{
				bRtnValue = true;
				break;
			}
		}
	}

	return bRtnValue;
}


PlayerColorTypes CvPlayer::getPlayerColor() const
{
	return CvPreGame::playerColor(GetID());
}


const CvColorA& CvPlayer::getPlayerTextColor() const
{
	CvAssertMsg(getPlayerColor() != NO_PLAYERCOLOR, "getPlayerColor() is not expected to be equal with NO_PLAYERCOLOR");
	CvPlayerColorInfo* pkPlayerColorInfo = GC.GetPlayerColorInfo(getPlayerColor());
	CvColorInfo* pkColorInfo = NULL;
	if(pkPlayerColorInfo)
	{
		ColorTypes eTextColor = static_cast<ColorTypes>(pkPlayerColorInfo->GetColorTypeText());
		pkColorInfo = GC.GetColorInfo(eTextColor);
		if(pkColorInfo)
			return pkColorInfo->GetColor();
	}


	static CvColorA black(0,0,0,1.0f);
	return black;
}


int CvPlayer::getSeaPlotYield(YieldTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiSeaPlotYield[eIndex];
}



void CvPlayer::changeSeaPlotYield(YieldTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_aiSeaPlotYield.setAt(eIndex, m_aiSeaPlotYield[eIndex] + iChange);

		updateYield();
	}
}



int CvPlayer::getYieldRateModifier(YieldTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiYieldRateModifier[eIndex];
}



void CvPlayer::changeYieldRateModifier(YieldTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_aiYieldRateModifier.setAt(eIndex, m_aiYieldRateModifier[eIndex] + iChange);

		invalidateYieldRankCache(eIndex);

		if(getTeam() == GC.getGame().getActiveTeam())
		{
			GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::GetImprovementExtraYield(ImprovementTypes eImprovement, YieldTypes eYield) const
{
	CvAssertMsg(eImprovement >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eImprovement < GC.getNumFeatureInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eYield >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppiImprovementYieldChange[eImprovement][eYield];
}


void CvPlayer::ChangeImprovementExtraYield(ImprovementTypes eImprovement, YieldTypes eYield, int iChange)
{
	CvAssertMsg(eImprovement >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eImprovement < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eYield >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppiImprovementYieldChange[eImprovement];
		yields[eYield] = (m_ppiImprovementYieldChange[eImprovement][eYield] + iChange);
		m_ppiImprovementYieldChange[eImprovement] = yields;
		CvAssert(GetImprovementExtraYield(eImprovement, eYield) >= 0);

		updateYield();
	}
}


int CvPlayer::getCapitalYieldRateModifier(YieldTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCapitalYieldRateModifier[eIndex];
}



void CvPlayer::changeCapitalYieldRateModifier(YieldTypes eIndex, int iChange)
{
	CvCity* pCapitalCity;

	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_aiCapitalYieldRateModifier.setAt(eIndex, m_aiCapitalYieldRateModifier[eIndex] + iChange);

		invalidateYieldRankCache(eIndex);

		pCapitalCity = getCapitalCity();
	}
}

int CvPlayer::getExtraYieldThreshold(YieldTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiExtraYieldThreshold[eIndex];
}



void CvPlayer::updateExtraYieldThreshold(YieldTypes eIndex)
{
	int iBestValue;

	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	iBestValue = GetPlayerTraits()->GetExtraYieldThreshold(eIndex);

	if(getExtraYieldThreshold(eIndex) != iBestValue)
	{
		m_aiExtraYieldThreshold.setAt(eIndex, iBestValue);
		CvAssert(getExtraYieldThreshold(eIndex) >= 0);

		updateYield();
	}
}
#if defined(STANDARDIZE_YIELDS)
int CvPlayer::getYieldTimes100(YieldTypes eYield, bool bForReligion) const
{
	int yield = 0;
	const CvYieldInfo& kYield = *GC.getYieldInfo(eYield);

	yield += getYieldFromCitiesTimes100(eYield, false);
	yield += getYieldFromOtherPlayersTimes100(eYield);
	yield += getYieldFromHappinessTimes100(eYield);
	yield += getYieldFromTraitsTimes100(eYield);
	yield += GetYieldPerTurnFromMisc(eYield) * 100;
	if (bForReligion)
		return yield;
	yield += getYieldFromReligionTimes100(eYield, yield);

	if (GetCultureBonusTurns() > 0 && YIELD_CULTURE == eYield)
	{
		yield *= (100 + GC.getTEMPORARY_CULTURE_BOOST_MOD());
		yield /= 100;
	}

	int iModifier = 100;
	if (isGoldenAge())
	{
#if defined(LEKMOD_PLAYER_GOLDEN_AGE_YIELD_MOD_INFO)
		iModifier += kYield.getPlayerGoldenAgeYieldMod();
#else
		iModifier += GC.getGOLDEN_AGE_CULTURE_MODIFIER();
#endif
	}
	yield *= iModifier;
	yield /= 100;

	yield += getYieldPenaltiesTimes100(eYield);
	yield += getYieldFromMinorCivsTimes100(eYield);

	return yield;
}
int CvPlayer::getYieldFromCitiesTimes100(YieldTypes eYield, bool bIgnoreTrade) const
{
	int yield = 0;
	const CvCity* pLoopCity;
	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		yield += pLoopCity->getYieldRateTimes100(eYield, bIgnoreTrade);
	}

	return yield;
}
int CvPlayer::getYieldFromOtherPlayersTimes100(YieldTypes eYield) const
{
	int yield = 0;

	if (YIELD_SCIENCE == eYield)
	{
		int iScience = getYieldFromCitiesTimes100(eYield, false);

		int iResearchAgreementBonus =       GC.getRESEARCH_AGREEMENT_MOD() * GET_TEAM(getTeam()).GetTotalNumResearchAgreements();
		iScience *= iResearchAgreementBonus;
		iScience /= 100;
		yield += iScience;
	}

	return yield;

}
int CvPlayer::getYieldFromHappinessTimes100(YieldTypes eYield) const
{
	if (GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return 0;
	}
	int iExcessHappiness = GetExcessHappiness();
	switch (eYield)
	{
	case YIELD_SCIENCE:
		if (iExcessHappiness >= 0)
		{
			int iFreeScience = getYieldFromCitiesTimes100(eYield, false) * getHappinessToScience();
			iFreeScience /= 100;
			return iFreeScience;
		}
	break;
	case YIELD_CULTURE:
		if (iExcessHappiness > 0)
		{
			int iFreeCulture = (iExcessHappiness * getHappinessToCulture());
			return iFreeCulture;
		}
	break;
	case YIELD_GOLDEN_AGE_POINTS:
	{
		return iExcessHappiness * 100;
	}
	default:
		return 0;
		break;
	}
	return 0;
}
int CvPlayer::getYieldPenaltiesTimes100(YieldTypes eYield) const
{
	if (YIELD_SCIENCE != eYield)
		return 0;
	int iGoldPerTurn = calculateGoldRateTimes100();
	int iScience = 0;
	if (GetTreasury()->GetGoldTimes100() + iGoldPerTurn < 0)
	{
		iScience += (GetTreasury()->GetGoldTimes100() + iGoldPerTurn);
	}
	return iScience;
}
int CvPlayer::getYieldFromMinorCivsTimes100(YieldTypes eYield) const
{
	int yield = 0;
	PlayerTypes ePlayer;
	for (int iPlayerLoop = MAX_MAJOR_CIVS; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		ePlayer = static_cast<PlayerTypes>(iPlayerLoop);
		const CvPlayer& kMinor = GET_PLAYER(ePlayer);
		if (!kMinor.isAlive() || !kMinor.isMinorCiv())
			continue;
		switch (eYield)
		{
			case YIELD_SCIENCE:
				yield += kMinor.GetMinorCivAI()->GetCurrentScienceFriendshipBonusTimes100(GetID());
				break;
			case YIELD_CULTURE:
				yield += GetCulturePerTurnFromMinor(ePlayer) * 100;
				break;
			case YIELD_FAITH:
				yield += GetFaithPerTurnFromMinor(ePlayer) * 100;
				break;
		}
	}
	
	return yield;
}
int CvPlayer::getYieldFromTraitsTimes100(YieldTypes eYield) const
{
	int iYield = GetPlayerTraits()->GetYieldChangePerTradePartnerByDomain(DOMAIN_LAND, eYield);
	if (iYield > 0)
	{
		iYield += GetPlayerTraits()->GetTradePartnerYieldFlatBonusPerEra(eYield) * GetCurrentEra();
		iYield *= GetTrade()->GetNumDifferentTradingPartners();
	}
	return iYield;
}
int CvPlayer::getYieldFromReligionTimes100(YieldTypes eYield, int iPrevTotal) const
{
	int rtnValue = 0;

	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	ReligionTypes eFoundedReligion = pReligions->GetFounderBenefitsReligion(GetID());
	if (eFoundedReligion != NO_RELIGION)
	{
		const CvReligion* pReligion = pReligions->GetReligion(eFoundedReligion, NO_PLAYER);
		if (pReligion)
		{
			rtnValue += pReligion->m_Beliefs.GetHolyCityYieldChange(eYield);

			int iTemp = pReligion->m_Beliefs.GetYieldChangePerForeignCity(eYield);
			if (iTemp > 0)
			{
				rtnValue += (iTemp * GetReligions()->GetNumForeignCitiesFollowing());
			}
#if defined(LEKMOD_BELIEF_YIELDIFY)
			iTemp = pReligion->m_Beliefs.GetYieldChangePerFollowingCity(eYield);
			if (iTemp > 0)
			{
				rtnValue += (iTemp * pReligions->GetNumCitiesFollowing(eFoundedReligion));
			}
#endif
#if defined(LEK_CULTURE_SCIENCE_SPREAD_BELIEFS_ALL_CITIES)
			iTemp = pReligion->m_Beliefs.GetYieldChangePerXFollowers(eYield);
			if (iTemp > 0)
			{
				int iFollowers = pReligions->GetNumFollowers(eFoundedReligion);
				if (iFollowers > 0)
				{
					rtnValue += (iFollowers / iTemp);
				}
			}
#endif
			iTemp = pReligion->m_Beliefs.GetYieldChangePerXForeignFollowers(eYield);
			if (iTemp > 0)
			{
				int iFollowers = GetReligions()->GetNumForeignFollowers(false             );
				if (iFollowers > 0)
				{
					rtnValue += (iFollowers / iTemp);
				}
			}
			rtnValue *= 100;
			bool bAtPeace = GET_TEAM(getTeam()).getAtWarCount(false) == 0;
#if !defined(LEKMOD_BELIEF_YIELDIFY)
			int iMod = pReligion->m_Beliefs.GetPlayerCultureModifier(bAtPeace);
#else 
			int iMod = pReligion->m_Beliefs.GetPlayerYieldModifier(eYield, bAtPeace);
#endif
			if (iMod != 0)
			{
				rtnValue += (rtnValue + iPrevTotal) * iMod / 100;
			}
		}
	}
	return rtnValue;
}
#endif

int CvPlayer::GetScience() const
{
	return GetScienceTimes100() / 100;
}


int CvPlayer::GetScienceTimes100() const
{

	if(IsAnarchy())
		return 0;

#if !defined(STANDARDIZE_YIELDS)
	int iValue = 0;


	iValue += GetScienceFromCitiesTimes100(false);


	iValue += GetScienceFromOtherPlayersTimes100();


	iValue += GetScienceFromHappinessTimes100();

#ifdef NQ_GOLD_TO_SCIENCE_FROM_POLICIES

	iValue += GetScienceFromGoldTimes100();
#endif


	iValue += GetScienceFromResearchAgreementsTimes100();


	iValue += GetScienceFromBudgetDeficitTimes100();

	return max(iValue, 0);
#else
	return max(getYieldTimes100(YIELD_SCIENCE, false                 ), 0);
#endif
}




int CvPlayer::GetScienceFromCitiesTimes100(bool bIgnoreTrade) const
{
	int iScience = 0;

	const CvCity* pLoopCity;

	int iLoop;
	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iScience += pLoopCity->getYieldRateTimes100(YIELD_SCIENCE, bIgnoreTrade);
	}

	return iScience;
}



int CvPlayer::GetScienceFromOtherPlayersTimes100() const
{
	int iScience = 0;

	PlayerTypes ePlayer;
	int iScienceFromPlayer;
#if !defined(LEKMOD_FIX_SCHOLASTICISM)
	for (int iPlayerLoop = MAX_MAJOR_CIVS; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes)iPlayerLoop;

		iScienceFromPlayer = 0;


		if (IsGetsScienceFromPlayer(ePlayer))
		{
			if (GET_PLAYER(ePlayer).GetMinorCivAI()->IsAllies(GetID()))
			{
				switch ((int)GetCurrentEra())
				{
				case 0:
					iScienceFromPlayer = 0;
					break;
				case 1:
					iScienceFromPlayer = 100;
					break;
				case 2:
					iScienceFromPlayer = 300;
					break;
				case 3:
					iScienceFromPlayer = 600;
					break;
				case 4:
					iScienceFromPlayer = 1000;
					break;
				case 5:
					iScienceFromPlayer = 1500;
					break;
				case 6:
					iScienceFromPlayer = 2100;
					break;
				case 7:
					iScienceFromPlayer = 2800;
					break;
				}
			}
			else if (GET_PLAYER(ePlayer).GetMinorCivAI()->IsFriends(GetID()))
			{
				iScienceFromPlayer = GetCurrentEra() * 100;
			}



			iScience += iScienceFromPlayer;
		}
	}
#else
	iScienceFromPlayer = 0;
	for (int iPlayerLoop = MAX_MAJOR_CIVS; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes)iPlayerLoop;
		const CvPlayer& kMinor = GET_PLAYER(ePlayer);
		if (!kMinor.isAlive() || !kMinor.isMinorCiv())
			continue;
		iScienceFromPlayer += GET_PLAYER(ePlayer).GetMinorCivAI()->GetCurrentScienceFriendshipBonusTimes100(GetID());
	}
	iScience += iScienceFromPlayer;
#endif



	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	ReligionTypes eFoundedReligion = pReligions->GetFounderBenefitsReligion(GetID());
	if(eFoundedReligion != NO_RELIGION)
	{
		const CvReligion* pReligion = pReligions->GetReligion(eFoundedReligion, NO_PLAYER);
		if (pReligion)
		{
			int iTemp = pReligion->m_Beliefs.GetYieldChangePerXForeignFollowers(YIELD_SCIENCE);
			if (iTemp > 0)
			{
#ifdef LEK_CULTURE_SCIENCE_SPREAD_BELIEFS_ALL_CITIES
				int iFollowers = pReligions->GetNumFollowers(eFoundedReligion);
#else
				int iFollowers = GetReligions()->GetNumForeignFollowers(false             );
#endif
				if (iFollowers > 0)
				{
					iScienceFromPlayer = (iFollowers / iTemp);
					iScience += iScienceFromPlayer * 100;
				}
			}
		}
	}


	return iScience;
}



#ifdef AUI_CITIZENS_CONSIDER_HAPPINESS_VALUE_ON_OTHER_YIELDS
int CvPlayer::GetScienceFromHappinessTimes100(bool bIgnoreHappinessRequirement) const
#else
int CvPlayer::GetScienceFromHappinessTimes100() const
#endif
{
	if(GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return 0;
	}

	int iScience = 0;

	if(getHappinessToScience() != 0)
	{
#ifdef AUI_CITIZENS_CONSIDER_HAPPINESS_VALUE_ON_OTHER_YIELDS
		if (!bIgnoreHappinessRequirement && GetExcessHappiness() >= 0)
#else
		if(GetExcessHappiness() >= 0)
#endif
		{
			int iFreeScience = GetScienceFromCitiesTimes100(false) * getHappinessToScience();
			iFreeScience /= 100;

			iScience += iFreeScience;
		}
	}

	return iScience;
}

#ifdef NQ_GOLD_TO_SCIENCE_FROM_POLICIES

int CvPlayer::GetScienceFromGoldTimes100() const
{
	int iScience = 0;
	int iGoldToScience = getGoldToScience();

	if(iGoldToScience != 0)
	{
		int iGoldPerTurn = calculateGoldRateTimes100();
		if(iGoldPerTurn > 0)
		{
			iScience += iGoldPerTurn * iGoldToScience / 100;
		}
	}

	return iScience;
}
#endif

#ifdef NQ_MINOR_FRIENDSHIP_GAIN_BULLY_GOLD_SUCCESS_FROM_POLICIES

int CvPlayer::GetMinorFriendshipGainBullyGoldSuccess() const
{
	int iInfluenceChange = GetPlayerPolicies()->GetNumericModifier(POLICYMOD_MINOR_FRIENDSHIP_GAIN_BULLY_GOLD_SUCCESS) / 100;
	return iInfluenceChange;
}
#endif



int CvPlayer::GetScienceFromResearchAgreementsTimes100() const
{
	int iScience = GetScienceFromCitiesTimes100(false);

	int iResearchAgreementBonus =       GC.getRESEARCH_AGREEMENT_MOD() * GET_TEAM(getTeam()).GetTotalNumResearchAgreements();
	iScience *= iResearchAgreementBonus;
	iScience /= 100;

	return iScience;
}



int CvPlayer::GetScienceFromBudgetDeficitTimes100() const
{
	int iScience = 0;
	
	int iMyNum = 0;
	if (iScience > 0)
	{
		iMyNum = -1;
	}


	int iGoldPerTurn = calculateGoldRateTimes100();
	if(GetTreasury()->GetGoldTimes100() + iGoldPerTurn < 0)
	{
		iScience += (GetTreasury()->GetGoldTimes100() + iGoldPerTurn);
	}

	return iScience;
}




int CvPlayer::GetScienceYieldFromPreviousTurns(int iGameTurn, int iNumPreviousTurnsToCount)
{

	int iSum = 0;
	for (int iI = 0; iI < iNumPreviousTurnsToCount; iI++)
	{
		int iTurn = iGameTurn - iI;
		if (iTurn < 0)
		{
			break;
		}

		int iTurnScience = getReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_SCIENCEPERTURN"), iTurn);
		if (iTurnScience >= 0)
		{
			iSum += iTurnScience;
		}
		else if (iTurnScience == -1)
		{
			iSum += (         GetScience());
		}
	}

	return iSum;
}


bool CvPlayer::IsGetsScienceFromPlayer(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= MAX_MAJOR_CIVS, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");


	if(IsMinorScienceAllies() && (GET_PLAYER(ePlayer).GetMinorCivAI()->IsAllies(GetID()) || GET_PLAYER(ePlayer).GetMinorCivAI()->IsFriends(GetID())))
	{
		return true;
	}

	return m_pabGetsScienceFromPlayer[ePlayer];
}


void CvPlayer::SetGetsScienceFromPlayer(PlayerTypes ePlayer, bool bNewValue)
{
	CvAssertMsg(ePlayer >= MAX_MAJOR_CIVS, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(bNewValue != m_pabGetsScienceFromPlayer[ePlayer])
	{
		m_pabGetsScienceFromPlayer.setAt(ePlayer, bNewValue);
	}
}



void CvPlayer::DoDeficit()
{
	int iNumMilitaryUnits = 0;

	CvUnit* pLoopUnit;
	int iLoop;
	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if(pLoopUnit->IsCombatUnit())
			iNumMilitaryUnits++;
	}


	if(iNumMilitaryUnits > getNumCities())
	{
		if(GC.getGame().getJonRandNum(100, "Disband rand") < 50)
		{
			UnitHandle pLandUnit;
			UnitHandle pNavalUnit;
			int iLandScore = MAX_INT;
			int iNavalScore = MAX_INT;


			if(GetMilitaryAI()->GetLandDefenseState() <= DEFENSE_STATE_NEUTRAL)
			{
				pLandUnit = GetMilitaryAI()->FindBestUnitToScrap(true          , true                          , iLandScore);
			}


			if(GetMilitaryAI()->GetNavalDefenseState() <= DEFENSE_STATE_NEUTRAL)
			{
				pNavalUnit = GetMilitaryAI()->FindBestUnitToScrap(false          , true                          , iNavalScore);
			}

			if(iLandScore < MAX_INT && (GetMilitaryAI()->GetLandDefenseState() <= GetMilitaryAI()->GetNavalDefenseState() || iLandScore <= iNavalScore))
			{
				if(pLandUnit)
				{
					CvNotifications* pNotifications = GetNotifications();
					if(pNotifications)
					{
						Localization::String locString = Localization::Lookup("TXT_KEY_NTFN_UNIT_DISBANDED");
						Localization::String locSummary = Localization::Lookup("TXT_KEY_NTFN_UNIT_DISBANDED_S");
						pNotifications->Add(NOTIFICATION_UNIT_DIED, locString.toUTF8(), locSummary.toUTF8(), pLandUnit->getX(), pLandUnit->getY(), pLandUnit->getUnitType(), GetID());
					}

					pLandUnit->scrap();
					GetMilitaryAI()->LogDeficitScrapUnit(pLandUnit);
				}
			}
			else if(iNavalScore < MAX_INT)
			{
				if(pNavalUnit)
				{
					CvNotifications* pNotifications = GetNotifications();
					if(pNotifications)
					{
						Localization::String locString = Localization::Lookup("TXT_KEY_NTFN_UNIT_DISBANDED");
						Localization::String locSummary = Localization::Lookup("TXT_KEY_NTFN_UNIT_DISBANDED_S");
						pNotifications->Add(NOTIFICATION_UNIT_DIED, locString.toUTF8(), locSummary.toUTF8(), pNavalUnit->getX(), pNavalUnit->getY(), pNavalUnit->getUnitType(), GetID());
					}

					pNavalUnit->scrap();
					GetMilitaryAI()->LogDeficitScrapUnit(pNavalUnit);
				}
			}
		}
	}
}


int CvPlayer::getSpecialistExtraYield(YieldTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiSpecialistExtraYield[eIndex];
}



void CvPlayer::changeSpecialistExtraYield(YieldTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{


		CvCity* pLoopCity;
		int iLoop;
		int iNumTotalSpecialists = 0;

		for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
#ifdef AUI_WARNING_FIXES
			for (uint iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
#else
			for(int iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
#endif
			{
				iNumTotalSpecialists = pLoopCity->GetCityCitizens()->GetSpecialistCount((SpecialistTypes) iSpecialistLoop);


				for(int iTempLoop = 0; iTempLoop < iNumTotalSpecialists; iTempLoop++)
				{
					pLoopCity->processSpecialist((SpecialistTypes) iSpecialistLoop, -1);
				}
			}
		}

		m_aiSpecialistExtraYield.setAt(eIndex ,m_aiSpecialistExtraYield[eIndex] + iChange);
		CvAssert(getSpecialistExtraYield(eIndex) >= 0);

		updateYield();


		for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
#ifdef AUI_WARNING_FIXES
			for (uint iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
#else
			for(int iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
#endif
			{
				iNumTotalSpecialists = pLoopCity->GetCityCitizens()->GetSpecialistCount((SpecialistTypes) iSpecialistLoop);


				for(int iTempLoop = 0; iTempLoop < iNumTotalSpecialists; iTempLoop++)
				{
					pLoopCity->processSpecialist((SpecialistTypes) iSpecialistLoop, 1);
				}
			}
		}
	}
}



PlayerProximityTypes CvPlayer::GetProximityToPlayer(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return (PlayerProximityTypes) m_aiProximityToPlayer[ePlayer];
}



void CvPlayer::SetProximityToPlayer(PlayerTypes ePlayer, PlayerProximityTypes eProximity)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	CvAssertMsg(GetID() != ePlayer, "Trying to calculate proximity to oneself. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	CvAssertMsg(eProximity >= NO_PLAYER_PROXIMITY, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eProximity < NUM_PLAYER_PROXIMITIES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if((GC.getLogging() && GC.getAILogging()))
	{
		if(eProximity != m_aiProximityToPlayer[ePlayer])
		{

			CvString strFileName = "PlayerProximityLog.csv";
			FILogFile* pLog;
			pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);
			CvString strLog, strTemp;

			CvString strPlayerName;
			strPlayerName = getCivilizationShortDescription();
			strLog += strPlayerName;
			strLog += ",";

			strTemp.Format("%d,", GC.getGame().getGameTurn());
			strLog += strTemp;
			CvString strOtherPlayerName;
			strOtherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
			strLog += strOtherPlayerName;
			strLog += ",";

			switch(m_aiProximityToPlayer[ePlayer])
			{
			case NO_PLAYER_PROXIMITY:
				strLog += "No player proximity,";
				break;
			case PLAYER_PROXIMITY_NEIGHBORS:
				strLog += "Neighbors,";
				break;
			case PLAYER_PROXIMITY_CLOSE:
				strLog += "Close,";
				break;
			case PLAYER_PROXIMITY_FAR:
				strLog += "Far,";
				break;
			case PLAYER_PROXIMITY_DISTANT:
				strLog += "Distant,";
				break;
			}

			strLog += "-->,";

			switch(eProximity)
			{
			case NO_PLAYER_PROXIMITY:
				strLog += "No player proximity,";
				break;
			case PLAYER_PROXIMITY_NEIGHBORS:
				strLog += "Neighbors,";
				break;
			case PLAYER_PROXIMITY_CLOSE:
				strLog += "Close,";
				break;
			case PLAYER_PROXIMITY_FAR:
				strLog += "Far,";
				break;
			case PLAYER_PROXIMITY_DISTANT:
				strLog += "Distant,";
				break;
			}

			pLog->Msg(strLog);
		}
	}

	m_aiProximityToPlayer.setAt(ePlayer, eProximity);
}



void CvPlayer::DoUpdateProximityToPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	int iSmallestDistanceBetweenCities = GC.getMap().numPlots();
	int iAverageDistanceBetweenCities = 0;

	int iNumCityConnections = 0;

	CvCity* pLoopMyCity;
	CvCity* pLoopTheirCity;

	int iMyCityLoop;
	int iTheirCityLoop;

	int iTempDistance;


	for(pLoopMyCity = firstCity(&iMyCityLoop); pLoopMyCity != NULL; pLoopMyCity = nextCity(&iMyCityLoop))
	{

		for(pLoopTheirCity = GET_PLAYER(ePlayer).firstCity(&iTheirCityLoop); pLoopTheirCity != NULL; pLoopTheirCity = GET_PLAYER(ePlayer).nextCity(&iTheirCityLoop))
		{
			iNumCityConnections++;



			{
				iTempDistance = plotDistance(pLoopMyCity->getX(), pLoopMyCity->getY(), pLoopTheirCity->getX(), pLoopTheirCity->getY());
			}


			if(iTempDistance < iSmallestDistanceBetweenCities)
			{
				iSmallestDistanceBetweenCities = iTempDistance;
			}

			iAverageDistanceBetweenCities += iTempDistance;
		}
	}


	PlayerProximityTypes eProximity = NO_PLAYER_PROXIMITY;

	if(iNumCityConnections > 0)
	{
		iAverageDistanceBetweenCities /= iNumCityConnections;


		if(iSmallestDistanceBetweenCities <=       GC.getPROXIMITY_NEIGHBORS_CLOSEST_CITY_REQUIREMENT())
		{
			eProximity = PLAYER_PROXIMITY_NEIGHBORS;
		}

		else if(iSmallestDistanceBetweenCities <=        GC.getPROXIMITY_CLOSE_CLOSEST_CITY_POSSIBILITY())
		{
			eProximity = PLAYER_PROXIMITY_CLOSE;
		}


		if(eProximity != PLAYER_PROXIMITY_NEIGHBORS)
		{
			int iMapFactor = (GC.getMap().getGridWidth() + GC.getMap().getGridHeight()) / 2;



			int iCloseDistance = iMapFactor*        GC.getPROXIMITY_CLOSE_DISTANCE_MAP_MULTIPLIER() / 100;
			if(iCloseDistance >        GC.getPROXIMITY_CLOSE_DISTANCE_MAX())
			{
				iCloseDistance =        GC.getPROXIMITY_CLOSE_DISTANCE_MAX();
			}

			else if(iCloseDistance <        GC.getPROXIMITY_CLOSE_DISTANCE_MIN())
			{
				iCloseDistance =        GC.getPROXIMITY_CLOSE_DISTANCE_MIN();
			}


			int iFarDistance = iMapFactor*        GC.getPROXIMITY_FAR_DISTANCE_MAP_MULTIPLIER() / 100;
			if(iFarDistance >        GC.getPROXIMITY_FAR_DISTANCE_MAX())
			{
				iFarDistance =        GC.getPROXIMITY_FAR_DISTANCE_MAX();
			}

			else if(iFarDistance <        GC.getPROXIMITY_FAR_DISTANCE_MIN())
			{
				iFarDistance =        GC.getPROXIMITY_FAR_DISTANCE_MIN();
			}


			if(eProximity == PLAYER_PROXIMITY_CLOSE && iAverageDistanceBetweenCities <= iCloseDistance)
			{
				eProximity = PLAYER_PROXIMITY_CLOSE;
			}

			else if(iAverageDistanceBetweenCities <= iFarDistance)
			{
				eProximity = PLAYER_PROXIMITY_FAR;
			}

			else
			{
				eProximity = PLAYER_PROXIMITY_DISTANT;
			}
		}


		if(eProximity != PLAYER_PROXIMITY_DISTANT && !(GC.getMap().GetAIMapHint() & 1))
		{

			if(getCapitalCity() != NULL && GET_PLAYER(ePlayer).getCapitalCity() != NULL)
			{
				if(getCapitalCity()->getArea() != GET_PLAYER(ePlayer).getCapitalCity()->getArea())
				{
					eProximity = PlayerProximityTypes(eProximity - 1);
				}
			}
		}
	}

	int iNumMajorsLeft = GC.getGame().countMajorCivsAlive();


	if(iNumMajorsLeft == 2)
		eProximity = max(eProximity, PLAYER_PROXIMITY_CLOSE);


	else if(iNumMajorsLeft <= 4)
		eProximity = max(eProximity, PLAYER_PROXIMITY_FAR);

	SetProximityToPlayer(ePlayer, eProximity);
}



void CvPlayer::UpdateResearchAgreements(int iValue)
{
	PlayerTypes ePlayerLoop;
	TeamTypes eTeamLoop;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayerLoop = (PlayerTypes) iPlayerLoop;
		eTeamLoop = GET_PLAYER(ePlayerLoop).getTeam();
		if(ePlayerLoop == GetID())
			continue;
		if(eTeamLoop == getTeam())
			continue;

		if(GET_TEAM(getTeam()).IsHasResearchAgreement(eTeamLoop))
		{





			ChangeResearchAgreementCounter(ePlayerLoop, iValue);
		}
		else if(GetResearchAgreementCounter(ePlayerLoop) != 0)
		{
			SetResearchAgreementCounter(ePlayerLoop, 0);
		}
	}
}



int CvPlayer::GetResearchAgreementCounter(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiResearchAgreementCounter[ePlayer];
}



void CvPlayer::SetResearchAgreementCounter(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	CvAssertMsg(GetID() != ePlayer, "Trying to make a RA Agreement with oneself. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_aiResearchAgreementCounter.setAt(ePlayer, iValue);
}



void CvPlayer::ChangeResearchAgreementCounter(PlayerTypes ePlayer, int iChange)
{
	SetResearchAgreementCounter(ePlayer, GetResearchAgreementCounter(ePlayer) + iChange);
}



void CvPlayer::DoCivilianReturnLogic(bool bReturn, PlayerTypes eToPlayer, int iUnitID)
{
	CvUnit* pUnit = getUnit(iUnitID);
	if(!pUnit)
	{
		return;
	}

	CvPlot* pPlot = pUnit->plot();
	if(!pPlot)
	{
		return;
	}


	IDInfo* pUnitNode = pPlot->headUnitNode();
	while(pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if(NULL != pLoopUnit && pLoopUnit->getTransportUnit() == pUnit)
			pLoopUnit->kill(true);
	}


	UnitTypes eNewUnitType = pUnit->getUnitType();

	if(!bReturn)
		eNewUnitType = pUnit->getCaptureUnitType(getCivilizationType());

	int iX = pUnit->getX();
	int iY = pUnit->getY();


	if(bReturn)
	{
		pUnit->kill(true);
		CvUnit* pNewUnit = GET_PLAYER(eToPlayer).initUnit(eNewUnitType, iX, iY);
		CvAssert(pNewUnit != NULL);
		if (pNewUnit)
		{
			if (!pNewUnit->jumpToNearestValidPlot())
				pNewUnit->kill(false);
		}


		if(GET_PLAYER(eToPlayer).isMinorCiv())
		{
			int iInfluence =        GC.getRETURN_CIVILIAN_FRIENDSHIP();
			GET_PLAYER(eToPlayer).GetMinorCivAI()->ChangeFriendshipWithMajor(GetID(), iInfluence);
		}

		else if(!GET_PLAYER(eToPlayer).isHuman())
		{
			GET_PLAYER(eToPlayer).GetDiplomacyAI()->ChangeNumCiviliansReturnedToMe(GetID(), 1);
		}
	}

	else
	{

		if(eNewUnitType != pUnit->getUnitType())
		{
			pUnit->kill(true);
			CvUnit* pNewUnit = initUnit(eNewUnitType, iX, iY);
			CvAssert(pNewUnit != NULL);
			if (pNewUnit)
				pNewUnit->finishMoves();
		}
	}
}



void CvPlayer::DoIncomingUnits()
{
	AI_PERF_FORMAT("AI-perf.csv", ("CvPlayer::DoIncomingUnits, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), getCivilizationShortDescription()) );
	for(int iLoop = 0; iLoop < MAX_PLAYERS; iLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iLoop;
		CvAssertMsg(GetIncomingUnitCountdown(eLoopPlayer) >= -1, "Incoming Unit countdown is an invalid value. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
		if(GetIncomingUnitCountdown(eLoopPlayer) > 0)
		{
			ChangeIncomingUnitCountdown(eLoopPlayer, -1);


			if(GetIncomingUnitCountdown(eLoopPlayer) == 0)
			{

				CvCity* pCapital = getCapitalCity();
				if(pCapital)
				{
					CvUnit* pNewUnit = initUnit(GetIncomingUnitType(eLoopPlayer), pCapital->getX(), pCapital->getY());
					CvAssert(pNewUnit);
					if (pNewUnit)
					{
						if(pNewUnit->getDomainType() != DOMAIN_AIR)
						{
							if (!pNewUnit->jumpToNearestValidPlot())
								pNewUnit->kill(false);
						}


						if (isMinorCiv() && !GET_PLAYER(eLoopPlayer).isMinorCiv())
						{
							GetMinorCivAI()->DoUnitGiftFromMajor(eLoopPlayer, pNewUnit,                   true);
						}
					}
				}


				SetIncomingUnitCountdown(eLoopPlayer, -1);
				SetIncomingUnitType(eLoopPlayer, NO_UNIT);
			}
		}
	}
}



void CvPlayer::DoDistanceGift(PlayerTypes eFromPlayer, CvUnit* pUnit)
{
	if(!pUnit)
	{
		return;
	}

	CvPlot* pPlot = pUnit->plot();
	if(!pPlot)
	{
		return;
	}

#ifdef NET_FIX_SINGLE_USE_ABILITY_DUPE
	if (pUnit->isDelayedDeath())
	{
		return;
	}
#endif


	IDInfo* pUnitNode = pPlot->headUnitNode();
	while(pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if(NULL != pLoopUnit && pLoopUnit->getTransportUnit() == pUnit)
			AddIncomingUnit(eFromPlayer, pLoopUnit);
	}

	AddIncomingUnit(eFromPlayer, pUnit);
}



void CvPlayer::AddIncomingUnit(PlayerTypes eFromPlayer, CvUnit* pUnit)
{
	UnitTypes eUnitType = pUnit->getUnitType();


	if(isMinorCiv() && eFromPlayer < MAX_MAJOR_CIVS)
	{
		CvAssertMsg(GetIncomingUnitType(eFromPlayer) == NO_UNIT, "Adding incoming unit when one is already on its way. Please send Anton your save file and version.");
		CvAssertMsg(GetIncomingUnitCountdown(eFromPlayer) == -1, "Adding incoming unit when one is already on its way. Please send Anton your save file and version.");
		if(GetIncomingUnitCountdown(eFromPlayer) == -1)
		{
			SetIncomingUnitCountdown(eFromPlayer, GC.getMINOR_UNIT_GIFT_TRAVEL_TURNS());
			SetIncomingUnitType(eFromPlayer, eUnitType);
		}


		pUnit->kill(true);
	}

	else if(!isMinorCiv() && GET_PLAYER(eFromPlayer).isMinorCiv())
	{
		int iX = pUnit->getX();
		int iY = pUnit->getY();
		UnitTypes eType = pUnit->getUnitType();


		pUnit->kill(true);


		if(eType != NO_UNIT)
		{
			CvUnit* pNewUnit = initUnit(eType, iX, iY);
			CvAssert(pNewUnit);
			if (pNewUnit)
				pNewUnit->finishMoves();
		}
	}
	else
	{
		CvAssertMsg(false, "Unexpected case for adding an incoming unit for this player. Please send Anton your save file and version.");
	}
}



UnitTypes CvPlayer::GetIncomingUnitType(PlayerTypes eFromPlayer) const
{
	CvAssertMsg(eFromPlayer >= 0, "eFromPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eFromPlayer < MAX_PLAYERS, "eFromPlayer is expected to be within maximum bounds (invalid Index)");
	return (UnitTypes) m_aiIncomingUnitTypes[eFromPlayer];
}



void CvPlayer::SetIncomingUnitType(PlayerTypes eFromPlayer, UnitTypes eUnitType)
{
	CvAssertMsg(eFromPlayer >= 0, "eFromPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eFromPlayer < MAX_PLAYERS, "eFromPlayer is expected to be within maximum bounds (invalid Index)");

	CvAssertMsg(eUnitType >= NO_UNIT, "eUnitType is expected to be non-negative (invalid Index)");
	CvAssertMsg(eUnitType < GC.getNumUnitInfos(), "eUnitType is expected to be within maximum bounds (invalid Index)");

	if(eUnitType != m_aiIncomingUnitTypes[eFromPlayer])
	{
		m_aiIncomingUnitTypes.setAt(eFromPlayer, eUnitType);
	}
}



int CvPlayer::GetIncomingUnitCountdown(PlayerTypes eFromPlayer) const
{
	CvAssertMsg(eFromPlayer >= 0, "eFromPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eFromPlayer < MAX_PLAYERS, "eFromPlayer is expected to be within maximum bounds (invalid Index)");
	return m_aiIncomingUnitCountdowns[eFromPlayer];
}



void CvPlayer::SetIncomingUnitCountdown(PlayerTypes eFromPlayer, int iNumTurns)
{
	CvAssertMsg(eFromPlayer >= 0, "eFromPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eFromPlayer < MAX_PLAYERS, "eFromPlayer is expected to be within maximum bounds (invalid Index)");

	if(iNumTurns != m_aiIncomingUnitCountdowns[eFromPlayer])
		m_aiIncomingUnitCountdowns.setAt(eFromPlayer, iNumTurns);
}



void CvPlayer::ChangeIncomingUnitCountdown(PlayerTypes eFromPlayer, int iChange)
{
	if(iChange != 0)
		SetIncomingUnitCountdown(eFromPlayer, GetIncomingUnitCountdown(eFromPlayer) + iChange);
}


bool CvPlayer::isOption(PlayerOptionTypes eID) const
{
	for (PlayerOptionsVector::const_iterator itr = m_aOptions.begin(); itr != m_aOptions.end(); ++itr )
	{
		if ((*itr).first == eID)
			return (*itr).second != 0;
	}
	return false;
}



void CvPlayer::setOption(PlayerOptionTypes eID, bool bNewValue)
{
	int iIndex = 0;
	for (PlayerOptionsVector::const_iterator itr = m_aOptions.begin(); itr != m_aOptions.end(); ++itr )
	{
		if ((*itr).first == eID)
		{
			m_aOptions.setAt(iIndex, PlayerOptionEntry((uint)eID, bNewValue?1:0));
			return;
		}
		++iIndex;
	}

	m_aOptions.push_back(PlayerOptionEntry((uint)eID, bNewValue?1:0));
}


bool CvPlayer::isPlayable() const
{
	return CvPreGame::isPlayable(GetID());
}


void CvPlayer::setPlayable(bool bNewValue)
{
	CvPreGame::setPlayable(GetID(), bNewValue);
}


int CvPlayer::getNumResourceUsed(ResourceTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiNumResourceUsed[eIndex];
}


void CvPlayer::changeNumResourceUsed(ResourceTypes eIndex, int iChange)
{
	CvAssert(eIndex >= 0);
	CvAssert(eIndex < GC.getNumResourceInfos());

	if(iChange != 0)
	{
		m_paiNumResourceUsed.setAt(eIndex, m_paiNumResourceUsed[eIndex] + iChange);
	}

	if(iChange > 0)
		DoTestOverResourceNotification(eIndex);

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);

	CvAssert(m_paiNumResourceUsed[eIndex] >= 0);
}
#if defined(LEKMOD_FIX_PATRO_FOOD)

int CvPlayer::GetCityStateBonusModifier() const
{
	return m_iCityStateBonusModifier;
}

void CvPlayer::ChangeCityStateBonusModifier(int iChange)
{
	if (iChange != 0)
		m_iCityStateBonusModifier += iChange;
}
#endif

int CvPlayer::getNumResourceTotal(ResourceTypes eIndex, bool bIncludeImport) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");


	CvResourceInfo *pkResource = GC.getResourceInfo(eIndex);
	if (pkResource == NULL)
	{
		return 0;
	}

	int iTotalNumResource = m_paiNumResourceTotal[eIndex];

	if(pkResource->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
	{
		if(GetStrategicResourceMod() != 0)
		{
			iTotalNumResource *= GetStrategicResourceMod();
			iTotalNumResource /= 100;
		}
	}

	if(bIncludeImport)
	{
		iTotalNumResource += getResourceImport(eIndex);
		iTotalNumResource += getResourceFromMinors(eIndex);
		iTotalNumResource += getResourceSiphoned(eIndex);
	}

	iTotalNumResource -= getResourceExport(eIndex);

	return iTotalNumResource;
}


#ifdef LEKMOD_CS_BUILDING_STRATEGIC_NO_ALLY_SHARE
void CvPlayer::changeNumResourceTotal(ResourceTypes eIndex, int iChange, bool bIgnoreResourceWarning, bool bMinorStrategicFromBuilding)
{
	CvAssert(eIndex >= 0);
	CvAssert(eIndex < GC.getNumResourceInfos());

	if(iChange != 0)
	{
		m_paiNumResourceTotal.setAt(eIndex, m_paiNumResourceTotal[eIndex] + iChange);

		if(bMinorStrategicFromBuilding && isMinorCiv())
		{
			const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eIndex);
			if(pkResourceInfo != NULL && pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
			{
				m_paiMinorStrategicResourceFromBuildings.setAt(eIndex, m_paiMinorStrategicResourceFromBuildings[eIndex] + iChange);
				CvAssert(m_paiMinorStrategicResourceFromBuildings[eIndex] >= 0);
			}
		}


		if(isMinorCiv())
		{
			PlayerTypes eBestRelationsPlayer = GetMinorCivAI()->GetAlly();

			if(eBestRelationsPlayer != NO_PLAYER)
			{
				ResourceUsageTypes eUsage = GC.getResourceInfo(eIndex)->getResourceUsage();

				const bool bSkipAllyShare = (bMinorStrategicFromBuilding && eUsage == RESOURCEUSAGE_STRATEGIC);

				if(!bSkipAllyShare && (eUsage == RESOURCEUSAGE_STRATEGIC || eUsage == RESOURCEUSAGE_LUXURY))
				{

					if(eBestRelationsPlayer != NO_PLAYER)
					{
						GET_PLAYER(eBestRelationsPlayer).changeResourceFromMinors(eIndex, iChange);
						changeResourceExport(eIndex, iChange);

						CvNotifications* pNotifications = GET_PLAYER(eBestRelationsPlayer).GetNotifications();
						if(pNotifications && !GetMinorCivAI()->IsDisableNotifications())
						{
							Localization::String strMessage;
							Localization::String strSummary;


							if(iChange > 0)
							{
								strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BFF_NEW_RESOURCE");
								strMessage << getNameKey() << GC.getResourceInfo(eIndex)->GetDescriptionKey();
								strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_BFF_NEW_RESOURCE");
								strSummary << getNameKey() << GC.getResourceInfo(eIndex)->GetDescriptionKey();
							}

							else
							{
								strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BFF_LOST_RESOURCE");
								strMessage << getNameKey() << GC.getResourceInfo(eIndex)->GetDescriptionKey();
								strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_BFF_LOST_RESOURCE");
								strSummary << getNameKey() << GC.getResourceInfo(eIndex)->GetDescriptionKey();
							}

							int iX = -1;
							int iY = -1;

							CvCity* capCity = getCapitalCity();

							if(capCity != NULL)
							{
								iX = capCity->getX();
								iY = capCity->getY();
							}

							pNotifications->Add(NOTIFICATION_MINOR, strMessage.toUTF8(), strSummary.toUTF8(), iX, iY, -1);
						}
					}
				}
			}
		}


		for (int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
		{
			GET_PLAYER((PlayerTypes)iPlayerLoop).UpdateResourcesSiphoned();
		}
	}

	if(iChange < 0 && !bIgnoreResourceWarning)
		DoTestOverResourceNotification(eIndex);

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);

	CvAssert(m_paiNumResourceTotal[eIndex] >= 0);
}
#else
void CvPlayer::changeNumResourceTotal(ResourceTypes eIndex, int iChange, bool bIgnoreResourceWarning)
{
	CvAssert(eIndex >= 0);
	CvAssert(eIndex < GC.getNumResourceInfos());

	if(iChange != 0)
	{
		m_paiNumResourceTotal.setAt(eIndex, m_paiNumResourceTotal[eIndex] + iChange);


		if(isMinorCiv())
		{
			PlayerTypes eBestRelationsPlayer = GetMinorCivAI()->GetAlly();

			if(eBestRelationsPlayer != NO_PLAYER)
			{
				ResourceUsageTypes eUsage = GC.getResourceInfo(eIndex)->getResourceUsage();

				if(eUsage == RESOURCEUSAGE_STRATEGIC || eUsage == RESOURCEUSAGE_LUXURY)
				{

					if(eBestRelationsPlayer != NO_PLAYER)
					{
						GET_PLAYER(eBestRelationsPlayer).changeResourceFromMinors(eIndex, iChange);
						changeResourceExport(eIndex, iChange);

						CvNotifications* pNotifications = GET_PLAYER(eBestRelationsPlayer).GetNotifications();
						if(pNotifications && !GetMinorCivAI()->IsDisableNotifications())
						{
							Localization::String strMessage;
							Localization::String strSummary;


							if(iChange > 0)
							{
								strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BFF_NEW_RESOURCE");
								strMessage << getNameKey() << GC.getResourceInfo(eIndex)->GetDescriptionKey();
								strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_BFF_NEW_RESOURCE");
								strSummary << getNameKey() << GC.getResourceInfo(eIndex)->GetDescriptionKey();
							}

							else
							{
								strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BFF_LOST_RESOURCE");
								strMessage << getNameKey() << GC.getResourceInfo(eIndex)->GetDescriptionKey();
								strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_BFF_LOST_RESOURCE");
								strSummary << getNameKey() << GC.getResourceInfo(eIndex)->GetDescriptionKey();
							}

							int iX = -1;
							int iY = -1;

							CvCity* capCity = getCapitalCity();

							if(capCity != NULL)
							{
								iX = capCity->getX();
								iY = capCity->getY();
							}

							pNotifications->Add(NOTIFICATION_MINOR, strMessage.toUTF8(), strSummary.toUTF8(), iX, iY, -1);
						}
					}
				}
			}
		}


		for (int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
		{
			GET_PLAYER((PlayerTypes)iPlayerLoop).UpdateResourcesSiphoned();
		}
	}

	if(iChange < 0 && !bIgnoreResourceWarning)
		DoTestOverResourceNotification(eIndex);

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);

	CvAssert(m_paiNumResourceTotal[eIndex] >= 0);
}
#endif



int CvPlayer::getSiphonLuxuryCount(PlayerTypes eFromPlayer) const
{
	CvAssertMsg(eFromPlayer >= 0, "eFromPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eFromPlayer < MAX_PLAYERS, "eFromPlayer is expected to be within maximum bounds (invalid Index)");

	return m_aiSiphonLuxuryCount[eFromPlayer];
}



void CvPlayer::changeSiphonLuxuryCount(PlayerTypes eFromPlayer, int iChange)
{
	CvAssertMsg(eFromPlayer >= 0, "eFromPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eFromPlayer < MAX_PLAYERS, "eFromPlayer is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiSiphonLuxuryCount[eFromPlayer] = m_aiSiphonLuxuryCount[eFromPlayer] + iChange;
		CvAssert(getSiphonLuxuryCount(eFromPlayer) >= 0);

		UpdateResourcesSiphoned();
	}
}




void CvPlayer::UpdateResourcesSiphoned()
{
	FStaticVector<int, 64, true, c_eCiv5GameplayDLL> vDeltas;
	

#ifdef AUI_WARNING_FIXES
	for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
	for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
	{
		ResourceTypes eResourceLoop = (ResourceTypes) iResourceLoop;
		vDeltas.push_back(-1 * getResourceSiphoned(eResourceLoop));
	}


	for (int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
	{
		PlayerTypes ePlayerLoop = (PlayerTypes) iPlayerLoop;
		int iSiphonLuxuryCount = getSiphonLuxuryCount(ePlayerLoop);
		if (iSiphonLuxuryCount > 0)
		{
#ifdef AUI_WARNING_FIXES
			for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
			for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
			{
				ResourceTypes eResourceLoop = (ResourceTypes) iResourceLoop;
				CvResourceInfo* pInfo = GC.getResourceInfo(eResourceLoop);

				if (pInfo && pInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
				{

					if (GET_PLAYER(ePlayerLoop).getNumResourceTotal(eResourceLoop,                    false) > 0 || GET_PLAYER(ePlayerLoop).getResourceExport(eResourceLoop) > 0)
					{
						vDeltas[eResourceLoop] += iSiphonLuxuryCount;
					}
				}
			}
		}
	}


	for (uint i = 0; i < vDeltas.size(); i++)
	{
		if (vDeltas[i] != 0)
		{
			ResourceTypes eResource = (ResourceTypes) i;
			changeResourceSiphoned(eResource, vDeltas[i]);
		}
	}
}



void CvPlayer::DoTestOverResourceNotification(ResourceTypes eIndex)
{
	if(getNumResourceAvailable(eIndex, true) < 0)
	{
		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eIndex);
		if(pkResourceInfo != NULL && pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
		{
			CvNotifications* pNotifications = GetNotifications();
			if(pNotifications)
			{
				Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_OVER_RESOURCE_LIMIT");
				strText << pkResourceInfo->GetTextKey();
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_OVER_RESOURCE_LIMIT");
				strSummary << pkResourceInfo->GetTextKey();
				pNotifications->Add(NOTIFICATION_DEMAND_RESOURCE, strText.toUTF8(), strSummary.toUTF8(), -1, -1, eIndex);
			}
		}
	}
}



int CvPlayer::GetStrategicResourceMod() const
{
	return m_iStrategicResourceMod;
}



void CvPlayer::ChangeStrategicResourceMod(int iChange)
{
	m_iStrategicResourceMod += iChange;
}


int CvPlayer::getNumResourceAvailable(ResourceTypes eIndex, bool bIncludeImport) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return getNumResourceTotal(eIndex, bIncludeImport) - getNumResourceUsed(eIndex);
}


#ifdef LEKMOD_CS_BUILDING_STRATEGIC_NO_ALLY_SHARE
int CvPlayer::getNumMinorStrategicResourceFromBuildings(ResourceTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiMinorStrategicResourceFromBuildings[eIndex];
}
#endif



int CvPlayer::getResourceGiftedToMinors(ResourceTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiResourceGiftedToMinors[eIndex];
}



void CvPlayer::changeResourceGiftedToMinors(ResourceTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_paiResourceGiftedToMinors.setAt(eIndex, m_paiResourceGiftedToMinors[eIndex] + iChange);
		CvAssert(getResourceGiftedToMinors(eIndex) >= 0);
	}
}


int CvPlayer::getResourceExport(ResourceTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiResourceExport[eIndex];
}


void CvPlayer::changeResourceExport(ResourceTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_paiResourceExport.setAt(eIndex, m_paiResourceExport[eIndex] + iChange);
		CvAssert(getResourceExport(eIndex) >= 0);

		DoUpdateHappiness();
#ifdef AUI_CITIZENS_MID_TURN_ASSIGN_RUNS_SELF_CONSISTENCY
		doSelfConsistencyCheckAllCities();
#endif
	}
}


int CvPlayer::getResourceImport(ResourceTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiResourceImport[eIndex];
}


void CvPlayer::changeResourceImport(ResourceTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_paiResourceImport.setAt(eIndex, m_paiResourceImport[eIndex] + iChange);
		CvAssert(getResourceImport(eIndex) >= 0);

		DoUpdateHappiness();
#ifdef AUI_CITIZENS_MID_TURN_ASSIGN_RUNS_SELF_CONSISTENCY
		doSelfConsistencyCheckAllCities();
#endif
	}
}


int CvPlayer::getResourceFromMinors(ResourceTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	int iNumResourceFromMinors = m_paiResourceFromMinors[eIndex];


	if(IsMinorResourceBonus())
	{
		iNumResourceFromMinors *=         GC.getMINOR_POLICY_RESOURCE_MULTIPLIER();
		iNumResourceFromMinors /= 100;
	}

	return iNumResourceFromMinors;
}


void CvPlayer::changeResourceFromMinors(ResourceTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_paiResourceFromMinors.setAt(eIndex, m_paiResourceFromMinors[eIndex] + iChange);
		CvAssert(getResourceFromMinors(eIndex) >= 0);

		DoUpdateHappiness();
#ifdef AUI_CITIZENS_MID_TURN_ASSIGN_RUNS_SELF_CONSISTENCY
		doSelfConsistencyCheckAllCities();
#endif
	}
}


int CvPlayer::getResourceSiphoned(ResourceTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	int iNumResourceSiphoned = m_paiResourcesSiphoned[eIndex];

	return iNumResourceSiphoned;
}


void CvPlayer::changeResourceSiphoned(ResourceTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiResourcesSiphoned.setAt(eIndex, m_paiResourcesSiphoned[eIndex] + iChange);
		CvAssert(getResourceSiphoned(eIndex) >= 0);

		DoUpdateHappiness();
#ifdef AUI_CITIZENS_MID_TURN_ASSIGN_RUNS_SELF_CONSISTENCY
		doSelfConsistencyCheckAllCities();
#endif
	}
}


#ifdef AUI_CONSTIFY
int CvPlayer::getResourceInOwnedPlots(ResourceTypes eIndex) const
#else
int CvPlayer::getResourceInOwnedPlots(ResourceTypes eIndex)
#endif
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	int iCount = 0;


#ifdef AUI_CONSTIFY
	for (uint uiPlotIndex = 0; uiPlotIndex < m_aiPlots.size(); uiPlotIndex++)
	{
		if (m_aiPlots[uiPlotIndex] == -1)
			continue;

		CvPlot* pPlot = GC.getMap().plotByIndex(m_aiPlots[uiPlotIndex]);
#else
	const CvPlotsVector& aiPlots = GetPlots();
	for (uint uiPlotIndex = 0; uiPlotIndex < aiPlots.size(); uiPlotIndex++)
	{
		if (aiPlots[uiPlotIndex] == -1)
			continue;

		CvPlot* pPlot = GC.getMap().plotByIndex(aiPlots[uiPlotIndex]);
#endif
		if (pPlot && pPlot->getResourceType(getTeam()) == eIndex)
		{
			iCount++;
		}
	}

	return iCount;
}


int CvPlayer::getTotalImprovementsBuilt() const
{
	return m_iTotalImprovementsBuilt;
}


void CvPlayer::changeTotalImprovementsBuilt(int iChange)
{
	m_iTotalImprovementsBuilt = (m_iTotalImprovementsBuilt + iChange);
	CvAssert(getTotalImprovementsBuilt() >= 0);
}



int CvPlayer::getImprovementCount(ImprovementTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumImprovementInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiImprovementCount[eIndex];
}



void CvPlayer::changeImprovementCount(ImprovementTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumImprovementInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiImprovementCount.setAt(eIndex, m_paiImprovementCount[eIndex] + iChange);
	CvAssert(getImprovementCount(eIndex) >= 0);
}



#if defined(AUI_WARNING_FIXES) || defined(AUI_CONSTIFY) || defined(NQ_HAPPINESS_FROM_GREAT_IMPROVEMENTS_FROM_POLICIES)
int CvPlayer::getGreatPersonImprovementCount() const
#else
int CvPlayer::getGreatPersonImprovementCount()
#endif
{
	int iCount = 0;
#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < GC.getNumImprovementInfos(); i++)
#else
	for (int i = 0; i < GC.getNumImprovementInfos(); i++)
#endif
	{
		ImprovementTypes e = (ImprovementTypes)i;
		CvImprovementEntry* pInfo = GC.getImprovementInfo(e);
		if (pInfo && pInfo->IsCreatedByGreatPerson())
		{
			iCount += getImprovementCount(e);
		}
	}
	return iCount;
}



int CvPlayer::getFreeBuildingCount(BuildingTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiFreeBuildingCount[eIndex];
}



bool CvPlayer::isBuildingFree(BuildingTypes eIndex)	const
{
	return (getFreeBuildingCount(eIndex) > 0);
}



void CvPlayer::changeFreeBuildingCount(BuildingTypes eIndex, int iChange)
{
	CvCity* pLoopCity;
	int iOldFreeBuildingCount;
	int iLoop;

	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		iOldFreeBuildingCount = getFreeBuildingCount(eIndex);

		m_paiFreeBuildingCount.setAt(eIndex, m_paiFreeBuildingCount[eIndex] + iChange);
		CvAssert(getFreeBuildingCount(eIndex) >= 0);

		if(iOldFreeBuildingCount == 0)
		{
			CvAssertMsg(getFreeBuildingCount(eIndex) > 0, "getFreeBuildingCount(eIndex) is expected to be greater than 0");

			for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				pLoopCity->GetCityBuildings()->SetNumFreeBuilding(eIndex, 1);
			}
		}
		else if(getFreeBuildingCount(eIndex) == 0)
		{
			CvAssertMsg(iOldFreeBuildingCount > 0, "iOldFreeBuildingCount is expected to be greater than 0");

			for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				pLoopCity->GetCityBuildings()->SetNumFreeBuilding(eIndex, 0);
			}
		}
	}
}



int CvPlayer::GetFreePromotionCount(PromotionTypes ePromotion) const
{
	CvAssertMsg(ePromotion >= 0, "ePromotion is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePromotion < GC.getNumPromotionInfos(), "ePromotion is expected to be within maximum bounds (invalid Index)");
	return m_paiFreePromotionCount[ePromotion];
}



bool CvPlayer::IsFreePromotion(PromotionTypes ePromotion)	const
{
	return (GetFreePromotionCount(ePromotion) > 0);
}



void CvPlayer::ChangeFreePromotionCount(PromotionTypes ePromotion, int iChange)
{
	CvAssertMsg(ePromotion >= 0, "ePromotion is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePromotion < GC.getNumPromotionInfos(), "ePromotion is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		bool bWasFree = IsFreePromotion(ePromotion);

		m_paiFreePromotionCount.setAt(ePromotion, m_paiFreePromotionCount[ePromotion] + iChange);

		CvAssert(GetFreePromotionCount(ePromotion) >= 0);


		if(IsFreePromotion(ePromotion) && !bWasFree)
		{

			CvUnit* pLoopUnit;

			int iLoop;
			for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
			{

				if(::IsPromotionValidForUnitCombatType(ePromotion, pLoopUnit->getUnitType()))
				{
					pLoopUnit->setHasPromotion(ePromotion, true);
				}

				else if(::IsPromotionValidForCivilianUnitType(ePromotion, pLoopUnit->getUnitType()))
				{
					pLoopUnit->setHasPromotion(ePromotion, true);
				}
			}
		}
	}
}
#ifdef LEKMOD_UNITCOMBAT_FREE_PROMOTION



int CvPlayer::GetFreePromotionUnitCombatCount(PromotionTypes ePromotion, int iIndex) const
{
	CvAssertMsg(ePromotion >= 0, "ePromotion is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePromotion < GC.getNumPromotionInfos(), "ePromotion is expected to be within maximum bounds (invalid Index)");
	return m_paiUnitCombatFreePromotionCount[ePromotion][iIndex];
}



bool CvPlayer::IsFreePromotionUnitCombat(PromotionTypes ePromotion, int iIndex) const
{
	return (GetFreePromotionUnitCombatCount(ePromotion, iIndex) > 0);
}

void CvPlayer::ChangeFreePromotionUnitCombatCount(PromotionTypes ePromotion, int iChange, int iIndex)
{
	CvAssertMsg(ePromotion >= 0, "ePromotion is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePromotion < GC.getNumPromotionInfos(), "ePromotion is expected to be within maximum bounds (invalid Index)");
	OutputDebugStringA(CvString::format("UnitCombat: %i\n", iIndex).c_str());
	if(iChange != 0)
	{
		bool bWasFree = IsFreePromotionUnitCombat(ePromotion, iIndex);

		

		m_paiUnitCombatFreePromotionCount.setAt2D(ePromotion, iIndex, m_paiUnitCombatFreePromotionCount[ePromotion][iIndex] + iChange);

		CvAssert(GetFreePromotionUnitCombatCount(ePromotion, iIndex) >= 0);



		if(IsFreePromotionUnitCombat(ePromotion, iIndex) && !bWasFree)
		{

			CvUnit* pLoopUnit;

			int iLoop;
			for (pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
			{
				OutputDebugStringA(CvString::format("LoopUnit: %i\n", pLoopUnit->getUnitCombatType()).c_str());


				if (pLoopUnit->getUnitCombatType() == iIndex)
				{

					OutputDebugStringA(CvString::format("UnitCombat: %i\n", pLoopUnit->getUnitCombatType()).c_str());

					pLoopUnit->setHasPromotion(ePromotion, true);
			
				}
				
			}
		}
	}
}
#endif


int CvPlayer::getUnitCombatProductionModifiers(UnitCombatTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiUnitCombatProductionModifiers[eIndex];
}



void CvPlayer::changeUnitCombatProductionModifiers(UnitCombatTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_paiUnitCombatProductionModifiers.setAt(eIndex, m_paiUnitCombatProductionModifiers[eIndex] + iChange);
	}
}



int CvPlayer::getUnitCombatFreeExperiences(UnitCombatTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiUnitCombatFreeExperiences[eIndex];
}



void CvPlayer::changeUnitCombatFreeExperiences(UnitCombatTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_paiUnitCombatFreeExperiences.setAt(eIndex, m_paiUnitCombatFreeExperiences[eIndex] + iChange);
	}
}


int CvPlayer::getUnitClassCount(UnitClassTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiUnitClassCount[eIndex];
}



bool CvPlayer::isUnitClassMaxedOut(UnitClassTypes eIndex, int iExtra) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eIndex);
	if(pkUnitClassInfo == NULL)
	{
		return false;
	}

	if(!isNationalUnitClass(eIndex))
	{
		return false;
	}

	CvAssertMsg(getUnitClassCount(eIndex) <= pkUnitClassInfo->getMaxPlayerInstances(), "getUnitClassCount is expected to be less than maximum bound of MaxPlayerInstances (invalid index)");

	return ((getUnitClassCount(eIndex) + iExtra) >= pkUnitClassInfo->getMaxPlayerInstances());
}



void CvPlayer::changeUnitClassCount(UnitClassTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiUnitClassCount.setAt(eIndex, m_paiUnitClassCount[eIndex] + iChange);
	CvAssert(getUnitClassCount(eIndex) >= 0);
}



int CvPlayer::getUnitClassMaking(UnitClassTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiUnitClassMaking[eIndex];
}



void CvPlayer::changeUnitClassMaking(UnitClassTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_paiUnitClassMaking.setAt(eIndex, m_paiUnitClassMaking[eIndex] + iChange);
		CvAssert(getUnitClassMaking(eIndex) >= 0);

		CvCivilizationInfo& playerCivilizationInfo = getCivilizationInfo();
		UnitTypes eUnit = static_cast<UnitTypes>(playerCivilizationInfo.getCivilizationUnits(eIndex));
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
		if(pkUnitInfo)
		{

			if(pkUnitInfo->GetWorkRate() > 0 && pkUnitInfo->GetDomainType() == DOMAIN_LAND)
			{
				ChangeNumBuilders(iChange);
			}


#ifdef AUI_WARNING_FIXES
			for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
			for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
			{
				ResourceTypes eResource = static_cast<ResourceTypes>(iResourceLoop);
				CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
				if(pkResource)
				{
					if(pkUnitInfo->GetResourceQuantityRequirement(iResourceLoop) > 0)
					{
						changeNumResourceUsed(eResource, iChange * pkUnitInfo->GetResourceQuantityRequirement(iResourceLoop));
					}
				}
			}

			if(GetID() == GC.getGame().getActivePlayer())
			{
				GC.GetEngineUserInterface()->setDirty(Help_DIRTY_BIT, true);
			}
		}
	}
}



int CvPlayer::getUnitClassCountPlusMaking(UnitClassTypes eIndex) const
{
	return (getUnitClassCount(eIndex) + getUnitClassMaking(eIndex));
}



int CvPlayer::getBuildingClassCount(BuildingClassTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiBuildingClassCount[eIndex];
}



bool CvPlayer::isBuildingClassMaxedOut(BuildingClassTypes eIndex, int iExtra) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eIndex);
	if(pkBuildingClassInfo == NULL)
	{
		CvAssertMsg(false, "This should never happen...");
		return false;
	}

	if(!isNationalWonderClass(*pkBuildingClassInfo))
	{
		return false;
	}

	CvAssertMsg(getBuildingClassCount(eIndex) <= (pkBuildingClassInfo->getMaxPlayerInstances() + pkBuildingClassInfo->getExtraPlayerInstances()), "BuildingClassCount is expected to be less than or match the number of max player instances plus extra player instances");

	return ((getBuildingClassCount(eIndex) + iExtra) >= (pkBuildingClassInfo->getMaxPlayerInstances() + pkBuildingClassInfo->getExtraPlayerInstances()));
}



void CvPlayer::changeBuildingClassCount(BuildingClassTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiBuildingClassCount.setAt(eIndex, m_paiBuildingClassCount[eIndex] + iChange);
	CvAssert(getBuildingClassCount(eIndex) >= 0);
}



int CvPlayer::getBuildingClassMaking(BuildingClassTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiBuildingClassMaking[eIndex];
}



void CvPlayer::changeBuildingClassMaking(BuildingClassTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_paiBuildingClassMaking.setAt(eIndex, m_paiBuildingClassMaking[eIndex] + iChange);
		CvAssert(getBuildingClassMaking(eIndex) >= 0);

		const BuildingTypes eBuilding = (BuildingTypes) getCivilizationInfo().getCivilizationBuildings(eIndex);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if(pkBuildingInfo)
		{

#ifdef AUI_WARNING_FIXES
			for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
			for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
			{
				const ResourceTypes eResource = static_cast<ResourceTypes>(iResourceLoop);
				CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
				if(pkResourceInfo)
				{
					if(pkBuildingInfo->GetResourceQuantityRequirement(iResourceLoop) > 0)
					{
						changeNumResourceUsed(eResource, iChange * pkBuildingInfo->GetResourceQuantityRequirement(iResourceLoop));
					}
				}

			}
		}


		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(Help_DIRTY_BIT, true);
		}
	}
}



int CvPlayer::getBuildingClassCountPlusMaking(BuildingClassTypes eIndex) const
{
	return (getBuildingClassCount(eIndex) + getBuildingClassMaking(eIndex));
}





int CvPlayer::getProjectMaking(ProjectTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiProjectMaking[eIndex];
}


void CvPlayer::changeProjectMaking(ProjectTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_paiProjectMaking.setAt(eIndex, m_paiProjectMaking[eIndex] + iChange);
		CvAssert(getProjectMaking(eIndex) >= 0);


#ifdef AUI_WARNING_FIXES
		for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
		{
			if(GC.getProjectInfo(eIndex)->GetResourceQuantityRequirement(iResourceLoop) > 0)
			{
				changeNumResourceUsed((ResourceTypes) iResourceLoop, iChange * GC.getProjectInfo(eIndex)->GetResourceQuantityRequirement(iResourceLoop));
			}
		}
	}
}



int CvPlayer::getHurryCount(HurryTypes eIndex) const
{
	CvAssert(eIndex >= 0);
	CvAssert(eIndex < GC.getNumHurryInfos());
	return m_paiHurryCount[eIndex];
}




bool CvPlayer::IsHasAccessToHurry(HurryTypes eIndex) const
{
	return (getHurryCount(eIndex) > 0);
}



bool CvPlayer::IsCanHurry(HurryTypes eIndex) const
{
	CvHurryInfo* pkHurryInfo = GC.getHurryInfo(eIndex);
	if(pkHurryInfo == NULL)
		return false;

	int iCost = GetHurryGoldCost(eIndex);


	if(iCost < 0 || GetTreasury()->GetGold() < iCost)
	{
		return false;
	}


	if(pkHurryInfo->getGoldPerBeaker() > 0)
	{
		return true;
	}


	if(pkHurryInfo->getGoldPerCulture() > 0)
	{

		if(getNextPolicyCost() > getJONSCulture())
		{
			return true;
		}
	}

	return false;
}



int CvPlayer::GetHurryGoldCost(HurryTypes eHurry) const
{
	int iGold = -1;

	CvHurryInfo* pkHurryInfo = GC.getHurryInfo(eHurry);
	if(pkHurryInfo == NULL)
	{

		return -1;
	}


	if(pkHurryInfo->getGoldPerBeaker() > 0)
	{
		TechTypes eTech = GetPlayerTechs()->GetCurrentResearch();

		if(eTech != NO_TECH)
		{
#ifdef AUI_TECH_FIX_PLAYER_BASED_RESEARCH_COST_ONLY_AESTHETIC
			int iTotalCost = GetPlayerTechs()->GetResearchCost(eTech);
			int iResearchLeft = MAX(0, (iTotalCost - GET_TEAM(getTeam()).GetTeamTechs()->GetResearchProgress(eTech)));
#else
			int iTotalCost = GET_TEAM(getTeam()).GetTeamTechs()->GetResearchCost(eTech);
			int iResearchLeft = GET_TEAM(getTeam()).GetTeamTechs()->GetResearchLeft(eTech);
#endif


			int iGoldForFullPrice = iTotalCost * pkHurryInfo->getGoldPerBeaker();
			iGoldForFullPrice = (int) pow((double) iGoldForFullPrice, (double)           GC.getHURRY_GOLD_TECH_EXPONENT());


			iGold = (iGoldForFullPrice * iResearchLeft / iTotalCost);
		}
	}


	if(pkHurryInfo->getGoldPerCulture() > 0)
	{
		int iCurrentPolicyCost = getNextPolicyCost();

		if(iCurrentPolicyCost > 0)
		{
#ifdef AUI_PLAYER_FIX_JONS_CULTURE_IS_T100
			int iCultureLeft = iCurrentPolicyCost * 100 - getJONSCultureTimes100();
#else
			int iCultureLeft = iCurrentPolicyCost - getJONSCulture();
#endif


			int iGoldForFullPrice = iCurrentPolicyCost * pkHurryInfo->getGoldPerCulture();
			iGoldForFullPrice = (int) pow((double) iGoldForFullPrice, (double)           GC.getHURRY_GOLD_CULTURE_EXPONENT());


#ifdef AUI_PLAYER_FIX_JONS_CULTURE_IS_T100
			iGold = (iGoldForFullPrice * iCultureLeft / iCurrentPolicyCost / 100);
#else
			iGold = (iGoldForFullPrice * iCultureLeft / iCurrentPolicyCost);
#endif
		}
	}

	return iGold;
}



void CvPlayer::DoHurry(HurryTypes eIndex)
{
	CvHurryInfo* pkHurryInfo = GC.getHurryInfo(eIndex);
	if(pkHurryInfo)
	{
		if(IsCanHurry(eIndex))
		{
			int iGoldCost = GetHurryGoldCost(eIndex);
			GetTreasury()->ChangeGold(-iGoldCost);


			if(pkHurryInfo->getGoldPerBeaker() > 0)
			{
				TechTypes eTech = GetPlayerTechs()->GetCurrentResearch();

				GET_TEAM(getTeam()).setHasTech(eTech, true, GetID(), false, false);
			}


			if(pkHurryInfo->getGoldPerCulture() > 0)
			{
				setJONSCulture(getNextPolicyCost());
			}
		}
	}
}



bool CvPlayer::canPopRush()
{
	return (m_iPopRushHurryCount > 0);
}


void CvPlayer::changeHurryCount(HurryTypes eIndex, int iChange)
{
	CvAssert(eIndex >= 0);
	CvAssert(eIndex < GC.getNumHurryInfos());

	int oldHurryCount = m_paiHurryCount[eIndex];
	m_paiHurryCount.setAt(eIndex, m_paiHurryCount[eIndex] + iChange);
	CvAssert(getHurryCount(eIndex) >= 0);

	CvHurryInfo* pkHurryInfo = GC.getHurryInfo(eIndex);
	if(pkHurryInfo == NULL)
		return;


	if((oldHurryCount > 0) != (m_paiHurryCount[eIndex] > 0))
	{

		if(pkHurryInfo->getProductionPerPopulation() > 0)
		{
			m_iPopRushHurryCount += iChange;
			CvAssert(m_iPopRushHurryCount >= 0);
		}
	}
}



int CvPlayer::getHurryModifier(HurryTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumHurryInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiHurryModifier[eIndex];
}


void CvPlayer::changeHurryModifier(HurryTypes eIndex, int iChange)
{
	if(iChange != 0)
	{
		CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
		CvAssertMsg(eIndex < GC.getNumHurryInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
		m_paiHurryModifier.setAt(eIndex, m_paiHurryModifier[eIndex] + iChange);
	}
}


void CvPlayer::setResearchingTech(TechTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(m_pPlayerTechs->IsResearchingTech(eIndex) != bNewValue)
	{
		GetPlayerTechs()->SetResearchingTech(eIndex, bNewValue);

		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(Popup_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getSpecialistExtraYield(SpecialistTypes eIndex1, YieldTypes eIndex2) const
{
	CvAssertMsg(eIndex1 >= 0, "eIndex1 expected to be >= 0");
	CvAssertMsg(eIndex1 < GC.getNumSpecialistInfos(), "eIndex1 expected to be < GC.getNumSpecialistInfos()");
	CvAssertMsg(eIndex2 >= 0, "eIndex2 expected to be >= 0");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 expected to be < NUM_YIELD_TYPES");
	return m_ppaaiSpecialistExtraYield[eIndex1][eIndex2];
}



void CvPlayer::changeSpecialistExtraYield(SpecialistTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	CvAssertMsg(eIndex1 >= 0, "eIndex1 expected to be >= 0");
	CvAssertMsg(eIndex1 < GC.getNumSpecialistInfos(), "eIndex1 expected to be < GC.getNumSpecialistInfos()");
	CvAssertMsg(eIndex2 >= 0, "eIndex2 expected to be >= 0");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 expected to be < NUM_YIELD_TYPES");

	if(iChange != 0)
	{
		Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiSpecialistExtraYield[eIndex1];
		yields[eIndex2] = (m_ppaaiSpecialistExtraYield[eIndex1][eIndex2] + iChange);
		m_ppaaiSpecialistExtraYield.setAt(eIndex1, yields);
		CvAssert(getSpecialistExtraYield(eIndex1, eIndex2) >= 0);

		updateExtraSpecialistYield();
	}
}


int CvPlayer::getResourceYieldChange(ResourceTypes eIndex1, YieldTypes eIndex2) const
{
	CvAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex1 < GC.getNumResourceInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppiResourceYieldChange[eIndex1][eIndex2];
}


void CvPlayer::changeResourceYieldChange(ResourceTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	CvAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex1 < GC.getNumResourceInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		CvAssertMsg(iChange > -50 && iChange < 50, "GAMEPLAY: Yield for a plot is either negative or a ridiculously large number. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppiResourceYieldChange[eIndex1];
		yields[eIndex2] = (m_ppiResourceYieldChange[eIndex1][eIndex2] + iChange);
		m_ppiResourceYieldChange[eIndex1] = yields;
		CvAssert(getResourceYieldChange(eIndex1, eIndex2) >= 0);

		updateYield();
	}
}


int CvPlayer::getImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const
{
	CvAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppaaiImprovementYieldChange[eIndex1][eIndex2];
}



void CvPlayer::changeImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	CvAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		CvAssertMsg(iChange > -50 && iChange < 50, "GAMEPLAY: Yield for a plot is either negative or a ridiculously large number. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiImprovementYieldChange[eIndex1];
		yields[eIndex2] = (m_ppaaiImprovementYieldChange[eIndex1][eIndex2] + iChange);
		m_ppaaiImprovementYieldChange.setAt(eIndex1, yields);
		CvAssert(getImprovementYieldChange(eIndex1, eIndex2) >= 0);

		updateYield();
	}
}

bool CvPlayer::removeFromArmy(int iArmyID, int iID)
{
	bool bRemoved = false;
	CvArmyAI* pThisArmyAI = getArmyAI(iArmyID);
	if(pThisArmyAI)
	{
		bRemoved = pThisArmyAI->RemoveUnit(iID);
	}

	return bRemoved;
}



bool CvPlayer::removeFromArmy(int iID)
{
	CvArmyAI* pLoopArmyAI;
	int iLoop;
	bool bRemoved = false;


	for(pLoopArmyAI = firstArmyAI(&iLoop); pLoopArmyAI != NULL && !bRemoved; pLoopArmyAI = nextArmyAI(&iLoop))
	{

		bRemoved = removeFromArmy(pLoopArmyAI->GetID(), iID);
	}
	return bRemoved;
}




int CvPlayer::findPathLength(TechTypes eTech, bool bCost) const
{
	int i;
	int iNumSteps = 0;
	int iShortestPath = 0;
	int iPathLength = 0;
	TechTypes ePreReq;
	TechTypes eShortestOr;

	CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
	if(pkTechInfo == NULL)
		return 0;

	if(GET_TEAM(getTeam()).GetTeamTechs()->HasTech(eTech) || m_pPlayerTechs->IsResearchingTech(eTech))
	{


		return 0;
	}


	for(i = 0; i < GC.getNUM_AND_TECH_PREREQS(); i++)
	{
		ePreReq = (TechTypes)pkTechInfo->GetPrereqAndTechs(i);

		if(ePreReq != NO_TECH)
		{
			iPathLength += findPathLength(ePreReq, bCost);
		}
	}

	eShortestOr = NO_TECH;
	iShortestPath = INT_MAX;

	for(i = 0; i < GC.getNUM_OR_TECH_PREREQS(); i++)
	{

		ePreReq = (TechTypes)pkTechInfo->GetPrereqOrTechs(i);


		if(ePreReq != NO_TECH)
		{

			iNumSteps = findPathLength(ePreReq, bCost);


			if(iNumSteps < iShortestPath)
			{
				eShortestOr = ePreReq;
				iShortestPath = iNumSteps;
			}
		}
	}


	if(eShortestOr != NO_TECH)
	{
		iPathLength += iShortestPath;
	}

#ifdef AUI_TECH_FIX_PLAYER_BASED_RESEARCH_COST_ONLY_AESTHETIC
	return (iPathLength + ((bCost) ? GetPlayerTechs()->GetResearchCost(eTech) : 1));
#else
	return (iPathLength + ((bCost) ? GET_TEAM(getTeam()).GetTeamTechs()->GetResearchCost(eTech) : 1));
#endif
}




int CvPlayer::getQueuePosition(TechTypes eTech) const
{
	int i = 1;
	const CLLNode<TechTypes>* pResearchNode;

	for(pResearchNode = headResearchQueueNode(); pResearchNode; pResearchNode = nextResearchQueueNode(pResearchNode))
	{
		if(pResearchNode->m_data == eTech)
		{
			return i;
		}
		i++;
	}

	return -1;
}



void CvPlayer::clearResearchQueue()
{
#ifdef AUI_WARNING_FIXES
	uint iI;
#else
	int iI;
#endif

	m_researchQueue.clear();

	for(iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		setResearchingTech(((TechTypes)iI), false);
	}

	if(getTeam() == GC.getGame().getActiveTeam())
	{
		GC.GetEngineUserInterface()->setDirty(ResearchButtons_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(Score_DIRTY_BIT, true);
	}
}






bool CvPlayer::pushResearch(TechTypes eTech, bool bClear)
{
	int i;
	int iNumSteps;
	int iShortestPath;
	bool bOrPrereqFound;
	TechTypes ePreReq;
	TechTypes eShortestOr;

	CvAssertMsg(eTech != NO_TECH, "Tech is not assigned a valid value");

	CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
	if(pkTechInfo == NULL)
		return false;


	if(GET_TEAM(getTeam()).GetTeamTechs()->HasTech(eTech) || m_pPlayerTechs->IsResearchingTech(eTech))
	{

		return true;
	}

	if(!GetPlayerTechs()->CanEverResearch(eTech))
	{
		return false;
	}


	if(bClear)
	{
		clearResearchQueue();
	}


	for(i = 0; i < GC.getNUM_AND_TECH_PREREQS(); i++)
	{
		ePreReq = (TechTypes)pkTechInfo->GetPrereqAndTechs(i);

		if(ePreReq != NO_TECH)
		{
			if(!pushResearch(ePreReq))
			{
				return false;
			}
		}
	}


	eShortestOr = NO_TECH;
	iShortestPath = INT_MAX;
	bOrPrereqFound = false;

	for(i = 0; i < GC.getNUM_OR_TECH_PREREQS(); i++)
	{
		ePreReq = (TechTypes)pkTechInfo->GetPrereqOrTechs(i);

		if(ePreReq != NO_TECH)
		{
			bOrPrereqFound = true;


			if(GET_TEAM(getTeam()).GetTeamTechs()->HasTech(ePreReq))
			{
				eShortestOr = ePreReq;
				break;
			}

			if(GetPlayerTechs()->CanEverResearch(ePreReq))
			{

				iNumSteps = findPathLength(ePreReq);


				if(iNumSteps < iShortestPath)
				{
					eShortestOr = ePreReq;
					iShortestPath = iNumSteps;
				}
			}
		}
	}


	if(eShortestOr != NO_TECH)
	{
		if(!pushResearch(eShortestOr))
		{
			return false;
		}
	}
	else if(bOrPrereqFound)
	{
		return false;
	}


	m_researchQueue.insertAtEnd(eTech);

	setResearchingTech(eTech, true);


	if(getTeam() == GC.getGame().getActiveTeam())
	{
		GC.GetEngineUserInterface()->setDirty(ResearchButtons_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(Score_DIRTY_BIT, true);
	}
	return true;
}




void CvPlayer::popResearch(TechTypes eTech)
{
	CLLNode<TechTypes>* pResearchNode;

	for(pResearchNode = headResearchQueueNode(); pResearchNode; pResearchNode = nextResearchQueueNode(pResearchNode))
	{
		if(pResearchNode->m_data == eTech)
		{
			m_researchQueue.deleteNode(pResearchNode);
			break;
		}
	}

	setResearchingTech(eTech, false);

	if(getTeam() == GC.getGame().getActiveTeam())
	{
		GC.GetEngineUserInterface()->setDirty(ResearchButtons_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(Score_DIRTY_BIT, true);
	}
}



int CvPlayer::getLengthResearchQueue() const
{
	return m_researchQueue.getLength();
}



CLLNode<TechTypes>* CvPlayer::nextResearchQueueNode(CLLNode<TechTypes>* pNode)
{
	return m_researchQueue.next(pNode);
}


const CLLNode<TechTypes>* CvPlayer::nextResearchQueueNode(const CLLNode<TechTypes>* pNode) const
{
	return m_researchQueue.next(pNode);
}


CLLNode<TechTypes>* CvPlayer::headResearchQueueNode()
{
	return m_researchQueue.head();
}


const CLLNode<TechTypes>* CvPlayer::headResearchQueueNode() const
{
	return m_researchQueue.head();
}


CLLNode<TechTypes>* CvPlayer::tailResearchQueueNode()
{
	return m_researchQueue.tail();
}



void CvPlayer::addCityName(const CvString& szName)
{
	m_cityNames.insertAtEnd(szName);
}



int CvPlayer::getNumCityNames() const
{
	return m_cityNames.getLength();
}



CvString CvPlayer::getCityName(int iIndex) const
{
	CLLNode<CvString>* pCityNameNode;

	pCityNameNode = m_cityNames.nodeNum(iIndex);

	if(pCityNameNode != NULL)
	{
		return pCityNameNode->m_data;
	}
	else
	{
		return "";
	}
}


CLLNode<CvString>* CvPlayer::nextCityNameNode(CLLNode<CvString>* pNode)
{
	return m_cityNames.next(pNode);
}


const CLLNode<CvString>* CvPlayer::nextCityNameNode(const CLLNode<CvString>* pNode) const
{
	return m_cityNames.next(pNode);
}


CLLNode<CvString>* CvPlayer::headCityNameNode()
{
	return m_cityNames.head();
}


const CLLNode<CvString>* CvPlayer::headCityNameNode() const
{
	return m_cityNames.head();
}


CvCity* CvPlayer::firstCity(int* pIterIdx, bool bRev)
{
	return !bRev ? m_cities.BeginIter(pIterIdx) : m_cities.EndIter(pIterIdx);
}


const CvCity* CvPlayer::firstCity(int* pIterIdx, bool bRev) const
{
	return !bRev ? m_cities.BeginIter(pIterIdx) : m_cities.EndIter(pIterIdx);
}


CvCity* CvPlayer::nextCity(int* pIterIdx, bool bRev)
{
	return !bRev ? m_cities.NextIter(pIterIdx) : m_cities.PrevIter(pIterIdx);
}


const CvCity* CvPlayer::nextCity(int* pIterIdx, bool bRev) const
{
	return !bRev ? m_cities.NextIter(pIterIdx) : m_cities.PrevIter(pIterIdx);
}


int CvPlayer::getNumCities() const
{
	return m_cities.GetCount();
}



CvCity* CvPlayer::getCity(int iID)
{
	return(m_cities.GetAt(iID));
}


const CvCity* CvPlayer::getCity(int iID) const
{
	return(m_cities.GetAt(iID));
}



CvCity* CvPlayer::addCity()
{
	return(m_cities.Add());
}


void CvPlayer::deleteCity(int iID)
{
	m_cities.RemoveAt(iID);
}


CvCity* CvPlayer::GetFirstCityWithBuildingClass(BuildingClassTypes eBuildingClass)
{
	CvCity *pLoopCity;
	int iLoop;
	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		CvCivilizationInfo& playerCivilizationInfo = getCivilizationInfo();
		BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings((BuildingClassTypes)eBuildingClass);
		if (eBuilding != NO_BUILDING)
		{
			if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				return pLoopCity;
			}
		}
	}
#ifdef AUI_WARNING_FIXES
	return NULL;
#else
#if defined(LEKMOD_MACOS)
	return NULL;
#else
	return false;
#endif
#endif
}


const CvUnit* CvPlayer::firstUnit(int* pIterIdx, bool bRev) const
{
	return !bRev ? m_units.BeginIter(pIterIdx) : m_units.EndIter(pIterIdx);
}


const CvUnit* CvPlayer::nextUnit(int* pIterIdx, bool bRev) const
{
	return !bRev ? m_units.NextIter(pIterIdx) : m_units.PrevIter(pIterIdx);
}


CvUnit* CvPlayer::firstUnit(int* pIterIdx, bool bRev)
{
	return !bRev ? m_units.BeginIter(pIterIdx) : m_units.EndIter(pIterIdx);
}


CvUnit* CvPlayer::nextUnit(int* pIterIdx, bool bRev)
{
	return !bRev ? m_units.NextIter(pIterIdx) : m_units.PrevIter(pIterIdx);
}


int CvPlayer::getNumUnits() const
{
	return m_units.GetCount();
}



const CvUnit* CvPlayer::getUnit(int iID) const
{
	return (m_units.GetAt(iID));
}


CvUnit* CvPlayer::getUnit(int iID)
{
	return (m_units.GetAt(iID));
}


CvUnit* CvPlayer::addUnit()
{
	return (m_units.Add());
}



void CvPlayer::deleteUnit(int iID)
{
	m_units.RemoveAt(iID);
}



const CvArmyAI* CvPlayer::firstArmyAI(int* pIterIdx, bool bRev) const
{
	return !bRev ? m_armyAIs.BeginIter(pIterIdx) : m_armyAIs.EndIter(pIterIdx);
}



const CvArmyAI* CvPlayer::nextArmyAI(int* pIterIdx, bool bRev) const
{
	return !bRev ? m_armyAIs.NextIter(pIterIdx) : m_armyAIs.PrevIter(pIterIdx);
}


CvArmyAI* CvPlayer::firstArmyAI(int* pIterIdx, bool bRev)
{
	return !bRev ? m_armyAIs.BeginIter(pIterIdx) : m_armyAIs.EndIter(pIterIdx);
}



CvArmyAI* CvPlayer::nextArmyAI(int* pIterIdx, bool bRev)
{
	return !bRev ? m_armyAIs.NextIter(pIterIdx) : m_armyAIs.PrevIter(pIterIdx);
}


int CvPlayer::getNumArmyAIs() const
{
	return m_armyAIs.GetCount();
}



const CvArmyAI* CvPlayer::getArmyAI(int iID) const
{
	return ((CvArmyAI*)(m_armyAIs.GetAt(iID)));
}


CvArmyAI* CvPlayer::getArmyAI(int iID)
{
	return ((CvArmyAI*)(m_armyAIs.GetAt(iID)));
}



CvArmyAI* CvPlayer::addArmyAI()
{
	return ((CvArmyAI*)(m_armyAIs.Add()));
}



void CvPlayer::deleteArmyAI(int iID)
{
#ifdef CVASSERT_ENABLE
	bool bRemoved = m_armyAIs.RemoveAt(iID);
	DEBUG_VARIABLE(bRemoved);
	CvAssertMsg(bRemoved, "could not find army, delete failed");
#else
	m_armyAIs.RemoveAt(iID);
#endif
}



const CvAIOperation* CvPlayer::getAIOperation(int iID) const
{
	std::map<int, CvAIOperation*>::const_iterator it = m_AIOperations.find(iID);
	if(it != m_AIOperations.end())
	{
		return it->second;
	}
	return 0;
}


CvAIOperation* CvPlayer::getFirstAIOperation()
{
	CvAIOperation* rtnValue = NULL;

	m_CurrentOperation = m_AIOperations.begin();
	if(m_CurrentOperation != m_AIOperations.end())
	{
		rtnValue = m_CurrentOperation->second;
	}
	return rtnValue;
}


CvAIOperation* CvPlayer::getNextAIOperation()
{
	CvAIOperation* rtnValue = NULL;

	if(m_CurrentOperation != m_AIOperations.end())
	{
		++m_CurrentOperation;
		if(m_CurrentOperation != m_AIOperations.end())
		{
			rtnValue = m_CurrentOperation->second;
		}
	}
	return rtnValue;
}


CvAIOperation* CvPlayer::getAIOperation(int iID)
{
	std::map<int, CvAIOperation*>::iterator it = m_AIOperations.find(iID);
	if(it != m_AIOperations.end())
	{
		return it->second;
	}
	return 0;
}



CvAIOperation* CvPlayer::addAIOperation(int OperationType, PlayerTypes eEnemy, int iArea, CvCity* pTarget, CvCity* pMuster)
{
	CvAIOperation* pNewOperation = CvAIOperation::CreateOperation((AIOperationTypes) OperationType, m_eID);
	if(pNewOperation)
	{
		m_AIOperations.insert(std::make_pair(m_iNextOperationID.get(), pNewOperation));
		pNewOperation->Init(m_iNextOperationID, m_eID, eEnemy, iArea, pTarget, pMuster);
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
		m_iNextOperationID += 1;
#else
		m_iNextOperationID++;
#endif
	}
	return pNewOperation;
}



void CvPlayer::deleteAIOperation(int iID)
{
	std::map<int, CvAIOperation*>::iterator it = m_AIOperations.find(iID);
	if(it != m_AIOperations.end())
	{
		delete(it->second);
		m_AIOperations.erase(it);
	}
	else
	{
		CvAssertMsg(false, "could not find operation, delete failed");
	}
}


bool CvPlayer::haveAIOperationOfType(int iOperationType, int* piID                               , PlayerTypes eTargetPlayer                                         , CvPlot* pTarget                                         )
{

	std::map<int , CvAIOperation*>::iterator iter;
	for(iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		CvAIOperation* pThisOperation = iter->second;
		if(pThisOperation->GetOperationType() == iOperationType)
		{
			if(eTargetPlayer == NO_PLAYER || eTargetPlayer == pThisOperation->GetEnemy())
			{
				if(pTarget == NULL || pTarget == pThisOperation->GetTargetPlot())
				{

					if(piID != NULL)
					{
						*piID = pThisOperation->GetID();
					}
					return true;
				}
			}
		}
	}

	if(piID != NULL)
	{
		*piID = -1;
	}
	return false;
}


int CvPlayer::numOperationsOfType(int iOperationType)
{
	int iRtnValue = 0;

	std::map<int , CvAIOperation*>::iterator iter;
	for(iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		CvAIOperation* pThisOperation = iter->second;
		if(pThisOperation->GetOperationType() == iOperationType)
		{
			iRtnValue++;
		}
	}

	return iRtnValue;
}



bool CvPlayer::IsCityAlreadyTargeted(CvCity* pCity, DomainTypes eDomain, int iPercentToTarget, int iIgnoreOperationID) const
{
	CvAIOperation* pOperation;
	std::map<int , CvAIOperation*>::const_iterator iter;

	for(iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		pOperation = iter->second;

		if(pOperation)
		{
			if(iIgnoreOperationID == -1 || iIgnoreOperationID != pOperation->GetID())
			{
				if(pOperation->GetTargetPlot() == pCity->plot() && pOperation->PercentFromMusterPointToTarget() < iPercentToTarget)
				{

					if((eDomain == NO_DOMAIN || eDomain == DOMAIN_SEA) && pOperation->IsMixedLandNavalOperation())
					{
						return true;
					}

					if((eDomain == NO_DOMAIN || eDomain == DOMAIN_LAND) && !pOperation->IsMixedLandNavalOperation())
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}



bool CvPlayer::IsPlotTargetedForCity(CvPlot *pPlot) const
{
	CvAIOperation* pOperation;
	std::map<int , CvAIOperation*>::const_iterator iter;

	for(iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		pOperation = iter->second;
		if(pOperation)
		{
			switch (pOperation->GetOperationType())
			{
			case AI_OPERATION_FOUND_CITY:
			case AI_OPERATION_COLONIZE:
			case AI_OPERATION_QUICK_COLONIZE:
				{
					if (plotDistance(pPlot->getX(), pPlot->getY(), pOperation->GetTargetPlot()->getX(), pOperation->GetTargetPlot()->getY()) <= 2)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}


unsigned int CvPlayer::getNumReplayDataSets() const
{
	return m_ReplayDataSets.size();
}


const char* CvPlayer::getReplayDataSetName(unsigned int idx) const
{
	if(idx < m_ReplayDataSets.size())
		return m_ReplayDataSets[idx];

	return NULL;
}


unsigned int CvPlayer::getReplayDataSetIndex(const char* szDataSetName)
{
	CvString dataSetName = szDataSetName;

	unsigned int idx = 0;
	for(std::vector<CvString>::iterator it = m_ReplayDataSets.begin(); it != m_ReplayDataSets.end(); ++it)
	{
		if((*it) == dataSetName)
			return idx;

		idx++;
	}

	m_ReplayDataSets.push_back(dataSetName);
	m_ReplayDataSetValues.push_back(TurnData());
	return m_ReplayDataSets.size() - 1;
}


int CvPlayer::getReplayDataValue(unsigned int uiDataSet, unsigned int uiTurn) const
{
	if(uiDataSet < m_ReplayDataSetValues.size())
	{
		const TurnData& dataSet = m_ReplayDataSetValues[uiDataSet];
		TurnData::const_iterator it = dataSet.find(uiTurn);
		if(it != dataSet.end())
		{
			return (*it).second;
		}
	}

	return -1;
}

void CvPlayer::setReplayDataValue(unsigned int uiDataSet, unsigned int uiTurn, int iValue)
{
	if(uiDataSet < m_ReplayDataSetValues.size())
	{
		TurnData& dataSet = m_ReplayDataSetValues[uiDataSet];
		dataSet[uiTurn] = iValue;
	}
}


CvPlayer::TurnData CvPlayer::getReplayDataHistory(unsigned int uiDataSet) const
{
	if(uiDataSet < m_ReplayDataSetValues.size())
	{
		return m_ReplayDataSetValues[uiDataSet];
	}

	return CvPlayer::TurnData();
}


std::string CvPlayer::getScriptData() const
{
	return m_strScriptData;
}


#ifdef AUI_WARNING_FIXES
void CvPlayer::setScriptData(const std::string& strNewValue)
#else
void CvPlayer::setScriptData(std::string strNewValue)
#endif
{
	m_strScriptData = strNewValue;
}


const CvString& CvPlayer::getPbemEmailAddress() const
{
	return CvPreGame::emailAddress(GetID());
}


void CvPlayer::setPbemEmailAddress(const char* szAddress)
{
	CvPreGame::setEmailAddress(GetID(), szAddress);
}




void CvPlayer::doResearch()
{
	if(GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
	{
		return;
	}

	AI_PERF_FORMAT("AI-perf.csv", ("CvPlayer::doResearch, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), getCivilizationShortDescription()) );
	bool bForceResearchChoice;
	int iOverflowResearch;

	if(GetPlayerTechs()->IsResearch())
	{
		bForceResearchChoice = false;


		if(GetPlayerTechs()->GetCurrentResearch() == NO_TECH)
		{
#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
			if (GetID() == GC.getGame().getActivePlayer() && getCachedScienceT100ForThisTurn() > 0)
#else
			if(GetID() == GC.getGame().getActivePlayer() && GetScienceTimes100() > 0)
#endif
			{
				chooseTech();
			}

			if(GC.getGame().getElapsedGameTurns() > 4)
			{
				AI_chooseResearch();

				bForceResearchChoice = true;
			}
		}

		TechTypes eCurrentTech = GetPlayerTechs()->GetCurrentResearch();
		if(eCurrentTech == NO_TECH)
		{
#if defined(AUI_PLAYER_FIX_NO_RESEARCH_OVERFLOW_DOUBLE_DIP) && defined(AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE)
			int iOverflow = getCachedScienceT100ForThisTurn();
#elif defined(AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE)
			int iOverflow = (getCachedScienceT100ForThisTurn()) / MAX(1, calculateResearchModifier(eCurrentTech));
#elif defined(AUI_PLAYER_FIX_NO_RESEARCH_OVERFLOW_DOUBLE_DIP)
			int iOverflow = GetScienceTimes100();
#else
			int iOverflow = (GetScienceTimes100()) / std::max(1, calculateResearchModifier(eCurrentTech));
#endif
			changeOverflowResearchTimes100(iOverflow);
		}
		else
		{
#ifdef AUI_PLAYER_FIX_NO_RESEARCH_OVERFLOW_DOUBLE_DIP
			iOverflowResearch = getOverflowResearchTimes100();
#else
			iOverflowResearch = (getOverflowResearchTimes100() * calculateResearchModifier(eCurrentTech)) / 100;
#endif
			setOverflowResearch(0);
			if(GET_TEAM(getTeam()).GetTeamTechs())
			{
#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
				int iBeakersTowardsTechTimes100 = getCachedScienceT100ForThisTurn() + iOverflowResearch;
				GET_TEAM(getTeam()).GetTeamTechs()->ChangeResearchProgressTimes100(eCurrentTech, iBeakersTowardsTechTimes100, GetID());
				UpdateResearchAgreements(getCachedScienceT100ForThisTurn() / 100);
#else
				int iBeakersTowardsTechTimes100 = GetScienceTimes100() + iOverflowResearch;
				GET_TEAM(getTeam()).GetTeamTechs()->ChangeResearchProgressTimes100(eCurrentTech, iBeakersTowardsTechTimes100, GetID());
				UpdateResearchAgreements(GetScienceTimes100() / 100);
#endif
			}
		}

		if(bForceResearchChoice)
		{
			clearResearchQueue();
		}
	}
	GetPlayerTechs()->CheckForTechAchievement();

}


void CvPlayer::doAdvancedStartAction(AdvancedStartActionTypes eAction, int iX, int iY, int iData, bool bAdd)
{
	CvPlot* pPlot = GC.getMap().plot(iX, iY);

	if(0 == getNumCities())
	{
		switch(eAction)
		{
		case ADVANCEDSTARTACTION_EXIT:

			if(GetID() == GC.getGame().getActivePlayer())
			{
				GC.GetEngineUserInterface()->setBusy(true);
			}
			if(GetID() == GC.getGame().getActivePlayer())
			{
				GC.GetEngineUserInterface()->setBusy(false);
			}
			break;
		case ADVANCEDSTARTACTION_AUTOMATE:
		case ADVANCEDSTARTACTION_CITY:
			break;
		default:


			return;
		}
	}

	switch(eAction)
	{
	case ADVANCEDSTARTACTION_EXIT:
		GetTreasury()->ChangeGold(getAdvancedStartPoints());
		setAdvancedStartPoints(-1);
		if(GC.getGame().getActivePlayer() == GetID())
		{
			GC.GetEngineUserInterface()->setInAdvancedStart(false);
		}

		if(isHuman())
		{
			int iLoop;
			for(CvCity* pCity = firstCity(&iLoop); NULL != pCity; pCity = nextCity(&iLoop))
			{
				pCity->chooseProduction();
			}

			chooseTech();
		}
		break;
	case ADVANCEDSTARTACTION_AUTOMATE:
		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setBusy(true);
		}
		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setBusy(false);
		}
		break;
	case ADVANCEDSTARTACTION_UNIT:
	{
		if(pPlot == NULL)
			return;

		UnitTypes eUnit = (UnitTypes) iData;
		int iCost = getAdvancedStartUnitCost(eUnit, bAdd, pPlot);

		if(bAdd && iCost < 0)
		{
			return;
		}


		if(bAdd)
		{
			if(getAdvancedStartPoints() >= iCost)
			{
				CvUnit* pUnit = initUnit(eUnit, iX, iY);
				if(NULL != pUnit)
				{
					pUnit->finishMoves();
					changeAdvancedStartPoints(-iCost);
				}
			}
		}


		else
		{

			if(iCost != -1)
			{
				IDInfo* pUnitNode = pPlot->headUnitNode();
				while(pUnitNode != NULL)
				{
					CvUnit* pLoopUnit = ::getUnit(*pUnitNode);
					pUnitNode = pPlot->nextUnitNode(pUnitNode);

					if(NULL != pLoopUnit && pLoopUnit->getUnitType() == eUnit)
					{
						pLoopUnit->kill(false);
						changeAdvancedStartPoints(iCost);
						return;
					}
				}
			}


			IDInfo* pUnitNode = pPlot->headUnitNode();
			if(pUnitNode != NULL)
			{
				CvUnit* pUnit = ::getUnit(*pUnitNode);

				iCost = getAdvancedStartUnitCost(pUnit->getUnitType(), false);
				CvAssertMsg(iCost != -1, "If this is -1 then that means it's going to try to delete a unit which shouldn't exist");
				pUnit->kill(false);
				changeAdvancedStartPoints(iCost);
			}
		}

		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(Advanced_Start_DIRTY_BIT, true);
		}
	}
	break;
	case ADVANCEDSTARTACTION_CITY:
	{
		if(pPlot == NULL)
			return;

		int iCost = getAdvancedStartCityCost(bAdd, pPlot);

		if(iCost < 0)
		{
			return;
		}


		if(bAdd)
		{
			if(0 == getNumCities())
			{
				PlayerTypes eClosestPlayer = NO_PLAYER;
				int iMinDistance = INT_MAX;
				for(int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++)
				{
					CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
					if(kPlayer.isAlive())
					{
						if(kPlayer.getTeam() == getTeam())
						{
							if(0 == kPlayer.getNumCities())
							{
								CvAssert(kPlayer.getStartingPlot() != NULL);
								int iDistance = plotDistance(iX, iY, kPlayer.getStartingPlot()->getX(), kPlayer.getStartingPlot()->getY());
								if(iDistance < iMinDistance)
								{
									eClosestPlayer = kPlayer.GetID();
									iMinDistance = iDistance;
								}
							}
						}
					}
				}
				CvAssertMsg(eClosestPlayer != NO_PLAYER, "Self at a minimum should always be valid");
				if(eClosestPlayer != GetID())
				{
					CvPlot* pTempPlot = GET_PLAYER(eClosestPlayer).getStartingPlot();
					GET_PLAYER(eClosestPlayer).setStartingPlot(getStartingPlot());
					setStartingPlot(pTempPlot);
				}
			}
			if(getAdvancedStartPoints() >= iCost || 0 == getNumCities())
			{
				found(iX, iY);
				changeAdvancedStartPoints(-iCost);
				CvCity* pCity = pPlot->getPlotCity();
				if(pCity != NULL)
				{
					if(pCity->getPopulation() > 1)
					{
						pCity->setFood(pCity->growthThreshold() / 2);
					}
				}
			}
		}


		else
		{
			pPlot->setRouteType(NO_ROUTE);
			pPlot->getPlotCity()->kill();
			pPlot->setImprovementType(NO_IMPROVEMENT);
			changeAdvancedStartPoints(iCost);
		}

		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(Advanced_Start_DIRTY_BIT, true);
		}
	}
	break;
	case ADVANCEDSTARTACTION_POP:
	{
		if(pPlot == NULL)
			return;

		CvCity* pCity = pPlot->getPlotCity();

		if(pCity != NULL)
		{
			int iCost = getAdvancedStartPopCost(bAdd, pCity);

			if(iCost < 0)
			{
				return;
			}


			if(bAdd)
			{
				if(getAdvancedStartPoints() >= iCost)
				{
					pCity->changePopulation(1);
					changeAdvancedStartPoints(-iCost);
					if(pCity->getPopulation() > 1)
					{
						pCity->setFood(pCity->growthThreshold() / 2);
						pCity->setFoodKept((pCity->getFood() * pCity->getMaxFoodKeptPercent()) / 100);
					}
				}
			}


			else
			{
				pCity->changePopulation(-1);
				changeAdvancedStartPoints(iCost);
				if(pCity->getPopulation() == 1)
				{
					pCity->setFood(0);
					pCity->setFoodKept(0);
				}
			}
		}
	}
	break;
	case ADVANCEDSTARTACTION_BUILDING:
	{
		if(pPlot == NULL)
			return;

		CvCity* pCity = pPlot->getPlotCity();

		if(pCity != NULL)
		{
			BuildingTypes eBuilding = (BuildingTypes) iData;

			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if(pkBuildingInfo == NULL)
			{
				return;
			}

			int iCost = getAdvancedStartBuildingCost(eBuilding, bAdd, pCity);
			if(iCost < 0)
			{
				return;
			}


			if(bAdd)
			{
				if(getAdvancedStartPoints() >= iCost)
				{
					pCity->GetCityBuildings()->SetNumRealBuilding(eBuilding, pCity->GetCityBuildings()->GetNumRealBuilding(eBuilding)+1);
					changeAdvancedStartPoints(-iCost);
					if(pkBuildingInfo->GetFoodKept() != 0)
					{
						pCity->setFoodKept((pCity->getFood() * pCity->getMaxFoodKeptPercent()) / 100);
					}
				}
			}


			else
			{
				pCity->GetCityBuildings()->SetNumRealBuilding(eBuilding, pCity->GetCityBuildings()->GetNumRealBuilding(eBuilding)-1);
				changeAdvancedStartPoints(iCost);
				if(pkBuildingInfo->GetFoodKept() != 0)
				{
					pCity->setFoodKept((pCity->getFood() * pCity->getMaxFoodKeptPercent()) / 100);
				}
			}
		}

		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(Advanced_Start_DIRTY_BIT, true);
		}
	}
	break;
	case ADVANCEDSTARTACTION_ROUTE:
	{
		if(pPlot == NULL)
			return;

		RouteTypes eRoute = (RouteTypes) iData;
		int iCost = getAdvancedStartRouteCost(eRoute, bAdd, pPlot);

		if(bAdd && iCost < 0)
		{
			return;
		}


		if(bAdd)
		{
			if(getAdvancedStartPoints() >= iCost)
			{
				pPlot->setRouteType(eRoute);
				changeAdvancedStartPoints(-iCost);
			}
		}


		else
		{
			if(pPlot->getRouteType() != eRoute)
			{
				eRoute = pPlot->getRouteType();
				iCost = getAdvancedStartRouteCost(eRoute, bAdd);
			}

			if(iCost < 0)
			{
				return;
			}

			pPlot->setRouteType(NO_ROUTE);
			changeAdvancedStartPoints(iCost);
		}

		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(Advanced_Start_DIRTY_BIT, true);
		}
	}
	break;
	case ADVANCEDSTARTACTION_IMPROVEMENT:
	{
		if(pPlot == NULL)
			return;

		ImprovementTypes eImprovement = (ImprovementTypes) iData;
		int iCost = getAdvancedStartImprovementCost(eImprovement, bAdd, pPlot);

		if(bAdd && iCost < 0)
		{
			return;
		}


		if(bAdd)
		{
			if(getAdvancedStartPoints() >= iCost)
			{
				if(pPlot->getFeatureType() != NO_FEATURE)
				{
#ifdef AUI_WARNING_FIXES
					for (uint iI = 0; iI < GC.getNumBuildInfos(); ++iI)
#else
					for(int iI = 0; iI < GC.getNumBuildInfos(); ++iI)
#endif
					{
						CvBuildInfo* pkBuildInfo = GC.getBuildInfo((BuildTypes) iI);
						if(!pkBuildInfo)
						{
							continue;
						}

						ImprovementTypes eLoopImprovement = ((ImprovementTypes)(pkBuildInfo->getImprovement()));

						if(eImprovement == eLoopImprovement)
						{
							if(pkBuildInfo->isFeatureRemove(pPlot->getFeatureType()) && canBuild(pPlot, (BuildTypes)iI))
							{
								pPlot->setFeatureType(NO_FEATURE);
								break;
							}
						}
					}
				}

				pPlot->setImprovementType(eImprovement, GetID());

				changeAdvancedStartPoints(-iCost);
			}
		}


		else
		{
			if(pPlot->getImprovementType() != eImprovement)
			{
				eImprovement = pPlot->getImprovementType();
				iCost = getAdvancedStartImprovementCost(eImprovement, bAdd, pPlot);
			}

			if(iCost < 0)
			{
				return;
			}

			pPlot->setImprovementType(NO_IMPROVEMENT);
			changeAdvancedStartPoints(iCost);
		}

		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(Advanced_Start_DIRTY_BIT, true);
		}
	}
	break;
	case ADVANCEDSTARTACTION_TECH:
	{
		TechTypes eTech = (TechTypes) iData;
		int iCost = getAdvancedStartTechCost(eTech, bAdd);

		if(iCost < 0)
		{
			return;
		}


		if(bAdd)
		{
			if(getAdvancedStartPoints() >= iCost)
			{
				GET_TEAM(getTeam()).setHasTech(eTech, true, GetID(), false, false);
				changeAdvancedStartPoints(-iCost);
			}
		}


		else
		{
			GET_TEAM(getTeam()).setHasTech(eTech, false, GetID(), false, false);
			changeAdvancedStartPoints(iCost);
		}

		if(GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(Advanced_Start_DIRTY_BIT, true);
		}
	}
	break;
	case ADVANCEDSTARTACTION_VISIBILITY:
	{
		if(pPlot == NULL)
			return;

		int iCost = getAdvancedStartVisibilityCost(bAdd, pPlot);

		if(iCost < 0)
		{
			return;
		}


		if(bAdd)
		{
			if(getAdvancedStartPoints() >= iCost)
			{
				pPlot->setRevealed(getTeam(), true, true);
				changeAdvancedStartPoints(-iCost);
			}
		}


		else
		{
			pPlot->setRevealed(getTeam(), false, true);
			changeAdvancedStartPoints(iCost);
		}
	}
	break;
	default:
		CvAssert(false);
		break;
	}
}





int CvPlayer::getAdvancedStartUnitCost(UnitTypes eUnit, bool bAdd, CvPlot* pPlot)
{
	if(0 == getNumCities())
	{
		return -1;
	}

	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	if(pkUnitInfo == NULL)
	{
		return -1;
	}

	int iCost = (getProductionNeeded(eUnit) * pkUnitInfo->GetAdvancedStartCost()) / 100;
	if(iCost < 0)
	{
		return -1;
	}

	if(NULL == pPlot)
	{
		if(bAdd)
		{
			bool bValid = false;
			int iLoop;
			for(CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
			{
				if(pLoopCity->canTrain(eUnit))
				{
					bValid = true;
					break;
				}
			}

			if(!bValid)
			{
				return -1;
			}
		}
	}
	else
	{
		CvCity* pCity = NULL;

		if(0 == GC.getADVANCED_START_ALLOW_UNITS_OUTSIDE_CITIES())
		{
			pCity = pPlot->getPlotCity();

			if(NULL == pCity || pCity->getOwner() != GetID())
			{
				return -1;
			}

			iCost *= 100;
			iCost /= std::max(1, 100 + pCity->getProductionModifier(eUnit));
		}
		else
		{
			if(pPlot->getOwner() != GetID())
			{
				return -1;
			}

			iCost *= 100;
			iCost /= std::max(1, 100 + getProductionModifier(eUnit));
		}


		if(bAdd)
		{
			int iMaxUnitsPerCity = GC.getADVANCED_START_MAX_UNITS_PER_CITY();
			if(iMaxUnitsPerCity >= 0)
			{
				if(pkUnitInfo->IsMilitarySupport() && getNumMilitaryUnits() >= iMaxUnitsPerCity * getNumCities())
				{
					return -1;
				}
			}

			if(NULL != pCity)
			{
				if(!pCity->canTrain(eUnit))
				{
					return -1;
				}
			}
			else
			{
				if(!pPlot->canTrain(eUnit, false, false))
				{
					return -1;
				}

				if(pPlot->isImpassable() || pPlot->isMountain())
				{
					return -1;
				}
			}
		}

		else
		{
			bool bUnitFound = false;

			IDInfo* pUnitNode = pPlot->headUnitNode();
			while(pUnitNode != NULL)
			{
				CvUnit* pLoopUnit = ::getUnit(*pUnitNode);
				pUnitNode = pPlot->nextUnitNode(pUnitNode);

				if(NULL != pLoopUnit && pLoopUnit->getUnitType() == eUnit)
				{
					bUnitFound = true;
				}
			}

			if(!bUnitFound)
			{
				return -1;
			}
		}
	}

	return iCost;
}





int CvPlayer::getAdvancedStartCityCost(bool bAdd, CvPlot* pPlot)
{
	int iNumCities = getNumCities();

	int iCost = getNewCityProductionValue();

	if(iCost < 0)
	{
		return -1;
	}


	if(pPlot != NULL)
	{

		if(bAdd)
		{
			if(!canFound(pPlot->getX(), pPlot->getY(), false))
			{
				return -1;
			}
		}

		else
		{
			if(pPlot->isCity())
			{
				if(pPlot->getPlotCity()->getOwner() != GetID())
				{
					return -1;
				}
			}
			else
			{
				return -1;
			}
		}


		if(GC.getADVANCED_START_CITY_PLACEMENT_MAX_RANGE() > 0)
		{
			PlayerTypes eClosestPlayer = NO_PLAYER;
			int iClosestDistance = INT_MAX;

			for(int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; ++iPlayer)
			{
				CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);

				if(kPlayer.isAlive())
				{
					CvPlot* pStartingPlot = kPlayer.getStartingPlot();

					if(NULL != pStartingPlot)
					{
						int iDistance = ::plotDistance(pPlot->getX(), pPlot->getY(), pStartingPlot->getX(), pStartingPlot->getY());
						if(iDistance <= GC.getADVANCED_START_CITY_PLACEMENT_MAX_RANGE())
						{
							if(iDistance < iClosestDistance || (iDistance == iClosestDistance && getTeam() != kPlayer.getTeam()))
							{
								iClosestDistance = iDistance;
								eClosestPlayer = kPlayer.GetID();
							}
						}
					}
				}
			}

			if(NO_PLAYER == eClosestPlayer || GET_PLAYER(eClosestPlayer).getTeam() != getTeam())
			{
				return -1;
			}


			if((GetID() != eClosestPlayer) && ((getNumCities() > 0) || (GET_PLAYER(eClosestPlayer).getNumCities() > 0)))
			{
				return -1;
			}
		}
	}


	if(0 != GC.getADVANCED_START_CITY_COST_INCREASE())
	{
		if(!bAdd)
		{
			--iNumCities;
		}

		if(iNumCities > 0)
		{
			iCost *= 100 + GC.getADVANCED_START_CITY_COST_INCREASE() * iNumCities;
			iCost /= 100;
		}
	}

	return iCost;
}





int CvPlayer::getAdvancedStartPopCost(bool bAdd, CvCity* pCity)
{
	if(0 == getNumCities())
	{
		return -1;
	}

	int iCost = (getGrowthThreshold(1) * GC.getADVANCED_START_POPULATION_COST()) / 100;

	if(NULL != pCity)
	{
		if(pCity->getOwner() != GetID())
		{
			return -1;
		}

		int iPopulation = pCity->getPopulation();


		if(!bAdd)
		{
			--iPopulation;

			if(iPopulation < GC.getINITIAL_CITY_POPULATION() + GC.getGame().getStartEraInfo().getFreePopulation())
			{
				return -1;
			}
		}

		iCost = (getGrowthThreshold(iPopulation) * GC.getADVANCED_START_POPULATION_COST()) / 100;


		if(0 != GC.getADVANCED_START_POPULATION_COST_INCREASE())
		{
			--iPopulation;

			if(iPopulation > 0)
			{
				iCost *= 100 + GC.getADVANCED_START_POPULATION_COST_INCREASE() * iPopulation;
				iCost /= 100;
			}
		}
	}

	return iCost;
}





int CvPlayer::getAdvancedStartBuildingCost(BuildingTypes eBuilding, bool bAdd, CvCity* pCity)
{
	if(0 == getNumCities())
	{
		return -1;
	}

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pkBuildingInfo == NULL)
	{
		return -1;
	}

	int iCost = getProductionNeeded(eBuilding);

	if(iCost < 0)
	{
		return -1;
	}

	if(pkBuildingInfo->GetFreeStartEra() != NO_ERA && GC.getGame().getStartEra() >=  pkBuildingInfo->GetFreeStartEra())
	{

		return -1;
	}

	if(NULL == pCity)
	{
		if(bAdd)
		{
			bool bValid = false;
			int iLoop;
			for(CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
			{
				if(pLoopCity->canConstruct(eBuilding))
				{
					bValid = true;
					break;
				}
			}

			if(!bValid)
			{
				return -1;
			}
		}
	}
	if(NULL != pCity)
	{
		if(pCity->getOwner() != GetID())
		{
			return -1;
		}

		iCost *= 100;
		iCost /= std::max(1, 100 + pCity->getProductionModifier(eBuilding));

		if(bAdd)
		{
			if(!pCity->canConstruct(eBuilding, true, false, false))
			{
				return -1;
			}
		}
		else
		{
			if(pCity->GetCityBuildings()->GetNumRealBuilding(eBuilding) <= 0)
			{
				return -1;
			}




#ifdef AUI_WARNING_FIXES
			for (uint iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
#else
			for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
#endif
			{
				const BuildingTypes eBuildingLoop = static_cast<BuildingTypes>(iBuildingLoop);
				CvBuildingEntry* pkBuildingLoopInfo = GC.getBuildingInfo(eBuildingLoop);
				if(pkBuildingLoopInfo)
				{
					if(pCity->GetCityBuildings()->GetNumBuilding(eBuildingLoop) > 0)
					{

#ifdef AUI_WARNING_FIXES
						for (uint iBuildingClassPrereqLoop = 0; iBuildingClassPrereqLoop < GC.getNumBuildingClassInfos(); iBuildingClassPrereqLoop++)
#else
						for(int iBuildingClassPrereqLoop = 0; iBuildingClassPrereqLoop < GC.getNumBuildingClassInfos(); iBuildingClassPrereqLoop++)
#endif
						{
							const BuildingClassTypes eBuildingClass = static_cast<BuildingClassTypes>(iBuildingClassPrereqLoop);
							CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
							if(pkBuildingClassInfo)
							{
								if(pkBuildingLoopInfo->IsBuildingClassNeededInCity(iBuildingClassPrereqLoop))
								{
									if((BuildingTypes)(getCivilizationInfo().getCivilizationBuildings(iBuildingClassPrereqLoop)) == eBuilding)
									{
										return -1;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return iCost;
}





int CvPlayer::getAdvancedStartRouteCost(RouteTypes eRoute, bool bAdd, CvPlot* pPlot)
{
	if(0 == getNumCities())
	{
		return -1;
	}

	if(eRoute == NO_ROUTE)
	{
		return -1;
	}

	CvRouteInfo* pkRouteInfo = GC.getRouteInfo(eRoute);
	if(pkRouteInfo == NULL)
	{
		return -1;
	}

	int iCost = pkRouteInfo->getAdvancedStartCost();


	if(iCost < 0)
	{
		return -1;
	}

	iCost *= GC.getGame().getGameSpeedInfo().getBuildPercent();
	iCost /= 100;


	if(pPlot != NULL)
	{
		if(pPlot->isCity())
		{
			return -1;
		}

		if(bAdd)
		{
			if(pPlot->isImpassable() || pPlot->isWater() || pPlot->isMountain())
			{
				return -1;
			}

			if(pPlot->getRouteType() == eRoute)
			{
				return -1;
			}
		}
		else
		{

			if(pPlot->getRouteType() != eRoute)
			{
				return -1;
			}
		}


		if(pPlot->getOwner() != GetID())
		{
			return -1;
		}
	}


#ifdef AUI_WARNING_FIXES
	for (uint iBuildLoop = 0; iBuildLoop < GC.getNumBuildInfos(); iBuildLoop++)
#else
	for(int iBuildLoop = 0; iBuildLoop < GC.getNumBuildInfos(); iBuildLoop++)
#endif
	{
		const BuildTypes eBuild = static_cast<BuildTypes>(iBuildLoop);
		CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
		if(pkBuildInfo)
		{
			if(pkBuildInfo->getRoute() == eRoute)
			{
				if(!(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)pkBuildInfo->getTechPrereq())))
				{
					return -1;
				}
			}
		}
	}

	return iCost;
}





int CvPlayer::getAdvancedStartImprovementCost(ImprovementTypes eImprovement, bool bAdd, CvPlot* pPlot)
{
	if(eImprovement == NO_IMPROVEMENT)
	{
		return -1;
	}

	if(0 == getNumCities())
	{
		return -1;
	}

	int iCost = 0;


	if(iCost < 0)
	{
		return -1;
	}

	iCost *= GC.getGame().getGameSpeedInfo().getBuildPercent();
	iCost /= 100;


	if(pPlot != NULL)
	{
		if(bAdd)
		{

			if(!pPlot->canHaveImprovement(eImprovement, getTeam(), false))
			{
				return -1;
			}

			bool bValid = false;

#ifdef AUI_WARNING_FIXES
			for (uint iI = 0; iI < GC.getNumBuildInfos(); ++iI)
#else
			for(int iI = 0; iI < GC.getNumBuildInfos(); ++iI)
#endif
			{
				CvBuildInfo* pkBuildInfo = GC.getBuildInfo((BuildTypes) iI);
				if(!pkBuildInfo)
				{
					continue;
				}
				ImprovementTypes eLoopImprovement = ((ImprovementTypes)(pkBuildInfo->getImprovement()));

				if(eImprovement == eLoopImprovement && canBuild(pPlot, (BuildTypes)iI))
				{
					bValid = true;

					FeatureTypes eFeature = pPlot->getFeatureType();
					if(NO_FEATURE != eFeature && pkBuildInfo->isFeatureRemove(eFeature))
					{
						iCost += GC.getFeatureInfo(eFeature)->getAdvancedStartRemoveCost();
					}

					break;
				}
			}

			if(!bValid)
			{
				return -1;
			}


			if(pPlot->getImprovementType() == eImprovement)
			{
				return -1;
			}
		}
		else
		{

			if(pPlot->getImprovementType() != eImprovement)
			{
				return -1;
			}
		}


		if(pPlot->getOwner() != GetID())
		{
			return -1;
		}
	}


#ifdef AUI_WARNING_FIXES
	for (uint iBuildLoop = 0; iBuildLoop < GC.getNumBuildInfos(); iBuildLoop++)
#else
	for(int iBuildLoop = 0; iBuildLoop < GC.getNumBuildInfos(); iBuildLoop++)
#endif
	{
		CvBuildInfo* pkBuildInfo = GC.getBuildInfo((BuildTypes) iBuildLoop);
		if(!pkBuildInfo)
		{
			continue;
		}

		if(pkBuildInfo->getImprovement() == eImprovement)
		{
			if(!(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)pkBuildInfo->getTechPrereq())))
			{
				return -1;
			}
		}
	}

	return iCost;
}





int CvPlayer::getAdvancedStartTechCost(TechTypes eTech, bool bAdd)
{
	if(eTech == NO_TECH)
	{
		return -1;
	}

	if(0 == getNumCities())
	{
		return -1;
	}

#ifdef AUI_TECH_FIX_PLAYER_BASED_RESEARCH_COST_ONLY_AESTHETIC
	int iCost = (GetPlayerTechs()->GetResearchCost(eTech) * GC.getTechInfo(eTech)->GetAdvancedStartCost()) / 100;
#else
	int iCost = (GET_TEAM(getTeam()).GetTeamTechs()->GetResearchCost(eTech) * GC.getTechInfo(eTech)->GetAdvancedStartCost()) / 100;
#endif
	if(iCost < 0)
	{
		return -1;
	}

	if(bAdd)
	{
		if(!GetPlayerTechs()->CanResearch(eTech, false))
		{
			return -1;
		}
	}
	else if(!bAdd)
	{
		if(!GET_TEAM(getTeam()).GetTeamTechs()->HasTech(eTech))
		{
			return -1;
		}


#ifdef AUI_WARNING_FIXES
		for (uint iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
#else
		for(int iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
#endif
		{
			TechTypes eTechLoop = (TechTypes) iTechLoop;

			if(GET_TEAM(getTeam()).GetTeamTechs()->HasTech(eTechLoop))
			{
				int iPrereqLoop;


				for(iPrereqLoop = 0; iPrereqLoop < GC.getNUM_OR_TECH_PREREQS(); iPrereqLoop++)
				{
					if(GC.getTechInfo(eTechLoop)->GetPrereqOrTechs(iPrereqLoop) == eTech)
					{
						return -1;
					}
				}


				for(iPrereqLoop = 0; iPrereqLoop < GC.getNUM_AND_TECH_PREREQS(); iPrereqLoop++)
				{
					if(GC.getTechInfo(eTechLoop)->GetPrereqAndTechs(iPrereqLoop) == eTech)
					{
						return -1;
					}
				}
			}
		}


		int iLoop;


		CvUnit* pLoopUnit;
		for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
		{
			if(pLoopUnit->getUnitInfo().GetPrereqAndTech() == eTech)
			{
				return -1;
			}

			for(int iI = 0; iI < GC.getNUM_UNIT_AND_TECH_PREREQS(); iI++)
			{
				if(pLoopUnit->getUnitInfo().GetPrereqAndTechs(iI) == eTech)
				{
					return -1;
				}
			}
		}


		CvCity* pLoopCity;
		for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{

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
					if(pLoopCity->GetCityBuildings()->GetNumRealBuilding(eBuilding) > 0)
					{
						if(pkBuildingInfo->GetPrereqAndTech() == eTech)
						{
							return -1;
						}

						for(int iI = 0; iI < GC.getNUM_BUILDING_AND_TECH_PREREQS(); iI++)
						{
							if(pkBuildingInfo->GetPrereqAndTechs(iI) == eTech)
							{
								return -1;
							}
						}
					}
				}
			}
		}

	}

	return iCost;
}





int CvPlayer::getAdvancedStartVisibilityCost(bool bAdd, CvPlot* pPlot)
{
	if(0 == getNumCities())
	{
		return -1;
	}

	int iNumVisiblePlots = 0;
	int iCost = GC.getADVANCED_START_VISIBILITY_COST();


	if(iCost == -1)
	{
		return -1;
	}


	if(pPlot != NULL)
	{
		if(bAdd)
		{
			if(pPlot->isRevealed(getTeam()))
			{
				return -1;
			}
			if(!pPlot->isAdjacentRevealed(getTeam()))
			{
				return -1;
			}
		}
		else
		{
			if(!pPlot->isRevealed(getTeam()))
			{
				return -1;
			}
		}
	}


	if(0 != GC.getADVANCED_START_VISIBILITY_COST_INCREASE())
	{
#ifdef AUI_WARNING_FIXES
		for (uint iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
#else
		const int nPlots = GC.getMap().numPlots();
		for(int iPlotLoop = 0; iPlotLoop < nPlots; iPlotLoop++)
#endif
		{
			CvPlot* pMapPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

			if(pMapPlot->isRevealed(getTeam()))
			{
				++iNumVisiblePlots;
			}
		}

		if(!bAdd)
		{
			--iNumVisiblePlots;
		}

		if(iNumVisiblePlots > 0)
		{
			iCost *= 100 + GC.getADVANCED_START_VISIBILITY_COST_INCREASE() * iNumVisiblePlots;
			iCost /= 100;
		}
	}

	return iCost;
}


void CvPlayer::doWarnings()
{
	if(m_eID == GC.getGame().getActivePlayer())
	{

		int iMaxCount = range(((getNumCities() + 4) / 7), 2, 5);
#ifdef AUI_WARNING_FIXES
		for (uint iI = 0; iI < GC.getMap().numPlots(); iI++)
#else
		for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
#endif
		{
			if(iMaxCount == 0)
			{
				break;
			}

			CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);

			if(pLoopPlot->isAdjacentPlayer(GetID()))
			{
				if(!(pLoopPlot->isCity()))
				{
					if(pLoopPlot->isVisible(getTeam()))
					{
						CvUnit* pUnit = pLoopPlot->getVisibleEnemyDefender(GetID());
						if(pUnit != NULL)
						{
							CvCity* pNearestCity = GC.getMap().findCity(pLoopPlot->getX(), pLoopPlot->getY(), GetID(), NO_TEAM, !(pLoopPlot->isWater()));

							if(pNearestCity != NULL)
							{
								CvString message = GetLocalizedText("TXT_KEY_MISC_ENEMY_TROOPS_SPOTTED", pNearestCity->getNameKey());
								GC.GetEngineUserInterface()->AddPlotMessage(0, pLoopPlot->GetPlotIndex(), GetID(), true, GC.getEVENT_MESSAGE_TIME(), message);

								iMaxCount--;
							}
						}
					}
				}
			}
		}
	}
}


void CvPlayer::processPolicies(PolicyTypes ePolicy, int iChange)
{
#ifdef AUI_WARNING_FIXES
	uint iI;
	int iJ;
#else
	int iI, iJ;
#endif

	CvPolicyEntry* pPolicy = GC.getPolicyInfo(ePolicy);
	if(pPolicy == NULL)
		return;

	const CvPolicyEntry& kPolicy = (*pPolicy);

	ChangeCulturePerWonder(pPolicy->GetCulturePerWonder() * iChange);
	ChangeCultureWonderMultiplier(pPolicy->GetCultureWonderMultiplier() * iChange);
	ChangeCulturePerTechResearched(pPolicy->GetCulturePerTechResearched() * iChange);
	ChangeGoldenAgeMeterMod(pPolicy->GetGoldenAgeMeterMod() * iChange);
	changeGoldenAgeModifier(pPolicy->GetGoldenAgeDurationMod() * iChange);
	changeWorkerSpeedModifier(pPolicy->GetWorkerSpeedModifier() * iChange);
	changeImprovementCostModifier(pPolicy->GetImprovementCostModifier() * iChange);
	changeImprovementUpgradeRateModifier(pPolicy->GetImprovementUpgradeRateModifier() * iChange);
	changeSpecialistProductionModifier(pPolicy->GetSpecialistProductionModifier() * iChange);
	changeMilitaryProductionModifier(pPolicy->GetMilitaryProductionModifier() * iChange);
	changeBaseFreeUnits(pPolicy->GetBaseFreeUnits() * iChange);
	ChangeHappinessPerGarrisonedUnit(pPolicy->GetHappinessPerGarrisonedUnit() * iChange);
	ChangeHappinessPerTradeRoute(pPolicy->GetHappinessPerTradeRoute() * iChange);
#ifdef NQ_RAIL_CONNECTION_HAPPINESS_FROM_POLICIES
	ChangeHappinessPerRailConnection(pPolicy->GetHappinessPerRailConnection() * iChange);
#endif
#if defined(MISC_CHANGES)
	ChangePolicyExtraLeagueVotes(pPolicy->GetNumExtraLeagueVotes() * iChange);
#endif
	ChangeHappinessPerXPopulation(pPolicy->GetHappinessPerXPopulation() * iChange);
	ChangeExtraHappinessPerLuxury(pPolicy->GetExtraHappinessPerLuxury() * iChange);
	ChangeUnhappinessFromUnitsMod(pPolicy->GetUnhappinessFromUnitsMod() * iChange);
	ChangeUnhappinessMod(pPolicy->GetUnhappinessMod() * iChange);
	ChangeCityCountUnhappinessMod(pPolicy->GetCityCountUnhappinessMod() * iChange);
	ChangeOccupiedPopulationUnhappinessMod(pPolicy->GetOccupiedPopulationUnhappinessMod() * iChange);
	ChangeCapitalUnhappinessMod(pPolicy->GetCapitalUnhappinessMod() * iChange);
	ChangeWoundedUnitDamageMod(pPolicy->GetWoundedUnitDamageMod() * iChange);
	ChangeUnitUpgradeCostMod(pPolicy->GetUnitUpgradeCostMod() * iChange);
	ChangeBarbarianCombatBonus(pPolicy->GetBarbarianCombatBonus() * iChange);
	ChangeAlwaysSeeBarbCampsCount(pPolicy->IsAlwaysSeeBarbCamps() * iChange);
	ChangeMaxNumBuilders(pPolicy->GetNumExtraBuilders() * iChange);
	ChangePlotGoldCostMod(pPolicy->GetPlotGoldCostMod() * iChange);
	ChangePlotCultureCostModifier(pPolicy->GetPlotCultureCostModifier() * iChange);
	ChangePlotCultureExponentModifier(pPolicy->GetPlotCultureExponentModifier() * iChange);
	ChangeNumCitiesPolicyCostDiscount(pPolicy->GetNumCitiesPolicyCostDiscount() * iChange);
	ChangeNumCitiesResearchCostDiscount(pPolicy->GetNumCitiesResearchCostDiscount() * iChange);
	ChangeGarrisonFreeMaintenanceCount(pPolicy->IsGarrisonFreeMaintenance() * iChange);
	ChangeGarrisonedCityRangeStrikeModifier(pPolicy->GetGarrisonedCityRangeStrikeModifier() * iChange);
	ChangeUnitPurchaseCostModifier(pPolicy->GetUnitPurchaseCostModifier() * iChange);
	GetTreasury()->ChangeCityConnectionTradeRouteGoldModifier(pPolicy->GetCityConnectionTradeRouteGoldModifier() * iChange);
	changeGoldPerUnit(pPolicy->GetGoldPerUnit() * iChange);
	changeGoldPerMilitaryUnit(pPolicy->GetGoldPerMilitaryUnit() * iChange);
	ChangeCityStrengthMod(pPolicy->GetCityStrengthMod() * iChange);
	ChangeCityGrowthMod(pPolicy->GetCityGrowthMod() * iChange);
	ChangeCapitalGrowthMod(pPolicy->GetCapitalGrowthMod() * iChange);
	changeSettlerProductionModifier(pPolicy->GetSettlerProductionModifier() * iChange);
	changeCapitalSettlerProductionModifier(pPolicy->GetCapitalSettlerProductionModifier() * iChange);
	ChangeRouteGoldMaintenanceMod(pPolicy->GetRouteGoldMaintenanceMod() * iChange);
	ChangeBuildingGoldMaintenanceMod(pPolicy->GetBuildingGoldMaintenanceMod() * iChange);
	ChangeUnitGoldMaintenanceMod(pPolicy->GetUnitGoldMaintenanceMod() * iChange);
	ChangeUnitSupplyMod(pPolicy->GetUnitSupplyMod() * iChange);
	changeHappyPerMilitaryUnit(pPolicy->GetHappyPerMilitaryUnit() * iChange);
	changeHappinessToCulture(pPolicy->GetHappinessToCulture() * iChange);
	changeHappinessToScience(pPolicy->GetHappinessToScience() * iChange);
#ifdef NQ_GOLD_TO_SCIENCE_FROM_POLICIES
	changeGoldToScience(pPolicy->GetGoldToScience() * iChange);
#endif
	changeHalfSpecialistUnhappinessCount((pPolicy->IsHalfSpecialistUnhappiness()) ? iChange : 0);
	changeHalfMoreSpecialistUnhappinessCount((pPolicy->IsHalfMoreSpecialistUnhappiness()) ? iChange : 0);
	changeHalfSpecialistFoodCount((pPolicy->IsHalfSpecialistFood()) ? iChange : 0);
	changeMilitaryFoodProductionCount((pPolicy->IsMilitaryFoodProduction()) ? iChange : 0);
	ChangeGoldenAgeCultureBonusDisabledCount((pPolicy->IsGoldenAgeCultureBonusDisabled()) ? iChange : 0);
	ChangeSecondReligionPantheonCount((pPolicy->IsSecondReligionPantheon()) ? iChange : 0);
	ChangeEnablesSSPartHurryCount((pPolicy->IsEnablesSSPartHurry()) ? iChange : 0);
#ifdef NQ_DIABLE_RESISTANCE_TIME_VIA_POLICIES
	ChangeDisablesResistanceTimeCount((pPolicy->IsDisablesResistanceTime()) ? iChange : 0);
#endif
#ifdef NQ_PATRIOTIC_WAR
	ChangeDoubleTrainedMilitaryLandUnitCount((pPolicy->IsDoubleTrainedMilitaryLandUnit()) ? iChange : 0);
#endif
#ifdef NQ_WAR_HERO
	ChangeWarHeroCount((pPolicy->IsWarHero()) ? iChange : 0);
#endif
#ifdef NQ_IGNORE_PUPPETS_FOR_RESEARCH_COSTS_FROM_POLICIES
	ChangeIgnorePuppetsForResearchCostsCount ((pPolicy->IsIgnorePuppetsForResearchCosts()) ? iChange : 0);
#endif
#ifdef NQ_POLICY_TOGGLE_NO_MINOR_DOW_IF_FRIENDS
	ChangeNoMinorDOWIfFriendsCount((pPolicy->IsNoMinorDOWIfFriends()) ? iChange : 0);
#endif
#ifdef NQ_ALLOW_PUPPET_PURCHASING_FROM_POLICIES
	ChangeAllowPuppetPurchasingCount((pPolicy->IsAllowPuppetPurchasing()) ? iChange : 0);
#endif
	ChangeEnablesSSPartPurchaseCount((pPolicy->IsEnablesSSPartPurchase()) ? iChange : 0);
	changeMaxConscript(getWorldSizeMaxConscript(kPolicy) * iChange);
	changeExpModifier(pPolicy->GetExpModifier() * iChange);
	changeExpInBorderModifier(pPolicy->GetExpInBorderModifier() * iChange);
	changeMinorQuestFriendshipMod(pPolicy->GetMinorQuestFriendshipMod() * iChange);
	changeMinorGoldFriendshipMod(pPolicy->GetMinorGoldFriendshipMod() * iChange);
	ChangeMinorFriendshipAnchorMod(pPolicy->GetMinorFriendshipMinimum() * iChange);
	changeGetMinorFriendshipDecayMod(pPolicy->GetMinorFriendshipDecayMod() * iChange);
	ChangeMinorScienceAlliesCount(pPolicy->IsMinorScienceAllies() * iChange);
	ChangeMinorResourceBonusCount(pPolicy->IsMinorResourceBonus() * iChange);
	ChangeNewCityExtraPopulation(pPolicy->GetNewCityExtraPopulation() * iChange);
	ChangeFreeFoodBox(pPolicy->GetFreeFoodBox() * iChange);
	ChangeStrategicResourceMod(pPolicy->GetStrategicResourceMod() * iChange);
	ChangeAbleToAnnexCityStatesCount((pPolicy->IsAbleToAnnexCityStates()) ? iChange : 0);

	if(pPolicy->IsOneShot())
	{
		if(m_pPlayerPolicies->HasOneShotPolicyFired(ePolicy))
		{
			return;
		}
		else
		{
			m_pPlayerPolicies->SetOneShotPolicyFired(ePolicy,true);
		}
	}

#ifdef NQ_EXTRA_SPIES_FROM_POLICIES
	int iNumExtraSpies = pPolicy->GetNumExtraSpies() * iChange;
	if (iNumExtraSpies > 0)
	{
		CvPlayerEspionage* pEspionage = GetEspionage();
		if (pEspionage)
		{
			for (int i = 0; i < iNumExtraSpies; i++)
			{
				pEspionage->CreateSpy();
			}
		}
	}
#endif

	GetPlayerPolicies()->ChangeNumExtraBranches(pPolicy->GetNumExtraBranches() * iChange);

	ChangeAllFeatureProduction(pPolicy->GetAllFeatureProduction());

	int iMod;
	YieldTypes eYield;

	for(iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		eYield = (YieldTypes) iI;

		iMod = pPolicy->GetYieldModifier(iI) * iChange;
		if(iMod != 0)
			changeYieldRateModifier(eYield, iMod);

		iMod = pPolicy->GetCityYieldChange(iI) * iChange;
		if(iMod != 0)
			ChangeCityYieldChange(eYield, iMod * 100);

		iMod = pPolicy->GetCoastalCityYieldChange(iI) * iChange;
		if(iMod != 0)
			ChangeCoastalCityYieldChange(eYield, iMod);

		iMod = pPolicy->GetCapitalYieldChange(iI) * iChange;
		if(iMod != 0)
			ChangeCapitalYieldChange(eYield, iMod * 100);

		iMod = pPolicy->GetCapitalYieldPerPopChange(iI) * iChange;
		if(iMod != 0)
			ChangeCapitalYieldPerPopChange(eYield, iMod);

		iMod = pPolicy->GetCapitalYieldModifier(iI) * iChange;
		if(iMod != 0)
			changeCapitalYieldRateModifier(eYield, iMod);

		iMod = pPolicy->GetGreatWorkYieldChange(iI) * iChange;
		if(iMod != 0)
			ChangeGreatWorkYieldChange(eYield, iMod);

		iMod = pPolicy->GetSpecialistExtraYield(iI) * iChange;
		if(iMod != 0)
			changeSpecialistExtraYield(eYield, iMod);
#if defined(LEKMOD_EXPERIMENTAL_CHANGES)
		iMod = pPolicy->GetWorldWonderYieldChange(iI) * iChange;
		if (iMod != 0)
			ChangeWorldWonderYieldChanges(eYield, iMod);
#endif
	}
#if defined(LEKMOD_v34)
	for (int i = 0; i < GC.getNumResourceInfos(); ++i)
	{
		int iAmount = pPolicy->GetPolicyResourceQuantity(i);
		if (iAmount != 0)
		{
			changeNumResourceTotal((ResourceTypes)i, iAmount * iChange);
		}
	}
#endif
	for(iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
	{
		changeUnitCombatProductionModifiers((UnitCombatTypes)iI, (pPolicy->GetUnitCombatProductionModifiers(iI) * iChange));
		changeUnitCombatFreeExperiences((UnitCombatTypes)iI, (pPolicy->GetUnitCombatFreeExperiences(iI) * iChange));
	}

	for(iI = 0; iI < GC.getNumHurryInfos(); iI++)
	{
		if(GC.getHurryInfo((HurryTypes) iI)->getPolicyPrereq() == ePolicy)
		{
			changeHurryCount(((HurryTypes)iI), iChange);
		}
		{
			changeHurryModifier((HurryTypes) iI, (pPolicy->GetHurryModifier(iI) * iChange));
		}
	}

	for(iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		for(iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			changeImprovementYieldChange(((ImprovementTypes)iI), ((YieldTypes)iJ), (pPolicy->GetImprovementYieldChanges(iI, iJ) * iChange));
		}
	}


	PromotionTypes ePromotion;
	for(iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		ePromotion = (PromotionTypes) iI;

		if (pPolicy->IsFreePromotion(ePromotion))
		{
			ChangeFreePromotionCount(ePromotion, iChange);
		}

#ifdef LEKMOD_UNITCOMBAT_FREE_PROMOTION

		for (iJ = 0; iJ < GC.getNumUnitCombatClassInfos(); iJ++)
		{
			UnitCombatTypes eUnitCombat = (UnitCombatTypes)iJ;
			
			if (pPolicy->IsFreePromotionUnitCombat(ePromotion, eUnitCombat))
			{
				OutputDebugStringA(CvString::format("Free promotion for %s in unit combat %s\n", GC.getPromotionInfo(ePromotion)->GetDescription(), GC.getUnitCombatClassInfo(eUnitCombat)->GetType()).c_str());
				int unitCombatIndex = GC.getUnitCombatClassInfo(eUnitCombat)->GetID();
				ChangeFreePromotionUnitCombatCount(ePromotion, iChange, unitCombatIndex);
			}
		}
#endif

	}

	CvCity* pLoopCity;
	PlayerTypes ePlayer;


	if(pPolicy->IsRevealAllCapitals())
	{
		for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
		{
			ePlayer = (PlayerTypes) iI;

			if(GET_PLAYER(ePlayer).isAlive())
			{
				pLoopCity = GET_PLAYER(ePlayer).getCapitalCity();

				if(pLoopCity != NULL)
				{
					pLoopCity->plot()->setRevealed(getTeam(), true);
				}
			}
		}
	}


	CvNotifications* pNotifications;
	Localization::String locString;
	Localization::String locSummary;

	int iOtherPlayersDecay = pPolicy->GetOtherPlayersMinorFriendshipDecayMod();
	if(iOtherPlayersDecay != 0)
	{
		for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			ePlayer = (PlayerTypes) iI;

			if(GET_PLAYER(ePlayer).isEverAlive())
			{

				if(GET_PLAYER(ePlayer).getTeam() != getTeam())
				{
					GET_PLAYER(ePlayer).changeGetMinorFriendshipDecayMod(iOtherPlayersDecay * iChange);


					locString = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_FRIENDSHIP_DECAY");
					locString << getNameKey();
					locSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_FRIENDSHIP_DECAY");

					pNotifications = GET_PLAYER(ePlayer).GetNotifications();
					if(pNotifications)
					{
						pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, locString.toUTF8(), locSummary.toUTF8(), -1, -1, -1);
					}
				}
			}
		}
	}

	BuildingClassTypes eBuildingClass;
	BuildingTypes eBuilding;
	int iBuildingCount;
	int iYieldMod;
	int iYieldChange;


	int iNumCitiesFreeAestheticsSchools = pPolicy->GetNumCitiesFreeAestheticsSchools();
	int iNumCitiesFreePietyGardens = pPolicy->GetNumCitiesFreePietyGardens();
	int iNumCitiesFreeWalls = pPolicy->GetNumCitiesFreeWalls();
	int iNumCitiesFreeCultureBuilding = pPolicy->GetNumCitiesFreeCultureBuilding();
	int iNumCitiesFreeFoodBuilding = pPolicy->GetNumCitiesFreeFoodBuilding();


	int iLoop;
	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{


		if (iNumCitiesFreePietyGardens > 0)
		{
			BuildingTypes ePietyGarden = pLoopCity->ChooseFreeGardenBuilding();
			if (ePietyGarden != NO_BUILDING)
			{
				pLoopCity->GetCityBuildings()->SetNumRealBuilding(ePietyGarden, 0);
				pLoopCity->GetCityBuildings()->SetNumFreeBuilding(ePietyGarden, 1);
				if (pLoopCity->getFirstBuildingOrder(ePietyGarden) == 0)
				{
					pLoopCity->clearOrderQueue();
					pLoopCity->chooseProduction();
				}
			}
			iNumCitiesFreePietyGardens--;
		}

		if(iNumCitiesFreeAestheticsSchools > 0)
		{
			BuildingTypes eAestheticsSchool = (BuildingTypes)GC.getInfoTypeForString("BUILDING_SCRIPTORIUM", true);
			if (eAestheticsSchool != NO_BUILDING)
			{
				pLoopCity->GetCityBuildings()->SetNumFreeBuilding(eAestheticsSchool, 1);
				if (pLoopCity->getFirstBuildingOrder(eAestheticsSchool) == 0)
				{
					pLoopCity->clearOrderQueue();
					pLoopCity->chooseProduction();
				}
			}
			eAestheticsSchool = (BuildingTypes)GC.getInfoTypeForString("BUILDING_GALLERY", true);
			if (eAestheticsSchool != NO_BUILDING)
			{
				pLoopCity->GetCityBuildings()->SetNumFreeBuilding(eAestheticsSchool, 1);
				if (pLoopCity->getFirstBuildingOrder(eAestheticsSchool) == 0)
				{
					pLoopCity->clearOrderQueue();
					pLoopCity->chooseProduction();
				}
			}
			eAestheticsSchool = (BuildingTypes)GC.getInfoTypeForString("BUILDING_CONSERVATORY", true);
			if (eAestheticsSchool != NO_BUILDING)
			{
				pLoopCity->GetCityBuildings()->SetNumFreeBuilding(eAestheticsSchool, 1);
				if (pLoopCity->getFirstBuildingOrder(eAestheticsSchool) == 0)
				{
					pLoopCity->clearOrderQueue();
					pLoopCity->chooseProduction();
				}
			}
			iNumCitiesFreeAestheticsSchools--;
		}
		

		if (iNumCitiesFreeWalls > 0)
		{
			BuildingTypes eWalls = pLoopCity->ChooseFreeWallsBuilding();
			if (eWalls != NO_BUILDING)
			{
				pLoopCity->GetCityBuildings()->SetNumRealBuilding(eWalls, 1);
				if (pLoopCity->getFirstBuildingOrder(eWalls) == 0)
				{
					pLoopCity->clearOrderQueue();
					pLoopCity->chooseProduction();
				}
			}
			iNumCitiesFreeWalls--;
		}

		if(iNumCitiesFreeCultureBuilding > 0)
		{
			BuildingTypes eCultureBuilding = pLoopCity->ChooseFreeCultureBuilding();
			if(eCultureBuilding != NO_BUILDING)
			{
				pLoopCity->GetCityBuildings()->SetNumFreeBuilding(eCultureBuilding, 1);

				if(pLoopCity->getFirstBuildingOrder(eCultureBuilding) == 0)
				{
					pLoopCity->clearOrderQueue();
					pLoopCity->chooseProduction();
				}
			}
			else
			{
				pLoopCity->SetOwedCultureBuilding(true);
			}


			iNumCitiesFreeCultureBuilding--;
		}

		if(iNumCitiesFreeFoodBuilding > 0)
		{
#ifndef AQUEDUCT_FIX
			BuildingTypes eFoodBuilding = pLoopCity->ChooseFreeFoodBuilding();
#else
			BuildingTypes eFoodBuilding = (BuildingTypes)GC.getInfoTypeForString("BUILDING_AQUEDUCT", true);
#endif
			if(eFoodBuilding != NO_BUILDING)
			{
#ifdef AQUEDUCT_FIX
				pLoopCity->GetCityBuildings()->SetNumFreeBuilding(eFoodBuilding, 0);
#endif
				pLoopCity->GetCityBuildings()->SetNumFreeBuilding(eFoodBuilding, 1);

				if(pLoopCity->getFirstBuildingOrder(eFoodBuilding) == 0)
				{
					pLoopCity->clearOrderQueue();
					pLoopCity->chooseProduction();
				}
			}


			iNumCitiesFreeFoodBuilding--;
		}


		int iCityCultureChange = pPolicy->GetCulturePerCity() * iChange;
#ifdef FRUITY_TRADITION_ARISTOCRACY
		if (pPolicy->GetCapitalCulturePerUniqueLuxury() != 0 && pLoopCity->isCapital())
		{
			int iNumLuxuries = 0;
			ResourceTypes eResource = NO_RESOURCE;
			const CvResourceInfo* pkResourceInfo = NULL;
#ifdef AUI_WARNING_FIXES
			for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
			for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
			{
				eResource = static_cast<ResourceTypes>(iResourceLoop);
				pkResourceInfo = GC.getResourceInfo(eResource);
				if (pkResourceInfo && pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY && getNumResourceAvailable(eResource) > 0)
				{
					iNumLuxuries++;
				}
			}
			iCityCultureChange += pPolicy->GetCapitalCulturePerUniqueLuxury() * iNumLuxuries * iChange;
		}
#endif
		if(pLoopCity->GetGarrisonedUnit() != NULL)
		{
#ifndef FIX_POLICY_CULTURE_PER_GARRISONED_UNIT
			iCityCultureChange += (pPolicy->GetCulturePerGarrisonedUnit() * iChange);
#endif
		}
#if !defined(STANDARDIZE_YIELDS)
		pLoopCity->ChangeJONSCulturePerTurnFromPolicies(iCityCultureChange);
#else
		pLoopCity->ChangeBaseYieldRateFromPolicies(YIELD_CULTURE, iCityCultureChange);
#if defined(LEKMOD_GARRISON_YIELD_EFFECTS)
		pLoopCity->ChangeGarrisonYieldBonus(YIELD_CULTURE, pPolicy->GetCulturePerGarrisonedUnit() * iChange);
		pLoopCity->ChangeGarrisonYieldBonus(YIELD_PRODUCTION, pPolicy->GetProductionFromGarrison() * iChange);
#endif
#endif
#if !defined(LEKMOD_NONCIV_BUILDINGCLASS_YIELD_CHANGE)

		for(iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			eBuildingClass = (BuildingClassTypes) iI;

			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
			if(!pkBuildingClassInfo)
			{
				continue;
			}

			eBuilding = (BuildingTypes) getCivilizationInfo().getCivilizationBuildings(eBuildingClass);

			if(eBuilding != NO_BUILDING)
			{

				CvBuildingEntry* pkBuilding = GC.getBuildingInfo(eBuilding);
				if(pkBuilding)
				{
					iBuildingCount = pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding);
#if !defined(STANDARDIZE_YIELDS)

					pLoopCity->ChangeJONSCulturePerTurnFromPolicies(pPolicy->GetBuildingClassCultureChange(eBuildingClass) * iBuildingCount * iChange);


						for(iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
						{
							switch(iJ)
							{
							case YIELD_CULTURE:

								break;
							case YIELD_FAITH:
								pLoopCity->ChangeFaithPerTurnFromPolicies(pPolicy->GetBuildingClassYieldChanges(eBuildingClass, iJ) * iBuildingCount * iChange);
								break;
							default:
								{
									eYield = (YieldTypes) iJ;
									iYieldMod = pPolicy->GetBuildingClassYieldModifiers(eBuildingClass, eYield);
									if (iYieldMod > 0)
									{
										pLoopCity->changeYieldRateModifier(eYield, iYieldMod * iBuildingCount * iChange);
									}
									iYieldChange = pPolicy->GetBuildingClassYieldChanges(eBuildingClass, eYield);
									if (iYieldChange != 0)
									{
										pLoopCity->ChangeBaseYieldRateFromBuildings(eYield, iYieldChange * iBuildingCount * iChange);
									}
								}
							}
						}
#else

					for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
					{
						eYield = (YieldTypes)iJ;
						iYieldMod = pPolicy->GetBuildingClassYieldModifiers(eBuildingClass, eYield);
						if (iYieldMod > 0)
						{
							pLoopCity->changeYieldRateModifier(eYield, iYieldMod * iBuildingCount * iChange);
						}
						iYieldChange = pPolicy->GetBuildingClassYieldChanges(eBuildingClass, eYield);
						if (iYieldChange != 0)
						{
							pLoopCity->ChangeBaseYieldRateFromBuildings(eYield, iYieldChange * iBuildingCount * iChange);
						}
					}
#endif
					}
				}
			}
		}
#else
		for (int jJ = 0; jJ < GC.getNumBuildingInfos(); jJ++)
		{
			eBuilding = (BuildingTypes)jJ;
			if (eBuilding == NO_BUILDING)
				continue;
			CvBuildingEntry* pkBuilding = GC.getBuildingInfo(eBuilding);
			if(!pkBuilding)
				continue;
			eBuildingClass = (BuildingClassTypes)pkBuilding->GetBuildingClassType();
			if (eBuildingClass == NO_BUILDINGCLASS)
				continue;
			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
			if(!pkBuildingClassInfo)
				continue;
			iBuildingCount = pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding);
			if (iBuildingCount > 0)
			{
#if !defined(STANDARDIZE_YIELDS)

				pLoopCity->ChangeJONSCulturePerTurnFromPolicies(pPolicy->GetBuildingClassCultureChange(eBuildingClass)* iBuildingCount* iChange);


				for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					switch (iJ)
					{
					case YIELD_CULTURE:

						break;
					case YIELD_FAITH:
						pLoopCity->ChangeFaithPerTurnFromPolicies(pPolicy->GetBuildingClassYieldChanges(eBuildingClass, iJ) * iBuildingCount * iChange);
						break;
					default:
					{
						eYield = (YieldTypes)iJ;
						iYieldMod = pPolicy->GetBuildingClassYieldModifiers(eBuildingClass, eYield);
						if (iYieldMod > 0)
						{
							pLoopCity->changeYieldRateModifier(eYield, iYieldMod * iBuildingCount * iChange);
						}
						iYieldChange = pPolicy->GetBuildingClassYieldChanges(eBuildingClass, eYield);
						if (iYieldChange != 0)
						{
							pLoopCity->ChangeBaseYieldRateFromBuildings(eYield, iYieldChange * iBuildingCount * iChange);
						}
					}
					}
				}
#else

				for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					eYield = (YieldTypes)iJ;
					iYieldMod = pPolicy->GetBuildingClassYieldModifiers(eBuildingClass, eYield);
					if (iYieldMod != 0)
					{
						pLoopCity->changeYieldRateModifier(eYield, iYieldMod * iBuildingCount * iChange);
					}
					iYieldChange = pPolicy->GetBuildingClassYieldChanges(eBuildingClass, eYield);
					if (iYieldChange != 0)
					{
						pLoopCity->ChangeBaseYieldRateFromBuildings(eYield, iYieldChange * iBuildingCount * iChange);
					}
				}
#endif
			}
		}
#endif
	}
					

	ChangeNumCitiesFreeAestheticsSchools(iNumCitiesFreeAestheticsSchools);
	ChangeNumCitiesFreePietyGardens(iNumCitiesFreePietyGardens);
	ChangeNumCitiesFreeWalls(iNumCitiesFreeWalls);
	ChangeNumCitiesFreeCultureBuilding(iNumCitiesFreeCultureBuilding);
	ChangeNumCitiesFreeFoodBuilding(iNumCitiesFreeFoodBuilding);


#ifdef AUI_WARNING_FIXES
	for (iJ = 0; iJ < GC.getNUM_AND_TECH_PREREQS(); iJ++)
	{
		if (pPolicy->GetPolicyDisables(iJ) != NO_POLICY)
		{
			if (m_pPlayerPolicies->HasPolicy((PolicyTypes)pPolicy->GetPolicyDisables(iJ)))
			{
				setHasPolicy((PolicyTypes)pPolicy->GetPolicyDisables(iJ), false);
#else
	for(iI = 0; iI < GC.getNUM_AND_TECH_PREREQS(); iI++)
	{
		if(pPolicy->GetPolicyDisables(iI) != NO_POLICY)
		{
			if(m_pPlayerPolicies->HasPolicy((PolicyTypes) pPolicy->GetPolicyDisables(iI)))
			{
				setHasPolicy((PolicyTypes) pPolicy->GetPolicyDisables(iI), false);
#endif
			}
		}
	}


	int iTurns = pPolicy->GetAttackBonusTurns() * iChange;
	if(iTurns > 0)
	{
		ChangeAttackBonusTurns(iTurns);
	}


	int iGoldenAgeTurns = pPolicy->GetGoldenAgeTurns() * iChange;
	if(iGoldenAgeTurns > 0)
	{

		int iLengthModifier = getGoldenAgeModifier();


		iLengthModifier += GetPlayerTraits()->GetGoldenAgeDurationModifier();

		if(iLengthModifier > 0)
		{
			iGoldenAgeTurns = iGoldenAgeTurns * (100 + iLengthModifier) / 100;
		}


		iGoldenAgeTurns *= GC.getGame().getGameSpeedInfo().getGoldenAgePercent();
		iGoldenAgeTurns /= 100;

		changeGoldenAgeTurns(iGoldenAgeTurns);
	}


	int iNumFreeTechs = pPolicy->GetNumFreeTechs() * iChange;
	if(iNumFreeTechs > 0)
	{
		if(!isHuman())
		{
#ifdef AUI_WARNING_FIXES
			for (iJ = 0; iJ < iNumFreeTechs; iJ++)
#else
			for(iI = 0; iI < iNumFreeTechs; iI++)
#endif
			{
				AI_chooseFreeTech();
			}
		}
		else
		{
			CvString strBuffer = GetLocalizedText("TXT_KEY_MISC_COMPLETED_WONDER_CHOOSE_TECH", pPolicy->GetTextKey());
			chooseTech(iNumFreeTechs, strBuffer.GetCString());
		}
	}

	ChangeMedianTechPercentage(pPolicy->GetMedianTechPercentChange());


	int iNumFreePolicies = pPolicy->GetNumFreePolicies() * iChange;
	if(iNumFreePolicies > 0)
	{
		ChangeNumFreePolicies(iNumFreePolicies);
	}

	if(pPolicy->IncludesOneShotFreeUnits())
	{
		if(!m_pPlayerPolicies->HaveOneShotFreeUnitsFired(ePolicy))
		{
			m_pPlayerPolicies->SetOneShotFreeUnitsFired(ePolicy,true);

			int iNumFreeGreatPeople = pPolicy->GetNumFreeGreatPeople() * iChange;
			if(iNumFreeGreatPeople > 0)
			{
				ChangeNumFreeGreatPeople(iNumFreeGreatPeople);
			}

			if(getCapitalCity() != NULL)
			{
				int iX = getCapitalCity()->getX();
				int iY = getCapitalCity()->getY();

				for(iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
				{
					const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(iI);
					CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
					if(pkUnitClassInfo)
					{
						int iNumFreeUnits = pPolicy->GetNumFreeUnitsByClass(eUnitClass);
						if(iNumFreeUnits > 0)
						{
							const UnitTypes eUnit = (UnitTypes) getCivilizationInfo().getCivilizationUnits(eUnitClass);
							CvUnitEntry* pUnitEntry = GC.getUnitInfo(eUnit);
							if(GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman() && pUnitEntry != NULL && pUnitEntry->IsFound())
							{
								continue;
							}

							for(int iUnitLoop = 0; iUnitLoop < iNumFreeUnits; iUnitLoop++)
							{
								CvUnit* pNewUnit = NULL;



#ifdef AUI_PLAYER_FIX_VENICE_ONLY_BANS_SETTLERS_NOT_SETTLING
								if (GetPlayerTraits()->IsNoAnnexing() && pUnitEntry->GetDefaultUnitAIType() == UNITAI_SETTLE)
#else
								if (pUnitEntry->IsFound() && GetPlayerTraits()->IsNoAnnexing())
#endif
								{


#ifdef AUI_WARNING_FIXES
									for (uint iVeniceSearch = 0; iVeniceSearch < GC.getNumUnitClassInfos(); iVeniceSearch++)
#else
									for(int iVeniceSearch = 0; iVeniceSearch < GC.getNumUnitClassInfos(); iVeniceSearch++)
#endif
									{
										const UnitClassTypes eVeniceUnitClass = static_cast<UnitClassTypes>(iVeniceSearch);
										CvUnitClassInfo* pkVeniceUnitClassInfo = GC.getUnitClassInfo(eVeniceUnitClass);
										if(pkVeniceUnitClassInfo)
										{
											const UnitTypes eMerchantOfVeniceUnit = (UnitTypes) getCivilizationInfo().getCivilizationUnits(eVeniceUnitClass);
											if (eMerchantOfVeniceUnit != NO_UNIT)
											{
												CvUnitEntry* pVeniceUnitEntry = GC.getUnitInfo(eMerchantOfVeniceUnit);
												if (pVeniceUnitEntry->IsCanBuyCityState())
												{
													pNewUnit = initUnit(eMerchantOfVeniceUnit, iX, iY);
#ifdef ENHANCED_GRAPHS
													ChangeNumMerchantsTotal(1);
#endif
													break;
												}
											}
										}
									}
								}
								else
								{
									pNewUnit = initUnit(eUnit, iX, iY);
								}

								CvAssert(pNewUnit);

								if (pNewUnit)
								{
									if(pNewUnit->IsGreatGeneral())
									{
#ifdef ENHANCED_GRAPHS
										ChangeNumGeneralsTotal(1);
#else
										incrementGreatGeneralsCreated();
#endif
										pNewUnit->jumpToNearestValidPlot();
#ifdef NQ_WAR_HERO
										if (IsWarHero())
										{
											addFreeUnit((UnitTypes)GC.getInfoTypeForString("UNIT_ARTIST"));
										}
#endif
									}
									else if(pNewUnit->IsGreatAdmiral())
									{
#ifdef ENHANCED_GRAPHS
										ChangeNumAdmiralsTotal(1);
#else
										incrementGreatAdmiralsCreated();
#endif
										CvPlot *pSpawnPlot = GetGreatAdmiralSpawnPlot(pNewUnit);
										if (pNewUnit->plot() != pSpawnPlot)
										{
											pNewUnit->setXY(pSpawnPlot->getX(), pSpawnPlot->getY());
										}
									}
									else if(pNewUnit->getUnitInfo().IsFoundReligion())
									{
										ReligionTypes eReligion = GetReligions()->GetReligionCreatedByPlayer();
										int iReligionSpreads = pNewUnit->getUnitInfo().GetReligionSpreads();
										int iReligiousStrength = pNewUnit->getUnitInfo().GetReligiousStrength();
										if(iReligionSpreads > 0 && eReligion > RELIGION_PANTHEON)
										{
											pNewUnit->GetReligionData()->SetSpreadsLeft(iReligionSpreads);
											pNewUnit->GetReligionData()->SetReligiousStrength(iReligiousStrength);
											pNewUnit->GetReligionData()->SetReligion(eReligion);
										}
#ifdef ENHANCED_GRAPHS
										ChangeNumProphetsTotal(1);
#endif
									}
									else if (pNewUnit->getUnitInfo().GetUnitClassType() == GC.getInfoTypeForString("UNITCLASS_WRITER"))
									{


#ifdef ENHANCED_GRAPHS
										ChangeNumWritersTotal(1);
#endif
										if (pNewUnit->getUnitInfo().GetOneShotTourism() > 0)
										{
											pNewUnit->SetTourismBlastStrength(GetCulture()->GetTourismBlastStrength(pNewUnit->getUnitInfo().GetOneShotTourism()));
										}

										pNewUnit->jumpToNearestValidPlot();
									}							
									else if (pNewUnit->getUnitInfo().GetUnitClassType() == GC.getInfoTypeForString("UNITCLASS_ARTIST"))
									{


#ifdef ENHANCED_GRAPHS
										ChangeNumArtistsTotal(1);
#endif
										pNewUnit->jumpToNearestValidPlot();
									}							
									else if (pNewUnit->getUnitInfo().GetUnitClassType() == GC.getInfoTypeForString("UNITCLASS_MUSICIAN"))
									{


#ifdef ENHANCED_GRAPHS
										ChangeNumMusiciansTotal(1);
#endif
										pNewUnit->jumpToNearestValidPlot();
									}

									else if (pNewUnit->getUnitInfo().GetUnitClassType() == GC.getInfoTypeForString("UNITCLASS_SCIENTIST"))
									{

#ifdef DECREASE_BULB_AMOUNT_OVER_TIME
										if (GC.getGame().isOption("GAMEOPTION_NO_SCIENTIST_SAVING"))
										{
											pNewUnit->SetScientistBirthTurn(GC.getGame().getGameTurn());
										}
										else
										{
											pNewUnit->SetResearchBulbAmount(GetScienceYieldFromPreviousTurns(GC.getGame().getGameTurn(), pNewUnit->getUnitInfo().GetBaseBeakersTurnsToCount()));
										}
#else
										pNewUnit->SetResearchBulbAmount(GetScienceYieldFromPreviousTurns(GC.getGame().getGameTurn(), pNewUnit->getUnitInfo().GetBaseBeakersTurnsToCount()));
#endif


#ifdef ENHANCED_GRAPHS
										ChangeNumScientistsTotal(1);
#endif
										pNewUnit->jumpToNearestValidPlot();
									}
									else if (pNewUnit->getUnitInfo().GetUnitClassType() == GC.getInfoTypeForString("UNITCLASS_ENGINEER"))
									{


#ifdef ENHANCED_GRAPHS
										ChangeNumEngineersTotal(1);
#endif
										pNewUnit->jumpToNearestValidPlot();
									}
									else if (pNewUnit->getUnitInfo().GetUnitClassType() == GC.getInfoTypeForString("UNITCLASS_MERCHANT"))
									{


#ifdef ENHANCED_GRAPHS
										ChangeNumMerchantsTotal(1);
#endif
										pNewUnit->jumpToNearestValidPlot();
									}
									else if (pNewUnit->getUnitInfo().GetUnitClassType() == GC.getInfoTypeForString("UNITCLASS_PROPHET"))
									{
										incrementGreatProphetsCreated();
#ifdef ENHANCED_GRAPHS
										ChangeNumProphetsTotal(1);
#endif
										pNewUnit->jumpToNearestValidPlot();
									}

									else if(pNewUnit->IsGreatPerson())
									{
										incrementGreatPeopleCreated();
										pNewUnit->jumpToNearestValidPlot();
									}
									else
									{
										pNewUnit->jumpToNearestValidPlot();
									}
								}
							}
						}
					}
				}
			}
		}
	}


	if(pPolicy->IsMinorGreatPeopleAllies())
	{
		DoAdoptedGreatPersonCityStatePolicy();
	}


#ifdef LEKMOD_REFORMATION_NOTIFICATION_MID_TURN
	if (isHuman() && pPolicy->IsAddReformationBelief() && GetReligions()->HasCreatedReligion() && !GetReligions()->HasAddedReformationBelief())
	{
		DoReformationNotification();
	}
	
#else
	if (isHuman() && pPolicy->IsAddReformationBelief() && GetReligions()->HasCreatedReligion() && !GetReligions()->HasAddedReformationBelief())
	{
		pNotifications = GetNotifications();
		if(pNotifications)
		{
			CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ADD_REFORMATION_BELIEF");
			CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ADD_REFORMATION_BELIEF");
			pNotifications->Add(NOTIFICATION_ADD_REFORMATION_BELIEF, strBuffer, strSummary, -1, -1, -1);
		}
	}
#endif

	if (pPolicy->GetStealTechFasterModifier() != 0)
	{
		GetEspionage()->UpdateSpies();
	}

	CvPlot *pLoopPlot;
	ResourceTypes eResource;
	for(iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
		eResource = pLoopPlot->getResourceType();
		if(eResource != NO_RESOURCE)
		{
			if(GC.getResourceInfo(eResource)->getPolicyReveal() == (int)ePolicy)
			{
				pLoopPlot->updateYield();
				if (pLoopPlot->isRevealed(getTeam()))
				{
					pLoopPlot->setLayoutDirty(true);
				}
			}
		}
	}
#if defined(LEKMOD_FIX_PATRO_FOOD)
	if (pPolicy->GetCityStateBonusModifier() != 0)
	{

		int iMinorLoop;
		for (iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
		{
			CvPlayer& kMinor = GET_PLAYER((PlayerTypes)iMinorLoop);
			if (!kMinor.isAlive())
				continue;
			if (!kMinor.isMinorCiv())
				continue;
			if (kMinor.GetMinorCivAI()->GetTrait() != MINOR_CIV_TRAIT_MARITIME)
				continue;
			bool bFriends = kMinor.GetMinorCivAI()->IsFriends(GetID());
			bool bAllies = kMinor.GetMinorCivAI()->IsAllies(GetID());
			kMinor.GetMinorCivAI()->DoSetBonus(GetID(), false         , bFriends, bAllies, true                           , false                     , NO_PLAYER);
		}
		ChangeCityStateBonusModifier(pPolicy->GetCityStateBonusModifier() * iChange);

		for (iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
		{
			CvPlayer& kMinor = GET_PLAYER((PlayerTypes)iMinorLoop);
			if (!kMinor.isAlive())
				continue;
			if (!kMinor.isMinorCiv())
				continue;
			if (kMinor.GetMinorCivAI()->GetTrait() != MINOR_CIV_TRAIT_MARITIME)
				continue;
			bool bFriends = kMinor.GetMinorCivAI()->IsFriends(GetID());
			bool bAllies = kMinor.GetMinorCivAI()->IsAllies(GetID());
			kMinor.GetMinorCivAI()->DoSetBonus(GetID(), true         , bFriends, bAllies, true                           , false                     , NO_PLAYER);
		}
	}
#endif
	DoUpdateHappiness();
	GetTrade()->UpdateTradeConnectionValues();
	recomputeGreatPeopleModifiers();
	recomputePolicyCostModifier();
	recomputeFreeExperience();

	doUpdateBarbarianCampVisibility();
#ifdef AUI_CITIZENS_MID_TURN_ASSIGN_RUNS_SELF_CONSISTENCY
	doSelfConsistencyCheckAllCities();
#endif

	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}

#ifdef LEKMOD_REFORMATION_NOTIFICATION_MID_TURN


void CvPlayer::DoReformationNotification()
{	
	CvNotifications* pNotifications;
	pNotifications = GetNotifications();
	if (pNotifications)
	{
		CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ADD_REFORMATION_BELIEF");
		CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ADD_REFORMATION_BELIEF");
		pNotifications->Add(NOTIFICATION_ADD_REFORMATION_BELIEF, strBuffer, strSummary, -1, -1, -1);
	}
}
#endif




void CvPlayer::doUpdateBarbarianCampVisibility()
{
	if(IsAlwaysSeeBarbCamps())
	{
		CvPlot* pPlot;

		ImprovementTypes eImprovement;

#ifdef AUI_WARNING_FIXES
		for (uint iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); ++iPlotLoop)
#else
		for(int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); ++iPlotLoop)
#endif
		{
			pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

			if(pPlot->isRevealed(getTeam()))
			{
				eImprovement = pPlot->getImprovementType();


				if(eImprovement == GC.getBARBARIAN_CAMP_IMPROVEMENT())
				{

					if(pPlot->getRevealedImprovementType(getTeam()) != eImprovement)
					{
						pPlot->setRevealedImprovementType(getTeam(), eImprovement);
					}
				}
			}
		}
	}
}


bool CvPlayer::isPbemNewTurn() const
{
	return m_bPbemNewTurn;
}


void CvPlayer::setPbemNewTurn(bool bNew)
{
	m_bPbemNewTurn = bNew;
}


CvEconomicAI* CvPlayer::GetEconomicAI() const
{
	return m_pEconomicAI;
}


CvMilitaryAI* CvPlayer::GetMilitaryAI() const
{
	return m_pMilitaryAI;
}


CvCitySpecializationAI* CvPlayer::GetCitySpecializationAI() const
{
	return m_pCitySpecializationAI;
}


CvWonderProductionAI* CvPlayer::GetWonderProductionAI() const
{
	return m_pWonderProductionAI;
}


CvGrandStrategyAI* CvPlayer::GetGrandStrategyAI() const
{
	return m_pGrandStrategyAI;
}


CvDiplomacyAI* CvPlayer::GetDiplomacyAI() const
{
	return m_pDiplomacyAI;
}


CvPlayerReligions* CvPlayer::GetReligions() const
{
	return m_pReligions;
}


CvReligionAI* CvPlayer::GetReligionAI() const
{
	return m_pReligionAI;
}


CvMinorCivAI* CvPlayer::GetMinorCivAI() const
{
	return m_pMinorCivAI;
}


CvDealAI* CvPlayer::GetDealAI() const
{
	return m_pDealAI;
}



CvBuilderTaskingAI* CvPlayer::GetBuilderTaskingAI() const
{
	return m_pBuilderTaskingAI;
}



CvCityConnections* CvPlayer::GetCityConnections() const
{
	return m_pCityConnections;
}



CvPlayerEspionage* CvPlayer::GetEspionage() const
{
	return m_pEspionage;
}



CvEspionageAI* CvPlayer::GetEspionageAI() const
{
	return m_pEspionageAI;
}



CvPlayerTrade* CvPlayer::GetTrade() const
{
	return m_pTrade;
}



CvTradeAI* CvPlayer::GetTradeAI() const
{
	return m_pTradeAI;
}



CvLeagueAI* CvPlayer::GetLeagueAI() const
{
	return m_pLeagueAI;
}


CvPlayerCulture* CvPlayer::GetCulture() const
{
	return m_pCulture;
}


CvNotifications* CvPlayer::GetNotifications() const
{
	return m_pNotifications;
}


CvTreasury* CvPlayer::GetTreasury() const
{
	return m_pTreasury;
}


CvDiplomacyRequests* CvPlayer::GetDiplomacyRequests() const
{
	return m_pDiplomacyRequests;
}


bool CvPlayer::HasActiveDiplomacyRequests() const
{
	PlayerTypes ePlayer = GetID();


	CvDiplomacyRequests* pkDiploRequests = GetDiplomacyRequests();
	if(pkDiploRequests && pkDiploRequests->HasActiveRequest())
		return true;


	for(int i = 0; i < MAX_PLAYERS; ++i)
	{
		const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)i);
		if(kPlayer.isAlive())
		{
			pkDiploRequests = kPlayer.GetDiplomacyRequests();
			if(pkDiploRequests)
			{
				if(pkDiploRequests->HasActiveRequestFrom(ePlayer))
					return true;
			}
		}
	}

	return false;
}






void CvPlayer::Read(FDataStream& kStream)
{

	reset();


	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_iStartingX;
	kStream >> m_iStartingY;
	kStream >> m_iTotalPopulation;
	kStream >> m_iTotalLand;
	kStream >> m_iTotalLandScored;
	kStream >> m_iJONSCulturePerTurnForFree;
	kStream >> m_iJONSCulturePerTurnFromMinorCivs;
	kStream >> m_iJONSCultureCityModifier;
#ifdef AUI_PLAYER_FIX_JONS_CULTURE_IS_T100
	kStream >> m_iJONSCultureT100;
	kStream >> m_iJONSCultureEverGeneratedT100;
#else
	kStream >> m_iJONSCulture;
	kStream >> m_iJONSCultureEverGenerated;
#endif
#if !defined(LEKMOD_EXPERIMENTAL_CHANGES)
	kStream >> m_iCulturePerWonder;
#endif
	kStream >> m_iCultureWonderMultiplier;
	kStream >> m_iCulturePerTechResearched;
	kStream >> m_iFaith;
	kStream >> m_iFaithEverGenerated;
	kStream >> m_iHappiness;
	kStream >> m_iUprisingCounter;
	kStream >> m_iExtraHappinessPerLuxury;
	kStream >> m_iUnhappinessFromUnits;
	kStream >> m_iUnhappinessFromUnitsMod;
	kStream >> m_iUnhappinessMod;
	kStream >> m_iCityCountUnhappinessMod;
	kStream >> m_iOccupiedPopulationUnhappinessMod;
	kStream >> m_iCapitalUnhappinessMod;
	kStream >> m_iCityRevoltCounter;
	kStream >> m_iHappinessPerGarrisonedUnitCount;
	kStream >> m_iHappinessPerTradeRouteCount;
#ifdef NQ_RAIL_CONNECTION_HAPPINESS_FROM_POLICIES
	kStream >> m_iHappinessPerRailConnectionCount;
#endif
	kStream >> m_iHappinessPerXPopulation;
	kStream >> m_iHappinessPerXPolicies;
	if (uiVersion >= 8)
	{
		kStream >> m_iHappinessFromLeagues;
	}
	else
	{
		m_iHappinessFromLeagues = 0;
	}
	kStream >> m_iEspionageModifier;
	kStream >> m_iSpyStartingRank;
#ifdef ENHANCED_GRAPHS
	kStream >> m_iNumStolenScience;
	kStream >> m_iNumTrainedUnits;
	kStream >> m_iNumKilledUnits;
	kStream >> m_iNumLostUnits;
	kStream >> m_iUnitsDamageDealt;
	kStream >> m_iUnitsDamageTaken;
	kStream >> m_iCitiesDamageDealt;
	kStream >> m_iCitiesDamageTaken;
	kStream >> m_iNumScientistsTotal;
	kStream >> m_iNumEngineersTotal;
	kStream >> m_iNumMerchantsTotal;
	kStream >> m_iNumWritersTotal;
	kStream >> m_iNumArtistsTotal;
	kStream >> m_iNumMusiciansTotal;
	kStream >> m_iNumGeneralsTotal;
	kStream >> m_iNumAdmiralsTotal;
	kStream >> m_iNumProphetsTotal;
	kStream >> m_iProductionGoldFromWonders;
	kStream >> m_iNumChops;
	kStream >> m_iNumTimesOpenedDemographics;
	kStream >> m_iUnitsDamageHealed;
	kStream >> m_iTurnsStagnated;
	kStream >> m_iGoldSpentBuys;
	kStream >> m_iGoldSpentUpgrades;
	kStream >> m_bMayaBoostScientist;
	kStream >> m_bMayaBoostEngineers;
	kStream >> m_bMayaBoostMerchants;
	kStream >> m_bMayaBoostWriters;
	kStream >> m_bMayaBoostArtists;
	kStream >> m_bMayaBoostMusicians;
#endif
	if (uiVersion >= 14)
	{
		kStream >> m_iExtraLeagueVotes;
	}
	else
	{
		m_iExtraLeagueVotes = 0;
	}
#if defined(TRAITIFY)
	kStream >> m_iTraitExtraLeagueVotes;
#endif
#if defined(MISC_CHANGES)
	kStream >> m_iTechExtraLeagueVotes;
	kStream >> m_iPolicyExtraLeagueVotes;
	kStream >> m_iMiscTradeRoutes;
#endif
#if defined(LEKMOD_v34)
	kStream >> m_bCanChooseReformationBelief;
#endif
	kStream >> m_iSpecialPolicyBuildingHappiness;
	kStream >> m_iWoundedUnitDamageMod;
	kStream >> m_iUnitUpgradeCostMod;
	kStream >> m_iBarbarianCombatBonus;
	kStream >> m_iAlwaysSeeBarbCampsCount;
	kStream >> m_iHappinessFromBuildings;
	kStream >> m_iHappinessPerCity;
	kStream >> m_iAdvancedStartPoints;
	kStream >> m_iAttackBonusTurns;
	if (uiVersion >= 9)
	{
		kStream >> m_iCultureBonusTurns;
		kStream >> m_iTourismBonusTurns;
	}
	else
	{
		m_iCultureBonusTurns = 0;
		m_iTourismBonusTurns = 0;
	}
	kStream >> m_iGoldenAgeProgressMeter;
	kStream >> m_iGoldenAgeMeterMod;
	kStream >> m_iNumGoldenAges;
	kStream >> m_iGoldenAgeTurns;
	kStream >> m_iNumUnitGoldenAges;
	kStream >> m_iStrikeTurns;
	kStream >> m_iGoldenAgeModifier;
	kStream >> m_iGreatPeopleCreated;
	kStream >> m_iGreatGeneralsCreated;
	kStream >> m_iGreatAdmiralsCreated;
	kStream >> m_iGreatWritersCreated;
	kStream >> m_iGreatArtistsCreated;
	kStream >> m_iGreatMusiciansCreated;
#ifdef NQ_FREE_SETTLERS_FROM_BELIEF
	kStream >> m_bHasUsedReligiousSettlements;
#endif
#ifdef NQ_DEUS_VULT
	kStream >> m_bHasUsedDeusVult;
#endif
#ifdef NQ_GOLDEN_AGE_TURNS_FROM_BELIEF
	kStream >> m_bHasUsedDharma;
#endif
	kStream >> m_iGreatScientistsCreated;
	kStream >> m_iGreatEngineersCreated;
	kStream >> m_iGreatMerchantsCreated;
	kStream >> m_iGreatProphetsCreated;
	kStream >> m_iMerchantsFromFaith;
	kStream >> m_iScientistsFromFaith;
	kStream >> m_iWritersFromFaith;
	kStream >> m_iArtistsFromFaith;
	kStream >> m_iMusiciansFromFaith;
	kStream >> m_iGeneralsFromFaith;
	kStream >> m_iAdmiralsFromFaith;
	kStream >> m_iEngineersFromFaith;
	kStream >> m_iGreatPeopleThresholdModifier;
	kStream >> m_iGreatGeneralsThresholdModifier;
	kStream >> m_iGreatAdmiralsThresholdModifier;
	kStream >> m_iGreatGeneralCombatBonus;
	kStream >> m_iAnarchyNumTurns;
	kStream >> m_iPolicyCostModifier;
	kStream >> m_iGreatPeopleRateModifier;
	kStream >> m_iGreatPeopleRateModFromBldgs;
	kStream >> m_iGreatGeneralRateModifier;
	kStream >> m_iGreatGeneralRateModFromBldgs;
	kStream >> m_iDomesticGreatGeneralRateModifier;
	kStream >> m_iDomesticGreatGeneralRateModFromBldgs;
	kStream >> m_iGreatAdmiralRateModifier;
	kStream >> m_iGreatWriterRateModifier;
	kStream >> m_iGreatArtistRateModifier;
	kStream >> m_iGreatMusicianRateModifier;
	kStream >> m_iGreatMerchantRateModifier;
	kStream >> m_iGreatScientistRateModifier;
	if (uiVersion >= 10)
	{
		kStream >> m_iGreatScientistBeakerModifier;
	}
	else
	{
		m_iGreatScientistBeakerModifier = 0;
	}
	if (uiVersion >= 13)
	{
		kStream >> m_iGreatEngineerRateModifier;
	}
	else
	{
		m_iGreatEngineerRateModifier = 0;
	}
	kStream >> m_iGreatPersonExpendGold;
	kStream >> m_iMaxGlobalBuildingProductionModifier;
	kStream >> m_iMaxTeamBuildingProductionModifier;
	kStream >> m_iMaxPlayerBuildingProductionModifier;
	kStream >> m_iFreeExperience;
	kStream >> m_iFreeExperienceFromBldgs;
	kStream >> m_iFreeExperienceFromMinors;
	kStream >> m_iFeatureProductionModifier;
	kStream >> m_iWorkerSpeedModifier;
	kStream >> m_iImprovementCostModifier;
	kStream >> m_iImprovementUpgradeRateModifier;
	kStream >> m_iSpecialistProductionModifier;
	kStream >> m_iMilitaryProductionModifier;
	kStream >> m_iSpaceProductionModifier;
	kStream >> m_iCityDefenseModifier;
	kStream >> m_iUnitFortificationModifier;
	kStream >> m_iUnitBaseHealModifier;
	kStream >> m_iWonderProductionModifier;
	kStream >> m_iSettlerProductionModifier;
	kStream >> m_iCapitalSettlerProductionModifier;
	kStream >> m_iUnitProductionMaintenanceMod;
	kStream >> m_iPolicyCostBuildingModifier;
	kStream >> m_iPolicyCostMinorCivModifier;
#if !defined(LEK_YIELD_TOURISM)
	kStream >> m_iInfluenceSpreadModifier;
#endif
	if (uiVersion >= 15)
	{
		kStream >> m_iExtraVotesPerDiplomat;
	}
	else
	{
		m_iExtraVotesPerDiplomat = 0;
	}
	kStream >> m_iNumNukeUnits;
	kStream >> m_iNumOutsideUnits;
	kStream >> m_iBaseFreeUnits;
	kStream >> m_iBaseFreeMilitaryUnits;
	kStream >> m_iFreeUnitsPopulationPercent;
	kStream >> m_iFreeMilitaryUnitsPopulationPercent;
	kStream >> m_iGoldPerUnit;
	kStream >> m_iGoldPerMilitaryUnit;
	kStream >> m_iRouteGoldMaintenanceMod;
	kStream >> m_iBuildingGoldMaintenanceMod;
	kStream >> m_iUnitGoldMaintenanceMod;
	kStream >> m_iUnitSupplyMod;
	kStream >> m_iExtraUnitCost;
	kStream >> m_iNumMilitaryUnits;
	kStream >> m_iHappyPerMilitaryUnit;
	kStream >> m_iHappinessToCulture;
	kStream >> m_iHappinessToScience;
#ifdef NQ_GOLD_TO_SCIENCE_FROM_POLICIES
	kStream >> m_iGoldToScience;
#endif
	kStream >> m_iHalfSpecialistUnhappinessCount;
	kStream >> m_iHalfMoreSpecialistUnhappinessCount;
	kStream >> m_iHalfSpecialistFoodCount;
	kStream >> m_iMilitaryFoodProductionCount;
	kStream >> m_iGoldenAgeCultureBonusDisabledCount;
	kStream >> m_iSecondReligionPantheonCount;
	if (uiVersion >= 2)
	{
		kStream >> m_iEnablesSSPartHurryCount;
	}
	else
	{
		m_iEnablesSSPartHurryCount = 0;
	}
#ifdef NQ_DIABLE_RESISTANCE_TIME_VIA_POLICIES
	kStream >> m_iDisablesResistanceTimeCount;
#endif
#ifdef NQ_PATRIOTIC_WAR
	kStream >> m_iDoubleTrainedMilitaryLandUnitCount;
#endif
#ifdef NQ_WAR_HERO
	kStream >> m_iWarHeroCount;
#endif
#ifdef NQ_IGNORE_PUPPETS_FOR_RESEARCH_COSTS_FROM_POLICIES
	kStream >> m_iIgnorePuppetsForResearchCostsCount;
#endif
#ifdef NQ_POLICY_TOGGLE_NO_MINOR_DOW_IF_FRIENDS
	kStream >> m_iNoMinorDOWIfFriendsCount;
#endif
#ifdef NQ_ALLOW_PUPPET_PURCHASING_FROM_POLICIES
	kStream >> m_iAllowPuppetPurchasingCount;
#endif
#if defined(LEKMOD_GREAT_FIREWALL_PLAYER_EFFECT)
	kStream >> m_iInfluenceNullificationCount;
#endif
	if (uiVersion >= 3)
	{
		kStream >> m_iEnablesSSPartPurchaseCount;
	}
	else
	{
		m_iEnablesSSPartPurchaseCount = 0;
	}
	kStream >> m_iConscriptCount;
	kStream >> m_iMaxConscript;
	kStream >> m_iHighestUnitLevel;
	kStream >> m_iOverflowResearch;
	kStream >> m_iExpModifier;
	kStream >> m_iExpInBorderModifier;
	kStream >> m_iLevelExperienceModifier;
	kStream >> m_iMinorQuestFriendshipMod;
	kStream >> m_iMinorGoldFriendshipMod;
	kStream >> m_iMinorFriendshipMinimum;
	kStream >> m_iMinorFriendshipDecayMod;
	kStream >> m_iMinorScienceAlliesCount;
	kStream >> m_iMinorResourceBonusCount;
	if (uiVersion >= 12)
	{
		kStream >> m_iAbleToAnnexCityStatesCount;
	}
	else
	{
		m_iAbleToAnnexCityStatesCount = 0;
	}
	kStream >> m_iConversionTimer;
	kStream >> m_iCapitalCityID;
	kStream >> m_iCitiesLost;
	kStream >> m_iMilitaryMight;
	kStream >> m_iEconomicMight;
	kStream >> m_iTurnMightRecomputed;
	kStream >> m_iNewCityExtraPopulation;
	kStream >> m_iFreeFoodBox;
	kStream >> m_iScenarioScore1;
	kStream >> m_iScenarioScore2;
	kStream >> m_iScenarioScore3;
	kStream >> m_iScoreFromFutureTech;
	kStream >> m_iScenarioScore4;
	kStream >> m_iCombatExperience;
	kStream >> m_iNavalCombatExperience;
	kStream >> m_iLifetimeCombatExperience;
	kStream >> m_iBorderObstacleCount;
	kStream >> m_iNextOperationID;
	kStream >> m_iCostNextPolicy;
	kStream >> m_iNumBuilders;
	kStream >> m_iMaxNumBuilders;
	kStream >> m_iCityStrengthMod;
	kStream >> m_iCityGrowthMod;
	kStream >> m_iCapitalGrowthMod;
	kStream >> m_iNumPlotsBought;
	kStream >> m_iPlotGoldCostMod;
	kStream >> m_iPlotCultureCostModifier;
	kStream >> m_iPlotCultureExponentModifier;
	kStream >> m_iNumCitiesPolicyCostDiscount;
	kStream >> m_iNumCitiesResearchCostDiscount;
	kStream >> m_iGarrisonFreeMaintenanceCount;
	kStream >> m_iGarrisonedCityRangeStrikeModifier;
	kStream >> m_iNumCitiesFreeAestheticsSchools;
	kStream >> m_iNumCitiesFreePietyGardens;
	kStream >> m_iNumCitiesFreeWalls;
	kStream >> m_iNumCitiesFreeCultureBuilding;
	kStream >> m_iNumCitiesFreeFoodBuilding;
#if defined(LEKMOD_FIX_PATRO_FOOD)
	kStream >> m_iCityStateBonusModifier;
#endif
	kStream >> m_iUnitPurchaseCostModifier;
	kStream >> m_iAllFeatureProduction;
	kStream >> m_iCityDistanceHighwaterMark;
	kStream >> m_iOriginalCapitalX;
	kStream >> m_iOriginalCapitalY;
	kStream >> m_iNumWonders;
	kStream >> m_iNumPolicies;
	kStream >> m_iNumGreatPeople;
	kStream >> m_iCityConnectionHappiness;
	kStream >> m_iHolyCityID;
	kStream >> m_iTurnsSinceSettledLastCity;
	kStream >> m_iNumNaturalWondersDiscoveredInArea;
	kStream >> m_iStrategicResourceMod;
	kStream >> m_iSpecialistCultureChange;
	kStream >> m_iGreatPeopleSpawnCounter;
	kStream >> m_iFreeTechCount;
	kStream >> m_iMedianTechPercentage;
	kStream >> m_iNumFreePolicies;
	kStream >> m_iNumFreePoliciesEver;
	if (uiVersion >= 16)
	{
		kStream >> m_iNumFreeTenets;
	}
	else
	{
		m_iNumFreeTenets = 0;
	}
	kStream >> m_iNumFreeGreatPeople;
	kStream >> m_iNumMayaBoosts;
	kStream >> m_iNumFaithGreatPeople;
	kStream >> m_iNumArchaeologyChoices;

	int temp;
	kStream >> temp;
	m_eFaithPurchaseType = (FaithPurchaseTypes)temp;
	kStream >> m_iFaithPurchaseIndex;

	if (uiVersion >= 6)
	{
		kStream >> m_iMaxEffectiveCities;
	}
	else
	{
		m_iMaxEffectiveCities = 1;
	}

	kStream >> m_iLastSliceMoved;
#ifdef NQ_CHEAT_FIRST_ROYAL_LIBRARY_COMES_WITH_GREAT_WORK
	kStream >> m_bHasEverBuiltRoyalLibrary;
#endif
	kStream >> m_bHasBetrayedMinorCiv;
#ifdef PENALTY_FOR_DELAYING_POLICIES
	kStream >> m_bIsDelayedPolicy;
#endif
	kStream >> m_bAlive;
	kStream >> m_bEverAlive;
	kStream >> m_bBeingResurrected;
	kStream >> m_bTurnActive;
	kStream >> m_bAutoMoves;
	kStream >> m_bEndTurn;
#ifdef AUI_GAME_BETTER_HYBRID_MODE
	kStream >> m_iTurnOrder;
#else
	kStream >> m_bDynamicTurnsSimultMode;
#endif
	kStream >> m_bPbemNewTurn;
	kStream >> m_bExtendedGame;
	kStream >> m_bFoundedFirstCity;
	kStream >> m_iNumCitiesFounded;
	kStream >> m_bStrike;
	kStream >> m_bCramped;
	kStream >> m_bLostCapital;
	kStream >> m_eConqueror;
	kStream >> m_bHasAdoptedStateReligion;
	kStream >> m_bAlliesGreatPersonBiasApplied;
#if defined(LEKMOD_PROMO_YIELD_FROM_CONVERSION) && defined(LEKMOD_PROMO_CONVERSION_MAJORITY_ONLY_ONCE)
	if (uiVersion >= 17)
	{
		kStream >> m_aiConversionMajorityOnceUsedKeys;
	}
	else
	{
		m_aiConversionMajorityOnceUsedKeys.clear();
	}
#endif
	kStream >> m_eID;
	kStream >> m_ePersonalityType;
#if defined(LEKMOD_EXPERIMENTAL_CHANGES)
	kStream >> m_aiWorldWonderYieldChanges;
#endif
#if defined(STANDARDIZE_YIELDS)
	kStream >> m_aiYieldEverGeneratedTimes100;
	kStream >> m_aiYieldPerTurnFromMisc;
#endif
	kStream >> m_aiCityYieldChange;
	kStream >> m_aiCoastalCityYieldChange;
	kStream >> m_aiCapitalYieldChange;
	kStream >> m_aiCapitalYieldPerPopChange;
	kStream >> m_aiSeaPlotYield;
	kStream >> m_aiYieldRateModifier;
	kStream >> m_aiCapitalYieldRateModifier;

	if (uiVersion >= 4)
	{
		kStream >> m_aiGreatWorkYieldChange;
	}
	else
	{
		m_aiGreatWorkYieldChange.clear();
		m_aiGreatWorkYieldChange.resize(NUM_YIELD_TYPES, 0);
	}
	kStream >> m_aiExtraYieldThreshold;
	kStream >> m_aiSpecialistExtraYield;
	kStream >> m_aiProximityToPlayer;
	kStream >> m_aiResearchAgreementCounter;
	if (uiVersion >= 5)
	{
		kStream >> m_aiIncomingUnitTypes;
		kStream >> m_aiIncomingUnitCountdowns;
	}
	else
	{
		std::vector<int> aiOldIncomingUnitTypes;
		kStream >> aiOldIncomingUnitTypes;
		m_aiIncomingUnitTypes.clear();
		m_aiIncomingUnitTypes.resize(MAX_PLAYERS, NO_UNIT);


		m_aiIncomingUnitCountdowns.clear();
		m_aiIncomingUnitCountdowns.resize(MAX_PLAYERS, -1);
	}
	
	kStream >> m_aiMinorFriendshipAnchors;
	if (uiVersion >= 7)
	{
		kStream >> m_aiSiphonLuxuryCount;
	}
	else
	{
		m_aiSiphonLuxuryCount.clear();
		m_aiSiphonLuxuryCount.resize(MAX_PLAYERS, 0);
	}
	kStream >> m_strReligionKey;
	kStream >> m_strScriptData;

	CvAssertMsg((0 < GC.getNumResourceInfos()), "GC.getNumResourceInfos() is not greater than zero but it is expected to be in CvPlayer::read");
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiNumResourceUsed.dirtyGet());
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiNumResourceTotal.dirtyGet());
#ifdef LEKMOD_CS_BUILDING_STRATEGIC_NO_ALLY_SHARE
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiMinorStrategicResourceFromBuildings.dirtyGet());
#else
	std::vector<int> vDiscardMinorStrategicFromBuildings;
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, vDiscardMinorStrategicFromBuildings);
#endif
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiResourceGiftedToMinors.dirtyGet());
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiResourceExport.dirtyGet());
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiResourceImport.dirtyGet());
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiResourceFromMinors.dirtyGet());
	if (uiVersion >= 7)
	{
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiResourcesSiphoned.dirtyGet());
	}
	else
	{
		m_paiResourcesSiphoned.clear();
		m_paiResourcesSiphoned.resize(GC.getNumResourceInfos(), 0);
	}

	kStream >> m_paiImprovementCount;

	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiFreeBuildingCount.dirtyGet());
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiFreePromotionCount.dirtyGet());

#ifdef LEKMOD_UNITCOMBAT_FREE_PROMOTION
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiUnitCombatFreePromotionCount.dirtyGet());
#endif

	kStream >> m_paiUnitCombatProductionModifiers;
	kStream >> m_paiUnitCombatFreeExperiences;
	kStream >> m_paiUnitClassCount;
	kStream >> m_paiUnitClassMaking;
	kStream >> m_paiBuildingClassCount;
	kStream >> m_paiBuildingClassMaking;
	kStream >> m_paiProjectMaking;
	kStream >> m_paiHurryCount;
	kStream >> m_paiHurryModifier;

	CvAssertMsg((0 < GC.getNumTechInfos()), "GC.getNumTechInfos() is not greater than zero but it is expected to be in CvPlayer::read");

	kStream >> m_pabLoyalMember;

	kStream >> m_pabGetsScienceFromPlayer;

#ifdef CS_ALLYING_WAR_RESCTRICTION
	kStream >> m_ppaaiTurnCSWarAllowing;
	kStream >> m_ppaafTimeCSWarAllowing;
	kStream >> m_paiPriorityTurn;
	kStream >> m_piPriorityTime;
#endif

	m_pPlayerPolicies->Read(kStream);
	m_pEconomicAI->Read(kStream);
	m_pCitySpecializationAI->Read(kStream);
	m_pWonderProductionAI->Read(kStream);
	m_pMilitaryAI->Read(kStream);
	m_pGrandStrategyAI->Read(kStream);
	m_pDiplomacyAI->Read(kStream);
	m_pReligions->Read(kStream);
	m_pReligionAI->Read(kStream);
	m_pPlayerTechs->Read(kStream);
	m_pFlavorManager->Read(kStream);
	m_pTacticalAI->Read(kStream);
	m_pHomelandAI->Read(kStream);
	m_pMinorCivAI->Read(kStream);
	m_pDealAI->Read(kStream);
	m_pBuilderTaskingAI->Read(kStream);
	m_pCityConnections->Read(kStream);
	m_pDangerPlots->Read(kStream);
	{ FILogFile* pDbg = LOGFILEMGR.GetLog("LoadDebug.log", FILogFile::kDontTimeStamp); pDbg->Msg("Player %d: before Traits used=%u", GetID(), kStream.GetSizeLeft()); }
	m_pTraits->Read(kStream);
	{ FILogFile* pDbg = LOGFILEMGR.GetLog("LoadDebug.log", FILogFile::kDontTimeStamp); pDbg->Msg("Player %d: after Traits used=%u", GetID(), kStream.GetSizeLeft()); }
	kStream >> *m_pEspionage;
	{ FILogFile* pDbg = LOGFILEMGR.GetLog("LoadDebug.log", FILogFile::kDontTimeStamp); pDbg->Msg("Player %d: after Espionage used=%u", GetID(), kStream.GetSizeLeft()); }
	kStream >> *m_pEspionageAI;
	{ FILogFile* pDbg = LOGFILEMGR.GetLog("LoadDebug.log", FILogFile::kDontTimeStamp); pDbg->Msg("Player %d: after EspionageAI used=%u", GetID(), kStream.GetSizeLeft()); }
	kStream >> *m_pTrade;
	{ FILogFile* pDbg = LOGFILEMGR.GetLog("LoadDebug.log", FILogFile::kDontTimeStamp); pDbg->Msg("Player %d: after Trade used=%u", GetID(), kStream.GetSizeLeft()); }
	kStream >> *m_pTradeAI;
	{ FILogFile* pDbg = LOGFILEMGR.GetLog("LoadDebug.log", FILogFile::kDontTimeStamp); pDbg->Msg("Player %d: after TradeAI used=%u", GetID(), kStream.GetSizeLeft()); }
	m_pLeagueAI->Read(kStream);
	{ FILogFile* pDbg = LOGFILEMGR.GetLog("LoadDebug.log", FILogFile::kDontTimeStamp); pDbg->Msg("Player %d: after LeagueAI used=%u, about to read Culture", GetID(), kStream.GetSizeLeft()); }
	kStream >> *m_pCulture;
	{ FILogFile* pDbg = LOGFILEMGR.GetLog("LoadDebug.log", FILogFile::kDontTimeStamp); pDbg->Msg("Player %d: after Culture used=%u", GetID(), kStream.GetSizeLeft()); }

	bool bReadNotifications;
	kStream >> bReadNotifications;
	if(bReadNotifications)
	{
		m_pNotifications = FNEW(CvNotifications, c_eCiv5GameplayDLL, 0);
		m_pNotifications->Init(GetID());
		m_pNotifications->Read(kStream);
	}
	m_pTreasury->Read(kStream);


	if(GetID() != NO_PLAYER)
	{
		SlotStatus s = CvPreGame::slotStatus(GetID());
		if((s == SS_TAKEN || s == SS_COMPUTER) && !isBarbarian())
		{
			m_pFlavorManager->AddFlavorRecipient(m_pPlayerTechs,        false                      );
			m_pFlavorManager->AddFlavorRecipient(m_pPlayerPolicies,     false                      );
			m_pFlavorManager->AddFlavorRecipient(m_pWonderProductionAI, false                      );
		}
	}
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	kStream >> m_ppaaiGreatWorkClassYieldChange;
#endif
	kStream >> m_ppaaiSpecialistExtraYield;
	kStream >> m_ppaaiImprovementYieldChange;
#if defined(STANDARDIZE_YIELDS)
	kStream >> m_aBuildingYieldChangeCache;
#endif

	kStream >> m_UnitCycle;
	kStream >> m_researchQueue;

	kStream >> m_bEverPoppedGoody;
	kStream >> m_bEverTrainedBuilder;
	kStream >> m_eEndTurnBlockingType;
	kStream >> m_iEndTurnBlockingNotificationIndex;

	kStream >> m_cityNames;

	kStream >> m_cities;
	kStream >> m_units;
	kStream >> m_armyAIs;

	{
		m_AIOperations.clear();
		uint iSize;
		int iID;
		int iOperationType;
		kStream >> iSize;
		for(uint i = 0; i < iSize; i++)
		{
			kStream >> iID;
			kStream >> iOperationType;
			CvAIOperation* pThisOperation = CvAIOperation::CreateOperation((AIOperationTypes)iOperationType, m_eID);
			pThisOperation->Read(kStream);
			m_AIOperations.insert(std::make_pair(pThisOperation->GetID(), pThisOperation));
		}
	}

	if (uiVersion <= 10)
	{

		int iSize;
		kStream >> iSize;
		CvAssert(iSize == 0);
	}

	kStream >> m_ReplayDataSets;
	kStream >> m_ReplayDataSetValues;

	kStream >> m_aVote;
	kStream >> m_aUnitExtraCosts;


	{
		m_aiPlots.clear();
		m_aiPlots.push_back_copy(-1, GC.getMap().numPlots());


		int iSize;
		kStream >> iSize;
		for(int i = 0; i < iSize; i++)
		{
			kStream >> m_aiPlots[i];
		}
	}

	if(!isBarbarian())
	{

		setNetID(gDLL->getAssignedNetworkID(GetID()));
	}

	kStream >> m_iPopRushHurryCount;
	kStream >> m_iTotalImprovementsBuilt;

	m_bfEverConqueredBy.ClearAll();
	int iSize;
	kStream >> iSize;
	for(int i = 0; i < iSize; i++)
	{
		bool bValue;
		kStream >> bValue;
		if(bValue)
		{
			m_bfEverConqueredBy.SetBit(i);
		}
	}

	kStream >> m_strEmbarkedGraphicOverride;
	m_kPlayerAchievements.Read(kStream);

	if(GetID() < MAX_MAJOR_CIVS)
	{
		if(!m_pDiplomacyRequests)
			m_pDiplomacyRequests = FNEW(CvDiplomacyRequests, c_eCiv5GameplayDLL, 0);
		else
			m_pDiplomacyRequests->Uninit();

		m_pDiplomacyRequests->Init(GetID());

	}

#ifdef AUTOSAVE_FIX_PREVENT_TURN_SKIP

	if (CvPreGame::gameType() == GAME_NETWORK_MULTIPLAYER && m_bAlive)
	{

		m_bEndTurn = false;
	}
#endif

	if(m_bTurnActive)
		GC.getGame().changeNumGameTurnActive(1, std::string("setTurnActive() [loading save game] for player ") + getName());

		kStream >> m_ppiImprovementYieldChange;
		kStream >> m_ppiResourceYieldChange;

}






void CvPlayer::Write(FDataStream& kStream) const
{

	kStream << g_CurrentCvPlayerVersion;

	kStream << m_iStartingX;
	kStream << m_iStartingY;
	kStream << m_iTotalPopulation;
	kStream << m_iTotalLand;
	kStream << m_iTotalLandScored;
	kStream << m_iJONSCulturePerTurnForFree;
	kStream << m_iJONSCulturePerTurnFromMinorCivs;
	kStream << m_iJONSCultureCityModifier;
#ifdef AUI_PLAYER_FIX_JONS_CULTURE_IS_T100
	kStream << m_iJONSCultureT100;
	kStream << m_iJONSCultureEverGeneratedT100;
#else
	kStream << m_iJONSCulture;
	kStream << m_iJONSCultureEverGenerated;
#endif
#if !defined(LEKMOD_EXPERIMENTAL_CHANGES)
	kStream << m_iCulturePerWonder;
#endif
	kStream << m_iCultureWonderMultiplier;
	kStream << m_iCulturePerTechResearched;
	kStream << m_iFaith;
	kStream << m_iFaithEverGenerated;
	kStream << m_iHappiness;
	kStream << m_iUprisingCounter;
	kStream << m_iExtraHappinessPerLuxury;
	kStream << m_iUnhappinessFromUnits;
	kStream << m_iUnhappinessFromUnitsMod;
	kStream << m_iUnhappinessMod;
	kStream << m_iCityCountUnhappinessMod;
	kStream << m_iOccupiedPopulationUnhappinessMod;
	kStream << m_iCapitalUnhappinessMod;
	kStream << m_iCityRevoltCounter;
	kStream << m_iHappinessPerGarrisonedUnitCount;
	kStream << m_iHappinessPerTradeRouteCount;
#ifdef NQ_RAIL_CONNECTION_HAPPINESS_FROM_POLICIES
	kStream << m_iHappinessPerRailConnectionCount;
#endif
	kStream << m_iHappinessPerXPopulation;
	kStream << m_iHappinessPerXPolicies;
	kStream << m_iHappinessFromLeagues;
	kStream << m_iEspionageModifier;
	kStream << m_iSpyStartingRank;
#ifdef ENHANCED_GRAPHS
	kStream << m_iNumStolenScience;
	kStream << m_iNumTrainedUnits;
	kStream << m_iNumKilledUnits;
	kStream << m_iNumLostUnits;
	kStream << m_iUnitsDamageDealt;
	kStream << m_iUnitsDamageTaken;
	kStream << m_iCitiesDamageDealt;
	kStream << m_iCitiesDamageTaken;
	kStream << m_iNumScientistsTotal;
	kStream << m_iNumEngineersTotal;
	kStream << m_iNumMerchantsTotal;
	kStream << m_iNumWritersTotal;
	kStream << m_iNumArtistsTotal;
	kStream << m_iNumMusiciansTotal;
	kStream << m_iNumGeneralsTotal;
	kStream << m_iNumAdmiralsTotal;
	kStream << m_iNumProphetsTotal;
	kStream << m_iProductionGoldFromWonders;
	kStream << m_iNumChops;
	kStream << m_iNumTimesOpenedDemographics;
	kStream << m_iUnitsDamageHealed;
	kStream << m_iTurnsStagnated;
	kStream << m_iGoldSpentBuys;
	kStream << m_iGoldSpentUpgrades;
	kStream << m_bMayaBoostScientist;
	kStream << m_bMayaBoostEngineers;
	kStream << m_bMayaBoostMerchants;
	kStream << m_bMayaBoostWriters;
	kStream << m_bMayaBoostArtists;
	kStream << m_bMayaBoostMusicians;
#endif
	kStream << m_iExtraLeagueVotes;
#if defined(TRAITIFY)
	kStream << m_iTraitExtraLeagueVotes;
#endif
#if defined(MISC_CHANGES)
	kStream << m_iTechExtraLeagueVotes;
	kStream << m_iPolicyExtraLeagueVotes;
	kStream << m_iMiscTradeRoutes;
#endif
#if defined(LEKMOD_v34)
	kStream << m_bCanChooseReformationBelief;
#endif
	kStream << m_iSpecialPolicyBuildingHappiness;
	kStream << m_iWoundedUnitDamageMod;
	kStream << m_iUnitUpgradeCostMod;
	kStream << m_iBarbarianCombatBonus;
	kStream << m_iAlwaysSeeBarbCampsCount;
	kStream << m_iHappinessFromBuildings;
	kStream << m_iHappinessPerCity;
	kStream << m_iAdvancedStartPoints;
	kStream << m_iAttackBonusTurns;
	kStream << m_iCultureBonusTurns;
	kStream << m_iTourismBonusTurns;
	kStream << m_iGoldenAgeProgressMeter;
	kStream << m_iGoldenAgeMeterMod;
	kStream << m_iNumGoldenAges;
	kStream << m_iGoldenAgeTurns;
	kStream << m_iNumUnitGoldenAges;
	kStream << m_iStrikeTurns;
	kStream << m_iGoldenAgeModifier;
	kStream << m_iGreatPeopleCreated;
	kStream << m_iGreatGeneralsCreated;
	kStream << m_iGreatAdmiralsCreated;
	kStream << m_iGreatWritersCreated;
	kStream << m_iGreatArtistsCreated;
	kStream << m_iGreatMusiciansCreated;
#ifdef NQ_FREE_SETTLERS_FROM_BELIEF
	kStream << m_bHasUsedReligiousSettlements;
#endif
#ifdef NQ_DEUS_VULT
	kStream << m_bHasUsedDeusVult;
#endif
#ifdef NQ_GOLDEN_AGE_TURNS_FROM_BELIEF
	kStream << m_bHasUsedDharma;
#endif
	kStream << m_iGreatScientistsCreated;
	kStream << m_iGreatEngineersCreated;
	kStream << m_iGreatMerchantsCreated;
	kStream << m_iGreatProphetsCreated;
	kStream << m_iMerchantsFromFaith;
	kStream << m_iScientistsFromFaith;
	kStream << m_iWritersFromFaith;
	kStream << m_iArtistsFromFaith;
	kStream << m_iMusiciansFromFaith;
	kStream << m_iGeneralsFromFaith;
	kStream << m_iAdmiralsFromFaith;
	kStream << m_iEngineersFromFaith;
	kStream << m_iGreatPeopleThresholdModifier;
	kStream << m_iGreatGeneralsThresholdModifier;
	kStream << m_iGreatAdmiralsThresholdModifier;
	kStream << m_iGreatGeneralCombatBonus;
	kStream << m_iAnarchyNumTurns;
	kStream << m_iPolicyCostModifier;
	kStream << m_iGreatPeopleRateModifier;
	kStream << m_iGreatPeopleRateModFromBldgs;
	kStream << m_iGreatGeneralRateModifier;
	kStream << m_iGreatGeneralRateModFromBldgs;
	kStream << m_iDomesticGreatGeneralRateModifier;
	kStream << m_iDomesticGreatGeneralRateModFromBldgs;
	kStream << m_iGreatAdmiralRateModifier;
	kStream << m_iGreatWriterRateModifier;
	kStream << m_iGreatArtistRateModifier;
	kStream << m_iGreatMusicianRateModifier;
	kStream << m_iGreatMerchantRateModifier;
	kStream << m_iGreatScientistRateModifier;
	kStream << m_iGreatScientistBeakerModifier;
	kStream << m_iGreatEngineerRateModifier;
	kStream << m_iGreatPersonExpendGold;
	kStream << m_iMaxGlobalBuildingProductionModifier;
	kStream << m_iMaxTeamBuildingProductionModifier;
	kStream << m_iMaxPlayerBuildingProductionModifier;
	kStream << m_iFreeExperience;
	kStream << m_iFreeExperienceFromBldgs;
	kStream << m_iFreeExperienceFromMinors;
	kStream << m_iFeatureProductionModifier;
	kStream << m_iWorkerSpeedModifier;
	kStream << m_iImprovementCostModifier;
	kStream << m_iImprovementUpgradeRateModifier;
	kStream << m_iSpecialistProductionModifier;
	kStream << m_iMilitaryProductionModifier;
	kStream << m_iSpaceProductionModifier;
	kStream << m_iCityDefenseModifier;
	kStream << m_iUnitFortificationModifier;
	kStream << m_iUnitBaseHealModifier;
	kStream << m_iWonderProductionModifier;
	kStream << m_iSettlerProductionModifier;
	kStream << m_iCapitalSettlerProductionModifier;
	kStream << m_iUnitProductionMaintenanceMod;
	kStream << m_iPolicyCostBuildingModifier;
	kStream << m_iPolicyCostMinorCivModifier;
#if !defined(LEK_YIELD_TOURISM)
	kStream << m_iInfluenceSpreadModifier;
#endif
	kStream << m_iExtraVotesPerDiplomat;
	kStream << m_iNumNukeUnits;
	kStream << m_iNumOutsideUnits;
	kStream << m_iBaseFreeUnits;
	kStream << m_iBaseFreeMilitaryUnits;
	kStream << m_iFreeUnitsPopulationPercent;
	kStream << m_iFreeMilitaryUnitsPopulationPercent;
	kStream << m_iGoldPerUnit;
	kStream << m_iGoldPerMilitaryUnit;
	kStream << m_iRouteGoldMaintenanceMod;
	kStream << m_iBuildingGoldMaintenanceMod;
	kStream << m_iUnitGoldMaintenanceMod;
	kStream << m_iUnitSupplyMod;
	kStream << m_iExtraUnitCost;
	kStream << m_iNumMilitaryUnits;
	kStream << m_iHappyPerMilitaryUnit;
	kStream << m_iHappinessToCulture;
	kStream << m_iHappinessToScience;
#ifdef NQ_GOLD_TO_SCIENCE_FROM_POLICIES
	kStream << m_iGoldToScience;
#endif
	kStream << m_iHalfSpecialistUnhappinessCount;
	kStream << m_iHalfMoreSpecialistUnhappinessCount;
	kStream << m_iHalfSpecialistFoodCount;
	kStream << m_iMilitaryFoodProductionCount;
	kStream << m_iGoldenAgeCultureBonusDisabledCount;
	kStream << m_iSecondReligionPantheonCount;
	kStream << m_iEnablesSSPartHurryCount;
#ifdef NQ_DIABLE_RESISTANCE_TIME_VIA_POLICIES
	kStream << m_iDisablesResistanceTimeCount;
#endif
#ifdef NQ_PATRIOTIC_WAR
	kStream << m_iDoubleTrainedMilitaryLandUnitCount;
#endif
#ifdef NQ_WAR_HERO
	kStream << m_iWarHeroCount;
#endif
#ifdef NQ_IGNORE_PUPPETS_FOR_RESEARCH_COSTS_FROM_POLICIES
	kStream << m_iIgnorePuppetsForResearchCostsCount;
#endif
#ifdef NQ_POLICY_TOGGLE_NO_MINOR_DOW_IF_FRIENDS
	kStream << m_iNoMinorDOWIfFriendsCount;
#endif
#ifdef NQ_ALLOW_PUPPET_PURCHASING_FROM_POLICIES
	kStream << m_iAllowPuppetPurchasingCount;
#endif
#if defined(LEKMOD_GREAT_FIREWALL_PLAYER_EFFECT)
	kStream << m_iInfluenceNullificationCount;
#endif
	kStream << m_iEnablesSSPartPurchaseCount;
	kStream << m_iConscriptCount;
	kStream << m_iMaxConscript;
	kStream << m_iHighestUnitLevel;
	kStream << m_iOverflowResearch;
	kStream << m_iExpModifier;
	kStream << m_iExpInBorderModifier;
	kStream << m_iLevelExperienceModifier;
	kStream << m_iMinorQuestFriendshipMod;
	kStream << m_iMinorGoldFriendshipMod;
	kStream << m_iMinorFriendshipMinimum;
	kStream << m_iMinorFriendshipDecayMod;
	kStream << m_iMinorScienceAlliesCount;
	kStream << m_iMinorResourceBonusCount;
	kStream << m_iAbleToAnnexCityStatesCount;
	kStream << m_iConversionTimer;
	kStream << m_iCapitalCityID;
	kStream << m_iCitiesLost;
	kStream << m_iMilitaryMight;
	kStream << m_iEconomicMight;
	kStream << m_iTurnMightRecomputed;
	kStream << m_iNewCityExtraPopulation;
	kStream << m_iFreeFoodBox;
	kStream << m_iScenarioScore1;
	kStream << m_iScenarioScore2;
	kStream << m_iScenarioScore3;
	kStream << m_iScoreFromFutureTech;
	kStream << m_iScenarioScore4;
	kStream << m_iCombatExperience;
	kStream << m_iNavalCombatExperience;
	kStream << m_iLifetimeCombatExperience;
	kStream << m_iBorderObstacleCount;
	kStream << m_iNextOperationID;
	kStream << m_iCostNextPolicy;
	kStream << m_iNumBuilders;
	kStream << m_iMaxNumBuilders;
	kStream << m_iCityStrengthMod;
	kStream << m_iCityGrowthMod;
	kStream << m_iCapitalGrowthMod;
	kStream << m_iNumPlotsBought;
	kStream << m_iPlotGoldCostMod;
	kStream << m_iPlotCultureCostModifier;
	kStream << m_iPlotCultureExponentModifier;
	kStream << m_iNumCitiesPolicyCostDiscount;
	kStream << m_iNumCitiesResearchCostDiscount;
	kStream << m_iGarrisonFreeMaintenanceCount;
	kStream << m_iGarrisonedCityRangeStrikeModifier;
	kStream << m_iNumCitiesFreeAestheticsSchools;
	kStream << m_iNumCitiesFreePietyGardens;
	kStream << m_iNumCitiesFreeWalls;
	kStream << m_iNumCitiesFreeCultureBuilding;
	kStream << m_iNumCitiesFreeFoodBuilding;
#if defined(LEKMOD_FIX_PATRO_FOOD)
	kStream << m_iCityStateBonusModifier;
#endif
	kStream << m_iUnitPurchaseCostModifier;
	kStream << m_iAllFeatureProduction;
	kStream << m_iCityDistanceHighwaterMark;
	kStream << m_iOriginalCapitalX;
	kStream << m_iOriginalCapitalY;
	kStream << m_iNumWonders;
	kStream << m_iNumPolicies;
	kStream << m_iNumGreatPeople;
	kStream << m_iCityConnectionHappiness;
	kStream << m_iHolyCityID;
	kStream << m_iTurnsSinceSettledLastCity;
	kStream << m_iNumNaturalWondersDiscoveredInArea;
	kStream << m_iStrategicResourceMod;
	kStream << m_iSpecialistCultureChange;
	kStream << m_iGreatPeopleSpawnCounter;
	kStream << m_iFreeTechCount;
	kStream << m_iMedianTechPercentage;
	kStream << m_iNumFreePolicies;
	kStream << m_iNumFreePoliciesEver;
	kStream << m_iNumFreeTenets;
	kStream << m_iNumFreeGreatPeople;
	kStream << m_iNumMayaBoosts;
	kStream << m_iNumFaithGreatPeople;
	kStream << m_iNumArchaeologyChoices;
	kStream << m_eFaithPurchaseType;
	kStream << m_iFaithPurchaseIndex;
	kStream << m_iMaxEffectiveCities;
	kStream << m_iLastSliceMoved;

#ifdef NQ_CHEAT_FIRST_ROYAL_LIBRARY_COMES_WITH_GREAT_WORK
	kStream << m_bHasEverBuiltRoyalLibrary;
#endif
	kStream << m_bHasBetrayedMinorCiv;
#ifdef PENALTY_FOR_DELAYING_POLICIES
	kStream << m_bIsDelayedPolicy;
#endif
	kStream << m_bAlive;
	kStream << m_bEverAlive;
	kStream << m_bBeingResurrected;
	kStream << m_bTurnActive;
	kStream << m_bAutoMoves;
	kStream << m_bEndTurn;
#ifdef AUI_GAME_BETTER_HYBRID_MODE
	kStream << m_iTurnOrder;
#else
	kStream << m_bDynamicTurnsSimultMode;
#endif
	kStream << static_cast<bool>(m_bPbemNewTurn && GC.getGame().isPbem());
	kStream << m_bExtendedGame;
	kStream << m_bFoundedFirstCity;
	kStream << m_iNumCitiesFounded;
	kStream << m_bStrike;
	kStream << m_bCramped;
	kStream << m_bLostCapital;
	kStream << m_eConqueror;
	kStream << m_bHasAdoptedStateReligion;
	kStream << m_bAlliesGreatPersonBiasApplied;
#if defined(LEKMOD_PROMO_YIELD_FROM_CONVERSION) && defined(LEKMOD_PROMO_CONVERSION_MAJORITY_ONLY_ONCE)
	kStream << m_aiConversionMajorityOnceUsedKeys;
#endif

	kStream << m_eID;
	kStream << m_ePersonalityType;
#if defined(LEKMOD_EXPERIMENTAL_CHANGES)
	kStream << m_aiWorldWonderYieldChanges;
#endif
#if defined(STANDARDIZE_YIELDS)
	kStream << m_aiYieldEverGeneratedTimes100;
	kStream << m_aiYieldPerTurnFromMisc;
#endif
	kStream << m_aiCityYieldChange;
	kStream << m_aiCoastalCityYieldChange;
	kStream << m_aiCapitalYieldChange;
	kStream << m_aiCapitalYieldPerPopChange;
	kStream << m_aiSeaPlotYield;
	kStream << m_aiYieldRateModifier;
	kStream << m_aiCapitalYieldRateModifier;
	kStream << m_aiGreatWorkYieldChange;
	kStream << m_aiExtraYieldThreshold;
	kStream << m_aiSpecialistExtraYield;
	kStream << m_aiProximityToPlayer;
	kStream << m_aiResearchAgreementCounter;
	kStream << m_aiIncomingUnitTypes;
	kStream << m_aiIncomingUnitCountdowns;
	kStream << m_aiMinorFriendshipAnchors;
	kStream << m_aiSiphonLuxuryCount;



	kStream << m_strReligionKey;
	kStream << m_strScriptData;

	CvAssertMsg((0 < GC.getNumResourceInfos()), "GC.getNumResourceInfos() is not greater than zero but an array is being allocated in CvPlayer::write");
	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes, int>(kStream, m_paiNumResourceUsed);
	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes, int>(kStream, m_paiNumResourceTotal);
#ifdef LEKMOD_CS_BUILDING_STRATEGIC_NO_ALLY_SHARE
	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes, int>(kStream, m_paiMinorStrategicResourceFromBuildings);
#else
	std::vector<int> vDiscardMinorStrategicFromBuildings;
	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes, int>(kStream, vDiscardMinorStrategicFromBuildings);
#endif
	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes, int>(kStream, m_paiResourceGiftedToMinors);
	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes, int>(kStream, m_paiResourceExport);
	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes, int>(kStream, m_paiResourceImport);
	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes, int>(kStream, m_paiResourceFromMinors);
	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes, int>(kStream, m_paiResourcesSiphoned);

	kStream << m_paiImprovementCount;

	CvInfosSerializationHelper::WriteHashedDataArray<BuildingTypes, int>(kStream, m_paiFreeBuildingCount);

	CvInfosSerializationHelper::WriteHashedDataArray<PromotionTypes, int>(kStream, m_paiFreePromotionCount);

#ifdef LEKMOD_UNITCOMBAT_FREE_PROMOTION
	

	CvInfosSerializationHelper::WriteHashedDataArray<UnitCombatTypes, int>(kStream, m_paiUnitCombatFreePromotionCount);

#endif

	kStream << m_paiUnitCombatProductionModifiers;
	kStream << m_paiUnitCombatFreeExperiences;
	kStream << m_paiUnitClassCount;
	kStream << m_paiUnitClassMaking;
	kStream << m_paiBuildingClassCount;
	kStream << m_paiBuildingClassMaking;
	kStream << m_paiProjectMaking;
	kStream << m_paiHurryCount;
	kStream << m_paiHurryModifier;


	CvAssertMsg((0 < GC.getNumTechInfos()), "GC.getNumTechInfos() is not greater than zero but it is expected to be in CvPlayer::write");

	kStream << m_pabLoyalMember;

	kStream << m_pabGetsScienceFromPlayer;

#ifdef CS_ALLYING_WAR_RESCTRICTION
	kStream << m_ppaaiTurnCSWarAllowing;
	kStream << m_ppaafTimeCSWarAllowing;
	kStream << m_paiPriorityTurn;
	kStream << m_piPriorityTime;
#endif

	m_pPlayerPolicies->Write(kStream);
	m_pEconomicAI->Write(kStream);
	m_pCitySpecializationAI->Write(kStream);
	m_pWonderProductionAI->Write(kStream);
	m_pMilitaryAI->Write(kStream);
	m_pGrandStrategyAI->Write(kStream);
	m_pDiplomacyAI->Write(kStream);
	m_pReligions->Write(kStream);
	m_pReligionAI->Write(kStream);
	m_pPlayerTechs->Write(kStream);
	m_pFlavorManager->Write(kStream);
	m_pTacticalAI->Write(kStream);
	m_pHomelandAI->Write(kStream);
	m_pMinorCivAI->Write(kStream);
	m_pDealAI->Write(kStream);
	m_pBuilderTaskingAI->Write(kStream);
	m_pCityConnections->Write(kStream);
	m_pDangerPlots->Write(kStream);
	{ FILogFile* pDbg = LOGFILEMGR.GetLog("SaveDebug.log", FILogFile::kDontTimeStamp); pDbg->Msg("Player %d: before Traits used=%u", GetID(), kStream.GetSizeLeft()); }
	m_pTraits->Write(kStream);
	{ FILogFile* pDbg = LOGFILEMGR.GetLog("SaveDebug.log", FILogFile::kDontTimeStamp); pDbg->Msg("Player %d: after Traits used=%u", GetID(), kStream.GetSizeLeft()); }
	kStream << *m_pEspionage;
	{ FILogFile* pDbg = LOGFILEMGR.GetLog("SaveDebug.log", FILogFile::kDontTimeStamp); pDbg->Msg("Player %d: after Espionage used=%u", GetID(), kStream.GetSizeLeft()); }
	kStream << *m_pEspionageAI;
	{ FILogFile* pDbg = LOGFILEMGR.GetLog("SaveDebug.log", FILogFile::kDontTimeStamp); pDbg->Msg("Player %d: after EspionageAI used=%u", GetID(), kStream.GetSizeLeft()); }
	kStream << *m_pTrade;
	{ FILogFile* pDbg = LOGFILEMGR.GetLog("SaveDebug.log", FILogFile::kDontTimeStamp); pDbg->Msg("Player %d: after Trade used=%u", GetID(), kStream.GetSizeLeft()); }
	kStream << *m_pTradeAI;
	{ FILogFile* pDbg = LOGFILEMGR.GetLog("SaveDebug.log", FILogFile::kDontTimeStamp); pDbg->Msg("Player %d: after TradeAI used=%u", GetID(), kStream.GetSizeLeft()); }
	m_pLeagueAI->Write(kStream);
	{ FILogFile* pDbg = LOGFILEMGR.GetLog("SaveDebug.log", FILogFile::kDontTimeStamp); pDbg->Msg("Player %d: after LeagueAI used=%u, about to write Culture", GetID(), kStream.GetSizeLeft()); }
	kStream << *m_pCulture;
	{ FILogFile* pDbg = LOGFILEMGR.GetLog("SaveDebug.log", FILogFile::kDontTimeStamp); pDbg->Msg("Player %d: after Culture used=%u", GetID(), kStream.GetSizeLeft()); }

	if(m_pNotifications)
	{
		kStream << true;
		m_pNotifications->Write(kStream);
	}
	else
	{
		kStream << false;
	}
	m_pTreasury->Write(kStream);
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	kStream << m_ppaaiGreatWorkClassYieldChange;
#endif
	kStream << m_ppaaiSpecialistExtraYield;
	kStream << m_ppaaiImprovementYieldChange;
#if defined(STANDARDIZE_YIELDS)
	kStream << m_aBuildingYieldChangeCache;
#endif

	kStream << m_UnitCycle;
	kStream << m_researchQueue;

	kStream << m_bEverPoppedGoody;
	kStream << m_bEverTrainedBuilder;
	kStream << m_eEndTurnBlockingType;
	kStream << m_iEndTurnBlockingNotificationIndex;

	kStream << m_cityNames;
	kStream << m_cities;
	kStream << m_units;
	kStream << m_armyAIs;

	{
		uint iSize = m_AIOperations.size();
		kStream << iSize;
		std::map<int, CvAIOperation*>::const_iterator it;
		for(it = m_AIOperations.begin(); it != m_AIOperations.end(); ++it)
		{
			kStream << it->first;
			CvAIOperation* pThisOperation = it->second;
			kStream << pThisOperation->GetOperationType();
			pThisOperation->Write(kStream);
		}
	}

	kStream << m_ReplayDataSets;
	kStream << m_ReplayDataSetValues;

	kStream << m_aVote;
	kStream << m_aUnitExtraCosts;


	{

		int iSize = -1;
		for(int i = m_aiPlots.size() - 1; i >= 0; i--)
		{
			if(m_aiPlots[i] != -1)
			{
				iSize = i + 1;
				break;
			}
		}

		if(iSize < 0)
		{
			iSize = 0;
		}

		kStream << iSize;
		for(int i = 0; i < iSize; i++)
		{
			kStream << m_aiPlots[i];
		}
	}

	kStream << m_iPopRushHurryCount;
	kStream << m_iTotalImprovementsBuilt;


	{
		int iSize = MAX_PLAYERS;
		kStream << iSize;
		for(int i = 0; i < iSize; i++)
		{
			bool bValue = m_bfEverConqueredBy.GetBit(i);
			kStream << bValue;
		}
	}

	kStream << m_strEmbarkedGraphicOverride;


	m_kPlayerAchievements.Write(kStream);

	kStream << m_ppiImprovementYieldChange;
	kStream << m_ppiResourceYieldChange;
}


void CvPlayer::createGreatGeneral(UnitTypes eGreatPersonUnit, int iX, int iY)
{
	CvUnit* pGreatPeopleUnit = initUnit(eGreatPersonUnit, iX, iY);
	if(NULL == pGreatPeopleUnit)
	{
		CvAssert(false);
		return;
	}

	ChangeNumGreatPeople(1);

	incrementGreatGeneralsCreated();
#ifdef ENHANCED_GRAPHS
	ChangeNumGeneralsTotal(1);
#endif
#ifdef NQ_WAR_HERO
	if (IsWarHero())
	{
		addFreeUnit((UnitTypes)GC.getInfoTypeForString("UNIT_ARTIST"));
	}
#endif

	changeGreatGeneralsThresholdModifier(       GC.getGREAT_GENERALS_THRESHOLD_INCREASE() * ((getGreatGeneralsCreated() / 10) + 1));

	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
		{
			GET_PLAYER((PlayerTypes)iI).changeGreatGeneralsThresholdModifier(       GC.getGREAT_GENERALS_THRESHOLD_INCREASE_TEAM() * ((getGreatPeopleCreated() / 10) + 1));
		}
	}

	CvPlot* pPlot = GC.getMap().plot(iX, iY);


	if(pGreatPeopleUnit->isHuman() && !GC.getGame().isGameMultiPlayer())
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_GREATGENERALS);
		const char* strLeader = GET_PLAYER(pGreatPeopleUnit->getOwner()).getLeaderTypeKey();
		if(strLeader && strcmp(strLeader, "LEADER_WU_ZETIAN") == 0)
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_SUNTZU);
		}

		CvAchievementUnlocker::Check_PSG();
	}


	if(GetNotifications())
	{
		Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER");
		Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_GREAT_PERSON");
		GetNotifications()->Add(NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER, strText.toUTF8(), strSummary.toUTF8(), pPlot->getX(), pPlot->getY(), eGreatPersonUnit);
	}
}


void CvPlayer::createGreatAdmiral(UnitTypes eGreatPersonUnit, int iX, int iY)
{
	CvUnit* pGreatPeopleUnit = initUnit(eGreatPersonUnit, iX, iY);
	if(NULL == pGreatPeopleUnit)
	{
		CvAssert(false);
		return;
	}

	ChangeNumGreatPeople(1);
	CvPlot *pSpawnPlot = GetGreatAdmiralSpawnPlot(pGreatPeopleUnit);
	if (pGreatPeopleUnit->plot() != pSpawnPlot)
	{
		pGreatPeopleUnit->setXY(pSpawnPlot->getX(), pSpawnPlot->getY());
	}

	incrementGreatAdmiralsCreated();
#ifdef ENHANCED_GRAPHS
	ChangeNumAdmiralsTotal(1);
#endif
	changeGreatAdmiralsThresholdModifier(       GC.getGREAT_GENERALS_THRESHOLD_INCREASE() * ((getGreatAdmiralsCreated() / 10) + 1));

	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
		{
			GET_PLAYER((PlayerTypes)iI).changeGreatAdmiralsThresholdModifier(       GC.getGREAT_GENERALS_THRESHOLD_INCREASE_TEAM() * ((getGreatPeopleCreated() / 10) + 1));
		}
	}

	CvPlot* pPlot = GC.getMap().plot(iX, iY);


	if(GetNotifications())
	{
		Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER");
		Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_GREAT_PERSON");
		GetNotifications()->Add(NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER, strText.toUTF8(), strSummary.toUTF8(), pPlot->getX(), pPlot->getY(), eGreatPersonUnit);
	}
}


void CvPlayer::launch(VictoryTypes eVictory)
{
	CvTeam& kTeam = GET_TEAM(getTeam());

	if(!kTeam.canLaunch(eVictory))
	{
		return;
	}
}


int CvPlayer::getUnitExtraCost(UnitClassTypes eUnitClass) const
{
	for(std::vector< std::pair<UnitClassTypes, int> >::const_iterator it = m_aUnitExtraCosts.begin(); it != m_aUnitExtraCosts.end(); ++it)
	{
		if((*it).first == eUnitClass)
		{
			return ((*it).second);
		}
	}

	return 0;
}


void CvPlayer::setUnitExtraCost(UnitClassTypes eUnitClass, int iCost)
{
	for(std::vector< std::pair<UnitClassTypes, int> >::iterator it = m_aUnitExtraCosts.begin(); it != m_aUnitExtraCosts.end(); ++it)
	{
		if((*it).first == eUnitClass)
		{
			if(0 == iCost)
			{
				m_aUnitExtraCosts.erase(it);
			}
			else
			{
				(*it).second = iCost;
			}
			return;
		}
	}

	if(0 != iCost)
	{
		m_aUnitExtraCosts.push_back(std::make_pair(eUnitClass, iCost));
	}
}





void CvPlayer::invalidatePopulationRankCache()
{
	int iLoop;
	CvCity* pLoopCity;
	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->invalidatePopulationRankCache();
	}
}


void CvPlayer::invalidateYieldRankCache(YieldTypes)
{
	int iLoop;
	CvCity* pLoopCity;
	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->invalidateYieldRankCache();
	}
}


void CvPlayer::doUpdateCacheOnTurn()
{

#if defined(LEKMOD_AREA_BASED_CITY_YIELD)
	struct AreaYieldEntry
	{
		int iArea;
		int aiSame[NUM_YIELD_TYPES];
		int aiDifferent[NUM_YIELD_TYPES];

		AreaYieldEntry()
		{
			iArea = -1;
			for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
			{
				aiSame[iYield] = 0;
				aiDifferent[iYield] = 0;
			}
		}
	};

	std::vector<AreaYieldEntry> areaYields;
	int aiDifferentTotal[NUM_YIELD_TYPES] = {};
	
	int iLoop;
	YieldTypes eYield;
	CvCity* pLoopCity;
	CvCityBuildings* pCityBuildings;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pCityBuildings = pLoopCity->GetCityBuildings();
		if (!pCityBuildings)
			continue;
		const int iArea = pLoopCity->getArea();
		AreaYieldEntry* pEntry = NULL;

		for (size_t i = 0; i < areaYields.size(); i++)
		{
			if (areaYields[i].iArea == iArea)
			{
				pEntry = &areaYields[i];
				break;
			}
		}

		if (pEntry == NULL)
		{
			AreaYieldEntry kNewEntry;
			kNewEntry.iArea = iArea;
			areaYields.push_back(kNewEntry);
			pEntry = &areaYields.back();
		}

		for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
		{
			YieldTypes eYield = static_cast<YieldTypes>(iYield);

			const int iSame = pCityBuildings->GetSameLandMassYieldChange(eYield);
			const int iDifferent = pCityBuildings->GetDifferentLandMassYieldChange(eYield);

			pEntry->aiSame[iYield] += iSame;
			pEntry->aiDifferent[iYield] += iDifferent;

			aiDifferentTotal[iYield] += iDifferent;
		}
	}

	CvArea* pLoopArea;
	CvMap& kMap = GC.getMap();
	for (pLoopArea = kMap.firstArea(&iLoop); pLoopArea != NULL; pLoopArea = kMap.nextArea(&iLoop))
	{
		const int iArea = pLoopArea->GetID();
		AreaYieldEntry* pEntry = NULL;

		for (size_t i = 0; i < areaYields.size(); i++)
		{
			if (areaYields[i].iArea == iArea)
			{
				pEntry = &areaYields[i];
				break;
			}
		}

		for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
		{
			eYield = static_cast<YieldTypes>(iYield);

			const int iSame = pEntry ? pEntry->aiSame[iYield] : 0;
			const int iDifferentFromThisArea = pEntry ? pEntry->aiDifferent[iYield] : 0;
			const int iDifferentFromOtherAreas = aiDifferentTotal[iYield] - iDifferentFromThisArea;

			const int iFinalYield = iSame + iDifferentFromOtherAreas;

			pLoopArea->setCityYieldChange(GetID(), eYield, iFinalYield);
		}
	}
	updateYield();
#endif
}


PlayerTypes CvPlayer::pickConqueredCityOwner(const CvCity& kCity) const
{
	PlayerTypes eBestPlayer = kCity.getOriginalOwner();

	if(NO_PLAYER != eBestPlayer)
	{
		CvPlayer& kBestPlayer = GET_PLAYER(eBestPlayer);

		if(kBestPlayer.getTeam() == getTeam())
		{
			return eBestPlayer;
		}
	}

	return GetID();
}


bool CvPlayer::canStealTech(PlayerTypes eTarget, TechTypes eTech) const
{
	if(GET_TEAM(GET_PLAYER(eTarget).getTeam()).GetTeamTechs()->HasTech(eTech))
	{
#ifdef BUILD_STEALABLE_TECH_LIST_ONCE_PER_TURN
		if (GetPlayerTechs()->CanResearch(eTech) && GetEspionage()->IsTechStealable(eTarget, eTech) && GetEspionage()->m_aiNumTechsToStealList[eTarget] > 0)
#else
		if(GetPlayerTechs()->CanResearch(eTech))
#endif
		{
			return true;
		}
	}

	return false;
}


bool CvPlayer::canSpyDestroyUnit(PlayerTypes, CvUnit& kUnit) const
{
	if(kUnit.getTeam() == getTeam())
	{
		return false;
	}

	if(kUnit.getUnitInfo().GetProductionCost() <= 0)
	{
		return false;
	}

	if(!kUnit.plot()->isVisible(getTeam()))
	{
		return false;
	}

	return true;
}


bool CvPlayer::canSpyBribeUnit(PlayerTypes eTarget, CvUnit& kUnit) const
{
	if(!canSpyDestroyUnit(eTarget, kUnit))
	{
		return false;
	}


	if(kUnit.isEnemy(getTeam()))
	{
		return false;
	}

	IDInfo* pUnitNode = kUnit.plot()->headUnitNode();

	while(pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = kUnit.plot()->nextUnitNode(pUnitNode);

		if(NULL != pLoopUnit && pLoopUnit != &kUnit)
		{
			if(pLoopUnit->isEnemy(getTeam()))
			{

				return false;
			}
		}
	}

	return true;
}


bool CvPlayer::canSpyDestroyBuilding(PlayerTypes, BuildingTypes eBuilding) const
{
	CvBuildingEntry* pkBuilding = GC.getBuildingInfo(eBuilding);
	if(pkBuilding)
	{
		if(pkBuilding->GetProductionCost() <= 0)
		{
			return false;
		}

		if(::isLimitedWonderClass(pkBuilding->GetBuildingClassInfo()))
		{
			return false;
		}
	}

	return true;
}


bool CvPlayer::canSpyDestroyProject(PlayerTypes eTarget, ProjectTypes eProject) const
{
	CvProjectEntry& kProject = *GC.getProjectInfo(eProject);
	if(kProject.GetProductionCost() <= 0)
	{
		return false;
	}

	if(GET_TEAM(GET_PLAYER(eTarget).getTeam()).getProjectCount(eProject) <= 0)
	{
		return false;
	}

	if(::isWorldProject(eProject))
	{
		return false;
	}

	if(!kProject.IsSpaceship())
	{
		return false;
	}
	else
	{
		VictoryTypes eVictory = (VictoryTypes)kProject.GetVictoryPrereq();
		if(NO_VICTORY != eVictory)
		{

			if(GET_TEAM(GET_PLAYER(eTarget).getTeam()).getVictoryCountdown(eVictory) >= 0)
			{
				return false;
			}
		}
	}

	return true;
}


int CvPlayer::getNewCityProductionValue() const
{
	if(GC.getSETTLER_PRODUCTION_SPEED() != 0)
	{
		return GC.getSETTLER_PRODUCTION_SPEED();
	}

	int iValue = 0;
#ifdef AUI_WARNING_FIXES
	for (uint iJ = 0; iJ < GC.getNumBuildingClassInfos(); iJ++)
#else
	for(int iJ = 0; iJ < GC.getNumBuildingClassInfos(); iJ++)
#endif
	{
		const BuildingClassTypes eBuildingClass = static_cast<BuildingClassTypes>(iJ);
		CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
		if(pkBuildingClassInfo)
		{
			const BuildingTypes eBuilding = ((BuildingTypes)(getCivilizationInfo().getCivilizationBuildings(iJ)));
			if(NO_BUILDING != eBuilding)
			{
				CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
				if(pkBuildingInfo)
				{
					if(pkBuildingInfo->GetFreeStartEra() != NO_ERA)
					{
						if(GC.getGame().getStartEra() >= pkBuildingInfo->GetFreeStartEra())
						{
							iValue += (100 * getProductionNeeded(eBuilding)) / std::max(1, 100 + getProductionModifier(eBuilding));
						}
					}
				}
			}
		}
	}

	iValue *= 100 + GC.getNEW_CITY_BUILDING_VALUE_MODIFIER();
	iValue /= 100;

	CvGame& kGame = GC.getGame();

	iValue += (GC.getADVANCED_START_CITY_COST() * kGame.getGameSpeedInfo().getGrowthPercent()) / 100;

	int iPopulation = GC.getINITIAL_CITY_POPULATION() + kGame.getStartEraInfo().getFreePopulation();
	for(int i = 1; i <= iPopulation; ++i)
	{
		iValue += (getGrowthThreshold(i) * GC.getADVANCED_START_POPULATION_COST()) / 100;
	}

	return iValue;
}


int CvPlayer::getGrowthThreshold(int iPopulation) const
{
	CvAssertMsg(iPopulation > 0, "Population of city should be at least 1. Please show Jon this and send your last 5 autosaves.");

	int iThreshold;

	int iBaseThreshold =        GC.getBASE_CITY_GROWTH_THRESHOLD();

	int iExtraPopThreshold = int((iPopulation-1) *       GC.getCITY_GROWTH_MULTIPLIER());

	iBaseThreshold += iExtraPopThreshold;
	iExtraPopThreshold = (int) pow(double(iPopulation-1), (double)         GC.getCITY_GROWTH_EXPONENT());

	iThreshold = iBaseThreshold + iExtraPopThreshold;

	if(isMinorCiv())
	{
		iThreshold *= GC.getMINOR_CIV_GROWTH_PERCENT();
		iThreshold /= 100;
	}

	iThreshold *= GC.getGame().getGameSpeedInfo().getGrowthPercent();
	iThreshold /= 100;

	iThreshold *= GC.getGame().getStartEraInfo().getGrowthPercent();
	iThreshold /= 100;

	if(!isHuman() && !IsAITeammateOfHuman() && !isBarbarian())
	{
		iThreshold *= GC.getGame().getHandicapInfo().getAIGrowthPercent();
		iThreshold /= 100;

		iThreshold *= std::max(0, ((GC.getGame().getHandicapInfo().getAIPerEraModifier() * GetCurrentEra()) + 100));
		iThreshold /= 100;
	}

	return std::max(1, iThreshold);
}



void CvPlayer::InitPlots(void)
{
#ifdef AUI_WARNING_FIXES
	uint iNumPlots = GC.getMap().numPlots();
#else
	int iNumPlots = GC.getMap().getGridHeight() * GC.getMap().getGridHeight();
#endif

	if(iNumPlots != m_aiPlots.size())
	{
		m_aiPlots.clear();
		m_aiPlots.push_back_copy(-1, iNumPlots);
	}
}



void CvPlayer::UpdatePlots(void)
{
	if(m_aiPlots.size() == 0)
	{
		return;
	}

	int iPlotIndex = 0;
	int iMaxNumPlots = (int) m_aiPlots.size();
	while(iPlotIndex < iMaxNumPlots && m_aiPlots[iPlotIndex] != -1)
	{
		m_aiPlots[iPlotIndex] = -1;
		iPlotIndex++;
	}

	int iI;
	CvPlot* pLoopPlot;
	iPlotIndex = 0;
	int iNumPlotsInEntireWorld = GC.getMap().numPlots();
	for(iI = 0; iI < iNumPlotsInEntireWorld; iI++)
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
		if(pLoopPlot->getOwner() != m_eID)
		{
			continue;
		}

		m_aiPlots[iPlotIndex] = iI;
		iPlotIndex++;
	}
}



void CvPlayer::AddAPlot(CvPlot* pPlot)
{
	if(!pPlot)
	{
		return;
	}

	if(m_aiPlots.size() == 0)
	{
		return;
	}

	if(pPlot->getOwner() == m_eID)
	{
		return;
	}

	int iPlotIndex = 0;
	int iMaxNumPlots = (int)m_aiPlots.size();
	while(iPlotIndex < iMaxNumPlots && m_aiPlots[iPlotIndex] != -1)
	{
		iPlotIndex++;
	}

	m_aiPlots[iPlotIndex] = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());

}



CvPlotsVector& CvPlayer::GetPlots(void)
{
	return m_aiPlots;
}

#if defined(AUI_WARNING_FIXES) || defined(AUI_CONSTIFY)
const CvPlotsVector& CvPlayer::GetPlots() const
{
	return m_aiPlots;
}
#endif



int CvPlayer::GetNumPlots() const
{
	int iNumPlots = 0;

	CvPlot* pLoopPlot;
	int iNumPlotsInEntireWorld = GC.getMap().numPlots();
	for(int iI = 0; iI < iNumPlotsInEntireWorld; iI++)
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);

		if(pLoopPlot->getOwner() != m_eID)
			continue;

		iNumPlots++;
	}

	return iNumPlots;
}




int CvPlayer::GetCityStrengthMod() const
{
	return m_iCityStrengthMod;
}



void CvPlayer::SetCityStrengthMod(int iValue)
{
	CvAssert(iValue >= 0);
	m_iCityStrengthMod = iValue;


	CvCity* pLoopCity;
	int iLoop;

	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateStrengthValue();
	}
}



void CvPlayer::ChangeCityStrengthMod(int iChange)
{
	if(iChange != 0)
	{
		SetCityStrengthMod(GetCityStrengthMod() + iChange);
	}
}



int CvPlayer::GetCityGrowthMod() const
{
	return m_iCityGrowthMod;
}



void CvPlayer::SetCityGrowthMod(int iValue)
{
	CvAssert(iValue >= 0);
	m_iCityGrowthMod = iValue;
}



void CvPlayer::ChangeCityGrowthMod(int iChange)
{
	if(iChange != 0)
	{
		SetCityGrowthMod(GetCityGrowthMod() + iChange);
	}
}




int CvPlayer::GetCapitalGrowthMod() const
{
	return m_iCapitalGrowthMod;
}



void CvPlayer::SetCapitalGrowthMod(int iValue)
{
	CvAssert(iValue >= 0);
	m_iCapitalGrowthMod = iValue;
}



void CvPlayer::ChangeCapitalGrowthMod(int iChange)
{
	if(iChange != 0)
	{
		SetCapitalGrowthMod(GetCapitalGrowthMod() + iChange);
	}
}



int CvPlayer::GetNumPlotsBought() const
{
	return m_iNumPlotsBought;
}



void CvPlayer::SetNumPlotsBought(int iValue)
{
	CvAssert(iValue >= 0);
	m_iNumPlotsBought = iValue;
}



void CvPlayer::ChangeNumPlotsBought(int iChange)
{
	if(iChange != 0)
	{
		SetNumPlotsBought(GetNumPlotsBought() + iChange);
	}
}



int CvPlayer::GetBuyPlotCost() const
{
	int iCost =        GC.getPLOT_BASE_COST();
	iCost += (      GC.getPLOT_ADDITIONAL_COST_PER_PLOT() * GetNumPlotsBought());


	if(GetPlotGoldCostMod() != 0)
	{
		iCost *= (100 + GetPlotGoldCostMod());
		iCost /= 100;
	}

	if(isMinorCiv())
	{
		iCost *=         GC.getMINOR_CIV_GOLD_PERCENT();
		iCost /= 100;
	}

	return iCost;
}



int CvPlayer::GetPlotGoldCostMod() const
{
	return m_iPlotGoldCostMod;
}



void CvPlayer::ChangePlotGoldCostMod(int iChange)
{
	if(iChange != 0)
	{
		m_iPlotGoldCostMod += iChange;
	}
}



int CvPlayer::GetPlotCultureCostModifier() const
{
	return m_iPlotCultureCostModifier;
}



void CvPlayer::ChangePlotCultureCostModifier(int iChange)
{
	if(iChange != 0)
	{
		m_iPlotCultureCostModifier += iChange;
	}
}



int CvPlayer::GetPlotCultureExponentModifier() const
{
	return m_iPlotCultureExponentModifier;
}



void CvPlayer::ChangePlotCultureExponentModifier(int iChange)
{
	if(iChange != 0)
	{
		m_iPlotCultureExponentModifier += iChange;
	}
}



int CvPlayer::GetNumCitiesPolicyCostDiscount() const
{
	return m_iNumCitiesPolicyCostDiscount;
}



void CvPlayer::ChangeNumCitiesPolicyCostDiscount(int iChange)
{
	if(iChange != 0)
	{
		m_iNumCitiesPolicyCostDiscount += iChange;
	}
}



int CvPlayer::GetNumCitiesResearchCostDiscount() const
{
	return m_iNumCitiesResearchCostDiscount;
}

void CvPlayer::ChangeNumCitiesResearchCostDiscount(int iChange)
{
	if(iChange != 0)
	{
		m_iNumCitiesResearchCostDiscount += iChange;
	}
}




bool CvPlayer::IsGarrisonFreeMaintenance() const
{
	return m_iGarrisonFreeMaintenanceCount > 0;
}



void CvPlayer::ChangeGarrisonFreeMaintenanceCount(int iChange)
{
	if(iChange != 0)
	{
		m_iGarrisonFreeMaintenanceCount += iChange;
	}
}



int CvPlayer::GetGarrisonedCityRangeStrikeModifier() const
{
	return m_iGarrisonedCityRangeStrikeModifier;
}



void CvPlayer::ChangeGarrisonedCityRangeStrikeModifier(int iChange)
{
	if(iChange != 0)
	{
		m_iGarrisonedCityRangeStrikeModifier += iChange;
	}
}



int CvPlayer::GetUnitPurchaseCostModifier() const
{
	return m_iUnitPurchaseCostModifier;
}



void CvPlayer::ChangeUnitPurchaseCostModifier(int iChange)
{
	if(iChange != 0)
	{
		m_iUnitPurchaseCostModifier += iChange;
	}
}


int CvPlayer::GetPlotDanger(CvPlot& pPlot) const
{
	return m_pDangerPlots->GetDanger(pPlot);
}


bool CvPlayer::IsPlotUnderImmediateThreat(CvPlot& pPlot) const
{
	return m_pDangerPlots->IsUnderImmediateThreat(pPlot);
}



CvCity* CvPlayer::GetClosestFriendlyCity(CvPlot& plot, int iSearchRadius)
{
	CvCity* pClosestCity = NULL;
	CvCity* pLoopCity;
	int iBestDistance = INT_MAX;

	int iLoop;
	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		int iDistance = plotDistance(plot.getX(), plot.getY(), pLoopCity->getX(), pLoopCity->getY());
		if(iDistance < iBestDistance && iDistance <= iSearchRadius)
		{
			pClosestCity = pLoopCity;
			iBestDistance = iDistance;
		}
	}

	return pClosestCity;
}



int CvPlayer::GetNumPuppetCities() const
{
	int iNum = 0;

	const CvCity* pLoopCity;
	int iLoop;
	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if(pLoopCity->IsPuppet())
		{
			iNum++;
		}
	}

	return iNum;
}


int CvPlayer::GetMaxEffectiveCities(bool bIncludePuppets)
{
	int iNumCities = getNumCities();


	int iNumPuppetCities = GetNumPuppetCities();
	iNumCities -= iNumPuppetCities;


	int iNumLimboCities = 0;
	const CvCity* pLoopCity;
	int iLoop;
	for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if(pLoopCity->IsIgnoreCityForHappiness() || pLoopCity->IsRazing())
		{
			iNumLimboCities++;
		}
	}
	iNumCities -= iNumLimboCities;

	if(iNumCities == 0)
		iNumCities = 1;


	m_iMaxEffectiveCities = (m_iMaxEffectiveCities > iNumCities) ? m_iMaxEffectiveCities : iNumCities;

	if (bIncludePuppets)
	{
#ifdef FIX_MAX_EFFECTIVE_CITIES
		if (m_iMaxEffectiveCities > getNumCities() - iNumLimboCities)
		{
			return m_iMaxEffectiveCities;
		}
		else
		{
			return getNumCities() - iNumLimboCities;
		}
#else
		return m_iMaxEffectiveCities + iNumPuppetCities;
#endif
	}

	return m_iMaxEffectiveCities;
}


int CvPlayer::GetNumNaturalWondersDiscoveredInArea() const
{
	return m_iNumNaturalWondersDiscoveredInArea;
}



void CvPlayer::SetNumNaturalWondersDiscoveredInArea(int iValue)
{
	m_iNumNaturalWondersDiscoveredInArea = iValue;
}



void CvPlayer::ChangeNumNaturalWondersDiscoveredInArea(int iChange)
{
	SetNumNaturalWondersDiscoveredInArea(GetNumNaturalWondersDiscoveredInArea() + iChange);
}



#if defined(AUI_WARNING_FIXES) || defined(AUI_CONSTIFY)
int CvPlayer::GetNumNaturalWondersInOwnedPlots() const
#else
int CvPlayer::GetNumNaturalWondersInOwnedPlots()
#endif
{
	int iValue = 0;
#if defined(AUI_WARNING_FIXES) || defined(AUI_CONSTIFY)
	const CvPlotsVector& aiPlots = GetPlots();
#else
	CvPlotsVector& aiPlots = GetPlots();
#endif
	for(uint ui = 0; ui < aiPlots.size(); ui++)
	{

		if(aiPlots[ui] == -1)
		{
			break;
		}

		CvPlot* pPlot = GC.getMap().plotByIndex(aiPlots[ui]);
		if (pPlot && pPlot->IsNaturalWonder())
		{
			iValue++;
		}
	}
	return iValue;
}



int CvPlayer::GetTurnsSinceSettledLastCity() const
{
	return m_iTurnsSinceSettledLastCity;
}



void CvPlayer::SetTurnsSinceSettledLastCity(int iValue)
{
	if(m_iTurnsSinceSettledLastCity != iValue)
		m_iTurnsSinceSettledLastCity = iValue;
}



void CvPlayer::ChangeTurnsSinceSettledLastCity(int iChange)
{
	if(iChange != 0)
		SetTurnsSinceSettledLastCity(GetTurnsSinceSettledLastCity() + iChange);
}



int CvPlayer::GetBestSettleAreas(int iMinScore, int& iFirstArea, int& iSecondArea)
{
	CvArea* pLoopArea;
	int iLoop;
	int iBestScore = -1;
	int iSecondBestScore = -1;
	int iBestArea = -1;
	int iSecondBestArea = -1;
	int iNumFound = 0;
	int iScore;

	CvMap& theMap = GC.getMap();


	for(pLoopArea = theMap.firstArea(&iLoop); pLoopArea != NULL; pLoopArea = theMap.nextArea(&iLoop))
	{
		if(!pLoopArea->isWater())
		{
			iScore = pLoopArea->getTotalFoundValue();

			if(iScore >= iMinScore)
			{
				if(!(GC.getMap().GetAIMapHint() & 4) && !(EconomicAIHelpers::IsAreaSafeForQuickColony(pLoopArea->GetID(), this)))
				{
					iScore /= 3;
				}

				if(iScore > iBestScore)
				{

					if(iBestScore > iMinScore)
					{
						iSecondBestScore = iBestScore;
						iSecondBestArea = iBestArea;
					}
					iBestArea = pLoopArea->GetID();
					iBestScore = iScore;
				}

				else if(iScore > iSecondBestScore)
				{
					iSecondBestArea = pLoopArea->GetID();
					iSecondBestScore = iScore;
				}
			}
		}
	}


	iFirstArea = iBestArea;
	iSecondArea = iSecondBestArea;

	if(iSecondArea != -1)
	{
		iNumFound = 2;
	}
	else if(iFirstArea != -1)
	{
		iNumFound = 1;
	}
	return iNumFound;
}



CvPlot* CvPlayer::GetBestSettlePlot(CvUnit* pUnit, bool bEscorted, int iArea) const
{
	if(!pUnit)
		return NULL;

	int iSettlerX = pUnit->getX();
	int iSettlerY = pUnit->getY();
	int iUnitArea = pUnit->getArea();
	PlayerTypes eOwner = pUnit->getOwner();
	TeamTypes eTeam = pUnit->getTeam();

	int iBestFoundValue = 0;
	CvPlot* pBestFoundPlot = NULL;

	int iEvalDistance =        GC.getSETTLER_EVALUATION_DISTANCE();
	int iDistanceDropoffMod =        GC.getSETTLER_DISTANCE_DROPOFF_MODIFIER();

	iEvalDistance += (GC.getGame().getGameTurn() * 5) / 100;


	const int iDefaultNumTiles = 80*52;
	int iDefaultEvalDistance = iEvalDistance;
	iEvalDistance = (iEvalDistance * GC.getMap().numPlots()) / iDefaultNumTiles;
	iEvalDistance = max(iDefaultEvalDistance,iEvalDistance);

	if(bEscorted && GC.getMap().GetAIMapHint() & 5)
	{
		iEvalDistance *= 3;
		iEvalDistance /= 2;
	}

	else if(!bEscorted)
	{
		iEvalDistance *= 2;
		iEvalDistance /= 3;
	}

	CvMap& kMap = GC.getMap();
	int iNumPlots = kMap.numPlots();
	for(int iPlotLoop = 0; iPlotLoop < iNumPlots; iPlotLoop++)
	{
		CvPlot* pPlot = kMap.plotByIndexUnchecked(iPlotLoop);

		if(!pPlot)
		{
			continue;
		}

		if(pPlot->getOwner() != NO_PLAYER && pPlot->getOwner() != eOwner)
		{
			continue;
		}

		if(!pPlot->isRevealed(getTeam()))
		{
			continue;
		}

		if(!pUnit->canFound(pPlot))
		{
			continue;
		}

		if(iArea != -1 && pPlot->getArea() != iArea)
		{
			continue;
		}

		if(pPlot->IsAdjacentOwnedByOtherTeam(eTeam))
		{
			continue;
		}

		if (IsPlotTargetedForCity(pPlot))
		{
			continue;
		}


		if(bEscorted || (!pPlot->isVisibleEnemyUnit(eOwner)))
		{
			int iValue = pPlot->getFoundValue(eOwner);
			if(iValue > 5000)
			{
				int iSettlerDistance = ::plotDistance(pPlot->getX(), pPlot->getY(), iSettlerX, iSettlerY);
				int iDistanceDropoff = min(99,(iDistanceDropoffMod * iSettlerDistance) / iEvalDistance);
				iDistanceDropoff = max(0,iDistanceDropoff);
				iValue = iValue * (100 - iDistanceDropoff) / 100;
				if(pPlot->getArea() != iUnitArea)
				{
					if(GC.getMap().GetAIMapHint() & 5)
					{
						iValue *= 3;
						iValue /= 2;
					}
					else
					{
						iValue *= 2;
						iValue /= 3;
					}
				}
				if(iValue > iBestFoundValue)
				{
					iBestFoundValue = iValue;
					pBestFoundPlot = pPlot;
				}
			}
		}
	}
	return pBestFoundPlot;
}




int CvPlayer::GetNumWonders() const
{
	return m_iNumWonders;
}



void CvPlayer::ChangeNumWonders(int iValue)
{
	if(iValue != 0)
	{
		m_iNumWonders += iValue;
	}
}



int CvPlayer::GetNumPolicies() const
{
	return m_iNumPolicies;
}



void CvPlayer::ChangeNumPolicies(int iValue)
{
	if(iValue != 0)
	{
		m_iNumPolicies += iValue;
	}
}



int CvPlayer::GetNumGreatPeople() const
{
	return m_iNumGreatPeople;
}



void CvPlayer::ChangeNumGreatPeople(int iValue)
{
	if(iValue != 0)
	{
		m_iNumGreatPeople += iValue;
	}
}



void CvPlayer::DoAdoptedGreatPersonCityStatePolicy()
{

	PlayerTypes eMinor;
	for(int iPlayerLoop = MAX_MAJOR_CIVS; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eMinor = (PlayerTypes) iPlayerLoop;

		if(GET_PLAYER(eMinor).isEverAlive())
		{
			if(GET_PLAYER(eMinor).GetMinorCivAI()->GetAlly() == GetID())
			{
				DoSeedGreatPeopleSpawnCounter();

				break;
			}
		}
	}
}



bool CvPlayer::IsAlliesGreatPersonBiasApplied() const
{
	return m_bAlliesGreatPersonBiasApplied;
}



void CvPlayer::SetAlliesGreatPersonBiasApplied(bool bValue)
{
	if(m_bAlliesGreatPersonBiasApplied != bValue)
	{
		m_bAlliesGreatPersonBiasApplied = bValue;
	}
}



bool CvPlayer::IsHasAdoptedStateReligion() const
{
	return m_bHasAdoptedStateReligion;
}



void CvPlayer::SetHasAdoptedStateReligion(bool bValue)
{
	if(m_bHasAdoptedStateReligion != bValue)
	{
		m_bHasAdoptedStateReligion = bValue;
	}
}



int CvPlayer::GetNumCitiesWithStateReligion()
{
	int iNumCitiesWithStateReligion = 0;

	int iLoopCity;
	CvCity* pLoopCity = NULL;

	for(pLoopCity = firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = nextCity(&iLoopCity))
	{
		if(pLoopCity->GetPlayersReligion() == GetID())
		{
			iNumCitiesWithStateReligion++;
		}
	}

	return iNumCitiesWithStateReligion;
}



CvCity* CvPlayer::GetHolyCity()
{
	return getCity(m_iHolyCityID);
}



void CvPlayer::SetHolyCity(int iCityID)
{

	CvAssert(m_iHolyCityID == -1);

	m_iHolyCityID = iCityID;
}


PromotionTypes CvPlayer::GetEmbarkationPromotion() const
{
	if(GET_TEAM(getTeam()).canDefensiveEmbark())
	{
		return (PromotionTypes)GC.getPROMOTION_DEFENSIVE_EMBARKATION();
	}

	if(m_pTraits)
	{
		if(m_pTraits->IsEmbarkedAllWater())
		{
			return (PromotionTypes)GC.getPROMOTION_ALLWATER_EMBARKATION();
		}
	}

	return (PromotionTypes)GC.getPROMOTION_EMBARKATION();
}



void CvPlayer::DoAnnounceReligionAdoption()
{
	CvCity* pHolyCity = GetHolyCity();

	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& thisPlayer = GET_PLAYER((PlayerTypes)iI);
		if(thisPlayer.isHuman() && thisPlayer.isAlive() && thisPlayer.GetNotifications())
		{
			int iX = -1;
			int iY = -1;

			Localization::String localizedText;


			if(GC.getGame().getActivePlayer() == GetID())
			{
				iX = pHolyCity->getX();
				iY = pHolyCity->getY();
				localizedText = Localization::Lookup("TXT_KEY_MISC_RELIGION_ADOPTED_YOU");
				localizedText << pHolyCity->getNameKey() << GetStateReligionKey();
			}

			else if(GET_TEAM(GC.getGame().getActiveTeam()).isHasMet(thisPlayer.getTeam()))
			{
				localizedText = Localization::Lookup("TXT_KEY_MISC_RELIGION_ADOPTED_ANOTHER_PLAYER");
				localizedText << GET_PLAYER(pHolyCity->getOwner()).getNameKey() << GetStateReligionKey();


				if(pHolyCity->isRevealed(thisPlayer.getTeam(), false))
				{
					iX = pHolyCity->getX();
					iY = pHolyCity->getY();
				}
			}

			else
			{
				localizedText = Localization::Lookup("TXT_KEY_MISC_RELIGION_ADOPTED_UNKNOWN");
			}

			thisPlayer.GetNotifications()->Add(NOTIFICATION_RELIGION_RACE, localizedText.toUTF8(), localizedText.toUTF8(), iX, iY, -1);
		}
	}
}

bool CvPlayer::IsAllowedToTradeWith(PlayerTypes eOtherPlayer)
{
	if (GC.getGame().GetGameLeagues()->IsTradeEmbargoed(GetID(), eOtherPlayer) && eOtherPlayer != m_eID)
	{
		return false;
	}

	return true;
}

#ifdef CS_ALLYING_WAR_RESCTRICTION
int CvPlayer::getTurnCSWarAllowing(PlayerTypes ePlayer)
{
	int iValue = -1;
	for (int iI = 0; iI < MAX_MINOR_CIVS; iI++)
	{
		if (m_ppaaiTurnCSWarAllowing[ePlayer][iI] > iValue)
		{
			iValue = m_ppaaiTurnCSWarAllowing[ePlayer][iI];
		}
	}

	return iValue;
}

int CvPlayer::getTurnCSWarAllowingMinor(PlayerTypes ePlayer, PlayerTypes eMinor)
{
	return m_ppaaiTurnCSWarAllowing[ePlayer][int(eMinor) - MAX_MAJOR_CIVS];
}

void CvPlayer::setTurnCSWarAllowingMinor(PlayerTypes ePlayer, PlayerTypes eMinor, int iValue)
{
	Firaxis::Array<int, MAX_MINOR_CIVS> turn = m_ppaaiTurnCSWarAllowing[ePlayer];
	turn[int(eMinor) - MAX_MAJOR_CIVS] = iValue;
	m_ppaaiTurnCSWarAllowing.setAt(ePlayer, turn);
}

float CvPlayer::getTimeCSWarAllowing(PlayerTypes ePlayer)
{
	float fValue = 0.f;
	for (int iI = 0; iI < MAX_MINOR_CIVS; iI++)
	{
		if (m_ppaafTimeCSWarAllowing[ePlayer][iI] > fValue)
		{
			fValue = m_ppaafTimeCSWarAllowing[ePlayer][iI];
		}
	}

	return fValue;
}

float CvPlayer::getTimeCSWarAllowingMinor(PlayerTypes ePlayer, PlayerTypes eMinor)
{
	return m_ppaafTimeCSWarAllowing[ePlayer][int(eMinor) - MAX_MAJOR_CIVS];
}

void CvPlayer::setTimeCSWarAllowingMinor(PlayerTypes ePlayer, PlayerTypes eMinor, float fValue)
{
	Firaxis::Array<float, MAX_MINOR_CIVS> time = m_ppaafTimeCSWarAllowing[ePlayer];
	time[int(eMinor) - MAX_MAJOR_CIVS] = fValue;
	m_ppaafTimeCSWarAllowing.setAt(ePlayer, time);
}

int CvPlayer::getPriorityTurn(PlayerTypes eMinor) const
{
	return m_paiPriorityTurn[eMinor];
}

void CvPlayer::setPriorityTurn(PlayerTypes eMinor, int iValue)
{
	m_paiPriorityTurn.setAt(eMinor, iValue);
}

float CvPlayer::getPriorityTime(PlayerTypes eMinor) const
{
	return m_piPriorityTime[eMinor];
}

void CvPlayer::setPriorityTime(PlayerTypes eMinor, float fValue)
{
	m_piPriorityTime.setAt(eMinor, fValue);
}
#endif

#ifdef PENALTY_FOR_DELAYING_POLICIES
bool CvPlayer::IsDelayedPolicy() const
{
	return m_bIsDelayedPolicy;
}

void CvPlayer::setIsDelayedPolicy(bool bValue)
{
	m_bIsDelayedPolicy = bValue;
}
#endif






bool CvPlayer::GetEverPoppedGoody()
{
	return m_bEverPoppedGoody;
}


CvPlot* CvPlayer::GetClosestGoodyPlot(bool bStopAfterFindingFirst)
{
	FFastVector<int> aiGoodyPlots = GetEconomicAI()->GetGoodyHutPlots();

	CvPlot* pResultPlot = NULL;
	int iShortestPath = INT_MAX;


	for(uint uiGoodyIndex = 0; uiGoodyIndex < aiGoodyPlots.size(); uiGoodyIndex++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(aiGoodyPlots[uiGoodyIndex]);
		if(!pPlot || !pPlot->isGoody(getTeam()))
		{
			continue;
		}


		int iUnitLoop;
		CvUnit* pLoopUnit = NULL;


		for(pLoopUnit = firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iUnitLoop))
		{
			if(!pLoopUnit)
			{
				continue;
			}

			if(pPlot->getArea() != pLoopUnit->getArea() && !pLoopUnit->CanEverEmbark())
			{
				continue;
			}

			int iReturnValue = INT_MAX;
			bool bResult = pLoopUnit->GeneratePath(pPlot, MOVE_UNITS_IGNORE_DANGER, true, &iReturnValue);

			if(bResult)
			{
				if(iReturnValue < iShortestPath)
				{
					pResultPlot = pPlot;
				}

				if(bStopAfterFindingFirst)
				{
					return pPlot;
				}
			}
		}
	}

	return pResultPlot;
}


bool CvPlayer::GetPlotHasOrder(CvPlot* pPlot)
{
	if(!pPlot)
	{
		return false;
	}

	int iLoopUnit;
	for(CvUnit* pLoopUnit = firstUnit(&iLoopUnit); pLoopUnit; pLoopUnit = nextUnit(&iLoopUnit))
	{
		CvPlot* pMissionPlot = pLoopUnit->GetMissionAIPlot();
		if(NULL != pMissionPlot && pMissionPlot->getX() == pPlot->getX() && pMissionPlot->getY() == pPlot->getY())
		{
			return true;
		}
	}

	return false;
}


bool CvPlayer::GetAnyUnitHasOrderToGoody()
{
	FFastVector<int> aiGoodyPlots = GetEconomicAI()->GetGoodyHutPlots();


	for(uint uiGoodyIndex = 0; uiGoodyIndex < aiGoodyPlots.size(); uiGoodyIndex++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(aiGoodyPlots[uiGoodyIndex]);
		if(!pPlot)
		{
			continue;
		}

		if(!pPlot->isGoody(getTeam()))
		{
			continue;
		}

		if(GetPlotHasOrder(pPlot))
		{
			return true;
		}
	}

	return false;
}


bool CvPlayer::GetEverTrainedBuilder()
{
	return m_bEverTrainedBuilder;
}


int CvPlayer::GetNumFreeTechs() const
{
	return m_iFreeTechCount;
}


void CvPlayer::SetNumFreeTechs(int iValue)
{
	m_iFreeTechCount = iValue;
	if(GetID() == GC.getGame().getActivePlayer())
	{
		GC.GetEngineUserInterface()->setDirty(ResearchButtons_DIRTY_BIT, true);
	}
}


int CvPlayer::GetMedianTechPercentage() const
{
	return m_iMedianTechPercentage;
}


void CvPlayer::ChangeMedianTechPercentage(int iValue)
{
	m_iMedianTechPercentage += iValue;
}


int CvPlayer::GetNumFreePolicies() const
{
	return m_iNumFreePolicies;
}


void CvPlayer::SetNumFreePolicies(int iValue)
{

	int iDifference = iValue - m_iNumFreePolicies;
	if(iDifference > 0)
	{
		ChangeNumFreePoliciesEver(iDifference);
	}

	m_iNumFreePolicies = iValue;
}


void CvPlayer::ChangeNumFreePolicies(int iChange)
{
	SetNumFreePolicies(GetNumFreePolicies() + iChange);

	if(iChange > 0 && getNumCities() > 0)
	{
		CvNotifications* pNotifications = GetNotifications();
		if(pNotifications)
		{
			CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_FREE_POLICY");
			CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_FREE_POLICY_SUMMARY");
			pNotifications->Add(NOTIFICATION_FREE_POLICY, strBuffer, strSummary, -1, -1, -1);
		}

	}
}


int CvPlayer::GetNumFreeTenets() const
{
	return m_iNumFreeTenets;
}


void CvPlayer::SetNumFreeTenets(int iValue, bool bCountAsFreePolicies)
{

	int iDifference = iValue - m_iNumFreeTenets;
	if (bCountAsFreePolicies && iDifference > 0)
	{
		ChangeNumFreePoliciesEver(iDifference);
	}

	m_iNumFreeTenets = iValue;
}


void CvPlayer::ChangeNumFreeTenets(int iChange, bool bCountAsFreePolicies)
{
	SetNumFreeTenets(GetNumFreeTenets() + iChange, bCountAsFreePolicies);

	if(iChange > 0 && getNumCities() > 0)
	{
		CvNotifications* pNotifications = GetNotifications();
		if(pNotifications)
		{
			CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_FREE_POLICY");
			CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_FREE_POLICY_SUMMARY");
			pNotifications->Add(NOTIFICATION_FREE_POLICY, strBuffer, strSummary, -1, -1, -1);
		}

	}
}


int CvPlayer::GetNumFreeGreatPeople() const
{
	return m_iNumFreeGreatPeople;
}


void CvPlayer::SetNumFreeGreatPeople(int iValue)
{
	m_iNumFreeGreatPeople = iValue;
}


void CvPlayer::ChangeNumFreeGreatPeople(int iChange)
{
	m_iNumFreeGreatPeople = GetNumFreeGreatPeople() + iChange;
	if(iChange > 0)
	{
		if(isHuman())
		{
			CvNotifications* pNotifications = GetNotifications();
			if(pNotifications)
			{
				CvString strBuffer = GetLocalizedText("TXT_KEY_CHOOSE_FREE_GREAT_PERSON");
				CvString strSummary = GetLocalizedText("TXT_KEY_CHOOSE_FREE_GREAT_PERSON_TT");
				pNotifications->Add(NOTIFICATION_FREE_GREAT_PERSON, strSummary.c_str(), strBuffer.c_str(), -1, -1, -1);
			}
		}
		else
		{
			for(int iI = 0; iI < iChange; iI++)
			{
				AI_chooseFreeGreatPerson();
			}
		}
	}
}


int CvPlayer::GetNumMayaBoosts() const
{
	return m_iNumMayaBoosts;
}


void CvPlayer::SetNumMayaBoosts(int iValue)
{
	m_iNumMayaBoosts = iValue;
}


void CvPlayer::ChangeNumMayaBoosts(int iChange)
{
	m_iNumMayaBoosts = GetNumMayaBoosts() + iChange;
	if(iChange > 0)
	{
		if(isHuman())
		{
			CvNotifications* pNotifications = GetNotifications();
			if(pNotifications)
			{
				CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_MAYA_LONG_COUNT");
				CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_MAYA_LONG_COUNT_TT");
				pNotifications->Add(NOTIFICATION_MAYA_LONG_COUNT, strSummary.c_str(), strBuffer.c_str(), -1, -1, -1);
			}
		}
		else
		{
			for(int iI = 0; iI < iChange; iI++)
			{
				GetPlayerTraits()->ChooseMayaBoost();
			}
		}
	}
}


int CvPlayer::GetNumFaithGreatPeople() const
{
	return m_iNumFaithGreatPeople;
}


void CvPlayer::SetNumFaithGreatPeople(int iValue)
{
	m_iNumFaithGreatPeople = iValue;
}


void CvPlayer::ChangeNumFaithGreatPeople(int iChange)
{
	m_iNumFaithGreatPeople = GetNumFaithGreatPeople() + iChange;
	if(iChange > 0)
	{
		if(isHuman())
		{
			CvNotifications* pNotifications = GetNotifications();
			if(pNotifications)
			{
				CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_FAITH_GREAT_PERSON");
				CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_FAITH_GREAT_PERSON_TT");
				pNotifications->Add(NOTIFICATION_FAITH_GREAT_PERSON, strSummary.c_str(), strBuffer.c_str(), -1, -1, -1);
			}
		}
		else
		{
			for(int iI = 0; iI < iChange; iI++)
			{
				AI_chooseFreeGreatPerson();
			}
		}
	}
}

int CvPlayer::GetNumArchaeologyChoices() const
{
	return m_iNumArchaeologyChoices;
}


void CvPlayer::SetNumArchaeologyChoices(int iValue)
{
	m_iNumArchaeologyChoices = iValue;
}


FaithPurchaseTypes CvPlayer::GetFaithPurchaseType() const
{
	return m_eFaithPurchaseType;
}


void CvPlayer::SetFaithPurchaseType(FaithPurchaseTypes eType)
{
	m_eFaithPurchaseType = eType;
}


int CvPlayer::GetFaithPurchaseIndex() const
{
	return m_iFaithPurchaseIndex;
}


void CvPlayer::SetFaithPurchaseIndex(int iIndex)
{
	m_iFaithPurchaseIndex = iIndex;
}


int CvPlayer::GetNumFreePoliciesEver() const
{
	return m_iNumFreePoliciesEver;
}


void CvPlayer::SetNumFreePoliciesEver(int iValue)
{
	m_iNumFreePoliciesEver = iValue;
}


void CvPlayer::ChangeNumFreePoliciesEver(int iChange)
{
	SetNumFreePoliciesEver(GetNumFreePoliciesEver() + iChange);
}


int CvPlayer::GetLastSliceMoved() const
{
	return m_iLastSliceMoved;
}


void CvPlayer::SetLastSliceMoved(int iValue)
{
	m_iLastSliceMoved = iValue;
}


void CvPlayer::SetEverConqueredBy(PlayerTypes ePlayer, bool bValue)
{
	if(bValue)
	{
		m_bfEverConqueredBy.SetBit(ePlayer);
	}
	else
	{
		m_bfEverConqueredBy.ClearBit(ePlayer);
	}
}


bool CvPlayer::IsEverConqueredBy(PlayerTypes ePlayer)
{
	return m_bfEverConqueredBy.GetBit(ePlayer);
}


const FAutoArchive& CvPlayer::getSyncArchive() const
{
	return m_syncArchive;
}


FAutoArchive& CvPlayer::getSyncArchive()
{
	return m_syncArchive;
}


bool CvPlayer::isLocalPlayer() const
{
	return (GC.getGame().getActivePlayer() == GetID());
}


void CvPlayer::disconnected()
{
	bool isMultiplayer = GC.getGame().isGameMultiPlayer();
	if(isMultiplayer && isHuman() && !isLocalPlayer())
	{

			FILogFile* logFile = LOGFILEMGR.GetLog("net_message_debug.log", 0);
			if(logFile)
			{
				logFile->DebugMsg("Attempted to post notification for player disconnect event.  Player(%i)", GetID());
			}

			CvNotifications* pNotifications = GET_PLAYER(GC.getGame().getActivePlayer()).GetNotifications();
			if(pNotifications){
				if(gDLL->IsPlayerKicked(GetID())){
					Localization::String kickedMsg = Localization::Lookup("TXT_KEY_PLAYER_KICKED");
					kickedMsg << getNameKey();
					pNotifications->Add(NOTIFICATION_PLAYER_KICKED, kickedMsg.toUTF8(), kickedMsg.toUTF8(), -1, -1, GetID());
				}
				else{
					Localization::String disconnectString = Localization::Lookup("TXT_KEY_PLAYER_DISCONNECTED");
					disconnectString << getNameKey();

					if(CvPreGame::isPitBoss()){
						disconnectString = Localization::Lookup("TXT_KEY_PLAYER_DISCONNECTED_PITBOSS");
						disconnectString << getNameKey();	
					}

					pNotifications->Add(NOTIFICATION_PLAYER_DISCONNECTED, disconnectString.toUTF8(), disconnectString.toUTF8(), -1, -1, GetID());
				}
			}

#ifdef GAME_AUTOPAUSE_ON_ACTIVE_DISCONNECT_IF_NOT_SEQUENTIAL
			if (!isObserver())
			{
				if (!CvPreGame::isPitBoss() || gDLL->IsPlayerKicked(GetID()))
				{
					setIsDisconnected(false);
					bool isAnyDisconnected = false;
					for (int iI = 0; iI < MAX_PLAYERS; iI++)
					{
						PlayerTypes eLoopPlayer = (PlayerTypes)iI;
						if (GET_PLAYER(eLoopPlayer).isDisconnected())
						{
							isAnyDisconnected = true;
						}
				}
#ifdef TURN_TIMER_PAUSE_BUTTON
					{
						if (!isAnyDisconnected && GC.getGame().isOption(GAMEOPTION_END_TURN_TIMER_ENABLED) && !GC.getGame().isPaused() && GC.getGame().getGameState() == GAMESTATE_ON)
						{
							if ((GC.getGame().getElapsedGameTurns() > 0) && GET_PLAYER(GC.getGame().getActivePlayer()).isTurnActive())
							{



								gDLL->sendGiftUnit(NO_PLAYER, -11);
							}
						}
					}
#endif




			CvPreGame::setSlotStatus(GetID(), SS_COMPUTER);
			CvPreGame::VerifyHandicap(GetID());


					gDLL->NotifySpecificAILeaderInGame(GetID());

					if (!GC.getGame().isOption(GAMEOPTION_DYNAMIC_TURNS) && GC.getGame().isOption(GAMEOPTION_SIMULTANEOUS_TURNS))
					{
						checkRunAutoMovesForEveryone();
					}
#ifdef DO_CANCEL_DEALS_WITH_AI
					if (!isHuman() && GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_DEALS"))
					{
						GC.getGame().GetGameTrade()->ClearAllCivTradeRoutes(GetID());
						for (int iLoopTeam = 0; iLoopTeam < MAX_CIV_TEAMS; iLoopTeam++)
						{
							TeamTypes eTeam = (TeamTypes)iLoopTeam;
							if (getTeam() != eTeam && GET_TEAM(eTeam).isAlive() && GET_TEAM(eTeam).isHuman())
							{
								GC.getGame().GetGameDeals()->DoCancelDealsBetweenTeams(GET_PLAYER(GetID()).getTeam(), (TeamTypes)iLoopTeam);
								GET_TEAM(getTeam()).CloseEmbassyAtTeam(eTeam);
								GET_TEAM(eTeam).CloseEmbassyAtTeam(getTeam());
								GET_TEAM(getTeam()).CancelResearchAgreement(eTeam);
								GET_TEAM(eTeam).CancelResearchAgreement(getTeam());
								GET_TEAM(getTeam()).EvacuateDiplomatsAtTeam(eTeam);
								GET_TEAM(eTeam).EvacuateDiplomatsAtTeam(getTeam());


								GC.getMap().verifyUnitValidPlot();
							}
						}
					}
#endif
#ifdef CHANGE_HOST_IF_DISCONNECTED
					CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
					if (pLeague != NULL)
					{

						if (pLeague->IsHostMember(GetID()))
						{
							pLeague->AssignNewHost();
						}
					}
#endif
			}
#else
			if (!isObserver() && (!CvPreGame::isPitBoss() || gDLL->IsPlayerKicked(GetID())))
			{




				CvPreGame::setSlotStatus(GetID(), SS_COMPUTER);
				CvPreGame::VerifyHandicap(GetID());


				gDLL->NotifySpecificAILeaderInGame(GetID());

				if (!GC.getGame().isOption(GAMEOPTION_DYNAMIC_TURNS) && GC.getGame().isOption(GAMEOPTION_SIMULTANEOUS_TURNS))
				{
					checkRunAutoMovesForEveryone();
				}
#ifdef DO_CANCEL_DEALS_WITH_AI
				if (!isHuman() && GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_DEALS"))
				{
					GC.getGame().GetGameTrade()->ClearAllCivTradeRoutes(GetID());
					for (int iLoopTeam = 0; iLoopTeam < MAX_CIV_TEAMS; iLoopTeam++)
					{
						TeamTypes eTeam = (TeamTypes)iLoopTeam;
						if (getTeam() != eTeam && GET_TEAM(eTeam).isAlive() && GET_TEAM(eTeam).isHuman())
						{
							GC.getGame().GetGameDeals()->DoCancelDealsBetweenTeams(GET_PLAYER(GetID()).getTeam(), (TeamTypes)iLoopTeam);
							GET_TEAM(getTeam()).CloseEmbassyAtTeam(eTeam);
							GET_TEAM(eTeam).CloseEmbassyAtTeam(getTeam());
							GET_TEAM(getTeam()).CancelResearchAgreement(eTeam);
							GET_TEAM(eTeam).CancelResearchAgreement(getTeam());
							GET_TEAM(getTeam()).EvacuateDiplomatsAtTeam(eTeam);
							GET_TEAM(eTeam).EvacuateDiplomatsAtTeam(getTeam());


							GC.getMap().verifyUnitValidPlot();
						}
					}
				}
#endif
#ifdef CHANGE_HOST_IF_DISCONNECTED
				CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
				if (pLeague != NULL)
				{

					if (pLeague->IsHostMember(GetID()))
					{
						pLeague->AssignNewHost();
					}
				}
#endif
			}
#endif
#ifdef GAME_AUTOPAUSE_ON_ACTIVE_DISCONNECT_IF_NOT_SEQUENTIAL
		else if (                                                                       true && isAlive() && isTurnActive() && (GC.getGame().isOption(GAMEOPTION_DYNAMIC_TURNS) || GC.getGame().isOption(GAMEOPTION_SIMULTANEOUS_TURNS)) && !gDLL->IsPlayerKicked(GetID()))
		{
			setIsDisconnected(true);
#ifdef TURN_TIMER_PAUSE_BUTTON
			{
				if (GC.getGame().isOption(GAMEOPTION_END_TURN_TIMER_ENABLED) && !GC.getGame().isPaused() && GC.getGame().getGameState() == GAMESTATE_ON)
				{
					if ((GC.getGame().getElapsedGameTurns() > 0) && GET_PLAYER(GC.getGame().getActivePlayer()).isTurnActive())
					{



						gDLL->sendGiftUnit(NO_PLAYER, -10);
					}
				}
			}
#endif
		}
		}
#endif
	}
}

void CvPlayer::reconnected()
{

	if(CvPreGame::slotStatus(GetID()) != SS_OBSERVER){
		CvPreGame::setSlotStatus(GetID(), SS_TAKEN);
	}

	CvPreGame::VerifyHandicap(GetID());

	CvGame& kGame = GC.getGame();
	bool isMultiplayer = kGame.isGameMultiPlayer();
	if(isMultiplayer && !isLocalPlayer())
	{
		FAutoArchive& archive = getSyncArchive();
		archive.clearDelta();

		Localization::String connectString = Localization::Lookup("TXT_KEY_PLAYER_CONNECTING");
		connectString << getNameKey();

		CvNotifications* pNotifications = GET_PLAYER(kGame.getActivePlayer()).GetNotifications();
		if(pNotifications)
		{
			pNotifications->Add(NOTIFICATION_PLAYER_CONNECTING, connectString.toUTF8(), connectString.toUTF8(), -1, -1, GetID());
		}
#ifdef GAME_AUTOPAUSE_ON_ACTIVE_DISCONNECT_IF_NOT_SEQUENTIAL
		setIsDisconnected(false);
		bool isAnyDisconnected = false;
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			PlayerTypes eLoopPlayer = (PlayerTypes)iI;
			if (GET_PLAYER(eLoopPlayer).isDisconnected())
			{
				isAnyDisconnected = true;
			}
		}
#ifdef TURN_TIMER_PAUSE_BUTTON
		{
			if (!isAnyDisconnected && GC.getGame().isOption(GAMEOPTION_END_TURN_TIMER_ENABLED) && !GC.getGame().isPaused() && GC.getGame().getGameState() == GAMESTATE_ON)
			{
				if ((GC.getGame().getElapsedGameTurns() > 0) && GET_PLAYER(GC.getGame().getActivePlayer()).isTurnActive())
				{



					gDLL->sendGiftUnit(NO_PLAYER, -11);
				}
			}
		}
#endif

#endif
	}
#ifdef MP_PLAYERS_VOTING_SYSTEM
	if (isLocalPlayer())
	{
		GC.getGame().GetMPVotingSystem()->Init();
	}
#endif
}

bool CvPlayer::hasBusyUnitUpdatesRemaining() const
{
	return m_endTurnBusyUnitUpdatesLeft > 0;
}


void CvPlayer::setBusyUnitUpdatesRemaining(int iUpdateCount)
{
	m_endTurnBusyUnitUpdatesLeft = iUpdateCount;
}


const char* const CvPlayer::getNickName() const
{
	return CvPreGame::nicknameDisplayed(GetID()).c_str();
}


bool CvPlayer::hasUnitsThatNeedAIUpdate() const
{
	const CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if(!pLoopUnit->TurnProcessed() &&
		        (pLoopUnit->IsAutomated() &&
		         pLoopUnit->AI_getUnitAIType() != UNITAI_UNKNOWN &&
		         pLoopUnit->canMove()))
		{
			return true;
		}
	}

	return false;
}


std::string CvPlayer::debugDump(const FAutoVariableBase&) const
{
	std::string result = "Game Turn : ";
	char gameTurnBuffer[8] = {0};
	int gameTurn = GC.getGame().getGameTurn();
	sprintf_s(gameTurnBuffer, "%d\0", gameTurn);
	result += gameTurnBuffer;
	return result;
}


std::string CvPlayer::stackTraceRemark(const FAutoVariableBase& var) const
{
	std::string result = debugDump(var);
	if(&var == &m_aOptions)
	{
		result += "\nPlayer Options:";
		for (PlayerOptionsVector::const_iterator itr = m_aOptions.begin(); itr != m_aOptions.end(); ++itr )
		{
			CvString curOptionsStr;
			curOptionsStr.Format("\n%u, %d", itr->first, itr->second);
			result += curOptionsStr;
		}
		result += "\n";
	}
	return result;
}


bool CvPlayer::hasTurnTimerExpired()
{
	return GC.getGame().hasTurnTimerExpired(GetID());
}


void CvPlayer::doArmySize()
{
	int numUnits = 0;
	int32 nLargestArmy = 0;
	int iI;

	for(iI = 0; iI < NUM_UNITAI_TYPES; iI++)
	{
		if((UnitAITypes)iI == UNITAI_ARTIST ||(UnitAITypes)iI == UNITAI_ENGINEER || (UnitAITypes)iI == UNITAI_UNKNOWN ||
		        (UnitAITypes)iI == UNITAI_GENERAL || (UnitAITypes)iI == UNITAI_SETTLE || (UnitAITypes)iI == UNITAI_WORKER ||
		        (UnitAITypes)iI == UNITAI_SCIENTIST || (UnitAITypes)iI == UNITAI_MERCHANT || (UnitAITypes)iI == UNITAI_WORKER_SEA ||
		        (UnitAITypes)iI == UNITAI_SPACESHIP_PART || (UnitAITypes)iI == UNITAI_TREASURE || (UnitAITypes)iI == UNITAI_PROPHET ||
		        (UnitAITypes)iI == UNITAI_MISSIONARY || (UnitAITypes)iI == UNITAI_INQUISITOR || (UnitAITypes)iI == UNITAI_ADMIRAL ||
				(UnitAITypes)iI == UNITAI_WRITER || (UnitAITypes)iI == UNITAI_MUSICIAN)
		{
			continue;
		}
		else
		{
			numUnits += GetNumUnitsWithUnitAI((UnitAITypes)iI, false, true);
		}
	}
	gDLL->GetSteamStat(ESTEAMSTAT_STANDINGARMY, &nLargestArmy);

	if(nLargestArmy < numUnits)
	{
		gDLL->SetSteamStat(ESTEAMSTAT_STANDINGARMY, numUnits);

		CvAchievementUnlocker::Check_PSG();
	}
}


void CvPlayer::checkInitialTurnAIProcessed()
{
	int turn = GC.getGame().getGameTurn();
	if(m_lastGameTurnInitialAIProcessed != turn)
	{



		if(!isTurnActive() || !hasUnitsThatNeedAIUpdate())
		{
			m_lastGameTurnInitialAIProcessed = turn;
			if(GC.getGame().getActivePlayer() == GetID())
				gDLL->sendPlayerInitialAIProcessed();
		}
	}
}


void CvPlayer::GatherPerTurnReplayStats(int iGameTurn)
{
	AI_PERF_FORMAT("AI-perf.csv", ("CvPlayer::GatherPerTurnReplayStats, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), getCivilizationShortDescription()) );
#if !defined(FINAL_RELEASE)
	cvStopWatch watch("Replay Stat Recording");
#endif
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "GatherPerTurnReplayStats", args.get(), bResult);
	}


#ifdef ENHANCED_GRAPHS
	if ((GC.getGame().isNetworkMultiPlayer() && isHuman()) || (!GC.getGame().isNetworkMultiPlayer() && isAlive()) && !isMinorCiv())
#else
	if (isAlive())
#endif
	{

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_PRODUCTIONPERTURN"), iGameTurn, calculateTotalYield(YIELD_PRODUCTION));

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_TOTALGOLD"), iGameTurn, GetTreasury()->GetGold());

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_GOLDPERTURN"), iGameTurn, calculateTotalYield(YIELD_GOLD));

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_CITYCOUNT"), iGameTurn, getNumCities());


		CvTeam& team = GET_TEAM(getTeam());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_TECHSKNOWN"), iGameTurn, team.GetTeamTechs()->GetNumTechsKnown());


		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_SCIENCEPERTURN"), iGameTurn, calculateTotalYield(YIELD_SCIENCE));




		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_TOTALCULTURE"), iGameTurn, getJONSCulture());


		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_CULTUREPERTURN"), iGameTurn, GetTotalJONSCulturePerTurn());


		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_EXCESSHAPINESS"), iGameTurn, GetExcessHappiness());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_HAPPINESS"), iGameTurn, GetHappiness());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_UNHAPPINESS"), iGameTurn, GetUnhappiness());


		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_GOLDENAGETURNS"), iGameTurn, getGoldenAgeTurns());


		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_POPULATION"), iGameTurn, getTotalPopulation());


		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_FOODPERTURN"), iGameTurn, calculateTotalYield(YIELD_FOOD));


		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_TOTALLAND"), iGameTurn, getTotalLand());

		CvTreasury* pkTreasury = GetTreasury();
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_GPTCITYCONNECTIONS"), iGameTurn, pkTreasury->GetCityConnectionGold());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_GPTINTERNATIONALTRADE"), iGameTurn, pkTreasury->GetGoldPerTurnFromTradeRoutes());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_GPTDEALS"), iGameTurn, pkTreasury->GetGoldPerTurnFromDiplomacy());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_UNITMAINTENANCE"), iGameTurn, pkTreasury->GetExpensePerTurnUnitMaintenance());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_BUILDINGMAINTENANCE"), iGameTurn, pkTreasury->GetBuildingGoldMaintenance());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_IMPROVEMENTMAINTENANCE"), iGameTurn, pkTreasury->GetImprovementGoldMaintenance());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMBEROFPOLICIES"), iGameTurn, GetPlayerPolicies()->GetNumPoliciesOwned());


		int iWorkerCount = 0;
		CvUnit* pLoopUnit;
		int iLoopUnit;
		for(pLoopUnit = firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoopUnit))
		{
			if(pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER)
			{
				iWorkerCount++;
			}
		}
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMBEROFWORKERS"), iGameTurn, iWorkerCount);



		CvPlotsVector& aiPlots = GetPlots();


		int iWorkedTiles = 0;
		int iImprovedTiles = 0;
		for(uint uiPlotIndex = 0; uiPlotIndex < aiPlots.size(); uiPlotIndex++)
		{

			if(aiPlots[uiPlotIndex] == -1)
			{
				break;
			}

			CvPlot* pPlot = GC.getMap().plotByIndex(aiPlots[uiPlotIndex]);
			if(!pPlot)
			{
				continue;
			}


			if(pPlot->getPlotCity())
			{
				continue;
			}

			if(pPlot->isBeingWorked())
			{
				iWorkedTiles++;
			}

			if(pPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				iImprovedTiles++;
			}
		}

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_IMPROVEDTILES"), iGameTurn, iImprovedTiles);
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_WORKEDTILES"), iGameTurn, iWorkedTiles);
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMBEROFWORKERS"), iGameTurn, iWorkerCount);


		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_MILITARYMIGHT"), iGameTurn, GetMilitaryMight());

#ifdef ENHANCED_GRAPHS
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_FAITHPERTURN"), iGameTurn, GetTotalFaithPerTurn());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_TOTALFAITH"), iGameTurn, GetFaith());

		ReligionTypes eReligion = GetReligions()->GetReligionCreatedByPlayer();
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, GetID());
		bool bIsFaithPurchaseAllGreatPeople = false;
		if (pReligion)
		{
			if (pReligion->m_Beliefs.IsFaithPurchaseAllGreatPeople())
			{
				bIsFaithPurchaseAllGreatPeople = true;
			}
		}
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMOFBORNSCIENTISTS"), iGameTurn, getGreatScientistsCreated() - GetMayaBoostScientist());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMOFBOUGHTSCIENTISTS"), iGameTurn, getScientistsFromFaith());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_TOTALNUMOFSCIENTISTS"), iGameTurn, GetNumScientistsTotal());

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMOFBORNENGINEERS"), iGameTurn, getGreatEngineersCreated() - GetMayaBoostEngineers());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMOFBOUGHTENGINEERS"), iGameTurn, getEngineersFromFaith());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_TOTALNUMOFENGINEERS"), iGameTurn, GetNumEngineersTotal());

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMOFBORNMERCHANTS"), iGameTurn, getGreatMerchantsCreated() - GetMayaBoostMerchants());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMOFBOUGHTMERCHANTS"), iGameTurn, getMerchantsFromFaith());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_TOTALNUMOFMERCHANTS"), iGameTurn, GetNumMerchantsTotal());

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMOFBORNWRITERS"), iGameTurn, getGreatWritersCreated() - GetMayaBoostWriters());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMOFBOUGHTWRITERS"), iGameTurn, getWritersFromFaith());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_TOTALNUMOFWRITERS"), iGameTurn, GetNumWritersTotal());

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMOFBORNARTISTS"), iGameTurn, getGreatArtistsCreated() - GetMayaBoostArtists());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMOFBOUGHTARTISTS"), iGameTurn, getArtistsFromFaith());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_TOTALNUMOFARTISTS"), iGameTurn, GetNumAristsTotal());

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMOFBORNMUSICIANS"), iGameTurn, getGreatMusiciansCreated() - GetMayaBoostMusicians());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMOFBOUGHTMUSICIANS"), iGameTurn, getMusiciansFromFaith());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_TOTALNUMOFMUSICIANS"), iGameTurn, GetNumMusiciansTotal());

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMOFBORNGENERALS"), iGameTurn, getGreatGeneralsCreated());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMOFBOUGHTGENERALS"), iGameTurn, getGeneralsFromFaith());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_TOTALNUMOFGENERALS"), iGameTurn, GetNumGeneralsTotal());

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMOFBORNADMIRALS"), iGameTurn, getGreatAdmiralsCreated());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMOFBOUGHTADMIRALS"), iGameTurn, getAdmiralsFromFaith());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_TOTALNUMOFADMIRALS"), iGameTurn, GetNumAdmiralsTotal());

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMOFBOUGHTPROPHETS"), iGameTurn, GetReligions()->GetNumProphetsSpawned());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_TOTALNUMOFPROPHETS"), iGameTurn, GetNumProphetsTotal());

		int iBullyGold = 0;
		int iBullyWorkers = 0;
		for (int iI = MAX_MAJOR_CIVS; iI < MAX_CIV_PLAYERS; iI++)
		{
			iBullyGold += GET_PLAYER((PlayerTypes)iI).GetMinorCivAI()->GetBullyGoldAmountTotalByPlayer(GetID());
			iBullyWorkers += GET_PLAYER((PlayerTypes)iI).GetMinorCivAI()->GetBullyWorkersAmountTotalByPlayer(GetID());
		}
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_GOLDFROMBULLING"), iGameTurn, iBullyGold);
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_WORKERSFROMBULLING"), iGameTurn, iBullyWorkers);

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMTRAINEDUNITS"), iGameTurn, GetNumTrainedUnits());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMLOSTUNITS"), iGameTurn, GetNumLostUnits());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMKILLEDUNITS"), iGameTurn, GetNumKilledUnits());

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMBUILTWONDERS"), iGameTurn, GetNumWonders());


		int iRevealedTiles = 0;
		CvPlot* pLoopPlot;
		for (int iLoopPlot = 0; iLoopPlot < GC.getMap().numPlots(); iLoopPlot++)
		{
			pLoopPlot = GC.getMap().plotByIndexUnchecked(iLoopPlot);
			if (pLoopPlot && pLoopPlot->isRevealed(getTeam()))
				iRevealedTiles++;
		}
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMREVEALEDTILES"), iGameTurn, iRevealedTiles);

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMSTOLENSCIENCE"), iGameTurn, GetNumStolenScience());

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_DAMAGEDEALTTOUNITS"), iGameTurn, GetUnitsDamageDealt());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_DAMAGEDEALTTOCITIES"), iGameTurn, GetCitiesDamageDealt());

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_DAMAGETAKENBYUNITS"), iGameTurn, GetUnitsDamageTaken());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_DAMAGETAKENBYCITIES"), iGameTurn, GetCitiesDamageTaken());

		CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
		int iNumDelegates;
		if (pLeague)
		{
			iNumDelegates = pLeague->CalculateStartingVotesForMember(GetID());
		}
		else
		{
			iNumDelegates = 0;
		}
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMDELEGATES"), iGameTurn, iNumDelegates);

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_TOTALCHOPS"), iGameTurn, GetNumChops());

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_LOSTHAMMERSFROMLOSTWONDERS"), iGameTurn, GetProductionGoldFromWonders());

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMTIMESOPENEDDEMOGRAPHICS"), iGameTurn, GetNumTimesOpenedDemographics());

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_UNITSDAMAGEHEALED"), iGameTurn, GetUnitsDamageHealed());

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_TURNSSTAGNATED"), iGameTurn, GetTurnsStagnated());

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_GOLDSPENTBUYS"), iGameTurn, GetGoldSpentBuys());

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_GOLDSPENTUPGRADES"), iGameTurn, GetGoldSpentUpgrades());

#endif
	}
}






bool CancelActivePlayerEndTurn()
{
	CvPlayer& kActivePlayer = GET_PLAYER(GC.getGame().getActivePlayer());

	if(kActivePlayer.isLocalPlayer() && kActivePlayer.isEndTurn())
	{
		if (gDLL->sendTurnUnready())
		{
			kActivePlayer.setEndTurn(false);
			return true;
		}
		return false;
	}
	return true;
}
