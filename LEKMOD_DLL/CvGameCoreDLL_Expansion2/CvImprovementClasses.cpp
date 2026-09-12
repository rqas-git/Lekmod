






#include "CvGameCoreDLLPCH.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvImprovementClasses.h"
#include "FireWorks/FRemark.h"
#include "CvInfosSerializationHelper.h"


#include "LintFree.h"

#ifdef _MSC_VER
#pragma warning ( disable : 4505 )
#endif
REMARK_GROUP("CvImprovementClasses");





CvImprovementResourceInfo::CvImprovementResourceInfo() :
	m_iDiscoverRand(0),
	m_bResourceMakesValid(false),
	m_bResourceTrade(false),
	m_piYieldChange(NULL)
{
}

CvImprovementResourceInfo::~CvImprovementResourceInfo()
{
	SAFE_DELETE_ARRAY(m_piYieldChange);
}

int CvImprovementResourceInfo::getDiscoverRand() const
{
	return m_iDiscoverRand;
}

bool CvImprovementResourceInfo::isResourceMakesValid() const
{
	return m_bResourceMakesValid;
}

bool CvImprovementResourceInfo::isResourceTrade() const
{
	return m_bResourceTrade;
}

int CvImprovementResourceInfo::getYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChange ? m_piYieldChange[i] : -1;
}





CvImprovementEntry::CvImprovementEntry(void):
	m_iGoldMaintenance(0),
	m_iCultureBombRadius(0),
	m_iCultureBombRadiusNeutral(0),
	m_iRequiresXAdjacentLand(-1),
	m_iCultureAdjacentSameType(0),
	m_iTilesPerGoody(0),
	m_iGoodyUniqueRange(0),
	m_iFeatureGrowthProbability(0),
	m_iUpgradeTime(0),
	m_iRiverSideUpgradeMod(0),
	m_iCoastalLandUpgradeMod(0),
	m_iHillsUpgradeMod(0),
	m_iFreshWaterUpgradeMod(0),
	m_iDefenseModifier(0),
	m_iDefenseModifierGlobal(0),
	m_iNearbyEnemyDamage(0),
	m_iPillageGold(0),
	m_iResourceExtractionMod(0),
	m_iLuxuryCopiesSiphonedFromMinor(0),
	m_iImprovementPillage(NO_IMPROVEMENT),
	m_iImprovementUpgrade(NO_IMPROVEMENT),
	m_bHillsMakesValid(false),
	m_bWaterAdjacencyMakesValid(false),
	m_bFreshWaterMakesValid(false),
	m_bRiverSideMakesValid(false),
	m_bNoFreshWater(false),
#if defined(LEKMOD_BUGANDA_LAKE)
	m_bAdjacentCityMakesValid(false),
	m_bFreshWaterSource(false),
#endif
	m_bRequiresFlatlands(false),
	m_bRequiresFlatlandsOrFreshWater(false),
	m_bRequiresFeature(false),
	m_bRequiresImprovement(false),
	m_bRemovesResource(false),
	m_bPromptWhenComplete(false),
	m_bWater(false),
	m_bCoastal(false),
	m_bDestroyedWhenPillaged(false),
	m_bDisplacePillager(false),
	m_bBuildableOnResources(false),
	m_bBarbarianCamp(false),
	m_bGoody(false),
	m_bPermanent(false),
	m_bOutsideBorders(false),
	m_bInAdjacentFriendly(false),
	m_bIgnoreOwnership(false),
	m_bOnlyCityStateTerritory(false),
	m_bNoTwoAdjacent(false),
	m_bAdjacentLuxury(false),
	m_bAllowsWalkWater(false),
	m_bAllowsSailLand(false),
	m_bCreatedByGreatPerson(false),
	m_bSpecificCivRequired(false),
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
	m_bActsAsRoute(false),
	m_eActsAsRouteTech(NO_TECH),
	m_eActsAsRailroadTech(NO_TECH),
	m_iStackedDomainDefensePenalty(0),
#endif
	m_eImprovementUsageType(IMPROVEMENTUSAGE_BASIC),
	m_eRequiredCivilization(NO_CIVILIZATION),
	m_iWorldSoundscapeScriptId(0),
	m_piResourceQuantityRequirements(NULL),
	m_piPrereqNatureYield(NULL),
	m_piYieldChange(NULL),
	m_piYieldPerEra(NULL),
	m_piRiverSideYieldChange(NULL),
	m_piCoastalLandYieldChange(NULL),
	m_piHillsYieldChange(NULL),
	m_piFreshWaterChange(NULL),
	m_piAdjacentCityYieldChange(NULL),
	m_piAdjacentMountainYieldChange(NULL),
	m_piFlavorValue(NULL),
	m_pbTerrainMakesValid(NULL),
	m_pbFeatureMakesValid(NULL),
	m_pbImprovementMakesValid(NULL),
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	m_ppiTechYieldChanges(std::pair<int**, size_t>(NULL, 0)),
	m_ppiTechNoFreshWaterYieldChanges(std::pair<int**, size_t>(NULL, 0)),
	m_ppiTechFreshWaterYieldChanges(std::pair<int**, size_t>(NULL, 0)),
	m_ppiRouteYieldChanges(std::pair<int**, size_t>(NULL, 0)),
#else
	m_ppiTechYieldChanges(NULL),
	m_ppiTechNoFreshWaterYieldChanges(NULL),
	m_ppiTechFreshWaterYieldChanges(NULL),
#if defined(LEKMOD_ERA_ENHANCED_YIELDS)
	m_ppiEraYieldChanges(NULL),
#endif
	m_ppiRouteYieldChanges(NULL),
#endif
#ifdef LEKMOD_ADJACENT_IMPROVEMENT_YIELD
	m_piImprovementAdjacentBonusCivilization(NULL),
	m_ppiImprovementAdjacentCivilizationAmount(NULL),
	m_ppiImprovementAdjacentBonusCivilizationNoAmount(NULL),
	m_ppiImprovementAdjacentBonus(NULL),
	m_ppiImprovementAdjacentAmount(NULL),
	m_bHasAnyAdjacencyYieldBonus(false),

#endif

	m_paImprovementResource(NULL)
{
}


