






#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvInfosSerializationHelper.h"
#include "CvDiplomacyAI.h"
#include "CvGrandStrategyAI.h"

#include "LintFree.h"





CvTraitEntry::CvTraitEntry() :
	m_iLevelExperienceModifier(0),
	m_iGreatPeopleRateModifier(0),
	m_iGreatScientistRateModifier(0),
	m_iGreatGeneralRateModifier(0),
	m_iGreatGeneralExtraBonus(0),
	m_iGreatPersonGiftInfluence(0),
	m_iMaxGlobalBuildingProductionModifier(0),
	m_iMaxTeamBuildingProductionModifier(0),
	m_iMaxPlayerBuildingProductionModifier(0),
	m_iCityUnhappinessModifier(0),
	m_iPopulationUnhappinessModifier(0),
	m_iCityStateBonusModifier(0),
	m_iCityStateFriendshipModifier(0),
#ifdef LEKMOD_TRAIT_FIRST_PROPHET_COST_MOD
	m_iFirstProphetCostMod(0),
#endif
	m_iCityStateCombatModifier(0),
	m_iLandBarbarianConversionPercent(0),
	m_iLandBarbarianConversionExtraUnits(0),
	m_iSeaBarbarianConversionPercent(0),
	m_iCapitalBuildingModifier(0),
	m_iPlotBuyCostModifier(0),
	m_iPlotCultureCostModifier(0),
#ifdef NQ_SCIENCE_PER_GREAT_PERSON_BORN
	m_iSciencePerGreatPersonBorn(0),
#endif
#ifdef NQ_NUM_TURNS_BEFORE_MINOR_ALLIES_REFUSE_BRIBES_FROM_TRAIT
	m_iNumTurnsBeforeMinorAlliesRefuseBribes(0),
#endif
	m_iCultureFromKills(0),
	m_iFaithFromKills(0),
	m_iCityCultureBonus(0),
	m_iCapitalThemingBonusModifier(0),
	m_iPolicyCostModifier(0),
	m_iCityConnectionTradeRouteChange(0),
	m_iWonderProductionModifier(0),
	m_iPlunderModifier(0),
	m_iImprovementMaintenanceModifier(0),
	m_iGoldenAgeDurationModifier(0),
	m_iGoldenAgeMoveChange(0),
	m_iGoldenAgeCombatModifier(0),
	m_iGoldenAgeTourismModifier(0),
	m_iGoldenAgeGreatArtistRateModifier(0),
	m_iGoldenAgeGreatMusicianRateModifier(0),
	m_iGoldenAgeGreatWriterRateModifier(0),
	m_iObsoleteTech(NO_TECH),
	m_iPrereqTech(NO_TECH),
	m_iExtraEmbarkMoves(0),
	m_iFreeUnitClassType(NO_UNITCLASS),
	m_iNaturalWonderFirstFinderGold(0),
	m_iNaturalWonderSubsequentFinderGold(0),

#if defined(TRAITIFY)
	m_bHalfMoreSpecialistUnhappiness(false),

	m_iNumExtraLeagueVotes(0),
	m_iNumTradeRouteBonus(0),
	m_iMinorFriendshipMinimum(0),
	m_iGreatEngineerRateModifier(0),
	m_iGreatMerchantRateModifier(0),
	m_iMinorBullyModifier(0),
	m_iExtraPopulation(0),
	m_iInternationalRouteGrowthModifier(0),
	m_iLocalHappinessPerCity(0),
	m_iGlobalHappinessPerCity(0),
	m_iUnhappinessModifierForPuppetedCities(0),
	m_iFaithCostModifier(0),
	m_iIdeologyPressureUnhappinessModifier(0),
	m_iForeignRelgionPressureModifier(0),
	m_iFriendlyLandsCitizenMoveChange(0),
#endif
#if defined(v35_TRAITIFY)
	m_bEmbarkedUnitsFullStrength(false),
	m_iCityStateUnitGiftExtraExperience(0),
	m_iGreatGeneralSiegeBonus(0),
#endif
#if defined(LEKMOD_v34)
	m_bReligionEnhanceReformation(false),

	m_iSelfReligiousPressureModifier(0),
#endif


	m_iNaturalWonderFirstFinderFaith(0),
	m_iNaturalWonderSubsequentFinderFaith(0),

	m_iNaturalWonderYieldModifier(0),
	m_iNaturalWonderHappinessModifier(0),
	m_iNearbyImprovementCombatBonus(0),
	m_iNearbyImprovementBonusRange(0),
	m_iCultureBuildingYieldChange(0),
	m_iCombatBonusVsHigherTech(0),
	m_iCombatBonusVsLargerCiv(0),
	m_iLandUnitMaintenanceModifier(0),
	m_iNavalUnitMaintenanceModifier(0),
	m_iRazeSpeedModifier(0),
	m_iDOFGreatPersonModifier(0),
	m_iLuxuryHappinessRetention(0),
	m_iExtraHappinessPerLuxury(0),
	m_iExtraSpies(0),
	m_iHappinessPerReligion(0),
	m_iUnresearchedTechBonusFromKills(0),
	m_iExtraFoundedCityTerritoryClaimRange(0),
	m_iFreeSocialPoliciesPerEra(0),
	m_iNumTradeRoutesModifier(0),
	m_iTradeRouteResourceModifier(0),
	m_iUniqueLuxuryCities(0),
	m_iUniqueLuxuryQuantity(0),
#if defined(LEKMOD_CITY_YIELDS_TRAITS)
	m_eYieldSettleUnit(NO_UNIT),
#endif
	m_eFreeUnitPrereqTech(NO_TECH),
	m_eFreeBuilding(NO_BUILDING),

	m_eFreeCapitalBuilding(NO_BUILDING),
	m_eFreeBuildingPrereqTech(NO_TECH),
	m_eCapitalFreeBuildingPrereqTech(NO_TECH),

	m_eFreeBuildingOnConquest(NO_BUILDING),
	m_bFightWellDamaged(false),
	m_bMoveFriendlyWoodsAsRoad(false),
	m_bFasterAlongRiver(false),
	m_bFasterInHills(false),
	m_bEmbarkedAllWater(false),
    m_bEmbarkedToLandFlatCost(false),
#ifdef LEKMOD_TRAIT_CIVILIAN_EMBARK_ONE_MOVE
    m_bCiviliansEmbarkOneMove(false),
#endif
	m_bNoHillsImprovementMaintenance(false),
	m_bTechBoostFromCapitalScienceBuildings(false),
	m_bStaysAliveZeroCities(false),
	m_bFaithFromUnimprovedForest(false),
#ifdef NQ_UNIT_IMMUNE_TO_PLUNDER_FROM_TRAIT
	m_bSeaTradeRoutesArePlunderImmune(false),
#endif


	m_bEarnsGreatPersonOnSlotOrGuild(false),
	m_bHasBuiltWritersGuild(false),
	m_bHasBuiltArtistsGuild(false),
	m_bHasBuiltMusiciansGuild(false),


	m_bBonusReligiousBelief(false),
	m_bAbleToAnnexCityStates(false),
	m_bCrossesMountainsAfterGreatGeneral(false),
	m_bMayaCalendarBonuses(false),
	m_bNoAnnexing(false),
	m_bTechFromCityConquer(false),
	m_bUniqueLuxuryRequiresNewArea(false),
#if defined(LEKMOD_EXPERIMENTAL_CHANGES)
	m_piWorldWonderYieldChanges(NULL),
#endif
#if defined(TRAITIFY)
	m_ppiBuildingCostOverride(NULL),
	m_ppiBuildingClassYieldChanges(NULL),
	m_piPuppetYieldModifiers(NULL),
	m_piGoldenAgeYieldModifiers(NULL),
	m_paiRouteMovementChange(NULL),
	m_ppiResourceClassYieldChanges(NULL),
	m_ppiFeatureYieldChanges(NULL),
	m_ppiTerrainYieldChanges(NULL),
	m_ppiResourceYieldChanges(NULL),
	m_ppiFreshWaterImprovementYieldChanges(NULL),
	m_ppiNonFreshWaterImprovementYieldChanges(NULL),

	m_paiBuildingClassGlobalHappiness(NULL),
	m_paiBuildingClassHappiness(NULL),
#endif
#if defined(v35_TRAITIFY)
	m_paiUnitCombatWorkRateChange(NULL),
	m_paiBuildCompleteTileClaimRange(NULL),
	m_paiBuildCompleteTileStealRange(NULL),
#endif
#if defined(LEKMOD_v34)
	m_paiYieldPerPopulation(NULL),
	m_paiYieldPerPopulationForeignReligion(NULL),
#endif
#if defined(FULL_YIELD_FROM_KILLS)
	m_paiYieldFromKills(NULL),
	m_paiYieldFromKillsMax(NULL),
#endif
	m_paiExtraYieldThreshold(NULL),
	m_paiYieldChange(NULL),
#if defined(LEKMOD_CITY_YIELDS_TRAITS)
	m_piCapitalYieldChange(NULL),
	m_ppiCapitalEraYieldChange(NULL),
	m_ppiCapitalTechYieldChange(NULL),
	m_piCityYieldChange(NULL),
	m_ppiCityEraYieldChange(NULL),
	m_ppiCityTechYieldChange(NULL),
#endif
#if defined(LEKMOD_FREE_RESOURCE_CITY_GRANT)
	m_piFreeResourceCityYieldChange(NULL),
#endif
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	m_paiGreatWorkYieldChanges(NULL),
	m_ppiGreatWorkClassYieldChanges(NULL),
#endif
	m_paiYieldChangeStrategicResources(NULL),
	m_paiYieldChangeLuxuryResources(NULL),
	m_paiYieldChangeNaturalWonder(NULL),
#if !defined(TRADE_REFACTOR)
	m_paiYieldChangePerTradePartner(NULL),
	m_paiYieldChangeIncomingTradeRoute(NULL),
#else
	m_paiTradePartnerYieldFlatBonusPerEra(NULL),
	m_ppiTradeConnectionLandYieldChange(NULL),
	m_ppiTradeConnectionSeaYieldChange(NULL),
	m_ppiYieldChangePerTradePartnerByDomain(NULL),
	m_ppiIncomingTradeConnectionLandYieldChange(NULL),
	m_ppiIncomingTradeConnectionSeaYieldChange(NULL),
	m_ppiTradeConnectionLandYieldModifier(NULL),
	m_ppiTradeConnectionSeaYieldModifier(NULL),
#endif
	m_paiYieldModifier(NULL),
	m_piStrategicResourceQuantityModifier(NULL),
	m_piResourceQuantityModifiers(NULL),
#ifdef AUI_WARNING_FIXES
	m_piMovesChangeUnitCombats(NULL),
	m_piMaintenanceModifierUnitCombats(NULL),
#if defined(LEKMOD_TRAIT_BUILDING_CLASS_PRODUCTION_MODIFIERS)
	m_paiBuildingClassProductionModifiers(NULL),
#endif
	m_iWorkerSpeedModifier(0),
	m_iAfraidMinorPerTurnInfluence(0),
	m_iLandTradeRouteRangeBonus(0),
	m_iTradeReligionModifier(0),
	m_iTradeBuildingModifier(0),
	m_eCombatBonusImprovement(NO_IMPROVEMENT),
	m_bRiverTradeRoad(false),
	m_bAngerFreeIntrusionOfCityStates(false),
#endif
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	m_ppiImprovementYieldChanges(std::pair<int**, size_t>(NULL, 0)),
	m_ppiSpecialistYieldChanges(std::pair<int**, size_t>(NULL, 0)),
	m_ppiUnimprovedFeatureYieldChanges(std::pair<int**, size_t>(NULL, 0))
#else
	m_ppiImprovementYieldChanges(NULL),
	m_ppiSpecialistYieldChanges(NULL),
#ifdef LEK_TRAIT_SPECIALIST_YIELD_MAX_ONE
	m_ppiAnySpecificSpecialistYieldChanges(NULL),
#endif
	m_ppiUnimprovedFeatureYieldChanges(NULL)
#endif
{
}


CvTraitEntry::~CvTraitEntry()
{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	CvDatabaseUtility::SafeDelete2DArray(m_ppiImprovementYieldChanges.first, m_ppiImprovementYieldChanges.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiSpecialistYieldChanges.first, m_ppiSpecialistYieldChanges.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiUnimprovedFeatureYieldChanges.first, m_ppiUnimprovedFeatureYieldChanges.second);
#else
	CvDatabaseUtility::SafeDelete2DArray(m_ppiImprovementYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiSpecialistYieldChanges);
#if defined(LEKMOD_EXPERIMENTAL_CHANGES)
	SAFE_DELETE_ARRAY(m_piWorldWonderYieldChanges);
#endif
#if defined(TRAITIFY)
	CvDatabaseUtility::SafeDelete2DArray(m_ppiBuildingClassYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiBuildingCostOverride);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiResourceClassYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiFeatureYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiResourceYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiTerrainYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiFreshWaterImprovementYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiNonFreshWaterImprovementYieldChanges);
	SAFE_DELETE_ARRAY(m_piPuppetYieldModifiers);
	SAFE_DELETE_ARRAY(m_piGoldenAgeYieldModifiers);
	SAFE_DELETE_ARRAY(m_paiRouteMovementChange);
	SAFE_DELETE_ARRAY(m_paiBuildingClassGlobalHappiness);
	SAFE_DELETE_ARRAY(m_paiBuildingClassHappiness);
#endif
#if defined(v35_TRAITIFY)
	SAFE_DELETE_ARRAY(m_paiUnitCombatWorkRateChange);
	SAFE_DELETE_ARRAY(m_paiBuildCompleteTileClaimRange);
	SAFE_DELETE_ARRAY(m_paiBuildCompleteTileStealRange);
#endif
#if defined(FULL_YIELD_FROM_KILLS)
	SAFE_DELETE_ARRAY(m_paiYieldFromKills);
	SAFE_DELETE_ARRAY(m_paiYieldFromKillsMax);
#endif
#if defined(LEKMOD_v34)
	SAFE_DELETE_ARRAY(m_paiYieldPerPopulation);
	SAFE_DELETE_ARRAY(m_paiYieldPerPopulationForeignReligion);
#endif
#if defined(LEKMOD_CITY_YIELDS_TRAITS)
	SAFE_DELETE_ARRAY(m_piCapitalYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiCapitalEraYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiCapitalTechYieldChange);
	SAFE_DELETE_ARRAY(m_piCityYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiCityEraYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiCityTechYieldChange);
#endif
#if defined(LEKMOD_FREE_RESOURCE_CITY_GRANT)
	SAFE_DELETE_ARRAY(m_piFreeResourceCityYieldChange);
#endif
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	SAFE_DELETE_ARRAY(m_paiGreatWorkYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiGreatWorkClassYieldChanges);
#endif
#ifdef LEK_TRAIT_SPECIALIST_YIELD_MAX_ONE
	CvDatabaseUtility::SafeDelete2DArray(m_ppiAnySpecificSpecialistYieldChanges);
#endif
	CvDatabaseUtility::SafeDelete2DArray(m_ppiUnimprovedFeatureYieldChanges);
#endif
#ifdef LEKMOD_BUILD_TIME_OVERRIDE

	m_aiBuildTimeOverride.clear();
	m_aiBuildTimeOverrideResourceClassRequired.clear();
	m_BuildTimeOverrides.clear();
#endif
#ifdef AUI_WARNING_FIXES
	SAFE_DELETE_ARRAY(m_paiExtraYieldThreshold);
	SAFE_DELETE_ARRAY(m_paiYieldChange);
	SAFE_DELETE_ARRAY(m_paiYieldChangeStrategicResources);
	SAFE_DELETE_ARRAY(m_paiYieldChangeLuxuryResources);
	SAFE_DELETE_ARRAY(m_paiYieldChangeNaturalWonder);
	SAFE_DELETE_ARRAY(m_paiYieldChangePerTradePartner);
	SAFE_DELETE_ARRAY(m_paiYieldChangeIncomingTradeRoute);
	SAFE_DELETE_ARRAY(m_paiYieldModifier);
	SAFE_DELETE_ARRAY(m_piStrategicResourceQuantityModifier);
	SAFE_DELETE_ARRAY(m_piResourceQuantityModifiers);
	SAFE_DELETE_ARRAY(m_piMovesChangeUnitCombats);
	SAFE_DELETE_ARRAY(m_piMaintenanceModifierUnitCombats);
#if defined(LEKMOD_TRAIT_BUILDING_CLASS_PRODUCTION_MODIFIERS)
	SAFE_DELETE_ARRAY(m_paiBuildingClassProductionModifiers);
#endif
#endif
}


int CvTraitEntry::GetLevelExperienceModifier() const
{
	return m_iLevelExperienceModifier;
}


int CvTraitEntry::GetGreatPeopleRateModifier() const
{
	return m_iGreatPeopleRateModifier;
}


int CvTraitEntry::GetGreatScientistRateModifier() const
{
	return m_iGreatScientistRateModifier;
}


int CvTraitEntry::GetGreatGeneralRateModifier() const
{
	return m_iGreatGeneralRateModifier;
}


int CvTraitEntry::GetGreatGeneralExtraBonus() const
{
	return m_iGreatGeneralExtraBonus;
}


int CvTraitEntry::GetGreatPersonGiftInfluence() const
{
	return m_iGreatPersonGiftInfluence;
}


int CvTraitEntry::GetMaxGlobalBuildingProductionModifier() const
{
	return m_iMaxGlobalBuildingProductionModifier;
}


int CvTraitEntry::GetMaxTeamBuildingProductionModifier() const
{
	return m_iMaxTeamBuildingProductionModifier;
}


int CvTraitEntry::GetMaxPlayerBuildingProductionModifier() const
{
	return m_iMaxPlayerBuildingProductionModifier;
}


int CvTraitEntry::GetCityUnhappinessModifier() const
{
	return m_iCityUnhappinessModifier;
}


int CvTraitEntry::GetPopulationUnhappinessModifier() const
{
	return m_iPopulationUnhappinessModifier;
}


int CvTraitEntry::GetCityStateBonusModifier() const
{
	return m_iCityStateBonusModifier;
}


int CvTraitEntry::GetCityStateFriendshipModifier() const
{
	return m_iCityStateFriendshipModifier;
}

#ifdef LEKMOD_TRAIT_FIRST_PROPHET_COST_MOD
int CvTraitEntry::GetFirstProphetCostMod() const
{
	return m_iFirstProphetCostMod;
}
#endif


int CvTraitEntry::GetCityStateCombatModifier() const
{
	return m_iCityStateCombatModifier;
}


int CvTraitEntry::GetLandBarbarianConversionPercent() const
{
	return m_iLandBarbarianConversionPercent;
}


int CvTraitEntry::GetLandBarbarianConversionExtraUnits() const
{
	return m_iLandBarbarianConversionExtraUnits;
}


int CvTraitEntry::GetSeaBarbarianConversionPercent() const
{
	return m_iSeaBarbarianConversionPercent;
}


int CvTraitEntry::GetCapitalBuildingModifier() const
{
	return m_iCapitalBuildingModifier;
}


int CvTraitEntry::GetPlotBuyCostModifier() const
{
	return m_iPlotBuyCostModifier;
}


int CvTraitEntry::GetPlotCultureCostModifier() const
{
	return m_iPlotCultureCostModifier;
}

#ifdef NQ_SCIENCE_PER_GREAT_PERSON_BORN

int CvTraitEntry::GetSciencePerGreatPersonBorn() const
{
	return m_iSciencePerGreatPersonBorn;
}
#endif

#ifdef NQ_NUM_TURNS_BEFORE_MINOR_ALLIES_REFUSE_BRIBES_FROM_TRAIT

int CvTraitEntry::GetNumTurnsBeforeMinorAlliesRefuseBribes() const
{
	return m_iNumTurnsBeforeMinorAlliesRefuseBribes;
}
#endif


int CvTraitEntry::GetCultureFromKills() const
{
	return m_iCultureFromKills;
}


int CvTraitEntry::GetFaithFromKills() const
{
	return m_iFaithFromKills;
}


int CvTraitEntry::GetCityCultureBonus() const
{
	return m_iCityCultureBonus;
}


int CvTraitEntry::GetCapitalThemingBonusModifier() const
{
	return m_iCapitalThemingBonusModifier;
}


int CvTraitEntry::GetPolicyCostModifier() const
{
	return m_iPolicyCostModifier;
}


int CvTraitEntry::GetCityConnectionTradeRouteChange() const
{
	return m_iCityConnectionTradeRouteChange;
}


int CvTraitEntry::GetWonderProductionModifier() const
{
	return m_iWonderProductionModifier;
}


int CvTraitEntry::GetPlunderModifier() const
{
	return m_iPlunderModifier;
}


int CvTraitEntry::GetImprovementMaintenanceModifier() const
{
	return m_iImprovementMaintenanceModifier;
}


int CvTraitEntry::GetGoldenAgeDurationModifier() const
{
	return m_iGoldenAgeDurationModifier;
}


int CvTraitEntry::GetGoldenAgeMoveChange() const
{
	return m_iGoldenAgeMoveChange;
}


int CvTraitEntry::GetGoldenAgeCombatModifier() const
{
	return m_iGoldenAgeCombatModifier;
}


int CvTraitEntry::GetGoldenAgeTourismModifier() const
{
	return m_iGoldenAgeTourismModifier;
}


int CvTraitEntry::GetGoldenAgeGreatArtistRateModifier() const
{
	return m_iGoldenAgeGreatArtistRateModifier;
}


int CvTraitEntry::GetGoldenAgeGreatMusicianRateModifier() const
{
	return m_iGoldenAgeGreatMusicianRateModifier;
}


int CvTraitEntry::GetGoldenAgeGreatWriterRateModifier() const
{
	return m_iGoldenAgeGreatWriterRateModifier;
}


int CvTraitEntry::GetExtraEmbarkMoves() const
{
	return m_iExtraEmbarkMoves;
}


int CvTraitEntry::GetFreeUnitClassType() const
{
	return m_iFreeUnitClassType;
}


int CvTraitEntry::GetNaturalWonderFirstFinderGold() const
{
	return m_iNaturalWonderFirstFinderGold;
}


int CvTraitEntry::GetNaturalWonderSubsequentFinderGold() const
{
	return m_iNaturalWonderSubsequentFinderGold;
}

#if defined(TRAITIFY)

bool CvTraitEntry::IsHalfMoreSpecialistUnhappiness() const
{
	return m_bHalfMoreSpecialistUnhappiness;
}

int CvTraitEntry::GetNumExtraLeagueVotes() const
{
	return m_iNumExtraLeagueVotes;
}

int CvTraitEntry::GetNumTradeRouteBonus() const
{
	return m_iNumTradeRouteBonus;
}

int CvTraitEntry::GetMinorFriendshipMinimum() const
{
	return m_iMinorFriendshipMinimum;
}

int CvTraitEntry::GetGreatEngineerRateModifier() const
{
	return m_iGreatEngineerRateModifier;
}

int CvTraitEntry::GetGreatMerchantRateModifier() const
{
	return m_iGreatMerchantRateModifier;
}

int CvTraitEntry::GetMinorBullyModifier() const
{
	return m_iMinorBullyModifier;
}

int CvTraitEntry::GetExtraPopulation() const
{
	return m_iExtraPopulation;
}

int CvTraitEntry::GetInternationalRouteGrowthModifier() const
{
	return m_iInternationalRouteGrowthModifier;
}

int CvTraitEntry::GetLocalHappinessPerCity() const
{
	return m_iLocalHappinessPerCity;
}

int CvTraitEntry::GetGlobalHappinessPerCity() const
{
	return m_iGlobalHappinessPerCity;
}

int CvTraitEntry::GetUnhappinessModifierForPuppetedCities() const
{
	return m_iUnhappinessModifierForPuppetedCities;
}

int CvTraitEntry::GetFaithCostModifier() const
{
	return m_iFaithCostModifier;
}

int CvTraitEntry::GetIdeologyPressureUnhappinessModifier() const
{
	return m_iIdeologyPressureUnhappinessModifier;
}

int CvTraitEntry::GetForeignRelgionPressureModifier() const
{
	return m_iForeignRelgionPressureModifier;
}

int CvTraitEntry::GetFriendlyLandsCitizenMoveChange() const
{
	return m_iFriendlyLandsCitizenMoveChange;
}
#endif
#if defined(LEKMOD_v34)

bool CvTraitEntry::IsReligionEnhanceReformation() const
{
	return m_bReligionEnhanceReformation;
}

int CvTraitEntry::GetSelfReligiousPressureModifier() const
{
	return m_iSelfReligiousPressureModifier;
}
#endif



int CvTraitEntry::GetNaturalWonderFirstFinderFaith() const
{
	return m_iNaturalWonderFirstFinderFaith;
}


int CvTraitEntry::GetNaturalWonderSubsequentFinderFaith() const
{
	return m_iNaturalWonderSubsequentFinderFaith;
}




int CvTraitEntry::GetNaturalWonderYieldModifier() const
{
	return m_iNaturalWonderYieldModifier;
}


int CvTraitEntry::GetNaturalWonderHappinessModifier() const
{
	return m_iNaturalWonderHappinessModifier;
}


int CvTraitEntry::GetNearbyImprovementCombatBonus() const
{
	return m_iNearbyImprovementCombatBonus;
}


int CvTraitEntry::GetNearbyImprovementBonusRange() const
{
	return m_iNearbyImprovementBonusRange;
}


int CvTraitEntry::GetCultureBuildingYieldChange() const
{
	return m_iCultureBuildingYieldChange;
}


int CvTraitEntry::GetCombatBonusVsHigherTech() const
{
	return m_iCombatBonusVsHigherTech;
}


int CvTraitEntry::GetCombatBonusVsLargerCiv() const
{
	return m_iCombatBonusVsLargerCiv;
}


int CvTraitEntry::GetRazeSpeedModifier() const
{
	return m_iRazeSpeedModifier;
}


int CvTraitEntry::GetDOFGreatPersonModifier() const
{
	return m_iDOFGreatPersonModifier;
}


int CvTraitEntry::GetLandUnitMaintenanceModifier() const
{
	return m_iLandUnitMaintenanceModifier;
}


int CvTraitEntry::GetNavalUnitMaintenanceModifier() const
{
	return m_iNavalUnitMaintenanceModifier;
}


int CvTraitEntry::GetLuxuryHappinessRetention() const
{
	return m_iLuxuryHappinessRetention;
}



int CvTraitEntry::GetExtraHappinessPerLuxury() const
{
	return m_iExtraHappinessPerLuxury;
}




int CvTraitEntry::GetExtraSpies() const
{
	return m_iExtraSpies;
}



int CvTraitEntry::GetHappinessPerReligion() const
{
	return m_iHappinessPerReligion;
}



int CvTraitEntry::GetUnresearchedTechBonusFromKills() const
{
	return m_iUnresearchedTechBonusFromKills;
}

int CvTraitEntry::GetExtraFoundedCityTerritoryClaimRange() const
{
	return m_iExtraFoundedCityTerritoryClaimRange;
}


int CvTraitEntry::GetFreeSocialPoliciesPerEra() const
{
	return m_iFreeSocialPoliciesPerEra;
}

int CvTraitEntry::GetNumTradeRoutesModifier() const
{
	return m_iNumTradeRoutesModifier;
}

int CvTraitEntry::GetTradeRouteResourceModifier() const
{
	return m_iTradeRouteResourceModifier;
}

int CvTraitEntry::GetUniqueLuxuryCities() const
{
	return m_iUniqueLuxuryCities;
}

int CvTraitEntry::GetUniqueLuxuryQuantity() const
{
	return m_iUniqueLuxuryQuantity;
}

int CvTraitEntry::GetWorkerSpeedModifier() const
{
	return m_iWorkerSpeedModifier;
}

int CvTraitEntry::GetAfraidMinorPerTurnInfluence() const
{
	return m_iAfraidMinorPerTurnInfluence;
}

int CvTraitEntry::GetLandTradeRouteRangeBonus() const
{
	return m_iLandTradeRouteRangeBonus;
}

int CvTraitEntry::GetTradeReligionModifier() const
{
	return m_iTradeReligionModifier;
}

int CvTraitEntry::GetTradeBuildingModifier() const
{
	return m_iTradeBuildingModifier;
}



TechTypes CvTraitEntry::GetFreeBuildingPrereqTech() const
{
	return m_eFreeBuildingPrereqTech;
}
TechTypes CvTraitEntry::GetCapitalFreeBuildingPrereqTech() const
{
	return m_eCapitalFreeBuildingPrereqTech;
}
#if defined(LEKMOD_CITY_YIELDS_TRAITS)
UnitTypes CvTraitEntry::GetYieldSettleUnit() const
{
	return m_eYieldSettleUnit;
}
#endif

TechTypes CvTraitEntry::GetFreeUnitPrereqTech() const
{
	return m_eFreeUnitPrereqTech;
}


ImprovementTypes CvTraitEntry::GetCombatBonusImprovement() const
{
	return m_eCombatBonusImprovement;
}


BuildingTypes CvTraitEntry::GetFreeBuilding() const
{
	return m_eFreeBuilding;
}


BuildingTypes CvTraitEntry::GetFreeCapitalBuilding() const
{
	return m_eFreeCapitalBuilding;
}


BuildingTypes CvTraitEntry::GetFreeBuildingOnConquest() const
{
	return m_eFreeBuildingOnConquest;
}


bool CvTraitEntry::IsFightWellDamaged() const
{
	return m_bFightWellDamaged;
}


bool CvTraitEntry::IsMoveFriendlyWoodsAsRoad() const
{
	return m_bMoveFriendlyWoodsAsRoad;
}


bool CvTraitEntry::IsFasterAlongRiver() const
{
	return m_bFasterAlongRiver;
}


bool CvTraitEntry::IsFasterInHills() const
{
	return m_bFasterInHills;
}


bool CvTraitEntry::IsEmbarkedAllWater() const
{
	return m_bEmbarkedAllWater;
}


bool CvTraitEntry::IsEmbarkedToLandFlatCost() const
{
	return m_bEmbarkedToLandFlatCost;
}


bool CvTraitEntry::IsCiviliansEmbarkOneMove() const
{
    return m_bCiviliansEmbarkOneMove;
}


bool CvTraitEntry::IsNoHillsImprovementMaintenance() const
{
	return m_bNoHillsImprovementMaintenance;
}


bool CvTraitEntry::IsTechBoostFromCapitalScienceBuildings() const
{
	return m_bTechBoostFromCapitalScienceBuildings;
}


bool CvTraitEntry::IsStaysAliveZeroCities() const
{
	return m_bStaysAliveZeroCities;
}


bool CvTraitEntry::IsFaithFromUnimprovedForest() const
{
	return m_bFaithFromUnimprovedForest;
}

#ifdef NQ_UNIT_IMMUNE_TO_PLUNDER_FROM_TRAIT

bool CvTraitEntry::IsSeaTradeRoutesArePlunderImmune() const
{
	return m_bSeaTradeRoutesArePlunderImmune;
}
#endif



bool CvTraitEntry::IsEarnsGreatPersonOnSlotOrGuild() const
{
	return m_bEarnsGreatPersonOnSlotOrGuild;
}

bool CvTraitEntry::IsHasBuiltWritersGuild() const
{
	return m_bHasBuiltWritersGuild;
};

void CvTraitEntry::SetHasBuiltWritersGuild(bool hasBuilt)
{
	m_bHasBuiltWritersGuild = hasBuilt;
};

bool CvTraitEntry::IsHasBuiltArtistsGuild() const
{
	return m_bHasBuiltArtistsGuild;
};

