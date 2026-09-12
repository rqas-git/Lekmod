






#include "CvGameCoreDLLPCH.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvInternalGameCoreUtils.h"
#include "FStlContainerSerialization.h"
#include "CvEnumSerialization.h"
#include "CvDLLUtilDefines.h"
#include "CvDllCity.h"
#include "CvDllPlot.h"
#include "CvInfosSerializationHelper.h"


#include "LintFree.h"


CvBuildingEntry::CvBuildingEntry(void):

	m_iBuildingClassType(NO_BUILDINGCLASS),
	m_pkBuildingClassInfo(NULL),
	m_iNearbyTerrainRequired(NO_VICTORY),
	m_iProhibitedCityTerrain(NO_VICTORY),
	m_iVictoryPrereq(NO_VICTORY),
	m_iFreeStartEra(NO_ERA),
	m_iMaxStartEra(NO_ERA),
	m_iObsoleteTech(NO_TECH),
	m_iEnhancedYieldTech(NO_TECH),
	m_iTechEnhancedTourism(0),
	m_iGoldMaintenance(0),
	m_iMutuallyExclusiveGroup(0),
	m_iReplacementBuildingClass(NO_BUILDINGCLASS),
	m_iPrereqAndTech(NO_TECH),
	m_iPolicyBranchType(NO_POLICY_BRANCH_TYPE),
	m_iSpecialistType(NO_SPECIALIST),
	m_iSpecialistCount(0),
	m_iSpecialistExtraCulture(0),
	m_iGreatPeopleRateChange(0),
	m_eGreatWorkSlotType(NO_GREAT_WORK_SLOT),
	m_iGreatWorkCount(0),
	m_eFreeGreatWork(NO_GREAT_WORK),
#ifdef LEKMOD_v34
	m_iFreeGreatWorkCount(0),
#endif
	m_iFreeBuildingClass(NO_BUILDINGCLASS),
	m_iFreeBuildingThisCity(NO_BUILDINGCLASS),
	m_iFreePromotion(NO_PROMOTION),
	m_iTrainedFreePromotion(NO_PROMOTION),
	m_iFreePromotionRemoved(NO_PROMOTION),
	m_iProductionCost(0),
#ifdef LEKMOD_BUILDING_GOLD_COST
	m_iGoldCost(0),
	m_bRequiresGoldPurchase(false),
#endif
	m_iFaithCost(0),
	m_iLeagueCost(0),
	m_iNumCityCostMod(0),
	m_iHurryCostModifier(0),
	m_iNumCitiesPrereq(0),
	m_iUnitLevelPrereq(0),
	m_iCultureRateModifier(0),
	m_iGlobalCultureRateModifier(0),
	m_iGreatPeopleRateModifier(0),
	m_iGlobalGreatPeopleRateModifier(0),
	m_iGreatGeneralRateModifier(0),
	m_iGreatPersonExpendGold(0),
	m_iUnitUpgradeCostMod(0),
	m_iGoldenAgeModifier(0),
	m_iFreeExperience(0),
	m_iGlobalFreeExperience(0),
	m_iFoodKept(0),
#if defined(LEKMOD_BUILDING_EXCESS_GROWTH)
	m_iExcessGrowth(0),
#endif
	m_bAirlift(false),
	m_iAirModifier(0),
	m_iNukeModifier(0),
	m_iNukeExplosionRand(0),
	m_iWorkerSpeedModifier(0),
	m_iMilitaryProductionModifier(0),
#if defined(LEKMOD_BUILDING_MILITARY_PRODUCTION_MOD)
	m_iMilitaryProductionMod(0),
#endif
	m_iSpaceProductionModifier(0),
	m_iGlobalSpaceProductionModifier(0),
	m_iMinAreaSize(0),
	m_iConquestProbability(0),
	m_iHealRateChange(0),
	m_iHappiness(0),
	m_iUnmoddedHappiness(0),
	m_iUnhappinessModifier(0),
	m_iHappinessPerCity(0),
	m_iHappinessPerXPolicies(0),
	m_iCityCountUnhappinessMod(0),
	m_bNoOccupiedUnhappiness(false),
	m_iGlobalPopulationChange(0),
#ifdef NQ_LOCAL_POPULATION_CHANGE_FROM_BUILDING
	m_iLocalPopulationChange(0),
#endif
	m_iTechShare(0),
	m_iFreeTechs(0),
	m_iFreePolicies(0),
	m_iFreeFlatFaith(0),
	m_iFreeGreatPeople(0),
	m_iMedianTechPercentChange(0),
	m_iGold(0),
	m_bNearbyMountainRequired(false),
	m_bAllowsRangeStrike(false),
	m_iDefenseModifier(0),
#ifdef NQ_BUILDING_DEFENSE_FROM_CITIZENS
	m_iDefensePerCitizen(0),
#endif
	m_iGlobalDefenseModifier(0),
	m_iExtraCityHitPoints(0),
	m_iMissionType(NO_MISSION),
	m_iMinorFriendshipChange(0),
	m_iVictoryPoints(0),
	m_iExtraMissionarySpreads(0),
	m_iReligiousPressureModifier(0),
	m_iEspionageModifier(0),
	m_iGlobalEspionageModifier(0),
	m_iExtraSpies(0),
	m_iSpyRankChange(0),
	m_iTradeRouteRecipientBonus(0),
	m_iTradeRouteTargetBonus(0),
	m_iNumTradeRouteBonus(0),
	m_iTradeRouteSeaDistanceModifier(0),
	m_iTradeRouteSeaGoldBonus(0),
	m_iTradeRouteLandDistanceModifier(0),
	m_iTradeRouteLandGoldBonus(0),
	m_iCityStateTradeRouteProductionModifier(0),
	m_iCityStateTradeRouteGoldModifier(0),
	m_iInstantSpyRankChange(0),
	m_iLandmarksTourismPercent(0),
	m_iInstantMilitaryIncrease(0),
	m_iGreatWorksTourismModifier(0),
	m_iXBuiltTriggersIdeologyChoice(0),
	m_iGreatScientistBeakerModifier(0),
	m_iExtraLeagueVotes(0),
#if defined(LEKMOD_GARRISON_YIELD_EFFECTS)
	m_iGarrisonStrengthBonus(0),
	m_bGarrisonMaintenanceFree(false),
#endif
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	m_iGreatWorkHappiness(0),
#endif
	m_iPreferredDisplayPosition(0),
	m_iPortraitIndex(-1),
	m_bTeamShare(false),
	m_bWater(false),
	m_bRiver(false),
	m_bFreshWater(false),
#if defined(LEKMOD_BUILDING_LAKE_REQ)
	m_bLake(false),
#endif
	m_bMountain(false),
	m_bHill(false),
	m_bFlat(false),
	m_bFoundsReligion(false),
	m_bIsReligious(false),
	m_bBorderObstacle(false),
	m_bPlayerBorderObstacle(false),
	m_bCapital(false),
	m_bGoldenAge(false),
	m_bGrantsFreeCulturalGreatPersonWithTrait(false),
	m_bMapCentering(false),
	m_bNeverCapture(false),
	m_bNukeImmune(false),
	m_bExtraLuxuries(false),
	m_bDiplomaticVoting(false),
	m_bAllowsWaterRoutes(false),
	m_bCityWall(false),
	m_bUnlockedByBelief(false),
	m_bUnlockedByLeague(false),
	m_bRequiresHolyCity(false),
	m_bAffectSpiesNow(false),
	m_bEspionage(false),
#ifdef NQ_MALI_TREASURY
	m_bMalianTreasury(false),
#endif
	m_bAllowsFoodTradeRoutes(false),
	m_bAllowsProductionTradeRoutes(false),
	m_bNullifyInfluenceModifier(false),
#if defined(LEKMOD_BUILDING_FIRST_PURCHASE_DISCOUNT)
	m_iFirstPurchaseDiscount(0),
#endif
	m_piLockedBuildingClasses(NULL),
	m_piPrereqAndTechs(NULL),
	m_piResourceQuantityRequirements(NULL),
	m_piResourceQuantity(NULL),
#if !defined(LEKMOD_FIX_BUILDING_RESOURCE_YIELD_CHANGE)
	m_piResourceCultureChanges(NULL),
	m_piResourceFaithChanges(NULL),
#else
	m_paiBuildingLocalResourceYieldChanges(NULL),
#endif
	m_piProductionTraits(NULL),
	m_piSeaPlotYieldChange(NULL),
	m_piRiverPlotYieldChange(NULL),
	m_piLakePlotYieldChange(NULL),
	m_piSeaResourceYieldChange(NULL),
	m_piYieldChange(NULL),
	m_piYieldChangePerPop(NULL),
	m_piYieldChangePerReligion(NULL),
	m_piYieldModifier(NULL),
	m_piAreaYieldModifier(NULL),
	m_piGlobalYieldModifier(NULL),
	m_piTechEnhancedYieldChange(NULL),
#if defined(LEKMOD_ERA_ENHANCED_YIELDS)
	m_ppiEraEnhancedYieldChange(NULL),
#endif
	m_piUnitCombatFreeExperience(NULL),
	m_piUnitCombatProductionModifiers(NULL),
#ifdef LEKMOD_v34
	m_piUnitCombatProductionCostModifiers(NULL),
	m_piUnitCombatExtraProduction(NULL),
#endif
	m_piDomainFreeExperience(NULL),
	m_piDomainFreeExperiencePerGreatWork(NULL),
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	m_iGreatWorkMilitaryProductionModifier(0),
	m_piBuildingGreatWorkYieldChange(NULL),
	m_piCityGreatWorkYieldChange(NULL),
#endif
	m_piDomainProductionModifier(NULL),
	m_piPrereqNumOfBuildingClass(NULL),
	m_piFlavorValue(NULL),
	m_piLocalResourceAnds(NULL),
	m_piLocalResourceOrs(NULL),
	m_paiHurryModifier(NULL),
	m_pbBuildingClassNeededInCity(NULL),
	m_piNumFreeUnits(NULL),
	m_bArtInfoEraVariation(false),
	m_bArtInfoCulturalVariation(false),
	m_bArtInfoRandomVariation(false),
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	m_ppaiResourceYieldChange(std::pair<int**, size_t>(NULL, 0)),
	m_ppaiFeatureYieldChange(std::pair<int**, size_t>(NULL, 0)),
	m_ppaiSpecialistYieldChange(std::pair<int**, size_t>(NULL, 0)),
	m_ppaiResourceYieldModifier(std::pair<int**, size_t>(NULL, 0)),
	m_ppaiTerrainYieldChange(std::pair<int**, size_t>(NULL, 0)),
	m_ppiBuildingClassYieldChanges(std::pair<int**, size_t>(NULL, 0)),
#else
	m_ppaiResourceYieldChange(NULL),
#if defined(TRADE_REFACTOR)
	m_ppaiTradeConnectionOriginLandYieldChange(NULL),
	m_ppaiTradeConnectionOriginSeaYieldChange(NULL),
	m_ppaiTradeConnectionDestinationLandYieldChange(NULL),
	m_ppaiTradeConnectionDestinationSeaYieldChange(NULL),
	m_ppaiIncomingTradeConnectionLandYieldChange(NULL),
	m_ppaiIncomingTradeConnectionSeaYieldChange(NULL),
#endif
	#if defined(MISC_CHANGES)
	m_ppaiResourceClassYieldChange(NULL),
#endif
#if defined(LEKMOD_AREA_BASED_CITY_YIELD)
	m_piSameLandMassYieldChange(NULL),
	m_piDifferentLandMassYieldChange(NULL),
#endif
#if defined(LEKMOD_v34)
	m_piGarrisonYieldChange(NULL),
#endif
	m_ppaiFeatureYieldChange(NULL),
	m_ppiResourceYieldChangeGlobal(),
	m_ppaiImprovementYieldChange(NULL),
	m_ppaiImprovementYieldChangeGlobal(NULL),
	m_ppaiSpecialistYieldChange(NULL),
	m_ppaiResourceYieldModifier(NULL),
	m_ppaiTerrainYieldChange(NULL),
	m_ppiBuildingClassYieldChanges(NULL),
#endif
#ifdef LEKMOD_BUILDING_GP_EXPEND_YIELD
	m_piGreatPersonExpendYield(NULL),
#endif
	m_paiBuildingClassHappiness(NULL),
	m_paThemingBonusInfo(NULL),
#ifdef AUI_WARNING_FIXES
	m_iBuildingProductionModifier(0),
	m_iWonderProductionModifier(0),
	m_iCityConnectionTradeRouteModifier(0),
	m_iCapturePlunderModifier(0),
	m_iPolicyCostModifier(0),
	m_iGlobalPlotCultureCostModifier(0),
	m_iPlotCultureCostModifier(0),
	m_iGlobalPlotBuyCostModifier(0),
	m_iPlotBuyCostModifier(0),
#endif
	m_iNumThemingBonuses(0)
{
}


CvBuildingEntry::~CvBuildingEntry(void)
{
	SAFE_DELETE_ARRAY(m_piLockedBuildingClasses);
	SAFE_DELETE_ARRAY(m_piPrereqAndTechs);
	SAFE_DELETE_ARRAY(m_piResourceQuantityRequirements);
	SAFE_DELETE_ARRAY(m_piResourceQuantity);
#if !defined(LEKMOD_FIX_BUILDING_RESOURCE_YIELD_CHANGE)
	SAFE_DELETE_ARRAY(m_piResourceCultureChanges);
	SAFE_DELETE_ARRAY(m_piResourceFaithChanges);
#else
	CvDatabaseUtility::SafeDelete2DArray(m_paiBuildingLocalResourceYieldChanges);
#endif
	SAFE_DELETE_ARRAY(m_piProductionTraits);
	SAFE_DELETE_ARRAY(m_piSeaPlotYieldChange);
	SAFE_DELETE_ARRAY(m_piRiverPlotYieldChange);
	SAFE_DELETE_ARRAY(m_piLakePlotYieldChange);
	SAFE_DELETE_ARRAY(m_piSeaResourceYieldChange);
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piYieldChangePerPop);
	SAFE_DELETE_ARRAY(m_piYieldChangePerReligion);
	SAFE_DELETE_ARRAY(m_piYieldModifier);
	SAFE_DELETE_ARRAY(m_piAreaYieldModifier);
	SAFE_DELETE_ARRAY(m_piGlobalYieldModifier);
	SAFE_DELETE_ARRAY(m_piTechEnhancedYieldChange);
#if defined(LEKMOD_ERA_ENHANCED_YIELDS)
	CvDatabaseUtility::SafeDelete2DArray(m_ppiEraEnhancedYieldChange);
#endif
	SAFE_DELETE_ARRAY(m_piUnitCombatFreeExperience);
	SAFE_DELETE_ARRAY(m_piUnitCombatProductionModifiers);
#ifdef LEKMOD_v34
	SAFE_DELETE_ARRAY(m_piUnitCombatProductionCostModifiers);
	SAFE_DELETE_ARRAY(m_piUnitCombatExtraProduction);
#endif
	SAFE_DELETE_ARRAY(m_piDomainFreeExperience);
	SAFE_DELETE_ARRAY(m_piDomainFreeExperiencePerGreatWork);
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	SAFE_DELETE_ARRAY(m_piBuildingGreatWorkYieldChange);
	SAFE_DELETE_ARRAY(m_piCityGreatWorkYieldChange);
#endif
	SAFE_DELETE_ARRAY(m_piDomainProductionModifier);
	SAFE_DELETE_ARRAY(m_piPrereqNumOfBuildingClass);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piLocalResourceAnds);
	SAFE_DELETE_ARRAY(m_piLocalResourceOrs);
	SAFE_DELETE_ARRAY(m_paiHurryModifier);
	SAFE_DELETE_ARRAY(m_pbBuildingClassNeededInCity);
	SAFE_DELETE_ARRAY(m_piNumFreeUnits);
	SAFE_DELETE_ARRAY(m_paiBuildingClassHappiness);
	SAFE_DELETE_ARRAY(m_paThemingBonusInfo);

#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiResourceYieldChange.first, m_ppaiResourceYieldChange.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiFeatureYieldChange.first, m_ppaiFeatureYieldChange.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiSpecialistYieldChange.first, m_ppaiSpecialistYieldChange.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiResourceYieldModifier.first, m_ppaiResourceYieldModifier.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiTerrainYieldChange.first, m_ppaiTerrainYieldChange.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiBuildingClassYieldChanges.first, m_ppiBuildingClassYieldChanges.second);
#else
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiResourceYieldChange);
#if defined(TRADE_REFACTOR)
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiTradeConnectionOriginLandYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiTradeConnectionOriginSeaYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiTradeConnectionDestinationLandYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiTradeConnectionDestinationSeaYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiIncomingTradeConnectionLandYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiIncomingTradeConnectionSeaYieldChange);
#endif
#if defined(MISC_CHANGES)
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiResourceClassYieldChange);
#endif
#if defined(LEKMOD_v34)
	SAFE_DELETE_ARRAY(m_piGarrisonYieldChange);
#endif
#if defined(LEKMOD_AREA_BASED_CITY_YIELD)
	SAFE_DELETE_ARRAY(m_piSameLandMassYieldChange);
	SAFE_DELETE_ARRAY(m_piDifferentLandMassYieldChange);
#endif
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiFeatureYieldChange);
	m_ppiResourceYieldChangeGlobal.clear();
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiImprovementYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiImprovementYieldChangeGlobal);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiSpecialistYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiResourceYieldModifier);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiTerrainYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiBuildingClassYieldChanges);
#endif
#ifdef LEKMOD_BUILDING_GP_EXPEND_YIELD
	SAFE_DELETE_ARRAY(m_piGreatPersonExpendYield);
#endif
}


bool CvBuildingEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;


	m_iGoldMaintenance = kResults.GetInt("GoldMaintenance");
#if defined(BEE)
	m_bNoSell = kResults.GetBool("Sellable");
#endif
	m_iMutuallyExclusiveGroup = kResults.GetInt("MutuallyExclusiveGroup");
	m_bTeamShare = kResults.GetBool("TeamShare");
	m_bWater = kResults.GetBool("Water");
	m_bRiver = kResults.GetBool("River");
	m_bFreshWater = kResults.GetBool("FreshWater");
#if defined(LEKMOD_BUILDING_LAKE_REQ)
	m_bLake = kResults.GetBool("Lake");
#endif
	m_bMountain = kResults.GetBool("Mountain");
	m_bHill = kResults.GetBool("Hill");
	m_bFlat = kResults.GetBool("Flat");
	m_bFoundsReligion = kResults.GetBool("FoundsReligion");
	m_bIsReligious = kResults.GetBool("IsReligious");
	m_bBorderObstacle = kResults.GetBool("BorderObstacle");
	m_bPlayerBorderObstacle = kResults.GetBool("PlayerBorderObstacle");
	m_bCapital = kResults.GetBool("Capital");
	m_bGoldenAge = kResults.GetBool("GoldenAge");
	m_bGrantsFreeCulturalGreatPersonWithTrait = kResults.GetBool("GrantsFreeCulturalGreatPersonWithTrait");
	m_bMapCentering = kResults.GetBool("MapCentering");
	m_bNeverCapture = kResults.GetBool("NeverCapture");
	m_bNukeImmune = kResults.GetBool("NukeImmune");
	m_bCityWall = kResults.GetBool("CityWall");
	m_bExtraLuxuries = kResults.GetBool("ExtraLuxuries");
	m_bDiplomaticVoting = kResults.GetBool("DiplomaticVoting");
	m_bAllowsWaterRoutes = kResults.GetBool("AllowsWaterRoutes");
	m_iProductionCost = kResults.GetInt("Cost");
#ifdef LEKMOD_BUILDING_GOLD_COST
	m_iGoldCost = kResults.GetInt("GoldCost");
	m_bRequiresGoldPurchase = kResults.GetBool("RequiresGoldPurchase");
#endif
	m_iFaithCost = kResults.GetInt("FaithCost");
	m_iLeagueCost = kResults.GetInt("LeagueCost");
	m_bUnlockedByBelief = kResults.GetBool("UnlockedByBelief");
	m_bUnlockedByLeague = kResults.GetBool("UnlockedByLeague");
	m_bRequiresHolyCity = kResults.GetBool("HolyCity");
	m_bAffectSpiesNow = kResults.GetBool("AffectSpiesNow");
	m_bEspionage = kResults.GetBool("Espionage");
#ifdef NQ_MALI_TREASURY
	m_bMalianTreasury = kResults.GetBool("MaliTreasury");
#endif
	m_bAllowsFoodTradeRoutes = kResults.GetBool("AllowsFoodTradeRoutes");
	m_bAllowsProductionTradeRoutes = kResults.GetBool("AllowsProductionTradeRoutes");
	m_bNullifyInfluenceModifier = kResults.GetBool("NullifyInfluenceModifier");
#if defined(LEKMOD_BUILDING_FIRST_PURCHASE_DISCOUNT)
	m_iFirstPurchaseDiscount = kResults.GetInt("FirstPurchaseDiscount");
#endif
	m_iNumCityCostMod = kResults.GetInt("NumCityCostMod");
	m_iHurryCostModifier = kResults.GetInt("HurryCostModifier");
	m_iMinAreaSize = kResults.GetInt("MinAreaSize");
	m_iConquestProbability = kResults.GetInt("ConquestProb");
	m_iNumCitiesPrereq = kResults.GetInt("CitiesPrereq");
	m_iUnitLevelPrereq = kResults.GetInt("LevelPrereq");
	m_iCultureRateModifier = kResults.GetInt("CultureRateModifier");
	m_iGlobalCultureRateModifier = kResults.GetInt("GlobalCultureRateModifier");
	m_iGreatPeopleRateModifier = kResults.GetInt("GreatPeopleRateModifier");
	m_iGlobalGreatPeopleRateModifier = kResults.GetInt("GlobalGreatPeopleRateModifier");
	m_iGreatGeneralRateModifier = kResults.GetInt("GreatGeneralRateModifier");
	m_iGreatPersonExpendGold = kResults.GetInt("GreatPersonExpendGold");
	m_iUnitUpgradeCostMod = kResults.GetInt("UnitUpgradeCostMod");
	m_iGoldenAgeModifier = kResults.GetInt("GoldenAgeModifier");
	m_iFreeExperience = kResults.GetInt("Experience");
	m_iGlobalFreeExperience = kResults.GetInt("GlobalExperience");
	m_iFoodKept = kResults.GetInt("FoodKept");