CvImprovementEntry::~CvImprovementEntry(void)
{
	SAFE_DELETE_ARRAY(m_piResourceQuantityRequirements);
	SAFE_DELETE_ARRAY(m_piPrereqNatureYield);
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piYieldPerEra);
	SAFE_DELETE_ARRAY(m_piRiverSideYieldChange);
	SAFE_DELETE_ARRAY(m_piCoastalLandYieldChange);
	SAFE_DELETE_ARRAY(m_piHillsYieldChange);
	SAFE_DELETE_ARRAY(m_piFreshWaterChange);
	SAFE_DELETE_ARRAY(m_piAdjacentCityYieldChange);
	SAFE_DELETE_ARRAY(m_piAdjacentMountainYieldChange);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_pbTerrainMakesValid);
	SAFE_DELETE_ARRAY(m_pbFeatureMakesValid);
	SAFE_DELETE_ARRAY(m_pbImprovementMakesValid);

	if(m_paImprovementResource != NULL)
	{
		SAFE_DELETE_ARRAY(m_paImprovementResource);
	}

#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	CvDatabaseUtility::SafeDelete2DArray(m_ppiTechYieldChanges.first, m_ppiTechYieldChanges.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiTechNoFreshWaterYieldChanges.first, m_ppiTechNoFreshWaterYieldChanges.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiTechFreshWaterYieldChanges.first, m_ppiTechFreshWaterYieldChanges.second);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiRouteYieldChanges.first, m_ppiRouteYieldChanges.second);
#else
	if(m_ppiTechYieldChanges != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray(m_ppiTechYieldChanges);
	}

	if(m_ppiTechNoFreshWaterYieldChanges != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray(m_ppiTechNoFreshWaterYieldChanges);
	}

	if(m_ppiTechFreshWaterYieldChanges != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray(m_ppiTechFreshWaterYieldChanges);
	}
#if defined(LEKMOD_ERA_ENHANCED_YIELDS)
	if (m_ppiEraYieldChanges != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray(m_ppiEraYieldChanges);
	}
#endif
	if(m_ppiRouteYieldChanges != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray(m_ppiRouteYieldChanges);
	}
#endif
#ifdef LEKMOD_ADJACENT_IMPROVEMENT_YIELD
	if (m_ppiImprovementAdjacentBonus != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray(m_ppiImprovementAdjacentBonus);
	}

	if (m_ppiImprovementAdjacentCivilizationAmount != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray(m_ppiImprovementAdjacentCivilizationAmount);
	}

	if (m_ppiImprovementAdjacentBonusCivilizationNoAmount != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray(m_ppiImprovementAdjacentBonusCivilizationNoAmount);
	}

	if (m_piImprovementAdjacentBonusCivilization != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray(m_piImprovementAdjacentBonusCivilization);
	}

	if (m_ppiImprovementAdjacentAmount != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray(m_ppiImprovementAdjacentAmount);
	}

#endif
}


bool CvImprovementEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
#ifdef LEKMOD_ADJACENT_IMPROVEMENT_YIELD

	m_bHasAnyAdjacencyYieldBonus = false;
#endif
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;


	const char* szArtDefineTag = kResults.GetText("ArtDefineTag");
	SetArtDefineTag(szArtDefineTag);

	m_iGoldMaintenance = kResults.GetInt("GoldMaintenance");
	m_iCultureBombRadius = kResults.GetInt("CultureBombRadius");
	m_iCultureBombRadiusNeutral = kResults.GetInt("CultureBombRadiusNeutral");
	m_iRequiresXAdjacentLand = kResults.GetInt("RequiresXAdjacentLand");
	m_iCultureAdjacentSameType = kResults.GetInt("CultureAdjacentSameType");
	m_bHillsMakesValid = kResults.GetBool("HillsMakesValid");
	m_bWaterAdjacencyMakesValid = kResults.GetBool("WaterAdjacencyMakesValid");
	m_bFreshWaterMakesValid = kResults.GetBool("FreshWaterMakesValid");
	m_bRiverSideMakesValid = kResults.GetBool("RiverSideMakesValid");
	m_bNoFreshWater = kResults.GetBool("NoFreshWater");
#if defined(LEKMOD_BUGANDA_LAKE)
	m_bAdjacentCityMakesValid = kResults.GetBool("AdjacentCityMakesValid");
	m_bFreshWaterSource = kResults.GetBool("FreshWaterSource");
#endif
	m_bRequiresFlatlands = kResults.GetBool("RequiresFlatlands");
	m_bRequiresFlatlandsOrFreshWater = kResults.GetBool("RequiresFlatlandsOrFreshWater");
	m_bRequiresFeature = kResults.GetBool("RequiresFeature");
	m_bRequiresImprovement = kResults.GetBool("RequiresImprovement");
	m_bRemovesResource = kResults.GetBool("RemovesResource");
	m_bPromptWhenComplete = kResults.GetBool("PromptWhenComplete");
	m_bWater = kResults.GetBool("Water");
	m_bCoastal = kResults.GetBool("Coastal");
	m_bDestroyedWhenPillaged = kResults.GetBool("DestroyedWhenPillaged");
	m_bDisplacePillager = kResults.GetBool("DisplacePillager");
	m_bBuildableOnResources = kResults.GetBool("BuildableOnResources");
	m_eImprovementUsageType = m_bBuildableOnResources ? IMPROVEMENTUSAGE_LANDMARK : IMPROVEMENTUSAGE_BASIC;
	m_bBarbarianCamp = kResults.GetBool("BarbarianCamp");
	m_bGoody = kResults.GetBool("Goody");
	m_bPermanent = kResults.GetBool("Permanent");
	m_iTilesPerGoody = kResults.GetInt("TilesPerGoody");
	m_iGoodyUniqueRange = kResults.GetInt("GoodyRange");
	m_iFeatureGrowthProbability = kResults.GetInt("FeatureGrowth");
	m_iUpgradeTime = kResults.GetInt("UpgradeTime");
	m_iRiverSideUpgradeMod = kResults.GetInt("RiverSideUpgradeMod");
	m_iCoastalLandUpgradeMod = kResults.GetInt("CoastalLandUpgradeMod");
	m_iHillsUpgradeMod = kResults.GetInt("HillsUpgradeMod");
	m_iFreshWaterUpgradeMod = kResults.GetInt("FreshWaterUpgradeMod");
	m_iDefenseModifier = kResults.GetInt("DefenseModifier");
	m_iDefenseModifierGlobal = kResults.GetInt("DefenseModifierGlobal");
	m_iNearbyEnemyDamage = kResults.GetInt("NearbyEnemyDamage");
	m_iPillageGold = kResults.GetInt("PillageGold");
	m_bOutsideBorders = kResults.GetBool("OutsideBorders");
	m_bInAdjacentFriendly = kResults.GetBool("InAdjacentFriendly");
	m_bIgnoreOwnership = kResults.GetBool("IgnoreOwnership");
	m_bOnlyCityStateTerritory = kResults.GetBool("OnlyCityStateTerritory");
	m_bNoTwoAdjacent = kResults.GetBool("NoTwoAdjacent");
	m_bAdjacentLuxury = kResults.GetBool("AdjacentLuxury");
	m_bAllowsWalkWater = kResults.GetBool("AllowsWalkWater");
	m_bAllowsSailLand = kResults.GetBool("AllowsSailLand");
	m_bCreatedByGreatPerson = kResults.GetBool("CreatedByGreatPerson");
	m_bSpecificCivRequired = kResults.GetBool("SpecificCivRequired");
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
	m_bActsAsRoute = kResults.GetBool("ActsAsRoute");
	m_iStackedDomainDefensePenalty = kResults.GetInt("StackedDomainDefensePenalty");
	{
		const char* szRouteTech = kResults.GetText("ActsAsRouteTech");
		m_eActsAsRouteTech = (szRouteTech != NULL) ? (TechTypes)GC.getInfoTypeForString(szRouteTech, true) : NO_TECH;
		const char* szRailTech = kResults.GetText("ActsAsRailroadTech");
		m_eActsAsRailroadTech = (szRailTech != NULL) ? (TechTypes)GC.getInfoTypeForString(szRailTech, true) : NO_TECH;
	}