void CvTraitEntry::SetHasBuiltArtistsGuild(bool hasBuilt)
{
	m_bHasBuiltArtistsGuild = hasBuilt;
};

bool CvTraitEntry::IsHasBuiltMusiciansGuild() const
{
	return m_bHasBuiltMusiciansGuild;
};

void CvTraitEntry::SetHasBuiltMusiciansGuild(bool hasBuilt)
{
	m_bHasBuiltMusiciansGuild = hasBuilt;
};



bool CvTraitEntry::IsBonusReligiousBelief() const
{
	return m_bBonusReligiousBelief;
}


bool CvTraitEntry::IsAbleToAnnexCityStates() const
{
	return m_bAbleToAnnexCityStates;
}


bool CvTraitEntry::IsCrossesMountainsAfterGreatGeneral() const
{
	return m_bCrossesMountainsAfterGreatGeneral;
}


bool CvTraitEntry::IsMayaCalendarBonuses() const
{
	return m_bMayaCalendarBonuses;
}


bool CvTraitEntry::IsNoAnnexing() const
{
	return m_bNoAnnexing;
}


bool CvTraitEntry::IsTechFromCityConquer() const
{
	return m_bTechFromCityConquer;
}


bool CvTraitEntry::IsUniqueLuxuryRequiresNewArea() const
{
	return m_bUniqueLuxuryRequiresNewArea;
}


bool CvTraitEntry::IsRiverTradeRoad() const
{
	return m_bRiverTradeRoad;
}


bool CvTraitEntry::IsAngerFreeIntrusionOfCityStates() const
{
	return m_bAngerFreeIntrusionOfCityStates;
}


const char* CvTraitEntry::getShortDescription() const
{
	return m_strShortDescription;
}


void CvTraitEntry::setShortDescription(const char* szVal)
{
	m_strShortDescription = szVal;
}


int CvTraitEntry::GetExtraYieldThreshold(int i) const
{
	return m_paiExtraYieldThreshold ? m_paiExtraYieldThreshold[i] : -1;
}


int CvTraitEntry::GetYieldChange(int i) const
{
	return m_paiYieldChange ? m_paiYieldChange[i] : -1;
}
#if defined(LEKMOD_CITY_YIELDS_TRAITS)
int CvTraitEntry::GetCapitalYieldChange(int i) const
{
	return m_piCapitalYieldChange ? m_piCapitalYieldChange[i] : 0;
}
int CvTraitEntry::GetCapitalEraYieldChange(int i, int j) const
{
	return m_ppiCapitalEraYieldChange ? m_ppiCapitalEraYieldChange[i][j] : 0;
}
int CvTraitEntry::GetCapitalTechYieldChange(int i, int j) const
{
	return m_ppiCapitalTechYieldChange ? m_ppiCapitalTechYieldChange[i][j] : 0;
}
int CvTraitEntry::GetCityYieldChange(int i) const
{
	return m_piCityYieldChange ? m_piCityYieldChange[i] : 0;
}
int CvTraitEntry::GetCityEraYieldChange(int i, int j) const
{
	return m_ppiCityEraYieldChange ? m_ppiCityEraYieldChange[i][j] : 0;
}
int CvTraitEntry::GetCityTechYieldChange(int i, int j) const
{
	return m_ppiCityTechYieldChange ? m_ppiCityTechYieldChange[i][j] : 0;
}
#endif
#if defined(LEKMOD_FREE_RESOURCE_CITY_GRANT)
int CvTraitEntry::GetFreeResourceCityYieldChange(int i) const
{
	return m_piFreeResourceCityYieldChange ? m_piFreeResourceCityYieldChange[i] : 0;
}
#endif

int CvTraitEntry::GetYieldChangeStrategicResources(int i) const
{
	return m_paiYieldChangeStrategicResources ? m_paiYieldChangeStrategicResources[i] : -1;
}



int CvTraitEntry::GetYieldChangeLuxuryResources(int i) const
{
	return m_paiYieldChangeLuxuryResources ? m_paiYieldChangeLuxuryResources[i] : -1;
}



int CvTraitEntry::GetYieldChangeNaturalWonder(int i) const
{
	return m_paiYieldChangeNaturalWonder ? m_paiYieldChangeNaturalWonder[i] : -1;
}
#if defined(FULL_YIELD_FROM_KILLS)

int CvTraitEntry::GetYieldFromKills(int i) const
{
	return m_paiYieldFromKills ? m_paiYieldFromKills[i] : -1;
}

int CvTraitEntry::GetYieldFromKillsMax(int i) const
{
	return m_paiYieldFromKillsMax ? m_paiYieldFromKillsMax[i] : 0;
}
#endif
#if !defined(TRADE_REFACTOR)

int CvTraitEntry::GetYieldChangePerTradePartner(int i) const
{
	return m_paiYieldChangePerTradePartner ? m_paiYieldChangePerTradePartner[i] : -1;
}


int CvTraitEntry::GetYieldChangeIncomingTradeRoute(int i) const
{
	return m_paiYieldChangeIncomingTradeRoute ? m_paiYieldChangeIncomingTradeRoute[i] : -1;
}
#else

int CvTraitEntry::GetTradePartnerYieldFlatBonusPerEra(int i) const
{
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_paiTradePartnerYieldFlatBonusPerEra ? m_paiTradePartnerYieldFlatBonusPerEra[i] : 0;
}

int CvTraitEntry::GetTradeConnectionLandYieldChange(int i, int j) const
{
	CvAssertMsg(i < NUM_TRADE_CONNECTION_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiTradeConnectionLandYieldChange ? m_ppiTradeConnectionLandYieldChange[i][j] : 0;
}

int CvTraitEntry::GetTradeConnectionSeaYieldChange(int i, int j) const
{
	CvAssertMsg(i < NUM_TRADE_CONNECTION_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiTradeConnectionSeaYieldChange ? m_ppiTradeConnectionSeaYieldChange[i][j] : 0;
}

int CvTraitEntry::GetYieldChangePerTradePartnerByDomain(int i, int j) const
{
	CvAssertMsg(i < GC.getNumUnitDomainInfos, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiYieldChangePerTradePartnerByDomain ? m_ppiYieldChangePerTradePartnerByDomain[i][j] : 0;
}

int CvTraitEntry::GetIncomingTradeConnectionLandYieldChange(int i, int j) const
{
	CvAssertMsg(i < NUM_TRADE_CONNECTION_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiIncomingTradeConnectionLandYieldChange ? m_ppiIncomingTradeConnectionLandYieldChange[i][j] : 0;
}

int CvTraitEntry::GetIncomingTradeConnectionSeaYieldChange(int i, int j) const
{
	CvAssertMsg(i < NUM_TRADE_CONNECTION_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiIncomingTradeConnectionSeaYieldChange ? m_ppiIncomingTradeConnectionSeaYieldChange[i][j] : 0;
}

int CvTraitEntry::GetTradeConnectionLandYieldModifier(int i, int j) const
{
	CvAssertMsg(i < NUM_TRADE_CONNECTION_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiTradeConnectionLandYieldModifier ? m_ppiTradeConnectionLandYieldModifier[i][j] : 0;
}

int CvTraitEntry::GetTradeConnectionSeaYieldModifier(int i, int j) const
{
	CvAssertMsg(i < NUM_TRADE_CONNECTION_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiTradeConnectionSeaYieldModifier ? m_ppiTradeConnectionSeaYieldModifier[i][j] : 0;
}
#endif

int CvTraitEntry::GetYieldModifier(int i) const
{
	return m_paiYieldModifier ? m_paiYieldModifier[i] : -1;
}

#if defined(LEKMOD_TRAIT_BUILDING_CLASS_PRODUCTION_MODIFIERS)
int CvTraitEntry::GetBuildingClassProductionModifier(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiBuildingClassProductionModifiers ? m_paiBuildingClassProductionModifiers[i] : 0;
}
#endif


int CvTraitEntry::GetStrategicResourceQuantityModifier(int i) const
{
	return m_piStrategicResourceQuantityModifier ? m_piStrategicResourceQuantityModifier[i] : -1;
}


int CvTraitEntry::GetResourceQuantityModifier(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piResourceQuantityModifiers ? m_piResourceQuantityModifiers[i] : -1;
}


int CvTraitEntry::GetImprovementYieldChanges(ImprovementTypes eIndex1, YieldTypes eIndex2) const
{
	CvAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "Index out of bounds");
	CvAssertMsg(eIndex1 > -1, "Index out of bounds");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(eIndex2 > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppiImprovementYieldChanges.first ? m_ppiImprovementYieldChanges.first[eIndex1][eIndex2] : 0;
#else
	return m_ppiImprovementYieldChanges ? m_ppiImprovementYieldChanges[eIndex1][eIndex2] : 0;
#endif
}


int CvTraitEntry::GetSpecialistYieldChanges(SpecialistTypes eIndex1, YieldTypes eIndex2) const
{
	CvAssertMsg(eIndex1 < GC.getNumSpecialistInfos(), "Index out of bounds");
	CvAssertMsg(eIndex1 > -1, "Index out of bounds");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(eIndex2 > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppiSpecialistYieldChanges.first ? m_ppiSpecialistYieldChanges.first[eIndex1][eIndex2] : 0;
#else
	return m_ppiSpecialistYieldChanges ? m_ppiSpecialistYieldChanges[eIndex1][eIndex2] : 0;
#endif
}

#ifdef LEK_TRAIT_SPECIALIST_YIELD_MAX_ONE
int CvTraitEntry::GetAnySpecificSpecialistYieldChanges(SpecialistTypes eIndex1, YieldTypes eIndex2) const
{
	CvAssertMsg(eIndex1 < GC.getNumSpecialistInfos(), "Index out of bounds");
	CvAssertMsg(eIndex1 > -1, "Index out of bounds");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(eIndex2 > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppiAnySpecificSpecialistYieldChanges.first ? m_ppiAnySpecificSpecialistYieldChanges.first[eIndex1][eIndex2] : 0;
#else
	return m_ppiAnySpecificSpecialistYieldChanges ? m_ppiAnySpecificSpecialistYieldChanges[eIndex1][eIndex2] : 0;
#endif
}
#endif

int CvTraitEntry::GetUnimprovedFeatureYieldChanges(FeatureTypes eIndex1, YieldTypes eIndex2) const
{
	CvAssertMsg(eIndex1 < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(eIndex1 > -1, "Index out of bounds");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(eIndex2 > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppiUnimprovedFeatureYieldChanges.first ? m_ppiUnimprovedFeatureYieldChanges.first[eIndex1][eIndex2] : 0;
#else
	return m_ppiUnimprovedFeatureYieldChanges ? m_ppiUnimprovedFeatureYieldChanges[eIndex1][eIndex2] : 0;
#endif
}


int CvTraitEntry::GetMovesChangeUnitCombat(const int unitCombatID) const
{
	CvAssertMsg((unitCombatID >= 0), "unitCombatID is less than zero");
	CvAssertMsg((unitCombatID < GC.getNumUnitCombatClassInfos()), "unitCombatID exceeds number of combat classes");

	return m_piMovesChangeUnitCombats[unitCombatID];
}


int CvTraitEntry::GetMaintenanceModifierUnitCombat(const int unitCombatID) const
{
	CvAssertMsg((unitCombatID >= 0), "unitCombatID is less than zero");
	CvAssertMsg((unitCombatID < GC.getNumUnitCombatClassInfos()), "unitCombatID exceeds number of combat classes");

	return m_piMaintenanceModifierUnitCombats[unitCombatID];
}


int CvTraitEntry::GetObsoleteTech() const
{
	return m_iObsoleteTech;
}


int CvTraitEntry::GetPrereqTech() const
{
	return m_iPrereqTech;
}
#if defined(v35_TRAITIFY)
bool CvTraitEntry::IsBuildableByUnitCombat(const int buildID, const int unitCombatID) const
{
	std::multimap<int, int>::const_iterator it = m_BuildableByUnitCombat.find(buildID);
	if(it != m_BuildableByUnitCombat.end())
	{

		std::multimap<int, int>::const_iterator lastElement = m_BuildableByUnitCombat.upper_bound(buildID);

		for(; it != lastElement; ++it)
		{
			if(it->second == unitCombatID)
			{
				return true;
			}
		}
	}
	return false;
}
#endif

bool CvTraitEntry::IsFreePromotionUnitCombat(const int promotionID, const int unitCombatID) const
{
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
}


bool CvTraitEntry::IsObsoleteByTech(TeamTypes eTeam)
{
	if(m_iObsoleteTech != NO_TECH)
	{
		if(GET_TEAM(eTeam).GetTeamTechs()->HasTech((TechTypes)m_iObsoleteTech))
		{
			return true;
		}
	}
	return false;
}


bool CvTraitEntry::IsEnabledByTech(TeamTypes eTeam)
{
	if(m_iPrereqTech != NO_TECH)
	{
		if(GET_TEAM(eTeam).GetTeamTechs()->HasTech((TechTypes)m_iPrereqTech))
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

bool CvTraitEntry::NoTrain(UnitClassTypes eUnitClass)
{
	if (eUnitClass != NO_UNITCLASS)
	{
		return m_abNoTrainUnitClass[eUnitClass];
	}
	else
	{
		return false;
	}
}

#ifdef LEKMOD_TRAIT_NO_BUILD_IMPROVEMENTS
bool CvTraitEntry::NoBuildImprovements(ImprovementTypes eImprovement)
{
	if (eImprovement != NO_IMPROVEMENT)
	{
		return m_abNoBuildImprovements[eImprovement];
	}
	else
	{
		return false;
	}
}
#endif
#if defined(LEKMOD_TRAIT_BAN_UNIT_MISSIONS)
bool CvTraitEntry::IsBannedUnitMission(MissionTypes eMission)
{
	return (eMission != NO_MISSION) ? m_abNoUnitMissions[eMission] : false;
}
#endif
#ifdef LEKMOD_BUILD_TIME_OVERRIDE
int CvTraitEntry::GetBuildTimeOverride(BuildTypes eBuild, ResourceClassTypes eResourceClass)
{
	if (eBuild == NO_BUILD)
	{
		return -1;
	}


	int iBestTime = -1;
	bool bFoundGenericMatch = false;

	typedef std::multimap<BuildTypes, std::pair<int, ResourceClassTypes>>::const_iterator it_type;
	std::pair<it_type, it_type> range = m_BuildTimeOverrides.equal_range(eBuild);


	for (it_type it = range.first; it != range.second; ++it)
	{
		ResourceClassTypes eRequiredClass = it->second.second;
		int iBuildTime = it->second.first;


		if (eRequiredClass == eResourceClass)
		{
			return iBuildTime;
		}


		if (eRequiredClass == NO_RESOURCECLASS)
		{
			iBestTime = iBuildTime;
			bFoundGenericMatch = true;
		}
	}


	if (bFoundGenericMatch)
	{
		return iBestTime;
	}

	return -1;
}
#endif
#if defined(LEKMOD_EXPERIMENTAL_CHANGES)
int CvTraitEntry::GetWorldWonderYieldChanges(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piWorldWonderYieldChanges ? m_piWorldWonderYieldChanges[i] : -1;
}
#endif
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
int CvTraitEntry::GetGreatWorkYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiGreatWorkYieldChanges ? m_paiGreatWorkYieldChanges[i] : -1;
}
int CvTraitEntry::GetGreatWorkClassYieldChange(int i, int j) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < GC.getNumGreatWorkClassInfos(), "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiGreatWorkClassYieldChanges ? m_ppiGreatWorkClassYieldChanges[i][j] : -1;
}
#endif
#if defined(TRAITIFY)

bool CvTraitEntry::IsBuildingClassRemoveRequiredTerrain(BuildingClassTypes eBuildingClass) const
{
	return eBuildingClass != NO_BUILDINGCLASS ? m_abBuildingClassRemoveRequiredTerrain[eBuildingClass] : false;
}

bool CvTraitEntry::IsUnitClassForceSpawnCapital(UnitClassTypes eUnitClass) const
{
	return eUnitClass != NO_UNITCLASS ? m_abUnitClassForceSpawnCapital[eUnitClass] : false;
}

int CvTraitEntry::GetResourceClassYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumResourceClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiResourceClassYieldChanges ? m_ppiResourceClassYieldChanges[i][j] : -1;
}

int CvTraitEntry::GetBuildingClassHappiness(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiBuildingClassHappiness ? m_paiBuildingClassHappiness[i] : -1;
}

int CvTraitEntry::GetBuildingClassGlobalHappiness(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiBuildingClassGlobalHappiness ? m_paiBuildingClassGlobalHappiness[i] : -1;
}

int CvTraitEntry::GetBuildingCostOverride(int i, int j) const
{
	CvAssertMsg(i < GC.getNumBuildingInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiBuildingCostOverride[i][j];
}

int CvTraitEntry::GetBuildingClassYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiBuildingClassYieldChanges[i][j];
}

int CvTraitEntry::GetGoldenAgeYieldModifier(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piGoldenAgeYieldModifiers ? m_piGoldenAgeYieldModifiers[i] : -1;
}

int CvTraitEntry::GetPuppetYieldModifiers(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piPuppetYieldModifiers ? m_piPuppetYieldModifiers[i] : -1;
}

int CvTraitEntry::GetRouteMovementChange(int i) const
{
	CvAssertMsg(i < GC.getNumRouteInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiRouteMovementChange ? m_paiRouteMovementChange[i] : 0;
}

int CvTraitEntry::GetFeatureYieldChanges(FeatureTypes eIndex1, YieldTypes eIndex2) const
{
	CvAssertMsg(eIndex1 < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(eIndex1 > -1, "Index out of bounds");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(eIndex2 > -1, "Index out of bounds");
	return m_ppiFeatureYieldChanges ? m_ppiFeatureYieldChanges[eIndex1][eIndex2] : 0;
}

int CvTraitEntry::GetTerrainYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiTerrainYieldChanges ? m_ppiTerrainYieldChanges[i][j] : 0;
}

int CvTraitEntry::GetResourceYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiResourceYieldChanges ? m_ppiResourceYieldChanges[i][j] : 0;
}

int CvTraitEntry::GetYieldPerPopulation(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiYieldPerPopulation ? m_paiYieldPerPopulation[i] : -1;
}

int CvTraitEntry::GetYieldPerPopulationForeignReligion(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiYieldPerPopulationForeignReligion ? m_paiYieldPerPopulationForeignReligion[i] : -1;
}

int CvTraitEntry::GetFreshWaterImprovementYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiFreshWaterImprovementYieldChanges ? m_ppiFreshWaterImprovementYieldChanges[i][j] : 0;
}

int CvTraitEntry::GetNonFreshWaterImprovementYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiNonFreshWaterImprovementYieldChanges ? m_ppiNonFreshWaterImprovementYieldChanges[i][j] : 0;
}
#endif

bool CvTraitEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;


	setShortDescription(kResults.GetText("ShortDescription"));

	m_iLevelExperienceModifier				= kResults.GetInt("LevelExperienceModifier");
	m_iGreatPeopleRateModifier				= kResults.GetInt("GreatPeopleRateModifier");
	m_iGreatScientistRateModifier			= kResults.GetInt("GreatScientistRateModifier");
	m_iGreatGeneralRateModifier				= kResults.GetInt("GreatGeneralRateModifier");
	m_iGreatGeneralExtraBonus				= kResults.GetInt("GreatGeneralExtraBonus");
	m_iGreatPersonGiftInfluence				= kResults.GetInt("GreatPersonGiftInfluence");
	m_iMaxGlobalBuildingProductionModifier	= kResults.GetInt("MaxGlobalBuildingProductionModifier");
	m_iMaxTeamBuildingProductionModifier	= kResults.GetInt("MaxTeamBuildingProductionModifier");
	m_iMaxPlayerBuildingProductionModifier	= kResults.GetInt("MaxPlayerBuildingProductionModifier");
	m_iCityUnhappinessModifier           	= kResults.GetInt("CityUnhappinessModifier");
	m_iPopulationUnhappinessModifier    	= kResults.GetInt("PopulationUnhappinessModifier");
	m_iCityStateBonusModifier               = kResults.GetInt("CityStateBonusModifier");
	m_iCityStateFriendshipModifier          = kResults.GetInt("CityStateFriendshipModifier");
#ifdef LEKMOD_TRAIT_FIRST_PROPHET_COST_MOD
	m_iFirstProphetCostMod					= kResults.GetInt("FirstProphetCostMod");
#endif
	m_iCityStateCombatModifier				= kResults.GetInt("CityStateCombatModifier");
	m_iLandBarbarianConversionPercent       = kResults.GetInt("LandBarbarianConversionPercent");
	m_iLandBarbarianConversionExtraUnits    = kResults.GetInt("LandBarbarianConversionExtraUnits");
	m_iSeaBarbarianConversionPercent        = kResults.GetInt("SeaBarbarianConversionPercent");
	m_iCapitalBuildingModifier				= kResults.GetInt("CapitalBuildingModifier");
	m_iPlotBuyCostModifier					= kResults.GetInt("PlotBuyCostModifier");
	m_iPlotCultureCostModifier              = kResults.GetInt("PlotCultureCostModifier");
#ifdef NQ_SCIENCE_PER_GREAT_PERSON_BORN
	m_iSciencePerGreatPersonBorn			= kResults.GetInt("SciencePerGreatPersonBorn");
#endif
#ifdef NQ_NUM_TURNS_BEFORE_MINOR_ALLIES_REFUSE_BRIBES_FROM_TRAIT
	m_iNumTurnsBeforeMinorAlliesRefuseBribes = kResults.GetInt("NumTurnsBeforeMinorAlliesRefuseBribes");
#endif
	m_iCultureFromKills						= kResults.GetInt("CultureFromKills");
	m_iFaithFromKills						= kResults.GetInt("FaithFromKills");
	m_iCityCultureBonus						= kResults.GetInt("CityCultureBonus");
	m_iCapitalThemingBonusModifier          = kResults.GetInt("CapitalThemingBonusModifier");
	m_iPolicyCostModifier					= kResults.GetInt("PolicyCostModifier");
	m_iCityConnectionTradeRouteChange		= kResults.GetInt("CityConnectionTradeRouteChange");
	m_iWonderProductionModifier				= kResults.GetInt("WonderProductionModifier");
	m_iPlunderModifier						= kResults.GetInt("PlunderModifier");
	m_iImprovementMaintenanceModifier       = kResults.GetInt("ImprovementMaintenanceModifier");
	m_iGoldenAgeDurationModifier			= kResults.GetInt("GoldenAgeDurationModifier");
	m_iGoldenAgeMoveChange				    = kResults.GetInt("GoldenAgeMoveChange");
	m_iGoldenAgeCombatModifier				= kResults.GetInt("GoldenAgeCombatModifier");
#if !defined(LEK_YIELD_TOURISM)
	m_iGoldenAgeTourismModifier				= kResults.GetInt("GoldenAgeTourismModifier");
#endif
	m_iGoldenAgeGreatArtistRateModifier		= kResults.GetInt("GoldenAgeGreatArtistRateModifier");
	m_iGoldenAgeGreatMusicianRateModifier	= kResults.GetInt("GoldenAgeGreatMusicianRateModifier");
	m_iGoldenAgeGreatWriterRateModifier		= kResults.GetInt("GoldenAgeGreatWriterRateModifier");
	m_iExtraEmbarkMoves						= kResults.GetInt("ExtraEmbarkMoves");
	m_iNaturalWonderFirstFinderGold         = kResults.GetInt("NaturalWonderFirstFinderGold");
	m_iNaturalWonderSubsequentFinderGold    = kResults.GetInt("NaturalWonderSubsequentFinderGold");

#if defined(TRAITIFY)
	m_bHalfMoreSpecialistUnhappiness		= kResults.GetBool("HalfMoreSpecialistUnhappiness");

	m_iNumExtraLeagueVotes					= kResults.GetInt("NumExtraLeagueVotes");
	m_iNumTradeRouteBonus					= kResults.GetInt("NumTradeRouteBonus");
	m_iMinorFriendshipMinimum				= kResults.GetInt("MinorFriendshipMinimum");
	m_iGreatEngineerRateModifier			= kResults.GetInt("GreatEngineerRateModifier");
	m_iGreatMerchantRateModifier			= kResults.GetInt("GreatMerchantRateModifier");
	m_iMinorBullyModifier					= kResults.GetInt("MinorBullyModifier");
	m_iInternationalRouteGrowthModifier		= kResults.GetInt("InternationalRouteGrowthModifier");
	m_iLocalHappinessPerCity				= kResults.GetInt("LocalHappinessPerCity");
	m_iGlobalHappinessPerCity				= kResults.GetInt("GlobalHappinessPerCity");
	m_iUnhappinessModifierForPuppetedCities = kResults.GetInt("UnhappinessModifierForPuppetedCities");
	m_iExtraPopulation						= kResults.GetInt("ExtraPopulation");
	m_iFaithCostModifier					= kResults.GetInt("FaithCostModifier");
	m_iIdeologyPressureUnhappinessModifier  = kResults.GetInt("IdeologyPressureUnhappinessModifier");
	m_iForeignRelgionPressureModifier		= kResults.GetInt("ForeignRelgionPressureModifier");
	m_iFriendlyLandsCitizenMoveChange		= kResults.GetInt("FriendlyLandsCitizenMoveChange");
#endif
#if defined(v35_TRAITIFY)
	m_bEmbarkedUnitsFullStrength			= kResults.GetBool("EmbarkedUnitsFullStrength");
	m_iCityStateUnitGiftExtraExperience		= kResults.GetInt("CityStateUnitGiftExtraExperience");
	m_iGreatGeneralSiegeBonus				= kResults.GetInt("GreatGeneralSiegeBonus");
#endif
#if defined(LEKMOD_v34)
	m_bReligionEnhanceReformation			= kResults.GetBool("ReligionEnhanceReformation");
	m_iSelfReligiousPressureModifier		= kResults.GetInt("SelfReligiousPressureModifier");
#endif

	m_iNaturalWonderFirstFinderFaith         = kResults.GetInt("NaturalWonderFirstFinderFaith");
	m_iNaturalWonderSubsequentFinderFaith    = kResults.GetInt("NaturalWonderSubsequentFinderFaith");

	m_iNaturalWonderYieldModifier           = kResults.GetInt("NaturalWonderYieldModifier");
	m_iNaturalWonderHappinessModifier       = kResults.GetInt("NaturalWonderHappinessModifier");
	m_iNearbyImprovementCombatBonus			= kResults.GetInt("NearbyImprovementCombatBonus");
	m_iNearbyImprovementBonusRange			= kResults.GetInt("NearbyImprovementBonusRange");
	m_iCultureBuildingYieldChange			= kResults.GetInt("CultureBuildingYieldChange");
	m_iCombatBonusVsHigherTech				= kResults.GetInt("CombatBonusVsHigherTech");
	m_iCombatBonusVsLargerCiv				= kResults.GetInt("CombatBonusVsLargerCiv");
	m_iLandUnitMaintenanceModifier          = kResults.GetInt("LandUnitMaintenanceModifier");
	m_iNavalUnitMaintenanceModifier         = kResults.GetInt("NavalUnitMaintenanceModifier");
	m_iRazeSpeedModifier					= kResults.GetInt("RazeSpeedModifier");
	m_iDOFGreatPersonModifier				= kResults.GetInt("DOFGreatPersonModifier");
	m_iLuxuryHappinessRetention				= kResults.GetInt("LuxuryHappinessRetention");
	m_iExtraHappinessPerLuxury				= kResults.GetInt("ExtraHappinessPerLuxury");
	m_iExtraSpies							= kResults.GetInt("ExtraSpies");
	m_iHappinessPerReligion					= kResults.GetInt("HappinessPerReligion");
	m_iUnresearchedTechBonusFromKills		= kResults.GetInt("UnresearchedTechBonusFromKills");
	m_iExtraFoundedCityTerritoryClaimRange  = kResults.GetInt("ExtraFoundedCityTerritoryClaimRange");
	m_iFreeSocialPoliciesPerEra				= kResults.GetInt("FreeSocialPoliciesPerEra");
	m_iNumTradeRoutesModifier				= kResults.GetInt("NumTradeRoutesModifier");
	m_iTradeRouteResourceModifier			= kResults.GetInt("TradeRouteResourceModifier");
	m_iUniqueLuxuryCities					= kResults.GetInt("UniqueLuxuryCities");
	m_iUniqueLuxuryQuantity					= kResults.GetInt("UniqueLuxuryQuantity");
	m_iWorkerSpeedModifier					= kResults.GetInt("WorkerSpeedModifier");
	m_iAfraidMinorPerTurnInfluence			= kResults.GetInt("AfraidMinorPerTurnInfluence");
	m_iLandTradeRouteRangeBonus				= kResults.GetInt("LandTradeRouteRangeBonus");
	m_iTradeReligionModifier				= kResults.GetInt("TradeReligionModifier");
	m_iTradeBuildingModifier				= kResults.GetInt("TradeBuildingModifier");

	const char* szTextVal = NULL;
	szTextVal = kResults.GetText("FreeUnit");
	if(szTextVal)
	{
		m_iFreeUnitClassType = GC.getInfoTypeForString(szTextVal, true);
	}

	szTextVal = kResults.GetText("FreeUnitPrereqTech");
	if(szTextVal)
	{
		m_eFreeUnitPrereqTech = (TechTypes)GC.getInfoTypeForString(szTextVal, true);
	}
#if defined(LEKMOD_CITY_YIELDS_TRAITS)
	szTextVal = kResults.GetText("YieldSettleUnit");
	if(szTextVal)
	{
		m_eYieldSettleUnit = (UnitTypes)GC.getInfoTypeForString(szTextVal, true);
	}
#endif


	szTextVal = kResults.GetText("FreeBuildingPrereqTech");
	if(szTextVal)
	{
		m_eFreeBuildingPrereqTech = (TechTypes)GC.getInfoTypeForString(szTextVal, true);
	}
	szTextVal = kResults.GetText("CapitalFreeBuildingPrereqTech");
	if(szTextVal)
	{
		m_eCapitalFreeBuildingPrereqTech = (TechTypes)GC.getInfoTypeForString(szTextVal, true);
	}




	szTextVal = kResults.GetText("CombatBonusImprovement");
	if(szTextVal)
	{
		m_eCombatBonusImprovement = (ImprovementTypes)GC.getInfoTypeForString(szTextVal, true);
	}

	szTextVal = kResults.GetText("ObsoleteTech");
	if(szTextVal)
	{
		m_iObsoleteTech = GC.getInfoTypeForString(szTextVal, true);
	}

	szTextVal = kResults.GetText("PrereqTech");
	if(szTextVal)
	{
		m_iPrereqTech = GC.getInfoTypeForString(szTextVal, true);
	}

	szTextVal = kResults.GetText("FreeBuilding");
	if(szTextVal)
	{
		m_eFreeBuilding = (BuildingTypes)GC.getInfoTypeForString(szTextVal, true);
	}

	szTextVal = kResults.GetText("FreeCapitalBuilding");
	if(szTextVal)
	{
		m_eFreeCapitalBuilding = (BuildingTypes)GC.getInfoTypeForString(szTextVal, true);
	}

	szTextVal = kResults.GetText("FreeBuildingOnConquest");
	if(szTextVal)
	{
		m_eFreeBuildingOnConquest = (BuildingTypes)GC.getInfoTypeForString(szTextVal, true);
	}

	m_bFightWellDamaged = kResults.GetBool("FightWellDamaged");
	m_bMoveFriendlyWoodsAsRoad = kResults.GetBool("MoveFriendlyWoodsAsRoad");
	m_bFasterAlongRiver = kResults.GetBool("FasterAlongRiver");
	m_bFasterInHills = kResults.GetBool("FasterInHills");
	m_bEmbarkedAllWater = kResults.GetBool("EmbarkedAllWater");
    m_bEmbarkedToLandFlatCost = kResults.GetBool("EmbarkedToLandFlatCost");
#ifdef LEKMOD_TRAIT_CIVILIAN_EMBARK_ONE_MOVE
    m_bCiviliansEmbarkOneMove = kResults.GetBool("CiviliansEmbarkOneMove");
#endif
	m_bNoHillsImprovementMaintenance = kResults.GetBool("NoHillsImprovementMaintenance");
	m_bTechBoostFromCapitalScienceBuildings = kResults.GetBool("TechBoostFromCapitalScienceBuildings");
	m_bStaysAliveZeroCities = kResults.GetBool("StaysAliveZeroCities");
	m_bFaithFromUnimprovedForest = kResults.GetBool("FaithFromUnimprovedForest");
#ifdef NQ_UNIT_IMMUNE_TO_PLUNDER_FROM_TRAIT
	m_bSeaTradeRoutesArePlunderImmune = kResults.GetBool("SeaTradeRoutesArePlunderImmune");
#endif
	m_bEarnsGreatPersonOnSlotOrGuild = kResults.GetBool("EarnsGreatPersonOnSlotOrGuild");
	m_bBonusReligiousBelief = kResults.GetBool("BonusReligiousBelief");
	m_bAbleToAnnexCityStates = kResults.GetBool("AbleToAnnexCityStates");
	m_bCrossesMountainsAfterGreatGeneral = kResults.GetBool("CrossesMountainsAfterGreatGeneral");
	m_bMayaCalendarBonuses = kResults.GetBool("MayaCalendarBonuses");
	m_bNoAnnexing = kResults.GetBool("NoAnnexing");
	m_bTechFromCityConquer = kResults.GetBool("TechFromCityConquer");
	m_bUniqueLuxuryRequiresNewArea = kResults.GetBool("UniqueLuxuryRequiresNewArea");
	m_bRiverTradeRoad = kResults.GetBool("RiverTradeRoad");
	m_bAngerFreeIntrusionOfCityStates = kResults.GetBool("AngerFreeIntrusionOfCityStates");


	const char* szTraitType = GetType();
	kUtility.SetYields(m_paiExtraYieldThreshold, "Trait_ExtraYieldThresholds", "TraitType", szTraitType);

	kUtility.SetYields(m_paiYieldChange, "Trait_YieldChanges", "TraitType", szTraitType);

#if !defined(LEKMOD_v34)
	kUtility.SetYields(m_paiYieldChangeStrategicResources, "Trait_YieldChangesStrategicResources", "TraitType", szTraitType);
	kUtility.SetYields(m_paiYieldChangeLuxuryResources, "Trait_YieldChangesLuxuryResources", "TraitType", szTraitType);
#endif
#if defined(FULL_YIELD_FROM_KILLS)
	{
		kUtility.InitializeArray(m_paiYieldFromKills, "Yields", 0);
		kUtility.InitializeArray(m_paiYieldFromKillsMax, "Yields", 0);
		std::string sqlKey = "Trait_YieldFromKills";
		Database::Results* pResults = kUtility.GetOrPrepareResults(sqlKey,
			"SELECT Yields.ID, Yield, COALESCE(Max, 0) "
			"FROM Trait_YieldFromKills "
			"INNER JOIN Yields ON Yields.Type = YieldType "
			"WHERE TraitType = ?");

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int iYieldID = pResults->GetInt(0);
			m_paiYieldFromKills[iYieldID] = pResults->GetInt(1);
			m_paiYieldFromKillsMax[iYieldID] = pResults->GetInt(2);
		}
		pResults->Reset();
	}
#endif
	kUtility.SetYields(m_paiYieldChangeNaturalWonder, "Trait_YieldChangesNaturalWonder", "TraitType", szTraitType);
	kUtility.SetYields(m_paiYieldModifier, "Trait_YieldModifiers", "TraitType", szTraitType);
	kUtility.SetYields(m_paiYieldModifier, "Trait_YieldModifiers", "TraitType", szTraitType);
#if !defined(TRADE_REFACTOR)
	kUtility.SetYields(m_paiYieldChangePerTradePartner, "Trait_YieldChangesPerTradePartner", "TraitType", szTraitType);
	kUtility.SetYields(m_paiYieldChangeIncomingTradeRoute, "Trait_YieldChangesIncomingTradeRoute", "TraitType", szTraitType);
#else

	{
		kUtility.Initialize2DArray(m_ppiTradeConnectionLandYieldChange, "TradeConnections", "Yields");
		kUtility.Initialize2DArray(m_ppiTradeConnectionSeaYieldChange, "TradeConnections", "Yields");
		std::string strKey("Trait_TradeConnectionYieldChange");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT TradeConnections.ID AS TradeConnectionID, Domains.ID as DomainID, Yields.ID AS YieldID, YieldTimes100 "
			"FROM Trait_TradeConnectionYieldChange "
			"INNER JOIN TradeConnections ON TradeConnections.Type = TradeConnectionType "
			"INNER JOIN Domains ON Domains.Type = DomainType "
			"INNER JOIN Yields ON Yields.Type = YieldType "
			"WHERE TraitType = ?");
		pResults->Bind(1, szTraitType);
		while (pResults->Step())
		{
			const int TradeConnectionID = pResults->GetInt(0);
			const int DomainID = pResults->GetInt(1);
			const int YieldID = pResults->GetInt(2);
			const int YieldTimes100 = pResults->GetInt(3);
			if (DomainID == DOMAIN_LAND)
				m_ppiTradeConnectionLandYieldChange[TradeConnectionID][YieldID] = YieldTimes100;
			else if (DomainID == DOMAIN_SEA)
				m_ppiTradeConnectionSeaYieldChange[TradeConnectionID][YieldID] = YieldTimes100;
		}
		pResults->Reset();
	}

	{
		kUtility.Initialize2DArray(m_ppiYieldChangePerTradePartnerByDomain, "Domains", "Yields");
		kUtility.InitializeArray(m_paiTradePartnerYieldFlatBonusPerEra, "Yields");
		std::string strKey("Trait_YieldChangesPerTradePartner");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT Domains.ID AS DomainID, Yields.ID AS YieldID, YieldTimes100, EraIncreaseTimes100 "
			"FROM Trait_YieldChangesPerTradePartner "
			"INNER JOIN Domains ON Domains.Type = DomainType "
			"INNER JOIN Yields ON Yields.Type = YieldType "
			"WHERE TraitType = ?");
		pResults->Bind(1, szTraitType);
		while (pResults->Step())
		{
			const int DomainID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int YieldTimes100 = pResults->GetInt(2);
			const int EraIncreaseTimes100 = pResults->GetInt(3);
			m_ppiYieldChangePerTradePartnerByDomain[DomainID][YieldID] = YieldTimes100;
			m_paiTradePartnerYieldFlatBonusPerEra[YieldID] = EraIncreaseTimes100;
		}
		pResults->Reset();
	}

	{
		kUtility.Initialize2DArray(m_ppiIncomingTradeConnectionLandYieldChange, "TradeConnections", "Yields");
		kUtility.Initialize2DArray(m_ppiIncomingTradeConnectionSeaYieldChange, "TradeConnections", "Yields");
		std::string strKey("Trait_IncomingTradeConnectionYieldChange");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT TradeConnections.ID AS TradeConnectionID, Domains.ID as DomainID, Yields.ID AS YieldID, YieldTimes100 "
			"FROM Trait_IncomingTradeConnectionYieldChange "
			"INNER JOIN TradeConnections ON TradeConnections.Type = TradeConnectionType "
			"INNER JOIN Domains ON Domains.Type = DomainType "
			"INNER JOIN Yields ON Yields.Type = YieldType "
			"WHERE TraitType = ?");
		pResults->Bind(1, szTraitType);
		while (pResults->Step())
		{
			const int TradeConnectionID = pResults->GetInt(0);
			const int DomainID = pResults->GetInt(1);
			const int YieldID = pResults->GetInt(2);
			const int YieldTimes100 = pResults->GetInt(3);
			if (DomainID == DOMAIN_LAND)
				m_ppiIncomingTradeConnectionLandYieldChange[TradeConnectionID][YieldID] = YieldTimes100;
			else if (DomainID == DOMAIN_SEA)
				m_ppiIncomingTradeConnectionSeaYieldChange[TradeConnectionID][YieldID] = YieldTimes100;
		}
		pResults->Reset();
	}

	{
		kUtility.Initialize2DArray(m_ppiTradeConnectionLandYieldModifier, "TradeConnections", "Yields");
		kUtility.Initialize2DArray(m_ppiTradeConnectionSeaYieldModifier, "TradeConnections", "Yields");
		std::string strKey("Trait_TradeConnectionYieldModifier");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT TradeConnections.ID AS TradeConnectionID, Domains.ID as DomainID, Yields.ID AS YieldID, YieldModifier "
			"FROM Trait_TradeConnectionYieldModifier "
			"INNER JOIN TradeConnections ON TradeConnections.Type = TradeConnectionType "
			"INNER JOIN Domains ON Domains.Type = DomainType "
			"INNER JOIN Yields ON Yields.Type = YieldType "
			"WHERE TraitType = ?");
		pResults->Bind(1, szTraitType);
		while (pResults->Step())
		{
			const int TradeConnectionID = pResults->GetInt(0);
			const int DomainID = pResults->GetInt(1);
			const int YieldID = pResults->GetInt(2);
			const int YieldModifier = pResults->GetInt(3);
			if (DomainID == DOMAIN_LAND)
				m_ppiTradeConnectionLandYieldModifier[TradeConnectionID][YieldID] = YieldModifier;
			else if (DomainID == DOMAIN_SEA)
				m_ppiTradeConnectionSeaYieldModifier[TradeConnectionID][YieldID] = YieldModifier;
		}
		pResults->Reset();
	}
#endif
	const int iNumTerrains = GC.getNumTerrainInfos();
#if defined(LEKMOD_CITY_YIELDS_TRAITS)
	kUtility.SetYields(m_piCapitalYieldChange, "Trait_CapitalYieldChange", "TraitType", szTraitType);
	kUtility.SetYieldMatrix(m_ppiCapitalEraYieldChange, "Eras", "Trait_CapitalEraYieldChange",
		"SELECT COALESCE(Eras.ID, -1) AS EraID, Yields.ID AS YieldID, YieldChange "
		"FROM Trait_CapitalEraYieldChange "
		"INNER JOIN Eras ON Eras.Type = EraType "
		"INNER JOIN Yields ON Yields.Type = YieldType "
		"WHERE TraitType = ?",
		szTraitType);
	kUtility.SetYieldMatrix(m_ppiCapitalTechYieldChange, "Technologies", "Trait_CapitalTechYieldChange",
		"SELECT COALESCE(Technologies.ID, -1) as TechID, Yields.ID AS YieldID, YieldChange "
		"FROM Trait_CapitalTechYieldChange "
		"INNER JOIN Technologies ON Technologies.Type = TechType "
		"INNER JOIN Yields ON Yields.Type = YieldType "
		"WHERE TraitType = ?",
		szTraitType);
	kUtility.SetYields(m_piCityYieldChange, "Trait_CityYieldChange", "TraitType", szTraitType);
	kUtility.SetYieldMatrix(m_ppiCityEraYieldChange, "Eras", "Trait_CityEraYieldChange",
		"SELECT COALESCE(Eras.ID, -1) AS EraID, Yields.ID AS YieldID, YieldChange "
		"FROM Trait_CityEraYieldChange "
		"INNER JOIN Eras ON Eras.Type = EraType "
		"INNER JOIN Yields ON Yields.Type = YieldType "
		"WHERE TraitType = ?",
		szTraitType);
	kUtility.SetYieldMatrix(m_ppiCityTechYieldChange, "Technologies", "Trait_CityTechYieldChange",
		"SELECT COALESCE(Technologies.ID, -1) AS TechID, Yields.ID AS YieldID, YieldChange "
		"FROM Trait_CityTechYieldChange "
		"INNER JOIN Technologies ON Technologies.Type = TechType "
		"INNER JOIN Yields ON Yields.Type = YieldType "
		"WHERE TraitType = ?",
		szTraitType);
#endif
#if defined(BENCHED)
	{
		kUtility.InitializeArray(m_paiYieldPerPopulation, "Trait_YieldPerPopulation", 0);
		kUtility.InitializeArray(m_paiYieldPerPopulationForeignReligion, "Trait_YieldPerPopulationForeignReligion", 0);

		std::string strKey("Trait_YieldPerPopulation");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT Yields.ID AS YieldID, Trait_YieldPerPopulation.YieldTimes100, Trait_YieldPerPopulation.ForeignReligion"
			"FROM Trait_YieldPerPopulation"
			"INNER JOIN Yields ON Yields.Type = Trait_YieldPerPopulation.YieldType"
			"WHERE Trait_YieldPerPopulation.TraitType = ?");
		pResults->Bind(1, szTraitType);
		while (pResults->Step())
		{
			const int YieldID = pResults->GetInt(0);
			const int yield = pResults->GetInt(1);
			const bool ForeignReligion = pResults->GetBool(2);

			if (ForeignReligion)
			{
				m_paiYieldPerPopulationForeignReligion[YieldID] = yield;
			}
			else
			{
				m_paiYieldPerPopulation[YieldID] = yield;
			}
		}
		pResults->Reset();
	}
#endif
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	{
		kUtility.InitializeArray(m_paiGreatWorkYieldChanges, "Yields", 0);
		kUtility.Initialize2DArray(m_ppiGreatWorkClassYieldChanges, "GreatWorkClasses", "Yields");
		std::string strKey("Trait_GreatWorkYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT COALESCE(GreatWorkClasses.ID, -1) AS GreatWorkClassID, Yields.ID AS YieldID, Trait_GreatWorkYieldChanges.YieldChange "
			"FROM Trait_GreatWorkYieldChanges "
			"LEFT JOIN GreatWorkClasses ON GreatWorkClasses.Type = Trait_GreatWorkYieldChanges.GreatWorkClassType "
			"INNER JOIN Yields ON Yields.Type = Trait_GreatWorkYieldChanges.YieldType "
			"WHERE Trait_GreatWorkYieldChanges.TraitType = ?");
		pResults->Bind(1, szTraitType);
		while (pResults->Step())
		{
			const int GreatWorkClassID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int YieldChange = pResults->GetInt(2);

			if (GreatWorkClassID == -1)
			{
				m_paiGreatWorkYieldChanges[YieldID] += YieldChange;
			}
			else
			{
				m_ppiGreatWorkClassYieldChanges[GreatWorkClassID][YieldID] += YieldChange;
			}
		}
		pResults->Reset();
	}
#endif
#if defined(LEKMOD_EXPERIMENTAL_CHANGES)
	kUtility.SetYields(m_piWorldWonderYieldChanges, "WorldWonderYieldChanges", "TraitType", szTraitType);
#endif
#if defined(TRAITIFY)
	kUtility.SetYields(m_piPuppetYieldModifiers, "Trait_PuppetYieldModifiers", "TraitType", szTraitType);
#if defined(LEK_YIELD_TOURISM)
	int iTourism = kResults.GetInt("GoldenAgeTourismModifier");
#endif

	{
		kUtility.InitializeArray(m_piGoldenAgeYieldModifiers, "Yields", 0);
		std::string strKey("Trait_GoldenAgeYieldModifiers");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT Yields.ID AS YieldID, Trait_GoldenAgeYieldModifiers.YieldModifier "
			"FROM Trait_GoldenAgeYieldModifiers "
			"INNER JOIN Yields ON Yields.Type = Trait_GoldenAgeYieldModifiers.YieldType "
			"WHERE Trait_GoldenAgeYieldModifiers.TraitType = ?");
		pResults->Bind(1, szTraitType);
		while (pResults->Step())
		{
			const int YieldID = pResults->GetInt(0);
			const int YieldModifier = pResults->GetInt(1);
#if defined(LEK_YIELD_TOURISM)
			if (YieldID == YIELD_TOURISM)
			{
				const_cast<int&>(YieldModifier) += iTourism;
			}
#endif
			m_piGoldenAgeYieldModifiers[YieldID] = YieldModifier;
		}
		pResults->Reset();
	}

	{
		kUtility.InitializeArray(m_paiRouteMovementChange, "Routes", 0);

		std::string strKey("Trait_RouteMovementChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT Routes.ID, Trait_RouteMovementChanges.MovementChange "
			"FROM Trait_RouteMovementChanges "
			"INNER JOIN Routes ON Trait_RouteMovementChanges.RouteType = Routes.Type "
			"WHERE Trait_RouteMovementChanges.TraitType = ?");
		pResults->Bind(1, szTraitType);
		while (pResults->Step())
		{
			const int RouteID = pResults->GetInt(0);
			const int MovementChange = pResults->GetInt(1);
			m_paiRouteMovementChange[RouteID] = MovementChange;
		}
		pResults->Reset();
	}

	{
		int BuildingClassLoop;
		for (BuildingClassLoop = 0; BuildingClassLoop < GC.getNumBuildingClassInfos(); BuildingClassLoop++)
		{
			m_abBuildingClassRemoveRequiredTerrain.push_back(false);
		}

		std::string strKey("Trait_BuildingClassRequiredTerrainRemoval");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT Traits.ID, BuildingClasses.ID FROM Trait_BuildingClassRequiredTerrainRemoval "
			"INNER JOIN Traits on Trait_BuildingClassRequiredTerrainRemoval.TraitType = Traits.Type "
			"INNER JOIN BuildingClasses on Trait_BuildingClassRequiredTerrainRemoval.BuildingClassType = BuildingClasses.Type "
			"WHERE TraitType = ?");

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int BuildingClassID = pResults->GetInt(1);

			m_abBuildingClassRemoveRequiredTerrain[BuildingClassID] = true;
		}
	}

	{
		int iUnitClassLoop;
		for (iUnitClassLoop = 0; iUnitClassLoop < GC.getNumUnitClassInfos(); iUnitClassLoop++)
		{
			m_abUnitClassForceSpawnCapital.push_back(false);
		}

		std::string strKey("Trait_UnitClassForceCapitalSpawn");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT Traits.ID as TraitID, UnitClasses.ID as UnitClassID FROM Trait_UnitClassForceCapitalSpawn "
			"INNER JOIN Traits ON Trait_UnitClassForceCapitalSpawn.TraitType = TraitType "
			"INNER JOIN UnitClasses on Trait_UnitClassForceCapitalSpawn.UnitClassType = UnitClasses.Type "
			"WHERE TraitType = ?");

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int UnitClassID = pResults->GetInt(1);

			m_abUnitClassForceSpawnCapital[UnitClassID] = true;
		}
	}
	{
		kUtility.Initialize2DArray(m_ppiResourceClassYieldChanges, "ResourceClasses", "Yields");

		std::string strKey("Trait_ResourceClassYieldChange");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey,
				"SELECT ResourceClasses.ID AS ResourceClassID, Yields.ID AS YieldID, Trait_ResourceClassYieldChange.Yield \
			 FROM Trait_ResourceClassYieldChange \
			 INNER JOIN ResourceClasses ON ResourceClasses.Type = Trait_ResourceClassYieldChange.ResourceClassType \
			 INNER JOIN Yields ON Yields.Type = Trait_ResourceClassYieldChange.YieldType \
			 WHERE Trait_ResourceClassYieldChange.TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int ResourceClassID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);
			m_ppiResourceClassYieldChanges[ResourceClassID][YieldID] = yield;
		}

		pResults->Reset();
	}

	{
		{
			const int iNumBuildingClasses = GC.getNumBuildingClassInfos();

			kUtility.InitializeArray(m_paiBuildingClassHappiness, iNumBuildingClasses, 0);
			kUtility.InitializeArray(m_paiBuildingClassGlobalHappiness, iNumBuildingClasses, 0);

			std::string strKey("Trait_BuildingClassHappinessChanges");
			Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
				"SELECT BuildingClasses.ID AS BuildingClassID, Trait_BuildingClassHappinessChanges.Happiness, Trait_BuildingClassHappinessChanges.GlobalHappiness "
				"FROM Trait_BuildingClassHappinessChanges "
				"INNER JOIN BuildingClasses ON BuildingClasses.Type = Trait_BuildingClassHappinessChanges.BuildingClassType "
				"WHERE Trait_BuildingClassHappinessChanges.TraitType = ?");

			pResults->Bind(1, szTraitType);

			while (pResults->Step())
			{
				const int iBuildingClassID = pResults->GetInt(0);
				const int iHappiness = pResults->GetInt(1);
				const bool bGlobal = pResults->GetBool(2);

				if (bGlobal)
				{
					m_paiBuildingClassGlobalHappiness[iBuildingClassID] = iHappiness;
				}
				else
				{
					m_paiBuildingClassHappiness[iBuildingClassID] = iHappiness;
				}
			}
		}
	}

	{
		kUtility.Initialize2DArray(m_ppiBuildingClassYieldChanges, "BuildingClasses", "Yields");

		std::string strKey("Trait_BuildingClassYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey,
				"select BuildingClasses.ID as BuildingClassID, Yields.ID as YieldID, Yield from Trait_BuildingClassYieldChanges \
inner join BuildingClasses on BuildingClasses.Type = BuildingClassType inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int BuildingClassID = pResults->GetInt(0);
			const int iYieldID = pResults->GetInt(1);
			const int iYieldChange = pResults->GetInt(2);

			m_ppiBuildingClassYieldChanges[BuildingClassID][iYieldID] = iYieldChange;
		}
	}

	kUtility.SetYieldMatrix(m_ppiBuildingCostOverride, "Buildings", "Trait_BuildingCostOverride",
		"SELECT Buildings.ID AS BuildingID, Yields.ID AS YieldID, Trait_BuildingCostOverride.Cost "
		"FROM Trait_BuildingCostOverride "
		"INNER JOIN Buildings ON Buildings.Type = Trait_BuildingCostOverride.BuildingType "
		"INNER JOIN Yields ON Yields.Type = Trait_BuildingCostOverride.YieldType "
		"WHERE Trait_BuildingCostOverride.TraitType = ?",
		szTraitType);

	kUtility.SetYieldMatrix(m_ppiResourceYieldChanges, "Resources", "Trait_ResourceYieldChanges",
		"select Resources.ID as ResourceID, Yields.ID as YieldID, Yield from Trait_ResourceYieldChanges "
		"inner join Resources on Resources.Type = ResourceType "
		"inner join Yields on Yields.Type = YieldType "
		"where TraitType = ?",
		szTraitType);

	{
		kUtility.Initialize2DArray(m_ppiTerrainYieldChanges, "Terrains", "Yields");

		std::string strKey("Trait_TerrainYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Terrains.ID as TerrainID, Yields.ID as YieldID, Yield from Trait_TerrainYieldChanges\
 inner join Terrains on Terrains.Type = TerrainType inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int TerrainID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiTerrainYieldChanges[TerrainID][YieldID] = yield;
		}
	}

	{
		kUtility.Initialize2DArray(m_ppiImprovementYieldChanges, "Improvements", "Yields");
		kUtility.Initialize2DArray(m_ppiFreshWaterImprovementYieldChanges, "Improvements", "Yields");
		kUtility.Initialize2DArray(m_ppiNonFreshWaterImprovementYieldChanges, "Improvements", "Yields");
		std::string strKey("Trait_ImprovementYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT Improvements.ID as ImprovementID, Yields.ID as YieldID, Yield , FreshWaterYield, NonFreshWaterYield "
			"FROM Trait_ImprovementYieldChanges "
			"INNER JOIN Improvements on Improvements.Type = ImprovementType "
			"INNER JOIN Yields on Yields.Type = YieldType "
			"WHERE TraitType = ?");
		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int ImprovementID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);
			const int freshWaterYield = pResults->GetInt(3);
			const int nonFreshWaterYield = pResults->GetInt(4);

			m_ppiImprovementYieldChanges[ImprovementID][YieldID] = yield;
			m_ppiFreshWaterImprovementYieldChanges[ImprovementID][YieldID] = freshWaterYield;
			m_ppiNonFreshWaterImprovementYieldChanges[ImprovementID][YieldID] = nonFreshWaterYield;
		}
	}
