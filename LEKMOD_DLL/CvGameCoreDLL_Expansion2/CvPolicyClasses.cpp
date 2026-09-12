






#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "CvPolicyAI.h"
#include "CvFlavorManager.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvEconomicAI.h"
#include "CvGrandStrategyAI.h"
#include "CvInfosSerializationHelper.h"


#include "LintFree.h"


CvPolicyEntry::CvPolicyEntry(void):
	m_iCultureCost(0),
	m_iGridX(0),
	m_iGridY(0),
	m_iLevel(0),
	m_iPolicyCostModifier(0),
	m_iCulturePerCity(0),
#ifdef FRUITY_TRADITION_ARISTOCRACY
	m_iCapitalCulturePerUniqueLuxury(0),
#endif
	m_iCulturePerWonder(0),
	m_iCultureWonderMultiplier(0),
	m_iCulturePerTechResearched(0),
	m_iCultureImprovementChange(0),
	m_iCultureFromKills(0),
	m_iCultureFromBarbarianKills(0),
	m_iGoldFromKills(0),
	m_iScienceFromKills(0),
	m_iEmbarkedExtraMoves(0),
	m_iAttackBonusTurns(0),
	m_iGoldenAgeTurns(0),
	m_iGoldenAgeMeterMod(0),
	m_iGoldenAgeDurationMod(0),
	m_iNumFreeTechs(0),
	m_iNumFreePolicies(0),
	m_iNumFreeGreatPeople(0),
#ifdef NQ_EXTRA_SPIES_FROM_POLICIES
	m_iNumExtraSpies(0),
#endif
#if defined(MISC_CHANGES)
	m_iNumExtraLeagueVotes(0),
	m_iNumTradeRouteBonus(0),
#endif
	m_iMedianTechPercentChange(0),
	m_iStrategicResourceMod(0),
	m_iWonderProductionModifier(0),
	m_iBuildingProductionModifier(0),
	m_iGreatPeopleRateModifier(0),
	m_iGreatGeneralRateModifier(0),
	m_iGreatAdmiralRateModifier(0),
	m_iGreatWriterRateModifier(0),
	m_iGreatArtistRateModifier(0),
	m_iGreatMusicianRateModifier(0),
	m_iGreatMerchantRateModifier(0),
	m_iGreatScientistRateModifier(0),
	m_iDomesticGreatGeneralRateModifier(0),
	m_iExtraHappiness(0),
	m_iExtraHappinessPerCity(0),
	m_iUnhappinessMod(0),
	m_iCityCountUnhappinessMod(0),
	m_iOccupiedPopulationUnhappinessMod(0),
	m_iCapitalUnhappinessMod(0),
	m_iFreeExperience(0),
	m_iWorkerSpeedModifier(0),
	m_iAllFeatureProduction(0),
	m_iImprovementCostModifier(0),
	m_iImprovementUpgradeRateModifier(0),
	m_iSpecialistProductionModifier(0),
	m_iSpecialistUpgradeModifier(0),
	m_iMilitaryProductionModifier(0),
	m_iBaseFreeUnits(0),
	m_iBaseFreeMilitaryUnits(0),
	m_iFreeUnitsPopulationPercent(0),
	m_iFreeMilitaryUnitsPopulationPercent(0),
	m_iHappinessPerGarrisonedUnit(0),
	m_iCulturePerGarrisonedUnit(0),
	m_iHappinessPerTradeRoute(0),
#ifdef NQ_RAIL_CONNECTION_HAPPINESS_FROM_POLICIES
	m_iHappinessPerRailConnection(0),
#endif
	m_iHappinessPerXPopulation(0),
	m_iExtraHappinessPerLuxury(0),
	m_iUnhappinessFromUnitsMod(0),
	m_iNumExtraBuilders(0),
	m_iPlotGoldCostMod(0),
	m_iPlotCultureCostModifier(0),
	m_iPlotCultureExponentModifier(0),
	m_iNumCitiesPolicyCostDiscount(0),
	m_iNumCitiesResearchCostDiscount(0),
	m_iGarrisonedCityRangeStrikeModifier(0),
	m_iUnitPurchaseCostModifier(0),
	m_iBuildingPurchaseCostModifier(0),
	m_iCityConnectionTradeRouteGoldModifier(0),
	m_iTradeMissionGoldModifier(0),
	m_iFaithCostModifier(0),
	m_iCulturalPlunderMultiplier(0),
	m_iStealTechSlowerModifier(0),
	m_iStealTechFasterModifier(0),
	m_iCatchSpiesModifier(0),
	m_iGoldPerUnit(0),
	m_iGoldPerMilitaryUnit(0),
	m_iCityStrengthMod(0),
	m_iCityGrowthMod(0),
	m_iCapitalGrowthMod(0),
	m_iSettlerProductionModifier(0),
	m_iCapitalSettlerProductionModifier(0),
	m_iNewCityExtraPopulation(0),
	m_iFreeFoodBox(0),
	m_iRouteGoldMaintenanceMod(0),
	m_iBuildingGoldMaintenanceMod(0),
	m_iUnitGoldMaintenanceMod(0),
	m_iUnitSupplyMod(0),
	m_iHappyPerMilitaryUnit(0),
	m_iFreeSpecialist(0),
	m_iTechPrereq(NO_TECH),
	m_iMaxConscript(0),
	m_iExpModifier(0),
	m_iExpInBorderModifier(0),
	m_iMinorQuestFriendshipMod(0),
	m_iMinorGoldFriendshipMod(0),
	m_iMinorFriendshipMinimum(0),
	m_iMinorFriendshipDecayMod(0),
	m_iOtherPlayersMinorFriendshipDecayMod(0),
	m_iCityStateUnitFrequencyModifier(0),
	m_iCommonFoeTourismModifier(0),
	m_iLessHappyTourismModifier(0),
	m_iSharedIdeologyTourismModifier(0),
	m_iLandTradeRouteGoldChange(0),
	m_iSeaTradeRouteGoldChange(0),
	m_iInternalTradeRouteGoldChange(0),
	m_iSharedIdeologyTradeGoldChange(0),
	m_iRiggingElectionModifier(0),
	m_iMilitaryUnitGiftExtraInfluence(0),
	m_iProtectedMinorPerTurnInfluence(0),
	m_iAfraidMinorPerTurnInfluence(0),
	m_iMinorBullyScoreModifier(0),
#if defined(LEKMOD_POLICY_MINOR_BULLY_TRIBUTE)
	m_iMinorBullyInfluenceReward(0),
	m_bMinorBullyNoPenalty(false),
#endif
	m_iThemingBonusMultiplier(0),
	m_iInternalTradeRouteYieldModifier(0),
#ifdef FRUITY_TRADITION_LANDED_ELITE 
	m_iInternalTradeRouteFoodYieldChange(0),
#endif
#ifdef NQ_INTERNAL_TRADE_ROUTE_PRODUCTION_YIELD_CHANGE_FROM_POLICIES
	m_iInternalTradeRouteProductionYieldChange(0),
#endif
#ifdef NQ_RAIL_CONNECTION_PRODUCTION_MODIFIER_FROM_POLICIES
	m_iRailConnectionProductionModifier(0),
#endif
#ifdef NQ_COMBAT_BONUS_VS_SMALLER_CIV_FROM_POLICIES
	m_iCombatBonusVsSmallerCiv(0),
#endif
#ifdef NQ_PRODUCTION_TO_GREAT_MUSICIANS_MODIFIER_FROM_POLICIES
	m_iProductionToGreatMusiciansModifier(0),
#endif
#ifdef NQ_TRADE_MISSION_INFLUENCE_MODIFIER_FROM_POLICIES
	m_iTradeMissionInfluenceModifier(0),
#endif
#ifdef NQ_SCIENCE_PER_GREAT_PERSON_BORN_FROM_POLICIES
	m_iSciencePerGreatPersonBorn(0),
#endif
#ifdef NQ_INFLUENCE_BOOST_PER_GREAT_PERSON_BORN_FROM_POLICIES
	m_iInfluenceBoostPerGreatPersonBorn(0),
#endif
#ifdef NQ_TOURISM_FROM_TRADE_MISSIONS_FROM_POLICIES
	m_iTourismFromTradeMissions(0),
#endif
#ifdef NQ_HAPPINESS_FROM_GREAT_IMPROVEMENTS_FROM_POLICIES
	m_iHappinessFromGreatImprovements(0),
#endif
#ifdef NQ_MINOR_FRIENDSHIP_GAIN_BULLY_GOLD_SUCCESS_FROM_POLICIES
	m_iMinorFriendshipGainBullyGoldSuccess(0),
#endif
#ifdef NQ_COUP_CHANCE_MODIFIER_FROM_POLICIES
	m_iCoupChanceModifier(0),
#endif
	m_iSharedReligionTourismModifier(0),
	m_iTradeRouteTourismModifier(0),
	m_iOpenBordersTourismModifier(0),
	m_iCityStateTradeChange(0),
	m_bMinorGreatPeopleAllies(false),
	m_bMinorScienceAllies(false),
	m_bMinorResourceBonus(false),
	m_iMinorMilitaryNumExtraUnitsToGift(0),
	m_iCityStateBonusModifier(0),
	m_iExtraTerritoryClaim(0),
	m_iExtraTourismPerGreatWork(0),
	m_iTourismPerWonder(0),
#ifdef NQ_TOURISM_PER_CITY
	m_iTourismPerCity(0),
#endif
#ifdef NQ_IDEOLOGY_PRESSURE_UNHAPPINESS_MODIFIER_FROM_POLICIES
	m_iIdeologyPressureUnhappinessModifier(0),
#endif

	m_iProductionFromGarrison(0),
	m_bGoldenAgeCultureBonusDisabled(false),
	m_bSecondReligionPantheon(false),
	m_bAddReformationBelief(false),
	m_bEnablesSSPartHurry(false),
#ifdef NQ_DIABLE_RESISTANCE_TIME_VIA_POLICIES
	m_bDisablesResistanceTime(false),
#endif
#ifdef NQ_PATRIOTIC_WAR
	m_bDoubleTrainedMilitaryLandUnit(false),
#endif
#ifdef NQ_WAR_HERO
	m_bWarHero(false),
#endif
#ifdef NQ_IGNORE_PUPPETS_FOR_RESEARCH_COSTS_FROM_POLICIES
	m_bIgnorePuppetsForResearchCosts(false),
#endif
#ifdef NQ_POLICY_TOGGLE_NO_MINOR_DOW_IF_FRIENDS
	m_bNoMinorDOWIfFriends(false),
#endif
#ifdef NQ_ALLOW_PUPPET_PURCHASING_FROM_POLICIES
	m_bAllowPuppetPurchasing(false),
#endif
	m_bEnablesSSPartPurchase(false),
	m_iPolicyBranchType(NO_POLICY_BRANCH_TYPE),
	m_iNumExtraBranches(0),
	m_iHappinessToCulture(0),
	m_iHappinessToScience(0),
#ifdef NQ_GOLD_TO_SCIENCE_FROM_POLICIES
	m_iGoldToScience(0),
#endif
#ifdef NQ_SPREAD_MODIFIER_OWNED_CITIES
	m_iSpreadModifierOwnedCities(0),
	m_iSpreadModifierUnownedCities(0),
#endif
	m_iNumCitiesFreeAestheticsSchools(0),
	m_iNumCitiesFreePietyGardens(0),
	m_iNumCitiesFreeWalls(0),
	m_iNumCitiesFreeCultureBuilding(0),
	m_iNumCitiesFreeFoodBuilding(0),
	m_bHalfSpecialistUnhappiness(false),
	m_bHalfMoreSpecialistUnhappiness(false),
	m_bHalfSpecialistFood(false),
	m_bMilitaryFoodProduction(false),
	m_iWoundedUnitDamageMod(0),
	m_iUnitUpgradeCostMod(0),
	m_iBarbarianCombatBonus(0),
	m_bAlwaysSeeBarbCamps(false),
	m_bRevealAllCapitals(false),
	m_bGarrisonFreeMaintenance(false),
	m_bAbleToAnnexCityStates(false),
	m_bOneShot(false),
	m_bIncludesOneShotFreeUnits(false),
	m_piPrereqOrPolicies(NULL),
	m_piPrereqAndPolicies(NULL),
	m_piPolicyDisables(NULL),
	m_piYieldModifier(NULL),
	m_piCityYieldChange(NULL),
	m_piCoastalCityYieldChange(NULL),
	m_piCapitalYieldChange(NULL),
	m_piCapitalYieldPerPopChange(NULL),
	m_piCapitalYieldModifier(NULL),
	m_piGreatWorkYieldChange(NULL),
	m_piSpecialistExtraYield(NULL),
	m_pabFreePromotion(NULL),
	m_paiUnitCombatProductionModifiers(NULL),
	m_paiUnitCombatFreeExperiences(NULL),
	m_paiBuildingClassCultureChanges(NULL),
	m_paiBuildingClassProductionModifiers(NULL),
	m_paiBuildingClassTourismModifiers(NULL),
	m_paiBuildingClassHappiness(NULL),
	m_paiFreeUnitClasses(NULL),
	m_paiTourismOnUnitCreation(NULL),
#if defined(TRADE_REFACTOR)
	m_ppiMinorTradeRouteDomainYieldChanges(NULL),
	m_ppiTradeConnectionLandYieldChanges(NULL),
	m_ppiTradeConnectionSeaYieldChanges(NULL),
	m_ppiTradeConnectionLandYieldModifiers(NULL),
	m_ppiTradeConnectionSeaYieldModifiers(NULL),
#endif
#if defined(FULL_YIELD_FROM_KILLS)
	m_paiYieldFromKills(NULL),
	m_paiYieldFromKillsMax(NULL),
#endif
#if defined(LEKMOD_v34)
	m_piPolicyResourceQuantity(NULL),
	m_ppiPolicyResourceYieldChanges(NULL),
	m_ppiPolicyResourceClassYieldChanges(NULL),
#endif
#if defined(LEKMOD_FIX_SCHOLASTICISM)
	m_paiMinorFriendYieldBonus(NULL),
	m_paiMinorAllyYieldBonus(NULL),
#endif
#if defined(LEKMOD_EXPERIMENTAL_CHANGES)
	m_piWorldWonderYieldChanges(NULL),
#endif
	m_paiHurryModifier(NULL),
	m_pabSpecialistValid(NULL),
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	m_ppiImprovementYieldChanges(std::pair<int**, size_t>(NULL, 0)),
	m_ppiBuildingClassYieldModifiers(std::pair<int**, size_t>(NULL, 0)),
	m_ppiBuildingClassYieldChanges(std::pair<int**, size_t>(NULL, 0)),
#ifdef AUI_POLICY_BUILDING_CLASS_FLAVOR_MODIFIERS
	m_ppiBuildingClassFlavorChanges(std::pair<int**, size_t>(NULL, 0)),
#endif
#else
	m_ppiImprovementYieldChanges(NULL),
	m_ppiBuildingClassYieldModifiers(NULL),
	m_ppiBuildingClassYieldChanges(NULL),
#ifdef AUI_POLICY_BUILDING_CLASS_FLAVOR_MODIFIERS
	m_ppiBuildingClassFlavorChanges(NULL),
#endif
#endif
#if defined(LEKMOD_POLICY_TERRAIN_FEATURE_YIELDS)
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	m_ppiPolicyTerrainYieldChange(std::pair<int**, size_t>(NULL, 0)),
	m_ppiPolicyTerrainYieldChangeUnimproved(std::pair<int**, size_t>(NULL, 0)),
	m_ppiPolicyTerrainYieldChangeNoResource(std::pair<int**, size_t>(NULL, 0)),
	m_ppiPolicyTerrainYieldChangeUnimprovedNoResource(std::pair<int**, size_t>(NULL, 0)),
	m_ppiPolicyTerrainYieldChangeExcludingLakes(std::pair<int**, size_t>(NULL, 0)),
	m_ppiPolicyTerrainYieldChangeUnimprovedExcludingLakes(std::pair<int**, size_t>(NULL, 0)),
	m_ppiPolicyTerrainYieldChangeNoResourceExcludingLakes(std::pair<int**, size_t>(NULL, 0)),
	m_ppiPolicyTerrainYieldChangeUnimprovedNoResourceExcludingLakes(std::pair<int**, size_t>(NULL, 0)),
	m_ppiPolicyFeatureYieldChange(std::pair<int**, size_t>(NULL, 0)),
	m_ppiPolicyFeatureYieldChangeUnimproved(std::pair<int**, size_t>(NULL, 0)),
	m_ppiPolicyFeatureYieldChangeNoResource(std::pair<int**, size_t>(NULL, 0)),
	m_ppiPolicyFeatureYieldChangeUnimprovedNoResource(std::pair<int**, size_t>(NULL, 0)),
#else
	m_ppiPolicyTerrainYieldChange(NULL),
	m_ppiPolicyTerrainYieldChangeUnimproved(NULL),
	m_ppiPolicyTerrainYieldChangeNoResource(NULL),
	m_ppiPolicyTerrainYieldChangeUnimprovedNoResource(NULL),
	m_ppiPolicyTerrainYieldChangeExcludingLakes(NULL),
	m_ppiPolicyTerrainYieldChangeUnimprovedExcludingLakes(NULL),
	m_ppiPolicyTerrainYieldChangeNoResourceExcludingLakes(NULL),
	m_ppiPolicyTerrainYieldChangeUnimprovedNoResourceExcludingLakes(NULL),
	m_ppiPolicyFeatureYieldChange(NULL),
	m_ppiPolicyFeatureYieldChangeUnimproved(NULL),
	m_ppiPolicyFeatureYieldChangeNoResource(NULL),
	m_ppiPolicyFeatureYieldChangeUnimprovedNoResource(NULL),
#endif
#endif
#if defined(LEKMOD_POLICY_GREATPERSON_IMPROVEMENT_ADJACENCY_YIELD)
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	m_ppiPolicyGreatPersonImprovementAdjacencyYieldBonus(std::pair<int**, size_t>(NULL, 0)),
#else
	m_ppiPolicyGreatPersonImprovementAdjacencyYieldBonus(NULL),
#endif
#endif
#ifdef AUI_WARNING_FIXES
	m_piImprovementCultureChange(NULL),
#endif
	m_piFlavorValue(NULL),
	m_eFreeBuildingOnConquest(NO_BUILDING),
#ifdef LEKMOD_POLICIES_GLOBAL_MOVE_CHANGE
	m_iGlobalMoveChange(0),
	m_iGlobalMoveChangeFriendly(0),
	m_iGlobalMoveChangeEnemy(0)
#endif
{

}

CvPolicyEntry::~CvPolicyEntry(void)
{
	SAFE_DELETE_ARRAY(m_piPrereqOrPolicies);
	SAFE_DELETE_ARRAY(m_piPrereqAndPolicies);
	SAFE_DELETE_ARRAY(m_piPolicyDisables);
	SAFE_DELETE_ARRAY(m_piYieldModifier);
	SAFE_DELETE_ARRAY(m_piCityYieldChange);
	SAFE_DELETE_ARRAY(m_piCoastalCityYieldChange);
	SAFE_DELETE_ARRAY(m_piCapitalYieldChange);
	SAFE_DELETE_ARRAY(m_piCapitalYieldPerPopChange);
	SAFE_DELETE_ARRAY(m_piCapitalYieldModifier);
	SAFE_DELETE_ARRAY(m_piGreatWorkYieldChange);
	SAFE_DELETE_ARRAY(m_piSpecialistExtraYield);
	SAFE_DELETE_ARRAY(m_pabFreePromotion);
	SAFE_DELETE_ARRAY(m_paiUnitCombatProductionModifiers);
	SAFE_DELETE_ARRAY(m_paiUnitCombatFreeExperiences);
	SAFE_DELETE_ARRAY(m_paiBuildingClassCultureChanges);
	SAFE_DELETE_ARRAY(m_paiBuildingClassProductionModifiers);
	SAFE_DELETE_ARRAY(m_paiBuildingClassTourismModifiers);
	SAFE_DELETE_ARRAY(m_paiBuildingClassHappiness);
	SAFE_DELETE_ARRAY(m_paiFreeUnitClasses);
	SAFE_DELETE_ARRAY(m_paiTourismOnUnitCreation);
#if defined(TRADE_REFACTOR)
	CvDatabaseUtility::SafeDelete2DArray(m_ppiMinorTradeRouteDomainYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiTradeConnectionLandYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiTradeConnectionSeaYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiTradeConnectionLandYieldModifiers);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiTradeConnectionSeaYieldModifiers);
#endif
#if defined(FULL_YIELD_FROM_KILLS)
	SAFE_DELETE_ARRAY(m_paiYieldFromKills);
	SAFE_DELETE_ARRAY(m_paiYieldFromKillsMax);
#endif
#if defined(LEKMOD_v34)
	SAFE_DELETE_ARRAY(m_piPolicyResourceQuantity);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyResourceYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyResourceClassYieldChanges);
#endif
#if defined(LEKMOD_FIX_SCHOLASTICISM)
	CvDatabaseUtility::SafeDelete2DArray(m_paiMinorFriendYieldBonus);
	CvDatabaseUtility::SafeDelete2DArray(m_paiMinorAllyYieldBonus);
#endif
#if defined(LEKMOD_EXPERIMENTAL_CHANGES)
	SAFE_DELETE_ARRAY(m_piWorldWonderYieldChanges);
#endif


	SAFE_DELETE_ARRAY(m_paiHurryModifier);
	SAFE_DELETE_ARRAY(m_pabSpecialistValid);

#ifdef AUI_WARNING_FIXES
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piImprovementCultureChange);
#endif

#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	CvDatabaseUtility::SafeDelete2DArray(m_ppiImprovementYieldChanges.first, m_ppiImprovementYieldChanges.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiBuildingClassYieldModifiers.first, m_ppiBuildingClassYieldModifiers.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiBuildingClassYieldChanges.first, m_ppiBuildingClassYieldChanges.second);
#ifdef AUI_POLICY_BUILDING_CLASS_FLAVOR_MODIFIERS
	CvDatabaseUtility::SafeDelete2DArray(m_ppiBuildingClassFlavorChanges.first, m_ppiBuildingClassFlavorChanges.second);
#endif
#else
	CvDatabaseUtility::SafeDelete2DArray(m_ppiImprovementYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiBuildingClassYieldModifiers);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiBuildingClassYieldChanges);
#ifdef AUI_POLICY_BUILDING_CLASS_FLAVOR_MODIFIERS
	CvDatabaseUtility::SafeDelete2DArray(m_ppiBuildingClassFlavorChanges);
#endif
#endif
#if defined(LEKMOD_POLICY_TERRAIN_FEATURE_YIELDS)
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyTerrainYieldChange.first, m_ppiPolicyTerrainYieldChange.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyTerrainYieldChangeUnimproved.first, m_ppiPolicyTerrainYieldChangeUnimproved.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyTerrainYieldChangeNoResource.first, m_ppiPolicyTerrainYieldChangeNoResource.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyTerrainYieldChangeUnimprovedNoResource.first, m_ppiPolicyTerrainYieldChangeUnimprovedNoResource.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyTerrainYieldChangeExcludingLakes.first, m_ppiPolicyTerrainYieldChangeExcludingLakes.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyTerrainYieldChangeUnimprovedExcludingLakes.first, m_ppiPolicyTerrainYieldChangeUnimprovedExcludingLakes.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyTerrainYieldChangeNoResourceExcludingLakes.first, m_ppiPolicyTerrainYieldChangeNoResourceExcludingLakes.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyTerrainYieldChangeUnimprovedNoResourceExcludingLakes.first, m_ppiPolicyTerrainYieldChangeUnimprovedNoResourceExcludingLakes.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyFeatureYieldChange.first, m_ppiPolicyFeatureYieldChange.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyFeatureYieldChangeUnimproved.first, m_ppiPolicyFeatureYieldChangeUnimproved.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyFeatureYieldChangeNoResource.first, m_ppiPolicyFeatureYieldChangeNoResource.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyFeatureYieldChangeUnimprovedNoResource.first, m_ppiPolicyFeatureYieldChangeUnimprovedNoResource.second);
#else
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyTerrainYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyTerrainYieldChangeUnimproved);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyTerrainYieldChangeNoResource);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyTerrainYieldChangeUnimprovedNoResource);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyTerrainYieldChangeExcludingLakes);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyTerrainYieldChangeUnimprovedExcludingLakes);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyTerrainYieldChangeNoResourceExcludingLakes);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyTerrainYieldChangeUnimprovedNoResourceExcludingLakes);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyFeatureYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyFeatureYieldChangeUnimproved);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyFeatureYieldChangeNoResource);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyFeatureYieldChangeUnimprovedNoResource);
#endif
#endif
#if defined(LEKMOD_POLICY_GREATPERSON_IMPROVEMENT_ADJACENCY_YIELD)
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyGreatPersonImprovementAdjacencyYieldBonus.first, m_ppiPolicyGreatPersonImprovementAdjacencyYieldBonus.second);
#else
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPolicyGreatPersonImprovementAdjacencyYieldBonus);
#endif
#endif

#ifdef LEKMOD_UNITCOMBAT_FREE_PROMOTION
	CvDatabaseUtility::SafeDelete2DArray(m_FreePromotionUnitCombats);
#endif
}


bool CvPolicyEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;


	m_iCultureCost = kResults.GetInt("CultureCost");
	m_iGridX = kResults.GetInt("GridX");
	m_iGridY = kResults.GetInt("GridY");
	m_iLevel = kResults.GetInt("Level");
	m_iPolicyCostModifier = kResults.GetInt("PolicyCostModifier");
	m_iCulturePerCity = kResults.GetInt("CulturePerCity");
#ifdef FRUITY_TRADITION_ARISTOCRACY
	m_iCapitalCulturePerUniqueLuxury = kResults.GetInt("CapitalCulturePerUniqueLuxury");
#endif
#if !defined(LEKMOD_EXPERIMENTAL_CHANGES)
	m_iCulturePerWonder = kResults.GetInt("CulturePerWonder");
#endif
	m_iCultureWonderMultiplier = kResults.GetInt("CultureWonderMultiplier");
	m_iCulturePerTechResearched = kResults.GetInt("CulturePerTechResearched");
	m_iCultureImprovementChange = kResults.GetInt("CultureImprovementChange");
	m_iCultureFromKills = kResults.GetInt("CultureFromKills");
	m_iCultureFromBarbarianKills = kResults.GetInt("CultureFromBarbarianKills");
	m_iGoldFromKills = kResults.GetInt("GoldFromKills");
	m_iScienceFromKills = kResults.GetInt("ScienceFromKills");
	m_iEmbarkedExtraMoves = kResults.GetInt("EmbarkedExtraMoves");
	m_iAttackBonusTurns = kResults.GetInt("AttackBonusTurns");
	m_iGoldenAgeTurns = kResults.GetInt("GoldenAgeTurns");
	m_iGoldenAgeMeterMod = kResults.GetInt("GoldenAgeMeterMod");
	m_iGoldenAgeDurationMod = kResults.GetInt("GoldenAgeDurationMod");
	m_iNumFreeTechs = kResults.GetInt("NumFreeTechs");
	m_iNumFreePolicies = kResults.GetInt("NumFreePolicies");
	m_iNumFreeGreatPeople = kResults.GetInt("NumFreeGreatPeople");
#ifdef NQ_EXTRA_SPIES_FROM_POLICIES
	m_iNumExtraSpies = kResults.GetInt("NumExtraSpies");