#endif
	m_iResourceExtractionMod = kResults.GetInt("ResourceExtractionMod");
	m_iLuxuryCopiesSiphonedFromMinor = kResults.GetInt("LuxuryCopiesSiphonedFromMinor");

	const char* szCivilizationType = kResults.GetText("CivilizationType");
	m_eRequiredCivilization = (CivilizationTypes)GC.getInfoTypeForString(szCivilizationType, true);

#ifdef LEKMOD_CUSTOM_IMPROVEMENT_ICONS
	const char* szIconString = kResults.GetText("IconString");
	SetIconString(szIconString);
#endif


	const char* szWorldsoundscapeAudioScript = kResults.GetText("WorldSoundscapeAudioScript");
	if(szWorldsoundscapeAudioScript != NULL)
	{
		m_iWorldSoundscapeScriptId = gDLL->GetAudioTagIndex(szWorldsoundscapeAudioScript, AUDIOTAG_SOUNDSCAPE);
	}
	else
	{
		m_iWorldSoundscapeScriptId = -1;
		Remark(1, "Warning: Missing soundscape definition in XML for feature: '%s'", GetType());
	}

	const char* szImprovementPillage = kResults.GetText("ImprovementPillage");
	m_iImprovementPillage = GC.getInfoTypeForString(szImprovementPillage, true);

	const char* szImprovementUpgrade = kResults.GetText("ImprovementUpgrade");
	m_iImprovementUpgrade = GC.getInfoTypeForString(szImprovementUpgrade, true);


	const char* szImprovementType = GetType();
	const size_t lenImprovementType = strlen(szImprovementType);

	kUtility.PopulateArrayByExistence(m_pbTerrainMakesValid,
	                                  "Terrains",
	                                  "Improvement_ValidTerrains",
	                                  "TerrainType",
	                                  "ImprovementType",
	                                  szImprovementType);

	kUtility.PopulateArrayByExistence(m_pbFeatureMakesValid,
	                                  "Features",
	                                  "Improvement_ValidFeatures",
	                                  "FeatureType",
	                                  "ImprovementType",
	                                  szImprovementType);

	kUtility.PopulateArrayByExistence(m_pbImprovementMakesValid,
									  "Improvements",
									  "Improvement_ValidImprovements",
									  "PrereqImprovement",
									  "ImprovementType",
							          szImprovementType);

	kUtility.SetYields(m_piYieldChange, "Improvement_Yields", "ImprovementType", szImprovementType);
	kUtility.SetYields(m_piYieldPerEra, "Improvement_YieldPerEra", "ImprovementType", szImprovementType);
	kUtility.SetYields(m_piAdjacentCityYieldChange, "Improvement_AdjacentCityYields", "ImprovementType", szImprovementType);
	kUtility.SetYields(m_piAdjacentMountainYieldChange, "Improvement_AdjacentMountainYieldChanges", "ImprovementType", szImprovementType);
	kUtility.SetYields(m_piCoastalLandYieldChange, "Improvement_CoastalLandYields", "ImprovementType", szImprovementType);
	kUtility.SetYields(m_piFreshWaterChange, "Improvement_FreshWaterYields", "ImprovementType", szImprovementType);
	kUtility.SetYields(m_piHillsYieldChange, "Improvement_HillsYields", "ImprovementType", szImprovementType);
	kUtility.SetYields(m_piRiverSideYieldChange, "Improvement_RiverSideYields", "ImprovementType", szImprovementType);
	kUtility.SetYields(m_piPrereqNatureYield, "Improvement_PrereqNatureYields", "ImprovementType", szImprovementType);

	kUtility.SetFlavors(m_piFlavorValue, "Improvement_Flavors", "ImprovementType", szImprovementType);



	{

		const int iNumResources = kUtility.MaxRows("Resources");
		m_paImprovementResource = FNEW(CvImprovementResourceInfo[iNumResources], c_eCiv5GameplayDLL, 0);

		kUtility.InitializeArray(m_piResourceQuantityRequirements, iNumResources);


		std::string strResourceTypesKey = "Improvements - ResourceTypes";
		Database::Results* pResourceTypes = kUtility.GetOrPrepareResults(strResourceTypesKey,
			"select Resources.ID, ResourceType, ResourceMakesValid, ResourceTrade, DiscoveryRand from Improvement_ResourceTypes inner join Resources on ResourceType = Resources.Type where ImprovementType = ?");

		std::string strYieldResultsKey = "Improvements - YieldResults";
		Database::Results* pYieldResults = kUtility.GetOrPrepareResults(strYieldResultsKey,
			"select Yields.ID, Yield from Improvement_ResourceType_Yields inner join Yields on YieldType = Yields.Type where ImprovementType = ? and ResourceType = ?");

		pResourceTypes->Bind(1, szImprovementType, lenImprovementType, false);

		while(pResourceTypes->Step())
		{
			const int idx = pResourceTypes->GetInt("ID");
			CvImprovementResourceInfo& pResourceInfo = m_paImprovementResource[idx];

			const char* szResourceType = pResourceTypes->GetText("ResourceType");
			pResourceInfo.m_bResourceMakesValid = pResourceTypes->GetBool("ResourceMakesValid");
			pResourceInfo.m_bResourceTrade = pResourceTypes->GetBool("ResourceTrade");
			pResourceInfo.m_iDiscoverRand = pResourceTypes->GetInt("DiscoveryRand");
			m_piResourceQuantityRequirements[idx] = pResourceTypes->GetInt("QuantityRequirement");


			kUtility.InitializeArray(pResourceInfo.m_piYieldChange, "Yields");

			pYieldResults->Bind(1, szImprovementType, lenImprovementType, false);
			pYieldResults->Bind(2, szResourceType, -1, false);
			while(pYieldResults->Step())
			{
				const int yieldIdx = pYieldResults->GetInt("ID");
				const int yield = pYieldResults->GetInt("Yield");
				pResourceInfo.m_piYieldChange[yieldIdx] = yield;
			}

			pYieldResults->Reset();
		}

		pResourceTypes->Reset();
	}


	const int iNumYields = kUtility.MaxRows("Yields");
	const int iNumTechs = GC.getNumTechInfos();
	CvAssertMsg(iNumTechs > 0, "Num Tech Infos <= 0");
