






#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvInfosSerializationHelper.h"
#include "CvBarbarians.h"
#ifdef LEKMOD_BELIEF_BUILDING_PURCHASE
#include "CvInternalGameCoreUtils.h"
#include "CvReligionClasses.h"
#endif

#include "LintFree.h"





CvBeliefEntry::CvBeliefEntry() :
	m_iMinPopulation(0),
	m_iMinFollowers(0),
	m_iMaxDistance(0),
	m_iCityGrowthModifier(0),
	m_iFaithFromKills(0),
	m_iFaithFromDyingUnits(0),
	m_iRiverHappiness(0),
	m_iHappinessPerCity(0),
	m_iHappinessPerXPeacefulForeignFollowers(0),
	m_iPlotCultureCostModifier(0),
	m_iCityRangeStrikeModifier(0),
	m_iCombatModifierEnemyCities(0),
	m_iCombatModifierFriendlyCities(0),
	m_iFriendlyHealChange(0),
	m_iCityStateFriendshipModifier(0),
	m_iLandBarbarianConversionPercent(0),
	m_iWonderProductionModifier(0),
	m_iPlayerHappiness(0),
	m_iPlayerCultureModifier(0),
	m_fHappinessPerFollowingCity(0),
	m_iGoldPerFollowingCity(0),
	m_iGoldPerXFollowers(0),
	m_iGoldWhenCityAdopts(0),
	m_iSciencePerOtherReligionFollower(0),
	m_iSpreadDistanceModifier(0),
	m_iSpreadStrengthModifier(0),
	m_iProphetStrengthModifier(0),
	m_iProphetCostModifier(0),
	m_iMissionaryStrengthModifier(0),
#ifdef NQ_FLAT_FAITH_PER_CITIZEN_BORN_FROM_BELIEFS
	m_iFlatFaithPerCitizenBorn(0),
#endif
#ifdef NQ_BELIEF_EXTRA_MISSIONARY_SPREADS
	m_iMissionaryExtraSpreads(0),
#endif
#ifdef NQ_SPREAD_MODIFIER_OWNED_CITIES
	m_iSpreadModifierOwnedCities(0),
	m_iSpreadModifierUnownedCities(0),
#endif
#ifdef NQ_EXTRA_TRADE_ROUTES_FROM_BELIEF
	m_iExtraTradeRoutes(0),
#endif
#ifdef NQ_FAITH_PER_CITY_STATE_THIS_RELIGION
	m_iFaithPerCityStateThisReligion(0),
#endif
#ifdef NQ_FAITH_PER_FOREIGN_TRADE_ROUTE
	m_iFaithPerForeignTradeRoute(0),
#endif
#ifdef NQ_GOLDEN_AGE_TURNS_FROM_BELIEF
	m_iGoldenAgeTurns(0),
#endif
	m_iMissionaryCostModifier(0),
	m_iFriendlyCityStateSpreadModifier(0),
	m_iGreatPersonExpendedFaith(0),
	m_iCityStateMinimumInfluence(0),
	m_iCityStateInfluenceModifier(0),
#ifdef LEKMOD_BELIEF_CITY_STATE_FOLLOWING_RELIGION_INFLUENCE
	m_iCityStateFollowingReligionDecayMod(0),
	m_iCityStateFollowingReligionRecoveryMod(0),
#endif
	m_iOtherReligionPressureErosion(0),
	m_iSpyPressure(0),
	m_iInquisitorPressureRetention(0),
	m_iFaithBuildingTourism(0),
#ifdef NQ_FREE_SETTLERS_FROM_BELIEF
	m_iNumFreeSettlers(0),
#endif

	m_bPantheon(false),
	m_bFounder(false),
	m_bFollower(false),
	m_bEnhancer(false),
	m_bReformer(false),
	m_bRequiresPeace(false),
	m_bConvertsBarbarians(false),
#ifdef NQ_DEUS_VULT
	m_bDeusVult(false),
#endif
	m_bFaithPurchaseAllGreatPeople(false),
#ifdef NQ_BELIEF_TOGGLE_ALLOW_FAITH_GIFTS_TO_MINORS
	m_bAllowsFaithGiftsToMinors(false),
#endif

	m_eObsoleteEra(NO_ERA),
	m_eResourceRevealed(NO_RESOURCE),
	m_eSpreadModifierDoublingTech(NO_TECH),

	m_paiCityYieldChange(NULL),
	m_paiHolyCityYieldChange(NULL),
	m_paiYieldChangePerForeignCity(NULL),
#if defined(LEKMOD_BELIEF_YIELDIFY)
	m_paiYieldChangePerFollowingCity(NULL),
	m_paiPlayerYieldModifier(NULL),
#endif
#if defined(LEK_CULTURE_SCIENCE_SPREAD_BELIEFS_ALL_CITIES)
	m_paiYieldChangePerXFollowers(NULL),
#endif
	m_paiYieldChangePerXForeignFollowers(NULL),
	m_piResourceQuantityModifiers(NULL),
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	m_ppiImprovementYieldChanges(std::pair<int**, size_t>(NULL, 0)),
	m_ppiBuildingClassYieldChanges(std::pair<int**, size_t>(NULL, 0)),
#ifdef AUI_BELIEF_BUILDING_CLASS_FLAVOR_MODIFIERS
	m_ppiBuildingClassFlavorChanges(std::pair<int**, size_t>(NULL, 0)),
#endif
#else
	m_ppiImprovementYieldChanges(NULL),
	m_ppiBuildingClassYieldChanges(NULL),
#ifdef AUI_BELIEF_BUILDING_CLASS_FLAVOR_MODIFIERS
	m_ppiBuildingClassFlavorChanges(NULL),
#endif
#endif
	m_paiBuildingClassHappiness(NULL),
	m_paiBuildingClassTourism(NULL),
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	m_ppaiFeatureYieldChange(std::pair<int**, size_t>(NULL, 0)),
	m_ppaiResourceYieldChange(std::pair<int**, size_t>(NULL, 0)),
	m_ppaiTerrainYieldChange(std::pair<int**, size_t>(NULL, 0)),
#else
	m_ppaiFeatureYieldChange(NULL),
	m_ppaiResourceYieldChange(NULL),
	m_ppaiTerrainYieldChange(NULL),
#endif
	m_piResourceHappiness(NULL),
	m_piYieldChangeAnySpecialist(NULL),
	m_piYieldChangeTradeRoute(NULL),
#if defined(TRADE_REFACTOR)
	m_ppiTradeConnectionOriginLandYieldChange(NULL),
	m_ppiTradeConnectionOriginSeaYieldChange(NULL),
	m_ppiIncomingTradeConnectionLandYieldChange(NULL),
	m_ppiIncomingTradeConnectionSeaYieldChange(NULL),
#endif
	m_piYieldChangeNaturalWonder(NULL),
	m_piYieldChangeWorldWonder(NULL),
	m_piYieldModifierNaturalWonder(NULL),
	m_piMaxYieldModifierPerFollower(NULL),
	m_pbFaithPurchaseUnitEraEnabled(NULL),
	m_pbBuildingClassEnabled(NULL)
{
}


CvBeliefEntry::~CvBeliefEntry()
{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	CvDatabaseUtility::SafeDelete2DArray(m_ppiImprovementYieldChanges.first, m_ppiImprovementYieldChanges.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiBuildingClassYieldChanges.first, m_ppiBuildingClassYieldChanges.second);
#ifdef AUI_BELIEF_BUILDING_CLASS_FLAVOR_MODIFIERS
	CvDatabaseUtility::SafeDelete2DArray(m_ppiBuildingClassFlavorChanges.first, m_ppiBuildingClassFlavorChanges.second);
#endif
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiFeatureYieldChange.first, m_ppaiFeatureYieldChange.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiResourceYieldChange.first, m_ppaiResourceYieldChange.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiTerrainYieldChange.first, m_ppaiTerrainYieldChange.second);
#else
	CvDatabaseUtility::SafeDelete2DArray(m_ppiImprovementYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiBuildingClassYieldChanges);
#ifdef AUI_BELIEF_BUILDING_CLASS_FLAVOR_MODIFIERS
	CvDatabaseUtility::SafeDelete2DArray(m_ppiBuildingClassFlavorChanges);
#endif
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiFeatureYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiResourceYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiTerrainYieldChange);
#endif
}


int CvBeliefEntry::GetMinPopulation() const
{
	return m_iMinPopulation;
}


int CvBeliefEntry::GetMinFollowers() const
{
	return m_iMinFollowers;
}


int CvBeliefEntry::GetMaxDistance() const
{
	return m_iMaxDistance;
}


int CvBeliefEntry::GetCityGrowthModifier() const
{
	return m_iCityGrowthModifier;
}


int CvBeliefEntry::GetFaithFromKills() const
{
	return m_iFaithFromKills;
}


int CvBeliefEntry::GetFaithFromDyingUnits() const
{
	return m_iFaithFromDyingUnits;
}


int CvBeliefEntry::GetRiverHappiness() const
{
	return m_iRiverHappiness;
}


int CvBeliefEntry::GetHappinessPerCity() const
{
	return m_iHappinessPerCity;
}


int CvBeliefEntry::GetHappinessPerXPeacefulForeignFollowers() const
{
	return m_iHappinessPerXPeacefulForeignFollowers;
}


int CvBeliefEntry::GetPlotCultureCostModifier() const
{
	return m_iPlotCultureCostModifier;
}


int CvBeliefEntry::GetCityRangeStrikeModifier() const
{
	return m_iCityRangeStrikeModifier;
}


int CvBeliefEntry::GetCombatModifierEnemyCities() const
{
	return m_iCombatModifierEnemyCities;
}


int CvBeliefEntry::GetCombatModifierFriendlyCities() const
{
	return m_iCombatModifierFriendlyCities;
}


int CvBeliefEntry::GetFriendlyHealChange() const
{
	return m_iFriendlyHealChange;
}


int CvBeliefEntry::GetCityStateFriendshipModifier() const
{
	return m_iCityStateFriendshipModifier;
}


int CvBeliefEntry::GetLandBarbarianConversionPercent() const
{
	return m_iLandBarbarianConversionPercent;
}


int CvBeliefEntry::GetWonderProductionModifier() const
{
	return m_iWonderProductionModifier;
}


int CvBeliefEntry::GetPlayerHappiness() const
{
	return m_iPlayerHappiness;
}


int CvBeliefEntry::GetPlayerCultureModifier() const
{
	return m_iPlayerCultureModifier;
}


float CvBeliefEntry::GetHappinessPerFollowingCity() const
{
	return m_fHappinessPerFollowingCity;
}


int CvBeliefEntry::GetGoldPerFollowingCity() const
{
	return m_iGoldPerFollowingCity;
}


int CvBeliefEntry::GetGoldPerXFollowers() const
{
	return m_iGoldPerXFollowers;
}


int CvBeliefEntry::GetGoldWhenCityAdopts() const
{
	return m_iGoldWhenCityAdopts;
}