#endif
#if defined(MISC_CHANGES)
	m_iNumExtraLeagueVotes = kResults.GetInt("NumExtraLeagueVotes");
	m_iNumTradeRouteBonus = kResults.GetInt("NumTradeRouteBonus");
#endif
	m_iMedianTechPercentChange = kResults.GetInt("MedianTechPercentChange");
	m_iStrategicResourceMod = kResults.GetInt("StrategicResourceMod");
	m_iWonderProductionModifier = kResults.GetInt("WonderProductionModifier");
	m_iBuildingProductionModifier = kResults.GetInt("BuildingProductionModifier");
	m_iGreatPeopleRateModifier = kResults.GetInt("GreatPeopleRateModifier");
	m_iGreatGeneralRateModifier = kResults.GetInt("GreatGeneralRateModifier");
	m_iGreatAdmiralRateModifier = kResults.GetInt("GreatAdmiralRateModifier");
	m_iGreatWriterRateModifier = kResults.GetInt("GreatWriterRateModifier");
	m_iGreatArtistRateModifier = kResults.GetInt("GreatArtistRateModifier");
	m_iGreatMusicianRateModifier = kResults.GetInt("GreatMusicianRateModifier");
	m_iGreatMerchantRateModifier = kResults.GetInt("GreatMerchantRateModifier");
	m_iGreatScientistRateModifier = kResults.GetInt("GreatScientistRateModifier");
	m_iDomesticGreatGeneralRateModifier = kResults.GetInt("DomesticGreatGeneralRateModifier");
	m_iExtraHappiness = kResults.GetInt("ExtraHappiness");
	m_iExtraHappinessPerCity = kResults.GetInt("ExtraHappinessPerCity");
	m_iUnhappinessMod = kResults.GetInt("UnhappinessMod");
	m_iCityCountUnhappinessMod = kResults.GetInt("CityCountUnhappinessMod");
	m_iOccupiedPopulationUnhappinessMod = kResults.GetInt("OccupiedPopulationUnhappinessMod");
	m_iCapitalUnhappinessMod = kResults.GetInt("CapitalUnhappinessMod");
	m_iFreeExperience = kResults.GetInt("FreeExperience");
	m_iWorkerSpeedModifier = kResults.GetInt("WorkerSpeedModifier");
	m_iAllFeatureProduction = kResults.GetInt("AllFeatureProduction");
	m_iImprovementCostModifier = kResults.GetInt("ImprovementCostModifier");
	m_iImprovementUpgradeRateModifier = kResults.GetInt("ImprovementUpgradeRateModifier");
	m_iSpecialistProductionModifier = kResults.GetInt("SpecialistProductionModifier");
	m_iSpecialistUpgradeModifier = kResults.GetInt("SpecialistUpgradeModifier");
	m_iMilitaryProductionModifier = kResults.GetInt("MilitaryProductionModifier");
	m_iBaseFreeUnits = kResults.GetInt("BaseFreeUnits");
	m_iBaseFreeMilitaryUnits = kResults.GetInt("BaseFreeMilitaryUnits");
	m_iFreeUnitsPopulationPercent = kResults.GetInt("FreeUnitsPopulationPercent");
	m_iFreeMilitaryUnitsPopulationPercent = kResults.GetInt("FreeMilitaryUnitsPopulationPercent");
	m_iHappinessPerGarrisonedUnit = kResults.GetInt("HappinessPerGarrisonedUnit");
	m_iCulturePerGarrisonedUnit = kResults.GetInt("CulturePerGarrisonedUnit");
	m_iHappinessPerTradeRoute = kResults.GetInt("HappinessPerTradeRoute");
#ifdef NQ_RAIL_CONNECTION_HAPPINESS_FROM_POLICIES
	m_iHappinessPerRailConnection = kResults.GetInt("HappinessPerRailConnection");
#endif
	m_iHappinessPerXPopulation = kResults.GetInt("HappinessPerXPopulation");
	m_iExtraHappinessPerLuxury = kResults.GetInt("ExtraHappinessPerLuxury");
	m_iUnhappinessFromUnitsMod = kResults.GetInt("UnhappinessFromUnitsMod");
	m_iNumExtraBuilders = kResults.GetInt("NumExtraBuilders");
	m_iPlotGoldCostMod = kResults.GetInt("PlotGoldCostMod");
	m_iPlotCultureCostModifier = kResults.GetInt("PlotCultureCostModifier");
	m_iPlotCultureExponentModifier = kResults.GetInt("PlotCultureExponentModifier");
	m_iNumCitiesPolicyCostDiscount = kResults.GetInt("NumCitiesPolicyCostDiscount");
	m_iNumCitiesResearchCostDiscount = kResults.GetInt("NumCitiesResearchCostDiscount");
	m_iGarrisonedCityRangeStrikeModifier = kResults.GetInt("GarrisonedCityRangeStrikeModifier");
	m_iUnitPurchaseCostModifier = kResults.GetInt("UnitPurchaseCostModifier");
	m_iBuildingPurchaseCostModifier = kResults.GetInt("BuildingPurchaseCostModifier");
	m_iCityConnectionTradeRouteGoldModifier = kResults.GetInt("CityConnectionTradeRouteGoldModifier");
	m_iTradeMissionGoldModifier = kResults.GetInt("TradeMissionGoldModifier");
	m_iFaithCostModifier = kResults.GetInt("FaithCostModifier");
	m_iCulturalPlunderMultiplier = kResults.GetInt("CulturalPlunderMultiplier");
	m_iStealTechSlowerModifier = kResults.GetInt("StealTechSlowerModifier");
	m_iStealTechFasterModifier = kResults.GetInt("StealTechFasterModifier");
	m_iCatchSpiesModifier = kResults.GetInt("CatchSpiesModifier");
	m_iGoldPerUnit = kResults.GetInt("GoldPerUnit");
	m_iGoldPerMilitaryUnit = kResults.GetInt("GoldPerMilitaryUnit");
	m_iCityStrengthMod = kResults.GetInt("CityStrengthMod");
	m_iCityGrowthMod = kResults.GetInt("CityGrowthMod");
	m_iCapitalGrowthMod = kResults.GetInt("CapitalGrowthMod");
	m_iSettlerProductionModifier = kResults.GetInt("SettlerProductionModifier");
	m_iCapitalSettlerProductionModifier = kResults.GetInt("CapitalSettlerProductionModifier");
	m_iNewCityExtraPopulation = kResults.GetInt("NewCityExtraPopulation");
	m_iFreeFoodBox = kResults.GetInt("FreeFoodBox");
	m_iRouteGoldMaintenanceMod = kResults.GetInt("RouteGoldMaintenanceMod");
	m_iBuildingGoldMaintenanceMod = kResults.GetInt("BuildingGoldMaintenanceMod");
	m_iUnitGoldMaintenanceMod = kResults.GetInt("UnitGoldMaintenanceMod");
	m_iUnitSupplyMod = kResults.GetInt("UnitSupplyMod");
	m_iHappyPerMilitaryUnit = kResults.GetInt("HappyPerMilitaryUnit");
	m_iHappinessToCulture = kResults.GetInt("HappinessToCulture");
	m_iHappinessToScience = kResults.GetInt("HappinessToScience");
#ifdef NQ_GOLD_TO_SCIENCE_FROM_POLICIES
	m_iGoldToScience = kResults.GetInt("GoldToScience");
#endif
	m_iNumCitiesFreeAestheticsSchools = kResults.GetInt("NumCitiesFreeAestheticsSchools");
	m_iNumCitiesFreePietyGardens = kResults.GetInt("NumCitiesFreePietyGardens");
	m_iNumCitiesFreeWalls = kResults.GetInt("NumCitiesFreeWalls");
	m_iNumCitiesFreeCultureBuilding = kResults.GetInt("NumCitiesFreeCultureBuilding");
	m_iNumCitiesFreeFoodBuilding = kResults.GetInt("NumCitiesFreeFoodBuilding");
	m_bHalfSpecialistUnhappiness = kResults.GetBool("HalfSpecialistUnhappiness");
	m_bHalfMoreSpecialistUnhappiness = kResults.GetBool("HalfMoreSpecialistUnhappiness");
	m_bHalfSpecialistFood = kResults.GetBool("HalfSpecialistFood");
	m_bMilitaryFoodProduction = kResults.GetBool("MilitaryFoodProduction");
	m_iMaxConscript = kResults.GetInt("MaxConscript");
	m_iWoundedUnitDamageMod = kResults.GetInt("WoundedUnitDamageMod");
	m_iUnitUpgradeCostMod = kResults.GetInt("UnitUpgradeCostMod");
	m_iBarbarianCombatBonus = kResults.GetInt("BarbarianCombatBonus");
	m_bAlwaysSeeBarbCamps = kResults.GetBool("AlwaysSeeBarbCamps");
	m_bRevealAllCapitals = kResults.GetBool("RevealAllCapitals");
	m_bGarrisonFreeMaintenance = kResults.GetBool("GarrisonFreeMaintenance");
	m_iFreeSpecialist = kResults.GetInt("FreeSpecialist");
	m_iExpModifier = kResults.GetInt("ExpModifier");
	m_iExpInBorderModifier = kResults.GetInt("ExpInBorderModifier");
	m_iMinorQuestFriendshipMod = kResults.GetInt("MinorQuestFriendshipMod");
	m_iMinorGoldFriendshipMod = kResults.GetInt("MinorGoldFriendshipMod");
	m_iMinorFriendshipMinimum = kResults.GetInt("MinorFriendshipMinimum");
	m_iMinorFriendshipDecayMod = kResults.GetInt("MinorFriendshipDecayMod");
	m_iOtherPlayersMinorFriendshipDecayMod = kResults.GetInt("OtherPlayersMinorFriendshipDecayMod");
	m_iCityStateUnitFrequencyModifier = kResults.GetInt("CityStateUnitFrequencyModifier");
	m_iCommonFoeTourismModifier = kResults.GetInt("CommonFoeTourismModifier");
	m_iLessHappyTourismModifier = kResults.GetInt("LessHappyTourismModifier");
	m_iSharedIdeologyTourismModifier = kResults.GetInt("SharedIdeologyTourismModifier");
	m_iLandTradeRouteGoldChange = kResults.GetInt("LandTradeRouteGoldChange");
	m_iSeaTradeRouteGoldChange = kResults.GetInt("SeaTradeRouteGoldChange");
	m_iInternalTradeRouteGoldChange = kResults.GetInt("InternalTradeRouteGoldChange");
	m_iSharedIdeologyTradeGoldChange = kResults.GetInt("SharedIdeologyTradeGoldChange");
#ifdef LEKMOD_POLICIES_GLOBAL_MOVE_CHANGE
	m_iGlobalMoveChange = kResults.GetInt("GlobalMoveChange");
	m_iGlobalMoveChangeFriendly = kResults.GetInt("GlobalMoveChangeFriendly");
	m_iGlobalMoveChangeEnemy = kResults.GetInt("GlobalMoveChangeEnemy");
#endif
	m_iRiggingElectionModifier = kResults.GetInt("RiggingElectionModifier");
	m_iMilitaryUnitGiftExtraInfluence = kResults.GetInt("MilitaryUnitGiftExtraInfluence");
	m_iProtectedMinorPerTurnInfluence = kResults.GetInt("ProtectedMinorPerTurnInfluence");
	m_iAfraidMinorPerTurnInfluence = kResults.GetInt("AfraidMinorPerTurnInfluence");
	m_iMinorBullyScoreModifier = kResults.GetInt("MinorBullyScoreModifier");
#if defined(LEKMOD_POLICY_MINOR_BULLY_TRIBUTE)
	m_iMinorBullyInfluenceReward = kResults.GetInt("MinorBullyInfluenceReward");
	m_bMinorBullyNoPenalty = (kResults.GetInt("MinorBullyNoPenalty") != 0);
#endif
	m_iThemingBonusMultiplier = kResults.GetInt("ThemingBonusMultiplier");
	m_iInternalTradeRouteYieldModifier = kResults.GetInt("InternalTradeRouteYieldModifier");
#ifdef FRUITY_TRADITION_LANDED_ELITE
	m_iInternalTradeRouteFoodYieldChange = kResults.GetInt("InternalTradeRouteFoodYieldChange");
#endif
#ifdef NQ_INTERNAL_TRADE_ROUTE_PRODUCTION_YIELD_CHANGE_FROM_POLICIES
	m_iInternalTradeRouteProductionYieldChange = kResults.GetInt("InternalTradeRouteProductionYieldChange");
#endif
#ifdef NQ_RAIL_CONNECTION_PRODUCTION_MODIFIER_FROM_POLICIES
	m_iRailConnectionProductionModifier = kResults.GetInt("RailConnectionProductionModifier");
#endif
#ifdef NQ_COMBAT_BONUS_VS_SMALLER_CIV_FROM_POLICIES
	m_iCombatBonusVsSmallerCiv = kResults.GetInt("CombatBonusVsSmallerCiv");
#endif
#ifdef NQ_PRODUCTION_TO_GREAT_MUSICIANS_MODIFIER_FROM_POLICIES
	m_iProductionToGreatMusiciansModifier = kResults.GetInt("ProductionToGreatMusiciansModifier");
#endif
#ifdef NQ_TRADE_MISSION_INFLUENCE_MODIFIER_FROM_POLICIES
	m_iTradeMissionInfluenceModifier = kResults.GetInt("TradeMissionInfluenceModifier");
#endif
#ifdef NQ_SCIENCE_PER_GREAT_PERSON_BORN_FROM_POLICIES
	m_iSciencePerGreatPersonBorn = kResults.GetInt("SciencePerGreatPersonBorn");
#endif
#ifdef NQ_INFLUENCE_BOOST_PER_GREAT_PERSON_BORN_FROM_POLICIES
	m_iInfluenceBoostPerGreatPersonBorn = kResults.GetInt("InfluenceBoostPerGreatPersonBorn");
#endif
#ifdef NQ_TOURISM_FROM_TRADE_MISSIONS_FROM_POLICIES
	m_iTourismFromTradeMissions = kResults.GetInt("TourismFromTradeMissions");
#endif
#ifdef NQ_HAPPINESS_FROM_GREAT_IMPROVEMENTS_FROM_POLICIES
	m_iHappinessFromGreatImprovements = kResults.GetInt("HappinessFromGreatImprovements");
#endif
#ifdef NQ_MINOR_FRIENDSHIP_GAIN_BULLY_GOLD_SUCCESS_FROM_POLICIES
	m_iMinorFriendshipGainBullyGoldSuccess = kResults.GetInt("MinorFriendshipGainBullyGoldSuccess");
#endif
#ifdef NQ_COUP_CHANCE_MODIFIER_FROM_POLICIES
	m_iCoupChanceModifier = kResults.GetInt("CoupChanceModifier");
#endif
	m_iSharedReligionTourismModifier = kResults.GetInt("SharedReligionTourismModifier");
	m_iTradeRouteTourismModifier = kResults.GetInt("TradeRouteTourismModifier");
	m_iOpenBordersTourismModifier = kResults.GetInt("OpenBordersTourismModifier");
	m_iCityStateTradeChange = kResults.GetInt("CityStateTradeChange");
	m_bMinorGreatPeopleAllies = kResults.GetBool("MinorGreatPeopleAllies");
	m_bMinorScienceAllies = kResults.GetBool("MinorScienceAllies");
	m_bMinorResourceBonus = kResults.GetBool("MinorResourceBonus");
	m_iMinorMilitaryNumExtraUnitsToGift = kResults.GetInt("MinorMilitaryNumExtraUnitsToGift");
	m_iCityStateBonusModifier = kResults.GetInt("CityStateBonusModifier");
	m_iExtraTerritoryClaim = kResults.GetInt("ExtraTerritoryClaim");
	m_iExtraTourismPerGreatWork = kResults.GetInt("ExtraTourismPerGreatWork");
#if !defined(LEK_YIELD_TOURISM) && defined(LEKMOD_EXPERIMENTAL_CHANGES)
	m_iTourismPerWonder = kResults.GetInt("TourismPerWonder");
#endif
#ifdef NQ_TOURISM_PER_CITY
	m_iTourismPerCity = kResults.GetInt("TourismPerCity");
#endif
#ifdef NQ_IDEOLOGY_PRESSURE_UNHAPPINESS_MODIFIER_FROM_POLICIES
	m_iIdeologyPressureUnhappinessModifier = kResults.GetInt("IdeologyPressureUnhappinessModifier");
#endif

	m_iProductionFromGarrison = kResults.GetInt("ProductionFromGarrison");
	m_bGoldenAgeCultureBonusDisabled = kResults.GetBool("GoldenAgeCultureBonusDisabled");
	m_bSecondReligionPantheon = kResults.GetBool("SecondReligionPantheon");
	m_bAddReformationBelief = kResults.GetBool("AddReformationBelief");
	m_bEnablesSSPartHurry = kResults.GetBool("EnablesSSPartHurry");
#ifdef NQ_DIABLE_RESISTANCE_TIME_VIA_POLICIES
	m_bDisablesResistanceTime = kResults.GetBool("DisablesResistanceTime");
#endif
#ifdef NQ_PATRIOTIC_WAR
	m_bDoubleTrainedMilitaryLandUnit = kResults.GetBool("IsDoubleTrainedMilitaryLandUnits");
#endif
#ifdef NQ_WAR_HERO
	m_bWarHero = kResults.GetBool("IsWarHero");
#endif
#ifdef NQ_IGNORE_PUPPETS_FOR_RESEARCH_COSTS_FROM_POLICIES
	m_bIgnorePuppetsForResearchCosts = kResults.GetBool("IgnorePuppetsForResearchCosts");
#endif
#ifdef NQ_POLICY_TOGGLE_NO_MINOR_DOW_IF_FRIENDS
	m_bNoMinorDOWIfFriends = kResults.GetBool("NoMinorDOWIfFriends");
#endif
#ifdef NQ_ALLOW_PUPPET_PURCHASING_FROM_POLICIES
	m_bAllowPuppetPurchasing = kResults.GetBool("AllowPuppetPurchasing");
#endif
#ifdef NQ_SPREAD_MODIFIER_OWNED_CITIES
	m_iSpreadModifierOwnedCities = kResults.GetInt("SpreadModifierOwnedCities");
	m_iSpreadModifierUnownedCities = kResults.GetInt("SpreadModifierUnownedCities");
#endif
	m_bEnablesSSPartPurchase = kResults.GetBool("EnablesSSPartPurchase");
	m_bAbleToAnnexCityStates = kResults.GetBool("AbleToAnnexCityStates");
	m_bOneShot = kResults.GetBool("OneShot");
	m_bIncludesOneShotFreeUnits = kResults.GetBool("IncludesOneShotFreeUnits");

	m_strWeLoveTheKingKey = kResults.GetText("WeLoveTheKing");
	m_wstrWeLoveTheKing = GetLocalizedText(m_strWeLoveTheKingKey);


	const char* szTechPrereq = kResults.GetText("TechPrereq");
	m_iTechPrereq = GC.getInfoTypeForString(szTechPrereq, true);

	const char* szPolicyBranchType = kResults.GetText("PolicyBranchType");
	m_iPolicyBranchType = GC.getInfoTypeForString(szPolicyBranchType, true);

	m_iNumExtraBranches = kResults.GetInt("NumExtraBranches");

	const char* szFreeBuilding = kResults.GetText("FreeBuildingOnConquest");
	if(szFreeBuilding)
	{
		m_eFreeBuildingOnConquest = (BuildingTypes)GC.getInfoTypeForString(szFreeBuilding, true);
	}


	const char* szPolicyType = GetType();
	kUtility.SetYields(m_piYieldModifier, "Policy_YieldModifiers", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piCityYieldChange, "Policy_CityYieldChanges", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piCoastalCityYieldChange, "Policy_CoastalCityYieldChanges", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piCapitalYieldChange, "Policy_CapitalYieldChanges", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piCapitalYieldPerPopChange, "Policy_CapitalYieldPerPopChanges", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piCapitalYieldModifier, "Policy_CapitalYieldModifiers", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piGreatWorkYieldChange, "Policy_GreatWorkYieldChanges", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piSpecialistExtraYield, "Policy_SpecialistExtraYields", "PolicyType", szPolicyType);

	kUtility.SetFlavors(m_piFlavorValue, "Policy_Flavors", "PolicyType", szPolicyType);

	kUtility.PopulateArrayByValue(m_paiHurryModifier, "HurryInfos", "Policy_HurryModifiers", "HurryType", "PolicyType", szPolicyType, "HurryCostModifier");

	kUtility.PopulateArrayByExistence(m_pabSpecialistValid, "Specialists", "Policy_ValidSpecialists", "SpecialistType", "PolicyType", szPolicyType);

	kUtility.PopulateArrayByExistence(m_pabFreePromotion, "UnitPromotions", "Policy_FreePromotions", "PromotionType", "PolicyType", szPolicyType);
	kUtility.PopulateArrayByValue(m_paiUnitCombatFreeExperiences, "UnitCombatInfos", "Policy_UnitCombatFreeExperiences", "UnitCombatType", "PolicyType", szPolicyType, "FreeExperience");
	kUtility.PopulateArrayByValue(m_paiUnitCombatProductionModifiers, "UnitCombatInfos", "Policy_UnitCombatProductionModifiers", "UnitCombatType", "PolicyType", szPolicyType, "ProductionModifier");

	kUtility.PopulateArrayByValue(m_paiBuildingClassCultureChanges, "BuildingClasses", "Policy_BuildingClassCultureChanges", "BuildingClassType", "PolicyType", szPolicyType, "CultureChange");
	kUtility.PopulateArrayByValue(m_paiBuildingClassProductionModifiers, "BuildingClasses", "Policy_BuildingClassProductionModifiers", "BuildingClassType", "PolicyType", szPolicyType, "ProductionModifier");

	kUtility.PopulateArrayByValue(m_paiBuildingClassHappiness, "BuildingClasses", "Policy_BuildingClassHappiness", "BuildingClassType", "PolicyType", szPolicyType, "Happiness");

	kUtility.PopulateArrayByValue(m_paiFreeUnitClasses, "UnitClasses", "Policy_FreeUnitClasses", "UnitClassType", "PolicyType", szPolicyType, "Count");
	kUtility.PopulateArrayByValue(m_paiTourismOnUnitCreation, "UnitClasses", "Policy_TourismOnUnitCreation", "UnitClassType", "PolicyType", szPolicyType, "Tourism");
#if defined(FULL_YIELD_FROM_KILLS)
	{
		kUtility.InitializeArray(m_paiYieldFromKills, "Yields", 0);
		kUtility.InitializeArray(m_paiYieldFromKillsMax, "Yields", 0);
		std::string sqlKey = "Policy_YieldFromKills";
		Database::Results* pResults = kUtility.GetOrPrepareResults(sqlKey,
			"SELECT Yields.ID, Yield, COALESCE(Max, 0) "
			"FROM Policy_YieldFromKills "
			"INNER JOIN Yields ON Yields.Type = YieldType "
			"WHERE PolicyType = ?");

		pResults->Bind(1, szPolicyType);

		while (pResults->Step())
		{
			const int iYieldID = pResults->GetInt(0);
			m_paiYieldFromKills[iYieldID] = pResults->GetInt(1);
			m_paiYieldFromKillsMax[iYieldID] = pResults->GetInt(2);
		}
		pResults->Reset();
	}
#endif
#if defined(TRADE_REFACTOR)
	{
		kUtility.Initialize2DArray(m_ppiMinorTradeRouteDomainYieldChanges, "Domains", "Yields");
		kUtility.Initialize2DArray(m_ppiTradeConnectionLandYieldChanges, "TradeConnections", "Yields");
		kUtility.Initialize2DArray(m_ppiTradeConnectionSeaYieldChanges, "TradeConnections", "Yields");
		std::string strKey("Policy_TradeRouteYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT TradeConnections.ID as TradeConnectionID, Domains.ID as DomainID, Yields.ID as YieldID, YieldTimes100, CityStateOnly  "
			"FROM Policy_TradeRouteYieldChanges "
			"INNER JOIN TradeConnections ON TradeConnections.Type = TradeConnectionType "
			"INNER JOIN Domains ON Domains.Type = DomainType "
			"INNER JOIN Yields ON Yields.Type = YieldType "
			"WHERE PolicyType = ?");
		pResults->Bind(1, szPolicyType);
		while (pResults->Step())
		{
			const int iTradeConnectionID = pResults->GetInt(0);
			const int iDomainID = pResults->GetInt(1);
			const int iYieldID = pResults->GetInt(2);
			const int iYieldTimes100 = pResults->GetInt(3);
			const bool bCityStateOnly = pResults->GetBool(4);

			if (bCityStateOnly && TRADE_CONNECTION_INTERNATIONAL == iTradeConnectionID)
				m_ppiMinorTradeRouteDomainYieldChanges[iDomainID][iYieldID] = iYieldTimes100;
			else
			{
				if (iDomainID == DOMAIN_LAND)
					m_ppiTradeConnectionLandYieldChanges[iTradeConnectionID][iYieldID] = iYieldTimes100;
				else if (iDomainID == DOMAIN_SEA)
					m_ppiTradeConnectionSeaYieldChanges[iTradeConnectionID][iYieldID] = iYieldTimes100;
			}
		}
		pResults->Reset();
	}

	{
		kUtility.Initialize2DArray(m_ppiTradeConnectionLandYieldModifiers, "TradeConnections", "Yields");
		kUtility.Initialize2DArray(m_ppiTradeConnectionSeaYieldModifiers, "TradeConnections", "Yields");
		std::string strKey("Policy_TradeRouteYieldModifiers");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT TradeConnections.ID as TradeConnectionID, Domains.ID as DomainID, Yields.ID as YieldID, YieldModifier  "
			"FROM Policy_TradeRouteYieldModifiers "
			"INNER JOIN TradeConnections ON TradeConnections.Type = TradeConnectionType "
			"INNER JOIN Domains ON Domains.Type = DomainType "
			"INNER JOIN Yields ON Yields.Type = YieldType "
			"WHERE PolicyType = ?");
		pResults->Bind(1, szPolicyType);
		while (pResults->Step())
		{
			const int iTradeConnectionID = pResults->GetInt(0);
			const int iDomainID = pResults->GetInt(1);
			const int iYieldID = pResults->GetInt(2);
			const int iYieldModifier = pResults->GetInt(3);
			if (iDomainID == DOMAIN_LAND)
				m_ppiTradeConnectionLandYieldModifiers[iTradeConnectionID][iYieldID] = iYieldModifier;
			else if (iDomainID == DOMAIN_SEA)
				m_ppiTradeConnectionSeaYieldModifiers[iTradeConnectionID][iYieldID] = iYieldModifier;
		}
		pResults->Reset();
	}