#if defined(LEKMOD_ERA_ENHANCED_YIELDS)
	const int iNumEras = kUtility.MaxRows("Eras");
	{
		kUtility.Initialize2DArray(m_ppiEraYieldChanges, iNumEras, iNumYields);
		std::string strKey = "Improvements_EraYieldChanges";
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Eras.ID as EraID, Yields.ID as YieldID, Yield from Improvement_EraYieldChanges inner join Yields on YieldType = Yields.Type inner join Eras on EraType = Eras.Type where ImprovementType = ?");
		pResults->Bind(1, szImprovementType, lenImprovementType, false);
		while (pResults->Step())
		{
			const int era_idx = pResults->GetInt(0);
			CvAssert(era_idx > -1);
			const int yield_idx = pResults->GetInt(1);
			CvAssert(yield_idx > -1);
			const int yield = pResults->GetInt(2);
			m_ppiEraYieldChanges[era_idx][yield_idx] = yield;
		}
		pResults->Reset();
	}
#endif

	{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
		kUtility.Initialize2DArray(m_ppiTechYieldChanges.first, iNumTechs, iNumYields);
		m_ppiTechYieldChanges.second = iNumTechs;
#else
		kUtility.Initialize2DArray(m_ppiTechYieldChanges, iNumTechs, iNumYields);
#endif

		std::string strKey = "Improvements - TechYieldChanges";
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Yields.ID as YieldID, Technologies.ID as TechID, Yield from Improvement_TechYieldChanges inner join Yields on YieldType = Yields.Type inner join Technologies on TechType = Technologies.Type where ImprovementType = ?");

		pResults->Bind(1, szImprovementType, lenImprovementType, false);

		while(pResults->Step())
		{
			const int yield_idx = pResults->GetInt(0);
			CvAssert(yield_idx > -1);

			const int tech_idx = pResults->GetInt(1);
			CvAssert(tech_idx > -1);

			const int yield = pResults->GetInt(2);

#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
			m_ppiTechYieldChanges.first[tech_idx][yield_idx] = yield;
#else
			m_ppiTechYieldChanges[tech_idx][yield_idx] = yield;
#endif
		}
	}

#ifdef LEKMOD_ADJACENT_IMPROVEMENT_YIELD

	{
		
		const int iImprovementTypes = kUtility.MaxRows("Improvements");
		const int iNumYields = kUtility.MaxRows("Yields");
		const int iNumMaxAmount = 7;

		kUtility.Initialize2DArray(m_piImprovementAdjacentBonusCivilization, 200, iImprovementTypes);
		kUtility.Initialize2DArray(m_ppiImprovementAdjacentCivilizationAmount, iNumYields, iNumMaxAmount);
		kUtility.Initialize2DArray(m_ppiImprovementAdjacentBonusCivilizationNoAmount, iImprovementTypes, iNumYields);

		std::string strKey = "Improvement_AdjacencyYieldCivilization";
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select OtherImprovementType, Civilizations.ID, Amount, Yields.ID, Yield from Improvement_AdjacencyYieldCivilization inner join Yields on YieldType = Yields.Type inner join Civilizations on CivilizationType = Civilizations.Type where ImprovementType = ?");

		pResults->Bind(1, szImprovementType, lenImprovementType, false);

		while (pResults->Step())
		{
			const int sImprovementType = GC.getInfoTypeForString(pResults->GetText(0), true);
			CvAssert(sImprovementType > -1);
			const int iCivilizationType = pResults->GetInt(1);
			CvAssert(iCivilizationType > -1);
			const int iAmount = pResults->GetInt(2);
			CvAssert(iAmount > -1);
			const int iYieldType = pResults->GetInt(3);
			CvAssert(iYieldType > -1);
			const int iYield = pResults->GetInt(4);

			m_piImprovementAdjacentBonusCivilization[iCivilizationType][sImprovementType] = sImprovementType;
			if (iAmount == 0)
			{
				m_ppiImprovementAdjacentBonusCivilizationNoAmount[sImprovementType][iYieldType] = iYield;
			}
			else if (iAmount > 0 && iAmount < iNumMaxAmount)
			{
				m_ppiImprovementAdjacentCivilizationAmount[iYieldType][iAmount] = iYield;
			}
		}

		pResults->Reset();

	}
	

	{
		
		const int iImprovementTypes = kUtility.MaxRows("Improvements");
		const int iNumYields = kUtility.MaxRows("Yields");
		const int iNumMaxAmount = 7;
		kUtility.Initialize2DArray(m_ppiImprovementAdjacentBonus, iImprovementTypes, iNumYields);
		kUtility.Initialize2DArray(m_ppiImprovementAdjacentAmount, iImprovementTypes, iNumMaxAmount);

		std::string strKey = "Improvement_AdjacencyYield";
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select OtherImprovementType, Amount, Yields.ID, Yield from Improvement_AdjacencyYield inner join Yields on YieldType = Yields.Type where ImprovementType = ?");

		pResults->Bind(1, szImprovementType, lenImprovementType, false);

		while (pResults->Step())
		{
			const int sImprovementType = GC.getInfoTypeForString(pResults->GetText(0), true);
			CvAssert(sImprovementType > -1);
			const int iAmount = pResults->GetInt(1);
			CvAssert(iAmount > -1);
			const int iYieldType = pResults->GetInt(2);
			CvAssert(iYieldType > -1);
			const int iYield = pResults->GetInt(3);

			if (iAmount == 0)
			{
				m_ppiImprovementAdjacentBonus[sImprovementType][iYieldType] = iYield;
			}
			else if (iAmount > 0 && iAmount < iNumMaxAmount)
			{
				m_ppiImprovementAdjacentAmount[iYieldType][iAmount] = iYield;
			}
		}

		pResults->Reset();

	}
	