int CvBeliefEntry::GetSciencePerOtherReligionFollower() const
{
	return m_iSciencePerOtherReligionFollower;
}


int CvBeliefEntry::GetSpreadDistanceModifier() const
{
	return m_iSpreadDistanceModifier;
}


int CvBeliefEntry::GetSpreadStrengthModifier() const
{
	return m_iSpreadStrengthModifier;
}


int CvBeliefEntry::GetProphetStrengthModifier() const
{
	return m_iProphetStrengthModifier;
}


int CvBeliefEntry::GetProphetCostModifier() const
{
	return m_iProphetCostModifier;
}


int CvBeliefEntry::GetMissionaryStrengthModifier() const
{
	return m_iMissionaryStrengthModifier;
}

#ifdef NQ_FLAT_FAITH_PER_CITIZEN_BORN_FROM_BELIEFS

int CvBeliefEntry::GetFlatFaithPerCitizenBorn() const
{
	return m_iFlatFaithPerCitizenBorn;
}
#endif

#ifdef NQ_BELIEF_EXTRA_MISSIONARY_SPREADS

int CvBeliefEntry::GetMissionaryExtraSpreads() const
{
	return m_iMissionaryExtraSpreads;
}
#endif

#ifdef NQ_SPREAD_MODIFIER_OWNED_CITIES

int CvBeliefEntry::GetSpreadModifierOwnedCities() const
{
	return m_iSpreadModifierOwnedCities;
}


int CvBeliefEntry::GetSpreadModifierUnownedCities() const
{
	return m_iSpreadModifierUnownedCities;
}
#endif

#ifdef NQ_EXTRA_TRADE_ROUTES_FROM_BELIEF

int CvBeliefEntry::GetExtraTradeRoutes() const
{
	return m_iExtraTradeRoutes;
}
#endif

#ifdef NQ_FAITH_PER_CITY_STATE_THIS_RELIGION

int CvBeliefEntry::GetFaithPerCityStateThisReligion() const
{
	return m_iFaithPerCityStateThisReligion;
}
#endif

#ifdef NQ_FAITH_PER_FOREIGN_TRADE_ROUTE

int CvBeliefEntry::GetFaithPerForeignTradeRoute() const
{
	return m_iFaithPerForeignTradeRoute;
}
#endif

#ifdef NQ_GOLDEN_AGE_TURNS_FROM_BELIEF

int CvBeliefEntry::GetGoldenAgeTurns() const
{
	return m_iGoldenAgeTurns;
}
#endif


int CvBeliefEntry::GetMissionaryCostModifier() const
{
	return m_iMissionaryCostModifier;
}


int CvBeliefEntry::GetFriendlyCityStateSpreadModifier() const
{
	return m_iFriendlyCityStateSpreadModifier;
}


int CvBeliefEntry::GetGreatPersonExpendedFaith() const
{
	return m_iGreatPersonExpendedFaith;
}


int CvBeliefEntry::GetCityStateMinimumInfluence() const
{
	return m_iCityStateMinimumInfluence;
}


int CvBeliefEntry::GetCityStateInfluenceModifier() const
{
	return m_iCityStateInfluenceModifier;
}

#ifdef LEKMOD_BELIEF_CITY_STATE_FOLLOWING_RELIGION_INFLUENCE
int CvBeliefEntry::GetCityStateFollowingReligionDecayMod() const
{
	return m_iCityStateFollowingReligionDecayMod;
}

int CvBeliefEntry::GetCityStateFollowingReligionRecoveryMod() const
{
	return m_iCityStateFollowingReligionRecoveryMod;
}
#endif


int CvBeliefEntry::GetOtherReligionPressureErosion() const
{
	return m_iOtherReligionPressureErosion;
}


int CvBeliefEntry::GetSpyPressure() const
{
	return m_iSpyPressure;
}


int CvBeliefEntry::GetInquisitorPressureRetention() const
{
	return m_iInquisitorPressureRetention;
}


int CvBeliefEntry::GetFaithBuildingTourism() const
{
	return m_iFaithBuildingTourism;
}

#ifdef NQ_FREE_SETTLERS_FROM_BELIEF

int CvBeliefEntry::GetNumFreeSettlers() const
{
	return m_iNumFreeSettlers;
}
#endif


bool CvBeliefEntry::IsPantheonBelief() const
{
	return m_bPantheon;
}


bool CvBeliefEntry::IsFounderBelief() const
{
	return m_bFounder;
}


bool CvBeliefEntry::IsFollowerBelief() const
{
	return m_bFollower;
}


bool CvBeliefEntry::IsEnhancerBelief() const
{
	return m_bEnhancer;
}


bool CvBeliefEntry::IsReformationBelief() const
{
	return m_bReformer;
}


bool CvBeliefEntry::RequiresPeace() const
{
	return m_bRequiresPeace;
}


bool CvBeliefEntry::ConvertsBarbarians() const
{
	return m_bConvertsBarbarians;
}

#ifdef NQ_DEUS_VULT

bool CvBeliefEntry::DeusVult() const
{
	return m_bDeusVult;
}
#endif


bool CvBeliefEntry::FaithPurchaseAllGreatPeople() const
{
	return m_bFaithPurchaseAllGreatPeople;
}

#ifdef NQ_BELIEF_TOGGLE_ALLOW_FAITH_GIFTS_TO_MINORS

bool CvBeliefEntry::AllowsFaithGiftsToMinors() const
{
	return m_bAllowsFaithGiftsToMinors;
}
#endif


EraTypes CvBeliefEntry::GetObsoleteEra() const
{
	return m_eObsoleteEra;
}


ResourceTypes CvBeliefEntry::GetResourceRevealed() const
{
	return m_eResourceRevealed;
}


TechTypes CvBeliefEntry::GetSpreadModifierDoublingTech() const
{
	return m_eSpreadModifierDoublingTech;
}


const char* CvBeliefEntry::getShortDescription() const
{
	return m_strShortDescription;
}


void CvBeliefEntry::setShortDescription(const char* szVal)
{
	m_strShortDescription = szVal;
}


int CvBeliefEntry::GetCityYieldChange(int i) const
{
	return m_paiCityYieldChange ? m_paiCityYieldChange[i] : -1;
}


int CvBeliefEntry::GetHolyCityYieldChange(int i) const
{
	return m_paiHolyCityYieldChange ? m_paiHolyCityYieldChange[i] : -1;
}


int CvBeliefEntry::GetYieldChangePerForeignCity(int i) const
{
	return m_paiYieldChangePerForeignCity ? m_paiYieldChangePerForeignCity[i] : -1;
}
#if defined(LEKMOD_BELIEF_YIELDIFY)

int CvBeliefEntry::GetYieldChangePerFollowingCity(int i) const
{
	return m_paiYieldChangePerFollowingCity ? m_paiYieldChangePerFollowingCity[i] : -1;
}

int CvBeliefEntry::GetPlayerYieldModifier(int i) const
{
	return m_paiPlayerYieldModifier ? m_paiPlayerYieldModifier[i] : -1;
}
#endif
#if defined(LEK_CULTURE_SCIENCE_SPREAD_BELIEFS_ALL_CITIES)
int CvBeliefEntry::GetYieldChangePerXFollowers(int i) const
{
	return m_paiYieldChangePerXFollowers ? m_paiYieldChangePerXFollowers[i] : -1;
}
#endif

int CvBeliefEntry::GetYieldChangePerXForeignFollowers(int i) const
{
	return m_paiYieldChangePerXForeignFollowers ? m_paiYieldChangePerXForeignFollowers[i] : -1;
}


int CvBeliefEntry::GetResourceQuantityModifier(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piResourceQuantityModifiers ? m_piResourceQuantityModifiers[i] : -1;
}


int CvBeliefEntry::GetImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const
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


int CvBeliefEntry::GetBuildingClassYieldChange(int i, int j) const
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

#ifdef AUI_BELIEF_BUILDING_CLASS_FLAVOR_MODIFIERS

int CvBeliefEntry::GetBuildingClassFlavorChange(int i, int j) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppiBuildingClassFlavorChanges.first ? m_ppiBuildingClassFlavorChanges.first[i][j] : 0;
#else
	return m_ppiBuildingClassFlavorChanges[i][j];
#endif
}
#endif



int CvBeliefEntry::GetBuildingClassHappiness(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiBuildingClassHappiness ? m_paiBuildingClassHappiness[i] : -1;
}


int CvBeliefEntry::GetBuildingClassTourism(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiBuildingClassTourism ? m_paiBuildingClassTourism[i] : -1;
}


int CvBeliefEntry::GetFeatureYieldChange(int i, int j) const
{
	CvAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppaiFeatureYieldChange.first ? m_ppaiFeatureYieldChange.first[i][j] : -1;
#else
	return m_ppaiFeatureYieldChange ? m_ppaiFeatureYieldChange[i][j] : -1;
#endif
}


int CvBeliefEntry::GetResourceYieldChange(int i, int j) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppaiResourceYieldChange.first ? m_ppaiResourceYieldChange.first[i][j] : -1;
#else
	return m_ppaiResourceYieldChange ? m_ppaiResourceYieldChange[i][j] : -1;
#endif
}


int CvBeliefEntry::GetTerrainYieldChange(int i, int j) const
{
	CvAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppaiTerrainYieldChange.first ? m_ppaiTerrainYieldChange.first[i][j] : -1;
#else
	return m_ppaiTerrainYieldChange ? m_ppaiTerrainYieldChange[i][j] : -1;
#endif
}


int CvBeliefEntry::GetResourceHappiness(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piResourceHappiness ? m_piResourceHappiness[i] : -1;
}


int CvBeliefEntry::GetYieldChangeAnySpecialist(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChangeAnySpecialist ? m_piYieldChangeAnySpecialist[i] : -1;
}


int CvBeliefEntry::GetYieldChangeTradeRoute(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChangeTradeRoute ? m_piYieldChangeTradeRoute[i] : -1;
}
#if defined(TRADE_REFACTOR)