#endif
#if defined(LEKMOD_v34)
	kUtility.PopulateArrayByValue(m_piPolicyResourceQuantity, "Resources", "Policy_ResourceQuantity", "ResourceType", "PolicyType", szPolicyType, "Quantity");
	kUtility.SetYieldMatrix(m_ppiPolicyResourceClassYieldChanges, "ResourceClasses", "Policy_ResourceClassYieldChanges",
		"SELECT ResourceClasses.ID as ResourceClassID, Yields.ID as YieldID, YieldChange "
		"FROM Policy_ResourceClassYieldChanges "
		"INNER JOIN ResourceClasses ON ResourceClasses.Type = Policy_ResourceClassYieldChanges.ResourceClassType "
		"INNER JOIN Yields ON Yields.Type = Policy_ResourceClassYieldChanges.YieldType "
		"WHERE Policy_ResourceClassYieldChanges.PolicyType = ?",
		szPolicyType);
	kUtility.SetYieldMatrix(m_ppiPolicyResourceYieldChanges, "Resources", "Policy_ResourceYieldChanges",
		"SELECT Resources.ID as ResourceID, Yields.ID as YieldID, YieldChange "
		"FROM Policy_ResourceYieldChanges "
		"INNER JOIN Resources ON Resources.Type = Policy_ResourceYieldChanges.ResourceType "
		"INNER JOIN Yields ON Yields.Type = Policy_ResourceYieldChanges.YieldType "
		"WHERE Policy_ResourceYieldChanges.PolicyType = ?",
		szPolicyType);
#endif
#if defined(LEKMOD_FIX_SCHOLASTICISM)
	{
		kUtility.Initialize2DArray(m_paiMinorFriendYieldBonus, "Eras", "Yields");
		kUtility.Initialize2DArray(m_paiMinorAllyYieldBonus, "Eras", "Yields");
		
		std::string strKey("Policy_CityStateRelationshipYieldBonus");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT Eras.ID as EraID, Yields.ID as YieldID, FriendYieldBonusTimes100, AllyYieldBonusTimes100 "
			"FROM Policy_CityStateRelationshipYieldBonus "
			"INNER JOIN Eras ON Eras.Type = Policy_CityStateRelationshipYieldBonus.EraType "
			"INNER JOIN Yields ON Yields.Type = Policy_CityStateRelationshipYieldBonus.YieldType "
			"WHERE Policy_CityStateRelationshipYieldBonus.PolicyType = ?");
		pResults->Bind(1, szPolicyType);
		while (pResults->Step())
		{
			const int EraID = pResults->GetInt(0);
			const int iYieldID = pResults->GetInt(1);
			const int iFriendYieldBonus = pResults->GetInt(2);
			const int iAllyYieldBonus = pResults->GetInt(3);
			m_paiMinorFriendYieldBonus[EraID][iYieldID] = iFriendYieldBonus;
			m_paiMinorAllyYieldBonus[EraID][iYieldID] = iAllyYieldBonus;
		}
		pResults->Reset();
	}
#endif
#if defined(LEKMOD_EXPERIMENTAL_CHANGES)



	int iCultureFromWonders = kResults.GetInt("CulturePerWonder");
#if defined(LEK_YIELD_TOURISM)
	int iTourismFromWonders = kResults.GetInt("TourismPerWonder");
#endif
	{
		kUtility.InitializeArray(m_piWorldWonderYieldChanges, "Yields", 0);
		std::string key("WorldWonderYieldChanges");
		Database::Results* result = kUtility.GetOrPrepareResults(key,
			"SELECT Yields.ID as YieldID, Yield "
			"FROM WorldWonderYieldChanges "
			"INNER JOIN Yields ON Yields.Type = WorldWonderYieldChanges.YieldType "
			"WHERE PolicyType = ?");
		result->Bind(1, szPolicyType);
		while(result->Step())
		{
			const int yieldID = result->GetInt(0);
			const int yieldChange = result->GetInt(1);
			m_piWorldWonderYieldChanges[yieldID] = yieldChange;
			if (yieldID == YIELD_CULTURE)
			{
				m_piWorldWonderYieldChanges[YIELD_CULTURE] += iCultureFromWonders;
			}
#if defined(LEK_YIELD_TOURISM)
			else if (yieldID == YIELD_TOURISM)
			{
				m_piWorldWonderYieldChanges[YIELD_TOURISM] += iTourismFromWonders;
			}
#endif
		}

	}
#endif

#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	{
		kUtility.Initialize2DArray(m_ppiBuildingClassYieldModifiers.first, "BuildingClasses", "Yields");
		m_ppiBuildingClassYieldModifiers.second = kUtility.MaxRows("BuildingClasses");

		std::string strKey("Policy_BuildingClassYieldModifiers");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select BuildingClasses.ID as BuildingClassID, Yields.ID as YieldID, YieldMod from Policy_BuildingClassYieldModifiers inner join BuildingClasses on BuildingClasses.Type = BuildingClassType inner join Yields on Yields.Type = YieldType where PolicyType = ?");

		pResults->Bind(1, szPolicyType);

		while(pResults->Step())
		{
			const int BuildingClassID = pResults->GetInt(0);
			const int iYieldID = pResults->GetInt(1);
			const int iYieldMod = pResults->GetInt(2);

			m_ppiBuildingClassYieldModifiers.first[BuildingClassID][iYieldID] = iYieldMod;
		}
	}
#else
	kUtility.SetYieldMatrix(m_ppiBuildingClassYieldModifiers, "BuildingClasses", "Policy_BuildingClassYieldModifiers",
		"select BuildingClasses.ID as BuildingClassID, Yields.ID as YieldID, YieldMod from Policy_BuildingClassYieldModifiers inner join BuildingClasses on BuildingClasses.Type = BuildingClassType inner join Yields on Yields.Type = YieldType where PolicyType = ?",
		szPolicyType);
#endif


#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	{
		kUtility.Initialize2DArray(m_ppiBuildingClassYieldChanges.first, "BuildingClasses", "Yields");
		m_ppiBuildingClassYieldChanges.second = kUtility.MaxRows("BuildingClasses");

		std::string strKey("Policy_BuildingClassYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select BuildingClasses.ID as BuildingClassID, Yields.ID as YieldID, YieldChange from Policy_BuildingClassYieldChanges inner join BuildingClasses on BuildingClasses.Type = BuildingClassType inner join Yields on Yields.Type = YieldType where PolicyType = ?");

		pResults->Bind(1, szPolicyType);

		while(pResults->Step())
		{
			const int BuildingClassID = pResults->GetInt(0);
			const int iYieldID = pResults->GetInt(1);
			const int iYieldChange = pResults->GetInt(2);

			m_ppiBuildingClassYieldChanges.first[BuildingClassID][iYieldID] = iYieldChange;
		}
	}
#else
	kUtility.SetYieldMatrix(m_ppiBuildingClassYieldChanges, "BuildingClasses", "Policy_BuildingClassYieldChanges",
		"select BuildingClasses.ID as BuildingClassID, Yields.ID as YieldID, YieldChange from Policy_BuildingClassYieldChanges inner join BuildingClasses on BuildingClasses.Type = BuildingClassType inner join Yields on Yields.Type = YieldType where PolicyType = ?",
		szPolicyType);
#endif

#if defined(LEKMOD_POLICY_TERRAIN_FEATURE_YIELDS)

	{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
		kUtility.Initialize2DArray(m_ppiPolicyTerrainYieldChange.first, "Terrains", "Yields");
		m_ppiPolicyTerrainYieldChange.second = kUtility.MaxRows("Terrains");
		kUtility.Initialize2DArray(m_ppiPolicyTerrainYieldChangeUnimproved.first, "Terrains", "Yields");
		m_ppiPolicyTerrainYieldChangeUnimproved.second = kUtility.MaxRows("Terrains");
		kUtility.Initialize2DArray(m_ppiPolicyTerrainYieldChangeNoResource.first, "Terrains", "Yields");
		m_ppiPolicyTerrainYieldChangeNoResource.second = kUtility.MaxRows("Terrains");
		kUtility.Initialize2DArray(m_ppiPolicyTerrainYieldChangeUnimprovedNoResource.first, "Terrains", "Yields");
		m_ppiPolicyTerrainYieldChangeUnimprovedNoResource.second = kUtility.MaxRows("Terrains");
		kUtility.Initialize2DArray(m_ppiPolicyTerrainYieldChangeExcludingLakes.first, "Terrains", "Yields");
		m_ppiPolicyTerrainYieldChangeExcludingLakes.second = kUtility.MaxRows("Terrains");
		kUtility.Initialize2DArray(m_ppiPolicyTerrainYieldChangeUnimprovedExcludingLakes.first, "Terrains", "Yields");
		m_ppiPolicyTerrainYieldChangeUnimprovedExcludingLakes.second = kUtility.MaxRows("Terrains");
		kUtility.Initialize2DArray(m_ppiPolicyTerrainYieldChangeNoResourceExcludingLakes.first, "Terrains", "Yields");
		m_ppiPolicyTerrainYieldChangeNoResourceExcludingLakes.second = kUtility.MaxRows("Terrains");
		kUtility.Initialize2DArray(m_ppiPolicyTerrainYieldChangeUnimprovedNoResourceExcludingLakes.first, "Terrains", "Yields");
		m_ppiPolicyTerrainYieldChangeUnimprovedNoResourceExcludingLakes.second = kUtility.MaxRows("Terrains");
#else
		kUtility.Initialize2DArray(m_ppiPolicyTerrainYieldChange, "Terrains", "Yields");
		kUtility.Initialize2DArray(m_ppiPolicyTerrainYieldChangeUnimproved, "Terrains", "Yields");
		kUtility.Initialize2DArray(m_ppiPolicyTerrainYieldChangeNoResource, "Terrains", "Yields");
		kUtility.Initialize2DArray(m_ppiPolicyTerrainYieldChangeUnimprovedNoResource, "Terrains", "Yields");
		kUtility.Initialize2DArray(m_ppiPolicyTerrainYieldChangeExcludingLakes, "Terrains", "Yields");
		kUtility.Initialize2DArray(m_ppiPolicyTerrainYieldChangeUnimprovedExcludingLakes, "Terrains", "Yields");
		kUtility.Initialize2DArray(m_ppiPolicyTerrainYieldChangeNoResourceExcludingLakes, "Terrains", "Yields");
		kUtility.Initialize2DArray(m_ppiPolicyTerrainYieldChangeUnimprovedNoResourceExcludingLakes, "Terrains", "Yields");
#endif
		std::string strKeyTerrain("Policy_TerrainYieldChanges_ExcludeLakes");
		Database::Results* pResultsTerrain = kUtility.GetOrPrepareResults(strKeyTerrain,
			"select Terrains.ID as TerrainID, Yields.ID as YieldID, YieldChange, Unimproved, NoResource, ExcludeLakes from Policy_TerrainYieldChanges inner join Terrains on Terrains.Type = TerrainType inner join Yields on Yields.Type = YieldType where PolicyType = ?");
		pResultsTerrain->Bind(1, szPolicyType);
		while (pResultsTerrain->Step())
		{
			const int iTerrainID = pResultsTerrain->GetInt(0);
			const int iYieldID = pResultsTerrain->GetInt(1);
			const int iYieldChange = pResultsTerrain->GetInt(2);
			const bool bUnimproved = (pResultsTerrain->GetInt(3) != 0);
			const bool bNoResource = (pResultsTerrain->GetInt(4) != 0);
			const bool bExcludeLakes = (pResultsTerrain->GetInt(5) != 0);
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
			if (bExcludeLakes)
			{
				if (bUnimproved && bNoResource)
					m_ppiPolicyTerrainYieldChangeUnimprovedNoResourceExcludingLakes.first[iTerrainID][iYieldID] += iYieldChange;
				else if (bUnimproved)
					m_ppiPolicyTerrainYieldChangeUnimprovedExcludingLakes.first[iTerrainID][iYieldID] += iYieldChange;
				else if (bNoResource)
					m_ppiPolicyTerrainYieldChangeNoResourceExcludingLakes.first[iTerrainID][iYieldID] += iYieldChange;
				else
					m_ppiPolicyTerrainYieldChangeExcludingLakes.first[iTerrainID][iYieldID] += iYieldChange;
			}
			else
			{
				if (bUnimproved && bNoResource)
					m_ppiPolicyTerrainYieldChangeUnimprovedNoResource.first[iTerrainID][iYieldID] += iYieldChange;
				else if (bUnimproved)
					m_ppiPolicyTerrainYieldChangeUnimproved.first[iTerrainID][iYieldID] += iYieldChange;
				else if (bNoResource)
					m_ppiPolicyTerrainYieldChangeNoResource.first[iTerrainID][iYieldID] += iYieldChange;
				else
					m_ppiPolicyTerrainYieldChange.first[iTerrainID][iYieldID] += iYieldChange;
			}
#else
			if (bExcludeLakes)
			{
				if (bUnimproved && bNoResource)
					m_ppiPolicyTerrainYieldChangeUnimprovedNoResourceExcludingLakes[iTerrainID][iYieldID] += iYieldChange;
				else if (bUnimproved)
					m_ppiPolicyTerrainYieldChangeUnimprovedExcludingLakes[iTerrainID][iYieldID] += iYieldChange;
				else if (bNoResource)
					m_ppiPolicyTerrainYieldChangeNoResourceExcludingLakes[iTerrainID][iYieldID] += iYieldChange;
				else
					m_ppiPolicyTerrainYieldChangeExcludingLakes[iTerrainID][iYieldID] += iYieldChange;
			}
			else
			{
				if (bUnimproved && bNoResource)
					m_ppiPolicyTerrainYieldChangeUnimprovedNoResource[iTerrainID][iYieldID] += iYieldChange;
				else if (bUnimproved)
					m_ppiPolicyTerrainYieldChangeUnimproved[iTerrainID][iYieldID] += iYieldChange;
				else if (bNoResource)
					m_ppiPolicyTerrainYieldChangeNoResource[iTerrainID][iYieldID] += iYieldChange;
				else
					m_ppiPolicyTerrainYieldChange[iTerrainID][iYieldID] += iYieldChange;
			}
#endif
		}
	}

	{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
		kUtility.Initialize2DArray(m_ppiPolicyFeatureYieldChange.first, "Features", "Yields");
		m_ppiPolicyFeatureYieldChange.second = kUtility.MaxRows("Features");
		kUtility.Initialize2DArray(m_ppiPolicyFeatureYieldChangeUnimproved.first, "Features", "Yields");
		m_ppiPolicyFeatureYieldChangeUnimproved.second = kUtility.MaxRows("Features");
		kUtility.Initialize2DArray(m_ppiPolicyFeatureYieldChangeNoResource.first, "Features", "Yields");
		m_ppiPolicyFeatureYieldChangeNoResource.second = kUtility.MaxRows("Features");
		kUtility.Initialize2DArray(m_ppiPolicyFeatureYieldChangeUnimprovedNoResource.first, "Features", "Yields");
		m_ppiPolicyFeatureYieldChangeUnimprovedNoResource.second = kUtility.MaxRows("Features");
#else
		kUtility.Initialize2DArray(m_ppiPolicyFeatureYieldChange, "Features", "Yields");
		kUtility.Initialize2DArray(m_ppiPolicyFeatureYieldChangeUnimproved, "Features", "Yields");
		kUtility.Initialize2DArray(m_ppiPolicyFeatureYieldChangeNoResource, "Features", "Yields");
		kUtility.Initialize2DArray(m_ppiPolicyFeatureYieldChangeUnimprovedNoResource, "Features", "Yields");
#endif
		std::string strKeyFeature("Policy_FeatureYieldChanges");
		Database::Results* pResultsFeature = kUtility.GetOrPrepareResults(strKeyFeature,
			"select Features.ID as FeatureID, Yields.ID as YieldID, YieldChange, Unimproved, NoResource from Policy_FeatureYieldChanges inner join Features on Features.Type = FeatureType inner join Yields on Yields.Type = YieldType where PolicyType = ?");
		pResultsFeature->Bind(1, szPolicyType);
		while (pResultsFeature->Step())
		{
			const int iFeatureID = pResultsFeature->GetInt(0);
			const int iYieldID = pResultsFeature->GetInt(1);
			const int iYieldChange = pResultsFeature->GetInt(2);
			const bool bUnimproved = (pResultsFeature->GetInt(3) != 0);
			const bool bNoResource = (pResultsFeature->GetInt(4) != 0);
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
			if (bUnimproved && bNoResource)
				m_ppiPolicyFeatureYieldChangeUnimprovedNoResource.first[iFeatureID][iYieldID] += iYieldChange;
			else if (bUnimproved)
				m_ppiPolicyFeatureYieldChangeUnimproved.first[iFeatureID][iYieldID] += iYieldChange;
			else if (bNoResource)
				m_ppiPolicyFeatureYieldChangeNoResource.first[iFeatureID][iYieldID] += iYieldChange;
			else
				m_ppiPolicyFeatureYieldChange.first[iFeatureID][iYieldID] += iYieldChange;
#else
			if (bUnimproved && bNoResource)
				m_ppiPolicyFeatureYieldChangeUnimprovedNoResource[iFeatureID][iYieldID] += iYieldChange;
			else if (bUnimproved)
				m_ppiPolicyFeatureYieldChangeUnimproved[iFeatureID][iYieldID] += iYieldChange;
			else if (bNoResource)
				m_ppiPolicyFeatureYieldChangeNoResource[iFeatureID][iYieldID] += iYieldChange;
			else
				m_ppiPolicyFeatureYieldChange[iFeatureID][iYieldID] += iYieldChange;
#endif
		}
	}
#endif

#ifdef AUI_POLICY_BUILDING_CLASS_FLAVOR_MODIFIERS

	{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
		kUtility.Initialize2DArray(m_ppiBuildingClassFlavorChanges.first, "BuildingClasses", "Flavors");
		m_ppiBuildingClassFlavorChanges.second = kUtility.MaxRows("BuildingClasses");
#else
		kUtility.Initialize2DArray(m_ppiBuildingClassFlavorChanges, "BuildingClasses", "Flavors");
#endif

		std::string strKey("Policy_BuildingClassFlavorChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select BuildingClasses.ID as BuildingClassID, Flavors.ID as FlavorID, FlavorChange from Policy_BuildingClassFlavorChanges inner join BuildingClasses on BuildingClasses.Type = BuildingClassType inner join Flavors on Flavors.Type = FlavorType where PolicyType = ?");

		pResults->Bind(1, szPolicyType);

		while (pResults->Step())
		{
			const int BuildingClassID = pResults->GetInt(0);
			const int iFlavorID = pResults->GetInt(1);
			const int iFlavorChange = pResults->GetInt(2);

#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
			m_ppiBuildingClassFlavorChanges.first[BuildingClassID][iFlavorID] = iFlavorChange;
#else
			m_ppiBuildingClassFlavorChanges[BuildingClassID][iFlavorID] = iFlavorChange;
#endif
		}
	}
#endif


#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	{
		kUtility.Initialize2DArray(m_ppiImprovementYieldChanges.first, "Improvements", "Yields");
		m_ppiImprovementYieldChanges.second = kUtility.MaxRows("Improvements");

		std::string strKey("Policy_ImprovementYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Improvements.ID as ImprovementID, Yields.ID as YieldID, Yield from Policy_ImprovementYieldChanges inner join Improvements on Improvements.Type = ImprovementType inner join Yields on Yields.Type = YieldType where PolicyType = ?");

		pResults->Bind(1, szPolicyType);

		while(pResults->Step())
		{
			const int ImprovementID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiImprovementYieldChanges.first[ImprovementID][YieldID] = yield;
		}
	}
#else
	kUtility.SetYieldMatrix(m_ppiImprovementYieldChanges, "Improvements", "Policy_ImprovementYieldChanges",
		"select Improvements.ID as ImprovementID, Yields.ID as YieldID, Yield from Policy_ImprovementYieldChanges inner join Improvements on Improvements.Type = ImprovementType inner join Yields on Yields.Type = YieldType where PolicyType = ?",
		szPolicyType);
#endif

#if defined(LEKMOD_POLICY_GREATPERSON_IMPROVEMENT_ADJACENCY_YIELD)

	{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
		kUtility.Initialize2DArray(m_ppiPolicyGreatPersonImprovementAdjacencyYieldBonus.first, "Improvements", "Yields");
		m_ppiPolicyGreatPersonImprovementAdjacencyYieldBonus.second = kUtility.MaxRows("Improvements");
#else
		kUtility.Initialize2DArray(m_ppiPolicyGreatPersonImprovementAdjacencyYieldBonus, "Improvements", "Yields");
#endif
		std::string strKeyGPAdj("Policy_GreatPersonImprovement_Adjacency_YieldBonus");
		Database::Results* pResultsGPAdj = kUtility.GetOrPrepareResults(strKeyGPAdj,
			"select Improvements.ID as ImprovementID, Yields.ID as YieldID, Yield from Policy_GreatPersonImprovement_Adjacency_YieldBonus inner join Improvements on Improvements.Type = ImprovementType inner join Yields on Yields.Type = YieldType where PolicyType = ?");
		pResultsGPAdj->Bind(1, szPolicyType);
		while (pResultsGPAdj->Step())
		{
			const int ImprovementID = pResultsGPAdj->GetInt(0);
			const int YieldID = pResultsGPAdj->GetInt(1);
			const int iBonus = pResultsGPAdj->GetInt(2);
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
			m_ppiPolicyGreatPersonImprovementAdjacencyYieldBonus.first[ImprovementID][YieldID] += iBonus;
#else
			m_ppiPolicyGreatPersonImprovementAdjacencyYieldBonus[ImprovementID][YieldID] += iBonus;
#endif
		}
	}
#endif


	kUtility.PopulateArrayByValue(m_piImprovementCultureChange, "Improvements", "Policy_ImprovementCultureChanges", "ImprovementType", "PolicyType", szPolicyType, "CultureChange");


	{
		kUtility.InitializeArray(m_piPrereqOrPolicies, "Policies", (int)NO_POLICY);

		std::string sqlKey = "m_piPrereqOrPolicies";
		Database::Results* pResults = kUtility.GetOrPrepareResults(sqlKey,
			"select Policies.ID from Policy_PrereqORPolicies inner join Policies on Policies.Type = PrereqPolicy where PolicyType = ?");

		pResults->Bind(1, szPolicyType, false);

		int i = 0;
		while(pResults->Step())
		{
			m_piPrereqOrPolicies[i++] = pResults->GetInt(0);
		}

		pResults->Reset();
	}


	{
		kUtility.InitializeArray(m_piPrereqAndPolicies, "Policies", (int)NO_POLICY);
		std::string sqlKey = "m_piPrereqAndPolicies";

		Database::Results* pResults = kUtility.GetOrPrepareResults(sqlKey,
			"select Policies.ID from Policy_PrereqPolicies inner join Policies on Policies.Type = PrereqPolicy where PolicyType = ?");

		pResults->Bind(1, szPolicyType, false);

		int i = 0;
		while(pResults->Step())
		{
			m_piPrereqAndPolicies[i++] = pResults->GetInt(0);
		}

		pResults->Reset();
	}


	{
		kUtility.InitializeArray(m_piPolicyDisables, "Policies", (int)NO_POLICY);

		std::string sqlKey = "m_piPolicyDisables";
		Database::Results* pResults = kUtility.GetOrPrepareResults(sqlKey,
			"select Policies.ID from Policy_Disables inner join Policies on Policies.Type = PolicyDisable where PolicyType = ?");

		pResults->Bind(1, szPolicyType, false);

		int i = 0;
		while(pResults->Step())
		{
			m_piPolicyDisables[i++] = pResults->GetInt(0);
		}

		pResults->Reset();
	}


#ifdef LEKMOD_UNITCOMBAT_FREE_PROMOTION
	{
		
		kUtility.Initialize2DArray(m_FreePromotionUnitCombats, "UnitPromotions", "UnitCombatInfos");

		std::string sqlKey = "Policy_FreePromotionUnitCombats";
		Database::Results* pResults = kUtility.GetOrPrepareResults(sqlKey,
			"select UnitPromotions.ID as UnitPromotionID, UnitCombatInfos.ID as UnitCombatInfoID from Policy_FreePromotionUnitCombats inner join UnitPromotions on UnitPromotions.Type = PromotionType inner join UnitCombatInfos on UnitCombatInfos.Type = UnitCombatType where PolicyType = ?");

		pResults->Bind(1, szPolicyType);

		while (pResults->Step())
		{
			const int UnitPromotionID = pResults->GetInt(0);
			const int UnitCombatInfoID = pResults->GetInt(1);

			OutputDebugStringA(CvString::format("UnitPromotionID: %d, UnitCombatInfoID: %d\n", UnitPromotionID, UnitCombatInfoID).c_str());

			m_FreePromotionUnitCombats[UnitPromotionID][UnitCombatInfoID] = UnitCombatInfoID;
		}

		pResults->Reset();
	}
#else
	{
		m_FreePromotionUnitCombats.clear();


		std::string sqlKey = "m_FreePromotionsUnitCombats";
		Database::Results* pResults = kUtility.GetOrPrepareResults(sqlKey,
			"select UnitPromotions.ID, UnitCombatInfos.ID  from Policy_FreePromotionUnitCombats, UnitPromotions, UnitCombatInfos where PolicyType = ? and PromotionType = UnitPromotions.ID and UnitCombatType = UnitCombatInfos.ID");

		pResults->Bind(1, szPolicyType, false);

		while(pResults->Step())
		{
			const int UnitPromotionID = pResults->GetInt(0);
			const int UnitCombatInfoID = pResults->GetInt(1);


			m_FreePromotionUnitCombats.insert(std::pair<int, int>(UnitPromotionID, UnitCombatInfoID));
		}


		std::multimap<int, int>(m_FreePromotionUnitCombats).swap(m_FreePromotionUnitCombats);

		pResults->Reset();
	}
#endif
	return true;

}


int CvPolicyEntry::GetCultureCost() const
{
	return m_iCultureCost;
}


int CvPolicyEntry::GetGridX() const
{
	return m_iGridX;
}


int CvPolicyEntry::GetGridY() const
{
	return m_iGridY;
}


int CvPolicyEntry::GetLevel() const
{
	return m_iLevel;
}


int CvPolicyEntry::GetPolicyCostModifier() const
{
	return m_iPolicyCostModifier;
}


int CvPolicyEntry::GetCulturePerCity() const
{
	return m_iCulturePerCity;
}

#ifdef FRUITY_TRADITION_ARISTOCRACY

int CvPolicyEntry::GetCapitalCulturePerUniqueLuxury() const
{
	return m_iCapitalCulturePerUniqueLuxury;
}
#endif



int CvPolicyEntry::GetCulturePerWonder() const
{
	return m_iCulturePerWonder;
}


int CvPolicyEntry::GetCultureWonderMultiplier() const
{
	return m_iCultureWonderMultiplier;
}


int CvPolicyEntry::GetCulturePerTechResearched() const
{
	return m_iCulturePerTechResearched;
}


int CvPolicyEntry::GetCultureImprovementChange() const
{
	return m_iCultureImprovementChange;
}


int CvPolicyEntry::GetCultureFromKills() const
{
	return m_iCultureFromKills;
}


int CvPolicyEntry::GetCultureFromBarbarianKills() const
{
	return m_iCultureFromBarbarianKills;
}


int CvPolicyEntry::GetGoldFromKills() const
{
	return m_iGoldFromKills;
}



int CvPolicyEntry::GetScienceFromKills() const
{
	return m_iScienceFromKills;
}



int CvPolicyEntry::GetEmbarkedExtraMoves() const
{
	return m_iEmbarkedExtraMoves;
}


int CvPolicyEntry::GetAttackBonusTurns() const
{
	return m_iAttackBonusTurns;
}


int CvPolicyEntry::GetGoldenAgeTurns() const
{
	return m_iGoldenAgeTurns;
}


int CvPolicyEntry::GetGoldenAgeMeterMod() const
{
	return m_iGoldenAgeMeterMod;
}