#endif


	{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
		kUtility.Initialize2DArray(m_ppiTechNoFreshWaterYieldChanges.first, iNumTechs, iNumYields);
		m_ppiTechNoFreshWaterYieldChanges.second = iNumTechs;
#else
		kUtility.Initialize2DArray(m_ppiTechNoFreshWaterYieldChanges, iNumTechs, iNumYields);
#endif

		std::string strKey = "Improvements - TechNoFreshWaterYieldChanges";
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Yields.ID as YieldID, Technologies.ID as TechID, Yield from Improvement_TechNoFreshWaterYieldChanges inner join Yields on YieldType = Yields.Type inner join Technologies on TechType = Technologies.Type where ImprovementType = ?");

		pResults->Bind(1, szImprovementType, lenImprovementType, false);

		while(pResults->Step())
		{
			const int yield_idx = pResults->GetInt(0);
			CvAssert(yield_idx > -1);

			const int tech_idx = pResults->GetInt(1);
			CvAssert(tech_idx > -1);

			const int yield = pResults->GetInt(2);

#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
			m_ppiTechNoFreshWaterYieldChanges.first[tech_idx][yield_idx] = yield;
#else
			m_ppiTechNoFreshWaterYieldChanges[tech_idx][yield_idx] = yield;
#endif
		}

		pResults->Reset();

	}


	{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
		kUtility.Initialize2DArray(m_ppiTechFreshWaterYieldChanges.first, iNumTechs, iNumYields);
		m_ppiTechFreshWaterYieldChanges.second = iNumTechs;
#else
		kUtility.Initialize2DArray(m_ppiTechFreshWaterYieldChanges, iNumTechs, iNumYields);
#endif

		std::string strKey = "Improvements - TechFreshWaterYieldChanges";
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Yields.ID as YieldID, Technologies.ID as TechID, Yield from Improvement_TechFreshWaterYieldChanges inner join Yields on YieldType = Yields.Type inner join Technologies on TechType = Technologies.Type where ImprovementType = ?");

		pResults->Bind(1, szImprovementType, lenImprovementType, false);

		while(pResults->Step())
		{
			const int yield_idx = pResults->GetInt(0);
			CvAssert(yield_idx > -1);

			const int tech_idx = pResults->GetInt(1);
			CvAssert(tech_idx > -1);

			const int yield = pResults->GetInt(2);

#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
			m_ppiTechFreshWaterYieldChanges.first[tech_idx][yield_idx] = yield;
#else
			m_ppiTechFreshWaterYieldChanges[tech_idx][yield_idx] = yield;
#endif
		}

		pResults->Reset();

	}


	{
		const int iNumRoutes = kUtility.MaxRows("Routes");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
		kUtility.Initialize2DArray(m_ppiRouteYieldChanges.first, iNumRoutes, iNumYields);
		m_ppiRouteYieldChanges.second = iNumRoutes;
#else
		kUtility.Initialize2DArray(m_ppiRouteYieldChanges, iNumRoutes, iNumYields);
#endif

		std::string strKey = "Improvements - RouteYieldChanges";
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Yields.ID as YieldID, Routes.ID as RouteID, Yield from Improvement_RouteYieldChanges inner join Yields on YieldType = Yields.Type inner join Routes on RouteType = Routes.Type where ImprovementType = ?;");

		pResults->Bind(1, szImprovementType, lenImprovementType, false);

		while(pResults->Step())
		{
			const int yield_idx = pResults->GetInt(0);
			CvAssert(yield_idx > -1);

			const int route_idx = pResults->GetInt(1);
			CvAssert(route_idx > -1);

			const int yield = pResults->GetInt(2);

#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
			m_ppiRouteYieldChanges.first[route_idx][yield_idx] = yield;
#else
			m_ppiRouteYieldChanges[route_idx][yield_idx] = yield;
#endif
		}

		pResults->Reset();

	}

	return true;
}


#ifdef LEKMOD_CUSTOM_IMPROVEMENT_ICONS
const char* CvImprovementEntry::GetIconString() const
{
	return m_strIconString;
}

void CvImprovementEntry::SetIconString(const char* szVal)
{
	m_strIconString = szVal;
}
#endif


int CvImprovementEntry::GetGoldMaintenance() const
{
	return m_iGoldMaintenance;
}


int CvImprovementEntry::GetCultureBombRadius() const
{
	return m_iCultureBombRadius;
}

int CvImprovementEntry::GetCultureBombRadiusNeutral() const
{
	return m_iCultureBombRadiusNeutral;
}


int CvImprovementEntry::GetRequiresXAdjacentLand() const
{
	return m_iRequiresXAdjacentLand;
}


int CvImprovementEntry::GetCultureAdjacentSameType() const
{
	return m_iCultureAdjacentSameType;
}


int CvImprovementEntry::GetTilesPerGoody() const
{
	return m_iTilesPerGoody;
}


int CvImprovementEntry::GetGoodyUniqueRange() const
{
	return m_iGoodyUniqueRange;
}


int CvImprovementEntry::GetFeatureGrowthProbability() const
{
	return m_iFeatureGrowthProbability;
}


int CvImprovementEntry::GetUpgradeTime() const
{
	return m_iUpgradeTime;
}


int CvImprovementEntry::GetRiverSideUpgradeMod() const
{
	return m_iRiverSideUpgradeMod;
}


int CvImprovementEntry::GetCoastalLandUpgradeMod() const
{
	return m_iCoastalLandUpgradeMod;
}