#endif
#if defined(v35_TRAITIFY)

	{
		kUtility.InitializeArray(m_paiUnitCombatWorkRateChange, "UnitCombatInfos", 0);
		std::string strKey("Trait_UnitCombatWorkRateChange");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT UnitCombatInfos.ID AS UnitCombatID, Trait_UnitCombatWorkRateChange.WorkRateChange "
			"FROM Trait_UnitCombatWorkRateChange "
			"INNER JOIN UnitCombatInfos ON UnitCombatInfos.Type = Trait_UnitCombatWorkRateChange.UnitCombatType "
			"WHERE Trait_UnitCombatWorkRateChange.TraitType = ?");
		pResults->Bind(1, szTraitType);
		while (pResults->Step())
		{
			const int UnitCombatID = pResults->GetInt(0);
			const int WorkRateChange = pResults->GetInt(1);
			m_paiUnitCombatWorkRateChange[UnitCombatID] = WorkRateChange;
		}
		pResults->Reset();
	}

	{
		kUtility.InitializeArray(m_paiBuildCompleteTileClaimRange, "Improvements", 0);
		kUtility.InitializeArray(m_paiBuildCompleteTileStealRange, "Improvements", 0);

		std::string strKey("Trait_BuildCompleteTileClaimRange");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT Improvements.ID AS ImprovementID, Trait_BuildCompleteTileClaimRange.ClaimRange, Trait_BuildCompleteTileClaimRange.Steal "
			"FROM Trait_BuildCompleteTileClaimRange "
			"INNER JOIN Improvements ON Improvements.Type = Trait_BuildCompleteTileClaimRange.ImprovementType "
			"WHERE Trait_BuildCompleteTileClaimRange.TraitType = ?");
		pResults->Bind(1, szTraitType);
		while (pResults->Step())
		{
			const int ImprovementID = pResults->GetInt(0);
			const int ClaimRange = pResults->GetInt(1);
			const bool Steal = pResults->GetBool(2);
			Steal ? m_paiBuildCompleteTileStealRange[ImprovementID] = ClaimRange : m_paiBuildCompleteTileClaimRange[ImprovementID] = ClaimRange;
		}
	}

	{
		for (int MissionLoop = 0; MissionLoop < GC.getNumMissionInfos(); MissionLoop++)
		{
			m_vbEmbarkedMissionAllowed.push_back(false);
		}
		std::string strKey("Trait_EmbarkAllowMissions");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT Missions.ID AS MissionID "
			"FROM Trait_EmbarkAllowMissions "
			"INNER JOIN Missions ON Missions.Type = Trait_EmbarkAllowMissions.MissionType "
			"WHERE Trait_EmbarkAllowMissions.TraitType = ?");
		pResults->Bind(1, szTraitType);
		while (pResults->Step())
		{
			const int MissionID = pResults->GetInt(0);
			m_vbEmbarkedMissionAllowed[MissionID] = true;
		}
	}

	{
		std::string sqlKey("Trait_UnitCombatBuilds");
		Database::Results* pResults = kUtility.GetOrPrepareResults(sqlKey,
			"SELECT UnitCombatInfos.ID AS UnitCombatID, Builds.ID AS BuildID "
			"FROM Trait_UnitCombatBuilds "
			"INNER JOIN UnitCombatInfos ON UnitCombatInfos.Type = Trait_UnitCombatBuilds.UnitCombatType "
			"INNER JOIN Builds ON Builds.Type = Trait_UnitCombatBuilds.BuildType "
			"WHERE Trait_UnitCombatBuilds.TraitType = ?");
		pResults->Bind(1, szTraitType);
		while (pResults->Step())
		{
			const int unitCombatID = pResults->GetInt(0);
			const int buildID = pResults->GetInt(1);



			m_BuildableByUnitCombat.insert(std::pair<int, int>(buildID, unitCombatID));
		}
		pResults->Reset();
		std::multimap<int, int>(m_BuildableByUnitCombat).swap(m_BuildableByUnitCombat);
	}
#endif

	{
		kUtility.InitializeArray(m_piStrategicResourceQuantityModifier, iNumTerrains, 0);

		std::string sqlKey = "Trait_Terrains";
		Database::Results* pResults = kUtility.GetOrPrepareResults(sqlKey,
			"select TraitType, Terrains.ID as TerrainID, StrategicResourceQuantityModifier from Trait_Terrains join Terrains on Terrains.Type = TerrainType where TraitType = ?");

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int iTerrainID = pResults->GetInt("TerrainID");
			CvAssert(iTerrainID > -1 && iTerrainID < iNumTerrains);

			const int iStrategicResourceQuantityModifier = pResults->GetInt("StrategicResourceQuantityModifier");
			m_piStrategicResourceQuantityModifier[iTerrainID] = iStrategicResourceQuantityModifier;
		}
	}


	{
		std::string sqlKey = "FreePromotionUnitCombats";
		Database::Results* pResults = kUtility.GetOrPrepareResults(sqlKey,
			"select UnitPromotions.ID, UnitCombatInfos.ID from Trait_FreePromotionUnitCombats, UnitPromotions, UnitCombatInfos where TraitType = ? and PromotionType = UnitPromotions.Type and UnitCombatType = UnitCombatInfos.Type");

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int unitPromotionID = pResults->GetInt(0);
			const int unitCombatInfoID = pResults->GetInt(1);

			m_FreePromotionUnitCombats.insert(std::pair<int, int>(unitPromotionID, unitCombatInfoID));
		}

		pResults->Reset();


		std::multimap<int,int>(m_FreePromotionUnitCombats).swap(m_FreePromotionUnitCombats);

		kUtility.PopulateArrayByValue(m_piResourceQuantityModifiers, "Resources", "Trait_ResourceQuantityModifiers", "ResourceType", "TraitType", szTraitType, "ResourceQuantityModifier");
#if defined(LEKMOD_TRAIT_BUILDING_CLASS_PRODUCTION_MODIFIERS)
		kUtility.PopulateArrayByValue(m_paiBuildingClassProductionModifiers, "BuildingClasses", "Trait_BuildingClassProductionModifiers", "BuildingClassType", "TraitType", szTraitType, "ProductionModifier");
#endif
	}


	{
		const int iNumUnitCombatClasses = kUtility.MaxRows("UnitCombatInfos");
		kUtility.InitializeArray(m_piMovesChangeUnitCombats, iNumUnitCombatClasses, 0);

		std::string sqlKey = "Trait_MovesChangeUnitCombats";
		Database::Results* pResults = kUtility.GetOrPrepareResults(sqlKey,
			"select UnitCombatInfos.ID, MovesChange from Trait_MovesChangeUnitCombats inner join UnitCombatInfos on UnitCombatInfos.Type = UnitCombatType where TraitType = ?;");

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int iUnitCombatID = pResults->GetInt(0);
			CvAssert(iUnitCombatID > -1 && iUnitCombatID < iNumUnitCombatClasses);

			const int iMovesChange = pResults->GetInt(1);
			m_piMovesChangeUnitCombats[iUnitCombatID] = iMovesChange;
		}

		pResults->Reset();
	}


	{
		const int iNumUnitCombatClasses = kUtility.MaxRows("UnitCombatInfos");
		kUtility.InitializeArray(m_piMaintenanceModifierUnitCombats, iNumUnitCombatClasses, 0);

		std::string sqlKey = "Trait_MaintenanceModifierUnitCombats";
		Database::Results* pResults = kUtility.GetOrPrepareResults(sqlKey,
			"select UnitCombatInfos.ID, MaintenanceModifier from Trait_MaintenanceModifierUnitCombats inner join UnitCombatInfos on UnitCombatInfos.Type = UnitCombatType where TraitType = ?;");

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int iUnitCombatID = pResults->GetInt(0);
			CvAssert(iUnitCombatID > -1 && iUnitCombatID < iNumUnitCombatClasses);

			const int iMaintenanceModifier = pResults->GetInt(1);
			m_piMaintenanceModifierUnitCombats[iUnitCombatID] = iMaintenanceModifier;
		}

		pResults->Reset();
	}