#if defined(LEKMOD_BUILDING_EXCESS_GROWTH)
	m_iExcessGrowth = kResults.GetInt("ExcessGrowth");
#endif
	m_bAirlift = kResults.GetBool("Airlift");
	m_iAirModifier = kResults.GetInt("AirModifier");
	m_iNukeModifier = kResults.GetInt("NukeModifier");
	m_iNukeExplosionRand = kResults.GetInt("NukeExplosionRand");
	m_iHealRateChange = kResults.GetInt("HealRateChange");
	m_iHappiness = kResults.GetInt("Happiness");
	m_iUnmoddedHappiness = kResults.GetInt("UnmoddedHappiness");
	m_iUnhappinessModifier = kResults.GetInt("UnhappinessModifier");
	m_iHappinessPerCity = kResults.GetInt("HappinessPerCity");
	m_iHappinessPerXPolicies = kResults.GetInt("HappinessPerXPolicies");
	m_iCityCountUnhappinessMod = kResults.GetInt("CityCountUnhappinessMod");
	m_bNoOccupiedUnhappiness = kResults.GetBool("NoOccupiedUnhappiness");
	m_iWorkerSpeedModifier = kResults.GetInt("WorkerSpeedModifier");
	m_iMilitaryProductionModifier = kResults.GetInt("MilitaryProductionModifier");
#if defined(LEKMOD_BUILDING_MILITARY_PRODUCTION_MOD)
	m_iMilitaryProductionMod = kResults.GetInt("MilitaryProductionMod");
#endif
	m_iSpaceProductionModifier = kResults.GetInt("SpaceProductionModifier");
	m_iGlobalSpaceProductionModifier = kResults.GetInt("GlobalSpaceProductionModifier");
	m_iBuildingProductionModifier = kResults.GetInt("BuildingProductionModifier");
	m_iWonderProductionModifier = kResults.GetInt("WonderProductionModifier");
	m_iCityConnectionTradeRouteModifier = kResults.GetInt("CityConnectionTradeRouteModifier");
	m_iCapturePlunderModifier = kResults.GetInt("CapturePlunderModifier");
	m_iPolicyCostModifier = kResults.GetInt("PolicyCostModifier");
	m_iPlotCultureCostModifier = kResults.GetInt("PlotCultureCostModifier");
	m_iGlobalPlotCultureCostModifier = kResults.GetInt("GlobalPlotCultureCostModifier");
	m_iPlotBuyCostModifier = kResults.GetInt("PlotBuyCostModifier");
	m_iGlobalPlotBuyCostModifier = kResults.GetInt("GlobalPlotBuyCostModifier");
	m_iGlobalPopulationChange = kResults.GetInt("GlobalPopulationChange");
#ifdef NQ_LOCAL_POPULATION_CHANGE_FROM_BUILDING
	m_iLocalPopulationChange = kResults.GetInt("LocalPopulationChange");
#endif
	m_iTechShare = kResults.GetInt("TechShare");
	m_iFreeTechs = kResults.GetInt("FreeTechs");
	m_iFreePolicies = kResults.GetInt("FreePolicies");
	m_iFreeFlatFaith = kResults.GetInt("FreeFlatFaith");
	m_iFreeGreatPeople = kResults.GetInt("FreeGreatPeople");
	m_iMedianTechPercentChange = kResults.GetInt("MedianTechPercentChange");
	m_iGold = kResults.GetInt("Gold");
	m_bNearbyMountainRequired = kResults.GetInt("NearbyMountainRequired");
	m_bAllowsRangeStrike = kResults.GetInt("AllowsRangeStrike");
	m_iDefenseModifier = kResults.GetInt("Defense");
#ifdef NQ_BUILDING_DEFENSE_FROM_CITIZENS
	m_iDefensePerCitizen = kResults.GetInt("DefensePerCitizen");
#endif
	m_iGlobalDefenseModifier = kResults.GetInt("GlobalDefenseMod");
	m_iExtraCityHitPoints = kResults.GetInt("ExtraCityHitPoints");
	m_iMinorFriendshipChange = kResults.GetInt("MinorFriendshipChange");
	m_iVictoryPoints = kResults.GetInt("VictoryPoints");
	m_iExtraMissionarySpreads = kResults.GetInt("ExtraMissionarySpreads");
	m_iReligiousPressureModifier = kResults.GetInt("ReligiousPressureModifier");
	m_iEspionageModifier = kResults.GetInt("EspionageModifier");
	m_iGlobalEspionageModifier = kResults.GetInt("GlobalEspionageModifier");
	m_iExtraSpies = kResults.GetInt("ExtraSpies");
	m_iSpyRankChange = kResults.GetInt("SpyRankChange");
	m_iTradeRouteRecipientBonus = kResults.GetInt("TradeRouteRecipientBonus");
	m_iTradeRouteTargetBonus = kResults.GetInt("TradeRouteTargetBonus");
	m_iNumTradeRouteBonus = kResults.GetInt("NumTradeRouteBonus");
	m_iTradeRouteSeaDistanceModifier = kResults.GetInt("TradeRouteSeaDistanceModifier");
	m_iTradeRouteSeaGoldBonus = kResults.GetInt("TradeRouteSeaGoldBonus");
	m_iTradeRouteLandDistanceModifier = kResults.GetInt("TradeRouteLandDistanceModifier");
	m_iTradeRouteLandGoldBonus = kResults.GetInt("TradeRouteLandGoldBonus");
	m_iCityStateTradeRouteProductionModifier = kResults.GetInt("CityStateTradeRouteProductionModifier");
	m_iCityStateTradeRouteGoldModifier = kResults.GetInt("CityStateTradeRouteGoldModifier");
	m_iInstantSpyRankChange = kResults.GetInt("InstantSpyRankChange");
	m_iLandmarksTourismPercent = kResults.GetInt("LandmarksTourismPercent");
	m_iInstantMilitaryIncrease = kResults.GetInt("InstantMilitaryIncrease");
	m_iGreatWorksTourismModifier = kResults.GetInt("GreatWorksTourismModifier");
	m_iXBuiltTriggersIdeologyChoice = kResults.GetInt("XBuiltTriggersIdeologyChoice");
	m_iGreatScientistBeakerModifier = kResults.GetInt("GreatScientistBeakerModifier");
	m_iExtraLeagueVotes = kResults.GetInt("ExtraLeagueVotes");
#if defined(LEKMOD_GARRISON_YIELD_EFFECTS)
	m_iGarrisonStrengthBonus = kResults.GetInt("GarrisonStrengthBonus");
	m_bGarrisonMaintenanceFree = kResults.GetBool("GarrisonMaintenanceFree");
#endif
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	m_iGreatWorkHappiness = kResults.GetInt("GreatWorkHappiness");
#endif
	m_iPreferredDisplayPosition = kResults.GetInt("DisplayPosition");
	m_iPortraitIndex = kResults.GetInt("PortraitIndex");

	m_bArtInfoCulturalVariation = kResults.GetBool("ArtInfoCulturalVariation");
	m_bArtInfoEraVariation = kResults.GetBool("ArtInfoEraVariation");
	m_bArtInfoRandomVariation = kResults.GetBool("ArtInfoRandomVariation");


	const char* szTextVal;
	szTextVal = kResults.GetText("BuildingClass");
	m_iBuildingClassType = GC.getInfoTypeForString(szTextVal, true);


	m_pkBuildingClassInfo = GC.getBuildingClassInfo(static_cast<BuildingClassTypes>(m_iBuildingClassType));
	CvAssertMsg(m_pkBuildingClassInfo, "Could not find BuildingClassInfo for BuildingType. Have BuildingClasses been prefetched yet?");

	szTextVal = kResults.GetText("ArtDefineTag");
	SetArtDefineTag(szTextVal);

	szTextVal = kResults.GetText("WonderSplashAudio");
	m_strWonderSplashAudio = szTextVal;

	szTextVal = kResults.GetText("ThemingBonusHelp");
	m_strThemingBonusHelp = szTextVal;

	szTextVal = kResults.GetText("NearbyTerrainRequired");
	m_iNearbyTerrainRequired = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("ProhibitedCityTerrain");
	m_iProhibitedCityTerrain = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("VictoryPrereq");
	m_iVictoryPrereq = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("FreeStartEra");
	m_iFreeStartEra = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("MaxStartEra");
	m_iMaxStartEra = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("ObsoleteTech");
	m_iObsoleteTech = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("EnhancedYieldTech");
	m_iEnhancedYieldTech = GC.getInfoTypeForString(szTextVal, true);

	m_iTechEnhancedTourism = kResults.GetInt("TechEnhancedTourism");

	szTextVal = kResults.GetText("FreeBuilding");
	m_iFreeBuildingClass = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("FreeBuildingThisCity");
	m_iFreeBuildingThisCity = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("FreePromotion");
	m_iFreePromotion = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("TrainedFreePromotion");
	m_iTrainedFreePromotion = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("FreePromotionRemoved");
	m_iFreePromotionRemoved = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("ReplacementBuildingClass");
	m_iReplacementBuildingClass= GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("PrereqTech");
	m_iPrereqAndTech = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("PolicyBranchType");
	m_iPolicyBranchType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("SpecialistType");
	m_iSpecialistType = GC.getInfoTypeForString(szTextVal, true);
	m_iSpecialistCount = kResults.GetInt("SpecialistCount");
	m_iSpecialistExtraCulture = kResults.GetInt("SpecialistExtraCulture");
	m_iGreatPeopleRateChange= kResults.GetInt("GreatPeopleRateChange");

	szTextVal = kResults.GetText("GreatWorkSlotType");
	m_eGreatWorkSlotType = (GreatWorkSlotType)GC.getInfoTypeForString(szTextVal, true);
	m_iGreatWorkCount = kResults.GetInt("GreatWorkCount");
	szTextVal = kResults.GetText("FreeGreatWork");
	m_eFreeGreatWork = (GreatWorkType)GC.getInfoTypeForString(szTextVal, true);
#ifdef LEKMOD_v34
	m_iFreeGreatWorkCount = kResults.GetInt("FreeGreatWorkCount");
#endif


	const char* szBuildingType = GetType();

	kUtility.SetFlavors(m_piFlavorValue, "Building_Flavors", "BuildingType", szBuildingType);

	kUtility.SetYields(m_piSeaPlotYieldChange, "Building_SeaPlotYieldChanges", "BuildingType", szBuildingType);
	kUtility.SetYields(m_piRiverPlotYieldChange, "Building_RiverPlotYieldChanges", "BuildingType", szBuildingType);
	kUtility.SetYields(m_piLakePlotYieldChange, "Building_LakePlotYieldChanges", "BuildingType", szBuildingType);
	kUtility.SetYields(m_piSeaResourceYieldChange, "Building_SeaResourceYieldChanges", "BuildingType", szBuildingType);
	kUtility.SetYields(m_piYieldChange, "Building_YieldChanges", "BuildingType", szBuildingType);
	kUtility.SetYields(m_piYieldChangePerPop, "Building_YieldChangesPerPop", "BuildingType", szBuildingType);
	kUtility.SetYields(m_piYieldChangePerReligion, "Building_YieldChangesPerReligion", "BuildingType", szBuildingType);
	kUtility.SetYields(m_piYieldModifier, "Building_YieldModifiers", "BuildingType", szBuildingType);
	kUtility.SetYields(m_piAreaYieldModifier, "Building_AreaYieldModifiers", "BuildingType", szBuildingType);
	kUtility.SetYields(m_piGlobalYieldModifier, "Building_GlobalYieldModifiers", "BuildingType", szBuildingType);
	kUtility.SetYields(m_piTechEnhancedYieldChange, "Building_TechEnhancedYieldChanges", "BuildingType", szBuildingType);
#if defined(LEKMOD_AREA_BASED_CITY_YIELD)
	kUtility.SetYields(m_piSameLandMassYieldChange, "Building_SameLandMassYieldChanges", "BuildingType", szBuildingType);
	kUtility.SetYields(m_piDifferentLandMassYieldChange, "Building_DifferentLandMassYieldChanges", "BuildingType", szBuildingType);
#endif
#if defined(LEKMOD_ERA_ENHANCED_YIELDS)
	kUtility.SetYieldMatrix(m_ppiEraEnhancedYieldChange, "Eras", "Building_EraEnhancedYieldChanges",
		"SELECT Eras.ID as EraID, Yields.ID as YieldID, Yield from Building_EraEnhancedYieldChanges "
		"INNER JOIN Eras on Eras.Type = EraType "
		"INNER JOIN Yields on Yields.Type = YieldType "
		"WHERE BuildingType = ?",
		szBuildingType);
#endif
	kUtility.PopulateArrayByValue(m_piResourceQuantityRequirements, "Resources", "Building_ResourceQuantityRequirements", "ResourceType", "BuildingType", szBuildingType, "Cost");
	kUtility.PopulateArrayByValue(m_piResourceQuantity, "Resources", "Building_ResourceQuantity", "ResourceType", "BuildingType", szBuildingType, "Quantity");
#if !defined(LEKMOD_FIX_BUILDING_RESOURCE_YIELD_CHANGE)
	kUtility.PopulateArrayByValue(m_piResourceCultureChanges, "Resources", "Building_ResourceCultureChanges", "ResourceType", "BuildingType", szBuildingType, "CultureChange");
	kUtility.PopulateArrayByValue(m_piResourceFaithChanges, "Resources", "Building_ResourceFaithChanges", "ResourceType", "BuildingType", szBuildingType, "FaithChange");
#else
	kUtility.SetYieldMatrix(m_paiBuildingLocalResourceYieldChanges, "Resources", "Building_LocalResourceYieldChanges",
		"SELECT Resources.ID as ResourceID, Yields.ID as YieldID, Yield from Building_LocalResourceYieldChanges "
		"INNER JOIN Resources on Resources.Type = ResourceType "
		"INNER JOIN Yields on Yields.Type = YieldType "
		"WHERE BuildingType = ?",
		szBuildingType);
#endif
	kUtility.PopulateArrayByValue(m_paiHurryModifier, "HurryInfos", "Building_HurryModifiers", "HurryType", "BuildingType", szBuildingType, "HurryCostModifier");



	kUtility.PopulateArrayByValue(m_piUnitCombatFreeExperience, "UnitCombatInfos", "Building_UnitCombatFreeExperiences", "UnitCombatType", "BuildingType", szBuildingType, "Experience");
	kUtility.PopulateArrayByValue(m_piUnitCombatProductionModifiers, "UnitCombatInfos", "Building_UnitCombatProductionModifiers", "UnitCombatType", "BuildingType", szBuildingType, "Modifier");
	kUtility.PopulateArrayByValue(m_piDomainFreeExperience, "Domains", "Building_DomainFreeExperiences", "DomainType", "BuildingType", szBuildingType, "Experience", 0, NUM_DOMAIN_TYPES);
	kUtility.PopulateArrayByValue(m_piDomainFreeExperiencePerGreatWork, "Domains", "Building_DomainFreeExperiencePerGreatWork", "DomainType", "BuildingType", szBuildingType, "Experience", 0, NUM_DOMAIN_TYPES);
	kUtility.PopulateArrayByValue(m_piDomainProductionModifier, "Domains", "Building_DomainProductionModifiers", "DomainType", "BuildingType", szBuildingType, "Modifier", 0, NUM_DOMAIN_TYPES);

	kUtility.PopulateArrayByValue(m_piPrereqNumOfBuildingClass, "BuildingClasses", "Building_PrereqBuildingClasses", "BuildingClassType", "BuildingType", szBuildingType, "NumBuildingNeeded");
	kUtility.PopulateArrayByExistence(m_pbBuildingClassNeededInCity, "BuildingClasses", "Building_ClassesNeededInCity", "BuildingClassType", "BuildingType", szBuildingType);

	kUtility.PopulateArrayByValue(m_piNumFreeUnits, "Units", "Building_FreeUnits", "UnitType", "BuildingType", szBuildingType, "NumUnits");
	kUtility.PopulateArrayByValue(m_paiBuildingClassHappiness, "BuildingClasses", "Building_BuildingClassHappiness", "BuildingClassType", "BuildingType", szBuildingType, "Happiness");

	kUtility.PopulateArrayByExistence(m_piLockedBuildingClasses, "BuildingClasses", "Building_LockedBuildingClasses", "BuildingClassType", "BuildingType", szBuildingType);
	kUtility.PopulateArrayByExistence(m_piPrereqAndTechs, "Technologies", "Building_TechAndPrereqs", "TechType", "BuildingType", szBuildingType);
	kUtility.PopulateArrayByExistence(m_piLocalResourceAnds, "Resources", "Building_LocalResourceAnds", "ResourceType", "BuildingType", szBuildingType);
	kUtility.PopulateArrayByExistence(m_piLocalResourceOrs, "Resources", "Building_LocalResourceOrs", "ResourceType", "BuildingType", szBuildingType);
#if defined(LEKMOD_v34)
	kUtility.SetYields(m_piGarrisonYieldChange, "Building_GarrisonYieldChanges", "BuildingType", szBuildingType);
#endif
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	m_iGreatWorkMilitaryProductionModifier = kResults.GetInt("GreatWorkMilitaryProductionModifier");

	{
		kUtility.InitializeArray(m_piBuildingGreatWorkYieldChange, "Yields");
		kUtility.InitializeArray(m_piCityGreatWorkYieldChange, "Yields");
		kUtility.Initialize2DArray(m_ppiCityGreatWorkClassYieldChange, "GreatWorkClasses", "Yields");
		std::string strKey("Building_GreatWorkYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT Yields.ID as YieldID, COALESCE(GreatWorkClasses.ID, -1) as GreatWorkClassID, YieldChange, HoldingYield "
			"FROM Building_GreatWorkYieldChanges "
			"INNER JOIN Yields on Yields.Type = YieldType "
			"INNER JOIN GreatWorkClasses on GreatWorkClasses.Type = GreatWorkClassType "
			"WHERE BuildingType = ?");
		pResults->Bind(1, szBuildingType);
		while (pResults->Step())
		{
			const int YieldID = pResults->GetInt(0);
			const int GreatWorkClassID = pResults->GetInt(1);
			const int YieldChange = pResults->GetInt(2);
			const int HoldingYield = pResults->GetInt(3);
			m_piBuildingGreatWorkYieldChange[YieldID] = HoldingYield;
			m_piCityGreatWorkYieldChange[YieldID] = YieldChange;
			if (GreatWorkClassID != NO_GREAT_WORK_CLASS) 
			{
				m_ppiCityGreatWorkClassYieldChange[GreatWorkClassID][YieldID] = YieldChange;
			}
		}
	}
#endif

	{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
		kUtility.Initialize2DArray(m_ppaiResourceYieldChange.first, "Resources", "Yields");
		m_ppaiResourceYieldChange.second = kUtility.MaxRows("Resources");
#else
		kUtility.Initialize2DArray(m_ppaiResourceYieldChange, "Resources", "Yields");
#endif

		std::string strKey("Building_ResourceYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Resources.ID as ResourceID, Yields.ID as YieldID, Yield from Building_ResourceYieldChanges inner join Resources on Resources.Type = ResourceType inner join Yields on Yields.Type = YieldType where BuildingType = ?");

		pResults->Bind(1, szBuildingType);

		while(pResults->Step())
		{
			const int ResourceID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
			m_ppaiResourceYieldChange.first[ResourceID][YieldID] = yield;
#else
			m_ppaiResourceYieldChange[ResourceID][YieldID] = yield;
#endif
		}
	}

	{
		std::string strKey("Building_ResourceYieldChangesGlobal");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Resources.ID as ResourceID, Yields.ID as YieldID, Yield from Building_ResourceYieldChangesGlobal inner join Resources on Resources.Type = ResourceType inner join Yields on Yields.Type = YieldType where BuildingType = ?");

		pResults->Bind(1, szBuildingType);

		while (pResults->Step())
		{
			const int iResource = pResults->GetInt(0);
			const int iYieldType = pResults->GetInt(1);
			const int iYield = pResults->GetInt(2);

			m_ppiResourceYieldChangeGlobal[iResource][iYieldType] += iYield;
		}

		pResults->Reset();


		std::map<int, std::map<int, int>>(m_ppiResourceYieldChangeGlobal).swap(m_ppiResourceYieldChangeGlobal);
	}