int CvImprovementEntry::GetHillsUpgradeMod() const
{
	return m_iHillsUpgradeMod;
}


int CvImprovementEntry::GetFreshWaterUpgradeMod() const
{
	return m_iFreshWaterUpgradeMod;
}


int CvImprovementEntry::GetDefenseModifier() const
{
	return m_iDefenseModifier;
}


int CvImprovementEntry::GetDefenseModifierGlobal() const
{
	return m_iDefenseModifierGlobal;
}


int CvImprovementEntry::GetNearbyEnemyDamage() const
{
	return m_iNearbyEnemyDamage;
}


int CvImprovementEntry::GetPillageGold() const
{
	return m_iPillageGold;
}


int CvImprovementEntry::GetResourceExtractionMod() const
{
	return m_iResourceExtractionMod;
}


int CvImprovementEntry::GetLuxuryCopiesSiphonedFromMinor() const
{
	return m_iLuxuryCopiesSiphonedFromMinor;
}


int CvImprovementEntry::GetImprovementPillage() const
{
	return m_iImprovementPillage;
}


void CvImprovementEntry::SetImprovementPillage(int i)
{
	m_iImprovementPillage = i;
}


int CvImprovementEntry::GetImprovementUpgrade() const
{
	return m_iImprovementUpgrade;
}


void CvImprovementEntry::SetImprovementUpgrade(int i)
{
	m_iImprovementUpgrade = i;
}


bool CvImprovementEntry::IsHillsMakesValid() const
{
	return m_bHillsMakesValid;
}

bool CvImprovementEntry::IsWaterAdjacencyMakesValid() const
{
	return m_bWaterAdjacencyMakesValid;
}


bool CvImprovementEntry::IsFreshWaterMakesValid() const
{
	return m_bFreshWaterMakesValid;
}


bool CvImprovementEntry::IsRiverSideMakesValid() const
{
	return m_bRiverSideMakesValid;
}


bool CvImprovementEntry::IsNoFreshWater() const
{
	return m_bNoFreshWater;
}
#if defined(LEKMOD_BUGANDA_LAKE)

bool CvImprovementEntry::IsAdjacentCityMakesValid() const
{
	return m_bAdjacentCityMakesValid;
}

bool CvImprovementEntry::IsFreshWaterSource() const
{
	return m_bFreshWaterSource;
}
#endif

bool CvImprovementEntry::IsRequiresFlatlands() const
{
	return m_bRequiresFlatlands;
}


bool CvImprovementEntry::IsRequiresFlatlandsOrFreshWater() const
{
	return m_bRequiresFlatlandsOrFreshWater;
}


bool CvImprovementEntry::IsRequiresFeature() const
{
	return m_bRequiresFeature;
}


bool CvImprovementEntry::IsRequiresImprovement() const
{
	return m_bRequiresImprovement;
}


bool CvImprovementEntry::IsRemovesResource() const
{
	return m_bRemovesResource;
}


bool CvImprovementEntry::IsPromptWhenComplete() const
{
	return m_bPromptWhenComplete;
}


bool CvImprovementEntry::IsWater() const
{
	return m_bWater;
}


bool CvImprovementEntry::IsCoastal() const
{
	return m_bCoastal;
}



bool CvImprovementEntry::IsDestroyedWhenPillaged() const
{
	return m_bDestroyedWhenPillaged;
}


bool CvImprovementEntry::IsDisplacePillager() const
{
	return m_bDisplacePillager;
}


bool CvImprovementEntry::IsBuildableOnResources() const
{
	return m_bBuildableOnResources;
}


bool CvImprovementEntry::IsBarbarianCamp() const
{
	return m_bBarbarianCamp;
}


bool CvImprovementEntry::IsGoody() const
{
	return m_bGoody;
}


bool CvImprovementEntry::IsPermanent() const
{
	return m_bPermanent;
}


bool CvImprovementEntry::IsOutsideBorders() const
{
	return m_bOutsideBorders;
}


bool CvImprovementEntry::IsIgnoreOwnership() const
{
	return m_bIgnoreOwnership;
}


bool CvImprovementEntry::IsOnlyCityStateTerritory() const
{
	return m_bOnlyCityStateTerritory;
}


bool CvImprovementEntry::IsNoTwoAdjacent() const
{
	return m_bNoTwoAdjacent;
}


bool CvImprovementEntry::IsAdjacentLuxury() const
{
	return m_bAdjacentLuxury;
}


bool CvImprovementEntry::IsAllowsWalkWater() const
{
	return m_bAllowsWalkWater;
}

bool CvImprovementEntry::IsAllowsSailLand() const
{
    return m_bAllowsSailLand;
}
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
bool CvImprovementEntry::IsActsAsRoute() const
{
	return m_bActsAsRoute;
}
TechTypes CvImprovementEntry::GetActsAsRouteTech() const
{
	return m_eActsAsRouteTech;
}
TechTypes CvImprovementEntry::GetActsAsRailroadTech() const
{
	return m_eActsAsRailroadTech;
}
int CvImprovementEntry::GetStackedDomainDefensePenalty() const
{
	return m_iStackedDomainDefensePenalty;
}
#endif

bool CvImprovementEntry::IsInAdjacentFriendly() const
{
	return m_bInAdjacentFriendly;
}

bool CvImprovementEntry::IsCreatedByGreatPerson() const
{
	return m_bCreatedByGreatPerson;
}

bool CvImprovementEntry::IsSpecificCivRequired() const
{
	return m_bSpecificCivRequired;
}

CivilizationTypes CvImprovementEntry::GetRequiredCivilization() const
{
	return m_eRequiredCivilization;
}


const char* CvImprovementEntry::GetArtDefineTag() const
{
	return m_strArtDefineTag;
}


void CvImprovementEntry::SetArtDefineTag(const char* szVal)
{
	m_strArtDefineTag = szVal;
}

ImprovementUsageTypes CvImprovementEntry::GetImprovementUsage() const
{
	return m_eImprovementUsageType;
}

void CvImprovementEntry::SetImprovementUsage(const ImprovementUsageTypes usageType)
{
	m_eImprovementUsageType = usageType;
}


int CvImprovementEntry::GetWorldSoundscapeScriptId() const
{
	return m_iWorldSoundscapeScriptId;
}


int CvImprovementEntry::GetResourceQuantityRequirement(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piResourceQuantityRequirements ? m_piResourceQuantityRequirements[i] : -1;
}


