






#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreUtils.h"
#include "CvInfosSerializationHelper.h"

#include "LintFree.h"


CvUnitEntry::CvUnitEntry(void) :
	m_iProductionCost(0),
#ifdef NQ_UNIT_FINAL_PRODUCTION_COST_MODIFIER
	m_iFinalProductionCostModifier(0),
#endif
	m_iFaithCost(0),
	m_bRequiresFaithPurchaseEnabled(false),
	m_bPurchaseOnly(false),
	m_bMoveAfterPurchase(false),
	m_iHurryCostModifier(0),
	m_iAdvancedStartCost(0),
	m_iMinAreaSize(0),
	m_iMoves(0),
	m_bMoves(false),
	m_iBaseSightRange(0),
	m_iRange(0),
	m_iAirInterceptRange(0),
	m_iAirUnitCap(0),
	m_iNukeDamageLevel(0),
	m_iWorkRate(0),
	m_iNumFreeTechs(0),
	m_iBaseBeakersTurnsToCount(0),
	m_iBaseCultureTurnsToCount(0),
	m_iBaseHurry(0),
	m_iHurryMultiplier(0),
	m_bRushBuilding(false),
	m_iBaseGold(0),
	m_iNumGoldPerEra(0),
	m_bSpreadReligion(false),
	m_bRemoveHeresy(false),
	m_iReligionSpreads(0),
	m_iReligiousStrength(0),
	m_bFoundReligion(false),
	m_bRequiresEnhancedReligion(false),
	m_bProhibitsSpread(false),
	m_bCanBuyCityState(false),
	m_iCombat(0),
	m_iCombatLimit(0),
	m_iRangedCombat(0),
	m_iRangedCombatLimit(0),
	m_iXPValueAttack(0),
	m_iXPValueDefense(0),
	m_iSpecialCargo(0),
	m_iDomainCargo(0),
	m_iConscriptionValue(0),
	m_iExtraMaintenanceCost(0),
	m_bNoMaintenance(false),
	m_iUnhappiness(0),
	m_iUnitClassType(NO_UNITCLASS),
	m_iSpecialUnitType(NO_SPECIALUNIT),
	m_iUnitCaptureClassType(NO_UNITCLASS),
	m_iUnitCombatType(NO_UNITCOMBAT),
	m_iDomainType(NO_DOMAIN),
	m_iCivilianAttackPriority(NO_CIVILIAN_ATTACK_PRIORITY),
	m_iDefaultUnitAIType(NO_UNITAI),
	m_iPrereqPillageTech(NO_TECH),
	m_iPrereqAndTech(NO_TECH),
	m_iObsoleteTech(NO_TECH),
	m_iPolicyType(NO_POLICY),
#if defined(MISC_CHANGES)
	m_iPolicyBranchType(NO_POLICY_BRANCH_TYPE),
	m_bAnyIdeologyUnlock(false),
#endif
	m_iGoodyHutUpgradeUnitClass(NO_UNITCLASS),
	m_iGroupSize(0),
	m_iGroupDefinitions(0),
	m_iUnitMeleeWaveSize(0),
	m_iUnitRangedWaveSize(0),
	m_iNumUnitNames(0),
	m_iCommandType(NO_COMMAND),
	m_bFoodProduction(false),
	m_bNoBadGoodies(false),
	m_bRivalTerritory(false),
	m_bMilitarySupport(false),
	m_bMilitaryProduction(false),
	m_bPillage(false),
	m_bFound(false),
	m_bFoundAbroad(false),
	m_iCultureBombRadius(0),
	m_iGoldenAgeTurns(0),
	m_iFreePolicies(0),
	m_iOneShotTourism(0),
	m_iOneShotTourismPercentOthers(0),
	m_bIgnoreBuildingDefense(false),
	m_bPrereqResources(false),
	m_bMechanized(false),
	m_bSuicide(false),
	m_bCaptureWhileEmbarked(false),
	m_bRangeAttackOnlyInDomain(false),
	m_bTrade(false),
	m_iNumExoticGoods(0),
#if defined(LEKMOD_SUBMERGE_MISSION)
	m_bSubmerge(false),
#endif
	m_pbUpgradeUnitClass(NULL),
	m_pbUnitAIType(NULL),
	m_pbNotUnitAIType(NULL),
	m_pbBuilds(NULL),
	m_pbGreatPeoples(NULL),
	m_pbBuildings(NULL),
	m_pbBuildingClassRequireds(NULL),
	m_piPrereqAndTechs(NULL),
	m_piResourceQuantityRequirements(NULL),
	m_piProductionTraits(NULL),
	m_piFlavorValue(NULL),
	m_piUnitGroupRequired(NULL),
	m_pbFreePromotions(NULL),
	m_paszEarlyArtDefineTags(NULL),
	m_paszLateArtDefineTags(NULL),
	m_paszMiddleArtDefineTags(NULL),
	m_paszUnitNames(NULL),
	m_paeGreatWorks(NULL),
	m_piProductionModifierBuildings(NULL),
	m_piYieldFromKills(NULL),
	m_piYieldFromKillsMax(NULL),
#if defined(LEKMOD_UNIT_STRENGTH_PROMOTION_ERA)
	m_piEraStrengthChanges(NULL),
	m_piEraRangedStrengthChanges(NULL),
	m_piEraMovesChanges(NULL),
	m_piEraStartingExperienceChanges(NULL),
#endif
	m_iLeaderExperience(0),
	m_iProjectPrereq(0),
	m_iSpaceshipProject(0),
	m_iLeaderPromotion(0),
	m_iCachedPower(0),

	m_bUnitArtInfoEraVariation(false),
	m_bUnitArtInfoCulturalVariation(false),
	m_iUnitFlagIconOffset(0),
	m_iUnitPortraitOffset(0)
{
}