int CvBeliefEntry::GetTradeConnectionOriginLandYieldChange(int i, int j) const
{
	CvAssertMsg(i < NUM_TRADE_CONNECTION_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiTradeConnectionOriginLandYieldChange ? m_ppiTradeConnectionOriginLandYieldChange[i][j] : 0;
}
int CvBeliefEntry::GetTradeConnectionOriginSeaYieldChange(int i, int j) const
{
	CvAssertMsg(i < NUM_TRADE_CONNECTION_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiTradeConnectionOriginSeaYieldChange ? m_ppiTradeConnectionOriginSeaYieldChange[i][j] : 0;
}

int CvBeliefEntry::GetIncomingTradeConnectionLandYieldChange(int i, int j) const
{
	CvAssertMsg(i < NUM_TRADE_CONNECTION_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiIncomingTradeConnectionLandYieldChange ? m_ppiIncomingTradeConnectionLandYieldChange[i][j] : 0;
}
int CvBeliefEntry::GetIncomingTradeConnectionSeaYieldChange(int i, int j) const
{
	CvAssertMsg(i < NUM_TRADE_CONNECTION_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiIncomingTradeConnectionSeaYieldChange ? m_ppiIncomingTradeConnectionSeaYieldChange[i][j] : 0;
}
#endif

int CvBeliefEntry::GetYieldChangeNaturalWonder(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChangeNaturalWonder ? m_piYieldChangeNaturalWonder[i] : -1;
}


int CvBeliefEntry::GetYieldChangeWorldWonder(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChangeWorldWonder ? m_piYieldChangeWorldWonder[i] : -1;
}


int CvBeliefEntry::GetYieldModifierNaturalWonder(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldModifierNaturalWonder ? m_piYieldModifierNaturalWonder[i] : -1;
}


int CvBeliefEntry::GetMaxYieldModifierPerFollower(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piMaxYieldModifierPerFollower ? m_piMaxYieldModifierPerFollower[i] : -1;
}


bool CvBeliefEntry::IsFaithUnitPurchaseEra(int i) const
{
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(i < GC.getNumEraInfos(), "Index out of bounds");
	return m_pbFaithPurchaseUnitEraEnabled ? m_pbFaithPurchaseUnitEraEnabled[i] : false;
}


bool CvBeliefEntry::IsBuildingClassEnabled(int i) const
{
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	return m_pbBuildingClassEnabled ? m_pbBuildingClassEnabled[i] : false;
}


bool CvBeliefEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;


	setShortDescription(kResults.GetText("ShortDescription"));

	m_iMinPopulation                  = kResults.GetInt("MinPopulation");
	m_iMinFollowers                   = kResults.GetInt("MinFollowers");
	m_iMaxDistance					  = kResults.GetInt("MaxDistance");
	m_iCityGrowthModifier		      = kResults.GetInt("CityGrowthModifier");
	m_iFaithFromKills				  = kResults.GetInt("FaithFromKills");
	m_iFaithFromDyingUnits			  = kResults.GetInt("FaithFromDyingUnits");
	m_iRiverHappiness				  = kResults.GetInt("RiverHappiness");
	m_iHappinessPerCity				  = kResults.GetInt("HappinessPerCity");
	m_iHappinessPerXPeacefulForeignFollowers  = kResults.GetInt("HappinessPerXPeacefulForeignFollowers");
	m_iPlotCultureCostModifier	      = kResults.GetInt("PlotCultureCostModifier");
	m_iCityRangeStrikeModifier	      = kResults.GetInt("CityRangeStrikeModifier");
	m_iCombatModifierEnemyCities      = kResults.GetInt("CombatModifierEnemyCities");
	m_iCombatModifierFriendlyCities   = kResults.GetInt("CombatModifierFriendlyCities");
	m_iFriendlyHealChange	          = kResults.GetInt("FriendlyHealChange");
	m_iCityStateFriendshipModifier    = kResults.GetInt("CityStateFriendshipModifier");
	m_iLandBarbarianConversionPercent = kResults.GetInt("LandBarbarianConversionPercent");
	m_iWonderProductionModifier       = kResults.GetInt("WonderProductionModifier");
	m_iPlayerHappiness			      = kResults.GetInt("PlayerHappiness");
	m_fHappinessPerFollowingCity      = kResults.GetFloat("HappinessPerFollowingCity");
#if !defined(LEKMOD_BELIEF_YIELDIFY)
	m_iGoldPerFollowingCity           = kResults.GetInt("GoldPerFollowingCity");
	m_iPlayerCultureModifier          = kResults.GetInt("PlayerCultureModifier");
#endif
#if !defined(LEK_CULTURE_SCIENCE_SPREAD_BELIEFS_ALL_CITIES)
	m_iGoldPerXFollowers              = kResults.GetInt("GoldPerXFollowers");
#endif
	m_iGoldWhenCityAdopts             = kResults.GetInt("GoldPerFirstCityConversion");
	m_iSciencePerOtherReligionFollower= kResults.GetInt("SciencePerOtherReligionFollower");
	m_iSpreadDistanceModifier         = kResults.GetInt("SpreadDistanceModifier");
	m_iSpreadStrengthModifier		  = kResults.GetInt("SpreadStrengthModifier");
	m_iProphetStrengthModifier        = kResults.GetInt("ProphetStrengthModifier");
	m_iProphetCostModifier            = kResults.GetInt("ProphetCostModifier");
	m_iMissionaryStrengthModifier     = kResults.GetInt("MissionaryStrengthModifier");
#ifdef NQ_FLAT_FAITH_PER_CITIZEN_BORN_FROM_BELIEFS
	m_iFlatFaithPerCitizenBorn        = kResults.GetInt("FlatFaithPerCitizenBorn");
#endif
#ifdef NQ_BELIEF_EXTRA_MISSIONARY_SPREADS
	m_iMissionaryExtraSpreads		  = kResults.GetInt("MissionaryExtraSpreads");
#endif
#ifdef NQ_SPREAD_MODIFIER_OWNED_CITIES
	m_iSpreadModifierOwnedCities	  = kResults.GetInt("SpreadModifierOwnedCities");
	m_iSpreadModifierUnownedCities	  = kResults.GetInt("SpreadModifierUnownedCities");
#endif
#ifdef NQ_EXTRA_TRADE_ROUTES_FROM_BELIEF
	m_iExtraTradeRoutes				  = kResults.GetInt("ExtraTradeRoutes");
#endif
#ifdef NQ_FAITH_PER_CITY_STATE_THIS_RELIGION
	m_iFaithPerCityStateThisReligion  = kResults.GetInt("FaithPerCityStateThisReligion");
#endif
#ifdef NQ_FAITH_PER_FOREIGN_TRADE_ROUTE
	m_iFaithPerForeignTradeRoute	  = kResults.GetInt("FaithPerForeignTradeRoute");
#endif
#ifdef NQ_GOLDEN_AGE_TURNS_FROM_BELIEF
	m_iGoldenAgeTurns				  = kResults.GetInt("GoldenAgeTurns");
#endif
	m_iMissionaryCostModifier         = kResults.GetInt("MissionaryCostModifier");
	m_iFriendlyCityStateSpreadModifier= kResults.GetInt("FriendlyCityStateSpreadModifier");
	m_iGreatPersonExpendedFaith       = kResults.GetInt("GreatPersonExpendedFaith");
	m_iCityStateMinimumInfluence      = kResults.GetInt("CityStateMinimumInfluence");
	m_iCityStateInfluenceModifier     = kResults.GetInt("CityStateInfluenceModifier");
#ifdef LEKMOD_BELIEF_CITY_STATE_FOLLOWING_RELIGION_INFLUENCE
	m_iCityStateFollowingReligionDecayMod = kResults.GetInt("CityStateFollowingReligionDecayMod");
	m_iCityStateFollowingReligionRecoveryMod = kResults.GetInt("CityStateFollowingReligionRecoveryMod");
#endif
	m_iOtherReligionPressureErosion   = kResults.GetInt("OtherReligionPressureErosion");
	m_iSpyPressure					  = kResults.GetInt("SpyPressure");
	m_iInquisitorPressureRetention    = kResults.GetInt("InquisitorPressureRetention");
	m_iFaithBuildingTourism           = kResults.GetInt("FaithBuildingTourism");
#ifdef NQ_FREE_SETTLERS_FROM_BELIEF
	m_iNumFreeSettlers				  = kResults.GetInt("NumFreeSettlers");
#endif

	m_bPantheon						  = kResults.GetBool("Pantheon");
	m_bFounder						  = kResults.GetBool("Founder");
	m_bFollower						  = kResults.GetBool("Follower");
	m_bEnhancer						  = kResults.GetBool("Enhancer");
	m_bReformer						  = kResults.GetBool("Reformation");
	m_bRequiresPeace				  = kResults.GetBool("RequiresPeace");
	m_bConvertsBarbarians			  = kResults.GetBool("ConvertsBarbarians");
#ifdef NQ_DEUS_VULT
	m_bDeusVult						  = kResults.GetBool("DeusVult");
#endif
	m_bFaithPurchaseAllGreatPeople	  = kResults.GetBool("FaithPurchaseAllGreatPeople");
#ifdef NQ_BELIEF_TOGGLE_ALLOW_FAITH_GIFTS_TO_MINORS
	m_bAllowsFaithGiftsToMinors       = kResults.GetBool("AllowsFaithGiftsToMinors");
#endif


	const char* szTextVal;
	szTextVal						  = kResults.GetText("ObsoleteEra");
	m_eObsoleteEra					  = (EraTypes)GC.getInfoTypeForString(szTextVal, true);
	szTextVal						  = kResults.GetText("ResourceRevealed");
	m_eResourceRevealed				  = (ResourceTypes)GC.getInfoTypeForString(szTextVal, true);
	szTextVal						  = kResults.GetText("SpreadModifierDoublingTech");
	m_eSpreadModifierDoublingTech     = (TechTypes)GC.getInfoTypeForString(szTextVal, true);


	const char* szBeliefType = GetType();
	kUtility.SetYields(m_paiCityYieldChange, "Belief_CityYieldChanges", "BeliefType", szBeliefType);
	kUtility.SetYields(m_paiHolyCityYieldChange, "Belief_HolyCityYieldChanges", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldChangeAnySpecialist, "Belief_YieldChangeAnySpecialist", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldChangeTradeRoute, "Belief_YieldChangeTradeRoute", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldChangeNaturalWonder, "Belief_YieldChangeNaturalWonder", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldModifierNaturalWonder, "Belief_YieldModifierNaturalWonder", "BeliefType", szBeliefType);
	kUtility.PopulateArrayByValue(m_piMaxYieldModifierPerFollower, "Yields", "Belief_MaxYieldModifierPerFollower", "YieldType", "BeliefType", szBeliefType, "Max");
	kUtility.PopulateArrayByValue(m_piResourceHappiness, "Resources", "Belief_ResourceHappiness", "ResourceType", "BeliefType", szBeliefType, "HappinessChange");
	kUtility.PopulateArrayByValue(m_piResourceQuantityModifiers, "Resources", "Belief_ResourceQuantityModifiers", "ResourceType", "BeliefType", szBeliefType, "ResourceQuantityModifier");
	kUtility.PopulateArrayByValue(m_paiBuildingClassHappiness, "BuildingClasses", "Belief_BuildingClassHappiness", "BuildingClassType", "BeliefType", szBeliefType, "Happiness");
	kUtility.PopulateArrayByValue(m_paiBuildingClassTourism, "BuildingClasses", "Belief_BuildingClassTourism", "BuildingClassType", "BeliefType", szBeliefType, "Tourism");
#if defined(TRADE_REFACTOR)
	{
		kUtility.Initialize2DArray(m_ppiTradeConnectionOriginLandYieldChange, "TradeConnections", "Yields");
		kUtility.Initialize2DArray(m_ppiTradeConnectionOriginSeaYieldChange, "TradeConnections", "Yields");
		std::string strKey("Belief_TradeConnectionOriginYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT TradeConnections.ID as TradeConnectionsID, Domains.ID as DomainID, Yields.ID as YieldID, YieldTimes100 "
			"FROM Belief_TradeConnectionOriginYieldChanges "
			"INNER JOIN TradeConnections on TradeConnections.Type = TradeConnectionType "
			"INNER JOIN Domains on Domains.Type = DomainType "
			"INNER JOIN Yields on Yields.Type = YieldType "
			"WHERE BeliefType = ? ");
		pResults->Bind(1, szBeliefType);
		while (pResults->Step())
		{
			const int TradeConnectionsID = pResults->GetInt(0);
			const int domainID = pResults->GetInt(1);
			const int YieldID = pResults->GetInt(2);
			const int yieldTimes100 = pResults->GetInt(3);
			if (DOMAIN_LAND == domainID)
				m_ppiTradeConnectionOriginLandYieldChange[TradeConnectionsID][YieldID] = yieldTimes100;
			else if (DOMAIN_SEA == domainID)
				m_ppiTradeConnectionOriginSeaYieldChange[TradeConnectionsID][YieldID] = yieldTimes100;
		}
		pResults->Reset();
	}
	{
		kUtility.Initialize2DArray(m_ppiIncomingTradeConnectionLandYieldChange, "TradeConnections", "Yields");
		kUtility.Initialize2DArray(m_ppiIncomingTradeConnectionSeaYieldChange, "TradeConnections", "Yields");
		std::string strKey("Belief_IncomingTradeConnectionYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"SELECT TradeConnections.ID as TradeConnectionsID, Domains.ID as DomainID, Yields.ID as YieldID, YieldTimes100 "
			"FROM Belief_IncomingTradeConnectionYieldChanges "
			"INNER JOIN TradeConnections on TradeConnections.Type = TradeConnectionType "
			"INNER JOIN Domains on Domains.Type = DomainType "
			"INNER JOIN Yields on Yields.Type = YieldType "
			"WHERE BeliefType = ? ");
		pResults->Bind(1, szBeliefType);
		while (pResults->Step())
		{
			const int TradeConnectionsID = pResults->GetInt(0);
			const int domainID = pResults->GetInt(1);
			const int YieldID = pResults->GetInt(2);
			const int yieldTimes100 = pResults->GetInt(3);
			if (DOMAIN_LAND == domainID)
				m_ppiIncomingTradeConnectionLandYieldChange[TradeConnectionsID][YieldID] = yieldTimes100;
			else if (DOMAIN_SEA == domainID)
				m_ppiIncomingTradeConnectionSeaYieldChange[TradeConnectionsID][YieldID] = yieldTimes100;
		}
		pResults->Reset();
	}
#endif
	#if !defined(LEK_CULTURE_SCIENCE_SPREAD_BELIEFS_ALL_CITIES)
	kUtility.PopulateArrayByValue(m_paiYieldChangePerXForeignFollowers, "Yields", "Belief_YieldChangePerXForeignFollowers", "YieldType", "BeliefType", szBeliefType, "ForeignFollowers");
#else

	int iGoldPerXFollowers = kResults.GetInt("GoldPerXFollowers");

	{
		kUtility.InitializeArray(m_paiYieldChangePerXForeignFollowers, "Yields");
		kUtility.InitializeArray(m_paiYieldChangePerXFollowers, "Yields");
		std::string key("Belief_YieldChangePerXFollowers");
		Database::Results* results = kUtility.GetOrPrepareResults(key,
			"SELECT Yields.ID as YieldID, YieldPerXFollowers, YieldPerXForeignFollowers "
			"FROM Belief_YieldChangePerXFollowers "
			"INNER JOIN Yields ON Yields.Type = YieldType "
			"WHERE BeliefType = ?");
		results->Bind(1, szBeliefType);
		while (results->Step())
		{
			const int YieldID = results->GetInt(0);
			const int YieldPerXFollowers = results->GetInt(1);
			const int YieldPerXForeignFollowers = results->GetInt(2);
			m_paiYieldChangePerXFollowers[YieldID] = YieldPerXFollowers;
			m_paiYieldChangePerXForeignFollowers[YieldID] = YieldPerXForeignFollowers;
			if (YieldID == YIELD_GOLD)
			{
				m_paiYieldChangePerXFollowers[YieldID] += iGoldPerXFollowers;
			}
		}
		results->Reset();
	}
#endif
#if !defined(LEKMOD_BELIEF_YIELDIFY)
	kUtility.PopulateArrayByValue(m_paiYieldChangePerForeignCity, "Yields", "Belief_YieldChangePerForeignCity", "YieldType", "BeliefType", szBeliefType, "Yield");
#else
	int iGoldPerFollowingCity = kResults.GetInt("GoldPerFollowingCity");
	{
		kUtility.InitializeArray(m_paiYieldChangePerFollowingCity, "Yields");
		kUtility.InitializeArray(m_paiYieldChangePerForeignCity, "Yields");
		std::string key("Belief_YieldChangePerFollowingCity");
		Database::Results* results = kUtility.GetOrPrepareResults(key,
			"SELECT Yields.ID as YieldID, YieldPerFollowingCity, YieldPerForeignCity "
			"FROM Belief_YieldChangePerFollowingCity "
			"INNER JOIN Yields ON Yields.Type = YieldType "
			"WHERE BeliefType = ?");
		results->Bind(1, szBeliefType);
		while (results->Step())
		{
			const int YieldID = results->GetInt(0);
			const int YieldPerFollowingCity = results->GetInt(1);
			const int YieldPerForeignCity = results->GetInt(2);
			m_paiYieldChangePerFollowingCity[YieldID] = YieldPerFollowingCity;
			m_paiYieldChangePerForeignCity[YieldID] = YieldPerForeignCity;
			if (YieldID == YIELD_GOLD)
			{
				m_paiYieldChangePerFollowingCity[YieldID] += iGoldPerFollowingCity;
			}
		}
		results->Reset();
	}
	int iPlayerCultureModifier = kResults.GetInt("PlayerCultureModifier");
	{
		kUtility.InitializeArray(m_paiPlayerYieldModifier, "Yields");
		std::string key("Belief_PlayerYieldModifier");
		Database::Results* results = kUtility.GetOrPrepareResults(key,
			"SELECT Yields.ID as YieldID, PlayerYieldModifier "
			"FROM Belief_PlayerYieldModifier "
			"INNER JOIN Yields ON Yields.Type = YieldType "
			"WHERE BeliefType = ?");
		results->Bind(1, szBeliefType);
		while (results->Step())
		{
			const int YieldID = results->GetInt(0);
			const int PlayerYieldModifier = results->GetInt(1);
			m_paiPlayerYieldModifier[YieldID] = PlayerYieldModifier;
			if (YieldID == YIELD_CULTURE)
			{
				m_paiPlayerYieldModifier[YieldID] += iPlayerCultureModifier;
			}
		}
		results->Reset();
	}
#endif
	kUtility.PopulateArrayByExistence(m_pbFaithPurchaseUnitEraEnabled, "Eras", "Belief_EraFaithUnitPurchase", "EraType", "BeliefType", szBeliefType);
	kUtility.PopulateArrayByExistence(m_pbBuildingClassEnabled, "BuildingClasses", "Belief_BuildingClassFaithPurchase", "BuildingClassType", "BeliefType", szBeliefType);
#if !defined(LEKMOD_EXPERIMENTAL_CHANGES)
	kUtility.SetYields(m_piYieldChangeWorldWonder, "Belief_YieldChangeWorldWonder", "BeliefType", szBeliefType);
#else
	kUtility.SetYields(m_piYieldChangeWorldWonder, "WorldWonderYieldChanges", "BeliefType", szBeliefType);
#endif

#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	{
		kUtility.Initialize2DArray(m_ppiImprovementYieldChanges.first, "Improvements", "Yields");
		m_ppiImprovementYieldChanges.second = kUtility.MaxRows("Improvements");

		std::string strKey("Belief_ImprovementYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Improvements.ID as ImprovementID, Yields.ID as YieldID, Yield from Belief_ImprovementYieldChanges inner join Improvements on Improvements.Type = ImprovementType inner join Yields on Yields.Type = YieldType where BeliefType = ?");

		pResults->Bind(1, szBeliefType);

		while(pResults->Step())
		{
			const int ImprovementID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiImprovementYieldChanges.first[ImprovementID][YieldID] = yield;
		}
	}
#else
	kUtility.SetYieldMatrix(m_ppiImprovementYieldChanges, "Improvements", "Belief_ImprovementYieldChanges",
		"select Improvements.ID as ImprovementID, Yields.ID as YieldID, Yield from Belief_ImprovementYieldChanges inner join Improvements on Improvements.Type = ImprovementType inner join Yields on Yields.Type = YieldType where BeliefType = ?",
		szBeliefType);
#endif


#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	{
		kUtility.Initialize2DArray(m_ppiBuildingClassYieldChanges.first, "BuildingClasses", "Yields");
		m_ppiBuildingClassYieldChanges.second = kUtility.MaxRows("BuildingClasses");

		std::string strKey("Belief_BuildingClassYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select BuildingClasses.ID as BuildingClassID, Yields.ID as YieldID, YieldChange from Belief_BuildingClassYieldChanges inner join BuildingClasses on BuildingClasses.Type = BuildingClassType inner join Yields on Yields.Type = YieldType where BeliefType = ?");

		pResults->Bind(1, szBeliefType);

		while(pResults->Step())
		{
			const int BuildingClassID = pResults->GetInt(0);
			const int iYieldID = pResults->GetInt(1);
			const int iYieldChange = pResults->GetInt(2);

			m_ppiBuildingClassYieldChanges.first[BuildingClassID][iYieldID] = iYieldChange;
		}
	}
#else
	kUtility.SetYieldMatrix(m_ppiBuildingClassYieldChanges, "BuildingClasses", "Belief_BuildingClassYieldChanges",
		"select BuildingClasses.ID as BuildingClassID, Yields.ID as YieldID, YieldChange from Belief_BuildingClassYieldChanges inner join BuildingClasses on BuildingClasses.Type = BuildingClassType inner join Yields on Yields.Type = YieldType where BeliefType = ?",
		szBeliefType);
#endif

#ifdef AUI_BELIEF_BUILDING_CLASS_FLAVOR_MODIFIERS

	{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
		kUtility.Initialize2DArray(m_ppiBuildingClassFlavorChanges.first, "BuildingClasses", "Flavors");
		m_ppiBuildingClassFlavorChanges.second = kUtility.MaxRows("BuildingClasses");
#else
		kUtility.Initialize2DArray(m_ppiBuildingClassFlavorChanges, "BuildingClasses", "Flavors");
#endif

		std::string strKey("Belief_BuildingClassFlavorChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select BuildingClasses.ID as BuildingClassID, Flavors.ID as FlavorID, FlavorChange from Belief_BuildingClassFlavorChanges inner join BuildingClasses on BuildingClasses.Type = BuildingClassType inner join Flavors on Flavors.Type = FlavorType where BeliefType = ?");

		pResults->Bind(1, szBeliefType);

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
		kUtility.Initialize2DArray(m_ppaiFeatureYieldChange.first, "Features", "Yields");
		m_ppaiFeatureYieldChange.second = kUtility.MaxRows("Features");

		std::string strKey("Belief_FeatureYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Features.ID as FeatureID, Yields.ID as YieldID, Yield from Belief_FeatureYieldChanges inner join Features on Features.Type = FeatureType inner join Yields on Yields.Type = YieldType where BeliefType = ?");

		pResults->Bind(1, szBeliefType);

		while(pResults->Step())
		{
			const int FeatureID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppaiFeatureYieldChange.first[FeatureID][YieldID] = yield;
		}
	}
#else
	kUtility.SetYieldMatrix(m_ppaiFeatureYieldChange, "Features", "Belief_FeatureYieldChanges",
		"select Features.ID as FeatureID, Yields.ID as YieldID, Yield from Belief_FeatureYieldChanges inner join Features on Features.Type = FeatureType inner join Yields on Yields.Type = YieldType where BeliefType = ?",
		szBeliefType);
#endif


#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	{
		kUtility.Initialize2DArray(m_ppaiResourceYieldChange.first, "Resources", "Yields");
		m_ppaiResourceYieldChange.second = kUtility.MaxRows("Resources");

		std::string strKey("Belief_ResourceYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Resources.ID as ResourceID, Yields.ID as YieldID, Yield from Belief_ResourceYieldChanges inner join Resources on Resources.Type = ResourceType inner join Yields on Yields.Type = YieldType where BeliefType = ?");

		pResults->Bind(1, szBeliefType);

		while(pResults->Step())
		{
			const int ResourceID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppaiResourceYieldChange.first[ResourceID][YieldID] = yield;
		}
	}
#else
	kUtility.SetYieldMatrix(m_ppaiResourceYieldChange, "Resources", "Belief_ResourceYieldChanges",
		"select Resources.ID as ResourceID, Yields.ID as YieldID, Yield from Belief_ResourceYieldChanges inner join Resources on Resources.Type = ResourceType inner join Yields on Yields.Type = YieldType where BeliefType = ?",
		szBeliefType);
#endif


#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	{
		kUtility.Initialize2DArray(m_ppaiTerrainYieldChange.first, "Terrains", "Yields");
		m_ppaiTerrainYieldChange.second = kUtility.MaxRows("Terrains");

		std::string strKey("Belief_TerrainYieldChanges");
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Terrains.ID as TerrainID, Yields.ID as YieldID, Yield from Belief_TerrainYieldChanges inner join Terrains on Terrains.Type = TerrainType inner join Yields on Yields.Type = YieldType where BeliefType = ?");

		pResults->Bind(1, szBeliefType);

		while(pResults->Step())
		{
			const int TerrainID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppaiTerrainYieldChange.first[TerrainID][YieldID] = yield;
		}
	}