#if !defined(TRAITIFY)

	{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
		kUtility.Initialize2DArray(m_ppiImprovementYieldChanges.first, "Improvements", "Yields");
		m_ppiImprovementYieldChanges.second = kUtility.MaxRows("Improvements");
#else
		kUtility.Initialize2DArray(m_ppiImprovementYieldChanges, "Improvements", "Yields");
#endif

		std::string strKey("Trait_ImprovementYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Improvements.ID as ImprovementID, Yields.ID as YieldID, Yield from Trait_ImprovementYieldChanges inner join Improvements on Improvements.Type = ImprovementType inner join Yields on Yields.Type = YieldType where TraitType = ?");

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int ImprovementID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
			m_ppiImprovementYieldChanges.first[ImprovementID][YieldID] = yield;
#else
			m_ppiImprovementYieldChanges[ImprovementID][YieldID] = yield;
#endif
		}
	}
#endif


	{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
		kUtility.Initialize2DArray(m_ppiSpecialistYieldChanges.first, "Specialists", "Yields");
		m_ppiSpecialistYieldChanges.second = kUtility.MaxRows("Specialists");
#else
		kUtility.Initialize2DArray(m_ppiSpecialistYieldChanges, "Specialists", "Yields");
#endif


		std::string strKey("Building_SpecialistYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Specialists.ID as SpecialistID, Yields.ID as YieldID, Yield from Trait_SpecialistYieldChanges inner join Specialists on Specialists.Type = SpecialistType inner join Yields on Yields.Type = YieldType where TraitType = ?");

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int SpecialistID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
			m_ppiSpecialistYieldChanges.first[SpecialistID][YieldID] = yield;
#else
			m_ppiSpecialistYieldChanges[SpecialistID][YieldID] = yield;
#endif

		}
	}

#ifdef LEK_TRAIT_SPECIALIST_YIELD_MAX_ONE

	{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
		kUtility.Initialize2DArray(m_ppiAnySpecificSpecialistYieldChanges.first, "Specialists", "Yields");
		m_ppiAnySpecificSpecialistYieldChanges.second = kUtility.MaxRows("Specialists");
#else
		kUtility.Initialize2DArray(m_ppiAnySpecificSpecialistYieldChanges, "Specialists", "Yields");
#endif


		std::string strKey("Building_SpecialistYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Specialists.ID as SpecialistID, Yields.ID as YieldID, Yield from Trait_AnySpecificSpecialistYieldChanges inner join Specialists on Specialists.Type = SpecialistType inner join Yields on Yields.Type = YieldType where TraitType = ?");

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int SpecialistID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
			m_ppiAnySpecificSpecialistYieldChanges.first[SpecialistID][YieldID] = yield;
#else
			m_ppiAnySpecificSpecialistYieldChanges[SpecialistID][YieldID] = yield;
#endif

		}
	}
#endif
#if !defined(TRAITIFY)

	{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
		kUtility.Initialize2DArray(m_ppiUnimprovedFeatureYieldChanges.first, "Features", "Yields");
		m_ppiUnimprovedFeatureYieldChanges.second = kUtility.MaxRows("Features");
#else
		kUtility.Initialize2DArray(m_ppiUnimprovedFeatureYieldChanges, "Features", "Yields");
#endif

		std::string strKey("Trait_UnimprovedFeatureYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Features.ID as FeatureID, Yields.ID as YieldID, Yield from Trait_UnimprovedFeatureYieldChanges inner join Features on Features.Type = FeatureType inner join Yields on Yields.Type = YieldType where TraitType = ?");

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int FeatureID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
			m_ppiUnimprovedFeatureYieldChanges.first[FeatureID][YieldID] = yield;
#else
			m_ppiUnimprovedFeatureYieldChanges[FeatureID][YieldID] = yield;
#endif
		}
	}
#else
	{

		kUtility.Initialize2DArray(m_ppiUnimprovedFeatureYieldChanges, "Features", "Yields");
		kUtility.Initialize2DArray(m_ppiFeatureYieldChanges, "Features", "Yields");

		std::string strKey("Trait_FeatureYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT AllowImprovement, Features.ID as FeatureID, Yields.ID as YieldID, Yield "
			"FROM Trait_FeatureYieldChanges "
			"INNER JOIN Features ON Features.Type = FeatureType "
			"INNER JOIN Yields ON Yields.Type = YieldType "
			"WHERE TraitType = ? ");

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const bool bAllowImprovement = pResults->GetBool(0);
			const int iFeatureType = pResults->GetInt(1);
			const int iYieldType = pResults->GetInt(2);
			const int iYield = pResults->GetInt(3);

			bAllowImprovement ? m_ppiFeatureYieldChanges[iFeatureType][iYieldType] = iYield : m_ppiUnimprovedFeatureYieldChanges[iFeatureType][iYieldType] = iYield;
		}
		pResults->Reset();
	}
#endif


	{
#ifdef AUI_WARNING_FIXES
		for (uint iUnitClassLoop = 0; iUnitClassLoop < GC.getNumUnitClassInfos(); iUnitClassLoop++)
#else
		int iUnitClassLoop;
		for (iUnitClassLoop = 0; iUnitClassLoop < GC.getNumUnitClassInfos(); iUnitClassLoop++)
#endif
		{
			m_abNoTrainUnitClass.push_back(false);
		}

		std::string strKey("Trait_NoTrain");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT Traits.ID, UnitClasses.ID FROM Trait_NoTrain inner join Traits on Trait_NoTrain.TraitType = Traits.Type inner join UnitClasses on Trait_NoTrain.UnitClassType = UnitClasses.Type where TraitType = ?");

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int iUnitClass = pResults->GetInt(1);
			m_abNoTrainUnitClass[iUnitClass] = true;
		}
	}

#ifdef LEKMOD_TRAIT_NO_BUILD_IMPROVEMENTS

	{
		for (int iImprovementLoop = 0; iImprovementLoop < GC.getNumImprovementInfos(); iImprovementLoop++)
		{
			m_abNoBuildImprovements.push_back(false);
		}

		std::string strKey("Trait_NoBuildImprovement");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT Traits.ID, Improvements.ID FROM Trait_NoBuildImprovement inner join Traits on Trait_NoBuildImprovement.TraitType = Traits.Type inner join Improvements on Trait_NoBuildImprovement.ImprovementType = Improvements.Type where TraitType = ?");

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int iImprovement = pResults->GetInt(1);
			m_abNoBuildImprovements[iImprovement] = true;
		}

	}
#endif
#if defined(LEKMOD_TRAIT_BAN_UNIT_MISSIONS)
	{
		for (int iMissionLoop = 0; iMissionLoop < GC.getNumMissionInfos(); iMissionLoop++)
		{
			m_abNoUnitMissions.push_back(false);
		}
		std::string strKey("Trait_BanUnitMissions");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT Missions.ID FROM Trait_BanUnitMissions "
			"INNER JOIN Missions on MissionType = Missions.Type "
			"WHERE TraitType = ? ");
		pResults->Bind(1, szTraitType);
		while (pResults->Step())
		{
			const int iMission = pResults->GetInt(0);
			m_abNoUnitMissions[iMission] = true;
		}
		pResults->Reset();
	}
#endif
#ifdef LEKMOD_BUILD_TIME_OVERRIDE

	{

		const int iNumBuilds = kUtility.MaxRows("Builds");
		if (iNumBuilds > 0)
		{
			m_aiBuildTimeOverride.resize(iNumBuilds, -1);
			m_aiBuildTimeOverrideResourceClassRequired.resize(iNumBuilds, NO_RESOURCECLASS);
		}


		m_BuildTimeOverrides.clear();

		std::string strKey("Trait_BuildImprovementBuildTimeOverride");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT Builds.ID as BuildID, Trait_BuildImprovementBuildTimeOverride.Time, COALESCE(ResourceClasses.ID, -1) as ResourceClassID "
			"FROM Trait_BuildImprovementBuildTimeOverride "
			"LEFT JOIN ResourceClasses ON Trait_BuildImprovementBuildTimeOverride.ResourceClassRequired = ResourceClasses.Type "
			"INNER JOIN Traits ON Trait_BuildImprovementBuildTimeOverride.TraitType = Traits.Type "
			"INNER JOIN Builds ON Trait_BuildImprovementBuildTimeOverride.BuildType = Builds.Type "
			"WHERE TraitType = ?");

		if (pResults != NULL)
		{
			pResults->Bind(1, szTraitType);

			while (pResults->Step())
			{
				const int iBuildID = pResults->GetInt(0);
				const int iBuildTime = pResults->GetInt(1);
				const int iResourceClassID = pResults->GetInt(2);
				ResourceClassTypes eResourceClass = NO_RESOURCECLASS;

				if (iResourceClassID != -1)
				{
					eResourceClass = (ResourceClassTypes)iResourceClassID;
				}
				

				m_BuildTimeOverrides.insert(std::make_pair((BuildTypes)iBuildID, std::make_pair(iBuildTime, eResourceClass)));


				if (iBuildID >= 0 && iBuildID < iNumBuilds)
				{
					m_aiBuildTimeOverride[iBuildID] = iBuildTime;
					m_aiBuildTimeOverrideResourceClassRequired[iBuildID] = eResourceClass;
				}
			}

			pResults->Reset();
		}


		std::multimap<BuildTypes, std::pair<int, ResourceClassTypes>>(m_BuildTimeOverrides).swap(m_BuildTimeOverrides);
	}
#endif

	{
		m_vFreeResourceCities.clear();
		std::string strKey("Trait_FreeResourceCities");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT Resources.ID, ResourceQuantity, COALESCE(Technologies.ID, -1), "
			"NumCities, City, Found, Tech, UniqueArea, ClaimPlot, ResourceGroup, Priority, CycleGroup "
			"FROM Trait_FreeResourceCities "
			"INNER JOIN Resources ON Resources.Type = ResourceType "
			"LEFT JOIN Technologies ON Technologies.Type = TechType "
			"WHERE TraitType = ?");
		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			FreeResourceCities temp;

			temp.m_eResource = static_cast<ResourceTypes>(pResults->GetInt(0));
			temp.m_iResourceQuantity = pResults->GetInt(1);
			temp.m_eTechRequired = static_cast<TechTypes>(pResults->GetInt(2));
			temp.m_iNumCities = pResults->GetInt(3);
			
			temp.m_bCity = pResults->GetBool(4);
			temp.m_bFound = pResults->GetBool(5);
			temp.m_bTech = pResults->GetBool(6);
			temp.m_bUniqueArea = pResults->GetBool(7);
			temp.m_bClaimPlot = pResults->GetBool(8);

			temp.m_iGroup = pResults->GetInt(9);
			temp.m_iPriority = pResults->GetInt(10);
			temp.m_bCycleGroup = pResults->GetBool(11);

			if (temp.m_eResource != NO_RESOURCE && temp.m_iResourceQuantity > 0)
			{
				m_vFreeResourceCities.push_back(temp);
			}
		}
		pResults->Reset();
	}
#if defined(LEKMOD_FREE_RESOURCE_CITY_GRANT)
	kUtility.SetYields(m_piFreeResourceCityYieldChange, "Trait_FreeResourceCityYieldChanges", "TraitType", szTraitType);
#endif
#if defined(LEKMOD_GOLDEN_AGE_YIELD_THRESHOLD)

	{
		m_sGoldenAgeYieldThresholds.clear();
		std::string strKey("Trait_GoldenAgeYieldThresholdBonus");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT RequiredYields.ID as ThresholdYield, ThresholdAmount, BonusYields.ID as RwdYield, RwdAmount "
			"FROM Trait_GoldenAgeYieldThresholdBonus "
			"INNER JOIN Yields AS RequiredYields ON RequiredYields.Type = Trait_GoldenAgeYieldThresholdBonus.ThresholdYield "
			"INNER JOIN Yields AS BonusYields ON BonusYields.Type = Trait_GoldenAgeYieldThresholdBonus.RwdYield "
			"WHERE TraitType = ?");
		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			GoldenAgeYieldThreshold temp;

			temp.m_eThresholdYield = static_cast<YieldTypes>(pResults->GetInt(0));
			temp.m_iThresholdAmount = pResults->GetInt(1);
			temp.m_eRwdYield = static_cast<YieldTypes>(pResults->GetInt(2));
			temp.m_iRwdAmount = pResults->GetInt(3);

			m_sGoldenAgeYieldThresholds.push_back(temp);
		}
		pResults->Reset();
	}
#endif

	return true;
}





CvTraitXMLEntries::CvTraitXMLEntries(void)
{

}


CvTraitXMLEntries::~CvTraitXMLEntries(void)
{
	DeleteArray();
}


std::vector<CvTraitEntry*>& CvTraitXMLEntries::GetTraitEntries()
{
	return m_paTraitEntries;
}


int CvTraitXMLEntries::GetNumTraits()
{
	return m_paTraitEntries.size();
}


void CvTraitXMLEntries::DeleteArray()
{
	for(std::vector<CvTraitEntry*>::iterator it = m_paTraitEntries.begin(); it != m_paTraitEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paTraitEntries.clear();
}


CvTraitEntry* CvTraitXMLEntries::GetEntry(int index)
{
	return m_paTraitEntries[index];
}





CvPlayerTraits::CvPlayerTraits()
{
}


CvPlayerTraits::~CvPlayerTraits(void)
{
	Uninit();
}


void CvPlayerTraits::Init(CvTraitXMLEntries* pTraits, CvPlayer* pPlayer)
{

	m_pTraits = pTraits;
	m_pPlayer = pPlayer;

	Reset();



	m_iBaktunPreviousTurn = 0;
	m_aMayaBonusChoices.clear();
	m_iBaktun = 0;
	m_iKatun = 0;
	m_iTun = 0;
	m_iWinal = 0;
	m_iKin = 0;

	m_aUniqueLuxuryAreas.clear();
	m_iUniqueLuxuryCitiesPlaced = 0;

	m_vLeaderHasTrait = std::vector<bool>( GC.getNumTraitInfos(), false );
}


void CvPlayerTraits::InitPlayerTraits()
{


	m_vPotentiallyActiveLeaderTraits.clear();
	for(int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		m_vLeaderHasTrait[iI] = false;
		if (m_pPlayer && m_pPlayer->isMajorCiv() && m_pPlayer->isAlive() && m_pPlayer->getLeaderInfo().hasTrait( (TraitTypes)iI ))
		{
			m_vLeaderHasTrait[iI] = true;
			m_vPotentiallyActiveLeaderTraits.push_back( (TraitTypes)iI );
		}
	}
#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumTraitInfos(); iI++)
#else
	for(int iI = 0; iI < GC.getNumTraitInfos(); iI++)
#endif

	{
		if(HasTrait((TraitTypes)iI))
		{
			CvTraitEntry* trait = GC.getTraitInfo((TraitTypes)iI);
			m_iGreatPeopleRateModifier += trait->GetGreatPeopleRateModifier();
			m_iGreatScientistRateModifier += trait->GetGreatScientistRateModifier();
			m_iGreatGeneralRateModifier += trait->GetGreatGeneralRateModifier();
			m_iGreatGeneralExtraBonus += trait->GetGreatGeneralExtraBonus();
			m_iGreatPersonGiftInfluence += trait->GetGreatPersonGiftInfluence();
			m_iLevelExperienceModifier += trait->GetLevelExperienceModifier();
			m_iMaxGlobalBuildingProductionModifier += trait->GetMaxGlobalBuildingProductionModifier();
			m_iMaxTeamBuildingProductionModifier += trait->GetMaxTeamBuildingProductionModifier();
			m_iMaxPlayerBuildingProductionModifier += trait->GetMaxPlayerBuildingProductionModifier();
			m_iCityUnhappinessModifier += trait->GetCityUnhappinessModifier();
			m_iPopulationUnhappinessModifier += trait->GetPopulationUnhappinessModifier();
			m_iCityStateBonusModifier += trait->GetCityStateBonusModifier();
			m_iCityStateFriendshipModifier += trait->GetCityStateFriendshipModifier();
#ifdef LEKMOD_TRAIT_FIRST_PROPHET_COST_MOD
			m_iFirstProphetCostMod += trait->GetFirstProphetCostMod();
#endif
			m_iCityStateCombatModifier += trait->GetCityStateCombatModifier();
			m_iLandBarbarianConversionPercent += trait->GetLandBarbarianConversionPercent();
			m_iLandBarbarianConversionExtraUnits += trait->GetLandBarbarianConversionExtraUnits();
			m_iSeaBarbarianConversionPercent += trait->GetSeaBarbarianConversionPercent();
			m_iCapitalBuildingModifier += trait->GetCapitalBuildingModifier();
			m_iPlotBuyCostModifier += trait->GetPlotBuyCostModifier();
			m_iPlotCultureCostModifier += trait->GetPlotCultureCostModifier();
#ifdef NQ_SCIENCE_PER_GREAT_PERSON_BORN
			m_iSciencePerGreatPersonBorn += trait->GetSciencePerGreatPersonBorn();
#endif
#ifdef NQ_NUM_TURNS_BEFORE_MINOR_ALLIES_REFUSE_BRIBES_FROM_TRAIT
			m_iNumTurnsBeforeMinorAlliesRefuseBribes += trait->GetNumTurnsBeforeMinorAlliesRefuseBribes();
#endif
			m_iCultureFromKills += trait->GetCultureFromKills();
			m_iFaithFromKills += trait->GetFaithFromKills();
			m_iCityCultureBonus += trait->GetCityCultureBonus();
			m_iCapitalThemingBonusModifier += trait->GetCapitalThemingBonusModifier();
			m_iPolicyCostModifier += trait->GetPolicyCostModifier();
			m_iCityConnectionTradeRouteChange += trait->GetCityConnectionTradeRouteChange();
			m_iWonderProductionModifier += trait->GetWonderProductionModifier();
			m_iPlunderModifier += trait->GetPlunderModifier();
			m_iImprovementMaintenanceModifier += trait->GetImprovementMaintenanceModifier();
			m_iGoldenAgeDurationModifier += trait->GetGoldenAgeDurationModifier();
			m_iGoldenAgeMoveChange += trait->GetGoldenAgeMoveChange();
			m_iGoldenAgeCombatModifier += trait->GetGoldenAgeCombatModifier();
			m_iGoldenAgeTourismModifier += trait->GetGoldenAgeTourismModifier();
			m_iGoldenAgeGreatArtistRateModifier += trait->GetGoldenAgeGreatArtistRateModifier();
			m_iGoldenAgeGreatMusicianRateModifier += trait->GetGoldenAgeGreatMusicianRateModifier();
			m_iGoldenAgeGreatWriterRateModifier += trait->GetGoldenAgeGreatWriterRateModifier();
			m_iExtraEmbarkMoves += trait->GetExtraEmbarkMoves();
			m_iNaturalWonderFirstFinderGold += trait->GetNaturalWonderFirstFinderGold();
			m_iNaturalWonderSubsequentFinderGold += trait->GetNaturalWonderSubsequentFinderGold();
#if defined(TRAITIFY)
			m_bHalfMoreSpecialistUnhappiness = trait->IsHalfMoreSpecialistUnhappiness();

			m_iNumExtraLeagueVotes += trait->GetNumExtraLeagueVotes();
			m_iNumTradeRouteBonus += trait->GetNumTradeRouteBonus();
			m_iMinorFriendshipMinimum += trait->GetMinorFriendshipMinimum();
			m_iGreatEngineerRateModifier += trait->GetGreatEngineerRateModifier();
			m_iGreatMerchantRateModifier += trait->GetGreatMerchantRateModifier();
			m_iMinorBullyModifier += trait->GetMinorBullyModifier();
			m_iInternationalRouteGrowthModifier += trait->GetInternationalRouteGrowthModifier();
			m_iLocalHappinessPerCity += trait->GetLocalHappinessPerCity();
			m_iGlobalHappinessPerCity += trait->GetGlobalHappinessPerCity();
			m_iUnhappinessModifierForPuppetedCities += trait->GetUnhappinessModifierForPuppetedCities();
			m_iExtraPopulation += trait->GetExtraPopulation();
			m_iFaithCostModifier += trait->GetFaithCostModifier();
			m_iIdeologyPressureUnhappinessModifier += trait->GetIdeologyPressureUnhappinessModifier();
			m_iForeignRelgionPressureModifier += trait->GetForeignRelgionPressureModifier();
			m_iFriendlyLandsCitizenMoveChange += trait->GetFriendlyLandsCitizenMoveChange();
#endif
#if defined(v35_TRAITIFY)
			m_iEmbarkedUnitFullStrengthCount += trait->IsEmbarkedUnitsFullStrength() ? 1 : 0;
			m_iCityStateUnitGiftExtraExperience += trait->GetCityStateUnitGiftExtraExperience();
			m_iGreatGeneralSiegeBonus += trait->GetGreatGeneralSiegeBonus();
#endif
#if defined(LEKMOD_v34)
			m_bReligionEnhanceReformation = trait->IsReligionEnhanceReformation();
			m_iSelfReligiousPressureModifier += trait->GetSelfReligiousPressureModifier();
#endif

			m_iNaturalWonderFirstFinderFaith += trait->GetNaturalWonderFirstFinderFaith();
			m_iNaturalWonderSubsequentFinderFaith += trait->GetNaturalWonderSubsequentFinderFaith();

			m_iNaturalWonderYieldModifier += trait->GetNaturalWonderYieldModifier();
			m_iNaturalWonderHappinessModifier += trait->GetNaturalWonderHappinessModifier();
			m_iNearbyImprovementCombatBonus += trait->GetNearbyImprovementCombatBonus();
			m_iNearbyImprovementBonusRange += trait->GetNearbyImprovementBonusRange();
			m_iCultureBuildingYieldChange += trait->GetCultureBuildingYieldChange();
			m_iCombatBonusVsHigherTech += trait->GetCombatBonusVsHigherTech();
			m_iCombatBonusVsLargerCiv += trait->GetCombatBonusVsLargerCiv();
			m_iLandUnitMaintenanceModifier += trait->GetLandUnitMaintenanceModifier();
			m_iNavalUnitMaintenanceModifier += trait->GetNavalUnitMaintenanceModifier();
			m_iRazeSpeedModifier += trait->GetRazeSpeedModifier();
			m_iDOFGreatPersonModifier += trait->GetDOFGreatPersonModifier();
			m_iLuxuryHappinessRetention += trait->GetLuxuryHappinessRetention();
			m_iExtraHappinessPerLuxury += trait->GetExtraHappinessPerLuxury();
			m_iExtraSpies += trait->GetExtraSpies();
			m_iHappinessPerReligion += trait->GetHappinessPerReligion();
			m_iUnresearchedTechBonusFromKills += trait->GetUnresearchedTechBonusFromKills();
			m_iExtraFoundedCityTerritoryClaimRange += trait->GetExtraFoundedCityTerritoryClaimRange();
			m_iFreeSocialPoliciesPerEra += trait->GetFreeSocialPoliciesPerEra();
			m_iNumTradeRoutesModifier += trait->GetNumTradeRoutesModifier();
			m_iTradeRouteResourceModifier += trait->GetTradeRouteResourceModifier();
			m_iUniqueLuxuryCities += trait->GetUniqueLuxuryCities();
			m_iUniqueLuxuryQuantity	+= trait->GetUniqueLuxuryQuantity();
			m_iWorkerSpeedModifier += trait->GetWorkerSpeedModifier();
			m_iAfraidMinorPerTurnInfluence += trait->GetAfraidMinorPerTurnInfluence();
			m_iLandTradeRouteRangeBonus += trait->GetLandTradeRouteRangeBonus();
			m_iTradeReligionModifier += trait->GetTradeReligionModifier();
			m_iTradeBuildingModifier += trait->GetTradeBuildingModifier();

			if(trait->IsFightWellDamaged())
			{
				m_bFightWellDamaged = true;

				int iWoundedUnitDamageMod =         GC.getTRAIT_WOUNDED_DAMAGE_MOD();
				m_pPlayer->ChangeWoundedUnitDamageMod(iWoundedUnitDamageMod);
			}
			if(trait->IsMoveFriendlyWoodsAsRoad())
			{
				m_bMoveFriendlyWoodsAsRoad = true;
			}
			if(trait->IsFasterAlongRiver())
			{
				m_bFasterAlongRiver = true;
			}
			if(trait->IsFasterInHills())
			{
				m_bFasterInHills = true;
			}
			if(trait->IsEmbarkedAllWater())
			{
				m_bEmbarkedAllWater = true;
			}
			if(trait->IsEmbarkedToLandFlatCost())
			{
				m_bEmbarkedToLandFlatCost = true;
			}
			if(trait->IsCiviliansEmbarkOneMove())
			{
				m_bCiviliansEmbarkOneMove = true;
			}
			if(trait->IsNoHillsImprovementMaintenance())
			{
				m_bNoHillsImprovementMaintenance = true;
			}
			if(trait->IsTechBoostFromCapitalScienceBuildings())
			{
				m_bTechBoostFromCapitalScienceBuildings = true;
			}
			if(trait->IsStaysAliveZeroCities())
			{
				m_bStaysAliveZeroCities = true;
			}
			if(trait->IsFaithFromUnimprovedForest())
			{
				m_bFaithFromUnimprovedForest = true;
			}
#ifdef NQ_UNIT_IMMUNE_TO_PLUNDER_FROM_TRAIT
			if(trait->IsSeaTradeRoutesArePlunderImmune())
			{
				m_bSeaTradeRoutesArePlunderImmune = true;
			}
#endif

			if(trait->IsEarnsGreatPersonOnSlotOrGuild())
			{
				m_bEarnsGreatPersonOnSlotOrGuild = true;
			}

			if(trait->IsBonusReligiousBelief())
			{
				m_bBonusReligiousBelief = true;
			}
			if(trait->IsAbleToAnnexCityStates())
			{
				m_bAbleToAnnexCityStates = true;
			}
			if(trait->IsCrossesMountainsAfterGreatGeneral())
			{
				m_bCrossesMountainsAfterGreatGeneral = true;
			}
			if(trait->IsMayaCalendarBonuses())
			{
				m_bMayaCalendarBonuses = true;
			}
			if (trait->IsNoAnnexing())
			{
				m_bNoAnnexing = true;
			}
			if (trait->IsTechFromCityConquer())
			{
				m_bTechFromCityConquer = true;
			}
			if (trait->IsUniqueLuxuryRequiresNewArea())
			{
				m_bUniqueLuxuryRequiresNewArea = true;
			}
			if (trait->IsRiverTradeRoad())
			{
				m_bRiverTradeRoad = true;
			}
			if (trait->IsAngerFreeIntrusionOfCityStates())
			{
				m_bAngerFreeIntrusionOfCityStates = true;
			}

			for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
			{
				if(trait->GetExtraYieldThreshold(iYield) > m_iExtraYieldThreshold[iYield])
				{
					m_iExtraYieldThreshold[iYield] = trait->GetExtraYieldThreshold(iYield);
				}
				m_iFreeCityYield[iYield] = trait->GetYieldChange(iYield);
				m_iYieldChangeStrategicResources[iYield] = trait->GetYieldChangeStrategicResources(iYield);
				m_iYieldChangeLuxuryResources[iYield] = trait->GetYieldChangeLuxuryResources(iYield);
				m_iYieldChangeNaturalWonder[iYield] = trait->GetYieldChangeNaturalWonder(iYield);
#if !defined(TRADE_REFACTOR)
				m_iYieldChangePerTradePartner[iYield] = trait->GetYieldChangePerTradePartner(iYield);
				m_iYieldChangeIncomingTradeRoute[iYield] = trait->GetYieldChangeIncomingTradeRoute(iYield);
#else
				m_iTradePartnerYieldFlatBonusPerEra[iYield] = trait->GetTradePartnerYieldFlatBonusPerEra(iYield);
				for (int iDomain = 0; iDomain < NUM_DOMAIN_TYPES; iDomain++)
				{
					int iChange = trait->GetYieldChangePerTradePartnerByDomain((DomainTypes)iDomain, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiYieldChangePerTradePartnerByDomain[iDomain];
						yields[iYield] = (m_ppaaiYieldChangePerTradePartnerByDomain[iDomain][iYield] + iChange);
						m_ppaaiYieldChangePerTradePartnerByDomain[iDomain] = yields;
					}
				}
				for (int iConnectionLoop = 0; iConnectionLoop < NUM_TRADE_CONNECTION_TYPES; iConnectionLoop++)
				{

					int iChange = trait->GetTradeConnectionLandYieldChange((TradeConnectionType)iConnectionLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiTradeConnectionLandYieldChange[iConnectionLoop];
						yields[iYield] = (m_ppaaiTradeConnectionLandYieldChange[iConnectionLoop][iYield] + iChange);
						m_ppaaiTradeConnectionLandYieldChange[iConnectionLoop] = yields;
					}
					iChange = trait->GetTradeConnectionSeaYieldChange((TradeConnectionType)iConnectionLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiTradeConnectionSeaYieldChange[iConnectionLoop];
						yields[iYield] = (m_ppaaiTradeConnectionSeaYieldChange[iConnectionLoop][iYield] + iChange);
						m_ppaaiTradeConnectionSeaYieldChange[iConnectionLoop] = yields;
					}

					iChange = trait->GetIncomingTradeConnectionLandYieldChange((TradeConnectionType)iConnectionLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiIncomingTradeConnectionLandYieldChange[iConnectionLoop];
						yields[iYield] = (m_ppaaiIncomingTradeConnectionLandYieldChange[iConnectionLoop][iYield] + iChange);
						m_ppaaiIncomingTradeConnectionLandYieldChange[iConnectionLoop] = yields;
					}
					iChange = trait->GetIncomingTradeConnectionSeaYieldChange((TradeConnectionType)iConnectionLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiIncomingTradeConnectionSeaYieldChange[iConnectionLoop];
						yields[iYield] = (m_ppaaiIncomingTradeConnectionSeaYieldChange[iConnectionLoop][iYield] + iChange);
						m_ppaaiIncomingTradeConnectionSeaYieldChange[iConnectionLoop] = yields;
					}

					iChange = trait->GetTradeConnectionLandYieldModifier((TradeConnectionType)iConnectionLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiTradeConnectionLandYieldModifier[iConnectionLoop];
						yields[iYield] = (m_ppaaiTradeConnectionLandYieldModifier[iConnectionLoop][iYield] + iChange);
						m_ppaaiTradeConnectionLandYieldModifier[iConnectionLoop] = yields;
					}
					iChange = trait->GetTradeConnectionSeaYieldModifier((TradeConnectionType)iConnectionLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiTradeConnectionSeaYieldModifier[iConnectionLoop];
						yields[iYield] = (m_ppaaiTradeConnectionSeaYieldModifier[iConnectionLoop][iYield] + iChange);
						m_ppaaiTradeConnectionSeaYieldModifier[iConnectionLoop] = yields;
					}
				}
#endif
				m_iYieldRateModifier[iYield] = trait->GetYieldModifier(iYield);
#if defined(FULL_YIELD_FROM_KILLS)
				m_iYieldFromKills[iYield] = trait->GetYieldFromKills(iYield);
				m_iYieldFromKillsMax[iYield] = trait->GetYieldFromKillsMax(iYield);
#endif
#if defined(LEKMOD_EXPERIMENTAL_CHANGES)
				m_iWorldWonderYieldChange[iYield] = trait->GetWorldWonderYieldChanges(iYield);
#endif
#if defined(TRAITIFY)
				m_iPuppetYieldModifiers[iYield] = trait->GetPuppetYieldModifiers(iYield);
				m_iGoldenAgeYieldModifier[iYield] = trait->GetGoldenAgeYieldModifier(iYield);
#endif
#if defined(LEKMOD_CITY_YIELDS_TRAITS)
				m_aiCapitalYieldChange[iYield] += trait->GetCapitalYieldChange(iYield);
				m_aiCityYieldChange[iYield] = trait->GetCityYieldChange(iYield);
				for (int iEraLoop = 0; iEraLoop < GC.getNumEraInfos(); iEraLoop++)
				{
					int iChange = trait->GetCapitalEraYieldChange((EraTypes)iEraLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiCapitalEraYieldChange[iEraLoop];
						yields[iYield] = (m_ppaaiCapitalEraYieldChange[iEraLoop][iYield] + iChange);
						m_ppaaiCapitalEraYieldChange[iEraLoop] = yields;
					}
					iChange = trait->GetCityEraYieldChange((EraTypes)iEraLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiCityEraYieldChange[iEraLoop];
						yields[iYield] = (m_ppaaiCityEraYieldChange[iEraLoop][iYield] + iChange);
						m_ppaaiCityEraYieldChange[iEraLoop] = yields;
					}
				}
				for (int iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
				{
					int iChange = trait->GetCapitalTechYieldChange((TechTypes)iTechLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiCapitalTechYieldChange[iTechLoop];
						yields[iYield] = (m_ppaaiCapitalTechYieldChange[iTechLoop][iYield] + iChange);
						m_ppaaiCapitalTechYieldChange[iTechLoop] = yields;
					}
					iChange = trait->GetCityTechYieldChange((TechTypes)iTechLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiCityTechYieldChange[iTechLoop];
						yields[iYield] = (m_ppaaiCityTechYieldChange[iTechLoop][iYield] + iChange);
						m_ppaaiCityTechYieldChange[iTechLoop] = yields;
					}
				}
#endif
#ifdef AUI_WARNING_FIXES
				for (uint iFeatureLoop = 0; iFeatureLoop < GC.getNumFeatureInfos(); iFeatureLoop++)
#else
				for (int iFeatureLoop = 0; iFeatureLoop < GC.getNumFeatureInfos(); iFeatureLoop++)
#endif
				{
					int iChange = trait->GetUnimprovedFeatureYieldChanges((FeatureTypes)iFeatureLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiUnimprovedFeatureYieldChange[iFeatureLoop];
						yields[iYield] = (m_ppaaiUnimprovedFeatureYieldChange[iFeatureLoop][iYield] + iChange);
						m_ppaaiUnimprovedFeatureYieldChange[iFeatureLoop] = yields;
					}
#if defined(TRAITIFY)

					iChange = trait->GetFeatureYieldChanges((FeatureTypes)iFeatureLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiFeatureYieldChange[iFeatureLoop];
						yields[iYield] = (m_ppaaiFeatureYieldChange[iFeatureLoop][iYield] + iChange);
						m_ppaaiFeatureYieldChange[iFeatureLoop] = yields;
					}
#endif
				}
#if defined(TRAITIFY)

				for (int iTerrainLoop = 0; iTerrainLoop < GC.getNumTerrainInfos(); iTerrainLoop++)
				{
					int iChange = trait->GetTerrainYieldChanges((TerrainTypes)iTerrainLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiTerrainYieldChange[iTerrainLoop];
						yields[iYield] = (m_ppaaiTerrainYieldChange[iTerrainLoop][iYield] + iChange);
						m_ppaaiTerrainYieldChange[iTerrainLoop] = yields;
					}
				}

				for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
				{
					int iChange = trait->GetResourceYieldChanges((ResourceTypes)iResourceLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiResourceYieldChange[iResourceLoop];
						yields[iYield] = (m_ppaaiResourceYieldChange[iResourceLoop][iYield] + iChange);
						m_ppaaiResourceYieldChange[iResourceLoop] = yields;
					}
				}

				for (int iResourceClassLoop = 0; iResourceClassLoop < GC.getNumResourceClassInfos(); iResourceClassLoop++)
				{
					int iChange = trait->GetResourceClassYieldChanges((ResourceClassTypes)iResourceClassLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiResourceClassYieldChange[iResourceClassLoop];
						yields[iYield] = (m_ppaaiResourceClassYieldChange[iResourceClassLoop][iYield] + iChange);
						m_ppaaiResourceClassYieldChange[iResourceClassLoop] = yields;
					}
				}
				for (int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
				{
					int iChange = trait->GetBuildingClassYieldChanges((BuildingClassTypes)iBuildingClassLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiBuildingClassYieldChange[iBuildingClassLoop];
						yields[iYield] = (m_ppaaiBuildingClassYieldChange[iBuildingClassLoop][iYield] + iChange);
						m_ppaaiBuildingClassYieldChange[iBuildingClassLoop] = yields;
					}
				}
				for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
				{
					int iChange = trait->GetBuildingCostOverride((BuildingTypes)iBuildingLoop, (YieldTypes)iYield);
					if (iChange != 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiBuildingCostOverride[iBuildingLoop];
						yields[iYield] = (m_ppaaiBuildingCostOverride[iBuildingLoop][iYield] + iChange);
						m_ppaaiBuildingCostOverride[iBuildingLoop] = yields;
					}
				}
#endif
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
				m_iGreatWorkYieldChange[iYield] = trait->GetGreatWorkYieldChange((YieldTypes)iYield);
				for (int iGreatWorkLoop = 0; iGreatWorkLoop < GC.getNumGreatWorkClassInfos(); iGreatWorkLoop++)
				{
					int iChange = trait->GetGreatWorkClassYieldChange((GreatWorkClass)iGreatWorkLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiGreatWorkClassYieldChange[iGreatWorkLoop];
						yields[iYield] = (m_ppaaiGreatWorkClassYieldChange[iGreatWorkLoop][iYield] + iChange);
						m_ppaaiGreatWorkClassYieldChange[iGreatWorkLoop] = yields;
					}
				}
#endif
#ifdef AUI_WARNING_FIXES
				for (uint iImprovementLoop = 0; iImprovementLoop < GC.getNumImprovementInfos(); iImprovementLoop++)
#else
				for(int iImprovementLoop = 0; iImprovementLoop < GC.getNumImprovementInfos(); iImprovementLoop++)
#endif
				{
					int iChange = trait->GetImprovementYieldChanges((ImprovementTypes)iImprovementLoop, (YieldTypes)iYield);
					if(iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiImprovementYieldChange[iImprovementLoop];
						yields[iYield] = (m_ppaaiImprovementYieldChange[iImprovementLoop][iYield] + iChange);
						m_ppaaiImprovementYieldChange[iImprovementLoop] = yields;
					}
#if defined(TRAITIFY)
					iChange = trait->GetFreshWaterImprovementYieldChanges((ImprovementTypes)iImprovementLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiFreshWaterImprovementYieldChange[iImprovementLoop];
						yields[iYield] = (m_ppaaiFreshWaterImprovementYieldChange[iImprovementLoop][iYield] + iChange);
						m_ppaaiFreshWaterImprovementYieldChange[iImprovementLoop] = yields;
					}
					iChange = trait->GetNonFreshWaterImprovementYieldChanges((ImprovementTypes)iImprovementLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiNonFreshWaterImprovementYieldChange[iImprovementLoop];
						yields[iYield] = (m_ppaaiNonFreshWaterImprovementYieldChange[iImprovementLoop][iYield] + iChange);
						m_ppaaiNonFreshWaterImprovementYieldChange[iImprovementLoop] = yields;
					}
#endif
				}

#ifdef AUI_WARNING_FIXES
				for (uint iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
#else
				for(int iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
#endif
				{
					int iChange = trait->GetSpecialistYieldChanges((SpecialistTypes)iSpecialistLoop, (YieldTypes)iYield);
					if(iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiSpecialistYieldChange[iSpecialistLoop];
						yields[iYield] = (m_ppaaiSpecialistYieldChange[iSpecialistLoop][iYield] + iChange);
						m_ppaaiSpecialistYieldChange[iSpecialistLoop] = yields;
					}
				}
#ifdef LEK_TRAIT_SPECIALIST_YIELD_MAX_ONE
				for(int iSpecialistLoop = 0; iSpecialistLoop < 2; iSpecialistLoop++)

				{
					int iChange = trait->GetAnySpecificSpecialistYieldChanges((SpecialistTypes)iSpecialistLoop, (YieldTypes)iYield);
					if(iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiAnySpecificSpecialistYieldChange[iSpecialistLoop];
						yields[iYield] = (m_ppaaiAnySpecificSpecialistYieldChange[iSpecialistLoop][iYield] + iChange);
						m_ppaaiAnySpecificSpecialistYieldChange[iSpecialistLoop] = yields;
					}
				}
#endif
			}
			CvAssert(GC.getNumTerrainInfos() <= NUM_TERRAIN_TYPES);
#if defined(TRAITIFY)
			for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); iBuildingClass++)
			{
				m_aiBuildingClassHappiness[iBuildingClass] = trait->GetBuildingClassHappiness((BuildingClassTypes)iBuildingClass);
				m_aiBuildingClassGlobalHappiness[iBuildingClass] = trait->GetBuildingClassGlobalHappiness((BuildingClassTypes)iBuildingClass);
				m_abRemoveRequiredTerrain[iBuildingClass] = trait->IsBuildingClassRemoveRequiredTerrain((BuildingClassTypes)iBuildingClass);
			}
			for (int iRouteType = 0; iRouteType < NUM_ROUTE_TYPES; iRouteType++)
			{
				m_iRouteMovementChange[iRouteType] = trait->GetRouteMovementChange((RouteTypes)iRouteType);
			}
#endif
#ifdef AUI_WARNING_FIXES
			for (uint iTerrain = 0; iTerrain < GC.getNumTerrainInfos(); iTerrain++)
#else
			for(int iTerrain = 0; iTerrain < GC.getNumTerrainInfos(); iTerrain++)
#endif
			{
				m_iStrategicResourceQuantityModifier[iTerrain] = trait->GetStrategicResourceQuantityModifier(iTerrain);
			}

#ifdef AUI_WARNING_FIXES
			for (uint iResource = 0; iResource < GC.getNumResourceInfos(); iResource++)
#else
			for(int iResource = 0; iResource < GC.getNumResourceInfos(); iResource++)
#endif
			{
				m_aiResourceQuantityModifier[iResource] = trait->GetResourceQuantityModifier(iResource);
			}

#ifdef AUI_WARNING_FIXES
			for (uint iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); iUnitClass++)
#else
			for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); iUnitClass++)
#endif
			{
				m_abNoTrain[iUnitClass] = trait->NoTrain((UnitClassTypes)iUnitClass);
#if defined(TRAITIFY)
				m_abForceSpawnCapital[iUnitClass] = trait->IsUnitClassForceSpawnCapital((UnitClassTypes)iUnitClass);
#endif
			}


			for (int iImprovement = 0; iImprovement < GC.getNumImprovementInfos(); iImprovement++)
			{
#ifdef LEKMOD_TRAIT_NO_BUILD_IMPROVEMENTS
				m_abNoBuild[iImprovement] = trait->NoBuildImprovements((ImprovementTypes)iImprovement);
#if defined(v35_TRAITIFY)
				m_viBuildCompleteTileClaimRange[iImprovement] = trait->GetBuildCompleteTileClaimRange(iImprovement);
				m_viBuildCompleteTileStealRange[iImprovement] = trait->GetBuildCompleteTileStealRange(iImprovement);
#endif
#endif
			}


			for (int iMission = 0; iMission < GC.getNumMissionInfos(); iMission++)
			{
#if defined(LEKMOD_TRAIT_BAN_UNIT_MISSIONS)
				m_abBannedUnitMissions[iMission] = trait->IsBannedUnitMission((MissionTypes)iMission);
#endif
#if defined(v35_TRAITIFY)
				m_vbEmbarkedAllowedMissions[iMission] = trait->IsEmbarkedMissionAllowed((MissionTypes)iMission);
#endif
			}

#ifdef LEKMOD_BUILD_TIME_OVERRIDE

	for (int iBuild = 0; iBuild < GC.getNumBuildInfos(); iBuild++)
	{
		m_aiBuildTimeOverride[iBuild] = trait->GetBuildTimeOverrideVector(iBuild);
		m_aiBuildTimeOverrideResourceClassRequired[iBuild] = trait->GetBuildTimeOverrideResourceClassRequiredVector(iBuild);

	}


	typedef std::multimap<BuildTypes, std::pair<int, ResourceClassTypes>>::const_iterator it_type;
	const std::multimap<BuildTypes, std::pair<int, ResourceClassTypes>>& buildTimeOverrides = trait->GetBuildTimeOverridesMultimap();
	for (it_type it = buildTimeOverrides.begin(); it != buildTimeOverrides.end(); ++it)
	{
		m_BuildTimeOverrides.insert(*it);
	}
#endif

			FreeTraitUnit traitUnit;
#ifdef AUI_WARNING_FIXES
			traitUnit.m_iFreeUnit = (UnitClassTypes)trait->GetFreeUnitClassType();
#else
			traitUnit.m_iFreeUnit = (UnitTypes)trait->GetFreeUnitClassType();
#endif
			if(traitUnit.m_iFreeUnit != NO_UNITCLASS)
			{
				traitUnit.m_ePrereqTech = trait->GetFreeUnitPrereqTech();
				m_aFreeTraitUnits.push_back(traitUnit);
			}

			m_eCombatBonusImprovement = trait->GetCombatBonusImprovement();

#ifdef AUI_WARNING_FIXES
			for (uint jJ = 0; jJ < GC.getNumUnitCombatClassInfos(); jJ++)
#else
			int iNumUnitCombatClassInfos = GC.getNumUnitCombatClassInfos();
			for(int jJ= 0; jJ < iNumUnitCombatClassInfos; jJ++)
#endif
			{
				m_paiMovesChangeUnitCombat[jJ] += trait->GetMovesChangeUnitCombat(jJ);
				m_paiMaintenanceModifierUnitCombat[jJ] += trait->GetMaintenanceModifierUnitCombat(jJ);
#if defined(v35_TRAITIFY)
				m_viUnitCombatWorkRateChange[jJ] += trait->GetUnitCombatWorkRateChange(jJ);
#endif
			}

			const std::vector<FreeResourceCities>& vRules = trait->GetFreeResourceCities();
			for (size_t i = 0; i < vRules.size(); ++i)
			{
				m_vFreeResourceCities.push_back(vRules[i]);
			}
#if defined(LEKMOD_FREE_RESOURCE_CITY_GRANT)
			for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
			{
				m_aiFreeResourceCityYieldChange[iYield] += trait->GetFreeResourceCityYieldChange(iYield);
			}
#endif
#if defined(LEKMOD_GOLDEN_AGE_YIELD_THRESHOLD)
			const std::vector<GoldenAgeYieldThreshold>& vThresholds = trait->GetGoldenAgeYieldThresholds();
			for (size_t i = 0; i < vThresholds.size(); ++i)
			{
				m_sGoldenAgeYieldThreshold.push_back(vThresholds[i]);
			}
#endif
		}
	}
}