CvUnitEntry::~CvUnitEntry(void)
{
	SAFE_DELETE_ARRAY(m_pbUpgradeUnitClass);
	SAFE_DELETE_ARRAY(m_pbUnitAIType);
	SAFE_DELETE_ARRAY(m_pbNotUnitAIType);
	SAFE_DELETE_ARRAY(m_pbBuilds);
	SAFE_DELETE_ARRAY(m_pbGreatPeoples);
	SAFE_DELETE_ARRAY(m_pbBuildings);
	SAFE_DELETE_ARRAY(m_pbBuildingClassRequireds);
	SAFE_DELETE_ARRAY(m_piPrereqAndTechs);
	SAFE_DELETE_ARRAY(m_piResourceQuantityRequirements);
	SAFE_DELETE_ARRAY(m_piProductionTraits);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piUnitGroupRequired);
	SAFE_DELETE_ARRAY(m_pbFreePromotions);
	SAFE_DELETE_ARRAY(m_paszEarlyArtDefineTags);
	SAFE_DELETE_ARRAY(m_paszLateArtDefineTags);
	SAFE_DELETE_ARRAY(m_paszMiddleArtDefineTags);
	SAFE_DELETE_ARRAY(m_paszUnitNames);
	SAFE_DELETE_ARRAY(m_paeGreatWorks);
	SAFE_DELETE_ARRAY(m_piProductionModifierBuildings);
	SAFE_DELETE_ARRAY(m_piYieldFromKills);
	SAFE_DELETE_ARRAY(m_piYieldFromKillsMax);
#if defined(LEKMOD_UNIT_STRENGTH_PROMOTION_ERA)
	SAFE_DELETE_ARRAY(m_piEraStrengthChanges);
	SAFE_DELETE_ARRAY(m_piEraRangedStrengthChanges);
	SAFE_DELETE_ARRAY(m_piEraMovesChanges);
	SAFE_DELETE_ARRAY(m_piEraStartingExperienceChanges);
#endif
}

bool CvUnitEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;


	m_iProductionCost = kResults.GetInt("Cost");
#ifdef NQ_UNIT_FINAL_PRODUCTION_COST_MODIFIER
	m_iFinalProductionCostModifier = kResults.GetInt("FinalProductionCostModifier");
#endif
	m_iFaithCost = kResults.GetInt("FaithCost");
	m_bRequiresFaithPurchaseEnabled = kResults.GetBool("RequiresFaithPurchaseEnabled");
	m_bPurchaseOnly = kResults.GetBool("PurchaseOnly");
	m_bMoveAfterPurchase = kResults.GetBool("MoveAfterPurchase");
	m_iHurryCostModifier = kResults.GetInt("HurryCostModifier");
	m_iAdvancedStartCost = kResults.GetInt("AdvancedStartCost");
	m_iMinAreaSize = kResults.GetInt("MinAreaSize");
	m_iMoves = kResults.GetInt("Moves");
	m_bMoves = kResults.GetInt("Immobile");
	m_iBaseSightRange = kResults.GetInt("BaseSightRange");
	m_iRange = kResults.GetInt("Range");
	m_iAirInterceptRange = kResults.GetInt("AirInterceptRange");
	m_iAirUnitCap = kResults.GetInt("AirUnitCap");
	m_iNukeDamageLevel = kResults.GetInt("NukeDamageLevel");
	m_iWorkRate = kResults.GetInt("WorkRate");
	m_iNumFreeTechs = kResults.GetInt("NumFreeTechs");
	m_iBaseBeakersTurnsToCount = kResults.GetInt("BaseBeakersTurnsToCount");
	m_iBaseCultureTurnsToCount = kResults.GetInt("BaseCultureTurnsToCount");
	m_iBaseHurry = kResults.GetInt("BaseHurry");
	m_iHurryMultiplier = kResults.GetInt("HurryMultiplier");
	m_bRushBuilding= kResults.GetInt("RushBuilding");
	m_iBaseGold = kResults.GetInt("BaseGold");
	m_iNumGoldPerEra = kResults.GetInt("NumGoldPerEra");
	m_bSpreadReligion = kResults.GetBool("SpreadReligion");
	m_bRemoveHeresy = kResults.GetBool("RemoveHeresy");
	m_iReligionSpreads = kResults.GetInt("ReligionSpreads");
	m_iReligiousStrength = kResults.GetInt("ReligiousStrength");
	m_bFoundReligion = kResults.GetBool("FoundReligion");
	m_bRequiresEnhancedReligion = kResults.GetBool("RequiresEnhancedReligion");
	m_bProhibitsSpread = kResults.GetBool("ProhibitsSpread");
	m_bCanBuyCityState = kResults.GetBool("CanBuyCityState");
	m_iCombat = kResults.GetInt("Combat");
	m_iCombatLimit = kResults.GetInt("CombatLimit");
	m_iRangedCombat = kResults.GetInt("RangedCombat");
	m_iRangedCombatLimit = kResults.GetInt("RangedCombatLimit");
	m_iXPValueAttack = kResults.GetInt("XPValueAttack");
	m_iXPValueDefense = kResults.GetInt("XPValueDefense");
	m_iConscriptionValue = kResults.GetInt("Conscription");
	m_iExtraMaintenanceCost = kResults.GetInt("ExtraMaintenanceCost");
	m_bNoMaintenance = kResults.GetBool("NoMaintenance");
	m_iUnhappiness = kResults.GetInt("Unhappiness");
	m_iUnitFlagIconOffset = kResults.GetInt("UnitFlagIconOffset");
	m_iUnitPortraitOffset = kResults.GetInt("PortraitIndex");
	m_iLeaderExperience = kResults.GetInt("LeaderExperience");
	m_bFoodProduction = kResults.GetBool("Food");
	m_bNoBadGoodies = kResults.GetBool("NoBadGoodies");
	m_bRivalTerritory = kResults.GetBool("RivalTerritory");
	m_bMilitarySupport = kResults.GetBool("MilitarySupport");
	m_bMilitaryProduction = kResults.GetBool("MilitaryProduction");
	m_bPillage = kResults.GetBool("Pillage");
	m_bFound = kResults.GetBool("Found");
	m_bFoundAbroad = kResults.GetBool("FoundAbroad");