#else
	kUtility.SetYieldMatrix(m_ppaiTerrainYieldChange, "Terrains", "Belief_TerrainYieldChanges",
		"select Terrains.ID as TerrainID, Yields.ID as YieldID, Yield from Belief_TerrainYieldChanges inner join Terrains on Terrains.Type = TerrainType inner join Yields on Yields.Type = YieldType where BeliefType = ?",
		szBeliefType);
#endif

	return true;
}





CvBeliefXMLEntries::CvBeliefXMLEntries(void)
{

}


CvBeliefXMLEntries::~CvBeliefXMLEntries(void)
{
	DeleteArray();
}


std::vector<CvBeliefEntry*>& CvBeliefXMLEntries::GetBeliefEntries()
{
	return m_paBeliefEntries;
}


int CvBeliefXMLEntries::GetNumBeliefs()
{
	return m_paBeliefEntries.size();
}


void CvBeliefXMLEntries::DeleteArray()
{
	for(std::vector<CvBeliefEntry*>::iterator it = m_paBeliefEntries.begin(); it != m_paBeliefEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paBeliefEntries.clear();
}


CvBeliefEntry* CvBeliefXMLEntries::GetEntry(int index)
{
	return m_paBeliefEntries[index];
}





CvReligionBeliefs::CvReligionBeliefs():
	m_paiBuildingClassEnabled(NULL)
{
	Reset();
}


CvReligionBeliefs::~CvReligionBeliefs(void)
{
	Uninit();
}


CvReligionBeliefs::CvReligionBeliefs(const CvReligionBeliefs& source)
{
	m_iFaithFromDyingUnits = source.m_iFaithFromDyingUnits;
	m_iRiverHappiness = source.m_iRiverHappiness;
	m_iPlotCultureCostModifier = source.m_iPlotCultureCostModifier;
	m_iCityRangeStrikeModifier = source.m_iCityRangeStrikeModifier;
	m_iCombatModifierEnemyCities = source.m_iCombatModifierEnemyCities;
	m_iCombatModifierFriendlyCities = source.m_iCombatModifierFriendlyCities;
	m_iFriendlyHealChange = source.m_iFriendlyHealChange;
	m_iCityStateFriendshipModifier = source.m_iCityStateFriendshipModifier;
	m_iLandBarbarianConversionPercent = source.m_iLandBarbarianConversionPercent;
	m_iSpreadDistanceModifier = source.m_iSpreadDistanceModifier;
	m_iSpreadStrengthModifier = source.m_iSpreadStrengthModifier;
	m_iProphetStrengthModifier = source.m_iProphetStrengthModifier;
	m_iProphetCostModifier = source.m_iProphetCostModifier;
	m_iMissionaryStrengthModifier = source.m_iMissionaryStrengthModifier;
#ifdef NQ_FLAT_FAITH_PER_CITIZEN_BORN_FROM_BELIEFS
	m_iFlatFaithPerCitizenBorn = source.m_iFlatFaithPerCitizenBorn;
#endif
#ifdef NQ_BELIEF_EXTRA_MISSIONARY_SPREADS
	m_iMissionaryExtraSpreads = source.m_iMissionaryExtraSpreads;
#endif
#ifdef NQ_SPREAD_MODIFIER_OWNED_CITIES
	m_iSpreadModifierOwnedCities = source.m_iSpreadModifierOwnedCities;
	m_iSpreadModifierUnownedCities = source.m_iSpreadModifierUnownedCities;
#endif
#ifdef NQ_EXTRA_TRADE_ROUTES_FROM_BELIEF
	m_iExtraTradeRoutes	= source.m_iExtraTradeRoutes;
#endif
#ifdef NQ_FAITH_PER_CITY_STATE_THIS_RELIGION
	m_iFaithPerCityStateThisReligion = source.m_iFaithPerCityStateThisReligion;
#endif
#ifdef NQ_FAITH_PER_FOREIGN_TRADE_ROUTE
	m_iFaithPerForeignTradeRoute = source.m_iFaithPerForeignTradeRoute;
#endif
#ifdef NQ_GOLDEN_AGE_TURNS_FROM_BELIEF
	m_iGoldenAgeTurns = source.m_iGoldenAgeTurns;
#endif
	m_iMissionaryCostModifier = source.m_iMissionaryCostModifier;
	m_iFriendlyCityStateSpreadModifier = source.m_iFriendlyCityStateSpreadModifier;
	m_iGreatPersonExpendedFaith = source.m_iGreatPersonExpendedFaith;
	m_iCityStateMinimumInfluence = source.m_iCityStateMinimumInfluence;
	m_iCityStateInfluenceModifier = source.m_iCityStateInfluenceModifier;
#ifdef LEKMOD_BELIEF_CITY_STATE_FOLLOWING_RELIGION_INFLUENCE
	m_iCityStateFollowingReligionDecayMod = source.m_iCityStateFollowingReligionDecayMod;
	m_iCityStateFollowingReligionRecoveryMod = source.m_iCityStateFollowingReligionRecoveryMod;
#endif
	m_iOtherReligionPressureErosion = source.m_iOtherReligionPressureErosion;
	m_iSpyPressure = source.m_iSpyPressure;
	m_iInquisitorPressureRetention = source.m_iInquisitorPressureRetention;
	m_iFaithBuildingTourism = source.m_iFaithBuildingTourism;

	m_eObsoleteEra = source.m_eObsoleteEra;
	m_eResourceRevealed = source.m_eResourceRevealed;
	m_eSpreadModifierDoublingTech = source.m_eSpreadModifierDoublingTech;

	m_ReligionBeliefs = source.m_ReligionBeliefs;

	m_paiBuildingClassEnabled = FNEW(int[GC.getNumBuildingClassInfos()], c_eCiv5GameplayDLL, 0);
#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
#else
	for(int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo((BuildingClassTypes)iI);
		if(!pkBuildingClassInfo)
		{
			continue;
		}
#endif

		m_paiBuildingClassEnabled[iI] = source.m_paiBuildingClassEnabled[iI];
	}
}


void CvReligionBeliefs::Uninit()
{
	SAFE_DELETE_ARRAY(m_paiBuildingClassEnabled);
}


void CvReligionBeliefs::Reset()
{
	m_iFaithFromDyingUnits = 0;
	m_iRiverHappiness = 0;
	m_iPlotCultureCostModifier = 0;
	m_iCityRangeStrikeModifier = 0;
	m_iCombatModifierEnemyCities = 0;
	m_iCombatModifierFriendlyCities = 0;
	m_iFriendlyHealChange = 0;
	m_iCityStateFriendshipModifier = 0;
	m_iLandBarbarianConversionPercent = 0;
	m_iSpreadDistanceModifier = 0;
	m_iSpreadStrengthModifier = 0;
	m_iProphetStrengthModifier = 0;
	m_iProphetCostModifier = 0;
	m_iMissionaryStrengthModifier = 0;
#ifdef NQ_FLAT_FAITH_PER_CITIZEN_BORN_FROM_BELIEFS
	m_iFlatFaithPerCitizenBorn = 0;
#endif
#ifdef NQ_BELIEF_EXTRA_MISSIONARY_SPREADS
	m_iMissionaryExtraSpreads = 0;
#endif
#ifdef NQ_SPREAD_MODIFIER_OWNED_CITIES
	m_iSpreadModifierOwnedCities = 0;
	m_iSpreadModifierUnownedCities = 0;
#endif
#ifdef NQ_EXTRA_TRADE_ROUTES_FROM_BELIEF
	m_iExtraTradeRoutes	= 0;
#endif
#ifdef NQ_FAITH_PER_CITY_STATE_THIS_RELIGION
	m_iFaithPerCityStateThisReligion = 0;
#endif
#ifdef NQ_FAITH_PER_FOREIGN_TRADE_ROUTE
	m_iFaithPerForeignTradeRoute = 0;
#endif
#ifdef NQ_GOLDEN_AGE_TURNS_FROM_BELIEF
	m_iGoldenAgeTurns = 0;
#endif
	m_iMissionaryCostModifier = 0;
	m_iFriendlyCityStateSpreadModifier = 0;
	m_iGreatPersonExpendedFaith = 0;
	m_iCityStateMinimumInfluence = 0;
	m_iCityStateInfluenceModifier = 0;
#ifdef LEKMOD_BELIEF_CITY_STATE_FOLLOWING_RELIGION_INFLUENCE
	m_iCityStateFollowingReligionDecayMod = 0;
	m_iCityStateFollowingReligionRecoveryMod = 0;
#endif
	m_iOtherReligionPressureErosion = 0;
	m_iSpyPressure = 0;
	m_iInquisitorPressureRetention = 0;
	m_iFaithBuildingTourism = 0;

	m_eObsoleteEra = NO_ERA;
	m_eResourceRevealed = NO_RESOURCE;
	m_eSpreadModifierDoublingTech = NO_TECH;

	m_ReligionBeliefs.clear();

#ifdef AUI_WARNING_FIXES
	SAFE_DELETE_ARRAY(m_paiBuildingClassEnabled);
#endif
	m_paiBuildingClassEnabled = FNEW(int[GC.getNumBuildingClassInfos()], c_eCiv5GameplayDLL, 0);
#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
#else
	for(int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo((BuildingClassTypes)iI);
		if(!pkBuildingClassInfo)
		{
			continue;
		}
#endif

		m_paiBuildingClassEnabled[iI] = 0;
	}
}


void CvReligionBeliefs::AddBelief(BeliefTypes eBelief)
{
	CvAssert(eBelief != NO_BELIEF);
	if(eBelief == NO_BELIEF)
		return;

	CvBeliefEntry* belief = GC.GetGameBeliefs()->GetEntry(eBelief);
	CvAssert(belief != NULL);
	if(belief == NULL)
		return;

	m_iFaithFromDyingUnits += belief->GetFaithFromDyingUnits();
	m_iRiverHappiness += belief->GetRiverHappiness();
	m_iPlotCultureCostModifier += belief->GetPlotCultureCostModifier();
	m_iCityRangeStrikeModifier += belief->GetCityRangeStrikeModifier();
	m_iCombatModifierEnemyCities += belief->GetCombatModifierEnemyCities();
	m_iCombatModifierFriendlyCities += belief->GetCombatModifierFriendlyCities();
	m_iFriendlyHealChange += belief->GetFriendlyHealChange();
	m_iCityStateFriendshipModifier += belief->GetCityStateFriendshipModifier();
	m_iLandBarbarianConversionPercent += belief->GetLandBarbarianConversionPercent();
	m_iSpreadDistanceModifier += belief->GetSpreadDistanceModifier();
	m_iSpreadStrengthModifier += belief->GetSpreadStrengthModifier();
	m_iProphetStrengthModifier += belief->GetProphetStrengthModifier();
	m_iProphetCostModifier += belief->GetProphetCostModifier();
	m_iMissionaryStrengthModifier += belief->GetMissionaryStrengthModifier();
#ifdef NQ_FLAT_FAITH_PER_CITIZEN_BORN_FROM_BELIEFS
	m_iFlatFaithPerCitizenBorn += belief->GetFlatFaithPerCitizenBorn();
#endif
#ifdef NQ_BELIEF_EXTRA_MISSIONARY_SPREADS
	m_iMissionaryExtraSpreads += belief->GetMissionaryExtraSpreads();
#endif
#ifdef NQ_SPREAD_MODIFIER_OWNED_CITIES
	m_iSpreadModifierOwnedCities += belief->GetSpreadModifierOwnedCities();
	m_iSpreadModifierUnownedCities += belief->GetSpreadModifierUnownedCities();
#endif
#ifdef NQ_EXTRA_TRADE_ROUTES_FROM_BELIEF
	m_iExtraTradeRoutes	+= belief->GetExtraTradeRoutes();
#endif
#ifdef NQ_FAITH_PER_CITY_STATE_THIS_RELIGION
	m_iFaithPerCityStateThisReligion += belief->GetFaithPerCityStateThisReligion();
#endif
#ifdef NQ_FAITH_PER_FOREIGN_TRADE_ROUTE
	m_iFaithPerForeignTradeRoute += belief->GetFaithPerForeignTradeRoute();
#endif
#ifdef NQ_GOLDEN_AGE_TURNS_FROM_BELIEF
	m_iGoldenAgeTurns += belief->GetGoldenAgeTurns();
#endif
	m_iMissionaryCostModifier += belief->GetMissionaryCostModifier();
	m_iFriendlyCityStateSpreadModifier += belief->GetFriendlyCityStateSpreadModifier();
	m_iGreatPersonExpendedFaith += belief->GetGreatPersonExpendedFaith();
	m_iCityStateMinimumInfluence += belief->GetCityStateMinimumInfluence();
	m_iCityStateInfluenceModifier += belief->GetCityStateInfluenceModifier();
#ifdef LEKMOD_BELIEF_CITY_STATE_FOLLOWING_RELIGION_INFLUENCE
	m_iCityStateFollowingReligionDecayMod += belief->GetCityStateFollowingReligionDecayMod();
	m_iCityStateFollowingReligionRecoveryMod += belief->GetCityStateFollowingReligionRecoveryMod();
#endif
	m_iOtherReligionPressureErosion += belief->GetOtherReligionPressureErosion();
	m_iSpyPressure += belief->GetSpyPressure();
	m_iInquisitorPressureRetention += belief->GetInquisitorPressureRetention();
	m_iFaithBuildingTourism += belief->GetFaithBuildingTourism();

	m_eObsoleteEra = belief->GetObsoleteEra();
	m_eResourceRevealed = belief->GetResourceRevealed();

#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#else
	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
#endif
	{
		if (belief->IsBuildingClassEnabled(iI))
		{
			m_paiBuildingClassEnabled[iI]++;
		}
	}

	if(belief->GetSpreadModifierDoublingTech() != NO_TECH)
	{
		m_eSpreadModifierDoublingTech = belief->GetSpreadModifierDoublingTech();
	}

	m_ReligionBeliefs.push_back((int)eBelief);
}


bool CvReligionBeliefs::HasBelief(BeliefTypes eBelief) const
{
	return (find(m_ReligionBeliefs.begin(), m_ReligionBeliefs.end(), (int)eBelief) != m_ReligionBeliefs.end());
}


BeliefTypes CvReligionBeliefs::GetBelief(int iIndex) const
{
	return (BeliefTypes)m_ReligionBeliefs[iIndex];
}


int CvReligionBeliefs::GetNumBeliefs() const
{
	return m_ReligionBeliefs.size();
}


int CvReligionBeliefs::GetFaithFromKills(int iDistance) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;
	int iRequiredDistance;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			iRequiredDistance = pBeliefs->GetEntry(i)->GetMaxDistance();
			if(iRequiredDistance == 0 || iDistance <= iRequiredDistance)
			{
				rtnValue += pBeliefs->GetEntry(i)->GetFaithFromKills();
			}
		}
	}

	return rtnValue;
}