void CvPlayerTraits::Uninit()
{
	m_aiResourceQuantityModifier.clear();
	m_abNoTrain.clear();

#ifdef LEKMOD_TRAIT_NO_BUILD_IMPROVEMENTS
	m_abNoBuild.clear();
#endif
#if defined(LEKMOD_TRAIT_BAN_UNIT_MISSIONS)
	m_abBannedUnitMissions.clear();
#endif
#ifdef LEKMOD_BUILD_TIME_OVERRIDE
	m_aiBuildTimeOverride.clear();
#endif
	m_paiMovesChangeUnitCombat.clear();
	m_paiMaintenanceModifierUnitCombat.clear();
	m_ppaaiImprovementYieldChange.clear();
	m_ppaaiSpecialistYieldChange.clear();

#ifdef LEK_TRAIT_SPECIALIST_YIELD_MAX_ONE
	m_ppaaiAnySpecificSpecialistYieldChange.clear();
#endif

	m_ppaaiUnimprovedFeatureYieldChange.clear();
#if defined(TRAITIFY)
	m_abForceSpawnCapital.clear();
	m_abRemoveRequiredTerrain.clear();
	m_aiBuildingClassHappiness.clear();
	m_aiBuildingClassGlobalHappiness.clear();
	m_ppaaiBuildingClassYieldChange.clear();
	m_ppaaiFeatureYieldChange.clear();
	m_ppaaiTerrainYieldChange.clear();
	m_ppaaiResourceYieldChange.clear();
	m_ppaaiResourceClassYieldChange.clear();
	m_ppaaiBuildingCostOverride.clear();
	m_ppaaiFreshWaterImprovementYieldChange.clear();
	m_ppaaiNonFreshWaterImprovementYieldChange.clear();
#endif
#if defined(TRADE_REFACTOR)
	m_ppaaiYieldChangePerTradePartnerByDomain.clear();
	m_ppaaiTradeConnectionLandYieldChange.clear();
	m_ppaaiTradeConnectionSeaYieldChange.clear();
	m_ppaaiIncomingTradeConnectionLandYieldChange.clear();
	m_ppaaiIncomingTradeConnectionSeaYieldChange.clear();
	m_ppaaiTradeConnectionLandYieldModifier.clear();
	m_ppaaiTradeConnectionSeaYieldModifier.clear();
#endif
#if defined(LEKMOD_CITY_YIELDS_TRAITS)
	m_ppaaiCapitalEraYieldChange.clear();
	m_ppaaiCapitalTechYieldChange.clear();
	m_ppaaiCityEraYieldChange.clear();
	m_ppaaiCityTechYieldChange.clear();
#endif
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	m_ppaaiGreatWorkClassYieldChange.clear();
#endif
	m_vFreeResourceCities.clear();
#if defined(LEKMOD_GOLDEN_AGE_YIELD_THRESHOLD)
	m_sGoldenAgeYieldThreshold.clear();
#endif
#if defined(v35_TRAITIFY)
	m_viUnitCombatWorkRateChange.clear();
	m_viBuildCompleteTileClaimRange.clear();
	m_viBuildCompleteTileStealRange.clear();
	m_vbEmbarkedAllowedMissions.clear();
#endif
}


void CvPlayerTraits::Reset()
{
	Uninit();


	m_vLeaderHasTrait = std::vector<bool>( GC.getNumTraitInfos(), false );
	m_vPotentiallyActiveLeaderTraits.clear();

	m_iGreatPeopleRateModifier = 0;
	m_iGreatScientistRateModifier = 0;
	m_iGreatGeneralRateModifier = 0;
	m_iGreatGeneralExtraBonus = 0;
	m_iGreatPersonGiftInfluence = 0;
	m_iLevelExperienceModifier= 0;
	m_iMaxGlobalBuildingProductionModifier = 0;
	m_iMaxTeamBuildingProductionModifier = 0;
	m_iMaxPlayerBuildingProductionModifier = 0;
	m_iCityUnhappinessModifier = 0;
	m_iPopulationUnhappinessModifier = 0;
	m_iCityStateBonusModifier = 0;
	m_iCityStateFriendshipModifier = 0;
#ifdef LEKMOD_TRAIT_FIRST_PROPHET_COST_MOD
	m_iFirstProphetCostMod = 0;
#endif
	m_iCityStateCombatModifier = 0;
	m_iLandBarbarianConversionPercent = 0;
	m_iLandBarbarianConversionExtraUnits = 0;
	m_iSeaBarbarianConversionPercent = 0;
	m_iCapitalBuildingModifier = 0;
	m_iPlotBuyCostModifier = 0;
	m_iPlotCultureCostModifier = 0;
#ifdef NQ_SCIENCE_PER_GREAT_PERSON_BORN
	m_iSciencePerGreatPersonBorn = 0;
#endif
#ifdef NQ_NUM_TURNS_BEFORE_MINOR_ALLIES_REFUSE_BRIBES_FROM_TRAIT
	m_iNumTurnsBeforeMinorAlliesRefuseBribes = 0;
#endif
	m_iCultureFromKills = 0;
	m_iFaithFromKills = 0;
	m_iCityCultureBonus = 0;
	m_iCapitalThemingBonusModifier = 0;
	m_iPolicyCostModifier = 0;
	m_iCityConnectionTradeRouteChange = 0;
	m_iWonderProductionModifier = 0;
	m_iPlunderModifier = 0;
	m_iImprovementMaintenanceModifier = 0;
	m_iGoldenAgeDurationModifier = 0;
	m_iGoldenAgeMoveChange = 0;
	m_iGoldenAgeCombatModifier = 0;
	m_iGoldenAgeTourismModifier = 0;
	m_iGoldenAgeGreatArtistRateModifier = 0;
	m_iGoldenAgeGreatMusicianRateModifier = 0;
	m_iGoldenAgeGreatWriterRateModifier = 0;
	m_iExtraEmbarkMoves = 0;
	m_iNaturalWonderFirstFinderGold = 0;
	m_iNaturalWonderSubsequentFinderGold = 0;
#if defined(TRAITIFY)
	m_bHalfMoreSpecialistUnhappiness = false;

	m_iNumExtraLeagueVotes = 0;
	m_iNumTradeRouteBonus = 0;
	m_iMinorFriendshipMinimum = 0;
	m_iGreatEngineerRateModifier = 0;
	m_iGreatMerchantRateModifier = 0;
	m_iMinorBullyModifier = 0;
	m_iInternationalRouteGrowthModifier = 0;
	m_iLocalHappinessPerCity = 0;
	m_iGlobalHappinessPerCity = 0;
	m_iUnhappinessModifierForPuppetedCities = 0;
	m_iExtraPopulation = 0;
	m_iFaithCostModifier = 0; 
	m_iIdeologyPressureUnhappinessModifier = 0;
	m_iForeignRelgionPressureModifier = 0;
	m_iFriendlyLandsCitizenMoveChange = 0;
#endif
#if defined(v35_TRAITIFY)
	m_iEmbarkedUnitFullStrengthCount = 0;
	m_iCityStateUnitGiftExtraExperience = 0;
	m_iGreatGeneralSiegeBonus = 0;
#endif
#if defined(LEKMOD_v34)
	m_bReligionEnhanceReformation = false;
	m_iSelfReligiousPressureModifier = 0;
#endif

	m_iNaturalWonderFirstFinderFaith = 0;
	m_iNaturalWonderSubsequentFinderFaith = 0;

	m_iNaturalWonderYieldModifier = 0;
	m_iNaturalWonderHappinessModifier = 0;
	m_iNearbyImprovementCombatBonus = 0;
	m_iNearbyImprovementBonusRange = 0;
	m_iCultureBuildingYieldChange = 0;
	m_iCombatBonusVsHigherTech = 0;
	m_iCombatBonusVsLargerCiv = 0;
	m_iLandUnitMaintenanceModifier = 0;
	m_iNavalUnitMaintenanceModifier = 0;
	m_iRazeSpeedModifier = 0;
	m_iDOFGreatPersonModifier = 0;
	m_iLuxuryHappinessRetention = 0;
	m_iExtraHappinessPerLuxury = 0;
	m_iExtraSpies = 0;
	m_iHappinessPerReligion = 0;
	m_iUnresearchedTechBonusFromKills = 0;
	m_iExtraFoundedCityTerritoryClaimRange = 0;
	m_iFreeSocialPoliciesPerEra = 0;
	m_iNumTradeRoutesModifier = 0;
	m_iTradeRouteResourceModifier = 0;
	m_iUniqueLuxuryCities = 0;
	m_iUniqueLuxuryQuantity = 0;
	m_iWorkerSpeedModifier = 0;
	m_iAfraidMinorPerTurnInfluence = 0;
	m_iLandTradeRouteRangeBonus = 0;
	m_iTradeReligionModifier = 0;
	m_iTradeBuildingModifier = 0;

	m_bFightWellDamaged = false;
	m_bMoveFriendlyWoodsAsRoad = false;
	m_bFasterAlongRiver = false;
	m_bFasterInHills = false;
	m_bEmbarkedAllWater = false;
	m_bEmbarkedToLandFlatCost = false;
#ifdef LEKMOD_TRAIT_CIVILIAN_EMBARK_ONE_MOVE
	m_bCiviliansEmbarkOneMove = false;
#endif
	m_bNoHillsImprovementMaintenance = false;
	m_bTechBoostFromCapitalScienceBuildings = false;
	m_bStaysAliveZeroCities = false;
	m_bFaithFromUnimprovedForest = false;
#ifdef NQ_UNIT_IMMUNE_TO_PLUNDER_FROM_TRAIT
	m_bSeaTradeRoutesArePlunderImmune = false;
#endif
	

	m_bEarnsGreatPersonOnSlotOrGuild = false;
	m_bHasBuiltWritersGuild = false;
	m_bHasBuiltArtistsGuild = false;
	m_bHasBuiltMusiciansGuild = false;

	
	m_bBonusReligiousBelief = false;
	m_bAbleToAnnexCityStates = false;
	m_bCrossesMountainsAfterGreatGeneral = false;
	m_bMayaCalendarBonuses = false;
	m_bNoAnnexing = false;
	m_bTechFromCityConquer = false;
	m_bUniqueLuxuryRequiresNewArea = false;
	m_bRiverTradeRoad = false;
	m_bAngerFreeIntrusionOfCityStates = false;

	m_eCampGuardType = NO_UNIT;
	m_eCombatBonusImprovement = NO_IMPROVEMENT;

	m_ppaaiImprovementYieldChange.clear();
	m_ppaaiImprovementYieldChange.resize(GC.getNumImprovementInfos());
	m_ppaaiSpecialistYieldChange.clear();
#ifdef LEK_TRAIT_SPECIALIST_YIELD_MAX_ONE
	m_ppaaiAnySpecificSpecialistYieldChange.clear();
#endif
	m_ppaaiSpecialistYieldChange.resize(GC.getNumSpecialistInfos());
#ifdef LEK_TRAIT_SPECIALIST_YIELD_MAX_ONE
	m_ppaaiAnySpecificSpecialistYieldChange.resize(GC.getNumSpecialistInfos());
#endif

	m_ppaaiUnimprovedFeatureYieldChange.clear();
	m_ppaaiUnimprovedFeatureYieldChange.resize(GC.getNumFeatureInfos());
#if defined(TRAITIFY)
	m_abForceSpawnCapital.clear();
	m_abForceSpawnCapital.resize(GC.getNumUnitClassInfos());
	m_abRemoveRequiredTerrain.clear();
	m_abRemoveRequiredTerrain.resize(GC.getNumBuildingClassInfos());
	m_aiBuildingClassHappiness.clear();
	m_aiBuildingClassHappiness.resize(GC.getNumBuildingClassInfos());
	m_aiBuildingClassGlobalHappiness.clear();
	m_aiBuildingClassGlobalHappiness.resize(GC.getNumBuildingClassInfos());
	m_ppaaiBuildingClassYieldChange.clear();
	m_ppaaiBuildingClassYieldChange.resize(GC.getNumBuildingClassInfos());
	m_ppaaiFeatureYieldChange.clear();
	m_ppaaiFeatureYieldChange.resize(GC.getNumFeatureInfos());
	m_ppaaiTerrainYieldChange.clear();
	m_ppaaiTerrainYieldChange.resize(GC.getNumTerrainInfos());
	m_ppaaiResourceYieldChange.clear();
	m_ppaaiResourceYieldChange.resize(GC.getNumResourceInfos());
	m_ppaaiResourceClassYieldChange.clear();
	m_ppaaiResourceClassYieldChange.resize(GC.getNumResourceClassInfos());
	m_ppaaiBuildingCostOverride.clear();
	m_ppaaiBuildingCostOverride.resize(GC.getNumBuildingInfos());
	m_ppaaiFreshWaterImprovementYieldChange.clear();
	m_ppaaiFreshWaterImprovementYieldChange.resize(GC.getNumImprovementInfos());
	m_ppaaiNonFreshWaterImprovementYieldChange.clear();
	m_ppaaiNonFreshWaterImprovementYieldChange.resize(GC.getNumImprovementInfos());
#endif
#if defined(v35_TRAITIFY)
	m_viUnitCombatWorkRateChange.clear();
	m_viUnitCombatWorkRateChange.resize(GC.getNumUnitCombatClassInfos());
	m_viBuildCompleteTileClaimRange.clear();
	m_viBuildCompleteTileClaimRange.resize(GC.getNumImprovementInfos());
	m_viBuildCompleteTileStealRange.clear();
	m_viBuildCompleteTileStealRange.resize(GC.getNumImprovementInfos());
	m_vbEmbarkedAllowedMissions.clear();
	m_vbEmbarkedAllowedMissions.resize(GC.getNumMissionInfos());
#endif
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	m_ppaaiGreatWorkClassYieldChange.clear();
	m_ppaaiGreatWorkClassYieldChange.resize(GC.getNumGreatWorkClassInfos());
#endif
#if defined(TRADE_REFACTOR)
	m_ppaaiTradeConnectionLandYieldChange.clear();
	m_ppaaiTradeConnectionLandYieldChange.resize(NUM_TRADE_CONNECTION_TYPES);
	m_ppaaiTradeConnectionSeaYieldChange.clear();
	m_ppaaiTradeConnectionSeaYieldChange.resize(NUM_TRADE_CONNECTION_TYPES);
	m_ppaaiYieldChangePerTradePartnerByDomain.clear();
	m_ppaaiYieldChangePerTradePartnerByDomain.resize(NUM_DOMAIN_TYPES);
	m_ppaaiIncomingTradeConnectionLandYieldChange.clear();
	m_ppaaiIncomingTradeConnectionLandYieldChange.resize(NUM_TRADE_CONNECTION_TYPES);
	m_ppaaiIncomingTradeConnectionSeaYieldChange.clear();
	m_ppaaiIncomingTradeConnectionSeaYieldChange.resize(NUM_TRADE_CONNECTION_TYPES);
	m_ppaaiTradeConnectionLandYieldModifier.clear();
	m_ppaaiTradeConnectionLandYieldModifier.resize(NUM_TRADE_CONNECTION_TYPES);
	m_ppaaiTradeConnectionSeaYieldModifier.clear();
	m_ppaaiTradeConnectionSeaYieldModifier.resize(NUM_TRADE_CONNECTION_TYPES);
#endif
#if defined(LEKMOD_CITY_YIELDS_TRAITS)
	m_ppaaiCapitalEraYieldChange.clear();
	m_ppaaiCapitalEraYieldChange.resize(GC.getNumEraInfos());
	m_ppaaiCapitalTechYieldChange.clear();
	m_ppaaiCapitalTechYieldChange.resize(GC.getNumTechInfos());
	m_ppaaiCityEraYieldChange.clear();
	m_ppaaiCityEraYieldChange.resize(GC.getNumEraInfos());
	m_ppaaiCityTechYieldChange.clear();
	m_ppaaiCityTechYieldChange.resize(GC.getNumTechInfos());
#endif
	
	Firaxis::Array< int, NUM_YIELD_TYPES > yield;
	for(unsigned int j = 0; j < NUM_YIELD_TYPES; ++j)
	{
		yield[j] = 0;
	}

	for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		m_iExtraYieldThreshold[iYield] = 0;
		m_iFreeCityYield[iYield] = 0;
		m_iYieldChangeStrategicResources[iYield] = 0;
		m_iYieldChangeLuxuryResources[iYield] = 0;
		m_iYieldChangeNaturalWonder[iYield] = 0;
#if !defined(TRADE_REFACTOR)
		m_iYieldChangePerTradePartner[iYield] = 0;
		m_iYieldChangeIncomingTradeRoute[iYield] = 0;
#else
		m_iTradePartnerYieldFlatBonusPerEra[iYield] = 0;
		for (uint iDomain = 0; iDomain < NUM_DOMAIN_TYPES; iDomain++)
		{
			m_ppaaiYieldChangePerTradePartnerByDomain[iDomain] = yield;
		}
		for (uint iConnection = 0; iConnection < NUM_TRADE_CONNECTION_TYPES; iConnection++)
		{
			m_ppaaiTradeConnectionLandYieldChange[iConnection] = yield;
			m_ppaaiTradeConnectionSeaYieldChange[iConnection] = yield;
			m_ppaaiIncomingTradeConnectionLandYieldChange[iConnection] = yield;
			m_ppaaiIncomingTradeConnectionSeaYieldChange[iConnection] = yield;
			m_ppaaiTradeConnectionLandYieldModifier[iConnection] = yield;
			m_ppaaiTradeConnectionSeaYieldModifier[iConnection] = yield;
		}
#endif
		m_iYieldRateModifier[iYield] = 0;
#if defined(FULL_YIELD_FROM_KILLS)
		m_iYieldFromKills[iYield] = 0;
		m_iYieldFromKillsMax[iYield] = 0;
#endif
#if defined(LEKMOD_EXPERIMENTAL_CHANGES)
		m_iWorldWonderYieldChange[iYield] = 0;
#endif
#if defined(TRAITIFY)
		m_iPuppetYieldModifiers[iYield] = 0;
		m_iGoldenAgeYieldModifier[iYield] = 0;
#endif

#ifdef AUI_WARNING_FIXES
		for (uint iImprovement = 0; iImprovement < GC.getNumImprovementInfos(); iImprovement++)
		{
			m_ppaaiImprovementYieldChange[iImprovement] = yield;
		}
		for (uint iSpecialist = 0; iSpecialist < GC.getNumSpecialistInfos(); iSpecialist++)
		{
			m_ppaaiSpecialistYieldChange[iSpecialist] = yield;
		}
		for (uint iFeature = 0; iFeature < GC.getNumFeatureInfos(); iFeature++)
#else
		for(int iImprovement = 0; iImprovement < GC.getNumImprovementInfos(); iImprovement++)
		{
			m_ppaaiImprovementYieldChange[iImprovement] = yield;
#if defined(TRAITIFY)
			m_ppaaiFreshWaterImprovementYieldChange[iImprovement] = yield;
			m_ppaaiNonFreshWaterImprovementYieldChange[iImprovement] = yield;
#endif
		}
		for(int iSpecialist = 0; iSpecialist < GC.getNumSpecialistInfos(); iSpecialist++)
		{
			m_ppaaiSpecialistYieldChange[iSpecialist] = yield;
		}
#ifdef LEK_TRAIT_SPECIALIST_YIELD_MAX_ONE
		for(int iSpecialist = 0; iSpecialist < 2; iSpecialist++)
		{
			m_ppaaiAnySpecificSpecialistYieldChange[iSpecialist] = yield;
		}
#endif
#if defined(LEKMOD_CITY_YIELDS_TRAITS)
		m_aiCapitalYieldChange[iYield] = 0;
		m_aiCityYieldChange[iYield] = 0;
		for (int iEra = 0; iEra < GC.getNumEraInfos(); iEra++)
		{
			m_ppaaiCapitalEraYieldChange[iEra] = yield;
			m_ppaaiCityEraYieldChange[iEra] = yield;
		}
		for (int iTech = 0; iTech < GC.getNumTechInfos(); iTech++)
		{
			m_ppaaiCapitalTechYieldChange[iTech] = yield;
			m_ppaaiCityTechYieldChange[iTech] = yield;
		}
#endif
#if defined(LEKMOD_FREE_RESOURCE_CITY_GRANT)
		m_aiFreeResourceCityYieldChange[iYield] = 0;
#endif
		for (int iFeature = 0; iFeature < GC.getNumFeatureInfos(); iFeature++)
#endif
		{
			m_ppaaiUnimprovedFeatureYieldChange[iFeature] = yield;
#if !defined(TRAITIFY)
		}
#else 
			m_ppaaiFeatureYieldChange[iFeature] = yield;
		}