#ifdef LEKMOD_CUSTOM_SETTLERS
	m_iPopulationReq = kResults.GetInt("PopulationReq");
	m_iSettlerCostModifier = kResults.GetInt("SettlerCostModifier");
	m_iLocalPopChange = kResults.GetInt("LocalPopChange");
#endif
	m_iCultureBombRadius = kResults.GetInt("CultureBombRadius");
	m_iGoldenAgeTurns = kResults.GetInt("GoldenAgeTurns");
	m_iFreePolicies = kResults.GetInt("FreePolicies");
	m_iOneShotTourism = kResults.GetInt("OneShotTourism");
	m_iOneShotTourismPercentOthers = kResults.GetInt("OneShotTourismPercentOthers");
	m_bIgnoreBuildingDefense = kResults.GetBool("IgnoreBuildingDefense");
	m_bPrereqResources = kResults.GetBool("PrereqResources");
	m_bMechanized = kResults.GetBool("Mechanized");
	m_bSuicide = kResults.GetBool("Suicide");
	m_bCaptureWhileEmbarked = kResults.GetBool("CaptureWhileEmbarked");
	m_bRangeAttackOnlyInDomain = kResults.GetBool("RangeAttackOnlyInDomain");
	m_bTrade = kResults.GetBool("Trade");
	m_iNumExoticGoods = kResults.GetInt("NumExoticGoods");
#if defined(MISC_CHANGES)
	m_bAnyIdeologyUnlock = kResults.GetBool("AnyIdeologyUnlock");
#endif
#if defined(LEKMOD_SUBMERGE_MISSION)
	m_bSubmerge = kResults.GetBool("Submerge");
#endif

	m_strUnitArtInfoTag = kResults.GetText("UnitArtInfo");
	m_bUnitArtInfoCulturalVariation = kResults.GetBool("UnitArtInfoCulturalVariation");
	m_bUnitArtInfoEraVariation = kResults.GetBool("UnitArtInfoEraVariation");


	const char* szTextVal = NULL;
	szTextVal = kResults.GetText("Class");
	m_iUnitClassType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("Special");
	m_iSpecialUnitType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("Capture");
	m_iUnitCaptureClassType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("CombatClass");
	m_iUnitCombatType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("Domain");
	m_iDomainType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("CivilianAttackPriority");
	m_iCivilianAttackPriority = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("DefaultUnitAI");
	m_iDefaultUnitAIType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("PillagePrereqTech");
	m_iPrereqPillageTech = GC.getInfoTypeForString(szTextVal, true);
	
	szTextVal = kResults.GetText("PrereqTech");
	m_iPrereqAndTech = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("ObsoleteTech");
	m_iObsoleteTech = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("PolicyType");
	m_iPolicyType = GC.getInfoTypeForString(szTextVal, true);
#if defined(MISC_CHANGES)
	szTextVal = kResults.GetText("PolicyBranchType");
	m_iPolicyBranchType = GC.getInfoTypeForString(szTextVal, true);
#endif
	szTextVal = kResults.GetText("GoodyHutUpgradeUnitClass");
	m_iGoodyHutUpgradeUnitClass = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("SpecialCargo");
	m_iSpecialCargo = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("DomainCargo");
	m_iDomainCargo = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("ProjectPrereq");
	m_iProjectPrereq = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("SpaceshipProject");
	m_iSpaceshipProject = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("LeaderPromotion");
	m_iLeaderPromotion = GC.getInfoTypeForString(szTextVal, true);


	const char* szUnitType = GetType();

	kUtility.SetFlavors(m_piFlavorValue, "Unit_Flavors", "UnitType", szUnitType);

	kUtility.PopulateArrayByValue(m_piProductionTraits, "Traits", "Unit_ProductionTraits", "TraitType", "UnitType", szUnitType, "Trait");

	kUtility.PopulateArrayByValue(m_piResourceQuantityRequirements, "Resources", "Unit_ResourceQuantityRequirements", "ResourceType", "UnitType", szUnitType, "Cost");
	kUtility.PopulateArrayByValue(m_piProductionModifierBuildings, "Buildings", "Unit_ProductionModifierBuildings", "BuildingType", "UnitType", szUnitType, "ProductionModifier");
	{
		kUtility.InitializeArray(m_piYieldFromKills, "Yields", 0);
		kUtility.InitializeArray(m_piYieldFromKillsMax, "Yields", 0);
		std::string sqlKey = "Unit_YieldFromKills";
		Database::Results* pResults = kUtility.GetOrPrepareResults(sqlKey,
			"SELECT Yields.ID, Yield, COALESCE(Max, 0) "
			"FROM Unit_YieldFromKills "
			"INNER JOIN Yields ON Yields.Type = YieldType "
			"WHERE UnitType = ?");

		pResults->Bind(1, szUnitType);

		while (pResults->Step())
		{
			const int iYieldID = pResults->GetInt(0);
			m_piYieldFromKills[iYieldID] = pResults->GetInt(1);
			m_piYieldFromKillsMax[iYieldID] = pResults->GetInt(2);
		}
		pResults->Reset();
	}
	kUtility.PopulateArrayByExistence(m_pbFreePromotions, "UnitPromotions", "Unit_FreePromotions", "PromotionType", "UnitType", szUnitType);

	kUtility.PopulateArrayByExistence(m_pbUpgradeUnitClass, "UnitClasses", "Unit_ClassUpgrades", "UnitClassType", "UnitType", szUnitType);

	kUtility.PopulateArrayByExistence(m_pbUnitAIType, "UnitAIInfos", "Unit_AITypes", "UnitAIType", "UnitType", szUnitType);
	kUtility.PopulateArrayByExistence(m_pbNotUnitAIType, "UnitAIInfos", "Unit_NotAITypes", "UnitAIType", "UnitType", szUnitType);

	kUtility.PopulateArrayByExistence(m_pbBuilds, "Builds", "Unit_Builds", "BuildType", "UnitType", szUnitType);
	kUtility.PopulateArrayByExistence(m_pbGreatPeoples, "Specialists", "Unit_GreatPersons", "GreatPersonType", "UnitType", szUnitType);
	kUtility.PopulateArrayByExistence(m_pbBuildings, "Buildings", "Unit_Buildings", "BuildingType", "UnitType", szUnitType);
	kUtility.PopulateArrayByExistence(m_pbBuildingClassRequireds, "BuildingClasses", "Unit_BuildingClassRequireds", "BuildingClassType", "UnitType", szUnitType);