int CvPolicyEntry::GetGoldenAgeDurationMod() const
{
	return m_iGoldenAgeDurationMod;
}


int CvPolicyEntry::GetNumFreeTechs() const
{
	return m_iNumFreeTechs;
}


int CvPolicyEntry::GetNumFreePolicies() const
{
	return m_iNumFreePolicies;
}


int CvPolicyEntry::GetNumFreeGreatPeople() const
{
	return m_iNumFreeGreatPeople;
}

#ifdef NQ_EXTRA_SPIES_FROM_POLICIES

int CvPolicyEntry::GetNumExtraSpies() const
{
	return m_iNumExtraSpies;
}
#endif
#if defined(MISC_CHANGES)

int CvPolicyEntry::GetNumExtraLeagueVotes() const
{
	return m_iNumExtraLeagueVotes;
}

int CvPolicyEntry::GetNumTradeRouteBonus() const
{
	return m_iNumTradeRouteBonus;
}
#endif

int CvPolicyEntry::GetMedianTechPercentChange() const
{
	return m_iMedianTechPercentChange;
}


int CvPolicyEntry::GetStrategicResourceMod() const
{
	return m_iStrategicResourceMod;
}


int CvPolicyEntry::GetWonderProductionModifier() const
{
	return m_iWonderProductionModifier;
}


int CvPolicyEntry::GetBuildingProductionModifier() const
{
	return m_iBuildingProductionModifier;
}


int CvPolicyEntry::GetGreatPeopleRateModifier() const
{
	return m_iGreatPeopleRateModifier;
}


int CvPolicyEntry::GetGreatGeneralRateModifier() const
{
	return m_iGreatGeneralRateModifier;
}


int CvPolicyEntry::GetGreatAdmiralRateModifier() const
{
	return m_iGreatAdmiralRateModifier;
}


int CvPolicyEntry::GetGreatWriterRateModifier() const
{
	return m_iGreatWriterRateModifier;
}


int CvPolicyEntry::GetGreatArtistRateModifier() const
{
	return m_iGreatArtistRateModifier;
}


int CvPolicyEntry::GetGreatMusicianRateModifier() const
{
	return m_iGreatMusicianRateModifier;
}


int CvPolicyEntry::GetGreatMerchantRateModifier() const
{
	return m_iGreatMerchantRateModifier;
}


int CvPolicyEntry::GetGreatScientistRateModifier() const
{
	return m_iGreatScientistRateModifier;
}


int CvPolicyEntry::GetDomesticGreatGeneralRateModifier() const
{
	return m_iDomesticGreatGeneralRateModifier;
}


int CvPolicyEntry::GetExtraHappiness() const
{
	return m_iExtraHappiness;
}


int CvPolicyEntry::GetExtraHappinessPerCity() const
{
	return m_iExtraHappinessPerCity;
}


int CvPolicyEntry::GetUnhappinessMod() const
{
	return m_iUnhappinessMod;
}


int CvPolicyEntry::GetCityCountUnhappinessMod() const
{
	return m_iCityCountUnhappinessMod;
}


int CvPolicyEntry::GetOccupiedPopulationUnhappinessMod() const
{
	return m_iOccupiedPopulationUnhappinessMod;
}


int CvPolicyEntry::GetCapitalUnhappinessMod() const
{
	return m_iCapitalUnhappinessMod;
}


int CvPolicyEntry::GetFreeExperience() const
{
	return m_iFreeExperience;
}


int CvPolicyEntry::GetWorkerSpeedModifier() const
{
	return m_iWorkerSpeedModifier;
}


int CvPolicyEntry::GetAllFeatureProduction() const
{
	return m_iAllFeatureProduction;
}


int CvPolicyEntry::GetImprovementCostModifier() const
{
	return m_iImprovementCostModifier;
}


int CvPolicyEntry::GetImprovementUpgradeRateModifier() const
{
	return m_iImprovementUpgradeRateModifier;
}


int CvPolicyEntry::GetSpecialistProductionModifier() const
{
	return m_iSpecialistProductionModifier;
}


int CvPolicyEntry::GetSpecialistUpgradeModifier() const
{
	return m_iSpecialistUpgradeModifier;
}


int CvPolicyEntry::GetMilitaryProductionModifier() const
{
	return m_iMilitaryProductionModifier;
}


int CvPolicyEntry::GetBaseFreeUnits() const
{
	return m_iBaseFreeUnits;
}


int CvPolicyEntry::GetBaseFreeMilitaryUnits() const
{
	return m_iBaseFreeMilitaryUnits;
}


int CvPolicyEntry::GetFreeUnitsPopulationPercent() const
{
	return m_iFreeUnitsPopulationPercent;
}


int CvPolicyEntry::GetFreeMilitaryUnitsPopulationPercent() const
{
	return m_iFreeMilitaryUnitsPopulationPercent;
}


int CvPolicyEntry::GetHappinessPerGarrisonedUnit() const
{
	return m_iHappinessPerGarrisonedUnit;
}


int CvPolicyEntry::GetCulturePerGarrisonedUnit() const
{
	return m_iCulturePerGarrisonedUnit;
}


int CvPolicyEntry::GetHappinessPerTradeRoute() const
{
	return m_iHappinessPerTradeRoute;
}

#ifdef NQ_RAIL_CONNECTION_HAPPINESS_FROM_POLICIES

int CvPolicyEntry::GetHappinessPerRailConnection() const
{
	return m_iHappinessPerRailConnection;
}
#endif


int CvPolicyEntry::GetHappinessPerXPopulation() const
{
	return m_iHappinessPerXPopulation;
}


int CvPolicyEntry::GetExtraHappinessPerLuxury() const
{
	return m_iExtraHappinessPerLuxury;
}


int CvPolicyEntry::GetUnhappinessFromUnitsMod() const
{
	return m_iUnhappinessFromUnitsMod;
}


int CvPolicyEntry::GetNumExtraBuilders() const
{
	return m_iNumExtraBuilders;
}


int CvPolicyEntry::GetPlotGoldCostMod() const
{
	return m_iPlotGoldCostMod;
}


int CvPolicyEntry::GetPlotCultureCostModifier() const
{
	return m_iPlotCultureCostModifier;
}


int CvPolicyEntry::GetPlotCultureExponentModifier() const
{
	return m_iPlotCultureExponentModifier;
}


int CvPolicyEntry::GetNumCitiesPolicyCostDiscount() const
{
	return m_iNumCitiesPolicyCostDiscount;
}



int CvPolicyEntry::GetNumCitiesResearchCostDiscount() const
{
	return m_iNumCitiesResearchCostDiscount;
}



int CvPolicyEntry::GetGarrisonedCityRangeStrikeModifier() const
{
	return m_iGarrisonedCityRangeStrikeModifier;
}


int CvPolicyEntry::GetUnitPurchaseCostModifier() const
{
	return m_iUnitPurchaseCostModifier;
}


int CvPolicyEntry::GetBuildingPurchaseCostModifier() const
{
	return m_iBuildingPurchaseCostModifier;
}


int CvPolicyEntry::GetCityConnectionTradeRouteGoldModifier() const
{
	return m_iCityConnectionTradeRouteGoldModifier;
}


int CvPolicyEntry::GetTradeMissionGoldModifier() const
{
	return m_iTradeMissionGoldModifier;
}


int CvPolicyEntry::GetFaithCostModifier() const
{
	return m_iFaithCostModifier;
}


int CvPolicyEntry::GetCulturalPlunderMultiplier() const
{
	return m_iCulturalPlunderMultiplier;
}


int CvPolicyEntry::GetStealTechSlowerModifier() const
{
	return m_iStealTechSlowerModifier;
}

int CvPolicyEntry::GetStealTechFasterModifier() const
{
	return m_iStealTechFasterModifier;
}


int CvPolicyEntry::GetCatchSpiesModifier() const
{
	return m_iCatchSpiesModifier;
}


int CvPolicyEntry::GetGoldPerUnit() const
{
	return m_iGoldPerUnit;
}


int CvPolicyEntry::GetGoldPerMilitaryUnit() const
{
	return m_iGoldPerMilitaryUnit;
}


int CvPolicyEntry::GetCityStrengthMod() const
{
	return m_iCityStrengthMod;
}


int CvPolicyEntry::GetCityGrowthMod() const
{
	return m_iCityGrowthMod;
}


int CvPolicyEntry::GetCapitalGrowthMod() const
{
	return m_iCapitalGrowthMod;
}


int CvPolicyEntry::GetSettlerProductionModifier() const
{
	return m_iSettlerProductionModifier;
}


int CvPolicyEntry::GetCapitalSettlerProductionModifier() const
{
	return m_iCapitalSettlerProductionModifier;
}


int CvPolicyEntry::GetNewCityExtraPopulation() const
{
	return m_iNewCityExtraPopulation;
}


int CvPolicyEntry::GetFreeFoodBox() const
{
	return m_iFreeFoodBox;
}


int CvPolicyEntry::GetRouteGoldMaintenanceMod() const
{
	return m_iRouteGoldMaintenanceMod;
}


int CvPolicyEntry::GetBuildingGoldMaintenanceMod() const
{
	return m_iBuildingGoldMaintenanceMod;
}


int CvPolicyEntry::GetUnitGoldMaintenanceMod() const
{
	return m_iUnitGoldMaintenanceMod;
}


int CvPolicyEntry::GetUnitSupplyMod() const
{
	return m_iUnitSupplyMod;
}


int CvPolicyEntry::GetHappyPerMilitaryUnit() const
{
	return m_iHappyPerMilitaryUnit;
}


int CvPolicyEntry::GetFreeSpecialist() const
{
	return m_iFreeSpecialist;
}


int CvPolicyEntry::GetTechPrereq() const
{
	return m_iTechPrereq;
}


int CvPolicyEntry::GetMaxConscript() const
{
	return m_iMaxConscript;
}


int CvPolicyEntry::GetExpModifier() const
{
	return m_iExpModifier;
}


int CvPolicyEntry::GetExpInBorderModifier() const
{
	return m_iExpInBorderModifier;
}


int CvPolicyEntry::GetMinorQuestFriendshipMod() const
{
	return m_iMinorQuestFriendshipMod;
}


int CvPolicyEntry::GetMinorGoldFriendshipMod() const
{
	return m_iMinorGoldFriendshipMod;
}


int CvPolicyEntry::GetMinorFriendshipMinimum() const
{
	return m_iMinorFriendshipMinimum;
}


int CvPolicyEntry::GetMinorFriendshipDecayMod() const
{
	return m_iMinorFriendshipDecayMod;
}


int CvPolicyEntry::GetOtherPlayersMinorFriendshipDecayMod() const
{
	return m_iOtherPlayersMinorFriendshipDecayMod;
}


int CvPolicyEntry::GetCityStateUnitFrequencyModifier() const
{
	return m_iCityStateUnitFrequencyModifier;
}


int CvPolicyEntry::GetCommonFoeTourismModifier() const
{
	return m_iCommonFoeTourismModifier;
}


int CvPolicyEntry::GetLessHappyTourismModifier() const
{
	return m_iLessHappyTourismModifier;
}


int CvPolicyEntry::GetSharedIdeologyTourismModifier() const
{
	return m_iSharedIdeologyTourismModifier;
}


int CvPolicyEntry::GetLandTradeRouteGoldChange() const
{
	return m_iLandTradeRouteGoldChange;
}


int CvPolicyEntry::GetSeaTradeRouteGoldChange() const
{
	return m_iSeaTradeRouteGoldChange;
}



int CvPolicyEntry::GetInternalTradeRouteGoldChange() const
{
	return m_iInternalTradeRouteGoldChange;
}


int CvPolicyEntry::GetSharedIdeologyTradeGoldChange() const
{
	return m_iSharedIdeologyTradeGoldChange;
}


int CvPolicyEntry::GetRiggingElectionModifier() const
{
	return m_iRiggingElectionModifier;
}


int CvPolicyEntry::GetMilitaryUnitGiftExtraInfluence() const
{
	return m_iMilitaryUnitGiftExtraInfluence;
}


int CvPolicyEntry::GetProtectedMinorPerTurnInfluence() const
{
	return m_iProtectedMinorPerTurnInfluence;
}


int CvPolicyEntry::GetAfraidMinorPerTurnInfluence() const
{
	return m_iAfraidMinorPerTurnInfluence;
}


int CvPolicyEntry::GetMinorBullyScoreModifier() const
{
	return m_iMinorBullyScoreModifier;
}

#if defined(LEKMOD_POLICY_MINOR_BULLY_TRIBUTE)
int CvPolicyEntry::GetMinorBullyInfluenceReward() const
{
	return m_iMinorBullyInfluenceReward;
}

bool CvPolicyEntry::IsMinorBullyNoPenalty() const
{
	return m_bMinorBullyNoPenalty;
}
#endif


int CvPolicyEntry::GetThemingBonusMultiplier() const
{
	return m_iThemingBonusMultiplier;
}


int CvPolicyEntry::GetInternalTradeRouteYieldModifier() const
{
	return m_iInternalTradeRouteYieldModifier;
}

#ifdef FRUITY_TRADITION_LANDED_ELITE

int CvPolicyEntry::GetInternalTradeRouteFoodYieldChange() const
{
	return m_iInternalTradeRouteFoodYieldChange;
}
#endif

#ifdef NQ_INTERNAL_TRADE_ROUTE_PRODUCTION_YIELD_CHANGE_FROM_POLICIES

int CvPolicyEntry::GetInternalTradeRouteProductionYieldChange() const
{
	return m_iInternalTradeRouteProductionYieldChange;
}
#endif

#ifdef NQ_RAIL_CONNECTION_PRODUCTION_MODIFIER_FROM_POLICIES

int CvPolicyEntry::GetRailConnectionProductionModifier() const
{
	return m_iRailConnectionProductionModifier;
}
#endif

#ifdef NQ_COMBAT_BONUS_VS_SMALLER_CIV_FROM_POLICIES

int CvPolicyEntry::GetCombatBonusVsSmallerCiv() const
{
	return m_iCombatBonusVsSmallerCiv;
}
#endif

#ifdef NQ_PRODUCTION_TO_GREAT_MUSICIANS_MODIFIER_FROM_POLICIES

int CvPolicyEntry::GetProductionToGreatMusiciansModifier() const
{
	return m_iProductionToGreatMusiciansModifier;
}
#endif


#ifdef NQ_TRADE_MISSION_INFLUENCE_MODIFIER_FROM_POLICIES

int CvPolicyEntry::GetTradeMissionInfluenceModifier() const
{
	return m_iTradeMissionInfluenceModifier;
}
#endif

#ifdef NQ_SCIENCE_PER_GREAT_PERSON_BORN_FROM_POLICIES

int CvPolicyEntry::GetSciencePerGreatPersonBorn() const
{
	return m_iSciencePerGreatPersonBorn;
}
#endif

#ifdef NQ_INFLUENCE_BOOST_PER_GREAT_PERSON_BORN_FROM_POLICIES

int CvPolicyEntry::GetInfluenceBoostPerGreatPersonBorn() const
{
	return m_iInfluenceBoostPerGreatPersonBorn;
}
#endif

#ifdef NQ_TOURISM_FROM_TRADE_MISSIONS_FROM_POLICIES

int CvPolicyEntry::GetTourismFromTradeMissions() const
{
	return m_iTourismFromTradeMissions;
}
#endif

#ifdef NQ_HAPPINESS_FROM_GREAT_IMPROVEMENTS_FROM_POLICIES

int CvPolicyEntry::GetHappinessFromGreatImprovements() const
{
	return m_iHappinessFromGreatImprovements;
}
#endif

#ifdef NQ_MINOR_FRIENDSHIP_GAIN_BULLY_GOLD_SUCCESS_FROM_POLICIES

int CvPolicyEntry::GetMinorFriendshipGainBullyGoldSuccess() const
{
	return m_iMinorFriendshipGainBullyGoldSuccess;
}
#endif

#ifdef NQ_COUP_CHANCE_MODIFIER_FROM_POLICIES

int CvPolicyEntry::GetCoupChanceModifier() const
{
	return m_iCoupChanceModifier;
}
#endif


int CvPolicyEntry::GetSharedReligionTourismModifier() const
{
	return m_iSharedReligionTourismModifier;
}


int CvPolicyEntry::GetTradeRouteTourismModifier() const
{
	return m_iTradeRouteTourismModifier;
}


int CvPolicyEntry::GetOpenBordersTourismModifier() const
{
	return m_iOpenBordersTourismModifier;
}


int CvPolicyEntry::GetCityStateTradeChange() const
{
	return m_iCityStateTradeChange;
}


bool CvPolicyEntry::IsMinorGreatPeopleAllies() const
{
	return m_bMinorGreatPeopleAllies;
}


bool CvPolicyEntry::IsMinorScienceAllies() const
{
	return m_bMinorScienceAllies;
}


bool CvPolicyEntry::IsMinorResourceBonus() const
{
	return m_bMinorResourceBonus;
}



int CvPolicyEntry::GetMinorMilitaryNumExtraUnitsToGift() const
{
	return m_iMinorMilitaryNumExtraUnitsToGift;
}


int CvPolicyEntry::GetCityStateBonusModifier() const
{
	return m_iCityStateBonusModifier;
}




int CvPolicyEntry::GetExtraTerritoryClaim() const
{
	return m_iExtraTerritoryClaim;
}



int CvPolicyEntry::GetExtraTourismPerGreatWork() const
{
	return m_iExtraTourismPerGreatWork;
}



int CvPolicyEntry::GetTourismPerWonder() const
{
	return m_iTourismPerWonder;
}

#ifdef NQ_TOURISM_PER_CITY

int CvPolicyEntry::GetTourismPerCity() const
{
	return m_iTourismPerCity;
}
#endif

#ifdef NQ_IDEOLOGY_PRESSURE_UNHAPPINESS_MODIFIER_FROM_POLICIES

int CvPolicyEntry::GetIdeologyPressureUnhappinessModifier() const
{
	return m_iIdeologyPressureUnhappinessModifier;
}
#endif



int CvPolicyEntry::GetProductionFromGarrison() const
{
	return m_iProductionFromGarrison;
}


int CvPolicyEntry::GetPolicyBranchType() const
{
	return m_iPolicyBranchType;
}


int CvPolicyEntry::GetNumExtraBranches() const
{
	return m_iNumExtraBranches;
}


int CvPolicyEntry::GetHappinessToCulture() const
{
	return m_iHappinessToCulture;
}

#ifdef NQ_SPREAD_MODIFIER_OWNED_CITIES

int CvPolicyEntry::GetReligionSpreadModifierFriendly() const
{
	return m_iSpreadModifierOwnedCities;
}

int CvPolicyEntry::GetReligionSpreadModifierForeign() const
{
	return m_iSpreadModifierUnownedCities;
}
#endif;


int CvPolicyEntry::GetHappinessToScience() const
{
	return m_iHappinessToScience;
}

#ifdef NQ_GOLD_TO_SCIENCE_FROM_POLICIES

int CvPolicyEntry::GetGoldToScience() const
{
	return m_iGoldToScience;
}
#endif



int CvPolicyEntry::GetNumCitiesFreeAestheticsSchools() const
{
	return m_iNumCitiesFreeAestheticsSchools;
}


int CvPolicyEntry::GetNumCitiesFreePietyGardens() const
{
	return m_iNumCitiesFreePietyGardens;
}



int CvPolicyEntry::GetNumCitiesFreeWalls() const
{
	return m_iNumCitiesFreeWalls;
}


int CvPolicyEntry::GetNumCitiesFreeCultureBuilding() const
{
	return m_iNumCitiesFreeCultureBuilding;
}


int CvPolicyEntry::GetNumCitiesFreeFoodBuilding() const
{
	return m_iNumCitiesFreeFoodBuilding;
}


bool CvPolicyEntry::IsHalfSpecialistUnhappiness() const
{
	return m_bHalfSpecialistUnhappiness;
}

bool CvPolicyEntry::IsHalfMoreSpecialistUnhappiness() const
{
	return m_bHalfMoreSpecialistUnhappiness;
}

bool CvPolicyEntry::IsHalfSpecialistFood() const
{
	return m_bHalfSpecialistFood;
}


bool CvPolicyEntry::IsMilitaryFoodProduction() const
{
	return m_bMilitaryFoodProduction;
}


int CvPolicyEntry::GetWoundedUnitDamageMod() const
{
	return m_iWoundedUnitDamageMod;
}


int CvPolicyEntry::GetUnitUpgradeCostMod() const
{
	return m_iUnitUpgradeCostMod;
}


int CvPolicyEntry::GetBarbarianCombatBonus() const
{
	return m_iBarbarianCombatBonus;
}


bool CvPolicyEntry::IsAlwaysSeeBarbCamps() const
{
	return m_bAlwaysSeeBarbCamps;
}


bool CvPolicyEntry::IsRevealAllCapitals() const
{
	return m_bRevealAllCapitals;
}


bool CvPolicyEntry::IsGarrisonFreeMaintenance() const
{
	return m_bGarrisonFreeMaintenance;
}

bool CvPolicyEntry::IsGoldenAgeCultureBonusDisabled() const
{
	return m_bGoldenAgeCultureBonusDisabled;
}

bool CvPolicyEntry::IsSecondReligionPantheon() const
{
	return m_bSecondReligionPantheon;
}

bool CvPolicyEntry::IsAddReformationBelief() const
{
	return m_bAddReformationBelief;
}

bool CvPolicyEntry::IsEnablesSSPartHurry() const
{
	return m_bEnablesSSPartHurry;
}

#ifdef NQ_DIABLE_RESISTANCE_TIME_VIA_POLICIES
bool CvPolicyEntry::IsDisablesResistanceTime() const
{
	return m_bDisablesResistanceTime;
}
#endif

#ifdef NQ_PATRIOTIC_WAR
bool CvPolicyEntry::IsDoubleTrainedMilitaryLandUnit() const
{
	return m_bDoubleTrainedMilitaryLandUnit;
}
#endif

#ifdef NQ_WAR_HERO
bool CvPolicyEntry::IsWarHero() const
{
	return m_bWarHero;
}
#endif

#ifdef NQ_IGNORE_PUPPETS_FOR_RESEARCH_COSTS_FROM_POLICIES
bool CvPolicyEntry::IsIgnorePuppetsForResearchCosts() const
{
	return m_bIgnorePuppetsForResearchCosts;
}
#endif

#ifdef NQ_POLICY_TOGGLE_NO_MINOR_DOW_IF_FRIENDS
bool CvPolicyEntry::IsNoMinorDOWIfFriends() const
{
	return m_bNoMinorDOWIfFriends;
}
#endif

#ifdef NQ_ALLOW_PUPPET_PURCHASING_FROM_POLICIES
bool CvPolicyEntry::IsAllowPuppetPurchasing() const
{
	return m_bAllowPuppetPurchasing;
}
#endif

bool CvPolicyEntry::IsEnablesSSPartPurchase() const
{
	return m_bEnablesSSPartPurchase;
}


bool CvPolicyEntry::IsAbleToAnnexCityStates() const
{
	return m_bAbleToAnnexCityStates;
}


bool CvPolicyEntry::IsOneShot() const
{
	return m_bOneShot;
}


bool CvPolicyEntry::IncludesOneShotFreeUnits() const
{
	return m_bIncludesOneShotFreeUnits;
}


const char* CvPolicyEntry::GetWeLoveTheKing()
{
	return m_wstrWeLoveTheKing.c_str();
}


void CvPolicyEntry::SetWeLoveTheKingKey(const char* szVal)
{
	m_strWeLoveTheKingKey = szVal;
}




int CvPolicyEntry::GetPrereqOrPolicies(int i) const
{
	return m_piPrereqOrPolicies ? m_piPrereqOrPolicies[i] : -1;
}


int CvPolicyEntry::GetPrereqAndPolicies(int i) const
{
	return m_piPrereqAndPolicies ? m_piPrereqAndPolicies[i] : -1;
}


int CvPolicyEntry::GetPolicyDisables(int i) const
{
	return m_piPolicyDisables ? m_piPolicyDisables[i] : -1;
}


int CvPolicyEntry::GetYieldModifier(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldModifier ? m_piYieldModifier[i] : -1;
}


int* CvPolicyEntry::GetYieldModifierArray() const
{
	return m_piYieldModifier;
}


int CvPolicyEntry::GetCityYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piCityYieldChange ? m_piCityYieldChange[i] : -1;
}


int* CvPolicyEntry::GetCityYieldChangeArray() const
{
	return m_piCityYieldChange;
}


int CvPolicyEntry::GetCoastalCityYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piCoastalCityYieldChange ? m_piCoastalCityYieldChange[i] : -1;
}


int* CvPolicyEntry::GetCoastalCityYieldChangeArray() const
{
	return m_piCoastalCityYieldChange;
}


int CvPolicyEntry::GetCapitalYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piCapitalYieldChange ? m_piCapitalYieldChange[i] : -1;
}


int* CvPolicyEntry::GetCapitalYieldChangeArray() const
{
	return m_piCapitalYieldChange;
}


int CvPolicyEntry::GetCapitalYieldPerPopChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piCapitalYieldPerPopChange ? m_piCapitalYieldPerPopChange[i] : -1;
}


int* CvPolicyEntry::GetCapitalYieldPerPopChangeArray() const
{
	return m_piCapitalYieldPerPopChange;
}


int CvPolicyEntry::GetCapitalYieldModifier(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piCapitalYieldModifier ? m_piCapitalYieldModifier[i] : -1;
}


int* CvPolicyEntry::GetCapitalYieldModifierArray() const
{
	return m_piCapitalYieldModifier;
}


int CvPolicyEntry::GetGreatWorkYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piGreatWorkYieldChange ? m_piGreatWorkYieldChange[i] : -1;
}


int* CvPolicyEntry::GetGreatWorkYieldChangeArray() const
{
	return m_piGreatWorkYieldChange;
}


int CvPolicyEntry::GetSpecialistExtraYield(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piSpecialistExtraYield ? m_piSpecialistExtraYield[i] : -1;
}


int* CvPolicyEntry::GetSpecialistExtraYieldArray() const
{
	return m_piSpecialistExtraYield;
}