int CvImprovementEntry::GetPrereqNatureYield(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piPrereqNatureYield ? m_piPrereqNatureYield[i] : -1;
}

int* CvImprovementEntry::GetPrereqNatureYieldArray()
{
	return m_piPrereqNatureYield;
}


int CvImprovementEntry::GetYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}

int* CvImprovementEntry::GetYieldChangeArray()
{
	return m_piYieldChange;
}


int CvImprovementEntry::GetYieldChangePerEra(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldPerEra ? m_piYieldPerEra[i] : 0;
}


int CvImprovementEntry::GetRiverSideYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piRiverSideYieldChange ? m_piRiverSideYieldChange[i] : 0;
}

int* CvImprovementEntry::GetRiverSideYieldChangeArray()
{
	return m_piRiverSideYieldChange;
}


int CvImprovementEntry::GetCoastalLandYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piCoastalLandYieldChange ? m_piCoastalLandYieldChange[i] : 0;
}

int* CvImprovementEntry::GetCoastalLandYieldChangeArray()
{
	return m_piCoastalLandYieldChange;
}


int CvImprovementEntry::GetHillsYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piHillsYieldChange ? m_piHillsYieldChange[i] : 0;
}

int* CvImprovementEntry::GetHillsYieldChangeArray()
{
	return m_piHillsYieldChange;
}


int CvImprovementEntry::GetFreshWaterYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piFreshWaterChange ? m_piFreshWaterChange[i] : 0;
}

int* CvImprovementEntry::GetFreshWaterYieldChangeArray()
{
	return m_piFreshWaterChange;
}


int CvImprovementEntry::GetAdjacentCityYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piAdjacentCityYieldChange ? m_piAdjacentCityYieldChange[i] : 0;
}

int* CvImprovementEntry::GetAdjacentCityYieldChangeArray()
{
	return m_piAdjacentCityYieldChange;
}


int CvImprovementEntry::GetAdjacentMountainYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piAdjacentMountainYieldChange ? m_piAdjacentMountainYieldChange[i] : 0;
}

int* CvImprovementEntry::GetAdjacentMountainYieldChangeArray()
{
	return m_piAdjacentMountainYieldChange;
}


bool CvImprovementEntry::GetTerrainMakesValid(int i) const
{
	CvAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbTerrainMakesValid ? m_pbTerrainMakesValid[i] : false;
}


bool CvImprovementEntry::GetFeatureMakesValid(int i) const
{
	CvAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbFeatureMakesValid ? m_pbFeatureMakesValid[i] : false;
}


bool CvImprovementEntry::GetImprovementMakesValid(int i) const
{
	CvAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbImprovementMakesValid ? m_pbImprovementMakesValid[i] : false;
}


int CvImprovementEntry::GetTechYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumTechInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppiTechYieldChanges.first ? m_ppiTechYieldChanges.first[i][j] : 0;
#else
	return m_ppiTechYieldChanges[i][j];
#endif
}

int* CvImprovementEntry::GetTechYieldChangesArray(int i)
{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppiTechYieldChanges.first ? m_ppiTechYieldChanges.first[i] : NULL;
#else
	return m_ppiTechYieldChanges[i];
#endif
}
#if defined(LEKMOD_ERA_ENHANCED_YIELDS)
int CvImprovementEntry::GetEraYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumEraInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiEraYieldChanges[i][j];
}
#endif
#ifdef LEKMOD_ADJACENT_IMPROVEMENT_YIELD

int CvImprovementEntry::GetImprovementAdjacentBonus(int i, int j) const
{
	CvAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiImprovementAdjacentBonus[i][j];
}

int CvImprovementEntry::GetImprovementAdjacentBonusCivilization(int i, int j) const
{	
	CvAssertMsg(i < 200, "Index out of bounds");
	CvAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piImprovementAdjacentBonusCivilization[i][j];
}


int CvImprovementEntry::GetImprovementAdjacentCivilizationAmount(int i, int j) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < 6, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiImprovementAdjacentCivilizationAmount[i][j];
}

int CvImprovementEntry::GetImprovementAdjacentBonusCivilizationNoAmount(int i, int j) const
{
	CvAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiImprovementAdjacentBonusCivilizationNoAmount[i][j];
}


int CvImprovementEntry::GetImprovementAdjacentAmount(int i, int j) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < 6, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiImprovementAdjacentAmount[i][j];
}

void CvImprovementEntry::CacheAdjacencyYieldBonus()
{
	m_bHasAnyAdjacencyYieldBonus = ComputeHasAnyAdjacencyYieldBonus();
}


bool CvImprovementEntry::HasAnyAdjacencyYieldBonus() const
{
	return m_bHasAnyAdjacencyYieldBonus;
}

bool CvImprovementEntry::ComputeHasAnyAdjacencyYieldBonus() const
{
	for (int i = 0; i < GC.getNumImprovementInfos(); ++i)
	{
		for (int j = 0; j < NUM_YIELD_TYPES; ++j)
		{
			if (GetImprovementAdjacentBonus(i, j) > 0)
				return true;
			if (GetImprovementAdjacentBonusCivilization(i, j) > 0)
				return true;
			if (GetImprovementAdjacentBonusCivilizationNoAmount(i, j) > 0)
				return true;
		}
	}

	for (int i = 0; i < NUM_YIELD_TYPES; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			if (GetImprovementAdjacentAmount(i, j) > 0)
				return true;
			if (GetImprovementAdjacentCivilizationAmount(i, j) > 0)
				return true;
		}
	}

	return false;
}
#endif

int CvImprovementEntry::GetTechNoFreshWaterYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumTechInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppiTechNoFreshWaterYieldChanges.first ? m_ppiTechNoFreshWaterYieldChanges.first[i][j] : 0;
#else
	return m_ppiTechNoFreshWaterYieldChanges[i][j];
#endif
}

int* CvImprovementEntry::GetTechNoFreshWaterYieldChangesArray(int i)
{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppiTechNoFreshWaterYieldChanges.first ? m_ppiTechNoFreshWaterYieldChanges.first[i] : NULL;
#else
	return m_ppiTechNoFreshWaterYieldChanges[i];
#endif
}


int CvImprovementEntry::GetTechFreshWaterYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumTechInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppiTechFreshWaterYieldChanges.first ? m_ppiTechFreshWaterYieldChanges.first[i][j] : 0;
#else
	return m_ppiTechFreshWaterYieldChanges[i][j];