#if defined(LEKMOD_UNIT_STRENGTH_PROMOTION_ERA)
	{
		kUtility.InitializeArray(m_piEraStrengthChanges, "Eras", 0);
		kUtility.InitializeArray(m_piEraRangedStrengthChanges, "Eras", 0);
		kUtility.InitializeArray(m_piEraMovesChanges, "Eras", 0);
		kUtility.InitializeArray(m_piEraStartingExperienceChanges, "Eras", 0);
		std::string key = "Unit_EraStrengthChanges";
		Database::Results* results = kUtility.GetOrPrepareResults(key,
			"SELECT Eras.ID, StrengthChange, RangedStrengthChange, MovesChange, StartingExperienceChange FROM Unit_EraStrengthChanges "
			"INNER JOIN Eras ON EraType = Eras.Type "
			"WHERE UnitType = ?");
		results->Bind(1, szUnitType);
		while (results->Step())
		{
			const int iEra = results->GetInt(0);
			const int iStrengthChange = results->GetInt(1);
			const int iRangedStrengthChange = results->GetInt(2);
			const int iMovesChange = results->GetInt(3);
			const int iStartingExperienceChange = results->GetInt(4);

			m_piEraStrengthChanges[iEra] = iStrengthChange;
			m_piEraRangedStrengthChanges[iEra] = iRangedStrengthChange;
			m_piEraMovesChanges[iEra] = iMovesChange;
			m_piEraStartingExperienceChanges[iEra] = iStartingExperienceChange;
		}
		results->Reset();
	}
	{
		std::string key = "Unit_FreePromotionEras";
		Database::Results* results = kUtility.GetOrPrepareResults(key,
			"SELECT UnitPromotions.ID, Eras.ID FROM Unit_FreePromotionEras "
			"INNER JOIN UnitPromotions ON PromotionType = UnitPromotions.Type "
			"INNER JOIN Eras ON EraType = Eras.Type "
			"WHERE UnitType = ?");
			
		results->Bind(1, szUnitType);

		while (results->Step())
		{
			const int unitPromotionID = results->GetInt(0);
			const int eraID = results->GetInt(1);

			m_FreePromotionEras.insert(std::pair<int, int>(unitPromotionID, eraID));
		}

		results->Reset();


		std::multimap<int, int>(m_FreePromotionEras).swap(m_FreePromotionEras);
	}
#endif

	{

		kUtility.InitializeArray(m_piPrereqAndTechs, "Technologies", (int)NO_TECH);

		std::string strKey = "Units - TechTypes";
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Technologies.ID from Unit_TechTypes inner join Technologies on TechType = Technologies.Type where UnitType = ?");

		pResults->Bind(1, szUnitType, -1, false);

		int i = 0;
		while(pResults->Step())
		{
			m_piPrereqAndTechs[i++] = pResults->GetInt(0);
		}

		pResults->Reset();
	}


	{
		std::string strKey = "Units - UniqueNameCount";
		Database::Results* pUnitNameCount = kUtility.GetOrPrepareResults(strKey,
			"select count(*) from Unit_UniqueNames where UnitType = ?");

		pUnitNameCount->Bind(1, szUnitType, -1, false);

		pUnitNameCount->Step();
		m_iNumUnitNames = pUnitNameCount->GetInt(0);

		pUnitNameCount->Reset();
	}


	{
		if(m_iNumUnitNames > 0)
		{
#ifdef AUI_WARNING_FIXES
			SAFE_DELETE_ARRAY(m_paszUnitNames);
			SAFE_DELETE_ARRAY(m_paeGreatWorks);
#endif
			m_paszUnitNames = FNEW(CvString[m_iNumUnitNames], c_eCiv5GameplayDLL, 0);
			m_paeGreatWorks = FNEW(GreatWorkType[m_iNumUnitNames], c_eCiv5GameplayDLL, 0);

			std::string strKey = "Units - UniqueNames";
			Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
				"select UniqueName, GreatWorkType from Unit_UniqueNames where UnitType = ? ORDER BY rowid");

			pResults->Bind(1, szUnitType, -1, false);

			int i = 0;
			while(pResults->Step())
			{
				m_paszUnitNames[i] = pResults->GetText(0);
			
				const char* szGreatWorkType = pResults->GetText(1);
				if(szGreatWorkType == NULL)
				{
					m_paeGreatWorks[i] = NO_GREAT_WORK;
				}
				else
				{
					m_paeGreatWorks[i] = static_cast<GreatWorkType>(GC.getInfoTypeForString(szGreatWorkType, true));
				}

				i++;
			}

			pResults->Reset();
		}
	}


	{
		std::string strKey = "Units - MovementRates";
		Database::Results* pMovementRates = kUtility.GetOrPrepareResults(strKey,
			"SELECT * FROM MovementRates where Type = ? ORDER BY NumHexes");

		const char* szMovementRate = kResults.GetText("MoveRate");
		pMovementRates->Bind(1, szMovementRate, -1, false);
		while(pMovementRates->Step())
		{
			int f0 = pMovementRates->GetInt("NumHexes") - 1;
			float f1 = pMovementRates->GetFloat("TotalTime");
			float f2 = pMovementRates->GetFloat("EaseIn");
			float f3 = pMovementRates->GetFloat("EaseOut");
			float f4 = pMovementRates->GetFloat("IndividualOffset");
			float f5 = pMovementRates->GetFloat("RowOffset");
			float f6 = pMovementRates->GetFloat("CurveRoll");
			int   iPathSubdivision = pMovementRates->GetInt("PathSubdivision");
			if(f0 >= 0 && f0 <= 11)
			{
				m_unitMoveRate[f0].m_fTotalMoveRate    = f1;
				m_unitMoveRate[f0].m_fEaseIn           = f2;
				m_unitMoveRate[f0].m_fEaseOut          = f3;
				m_unitMoveRate[f0].m_fIndividualOffset = f4;
				m_unitMoveRate[f0].m_fRowOffset        = f5;
				m_unitMoveRate[f0].m_fCurveRoll        = f6;
				m_unitMoveRate[f0].m_iPathSubdivision  = iPathSubdivision;
			}
		}
		pMovementRates->Reset();

	}

	m_iCachedPower = DoUpdatePower(GetCombat(), GetRangedCombat());

	return true;
}