int CvReligionBeliefs::GetHappinessPerCity(int iPopulation) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			if(iPopulation >= pBeliefs->GetEntry(i)->GetMinPopulation())
			{
				rtnValue += pBeliefs->GetEntry(i)->GetHappinessPerCity();
			}
		}
	}

	return rtnValue;
}


int CvReligionBeliefs::GetHappinessPerXPeacefulForeignFollowers() const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i) && pBeliefs->GetEntry(i)->GetHappinessPerXPeacefulForeignFollowers() > 0)
		{
			return pBeliefs->GetEntry(i)->GetHappinessPerXPeacefulForeignFollowers();
		}
	}

	return 0;
}


int CvReligionBeliefs:: GetWonderProductionModifier(EraTypes eWonderEra) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			if((int)eWonderEra < (int)pBeliefs->GetEntry(i)->GetObsoleteEra())
			{
				rtnValue += pBeliefs->GetEntry(i)->GetWonderProductionModifier();
			}
		}
	}

	return rtnValue;
}


int CvReligionBeliefs:: GetPlayerHappiness(bool bAtPeace) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			if(bAtPeace || !pBeliefs->GetEntry(i)->RequiresPeace())
			{
				rtnValue += pBeliefs->GetEntry(i)->GetPlayerHappiness();
			}
		}
	}

	return rtnValue;
}