#if defined(TRADE_REFACTOR)

	{
		kUtility.Initialize2DArray(m_ppaiTradeConnectionOriginLandYieldChange, "TradeConnections", "Yields");
		kUtility.Initialize2DArray(m_ppaiTradeConnectionOriginSeaYieldChange, "TradeConnections", "Yields");
		std::string strKey("Building_TradeConnectionOriginYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT TradeConnections.ID as TradeConnectionID, Domains.ID as DomainID, Yields.ID AS YieldID, YieldTimes100 "
			"FROM Building_TradeConnectionOriginYieldChanges "
			"INNER JOIN TradeConnections ON TradeConnections.Type = TradeConnectionType "
			"INNER JOIN Domains ON Domains.Type = DomainType "
			"INNER JOIN Yields ON Yields.Type = YieldType "
			"WHERE BuildingType = ?");
		pResults->Bind(1, szBuildingType);
		while (pResults->Step())
		{
			const int iTradeConnectionID = pResults->GetInt(0);
			const int iDomainID = pResults->GetInt(1);
			const int iYieldID = pResults->GetInt(2);
			const int iYieldTimes100 = pResults->GetInt(3);
			if (iDomainID == DOMAIN_LAND)
				m_ppaiTradeConnectionOriginLandYieldChange[iTradeConnectionID][iYieldID] = iYieldTimes100;
			if (iDomainID == DOMAIN_SEA)
				m_ppaiTradeConnectionOriginSeaYieldChange[iTradeConnectionID][iYieldID] = iYieldTimes100;
		}
		pResults->Reset();
	}

	{
		kUtility.Initialize2DArray(m_ppaiTradeConnectionDestinationLandYieldChange, "TradeConnections", "Yields");
		kUtility.Initialize2DArray(m_ppaiTradeConnectionDestinationSeaYieldChange, "TradeConnections", "Yields");
		std::string strKey("Building_TradeConnectionDestinationYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT TradeConnections.ID as TradeConnectionID, Domains.ID as DomainID, Yields.ID AS YieldID, YieldTimes100 "
			"FROM Building_TradeConnectionDestinationYieldChanges "
			"INNER JOIN TradeConnections ON TradeConnections.Type = TradeConnectionType "
			"INNER JOIN Domains ON Domains.Type = DomainType "
			"INNER JOIN Yields ON Yields.Type = YieldType "
			"WHERE BuildingType = ?");
		pResults->Bind(1, szBuildingType);
		while (pResults->Step())
		{
			const int iTradeConnectionID = pResults->GetInt(0);
			const int iDomainID = pResults->GetInt(1);
			const int iYieldID = pResults->GetInt(2);
			const int iYieldTimes100 = pResults->GetInt(3);
			if (iDomainID == DOMAIN_LAND)
				m_ppaiTradeConnectionDestinationLandYieldChange[iTradeConnectionID][iYieldID] = iYieldTimes100;
			if (iDomainID == DOMAIN_SEA)
				m_ppaiTradeConnectionDestinationSeaYieldChange[iTradeConnectionID][iYieldID] = iYieldTimes100;
		}
		pResults->Reset();
	}

	{
		kUtility.Initialize2DArray(m_ppaiIncomingTradeConnectionLandYieldChange, "TradeConnections", "Yields");
		kUtility.Initialize2DArray(m_ppaiIncomingTradeConnectionSeaYieldChange, "TradeConnections", "Yields");
		std::string strKey("Building_IncomingTradeConnectionYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT TradeConnections.ID as TradeConnectionID, Domains.ID as DomainID, Yields.ID AS YieldID, YieldTimes100 "
			"FROM Building_IncomingTradeConnectionYieldChanges "
			"INNER JOIN TradeConnections ON TradeConnections.Type = TradeConnectionType "
			"INNER JOIN Domains ON Domains.Type = DomainType "
			"INNER JOIN Yields ON Yields.Type = YieldType "
			"WHERE BuildingType = ?");
		pResults->Bind(1, szBuildingType);
		while (pResults->Step())
		{
			const int iTradeConnectionID = pResults->GetInt(0);
			const int iDomainID = pResults->GetInt(1);
			const int iYieldID = pResults->GetInt(2);
			const int iYieldTimes100 = pResults->GetInt(3);
			if (iDomainID == DOMAIN_LAND)
				m_ppaiIncomingTradeConnectionLandYieldChange[iTradeConnectionID][iYieldID] = iYieldTimes100;
			if (iDomainID == DOMAIN_SEA)
				m_ppaiIncomingTradeConnectionSeaYieldChange[iTradeConnectionID][iYieldID] = iYieldTimes100;
		}
		pResults->Reset();
	}
#endif
#if defined(MISC_CHANGES)
	{
		kUtility.Initialize2DArray(m_ppaiResourceClassYieldChange, "ResourceClasses", "Yields");
		std::string strKey("Building_ResourceClassYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT ResourceClasses.ID AS ResourceClassID, Yields.ID AS YieldID, Building_ResourceClassYieldChanges.Yield "
			"FROM Building_ResourceClassYieldChanges "
			"INNER JOIN ResourceClasses ON ResourceClasses.Type = ResourceClassType "
			"INNER JOIN Yields ON Yields.Type = YieldType "
			"WHERE BuildingType = ?");
		pResults->Bind(1, szBuildingType);
		while (pResults->Step())
		{
			const int iResourceClassID = pResults->GetInt(0);
			const int iYieldID = pResults->GetInt(1);
			const int iYield = pResults->GetInt(2);
			m_ppaiResourceClassYieldChange[iResourceClassID][iYieldID] = iYield;
		}
		pResults->Reset();
	}
#endif

	{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
		kUtility.Initialize2DArray(m_ppaiFeatureYieldChange.first, "Features", "Yields");
		m_ppaiFeatureYieldChange.second = kUtility.MaxRows("Features");
#else
		kUtility.Initialize2DArray(m_ppaiFeatureYieldChange, "Features", "Yields");
#endif

		std::string strKey("Building_FeatureYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Features.ID as FeatureID, Yields.ID as YieldID, Yield from Building_FeatureYieldChanges inner join Features on Features.Type = FeatureType inner join Yields on Yields.Type = YieldType where BuildingType = ?");

		pResults->Bind(1, szBuildingType);

		while(pResults->Step())
		{
			const int FeatureID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
			m_ppaiFeatureYieldChange.first[FeatureID][YieldID] = yield;
#else
			m_ppaiFeatureYieldChange[FeatureID][YieldID] = yield;
#endif
		}
	}

	kUtility.SetYieldMatrix(m_ppaiImprovementYieldChange, "Improvements", "Building_ImprovementYieldChanges",
		"select Improvements.ID as ImprovementID, Yields.ID as YieldID, Yield from Building_ImprovementYieldChanges inner join Improvements on Improvements.Type = ImprovementType inner join Yields on Yields.Type = YieldType where BuildingType = ?",
		szBuildingType);

	kUtility.SetYieldMatrix(m_ppaiImprovementYieldChangeGlobal, "Improvements", "Building_ImprovementYieldChangesGlobal",
		"select Improvements.ID as ImprovementID, Yields.ID as YieldID, Yield from Building_ImprovementYieldChangesGlobal inner join Improvements on Improvements.Type = ImprovementType inner join Yields on Yields.Type = YieldType where BuildingType = ?",
		szBuildingType);


	{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
		kUtility.Initialize2DArray(m_ppaiTerrainYieldChange.first, "Terrains", "Yields");
		m_ppaiTerrainYieldChange.second = kUtility.MaxRows("Terrains");
#else
		kUtility.Initialize2DArray(m_ppaiTerrainYieldChange, "Terrains", "Yields");
#endif

		std::string strKey("Building_TerrainYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Terrains.ID as TerrainID, Yields.ID as YieldID, Yield from Building_TerrainYieldChanges inner join Terrains on Terrains.Type = TerrainType inner join Yields on Yields.Type = YieldType where BuildingType = ?");

		pResults->Bind(1, szBuildingType);

		while(pResults->Step())
		{
			const int TerrainID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
			m_ppaiTerrainYieldChange.first[TerrainID][YieldID] = yield;
#else
			m_ppaiTerrainYieldChange[TerrainID][YieldID] = yield;
#endif
		}
	}
#if defined(LEKMOD_NEARBY_TERRAIN_FREE_YIELDS)

	{
		m_aFreeTerrainYields.clear();

		std::string strKey("Building_NearbyTerrainFreeYields");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Terrains.ID as TerrainID, Yields.ID as YieldID, Radius, MinTerrainRequired, RequiresOwner, Yield "
			"from Building_NearbyTerrainFreeYields "
			"inner join Terrains on Terrains.Type = TerrainType "
			"inner join Yields on Yields.Type = YieldType "
			"where BuildingType = ?");

		pResults->Bind(1, szBuildingType);

		while(pResults->Step())
		{
			BuildingFreeTerrainYields kFreeTerrainYield;
			kFreeTerrainYield.m_eTerrain = static_cast<TerrainTypes>(pResults->GetInt(0));
			kFreeTerrainYield.m_eYield = static_cast<YieldTypes>(pResults->GetInt(1));
			kFreeTerrainYield.m_iRadius = pResults->GetInt(2);
			kFreeTerrainYield.m_iMinTerrainRequired = pResults->GetInt(3);
			kFreeTerrainYield.m_bRequiresOwner = pResults->GetBool(4);
			kFreeTerrainYield.m_iYieldChange = pResults->GetInt(5);

			m_aFreeTerrainYields.push_back(kFreeTerrainYield);
		}
	}
#endif

	{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
		kUtility.Initialize2DArray(m_ppaiSpecialistYieldChange.first, "Specialists", "Yields");
		m_ppaiSpecialistYieldChange.second = kUtility.MaxRows("Specialists");
#else
		kUtility.Initialize2DArray(m_ppaiSpecialistYieldChange, "Specialists", "Yields");
#endif

		std::string strKey("Building_SpecialistYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Specialists.ID as SpecialistID, Yields.ID as YieldID, Yield from Building_SpecialistYieldChanges inner join Specialists on Specialists.Type = SpecialistType inner join Yields on Yields.Type = YieldType where BuildingType = ?");

		pResults->Bind(1, szBuildingType);

		while(pResults->Step())
		{
			const int SpecialistID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
			m_ppaiSpecialistYieldChange.first[SpecialistID][YieldID] = yield;
#else
			m_ppaiSpecialistYieldChange[SpecialistID][YieldID] = yield;
#endif
		}
	}


	{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
		kUtility.Initialize2DArray(m_ppaiResourceYieldModifier.first, "Resources", "Yields");
		m_ppaiResourceYieldModifier.second = kUtility.MaxRows("Resources");
#else
		kUtility.Initialize2DArray(m_ppaiResourceYieldModifier, "Resources", "Yields");
#endif

		std::string strKey("Building_ResourceYieldModifiers");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Resources.ID as ResourceID, Yields.ID as YieldID, Yield from Building_ResourceYieldModifiers inner join Resources on Resources.Type = ResourceType inner join Yields on Yields.Type = YieldType where BuildingType = ?");

		pResults->Bind(1, szBuildingType);

		while(pResults->Step())
		{
			const int ResourceID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
			m_ppaiResourceYieldModifier.first[ResourceID][YieldID] = yield;
#else
			m_ppaiResourceYieldModifier[ResourceID][YieldID] = yield;
#endif
		}
	}


	{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
		kUtility.Initialize2DArray(m_ppiBuildingClassYieldChanges.first, "BuildingClasses", "Yields");
		m_ppiBuildingClassYieldChanges.second = kUtility.MaxRows("BuildingClasses");
#else
		kUtility.Initialize2DArray(m_ppiBuildingClassYieldChanges, "BuildingClasses", "Yields");
#endif

		std::string strKey("Building_BuildingClassYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select BuildingClasses.ID as BuildingClassID, Yields.ID as YieldID, YieldChange from Building_BuildingClassYieldChanges inner join BuildingClasses on BuildingClasses.Type = BuildingClassType inner join Yields on Yields.Type = YieldType where BuildingType = ?");

		pResults->Bind(1, szBuildingType);

		while(pResults->Step())
		{
			const int BuildingClassID = pResults->GetInt(0);
			const int iYieldID = pResults->GetInt(1);
			const int iYieldChange = pResults->GetInt(2);

#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
			m_ppiBuildingClassYieldChanges.first[BuildingClassID][iYieldID] = iYieldChange;
#else
			m_ppiBuildingClassYieldChanges[BuildingClassID][iYieldID] = iYieldChange;
#endif
		}
	}

#ifdef LEKMOD_BUILDING_GP_EXPEND_YIELD

	{

		kUtility.InitializeArray(m_piGreatPersonExpendYield, "Yields");

		std::string strKey("Building_GreatPersonExpendedYields");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Yields.ID as YieldID, Yield from Building_GreatPersonExpendedYields inner join Yields on Yields.Type = YieldType where BuildingType = ?");

		pResults->Bind(1, szBuildingType);

		while (pResults->Step())
		{
			const int iYieldID = pResults->GetInt(0);
			const int iYieldChange = pResults->GetInt(1);

			m_piGreatPersonExpendYield[iYieldID] = iYieldChange;



		}
	}
#endif

	{

		const int iNumThemes = MAX_THEMING_BONUSES;
		m_paThemingBonusInfo = FNEW(CvThemingBonusInfo[iNumThemes], c_eCiv5GameplayDLL, 0);
		int idx = 0;

		std::string strResourceTypesKey = "Building_ThemingBonuses";
		Database::Results* pResourceTypes = kUtility.GetOrPrepareResults(strResourceTypesKey,
			"select Bonus, Description, SameEra, UniqueEras, MustBeArt, MustBeArtifact, MustBeEqualArtArtifact, RequiresOwner, RequiresAnyButOwner, RequiresSamePlayer, RequiresUniquePlayers, AIPriority from Building_ThemingBonuses where BuildingType = ?");

		const size_t lenBuildingType = strlen(szBuildingType);
		pResourceTypes->Bind(1, szBuildingType, lenBuildingType, false);

		while(pResourceTypes->Step())
		{
			CvThemingBonusInfo& pThemingInfo = m_paThemingBonusInfo[idx];

			pThemingInfo.m_iBonus = pResourceTypes->GetInt("Bonus");
			pThemingInfo.m_strDescription = pResourceTypes->GetText("Description");
			pThemingInfo.m_bSameEra = pResourceTypes->GetBool("SameEra");
			pThemingInfo.m_bUniqueEras = pResourceTypes->GetBool("UniqueEras");
			pThemingInfo.m_bMustBeArt = pResourceTypes->GetBool("MustBeArt");
			pThemingInfo.m_bMustBeArtifact = pResourceTypes->GetBool("MustBeArtifact");
			pThemingInfo.m_bMustBeEqualArtArtifact = pResourceTypes->GetBool("MustBeEqualArtArtifact");
			pThemingInfo.m_bRequiresOwner = pResourceTypes->GetBool("RequiresOwner");
			pThemingInfo.m_bRequiresAnyButOwner = pResourceTypes->GetBool("RequiresAnyButOwner");
			pThemingInfo.m_bRequiresSamePlayer = pResourceTypes->GetBool("RequiresSamePlayer");
			pThemingInfo.m_bRequiresUniquePlayers = pResourceTypes->GetBool("RequiresUniquePlayers");
			pThemingInfo.m_iAIPriority = pResourceTypes->GetInt("AIPriority");

			idx++;
		}

		m_iNumThemingBonuses = idx;
		pResourceTypes->Reset();
	}

#ifdef LEKMOD_v34

	{
		kUtility.InitializeArray(m_piUnitCombatProductionCostModifiers, "UnitCombatInfos");

		std::string strKey = "Building_UnitCombatProductionCostModifiers";
		Database::Results *pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "SELECT UnitCombatInfos.ID as UnitCombatID, CostModifier FROM Building_UnitCombatProductionCostModifiers INNER JOIN UnitCombatInfos ON UnitCombatInfos.Type = UnitCombatType WHERE BuildingType = ?");
		}

		pResults->Bind(1, szBuildingType);

		while (pResults->Step())
		{
			const int iUnitCombatID = pResults->GetInt("UnitCombatID");
			const int iCostModifier = pResults->GetInt("CostModifier");

			m_piUnitCombatProductionCostModifiers[iUnitCombatID] = iCostModifier;
		}
	}


	{
		kUtility.InitializeArray(m_piUnitCombatExtraProduction, "UnitCombatInfos");

		std::string strKey = "Building_UnitCombatExtraProduction";
		Database::Results *pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "SELECT UnitCombatInfos.ID as UnitCombatID, ExtraProduction FROM Building_UnitCombatExtraProduction INNER JOIN UnitCombatInfos ON UnitCombatInfos.Type = UnitCombatType WHERE BuildingType = ?");
		}

		pResults->Bind(1, szBuildingType);

		while (pResults->Step())
		{
			const int iUnitCombatID = pResults->GetInt("UnitCombatID");
			const int iExtraProduction = pResults->GetInt("ExtraProduction");

			m_piUnitCombatExtraProduction[iUnitCombatID] = iExtraProduction;
		}
	}
#endif

	return true;
}


int CvBuildingEntry::GetBuildingClassType() const
{
	return m_iBuildingClassType;
}

const CvBuildingClassInfo& CvBuildingEntry::GetBuildingClassInfo() const
{
	if(m_pkBuildingClassInfo == NULL)
	{
		const char* szError = "ERROR: Building does not contain valid BuildingClass type!!";
		GC.LogMessage(szError);
		CvAssertMsg(false, szError);
	}

#pragma warning ( push )
#pragma warning ( disable : 6011 )
	return *m_pkBuildingClassInfo;
#pragma warning ( pop )
}


int CvBuildingEntry::GetNearbyTerrainRequired() const
{
	return m_iNearbyTerrainRequired;
}


int CvBuildingEntry::GetProhibitedCityTerrain() const
{
	return m_iProhibitedCityTerrain;
}


int CvBuildingEntry::GetVictoryPrereq() const
{
	return m_iVictoryPrereq;
}


int CvBuildingEntry::GetFreeStartEra() const
{
	return m_iFreeStartEra;
}


int CvBuildingEntry::GetMaxStartEra() const
{
	return m_iMaxStartEra;
}


int CvBuildingEntry::GetObsoleteTech() const
{
	return m_iObsoleteTech;
}


int CvBuildingEntry::GetEnhancedYieldTech() const
{
	return m_iEnhancedYieldTech;
}


int CvBuildingEntry::GetTechEnhancedTourism() const
{
	return m_iTechEnhancedTourism;
}


int CvBuildingEntry::GetGoldMaintenance() const
{
	return m_iGoldMaintenance;
}


int CvBuildingEntry::GetMutuallyExclusiveGroup() const
{
	return m_iMutuallyExclusiveGroup;
}


int CvBuildingEntry::GetReplacementBuildingClass() const
{
	return m_iReplacementBuildingClass;
}


int CvBuildingEntry::GetEra() const
{
	TechTypes eTech = (TechTypes)GetPrereqAndTech();
	if (eTech != NO_TECH)
	{
		CvTechEntry* pTech = GC.getTechInfo((TechTypes)GetPrereqAndTech());
		return pTech->GetEra();
	}

	return -1;
}



int CvBuildingEntry::GetPrereqAndTech() const
{
	return m_iPrereqAndTech;
}


int CvBuildingEntry::GetPolicyBranchType() const
{
	return m_iPolicyBranchType;
}


int CvBuildingEntry::GetSpecialistType() const
{
	return m_iSpecialistType;
}


int CvBuildingEntry::GetSpecialistCount() const
{
	return m_iSpecialistCount;
}


int CvBuildingEntry::GetSpecialistExtraCulture() const
{
	return m_iSpecialistExtraCulture;
}


int CvBuildingEntry::GetGreatPeopleRateChange() const
{
	return m_iGreatPeopleRateChange;
}


GreatWorkSlotType CvBuildingEntry::GetGreatWorkSlotType() const
{
	return m_eGreatWorkSlotType;
}


#ifdef AUI_WARNING_FIXES
uint CvBuildingEntry::GetGreatWorkCount() const
#else
int CvBuildingEntry::GetGreatWorkCount() const
#endif
{
	return m_iGreatWorkCount;
}


GreatWorkType CvBuildingEntry::GetFreeGreatWork() const
{
	return m_eFreeGreatWork;
}

#ifdef LEKMOD_v34

int CvBuildingEntry::GetFreeGreatWorkCount() const
{
	return m_iFreeGreatWorkCount;
}
#endif


int CvBuildingEntry::GetFreeBuildingClass() const
{
	return m_iFreeBuildingClass;
}


int CvBuildingEntry::GetFreeBuildingThisCity() const
{
	return m_iFreeBuildingThisCity;
}


int CvBuildingEntry::GetFreePromotion() const
{
	return m_iFreePromotion;
}


int CvBuildingEntry::GetTrainedFreePromotion() const
{
	return m_iTrainedFreePromotion;
}


int CvBuildingEntry::GetFreePromotionRemoved() const
{
	return m_iFreePromotionRemoved;
}


int CvBuildingEntry::GetProductionCost() const
{
	return m_iProductionCost;
}

#ifdef LEKMOD_BUILDING_GOLD_COST


bool CvBuildingEntry::RequiresGoldPurchase() const
{
	return m_bRequiresGoldPurchase;
}

int CvBuildingEntry::GetGoldCost() const
{
	return m_iGoldCost;
}
#endif

int CvBuildingEntry::GetFaithCost() const
{
	return m_iFaithCost;
}


int CvBuildingEntry::GetLeagueCost() const
{
	return m_iLeagueCost;
}


int CvBuildingEntry::GetNumCityCostMod() const
{
	return m_iNumCityCostMod;
}


int CvBuildingEntry::GetHurryCostModifier() const
{
	return m_iHurryCostModifier;
}


int CvBuildingEntry::GetNumCitiesPrereq() const
{
	return m_iNumCitiesPrereq;
}


int CvBuildingEntry::GetUnitLevelPrereq() const
{
	return m_iUnitLevelPrereq;
}


int CvBuildingEntry::GetCultureRateModifier() const
{
	return m_iCultureRateModifier;
}


int CvBuildingEntry::GetGlobalCultureRateModifier() const
{
	return m_iGlobalCultureRateModifier;
}


int CvBuildingEntry::GetGreatPeopleRateModifier() const
{
	return m_iGreatPeopleRateModifier;
}


int CvBuildingEntry::GetGlobalGreatPeopleRateModifier() const
{
	return m_iGlobalGreatPeopleRateModifier;
}


int CvBuildingEntry::GetGreatGeneralRateModifier() const
{
	return m_iGreatGeneralRateModifier;
}


int CvBuildingEntry::GetGreatPersonExpendGold() const
{
	return m_iGreatPersonExpendGold;
}


int CvBuildingEntry::GetUnitUpgradeCostMod() const
{
	return m_iUnitUpgradeCostMod;
}


int CvBuildingEntry::GetGoldenAgeModifier() const
{
	return m_iGoldenAgeModifier;
}


int CvBuildingEntry::GetFreeExperience() const
{
	return m_iFreeExperience;
}


int CvBuildingEntry::GetGlobalFreeExperience() const
{
	return m_iGlobalFreeExperience;
}


int CvBuildingEntry::GetFoodKept() const
{
	return m_iFoodKept;
}

#if defined(LEKMOD_BUILDING_EXCESS_GROWTH)

int CvBuildingEntry::GetExcessGrowth() const
{
	return m_iExcessGrowth;
}
#endif


bool CvBuildingEntry::IsAirlift() const
{
	return m_bAirlift;
}


int CvBuildingEntry::GetAirModifier() const
{
	return m_iAirModifier;
}


int CvBuildingEntry::GetNukeModifier() const
{
	return m_iNukeModifier;
}


int CvBuildingEntry::GetNukeExplosionRand() const
{
	return m_iNukeExplosionRand;
}