int CvUnitEntry::GetProductionCost() const
{
	return m_iProductionCost;
}

#ifdef NQ_UNIT_FINAL_PRODUCTION_COST_MODIFIER

int CvUnitEntry::GetFinalProductionCostModifier() const
{
	return m_iFinalProductionCostModifier;
}
#endif



int CvUnitEntry::GetFaithCost() const
{
	return m_iFaithCost;
}


bool CvUnitEntry::IsRequiresFaithPurchaseEnabled() const
{
	return m_bRequiresFaithPurchaseEnabled;
}


bool CvUnitEntry::IsPurchaseOnly() const
{
	return m_bPurchaseOnly;
}


bool CvUnitEntry::CanMoveAfterPurchase() const
{
	return m_bMoveAfterPurchase;
}


int CvUnitEntry::GetHurryCostModifier() const
{
	return m_iHurryCostModifier;
}


int CvUnitEntry::GetAdvancedStartCost() const
{
	return m_iAdvancedStartCost;
}


int CvUnitEntry::GetMinAreaSize() const
{
	return m_iMinAreaSize;
}


int CvUnitEntry::GetMoves() const
{
	return m_iMoves;
}


bool CvUnitEntry::IsImmobile() const
{
	return m_bMoves;
}


int CvUnitEntry::GetBaseSightRange() const
{
	return m_iBaseSightRange;
}


int CvUnitEntry::GetRange() const
{
	return m_iRange;
}


int CvUnitEntry::GetAirInterceptRange() const
{
	return m_iAirInterceptRange;
}


int CvUnitEntry::GetAirUnitCap() const
{
	return m_iAirUnitCap;
}


int CvUnitEntry::GetNukeDamageLevel() const
{
	return m_iNukeDamageLevel;
}


int CvUnitEntry::GetWorkRate() const
{
	return m_iWorkRate;
}


int CvUnitEntry::GetNumFreeTechs() const
{
	return m_iNumFreeTechs;
}


int CvUnitEntry::GetBaseBeakersTurnsToCount() const
{
	return m_iBaseBeakersTurnsToCount;
}


int CvUnitEntry::GetBaseCultureTurnsToCount() const
{
	return m_iBaseCultureTurnsToCount;
}


int CvUnitEntry::GetBaseHurry() const
{
	return m_iBaseHurry;
}


int CvUnitEntry::GetHurryMultiplier() const
{
	return m_iHurryMultiplier;
}


bool CvUnitEntry::IsRushBuilding() const
{
	return m_bRushBuilding;
}


int CvUnitEntry::GetBaseGold() const
{
	return m_iBaseGold;
}


int CvUnitEntry::GetNumGoldPerEra() const
{
	return m_iNumGoldPerEra;
}


bool CvUnitEntry::IsSpreadReligion() const
{
	return m_bSpreadReligion;
}


bool CvUnitEntry::IsRemoveHeresy() const
{
	return m_bRemoveHeresy;
}


int CvUnitEntry::GetReligionSpreads() const
{
	return m_iReligionSpreads;
}


int CvUnitEntry::GetReligiousStrength() const
{
	return m_iReligiousStrength;
}


bool CvUnitEntry::IsFoundReligion() const
{
	return m_bFoundReligion;
}


bool CvUnitEntry::IsRequiresEnhancedReligion() const
{
	return m_bRequiresEnhancedReligion;
}


bool CvUnitEntry::IsProhibitsSpread() const
{
	return m_bProhibitsSpread;
}


bool CvUnitEntry::IsCanBuyCityState() const
{
	return m_bCanBuyCityState;
}


int CvUnitEntry::GetCombat() const
{
	return m_iCombat;
}


void CvUnitEntry::SetCombat(int iNum)
{
	m_iCombat = iNum;
}


int CvUnitEntry::GetCombatLimit() const
{
	return m_iCombatLimit;
}


int CvUnitEntry::GetRangedCombat() const
{
	return m_iRangedCombat;
}


int CvUnitEntry::GetRangedCombatLimit() const
{
	return m_iRangedCombatLimit;
}


int CvUnitEntry::GetXPValueAttack() const
{
	return m_iXPValueAttack;
}


int CvUnitEntry::GetXPValueDefense() const
{
	return m_iXPValueDefense;
}


int CvUnitEntry::GetSpecialCargo() const
{
	return m_iSpecialCargo;
}


int CvUnitEntry::GetDomainCargo() const
{
	return m_iDomainCargo;
}


int CvUnitEntry::GetConscriptionValue() const
{
	return m_iConscriptionValue;
}


int CvUnitEntry::GetExtraMaintenanceCost() const
{
	return m_iExtraMaintenanceCost;
}


bool CvUnitEntry::IsNoMaintenance() const
{
	return m_bNoMaintenance;
}


int CvUnitEntry::GetUnhappiness() const
{
	return m_iUnhappiness;
}


int CvUnitEntry::GetUnitClassType() const
{
	return m_iUnitClassType;
}


int CvUnitEntry::GetSpecialUnitType() const
{
	return m_iSpecialUnitType;
}


int CvUnitEntry::GetUnitCaptureClassType() const
{
	return m_iUnitCaptureClassType;
}