int CvReligionBeliefs:: GetPlayerCultureModifier(bool bAtPeace) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			if(bAtPeace || !pBeliefs->GetEntry(i)->RequiresPeace())
			{
				rtnValue += pBeliefs->GetEntry(i)->GetPlayerCultureModifier();
			}
		}
	}

	return rtnValue;
}


float CvReligionBeliefs:: GetHappinessPerFollowingCity() const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	float rtnValue = 0.0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetHappinessPerFollowingCity();
		}
	}

	return rtnValue;
}


int CvReligionBeliefs:: GetGoldPerFollowingCity() const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetGoldPerFollowingCity();
		}
	}

	return rtnValue;
}


int CvReligionBeliefs:: GetGoldPerXFollowers() const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetGoldPerXFollowers();
		}
	}

	return rtnValue;
}


int CvReligionBeliefs:: GetGoldWhenCityAdopts() const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetGoldWhenCityAdopts();
		}
	}

	return rtnValue;
}


int CvReligionBeliefs:: GetSciencePerOtherReligionFollower() const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetSciencePerOtherReligionFollower();
		}
	}

	return rtnValue;
}


int CvReligionBeliefs::GetCityGrowthModifier(bool bAtPeace) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			if(bAtPeace || !pBeliefs->GetEntry(i)->RequiresPeace())
			{
				rtnValue += pBeliefs->GetEntry(i)->GetCityGrowthModifier();
			}
		}
	}

	return rtnValue;
}


int CvReligionBeliefs::GetCityYieldChange(int iPopulation, YieldTypes eYield) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			if(iPopulation >= pBeliefs->GetEntry(i)->GetMinPopulation())
			{
				rtnValue += pBeliefs->GetEntry(i)->GetCityYieldChange(eYield);
			}
		}
	}

	return rtnValue;
}


int CvReligionBeliefs::GetHolyCityYieldChange (YieldTypes eYield) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
				rtnValue += pBeliefs->GetEntry(i)->GetHolyCityYieldChange(eYield);
		}
	}

	return rtnValue;
}