int CvPolicyEntry::GetUnitCombatProductionModifiers(int i) const
{
	CvAssertMsg(i < GC.getNumUnitCombatClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiUnitCombatProductionModifiers ? m_paiUnitCombatProductionModifiers[i] : -1;
}

int CvPolicyEntry::IsFreePromotion(int i) const
{
	return m_pabFreePromotion ? m_pabFreePromotion[i] : -1;
}


bool CvPolicyEntry::IsFreePromotionUnitCombat(const int promotionID, const int unitCombatID) const
{
#ifdef LEKMOD_UNITCOMBAT_FREE_PROMOTION

	return m_FreePromotionUnitCombats ? m_FreePromotionUnitCombats[promotionID][unitCombatID] : -1;

#else
	std::multimap<int, int>::const_iterator it = m_FreePromotionUnitCombats.find(promotionID);
	if(it != m_FreePromotionUnitCombats.end())
	{

		std::multimap<int, int>::const_iterator lastElement = m_FreePromotionUnitCombats.upper_bound(promotionID);


		for(; it != lastElement; ++it)
		{
			if(it->second == unitCombatID)
			{
				return true;
			}
		}
	}
	return false;
#endif

	
}


int CvPolicyEntry::GetUnitCombatFreeExperiences(int i) const
{
	CvAssertMsg(i < GC.getNumUnitCombatClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiUnitCombatFreeExperiences ? m_paiUnitCombatFreeExperiences[i] : -1;
}


int CvPolicyEntry::GetBuildingClassCultureChange(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiBuildingClassCultureChanges ? m_paiBuildingClassCultureChanges[i] : -1;
}


int CvPolicyEntry::GetBuildingClassHappiness(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiBuildingClassHappiness ? m_paiBuildingClassHappiness[i] : -1;
}


int CvPolicyEntry::GetNumFreeUnitsByClass(int i) const
{
	CvAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiFreeUnitClasses ? m_paiFreeUnitClasses[i] : -1;
}


int CvPolicyEntry::GetTourismByUnitClassCreated(int i) const
{
	CvAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiTourismOnUnitCreation ? m_paiTourismOnUnitCreation[i] : -1;
}
#if defined(TRADE_REFACTOR)

int CvPolicyEntry::GetMinorTradeRouteDomainYieldChanges(int i, int j) const
{ 
	CvAssertMsg(i < GC.getNumDomainInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiMinorTradeRouteDomainYieldChanges ? m_ppiMinorTradeRouteDomainYieldChanges[i][j] : 0;
}

int CvPolicyEntry::GetTradeConnectionLandYieldChanges(int i, int j) const
{
	CvAssertMsg(i < NUM_TRADE_CONNECTION_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiTradeConnectionLandYieldChanges ? m_ppiTradeConnectionLandYieldChanges[i][j] : 0;
}

int CvPolicyEntry::GetTradeConnectionSeaYieldChanges(int i, int j) const
{
	CvAssertMsg(i < NUM_TRADE_CONNECTION_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiTradeConnectionSeaYieldChanges ? m_ppiTradeConnectionSeaYieldChanges[i][j] : 0;
}

int CvPolicyEntry::GetTradeConnectionLandYieldModifier(int i, int j) const
{
	CvAssertMsg(i < NUM_TRADE_CONNECTION_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiTradeConnectionLandYieldModifiers ? m_ppiTradeConnectionLandYieldModifiers[i][j] : 0;
}

int CvPolicyEntry::GetTradeConnectionSeaYieldModifier(int i, int j) const
{
	CvAssertMsg(i < NUM_TRADE_CONNECTION_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiTradeConnectionSeaYieldModifiers ? m_ppiTradeConnectionSeaYieldModifiers[i][j] : 0;
}
#endif

#if defined(FULL_YIELD_FROM_KILLS)

int CvPolicyEntry::GetYieldFromKills(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiYieldFromKills ? m_paiYieldFromKills[i] : -1;
}

int CvPolicyEntry::GetYieldFromKillsMax(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiYieldFromKillsMax ? m_paiYieldFromKillsMax[i] : 0;
}
#endif
#if defined(LEKMOD_v34)

int CvPolicyEntry::GetPolicyResourceQuantity(int i) const
{
	CvAssertMsg(i >= 0 && i < GC.getNumResourceInfos(), "Index out of bounds");
	return m_piPolicyResourceQuantity ? m_piPolicyResourceQuantity[i] : 0;
}

int CvPolicyEntry::GetPolicyResourceClassYieldChanges(int i, int j) const
{
	CvAssertMsg(i >= 0 && i < GC.getNumResourceClassInfos(), "Index out of bounds");
	CvAssertMsg(j >= 0 && j < NUM_YIELD_TYPES, "Index out of bounds");
	return m_ppiPolicyResourceClassYieldChanges ? m_ppiPolicyResourceClassYieldChanges[i][j] : 0;
}

int CvPolicyEntry::GetPolicyResourceYieldChanges(int i, int j) const
{
	CvAssertMsg(i >= 0 && i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(j >= 0 && j < NUM_YIELD_TYPES, "Index out of bounds");
	return m_ppiPolicyResourceYieldChanges ? m_ppiPolicyResourceYieldChanges[i][j] : 0;
}
#endif
#if defined(LEKMOD_FIX_SCHOLASTICISM)

int CvPolicyEntry::GetMinorFriendYieldBonus(int i, int j) const
{
	CvAssertMsg(i < GC.getNumEraInfos(), "Era index out of bounds");
	CvAssertMsg(i > -1, "Era index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Yield index out of bounds");
	CvAssertMsg(j > -1, "Yield index out of bounds");
	return m_paiMinorFriendYieldBonus ? m_paiMinorFriendYieldBonus[i][j] : 0;
}

int CvPolicyEntry::GetMinorAllyYieldBonus(int i, int j) const
{
	CvAssertMsg(i < GC.getNumEraInfos(), "Era index out of bounds");
	CvAssertMsg(i > -1, "Era index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Yield index out of bounds");
	CvAssertMsg(j > -1, "Yield index out of bounds");
	return m_paiMinorAllyYieldBonus ? m_paiMinorAllyYieldBonus[i][j] : 0;
}
#endif
#if defined(LEKMOD_EXPERIMENTAL_CHANGES)

int CvPolicyEntry::GetWorldWonderYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piWorldWonderYieldChanges ? m_piWorldWonderYieldChanges[i] : 0;
}
#endif









int CvPolicyEntry::GetHurryModifier(int i) const
{
	CvAssertMsg(i < GC.getNumHurryInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiHurryModifier ? m_paiHurryModifier[i] : -1;
}


bool CvPolicyEntry::IsSpecialistValid(int i) const
{
	CvAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pabSpecialistValid ? m_pabSpecialistValid[i] : false;
}


int CvPolicyEntry::GetImprovementYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppiImprovementYieldChanges.first ? m_ppiImprovementYieldChanges.first[i][j] : 0;
#else
	return m_ppiImprovementYieldChanges[i][j];
#endif
}

#if defined(LEKMOD_POLICY_GREATPERSON_IMPROVEMENT_ADJACENCY_YIELD)
int CvPolicyEntry::GetGreatPersonImprovementAdjacencyYieldBonus(int i, int j) const
{
	CvAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppiPolicyGreatPersonImprovementAdjacencyYieldBonus.first ? m_ppiPolicyGreatPersonImprovementAdjacencyYieldBonus.first[i][j] : 0;
#else
	return m_ppiPolicyGreatPersonImprovementAdjacencyYieldBonus ? m_ppiPolicyGreatPersonImprovementAdjacencyYieldBonus[i][j] : 0;
#endif
}
#endif


int CvPolicyEntry::GetBuildingClassYieldModifiers(int i, int j) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppiBuildingClassYieldModifiers.first ? m_ppiBuildingClassYieldModifiers.first[i][j] : 0;
#else
	return m_ppiBuildingClassYieldModifiers[i][j];
#endif
}


int CvPolicyEntry::GetBuildingClassYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppiBuildingClassYieldChanges.first ? m_ppiBuildingClassYieldChanges.first[i][j] : 0;
#else
	return m_ppiBuildingClassYieldChanges[i][j];
#endif
}

#if defined(LEKMOD_POLICY_TERRAIN_FEATURE_YIELDS)
int CvPolicyEntry::GetPolicyTerrainYieldChange(TerrainTypes eTerrain, YieldTypes eYield, bool bUnimprovedPlot, bool bNoResourcePlot, bool bLakePlot) const
{
	if (eTerrain == NO_TERRAIN || eYield < 0 || eYield >= NUM_YIELD_TYPES)
		return 0;
	CvAssertMsg(eTerrain < GC.getNumTerrainInfos(), "Index out of bounds");
	CvAssertMsg(eTerrain > -1, "Index out of bounds");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(eYield > -1, "Index out of bounds");
	int iSum = 0;
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	if (m_ppiPolicyTerrainYieldChange.first)
		iSum += m_ppiPolicyTerrainYieldChange.first[eTerrain][eYield];
	if (bUnimprovedPlot && m_ppiPolicyTerrainYieldChangeUnimproved.first)
		iSum += m_ppiPolicyTerrainYieldChangeUnimproved.first[eTerrain][eYield];
	if (bNoResourcePlot && m_ppiPolicyTerrainYieldChangeNoResource.first)
		iSum += m_ppiPolicyTerrainYieldChangeNoResource.first[eTerrain][eYield];
	if (bUnimprovedPlot && bNoResourcePlot && m_ppiPolicyTerrainYieldChangeUnimprovedNoResource.first)
		iSum += m_ppiPolicyTerrainYieldChangeUnimprovedNoResource.first[eTerrain][eYield];
	if (!bLakePlot)
	{
		if (m_ppiPolicyTerrainYieldChangeExcludingLakes.first)
			iSum += m_ppiPolicyTerrainYieldChangeExcludingLakes.first[eTerrain][eYield];
		if (bUnimprovedPlot && m_ppiPolicyTerrainYieldChangeUnimprovedExcludingLakes.first)
			iSum += m_ppiPolicyTerrainYieldChangeUnimprovedExcludingLakes.first[eTerrain][eYield];
		if (bNoResourcePlot && m_ppiPolicyTerrainYieldChangeNoResourceExcludingLakes.first)
			iSum += m_ppiPolicyTerrainYieldChangeNoResourceExcludingLakes.first[eTerrain][eYield];
		if (bUnimprovedPlot && bNoResourcePlot && m_ppiPolicyTerrainYieldChangeUnimprovedNoResourceExcludingLakes.first)
			iSum += m_ppiPolicyTerrainYieldChangeUnimprovedNoResourceExcludingLakes.first[eTerrain][eYield];
	}
#else
	if (m_ppiPolicyTerrainYieldChange)
		iSum += m_ppiPolicyTerrainYieldChange[eTerrain][eYield];
	if (bUnimprovedPlot && m_ppiPolicyTerrainYieldChangeUnimproved)
		iSum += m_ppiPolicyTerrainYieldChangeUnimproved[eTerrain][eYield];
	if (bNoResourcePlot && m_ppiPolicyTerrainYieldChangeNoResource)
		iSum += m_ppiPolicyTerrainYieldChangeNoResource[eTerrain][eYield];
	if (bUnimprovedPlot && bNoResourcePlot && m_ppiPolicyTerrainYieldChangeUnimprovedNoResource)
		iSum += m_ppiPolicyTerrainYieldChangeUnimprovedNoResource[eTerrain][eYield];
	if (!bLakePlot)
	{
		if (m_ppiPolicyTerrainYieldChangeExcludingLakes)
			iSum += m_ppiPolicyTerrainYieldChangeExcludingLakes[eTerrain][eYield];
		if (bUnimprovedPlot && m_ppiPolicyTerrainYieldChangeUnimprovedExcludingLakes)
			iSum += m_ppiPolicyTerrainYieldChangeUnimprovedExcludingLakes[eTerrain][eYield];
		if (bNoResourcePlot && m_ppiPolicyTerrainYieldChangeNoResourceExcludingLakes)
			iSum += m_ppiPolicyTerrainYieldChangeNoResourceExcludingLakes[eTerrain][eYield];
		if (bUnimprovedPlot && bNoResourcePlot && m_ppiPolicyTerrainYieldChangeUnimprovedNoResourceExcludingLakes)
			iSum += m_ppiPolicyTerrainYieldChangeUnimprovedNoResourceExcludingLakes[eTerrain][eYield];
	}
#endif
	return iSum;
}

int CvPolicyEntry::GetPolicyFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYield, bool bUnimprovedPlot, bool bNoResourcePlot) const
{
	if (eFeature == NO_FEATURE || eYield < 0 || eYield >= NUM_YIELD_TYPES)
		return 0;
	CvAssertMsg(eFeature < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(eFeature > -1, "Index out of bounds");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(eYield > -1, "Index out of bounds");
	int iSum = 0;
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	if (m_ppiPolicyFeatureYieldChange.first)
		iSum += m_ppiPolicyFeatureYieldChange.first[eFeature][eYield];
	if (bUnimprovedPlot && m_ppiPolicyFeatureYieldChangeUnimproved.first)
		iSum += m_ppiPolicyFeatureYieldChangeUnimproved.first[eFeature][eYield];
	if (bNoResourcePlot && m_ppiPolicyFeatureYieldChangeNoResource.first)
		iSum += m_ppiPolicyFeatureYieldChangeNoResource.first[eFeature][eYield];
	if (bUnimprovedPlot && bNoResourcePlot && m_ppiPolicyFeatureYieldChangeUnimprovedNoResource.first)
		iSum += m_ppiPolicyFeatureYieldChangeUnimprovedNoResource.first[eFeature][eYield];
#else
	if (m_ppiPolicyFeatureYieldChange)
		iSum += m_ppiPolicyFeatureYieldChange[eFeature][eYield];
	if (bUnimprovedPlot && m_ppiPolicyFeatureYieldChangeUnimproved)
		iSum += m_ppiPolicyFeatureYieldChangeUnimproved[eFeature][eYield];
	if (bNoResourcePlot && m_ppiPolicyFeatureYieldChangeNoResource)
		iSum += m_ppiPolicyFeatureYieldChangeNoResource[eFeature][eYield];
	if (bUnimprovedPlot && bNoResourcePlot && m_ppiPolicyFeatureYieldChangeUnimprovedNoResource)
		iSum += m_ppiPolicyFeatureYieldChangeUnimprovedNoResource[eFeature][eYield];
#endif
	return iSum;
}
#endif

#ifdef AUI_POLICY_BUILDING_CLASS_FLAVOR_MODIFIERS
int CvPolicyEntry::GetBuildingClassFlavorChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiBuildingClassFlavorChanges[i][j];
}
#endif


int CvPolicyEntry::GetBuildingClassProductionModifier(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiBuildingClassProductionModifiers[i];
}


int CvPolicyEntry::GetBuildingClassTourismModifier(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiBuildingClassTourismModifiers[i];
}


int CvPolicyEntry::GetFlavorValue(int i) const
{
	CvAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}


int CvPolicyEntry::GetImprovementCultureChanges(int i) const
{
	CvAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piImprovementCultureChange[i];
}


BuildingTypes CvPolicyEntry::GetFreeBuildingOnConquest() const
{
	return m_eFreeBuildingOnConquest;
}






CvPolicyBranchEntry::CvPolicyBranchEntry(void):
	m_iEraPrereq(NO_ERA),
	m_iFreePolicy(NO_POLICY),
	m_iFreeFinishingPolicy(NO_POLICY),
	m_iFirstAdopterFreePolicies(0),
	m_iSecondAdopterFreePolicies(0),

	m_piPolicyBranchDisables(NULL)
{
}


CvPolicyBranchEntry::~CvPolicyBranchEntry(void)
{
	SAFE_DELETE_ARRAY(m_piPolicyBranchDisables);
}


bool CvPolicyBranchEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;


	const char* szEraPrereq = kResults.GetText("EraPrereq");
	m_iEraPrereq = GC.getInfoTypeForString(szEraPrereq, true);

	const char* szFreePolicy = kResults.GetText("FreePolicy");
	m_iFreePolicy = GC.getInfoTypeForString(szFreePolicy, true);

	const char* szFreeFinishingPolicy = kResults.GetText("FreeFinishingPolicy");
	m_iFreeFinishingPolicy = GC.getInfoTypeForString(szFreeFinishingPolicy, true);

	m_iFirstAdopterFreePolicies = kResults.GetInt("FirstAdopterFreePolicies");
	m_iSecondAdopterFreePolicies = kResults.GetInt("SecondAdopterFreePolicies");

	const char* szPolicyBranchType = GetType();

	m_bPurchaseByLevel = kResults.GetBool("PurchaseByLevel");
	m_bLockedWithoutReligion = kResults.GetBool("LockedWithoutReligion");
	m_bMutuallyExclusive = kResults.GetBool("AIMutuallyExclusive");

	m_bDelayWhenNoReligion = kResults.GetBool("AIDelayNoReligion");
	m_bDelayWhenNoCulture = kResults.GetBool("AIDelayNoCulture");
	m_bDelayWhenNoCityStates = kResults.GetBool("AIDelayNoCityStates");
	m_bDelayWhenNoScience = kResults.GetBool("AIDelayNoScience");
	m_szIconString = kResults.GetText("IconString");

	{
		kUtility.InitializeArray(m_piPolicyBranchDisables, "PolicyBranchTypes", (int)NO_POLICY_BRANCH_TYPE);

		std::string sqlKey = "m_piPolicyBranchDisables";
		Database::Results* pResults = kUtility.GetOrPrepareResults(sqlKey,
			"select PolicyBranchTypes.ID from PolicyBranch_Disables inner join PolicyBranchTypes on PolicyBranchTypes.Type = PolicyBranchDisable where PolicyBranchType = ?");

		pResults->Bind(1, szPolicyBranchType, false);

		int iID;
		while(pResults->Step())
		{
			iID = pResults->GetInt(0);
			m_piPolicyBranchDisables[iID] = 1;
		}

		pResults->Reset();
	}

	return true;
}


int CvPolicyBranchEntry::GetEraPrereq() const
{
	return m_iEraPrereq;
}


int CvPolicyBranchEntry::GetFreePolicy() const
{
	return m_iFreePolicy;
}


int CvPolicyBranchEntry::GetFreeFinishingPolicy() const
{
	return m_iFreeFinishingPolicy;
}


int CvPolicyBranchEntry::GetFirstAdopterFreePolicies() const
{
	return m_iFirstAdopterFreePolicies;
}


int CvPolicyBranchEntry::GetSecondAdopterFreePolicies() const
{
	return m_iSecondAdopterFreePolicies;
}


int CvPolicyBranchEntry::GetPolicyBranchDisables(int i) const
{
	return m_piPolicyBranchDisables ? m_piPolicyBranchDisables[i] : -1;
}


bool CvPolicyBranchEntry::IsPurchaseByLevel() const
{
	return m_bPurchaseByLevel;
}


bool CvPolicyBranchEntry::IsLockedWithoutReligion() const
{
	return m_bLockedWithoutReligion;
}


bool CvPolicyBranchEntry::IsMutuallyExclusive() const
{
	return m_bMutuallyExclusive;
}


bool CvPolicyBranchEntry::IsDelayWhenNoReligion() const
{
	return m_bDelayWhenNoReligion;
}


bool CvPolicyBranchEntry::IsDelayWhenNoCulture() const
{
	return m_bDelayWhenNoCulture;
}


bool CvPolicyBranchEntry::IsDelayWhenNoCityStates() const
{
	return m_bDelayWhenNoCityStates;
}


bool CvPolicyBranchEntry::IsDelayWhenNoScience() const
{
	return m_bDelayWhenNoScience;
}





CvPolicyXMLEntries::CvPolicyXMLEntries(void)
{

}


CvPolicyXMLEntries::~CvPolicyXMLEntries(void)
{
	DeletePoliciesArray();
	DeletePolicyBranchesArray();
}


std::vector<CvPolicyEntry*>& CvPolicyXMLEntries::GetPolicyEntries()
{
	return m_paPolicyEntries;
}


#ifdef AUI_WARNING_FIXES
uint CvPolicyXMLEntries::GetNumPolicies() const
#else
int CvPolicyXMLEntries::GetNumPolicies()
#endif
{
	return m_paPolicyEntries.size();
}


void CvPolicyXMLEntries::DeletePoliciesArray()
{
	for(std::vector<CvPolicyEntry*>::iterator it = m_paPolicyEntries.begin(); it != m_paPolicyEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paPolicyEntries.clear();
}


#ifdef AUI_WARNING_FIXES
CvPolicyEntry* CvPolicyXMLEntries::GetPolicyEntry(uint index)
#else
CvPolicyEntry* CvPolicyXMLEntries::GetPolicyEntry(int index)
#endif
{
	return m_paPolicyEntries[index];
}


std::vector<CvPolicyBranchEntry*>& CvPolicyXMLEntries::GetPolicyBranchEntries()
{
	return m_paPolicyBranchEntries;
}


#ifdef AUI_WARNING_FIXES
uint CvPolicyXMLEntries::GetNumPolicyBranches() const
#else
int CvPolicyXMLEntries::GetNumPolicyBranches()
#endif
{
	return m_paPolicyBranchEntries.size();
}


void CvPolicyXMLEntries::DeletePolicyBranchesArray()
{
	for(std::vector<CvPolicyBranchEntry*>::iterator it = m_paPolicyBranchEntries.begin(); it != m_paPolicyBranchEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paPolicyBranchEntries.clear();
}


#ifdef AUI_WARNING_FIXES
_Ret_maybenull_ CvPolicyBranchEntry* CvPolicyXMLEntries::GetPolicyBranchEntry(uint index)
#else
CvPolicyBranchEntry* CvPolicyXMLEntries::GetPolicyBranchEntry(int index)
#endif
{
	return m_paPolicyBranchEntries[index];
}





CvPlayerPolicies::CvPlayerPolicies():
	m_pabHasPolicy(NULL),
#ifdef LEKMOD_NEW_LUA_METHODS
	m_pabHasPolicyBranch(NULL),
#endif
	m_pabHasOneShotPolicyFired(NULL),
	m_pabHaveOneShotFreeUnitsFired(NULL),
	m_pabPolicyBranchUnlocked(NULL),
#ifdef POLICY_BRANCH_UNLOCKING_TURN
	m_paiPolicyBranchUnlockingTurn(NULL),
#endif
	m_pabPolicyBranchBlocked(NULL),
	m_pabPolicyBranchFinished(NULL),
	m_paePolicyBranchesChosen(NULL),
	m_paePolicyBlockedBranchCheck(NULL),
	m_pPolicyAI(NULL),
	m_pPolicies(NULL),
#ifdef AUI_WARNING_FIXES
	m_paiPolicyBlockedCount(NULL),
	m_paiPolicyBranchBlockedCount(NULL),
	m_iNumExtraBranches(0),
	m_eBranchPicked1(NO_POLICY_BRANCH_TYPE),
	m_eBranchPicked2(NO_POLICY_BRANCH_TYPE),
	m_eBranchPicked3(NO_POLICY_BRANCH_TYPE),
#endif
	m_pPlayer(NULL)
{
}


CvPlayerPolicies::~CvPlayerPolicies(void)
{
}


void CvPlayerPolicies::Init(CvPolicyXMLEntries* pPolicies, CvPlayer* pPlayer, bool bIsCity)
{

	CvFlavorRecipient::Init();


	m_bIsCity = bIsCity;
	m_pPolicies = pPolicies;
	m_pPlayer = pPlayer;


	CvAssertMsg(m_pabHasPolicy==NULL, "about to leak memory, CvPlayerPolicies::m_pabHasPolicy");
	m_pabHasPolicy = FNEW(bool[m_pPolicies->GetNumPolicies()], c_eCiv5GameplayDLL, 0);
#ifdef LEKMOD_NEW_LUA_METHODS
	CvAssertMsg(m_pabHasPolicyBranch == NULL, "about to leak memory, CvPlayerPolicies::m_pabHasPolicyBranch");
	m_pabHasPolicyBranch = FNEW(bool[m_pPolicies->GetNumPolicyBranches()], c_eCiv5GameplayDLL, 0);
#endif
	CvAssertMsg(m_pabHasOneShotPolicyFired==NULL, "about to leak memory, CvPlayerPolicies::m_pabHasOneShotPolicyFired");
	m_pabHasOneShotPolicyFired = FNEW(bool[m_pPolicies->GetNumPolicies()], c_eCiv5GameplayDLL, 0);
	CvAssertMsg(m_pabHaveOneShotFreeUnitsFired==NULL, "about to leak memory, CvPlayerPolicies::m_pabHaveOneShotFreeUnitsFired");
	m_pabHaveOneShotFreeUnitsFired = FNEW(bool[m_pPolicies->GetNumPolicies()], c_eCiv5GameplayDLL, 0);


	CvAssertMsg(m_pabPolicyBranchUnlocked==NULL, "about to leak memory, CvPlayerPolicies::m_pabPolicyBranchUnlocked");
	m_pabPolicyBranchUnlocked = FNEW(bool[m_pPolicies->GetNumPolicyBranches()], c_eCiv5GameplayDLL, 0);
#ifdef POLICY_BRANCH_UNLOCKING_TURN
	CvAssertMsg(m_paiPolicyBranchUnlockingTurn == NULL, "about to leak memory, CvPlayerPolicies::m_paiPolicyBranchUnlockingTurn");
	m_paiPolicyBranchUnlockingTurn = FNEW(int[m_pPolicies->GetNumPolicyBranches()], c_eCiv5GameplayDLL, 0);
#endif


	CvAssertMsg(m_pabPolicyBranchBlocked==NULL, "about to leak memory, CvPlayerPolicies::m_pabPolicyBranchBlocked");
	m_pabPolicyBranchBlocked = FNEW(bool[m_pPolicies->GetNumPolicyBranches()], c_eCiv5GameplayDLL, 0);


	CvAssertMsg(m_pabPolicyBranchFinished==NULL, "about to leak memory, CvPlayerPolicies::m_pabPolicyBranchFinished");
	m_pabPolicyBranchFinished = FNEW(bool[m_pPolicies->GetNumPolicyBranches()], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paePolicyBranchesChosen==NULL, "about to leak memory, CvPlayerPolicies::m_paePolicyBranchesChosen");
	m_paePolicyBranchesChosen = FNEW(PolicyBranchTypes[m_pPolicies->GetNumPolicyBranches()], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paePolicyBlockedBranchCheck==NULL, "about to leak memory, CvPlayerPolicies::m_paePolicyBlockedBranchCheck");
	m_paePolicyBlockedBranchCheck = FNEW(PolicyBranchTypes[m_pPolicies->GetNumPolicies()], c_eCiv5GameplayDLL, 0);
	

	m_pPolicyAI = FNEW(CvPolicyAI(this), c_eCiv5GameplayDLL, 0);

	Reset();
}


void CvPlayerPolicies::Uninit()
{

	CvFlavorRecipient::Uninit();

	SAFE_DELETE_ARRAY(m_pabHasPolicy);
#ifdef LEKMOD_NEW_LUA_METHODS
	SAFE_DELETE_ARRAY(m_pabHasPolicyBranch);
#endif
	SAFE_DELETE_ARRAY(m_pabHasOneShotPolicyFired);
	SAFE_DELETE_ARRAY(m_pabHaveOneShotFreeUnitsFired);
	SAFE_DELETE_ARRAY(m_pabPolicyBranchUnlocked);
#ifdef POLICY_BRANCH_UNLOCKING_TURN
	SAFE_DELETE_ARRAY(m_paiPolicyBranchUnlockingTurn);
#endif
	SAFE_DELETE_ARRAY(m_pabPolicyBranchBlocked);
	SAFE_DELETE_ARRAY(m_pabPolicyBranchFinished);
	SAFE_DELETE_ARRAY(m_paePolicyBranchesChosen);
	SAFE_DELETE(m_pPolicyAI);
	SAFE_DELETE_ARRAY(m_paePolicyBlockedBranchCheck);
}


void CvPlayerPolicies::Reset()
{
#ifdef AUI_WARNING_FIXES
	uint iI;
#else
	int iI;
#endif

	for(iI = 0; iI < m_pPolicies->GetNumPolicies(); iI++)
	{
		m_pabHasPolicy[iI] = false;
		m_pabHasOneShotPolicyFired[iI] = false;
		m_pabHaveOneShotFreeUnitsFired[iI] = false;
		m_paePolicyBlockedBranchCheck[iI] = (PolicyBranchTypes)-2;
	}

	for(iI = 0; iI < m_pPolicies->GetNumPolicyBranches(); iI++)
	{
#ifdef LEKMOD_NEW_LUA_METHODS
		m_pabHasPolicyBranch[iI] = false;
#endif
		m_pabPolicyBranchUnlocked[iI] = false;
#ifdef POLICY_BRANCH_UNLOCKING_TURN
		m_paiPolicyBranchUnlockingTurn[iI] = -1;
#endif
		m_pabPolicyBranchBlocked[iI] = false;
		m_pabPolicyBranchFinished[iI] = false;
		m_paePolicyBranchesChosen[iI] = NO_POLICY_BRANCH_TYPE;
	}

	m_iNumExtraBranches = 0;

	m_eBranchPicked1 = NO_POLICY_BRANCH_TYPE;
	m_eBranchPicked2 = NO_POLICY_BRANCH_TYPE;
	m_eBranchPicked3 = NO_POLICY_BRANCH_TYPE;


	m_pPolicyAI->Reset();


	CvAssert( m_pPolicies->GetNumPolicies() == m_pPolicies->GetNumPolicies());

	for (iI = 0; iI < m_pPolicies->GetNumPolicies(); ++iI)
	{
		PolicyTypes eType = (PolicyTypes) iI;
		CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(eType);
		if(pkPolicyInfo == NULL)
		{
			m_paePolicyBlockedBranchCheck[eType] = NO_POLICY_BRANCH_TYPE;
		}
		else
		{

			PolicyBranchTypes eBranch = (PolicyBranchTypes) pkPolicyInfo->GetPolicyBranchType();


			if(eBranch == NO_POLICY_BRANCH_TYPE)
			{
				int iNumPolicyBranches = m_pPolicies->GetNumPolicyBranches();
				for(int iBranchLoop = 0; iBranchLoop < iNumPolicyBranches; iBranchLoop++)
				{
					const PolicyBranchTypes eLoopBranch = static_cast<PolicyBranchTypes>(iBranchLoop);
					CvPolicyBranchEntry* pkLoopPolicyBranch = GC.getPolicyBranchInfo(eLoopBranch);
					if(pkLoopPolicyBranch)
					{

						if(pkLoopPolicyBranch->GetFreePolicy() == eType)
						{
							eBranch = eLoopBranch;
							break;
						}
					}
				}
			}

			m_paePolicyBlockedBranchCheck[eType] = eBranch;
		}
	}
}


void CvPlayerPolicies::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	CvAssertMsg(m_pPolicies != NULL && m_pPolicies->GetNumPolicies() > 0, "Number of policies to serialize is expected to greater than 0");

	uint uiPolicyCount = 0;
	uint uiPolicyBranchCount = 0;
	if(m_pPolicies)
	{
		uiPolicyCount = m_pPolicies->GetNumPolicies();
		uiPolicyBranchCount = m_pPolicies->GetNumPolicyBranches();
	}

	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabHasPolicy, uiPolicyCount);
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabHasOneShotPolicyFired, uiPolicyCount);
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabHaveOneShotFreeUnitsFired, uiPolicyCount);

#ifdef LEKMOD_NEW_LUA_METHODS
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabHasPolicyBranch, uiPolicyBranchCount);
#endif

	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabPolicyBranchUnlocked, uiPolicyBranchCount);