int CvBuildingEntry::GetWorkerSpeedModifier() const
{
	return m_iWorkerSpeedModifier;
}


int CvBuildingEntry::GetMilitaryProductionModifier() const
{
	return m_iMilitaryProductionModifier;
}

#if defined(LEKMOD_BUILDING_MILITARY_PRODUCTION_MOD)

int CvBuildingEntry::GetMilitaryProductionMod() const
{
	return m_iMilitaryProductionMod;
}
#endif


int CvBuildingEntry::GetSpaceProductionModifier() const
{
	return m_iSpaceProductionModifier;
}


int CvBuildingEntry::GetGlobalSpaceProductionModifier() const
{
	return m_iGlobalSpaceProductionModifier;
}


int CvBuildingEntry::GetBuildingProductionModifier() const
{
	return m_iBuildingProductionModifier;
}


int CvBuildingEntry::GetWonderProductionModifier() const
{
	return m_iWonderProductionModifier;
}


int CvBuildingEntry::GetCityConnectionTradeRouteModifier() const
{
	return m_iCityConnectionTradeRouteModifier;
}


int CvBuildingEntry::GetCapturePlunderModifier() const
{
	return m_iCapturePlunderModifier;
}


int CvBuildingEntry::GetPolicyCostModifier() const
{
	return m_iPolicyCostModifier;
}


int CvBuildingEntry::GetPlotCultureCostModifier() const
{
	return m_iPlotCultureCostModifier;
}


int CvBuildingEntry::GetGlobalPlotCultureCostModifier() const
{
	return m_iGlobalPlotCultureCostModifier;
}


int CvBuildingEntry::GetPlotBuyCostModifier() const
{
	return m_iPlotBuyCostModifier;
}


int CvBuildingEntry::GetGlobalPlotBuyCostModifier() const
{
	return m_iGlobalPlotBuyCostModifier;
}


int CvBuildingEntry::GetMinAreaSize() const
{
	return m_iMinAreaSize;
}


int CvBuildingEntry::GetConquestProbability() const
{
	return m_iConquestProbability;
}


int CvBuildingEntry::GetHealRateChange() const
{
	return m_iHealRateChange;
}


int CvBuildingEntry::GetHappiness() const
{
	return m_iHappiness;
}


int CvBuildingEntry::GetUnmoddedHappiness() const
{
	return m_iUnmoddedHappiness;
}


int CvBuildingEntry::GetUnhappinessModifier() const
{
	return m_iUnhappinessModifier;
}


int CvBuildingEntry::GetHappinessPerCity() const
{
	return m_iHappinessPerCity;
}


int CvBuildingEntry::GetHappinessPerXPolicies() const
{
	return m_iHappinessPerXPolicies;
}


int CvBuildingEntry::GetCityCountUnhappinessMod() const
{
	return m_iCityCountUnhappinessMod;
}


bool CvBuildingEntry::IsNoOccupiedUnhappiness() const
{
	return m_bNoOccupiedUnhappiness;
}


int CvBuildingEntry::GetGlobalPopulationChange() const
{
	return m_iGlobalPopulationChange;
}

#ifdef NQ_LOCAL_POPULATION_CHANGE_FROM_BUILDING

int CvBuildingEntry::GetLocalPopulationChange() const
{
	return m_iLocalPopulationChange;
}
#endif


int CvBuildingEntry::GetTechShare() const
{
	return m_iTechShare;
}


int CvBuildingEntry::GetFreeTechs() const
{
	return m_iFreeTechs;
}


int CvBuildingEntry::GetFreePolicies() const
{
	return m_iFreePolicies;
}



int CvBuildingEntry::GetFreeFlatFaith() const
{
	return m_iFreeFlatFaith;
}



int CvBuildingEntry::GetFreeGreatPeople() const
{
	return m_iFreeGreatPeople;
}


int CvBuildingEntry::GetMedianTechPercentChange() const
{
	return m_iMedianTechPercentChange;
}


int CvBuildingEntry::GetGold() const
{
	return m_iGold;
}


bool CvBuildingEntry::IsNearbyMountainRequired() const
{
	return m_bNearbyMountainRequired;
}


bool CvBuildingEntry::IsAllowsRangeStrike() const
{
	return m_bAllowsRangeStrike;
}


int CvBuildingEntry::GetDefenseModifier() const
{
	return m_iDefenseModifier;
}

#ifdef NQ_BUILDING_DEFENSE_FROM_CITIZENS

int CvBuildingEntry::GetDefensePerCitizen() const
{
	return m_iDefensePerCitizen;
}
#endif



int CvBuildingEntry::GetGlobalDefenseModifier() const
{
	return m_iGlobalDefenseModifier;
}


int CvBuildingEntry::GetExtraCityHitPoints() const
{
	return m_iExtraCityHitPoints;
}


int CvBuildingEntry::GetMinorFriendshipChange() const
{
	return m_iMinorFriendshipChange;
}


int CvBuildingEntry::GetVictoryPoints() const
{
	return m_iVictoryPoints;
}


int CvBuildingEntry::GetExtraMissionarySpreads() const
{
	return m_iExtraMissionarySpreads;
}


int CvBuildingEntry::GetReligiousPressureModifier() const
{
	return m_iReligiousPressureModifier;
}

int CvBuildingEntry::GetEspionageModifier() const
{
	return m_iEspionageModifier;
}


int CvBuildingEntry::GetGlobalEspionageModifier() const
{
	return m_iGlobalEspionageModifier;
}


int CvBuildingEntry::GetExtraSpies() const
{
	return m_iExtraSpies;
}


int CvBuildingEntry::GetSpyRankChange() const
{
	return m_iSpyRankChange;
}


int CvBuildingEntry::GetTradeRouteRecipientBonus() const
{
	return m_iTradeRouteRecipientBonus;
}


int CvBuildingEntry::GetTradeRouteTargetBonus() const
{
	return m_iTradeRouteTargetBonus;
}

int CvBuildingEntry::GetNumTradeRouteBonus() const
{
	return m_iNumTradeRouteBonus;
}

int CvBuildingEntry::GetTradeRouteSeaDistanceModifier() const
{
	return m_iTradeRouteSeaDistanceModifier;
}

int CvBuildingEntry::GetTradeRouteSeaGoldBonus() const
{
	return m_iTradeRouteSeaGoldBonus;
}

int CvBuildingEntry::GetTradeRouteLandDistanceModifier() const
{
	return m_iTradeRouteLandDistanceModifier;
}

int CvBuildingEntry::GetTradeRouteLandGoldBonus() const
{
	return m_iTradeRouteLandGoldBonus;
}

int CvBuildingEntry::GetCityStateTradeRouteProductionModifier() const
{
	return m_iCityStateTradeRouteProductionModifier;
}


int CvBuildingEntry::GetCityStateTradeRouteGoldModifier() const
{
	return m_iCityStateTradeRouteGoldModifier;
}


int CvBuildingEntry::GetGreatScientistBeakerModifier() const
{
	return m_iGreatScientistBeakerModifier;
}


int CvBuildingEntry::GetInstantSpyRankChange() const
{
	return m_iInstantSpyRankChange;
}


int CvBuildingEntry::GetLandmarksTourismPercent() const
{
	return m_iLandmarksTourismPercent;
}


int CvBuildingEntry::GetInstantMilitaryIncrease() const
{
	return m_iInstantMilitaryIncrease;
}


int CvBuildingEntry::GetGreatWorksTourismModifier() const
{
	return m_iGreatWorksTourismModifier;
}


int CvBuildingEntry::GetXBuiltTriggersIdeologyChoice() const
{
	return m_iXBuiltTriggersIdeologyChoice;
}


int CvBuildingEntry::GetExtraLeagueVotes() const
{
	return m_iExtraLeagueVotes;
}
#if defined(LEKMOD_GARRISON_YIELD_EFFECTS)

int CvBuildingEntry::GetGarrisonStrengthBonus() const
{
	return m_iGarrisonStrengthBonus;
}
#endif
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)

int CvBuildingEntry::GetGreatWorkHappiness() const
{
	return m_iGreatWorkHappiness;
}
#endif

int CvBuildingEntry::GetPreferredDisplayPosition() const
{
	return m_iPreferredDisplayPosition;
}


int CvBuildingEntry::GetPortraitIndex() const
{
	return m_iPortraitIndex;
}


bool CvBuildingEntry::IsTeamShare() const
{
	return m_bTeamShare;
}


bool CvBuildingEntry::IsWater() const
{
	return m_bWater;
}


bool CvBuildingEntry::IsRiver() const
{
	return m_bRiver;
}


bool CvBuildingEntry::IsFreshWater() const
{
	return m_bFreshWater;
}
#if defined(LEKMOD_BUILDING_LAKE_REQ)
bool CvBuildingEntry::IsLake() const
{
	return m_bLake;
}
#endif

bool CvBuildingEntry::IsMountain() const
{
	return m_bMountain;
}


bool CvBuildingEntry::IsHill() const
{
	return m_bHill;
}


bool CvBuildingEntry::IsFlat() const
{
	return m_bFlat;
}


bool CvBuildingEntry::IsFoundsReligion() const
{
	return m_bFoundsReligion;
}


bool CvBuildingEntry::IsReligious() const
{
	return m_bIsReligious;
}


bool CvBuildingEntry::IsBorderObstacle() const
{
	return m_bBorderObstacle;
}


bool CvBuildingEntry::IsPlayerBorderObstacle() const
{
	return m_bPlayerBorderObstacle;
}


bool CvBuildingEntry::IsCityWall() const
{
	return m_bCityWall;
}


bool CvBuildingEntry::IsUnlockedByBelief() const
{
	return m_bUnlockedByBelief;
}


bool CvBuildingEntry::IsUnlockedByLeague() const
{
	return m_bUnlockedByLeague;
}


bool CvBuildingEntry::IsRequiresHolyCity() const
{
	return m_bRequiresHolyCity;
}


bool CvBuildingEntry::AffectSpiesNow() const
{
	return m_bAffectSpiesNow;
}


bool CvBuildingEntry::IsEspionage() const
{
	return m_bEspionage;
}

#ifdef NQ_MALI_TREASURY

bool CvBuildingEntry::IsMalianTreasury() const
{
	return m_bMalianTreasury;
}
#endif

bool CvBuildingEntry::AllowsFoodTradeRoutes() const
{
	return m_bAllowsFoodTradeRoutes;
}

bool CvBuildingEntry::AllowsProductionTradeRoutes() const
{
	return m_bAllowsProductionTradeRoutes;
}

bool CvBuildingEntry::NullifyInfluenceModifier() const
{
	return m_bNullifyInfluenceModifier;
}


bool CvBuildingEntry::IsCapital() const
{
	return m_bCapital;
}


bool CvBuildingEntry::IsGoldenAge() const
{
	return m_bGoldenAge;
}



bool CvBuildingEntry::IsGrantsFreeCulturalGreatPersonWithTrait() const
{
	return m_bGrantsFreeCulturalGreatPersonWithTrait;
}



bool CvBuildingEntry::IsMapCentering() const
{
	return m_bMapCentering;
}


bool CvBuildingEntry::IsNeverCapture() const
{
	return m_bNeverCapture;
}


bool CvBuildingEntry::IsNukeImmune() const
{
	return m_bNukeImmune;
}


bool CvBuildingEntry::IsExtraLuxuries() const
{
	return m_bExtraLuxuries;
}


bool CvBuildingEntry::IsDiplomaticVoting() const
{
	return m_bDiplomaticVoting;
}


bool CvBuildingEntry::AllowsWaterRoutes() const
{
	return m_bAllowsWaterRoutes;
}


bool CvBuildingEntry::IsScienceBuilding() const
{
	bool bRtnValue = false;

	if(IsCapital())
	{
		bRtnValue = false;
	}
	else if(GetYieldChange(YIELD_SCIENCE) > 0)
	{
		bRtnValue = true;
	}
	else if(GetYieldChangePerPop(YIELD_SCIENCE) > 0)
	{
		bRtnValue = true;
	}
	else if(GetYieldChangePerReligion(YIELD_SCIENCE) > 0)
	{
		bRtnValue = true;
	}
	else if(GetTechEnhancedYieldChange(YIELD_SCIENCE) > 0)
	{
		bRtnValue = true;
	}
	else if(GetYieldModifier(YIELD_SCIENCE) > 0)
	{
		bRtnValue = true;
	}

	return bRtnValue;
}


const char* CvBuildingEntry::GetArtDefineTag() const
{
	return m_strArtDefineTag.c_str();
}


void CvBuildingEntry::SetArtDefineTag(const char* szVal)
{
	m_strArtDefineTag = szVal;
}


const bool CvBuildingEntry::GetArtInfoCulturalVariation() const
{
	return m_bArtInfoCulturalVariation;
}


const bool CvBuildingEntry::GetArtInfoEraVariation() const
{
	return m_bArtInfoEraVariation;
}


const bool CvBuildingEntry::GetArtInfoRandomVariation() const
{
	return m_bArtInfoRandomVariation;
}

const char* CvBuildingEntry::GetWonderSplashAudio() const
{
	return m_strWonderSplashAudio.c_str();
}

CvString CvBuildingEntry::GetThemingBonusHelp() const
{
	return m_strThemingBonusHelp;
}




int CvBuildingEntry::GetYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChange ? m_piYieldChange[i] : -1;
}


int* CvBuildingEntry::GetYieldChangeArray() const
{
	return m_piYieldChange;
}


int CvBuildingEntry::GetYieldChangePerPop(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChangePerPop ? m_piYieldChangePerPop[i] : -1;
}


int* CvBuildingEntry::GetYieldChangePerPopArray() const
{
	return m_piYieldChangePerPop;
}


int CvBuildingEntry::GetYieldChangePerReligion(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChangePerReligion ? m_piYieldChangePerReligion[i] : -1;
}


int* CvBuildingEntry::GetYieldChangePerReligionArray() const
{
	return m_piYieldChangePerReligion;
}


int CvBuildingEntry::GetYieldModifier(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldModifier ? m_piYieldModifier[i] : -1;
}


int* CvBuildingEntry::GetYieldModifierArray() const
{
	return m_piYieldModifier;
}


int CvBuildingEntry::GetAreaYieldModifier(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piAreaYieldModifier ? m_piAreaYieldModifier[i] : -1;
}


int* CvBuildingEntry::GetAreaYieldModifierArray() const
{
	return m_piAreaYieldModifier;
}


int CvBuildingEntry::GetGlobalYieldModifier(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piGlobalYieldModifier ? m_piGlobalYieldModifier[i] : -1;
}


int* CvBuildingEntry::GetGlobalYieldModifierArray() const
{
	return m_piGlobalYieldModifier;
}


int CvBuildingEntry::GetTechEnhancedYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piTechEnhancedYieldChange ? m_piTechEnhancedYieldChange[i] : -1;
}


int* CvBuildingEntry::GetTechEnhancedYieldChangeArray() const
{
	return m_piTechEnhancedYieldChange;
}
#if defined(LEKMOD_ERA_ENHANCED_YIELDS)

int CvBuildingEntry::GetEraEnhancedYieldChange(int i, int j) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_ppiEraEnhancedYieldChange ? m_ppiEraEnhancedYieldChange[i][j] : -1;
}

int** CvBuildingEntry::GetEraEnhancedYieldChangeArray() const
{
	return m_ppiEraEnhancedYieldChange;
}
#endif

int CvBuildingEntry::GetSeaPlotYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piSeaPlotYieldChange ? m_piSeaPlotYieldChange[i] : -1;
}


int* CvBuildingEntry::GetSeaPlotYieldChangeArray() const
{
	return m_piSeaPlotYieldChange;
}


int CvBuildingEntry::GetRiverPlotYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piRiverPlotYieldChange ? m_piRiverPlotYieldChange[i] : -1;
}


int* CvBuildingEntry::GetRiverPlotYieldChangeArray() const
{
	return m_piRiverPlotYieldChange;
}


int CvBuildingEntry::GetLakePlotYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piLakePlotYieldChange ? m_piLakePlotYieldChange[i] : -1;
}


int* CvBuildingEntry::GetLakePlotYieldChangeArray() const
{
	return m_piLakePlotYieldChange;
}


int CvBuildingEntry::GetSeaResourceYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piSeaResourceYieldChange ? m_piSeaResourceYieldChange[i] : -1;
}


int* CvBuildingEntry::GetSeaResourceYieldChangeArray() const
{
	return m_piSeaResourceYieldChange;
}


int CvBuildingEntry::GetUnitCombatFreeExperience(int i) const
{
	CvAssertMsg(i < GC.getNumUnitCombatClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piUnitCombatFreeExperience ? m_piUnitCombatFreeExperience[i] : -1;
}


int CvBuildingEntry::GetUnitCombatProductionModifier(int i) const
{
	CvAssertMsg(i < GC.getNumUnitCombatClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piUnitCombatProductionModifiers ? m_piUnitCombatProductionModifiers[i] : -1;
}


int CvBuildingEntry::GetDomainFreeExperience(int i) const
{
	CvAssertMsg(i < NUM_DOMAIN_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piDomainFreeExperience ? m_piDomainFreeExperience[i] : -1;
}


int CvBuildingEntry::GetDomainFreeExperiencePerGreatWork(int i) const
{
	CvAssertMsg(i < NUM_DOMAIN_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piDomainFreeExperiencePerGreatWork ? m_piDomainFreeExperiencePerGreatWork[i] : -1;
}
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)

int CvBuildingEntry::GetGreatWorkMilitaryProductionModifier() const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_iGreatWorkMilitaryProductionModifier;
}

int CvBuildingEntry::GetBuildingGreatWorkYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piBuildingGreatWorkYieldChange ? m_piBuildingGreatWorkYieldChange[i] : 0;
}

int CvBuildingEntry::GetCityGreatWorkYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piCityGreatWorkYieldChange ? m_piCityGreatWorkYieldChange[i] : 0;
}
int CvBuildingEntry::GetCityGreatWorkClassYieldChange(int i, int j) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiCityGreatWorkClassYieldChange ? m_ppiCityGreatWorkClassYieldChange[i][j] : 0;
}
#endif