int CvUnitEntry::GetUnitCombatType() const
{
	return m_iUnitCombatType;
}


int CvUnitEntry::GetDomainType() const
{
	return m_iDomainType;
}


int CvUnitEntry::GetCivilianAttackPriority() const
{
	return m_iCivilianAttackPriority;
}


int CvUnitEntry::GetDefaultUnitAIType() const
{
	return m_iDefaultUnitAIType;
}


int CvUnitEntry::GetPrereqPillageTech() const
{
	return m_iPrereqPillageTech;
}


int CvUnitEntry::GetPrereqAndTech() const
{
	return m_iPrereqAndTech;
}


int CvUnitEntry::GetObsoleteTech() const
{
	return m_iObsoleteTech;
}


int CvUnitEntry::GetEra() const
{
	TechTypes eTech = (TechTypes)GetPrereqAndTech();
	if (eTech != NO_TECH)
	{
		CvTechEntry* pTech = GC.getTechInfo((TechTypes)GetPrereqAndTech());
		return pTech->GetEra();
	}

	return -1;
}


int CvUnitEntry::GetPolicyType() const
{
	return m_iPolicyType;
}
#if defined(MISC_CHANGES)

int CvUnitEntry::GetPolicyBranchType() const
{
	return m_iPolicyBranchType;
}

bool CvUnitEntry::IsAnyIdeologyUnlock() const
{
	return m_bAnyIdeologyUnlock;
}
#endif

int CvUnitEntry::GetGoodyHutUpgradeUnitClass() const
{
	return m_iGoodyHutUpgradeUnitClass;
}


int CvUnitEntry::GetGroupSize() const
{
	return m_iGroupSize;
}


int CvUnitEntry::GetGroupDefinitions() const
{
	return m_iGroupDefinitions;
}


int CvUnitEntry::GetMeleeWaveSize() const
{
	return m_iUnitMeleeWaveSize;
}


int CvUnitEntry::GetRangedWaveSize() const
{
	return m_iUnitRangedWaveSize;
}


int CvUnitEntry::GetNumUnitNames() const
{
	return m_iNumUnitNames;
}


bool CvUnitEntry::IsFoodProduction() const
{
	return m_bFoodProduction;
}


bool CvUnitEntry::IsNoBadGoodies() const
{
	return m_bNoBadGoodies;
}


bool CvUnitEntry::IsRivalTerritory() const
{
	return m_bRivalTerritory;
}


bool CvUnitEntry::IsMilitarySupport() const
{
	return m_bMilitarySupport;
}


bool CvUnitEntry::IsMilitaryProduction() const
{
	return m_bMilitaryProduction;
}


bool CvUnitEntry::IsPillage() const
{
	return m_bPillage;
}


bool CvUnitEntry::IsFound() const
{
	return m_bFound;
}


bool CvUnitEntry::IsFoundAbroad() const
{
	return m_bFoundAbroad;
}

#ifdef LEKMOD_CUSTOM_SETTLERS

int CvUnitEntry::GetPopulationReq() const
{
	return m_iPopulationReq;
}


int CvUnitEntry::GetSettlerCostModifier() const
{
	return m_iSettlerCostModifier;
}


int CvUnitEntry::GetLocalPopChange() const
{
	return m_iLocalPopChange;
}
#endif


int CvUnitEntry::GetCultureBombRadius() const
{
	return m_iCultureBombRadius;
}


int CvUnitEntry::GetGoldenAgeTurns() const
{
	return m_iGoldenAgeTurns;
}


int CvUnitEntry::GetFreePolicies() const
{
	return m_iFreePolicies;
}


int CvUnitEntry::GetOneShotTourism() const
{
	return m_iOneShotTourism;
}


int CvUnitEntry::GetOneShotTourismPercentOthers() const
{
	return m_iOneShotTourismPercentOthers;
}


bool CvUnitEntry::IsIgnoreBuildingDefense() const
{
	return m_bIgnoreBuildingDefense;
}


bool CvUnitEntry::IsPrereqResources() const
{
	return m_bPrereqResources;
}


bool CvUnitEntry::IsMechUnit() const
{
	return m_bMechanized;
}


bool CvUnitEntry::IsSuicide() const
{
	return m_bSuicide;
}


bool CvUnitEntry::IsCaptureWhileEmbarked() const
{
	return m_bCaptureWhileEmbarked;
}


bool CvUnitEntry::IsRangeAttackOnlyInDomain() const
{
	return m_bRangeAttackOnlyInDomain;
}


bool CvUnitEntry::IsTrade() const
{
	return m_bTrade;
}


int CvUnitEntry::GetNumExoticGoods() const
{
	return m_iNumExoticGoods;
}
#if defined(LEKMOD_SUBMERGE_MISSION)

bool CvUnitEntry::IsSubmerge() const
{
	return m_bSubmerge;
}
#endif

int CvUnitEntry::GetCommandType() const
{
	return m_iCommandType;
}


void CvUnitEntry::SetCommandType(int iNewType)
{
	m_iCommandType = iNewType;
}