#ifdef POLICY_BRANCH_UNLOCKING_TURN
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiPolicyBranchUnlockingTurn, uiPolicyBranchCount);
#endif
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabPolicyBranchBlocked, uiPolicyBranchCount);
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabPolicyBranchFinished, uiPolicyBranchCount);
	CvInfosSerializationHelper::ReadHashedTypeArray(kStream, m_paePolicyBranchesChosen, uiPolicyBranchCount);

	kStream >> m_iNumExtraBranches;

	m_eBranchPicked1 = (PolicyBranchTypes)CvInfosSerializationHelper::ReadHashed(kStream);
	m_eBranchPicked2 = (PolicyBranchTypes)CvInfosSerializationHelper::ReadHashed(kStream);
	m_eBranchPicked3 = (PolicyBranchTypes)CvInfosSerializationHelper::ReadHashed(kStream);

	if (uiVersion < 2)
	{
		int temp;
		kStream >> temp;
	}


	m_pPolicyAI->Read(kStream);

	CvAssertMsg(m_piLatestFlavorValues != NULL && GC.getNumFlavorTypes() > 0, "Number of flavor values to serialize is expected to greater than 0");

	int iNumFlavors;
	kStream >> iNumFlavors;

	ArrayWrapper<int> wrapm_piLatestFlavorValues(iNumFlavors, m_piLatestFlavorValues);
	kStream >> wrapm_piLatestFlavorValues;
}


void CvPlayerPolicies::Write(FDataStream& kStream) const
{

	uint uiVersion = 2;
	kStream << uiVersion;

	CvAssertMsg(m_pPolicies != NULL && GC.getNumPolicyInfos() > 0, "Number of policies to serialize is expected to greater than 0");

	uint uiPolicyCount = 0;
	uint uiPolicyBranchCount = 0;
	if(m_pPolicies)
	{
		uiPolicyCount = m_pPolicies->GetNumPolicies();
		uiPolicyBranchCount = m_pPolicies->GetNumPolicyBranches();
	}

	CvInfosSerializationHelper::WriteHashedDataArray<PolicyTypes>(kStream, m_pabHasPolicy, uiPolicyCount);
	CvInfosSerializationHelper::WriteHashedDataArray<PolicyTypes>(kStream, m_pabHasOneShotPolicyFired, uiPolicyCount);
	CvInfosSerializationHelper::WriteHashedDataArray<PolicyTypes>(kStream, m_pabHaveOneShotFreeUnitsFired, uiPolicyCount);
#ifdef LEKMOD_NEW_LUA_METHODS
	CvInfosSerializationHelper::WriteHashedDataArray<PolicyBranchTypes>(kStream, m_pabHasPolicyBranch, uiPolicyBranchCount);
#endif
	CvInfosSerializationHelper::WriteHashedDataArray<PolicyBranchTypes>(kStream, m_pabPolicyBranchUnlocked, uiPolicyBranchCount);
#ifdef POLICY_BRANCH_UNLOCKING_TURN
	CvInfosSerializationHelper::WriteHashedDataArray<PolicyBranchTypes>(kStream, m_paiPolicyBranchUnlockingTurn, uiPolicyBranchCount);
#endif
	CvInfosSerializationHelper::WriteHashedDataArray<PolicyBranchTypes>(kStream, m_pabPolicyBranchBlocked, uiPolicyBranchCount);
	CvInfosSerializationHelper::WriteHashedDataArray<PolicyBranchTypes>(kStream, m_pabPolicyBranchFinished, uiPolicyBranchCount);
	CvInfosSerializationHelper::WriteHashedTypeArray<PolicyBranchTypes>(kStream, m_paePolicyBranchesChosen, uiPolicyBranchCount);

	kStream << m_iNumExtraBranches;

	CvInfosSerializationHelper::WriteHashed(kStream, m_eBranchPicked1);
	CvInfosSerializationHelper::WriteHashed(kStream, m_eBranchPicked2);
	CvInfosSerializationHelper::WriteHashed(kStream, m_eBranchPicked3);


	m_pPolicyAI->Write(kStream);

	CvAssertMsg(m_piLatestFlavorValues != NULL && GC.getNumFlavorTypes() > 0, "Number of flavor values to serialize is expected to greater than 0");
	kStream << GC.getNumFlavorTypes();
	kStream << ArrayWrapper<int>(GC.getNumFlavorTypes(), m_piLatestFlavorValues);
}


void CvPlayerPolicies::FlavorUpdate()
{
	AddFlavorAsStrategies(GC.getPOLICY_WEIGHT_PROPAGATION_PERCENT());
}


CvPlayer* CvPlayerPolicies::GetPlayer()
{
	return m_pPlayer;
}


bool CvPlayerPolicies::HasPolicy(PolicyTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumPolicyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabHasPolicy[eIndex];
}
#ifdef LEKMOD_NEW_LUA_METHODS

bool CvPlayerPolicies::HasPolicyBranch(PolicyBranchTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumPolicyBranchInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabHasPolicyBranch[eIndex];
}
#endif

void CvPlayerPolicies::SetPolicy(PolicyTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < m_pPolicies->GetNumPolicies(), "eIndex is expected to be within maximum bounds (invalid Index)");

	CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(eIndex);
	if(pkPolicyInfo == NULL)
		return;

	if(HasPolicy(eIndex) != bNewValue)
	{
		m_pabHasPolicy[eIndex] = bNewValue;

		int iChange = bNewValue ? 1 : -1;
		GetPlayer()->ChangeNumPolicies(iChange);

		if(bNewValue)
		{
			DoNewPolicyPickedForHistory(eIndex);

			if(m_pPlayer->GetID() == GC.getGame().getActivePlayer())
				GC.GetEngineUserInterface()->SetPolicyNotificationSeen(false);
		}

		PolicyBranchTypes eThisBranch = (PolicyBranchTypes) pkPolicyInfo->GetPolicyBranchType();

		if(eThisBranch != NO_POLICY_BRANCH_TYPE)
		{
			bool bBranchFinished;


			if(!bNewValue)
			{
				bBranchFinished = false;
			}

			else
			{
				bBranchFinished = true;


#ifdef AUI_WARNING_FIXES
				for (uint iPolicyLoop = 0; iPolicyLoop < GetPolicies()->GetNumPolicies(); iPolicyLoop++)
#else
				for(int iPolicyLoop = 0; iPolicyLoop < GetPolicies()->GetNumPolicies(); iPolicyLoop++)
#endif
				{
					const PolicyTypes eLoopPolicy = static_cast<PolicyTypes>(iPolicyLoop);

					CvPolicyEntry* pkLoopPolicyInfo = GC.getPolicyInfo(eLoopPolicy);
					if(pkLoopPolicyInfo)
					{

						if(pkLoopPolicyInfo->GetPolicyBranchType() == eThisBranch)
						{

							if(!HasPolicy(eLoopPolicy))
							{
								bBranchFinished = false;


								break;
							}
						}
					}
				}
			}

			SetPolicyBranchFinished(eThisBranch, bBranchFinished);

			if(bBranchFinished)
			{
				CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(eThisBranch);
				if(pkPolicyBranchInfo)
				{
					PolicyTypes eFinisher = (PolicyTypes)pkPolicyBranchInfo->GetFreeFinishingPolicy();
					if(eFinisher != NO_POLICY)
					{
						GetPlayer()->setHasPolicy(eFinisher, true);
						GetPlayer()->ChangeNumFreePoliciesEver(1);
					}
				}
			}
		}
	}
}


bool CvPlayerPolicies::HasOneShotPolicyFired(PolicyTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumPolicyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabHasOneShotPolicyFired[eIndex];
}


void CvPlayerPolicies::SetOneShotPolicyFired(PolicyTypes eIndex, bool bFired)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumPolicyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_pabHasOneShotPolicyFired[eIndex] = bFired;
}

bool CvPlayerPolicies::HaveOneShotFreeUnitsFired(PolicyTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumPolicyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabHaveOneShotFreeUnitsFired[eIndex];
}


void CvPlayerPolicies::SetOneShotFreeUnitsFired(PolicyTypes eIndex, bool bFired)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumPolicyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_pabHaveOneShotFreeUnitsFired[eIndex] = bFired;
}


int CvPlayerPolicies::GetNumPoliciesOwned() const
{
	int rtnValue = 0;

#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#else
	for(int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#endif
	{

		if(m_pabHasPolicy[i])
		{
			rtnValue++;
		}
	}

	return rtnValue;
}


int CvPlayerPolicies::GetNumPoliciesOwnedInBranch(PolicyBranchTypes eBranch) const
{
	int rtnValue = 0;

#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#else
	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#endif
	{

		if (m_pabHasPolicy[i] && m_pPolicies->GetPolicyEntry(i)->GetPolicyBranchType() == eBranch)
		{
			rtnValue++;
		}
	}

	return rtnValue;
}


CvPolicyXMLEntries* CvPlayerPolicies::GetPolicies() const
{
	return m_pPolicies;
}


int CvPlayerPolicies::GetNumericModifier(PolicyModifierType eType)
{
	int rtnValue = 0;

	int iNumPolicies = m_pPolicies->GetNumPolicies();
	for(int i = 0; i < iNumPolicies; i++)
	{

		if(m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{

			switch(eType)
			{
			case POLICYMOD_EXTRA_HAPPINESS:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetExtraHappiness();
				break;
			case POLICYMOD_EXTRA_HAPPINESS_PER_CITY:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetExtraHappinessPerCity();
				break;
			case POLICYMOD_GREAT_PERSON_RATE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGreatPeopleRateModifier();
				break;
			case POLICYMOD_GREAT_GENERAL_RATE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGreatGeneralRateModifier();
				break;
			case POLICYMOD_GREAT_ADMIRAL_RATE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGreatAdmiralRateModifier();
				break;
			case POLICYMOD_GREAT_WRITER_RATE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGreatWriterRateModifier();
				break;
			case POLICYMOD_GREAT_ARTIST_RATE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGreatArtistRateModifier();
				break;
			case POLICYMOD_GREAT_MUSICIAN_RATE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGreatMusicianRateModifier();
				break;
			case POLICYMOD_GREAT_MERCHANT_RATE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGreatMerchantRateModifier();
				break;
			case POLICYMOD_GREAT_SCIENTIST_RATE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGreatScientistRateModifier();
				break;
			case POLICYMOD_DOMESTIC_GREAT_GENERAL_RATE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetDomesticGreatGeneralRateModifier();
				break;
			case POLICYMOD_POLICY_COST_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetPolicyCostModifier();
				break;
			case POLICYMOD_WONDER_PRODUCTION_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetWonderProductionModifier();
				break;
			case POLICYMOD_BUILDING_PRODUCTION_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetBuildingProductionModifier();
				break;
			case POLICYMOD_FREE_EXPERIENCE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetFreeExperience();
				break;
			case POLICYMOD_EXTRA_CULTURE_FROM_IMPROVEMENTS:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetCultureImprovementChange();
				break;
			case POLICYMOD_CULTURE_FROM_KILLS:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetCultureFromKills();
				break;
			case POLICYMOD_EMBARKED_EXTRA_MOVES:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetEmbarkedExtraMoves();
				break;
			case POLICYMOD_CULTURE_FROM_BARBARIAN_KILLS:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetCultureFromBarbarianKills();
				break;
			case POLICYMOD_GOLD_FROM_KILLS:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGoldFromKills();
				break;

			case POLICYMOD_SCIENCE_FROM_KILLS:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetScienceFromKills();
				break;

			case POLICYMOD_CULTURE_FROM_GARRISON:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetCulturePerGarrisonedUnit();
				break;
			case POLICYMOD_UNIT_FREQUENCY_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetCityStateUnitFrequencyModifier();
				break;

			case POLICYMOD_MINOR_MILITARY_NUM_EXTRA_UNITS_TO_GIFT:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetMinorMilitaryNumExtraUnitsToGift();
				break;
#if !defined(LEKMOD_FIX_PATRO_FOOD)
			case POLICYMOD_CITY_STATE_BONUS_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetCityStateBonusModifier();
				break;
#endif


			case POLICYMOD_EXTRA_TERRITORY_CLAIM:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetExtraTerritoryClaim();
				break;


			case POLICYMOD_EXTRA_TOURISM_PER_GREAT_WORK:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetExtraTourismPerGreatWork();
				break;


			case POLICYMOD_TOURISM_PER_WONDER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetTourismPerWonder();
				break;

#ifdef NQ_TOURISM_PER_CITY
			case POLICYMOD_TOURISM_PER_CITY:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetTourismPerCity();
				break;
#endif
#ifdef NQ_IDEOLOGY_PRESSURE_UNHAPPINESS_MODIFIER_FROM_POLICIES
			case POLICYMOD_IDEOLOGY_PRESSURE_UNHAPPINESS_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetIdeologyPressureUnhappinessModifier();
				break;
#endif

			case POLICYMOD_PRODUCTION_FROM_GARRISON:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetProductionFromGarrison();
				break;

			case POLICYMOD_TOURISM_MOD_COMMON_FOE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetCommonFoeTourismModifier();
				break;
			case POLICYMOD_TOURISM_MOD_LESS_HAPPY:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetLessHappyTourismModifier();
				break;
			case POLICYMOD_TOURISM_MOD_SHARED_IDEOLOGY:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetSharedIdeologyTourismModifier();
				break;
			case POLICYMOD_TRADE_MISSION_GOLD_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetTradeMissionGoldModifier();
				break;
			case POLICYMOD_FAITH_COST_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetFaithCostModifier();
				break;
			case POLICYMOD_CULTURAL_PLUNDER_MULTIPLIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetCulturalPlunderMultiplier();
				break;
			case POLICYMOD_STEAL_TECH_SLOWER_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetStealTechSlowerModifier();
				break;
			case POLICYMOD_STEAL_TECH_FASTER_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetStealTechFasterModifier();
				break;
			case POLICYMOD_CATCH_SPIES_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetCatchSpiesModifier();
				break;
			case POLICYMOD_BUILDING_PURCHASE_COST_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetBuildingPurchaseCostModifier();
				break;
			case POLICYMOD_LAND_TRADE_GOLD_CHANGE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetLandTradeRouteGoldChange();
				break;
			case POLICYMOD_SEA_TRADE_GOLD_CHANGE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetSeaTradeRouteGoldChange();
				break;

			case POLICYMOD_INTERNAL_TRADE_GOLD_CHANGE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetInternalTradeRouteGoldChange();
				break;

			case POLICYMOD_SHARED_IDEOLOGY_TRADE_CHANGE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetSharedIdeologyTradeGoldChange();
				break;
			case POLICYMOD_RIGGING_ELECTION_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetRiggingElectionModifier();
				break;
			case POLICYMOD_MILITARY_UNIT_GIFT_INFLUENCE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetMilitaryUnitGiftExtraInfluence();
				break;
			case POLICYMOD_PROTECTED_MINOR_INFLUENCE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetProtectedMinorPerTurnInfluence();
				break;
			case POLICYMOD_AFRAID_INFLUENCE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetAfraidMinorPerTurnInfluence();
				break;
			case POLICYMOD_MINOR_BULLY_SCORE_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetMinorBullyScoreModifier();
				break;
			case POLICYMOD_THEMING_BONUS:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetThemingBonusMultiplier();
				break;
			case POLICYMOD_CITY_STATE_TRADE_CHANGE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetCityStateTradeChange();
				break;
			case POLICYMOD_INTERNAL_TRADE_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetInternalTradeRouteYieldModifier();
				break;
#ifdef FRUITY_TRADITION_LANDED_ELITE
			case POLICYMOD_INTERNAL_TRADE_FOOD_YIELD_CHANGE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetInternalTradeRouteFoodYieldChange();
				break;
#endif
#ifdef NQ_INTERNAL_TRADE_ROUTE_PRODUCTION_YIELD_CHANGE_FROM_POLICIES
			case POLICYMOD_INTERNAL_TRADE_PRODUCTION_YIELD_CHANGE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetInternalTradeRouteProductionYieldChange();
				break;
#endif
#ifdef NQ_RAIL_CONNECTION_PRODUCTION_MODIFIER_FROM_POLICIES
			case POLICYMOD_RAIL_CONNECTION_PRODUCTION_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetRailConnectionProductionModifier();
				break;
#endif
#ifdef NQ_COMBAT_BONUS_VS_SMALLER_CIV_FROM_POLICIES
			case POLICYMOD_COMBAT_BONUS_VS_SMALLER_CIV:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetCombatBonusVsSmallerCiv();
				break;
#endif
#ifdef NQ_PRODUCTION_TO_GREAT_MUSICIANS_MODIFIER_FROM_POLICIES
			case POLICYMOD_PRODUCTION_TO_GREAT_MUSICIANS_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetProductionToGreatMusiciansModifier();
				break;
#endif
#ifdef NQ_TRADE_MISSION_INFLUENCE_MODIFIER_FROM_POLICIES
			case POLICYMOD_TRADE_MISSION_INFLUENCE_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetTradeMissionInfluenceModifier();
				break;
#endif
#ifdef NQ_SCIENCE_PER_GREAT_PERSON_BORN_FROM_POLICIES
			case POLICYMOD_SCIENCE_PER_GREAT_PERSON_BORN:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetSciencePerGreatPersonBorn();
				break;
#endif
#ifdef NQ_INFLUENCE_BOOST_PER_GREAT_PERSON_BORN_FROM_POLICIES
			case POLICYMOD_INFLUENCE_BOOST_PER_GREAT_PERSON_BORN:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetInfluenceBoostPerGreatPersonBorn();
				break;
#endif
#ifdef NQ_TOURISM_FROM_TRADE_MISSIONS_FROM_POLICIES
			case POLICYMOD_TOURISM_FROM_TRADE_MISSIONS:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetTourismFromTradeMissions();
				break;
#endif
#ifdef NQ_HAPPINESS_FROM_GREAT_IMPROVEMENTS_FROM_POLICIES
			case POLICYMOD_HAPPINESS_FROM_GREAT_IMPROVEMENTS:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetHappinessFromGreatImprovements();
				break;
#endif
#ifdef NQ_MINOR_FRIENDSHIP_GAIN_BULLY_GOLD_SUCCESS_FROM_POLICIES
			case POLICYMOD_MINOR_FRIENDSHIP_GAIN_BULLY_GOLD_SUCCESS:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetMinorFriendshipGainBullyGoldSuccess();
				break;
#endif
#ifdef NQ_COUP_CHANCE_MODIFIER_FROM_POLICIES
			case POLICYMOD_COUP_CHANCE_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetCoupChanceModifier();
				break;
#endif
#ifdef NQ_SPREAD_MODIFIER_OWNED_CITIES
			case POLICYMOD_SPREAD_MODIFIER_OWNED_CITIES:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetReligionSpreadModifierFriendly();
				break;
			case POLICYMOD_SPREAD_MODIFIER_OTHER_CITIES:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetReligionSpreadModifierForeign();
				break;
#endif
			case POLICYMOD_SHARED_RELIGION_TOURISM_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetSharedReligionTourismModifier();
				break;
			case POLICYMOD_TRADE_ROUTE_TOURISM_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetTradeRouteTourismModifier();
				break;
			case POLICYMOD_OPEN_BORDERS_TOURISM_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetOpenBordersTourismModifier();
				break;
#if defined(MISC_CHANGES)
			case POLICYMOD_NUM_TRADE_ROUTES_BONUS:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetNumTradeRouteBonus();
				break;
#endif
#ifdef LEKMOD_POLICIES_GLOBAL_MOVE_CHANGE
			case POLICYMOD_GLOBAL_MOVE_CHANGE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGlobalMoveChange();
				break;
			case POLICYMOD_GLOBAL_MOVE_CHANGE_FRIENDLY:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGlobalMoveChangeFriendly();
				break;
			case POLICYMOD_GLOBAL_MOVE_CHANGE_ENEMY:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGlobalMoveChangeEnemy();
				break;
#endif
			}
		}
	}

	return rtnValue;
}

#ifdef NQ_IDEOLOGY_PRESSURE_UNHAPPINESS_MODIFIER_FROM_POLICIES

int CvPlayerPolicies::GetIdeologyPressureUnhappinessMultiplierTimes100()
{
	int iMultiplier = 100;
	const int iNumPolicies = m_pPolicies->GetNumPolicies();
	for(int i = 0; i < iNumPolicies; i++)
	{
		if(m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			const int m = m_pPolicies->GetPolicyEntry(i)->GetIdeologyPressureUnhappinessModifier();
			if(m != 0)
			{
				iMultiplier = iMultiplier * (100 + m) / 100;
			}
		}
	}
	return iMultiplier;
}
#endif


int CvPlayerPolicies::GetYieldModifier(YieldTypes eYieldType)
{
	int rtnValue = 0;

#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#else
	for(int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#endif
	{

		if(m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			rtnValue += m_pPolicies->GetPolicyEntry(i)->GetYieldModifier(eYieldType);
		}
	}

	return rtnValue;
}


int CvPlayerPolicies::GetBuildingClassYieldModifier(BuildingClassTypes eBuildingClass, YieldTypes eYieldType)
{
	int rtnValue = 0;

#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#else
	for(int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#endif
	{

		if(m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			rtnValue += m_pPolicies->GetPolicyEntry(i)->GetBuildingClassYieldModifiers(eBuildingClass, eYieldType);
		}
	}

	return rtnValue;
}


int CvPlayerPolicies::GetBuildingClassYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYieldType)
{
	int rtnValue = 0;

#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#else
	for(int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#endif
	{

		if(m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			rtnValue += m_pPolicies->GetPolicyEntry(i)->GetBuildingClassYieldChanges(eBuildingClass, eYieldType);
		}
	}

	return rtnValue;
}

#if defined(LEKMOD_POLICY_TERRAIN_FEATURE_YIELDS)
int CvPlayerPolicies::GetPolicyTerrainYieldChange(TerrainTypes eTerrain, YieldTypes eYield, bool bUnimprovedPlot, bool bNoResourcePlot, bool bLakePlot)
{
	int rtnValue = 0;
#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#else
	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#endif
	{
		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			rtnValue += m_pPolicies->GetPolicyEntry(i)->GetPolicyTerrainYieldChange(eTerrain, eYield, bUnimprovedPlot, bNoResourcePlot, bLakePlot);
		}
	}
	return rtnValue;
}

int CvPlayerPolicies::GetPolicyFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYield, bool bUnimprovedPlot, bool bNoResourcePlot)
{
	int rtnValue = 0;
#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#else
	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#endif
	{
		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			rtnValue += m_pPolicies->GetPolicyEntry(i)->GetPolicyFeatureYieldChange(eFeature, eYield, bUnimprovedPlot, bNoResourcePlot);
		}
	}
	return rtnValue;
}
#endif

#if defined(LEKMOD_POLICY_MINOR_BULLY_TRIBUTE)
int CvPlayerPolicies::GetMinorBullyInfluenceReward() const
{
	int rtnValue = 0;
#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#else
	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#endif
	{
		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
			rtnValue += m_pPolicies->GetPolicyEntry(i)->GetMinorBullyInfluenceReward();
	}
	return rtnValue;
}

bool CvPlayerPolicies::IsMinorBullyNoPenalty() const
{
#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#else
	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#endif
	{
		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i) && m_pPolicies->GetPolicyEntry(i)->IsMinorBullyNoPenalty())
			return true;
	}
	return false;
}
#endif

#if defined(LEKMOD_POLICY_GREATPERSON_IMPROVEMENT_ADJACENCY_YIELD)
int CvPlayerPolicies::GetPolicyGreatPersonImprovementAdjacencyYieldBonus(ImprovementTypes eImprovement, YieldTypes eYield)
{
	int rtnValue = 0;
#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#else
	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#endif
	{
		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGreatPersonImprovementAdjacencyYieldBonus(eImprovement, eYield);
		}
	}
	return rtnValue;
}
#endif