int CvReligionBeliefs::GetYieldChangePerForeignCity(YieldTypes eYield) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetYieldChangePerForeignCity(eYield);
		}
	}

	return rtnValue;
}
#if defined(LEKMOD_BELIEF_YIELDIFY)
int CvReligionBeliefs::GetYieldChangePerFollowingCity(YieldTypes eYield) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;
	for (int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if (HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetYieldChangePerFollowingCity(eYield);
		}
	}
	return rtnValue;
}
int CvReligionBeliefs::GetPlayerYieldModifier(YieldTypes eYield, bool bAtPeace) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;
	for (int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if (HasBelief((BeliefTypes)i))
		{
			if (bAtPeace || !pBeliefs->GetEntry(i)->RequiresPeace())
			{
				rtnValue += pBeliefs->GetEntry(i)->GetPlayerYieldModifier(eYield);
			}
		}
	}
	return rtnValue;
}
#endif
#if defined(LEK_CULTURE_SCIENCE_SPREAD_BELIEFS_ALL_CITIES)
int CvReligionBeliefs::GetYieldChangePerXFollowers(YieldTypes eYield) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;
	for (int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if (HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetYieldChangePerXFollowers(eYield);
		}
	}
	return rtnValue;
}
#endif

int CvReligionBeliefs::GetYieldChangePerXForeignFollowers(YieldTypes eYield) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetYieldChangePerXForeignFollowers(eYield);
		}
	}

	return rtnValue;
}


int CvReligionBeliefs::GetResourceQuantityModifier(ResourceTypes eResource) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetResourceQuantityModifier(eResource);
		}
	}

	return rtnValue;
}


int CvReligionBeliefs::GetImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYield) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetImprovementYieldChange(eImprovement, eYield);
		}
	}

	return rtnValue;
}


int CvReligionBeliefs::GetBuildingClassYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYieldType, int iFollowers) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			if(iFollowers >= pBeliefs->GetEntry(i)->GetMinFollowers())
			{
				rtnValue += pBeliefs->GetEntry(i)->GetBuildingClassYieldChange(eBuildingClass, eYieldType);
			}
		}
	}

	return rtnValue;
}

#ifdef AUI_BELIEF_BUILDING_CLASS_FLAVOR_MODIFIERS
int CvReligionBeliefs::GetBuildingClassFlavorChange(BuildingClassTypes eBuildingClass, FlavorTypes eFlavorType) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for (int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if (HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetBuildingClassFlavorChange(eBuildingClass, eFlavorType);
		}
	}

	return rtnValue;
}
#endif


int CvReligionBeliefs::GetBuildingClassHappiness(BuildingClassTypes eBuildingClass, int iFollowers) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			if(iFollowers >= pBeliefs->GetEntry(i)->GetMinFollowers())
			{
				rtnValue += pBeliefs->GetEntry(i)->GetBuildingClassHappiness(eBuildingClass);
			}
		}
	}

	return rtnValue;
}


int CvReligionBeliefs::GetBuildingClassTourism(BuildingClassTypes eBuildingClass) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetBuildingClassTourism(eBuildingClass);
		}
	}

	return rtnValue;
}


int CvReligionBeliefs::GetFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYieldType) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetFeatureYieldChange(eFeature, eYieldType);
		}
	}

	return rtnValue;
}


int CvReligionBeliefs::GetResourceYieldChange(ResourceTypes eResource, YieldTypes eYieldType) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetResourceYieldChange(eResource, eYieldType);
		}
	}

	return rtnValue;
}


int CvReligionBeliefs::GetTerrainYieldChange(TerrainTypes eTerrain, YieldTypes eYieldType) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetTerrainYieldChange(eTerrain, eYieldType);
		}
	}

	return rtnValue;
}


int CvReligionBeliefs::GetResourceHappiness(ResourceTypes eResource) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetResourceHappiness(eResource);
		}
	}

	return rtnValue;
}


int CvReligionBeliefs::GetYieldChangeAnySpecialist(YieldTypes eYieldType) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetYieldChangeAnySpecialist(eYieldType);
		}
	}

	return rtnValue;
}


int CvReligionBeliefs::GetYieldChangeTradeRoute(YieldTypes eYieldType) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetYieldChangeTradeRoute(eYieldType);
		}
	}

	return rtnValue;
}
#if defined(TRADE_REFACTOR)

int CvReligionBeliefs::GetTradeConnectionOriginLandYieldChange(TradeConnectionType eConnection, YieldTypes eYield) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for (int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if (HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetTradeConnectionOriginLandYieldChange(eConnection, eYield);
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetTradeConnectionOriginSeaYieldChange(TradeConnectionType eConnection, YieldTypes eYield) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for (int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if (HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetTradeConnectionOriginSeaYieldChange(eConnection, eYield);
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetIncomingTradeConnectionLandYieldChange(TradeConnectionType eConnection, YieldTypes eYield) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for (int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if (HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetIncomingTradeConnectionLandYieldChange(eConnection, eYield);
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetIncomingTradeConnectionSeaYieldChange(TradeConnectionType eConnection, YieldTypes eYield) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for (int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if (HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetIncomingTradeConnectionSeaYieldChange(eConnection, eYield);
		}
	}

	return rtnValue;
}
#endif

int CvReligionBeliefs::GetYieldChangeNaturalWonder(YieldTypes eYieldType) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetYieldChangeNaturalWonder(eYieldType);
		}
	}

	return rtnValue;
}


int CvReligionBeliefs::GetYieldChangeWorldWonder(YieldTypes eYieldType) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetYieldChangeWorldWonder(eYieldType);
		}
	}

	return rtnValue;
}


int CvReligionBeliefs::GetYieldModifierNaturalWonder(YieldTypes eYieldType) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetYieldModifierNaturalWonder(eYieldType);
		}
	}

	return rtnValue;
}


int CvReligionBeliefs::GetMaxYieldModifierPerFollower(YieldTypes eYieldType) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetMaxYieldModifierPerFollower(eYieldType);
		}
	}

	return rtnValue;
}

#ifdef NQ_FREE_SETTLERS_FROM_BELIEF

int CvReligionBeliefs::GetNumFreeSettlers() const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			rtnValue += pBeliefs->GetEntry(i)->GetNumFreeSettlers();
		}
	}

	return rtnValue;
}
#endif


bool CvReligionBeliefs::IsBuildingClassEnabled(BuildingClassTypes eType) const
{
	return m_paiBuildingClassEnabled[(int)eType];
}


bool CvReligionBeliefs::IsFaithBuyingEnabled(EraTypes eEra) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			if (pBeliefs->GetEntry(i)->IsFaithUnitPurchaseEra((int)eEra))
			{
				return true;
			}
		}
	}

	return false;
}


bool CvReligionBeliefs::IsConvertsBarbarians() const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			if (pBeliefs->GetEntry(i)->ConvertsBarbarians())
			{
				return true;
			}
		}
	}

	return false;
}

#ifdef NQ_DEUS_VULT

bool CvReligionBeliefs::IsDeusVult() const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			if (pBeliefs->GetEntry(i)->DeusVult())
			{
				return true;
			}
		}
	}

	return false;
}
#endif


bool CvReligionBeliefs::IsFaithPurchaseAllGreatPeople() const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			if (pBeliefs->GetEntry(i)->FaithPurchaseAllGreatPeople())
			{
				return true;
			}
		}
	}

	return false;
}

#ifdef NQ_BELIEF_TOGGLE_ALLOW_FAITH_GIFTS_TO_MINORS

bool CvReligionBeliefs::IsAllowsFaithGiftsToMinors() const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();

	for(int i = 0; i < pBeliefs->GetNumBeliefs(); i++)
	{
		if(HasBelief((BeliefTypes)i))
		{
			if (pBeliefs->GetEntry(i)->AllowsFaithGiftsToMinors())
			{
				return true;
			}
		}
	}

	return false;
}
#endif


void CvReligionBeliefs::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_iFaithFromDyingUnits;
	kStream >> m_iRiverHappiness;
	kStream >> m_iPlotCultureCostModifier;
	kStream >> m_iCityRangeStrikeModifier;
	kStream >> m_iCombatModifierEnemyCities;
	kStream >> m_iCombatModifierFriendlyCities;
	kStream >> m_iFriendlyHealChange;
	kStream >> m_iCityStateFriendshipModifier;
	kStream >> m_iLandBarbarianConversionPercent;
	kStream >> m_iSpreadStrengthModifier;
	kStream >> m_iSpreadDistanceModifier;
	kStream >> m_iProphetStrengthModifier;
	kStream >> m_iProphetCostModifier;
	kStream >> m_iMissionaryStrengthModifier;
#ifdef NQ_FLAT_FAITH_PER_CITIZEN_BORN_FROM_BELIEFS
	kStream >> m_iFlatFaithPerCitizenBorn;
#endif
#ifdef NQ_BELIEF_EXTRA_MISSIONARY_SPREADS
	kStream >> m_iMissionaryExtraSpreads;
#endif
#ifdef NQ_SPREAD_MODIFIER_OWNED_CITIES
	kStream >> m_iSpreadModifierOwnedCities;
	kStream >> m_iSpreadModifierUnownedCities;
#endif
#ifdef NQ_EXTRA_TRADE_ROUTES_FROM_BELIEF
	kStream >> m_iExtraTradeRoutes;
#endif
#ifdef NQ_FAITH_PER_CITY_STATE_THIS_RELIGION
	kStream >> m_iFaithPerCityStateThisReligion;
#endif
#ifdef NQ_FAITH_PER_FOREIGN_TRADE_ROUTE
	kStream >> m_iFaithPerForeignTradeRoute;
#endif
#ifdef NQ_GOLDEN_AGE_TURNS_FROM_BELIEF
	kStream >> m_iGoldenAgeTurns;
#endif
	kStream >> m_iMissionaryCostModifier;
	kStream >> m_iFriendlyCityStateSpreadModifier;
	kStream >> m_iGreatPersonExpendedFaith;
	kStream >> m_iCityStateMinimumInfluence;
	kStream >> m_iCityStateInfluenceModifier;
	kStream >> m_iOtherReligionPressureErosion;
	kStream >> m_iSpyPressure;
	kStream >> m_iInquisitorPressureRetention;
	if (uiVersion >= 2)
	{
		kStream >> m_iFaithBuildingTourism;
#ifdef LEKMOD_BELIEF_CITY_STATE_FOLLOWING_RELIGION_INFLUENCE
		kStream >> m_iCityStateFollowingReligionDecayMod;
		kStream >> m_iCityStateFollowingReligionRecoveryMod;
#else
		{
			int iDiscardDecay;
			int iDiscardRecovery;
			kStream >> iDiscardDecay;
			kStream >> iDiscardRecovery;
		}
#endif
	}
	else
	{
		m_iFaithBuildingTourism = 0;
#ifdef LEKMOD_BELIEF_CITY_STATE_FOLLOWING_RELIGION_INFLUENCE
		m_iCityStateFollowingReligionDecayMod = 0;
		m_iCityStateFollowingReligionRecoveryMod = 0;
#endif
	}

	kStream >> m_eObsoleteEra;
	kStream >> m_eResourceRevealed;
	kStream >> m_eSpreadModifierDoublingTech;

	m_ReligionBeliefs.clear();
	uint uiBeliefCount;
	kStream >> uiBeliefCount;
	while(uiBeliefCount--)
	{
		int iBeliefIndex = CvInfosSerializationHelper::ReadHashed(kStream);
		m_ReligionBeliefs.push_back(iBeliefIndex);
	}

	BuildingClassArrayHelpers::Read(kStream, m_paiBuildingClassEnabled);
}