#endif
}

int* CvImprovementEntry::GetTechFreshWaterYieldChangesArray(int i)
{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppiTechFreshWaterYieldChanges.first ? m_ppiTechFreshWaterYieldChanges.first[i] : NULL;
#else
	return m_ppiTechFreshWaterYieldChanges[i];
#endif
}


int CvImprovementEntry::GetRouteYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumRouteInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppiRouteYieldChanges.first ? m_ppiRouteYieldChanges.first[i][j] : 0;
#else
	return m_ppiRouteYieldChanges[i][j];
#endif
}

int* CvImprovementEntry::GetRouteYieldChangesArray(int i)
{
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	return m_ppiRouteYieldChanges.first ? m_ppiRouteYieldChanges.first[i] : NULL;
#else
	return m_ppiRouteYieldChanges[i];
#endif
}


int CvImprovementEntry::GetImprovementResourceYield(int i, int j) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_paImprovementResource[i].m_piYieldChange ? m_paImprovementResource[i].getYieldChange(j) : 0;
}


bool CvImprovementEntry::IsImprovementResourceMakesValid(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paImprovementResource[i].m_bResourceMakesValid;
}


bool CvImprovementEntry::IsImprovementResourceTrade(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paImprovementResource[i].m_bResourceTrade;
}


int CvImprovementEntry::GetImprovementResourceDiscoverRand(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paImprovementResource[i].m_iDiscoverRand;
}


int CvImprovementEntry::GetFlavorValue(int i) const
{
	CvAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(i > -1, "Indes out of bounds");
	return m_piFlavorValue[i];
}






CvImprovementXMLEntries::CvImprovementXMLEntries(void)
{

}


CvImprovementXMLEntries::~CvImprovementXMLEntries(void)
{
	DeleteArray();
}


std::vector<CvImprovementEntry*>& CvImprovementXMLEntries::GetImprovementEntries()
{
	return m_paImprovementEntries;
}


#ifdef AUI_WARNING_FIXES
uint CvImprovementXMLEntries::GetNumImprovements() const
#else
int CvImprovementXMLEntries::GetNumImprovements()
#endif
{
	return m_paImprovementEntries.size();
}


#ifdef AUI_WARNING_FIXES
_Ret_maybenull_ CvImprovementEntry* CvImprovementXMLEntries::GetEntry(uint index)
#else
CvImprovementEntry* CvImprovementXMLEntries::GetEntry(int index)
#endif
{
	return m_paImprovementEntries[index];
}


CvImprovementEntry* CvImprovementXMLEntries::GetImprovementForResource(int eResource)
{
	for(unsigned int iImprovement = 0; iImprovement < m_paImprovementEntries.size(); ++iImprovement)
	{
		CvImprovementEntry* pImprovement = GetEntry((ImprovementTypes)iImprovement);
		if(pImprovement && pImprovement->IsImprovementResourceMakesValid(eResource))
		{
			return pImprovement;
		}
	}

	return NULL;
}


void CvImprovementXMLEntries::DeleteArray()
{
	for(std::vector<CvImprovementEntry*>::iterator it = m_paImprovementEntries.begin(); it != m_paImprovementEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paImprovementEntries.clear();
}


void ImprovementArrayHelpers::Read(FDataStream& kStream, int* paiImprovementArray)
{
	int iNumEntries;

	kStream >> iNumEntries;

	int iArraySize = GC.getNumImprovementInfos();
	for(int iI = 0; iI < iNumEntries; iI++)
	{
		uint uiHash;
		kStream >> uiHash;
		if (uiHash != 0 && uiHash != (uint)NO_IMPROVEMENT)
		{
			int iType = GC.getInfoTypeForHash(uiHash);
			if(iType != -1 && iType < iArraySize)
			{
				kStream >> paiImprovementArray[iType];
			}
			else
			{
				CvString szError;
				szError.Format("LOAD ERROR: Improvement Type not found");
				GC.LogMessage(szError.GetCString());
				CvAssertMsg(false, szError);

				int iDummy;
				kStream >> iDummy;
			}
		}
	}
}


void ImprovementArrayHelpers::Write(FDataStream& kStream, int* paiImprovementArray, int iArraySize)
{
	kStream << iArraySize;

	for(int iI = 0; iI < iArraySize; iI++)
	{
		const ImprovementTypes eImprovement = static_cast<ImprovementTypes>(iI);
		CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
		if(pkImprovementInfo)
		{
			CvInfosSerializationHelper::WriteHashed(kStream, pkImprovementInfo);
			kStream << paiImprovementArray[iI];
		}
		else
		{
			kStream << (int)NO_IMPROVEMENT;
		}
	}
}


void ImprovementArrayHelpers::ReadYieldArray(FDataStream& kStream, int** ppaaiImprovementYieldArray, int iNumYields)
{
	int iNumEntries;

	kStream >> iNumEntries;

	for(int iI = 0; iI < iNumEntries; iI++)
	{
		int iHash;
		kStream >> iHash;
		if(iHash != (int)0)
		{
			int iType = GC.getInfoTypeForHash(iHash);
			if(iType != -1)
			{
				for(int jJ = 0; jJ < iNumYields; jJ++)
				{
					kStream >> ppaaiImprovementYieldArray[iType][jJ];
				}
			}
			else
			{
				CvString szError;
				szError.Format("LOAD ERROR: Improvement Type not found: %08x", iHash);
				GC.LogMessage(szError.GetCString());
				CvAssertMsg(false, szError);

				for(int jJ = 0; jJ < iNumYields; jJ++)
				{
					int iDummy;
					kStream >> iDummy;
				}
			}
		}
	}
}


void ImprovementArrayHelpers::WriteYieldArray(FDataStream& kStream, int** ppaaiImprovementYieldArray, int iArraySize)
{
	kStream << iArraySize;

	for(int iI = 0; iI < iArraySize; iI++)
	{
		const ImprovementTypes eImprovement = static_cast<ImprovementTypes>(iI);
		CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
		if(pkImprovementInfo)
		{
			CvInfosSerializationHelper::WriteHashed(kStream, pkImprovementInfo);
			for(int jJ = 0; jJ < NUM_YIELD_TYPES; jJ++)
			{
				kStream << ppaaiImprovementYieldArray[iI][jJ];
			}
		}
		else
		{
			kStream << (int)0;
		}
	}
}