#endif
#if defined(TRAITIFY)
		for (int iTerrain = 0; iTerrain < GC.getNumTerrainInfos(); iTerrain++)
		{
			m_ppaaiTerrainYieldChange[iTerrain] = yield;
		}
		for (int iResource = 0; iResource < GC.getNumResourceInfos(); iResource++)
		{
			m_ppaaiResourceYieldChange[iResource] = yield;
		}
		for (int iResourceClass = 0; iResourceClass < GC.getNumResourceClassInfos(); iResourceClass++)
		{
			m_ppaaiResourceClassYieldChange[iResourceClass] = yield;
		}
		for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); iBuildingClass++)
		{
			m_ppaaiBuildingClassYieldChange[iBuildingClass] = yield;
		}
		for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); iBuilding++)
		{
			m_ppaaiBuildingCostOverride[iBuilding] = yield;
		}
#endif
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
		m_iGreatWorkYieldChange[iYield] = 0;
		for (int iGreatWorkClass = 0; iGreatWorkClass < GC.getNumGreatWorkClassInfos(); iGreatWorkClass++)
		{
			m_ppaaiGreatWorkClassYieldChange[iGreatWorkClass] = yield;
		}
#endif
	}

#ifdef AUI_WARNING_FIXES
	for (uint iTerrain = 0; iTerrain < GC.getNumTerrainInfos(); iTerrain++)
#else
	for(int iTerrain = 0; iTerrain < GC.getNumTerrainInfos(); iTerrain++)
#endif
	{
		m_iStrategicResourceQuantityModifier[iTerrain] = 0;
	}
#if defined(TRAITIFY)
	for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); iBuildingClass++)
	{
		m_aiBuildingClassHappiness[iBuildingClass] = 0;
		m_aiBuildingClassGlobalHappiness[iBuildingClass] = 0;
		m_abRemoveRequiredTerrain[iBuildingClass] = false;
	}
	for (int iRouteType = 0; iRouteType < NUM_ROUTE_TYPES; iRouteType++)
	{
		m_iRouteMovementChange[iRouteType] = 0;
	}
#endif
	m_aiResourceQuantityModifier.clear();
	m_aiResourceQuantityModifier.resize(GC.getNumResourceInfos());

#ifdef AUI_WARNING_FIXES
	for (uint iResource = 0; iResource < GC.getNumResourceInfos(); iResource++)
#else
	for(int iResource = 0; iResource < GC.getNumResourceInfos(); iResource++)
#endif
	{
		m_aiResourceQuantityModifier[iResource] = 0;
	}

	m_abNoTrain.clear();
	m_abNoTrain.resize(GC.getNumUnitClassInfos());



#ifdef AUI_WARNING_FIXES
	for (uint iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); iUnitClass++)
#else
	for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); iUnitClass++)
#endif
	{
		m_abNoTrain[iUnitClass] = false;
#if defined(TRAITIFY)
		m_abForceSpawnCapital[iUnitClass] = false;
#endif
	}

#ifdef LEKMOD_TRAIT_NO_BUILD_IMPROVEMENTS
	m_abNoBuild.clear();
	m_abNoBuild.resize(GC.getNumImprovementInfos());

	for (int iImprovement = 0; iImprovement < GC.getNumImprovementInfos(); iImprovement++)
	{
		m_abNoBuild[iImprovement] = false;
#endif
#if defined(v35_TRAITIFY)
		m_viBuildCompleteTileClaimRange[iImprovement] = 0;
		m_viBuildCompleteTileStealRange[iImprovement] = 0;
#endif
	}

#if defined(LEKMOD_TRAIT_BAN_UNIT_MISSIONS)
	m_abBannedUnitMissions.clear();
	m_abBannedUnitMissions.resize(GC.getNumMissionInfos());
	
	for (int iMission = 0; iMission < GC.getNumMissionInfos(); iMission++)
	{
		m_abBannedUnitMissions[iMission] = false;
#else
	for (int iMission = 0; iMission < GC.getNumMissionInfos(); iMission++)
	{
#endif
#if defined(v35_TRAITIFY)
		m_vbEmbarkedAllowedMissions[iMission] = false;
#endif
	}

#ifdef LEKMOD_BUILD_TIME_OVERRIDE
	m_aiBuildTimeOverride.clear();
	m_aiBuildTimeOverrideResourceClassRequired.clear();
	m_BuildTimeOverrides.clear();


	m_aiBuildTimeOverride.resize(GC.getNumBuildInfos(), -1);
	m_aiBuildTimeOverrideResourceClassRequired.resize(GC.getNumBuildInfos(), NO_RESOURCECLASS);
#endif

	m_aFreeTraitUnits.clear();

#ifdef AUI_WARNING_FIXES
	uint iNumUnitCombatClassInfos = GC.getNumUnitCombatClassInfos();
	CvAssertMsg((0 < iNumUnitCombatClassInfos), "GC.getNumUnitCombatClassInfos() is not greater than zero but an array is being allocated in CvPlayerTraits::Reset");
	m_paiMovesChangeUnitCombat.clear();
	m_paiMaintenanceModifierUnitCombat.clear();
	m_paiMovesChangeUnitCombat.resize(iNumUnitCombatClassInfos);
	m_paiMaintenanceModifierUnitCombat.resize(iNumUnitCombatClassInfos);
	for (uint iI = 0; iI < iNumUnitCombatClassInfos; iI++)
#else
	int iNumUnitCombatClassInfos = GC.getNumUnitCombatClassInfos();
	CvAssertMsg((0 < iNumUnitCombatClassInfos),  "GC.getNumUnitCombatClassInfos() is not greater than zero but an array is being allocated in CvPlayerTraits::Reset");
	m_paiMovesChangeUnitCombat.clear();
	m_paiMaintenanceModifierUnitCombat.clear();
	m_paiMovesChangeUnitCombat.resize(iNumUnitCombatClassInfos);
	m_paiMaintenanceModifierUnitCombat.resize(iNumUnitCombatClassInfos);
	for(int iI = 0; iI < iNumUnitCombatClassInfos; iI++)
#endif
	{
		m_paiMovesChangeUnitCombat[iI] = 0;
		m_paiMaintenanceModifierUnitCombat[iI] = 0;
#if defined(v35_TRAITIFY)
		m_viUnitCombatWorkRateChange[iI] = 0;
#endif
	}

	m_vFreeResourceCities.clear();
#if defined(LEKMOD_FREE_RESOURCE_CITY_GRANT)
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		m_aiFreeResourceCityYieldChange[iYield] = 0;
	}
#endif
}


bool CvPlayerTraits::HasTrait(TraitTypes eTrait) const
{
	CvAssert(m_pPlayer);

	if(m_pPlayer != NULL)
	{
		CvAssertMsg((m_pPlayer->getLeaderType() >= 0), "getLeaderType() is less than zero");
		CvAssertMsg((eTrait >= 0), "eTrait is less than zero");
		return m_pPlayer->getLeaderInfo().hasTrait(eTrait) && !m_pTraits->GetEntry(eTrait)->IsObsoleteByTech(m_pPlayer->getTeam()) && m_pTraits->GetEntry(eTrait)->IsEnabledByTech(m_pPlayer->getTeam());
	}
	else
	{
		return false;
	}
}


bool CvPlayerTraits::WillGetUniqueLuxury(CvArea *pArea) const
{

	if (m_iUniqueLuxuryCities > m_iUniqueLuxuryCitiesPlaced)
	{

		if (m_pPlayer->GetNumCitiesFounded() == 0)
		{
			return false;
		}


		int iArea = pArea->GetID();


		if (m_bUniqueLuxuryRequiresNewArea)
		{








			CvPlot *pOriginalCapitalPlot = GC.getMap().plot(m_pPlayer->GetOriginalCapitalX(), m_pPlayer->GetOriginalCapitalY());
			if (pOriginalCapitalPlot)
			{
				if (pOriginalCapitalPlot->getArea() == iArea)
				{
					return false;
				}
			}


			if (std::find (m_aUniqueLuxuryAreas.begin(), m_aUniqueLuxuryAreas.end(), iArea) != m_aUniqueLuxuryAreas.end())
			{
				return false;
			}
		}

		int iNumUniqueResourcesGiven = m_aUniqueLuxuryAreas.size();


		int iNumUniquesFound = 0;
#ifdef AUI_WARNING_FIXES
		for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
		{
			ResourceTypes eResource = (ResourceTypes) iResourceLoop;
			CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
			if (pkResource != NULL && pkResource->GetRequiredCivilization() == m_pPlayer->getCivilizationType())
			{
				iNumUniquesFound++;
				if (iNumUniquesFound > iNumUniqueResourcesGiven)
				{
					return true;
				}
			}
		}
	}

	return false;
}


int CvPlayerTraits::GetMovesChangeUnitCombat(const int unitCombatID) const
{
	CvAssertMsg(unitCombatID < GC.getNumUnitCombatClassInfos(),  "Invalid unitCombatID parameter in call to CvPlayerTraits::GetMovesChangeUnitCombat()");

	if(unitCombatID == NO_UNITCLASS)
	{
		return 0;
	}

	return m_paiMovesChangeUnitCombat[unitCombatID];
}


int CvPlayerTraits::GetMaintenanceModifierUnitCombat(const int unitCombatID) const
{
	CvAssertMsg(unitCombatID < GC.getNumUnitCombatClassInfos(),  "Invalid unitCombatID parameter in call to CvPlayerTraits::GetMaintenanceModifierUnitCombat()");

	if(unitCombatID == NO_UNITCLASS)
	{
		return 0;
	}

	return m_paiMaintenanceModifierUnitCombat[unitCombatID];
}


int CvPlayerTraits::GetImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYield) const
{
	CvAssertMsg(eImprovement < GC.getNumImprovementInfos(),  "Invalid eImprovement parameter in call to CvPlayerTraits::GetImprovementYieldChange()");
	CvAssertMsg(eYield < NUM_YIELD_TYPES,  "Invalid eYield parameter in call to CvPlayerTraits::GetImprovementYieldChange()");

	if(eImprovement == NO_IMPROVEMENT)
	{
		return 0;
	}

	return m_ppaaiImprovementYieldChange[(int)eImprovement][(int)eYield];
}


int CvPlayerTraits::GetSpecialistYieldChange(SpecialistTypes eSpecialist, YieldTypes eYield) const
{
	CvAssertMsg(eSpecialist < GC.getNumSpecialistInfos(),  "Invalid eSpecialist parameter in call to CvPlayerTraits::GetSpecialistYieldChange()");
	CvAssertMsg(eYield < NUM_YIELD_TYPES,  "Invalid eYield parameter in call to CvPlayerTraits::GetSpecialistYieldChange()");

	if(eSpecialist == NO_SPECIALIST)
	{
		return 0;
	}

	return m_ppaaiSpecialistYieldChange[(int)eSpecialist][(int)eYield];
}
#ifdef LEK_TRAIT_SPECIALIST_YIELD_MAX_ONE

int CvPlayerTraits::GetAnySpecificSpecialistYieldChange(SpecialistTypes eSpecialist, YieldTypes eYield) const
{
	CvAssertMsg(eSpecialist < GC.getNumSpecialistInfos(),  "Invalid eSpecialist parameter in call to CvPlayerTraits::GetAnySpecificSpecialistYieldChange()");
	CvAssertMsg(eYield < NUM_YIELD_TYPES,  "Invalid eYield parameter in call to CvPlayerTraits::GetAnySpecificSpecialistYieldChange()");

	if(eSpecialist == NO_SPECIALIST)
	{
		return 0;
	}

	return m_ppaaiAnySpecificSpecialistYieldChange[(int)eSpecialist][(int)eYield];
}
#endif

int CvPlayerTraits::GetUnimprovedFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYield) const
{
	CvAssertMsg(eFeature < GC.getNumFeatureInfos(),  "Invalid eFeature parameter in call to CvPlayerTraits::GetUnimprovedFeatureYieldChange()");
	CvAssertMsg(eYield < NUM_YIELD_TYPES,  "Invalid eYield parameter in call to CvPlayerTraits::GetUnimprovedFeatureYieldChange()");

	if(eFeature == NO_FEATURE)
	{
		return 0;
	}

	return m_ppaaiUnimprovedFeatureYieldChange[(int)eFeature][(int)eYield];
}
#if defined(v35_TRAITIFY)
bool CvPlayerTraits::IsBuildableByUnitCombat(BuildTypes eBuild, UnitCombatTypes eUnitCombat) const
{
	if (eBuild == NO_BUILD || eUnitCombat == NO_UNITCOMBAT)
	{
		return false;
	}
	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		const TraitTypes eTrait = static_cast<TraitTypes>(iI);
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(eTrait);
		if (pkTraitInfo)
		{
			if (HasTrait(eTrait))
			{
				if (pkTraitInfo->IsBuildableByUnitCombat(eBuild, eUnitCombat))
				{
					return true;
				}
			}
		}
	}

	return false;
}
#endif
bool CvPlayerTraits::HasFreePromotionUnitCombat(const int promotionID, const int unitCombatID) const
{
	CvAssertMsg((promotionID >= 0), "promotionID is less than zero");
#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumTraitInfos(); iI++)
#else
	for(int iI = 0; iI < GC.getNumTraitInfos(); iI++)
#endif
	{
		const TraitTypes eTrait = static_cast<TraitTypes>(iI);
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(eTrait);
		if(pkTraitInfo)
		{
			if(HasTrait(eTrait))
			{
				if(pkTraitInfo->IsFreePromotionUnitCombat(promotionID, unitCombatID))
				{
					return true;
				}
			}
		}
	}

	return false;
}


BuildingTypes CvPlayerTraits::GetFreeBuilding() const
{
	for(size_t iI = 0; iI < m_vPotentiallyActiveLeaderTraits.size(); iI++)
	{
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(m_vPotentiallyActiveLeaderTraits[iI]);
		if(pkTraitInfo && HasTrait(m_vPotentiallyActiveLeaderTraits[iI]))
		{
			if(pkTraitInfo->GetFreeBuilding()!=NO_BUILDING)
			{
				return pkTraitInfo->GetFreeBuilding();
			}
		}
	}


	return NO_BUILDING;
}

BuildingTypes CvPlayerTraits::GetFreeCapitalBuilding() const
{
	for(size_t iI = 0; iI < m_vPotentiallyActiveLeaderTraits.size(); iI++)
	{
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(m_vPotentiallyActiveLeaderTraits[iI]);
		if(pkTraitInfo && HasTrait(m_vPotentiallyActiveLeaderTraits[iI]))
		{
			if(pkTraitInfo->GetFreeCapitalBuilding()!=NO_BUILDING)
			{
				return pkTraitInfo->GetFreeCapitalBuilding();
			}
		}
	}

	return NO_BUILDING;
}



BuildingTypes CvPlayerTraits::GetFreeBuildingOnConquest() const
{
	for(size_t iI = 0; iI < m_vPotentiallyActiveLeaderTraits.size(); iI++)
	{
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(m_vPotentiallyActiveLeaderTraits[iI]);
		if(pkTraitInfo && HasTrait(m_vPotentiallyActiveLeaderTraits[iI]))
		{
			if(pkTraitInfo->GetFreeBuildingOnConquest()!=NO_BUILDING)
			{
				return pkTraitInfo->GetFreeBuildingOnConquest();
			}
		}
	}

	return NO_BUILDING;
}
#if defined(TRAITIFY)

bool CvPlayerTraits::IsBuildingClassRemoveRequiredTerrain(BuildingClassTypes eBuildingClass)
{
	if (eBuildingClass != NO_BUILDINGCLASS)
	{
		return m_abRemoveRequiredTerrain[eBuildingClass];
	}
	else
	{
		return false;
	}
}

bool CvPlayerTraits::IsUnitClassForceSpawnCapital(UnitClassTypes eUnitClass)
{
	return NO_UNITCLASS != eUnitClass ? m_abForceSpawnCapital[eUnitClass] : false;
}

int CvPlayerTraits::GetBuildingCostOverride(BuildingTypes eBuilding, YieldTypes eYieldType)
{
	CvAssertMsg(eBuilding < GC.getNumBuildingInfos(), "Invalid eBuilding parameter in call to CvPlayerTraits::GetBuildingCostOverride()");
	CvAssertMsg(eYieldType < NUM_YIELD_TYPES, "Invalid eYieldType parameter in call to CvPlayerTraits::GetBuildingCostOverride()");

	if (eBuilding == NO_BUILDING)
	{
		return 0;
	}

	return m_ppaaiBuildingCostOverride[(int)eBuilding][(int)eYieldType];
}

int CvPlayerTraits::GetFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYield) const
{
	CvAssertMsg(eFeature < GC.getNumFeatureInfos(), "Invalid eFeature parameter in call to CvPlayerTraits::GetFeatureYieldChange()");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "Invalid eYield parameter in call to CvPlayerTraits::GetFeatureYieldChange()");

	if (eFeature == NO_FEATURE)
	{
		return 0;
	}

	return m_ppaaiFeatureYieldChange[(int)eFeature][(int)eYield];
}

int CvPlayerTraits::GetTerrainYieldChange(TerrainTypes eTerrain, YieldTypes eYieldType)
{
	CvAssertMsg(eTerrain < GC.getNumTerrainInfos(), "Invalid eTerrain parameter in call to CvPlayerTraits::GetTerrainYieldChange()");
	CvAssertMsg(eYieldType < NUM_YIELD_TYPES, "Invalid eYield parameter in call to CvPlayerTraits::GetTerrainYieldChange()");

	if (eTerrain == NO_TERRAIN)
	{
		return 0;
	}

	return m_ppaaiTerrainYieldChange[(int)eTerrain][(int)eYieldType];
}

int CvPlayerTraits::GetResourceYieldChange(ResourceTypes eResource, YieldTypes eYieldType)
{
	CvAssertMsg(eResource < GC.getNumResourceInfos(), "Invalid eResource parameter in call to CvPlayerTraits::GetResourceYieldChange()");
	CvAssertMsg(eYieldType < NUM_YIELD_TYPES, "Invalid eYieldType parameter in call to CvPlayerTraits::GetResourceYieldChange()");

	if (eResource == NO_RESOURCE)
	{
		return 0;
	}

	return m_ppaaiResourceYieldChange[(int)eResource][(int)eYieldType];
}

int CvPlayerTraits::GetResourceClassYieldChange(ResourceClassTypes eResourceClass, YieldTypes eYieldType)
{
	CvAssertMsg(eResourceClass < GC.getNumResourceClassInfos(), "Invalid eResourceClass parameter in call to CvPlayerTraits::GetResourceClassYieldChange()");
	CvAssertMsg(eYieldType < NUM_YIELD_TYPES, "Invalid eYieldType parameter in call to CvPlayerTraits::GetResourceClassYieldChange()");

	if (eResourceClass == NO_RESOURCECLASS)
	{
		return 0;
	}

	return m_ppaaiResourceClassYieldChange[(int)eResourceClass][(int)eYieldType];
}

int CvPlayerTraits::GetBuildingClassHappiness(BuildingClassTypes eBuildingClass)
{
	CvAssertMsg(eBuildingClass < GC.getNumBuildingClassInfos(), "Invalid eBuildingClass parameter in call to CvPlayerTraits::GetBuildingClassHappiness()");
	if (eBuildingClass == NO_BUILDINGCLASS)
		return 0;

	return m_aiBuildingClassHappiness[(int)eBuildingClass];
}

int CvPlayerTraits::GetBuildingClassGlobalHappiness(BuildingClassTypes eBuildingClass)
{
	CvAssertMsg(eBuildingClass < GC.getNumBuildingClassInfos(), "Invalid eBuildingClass parameter in call to CvPlayerTraits::GetBuildingClassGlobalHappiness()");
	if (eBuildingClass == NO_BUILDINGCLASS)
	{
		return 0;
	}

	return m_aiBuildingClassGlobalHappiness[(int)eBuildingClass];
}
#if defined(LEKMOD_TRAIT_BUILDING_CLASS_PRODUCTION_MODIFIERS)
int CvPlayerTraits::GetBuildingClassProductionModifier(BuildingClassTypes eBuildingClass) const
{
	if (eBuildingClass == NO_BUILDINGCLASS)
		return 0;
	CvAssertMsg(eBuildingClass < GC.getNumBuildingClassInfos(), "Invalid eBuildingClass parameter in call to CvPlayerTraits::GetBuildingClassProductionModifier()");

	int rtnValue = 0;
#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < (uint)GC.getNumTraitInfos(); i++)
#else
	for (int i = 0; i < GC.getNumTraitInfos(); i++)
#endif
	{
		if (HasTrait((TraitTypes)i))
		{
			rtnValue += GC.getTraitInfo((TraitTypes)i)->GetBuildingClassProductionModifier((int)eBuildingClass);
		}
	}
	return rtnValue;
}
#endif

int CvPlayerTraits::GetBuildingClassYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYieldType)
{
	CvAssertMsg(eBuildingClass < GC.getNumBuildingClassInfos(), "Invalid eBuildingClass parameter in call to CvPlayerTraits::GetBuildingClassYieldChange()");
	CvAssertMsg(eYieldType < NUM_YIELD_TYPES, "Invalid eYieldType parameter in call to CvPlayerTraits::GetBuildingClassYieldChange()");
	if (eBuildingClass == NO_BUILDINGCLASS)
	{
		return 0;
	}

	return m_ppaaiBuildingClassYieldChange[(int)eBuildingClass][(int)eYieldType];
}

int CvPlayerTraits::GetFreshWaterImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYieldType)
{
	CvAssertMsg(eImprovement < GC.getNumImprovementInfos(), "Invalid eImprovement parameter in call to CvPlayerTraits::GetFreshWaterImprovementYieldChange()");
	CvAssertMsg(eYieldType < NUM_YIELD_TYPES, "Invalid eYieldType parameter in call to CvPlayerTraits::GetFreshWaterImprovementYieldChange()");
	if (eImprovement == NO_IMPROVEMENT)
	{
		return 0;
	}
	return m_ppaaiFreshWaterImprovementYieldChange[(int)eImprovement][(int)eYieldType];
}

int CvPlayerTraits::GetNonFreshWaterImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYieldType)
{
	CvAssertMsg(eImprovement < GC.getNumImprovementInfos(), "Invalid eImprovement parameter in call to CvPlayerTraits::GetNonFreshWaterImprovementYieldChange()");
	CvAssertMsg(eYieldType < NUM_YIELD_TYPES, "Invalid eYieldType parameter in call to CvPlayerTraits::GetNonFreshWaterImprovementYieldChange()");
	if (eImprovement == NO_IMPROVEMENT)
	{
		return 0;
	}
	return m_ppaaiNonFreshWaterImprovementYieldChange[(int)eImprovement][(int)eYieldType];
}
#if defined(LEKMOD_CITY_YIELDS_TRAITS)

UnitTypes CvPlayerTraits::GetYieldSettleUnit() const
{
	for (size_t iI = 0; iI < m_vPotentiallyActiveLeaderTraits.size(); iI++)
	{
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(m_vPotentiallyActiveLeaderTraits[iI]);
		if (pkTraitInfo && HasTrait(m_vPotentiallyActiveLeaderTraits[iI]))
		{
			if (pkTraitInfo->GetYieldSettleUnit() != NO_UNIT)
			{
				return pkTraitInfo->GetYieldSettleUnit();
			}
		}
	}

	return NO_UNIT;
}
int CvPlayerTraits::GetCapitalYieldChange(YieldTypes eYieldType)
{
	CvAssertMsg(eYieldType < NUM_YIELD_TYPES, "Invalid eYieldType parameter in call to CvPlayerTraits::GetCapitalYieldChange()");
	if (eYieldType == NO_YIELD)
	{
		return 0;
	}
	return m_aiCapitalYieldChange[(int)eYieldType];
}

int CvPlayerTraits::GetCapitalEraYieldChange(EraTypes eEra, YieldTypes eYield)
{
	CvAssertMsg(eEra < GC.getNumEraInfos(), "Invalid eEra parameter in call to CvPlayerTraits::GetCapitalEraYieldChange()");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "Invalid eYield parameter in call to CvPlayerTraits::GetCapitalEraYieldChange()");
	if (eEra == NO_ERA)
	{
		return 0;
	}

	return m_ppaaiCapitalEraYieldChange[(int)eEra][(int)eYield];
}

int CvPlayerTraits::GetCapitalTechYieldChange(TechTypes eTech, YieldTypes eYield)
{
	CvAssertMsg(eTech < GC.getNumTechInfos(), "Invalid eTech parameter in call to CvPlayerTraits::GetCapitalTechYieldChange()");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "Invalid eYield parameter in call to CvPlayerTraits::GetCapitalTechYieldChange()");
	if (eTech == NO_TECH)
	{
		return 0;
	}
	return m_ppaaiCapitalTechYieldChange[(int)eTech][(int)eYield];
}
int CvPlayerTraits::GetCityYieldChange(YieldTypes eYieldType)
{
	CvAssertMsg(eYieldType < NUM_YIELD_TYPES, "Invalid eYieldType parameter in call to CvPlayerTraits::GetCityYieldChange()");
	if (eYieldType == NO_YIELD)
	{
		return 0;
	}
	return m_aiCityYieldChange[(int)eYieldType];
}
#if defined(LEKMOD_FREE_RESOURCE_CITY_GRANT)
int CvPlayerTraits::GetFreeResourceCityYieldChange(YieldTypes eYield) const
{
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "Invalid eYield parameter in call to CvPlayerTraits::GetFreeResourceCityYieldChange()");
	if (eYield == NO_YIELD)
	{
		return 0;
	}
	return m_aiFreeResourceCityYieldChange[(int)eYield];
}
#endif

int CvPlayerTraits::GetCityTechYieldChange(TechTypes eTech, YieldTypes eYield)
{
	CvAssertMsg(eTech < GC.getNumTechInfos(), "Invalid eTech parameter in call to CvPlayerTraits::GetCityTechYieldChange()");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "Invalid eYield parameter in call to CvPlayerTraits::GetCityTechYieldChange()");
	if (eTech == NO_TECH)
	{
		return 0;
	}
	return m_ppaaiCityTechYieldChange[(int)eTech][(int)eYield];
}

int CvPlayerTraits::GetCityEraYieldChange(EraTypes eEra, YieldTypes eYield)
{
	CvAssertMsg(eEra < GC.getNumEraInfos(), "Invalid eEra parameter in call to CvPlayerTraits::GetCityEraYieldChange()");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "Invalid eYield parameter in call to CvPlayerTraits::GetCityEraYieldChange()");
	if (eEra == NO_ERA)
	{
		return 0;
	}
	return m_ppaaiCityEraYieldChange[(int)eEra][(int)eYield];
}
#endif

int CvPlayerTraits::GetYieldPerPopulation(YieldTypes eYieldType)
{
	int rtnValue = 0;
	for (int i = 0; i < GC.getNumTraitInfos(); i++)
	{
		if (HasTrait((TraitTypes)i))
		{
			rtnValue += GC.getTraitInfo((TraitTypes)i)->GetYieldPerPopulation(eYieldType);
		}
	}
	return rtnValue;
}