int CvBuildingEntry::GetDomainProductionModifier(int i) const
{
	CvAssertMsg(i < NUM_DOMAIN_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piDomainProductionModifier ? m_piDomainProductionModifier[i] : -1;
}


int CvBuildingEntry::GetLockedBuildingClasses(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piLockedBuildingClasses ? m_piLockedBuildingClasses[i] : -1;
}


int CvBuildingEntry::GetPrereqAndTechs(int i) const
{
	CvAssertMsg(i < GC.getNUM_BUILDING_AND_TECH_PREREQS(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piPrereqAndTechs ? m_piPrereqAndTechs[i] : -1;
}


int CvBuildingEntry::GetResourceQuantityRequirement(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piResourceQuantityRequirements ? m_piResourceQuantityRequirements[i] : -1;
}


int CvBuildingEntry::GetResourceQuantity(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piResourceQuantity ? m_piResourceQuantity[i] : -1;
}
#if !defined(LEKMOD_FIX_BUILDING_RESOURCE_YIELD_CHANGE)

int CvBuildingEntry::GetResourceCultureChange(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piResourceCultureChanges ? m_piResourceCultureChanges[i] : -1;
}


int CvBuildingEntry::GetResourceFaithChange(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piResourceFaithChanges ? m_piResourceFaithChanges[i] : -1;
}
#else

int CvBuildingEntry::GetBuildingLocalResourceYieldChange(int i, int j) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	return m_paiBuildingLocalResourceYieldChanges ? m_paiBuildingLocalResourceYieldChanges[i][j] : 0;
}
#endif

int CvBuildingEntry::GetProductionTraits(int i) const
{
	CvAssertMsg(i < GC.getNumTraitInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piProductionTraits ? m_piProductionTraits[i] : 0;
}


int CvBuildingEntry::GetPrereqNumOfBuildingClass(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piPrereqNumOfBuildingClass ? m_piPrereqNumOfBuildingClass[i] : -1;
}


int CvBuildingEntry::GetFlavorValue(int i) const
{
	CvAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}


int CvBuildingEntry::GetLocalResourceAnd(int i) const
{
	CvAssertMsg(i < GC.getNUM_BUILDING_RESOURCE_PREREQS(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piLocalResourceAnds ? m_piLocalResourceAnds[i] : -1;
}


int CvBuildingEntry::GetLocalResourceOr(int i) const
{
	CvAssertMsg(i < GC.getNUM_BUILDING_RESOURCE_PREREQS(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piLocalResourceOrs ? m_piLocalResourceOrs[i] : -1;
}


int CvBuildingEntry::GetHurryModifier(int i) const
{
	CvAssertMsg(i < GC.getNumHurryInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiHurryModifier ? m_paiHurryModifier[i] : -1;
}


bool CvBuildingEntry::IsBuildingClassNeededInCity(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbBuildingClassNeededInCity ? m_pbBuildingClassNeededInCity[i] : false;
}


int CvBuildingEntry::GetNumFreeUnits(int i) const
{
	CvAssertMsg(i < GC.getNumUnitInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piNumFreeUnits ? m_piNumFreeUnits[i] : -1;
}


int CvBuildingEntry::GetResourceYieldChange(int i, int j) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppaiResourceYieldChange.first ? (m_ppaiResourceYieldChange.first)[i][j] : -1;
#else
	return m_ppaiResourceYieldChange ? m_ppaiResourceYieldChange[i][j] : -1;
#endif
}


int* CvBuildingEntry::GetResourceYieldChangeArray(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppaiResourceYieldChange.first ? m_ppaiResourceYieldChange.first[i] : NULL;
#else
	return m_ppaiResourceYieldChange[i];
#endif
}

int CvBuildingEntry::GetResourceYieldChangeGlobal(int iResource, int iYieldType) const
{
	CvAssertMsg(iResource < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(iResource > -1, "Index out of bounds");
	CvAssertMsg(iYieldType < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(iYieldType > -1, "Index out of bounds");
	std::map<int, std::map<int, int>>::const_iterator itResource = m_ppiResourceYieldChangeGlobal.find(iResource);
	if (itResource != m_ppiResourceYieldChangeGlobal.end())
	{
		std::map<int, int>::const_iterator itYield = itResource->second.find(iYieldType);
		if (itYield != itResource->second.end())
		{
			return itYield->second;
		}
	}

	return 0;
}
#if defined(TRADE_REFACTOR)
int CvBuildingEntry::GetTradeConnectionOriginLandYieldChange(int i, int j) const
{
	CvAssertMsg(i < NUM_TRADE_CONNECTION_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppaiTradeConnectionOriginLandYieldChange ? m_ppaiTradeConnectionOriginLandYieldChange[i][j] : 0;
}
int CvBuildingEntry::GetTradeConnectionOriginSeaYieldChange(int i, int j) const
{
	CvAssertMsg(i < NUM_TRADE_CONNECTION_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppaiTradeConnectionOriginSeaYieldChange ? m_ppaiTradeConnectionOriginSeaYieldChange[i][j] : 0;
}
int CvBuildingEntry::GetTradeConnectionDestinationLandYieldChange(int i, int j) const
{
	CvAssertMsg(i < NUM_TRADE_CONNECTION_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppaiTradeConnectionDestinationLandYieldChange ? m_ppaiTradeConnectionDestinationLandYieldChange[i][j] : 0;
}
int CvBuildingEntry::GetTradeConnectionDestinationSeaYieldChange(int i, int j) const
{
	CvAssertMsg(i < NUM_TRADE_CONNECTION_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppaiTradeConnectionDestinationSeaYieldChange ? m_ppaiTradeConnectionDestinationSeaYieldChange[i][j] : 0;
}
int CvBuildingEntry::GetIncomingTradeConnectionLandYieldChange(int i, int j) const
{
	CvAssertMsg(i < NUM_TRADE_CONNECTION_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppaiIncomingTradeConnectionLandYieldChange ? m_ppaiIncomingTradeConnectionLandYieldChange[i][j] : 0;
}
int CvBuildingEntry::GetIncomingTradeConnectionSeaYieldChange(int i, int j) const
{
	CvAssertMsg(i < NUM_TRADE_CONNECTION_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppaiIncomingTradeConnectionSeaYieldChange ? m_ppaiIncomingTradeConnectionSeaYieldChange[i][j] : 0;
}
#endif
#if defined(MISC_CHANGES)

int CvBuildingEntry::GetResourceClassYieldChange(int i, int j) const
{
	CvAssertMsg(i < GC.getNumResourceClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppaiResourceClassYieldChange ? m_ppaiResourceClassYieldChange[i][j] : -1;
}
#endif
#if defined(LEKMOD_v34)

int CvBuildingEntry::GetGarrisonYieldChange(int j) const
{
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_piGarrisonYieldChange ? m_piGarrisonYieldChange[j] : -1;
}
#endif

int CvBuildingEntry::GetFeatureYieldChange(int i, int j) const
{
	CvAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppaiFeatureYieldChange.first ? (m_ppaiFeatureYieldChange.first)[i][j] : -1;
#else
	return m_ppaiFeatureYieldChange ? m_ppaiFeatureYieldChange[i][j] : -1;
#endif
}


int* CvBuildingEntry::GetFeatureYieldChangeArray(int i) const
{
	CvAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppaiFeatureYieldChange.first ? m_ppaiFeatureYieldChange.first[i] : NULL;
#else
	return m_ppaiFeatureYieldChange[i];
#endif
}

int CvBuildingEntry::GetImprovementYieldChange(int i, int j) const
{
	CvAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppaiImprovementYieldChange ? m_ppaiImprovementYieldChange[i][j] : -1;
}


int* CvBuildingEntry::GetImprovementYieldChangeArray(int i) const
{
	CvAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_ppaiImprovementYieldChange[i];
}

int CvBuildingEntry::GetImprovementYieldChangeGlobal(int i, int j) const
{
	CvAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppaiImprovementYieldChangeGlobal ? m_ppaiImprovementYieldChangeGlobal[i][j] : -1;
}


int* CvBuildingEntry::GetImprovementYieldChangeGlobalArray(int i) const
{
	CvAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_ppaiImprovementYieldChangeGlobal[i];
}


#ifdef AUI_WARNING_FIXES
int CvBuildingEntry::GetSpecialistYieldChange(uint i, int j) const
#else
int CvBuildingEntry::GetSpecialistYieldChange(int i, int j) const
#endif
{
	CvAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
#ifndef AUI_WARNING_FIXES
	CvAssertMsg(i > -1, "Index out of bounds");
#endif
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppaiSpecialistYieldChange.first ? (m_ppaiSpecialistYieldChange.first)[i][j] : -1;
#else
	return m_ppaiSpecialistYieldChange ? m_ppaiSpecialistYieldChange[i][j] : -1;
#endif
}


int* CvBuildingEntry::GetSpecialistYieldChangeArray(int i) const
{
	CvAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppaiSpecialistYieldChange.first ? m_ppaiSpecialistYieldChange.first[i] : NULL;
#else
	return m_ppaiSpecialistYieldChange[i];
#endif
}


int CvBuildingEntry::GetResourceYieldModifier(int i, int j) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppaiResourceYieldModifier.first ? (m_ppaiResourceYieldModifier.first)[i][j] : -1;
#else
	return m_ppaiResourceYieldModifier ? m_ppaiResourceYieldModifier[i][j] : -1;
#endif
}


int* CvBuildingEntry::GetResourceYieldModifierArray(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppaiResourceYieldModifier.first ? m_ppaiResourceYieldModifier.first[i] : NULL;
#else
	return m_ppaiResourceYieldModifier[i];
#endif
}


int CvBuildingEntry::GetTerrainYieldChange(int i, int j) const
{
	CvAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppaiTerrainYieldChange.first ? (m_ppaiTerrainYieldChange.first)[i][j] : -1;
#else
	return m_ppaiTerrainYieldChange ? m_ppaiTerrainYieldChange[i][j] : -1;
#endif
}


int* CvBuildingEntry::GetTerrainYieldChangeArray(int i) const
{
	CvAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppaiTerrainYieldChange.first ? m_ppaiTerrainYieldChange.first[i] : NULL;
#else
	return m_ppaiTerrainYieldChange[i];
#endif
}


int CvBuildingEntry::GetBuildingClassYieldChange(int i, int j) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiBuildingClassYieldChanges ? m_ppiBuildingClassYieldChanges[i][j] : -1;
}

#if defined(LEKMOD_AREA_BASED_CITY_YIELD)

int CvBuildingEntry::GetSameLandMassYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piSameLandMassYieldChange ? m_piSameLandMassYieldChange[i] : 0;
}


int CvBuildingEntry::GetDifferentLandMassYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piDifferentLandMassYieldChange ? m_piDifferentLandMassYieldChange[i] : 0;
}
#endif
#if defined(LEKMOD_NEARBY_TERRAIN_FREE_YIELDS)

const std::vector<BuildingFreeTerrainYields>& CvBuildingEntry::GetFreeTerrainYields() const
{
	return m_aFreeTerrainYields;
}
#endif
#ifdef LEKMOD_BUILDING_GP_EXPEND_YIELD

int CvBuildingEntry::GetGreatPersonExpendYield(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piGreatPersonExpendYield[i];
}
#endif

int CvBuildingEntry::GetBuildingClassHappiness(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiBuildingClassHappiness ? m_paiBuildingClassHappiness[i] : -1;
}

CvThemingBonusInfo *CvBuildingEntry::GetThemingBonusInfo(int i) const
{
	CvAssertMsg(i < MAX_THEMING_BONUSES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if (m_paThemingBonusInfo[0].m_iBonus == 0)
	{
		return NULL;
	}
	else
	{
		return &m_paThemingBonusInfo[i];
	}
}

#ifdef LEKMOD_v34

int CvBuildingEntry::GetUnitCombatProductionCostModifier(int i) const
{
	CvAssertMsg(i < GC.getNumUnitCombatClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piUnitCombatProductionCostModifiers ? m_piUnitCombatProductionCostModifiers[i] : 0;
}


int CvBuildingEntry::GetUnitCombatExtraProduction(int i) const
{
	CvAssertMsg(i < GC.getNumUnitCombatClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piUnitCombatExtraProduction ? m_piUnitCombatExtraProduction[i] : 0;
}
#endif





CvBuildingXMLEntries::CvBuildingXMLEntries(void)
{

}


CvBuildingXMLEntries::~CvBuildingXMLEntries(void)
{
	DeleteArray();
}


std::vector<CvBuildingEntry*>& CvBuildingXMLEntries::GetBuildingEntries()
{
	return m_paBuildingEntries;
}


#ifdef AUI_WARNING_FIXES
uint CvBuildingXMLEntries::GetNumBuildings() const
#else
int CvBuildingXMLEntries::GetNumBuildings()
#endif
{
	return m_paBuildingEntries.size();
}


void CvBuildingXMLEntries::DeleteArray()
{
	for(std::vector<CvBuildingEntry*>::iterator it = m_paBuildingEntries.begin(); it != m_paBuildingEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paBuildingEntries.clear();
}


#ifdef AUI_WARNING_FIXES
_Ret_maybenull_ CvBuildingEntry* CvBuildingXMLEntries::GetEntry(uint index)
#else
CvBuildingEntry* CvBuildingXMLEntries::GetEntry(int index)
#endif
{
	return m_paBuildingEntries[index];
}





#ifdef AUI_CITY_FIX_COMPONENT_CONSTRUCTORS_CONTAIN_POINTERS
CvCityBuildings::CvCityBuildings(CvCity* pCity) :
#else
CvCityBuildings::CvCityBuildings():
#endif
	m_paiBuildingProduction(NULL),
	m_paiBuildingProductionTime(NULL),
	m_paiBuildingOriginalOwner(NULL),
	m_paiBuildingOriginalTime(NULL),
	m_paiNumRealBuilding(NULL),
	m_paiNumFreeBuilding(NULL),
	m_iNumBuildings(0),
	m_iBuildingProductionModifier(0),
#if defined(LEKMOD_AREA_BASED_CITY_YIELD)
	m_paiSameLandMassYieldChange(NULL),
	m_paiDifferentLandMassYieldChange(NULL),
#endif
#if defined(LEKMOD_NEARBY_TERRAIN_FREE_YIELDS)
	m_paiFreeTerrainYieldChange(NULL),
#endif
	m_iBuildingDefense(0),
#if defined(LEKMOD_GARRISON_YIELD_EFFECTS)
	m_iBuildingGarrisonStrengthBonus(0),
	m_iGarrisonMaintenanceFreeCount(0),
#endif
#ifdef NQ_BUILDING_DEFENSE_FROM_CITIZENS
	m_iBuildingDefensePerCitizen(0),
#endif
	m_iBuildingDefenseMod(0),
	m_iMissionaryExtraSpreads(0),
	m_iLandmarksTourismPercent(0),
	m_iGreatWorksTourismModifier(0),
	m_bSoldBuildingThisTurn(false),
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	m_bGreatWorkClassMapDirty(true),
	m_iHappinessFromGreatWorks(0),
#endif
#if defined(LEKMOD_BUILDING_FIRST_PURCHASE_DISCOUNT)
	m_iFirstPurchaseDiscount(0),
#endif

	m_pBuildings(NULL),
#ifdef AUI_CITY_FIX_COMPONENT_CONSTRUCTORS_CONTAIN_POINTERS
	m_pCity(pCity)
#else
	m_pCity(NULL)
#endif
{
}


CvCityBuildings::~CvCityBuildings(void)
{
}


void CvCityBuildings::Init(CvBuildingXMLEntries* pBuildings, CvCity* pCity)
{

	m_pBuildings = pBuildings;
	m_pCity = pCity;



	int iNumBuildings = m_pBuildings->GetNumBuildings();

	CvAssertMsg((0 < iNumBuildings),  "m_pBuildings->GetNumBuildings() is not greater than zero but an array is being allocated in CvCityBuildings::Init");

	CvAssertMsg(m_paiBuildingProduction==NULL, "about to leak memory, CvCityBuildings::m_paiBuildingProduction");
	m_paiBuildingProduction = FNEW(int[iNumBuildings], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paiBuildingProductionTime==NULL, "about to leak memory, CvCityBuildings::m_paiBuildingProductionTime");
	m_paiBuildingProductionTime = FNEW(int[iNumBuildings], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paiBuildingOriginalOwner==NULL, "about to leak memory, CvCityBuildings::m_paiBuildingOriginalOwner");
	m_paiBuildingOriginalOwner = FNEW(int[iNumBuildings], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paiBuildingOriginalTime==NULL, "about to leak memory, CvCityBuildings::m_paiBuildingOriginalTime");
	m_paiBuildingOriginalTime = FNEW(int[iNumBuildings], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paiNumRealBuilding==NULL, "about to leak memory, CvCityBuildings::m_paiNumRealBuilding");
	m_paiNumRealBuilding = FNEW(int[iNumBuildings], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paiNumFreeBuilding==NULL, "about to leak memory, CvCityBuildings::m_paiNumFreeBuilding");
	m_paiNumFreeBuilding = FNEW(int[iNumBuildings], c_eCiv5GameplayDLL, 0);
#if defined(LEKMOD_AREA_BASED_CITY_YIELD)
	CvAssertMsg(m_paiSameLandMassYieldChange==NULL, "about to leak memory, CvCityBuildings::m_paiSameLandMassYieldChange");
	m_paiSameLandMassYieldChange = FNEW(int[NUM_YIELD_TYPES], c_eCiv5GameplayDLL, 0);
	CvAssertMsg(m_paiDifferentLandMassYieldChange == NULL, "about to leak memory, CvCityBuildings::m_paiDifferentLandMassYieldChange");
	m_paiDifferentLandMassYieldChange = FNEW(int[NUM_YIELD_TYPES], c_eCiv5GameplayDLL, 0);
#endif

#if defined(LEKMOD_AREA_BASED_CITY_YIELD)
	CvAssertMsg(m_paiSameLandMassYieldChange == NULL, "about to leak memory, CvCityBuildings::m_paiSameLandMassYieldChange");
	m_paiSameLandMassYieldChange = FNEW(int[NUM_YIELD_TYPES], c_eCiv5GameplayDLL, 0);
	CvAssertMsg(m_paiDifferentLandMassYieldChange == NULL, "about to leak memory, CvCityBuildings::m_paiDifferentLandMassYieldChange");
	m_paiDifferentLandMassYieldChange = FNEW(int[NUM_YIELD_TYPES], c_eCiv5GameplayDLL, 0);
#endif
#if defined(LEKMOD_NEARBY_TERRAIN_FREE_YIELDS)
	CvAssertMsg(m_paiFreeTerrainYieldChange == NULL, "about to leak memory, CvCityBuildings::m_paiFreeTerrainYieldChange");
	m_paiFreeTerrainYieldChange = FNEW(int[NUM_YIELD_TYPES], c_eCiv5GameplayDLL, 0);
#endif

	m_aBuildingYieldChange.clear();
	m_aBuildingGreatWork.clear();


	Reset();
}


void CvCityBuildings::Uninit()
{
	SAFE_DELETE_ARRAY(m_paiBuildingProduction);
	SAFE_DELETE_ARRAY(m_paiBuildingProductionTime);
	SAFE_DELETE_ARRAY(m_paiBuildingOriginalOwner);
	SAFE_DELETE_ARRAY(m_paiBuildingOriginalTime);
	SAFE_DELETE_ARRAY(m_paiNumRealBuilding);
	SAFE_DELETE_ARRAY(m_paiNumFreeBuilding);
#if defined(LEKMOD_AREA_BASED_CITY_YIELD)
	SAFE_DELETE_ARRAY(m_paiSameLandMassYieldChange);
	SAFE_DELETE_ARRAY(m_paiDifferentLandMassYieldChange);
#endif
#if defined(LEKMOD_NEARBY_TERRAIN_FREE_YIELDS)
	SAFE_DELETE_ARRAY(m_paiFreeTerrainYieldChange);
#endif
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	m_aaiCityGreatWorkClassYieldChange.clear();
#endif
}


void CvCityBuildings::Reset()
{
#ifdef AUI_WARNING_FIXES
	uint iI;
#else
	int iI;
#endif


	m_iNumBuildings = 0;
	m_iBuildingProductionModifier = 0;

	for (int i = 0; i < NUM_YIELD_TYPES; i++)
	{
#if defined(LEKMOD_AREA_BASED_CITY_YIELD)
		m_paiSameLandMassYieldChange[i] = 0;
		m_paiDifferentLandMassYieldChange[i] = 0;
#endif
#if defined(LEKMOD_NEARBY_TERRAIN_FREE_YIELDS)
		m_paiFreeTerrainYieldChange[i] = 0;
#endif
	}
	m_iBuildingDefense = 0;
#if defined(LEKMOD_GARRISON_YIELD_EFFECTS)
	m_iBuildingGarrisonStrengthBonus = 0;
	m_iGarrisonMaintenanceFreeCount = 0;
#endif
#ifdef NQ_BUILDING_DEFENSE_FROM_CITIZENS
	m_iBuildingDefensePerCitizen = 0;
#endif
	m_iBuildingDefenseMod = 0;
#if defined(LEKMOD_BUILDING_FIRST_PURCHASE_DISCOUNT)
	m_iFirstPurchaseDiscount = 0;
#endif
	m_iMissionaryExtraSpreads = 0;
	m_iLandmarksTourismPercent = 0;
	m_iGreatWorksTourismModifier = 0;

	m_bSoldBuildingThisTurn = false;
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	m_bGreatWorkClassMapDirty = true;
	m_iHappinessFromGreatWorks = 0;
	m_aaiCityGreatWorkClassYieldChange.clear();
	m_aaiCityGreatWorkClassYieldChange.resize(GC.getNumGreatWorkClassInfos());
	Firaxis::Array< int, NUM_YIELD_TYPES > yield;
	for (unsigned int j = 0; j < NUM_YIELD_TYPES; ++j)
	{
		yield[j] = 0;
	}

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		for (int iClass = 0; iClass < GC.getNumGreatWorkClassInfos(); iClass++)
		{
			m_aaiCityGreatWorkClassYieldChange[iClass] = yield;
		}
	}
#endif

	for(iI = 0; iI < m_pBuildings->GetNumBuildings(); iI++)
	{
		m_paiBuildingProduction[iI] = 0;
		m_paiBuildingProductionTime[iI] = 0;
		m_paiBuildingOriginalOwner[iI] = NO_PLAYER;
		m_paiBuildingOriginalTime[iI] = MIN_INT;
		m_paiNumRealBuilding[iI] = 0;
		m_paiNumFreeBuilding[iI] = 0;
	}

	m_buildingsThatExistAtLeastOnce.clear();
}

		

void CvCityBuildings::Read(FDataStream& kStream)
{
	CvAssertMsg(m_pBuildings != NULL && m_pBuildings->GetNumBuildings() > 0, "Number of buildings to serialize is expected to greater than 0");


	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_iNumBuildings;
	kStream >> m_iBuildingProductionModifier;
	kStream >> m_iBuildingDefense;
#if defined(LEKMOD_GARRISON_YIELD_EFFECTS)
	kStream >> m_iBuildingGarrisonStrengthBonus;
	kStream >> m_iGarrisonMaintenanceFreeCount;
#endif
#ifdef NQ_BUILDING_DEFENSE_FROM_CITIZENS
	kStream >> m_iBuildingDefensePerCitizen;
#endif
	kStream >> m_iBuildingDefenseMod;
#if defined(LEKMOD_BUILDING_FIRST_PURCHASE_DISCOUNT)
	kStream >> m_iFirstPurchaseDiscount;
#endif
	kStream >> m_iMissionaryExtraSpreads;
	kStream >> m_iLandmarksTourismPercent;
	kStream >> m_iGreatWorksTourismModifier;

	kStream >> m_bSoldBuildingThisTurn;
#if defined(LEKMOD_AREA_BASED_CITY_YIELD)
	ArrayWrapper<int> kSameLandMassYieldChangeWrapper(NUM_YIELD_TYPES, m_paiSameLandMassYieldChange);
	kStream >> kSameLandMassYieldChangeWrapper;
	ArrayWrapper<int> kDifferentLandMassYieldChangeWrapper(NUM_YIELD_TYPES, m_paiDifferentLandMassYieldChange);
	kStream >> kDifferentLandMassYieldChangeWrapper;
#endif
#if defined(LEKMOD_NEARBY_TERRAIN_FREE_YIELDS)
	ArrayWrapper<int> kFreeTerrainYieldChangeWrapper(NUM_YIELD_TYPES, m_paiFreeTerrainYieldChange);
	kStream >> kFreeTerrainYieldChangeWrapper;
#endif
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	kStream >> m_aaiCityGreatWorkClassYieldChange;
#endif
	BuildingArrayHelpers::Read(kStream, m_paiBuildingProduction);
	BuildingArrayHelpers::Read(kStream, m_paiBuildingProductionTime);
	BuildingArrayHelpers::Read(kStream, m_paiBuildingOriginalOwner);
	BuildingArrayHelpers::Read(kStream, m_paiBuildingOriginalTime);
	BuildingArrayHelpers::Read(kStream, m_paiNumRealBuilding);
	BuildingArrayHelpers::Read(kStream, m_paiNumFreeBuilding);

	kStream >> m_aBuildingYieldChange;
	kStream >> m_aBuildingGreatWork;
}


void CvCityBuildings::Write(FDataStream& kStream)
{
	CvAssertMsg(m_pBuildings != NULL && m_pBuildings->GetNumBuildings() > 0, "Number of buildings to serialize is expected to greater than 0");


	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << m_iNumBuildings;
	kStream << m_iBuildingProductionModifier;

	kStream << m_iBuildingDefense;
#if defined(LEKMOD_GARRISON_YIELD_EFFECTS)
	kStream << m_iBuildingGarrisonStrengthBonus;
	kStream << m_iGarrisonMaintenanceFreeCount;
#endif
#ifdef NQ_BUILDING_DEFENSE_FROM_CITIZENS
	kStream << m_iBuildingDefensePerCitizen;
#endif
	kStream << m_iBuildingDefenseMod;
#if defined(LEKMOD_BUILDING_FIRST_PURCHASE_DISCOUNT)
	kStream << m_iFirstPurchaseDiscount;
#endif	
	kStream << m_iMissionaryExtraSpreads;
	kStream << m_iLandmarksTourismPercent;
	kStream << m_iGreatWorksTourismModifier;
	kStream << m_bSoldBuildingThisTurn;

#ifdef _MSC_VER
#pragma warning ( push )
#pragma warning ( disable : 6011 )
#endif
	int iNumBuildings = m_pBuildings->GetNumBuildings();
#ifdef _MSC_VER
#pragma warning ( pop )
#endif
#if defined(LEKMOD_AREA_BASED_CITY_YIELD)
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_paiSameLandMassYieldChange);
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_paiDifferentLandMassYieldChange);
#endif
#if defined(LEKMOD_NEARBY_TERRAIN_FREE_YIELDS)
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_paiFreeTerrainYieldChange);
#endif
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	kStream << m_aaiCityGreatWorkClassYieldChange;
#endif
	BuildingArrayHelpers::Write(kStream, m_paiBuildingProduction, iNumBuildings);
	BuildingArrayHelpers::Write(kStream, m_paiBuildingProductionTime, iNumBuildings);
	BuildingArrayHelpers::Write(kStream, m_paiBuildingOriginalOwner, iNumBuildings);
	BuildingArrayHelpers::Write(kStream, m_paiBuildingOriginalTime, iNumBuildings);
	BuildingArrayHelpers::Write(kStream, m_paiNumRealBuilding, iNumBuildings);
	BuildingArrayHelpers::Write(kStream, m_paiNumFreeBuilding, iNumBuildings);

	kStream << m_aBuildingYieldChange;
	kStream << m_aBuildingGreatWork;
}


CvBuildingXMLEntries* CvCityBuildings::GetBuildings() const
{
	return m_pBuildings;
}


int CvCityBuildings::GetNumBuildings() const
{
	return m_iNumBuildings;
}


void CvCityBuildings::ChangeNumBuildings(int iChange)
{
	m_iNumBuildings = (m_iNumBuildings + iChange);
	CvAssert(GetNumBuildings() >= 0);


}


int CvCityBuildings::GetNumBuilding(BuildingTypes eIndex) const
{
	CvAssertMsg(eIndex != NO_BUILDING, "BuildingType eIndex is expected to not be NO_BUILDING");

	if(GC.getCITY_MAX_NUM_BUILDINGS() <= 1)
	{
		return std::max(GetNumRealBuilding(eIndex), GetNumFreeBuilding(eIndex));
	}
	else
	{
		return (GetNumRealBuilding(eIndex) + GetNumFreeBuilding(eIndex));
	}
}


bool CvCityBuildings::HasBuildingClass(BuildingClassTypes eIndex) const
{
	CvAssertMsg(eIndex != NO_BUILDINGCLASS, "BuildingClassTypes eIndex is expected to not be NO_BUILDINGCLASS");

	for (std::vector<BuildingTypes>::const_iterator iI = m_buildingsThatExistAtLeastOnce.begin(); iI != m_buildingsThatExistAtLeastOnce.end(); ++iI)
	{
		CvBuildingEntry* pkInfo = GC.getBuildingInfo(*iI);
		if (pkInfo && pkInfo->GetBuildingClassType() == eIndex && GetNumBuilding(*iI) > 0)
		{
			return true;
		}
	}
	return false;
}

int CvCityBuildings::GetNumActiveBuilding(BuildingTypes eIndex) const
{
	CvAssertMsg(eIndex != NO_BUILDING, "BuildingType eIndex is expected to not be NO_BUILDING");

	if(GET_TEAM(m_pCity->getTeam()).isObsoleteBuilding(eIndex))
	{
		return 0;
	}

	return (GetNumBuilding(eIndex));
}


bool CvCityBuildings::IsBuildingSellable(const CvBuildingEntry& kBuilding) const
{

	if(IsSoldBuildingThisTurn())
		return false;


	
	if(kBuilding.GetGoldMaintenance() <= 0)
		return false;
#if defined(BEE)
	if (!kBuilding.IsSellable())
		return false;
#endif

	if(GetNumFreeBuilding((BuildingTypes)kBuilding.GetID()) > 0)
		return false;


	if(m_pCity->isCapital() && kBuilding.IsScienceBuilding())
	{
		return !(GET_PLAYER(m_pCity->getOwner()).GetPlayerTraits()->IsTechBoostFromCapitalScienceBuildings());
	}


	const BuildingClassTypes buildingClassType = (BuildingClassTypes) kBuilding.GetBuildingClassType();
	if (IsHoldingGreatWork(buildingClassType))
	{
		return false;
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pCity->getOwner());
		args->Push(kBuilding.GetID());



		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "CityBuildingsIsBuildingSellable", args.get(), bResult))
		{

			if(bResult == false)
			{
				return false;
			}
		}
	}

	return true;
}


void CvCityBuildings::DoSellBuilding(BuildingTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");

	CvBuildingEntry* pkBuildingEntry = GC.getBuildingInfo(eIndex);
	if(!pkBuildingEntry)
		return;


	if(!IsBuildingSellable(*pkBuildingEntry))
		return;


	int iRefund = GetSellBuildingRefund(eIndex);
	GET_PLAYER(m_pCity->getOwner()).GetTreasury()->ChangeGold(iRefund);


	m_pCity->GetCityCitizens()->DoRemoveAllSpecialistsFromBuilding(eIndex);

	SetNumRealBuilding(eIndex, 0);

	SetSoldBuildingThisTurn(true);
#if defined(FIX_PRODUCTION_KEEPING_EXPLOITS)
	int iLoop;
	for (CvCity* pLoopCity = GET_PLAYER(m_pCity->getOwner()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(m_pCity->getOwner()).nextCity(&iLoop))
	{
		pLoopCity->CleanUpQueue();
		if (pLoopCity->headOrderQueueNode() == NULL)
			pLoopCity->chooseProduction();
	}
#endif
#ifdef LEKMOD_NEW_LUA_EVENTS

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem)
	{
		CvLuaArgsHandle args;

		args->Push(m_pCity->getOwner());
		args->Push(eIndex);
		args->Push(m_pCity->GetID());



		bool bResult = false;
		LuaSupport::CallHook(pkScriptSystem, "BuildingSold", args.get(), bResult);
	}
#endif
}


int CvCityBuildings::GetSellBuildingRefund(BuildingTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");

	int iRefund = GET_PLAYER(m_pCity->getOwner()).getProductionNeeded(eIndex);
	iRefund /=        GC.getBUILDING_SALE_DIVISOR();

	return iRefund;
}


bool CvCityBuildings::IsSoldBuildingThisTurn() const
{
	return m_bSoldBuildingThisTurn;
}


void CvCityBuildings::SetSoldBuildingThisTurn(bool bValue)
{
	if(IsSoldBuildingThisTurn() != bValue)
		m_bSoldBuildingThisTurn = bValue;
}


int CvCityBuildings::GetTotalBaseBuildingMaintenance() const
{
	int iTotalCost = 0;

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
			if(GetNumBuilding(eBuilding))
				iTotalCost += (pkBuildingInfo->GetGoldMaintenance() * GetNumBuilding(eBuilding));
		}
	}

	return iTotalCost;
}


int CvCityBuildings::GetBuildingProduction(BuildingTypes eIndex)	const
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");
	return m_paiBuildingProduction[eIndex] / 100;
}


int CvCityBuildings::GetBuildingProductionTimes100(BuildingTypes eIndex)	const
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");
	return m_paiBuildingProduction[eIndex];
}


void CvCityBuildings::SetBuildingProduction(BuildingTypes eIndex, int iNewValue)
{
	SetBuildingProductionTimes100(eIndex, iNewValue*100);
}


void CvCityBuildings::SetBuildingProductionTimes100(BuildingTypes eIndex, int iNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings())");

	if(GetBuildingProductionTimes100(eIndex) != iNewValue)
	{
		if(GetBuildingProductionTimes100(eIndex) == 0)
		{
			NotifyNewBuildingStarted(eIndex);
		}

		m_paiBuildingProduction[eIndex] = iNewValue;
		CvAssert(GetBuildingProductionTimes100(eIndex) >= 0);

		if((m_pCity->getOwner() == GC.getGame().getActivePlayer()) && m_pCity->isCitySelected())
		{
			GC.GetEngineUserInterface()->setDirty(CityScreen_DIRTY_BIT, true);
		}

		auto_ptr<ICvCity1> pCity = GC.WrapCityPointer(m_pCity);

		GC.GetEngineUserInterface()->SetSpecificCityInfoDirty(pCity.get(), CITY_UPDATE_TYPE_BANNER);
	}
}


void CvCityBuildings::ChangeBuildingProduction(BuildingTypes eIndex, int iChange)
{
	ChangeBuildingProductionTimes100(eIndex, iChange*100);
}


void CvCityBuildings::ChangeBuildingProductionTimes100(BuildingTypes eIndex, int iChange)
{
	SetBuildingProductionTimes100(eIndex, (GetBuildingProductionTimes100(eIndex) + iChange));
}


int CvCityBuildings::GetBuildingProductionTime(BuildingTypes eIndex)	const
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");
	return m_paiBuildingProductionTime[eIndex];
}


void CvCityBuildings::SetBuildingProductionTime(BuildingTypes eIndex, int iNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");
	m_paiBuildingProductionTime[eIndex] = iNewValue;
	CvAssert(GetBuildingProductionTime(eIndex) >= 0);
}


void CvCityBuildings::ChangeBuildingProductionTime(BuildingTypes eIndex, int iChange)
{
	SetBuildingProductionTime(eIndex, (GetBuildingProductionTime(eIndex) + iChange));
}


int CvCityBuildings::GetBuildingOriginalOwner(BuildingTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");
	return m_paiBuildingOriginalOwner[eIndex];
}


void CvCityBuildings::SetBuildingOriginalOwner(BuildingTypes eIndex, int iNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");
	m_paiBuildingOriginalOwner[eIndex] = iNewValue;
}


int CvCityBuildings::GetBuildingOriginalTime(BuildingTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");
	return m_paiBuildingOriginalTime[eIndex];
}


void CvCityBuildings::SetBuildingOriginalTime(BuildingTypes eIndex, int iNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");
	m_paiBuildingOriginalTime[eIndex] = iNewValue;
}


int CvCityBuildings::GetNumRealBuilding(BuildingTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");
	return m_paiNumRealBuilding[eIndex];
}


void CvCityBuildings::SetNumRealBuilding(BuildingTypes eIndex, int iNewValue)
{
	SetNumRealBuildingTimed(eIndex, iNewValue, true, m_pCity->getOwner(), GC.getGame().getGameTurnYear());

}


void CvCityBuildings::SetNumRealBuildingTimed(BuildingTypes eIndex, int iNewValue, bool bFirst, PlayerTypes eOriginalOwner, int iOriginalTime)
{
	CvPlayer* pPlayer = &GET_PLAYER(m_pCity->getOwner());

	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");

	int iChangeNumRealBuilding = iNewValue - GetNumRealBuilding(eIndex);

	CvBuildingEntry* buildingEntry = GC.getBuildingInfo(eIndex);
	const BuildingClassTypes buildingClassType = (BuildingClassTypes) buildingEntry->GetBuildingClassType();
	const CvBuildingClassInfo& kBuildingClassInfo = buildingEntry->GetBuildingClassInfo();

	if(iChangeNumRealBuilding != 0)
	{
		int iOldNumBuilding = GetNumBuilding(eIndex);

		m_paiNumRealBuilding[eIndex] = iNewValue;

		if(GetNumRealBuilding(eIndex) > 0)
		{
			SetBuildingOriginalOwner(eIndex, eOriginalOwner);
			SetBuildingOriginalTime(eIndex, iOriginalTime);
		}
		else
		{
			SetBuildingOriginalOwner(eIndex, NO_PLAYER);
			SetBuildingOriginalTime(eIndex, MIN_INT);
		}


		if(iOldNumBuilding != GetNumBuilding(eIndex))
		{
			m_pCity->processBuilding(eIndex, iChangeNumRealBuilding, bFirst);
		}


		if(buildingEntry->GetGoldMaintenance() != 0)
		{
			pPlayer->GetTreasury()->ChangeBaseBuildingGoldMaintenance(buildingEntry->GetGoldMaintenance() * iChangeNumRealBuilding);
		}


		const char* szBuildingTypeC = buildingEntry->GetType();
		CvString szBuildingType = szBuildingTypeC;
		if(szBuildingType == "BUILDING_TEMPLE")
		{
			if(m_pCity->getOwner() == GC.getGame().getActivePlayer())
			{
				gDLL->IncrementSteamStatAndUnlock(ESTEAMSTAT_TEMPLES, 1000, ACHIEVEMENT_1000TEMPLES);
			}
		}

		if(buildingEntry->GetPreferredDisplayPosition() > 0)
		{
			auto_ptr<ICvCity1> pDllCity(new CvDllCity(m_pCity));

			if(iNewValue > 0)
			{

				if(isWorldWonderClass(kBuildingClassInfo))
				{
					if(GetBuildingProduction(eIndex))
					{
						GC.GetEngineUserInterface()->AddDeferredWonderCommand(WONDER_EDITED, pDllCity.get(), eIndex, 1);
					}
					else
					{
						GC.GetEngineUserInterface()->AddDeferredWonderCommand(WONDER_CREATED, pDllCity.get(), eIndex, 1);
					}
				}
				else
				{
					GC.GetEngineUserInterface()->AddDeferredWonderCommand(WONDER_CREATED, pDllCity.get(), eIndex, 1);
				}
			}
			else
			{
				GC.GetEngineUserInterface()->AddDeferredWonderCommand(WONDER_REMOVED, pDllCity.get(), eIndex, 0);
			}
		}

		if(!(kBuildingClassInfo.isNoLimit()))
		{
			if(isWorldWonderClass(kBuildingClassInfo))
			{
				m_pCity->changeNumWorldWonders(iChangeNumRealBuilding);
				pPlayer->ChangeNumWonders(iChangeNumRealBuilding);
			}
			else if(isTeamWonderClass(kBuildingClassInfo))
			{
				m_pCity->changeNumTeamWonders(iChangeNumRealBuilding);
			}
			else if(isNationalWonderClass(kBuildingClassInfo))
			{
				m_pCity->changeNumNationalWonders(iChangeNumRealBuilding);
				if(m_pCity->isHuman() && !GC.getGame().isGameMultiPlayer())
				{
					IncrementWonderStats(buildingClassType);
				}
			}
			else
			{
				ChangeNumBuildings(iChangeNumRealBuilding);
			}
		}

		if(buildingEntry->IsCityWall())
		{
			auto_ptr<ICvPlot1> pDllPlot(new CvDllPlot(m_pCity->plot()));
			gDLL->GameplayWallCreated(pDllPlot.get());
		}


		int iNumResources = GC.getNumResourceInfos();
		for(int iResourceLoop = 0; iResourceLoop < iNumResources; iResourceLoop++)
		{
			if(buildingEntry->GetResourceQuantityRequirement(iResourceLoop) > 0)
			{
				pPlayer->changeNumResourceUsed((ResourceTypes) iResourceLoop, iChangeNumRealBuilding * buildingEntry->GetResourceQuantityRequirement(iResourceLoop));
			}
		}

		if(iChangeNumRealBuilding > 0)
		{
			if(bFirst)
			{
				if(GC.getGame().isFinalInitialized()                                       )
				{

					if(isWorldWonderClass(kBuildingClassInfo))
					{
						Localization::String localizedText = Localization::Lookup("TXT_KEY_MISC_COMPLETES_WONDER");
						localizedText << pPlayer->getNameKey() << buildingEntry->GetTextKey();
						GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, m_pCity->getOwner(), localizedText.toUTF8(), m_pCity->getX(), m_pCity->getY());

						bool bDontShowRewardPopup = GC.GetEngineUserInterface()->IsOptionNoRewardPopups();


						if(bDontShowRewardPopup || GC.getGame().isNetworkMultiPlayer())
						{
							CvNotifications* pNotifications = GET_PLAYER(m_pCity->getOwner()).GetNotifications();
							if(pNotifications)
							{
								localizedText = Localization::Lookup("TXT_KEY_MISC_WONDER_COMPLETED");
								localizedText << pPlayer->getNameKey() << buildingEntry->GetTextKey();
								pNotifications->Add(NOTIFICATION_WONDER_COMPLETED_ACTIVE_PLAYER, localizedText.toUTF8(), localizedText.toUTF8(), m_pCity->getX(), m_pCity->getY(), eIndex, pPlayer->GetID());
							}
						}

						else
						{
							if(m_pCity->getOwner() == GC.getGame().getActivePlayer())
							{
								CvPopupInfo kPopup(BUTTONPOPUP_WONDER_COMPLETED_ACTIVE_PLAYER, eIndex);
								GC.GetEngineUserInterface()->AddPopup(kPopup);

								if(GET_PLAYER(GC.getGame().getActivePlayer()).isHuman())
								{
									gDLL->UnlockAchievement(ACHIEVEMENT_BUILD_WONDER);


									IncrementWonderStats(buildingClassType);

								}
							}
						}


						for(int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
						{
							CvPlayerAI& thisPlayer = GET_PLAYER((PlayerTypes)iI);
							if(thisPlayer.isAlive())
							{

								if(iI != m_pCity->getOwner())
								{


									if((m_pCity->plot()->isRevealed(thisPlayer.getTeam()) && GET_TEAM(thisPlayer.getTeam()).isHasMet(m_pCity->getTeam())) || gDLL->IsModActivated(CIV5_DLC_06_SCENARIO_MODID))
									{
										CvNotifications* pNotifications = thisPlayer.GetNotifications();
										if(pNotifications)
										{
											localizedText = Localization::Lookup("TXT_KEY_MISC_WONDER_COMPLETED");
											localizedText << pPlayer->getNameKey() << buildingEntry->GetTextKey();
											pNotifications->Add(NOTIFICATION_WONDER_COMPLETED, localizedText.toUTF8(), localizedText.toUTF8(), m_pCity->getX(), m_pCity->getY(), eIndex, pPlayer->GetID());
										}
									}
									else
									{
										CvNotifications* pNotifications = thisPlayer.GetNotifications();
										if(pNotifications)
										{
											localizedText = Localization::Lookup("TXT_KEY_MISC_WONDER_COMPLETED_UNKNOWN");
											localizedText <<  buildingEntry->GetTextKey();
											pNotifications->Add(NOTIFICATION_WONDER_COMPLETED, localizedText.toUTF8(), localizedText.toUTF8(), -1, -1, eIndex, -1);
										}
									}
								}
							}


							if(pPlayer->GetID() == GC.getGame().getActivePlayer() && strcmp(buildingEntry->GetType(), "BUILDING_GREAT_FIREWALL") == 0)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP1_16);
							}
						}
					}
				}

				GC.getGame().incrementBuildingClassCreatedCount(buildingClassType);
			}
		}

		m_pCity->updateStrengthValue();


		auto_ptr<ICvCity1> pCity = GC.WrapCityPointer(m_pCity);
		GC.GetEngineUserInterface()->SetSpecificCityInfoDirty(pCity.get(), CITY_UPDATE_TYPE_BANNER);


		pPlayer->GetPlayerAchievements().FinishedBuilding(m_pCity, eIndex);
	}
}


int CvCityBuildings::GetNumFreeBuilding(BuildingTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");
	return m_paiNumFreeBuilding[eIndex];
}


void CvCityBuildings::SetNumFreeBuilding(BuildingTypes eIndex, int iNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");

	if (GetNumFreeBuilding(eIndex) != iNewValue)
	{
		int iOldNumBuilding = GetNumBuilding(eIndex);

		if (iOldNumBuilding > 0 && iNewValue > 0)
		{
#ifndef AQUEDUCT_FIX
			DoSellBuilding(eIndex);
#else
			SetNumRealBuilding(eIndex, 0);
#endif
			m_paiNumFreeBuilding[eIndex] = iNewValue;
			m_pCity->processBuilding(eIndex, iNewValue, true);			
		}
		
		else
		{
			m_paiNumFreeBuilding[eIndex] = iNewValue;

			if (iOldNumBuilding != GetNumBuilding(eIndex))
			{
				m_pCity->processBuilding(eIndex, iNewValue - iOldNumBuilding, true);
			}
		}
	}
}


int CvCityBuildings::GetBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield) const
{
	for(std::vector<BuildingYieldChange>::const_iterator it = m_aBuildingYieldChange.begin(); it != m_aBuildingYieldChange.end(); ++it)
	{
		if((*it).eBuildingClass == eBuildingClass && (*it).eYield == eYield)
		{
			return (*it).iChange;
		}
	}

	return 0;
}


void CvCityBuildings::SetBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield, int iChange)
{
#ifdef AUI_WARNING_FIXES
	CvCivilizationInfo* pCivInfo = GC.getCivilizationInfo(m_pCity->getCivilizationType());
	if (!pCivInfo)
		return;
#endif
	for(std::vector<BuildingYieldChange>::iterator it = m_aBuildingYieldChange.begin(); it != m_aBuildingYieldChange.end(); ++it)
	{
		if((*it).eBuildingClass == eBuildingClass && (*it).eYield == eYield)
		{
			int iOldChange = (*it).iChange;
			if(iOldChange != iChange)
			{

				if(iChange == 0)
				{
					m_aBuildingYieldChange.erase(it);
				}
				else
				{
					(*it).iChange = iChange;
				}

#ifdef AUI_WARNING_FIXES
				BuildingTypes eBuilding = (BuildingTypes)pCivInfo->getCivilizationBuildings(eBuildingClass);
#else
				BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(m_pCity->getCivilizationType())->getCivilizationBuildings(eBuildingClass);
#endif
				if(NO_BUILDING != eBuilding)
				{
					if(GetNumActiveBuilding(eBuilding) > 0)
					{
						m_pCity->ChangeBaseYieldRateFromBuildings(eYield, (iChange - iOldChange) * GetNumActiveBuilding(eBuilding));
					}
				}
			}

			return;
		}
	}

	if(0 != iChange)
	{
		BuildingYieldChange kChange;
		kChange.eBuildingClass = eBuildingClass;
		kChange.eYield = eYield;
		kChange.iChange = iChange;
		m_aBuildingYieldChange.push_back(kChange);

		BuildingTypes eBuilding = (BuildingTypes)m_pCity->getCivilizationInfo().getCivilizationBuildings(eBuildingClass);
		if(NO_BUILDING != eBuilding)
		{
			if(GetNumActiveBuilding(eBuilding) > 0)
			{
				m_pCity->ChangeBaseYieldRateFromBuildings(eYield, iChange * GetNumActiveBuilding(eBuilding));
			}
		}
	}
}