void CvReligionBeliefs::Write(FDataStream& kStream) const
{

	uint uiVersion = 2;
	kStream << uiVersion;

	kStream << m_iFaithFromDyingUnits;
	kStream << m_iRiverHappiness;
	kStream << m_iPlotCultureCostModifier;
	kStream << m_iCityRangeStrikeModifier;
	kStream << m_iCombatModifierEnemyCities;
	kStream << m_iCombatModifierFriendlyCities;
	kStream << m_iFriendlyHealChange;
	kStream << m_iCityStateFriendshipModifier;
	kStream << m_iLandBarbarianConversionPercent;
	kStream << m_iSpreadStrengthModifier;
	kStream << m_iSpreadDistanceModifier;
	kStream << m_iProphetStrengthModifier;
	kStream << m_iProphetCostModifier;
	kStream << m_iMissionaryStrengthModifier;
#ifdef NQ_FLAT_FAITH_PER_CITIZEN_BORN_FROM_BELIEFS
	kStream << m_iFlatFaithPerCitizenBorn;
#endif
#ifdef NQ_BELIEF_EXTRA_MISSIONARY_SPREADS
	kStream << m_iMissionaryExtraSpreads;
#endif
#ifdef NQ_SPREAD_MODIFIER_OWNED_CITIES
	kStream << m_iSpreadModifierOwnedCities;
	kStream << m_iSpreadModifierUnownedCities;
#endif
#ifdef NQ_EXTRA_TRADE_ROUTES_FROM_BELIEF
	kStream << m_iExtraTradeRoutes;
#endif
#ifdef NQ_FAITH_PER_CITY_STATE_THIS_RELIGION
	kStream << m_iFaithPerCityStateThisReligion;
#endif
#ifdef NQ_FAITH_PER_FOREIGN_TRADE_ROUTE
	kStream << m_iFaithPerForeignTradeRoute;
#endif
#ifdef NQ_GOLDEN_AGE_TURNS_FROM_BELIEF
	kStream << m_iGoldenAgeTurns;
#endif
	kStream << m_iMissionaryCostModifier;
	kStream << m_iFriendlyCityStateSpreadModifier;
	kStream << m_iGreatPersonExpendedFaith;
	kStream << m_iCityStateMinimumInfluence;
	kStream << m_iCityStateInfluenceModifier;
	kStream << m_iOtherReligionPressureErosion;
	kStream << m_iSpyPressure;
	kStream << m_iInquisitorPressureRetention;
	kStream << m_iFaithBuildingTourism;
#ifdef LEKMOD_BELIEF_CITY_STATE_FOLLOWING_RELIGION_INFLUENCE
	kStream << m_iCityStateFollowingReligionDecayMod;
	kStream << m_iCityStateFollowingReligionRecoveryMod;
#else
	kStream << 0;
	kStream << 0;
#endif

	kStream << m_eObsoleteEra;
	kStream << m_eResourceRevealed;
	kStream << m_eSpreadModifierDoublingTech;


	kStream << m_ReligionBeliefs.size();
	for (uint i = 0; i < m_ReligionBeliefs.size(); ++i)
	{
		CvInfosSerializationHelper::WriteHashed(kStream, (BeliefTypes)m_ReligionBeliefs[i]);
	}

	BuildingClassArrayHelpers::Write(kStream, m_paiBuildingClassEnabled, GC.getNumBuildingClassInfos());
}

#ifdef LEKMOD_BELIEF_BUILDING_PURCHASE
namespace
{
	struct LekmodBeliefBldPurchaseRow
	{
		BeliefTypes eBelief;
		BuildingClassTypes eBuildingClass;
		int iCost;
		YieldTypes eYield;
		int iIncrementalPerCity;
	};

	std::vector<LekmodBeliefBldPurchaseRow> s_buildingPurchaseFaithGoldRows;
}

void CvReligionBeliefs::LoadBuildingPurchaseFaithGoldTable()
{
	s_buildingPurchaseFaithGoldRows.clear();

	Database::Connection* pDb = GC.GetGameDatabase();
	if (!pDb)
	{
		return;
	}

	Database::Results kQuery;
	const char* szSql =
		"SELECT Beliefs.ID, BuildingClasses.ID, Beliefs_BuildingPurchaseFaithGold.Cost, Yields.ID, Beliefs_BuildingPurchaseFaithGold.IncrementalCostPerCity "
		"FROM Beliefs_BuildingPurchaseFaithGold "
		"INNER JOIN Beliefs ON Beliefs.Type = Beliefs_BuildingPurchaseFaithGold.BeliefType "
		"INNER JOIN BuildingClasses ON BuildingClasses.Type = Beliefs_BuildingPurchaseFaithGold.BuildingClassType "
		"INNER JOIN Yields ON Yields.Type = Beliefs_BuildingPurchaseFaithGold.YieldType";

	if (!pDb->Execute(kQuery, szSql))
	{
		return;
	}

	while (kQuery.Step())
	{
		LekmodBeliefBldPurchaseRow row;
		row.eBelief = (BeliefTypes)kQuery.GetInt(0);
		row.eBuildingClass = (BuildingClassTypes)kQuery.GetInt(1);
		row.iCost = kQuery.GetInt(2);
		row.eYield = (YieldTypes)kQuery.GetInt(3);
		row.iIncrementalPerCity = kQuery.GetInt(4);
		if (row.eBelief != NO_BELIEF && row.eBuildingClass != NO_BUILDINGCLASS && row.iCost >= 0 && (row.eYield == YIELD_FAITH || row.eYield == YIELD_GOLD))
		{
			s_buildingPurchaseFaithGoldRows.push_back(row);
		}
	}

	const BeliefTypes eNatBelief = (BeliefTypes)GC.getInfoTypeForString("BELIEF_LEKMOD_NATWONDER_FAITH_PURCHASE", true);
	if (eNatBelief != NO_BELIEF)
	{
		std::vector<bool> hasRowForClass(GC.getNumBuildingClassInfos(), false);
		for (size_t i = 0; i < s_buildingPurchaseFaithGoldRows.size(); i++)
		{
			if (s_buildingPurchaseFaithGoldRows[i].eBelief == eNatBelief && s_buildingPurchaseFaithGoldRows[i].eYield == YIELD_FAITH)
			{
				const int idx = (int)s_buildingPurchaseFaithGoldRows[i].eBuildingClass;
				if (idx >= 0 && idx < (int)hasRowForClass.size())
				{
					hasRowForClass[idx] = true;
				}
			}
		}

		for (int iBC = 0; iBC < GC.getNumBuildingClassInfos(); iBC++)
		{
			const BuildingClassTypes eBC = (BuildingClassTypes)iBC;
			CvBuildingClassInfo* pkC = GC.getBuildingClassInfo(eBC);
			if (!pkC || !isNationalWonderClass(*pkC))
			{
				continue;
			}
			if (hasRowForClass[iBC])
			{
				continue;
			}
			BuildingTypes eSample = NO_BUILDING;
			for (int iB = 0; iB < GC.getNumBuildingInfos(); iB++)
			{
				CvBuildingEntry* pkB = GC.getBuildingInfo((BuildingTypes)iB);
				if (pkB && pkB->GetBuildingClassType() == iBC)
				{
					eSample = (BuildingTypes)iB;
					break;
				}
			}
			if (eSample == NO_BUILDING)
			{
				continue;
			}
			if (GC.getBuildingInfo(eSample)->GetFaithCost() > 0)
			{
				continue;
			}
			LekmodBeliefBldPurchaseRow row;
			row.eBelief = eNatBelief;
			row.eBuildingClass = eBC;
			row.iCost = 200;
			row.eYield = YIELD_FAITH;
			row.iIncrementalPerCity = 50;
			s_buildingPurchaseFaithGoldRows.push_back(row);
		}
	}
}

bool CvReligionBeliefs::TryGetBuildingPurchaseFaithGoldRawCost(BuildingTypes eBuilding, YieldTypes eYield, int iNumCities, int* piTotal) const
{
	if (piTotal == NULL || eBuilding == NO_BUILDING)
	{
		return false;
	}
	CvBuildingEntry* pkB = GC.getBuildingInfo(eBuilding);
	if (!pkB)
	{
		return false;
	}
	const BuildingClassTypes eBC = (BuildingClassTypes)pkB->GetBuildingClassType();
	for (size_t i = 0; i < s_buildingPurchaseFaithGoldRows.size(); i++)
	{
		const LekmodBeliefBldPurchaseRow& row = s_buildingPurchaseFaithGoldRows[i];
		if (row.eYield != eYield || row.eBuildingClass != eBC)
		{
			continue;
		}
		if (!HasBelief(row.eBelief))
		{
			continue;
		}
		*piTotal = row.iCost + row.iIncrementalPerCity * (iNumCities > 0 ? iNumCities : 0);
		return true;
	}
	return false;
}
#endif




bool CvBeliefHelpers::ConvertBarbarianUnit(CvPlayer *pPlayer, UnitHandle pUnit)
{
	UnitHandle pNewUnit;
	CvPlot *pPlot = pUnit->plot();


#ifdef AUI_UNIT_FIX_GIFTED_UNITS_ARE_GIFTED_NOT_CLONED
	pNewUnit = pPlayer->initUnit(pUnit->getUnitType(), pUnit->getX(), pUnit->getY(), pUnit->AI_getUnitAIType(), NO_DIRECTION, true            , false, DEFAULT_UNIT_MAP_LAYER, 0, true);
#else
	pNewUnit = pPlayer->initUnit(pUnit->getUnitType(), pUnit->getX(), pUnit->getY(), pUnit->AI_getUnitAIType(), NO_DIRECTION, true            , false);
#endif
	CvAssertMsg(pNewUnit, "pNewUnit is not assigned a valid value");
	pNewUnit->convert(pUnit.pointer(), false);
	pNewUnit->setupGraphical();
	pNewUnit->finishMoves();

	if(GC.getLogging() && GC.getAILogging())
	{
		CvString logMsg;
		logMsg.Format("Converted barbarian (with belief), X: %d, Y: %d", pUnit->getX(), pUnit->getY());
		pPlayer->GetHomelandAI()->LogHomelandMessage(logMsg);
	}

	CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_HEATHEN_CONVERTS");
	CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_HEATHEN_CONVERTS");
	pPlayer->GetNotifications()->Add(NOTIFICATION_GENERIC, strBuffer, strSummary, pUnit->getX(), pUnit->getY(), -1);

	if (pPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
	{
		pPlot->setImprovementType(NO_IMPROVEMENT);

		CvBarbarians::DoBarbCampCleared(pPlot, pPlayer->GetID());
		pPlot->SetPlayerThatClearedBarbCampHere(pPlayer->GetID());


	}

	return true;
}