int CvPlayerTraits::GetYieldPerPopulationForeignReligion(YieldTypes eYieldType)
{
	int rtnValue = 0;
	for (int i = 0; i < GC.getNumTraitInfos(); i++)
	{
		if (HasTrait((TraitTypes)i))
		{
			rtnValue += GC.getTraitInfo((TraitTypes)i)->GetYieldPerPopulationForeignReligion(eYieldType);
		}
	}
	return rtnValue;
}
#endif
#if defined(TRADE_REFACTOR)
int CvPlayerTraits::GetTradeConnectionLandYieldChange(TradeConnectionType eTradeConnection, YieldTypes eYield) const
{
	CvAssertMsg(eTradeConnection < NUM_TRADE_CONNECTION_TYPES, "Invalid eTradeConnection parameter in call to CvPlayerTraits::GetTradeConnectionLandYieldChange()");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "Invalid eYield parameter in call to CvPlayerTraits::GetTradeConnectionLandYieldChange()");

	return eTradeConnection != NO_TECH ? m_ppaaiTradeConnectionLandYieldChange[(int)eTradeConnection][(int)eYield] : 0;
}
int CvPlayerTraits::GetTradeConnectionSeaYieldChange(TradeConnectionType eTradeConnection, YieldTypes eYield) const
{
	CvAssertMsg(eTradeConnection < NUM_TRADE_CONNECTION_TYPES, "Invalid eTradeConnection parameter in call to CvPlayerTraits::GetTradeConnectionSeaYieldChange()");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "Invalid eYield parameter in call to CvPlayerTraits::GetTradeConnectionSeaYieldChange()");
	return eTradeConnection != NO_TECH ? m_ppaaiTradeConnectionSeaYieldChange[(int)eTradeConnection][(int)eYield] : 0;
}
int CvPlayerTraits::GetYieldChangePerTradePartnerByDomain(DomainTypes eDomain, YieldTypes eYield) const
{
	CvAssertMsg(eDomain < NUM_DOMAIN_TYPES, "Invalid eDomain parameter in call to CvPlayerTraits::GetYieldChangePerTradePartnerByDomain()");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "Invalid eYield parameter in call to CvPlayerTraits::GetYieldChangePerTradePartnerByDomain()");
	return eDomain != NO_DOMAIN ? m_ppaaiYieldChangePerTradePartnerByDomain[(int)eDomain][(int)eYield] : 0;
}
int CvPlayerTraits::GetIncomingTradeConnectionLandYieldChange(TradeConnectionType eTradeConnection, YieldTypes eYield) const
{
	CvAssertMsg(eTradeConnection < NUM_TRADE_CONNECTION_TYPES, "Invalid eTradeConnection parameter in call to CvPlayerTraits::GetIncomingTradeConnectionLandYieldChange()");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "Invalid eYield parameter in call to CvPlayerTraits::GetIncomingTradeConnectionLandYieldChange()");
	return eTradeConnection != NO_TECH ? m_ppaaiIncomingTradeConnectionLandYieldChange[(int)eTradeConnection][(int)eYield] : 0;
}
int CvPlayerTraits::GetIncomingTradeConnectionSeaYieldChange(TradeConnectionType eTradeConnection, YieldTypes eYield) const
{
	CvAssertMsg(eTradeConnection < NUM_TRADE_CONNECTION_TYPES, "Invalid eTradeConnection parameter in call to CvPlayerTraits::GetIncomingTradeConnectionSeaYieldChange()");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "Invalid eYield parameter in call to CvPlayerTraits::GetIncomingTradeConnectionSeaYieldChange()");
	return eTradeConnection != NO_TECH ? m_ppaaiIncomingTradeConnectionSeaYieldChange[(int)eTradeConnection][(int)eYield] : 0;
}
int CvPlayerTraits::GetTradeConnectionLandYieldModifier(TradeConnectionType eTradeConnection, YieldTypes eYield) const
{
	CvAssertMsg(eTradeConnection < NUM_TRADE_CONNECTION_TYPES, "Invalid eTradeConnection parameter in call to CvPlayerTraits::GetTradeConnectionLandYieldModifier()");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "Invalid eYield parameter in call to CvPlayerTraits::GetTradeConnectionLandYieldModifier()");
	return eTradeConnection != NO_TECH ? m_ppaaiTradeConnectionLandYieldModifier[(int)eTradeConnection][(int)eYield] : 0;
}
int CvPlayerTraits::GetTradeConnectionSeaYieldModifier(TradeConnectionType eTradeConnection, YieldTypes eYield) const
{
	CvAssertMsg(eTradeConnection < NUM_TRADE_CONNECTION_TYPES, "Invalid eTradeConnection parameter in call to CvPlayerTraits::GetTradeConnectionSeaYieldModifier()");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "Invalid eYield parameter in call to CvPlayerTraits::GetTradeConnectionSeaYieldModifier()");
	return eTradeConnection != NO_TECH ? m_ppaaiTradeConnectionSeaYieldModifier[(int)eTradeConnection][(int)eYield] : 0;
}
#endif
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
int CvPlayerTraits::GetGreatWorkClassYieldChange(GreatWorkClass eGreatWorkClass, YieldTypes eYieldType)
{
	CvAssertMsg(eGreatWorkClass < GC.getNumGreatWorkClassInfos(), "Invalid eGreatWorkClass parameter in call to CvPlayerTraits::GetGreatWorkClassYieldChange()");
	CvAssertMsg(eYieldType < NUM_YIELD_TYPES, "Invalid eYieldType parameter in call to CvPlayerTraits::GetGreatWorkClassYieldChange()");
	if (eGreatWorkClass == NO_GREAT_WORK_CLASS)
	{
		return 0;
	}
	return m_ppaaiGreatWorkClassYieldChange[(int)eGreatWorkClass][(int)eYieldType];
}
#endif

void CvPlayerTraits::AddUniqueLuxuries(CvCity *pCity)
{

	if (m_iUniqueLuxuryCities > m_iUniqueLuxuryCitiesPlaced)
	{

		if (m_pPlayer->GetNumCitiesFounded() == 1)
		{
			return;
		}



		if (!pCity->isCoastal())
		{
			return;
		}


		int iArea = pCity->getArea();


		if (m_bUniqueLuxuryRequiresNewArea)
		{








			CvPlot *pOriginalCapitalPlot = GC.getMap().plot(m_pPlayer->GetOriginalCapitalX(), m_pPlayer->GetOriginalCapitalY());
			if (pOriginalCapitalPlot)
			{
				if (pOriginalCapitalPlot->getArea() == iArea)
				{
					return;
				}
			}


			if (std::find (m_aUniqueLuxuryAreas.begin(), m_aUniqueLuxuryAreas.end(), iArea) != m_aUniqueLuxuryAreas.end())
			{
				return;
			}
		}

		m_aUniqueLuxuryAreas.push_back(iArea);
		int iNumUniqueResourcesGiven = m_aUniqueLuxuryAreas.size();
		m_iUniqueLuxuryCitiesPlaced++;


		ResourceTypes eResourceToGive = NO_RESOURCE;
		int iNumUniquesFound = 0;
#ifdef AUI_WARNING_FIXES
		for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
		{
			ResourceTypes eResource = (ResourceTypes) iResourceLoop;
			CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
			if (pkResource != NULL && pkResource->GetRequiredCivilization() == m_pPlayer->getCivilizationType())
			{
				iNumUniquesFound++;
				if (iNumUniquesFound == iNumUniqueResourcesGiven)
				{
					eResourceToGive = eResource;
					break;
				}
			}
		}

		if (eResourceToGive != NO_RESOURCE)
		{
			pCity->plot()->setResourceType(NO_RESOURCE, 0, true);
			pCity->plot()->setResourceType(eResourceToGive, m_iUniqueLuxuryQuantity, true);
		}
	}
}


bool CvPlayerTraits::CheckForBarbarianConversion(CvPlot* pPlot)
{

	CvPlot* pAdjacentPlot;
	int iI;
	bool bRtnValue = false;

	if(pPlot->isWater() && GetSeaBarbarianConversionPercent() > 0)
	{
		for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

			if(pAdjacentPlot != NULL)
			{
				if(pAdjacentPlot->isWater())
				{
					UnitHandle pNavalUnit = pAdjacentPlot->getBestDefender(BARBARIAN_PLAYER);
					if(pNavalUnit)
					{
						if(ConvertBarbarianNavalUnit(pNavalUnit))
						{
							bRtnValue = true;
						}
					}
				}
			}
		}
	}

	else if(GetLandBarbarianConversionPercent() > 0 && pPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT() &&
	        m_eCampGuardType != NO_UNIT)
	{
		bRtnValue = ConvertBarbarianCamp(pPlot);
	}

	return bRtnValue;
}


int CvPlayerTraits::GetCapitalBuildingDiscount(BuildingTypes eBuilding)
{
	if(m_iCapitalBuildingModifier > 0)
	{

		CvCity* pCapital = m_pPlayer->getCapitalCity();
		if(pCapital)
		{
			if(pCapital->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				return m_iCapitalBuildingModifier;
			}
		}
	}
	return 0;
}



TechTypes CvPlayerTraits::GetFreeBuildingPrereqTech() const
{
	for(size_t iI = 0; iI < m_vPotentiallyActiveLeaderTraits.size(); iI++)
	{
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(m_vPotentiallyActiveLeaderTraits[iI]);
		if(pkTraitInfo && HasTrait(m_vPotentiallyActiveLeaderTraits[iI]) && pkTraitInfo->GetFreeBuildingPrereqTech())
			return pkTraitInfo->GetFreeBuildingPrereqTech();
	}

	return NO_TECH;
}

TechTypes CvPlayerTraits::GetCapitalFreeBuildingPrereqTech() const
{
	for(size_t iI = 0; iI < m_vPotentiallyActiveLeaderTraits.size(); iI++)
	{
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(m_vPotentiallyActiveLeaderTraits[iI]);
		if(pkTraitInfo && HasTrait(m_vPotentiallyActiveLeaderTraits[iI]) && pkTraitInfo->GetCapitalFreeBuildingPrereqTech())
			return pkTraitInfo->GetCapitalFreeBuildingPrereqTech();
	}

	return NO_TECH;
}



int CvPlayerTraits::GetFirstFreeUnit(TechTypes eTech)
{
	m_uiFreeUnitIndex = 0;
	m_eFreeUnitPrereqTech = eTech;

	while(m_uiFreeUnitIndex < m_aFreeTraitUnits.size())
	{
		if(eTech == m_aFreeTraitUnits[m_uiFreeUnitIndex].m_ePrereqTech)
		{
			return m_aFreeTraitUnits[m_uiFreeUnitIndex].m_iFreeUnit;
		}

		m_uiFreeUnitIndex++;
	}

	return NO_UNITCLASS;
}


int CvPlayerTraits::GetNextFreeUnit()
{
	m_uiFreeUnitIndex++;

	while(m_uiFreeUnitIndex < m_aFreeTraitUnits.size())
	{
		if(m_eFreeUnitPrereqTech == m_aFreeTraitUnits[m_uiFreeUnitIndex].m_ePrereqTech)
		{
			return m_aFreeTraitUnits[m_uiFreeUnitIndex].m_iFreeUnit;
		}

		m_uiFreeUnitIndex++;
	}

	return NO_UNITCLASS;
}


bool CvPlayerTraits::IsAbleToCrossMountains() const
{
	return (m_bCrossesMountainsAfterGreatGeneral                                                );
}

bool CvPlayerTraits::NoTrain(UnitClassTypes eUnitClassType)
{
	if (eUnitClassType != NO_UNITCLASS)
	{
		return m_abNoTrain[eUnitClassType];
	}
	else
	{
		return false;
	}
}

#ifdef LEKMOD_TRAIT_NO_BUILD_IMPROVEMENTS
bool CvPlayerTraits::NoBuild(ImprovementTypes eImprovement)
{
	if (eImprovement != NO_IMPROVEMENT)
	{
		return m_abNoBuild[eImprovement];
	}
	else
	{
		return false;
	}
}
#endif
#if defined(LEKMOD_TRAIT_BAN_UNIT_MISSIONS)
bool CvPlayerTraits::IsBannedUnitMission(MissionTypes eMission)
{
	return (eMission != NO_MISSION) ? m_abBannedUnitMissions[eMission] : false;
}
#endif
#ifdef LEKMOD_BUILD_TIME_OVERRIDE
int CvPlayerTraits::GetBuildTimeOverride(BuildTypes eBuild, ResourceClassTypes eResourceClass)
{
	CvAssertMsg(eBuild < GC.getNumBuildInfos(), "Invalid eBuild parameter in call to CvPlayerTraits::GetBuildTimeOverride()");

	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		TraitTypes eTrait = static_cast<TraitTypes>(iI);
		if (!HasTrait(eTrait))
			continue;

		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(eTrait);
		if (!pkTraitInfo)
			continue;

		int iOverride = pkTraitInfo->GetBuildTimeOverride(eBuild, eResourceClass);
		if (iOverride > -1)
		{
			return iOverride;
		}
	}

	return -1;
}
#endif



const float CALENDAR_START = -3112.3973f;
const float DAYS_IN_BAKTUN = 144000.0f;
const float DAYS_IN_KATUN = 7200.0f;
const float DAYS_IN_TUN = 360.0f;
const float DAYS_IN_WINAL = 20.0f;
const float DAYS_IN_YEAR = 365.242199f;