void CvCityBuildings::ChangeBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield, int iChange)
{
	SetBuildingYieldChange(eBuildingClass, eYield, GetBuildingYieldChange(eBuildingClass, eYield) + iChange);
}


#ifdef AUI_WARNING_FIXES
int CvCityBuildings::GetBuildingGreatWork(BuildingClassTypes eBuildingClass, uint iSlot) const
#else
int CvCityBuildings::GetBuildingGreatWork(BuildingClassTypes eBuildingClass, int iSlot) const
#endif
{
	for(std::vector<BuildingGreatWork>::const_iterator it = m_aBuildingGreatWork.begin(); it != m_aBuildingGreatWork.end(); ++it)
	{
		if((*it).eBuildingClass == eBuildingClass && (*it).iSlot == iSlot)
		{
			return (*it).iGreatWorkIndex;
		}
	}

	return -1;
}


void CvCityBuildings::SetBuildingGreatWork(BuildingClassTypes eBuildingClass, int iSlot, int iGreatWorkIndex)
{
	for(std::vector<BuildingGreatWork>::iterator it = m_aBuildingGreatWork.begin(); it != m_aBuildingGreatWork.end(); ++it)
	{
		if((*it).eBuildingClass == eBuildingClass && (*it).iSlot == iSlot)
		{
			int iOldGreatWorkIndex = (*it).iGreatWorkIndex;
			if (iOldGreatWorkIndex != iGreatWorkIndex)
			{
				if (iGreatWorkIndex == -1)
				{
					m_aBuildingGreatWork.erase(it);
				}
				else
				{
					(*it).iGreatWorkIndex = iGreatWorkIndex;
#if !defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
				}
#else
					rebuildGreatWorkYields(GC.getGame().GetGameCulture()->GetGreatWorkClass(iGreatWorkIndex));
					calculateHappinessFromGreatWorks();
				}
			}
#endif
			GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
			GC.GetEngineUserInterface()->setDirty(GreatWorksScreen_DIRTY_BIT, true);
			return;
		}
	}

	if (iGreatWorkIndex != -1)
	{
		BuildingGreatWork kWork;
		kWork.eBuildingClass = eBuildingClass;
		kWork.iSlot = iSlot;
		kWork.iGreatWorkIndex = iGreatWorkIndex;
		m_aBuildingGreatWork.push_back(kWork);
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
		m_bGreatWorkClassMapDirty = true;
		rebuildGreatWorkYields(GC.getGame().GetGameCulture()->GetGreatWorkClass(iGreatWorkIndex));
		calculateHappinessFromGreatWorks();
#endif
	}

	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
	GC.GetEngineUserInterface()->setDirty(GreatWorksScreen_DIRTY_BIT, true);
}