int CvUnitEntry::GetPrereqAndTechs(int i) const
{
	CvAssertMsg(i < GC.getNUM_UNIT_AND_TECH_PREREQS(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piPrereqAndTechs ? m_piPrereqAndTechs[i] : -1;
}


int CvUnitEntry::GetResourceQuantityRequirement(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piResourceQuantityRequirements ? m_piResourceQuantityRequirements[i] : -1;
}


int CvUnitEntry::GetBuildingProductionModifier(BuildingTypes eBuilding) const
{
	CvAssertMsg((int)eBuilding < GC.getNumBuildingInfos(), "Building type out of bounds");
	CvAssertMsg((int)eBuilding > -1, "Index out of bounds");
	return m_piProductionModifierBuildings[(int)eBuilding];
}


int CvUnitEntry::GetYieldFromKills(YieldTypes eYield) const
{
	CvAssertMsg((int)eYield < NUM_YIELD_TYPES, "Yield type out of bounds");
	CvAssertMsg((int)eYield > -1, "Index out of bounds");
	return m_piYieldFromKills ? m_piYieldFromKills[(int)eYield] : 0;
}


int CvUnitEntry::GetYieldFromKillsMax(YieldTypes eYield) const
{
	CvAssertMsg((int)eYield < NUM_YIELD_TYPES, "Yield type out of bounds");
	CvAssertMsg((int)eYield > -1, "Index out of bounds");
	return m_piYieldFromKillsMax ? m_piYieldFromKillsMax[(int)eYield] : 0;
}


int CvUnitEntry::GetProductionTraits(int i) const
{
	CvAssertMsg(i < GC.getNumTraitInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piProductionTraits ? m_piProductionTraits[i] : -1;
}


int CvUnitEntry::GetFlavorValue(int i) const
{
	CvAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}

int CvUnitEntry::GetUnitGroupRequired(int i) const
{
	CvAssertMsg(i < GetGroupDefinitions(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piUnitGroupRequired ? m_piUnitGroupRequired[i] : NULL;
}


bool CvUnitEntry::GetUpgradeUnitClass(int i) const
{
	CvAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbUpgradeUnitClass ? m_pbUpgradeUnitClass[i] : false;
}


bool CvUnitEntry::GetUnitAIType(int i) const
{
	CvAssertMsg(i < NUM_UNITAI_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbUnitAIType ? m_pbUnitAIType[i] : false;
}


bool CvUnitEntry::GetNotUnitAIType(int i) const
{
	CvAssertMsg(i < NUM_UNITAI_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbNotUnitAIType ? m_pbNotUnitAIType[i] : false;
}


bool CvUnitEntry::GetBuilds(int i) const
{
	CvAssertMsg(i < GC.getNumBuildInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbBuilds ? m_pbBuilds[i] : false;
}


bool CvUnitEntry::GetGreatPeoples(int i) const
{
	CvAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbGreatPeoples ? m_pbGreatPeoples[i] : false;
}


bool CvUnitEntry::GetBuildings(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbBuildings ? m_pbBuildings[i] : false;
}


bool CvUnitEntry::GetBuildingClassRequireds(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbBuildingClassRequireds ? m_pbBuildingClassRequireds[i] : false;
}


bool CvUnitEntry::GetFreePromotions(int i) const
{
	CvAssertMsg(i < GC.getNumPromotionInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbFreePromotions ? m_pbFreePromotions[i] : false;
}

#if defined(LEKMOD_UNIT_STRENGTH_PROMOTION_ERA)
bool CvUnitEntry::IsFreePromotionEra(int iPromotion, int iEra) const
{
	std::multimap<int, int>::const_iterator it = m_FreePromotionEras.find(iPromotion);
	if (it != m_FreePromotionEras.end())
	{

		std::multimap<int, int>::const_iterator lastElement = m_FreePromotionEras.upper_bound(iPromotion);

		for (; it != lastElement; ++it)
		{
			if (it->second == iEra)
			{
				return true;
			}
		}
	}

	return false;
}
#endif

int CvUnitEntry::GetProjectPrereq() const
{
	return m_iProjectPrereq;
}


int CvUnitEntry::GetSpaceshipProject() const
{
	return m_iSpaceshipProject;
}


int CvUnitEntry::GetLeaderPromotion() const
{
	return m_iLeaderPromotion;
}


int CvUnitEntry::GetLeaderExperience() const
{
	return m_iLeaderExperience;
}


const char* CvUnitEntry::GetUnitArtInfoTag() const
{
	return m_strUnitArtInfoTag;
}


const bool CvUnitEntry::GetUnitArtInfoCulturalVariation() const
{
	return m_bUnitArtInfoCulturalVariation;
}



const bool CvUnitEntry::GetUnitArtInfoEraVariation() const
{
	return m_bUnitArtInfoEraVariation;
}



const char* CvUnitEntry::GetUnitNames(int i) const
{
	CvAssertMsg(i < GetNumUnitNames(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
#if defined(LEKMOD_MACOS)
	return (m_paszUnitNames) ? m_paszUnitNames[i].c_str() : NULL;
#else
	return (m_paszUnitNames) ? m_paszUnitNames[i] : NULL;
#endif
}


GreatWorkType CvUnitEntry::GetGreatWorks(int i) const
{
	CvAssertMsg(i < GetNumUnitNames(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return (m_paeGreatWorks) ? m_paeGreatWorks[i] : NO_GREAT_WORK;
}


int CvUnitEntry::GetUnitFlagIconOffset() const
{
	return m_iUnitFlagIconOffset;
}



int CvUnitEntry::GetUnitPortraitOffset() const
{
	return m_iUnitPortraitOffset;
}





int CvUnitEntry::GetCargoSpace() const
{
	int rtnValue = 0;

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
			if(GetFreePromotions(iLoop))
			{
				const int iVal = pkPromotionInfo->GetCargoChange();
				rtnValue += iVal;
			}
		}
	}

	return rtnValue;
}


int CvUnitEntry::GetPower() const
{
	return m_iCachedPower;
}


int CvUnitEntry::DoUpdatePower(int iMeleeStrength, int iRangedStrength) const
{
	int iPower;






	iPower = int(pow((double)iMeleeStrength, 1.5));


	int iRangedPower = int(pow((double) iRangedStrength, 1.45));


	if(GetDomainType() == DOMAIN_SEA)
	{
		iRangedPower *= 3;
		iRangedPower /= 4;
	}

	if(iRangedPower > iPower)
	{
		iPower = iRangedPower;
	}


	iPower = int((float) iPower * pow(min(1.0,(double) GetMoves()), 0.3));






	if(IsSuicide())
	{
		iPower /= 2;
	}


	if(GetNukeDamageLevel() > 0)
	{
		iPower += 4000;
	}





	int iTemp;
#ifdef AUI_WARNING_FIXES
	uint iLoop;

	for (uint iPromotionLoop = 0; iPromotionLoop < GC.getNumPromotionInfos(); iPromotionLoop++)
#else
	int iLoop;

	for(int iPromotionLoop = 0; iPromotionLoop < GC.getNumPromotionInfos(); iPromotionLoop++)
#endif
	{
		CvPromotionEntry* kPromotion = GC.getPromotionInfo((PromotionTypes)iPromotionLoop);
		if(kPromotion == NULL)
			continue;

		if(GetFreePromotions(iPromotionLoop))
		{

			if(kPromotion->GetCityAttackPercent() > 0)
			{
				iTemp = (iPower * kPromotion->GetCityAttackPercent() / 2);
				iTemp /= 100;
				iPower += iTemp;
			}


			if(kPromotion->GetAttackMod() > 0)
			{
				iTemp = (iPower * kPromotion->GetAttackMod() / 2);
				iTemp /= 100;
				iPower += iTemp;
			}


			if(kPromotion->GetDefenseMod() > 0)
			{
				iTemp = (iPower * kPromotion->GetDefenseMod() / 2);
				iTemp /= 100;
				iPower += iTemp;
			}


			if(kPromotion->GetDropRange() > 0)
			{
				iTemp = iPower;
				iTemp /= 4;
				iPower += iTemp;
			}


			if(kPromotion->IsBlitz())
			{
				iTemp = iPower;
				iTemp /= 5;
				iPower += iTemp;
			}


			if(kPromotion->IsMustSetUpToRangedAttack())
			{
				iTemp = iPower;
				iTemp /= 5;
				iPower -= iTemp;
			}


			if(kPromotion->IsOnlyDefensive() && GetRangedCombat() == 0)
			{
				iTemp = iPower;
				iTemp /= 4;
				iPower -= iTemp;
			}

			for(iLoop = 0; iLoop < GC.getNumTerrainInfos(); iLoop++)
			{

				if(kPromotion->GetTerrainAttackPercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetTerrainAttackPercent(iLoop) / 4);
					iTemp /= 100;
					iPower += iTemp;
				}

				if(kPromotion->GetTerrainDefensePercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetTerrainDefensePercent(iLoop) / 4);
					iTemp /= 100;
					iPower += iTemp;
				}
			}

			for(iLoop = 0; iLoop < GC.getNumFeatureInfos(); iLoop++)
			{

				if(kPromotion->GetFeatureAttackPercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetFeatureAttackPercent(iLoop) / 4);
					iTemp /= 100;
					iPower += iTemp;
				}

				if(kPromotion->GetFeatureDefensePercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetFeatureDefensePercent(iLoop) / 4);
					iTemp /= 100;
					iPower += iTemp;
				}
			}

			for(iLoop = 0; iLoop < GC.getNumUnitCombatClassInfos(); iLoop++)
			{

				if(kPromotion->GetUnitCombatModifierPercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetUnitCombatModifierPercent(iLoop) / 4);
					iTemp /= 100;
					iPower += iTemp;
				}
			}

			for(iLoop = 0; iLoop < GC.getNumUnitClassInfos(); iLoop++)
			{


				if(kPromotion->GetUnitClassModifierPercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetUnitClassModifierPercent(iLoop) / 8);
					iTemp /= 100;
					iPower += iTemp;
				}

				if(kPromotion->GetUnitClassAttackModifier(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetUnitClassAttackModifier(iLoop) / 10);
					iTemp /= 100;
					iPower += iTemp;
				}

				if(kPromotion->GetUnitClassDefenseModifier(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetUnitClassDefenseModifier(iLoop) / 10);
					iTemp /= 100;
					iPower += iTemp;
				}
			}

			for(iLoop = 0; iLoop < NUM_DOMAIN_TYPES; iLoop++)
			{

				if(kPromotion->GetDomainModifierPercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetDomainModifierPercent(iLoop) / 4);
					iTemp /= 100;
					iPower += iTemp;
				}
#if defined(LEKMOD_DOMAIN_PROMO_ATTACK_DEFENSE)

				if(kPromotion->GetDomainAttackPercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetDomainAttackPercent(iLoop) / 4);
					iTemp /= 100;
					iPower += iTemp;
				}
				if(kPromotion->GetDomainDefensePercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetDomainDefensePercent(iLoop) / 4);
					iTemp /= 100;
					iPower += iTemp;
				}
#endif
			}
		}
	}






	return iPower;
}

UnitMoveRate CvUnitEntry::GetMoveRate(int numHexes) const
{
	if(numHexes < 0)
		numHexes = 0;
	if(numHexes > 11)
		numHexes = 11;
	return m_unitMoveRate[numHexes];
}





CvUnitXMLEntries::CvUnitXMLEntries(void)
{

}


CvUnitXMLEntries::~CvUnitXMLEntries(void)
{
	DeleteArray();
}


std::vector<CvUnitEntry*>& CvUnitXMLEntries::GetUnitEntries()
{
	return m_paUnitEntries;
}


#ifdef AUI_WARNING_FIXES
uint CvUnitXMLEntries::GetNumUnits() const
#else
int CvUnitXMLEntries::GetNumUnits()
#endif
{
	return m_paUnitEntries.size();
}



void CvUnitXMLEntries::DeleteArray()
{
	for(std::vector<CvUnitEntry*>::iterator it = m_paUnitEntries.begin(); it != m_paUnitEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paUnitEntries.clear();
}


#ifdef AUI_WARNING_FIXES
_Ret_maybenull_ CvUnitEntry* CvUnitXMLEntries::GetEntry(uint index)
#else
CvUnitEntry* CvUnitXMLEntries::GetEntry(int index)
#endif
{
	return m_paUnitEntries[index];
}


void UnitArrayHelpers::Read(FDataStream& kStream, int* paiUnitArray)
{
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, paiUnitArray, GC.getNumUnitInfos());
}


void UnitArrayHelpers::Write(FDataStream& kStream, int* paiUnitArray, int iArraySize)
{
	CvInfosSerializationHelper::WriteHashedDataArray<UnitTypes, int>(kStream, paiUnitArray, iArraySize);
}