bool CvPlayerTraits::IsUsingMayaCalendar() const
{
	int iNumTraits = GC.getNumTraitInfos();
	for(int iI = 0; iI < iNumTraits; iI++)
	{
		const TraitTypes eTrait = static_cast<TraitTypes>(iI);
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(eTrait);
		if(pkTraitInfo)
		{
			if(pkTraitInfo->IsMayaCalendarBonuses())
			{
				if(HasTrait(eTrait))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}
	return false;
}


bool CvPlayerTraits::IsEndOfMayaLongCount()
{
	bool bRtnValue = false;

	if (!IsUsingMayaCalendar())
	{
		return bRtnValue;
	}

	ComputeMayaDate();

	if(m_iBaktunPreviousTurn + 1 == m_iBaktun)
	{
		bRtnValue = true;
	}

	m_iBaktunPreviousTurn = m_iBaktun;

	return bRtnValue;
}


CvString CvPlayerTraits::GetMayaCalendarString()
{
	CvString szRtnValue;
	ComputeMayaDate();
	szRtnValue.Format("%d.%d.%d.%d.%d", m_iBaktun, m_iKatun, m_iTun, m_iWinal, m_iKin);
	return szRtnValue;
}


CvString CvPlayerTraits::GetMayaCalendarLongString()
{
	CvString szRtnValue;
	ComputeMayaDate();
	szRtnValue = GetLocalizedText("TXT_KEY_MAYA_CALENDAR_LONG_STRING", m_iBaktun, m_iKatun, m_iTun, m_iWinal, m_iKin);
	return szRtnValue;
}


void CvPlayerTraits::ChooseMayaBoost()
{
	UnitTypes eDesiredGreatPerson = NO_UNIT;
	UnitTypes ePossibleGreatPerson;


	ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_PROPHET", true);
	if(GetUnitBaktun(ePossibleGreatPerson) == 0)
	{
		CvGameReligions* pReligions = GC.getGame().GetGameReligions();
		ReligionTypes eReligion = pReligions->GetReligionCreatedByPlayer(m_pPlayer->GetID());


		if(eReligion != NO_RELIGION)
		{
			const CvReligion* pMyReligion = pReligions->GetReligion(eReligion, m_pPlayer->GetID());
			if(!pMyReligion->m_bEnhanced)
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
		}


		else
		{
			if(pReligions->GetNumReligionsStillToFound() > 0)
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
		}
	}


	if(eDesiredGreatPerson == NO_UNIT)
	{
		ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_ENGINEER");
		if(GetUnitBaktun(ePossibleGreatPerson) == 0)
		{
			if(m_pPlayer->GetDiplomacyAI()->GetWonderCompetitiveness() >= 8 && GC.getGame().getGameTurn() <= (GC.getGame().getEstimateEndTurn() / 2))
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
		}
	}


	AIGrandStrategyTypes eVictoryStrategy = m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy();
	if(eDesiredGreatPerson == NO_UNIT)
	{
		ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_SCIENTIST");
		if(GetUnitBaktun(ePossibleGreatPerson) == 0)
		{
			if(eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_SPACESHIP"))
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
		}
	}
	if(eDesiredGreatPerson == NO_UNIT)
	{
		ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_ARTIST");
		if(GetUnitBaktun(ePossibleGreatPerson) == 0)
		{
			if(eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE"))
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
		}
	}
	if(eDesiredGreatPerson == NO_UNIT)
	{
		ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_MERCHANT");
		if(GetUnitBaktun(ePossibleGreatPerson) == 0)
		{
			if(eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_UNITED_NATIONS"))
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
		}
	}
	if(eDesiredGreatPerson == NO_UNIT)
	{
		ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_GREAT_GENERAL");
		if(GetUnitBaktun(ePossibleGreatPerson) == 0)
		{
			if(eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST"))
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
		}
	}


	if(eDesiredGreatPerson == NO_UNIT)
	{
		ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_PROPHET", true);
		if(GetUnitBaktun(ePossibleGreatPerson) == 0)
		{
			eDesiredGreatPerson = ePossibleGreatPerson;
		}
		else
		{
			ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_ENGINEER");
			if(GetUnitBaktun(ePossibleGreatPerson) == 0)
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
			else
			{
				ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_WRITER");
				if(GetUnitBaktun(ePossibleGreatPerson) == 0)
				{
					eDesiredGreatPerson = ePossibleGreatPerson;
				}
				else
				{
					ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_MERCHANT");
					if(GetUnitBaktun(ePossibleGreatPerson) == 0)
					{
						eDesiredGreatPerson = ePossibleGreatPerson;
					}
					else
					{
						ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_SCIENTIST");
						if(GetUnitBaktun(ePossibleGreatPerson) == 0)
						{
							eDesiredGreatPerson = ePossibleGreatPerson;
						}
						else
						{
							ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_ARTIST");
							if(GetUnitBaktun(ePossibleGreatPerson) == 0)
							{
								eDesiredGreatPerson = ePossibleGreatPerson;
							}
							else
							{
								ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_MUSICIAN");
								if(GetUnitBaktun(ePossibleGreatPerson) == 0)
								{
									eDesiredGreatPerson = ePossibleGreatPerson;
								}
								else
								{
									ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_GREAT_GENERAL");
									if(GetUnitBaktun(ePossibleGreatPerson) == 0)
									{
										eDesiredGreatPerson = ePossibleGreatPerson;
									}
									else
									{
										ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_GREAT_ADMIRAL");
										if(GetUnitBaktun(ePossibleGreatPerson) == 0)
										{
											eDesiredGreatPerson = ePossibleGreatPerson;
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


	if(eDesiredGreatPerson != NO_UNIT)
	{
		CvCity* pCity = m_pPlayer->GetGreatPersonSpawnCity(eDesiredGreatPerson);
		if(pCity)
		{
			pCity->GetCityCitizens()->DoSpawnGreatPerson(eDesiredGreatPerson, true, false);
			SetUnitBaktun(eDesiredGreatPerson);
		}
		m_pPlayer->ChangeNumMayaBoosts(-1);
	}
}


void CvPlayerTraits::ComputeMayaDate()
{
	float fCalendarStart = CALENDAR_START;
	int iYear = GC.getGame().getGameTurnYear();
	float fYear = (float)iYear;

	if(fYear >= fCalendarStart)
	{

		float fDays = (fYear - fCalendarStart) * DAYS_IN_YEAR;
		m_iBaktun = (int)(fDays / DAYS_IN_BAKTUN);
		fDays = fDays - ((float)m_iBaktun * DAYS_IN_BAKTUN);
		m_iKatun = (int)(fDays / DAYS_IN_KATUN);
		fDays = fDays - ((float)m_iKatun * DAYS_IN_KATUN);
		m_iTun = (int)(fDays / DAYS_IN_TUN);
		fDays = fDays - ((float)m_iTun * DAYS_IN_TUN);
		m_iWinal = (int)(fDays / DAYS_IN_WINAL);
		fDays = fDays - ((float)m_iWinal * DAYS_IN_WINAL);
		m_iKin = (int)fDays;
	}
}


int CvPlayerTraits::GetUnitBaktun(UnitTypes eUnit) const
{
	std::vector<MayaBonusChoice>::const_iterator it;


#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_aMayaBonusChoices.begin(); it != m_aMayaBonusChoices.end(); ++it)
#else
	for(it = m_aMayaBonusChoices.begin(); it != m_aMayaBonusChoices.end(); it++)
#endif
	{
		if(it->m_eUnitType == eUnit)
		{
			return it->m_iBaktunJustFinished;
		}
	}

	return 0;
}


void CvPlayerTraits::SetUnitBaktun(UnitTypes eUnit)
{
	MayaBonusChoice choice;
	choice.m_eUnitType = eUnit;
	choice.m_iBaktunJustFinished = m_iBaktun;
	m_aMayaBonusChoices.push_back(choice);
}


bool CvPlayerTraits::IsFreeMayaGreatPersonChoice() const
{

	int iNumGreatPeopleTypes = 0;
	SpecialUnitTypes eSpecialUnitGreatPerson = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_PEOPLE");


#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
#else
	for(int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
#endif
	{
		const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(iI);
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
		if(pkUnitClassInfo)
		{
			const UnitTypes eUnit = (UnitTypes)m_pPlayer->getCivilizationInfo().getCivilizationUnits(eUnitClass);
			if (eUnit != NO_UNIT)
			{
				CvUnitEntry* pUnitEntry = GC.getUnitInfo(eUnit);
				if (pUnitEntry)
				{
					if (pUnitEntry->GetSpecialUnitType() == eSpecialUnitGreatPerson)
					{
						iNumGreatPeopleTypes++;
					}
				}
			}	
		}
	}

	return ((int)m_aMayaBonusChoices.size() >= iNumGreatPeopleTypes);
}




void CvPlayerTraits::Read(FDataStream& kStream)
{
	int iNumEntries;


	uint uiVersion;
	kStream >> uiVersion;
	{ FILogFile* pDbg = LOGFILEMGR.GetLog("LoadDebug.log", FILogFile::kDontTimeStamp); pDbg->Msg("  [CvPlayerTraits::Read] uiVersion=%u (expected 19)", uiVersion); }


	m_vPotentiallyActiveLeaderTraits.clear();
	for(int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		m_vLeaderHasTrait[iI] = false;
		if (m_pPlayer && m_pPlayer->isMajorCiv() && m_pPlayer->isAlive() && m_pPlayer->getLeaderInfo().hasTrait( (TraitTypes)iI ))
		{
			m_vLeaderHasTrait[iI] = true;
			m_vPotentiallyActiveLeaderTraits.push_back( (TraitTypes)iI );
		}
	}

	kStream >> m_iGreatPeopleRateModifier;
	kStream >> m_iGreatScientistRateModifier;
	kStream >> m_iGreatGeneralRateModifier;
	kStream >> m_iGreatGeneralExtraBonus;

	kStream >> m_iGreatPersonGiftInfluence;

	kStream >> m_iLevelExperienceModifier;
	kStream >> m_iMaxGlobalBuildingProductionModifier;
	kStream >> m_iMaxTeamBuildingProductionModifier;
	kStream >> m_iMaxPlayerBuildingProductionModifier;
	kStream >> m_iCityUnhappinessModifier;
	kStream >> m_iPopulationUnhappinessModifier;
	kStream >> m_iCityStateBonusModifier;
	kStream >> m_iCityStateFriendshipModifier;
	kStream >> m_iCityStateCombatModifier;
	kStream >> m_iLandBarbarianConversionPercent;
	kStream >> m_iLandBarbarianConversionExtraUnits;
	kStream >> m_iSeaBarbarianConversionPercent;
	kStream >> m_iCapitalBuildingModifier;
	kStream >> m_iPlotBuyCostModifier;
	kStream >> m_iPlotCultureCostModifier;
#ifdef NQ_SCIENCE_PER_GREAT_PERSON_BORN
	kStream >> m_iSciencePerGreatPersonBorn;
#endif
#ifdef NQ_NUM_TURNS_BEFORE_MINOR_ALLIES_REFUSE_BRIBES_FROM_TRAIT
	kStream >> m_iNumTurnsBeforeMinorAlliesRefuseBribes;
#endif
	kStream >> m_iCultureFromKills;
	if (uiVersion >= 19)
	{
		kStream >> m_iFaithFromKills;
	}
	else
	{
		m_iFaithFromKills = 0;
	}
	kStream >> m_iCityCultureBonus;

	if (uiVersion >= 17)
	{
		kStream >> m_iCapitalThemingBonusModifier;
	}
	else
	{
		m_iCapitalThemingBonusModifier = 0;
	}

	kStream >> m_iPolicyCostModifier;
	kStream >> m_iCityConnectionTradeRouteChange;
	kStream >> m_iWonderProductionModifier;
	kStream >> m_iPlunderModifier;

	kStream >> m_iImprovementMaintenanceModifier;

	kStream >> m_iGoldenAgeDurationModifier;
	kStream >> m_iGoldenAgeMoveChange;
	kStream >> m_iGoldenAgeCombatModifier;

	if (uiVersion >= 2)
	{
		kStream >> m_iGoldenAgeTourismModifier;
		kStream >> m_iGoldenAgeGreatArtistRateModifier;
		kStream >> m_iGoldenAgeGreatMusicianRateModifier;
		kStream >> m_iGoldenAgeGreatWriterRateModifier;
	}
	else
	{
		m_iGoldenAgeTourismModifier = 0;
		m_iGoldenAgeGreatArtistRateModifier = 0;
		m_iGoldenAgeGreatMusicianRateModifier = 0;
		m_iGoldenAgeGreatWriterRateModifier = 0;
	}

	kStream >> m_iExtraEmbarkMoves;

	kStream >> m_iNaturalWonderFirstFinderGold;

	kStream >> m_iNaturalWonderSubsequentFinderGold;
#if defined(TRAITIFY)
	kStream >> m_bHalfMoreSpecialistUnhappiness;

	kStream >> m_iNumExtraLeagueVotes;
	kStream >> m_iNumTradeRouteBonus;
	kStream >> m_iMinorFriendshipMinimum;
	kStream >> m_iGreatEngineerRateModifier;
	kStream >> m_iGreatMerchantRateModifier;
	kStream >> m_iMinorBullyModifier;
	kStream >> m_iInternationalRouteGrowthModifier;
	kStream >> m_iLocalHappinessPerCity;
	kStream >> m_iGlobalHappinessPerCity;
	kStream >> m_iUnhappinessModifierForPuppetedCities;
	kStream >> m_iExtraPopulation;
	kStream >> m_iFaithCostModifier;
	kStream >> m_iIdeologyPressureUnhappinessModifier;
	kStream >> m_iForeignRelgionPressureModifier;
	kStream >> m_iFriendlyLandsCitizenMoveChange;
#endif
#if defined(v35_TRAITIFY)
	kStream >> m_iEmbarkedUnitFullStrengthCount;
	kStream >> m_iCityStateUnitGiftExtraExperience;
	kStream >> m_iGreatGeneralSiegeBonus;
#endif
#if defined(LEKMOD_v34)
	kStream >> m_bReligionEnhanceReformation;
	kStream >> m_iSelfReligiousPressureModifier;
#endif



	kStream >> m_iNaturalWonderFirstFinderFaith;

	kStream >> m_iNaturalWonderSubsequentFinderFaith;



	kStream >> m_iNaturalWonderYieldModifier;
	kStream >> m_iNaturalWonderHappinessModifier;

	kStream >> m_iNearbyImprovementCombatBonus;
	kStream >> m_iNearbyImprovementBonusRange;

	kStream >> m_iCultureBuildingYieldChange;

	kStream >> m_iCombatBonusVsHigherTech;

	kStream >> m_iCombatBonusVsLargerCiv;

	kStream >> m_iLandUnitMaintenanceModifier;

	kStream >> m_iNavalUnitMaintenanceModifier;

	kStream >> m_iRazeSpeedModifier;

	kStream >> m_iDOFGreatPersonModifier;

	kStream >> m_iLuxuryHappinessRetention;

	kStream >> m_iExtraHappinessPerLuxury;

	kStream >> m_iExtraSpies;

	kStream >> m_iHappinessPerReligion;

	kStream >> m_iUnresearchedTechBonusFromKills;

	if (uiVersion >= 4)
	{
		kStream >> m_iExtraFoundedCityTerritoryClaimRange;
	}
	else
	{
		m_iExtraFoundedCityTerritoryClaimRange = 0;
	}

	if (uiVersion >= 5)
	{
		kStream >> m_iFreeSocialPoliciesPerEra;
	}
	else
	{
		m_iFreeSocialPoliciesPerEra = 0;
	}

	if (uiVersion >= 6)
	{
		kStream >> m_iNumTradeRoutesModifier;
	}
	else
	{
		m_iNumTradeRoutesModifier = 0;
	}

	if (uiVersion >= 8)
	{
		kStream >> m_iTradeRouteResourceModifier;
	}
	else
	{
		m_iTradeRouteResourceModifier = 0;
	}

	if (uiVersion >= 9)
	{
		kStream >> m_iUniqueLuxuryCities;
		kStream >> m_iUniqueLuxuryQuantity;
	}
	else
	{
		m_iUniqueLuxuryCities = 0;
		m_iUniqueLuxuryQuantity = 0;
	}

	if (uiVersion >= 11)
	{
		kStream >> m_iUniqueLuxuryCitiesPlaced;
	}
	else
	{
		m_iUniqueLuxuryCitiesPlaced = 0;
	}

	if (uiVersion >= 13)
	{
		kStream >> m_iWorkerSpeedModifier;
	}
	else
	{
		m_iWorkerSpeedModifier = 0;
	}

	if (uiVersion >= 14)
	{
		kStream >> m_iAfraidMinorPerTurnInfluence;
	}
	else
	{
		m_iAfraidMinorPerTurnInfluence = 0;
	}
	
	if (uiVersion >= 15)
	{
		kStream >> m_iLandTradeRouteRangeBonus;
		kStream >> m_iTradeReligionModifier;
	}
	else
	{
		m_iLandTradeRouteRangeBonus = 0;
		m_iTradeReligionModifier = 0;
	}

	if (uiVersion >= 16)
	{
		kStream >> m_iTradeBuildingModifier;
	}
	else
	{
		m_iTradeBuildingModifier = 0;
	}

	kStream >> m_bFightWellDamaged;
	kStream >> m_bMoveFriendlyWoodsAsRoad;
	kStream >> m_bFasterAlongRiver;

	kStream >> m_bFasterInHills;

	kStream >> m_bEmbarkedAllWater;

    kStream >> m_bEmbarkedToLandFlatCost;
#ifdef LEKMOD_TRAIT_CIVILIAN_EMBARK_ONE_MOVE
    kStream >> m_bCiviliansEmbarkOneMove;
#endif

	kStream >> m_bNoHillsImprovementMaintenance;

	kStream >> m_bTechBoostFromCapitalScienceBuildings;
	kStream >> m_bStaysAliveZeroCities;

	kStream >> m_bFaithFromUnimprovedForest;
#ifdef NQ_UNIT_IMMUNE_TO_PLUNDER_FROM_TRAIT
	kStream >> m_bSeaTradeRoutesArePlunderImmune;
#endif


	kStream >> m_bEarnsGreatPersonOnSlotOrGuild;
	kStream >> m_bHasBuiltWritersGuild;
	kStream >> m_bHasBuiltArtistsGuild;
	kStream >> m_bHasBuiltMusiciansGuild;



	kStream >> m_bBonusReligiousBelief;

	kStream >> m_bAbleToAnnexCityStates;

	kStream >> m_bCrossesMountainsAfterGreatGeneral;

	kStream >> m_bMayaCalendarBonuses;
	kStream >> m_iBaktunPreviousTurn;

	kStream >> iNumEntries;
	m_aMayaBonusChoices.clear();
	MayaBonusChoice choice;
	for(int iI = 0; iI < iNumEntries; iI++)
	{
		kStream >> choice.m_eUnitType;
		kStream >> choice.m_iBaktunJustFinished;
		m_aMayaBonusChoices.push_back(choice);
	}

	kStream >> m_bNoAnnexing;
	if (uiVersion >= 3)
	{
		kStream >> m_bTechFromCityConquer;
	}
	else
	{
		m_bTechFromCityConquer = false;
	}

	if (uiVersion >= 9)
	{
		kStream >> m_bUniqueLuxuryRequiresNewArea;
	}
	else
	{
		m_bUniqueLuxuryRequiresNewArea = false;
	}

	if (uiVersion >= 12)
	{
		kStream >> m_bRiverTradeRoad;
	}
	else
	{
		m_bRiverTradeRoad = false;
	}

	if (uiVersion >= 18)
	{
		kStream >> m_bAngerFreeIntrusionOfCityStates;
	}
	else
	{
		m_bAngerFreeIntrusionOfCityStates = false;
	}

	kStream >> m_eCampGuardType;

	kStream >> m_eCombatBonusImprovement;

	ArrayWrapper<int> kExtraYieldThreshold(NUM_YIELD_TYPES, m_iExtraYieldThreshold);
	kStream >> kExtraYieldThreshold;

	ArrayWrapper<int> kFreeCityYield(NUM_YIELD_TYPES, m_iFreeCityYield);
	kStream >> kFreeCityYield;

	ArrayWrapper<int> kYieldChangeResourcesWrapper(NUM_YIELD_TYPES, m_iYieldChangeStrategicResources);
	kStream >> kYieldChangeResourcesWrapper;

	ArrayWrapper<int> kYieldChangeLuxuryResourcesWrapper(NUM_YIELD_TYPES, m_iYieldChangeLuxuryResources);
	kStream >> kYieldChangeLuxuryResourcesWrapper;

	ArrayWrapper<int> kYieldRateModifierWrapper(NUM_YIELD_TYPES, m_iYieldRateModifier);
	kStream >> kYieldRateModifierWrapper;

	ArrayWrapper<int> kYieldChangeNaturalWonderWrapper(NUM_YIELD_TYPES, m_iYieldChangeNaturalWonder);
	kStream >> kYieldChangeNaturalWonderWrapper;

#if defined(FULL_YIELD_FROM_KILLS)
	ArrayWrapper<int> kYieldFromKillsWrapper(NUM_YIELD_TYPES, m_iYieldFromKills);
	kStream >> kYieldFromKillsWrapper;
	ArrayWrapper<int> kYieldFromKillsMaxWrapper(NUM_YIELD_TYPES, m_iYieldFromKillsMax);
	kStream >> kYieldFromKillsMaxWrapper;
#endif
#if defined(LEKMOD_EXPERIMENTAL_CHANGES)
	ArrayWrapper<int> kWorldWonderYieldChangesWrapper(NUM_YIELD_TYPES, m_iWorldWonderYieldChange);
	kStream >> kWorldWonderYieldChangesWrapper;
#endif
#if defined(TRAITIFY)
	ArrayWrapper<int> kPuppetYieldModifiersWrapper(NUM_YIELD_TYPES, m_iPuppetYieldModifiers);
	kStream >> kPuppetYieldModifiersWrapper;
	ArrayWrapper<int> kGoldenAgeYieldModifierWrapper(NUM_YIELD_TYPES, m_iGoldenAgeYieldModifier);
	kStream >> kGoldenAgeYieldModifierWrapper;
	ArrayWrapper<int> kRouteMovementChangeWrapper(NUM_ROUTE_TYPES, m_iRouteMovementChange);
	kStream >> kRouteMovementChangeWrapper;
#endif
#if !defined(TRADE_REFACTOR)
	if (uiVersion >= 7)
	{
		ArrayWrapper<int> kYieldChangePerTradePartnerWrapper(NUM_YIELD_TYPES, m_iYieldChangePerTradePartner);
		kStream >> kYieldChangePerTradePartnerWrapper;

		ArrayWrapper<int> kYieldChangeIncomingTradeRouteWrapper(NUM_YIELD_TYPES, m_iYieldChangeIncomingTradeRoute);
		kStream >> kYieldChangeIncomingTradeRouteWrapper;
	}
	else
	{
		for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
		{
			m_iYieldChangePerTradePartner[iYield] = 0;
			m_iYieldChangeIncomingTradeRoute[iYield] = 0;
		}
	}
#else
	ArrayWrapper<int> kTradePartnerYieldFlatBonusPerEraWrapper(NUM_YIELD_TYPES, m_iTradePartnerYieldFlatBonusPerEra);
	kStream >> kTradePartnerYieldFlatBonusPerEraWrapper;
	kStream >> m_ppaaiTradeConnectionLandYieldChange;
	kStream >> m_ppaaiTradeConnectionSeaYieldChange;
	kStream >> m_ppaaiYieldChangePerTradePartnerByDomain;
	kStream >> m_ppaaiIncomingTradeConnectionLandYieldChange;
	kStream >> m_ppaaiIncomingTradeConnectionSeaYieldChange;
	kStream >> m_ppaaiTradeConnectionLandYieldModifier;
	kStream >> m_ppaaiTradeConnectionSeaYieldModifier;
#endif
	CvAssert(GC.getNumTerrainInfos() == NUM_TERRAIN_TYPES);
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, &m_iStrategicResourceQuantityModifier[0], GC.getNumTerrainInfos());

	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_aiResourceQuantityModifier);
#if defined(TRAITIFY)
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_aiBuildingClassGlobalHappiness);
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_aiBuildingClassHappiness);

	kStream >> iNumEntries;
	m_abRemoveRequiredTerrain.clear();
	for (int i = 0; i < iNumEntries; i++)
	{
		bool bValue;
		kStream >> bValue;
		m_abRemoveRequiredTerrain.push_back(bValue);
	}

	kStream >> iNumEntries;
	m_abForceSpawnCapital.clear();
	for (int i = 0; i < iNumEntries; i++)
	{
		bool bValue;
		kStream >> bValue;
		m_abForceSpawnCapital.push_back(bValue);
	}
#endif
#if defined(v35_TRAITIFY)
	kStream >> iNumEntries;
	m_vbEmbarkedAllowedMissions.clear();
	for (int i = 0; i < iNumEntries; i++)
	{
		bool bValue;
		kStream >> bValue;
		m_vbEmbarkedAllowedMissions.push_back(bValue);
	}
	kStream >> m_viBuildCompleteTileClaimRange;
	kStream >> m_viBuildCompleteTileStealRange;
	kStream >> m_viUnitCombatWorkRateChange;
#endif
	kStream >> iNumEntries;
	m_abNoTrain.clear();
	for (int i = 0; i < iNumEntries; i++)
	{
		bool bValue;
		kStream >> bValue;
		m_abNoTrain.push_back(bValue);
	}

#ifdef LEKMOD_TRAIT_NO_BUILD_IMPROVEMENTS
	kStream >> iNumEntries;
	m_abNoBuild.clear();
	for (int i = 0; i < iNumEntries; i++)
	{
		bool bValue;
		kStream >> bValue;
		m_abNoBuild.push_back(bValue);
	}
#endif
#if defined(LEKMOD_TRAIT_BAN_UNIT_MISSIONS)
	kStream >> iNumEntries;
	m_abBannedUnitMissions.clear();
	for (int i = 0; i < iNumEntries; i++)
	{
		bool bValue;
		kStream >> bValue;
		m_abBannedUnitMissions.push_back(bValue);
	}
#endif
#ifdef LEKMOD_BUILD_TIME_OVERRIDE
	kStream >> iNumEntries;
	m_aiBuildTimeOverride.clear();
	for (int i = 0; i < iNumEntries; i++)
	{
		int iBuildTime;
		kStream >> iBuildTime;
		m_aiBuildTimeOverride.push_back(iBuildTime);
	}
#endif

	kStream >> iNumEntries;
	m_aFreeTraitUnits.clear();
	for(int iI = 0; iI < iNumEntries; iI++)
	{
		FreeTraitUnit trait;
		kStream >> trait.m_iFreeUnit;
		kStream >> trait.m_ePrereqTech;
		m_aFreeTraitUnits.push_back(trait);
	}

	kStream >> iNumEntries;
	for(int iI = 0; iI < iNumEntries; iI++)
	{
		kStream >> m_paiMovesChangeUnitCombat[iI];
	}
	if (uiVersion >= 10)
	{
		for(int iI = 0; iI < iNumEntries; iI++)
		{
			kStream >> m_paiMaintenanceModifierUnitCombat[iI];
		}
	}
	else
	{
		int iNumUnitCombatClassInfos = GC.getNumUnitCombatClassInfos();
		m_paiMaintenanceModifierUnitCombat.resize(iNumUnitCombatClassInfos);
		for(int iI = 0; iI < iNumUnitCombatClassInfos; iI++)
		{
			m_paiMaintenanceModifierUnitCombat[iI] = 0;
		}
	}

	kStream >> m_ppaaiImprovementYieldChange;
	kStream >> m_ppaaiSpecialistYieldChange;
#ifdef LEK_TRAIT_SPECIALIST_YIELD_MAX_ONE
	kStream >> m_ppaaiAnySpecificSpecialistYieldChange;
#endif
	kStream >> m_ppaaiUnimprovedFeatureYieldChange;
#if defined(TRAITIFY)
	kStream >> m_ppaaiBuildingClassYieldChange;
	kStream >> m_ppaaiFeatureYieldChange;
	kStream >> m_ppaaiTerrainYieldChange;
	kStream >> m_ppaaiResourceClassYieldChange;
	kStream >> m_ppaaiResourceYieldChange;
	kStream >> m_ppaaiBuildingCostOverride;
	kStream >> m_ppaaiFreshWaterImprovementYieldChange;
	kStream >> m_ppaaiNonFreshWaterImprovementYieldChange;
#endif
#if defined(LEKMOD_CITY_YIELDS_TRAITS)
	ArrayWrapper<int> kCapitalYieldChangeWrapper(NUM_YIELD_TYPES, m_aiCapitalYieldChange);
	kStream >> kCapitalYieldChangeWrapper;
	kStream >> m_ppaaiCapitalEraYieldChange;
	kStream >> m_ppaaiCapitalTechYieldChange;
	ArrayWrapper<int> kCityYieldChangeWrapper(NUM_YIELD_TYPES, m_aiCityYieldChange);
	kStream >> kCityYieldChangeWrapper;
	kStream >> m_ppaaiCityEraYieldChange;
	kStream >> m_ppaaiCityTechYieldChange;
#endif
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	kStream >> m_ppaaiGreatWorkClassYieldChange;
#endif
	if (uiVersion >= 11)
	{
		kStream >> iNumEntries;
		m_aUniqueLuxuryAreas.clear();
		for (int iI = 0; iI < iNumEntries; iI++)
		{
			int iAreaID;
			kStream >> iAreaID;
			m_aUniqueLuxuryAreas.push_back(iAreaID);
		}
	}
	else if (uiVersion >= 9)
	{
		kStream >> m_aUniqueLuxuryAreas;
	}
	else
	{
		m_aUniqueLuxuryAreas.clear();
	}
#ifdef LEKMOD_TRAIT_FIRST_PROPHET_COST_MOD
	kStream >> m_iFirstProphetCostMod;
#endif
#if defined(LEKMOD_GOLDEN_AGE_YIELD_THRESHOLD)
	kStream >> m_sGoldenAgeYieldThreshold;
#endif
	kStream >> m_vUsedGroupAreas;
	kStream >> m_vGroupPriority;
}


void CvPlayerTraits::Write(FDataStream& kStream)
{

	uint uiVersion = 19;
	kStream << uiVersion;

	kStream << m_iGreatPeopleRateModifier;
	kStream << m_iGreatScientistRateModifier;
	kStream << m_iGreatGeneralRateModifier;
	kStream << m_iGreatGeneralExtraBonus;
	kStream << m_iGreatPersonGiftInfluence;
	kStream << m_iLevelExperienceModifier;
	kStream << m_iMaxGlobalBuildingProductionModifier;
	kStream << m_iMaxTeamBuildingProductionModifier;
	kStream << m_iMaxPlayerBuildingProductionModifier;
	kStream << m_iCityUnhappinessModifier;
	kStream << m_iPopulationUnhappinessModifier;
	kStream << m_iCityStateBonusModifier;
	kStream << m_iCityStateFriendshipModifier;
	kStream << m_iCityStateCombatModifier;
	kStream << m_iLandBarbarianConversionPercent;
	kStream << m_iLandBarbarianConversionExtraUnits;
	kStream << m_iSeaBarbarianConversionPercent;
	kStream << m_iCapitalBuildingModifier;
	kStream << m_iPlotBuyCostModifier;
	kStream << m_iPlotCultureCostModifier;
#ifdef NQ_SCIENCE_PER_GREAT_PERSON_BORN
	kStream << m_iSciencePerGreatPersonBorn;
#endif
#ifdef NQ_NUM_TURNS_BEFORE_MINOR_ALLIES_REFUSE_BRIBES_FROM_TRAIT
	kStream << m_iNumTurnsBeforeMinorAlliesRefuseBribes;
#endif
	kStream << m_iCultureFromKills;
	kStream << m_iFaithFromKills;
	kStream << m_iCityCultureBonus;
	kStream << m_iCapitalThemingBonusModifier;
	kStream << m_iPolicyCostModifier;
	kStream << m_iCityConnectionTradeRouteChange;
	kStream << m_iWonderProductionModifier;
	kStream << m_iPlunderModifier;
	kStream << m_iImprovementMaintenanceModifier;
	kStream << m_iGoldenAgeDurationModifier;
	kStream << m_iGoldenAgeMoveChange;
	kStream << m_iGoldenAgeCombatModifier;
	kStream << m_iGoldenAgeTourismModifier;
	kStream << m_iGoldenAgeGreatArtistRateModifier;
	kStream << m_iGoldenAgeGreatMusicianRateModifier;
	kStream << m_iGoldenAgeGreatWriterRateModifier;
	kStream << m_iExtraEmbarkMoves;
	kStream << m_iNaturalWonderFirstFinderGold;
	kStream << m_iNaturalWonderSubsequentFinderGold;
#if defined(TRAITIFY)
	kStream << m_bHalfMoreSpecialistUnhappiness;

	kStream << m_iNumExtraLeagueVotes;
	kStream << m_iNumTradeRouteBonus;
	kStream << m_iMinorFriendshipMinimum;
	kStream << m_iGreatEngineerRateModifier;
	kStream << m_iGreatMerchantRateModifier;
	kStream << m_iMinorBullyModifier;
	kStream << m_iInternationalRouteGrowthModifier;
	kStream << m_iLocalHappinessPerCity;
	kStream << m_iGlobalHappinessPerCity;
	kStream << m_iUnhappinessModifierForPuppetedCities;
	kStream << m_iExtraPopulation;
	kStream << m_iFaithCostModifier;
	kStream << m_iIdeologyPressureUnhappinessModifier;
	kStream << m_iForeignRelgionPressureModifier;
	kStream << m_iFriendlyLandsCitizenMoveChange;
#endif
#if defined(v35_TRAITIFY)
	kStream << m_iEmbarkedUnitFullStrengthCount;	
	kStream << m_iCityStateUnitGiftExtraExperience;
	kStream << m_iGreatGeneralSiegeBonus;
#endif
#if defined(LEKMOD_v34)
	kStream << m_bReligionEnhanceReformation;
	kStream << m_iSelfReligiousPressureModifier;
#endif

	
	kStream << m_iNaturalWonderFirstFinderFaith;
	kStream << m_iNaturalWonderSubsequentFinderFaith;
	


	kStream << m_iNaturalWonderYieldModifier;
	kStream << m_iNaturalWonderHappinessModifier;
	kStream << m_iNearbyImprovementCombatBonus;
	kStream << m_iNearbyImprovementBonusRange;
	kStream << m_iCultureBuildingYieldChange;
	kStream << m_iCombatBonusVsHigherTech;
	kStream << m_iCombatBonusVsLargerCiv;
	kStream << m_iLandUnitMaintenanceModifier;
	kStream << m_iNavalUnitMaintenanceModifier;
	kStream << m_iRazeSpeedModifier;
	kStream << m_iDOFGreatPersonModifier;
	kStream << m_iLuxuryHappinessRetention;
	kStream << m_iExtraHappinessPerLuxury;
	kStream << m_iExtraSpies;
	kStream << m_iHappinessPerReligion;
	kStream << m_iUnresearchedTechBonusFromKills;
	kStream << m_iExtraFoundedCityTerritoryClaimRange;
	kStream << m_iFreeSocialPoliciesPerEra;
	kStream << m_iNumTradeRoutesModifier;
	kStream << m_iTradeRouteResourceModifier;
	kStream << m_iUniqueLuxuryCities;
	kStream << m_iUniqueLuxuryQuantity;
	kStream << m_iUniqueLuxuryCitiesPlaced;
	kStream << m_iWorkerSpeedModifier;
	kStream << m_iAfraidMinorPerTurnInfluence;
	kStream << m_iLandTradeRouteRangeBonus;
	kStream << m_iTradeReligionModifier;
	kStream << m_iTradeBuildingModifier;

	kStream << m_bFightWellDamaged;
	kStream << m_bMoveFriendlyWoodsAsRoad;
	kStream << m_bFasterAlongRiver;
	kStream << m_bFasterInHills;
	kStream << m_bEmbarkedAllWater;
	kStream << m_bEmbarkedToLandFlatCost;
#ifdef LEKMOD_TRAIT_CIVILIAN_EMBARK_ONE_MOVE
    kStream << m_bCiviliansEmbarkOneMove;
#endif
	kStream << m_bNoHillsImprovementMaintenance;
	kStream << m_bTechBoostFromCapitalScienceBuildings;
	kStream << m_bStaysAliveZeroCities;
	kStream << m_bFaithFromUnimprovedForest;
#ifdef NQ_UNIT_IMMUNE_TO_PLUNDER_FROM_TRAIT
	kStream << m_bSeaTradeRoutesArePlunderImmune;
#endif


	kStream << m_bEarnsGreatPersonOnSlotOrGuild;
	kStream << m_bHasBuiltWritersGuild;
	kStream << m_bHasBuiltArtistsGuild;
	kStream << m_bHasBuiltMusiciansGuild;


	kStream << m_bBonusReligiousBelief;
	kStream << m_bAbleToAnnexCityStates;
	kStream << m_bCrossesMountainsAfterGreatGeneral;
	kStream << m_bMayaCalendarBonuses;

	kStream << m_iBaktunPreviousTurn;

	std::vector<MayaBonusChoice>::const_iterator it;
	kStream << m_aMayaBonusChoices.size();
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_aMayaBonusChoices.begin(); it != m_aMayaBonusChoices.end(); ++it)
#else
	for(it = m_aMayaBonusChoices.begin(); it != m_aMayaBonusChoices.end(); it++)
#endif
	{
		kStream << it->m_eUnitType;
		kStream << it->m_iBaktunJustFinished;
	}

	kStream << m_bNoAnnexing;
	kStream << m_bTechFromCityConquer;
	kStream << m_bUniqueLuxuryRequiresNewArea;
	kStream << m_bRiverTradeRoad;
	kStream << m_bAngerFreeIntrusionOfCityStates;

	kStream << m_eCampGuardType;
	kStream << m_eCombatBonusImprovement;

	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iExtraYieldThreshold);
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iFreeCityYield);
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iYieldChangeStrategicResources);
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iYieldChangeLuxuryResources);
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iYieldRateModifier);
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iYieldChangeNaturalWonder);
#if defined(FULL_YIELD_FROM_KILLS)
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iYieldFromKills);
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iYieldFromKillsMax);
#endif
#if defined(LEKMOD_EXPERIMENTAL_CHANGES)
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iWorldWonderYieldChange);
#endif
#if defined(TRAITIFY)
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iPuppetYieldModifiers);
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iGoldenAgeYieldModifier);
	kStream << ArrayWrapper<int>(NUM_ROUTE_TYPES, m_iRouteMovementChange);
#endif
#if !defined(TRADE_REFACTOR)
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iYieldChangePerTradePartner);
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iYieldChangeIncomingTradeRoute);
#else
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iTradePartnerYieldFlatBonusPerEra);
	kStream << m_ppaaiTradeConnectionLandYieldChange;
	kStream << m_ppaaiTradeConnectionSeaYieldChange;
	kStream << m_ppaaiYieldChangePerTradePartnerByDomain;
	kStream << m_ppaaiIncomingTradeConnectionLandYieldChange;
	kStream << m_ppaaiIncomingTradeConnectionSeaYieldChange;
	kStream << m_ppaaiTradeConnectionLandYieldModifier;
	kStream << m_ppaaiTradeConnectionSeaYieldModifier;
#endif
	CvInfosSerializationHelper::WriteHashedDataArray<TerrainTypes>(kStream, &m_iStrategicResourceQuantityModifier[0], GC.getNumTerrainInfos());
	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes>(kStream, m_aiResourceQuantityModifier);
#if defined(TRAITIFY)
	CvInfosSerializationHelper::WriteHashedDataArray<BuildingClassTypes, int>(kStream, m_aiBuildingClassGlobalHappiness);
	CvInfosSerializationHelper::WriteHashedDataArray<BuildingClassTypes, int>(kStream, m_aiBuildingClassHappiness);

	kStream << m_abRemoveRequiredTerrain.size();
	for (uint ui = 0; ui < m_abRemoveRequiredTerrain.size(); ui++)
	{
		kStream << m_abRemoveRequiredTerrain[ui];
	}

	kStream << m_abForceSpawnCapital.size();
	for (uint ui = 0; ui < m_abForceSpawnCapital.size(); ui++)
	{
		kStream << m_abForceSpawnCapital[ui];
	}
#endif
#if defined(v35_TRAITIFY)
	kStream << m_vbEmbarkedAllowedMissions.size();
	for (uint ui = 0; ui < m_vbEmbarkedAllowedMissions.size(); ui++)
	{
		kStream << m_vbEmbarkedAllowedMissions[ui];
	}
	kStream << m_viBuildCompleteTileClaimRange;
	kStream << m_viBuildCompleteTileStealRange;
	kStream << m_viUnitCombatWorkRateChange;
#endif
	kStream << m_abNoTrain.size();
	for (uint ui = 0; ui < m_abNoTrain.size(); ui++)
	{
		kStream << m_abNoTrain[ui];
	}

#ifdef LEKMOD_TRAIT_NO_BUILD_IMPROVEMENTS
	kStream << m_abNoBuild.size();
	for (uint ui = 0; ui < m_abNoBuild.size(); ui++)
	{
		kStream << m_abNoBuild[ui];
	}
#endif
#if defined(LEKMOD_TRAIT_BAN_UNIT_MISSIONS)
	kStream << m_abBannedUnitMissions.size();
	for (uint ui = 0; ui < m_abBannedUnitMissions.size(); ui++)
	{
		kStream << m_abBannedUnitMissions[ui];
	}
#endif
#ifdef LEKMOD_BUILD_TIME_OVERRIDE
	kStream << m_aiBuildTimeOverride.size();
	for (uint ui = 0; ui < m_aiBuildTimeOverride.size(); ui++)
	{
		kStream << m_aiBuildTimeOverride[ui];
	}
#endif
	kStream << m_aFreeTraitUnits.size();
	for(uint ui = 0; ui < m_aFreeTraitUnits.size(); ui++)
	{
		kStream << m_aFreeTraitUnits[ui].m_iFreeUnit;
		kStream << m_aFreeTraitUnits[ui].m_ePrereqTech;
	}

	int iNumUnitCombatClassInfos = GC.getNumUnitCombatClassInfos();
	kStream << 	iNumUnitCombatClassInfos;
	for(int iI = 0; iI < iNumUnitCombatClassInfos; iI++)
	{
		kStream << m_paiMovesChangeUnitCombat[iI];
	}
	for(int iI = 0; iI < iNumUnitCombatClassInfos; iI++)
	{
		kStream << m_paiMaintenanceModifierUnitCombat[iI];
	}

	kStream << m_ppaaiImprovementYieldChange;
	kStream << m_ppaaiSpecialistYieldChange;
#ifdef LEK_TRAIT_SPECIALIST_YIELD_MAX_ONE
	kStream << m_ppaaiAnySpecificSpecialistYieldChange;
#endif
	kStream << m_ppaaiUnimprovedFeatureYieldChange;
#if defined(TRAITIFY)
	kStream << m_ppaaiBuildingClassYieldChange;
	kStream << m_ppaaiFeatureYieldChange;
	kStream << m_ppaaiTerrainYieldChange;
	kStream << m_ppaaiResourceClassYieldChange;
	kStream << m_ppaaiResourceYieldChange;
	kStream << m_ppaaiBuildingCostOverride;
	kStream << m_ppaaiFreshWaterImprovementYieldChange;
	kStream << m_ppaaiNonFreshWaterImprovementYieldChange;
#endif
#if defined(LEKMOD_CITY_YIELDS_TRAITS)
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_aiCapitalYieldChange);
	kStream << m_ppaaiCapitalEraYieldChange;
	kStream << m_ppaaiCapitalTechYieldChange;
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_aiCityYieldChange);
	kStream << m_ppaaiCityEraYieldChange;
	kStream << m_ppaaiCityTechYieldChange;
#endif
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	kStream << m_ppaaiGreatWorkClassYieldChange;
#endif

	kStream << (int)m_aUniqueLuxuryAreas.size();
	for (unsigned int iI = 0; iI < m_aUniqueLuxuryAreas.size(); iI++)
	{
		kStream << m_aUniqueLuxuryAreas[iI];
	}
#ifdef LEKMOD_TRAIT_FIRST_PROPHET_COST_MOD
	kStream << m_iFirstProphetCostMod;
#endif
#if defined(LEKMOD_GOLDEN_AGE_YIELD_THRESHOLD)
	kStream << m_sGoldenAgeYieldThreshold;
#endif
	kStream << m_vUsedGroupAreas;
	kStream << m_vGroupPriority;
}




bool CvPlayerTraits::ConvertBarbarianCamp(CvPlot* pPlot)
{
	UnitHandle pGiftUnit;


	if(pPlot->IsBarbarianCampNotConverting())
	{
		return false;
	}


	if(GC.getGame().getJonRandNum(100, "Barbarian Camp Conversion") < m_iLandBarbarianConversionPercent)
	{
		pPlot->setImprovementType(NO_IMPROVEMENT);

		int iNumGold =        GC.getGOLD_FROM_BARBARIAN_CONVERSION();
		m_pPlayer->GetTreasury()->ChangeGold(iNumGold);


		pPlot->SetPlayerThatClearedBarbCampHere(m_pPlayer->GetID());


		FAssertMsg(m_eCampGuardType < GC.getNumUnitInfos(), "Illegal camp guard unit type");
		pGiftUnit = m_pPlayer->initUnit(m_eCampGuardType, pPlot->getX(), pPlot->getY(), NO_UNITAI, NO_DIRECTION, true            );
		if (!pGiftUnit->jumpToNearestValidPlot())
			pGiftUnit->kill(false);
		else
			pGiftUnit->finishMoves();


		for(int iI = 0; iI < m_iLandBarbarianConversionExtraUnits; iI++)
		{
			pGiftUnit = m_pPlayer->initUnit(m_eCampGuardType, pPlot->getX(), pPlot->getY(), NO_UNITAI, NO_DIRECTION, true            );
			if (!pGiftUnit->jumpToNearestValidPlot())
				pGiftUnit->kill(false);
			else
				pGiftUnit->finishMoves();
		}

		if(GC.getLogging() && GC.getAILogging())
		{
			CvString logMsg;
			logMsg.Format("Converted barbarian camp, X: %d, Y: %d", pPlot->getX(), pPlot->getY());
			m_pPlayer->GetHomelandAI()->LogHomelandMessage(logMsg);
		}

		CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BARB_CAMP_CONVERTS");
		CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BARB_CAMP_CONVERTS");
		m_pPlayer->GetNotifications()->Add(NOTIFICATION_GENERIC, strBuffer, strSummary, pPlot->getX(), pPlot->getY(), -1);

		if(m_pPlayer->isHuman() &&!GC.getGame().isGameMultiPlayer())
		{
			gDLL->IncrementSteamStatAndUnlock(ESTEAMSTAT_BARBSCONVERTED, 10, ACHIEVEMENT_SPECIAL_BARBARIANWARLORD);
		}
	}


	else
	{
		pPlot->SetBarbarianCampNotConverting(true);
		return false;
	}

	return true;
}


bool CvPlayerTraits::ConvertBarbarianNavalUnit(UnitHandle pUnit)
{
	UnitHandle pGiftUnit;


	if(pUnit->IsNotConverting())
	{
		return false;
	}


	if(GC.getGame().getJonRandNum(100, "Barbarian Naval Unit Conversion") < m_iSeaBarbarianConversionPercent)
	{
		int iNumGold =        GC.getGOLD_FROM_BARBARIAN_CONVERSION();
		m_pPlayer->GetTreasury()->ChangeGold(iNumGold);


#ifdef AUI_UNIT_FIX_GIFTED_UNITS_ARE_GIFTED_NOT_CLONED
		pGiftUnit = m_pPlayer->initUnit(pUnit->getUnitType(), pUnit->getX(), pUnit->getY(), pUnit->AI_getUnitAIType(), NO_DIRECTION, true            , false, DEFAULT_UNIT_MAP_LAYER, 0, true);
#else
		pGiftUnit = m_pPlayer->initUnit(pUnit->getUnitType(), pUnit->getX(), pUnit->getY(), pUnit->AI_getUnitAIType(), NO_DIRECTION, true            , false);
#endif
		CvAssertMsg(pGiftUnit, "GiftUnit is not assigned a valid value");
		pGiftUnit->convert(pUnit.pointer(), false);
		pGiftUnit->setupGraphical();
		pGiftUnit->finishMoves();


		if(m_pPlayer->isHuman() && !GC.getGame().isGameMultiPlayer() && m_pPlayer->getLeaderInfo().GetType() && _stricmp(m_pPlayer->getLeaderInfo().GetType(), "LEADER_SULEIMAN") == 0)
		{
			gDLL->IncrementSteamStatAndUnlock(ESTEAMSTAT_BARBSNAVALCONVERTED, 10, ACHIEVEMENT_SPECIAL_BARBARYPIRATE);
		}

		if(GC.getLogging() && GC.getAILogging())
		{
			CvString logMsg;
			logMsg.Format("Converted barbarian naval unit, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
			m_pPlayer->GetHomelandAI()->LogHomelandMessage(logMsg);
		}

		CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BARB_NAVAL_UNIT_CONVERTS");
		CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BARB_NAVAL_UNIT_CONVERTS");
		m_pPlayer->GetNotifications()->Add(NOTIFICATION_GENERIC, strBuffer, strSummary, pUnit->getX(), pUnit->getY(), -1);
		return true;
	}


	else
	{
		pUnit->SetNotConverting(true);
		return false;
	}
}