bool CvCityBuildings::IsHoldingGreatWork(BuildingClassTypes eBuildingClass) const
{
	for(std::vector<BuildingGreatWork>::const_iterator it = m_aBuildingGreatWork.begin(); it != m_aBuildingGreatWork.end(); ++it)
	{
		if((*it).eBuildingClass == eBuildingClass)
		{
			return true;
		}
	}
	return false;
}


#ifdef AUI_WARNING_FIXES
uint CvCityBuildings::GetNumGreatWorksInBuilding(BuildingClassTypes eBuildingClass) const
{
	uint iCount = 0;
#else
int CvCityBuildings::GetNumGreatWorksInBuilding(BuildingClassTypes eBuildingClass) const
{
	int iCount = 0;
#endif
	for(std::vector<BuildingGreatWork>::const_iterator it = m_aBuildingGreatWork.begin(); it != m_aBuildingGreatWork.end(); ++it)
	{
		if((*it).eBuildingClass == eBuildingClass)
		{
			iCount++;
		}
	}
	return iCount;
}


bool CvCityBuildings::HasAnyAvailableGreatWorkSlot() const
{
	BuildingClassTypes eBuildingClass = NO_BUILDINGCLASS;
#ifdef AUI_WARNING_FIXES
	uint iSlot = MAX_UNSIGNED_INT;
#else
	int iSlot = -1;
#endif

	return GetNextAvailableGreatWorkSlot (&eBuildingClass, &iSlot);
}


bool CvCityBuildings::HasAvailableGreatWorkSlot(GreatWorkSlotType eSlotType) const
{
	BuildingClassTypes eBuildingClass = NO_BUILDINGCLASS;
#ifdef AUI_WARNING_FIXES
	uint iSlot = MAX_UNSIGNED_INT;
#else
	int iSlot = -1;
#endif

	return GetNextAvailableGreatWorkSlot (eSlotType, &eBuildingClass, &iSlot);
}


#ifdef AUI_WARNING_FIXES
uint CvCityBuildings::GetNumAvailableGreatWorkSlots() const
{
	uint iCount = 0;

	for (uint iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#else
int CvCityBuildings::GetNumAvailableGreatWorkSlots() const
{
	int iCount = 0;

	for(int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#endif
	{
		BuildingClassTypes eLoopBuildingClass = (BuildingClassTypes) iI;
		CvCivilizationInfo *pkCivInfo = GC.getCivilizationInfo(m_pCity->getCivilizationType());
		if (pkCivInfo)
		{
			BuildingTypes eBuilding = (BuildingTypes)pkCivInfo->getCivilizationBuildings(eLoopBuildingClass);
			if(NO_BUILDING != eBuilding)
			{
				if (GetNumBuilding(eBuilding) > 0)
				{
					CvBuildingEntry *pkBuilding = GC.getBuildingInfo(eBuilding);
					if (pkBuilding)
					{
#ifdef AUI_WARNING_FIXES
						uint iNumSlots = pkBuilding->GetGreatWorkCount();
						uint iNumGreatWorksInBuilding = GetNumGreatWorksInBuilding(eLoopBuildingClass);
						if (iNumSlots > iNumGreatWorksInBuilding)
						{
							iCount += iNumSlots - iNumGreatWorksInBuilding;
#else
						int iNumSlots = pkBuilding->GetGreatWorkCount();
						int iNumOpenSlots = iNumSlots - GetNumGreatWorksInBuilding(eLoopBuildingClass);
						if(iNumOpenSlots > 0)
						{
							iCount += iNumOpenSlots;
#endif
						}
					}
				}
			}
		}
	}

	return iCount;
}


#ifdef AUI_WARNING_FIXES
uint CvCityBuildings::GetNumAvailableGreatWorkSlots(GreatWorkSlotType eSlotType) const
{
	uint iCount = 0;

	for (uint iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#else
int CvCityBuildings::GetNumAvailableGreatWorkSlots(GreatWorkSlotType eSlotType) const
{
	int iCount = 0;

	for(int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#endif
	{
		BuildingClassTypes eLoopBuildingClass = (BuildingClassTypes) iI;
		CvCivilizationInfo *pkCivInfo = GC.getCivilizationInfo(m_pCity->getCivilizationType());
		if (pkCivInfo)
		{
			BuildingTypes eBuilding = (BuildingTypes)pkCivInfo->getCivilizationBuildings(eLoopBuildingClass);
			if(NO_BUILDING != eBuilding)
			{
				if (GetNumBuilding(eBuilding) > 0)
				{
					CvBuildingEntry *pkBuilding = GC.getBuildingInfo(eBuilding);
					if (pkBuilding)
					{
						if (pkBuilding->GetGreatWorkSlotType() == eSlotType)
						{
#ifdef AUI_WARNING_FIXES
							uint iNumSlots = pkBuilding->GetGreatWorkCount();
							uint iNumGreatWorksInBuilding = GetNumGreatWorksInBuilding(eLoopBuildingClass);
							if (iNumSlots > iNumGreatWorksInBuilding)
							{
								iCount += iNumSlots - iNumGreatWorksInBuilding;
#else
							int iNumSlots = pkBuilding->GetGreatWorkCount();
							int iNumOpenSlots = iNumSlots - GetNumGreatWorksInBuilding(eLoopBuildingClass);
							if(iNumOpenSlots > 0)
							{
								iCount += iNumOpenSlots;
#endif
							}
						}
					}
				}
			}
		}
	}

	return iCount;
}


#ifdef AUI_WARNING_FIXES
bool CvCityBuildings::GetNextAvailableGreatWorkSlot(BuildingClassTypes *eBuildingClass, uint *iSlot) const
#else
bool CvCityBuildings::GetNextAvailableGreatWorkSlot(BuildingClassTypes *eBuildingClass, int *iSlot) const
#endif
{
	if (eBuildingClass && iSlot)
	{
#ifdef AUI_WARNING_FIXES
		CvBuildingEntry* pBuildingInfo;
		CvCivilizationInfo* pCivInfo = GC.getCivilizationInfo(m_pCity->getCivilizationType());
		if (!pCivInfo)
			return false;
		for (uint iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#else
		for(int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#endif
		{
			BuildingClassTypes eLoopBuildingClass = (BuildingClassTypes) iI;
#ifdef AUI_WARNING_FIXES
			BuildingTypes eBuilding = (BuildingTypes)pCivInfo->getCivilizationBuildings(eLoopBuildingClass);
#else
			BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(m_pCity->getCivilizationType())->getCivilizationBuildings(eLoopBuildingClass);
#endif
			if(NO_BUILDING != eBuilding)
			{
				if (GetNumBuilding(eBuilding) > 0)
				{
#ifdef AUI_WARNING_FIXES
					uint iNumSlots = 0;
					pBuildingInfo = GC.getBuildingInfo(eBuilding);
					if (pBuildingInfo)
						iNumSlots = pBuildingInfo->GetGreatWorkCount();
					for (uint jJ = 0; jJ < iNumSlots; jJ++)
#else
					int iNumSlots = GC.getBuildingInfo(eBuilding)->GetGreatWorkCount();
					for (int jJ = 0; jJ < iNumSlots; jJ++)
#endif
					{
						if (GetBuildingGreatWork (eLoopBuildingClass, jJ) == NO_GREAT_WORK)
						{
							*eBuildingClass = eLoopBuildingClass;
							*iSlot = jJ;
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}


#ifdef AUI_WARNING_FIXES
bool CvCityBuildings::GetNextAvailableGreatWorkSlot(GreatWorkSlotType eGreatWorkSlot, BuildingClassTypes *eBuildingClass, uint *iSlot) const
#else
bool CvCityBuildings::GetNextAvailableGreatWorkSlot(GreatWorkSlotType eGreatWorkSlot, BuildingClassTypes *eBuildingClass, int *iSlot) const
#endif
{
	if (eBuildingClass && iSlot)
	{
#ifdef AUI_WARNING_FIXES
		CvBuildingEntry* pBuildingInfo;
		CvCivilizationInfo* pCivInfo = GC.getCivilizationInfo(m_pCity->getCivilizationType());
		if (!pCivInfo)
			return false;
		for (uint iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#else
		for(int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#endif
		{
			BuildingClassTypes eLoopBuildingClass = (BuildingClassTypes) iI;
#ifdef AUI_WARNING_FIXES
			BuildingTypes eBuilding = (BuildingTypes)pCivInfo->getCivilizationBuildings(eLoopBuildingClass);
#else
			BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(m_pCity->getCivilizationType())->getCivilizationBuildings(eLoopBuildingClass);
#endif
			if(NO_BUILDING != eBuilding)
			{
				if (GetNumBuilding(eBuilding) > 0)
				{
#ifdef AUI_WARNING_FIXES
					pBuildingInfo = GC.getBuildingInfo(eBuilding);
					if (pBuildingInfo && pBuildingInfo->GetGreatWorkSlotType() == eGreatWorkSlot)
#else
					if (GC.getBuildingInfo(eBuilding)->GetGreatWorkSlotType() == eGreatWorkSlot)
#endif
					{
#ifdef AUI_WARNING_FIXES
						uint iNumSlots = pBuildingInfo->GetGreatWorkCount();
						for (uint jJ = 0; jJ < iNumSlots; jJ++)
#else
						int iNumSlots = GC.getBuildingInfo(eBuilding)->GetGreatWorkCount();
						for (int jJ = 0; jJ < iNumSlots; jJ++)
#endif
						{
							if (GetBuildingGreatWork (eLoopBuildingClass, jJ) == NO_GREAT_WORK)
							{
								*eBuildingClass = eLoopBuildingClass;
								*iSlot = jJ;
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}
#if !defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)

int CvCityBuildings::GetCultureFromGreatWorks() const
{
	int iCulturePerWork = GC.getBASE_CULTURE_PER_GREAT_WORK();
	iCulturePerWork += GET_PLAYER(m_pCity->getOwner()).GetGreatWorkYieldChange(YIELD_CULTURE);

	int iRtnValue = iCulturePerWork * m_aBuildingGreatWork.size();
	iRtnValue += GetThemingBonuses();

	return iRtnValue;
}
#endif
#if !defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)


int CvCityBuildings::GetYieldFromGreatWorks(YieldTypes eIndex) const
{
	int iYieldPerWork = GET_PLAYER(m_pCity->getOwner()).GetGreatWorkYieldChange(eIndex);
	int iRtnValue = iYieldPerWork * m_aBuildingGreatWork.size();
	return iRtnValue;
}

int CvCityBuildings::GetThemingBonuses() const
{
	int iBonus = 0;

#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#else
	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#endif
	{
		BuildingClassTypes eLoopBuildingClass = (BuildingClassTypes)iI;
		CvCivilizationInfo* pkCivInfo = GC.getCivilizationInfo(m_pCity->getCivilizationType());
		if (pkCivInfo)
		{
			BuildingTypes eBuilding = (BuildingTypes)pkCivInfo->getCivilizationBuildings(eLoopBuildingClass);
			if (NO_BUILDING != eBuilding)
			{
				if (GetNumBuilding(eBuilding) > 0)
				{
					iBonus += m_pCity->GetCityCulture()->GetThemingBonus(eLoopBuildingClass);
				}
			}
		}
	}
	return iBonus;
}
#else
int CvCityBuildings::GetYieldFromGreatWorks(YieldTypes eIndex) const
{
	int iTotalYield = 0;
	int thisWork = 0;
#if defined(LEK_YIELD_TOURISM)
	int iMod = 100 + (YIELD_TOURISM == eIndex ? GetGreatWorksTourismModifier() : 0);
#endif
	CvGameCulture* culture = GC.getGame().GetGameCulture();
	if (!culture)
		return 0;
	for (std::vector<BuildingGreatWork>::const_iterator it = m_aBuildingGreatWork.begin(); it != m_aBuildingGreatWork.end(); ++it)
	{
		const CvGreatWork* pWork = &culture->m_CurrentGreatWorks[(*it).iGreatWorkIndex];
		if (pWork)
		{
			thisWork = pWork->m_viYield[eIndex];
#if defined(LEK_YIELD_TOURISM)
			thisWork *= iMod;
			thisWork /= 100;
#endif
			iTotalYield += thisWork;
		}
	}
	return iTotalYield;
}
int CvCityBuildings::GetNumGreatWorks(GreatWorkClass eGreatWorkClass) const
{
	const std::map<GreatWorkClass, int>& mClassCounts = GetGreatWorkClassCounts();

	std::map<GreatWorkClass, int>::const_iterator it = mClassCounts.find(eGreatWorkClass);
	if (it != mClassCounts.end())
	{
		return it->second;
	}

	return 0;
}
const std::map<GreatWorkClass, int>& CvCityBuildings::GetGreatWorkClassCounts() const
{

	if (m_bGreatWorkClassMapDirty)
	{
		m_cachedGreatWorkClassCounts.clear();

		CvGameCulture* pCulture = GC.getGame().GetGameCulture();
		if (pCulture)
		{
			for (size_t i = 0; i < m_aBuildingGreatWork.size(); ++i)
			{
				const BuildingGreatWork& kWork = m_aBuildingGreatWork[i];
				if (kWork.iGreatWorkIndex < 0)
					continue;

				GreatWorkClass eClass = pCulture->GetGreatWorkClass(kWork.iGreatWorkIndex);
				if (eClass != NO_GREAT_WORK_CLASS)
				{
					std::map<GreatWorkClass, int>::iterator it = m_cachedGreatWorkClassCounts.find(eClass);
					if (it != m_cachedGreatWorkClassCounts.end())
					{
						it->second += 1;
					}
					else
					{
						m_cachedGreatWorkClassCounts.insert(std::make_pair(eClass, 1));
					}
				}
			}
		}

		m_bGreatWorkClassMapDirty = false;
	}

	return m_cachedGreatWorkClassCounts;
}
void CvCityBuildings::rebuildGreatWorkYields(GreatWorkClass eClass)
{
	CvGameCulture* culture = GC.getGame().GetGameCulture();
	if (!culture)
		return;
	CvPlayerCulture* playerCulture = GET_PLAYER(m_pCity->getOwner()).GetCulture();
	if (playerCulture)
	{
		for (std::vector<BuildingGreatWork>::const_iterator it = m_aBuildingGreatWork.begin(); it != m_aBuildingGreatWork.end(); ++it)
		{
			CvGreatWork* work = &culture->m_CurrentGreatWorks[(*it).iGreatWorkIndex];
			if (work->m_eClassType != eClass)
				continue;
			if (work)
			{
				int iCityID, iSlot, workYield;
				BuildingTypes eBuilding;
				if (!playerCulture->GetGreatWorkLocation((*it).iGreatWorkIndex, iCityID, eBuilding, iSlot))
					continue;
				CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
				if (!pkBuildingInfo)
					continue;
			
				for (int yield = 0; yield < NUM_YIELD_TYPES; yield++)
				{
					YieldTypes eYield = static_cast<YieldTypes>(yield);
					work->m_viYield[eYield] = 0;
					workYield = GC.getGreatWorkClassInfo(work->m_eClassType)->getGreatWorkClassBaseYield(eYield);
					workYield += GET_PLAYER(m_pCity->getOwner()).GetGreatWorkClassYieldChange(work->m_eClassType, eYield);
					workYield += pkBuildingInfo->GetBuildingGreatWorkYieldChange(eYield);
					workYield += GetCityGreatWorkClassYieldChanges(work->m_eClassType, eYield);
					work->m_viYield[eYield] = workYield;
				}
			}
		}
	}
}
int CvCityBuildings::GetHappinessFromGreatWorks() const
{
	return m_iHappinessFromGreatWorks;
}
void CvCityBuildings::calculateHappinessFromGreatWorks()
{
	m_iHappinessFromGreatWorks = 0;
	int iTotalHappiness = 0;
	CvGameCulture* culture = GC.getGame().GetGameCulture();
	if (!culture)
		return;
	CvPlayerCulture* playerCulture = GET_PLAYER(m_pCity->getOwner()).GetCulture();
	if (playerCulture)
	{
		for (std::vector<BuildingGreatWork>::const_iterator it = m_aBuildingGreatWork.begin(); it != m_aBuildingGreatWork.end(); ++it)
		{
			CvGreatWork* pWork = &culture->m_CurrentGreatWorks[(*it).iGreatWorkIndex];
			if (pWork)
			{
				int iCityID, iSlot;
				BuildingTypes eBuilding;
				if (!playerCulture->GetGreatWorkLocation((*it).iGreatWorkIndex, iCityID, eBuilding, iSlot))
					continue;
				CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
				if (!pkBuildingInfo)
					continue;
				int buildingHappiness = pkBuildingInfo->GetGreatWorkHappiness();
				iTotalHappiness += buildingHappiness;
			}
		}
	}
	m_iHappinessFromGreatWorks = iTotalHappiness;
	GET_PLAYER(m_pCity->getOwner()).DoUpdateHappiness();
}
int CvCityBuildings::countNumThemesActive() const
{
	int iCount = 0;
	for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); iBuildingClass++)
	{
		BuildingClassTypes eBuildingClass = (BuildingClassTypes)iBuildingClass;
		if (m_pCity->GetCityCulture()->GetPublicThemingBonusIndex(eBuildingClass) != -1)
			iCount++;
	}
	return iCount;
}
int CvCityBuildings::GetThemingBonuses(YieldTypes eYield) const
{
	int iBonus = 0;
	if (YIELD_CULTURE == eYield
#if defined(LEK_YIELD_TOURISM)
		|| YIELD_TOURISM == eYield)
#else 
		)
#endif
	{
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			BuildingClassTypes eLoopBuildingClass = (BuildingClassTypes)iI;
			CvCivilizationInfo* pkCivInfo = GC.getCivilizationInfo(m_pCity->getCivilizationType());
			if (pkCivInfo)
			{
				BuildingTypes eBuilding = (BuildingTypes)pkCivInfo->getCivilizationBuildings(eLoopBuildingClass);
				if (NO_BUILDING != eBuilding)
				{
					if (GetNumBuilding(eBuilding) > 0)
					{
						iBonus += m_pCity->GetCityCulture()->GetThemingBonus(eLoopBuildingClass);
					}
				}
			}
		}
	}
	return iBonus;
}
int CvCityBuildings::GetGreatWorkClassGreatPersonPoints(SpecialistTypes eSpecialist) const
{
	return 0;













}
#endif
#if defined(LEK_YIELD_TOURISM)
int CvCityBuildings::GetYieldFromLandmarks(YieldTypes eYield) const
{
	int iRtnValue = 0;
	if (YIELD_TOURISM != eYield)
		return iRtnValue;
	int iPercent = GetLandmarksTourismPercent();
	if (iPercent > 0)
	{
		iRtnValue += m_pCity->getWonderYields(YIELD_CULTURE);
		iRtnValue += m_pCity->GetCityCulture()->GetCultureFromImprovements();
		iRtnValue += m_pCity->GetCityCulture()->GetCultureFromNaturalWonders();
		iRtnValue *= iPercent;
		iRtnValue /= 100;
	}
	return iRtnValue;
}
#endif

#ifdef AUI_WARNING_FIXES
uint CvCityBuildings::GetNumGreatWorks() const
#else
int CvCityBuildings::GetNumGreatWorks() const
#endif
{

	return m_aBuildingGreatWork.size();
}


#ifdef AUI_WARNING_FIXES
uint CvCityBuildings::GetNumGreatWorks(GreatWorkSlotType eGreatWorkSlot) const
{
	uint iRtnValue = 0;
#else
int CvCityBuildings::GetNumGreatWorks(GreatWorkSlotType eGreatWorkSlot) const
{
	int iRtnValue = 0;
#endif

	CvCivilizationInfo *pkCivInfo = GC.getCivilizationInfo(m_pCity->getCivilizationType());
	if (pkCivInfo)
	{
		for(std::vector<BuildingGreatWork>::const_iterator it = m_aBuildingGreatWork.begin(); it != m_aBuildingGreatWork.end(); ++it)
		{
			BuildingClassTypes eBldgClass = (*it).eBuildingClass;
			CvBuildingClassInfo *pkClassInfo = GC.getBuildingClassInfo(eBldgClass);
			if (pkClassInfo)
			{
				BuildingTypes eBuilding = (BuildingTypes)pkCivInfo->getCivilizationBuildings(eBldgClass);
				CvBuildingEntry *pkInfo = GC.getBuildingInfo(eBuilding);
				if (pkInfo)
				{
					if (pkInfo->GetGreatWorkSlotType() == eGreatWorkSlot)
					{
						iRtnValue++;
					}
				}
			}
		}
	}
	return iRtnValue;
}


int CvCityBuildings::GetLandmarksTourismPercent() const
{
	return m_iLandmarksTourismPercent;
}


void CvCityBuildings::ChangeLandmarksTourismPercent(int iChange)
{
	if(iChange != 0)
	{
		m_iLandmarksTourismPercent = (m_iLandmarksTourismPercent + iChange);
		CvAssert(m_iLandmarksTourismPercent >= 0);
	}
}


int CvCityBuildings::GetGreatWorksTourismModifier() const
{
	return m_iGreatWorksTourismModifier;
}


void CvCityBuildings::ChangeGreatWorksTourismModifier(int iChange)
{
	if(iChange != 0)
	{
		m_iGreatWorksTourismModifier = (m_iGreatWorksTourismModifier + iChange);
		CvAssert(m_iGreatWorksTourismModifier >= 0);
	}
}


int CvCityBuildings::GetThemingBonuses() const
{
	int iBonus = 0;

#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#else
	for(int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#endif
	{
		BuildingClassTypes eLoopBuildingClass = (BuildingClassTypes) iI;
		CvCivilizationInfo *pkCivInfo = GC.getCivilizationInfo(m_pCity->getCivilizationType());
		if (pkCivInfo)
		{
			BuildingTypes eBuilding = (BuildingTypes)pkCivInfo->getCivilizationBuildings(eLoopBuildingClass);
			if(NO_BUILDING != eBuilding)
			{
				if (GetNumBuilding(eBuilding) > 0)
				{
					iBonus += m_pCity->GetCityCulture()->GetThemingBonus(eLoopBuildingClass);
				}
			}
		}
	}

	return iBonus;
}


int CvCityBuildings::GetNumBuildingsFromFaith() const
{
	int iRtnValue = 0;

#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#else
	for(int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#endif
	{
		BuildingClassTypes eLoopBuildingClass = (BuildingClassTypes) iI;
		CvCivilizationInfo *pkCivInfo = GC.getCivilizationInfo(m_pCity->getCivilizationType());
		if (pkCivInfo)
		{
			BuildingTypes eBuilding = (BuildingTypes)pkCivInfo->getCivilizationBuildings(eLoopBuildingClass);
			if (NO_BUILDING != eBuilding)
			{
				if (GetNumBuilding(eBuilding) > 0)
				{
					CvBuildingEntry *pkEntry = GC.getBuildingInfo(eBuilding);
					if (pkEntry)
					{
						if (pkEntry->GetFaithCost() > 0 && pkEntry->IsUnlockedByBelief() && pkEntry->GetProductionCost() == -1)
						{
							iRtnValue++;
						}
					}
				}
			}
		}
	}

	return iRtnValue;
}


int CvCityBuildings::GetCityStateTradeRouteProductionModifier() const
{
	int iRtnValue = 0;

#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#else
	for(int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#endif
	{
		BuildingClassTypes eLoopBuildingClass = (BuildingClassTypes) iI;
		CvCivilizationInfo *pkCivInfo = GC.getCivilizationInfo(m_pCity->getCivilizationType());
		if (pkCivInfo)
		{
			BuildingTypes eBuilding = (BuildingTypes)pkCivInfo->getCivilizationBuildings(eLoopBuildingClass);
			if (NO_BUILDING != eBuilding)
			{
				if (GetNumBuilding(eBuilding) > 0)
				{
					CvBuildingEntry *pkEntry = GC.getBuildingInfo(eBuilding);
					if (pkEntry)
					{
						int iProductionModifier = pkEntry->GetCityStateTradeRouteProductionModifier();
						int iCityStates = GET_PLAYER(m_pCity->getOwner()).GetTrade()->GetNumberOfCityStateTradeRoutes();
						if (iProductionModifier > 0  && iCityStates > 0)
						{
							iRtnValue = iProductionModifier * iCityStates;
						}
					}
				}
			}
		}
	}

	return iRtnValue;
}



int CvCityBuildings::GetCityStateTradeRouteGoldModifier() const
{
	int iRtnValue = 0;

	for(uint iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		BuildingClassTypes eLoopBuildingClass = (BuildingClassTypes) iI;
		CvCivilizationInfo *pkCivInfo = GC.getCivilizationInfo(m_pCity->getCivilizationType());
		if (pkCivInfo)
		{
			BuildingTypes eBuilding = (BuildingTypes)pkCivInfo->getCivilizationBuildings(eLoopBuildingClass);
			if (NO_BUILDING != eBuilding)
			{
				if (GetNumBuilding(eBuilding) > 0)
				{
					CvBuildingEntry *pkEntry = GC.getBuildingInfo(eBuilding);
					if (pkEntry)
					{
						int iGoldModifier = pkEntry->GetCityStateTradeRouteGoldModifier();
						int iCityStates = GET_PLAYER(m_pCity->getOwner()).GetTrade()->GetNumberOfCityStateTradeRoutes();
						if (iGoldModifier > 0  && iCityStates > 0)
						{
							iRtnValue = iGoldModifier * iCityStates;
						}
					}
				}
			}
		}
	}

	return iRtnValue;
}




int CvCityBuildings::GetBuildingProductionModifier() const
{
	return m_iBuildingProductionModifier;
}


void CvCityBuildings::ChangeBuildingProductionModifier(int iChange)
{
	m_iBuildingProductionModifier = (m_iBuildingProductionModifier + iChange);
	CvAssert(GetBuildingProductionModifier() >= 0);
}
#if defined(LEKMOD_AREA_BASED_CITY_YIELD)
int CvCityBuildings::GetSameLandMassYieldChange(YieldTypes eYield) const
{
	CvAssertMsg(eYield >= 0, "eYield expected to be >= 0");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield expected to be < NUM_YIELD_TYPES");
	return m_paiSameLandMassYieldChange[eYield];
}
void CvCityBuildings::ChangeSameLandMassYieldChange(YieldTypes eYield, int iChange)
{
	CvAssertMsg(eYield >= 0, "eYield expected to be >= 0");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield expected to be < NUM_YIELD_TYPES");
	if(iChange != 0)
	{
		m_paiSameLandMassYieldChange[eYield] = (m_paiSameLandMassYieldChange[eYield] + iChange);
		CvAssert(GetSameLandMassYieldChange(eYield) >= 0);
		m_pCity->updateYield();
	}
}
int CvCityBuildings::GetDifferentLandMassYieldChange(YieldTypes eYield) const
{
	CvAssertMsg(eYield >= 0, "eYield expected to be >= 0");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield expected to be < NUM_YIELD_TYPES");
	return m_paiDifferentLandMassYieldChange[eYield];
}
void CvCityBuildings::ChangeDifferentLandMassYieldChange(YieldTypes eYield, int iChange)
{
	CvAssertMsg(eYield >= 0, "eYield expected to be >= 0");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield expected to be < NUM_YIELD_TYPES");
	if(iChange != 0)
	{
		m_paiDifferentLandMassYieldChange[eYield] = (m_paiDifferentLandMassYieldChange[eYield] + iChange);
		CvAssert(GetDifferentLandMassYieldChange(eYield) >= 0);
		m_pCity->updateYield();
	}
}
#endif
#if defined(LEKMOD_NEARBY_TERRAIN_FREE_YIELDS)
int CvCityBuildings::GetFreeTerrainYieldChange(YieldTypes eYield) const
{
	CvAssertMsg(eYield >= 0, "eYield expected to be >= 0");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield expected to be < NUM_YIELD_TYPES");
	return m_paiFreeTerrainYieldChange[eYield];
}
void CvCityBuildings::ChangeFreeTerrainYieldChange(YieldTypes eYield, int iChange)
{
	CvAssertMsg(eYield >= 0, "eYield expected to be >= 0");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield expected to be < NUM_YIELD_TYPES");
	if(iChange != 0)
	{
		m_paiFreeTerrainYieldChange[eYield] = (m_paiFreeTerrainYieldChange[eYield] + iChange);
	}
}
#endif

int CvCityBuildings::GetBuildingDefense() const
{
	return m_iBuildingDefense;
}


void CvCityBuildings::ChangeBuildingDefense(int iChange)
{
	if(iChange != 0)
	{
		m_iBuildingDefense = (m_iBuildingDefense + iChange);
		CvAssert(GetBuildingDefense() >= 0);

		m_pCity->plot()->plotAction(PUF_makeInfoBarDirty);
	}
}
#if defined(LEKMOD_GARRISON_YIELD_EFFECTS)

int CvCityBuildings::GetGarrisonStrengthBonus() const
{
	return m_iBuildingGarrisonStrengthBonus;
}


void CvCityBuildings::ChangeGarrisonStrengthBonus(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingGarrisonStrengthBonus = (m_iBuildingGarrisonStrengthBonus + iChange);
		CvAssert(GetGarrisonStrengthBonus() >= 0);

		m_pCity->plot()->plotAction(PUF_makeInfoBarDirty);
	}
}
#endif
#if defined(LEKMOD_BUILDING_FIRST_PURCHASE_DISCOUNT)
void CvCityBuildings::ChangeFirstPurchaseDiscount(int iChange)
{
	if (iChange != 0)
	{
		m_iFirstPurchaseDiscount = (m_iFirstPurchaseDiscount + iChange);
		CvAssert(GetFirstPurchaseDiscount() >= 0);
	}
}
#endif
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
int CvCityBuildings::GetCityGreatWorkClassYieldChanges(GreatWorkClass eClass, YieldTypes eYield) const
{
	CvAssertMsg(eClass >= 0, "eClass expected to be >= 0");
	CvAssertMsg(eClass < GC.getNumGreatWorkClassInfos(), "eClass expected to be < GC.getNumGreatWorkClassInfos()");
	CvAssertMsg(eYield >= 0, "eYield expected to be >= 0");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield expected to be < NUM_YIELD_TYPES");
	return NO_GREAT_WORK_CLASS != eClass ? m_aaiCityGreatWorkClassYieldChange[eClass][eYield] : 0;
}
void CvCityBuildings::ChangeCityGreatWorkClassYieldChanges(GreatWorkClass eClass, YieldTypes eYield, int iChange)
{
	CvAssertMsg(eClass >= 0, "eClass expected to be >= 0");
	CvAssertMsg(eClass < GC.getNumGreatWorkClassInfos(), "eClass expected to be < GC.getNumGreatWorkClassInfos()");
	CvAssertMsg(eYield >= 0, "eYield expected to be >= 0");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield expected to be < NUM_YIELD_TYPES");
	if (iChange == 0)
		return;
	if (NO_GREAT_WORK_CLASS != eClass)
	{
		m_aaiCityGreatWorkClassYieldChange[eClass][eYield] += iChange;
	}
	rebuildGreatWorkYields(eClass);
}
void CvCityBuildings::ChangeCityGreatWorkYieldChange(YieldTypes eYield, int iChange)
{
	CvAssertMsg(eYield >= 0, "eYield expected to be >= 0");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield expected to be < NUM_YIELD_TYPES");
	for (int i = 0; i < GC.getNumGreatWorkClassInfos(); i++)
	{
		ChangeCityGreatWorkClassYieldChanges(static_cast<GreatWorkClass>(i), eYield, iChange);
	}
}
#endif
#ifdef NQ_BUILDING_DEFENSE_FROM_CITIZENS

int CvCityBuildings::GetBuildingDefensePerCitizen() const
{
	return m_iBuildingDefensePerCitizen;
}


void CvCityBuildings::ChangeBuildingDefensePerCitizen(int iChange)
{
	if(iChange != 0)
	{
		m_iBuildingDefensePerCitizen = (m_iBuildingDefensePerCitizen + iChange);
		CvAssert(GetBuildingDefensePerCitizen() >= 0);

		m_pCity->plot()->plotAction(PUF_makeInfoBarDirty);
	}
}
#endif



int CvCityBuildings::GetBuildingDefenseMod() const
{
	return m_iBuildingDefenseMod;
}


void CvCityBuildings::ChangeBuildingDefenseMod(int iChange)
{
	if(iChange != 0)
	{
		m_iBuildingDefenseMod = (m_iBuildingDefenseMod + iChange);
		CvAssert(m_iBuildingDefenseMod >= 0);

		m_pCity->plot()->plotAction(PUF_makeInfoBarDirty);
	}
}


int CvCityBuildings::GetMissionaryExtraSpreads() const
{
	return m_iMissionaryExtraSpreads;
}


void CvCityBuildings::ChangeMissionaryExtraSpreads(int iChange)
{
	if(iChange != 0)
	{
		m_iMissionaryExtraSpreads = (m_iMissionaryExtraSpreads + iChange);
		CvAssert(m_iMissionaryExtraSpreads >= 0);
	}
}

void CvCityBuildings::IncrementWonderStats(BuildingClassTypes eIndex)
{
	CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eIndex);
	if(pkBuildingClassInfo == NULL)
		return;

	const char* szWonderTypeChar = pkBuildingClassInfo->GetType();
	CvString szWonderType = szWonderTypeChar;

	if(szWonderType == "BUILDINGCLASS_HEROIC_EPIC")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_HEROICEPIC);
	}
	else if(szWonderType == "BUILDINGCLASS_NATIONAL_COLLEGE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_NATIONALCOLLEGE);
	}
	else if(szWonderType == "BUILDINGCLASS_NATIONAL_EPIC")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_NATIONALEPIC);
	}
	else if(szWonderType == "BUILDINGCLASS_IRONWORKS")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_IRONWORKS);
	}
	else if(szWonderType == "BUILDINGCLASS_OXFORD_UNIVERSITY")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_OXFORDUNIVERSITY);
	}
	else if(szWonderType == "BUILDINGCLASS_HERMITAGE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_HERMITAGE);
	}
	else if(szWonderType == "BUILDINGCLASS_GREAT_LIGHTHOUSE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_GREATLIGHTHOUSE);
	}
	else if(szWonderType == "BUILDINGCLASS_STONEHENGE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_STONEHENGE);
	}
	else if(szWonderType == "BUILDINGCLASS_GREAT_LIBRARY")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_GREATLIBRARY);
	}
	else if(szWonderType == "BUILDINGCLASS_PYRAMID")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_PYRAMIDS);
	}
	else if(szWonderType == "BUILDINGCLASS_COLOSSUS")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_COLOSSUS);
	}
	else if(szWonderType == "BUILDINGCLASS_ORACLE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_ORACLE);
	}
	else if(szWonderType == "BUILDINGCLASS_HANGING_GARDEN")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_HANGINGGARDENS);
	}
	else if(szWonderType == "BUILDINGCLASS_GREAT_WALL")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_GREATWALL);
	}
	else if(szWonderType == "BUILDINGCLASS_ANGKOR_WAT")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_ANGKORWAT);
	}
	else if(szWonderType == "BUILDINGCLASS_HAGIA_SOPHIA")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_HAGIASOPHIA);
	}
	else if(szWonderType == "BUILDINGCLASS_CHICHEN_ITZA")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_CHICHENITZA);
	}
	else if(szWonderType == "BUILDINGCLASS_MACHU_PICHU")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_MACHUPICCHU);
	}
	else if(szWonderType == "BUILDINGCLASS_NOTRE_DAME")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_NOTREDAME);
	}
	else if(szWonderType == "BUILDINGCLASS_PORCELAIN_TOWER")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_PORCELAINTOWER);
	}
	else if(szWonderType == "BUILDINGCLASS_HIMEJI_CASTLE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_HIMEJICASTLE);
	}
	else if(szWonderType == "BUILDINGCLASS_SISTINE_CHAPEL")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_SISTINECHAPEL);
	}
	else if(szWonderType == "BUILDINGCLASS_KREMLIN")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_KREMLIN);
	}
	else if(szWonderType == "BUILDINGCLASS_FORBIDDEN_PALACE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_FORBIDDENPALACE);
	}
	else if(szWonderType == "BUILDINGCLASS_TAJ_MAHAL")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_TAJMAHAL);
	}
	else if(szWonderType == "BUILDINGCLASS_BIG_BEN")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_BIGBEN);
	}
	else if(szWonderType == "BUILDINGCLASS_LOUVRE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_LOUVRE);
	}
	else if(szWonderType == "BUILDINGCLASS_BRANDENBURG_GATE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_BRANDENBURGGATE);
	}
	else if(szWonderType == "BUILDINGCLASS_STATUE_OF_LIBERTY")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_STATUEOFLIBERTY);
	}
	else if(szWonderType == "BUILDINGCLASS_CRISTO_REDENTOR")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_CRISTOREDENTOR);
	}
	else if(szWonderType == "BUILDINGCLASS_EIFFEL_TOWER")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_EIFFELTOWER);
	}
	else if(szWonderType == "BUILDINGCLASS_PENTAGON")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_PENTAGON);
	}
	else if(szWonderType == "BUILDINGCLASS_UNITED_NATIONS")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_UNITEDNATION);
	}
	else if(szWonderType == "BUILDINGCLASS_SYDNEY_OPERA_HOUSE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_SYDNEYOPERAHOUSE);
	}
	else if(szWonderType == "BUILDINGCLASS_STATUE_ZEUS")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_STATUEOFZEUS);
	}
	else if(szWonderType == "BUILDINGCLASS_TEMPLE_ARTEMIS")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_TEMPLEOFARTEMIS);
	}
	else if(szWonderType == "BUILDINGCLASS_MAUSOLEUM_HALICARNASSUS")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_MAUSOLEUMOFHALICARNASSUS);
	}
	else
	{
		OutputDebugString("\nNo Stat for selected Wonder: ");
		OutputDebugString(szWonderType);
		OutputDebugString("\n");
	}

	bool bCheckForWonders = false;
	bCheckForWonders = CheckForAllWondersBuilt();
	if(bCheckForWonders)
	{
		gDLL->UnlockAchievement(ACHIEVEMENT_ALL_WONDERS);
	}


	bool bCheckForAncientWonders = false;
	bCheckForAncientWonders = CheckForSevenAncientWondersBuilt();
	if(bCheckForAncientWonders)
	{
		gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_ANCIENT_WONDERS);
	}

}
bool CvCityBuildings::CheckForAllWondersBuilt()
{
	int iI;
	int iStartStatWonder = ESTEAMSTAT_ANGKORWAT;
	int iEndStatWonder = ESTEAMSTAT_PYRAMIDS;
	int32 nStat;

	for(iI = iStartStatWonder; iI < iEndStatWonder; iI++)
	{
		if(gDLL->GetSteamStat((ESteamStat)iI, &nStat))
		{
			if(nStat <= 0)
			{
				return false;
			}
		}
	}
	return true;
}