int CvPlayerPolicies::GetImprovementCultureChange(ImprovementTypes eImprovement)
{
	int rtnValue = 0;

#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#else
	for(int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#endif
	{

		if(m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			rtnValue += m_pPolicies->GetPolicyEntry(i)->GetImprovementCultureChanges(eImprovement);
		}
	}

	return rtnValue;
}


int CvPlayerPolicies::GetBuildingClassProductionModifier(BuildingClassTypes eBuildingClass)
{
	int rtnValue = 0;

#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#else
	for(int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#endif
	{

		if(m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			rtnValue += m_pPolicies->GetPolicyEntry(i)->GetBuildingClassProductionModifier(eBuildingClass);
		}
	}

	return rtnValue;
}


int CvPlayerPolicies::GetBuildingClassTourismModifier(BuildingClassTypes eBuildingClass)
{
	int rtnValue = 0;

#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#else
	for(int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#endif
	{

		if(m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			rtnValue += m_pPolicies->GetPolicyEntry(i)->GetBuildingClassTourismModifier(eBuildingClass);
		}
	}

	return rtnValue;
}


bool CvPlayerPolicies::HasPolicyEncouragingGarrisons() const
{
#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#else
	for(int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#endif
	{

		if(m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			CvPolicyEntry* pPolicy = m_pPolicies->GetPolicyEntry(i);
			if(pPolicy->GetGarrisonedCityRangeStrikeModifier() > 0)
			{
				return true;
			}
			else if(pPolicy->GetCulturePerGarrisonedUnit() > 0)
			{
				return true;
			}
			else if(pPolicy->GetHappinessPerGarrisonedUnit() > 0)
			{
				return true;
			}
			else if(pPolicy->IsGarrisonFreeMaintenance())
			{
				return true;
			}
		}
	}

	return false;
}


bool CvPlayerPolicies::HasPolicyGrantingReformationBelief() const
{
#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#else
	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#endif
	{

		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			CvPolicyEntry *pPolicy = m_pPolicies->GetPolicyEntry(i);
			if (pPolicy->IsAddReformationBelief())
			{
				return true;
			}
		}
	}

	return false;
}


CvString CvPlayerPolicies::GetWeLoveTheKingString()
{
	CvString rtnValue;



#ifdef AUI_WARNING_FIXES
	for (uint i = m_pPolicies->GetNumPolicies(); i > 0; i--)
	{

		if (m_pabHasPolicy[i - 1] && !IsPolicyBlocked((PolicyTypes)(i - 1)))
		{

			CvString str = m_pPolicies->GetPolicyEntry(i - 1)->GetWeLoveTheKing();
#else
	for(int i = m_pPolicies->GetNumPolicies() - 1; i >= 0; i--)
	{

		if(m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{

			CvString str = m_pPolicies->GetPolicyEntry(i)->GetWeLoveTheKing();
#endif
			if(str.length() > 0)
			{
				rtnValue = str;
				break;
			}
		}
	}

	return rtnValue;
}


std::vector<BuildingTypes> CvPlayerPolicies::GetFreeBuildingsOnConquest()
{
	std::vector<BuildingTypes> freeBuildings;
	freeBuildings.reserve(m_pPolicies->GetNumPolicies());

#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#else
	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#endif
	{

		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			CvPolicyEntry *pPolicy = m_pPolicies->GetPolicyEntry(i);
			BuildingTypes eFreeBuilding = pPolicy->GetFreeBuildingOnConquest();
			if (eFreeBuilding)
			{
				freeBuildings.push_back(eFreeBuilding);
			}
		}
	}
	return freeBuildings;
}
#if defined(LEKMOD_NONCIV_BUILDINGCLASS_YIELD_CHANGE)
int CvPlayerPolicies::GetBuildingClassHappiness(BuildingClassTypes eBuildingClass) const
{
	int iHappiness = 0;

	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{

		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			CvPolicyEntry* pPolicy = m_pPolicies->GetPolicyEntry(i);
			if (pPolicy->GetBuildingClassHappiness(eBuildingClass) > 0)
			{
				iHappiness += pPolicy->GetBuildingClassHappiness(eBuildingClass);
			}
		}
	}

	return iHappiness;
}
#endif

int CvPlayerPolicies::GetTourismFromUnitCreation(UnitClassTypes eUnitClass) const
{
	int iTourism = 0;

#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#else
	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
#endif
	{

		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			CvPolicyEntry *pPolicy = m_pPolicies->GetPolicyEntry(i);
			if (pPolicy->GetTourismByUnitClassCreated(eUnitClass) > 0)
			{
				iTourism += pPolicy->GetTourismByUnitClassCreated(eUnitClass);
			}
		}
	}

	return iTourism;
}
#if defined(TRADE_REFACTOR)

int CvPlayerPolicies::GetMinorTradeRouteDomainYieldChanges(DomainTypes eDomain, YieldTypes eYield) const
{
	int iYield = 0;
	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{

		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			CvPolicyEntry* pPolicy = m_pPolicies->GetPolicyEntry(i);
			if (pPolicy->GetMinorTradeRouteDomainYieldChanges(eDomain, eYield) > 0)
			{
				iYield += pPolicy->GetMinorTradeRouteDomainYieldChanges(eDomain, eYield);
			}
		}
	}
	return iYield;
}

int CvPlayerPolicies::GetTradeConnectionLandYieldChanges(TradeConnectionType eTradeConnection, YieldTypes eYield) const
{
	int iYield = 0;
	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{

		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			CvPolicyEntry* pPolicy = m_pPolicies->GetPolicyEntry(i);
			if (pPolicy->GetTradeConnectionLandYieldChanges(eTradeConnection, eYield) > 0)
			{
				iYield += pPolicy->GetTradeConnectionLandYieldChanges(eTradeConnection, eYield);
			}
		}
	}
	return iYield;
}

int CvPlayerPolicies::GetTradeConnectionSeaYieldChanges(TradeConnectionType eTradeConnection, YieldTypes eYield) const
{
	int iYield = 0;
	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{

		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			CvPolicyEntry* pPolicy = m_pPolicies->GetPolicyEntry(i);
			if (pPolicy->GetTradeConnectionSeaYieldChanges(eTradeConnection, eYield) > 0)
			{
				iYield += pPolicy->GetTradeConnectionSeaYieldChanges(eTradeConnection, eYield);
			}
		}
	}
	return iYield;
}

int CvPlayerPolicies::GetTradeConnectionLandYieldModifier(TradeConnectionType eTradeConnection, YieldTypes eYield) const
{
	int iYield = 0;
	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{

		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			CvPolicyEntry* pPolicy = m_pPolicies->GetPolicyEntry(i);
			if (pPolicy->GetTradeConnectionLandYieldModifier(eTradeConnection, eYield) > 0)
			{
				iYield += pPolicy->GetTradeConnectionLandYieldModifier(eTradeConnection, eYield);
			}
		}
	}
	return iYield;
}

int CvPlayerPolicies::GetTradeConnectionSeaYieldModifier(TradeConnectionType eTradeConnection, YieldTypes eYield) const
{
	int iYield = 0;
	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{

		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			CvPolicyEntry* pPolicy = m_pPolicies->GetPolicyEntry(i);
			if (pPolicy->GetTradeConnectionSeaYieldModifier(eTradeConnection, eYield) > 0)
			{
				iYield += pPolicy->GetTradeConnectionSeaYieldModifier(eTradeConnection, eYield);
			}
		}
	}
	return iYield;
}
#endif
#if defined(FULL_YIELD_FROM_KILLS)

int CvPlayerPolicies::GetYieldFromKills(YieldTypes eYield) const
{
	int iYield = 0;

	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{

		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			CvPolicyEntry* pPolicy = m_pPolicies->GetPolicyEntry(i);
			if (pPolicy->GetYieldFromKills(eYield) > 0)
			{
				iYield += pPolicy->GetYieldFromKills(eYield);
			}
		}
	}

	return iYield;
}

int CvPlayerPolicies::GetYieldFromKillsMax(YieldTypes eYield) const
{
	int iMax = 0;

	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{
		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			CvPolicyEntry* pPolicy = m_pPolicies->GetPolicyEntry(i);
			if (pPolicy->GetYieldFromKills(eYield) > 0)
			{
				const int iPolicyMax = pPolicy->GetYieldFromKillsMax(eYield);
				if (iPolicyMax > iMax)
					iMax = iPolicyMax;
			}
		}
	}

	return iMax;
}
#endif
#if defined(LEKMOD_v34)

int CvPlayerPolicies::GetPolicyResourceQuantity(ResourceTypes eResource) const
{
	int iQuantity = 0;

	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{

		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			CvPolicyEntry* pPolicy = m_pPolicies->GetPolicyEntry(i);
			if (pPolicy->GetPolicyResourceQuantity(eResource) > 0)
			{
				iQuantity += pPolicy->GetPolicyResourceQuantity(eResource);
			}
		}
	}
	
	return iQuantity;
}

int CvPlayerPolicies::GetPolicyResourceYieldChanges(ResourceTypes eResource, YieldTypes eYield) const
{
	int iYield = 0;
	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{

		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			CvPolicyEntry* pPolicy = m_pPolicies->GetPolicyEntry(i);
			if (pPolicy->GetPolicyResourceYieldChanges(eResource, eYield) > 0)
			{
				iYield += pPolicy->GetPolicyResourceYieldChanges(eResource, eYield);
			}
		}
	}
	return iYield;
}

int CvPlayerPolicies::GetPolicyResourceClassYieldChanges(ResourceClassTypes eResourceClass, YieldTypes eYield) const
{
	int iYield = 0;
	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{

		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			CvPolicyEntry* pPolicy = m_pPolicies->GetPolicyEntry(i);
			if (pPolicy->GetPolicyResourceClassYieldChanges(eResourceClass, eYield) > 0)
			{
				iYield += pPolicy->GetPolicyResourceClassYieldChanges(eResourceClass, eYield);
			}
		}
	}
	return iYield;
}
#endif
#if defined(LEKMOD_FIX_SCHOLASTICISM)
int CvPlayerPolicies::GetMinorFriendYieldBonus(EraTypes eEra, YieldTypes eYield) const
{
	int iYield = 0;
	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{

		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			CvPolicyEntry* pPolicy = m_pPolicies->GetPolicyEntry(i);
			if (pPolicy->GetMinorFriendYieldBonus(eEra, eYield) > 0)
			{
				iYield += pPolicy->GetMinorFriendYieldBonus(eEra, eYield);
			}
		}
	}
	return iYield;
}
int CvPlayerPolicies::GetMinorAllyYieldBonus(EraTypes eEra, YieldTypes eYield) const
{
	int iYield = 0;
	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{

		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			CvPolicyEntry* pPolicy = m_pPolicies->GetPolicyEntry(i);
			if (pPolicy->GetMinorAllyYieldBonus(eEra, eYield) > 0)
			{
				iYield += pPolicy->GetMinorAllyYieldBonus(eEra, eYield);
			}
		}
	}
	return iYield;
}
#endif
#if defined(LEKMOD_EXPERIMENTAL_CHANGES)
int CvPlayerPolicies::GetWorldWonderYieldChange(YieldTypes eYield) const
{
	int iYield = 0;
	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{

		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			CvPolicyEntry* pPolicy = m_pPolicies->GetPolicyEntry(i);
			if (pPolicy->GetWorldWonderYieldChange(eYield) > 0)
			{
				iYield += pPolicy->GetWorldWonderYieldChange(eYield);
			}
		}
	}
	return iYield;
}
#endif

int CvPlayerPolicies::GetNextPolicyCost()
{
	int iNumPolicies = GetNumPoliciesOwned();


	iNumPolicies -= (m_pPlayer->GetNumFreePoliciesEver() - m_pPlayer->GetNumFreePolicies() - m_pPlayer->GetNumFreeTenets());







	int iCost = 0;
	iCost += (iNumPolicies*       GC.getPOLICY_COST_INCREASE_TO_BE_EXPONENTED());


	iCost = (int) pow((double) iCost, (double)          GC.getPOLICY_COST_EXPONENT());


	iCost +=        GC.getBASE_POLICY_COST();


	int iMod = GC.getMap().getWorldInfo().GetNumCitiesPolicyCostMod();
	int iPolicyModDiscount = m_pPlayer->GetNumCitiesPolicyCostDiscount();
	if(iPolicyModDiscount != 0)
	{
		iMod = iMod * (100 + iPolicyModDiscount);
		iMod /= 100;
	}

	int iNumCities = m_pPlayer->GetMaxEffectiveCities();

	iMod = (iCost * (iNumCities - 1) * iMod);
	iMod /= 100;
	iCost += iMod;


	iCost *= (100 + m_pPlayer->getPolicyCostModifier());
	iCost /= 100;


	iCost *= GC.getGame().getGameSpeedInfo().getCulturePercent();
	iCost /= 100;


	iCost *= m_pPlayer->getHandicapInfo().getPolicyPercent();
	iCost /= 100;


	int iDivisor =       GC.getPOLICY_COST_VISIBLE_DIVISOR();
	iCost /= iDivisor;
	iCost *= iDivisor;

	return iCost;
}


bool CvPlayerPolicies::CanAdoptPolicy(PolicyTypes eIndex, bool bIgnoreCost) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumPolicyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	CvPolicyEntry* pkPolicyEntry = GC.getPolicyInfo(eIndex);
	if(pkPolicyEntry == NULL)
		return false;


	if(HasPolicy(eIndex))
	{
		return false;
	}


	if((!bIgnoreCost) && m_pPlayer->getNextPolicyCost() > 0)
	{
		if(m_pPlayer->getJONSCulture() < m_pPlayer->getNextPolicyCost())
		{
			bool bTenet = pkPolicyEntry->GetLevel() > 0;
			if (m_pPlayer->GetNumFreePolicies() == 0)
			{
				if (!bTenet || m_pPlayer->GetNumFreeTenets() == 0)
				{
					return false;
				}
			}
		}
	}

	PolicyBranchTypes eBranch = (PolicyBranchTypes) pkPolicyEntry->GetPolicyBranchType();


	if(eBranch == NO_POLICY_BRANCH_TYPE)
	{
		return false;
	}

	if(!IsPolicyBranchUnlocked(eBranch))
	{
		return false;
	}


	CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(eBranch);
	if(pkPolicyBranchInfo == NULL)
	{
		return false;
	}
	else
	{
		if (pkPolicyBranchInfo->IsPurchaseByLevel())
		{

			int iLevel = pkPolicyEntry->GetLevel();
			if (iLevel > 1)
			{
				int iPoliciesOfThisLevel = GetNumTenetsOfLevel(eBranch, iLevel) + 1                                       ;
				int iPoliciesOfPreviousLevel = GetNumTenetsOfLevel(eBranch, iLevel - 1);
				if (iPoliciesOfThisLevel >= iPoliciesOfPreviousLevel)
				{
					return false;
				}
			}
		}
	}



	bool bFoundPossible = false;
	bool bFoundValid = false;

	for(int iI = 0; iI < GC.getNUM_OR_TECH_PREREQS(); iI++)
	{
		PolicyTypes ePrereq = (PolicyTypes)pkPolicyEntry->GetPrereqOrPolicies(iI);
		if(ePrereq != NO_POLICY)
		{
			bFoundPossible = true;

			if(HasPolicy(ePrereq))
			{
				bFoundValid = true;
				break;
			}
		}
	}

	if(bFoundPossible && !bFoundValid)
	{
		return false;
	}

	for(int iI = 0; iI < GC.getNUM_AND_TECH_PREREQS(); iI++)
	{
		const PolicyTypes ePrereq = static_cast<PolicyTypes>(pkPolicyEntry->GetPrereqAndPolicies(iI));

		if(ePrereq == NO_POLICY)
			continue;

		CvPolicyEntry* pkPrereqPolicyInfo = GC.getPolicyInfo(ePrereq);
		if(pkPrereqPolicyInfo)
		{
			if(!HasPolicy(ePrereq))
			{
				return false;
			}
		}
	}


#ifdef AUI_WARNING_FIXES
	for (uint iPolicyLoop = 0; iPolicyLoop < GetPolicies()->GetNumPolicies(); iPolicyLoop++)
#else
	for(int iPolicyLoop = 0; iPolicyLoop < GetPolicies()->GetNumPolicies(); iPolicyLoop++)
#endif
	{
		const PolicyTypes eDisablePolicy =static_cast<PolicyTypes>(iPolicyLoop);

		CvPolicyEntry* pkDisablePolicyInfo = GC.getPolicyInfo(eDisablePolicy);
		if(pkDisablePolicyInfo)
		{
			if(HasPolicy(eDisablePolicy))
			{
				for(int iI = 0; iI < GC.getNUM_AND_TECH_PREREQS(); iI++)
				{
					if(pkDisablePolicyInfo->GetPolicyDisables(iI) == eIndex)
					{
						return false;
					}
				}
			}
		}
	}


	if(pkPolicyEntry->GetTechPrereq() != NO_TECH)
	{
		if(!GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech((TechTypes) pkPolicyEntry->GetTechPrereq()))
		{
			return false;
		}
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pPlayer->GetID());
		args->Push(eIndex);



		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "PlayerCanAdoptPolicy", args.get(), bResult))
		{

			if(bResult == false)
			{
				return false;
			}
		}
	}

	return true;
}


void CvPlayerPolicies::DoUnlockPolicyBranch(PolicyBranchTypes eBranchType)
{
	CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(eBranchType);
	if(pkPolicyBranchInfo == NULL)
	{
		return;
	}


	if(!CanUnlockPolicyBranch(eBranchType))
	{
		return;
	}


	SetPolicyBranchUnlocked(eBranchType, true, false);


	DoSwitchToPolicyBranch(eBranchType);


	PolicyTypes eFreePolicy = (PolicyTypes) pkPolicyBranchInfo->GetFreePolicy();
	if(eFreePolicy != NO_POLICY)
	{
		GetPlayer()->setHasPolicy(eFreePolicy, true);
	}


	if(GetPlayer()->GetNumFreePolicies() == 0)
	{
		GetPlayer()->changeJONSCulture(-GetPlayer()->getNextPolicyCost());
	}
	else
	{
		GetPlayer()->ChangeNumFreePolicies(-1);
	}


	GetPlayer()->DoUpdateNextPolicyCost();

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);


	if(GC.getGame().getActivePlayer() == GetPlayer()->GetID())
	{
		GC.GetEngineUserInterface()->setDirty(Policies_DIRTY_BIT, true);
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pPlayer->GetID());
		args->Push(eBranchType);



		bool bResult = false;
		LuaSupport::CallHook(pkScriptSystem, "PlayerAdoptPolicyBranch", args.get(), bResult);
	}
}


bool CvPlayerPolicies::CanUnlockPolicyBranch(PolicyBranchTypes eBranchType)
{

	if(GetPlayer()->getJONSCulture() < GetPlayer()->getNextPolicyCost())
	{
		if(GetPlayer()->GetNumFreePolicies() == 0)
			return false;
	}

	CvPolicyBranchEntry* pkBranchEntry = m_pPolicies->GetPolicyBranchEntry(eBranchType);
	if(pkBranchEntry)
	{

		if (pkBranchEntry->IsPurchaseByLevel())
		{
			return false;
		}

		if (pkBranchEntry->IsLockedWithoutReligion())
		{
			if (GC.getGame().isOption(GAMEOPTION_NO_RELIGION))
			{
				return false;
			}
		}

		EraTypes ePrereqEra = (EraTypes) pkBranchEntry->GetEraPrereq();


		if(ePrereqEra != NO_ERA)
		{
			if(GET_TEAM(GetPlayer()->getTeam()).GetCurrentEra() < ePrereqEra)
			{
				return false;
			}
		}
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pPlayer->GetID());
		args->Push(eBranchType);



		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "PlayerCanAdoptPolicyBranch", args.get(), bResult))
		{

			if(bResult == false)
			{
				return false;
			}
		}
	}

	return true;
}