bool CvCityBuildings::CheckForSevenAncientWondersBuilt()
{
	GUID guid;
	ExtractGUID(CIV5_DLC_06_PACKAGEID, guid);

	if(gDLL->IsDLCValid(guid))
	{
		ESteamStat arrWonderStats[7] =
		{
			ESTEAMSTAT_COLOSSUS,
			ESTEAMSTAT_GREATLIGHTHOUSE,
			ESTEAMSTAT_HANGINGGARDENS,
			ESTEAMSTAT_PYRAMIDS,
			ESTEAMSTAT_STATUEOFZEUS,
			ESTEAMSTAT_TEMPLEOFARTEMIS,
			ESTEAMSTAT_MAUSOLEUMOFHALICARNASSUS
		};
		int32 nStat;
		for(int iI = 0; iI < 7; iI++)
		{
			if(gDLL->GetSteamStat(arrWonderStats[iI], &nStat))
			{
				if(nStat <= 0)
				{
					return false;
				}
			}
			else
			{

				return false;
			}
		}
		return true;
	}
	return false;
}


void CvCityBuildings::NotifyNewBuildingStarted(BuildingTypes           )
{

	return;






















































}


void BuildingArrayHelpers::Read(FDataStream& kStream, int* paiBuildingArray)
{
	int iNumEntries;
	int iType;

	kStream >> iNumEntries;

	for(int iI = 0; iI < iNumEntries; iI++)
	{
		bool bValid = true;
		iType = CvInfosSerializationHelper::ReadHashed(kStream, &bValid);
		if(iType != -1 || !bValid)
		{
			if(iType != -1)
			{
				kStream >> paiBuildingArray[iType];
			}
			else
			{
				CvString szError;
				szError.Format("LOAD ERROR: Building Type not found");
				GC.LogMessage(szError.GetCString());
				CvAssertMsg(false, szError);
				int iDummy;
				kStream >> iDummy;
			}
		}
	}
}


void BuildingArrayHelpers::Write(FDataStream& kStream, int* paiBuildingArray, int iArraySize)
{
	kStream << iArraySize;

	for(int iI = 0; iI < iArraySize; iI++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iI);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if(pkBuildingInfo)
		{
			CvInfosSerializationHelper::WriteHashed(kStream, pkBuildingInfo);;
			kStream << paiBuildingArray[iI];
		}
		else
		{
			kStream << (int)0;
		}
	}
}