#ifdef POLICY_BRANCH_UNLOCKING_TURN
int CvPlayerPolicies::PolicyBranchUnlockingTurn(PolicyBranchTypes eBranchType) const
{
	CvAssertMsg(eBranchType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBranchType < m_pPolicies->GetNumPolicyBranches(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiPolicyBranchUnlockingTurn[eBranchType];
}

#endif

bool CvPlayerPolicies::IsPolicyBranchUnlocked(PolicyBranchTypes eBranchType) const
{
	CvAssertMsg(eBranchType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBranchType < m_pPolicies->GetNumPolicyBranches(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabPolicyBranchUnlocked[eBranchType];
}


void CvPlayerPolicies::SetPolicyBranchUnlocked(PolicyBranchTypes eBranchType, bool bNewValue, bool bRevolution)
{
	CvAssertMsg(eBranchType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBranchType < m_pPolicies->GetNumPolicyBranches(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(IsPolicyBranchUnlocked(eBranchType) != bNewValue)
	{
#ifdef LEKMOD_NEW_LUA_METHODS
		m_pabHasPolicyBranch[eBranchType] = bNewValue;
#endif

		if (bNewValue)
		{
			int iFreePolicies = PolicyHelpers::GetNumFreePolicies(eBranchType);


			CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(eBranchType);
			if(pkPolicyBranchInfo != NULL)
			{
				if (pkPolicyBranchInfo->IsPurchaseByLevel())
				{
					m_pPlayer->ChangeNumFreeTenets(iFreePolicies, !bRevolution);

					for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
						PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
						CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);


						if (m_pPlayer->GetID() != eNotifyPlayer)
						{
							CvTeam& kNotifyTeam = GET_TEAM(kCurNotifyPlayer.getTeam());
							const bool bHasMet = kNotifyTeam.isHasMet(m_pPlayer->getTeam());

							CvNotifications* pNotifications = kCurNotifyPlayer.GetNotifications();
							if(pNotifications)
							{
								CvString strBuffer;
								if(bHasMet)
								{
									if (bRevolution)
										strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_IDEOLOGY_CHANGE", m_pPlayer->getCivilizationShortDescriptionKey(), pkPolicyBranchInfo->GetDescriptionKey());
									else
										strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_IDEOLOGY_CHOSEN", m_pPlayer->getCivilizationShortDescriptionKey(), pkPolicyBranchInfo->GetDescriptionKey());
								}
								else
								{
									if (bRevolution)
										strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_IDEOLOGY_CHANGE_UNMET", pkPolicyBranchInfo->GetDescriptionKey());
									else
										strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_IDEOLOGY_CHOSEN_UNMET", pkPolicyBranchInfo->GetDescriptionKey());
								}

								CvString strSummary;
								if (bRevolution)
									strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_IDEOLOGY_CHANGE");
								else
									strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_IDEOLOGY_CHOSEN");

								pNotifications->Add(NOTIFICATION_IDEOLOGY_CHOSEN, strBuffer, strSummary, -1, -1, m_pPlayer->GetID());
							}
						}
					}
				}
				else
				{
					m_pPlayer->ChangeNumFreePolicies(iFreePolicies);
				}
			}
		}

		m_pabPolicyBranchUnlocked[eBranchType] = bNewValue;
#ifdef POLICY_BRANCH_UNLOCKING_TURN
		m_paiPolicyBranchUnlockingTurn[eBranchType] = GC.getGame().getGameTurn();
#endif
#ifdef LEKMOD_NEW_LUA_EVENTS
		if (bNewValue)
		{
			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if (pkScriptSystem)
			{
				CvLuaArgsHandle args;
				args->Push(m_pPlayer->GetID());
				args->Push(eBranchType);



				bool bResult = false;
				LuaSupport::CallHook(pkScriptSystem, "PlayerPolicyBranchUnlocked", args.get(), bResult);
			}
		}
#endif
	}
}


int CvPlayerPolicies::GetNumPolicyBranchesUnlocked() const
{
	int iCount = 0;

#ifdef AUI_WARNING_FIXES
	for (uint iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
#else
	for(int iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
#endif
	{
		if(IsPolicyBranchUnlocked((PolicyBranchTypes) iBranchLoop))
		{
			iCount++;
		}
	}

	return iCount;
}
 

void CvPlayerPolicies::DoSwitchToPolicyBranch(PolicyBranchTypes eBranchType)
{

	if(!IsPolicyBranchUnlocked(eBranchType))
	{
		return;
	}


	if(IsPolicyBranchBlocked(eBranchType))
	{

		int iNumTurnsAnarchy =       GC.getSWITCH_POLICY_BRANCHES_ANARCHY_TURNS();
		GetPlayer()->ChangeAnarchyNumTurns(iNumTurnsAnarchy);


		SetPolicyBranchBlocked(eBranchType, false);
	}


#ifdef AUI_WARNING_FIXES
	uint iBranchLoop;
#else
	int iBranchLoop;
#endif
	for(iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
	{
		const PolicyBranchTypes eDisableBranch = static_cast<PolicyBranchTypes>(iBranchLoop);
		CvPolicyBranchEntry* pkDisablePolicyBranchInfo = GC.getPolicyBranchInfo(eBranchType);
		if(pkDisablePolicyBranchInfo)
		{
			if(pkDisablePolicyBranchInfo->GetPolicyBranchDisables(eDisableBranch) > 0)
			{
				SetPolicyBranchBlocked(eDisableBranch, true);
			}
		}

	}


	bool bUnlockBranch;


	for(iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
	{
		const PolicyBranchTypes eActivateBranch = static_cast<PolicyBranchTypes>(iBranchLoop);
		CvPolicyBranchEntry* pkActivateBranchInfo = GC.getPolicyBranchInfo(eActivateBranch);
		if(pkActivateBranchInfo)
		{

			if(!IsPolicyBranchUnlocked(eActivateBranch))
			{
				continue;
			}


			if(!IsPolicyBranchBlocked(eActivateBranch))
			{
				continue;
			}


			bUnlockBranch = true;


#ifdef AUI_WARNING_FIXES
			for (uint iOtherBranchLoop = 0; iOtherBranchLoop < m_pPolicies->GetNumPolicyBranches(); iOtherBranchLoop++)
#else
			for(int iOtherBranchLoop = 0; iOtherBranchLoop < m_pPolicies->GetNumPolicyBranches(); iOtherBranchLoop++)
#endif
			{
				const PolicyBranchTypes eOtherBranch = static_cast<PolicyBranchTypes>(iOtherBranchLoop);
				CvPolicyBranchEntry* pkOtherPolicyBranchInfo = GC.getPolicyBranchInfo(eOtherBranch);
				if(pkOtherPolicyBranchInfo)
				{

					if(eActivateBranch != eOtherBranch)
					{

						if(IsPolicyBranchUnlocked(eOtherBranch))
						{
							if(!IsPolicyBranchBlocked(eOtherBranch))
							{

								if(pkActivateBranchInfo->GetPolicyBranchDisables(eOtherBranch) > 0)
								{
									bUnlockBranch = false;
								}

								if(pkOtherPolicyBranchInfo->GetPolicyBranchDisables(eActivateBranch) > 0)
								{
									bUnlockBranch = false;
								}
							}
						}


						if(!bUnlockBranch)
						{
							break;
						}
					}
				}
			}


			if(bUnlockBranch)
			{
				SetPolicyBranchBlocked(eActivateBranch, false);
			}
		}
	}


	if(GC.getGame().getActivePlayer() == GetPlayer()->GetID())
	{
		GC.GetEngineUserInterface()->setDirty(Policies_DIRTY_BIT, true);
	}
}


void CvPlayerPolicies::SetPolicyBranchBlocked(PolicyBranchTypes eBranchType, bool bValue)
{
	CvAssertMsg(eBranchType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBranchType < m_pPolicies->GetNumPolicyBranches(), "eIndex is expected to be within maximum bounds (invalid Index)");

	CvPolicyBranchEntry* pkPolicyBranchEntry = GC.getPolicyBranchInfo(eBranchType);
	if(pkPolicyBranchEntry)
	{
		if(bValue != IsPolicyBranchBlocked(eBranchType))
		{
			m_pabPolicyBranchBlocked[eBranchType] = bValue;

			int iPolicyEffectChange = bValue ? -1 : 1;

			if(iPolicyEffectChange != 0)
			{

#ifdef AUI_WARNING_FIXES
				for (uint iPolicyLoop = 0; iPolicyLoop < GetPolicies()->GetNumPolicies(); iPolicyLoop++)
#else
				for(int iPolicyLoop = 0; iPolicyLoop < GetPolicies()->GetNumPolicies(); iPolicyLoop++)
#endif
				{
					const PolicyTypes ePolicy = static_cast<PolicyTypes>(iPolicyLoop);
					CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(ePolicy);
					if(pkPolicyInfo)
					{
						if(eBranchType == (PolicyBranchTypes) pkPolicyInfo->GetPolicyBranchType() ||
						        pkPolicyBranchEntry->GetFreePolicy() == ePolicy ||
						        pkPolicyBranchEntry->GetFreeFinishingPolicy() == ePolicy)
						{



							if(HasPolicy(ePolicy))
							{
								GetPlayer()->processPolicies(ePolicy, iPolicyEffectChange);
							}
						}
					}
				}
			}
		}
	}
}


bool CvPlayerPolicies::IsPolicyBranchBlocked(PolicyBranchTypes eBranchType) const
{
	CvAssertMsg(eBranchType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBranchType < m_pPolicies->GetNumPolicyBranches(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabPolicyBranchBlocked[eBranchType];
}


bool CvPlayerPolicies::IsPolicyBlocked(PolicyTypes eType) const
{
	CvAssertMsg(eType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eType < m_pPolicies->GetNumPolicies(), "eIndex is expected to be within maximum bounds (invalid Index)");


	PolicyBranchTypes eBranch = m_paePolicyBlockedBranchCheck[eType];
	if (eBranch == NO_POLICY_BRANCH_TYPE)
		return false;

	return IsPolicyBranchBlocked(eBranch);
}


void CvPlayerPolicies::DoSwitchIdeologies(PolicyBranchTypes eNewBranchType)
{
	PolicyBranchTypes eOldBranchType = GetLateGamePolicyTree();
	CvAssertMsg (eOldBranchType != eNewBranchType && eNewBranchType != NO_POLICY_BRANCH_TYPE && eOldBranchType != NO_POLICY_BRANCH_TYPE, "Illegal time for Ideology change");

	int iOldBranchTenets = GetNumPoliciesOwnedInBranch(eOldBranchType);
	int iNewBranchTenets = max(0, iOldBranchTenets - GC.getSWITCH_POLICY_BRANCHES_TENETS_LOST());

	ClearPolicyBranch(eOldBranchType);
	SetPolicyBranchUnlocked(eOldBranchType, false, false);

	SetPolicyBranchUnlocked(eNewBranchType, true, true                );
	m_pPlayer->GetCulture()->DoPublicOpinion();
	m_pPlayer->GetCulture()->SetTurnIdeologySwitch(GC.getGame().getGameTurn());

	m_pPlayer->ChangeNumFreeTenets(iNewBranchTenets, false                         );

	if (GC.getGame().getActivePlayer() == m_pPlayer->GetID())
	{
		DLLUI->setDirty(Policies_DIRTY_BIT, true);
	}
}


void CvPlayerPolicies::ClearPolicyBranch(PolicyBranchTypes eBranchType)
{

#ifdef AUI_WARNING_FIXES
	for (uint iPolicyLoop = 0; iPolicyLoop < GetPolicies()->GetNumPolicies(); iPolicyLoop++)
#else
	for(int iPolicyLoop = 0; iPolicyLoop < GetPolicies()->GetNumPolicies(); iPolicyLoop++)
#endif
	{
		const PolicyTypes eLoopPolicy = static_cast<PolicyTypes>(iPolicyLoop);
		CvPolicyEntry* pkLoopPolicyInfo = GC.getPolicyInfo(eLoopPolicy);
		if(pkLoopPolicyInfo)
		{
			PolicyBranchTypes eLoopBranch = (PolicyBranchTypes)pkLoopPolicyInfo->GetPolicyBranchType();
			if (eLoopBranch == eBranchType)
			{
				m_pPlayer->setHasPolicy(eLoopPolicy, false);
			}
		}
	}
}


int CvPlayerPolicies::GetNumPolicyBranchesFinished() const
{
	int iNumBranchesFinished = 0;

	PolicyBranchTypes eLoopBranch;
#ifdef AUI_WARNING_FIXES
	for (uint iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
#else
	for(int iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
#endif
	{
		eLoopBranch = (PolicyBranchTypes) iBranchLoop;

		if(IsPolicyBranchFinished(eLoopBranch))
		{
			iNumBranchesFinished++;
		}
	}

	return iNumBranchesFinished;
}


void CvPlayerPolicies::SetPolicyBranchFinished(PolicyBranchTypes eBranchType, bool bValue)
{
	CvAssertMsg(eBranchType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBranchType < m_pPolicies->GetNumPolicyBranches(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(bValue != IsPolicyBranchFinished(eBranchType))
	{
		m_pabPolicyBranchFinished[eBranchType] = bValue;


		bool bUsingXP1Scenario3 = gDLL->IsModActivated(CIV5_XP1_SCENARIO3_MODID);


		if(!GC.getGame().isGameMultiPlayer() && GET_PLAYER(GC.getGame().getActivePlayer()).isHuman())
		{

			if(m_pPlayer->GetID() == GC.getGame().getActivePlayer() && !bUsingXP1Scenario3)
			{
#ifdef AUI_WARNING_FIXES
				switch (static_cast<int>(eBranchType))
#else
				switch(eBranchType)
#endif
				{
				case 0:
					gDLL->UnlockAchievement(ACHIEVEMENT_POLICY_TRADITION);
					break;
				case 1:
					gDLL->UnlockAchievement(ACHIEVEMENT_POLICY_LIBERTY);
					break;
				case 2:
					gDLL->UnlockAchievement(ACHIEVEMENT_POLICY_HONOR);
					break;
				case 3:
					gDLL->UnlockAchievement(ACHIEVEMENT_POLICY_PIETY);
					break;
				case 4:
					gDLL->UnlockAchievement(ACHIEVEMENT_POLICY_PATRONAGE);
					break;
				
				case 5:
					gDLL->UnlockAchievement(ACHIEVEMENT_XP2_48);
					break;
				case 6:
					gDLL->UnlockAchievement(ACHIEVEMENT_POLICY_COMMERCE);
					break;
				case 7:
					gDLL->UnlockAchievement(ACHIEVEMENT_XP2_47);
					break;
				case 8:
					gDLL->UnlockAchievement(ACHIEVEMENT_POLICY_RATIONALISM);
					break;
				}
			}
			if(gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_TRADITION) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_HONOR) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_LIBERTY) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_PIETY) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_PATRONAGE) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_ORDER) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_AUTOCRACY)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_FREEDOM)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_COMMERCE)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_RATIONALISM))
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_ALL_SOCIAL_POLICIES);
			}
		}

	}
}


bool CvPlayerPolicies::IsPolicyBranchFinished(PolicyBranchTypes eBranchType) const
{
	CvAssertMsg(eBranchType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBranchType < m_pPolicies->GetNumPolicyBranches(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabPolicyBranchFinished[eBranchType];
}


bool CvPlayerPolicies::WillFinishBranchIfAdopted(PolicyTypes eType) const
{
	PolicyBranchTypes eBranchType = (PolicyBranchTypes)m_pPolicies->GetPolicyEntry(eType)->GetPolicyBranchType();

	if(eBranchType != NO_POLICY_BRANCH_TYPE)
	{

#ifdef AUI_WARNING_FIXES
		for (uint iPolicyLoop = 0; iPolicyLoop < GetPolicies()->GetNumPolicies(); iPolicyLoop++)
#else
		for(int iPolicyLoop = 0; iPolicyLoop < GetPolicies()->GetNumPolicies(); iPolicyLoop++)
#endif
		{
			const PolicyTypes eLoopPolicy = static_cast<PolicyTypes>(iPolicyLoop);

			CvPolicyEntry* pkLoopPolicyInfo = GC.getPolicyInfo(eLoopPolicy);
			if(pkLoopPolicyInfo)
			{

				if(pkLoopPolicyInfo->GetPolicyBranchType() == eBranchType)
				{

					if(!HasPolicy(eLoopPolicy))
					{

						if(eLoopPolicy != eType)
						{

							return false;
						}
					}
				}
			}
		}


		return true;
	}

	return false;
}


PolicyBranchTypes CvPlayerPolicies::GetPolicyBranchChosen(int iID) const
{
	if(iID < GetNumPolicyBranchesAllowed())
	{
		return m_paePolicyBranchesChosen[iID];
	}

	FAssert(false);

	return NO_POLICY_BRANCH_TYPE;
}


void CvPlayerPolicies::SetPolicyBranchChosen(int iID, PolicyBranchTypes eBranchType)
{
	FAssert(eBranchType > -1);
	FAssert(eBranchType < m_pPolicies->GetNumPolicyBranches());

	if(iID < GetNumPolicyBranchesAllowed())
	{
		m_paePolicyBranchesChosen[iID] = eBranchType;
	}
	else
	{
		FAssert(false);
	}
}


int CvPlayerPolicies::GetNumPolicyBranchesAllowed() const
{
	return       GC.getNUM_POLICY_BRANCHES_ALLOWED() + GetNumExtraBranches();
}


int CvPlayerPolicies::GetNumExtraBranches() const
{
	return m_iNumExtraBranches;
}


void CvPlayerPolicies::ChangeNumExtraBranches(int iChange)
{
	m_iNumExtraBranches += iChange;
}


int CvPlayerPolicies::GetNumPoliciesCanBeAdopted()
{
	int iNumPoliciesToAcquire = 0;


#ifdef AUI_WARNING_FIXES
	for (uint iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
#else
	for(int iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
#endif
	{
		PolicyBranchTypes eBranchType = (PolicyBranchTypes)iBranchLoop;
		if (IsPolicyBranchUnlocked(eBranchType) || CanUnlockPolicyBranch(eBranchType))
		{
			CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(eBranchType);
			if(pkPolicyBranchInfo)
			{

				if(pkPolicyBranchInfo->GetFreePolicy() != NO_POLICY)
				{
					iNumPoliciesToAcquire++;
				}
			}
		}
	}


#ifdef AUI_WARNING_FIXES
	for (uint iPolicyLoop = 0; iPolicyLoop < GetPolicies()->GetNumPolicies(); iPolicyLoop++)
#else
	for(int iPolicyLoop = 0; iPolicyLoop < GetPolicies()->GetNumPolicies(); iPolicyLoop++)
#endif
	{
		const PolicyTypes eLoopPolicy = static_cast<PolicyTypes>(iPolicyLoop);
		CvPolicyEntry* pkLoopPolicyInfo = GC.getPolicyInfo(eLoopPolicy);
		if(pkLoopPolicyInfo)
		{
			PolicyBranchTypes eBranchType = (PolicyBranchTypes)pkLoopPolicyInfo->GetPolicyBranchType();
			if (eBranchType != -1)
			{
				if (IsPolicyBranchUnlocked(eBranchType) || CanUnlockPolicyBranch(eBranchType))
				{
					iNumPoliciesToAcquire++;
				}
			}
		}
	}

	return iNumPoliciesToAcquire - GetNumPoliciesOwned();
}


void CvPlayerPolicies::DoNewPolicyPickedForHistory(PolicyTypes ePolicy)
{
	CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(ePolicy);
	if(pkPolicyInfo == NULL)
		return;

	PolicyBranchTypes eNewBranch = (PolicyBranchTypes) pkPolicyInfo->GetPolicyBranchType();


	if(eNewBranch == NO_POLICY_BRANCH_TYPE)
	{
#ifdef AUI_WARNING_FIXES
		for (uint iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
#else
		for(int iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
#endif
		{
			const PolicyBranchTypes eLoopBranch = static_cast<PolicyBranchTypes>(iBranchLoop);
			CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(eLoopBranch);
			if(pkPolicyBranchInfo)
			{

				if(pkPolicyBranchInfo->GetFreePolicy() == ePolicy)
				{
					eNewBranch = eLoopBranch;
					break;
				}
			}
		}
	}


	if(GetBranchPicked1() == NO_POLICY_BRANCH_TYPE)
	{
		SetBranchPicked1(eNewBranch);
	}
	else if(GetBranchPicked2() == NO_POLICY_BRANCH_TYPE)
	{
		SetBranchPicked2(eNewBranch);
	}
	else if(GetBranchPicked3() == NO_POLICY_BRANCH_TYPE)
	{
		SetBranchPicked3(eNewBranch);
	}


	else
	{
		SetBranchPicked3(GetBranchPicked2());
		SetBranchPicked2(GetBranchPicked1());
		SetBranchPicked1(eNewBranch);
	}
}


PolicyBranchTypes CvPlayerPolicies::GetDominantPolicyBranchForTitle() const
{

	if(GetBranchPicked1() == GetBranchPicked2() &&
	        GetBranchPicked1() == GetBranchPicked3() &&
	        GetBranchPicked2() == GetBranchPicked3())
	{
		return GetBranchPicked1();
	}



	PolicyBranchTypes eTempBranch;

	std::vector<int> viPolicyBranchCounts;


#ifdef AUI_WARNING_FIXES
	uint iBranchLoop;
#else
	int iBranchLoop;
#endif
	for(iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
	{
		viPolicyBranchCounts.push_back(0);
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
			if(HasPolicy(eLoopPolicy))
			{
				eTempBranch = (PolicyBranchTypes) pkPolicyInfo->GetPolicyBranchType();


				if(eTempBranch == NO_POLICY_BRANCH_TYPE)
				{
					for(iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
					{
						const PolicyBranchTypes eLoopBranch = static_cast<PolicyBranchTypes>(iBranchLoop);
						CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(eLoopBranch);
						if(pkPolicyBranchInfo)
						{

							if(pkPolicyBranchInfo->GetFreePolicy() == eLoopPolicy)
							{
								eTempBranch = eLoopBranch;
								break;
							}
						}
					}
				}

				if(eTempBranch != NO_POLICY_BRANCH_TYPE)
				{
					viPolicyBranchCounts[eTempBranch]++;
				}
			}
		}
	}


	PolicyBranchTypes eBestBranch = NO_POLICY_BRANCH_TYPE;
	int iBestValue = 0;

	for(iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
	{
		if(viPolicyBranchCounts[iBranchLoop] > iBestValue)
		{
			eBestBranch = (PolicyBranchTypes) iBranchLoop;
			iBestValue = viPolicyBranchCounts[iBranchLoop];
		}
	}

	return eBestBranch;
}


PolicyBranchTypes CvPlayerPolicies::GetBranchPicked1() const
{
	return m_eBranchPicked1;
}


void CvPlayerPolicies::SetBranchPicked1(PolicyBranchTypes eBranch)
{
	m_eBranchPicked1 = eBranch;
}


PolicyBranchTypes CvPlayerPolicies::GetBranchPicked2() const
{
	return m_eBranchPicked2;
}


void CvPlayerPolicies::SetBranchPicked2(PolicyBranchTypes eBranch)
{
	m_eBranchPicked2 = eBranch;
}


PolicyBranchTypes CvPlayerPolicies::GetBranchPicked3() const
{
	return m_eBranchPicked3;
}


void CvPlayerPolicies::SetBranchPicked3(PolicyBranchTypes eBranch)
{
	m_eBranchPicked3 = eBranch;
}




PolicyBranchTypes CvPlayerPolicies::GetLateGamePolicyTree() const
{
	PolicyBranchTypes eOurChoice = NO_POLICY_BRANCH_TYPE;

	PolicyBranchTypes eLoopBranch;
#ifdef AUI_WARNING_FIXES
	for (uint iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
#else
	for(int iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
#endif
	{
		eLoopBranch = (PolicyBranchTypes) iBranchLoop;

		CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(eLoopBranch);
		if(pkPolicyBranchInfo)
		{
			if (pkPolicyBranchInfo->IsPurchaseByLevel() && IsPolicyBranchUnlocked(eLoopBranch))
			{
				eOurChoice = eLoopBranch;
				break;
			}
		}
	}

	return eOurChoice;
}


bool CvPlayerPolicies::IsTimeToChooseIdeology() const
{
	PolicyBranchTypes eFreedomBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_FREEDOM();
	PolicyBranchTypes eAutocracyBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_AUTOCRACY();
	PolicyBranchTypes eOrderBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_ORDER();
#ifdef AI_CANT_ADOPT_IDEOLOGY
	if (!m_pPlayer->isHuman() && GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_CULTURE"))
	{
		return false;
	}
#endif
	if (eFreedomBranch == NO_POLICY_BRANCH_TYPE || eAutocracyBranch == NO_POLICY_BRANCH_TYPE || eOrderBranch == NO_POLICY_BRANCH_TYPE)
	{
		return false;
	}

	if (m_pPlayer->GetCurrentEra() > GC.getInfoTypeForString("ERA_INDUSTRIAL"))
	{
		return true;
	}


	else
	{
		CvBuildingXMLEntries* pkGameBuildings = GC.GetGameBuildings();
		CvCivilizationInfo* pkInfo = GC.getCivilizationInfo(m_pPlayer->getCivilizationType());
		if(pkInfo)
		{


#ifdef AUI_WARNING_FIXES
			for (uint iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#else
			for(int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#endif
			{
				const BuildingTypes eBuilding = static_cast<BuildingTypes>(pkInfo->getCivilizationBuildings(iI));
				CvBuildingEntry* pkBuildingInfo = NULL;
				if(eBuilding != -1)
				{
					pkBuildingInfo = pkGameBuildings->GetEntry(eBuilding);
					if (pkBuildingInfo)
					{
						int iIdeologyTriggerCount = pkBuildingInfo->GetXBuiltTriggersIdeologyChoice();
						if (iIdeologyTriggerCount > 0)
						{

							if (m_pPlayer->getBuildingClassCount((BuildingClassTypes)iI) >= iIdeologyTriggerCount)
							{
								return true;
							}
#ifdef LEKMOD_UNLOCK_IDEO_ALL_CITIES
							else if (m_pPlayer->getNumCities() < iIdeologyTriggerCount)
							{
								if (m_pPlayer->getBuildingClassCount((BuildingClassTypes)iI) >= m_pPlayer->getNumCities())
								{
									return true;
								}
							}
#endif
						}
					}
				}
			}
		}
	}

	return false;
}


std::vector<PolicyTypes> CvPlayerPolicies::GetAvailableTenets(PolicyBranchTypes eBranch, int iLevel)
{
	std::vector<PolicyTypes> availableTenets;

	CvPolicyXMLEntries* pkPolicies = GC.GetGamePolicies();
#ifdef AUI_WARNING_FIXES
	const uint iNumPolicies = pkPolicies->GetNumPolicies();

	availableTenets.reserve(iNumPolicies);
	for (uint iI = 0; iI < iNumPolicies; iI++)
#else
	const int iNumPolicies = pkPolicies->GetNumPolicies();

	availableTenets.reserve(iNumPolicies);
	for(int iI = 0; iI < iNumPolicies; iI++)
#endif
	{
		const PolicyTypes eTenet(static_cast<PolicyTypes>(iI));
		CvPolicyEntry* pEntry = pkPolicies->GetPolicyEntry(eTenet);
		if (pEntry && pEntry->GetPolicyBranchType() == eBranch && pEntry->GetLevel() == iLevel && !HasPolicy(eTenet))
		{
			availableTenets.push_back(eTenet);
		}
	}

	return availableTenets;
}


PolicyTypes CvPlayerPolicies::GetTenet(PolicyBranchTypes eBranch, int iLevel, int iIndex)
{
	int iNumFound = 0;

	CvPolicyXMLEntries* pkPolicies = GC.GetGamePolicies();
#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < pkPolicies->GetNumPolicies(); iI++)
#else
	const int iNumPolicies = pkPolicies->GetNumPolicies();

	for(int iI = 0; iI < iNumPolicies; iI++)
#endif
	{
		const PolicyTypes eTenet(static_cast<PolicyTypes>(iI));
		CvPolicyEntry* pEntry = pkPolicies->GetPolicyEntry(eTenet);
		if (pEntry && pEntry->GetPolicyBranchType() == eBranch && pEntry->GetLevel() == iLevel && HasPolicy(eTenet))
		{
			iNumFound++;
			if (iNumFound == iIndex)
			{
				return eTenet;
			}
		}
	}

	return NO_POLICY;
}


int CvPlayerPolicies::GetNumTenetsOfLevel(PolicyBranchTypes eBranch, int iLevel) const
{
	int iNumFound = 0;

	CvPolicyXMLEntries* pkPolicies = GC.GetGamePolicies();
#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < pkPolicies->GetNumPolicies(); iI++)
#else
	const int iNumPolicies = pkPolicies->GetNumPolicies();

	for(int iI = 0; iI < iNumPolicies; iI++)
#endif
	{
		const PolicyTypes eTenet(static_cast<PolicyTypes>(iI));
		CvPolicyEntry* pEntry = pkPolicies->GetPolicyEntry(eTenet);
		if (pEntry && pEntry->GetPolicyBranchType() == eBranch && pEntry->GetLevel() == iLevel && HasPolicy(eTenet))
		{
			iNumFound++;
		}
	}

	return iNumFound;
}


bool CvPlayerPolicies::CanGetAdvancedTenet() const
{
	PolicyBranchTypes eIdeology = GetLateGamePolicyTree();
	if (eIdeology == NO_POLICY_BRANCH_TYPE)
	{
		return false;
	}
		
	CvPolicyXMLEntries* pkPolicies = GC.GetGamePolicies();
#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumPolicyInfos(); iI++)
#else
	for(int iI = 0; iI < GC.getNumPolicyInfos(); iI++)
#endif
	{
		const PolicyTypes eTenet(static_cast<PolicyTypes>(iI));
		CvPolicyEntry* pEntry = pkPolicies->GetPolicyEntry(eTenet);
		if (pEntry && pEntry->GetPolicyBranchType() == eIdeology && pEntry->GetLevel() > 1 && !HasPolicy(eTenet) && CanAdoptPolicy(eTenet))
		{
			return true;
		}
	}

	return false;
}




void CvPlayerPolicies::DoPolicyAI()
{
	CvString strBuffer;

	m_pPolicyAI->DoConsiderIdeologySwitch(m_pPlayer);


	if(m_pPlayer->getNextPolicyCost() > 0)
	{

		while(m_pPlayer->getJONSCulture() >= m_pPlayer->getNextPolicyCost() || m_pPlayer->GetNumFreePolicies() > 0 || m_pPlayer->GetNumFreeTenets() > 0)
		{

#ifdef AUI_WARNING_FIXES
			uint iNextPolicy = m_pPolicyAI->ChooseNextPolicy(m_pPlayer);
			if (iNextPolicy == (uint)NO_POLICY)
#else
			int iNextPolicy = m_pPolicyAI->ChooseNextPolicy(m_pPlayer);
			if (iNextPolicy == NO_POLICY)
#endif
				break;


			if(iNextPolicy < m_pPolicies->GetNumPolicyBranches())
			{
				m_pPlayer->GetPlayerPolicies()->DoUnlockPolicyBranch((PolicyBranchTypes)iNextPolicy);
			}
			else
			{
				m_pPlayer->doAdoptPolicy((PolicyTypes)(iNextPolicy - m_pPolicies->GetNumPolicyBranches()));
			}
		}
	}
}


void CvPlayerPolicies::DoChooseIdeology()
{
	m_pPolicyAI->DoChooseIdeology(m_pPlayer);
}




void CvPlayerPolicies::AddFlavorAsStrategies(int iPropagatePercent)
{
	int iFlavorValue;


	m_pPolicyAI->Reset();


	for(int iFlavor = 0; iFlavor < GC.getNumFlavorTypes(); iFlavor++)
	{





		iFlavorValue = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes) iFlavor);


		

		EconomicAIStrategyTypes eStrategyLosingMoney = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY", true);
		if (eStrategyLosingMoney == NO_ECONOMICAISTRATEGY)
		{
			continue;
		}
		CvTeam& kTeam = GET_TEAM(m_pPlayer->getTeam());
		bool bIsAtWarWithSomeone = (kTeam.getAtWarCount(false) > 0);
		bool bInDeficit = m_pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyLosingMoney);

		if(bInDeficit && iFlavor == GC.getInfoTypeForString("FLAVOR_GOLD"))
		{
			iFlavorValue += 5;
		}
		else if(m_pPlayer->GetHappiness() < m_pPlayer->GetUnhappiness() && iFlavor == GC.getInfoTypeForString("FLAVOR_HAPPINESS"))
		{
			iFlavorValue += 5;
		}
		else if(bIsAtWarWithSomeone && iFlavor == GC.getInfoTypeForString("FLAVOR_DEFENSE"))
		{
			iFlavorValue += 3;
		}
		else if(bIsAtWarWithSomeone && iFlavor == GC.getInfoTypeForString("FLAVOR_CITY_DEFENSE"))
		{
			iFlavorValue += 3;
		}

		if(iFlavorValue > 0)
		{
			m_pPolicyAI->AddFlavorWeights((FlavorTypes)iFlavor, iFlavorValue, iPropagatePercent);
		}
	}
}

void CvPlayerPolicies::LogFlavors(FlavorTypes)
{
	return;
}



int PolicyHelpers::GetNumPlayersWithBranchUnlocked(PolicyBranchTypes eBranch)
{
	int iRtnValue = 0;

	for (int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		CvPlayer &kPlayer = GET_PLAYER((PlayerTypes)iI);

		if (kPlayer.isAlive() && !kPlayer.isMinorCiv() && !kPlayer.isBarbarian())
		{
#ifdef POLICY_BRANCH_UNLOCKING_TURN
			if (kPlayer.GetPlayerPolicies()->IsPolicyBranchUnlocked(eBranch) && kPlayer.GetPlayerPolicies()->PolicyBranchUnlockingTurn(eBranch) < GC.getGame().getGameTurn())
#else
			if (kPlayer.GetPlayerPolicies()->IsPolicyBranchUnlocked(eBranch))
#endif
			{
				iRtnValue++;
			}
		}
	}

	return iRtnValue;
}

int PolicyHelpers::GetNumFreePolicies(PolicyBranchTypes eBranch)
{
	int iFreePolicies = 0;

	CvPolicyBranchEntry *pkEntry = GC.getPolicyBranchInfo(eBranch);
	if (pkEntry)
	{
		if (pkEntry->GetEraPrereq() >= GC.getGame().getStartEra())
		{
			int iNumPreviousUnlockers = PolicyHelpers::GetNumPlayersWithBranchUnlocked(eBranch);
			if (iNumPreviousUnlockers == 0)
			{
				iFreePolicies = pkEntry->GetFirstAdopterFreePolicies();
			}
			else if (iNumPreviousUnlockers == 1)
			{
				iFreePolicies = pkEntry->GetSecondAdopterFreePolicies();
			}
		}
	}

	return iFreePolicies;
}

#ifdef LEKMOD_POLICIES_GLOBAL_MOVE_CHANGE

int CvPolicyEntry::GetGlobalMoveChange() const
{
	return m_iGlobalMoveChange;
}


int CvPolicyEntry::GetGlobalMoveChangeFriendly() const
{
	return m_iGlobalMoveChangeFriendly;
}


int CvPolicyEntry::GetGlobalMoveChangeEnemy() const
{
	return m_iGlobalMoveChangeEnemy;
}
#endif
