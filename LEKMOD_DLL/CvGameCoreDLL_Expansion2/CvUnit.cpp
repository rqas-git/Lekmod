







#include "CvGameCoreDLLPCH.h"
#include "CvUnit.h"
#include "CvArea.h" 
#include "CvPlot.h"
#include "CvCity.h"
#include "CvGlobals.h"
#include "CvGameCoreUtils.h"
#include "CvInternalGameCoreUtils.h"
#include "CvMap.h"
#include "CvPlayerAI.h"
#include "CvRandom.h"
#include "CvTeam.h"
#include "CvGameCoreUtils.h"
#include "ICvDLLUserInterface.h"
#include "CvInfos.h"
#include "CvImprovementClasses.h"
#include "CvAStar.h"
#include "CvMinorCivAI.h"
#include "CvDiplomacyAI.h"
#include "CvEnumSerialization.h"
#include "CvNotifications.h"
#include "FMemoryStream.h"
#include <set>
#include "FStlContainerSerialization.h"
#include "CvUnitMission.h"
#include "CvUnitCombat.h"
#include "CvEconomicAI.h"
#include "CvAchievementUnlocker.h"
#include "CvMilitaryAI.h"
#include "CvTypes.h"

#include "CvDllPlot.h"
#include "CvDllUnit.h"
#include "CvDllCity.h"
#include "CvGameQueries.h"
#include "CvBarbarians.h"

#if !defined(FINAL_RELEASE)
#include <sstream>




#endif

#if defined(LOG_UNIT_MOVES)
#define LOG_UNIT_MOVES_MESSAGE(x)		gDLL->netMessageDebugLog(x)
#define LOG_UNIT_MOVES_MESSAGE_OSTR(x)	{ std::ostringstream str; str << x; gDLL->netMessageDebugLog(str.str()); }
#else
#define LOG_UNIT_MOVES_MESSAGE(x)		((void)0)
#define LOG_UNIT_MOVES_MESSAGE_OSTR(x)	((void)0)
#endif


#include "LintFree.h"

namespace FSerialization
{
std::set<CvUnit*> unitsToCheck;
void SyncUnits()
{
	if(GC.getGame().isNetworkMultiPlayer())
	{
		PlayerTypes authoritativePlayer = GC.getGame().getActivePlayer();

		std::set<CvUnit*>::const_iterator i;
		for(i = unitsToCheck.begin(); i != unitsToCheck.end(); ++i)
		{
			const CvUnit* unit = *i;

			if(unit)
			{
				const CvPlayer& player = GET_PLAYER(unit->getOwner());
				if(unit->getOwner() == authoritativePlayer || (gDLL->IsHost() && !player.isHuman() && player.isAlive()))
				{
					const FAutoArchive& archive = unit->getSyncArchive();
					if(archive.hasDeltas())
					{
						FMemoryStream memoryStream;
						std::vector<std::pair<std::string, std::string> > callStacks;
						archive.saveDelta(memoryStream, callStacks);
						gDLL->sendUnitSyncCheck(unit->getOwner(), unit->GetID(), memoryStream, callStacks);
					}
				}
			}
		}
	}
}



void ClearUnitDeltas()
{
	std::set<CvUnit*>::iterator i;
	for(i = unitsToCheck.begin(); i != unitsToCheck.end(); ++i)
	{
		CvUnit* unit = *i;

		if(unit)
		{
			FAutoArchive& archive = unit->getSyncArchive();
			archive.clearDelta();
		}
	}
}
}

bool s_dispatchingNetMessage = false;

OBJECT_VALIDATE_DEFINITION(CvUnit)



CvUnit::CvUnit() :
	m_syncArchive(*this)
	, m_iID("CvUnit::m_iID", m_syncArchive)
	, m_iHotKeyNumber("CvUnit::m_iHotKeyNumber", m_syncArchive)
	, m_iX("CvUnit::m_iX", m_syncArchive, true)
	, m_iY("CvUnit::m_iY", m_syncArchive, true)
	, m_iLastMoveTurn(0)
	, m_iCycleOrder(-1)
	, m_iDeployFromOperationTurn("CvUnit::DeployFromOperationTurn", m_syncArchive)
#if defined(UNITS_REMEMBER_HOME)
	, m_iHomeCity("CvUnit::m_iHomeCity", m_syncArchive)
#endif
	, m_iReconX("CvUnit::m_iReconX", m_syncArchive)
	, m_iReconY("CvUnit::m_iReconY", m_syncArchive)
	, m_iReconCount("CvUnit::m_iReconCount", m_syncArchive)
	, m_iGameTurnCreated("CvUnit::m_iGameTurnCreated", m_syncArchive)
	, m_iDamage("CvUnit::m_iDamage", m_syncArchive, true)
	, m_iMoves("CvUnit::m_iMoves", m_syncArchive, true)
	, m_bImmobile("CvUnit::m_bImmobile", m_syncArchive)
	, m_iExperience("CvUnit::m_iExperience", m_syncArchive)
	, m_iLevel("CvUnit::m_iLevel", m_syncArchive)
	, m_iCargo("CvUnit::m_iCargo", m_syncArchive)
	, m_iCargoCapacity("CvUnit::m_iCargoCapacity", m_syncArchive)
	, m_iAttackPlotX("CvUnit::m_iAttackPlotX", m_syncArchive)
	, m_iAttackPlotY("CvUnit::m_iAttackPlotY", m_syncArchive)
	, m_iCombatTimer("CvUnit::m_iCombatTimer", m_syncArchive)
	, m_iCombatFirstStrikes("CvUnit::m_iCombatFirstStrikes", m_syncArchive)
	, m_iCombatDamage("CvUnit::m_iCombatDamage", m_syncArchive)
	, m_iFortifyTurns("CvUnit::m_iFortifyTurns", m_syncArchive, true)
	, m_bFortifiedThisTurn("CvUnit::m_bFortifiedThisTurn", m_syncArchive)
	, m_iBlitzCount("CvUnit::m_iBlitzCount", m_syncArchive)
	, m_iAmphibCount("CvUnit::m_iAmphibCount", m_syncArchive)
	, m_iRiverCrossingNoPenaltyCount("CvUnit::m_iRiverCrossingNoPenaltyCount", m_syncArchive)
	, m_iEnemyRouteCount("CvUnit::m_iEnemyRouteCount", m_syncArchive)
	, m_iRivalTerritoryCount("CvUnit::m_iRivalTerritoryCount", m_syncArchive)
	, m_iMustSetUpToRangedAttackCount("CvUnit::m_iMustSetUpToRangedAttackCount", m_syncArchive)
	, m_iRangeAttackIgnoreLOSCount("CvUnit::m_iRangeAttackIgnoreLOSCount", m_syncArchive)
	, m_iCityAttackOnlyCount(0)
	, m_iCaptureDefeatedEnemyCount(0)
	, m_iRangedSupportFireCount("CvUnit::m_iRangedSupportFireCount", m_syncArchive)
	, m_iAlwaysHealCount("CvUnit::m_iAlwaysHealCount", m_syncArchive)
	, m_iHealOutsideFriendlyCount("CvUnit::m_iHealOutsideFriendlyCount", m_syncArchive)
	, m_iHillsDoubleMoveCount("CvUnit::m_iHillsDoubleMoveCount", m_syncArchive)
	, m_iImmuneToFirstStrikesCount("CvUnit::m_iImmuneToFirstStrikesCount", m_syncArchive)
	, m_iExtraVisibilityRange("CvUnit::m_iExtraVisibilityRange", m_syncArchive)
	, m_iExtraMoves("CvUnit::m_iExtraMoves", m_syncArchive)
	, m_iExtraMoveDiscount("CvUnit::m_iExtraMoveDiscount", m_syncArchive)
	, m_iExtraRange("CvUnit::m_iExtraRange", m_syncArchive)
	, m_iExtraIntercept("CvUnit::m_iExtraIntercept", m_syncArchive)
	, m_iExtraEvasion("CvUnit::m_iExtraEvasion", m_syncArchive)
	, m_iExtraFirstStrikes("CvUnit::m_iExtraFirstStrikes", m_syncArchive)
	, m_iExtraChanceFirstStrikes("CvUnit::m_iExtraChanceFirstStrikes", m_syncArchive)
	, m_iExtraWithdrawal("CvUnit::m_iExtraWithdrawal", m_syncArchive)
	, m_iExtraEnemyHeal("CvUnit::m_iExtraEnemyHeal", m_syncArchive)
	, m_iExtraNeutralHeal("CvUnit::m_iExtraNeutralHeal", m_syncArchive)
	, m_iExtraFriendlyHeal("CvUnit::m_iExtraFriendlyHeal", m_syncArchive)
	, m_iSameTileHeal("CvUnit::m_iSameTileHeal", m_syncArchive)
	, m_iAdjacentTileHeal("CvUnit::m_iAdjacentTileHeal", m_syncArchive)
	, m_iEnemyDamageChance(0)
	, m_iNeutralDamageChance(0)
	, m_iEnemyDamage(0)
	, m_iNeutralDamage(0)
	, m_iNearbyEnemyCombatMod(0)
	, m_iNearbyEnemyCombatRange(0)
	, m_iSapperCount(0)
	, m_iCanHeavyCharge(0)
#ifdef NQ_HEAVY_CHARGE_DOWNHILL
	, m_iHeavyChargeDownhill(0)
#endif
#ifdef LEKMOD_DIFFERENT_IDEO_COMBAT_BONUS
	, m_iCombatBonusVsDifferentIdeologyModifier(0)
#endif
	, m_iNumExoticGoods(0)
	, m_iAdjacentModifier("CvUnit::m_iAdjacentModifier", m_syncArchive)
	, m_iRangedAttackModifier("CvUnit::m_iRangedAttackModifier", m_syncArchive)
	, m_iInterceptionCombatModifier("CvUnit::m_iInterceptionCombatModifier", m_syncArchive)
	, m_iInterceptionDefenseDamageModifier("CvUnit::m_iInterceptionDefenseDamageModifier", m_syncArchive)
	, m_iAirSweepCombatModifier("CvUnit::m_iAirSweepCombatModifier", m_syncArchive)
	, m_iAttackModifier("CvUnit::m_iAttackModifier", m_syncArchive)
	, m_iDefenseModifier("CvUnit::m_iDefenseModifier", m_syncArchive)
	, m_iExtraCombatPercent("CvUnit::m_iExtraCombatPercent", m_syncArchive)
	, m_iExtraCityAttackPercent("CvUnit::m_iExtraCityAttackPercent", m_syncArchive)
	, m_iExtraCityDefensePercent("CvUnit::m_iExtraCityDefensePercent", m_syncArchive)
	, m_iExtraRangedDefenseModifier("CvUnit::m_iExtraRangedDefenseModifier", m_syncArchive)
	, m_iExtraHillsAttackPercent("CvUnit::m_iExtraHillsAttackPercent", m_syncArchive)
	, m_iExtraHillsDefensePercent("CvUnit::m_iExtraHillsDefensePercent", m_syncArchive)
	, m_iExtraOpenAttackPercent("CvUnit::m_iExtraOpenAttackPercent", m_syncArchive)
	, m_iExtraOpenRangedAttackMod("CvUnit::m_iExtraOpenRangedAttackMod", m_syncArchive)
	, m_iExtraRoughAttackPercent("CvUnit::m_iExtraRoughAttackPercent", m_syncArchive)
	, m_iExtraRoughRangedAttackMod("CvUnit::m_iExtraRoughRangedAttackMod", m_syncArchive)
	, m_iExtraAttackFortifiedMod("CvUnit::m_iExtraAttackFortifiedMod", m_syncArchive)
	, m_iExtraAttackWoundedMod("CvUnit::m_iExtraAttackWoundedMod", m_syncArchive)
	, m_iFlankAttackModifier(0)
	, m_iExtraOpenDefensePercent("CvUnit::m_iExtraOpenDefensePercent", m_syncArchive)
	, m_iExtraRoughDefensePercent("CvUnit::m_iExtraRoughDefensePercent", m_syncArchive)
	, m_iPillageChange("CvUnit::m_iPillageChange", m_syncArchive)
	, m_iPillageXPChange("CvUnit::m_iPillageXPChange", m_syncArchive)
	, m_iPillageHealChange("CvUnit::m_iPillageHealChange", m_syncArchive)
	, m_iUpgradeDiscount("CvUnit::m_iUpgradeDiscount", m_syncArchive)
	, m_iExperiencePercent("CvUnit::m_iExperiencePercent", m_syncArchive)
	, m_iDropRange("CvUnit::m_iDropRange", m_syncArchive)
	, m_iAirSweepCapableCount("CvUnit::m_iAirSweepCapableCount", m_syncArchive)
	, m_iExtraNavalMoves("CvUnit::m_iExtraNavalMoves", m_syncArchive)
	, m_iKamikazePercent("CvUnit::m_iKamikazePercent", m_syncArchive)
	, m_iBaseCombat("CvUnit::m_iBaseCombat", m_syncArchive)
	, m_iBaseRangedCombat("CvUnit::m_iBaseRangedCombat", m_syncArchive)
	, m_eFacingDirection("CvUnit::m_eFacingDirection", m_syncArchive, true)
	, m_iArmyId("CvUnit::m_iArmyId", m_syncArchive)
	, m_iIgnoreTerrainCostCount("CvUnit::m_iIgnoreTerrainCostCount", m_syncArchive)
	, m_iRoughTerrainEndsTurnCount("CvUnit::m_iRoughTerrainEndsTurnCount", m_syncArchive)
	, m_iEmbarkAbilityCount("CvUnit::m_iEmbarkAbilityCount", m_syncArchive)
	, m_iHoveringUnitCount("CvUnit::m_iHoveringUnitCount", m_syncArchive)
	, m_iFlatMovementCostCount("CvUnit::m_iFlatMovementCostCount", m_syncArchive)
	, m_iCanMoveImpassableCount("CvUnit::m_iCanMoveImpassableCount", m_syncArchive)
	, m_iOnlyDefensiveCount("CvUnit::m_iOnlyDefensiveCount", m_syncArchive)
	, m_iNoDefensiveBonusCount("CvUnit::m_iNoDefensiveBonusCount", m_syncArchive)
#if defined(LEKMOD_NO_FORTIFY_VS_RANGED_PROMO)
	, m_iNoFortifyVsRangedCount("CvUnit::m_iNoFortifyVsRangedCount", m_syncArchive)
#endif
	, m_iNoCaptureCount("CvUnit::m_iNoCaptureCount", m_syncArchive)
	, m_iNukeImmuneCount("CvUnit::m_iNukeImmuneCount", m_syncArchive)
	, m_iHiddenNationalityCount("CvUnit::m_iHiddenNationalityCount", m_syncArchive)
	, m_iAlwaysHostileCount("CvUnit::m_iAlwaysHostileCount", m_syncArchive)
	, m_iNoRevealMapCount("CvUnit::m_iNoRevealMapCount", m_syncArchive)
	, m_iCanMoveAllTerrainCount("CvUnit::m_iCanMoveAllTerrainCount", m_syncArchive)
	, m_iCanMoveAfterAttackingCount("CvUnit::m_iCanMoveAfterAttackingCount", m_syncArchive)
	, m_iFreePillageMoveCount("CvUnit::m_iFreePillageMoveCount", m_syncArchive)
	, m_iHealOnPillageCount(0)
	, m_iHPHealedIfDefeatEnemy("CvUnit::m_iHPHealedIfDefeatEnemy", m_syncArchive)
	, m_iGoldenAgeValueFromKills(0)
	, m_iHealIfDefeatExcludeBarbariansCount("CvUnit::m_iHealIfDefeatExcludeBarbariansCount", m_syncArchive)
	, m_iTacticalAIPlotX("CvUnit::m_iTacticalAIPlotX", m_syncArchive)
	, m_iTacticalAIPlotY("CvUnit::m_iTacticalAIPlotY", m_syncArchive)
	, m_iGarrisonCityID("CvUnit::m_iGarrisonCityID", m_syncArchive)
	, m_iFlags("CvUnit::m_iFlags", m_syncArchive)
	, m_iNumAttacks("CvUnit::m_iNumAttacks", m_syncArchive)
	, m_iAttacksMade("CvUnit::m_iAttacksMade", m_syncArchive)
#if defined(v35_TRAITIFY)
	, m_bAttackedLastTurn("CvUnit::m_bAttackedLastTurn", m_syncArchive) 
	, m_bKilledUnit("CvUnit::m_bKilledUnit", m_syncArchive)
	, m_bKilledUnitLastTurn("CvUnit::m_bKilledUnitLastTurn", m_syncArchive)
#endif
	, m_iGreatGeneralCount("CvUnit::m_iGreatGeneralCount", m_syncArchive)
	, m_iGreatAdmiralCount(0)
	, m_iGreatGeneralModifier("CvUnit::m_iGreatGeneralModifier", m_syncArchive)
	, m_iFriendlyLandsModifier("CvUnit::m_iFriendlyLandsModifier", m_syncArchive)
	, m_iFriendlyLandsAttackModifier("CvUnit::m_iFriendlyLandsAttackModifier", m_syncArchive)
	, m_iOutsideFriendlyLandsModifier("CvUnit::m_iOutsideFriendlyLandsModifier", m_syncArchive)
#ifdef NQ_GOLDEN_AGE_FOREIGN_ATTACK_BONUS
	, m_iGoldenAgeForeignAttackBonus("CvUnit::m_iGoldenAgeForeignAttackBonus", m_syncArchive)
#endif
#ifdef NQ_COMBAT_STRENGTH_NEAR_FRIENDLY_MINOR
	, m_iCombatStrengthNearFriendlyMinor("CvUnit::m_iCombatStrengthNearFriendlyMinor", m_syncArchive)
#endif
	, m_iNumInterceptions("CvUnit::m_iNumInterceptions", m_syncArchive)
	, m_iMadeInterceptionCount("CvUnit::m_iMadeInterceptionCount", m_syncArchive)
	, m_iEverSelectedCount(0)
	, m_bIgnoreDangerWakeup(false)
	, m_iEmbarkedAllWaterCount(0)
	, m_iEmbarkExtraVisibility(0)
	, m_iEmbarkDefensiveModifier(0)
	, m_iCapitalDefenseModifier(0)
	, m_iCapitalDefenseFalloff(0)
	, m_iCityAttackPlunderModifier(0)
#ifdef LEKMOD_MOVE_PENALTY_CITY_COMBAT
	, m_iCityAttackMovePenalty(0)
#endif
	, m_iReligiousStrengthLossRivalTerritory(0)
	, m_iTradeMissionInfluenceModifier(0)
	, m_iTradeMissionGoldModifier(0)
#if defined(v35_TRAITIFY)
	, m_iNearbyWaterCombatModifier(0)
#endif
	, m_strName("")
	, m_eGreatWork(NO_GREAT_WORK)
	, m_iTourismBlastStrength(0) 
	, m_iResearchBulbAmount(0)
#ifdef DECREASE_BULB_AMOUNT_OVER_TIME
	, m_iScientistBirthTurn(0)
#endif
#if defined(NQM_UNIT_FIX_NO_DOUBLE_INSTAHEAL_ON_SAME_TURN) || defined(NQM_UNIT_FIX_NO_INSTAHEAL_AFTER_PARADROP)
	, m_bCanInstahealThisTurn(true)
#endif
#if defined(NQM_UNIT_NO_AA_INTERCEPT_AFTER_MOVE_BEFORE_TURN_END) || defined(NQM_UNIT_FIGHTER_NO_INTERCEPT_UNTIL_AFTER_TURN_END)
	, m_bIsInterceptBlockedUntilEndTurn("CvUnit::m_bIsInterceptBlockedUntilEndTurn", m_syncArchive)
#endif
#ifdef AUI_DLLNETMESSAGEHANDLER_FIX_RESPAWN_PROPHET_IF_BEATEN_TO_LAST_RELIGION
	, m_bIsIgnoreExpended("CvUnit::m_bIsIgnoreExpended", m_syncArchive)
#endif
#ifdef PROMOTION_INSTA_HEAL_LOCKED
	, m_bInstaHealLocked(false)
#endif
	, m_bPromotionReady("CvUnit::m_bPromotionReady", m_syncArchive)
	, m_bDeathDelay("CvUnit::m_bDeathDelay", m_syncArchive)
	, m_bCombatFocus("CvUnit::m_bCombatFocus", m_syncArchive)
	, m_bInfoBarDirty("CvUnit::m_bInfoBarDirty", m_syncArchive)
	, m_bNotConverting("CvUnit::m_bNotConverting", m_syncArchive)
	, m_bAirCombat("CvUnit::m_bAirCombat", m_syncArchive)
	, m_bSetUpForRangedAttack("CvUnit::m_bSetUpForRangedAttack", m_syncArchive)
#if defined(LEKMOD_SUBMERGE_MISSION)
	, m_bCanSubmerge("CvUnit::m_bSubmerged", m_syncArchive)
	, m_bSubmerged("CvUnit::m_bSubmerged", m_syncArchive)
	, m_bHasSubmergedOrSurfaced("CvUnit::m_bHasSubmergedOrSurfaced", m_syncArchive)
#endif
#if defined(LEKMOD_RETRAIN_MISSION)
	, m_iNumSelectedPromotions("CvUnit::m_iNumSelectedPromotions", m_syncArchive)\
	, m_abSelectedPromotions("CvUnit::m_abSelectedPromotions", m_syncArchive)
#endif
	, m_bEmbarked("CvUnit::m_bEmbarked", m_syncArchive)
	, m_bAITurnProcessed("CvUnit::m_bAITurnProcessed", m_syncArchive, false, true)
	, m_eTacticalMove("CvUnit::m_eTacticalMove", m_syncArchive)
	, m_eOwner("CvUnit::m_eOwner", m_syncArchive)
	, m_eOriginalOwner("CvUnit::m_eOriginalOwner", m_syncArchive)
	, m_eCapturingPlayer("CvUnit::m_eCapturingPlayer", m_syncArchive)
	, m_bCapturedAsIs(false)
	, m_eUnitType("CvUnit::m_eUnitType", m_syncArchive)
	, m_eLeaderUnitType("CvUnit::m_eLeaderUnitType", m_syncArchive)
	, m_eInvisibleType("CvUnit::m_eInvisibleType", m_syncArchive)
	, m_eSeeInvisibleType("CvUnit::m_eSeeInvisibleType", m_syncArchive)
	, m_eGreatPeopleDirectiveType("CvUnit::m_eGreatPeopleDirectiveType", m_syncArchive)
	, m_combatUnit()
	, m_transportUnit()
	, m_extraDomainModifiers()
	, m_strNameIAmNotSupposedToBeUsedAnyMoreBecauseThisShouldNotBeCheckedAndWeNeedToPreserveSaveGameCompatibility("CvUnit::m_strNameIAmNotSupposedToBeUsedAnyMoreBecauseThisShouldNotBeCheckedAndWeNeedToPreserveSaveGameCompatibility", m_syncArchive, "")
	, m_strScriptData("CvUnit::m_szScriptData", m_syncArchive)
	, m_iScenarioData(0)
#if defined(FULL_YIELD_FROM_KILLS)
	, m_iYieldFromKills("CvUnit::m_iYieldFromKills", m_syncArchive)
	, m_iKillYieldCap("CvUnit::m_iKillYieldCap", m_syncArchive)
	, m_bKillYieldEraValid("CvUnit::m_bKillYieldEraValid", m_syncArchive)
#endif
	, m_terrainDoubleMoveCount("CvUnit::m_terrainDoubleMoveCount", m_syncArchive)
	, m_featureDoubleMoveCount("CvUnit::m_featureDoubleMoveCount", m_syncArchive)
	, m_terrainImpassableCount("CvUnit::m_terrainImpassableCount", m_syncArchive)
	, m_featureImpassableCount("CvUnit::m_featureImpassableCount", m_syncArchive)
	, m_extraTerrainAttackPercent("CvUnit::m_extraTerrainAttackPercent", m_syncArchive          )
	, m_extraTerrainDefensePercent("CvUnit::m_extraTerrainDefensePercent", m_syncArchive          )
	, m_extraFeatureAttackPercent("CvUnit::m_extraFeatureAttackPercent", m_syncArchive          )
	, m_extraFeatureDefensePercent("CvUnit::m_extraFeatureDefensePercent", m_syncArchive          )
	, m_extraUnitCombatModifier("CvUnit::m_extraUnitCombatModifier", m_syncArchive          )
	, m_unitClassModifier("CvUnit::m_unitClassModifier", m_syncArchive          )
	, m_iMissionTimer(0)
	, m_iMissionAIX("CvUnit::m_iMissionAIX", m_syncArchive)
	, m_iMissionAIY("CvUnit::m_iMissionAIY", m_syncArchive)
	, m_eMissionAIType("CvUnit::m_eMissionAIType", m_syncArchive)
	, m_missionAIUnit()
	, m_eActivityType("CvUnit::m_eActivityType", m_syncArchive, true)
	, m_eAutomateType("CvUnit::m_eAutomateType", m_syncArchive)
	, m_eUnitAIType("CvUnit::m_eUnitAIType", m_syncArchive)
	, m_thisHandle(this)
	, m_bWaitingForMove(false)
	, m_pReligion(FNEW(CvUnitReligion, c_eCiv5GameplayDLL, 0))
	, m_iMapLayer(DEFAULT_UNIT_MAP_LAYER)
	, m_iNumGoodyHutsPopped(0)
	, m_eGiveDomain("CvUnit::m_eGiveDomain", m_syncArchive)
	, m_eConvertDomain("CvUnit::m_eConvertDomain", m_syncArchive)
	, m_eConvertDomainUnit("CvUnit::m_eConvertDomainUnit", m_syncArchive)
	, m_iLastGameTurnAtFullHealth(-1)
{
	initPromotions();
	OBJECT_ALLOCATED
	FSerialization::unitsToCheck.insert(this);
	reset(0, NO_UNIT, NO_PLAYER, true);
}



CvUnit::~CvUnit()
{
	m_thisHandle.ignoreDestruction(true);
	FSerialization::unitsToCheck.erase(this);
	if(!gDLL->GetDone() && GC.IsGraphicsInitialized())
	{
#ifdef AUI_WARNING_FIXES
		try
		{
#endif
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitDestroyed(pDllUnit.get());
#ifdef AUI_WARNING_FIXES
		}
		catch (int) {}
#endif
	}

	if (m_iMapLayer != DEFAULT_UNIT_MAP_LAYER)
		GC.getMap().plotManager().RemoveUnit(GetIDInfo(), m_iX, m_iY, m_iMapLayer);

	uninit();

	OBJECT_DESTROYED
}



#ifdef AUI_UNIT_FIX_GIFTED_UNITS_ARE_GIFTED_NOT_CLONED
void CvUnit::init(int iID, UnitTypes eUnit, UnitAITypes eUnitAI, PlayerTypes eOwner, int iX, int iY, DirectionTypes eFacingDirection, bool bNoMove, bool bSetupGraphical, int iMapLayer                             , int iNumGoodyHutsPopped, bool bIsGifted)
{
	initWithNameOffset(iID, eUnit, -1, eUnitAI, eOwner, iX, iY, eFacingDirection, bNoMove, bSetupGraphical, iMapLayer, iNumGoodyHutsPopped, bIsGifted);
#else
void CvUnit::init(int iID, UnitTypes eUnit, UnitAITypes eUnitAI, PlayerTypes eOwner, int iX, int iY, DirectionTypes eFacingDirection, bool bNoMove, bool bSetupGraphical, int iMapLayer                             , int iNumGoodyHutsPopped)
{
	initWithNameOffset(iID, eUnit, -1, eUnitAI, eOwner, iX, iY, eFacingDirection, bNoMove, bSetupGraphical, iMapLayer, iNumGoodyHutsPopped);
#endif
}



#ifdef AUI_UNIT_FIX_GIFTED_UNITS_ARE_GIFTED_NOT_CLONED
void CvUnit::initWithNameOffset(int iID, UnitTypes eUnit, int iNameOffset, UnitAITypes eUnitAI, PlayerTypes eOwner, int iX, int iY, DirectionTypes eFacingDirection, bool bNoMove, bool bSetupGraphical, int iMapLayer, int iNumGoodyHutsPopped, bool bIsGifted)
#else
void CvUnit::initWithNameOffset(int iID, UnitTypes eUnit, int iNameOffset, UnitAITypes eUnitAI, PlayerTypes eOwner, int iX, int iY, DirectionTypes eFacingDirection, bool bNoMove, bool bSetupGraphical, int iMapLayer, int iNumGoodyHutsPopped)
#endif
{
	VALIDATE_OBJECT
	CvString strBuffer;
	int iUnitName;
#ifdef AUI_WARNING_FIXES
	uint iI;
	int iJ;
#else
	int iI;
#endif

	CvAssert(NO_UNIT != eUnit);

	initPromotions();
	m_pReligion->Init();



	reset(iID, eUnit, eOwner);

	if(eFacingDirection == NO_DIRECTION)
		m_eFacingDirection = DIRECTION_SOUTHEAST;
	else
		m_eFacingDirection = eFacingDirection;


	PromotionTypes ePromotion;
#ifndef AUI_UNIT_FIX_HOVERING_EMBARK
	for(iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		if(getUnitInfo().GetFreePromotions(iI))
		{
			ePromotion = (PromotionTypes) iI;

			if(GC.getPromotionInfo(ePromotion)->IsHoveringUnit())
				setHasPromotion(ePromotion, true);
		}
	}
#endif


	m_iMapLayer = iMapLayer;
	

	m_iNumGoodyHutsPopped = iNumGoodyHutsPopped;






	setXY(iX, iY, false, false, false, false, bNoMove);






	plot()->updateCenterUnit();

	SetGreatWork(NO_GREAT_WORK);
#ifdef AUI_UNIT_FIX_GIFTED_UNITS_ARE_GIFTED_NOT_CLONED
	if (!bIsGifted)
	{
#endif
	iUnitName = GC.getGame().getUnitCreatedCount(getUnitType());
	int iNumNames = getUnitInfo().GetNumUnitNames();
	if(iUnitName < iNumNames)
	{
		if(iNameOffset == -1)
		{
			iNameOffset = GC.getGame().getJonRandNum(iNumNames, "Unit name selection");
		}
	
#ifdef AUI_WARNING_FIXES
		for (iJ = 0; iJ < iNumNames; iJ++)
		{
			int iIndex = (iNameOffset + iJ) % iNumNames;
#else
		for(iI = 0; iI < iNumNames; iI++)
		{
			int iIndex = (iNameOffset + iI) % iNumNames;
#endif
			CvString strName = getUnitInfo().GetUnitNames(iIndex);
			if(!GC.getGame().isGreatPersonBorn(strName))
			{
				setName(strName);
				SetGreatWork(getUnitInfo().GetGreatWorks(iIndex));
				GC.getGame().addGreatPersonBornName(strName);
				break;
			}
		}
	}
#ifdef AUI_UNIT_FIX_GIFTED_UNITS_ARE_GIFTED_NOT_CLONED
	}
#endif

	setGameTurnCreated(GC.getGame().getGameTurn());

	GC.getGame().incrementUnitCreatedCount(getUnitType());

	CvPlayer& kPlayer = GET_PLAYER(getOwner());

	GC.getGame().incrementUnitClassCreatedCount((UnitClassTypes)(getUnitInfo().GetUnitClassType()));
	GET_TEAM(getTeam()).changeUnitClassCount(((UnitClassTypes)(getUnitInfo().GetUnitClassType())), 1);
	kPlayer.changeUnitClassCount(((UnitClassTypes)(getUnitInfo().GetUnitClassType())), 1);


	if(getUnitInfo().GetWorkRate() > 0 && getUnitInfo().GetDomainType() == DOMAIN_LAND)
	{
		kPlayer.ChangeNumBuilders(1);
	}


	if(GC.getUnitInfo(getUnitType())->GetUnhappiness() != 0)
	{
		kPlayer.ChangeUnhappinessFromUnits(GC.getUnitInfo(getUnitType())->GetUnhappiness());
	}

	kPlayer.changeExtraUnitCost(getUnitInfo().GetExtraMaintenanceCost());


#ifdef AUI_WARNING_FIXES
	for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
	{
		if(getUnitInfo().GetResourceQuantityRequirement(iResourceLoop) > 0)
		{
			kPlayer.changeNumResourceUsed((ResourceTypes) iResourceLoop, GC.getUnitInfo(getUnitType())->GetResourceQuantityRequirement(iResourceLoop));
		}
	}

	if(getUnitInfo().GetNukeDamageLevel() != -1)
	{
		kPlayer.changeNumNukeUnits(1);
	}

	if(getUnitInfo().IsMilitarySupport())
	{
		kPlayer.changeNumMilitaryUnits(1);
	}


	for(iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		if(getUnitInfo().GetFreePromotions(iI))
		{
			ePromotion = (PromotionTypes) iI;

#ifndef AUI_UNIT_FIX_HOVERING_EMBARK
			if(!GC.getPromotionInfo(ePromotion)->IsHoveringUnit())
#endif
				setHasPromotion(ePromotion, true);
		}
	}

	const UnitCombatTypes unitCombatType = getUnitCombatType();
	if(unitCombatType != NO_UNITCOMBAT)
	{

#ifdef AUI_WARNING_FIXES
		for (iI = 0; iI < GC.getNumPromotionInfos(); iI++)
		{
			const PromotionTypes promotionID = (PromotionTypes)iI;
#else
		for(int iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
		{
			const PromotionTypes promotionID = (PromotionTypes)iJ;
#endif
			if(kPlayer.GetPlayerTraits()->HasFreePromotionUnitCombat(promotionID, unitCombatType))
			{
				setHasPromotion(promotionID, true);
			}
		}
	}


	for(iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		ePromotion = (PromotionTypes) iI;

		if(kPlayer.IsFreePromotion(ePromotion))
		{

			if(::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()))
			{
				setHasPromotion(ePromotion, true);
			}

			else if(::IsPromotionValidForCivilianUnitType(ePromotion, getUnitType()))
			{
				setHasPromotion(ePromotion, true);
			}

		}
	}
#if defined(LEKMOD_UNIT_STRENGTH_PROMOTION_ERA)
	for (iI = 0; iI < GC.getNumEraInfos(); iI++)
	{
		EraTypes eEra = static_cast<EraTypes>(iI);
		if (eEra <= kPlayer.GetCurrentEra())
		{
			for (int jJ = 0; jJ < GC.getNumPromotionInfos(); jJ++)
			{
				ePromotion = static_cast<PromotionTypes>(jJ);
				if (getUnitInfo().IsFreePromotionEra(ePromotion, eEra))
				{
					if (!isHasPromotion(ePromotion))
						setHasPromotion(ePromotion, true);
				}
			}
			ChangeBaseCombatStrength(getUnitInfo().GetEraStrengthChanges(eEra));
			ChangeBaseRangedCombatStrength(getUnitInfo().GetEraRangedStrengthChanges(eEra));
			changeExtraMoves(getUnitInfo().GetEraMovesChanges(eEra));
			changeExperience(getUnitInfo().GetEraStartingExperienceChanges(eEra));
		}
	}
#endif

	if(GET_TEAM(getTeam()).canEmbark() || kPlayer.GetPlayerTraits()->IsEmbarkedAllWater())
	{
		PromotionTypes ePromotionEmbarkation = kPlayer.GetEmbarkationPromotion();

		bool bGivePromotion = false;


		if(getDomainType() == DOMAIN_LAND)
		{
			if(!IsCombatUnit())
				bGivePromotion = true;
		}


		if(!bGivePromotion && ::IsPromotionValidForUnitCombatType(ePromotionEmbarkation, getUnitType()))
			bGivePromotion = true;


		if(bGivePromotion)
			setHasPromotion(ePromotionEmbarkation, true);
	}


	if(kPlayer.GetPlayerTraits()->IsEmbarkedAllWater())
	{
		PromotionTypes ePromotionOceanImpassable = (PromotionTypes)GC.getPROMOTION_OCEAN_IMPASSABLE();
		if(isHasPromotion(ePromotionOceanImpassable))
		{
			setHasPromotion(ePromotionOceanImpassable, false);
		}
		PromotionTypes ePromotionOceanImpassableUntilAstronomy = (PromotionTypes)GC.getPROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY();
		if(isHasPromotion(ePromotionOceanImpassableUntilAstronomy))
		{
			setHasPromotion(ePromotionOceanImpassableUntilAstronomy, false);
		}
	}


	if (getUnitInfo().GetNumExoticGoods() > 0)
	{
		changeNumExoticGoods(getUnitInfo().GetNumExoticGoods());
	}
#if defined(LEKMOD_SUBMERGE_MISSION)
	if (getUnitInfo().IsSubmerge() == true)
	{
		setCanSubmerge(true);
	}
#endif

	int iXP = GC.getGame().getHandicapInfo().getAIFreeXP();
	if (iXP && !kPlayer.isHuman() &&                                         canAcquirePromotionAny())
	{
		changeExperience(iXP);
	}


	int iXPPercent = GC.getGame().getHandicapInfo().getAIFreeXPPercent();
	if (iXPPercent && !kPlayer.isHuman() &&                                         canAcquirePromotionAny())
	{
		changeExperiencePercent(iXPPercent);
	}



	if(getUnitInfo().IsImmobile())
	{
		SetImmobile(true);
	}

	setMoves(maxMoves());
#if defined(UNITS_REMEMBER_HOME)
	
	if (pPlotCity)
	{
		SetHomeCity(pPlotCity->GetID());
		if (!getUnitInfo().IsSpreadReligion() || !getUnitInfo().IsRemoveHeresy())
		{

		}
	}
#endif

	if (getUnitInfo().IsSpreadReligion() || getUnitInfo().IsRemoveHeresy())
	{
		#if !defined(UNITS_REMEMBER_HOME)
		CvCity* pPlotCity = plot()->getPlotCity();
		#endif
		if (pPlotCity)
		{
			ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
			if (eReligion > RELIGION_PANTHEON)
			{
				GetReligionData()->SetReligion(eReligion);
#ifdef NQ_BELIEF_EXTRA_MISSIONARY_SPREADS
				int iSpreadsLeft = getUnitInfo().GetReligionSpreads();
				if (!getUnitInfo().IsFoundReligion())
				{
					iSpreadsLeft += pPlotCity->GetCityBuildings()->GetMissionaryExtraSpreads();
					

					ReligionTypes eFoundedReligion = GC.getGame().GetGameReligions()->GetFounderBenefitsReligion(kPlayer.GetID());
					if(eFoundedReligion == eReligion)
					{
						const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eFoundedReligion, NO_PLAYER);
						iSpreadsLeft += pReligion->m_Beliefs.GetMissionaryExtraSpreads();
					}
					
				}
				GetReligionData()->SetSpreadsLeft(iSpreadsLeft);
#else
				GetReligionData()->SetSpreadsLeft(getUnitInfo().GetReligionSpreads() + pPlotCity->GetCityBuildings()->GetMissionaryExtraSpreads());
#endif
				GetReligionData()->SetReligiousStrength(getUnitInfo().GetReligiousStrength());
			}
		}
	}
	if (getUnitInfo().GetOneShotTourism() > 0)
	{
		SetTourismBlastStrength(kPlayer.GetCulture()->GetTourismBlastStrength(getUnitInfo().GetOneShotTourism()));
	}


#ifdef DECREASE_BULB_AMOUNT_OVER_TIME
	if (GC.getGame().isOption("GAMEOPTION_NO_SCIENTIST_SAVING"))
	{
		SetScientistBirthTurn(GC.getGame().getGameTurn());
	}
	else if (getUnitInfo().GetBaseBeakersTurnsToCount() > 0)
	{
		SetResearchBulbAmount(kPlayer.GetScienceYieldFromPreviousTurns(GC.getGame().getGameTurn(), getUnitInfo().GetBaseBeakersTurnsToCount()));
	}
#else
	if (getUnitInfo().GetBaseBeakersTurnsToCount() > 0)
	{
		SetResearchBulbAmount(kPlayer.GetScienceYieldFromPreviousTurns(GC.getGame().getGameTurn(), getUnitInfo().GetBaseBeakersTurnsToCount()));
	}
#endif
	int iTourism = kPlayer.GetPlayerPolicies()->GetTourismFromUnitCreation((UnitClassTypes)(getUnitInfo().GetUnitClassType()));
	if (iTourism > 0)
	{
#ifdef NQ_FIX_ADD_TOURISM_GAME_SPEED_MOD
		iTourism = iTourism * GC.getGame().getGameSpeedInfo().getCulturePercent() / 100;
#endif
		kPlayer.GetCulture()->AddTourismAllKnownCivs(iTourism);
	}

#if defined(NQ_SCIENCE_PER_GREAT_PERSON_BORN) || defined(NQ_SCIENCE_PER_GREAT_PERSON_BORN_FROM_POLICIES) || defined(NQ_INFLUENCE_BOOST_PER_GREAT_PERSON_BORN_FROM_POLICIES)
	if (IsGreatPerson())
	{
		int iScienceBonus = 0;
#ifdef NQ_SCIENCE_PER_GREAT_PERSON_BORN
		int iScienceBonusPerEra = kPlayer.GetPlayerTraits()->GetSciencePerGreatPersonBorn();
		if (iScienceBonusPerEra != 0)
		{
			iScienceBonus += iScienceBonusPerEra * (kPlayer.GetCurrentEra() + 1);
		}
#endif
#ifdef NQ_SCIENCE_PER_GREAT_PERSON_BORN_FROM_POLICIES
		int iScienceBonusFromPolicies = kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_SCIENCE_PER_GREAT_PERSON_BORN);
		if (iScienceBonusFromPolicies != 0)
		{
			iScienceBonus += iScienceBonusFromPolicies;
		}
#endif

		if (iScienceBonus > 0)
		{
			iScienceBonus *= GC.getGame().getGameSpeedInfo().getTrainPercent();
			iScienceBonus /= 100;


			iScienceBonus /= 5;
			iScienceBonus *= 5;

			TechTypes eCurrentTech = kPlayer.GetPlayerTechs()->GetCurrentResearch();
			if(eCurrentTech == NO_TECH)
			{
				kPlayer.changeOverflowResearch(iScienceBonus);
			}
			else
			{
				CvTeam &kTeam = GET_TEAM(kPlayer.getTeam());
				kTeam.GetTeamTechs()->ChangeResearchProgress(eCurrentTech, iScienceBonus, kPlayer.GetID());
			}
			
			if (plot() && plot()->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF)
			{
				char text[256] = {0};
				sprintf_s(text, "[COLOR_BLUE]+%d[ENDCOLOR]", iScienceBonus);
				float fDelay = GC.getPOST_COMBAT_TEXT_DELAY() * 2;
				DLLUI->AddPopupText(plot()->getX(), plot()->getY(), text, fDelay);
			}
		}

#ifdef NQ_INFLUENCE_BOOST_PER_GREAT_PERSON_BORN_FROM_POLICIES
		for (int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
		{
			PlayerTypes eMinorCivLoop = (PlayerTypes) iMinorCivLoop;
			if (GET_PLAYER(eMinorCivLoop).isAlive() && GET_TEAM(kPlayer.getTeam()).isHasMet(GET_PLAYER(eMinorCivLoop).getTeam()))
			{
				GET_PLAYER(eMinorCivLoop).GetMinorCivAI()->ChangeFriendshipWithMajor(kPlayer.GetID(), kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_INFLUENCE_BOOST_PER_GREAT_PERSON_BORN));
			}
		}
#endif
	}
#endif


	if(IsRecon())
	{
		setReconPlot(plot());
	}

	if(getDomainType() == DOMAIN_LAND)
	{
		if(IsCombatUnit())
		{
			if((GC.getGame().getBestLandUnit() == NO_UNIT) || (GetBaseCombatStrength() > GC.getGame().getBestLandUnitCombat()))
			{
				GC.getGame().setBestLandUnit(getUnitType());
			}
		}
	}


	if(getOwner() == GC.getGame().getActivePlayer())
	{
		DLLUI->setDirty(GameData_DIRTY_BIT, true);

		kPlayer.GetUnitCycler().AddUnit( GetID() );
	}


#ifdef AUI_UNIT_FIX_GIFTED_UNITS_ARE_GIFTED_NOT_CLONED
	if (!bIsGifted && isWorldUnitClass((UnitClassTypes)(getUnitInfo().GetUnitClassType())))
#else
	if(isWorldUnitClass((UnitClassTypes)(getUnitInfo().GetUnitClassType())))
#endif
	{
		for(iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if(GET_PLAYER((PlayerTypes)iI).isAlive() && GC.getGame().getActivePlayer())
			{
				if(GET_TEAM(getTeam()).isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_SOMEONE_CREATED_UNIT", kPlayer.getNameKey(), getNameKey());
					DLLUI->AddMessage(0, ((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), strBuffer                                                                                                                                                                       );
				}
				else
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_UNKNOWN_CREATED_UNIT", getNameKey());
					DLLUI->AddMessage(0, ((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), strBuffer                                                                                                                                           );
				}
			}
		}

		strBuffer = GetLocalizedText("TXT_KEY_MISC_SOMEONE_CREATED_UNIT", kPlayer.getNameKey(), getNameKey());
		GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getOwner(), strBuffer, getX(), getY());
	}


	if(plot()->isCity())
	{
		plot()->getPlotCity()->updateStrengthValue();
	}

	m_iArmyId = FFreeList::INVALID_INDEX;

	m_eUnitAIType = eUnitAI;


	kPlayer.UpdateUnitProductionMaintenanceMod();


#ifdef AUI_UNIT_FIX_GIFTED_UNITS_ARE_GIFTED_NOT_CLONED
	if (!bIsGifted && !kPlayer.isMinorCiv() && !isBarbarian())
#else
	if(!kPlayer.isMinorCiv() && !isBarbarian())
#endif
	{
		PlayerTypes eMinor;
		for(int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
		{
			eMinor = (PlayerTypes) iMinorCivLoop;
			if(GET_PLAYER(eMinor).isAlive())
			{

				GET_PLAYER(eMinor).GetMinorCivAI()->DoTestActiveQuestsForPlayer(getOwner(),                   true,                   false, MINOR_CIV_QUEST_GREAT_PERSON);
			}
		}
	}
#ifdef NQM_UNIT_FIX_NO_INSTAHEAL_ON_CREATION_TURN
	setCanInstahealThisTurn(canInstahealThisTurn());
#endif

#ifdef LEKMOD_NEW_LUA_EVENTS

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem)
	{
		CvLuaArgsHandle args;

		args->Push(eOwner);
		args->Push(GetID());
		args->Push(iX);
		args->Push(iY);



		bool bResult = false;
		LuaSupport::CallHook(pkScriptSystem, "UnitCreated", args.get(), bResult);
	}
#endif
	if(bSetupGraphical)
		setupGraphical();

}




void CvUnit::uninit()
{
	VALIDATE_OBJECT
	uninitInfos();

	m_missionQueue.clear();

	m_Promotions.Uninit();

	m_kLastPath.clear();

	delete m_pReligion;
	m_pReligion = NULL;
}





void CvUnit::reset(int iID, UnitTypes eUnit, PlayerTypes eOwner, bool bConstructorCall)
{
	VALIDATE_OBJECT
	int iI;

	FAutoArchive& archive = getSyncArchive();
	archive.clearDelta();
	archive.reset();

	m_iID = iID;
	m_iHotKeyNumber = -1;
	m_iX = INVALID_PLOT_COORD;
	m_iY = INVALID_PLOT_COORD;
	m_iLastMoveTurn = 0;
	m_iCycleOrder = -1;
	m_iDeployFromOperationTurn = -100;
#if defined(UNITS_REMEMBER_HOME)
	m_iHomeCity = -1; 
#endif
	m_iReconX = INVALID_PLOT_COORD;
	m_iReconY = INVALID_PLOT_COORD;
	m_iReconCount = 0;
	m_iGameTurnCreated = 0;
	m_iDamage = 0;
	m_iMoves = 0;
	m_bImmobile = false;
	m_iExperience = 0;
	m_iLevel = 1;
	m_iCargo = 0;
	m_iAttackPlotX = INVALID_PLOT_COORD;
	m_iAttackPlotY = INVALID_PLOT_COORD;
	m_iCombatTimer = 0;
	m_iCombatFirstStrikes = 0;
	m_iCombatDamage = 0;
	m_iFortifyTurns = 0;
	m_bFortifiedThisTurn = false;
	m_iBlitzCount = 0;
	m_iAmphibCount = 0;
	m_iRiverCrossingNoPenaltyCount = 0;
	m_iEnemyRouteCount = 0;
	m_iRivalTerritoryCount = 0;
	m_iMustSetUpToRangedAttackCount = 0;
	m_iRangeAttackIgnoreLOSCount = 0;
	m_iCityAttackOnlyCount = 0;
	m_iCaptureDefeatedEnemyCount = 0;
	m_iRangedSupportFireCount = 0;
	m_iAlwaysHealCount = 0;
	m_iHealOutsideFriendlyCount = 0;
	m_iHillsDoubleMoveCount = 0;
	m_iImmuneToFirstStrikesCount = 0;
	m_iExtraVisibilityRange = 0;
	m_iExtraMoves = 0;
	m_iExtraMoveDiscount = 0;
	m_iExtraRange = 0;
	m_iExtraIntercept = 0;
	m_iExtraEvasion = 0;
	m_iExtraFirstStrikes = 0;
	m_iExtraChanceFirstStrikes = 0;
	m_iExtraWithdrawal = 0;
	m_iExtraEnemyHeal = 0;
	m_iExtraNeutralHeal = 0;
	m_iExtraFriendlyHeal = 0;
	m_iSameTileHeal = 0;
	m_iAdjacentTileHeal = 0;
	m_iEnemyDamageChance = 0;
	m_iNeutralDamageChance = 0;
	m_iEnemyDamage = 0;
	m_iNeutralDamage = 0;
	m_iNearbyEnemyCombatMod = 0;
	m_iNearbyEnemyCombatRange = 0;
	m_iExtraCombatPercent = 0;
	m_iAdjacentModifier = 0;
	m_iRangedAttackModifier = 0;
	m_iInterceptionCombatModifier = 0;
	m_iInterceptionDefenseDamageModifier = 0;
	m_iAirSweepCombatModifier = 0;
	m_iAttackModifier = 0;
	m_iDefenseModifier = 0;
	m_iExtraCityAttackPercent = 0;
	m_iExtraCityDefensePercent = 0;
	m_iExtraRangedDefenseModifier = 0;
	m_iExtraHillsAttackPercent = 0;
	m_iExtraHillsDefensePercent = 0;
	m_iExtraOpenAttackPercent = 0;
	m_iExtraOpenRangedAttackMod= 0;
	m_iExtraRoughAttackPercent = 0;
	m_iExtraRoughRangedAttackMod= 0;
	m_iExtraAttackFortifiedMod= 0;
	m_iExtraAttackWoundedMod= 0;
	m_iFlankAttackModifier=0;
	m_iExtraOpenDefensePercent = 0;
	m_iExtraRoughDefensePercent = 0;
	m_iPillageChange = 0;
	m_iPillageXPChange = 0;
	m_iPillageHealChange = 0;
	m_iUpgradeDiscount = 0;
	m_iExperiencePercent = 0;
	m_iDropRange = 0;
	m_iAirSweepCapableCount = 0;
	m_iExtraNavalMoves = 0;
	m_iKamikazePercent = 0;
	m_eFacingDirection = DIRECTION_SOUTHEAST;
	m_iIgnoreTerrainCostCount = 0;
	m_eGiveDomain = NO_DOMAIN;
	m_iRoughTerrainEndsTurnCount = 0;
	m_iEmbarkAbilityCount = 0;
	m_iHoveringUnitCount = 0;
	m_iFlatMovementCostCount = 0;
	m_iCanMoveImpassableCount = 0;
	m_iOnlyDefensiveCount = 0;
	m_iNoDefensiveBonusCount = 0;
#if defined(LEKMOD_NO_FORTIFY_VS_RANGED_PROMO)
	m_iNoFortifyVsRangedCount = 0;
#endif
	m_iNoCaptureCount = 0;
	m_iNukeImmuneCount = 0;
	m_iAlwaysHealCount = 0;
	m_iHiddenNationalityCount = 0;
	m_iAlwaysHostileCount = 0;
	m_iNoRevealMapCount = 0;
	m_iCanMoveAllTerrainCount = 0;
	m_iCanMoveAfterAttackingCount = 0;
	m_iFreePillageMoveCount = 0;
	m_iHPHealedIfDefeatEnemy = 0;
	m_iGoldenAgeValueFromKills = 0;
	m_iSapperCount = 0;
	m_iCanHeavyCharge = 0;
#ifdef NQ_HEAVY_CHARGE_DOWNHILL
	m_iHeavyChargeDownhill = 0;
#endif
	m_iNumExoticGoods = 0;
	m_iTacticalAIPlotX = INVALID_PLOT_COORD;
	m_iTacticalAIPlotY = INVALID_PLOT_COORD;
	m_iGarrisonCityID = -1;
	m_iFlags = 0;
	m_iNumAttacks = 1;
	m_iAttacksMade = 0;
#if defined(v35_TRAITIFY)
	m_bAttackedLastTurn = false;
	m_bKilledUnit = false;
	m_bKilledUnitLastTurn = false;
#endif
	m_iGreatGeneralCount = 0;
	m_iGreatAdmiralCount = 0;
	m_iGreatGeneralModifier = 0;
	m_iGreatGeneralReceivesMovementCount = 0;
	m_iEmbarkedUnitReceivesMovementCount = 0;

#ifdef LEKMOD_LONGSHIP_ALL_PROMO
	m_iLandUnitReceivesMovementCount = 0;
#endif
#ifdef NQ_ART_OF_WAR_PROMOTION
	m_iGreatGeneralOnOrAdjacentConfersMovement = 0;
#endif
	m_iGreatGeneralCombatModifier = 0;
	m_iIgnoreGreatGeneralBenefit = 0;
	m_iIgnoreZOC = 0;
	m_iHealIfDefeatExcludeBarbariansCount = 0;
	m_iNumInterceptions = 1;
	m_iMadeInterceptionCount = 0;
	m_iEverSelectedCount = 0;

	m_bIgnoreDangerWakeup = false;
	m_iEmbarkedAllWaterCount = 0;
	m_iEmbarkExtraVisibility = 0;
	m_iEmbarkDefensiveModifier = 0;
	m_iCapitalDefenseModifier = 0;
	m_iCapitalDefenseFalloff = 0;
	m_iCityAttackPlunderModifier = 0;
#ifdef LEKMOD_MOVE_PENALTY_CITY_COMBAT
	m_iCityAttackMovePenalty = 0;
#endif
	m_iReligiousStrengthLossRivalTerritory = 0;
	m_iTradeMissionInfluenceModifier = 0;
	m_iTradeMissionGoldModifier = 0;
#if defined(v35_TRAITIFY)
	m_iNearbyWaterCombatModifier = 0;
	m_iAttackExtraMoves = 0;
	m_iKillRefreshMovesCount = 0;
	m_iKillRefreshAttacksCount = 0;
#endif
	m_bPromotionReady = false;
	m_bDeathDelay = false;
	m_bCombatFocus = false;
	m_bInfoBarDirty = false;
	m_bNotConverting = false;
	m_bAirCombat = false;
	m_bSetUpForRangedAttack = false;
#if defined(LEKMOD_SUBMERGE_MISSION)
	m_bCanSubmerge = false;
	m_bSubmerged = false;
	m_bHasSubmergedOrSurfaced = false;
#endif
#if defined(LEKMOD_RETRAIN_MISSION)
	m_iNumSelectedPromotions = 0;
#endif
	m_bEmbarked = false;
	m_bAITurnProcessed = false;
	m_bWaitingForMove = false;
	m_eTacticalMove = NO_TACTICAL_MOVE;
#if defined(NQM_UNIT_FIX_NO_DOUBLE_INSTAHEAL_ON_SAME_TURN) || defined(NQM_UNIT_FIX_NO_INSTAHEAL_AFTER_PARADROP)
	m_bCanInstahealThisTurn = true;
#endif
#if defined(NQM_UNIT_NO_AA_INTERCEPT_AFTER_MOVE_BEFORE_TURN_END) || defined(NQM_UNIT_FIGHTER_NO_INTERCEPT_UNTIL_AFTER_TURN_END)
	m_bIsInterceptBlockedUntilEndTurn = false;
#endif
#ifdef AUI_DLLNETMESSAGEHANDLER_FIX_RESPAWN_PROPHET_IF_BEATEN_TO_LAST_RELIGION
	m_bIsIgnoreExpended = false;
#endif

	m_eOwner = eOwner;
	m_eOriginalOwner = eOwner;
	m_eCapturingPlayer = NO_PLAYER;
	m_bCapturedAsIs = false;
	m_eUnitType = eUnit;
	m_pUnitInfo = (NO_UNIT != m_eUnitType) ? GC.getUnitInfo(m_eUnitType) : NULL;
	m_iBaseCombat = (NO_UNIT != m_eUnitType) ? m_pUnitInfo->GetCombat() : 0;
	m_iBaseRangedCombat = (NO_UNIT != m_eUnitType) ? m_pUnitInfo->GetRangedCombat() : 0;
	m_iCachedPower = (NO_UNIT != m_eUnitType) ? m_pUnitInfo->DoUpdatePower(m_iBaseCombat, m_iBaseRangedCombat) : 0;
	m_eLeaderUnitType = NO_UNIT;
	m_eInvisibleType = NO_INVISIBLE;
	m_eSeeInvisibleType = NO_INVISIBLE;
	m_eGreatPeopleDirectiveType = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;
	m_iCargoCapacity = 0;

	m_combatUnit.reset();
	m_transportUnit.reset();

	m_extraDomainModifiers.clear();

	for(iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		m_extraDomainModifiers.push_back(0);
	}

	m_strName = "";
	m_eGreatWork = NO_GREAT_WORK;
	m_iTourismBlastStrength = 0;
	m_iResearchBulbAmount = 0;
#ifdef DECREASE_BULB_AMOUNT_OVER_TIME
	m_iScientistBirthTurn = 0;
#endif
#ifdef PROMOTION_INSTA_HEAL_LOCKED
	m_bInstaHealLocked = false;
#endif
	m_strNameIAmNotSupposedToBeUsedAnyMoreBecauseThisShouldNotBeCheckedAndWeNeedToPreserveSaveGameCompatibility = "";
	m_strScriptData ="";
	m_iScenarioData = 0;

	m_unitMoveLocs.clear();

	uninitInfos();


	m_iMissionTimer = 0;
	m_eActivityType = ACTIVITY_AWAKE;
	m_eAutomateType = NO_AUTOMATE;

	m_kLastPath.clear();
	m_uiLastPathCacheDest = (uint)-1;

	m_iMapLayer = DEFAULT_UNIT_MAP_LAYER;
	m_iNumGoodyHutsPopped = 0;
	m_iLastGameTurnAtFullHealth = -1;

	if(!bConstructorCall)
	{
		m_Promotions.Reset();
#if defined(LEKMOD_RETRAIN_MISSION)
		m_abSelectedPromotions.clear();
		m_abSelectedPromotions.resize(GC.getNumPromotionInfos());
		for (int i = 0; i < GC.getNumPromotionInfos(); i++)
		{
			m_abSelectedPromotions.setAt(i, false);
		}
#endif
		CvAssertMsg((0 < GC.getNumTerrainInfos()), "GC.getNumTerrainInfos() is not greater than zero but a float array is being allocated in CvUnit::reset");
		m_terrainDoubleMoveCount.clear();
		m_terrainImpassableCount.clear();
		m_extraTerrainAttackPercent.clear();
		m_extraTerrainDefensePercent.clear();

		m_terrainDoubleMoveCount.resize(GC.getNumTerrainInfos());
		m_terrainImpassableCount.resize(GC.getNumTerrainInfos());
		m_extraTerrainAttackPercent.resize(GC.getNumTerrainInfos());
		m_extraTerrainDefensePercent.resize(GC.getNumTerrainInfos());
#if defined(FULL_YIELD_FROM_KILLS)
		m_iYieldFromKills.clear();
		m_iYieldFromKills.resize(NUM_YIELD_TYPES);
		m_iKillYieldCap.clear();
		m_iKillYieldCap.resize(NUM_YIELD_TYPES);
		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			m_iYieldFromKills.setAt(i, 0);
			m_iKillYieldCap.setAt(i, 0);
		}
		m_bKillYieldEraValid.clear();
		m_bKillYieldEraValid.resize(GC.getNumEraInfos());
		for (int i = 0; i < GC.getNumEraInfos(); i++)
		{
			m_bKillYieldEraValid.setAt(i, false);
		}
#endif

#ifdef AUI_WARNING_FIXES
		for (uint i = 0; i < GC.getNumTerrainInfos(); i++)
#else
		for(int i = 0; i < GC.getNumTerrainInfos(); i++)
#endif
		{
			m_terrainDoubleMoveCount.setAt(i,0);
			m_terrainImpassableCount.setAt(i,0);
			m_extraTerrainAttackPercent.setAt(i,0);
			m_extraTerrainDefensePercent.setAt(i,0);
		}

		CvAssertMsg((0 < GC.getNumFeatureInfos()), "GC.getNumFeatureInfos() is not greater than zero but a float array is being allocated in CvUnit::reset");
		m_featureDoubleMoveCount.clear();
		m_featureImpassableCount.clear();
		m_extraFeatureDefensePercent.clear();
		m_extraFeatureAttackPercent.clear();

		m_featureDoubleMoveCount.resize(GC.getNumFeatureInfos());
		m_featureImpassableCount.resize(GC.getNumFeatureInfos());
		m_extraFeatureDefensePercent.resize(GC.getNumFeatureInfos());
		m_extraFeatureAttackPercent.resize(GC.getNumFeatureInfos());

#ifdef AUI_WARNING_FIXES
		for (uint i = 0; i < GC.getNumFeatureInfos(); i++)
#else
		for(int i = 0; i < GC.getNumFeatureInfos(); i++)
#endif
		{
			m_featureDoubleMoveCount.setAt(i,0);
			m_featureImpassableCount.setAt(i,0);
			m_extraFeatureAttackPercent.setAt(i,0);
			m_extraFeatureDefensePercent.setAt(i,0);
		}

		CvAssertMsg((0 < GC.getNumUnitCombatClassInfos()), "GC.getNumUnitCombatClassInfos() is not greater than zero but an array is being allocated in CvUnit::reset");
		m_extraUnitCombatModifier.clear();
		m_extraUnitCombatModifier.resize(GC.getNumUnitCombatClassInfos());
#ifdef AUI_WARNING_FIXES
		for (uint i = 0; i < GC.getNumUnitCombatClassInfos(); i++)
#else
		for(int i = 0; i < GC.getNumUnitCombatClassInfos(); i++)
#endif
		{
			m_extraUnitCombatModifier.setAt(i,0);
		}

		m_unitClassModifier.clear();
		m_unitClassModifier.resize(GC.getNumUnitClassInfos());
#ifdef AUI_WARNING_FIXES
		for (uint i = 0; i < GC.getNumUnitClassInfos(); i++)
#else
		for(int i = 0; i < GC.getNumUnitClassInfos(); i++)
#endif
		{
			CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo((UnitClassTypes)i);
			if(!pkUnitClassInfo)
			{
				continue;
			}

			m_unitClassModifier.setAt(i,0);
		}


		m_iMissionAIX = INVALID_PLOT_COORD;
		m_iMissionAIY = INVALID_PLOT_COORD;
		m_eMissionAIType = NO_MISSIONAI;
		m_missionAIUnit.reset();

		m_eUnitAIType = NO_UNITAI;
	}
}





void CvUnit::setupGraphical()
{
	VALIDATE_OBJECT
	if(!GC.IsGraphicsInitialized())
	{
		return;
	}

	ICvEngineUtility1* pDLL = GC.getDLLIFace();
	auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));

	if(isEmbarked())
	{
		pDLL->GameplayUnitEmbark(pDllUnit.get(), true);
	}
	else
	{
		pDLL->GameplayUnitCreated(pDllUnit.get());

		pDLL->GameplayUnitShouldDimFlag(pDllUnit.get(),          getMoves() <= 0);

		if(getFortifyTurns() > 0)
		{
			pDLL->GameplayUnitFortify(pDllUnit.get(), true);
		}

		int iNewValue = getDamage();
		if(iNewValue > 0)
		{
			pDLL->GameplayUnitSetDamage(pDllUnit.get(), iNewValue, 0);
		}

		if(IsWork())
		{
			BuildTypes currentBuild = getBuildType();
			if(currentBuild != NO_BUILD)
			{
				pDLL->GameplayUnitWork(pDllUnit.get(), currentBuild);
			}
		}

		if(IsGarrisoned())
		{
			pDLL->GameplayUnitVisibility(pDllUnit.get(), false);
			pDLL->GameplayUnitGarrison(pDllUnit.get(), true);
		}
	}
}


void CvUnit::initPromotions()
{
	VALIDATE_OBJECT

	m_Promotions.Init(GC.GetGamePromotions(), this);
}


void CvUnit::uninitInfos()
{
	VALIDATE_OBJECT
	m_featureDoubleMoveCount.clear();
	m_terrainImpassableCount.clear();
	m_featureImpassableCount.clear();
	m_extraTerrainAttackPercent.clear();
	m_extraTerrainDefensePercent.clear();
	m_extraFeatureAttackPercent.clear();
	m_extraFeatureDefensePercent.clear();
	m_extraUnitCombatModifier.clear();
	m_unitClassModifier.clear();
}



void CvUnit::convert(CvUnit* pUnit, bool bIsUpgrade)
{
	VALIDATE_OBJECT
	IDInfo* pUnitNode;
	CvUnit* pTransportUnit;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;

	pPlot = plot();


#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumPromotionInfos(); iI++)
#else
	for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
#endif
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(iI);
		CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
		if(pkPromotionInfo)
		{
			bool bGivePromotion = false;
#if defined(LEKMOD_RETRAIN_MISSION)
			bool bWasChosen = false;

			if (pUnit->isHasPromotion(ePromotion) && pUnit->IsPromotionChosenByPlayer(ePromotion))
			{
				bWasChosen = true;
			}
#endif

			if(pUnit->isHasPromotion(ePromotion) && !pkPromotionInfo->IsLostWithUpgrade())
			{
				bGivePromotion = true;
			}


			else if(getUnitInfo().GetFreePromotions(ePromotion) && (!bIsUpgrade || !pkPromotionInfo->IsNotWithUpgrade()))
			{
				bGivePromotion = true;
			}


			else if(GET_PLAYER(getOwner()).IsFreePromotion(ePromotion) && (
			            ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForCivilianUnitType(ePromotion, getUnitType())))
			{
				bGivePromotion = true;
			}
#if defined(LEKMOD_HELICOPTER_EMBARK_FIX)

			if (ePromotion == GET_PLAYER(getOwner()).GetEmbarkationPromotion() && getUnitInfo().GetDomainType() == DOMAIN_HOVER)
			{
				bGivePromotion = false;
			}
#endif
			setHasPromotion(ePromotion, bGivePromotion);
#if defined(LEKMOD_RETRAIN_MISSION)
			if (bWasChosen)
			{
				SetPromotionChosenByPlayer(ePromotion, bGivePromotion);
			}
#endif
		}
	}

	setGameTurnCreated(pUnit->getGameTurnCreated());
	setLastMoveTurn(pUnit->getLastMoveTurn());
	setDamage(pUnit->getDamage());
	setMoves(pUnit->getMoves());
#if !defined(LEKMOD_HELICOPTER_EMBARK_FIX)
	setEmbarked(pUnit->isEmbarked());
#else
	setEmbarked((getUnitInfo().GetDomainType() == DOMAIN_HOVER) ? false : pUnit->isEmbarked());
#endif
	setFacingDirection(pUnit->getFacingDirection(false));
	SetBeenPromotedFromGoody(pUnit->IsHasBeenPromotedFromGoody());
	SetTourismBlastStrength(pUnit->GetTourismBlastStrength());
#ifdef AUI_UNIT_FIX_GIFTED_UNITS_ARE_GIFTED_NOT_CLONED
	SetResearchBulbAmount(pUnit->GetResearchBulbAmount());
	SetGreatWork(pUnit->GetGreatWork());
	m_iAttacksMade = pUnit->m_iAttacksMade;
	m_iMadeInterceptionCount = pUnit->m_iMadeInterceptionCount;
	setSetUpForRangedAttack(pUnit->isSetUpForRangedAttack());

	if (pUnit->getOwner() != pUnit->GetOriginalOwner())
	{
		SetOriginalOwner(pUnit->GetOriginalOwner());
	}

	SetImmobile(pUnit->IsImmobile());

	GetReligionData()->SetReligion(pUnit->GetReligionData()->GetReligion());
	GetReligionData()->SetSpreadsLeft(pUnit->GetReligionData()->GetSpreadsLeft());
	GetReligionData()->SetReligiousStrength(pUnit->GetReligionData()->GetReligiousStrength());
#endif
#if defined(NQM_UNIT_FIX_NO_DOUBLE_INSTAHEAL_ON_SAME_TURN) || defined(NQM_UNIT_FIX_NO_INSTAHEAL_AFTER_PARADROP)
	setCanInstahealThisTurn(pUnit->canInstahealThisTurn());
#endif
#if defined(NQM_UNIT_NO_AA_INTERCEPT_AFTER_MOVE_BEFORE_TURN_END) || defined(NQM_UNIT_FIGHTER_NO_INTERCEPT_UNTIL_AFTER_TURN_END)
	setIsInterceptBlockedUntilEndTurn(pUnit->isInterceptBlockedUntilEndTurn());
#endif

#ifdef GIFTED_UNITS_ATTACK
	m_iAttacksMade = pUnit->m_iAttacksMade;
#if defined(v35_TRAITIFY)
	m_bAttackedLastTurn = pUnit->m_bAttackedLastTurn;
	m_bKilledUnit = pUnit->m_bKilledUnit;
	m_bKilledUnitLastTurn = pUnit->m_bKilledUnitLastTurn;
#endif
#endif

	if (pUnit->getUnitInfo().GetNumExoticGoods() > 0)
	{
		setNumExoticGoods(pUnit->getNumExoticGoods());
	}

	setLevel(pUnit->getLevel());
	int iOldModifier = std::max(1, 100 + GET_PLAYER(pUnit->getOwner()).getLevelExperienceModifier());
	int iOurModifier = std::max(1, 100 + GET_PLAYER(getOwner()).getLevelExperienceModifier());
	setExperience(std::max(0, (pUnit->getExperience() * iOurModifier) / iOldModifier));

	setName(pUnit->getNameNoDesc());
	setLeaderUnitType(pUnit->getLeaderUnitType());
	ChangeNumGoodyHutsPopped(pUnit->GetNumGoodyHutsPopped());

	pTransportUnit = pUnit->getTransportUnit();

	if(pTransportUnit != NULL)
	{
		pUnit->setTransportUnit(NULL);
		setTransportUnit(pTransportUnit);
	}

	pUnitNode = pPlot->headUnitNode();

	while(pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if(pLoopUnit && pLoopUnit->getTransportUnit() == pUnit)
		{
			pLoopUnit->setTransportUnit(this);
		}
	}

#ifdef AUI_DLLNETMESSAGEHANDLER_FIX_RESPAWN_PROPHET_IF_BEATEN_TO_LAST_RELIGION
	SetIgnoreExpended(pUnit->IsIgnoreExpended());
#endif

	pUnit->kill(true);
}






void CvUnit::kill(bool bDelay, PlayerTypes ePlayer                )
{
	VALIDATE_OBJECT
	CvUnit* pTransportUnit;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;
	CvString strBuffer;

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(getOwner());
		args->Push(GetID());

		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "CanSaveUnit", args.get(), bResult))
		{

			if(bResult)
			{
				return;
			}
		}
	}

#ifdef ENHANCED_GRAPHS
	if (getUnitCombatType() != NO_UNITCOMBAT && ePlayer != NO_PLAYER && ePlayer != BARBARIAN_PLAYER)
	{
		GET_PLAYER(ePlayer).ChangeNumKilledUnits(1);
		GET_PLAYER(getOwner()).ChangeNumLostUnits(1);
	}
#endif

	auto_ptr<ICvUnit1> pDllThisUnit = GC.WrapUnitPointer(this);

	if(IsSelected() && !bDelay)
	{
		DLLUI->VisuallyDeselectUnit(pDllThisUnit.get());
	}

	GET_PLAYER(getOwner()).removeFromArmy(m_iArmyId, GetID());

	pPlot = plot();
	CvAssertMsg(pPlot != NULL, "Plot is not assigned a valid value");

	if(pPlot)
	{
		IDInfo* pkOldUnits = NULL;
		uint uiOldUnitCount = pPlot->getNumUnits();
		const IDInfo* pUnitNode = pPlot->headUnitNode();

		if(pUnitNode)
		{
			pkOldUnits = (IDInfo*)_malloca(pPlot->getNumUnits() * sizeof(IDInfo));
			IDInfo* pkEntry = pkOldUnits;
			while(pUnitNode != NULL)
			{
				*pkEntry++ = *pUnitNode;
				pUnitNode = pPlot->nextUnitNode(pUnitNode);
			}
#ifndef AUI_WARNING_FIXES
		}
#endif

		for(uint i = 0; i < uiOldUnitCount; i++)
		{
			pLoopUnit = ::getUnit(pkOldUnits[i]);

			if(pLoopUnit != NULL)
			{
				if(pLoopUnit->getTransportUnit() == this)
				{
					if(pLoopUnit->isSuicide() && pLoopUnit->isInCombat())
					{

						pLoopUnit->setTransportUnit(NULL);
					}
					else
					{
						if(pPlot->isValidDomainForLocation(*pLoopUnit))
						{
							pLoopUnit->setCapturingPlayer(getCapturingPlayer());
						}

						pLoopUnit->kill(false, ePlayer);
					}
				}
			}
		}

#ifndef AUI_WARNING_FIXES
		if(pkOldUnits)
#endif
			_freea(pkOldUnits);
#ifdef AUI_WARNING_FIXES
		}
#endif
	}

	if(ePlayer != NO_PLAYER)
	{
		if(!isBarbarian() && !GET_PLAYER(ePlayer).isBarbarian())
		{


			int iValue = getUnitInfo().GetPower();

			int iTypicalPower = GET_PLAYER(ePlayer).GetMilitaryAI()->GetPowerOfStrongestBuildableUnit(DOMAIN_LAND);

			if(iTypicalPower > 0)
			{
				iValue = iValue*         GC.getDEFAULT_WAR_VALUE_FOR_UNIT() / iTypicalPower;
			}
			else
			{
				iValue =         GC.getDEFAULT_WAR_VALUE_FOR_UNIT();
			}


			GET_PLAYER(getOwner()).GetDiplomacyAI()->ChangeWarValueLost(ePlayer, iValue);

			GET_PLAYER(ePlayer).GetDiplomacyAI()->ChangeOtherPlayerWarValueLost(getOwner(), ePlayer, iValue);
		}

		if(NO_UNIT != getLeaderUnitType())
		{
			for(int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if(GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_GENERAL_KILLED", getNameKey());
					DLLUI->AddUnitMessage(0, GetIDInfo(), ((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), strBuffer, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_MAJOR_EVENT);
				}
			}
		}
	}

	if (pkScriptSystem) 
	{
		CvLuaArgsHandle args;
		args->Push(((int)getOwner()));
		args->Push(GetID());
		args->Push(getUnitType());
		args->Push(getX());
		args->Push(getY());
		args->Push(bDelay);
		args->Push(ePlayer);

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "UnitPrekill", args.get(), bResult);
	}

	if(bDelay)
	{
		startDelayedDeath();
		return;
	}






	if(IsSelected())
	{
		DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
		if(DLLUI->GetLengthSelectionList() == 1)
		{
			if(!(DLLUI->isFocused()) && !(DLLUI->isCitySelection()) && !(DLLUI->isDiploOrPopupWaiting()))
			{
				GC.getGame().updateSelectionList();
			}

			if(IsSelected())
			{
				DLLUI->setCycleSelectionCounter(1);
			}
			else
			{
#ifndef REMOVE_EXCESS_CAMERA_CENTERING
				DLLUI->setDirty(SelectionCamera_DIRTY_BIT, true);
#endif
			}
		}
	}

	DLLUI->RemoveFromSelectionList(pDllThisUnit.get());
	GET_PLAYER(getOwner()).GetUnitCycler().RemoveUnit(GetID());



	CvAssertMsg_Debug(!isInCombat(), "isCombat did not return false as expected");

	clearCombat();

	pTransportUnit = getTransportUnit();

	if(pTransportUnit != NULL)
	{
		setTransportUnit(NULL);
	}

	setReconPlot(NULL);

	CvAssertMsg(getAttackPlot() == NULL, "The current unit instance's attack plot is expected to be NULL");
	CvAssertMsg(getCombatUnit() == NULL, "The current unit instance's combat unit is expected to be NULL");

	GET_TEAM(getTeam()).changeUnitClassCount((UnitClassTypes)getUnitInfo().GetUnitClassType(), -1);
	GET_PLAYER(getOwner()).changeUnitClassCount((UnitClassTypes)getUnitInfo().GetUnitClassType(), -1);


	if(getUnitInfo().GetWorkRate() > 0 && getUnitInfo().GetDomainType() == DOMAIN_LAND)
	{
		GET_PLAYER(getOwner()).ChangeNumBuilders(-1);
	}


	if(getUnitInfo().GetUnhappiness() != 0)
	{
		GET_PLAYER(getOwner()).ChangeUnhappinessFromUnits(-getUnitInfo().GetUnhappiness());
	}

	GET_PLAYER(getOwner()).changeExtraUnitCost(-(getUnitInfo().GetExtraMaintenanceCost()));

	if(getUnitInfo().GetNukeDamageLevel() != -1)
	{
		GET_PLAYER(getOwner()).changeNumNukeUnits(-1);
	}

	if(getUnitInfo().IsMilitarySupport())
	{
		GET_PLAYER(getOwner()).changeNumMilitaryUnits(-1);
	}


	if(getExperience() > 0 && ePlayer != NO_PLAYER)
	{
		int iGreatGeneralMeterLoss = getExperience() *        GC.getUNIT_DEATH_XP_GREAT_GENERAL_LOSS();
		iGreatGeneralMeterLoss /= 100;

		if(getDomainType() == DOMAIN_SEA)
		{
			GET_PLAYER(getOwner()).changeNavalCombatExperience(-iGreatGeneralMeterLoss);
		}
		else
		{
			GET_PLAYER(getOwner()).changeCombatExperience(-iGreatGeneralMeterLoss);
		}
	}

	CvUnitCaptureDefinition kCaptureDef;
	getCaptureDefinition(&kCaptureDef);

	setXY(INVALID_PLOT_COORD, INVALID_PLOT_COORD, true);
	if(pPlot)
		pPlot->removeUnit(this, false);


#ifdef AUI_WARNING_FIXES
	for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
	{
		if(getUnitInfo().GetResourceQuantityRequirement(iResourceLoop) > 0)
		{
			GET_PLAYER(getOwner()).changeNumResourceUsed((ResourceTypes) iResourceLoop, -getUnitInfo().GetResourceQuantityRequirement(iResourceLoop));
		}
	}




	GET_PLAYER(getOwner()).deleteUnit(GetID());


	GET_PLAYER(kCaptureDef.eOldPlayer).UpdateUnitProductionMaintenanceMod();


	CvUnit::createCaptureUnit(kCaptureDef);

	if(GC.getGame().getActivePlayer() == kCaptureDef.eOldPlayer)
	{
		CvMap& theMap = GC.getMap();
		theMap.updateDeferredFog();
	}



}



bool CvUnit::getCaptureDefinition(CvUnitCaptureDefinition* pkCaptureDef, PlayerTypes eCapturingPlayer                  )
{
	CvUnitCaptureDefinition kCaptureDef;
	kCaptureDef.eOldPlayer = getOwner();
	kCaptureDef.eOriginalOwner = GetOriginalOwner();
	kCaptureDef.eOldType = getUnitType();
	kCaptureDef.eCapturingPlayer = (eCapturingPlayer != NO_PLAYER) ? eCapturingPlayer : getCapturingPlayer();
	kCaptureDef.bEmbarked = m_bEmbarked;
	kCaptureDef.eCaptureUnitType = NO_UNIT;
	if (GetReligionData())
	{
		kCaptureDef.eReligion = GetReligionData()->GetReligion();
		kCaptureDef.iReligiousStrength = GetReligionData()->GetReligiousStrength();
		kCaptureDef.iSpreadsLeft = GetReligionData()->GetSpreadsLeft();
	}


	if(IsCapturedAsIs())
	{
		kCaptureDef.bAsIs = true;
		kCaptureDef.eCaptureUnitType = getUnitType();
	}


	else if(isBarbarian() || (kCaptureDef.eCapturingPlayer != NO_PLAYER && GET_PLAYER(kCaptureDef.eCapturingPlayer).isBarbarian()))
	{

		if(kCaptureDef.eCapturingPlayer != NO_PLAYER && getCaptureUnitType(GET_PLAYER(kCaptureDef.eCapturingPlayer).getCivilizationType()) != NO_UNIT)

			kCaptureDef.eCaptureUnitType = getUnitType();
	}


	else
	{
		if(kCaptureDef.eCapturingPlayer != NO_PLAYER)
			kCaptureDef.eCaptureUnitType = getCaptureUnitType(GET_PLAYER(kCaptureDef.eCapturingPlayer).getCivilizationType());
	}

	CvPlot* pkPlot = plot();
	if(pkPlot)
	{
		kCaptureDef.iX = pkPlot->getX();
		kCaptureDef.iY = pkPlot->getY();
	}
	else
	{
		kCaptureDef.iX = INVALID_PLOT_COORD;
		kCaptureDef.iY = INVALID_PLOT_COORD;
	}

	if(pkCaptureDef)
		*pkCaptureDef = kCaptureDef;

	return kCaptureDef.eCaptureUnitType != NO_UNIT && kCaptureDef.eCapturingPlayer != NO_PLAYER;
}









             CvUnit* CvUnit::createCaptureUnit(const CvUnitCaptureDefinition& kCaptureDef)
{
	CvUnit* pkCapturedUnit = NULL;

	if(kCaptureDef.eCapturingPlayer != NO_PLAYER && kCaptureDef.eCaptureUnitType != NO_UNIT)
	{
		CvPlot* pkPlot = GC.getMap().plot(kCaptureDef.iX , kCaptureDef.iY);
		if(pkPlot)
		{
			CvPlayerAI& kCapturingPlayer = GET_PLAYER(kCaptureDef.eCapturingPlayer);
			if(kCapturingPlayer.isHuman() || kCapturingPlayer.AI_captureUnit(kCaptureDef.eCaptureUnitType, pkPlot) || 0 == GC.getAI_CAN_DISBAND_UNITS())
			{
				pkCapturedUnit = kCapturingPlayer.initUnit(kCaptureDef.eCaptureUnitType, kCaptureDef.iX, kCaptureDef.iY);

				if(pkCapturedUnit != NULL)
				{
					pkCapturedUnit->GetReligionData()->SetReligion(kCaptureDef.eReligion);
					pkCapturedUnit->GetReligionData()->SetReligiousStrength(kCaptureDef.iReligiousStrength);
					pkCapturedUnit->GetReligionData()->SetSpreadsLeft(kCaptureDef.iSpreadsLeft);

					pkCapturedUnit->SetOriginalOwner(kCaptureDef.eOriginalOwner);

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString szMsg;
						szMsg.Format("Captured: %s, Enemy was: %s", GC.getUnitInfo(kCaptureDef.eOldType)->GetDescription(), kCapturingPlayer.getCivilizationShortDescription());
						GET_PLAYER(kCaptureDef.eOldPlayer).GetTacticalAI()->LogTacticalMessage(szMsg, true                          );
					}

					if(kCaptureDef.bEmbarked)
					{
						auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(pkCapturedUnit));
						gDLL->GameplayUnitEmbark(pDllUnit.get(), true);
						pkCapturedUnit->setEmbarked(true);
						if (!pkCapturedUnit->jumpToNearestValidPlot())
						{
							pkCapturedUnit->kill(true);
#ifdef AUI_PLAYER_FIX_ENSURE_NO_CS_SETTLER
							return NULL;
#else
							pkCapturedUnit = NULL;
#endif
						}
					}

#ifdef AUI_PLAYER_FIX_ENSURE_NO_CS_SETTLER
					{
						pkCapturedUnit->finishMoves();


						if (GET_PLAYER(pkCapturedUnit->getOwner()).isMinorCiv() && (pkCapturedUnit->isFound() || pkCapturedUnit->IsFoundAbroad()))
						{
							pkCapturedUnit->kill(false);
							return NULL;
#else
					bool bDisbanded = false;
					if (pkCapturedUnit != NULL)
					{
						pkCapturedUnit->finishMoves();


						if(!bDisbanded && GET_PLAYER(pkCapturedUnit->getOwner()).isMinorCiv() && (pkCapturedUnit->isFound() || pkCapturedUnit->IsFoundAbroad()))
						{
							bDisbanded = true;
							pkCapturedUnit->kill(false);
							pkCapturedUnit = NULL;
#endif
						}
					}


					if(!kCaptureDef.bAsIs)
					{
						bool bShowingHumanPopup = true;
						bool bShowingActivePlayerPopup = true;


						if(GC.getGame().getActivePlayer() != kCaptureDef.eCapturingPlayer)
						{
							bShowingActivePlayerPopup = false;
						}


						if(!GET_PLAYER(kCaptureDef.eOriginalOwner).isAlive())
							bShowingHumanPopup = false;


						else if(kCaptureDef.eOriginalOwner == kCaptureDef.eCapturingPlayer)
							bShowingHumanPopup = false;


						else if(GET_TEAM(GET_PLAYER(kCaptureDef.eOriginalOwner).getTeam()).isAtWar(kCapturingPlayer.getTeam()))
							bShowingHumanPopup = false;


						else if(!GET_TEAM(GET_PLAYER(kCaptureDef.eOriginalOwner).getTeam()).isHasMet(kCapturingPlayer.getTeam()))
							bShowingHumanPopup = false;


						if(bShowingHumanPopup && bShowingActivePlayerPopup && pkCapturedUnit)
						{
							CvPopupInfo kPopupInfo(BUTTONPOPUP_RETURN_CIVILIAN, kCaptureDef.eCapturingPlayer, kCaptureDef.eOriginalOwner, pkCapturedUnit->GetID());
							DLLUI->AddPopup(kPopupInfo);

							if (kCapturingPlayer.isLocalPlayer())
								CancelActivePlayerEndTurn();
						}


#ifdef AUI_PLAYER_FIX_ENSURE_NO_CS_SETTLER
						else if (!bShowingHumanPopup && !pkCapturedUnit->isBarbarian())
#else
						else if(!bShowingHumanPopup && !bDisbanded && pkCapturedUnit != NULL && !pkCapturedUnit->isBarbarian())
#endif
						{

							if(kCaptureDef.eCapturingPlayer != kCaptureDef.eOriginalOwner)
								kCapturingPlayer.DoCivilianReturnLogic(false, kCaptureDef.eOriginalOwner, pkCapturedUnit->GetID());
						}

						else if (kCapturingPlayer.GetPlayerTraits()->IsNoAnnexing())
						{

							if(kCaptureDef.eCapturingPlayer != kCaptureDef.eOriginalOwner)
								kCapturingPlayer.DoCivilianReturnLogic(false, kCaptureDef.eOriginalOwner, pkCapturedUnit->GetID());
						}
					}
					else
					{

#ifndef AUI_PLAYER_FIX_ENSURE_NO_CS_SETTLER
						if (pkCapturedUnit != NULL)
#endif
						{
							int iCapturedHealth = (pkCapturedUnit->GetMaxHitPoints() * GC.getCOMBAT_CAPTURE_HEALTH()) / 100;
							pkCapturedUnit->setDamage(iCapturedHealth);
						}
					}

					if(kCaptureDef.eCapturingPlayer == GC.getGame().getActivePlayer())
					{
						CvString strBuffer;
#ifdef AUI_WARNING_FIXES
						CvUnitEntry* pUnitInfo = GC.getUnitInfo(kCaptureDef.eCaptureUnitType);
						if (pUnitInfo)
						{
							if (kCaptureDef.eOriginalOwner == kCaptureDef.eCapturingPlayer)
							{

								strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_RECAPTURED_UNIT", pUnitInfo->GetTextKey());
							}
							else
							{
								strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_CAPTURED_UNIT", pUnitInfo->GetTextKey());
							}
#else
						if(kCaptureDef.eOriginalOwner == kCaptureDef.eCapturingPlayer){

							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_RECAPTURED_UNIT", GC.getUnitInfo(kCaptureDef.eCaptureUnitType)->GetTextKey());
						}
						else{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_CAPTURED_UNIT", GC.getUnitInfo(kCaptureDef.eCaptureUnitType)->GetTextKey());
#endif
						}
						DLLUI->AddUnitMessage(0, IDInfo(kCaptureDef.eCapturingPlayer, pkCapturedUnit->GetID()), kCaptureDef.eCapturingPlayer, true, GC.getEVENT_MESSAGE_TIME(), strBuffer                                                                                                                                                                            );
					}
				}
			}
		}
	}

	return pkCapturedUnit;
}


void CvUnit::doTurn()
{
	VALIDATE_OBJECT
	CvAssertMsg(!IsDead(), "isDead did not return false as expected");


	ActivityTypes eActivityType = GetActivityType();
	bool bHoldCheck = (eActivityType == ACTIVITY_HOLD) && (isHuman() || !getFortifyTurns());
	bool bHealCheck = (eActivityType == ACTIVITY_HEAL) && (!isHuman() || IsAutomated() || !IsHurt());
	bool bSentryCheck = (eActivityType == ACTIVITY_SENTRY) && SentryAlert();
	bool bInterceptCheck = eActivityType == ACTIVITY_INTERCEPT && !isHuman();

	if (bHoldCheck || bHealCheck || bSentryCheck || bInterceptCheck)	
	{
		SetActivityType(ACTIVITY_AWAKE);
	}

#ifdef PROMOTION_INSTA_HEAL_LOCKED
	if (isPromotionReady() && GET_PLAYER(getOwner()).isHuman())
	{
		setInstaHealLocked(true);
	}
#endif
#if defined(LEKMOD_SUBMERGE_MISSION)
	setHasSubmergedOrSurfacedThisTurn(false);
#endif
	testPromotionReady();

	FeatureTypes eFeature = plot()->getFeatureType();
	if(NO_FEATURE != eFeature)
	{
		if(0 != GC.getFeatureInfo(eFeature)->getTurnDamage())
		{
			changeDamage(GC.getFeatureInfo(eFeature)->getTurnDamage(), NO_PLAYER);
		}
	}

#ifndef NQM_UNIT_FIX_FORTIFY_BONUS_RECEIVED_END_OF_TURN_NOT_INSTANTLY

	if(IsFortifiedThisTurn())
	{
		changeFortifyTurns(1);
	}
#endif


	if(IsRecon())
	{
		setReconPlot(plot());
	}


	if(GetActivityType() == ACTIVITY_AWAKE)
	{
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitShouldDimFlag(pDllUnit.get(),          false);
	}


	if(GetActivityType() == ACTIVITY_SLEEP)
	{
		if(isFortifyable())
		{
			CvUnit::dispatchingNetMessage(true);
			PushMission(CvTypes::getMISSION_FORTIFY());
			CvUnit::dispatchingNetMessage(false);
			SetFortifiedThisTurn(true);
		}
	}

	doDelayedDeath();
}


bool CvUnit::isActionRecommended(int iAction)
{
	VALIDATE_OBJECT

	BuildTypes eBuild;

	if(getOwner() != GC.getGame().getActivePlayer())
	{
		return false;
	}

	CvPlot* pPlot = NULL;

	if(pPlot == NULL)
	{
		if(gDLL->shiftKey())
		{
			pPlot = LastMissionPlot();
		}
	}

	if(pPlot == NULL)
	{
		pPlot = plot();
	}

	if(GC.getActionInfo(iAction)->getMissionType() == CvTypes::getMISSION_HEAL())
	{
		if(IsHurt())
		{
			if(!hasMoved())
			{
				if((pPlot->getTeam() == getTeam()) || (healTurns(pPlot) < 4))
				{
					return true;
				}
			}
		}
	}

	if(GC.getActionInfo(iAction)->getMissionType() == CvTypes::getMISSION_FOUND())
	{
		if(canFound(pPlot))
		{
			if(pPlot->isBestAdjacentFound(getOwner()))
			{
				return true;
			}
		}
	}

	if(GC.getActionInfo(iAction)->getMissionType() == CvTypes::getMISSION_BUILD())
	{
		eBuild = ((BuildTypes)(GC.getActionInfo(iAction)->getMissionData()));
		CvAssert(eBuild != NO_BUILD);
		CvAssertMsg(eBuild < GC.getNumBuildInfos(), "Invalid Build");

		const int ciDirectiveSize = 1;
		BuilderDirective aDirective[ ciDirectiveSize ];

		GET_PLAYER(getOwner()).GetBuilderTaskingAI()->EvaluateBuilder(this, aDirective, ciDirectiveSize, false, true);

		if(aDirective[0].m_eDirective != BuilderDirective::NUM_DIRECTIVES && aDirective[0].m_eBuild == eBuild)
		{
			return true;
		}
	}

	if(GC.getActionInfo(iAction)->getCommandType() == COMMAND_PROMOTION)
	{
		return true;
	}

	return false;
}



bool CvUnit::isBetterDefenderThan(const CvUnit* pDefender, const CvUnit* pAttacker) const
{
	VALIDATE_OBJECT
	int iOurDefense;
	int iTheirDefense;

	if(pDefender == NULL)
	{
		return true;
	}

	TeamTypes eAttackerTeam = NO_TEAM;
	if(NULL != pAttacker)
	{
		eAttackerTeam = pAttacker->getTeam();
	}

	if(canCoexistWithEnemyUnit(eAttackerTeam))
	{
		return false;
	}

	if(!IsCanDefend())
	{
		return false;
	}

	if(IsCanDefend() && !(pDefender->IsCanDefend()))
	{
		return true;
	}


	if(isEmbarked() && !pDefender->isEmbarked())
	{
		return false;
	}

	if(pAttacker)
	{
		if(getDamage() >= pAttacker->GetCombatLimit() && pDefender->getDamage() < pAttacker->GetCombatLimit())
		{
			return false;
		}

		if(pDefender->getDamage() >= pAttacker->GetCombatLimit() && getDamage() < pAttacker->GetCombatLimit())
		{
			return true;
		}
	}
#if defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
	CvCombatInfo kOurCombatInfo;
	kOurCombatInfo.setUnit(BATTLE_UNIT_ATTACKER, const_cast<CvUnit*>(pAttacker));
	kOurCombatInfo.setUnit(BATTLE_UNIT_DEFENDER, const_cast<CvUnit*>(this));
	kOurCombatInfo.setPlot(plot());
	iOurDefense = GetMaxDefenseStrength(kOurCombatInfo);
#else
	iOurDefense = GetMaxDefenseStrength(plot(), pAttacker);
#endif
	if(::isWorldUnitClass(getUnitClassType()))
	{
		iOurDefense /= 2;
	}

	if(NULL == pAttacker)
	{
		if(pDefender->currInterceptionProbability() > 0)
		{
			iOurDefense *= (100 + pDefender->currInterceptionProbability());
			iOurDefense /= 100;
		}
	}
	else
	{
		if(!(pAttacker->immuneToFirstStrikes()))
		{
			iOurDefense *= ((((firstStrikes() * 2) + chanceFirstStrikes()) * ((GC.getCOMBAT_DAMAGE() * 2) / 5)) + 100);
			iOurDefense /= 100;
		}

		if(immuneToFirstStrikes())
		{
			iOurDefense *= ((((pAttacker->firstStrikes() * 2) + pAttacker->chanceFirstStrikes()) * ((GC.getCOMBAT_DAMAGE() * 2) / 5)) + 100);
			iOurDefense /= 100;
		}
	}

	iOurDefense /= (getCargo() + 1);
#if defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
	CvCombatInfo kTheirCombatInfo;
	kTheirCombatInfo.setUnit(BATTLE_UNIT_ATTACKER, const_cast<CvUnit*>(pAttacker));
	kTheirCombatInfo.setUnit(BATTLE_UNIT_DEFENDER, const_cast<CvUnit*>(pDefender));
	kTheirCombatInfo.setPlot(plot());
	iTheirDefense = pDefender->GetMaxDefenseStrength(kTheirCombatInfo);
#else
	iTheirDefense = pDefender->GetMaxDefenseStrength(plot(), pAttacker);
#endif
	if(::isWorldUnitClass(pDefender->getUnitClassType()))
	{
		iTheirDefense /= 2;
	}

	if(NULL == pAttacker)
	{
		if(currInterceptionProbability() > 0)
		{
			iTheirDefense *= (100 + currInterceptionProbability());
			iTheirDefense /= 100;
		}
	}
	else
	{
		if(!(pAttacker->immuneToFirstStrikes()))
		{
			iTheirDefense *= ((((pDefender->firstStrikes() * 2) + pDefender->chanceFirstStrikes()) * ((GC.getCOMBAT_DAMAGE() * 2) / 5)) + 100);
			iTheirDefense /= 100;
		}

		if(pDefender->immuneToFirstStrikes())
		{
			iTheirDefense *= ((((pAttacker->firstStrikes() * 2) + pAttacker->chanceFirstStrikes()) * ((GC.getCOMBAT_DAMAGE() * 2) / 5)) + 100);
			iTheirDefense /= 100;
		}
	}

	iTheirDefense /= (pDefender->getCargo() + 1);

	if(iOurDefense == iTheirDefense)
	{
		if(NO_UNIT == getLeaderUnitType() && NO_UNIT != pDefender->getLeaderUnitType())
		{
			++iOurDefense;
		}
		else if(NO_UNIT != getLeaderUnitType() && NO_UNIT == pDefender->getLeaderUnitType())
		{
			++iTheirDefense;
		}
		else if(isBeforeUnitCycle(this, pDefender))
		{
			++iOurDefense;
		}
	}

	return (iOurDefense > iTheirDefense);
}



bool CvUnit::canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible, bool bTestBusy) const
{
	VALIDATE_OBJECT

	if(bTestBusy && IsBusy())
	{
		return false;
	}

	if (GetAutomateType() == AUTOMATE_TRADE)
		return false;

	switch(eCommand)
	{
	case COMMAND_PROMOTION:
		if(canPromote((PromotionTypes)iData1, iData2))
		{
			return true;
		}
		break;

	case COMMAND_UPGRADE:
		if(CanUpgradeRightNow(bTestVisible))
		{
			return true;
		}
		break;

	case COMMAND_AUTOMATE:
		if(CanAutomate((AutomateTypes)iData1, bTestVisible))
		{
			return true;
		}
		break;

	case COMMAND_WAKE:
		if(!IsAutomated() && isWaiting())
		{
			return true;
		}
		break;

	case COMMAND_CANCEL:
	case COMMAND_CANCEL_ALL:
		if(!IsAutomated() && (GetLengthMissionQueue() > 0))
		{
			return true;
		}
		break;

	case COMMAND_STOP_AUTOMATION:
		if(IsAutomated())
		{
			return true;
		}
		break;

	case COMMAND_DELETE:
		if(canScrap(bTestVisible))
		{
			return true;
		}
		break;

	case COMMAND_GIFT:
		if(canGift(bTestVisible))
		{
			return true;
		}
		break;

	case COMMAND_HOTKEY:
		return true;
		break;

	default:
		CvAssert(false);
		break;
	}

	return false;
}



void CvUnit::doCommand(CommandTypes eCommand, int iData1, int iData2)
{
	VALIDATE_OBJECT
	bool bCycle;

	bCycle = false;

	CvAssert(getOwner() != NO_PLAYER);

	if(canDoCommand(eCommand, iData1, iData2))
	{
		switch(eCommand)
		{
		case COMMAND_PROMOTION:
			promote((PromotionTypes)iData1, iData2);
#ifdef PROMOTION_INSTA_HEAL_LOCKED
			if (getExperience() < experienceNeeded())
			{
				setInstaHealLocked(false);
			}
#endif
			break;

		case COMMAND_UPGRADE:
			DoUpgrade();
			bCycle = true;
			break;

		case COMMAND_AUTOMATE:
			Automate((AutomateTypes)iData1);
			bCycle = true;
			break;

		case COMMAND_WAKE:
			SetActivityType(ACTIVITY_AWAKE);
			if((getOwner() == GC.getGame().getActivePlayer()) && IsSelected())
			{

				DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
			}
			break;

		case COMMAND_CANCEL:
			PopMission();
			break;

		case COMMAND_CANCEL_ALL:
			ClearMissionQueue();
			break;

		case COMMAND_STOP_AUTOMATION:
			SetAutomateType(NO_AUTOMATE);
			break;

		case COMMAND_DELETE:
			scrap();
			bCycle = true;
			break;

		case COMMAND_GIFT:
			gift();
			bCycle = true;
			break;

		case COMMAND_HOTKEY:
			setHotKeyNumber(iData1);
			break;

		default:
			CvAssert(false);
			break;
		}
	}

	if(bCycle)
	{
		if(IsSelected())
		{
			DLLUI->setCycleSelectionCounter(1);
		}
	}
}


bool CvUnit::canEnterTerritory(TeamTypes eTeam, bool bIgnoreRightOfPassage, bool bIsCity, bool bIsDeclareWarMove) const
{
	VALIDATE_OBJECT

	if(eTeam == NO_TEAM)
	{
		return true;
	}

	TeamTypes eMyTeam = GET_PLAYER(getOwner()).getTeam();

	CvTeam& kMyTeam = GET_TEAM(eMyTeam);
	CvTeam& kTheirTeam = GET_TEAM(eTeam);

	if(kMyTeam.isFriendlyTerritory(eTeam))
	{
		return true;
	}

	if(isEnemy(eTeam))
	{
		return true;
	}

	if(isRivalTerritory())
	{
		return true;
	}

	if(kTheirTeam.isMinorCiv())
	{

		if(!kMyTeam.isMinorCiv())
		{


			if(!kMyTeam.isHasMet(eTeam))
			{
				return true;
			}

			if(bIsCity && bIsDeclareWarMove)
			{
				return false;
			}


			if (isHuman())
				return true;

			CvMinorCivAI* pMinorAI = GET_PLAYER(kTheirTeam.getLeaderID()).GetMinorCivAI();


			bool bAngerFreeUnit = IsAngerFreeUnit();

			bool bHasOpenBorders = pMinorAI->IsPlayerHasOpenBorders(getOwner());

			bool bIntruding = pMinorAI->IsMajorIntruding(getOwner());

			if(bAngerFreeUnit || bHasOpenBorders || bIntruding)
			{
				return true;
			}
		}
	}

	if(!bIgnoreRightOfPassage)
	{
		if(kTheirTeam.IsAllowsOpenBordersToTeam(eMyTeam))
		{
			return true;
		}
	}

	return false;
}


bool CvUnit::canEnterTerrain(const CvPlot& enterPlot, byte bMoveFlags) const
{
	VALIDATE_OBJECT
	if(enterPlot.isImpassable())
	{
		if(!(m_iCanMoveImpassableCount > 0) && !canMoveAllTerrain())
		{
			return false;
		}
	}
	
	if(enterPlot.isMountain())
	{
		CvPlayer& kPlayer = GET_PLAYER(getOwner());
		if(!kPlayer.GetPlayerTraits()->IsAbleToCrossMountains() && !IsHoveringUnit() && !canMoveAllTerrain())
		{
			return false;
		}
	}

	DomainTypes eDomain = getDomainType();



	if(eDomain == DOMAIN_IMMOBILE || m_bImmobile)
	{
		return false;
	}

	bool bAllowsWalkWater = enterPlot.IsAllowsWalkWater();
#ifdef AUI_UNIT_FIX_HOVERING_EMBARK
	bAllowsWalkWater = bAllowsWalkWater || (IsHoveringUnit() && enterPlot.isWater() && enterPlot.getTerrainType() != GC.getDEEP_WATER_TERRAIN());
#elif defined(LEKMOD_HELICOPTER_EMBARK_FIX)

	bAllowsWalkWater = bAllowsWalkWater || (IsHoveringUnit() && enterPlot.isWater());
#endif
	if (enterPlot.isWater() && (bMoveFlags & CvUnit::MOVEFLAG_STAY_ON_LAND) && !bAllowsWalkWater)
	{
		return false;
	}

	TeamTypes eTeam = getTeam();

	if(canEnterTerritory(enterPlot.getTeam(), false                          , enterPlot.getPlotCity() != NULL, bMoveFlags & MOVEFLAG_DECLARE_WAR))
	{
		if(enterPlot.getFeatureType() != NO_FEATURE && enterPlot.getRouteType() == NO_ROUTE)
		{
			if(isFeatureImpassable(enterPlot.getFeatureType()))
			{
				bool bCanPass = false;


				if(m_Promotions.GetAllowFeaturePassable(enterPlot.getFeatureType()))
				{
					return true;
				}

				if(!bCanPass)
				{
					if(DOMAIN_SEA != eDomain || enterPlot.getTeam() != eTeam)
					{
						if(!canLoad(enterPlot))
						{
							return false;
						}
					}
				}
			}
		}
		else
		{
			if(isTerrainImpassable(enterPlot.getTerrainType()))
			{
				bool bCanPass = false;


				bCanPass = m_Promotions.GetAllowTerrainPassable(enterPlot.getTerrainType());
				if(!bCanPass)
				{
					if(DOMAIN_SEA != eDomain || enterPlot.getTeam() != eTeam)
					{
						if(!canLoad(enterPlot))
						{
							return false;
						}
					}
				}
			}
		}
	}

	switch(eDomain)
	{
	case DOMAIN_SEA:
		if(enterPlot.IsAllowsSailLand()){
            return true;
        }
		if(!enterPlot.isWater() && !canMoveAllTerrain())
		{
			if(!enterPlot.isFriendlyCity(*this, true) && !enterPlot.isEnemyCity(*this))
			{
				return false;
			}
		}
		break;

	case DOMAIN_AIR:

		break;

	case DOMAIN_LAND:
	{
		if (bAllowsWalkWater)
		{
			return true;
		}

		if(bMoveFlags & MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE)
		{

			if(enterPlot.isWater() && !canMoveAllTerrain())
			{
				if(CanEverEmbark())
				{

					if(enterPlot.getTerrainType() == GC.getDEEP_WATER_TERRAIN() && !GET_TEAM(eTeam).canEmbarkAllWaterPassage() && !IsEmbarkAllWater())
					{
						return false;
					}
				}
#ifndef AUI_UNIT_FIX_HOVERING_EMBARK

				else if(IsHoveringUnit())
				{
#if !defined(LEKMOD_HELICOPTER_EMBARK_FIX)
					if(enterPlot.getTerrainType() == GC.getDEEP_WATER_TERRAIN())
					{
						return false;
					}
#endif
				}
#endif
				else
				{
					return false;
				}
			}
		}
		else
		{

			bool bEmbarked = (isEmbarked() || (bMoveFlags & MOVEFLAG_PRETEND_EMBARKED)) && !(bMoveFlags & MOVEFLAG_PRETEND_UNEMBARKED);

			if(bEmbarked)
			{
#ifdef AUI_UNIT_FIX_HOVERING_EMBARK
				if ((!IsHoveringUnit() && !enterPlot.isWater()) || (IsHoveringUnit() && enterPlot.getTerrainType() != GC.getDEEP_WATER_TERRAIN()))
#else
				if(!enterPlot.isWater())
#endif
				{
					return false;
				}

				if(enterPlot.getTerrainType() == GC.getDEEP_WATER_TERRAIN() && !GET_TEAM(eTeam).canEmbarkAllWaterPassage() && !IsEmbarkAllWater())
				{
					return false;
				}
			}
			else
			{
				if(enterPlot.isWater() && !canMoveAllTerrain() && !enterPlot.isCity())
				{

					if(IsHoveringUnit())
					{
#if !defined(LEKMOD_HELICOPTER_EMBARK_FIX)
						if(enterPlot.getTerrainType() == GC.getDEEP_WATER_TERRAIN())
						{
							return false;
						}
#endif
					}
					else if(!isHuman() || (plot() && plot()->isWater()) || !canLoad(enterPlot))
					{
						return false;
					}
				}
			}
		}
	}
	break;

	case DOMAIN_IMMOBILE:
		return false;
		break;

	default:
		CvAssert(false);
		break;
	}

	return true;
}



TeamTypes CvUnit::GetDeclareWarMove(const CvPlot& plot) const
{
	VALIDATE_OBJECT
	const CvUnit* pUnit;
	TeamTypes eRevealedTeam;

	CvAssert(isHuman());

	if(getDomainType() != DOMAIN_AIR)
	{
		eRevealedTeam = plot.getRevealedTeam(getTeam(), false);

		if(eRevealedTeam != NO_TEAM)
		{
			if(!GET_TEAM(eRevealedTeam).isMinorCiv() || plot.isCity())
			{
				if(!canEnterTerritory(eRevealedTeam, false                          , plot.isCity(), true))
				{
					if(GET_TEAM(getTeam()).canDeclareWar(plot.getTeam()))
					{
						return eRevealedTeam;
					}
				}
			}
		}

		if(plot.isActiveVisible(false))
		{
			if(canMoveInto(plot, MOVEFLAG_ATTACK | MOVEFLAG_DECLARE_WAR | MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE))
			{
				pUnit = plot.plotCheck(PUF_canDeclareWar, getOwner(), isAlwaysHostile(plot), NO_PLAYER, NO_TEAM, PUF_isVisible, getOwner());

				if(pUnit != NULL)
				{
					return pUnit->getTeam();
				}
			}
		}
	}

	return NO_TEAM;
}



PlayerTypes CvUnit::GetBullyMinorMove(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	PlayerTypes eMinor;

	CvAssert(isHuman());

	if(getDomainType() != DOMAIN_AIR)
	{
		eMinor = pPlot->getRevealedOwner(getTeam());

		if(eMinor != NO_PLAYER)
		{
			if(GET_PLAYER(eMinor).isMinorCiv())
			{
				if(!canEnterTerritory(GET_PLAYER(eMinor).getTeam(), false                          , pPlot->isCity()))
				{
					return eMinor;
				}
			}
		}
	}

	return NO_PLAYER;
}



TeamTypes CvUnit::GetDeclareWarRangeStrike(const CvPlot& plot) const
{
	VALIDATE_OBJECT
	const CvUnit* pUnit;

	CvAssert(isHuman());

	if(plot.isActiveVisible(false))
	{
		if(canRangeStrikeAt(plot.getX(), plot.getY(), false))
		{
			pUnit = plot.plotCheck(PUF_canDeclareWar, getOwner(), isAlwaysHostile(plot), NO_PLAYER, NO_TEAM, PUF_isVisible, getOwner());

			if(pUnit != NULL)
			{
				return pUnit->getTeam();
			}

			else
			{
				if(plot.isCity())
				{
					if(GET_TEAM(getTeam()).canDeclareWar(plot.getTeam()))
					{
						return plot.getTeam();
					}
				}
			}
		}
	}

	return NO_TEAM;
}


bool CvUnit::willRevealByMove(const CvPlot& plot) const
{
	VALIDATE_OBJECT
	int iVisRange = visibilityRange();
	TeamTypes eTeam = getTeam();
	int iRange = iVisRange + 1;
#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX, iDX;
	CvPlot* pLoopPlot;
	for (int iDY = -iRange; iDY <= iRange; iDY++)
	{
		iMaxDX = iRange - MAX(0, iDY);
		for (iDX = -iRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
		{

			pLoopPlot = plotXY(plot.getX(), plot.getY(), iDX, iDY);
#else
	for(int i = -iRange; i <= iRange; ++i)
	{
		for(int j = -iRange; j <= iRange; ++j)
		{
			CvPlot* pLoopPlot = ::plotXYWithRangeCheck(plot.getX(), plot.getY(), i, j, iRange);
#endif
			if(NULL != pLoopPlot)
			{
				if(!pLoopPlot->isRevealed(eTeam) && plot.canSeePlot(pLoopPlot, eTeam, iVisRange, NO_DIRECTION))
				{
					return true;
				}
			}
		}
	}

	return false;
}


#ifdef AUI_ASTAR_FIX_CAN_ENTER_TERRAIN_NO_DUPLICATE_CALLS
bool CvUnit::canMoveInto(const CvPlot& plot, byte bMoveFlags, bool bCanEnterTerrain, bool bIsPrecalcCanEnterTerrain) const
#else
bool CvUnit::canMoveInto(const CvPlot& plot, byte bMoveFlags) const
#endif
{
	VALIDATE_OBJECT
	TeamTypes ePlotTeam;

#ifdef AUI_ASTAR_FIX_CAN_ENTER_TERRAIN_NO_DUPLICATE_CALLS
	if (bIsPrecalcCanEnterTerrain && !bCanEnterTerrain)
	{
		return false;
	}
#endif
#ifdef AUI_UNIT_FIX_CAN_MOVE_INTO_OPTIMIZED

	bool bMoveFlagAttack = (bMoveFlags & MOVEFLAG_ATTACK);
#endif

	if(atPlot(plot))
	{
		return false;
	}


	if(!(bMoveFlags & MOVEFLAG_PRETEND_UNEMBARKED) && isNoRevealMap() && willRevealByMove(plot))
	{
		return false;
	}


	if(isBarbarian() && (GC.getGame().getGameTurn() < GC.getGame().GetBarbarianReleaseTurn()) && (plot.isOwned()))
	{
		return false;
	}


	if(bMoveFlags & MOVEFLAG_DESTINATION)
	{

#ifdef AUI_UNIT_FIX_CAN_MOVE_INTO_CITY_ATTACK_BLOCKER
		if (plot.isCity() && plot.isRevealed(getTeam()))
		{
#ifdef AUI_UNIT_FIX_CAN_MOVE_INTO_OPTIMIZED
			if ((bMoveFlagAttack || (bMoveFlags & MOVEFLAG_DECLARE_WAR)) == (plot.getPlotCity()->getOwner() == getOwner()))
#else
			if (((bMoveFlags & MOVEFLAG_ATTACK) || (bMoveFlags & MOVEFLAG_DECLARE_WAR)) == (plot.getPlotCity()->getOwner() == getOwner()))
#endif
				return false;
#else
#ifdef AUI_UNIT_FIX_CAN_MOVE_INTO_OPTIMIZED
		if (!bMoveFlagAttack && !(bMoveFlags & MOVEFLAG_DECLARE_WAR))
#else
		if(!(bMoveFlags & MOVEFLAG_ATTACK) && !(bMoveFlags & MOVEFLAG_DECLARE_WAR))
#endif
		{
			if(plot.isCity() && plot.getPlotCity()->getOwner() != getOwner())
				return false;
#endif
		}


		if(!(bMoveFlags & MOVEFLAG_IGNORE_STACKING) && GC.getPLOT_UNIT_LIMIT() > 0)
		{

			if(plot.isVisible(getTeam()) && plot.getNumFriendlyUnitsOfType(this) >= GC.getPLOT_UNIT_LIMIT())
			{
				return FALSE;
			}
		}
	}

#ifdef AUI_UNIT_FIX_CAN_MOVE_INTO_OPTIMIZED
	if (bMoveFlagAttack)
#else
	if(isNoCapture())
	{
		if(!(bMoveFlags & MOVEFLAG_ATTACK))
		{
			if(plot.isEnemyCity(*this))
			{
				return false;
			}
		}
	}

	if(bMoveFlags & MOVEFLAG_ATTACK)
#endif
	{
		if(isOutOfAttacks())
		{
			return false;
		}


		if(IsCityAttackOnly() && !plot.isEnemyCity(*this) && plot.getBestDefender(NO_PLAYER))
		{
			return false;
		}
	}
#ifdef AUI_UNIT_FIX_CAN_MOVE_INTO_OPTIMIZED
	else if (isNoCapture() && plot.isEnemyCity(*this))
	{
		return false;
	}
#endif


	if(plot.isEnemyCity(*this))
	{
#ifdef AUI_UNIT_FIX_CAN_MOVE_INTO_OPTIMIZED
		if (bMoveFlagAttack)
#else
		if(plot.getPlotCity()->getDamage() < plot.getPlotCity()->GetMaxHitPoints() && !(bMoveFlags & MOVEFLAG_ATTACK))
		{
			return false;
		}
		if(bMoveFlags & MOVEFLAG_ATTACK)
#endif
		{
			if(getDomainType() == DOMAIN_AIR)
				return false;
			if(isHasPromotion((PromotionTypes)GC.getPROMOTION_ONLY_DEFENSIVE()))
				return false;
		}
#ifdef AUI_UNIT_FIX_CAN_MOVE_INTO_OPTIMIZED
		else if (plot.getPlotCity()->getDamage() < plot.getPlotCity()->GetMaxHitPoints())
		{
			return false;
		}
#endif
	}

	if(getDomainType() == DOMAIN_AIR)
	{
#ifdef AUI_UNIT_FIX_CAN_MOVE_INTO_OPTIMIZED
		if (bMoveFlagAttack)
#else
		if(bMoveFlags & MOVEFLAG_ATTACK)
#endif
		{
			if(!canRangeStrikeAt(plot.getX(), plot.getY()))
			{
				return false;
			}
		}
	}
	else
	{
#ifdef AUI_UNIT_FIX_CAN_MOVE_INTO_OPTIMIZED
		if (bMoveFlagAttack)
#else
		if(bMoveFlags & MOVEFLAG_ATTACK)
#endif
		{
			if(!IsCanAttack())
			{
				return false;
			}

			if(getDomainType() == DOMAIN_LAND && plot.isWater() && !canMoveAllTerrain() && !plot.IsAllowsWalkWater()
#if defined(LEKMOD_HELICOPTER_EMBARK_FIX)

				&& !IsHoveringUnit()
#endif
				)
			{
#if defined(v35_TRAITIFY)
				if(!GET_PLAYER(getOwner()).GetPlayerTraits()->IsEmbarkedUnitsFullStrength())
#endif
					return false;
			}

			if(!isHuman() || (plot.isVisible(getTeam())))
			{

				bool bCanAdvanceOnDeadUnit = false;

				const IDInfo* pUnitNode = plot.headUnitNode();
				const CvUnit* pLoopUnit;

				bool bPlotContainsCombat = false;
				while(pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(*pUnitNode);
					CvAssertMsg(pLoopUnit, "pUnitNode data should lead to a unit");

					pUnitNode = plot.nextUnitNode(pUnitNode);


					if(pLoopUnit)
					{
						if(GET_TEAM(getTeam()).isAtWar(GET_PLAYER(pLoopUnit->getOwner()).getTeam()))
						{
							if(!pLoopUnit->IsDead() && pLoopUnit->isInCombat())
							{
								if(pLoopUnit->getCombatUnit() != this)
									bPlotContainsCombat = true;
							}

							if(pLoopUnit->IsDead() || !pLoopUnit->IsCombatUnit())
							{
								bCanAdvanceOnDeadUnit = true;
							}
							else
							{
								bCanAdvanceOnDeadUnit = false;
								break;
							}
						}
					}
				}

				if(bPlotContainsCombat)
				{
					return false;
				}

				if(!bCanAdvanceOnDeadUnit)
				{
#ifdef AUI_UNIT_FIX_CAN_MOVE_INTO_OPTIMIZED
					if (!plot.isVisibleEnemyUnit(this))
					{

						if (!plot.isEnemyCity(*this))
						{
							if (plot.isVisibleOtherUnit(getOwner()) == (plot.getPlotCity() && !isNoCapture()))
#else
					if(plot.isVisibleEnemyUnit(this) != (bMoveFlags & MOVEFLAG_ATTACK))
					{

						if(!(bMoveFlags & MOVEFLAG_ATTACK) || !plot.isEnemyCity(*this))
						{
							if(!(bMoveFlags & MOVEFLAG_DECLARE_WAR) || (plot.isVisibleOtherUnit(getOwner()) != (bMoveFlags & MOVEFLAG_ATTACK) && !((bMoveFlags & MOVEFLAG_ATTACK) && plot.getPlotCity() && !isNoCapture())))
#endif
							{
								return false;
							}
						}
					}
				}
			}

			if(plot.isVisible(getTeam()))
			{
				const UnitHandle pDefender = plot.getBestDefender(NO_PLAYER, getOwner(), this, true);
				if(pDefender)
				{
					if(pDefender->getDamage() >= GetCombatLimit())
					{
						return false;
					}


					else if(pDefender->GetBaseCombatStrength() > 0)
					{

						if(!(bMoveFlags & MOVEFLAG_NOT_ATTACKING_THIS_TURN) && !IsCanAttackWithMoveNow())
						{
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)



							if (plotDistance(getX(), getY(), plot.getX(), plot.getY()) == 1 || isRanged())
#endif
							{
								return false;
							}
						}
					}
				}
			}
		}
		else
		{
			bool bEmbarkedAndAdjacent = false;
#ifndef AUI_UNIT_FIX_RADAR
			bool bEnemyUnitPresent = false;
#endif


			if(isEmbarked() || (bMoveFlags & MOVEFLAG_PRETEND_EMBARKED))
			{
				if(plotDistance(getX(), getY(), plot.getX(), plot.getY()) == 1)
				{
					bEmbarkedAndAdjacent = true;
				}
			}

#ifdef AUI_UNIT_FIX_RADAR
			if (!isHuman() || plot.isVisible(getTeam()) || bEmbarkedAndAdjacent)
			{
				if (plot.isEnemyCity(*this))
				{
					return false;
				}
#else
			bool bPlotContainsCombat = false;
			if(plot.getNumUnits())
			{
#endif
#ifdef AUI_WARNING_FIXES
				for (uint iUnitLoop = 0; iUnitLoop < plot.getNumUnits(); iUnitLoop++)
#else
				for(int iUnitLoop = 0; iUnitLoop < plot.getNumUnits(); iUnitLoop++)
#endif
				{
					CvUnit* loopUnit = plot.getUnitByIndex(iUnitLoop);

#ifdef AUI_UNIT_FIX_RADAR
					if (loopUnit && !loopUnit->IsDead() && (GET_TEAM(getTeam()).isAtWar(loopUnit->getTeam()) || loopUnit->isAlwaysHostile(plot)) && !loopUnit->canCoexistWithEnemyUnit(getTeam()))
						return false;
#else
					if(loopUnit && GET_TEAM(getTeam()).isAtWar(plot.getUnitByIndex(iUnitLoop)->getTeam()))
					{
						bEnemyUnitPresent = true;
						if(!loopUnit->IsDead() && loopUnit->isInCombat())
						{
							if(loopUnit->getCombatUnit() != this)
								bPlotContainsCombat = true;
						}
						break;
					}
#endif
				}
			}
#ifndef AUI_UNIT_FIX_RADAR

			if(bPlotContainsCombat)
			{
				return false;
			}

			if(!isHuman() || plot.isVisible(getTeam()) || bEmbarkedAndAdjacent)
			{
				if(plot.isEnemyCity(*this))
				{
					return false;
				}

				if(plot.isVisibleEnemyUnit(this) || (bEmbarkedAndAdjacent && bEnemyUnitPresent))
				{
					return false;
				}
			}
#endif
		}

		ePlotTeam = ((isHuman()) ? plot.getRevealedTeam(getTeam()) : plot.getTeam());

		if(!canEnterTerritory(ePlotTeam, false                          , plot.isCity(), bMoveFlags & MOVEFLAG_DECLARE_WAR))
		{
			CvAssert(ePlotTeam != NO_TEAM);

			if(!(GET_TEAM(getTeam()).canDeclareWar(ePlotTeam)))
			{
				return false;
			}

			if(isHuman())
			{
				if(!(bMoveFlags & MOVEFLAG_DECLARE_WAR))
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	}


#ifdef AUI_ASTAR_FIX_CAN_ENTER_TERRAIN_NO_DUPLICATE_CALLS
	if (!bIsPrecalcCanEnterTerrain && !canEnterTerrain(plot, bMoveFlags))
#else
	if(!canEnterTerrain(plot, bMoveFlags))
#endif
	{
		return false;
	}

	return true;
}

#ifdef AUI_UNIT_FIX_CAN_MOVE_OR_ATTACK_INTO_NO_DUPLICATE_CALLS

#ifdef AUI_ASTAR_FIX_CAN_ENTER_TERRAIN_NO_DUPLICATE_CALLS
bool CvUnit::canMoveOrAttackIntoCommon(const CvPlot& plot, byte bMoveFlags, bool bCanEnterTerrain, bool bIsPrecalcCanEnterTerrain) const
#else
bool CvUnit::canMoveOrAttackIntoCommon(const CvPlot& plot, byte bMoveFlags) const
#endif
{
	VALIDATE_OBJECT
	TeamTypes ePlotTeam;

#ifdef AUI_ASTAR_FIX_CAN_ENTER_TERRAIN_NO_DUPLICATE_CALLS
	if (bIsPrecalcCanEnterTerrain && !bCanEnterTerrain)
	{
		return false;
	}
#endif

	if (atPlot(plot))
	{
		return false;
	}


	if (!(bMoveFlags & MOVEFLAG_PRETEND_UNEMBARKED) && isNoRevealMap() && willRevealByMove(plot))
	{
		return false;
	}


	if (isBarbarian() && (GC.getGame().getGameTurn() < GC.getGame().GetBarbarianReleaseTurn()) && (plot.isOwned()))
	{
		return false;
	}


	if (bMoveFlags & MOVEFLAG_DESTINATION)
	{

		if (!(bMoveFlags & MOVEFLAG_IGNORE_STACKING) && GC.getPLOT_UNIT_LIMIT() > 0)
		{

			if (plot.isVisible(getTeam()) && plot.getNumFriendlyUnitsOfType(this) >= GC.getPLOT_UNIT_LIMIT())
			{
				return FALSE;
			}
		}
	}

	if (getDomainType() != DOMAIN_AIR)
	{
		ePlotTeam = ((isHuman()) ? plot.getRevealedTeam(getTeam()) : plot.getTeam());

		if (!canEnterTerritory(ePlotTeam, false                          , plot.isCity(), bMoveFlags & MOVEFLAG_DECLARE_WAR))
		{
			CvAssert(ePlotTeam != NO_TEAM);

			if (!(GET_TEAM(getTeam()).canDeclareWar(ePlotTeam)))
			{
				return false;
			}

			if (!isHuman() || !(bMoveFlags & MOVEFLAG_DECLARE_WAR))
			{
					return false;
			}
		}
	}


#ifdef AUI_ASTAR_FIX_CAN_ENTER_TERRAIN_NO_DUPLICATE_CALLS
	if (!bIsPrecalcCanEnterTerrain && !canEnterTerrain(plot, bMoveFlags))
#else
	if (!canEnterTerrain(plot, bMoveFlags))
#endif
	{
		return false;
	}

	return true;
}


bool CvUnit::canMoveOrAttackIntoAttackOnly(const CvPlot& plot, byte bMoveFlags) const
{
	VALIDATE_OBJECT
	TeamTypes ePlotTeam;

	bool bMoveFlagAttack = (bMoveFlags & MOVEFLAG_ATTACK);


	if (bMoveFlags & MOVEFLAG_DESTINATION)
	{

		if (plot.isCity() && plot.isRevealed(getTeam()))
		{
			if ((bMoveFlagAttack || (bMoveFlags & MOVEFLAG_DECLARE_WAR)) == (plot.getPlotCity()->getTeam() == getTeam()))
				return false;
		}
	}

	if (bMoveFlagAttack)
	{
		if (isOutOfAttacks())
		{
			return false;
		}


		if (IsCityAttackOnly() && !plot.isEnemyCity(*this) && plot.getBestDefender(NO_PLAYER))
		{
			return false;
		}
	}


	if (plot.isEnemyCity(*this))
	{
		if (bMoveFlagAttack)
		{
			if (getDomainType() == DOMAIN_AIR)
				return false;
			if (isHasPromotion((PromotionTypes)GC.getPROMOTION_ONLY_DEFENSIVE()))
				return false;
		}
		else if (isNoCapture())
		{
			return false;
		}
		else if (plot.getPlotCity()->getDamage() < plot.getPlotCity()->GetMaxHitPoints())
		{
			return false;
		}
	}

	if (getDomainType() == DOMAIN_AIR)
	{
		if (bMoveFlagAttack)
		{
			if (!canRangeStrikeAt(plot.getX(), plot.getY()))
			{
				return false;
			}
		}
	}
	else
	{
		if (bMoveFlagAttack)
		{
			if (!IsCanAttack())
			{
				return false;
			}

#ifdef AUI_UNIT_FIX_HOVERING_EMBARK
			if (getDomainType() == DOMAIN_LAND && plot.isWater() && !canMoveAllTerrain() && (!IsHoveringUnit() || plot.getTerrainType() == GC.getDEEP_WATER_TERRAIN()) && !plot.IsAllowsWalkWater())
#elif defined(LEKMOD_HELICOPTER_EMBARK_FIX)
			if (getDomainType() == DOMAIN_LAND && plot.isWater() && !canMoveAllTerrain() && !IsHoveringUnit() && !plot.IsAllowsWalkWater())
#else
			if (getDomainType() == DOMAIN_LAND && plot.isWater() && !canMoveAllTerrain() && !plot.IsAllowsWalkWater())
#endif
			{
				if (!GET_PLAYER(getOwner()).GetPlayerTraits()->IsEmbarkedUnitsFullStrength())
				{
					return false;
				}
			}

			if (!isHuman() || (plot.isVisible(getTeam())))
			{

				bool bCanAdvanceOnDeadUnit = false;

				const IDInfo* pUnitNode = plot.headUnitNode();
				const CvUnit* pLoopUnit;

				bool bPlotContainsCombat = false;
				while (pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(*pUnitNode);
					CvAssertMsg(pLoopUnit, "pUnitNode data should lead to a unit");

					pUnitNode = plot.nextUnitNode(pUnitNode);


					if (pLoopUnit)
					{
						if (GET_TEAM(getTeam()).isAtWar(GET_PLAYER(pLoopUnit->getOwner()).getTeam()))
						{
							if (!pLoopUnit->IsDead() && pLoopUnit->isInCombat())
							{
								if (pLoopUnit->getCombatUnit() != this)
									bPlotContainsCombat = true;
							}

							if (pLoopUnit->IsDead() || !pLoopUnit->IsCombatUnit())
							{
								bCanAdvanceOnDeadUnit = true;
							}
							else
							{
								bCanAdvanceOnDeadUnit = false;
								break;
							}
						}
					}
				}

				if (bPlotContainsCombat)
				{
					return false;
				}

				if (!bCanAdvanceOnDeadUnit)
				{
					if (!plot.isVisibleEnemyUnit(this))
					{

						if (!plot.isEnemyCity(*this))
						{
							if (plot.isVisibleOtherUnit(getOwner()) == (plot.getPlotCity() && !isNoCapture()))
							{
								return false;
							}
						}
					}
				}
			}

			if (plot.isVisible(getTeam()))
			{
				const UnitHandle pDefender = plot.getBestDefender(NO_PLAYER, getOwner(), this, true);
				if (pDefender)
				{
					if (pDefender->getDamage() >= GetCombatLimit())
					{
						return false;
					}


					else if (pDefender->GetBaseCombatStrength() > 0)
					{

						if (!(bMoveFlags & MOVEFLAG_NOT_ATTACKING_THIS_TURN) && !IsCanAttackWithMoveNow())
						{
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)



							if (plotDistance(getX(), getY(), plot.getX(), plot.getY()) == 1 || isRanged())
#endif
							{
								return false;
							}
						}
					}
				}
			}
		}
		else
		{
			bool bEmbarkedAndAdjacent = false;


			if (isEmbarked() || (bMoveFlags & MOVEFLAG_PRETEND_EMBARKED))
			{
				if (plotDistance(getX(), getY(), plot.getX(), plot.getY()) == 1)
				{
					bEmbarkedAndAdjacent = true;
				}
			}

			if (!isHuman() || plot.isVisible(getTeam()) || bEmbarkedAndAdjacent)
			{
				if (plot.isEnemyCity(*this))
				{
					return false;
				}

#ifdef AUI_WARNING_FIXES
				for (uint iUnitLoop = 0; iUnitLoop < plot.getNumUnits(); iUnitLoop++)
#else
				for (int iUnitLoop = 0; iUnitLoop < plot.getNumUnits(); iUnitLoop++)
#endif
				{
					CvUnit* loopUnit = plot.getUnitByIndex(iUnitLoop);

					if (loopUnit && !loopUnit->IsDead() && (GET_TEAM(getTeam()).isAtWar(loopUnit->getTeam()) || loopUnit->isAlwaysHostile(plot)) && !loopUnit->canCoexistWithEnemyUnit(getTeam()))
						return false;
				}
			}
		}

		ePlotTeam = ((isHuman()) ? plot.getRevealedTeam(getTeam()) : plot.getTeam());

		if (!canEnterTerritory(ePlotTeam, false                          , plot.isCity(), bMoveFlags & MOVEFLAG_DECLARE_WAR))
		{
			CvAssert(ePlotTeam != NO_TEAM);

			if (!(GET_TEAM(getTeam()).canDeclareWar(ePlotTeam)))
			{
				return false;
			}

			if (!isHuman() || !(bMoveFlags & MOVEFLAG_DECLARE_WAR))
			{
				return false;
			}
		}
	}

	return true;
}
#endif



#ifdef AUI_ASTAR_FIX_CAN_ENTER_TERRAIN_NO_DUPLICATE_CALLS
bool CvUnit::canMoveOrAttackInto(const CvPlot& plot, byte bMoveFlags, bool bCanEnterTerrain, bool bIsPrecalcCanEnterTerrain) const
#else
bool CvUnit::canMoveOrAttackInto(const CvPlot& plot, byte bMoveFlags) const
#endif
{
	VALIDATE_OBJECT
#ifdef AUI_UNIT_FIX_CAN_MOVE_OR_ATTACK_INTO_NO_DUPLICATE_CALLS
	return ((canMoveOrAttackIntoAttackOnly(plot, bMoveFlags & ~(MOVEFLAG_ATTACK)) || canMoveOrAttackIntoAttackOnly(plot, bMoveFlags | MOVEFLAG_ATTACK)) &&
#ifdef AUI_ASTAR_FIX_CAN_ENTER_TERRAIN_NO_DUPLICATE_CALLS
		canMoveOrAttackIntoCommon(plot, bMoveFlags, bCanEnterTerrain, bIsPrecalcCanEnterTerrain));
#else
		canMoveOrAttackIntoCommon(plot, bMoveFlags));
#endif
#else
	return (canMoveInto(plot, bMoveFlags & ~(MOVEFLAG_ATTACK)) || canMoveInto(plot, bMoveFlags | MOVEFLAG_ATTACK));
#endif
}



#ifdef AUI_ASTAR_FIX_CAN_ENTER_TERRAIN_NO_DUPLICATE_CALLS
bool CvUnit::canMoveThrough(const CvPlot& plot, byte bMoveFlags, bool bCanEnterTerrain, bool bIsPrecalcCanEnterTerrain) const
{
	VALIDATE_OBJECT
	return canMoveInto(plot, bMoveFlags, bCanEnterTerrain, bIsPrecalcCanEnterTerrain);
#else
bool CvUnit::canMoveThrough(const CvPlot& plot, byte bMoveFlags) const
{
	VALIDATE_OBJECT
	return canMoveInto(plot, bMoveFlags);
#endif
}



bool CvUnit::IsAngerFreeUnit() const
{

	if(isRivalTerritory())
		return true;


	if(!IsCombatUnit())
		return true;


	if(GET_PLAYER(getOwner()).isMinorCiv())
		return true;


	if(GET_PLAYER(getOwner()).isBarbarian())
		return true;

	return false;
}
#if defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)

int CvUnit::getWoundedRatio(int iAssumeExtraDamage) const
{
	int woundedMultiplier =        GC.getWOUNDED_DAMAGE_MULTIPLIER();
	woundedMultiplier += GET_PLAYER(getOwner()).GetWoundedUnitDamageMod();
	int iWoundedRatio = GC.getMAX_HIT_POINTS() - ((getDamage() + iAssumeExtraDamage) * woundedMultiplier / 100);
	if (getDomainType() == DOMAIN_AIR)
	{
		iWoundedRatio = GC.getMAX_HIT_POINTS() - (getDamage() + iAssumeExtraDamage);
	}
	return std::max(1, iWoundedRatio);
}
#endif

int CvUnit::getCombatDamage(int iStrength, int iOpponentStrength, int iCurrentDamage, bool bIncludeRand, bool bAttackerIsCity, bool bDefenderIsCity) const
{
	VALIDATE_OBJECT

#if !defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
	int iDamageRatio;

	int iWoundedDamageMultiplier =        GC.getWOUNDED_DAMAGE_MULTIPLIER();

	if(bAttackerIsCity)
	{
		iDamageRatio = GC.getMAX_HIT_POINTS();
	}
	else
	{

		iWoundedDamageMultiplier += GET_PLAYER(getOwner()).GetWoundedUnitDamageMod();

		iDamageRatio = GC.getMAX_HIT_POINTS() - (iCurrentDamage * iWoundedDamageMultiplier / 100);
	}
#else
	int iDamageRatio = getWoundedRatio();
#endif
	int iDamage =          GC.getATTACK_SAME_STRENGTH_MIN_DAMAGE() * iDamageRatio / GC.getMAX_HIT_POINTS();

	int iRoll = 0;
	if(bIncludeRand)
	{
		iRoll =         GC.getGame().getJonRandNum(GC.getATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE(), "Unit Combat Damage");
		iRoll *= iDamageRatio;
		iRoll /= GC.getMAX_HIT_POINTS();
	}
	else
	{
		iRoll =          GC.getATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE() -1;
		iRoll *= iDamageRatio;
		iRoll /= GC.getMAX_HIT_POINTS();
		iRoll /= 2;
	}
	iDamage += iRoll;






	double fStrengthRatio = (double(iStrength) / iOpponentStrength);


	if(iOpponentStrength > iStrength)
	{
		fStrengthRatio = (double(iOpponentStrength) / iStrength);
	}

	fStrengthRatio = (fStrengthRatio + 3) / 4;
	fStrengthRatio = pow(fStrengthRatio, 4.0);
	fStrengthRatio = (fStrengthRatio + 1) / 2;

	if(iOpponentStrength > iStrength)
	{
		fStrengthRatio = 1 / fStrengthRatio;
	}

	iDamage = int(iDamage * fStrengthRatio);


	if(bAttackerIsCity)
	{
		iDamage *=        GC.getCITY_ATTACKING_DAMAGE_MOD();
		iDamage /= 100;
	}


	if(bDefenderIsCity)
	{
		iDamage *=         GC.getATTACKING_CITY_MELEE_DAMAGE_MOD();
		iDamage /= 100;
	}


	iDamage /= 100;

	iDamage = iDamage > 0 ? iDamage : 1;

	return iDamage;
}


void CvUnit::fightInterceptor(const CvPlot& pPlot)
{
	VALIDATE_OBJECT
	CvAssert(getCombatTimer() == 0);

	setAttackPlot(&pPlot, true);
}


void CvUnit::move(CvPlot& targetPlot, bool bShow)
{
	VALIDATE_OBJECT
	CvAssert(canMoveOrAttackInto(targetPlot, MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE) || isOutOfAttacks());

	CvPlot* pOldPlot = plot();
	CvAssertMsg(pOldPlot, "pOldPlot needs to have a value");

#ifdef NQM_UNIT_NO_AA_INTERCEPT_AFTER_MOVE_BEFORE_TURN_END
	setIsInterceptBlockedUntilEndTurn(true);
#endif

	bool bShouldDeductCost = true;
#ifdef NQ_FIX_MOVES_THAT_CONSUME_ALL_MOVEMENT
	int iMoveCost = targetPlot.movementCost(this, plot(), getMoves());
#else
	int iMoveCost = targetPlot.movementCost(this, plot());
#endif
#ifdef AUI_UNIT_FIX_HOVERING_EMBARK
	if (pOldPlot && CanEverEmbark())
	{
		bool bToPlotNeedEmbark = !targetPlot.IsAllowsWalkWater();
		bool bFromPlotNeedEmbark = !pOldPlot->IsAllowsWalkWater();
		if (IsHoveringUnit())
		{
			bToPlotNeedEmbark = bToPlotNeedEmbark && targetPlot.getTerrainType() == GC.getDEEP_WATER_TERRAIN();
			bFromPlotNeedEmbark = bFromPlotNeedEmbark && pOldPlot->getTerrainType() == GC.getDEEP_WATER_TERRAIN();
		}
		else
		{
			bToPlotNeedEmbark = bToPlotNeedEmbark && targetPlot.isWater();
			bFromPlotNeedEmbark = bFromPlotNeedEmbark && pOldPlot->isWater();
		}

		if (bToPlotNeedEmbark != bFromPlotNeedEmbark)
		{
			if (bFromPlotNeedEmbark)
			{
				if (isEmbarked())
				{
					if (m_unitMoveLocs.size())
						PublishQueuedVisualizationMoves();

					disembark(pOldPlot);
				}
			}
			else if (!isEmbarked() && canEmbarkOnto(*pOldPlot, targetPlot))
			{
				if (m_unitMoveLocs.size())
					PublishQueuedVisualizationMoves();

				embark(pOldPlot);
				finishMoves();
				bShouldDeductCost = false;
			}
		}
	}
#else


	if (pOldPlot && CanEverEmbark() && (targetPlot.isWater() != pOldPlot->isWater() || targetPlot.IsAllowsWalkWater() && pOldPlot->isWater() ||
		targetPlot.isWater() && pOldPlot->IsAllowsWalkWater()))
	{
#ifdef LEKMOD_WATER_IMPROVEMENT_FIX

		if(pOldPlot->isWater())
		{
			if(isEmbarked())
			{
				if (m_unitMoveLocs.size())
					PublishQueuedVisualizationMoves();

				disembark(pOldPlot);
				if (targetPlot.IsAllowsWalkWater())
				{
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)


					if (!GET_PLAYER(getOwner()).GetPlayerTraits()->IsEmbarkedToLandFlatCost())
					{
						finishMoves();
						bShouldDeductCost = false;
					}
#else
					finishMoves();
					bShouldDeductCost = false;
#endif
				}
			}
			if (!targetPlot.IsAllowsWalkWater() && targetPlot.isWater() && pOldPlot->IsAllowsWalkWater())

			{
				if (m_unitMoveLocs.size())
				PublishQueuedVisualizationMoves();

				embark(pOldPlot);
			
				changeMoves(-iMoveCost);
#ifndef LEK_EMBARK_1_MOVEMENT

#ifdef LEKMOD_TRAIT_CIVILIAN_EMBARK_ONE_MOVE
				if (!(!IsCombatUnit() && GET_PLAYER(getOwner()).GetPlayerTraits()->IsCiviliansEmbarkOneMove()))
#endif
					finishMoves();
#endif
				bShouldDeductCost = false;

			}
		}
		else
		{
			if((!isEmbarked() && canEmbarkOnto(*pOldPlot, targetPlot)))
			{
				if (m_unitMoveLocs.size())
					PublishQueuedVisualizationMoves();

				embark(pOldPlot);
				
				changeMoves(-iMoveCost);
#ifndef LEK_EMBARK_1_MOVEMENT

#ifdef LEKMOD_TRAIT_CIVILIAN_EMBARK_ONE_MOVE
				if (!(!IsCombatUnit() && GET_PLAYER(getOwner()).GetPlayerTraits()->IsCiviliansEmbarkOneMove()))
#endif
					finishMoves();
#endif

				bShouldDeductCost = false;
			}
		}
	}
#else
		if(pOldPlot->isWater() && !pOldPlot->IsAllowsWalkWater())
		{
			if(isEmbarked())
			{
				if (m_unitMoveLocs.size())
					PublishQueuedVisualizationMoves();

				disembark(pOldPlot);
			}
		}
		else
		{
			if((!isEmbarked() && canEmbarkOnto(*pOldPlot, targetPlot)))
			{
				if (m_unitMoveLocs.size())
					PublishQueuedVisualizationMoves();

				embark(pOldPlot);

				
				changeMoves(-iMoveCost);
#ifndef LEK_EMBARK_1_MOVEMENT

				finishMoves();
#endif

				bShouldDeductCost = false;
			}
		}
	}
#endif
#endif
#ifdef NQ_FIX_MOVES_THAT_CONSUME_ALL_MOVEMENT
	if (iMoveCost > getMoves())
	{
		iMoveCost = getMoves();
	}
#endif

	if(bShouldDeductCost)
		changeMoves(-iMoveCost);
	setXY(targetPlot.getX(), targetPlot.getY(), true, true, bShow && targetPlot.isVisibleToWatchingHuman(), bShow);
}



bool CvUnit::jumpToNearestValidPlot()
{
	VALIDATE_OBJECT
	CvCity* pNearestCity;
	CvPlot* pLoopPlot;
	CvPlot* pBestPlot;
	int iValue;
	int iBestValue;
#ifdef AUI_WARNING_FIXES
	uint iI;
#else
	int iI;
#endif

	CvAssertMsg(!isAttacking(), "isAttacking did not return false as expected");
	CvAssertMsg(!isFighting(), "isFighting did not return false as expected");

	pNearestCity = GC.getMap().findCity(getX(), getY(), getOwner());

	iBestValue = INT_MAX;
	pBestPlot = NULL;

	for(iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);

		if(pLoopPlot && pLoopPlot->isValidDomainForLocation(*this))
		{
			if(canMoveInto(*pLoopPlot))
			{
				if(pLoopPlot->getNumFriendlyUnitsOfType(this) < GC.getPLOT_UNIT_LIMIT())
				{

					if(canEnterTerritory(pLoopPlot->getTeam()) && (isBarbarian() || !isEnemy(pLoopPlot->getTeam(), pLoopPlot)) && !pLoopPlot->isMountain())
					{
						CvAssertMsg(!atPlot(*pLoopPlot), "atPlot(pLoopPlot) did not return false as expected");

						if((getDomainType() != DOMAIN_AIR) || pLoopPlot->isFriendlyCity(*this, true))
						{
							if(getDomainType() != DOMAIN_SEA || (pLoopPlot->isFriendlyCity(*this, true) && pLoopPlot->isCoastalLand()) || pLoopPlot->isWater())
							{
								if(pLoopPlot->isRevealed(getTeam()))
								{
									iValue = (plotDistance(getX(), getY(), pLoopPlot->getX(), pLoopPlot->getY()) * 2);

									if(pNearestCity != NULL)
									{
										iValue += plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pNearestCity->getX(), pNearestCity->getY());
									}

									if(pLoopPlot->area() != area())
									{
										iValue *= 3;
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

	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strLogString;
		if(pBestPlot != NULL)
		{
			strLogString.Format("Jump to nearest valid plot by %s , X: %d, Y: %d, From X: %d, From Y: %d", getName().GetCString(),
			                    pBestPlot->getX(), pBestPlot->getY(), getX(), getY());
			GET_PLAYER(m_eOwner).GetHomelandAI()->LogHomelandMessage(strLogString);
		}
		else
		{
			strLogString.Format("Can't find a valid plot within range. %s deleted, X: %d, Y: %d", getName().GetCString(), getX(), getY());
			GET_PLAYER(m_eOwner).GetHomelandAI()->LogHomelandMessage(strLogString);
		}
	}

	if(pBestPlot != NULL)
	{
		setXY(pBestPlot->getX(), pBestPlot->getY());
		ClearMissionQueue();
		SetActivityType(ACTIVITY_AWAKE);
	}
	else
	{
		return false;
	}

	return true;
}



bool CvUnit::jumpToNearestValidPlotWithinRange(int iRange)
{
	VALIDATE_OBJECT
	CvPlot* pLoopPlot;
	CvPlot* pBestPlot;
	int iValue;
	int iBestValue;

	CvAssertMsg(!isAttacking(), "isAttacking did not return false as expected");
	CvAssertMsg(!isFighting(), "isFighting did not return false as expected");

	iBestValue = INT_MAX;
	pBestPlot = NULL;

#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX, iDX;
	for (int iDY = -iRange; iDY <= iRange; iDY++)
	{
		iMaxDX = iRange - MAX(0, iDY);
		for (iDX = -iRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
		{

			pLoopPlot = plotXY(getX(), getY(), iDX, iDY);
#else
	for(int iDX = -iRange; iDX <= iRange; iDX++)
	{
		for(int iDY = -iRange; iDY <= iRange; iDY++)
		{
			pLoopPlot	= plotXYWithRangeCheck(getX(), getY(), iDX, iDY, iRange);
#endif

			if(pLoopPlot != NULL)
			{
				if(pLoopPlot->isValidDomainForLocation(*this))
				{
					if(canMoveInto(*pLoopPlot))
					{
						if(pLoopPlot->getNumFriendlyUnitsOfType(this) < GC.getPLOT_UNIT_LIMIT())
						{

							if(canEnterTerritory(pLoopPlot->getTeam()) && (isBarbarian() || !isEnemy(pLoopPlot->getTeam(), pLoopPlot)))
							{
								CvAssertMsg(!atPlot(*pLoopPlot), "atPlot(pLoopPlot) did not return false as expected");

								if((getDomainType() != DOMAIN_AIR) || pLoopPlot->isFriendlyCity(*this, true))
								{
									if(pLoopPlot->isRevealed(getTeam()))
									{
#ifdef AUI_FIX_HEX_DISTANCE_INSTEAD_OF_PLOT_DISTANCE
										iValue = (hexDistance(iDX, iDY) * 2);
#else
										iValue = (plotDistance(getX(), getY(), pLoopPlot->getX(), pLoopPlot->getY()) * 2);
#endif

										if(pLoopPlot->area() != area())
										{
											iValue *= 3;
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
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Jump to nearest valid plot within range by %s , X: %d, Y: %d, From X: %d, From Y: %d", getName().GetCString(),
			                    pBestPlot->getX(), pBestPlot->getY(), getX(), getY());
			GET_PLAYER(m_eOwner).GetHomelandAI()->LogHomelandMessage(strLogString);
		}
		setXY(pBestPlot->getX(), pBestPlot->getY(), false, true, pBestPlot->isVisibleToWatchingHuman(), false);
	}
	else
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Can't find a valid plot within range for %s, X: %d, Y: %d", getName().GetCString(), getX(), getY());
			GET_PLAYER(m_eOwner).GetHomelandAI()->LogHomelandMessage(strLogString);
		}
		return false;
	}

	return true;
}



bool CvUnit::CanAutomate(AutomateTypes eAutomate, bool bTestVisibility) const
{
	VALIDATE_OBJECT
	if(eAutomate == NO_AUTOMATE)
	{
		return false;
	}

#ifdef DISABLE_AUTOMOVES
	if (GC.getGame().isNetworkMultiPlayer() && GC.getGame().isOption("GAMEOPTION_REMOVE_AUTO_EXPLORE") && isHuman())
	{
		return false;
	}
#endif

	switch(eAutomate)
	{
	case AUTOMATE_BUILD:
		if((AI_getUnitAIType() != UNITAI_WORKER) && (AI_getUnitAIType() != UNITAI_WORKER_SEA))
		{
			return false;
		}

		if(!bTestVisibility)
		{
			if(GC.getUNIT_WORKER_AUTOMATION_DISABLED() == 1)
			{
				return false;
			}
		}

		break;

	case AUTOMATE_EXPLORE:
		if((GetBaseCombatStrength(true) == 0) || (getDomainType() == DOMAIN_AIR) || (getDomainType() == DOMAIN_IMMOBILE))
		{
			return false;
		}

		if(!bTestVisibility)
		{
			if(!GET_PLAYER(m_eOwner).GetHomelandAI()->IsAnyValidExploreMoves(this))
			{
				return false;
			}
		}

		if (GC.getUNIT_AUTO_EXPLORE_FULL_DISABLED() == 1)
		{
			return false;
		}

		if(!bTestVisibility)
		{
			if(GC.getUNIT_AUTO_EXPLORE_DISABLED() == 1)
			{
				return false;
			}
		}
		break;

	default:
		CvAssert(false);
		break;
	}

	return true;
}



void CvUnit::Automate(AutomateTypes eAutomate)
{
	VALIDATE_OBJECT
	if(!CanAutomate(eAutomate))
	{
		return;
	}

	SetAutomateType(eAutomate);
}



bool CvUnit::canScrap(bool bTestVisible) const
{
	VALIDATE_OBJECT
	if(plot()->isUnitFighting())
	{
		return false;
	}

	if(!canMove())
	{
		return false;
	}

	if(!bTestVisible)
	{
		if(GC.getUNIT_DELETE_DISABLED() == 1)
		{
			return false;
		}
	}


	return true;
}



void CvUnit::scrap()
{
	VALIDATE_OBJECT
	if(!canScrap())
	{
		return;
	}

	if(plot()->getOwner() == getOwner())
	{
		int iGold = GetScrapGold();
		GET_PLAYER(getOwner()).GetTreasury()->ChangeGold(iGold);
	}

	kill(true);
}



int CvUnit::GetScrapGold() const
{
	int iNumGold = getUnitInfo().GetProductionCost();

	if(iNumGold <= 0)
		return 0;

	iNumGold *=        GC.getDISBAND_UNIT_REFUND_PERCENT();
	iNumGold /= 100;


	iNumGold *= 100 * (GC.getMAX_HIT_POINTS() - getDamage()) / GC.getMAX_HIT_POINTS();
	iNumGold /= 100;




	iNumGold *= GC.getGame().getGameSpeedInfo().getTrainPercent();
	iNumGold /= 100;



	CvPlot* pPlot = plot();
	if(pPlot)
	{
		const IDInfo* pUnitNode = pPlot->headUnitNode();
		while(pUnitNode)
		{
			const CvUnit* pLoopUnit = ::getUnit(*pUnitNode);

			if(pLoopUnit != NULL)
			{
				if(pLoopUnit->getTransportUnit() == this)
				{
					iNumGold += pLoopUnit->GetScrapGold();
				}
			}

			pUnitNode = pPlot->nextUnitNode(pUnitNode);
		}
	}

	return iNumGold;
}


bool CvUnit::canGift(bool bTestVisible, bool bTestTransport) const
{
	VALIDATE_OBJECT
	const CvPlot* pPlot = plot();
	const CvUnit* pTransport = getTransportUnit();

	if(isDelayedDeath())
		return false;

	if(!(pPlot->isOwned()))
	{
		return false;
	}

	if(pPlot->getOwner() == getOwner())
	{
		return false;
	}

	if(pPlot->isVisibleEnemyUnit(this))
	{
		return false;
	}

	if(pPlot->isVisibleEnemyUnit(pPlot->getOwner()))
	{
		return false;
	}

	if(!pPlot->isValidDomainForLocation(*this) && NULL == pTransport)
	{
		return false;
	}


	if(GET_PLAYER(pPlot->getOwner()).isMinorCiv())
	{

		if(isFound() || IsFoundAbroad())
			return false;


		UnitClassTypes eScoutClass = (UnitClassTypes) GC.getInfoTypeForString("UNITCLASS_SCOUT", true);
		if (eScoutClass != NO_UNITCLASS && eScoutClass == getUnitClassType())
			return false;


		if(!IsCombatUnit())
		{
			CvPlayer& kPlayer = GET_PLAYER(m_eOwner);


			if(kPlayer.GetPlayerTraits()->GetGreatPersonGiftInfluence() == 0 || !IsGreatPerson())
			{
				return false;
			}
		}
	}
#ifdef NQ_NO_GIFTING_GREAT_PEOPLE_TO_MAJORS

	else 
	{
		if (IsGreatPerson())
		{
			return false;
		}
	}
#endif


	if (getUnitInfo().IsSpreadReligion() || getUnitInfo().IsRemoveHeresy())
	{
			return false;
	}



	if(bTestTransport)
	{
		if(pTransport && pTransport->getTeam() != pPlot->getTeam())
		{
			return false;
		}
	}

	if(!bTestVisible)
	{
		if(GET_TEAM(pPlot->getTeam()).isUnitClassMaxedOut(getUnitClassType(), GET_TEAM(pPlot->getTeam()).getUnitClassMaking(getUnitClassType())))
		{
			return false;
		}

		if(GET_PLAYER(pPlot->getOwner()).isUnitClassMaxedOut(getUnitClassType(), GET_PLAYER(pPlot->getOwner()).getUnitClassMaking(getUnitClassType())))
		{
			return false;
		}
	}

	return !atWar(pPlot->getTeam(), getTeam());
}



void CvUnit::gift(bool bTestTransport)
{
	VALIDATE_OBJECT
	IDInfo* pUnitNode;
	CvUnit* pGiftUnit;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;
	CvString strBuffer;
	PlayerTypes eOwner;

	if(!canGift(false, bTestTransport))
	{
		return;
	}

	pPlot = plot();

	pUnitNode = pPlot->headUnitNode();

	while(pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(*pUnitNode);

		if(pLoopUnit && pLoopUnit->getTransportUnit() == this)
		{
			pLoopUnit->kill(false);
			pUnitNode = pPlot->headUnitNode();
		}
		else
		{
			pUnitNode = pPlot->nextUnitNode(pUnitNode);
		}
	}

	CvAssertMsg(plot()->getOwner() != NO_PLAYER, "plot()->getOwner() is not expected to be equal with NO_PLAYER");
#ifdef AUI_UNIT_FIX_GIFTED_UNITS_ARE_GIFTED_NOT_CLONED
	pGiftUnit = GET_PLAYER(plot()->getOwner()).initUnit(getUnitType(), getX(), getY(), AI_getUnitAIType(), NO_DIRECTION, false, false, DEFAULT_UNIT_MAP_LAYER, 0, true);
#else
	pGiftUnit = GET_PLAYER(plot()->getOwner()).initUnit(getUnitType(), getX(), getY(), AI_getUnitAIType(), NO_DIRECTION, false, false);
#endif

	CvAssertMsg(pGiftUnit != NULL, "GiftUnit is not assigned a valid value");

	if(pGiftUnit != NULL)
	{
		eOwner = getOwner();

		pGiftUnit->convert(this, false);
		pGiftUnit->setupGraphical();

#ifndef AUI_UNIT_FIX_GIFTED_UNITS_ARE_GIFTED_NOT_CLONED
		pGiftUnit->GetReligionData()->SetReligion(GetReligionData()->GetReligion());
		pGiftUnit->GetReligionData()->SetReligiousStrength(GetReligionData()->GetReligiousStrength());
		pGiftUnit->GetReligionData()->SetSpreadsLeft(GetReligionData()->GetSpreadsLeft());
#endif

		if(pGiftUnit->getOwner() == GC.getGame().getActivePlayer())
		{
			strBuffer = GetLocalizedText("TXT_KEY_MISC_GIFTED_UNIT_TO_YOU", GET_PLAYER(eOwner).getNameKey(), pGiftUnit->getNameKey());
			DLLUI->AddUnitMessage(0, pGiftUnit->GetIDInfo(), pGiftUnit->getOwner(), false, GC.getEVENT_MESSAGE_TIME(), strBuffer                                                                                                                                                                                      );
		}

		CvPlayer* pMinorCiv = &GET_PLAYER(pGiftUnit->getOwner());


		CvPlayer& kPlayer = GET_PLAYER(getOwner());
		if(!kPlayer.isMinorCiv() && pMinorCiv->isMinorCiv())
		{
			pMinorCiv->GetMinorCivAI()->DoUnitGiftFromMajor(eOwner, pGiftUnit,                   false);
		}
	}
}



bool CvUnit::CanDistanceGift(PlayerTypes eToPlayer) const
{
	VALIDATE_OBJECT

	if (eToPlayer == NO_PLAYER)
		return false;


	if(GET_PLAYER(eToPlayer).isMinorCiv())
	{

		if(isFound() || IsFoundAbroad())
			return false;


		UnitClassTypes eScoutClass = (UnitClassTypes) GC.getInfoTypeForString("UNITCLASS_SCOUT", true);
		if (eScoutClass != NO_UNITCLASS && eScoutClass == getUnitClassType())
			return false;


		if(!IsCombatUnit() && !canAirAttack())
		{
			CvPlayer& kPlayer = GET_PLAYER(m_eOwner);


			if(kPlayer.GetPlayerTraits()->GetGreatPersonGiftInfluence() == 0 || !IsGreatPerson())
			{
				return false;
			}
		}


		if (GET_PLAYER(eToPlayer).GetIncomingUnitType(getOwner()) != NO_UNIT)
		{
			return false;
		}
	}


	else
		return false;

	TeamTypes eToTeam = GET_PLAYER(eToPlayer).getTeam();


	if(GET_TEAM(eToTeam).isUnitClassMaxedOut(getUnitClassType(), GET_TEAM(eToTeam).getUnitClassMaking(getUnitClassType())))
		return false;


	if(GET_PLAYER(eToPlayer).isUnitClassMaxedOut(getUnitClassType(), GET_PLAYER(eToPlayer).getUnitClassMaking(getUnitClassType())))
		return false;

	return !atWar(eToTeam, getTeam());
}


bool CvUnit::canLoadUnit(const CvUnit& unit, const CvPlot& targetPlot) const
{
	VALIDATE_OBJECT
	if(&unit == this)
	{
		return false;
	}

	if(unit.getTeam() != getTeam())
	{
		return false;
	}

	if(getCargo() > 0)
	{
		return false;
	}

	if(unit.isCargo())
	{
		return false;
	}

	if(!(unit.cargoSpaceAvailable(getSpecialUnitType(), getDomainType())))
	{
		return false;
	}

	if(!(unit.atPlot(targetPlot)))
	{
		return false;
	}

	if(!isHiddenNationality() && unit.isHiddenNationality())
	{
		return false;
	}

	return true;
}



void CvUnit::loadUnit(CvUnit& unit)
{
	VALIDATE_OBJECT
	CvPlot* p = plot();
	if(!p || !canLoadUnit(unit, *p))
	{
		return;
	}

	setTransportUnit(&unit);
}


bool CvUnit::shouldLoadOnMove(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(isCargo())
	{
		return false;
	}

	switch(getDomainType())
	{
	case DOMAIN_LAND:
		if(pPlot->isWater())
		{
			return true;
		}
		break;
	case DOMAIN_AIR:
		if(!pPlot->isFriendlyCity(*this, true))
		{
			return true;
		}
		break;
	default:
		break;
	}

	if(isTerrainImpassable(pPlot->getTerrainType()))
	{
		bool bCanPass = false;
		bCanPass = m_Promotions.GetAllowTerrainPassable(pPlot->getTerrainType());

		if(!bCanPass)
		{
			return true;
		}
	}

	return false;
}



bool CvUnit::canLoad(const CvPlot& targetPlot) const
{
	VALIDATE_OBJECT
	if(NO_SPECIALUNIT != getSpecialUnitType())
	{
		CvSpecialUnitInfo* pkSpecialUnitInfo = GC.getSpecialUnitInfo(getSpecialUnitType());
		if(pkSpecialUnitInfo)
		{
			if(pkSpecialUnitInfo->isCityLoad())
			{
				if(targetPlot.isCity())
				{
					return true;
				}

				ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
				if (pkScriptSystem)
				{
					CvLuaArgsHandle args;
					args->Push(getOwner());
					args->Push(GetID());
					args->Push(targetPlot.getX());
					args->Push(targetPlot.getY());



					bool bResult = false;
					if (LuaSupport::CallTestAny(pkScriptSystem, "CanLoadAt", args.get(), bResult))
					{

						if (bResult == true) {
							return true;
						}
					}
				}
			}
		}
	}

	const IDInfo* pUnitNode = targetPlot.headUnitNode();
	while(pUnitNode != NULL)
	{
		const CvUnit* pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = targetPlot.nextUnitNode(pUnitNode);

		if(pLoopUnit && canLoadUnit(*pLoopUnit, targetPlot))
		{
			return true;
		}
	}

	return false;
}



void CvUnit::load()
{
	VALIDATE_OBJECT
	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;
	int iPass;

	pPlot = plot();

	if(!pPlot || !canLoad(*pPlot))
	{
		return;
	}


	for(iPass = 0; iPass < 2; iPass++)
	{
		pUnitNode = pPlot->headUnitNode();

		while(pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(*pUnitNode);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);

			if(pLoopUnit && canLoadUnit(*pLoopUnit, *pPlot))
			{
				if((iPass == 0) ? (pLoopUnit->getOwner() == getOwner()) : (pLoopUnit->getTeam() == getTeam()))
				{
					setTransportUnit(pLoopUnit);
#ifdef INVISIBILITY_OF_NUCLEAR_MISSILESS_ON_SUBMARINES
					setInvisibleType(pLoopUnit->getInvisibleType());
#endif
					break;
				}
			}
		}

		if(isCargo())
		{
			break;
		}
	}
}



bool CvUnit::canUnload() const
{
	VALIDATE_OBJECT
	CvPlot& kPlot = *(plot());

	if(getTransportUnit() == NULL)
	{
		return false;
	}

	if(!kPlot.isValidDomainForLocation(*this))
	{
		return false;
	}

	return true;
}



void CvUnit::unload()
{
	VALIDATE_OBJECT
	if(!canUnload())
	{
		return;
	}

	setTransportUnit(NULL);
#ifdef INVISIBILITY_OF_NUCLEAR_MISSILESS_ON_SUBMARINES
	setInvisibleType(NO_INVISIBLE);
#endif
}



bool CvUnit::canUnloadAll() const
{
	VALIDATE_OBJECT
	if(getCargo() == 0)
	{
		return false;
	}

	return true;
}



void CvUnit::unloadAll()
{
	VALIDATE_OBJECT
	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;

	if(!canUnloadAll())
	{
		return;
	}

	pPlot = plot();

	pUnitNode = pPlot->headUnitNode();

	while(pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if(pLoopUnit && pLoopUnit->getTransportUnit() == this)
		{
			if(pLoopUnit->canUnload())
			{
				pLoopUnit->setTransportUnit(NULL);
			}
			else
			{
				CvAssert(isHuman());
				pLoopUnit->SetActivityType(ACTIVITY_AWAKE);
			}
		}
	}
}



bool CvUnit::canHold(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(isHuman() && !getFortifyTurns())
	{
		if(pPlot->getNumFriendlyUnitsOfType(this) > GC.getPLOT_UNIT_LIMIT())
		{
			return false;
		}
	}

	return true;
}



bool CvUnit::canSleep(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT

	if(!noDefensiveBonus() && IsCombatUnit())
	{
		return false;
	}

	if(isHuman() && !getFortifyTurns())
	{
		if(pPlot->getNumFriendlyUnitsOfType(this) > GC.getPLOT_UNIT_LIMIT())
		{
			return false;
		}
	}

	if(isFortifyable())
	{
		return false;
	}

	if(isWaiting())
	{
		return false;
	}

	return true;
}



bool CvUnit::canFortify(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(isHuman() && !getFortifyTurns())
	{
		if(pPlot->getNumFriendlyUnitsOfType(this) > GC.getPLOT_UNIT_LIMIT())
		{
			return false;
		}
	}

	if(!isFortifyable(true))
	{
		return false;
	}

	if(isWaiting())
	{
		return false;
	}

	return true;
}


bool CvUnit::canAirPatrol(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(getDomainType() != DOMAIN_AIR)
	{
		return false;
	}

	if(!canAirDefend(pPlot))
	{
		return false;
	}

	if(isWaiting())
	{
		return false;
	}

#ifdef CAN_SET_INTERCEPT_HALF_TIMER
	CvGame& kGame = GC.getGame();
	if (kGame.isOption(GAMEOPTION_END_TURN_TIMER_ENABLED) && (kGame.isOption("GAMEOPTION_FAST_HAND")) && kGame.getElapsedGameTurns() > 0 && 
#ifdef AUI_GAME_RELATIVE_TURN_TIMERS
		(kGame.getPitbossTurnTime() == 0 || kGame.isOption("GAMEOPTION_RELATIVE_TURN_TIMER")))
#else
		kGame.getPitbossTurnTime() == 0)
#endif
	{
#ifdef GAME_UPDATE_TURN_TIMER_ONCE_PER_TURN
		float fGameTurnEnd = kGame.getPreviousTurnLen();
#else
		float fGameTurnEnd = static_cast<float>(kGame.getMaxTurnLen());
#endif
#ifdef TURN_TIMER_PAUSE_BUTTON
		float fTimeElapsed = kGame.getTimeElapsed();
#else


		float fTimeSinceCurrentTurnStart = kGame.m_curTurnTimer.Peek() + kGame.m_fCurrentTurnTimerPauseDelta;


		float fTimeSinceGameTurnStart = kGame.m_timeSinceGameTurnStart.Peek() + kGame.m_fCurrentTurnTimerPauseDelta;

		float fTimeElapsed = (GET_PLAYER(kGame.getActivePlayer()).isSimultaneousTurns() ? fTimeSinceGameTurnStart : fTimeSinceCurrentTurnStart);
#endif

		if (fTimeElapsed * 2 > fGameTurnEnd)
		{
			return false;
		}
	}
#endif

	return true;
}


bool CvUnit::IsRangeAttackIgnoreLOS() const
{
	VALIDATE_OBJECT
	return GetRangeAttackIgnoreLOSCount() > 0;
}


int CvUnit::GetRangeAttackIgnoreLOSCount() const
{
	VALIDATE_OBJECT
	return m_iRangeAttackIgnoreLOSCount;
}


void CvUnit::ChangeRangeAttackIgnoreLOSCount(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iRangeAttackIgnoreLOSCount += iChange;
	}
}


bool CvUnit::IsCityAttackOnly() const
{
	VALIDATE_OBJECT
	return m_iCityAttackOnlyCount > 0;
}


void CvUnit::ChangeCityAttackOnlyCount(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iCityAttackOnlyCount += iChange;
	}
}


bool CvUnit::IsCaptureDefeatedEnemy() const
{
	VALIDATE_OBJECT
	return m_iCaptureDefeatedEnemyCount > 0;
}


void CvUnit::ChangeCaptureDefeatedEnemyCount(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iCaptureDefeatedEnemyCount += iChange;
	}
}


int CvUnit::GetCaptureChance(CvUnit *pEnemy)
{
	int iRtnValue = 0;

	if (m_iCaptureDefeatedEnemyCount > 0 && AreUnitsOfSameType(*pEnemy))
	{

		CvUnitEntry *pkEnemyInfo = GC.getUnitInfo(pEnemy->getUnitType());
		if (pkEnemyInfo)
		{
			int iTheirCombat = pkEnemyInfo->GetCombat();

			if (iTheirCombat > 0)
			{
				int iMyCombat = m_pUnitInfo->GetCombat();
				int iComputedChance = GC.getCOMBAT_CAPTURE_MIN_CHANCE() + (int)(((float)iMyCombat / (float)iTheirCombat) * GC.getCOMBAT_CAPTURE_RATIO_MULTIPLIER());
				iRtnValue = min(GC.getCOMBAT_CAPTURE_MAX_CHANCE(), iComputedChance);
			}
		}
	}

	return iRtnValue;
}


bool CvUnit::canSetUpForRangedAttack(const CvPlot*          ) const
{
	VALIDATE_OBJECT
	if(!isMustSetUpToRangedAttack())
	{
		return false;
	}

	if(isSetUpForRangedAttack())
	{
		return false;
	}
#if defined(v35_TRAITIFY)
	if (isEmbarked())
	{
		if (!GET_PLAYER(getOwner()).GetPlayerTraits()->IsEmbarkedMissionAllowed(static_cast<MissionTypes>(GC.getInfoTypeForString("MISSION_SET_UP_FOR_RANGED_ATTACK"))))
		{
			return false;
		}
	}
#else
	if(isEmbarked())
		return false;
#endif
		

	if(movesLeft() <= 0)
	{
		return false;
	}

	return true;
}


bool CvUnit::isSetUpForRangedAttack() const
{
	VALIDATE_OBJECT
	return m_bSetUpForRangedAttack;
}


void CvUnit::setSetUpForRangedAttack(bool bValue)
{
	VALIDATE_OBJECT
	if(isSetUpForRangedAttack() != bValue)
	{
		m_bSetUpForRangedAttack = bValue;

		if(bValue)
		{
			changeMoves(-GC.getMOVE_DENOMINATOR());
		}
	}
}
#if defined(LEKMOD_SUBMERGE_MISSION)

bool CvUnit::canSubmerge(const CvPlot*          ) const
{
	VALIDATE_OBJECT
	if(!canSubmerge())
	{
		return false;
	}

	if(hasSubmergedOrSurfacedThisTurn())
	{
		return false;
	}

	if(IsSubmerged())
	{
		return false;
	}

	if(movesLeft() <= 0)
	{
		return false;
	}

	return true;
}

bool CvUnit::canSurface(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(!IsSubmerged())
	{
		return false;
	}

	if(hasSubmergedOrSurfacedThisTurn())
	{
		return false;
	}

	if (pPlot->getFeatureType() == FEATURE_ICE)
	{
		return false;
	}

	if(movesLeft() <= 0)
	{
		return false;
	}

	return true;
}

bool CvUnit::canSubmerge() const
{
	VALIDATE_OBJECT
	return m_bCanSubmerge;
}

void CvUnit::setCanSubmerge(bool bValue)
{
	VALIDATE_OBJECT
	if(m_bCanSubmerge != bValue)
	{
		m_bCanSubmerge = bValue;
	}
}

bool CvUnit::IsSubmerged() const
{
	VALIDATE_OBJECT
	return m_bSubmerged;
}

void CvUnit::setSubmerged(bool bValue)
{
	VALIDATE_OBJECT
	if (IsSubmerged() != bValue)
	{
		for (int jJ = 0; jJ < GC.getNumPromotionInfos(); jJ++)
		{
			PromotionTypes ePromotion = (PromotionTypes)jJ;
			if (ePromotion == NO_PROMOTION)
				continue;
			CvPromotionEntry* pkPromotionEntry = GC.getPromotionInfo(ePromotion);
			if (!pkPromotionEntry)
				continue;
			if (pkPromotionEntry->IsSubmergePromotion())
			{
				setHasPromotion(ePromotion, bValue);
			}
		}
		InvisibleTypes eSubmarine = (InvisibleTypes)GC.getInfoTypeForString("INVISIBLE_SUBMARINE");
		setInvisibleType(bValue ? eSubmarine : NO_INVISIBLE);
		int iSpace = cargoSpace();
		if (iSpace > 0)
		{
			CvPlot* pTransportPlot = plot();
			for (const IDInfo* pUnitNode = pTransportPlot->headUnitNode(); pUnitNode != NULL; pUnitNode = pTransportPlot->nextUnitNode(pUnitNode))
			{
				CvUnit* pUnit = ::getUnit(*pUnitNode);
				if (pUnit && pUnit->getTransportUnit() == this)
				{
					setInvisibleType(bValue ? eSubmarine : NO_INVISIBLE);
				}
			}
		}
		m_bSubmerged = bValue;
		setHasSubmergedOrSurfacedThisTurn(true);
		changeMoves(-GC.getMOVE_DENOMINATOR());
	}
}
bool CvUnit::hasSubmergedOrSurfacedThisTurn() const
{
	return m_bHasSubmergedOrSurfaced;
}
void CvUnit::setHasSubmergedOrSurfacedThisTurn(bool bValue)
{
	m_bHasSubmergedOrSurfaced = bValue;
}
#endif
#if defined(LEKMOD_RETRAIN_MISSION)
bool CvUnit::canRetrain(const CvPlot* pPlot, bool bTestVisible) const
{
	VALIDATE_OBJECT
	if(bTestVisible)
	{
		return (getLevel() >= 3) ? true : false;
	}
	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
	if(getNumPlayerChosenPromotions() < 2)
	{
		return false;
	}
	if(getExperience() < 30)
	{
		return false;
	}
	if(pPlot->getOwner() != getOwner())
	{
		return false;
	}

	UnitTypes eUpgradeUnitType = GetUpgradeUnitType();
	CvUnitEntry* pUpgradeUnitInfo = GC.getUnitInfo(eUpgradeUnitType);
	if (pUpgradeUnitInfo != NULL)
	{
		int iUpgradePrice = upgradePrice(eUpgradeUnitType);
		if (kPlayer.GetTreasury()->GetGold() < std::max(50, (iUpgradePrice / 2)))
			return false;
	}
	else
	{
		if (kPlayer.GetTreasury()->GetGold() < 50)
			return false;
	}

	if(movesLeft() <= 0)
	{
		return false;
	}

	return true;
}
void CvUnit::retrain()
{
	VALIDATE_OBJECT
	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
	UnitTypes eUpgradeUnitType = GetUpgradeUnitType();
	CvUnitEntry* pUpgradeUnitInfo = GC.getUnitInfo(eUpgradeUnitType);
	if (pUpgradeUnitInfo != NULL)
	{
		int iUpgradePrice = upgradePrice(eUpgradeUnitType);
		kPlayer.GetTreasury()->ChangeGold(-std::max(50, (iUpgradePrice / 2)));
	}
	else
	{
		kPlayer.GetTreasury()->ChangeGold(-50);
	}
	int iSelectedPromotions = getNumPlayerChosenPromotions();
	changeExperience(-(15 * iSelectedPromotions));
	for (int jJ = 0; jJ < GC.getNumPromotionInfos(); jJ++)
	{
		PromotionTypes ePromotion = (PromotionTypes)jJ;
		if (ePromotion == NO_PROMOTION)
			continue;
		if (IsPromotionChosenByPlayer(ePromotion))
		{
			setHasPromotion(ePromotion, false);
		}
	}
	int iLevel = getLevel();
	setLevel((iLevel - iSelectedPromotions));
	changeExperience(15 * (iSelectedPromotions - 1));
	setNumPlayerChosenPromotions(0);
	setInstaHealLocked(true);
	testPromotionReady();
	setMoves(0);
}
int CvUnit::getNumPlayerChosenPromotions() const
{
	VALIDATE_OBJECT
	return m_iNumSelectedPromotions;
}
void CvUnit::setNumPlayerChosenPromotions(int iNewValue)
{
	VALIDATE_OBJECT
	m_iNumSelectedPromotions = iNewValue;
}
void CvUnit::changeNumPlayerChosenPromotions(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iNumSelectedPromotions += iChange;
	}
}
bool CvUnit::IsPromotionChosenByPlayer(PromotionTypes ePromotion) const
{
	VALIDATE_OBJECT
	return ePromotion != NO_PROMOTION ? m_abSelectedPromotions[ePromotion] : false;
}
void CvUnit::SetPromotionChosenByPlayer(PromotionTypes ePromotion, bool bChosen)
{
	VALIDATE_OBJECT
	if(ePromotion != NO_PROMOTION)
	{
		m_abSelectedPromotions.setAt(ePromotion, bChosen);
	}
}
#endif

bool CvUnit::canEmbark(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(getDomainType() != DOMAIN_LAND)
	{
		return false;
	}

	if(isEmbarked())
	{
		return false;
	}

	if(movesLeft() <= 0)
	{
		return false;
	}

#ifdef AUI_UNIT_FIX_HOVERING_EMBARK
	if ((!IsHoveringUnit() && !pPlot->isCoastalLand()) || (IsHoveringUnit() && pPlot->getTerrainType() == GC.getDEEP_WATER_TERRAIN()))
#else
	if(!pPlot->isCoastalLand())
#endif
	{
		return false;
	}

	if(!IsHasEmbarkAbility())
	{
		return false;
	}


	int iRange = 1;
	bool bOpenPlot = false;
#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX, iX;
	CvPlot* pEvalPlot;
	for (int iY = -iRange; iY <= iRange; iY++)
	{
		iMaxDX = iRange - MAX(0, iY);
		for (iX = -iRange - MIN(0, iY); iX <= iMaxDX; iX++)
		{

			pEvalPlot = plotXY(pPlot->getX(), pPlot->getY(), iX, iY);
#else
	for(int iX = -iRange; iX <= iRange; iX++)
	{
		for(int iY = -iRange; iY <= iRange; iY++)
		{
			CvPlot* pEvalPlot = plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iX, iY, iRange);
#endif
			if(!pEvalPlot)
			{
				continue;
			}

			if(pEvalPlot->getX() == pPlot->getX() && pEvalPlot->getY() == pPlot->getY())
			{
				continue;
			}

			CvPlot* p = plot();
			if(p && canEmbarkOnto(*p, *pEvalPlot))
			{
				bOpenPlot = true;


#ifdef AUI_HEXSPACE_DX_LOOPS
				goto LoopEnd;
#else
				iX = iRange + 1;
				iY = iRange + 1;
#endif
			}
		}
	}

#ifdef AUI_HEXSPACE_DX_LOOPS
	LoopEnd:
#endif
	return bOpenPlot;
}


bool CvUnit::canDisembark(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(getDomainType() != DOMAIN_LAND)
	{
		return false;
	}

	if(!isEmbarked())
	{
		return false;
	}

	if(movesLeft() <= 0)
	{
		return false;
	}

	if(!pPlot->isWater())
	{
		return false;
	}

#ifdef AUI_UNIT_FIX_HOVERING_EMBARK
	if ((!IsHoveringUnit() && !pPlot->isAdjacentToLand()) || (IsHoveringUnit() && pPlot->getTerrainType() != GC.getDEEP_WATER_TERRAIN()))
#else
	if(!pPlot->isAdjacentToLand())
#endif
	{
		return false;
	}


	int iRange = 1;
	bool bOpenPlot = false;
#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX, iX;
	CvPlot* pEvalPlot;
	for (int iY = -iRange; iY <= iRange; iY++)
	{
		iMaxDX = iRange - MAX(0, iY);
		for (iX = -iRange - MIN(0, iY); iX <= iMaxDX; iX++)
		{

			pEvalPlot = plotXY(pPlot->getX(), pPlot->getY(), iX, iY);
#else
	for(int iX = -iRange; iX <= iRange; iX++)
	{
		for(int iY = -iRange; iY <= iRange; iY++)
		{
			CvPlot* pEvalPlot = plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iX, iY, iRange);
#endif
			if(!pEvalPlot)
			{
				continue;
			}

			if(pEvalPlot->getX() == pPlot->getX() && pEvalPlot->getY() == pPlot->getY())
			{
				continue;
			}

			if(canDisembarkOnto(*pEvalPlot))
			{
				bOpenPlot = true;


#ifdef AUI_HEXSPACE_DX_LOOPS
				goto LoopEnd;
#else
				iX = iRange + 1;
				iY = iRange + 1;
#endif
			}
		}
	}

#ifdef AUI_HEXSPACE_DX_LOOPS
	LoopEnd:
#endif
	return bOpenPlot;
}


bool CvUnit::canEmbarkOnto(const CvPlot& originPlot, const CvPlot& targetPlot, bool bOverrideEmbarkedCheck              , bool bIsDestination              ) const
{
	VALIDATE_OBJECT
#if defined(LEKMOD_HELICOPTER_EMBARK_FIX)

	if (IsHoveringUnit())
	{
		return false;
	}
#endif
	if(isEmbarked() && !bOverrideEmbarkedCheck)
	{
		return false;
	}

	if(!targetPlot.isWater())
	{
		return false;
	}

	if (targetPlot.IsAllowsWalkWater())
	{
		return false;
	}

	if(m_iEmbarkAbilityCount <= 0)
	{
		return false;
	}

	if(!targetPlot.isRevealed(getTeam()))
	{
		return false;
	}

	if(getDomainType() != DOMAIN_LAND)
	{
		return false;
	}

#ifdef AUI_UNIT_FIX_HOVERING_EMBARK
	if ((!IsHoveringUnit() && !originPlot.isAdjacentToShallowWater()) || (IsHoveringUnit() && targetPlot.getTerrainType() != GC.getDEEP_WATER_TERRAIN()))
#else
	if(!originPlot.isAdjacentToShallowWater())
#endif
	{
		return false;
	}

	return canMoveInto(targetPlot, MOVEFLAG_PRETEND_EMBARKED | ((bIsDestination)?MOVEFLAG_DESTINATION:0));
}


bool CvUnit::canDisembarkOnto(const CvPlot& originPlot, const CvPlot& targetPlot, bool bOverrideEmbarkedCheck              , bool bIsDestination              ) const
{


	VALIDATE_OBJECT
#if defined(LEKMOD_HELICOPTER_EMBARK_FIX)
	if (IsHoveringUnit())
	{
		return false;
	}
#endif
	if(getDomainType() != DOMAIN_LAND)
	{
		return false;
	}

	if(!isEmbarked() && !bOverrideEmbarkedCheck)
	{
		return false;
	}

	if(!originPlot.isWater())
	{
		return false;
	}

#ifdef AUI_UNIT_FIX_HOVERING_EMBARK
	if ((!IsHoveringUnit() && !originPlot.isAdjacentToLand()) || (IsHoveringUnit() && originPlot.getTerrainType() != GC.getDEEP_WATER_TERRAIN()))
#else
	if(!originPlot.isAdjacentToLand())
#endif
	{
		return false;
	}

	if(!targetPlot.isRevealed(getTeam()))
	{
		return false;
	}

	return canMoveInto(targetPlot, MOVEFLAG_PRETEND_UNEMBARKED | ((bIsDestination)?MOVEFLAG_DESTINATION:0));
}


bool CvUnit::canDisembarkOnto(const CvPlot& targetPlot, bool bIsDestination              ) const
{
	VALIDATE_OBJECT
#if defined(LEKMOD_HELICOPTER_EMBARK_FIX)
	if (IsHoveringUnit())
	{
		return false;
	}
#endif
	if(getDomainType() != DOMAIN_LAND)
	{
		return false;
	}

	if(!isEmbarked())
	{
		return false;
	}

	if(!plot()->isWater())
	{
		return false;
	}

#ifdef AUI_UNIT_FIX_HOVERING_EMBARK
	if ((!IsHoveringUnit() && !plot()->isAdjacentToLand()) || (IsHoveringUnit() && plot()->getTerrainType() != GC.getDEEP_WATER_TERRAIN()))
#else
	if(!plot()->isAdjacentToLand())
#endif
	{
		return false;
	}

	if(!targetPlot.isRevealed(getTeam()))
	{
		return false;
	}

	return canMoveInto(targetPlot, MOVEFLAG_PRETEND_UNEMBARKED | ((bIsDestination)?MOVEFLAG_DESTINATION:0));
}


bool CvUnit::CanEverEmbark() const
{
	VALIDATE_OBJECT

	if(getDomainType() == DOMAIN_LAND && IsHasEmbarkAbility())
	{
		return true;
	}
	else
	{
		return false;
	}
}


void CvUnit::embark(CvPlot* pPlot)
{
	VALIDATE_OBJECT
	if (canChangeVisibility())
		pPlot->changeAdjacentSight(getTeam(), visibilityRange(), false, getSeeInvisibleType(), getFacingDirection(true));

	setEmbarked(true);

	if (canChangeVisibility())
		pPlot->changeAdjacentSight(getTeam(), visibilityRange(), true, getSeeInvisibleType(), getFacingDirection(true));

	auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
	gDLL->GameplayUnitEmbark(pDllUnit.get(), true);

	if(isHuman() && !GC.getGame().isGameMultiPlayer() && GET_PLAYER(GC.getGame().getActivePlayer()).isLocalPlayer())
	{
		gDLL->UnlockAchievement(ACHIEVEMENT_UNIT_EMBARK);
	}
}


void CvUnit::disembark(CvPlot* pPlot)
{
	VALIDATE_OBJECT
	if (canChangeVisibility())
		pPlot->changeAdjacentSight(getTeam(), visibilityRange(), false, getSeeInvisibleType(), getFacingDirection(true));

	setEmbarked(false);

	if (canChangeVisibility())
		pPlot->changeAdjacentSight(getTeam(), visibilityRange(), true, getSeeInvisibleType(), getFacingDirection(true));

	auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
	gDLL->GameplayUnitEmbark(pDllUnit.get(), false);
}


void CvUnit::setEmbarked(bool bValue)
{
	VALIDATE_OBJECT
	if(m_bEmbarked != bValue)
	{
		m_bEmbarked = bValue;
	}
}



bool CvUnit::IsHasEmbarkAbility() const
{
	return GetEmbarkAbilityCount() > 0;
}



int CvUnit::GetEmbarkAbilityCount() const
{
	return m_iEmbarkAbilityCount;
}



void CvUnit::ChangeEmbarkAbilityCount(int iChange)
{
	if(iChange != 0)
	{
		m_iEmbarkAbilityCount += iChange;
	}
}


bool CvUnit::IsIgnoringDangerWakeup() const
{
	return m_bIgnoreDangerWakeup;
}


void CvUnit::SetIgnoreDangerWakeup(bool bState)
{
	m_bIgnoreDangerWakeup = bState;
}


bool CvUnit::IsEmbarkAllWater() const
{
	VALIDATE_OBJECT
	return (GetEmbarkAllWaterCount() > 0);
}


void CvUnit::ChangeEmbarkAllWaterCount(int iValue)
{
	m_iEmbarkedAllWaterCount += iValue;
}


int CvUnit::GetEmbarkAllWaterCount() const
{
	return m_iEmbarkedAllWaterCount;
}


void CvUnit::ChangeEmbarkExtraVisibility(int iValue)
{
	m_iEmbarkExtraVisibility += iValue;
}


int CvUnit::GetEmbarkExtraVisibility() const
{
	return m_iEmbarkExtraVisibility;
}


void CvUnit::ChangeEmbarkDefensiveModifier(int iValue)
{
	m_iEmbarkDefensiveModifier += iValue;
}


int CvUnit::GetEmbarkDefensiveModifier() const
{
	return m_iEmbarkDefensiveModifier;
}


void CvUnit::ChangeCapitalDefenseModifier(int iValue)
{
	m_iCapitalDefenseModifier += iValue;
}


int CvUnit::GetCapitalDefenseModifier() const
{
	return m_iCapitalDefenseModifier;
}


void CvUnit::ChangeCapitalDefenseFalloff(int iValue)
{
	m_iCapitalDefenseFalloff += iValue;
}


int CvUnit::GetCapitalDefenseFalloff() const
{
	return m_iCapitalDefenseFalloff;
}


void CvUnit::ChangeCityAttackPlunderModifier(int iValue)
{
	m_iCityAttackPlunderModifier += iValue;
}


int CvUnit::GetCityAttackPlunderModifier() const
{
	return m_iCityAttackPlunderModifier;
}
#ifdef LEKMOD_MOVE_PENALTY_CITY_COMBAT


void CvUnit::ChangeCityAttackMovePenalty(int iValue)
{
	m_iCityAttackMovePenalty += iValue;
}

int CvUnit::GetCityAttackMovePenalty() const
{
	return m_iCityAttackMovePenalty;
}

#endif

void CvUnit::ChangeReligiousStrengthLossRivalTerritory(int iValue)
{
	m_iReligiousStrengthLossRivalTerritory += iValue;
}


int CvUnit::GetReligiousStrengthLossRivalTerritory() const
{
	return m_iReligiousStrengthLossRivalTerritory;
}


void CvUnit::ChangeTradeMissionInfluenceModifier(int iValue)
{
	m_iTradeMissionInfluenceModifier += iValue;
}


int CvUnit::GetTradeMissionInfluenceModifier() const
{
	return m_iTradeMissionInfluenceModifier;
}


void CvUnit::ChangeTradeMissionGoldModifier(int iValue)
{
	m_iTradeMissionGoldModifier += iValue;
}


int CvUnit::GetTradeMissionGoldModifier() const
{
	return m_iTradeMissionGoldModifier;
}

int CvUnit::GetNumGoodyHutsPopped() const
{
	return m_iNumGoodyHutsPopped;
}

void CvUnit::ChangeNumGoodyHutsPopped(int iValue)
{
	m_iNumGoodyHutsPopped = iValue;
}

bool CvUnit::TurnProcessed() const
{
	VALIDATE_OBJECT
	return m_bAITurnProcessed;
}


void CvUnit::SetTurnProcessed(bool bValue)
{
	VALIDATE_OBJECT
	if(TurnProcessed() != bValue)
	{
		m_bAITurnProcessed = bValue;
	}
}


bool CvUnit::isUnderTacticalControl() const
{
	VALIDATE_OBJECT
	return (m_eTacticalMove != NO_TACTICAL_MOVE);
}


void CvUnit::setTacticalMove(TacticalAIMoveTypes eMove)
{
	VALIDATE_OBJECT
	m_eTacticalMove = eMove;
}


TacticalAIMoveTypes CvUnit::getTacticalMove() const
{
	VALIDATE_OBJECT
	return m_eTacticalMove;
}


bool CvUnit::canRecruitFromTacticalAI() const
{
	VALIDATE_OBJECT
	if(m_eTacticalMove != NO_TACTICAL_MOVE)
	{
		return GC.GetGameTacticalMoves()->GetEntry(m_eTacticalMove)->CanRecruitForOperations();
	}
	return true;
}



void CvUnit::SetTacticalAIPlot(CvPlot* pPlot)
{
	VALIDATE_OBJECT
	if(pPlot != NULL)
	{
		m_iTacticalAIPlotX = pPlot->getX();
		m_iTacticalAIPlotY = pPlot->getY();
	}
	else
	{
		m_iTacticalAIPlotX = INVALID_PLOT_COORD;
		m_iTacticalAIPlotY = INVALID_PLOT_COORD;
	}
}



CvPlot* CvUnit::GetTacticalAIPlot() const
{
	VALIDATE_OBJECT
	CvPlot* pPlot;

	pPlot = GC.getMap().plot(m_iTacticalAIPlotX, m_iTacticalAIPlotY);

	return pPlot;
}



void CvUnit::LogWorkerEvent(BuildTypes eBuildType, bool bStartingConstruction)
{
	VALIDATE_OBJECT

	if(!GC.getLogging() || !GC.GetBuilderAILogging())
	{
		return;
	}

	CvString strLog;
	CvString strTemp;

	strTemp.Format("%d,", GC.getGame().getGameTurn());
	strLog += strTemp;

	CvString strPlayerName;
	strPlayerName = GET_PLAYER(getOwner()).getCivilizationShortDescription();
	strLog += strPlayerName;
	strLog += ",";

	strTemp.Format("%d,", GetID());
	strLog += strTemp;

	CvString strLoc;
	strLoc.Format("%d,%d,", getX(), getY());
	strLog += strLoc;

	strLog += GC.getBuildInfo(eBuildType)->GetType();
	strLog += ",";


	if(bStartingConstruction)
	{
		strLog += "begin,";
	}
	else
	{
		strLog += "complete,";
	}


	CvString strResource = ",";
	CvString strCanSee = ",";

	ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(eBuildType)->getImprovement();
	if(eImprovement != NO_IMPROVEMENT)
	{
		ResourceTypes eResource = plot()->getResourceType(getTeam());
		if(eResource != NO_RESOURCE)
		{
			strResource = GC.getResourceInfo(eResource)->GetType();
			strResource += ",";
		}
		else if(plot()->getResourceType(NO_TEAM) != NO_RESOURCE)
		{
			eResource = plot()->getResourceType(NO_TEAM);
			strResource = GC.getResourceInfo(eResource)->GetType();
			strResource += ",";

			strCanSee = "Can't see!,";
		}
	}

	strLog += strResource;
	strLog += strCanSee;


	CvString strCost = ",";
	if(bStartingConstruction)
	{
		strCost.Format("%d,", GET_PLAYER(getOwner()).getBuildCost(plot(), eBuildType));
	}
	strLog += strCost;


	CvString strGoldAmount = ",";
	if(bStartingConstruction)
	{
		strGoldAmount.Format("%d,", GET_PLAYER(getOwner()).GetTreasury()->GetGold());
	}
	strLog += strGoldAmount;


	CvString strFileName;
	if(GC.getPlayerAndCityAILogSplit())
	{
		strFileName = "BuilderEventLog_" + strPlayerName + ".csv";
	}
	else
	{
		strFileName = "BuilderEventLog.csv";
	}



	FILogFile* pLog;
	pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);
	pLog->Msg(strLog);
}



int CvUnit::GetPower() const
{
	VALIDATE_OBJECT
	int iPower = m_iCachedPower;

	int iPowerMod = getLevel() * 125;
	iPower = (iPower * (1000 + iPowerMod)) / 1000;
	iPower *= GetCurrHitPoints();
	iPower /= GetMaxHitPoints();
	return iPower;
}


bool CvUnit::canHeal(const CvPlot* pPlot, bool bTestVisible) const
{
	VALIDATE_OBJECT


	if(isBarbarian())
	{
		return false;
	}

	if(!IsHurt())
	{
		return false;
	}

	if(isWaiting())
	{
		return false;
	}

	if(healRate(pPlot) <= 0)
	{
		return false;
	}




	if(isHuman())
	{
		if(plot()->isCity() && getDomainType() != DOMAIN_AIR)
		{
			CvUnit* pUnit;
			int iBestDefenderValue = 0;
			int iBestDefenderID = 0;

#ifdef AUI_WARNING_FIXES
			for (uint iUnitLoop = 0; iUnitLoop < plot()->getNumUnits(); iUnitLoop++)
#else
			for(int iUnitLoop = 0; iUnitLoop < plot()->getNumUnits(); iUnitLoop++)
#endif
			{
				pUnit = plot()->getUnitByIndex(iUnitLoop);


				if(pUnit->getDomainType() == getDomainType())
				{
					if(pUnit->GetBaseCombatStrength() > iBestDefenderValue)
					{
						iBestDefenderValue = pUnit->GetBaseCombatStrength();
						iBestDefenderID = pUnit->GetID();
					}
				}
			}


			if(iBestDefenderID != GetID())
			{
				return false;
			}
		}
	}


	if(!bTestVisible)
	{

#if defined(v35_TRAITIFY)
		if (isEmbarked() && !GET_PLAYER(getOwner()).GetPlayerTraits()->IsEmbarkedMissionAllowed(static_cast<MissionTypes>(GC.getInfoTypeForString("MISSION_HEAL"))))
#else
		if(isEmbarked())
#endif
		{
			return false;
		}


		if(getDomainType() == DOMAIN_SEA)
		{
			if(!IsInFriendlyTerritory() && !isHealOutsideFriendly())
			{
				return false;
			}
		}
	}

	return true;
}



bool CvUnit::canSentry(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(isHuman() && !getFortifyTurns())
	{
		if(pPlot->getNumFriendlyUnitsOfType(this) > GC.getPLOT_UNIT_LIMIT())
		{
			return false;
		}
	}

	if(!IsCanDefend(pPlot))
	{
		return false;
	}

	if(isWaiting())
	{
		return false;
	}

	return true;
}



int CvUnit::healRate(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT

	if(getDomainType() == DOMAIN_SEA)
	{
		if(!IsInFriendlyTerritory() && !isHealOutsideFriendly())
		{
			return 0;
		}
	}

	const IDInfo* pUnitNode;
	CvCity* pCity = pPlot->getPlotCity();
	CvCity* pClosestCity = NULL;
	const CvUnit* pLoopUnit;
	CvPlot* pLoopPlot;

	int iExtraHeal = 0;
	int iExtraFriendlyHeal = getExtraFriendlyHeal();
	int iExtraNeutralHeal = getExtraNeutralHeal();
	int iExtraEnemyHeal = getExtraEnemyHeal();


	int iReligionMod = 0;
	if(!pCity)
	{
		pClosestCity = pPlot->GetAdjacentFriendlyCity(getTeam());
	}
	else
	{
		pClosestCity = pCity;
	}
	if(pClosestCity && pClosestCity->getOwner() == getOwner())
	{
		ReligionTypes eMajority = pClosestCity->GetCityReligions()->GetReligiousMajority();
		if(eMajority != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, getOwner());
			if(pReligion)
			{
				iReligionMod = pReligion->m_Beliefs.GetFriendlyHealChange();
				BeliefTypes eSecondaryPantheon = pClosestCity->GetCityReligions()->GetSecondaryReligionPantheonBelief();
				if (eSecondaryPantheon != NO_BELIEF)
				{
					iReligionMod += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetFriendlyHealChange();
				}
				iExtraFriendlyHeal += iReligionMod;
			}
		}
	}


	int iBestHealFromUnits = 0;
	pUnitNode = pPlot->headUnitNode();
	while(pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if(pLoopUnit && pLoopUnit->getTeam() == getTeam())
		{
			int iHeal = pLoopUnit->getSameTileHeal();

			if(iHeal > iBestHealFromUnits)
			{
				iBestHealFromUnits = iHeal;
			}
		}
	}
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

		if(pLoopPlot != NULL)
		{
			if(pLoopPlot->area() == pPlot->area())
			{
				pUnitNode = pLoopPlot->headUnitNode();

				while(pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(*pUnitNode);
					pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

					if(pLoopUnit && pLoopUnit->getTeam() == getTeam())
					{
						int iHeal = pLoopUnit->getAdjacentTileHeal();

						if(iHeal > iBestHealFromUnits)
						{
							iBestHealFromUnits = iHeal;
						}
					}
				}
			}
		}
	}
	iExtraHeal += iBestHealFromUnits;


	int iBaseHeal = 0;
	if(pPlot->isCity())
	{
		iBaseHeal = GC.getCITY_HEAL_RATE();
		iExtraHeal += (GET_TEAM(getTeam()).isFriendlyTerritory(pPlot->getTeam()) ? iExtraFriendlyHeal : iExtraNeutralHeal);
		if(pCity)
		{
			iExtraHeal += pCity->getHealRate();
		}
	}
	else
	{
		if(!IsInFriendlyTerritory())
		{
			if(isEnemy(pPlot->getTeam(), pPlot))
			{
				iBaseHeal = GC.getENEMY_HEAL_RATE();
				iExtraHeal += iExtraEnemyHeal;
			}
			else
			{
				iBaseHeal = GC.getNEUTRAL_HEAL_RATE();
				iExtraHeal += iExtraNeutralHeal;
			}
		}
		else
		{
			iBaseHeal = GC.getFRIENDLY_HEAL_RATE();
			iExtraHeal += iExtraFriendlyHeal;
		}
	}


	int iBaseHealMod = GET_PLAYER(getOwner()).getUnitBaseHealModifier();
	if(iBaseHealMod != 0)
	{
#ifdef AUI_UNIT_FIX_BASE_HEAL_MOD
		iBaseHeal = ((100 + iBaseHealMod) * iBaseHeal) / 100;
#else
		iBaseHeal = ((100 + iBaseHealMod) / 100) * iBaseHeal;
#endif
	}
	CvAssertMsg(iBaseHeal >= 0, "Base healing rate not expected to be negative!");

	int iTotalHeal = iBaseHeal + iExtraHeal;

	return iTotalHeal;
}



int CvUnit::healTurns(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	int iHeal;
	int iTurns;

	if(!IsHurt())
	{
		return 0;
	}

	iHeal = healRate(pPlot);

	if(iHeal > 0)
	{
		iTurns = (getDamage() / iHeal);

		if((getDamage() % iHeal) != 0)
		{
			iTurns++;
		}

		return iTurns;
	}
	else
	{
		return INT_MAX;
	}
}



void CvUnit::doHeal()
{
	VALIDATE_OBJECT
#ifdef ENHANCED_GRAPHS
		int	iHealth = GetCurrHitPoints();
#endif
	if(!isBarbarian())
	{
		changeDamage(-(healRate(plot())));
	}
#ifdef ENHANCED_GRAPHS
	int iHealed = GetCurrHitPoints() - iHealth;
	CvPlayer& thisPlayer = GET_PLAYER(getOwner());
	thisPlayer.ChangeUnitsDamageHealed(iHealed);
#endif
}


void CvUnit::DoAttrition()
{
	CvPlot* pPlot = plot();
#ifdef AUI_WARNING_FIXES
	if (!pPlot)
		return;
	TeamTypes eOwnerTeam = pPlot->getTeam();
#endif
	CvString strAppendText;

	if(!IsInFriendlyTerritory())
	{
#ifdef AUI_WARNING_FIXES
		if (isEnemy(eOwnerTeam, pPlot) && getEnemyDamageChance() > 0 && getEnemyDamage() > 0)
#else
		if(isEnemy(pPlot->getTeam(), pPlot) && getEnemyDamageChance() > 0 && getEnemyDamage() > 0)
#endif
		{
			if(GC.getGame().getJonRandNum(100, "Enemy Territory Damage Chance") < getEnemyDamageChance())
			{
				strAppendText =  GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_ATTRITION");
				changeDamage(getEnemyDamage(), NO_PLAYER, 0.0, &strAppendText);
			}
		}
		else if(getNeutralDamageChance() > 0 && getNeutralDamage() > 0)
		{
			if(GC.getGame().getJonRandNum(100, "Neutral Territory Damage Chance") < getNeutralDamageChance())
			{
				strAppendText =  GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_ATTRITION");
				changeDamage(getNeutralDamage(), NO_PLAYER, 0.0, &strAppendText);
			}
		}
	}


	if(getDomainType() == DOMAIN_LAND && pPlot->isMountain() && !canMoveAllTerrain())
	{
		strAppendText =  GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_ATTRITION");
		changeDamage(50, NO_PLAYER, 0.0, &strAppendText);
	}

	if(getDamage() >= GC.getMAX_HIT_POINTS())
	{
		CvString strBuffer;
		CvNotifications* pNotification = GET_PLAYER(getOwner()).GetNotifications();
		if(pNotification)
		{
			strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_ATTRITION", getNameKey());
			Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_LOST");
			pNotification->Add(NOTIFICATION_UNIT_DIED, strBuffer, strSummary.toUTF8(), getX(), getY(), (int)getUnitType(), getOwner());
		}
	}

#ifndef AUI_WARNING_FIXES
	TeamTypes eOwnerTeam = pPlot->getTeam();
#endif

	if (eOwnerTeam != NO_TEAM)
	{
		CvTeam &kTeam = GET_TEAM(eOwnerTeam);
		if (!kTeam.isMinorCiv() && eOwnerTeam != getTeam() && !kTeam.IsAllowsOpenBordersToTeam(getTeam()))
		{
			int iReligiousStrengthLoss = GetReligiousStrengthLossRivalTerritory();
			if (iReligiousStrengthLoss > 0)
			{
				int iStrength = GetReligionData()->GetReligiousStrength();
				int iStrengthLoss = (getUnitInfo().GetReligiousStrength() * iReligiousStrengthLoss) / 100;

				if ((iStrength - iStrengthLoss) <= 0)
				{
					CvString strBuffer;
					CvNotifications* pNotification = GET_PLAYER(getOwner()).GetNotifications();
					if(pNotification)
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_ATTRITION", getNameKey());
						Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_LOST");
						pNotification->Add(NOTIFICATION_UNIT_DIED, strBuffer, strSummary.toUTF8(), getX(), getY(), (int)getUnitType(), getOwner());
					}

					kill(false);
				}
				else
				{
					GetReligionData()->SetReligiousStrength(iStrength - iStrengthLoss);
#ifdef AUI_WARNING_FIXES
					if (pPlot->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF)
#else
					if (pPlot && pPlot->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF)
#endif
					{
#if defined(LEKMOD_MACOS)
						Localization::String string = Localization::String(GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_ATTRITION").c_str());
#else
						Localization::String string = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_ATTRITION");
#endif

						char text[256];
						sprintf_s (text, "%s [COLOR_WHITE]-%d [ICON_PEACE][ENDCOLOR]", string.toUTF8(), iStrengthLoss);
						float fDelay = 0.0f;
						DLLUI->AddPopupText(getX(), getY(), text, fDelay);
					}
				}
			}
		}
	}
}


bool CvUnit::canAirlift(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	CvCity* pCity;

	if(getDomainType() != DOMAIN_LAND)
	{
		return false;
	}

	if(hasMoved())
	{
		return false;
	}

	if (pPlot->isWater())
	{
		return false;
	}

	pCity = pPlot->getPlotCity();
	if(pCity == NULL)
	{
		pCity = pPlot->GetAdjacentCity();
		if (pCity == NULL)
		{
			return false;
		}
	}

	if (isTrade())
	{
		return false;
	}

	if (!pCity->CanAirlift())
	{
		return false;
	}

	if(pCity->getTeam() != getTeam())
	{
		return false;
	}

	return true;
}



bool CvUnit::canAirliftAt(const CvPlot* pPlot, int iX, int iY) const
{
	VALIDATE_OBJECT
	CvPlot* pTargetPlot;
	CvCity* pStartCity;
	CvCity* pTargetCity;

	if(!canAirlift(pPlot))
	{
		return false;
	}

	pStartCity = pPlot->getPlotCity();
	if(pStartCity == NULL)
	{
		pStartCity = pPlot->GetAdjacentCity();
		if (pStartCity == NULL)
		{
			return false;
		}
	}

	pTargetPlot = GC.getMap().plot(iX, iY);
	byte bMoveFlags = CvUnit::MOVEFLAG_DESTINATION;
	if(!pTargetPlot || !canMoveInto(*pTargetPlot, bMoveFlags) || pTargetPlot->isWater())
	{
		return false;
	}

	pTargetCity = pTargetPlot->getPlotCity();
	if(pTargetCity == NULL)
	{
		pTargetCity = pTargetPlot->GetAdjacentFriendlyCity(getTeam());
		if (pTargetCity == NULL)
		{
			return false;
		}
	}

	if (pTargetCity == pStartCity)
	{
		return false;
	}

	if (!pTargetCity->CanAirlift())
	{
		return false;
	}


	CvPlot* pAdjacentPlot;
	int iI;
	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(iX, iY, ((DirectionTypes)iI));
		if(pAdjacentPlot != NULL)
		{
			UnitHandle pDefender = pAdjacentPlot->getBestDefender(NO_PLAYER, getOwner(), NULL, true);
			if (pDefender)
			{
				return false;
			}
		}
	}

	if(pTargetCity->getTeam() != getTeam())
	{
		return false;
	}

	return true;
}



bool CvUnit::airlift(int iX, int iY)
{
	VALIDATE_OBJECT
	CvPlot* pTargetPlot;

	if(!canAirliftAt(plot(), iX, iY))
	{
		return false;
	}

	pTargetPlot = GC.getMap().plot(iX, iY);
	CvAssert(pTargetPlot != NULL);



	CvPlayerAI& kActivePlayer = GET_PLAYER(GC.getGame().getActivePlayer());
	if(pTargetPlot != NULL && getOwner() == kActivePlayer.GetID() && kActivePlayer.isHuman())
	{



		CvPlot* pCurrentPlot = plot();
		if(pCurrentPlot != NULL)
		{
			CvCity* pCurrentCity = pCurrentPlot->getPlotCity();
			CvCity* pTargetCity = pTargetPlot->getPlotCity();
			if(	pCurrentCity != NULL && pTargetCity != NULL &&
				strcmp(pCurrentCity->getNameKey(), "TXT_KEY_CITY_NAME_CASABLANCA") == 0 &&
				pTargetCity->IsOriginalCapital())
			{
				const PlayerTypes eOriginalOwner = pTargetCity->getOriginalOwner();
				CvPlayerAI& kOriginalPlayer = GET_PLAYER(eOriginalOwner);
				if(strcmp(kOriginalPlayer.getCivilizationTypeKey(), "CIVILIZATION_PORTUGAL") == 0)
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_XP2_22);
				}
			}
		}
	}


	if(pTargetPlot != NULL)
	{
		finishMoves();
		setXY(pTargetPlot->getX(), pTargetPlot->getY());
		return true;
	}

	return false;
}



bool CvUnit::isNukeVictim(const CvPlot* pPlot, TeamTypes eTeam) const
{
	VALIDATE_OBJECT
	CvPlot* pLoopPlot;
	int iDX, iDY;

	if(!(GET_TEAM(eTeam).isAlive()))
	{
		return false;
	}

	if(eTeam == getTeam())
	{
		return false;
	}

	int iBlastRadius =       GC.getNUKE_BLAST_RADIUS();

#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX;
	for (iDY = -iBlastRadius; iDY <= iBlastRadius; iDY++)
	{
		iMaxDX = iBlastRadius - MAX(0, iDY);
		for (iDX = -iBlastRadius - MIN(0, iDY); iDX <= iMaxDX; iDX++)
		{

			pLoopPlot = plotXY(pPlot->getX(), pPlot->getY(), iDX, iDY);
#else
	for(iDX = -(iBlastRadius); iDX <= iBlastRadius; iDX++)
	{
		for(iDY = -(iBlastRadius); iDY <= iBlastRadius; iDY++)
		{
			pLoopPlot	= plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iDX, iDY, iBlastRadius);
#endif

			if(pLoopPlot != NULL)
			{
				if(pLoopPlot->getTeam() == eTeam)
				{
					return true;
				}

				if(pLoopPlot->plotCheck(PUF_isCombatTeam, eTeam, getTeam()) != NULL)
				{
					return true;
				}
			}
		}
	}

	return false;
}



bool CvUnit::canNuke(const CvPlot*          ) const
{
	VALIDATE_OBJECT
	if(GetNukeDamageLevel() == -1)
	{
		return false;
	}

	return true;
}



bool CvUnit::canNukeAt(const CvPlot* pPlot, int iX, int iY) const
{
	VALIDATE_OBJECT

	if(!canNuke(pPlot))
	{
		return false;
	}

	int iDistance = plotDistance(pPlot->getX(), pPlot->getY(), iX, iY);

	if(iDistance <= 0)
		return false;

	int iRange  = GetRange();
	if(iRange > 0 && iDistance > iRange)
	{
		return false;
	}

	for(int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if(GET_TEAM((TeamTypes)iI).isAlive())
		{
			CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);
			if(pTargetPlot)
			{
				TeamTypes eTheirTeam = (TeamTypes)iI;
				if(isNukeVictim(pTargetPlot, eTheirTeam))
				{
					CvTeam& myTeam = GET_TEAM(getTeam());
					if(!myTeam.isAtWar(eTheirTeam) && !myTeam.canDeclareWar(eTheirTeam))
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}


bool CvUnit::canParadrop(const CvPlot* pPlot, bool bOnlyTestVisibility) const
{
	VALIDATE_OBJECT
	if(getDropRange() <= 0)
	{
		return false;
	}


	if(!bOnlyTestVisibility)
	{
		if(hasMoved())
		{
			return false;
		}
#if defined(v35_TRAITIFY)
		if (isEmbarked() && !GET_PLAYER(getOwner()).GetPlayerTraits()->IsEmbarkedMissionAllowed(static_cast<MissionTypes>(GC.getInfoTypeForString("MISSION_PARADROP"))))
#else
		if(isEmbarked())
#endif
		{
			return false;
		}

		if(pPlot->IsFriendlyTerritory(getOwner()))
		{

			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if (pkScriptSystem) 
			{
				CvLuaArgsHandle args;
				args->Push(((int)getOwner()));
				args->Push(GetID());
				args->Push(pPlot->getX());
				args->Push(pPlot->getY());



				bool bResult = false;
				if (LuaSupport::CallTestAll(pkScriptSystem, "CannotParadropFrom", args.get(), bResult))
				{
					if (bResult == true)
					{
						return false;
					}
				}
			}
		}
		else
		{

			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if (pkScriptSystem) {
				CvLuaArgsHandle args;
				args->Push(((int)getOwner()));
				args->Push(GetID());
				args->Push(pPlot->getX());
				args->Push(pPlot->getY());



				bool bResult = false;
				if (LuaSupport::CallTestAny(pkScriptSystem, "CanParadropFrom", args.get(), bResult))
				{
					if (bResult == true)
					{
						return true;
					}
				}
			}

			return false;
		}
	}

	return true;
}



bool CvUnit::canParadropAt(const CvPlot* pPlot, int iX, int iY) const
{
	VALIDATE_OBJECT
	if(!canParadrop(pPlot, false))
	{
		return false;
	}

	CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);
	if(NULL == pTargetPlot || pTargetPlot == pPlot)
	{
		return false;
	}

	if(!pTargetPlot->isVisible(getTeam()))
	{
		return false;
	}

	if(!canMoveInto(*pTargetPlot, MOVEFLAG_DESTINATION))
	{
		return false;
	}

	if(plotDistance(pPlot->getX(), pPlot->getY(), iX, iY) > getDropRange())
	{
		return false;
	}

#ifdef CAN_PARADROP_HALF_TIMER
	CvGame& kGame = GC.getGame();
	if (kGame.isOption(GAMEOPTION_END_TURN_TIMER_ENABLED) && (kGame.isOption("GAMEOPTION_FAST_HAND")) && kGame.getElapsedGameTurns() > 0 && 
#ifdef AUI_GAME_RELATIVE_TURN_TIMERS
		(kGame.getPitbossTurnTime() == 0 || kGame.isOption("GAMEOPTION_RELATIVE_TURN_TIMER")))
#else
		kGame.getPitbossTurnTime() == 0)
#endif
	{
#ifdef GAME_UPDATE_TURN_TIMER_ONCE_PER_TURN
		float fGameTurnEnd = kGame.getPreviousTurnLen();
#else
		float fGameTurnEnd = static_cast<float>(kGame.getMaxTurnLen());
#endif

#ifdef TURN_TIMER_PAUSE_BUTTON
		float fTimeElapsed = kGame.getTimeElapsed();
#else


		float fTimeSinceCurrentTurnStart = kGame.m_curTurnTimer.Peek() + kGame.m_fCurrentTurnTimerPauseDelta;


		float fTimeSinceGameTurnStart = kGame.m_timeSinceGameTurnStart.Peek() + kGame.m_fCurrentTurnTimerPauseDelta;

		float fTimeElapsed = (GET_PLAYER(kGame.getActivePlayer()).isSimultaneousTurns() ? fTimeSinceGameTurnStart : fTimeSinceCurrentTurnStart);
#endif

		if (fTimeElapsed * 2 > fGameTurnEnd && !pTargetPlot->IsFriendlyTerritory(getOwner()))
		{
			return false;
		}
	}
#endif

	return true;
}



bool CvUnit::paradrop(int iX, int iY)
{
	VALIDATE_OBJECT
	if(!canParadropAt(plot(), iX, iY))
	{
		return false;
	}

	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	if(! pPlot)
	{
		return false;
	}

	changeMoves(-(GC.getMOVE_DENOMINATOR() / 2));
	setMadeAttack(true);
#ifdef NQM_UNIT_FIX_NO_INSTAHEAL_AFTER_PARADROP
	setCanInstahealThisTurn(false);
#endif

	CvPlot* fromPlot = plot();


#ifdef REMOVE_PARADROP_ANIMATION
	if (CvPreGame::quickMovement())
	{
		setXY(pPlot->getX(), pPlot->getY());
	}
	else
	{

		if (pPlot->isActiveVisible(false))
		{
			auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
			gDLL->GameplayUnitParadrop(pDllUnit.get());
		}
		setXY(pPlot->getX(), pPlot->getY(), true, true, false);
	}
#else

	if (pPlot->isActiveVisible(false))
	{
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitParadrop(pDllUnit.get());
	}

	setXY(pPlot->getX(), pPlot->getY(), true, true, false);
#endif

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(((int)getOwner()));
		args->Push(GetID());
		args->Push(fromPlot->getX());
		args->Push(fromPlot->getY());
		args->Push(pPlot->getX());
		args->Push(pPlot->getY());

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "ParadropAt", args.get(), bResult);
	}

	return true;
}


bool CvUnit::canMakeTradeRoute(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if (!isTrade())
	{
		return false;
	}

	if (pPlot == NULL)
	{
		return false;
	}

	CvCity* pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		return false;
	}

	if (pCity->getOwner() != getOwner())
	{
		return false;
	}

	return true;
}


bool CvUnit::canMakeTradeRouteAt(const CvPlot* pPlot, int iX, int iY, TradeConnectionType eConnectionType) const
{
	if (!canMakeTradeRoute(pPlot))
	{
		return false;
	}

	if (pPlot == NULL)
	{
		return false;
	}

	CvCity* pFromCity = pPlot->getPlotCity();
	if (pFromCity == NULL)
	{
		return false;
	}


	if (pFromCity->getOwner() != getOwner())
	{
		return false;
	}

	CvPlot* pToPlot = GC.getMap().plot(iX, iY);


	if(pToPlot == NULL)
	{
		return false;
	}


	if(pToPlot == pPlot)
	{
		return false;
	}

	CvCity* pToCity = pToPlot->getPlotCity();
	if (pToCity == NULL)
	{
		return false;
	}

	if (!GET_PLAYER(getOwner()).GetTrade()->CanCreateTradeRoute(pFromCity, pToCity, getDomainType(), eConnectionType, false))
	{
		return false;
	}

	return true;
}


bool CvUnit::makeTradeRoute(int iX, int iY, TradeConnectionType eConnectionType)
{
	if (!canMakeTradeRouteAt(plot(), iX, iY, eConnectionType))
	{
		return false;
	}

	kill(true);

	CvCity* pFromCity = NULL;
	CvCity* pToCity = NULL;

	if (plot())
	{
		pFromCity = plot()->getPlotCity();
	}

	CvPlot* pToPlot = GC.getMap().plot(iX, iY);
	if (pToPlot)
	{
		pToCity = pToPlot->getPlotCity();
	}

	bool bResult = GET_PLAYER(getOwner()).GetTrade()->CreateTradeRoute(pFromCity, pToCity, getDomainType(), eConnectionType);
	return bResult;
}


bool CvUnit::canChangeTradeUnitHomeCity(const CvPlot* pPlot) const
{
	if (!isTrade())
	{
		return false;
	}

	if (pPlot == NULL)
	{
		return false;
	}

	CvCity* pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		return false;
	}

	if (pCity->getOwner() != getOwner())
	{
		return false;
	}

	return true;
}


bool CvUnit::canChangeTradeUnitHomeCityAt(const CvPlot* pPlot, int iX, int iY) const
{
	if (!canChangeTradeUnitHomeCity(pPlot))
	{
		return false;
	}

	if (pPlot == NULL)
	{
		return false;
	}

	CvCity* pFromCity = pPlot->getPlotCity();
	if (pFromCity == NULL)
	{
		return false;
	}


	if (pFromCity->getOwner() != getOwner())
	{
		return false;
	}

	CvPlot* pToPlot = GC.getMap().plot(iX, iY);


	if(pToPlot == NULL)
	{
		return false;
	}


	if(pToPlot == pPlot)
	{
		return false;
	}

	CvCity* pToCity = pToPlot->getPlotCity();
	if (pToCity == NULL)
	{
		return false;
	}


	if (pToCity->getOwner() != getOwner())
	{
		return false;
	}

	if (getDomainType() == DOMAIN_SEA)
	{
		if (!pToCity->isCoastal())
		{
			return false;
		}
	}

	return true;
}


bool CvUnit::changeTradeUnitHomeCity(int iX, int iY)
{
	if (!canChangeTradeUnitHomeCityAt(plot(), iX, iY))
	{
		return false;
	}

	setXY(iX, iY);
	setMoves(0);
	return true;
}


bool CvUnit::canChangeAdmiralPort(const CvPlot* pPlot) const
{
	if (!IsGreatAdmiral())
	{
		return false;
	}

	if (pPlot == NULL)
	{
		return false;
	}

	CvCity* pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		return false;
	}

	if (pCity->getOwner() != getOwner())
	{
		return false;
	}

	if (!pCity->isCoastal())
	{
		return false;
	}

	return true;
}


bool CvUnit::canChangeAdmiralPortAt(const CvPlot* pPlot, int iX, int iY) const
{
	if (!canChangeAdmiralPort(pPlot))
	{
		return false;
	}

	if (pPlot == NULL)
	{
		return false;
	}

	CvCity* pFromCity = pPlot->getPlotCity();
	if (pFromCity == NULL)
	{
		return false;
	}

	if (pFromCity->getOwner() != getOwner())
	{
		return false;
	}

	CvPlot* pToPlot = GC.getMap().plot(iX, iY);


	if(pToPlot == NULL)
	{
		return false;
	}


	if(pToPlot == pPlot)
	{
		return false;
	}

	CvCity* pToCity = pToPlot->getPlotCity();
	if (pToCity == NULL)
	{
		return false;
	}


	if (pToCity->getOwner() != getOwner())
	{
		return false;
	}

	if (!pToCity->isCoastal())
	{
		return false;
	}

	return true;
}


bool CvUnit::changeAdmiralPort(int iX, int iY)
{
	if (!canChangeAdmiralPortAt(plot(), iX, iY))
	{
		return false;
	}

	setXY(iX, iY);
	setMoves(0);
	return true;
}

#ifdef NQ_UNIT_IMMUNE_TO_PLUNDER_FROM_TRAIT
bool CvUnit::IsPlunderBlockedByOpposingTrait() const
{
	CvPlot* pPlot = plot();
	if (pPlot != NULL && GET_PLAYER(m_eOwner).GetTrade()->ContainsOpposingPlayerTradeUnit(pPlot))
	{
		std::vector<int> aiTradeUnitsAtPlot;
		aiTradeUnitsAtPlot = GET_PLAYER(m_eOwner).GetTrade()->GetOpposingTradeUnitsAtPlot(pPlot, true);
		if (aiTradeUnitsAtPlot.size() > 0)
		{
			PlayerTypes eTradeUnitOwner = GC.getGame().GetGameTrade()->GetOwnerFromID(aiTradeUnitsAtPlot[0]);
			if (eTradeUnitOwner != NO_PLAYER)
			{
				DomainTypes eDomain = GC.getGame().GetGameTrade()->GetDomainFromID(aiTradeUnitsAtPlot[0]);
				if (eDomain == DOMAIN_SEA && GET_PLAYER(eTradeUnitOwner).GetPlayerTraits()->IsSeaTradeRoutesArePlunderImmune())
				{
					return true;
				}
			}
		}
	}

	return false;
}
#endif


bool CvUnit::canPlunderTradeRoute(const CvPlot* pPlot, bool bOnlyTestVisibility) const
{

	if(!(getUnitInfo().IsPillage()))
	{
		return false;
	}
	if (!IsCombatUnit())
	{
		return false;
	}

	if (pPlot == NULL)
	{
		return false;
	}
#if defined(v35_TRAITIFY)
	if (isEmbarked() && !GET_PLAYER(getOwner()).GetPlayerTraits()->IsEmbarkedMissionAllowed(static_cast<MissionTypes>(GC.getInfoTypeForString("MISSION_PLUNDER_TRADE_ROUTE"))))
#else
	if (isEmbarked())
#endif
	{
		return false;
	}

	if (GET_PLAYER(m_eOwner).GetTrade()->ContainsOpposingPlayerTradeUnit(pPlot))
	{
		if (!bOnlyTestVisibility)
		{
			std::vector<int> aiTradeUnitsAtPlot;
			aiTradeUnitsAtPlot = GET_PLAYER(m_eOwner).GetTrade()->GetOpposingTradeUnitsAtPlot(pPlot, true);
			if (aiTradeUnitsAtPlot.size() <= 0)
			{
				return false;
			}

			PlayerTypes eTradeUnitOwner = GC.getGame().GetGameTrade()->GetOwnerFromID(aiTradeUnitsAtPlot[0]);
			if (eTradeUnitOwner == NO_PLAYER)
			{

				return false;
			}

			TeamTypes eTeam = GET_PLAYER(eTradeUnitOwner).getTeam();
			if (!GET_TEAM(GET_PLAYER(m_eOwner).getTeam()).isAtWar(eTeam))
			{
				return false;
			}

#ifdef NQ_UNIT_IMMUNE_TO_PLUNDER_FROM_TRAIT
			DomainTypes eDomain = GC.getGame().GetGameTrade()->GetDomainFromID(aiTradeUnitsAtPlot[0]);
			if (eDomain == DOMAIN_SEA && GET_PLAYER(eTradeUnitOwner).GetPlayerTraits()->IsSeaTradeRoutesArePlunderImmune())
			{
				return false;
			}
#endif

		}

		return true;
	}
	else
	{
		return false;
	}
}


bool CvUnit::plunderTradeRoute()
{
	CvPlot* pPlot = plot();
	if (!canPlunderTradeRoute(pPlot, false))
	{
		return false;
	}

	std::vector<int> aiTradeUnitsAtPlot;
	CvPlayerTrade* pTrade = GET_PLAYER(m_eOwner).GetTrade();
	aiTradeUnitsAtPlot = pTrade->GetOpposingTradeUnitsAtPlot(pPlot, false);
	CvAssertMsg(aiTradeUnitsAtPlot.size() > 0, "aiTradeUnitsAtPlot is empty. Earlier check should have verified that this can't be the case");
	if (aiTradeUnitsAtPlot.size() <= 0)
	{
		return false;
	}

#ifdef LEKMOD_NEW_LUA_EVENTS
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem)
	{
		CvLuaArgsHandle args;

		args->Push(m_eOwner);
		args->Push(GetID());
		args->Push(pPlot->getX());
		args->Push(pPlot->getY());



		bool bResult = false;
		LuaSupport::CallHook(pkScriptSystem, "UnitPlundered", args.get(), bResult);
	}
#endif


	pTrade->PlunderTradeRoute(aiTradeUnitsAtPlot[0]);
	return true;
}


bool CvUnit::canCreateGreatWork(const CvPlot* pPlot, bool bOnlyTestVisibility) const
{

	if(GetGreatWork() == NO_GREAT_WORK)
	{
		return false;
	}

	const TeamTypes eTeam = getTeam();
	if(GET_TEAM(eTeam).isMinorCiv())
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}


	if (bOnlyTestVisibility)
	{
		return true;
	}


	CvCity* pCity;
	if (pPlot == NULL)
	{
		return false;
	}
	pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		pCity = pPlot->GetAdjacentCity();
		if (pCity == NULL)
		{
			return false;
		}
	}


	if (eTeam != pCity->getTeam())
	{
		return false;
	}


	GreatWorkSlotType eGreatWorkSlot = CultureHelpers::GetGreatWorkSlot(GetGreatWork());
	CvPlayer &kPlayer = GET_PLAYER(getOwner());
	if (kPlayer.GetCulture()->GetNumAvailableGreatWorkSlots(eGreatWorkSlot) > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool CvUnit::createGreatWork()
{
	CvPlot* pPlot = plot();
	if (!canCreateGreatWork(pPlot, false))
	{
		return false;
	}

	CvGameCulture *pCulture = GC.getGame().GetGameCulture();
	if(pCulture == NULL)
	{
		CvAssertMsg(pCulture != NULL, "This should never happen.");
		return false;
	}
	
	CvPlayer &kPlayer = GET_PLAYER(m_eOwner);
	BuildingClassTypes eBuildingClass = NO_BUILDINGCLASS;
#ifdef AUI_WARNING_FIXES
	uint iSlot = MAX_UNSIGNED_INT;
#else
	int iSlot = -1;
#endif
	GreatWorkType eGreatWorkType = GetGreatWork();
	GreatWorkClass eClass = CultureHelpers::GetGreatWorkClass(eGreatWorkType);
	GreatWorkSlotType eGreatWorkSlot = CultureHelpers::GetGreatWorkSlot(eGreatWorkType);
	
	CvCity *pCity = kPlayer.GetCulture()->GetClosestAvailableGreatWorkSlot(getX(), getY(), eGreatWorkSlot, &eBuildingClass, &iSlot);
	if (pCity)
	{
		int iGWindex = pCulture->CreateGreatWork(eGreatWorkType, eClass, m_eOwner, kPlayer.GetCurrentEra(), getName());
		pCity->GetCityBuildings()->SetBuildingGreatWork(eBuildingClass, iSlot, iGWindex);

		if(pPlot->isActiveVisible(false))
		{
#ifndef REMOVE_GAMEPLAY_UNIT_ACTIVATE_ANIMATION
			auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
			gDLL->GameplayUnitActivate(pDllUnit.get());
#endif
		}

		if(IsGreatPerson())
		{
#ifdef AUI_DLLNETMESSAGEHANDLER_FIX_RESPAWN_PROPHET_IF_BEATEN_TO_LAST_RELIGION
			if (!IsIgnoreExpended())
#endif
			kPlayer.DoGreatPersonExpended(getUnitType());
		}

		kill(true);

		bool bDontShowRewardPopup = GC.GetEngineUserInterface()->IsOptionNoRewardPopups();
		Localization::String localizedText;


		if(bDontShowRewardPopup || GC.getGame().isNetworkMultiPlayer())
		{
			CvNotifications* pNotifications = kPlayer.GetNotifications();
			if(pNotifications)
			{
				localizedText = Localization::Lookup("TXT_KEY_MISC_WONDER_COMPLETED");
				localizedText << kPlayer.getNameKey() << pCulture->GetGreatWorkName(iGWindex);
				pNotifications->Add(NOTIFICATION_GREAT_WORK_COMPLETED_ACTIVE_PLAYER, localizedText.toUTF8(), localizedText.toUTF8(), getX(), getY(), iGWindex, kPlayer.GetID());
			}
		}

		else
		{
			if(kPlayer.GetID() == GC.getGame().getActivePlayer())
			{
				CvPopupInfo kPopup(BUTTONPOPUP_GREAT_WORK_COMPLETED_ACTIVE_PLAYER, iGWindex);
				GC.GetEngineUserInterface()->AddPopup(kPopup);
			}
		}

		return true;
	}

	return false;
}


int CvUnit::getNumExoticGoods() const
{
	return m_iNumExoticGoods;
}


void CvUnit::setNumExoticGoods(int iValue)
{
	CvAssert(iValue >= 0);
	m_iNumExoticGoods = iValue;
}


void CvUnit::changeNumExoticGoods(int iChange)
{
	setNumExoticGoods(getNumExoticGoods() + iChange);
}


float CvUnit::calculateExoticGoodsDistanceFactor(const CvPlot* pPlot)
{
	float fDistanceFactor = 0.0f;

	CvCity* pCapital = GET_PLAYER(getOwner()).getCapitalCity();
	if (pPlot && pCapital)
	{
		int iDistanceThreshold = (GC.getMap().getGridWidth() + GC.getMap().getGridHeight()) / 2;
		fDistanceFactor = plotDistance(pPlot->getX(), pPlot->getY(), pCapital->getX(), pCapital->getY()) / (float) iDistanceThreshold;
	}

	return fDistanceFactor;
}


bool CvUnit::canSellExoticGoods(const CvPlot* pPlot, bool bOnlyTestVisibility) const
{
	if (pPlot == NULL)
	{
		return false;
	}

	if (getNumExoticGoods() <= 0)
	{
		return false;
	}

	if (!bOnlyTestVisibility)
	{
		int iNumValidPlots = 0;
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			CvPlot* pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));
			if (pLoopPlot != NULL)
			{
				PlayerTypes eLoopPlotOwner = pLoopPlot->getOwner();
				if (eLoopPlotOwner != getOwner() && eLoopPlotOwner != NO_PLAYER)
				{
					if (!GET_TEAM(getTeam()).isAtWar(GET_PLAYER(eLoopPlotOwner).getTeam()))
					{
						iNumValidPlots++;
					}
				}
			}
		}

		if (iNumValidPlots <= 0)
		{
			return false;
		}
	}

	return true;
}


int CvUnit::getExoticGoodsGoldAmount()
{
	int iValue = 0;
	if (canSellExoticGoods(plot()))
	{
		float fDistanceFactor = calculateExoticGoodsDistanceFactor(plot());

		int iExtraGold = GC.getEXOTIC_GOODS_GOLD_MAX() - GC.getEXOTIC_GOODS_GOLD_MIN();
		iValue = GC.getEXOTIC_GOODS_GOLD_MIN() + (int)(iExtraGold * fDistanceFactor);
		iValue = MIN(iValue, GC.getEXOTIC_GOODS_GOLD_MAX());
	}
	return iValue;
}


int CvUnit::getExoticGoodsXPAmount()
{
	int iValue = 0;
	if (canSellExoticGoods(plot()))
	{
		float fDistanceFactor = calculateExoticGoodsDistanceFactor(plot());

		int iExtraXP = GC.getEXOTIC_GOODS_XP_MAX() - GC.getEXOTIC_GOODS_XP_MIN();
		iValue = GC.getEXOTIC_GOODS_XP_MIN() + (int)(iExtraXP * fDistanceFactor);
		iValue = MIN(iValue, GC.getEXOTIC_GOODS_XP_MAX());
	}
	return iValue;
}


bool CvUnit::sellExoticGoods()
{
	if (canSellExoticGoods(plot()))
	{
		int iXP = getExoticGoodsXPAmount();
		int iGold = getExoticGoodsGoldAmount();
		changeExperience(iXP);
		GET_PLAYER(getOwner()).GetTreasury()->ChangeGold(iGold);
		char text[256] = {0};
		sprintf_s(text, "[COLOR_YELLOW]+%d[ENDCOLOR][ICON_GOLD]", iGold);
		float fDelay = 0.0f;
		DLLUI->AddPopupText(getX(), getY(), text, fDelay);

		changeNumExoticGoods(-1);
	}
	return false;
}
#if defined(UNITS_REMEMBER_HOME)
CvCity* CvUnit::GetHomeCity() const
{
	VALIDATE_OBJECT

	if (getOwner() == NO_PLAYER)
		return NULL;

	if (m_iHomeCity == -1)
		return NULL;

	return GET_PLAYER(getOwner()).getCity(m_iHomeCity);
}
void CvUnit::SetHomeCity(int iNewCity)
{
	VALIDATE_OBJECT
	m_iHomeCity = iNewCity;
}

#endif

bool CvUnit::canRebase(const CvPlot*          ) const
{

	if(getDomainType() != DOMAIN_AIR)
	{
		return false;
	}


	if(!IsImmobile())
	{
		return false;
	}


	if(getMoves() <= 0)
	{
		return false;
	}

	return true;
}


bool CvUnit::canRebaseAt(const CvPlot* pPlot, int iX, int iY) const
{

	if(!canRebase(pPlot))
	{
		return false;
	}

	CvPlot* pToPlot = GC.getMap().plot(iX, iY);


	if(pToPlot == NULL)
	{
		return false;
	}


	if(pToPlot == pPlot)
	{
		return false;
	}


	int iRange = GetRange();
	iRange *=         GC.getAIR_UNIT_REBASE_RANGE_MULTIPLIER();
	iRange /= 100;

	if(plotDistance(getX(), getY(), iX, iY) > iRange)
	{
		return false;
	}


	if(!canLoad(*pToPlot))
	{
		return false;
	}


	bool bCityToRebase = false;
	if(pToPlot->isCity())
	{

		if(pToPlot->getPlotCity()->getOwner() == getOwner())
		{
			bCityToRebase = true;
		}

		if (!bCityToRebase)
		{
			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if (pkScriptSystem)
			{
				CvLuaArgsHandle args;
				args->Push(getOwner());
				args->Push(GetID());
				args->Push(iX);
				args->Push(iY);



				bool bResult = false;
				if (LuaSupport::CallTestAny(pkScriptSystem, "CanRebaseInCity", args.get(), bResult))
				{

					if (bResult == true)
					{
						bCityToRebase = true;
					}
				}
			}
		}

		int iUnitsThere = pToPlot->countNumAirUnits(getTeam());
		if (iUnitsThere >= pToPlot->getPlotCity()->GetMaxAirUnits())
		{
			return false;
		}
	}


	bool bUnitToRebase = false;

	if(!bCityToRebase)
	{
		IDInfo* pUnitNode;
		CvUnit* pLoopUnit;
		pUnitNode = pToPlot->headUnitNode();
		while(pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(*pUnitNode);
			pUnitNode = pToPlot->nextUnitNode(pUnitNode);


			if(canLoadUnit(*pLoopUnit, *pToPlot))
			{
				bUnitToRebase = true;

				break;
			}
		}
	}


	if(!bCityToRebase && !bUnitToRebase)
	{
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem) 
		{
			CvLuaArgsHandle args;
			args->Push(getOwner());
			args->Push(GetID());
			args->Push(iX);
			args->Push(iY);



			bool bResult = false;
			if (LuaSupport::CallTestAny(pkScriptSystem, "CanRebaseTo", args.get(), bResult))
			{

				if (bResult == true)
				{
					return true;
				}
			}
		}

		return false;
	}

	return true;
}


bool CvUnit::rebase(int iX, int iY)
{
	if(!canRebaseAt(plot(), iX, iY))
	{
		return false;
	}

	CvPlot* oldPlot = plot();

	CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);
	CvAssert(pTargetPlot != NULL);
	if(pTargetPlot == NULL)
		return false;

	finishMoves();


	setReconPlot(NULL);

	bool bShow = true;

	if ((plot()->isVisibleToWatchingHuman() || pTargetPlot->isVisibleToWatchingHuman()) && !CvPreGame::quickMovement())
	{
		SpecialUnitTypes eSpecialUnitPlane = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_FIGHTER");
		if(getSpecialUnitType() == eSpecialUnitPlane)
		{
			auto_ptr<ICvPlot1> pDllOldPlot(new CvDllPlot(oldPlot));
			auto_ptr<ICvPlot1> pDllTargetPlot(new CvDllPlot(pTargetPlot));

			auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
			gDLL->GameplayUnitRebased(pDllUnit.get(), pDllOldPlot.get(), pDllTargetPlot.get());
			bShow = false;
		}
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(getOwner());
		args->Push(GetID());
		args->Push(iX);
		args->Push(iY);

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "RebaseTo", args.get(), bResult);
	}

	setXY(pTargetPlot->getX(), pTargetPlot->getY(), false, bShow, false);
#if defined(FIX_PRODUCTION_KEEPING_EXPLOITS)
	if (pTargetPlot->isCity())
	{
		CvCity* pRebaseCity = pTargetPlot->getPlotCity();
		if (pRebaseCity)
		{
			const int iUnitsThere = pTargetPlot->countNumAirUnits(getTeam());
			if (iUnitsThere >= pRebaseCity->GetMaxAirUnits())
			{
				pRebaseCity->CleanUpQueue();
				if (pRebaseCity->headOrderQueueNode() == NULL)
					pRebaseCity->chooseProduction();
			}
		}
	}
#endif
	return true;
}


bool CvUnit::canPillage(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT

	if(pPlot->IsAllowsSailLand()){
        for (int iUnitLoop = 0; iUnitLoop < pPlot->getNumUnits(); iUnitLoop++)
        {
            CvUnit* pkUnit = pPlot->getUnitByIndex(iUnitLoop);
            if (pkUnit)
            {
                if (pkUnit->getDomainType() == DOMAIN_SEA)
                {
                    return false;
                }
            }
        }
    }
#if defined(v35_TRAITIFY)
	if(isEmbarked() && !GET_PLAYER(getOwner()).GetPlayerTraits()->IsEmbarkedMissionAllowed(static_cast<MissionTypes>(GC.getInfoTypeForString("MISSION_PILLAGE"))))
#else
	if(isEmbarked())
#endif
	{
		return false;
	}

	if(!(getUnitInfo().IsPillage()))
	{
		return false;
	}
	else
	{
		TechTypes ePillagePrereq = (TechTypes) getUnitInfo().GetPrereqPillageTech();
		if(ePillagePrereq != NO_TECH)
		{
			if(!GET_TEAM(GET_PLAYER(getOwner()).getTeam()).GetTeamTechs()->HasTech(ePillagePrereq))
			{
				return false;
			}
		}
	}


	if(isBarbarian() && getDomainType() == DOMAIN_SEA)
	{
		return false;
	}

	if(pPlot->isCity())
	{
		return false;
	}

	ImprovementTypes eImprovementType = pPlot->getImprovementType();
	if(eImprovementType == NO_IMPROVEMENT)
	{
		if(!(pPlot->isRoute()))
		{
			return false;
		}
	}
	else if(eImprovementType == (ImprovementTypes)GC.getRUINS_IMPROVEMENT())
	{
		return false;
	}
	else
	{
		CvImprovementEntry* pImprovementInfo = GC.getImprovementInfo(pPlot->getImprovementType());
		if(pImprovementInfo->IsPermanent())
		{
			return false;
		}
		else if(pImprovementInfo->IsGoody())
		{
			return false;
		}


		if(pImprovementInfo->GetLuxuryCopiesSiphonedFromMinor() > 0)
		{
			PlayerTypes eOwner = pPlot->getOwner();
			PlayerTypes eSiphoner = pPlot->GetPlayerThatBuiltImprovement();
			if (eOwner != NO_PLAYER && GET_PLAYER(eOwner).isMinorCiv())
			{
				if (eSiphoner != NO_PLAYER && GET_PLAYER(eSiphoner).isAlive())
				{
					if (!atWar(getTeam(), GET_PLAYER(eSiphoner).getTeam()))
					{
						return false;
					}
				}
			}
		}
	}


	if((eImprovementType == NO_IMPROVEMENT || pPlot->IsImprovementPillaged()) &&
	        (pPlot->getRouteType() == NO_ROUTE || pPlot->IsRoutePillaged()                                           ))
	{
		return false;
	}

	if(pPlot->isOwned())
	{
		if(!potentialWarAction(pPlot))
		{
			if((eImprovementType == NO_IMPROVEMENT && !pPlot->isRoute()) || (pPlot->getOwner() != getOwner()))
			{
				return false;
			}
		}
	}


	if(pPlot->getOwner() == getOwner())
	{
		return false;
	}

	if(!(pPlot->isValidDomainForAction(*this)))
	{
		return false;
	}

	return true;
}



bool CvUnit::pillage()
{
	VALIDATE_OBJECT
	CvString strBuffer;

	ImprovementTypes eTempImprovement = NO_IMPROVEMENT;

	CvPlot* pPlot = plot();

	if(!canPillage(pPlot))
	{
		return false;
	}

	if(pPlot->isOwned())
	{

		if(!isEnemy(pPlot->getTeam(), pPlot))
		{
			if((pPlot->getImprovementType() == NO_IMPROVEMENT && !pPlot->isRoute()) || (pPlot->getOwner() != getOwner()))
			{
				return false;
			}
		}
	}

	bool bImprovement = false;


	if(pPlot->getImprovementType() != NO_IMPROVEMENT)
	{
		if(!pPlot->IsImprovementPillaged())
		{
			bImprovement = true;
		}
	}


	bool bSuccessfulNonRoadPillage = false;
	if(bImprovement)
	{
		eTempImprovement = pPlot->getImprovementType();
		CvImprovementEntry* pkImprovement = GC.getImprovementInfo(eTempImprovement);
		if(pkImprovement)
		{
			if(pPlot->getTeam() != getTeam())
			{
				int iPillageGold = 0;


				iPillageGold = GC.getGame().getJonRandNum(pkImprovement->GetPillageGold(), "Pillage Gold 1");
				iPillageGold += GC.getGame().getJonRandNum(pkImprovement->GetPillageGold(), "Pillage Gold 2");
				iPillageGold += (getPillageChange() * iPillageGold) / 100;

				if(iPillageGold > 0)
				{
					GET_PLAYER(getOwner()).GetTreasury()->ChangeGold(iPillageGold);

					if(getOwner() == GC.getGame().getActivePlayer())
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_PLUNDERED_GOLD_FROM_IMP", iPillageGold, pkImprovement->GetTextKey());
						DLLUI->AddUnitMessage(0, GetIDInfo(), getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer                                                                                                                                                   );
					}

					if(pPlot->isOwned() && pPlot->getOwner() == GC.getGame().getActivePlayer())
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_IMP_DESTROYED", pkImprovement->GetTextKey(), getNameKey(), getVisualCivAdjective(pPlot->getTeam()));
						DLLUI->AddPlotMessage(0, pPlot->GetPlotIndex(), pPlot->getOwner(), false, GC.getEVENT_MESSAGE_TIME(), strBuffer                                                                                                                                                              );
					}
				}
			}

			changeExperience(getPillageXPChange());


			if(getOwner() == GC.getGame().getActivePlayer() && strcmp(pkImprovement->GetType(), "IMPROVEMENT_FARM") == 0)
				CvAchievementUnlocker::FarmImprovementPillaged();


			bSuccessfulNonRoadPillage = true;
			if(pkImprovement->IsDestroyedWhenPillaged())
			{
				pPlot->setImprovementType(NO_IMPROVEMENT);
			}

			else
			{
				pPlot->SetImprovementPillaged(true);
			}

			if (pkImprovement->IsDisplacePillager())
			{
				jumpToNearestValidPlot();
			}
		}
	}
	else if(pPlot->isRoute())
	{
		pPlot->SetRoutePillaged(true);
#if defined(LEKMOD_NO_INSTANT_REPAIR_ON_ROUTE)
		pPlot->SetWasRoutePillaged(true);
#endif
	}

	if(!hasFreePillageMove())
	{
		changeMoves(-GC.getMOVE_DENOMINATOR());
	}

	if(bSuccessfulNonRoadPillage)
	{
		if (hasHealOnPillage())
		{

			changeDamage(-getDamage());
		}
		else
		{
			int iHealAmount = min(getDamage(), GC.getPILLAGE_HEAL_AMOUNT() + getPillageHealChange());
			changeDamage(-iHealAmount);
		}
	}
#ifdef LEKMOD_NEW_LUA_EVENTS
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem)
	{
		CvLuaArgsHandle args;

		args->Push(getOwner());
		args->Push(GetID());
		args->Push(plot()->getX());
		args->Push(plot()->getY());



		bool bResult = false;
		LuaSupport::CallHook(pkScriptSystem, "UnitPillaged", args.get(), bResult);
	}
#endif
	return true;
}



bool CvUnit::canFound(const CvPlot* pPlot, bool bTestVisible) const
{
	VALIDATE_OBJECT
#ifdef NQM_UNIT_LIMIT_VENICE_CONQUISTADOR_SETTLES
	if (!m_pUnitInfo->IsFound() || (m_pUnitInfo->IsFoundAbroad() && m_pUnitInfo->IsFound() && GET_PLAYER(m_eOwner).GetPlayerTraits()->IsNoAnnexing()))
#else
	if(!m_pUnitInfo->IsFound())
#endif
	{
		if(!m_pUnitInfo->IsFoundAbroad())
		{
			return false;
		}
		else
		{
			CvCity* pCapital = GET_PLAYER(m_eOwner).getCapitalCity();
			if(!pCapital || pCapital->getArea() == pPlot->getArea())
			{
				return false;
			}
		}
	}

	if(!(GET_PLAYER(getOwner()).canFound(pPlot->getX(), pPlot->getY(), bTestVisible)))
	{
		return false;
	}

	return true;
}



bool CvUnit::found()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();

	if(!canFound(pPlot))
	{
		return false;
	}

	PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();

	if(eActivePlayer == getOwner())
	{
		auto_ptr<ICvPlot1> pDllPlot = GC.WrapPlotPointer(pPlot);
		DLLUI->lookAt(pDllPlot.get(), CAMERALOOKAT_NORMAL);
	}

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
	CvPlayerAI& kActivePlayer = GET_PLAYER(eActivePlayer);
#if defined(LEKMOD_TRACK_CITY_SETTLER_UNITTYPE)
	UnitTypes eThisUnitType = getUnitType();
	kPlayer.found(getX(), getY(), eThisUnitType);
#else
	kPlayer.found(getX(), getY());
#endif

	if(pPlot->isActiveVisible(false))
	{
#ifndef REMOVE_GAMEPLAY_UNIT_ACTIVATE_ANIMATION
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
#endif


		if(eActivePlayer == getOwner() && kActivePlayer.getNumCities() >= 2 && kActivePlayer.isHuman() && !GC.getGame().isGameMultiPlayer())
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_SECOND_CITY);
		}
	}

	auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
	gDLL->GameplayUnitVisibility(pDllUnit.get(), false);
	kill(true);

	return true;
}


bool CvUnit::canJoin(const CvPlot*          , SpecialistTypes                ) const
{
	VALIDATE_OBJECT


	return false;
}



bool CvUnit::join(SpecialistTypes eSpecialist)
{
	VALIDATE_OBJECT

	if(!canJoin(plot(), eSpecialist))
	{
		return false;
	}

	kill(true);

	return true;
}



bool CvUnit::canConstruct(const CvPlot* pPlot, BuildingTypes eBuilding) const
{
	VALIDATE_OBJECT
	CvCity* pCity;

	if(eBuilding == NO_BUILDING)
	{
		return false;
	}

	pCity = pPlot->getPlotCity();

	if(pCity == NULL)
	{
		return false;
	}

	if(getTeam() != pCity->getTeam())
	{
		return false;
	}

	if(pCity->GetCityBuildings()->GetNumRealBuilding(eBuilding) > 0)
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

	return true;
}



bool CvUnit::construct(BuildingTypes eBuilding)
{
	VALIDATE_OBJECT
	CvCity* pCity;

	if(!canConstruct(plot(), eBuilding))
	{
		return false;
	}

	pCity = plot()->getPlotCity();

	if(pCity != NULL)
	{
		pCity->GetCityBuildings()->SetNumRealBuilding(eBuilding, pCity->GetCityBuildings()->GetNumRealBuilding(eBuilding) + 1);
	}

	if(plot()->isActiveVisible(false))
	{
#ifndef REMOVE_GAMEPLAY_UNIT_ACTIVATE_ANIMATION
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
#endif
	}

	kill(true);

	return true;
}


bool CvUnit::CanFoundReligion(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
#ifdef AUI_WARNING_FIXES
	if (!m_pUnitInfo->IsFoundReligion())
	{
		return false;
	}
#endif
	CvCity* pCity;
#ifndef AUI_RELIGION_FIX_NO_BELIEFS_AVAILABLE_CHECK_FOR_NON_PANTHEON_MOVED
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
#endif

	pCity = pPlot->getPlotCity();

#ifndef AUI_WARNING_FIXES
	if(!m_pUnitInfo->IsFoundReligion())
	{
		return false;
	}
#endif

	if(pCity == NULL)
	{
		return false;
	}

#ifndef AUI_RELIGION_FIX_NO_BELIEFS_AVAILABLE_CHECK_FOR_NON_PANTHEON_MOVED
	if(GET_PLAYER(getOwner()).GetReligions()->IsFoundingReligion())
	{
		return false;
	}
#endif

	if(getTeam() != pCity->getTeam())
	{
		return false;
	}

#ifdef AI_CANNOT_FOUND_OR_ENHANCE_OR_SPREAD_RELIGION
	if (!GET_TEAM(getTeam()).isHuman() && GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_RELIGION_FOUNDING"))
#else
	if(GET_TEAM(getTeam()).isMinorCiv())
#endif
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

#ifdef AUI_RELIGION_FIX_NO_BELIEFS_AVAILABLE_CHECK_FOR_NON_PANTHEON_MOVED
	if (GC.getGame().GetGameReligions()->CanFoundReligion(getOwner(), NO_RELIGION, NULL, NO_BELIEF, NO_BELIEF, NO_BELIEF, NO_BELIEF, pCity) != CvGameReligions::FOUNDING_OK)
#else
	if(pReligions->GetNumReligionsStillToFound() <= 0)
	{
		return false;
	}

	if(pReligions->HasCreatedReligion(getOwner()))
#endif
	{
		return false;
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem) 
	{
		CvLuaArgsHandle args;
		args->Push(getOwner());
		args->Push(pCity->GetID());



		bool bResult = false;
		if (LuaSupport::CallTestAll(pkScriptSystem, "PlayerCanFoundReligion", args.get(), bResult))
		{
			if (bResult == false) 
			{
				return false;
			}
		}
	}

#ifndef AUI_RELIGION_FIX_NO_BELIEFS_AVAILABLE_CHECK_FOR_NON_PANTHEON_MOVED
	if (pReligions->GetAvailableFounderBeliefs().size() < 1 || pReligions->GetAvailableFollowerBeliefs().size() < 1)
	{
		return false;
	}
#endif

	return true;
}


bool CvUnit::DoFoundReligion()
{
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();

	CvPlot* pkPlot = plot();
	CvCity* pkCity = (NULL != pkPlot)? pkPlot->getPlotCity() : NULL;

	if(pkPlot != NULL && pkCity != NULL)
	{
		if(CanFoundReligion(pkPlot))
		{
			CvPlayerAI& kOwner = GET_PLAYER(getOwner());
			if(kOwner.isHuman())
			{
				CvAssertMsg(pkCity != NULL, "No City??");

				CvNotifications* pNotifications = kOwner.GetNotifications();
				if(pNotifications)
				{
					CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_FOUND_RELIGION");
					CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_FOUND_RELIGION");
					pNotifications->Add(NOTIFICATION_FOUND_RELIGION, strBuffer, strSummary, pkPlot->getX(), pkPlot->getY(), -1, pkCity->GetID());
				}
				kOwner.GetReligions()->SetFoundingReligion(true);
#ifdef AUI_DLLNETMESSAGEHANDLER_FIX_RESPAWN_PROPHET_IF_BEATEN_TO_LAST_RELIGION
				if (!IsIgnoreExpended())
#endif
				kOwner.DoGreatPersonExpended(getUnitType());
				kill(true);
			}
			else
			{
				ReligionTypes eReligion = pReligions->GetReligionToFound(getOwner());
				if(eReligion != NO_RELIGION)
				{

					BeliefTypes eBeliefs[4];
					eBeliefs[2] = NO_BELIEF;
					eBeliefs[3] = NO_BELIEF;
					int iIndex = 0;
					if(!kOwner.GetReligions()->HasCreatedPantheon())
					{
						eBeliefs[iIndex] = kOwner.GetReligionAI()->ChoosePantheonBelief();
						iIndex++;
					}
					eBeliefs[iIndex] = kOwner.GetReligionAI()->ChooseFollowerBelief();
					iIndex++;
					eBeliefs[iIndex] = kOwner.GetReligionAI()->ChooseFounderBelief();
					iIndex++;

					if(kOwner.GetPlayerTraits()->IsBonusReligiousBelief())
					{
						eBeliefs[iIndex] = kOwner.GetReligionAI()->ChooseBonusBelief(eBeliefs[0], eBeliefs[1], eBeliefs[2]);
					}

					pReligions->FoundReligion(getOwner(), eReligion, NULL, eBeliefs[0], eBeliefs[1], eBeliefs[2], eBeliefs[3], pkCity);
#ifdef AUI_DLLNETMESSAGEHANDLER_FIX_RESPAWN_PROPHET_IF_BEATEN_TO_LAST_RELIGION
					if (!IsIgnoreExpended())
#endif
					kOwner.DoGreatPersonExpended(getUnitType());
					kill(true);
				}
				else
				{
					CvAssertMsg(false, "No religions available to found.");
				}
			}
		}
	}

	return false;
}


bool CvUnit::CanEnhanceReligion(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	CvCity* pCity;
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();

	pCity = pPlot->getPlotCity();

	if(!m_pUnitInfo->IsFoundReligion())
	{
		return false;
	}


	if(m_pUnitInfo->IsSpreadReligion())
	{
		if(GetReligionData()->GetSpreadsLeft() < m_pUnitInfo->GetReligionSpreads())
		{
			return false;
		}
	}

	if(pCity == NULL)
	{
		return false;
	}

#if defined(AUI_RELIGION_FIX_SIMULTANEOUS_ENHANCE_OR_FOUND_CAUSING_MULTIPLE) && !defined(AUI_RELIGION_FIX_NO_BELIEFS_AVAILABLE_CHECK_FOR_NON_PANTHEON_MOVED)
	if (GET_PLAYER(getOwner()).GetReligions()->SetFoundingReligion())
	{
		return false;
	}
#endif

	if(getTeam() != pCity->getTeam())
	{
		return false;
	}

#ifdef AI_CANNOT_FOUND_OR_ENHANCE_OR_SPREAD_RELIGION
	if (!GET_TEAM(getTeam()).isHuman() && GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_RELIGION_FOUNDING"))
#else
	if(GET_TEAM(getTeam()).isMinorCiv())
#endif
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

#ifndef AUI_RELIGION_FIX_NO_BELIEFS_AVAILABLE_CHECK_FOR_NON_PANTHEON_MOVED
	if(!pReligions->HasCreatedReligion(getOwner()))
	{
		return false;
	}
#endif

	ReligionTypes eReligion = pReligions->GetReligionCreatedByPlayer(getOwner());
	const CvReligion* pReligion = pReligions->GetReligion(eReligion, getOwner());
#ifndef AUI_RELIGION_FIX_NO_BELIEFS_AVAILABLE_CHECK_FOR_NON_PANTHEON_MOVED
	if(pReligion->m_bEnhanced)
	{
		return false;
	}
#endif

	if(getX() != pReligion->m_iHolyCityX || getY() != pReligion->m_iHolyCityY)
	{
		return false;
	}

#ifdef AUI_RELIGION_FIX_NO_BELIEFS_AVAILABLE_CHECK_FOR_NON_PANTHEON_MOVED
	if (pReligions->CanEnhanceReligion(getOwner(), eReligion, NO_BELIEF, NO_BELIEF) != CvGameReligions::FOUNDING_OK)
#else
	if (pReligions->GetAvailableEnhancerBeliefs().size() < 1 || pReligions->GetAvailableFollowerBeliefs().size() < 1)
#endif
	{
		return false;
	}

	return true;
}


bool CvUnit::DoEnhanceReligion()
{
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();

	CvPlot* pkPlot = plot();
	CvCity* pkCity = (NULL != pkPlot)? pkPlot->getPlotCity() : NULL;

	if(pkPlot != NULL && pkCity != NULL)
	{
		if(CanEnhanceReligion(pkPlot))
		{
			CvPlayerAI& kOwner = GET_PLAYER(getOwner());
			if(kOwner.isHuman())
			{
				CvAssertMsg(pkCity != NULL, "No City??");

				CvNotifications* pNotifications = kOwner.GetNotifications();
				if(pNotifications)
				{
					CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ENHANCE_RELIGION");
					CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ENHANCE_RELIGION");
					pNotifications->Add(NOTIFICATION_ENHANCE_RELIGION, strBuffer, strSummary, pkPlot->getX(), pkPlot->getY(), -1, pkCity->GetID());
				}
#ifdef AUI_RELIGION_FIX_SIMULTANEOUS_ENHANCE_OR_FOUND_CAUSING_MULTIPLE
				kOwner.GetReligions()->SetFoundingReligion(true);
#endif
#ifdef AUI_DLLNETMESSAGEHANDLER_FIX_RESPAWN_PROPHET_IF_BEATEN_TO_LAST_RELIGION
				if (!IsIgnoreExpended())
#endif
				kOwner.DoGreatPersonExpended(getUnitType());
				kill(true);
			}
			else
			{
				ReligionTypes eReligion = pReligions->GetReligionCreatedByPlayer(getOwner());
				if(eReligion != NO_RELIGION)
				{

					BeliefTypes eBelief1 = kOwner.GetReligionAI()->ChooseFollowerBelief();
					BeliefTypes eBelief2 = kOwner.GetReligionAI()->ChooseEnhancerBelief();

					pReligions->EnhanceReligion(getOwner(), eReligion, eBelief1, eBelief2);

#ifdef AUI_DLLNETMESSAGEHANDLER_FIX_RESPAWN_PROPHET_IF_BEATEN_TO_LAST_RELIGION
					if (!IsIgnoreExpended())
#endif
					kOwner.DoGreatPersonExpended(getUnitType());
					kill(true);
				}
				else
				{
					CvAssertMsg(false, "No religions available to found.");
				}
			}
		}
	}

	return false;
}


bool CvUnit::CanSpreadReligion(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	CvCity* pCity;

	if(!m_pUnitInfo->IsSpreadReligion())
	{
		return false;
	}

	if(GetReligionData()->GetReligion() == NO_RELIGION)
	{
		return false;
	}

#ifdef AI_CANNOT_FOUND_OR_ENHANCE_OR_SPREAD_RELIGION
	if (!GET_TEAM(getTeam()).isHuman() && GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_RELIGION_SPREAD"))
	{
		return false;
	}
#endif

	pCity = pPlot->getPlotCity();
	if(pCity == NULL)
	{
		pCity = pPlot->GetAdjacentCity();
		if(pCity == NULL)
		{
			return false;
		}
	}


	if(pCity->GetCityReligions()->IsDefendedAgainstSpread(GetReligionData()->GetReligion()))
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

	return true;
}


bool CvUnit::DoSpreadReligion()
{
	int iScienceBonus = 0;

	CvCity* pCity = GetSpreadReligionTargetCity();

	if (pCity != NULL)
	{
		if(CanSpreadReligion(plot()))
		{
#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
			const PlayerTypes eCityOwner = pCity->getOwner();
			const bool bMinorCityStateTarget = (eCityOwner >= MAX_MAJOR_CIVS && GET_PLAYER(eCityOwner).isMinorCiv());
			const bool bMinorHadReligionBefore = bMinorCityStateTarget && (pCity->GetCityReligions()->GetReligiousMajority() != NO_RELIGION);
#endif
			int iConversionStrength = GetConversionStrength();
			CvGameReligions* pReligions = GC.getGame().GetGameReligions();
			ReligionTypes eReligion = GetReligionData()->GetReligion();
			if(eReligion > RELIGION_PANTHEON)
			{
				const CvReligion* pReligion = pReligions->GetReligion(eReligion, getOwner());
				if(pReligion)
				{
					iScienceBonus = pReligion->m_Beliefs.GetSciencePerOtherReligionFollower();
					if(iScienceBonus > 0)
					{

						ReligionTypes eCurrentReligion = pCity->GetCityReligions()->GetReligiousMajority();
						if (eCurrentReligion != NO_RELIGION && eCurrentReligion != eReligion)
						{
							iScienceBonus *= pCity->GetCityReligions()->GetFollowersOtherReligions(eReligion);
						}
						else
						{
							iScienceBonus = 0;
						}
					}
				}
			}

            #ifdef LEKMOD_PROMO_YIELD_FROM_CONVERSION

            int iFollowersBefore = pCity->GetCityReligions()->GetNumFollowers(eReligion);
            ReligionTypes eMajorityBefore = pCity->GetCityReligions()->GetReligiousMajority();
            #endif
            if(IsGreatPerson())
			{
				pCity->GetCityReligions()->AddProphetSpread(eReligion, iConversionStrength, getOwner());
			}
			else
			{
				pCity->GetCityReligions()->AddReligiousPressure(FOLLOWER_CHANGE_MISSIONARY, eReligion, iConversionStrength, getOwner());
			}
            #ifdef LEKMOD_PROMO_YIELD_FROM_CONVERSION

            int iFollowersAfter = pCity->GetCityReligions()->GetNumFollowers(eReligion);
            int iDeltaFollowers = std::max(0, iFollowersAfter - iFollowersBefore);
            ReligionTypes eMajorityAfter = pCity->GetCityReligions()->GetReligiousMajority();
            
            if (iDeltaFollowers > 0 && getUnitInfo().IsSpreadReligion())
            {
                bool bMajorityConversion = (eMajorityAfter == eReligion && eMajorityBefore != eReligion);
				CvPlayer& kPlayer = GET_PLAYER(getOwner());
				kPlayer.DoYieldsFromConversion(this, pCity, iDeltaFollowers, bMajorityConversion, getX(), getY(), 0);
            }
            #endif
			GetReligionData()->SetSpreadsLeft(GetReligionData()->GetSpreadsLeft() - 1);

			if (pCity->plot() && pCity->plot()->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF)
			{
				const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(GetReligionData()->GetReligion(), m_eOwner);
				Localization::String strReligionName = Localization::Lookup(pReligion->GetName());

				char text[256] = {0};
				sprintf_s(text, "[COLOR_WHITE]%s: %d [ICON_PEACE][ENDCOLOR]",
					strReligionName.toUTF8(),
					iConversionStrength / GC.getRELIGION_MISSIONARY_PRESSURE_MULTIPLIER());
				float fDelay = 0.0f;
				DLLUI->AddPopupText(pCity->getX(), pCity->getY(), text, fDelay);
			}

			if (iScienceBonus > 0)
			{
				CvPlayer &kPlayer = GET_PLAYER(m_eOwner);

				TechTypes eCurrentTech = kPlayer.GetPlayerTechs()->GetCurrentResearch();
				if(eCurrentTech == NO_TECH)
				{
					kPlayer.changeOverflowResearch(iScienceBonus);
				}
				else
				{
					CvTeam &kTeam = GET_TEAM(kPlayer.getTeam());
					kTeam.GetTeamTechs()->ChangeResearchProgress(eCurrentTech, iScienceBonus, kPlayer.GetID());
				}

				if (pCity->plot() && pCity->plot()->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF)
				{
					char text[256] = {0};
					sprintf_s(text, "[COLOR_BLUE]+%d[ENDCOLOR][ICON_RESEARCH]", iScienceBonus);
					float fDelay = GC.getPOST_COMBAT_TEXT_DELAY() * 2;
					DLLUI->AddPopupText(pCity->getX(), pCity->getY(), text, fDelay);
				}
			}

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
			if(bMinorCityStateTarget)
			{
				GET_PLAYER(eCityOwner).GetMinorCivAI()->DoPersonalityReligionSpreadInfluence(getOwner(), bMinorHadReligionBefore);
			}
#endif

			bool bShow = plot()->isActiveVisible(false);
			if(bShow)
			{
#ifndef REMOVE_GAMEPLAY_UNIT_ACTIVATE_ANIMATION
				auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
				gDLL->GameplayUnitActivate(pDllUnit.get());
#endif
			}

			if(GetReligionData()->GetSpreadsLeft() <= 0)
			{
				if(IsGreatPerson())
				{
					CvPlayer& kPlayer = GET_PLAYER(getOwner());
#ifdef AUI_DLLNETMESSAGEHANDLER_FIX_RESPAWN_PROPHET_IF_BEATEN_TO_LAST_RELIGION
					if (!IsIgnoreExpended())
#endif
					kPlayer.DoGreatPersonExpended(getUnitType());
				}

				kill(true);
			}
			else
			{
				if(bShow)
				{

					auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
					gDLL->GameplayUnitResetAnimationState(pDllUnit.get());
				}

				finishMoves();
			}


			const PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
			if(getOwner() == eActivePlayer && pCity->getOwner() != eActivePlayer)
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_XP1_17);
			}
		}
	}

	return false;
}


bool CvUnit::CanRemoveHeresy(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	CvCity* pCity;

	if(!m_pUnitInfo->IsRemoveHeresy())
	{
		return false;
	}

	if(GetReligionData()->GetReligion() == NO_RELIGION)
	{
		return false;
	}

	pCity = pPlot->getPlotCity();
	if(pCity == NULL)
	{
		pCity = pPlot->GetAdjacentCity();
		if(pCity == NULL)
		{
			return false;
		}
		else if(pCity->getOwner() != getOwner())
		{
			return false;
		}
	}

	if(!pCity->GetCityReligions()->IsReligionHereOtherThan(GetReligionData()->GetReligion()))
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

	return true;
}


bool CvUnit::DoRemoveHeresy()
{
	CvCity* pCity = GetSpreadReligionTargetCity();

	if (pCity != NULL)
	{
		if(CanRemoveHeresy(plot()))
		{
			bool bShow = plot()->isActiveVisible(false);
			if(bShow)
			{
#ifndef REMOVE_GAMEPLAY_UNIT_ACTIVATE_ANIMATION
				auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
				gDLL->GameplayUnitActivate(pDllUnit.get());
#endif
			}

			pCity->GetCityReligions()->RemoveOtherReligions(GetReligionData()->GetReligion(), getOwner());

			if(getOwner() == GC.getGame().getActivePlayer())
			{
				CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
				if(strcmp(kPlayer.getCivilizationTypeKey(), "CIVILIZATION_SPAIN") == 0)
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_XP1_24);
				}
			}
			
			kill(true);
		}
	}

	return false;
}


int CvUnit::GetNumFollowersAfterSpread() const
{
	int iRtnValue = 0;
	CvCity* pCity = GetSpreadReligionTargetCity();

	if (pCity != NULL)
	{
		if(IsGreatPerson())
		{
			iRtnValue = pCity->GetCityReligions()->GetNumFollowersAfterProphetSpread (GetReligionData()->GetReligion(), GetConversionStrength());
		}
		else
		{
			iRtnValue = pCity->GetCityReligions()->GetNumFollowersAfterSpread (GetReligionData()->GetReligion(), GetConversionStrength());
		}
	}

	return iRtnValue;
}


ReligionTypes CvUnit::GetMajorityReligionAfterSpread() const
{
	ReligionTypes eRtnValue = NO_RELIGION;
	CvCity* pCity = GetSpreadReligionTargetCity();

	if (pCity != NULL)
	{
		if(IsGreatPerson())
		{
			eRtnValue = pCity->GetCityReligions()->GetMajorityReligionAfterProphetSpread (GetReligionData()->GetReligion(), GetConversionStrength());
		}
		else
		{
			eRtnValue = pCity->GetCityReligions()->GetMajorityReligionAfterSpread (GetReligionData()->GetReligion(), GetConversionStrength());
		}
	}

	return eRtnValue;
}


CvCity *CvUnit::GetSpreadReligionTargetCity() const
{
	CvCity* pCity = NULL;
	CvPlot* pPlot = plot();

	if(pPlot)
	{
		pCity = pPlot->getPlotCity();
		if(pCity == NULL)
		{
			pCity = pPlot->GetAdjacentCity();
			if(pCity == NULL)
			{
				return NULL;
			}
		}
	}
	return pCity;
}


int CvUnit::GetConversionStrength() const
{
	int iReligiousStrength = GC.getRELIGION_MISSIONARY_PRESSURE_MULTIPLIER() * GetReligionData()->GetReligiousStrength();
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	ReligionTypes eReligion = GetReligionData()->GetReligion();
	if(eReligion > RELIGION_PANTHEON)
	{
		const CvReligion* pReligion = pReligions->GetReligion(eReligion, getOwner());
		if(pReligion)
		{
			int iStrengthMod;
			if (IsGreatPerson())
			{
				iStrengthMod = pReligion->m_Beliefs.GetProphetStrengthModifier();
			}
			else
			{
				iStrengthMod = pReligion->m_Beliefs.GetMissionaryStrengthModifier();
			}

			if(iStrengthMod != 0)
			{
				iReligiousStrength *= (100 + iStrengthMod);
				iReligiousStrength /= 100;
			}
		}
	}

	return iReligiousStrength;
}


bool CvUnit::canDiscover(const CvPlot*          , bool bTestVisible) const
{
	VALIDATE_OBJECT
	if(getUnitInfo().GetNumFreeTechs() == 0 && getUnitInfo().GetBaseBeakersTurnsToCount() == 0)
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

	if(!bTestVisible)
	{
		if(GET_PLAYER(getOwner()).getNumCities() == 0)
		{
			return false;
		}
	}

	return true;
}


int CvUnit::getDiscoverAmount()
{
	int iValue = 0;
	CvPlot* pPlot = plot();
	if(canDiscover(pPlot))
	{
		CvPlayer* pPlayer = &GET_PLAYER(getOwner());
		CvAssertMsg(pPlayer, "Owner of unit not expected to be NULL. Please send Anton your save file and version.");
		if (pPlayer)
		{



#ifdef DECREASE_BULB_AMOUNT_OVER_TIME
			if (GC.getGame().isOption("GAMEOPTION_NO_SCIENTIST_SAVING"))
			{
				int iPreviousTurnsToCount = std::max(m_pUnitInfo->GetBaseBeakersTurnsToCount() - std::max(GC.getGame().getGameTurn() - GetScientistBirthTurn() - 1, 0), 0);
				iValue = pPlayer->GetScienceYieldFromPreviousTurns(GetScientistBirthTurn(), iPreviousTurnsToCount);
			}
			else
			{
				iValue = GetResearchBulbAmount();
			}
#else
			iValue = GetResearchBulbAmount();
#endif
			if (pPlayer->GetGreatScientistBeakerMod() != 0)
			{
				iValue += (iValue * pPlayer->GetGreatScientistBeakerMod()) / 100;
				iValue = MAX(iValue, 0);
			}


			iValue *= GC.getGame().getGameSpeedInfo().getResearchPercent();
			iValue /= 100;
		}
	}
	return iValue;
}


bool CvUnit::discover()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();

	if(!canDiscover(pPlot))
	{
		return false;
	}

	CvPlayer* pPlayer = &GET_PLAYER(getOwner());
	CvAssertMsg(pPlayer, "Owner of unit not expected to be NULL. Please send Anton your save file and version.");
	if (!pPlayer) return false;
	CvTeam* pTeam = &GET_TEAM(pPlayer->getTeam());
	CvAssertMsg(pTeam, "Owner team of unit not expected to be NULL. Please send Anton your save file and version.");
	if (!pTeam) return false;


	int iBeakersBonus = getDiscoverAmount();
	TechTypes eCurrentTech = pPlayer->GetPlayerTechs()->GetCurrentResearch();
	if(eCurrentTech == NO_TECH)
	{
		pPlayer->changeOverflowResearch(iBeakersBonus);
	}
	else
	{
		pTeam->GetTeamTechs()->ChangeResearchProgress(eCurrentTech, iBeakersBonus, getOwner());
	}


	int iNumFreeTechs = m_pUnitInfo->GetNumFreeTechs();
	if(!isHuman())
	{
		for(int iI = 0; iI < iNumFreeTechs; iI++)
		{
			pPlayer->AI_chooseFreeTech();
		}
	}
	else
	{
		CvString strBuffer = GetLocalizedText("TXT_KEY_MISC_GREAT_PERSON_CHOOSE_TECH");
		pPlayer->chooseTech(iNumFreeTechs, strBuffer.GetCString());
	}

	if(pPlot->isActiveVisible(false))
	{
#ifndef REMOVE_GAMEPLAY_UNIT_ACTIVATE_ANIMATION
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
#endif
	}

	if(IsGreatPerson())
	{
#ifdef AUI_DLLNETMESSAGEHANDLER_FIX_RESPAWN_PROPHET_IF_BEATEN_TO_LAST_RELIGION
		if (!IsIgnoreExpended())
#endif
		pPlayer->DoGreatPersonExpended(getUnitType());
	}

	kill(true);

	return true;
}



bool CvUnit::IsCanRushBuilding(CvCity* pCity, bool bTestVisible) const
{
	VALIDATE_OBJECT
	if(pCity == NULL)
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

	if(!m_pUnitInfo->IsRushBuilding())
	{
		return false;
	}

	if(!bTestVisible)
	{
		if(pCity->getProductionTurnsLeft() == 1)
		{
			return false;
		}

		if(!pCity->isProductionBuilding())
		{
			return false;
		}
	}

	return true;
}



bool CvUnit::DoRushBuilding()
{
	VALIDATE_OBJECT
	CvCity* pCity = plot()->getPlotCity();

	if(pCity == NULL)
	{
		return false;
	}

	if(!IsCanRushBuilding(pCity, false))
	{
		return false;
	}

	pCity->setProduction(pCity->getProductionNeeded());

	if(plot()->isActiveVisible(false))
	{
#ifndef REMOVE_GAMEPLAY_UNIT_ACTIVATE_ANIMATION
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
#endif
	}

	kill(true);

	return true;
}



int CvUnit::getMaxHurryProduction(CvCity* pCity) const
{
	int iProduction;

	iProduction = (m_pUnitInfo->GetBaseHurry() + (m_pUnitInfo->GetHurryMultiplier() * pCity->getPopulation()));

	iProduction *= GC.getGame().getGameSpeedInfo().getUnitHurryPercent();
	iProduction /= 100;

	return std::max(0, iProduction);
}



int CvUnit::getHurryProduction(const CvPlot* pPlot) const
{
	CvCity* pCity;
	int iProduction;

	pCity = pPlot->getPlotCity();

	if(pCity == NULL)
	{
		return 0;
	}

	iProduction = getMaxHurryProduction(pCity);


	if (pCity->isProductionUnit())
	{
		UnitTypes eUnit = pCity->getProductionUnit();
		if (eUnit != -1)
		{
			CvUnitEntry *pkUnit = GC.GetGameUnits()->GetEntry(eUnit);
			if (pkUnit && pkUnit->GetSpaceshipProject() != NO_PROJECT)
			{
				iProduction *= 2;
			}
		}
	}


	iProduction = std::min(pCity->productionLeft(), iProduction);

	return std::max(0, iProduction);
}



bool CvUnit::canHurry(const CvPlot* pPlot, bool bTestVisible) const
{
	if(isDelayedDeath())
	{
		return false;
	}

	CvCity* pCity;

	if(getHurryProduction(pPlot) == 0)
	{
		return false;
	}

	pCity = pPlot->getPlotCity();

	if(pCity == NULL)
	{
		return false;
	}

	if(pCity->getProductionTurnsLeft() == 1)
	{
		return false;
	}

	if(!bTestVisible)
	{
		if(!(pCity->isProductionBuilding()))
		{
			if (!pCity->isProductionUnit())
			{
				return false;
			}
			else
			{
				UnitTypes eUnit = pCity->getProductionUnit();
				CvUnitEntry *pkUnit = GC.GetGameUnits()->GetEntry(eUnit);
#ifdef NQ_ALLOW_SS_PART_HURRY_BY_DEFAULT
				if (pkUnit)
#else
				if (pkUnit && GET_PLAYER(pCity->getOwner()).IsEnablesSSPartHurry())
#endif
				{
					if (pkUnit->GetSpaceshipProject() != NO_PROJECT)
					{
						return true;
					}
				}
				return false;
			}
		}
	}

	return true;
}



bool CvUnit::hurry()
{
	CvCity* pCity;

	CvPlot* pPlot = plot();

	if(!canHurry(pPlot))
	{
		return false;
	}

	pCity = pPlot->getPlotCity();

	if(pCity != NULL)
	{
		pCity->changeProduction(getHurryProduction(pPlot));
	}

	if(pPlot->isActiveVisible(false))
	{

		if(pCity != NULL && pCity->getProductionBuilding() != NO_BUILDING)
		{
			CvBuildingEntry* pkProductionBuildinInfo = GC.getBuildingInfo(pCity->getProductionBuilding());
			if(pkProductionBuildinInfo)
			{
				if(isLimitedWonderClass(pkProductionBuildinInfo->GetBuildingClassInfo()))
				{
					if(pCity->isHuman() && !GC.getGame().isGameMultiPlayer())
					{
						const char* pLeaderChar = GET_PLAYER(pCity->getOwner()).getLeaderTypeKey();
						CvString szLeader = pLeaderChar;
						if(szLeader == "LEADER_RAMESSES")
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_SPHINX);
						}
					}
				}
			}
		}
	}

	if(IsGreatPerson())
	{
		CvPlayer& kPlayer = GET_PLAYER(getOwner());
#ifdef AUI_DLLNETMESSAGEHANDLER_FIX_RESPAWN_PROPHET_IF_BEATEN_TO_LAST_RELIGION
		if (!IsIgnoreExpended())
#endif
		kPlayer.DoGreatPersonExpended(getUnitType());
	}

	kill(true);

	return true;
}



int CvUnit::getTradeGold(const CvPlot*          ) const
{
	VALIDATE_OBJECT
	int iGold;


	iGold = m_pUnitInfo->GetBaseGold();


	iGold += (m_pUnitInfo->GetNumGoldPerEra() * GET_TEAM(getTeam()).GetCurrentEra());

	iGold *= GC.getGame().getGameSpeedInfo().getUnitTradePercent();
	iGold /= 100;

	int iModifier = 100;
	iModifier += GetTradeMissionGoldModifier();
	iModifier += GET_PLAYER(getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_MISSION_GOLD_MODIFIER);

	iGold *= iModifier;
	iGold /= 100;

	return std::max(0, iGold);
}


int CvUnit::getTradeInfluence(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	int iInf = 0;
	if (pPlot && canTrade(pPlot))
	{
		PlayerTypes eMinor = pPlot->getOwner();
		CvAssertMsg(eMinor != NO_PLAYER, "Performing a trade mission and not in city state territory. This is bad. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
		if (eMinor != NO_PLAYER)
		{
			iInf =        GC.getMINOR_FRIENDSHIP_FROM_TRADE_MISSION();
#ifdef NQ_TRADE_MISSION_INFLUENCE_MODIFIER_FROM_POLICIES
			int iInfTimes100 = iInf * (100 + GetTradeMissionInfluenceModifier() + GET_PLAYER(getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_MISSION_INFLUENCE_MODIFIER));
#else
			int iInfTimes100 = iInf * (100 + GetTradeMissionInfluenceModifier());
#endif
			iInf = iInfTimes100 / 100;
		}
	}
	return iInf;
}


bool CvUnit::canTrade(const CvPlot* pPlot, bool bTestVisible) const
{
	VALIDATE_OBJECT
	if(isDelayedDeath())
	{
		return false;
	}

	if(m_pUnitInfo->GetBaseGold() == 0)
	{
		return false;
	}


	if(!bTestVisible)
	{

		if(pPlot->getOwner() == NO_PLAYER)
			return false;


		if(!GET_PLAYER(pPlot->getOwner()).isMinorCiv())
			return false;

		if(GET_TEAM(pPlot->getTeam()).isAtWar(getTeam()))
		{
			return false;
		}
	}

	return true;
}


bool CvUnit::trade()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();

	if(!canTrade(pPlot))
		return false;

	int iTradeGold = getTradeGold(pPlot);
	
	GET_PLAYER(getOwner()).GetTreasury()->ChangeGold(iTradeGold);


	PlayerTypes eMinor = pPlot->getOwner();
	CvAssertMsg(eMinor != NO_PLAYER, "Performing a trade mission and not in city state territory. This is bad. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	int iFriendship = getTradeInfluence(pPlot);
	GET_PLAYER(eMinor).GetMinorCivAI()->ChangeFriendshipWithMajor(getOwner(), iFriendship);

	if(getOwner() == GC.getGame().getActivePlayer())
	{
		DLLUI->AddUnitMessage(0, GetIDInfo(), getOwner(), true, GC.getEVENT_MESSAGE_TIME(), GetLocalizedText("TXT_KEY_MERCHANT_RESULT", iTradeGold, iFriendship));
	}

	if(pPlot->isActiveVisible(false))
	{
#ifndef REMOVE_GAMEPLAY_UNIT_ACTIVATE_ANIMATION
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
#endif
	}

	if(IsGreatPerson())
	{
		CvPlayer& kPlayer = GET_PLAYER(getOwner());
#ifdef AUI_DLLNETMESSAGEHANDLER_FIX_RESPAWN_PROPHET_IF_BEATEN_TO_LAST_RELIGION
		if (!IsIgnoreExpended())
#endif
		kPlayer.DoGreatPersonExpended(getUnitType());
	}

#ifdef NQ_TOURISM_FROM_TRADE_MISSIONS_FROM_POLICIES
	int iTourismFromTradeMissions = GET_PLAYER(getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TOURISM_FROM_TRADE_MISSIONS);
	if (iTourismFromTradeMissions > 0)
	{
		int iTourismBonus = iTradeGold * iTourismFromTradeMissions / 100;
		GET_PLAYER(getOwner()).GetCulture()->AddTourismAllKnownCivs(iTourismBonus);
	}
#endif

	kill(true);

	return true;
}


bool CvUnit::canBuyCityState(const CvPlot* pPlot, bool bTestVisible) const
{
	VALIDATE_OBJECT
	if(isDelayedDeath())
	{
		return false;
	}

	if (!m_pUnitInfo->IsCanBuyCityState())
	{
		return false;
	}

#ifdef NQM_AI_GIMP_NO_BUILDING_SETTLERS
	if ((GET_PLAYER(getOwner()).isHuman() && GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE)) || (!GET_PLAYER(getOwner()).isHuman() && (GC.getGame().isOption("GAMEOPTION_AI_TWEAKS") || GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_BUILDING_SETTLERS"))))
#else
	if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && GET_PLAYER(getOwner()).isHuman())
#endif
	{
		return false;
	}


	if(!bTestVisible)
	{

		if(pPlot->getOwner() == NO_PLAYER)
			return false;


		if (GET_PLAYER(m_eOwner).getNumCities() <= 0)
		{
			return false;
		}
#if !defined(LEKMOD_MERCHANT_BUYOUT_NOT_NOANNEXING)

		if(!GET_PLAYER(pPlot->getOwner()).isMinorCiv())
			return false;
#else

		if (!GET_PLAYER(pPlot->getOwner()).isMinorCiv())
		{
			return false;
		}
		else
		{

			CvMinorCivAI* pMinor = GET_PLAYER(pPlot->getOwner()).GetMinorCivAI();
			if(!pMinor)
				return false; 
			if (pMinor->GetAlly() != NO_PLAYER && pMinor->GetAlly() != getOwner())
			{
				return false;
			}
			for (int iPlayer = 0; iPlayer < MAX_MAJOR_CIVS; iPlayer++)
			{
				const PlayerTypes eLoopPlayer = (PlayerTypes)iPlayer;
				const CvPlayer& kLoopPlayer = GET_PLAYER(eLoopPlayer);
				if (eLoopPlayer == getOwner())
					continue;
				if (kLoopPlayer.isAlive() && kLoopPlayer.isMajorCiv())
				{
					const int iLastTurnAlliedThisMajor = pMinor->GetLastAllyTurnWithMajor(eLoopPlayer);
					if (iLastTurnAlliedThisMajor < 0)
						continue;
					const int iTurnDiff = GC.getGame().getGameTurn() - iLastTurnAlliedThisMajor;
					if (iTurnDiff < GC.getMINOR_CIV_BUYOUT_TURNS())
					{
						return false;
					}
				}
			}
			
		}
#endif
		if(GET_TEAM(pPlot->getTeam()).isAtWar(getTeam()))
		{
			return false;
		}
	}

	return true;
}


bool CvUnit::buyCityState()
{
	VALIDATE_OBJECT
	CvPlot* pPlot = plot();

	if (!canBuyCityState(pPlot))
	{
		return false;
	}


	PlayerTypes eMinor = pPlot->getOwner();
	CvAssertMsg(eMinor != NO_PLAYER, "Trying to buy a city state and not in city state territory. This is bad. Please send slewis this with your last 5 autosaves and what changelist # you're playing.");

	if (eMinor != NO_PLAYER)
	{
		CvCity* pMinorCapital = GET_PLAYER(eMinor).getCapitalCity();
		if (pMinorCapital)
		{
			pMinorCapital = NULL;
			int iNumUnits, iCapitalX, iCapitalY;
			GET_PLAYER(eMinor).GetMinorCivAI()->DoAcquire(getOwner(), iNumUnits, iCapitalX, iCapitalY);
			pMinorCapital = GC.getMap().plot(iCapitalX, iCapitalY)->getPlotCity();
			if (pMinorCapital)
			{

				pMinorCapital->ChangeResistanceTurns(-pMinorCapital->GetResistanceTurns());
			}

		}
	}

	if (getOwner() == GC.getGame().getActivePlayer())
	{
		DLLUI->AddUnitMessage(0, GetIDInfo(), getOwner(), true, GC.getEVENT_MESSAGE_TIME(), GetLocalizedText("TXT_KEY_VENETIAN_MERCHANT_BOUGHT_CITY_STATE"));
	}

	if (pPlot->isActiveVisible(false))
	{
#ifndef REMOVE_GAMEPLAY_UNIT_ACTIVATE_ANIMATION
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
#endif
	}

	if (IsGreatPerson())
	{
		CvPlayer& kPlayer = GET_PLAYER(getOwner());
#ifdef AUI_DLLNETMESSAGEHANDLER_FIX_RESPAWN_PROPHET_IF_BEATEN_TO_LAST_RELIGION
		if (!IsIgnoreExpended())
#endif
		kPlayer.DoGreatPersonExpended(getUnitType());
	}

	kill(true);

	return true;
}


bool CvUnit::canRepairFleet(const CvPlot*          , bool                 ) const
{
	VALIDATE_OBJECT
		if(isDelayedDeath())
		{
			return false;
		}

		if (!IsGreatAdmiral())
		{
			return false;
		}

		return true;
}


bool CvUnit::repairFleet()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();

	if(!canRepairFleet(pPlot))
		return false;


#ifdef AUI_WARNING_FIXES
	uint iUnitLoop;
#else
	int iUnitLoop;
#endif
	for (iUnitLoop = 0; iUnitLoop < pPlot->getNumUnits(); iUnitLoop++)
	{
		CvUnit *pUnit = pPlot->getUnitByIndex(iUnitLoop);
		if (pUnit->getOwner() == getOwner() && (pUnit->isEmbarked() || pUnit->getDomainType() == DOMAIN_SEA))
		{
			pUnit->changeDamage(-pUnit->getDamage());
		}
	}


	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot *pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			for (iUnitLoop = 0; iUnitLoop < pAdjacentPlot->getNumUnits(); iUnitLoop++)
			{	
				CvUnit *pUnit = pAdjacentPlot->getUnitByIndex(iUnitLoop);
				if (pUnit->getOwner() == getOwner() && (pUnit->isEmbarked() || pUnit->getDomainType() == DOMAIN_SEA))
				{
					pUnit->changeDamage(-pUnit->getDamage());
				}
			}
		}
	}

	if(pPlot->isActiveVisible(false))
	{
#ifndef REMOVE_GAMEPLAY_UNIT_ACTIVATE_ANIMATION
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
#endif
	}

	if(IsGreatPerson())
	{
		CvPlayer& kPlayer = GET_PLAYER(getOwner());
#ifdef AUI_DLLNETMESSAGEHANDLER_FIX_RESPAWN_PROPHET_IF_BEATEN_TO_LAST_RELIGION
		if (!IsIgnoreExpended())
#endif
		kPlayer.DoGreatPersonExpended(getUnitType());
	}

	kill(true);

	return true;
}



bool CvUnit::CanBuildSpaceship(const CvPlot* pPlot, bool bVisible) const
{
	VALIDATE_OBJECT

#ifdef NO_AI_PROJECTS
	if(!GET_PLAYER(getOwner()).isHuman() && GC.getGame().isOption("GAMEOPTION_AI_TWEAKS"))
		return false;
#endif


	if(isDelayedDeath())
		return false;

	if(GAMESTATE_ON != GC.getGame().getGameState())
		return false;


	ProjectTypes eSpaceshipProject = (ProjectTypes) getUnitInfo().GetSpaceshipProject();
	if(eSpaceshipProject == NO_PROJECT)
		return false;


	if(!bVisible)
	{
		CvCity* pCity = pPlot->getPlotCity();


		if(pCity == NULL)
			return false;


		if(pCity->getOwner() != getOwner())
			return false;


		if(!pCity->isCapital())
			return false;
	}

	return true;
}



bool CvUnit::DoBuildSpaceship()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();

	if(!CanBuildSpaceship(pPlot, false))
		return false;

	CvCity* pCity = pPlot->getPlotCity();


	if(pCity == NULL)
		return false;


	ProjectTypes eSpaceshipProject = (ProjectTypes)m_pUnitInfo->GetSpaceshipProject();
	if(eSpaceshipProject == NO_PROJECT)
		return false;

	pCity->CreateProject(eSpaceshipProject);

	kill(true);

	return true;
}


bool CvUnit::CanCultureBomb(const CvPlot* pPlot, bool bTestVisible) const
{
	VALIDATE_OBJECT

	if(m_pUnitInfo->GetCultureBombRadius() <= 0)
		return false;

	if(isDelayedDeath())
		return false;


	if(!bTestVisible)
	{

		if(GET_PLAYER(getOwner()).getCultureBombTimer() > 0)
			return false;

		if(pPlot != NULL)
		{

			if(pPlot->getOwner() != NO_PLAYER && pPlot->getOwner() != getOwner())
				return false;


			bool bFoundAdjacent = false;

			CvPlot* pLoopPlot;
			for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

				if(pLoopPlot != NULL)
				{
					if(pLoopPlot->getOwner() == getOwner())
					{
						bFoundAdjacent = true;
						break;
					}
				}
			}

			if(!bFoundAdjacent)
				return false;
		}
	}

	return true;
}


bool CvUnit::DoCultureBomb()
{

	CvPlot* pThisPlot = plot();
	if(!CanCultureBomb(pThisPlot))
		return false;

	CvUnitEntry *pkUnitEntry = GC.getUnitInfo(getUnitType());
	if (pkUnitEntry)
	{

		int iCooldown =        GC.getCULTURE_BOMB_COOLDOWN();

		CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
		kPlayer.changeCultureBombTimer(iCooldown);

		PerformCultureBomb(pkUnitEntry->GetCultureBombRadius());

		if(pThisPlot->isActiveVisible(false))
		{
#ifndef REMOVE_GAMEPLAY_UNIT_ACTIVATE_ANIMATION
			auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
			gDLL->GameplayUnitActivate(pDllUnit.get());
#endif
		}

		if(IsGreatPerson())
		{
#ifdef AUI_DLLNETMESSAGEHANDLER_FIX_RESPAWN_PROPHET_IF_BEATEN_TO_LAST_RELIGION
			if (!IsIgnoreExpended())
#endif
			kPlayer.DoGreatPersonExpended(getUnitType());
		}

		kill(true);

		return true;
	}

	return false;
}


void CvUnit::PerformCultureBomb(int iRadius)
{
	CvPlot* pThisPlot = plot();

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());


	int iBestCityID = -1;


	if(pThisPlot->getOwner() == getOwner() && pThisPlot->GetCityPurchaseID() != -1)
	{
		iBestCityID = pThisPlot->GetCityPurchaseID();
	}

	else
	{
		int iBestCityDistance = -1;

		int iDistance;

		CvCity* pLoopCity = NULL;
		int iLoop = 0;
		for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
		{
			CvPlot* pPlot = pLoopCity->plot();
			if(pPlot)
			{
				iDistance = plotDistance(getX(), getY(), pLoopCity->getX(), pLoopCity->getY());

				if(iBestCityDistance == -1 || iDistance < iBestCityDistance)
				{
					iBestCityID = pLoopCity->GetID();
					iBestCityDistance = iDistance;
				}
			}
		}
	}


	FStaticVector<bool, MAX_CIV_PLAYERS, true, c_eCiv5GameplayDLL, 0> vePlayersBombed;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		vePlayersBombed.push_back(false);
	}


	int iBombRange = iRadius;
	CvPlot* pLoopPlot;
#ifdef AUI_UNIT_CITADEL_RESISTANT_TO_CULTURE_BOMB
	ImprovementTypes eLoopImprovement = NO_IMPROVEMENT;
	const CvImprovementEntry* pImprovementInfo = NULL;
	const CvPlot* pLoopPlot2 = NULL;
	int iI;
#endif
#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX, iDX;
	for (int iDY = -iBombRange; iDY <= iBombRange; iDY++)
	{
		iMaxDX = iBombRange - MAX(0, iDY);
		for (iDX = -iBombRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
		{

			pLoopPlot = plotXY(getX(), getY(), iDX, iDY);
#else
	for(int i = -iBombRange; i <= iBombRange; ++i)
	{
		for(int j = -iBombRange; j <= iBombRange; ++j)
		{
			pLoopPlot = ::plotXYWithRangeCheck(getX(), getY(), i, j, iBombRange);
#endif

			if(pLoopPlot == NULL)
				continue;


			if(pLoopPlot->getOwner() == getOwner())
				continue;


			if(pLoopPlot->isCity())
				continue;

#ifdef AUI_UNIT_CITADEL_RESISTANT_TO_CULTURE_BOMB
			eLoopImprovement = pLoopPlot->getRevealedImprovementType(getTeam());
			if (eLoopImprovement != NO_IMPROVEMENT)
			{
				pImprovementInfo = GC.getImprovementInfo(eLoopImprovement);
				if (pImprovementInfo && pImprovementInfo->GetCultureBombRadius() > 0)
				{
					bool bPlotHasResistingCitadel = false;

					for (iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
					{
						pLoopPlot2 = plotDirection(pLoopPlot->getX(), pLoopPlot->getY(), (DirectionTypes)iI);
						if (pLoopPlot2 != NULL && pLoopPlot2->getOwner() == pLoopPlot->getOwner())
						{
							if (plotDistance(getX(), getY(), pLoopPlot2->getX(), pLoopPlot2->getY()) > iBombRange && !pLoopPlot->isCity())
							{
								eLoopImprovement = pLoopPlot->getRevealedImprovementType(getTeam());

								if (eLoopImprovement == NO_IMPROVEMENT ||
									(GC.getImprovementInfo(eLoopImprovement) && GC.getImprovementInfo(eLoopImprovement)->GetCultureBombRadius() <= 0))
								{
									bPlotHasResistingCitadel = true;
									break;
								}
							}
						}
					}

					if (bPlotHasResistingCitadel)
						continue;
				}
			}
#endif

			if(pLoopPlot->getOwner() != NO_PLAYER){

				if(pLoopPlot->getOwner() != getOwner() && !vePlayersBombed[pLoopPlot->getOwner()]){
					CvNotifications* pNotifications = GET_PLAYER(pLoopPlot->getOwner()).GetNotifications();
					if(pNotifications){
						CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_GREAT_ARTIST_STOLE_PLOT", GET_PLAYER(getOwner()).getNameKey());
						CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_GREAT_ARTIST_STOLE_PLOT", GET_PLAYER(getOwner()).getNameKey());
						pNotifications->Add(NOTIFICATION_GENERIC, strBuffer, strSummary, pLoopPlot->getX(), pLoopPlot->getY(), -1);
					}
				}
				vePlayersBombed[pLoopPlot->getOwner()] = true;
			}


			pLoopPlot->setOwner(getOwner(), iBestCityID);
		}
	}

	bool bAlreadyShownLeader = false;


	CvPlayer* pPlayer;
	for(int iSlotLoop = 0; iSlotLoop < MAX_CIV_PLAYERS; iSlotLoop++)
	{
		if(vePlayersBombed[iSlotLoop])
		{
			pPlayer = &GET_PLAYER((PlayerTypes) iSlotLoop);
			TeamTypes eOtherTeam = pPlayer->getTeam();


			if(pPlayer->isHuman())
				continue;


			if(pPlayer->isMinorCiv())
			{
				int iFriendship =         GC.getCULTURE_BOMB_MINOR_FRIENDSHIP_CHANGE();
				pPlayer->GetMinorCivAI()->ChangeFriendshipWithMajor(getOwner(), iFriendship);
			}

			else
			{
				pPlayer->GetDiplomacyAI()->ChangeNumTimesCultureBombed(getOwner(), 1);


				if(getTeam() != eOtherTeam && !GET_TEAM(eOtherTeam).isAtWar(getTeam()) && !CvPreGame::isNetworkMultiplayerGame() && GC.getGame().getActivePlayer() == getOwner() && !bAlreadyShownLeader)
				{
					bAlreadyShownLeader = true;

					DLLUI->SetForceDiscussionModeQuitOnBack(true);
					const char* strText = pPlayer->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_CULTURE_BOMBED);
					gDLL->GameplayDiplomacyAILeaderMessage(pPlayer->GetID(), DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
				}
			}
		}
	}
}


void CvUnit::PerformNeutralCultureBomb(int iRadius)
{
	CvPlot* pThisPlot = plot();

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());


	int iBestCityID = -1;


	if(pThisPlot->getOwner() == getOwner() && pThisPlot->GetCityPurchaseID() != -1)
	{
		iBestCityID = pThisPlot->GetCityPurchaseID();
	}

	else
	{
		int iBestCityDistance = -1;

		int iDistance;

		CvCity* pLoopCity = NULL;
		int iLoop = 0;
		for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
		{
			CvPlot* pPlot = pLoopCity->plot();
			if(pPlot)
			{
				iDistance = plotDistance(getX(), getY(), pLoopCity->getX(), pLoopCity->getY());

				if(iBestCityDistance == -1 || iDistance < iBestCityDistance)
				{
					iBestCityID = pLoopCity->GetID();
					iBestCityDistance = iDistance;
				}
			}
		}
	}


	FStaticVector<bool, MAX_CIV_PLAYERS, true, c_eCiv5GameplayDLL, 0> vePlayersBombed;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		vePlayersBombed.push_back(false);
	}


	int iBombRange = iRadius;
	CvPlot* pLoopPlot;
#ifdef AUI_UNIT_CITADEL_RESISTANT_TO_CULTURE_BOMB
	ImprovementTypes eLoopImprovement = NO_IMPROVEMENT;
	const CvImprovementEntry* pImprovementInfo = NULL;
	const CvPlot* pLoopPlot2 = NULL;
	int iI;
#endif
#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX, iDX;
	for (int iDY = -iBombRange; iDY <= iBombRange; iDY++)
	{
		iMaxDX = iBombRange - MAX(0, iDY);
		for (iDX = -iBombRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
		{

			pLoopPlot = plotXY(getX(), getY(), iDX, iDY);
#else
	for(int i = -iBombRange; i <= iBombRange; ++i)
	{
		for(int j = -iBombRange; j <= iBombRange; ++j)
		{
			pLoopPlot = ::plotXYWithRangeCheck(getX(), getY(), i, j, iBombRange);
#endif

			if(pLoopPlot == NULL)
				continue;


			if(pLoopPlot->getOwner() == getOwner())
				continue;


			if(pLoopPlot->getOwner() != NO_PLAYER )
				continue;


			if(pLoopPlot->isCity())
				continue;

#ifdef AUI_UNIT_CITADEL_RESISTANT_TO_CULTURE_BOMB
			eLoopImprovement = pLoopPlot->getRevealedImprovementType(getTeam());
			if (eLoopImprovement != NO_IMPROVEMENT)
			{
				pImprovementInfo = GC.getImprovementInfo(eLoopImprovement);
				if (pImprovementInfo && pImprovementInfo->GetCultureBombRadius() > 0)
				{
					bool bPlotHasResistingCitadel = false;

					for (iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
					{
						pLoopPlot2 = plotDirection(pLoopPlot->getX(), pLoopPlot->getY(), (DirectionTypes)iI);
						if (pLoopPlot2 != NULL && pLoopPlot2->getOwner() == pLoopPlot->getOwner())
						{
							if (plotDistance(getX(), getY(), pLoopPlot2->getX(), pLoopPlot2->getY()) > iBombRange && !pLoopPlot->isCity())
							{
								eLoopImprovement = pLoopPlot->getRevealedImprovementType(getTeam());

								if (eLoopImprovement == NO_IMPROVEMENT ||
									(GC.getImprovementInfo(eLoopImprovement) && GC.getImprovementInfo(eLoopImprovement)->GetCultureBombRadius() <= 0))
								{
									bPlotHasResistingCitadel = true;
									break;
								}
							}
						}
					}

					if (bPlotHasResistingCitadel)
						continue;
				}
			}
#endif

			if(pLoopPlot->getOwner() != NO_PLAYER){

				if(pLoopPlot->getOwner() != getOwner() && !vePlayersBombed[pLoopPlot->getOwner()]){
					CvNotifications* pNotifications = GET_PLAYER(pLoopPlot->getOwner()).GetNotifications();
					if(pNotifications){
						CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_GREAT_ARTIST_STOLE_PLOT", GET_PLAYER(getOwner()).getNameKey());
						CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_GREAT_ARTIST_STOLE_PLOT", GET_PLAYER(getOwner()).getNameKey());
						pNotifications->Add(NOTIFICATION_GENERIC, strBuffer, strSummary, pLoopPlot->getX(), pLoopPlot->getY(), -1);
					}
				}
				vePlayersBombed[pLoopPlot->getOwner()] = true;
			}


			pLoopPlot->setOwner(getOwner(), iBestCityID);
		}
	}

	bool bAlreadyShownLeader = false;


	CvPlayer* pPlayer;
	for(int iSlotLoop = 0; iSlotLoop < MAX_CIV_PLAYERS; iSlotLoop++)
	{
		if(vePlayersBombed[iSlotLoop])
		{
			pPlayer = &GET_PLAYER((PlayerTypes) iSlotLoop);
			TeamTypes eOtherTeam = pPlayer->getTeam();


			if(pPlayer->isHuman())
				continue;


			if(pPlayer->isMinorCiv())
			{
				int iFriendship =         GC.getCULTURE_BOMB_MINOR_FRIENDSHIP_CHANGE();
				pPlayer->GetMinorCivAI()->ChangeFriendshipWithMajor(getOwner(), iFriendship);
			}

			else
			{
				pPlayer->GetDiplomacyAI()->ChangeNumTimesCultureBombed(getOwner(), 1);


				if(getTeam() != eOtherTeam && !GET_TEAM(eOtherTeam).isAtWar(getTeam()) && !CvPreGame::isNetworkMultiplayerGame() && GC.getGame().getActivePlayer() == getOwner() && !bAlreadyShownLeader)
				{
					bAlreadyShownLeader = true;

					DLLUI->SetForceDiscussionModeQuitOnBack(true);
					const char* strText = pPlayer->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_CULTURE_BOMBED);
					gDLL->GameplayDiplomacyAILeaderMessage(pPlayer->GetID(), DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
				}
			}
		}
	}
}




bool CvUnit::canGoldenAge(const CvPlot* pPlot, bool bTestVisible) const
{
	VALIDATE_OBJECT
	if(!isGoldenAge())
	{
		return false;
	}


	if(m_pUnitInfo->IsSpreadReligion())
	{
		if(GetReligionData()->GetSpreadsLeft() < m_pUnitInfo->GetReligionSpreads())
		{
			return false;
		}
	}


	if(!bTestVisible)
	{

		if(pPlot->getOwner() != getOwner())
			return false;
	}

	return true;
}



bool CvUnit::goldenAge()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();

	if(!canGoldenAge(pPlot))
	{
		return false;
	}

	int iGoldenAgeTurns = GetGoldenAgeTurns();

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	kPlayer.changeGoldenAgeTurns(iGoldenAgeTurns);
	kPlayer.changeNumUnitGoldenAges(1);

	if(pPlot->isActiveVisible(false))
	{
#ifndef REMOVE_GAMEPLAY_UNIT_ACTIVATE_ANIMATION
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
#endif
	}

	if(IsGreatPerson())
	{
#ifdef AUI_DLLNETMESSAGEHANDLER_FIX_RESPAWN_PROPHET_IF_BEATEN_TO_LAST_RELIGION
		if (!IsIgnoreExpended())
#endif
		kPlayer.DoGreatPersonExpended(getUnitType());
	}

	kill(true);

	return true;
}


int CvUnit::GetGoldenAgeTurns() const
{
	int iGoldenAgeTurns = m_pUnitInfo->GetGoldenAgeTurns();


	if(iGoldenAgeTurns == 0)
		return 0;

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	int iMinTurns =       GC.getMIN_UNIT_GOLDEN_AGE_TURNS();
	if(iGoldenAgeTurns < iMinTurns)
		iGoldenAgeTurns = iMinTurns;


	int iLengthModifier = kPlayer.getGoldenAgeModifier();


	iLengthModifier += kPlayer.GetPlayerTraits()->GetGoldenAgeDurationModifier();
	if(iLengthModifier > 0)
		iGoldenAgeTurns = iGoldenAgeTurns * (100 + iLengthModifier) / 100;



	iGoldenAgeTurns *= GC.getGame().getGameSpeedInfo().getGoldenAgePercent();
	iGoldenAgeTurns /= 100;

	if(iGoldenAgeTurns < 1)
		iGoldenAgeTurns = 1;

	return iGoldenAgeTurns;
}


bool CvUnit::canGivePolicies(const CvPlot*          , bool                 ) const
{
	VALIDATE_OBJECT
#ifdef NET_FIX_SINGLE_USE_ABILITY_DUPE
	if (isDelayedDeath())
	{
		return false;
	}
#endif
	if(!isGivesPolicies() && getUnitInfo().GetBaseCultureTurnsToCount() == 0)
	{
		return false;
	}


	if(m_pUnitInfo->IsSpreadReligion())
	{
		if(GetReligionData()->GetSpreadsLeft() < m_pUnitInfo->GetReligionSpreads())
		{
			return false;
		}
	}

	return true;
}


int CvUnit::getGivePoliciesCulture()
{
	int iValue = 0;
	CvPlot* pPlot = plot();
	if(canGivePolicies(pPlot))
	{
		CvPlayerAI& kPlayer = GET_PLAYER(getOwner());


		int iPreviousTurnsToCount = getUnitInfo().GetBaseCultureTurnsToCount();
		if (iPreviousTurnsToCount != 0)
		{

			iValue = kPlayer.GetCultureYieldFromPreviousTurns(GC.getGame().getGameTurn(), iPreviousTurnsToCount);
		}


		iValue *= GC.getGame().getGameSpeedInfo().getCulturePercent();
		iValue /= 100;
	}
	return iValue;
}


bool CvUnit::givePolicies()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();
	if(!canGivePolicies(pPlot))
	{
		return false;
	}
	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());


	int iCultureBonus = getGivePoliciesCulture();
	if (iCultureBonus != 0)
	{
		kPlayer.changeJONSCulture(iCultureBonus);

#ifdef UPDATE_CULTURE_NOTIFICATION_DURING_TURN

		if (kPlayer.isAlive() && kPlayer.isHuman() && kPlayer.getNumCities() > 0)
		{
			kPlayer.TestMidTurnPolicyNotification();
		}
#endif
	}


	int iFreePolicies = m_pUnitInfo->GetFreePolicies();
	if (iFreePolicies != 0)
	{
		kPlayer.ChangeNumFreePolicies(iFreePolicies);
	}

	if(pPlot->isActiveVisible(false))
	{
#ifndef REMOVE_GAMEPLAY_UNIT_ACTIVATE_ANIMATION
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
#endif
	}

	if(IsGreatPerson())
	{
#ifdef AUI_DLLNETMESSAGEHANDLER_FIX_RESPAWN_PROPHET_IF_BEATEN_TO_LAST_RELIGION
		if (!IsIgnoreExpended())
#endif
		kPlayer.DoGreatPersonExpended(getUnitType());
	}

	kill(true);

	return true;
}


bool CvUnit::canBlastTourism(const CvPlot* pPlot, bool bTestVisible) const
{
	VALIDATE_OBJECT
#ifdef NET_FIX_SINGLE_USE_ABILITY_DUPE
	if (isDelayedDeath())
	{
		return false;
	}
#endif
	if(!isBlastTourism())
	{
		return false;
	}


	if(bTestVisible)
	{
		return true;
	}


	PlayerTypes eOwner = pPlot->getOwner();
	if (eOwner == NO_PLAYER)
	{
		return false;
	}

	CvPlayer &kTileOwner = GET_PLAYER(eOwner);
	if (kTileOwner.isAlive() && !kTileOwner.isMinorCiv() && eOwner != getOwner())
	{
		return true;
	}

	return false;
}


int CvUnit::getBlastTourism()
{
#ifdef AUI_UNIT_FIX_NO_DOUBLE_SPEED_MODIFIER_FOR_TOURISM_BLAST
	return GetTourismBlastStrength();
#else
	if (!canBlastTourism(plot()))
	{
		return 0;
	}

	int iTourismBlast = GetTourismBlastStrength();
	iTourismBlast = iTourismBlast * GC.getGame().getGameSpeedInfo().getCulturePercent() / 100;

	return iTourismBlast;
#endif
}


bool CvUnit::blastTourism()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();

	if(!canBlastTourism(pPlot))
	{
		return false;
	}

	int iTourismBlast = getBlastTourism();

	int iTourismBlastPercentOthers = m_pUnitInfo->GetOneShotTourismPercentOthers();
	PlayerTypes eOwner = pPlot->getOwner();
	CvPlayer &kUnitOwner = GET_PLAYER(getOwner());


	kUnitOwner.GetCulture()->ChangeInfluenceOn(eOwner, iTourismBlast);


	int iTourismBlastOthers = iTourismBlast * iTourismBlastPercentOthers / 100;
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if (eLoopPlayer != eOwner && eLoopPlayer != getOwner() && kUnitOwner.GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			kUnitOwner.GetCulture()->ChangeInfluenceOn(eLoopPlayer, iTourismBlastOthers);
		}
	}

	if(pPlot->isActiveVisible(false))
	{
#ifndef REMOVE_GAMEPLAY_UNIT_ACTIVATE_ANIMATION
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
#endif
	}

	if(IsGreatPerson())
	{
#ifdef AUI_DLLNETMESSAGEHANDLER_FIX_RESPAWN_PROPHET_IF_BEATEN_TO_LAST_RELIGION
		if (!IsIgnoreExpended())
#endif
		kUnitOwner.DoGreatPersonExpended(getUnitType());
	}

	kill(true);
 

 	if (pPlot->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF)
 	{
		CvString strInfluenceText;
		InfluenceLevelTypes eLevel = kUnitOwner.GetCulture()->GetInfluenceLevel(eOwner);

		if (eLevel == INFLUENCE_LEVEL_UNKNOWN)
			strInfluenceText = GetLocalizedText( "TXT_KEY_CO_UNKNOWN" );
		else if (eLevel == INFLUENCE_LEVEL_EXOTIC)
			strInfluenceText = GetLocalizedText( "TXT_KEY_CO_EXOTIC");
		else if (eLevel == INFLUENCE_LEVEL_FAMILIAR)
			strInfluenceText = GetLocalizedText( "TXT_KEY_CO_FAMILIAR");
		else if (eLevel == INFLUENCE_LEVEL_POPULAR)
			strInfluenceText = GetLocalizedText( "TXT_KEY_CO_POPULAR");
		else if (eLevel == INFLUENCE_LEVEL_INFLUENTIAL)
			strInfluenceText = GetLocalizedText( "TXT_KEY_CO_INFLUENTIAL");
		else if (eLevel == INFLUENCE_LEVEL_DOMINANT)
			strInfluenceText = GetLocalizedText( "TXT_KEY_CO_DOMINANT");

 		char text[256] = {0};
		sprintf_s(text, "[COLOR_WHITE]+%d [ICON_TOURISM][ENDCOLOR]   %s", iTourismBlast, strInfluenceText.c_str());
 		float fDelay = 0.0f;
 		DLLUI->AddPopupText(pPlot->getX(), pPlot->getY(), text, fDelay);
 	}


	if (GET_PLAYER(m_eOwner).isHuman() && !GC.getGame().isGameMultiPlayer())
	{
		if (strcmp(GET_PLAYER(eOwner).getCivilizationTypeKey(),   "CIVILIZATION_AMERICA") == 0 &&
			strcmp(GET_PLAYER(m_eOwner).getCivilizationTypeKey(), "CIVILIZATION_ENGLAND") == 0)
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_XP2_36);
		}

		ImprovementTypes eTempImprovement = pPlot->getImprovementType();
		if (eTempImprovement != NO_IMPROVEMENT)
		{
			CvImprovementEntry* pkImprovement = GC.getImprovementInfo(eTempImprovement);
			if (strcmp(pkImprovement->GetType(), "IMPROVEMENT_KASBAH") == 0)
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_XP2_37);
			}
		}
	}

	return true;
}


bool CvUnit::canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestVisible, bool bTestGold) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eBuild < GC.getNumBuildInfos() && eBuild >= 0, "Index out of bounds");

#if defined(v35_TRAITIFY)
	if (!(m_pUnitInfo->GetBuilds(eBuild)) && !GET_PLAYER(getOwner()).GetPlayerTraits()->IsBuildableByUnitCombat(eBuild, getUnitCombatType()))
#else
	if(!(m_pUnitInfo->GetBuilds(eBuild)))
#endif
	{
		return false;
	}

	CvBuildInfo *pkBuildInfo = GC.getBuildInfo(eBuild);
	if (!pkBuildInfo)
	{
		return false;
	}


	if(m_pUnitInfo->IsSpreadReligion())
	{
		if (GetReligionData()->GetReligion() != NO_RELIGION && GetReligionData()->GetSpreadsLeft() < m_pUnitInfo->GetReligionSpreads())
		{
			return false;
		}
	}

	if(!(GET_PLAYER(getOwner()).canBuild(pPlot, eBuild, false, bTestVisible, bTestGold)))
	{
		return false;
	}

	if(!pPlot->isValidDomainForAction(*this))
	{
		return false;
	}

 	if (pPlot->isWater())
 	{
		if ((isEmbarked() && !pkBuildInfo->IsCanBeEmbarked())                                                                         )
		{
 			return false;
		}
 	}

	if(!bTestVisible)
	{

		pPlot = plot();
		const IDInfo* pUnitNode = pPlot->headUnitNode();
		const CvUnit* pLoopUnit = NULL;

		while(pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(*pUnitNode);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);

			if(pLoopUnit && pLoopUnit != this)
			{
				if(pLoopUnit->IsWork() && pLoopUnit->getBuildType() != NO_BUILD)
				{
					return false;
				}
			}
		}
	}

	return true;
}



bool CvUnit::build(BuildTypes eBuild)
{
	VALIDATE_OBJECT
	bool bFinished = false;

	CvAssertMsg(eBuild < GC.getNumBuildInfos(), "Invalid Build");
	CvPlayer& kPlayer = GET_PLAYER(getOwner());

	CvBuildInfo* pkBuildInfo = NULL;
	if(eBuild != NO_BUILD)
	{
		pkBuildInfo = GC.getBuildInfo(eBuild);
	}
	CvPlot* pPlot = plot();


	if(!canBuild(pPlot, eBuild, false, false))
	{
		return false;
	}

	if(GC.getLogging() && GC.getAILogging())
	{
		if(pPlot->getBuildProgress(eBuild) == 0)
		{
			LogWorkerEvent(eBuild, true);
		}
	}

	int iStartedYet = pPlot->getBuildProgress(eBuild);


	if(iStartedYet == 0)
	{
		ImprovementTypes eImprovement = NO_IMPROVEMENT;

		if(pkBuildInfo)
		{
			eImprovement = (ImprovementTypes) pkBuildInfo->getImprovement();
		}

		if(eImprovement != NO_IMPROVEMENT)
		{
			if(pPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				CvImprovementEntry *pkImprovementEntry = GC.getImprovementInfo(eImprovement);
				if (!pkImprovementEntry || !pkImprovementEntry->IsRequiresImprovement())
				{
					pPlot->setImprovementType(NO_IMPROVEMENT);
				}
			}
		}

#ifndef NQ_FIX_BUILD_TIMES_UI


		bFinished = pPlot->changeBuildProgress(eBuild, workRate(false), getOwner());
#endif

	}
	if (!bFinished)
		bFinished = pPlot->changeBuildProgress(eBuild, workRate(false), getOwner());

	finishMoves();

	if(bFinished)
	{

		if(pkBuildInfo)
		{
			ImprovementTypes eImprovement = NO_IMPROVEMENT;
			RouteTypes eRoute = NO_ROUTE;

			if(pkBuildInfo->getImprovement() != NO_IMPROVEMENT)
			{
				eImprovement = (ImprovementTypes) pkBuildInfo->getImprovement();

				CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
				if(pkImprovementInfo)
				{
					if (pkImprovementInfo->GetCultureBombRadius() > 0)
					{
						PerformCultureBomb(pkImprovementInfo->GetCultureBombRadius());
					}

					if (pkImprovementInfo->GetCultureBombRadiusNeutral() > 0)
					{
						PerformNeutralCultureBomb(pkImprovementInfo->GetCultureBombRadiusNeutral());
					}





				}
			}
			else if(pkBuildInfo->getRoute() != NO_ROUTE)
			{
				eRoute = (RouteTypes) pkBuildInfo->getRoute();
			}

			int iNumResource = 0;


#ifdef AUI_WARNING_FIXES
			for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
			for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
			{
				if(eImprovement != NO_IMPROVEMENT)
				{
					CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
					if(pkImprovementInfo)
					{
						iNumResource = pkImprovementInfo->GetResourceQuantityRequirement(iResourceLoop);
					}
				}
				else if(eRoute != NO_ROUTE)
				{
					CvRouteInfo* pkRouteInfo = GC.getRouteInfo(eRoute);
					if(pkRouteInfo)
					{
						iNumResource = pkRouteInfo->getResourceQuantityRequirement(iResourceLoop);
					}
				}

				if(iNumResource > 0)
				{
					kPlayer.changeNumResourceUsed((ResourceTypes) iResourceLoop, -iNumResource);
				}
			}

			if(pkBuildInfo->isKill())
			{
				if (pPlot->isActiveVisible(false))
				{
#ifndef REMOVE_GAMEPLAY_UNIT_ACTIVATE_ANIMATION
					auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
					gDLL->GameplayUnitActivate(pDllUnit.get());
#endif
				}

				if(IsGreatPerson())
				{
#ifdef AUI_DLLNETMESSAGEHANDLER_FIX_RESPAWN_PROPHET_IF_BEATEN_TO_LAST_RELIGION
					if (!IsIgnoreExpended())
#endif
					kPlayer.DoGreatPersonExpended(getUnitType());
				}

				kill(true);
			}


			if(pkBuildInfo->getImprovement() != NO_IMPROVEMENT || pkBuildInfo->getRoute() != NO_ROUTE)
			{
				kPlayer.changeTotalImprovementsBuilt(1);
			}

			if(GC.getLogging() && GC.getAILogging())
			{
				LogWorkerEvent(eBuild, false);
			}
		}
	}
	else
	{
		if(pkBuildInfo)
		{
			ImprovementTypes eImprovement = NO_IMPROVEMENT;
			RouteTypes eRoute = NO_ROUTE;

			if(pkBuildInfo->getImprovement() != NO_IMPROVEMENT)
			{
				eImprovement = (ImprovementTypes) pkBuildInfo->getImprovement();
			}
			else if(pkBuildInfo->getRoute() != NO_ROUTE)
			{
				eRoute = (RouteTypes) pkBuildInfo->getRoute();
			}

			if(iStartedYet == 0)
			{
				if(pPlot->isVisible(GC.getGame().getActiveTeam(), true))
				{
					if(eImprovement != NO_IMPROVEMENT)
					{
						pPlot->setLayoutDirty(true);

					}
					else if(eRoute != NO_ROUTE)
					{
						pPlot->setLayoutDirty(true);
					}
				}
			}
		}
	}

	return bFinished;
}



bool CvUnit::canPromote(PromotionTypes ePromotion, int iLeaderUnitId) const
{
	VALIDATE_OBJECT
	if(iLeaderUnitId >= 0)
	{
		if(iLeaderUnitId == GetID())
		{
			return false;
		}


		UnitHandle pWarlord = GET_PLAYER(getOwner()).getUnit(iLeaderUnitId);
		if(pWarlord &&
		        NO_UNIT != pWarlord->getUnitType() &&
		        pWarlord->getUnitInfo().GetLeaderExperience() > 0 &&
		        NO_PROMOTION == pWarlord->getUnitInfo().GetLeaderPromotion() &&
		        canAcquirePromotionAny())
		{
			return true;
		}
	}

	if(ePromotion == NO_PROMOTION)
	{
		return false;
	}

	CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
	if(pkPromotionInfo == NULL)
	{
		return false;
	}
#ifdef PROMOTION_INSTA_HEAL_LOCKED
	if (isInstaHealLocked() && ePromotion == 0)
	{
		return false;
	}
#endif

	if(!canAcquirePromotion(ePromotion))
	{
		return false;
	}

	if(pkPromotionInfo->IsLeader())
	{
		if(iLeaderUnitId >= 0)
		{
			UnitHandle pWarlord = GET_PLAYER(getOwner()).getUnit(iLeaderUnitId);
			if(pWarlord && NO_UNIT != pWarlord->getUnitType())
			{
				return (pWarlord->getUnitInfo().GetLeaderPromotion() == ePromotion);
			}
		}
		return false;
	}
	else
	{
		if(!isPromotionReady())
		{
			return false;
		}
	}

	return true;
}


void CvUnit::promote(PromotionTypes ePromotion, int iLeaderUnitId)
{
	VALIDATE_OBJECT

	CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
	if(pkPromotionInfo == NULL)
	{
		return;
	}

	if(!canPromote(ePromotion, iLeaderUnitId))
	{
		return;
	}

	if(iLeaderUnitId >= 0)
	{
		UnitHandle pWarlord = GET_PLAYER(getOwner()).getUnit(iLeaderUnitId);
		if(pWarlord)
		{
			pWarlord->giveExperience();
			if(!pWarlord->getNameNoDesc().empty())
			{
				setName(pWarlord->getName());
			}


			m_eLeaderUnitType = pWarlord->getUnitType();
		}
	}

	if(!pkPromotionInfo->IsLeader())
	{
		changeLevel(1);
	}


	if(pkPromotionInfo->IsInstaHeal())
	{
		changeDamage(-GC.getINSTA_HEAL_RATE());
#ifdef NQM_UNIT_FIX_NO_DOUBLE_INSTAHEAL_ON_SAME_TURN
		setCanInstahealThisTurn(false);
#endif
	}

	else
	{
		setHasPromotion(ePromotion, true);
#if defined(LEKMOD_RETRAIN_MISSION)
		changeNumPlayerChosenPromotions(1);
		SetPromotionChosenByPlayer(ePromotion, true);
#endif
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem)
		{
			CvLuaArgsHandle args;
			args->Push(((int)getOwner()));
			args->Push(GetID());
			args->Push(ePromotion);

			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "UnitPromoted", args.get(), bResult);
		}
	}

	testPromotionReady();

	if(IsSelected())
	{
		DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
	}
	else
	{
		setInfoBarDirty(true);
	}
}


bool CvUnit::lead(int iUnitId)
{
	VALIDATE_OBJECT
	if(!canLead(plot(), iUnitId))
	{
		return false;
	}

	PromotionTypes eLeaderPromotion = (PromotionTypes)m_pUnitInfo->GetLeaderPromotion();

	if(-1 == iUnitId)
	{
		return false;
	}
	else
	{
		UnitHandle pUnit = GET_PLAYER(getOwner()).getUnit(iUnitId);

		if(!pUnit || !pUnit->canPromote(eLeaderPromotion, GetID()))
		{
			return false;
		}

		pUnit->promote(eLeaderPromotion, GetID());

		kill(true);

		return true;
	}
}



int CvUnit::canLead(const CvPlot* pPlot, int iUnitId) const
{
	VALIDATE_OBJECT
	if(isDelayedDeath())
	{
		return 0;
	}

	if(NO_UNIT == getUnitType())
	{
		return 0;
	}

	int iNumUnits = 0;
	CvUnitEntry& kUnitInfo = getUnitInfo();

	if(-1 == iUnitId)
	{
		const IDInfo* pUnitNode = pPlot->headUnitNode();
		while(pUnitNode != NULL)
		{
			const CvUnit* pUnit = ::getUnit(*pUnitNode);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);

			if(pUnit && pUnit != this && pUnit->getOwner() == getOwner() && pUnit->canPromote((PromotionTypes)kUnitInfo.GetLeaderPromotion(), GetID()))
			{
				++iNumUnits;
			}
		}
	}
	else
	{
		const UnitHandle pUnit = GET_PLAYER(getOwner()).getUnit(iUnitId);
		if(pUnit && pUnit != this && pUnit->canPromote((PromotionTypes)kUnitInfo.GetLeaderPromotion(), GetID()))
		{
			iNumUnits = 1;
		}
	}
	return iNumUnits;
}



int CvUnit::canGiveExperience(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	int iNumUnits = 0;

	if(NO_UNIT != getUnitType() && getUnitInfo().GetLeaderExperience() > 0)
	{
		const IDInfo* pUnitNode = pPlot->headUnitNode();
		while(pUnitNode != NULL)
		{
			const CvUnit* pUnit = ::getUnit(*pUnitNode);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);

			if(pUnit && pUnit != this && pUnit->getOwner() == getOwner() && pUnit->canAcquirePromotionAny())
			{
				++iNumUnits;
			}
		}
	}

	return iNumUnits;
}


bool CvUnit::giveExperience()
{
	VALIDATE_OBJECT
	CvPlot* pPlot = plot();

	if(pPlot)
	{
		int iNumUnits = canGiveExperience(pPlot);
		if(iNumUnits > 0)
		{
			int iTotalExperience = getStackExperienceToGive(iNumUnits);

			int iMinExperiencePerUnit = iTotalExperience / iNumUnits;
			int iRemainder = iTotalExperience % iNumUnits;

			IDInfo* pUnitNode = pPlot->headUnitNode();
			int i = 0;
			while(pUnitNode != NULL)
			{
				CvUnit* pUnit = ::getUnit(*pUnitNode);
				pUnitNode = pPlot->nextUnitNode(pUnitNode);

				if(pUnit && pUnit != this && pUnit->getOwner() == getOwner() && pUnit->canAcquirePromotionAny())
				{
					pUnit->changeExperience(i < iRemainder ? iMinExperiencePerUnit+1 : iMinExperiencePerUnit);
#ifndef AUI_UNIT_TEST_PROMOTION_READY_MOVED
					pUnit->testPromotionReady();
#endif
				}

				i++;
			}

			return true;
		}
	}

	return false;
}


int CvUnit::getStackExperienceToGive(int iNumUnits) const
{
	VALIDATE_OBJECT
	return (getUnitInfo().GetLeaderExperience() * (100 + std::min(50, (iNumUnits - 1) * GC.getWARLORD_EXTRA_EXPERIENCE_PER_UNIT_PERCENT()))) / 100;
}



bool CvUnit::isReadyForUpgrade() const
{
	VALIDATE_OBJECT

#ifdef NET_FIX_SINGLE_USE_ABILITY_DUPE
	if (isDelayedDeath())
	{
		return false;
	}
#endif

	if(m_iMoves <= 0)
	{
		return false;
	}

	return true;
}



bool CvUnit::CanUpgradeRightNow(bool bOnlyTestVisible) const
{
	VALIDATE_OBJECT

	if(!isReadyForUpgrade())
		return false;

	UnitTypes eUpgradeUnitType = GetUpgradeUnitType();


	if(eUpgradeUnitType == NO_UNIT)
		return false;

	CvUnitEntry* pUpgradeUnitInfo = GC.getUnitInfo(eUpgradeUnitType);
	if(pUpgradeUnitInfo == NULL)
		return false;


	TechTypes ePrereqTech = (TechTypes) pUpgradeUnitInfo->GetPrereqAndTech();

	if(ePrereqTech != NO_TECH && !GET_TEAM(getTeam()).GetTeamTechs()->HasTech(ePrereqTech))
		return false;

	CvPlot* pPlot = plot();


	if(!bOnlyTestVisible)
	{
		if(pPlot->getNumFriendlyUnitsOfType(this) > GC.getPLOT_UNIT_LIMIT())
		{
			return false;
		}


		if(pPlot->getOwner() != getOwner())
			return false;

		CvPlayerAI& kPlayer = GET_PLAYER(getOwner());


		if(kPlayer.GetTreasury()->GetGold() < upgradePrice(eUpgradeUnitType))
			return false;


		int iNumOfThisResourceAvailable;
		ResourceTypes eResource;
		int iNumResourceNeeded;
#ifdef AUI_WARNING_FIXES
		for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
		{
			eResource = (ResourceTypes) iResourceLoop;
			iNumResourceNeeded = pUpgradeUnitInfo->GetResourceQuantityRequirement(eResource);

			if(iNumResourceNeeded > 0)
			{

				iNumOfThisResourceAvailable = kPlayer.getNumResourceAvailable(eResource);

				iNumOfThisResourceAvailable += m_pUnitInfo->GetResourceQuantityRequirement(eResource);

				if(iNumOfThisResourceAvailable <= 0 || iNumOfThisResourceAvailable < iNumResourceNeeded)
					return false;
			}
		}

		if(getDomainType() == DOMAIN_AIR)
		{
			if(!pPlot->isCity() || isCargo())
			{
				return false;
			}
		}

		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem)
		{
			CvLuaArgsHandle args;
			args->Push(((int)getOwner()));
			args->Push(GetID());

			bool bResult = false;
			if (LuaSupport::CallTestAll(pkScriptSystem, "CanHaveAnyUpgrade", args.get(), bResult)) {
				if (bResult == false) {
					return false;
				}
			}
		}
	}

	return true;
}



UnitTypes CvUnit::GetUpgradeUnitType() const
{
	VALIDATE_OBJECT
	UnitTypes eUpgradeUnitType = NO_UNIT;

	CvCivilizationInfo& kCiv = GET_PLAYER(getOwner()).getCivilizationInfo();


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
			if(m_pUnitInfo->GetUpgradeUnitClass(iI))
			{
				eUpgradeUnitType = (UnitTypes) kCiv.getCivilizationUnits(iI);

				ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
				if (pkScriptSystem) 
				{
					CvLuaArgsHandle args;
					args->Push(((int)getOwner()));
					args->Push(GetID());
#ifdef AUI_WARNING_FIXES
					args->Push(int(iI));
#else
					args->Push(iI);
#endif
					args->Push(eUpgradeUnitType);

					bool bResult = false;
					if (LuaSupport::CallTestAll(pkScriptSystem, "CanHaveUpgrade", args.get(), bResult)) 
					{
						if (bResult == false) 
						{
							continue;
						}
					}
				}

				break;
			}
		}
	}

	return eUpgradeUnitType;
}



int CvUnit::upgradePrice(UnitTypes eUnit) const
{
	VALIDATE_OBJECT
	int iPrice = 0;

	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	if(pkUnitInfo == NULL)
	{
		return 0;
	}

	if(isBarbarian())
	{
		return 0;
	}

	iPrice =        GC.getBASE_UNIT_UPGRADE_COST();

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	iPrice += (std::max(0, (kPlayer.getProductionNeeded(eUnit) - kPlayer.getProductionNeeded(getUnitType()))) *       GC.getUNIT_UPGRADE_COST_PER_PRODUCTION());


	const TechTypes eTech = (TechTypes) pkUnitInfo->GetPrereqAndTech();
	CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
	if(pkTechInfo)
	{
		const EraTypes eUpgradeEra = (EraTypes) pkTechInfo->GetEra();

		double fMultiplier = 1.0f;
		fMultiplier += (eUpgradeEra*         GC.getUNIT_UPGRADE_COST_MULTIPLIER_PER_ERA());

		iPrice = int(iPrice * fMultiplier);
	}

	if(!isHuman() && !kPlayer.IsAITeammateOfHuman() && !isBarbarian())
	{
		iPrice *= GC.getGame().getHandicapInfo().getAIUnitUpgradePercent();
		iPrice /= 100;

		iPrice *= std::max(0, ((GC.getGame().getHandicapInfo().getAIPerEraModifier() * GET_TEAM(getTeam()).GetCurrentEra()) + 100));
		iPrice /= 100;
	}


	iPrice -= (iPrice * getUpgradeDiscount()) / 100;


	int iMod = kPlayer.GetUnitUpgradeCostMod();
	iPrice *= (100 + iMod);
	iPrice /= 100;


	iPrice = (int) pow((double) iPrice, (double)          GC.getUNIT_UPGRADE_COST_EXPONENT());


	int iDivisor =       GC.getUNIT_UPGRADE_COST_VISIBLE_DIVISOR();
	iPrice /= iDivisor;
	iPrice *= iDivisor;

	return iPrice;
}



CvUnit* CvUnit::DoUpgrade()
{
	VALIDATE_OBJECT

	UnitTypes eUnitType = GetUpgradeUnitType();


	int iUpgradeCost = upgradePrice(eUnitType);
	CvPlayerAI& thisPlayer = GET_PLAYER(getOwner());
	thisPlayer.GetTreasury()->LogExpenditure(getUnitInfo().GetText(), iUpgradeCost, 3);
	thisPlayer.GetTreasury()->ChangeGold(-iUpgradeCost);

#ifdef ENHANCED_GRAPHS
	thisPlayer.ChangeGoldSpentUpgrades(iUpgradeCost);
#endif


	CvUnit* pNewUnit = thisPlayer.initUnit(eUnitType, getX(), getY(), NO_UNITAI, NO_DIRECTION, false, false);

	if(NULL != pNewUnit)
	{
		if(GC.getGame().getActivePlayer() == getOwner())
		{
			auto_ptr<ICvUnit1> pDllNewUnit = GC.WrapUnitPointer(pNewUnit);
			DLLUI->selectUnit(pDllNewUnit.get(), true, false, false);
		}

		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem) 
		{
			CvLuaArgsHandle args;
			args->Push(((int)getOwner()));
			args->Push(GetID());
			args->Push(pNewUnit->GetID());
			args->Push(false);

			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "UnitUpgraded", args.get(), bResult);
		}

		pNewUnit->convert(this, true);
#if defined(LEKMOD_CONVERT_PROMOTIONS_UPGRADE)
		ConvertPromotions(this, pNewUnit);
#endif
#if defined(LEKMOD_RETRAIN_MISSION)
		int iOldNumPromos = getNumPlayerChosenPromotions();
		pNewUnit->changeNumPlayerChosenPromotions(iOldNumPromos);
#endif
		pNewUnit->setupGraphical();


		pNewUnit->finishMoves();

		kill(true);
	}

	if(isHuman() && !GC.getGame().isGameMultiPlayer() && GET_PLAYER(GC.getGame().getActivePlayer()).isLocalPlayer())
	{
		gDLL->UnlockAchievement(ACHIEVEMENT_UNIT_UPGRADE);
	}

	return pNewUnit;
}
#if defined(LEKMOD_CONVERT_PROMOTIONS_UPGRADE)
void CvUnit::ConvertPromotions(CvUnit* pOldUnit, CvUnit* pNewUnit)
{
	VALIDATE_OBJECT
	UnitCombatTypes eOldUnitCombat = pOldUnit->getUnitCombatType();
	UnitCombatTypes eNewUnitCombat = pNewUnit->getUnitCombatType();
	if (eOldUnitCombat == NO_UNITCOMBAT || eNewUnitCombat == NO_UNITCOMBAT)
		return;
	if (eOldUnitCombat != eNewUnitCombat)
	{
		for (int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); iPromotion++)
		{
			PromotionTypes eLoopPromotion = (PromotionTypes)iPromotion;
			if (!pOldUnit->isHasPromotion(eLoopPromotion))
				continue;
			CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eLoopPromotion);
			if (pkPromotionInfo == NULL)
				continue;
			if (pkPromotionInfo->IsCannotBeChosen())
				continue; 

			const int iNewPromo = pkPromotionInfo->GetUpgradeConversionPromotion(eNewUnitCombat);
			if (iNewPromo != NO_PROMOTION)
			{
				const PromotionTypes eNewPromotion = (PromotionTypes)iNewPromo;
				if (pOldUnit->IsPromotionChosenByPlayer(eLoopPromotion))
				{
					pNewUnit->SetPromotionChosenByPlayer(eLoopPromotion, false);
					pNewUnit->SetPromotionChosenByPlayer(eNewPromotion, true);
				}
				pNewUnit->setHasPromotion(eNewPromotion, true);
				pNewUnit->setHasPromotion(eLoopPromotion, false);
			}
		}
	}
}
#endif

HandicapTypes CvUnit::getHandicapType() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).getHandicapType();
}



CvCivilizationInfo& CvUnit::getCivilizationInfo() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).getCivilizationInfo();
}


CivilizationTypes CvUnit::getCivilizationType() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).getCivilizationType();
}


const char* CvUnit::getVisualCivAdjective(TeamTypes eForTeam) const
{
	VALIDATE_OBJECT
	if(getVisualOwner(eForTeam) == getOwner())
	{
		return GET_PLAYER(getOwner()).getCivilizationInfo().getAdjectiveKey();
	}

	return "";
}


SpecialUnitTypes CvUnit::getSpecialUnitType() const
{
	VALIDATE_OBJECT
	return ((SpecialUnitTypes)(m_pUnitInfo->GetSpecialUnitType()));
}


bool CvUnit::IsGreatPerson() const
{
	SpecialUnitTypes eSpecialUnitGreatPerson = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_PEOPLE");

	return (getSpecialUnitType() == eSpecialUnitGreatPerson);
}
#ifdef AUI_DLLNETMESSAGEHANDLER_FIX_RESPAWN_PROPHET_IF_BEATEN_TO_LAST_RELIGION
bool CvUnit::IsIgnoreExpended() const
{
	return m_bIsIgnoreExpended;
}
void CvUnit::SetIgnoreExpended(bool bNewValue)
{
	if (m_bIsIgnoreExpended != bNewValue)
		m_bIsIgnoreExpended = bNewValue;
}
#endif


UnitTypes CvUnit::getCaptureUnitType(CivilizationTypes eCivilization) const
{
	VALIDATE_OBJECT
	CvAssert(eCivilization != NO_CIVILIZATION);
	CvCivilizationInfo* pkCivilizationInfo = GC.getCivilizationInfo(eCivilization);
	if(pkCivilizationInfo == NULL)
	{
		return NO_UNIT;
	}

	return ((m_pUnitInfo->GetUnitCaptureClassType() == NO_UNITCLASS) ? NO_UNIT : (UnitTypes)pkCivilizationInfo->getCivilizationUnits(getUnitInfo().GetUnitCaptureClassType()));
}



UnitCombatTypes CvUnit::getUnitCombatType() const
{
	VALIDATE_OBJECT
	return ((UnitCombatTypes)(m_pUnitInfo->GetUnitCombatType()));
}


DomainTypes CvUnit::getDomainType() const
{
	VALIDATE_OBJECT
	DomainTypes eDomain = (DomainTypes) m_pUnitInfo->GetDomainType();

	if (eDomain == DOMAIN_HOVER)
	{
		CvPlot* pPlot = plot();
		CvAssertMsg(pPlot, "pPlot is unexpectedly NULL. Please send Anton your save file and version.");
		if (pPlot)
		{
			if (pPlot->isWater())
			{
				return DOMAIN_SEA;
			}
			else
			{
				return DOMAIN_LAND;
			}
		}
	}
	
	return eDomain;
}


int CvUnit::flavorValue(FlavorTypes eFlavor) const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->GetFlavorValue(eFlavor);
}



bool CvUnit::isBarbarian() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).isBarbarian();
}



bool CvUnit::isHuman() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).isHuman();
}




void CvUnit::DoTestBarbarianThreatToMinorsWithThisUnitsDeath(PlayerTypes eKillingPlayer)
{
	VALIDATE_OBJECT


	if(eKillingPlayer == NO_PLAYER)
		return;


	if(GET_PLAYER(eKillingPlayer).isMinorCiv())
		return;

	PlayerTypes eMinor;
	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		eMinor = (PlayerTypes) iMinorLoop;

		if(GET_PLAYER(eMinor).isAlive())
		{
			if(IsBarbarianUnitThreateningMinor(eMinor))
				GET_PLAYER(eMinor).GetMinorCivAI()->DoThreateningBarbKilled(eKillingPlayer, getX(), getY());
		}
	}
}



bool CvUnit::IsBarbarianUnitThreateningMinor(PlayerTypes eMinor)
{
	VALIDATE_OBJECT


	if(!isBarbarian())
		return false;


	if(plot()->getOwner() == eMinor)
		return true;


	CvPlot* pLoopPlot;
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(getX(), getY(), (DirectionTypes) iI);

		if(pLoopPlot != NULL)
		{
			if(pLoopPlot->getOwner() == eMinor)
				return true;
		}
	}

	return false;
}



int CvUnit::visibilityRange() const
{
	VALIDATE_OBJECT

	CvTeam& thisTeam = GET_TEAM(getTeam());

	int iRtnValue;

#ifdef LEK_EMBARK_VISIBILITY_FIX
	if(isEmbarked())
	{
		iRtnValue = max(1,GC.getEMBARKED_VISIBILITY_RANGE()) + m_iEmbarkExtraVisibility + thisTeam.getEmbarkedExtraSight();
	}
#else

	if(isEmbarked())
	{
		iRtnValue = GC.getEMBARKED_VISIBILITY_RANGE() + m_iEmbarkExtraVisibility + thisTeam.getEmbarkedExtraSight();
	}
#endif
	else
	{
		iRtnValue = m_pUnitInfo->GetBaseSightRange() + m_iExtraVisibilityRange;
	}

	return iRtnValue;
}








int CvUnit::baseMoves(DomainTypes eIntoDomain                  ) const
{
	VALIDATE_OBJECT
	CvTeam& thisTeam = GET_TEAM(getTeam());
	CvPlayer& thisPlayer = GET_PLAYER(getOwner());
	CvPlayerTraits* pTraits = thisPlayer.GetPlayerTraits();
	DomainTypes eDomain = getDomainType();
	CvPlayerPolicies *pPolicies = thisPlayer.GetPlayerPolicies();

	if((eIntoDomain == DOMAIN_SEA && CanEverEmbark()) || (eIntoDomain == NO_DOMAIN && isEmbarked()))
	{
		return GC.getEMBARKED_UNIT_MOVEMENT() + getExtraNavalMoves() + thisTeam.getEmbarkedExtraMoves() + thisTeam.getExtraMoves(eDomain) + pTraits->GetExtraEmbarkMoves() + pPolicies->GetNumericModifier(POLICYMOD_EMBARKED_EXTRA_MOVES);
	}
	int iMoves = m_pUnitInfo->GetMoves() + getExtraMoves() + thisTeam.getExtraMoves(eDomain);

	if(isEmbarked())
	{
		iMoves += getExtraNavalMoves();

		if(m_iBaseCombat == 0)
		{
			iMoves += pTraits->GetExtraEmbarkMoves();
		}
	}

	if(thisPlayer.isGoldenAge())
	{
		iMoves += pTraits->GetGoldenAgeMoveChange();
	}

	iMoves += pTraits->GetMovesChangeUnitCombat((UnitCombatTypes)(m_pUnitInfo->GetUnitCombatType()));



#ifdef NQ_ART_OF_WAR_PROMOTION
	if (plot() && eDomain == DOMAIN_LAND && !isEmbarked() && GetGreatGeneralOnOrAdjacentConfersMovement() > 0)
	{
		bool getsBonusMovementFromGeneral = false;
		int pX = plot()->getX();
		int pY = plot()->getY();
		CvPlot* pLoopPlot;
		CvUnit* pLoopUnit;
		IDInfo* pUnitNode;

		for (int iI = NO_DIRECTION; iI < NUM_DIRECTION_TYPES; iI++)
		{
			pLoopPlot = plotDirection(pX, pY, (DirectionTypes)iI);
			if (pLoopPlot != NULL && pLoopPlot->getNumUnits() > 0)
			{
				pUnitNode = pLoopPlot->headUnitNode();
				while (pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(*pUnitNode);
					if (pLoopUnit && pLoopUnit->getOwner() == getOwner() && pLoopUnit->IsGreatGeneral())
					{
						getsBonusMovementFromGeneral = true;
						break;
					}
					pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);
				}
			}
		}
		if (getsBonusMovementFromGeneral)
		{
			iMoves += GetGreatGeneralOnOrAdjacentConfersMovement();
		}
	}
#endif

#ifdef TRAITIFY
	if (plot() && plot()->IsFriendlyTerritory(getOwner()) && GetBaseCombatStrength() == 0)
	{
		iMoves += thisPlayer.GetPlayerTraits()->GetFriendlyLandsCitizenMoveChange();
	}
#endif
#if defined(v35_TRAITIFY)
	if (!killedUnitLastTurn() && (madeAttackLastTurn()))
	{
		iMoves += GetAttackExtraMoves();
	}
#endif
#ifdef LEKMOD_POLICIES_GLOBAL_MOVE_CHANGE

	iMoves += thisPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_GLOBAL_MOVE_CHANGE);

	if (plot() && plot()->IsFriendlyTerritory(getOwner()))
	{
		iMoves += thisPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_GLOBAL_MOVE_CHANGE_FRIENDLY);
	}

	if (plot() && !plot()->IsFriendlyTerritory(getOwner()))
	{
		CvPlot* pPlot = plot();
		if(isEnemy(pPlot->getTeam(), pPlot))
		{
			iMoves += thisPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_GLOBAL_MOVE_CHANGE_ENEMY);
		}
	}
#endif
#if defined(LEKMOD_SUBMERGE_MISSION)
	int iMoves = (m_pUnitInfo->GetMoves() + getExtraMoves() + thisTeam.getExtraMoves(eDomain) + m_iExtraNavalMoves + iExtraGoldenAgeMoves + iExtraUnitCombatTypeMoves + iExtraGlobalMoveChange + iExtraGlobalMoveChangeFriendlyCivilian);

	int iSubmergedMoves = iMoves;
	int eUnitClass = getUnitClassType();
	if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_SUBMARINE"))
	{
		iSubmergedMoves = 5;
	}
	else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_NUCLEAR_SUBMARINE"))
	{
		iSubmergedMoves = 6;
	}
	
	return IsSubmerged() ? iSubmergedMoves : iMoves;
#else
	return iMoves;
#endif
}



int CvUnit::maxMoves() const
{
	VALIDATE_OBJECT
	if(IsGreatGeneral())
	{
		return GetGreatGeneralStackMovement();
	}
#ifdef LEKMOD_LONGSHIP_ALL_PROMO






#else

	else if (isEmbarked())
	{
		return GetEmbarkedUnitStackMovement();
	}
#endif

	else
	{
		return (baseMoves() * GC.getMOVE_DENOMINATOR());
	}
}



int CvUnit::movesLeft() const
{
	VALIDATE_OBJECT
	return std::max(0, getMoves());
}



bool CvUnit::canMove() const
{
	VALIDATE_OBJECT
	return (getMoves() > 0);
}



bool CvUnit::hasMoved()	const
{
	VALIDATE_OBJECT
	return (getMoves() < maxMoves());
}



int CvUnit::GetRange() const
{
	VALIDATE_OBJECT
	return (m_pUnitInfo->GetRange() + m_iExtraRange);
}



int CvUnit::GetNukeDamageLevel() const
{
	VALIDATE_OBJECT
	return getUnitInfo().GetNukeDamageLevel();
}



bool CvUnit::canBuildRoute() const
{
	VALIDATE_OBJECT

	if(m_bEmbarked)
	{
		return false;
	}

	CvTeam& thisTeam = GET_TEAM(getTeam());

	CvTeamTechs* pTeamTechs = thisTeam.GetTeamTechs();

#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumBuildInfos(); iI++)
#else
	int iNumBuildInfos = GC.getNumBuildInfos();
	for(int iI = 0; iI < iNumBuildInfos; iI++)
#endif
	{
		CvBuildInfo* thisBuildInfo = GC.getBuildInfo((BuildTypes)iI);
		if(NULL != thisBuildInfo && thisBuildInfo->getRoute() != NO_ROUTE)
		{
#if defined(v35_TRAITIFY)
			if (m_pUnitInfo->GetBuilds(iI) || GET_PLAYER(getOwner()).GetPlayerTraits()->IsBuildableByUnitCombat((BuildTypes)iI, getUnitCombatType()))
#else
			if (m_pUnitInfo->GetBuilds(iI))
#endif
			{
				if(pTeamTechs->HasTech((TechTypes)(thisBuildInfo->getTechPrereq())))
				{
					return true;
				}
			}
		}
	}

	return false;
}


BuildTypes CvUnit::getBuildType() const
{
	VALIDATE_OBJECT
	const MissionQueueNode* pkMissionNode = HeadMissionQueueNode();
	if(pkMissionNode != NULL)
	{
		if(pkMissionNode->eMissionType == CvTypes::getMISSION_ROUTE_TO())
		{
			RouteTypes eBestRoute = GET_PLAYER(m_eOwner).getBestRoute(plot());
			if(eBestRoute != NO_ROUTE)
			{
#ifdef AUI_WARNING_FIXES
				for (uint iI = 0; iI < GC.getNumBuildInfos(); iI++)
#else
				for(int iI = 0; iI < GC.getNumBuildInfos(); iI++)
#endif
				{
					BuildTypes eBuild = (BuildTypes)iI;
					CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);

					if(pkBuildInfo)
					{
						RouteTypes eRoute = ((RouteTypes)(pkBuildInfo->getRoute()));
						if(eRoute == eBestRoute)
						{
							return eBuild;
						}
					}
				}
			}
		}
		else if(pkMissionNode->eMissionType == CvTypes::getMISSION_BUILD())
		{
			return (BuildTypes)pkMissionNode->iData1;
		}
	}

	return NO_BUILD;
}



int CvUnit::workRate(bool bMax, BuildTypes           ) const
{
	VALIDATE_OBJECT
	int iRate;

	if(!bMax)
	{
		if(!canMove())
		{
			return 0;
		}
	}

	iRate = m_pUnitInfo->GetWorkRate();

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
#if defined(v35_TRAITIFY)
	iRate += kPlayer.GetPlayerTraits()->GetUnitCombatWorkRateChange(static_cast<UnitCombatTypes>(m_pUnitInfo->GetUnitCombatType()));
#endif
	iRate *= std::max(0, (kPlayer.getWorkerSpeedModifier() + kPlayer.GetPlayerTraits()->GetWorkerSpeedModifier() + 100));
	iRate /= 100;

	if(!kPlayer.isHuman() && !kPlayer.IsAITeammateOfHuman() && !kPlayer.isBarbarian())
	{
		iRate *= std::max(0, (GC.getGame().getHandicapInfo().getAIWorkRateModifier() + 100));
		iRate /= 100;
	}

	return iRate;
}


bool CvUnit::isNoBadGoodies() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->IsNoBadGoodies();
}


int CvUnit::getRivalTerritoryCount() const
{
	VALIDATE_OBJECT
	return m_iRivalTerritoryCount;
}


bool CvUnit::isRivalTerritory() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->IsRivalTerritory() || getRivalTerritoryCount() > 0;
}


void CvUnit::changeRivalTerritoryCount(int iChange)
{
	VALIDATE_OBJECT
	m_iRivalTerritoryCount = (m_iRivalTerritoryCount + iChange);
	CvAssert(getRivalTerritoryCount() >= 0);
}


bool CvUnit::isFound() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->IsFound();
}


bool CvUnit::IsFoundAbroad() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->IsFoundAbroad();
}


bool CvUnit::IsWork() const
{
	VALIDATE_OBJECT
#if defined(v35_TRAITIFY)
	return workRate(true) > 0;
#else
	return (m_pUnitInfo->GetWorkRate() > 0);
#endif
}


bool CvUnit::isGoldenAge() const
{
	VALIDATE_OBJECT
	if(isDelayedDeath())
	{
		return false;
	}

	return m_pUnitInfo->GetGoldenAgeTurns() > 0;
}

#ifdef NQ_COMBAT_STRENGTH_NEAR_FRIENDLY_MINOR

bool CvUnit::IsNearFriendlyMinor(PlayerTypes* eMinor) const
{
	VALIDATE_OBJECT
	if(isDelayedDeath())
	{
		return false;
	}

	bool bIsNearFriendlyMinor = false;
	for (int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
	{
		CvPlayerAI& kMinor = GET_PLAYER((PlayerTypes) iMinorCivLoop);
		if (!kMinor.isEverAlive() || !kMinor.isMinorCiv())
		{
			continue;
		}
		
		CvMinorCivAI* pMinorAI = kMinor.GetMinorCivAI();
		if (pMinorAI)
		{

			if (pMinorAI->IsBoughtOut())
			{
				continue;
			}


			if (!kMinor.isAlive() && !kMinor.IsEverConqueredBy(getOwner()))
			{
				CvCity* pCity = GC.getMap().findCity(kMinor.GetOriginalCapitalX(), kMinor.GetOriginalCapitalY());
				
				if (pCity)
				{

					if (pCity->getOwner() != getOwner())
					{

						if (plotDistance(getX(), getY(), kMinor.GetOriginalCapitalX(), kMinor.GetOriginalCapitalY()) <= 3)
						{
							bIsNearFriendlyMinor = true;
							if (eMinor != NULL)
							{
								*eMinor = kMinor.GetID();
							}
							break;
						}
					}
				}
			}
			

			if (kMinor.isAlive() && kMinor.GetMinorCivAI()->GetEffectiveFriendshipWithMajor(getOwner()) >= GC.getFRIENDSHIP_THRESHOLD_FRIENDS())
			{

				int iLoop;
				CvCity *pLoopCity;
				for(pLoopCity = kMinor.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kMinor.nextCity(&iLoop))
				{
					if (pLoopCity)
					{
						if (plotDistance(getX(), getY(), pLoopCity->getX(), pLoopCity->getY()) <= 3)
						{
							bIsNearFriendlyMinor = true;
							if (eMinor != NULL)
							{
								*eMinor = kMinor.GetID();
							}
							break;
						}
					}
				}
			}
		}
	}

	return bIsNearFriendlyMinor;
}
#endif



bool CvUnit::isGivesPolicies() const
{
	VALIDATE_OBJECT
		if(isDelayedDeath())
		{
			return false;
		}

		return m_pUnitInfo->GetFreePolicies() > 0;
}


bool CvUnit::isBlastTourism() const
{
	VALIDATE_OBJECT
		if(isDelayedDeath())
		{
			return false;
		}

		return m_pUnitInfo->GetOneShotTourism() > 0;
}


bool CvUnit::canCoexistWithEnemyUnit(TeamTypes eTeam) const
{
	VALIDATE_OBJECT
	if(NO_TEAM == eTeam)
	{
		return false;
	}

	if(isInvisible(eTeam, false))
	{
		return true;
	}

	return false;
}



int CvUnit::getMustSetUpToRangedAttackCount() const
{
	VALIDATE_OBJECT
	return m_iMustSetUpToRangedAttackCount;
}


bool CvUnit::isMustSetUpToRangedAttack() const
{
	VALIDATE_OBJECT
	return getMustSetUpToRangedAttackCount() > 0;
}


void CvUnit::changeMustSetUpToRangedAttackCount(int iChange)
{
	VALIDATE_OBJECT
	m_iMustSetUpToRangedAttackCount = (m_iMustSetUpToRangedAttackCount + iChange);
	CvAssert(getMustSetUpToRangedAttackCount() >= 0);
}



int CvUnit::getRangedSupportFireCount() const
{
	VALIDATE_OBJECT
	return m_iRangedSupportFireCount;
}


bool CvUnit::isRangedSupportFire() const
{
	VALIDATE_OBJECT
	return getRangedSupportFireCount() > 0;
}


void CvUnit::changeRangedSupportFireCount(int iChange)
{
	VALIDATE_OBJECT
	m_iRangedSupportFireCount = (m_iRangedSupportFireCount + iChange);
	CvAssert(getRangedSupportFireCount() >= 0);
}



bool CvUnit::isFighting() const
{
	VALIDATE_OBJECT
	return (getCombatUnit() != NULL || getCombatCity() != NULL);
}



bool CvUnit::isAttacking() const
{
	VALIDATE_OBJECT
	return (getAttackPlot() != NULL && !isDelayedDeath());
}



bool CvUnit::isDefending() const
{
	VALIDATE_OBJECT
	return (isFighting() && !isAttacking());
}



bool CvUnit::isInCombat() const
{
	VALIDATE_OBJECT
	return (isFighting() || isAttacking());
}



int CvUnit::GetMaxHitPoints() const
{
	VALIDATE_OBJECT
	return GC.getMAX_HIT_POINTS();
}



int CvUnit::GetCurrHitPoints()	const
{
	VALIDATE_OBJECT
	return (GetMaxHitPoints() - getDamage());
}



bool CvUnit::IsHurt() const
{
	VALIDATE_OBJECT
	return (getDamage() > 0);
}



bool CvUnit::IsDead() const
{
	VALIDATE_OBJECT
	return (getDamage() >= GetMaxHitPoints());
}



int CvUnit::GetStrategicResourceCombatPenalty() const
{
	int iPenalty = 0;


	if(isBarbarian())
		return iPenalty;

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());


	ResourceTypes eResource;
	int iNumResourceInfos = GC.getNumResourceInfos();
	for(int iResourceLoop = 0; iResourceLoop < iNumResourceInfos; iResourceLoop++)
	{
		eResource = (ResourceTypes) iResourceLoop;

		int iAvailable = kPlayer.getNumResourceAvailable(eResource);
		if (kPlayer.isMinorCiv())
		{
			iAvailable += kPlayer.getResourceExport(eResource);
		}


		if(iAvailable < 0)
		{
			if(m_pUnitInfo->GetResourceQuantityRequirement(eResource) > 0)
			{
				int iUsed = kPlayer.getNumResourceUsed(eResource);
				int iMissing = iUsed - kPlayer.getNumResourceTotal(eResource);

				CvAssertMsg(iUsed > 0, "Number of used resources is zero or negative, this is unexpected. Please send Anton your save file and version.");
				if (iUsed <= 0)
					continue;
				CvAssertMsg(iMissing > 0, "Number of missing resources is zero or negative, this is unexpected. Please send Anton your save file and version.");
				if (iMissing <= 0)
					continue;

				double dDeficit = (double) iMissing / (double) iUsed;
				iPenalty += (int) floor((dDeficit) * GC.getSTRATEGIC_RESOURCE_EXHAUSTED_PENALTY());
			}
		}
	}

	iPenalty = max(iPenalty, GC.getSTRATEGIC_RESOURCE_EXHAUSTED_PENALTY());
	return iPenalty;
}



int CvUnit::GetUnhappinessCombatPenalty() const
{
	CvPlayer &kPlayer = GET_PLAYER(getOwner());
	int iPenalty = 0;

	if (kPlayer.IsEmpireUnhappy())
	{
		iPenalty = (-1 * kPlayer.GetExcessHappiness()) * GC.getVERY_UNHAPPY_COMBAT_PENALTY_PER_UNHAPPY();
		iPenalty = max(iPenalty, GC.getVERY_UNHAPPY_MAX_COMBAT_PENALTY());
	}

	return iPenalty;
}


void CvUnit::SetBaseCombatStrength(int iCombat)
{
	VALIDATE_OBJECT
	m_iBaseCombat = iCombat;
	m_iCachedPower = getUnitInfo().DoUpdatePower(m_iBaseCombat, m_iBaseRangedCombat);
}


int CvUnit::GetBaseCombatStrength(bool bIgnoreEmbarked) const
{
	VALIDATE_OBJECT
	if(m_bEmbarked && !bIgnoreEmbarked)
	{
		return GetEmbarkedUnitDefense() / 100;
	}

	return m_iBaseCombat;
}
void CvUnit::ChangeBaseCombatStrength(int iChange)
{
	VALIDATE_OBJECT
	SetBaseCombatStrength(GetBaseCombatStrength(true) + iChange);
}

int CvUnit::GetBaseCombatStrengthConsideringDamage() const
{
	int iWoundedDamageMultiplier =        GC.getWOUNDED_DAMAGE_MULTIPLIER();


	iWoundedDamageMultiplier += GET_PLAYER(getOwner()).GetWoundedUnitDamageMod();
#if defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
	CvCombatInfo kCombatInfo;
	kCombatInfo.setUnit(BATTLE_UNIT_ATTACKER, const_cast<CvUnit*>(this));
	int iStrength = GetMaxAttackStrength(kCombatInfo) / 100;
#else
	int iStrength = GetMaxAttackStrength(NULL,NULL,NULL) / 100;
#endif

	int iDamageMod = m_iDamage * iWoundedDamageMultiplier / 100;


	iStrength -= (iStrength * iDamageMod / GC.getMAX_HIT_POINTS());

	return iStrength;
}
#if !defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)


int CvUnit::GetGenericMaxStrengthModifier(const CvUnit* pOtherUnit, const CvPlot* pBattlePlot, bool bIgnoreUnitAdjacency) const
{
	VALIDATE_OBJECT

	int iModifier = 0;
	int iTempModifier;


	iTempModifier = getExtraCombatPercent();
	iModifier += iTempModifier;

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	ReligionTypes eFoundedReligion = pReligions->GetFounderBenefitsReligion(kPlayer.GetID());


	if(kPlayer.IsEmpireUnhappy())
		iModifier += GetUnhappinessCombatPenalty();


	iTempModifier = GetStrategicResourceCombatPenalty();
	if(iTempModifier != 0)
		iModifier += iTempModifier;


	if(IsNearGreatGeneral() && !IsIgnoreGreatGeneralBenefit())
	{
		iModifier += kPlayer.GetGreatGeneralCombatBonus();

		if(IsStackedGreatGeneral())
			iModifier += GetGreatGeneralCombatModifier();
	}


	int iReverseGGModifier = GetReverseGreatGeneralModifier();
	if(iReverseGGModifier != 0)
		iModifier += iReverseGGModifier;


	int iNearbyImprovementModifier = GetNearbyImprovementModifier();
	if(iNearbyImprovementModifier != 0)
		iModifier += iNearbyImprovementModifier;


	if(IsFriendlyUnitAdjacent(                true))
		iModifier += GetAdjacentModifier();


	if(kPlayer.isGoldenAge())
		iModifier += kPlayer.GetPlayerTraits()->GetGoldenAgeCombatModifier();





	if(pBattlePlot != NULL)
	{

		if(pBattlePlot->IsFriendlyTerritory(getOwner()))
		{
			iTempModifier = getFriendlyLandsModifier();
			iModifier += iTempModifier;


			CvCity* pPlotCity = pBattlePlot->getWorkingCity();
			if(pPlotCity)
			{
				ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
				if(eReligion != NO_RELIGION && eReligion == eFoundedReligion)
				{
					const CvReligion* pCityReligion = pReligions->GetReligion(eReligion, pPlotCity->getOwner());
					if(pCityReligion)
					{
						iTempModifier = pCityReligion->m_Beliefs.GetCombatModifierFriendlyCities();
						iModifier += iTempModifier;
					}
				}
			}
		}


		else
		{
			iTempModifier = getOutsideFriendlyLandsModifier();
			iModifier += iTempModifier;


			if(pBattlePlot->isCity() && GET_PLAYER(pBattlePlot->getOwner()).isMinorCiv())
			{
				iModifier += kPlayer.GetPlayerTraits()->GetCityStateCombatModifier();
			}


			CvCity* pPlotCity = pBattlePlot->getWorkingCity();
			if(pPlotCity)
			{
				if(atWar(getTeam(), pPlotCity->getTeam()))
				{
					ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
					if(eReligion != NO_RELIGION && eReligion == eFoundedReligion)
					{
						const CvReligion* pCityReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, pPlotCity->getOwner());
						if(pCityReligion)
						{
							iTempModifier = pCityReligion->m_Beliefs.GetCombatModifierEnemyCities();
							iModifier += iTempModifier;
						}
					}
				}
			}
		}


		iTempModifier = GetCapitalDefenseModifier();
		if(iTempModifier > 0)
		{
			CvCity* pCapital = GET_PLAYER(getOwner()).getCapitalCity();
			if(pCapital)
			{
				int iDistanceToCapital = plotDistance(pBattlePlot->getX(), pBattlePlot->getY(), pCapital->getX(), pCapital->getY());
				iTempModifier += (iDistanceToCapital * GetCapitalDefenseFalloff());
				if (iTempModifier > 0)
				{
					iModifier += iTempModifier;
				}
			}
		}

#ifdef NQ_COMBAT_STRENGTH_NEAR_FRIENDLY_MINOR

		iTempModifier = getCombatStrengthNearFriendlyMinor();
		if (iTempModifier > 0 && IsNearFriendlyMinor())
		{
			iModifier += iTempModifier;
		}
#endif


		iTempModifier = GET_PLAYER(getOwner()).GetPlayerTraits()->GetCombatBonusVsHigherTech();
		if(iTempModifier > 0)
		{

			if(pBattlePlot->getOwner() == getOwner())
			{

				UnitTypes eMyUnitType = getUnitType();
				if(pOtherUnit && pOtherUnit->IsHigherTechThan(eMyUnitType))
				{
					iModifier += iTempModifier;
				}
			}
		}
	}





	if(pOtherUnit != NULL)
	{
		CvAssertMsg(pOtherUnit != this, "Compared combat strength against one's own pointer. This is weird and probably wrong.");

		if(!bIgnoreUnitAdjacency)
		{

			int iNumAdjacentFriends = pOtherUnit->GetNumEnemyUnitsAdjacent(this);
			if(iNumAdjacentFriends > 0)
			{
				iTempModifier =        GC.getBONUS_PER_ADJACENT_FRIEND() * iNumAdjacentFriends;

				int iFlankModifier = GetFlankAttackModifier();
				if(iFlankModifier > 0)
				{
					iTempModifier = iTempModifier * (100 + iFlankModifier) / 100;
				}

				iModifier += iTempModifier;
			}
		}


		iTempModifier = getUnitClassModifier(pOtherUnit->getUnitClassType());
		iModifier += iTempModifier;


		if(pOtherUnit->getUnitCombatType() != NO_UNITCOMBAT)
		{
			iTempModifier = unitCombatModifier(pOtherUnit->getUnitCombatType());
			iModifier += iTempModifier;
		}


		iTempModifier = domainModifier(pOtherUnit->getDomainType());
		iModifier += iTempModifier;


		if(GET_PLAYER(pOtherUnit->getOwner()).isMinorCiv())
		{
			iModifier += kPlayer.GetPlayerTraits()->GetCityStateCombatModifier();
		}


		if(pOtherUnit->isBarbarian())
		{

			iTempModifier = kPlayer.GetBarbarianCombatBonus();
			iModifier += iTempModifier;

			CvHandicapInfo& thisGameHandicap = GC.getGame().getHandicapInfo();


			if(isHuman())
			{
				iTempModifier = thisGameHandicap.getBarbarianCombatModifier();
				iModifier += iTempModifier;
			}

			else
			{
				iTempModifier = thisGameHandicap.getAIBarbarianCombatModifier();
				iModifier += iTempModifier;
			}

			if(GC.getGame().isOption(GAMEOPTION_RAGING_BARBARIANS))
			{
				iModifier += 25;
			}
		}
#ifndef AUI_UNIT_FIX_BAD_BONUS_STACKS

		iTempModifier = GET_PLAYER(getOwner()).GetPlayerTraits()->GetCombatBonusVsLargerCiv();
		if (iTempModifier > 0)
		{
			if (pOtherUnit && pOtherUnit->IsLargerCivThan(this))
			{
				iModifier += iTempModifier;
			}
		}
#ifdef NQ_COMBAT_BONUS_VS_SMALLER_CIV_FROM_POLICIES

		iTempModifier = GET_PLAYER(getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_COMBAT_BONUS_VS_SMALLER_CIV);
		if (iTempModifier > 0)
		{
			if (pOtherUnit && this->IsLargerCivThan(pOtherUnit))
			{
				iModifier += iTempModifier;
			}
		}
#endif
#endif
	}

	return iModifier;
}


int CvUnit::GetMaxAttackStrength(const CvPlot* pFromPlot, const CvPlot* pToPlot, const CvUnit* pDefender) const
{
	VALIDATE_OBJECT

	bool bIsEmbarkedAttackingLand = isEmbarked() && (pToPlot && !pToPlot->isWater());
#ifdef NQ_HEAVY_CHARGE_DOWNHILL
	bool isAttackingFromHigherElevation = false;
	if (pFromPlot && pToPlot)
	{
		if ((pFromPlot->isMountain() && !pToPlot->isMountain()) || (pFromPlot->isHills() && pToPlot->isFlatlands()))
		{
			isAttackingFromHigherElevation = true;
		}
	}
#endif

	if (isEmbarked() && !bIsEmbarkedAttackingLand)
		return GetEmbarkedUnitDefense();

	if (GetBaseCombatStrength(bIsEmbarkedAttackingLand) == 0)
		return 0;

	int iCombat;

	int iTempModifier;
	int iModifier = GetGenericMaxStrengthModifier(pDefender, pToPlot,                     false);


	iTempModifier = getAttackModifier();
	iModifier += iTempModifier;


	if (getKamikazePercent() != 0)
	{
		iTempModifier = getKamikazePercent();
		iModifier += iTempModifier;
	}


	if (GET_PLAYER(getOwner()).GetAttackBonusTurns() > 0)
	{
		iTempModifier =        GC.getPOLICY_ATTACK_BONUS_MOD();
		iModifier += iTempModifier;
	}





	if (pToPlot != NULL)
	{
#ifdef NQ_HEAVY_CHARGE_DOWNHILL

		if (GetHeavyChargeDownhill() > 0 && isAttackingFromHigherElevation)
		{
			iTempModifier = GetHeavyChargeDownhill();
			iModifier += iTempModifier;
		}
#endif

#ifdef NQ_GOLDEN_AGE_FOREIGN_ATTACK_BONUS
		if (pToPlot->getOwner() != getOwner())
		{
			if (GET_PLAYER(getOwner()).isGoldenAge())
			{
				iTempModifier = getGoldenAgeForeignAttackBonus();
				iModifier += iTempModifier;
			}
		}
#endif


		if (pToPlot->isCity())
		{
			iTempModifier = cityAttackModifier();
			iModifier += iTempModifier;


			if (IsNearSapper(pToPlot->getPlotCity()))
			{
				iTempModifier = GC.getSAPPED_CITY_ATTACK_MODIFIER();
				iModifier += iTempModifier;
			}
#if defined(TRAITIFY) || defined(LEKMOD_LEGACY)
			PromotionTypes eGreatGeneralPromotion = NO_PROMOTION;
			for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
			{
				const PromotionTypes eLoopPromotion = static_cast<PromotionTypes>(iI);
				CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eLoopPromotion);
				if (pkPromotionInfo)
				{
					if (pkPromotionInfo->IsGreatGeneral())
					{
						eGreatGeneralPromotion = eLoopPromotion;
						break;
					}
				}
			}
			if (IsNearUnitWithPromotion(eGreatGeneralPromotion, GC.getGREAT_GENERAL_RANGE(), false                , true                ))
			{
				iTempModifier = GET_PLAYER(getOwner()).GetPlayerTraits()->GetGreatGeneralSiegeBonus();
				iModifier += iTempModifier;
				iTempModifier = GET_PLAYER(getOwner()).GetPlayerLegacies()->GetGreatGeneralSiegeBonus();
				iModifier += iTempModifier;
			}
#endif
#ifdef LEKMOD_PROMOTION_ADJACENT_CITY_ATTACK

			if (IsNearCityAttackBonus())
			{
				iTempModifier = getNearUnitCityAttackModifier();
				iModifier += iTempModifier;
			}
#endif


			if (isBarbarian())
			{
				iTempModifier = GC.getBARBARIAN_CITY_ATTACK_MODIFIER();
				iModifier += iTempModifier;
			}
		}

		else
		{

			if (pToPlot->isHills())
			{
				iTempModifier = hillsAttackModifier();
				iModifier += iTempModifier;
			}


			if (pToPlot->isOpenGround())
			{
				iTempModifier = openAttackModifier();
				iModifier += iTempModifier;
			}


			if (pToPlot->isRoughGround())
			{
				iTempModifier = roughAttackModifier();
				iModifier += iTempModifier;
			}


			if (pToPlot->getFeatureType() != NO_FEATURE)
			{
				iTempModifier = featureAttackModifier(pToPlot->getFeatureType());
				iModifier += iTempModifier;
			}

			else
			{
				iTempModifier = terrainAttackModifier(pToPlot->getTerrainType());
				iModifier += iTempModifier;


				if (pToPlot->isHills())
				{
					iTempModifier = terrainAttackModifier(TERRAIN_HILL);
					iModifier += iTempModifier;
				}
			}
		}

#ifndef AUI_UNIT_FIX_BAD_BONUS_STACKS

		if (pToPlot->IsFriendlyTerritory(getOwner()))
		{
			iTempModifier = getFriendlyLandsAttackModifier();
			iModifier += iTempModifier;
		}
#endif





		if (pFromPlot != NULL)
		{

			if (!isRiverCrossingNoPenalty())
			{
				if (pFromPlot->isRiverCrossing(directionXY(pFromPlot, pToPlot)))
				{
					iTempModifier = GC.getRIVER_ATTACK_MODIFIER();
					iModifier += iTempModifier;
				}
			}


			if (!isAmphib())
			{
				if (!(pToPlot->isWater()) && pFromPlot->isWater() && getDomainType() == DOMAIN_LAND)
				{
					iTempModifier = GC.getAMPHIB_ATTACK_MODIFIER();
					iModifier += iTempModifier;
				}
			}
		}
	}





	if (pDefender != NULL)
	{
		CvAssertMsg(pDefender != this, "Compared attack strength against one's own pointer. This is weird and probably wrong.");


		iTempModifier = unitClassAttackModifier(pDefender->getUnitClassType());
		iModifier += iTempModifier;


		if (pDefender->getFortifyTurns() > 0)
			iModifier += attackFortifiedModifier();


		if (pDefender->getDamage() > 0)
			iModifier += attackWoundedModifier();
#ifdef LEKMOD_DIFFERENT_IDEO_COMBAT_BONUS

		iTempModifier = GetCombatBonusVsDifferentIdeologyModifier();
		if (iTempModifier != 0)
		{

			if (isUnitDifferentIdeology(pDefender))
			{
				iModifier += iTempModifier;
			}
		}
		
#endif
#ifdef LEKMOD_TOURISM_COMBAT_MOD
		iTempModifier = getTourismInfluenceCombatModifierVsUnit(pDefender);
		if (iTempModifier != 0)
		{
			iModifier += iTempModifier;
		}
#endif
	}


	if (iModifier < -90)
		iModifier = -90;

	iCombat = GetBaseCombatStrength(bIsEmbarkedAttackingLand) * (iModifier + 100);

#ifdef AUI_UNIT_FIX_HEAVY_CHARGE_BONUS_INTEGRATED_INTO_STACKS
#ifdef NQ_HEAVY_CHARGE_DOWNHILL
	if (pDefender && pToPlot && pFromPlot)
	{
		bool isAttackingFromHigherElevation = 
			(pFromPlot->isMountain() && !pToPlot->isMountain()) ||
			(pFromPlot->isHills() && pToPlot->isFlatlands());
		if ((IsCanHeavyCharge() || (GetHeavyChargeDownhill() > 0 && isAttackingFromHigherElevation)) && !pDefender->CanFallBackFromMelee(*this, pToPlot))
		{
			iCombat = (iCombat * 150) / 100;
		}
	}
#else
	if (IsCanHeavyCharge() && pDefender && pToPlot && !pDefender->CanFallBackFromMelee(*this, pToPlot))
	{
		iCombat = (iCombat * 150) / 100;
	}
#endif
#endif

	return std::max(1, iCombat);
}



int CvUnit::GetMaxDefenseStrength(const CvPlot* pInPlot, const CvUnit* pAttacker, bool bFromRangedAttack) const
{
	VALIDATE_OBJECT

		if (m_bEmbarked)
		{
			return GetEmbarkedUnitDefense();;
		}

	if (GetBaseCombatStrength() == 0)
		return 0;

	int iCombat;

	int iTempModifier;
	int iModifier = GetGenericMaxStrengthModifier(pAttacker, pInPlot,                          bFromRangedAttack);


	iTempModifier = getDefenseModifier();
	iModifier += iTempModifier;


	if (bFromRangedAttack)
		iModifier += rangedDefenseModifier();





	if (pInPlot != NULL)
	{

		iTempModifier = pInPlot->defenseModifier(getTeam(), (pAttacker != NULL) ? pAttacker->ignoreBuildingDefense() : true);


		if (!noDefensiveBonus() || iTempModifier < 0)
			iModifier += iTempModifier;


		iTempModifier = fortifyModifier();
#if defined(LEKMOD_NO_FORTIFY_VS_RANGED_PROMO)

		bool bStripFortify = isNoFortifyVsRanged() &&
			(bFromRangedAttack || (pAttacker != NULL && pAttacker->getDomainType() == DOMAIN_AIR));
		if (!bStripFortify)
#endif
		{
			iModifier += iTempModifier;
		}


		if (pInPlot->isCity())
		{
			iTempModifier = cityDefenseModifier();
			iModifier += iTempModifier;
		}


		if (pInPlot->isHills())
		{
			iTempModifier = hillsDefenseModifier();
			iModifier += iTempModifier;
		}


		if (pInPlot->isOpenGround())
		{
			iTempModifier = openDefenseModifier();
			iModifier += iTempModifier;
		}


		if (pInPlot->isRoughGround())
		{
			iTempModifier = roughDefenseModifier();
			iModifier += iTempModifier;
		}


		if (pInPlot->getFeatureType() != NO_FEATURE)
		{
			iTempModifier = featureDefenseModifier(pInPlot->getFeatureType());
			iModifier += iTempModifier;
		}

		else
		{
			iTempModifier = terrainDefenseModifier(pInPlot->getTerrainType());
			iModifier += iTempModifier;


			if (pInPlot->isHills())
			{
				iTempModifier = terrainDefenseModifier(TERRAIN_HILL);
				iModifier += iTempModifier;
			}
		}
	}





	if (pAttacker != NULL)
	{
		CvAssertMsg(pAttacker != this, "Compared defense strength against one's own pointer. This is weird and probably wrong.");


		iTempModifier = unitClassDefenseModifier(pAttacker->getUnitClassType());
		iModifier += iTempModifier;

#ifdef LEKMOD_DIFFERENT_IDEO_COMBAT_BONUS

		iTempModifier = GetCombatBonusVsDifferentIdeologyModifier();
		if (iTempModifier != 0)
		{

			if (isUnitDifferentIdeology(pAttacker))
			{
				iModifier += iTempModifier;
			}
		}
#endif
#ifdef LEKMOD_TOURISM_COMBAT_MOD
		iTempModifier = getTourismInfluenceCombatModifierVsUnit(pAttacker);
		if (iTempModifier != 0)
		{
			iModifier += iTempModifier;
		}
#endif
	}


	if (iModifier < -90)
		iModifier = -90;

	iCombat = GetBaseCombatStrength() * (iModifier + 100);


	if (pAttacker != NULL)
	{
		if (pAttacker->getDomainType() == DOMAIN_SEA && getDomainType() == DOMAIN_SEA)
		{
			iCombat *=        GC.getNAVAL_COMBAT_DEFENDER_STRENGTH_MULTIPLIER();
			iCombat /= 100;
		}
	}

	return std::max(1, iCombat);
}

int CvUnit::GetMaxRangedCombatStrength(const CvUnit* pOtherUnit, const CvCity* pCity, bool bAttacking, bool bForRangedAttack) const
{
	VALIDATE_OBJECT
#if defined(AUI_UNIT_FIX_BAD_BONUS_STACKS)
	const CvPlot* pMyPlot = plot();
#endif
	int iModifier;
	int iCombat;

	int iTempModifier;

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
	CvPlayerTraits* pTraits = kPlayer.GetPlayerTraits();
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	ReligionTypes eFoundedReligion = pReligions->GetFounderBenefitsReligion(kPlayer.GetID());

	int iStr = isRangedSupportFire() ? GetBaseCombatStrength() / 2 : GetBaseRangedCombatStrength();

	if (iStr == 0)
	{
		return 0;
	}


	iModifier = getExtraCombatPercent();


	if (getKamikazePercent() != 0)
		iModifier += getKamikazePercent();


	if (kPlayer.IsEmpireUnhappy())
	{
		iModifier += GetUnhappinessCombatPenalty();
	}


	iTempModifier = GetStrategicResourceCombatPenalty();
	if (iTempModifier != 0)
		iModifier += iTempModifier;


	if (IsNearGreatGeneral() && !IsIgnoreGreatGeneralBenefit())
	{
		iModifier +=        GC.getGREAT_GENERAL_STRENGTH_MOD();
		iModifier += pTraits->GetGreatGeneralExtraBonus();

		if (IsStackedGreatGeneral())
		{
			iModifier += GetGreatGeneralCombatModifier();
		}
	}


	int iReverseGGModifier = GetReverseGreatGeneralModifier();
	if (iReverseGGModifier != 0)
	{
		iModifier += iReverseGGModifier;
	}


	int iNearbyImprovementModifier = GetNearbyImprovementModifier();
	if (iNearbyImprovementModifier != 0)
	{
		iModifier += iNearbyImprovementModifier;
	}


	if (kPlayer.isGoldenAge())
		iModifier += pTraits->GetGoldenAgeCombatModifier();





	if (NULL != pOtherUnit)
	{

		iModifier += getUnitClassModifier(pOtherUnit->getUnitClassType());


		if (pOtherUnit->getUnitCombatType() != NO_UNITCOMBAT)
			iModifier += unitCombatModifier(pOtherUnit->getUnitCombatType());


		iModifier += domainModifier(pOtherUnit->getDomainType());

#ifndef FIX_RANGE_DEFENSE_MOD

		if (pOtherUnit->getFortifyTurns() > 0)
			iModifier += attackFortifiedModifier();
#endif


		if (pOtherUnit->getDamage() > 0)
			iModifier += attackWoundedModifier();


		if (GET_PLAYER(pOtherUnit->getOwner()).isMinorCiv())
		{
			iModifier += pTraits->GetCityStateCombatModifier();
		}


		if (pOtherUnit->isBarbarian())
		{

			iTempModifier = kPlayer.GetBarbarianCombatBonus();
			iModifier += iTempModifier;

			CvHandicapInfo& thisGameHandicap = GC.getGame().getHandicapInfo();


			if (isHuman())
			{
				iTempModifier = thisGameHandicap.getBarbarianCombatModifier();
				iModifier += iTempModifier;
			}

			else
			{
				iTempModifier = thisGameHandicap.getAIBarbarianCombatModifier();
				iModifier += iTempModifier;
			}

			if (GC.getGame().isOption(GAMEOPTION_RAGING_BARBARIANS))
			{
				iModifier += 25;
			}
		}

#ifdef AUI_UNIT_FIX_BAD_BONUS_STACKS
		const CvPlot* pTargetPlot = pMyPlot;
#endif

		if (bForRangedAttack)
		{

			iModifier += unitClassAttackModifier(pOtherUnit->getUnitClassType());





#ifdef AUI_UNIT_FIX_BAD_BONUS_STACKS
			pTargetPlot = pOtherUnit->plot();
#else
			CvPlot* pTargetPlot = pOtherUnit->plot();
#endif


			if (pTargetPlot->isOpenGround())
				iModifier += openRangedAttackModifier();


			if (pTargetPlot->isRoughGround())
				iModifier += roughRangedAttackModifier();

#ifdef FIX_RANGE_DEFENSE_MOD

			if (pOtherUnit->getFortifyTurns() > 0)
				iModifier += attackFortifiedModifier();
#endif

#ifdef LEKMOD_DIFFERENT_IDEO_COMBAT_BONUS
			iTempModifier = GetCombatBonusVsDifferentIdeologyModifier();
			if (iTempModifier != 0)
			{

				if (isUnitDifferentIdeology(pOtherUnit))
				{
					iModifier += iTempModifier;
				}
			}
#endif
#ifdef LEKMOD_TOURISM_COMBAT_MOD
			iTempModifier = getTourismInfluenceCombatModifierVsUnit(pOtherUnit);
			if (iTempModifier != 0)
			{
				iModifier += iTempModifier;
			}
#endif
#ifdef AUI_UNIT_FIX_BAD_BONUS_STACKS
		}

		else
		{

			iModifier += unitClassDefenseModifier(pOtherUnit->getUnitClassType());
		}


		if (pMyPlot && pMyPlot->IsFriendlyTerritory(getOwner()))
		{
			iTempModifier = getFriendlyLandsModifier();
			iModifier += iTempModifier;


			CvCity* pPlotCity = pTargetPlot->getWorkingCity();
			if (pPlotCity)
			{
				ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
				if (eReligion != NO_RELIGION && eReligion == eFoundedReligion)
				{
					const CvReligion* pCityReligion = pReligions->GetReligion(eReligion, pPlotCity->getOwner());
					if (pCityReligion)
					{
						iTempModifier = pCityReligion->m_Beliefs.GetCombatModifierFriendlyCities();
						iModifier += iTempModifier;
					}
				}
			}
		}


		else
		{
			iTempModifier = getOutsideFriendlyLandsModifier();
			iModifier += iTempModifier;


			CvCity* pPlotCity = pTargetPlot->getWorkingCity();
			if (pPlotCity)
			{
				if (atWar(getTeam(), pPlotCity->getTeam()))
				{
					ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
					if (eReligion != NO_RELIGION && eReligion == eFoundedReligion)
					{
						const CvReligion* pCityReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, pPlotCity->getOwner());
						if (pCityReligion)
						{
							iTempModifier = pCityReligion->m_Beliefs.GetCombatModifierEnemyCities();
							iModifier += iTempModifier;
						}
					}
				}
			}
#else

			if (pTargetPlot->IsFriendlyTerritory(getOwner()))
			{
				iTempModifier = getFriendlyLandsModifier();
				iModifier += iTempModifier;


				CvCity* pPlotCity = pTargetPlot->getWorkingCity();
				if (pPlotCity)
				{
					ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
					if (eReligion != NO_RELIGION && eReligion == eFoundedReligion)
					{
						const CvReligion* pCityReligion = pReligions->GetReligion(eReligion, pPlotCity->getOwner());
						if (pCityReligion)
						{
							iTempModifier = pCityReligion->m_Beliefs.GetCombatModifierFriendlyCities();
							iModifier += iTempModifier;
						}
					}
				}
			}


			else
			{
				iTempModifier = getOutsideFriendlyLandsModifier();
				iModifier += iTempModifier;


				CvCity* pPlotCity = pTargetPlot->getWorkingCity();
				if (pPlotCity)
				{
					if (atWar(getTeam(), pPlotCity->getTeam()))
					{
						ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
						if (eReligion != NO_RELIGION && eReligion == eFoundedReligion)
						{
							const CvReligion* pCityReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, pPlotCity->getOwner());
							if (pCityReligion)
							{
								iTempModifier = pCityReligion->m_Beliefs.GetCombatModifierEnemyCities();
								iModifier += iTempModifier;
							}
						}
					}
				}
			}
		}


		else
		{
#ifndef FIX_RANGE_DEFENSE_MOD

			iModifier += rangedDefenseModifier();
#endif


			iModifier += unitClassDefenseModifier(pOtherUnit->getUnitClassType());
#endif

#ifdef LEKMOD_DIFFERENT_IDEO_COMBAT_BONUS
		iTempModifier = GetCombatBonusVsDifferentIdeologyModifier();
			if (iTempModifier != 0)
		{

				if (isUnitDifferentIdeology(pOtherUnit))
			{
				iModifier += iTempModifier;
			}
		
		}
#endif
#ifdef LEKMOD_TOURISM_COMBAT_MOD
		iTempModifier = getTourismInfluenceCombatModifierVsUnit(pOtherUnit);
			if (iTempModifier != 0)
		{
			iModifier += iTempModifier;
		}
#endif
		}
	}





	if (pCity != NULL)
	{

		iTempModifier = cityAttackModifier();
		iModifier += iTempModifier;


		if (IsNearSapper(pCity))
		{
			iTempModifier = GC.getSAPPED_CITY_ATTACK_MODIFIER();
			iModifier += iTempModifier;
		}


		if (GET_PLAYER(pCity->getOwner()).isMinorCiv())
		{
			iModifier += pTraits->GetCityStateCombatModifier();
		}
	}


	if (bForRangedAttack)
	{
		iModifier += GetRangedAttackModifier();
	}
#ifdef AUI_UNIT_FIX_BAD_BONUS_STACKS
	else
	{

		iModifier += rangedDefenseModifier();

		if (pMyPlot && pMyPlot->isHills())
			iModifier += hillsDefenseModifier();
	}
#endif


	if (bAttacking)
	{
		iModifier += getAttackModifier();
#ifdef AUI_UNIT_FIX_BAD_BONUS_STACKS

		if (pMyPlot && pMyPlot->isHills())
			iModifier += hillsAttackModifier();
#endif
	}

	else
	{
#ifdef AUI_UNIT_FIX_BAD_BONUS_STACKS

		if (pMyPlot)
		{
			if (pMyPlot->isHills())
				iModifier += hillsDefenseModifier();
			iTempModifier = pMyPlot->defenseModifier(getTeam(), false);
		}
#else

		iTempModifier = plot()->defenseModifier(getTeam(), false);
#endif


		if (!noDefensiveBonus() || iTempModifier < 0)
			iModifier += iTempModifier;

		iModifier += getDefenseModifier();

#ifdef FIX_RANGE_DEFENSE_MOD

		iModifier += rangedDefenseModifier();


		if (plot()->IsFriendlyTerritory(getOwner()))
		{
			iTempModifier = getFriendlyLandsModifier();
			iModifier += iTempModifier;


			CvCity* pPlotCity = plot()->getWorkingCity();
			if (pPlotCity)
			{
				ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
				if (eReligion != NO_RELIGION && eReligion == eFoundedReligion)
				{
					const CvReligion* pCityReligion = pReligions->GetReligion(eReligion, pPlotCity->getOwner());
					if (pCityReligion)
					{
						iTempModifier = pCityReligion->m_Beliefs.GetCombatModifierFriendlyCities();
						iModifier += iTempModifier;
					}
				}
			}
		}


		else
		{
			iTempModifier = getOutsideFriendlyLandsModifier();
			iModifier += iTempModifier;


			CvCity* pPlotCity = plot()->getWorkingCity();
			if (pPlotCity)
			{
				if (atWar(getTeam(), pPlotCity->getTeam()))
				{
					ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
					if (eReligion != NO_RELIGION && eReligion == eFoundedReligion)
					{
						const CvReligion* pCityReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, pPlotCity->getOwner());
						if (pCityReligion)
						{
							iTempModifier = pCityReligion->m_Beliefs.GetCombatModifierEnemyCities();
							iModifier += iTempModifier;
						}
					}
				}
			}
		}
#endif
	}


	if (iModifier < -90)
		iModifier = -90;

	iCombat = (iStr * (iModifier + 100));

	return std::max(1, iCombat);
}

#else


int CvUnit::GetGenericMaxStrengthModifier(const CvCombatInfo& kInfo, CvCombatModifierList* kModifierList) const
{
	VALIDATE_OBJECT
	int iModifier = 0;
	const bool bIsAttacker = kInfo.getUnit(BATTLE_UNIT_ATTACKER) == this;
	const bool bIsDefender = kInfo.getUnit(BATTLE_UNIT_DEFENDER) == this || kInfo.getUnit(BATTLE_UNIT_INTERCEPTOR) == this;
	CvAssertMsg(bIsAttacker != bIsDefender, "Unit must occupy exactly one combat side.");

	if (!bIsAttacker && !bIsDefender)
		return iModifier;

	const BattleUnitTypes eOtherSide = bIsAttacker ? BATTLE_UNIT_DEFENDER : BATTLE_UNIT_ATTACKER;
	if (kModifierList)
	{
		kModifierList->bAttackerSide = bIsAttacker;
	}
	CvUnit* pOtherUnit = kInfo.getUnit(eOtherSide);
	CvCity* pOtherCity = kInfo.getCity(eOtherSide);

	PlayerTypes eOtherPlayer = NO_PLAYER;
	if (pOtherUnit)
	{
		eOtherPlayer = pOtherUnit->getOwner();
	}
	else if (pOtherCity)
	{
		eOtherPlayer = pOtherCity->getOwner();
	}

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	ReligionTypes eFoundedReligion = pReligions->GetFounderBenefitsReligion(kPlayer.GetID());

	int iTempModifier;


	iTempModifier = getExtraCombatPercent();
	iModifier += iTempModifier;
	if (kModifierList && iTempModifier)
	{
		GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_GENERIC", iTempModifier);
	}
	

	if (kPlayer.IsEmpireUnhappy())
	{
		iTempModifier = GetUnhappinessCombatPenalty();
		iModifier += iTempModifier;
		if (kModifierList && iTempModifier)
		{
			GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_UNHAPPY", iTempModifier, kPlayer.GetExcessHappiness());
		}
	}


	iTempModifier = GetStrategicResourceCombatPenalty();
	if (iTempModifier != 0)
	{
		iModifier += iTempModifier;
		if (kModifierList && iTempModifier)
		{
			GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_STRATEGIC_RESOURCE_PENALTY", iTempModifier);
		}
	}


	if (IsNearGreatGeneral() && !IsIgnoreGreatGeneralBenefit())
	{
		iTempModifier = kPlayer.GetGreatGeneralCombatBonus();
		if (IsStackedGreatGeneral())
		{
			iTempModifier += GetGreatGeneralCombatModifier();
		}

		iModifier += iTempModifier;
		if (kModifierList && iTempModifier)
		{
			const char* szKey;
			if (getDomainType() == DOMAIN_SEA)
			{
				szKey = "TXT_KEY_COMBATMOD_NEAR_GREAT_ADMIRAL";
			}
			else
			{
				szKey = "TXT_KEY_COMBATMOD_NEAR_GREAT_GENERAL";
			}
			GC.getGame().BuildCombatModHelpText(*kModifierList, szKey, iTempModifier);
		}
	}


	iTempModifier = GetReverseGreatGeneralModifier();
	if (iTempModifier != 0)
	{
		iModifier += iTempModifier;
		if (kModifierList && iTempModifier)
		{

			GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_NEAR_REVERSE_GREAT_GENERAL", iTempModifier);
		}











	}



	ImprovementTypes eNearbyImprovement = NO_IMPROVEMENT;
	iTempModifier = GetNearbyImprovementModifier(kModifierList ? &eNearbyImprovement : NULL);
	iModifier += iTempModifier;
	if (kModifierList && iTempModifier)
	{
		GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_NEAR_IMPROVEMENT", iTempModifier, GC.getImprovementInfo(eNearbyImprovement)->GetDescription());
	}
	

	if (IsFriendlyUnitAdjacent(                true))
	{
		iTempModifier = GetAdjacentModifier();
		iModifier += iTempModifier;
		if (kModifierList && iTempModifier)
		{
			GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_ADJACENT_FRIENDLY_UNIT", iTempModifier);
		}
	}


	if (kPlayer.isGoldenAge())
	{
		iTempModifier = kPlayer.GetPlayerTraits()->GetGoldenAgeCombatModifier();
		iModifier += iTempModifier;
		if (kModifierList && iTempModifier)
		{
			GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_GOLDEN_AGE", iTempModifier);
		}
	}




	if (kInfo.getPlot() != NULL)
	{
		const CvPlot& plot = *kInfo.getPlot();

#if defined(v35_TRAITIFY)
		bool bIsNearWater = false;
		iTempModifier = GetNearbyWaterCombatModifier();
		if (iTempModifier > 0)
		{
			CvPlot* pLoopPlot;
			const char* szExtraText = "";
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				pLoopPlot = plotDirection(plot.getX(), plot.getY(), (DirectionTypes)iI);
				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->isLake() || pLoopPlot->isPseudoLake())
					{
						szExtraText = "TXT_KEY_PLOTROLL_LAKE";
						bIsNearWater = true;
						break;
					}
					else if (pLoopPlot->isRiver())
					{
						szExtraText = "TXT_KEY_PLOTROLL_RIVER";
						bIsNearWater = true;
						break;
					}
					else if (pLoopPlot->isWater())
					{
						szExtraText = "TXT_KEY_TERRAIN_OCEAN";
						bIsNearWater = true;
						break;
					}
				}
			}

			if (bIsNearWater)
			{
				iModifier += iTempModifier;
				if (kModifierList && iTempModifier)
				{
					GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_NEAR_WATER", iTempModifier, szExtraText);
				}
			}
		}
#endif

		if (plot.IsFriendlyTerritory(getOwner()))
		{
			iTempModifier = getFriendlyLandsModifier();

			CvCity* pPlotCity = plot.getWorkingCity();
			if (pPlotCity)
			{
				ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
				if (eReligion != NO_RELIGION && eReligion == eFoundedReligion)
				{
					const CvReligion* pCityReligion = pReligions->GetReligion(eReligion, pPlotCity->getOwner());
					if (pCityReligion)
					{
						iTempModifier += pCityReligion->m_Beliefs.GetCombatModifierFriendlyCities();
					}
				}
			}

			iModifier += iTempModifier;
			if (kModifierList && iTempModifier)
			{
				GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_FRIENDLY_LANDS", iTempModifier);
			}
		}
		else
		{
			iTempModifier = getOutsideFriendlyLandsModifier();

			CvCity* pPlotCity = plot.getWorkingCity();
			if (pPlotCity)
			{
				if (atWar(getTeam(), pPlotCity->getTeam()))
				{
					ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
					if (eReligion != NO_RELIGION && eReligion == eFoundedReligion)
					{
						const CvReligion* pCityReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, pPlotCity->getOwner());
						if (pCityReligion)
						{
							iTempModifier += pCityReligion->m_Beliefs.GetCombatModifierEnemyCities();
						}
					}
				}
			}

			iModifier += iTempModifier;
			if (kModifierList && iTempModifier)
			{
				GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_OUTSIDE_FRIENDLY_LANDS", iTempModifier);
			}
		}


		CvCity* pCapital = GET_PLAYER(getOwner()).getCapitalCity();
		if (pCapital)
		{
			iTempModifier = GetCapitalDefenseModifier();
			int iDistanceToCapital = plotDistance(plot.getX(), plot.getY(), pCapital->getX(), pCapital->getY());
			iTempModifier += (iDistanceToCapital * GetCapitalDefenseFalloff());
			if (iTempModifier > 0)
			{
				iModifier += iTempModifier;

				if (kModifierList && iTempModifier)
				{
					GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_CAPITAL_DEFENSE", iTempModifier, iDistanceToCapital);
				}
			}
		}


#ifdef NQ_COMBAT_STRENGTH_NEAR_FRIENDLY_MINOR

		PlayerTypes eMinor = NO_PLAYER;
		if (IsNearFriendlyMinor(&eMinor))
		{
			iTempModifier = getCombatStrengthNearFriendlyMinor();
			iModifier += iTempModifier;
			if (kModifierList && iTempModifier)
			{
				GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_NEAR_FRIENDLY_MINOR", iTempModifier, GET_PLAYER(eMinor).getName());
			}
		}
#endif
	}




	if (pOtherUnit != NULL)
	{
		CvAssertMsg(pOtherUnit != this, "Compared combat strength against one's own pointer. This is weird and probably wrong.");
		const CvUnit& otherUnit = *pOtherUnit;
		if (!kInfo.getAttackIsRanged() && !kInfo.getAttackIsBombingMission() && !kInfo.getAttackIsAirSweep())
		{

			int iNumAdjacentFriends = otherUnit.GetNumEnemyUnitsAdjacent(this);
			if (iNumAdjacentFriends > 0)
			{
				iTempModifier =        GC.getBONUS_PER_ADJACENT_FRIEND() * iNumAdjacentFriends;

				int iFlankModifier = GetFlankAttackModifier();
				if (iFlankModifier > 0)
				{
					iTempModifier = iTempModifier * (100 + iFlankModifier) / 100;
				}

				iModifier += iTempModifier;
				if (kModifierList && iTempModifier)
				{
					GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_FLANKING", iTempModifier, iNumAdjacentFriends);
				}
			}
		}


		iTempModifier = getUnitClassModifier(otherUnit.getUnitClassType());
		iModifier += iTempModifier;
		if (kModifierList && iTempModifier)
		{
			GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_UNIT_CLASS", iTempModifier, GC.getUnitClassInfo(otherUnit.getUnitClassType())->GetDescription());
		}


		if (otherUnit.getUnitCombatType() != NO_UNITCOMBAT)
		{
			iTempModifier = unitCombatModifier(otherUnit.getUnitCombatType());
			iModifier += iTempModifier;
			if (kModifierList && iTempModifier)
			{
				GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_UNIT_COMBAT_TYPE", iTempModifier, GC.getUnitCombatClassInfo(otherUnit.getUnitCombatType())->GetDescription());
			}
		}


		iTempModifier = domainModifier(otherUnit.getDomainType());
		iModifier += iTempModifier;
		if (kModifierList && iTempModifier)
		{
			GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_DOMAIN", iTempModifier, GC.getUnitDomainInfo(otherUnit.getDomainType())->GetDescription());
		}


		if (otherUnit.IsLargerCivThan(this))
		{
			iTempModifier = GET_PLAYER(getOwner()).GetPlayerTraits()->GetCombatBonusVsLargerCiv();
			iModifier += iTempModifier;
			if (kModifierList && iTempModifier)
			{
				GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_VS_LARGER_CIV", iTempModifier, GC.getCivilizationInfo(otherUnit.getCivilizationType())->GetDescription());
			}
		}
#ifdef NQ_COMBAT_BONUS_VS_SMALLER_CIV_FROM_POLICIES

		if (IsLargerCivThan(&otherUnit))
		{
			iTempModifier = GET_PLAYER(getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_COMBAT_BONUS_VS_SMALLER_CIV);
			iModifier += iTempModifier;
			if (kModifierList && iTempModifier)
			{
				GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_VS_SMALLER_CIV", iTempModifier, GC.getCivilizationInfo(otherUnit.getCivilizationType())->GetDescription());
			}
		}
#endif
#ifdef LEKMOD_DIFFERENT_IDEO_COMBAT_BONUS


		if (isUnitDifferentIdeology(&otherUnit))
		{
			iTempModifier = GetCombatBonusVsDifferentIdeologyModifier();
			iModifier += iTempModifier;
			if (kModifierList && iTempModifier)
			{
				PolicyBranchTypes eFreedomBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_FREEDOM();
				PolicyBranchTypes eAutocracyBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_AUTOCRACY();
				PolicyBranchTypes eOrderBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_ORDER();
				PolicyBranchTypes eOtherBranch = GET_PLAYER(otherUnit.getOwner()).GetPlayerPolicies()->GetLateGamePolicyTree();
				const char* extraText = "";
				if (eOtherBranch == eFreedomBranch)
				{
					extraText = "TXT_KEY_COMBATPANEL_FREEDOM";
				}
				else if (eOtherBranch == eAutocracyBranch)
				{
					extraText = "TXT_KEY_COMBATPANEL_AUTOCRACY";
				}
				else if (eOtherBranch == eOrderBranch)
				{
					extraText = "TXT_KEY_COMBATPANEL_ORDER";
				}
				GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_VS_DIFFERENT_IDEOLOGY", iTempModifier, extraText);
			}
		}
#endif
#ifdef LEKMOD_TOURISM_COMBAT_MOD
		iTempModifier = getTourismInfluenceCombatModifierVsUnit(&otherUnit);
		iModifier += iTempModifier;
		if (kModifierList && iTempModifier)
		{
			GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_TOURISM_INFLUENCE", iTempModifier);
		}
#endif
	}
	if (eOtherPlayer != NO_PLAYER)
	{
		CvPlayer& kOtherPlayer = GET_PLAYER(eOtherPlayer);
		if (kOtherPlayer.isMinorCiv())
		{
			iTempModifier = kPlayer.GetPlayerTraits()->GetCityStateCombatModifier();
			iModifier += iTempModifier;
			if (kModifierList && iTempModifier)
			{
				GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_CITY_STATE", iTempModifier, GC.getCivilizationInfo(kOtherPlayer.getCivilizationType())->GetDescription());
			}
		}
		else if (kOtherPlayer.isBarbarian())
		{
			CvHandicapInfo& thisGameHandicap = GC.getGame().getHandicapInfo();
			iTempModifier = kPlayer.GetBarbarianCombatBonus();
			iTempModifier += isHuman() ? thisGameHandicap.getBarbarianCombatModifier() : thisGameHandicap.getAIBarbarianCombatModifier();
			if (GC.getGame().isOption(GAMEOPTION_RAGING_BARBARIANS))
			{
				iTempModifier += 25;
			}
			iModifier += iTempModifier;
			if (kModifierList && iTempModifier)
			{
				GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_VS_BARBARIANS", iTempModifier);
			}
		}
	}

	if (kInfo.getPlot() != NULL && pOtherUnit != NULL)
	{
		const CvPlot& plot = *kInfo.getPlot();
		const CvUnit& otherUnit = *pOtherUnit;

		iTempModifier = kPlayer.GetPlayerTraits()->GetCombatBonusVsHigherTech();
		if (iTempModifier > 0)
		{

			if (plot.getOwner() == getOwner())
			{

				UnitTypes eMyUnitType = getUnitType();
				if (otherUnit.IsHigherTechThan(eMyUnitType))
				{		
					iModifier += iTempModifier;
					if (kModifierList && iTempModifier)
					{
						GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_COMBATMOD_VS_HIGHER_TECH", iTempModifier);
					}
				}
			}
		}
	}

	return iModifier;
}


int CvUnit::GetMaxAttackStrength(const CvCombatInfo& kInfo, CvCombatModifierList* kModifierList) const
{
	VALIDATE_OBJECT
	const bool bIsAttacker = kInfo.getUnit(BATTLE_UNIT_ATTACKER) == this;
	CvAssertMsg(bIsAttacker, "GetMaxAttackStrength called for a unit that is not the attacker.");
	if (!bIsAttacker)
		return 0;

	const CvPlot* pFromPlot = kInfo.getFromPlot() != NULL ? kInfo.getFromPlot() : plot();
	const CvPlot* pToPlot = kInfo.getPlot();
	CvUnit* pDefender = kInfo.getUnit(BATTLE_UNIT_DEFENDER);
	CvCity* pDefenderCity = kInfo.getCity(BATTLE_UNIT_DEFENDER);


	CvAssertMsg((pDefender != NULL) != (pDefenderCity != NULL), "Defender must be exactly one unit or city.");

	const bool bIsEmbarkedAttackingLand = isEmbarked() && pToPlot != NULL && !pToPlot->isWater();
	const bool bIgnoreEmbarkation = GET_PLAYER(getOwner()).GetPlayerTraits()->IsEmbarkedUnitsFullStrength() || bIsEmbarkedAttackingLand;
	CvPlayer& thisPlayer = GET_PLAYER(getOwner());

	if (isEmbarked() && !bIgnoreEmbarkation)
	{
		return GetEmbarkedUnitDefense();
	}
	
	const bool bRangedAttack = kInfo.getAttackIsRanged() || kInfo.getAttackIsBombingMission();
	const bool bUseRangedStrength = bRangedAttack || kInfo.getAttackIsAirSweep();

	int iCombat = bUseRangedStrength ? GetBaseRangedCombatStrength(isRangedSupportFire()) : GetBaseCombatStrength(bIgnoreEmbarkation);
	int iBaseCombat = iCombat;

	if (iCombat == 0)
		return 0;
#ifdef NQ_HEAVY_CHARGE_DOWNHILL
	const int iHeavyChargeDownhill = GetHeavyChargeDownhill();
	bool bAttackingFromHigherElevation = false;
	if (!bRangedAttack && iHeavyChargeDownhill != 0 && pFromPlot != NULL && pToPlot != NULL)
	{
		bAttackingFromHigherElevation = (pFromPlot->isMountain() && !pToPlot->isMountain()) || (pFromPlot->isHills() && pToPlot->isFlatlands());
	}
#endif
	int iModifier = GetGenericMaxStrengthModifier(kInfo, kModifierList);
	int iTempModifier = 0;


	iTempModifier = getAttackModifier();
	iTempModifier += bRangedAttack ? GetRangedAttackModifier() : 0;
	iModifier += iTempModifier;
	if (kModifierList && iTempModifier)
	{
		GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_ATTACK_MODIFIER", iTempModifier);
	}

	iTempModifier = getKamikazePercent();
	iModifier += iTempModifier;
	if (kModifierList && iTempModifier)
	{
		GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_KAMIKAZE", iTempModifier);
	}


	if (GET_PLAYER(getOwner()).GetAttackBonusTurns() > 0 && !bRangedAttack)
	{
		iTempModifier =        GC.getPOLICY_ATTACK_BONUS_MOD();
		iModifier += iTempModifier;
		if (kModifierList && iTempModifier)
		{
			GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_CLAUZWITZ", iTempModifier, GET_PLAYER(getOwner()).GetAttackBonusTurns());
		}
	}





	if (pToPlot != NULL)
	{
		const CvPlot& plot = *pToPlot;
#ifdef NQ_HEAVY_CHARGE_DOWNHILL

		if (bAttackingFromHigherElevation)
		{
			iTempModifier = iHeavyChargeDownhill;
			iModifier += iTempModifier;
			if (kModifierList && iTempModifier)
			{
				GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_HEAVY_CHARGE_DOWNHILL", iTempModifier);
			}
		}
#endif

#ifdef NQ_GOLDEN_AGE_FOREIGN_ATTACK_BONUS
		if (plot.getOwner() != getOwner())
		{
			if (thisPlayer.isGoldenAge())
			{
				iTempModifier = getGoldenAgeForeignAttackBonus();
				iModifier += iTempModifier;
				if (kModifierList && iTempModifier)
				{
					GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_GOLDEN_AGE_FOREIGN", iTempModifier);
				}
			}
		}
#endif


		if (pDefenderCity != NULL)
		{
			iTempModifier = cityAttackModifier();
			iModifier += iTempModifier;
			if (kModifierList && iTempModifier)
			{
				GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_CITY", iTempModifier);
			}


			if (IsNearSapper(pDefenderCity))
			{
				iTempModifier = GC.getSAPPED_CITY_ATTACK_MODIFIER();
				iModifier += iTempModifier;
				if (kModifierList && iTempModifier)
				{
					GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_SAPPER", iTempModifier);
				}
			}
#if defined(TRAITIFY)
			PromotionTypes eGreatGeneralPromotion = NO_PROMOTION;
			for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
			{
				const PromotionTypes eLoopPromotion = static_cast<PromotionTypes>(iI);
				CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eLoopPromotion);
				if (pkPromotionInfo)
				{
					if (pkPromotionInfo->IsGreatGeneral())
					{
						eGreatGeneralPromotion = eLoopPromotion;
						break;
					}
				}
			}
			if (eGreatGeneralPromotion != NO_PROMOTION && IsNearUnitWithPromotion(eGreatGeneralPromotion, GC.getGREAT_GENERAL_RANGE(), false                , true                ))
			{
				iTempModifier = GET_PLAYER(getOwner()).GetPlayerTraits()->GetGreatGeneralSiegeBonus();
				iModifier += iTempModifier;
				if (kModifierList && iTempModifier)
				{
					GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_NEAR_GREAT_GENERAL_SIEGE", iTempModifier);
				}
			}
#endif

			if (isBarbarian())
			{
				iTempModifier = GC.getBARBARIAN_CITY_ATTACK_MODIFIER();
				iModifier += iTempModifier;
				if (kModifierList && iTempModifier)
				{
					GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_BARBARIAN_PENALTY_VS_CITY", iTempModifier);
				}
			}
		}
		else
		{

			if (plot.isHills())
			{
				iTempModifier = hillsAttackModifier();
				iModifier += iTempModifier;
				if (kModifierList && iTempModifier)
				{
					GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_HILLS", iTempModifier);
				}
			}


			if (plot.isOpenGround())
			{
				iTempModifier = bRangedAttack ? openRangedAttackModifier() : openAttackModifier();
				iModifier += iTempModifier;
				if (kModifierList && iTempModifier)
				{
					GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_OPEN", iTempModifier);
				}
			}


			if (plot.isRoughGround())
			{
				iTempModifier = bRangedAttack ? roughRangedAttackModifier() : roughAttackModifier();
				iModifier += iTempModifier;
				if (kModifierList && iTempModifier)
				{
					GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_ROUGH", iTempModifier);
				}
			}


			if (plot.getFeatureType() != NO_FEATURE)
			{
				iTempModifier = featureAttackModifier(plot.getFeatureType());
				iModifier += iTempModifier;
				if (kModifierList && iTempModifier)
				{
					GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_FEATURE", iTempModifier, GC.getFeatureInfo(plot.getFeatureType())->GetDescription());
				}
			}
			else
			{
				iTempModifier = terrainAttackModifier(plot.getTerrainType());
				const char* szTerrainText = GC.getTerrainInfo(plot.getTerrainType())->GetDescription();
				if (plot.isHills())
				{
					iTempModifier += terrainAttackModifier(TERRAIN_HILL);
					szTerrainText = "TXT_KEY_TERRAIN_HILL";
				}
				else if (plot.isMountain())
				{
					iTempModifier += terrainAttackModifier(TERRAIN_MOUNTAIN);
					szTerrainText = "TXT_KEY_TERRAIN_MOUNTAIN";
				}
				iModifier += iTempModifier;
				if (kModifierList && iTempModifier)
				{
					GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_TERRAIN", iTempModifier, szTerrainText);
				}
			}
		}


		if (plot.IsFriendlyTerritory(getOwner()))
		{
			iTempModifier = getFriendlyLandsAttackModifier();
			iModifier += iTempModifier;
			if (kModifierList && iTempModifier)
			{
				GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_FRIENDLY_LANDS", iTempModifier);
			}
		}





		if (pFromPlot != NULL)
		{
			const CvPlot& fromPlot = *pFromPlot;

			if (!isRiverCrossingNoPenalty() && !bRangedAttack)
			{
				if (fromPlot.isRiverCrossing(directionXY(&fromPlot, &plot)))
				{
					iTempModifier = GC.getRIVER_ATTACK_MODIFIER();
					iModifier += iTempModifier;
					if (kModifierList && iTempModifier)
					{
						GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_RIVER_ATTACK_PENALTY", iTempModifier);
					}
				}
			}


			if (!isAmphib())
			{
				if (bIsEmbarkedAttackingLand)
				{
					iTempModifier = GC.getAMPHIB_ATTACK_MODIFIER();
					iModifier += iTempModifier;
					if (kModifierList && iTempModifier)
					{
						GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_AMPHIB_ATTACK_PENALTY", iTempModifier);
					}
				}
			}
		}
	}





	if (pDefender != NULL)
	{
		CvAssertMsg(pDefender != this, "Compared attack strength against one's own pointer. This is weird and probably wrong.");
		const CvUnit& defender = *pDefender;

		iTempModifier = unitClassAttackModifier(defender.getUnitClassType());
		iModifier += iTempModifier;
		if (kModifierList && iTempModifier)
		{
			GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_UNIT_CLASS", iTempModifier, GC.getUnitClassInfo(defender.getUnitClassType())->GetDescription());
		}
#if defined(LEKMOD_DOMAIN_PROMO_ATTACK_DEFENSE)

		iTempModifier = domainAttackModifier(defender.getDomainType());
		iModifier += iTempModifier;
		if (kModifierList && iTempModifier)
		{
			GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_DOMAIN", iTempModifier, GC.getUnitDomainInfo(defender.getDomainType())->GetDescription());
		}
#endif


		if (defender.getFortifyTurns() > 0)
		{
			iTempModifier = attackFortifiedModifier();
			iModifier += iTempModifier;
			if (kModifierList && iTempModifier)
			{
				GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_FORTIFIED", iTempModifier);
			}
		}

		if (defender.getDamage() > 0)
		{
			iTempModifier = attackWoundedModifier();
			iModifier += iTempModifier;
			if (kModifierList && iTempModifier)
			{
				GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_WOUNDED", iTempModifier);
			}
		}
	}


	if (iModifier < -90)
		iModifier = -90;

	iCombat *= (iModifier + 100);

#ifdef NQ_HEAVY_CHARGE_DOWNHILL
	if (!bRangedAttack && pDefender != NULL)
	{
		if ((IsCanHeavyCharge() || (bAttackingFromHigherElevation)))
		{
			if (!pDefender->CanFallBackFromMelee(*this))
			{
				if (kModifierList)
				{
					iTempModifier = (((iCombat * 150) / 100) - iCombat) / iBaseCombat;
					GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_HEAVY_CHARGE", iTempModifier);
				}
				iCombat *= 150;
				iCombat /= 100;
			}
		}
	}
#else
	if (IsCanHeavyCharge() && pDefender && pToPlot && !pDefender->CanFallBackFromMelee(*this))
	{
		iCombat = (iCombat * 150) / 100;
	}
#endif
	if (kInfo.getAttackIsAirSweep())
	{
		if (kModifierList)
		{
			iTempModifier = (((iCombat * (100 + GetAirSweepCombatModifier())) / 100) - iCombat) / iBaseCombat;
			GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_ATTACKMOD_AIR_SWEEP", iTempModifier);
		}
		iCombat *= (100 + GetAirSweepCombatModifier());
		iCombat /= 100;
	}

	return std::max(1, iCombat);
}


int CvUnit::GetMaxDefenseStrength(const CvCombatInfo& kInfo, CvCombatModifierList* kModifierList) const
{
	VALIDATE_OBJECT
	const bool bIsDefender = kInfo.getUnit(BATTLE_UNIT_DEFENDER) == this || kInfo.getUnit(BATTLE_UNIT_INTERCEPTOR) == this;
	CvAssertMsg(bIsDefender, "GetMaxDefenseStrength called for a unit that is not the defender.");
	if (!bIsDefender)
		return 0;
	CvUnit* pAttacker = kInfo.getUnit(BATTLE_UNIT_ATTACKER);
	CvCity* pAttackerCity = kInfo.getCity(BATTLE_UNIT_ATTACKER);
	CvAssertMsg((pAttacker != NULL) != (pAttackerCity != NULL), "Attacker must be exactly one unit or city.");

	const CvPlot* pDefensePlot = kInfo.getPlot();
	

	CvPlayer& thisPlayer = GET_PLAYER(getOwner());

	if (isEmbarked())
	{
		if (!GET_PLAYER(getOwner()).GetPlayerTraits()->IsEmbarkedUnitsFullStrength())
		{
			return GetEmbarkedUnitDefense();
		}
	}
	
	const bool bRangedAttack = kInfo.getAttackIsRanged() || kInfo.getAttackIsBombingMission();
	const bool bInterceptionCombat = kInfo.getAttackIsAirSweep() || kInfo.getUnit(BATTLE_UNIT_INTERCEPTOR) == this;

	const int iBaseRangedCombat = GetBaseRangedCombatStrength();
	int iBaseCombat = GetBaseCombatStrength(GET_PLAYER(getOwner()).GetPlayerTraits()->IsEmbarkedUnitsFullStrength());

	if (bInterceptionCombat)
	{
		if (getDomainType() == DOMAIN_AIR && iBaseRangedCombat > 0)
		{
			iBaseCombat = iBaseRangedCombat;
		}
	}
	else if (bRangedAttack && getDomainType() != DOMAIN_SEA && iBaseRangedCombat > 0)
	{
		iBaseCombat = iBaseRangedCombat;
	}
	if (iBaseCombat == 0)
		return 0;

	int iTempModifier;
	int iCombat = iBaseCombat;
	int iModifier = GetGenericMaxStrengthModifier(kInfo, kModifierList);


	iTempModifier = getDefenseModifier();
	iModifier += iTempModifier;
	if (kModifierList && iTempModifier)
	{
		GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_DEFENSEMOD_GENERIC", iTempModifier);
	}


	if (bRangedAttack)
	{
		iTempModifier = rangedDefenseModifier();
		iModifier += iTempModifier;
		if (kModifierList && iTempModifier)
		{
			GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_DEFENSEMOD_RANGED", iTempModifier);
		}
	}





	if (pDefensePlot != NULL)
	{
		const CvPlot& plot = *pDefensePlot;


		iTempModifier = plot.defenseModifier(getTeam(), (pAttacker != NULL) ? pAttacker->ignoreBuildingDefense() : true);


		if (!noDefensiveBonus() || iTempModifier < 0)
		{
			iModifier += iTempModifier;
			if (kModifierList && iTempModifier)
			{
				GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_DEFENSEMOD_TERRAIN", iTempModifier);
			}
		}


		iTempModifier = fortifyModifier();
#if defined(LEKMOD_NO_FORTIFY_VS_RANGED_PROMO)

		const bool bNonMeleeAttack = bRangedAttack || kInfo.getAttackIsAirSweep() ||
			(pAttacker != NULL && pAttacker->getDomainType() == DOMAIN_AIR);
		const bool bStripFortify = isNoFortifyVsRanged() && bNonMeleeAttack;
		if (bStripFortify)
		{

		}
		else
#endif
		{
			iModifier += iTempModifier;
			if (kModifierList && iTempModifier)
			{
#if defined(LEKMOD_NO_FORTIFY_VS_RANGED_PROMO)
				if (isNoFortifyVsRanged())
				{
					Localization::String localizedText = Localization::Lookup("TXT_KEY_DEFENSEMOD_FORTIFICATION_VS_COMBAT");
					localizedText << Localization::Lookup("TXT_KEY_DEFENSEMOD_FORTIFY_GROUP_MELEE").toUTF8();
					kModifierList->AddEntry(localizedText.toUTF8(), iTempModifier);
				}
				else
#endif
				{
					GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_DEFENSEMOD_FORTIFICATION", iTempModifier, getFortifyTurns());
				}
			}
		}


		if (plot.isHills())
		{
			iTempModifier = hillsDefenseModifier();
			iModifier += iTempModifier;
			if (kModifierList && iTempModifier)
			{
				GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_DEFENSEMOD_HILLS", iTempModifier);
			}
		}


		if (plot.isOpenGround())
		{
			iTempModifier = openDefenseModifier();
			iModifier += iTempModifier;
			if (kModifierList && iTempModifier)
			{
				GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_DEFENSEMOD_OPEN_GROUND", iTempModifier);
			}
		}


		if (plot.isRoughGround())
		{
			iTempModifier = roughDefenseModifier();
			iModifier += iTempModifier;
			if (kModifierList && iTempModifier)
			{
				GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_DEFENSEMOD_ROUGH_GROUND", iTempModifier);
			}
		}


		if (plot.getFeatureType() != NO_FEATURE)
		{
			iTempModifier = featureDefenseModifier(plot.getFeatureType());
			iModifier += iTempModifier;
			if (kModifierList && iTempModifier)
			{
				GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_DEFENSEMOD_FEATURE", iTempModifier, GC.getFeatureInfo(plot.getFeatureType())->GetDescription());
			}
		}
		else
		{
			iTempModifier = terrainDefenseModifier(plot.getTerrainType());
			const char* szTerrainText = GC.getTerrainInfo(plot.getTerrainType())->GetDescription();

			if (plot.isHills())
			{
				iTempModifier += terrainDefenseModifier(TERRAIN_HILL);
				szTerrainText = "TXT_KEY_TERRAIN_HILL";
			}
			if (plot.isMountain())
			{
				iTempModifier += terrainDefenseModifier(TERRAIN_MOUNTAIN);
				szTerrainText = "TXT_KEY_TERRAIN_MOUNTAIN";
			}
			iModifier += iTempModifier;
			if (kModifierList && iTempModifier)
			{
				GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_DEFENSEMOD_TERRAIN_EXTRA", iTempModifier, szTerrainText);
			}
		}
	}




	if (pAttacker != NULL)
	{
		CvAssertMsg(pAttacker != this, "Compared defense strength against one's own pointer. This is weird and probably wrong.");
		const CvUnit& attacker = *pAttacker;

		iTempModifier = unitClassDefenseModifier(attacker.getUnitClassType());
		iModifier += iTempModifier;
		if (kModifierList && iTempModifier)
		{
			GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_DEFENSEMOD_UNIT_CLASS", iTempModifier, GC.getUnitClassInfo(attacker.getUnitClassType())->GetDescription());
		}
#if defined(LEKMOD_DOMAIN_PROMO_ATTACK_DEFENSE)

		iTempModifier = domainDefenseModifier(attacker.getDomainType());
		iModifier += iTempModifier;
		if (kModifierList && iTempModifier)
		{
			GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_DEFENSEMOD_DOMAIN", iTempModifier, GC.getUnitDomainInfo(attacker.getDomainType())->GetDescription());
		}
#endif
	}



	if (pAttackerCity != NULL)
	{

		iTempModifier = cityDefenseModifier();
		iModifier += iTempModifier;
		if (kModifierList && iTempModifier)
		{
			GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_DEFENSEMOD_CITY", iTempModifier);
		}
	}


	if (iModifier < -90)
		iModifier = -90;

	iCombat *= (iModifier + 100);


	if (pAttacker != NULL)
	{
		if (pAttacker->getDomainType() == DOMAIN_SEA && getDomainType() == DOMAIN_SEA)
		{
			iCombat *=         GC.getNAVAL_COMBAT_DEFENDER_STRENGTH_MULTIPLIER();
			iCombat /= 100;
		}
	}
	if (thisPlayer.GetPlayerTraits()->IsEmbarkedUnitsFullStrength() && isEmbarked())
	{
		if (kModifierList)
		{
			iTempModifier = (((iCombat * (100 + GetEmbarkDefensiveModifier())) / 100) - iCombat) / iBaseCombat;
			GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_DEFENSEMOD_EMBARKED", iTempModifier);
		}
		iCombat *= (100 + GetEmbarkDefensiveModifier());
		iCombat /= 100;
	}

	if (kInfo.getUnit(BATTLE_UNIT_INTERCEPTOR) == this)
	{
		if (kModifierList)
		{
			iTempModifier = (((iCombat * (100 + GetInterceptionCombatModifier())) / 100) - iCombat) / iBaseCombat;
			GC.getGame().BuildCombatModHelpText(*kModifierList, "TXT_KEY_DEFENSEMOD_INTERCEPTION", iTempModifier);
		}
		iCombat *= (100 + GetInterceptionCombatModifier());
		iCombat /= 100;
	}

	return std::max(1, iCombat);
}

int CvUnit::GetMaxRangedCombatStrength(const CvCombatInfo& kInfo, CvCombatModifierList* kModifierList) const
{
	VALIDATE_OBJECT
	const bool bRangedCombat = kInfo.getAttackIsRanged() || kInfo.getAttackIsBombingMission();
	CvAssertMsg(bRangedCombat, "GetMaxRangedCombatStrength called for non-ranged combat.");
	if (!bRangedCombat)
		return 0;

	const bool bIsAttacker = kInfo.getUnit(BATTLE_UNIT_ATTACKER) == this;
	const bool bIsDefender = kInfo.getUnit(BATTLE_UNIT_DEFENDER) == this;
	CvAssertMsg(bIsAttacker != bIsDefender, "Unit must occupy exactly one combat side.");

	if (bIsAttacker)
	{
		return GetMaxAttackStrength(kInfo, kModifierList);
	}
	if (bIsDefender)
	{
		return GetMaxDefenseStrength(kInfo, kModifierList);
	}

	return 0;
}
#endif


int CvUnit::GetEmbarkedUnitDefense() const
{
	int iRtnValue;
	int iModifier;
	CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
	EraTypes eEra = kPlayer.GetCurrentEra();

	iRtnValue = GC.getEraInfo(eEra)->getEmbarkedUnitDefense() * 100;

	iModifier = GetEmbarkDefensiveModifier();
	if(iModifier > 0)
	{
		iRtnValue = iRtnValue * (100 + iModifier);
		iRtnValue /= 100;
	}

#ifdef NO_EMBARKED_CIVILIAN_DEFENSE
	if (IsCombatUnit())
		return iRtnValue;
	else
		return 0;
#else
	return iRtnValue;
#endif
}


bool CvUnit::canSiege(TeamTypes eTeam) const
{
	VALIDATE_OBJECT
	if(!IsCanDefend())
	{
		return false;
	}

	if(!isEnemy(eTeam))
	{
		return false;
	}

	if(!isNeverInvisible())
	{
		return false;
	}

	return true;
}



int CvUnit::GetBaseRangedCombatStrength(bool bRangedSupportFire) const
{
	VALIDATE_OBJECT
	if (bRangedSupportFire)
	{
		return m_iBaseCombat / 2;
	}

	return m_iBaseRangedCombat;
}
void CvUnit::ChangeBaseRangedCombatStrength(int iChange)
{
	VALIDATE_OBJECT
	m_iBaseRangedCombat += iChange;
	m_iCachedPower = getUnitInfo().DoUpdatePower(m_iBaseCombat, m_iBaseRangedCombat);
}


bool CvUnit::canAirAttack() const
{
	VALIDATE_OBJECT
	return (GetBaseRangedCombatStrength() > 0);
}



bool CvUnit::canAirDefend(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(pPlot == NULL)
	{
		pPlot = plot();
	}

	if(maxInterceptionProbability() == 0)
	{
		return false;
	}

	if(getDomainType() != DOMAIN_AIR)
	{
		if(!pPlot->isValidDomainForLocation(*this))
		{
			return false;
		}
	}

	return true;
}



int CvUnit::GetAirCombatDamage(const CvUnit* pDefender, CvCity* pCity, bool bIncludeRand, int iAssumeExtraDamage) const
{
	VALIDATE_OBJECT
#if defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
	CvCombatInfo kCombatInfo;
	kCombatInfo.setUnit(BATTLE_UNIT_ATTACKER, const_cast<CvUnit*>(this));
	kCombatInfo.setUnit(BATTLE_UNIT_DEFENDER, const_cast<CvUnit*>(pDefender));
	kCombatInfo.setCity(BATTLE_UNIT_DEFENDER, pCity);
	kCombatInfo.setPlot(pDefender ? pDefender->plot() : pCity->plot());
	kCombatInfo.setAttackIsBombingMission(true);

	int iAttackerStrength = GetMaxRangedCombatStrength(kCombatInfo, NULL);
#else
	int iAttackerStrength = GetMaxRangedCombatStrength(pDefender, pCity,                true,                      true);
#endif
	int iDefenderStrength;


	if(pCity == NULL)
	{
#if !defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
#ifdef LEKMOD_SPEARTHROW_FIX

		if (pDefender->GetMaxRangedCombatStrength(this,           NULL, false,                      false) > 0 && !pDefender->getDomainType() == DOMAIN_SEA && !pDefender->isRangedSupportFire())
		{
			iDefenderStrength = pDefender->GetMaxRangedCombatStrength(this,           NULL, false,                      false);
		}
#else

		if (pDefender->GetMaxRangedCombatStrength(this,           NULL, false,                      false) > 0 && !pDefender->getDomainType() == DOMAIN_SEA)
		{
			iDefenderStrength = pDefender->GetMaxRangedCombatStrength(this,           NULL, false,                      false);
		}
#endif
		else
		{
			iDefenderStrength = pDefender->GetMaxDefenseStrength(pDefender->plot(), this,                       true);
		}
	}
#else
		iDefenderStrength = pDefender->GetMaxDefenseStrength(kCombatInfo);
	}
#endif
		

	else
	{
		iDefenderStrength = pCity->getStrengthValue();
	}




	int iAttackerDamageRatio = GC.getMAX_HIT_POINTS() - getDamage() - iAssumeExtraDamage;
	if(iAttackerDamageRatio < 0)
		iAttackerDamageRatio = 0;

	int iAttackerDamage =         GC.getRANGE_ATTACK_SAME_STRENGTH_MIN_DAMAGE();
#ifndef NQM_COMBAT_RNG_USE_BINOM_RNG_OPTION
	iAttackerDamage *= iAttackerDamageRatio;
	iAttackerDamage /= GC.getMAX_HIT_POINTS();
#endif

	int iAttackerRoll = 0;
	if(bIncludeRand)
	{
#ifdef NQM_COMBAT_RNG_USE_BINOM_RNG_OPTION
		if (GC.getGame().isOption("GAMEOPTION_USE_BINOM_RNG_FOR_COMBAT_ROLLS"))
		{
			int iAverageDamage = iAttackerDamage + (GC.getRANGE_ATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE() / 2);
			int iSigma = GC.getRANGE_ATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE() / 6;
			int iMaxRoll = iSigma*iSigma * 4 + 1;
			iAttackerRoll = iAverageDamage + GC.getGame().getJonRandNumBinom(iMaxRoll, "Unit Ranged Combat Damage") - (iMaxRoll / 2) - iAverageDamage;
		}
		else
#endif
		iAttackerRoll =         GC.getGame().getJonRandNum(GC.getRANGE_ATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE(), "Unit Ranged Combat Damage");
#ifndef NQM_COMBAT_RNG_USE_BINOM_RNG_OPTION
		iAttackerRoll *= iAttackerDamageRatio;
		iAttackerRoll /= GC.getMAX_HIT_POINTS();
#endif
	}
	else
	{
		iAttackerRoll =         GC.getRANGE_ATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE();
		iAttackerRoll -= 1;
#ifndef NQM_COMBAT_RNG_USE_BINOM_RNG_OPTION
		iAttackerRoll *= iAttackerDamageRatio;
		iAttackerRoll /= GC.getMAX_HIT_POINTS();
#endif
		iAttackerRoll /= 2;
	}
	iAttackerDamage += iAttackerRoll;
#ifdef NQM_COMBAT_RNG_USE_BINOM_RNG_OPTION
	iAttackerDamage = MAX(1, MIN(iAttackerDamage, GC.getMAX_HIT_POINTS() * 100)) * iAttackerDamageRatio / GetMaxHitPoints();
#endif

	double fStrengthRatio = ((iDefenderStrength > 0)?(double(iAttackerStrength) / iDefenderStrength):double(iAttackerStrength));


	if(iDefenderStrength > iAttackerStrength)
	{
		fStrengthRatio = (double(iDefenderStrength) / iAttackerStrength);
	}

	fStrengthRatio = (fStrengthRatio + 3) / 4;
	fStrengthRatio = pow(fStrengthRatio, 4.0);
	fStrengthRatio = (fStrengthRatio + 1) / 2;

	if(iDefenderStrength > iAttackerStrength)
	{
		fStrengthRatio = 1 / fStrengthRatio;
	}

	double fAttackerDamage = (double)iAttackerDamage * fStrengthRatio;

	if(fAttackerDamage > INT_MAX)
		iAttackerDamage = INT_MAX;
	else
		iAttackerDamage = int(fAttackerDamage);


	iAttackerDamage /= 100;

	iAttackerDamage = max(1,iAttackerDamage);

	return iAttackerDamage;
}



int CvUnit::GetRangeCombatDamage(const CvUnit* pDefender, CvCity* pCity, bool bIncludeRand, int iAssumeExtraDamage) const
{
	VALIDATE_OBJECT
#if defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
	CvCombatInfo kCombatInfo;
	kCombatInfo.setUnit(BATTLE_UNIT_ATTACKER, const_cast<CvUnit*>(this));
	kCombatInfo.setUnit(BATTLE_UNIT_DEFENDER, const_cast<CvUnit*>(pDefender));
	kCombatInfo.setCity(BATTLE_UNIT_DEFENDER, pCity);
	kCombatInfo.setPlot(pDefender ? pDefender->plot() : pCity->plot());
	kCombatInfo.setAttackIsRanged(true);
	int iAttackerStrength = GetMaxRangedCombatStrength(kCombatInfo);
#else
	int iAttackerStrength = GetMaxRangedCombatStrength(pDefender, pCity, true,                      true);
#endif
	int iDefenderStrength;

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());


	if(pCity == NULL)
	{
#if !defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)

		if(!pDefender->IsCanDefend())
			return       GC.getNONCOMBAT_UNIT_RANGED_DAMAGE();

		if (pDefender->isEmbarked())
		{
			iDefenderStrength = pDefender->GetEmbarkedUnitDefense();
		}


		else if(!pDefender->isRangedSupportFire() && pDefender->getDomainType() != DOMAIN_SEA && pDefender->GetMaxRangedCombatStrength(this,           NULL, false, false) > 0)
		{
			iDefenderStrength = pDefender->GetMaxRangedCombatStrength(this,           NULL, false,                      false);


			iDefenderStrength *=         GC.getRANGE_ATTACK_RANGED_DEFENDER_MOD();
			iDefenderStrength /= 100;
		}
		else
		{
			iDefenderStrength = pDefender->GetMaxDefenseStrength(pDefender->plot(), this,                       true);
		}
#else
		iDefenderStrength = pDefender->GetMaxDefenseStrength(kCombatInfo);
#endif
	}

	else
	{
		iDefenderStrength = pCity->getStrengthValue();
	}




	int iWoundedDamageMultiplier =        GC.getWOUNDED_DAMAGE_MULTIPLIER();
	iWoundedDamageMultiplier += kPlayer.GetWoundedUnitDamageMod();


	int iAttackerDamageRatio = GC.getMAX_HIT_POINTS() - ((getDamage() - iAssumeExtraDamage) * iWoundedDamageMultiplier / 100);
	if(iAttackerDamageRatio < 0)
		iAttackerDamageRatio = 0;

	int iAttackerDamage =         GC.getRANGE_ATTACK_SAME_STRENGTH_MIN_DAMAGE();
	iAttackerDamage *= iAttackerDamageRatio;
	iAttackerDamage /= GC.getMAX_HIT_POINTS();

	int iAttackerRoll = 0;
	if(bIncludeRand)
	{
		iAttackerRoll =         GC.getGame().getJonRandNum(GC.getRANGE_ATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE(), "Unit Ranged Combat Damage");
		iAttackerRoll *= iAttackerDamageRatio;
		iAttackerRoll /= GC.getMAX_HIT_POINTS();
	}
	else
	{
		iAttackerRoll =         GC.getRANGE_ATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE();
		iAttackerRoll -= 1;
		iAttackerRoll *= iAttackerDamageRatio;
		iAttackerRoll /= GC.getMAX_HIT_POINTS();
		iAttackerRoll /= 2;
	}
	iAttackerDamage += iAttackerRoll;

	double fStrengthRatio = (iDefenderStrength > 0)?(double(iAttackerStrength) / iDefenderStrength):double(iAttackerStrength);


	if(iDefenderStrength > iAttackerStrength)
	{
		fStrengthRatio = (double(iDefenderStrength) / iAttackerStrength);
	}

	fStrengthRatio = (fStrengthRatio + 3) / 4;
	fStrengthRatio = pow(fStrengthRatio, 4.0);
	fStrengthRatio = (fStrengthRatio + 1) / 2;

	if(iDefenderStrength > iAttackerStrength)
	{
		fStrengthRatio = 1 / fStrengthRatio;
	}

	double fAttackerDamage = (double)iAttackerDamage * fStrengthRatio;

	if(fAttackerDamage > INT_MAX)
		iAttackerDamage = INT_MAX;
	else
		iAttackerDamage = int(fAttackerDamage);


	iAttackerDamage /= 100;

	iAttackerDamage = max(1,iAttackerDamage);

	return iAttackerDamage;
}


int CvUnit::GetAirStrikeDefenseDamage(const CvUnit* pAttacker, bool bIncludeRand) const
{
#if !defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
	int iAttackerStrength = pAttacker->GetMaxRangedCombatStrength(this,           NULL, true,                      false);
	int iDefenderStrength = 0;


	if(GetMaxRangedCombatStrength(this,           NULL, false, false) > 0 && !getDomainType() == DOMAIN_SEA)
		iDefenderStrength = GetMaxRangedCombatStrength(pAttacker,           NULL, false, false);
	else
		iDefenderStrength = GetMaxDefenseStrength(plot(), pAttacker);
#else
	CvCombatInfo kCombatInfo;
	kCombatInfo.setUnit(BATTLE_UNIT_ATTACKER, const_cast<CvUnit*>(pAttacker));
	kCombatInfo.setUnit(BATTLE_UNIT_DEFENDER, const_cast<CvUnit*>(this));
	kCombatInfo.setPlot(plot());
	kCombatInfo.setAttackIsBombingMission(true);

	int iAttackerStrength = pAttacker->GetMaxAttackStrength(kCombatInfo);
	int iDefenderStrength = GetMaxDefenseStrength(kCombatInfo);
#endif
	if(iDefenderStrength == 0)
		return 0;

	int iDefenderDamageRatio = GC.getMAX_HIT_POINTS() - getDamage();
#ifdef NQM_COMBAT_RNG_USE_BINOM_RNG_OPTION
	int iDefenderDamage =         GC.getAIR_STRIKE_SAME_STRENGTH_MIN_DEFENSE_DAMAGE();
#else
	int iDefenderDamage =         GC.getAIR_STRIKE_SAME_STRENGTH_MIN_DEFENSE_DAMAGE() * iDefenderDamageRatio / GC.getMAX_HIT_POINTS();
#endif

	int iDefenderRoll = 0;
	if(bIncludeRand)
	{
#ifdef NQM_COMBAT_RNG_USE_BINOM_RNG_OPTION
		if (GC.getGame().isOption("GAMEOPTION_USE_BINOM_RNG_FOR_COMBAT_ROLLS"))
		{
			int iAverageDamage = iDefenderDamage + (GC.getAIR_STRIKE_SAME_STRENGTH_POSSIBLE_EXTRA_DEFENSE_DAMAGE() / 2);
			int iSigma = GC.getAIR_STRIKE_SAME_STRENGTH_POSSIBLE_EXTRA_DEFENSE_DAMAGE() / 6;
			int iMaxRoll = iSigma*iSigma * 4 + 1;
			iDefenderRoll = iAverageDamage + GC.getGame().getJonRandNumBinom(iMaxRoll, "Unit Air Strike Combat Damage") - (iMaxRoll / 2) - iAverageDamage;
		}
		else
#endif
		iDefenderRoll =         GC.getGame().getJonRandNum(GC.getAIR_STRIKE_SAME_STRENGTH_POSSIBLE_EXTRA_DEFENSE_DAMAGE(), "Unit Air Strike Combat Damage");
#ifndef NQM_COMBAT_RNG_USE_BINOM_RNG_OPTION
		iDefenderRoll *= iDefenderDamageRatio;
		iDefenderRoll /= GC.getMAX_HIT_POINTS();
#endif
	}
	else
	{
		iDefenderRoll =         GC.getAIR_STRIKE_SAME_STRENGTH_POSSIBLE_EXTRA_DEFENSE_DAMAGE();
		iDefenderRoll -= 1;
#ifndef NQM_COMBAT_RNG_USE_BINOM_RNG_OPTION
		iDefenderRoll *= iDefenderDamageRatio;
		iDefenderRoll /= GC.getMAX_HIT_POINTS();
#endif
		iDefenderRoll /= 2;
	}
	iDefenderDamage += iDefenderRoll;
#ifdef NQM_COMBAT_RNG_USE_BINOM_RNG_OPTION
	iDefenderDamage = MAX(1, MIN(iDefenderDamage, GC.getMAX_HIT_POINTS() * 100)) * iDefenderDamageRatio / GetMaxHitPoints();
#endif

	double fStrengthRatio = (double(iDefenderStrength) / iAttackerStrength);


	if(iAttackerStrength > iDefenderStrength)
	{
		fStrengthRatio = (double(iAttackerStrength) / iDefenderStrength);
	}

	fStrengthRatio = (fStrengthRatio + 3) / 4;
	fStrengthRatio = pow(fStrengthRatio, 4.0);
	fStrengthRatio = (fStrengthRatio + 1) / 2;

	if(iAttackerStrength > iDefenderStrength)
	{
		fStrengthRatio = 1 / fStrengthRatio;
	}

	iDefenderDamage = int(iDefenderDamage * fStrengthRatio);


	iDefenderDamage /= 100;

	iDefenderDamage = max(1,iDefenderDamage);

	return iDefenderDamage;
}


CvUnit* CvUnit::GetBestInterceptor(const CvPlot& interceptPlot, CvUnit* pkDefender             , bool bLandInterceptorsOnly          , bool bVisibleInterceptorsOnly          , bool bIgnoreInterceptionState          ) const
{
	VALIDATE_OBJECT
		CvUnit* pLoopUnit;
	CvUnit* pBestUnit;
	int iValue;
	int iBestValue;
	int iLoop;
	int iI;

	iBestValue = 0;
	pBestUnit = NULL;


	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kLoopPlayer.isAlive())
		{
			TeamTypes eLoopTeam = kLoopPlayer.getTeam();
			if(isEnemy(eLoopTeam) && !isInvisible(eLoopTeam, false, false))
			{
				for(pLoopUnit = kLoopPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kLoopPlayer.nextUnit(&iLoop))
				{
					if (pLoopUnit == NULL)
						continue;
					if (pLoopUnit == pkDefender)
						continue;
					if (pLoopUnit->isDelayedDeath())
						continue;
					if (!pLoopUnit->canAirDefend())
						continue;
					if (pLoopUnit->isInCombat())
						continue;
					if (!bIgnoreInterceptionState)
					{
						if (pLoopUnit->isOutOfInterceptions())
							continue;
						if ((pLoopUnit->getDomainType() == DOMAIN_AIR) && (pLoopUnit->hasMoved()))
							continue;
						if ((pLoopUnit->getDomainType() == DOMAIN_AIR) && (pLoopUnit->GetActivityType() != ACTIVITY_INTERCEPT))
							continue;
					}


					if (!bLandInterceptorsOnly || pLoopUnit->getDomainType() == DOMAIN_LAND)
					{
						if (!bVisibleInterceptorsOnly || pLoopUnit->plot()->isVisible(getTeam()))
						{

							if(plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), interceptPlot.getX(), interceptPlot.getY()) <= pLoopUnit->getUnitInfo().GetAirInterceptRange())
							{
								iValue = pLoopUnit->currInterceptionProbability();

								if(iValue > iBestValue)
								{
									iBestValue = iValue;
									pBestUnit = pLoopUnit;
								}
							}
						}
					}
				}
			}
		}
	}

	return pBestUnit;
}


int CvUnit::GetInterceptorCount(const CvPlot& interceptPlot, CvUnit* pkDefender             , bool bLandInterceptorsOnly          , bool bVisibleInterceptorsOnly          , bool bIgnoreInterceptionState          ) const
{
	VALIDATE_OBJECT
	
	CvUnit* pLoopUnit;
	int iReturnValue;
	int iLoop;
	int iI;

	iReturnValue = 0;


	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (!kLoopPlayer.isAlive())
			continue;
			TeamTypes eLoopTeam = kLoopPlayer.getTeam();
		if (isEnemy(eLoopTeam) && !isInvisible(eLoopTeam, false, false))
			{
				for(pLoopUnit = kLoopPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kLoopPlayer.nextUnit(&iLoop))
				{

				if (pLoopUnit == pkDefender)
					continue;

				if (pLoopUnit->isDelayedDeath())
					continue;

				if (!pLoopUnit->canAirDefend())
					continue;

				if (pLoopUnit->isInCombat())
					continue;


				if (!bIgnoreInterceptionState && pLoopUnit->isOutOfInterceptions())
					continue;


							if(bIgnoreInterceptionState || (pLoopUnit->getDomainType() != DOMAIN_AIR) || !(pLoopUnit->hasMoved()))
							{

								if(bIgnoreInterceptionState || (pLoopUnit->getDomainType() != DOMAIN_AIR) || (pLoopUnit->GetActivityType() == ACTIVITY_INTERCEPT))
								{

									if (!bLandInterceptorsOnly || pLoopUnit->getDomainType() == DOMAIN_LAND)
									{
										if (!bVisibleInterceptorsOnly || pLoopUnit->plot()->isVisible(getTeam()))
										{

											if(plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), interceptPlot.getX(), interceptPlot.getY()) <= pLoopUnit->getUnitInfo().GetAirInterceptRange())
											{
												if (pLoopUnit->currInterceptionProbability() > 0)
												{
													iReturnValue++;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return iReturnValue;
}



int CvUnit::GetInterceptionDamage(const CvUnit* pAttacker, bool bIncludeRand) const
{
#if !defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
	int iAttackerStrength = pAttacker->GetMaxRangedCombatStrength(this,           NULL, true,                      false);

	int iInterceptorStrength = 0;


	if(GetMaxRangedCombatStrength(this,           NULL, false, false) > 0 && !getDomainType() == DOMAIN_SEA)
	{
		iInterceptorStrength = GetMaxRangedCombatStrength(pAttacker,           NULL, false,                      false);
	}
	else
	{
		iInterceptorStrength = GetMaxDefenseStrength(plot(), pAttacker);
	}

	iInterceptorStrength *= (100 + GetInterceptionCombatModifier());
	iInterceptorStrength /= 100;
#else
	CvCombatInfo kCombatInfo;
	kCombatInfo.setUnit(BATTLE_UNIT_ATTACKER, const_cast<CvUnit*>(pAttacker));
	kCombatInfo.setUnit(BATTLE_UNIT_DEFENDER, const_cast<CvUnit*>(this));
	kCombatInfo.setPlot(plot());
	kCombatInfo.setAttackIsBombingMission(true);
	int iAttackerStrength = pAttacker->GetMaxAttackStrength(kCombatInfo);
	int iInterceptorStrength = GetMaxDefenseStrength(kCombatInfo);
#endif



	int iInterceptorDamageRatio = GC.getMAX_HIT_POINTS() - getDamage();
#ifdef NQM_COMBAT_RNG_USE_BINOM_RNG_OPTION
	int iInterceptorDamage =         GC.getINTERCEPTION_SAME_STRENGTH_MIN_DAMAGE();
#else
	int iInterceptorDamage =         GC.getINTERCEPTION_SAME_STRENGTH_MIN_DAMAGE() * iInterceptorDamageRatio / GC.getMAX_HIT_POINTS();
#endif

	int iInterceptorRoll = 0;
	if(bIncludeRand)
	{
#ifdef NQM_COMBAT_RNG_USE_BINOM_RNG_OPTION
		if (GC.getGame().isOption("GAMEOPTION_USE_BINOM_RNG_FOR_COMBAT_ROLLS"))
		{
			int iAverageDamage = iInterceptorDamage + (GC.getINTERCEPTION_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE() / 2);
			int iSigma = GC.getINTERCEPTION_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE() / 6;
			int iMaxRoll = iSigma*iSigma * 4 + 1;
			iInterceptorRoll = iAverageDamage + GC.getGame().getJonRandNumBinom(iMaxRoll, "Interception Combat Damage") - (iMaxRoll / 2) - iInterceptorDamage;
		}
		else
#endif
		iInterceptorRoll =         GC.getGame().getJonRandNum(GC.getINTERCEPTION_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE(), "Interception Combat Damage");
#ifndef NQM_COMBAT_RNG_USE_BINOM_RNG_OPTION
		iInterceptorRoll *= iInterceptorDamageRatio;
		iInterceptorRoll /= GC.getMAX_HIT_POINTS();
#endif
	}
	else
	{
		iInterceptorRoll =         GC.getINTERCEPTION_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE();
		iInterceptorRoll -= 1;
#ifndef NQM_COMBAT_RNG_USE_BINOM_RNG_OPTION
		iInterceptorRoll *= iInterceptorDamageRatio;
		iInterceptorRoll /= GC.getMAX_HIT_POINTS();
#endif
		iInterceptorRoll /= 2;
	}
	iInterceptorDamage += iInterceptorRoll;
#ifdef NQM_COMBAT_RNG_USE_BINOM_RNG_OPTION
	iInterceptorDamage = MAX(1, MIN(iInterceptorDamage, GC.getMAX_HIT_POINTS() * 100)) * iInterceptorDamageRatio / GetMaxHitPoints();
#endif

	double fStrengthRatio = (double(iInterceptorStrength) / iAttackerStrength);


	if(iAttackerStrength > iInterceptorStrength)
	{
		fStrengthRatio = (double(iAttackerStrength) / iInterceptorStrength);
	}

	fStrengthRatio = (fStrengthRatio + 3) / 4;
	fStrengthRatio = pow(fStrengthRatio, 4.0);
	fStrengthRatio = (fStrengthRatio + 1) / 2;

	if(iAttackerStrength > iInterceptorStrength)
	{
		fStrengthRatio = 1 / fStrengthRatio;
	}

	iInterceptorDamage = int(iInterceptorDamage * fStrengthRatio);


#ifdef AUI_WARNING_FIXES
	if (pAttacker)
	{
		iInterceptorDamage *= (100 + pAttacker->GetInterceptionDefenseDamageModifier());
		iInterceptorDamage /= 100;
	}
#else
	iInterceptorDamage *= (100 + pAttacker->GetInterceptionDefenseDamageModifier());
	iInterceptorDamage /= 100;
#endif


	iInterceptorDamage /= 100;
	
	iInterceptorDamage = max(1,iInterceptorDamage);
	iInterceptorDamage = min(99,iInterceptorDamage);

	return iInterceptorDamage;
}


int CvUnit::GetCombatLimit() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->GetCombatLimit();
}



int CvUnit::GetRangedCombatLimit() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->GetRangedCombatLimit();
}



bool CvUnit::isWaiting() const
{
	VALIDATE_OBJECT

	ActivityTypes eActivityType = GetActivityType();

	return ((eActivityType == ACTIVITY_HOLD)||
	        (eActivityType == ACTIVITY_SLEEP)   ||
	        (eActivityType == ACTIVITY_HEAL)    ||
	        (eActivityType == ACTIVITY_SENTRY)  ||
	        (eActivityType == ACTIVITY_INTERCEPT));
}



bool CvUnit::isFortifyable(bool bCanWaitForNextTurn) const
{
	VALIDATE_OBJECT

	if(!bCanWaitForNextTurn)
	{
		if(hasMoved())
		{
			return false;
		}
	}

	if(!IsEverFortifyable())
	{
		return false;
	}

	return true;
}



bool CvUnit::IsEverFortifyable() const
{
	VALIDATE_OBJECT

	if(!IsCombatUnit() || noDefensiveBonus() || ((getDomainType() != DOMAIN_LAND) && (getDomainType() != DOMAIN_IMMOBILE)))
	{
		return false;
	}

	return true;
}


int CvUnit::fortifyModifier() const
{
	VALIDATE_OBJECT
	int iValue = 0;
	int iTurnsFortified = getFortifyTurns();
	if(iTurnsFortified > 0)
	{
		iValue = iTurnsFortified * GC.getFORTIFY_MODIFIER_PER_TURN();
	

		int iMod = GET_PLAYER(getOwner()).getUnitFortificationModifier();
		if(iMod != 0)
		{
			iValue = ((100 + iMod) / 100) * iValue;
		}

		if(iValue < 0)
		{
			CvAssertMsg(false, "Calculated a negative combat mod for a fortified unit. Resetting to 0 instead.");
			iValue = 0;
		}
	}

	return iValue;
}



int CvUnit::experienceNeeded() const
{
	VALIDATE_OBJECT

	const int iLevel = getLevel();


	int iExperienceMultiplier = 0;
	for(int iLevelLoop = 1; iLevelLoop <= iLevel; iLevelLoop++)
	{
		iExperienceMultiplier += iLevelLoop;
	}

	int iExperienceNeeded =        GC.getEXPERIENCE_PER_LEVEL() * iExperienceMultiplier;

	const int iModifier = GET_PLAYER(getOwner()).getLevelExperienceModifier();
	if(0 != iModifier)
	{
		float fTemp = (float) iExperienceNeeded;
		fTemp *= (100 + iModifier);
		fTemp /= 100;
		iExperienceNeeded = (int) ceil(fTemp);
	}

	return iExperienceNeeded;
}



int CvUnit::attackXPValue() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->GetXPValueAttack();
}



int CvUnit::defenseXPValue() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->GetXPValueDefense();
}



int CvUnit::maxXPValue() const
{
	VALIDATE_OBJECT
	int iMaxValue;

	iMaxValue = INT_MAX;

	if(isBarbarian() || GET_PLAYER(getOwner()).isMinorCiv())
	{
		iMaxValue = std::min(iMaxValue, GC.getBARBARIAN_MAX_XP_VALUE());
	}
#if defined(LEKMOD_AI_XP_CAP)
	if (GC.getGame().isOption("GAMEOPTION_AI_XP_CAP"))
	{
		if (!GET_PLAYER(getOwner()).isHuman())
		{
			iMaxValue = std::min(iMaxValue, GC.getBARBARIAN_MAX_XP_VALUE());
		}
	}
#endif
	return iMaxValue;
}
#if defined(NQ_NO_GG_POINTS_FROM_CS_OR_BARBS)



bool CvUnit::canEarnGlobalXP() const
{
	VALIDATE_OBJECT
	CvPlayer& kPlayer = GET_PLAYER(getOwner());
	if (isBarbarian() || kPlayer.isMinorCiv())
		return false;
#if defined(LEKMOD_AI_XP_CAP)
	if (GC.getGame().isOption("GAMEOPTION_AI_XP_CAP"))
	{
		if (!kPlayer.isHuman())
			return false;
	}
#endif
	return true;
}
#endif


int CvUnit::firstStrikes() const
{
	VALIDATE_OBJECT
	return std::max(0, getExtraFirstStrikes());
}



int CvUnit::chanceFirstStrikes() const
{
	VALIDATE_OBJECT
	return std::max(0, getExtraChanceFirstStrikes());
}



int CvUnit::maxFirstStrikes() const
{
	VALIDATE_OBJECT
	return (firstStrikes() + chanceFirstStrikes());
}



bool CvUnit::isRanged() const
{
	VALIDATE_OBJECT
	return GetBaseRangedCombatStrength() > 0 ? true : false;
}



bool CvUnit::immuneToFirstStrikes() const
{
	VALIDATE_OBJECT
	return (getImmuneToFirstStrikesCount() > 0);
}



bool CvUnit::ignoreBuildingDefense() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->IsIgnoreBuildingDefense();
}



bool CvUnit::ignoreTerrainCost() const
{
	VALIDATE_OBJECT
	return getIgnoreTerrainCostCount() > 0 ? true : false;
}


int CvUnit::getIgnoreTerrainCostCount() const
{
	VALIDATE_OBJECT
	return m_iIgnoreTerrainCostCount;
}


void CvUnit::changeIgnoreTerrainCostCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iIgnoreTerrainCostCount += iValue;
	}
}



const DomainTypes CvUnit::getGiveDomain() const
{
	VALIDATE_OBJECT
	return (DomainTypes)(int)m_eGiveDomain;
}
void CvUnit::ChangeGiveDomain(DomainTypes eDomain)
{
	VALIDATE_OBJECT
	m_eGiveDomain = eDomain;
}

const DomainTypes CvUnit::getConvertDomain() const
{
	VALIDATE_OBJECT
	return (DomainTypes)(int)m_eConvertDomain;
}
void CvUnit::ChangeConvertDomain(DomainTypes eDomain)
{
	VALIDATE_OBJECT
	m_eConvertDomain = eDomain;
}
const UnitTypes CvUnit::getConvertDomainUnitType() const
{
	VALIDATE_OBJECT
	return m_eConvertDomainUnit;
}


void CvUnit::ChangeConvertDomainUnit(UnitTypes eUnit)
{
	VALIDATE_OBJECT
	m_eConvertDomainUnit = eUnit;
}



bool CvUnit::IsRoughTerrainEndsTurn() const
{
	VALIDATE_OBJECT
	return GetRoughTerrainEndsTurnCount() > 0;
}


int CvUnit::GetRoughTerrainEndsTurnCount() const
{
	VALIDATE_OBJECT
	return m_iRoughTerrainEndsTurnCount;
}


void CvUnit::ChangeRoughTerrainEndsTurnCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iRoughTerrainEndsTurnCount += iValue;
	}
}



bool CvUnit::IsHoveringUnit() const
{
	VALIDATE_OBJECT
	return GetHoveringUnitCount() > 0 ? true : false;
}


int CvUnit::GetHoveringUnitCount() const
{
	VALIDATE_OBJECT
	return m_iHoveringUnitCount;
}


void CvUnit::ChangeHoveringUnitCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iHoveringUnitCount += iValue;
	}
}


bool CvUnit::flatMovementCost() const
{
	VALIDATE_OBJECT
	return getFlatMovementCostCount() > 0 ? true : false;
}


int CvUnit::getFlatMovementCostCount() const
{
	VALIDATE_OBJECT
	return m_iFlatMovementCostCount;
}


void CvUnit::changeFlatMovementCostCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iFlatMovementCostCount += iValue;
	}
}


bool CvUnit::canMoveImpassable() const
{
	VALIDATE_OBJECT
	return getCanMoveImpassableCount() > 0 ? true : false;
}


int CvUnit::getCanMoveImpassableCount() const
{
	VALIDATE_OBJECT
	return m_iCanMoveImpassableCount;
}


void CvUnit::changeCanMoveImpassableCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iCanMoveImpassableCount += iValue;
	}
}


void CvUnit::changeCanMoveAllTerrainCount(int iValue)
{
	VALIDATE_OBJECT
	m_iCanMoveAllTerrainCount += iValue;
	CvAssert(getCanMoveAllTerrainCount() >= 0);
}


bool CvUnit::canMoveAllTerrain() const
{
	VALIDATE_OBJECT
	return (getCanMoveAllTerrainCount() > 0);
}


int CvUnit::getCanMoveAllTerrainCount() const
{
	VALIDATE_OBJECT
	return m_iCanMoveAllTerrainCount;
}


void CvUnit::changeCanMoveAfterAttackingCount(int iValue)
{
	VALIDATE_OBJECT
	m_iCanMoveAfterAttackingCount += iValue;
	CvAssert(getCanMoveAfterAttackingCount() >= 0);
}


bool CvUnit::canMoveAfterAttacking() const
{
	VALIDATE_OBJECT


	if(isBlitz())
	{
		return true;
	}

	return (getCanMoveAfterAttackingCount() > 0);
}


int CvUnit::getCanMoveAfterAttackingCount() const
{
	VALIDATE_OBJECT
	return m_iCanMoveAfterAttackingCount;
}


bool CvUnit::hasFreePillageMove() const
{
	VALIDATE_OBJECT
	return getFreePillageMoveCount() > 0 ? true : false;
}


void CvUnit::changeFreePillageMoveCount(int iValue)
{
	VALIDATE_OBJECT
	m_iFreePillageMoveCount += iValue;
	CvAssert(getFreePillageMoveCount() >= 0);
}


int CvUnit::getFreePillageMoveCount() const
{
	VALIDATE_OBJECT
	return m_iFreePillageMoveCount;
}


bool CvUnit::hasHealOnPillage() const
{
	VALIDATE_OBJECT
	return getHealOnPillageCount() > 0 ? true : false;
}


void CvUnit::changeHealOnPillageCount(int iValue)
{
	VALIDATE_OBJECT
	m_iHealOnPillageCount += iValue;
	CvAssert(getHealOnPillageCount() >= 0);
}


int CvUnit::getHealOnPillageCount() const
{
	VALIDATE_OBJECT
	return m_iHealOnPillageCount;
}


int CvUnit::getHPHealedIfDefeatEnemy() const
{
	VALIDATE_OBJECT
	return m_iHPHealedIfDefeatEnemy;
}


void CvUnit::changeHPHealedIfDefeatEnemy(int iValue)
{
	VALIDATE_OBJECT
	m_iHPHealedIfDefeatEnemy += iValue;
	CvAssert(getHPHealedIfDefeatEnemy() >= 0);
}


bool CvUnit::IsHealIfDefeatExcludeBarbarians() const
{
	VALIDATE_OBJECT
	return GetHealIfDefeatExcludeBarbariansCount() > 0 ? true : false;
}


int CvUnit::GetHealIfDefeatExcludeBarbariansCount() const
{
	VALIDATE_OBJECT
	return m_iHealIfDefeatExcludeBarbariansCount;
}


void CvUnit::ChangeHealIfDefeatExcludeBarbariansCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iHealIfDefeatExcludeBarbariansCount += iValue;
	}
}


int CvUnit::GetGoldenAgeValueFromKills() const
{
	VALIDATE_OBJECT
		return m_iGoldenAgeValueFromKills;
}


void CvUnit::ChangeGoldenAgeValueFromKills(int iValue)
{
	VALIDATE_OBJECT
		m_iGoldenAgeValueFromKills += iValue;
	CvAssert(GetGoldenAgeValueFromKills() >= 0);
}


bool CvUnit::isOnlyDefensive() const
{
	VALIDATE_OBJECT
	return getOnlyDefensiveCount() > 0 ? true : false;
}


int CvUnit::getOnlyDefensiveCount() const
{
	VALIDATE_OBJECT
	return m_iOnlyDefensiveCount;
}


void CvUnit::changeOnlyDefensiveCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iOnlyDefensiveCount += iValue;
	}
}


bool CvUnit::noDefensiveBonus() const
{
	VALIDATE_OBJECT

	if(getDomainType() == DOMAIN_SEA)
	{
		return true;
	}

	return getNoDefensiveBonusCount() > 0 ? true : false;
}


int CvUnit::getNoDefensiveBonusCount() const
{
	VALIDATE_OBJECT
	return m_iNoDefensiveBonusCount;
}


void CvUnit::changeNoDefensiveBonusCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iNoDefensiveBonusCount += iValue;
	}
}

#if defined(LEKMOD_NO_FORTIFY_VS_RANGED_PROMO)

bool CvUnit::isNoFortifyVsRanged() const
{
	VALIDATE_OBJECT
	return getNoFortifyVsRangedCount() > 0;
}


int CvUnit::getNoFortifyVsRangedCount() const
{
	VALIDATE_OBJECT
	return m_iNoFortifyVsRangedCount;
}


void CvUnit::changeNoFortifyVsRangedCount(int iValue)
{
	VALIDATE_OBJECT
	if (iValue != 0)
	{
		m_iNoFortifyVsRangedCount += iValue;
	}
}
#endif


bool CvUnit::isNoCapture() const
{
	VALIDATE_OBJECT
	return getNoCaptureCount() > 0 ? true : false;
}


int CvUnit::getNoCaptureCount() const
{
	VALIDATE_OBJECT
	return m_iNoCaptureCount;
}


void CvUnit::changeNoCaptureCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iNoCaptureCount += iValue;
	}
}



bool CvUnit::isNeverInvisible() const
{
	VALIDATE_OBJECT
	return (getInvisibleType() == NO_INVISIBLE);
}



bool CvUnit::isInvisible(TeamTypes eTeam, bool bDebug, bool bCheckCargo) const
{
	VALIDATE_OBJECT
	if(bDebug && GC.getGame().isDebugMode())
	{
		return false;
	}

	if(GET_PLAYER(getOwner()).getTeam() == eTeam)
	{
		return false;
	}

	if(bCheckCargo)
	{
		if((m_transportUnit.eOwner >= 0) && m_transportUnit.eOwner < MAX_PLAYERS)
		{
			if(GET_PLAYER((PlayerTypes)m_transportUnit.eOwner).getUnit(m_transportUnit.iID))
			{
				return true;
			}
		}
	}

	if(m_eInvisibleType == NO_INVISIBLE)
	{
		return false;
	}

	return !(plot()->isInvisibleVisible(eTeam, getInvisibleType()));
}


bool CvUnit::isNukeImmune() const
{
	VALIDATE_OBJECT
	return (getNukeImmuneCount() > 0);
}



void CvUnit::changeNukeImmuneCount(int iValue)
{
	VALIDATE_OBJECT
	m_iNukeImmuneCount += iValue;
	CvAssert(getNukeImmuneCount() >= 0);

}


int CvUnit::getNukeImmuneCount() const
{
	VALIDATE_OBJECT
	return m_iNukeImmuneCount;
}


bool CvUnit::isHiddenNationality() const
{
	VALIDATE_OBJECT
	return (getHiddenNationalityCount() > 0);
}


void CvUnit::changeHiddenNationalityCount(int iValue)
{
	VALIDATE_OBJECT
	m_iHiddenNationalityCount += iValue;
	CvAssert(getHiddenNationalityCount() >= 0);

}


int CvUnit::getHiddenNationalityCount() const
{
	VALIDATE_OBJECT
	return m_iHiddenNationalityCount;
}


bool CvUnit::isNoRevealMap() const
{
	VALIDATE_OBJECT
	return (getNoRevealMapCount() > 0);
}


void CvUnit::changeNoRevealMapCount(int iValue)
{
	VALIDATE_OBJECT
	m_iNoRevealMapCount += iValue;
	CvAssert(getNoRevealMapCount() >= 0);
}


int CvUnit::getNoRevealMapCount() const
{
	VALIDATE_OBJECT
	return m_iNoRevealMapCount;
}


int CvUnit::maxInterceptionProbability() const
{
	VALIDATE_OBJECT
	return std::max(0, getExtraIntercept());
}



int CvUnit::currInterceptionProbability() const
{
	VALIDATE_OBJECT
	if(getDomainType() != DOMAIN_AIR)
	{
		return maxInterceptionProbability();
	}
	else
	{
		return ((maxInterceptionProbability() * GetCurrHitPoints()) / GetMaxHitPoints());
	}
}



int CvUnit::evasionProbability() const
{
	VALIDATE_OBJECT
	return std::max(0, getExtraEvasion());
}



int CvUnit::withdrawalProbability() const
{
	VALIDATE_OBJECT
	return std::max(0, getExtraWithdrawal());
}



int CvUnit::GetNumEnemyUnitsAdjacent(const CvUnit* pUnitToExclude) const
{
	int iNumEnemiesAdjacent = 0;

	TeamTypes eMyTeam = getTeam();

	CvPlot* pLoopPlot;
	IDInfo* pUnitNode;

	CvUnit* pLoopUnit;
	TeamTypes eTheirTeam;

	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pLoopPlot != NULL)
		{
			pUnitNode = pLoopPlot->headUnitNode();


			while(pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(*pUnitNode);
				pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);


				if(pLoopUnit && pLoopUnit != pUnitToExclude)
				{

					if(pLoopUnit->IsCombatUnit() && !pLoopUnit->isEmbarked())
					{
						eTheirTeam = pLoopUnit->getTeam();


						if(GET_TEAM(eTheirTeam).isAtWar(eMyTeam))
						{

							if (pLoopUnit->getDomainType() == getDomainType())
							{
								iNumEnemiesAdjacent++;
							}
						}
					}
				}
			}
		}
	}

	return iNumEnemiesAdjacent;
}



bool CvUnit::IsEnemyCityAdjacent() const
{
	TeamTypes eMyTeam = getTeam();
	CvPlot* pLoopPlot;
	CvCity* pCity;

	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
		if(!pLoopPlot)
		{
			continue;
		}

		pCity = pLoopPlot->getPlotCity();
		if(!pCity)
		{
			continue;
		}

		if(GET_TEAM(eMyTeam).isAtWar(pCity->getTeam()))
		{
			return true;
		}
	}

	return false;
}



bool CvUnit::IsEnemyCityAdjacent(const CvCity* pSpecifyCity) const
{
	CvAssertMsg(pSpecifyCity, "City is NULL when checking if it is adjacent to a unit");
	if (!pSpecifyCity)
	{
		return false;
	}

	TeamTypes eMyTeam = getTeam();
	CvPlot* pLoopPlot;
	CvCity* pCity;

	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
		if(!pLoopPlot)
		{
			continue;
		}

		pCity = pLoopPlot->getPlotCity();
		if(!pCity)
		{
			continue;
		}

		if (pCity->getX() == pSpecifyCity->getX() && pCity->getY() == pSpecifyCity->getY())
		{
			if(GET_TEAM(eMyTeam).isAtWar(pCity->getTeam()))
			{
				return true;
			}
		}
	}

	return false;
}


int CvUnit::GetNumSpecificEnemyUnitsAdjacent(const CvUnit* pUnitToExclude, const CvUnit* pUnitCompare) const
{
	int iNumEnemiesAdjacent = 0;

	TeamTypes eMyTeam = getTeam();

	CvPlot* pLoopPlot;
	IDInfo* pUnitNode;

	CvUnit* pLoopUnit;
	TeamTypes eTheirTeam;

	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pLoopPlot != NULL)
		{
			pUnitNode = pLoopPlot->headUnitNode();


			while(pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(*pUnitNode);
				pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);


				if(pLoopUnit && pLoopUnit != pUnitToExclude)
				{

					if(pLoopUnit->IsCombatUnit())
					{
						eTheirTeam = pLoopUnit->getTeam();


						if(GET_TEAM(eTheirTeam).isAtWar(eMyTeam))
						{

							if(pLoopUnit->getUnitType() == pUnitCompare->getUnitType())
							{
								iNumEnemiesAdjacent++;
							}
						}
					}
				}
			}
		}
	}

	return iNumEnemiesAdjacent;
}



bool CvUnit::IsFriendlyUnitAdjacent(bool bCombatUnit) const
{
	CvPlot* pLoopPlot;
	IDInfo* pUnitNode;

	CvUnit* pLoopUnit;

	CvPlot* pPlot = plot();

	TeamTypes eTeam = getTeam();

	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pLoopPlot != NULL)
		{

			if(pLoopPlot->getArea() == pPlot->getArea())
			{
				pUnitNode = pLoopPlot->headUnitNode();

				while(pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(*pUnitNode);
					pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

					if(pLoopUnit && pLoopUnit->getTeam() == eTeam)
					{

						if(!bCombatUnit || pLoopUnit->IsCombatUnit())
						{
#ifdef NQ_ADJACENT_MOD_REQUIRES_SAME_COMBAT_TYPE
							if (pLoopUnit->getUnitCombatType() == getUnitCombatType())
								return true;
#else
							return true;
#endif
						}
					}
				}
			}
		}
	}

	return false;
}



int CvUnit::GetAdjacentModifier() const
{
	VALIDATE_OBJECT
	return m_iAdjacentModifier;
}


void CvUnit::ChangeAdjacentModifier(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iAdjacentModifier += iValue;
	}
}



int CvUnit::GetRangedAttackModifier() const
{
	VALIDATE_OBJECT
	return m_iRangedAttackModifier;
}


void CvUnit::ChangeRangedAttackModifier(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iRangedAttackModifier += iValue;
	}
}



int CvUnit::GetInterceptionCombatModifier() const
{
	VALIDATE_OBJECT
	return m_iInterceptionCombatModifier;
}


void CvUnit::ChangeInterceptionCombatModifier(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iInterceptionCombatModifier += iValue;
	}
}



int CvUnit::GetInterceptionDefenseDamageModifier() const
{
	VALIDATE_OBJECT
	return m_iInterceptionDefenseDamageModifier;
}


void CvUnit::ChangeInterceptionDefenseDamageModifier(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iInterceptionDefenseDamageModifier += iValue;
	}
}



int CvUnit::GetAirSweepCombatModifier() const
{
	VALIDATE_OBJECT
	return m_iAirSweepCombatModifier;
}


void CvUnit::ChangeAirSweepCombatModifier(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iAirSweepCombatModifier += iValue;
	}
}


int CvUnit::getAttackModifier() const
{
	VALIDATE_OBJECT
	return m_iAttackModifier;
}


void CvUnit::changeAttackModifier(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iAttackModifier += iValue;
	}
}



int CvUnit::getDefenseModifier() const
{
	VALIDATE_OBJECT
	return m_iDefenseModifier;
}


void CvUnit::changeDefenseModifier(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iDefenseModifier += iValue;
	}
}


int CvUnit::cityAttackModifier() const
{
	VALIDATE_OBJECT
	return (getExtraCityAttackPercent());
}



int CvUnit::cityDefenseModifier() const
{
	VALIDATE_OBJECT
	return (getExtraCityDefensePercent());
}



int CvUnit::rangedDefenseModifier() const
{
	VALIDATE_OBJECT
	return (getExtraRangedDefenseModifier());
}



int CvUnit::hillsAttackModifier() const
{
	VALIDATE_OBJECT
	return (getExtraHillsAttackPercent());
}

#ifdef NQ_HEAVY_CHARGE_DOWNHILL

int CvUnit::heavyChargeDownhillModifier() const
{
	VALIDATE_OBJECT
	return (GetHeavyChargeDownhill());
}
#endif

#ifdef LEKMOD_DIFFERENT_IDEO_COMBAT_BONUS

int CvUnit::combatBonusVsDifferentIdeologyModifier() const
{
	VALIDATE_OBJECT
	return (GetCombatBonusVsDifferentIdeologyModifier());
}
#endif


int CvUnit::hillsDefenseModifier() const
{
	VALIDATE_OBJECT
	return (getExtraHillsDefensePercent());
}


int CvUnit::openAttackModifier() const
{
	VALIDATE_OBJECT
	return (getExtraOpenAttackPercent());
}


int CvUnit::openRangedAttackModifier() const
{
	VALIDATE_OBJECT
	return (getExtraOpenRangedAttackMod());
}


int CvUnit::roughAttackModifier() const
{
	VALIDATE_OBJECT
	return (getExtraRoughAttackPercent());
}


int CvUnit::roughRangedAttackModifier() const
{
	VALIDATE_OBJECT
	return (getExtraRoughRangedAttackMod());
}


int CvUnit::attackFortifiedModifier() const
{
	VALIDATE_OBJECT
	return (getExtraAttackFortifiedMod());
}


int CvUnit::attackWoundedModifier() const
{
	VALIDATE_OBJECT
	return (getExtraAttackWoundedMod());
}


int CvUnit::openDefenseModifier() const
{
	VALIDATE_OBJECT
	return (getExtraOpenDefensePercent());
}


int CvUnit::roughDefenseModifier() const
{
	VALIDATE_OBJECT
	return (getExtraRoughDefensePercent());
}


int CvUnit::terrainAttackModifier(TerrainTypes eTerrain) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eTerrain >= 0, "eTerrain is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTerrain < GC.getNumTerrainInfos(), "eTerrain is expected to be within maximum bounds (invalid Index)");
	return (getExtraTerrainAttackPercent(eTerrain));
}



int CvUnit::terrainDefenseModifier(TerrainTypes eTerrain) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eTerrain >= 0, "eTerrain is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTerrain < GC.getNumTerrainInfos(), "eTerrain is expected to be within maximum bounds (invalid Index)");
	return (getExtraTerrainDefensePercent(eTerrain));
}



int CvUnit::featureAttackModifier(FeatureTypes eFeature) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eFeature >= 0, "eFeature is expected to be non-negative (invalid Index)");
	CvAssertMsg(eFeature < GC.getNumFeatureInfos(), "eFeature is expected to be within maximum bounds (invalid Index)");
	return (getExtraFeatureAttackPercent(eFeature));
}


int CvUnit::featureDefenseModifier(FeatureTypes eFeature) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eFeature >= 0, "eFeature is expected to be non-negative (invalid Index)");
	CvAssertMsg(eFeature < GC.getNumFeatureInfos(), "eFeature is expected to be within maximum bounds (invalid Index)");
	return (getExtraFeatureDefensePercent(eFeature));
}


int CvUnit::unitClassAttackModifier(UnitClassTypes eUnitClass) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eUnitClass >= 0, "eUnitClass is expected to be non-negative (invalid Index)");
	CvAssertMsg(eUnitClass < GC.getNumUnitClassInfos(), "eUnitClass is expected to be within maximum bounds (invalid Index)");
	return m_Promotions.GetUnitClassAttackMod(eUnitClass);
}



int CvUnit::unitClassDefenseModifier(UnitClassTypes eUnitClass) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eUnitClass >= 0, "eUnitClass is expected to be non-negative (invalid Index)");
	CvAssertMsg(eUnitClass < GC.getNumUnitClassInfos(), "eUnitClass is expected to be within maximum bounds (invalid Index)");
	return m_Promotions.GetUnitClassDefenseMod(eUnitClass);
}



int CvUnit::unitCombatModifier(UnitCombatTypes eUnitCombat) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eUnitCombat >= 0, "eUnitCombat is expected to be non-negative (invalid Index)");
	CvAssertMsg(eUnitCombat < GC.getNumUnitCombatClassInfos(), "eUnitCombat is expected to be within maximum bounds (invalid Index)");
	return (getExtraUnitCombatModifier(eUnitCombat));
}



int CvUnit::domainModifier(DomainTypes eDomain) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eDomain >= 0, "eDomain is expected to be non-negative (invalid Index)");
	CvAssertMsg(eDomain < NUM_DOMAIN_TYPES, "eDomain is expected to be within maximum bounds (invalid Index)");
	return (getExtraDomainModifier(eDomain));
}

#if defined(LEKMOD_DOMAIN_PROMO_ATTACK_DEFENSE)

int CvUnit::domainAttackModifier(DomainTypes eDomain) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eDomain >= 0, "eDomain is expected to be non-negative (invalid Index)");
	CvAssertMsg(eDomain < NUM_DOMAIN_TYPES, "eDomain is expected to be within maximum bounds (invalid Index)");
	return m_Promotions.GetDomainAttackMod(eDomain);
}


int CvUnit::domainDefenseModifier(DomainTypes eDomain) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eDomain >= 0, "eDomain is expected to be non-negative (invalid Index)");
	CvAssertMsg(eDomain < NUM_DOMAIN_TYPES, "eDomain is expected to be within maximum bounds (invalid Index)");
	return m_Promotions.GetDomainDefenseMod(eDomain);
}
#endif



SpecialUnitTypes CvUnit::specialCargo() const
{
	VALIDATE_OBJECT
	return ((SpecialUnitTypes)(m_pUnitInfo->GetSpecialCargo()));
}



DomainTypes CvUnit::domainCargo() const
{
	VALIDATE_OBJECT
	return ((DomainTypes)(m_pUnitInfo->GetDomainCargo()));
}



int CvUnit::cargoSpace() const
{
	VALIDATE_OBJECT
	return m_iCargoCapacity;
}


void CvUnit::changeCargoSpace(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iCargoCapacity += iChange;
		CvAssert(m_iCargoCapacity >= 0);
		setInfoBarDirty(true);
	}
}


bool CvUnit::isFull() const
{
	VALIDATE_OBJECT
	return (getCargo() >= cargoSpace());
}



int CvUnit::cargoSpaceAvailable(SpecialUnitTypes eSpecialCargo, DomainTypes eDomainCargo) const
{
	VALIDATE_OBJECT
	if(specialCargo() != NO_SPECIALUNIT)
	{
		if(specialCargo() != eSpecialCargo)
		{
			return 0;
		}
	}

	if(domainCargo() != NO_DOMAIN)
	{
		if(domainCargo() != eDomainCargo)
		{
			return 0;
		}
	}

	return std::max(0, (cargoSpace() - getCargo()));
}



bool CvUnit::hasCargo() const
{
	VALIDATE_OBJECT
	return (getCargo() > 0);
}



bool CvUnit::canCargoAllMove() const
{
	VALIDATE_OBJECT
	const IDInfo* pUnitNode;
	const CvUnit* pLoopUnit;
	CvPlot* pPlot;

	pPlot = plot();

	pUnitNode = pPlot->headUnitNode();

	while(pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if(pLoopUnit && pLoopUnit->getTransportUnit() == this)
		{
			if(pLoopUnit->getDomainType() == DOMAIN_LAND)
			{
				if(!(pLoopUnit->canMove()))
				{
					return false;
				}
			}
		}
	}

	return true;
}



int CvUnit::getUnitAICargo(UnitAITypes eUnitAI) const
{
	VALIDATE_OBJECT
	const IDInfo* pUnitNode;
	const CvUnit* pLoopUnit;
	CvPlot* pPlot;
	int iCount;

	iCount = 0;

	pPlot = plot();

	pUnitNode = pPlot->headUnitNode();

	while(pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if(pLoopUnit && pLoopUnit->getTransportUnit() == this)
		{
			if(pLoopUnit->AI_getUnitAIType() == eUnitAI)
			{
				iCount++;
			}
		}
	}

	return iCount;
}


bool CvUnit::IsHasNoValidMove() const
{
	if(plot()->getNumFriendlyUnitsOfType(this) <= GC.getPLOT_UNIT_LIMIT())
	{
		return false;
	}

	CvTwoLayerPathFinder& thePathfinder = GC.getPathFinder();


	thePathfinder.SetDestValidFunc(NULL);
	thePathfinder.SetIsPathDestFunc(FindValidDestinationDest);
	thePathfinder.SetValidFunc(FindValidDestinationPathValid);


	bool bCanFindPath = thePathfinder.GenerateUnitPath(this, getX(), getY(), -1, -1, 0, false);


	thePathfinder.SetDestValidFunc(PathDestValid);
	thePathfinder.SetIsPathDestFunc(PathDest);
	thePathfinder.SetValidFunc(PathValid);
	thePathfinder.ForceReset();

	return !bCanFindPath;
}


int CvUnit::getIndex() const
{
	VALIDATE_OBJECT
	return (GetID() & FLTA_INDEX_MASK);
}



IDInfo CvUnit::GetIDInfo() const
{
	VALIDATE_OBJECT
	IDInfo unit(getOwner(), GetID());
	return unit;
}



void CvUnit::SetID(int iID)
{
	VALIDATE_OBJECT
	m_iID = iID;
}



int CvUnit::getHotKeyNumber()
{
	VALIDATE_OBJECT
	return m_iHotKeyNumber;
}



void CvUnit::setHotKeyNumber(int iNewValue)
{
	VALIDATE_OBJECT
	CvUnit* pLoopUnit;
	int iLoop;

	CvAssert(getOwner() != NO_PLAYER);

	if(getHotKeyNumber() != iNewValue)
	{
		if(iNewValue != -1)
		{
			for(pLoopUnit = GET_PLAYER(getOwner()).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(getOwner()).nextUnit(&iLoop))
			{
				if(pLoopUnit->getHotKeyNumber() == iNewValue)
				{
					pLoopUnit->setHotKeyNumber(-1);
				}
			}
		}

		m_iHotKeyNumber = iNewValue;
	}
}


void CvUnit::setXY(int iX, int iY, bool bGroup, bool bUpdate, bool bShow, bool bCheckPlotVisible, bool bNoMove)
{
	VALIDATE_OBJECT
#ifdef UPDATE_MINOR_BG_ICON_ON_UNIT_MOVE_OR_SET_DAMAGE
	std::vector<bool> oldCanBully(MAX_MAJOR_CIVS * (MAX_MINOR_CIVS - MAX_MAJOR_CIVS));
	for (int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		for (int jJ = MAX_MAJOR_CIVS; jJ < MAX_MINOR_CIVS; jJ++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				oldCanBully[iI * (MAX_MINOR_CIVS - MAX_MAJOR_CIVS) + jJ - MAX_MAJOR_CIVS] = GET_PLAYER((PlayerTypes)jJ).GetMinorCivAI()->CanMajorBullyGold((PlayerTypes)iI);
			}
			else
			{
				oldCanBully[iI * (MAX_MINOR_CIVS - MAX_MAJOR_CIVS) + jJ - MAX_MAJOR_CIVS] = false;
			}
		}
	}
#endif
	IDInfo* pUnitNode = 0;
	CvCity* pOldCity = 0;
	CvCity* pNewCity = 0;
	CvUnit* pTransportUnit = 0;
	CvUnit* pLoopUnit = 0;
	CvPlot* pOldPlot = 0;
	CvPlot* pNewPlot = 0;
	FStaticVector<IDInfo, 10, true, c_eCiv5GameplayDLL, 0> oldUnitList;
	FStaticVector<CvUnitCaptureDefinition, 8, true, c_eCiv5GameplayDLL, 0> kCaptureUnitList;
	ActivityTypes eOldActivityType = NO_ACTIVITY;
	int iI;
	TeamTypes activeTeam = GC.getGame().getActiveTeam();
	TeamTypes eOurTeam = getTeam();

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	bool bOwnerIsActivePlayer = GC.getGame().getActivePlayer() == getOwner();

	if (bOwnerIsActivePlayer)
		DLLUI->SetDontShowPopups(true);

	CvAssert(!at(iX, iY));
	CvAssert(!isFighting());
	CvAssert((iX == INVALID_PLOT_COORD) || (GC.getMap().plot(iX, iY)->getX() == iX));
	CvAssert((iY == INVALID_PLOT_COORD) || (GC.getMap().plot(iX, iY)->getY() == iY));

	eOldActivityType = GetActivityType();

	CvCity *pkPrevGarrisonedCity = GetGarrisonedCity();

	if(isSetUpForRangedAttack())
	{
		setSetUpForRangedAttack(false);
	}

	if(!bGroup || isCargo())
	{
		bShow = false;
	}

	int iMapLayer = m_iMapLayer;

	pNewPlot = GC.getMap().plot(iX, iY);

	if(pNewPlot != NULL && !bNoMove)
	{
		pTransportUnit = getTransportUnit();

		if(pTransportUnit != NULL)
		{
			if(!(pTransportUnit->atPlot(*pNewPlot)))
			{
				setTransportUnit(NULL);
			}
		}

		if(IsCombatUnit())
		{
			oldUnitList.clear();

			pUnitNode = pNewPlot->headUnitNode();

			while(pUnitNode != NULL)
			{
				oldUnitList.push_back(*pUnitNode);
				pUnitNode = pNewPlot->nextUnitNode(pUnitNode);
			}

			int iUnitListSize = (int) oldUnitList.size();
			for(int iVectorLoop = 0; iVectorLoop < (int) iUnitListSize; ++iVectorLoop)
			{
				pLoopUnit = ::getUnit(oldUnitList[iVectorLoop]);
				{
					if(pLoopUnit && !pLoopUnit->isDelayedDeath())
					{
						if(isEnemy(pLoopUnit->getTeam(), pNewPlot) || pLoopUnit->isEnemy(eOurTeam))
						{
							if(!pLoopUnit->canCoexistWithEnemyUnit(eOurTeam))
							{

								if(NO_UNITCLASS == pLoopUnit->getUnitInfo().GetUnitCaptureClassType() && pLoopUnit->IsCanDefend(pNewPlot))
								{
									if(!pNewPlot->isCity())
									{
										if (!pLoopUnit->jumpToNearestValidPlot())
											pLoopUnit->kill(false, getOwner());
									}
									else
									{
#if defined(FULL_YIELD_FROM_KILLS)
										kPlayer.DoYieldsFromKill(this, pLoopUnit, iX, iY, pLoopUnit->isBarbarian(), 0);
#else
										kPlayer.DoYieldsFromKill(getUnitType(), pLoopUnit->getUnitType(), iX, iY, pLoopUnit->isBarbarian(), 0);
#endif
										pLoopUnit->kill(false, getOwner());
									}
								}

								else
								{
									bool bDisplaced = false;
									ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
									if(pkScriptSystem)
									{
										CvLuaArgsHandle args;
										args->Push(pLoopUnit->getOwner());
										args->Push(pLoopUnit->GetID());

										bool bResult = false;
										if(LuaSupport::CallTestAll(pkScriptSystem, "CanDisplaceCivilian", args.get(), bResult))
										{

											if(bResult)
											{
												bDisplaced = true;
											}
										}
									}

									if (!bDisplaced)
									{
										Localization::String strMessage;
										Localization::String strSummary;

										bool bDoCapture = false;



#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)


										const bool bCaptureDespiteEmbark =
											pLoopUnit->isEmbarked() && pNewPlot != NULL && pNewPlot->IsAllowsWalkWater();
										if((!pLoopUnit->isEmbarked() || pLoopUnit->getUnitInfo().IsCaptureWhileEmbarked() || bCaptureDespiteEmbark) && pLoopUnit->getCaptureUnitType(GET_PLAYER(pLoopUnit->getOwner()).getCivilizationType()) != NO_UNIT)
#else
										if((!pLoopUnit->isEmbarked() || pLoopUnit->getUnitInfo().IsCaptureWhileEmbarked()) && pLoopUnit->getCaptureUnitType(GET_PLAYER(pLoopUnit->getOwner()).getCivilizationType()) != NO_UNIT)
#endif
										{
											bDoCapture = true;

											if(isBarbarian())
											{
												strMessage = Localization::Lookup("TXT_KEY_UNIT_CAPTURED_BARBS_DETAILED");
												strMessage << pLoopUnit->getUnitInfo().GetTextKey();
												strSummary = Localization::Lookup("TXT_KEY_UNIT_CAPTURED_BARBS");
											}
											else
											{
												strMessage = Localization::Lookup("TXT_KEY_UNIT_CAPTURED_DETAILED");
												strMessage << pLoopUnit->getUnitInfo().GetTextKey() << GET_PLAYER(getOwner()).getNameKey();
												strSummary = Localization::Lookup("TXT_KEY_UNIT_CAPTURED");
											}
										}

										else
										{
#ifndef NO_EMBARKED_CIVILIAN_DEFENSE
											if(pLoopUnit->isEmbarked())
												changeExperience(1);
#endif

											CvString strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_DESTROYED_ENEMY", getNameKey(), 0, pLoopUnit->getNameKey());
											DLLUI->AddUnitMessage(0, GetIDInfo(), getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer                                                                                                                                                                                                       );

											strMessage = Localization::Lookup("TXT_KEY_UNIT_LOST");
											strSummary = strMessage;

#if defined(FULL_YIELD_FROM_KILLS)
											kPlayer.DoYieldsFromKill(this, pLoopUnit, iX, iY, pLoopUnit->isBarbarian(), 0);
#else
											kPlayer.DoYieldsFromKill(getUnitType(), pLoopUnit->getUnitType(), iX, iY, pLoopUnit->isBarbarian(), 0);
#endif
											kPlayer.DoUnitKilledCombat(pLoopUnit->getOwner(), pLoopUnit->getUnitType());
										}

										CvNotifications* pNotification = GET_PLAYER(pLoopUnit->getOwner()).GetNotifications();
										if(pNotification)
											pNotification->Add(NOTIFICATION_UNIT_DIED, strMessage.toUTF8(), strSummary.toUTF8(), pLoopUnit->getX(), pLoopUnit->getY(), (int) pLoopUnit->getUnitType(), pLoopUnit->getOwner());
#ifndef NO_EMBARKED_CIVILIAN_DEFENSE
										if(pLoopUnit->isEmbarked())
											setMadeAttack(true);
#endif



										CvUnitCaptureDefinition kCaptureDef;
										if(bDoCapture)
										{
											if(pLoopUnit->getCaptureDefinition(&kCaptureDef, getOwner()))
												kCaptureUnitList.push_back(kCaptureDef);
											pLoopUnit->setCapturingPlayer(NO_PLAYER);
										}

										pLoopUnit->kill(false, getOwner());
									}
								}
							}
						}
					}
				}
			}
		}
	}

	pOldPlot = plot();

	if(pOldPlot != NULL)
	{
		pOldPlot->removeUnit(this, bUpdate);

		if (iMapLayer == DEFAULT_UNIT_MAP_LAYER)
		{

			if(pOldPlot->getPlotCity())
			{


				auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
				gDLL->GameplayUnitVisibility(pDllUnit.get(), pNewPlot != NULL && !this->isInvisible(activeTeam, false));

				if (GetBaseCombatStrength(true                   ) > 0 && getDomainType() == DOMAIN_LAND)
				{
#ifndef FIX_POLICY_CULTURE_PER_GARRISONED_UNIT
					pOldPlot->getPlotCity()->ChangeJONSCulturePerTurnFromPolicies(-(GET_PLAYER(getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURE_FROM_GARRISON)));
#endif
				}
			}

			if (canChangeVisibility())
				pOldPlot->changeAdjacentSight(eOurTeam, visibilityRange(), false, getSeeInvisibleType(), getFacingDirection(true));

			pOldPlot->area()->changeUnitsPerPlayer(getOwner(), -1);

			setLastMoveTurn(GC.getGame().getTurnSlice());

			pOldCity = pOldPlot->getPlotCity();
		}
	}

	if(pNewPlot != NULL)
	{
		m_iX = pNewPlot->getX();
		m_iY = pNewPlot->getY();
	}
	else
	{
		m_iX = INVALID_PLOT_COORD;
		m_iY = INVALID_PLOT_COORD;
	}

	CvAssertMsg(plot() == pNewPlot, "plot is expected to equal pNewPlot");

	if(pNewPlot != NULL)
	{

		if(pOldPlot != NULL)
		{
			DirectionTypes newDirection = directionXY(pOldPlot, pNewPlot);
			if(newDirection != NO_DIRECTION)
				m_eFacingDirection = newDirection;
		}


		if(isCargo())
		{
			if(eOldActivityType != ACTIVITY_MISSION)
			{
				SetActivityType(eOldActivityType);
			}
		}


		if(pNewPlot->getPlotCity() && iMapLayer == DEFAULT_UNIT_MAP_LAYER)
		{
			auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
			gDLL->GameplayUnitVisibility(pDllUnit.get(), false             );
			
			if (GetBaseCombatStrength(true                   ) > 0 && getDomainType() == DOMAIN_LAND)
			{
#ifndef FIX_POLICY_CULTURE_PER_GARRISONED_UNIT
				pNewPlot->getPlotCity()->ChangeJONSCulturePerTurnFromPolicies((GET_PLAYER(getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURE_FROM_GARRISON)));
#endif
			}
		}

		setFortifyTurns(0);

		if (iMapLayer == DEFAULT_UNIT_MAP_LAYER)
		{
			if (canChangeVisibility())
				pNewPlot->changeAdjacentSight(eOurTeam, visibilityRange(), true, getSeeInvisibleType(), getFacingDirection(true));

			pNewPlot->addUnit(this, bUpdate);

			pNewPlot->area()->changeUnitsPerPlayer(getOwner(), 1);
			pNewCity = pNewPlot->getPlotCity();
		}
		else
		{
			GC.getMap().plotManager().AddUnit(GetIDInfo(), iX, iY, iMapLayer);
		}


		if(pNewCity != NULL)
		{
			pNewCity->updateStrengthValue();

			if(isEnemy(pNewCity->getTeam()) && !canCoexistWithEnemyUnit(pNewCity->getTeam()))
			{
				PlayerTypes eNewOwner = GET_PLAYER(getOwner()).pickConqueredCityOwner(*pNewCity);

				if(NO_PLAYER != eNewOwner)
				{
					

					if(getOwner() == GC.getGame().getActivePlayer())
					{

						if(strcmp(getUnitInfo().GetType(), "UNIT_WWI_TANK") == 0)
						{
							const int numBuildings = GC.getNumBuildingInfos();
							for(int i = 0; i < numBuildings; ++i)
							{
								const BuildingTypes eBuilding = static_cast<BuildingTypes>(i);
								const CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

								if(pkBuildingInfo != NULL && strcmp(pkBuildingInfo->GetType(), "BUILDING_PETRA") == 0)
								{
									if(pNewCity->GetCityBuildings()->GetNumRealBuilding(eBuilding) > 0)
									{
										gDLL->UnlockAchievement(ACHIEVEMENT_XP1_26);
									}
									break;
								}
							}
						}


						if(strcmp(getUnitInfo().GetType(), "UNIT_DUTCH_SEA_BEGGAR") == 0)
						{
							if(pNewCity->isCoastal())
							{
								if(strcmp(pNewCity->getCivilizationInfo().GetType(), "CIVILIZATION_SPAIN") == 0)
								{
									gDLL->UnlockAchievement(ACHIEVEMENT_XP1_28);
								}
							}
						}





						if (!GC.getGame().isGameMultiPlayer())
						{
							if(strcmp(getCivilizationInfo().GetType(), "CIVILIZATION_VENICE") == 0)
							{
								CvCity* pCity = GET_PLAYER(m_eOwner).getCapitalCity();
								ReligionTypes eReligion = pCity->GetCityReligions()->GetReligiousMajority();
								if (eReligion != NO_RELIGION)
								{
									if (pNewCity->GetCityReligions()->IsHolyCityForReligion(eReligion))
									{
										gDLL->UnlockAchievement(ACHIEVEMENT_XP2_26);
									}
								}
							}
						}
					}

					bool bUsingXP1Scenario2 = gDLL->IsModActivated(CIV5_XP1_SCENARIO2_MODID);
					if(bUsingXP1Scenario2)
					{
						if(strcmp(getCivilizationInfo().GetType(), "CIVILIZATION_SONGHAI") == 0)
						{
							const int iRomeX = 33;
							const int iRomeY = 31;


							if(pNewCity->getX() == iRomeX && pNewCity->getY() == iRomeY && getDomainType() == DOMAIN_SEA)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP1_52);
							}
						}
					}
					

					

					GET_PLAYER(eNewOwner).acquireCity(pNewCity, true, false);
					pNewCity = NULL;
				}
			}
		}

		if(pOldPlot != NULL)
		{
			if(pOldCity != NULL)
			{
				pOldCity->updateStrengthValue();
			}
		}

		if(shouldLoadOnMove(pNewPlot))
		{
			load();
		}


		TeamTypes eTeamLoop;
		for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			eTeamLoop = (TeamTypes) iI;

			CvTeam& kLoopTeam = GET_TEAM(eTeamLoop);

			if(kLoopTeam.isAlive())
			{

				if(!isHuman() || kLoopTeam.isHuman())
				{
					if(!isInvisible(eTeamLoop, false))
					{
						if(pNewPlot->isVisible(eTeamLoop))
						{
							kLoopTeam.meet(eOurTeam, false);
						}
					}
				}
			}
		}


		InvisibleTypes eInvisoType = getInvisibleType();
		if(eOurTeam != activeTeam && eInvisoType != NO_INVISIBLE)
		{
			bool bOldInvisibleVisible = false;
			if(pOldPlot)
				bOldInvisibleVisible = pOldPlot->isInvisibleVisible(activeTeam, eInvisoType);
			bool bNewInvisibleVisible = pNewPlot->isInvisibleVisible(activeTeam, eInvisoType);
			if(bOldInvisibleVisible != bNewInvisibleVisible)
			{
				auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
				gDLL->GameplayUnitVisibility(pDllUnit.get(), bNewInvisibleVisible, true);
			}
		}

		CvTeam& kOurTeam = GET_TEAM(eOurTeam);


		CvPlot* pAdjacentPlot;
		for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(pNewPlot->getX(), pNewPlot->getY(), ((DirectionTypes)iI));

			if(pAdjacentPlot != NULL)
			{

				if(pAdjacentPlot->getTeam() != NO_TEAM)
				{
					kOurTeam.meet(pAdjacentPlot->getTeam(), false);
				}


				if(isBarbarian() && getDomainType() == DOMAIN_SEA && pAdjacentPlot->isWater())
				{
					UnitHandle pAdjacentUnit = pAdjacentPlot->getBestDefender(NO_PLAYER, BARBARIAN_PLAYER, NULL, true);
					if(pAdjacentUnit)
					{
						GET_PLAYER(pAdjacentUnit->getOwner()).GetPlayerTraits()->CheckForBarbarianConversion(pAdjacentPlot);
					}
				}


				UnitClassTypes eMissionary = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_MISSIONARY", true);
				if (eMissionary != NO_UNITCLASS)
				{
					if (getUnitClassType() == eMissionary)
					{
						ReligionTypes eFoundedReligion = kPlayer.GetReligions()->GetReligionCreatedByPlayer();
						const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eFoundedReligion, kPlayer.GetID());
						if (pReligion && pReligion->m_Beliefs.IsConvertsBarbarians())
						{
							UnitHandle pAdjacentUnit = pAdjacentPlot->getBestDefender(BARBARIAN_PLAYER);
							if(pAdjacentUnit)
							{
								CvBeliefHelpers::ConvertBarbarianUnit(&kPlayer, pAdjacentUnit);
							}
						}
					}

					else if (isBarbarian())
					{
						if(pAdjacentPlot->getNumUnits() > 0)
						{
#ifdef AUI_WARNING_FIXES
							for (uint iNearbyUnitLoop = 0; iNearbyUnitLoop < pAdjacentPlot->getNumUnits(); iNearbyUnitLoop++)
#else
							for(int iNearbyUnitLoop = 0; iNearbyUnitLoop < pAdjacentPlot->getNumUnits(); iNearbyUnitLoop++)
#endif
							{
								const CvUnit* const adjUnit = pAdjacentPlot->getUnitByIndex(iNearbyUnitLoop);
								if (adjUnit && adjUnit->getUnitClassType() == eMissionary)
								{
									CvPlayer &adjUnitPlayer = GET_PLAYER(adjUnit->getOwner());
									ReligionTypes eFoundedReligion = adjUnitPlayer.GetReligions()->GetReligionCreatedByPlayer();
									const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eFoundedReligion, adjUnitPlayer.GetID());
									if (pReligion && pReligion->m_Beliefs.IsConvertsBarbarians())
									{
										if (CvBeliefHelpers::ConvertBarbarianUnit(&adjUnitPlayer, this))
										{
											ClearMissionQueue();
										}
									}
								}
							}
						}
					}
				}


				FeatureTypes eFeature = pAdjacentPlot->getFeatureType();
				if(eFeature != NO_FEATURE && GC.getFeatureInfo(eFeature)->IsNaturalWonder())
				{
					PromotionTypes ePromotion = (PromotionTypes)GC.getFeatureInfo(eFeature)->getAdjacentUnitFreePromotion();
					if(ePromotion != NO_PROMOTION)
					{

						if(m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT &&
#ifdef AUI_WARNING_FIXES
							::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()))
#else
						        (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
#endif
						{
							setHasPromotion(ePromotion, true);
						}
					}
				}
			}
		}

		if(pOldPlot != NULL && getDomainType() == DOMAIN_SEA)
		{
			kPlayer.GetPlayerTraits()->CheckForBarbarianConversion(pNewPlot);
		}

		if(GC.IsGraphicsInitialized())
		{

			if(bCheckPlotVisible && (pNewPlot->isVisibleToWatchingHuman() || pOldPlot->isVisibleToWatchingHuman()))
				bShow = true;

			if(CvPreGame::quickMovement())
			{
				bShow = false;
			}

			if(bShow)
			{
				QueueMoveForVisualization(pNewPlot);
			}
			else
			{
				SetPosition(pNewPlot);
			}
		}

	}

	if(pOldPlot != NULL)
	{
		if(hasCargo())
		{
			pUnitNode = pOldPlot->headUnitNode();

			while(pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(*pUnitNode);
				pUnitNode = pOldPlot->nextUnitNode(pUnitNode);

				if(pLoopUnit && pLoopUnit->getTransportUnit() == this)
				{
					pLoopUnit->setXY(iX, iY, bGroup, bUpdate);


					pUnitNode = pOldPlot->headUnitNode();
				}
			}
		}
	}

	if(pNewPlot != NULL && iMapLayer == DEFAULT_UNIT_MAP_LAYER)
	{
		if(!bNoMove)
		{
			if(pNewPlot->isGoody(getTeam()))
			{
				GET_PLAYER(getOwner()).doGoody(pNewPlot, this);
			}
			if(!isBarbarian())
			{
				if(pNewPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
				{

					kPlayer.GetTacticalAI()->DeleteTemporaryZone(pNewPlot);


					if(!kPlayer.GetPlayerTraits()->CheckForBarbarianConversion(pNewPlot))
					{
						int iNumGold = GC.getGame().getHandicapInfo().getBarbCampGold();
						iNumGold *= (100 + kPlayer.GetPlayerTraits()->GetPlunderModifier()) / 100;


						iNumGold *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType())->getGoldPercent();
						iNumGold /= 100;


						if(getOwner() == GC.getGame().getActivePlayer())
						{
							DLLUI->AddUnitMessage(0, GetIDInfo(), getOwner(), true, GC.getEVENT_MESSAGE_TIME(), GetLocalizedText("TXT_KEY_MISC_DESTROYED_BARBARIAN_CAMP", iNumGold));
						}

						pNewPlot->setImprovementType(NO_IMPROVEMENT);

						CvBarbarians::DoBarbCampCleared(pNewPlot, getOwner());

						kPlayer.GetTreasury()->ChangeGold(iNumGold);


						pNewPlot->SetPlayerThatClearedBarbCampHere(getOwner());

						if(getOwner() < MAX_MAJOR_CIVS)
						{
#ifdef NQ_CLEARING_CAMPS_GIVES_INFLUENCE_NEARBY
#endif

							PlayerTypes eMinor;
							for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
							{
								eMinor = (PlayerTypes) iMinorLoop;
								CvPlayer& minorPlayer = GET_PLAYER(eMinor);

								if(!minorPlayer.isAlive())
									continue;

								CvMinorCivAI* pMinorCivAI = minorPlayer.GetMinorCivAI();
								pMinorCivAI->DoTestActiveQuestsForPlayer(getOwner(),                   true,                   false, MINOR_CIV_QUEST_KILL_CAMP);
							}
						}


						if(getOwner() == GC.getGame().getActivePlayer())
						{

							if(!GC.getGame().isNetworkMultiPlayer())
							{
								CvPopupInfo kPopupInfo(BUTTONPOPUP_BARBARIAN_CAMP_REWARD, iNumGold);
								DLLUI->AddPopup(kPopupInfo);

								CancelActivePlayerEndTurn();


								if(kPlayer.isHuman() && !GC.getGame().isGameMultiPlayer())
								{
									gDLL->IncrementSteamStatAndUnlock(ESTEAMSTAT_BARBARIANCAMPS, 100, ACHIEVEMENT_100CAMPS);
								}

							}
						}
					}
				}
			}
		}


		PlayerTypes ePlotOwner = pNewPlot->getOwner();
		if(ePlotOwner != NO_PLAYER)
		{

			if(pNewPlot->IsFriendlyTerritory(getOwner()))
			{
				if(GET_TEAM(eOurTeam).canEmbark())
				{
					PromotionTypes ePromotionEmbarkation = kPlayer.GetEmbarkationPromotion();

					bool bGivePromotion = false;


					if(getDomainType() == DOMAIN_LAND)
					{
						if(!IsCombatUnit())
							bGivePromotion = true;
					}


					if(!bGivePromotion && ::IsPromotionValidForUnitCombatType(ePromotionEmbarkation, getUnitType()))
						bGivePromotion = true;


					if(bGivePromotion)
						setHasPromotion(ePromotionEmbarkation, true);
				}
			}


			if(GET_TEAM(getTeam()).isAtWar(GET_PLAYER(ePlotOwner).getTeam()) && !isInvisible(GET_PLAYER(ePlotOwner).getTeam(),false,false))
			{
				CvNotifications* pNotifications = GET_PLAYER(ePlotOwner).GetNotifications();
				if(pNotifications)
				{
					Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_ENEMY");
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_ENEMY_SUMMARY");
					pNotifications->Add(NOTIFICATION_ENEMY_IN_TERRITORY, strMessage.toUTF8(), strSummary.toUTF8(), iX, iY, getUnitType(), getOwner());
				}
			}
		}
	}


	for(uint uiCaptureIndex = 0; uiCaptureIndex < kCaptureUnitList.size(); ++uiCaptureIndex)
	{
		createCaptureUnit(kCaptureUnitList[uiCaptureIndex]);
	}

	CvAssert(pOldPlot != pNewPlot);

	if(IsSelected())
		gDLL->GameplayMinimapUnitSelect(iX, iY);

	setInfoBarDirty(true);


	int iAttackRange = GC.getCITY_ATTACK_RANGE();
#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX, iDX;
	CvPlot* pTargetPlot;
	for (int iDY = -iAttackRange; iDY <= iAttackRange; iDY++)
	{
		iMaxDX = iAttackRange - MAX(0, iDY);
		for (iDX = -iAttackRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
		{

			pTargetPlot = plotXY(getX(), getY(), iDX, iDY);
#else
	for(int iDX = -iAttackRange; iDX <= iAttackRange; iDX++)
	{
		for(int iDY = -iAttackRange; iDY <= iAttackRange; iDY++)
		{
			CvPlot* pTargetPlot = plotXYWithRangeCheck(getX(), getY(), iDX, iDY, iAttackRange);
#endif
			if(pTargetPlot && pTargetPlot->isCity())
			{
				if(isEnemy(pTargetPlot->getTeam()))
				{

					CvCity* pkPlotCity = pTargetPlot->getPlotCity();
					auto_ptr<ICvCity1> pPlotCity = GC.WrapCityPointer(pkPlotCity);
					DLLUI->SetSpecificCityInfoDirty(pPlotCity.get(), CITY_UPDATE_TYPE_ENEMY_IN_RANGE);
				}
			}
		}
	}


	if((pNewPlot && pNewPlot->isCity()) || (pOldPlot && pOldPlot->isCity()))
	{
		GET_PLAYER(getOwner()).DoUpdateHappiness();
#ifdef AUI_CITIZENS_MID_TURN_ASSIGN_RUNS_SELF_CONSISTENCY
		GET_PLAYER(getOwner()).doSelfConsistencyCheckAllCities();
#endif
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(getOwner());
		args->Push(GetID());
		args->Push(getX());
		args->Push(getY());

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "UnitSetXY", args.get(), bResult);
	}

	if (bOwnerIsActivePlayer)
		DLLUI->SetDontShowPopups(false);


	CvCity* pkNewGarrisonCity = GetGarrisonedCity();
	if (pkNewGarrisonCity && pkNewGarrisonCity != pkPrevGarrisonedCity)
	{
		auto_ptr<ICvCity1> pkDllCity(new CvDllCity(pkNewGarrisonCity));
		DLLUI->SetSpecificCityInfoDirty(pkDllCity.get(), CITY_UPDATE_TYPE_GARRISON);
	}
	if(pkPrevGarrisonedCity && pkPrevGarrisonedCity != pkNewGarrisonCity)
	{
		auto_ptr<ICvCity1> pkDllCity(new CvDllCity(pkPrevGarrisonedCity));
		DLLUI->SetSpecificCityInfoDirty(pkDllCity.get(), CITY_UPDATE_TYPE_GARRISON);
	}
	

	if (isHuman() && !isDelayedDeath())
	{
		if(strcmp(getCivilizationInfo().GetType(), "CIVILIZATION_BRAZIL") == 0){
			UnitTypes eExplorer = (UnitTypes) GC.getInfoTypeForString("UNIT_EXPLORER", true                );
			if(getUnitType() == eExplorer && strcmp(getNameNoDesc(), "TXT_KEY_EXPLORER_STANLEY") == 0 ){
				CvPlot* pAdjacentPlot;
				for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
				{
					pAdjacentPlot = plotDirection(pNewPlot->getX(), pNewPlot->getY(), ((DirectionTypes)iI));

					if(pAdjacentPlot != NULL && pAdjacentPlot->getNumUnits() != NULL){
#ifdef AUI_WARNING_FIXES
						for (uint iJ = 0; iJ < pAdjacentPlot->getNumUnits(); iJ++)
						{
#else
						for(int iJ = 0; iJ < pAdjacentPlot->getNumUnits(); iJ++){
#endif
							if(pAdjacentPlot->getUnitByIndex(iJ)->getUnitType() ==  eExplorer && strcmp(pAdjacentPlot->getUnitByIndex(iJ)->getNameNoDesc(), "TXT_KEY_EXPLORER_LIVINGSTON") == 0){
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_52);
							}
						}
					}
				}
			}
		}
	}

#ifdef UPDATE_MINOR_BG_ICON_ON_UNIT_MOVE_OR_SET_DAMAGE
	for (int jJ = MAX_MAJOR_CIVS; jJ < MAX_MINOR_CIVS; jJ++)
	{
		for (int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (oldCanBully[iI * (MAX_MINOR_CIVS - MAX_MAJOR_CIVS) + jJ - MAX_MAJOR_CIVS] != GET_PLAYER((PlayerTypes)jJ).GetMinorCivAI()->CanMajorBullyGold((PlayerTypes)iI))
				{
					CvPlayer& kMinor = GET_PLAYER((PlayerTypes)jJ);
					if (kMinor.isAlive())
					{
						if (kMinor.getCapitalCity())
						{
							if (kMinor.getCapitalCity()->plot())
							{
								kMinor.getCapitalCity()->plot()->updateFog();
								break;
							}
						}
					}
				}
			}
		}
	}
#endif
}


bool CvUnit::at(int iX, int iY) const
{
	VALIDATE_OBJECT
	return((getX() == iX) && (getY() == iY));
}



bool CvUnit::atPlot(const CvPlot& checkPlot) const
{
	VALIDATE_OBJECT
	return((getX() == checkPlot.getX()) && (getY() == checkPlot.getY()));
}



CvPlot* CvUnit::plot() const
{
	VALIDATE_OBJECT
	return GC.getMap().plotCheckInvalid(getX(), getY());
}



int CvUnit::getArea() const
{
	VALIDATE_OBJECT
	return GC.getMap().plotCheckInvalid(getX(), getY())->getArea();
}



CvArea* CvUnit::area() const
{
	VALIDATE_OBJECT
	return GC.getMap().plotCheckInvalid(getX(), getY())->area();
}



bool CvUnit::onMap() const
{
	VALIDATE_OBJECT
	return (plot() != NULL);
}



int CvUnit::getLastMoveTurn() const
{
	VALIDATE_OBJECT
	return m_iLastMoveTurn;
}



void CvUnit::setLastMoveTurn(int iNewValue)
{
	VALIDATE_OBJECT
	m_iLastMoveTurn = iNewValue;
	CvAssert(getLastMoveTurn() >= 0);
}



int CvUnit::GetCycleOrder() const
{
	VALIDATE_OBJECT
	return m_iCycleOrder;
}



void CvUnit::SetCycleOrder(int iNewValue)
{
	VALIDATE_OBJECT
	m_iCycleOrder = iNewValue;
}



bool CvUnit::IsRecon() const
{
	return GetReconCount() > 0;
}


int CvUnit::GetReconCount() const
{
	return m_iReconCount;
}


void CvUnit::ChangeReconCount(int iChange)
{
	if(iChange != 0)
	{
		m_iReconCount += iChange;
	}
}


CvPlot* CvUnit::getReconPlot() const
{
	VALIDATE_OBJECT
	return GC.getMap().plotCheckInvalid(m_iReconX, m_iReconY);
}



void CvUnit::setReconPlot(CvPlot* pNewValue)
{
	VALIDATE_OBJECT
	CvPlot* pOldPlot;

	pOldPlot = getReconPlot();

	if(pOldPlot != pNewValue)
	{
		if(pOldPlot != NULL)
		{
			if (canChangeVisibility())
				pOldPlot->changeAdjacentSight(getTeam(), GC.getRECON_VISIBILITY_RANGE(), false, getSeeInvisibleType(), getFacingDirection(true));
			pOldPlot->changeReconCount(-1);
		}

		if(pNewValue == NULL)
		{
			m_iReconX = INVALID_PLOT_COORD;
			m_iReconY = INVALID_PLOT_COORD;
		}
		else
		{
			m_iReconX = pNewValue->getX();
			m_iReconY = pNewValue->getY();

			pNewValue->changeReconCount(1);
			if (canChangeVisibility())
				pNewValue->changeAdjacentSight(getTeam(), GC.getRECON_VISIBILITY_RANGE(), true, getSeeInvisibleType(), getFacingDirection(true));
		}
	}
}



int CvUnit::getGameTurnCreated() const
{
	VALIDATE_OBJECT
	return m_iGameTurnCreated;
}



void CvUnit::setGameTurnCreated(int iNewValue)
{
	VALIDATE_OBJECT
	m_iGameTurnCreated = iNewValue;
	CvAssert(getGameTurnCreated() >= 0);
}



int CvUnit::getDamage() const
{
	VALIDATE_OBJECT
	return m_iDamage;
}











void CvUnit::ShowDamageDeltaText(int iDelta, CvPlot* pkPlot, float fAdditionalTextDelay            , const CvString* pAppendText             )
{
	if (pkPlot)
	{
		if(pkPlot->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF)
		{
			float fDelay = 0.0f + fAdditionalTextDelay;
			CvString text;
			if(iDelta <= 0)
			{
				text.Format("[COLOR_GREEN]+%d", -iDelta);
				fDelay = GC.getPOST_COMBAT_TEXT_DELAY() * 2;
			}
			else
			{
				text.Format("[COLOR_RED]%d", -iDelta);
			}
			if(pAppendText != NULL)
			{
				text += " ";
				text += *pAppendText;
			}
			text += "[ENDCOLOR]";

			DLLUI->AddPopupText(pkPlot->getX(), pkPlot->getY(), text.c_str(), fDelay);
		}
	}
}













int CvUnit::setDamage(int iNewValue, PlayerTypes ePlayer, float fAdditionalTextDelay, const CvString* pAppendText)
{
	VALIDATE_OBJECT
#ifdef UPDATE_MINOR_BG_ICON_ON_UNIT_MOVE_OR_SET_DAMAGE
	std::vector<bool> oldCanBully(MAX_MAJOR_CIVS * (MAX_MINOR_CIVS - MAX_MAJOR_CIVS));
	for (int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		for (int jJ = MAX_MAJOR_CIVS; jJ < MAX_MINOR_CIVS; jJ++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				oldCanBully[iI * (MAX_MINOR_CIVS - MAX_MAJOR_CIVS) + jJ - MAX_MAJOR_CIVS] = GET_PLAYER((PlayerTypes)jJ).GetMinorCivAI()->CanMajorBullyGold((PlayerTypes)iI);
			}
			else
			{
				oldCanBully[iI * (MAX_MINOR_CIVS - MAX_MAJOR_CIVS) + jJ - MAX_MAJOR_CIVS] = false;
			}
		}
	}
#endif

	int iOldValue;

	iOldValue = getDamage();

	if(GetCurrHitPoints() == GetMaxHitPoints() && iNewValue < GetMaxHitPoints()) 
	{
		m_iLastGameTurnAtFullHealth = GC.getGame().getGameTurn();
	}

	m_iDamage = range(iNewValue, 0, GetMaxHitPoints());
	int iDiff = m_iDamage - iOldValue;

	CvAssertMsg(GetCurrHitPoints() >= 0, "currHitPoints() is expected to be non-negative (invalid Index)");

	if(iOldValue != getDamage())
	{
		if(IsGarrisoned())
		{
			if(GetGarrisonedCity() != NULL)
			{
				GetGarrisonedCity()->updateStrengthValue();
			}
		}

		setInfoBarDirty(true);

		if(IsSelected())
		{
			DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
		}

		auto_ptr<ICvPlot1> pDllSelectionPlot(DLLUI->getSelectionPlot());
#ifdef AUI_WARNING_FIXES
		const uint iSelectionPlotIndex = (pDllSelectionPlot.get() != NULL) ? uint(pDllSelectionPlot->GetPlotIndex()) : MAX_UNSIGNED_INT;
#else
		const int iSelectionPlotIndex = (pDllSelectionPlot.get() != NULL)? pDllSelectionPlot->GetPlotIndex() : -1;
#endif

		if(plot()->GetPlotIndex() == iSelectionPlotIndex)
		{
			DLLUI->setDirty(PlotListButtons_DIRTY_BIT, true);
		}

		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitSetDamage(pDllUnit.get(), m_iDamage, iOldValue);

		if(!IsDead() && !isSuicide() && 
			fAdditionalTextDelay >= 0.f)
		{
			ShowDamageDeltaText(iDiff, plot(), fAdditionalTextDelay, pAppendText);
		}
	}

	if(IsDead())
	{
		CvGame& game = GC.getGame();
		const PlayerTypes eActivePlayer = game.getActivePlayer();
		CvPlayerAI& activePlayer = GET_PLAYER(eActivePlayer);
		if(m_iLastGameTurnAtFullHealth != -1 && m_iLastGameTurnAtFullHealth == game.getGameTurn() && getOwner() == eActivePlayer && activePlayer.isHuman())
		{
			CvUnitEntry* pUnitInfo = GC.getUnitInfo(getUnitType());
			if(pUnitInfo != NULL && strcmp(pUnitInfo->GetType(),"UNIT_XCOM_SQUAD") == 0)
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_XP2_46);
			}
		}

		m_iLastGameTurnAtFullHealth = -1;

		kill(true, ePlayer);

		CvString szMsg;
		CvString szUnitAIString;
		getUnitAIString(szUnitAIString, AI_getUnitAIType());
		szMsg.Format("Killed in combat: %s, AI was: ", getName().GetCString());
		szMsg += szUnitAIString;
		GET_PLAYER(m_eOwner).GetTacticalAI()->LogTacticalMessage(szMsg, true                          );

		if(ePlayer != NO_PLAYER)
		{
			if(m_eOwner == BARBARIAN_PLAYER && plot()->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
			{
				GET_PLAYER(ePlayer).GetPlayerTraits()->SetDefeatedBarbarianCampGuardType(getUnitType());
			}

			GET_PLAYER(ePlayer).DoUnitKilledCombat(getOwner(), getUnitType());
		}
	}

#ifdef UPDATE_MINOR_BG_ICON_ON_UNIT_MOVE_OR_SET_DAMAGE
	for (int jJ = MAX_MAJOR_CIVS; jJ < MAX_MINOR_CIVS; jJ++)
	{
		for (int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (oldCanBully[iI * (MAX_MINOR_CIVS - MAX_MAJOR_CIVS) + jJ - MAX_MAJOR_CIVS] != GET_PLAYER((PlayerTypes)jJ).GetMinorCivAI()->CanMajorBullyGold((PlayerTypes)iI))
				{
					PlayerTypes eLoopMinor = (PlayerTypes)jJ;
					if (GET_PLAYER(eLoopMinor).isAlive())
					{
						if (GET_PLAYER(eLoopMinor).getCapitalCity())
						{
							if (GET_PLAYER(eLoopMinor).getCapitalCity()->plot())
							{
								GET_PLAYER(eLoopMinor).getCapitalCity()->plot()->updateFog();
								break;
							}
						}
					}
				}
			}
		}
	}
#endif

	return iDiff;
}













int CvUnit::changeDamage(int iChange, PlayerTypes ePlayer, float fAdditionalTextDelay, const CvString* pAppendText)
{
	VALIDATE_OBJECT;

#ifdef LEKMOD_NEW_LUA_EVENTS
	if (iChange < 0)
	{
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem)
		{
			CvLuaArgsHandle args;
			args->Push(getOwner());
			args->Push(GetID());
			args->Push(iChange);
			args->Push(getX());
			args->Push(getY());


			bool bResult = false;
			LuaSupport::CallHook(pkScriptSystem, "UnitHealed", args.get(), bResult);
		}
	}

#endif

#ifdef ENHANCED_GRAPHS
	if (ePlayer != NO_PLAYER && getUnitCombatType() != NO_UNITCOMBAT && iChange > 0)
	{
		GET_PLAYER(ePlayer).ChangeUnitsDamageDealt(iChange);
		GET_PLAYER(getOwner()).ChangeUnitsDamageTaken(iChange);
	}
#endif

	return setDamage(getDamage() + iChange, ePlayer, fAdditionalTextDelay, pAppendText);


}

int CvUnit::getMoves() const
{
	VALIDATE_OBJECT
	return m_iMoves;
}



void CvUnit::setMoves(int iNewValue)
{
	VALIDATE_OBJECT
	if(getMoves() != iNewValue)
	{
		CvPlot* pPlot = plot();

		m_iMoves = iNewValue;

		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitShouldDimFlag(pDllUnit.get(),          getMoves() <= 0);

		if(IsSelected())
		{
			GC.getPathFinder().ForceReset();
			DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
		}

		auto_ptr<ICvPlot1> pDllSelectionPlot(DLLUI->getSelectionPlot());
#ifdef AUI_WARNING_FIXES
		const uint iSelectionPlotIndex = (pDllSelectionPlot.get() != NULL) ? uint(pDllSelectionPlot->GetPlotIndex()) : MAX_UNSIGNED_INT;
#else
		int iSelectionPlotIndex = (pDllSelectionPlot.get() != NULL)? pDllSelectionPlot->GetPlotIndex() : -1;
#endif

		if(pPlot->GetPlotIndex() == iSelectionPlotIndex)
		{
			DLLUI->setDirty(PlotListButtons_DIRTY_BIT, true);
		}
	}
}



void CvUnit::changeMoves(int iChange)
{
	VALIDATE_OBJECT
	setMoves(getMoves() + iChange);
}



void CvUnit::finishMoves()
{
	VALIDATE_OBJECT
	setMoves(0);
}

#if defined(NQM_UNIT_FIX_NO_DOUBLE_INSTAHEAL_ON_SAME_TURN) || defined(NQM_UNIT_FIX_NO_INSTAHEAL_AFTER_PARADROP)
bool CvUnit::canInstahealThisTurn() const
{
	return m_bCanInstahealThisTurn;
}
void CvUnit::setCanInstahealThisTurn(bool bNewValue)
{
	m_bCanInstahealThisTurn = bNewValue;
}
#endif

#if defined(NQM_UNIT_NO_AA_INTERCEPT_AFTER_MOVE_BEFORE_TURN_END) || defined(NQM_UNIT_FIGHTER_NO_INTERCEPT_UNTIL_AFTER_TURN_END)
bool CvUnit::isInterceptBlockedUntilEndTurn() const
{
	return m_bIsInterceptBlockedUntilEndTurn;
}
void CvUnit::setIsInterceptBlockedUntilEndTurn(bool bNewValue)
{

	if (m_bIsInterceptBlockedUntilEndTurn != bNewValue) 
		m_bIsInterceptBlockedUntilEndTurn = bNewValue;
}
#endif



bool CvUnit::IsImmobile() const
{
	if(getDomainType() == DOMAIN_IMMOBILE)
	{
		return true;
	}

	return m_bImmobile;
}


void CvUnit::SetImmobile(bool bValue)
{
	if(IsImmobile() != bValue)
	{
		m_bImmobile = bValue;
	}
}



bool CvUnit::IsInFriendlyTerritory() const
{
	VALIDATE_OBJECT
	return plot()->IsFriendlyTerritory(getOwner());
}



bool CvUnit::IsUnderEnemyRangedAttack() const
{
	PlayerTypes eLoopPlayer;
#ifdef AUI_UNIT_FIX_UNDER_ENEMY_RANGED_ATTACK_HEALRATE
	int iTotalDamage = AUI_UNIT_FIX_UNDER_ENEMY_RANGED_ATTACK_HEALRATE;
#else
	int iTotalDamage = 0;
#endif

	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		CvPlayer &kPlayer = GET_PLAYER(eLoopPlayer);
		if(kPlayer.isAlive())
		{
			if (atWar(getTeam(), kPlayer.getTeam()))
			{
				int iLoop;
				CvCity *pLoopCity;
				for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
				{
					if(pLoopCity->canRangeStrikeAt(getX(), getY()))
					{
#ifdef AUI_UNIT_FIX_UNDER_ENEMY_RANGED_ATTACK_HEALRATE
						iTotalDamage += pLoopCity->rangeCombatDamage(this, NULL, false);
						if (iTotalDamage >= healRate(plot()) || GetCurrHitPoints() - iTotalDamage <= 0)
						{
							return true;
						}
#else
						return true;
#endif
					}
				}

				CvUnit *pLoopUnit;
				for(pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
				{
					if(pLoopUnit)
					{
						if(!pLoopUnit->IsCanAttack())
						{
							continue;
						}

						if(pLoopUnit->IsCanAttackRanged())
						{

							if(plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), getX(), getY()) <= pLoopUnit->GetRange())
							{

								if(pLoopUnit->canEverRangeStrikeAt(getX(), getY()))
								{

									int iExpectedDamage = pLoopUnit->GetRangeCombatDamage(this, NULL, false);
									iTotalDamage += iExpectedDamage;
#ifdef AUI_UNIT_FIX_UNDER_ENEMY_RANGED_ATTACK_HEALRATE
									if (iTotalDamage >= healRate(plot()))
#else
									if (iTotalDamage > healRate(plot()))
#endif
									{
										return true;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return false;
}


int CvUnit::getExperience() const
{
	VALIDATE_OBJECT
	return m_iExperience;
}



void CvUnit::setExperience(int iNewValue, int iMax)
{
	VALIDATE_OBJECT
	if((getExperience() != iNewValue) && (getExperience() < ((iMax == -1) ? INT_MAX : iMax)))
	{
		int iExperienceChange = iNewValue - getExperience();

		m_iExperience = std::min(((iMax == -1) ? INT_MAX : iMax), iNewValue);
		CvAssert(getExperience() >= 0);

#ifdef AUI_UNIT_TEST_PROMOTION_READY_MOVED
		if (!IsDead())
		{
			testPromotionReady();
			if (getOwner() == GC.getGame().getActivePlayer())
#else
		if(getOwner() == GC.getGame().getActivePlayer())
		{
#if !defined(LEKMOD_RETRAIN_MISSION)

			if(!IsDead())
#else
			if (!IsDead() && iExperienceChange > 0)
#endif
#endif
			{
				Localization::String localizedText = Localization::Lookup("TXT_KEY_EXPERIENCE_POPUP");
				localizedText << iExperienceChange;
				float fDelay = GC.getPOST_COMBAT_TEXT_DELAY();

				int iX = m_iX;
				int iY = m_iY;


				const MissionData* pkMissionData = GetHeadMissionData();
				if(pkMissionData != NULL)
				{
					if(pkMissionData->eMissionType == CvTypes::getMISSION_MOVE_TO())
					{
						int iTempX = pkMissionData->iData1;
						int iTempY = pkMissionData->iData2;

						if(GC.getMap().plot(iTempX, iTempY)->getNumVisibleEnemyDefenders(this) == 0)
						{
							iX = iTempX;
							iY = iTempY;
						}
					}
				}

				DLLUI->AddPopupText(iX, iY, localizedText.toUTF8(), fDelay);

				if(IsSelected())
				{
					DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
				}
			}
		}
	}
}



void CvUnit::changeExperience(int iChange, int iMax, bool bFromCombat, bool bInBorders, bool bUpdateGlobal)
{
	VALIDATE_OBJECT

	if(isBarbarian() && iChange > 0)
	{
		return;
	}

	int iUnitExperience = iChange;
	PromotionTypes eNewPromotion = NO_PROMOTION;

	if(bFromCombat)
	{
		CvPlayer& kPlayer = GET_PLAYER(getOwner());


#ifdef AUI_WARNING_FIXES
		for (uint iI = 0; iI < GC.getNumPromotionInfos(); iI++)
#else
		for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
#endif
		{
			const PromotionTypes eLoopPromotion = static_cast<PromotionTypes>(iI);
			CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eLoopPromotion);
			if(pkPromotionInfo)
			{
				if (pkPromotionInfo->HasPostCombatPromotions() && m_Promotions.HasPromotion(eLoopPromotion))
				{
					eNewPromotion = m_Promotions.ChangePromotionAfterCombat(eLoopPromotion);
					setHasPromotion(eLoopPromotion, false);

					if (eNewPromotion != NO_PROMOTION)
					{
						setHasPromotion(eNewPromotion, true);

						CvPromotionEntry* pkNewPromotionInfo = GC.getPromotionInfo(eNewPromotion);
						Localization::String localizedText = Localization::Lookup(pkNewPromotionInfo->GetDescriptionKey());
						float fDelay = GC.getPOST_COMBAT_TEXT_DELAY() * 2;
						DLLUI->AddPopupText(getX(), getY(), localizedText.toUTF8(), fDelay);
					}
				}
			}
		}


		int iCombatExperienceMod;
		if (getDomainType() == DOMAIN_SEA)
		{
			iCombatExperienceMod = 100 + kPlayer.getGreatAdmiralRateModifier();
		}
		else
		{
			iCombatExperienceMod = 100 + kPlayer.getGreatGeneralRateModifier();
		}


#ifdef LEKMOD_EXCLUDE_AIR_EXP_BONUS
		if (getDomainType() != DOMAIN_AIR)
		{
			iUnitExperience += (iChange * kPlayer.getExpModifier()) / 100;
		}
#else
		iUnitExperience += (iChange * kPlayer.getExpModifier()) / 100;
#endif

		if (bInBorders && getDomainType() == DOMAIN_LAND)
		{

			iCombatExperienceMod += kPlayer.getDomesticGreatGeneralRateModifier() + kPlayer.getExpInBorderModifier();
			iUnitExperience += (iChange * kPlayer.getExpInBorderModifier()) / 100;
		}

		if(bUpdateGlobal)
		{

			if (getDomainType() == DOMAIN_LAND) 
			{
				iCombatExperienceMod += getGreatGeneralModifier();
			}

			if(iMax == -1)
			{
				if(getDomainType() == DOMAIN_SEA)
				{
					kPlayer.changeNavalCombatExperience((iChange * iCombatExperienceMod) / 100);
#if defined(DISPLAY_GENERAL_ADMIRAL_POINTS)
					ReportGGPoints((iChange * iCombatExperienceMod) / 100, true             );
#endif
				}
				else
				{
					kPlayer.changeCombatExperience((iChange * iCombatExperienceMod) / 100);
#if defined(DISPLAY_GENERAL_ADMIRAL_POINTS)
					ReportGGPoints((iChange * iCombatExperienceMod) / 100, false             );
#endif
				}
			}
			else
			{
				int iModdedChange = min(iMax - m_iExperience, iChange);
				if(iModdedChange > 0)
				{
					if(getDomainType() == DOMAIN_SEA)
					{
						kPlayer.changeNavalCombatExperience((iModdedChange * iCombatExperienceMod) / 100);
#if defined(DISPLAY_GENERAL_ADMIRAL_POINTS)
						ReportGGPoints((iModdedChange* iCombatExperienceMod) / 100, true             );
#endif
					}
					else
					{
						kPlayer.changeCombatExperience((iModdedChange * iCombatExperienceMod) / 100);
#if defined(DISPLAY_GENERAL_ADMIRAL_POINTS)
						ReportGGPoints((iModdedChange * iCombatExperienceMod) / 100, false             );
#endif
					}
				}
			}
		}

		if(getExperiencePercent() != 0)
		{
			iUnitExperience *= std::max(0, 100 + getExperiencePercent());
			iUnitExperience /= 100;
		}
	}

	setExperience((getExperience() + iUnitExperience), iMax);
}
#if defined(DISPLAY_GENERAL_ADMIRAL_POINTS)
void CvUnit::ReportGGPoints(int iPoints, bool bAdmiral)
{
	if (iPoints > 0 && getOwner() == GC.getGame().getActivePlayer())
	{
		Localization::String localizedText;
		if (bAdmiral)
			localizedText = Localization::Lookup("TXT_KEY_GREAT_ADMIRAL_POINTS_GAINED");
		else
			localizedText = Localization::Lookup("TXT_KEY_GREAT_GENERAL_POINTS_GAINED");
		localizedText << iPoints;

		int iX = m_iX;
		int iY = m_iY;
		const MissionData* pkMissionData = GetHeadMissionData();
		if (pkMissionData != NULL && pkMissionData->eMissionType == CvTypes::getMISSION_MOVE_TO())
		{
			int iTempX = pkMissionData->iData1;
			int iTempY = pkMissionData->iData2;
			if (GC.getMap().plot(iTempX, iTempY)->getNumVisibleEnemyDefenders(this) == 0)
			{
				iX = iTempX;
				iY = iTempY;
			}
		}

		float fDelay = GC.getPOST_COMBAT_TEXT_DELAY() + 0.5f;
		DLLUI->AddPopupText(iX, iY, localizedText.toUTF8(), fDelay);
	}
}
#endif

int CvUnit::getLevel() const
{
	VALIDATE_OBJECT
	return m_iLevel;
}



void CvUnit::setLevel(int iNewValue)
{
	VALIDATE_OBJECT
	if(getLevel() != iNewValue)
	{
		m_iLevel = iNewValue;
		CvAssert(getLevel() >= 0);

		if(getLevel() > GET_PLAYER(getOwner()).getHighestUnitLevel())
		{
			GET_PLAYER(getOwner()).setHighestUnitLevel(getLevel());
		}

		if(IsSelected())
		{
			DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
		}
	}
}



void CvUnit::changeLevel(int iChange)
{
	VALIDATE_OBJECT
	setLevel(getLevel() + iChange);
}



int CvUnit::getCargo() const
{
	VALIDATE_OBJECT
	return m_iCargo;
}



void CvUnit::changeCargo(int iChange)
{
	VALIDATE_OBJECT
	m_iCargo = (m_iCargo + iChange);
	CvAssert(getCargo() >= 0);
}



CvPlot* CvUnit::getAttackPlot() const
{
	VALIDATE_OBJECT
	return GC.getMap().plotCheckInvalid(m_iAttackPlotX, m_iAttackPlotY);
}



void CvUnit::setAttackPlot(const CvPlot* pNewValue, bool bAirCombat)
{
	VALIDATE_OBJECT
	if(getAttackPlot() != pNewValue)
	{
		if(pNewValue != NULL)
		{
			m_iAttackPlotX = pNewValue->getX();
			m_iAttackPlotY = pNewValue->getY();
		}
		else
		{
			m_iAttackPlotX = INVALID_PLOT_COORD;
			m_iAttackPlotY = INVALID_PLOT_COORD;
		}
	}

	m_bAirCombat = bAirCombat;
}


bool CvUnit::isAirCombat() const
{
	VALIDATE_OBJECT
	return m_bAirCombat;
}



int CvUnit::getCombatTimer() const
{
	VALIDATE_OBJECT
	return m_iCombatTimer;
}



void CvUnit::setCombatTimer(int iNewValue)
{
	VALIDATE_OBJECT
	m_iCombatTimer = iNewValue;
	CvAssert(getCombatTimer() >= 0);
}



void CvUnit::changeCombatTimer(int iChange)
{
	VALIDATE_OBJECT
	setCombatTimer(getCombatTimer() + iChange);
}



int CvUnit::getCombatFirstStrikes() const
{
	VALIDATE_OBJECT
	return m_iCombatFirstStrikes;
}



void CvUnit::setCombatFirstStrikes(int iNewValue)
{
	VALIDATE_OBJECT
	m_iCombatFirstStrikes = iNewValue;
	CvAssert(getCombatFirstStrikes() >= 0);
}



void CvUnit::changeCombatFirstStrikes(int iChange)
{
	VALIDATE_OBJECT
	setCombatFirstStrikes(getCombatFirstStrikes() + iChange);
}



bool CvUnit::IsGarrisoned(void) const
{
	VALIDATE_OBJECT

	if (m_iMapLayer != DEFAULT_UNIT_MAP_LAYER)
		return false;

	CvPlot* pPlot = plot();
	if(pPlot)
	{
		CvCity* pCity = pPlot->getPlotCity();
		if(pCity)
		{
			if(pPlot->getOwner() == getOwner())
			{
				UnitHandle bestDefender = pPlot->getBestDefender(getOwner());
				if(bestDefender && bestDefender->GetID() == GetID())
				{
					return true;
				}
			}
		}
	}
	return false;
}


CvCity* CvUnit::GetGarrisonedCity()
{
	if(IsGarrisoned())
	{
		CvPlot* pPlot = plot();
		if(pPlot)
		{
			CvCity* pCity = pPlot->getPlotCity();
			if(pCity)
			{
				return pCity;
			}
		}
	}

	return NULL;
}


int CvUnit::getFortifyTurns() const
{
	VALIDATE_OBJECT
	return m_iFortifyTurns;
}



void CvUnit::setFortifyTurns(int iNewValue)
{
	VALIDATE_OBJECT
	iNewValue = range(iNewValue, 0, GC.getMAX_FORTIFY_TURNS());

	if(iNewValue != getFortifyTurns())
	{

		if(getFortifyTurns() == 0 && iNewValue > 0)
		{
			auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
			gDLL->GameplayUnitFortify(pDllUnit.get(), true);
		}

		m_iFortifyTurns = iNewValue;
		setInfoBarDirty(true);


		if(iNewValue == 0)
		{
			SetFortifiedThisTurn(false);

			auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
			gDLL->GameplayUnitFortify(pDllUnit.get(), false);
		}
	}
}



void CvUnit::changeFortifyTurns(int iChange)
{
	VALIDATE_OBJECT
	setFortifyTurns(getFortifyTurns() + iChange);
}



bool CvUnit::IsFortifiedThisTurn() const
{
	VALIDATE_OBJECT
	return m_bFortifiedThisTurn;
}



void CvUnit::SetFortifiedThisTurn(bool bValue)
{
	VALIDATE_OBJECT
	if(!IsEverFortifyable() && bValue)
	{
		return;
	}

#ifdef NQM_UNIT_FIX_FORTIFY_BONUS_RECEIVED_END_OF_TURN_NOT_INSTANTLY

	if (m_bFortifiedThisTurn != bValue)
	{
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitFortify(pDllUnit.get(), bValue);
	}

	m_bFortifiedThisTurn = bValue;
#else
	if(IsFortifiedThisTurn() != bValue)
	{
		m_bFortifiedThisTurn = bValue;

		if(bValue)
		{
			int iTurnsToFortify = 1;
			if(!isFortifyable())
			{
				iTurnsToFortify = 0;
			}


			setFortifyTurns(iTurnsToFortify);

			if(iTurnsToFortify > 0)
			{
				auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
				gDLL->GameplayUnitFortify(pDllUnit.get(), true);
			}
		}
	}
#endif
}



int CvUnit::getBlitzCount() const
{
	VALIDATE_OBJECT
	return m_iBlitzCount;
}



bool CvUnit::isBlitz() const
{
	VALIDATE_OBJECT
	if(GetRange() > 0)
	{
		return false;
	}
	return (getBlitzCount() > 0);
}



void CvUnit::changeBlitzCount(int iChange)
{
	VALIDATE_OBJECT
	m_iBlitzCount = (m_iBlitzCount + iChange);
	CvAssert(getBlitzCount() >= 0);
}



int CvUnit::getAmphibCount() const
{
	VALIDATE_OBJECT
	return m_iAmphibCount;
}



bool CvUnit::isAmphib() const
{
	VALIDATE_OBJECT
	return (getAmphibCount() > 0);
}



void CvUnit::changeAmphibCount(int iChange)
{
	VALIDATE_OBJECT
	m_iAmphibCount = (m_iAmphibCount + iChange);
	CvAssert(getAmphibCount() >= 0);
}



int CvUnit::getRiverCrossingNoPenaltyCount() const
{
	VALIDATE_OBJECT
	return m_iRiverCrossingNoPenaltyCount;
}



bool CvUnit::isRiverCrossingNoPenalty() const
{
	VALIDATE_OBJECT
	return (getRiverCrossingNoPenaltyCount() > 0);
}



void CvUnit::changeRiverCrossingNoPenaltyCount(int iChange)
{
	VALIDATE_OBJECT
	m_iRiverCrossingNoPenaltyCount = (m_iRiverCrossingNoPenaltyCount + iChange);
	CvAssert(getRiverCrossingNoPenaltyCount() >= 0);
}



int CvUnit::getEnemyRouteCount() const
{
	VALIDATE_OBJECT
	return m_iEnemyRouteCount;
}



bool CvUnit::isEnemyRoute() const
{
	VALIDATE_OBJECT
	return (getEnemyRouteCount() > 0);
}



void CvUnit::changeEnemyRouteCount(int iChange)
{
	VALIDATE_OBJECT
	m_iEnemyRouteCount = (m_iEnemyRouteCount + iChange);
	CvAssert(getEnemyRouteCount() >= 0);
}



int CvUnit::getAlwaysHealCount() const
{
	VALIDATE_OBJECT
	return m_iAlwaysHealCount;
}



bool CvUnit::isAlwaysHeal() const
{
	VALIDATE_OBJECT
	return (getAlwaysHealCount() > 0);
}



void CvUnit::changeAlwaysHealCount(int iChange)
{
	VALIDATE_OBJECT
	m_iAlwaysHealCount = (m_iAlwaysHealCount + iChange);
	CvAssert(getAlwaysHealCount() >= 0);
}



int CvUnit::getHealOutsideFriendlyCount() const
{
	VALIDATE_OBJECT
	return m_iHealOutsideFriendlyCount;
}



bool CvUnit::isHealOutsideFriendly() const
{
	VALIDATE_OBJECT
	return (getHealOutsideFriendlyCount() > 0);
}



void CvUnit::changeHealOutsideFriendlyCount(int iChange)
{
	VALIDATE_OBJECT
	m_iHealOutsideFriendlyCount = (m_iHealOutsideFriendlyCount + iChange);
	CvAssert(getHealOutsideFriendlyCount() >= 0);
}



int CvUnit::getHillsDoubleMoveCount() const
{
	VALIDATE_OBJECT
	return m_iHillsDoubleMoveCount;
}



bool CvUnit::isHillsDoubleMove() const
{
	VALIDATE_OBJECT
	return (getHillsDoubleMoveCount() > 0);
}



void CvUnit::changeHillsDoubleMoveCount(int iChange)
{
	VALIDATE_OBJECT
	m_iHillsDoubleMoveCount = (m_iHillsDoubleMoveCount + iChange);
	CvAssert(getHillsDoubleMoveCount() >= 0);
}



int CvUnit::getImmuneToFirstStrikesCount() const
{
	VALIDATE_OBJECT
	return m_iImmuneToFirstStrikesCount;
}



void CvUnit::changeImmuneToFirstStrikesCount(int iChange)
{
	VALIDATE_OBJECT
	m_iImmuneToFirstStrikesCount = (m_iImmuneToFirstStrikesCount + iChange);
	CvAssert(getImmuneToFirstStrikesCount() >= 0);
}



int CvUnit::getExtraVisibilityRange() const
{
	VALIDATE_OBJECT
	return m_iExtraVisibilityRange;
}



void CvUnit::changeExtraVisibilityRange(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		CvPlot* pkPlot = plot();
		if (canChangeVisibility())
			pkPlot->changeAdjacentSight(getTeam(), visibilityRange(), false, getSeeInvisibleType(), getFacingDirection(true));
		m_iExtraVisibilityRange = (m_iExtraVisibilityRange + iChange);
		if (canChangeVisibility())
			pkPlot->changeAdjacentSight(getTeam(), visibilityRange(), true, getSeeInvisibleType(), getFacingDirection(true));
	}
}



int CvUnit::getExtraMoves() const
{
	VALIDATE_OBJECT
	return m_iExtraMoves;
}



void CvUnit::changeExtraMoves(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraMoves += iChange;
}



int CvUnit::getExtraNavalMoves() const
{
	VALIDATE_OBJECT
	return m_iExtraNavalMoves;
}



void CvUnit::changeExtraNavalMoves(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraNavalMoves += iChange;
	CvAssert(getExtraNavalMoves() >= 0);
}




int CvUnit::getExtraMoveDiscount() const
{
	VALIDATE_OBJECT
	return m_iExtraMoveDiscount;
}



void CvUnit::changeExtraMoveDiscount(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraMoveDiscount = (m_iExtraMoveDiscount + iChange);
	CvAssert(getExtraMoveDiscount() >= 0);
}



int CvUnit::getExtraRange() const
{
	VALIDATE_OBJECT
	return m_iExtraRange;
}



void CvUnit::changeExtraRange(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraRange += iChange;
}



int CvUnit::getExtraIntercept() const
{
	VALIDATE_OBJECT
	return m_iExtraIntercept;
}



void CvUnit::changeExtraIntercept(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraIntercept += iChange;
}



int CvUnit::getExtraEvasion() const
{
	VALIDATE_OBJECT
	return m_iExtraEvasion;
}



void CvUnit::changeExtraEvasion(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraEvasion += iChange;
}



int CvUnit::getExtraFirstStrikes() const
{
	VALIDATE_OBJECT
	return m_iExtraFirstStrikes;
}



void CvUnit::changeExtraFirstStrikes(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraFirstStrikes = (m_iExtraFirstStrikes + iChange);
	CvAssert(getExtraFirstStrikes() >= 0);
}



int CvUnit::getExtraChanceFirstStrikes() const
{
	VALIDATE_OBJECT
	return m_iExtraChanceFirstStrikes;
}



void CvUnit::changeExtraChanceFirstStrikes(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraChanceFirstStrikes = (m_iExtraChanceFirstStrikes + iChange);
	CvAssert(getExtraChanceFirstStrikes() >= 0);
}



int CvUnit::getExtraWithdrawal() const
{
	VALIDATE_OBJECT
	return m_iExtraWithdrawal;
}



void CvUnit::changeExtraWithdrawal(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraWithdrawal = (m_iExtraWithdrawal + iChange);
	CvAssert(getExtraWithdrawal() >= 0);
}



int CvUnit::getExtraEnemyHeal() const
{
	VALIDATE_OBJECT
	return m_iExtraEnemyHeal;
}



void CvUnit::changeExtraEnemyHeal(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraEnemyHeal = (m_iExtraEnemyHeal + iChange);
}



int CvUnit::getExtraNeutralHeal() const
{
	VALIDATE_OBJECT
	return m_iExtraNeutralHeal;
}



void CvUnit::changeExtraNeutralHeal(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraNeutralHeal = (m_iExtraNeutralHeal + iChange);
}



int CvUnit::getExtraFriendlyHeal() const
{
	VALIDATE_OBJECT
	return m_iExtraFriendlyHeal;
}



void CvUnit::changeExtraFriendlyHeal(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraFriendlyHeal = (m_iExtraFriendlyHeal + iChange);
	CvAssert(getExtraFriendlyHeal() >= 0);
}



int CvUnit::getSameTileHeal() const
{
	VALIDATE_OBJECT
	return m_iSameTileHeal;
}



void CvUnit::changeSameTileHeal(int iChange)
{
	VALIDATE_OBJECT
	m_iSameTileHeal = (m_iSameTileHeal + iChange);
	CvAssert(getSameTileHeal() >= 0);
}



int CvUnit::getAdjacentTileHeal() const
{
	VALIDATE_OBJECT
	return m_iAdjacentTileHeal;
}



void CvUnit::changeAdjacentTileHeal(int iChange)
{
	VALIDATE_OBJECT
	m_iAdjacentTileHeal = (m_iAdjacentTileHeal + iChange);
	CvAssert(getAdjacentTileHeal() >= 0);
}



int CvUnit::getEnemyDamageChance() const
{
	VALIDATE_OBJECT
	return m_iEnemyDamageChance;
}



void CvUnit::changeEnemyDamageChance(int iChange)
{
	VALIDATE_OBJECT
	m_iEnemyDamageChance = (m_iEnemyDamageChance + iChange);
	CvAssert(getEnemyDamageChance() >= 0);
}



int CvUnit::getNeutralDamageChance() const
{
	VALIDATE_OBJECT
	return m_iNeutralDamageChance;
}



void CvUnit::changeNeutralDamageChance(int iChange)
{
	VALIDATE_OBJECT
	m_iNeutralDamageChance = (m_iNeutralDamageChance + iChange);
	CvAssert(getNeutralDamageChance() >= 0);
}


int CvUnit::getEnemyDamage() const
{
	VALIDATE_OBJECT
	return m_iEnemyDamage;
}


void CvUnit::changeEnemyDamage(int iChange)
{
	VALIDATE_OBJECT
	m_iEnemyDamage = (m_iEnemyDamage + iChange);
	CvAssert(getEnemyDamage() >= 0);
}


int CvUnit::getNeutralDamage() const
{
	VALIDATE_OBJECT
	return m_iNeutralDamage;
}


void CvUnit::changeNeutralDamage(int iChange)
{
	VALIDATE_OBJECT
	m_iNeutralDamage = (m_iNeutralDamage + iChange);
	CvAssert(getNeutralDamage() >= 0);
}


int CvUnit::getNearbyEnemyCombatMod() const
{
	VALIDATE_OBJECT
	return m_iNearbyEnemyCombatMod;
}



void CvUnit::changeNearbyEnemyCombatMod(int iChange)
{
	VALIDATE_OBJECT
	m_iNearbyEnemyCombatMod = (m_iNearbyEnemyCombatMod + iChange);
}



int CvUnit::getNearbyEnemyCombatRange() const
{
	VALIDATE_OBJECT
	return m_iNearbyEnemyCombatRange;
}



void CvUnit::changeNearbyEnemyCombatRange(int iChange)
{
	VALIDATE_OBJECT
	m_iNearbyEnemyCombatRange = (m_iNearbyEnemyCombatRange + iChange);
	CvAssert(getNearbyEnemyCombatRange() >= 0);
}



int CvUnit::getExtraCombatPercent() const
{
	VALIDATE_OBJECT
	return m_iExtraCombatPercent;
}



void CvUnit::changeExtraCombatPercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraCombatPercent = (m_iExtraCombatPercent + iChange);

		setInfoBarDirty(true);
	}
}



int CvUnit::getExtraCityAttackPercent() const
{
	VALIDATE_OBJECT
	return m_iExtraCityAttackPercent;
}



void CvUnit::changeExtraCityAttackPercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraCityAttackPercent = (m_iExtraCityAttackPercent + iChange);

		setInfoBarDirty(true);
	}
}



int CvUnit::getExtraCityDefensePercent() const
{
	VALIDATE_OBJECT
	return m_iExtraCityDefensePercent;
}



void CvUnit::changeExtraCityDefensePercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraCityDefensePercent = (m_iExtraCityDefensePercent + iChange);

		setInfoBarDirty(true);
	}
}



int CvUnit::getExtraRangedDefenseModifier() const
{
	VALIDATE_OBJECT
	return m_iExtraRangedDefenseModifier;
}



void CvUnit::changeExtraRangedDefenseModifier(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraRangedDefenseModifier = (m_iExtraRangedDefenseModifier + iChange);

		setInfoBarDirty(true);
	}
}



int CvUnit::getExtraHillsAttackPercent() const
{
	VALIDATE_OBJECT
	return m_iExtraHillsAttackPercent;
}



void CvUnit::changeExtraHillsAttackPercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraHillsAttackPercent = (m_iExtraHillsAttackPercent + iChange);

		setInfoBarDirty(true);
	}
}



int CvUnit::getExtraHillsDefensePercent() const
{
	VALIDATE_OBJECT
	return m_iExtraHillsDefensePercent;
}



void CvUnit::changeExtraHillsDefensePercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraHillsDefensePercent = (m_iExtraHillsDefensePercent + iChange);

		setInfoBarDirty(true);
	}
}



int CvUnit::getExtraOpenAttackPercent() const
{
	VALIDATE_OBJECT
	return m_iExtraOpenAttackPercent;
}



void CvUnit::changeExtraOpenAttackPercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraOpenAttackPercent = (m_iExtraOpenAttackPercent + iChange);

		setInfoBarDirty(true);
	}
}



int CvUnit::getExtraOpenRangedAttackMod() const
{
	VALIDATE_OBJECT
	return m_iExtraOpenRangedAttackMod;
}



void CvUnit::changeExtraOpenRangedAttackMod(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraOpenRangedAttackMod = (m_iExtraOpenRangedAttackMod + iChange);

		setInfoBarDirty(true);
	}
}



int CvUnit::getExtraRoughAttackPercent() const
{
	VALIDATE_OBJECT
	return m_iExtraRoughAttackPercent;
}



void CvUnit::changeExtraRoughAttackPercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraRoughAttackPercent = (m_iExtraRoughAttackPercent + iChange);

		setInfoBarDirty(true);
	}
}



int CvUnit::getExtraRoughRangedAttackMod() const
{
	VALIDATE_OBJECT
	return m_iExtraRoughRangedAttackMod;
}



void CvUnit::changeExtraRoughRangedAttackMod(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraRoughRangedAttackMod = (m_iExtraRoughRangedAttackMod + iChange);

		setInfoBarDirty(true);
	}
}



int CvUnit::getExtraAttackFortifiedMod() const
{
	VALIDATE_OBJECT
	return m_iExtraAttackFortifiedMod;
}



void CvUnit::changeExtraAttackFortifiedMod(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraAttackFortifiedMod = (m_iExtraAttackFortifiedMod + iChange);

		setInfoBarDirty(true);
	}
}



int CvUnit::getExtraAttackWoundedMod() const
{
	VALIDATE_OBJECT
	return m_iExtraAttackWoundedMod;
}



void CvUnit::changeExtraAttackWoundedMod(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraAttackWoundedMod = (m_iExtraAttackWoundedMod + iChange);

		setInfoBarDirty(true);
	}
}


int CvUnit::GetFlankAttackModifier() const
{
	VALIDATE_OBJECT
	return m_iFlankAttackModifier;
}



void CvUnit::ChangeFlankAttackModifier(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iFlankAttackModifier = (m_iFlankAttackModifier + iChange);

		setInfoBarDirty(true);
	}
}


int CvUnit::getExtraOpenDefensePercent() const
{
	VALIDATE_OBJECT
	return m_iExtraOpenDefensePercent;
}



void CvUnit::changeExtraOpenDefensePercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraOpenDefensePercent = (m_iExtraOpenDefensePercent + iChange);

		setInfoBarDirty(true);
	}
}



int CvUnit::getExtraRoughDefensePercent() const
{
	VALIDATE_OBJECT
	return m_iExtraRoughDefensePercent;
}



void CvUnit::changeExtraRoughDefensePercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraRoughDefensePercent = (m_iExtraRoughDefensePercent + iChange);

		setInfoBarDirty(true);
	}
}


void CvUnit::changeExtraAttacks(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iNumAttacks += iChange;

		setInfoBarDirty(true);
	}
}



bool CvUnit::IsNearEnemyCitadel(int& iCitadelDamage)
{
	VALIDATE_OBJECT

	int iCitadelRange = 1;

	CvPlot* pLoopPlot;

	ImprovementTypes eImprovement;
	int iDamage;


#ifdef AUI_HEXSPACE_DX_LOOPS
	for (int iDY = -iCitadelRange; iDY <= iCitadelRange; iDY++)
	{
		int iMaxDX = iCitadelRange - MAX(0, iDY);
		for (int iDX = -iCitadelRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
		{

#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
			pLoopPlot = plotXY(pInPlot->getX(), pInPlot->getY(), iDX, iDY);
#else
			pLoopPlot = plotXY(getX(), getY(), iDX, iDY);
#endif
#else
	for(int iX = -iCitadelRange; iX <= iCitadelRange; iX++)
	{
		for(int iY = -iCitadelRange; iY <= iCitadelRange; iY++)
		{
			pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iX, iY, iCitadelRange);
#endif

			if(pLoopPlot != NULL)
			{
				eImprovement = pLoopPlot->getImprovementType();


				if(eImprovement != NO_IMPROVEMENT && !pLoopPlot->IsImprovementPillaged())
				{
					iDamage = GC.getImprovementInfo(eImprovement)->GetNearbyEnemyDamage();
					if(iDamage != 0)
					{
						if(pLoopPlot->getOwner() != NO_PLAYER)
						{
							if(GET_TEAM(getTeam()).isAtWar(pLoopPlot->getTeam()))
							{
								iCitadelDamage = iDamage;
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



bool CvUnit::IsNearGreatGeneral() const
{
	VALIDATE_OBJECT

	int iGreatGeneralRange =       GC.getGREAT_GENERAL_RANGE();

	CvPlot* pLoopPlot;
	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;


#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX, iDX;
	for (int iDY = -iGreatGeneralRange; iDY <= iGreatGeneralRange; iDY++)
	{
		iMaxDX = iGreatGeneralRange - MAX(0, iDY);
		for (iDX = -iGreatGeneralRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
		{

#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
			pLoopPlot = plotXY(pAtPlot->getX(), pAtPlot->getY(), iDX, iDY);
#else
			pLoopPlot = plotXY(getX(), getY(), iDX, iDY);
#endif
#else
	for(int iX = -iGreatGeneralRange; iX <= iGreatGeneralRange; iX++)
	{
		for(int iY = -iGreatGeneralRange; iY <= iGreatGeneralRange; iY++)
		{
			pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iX, iY, iGreatGeneralRange);
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


						if(pLoopUnit && pLoopUnit->getOwner() == getOwner())
						{

							if(pLoopUnit->IsGreatGeneral() || pLoopUnit->IsGreatAdmiral())
							{

								if(pLoopUnit->getDomainType() == getDomainType())
								{
									return true;
								}
							}
						}
					}
				}
			}
		}
	}

	return false;
}



bool CvUnit::IsStackedGreatGeneral() const
{
	CvPlot* pLoopPlot = plot();
	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;

	if(pLoopPlot != NULL)
	{

		if(pLoopPlot->getNumUnits() > 0)
		{
			pUnitNode = pLoopPlot->headUnitNode();

			while(pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(*pUnitNode);
				pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

				if(pLoopUnit)
				{

#ifdef AUI_UNIT_FIX_GET_STACKED_GREAT_GENERAL_WORKS_WITH_ADMIRAL
					if (pLoopUnit->IsGreatGeneral() || pLoopUnit->IsGreatAdmiral())
#else
					if(pLoopUnit->IsGreatGeneral())
#endif
					{

						if(pLoopUnit->getDomainType() == getDomainType())
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


int CvUnit::GetGreatGeneralStackMovement() const
{
	int iRtnValue = baseMoves() * GC.getMOVE_DENOMINATOR();

	CvPlot* pLoopPlot = plot();
	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;

	if(pLoopPlot != NULL)
	{

		if(pLoopPlot->getNumUnits() > 0)
		{
			pUnitNode = pLoopPlot->headUnitNode();

			while(pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(*pUnitNode);
				pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

				if(pLoopUnit)
				{

					if(pLoopUnit->IsGreatGeneralReceivesMovement())
					{

						if(pLoopUnit->getDomainType() == getDomainType())
						{
							iRtnValue = pLoopUnit->maxMoves();
							break;
						}
					}
				}
			}
		}
	}

	return iRtnValue;
}



int CvUnit::GetEmbarkedUnitStackMovement() const
{
	int iRtnValue = baseMoves() * GC.getMOVE_DENOMINATOR();

	CvPlot* pLoopPlot = plot();
	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;

	if(pLoopPlot != NULL)
	{

		if(pLoopPlot->getNumUnits() > 0)
		{
			pUnitNode = pLoopPlot->headUnitNode();

			while(pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(*pUnitNode);
				pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

				if(pLoopUnit)
				{

					if(pLoopUnit->IsEmbarkedUnitReceivesMovement())
					{



							iRtnValue = pLoopUnit->maxMoves();
							break;

					}
				}
			}
		}
	}

	return iRtnValue;
}

#ifdef LEKMOD_LONGSHIP_ALL_PROMO

int CvUnit::GetLandUnitStackMovement() const
{
	int iRtnValue = baseMoves() * GC.getMOVE_DENOMINATOR();

	CvPlot* pLoopPlot = plot();
	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;

	if (pLoopPlot != NULL)
	{

		if (pLoopPlot->getNumUnits() > 0)
		{
			pUnitNode = pLoopPlot->headUnitNode();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(*pUnitNode);
				pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

				if (pLoopUnit)
				{

					if (pLoopUnit->IsLandUnitReceivesMovement())
					{

						if (pLoopUnit->getDomainType() == getDomainType())
						{
							iRtnValue = pLoopUnit->maxMoves();
							break;
						}
					}

				}
			}
		}
	}

	return iRtnValue;
}
#endif


int CvUnit::GetReverseGreatGeneralModifier()const
{
	VALIDATE_OBJECT

	int iGreatGeneralRange =       GC.getGREAT_GENERAL_RANGE();

	CvPlot* pLoopPlot;
	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;


	for(int iX = -iGreatGeneralRange; iX <= iGreatGeneralRange; iX++)
	{
		for(int iY = -iGreatGeneralRange; iY <= iGreatGeneralRange; iY++)
		{
			pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iX, iY, iGreatGeneralRange);
			if(pLoopPlot != NULL)
			{

				if(pLoopPlot->getNumUnits() > 0)
				{
					pUnitNode = pLoopPlot->headUnitNode();
					while(pUnitNode != NULL)
					{
						pLoopUnit = ::getUnit(*pUnitNode);
						pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

						if(pLoopUnit && GET_TEAM(getTeam()).isAtWar(pLoopUnit->getTeam()))
						{

							int iMod = pLoopUnit->getNearbyEnemyCombatMod();
							if(iMod != 0)
							{

								if (pLoopUnit->getGiveDomain() != NO_DOMAIN && !pLoopUnit->isEmbarked() &&  (pLoopUnit->getGiveDomain() == getDomainType()))
								{

									int iRange = pLoopUnit->getNearbyEnemyCombatRange();
									if(plotDistance(getX(), getY(), pLoopPlot->getX(), pLoopPlot->getY()) <= iRange)
									{
										return iMod;
									}
								}
							}
						}
					}
				}

			}
		}
	}

	return 0;
}


int CvUnit::GetNearbyImprovementModifier(ImprovementTypes* pImprovement)const
{
	VALIDATE_OBJECT
	if (pImprovement != NULL)
	{
		*pImprovement = NO_IMPROVEMENT;
	}
	CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
	int iImprovementRange = kPlayer.GetPlayerTraits()->GetNearbyImprovementBonusRange();
	int iImprovementModifier = kPlayer.GetPlayerTraits()->GetNearbyImprovementCombatBonus();

	if(iImprovementModifier != 0)
	{
		CvPlot* pLoopPlot;


#ifdef AUI_HEXSPACE_DX_LOOPS
		for (int iY = -iImprovementRange; iY <= iImprovementRange; iY++)
		{
			int iMaxDX = iImprovementRange - MAX(0, iY);
			for (int iX = -iImprovementRange - MIN(0, iY); iX <= iMaxDX; iX++)
			{

#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
				pLoopPlot = plotXY(pAtPlot->getX(), pAtPlot->getY(), iX, iY);
#else
				pLoopPlot = plotXY(getX(), getY(), iX, iY);
#endif
#else
		for(int iX = -iImprovementRange; iX <= iImprovementRange; iX++)
		{
			for(int iY = -iImprovementRange; iY <= iImprovementRange; iY++)
			{
				pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iX, iY, iImprovementRange);
#endif

				if(pLoopPlot != NULL)
				{

					if(pLoopPlot->getImprovementType() == kPlayer.GetPlayerTraits()->GetCombatBonusImprovementType())
					{
						if (pImprovement != NULL)
						{
							*pImprovement = pLoopPlot->getImprovementType();
						}
						return iImprovementModifier;
					}
				}
			}
		}
	}

	return 0;
}


bool CvUnit::IsGreatGeneral() const
{
	VALIDATE_OBJECT
	return GetGreatGeneralCount() > 0;
}


int CvUnit::GetGreatGeneralCount() const
{
	VALIDATE_OBJECT
	return m_iGreatGeneralCount;
}


void CvUnit::ChangeGreatGeneralCount(int iChange)
{
	VALIDATE_OBJECT
	m_iGreatGeneralCount += iChange;
}


bool CvUnit::IsGreatAdmiral() const
{
	VALIDATE_OBJECT
	return GetGreatAdmiralCount() > 0;
}


int CvUnit::GetGreatAdmiralCount() const
{
	VALIDATE_OBJECT
	return m_iGreatAdmiralCount;
}


void CvUnit::ChangeGreatAdmiralCount(int iChange)
{
	VALIDATE_OBJECT
	m_iGreatAdmiralCount += iChange;
}


int CvUnit::getGreatGeneralModifier() const
{
	VALIDATE_OBJECT
	return m_iGreatGeneralModifier;
}


void CvUnit::changeGreatGeneralModifier(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iGreatGeneralModifier += iChange;
	}
}


bool CvUnit::IsGreatGeneralReceivesMovement() const
{
	return m_iGreatGeneralReceivesMovementCount > 0;
}


void CvUnit::ChangeGreatGeneralReceivesMovementCount(int iChange)
{
	m_iGreatGeneralReceivesMovementCount += iChange;
}



bool CvUnit::IsEmbarkedUnitReceivesMovement() const
{
	return m_iEmbarkedUnitReceivesMovementCount > 0;
}


void CvUnit::ChangeEmbarkedUnitReceivesMovementCount(int iChange)
{
	m_iEmbarkedUnitReceivesMovementCount += iChange;
}

#ifdef LEKMOD_LONGSHIP_ALL_PROMO

bool CvUnit::IsLandUnitReceivesMovement() const
{
	return m_iLandUnitReceivesMovementCount > 0;
}

void CvUnit::ChangeLandUnitReceivesMovementCount(int iChange)
{
	m_iLandUnitReceivesMovementCount += iChange;
}
#endif
#ifdef NQ_ART_OF_WAR_PROMOTION

int CvUnit::GetGreatGeneralOnOrAdjacentConfersMovement() const
{
	return m_iGreatGeneralOnOrAdjacentConfersMovement;
}

void CvUnit::ChangeGreatGeneralOnOrAdjacentConfersMovement(int iChange)
{
	m_iGreatGeneralOnOrAdjacentConfersMovement += iChange;
}
#endif


int CvUnit::GetGreatGeneralCombatModifier() const
{
	return m_iGreatGeneralCombatModifier;
}


void CvUnit::ChangeGreatGeneralCombatModifier(int iChange)
{
	m_iGreatGeneralCombatModifier += iChange;
}


bool CvUnit::IsIgnoreGreatGeneralBenefit() const
{
	return m_iIgnoreGreatGeneralBenefit > 0;
}


void CvUnit::ChangeIgnoreGreatGeneralBenefitCount(int iChange)
{
	m_iIgnoreGreatGeneralBenefit += iChange;
}


bool CvUnit::IsIgnoreZOC() const
{
	return m_iIgnoreZOC > 0;
}


void CvUnit::ChangeIgnoreZOCCount(int iChange)
{
	m_iIgnoreZOC += iChange;
}


bool CvUnit::IsSapper() const
{
	return (m_iSapperCount > 0);
}


void CvUnit::ChangeSapperCount(int iChange)
{
	m_iSapperCount += iChange;
}



bool CvUnit::IsSappingCity(const CvCity* pTargetCity) const
{
	CvAssertMsg(pTargetCity, "Target city is NULL when checking sapping combat bonus");
	if (!pTargetCity)
	{
		return false;
	}

	if (IsSapper())
	{
		if (IsEnemyCityAdjacent(pTargetCity))
		{
			return true;
		}
	}
	return false;
}



bool CvUnit::IsNearSapper(const CvCity* pTargetCity) const
{
	VALIDATE_OBJECT

	CvAssertMsg(pTargetCity, "Target city is NULL when checking sapping combat bonus");
	if (!pTargetCity)
	{
		return false;
	}

	int iSapperRange = GC.getSAPPER_BONUS_RANGE();

	CvPlot* pLoopPlot;
	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;


#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX, iX;
	for (int iY = -iSapperRange; iY <= iSapperRange; iY++)
	{
		iMaxDX = iSapperRange - MAX(0, iY);
		for (iX = -iSapperRange - MIN(0, iY); iX <= iMaxDX; iX++)
		{

#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
			pLoopPlot = plotXY(pAtPlot->getX(), pAtPlot->getY(), iX, iY);
#else
			pLoopPlot = plotXY(getX(), getY(), iX, iY);
#endif
#else
	for(int iX = -iSapperRange; iX <= iSapperRange; iX++)
	{
		for(int iY = -iSapperRange; iY <= iSapperRange; iY++)
		{
			pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iX, iY, iSapperRange);
#endif

			if(pLoopPlot != NULL)
			{

				if (pLoopPlot->getX() == getX() && pLoopPlot->getY() == getY())
					continue;


				if(pLoopPlot->getNumUnits() > 0)
				{
					pUnitNode = pLoopPlot->headUnitNode();

					while(pUnitNode != NULL)
					{
						pLoopUnit = ::getUnit(*pUnitNode);
						pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);


						if(pLoopUnit && pLoopUnit->getOwner() == getOwner())
						{

							if(pLoopUnit->IsSappingCity(pTargetCity))
							{

								if(pLoopUnit->getDomainType() == getDomainType())
								{
									return true;
								}
							}
						}
					}
				}
			}
		}
	}

	return false;
}

#ifdef LEKMOD_NEW_LUA_METHODS
#ifdef LEKMOD_PROMOTION_ADJACENT_CITY_ATTACK

int CvUnit::GetAdjacentCityAttackModifier() const
{
	return m_iAdjacentCityAttackModifier;
}

void CvUnit::ChangeAdjacentCityAttackModifier(int iChange)
{
	m_iAdjacentCityAttackModifier += iChange;
}
#endif

bool CvUnit::IsNearUnitWithPromotion(PromotionTypes ePromotion, int iRange, bool bSameDomain, bool bSamePlayer) const
{
	VALIDATE_OBJECT

	CvAssertMsg(ePromotion, "Promotion is null!");
	if (!ePromotion || iRange < 1)
	{
		return false;
	}

	CvPlot* pLoopPlot;
	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;


	for (int iX = -iRange; iX <= iRange; iX++)
	{
		for (int iY = -iRange; iY <= iRange; iY++)
		{
			pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iX, iY, iRange);

			if (pLoopPlot != NULL)
			{

				if (pLoopPlot->getNumUnits() > 0)
				{
					pUnitNode = pLoopPlot->headUnitNode();

					while (pUnitNode != NULL)
					{
						pLoopUnit = ::getUnit(*pUnitNode);
						pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);


						if (bSamePlayer && (pLoopUnit->getOwner() != getOwner()))
						{
							continue;
						}

						if (pLoopUnit->isHasPromotion(ePromotion))
						{

							if (bSameDomain && (pLoopUnit->getDomainType() != getDomainType()))
							{
								continue;
							}
							
							return true;
							
						}
					}
				}
			}
		}
	}

	return false;
}

bool CvUnit::IsNearTerrainType(TerrainTypes eTerrainType, int iRange, bool bSameOwner) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eTerrainType > 0, "eTerrainType is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTerrainType < GC.getNumTerrainInfos(), "eTerrainType is expected to be within maximum bounds (invalid eTerrainType)");

	if (eTerrainType == NO_TERRAIN || iRange <= 0)
	{
		return false;
	}

	CvPlot* pLoopPlot;


	for (int iX = -iRange; iX <= iRange; iX++)
	{
		for (int iY = -iRange; iY <= iRange; iY++)
		{
			pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iX, iY, iRange);

			if (pLoopPlot != NULL)
			{	

				if (pLoopPlot->getTerrainType() == eTerrainType)
				{

					if (bSameOwner && pLoopPlot->getOwner() == getOwner())
					{
						continue;
					}
					
					return true;

				}
				else if (eTerrainType == TERRAIN_MOUNTAIN && pLoopPlot->isMountain())
				{

					if (bSameOwner && pLoopPlot->getOwner() == getOwner())
					{
						continue;
					}
					return true;
				}
				else if (eTerrainType == TERRAIN_HILL && pLoopPlot->isHills())
				{

					if (bSameOwner && pLoopPlot->getOwner() == getOwner())
					{
						continue;
					}
					return true;
				}
			}
		}
	}

	return false;
}

bool CvUnit::IsNearFeatureType(FeatureTypes eFeature, int iRange, bool bSameOwner) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eFeature > 0, "eFeature is expected to be non-negative (invalid eFeature)");
	CvAssertMsg(eFeature < GC.getNumFeatureInfos(), "eFeature is expected to be within maximum bounds (invalid eFeature)");

	if (eFeature < 0 || eFeature > GC.getNumFeatureInfos() || iRange <= 0)
	{
		return false;
	}

	CvPlot* pLoopPlot;


	for (int iX = -iRange; iX <= iRange; iX++)
	{
		for (int iY = -iRange; iY <= iRange; iY++)
		{
			pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iX, iY, iRange);

			if (pLoopPlot != NULL)
			{

				if (pLoopPlot->getFeatureType() == eFeature)
				{

					if (bSameOwner && pLoopPlot->getOwner() == getOwner())
					{
						continue;
					}

					return true;

				}
			}
		}
	}

	return false;
}

bool CvUnit::IsNearImprovementType(ImprovementTypes eImprovement, int iRange, bool bSameOwner) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eImprovement >= 0, "eImprovement is expected to be non-negative (invalid eImprovement)");
	if (!eImprovement || iRange < 1)
	{
		return false;
	}
	CvPlot* pLoopPlot;


	for (int iX = -iRange; iX <= iRange; iX++)
	{
		for (int iY = -iRange; iY <= iRange; iY++)
		{
			pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iX, iY, iRange);

			if (pLoopPlot != NULL)
			{

				if (pLoopPlot->getImprovementType() == eImprovement)
				{

					if (bSameOwner && pLoopPlot->getOwner() == getOwner())
					{
						continue;
					}
					return true;
				}
			}
		}
	}
	return false;
}
#endif

bool CvUnit::IsCanHeavyCharge() const
{
	return m_iCanHeavyCharge > 0;
}


void CvUnit::ChangeCanHeavyChargeCount(int iChange)
{
	m_iCanHeavyCharge += iChange;
}

#ifdef NQ_HEAVY_CHARGE_DOWNHILL

int CvUnit::GetHeavyChargeDownhill() const
{
	VALIDATE_OBJECT
	return m_iHeavyChargeDownhill;
}


void CvUnit::ChangeHeavyChargeDownhill(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iHeavyChargeDownhill += iChange;
	}
}
#endif

#ifdef LEKMOD_DIFFERENT_IDEO_COMBAT_BONUS

int CvUnit::GetCombatBonusVsDifferentIdeologyModifier() const
{
	VALIDATE_OBJECT
	return m_iCombatBonusVsDifferentIdeologyModifier;
}


void CvUnit::changeCombatBonusVsDifferentIdeologyModifier(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iCombatBonusVsDifferentIdeologyModifier += iChange;
	}
}

bool CvUnit::isUnitDifferentIdeology(const CvUnit* pOtherUnit) const
{
	VALIDATE_OBJECT
	if (!pOtherUnit)
		return false;

	PlayerTypes eOtherPlayer = pOtherUnit->getOwner();
	PlayerTypes eThisPlayer = getOwner();

	if (eOtherPlayer == NO_PLAYER || eThisPlayer == NO_PLAYER)
		return false;

	PolicyBranchTypes eOtherIdeology = GET_PLAYER(eOtherPlayer).GetPlayerPolicies()->GetLateGamePolicyTree();
	PolicyBranchTypes eThisIdeology = GET_PLAYER(eThisPlayer).GetPlayerPolicies()->GetLateGamePolicyTree();

	if (eOtherIdeology == NO_POLICY_BRANCH_TYPE || eThisIdeology == NO_POLICY_BRANCH_TYPE)
		return false;

	return eOtherIdeology != eThisIdeology;
}
#endif

#ifdef LEKMOD_TOURISM_COMBAT_MOD

int CvUnit::getTourismInfluenceCombatModifierVsUnit(const CvUnit* pOtherUnit) const
{
	VALIDATE_OBJECT
	if(!pOtherUnit || pOtherUnit == this)
	{
		return 0;
	}
	if(!(IsCombatUnit() || IsCanAttackRanged()))
	{
		return 0;
	}

	const CvPlayer& kOwner = GET_PLAYER(getOwner());
	const PlayerTypes eEnemy = pOtherUnit->getOwner();
	const CvPlayer& kEnemyOwner = GET_PLAYER(eEnemy);

	if(!kOwner.isMajorCiv() || !kEnemyOwner.isMajorCiv())
	{
		return 0;
	}
	if(kOwner.getTeam() == kEnemyOwner.getTeam())
	{
		return 0;
	}
	if(!GET_TEAM(kOwner.getTeam()).isHasMet(kEnemyOwner.getTeam()))
	{
		return 0;
	}

	if(GC.isLekmodTourismCombatIdeologyRequired())
	{
		const PolicyBranchTypes eEnemyIdeology = kEnemyOwner.GetPlayerPolicies()->GetLateGamePolicyTree();
		if(eEnemyIdeology == NO_POLICY_BRANCH_TYPE)
		{
			return 0;
		}
	}

	const int iMinInfl = GC.getLekmodTourismCombatMinInfluencePercent();
	const int iMaxInfl = GC.getLekmodTourismCombatMaxInfluencePercent();
	const int iMinBonus = GC.getLekmodTourismCombatMinBonus();
	const int iMaxBonus = GC.getLekmodTourismCombatMaxBonus();

	const int iLifetimeCulture = kEnemyOwner.GetJONSCultureEverGenerated();
	if(iLifetimeCulture <= 0)
	{
		return 0;
	}

	const int iInfluenceOn = kOwner.GetCulture()->GetInfluenceOn(eEnemy);
	const int iPercent = (iInfluenceOn * 100) / iLifetimeCulture;

	if(iPercent < iMinInfl)
	{
		return 0;
	}
	if(iPercent >= iMaxInfl)
	{
		return iMaxBonus;
	}

	const int iSpanInfl = iMaxInfl - iMinInfl;
	if(iSpanInfl <= 0)
	{
		return iMaxBonus;
	}

	const int iSpanBonus = iMaxBonus - iMinBonus;
	const int iNumer = (iPercent - iMinInfl) * iSpanBonus + (iSpanInfl / 2);
	return iMinBonus + (iNumer / iSpanInfl);
}
#endif


int CvUnit::getFriendlyLandsModifier() const
{
	VALIDATE_OBJECT
	return m_iFriendlyLandsModifier;
}


void CvUnit::changeFriendlyLandsModifier(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iFriendlyLandsModifier += iChange;
	}
}


int CvUnit::getFriendlyLandsAttackModifier() const
{
	VALIDATE_OBJECT
	return m_iFriendlyLandsAttackModifier;
}


void CvUnit::changeFriendlyLandsAttackModifier(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iFriendlyLandsAttackModifier += iChange;
	}
}


int CvUnit::getOutsideFriendlyLandsModifier() const
{
	VALIDATE_OBJECT
	return m_iOutsideFriendlyLandsModifier;
}


void CvUnit::changeOutsideFriendlyLandsModifier(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iOutsideFriendlyLandsModifier += iChange;
	}
}

#ifdef NQ_GOLDEN_AGE_FOREIGN_ATTACK_BONUS

int CvUnit::getGoldenAgeForeignAttackBonus() const
{
	VALIDATE_OBJECT
	return m_iGoldenAgeForeignAttackBonus;
}


void CvUnit::changeGoldenAgeForeignAttackBonus(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iGoldenAgeForeignAttackBonus += iChange;
	}
}
#endif

#ifdef NQ_COMBAT_STRENGTH_NEAR_FRIENDLY_MINOR

int CvUnit::getCombatStrengthNearFriendlyMinor() const
{
	VALIDATE_OBJECT
	return m_iCombatStrengthNearFriendlyMinor;
}


void CvUnit::changeCombatStrengthNearFriendlyMinor(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iCombatStrengthNearFriendlyMinor += iChange;
	}
}
#endif




int CvUnit::getPillageChange() const
{
	VALIDATE_OBJECT
	return m_iPillageChange;
}


void CvUnit::changePillageChange(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iPillageChange += iChange;

		setInfoBarDirty(true);
	}
}
void CvUnit::changePillageXPChange(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iPillageXPChange += iChange;
		setInfoBarDirty(true);
	}
}
void CvUnit::changePillageHealChange(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iPillageHealChange += iChange;
		setInfoBarDirty(true);
	}
}

int CvUnit::getUpgradeDiscount() const
{
	VALIDATE_OBJECT
	return m_iUpgradeDiscount;
}


void CvUnit::changeUpgradeDiscount(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iUpgradeDiscount += iChange;

		setInfoBarDirty(true);
	}
}


int CvUnit::getExperiencePercent() const
{
	VALIDATE_OBJECT
	return m_iExperiencePercent;
}


void CvUnit::changeExperiencePercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExperiencePercent += iChange;

		setInfoBarDirty(true);
	}
}


int CvUnit::getKamikazePercent() const
{
	VALIDATE_OBJECT
	return m_iKamikazePercent;
}


void CvUnit::changeKamikazePercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iKamikazePercent += iChange;

		setInfoBarDirty(true);
	}
}


DirectionTypes CvUnit::getFacingDirection(bool checkLineOfSightProperty) const
{
	VALIDATE_OBJECT
	if(checkLineOfSightProperty)
	{
		return NO_DIRECTION;
	}
	else
	{
		return m_eFacingDirection;
	}
}


void CvUnit::setFacingDirection(DirectionTypes facingDirection)
{
	VALIDATE_OBJECT
	if(facingDirection != m_eFacingDirection)
	{

		if (canChangeVisibility())
			plot()->changeAdjacentSight(getTeam(), visibilityRange(), false, getSeeInvisibleType(), getFacingDirection(true));


		m_eFacingDirection = facingDirection;


		if (canChangeVisibility())
			plot()->changeAdjacentSight(getTeam(), visibilityRange(), true, getSeeInvisibleType(), getFacingDirection(true));

		DLLUI->setDirty(ColoredPlots_DIRTY_BIT, true);
	}
}


void CvUnit::rotateFacingDirectionClockwise()
{
	VALIDATE_OBJECT

	DirectionTypes newDirection = (DirectionTypes)((m_eFacingDirection + 1) % NUM_DIRECTION_TYPES);
	setFacingDirection(newDirection);
}


void CvUnit::rotateFacingDirectionCounterClockwise()
{
	VALIDATE_OBJECT

	DirectionTypes newDirection = (DirectionTypes)((m_eFacingDirection + NUM_DIRECTION_TYPES - 1) % NUM_DIRECTION_TYPES);
	setFacingDirection(newDirection);
}


bool CvUnit::isOutOfAttacks() const
{
	VALIDATE_OBJECT


	if(isBlitz())
	{
		return false;
	}

	return m_iAttacksMade >= m_iNumAttacks;
}



void CvUnit::setMadeAttack(bool bNewValue)
{
	VALIDATE_OBJECT
	if(bNewValue)
	{
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
		m_iAttacksMade += 1;
#else
		m_iAttacksMade++;
#endif
	}
	else
	{
		m_iAttacksMade = 0;
	}
}



int CvUnit::GetNumInterceptions() const
{
	VALIDATE_OBJECT
	return m_iNumInterceptions;
}


void CvUnit::ChangeNumInterceptions(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
		m_iNumInterceptions += iChange;
}


bool CvUnit::isOutOfInterceptions() const
{
	VALIDATE_OBJECT
#if defined(NQM_UNIT_NO_AA_INTERCEPT_AFTER_MOVE_BEFORE_TURN_END) || defined(NQM_UNIT_FIGHTER_NO_INTERCEPT_UNTIL_AFTER_TURN_END)
	return isInterceptBlockedUntilEndTurn() || getMadeInterceptionCount() >= GetNumInterceptions();
#else
	return getMadeInterceptionCount() >= GetNumInterceptions();
#endif
}


int CvUnit::getMadeInterceptionCount() const
{
	return m_iMadeInterceptionCount;
}


void CvUnit::setMadeInterception(bool bNewValue)
{
	VALIDATE_OBJECT
	if(bNewValue)
	{
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
		m_iMadeInterceptionCount += 1;
#else
		m_iMadeInterceptionCount++;
#endif
	}
	else
	{
		m_iMadeInterceptionCount = 0;
	}
}



bool CvUnit::isPromotionReady() const
{
	VALIDATE_OBJECT
	return m_bPromotionReady;
}



void CvUnit::setPromotionReady(bool bNewValue)
{
	VALIDATE_OBJECT
	if(isPromotionReady() != bNewValue)
	{
		m_bPromotionReady = bNewValue;

		if(m_bPromotionReady)
		{
			SetAutomateType(NO_AUTOMATE);
			ClearMissionQueue();
			SetActivityType(ACTIVITY_AWAKE);
		}

		if(bNewValue)
		{
			CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
			if(pNotifications)
			{
				CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_UNIT_CAN_GET_PROMOTION");
				CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_UNIT_CAN_GET_PROMOTION");
				pNotifications->Add(NOTIFICATION_UNIT_PROMOTION, strBuffer, strSummary, -1, -1, getUnitType(), GetID());
				if(isHuman() && !GC.getGame().isGameMultiPlayer() && GET_PLAYER(GC.getGame().getActivePlayer()).isLocalPlayer())
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_UNIT_PROMOTE);
				}
			}
		}

		if(IsSelected())
		{
			DLLUI->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}



void CvUnit::testPromotionReady()
{
	VALIDATE_OBJECT
	setPromotionReady((getExperience() >= experienceNeeded()) && canAcquirePromotionAny());
}



bool CvUnit::isDelayedDeath() const
{
	VALIDATE_OBJECT
	return m_bDeathDelay;
}


bool CvUnit::isDelayedDeathExported() const
{
	VALIDATE_OBJECT
	return m_bDeathDelay;
}



void CvUnit::startDelayedDeath()
{
	VALIDATE_OBJECT
	m_bDeathDelay = true;
}




bool CvUnit::doDelayedDeath()
{
	VALIDATE_OBJECT
	if(m_bDeathDelay && !isFighting() && !IsBusy())
	{
		kill(false);
		return true;
	}

	return false;
}



bool CvUnit::isCombatFocus() const
{
	VALIDATE_OBJECT
	return m_bCombatFocus;
}



bool CvUnit::isInfoBarDirty() const
{
	VALIDATE_OBJECT
	return m_bInfoBarDirty;
}



void CvUnit::setInfoBarDirty(bool bNewValue)
{
	VALIDATE_OBJECT

	DLLUI->setDirty(UnitInfo_DIRTY_BIT, bNewValue);
}


bool CvUnit::IsNotConverting() const
{
	VALIDATE_OBJECT
	return m_bNotConverting;
}


void CvUnit::SetNotConverting(bool bNewValue)
{
	VALIDATE_OBJECT
	if(bNewValue != IsNotConverting())
	{
		m_bNotConverting = bNewValue;
	}
}


PlayerTypes CvUnit::getVisualOwner(TeamTypes eForTeam) const
{
	VALIDATE_OBJECT
	if(NO_TEAM == eForTeam)
	{
		eForTeam = GC.getGame().getActiveTeam();
	}

	if(getTeam() != eForTeam && eForTeam != BARBARIAN_TEAM)
	{
		if(isHiddenNationality())
		{
			if(!plot()->isCity())
			{
				return BARBARIAN_PLAYER;
			}
		}
	}

	return getOwner();
}



PlayerTypes CvUnit::getCombatOwner(TeamTypes eForTeam, const CvPlot& whosePlot) const
{
	VALIDATE_OBJECT
	if(eForTeam != NO_TEAM && getTeam() != eForTeam && eForTeam != BARBARIAN_TEAM)
	{
		if(isAlwaysHostile(whosePlot))
		{
			return BARBARIAN_PLAYER;
		}
	}

	return getOwner();
}


TeamTypes CvUnit::getTeam() const
{
	VALIDATE_OBJECT
	return CvPlayer::getTeam( getOwner() );
}


PlayerTypes CvUnit::GetOriginalOwner() const
{
	return m_eOriginalOwner;
}


void CvUnit::SetOriginalOwner(PlayerTypes ePlayer)
{
	m_eOriginalOwner = ePlayer;
}


PlayerTypes CvUnit::getCapturingPlayer() const
{
	VALIDATE_OBJECT
	return m_eCapturingPlayer;
}


void CvUnit::setCapturingPlayer(PlayerTypes eNewValue)
{
	VALIDATE_OBJECT
	m_eCapturingPlayer = eNewValue;
}


bool CvUnit::IsCapturedAsIs() const
{
	VALIDATE_OBJECT
	return m_bCapturedAsIs;
}


void CvUnit::SetCapturedAsIs(bool bSetValue)
{
	VALIDATE_OBJECT
	m_bCapturedAsIs = bSetValue;
}


const UnitTypes CvUnit::getUnitType() const
{
	VALIDATE_OBJECT
	return m_eUnitType;
}


CvUnitEntry& CvUnit::getUnitInfo() const
{
	VALIDATE_OBJECT
	return *m_pUnitInfo;
}



UnitClassTypes CvUnit::getUnitClassType() const
{
	VALIDATE_OBJECT
	return (UnitClassTypes)getUnitInfo().GetUnitClassType();
}


const UnitTypes CvUnit::getLeaderUnitType() const
{
	VALIDATE_OBJECT
	return m_eLeaderUnitType;
}


void CvUnit::setLeaderUnitType(UnitTypes leaderUnitType)
{
	VALIDATE_OBJECT
	if(m_eLeaderUnitType != leaderUnitType)
	{
		m_eLeaderUnitType = leaderUnitType;
	}
}


const InvisibleTypes CvUnit::getInvisibleType() const
{
	VALIDATE_OBJECT
	return m_eInvisibleType;
}


void CvUnit::setInvisibleType(InvisibleTypes InvisibleType)
{
	VALIDATE_OBJECT
	if(m_eInvisibleType != InvisibleType)
	{
		m_eInvisibleType = InvisibleType;
	}
}


const InvisibleTypes CvUnit::getSeeInvisibleType() const
{
	VALIDATE_OBJECT
	return m_eSeeInvisibleType;
}


void CvUnit::setSeeInvisibleType(InvisibleTypes InvisibleType)
{
	VALIDATE_OBJECT
	if(m_eSeeInvisibleType != InvisibleType)
	{
		CvPlot* pPlot = GC.getMap().plotCheckInvalid(getX(), getY());
		if(pPlot && canChangeVisibility())
		{
			pPlot->changeAdjacentSight(getTeam(), visibilityRange(), false, getSeeInvisibleType(), getFacingDirection(true));
		}
		m_eSeeInvisibleType = InvisibleType;
		if(pPlot && canChangeVisibility())
		{
			pPlot->changeAdjacentSight(getTeam(), visibilityRange(), true, getSeeInvisibleType(), getFacingDirection(true));
		}
	}
}


const CvUnit* CvUnit::getCombatUnit() const
{
	VALIDATE_OBJECT
	return getUnit(m_combatUnit);
}


CvUnit* CvUnit::getCombatUnit()
{
	VALIDATE_OBJECT
	return getUnit(m_combatUnit);
}



void CvUnit::setCombatUnit(CvUnit* pCombatUnit, bool bAttacking)
{
	VALIDATE_OBJECT
	if(isCombatFocus())
	{
		DLLUI->setCombatFocus(false);
	}

	if(pCombatUnit != NULL)
	{
		CvAssertMsg(getCombatUnit() == NULL && getCombatCity() == NULL, "Combat Unit or City is not expected to be assigned");
#ifdef AUI_GAME_BETTER_HYBRID_MODE
		PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
		m_bCombatFocus = bAttacking && !(DLLUI->isFocusedWidget()) && ((getOwner() == eActivePlayer) || ((pCombatUnit->getOwner() == eActivePlayer) && (!GET_PLAYER(eActivePlayer).isHuman() || !GC.getGame().isAnySimultaneousTurns() || GET_PLAYER(eActivePlayer).getTurnOrder() != GC.getGame().getCurrentTurnOrderActive())));
#else
		m_bCombatFocus = (bAttacking && !(DLLUI->isFocusedWidget()) && ((getOwner() == GC.getGame().getActivePlayer()) || ((pCombatUnit->getOwner() == GC.getGame().getActivePlayer()) && !(GET_PLAYER(GC.getGame().getActivePlayer()).isSimultaneousTurns()))));
#endif
		m_combatUnit = pCombatUnit->GetIDInfo();
	}
	else
	{
		clearCombat();
	}

	setCombatTimer(0);
	setInfoBarDirty(true);

	if(isCombatFocus())
	{
		DLLUI->setCombatFocus(true);
	}
}


const CvCity* CvUnit::getCombatCity() const
{
	VALIDATE_OBJECT
	return getCity(m_combatCity);
}


CvCity* CvUnit::getCombatCity()
{
	VALIDATE_OBJECT
	return getCity(m_combatCity);
}


void CvUnit::setCombatCity(CvCity* pCombatCity)
{
	VALIDATE_OBJECT
	if(isCombatFocus())
	{
		DLLUI->setCombatFocus(false);
	}

	if(pCombatCity != NULL)
	{
		CvAssertMsg(getCombatUnit() == NULL && getCombatCity() == NULL, "Combat Unit or City is not expected to be assigned");
#ifdef AUI_GAME_BETTER_HYBRID_MODE
		PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
		m_bCombatFocus = !(DLLUI->isFocusedWidget()) && ((getOwner() == eActivePlayer) || ((pCombatCity->getOwner() == eActivePlayer) && (!GET_PLAYER(eActivePlayer).isHuman() || !GC.getGame().isAnySimultaneousTurns() || GET_PLAYER(eActivePlayer).getTurnOrder() != GC.getGame().getCurrentTurnOrderActive())));
#else
		m_bCombatFocus = (!(DLLUI->isFocusedWidget()) && ((getOwner() == GC.getGame().getActivePlayer()) || ((pCombatCity->getOwner() == GC.getGame().getActivePlayer()) && !(GET_PLAYER(GC.getGame().getActivePlayer()).isSimultaneousTurns()))));
#endif
		m_combatCity = pCombatCity->GetIDInfo();
	}
	else
	{
		clearCombat();
	}

	setCombatTimer(0);
	setInfoBarDirty(true);

	if(isCombatFocus())
	{
		DLLUI->setCombatFocus(true);
	}
}


void CvUnit::clearCombat()
{
	VALIDATE_OBJECT
	if(isCombatFocus())
	{
		DLLUI->setCombatFocus(false);
	}

	m_bCombatFocus = false;
	if(getCombatUnit() != NULL || getCombatCity() != NULL)
	{
		CvAssertMsg(plot()->isUnitFighting(), "plot()->isUnitFighting is expected to be true");
		m_combatCity.reset();
		m_combatUnit.reset();
		setCombatFirstStrikes(0);

		if(IsSelected())
		{
			DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
		}

		auto_ptr<ICvPlot1> pDllSelectionPlot(DLLUI->getSelectionPlot());
#ifdef AUI_WARNING_FIXES
		const uint iSelectionPlotIndex = (pDllSelectionPlot.get() != NULL) ? uint(pDllSelectionPlot->GetPlotIndex()) : MAX_UNSIGNED_INT;
#else
		int iSelectionPlotIndex = (pDllSelectionPlot.get() != NULL)? pDllSelectionPlot->GetPlotIndex() : -1;
#endif
		if(plot()->GetPlotIndex() == iSelectionPlotIndex)
		{
			DLLUI->setDirty(PlotListButtons_DIRTY_BIT, true);
		}
	}
}


const CvUnit* CvUnit::getTransportUnit() const
{
	VALIDATE_OBJECT
	return getUnit(m_transportUnit);
}


CvUnit* CvUnit::getTransportUnit()
{
	VALIDATE_OBJECT
	return getUnit(m_transportUnit);
}



bool CvUnit::isCargo() const
{
	VALIDATE_OBJECT
	return (getTransportUnit() != NULL);
}



void CvUnit::setTransportUnit(CvUnit* pTransportUnit)
{
	VALIDATE_OBJECT
	CvUnit* pOldTransportUnit;

	pOldTransportUnit = getTransportUnit();

	if(pOldTransportUnit != pTransportUnit)
	{
		if(pOldTransportUnit != NULL)
		{
			pOldTransportUnit->changeCargo(-1);
		}

		if(pTransportUnit != NULL)
		{
			CvAssertMsg(pTransportUnit->cargoSpaceAvailable(getSpecialUnitType(), getDomainType()) > 0, "Cargo space is expected to be available");

			m_transportUnit = pTransportUnit->GetIDInfo();

			if(getDomainType() != DOMAIN_AIR)
			{
				SetActivityType(ACTIVITY_SLEEP);
			}

			if(GC.getGame().isFinalInitialized())
			{
				finishMoves();
			}

			pTransportUnit->changeCargo(1);
			pTransportUnit->SetActivityType(ACTIVITY_AWAKE);
		}
		else
		{
			m_transportUnit.reset();

			SetActivityType(ACTIVITY_AWAKE);
		}
	}
}



int CvUnit::getExtraDomainModifier(DomainTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_extraDomainModifiers[eIndex];
}



void CvUnit::changeExtraDomainModifier(DomainTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_extraDomainModifiers[eIndex] = (m_extraDomainModifiers[eIndex] + iChange);
}



const CvString CvUnit::getName() const
{
	VALIDATE_OBJECT
	CvString strBuffer;

	if(m_strName.IsEmpty())
	{
		return getUnitInfo().GetDescription();
	}

	Localization::String name = Localization::Lookup(m_strName);
	strBuffer.Format("%s (%s)", name.toUTF8(), getUnitInfo().GetDescription());

	return strBuffer;
}



const char* CvUnit::getNameKey() const
{
	VALIDATE_OBJECT
	return getUnitInfo().GetTextKey();
}



const CvString CvUnit::getNameNoDesc() const
{
	VALIDATE_OBJECT
	return m_strName.GetCString();
}



void CvUnit::setName(CvString strNewValue)
{
	VALIDATE_OBJECT
	gDLL->stripSpecialCharacters(strNewValue);

	m_strName = strNewValue;
	DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
}


GreatWorkType CvUnit::GetGreatWork() const
{
	return m_eGreatWork;
}


void CvUnit::SetGreatWork(GreatWorkType eGreatWork)
{
	VALIDATE_OBJECT
	m_eGreatWork = eGreatWork;
}


int CvUnit::GetTourismBlastStrength() const
{
	return m_iTourismBlastStrength;
}


void CvUnit::SetTourismBlastStrength(int iValue)
{
	m_iTourismBlastStrength = iValue;
}



int CvUnit::GetResearchBulbAmount() const
{
	return m_iResearchBulbAmount;
}



void CvUnit::SetResearchBulbAmount(int iValue)
{
	m_iResearchBulbAmount = iValue;
}

#ifdef DECREASE_BULB_AMOUNT_OVER_TIME
int CvUnit::GetScientistBirthTurn() const
{
	return m_iScientistBirthTurn;
}



void CvUnit::SetScientistBirthTurn(int iValue)
{
	m_iScientistBirthTurn = iValue;
}
#endif
#ifdef PROMOTION_INSTA_HEAL_LOCKED

bool CvUnit::isInstaHealLocked() const
{
	return m_bInstaHealLocked;
}


void CvUnit::setInstaHealLocked(bool bNewValue)
{
	m_bInstaHealLocked = bNewValue;
}

#endif

std::string CvUnit::getScriptData() const
{
	VALIDATE_OBJECT
	return m_strScriptData;
}


#ifdef AUI_WARNING_FIXES
void CvUnit::setScriptData(const std::string& strNewValue)
#else
void CvUnit::setScriptData(std::string strNewValue)
#endif
{
	VALIDATE_OBJECT
	m_strScriptData = strNewValue;
}


int CvUnit::getScenarioData() const
{
	VALIDATE_OBJECT
	return m_iScenarioData;
}


void CvUnit::setScenarioData(int iNewValue)
{
	VALIDATE_OBJECT
	m_iScenarioData = iNewValue;
}
#if defined(FULL_YIELD_FROM_KILLS)
int CvUnit::GetYieldFromKills(YieldTypes eYield) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eYield >= 0, "eYield is expected to be non-negative (invalid Yield)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield is expected to be within maximum bounds (invalid Yield)");
	return m_iYieldFromKills[eYield];
}
void CvUnit::ChangeYieldFromKills(YieldTypes eYield, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eYield >= 0, "eYield is expected to be non-negative (invalid Yield)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield is expected to be within maximum bounds (invalid Yield)");
	m_iYieldFromKills.setAt(eYield, m_iYieldFromKills[eYield] + iChange);
	CvAssert(GetYieldFromKills(eYield) >= 0);
}
int CvUnit::GetKillYieldCap(YieldTypes eYield) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eYield >= 0, "eYield is expected to be non-negative (invalid Yield)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield is expected to be within maximum bounds (invalid Yield)");
	return m_iKillYieldCap[eYield];
}
void CvUnit::ChangeKillYieldCap(YieldTypes eYield, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eYield >= 0, "eYield is expected to be non-negative (invalid Yield)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield is expected to be within maximum bounds (invalid Yield)");
	m_iKillYieldCap.setAt(eYield, m_iKillYieldCap[eYield] + iChange);
	CvAssert(GetKillYieldCap(eYield) >= 0);
}
bool CvUnit::IsKillYieldEraValid(EraTypes eEra) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eEra >= 0, "eEra is expected to be non-negative (invalid Era)");
	CvAssertMsg(eEra < GC.getNumEraInfos(), "eEra is expected to be within maximum bounds (invalid Era)");
	return m_bKillYieldEraValid[eEra];
}
void CvUnit::SetKillYieldEraValid(EraTypes eEra, bool bValid)
{
	VALIDATE_OBJECT
	CvAssertMsg(eEra >= 0, "eEra is expected to be non-negative (invalid Era)");
	CvAssertMsg(eEra < GC.getNumEraInfos(), "eEra is expected to be within maximum bounds (invalid Era)");
	m_bKillYieldEraValid.setAt(eEra, bValid);
}
#endif

int CvUnit::getTerrainDoubleMoveCount(TerrainTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_terrainDoubleMoveCount[eIndex];
}



bool CvUnit::isTerrainDoubleMove(TerrainTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return (getTerrainDoubleMoveCount(eIndex) > 0);
}



void CvUnit::changeTerrainDoubleMoveCount(TerrainTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_terrainDoubleMoveCount.setAt(eIndex, (m_terrainDoubleMoveCount[eIndex] + iChange));
	CvAssert(getTerrainDoubleMoveCount(eIndex) >= 0);
}



int CvUnit::getFeatureDoubleMoveCount(FeatureTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_featureDoubleMoveCount[eIndex];
}



bool CvUnit::isFeatureDoubleMove(FeatureTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return (getFeatureDoubleMoveCount(eIndex) > 0);
}



void CvUnit::changeFeatureDoubleMoveCount(FeatureTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_featureDoubleMoveCount.setAt(eIndex, m_featureDoubleMoveCount[eIndex] + iChange);
	CvAssert(getFeatureDoubleMoveCount(eIndex) >= 0);
}



int CvUnit::getImpassableCount() const
{
	VALIDATE_OBJECT
	int iCount = 0;

	int iNumTerrains = GC.getNumTerrainInfos();
	for(int iI = 0; iI < iNumTerrains; iI++)
	{
		if(isTerrainImpassable((TerrainTypes) iI))
		{
			iCount++;
		}
	}

	int iNumFeatures = GC.getNumFeatureInfos();
	for(int iI = 0; iI < iNumFeatures; iI++)
	{
		if(isFeatureImpassable((FeatureTypes) iI))
		{
			iCount++;
		}
	}

	return iCount;
}



int CvUnit::getTerrainImpassableCount(TerrainTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_terrainImpassableCount[eIndex];
}


void CvUnit::changeTerrainImpassableCount(TerrainTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_terrainImpassableCount.setAt(eIndex, m_terrainImpassableCount[eIndex] + iChange);
	CvAssert(getTerrainImpassableCount(eIndex) >= 0);
}



int CvUnit::getFeatureImpassableCount(FeatureTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_featureImpassableCount[eIndex];
}


void CvUnit::changeFeatureImpassableCount(FeatureTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_featureImpassableCount.setAt(eIndex, m_featureImpassableCount[eIndex] + iChange);
	CvAssert(getFeatureImpassableCount(eIndex) >= 0);
}


int CvUnit::getExtraTerrainAttackPercent(TerrainTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_extraTerrainAttackPercent[eIndex];
}



void CvUnit::changeExtraTerrainAttackPercent(TerrainTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_extraTerrainAttackPercent.setAt(eIndex, m_extraTerrainAttackPercent[eIndex] + iChange);

		setInfoBarDirty(true);
	}
}


int CvUnit::getExtraTerrainDefensePercent(TerrainTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_extraTerrainDefensePercent[eIndex];
}



void CvUnit::changeExtraTerrainDefensePercent(TerrainTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_extraTerrainDefensePercent.setAt(eIndex, m_extraTerrainDefensePercent[eIndex] + iChange);

		setInfoBarDirty(true);
	}
}


int CvUnit::getExtraFeatureAttackPercent(FeatureTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_extraFeatureAttackPercent[eIndex];
}



void CvUnit::changeExtraFeatureAttackPercent(FeatureTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_extraFeatureAttackPercent.setAt(eIndex, m_extraFeatureAttackPercent[eIndex] + iChange);

		setInfoBarDirty(true);
	}
}


int CvUnit::getExtraFeatureDefensePercent(FeatureTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_extraFeatureDefensePercent[eIndex];
}



void CvUnit::changeExtraFeatureDefensePercent(FeatureTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_extraFeatureDefensePercent.setAt(eIndex, m_extraFeatureDefensePercent[eIndex] + iChange);

		setInfoBarDirty(true);
	}
}


int CvUnit::getExtraUnitCombatModifier(UnitCombatTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_extraUnitCombatModifier[eIndex];
}



void CvUnit::changeExtraUnitCombatModifier(UnitCombatTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_extraUnitCombatModifier.setAt(eIndex, m_extraUnitCombatModifier[eIndex] + iChange);
}



int CvUnit::getUnitClassModifier(UnitClassTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_unitClassModifier[eIndex];
}



void CvUnit::changeUnitClassModifier(UnitClassTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_unitClassModifier.setAt(eIndex, m_unitClassModifier[eIndex] + iChange);
}



bool CvUnit::canAcquirePromotion(PromotionTypes ePromotion) const
{
	VALIDATE_OBJECT
	CvAssertMsg(ePromotion >= 0, "ePromotion is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePromotion < GC.getNumPromotionInfos(), "ePromotion is expected to be within maximum bounds (invalid Index)");

	CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
	if(pkPromotionInfo == NULL)
	{
		return false;
	}

	if(isHasPromotion(ePromotion))
	{
		return false;
	}
#if !defined(LEKMOD_RELOCATE_PROMOTION_PREREQ_ORS)

	if(pkPromotionInfo->GetPrereqPromotion() != NO_PROMOTION)
	{
		if(!isHasPromotion((PromotionTypes)(pkPromotionInfo->GetPrereqPromotion())))
		{
			return false;
		}
	}


	bool bLacksOrPrereq = false;

	PromotionTypes ePromotion1 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion1();
	if(ePromotion1 != NO_PROMOTION)
	{
		if(!isHasPromotion(ePromotion1))
			bLacksOrPrereq = true;
	}


	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion2 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion2();
		if(ePromotion2 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion2))
				bLacksOrPrereq = false;
		}
	}


	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion3 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion3();
		if(ePromotion3 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion3))
				bLacksOrPrereq = false;
		}
	}


	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion4 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion4();
		if(ePromotion4 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion4))
				bLacksOrPrereq = false;
		}
	}


	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion5 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion5();
		if(ePromotion5 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion5))
				bLacksOrPrereq = false;
		}
	}


	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion6 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion6();
		if(ePromotion6 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion6))
				bLacksOrPrereq = false;
		}
	}


	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion7 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion7();
		if(ePromotion7 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion7))
				bLacksOrPrereq = false;
		}
	}


	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion8 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion8();
		if(ePromotion8 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion8))
				bLacksOrPrereq = false;
		}
	}


	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion9 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion9();
		if(ePromotion9 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion9))
				bLacksOrPrereq = false;
		}
	}
	if (bLacksOrPrereq)
	{
		return false;
	}
#else
	const std::vector<int>& Prereqs = pkPromotionInfo->GetPromotionPrereqOrs();
	if (!Prereqs.empty())
	{
		bool bHasAny = false;
		for (size_t i = 0; i < Prereqs.size(); ++i)
		{
			const int prereqId = Prereqs[i];
			if (prereqId >= 0 && prereqId < GC.getNumPromotionInfos())
			{
				const PromotionTypes eReq = static_cast<PromotionTypes>(prereqId);
				if (eReq != NO_PROMOTION && isHasPromotion(eReq))
				{
					bHasAny = true;
					break;
				}
			}
		}
		if (!bHasAny)
			return false;
	}
#endif
	

	if(pkPromotionInfo->GetTechPrereq() != NO_TECH)
	{
		if(!(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)(pkPromotionInfo->GetTechPrereq()))))
		{
			return false;
		}
	}

	if(!isPromotionValid(ePromotion))
	{
		return false;
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem) 
	{
		CvLuaArgsHandle args;
		args->Push(((int)getOwner()));
		args->Push(GetID());
		args->Push(ePromotion);

		bool bResult = false;
		if (LuaSupport::CallTestAll(pkScriptSystem, "CanHavePromotion", args.get(), bResult))
		{
			if (bResult == false) 
			{
				return false;
			}
		}
	}

	return true;
}


bool CvUnit::isPromotionValid(PromotionTypes ePromotion) const
{
	VALIDATE_OBJECT

	CvPromotionEntry* promotionInfo = GC.getPromotionInfo(ePromotion);
	if(promotionInfo == NULL)
	{
		return false;
	}

	if(!::isPromotionValid(ePromotion, getUnitType(), true           ))
		return false;


	if(promotionInfo->IsInstaHeal())
	{
#ifdef NQM_UNIT_FIX_NO_DOUBLE_INSTAHEAL_ON_SAME_TURN
		if (!canInstahealThisTurn())
			return false;
#endif
		if(getDamage() == 0)
			return false;
	}


	if(promotionInfo->GetInterceptionCombatModifier() != 0)
	{
#ifdef AUI_UNIT_FIX_ALLOW_COMBO_AIR_COMBAT_PROMOTIONS
		if (maxInterceptionProbability() + promotionInfo->GetInterceptChanceChange() <= 0)
#else
		if(!canAirDefend())
#endif
			return false;
	}


	if(promotionInfo->GetAirSweepCombatModifier() != 0)
	{
#ifdef AUI_UNIT_FIX_ALLOW_COMBO_AIR_COMBAT_PROMOTIONS
		if (!IsAirSweepCapable() && !promotionInfo->IsAirSweepCapable())
#else
		if(!IsAirSweepCapable())
#endif
			return false;
	}


	if(promotionInfo->GetInterceptChanceChange() > 0)
	{
#ifdef AUI_UNIT_FIX_ALLOW_COMBO_AIR_COMBAT_PROMOTIONS
		if (maxInterceptionProbability() >= GC.getMAX_INTERCEPTION_PROBABILITY())
#else
		if(promotionInfo->GetInterceptChanceChange() + maxInterceptionProbability() > GC.getMAX_INTERCEPTION_PROBABILITY())
#endif
			return false;
	}


	if(promotionInfo->GetEvasionChange() > 0)
	{
#ifdef AUI_UNIT_FIX_ALLOW_COMBO_AIR_COMBAT_PROMOTIONS
		if (evasionProbability() >= GC.getMAX_EVASION_PROBABILITY())
#else
		if(promotionInfo->GetEvasionChange() + evasionProbability() > GC.getMAX_EVASION_PROBABILITY())
#endif
			return false;
	}

#ifndef AUI_UNIT_FIX_HOVERING_EMBARK

	if(IsHoveringUnit() && promotionInfo->IsAllowsEmbarkation())
		return false;
#endif

	return true;
}



bool CvUnit::canAcquirePromotionAny() const
{
	VALIDATE_OBJECT


	if(isOutOfAttacks())
		return false;

#ifdef AUI_WARNING_FIXES
	uint iI;
#else
	int iI;
#endif

	for(iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		if(canAcquirePromotion((PromotionTypes)iI))
		{
			return true;
		}
	}

	return false;
}


bool CvUnit::isHasPromotion(PromotionTypes eIndex) const
{
	VALIDATE_OBJECT
	return m_Promotions.HasPromotion(eIndex);
}



void CvUnit::setHasPromotion(PromotionTypes eIndex, bool bNewValue)
{
	VALIDATE_OBJECT
	int iChange;
#ifdef AUI_WARNING_FIXES
	uint iI;
#else
	int iI;
#endif

	if(isHasPromotion(eIndex) != bNewValue)
	{
		CvPromotionEntry& thisPromotion = *GC.getPromotionInfo(eIndex);

		m_Promotions.SetPromotion(eIndex, bNewValue);
		iChange = ((isHasPromotion(eIndex)) ? 1 : -1);

		if(getInvisibleType() == NO_INVISIBLE && thisPromotion.GetInvisibleType() != NO_INVISIBLE)
		{
			setInvisibleType((InvisibleTypes) thisPromotion.GetInvisibleType());
		}
		if(getSeeInvisibleType() == NO_INVISIBLE && thisPromotion.GetSeeInvisibleType() != NO_INVISIBLE)
		{
			setSeeInvisibleType((InvisibleTypes) thisPromotion.GetSeeInvisibleType());
		}

		changeBlitzCount((thisPromotion.IsBlitz()) ? iChange : 0);
		changeAmphibCount((thisPromotion.IsAmphib()) ? iChange : 0);
		changeRiverCrossingNoPenaltyCount((thisPromotion.IsRiver()) ? iChange : 0);
		changeEnemyRouteCount((thisPromotion.IsEnemyRoute()) ? iChange : 0);
		changeRivalTerritoryCount((thisPromotion.IsRivalTerritory()) ? iChange : 0);
		changeMustSetUpToRangedAttackCount((thisPromotion.IsMustSetUpToRangedAttack()) ? iChange : 0);
		changeRangedSupportFireCount((thisPromotion.IsRangedSupportFire()) ? iChange : 0);
		changeAlwaysHealCount((thisPromotion.IsAlwaysHeal()) ? iChange : 0);
		changeHealOutsideFriendlyCount((thisPromotion.IsHealOutsideFriendly()) ? iChange : 0);
		changeHillsDoubleMoveCount((thisPromotion.IsHillsDoubleMove()) ? iChange : 0);
		changeIgnoreTerrainCostCount((thisPromotion.IsIgnoreTerrainCost()) ? iChange : 0);
		ChangeRoughTerrainEndsTurnCount((thisPromotion.IsRoughTerrainEndsTurn()) ? iChange : 0);
		ChangeHoveringUnitCount((thisPromotion.IsHoveringUnit()) ? iChange : 0);
		changeFlatMovementCostCount((thisPromotion.IsFlatMovementCost()) ? iChange : 0);
		changeCanMoveImpassableCount((thisPromotion.IsCanMoveImpassable()) ? iChange : 0);
		changeOnlyDefensiveCount((thisPromotion.IsOnlyDefensive()) ? iChange : 0);
		changeNoDefensiveBonusCount((thisPromotion.IsNoDefensiveBonus()) ? iChange : 0);
#if defined(LEKMOD_NO_FORTIFY_VS_RANGED_PROMO)
		changeNoFortifyVsRangedCount((thisPromotion.IsNoFortifyVsRanged()) ? iChange : 0);
#endif
		changeNoCaptureCount((thisPromotion.IsNoCapture()) ? iChange : 0);
		changeNukeImmuneCount((thisPromotion.IsNukeImmune()) ? iChange: 0);
		changeHiddenNationalityCount((thisPromotion.IsHiddenNationality()) ? iChange: 0);
		changeAlwaysHostileCount((thisPromotion.IsAlwaysHostile()) ? iChange: 0);
		changeNoRevealMapCount((thisPromotion.IsNoRevealMap()) ? iChange: 0);
		ChangeReconCount((thisPromotion.IsRecon()) ? iChange: 0);
		changeCanMoveAllTerrainCount((thisPromotion.CanMoveAllTerrain()) ? iChange: 0);
		changeCanMoveAfterAttackingCount((thisPromotion.IsCanMoveAfterAttacking()) ? iChange: 0);
		ChangeAirSweepCapableCount((thisPromotion.IsAirSweepCapable()) ? iChange: 0);
		ChangeEmbarkAbilityCount((thisPromotion.IsAllowsEmbarkation()) ? iChange: 0);
		ChangeRangeAttackIgnoreLOSCount((thisPromotion.IsRangeAttackIgnoreLOS()) ? iChange: 0);
		ChangeHealIfDefeatExcludeBarbariansCount((thisPromotion.IsHealIfDefeatExcludeBarbarians()) ? iChange: 0);
		changeHealOnPillageCount((thisPromotion.IsHealOnPillage()) ? iChange : 0);
		changeFreePillageMoveCount((thisPromotion.IsFreePillageMoves()) ? iChange: 0);
		ChangeEmbarkAllWaterCount((thisPromotion.IsEmbarkedAllWater()) ? iChange: 0);
		ChangeCityAttackOnlyCount((thisPromotion.IsCityAttackOnly()) ? iChange: 0);
		ChangeCaptureDefeatedEnemyCount((thisPromotion.IsCaptureDefeatedEnemy()) ? iChange: 0);
		ChangeCanHeavyChargeCount((thisPromotion.IsCanHeavyCharge()) ? iChange : 0);
#ifdef NQ_HEAVY_CHARGE_DOWNHILL
		ChangeHeavyChargeDownhill((thisPromotion.GetHeavyChargeDownhill()) * iChange);
#endif
#ifdef LEKMOD_DIFFERENT_IDEO_COMBAT_BONUS
		changeCombatBonusVsDifferentIdeologyModifier((thisPromotion.GetCombatDifferentIdeology()) * iChange);
#endif

		ChangeEmbarkExtraVisibility((thisPromotion.GetEmbarkExtraVisibility()) * iChange);
		ChangeEmbarkDefensiveModifier((thisPromotion.GetEmbarkDefenseModifier()) * iChange);
		ChangeCapitalDefenseModifier((thisPromotion.GetCapitalDefenseModifier()) * iChange);
		ChangeCapitalDefenseFalloff((thisPromotion.GetCapitalDefenseFalloff()) * iChange);
		ChangeCityAttackPlunderModifier((thisPromotion.GetCityAttackPlunderModifier()) *  iChange);
#ifdef LEKMOD_MOVE_PENALTY_CITY_COMBAT
		ChangeCityAttackMovePenalty((thisPromotion.GetCityAttackMovePenalty()) * iChange);
#endif
		ChangeReligiousStrengthLossRivalTerritory((thisPromotion.GetReligiousStrengthLossRivalTerritory()) *  iChange);
		ChangeTradeMissionInfluenceModifier((thisPromotion.GetTradeMissionInfluenceModifier()) * iChange);
		ChangeTradeMissionGoldModifier((thisPromotion.GetTradeMissionGoldModifier()) * iChange);
#if defined(v35_TRAITIFY)
		ChangeNearbyWaterCombatModifier((thisPromotion.GetNearbyWaterCombatModifier()) * iChange);
		ChangeAttackExtraMoves((thisPromotion.GetAttackExtraMoves()) * iChange);
		ChangeKillRefreshMovesCount((thisPromotion.IsKillRefreshMove()) ? iChange : 0);
		ChangeKillRefreshAttacksCount((thisPromotion.IsKillRefreshAttack()) ? iChange : 0);
#endif
		changeDropRange(thisPromotion.GetDropRange() * iChange);
		changeExtraVisibilityRange(thisPromotion.GetVisibilityChange() * iChange);
		changeExtraMoves(thisPromotion.GetMovesChange() * iChange);
		changeExtraMoveDiscount(thisPromotion.GetMoveDiscountChange() * iChange);
		changeExtraNavalMoves(thisPromotion.GetExtraNavalMoves() * iChange);
		changeHPHealedIfDefeatEnemy(thisPromotion.GetHPHealedIfDefeatEnemy() * iChange);
		ChangeGoldenAgeValueFromKills(thisPromotion.GetGoldenAgeValueFromKills() * iChange);
		changeExtraWithdrawal(thisPromotion.GetExtraWithdrawal() * iChange);
		changeExtraRange(thisPromotion.GetRangeChange() * iChange);
		ChangeRangedAttackModifier(thisPromotion.GetRangedAttackModifier() * iChange);
		ChangeInterceptionCombatModifier(thisPromotion.GetInterceptionCombatModifier() * iChange);
		ChangeInterceptionDefenseDamageModifier(thisPromotion.GetInterceptionDefenseDamageModifier() * iChange);
		ChangeAirSweepCombatModifier(thisPromotion.GetAirSweepCombatModifier() * iChange);
		changeExtraIntercept(thisPromotion.GetInterceptChanceChange() * iChange);
		changeExtraEvasion(thisPromotion.GetEvasionChange() * iChange);
		changeExtraEnemyHeal(thisPromotion.GetEnemyHealChange() * iChange);
		changeExtraNeutralHeal(thisPromotion.GetNeutralHealChange() * iChange);
		changeExtraFriendlyHeal(thisPromotion.GetFriendlyHealChange() * iChange);
		changeSameTileHeal(thisPromotion.GetSameTileHealChange() * iChange);
		changeAdjacentTileHeal(thisPromotion.GetAdjacentTileHealChange() * iChange);
		changeEnemyDamageChance(thisPromotion.GetEnemyDamageChance() * iChange);
		changeNeutralDamageChance(thisPromotion.GetNeutralDamageChance() * iChange);
		changeEnemyDamage(thisPromotion.GetEnemyDamage() * iChange);
		changeNeutralDamage(thisPromotion.GetNeutralDamage() * iChange);
		changeNearbyEnemyCombatMod(thisPromotion.GetNearbyEnemyCombatMod() * iChange);
		changeNearbyEnemyCombatRange(thisPromotion.GetNearbyEnemyCombatRange() * iChange);
		ChangeAdjacentModifier(thisPromotion.GetAdjacentMod() * iChange);
		changeAttackModifier(thisPromotion.GetAttackMod() * iChange);
		changeDefenseModifier(thisPromotion.GetDefenseMod() * iChange);
		changeExtraCombatPercent(thisPromotion.GetCombatPercent() * iChange);
		changeExtraCityAttackPercent(thisPromotion.GetCityAttackPercent() * iChange);
		changeExtraCityDefensePercent(thisPromotion.GetCityDefensePercent() * iChange);
		changeExtraRangedDefenseModifier(thisPromotion.GetRangedDefenseMod() * iChange);
		changeExtraHillsAttackPercent(thisPromotion.GetHillsAttackPercent() * iChange);
		changeExtraHillsDefensePercent(thisPromotion.GetHillsDefensePercent() * iChange);
		changeExtraOpenAttackPercent(thisPromotion.GetOpenAttackPercent() * iChange);
		changeExtraOpenRangedAttackMod(thisPromotion.GetOpenRangedAttackMod() * iChange);
		changeExtraRoughAttackPercent(thisPromotion.GetRoughAttackPercent() * iChange);
		changeExtraRoughRangedAttackMod(thisPromotion.GetRoughRangedAttackMod() * iChange);
		changeExtraAttackFortifiedMod(thisPromotion.GetAttackFortifiedMod() * iChange);
		changeExtraAttackWoundedMod(thisPromotion.GetAttackWoundedMod() * iChange);
		ChangeFlankAttackModifier(thisPromotion.GetFlankAttackModifier() * iChange);
		changeExtraOpenDefensePercent(thisPromotion.GetOpenDefensePercent() * iChange);
		changeExtraRoughDefensePercent(thisPromotion.GetRoughDefensePercent() * iChange);
		changeExtraAttacks(thisPromotion.GetExtraAttacks() * iChange);
		ChangeNumInterceptions(thisPromotion.GetNumInterceptionChange() * iChange);

		ChangeGreatGeneralCount(thisPromotion.IsGreatGeneral() ? iChange: 0);
		ChangeGreatAdmiralCount(thisPromotion.IsGreatAdmiral() ? iChange: 0);
		changeGreatGeneralModifier(thisPromotion.GetGreatGeneralModifier() * iChange);
		ChangeGreatGeneralReceivesMovementCount(thisPromotion.IsGreatGeneralReceivesMovement() ? iChange: 0);
		ChangeEmbarkedUnitReceivesMovementCount(thisPromotion.IsEmbarkedUnitReceivesMovement() ? iChange : 0);
#ifdef LEKMOD_LONGSHIP_ALL_PROMO
		ChangeLandUnitReceivesMovementCount(thisPromotion.IsLandUnitReceivesMovement() ? iChange : 0);
#endif
#ifdef NQ_ART_OF_WAR_PROMOTION
		ChangeGreatGeneralOnOrAdjacentConfersMovement(thisPromotion.GetGreatGeneralOnOrAdjacentConfersMovement() * iChange);
#endif
		ChangeGreatGeneralCombatModifier(thisPromotion.GetGreatGeneralCombatModifier() * iChange);

		ChangeIgnoreGreatGeneralBenefitCount(thisPromotion.IsIgnoreGreatGeneralBenefit() ? iChange: 0);
		ChangeIgnoreZOCCount(thisPromotion.IsIgnoreZOC() ? iChange: 0);

		ChangeSapperCount((thisPromotion.IsSapper() ? iChange: 0));

		changeFriendlyLandsModifier(thisPromotion.GetFriendlyLandsModifier() * iChange);
		changeFriendlyLandsAttackModifier(thisPromotion.GetFriendlyLandsAttackModifier() * iChange);
		changeOutsideFriendlyLandsModifier(thisPromotion.GetOutsideFriendlyLandsModifier() * iChange);
#ifdef NQ_GOLDEN_AGE_FOREIGN_ATTACK_BONUS
		changeGoldenAgeForeignAttackBonus(thisPromotion.GetGoldenAgeForeignAttackBonus() * iChange);
#endif
#ifdef NQ_COMBAT_STRENGTH_NEAR_FRIENDLY_MINOR
		changeCombatStrengthNearFriendlyMinor(thisPromotion.GetCombatStrengthNearFriendlyMinor() * iChange);
#endif
		changePillageChange(thisPromotion.GetPillageChange() * iChange);
		changePillageXPChange(thisPromotion.GetPillageXPChange() * iChange);
		changePillageHealChange(thisPromotion.GetPillageHealChange() * iChange);
		changeUpgradeDiscount(thisPromotion.GetUpgradeDiscount() * iChange);
		changeExperiencePercent(thisPromotion.GetExperiencePercent() * iChange);
		changeCargoSpace(thisPromotion.GetCargoChange() * iChange);
#if defined(FULL_YIELD_FROM_KILLS)
		for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			ChangeYieldFromKills((YieldTypes)iI, (thisPromotion.GetYieldFromKills(iI) * iChange));
			ChangeKillYieldCap((YieldTypes)iI, (thisPromotion.GetKillYieldCap(iI) * iChange));
		}
		for (iI = 0; iI < GC.getNumEraInfos(); iI++)
		{
			SetKillYieldEraValid((EraTypes)iI, ((thisPromotion.IsKillYieldEraValid(iI)) ? iChange : 0));
		}
#endif
		for(iI = 0; iI < GC.getNumTerrainInfos(); iI++)
		{
			changeExtraTerrainAttackPercent(((TerrainTypes)iI), (thisPromotion.GetTerrainAttackPercent(iI) * iChange));
			changeExtraTerrainDefensePercent(((TerrainTypes)iI), (thisPromotion.GetTerrainDefensePercent(iI) * iChange));
			changeTerrainDoubleMoveCount(((TerrainTypes)iI), ((thisPromotion.GetTerrainDoubleMove(iI)) ? iChange : 0));
			changeTerrainImpassableCount(((TerrainTypes)iI), ((thisPromotion.GetTerrainImpassable(iI)) ? iChange : 0));
		}

		for(iI = 0; iI < GC.getNumFeatureInfos(); iI++)
		{
			changeExtraFeatureAttackPercent(((FeatureTypes)iI), (thisPromotion.GetFeatureAttackPercent(iI) * iChange));
			changeExtraFeatureDefensePercent(((FeatureTypes)iI), (thisPromotion.GetFeatureDefensePercent(iI) * iChange));
			changeFeatureDoubleMoveCount(((FeatureTypes)iI), ((thisPromotion.GetFeatureDoubleMove(iI)) ? iChange : 0));
			changeFeatureImpassableCount(((FeatureTypes)iI), ((thisPromotion.GetFeatureImpassable(iI)) ? iChange : 0));
		}

		for(iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
		{
			changeExtraUnitCombatModifier(((UnitCombatTypes)iI), (thisPromotion.GetUnitCombatModifierPercent(iI) * iChange));
		}

		for(iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo((UnitClassTypes)iI);
			if(!pkUnitClassInfo)
			{
				continue;
			}

			changeUnitClassModifier(((UnitClassTypes)iI), (thisPromotion.GetUnitClassModifierPercent(iI) * iChange));
		}

		for(iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
		{
			changeExtraDomainModifier(((DomainTypes)iI), (thisPromotion.GetDomainModifierPercent(iI) * iChange));
		}
		if (getGiveDomain() == NO_DOMAIN && thisPromotion.GetGiveDomain() != NO_DOMAIN)
		{
			ChangeGiveDomain((DomainTypes)thisPromotion.GetGiveDomain());
		}
		if (getConvertDomain() == NO_DOMAIN && thisPromotion.GetConvertDomain() != NO_DOMAIN)
		{
			ChangeConvertDomain((DomainTypes)thisPromotion.GetConvertDomain());
		}
		if (getConvertDomainUnitType() == NO_UNIT && thisPromotion.GetConvertDomainUnit() != NO_UNIT)
		{
			ChangeConvertDomainUnit((UnitTypes)thisPromotion.GetConvertDomainUnit());
		}

		if(IsSelected())
		{
			DLLUI->setDirty(SelectionButtons_DIRTY_BIT, true);
			DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
			if (thisPromotion.GetEmbarkExtraVisibility() || thisPromotion.IsNoRevealMap() || thisPromotion.GetVisibilityChange())
				GC.getMap().updateDeferredFog();
		}

		PromotionTypes eBuffaloChest =(PromotionTypes) GC.getInfoTypeForString("PROMOTION_BUFFALO_CHEST", true                );
		PromotionTypes eBuffaloLoins =(PromotionTypes) GC.getInfoTypeForString("PROMOTION_BUFFALO_LOINS", true                );

		const PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
		if(getOwner() == eActivePlayer && ((eIndex == eBuffaloChest && isHasPromotion(eBuffaloLoins)) || (eIndex == eBuffaloLoins && isHasPromotion(eBuffaloChest))))
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_XP2_27);
		}
	}
}



int CvUnit::getSubUnitCount() const
{
	VALIDATE_OBJECT
	return getUnitInfo().GetGroupSize();
}



int CvUnit::getSubUnitsAlive() const
{
	VALIDATE_OBJECT
	return getSubUnitsAlive(getDamage());
}



int CvUnit::getSubUnitsAlive(int iDamage) const
{
	VALIDATE_OBJECT
	if(iDamage >= GetMaxHitPoints())
	{
		return 0;
	}
	else
	{
		return std::max(1, (((getUnitInfo().GetGroupSize() * (GetMaxHitPoints() - iDamage)) + (GetMaxHitPoints() / ((getUnitInfo().GetGroupSize() * 2) + 1))) / GetMaxHitPoints()));
	}
}



bool CvUnit::potentialWarAction(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	TeamTypes ePlotTeam = pPlot->getTeam();

	if(ePlotTeam == NO_TEAM)
	{
		return false;
	}

	if(isEnemy(ePlotTeam, pPlot))
	{
		return true;
	}

	if(IsDeclareWar())
	{
		return true;
	}

	return false;
}


bool CvUnit::AreUnitsOfSameType(const CvUnit& pUnit2, const bool bPretendEmbarked) const
{
	VALIDATE_OBJECT

	bool bUnit1isEmbarked = isEmbarked();
	bool bUnit2isEmbarked = pUnit2.isEmbarked() || bPretendEmbarked;


	if(bUnit1isEmbarked && bUnit2isEmbarked)
	{
		return true;
	}

	return CvGameQueries::AreUnitsSameType(getUnitType(), pUnit2.getUnitType());
}


bool CvUnit::CanSwapWithUnitHere(CvPlot& swapPlot) const
{
	VALIDATE_OBJECT
#ifndef AUI_ASTAR_MINOR_OPTIMIZATION
	bool bSwapPossible = false;
#endif

	if(getDomainType() == DOMAIN_LAND || getDomainType() == DOMAIN_SEA)
	{
#ifndef AUI_ASTAR_MINOR_OPTIMIZATION
		if(canEnterTerrain(swapPlot))
#endif
		{
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)



			CvPlot* pHerePlot = plot();
			const bool bAdjacentWalkWaterSwap = pHerePlot != NULL && canMove() &&
				pHerePlot->isAdjacent(&swapPlot) &&
				(pHerePlot->IsAllowsWalkWater() || swapPlot.IsAllowsWalkWater());
#endif

#ifdef AUI_ASTAR_MINOR_OPTIMIZATION
			CvIgnoreUnitsPathFinder& kPathfinder = GC.getIgnoreUnitsPathFinder();
#ifdef AUI_ASTAR_TURN_LIMITER
			if (
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
				bAdjacentWalkWaterSwap ||
#endif
				kPathfinder.DoesPathExist(this, plot(), &swapPlot, 1))
#else
			if (
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
				bAdjacentWalkWaterSwap ||
#endif
				kPathfinder.DoesPathExist(this, plot(), &swapPlot))
#endif
			{
				CvPlot* pEndTurnPlot = kPathfinder.GetPathEndTurnPlot();
#else
			CvUnit* pUnit = (CvUnit*)this;
			if(
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
				bAdjacentWalkWaterSwap ||
#endif
				GC.getIgnoreUnitsPathFinder().DoesPathExist(*(pUnit), plot(), &swapPlot))
			{
				CvPlot* pEndTurnPlot = GC.getIgnoreUnitsPathFinder().GetPathEndTurnPlot();
#endif
				if(
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
					bAdjacentWalkWaterSwap ||
#endif
					pEndTurnPlot == &swapPlot)
				{
					if(swapPlot.getNumFriendlyUnitsOfType(this) >= GC.getPLOT_UNIT_LIMIT())
					{
#ifdef AUI_ASTAR_MINOR_OPTIMIZATION
						const IDInfo* pUnitNode = swapPlot.headUnitNode();
#else
						const IDInfo* pUnitNode;
#endif
						CvUnit* pLoopUnit;
#ifndef AUI_ASTAR_MINOR_OPTIMIZATION
						pUnitNode = swapPlot.headUnitNode();
#endif
						while(pUnitNode != NULL)
						{
							pLoopUnit = (CvUnit*)::getUnit(*pUnitNode);
							pUnitNode = swapPlot.nextUnitNode(pUnitNode);


#ifdef AUI_WARNING_FIXES
							if (!pLoopUnit || pLoopUnit == this)
#else
							if (pLoopUnit == this)
#endif
							{
								continue;
							}


							if(pLoopUnit && pLoopUnit->getOwner() == getOwner())
							{
								if(AreUnitsOfSameType(*pLoopUnit))
								{
#ifndef AUI_ASTAR_MINOR_OPTIMIZATION
									CvPlot* here = plot();
									if(here && pLoopUnit->canEnterTerrain(*here))
#endif
									{

#ifdef AUI_ASTAR_MINOR_OPTIMIZATION
#ifdef AUI_ASTAR_TURN_LIMITER
										if (pLoopUnit->ReadyToMove() &&
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
											(bAdjacentWalkWaterSwap ||
#endif
											kPathfinder.DoesPathExist(pLoopUnit, &swapPlot, plot(), 1)
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
											)
#endif
											)
#else
										if (pLoopUnit->ReadyToMove() &&
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
											(bAdjacentWalkWaterSwap ||
#endif
											kPathfinder.DoesPathExist(pLoopUnit, &swapPlot, plot())
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
											)
#endif
											)
#endif
										{
											CvPlot* pPathEndTurnPlot = kPathfinder.GetPathEndTurnPlot();
#else
										if(pLoopUnit->ReadyToMove() &&
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
											(bAdjacentWalkWaterSwap ||
#endif
											GC.getIgnoreUnitsPathFinder().DoesPathExist(*(pLoopUnit), &swapPlot, plot())
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
											)
#endif
											)
										{
											CvPlot* pPathEndTurnPlot = GC.getIgnoreUnitsPathFinder().GetPathEndTurnPlot();
#endif
											if(
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
												bAdjacentWalkWaterSwap ||
#endif
												pPathEndTurnPlot == plot())
#ifdef AUI_ASTAR_MINOR_OPTIMIZATION
												return true;
#else
											{
												bSwapPossible = true;
												break;
											}
#endif
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

#ifdef AUI_ASTAR_MINOR_OPTIMIZATION
	return false;
#else
	return bSwapPossible;
#endif
}


void CvUnit::read(FDataStream& kStream)
{
	VALIDATE_OBJECT

	reset();


	uint uiVersion;
	kStream >> uiVersion;



	kStream >> m_syncArchive;






	{
		uint idUnitType;
		kStream >> idUnitType;
		if (idUnitType != 0)
		{
			UnitTypes eUnitIndex = (UnitTypes)GC.getInfoTypeForHash(idUnitType);
			if (NO_UNIT != eUnitIndex)
				m_eUnitType = eUnitIndex;
		}
	}

	kStream >> m_iLastMoveTurn;
	m_Promotions.Read(kStream);
	m_pUnitInfo = (NO_UNIT != m_eUnitType) ? GC.getUnitInfo(m_eUnitType) : NULL;

	kStream >> m_combatUnit.eOwner;
	kStream >> m_combatUnit.iID;
	kStream >> m_transportUnit.eOwner;
	kStream >> m_transportUnit.iID;
	kStream >> m_missionAIUnit.eOwner;
	kStream >> m_missionAIUnit.iID;
	kStream >> m_extraDomainModifiers;
	kStream >> m_iEverSelectedCount;
	kStream >> m_iMapLayer;
	if (uiVersion >= 9)
	{
		kStream >> m_iNumGoodyHutsPopped;
	}
	else
	{
		m_iNumGoodyHutsPopped = 0;
	}

	kStream >> m_iCachedPower;

	kStream >> m_bIgnoreDangerWakeup;

	kStream >> m_iEmbarkedAllWaterCount;
	kStream >> m_iEmbarkExtraVisibility;

	kStream >> m_iEmbarkDefensiveModifier;

	kStream >> m_iCapitalDefenseModifier;
	kStream >> m_iCapitalDefenseFalloff;

	kStream >> m_iCityAttackPlunderModifier;
#ifdef LEKMOD_MOVE_PENALTY_CITY_COMBAT
	kStream >> m_iCityAttackMovePenalty;
#endif
	kStream >> m_iReligiousStrengthLossRivalTerritory;

	kStream >> m_iTradeMissionInfluenceModifier;
	kStream >> m_iTradeMissionGoldModifier;
#if defined(v35_TRAITIFY)
	kStream >> m_iNearbyWaterCombatModifier;
	kStream >> m_iAttackExtraMoves;
	kStream >> m_iKillRefreshMovesCount;
	kStream >> m_iKillRefreshAttacksCount;
#endif
	kStream >> m_iEnemyDamageChance;
	kStream >> m_iNeutralDamageChance;

	kStream >> m_iEnemyDamage;
	kStream >> m_iNeutralDamage;

	kStream >> m_iNearbyEnemyCombatMod;
	kStream >> m_iNearbyEnemyCombatRange;

	kStream >> m_iHealOnPillageCount;
	kStream >> m_iFlankAttackModifier;

	if (uiVersion >= 3)
	{
		kStream >> m_iGoldenAgeValueFromKills;
	}
	else
	{
		m_iGoldenAgeValueFromKills = 0;
	}

	kStream >> m_iGreatGeneralReceivesMovementCount;
	kStream >> m_iEmbarkedUnitReceivesMovementCount;
#ifdef LEKMOD_LONGSHIP_ALL_PROMO
	kStream >> m_iLandUnitReceivesMovementCount;
#endif
#ifdef NQ_ART_OF_WAR_PROMOTION
	kStream >> m_iGreatGeneralOnOrAdjacentConfersMovement;
#endif
	kStream >> m_iGreatGeneralCombatModifier;
	kStream >> m_iIgnoreGreatGeneralBenefit;

	if (uiVersion >= 8)
	{
		kStream >> m_iIgnoreZOC;
	}
	else
	{
		m_iIgnoreZOC = 0;
	}

	if (uiVersion >= 2)
	{
		kStream >> m_iSapperCount;
	}
	else
	{
		m_iSapperCount = 0;
	}

	kStream >> m_iCanHeavyCharge;
#ifdef NQ_HEAVY_CHARGE_DOWNHILL
	kStream >> m_iHeavyChargeDownhill;
#endif

	if (uiVersion >= 5)
	{
		kStream >> m_iNumExoticGoods;
	}
	else
	{
		m_iNumExoticGoods = 0;
	}

	kStream >> m_iCityAttackOnlyCount;

	kStream >> m_iCaptureDefeatedEnemyCount;

	kStream >> m_iGreatAdmiralCount;

	kStream >> m_strName;

	kStream >> m_iScenarioData;

	kStream >> *m_pReligion;

	m_eGreatWork = NO_GREAT_WORK;
	if(uiVersion > 3)
	{
		if(uiVersion > 5)
		{
			uint uiGreatWorkType = 0;
			kStream >> uiGreatWorkType;
			
			if (uiGreatWorkType != 0)
			{
				m_eGreatWork = (GreatWorkType)GC.getInfoTypeForHash(uiGreatWorkType);
			}
		}
		else
		{
			CvString strGreatWorkType;
			kStream >> strGreatWorkType;
			m_eGreatWork = static_cast<GreatWorkType>(GC.getInfoTypeForString(strGreatWorkType.c_str()));
		}
	}

	if (uiVersion >= 7)
	{
		kStream >> m_iTourismBlastStrength;
	}
	else
	{
		m_iTourismBlastStrength = 0;
	}

	kStream >> m_iResearchBulbAmount;
#ifdef DECREASE_BULB_AMOUNT_OVER_TIME
	kStream >> m_iScientistBirthTurn;
#endif
#ifdef PROMOTION_INSTA_HEAL_LOCKED
	kStream >> m_bInstaHealLocked;
#endif
#if defined(NQM_UNIT_FIX_NO_DOUBLE_INSTAHEAL_ON_SAME_TURN) || defined(NQM_UNIT_FIX_NO_INSTAHEAL_AFTER_PARADROP)
	kStream >> m_bCanInstahealThisTurn;
#endif
#if defined(NQM_UNIT_NO_AA_INTERCEPT_AFTER_MOVE_BEFORE_TURN_END) || defined(NQM_UNIT_FIGHTER_NO_INTERCEPT_UNTIL_AFTER_TURN_END)
	kStream >> m_bIsInterceptBlockedUntilEndTurn;
#endif
#ifdef AUI_DLLNETMESSAGEHANDLER_FIX_RESPAWN_PROPHET_IF_BEATEN_TO_LAST_RELIGION
	kStream >> m_bIsIgnoreExpended;
#endif


	UINT uSize;
	kStream >> uSize;
	for(UINT uIdx = 0; uIdx < uSize; ++uIdx)
	{
		MissionQueueNode Node;

		kStream >> Node.eMissionType;
		kStream >> Node.iData1;
		kStream >> Node.iData2;
		kStream >> Node.iFlags;
		kStream >> Node.iPushTurn;

		m_missionQueue.insertAtEnd(&Node);
	}


	if (m_iMapLayer != DEFAULT_UNIT_MAP_LAYER)
	{
		GC.getMap().plotManager().AddUnit(GetIDInfo(), m_iX, m_iY, m_iMapLayer);
	}
}



void CvUnit::write(FDataStream& kStream) const
{
	VALIDATE_OBJECT


	uint uiVersion = 10;
	kStream << uiVersion;

	kStream << m_syncArchive;


	if (m_eUnitType != NO_UNIT && m_pUnitInfo)
		kStream << FString::Hash(m_pUnitInfo->GetType());
	else
		kStream << (uint)0;

	kStream << m_iLastMoveTurn;
	m_Promotions.Write(kStream);
	kStream << m_combatUnit.eOwner;
	kStream << m_combatUnit.iID;
	kStream << m_transportUnit.eOwner;
	kStream << m_transportUnit.iID;
	kStream << m_missionAIUnit.eOwner;
	kStream << m_missionAIUnit.iID;
	kStream << m_extraDomainModifiers;
	kStream << m_iEverSelectedCount;
	kStream << m_iMapLayer;
	kStream << m_iNumGoodyHutsPopped;
	kStream << m_iCachedPower;


	kStream << m_bIgnoreDangerWakeup;
	kStream << m_iEmbarkedAllWaterCount;
	kStream << m_iEmbarkExtraVisibility;
	kStream << m_iEmbarkDefensiveModifier;
	kStream << m_iCapitalDefenseModifier;
	kStream << m_iCapitalDefenseFalloff;
	kStream << m_iCityAttackPlunderModifier;
#ifdef LEKMOD_MOVE_PENALTY_CITY_COMBAT
	kStream << m_iCityAttackMovePenalty;
#endif
	kStream << m_iReligiousStrengthLossRivalTerritory;
	kStream << m_iTradeMissionInfluenceModifier;
	kStream << m_iTradeMissionGoldModifier;
#if defined(v35_TRAITIFY)
	kStream << m_iNearbyWaterCombatModifier;
	kStream << m_iAttackExtraMoves;
	kStream << m_iKillRefreshMovesCount;
	kStream << m_iKillRefreshAttacksCount;
#endif
	kStream << m_iEnemyDamageChance;
	kStream << m_iNeutralDamageChance;
	kStream << m_iEnemyDamage;
	kStream << m_iNeutralDamage;
	kStream << m_iNearbyEnemyCombatMod;
	kStream << m_iNearbyEnemyCombatRange;

	kStream << m_iHealOnPillageCount;
	kStream << m_iFlankAttackModifier;
	kStream << m_iGoldenAgeValueFromKills;

	kStream << m_iGreatGeneralReceivesMovementCount;
	kStream << m_iEmbarkedUnitReceivesMovementCount;
#ifdef LEKMOD_LONGSHIP_ALL_PROMO
	kStream << m_iLandUnitReceivesMovementCount;
#endif
#ifdef NQ_ART_OF_WAR_PROMOTION
	kStream << m_iGreatGeneralOnOrAdjacentConfersMovement;
#endif
	kStream << m_iGreatGeneralCombatModifier;
	kStream << m_iIgnoreGreatGeneralBenefit;
	kStream << m_iIgnoreZOC;
	kStream << m_iSapperCount;
	kStream << m_iCanHeavyCharge;
#ifdef NQ_HEAVY_CHARGE_DOWNHILL
	kStream << m_iHeavyChargeDownhill;
#endif
	kStream << m_iNumExoticGoods;
	kStream << m_iCityAttackOnlyCount;
	kStream << m_iCaptureDefeatedEnemyCount;
	kStream << m_iGreatAdmiralCount;

	kStream << m_strName;

	kStream << m_iScenarioData;
	kStream << *m_pReligion;

	if (m_eGreatWork != NO_GREAT_WORK)
	{
		Database::Connection* db = GC.GetGameDatabase();
		Database::Results kQuery;
		if(db && db->Execute(kQuery, "SELECT Type from GreatWorks where ID = ? LIMIT 1"))
		{
			kQuery.Bind(1, m_eGreatWork);

			if(kQuery.Step())
			{
				kStream << FString::Hash(kQuery.GetText(0));
			}
		}
	}
	else
	{
		kStream << (uint)0;
	}

	kStream << m_iTourismBlastStrength;

	kStream << m_iResearchBulbAmount;
#ifdef DECREASE_BULB_AMOUNT_OVER_TIME
	kStream << m_iScientistBirthTurn;
#endif
#ifdef PROMOTION_INSTA_HEAL_LOCKED
	kStream << m_bInstaHealLocked;
#endif

#if defined(NQM_UNIT_FIX_NO_DOUBLE_INSTAHEAL_ON_SAME_TURN) || defined(NQM_UNIT_FIX_NO_INSTAHEAL_AFTER_PARADROP)
	kStream << m_bCanInstahealThisTurn;
#endif
#if defined(NQM_UNIT_NO_AA_INTERCEPT_AFTER_MOVE_BEFORE_TURN_END) || defined(NQM_UNIT_FIGHTER_NO_INTERCEPT_UNTIL_AFTER_TURN_END)
	kStream << m_bIsInterceptBlockedUntilEndTurn;
#endif
#ifdef AUI_DLLNETMESSAGEHANDLER_FIX_RESPAWN_PROPHET_IF_BEATEN_TO_LAST_RELIGION
	kStream << m_bIsIgnoreExpended;
#endif

	kStream << m_missionQueue.getLength();
#ifdef AUI_FIX_FFASTVECTOR_USE_UNSIGNED
	for (UINT uIdx = 0; uIdx < m_missionQueue.getLength(); ++uIdx)
#else
	for(int uIdx = 0; uIdx < m_missionQueue.getLength(); ++uIdx)
#endif
	{
		MissionQueueNode* pNode = m_missionQueue.getAt(uIdx);

		kStream << pNode->eMissionType;
		kStream << pNode->iData1;
		kStream << pNode->iData2;
		kStream << pNode->iFlags;
		kStream << pNode->iPushTurn;
	}
}


bool CvUnit::canRangeStrike() const
{
	VALIDATE_OBJECT
#if defined(v35_TRAITIFY)
	if(isEmbarked() && !GET_PLAYER(getOwner()).GetPlayerTraits()->IsEmbarkedMissionAllowed(static_cast<MissionTypes>(GC.getInfoTypeForString("MISSION_RANGE_ATTACK"))))
#else
	if(isEmbarked())
#endif
	{
		return false;
	}

	if(GetRange() <= 0)
	{
		return false;
	}

	if(GetBaseRangedCombatStrength() <= 0)
	{
		return false;
	}

	if(isMustSetUpToRangedAttack())
	{
		if(!isSetUpForRangedAttack())
		{
			return false;
		}
	}

	if(isOutOfAttacks())
	{
		return false;
	}

#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)

	CvPlot* pPlot = plot();
	if (pPlot && pPlot->IsAllowsWalkWater() && pPlot->HasStackedLandAndNavalUnits())
	{
		return false;
	}
#endif

	return true;
}


bool CvUnit::canEverRangeStrikeAt(int iX, int iY) const
{
	CvPlot* pSourcePlot = plot();
	CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);


	if(NULL == pTargetPlot)
	{
		return false;
	}


	if(!pTargetPlot->isVisible(getTeam()))
	{
		return false;
	}


	if(getUnitInfo().IsRangeAttackOnlyInDomain())
	{
		if(!pTargetPlot->isValidDomainForAction(*this))
		{
			return false;
		}
#if !defined(LEKMOD_SUBMARINE_ATTACK_CHANGES)

		if (pSourcePlot->getArea() != pTargetPlot->getArea())
		{
			return false;
		}
#else
		if (pSourcePlot->isWater() && (pSourcePlot->getArea() != pTargetPlot->getArea()))
		{
			return false;
		}
		if (pSourcePlot->isCity() && getDomainType() == DOMAIN_SEA)
		{
			CvArea* pTargetArea = pTargetPlot->area();
			bool bMatchesAdjacentWaterArea = false;

			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				CvPlot* pAdjacentPlot = plotDirection(pSourcePlot->getX(), pSourcePlot->getY(), (DirectionTypes)iI);
				if (pAdjacentPlot && pAdjacentPlot->isWater())
				{
					if (pAdjacentPlot->area() == pTargetArea)
					{
						bMatchesAdjacentWaterArea = true;
						break;
					}
				}
			}

			if (!bMatchesAdjacentWaterArea)
				return false;
		}
#endif
	}


	if(plotDistance(pSourcePlot->getX(), pSourcePlot->getY(), pTargetPlot->getX(), pTargetPlot->getY()) > GetRange())
	{
		return false;
	}


	if(!IsRangeAttackIgnoreLOS() && getDomainType() != DOMAIN_AIR)
	{
		if(!pSourcePlot->canSeePlot(pTargetPlot, getTeam(), GetRange(), getFacingDirection(true)))
		{
			return false;
		}
	}

	return true;
}


bool CvUnit::canRangeStrikeAt(int iX, int iY, bool bNeedWar, bool bNoncombatAllowed) const
{
	VALIDATE_OBJECT

	if(!canRangeStrike())
	{
		return false;
	}

#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)
	CvPlot* pFromPlot = plot();
	if (pFromPlot && pFromPlot->IsAllowsWalkWater() && pFromPlot->HasStackedLandAndNavalUnits())
	{
		return false;
	}
#endif

	if(!canEverRangeStrikeAt(iX, iY))
	{
		return false;
	}

	CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);


	if(!pTargetPlot->isCity())
	{
		if(bNeedWar)
		{
			const CvUnit* pDefender = airStrikeTarget(*pTargetPlot, bNoncombatAllowed);
			if(NULL == pDefender)
			{
				return false;
			}
		}

		else
		{
			const IDInfo* pUnitNode = pTargetPlot->headUnitNode();
			const CvUnit* pLoopUnit;
			bool bFoundUnit = false;

			CvTeam& myTeam = GET_TEAM(getTeam());

			while(pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(*pUnitNode);
				pUnitNode = pTargetPlot->nextUnitNode(pUnitNode);

				if(!pLoopUnit) continue;

				TeamTypes loopTeam = pLoopUnit->getTeam();


				if(myTeam.isAtWar(loopTeam) || myTeam.canDeclareWar(loopTeam))
				{
					bFoundUnit = true;
					break;
				}
			}

			if(!bFoundUnit)
			{
				return false;
			}
		}
	}

	else
	{
		CvAssert(pTargetPlot->getPlotCity() != NULL);


		if(!atWar(getTeam(), pTargetPlot->getPlotCity()->getTeam()))
		{
			if(bNeedWar)
			{
				return false;
			}

			else
			{
				if(!GET_TEAM(getTeam()).canDeclareWar(pTargetPlot->getPlotCity()->getTeam()))
				{
					return false;
				}
			}
		}
	}

	return true;
}



bool CvUnit::IsAirSweepCapable() const
{
	return GetAirSweepCapableCount() > 0;
}



int CvUnit::GetAirSweepCapableCount() const
{
	return m_iAirSweepCapableCount;
}



void CvUnit::ChangeAirSweepCapableCount(int iChange)
{
	if(iChange != 0)
	{
		m_iAirSweepCapableCount += iChange;
	}
}



bool CvUnit::canAirSweep() const
{
	VALIDATE_OBJECT

	if(!IsAirSweepCapable())
	{
		return false;
	}
#if defined(v35_TRAITIFY)
	if(isEmbarked() && !GET_PLAYER(getOwner()).GetPlayerTraits()->IsEmbarkedMissionAllowed(static_cast<MissionTypes>(GC.getInfoTypeForString("MISSION_AIR_SWEEP"))))
#else
	if(isEmbarked())
#endif
	{
		return false;
	}

	if(GetRange() <= 0)
	{
		return false;
	}

	if(GetBaseRangedCombatStrength() <= 0)
	{
		return false;
	}

	if(isOutOfAttacks())
	{
		return false;
	}

	return true;
}



bool CvUnit::canAirSweepAt(int iX, int iY) const
{
	VALIDATE_OBJECT
	if(!canAirSweep())
	{
		return false;
	}

	CvPlot* pSourcePlot = plot();

	CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);

	if(NULL == pTargetPlot)
	{
		return false;
	}


	if(!pTargetPlot->isVisible(getTeam()))
	{
		return false;
	}


	if(plotDistance(pSourcePlot->getX(), pSourcePlot->getY(), pTargetPlot->getX(), pTargetPlot->getY()) > GetRange())
	{
		return false;
	}

	return true;
}



bool CvUnit::airSweep(int iX, int iY)
{
	if(!canAirSweepAt(iX, iY))
	{
		return false;
	}

	CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);

	CvUnitCombat::AttackAirSweep(*this, *pTargetPlot, CvUnitCombat::ATTACK_OPTION_NONE);

	return true;
}


bool CvUnit::isEnemy(TeamTypes eTeam, const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(NULL == pPlot)
	{
		pPlot = plot();
	}

	if(! pPlot)
	{
		return false;
	}
	return (atWar(GET_PLAYER(getCombatOwner(eTeam, *pPlot)).getTeam(), eTeam));
}


bool CvUnit::isPotentialEnemy(TeamTypes eTeam, const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(NULL == pPlot)
	{
		pPlot = plot();
	}

	if(! pPlot)
	{
		return false;
	}

	return (::isPotentialEnemy(GET_PLAYER(getCombatOwner(eTeam, *pPlot)).getTeam(), eTeam));
}


bool CvUnit::isSuicide() const
{
	VALIDATE_OBJECT
	return (getUnitInfo().IsSuicide() || getKamikazePercent() != 0);
}


int CvUnit::getDropRange() const
{
	VALIDATE_OBJECT
	return m_iDropRange;
}


void CvUnit::changeDropRange(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iDropRange += iChange;
	}
}


bool CvUnit::isTrade() const
{
	VALIDATE_OBJECT
	return getUnitInfo().IsTrade();
}


int CvUnit::getAlwaysHostileCount() const
{
	VALIDATE_OBJECT
	return m_iAlwaysHostileCount;
}



void CvUnit::changeAlwaysHostileCount(int iValue)
{
	VALIDATE_OBJECT
	m_iAlwaysHostileCount += iValue;
	CvAssert(getAlwaysHostileCount() >= 0);
}



bool CvUnit::isAlwaysHostile(const CvPlot& plot) const
{
	VALIDATE_OBJECT
	if(plot.isCity())
	{
		return false;
	}

	return (getAlwaysHostileCount() > 0);
}



int CvUnit::getArmyID() const
{
	VALIDATE_OBJECT
	return m_iArmyId;
}



void CvUnit::setArmyID(int iNewArmyID)
{
	VALIDATE_OBJECT
	m_iArmyId = iNewArmyID;
}


bool CvUnit::IsSelected() const
{
	VALIDATE_OBJECT
	return DLLUI->IsUnitSelected(getOwner(), GetID());
}


bool CvUnit::IsFirstTimeSelected() const
{
	VALIDATE_OBJECT
	if(IsSelected() && m_iEverSelectedCount == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}


void CvUnit::IncrementFirstTimeSelected()
{
	VALIDATE_OBJECT
	if(m_iEverSelectedCount < 2)
	{
		m_iEverSelectedCount++;
	}
}



void CvUnit::QueueMoveForVisualization(CvPlot* pkPlot)
{
	VALIDATE_OBJECT
	if(pkPlot)
	{
		m_unitMoveLocs.push_back(pkPlot);
		if(m_unitMoveLocs.size() == 20)
		{
			PublishQueuedVisualizationMoves();
		}
	}
}


void CvUnit::PublishQueuedVisualizationMoves()
{
	VALIDATE_OBJECT
	auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
	CvPlotIndexVector kPlotArray;
	if(m_unitMoveLocs.size())
	{
		kPlotArray.reserve(m_unitMoveLocs.size());
		for(UnitMovementQueue::const_iterator itr = m_unitMoveLocs.begin(); itr != m_unitMoveLocs.end(); ++itr)
		{
			kPlotArray.push_back((*itr)->GetPlotIndex());
		}
	}
	gDLL->GameplayUnitMoved(pDllUnit.get(), kPlotArray);
	m_unitMoveLocs.clear();
}


void CvUnit::SetPosition(CvPlot* pkPlot)
{
	VALIDATE_OBJECT

	auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
	auto_ptr<ICvPlot1> pDllPlot(new CvDllPlot(pkPlot));
	gDLL->GameplayUnitTeleported(pDllUnit.get(), pDllPlot.get());

	m_unitMoveLocs.clear();
}


const FAutoArchive& CvUnit::getSyncArchive() const
{
	VALIDATE_OBJECT
	return m_syncArchive;
}


FAutoArchive& CvUnit::getSyncArchive()
{
	VALIDATE_OBJECT
	return m_syncArchive;
}


bool CvUnit::IsDoingPartialMove() const
{
	VALIDATE_OBJECT
	const MissionQueueNode* pkMissionNode = HeadMissionQueueNode();
	if(!pkMissionNode)
	{
		return false;
	}

	if(m_kLastPath.size() == 0)
	{
		return false;
	}

	CvPlot* pEndTurnPlot = GetPathEndTurnPlot();
	CvPlot* pEndPathPlot = GetPathLastPlot();

	if(plot() == pEndTurnPlot && plot() != pEndPathPlot && (pkMissionNode->iData1 == pEndPathPlot->getX() && pkMissionNode->iData2 == pEndPathPlot->getY()))
	{
		if(getMoves() < maxMoves() && getMoves() > 0)
		{
			return true;
		}
	}

	return false;
}



ActivityTypes CvUnit::GetActivityType() const
{
	VALIDATE_OBJECT
	return m_eActivityType;
}



void CvUnit::SetActivityType(ActivityTypes eNewValue)
{
	VALIDATE_OBJECT
	CvPlot* pPlot;

	CvAssert(getOwner() != NO_PLAYER);

	ActivityTypes eOldActivity = GetActivityType();

	if(eOldActivity != eNewValue)
	{
		pPlot = plot();

		m_eActivityType = eNewValue;


		if(eNewValue == ACTIVITY_AWAKE)
		{
			setFortifyTurns(0);
		}
#ifdef NQM_UNIT_FIGHTER_NO_INTERCEPT_UNTIL_AFTER_TURN_END
		if (eNewValue == ACTIVITY_INTERCEPT)
		{
			setIsInterceptBlockedUntilEndTurn(true);
		}
#endif

		auto_ptr<ICvPlot1> pDllSelectionPlot(DLLUI->getSelectionPlot());
#ifdef AUI_WARNING_FIXES
		const uint iSelectionPlotIndex = (pDllSelectionPlot.get() != NULL) ? uint(pDllSelectionPlot->GetPlotIndex()) : MAX_UNSIGNED_INT;
#else
		int iSelectionPlotIndex = (pDllSelectionPlot.get() != NULL)? pDllSelectionPlot->GetPlotIndex() : -1;
#endif
		if(pPlot->GetPlotIndex() == iSelectionPlotIndex)
		{
			DLLUI->setDirty(PlotListButtons_DIRTY_BIT, true);
			DLLUI->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}


AutomateTypes CvUnit::GetAutomateType() const
{
	VALIDATE_OBJECT
	return m_eAutomateType;
}



bool CvUnit::IsAutomated() const
{
	VALIDATE_OBJECT
	return (GetAutomateType() != NO_AUTOMATE);
}



void CvUnit::SetAutomateType(AutomateTypes eNewValue)
{
	VALIDATE_OBJECT
	CvAssert(getOwner() != NO_PLAYER);

	if(GetAutomateType() != eNewValue)
	{
		AutomateTypes eOldAutomateType = GetAutomateType();
		m_eAutomateType = eNewValue;

		ClearMissionQueue();
		SetActivityType(ACTIVITY_AWAKE);
		if(eOldAutomateType == AUTOMATE_EXPLORE)
		{
			GET_PLAYER(getOwner()).GetEconomicAI()->m_bExplorationPlotsDirty = true;
		}


		if(eNewValue == NO_AUTOMATE)
		{
			CvPlot* pPlot = plot();
			if(pPlot != NULL)
			{
				IDInfo* pUnitNode = pPlot->headUnitNode();
				while(pUnitNode != NULL)
				{
					CvUnit* pCargoUnit = ::getUnit(*pUnitNode);
					pUnitNode = pPlot->nextUnitNode(pUnitNode);

					CvUnit* pTransportUnit = pCargoUnit->getTransportUnit();
					if(pTransportUnit != NULL && pTransportUnit == this)
					{
						pCargoUnit->SetAutomateType(NO_AUTOMATE);
						pCargoUnit->SetActivityType(ACTIVITY_AWAKE);
					}
				}
			}
		}
		else if(m_eAutomateType == AUTOMATE_EXPLORE)
		{
			GET_PLAYER(getOwner()).GetEconomicAI()->m_bExplorationPlotsDirty = true;
		}
	}
}


bool CvUnit::ReadyToSelect() const
{
	VALIDATE_OBJECT
	return (ReadyToMove() && !IsAutomated());
}



bool CvUnit::ReadyToMove() const
{
	VALIDATE_OBJECT
	if(!canMove())
	{
		return false;
	}

	if(GetLengthMissionQueue() != 0)
	{
		return false;
	}

	if(GetActivityType() != ACTIVITY_AWAKE)
	{
		return false;
	}

	if (GetAutomateType() != NO_AUTOMATE)
	{
		return false;
	}

	if(IsBusy())
	{
		return false;
	}

	return true;
}



bool CvUnit::ReadyToAuto() const
{
	VALIDATE_OBJECT
	return (canMove() && (GetLengthMissionQueue() != 0));
}


bool CvUnit::IsBusy() const
{
	VALIDATE_OBJECT
	if(GetMissionTimer() > 0)
	{
		return true;
	}

	if(isInCombat())
	{
		return true;
	}

	return false;
}


bool CvUnit::SentryAlert() const
{
	VALIDATE_OBJECT
	int iRange = visibilityRange();

	if(iRange > 0)
	{
		CvUnit* pEnemyUnit;

#ifdef AUI_HEXSPACE_DX_LOOPS
		int iMaxDX, iX;
		CvPlot* pPlot;
		for (int iY = -iRange; iY <= iRange; iY++)
		{
			iMaxDX = iRange - MAX(0, iY);
			for (iX = -iRange - MIN(0, iY); iX <= iMaxDX; iX++)
			{

				pPlot = plotXY(getX(), getY(), iX, iY);
#else
		for(int iX = -iRange; iX <= iRange; ++iX)
		{
			for(int iY = -iRange; iY <= iRange; ++iY)
			{
				CvPlot* pPlot = ::plotXYWithRangeCheck(getX(), getY(), iX, iY, iRange);
#endif
				if(NULL != pPlot)
				{

					if(plot()->canSeePlot(pPlot, getTeam(), (iRange - 1), NO_DIRECTION))
					{
						if(pPlot->isVisibleEnemyUnit(this))
						{

							pEnemyUnit = pPlot->getVisibleEnemyDefender(getOwner());
							if(pEnemyUnit)
							{
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


bool CvUnit::ShowMoves() const
{
	VALIDATE_OBJECT
	if(CvPreGame::quickMovement())
	{
		return false;
	}

	for(int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kLoopPlayer.isAlive())
		{
			if(kLoopPlayer.isHuman())
			{
				if(isEnemy(kLoopPlayer.getTeam()))
				{
					return false;
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


bool CvUnit::IsDeclareWar() const
{
	VALIDATE_OBJECT
	if(isHuman())
	{
		return false;
	}
	else
	{
		switch(AI_getUnitAIType())
		{
		case UNITAI_UNKNOWN:
		case UNITAI_SETTLE:
		case UNITAI_WORKER:
			break;
		case UNITAI_CITY_BOMBARD:
			return true;
			break;

		case UNITAI_ATTACK:
		case UNITAI_FAST_ATTACK:
		case UNITAI_PARADROP:
		case UNITAI_DEFENSE:
		case UNITAI_COUNTER:
		case UNITAI_RANGED:
		case UNITAI_CITY_SPECIAL:
		case UNITAI_EXPLORE:
		case UNITAI_ARTIST:
		case UNITAI_SCIENTIST:
		case UNITAI_GENERAL:
		case UNITAI_MERCHANT:
		case UNITAI_ENGINEER:
		case UNITAI_ICBM:
		case UNITAI_WORKER_SEA:
		case UNITAI_SPACESHIP_PART:
		case UNITAI_TREASURE:
		case UNITAI_PROPHET:
		case UNITAI_MISSIONARY:
		case UNITAI_INQUISITOR:
		case UNITAI_ADMIRAL:
		case UNITAI_TRADE_UNIT:
		case UNITAI_ARCHAEOLOGIST:
		case UNITAI_WRITER:
		case UNITAI_MUSICIAN:
			break;

		case UNITAI_ATTACK_SEA:
		case UNITAI_RESERVE_SEA:
		case UNITAI_ESCORT_SEA:
		case UNITAI_EXPLORE_SEA:
		case UNITAI_ASSAULT_SEA:
			break;

		case UNITAI_SETTLER_SEA:
		case UNITAI_CARRIER_SEA:
		case UNITAI_MISSILE_CARRIER_SEA:
		case UNITAI_PIRATE_SEA:
		case UNITAI_ATTACK_AIR:
		case UNITAI_DEFENSE_AIR:
		case UNITAI_CARRIER_AIR:
		case UNITAI_MISSILE_AIR:
			break;

		default:
			CvAssert(false);
			break;
		}
	}

	return false;
}


RouteTypes CvUnit::GetBestBuildRoute(CvPlot* pPlot, BuildTypes* peBestBuild) const
{
	VALIDATE_OBJECT

	if(peBestBuild != NULL)
	{
		*peBestBuild = NO_BUILD;
	}

	int iBestValue = 0;
	RouteTypes eBestRoute = NO_ROUTE;

#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumBuildInfos(); iI++)
#else
	for(int iI = 0; iI < GC.getNumBuildInfos(); iI++)
#endif
	{
		const BuildTypes eBuild = static_cast<BuildTypes>(iI);
		CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
		if(pkBuildInfo)
		{
			const RouteTypes eRoute = (RouteTypes)pkBuildInfo->getRoute();
			if(eRoute != NO_ROUTE)
			{
				CvRouteInfo* pkRouteInfo = GC.getRouteInfo(eRoute);
				if(pkRouteInfo)
				{
					if(canBuild(pPlot, eBuild))
					{
						int iValue = pkRouteInfo->getValue();

						if(iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestRoute = eRoute;
							if(peBestBuild != NULL)
							{
								*peBestBuild = eBuild;
							}
						}
					}
				}
			}
		}
	}

	return eBestRoute;
}


void CvUnit::PlayActionSound()
{
	VALIDATE_OBJECT
}


bool CvUnit::UpdatePathCache(CvPlot* pDestPlot, int iFlags)
{
	bool bGenerated = false;
	CvMap& kMap = GC.getMap();

	if ( (m_iFlags & UNITFLAG_EVALUATING_MISSION) != 0 )
	{

		if (m_kLastPath.size() >= 2)
		{

			CvPlot* pkPathDest;
			if (m_kLastPath[0].GetFlag((int)CvPathNode::PLOT_INVISIBLE) && (pkPathDest = kMap.plot(m_kLastPath[0].m_iX, m_kLastPath[0].m_iY)) != NULL && pkPathDest->isVisible(getTeam()))
			{

				if ((bGenerated = GeneratePath(pkPathDest, iFlags)) == false && pDestPlot != pkPathDest)
				{

					bGenerated = GeneratePath(pDestPlot, iFlags);
				}
			}
			else
			{

				if (m_kLastPath[m_kLastPath.size() - 2].GetFlag((int)CvPathNode::PLOT_INVISIBLE))
				{


					const CvPathNode* pkPathNode = m_kLastPath.GetTurnDest(1);
					if (pkPathNode)
					{
						CvPlot* pkTurnDest = kMap.plot(pkPathNode->m_iX, pkPathNode->m_iY);
						if (pkTurnDest && (bGenerated = GeneratePath(pkTurnDest, iFlags)) == false && pDestPlot != pkTurnDest)
						{

							bGenerated = GeneratePath(pDestPlot, iFlags);
						}
					}
					else
					{
						bGenerated = GeneratePath(pDestPlot, iFlags);
					}
				}
				else
				{

					bGenerated = true;
				}
			}
		}
		else
		{

			if (m_uiLastPathCacheDest != pDestPlot->GetPlotIndex())	
			{
				bGenerated = GeneratePath(pDestPlot, iFlags);
			}
		}
	}
	else
	{

		bGenerated = GeneratePath(pDestPlot, iFlags);
	}
#ifdef FIX_DO_ATTACK_SUBMARINES_IN_SHADOW_OF_WAR
	if (plot()->getNumFriendlyUnitsOfType(this) <= GC.getPLOT_UNIT_LIMIT() && (this)->plot()->isAdjacent(pDestPlot))
	{
		bGenerated = GeneratePath(pDestPlot, iFlags);
	}
#endif
	return bGenerated;
}


bool CvUnit::UnitAttack(int iX, int iY, int iFlags, int iSteps)
{
	VALIDATE_OBJECT
	CvMap& kMap = GC.getMap();
	CvPlot* pDestPlot = kMap.plot(iX, iY);

	CvAssertMsg(pDestPlot != NULL, "DestPlot is not assigned a valid value");
	if(!pDestPlot)
	{
		return false;
	}


	if(isHuman() && getOwner() == GC.getGame().getActivePlayer() && pDestPlot->isVisible(getTeam()))
	{
		TeamTypes eRivalTeam = GetDeclareWarMove(*pDestPlot);

		if(eRivalTeam != NO_TEAM)
		{
			CvPopupInfo kPopup(BUTTONPOPUP_DECLAREWARMOVE);
			kPopup.iData1 = eRivalTeam;
			kPopup.iData2 = pDestPlot->getX();
			kPopup.iData3 = pDestPlot->getY();
			kPopup.bOption1 = false;
			kPopup.bOption2 = pDestPlot->getTeam() != eRivalTeam;
			if(pDestPlot->isCity())
			{
				kPopup.iFlags = DOW_MOVE_ONTO_CITY;
			}

			else if(pDestPlot->isVisibleOtherUnit(m_eOwner))
			{
				kPopup.iFlags = DOW_MOVE_ONTO_UNIT;
			}

			else
			{
				kPopup.iFlags = DOW_MOVE_INTO_TERRITORY;
			}
			DLLUI->AddPopup(kPopup);

			return false;
		}
	}

	const CvPathNodeArray& kPathNodeArray = GetPathNodeArray();

	if(iFlags & MOVE_UNITS_THROUGH_ENEMY)
	{
		if(GeneratePath(pDestPlot, iFlags))
		{
			pDestPlot = GetPathFirstPlot();
		}
	}
	else
	{
		if(getDomainType() != DOMAIN_AIR)
		{
			iSteps += 0;

			UpdatePathCache(pDestPlot, iFlags);
		}
	}

	CvAssertMsg(pDestPlot != NULL, "DestPlot is not assigned a valid value");

	if(!pDestPlot)
	{
		return false;
	}

	bool bAttack = false;
	bool bAdjacent = false;


	if(kPathNodeArray.size() == 0 || getDomainType() == DOMAIN_AIR)
	{

		if((getDomainType() == DOMAIN_AIR) || (plotDistance(getX(), getY(), iX, iY) == 1))
		{
			if((iFlags & MISSION_MODIFIER_DIRECT_ATTACK) || (getDomainType() == DOMAIN_AIR) || (GeneratePath(pDestPlot, iFlags) && (GetPathFirstPlot() == pDestPlot)))
			{
				bAdjacent = true;
			}
		}
	}

	else if(kPathNodeArray.size() != 0)
	{
		if(kPathNodeArray.size() > 1)
		{


			const CvPathNode& kNode = kPathNodeArray[1];
			if(kNode.m_iX == getX() && kNode.m_iY == getY())
			{

				const CvPathNode& kDestNode = kPathNodeArray[0];
				if (kDestNode.m_iX == iX && kDestNode.m_iY == iY)
				{
					bAdjacent = true;
				}
			}
		}
	}

	if(bAdjacent)
	{
		if(!isOutOfAttacks() && (!IsCityAttackOnly() || pDestPlot->isEnemyCity(*this) || !pDestPlot->getBestDefender(NO_PLAYER)))
		{
#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)


			if (getDomainType() != DOMAIN_AIR && !IsCanAttackWithMoveNow())
			{
				if (pDestPlot->isEnemyCity(*this) || pDestPlot->getVisibleEnemyDefender(getOwner()) != NULL)
				{
					return true;
				}
				return false;
			}
#endif

			if(isFighting() || pDestPlot->isFighting())
			{
				return true;
			}


			if(getDomainType() == DOMAIN_AIR && GetBaseCombatStrength() == 0)
			{
				if(canRangeStrikeAt(iX, iY))
				{
					CvUnitCombat::AttackAir(*this, *pDestPlot, (iFlags &  MISSION_MODIFIER_NO_DEFENSIVE_SUPPORT)?CvUnitCombat::ATTACK_OPTION_NO_DEFENSIVE_SUPPORT:CvUnitCombat::ATTACK_OPTION_NONE);
					bAttack = true;
				}
			}


			else if(pDestPlot->isCity())
			{
				if(GET_TEAM(getTeam()).isAtWar(pDestPlot->getPlotCity()->getTeam()))
				{
					if(getDomainType() != DOMAIN_AIR)
					{

#if defined(v35_TRAITIFY)
						if(isRanged() && (isEmbarked() && !GET_PLAYER(getOwner()).GetPlayerTraits()->IsEmbarkedMissionAllowed(static_cast<MissionTypes>(GC.getInfoTypeForString("MISSION_RANGE_ATTACK")))))
#else
						if (isRanged() && isEmbarked())
#endif
						{
							return false;
						}

						CvUnitCombat::AttackCity(*this, *pDestPlot, (iFlags &  MISSION_MODIFIER_NO_DEFENSIVE_SUPPORT)?CvUnitCombat::ATTACK_OPTION_NO_DEFENSIVE_SUPPORT:CvUnitCombat::ATTACK_OPTION_NONE);
						bAttack = true;
					}
				}
			}


			else
			{

				UnitHandle pBestDefender = pDestPlot->getBestDefender(NO_PLAYER, getOwner(), this, true);
				if(!pBestDefender)
				{
					return false;
				}


#if defined(v35_TRAITIFY)
				if(isRanged() && (isEmbarked() && !GET_PLAYER(getOwner()).GetPlayerTraits()->IsEmbarkedMissionAllowed(static_cast<MissionTypes>(GC.getInfoTypeForString("MISSION_RANGE_ATTACK")))))
#else
				if (isRanged() && isEmbarked())
#endif
				{
					return false;
				}

				bAttack = true;
				CvUnitCombat::Attack(*this, *pDestPlot, (iFlags &  MISSION_MODIFIER_NO_DEFENSIVE_SUPPORT)?CvUnitCombat::ATTACK_OPTION_NO_DEFENSIVE_SUPPORT:CvUnitCombat::ATTACK_OPTION_NONE);
			}


			if(pDestPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
			{
				CvBarbarians::DoCampAttacked(pDestPlot);
			}
		}
	}

	return bAttack;
}


bool CvUnit::UnitMove(CvPlot* pPlot, bool bCombat, CvUnit* pCombatUnit, bool bEndMove)
{
	VALIDATE_OBJECT
	bool bCanMoveIntoPlot = false;
	if(canMove() && pPlot)
	{

		if(bCombat && (!(isNoCapture()) || !(pPlot->isEnemyCity(*this))))
		{
			bCanMoveIntoPlot = canMoveOrAttackInto(*pPlot, CvUnit::MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE);
		}
		else	VALIDATE_OBJECT

		{
			bCanMoveIntoPlot = canMoveInto(*pPlot, CvUnit::MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE);
		}
	}
	VALIDATE_OBJECT


	bool bIsNoCapture = isNoCapture();
	bool bEnemyCity = pPlot && pPlot->isEnemyCity(*this);

	bool bIsCombatUnit = (this == pCombatUnit);
	bool bExecuteMove = false;

	if(pPlot && (bCanMoveIntoPlot || (bIsCombatUnit && !(bIsNoCapture && bEnemyCity))))
	{

		LOG_UNIT_MOVES_MESSAGE_OSTR(std::string("UnitMove() : player ") << GET_PLAYER(getOwner()).getName(); << std::string(" ") << getName() << std::string(" id=") << GetID() << std::string(" moving to ") << pPlot->getX() << std::string(", ") << pPlot->getY());
		move(*pPlot, true);
	}
	else
	{
		bExecuteMove = true;
	}


	if(bEndMove || !canMove() || IsDoingPartialMove())
	{
		bExecuteMove = true;
	}

	if(CvPreGame::quickMovement())
	{
		bExecuteMove = false;
	}

	if(bExecuteMove)
	{
		PublishQueuedVisualizationMoves();
	}

	return bCanMoveIntoPlot;
}





int CvUnit::UnitPathTo(int iX, int iY, int iFlags, int iPrevETA, bool bBuildingRoute)
{
	VALIDATE_OBJECT
	CvPlot* pDestPlot;
	CvPlot* pPathPlot = NULL;

	LOG_UNIT_MOVES_MESSAGE_OSTR( std::string("UnitPathTo() : player ") << GET_PLAYER(getOwner()).getName() << std::string(" ") << getName() << std::string(" id=") << GetID() << std::string(" moving to ") << iX << std::string(", ") << iY);

	if(at(iX, iY))
	{
		LOG_UNIT_MOVES_MESSAGE("Already at location");
		return 0;
	}

	CvAssert(!IsBusy());
	CvAssert(getOwner() != NO_PLAYER);

	CvMap& kMap = GC.getMap();
	pDestPlot = kMap.plot(iX, iY);
	CvAssertMsg(pDestPlot != NULL, "DestPlot is not assigned a valid value");
	if(!pDestPlot)
	{
		LOG_UNIT_MOVES_MESSAGE("Destination is not a valid plot location");
		return 0;
	}

	CvAssertMsg(canMove(), "canAllMove is expected to be true");

	if(getDomainType() == DOMAIN_AIR)
	{
		if(!canMoveInto(*pDestPlot))
		{
			return 0;
		}

		pPathPlot = pDestPlot;
		m_kLastPath.clear();
	}
	else
	{
		if(bBuildingRoute)
		{
			if(!GC.GetBuildRouteFinder().GeneratePath(getX(), getY(), iX, iY, getOwner()))
			{
				LOG_UNIT_MOVES_MESSAGE("Unable to generate path with BuildRouteFinder");
				return 0;
			}

			CvAStarNode* pNode = GC.GetBuildRouteFinder().GetLastNode();
			if(pNode)
			{

				while(pNode->m_pParent && pNode->m_pParent->m_pParent)
				{
					pNode = pNode->m_pParent;
				}

				pPathPlot = GC.getMap().plotCheckInvalid(pNode->m_iX, pNode->m_iY);
			}

#ifdef AUI_WARNING_FIXES
			if (!pPathPlot || !canMoveInto(*pPathPlot, byte(iFlags) | MOVEFLAG_DESTINATION))
#else
			if(!pPathPlot || !canMoveInto(*pPathPlot, iFlags | MOVEFLAG_DESTINATION))
#endif
			{

				CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
				if(pNotifications)
				{
					CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ROUTE_TO_CANCELLED");
					CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ROUTE_TO_CANCELLED");
					pNotifications->Add(NOTIFICATION_GENERIC, strBuffer, strSummary, getX(), getY(), -1);
				}

#ifdef LOG_UNIT_MOVES
				if(!pPathPlot)
					LOG_UNIT_MOVES_MESSAGE("pPathPlot is NULL");
				else
					LOG_UNIT_MOVES_MESSAGE_OSTR(std::string("Cannot move into pPathPlot ") << pPathPlot->getX() << std::string(" ") << pPathPlot->getY());
#endif
				return 0;
			}
		}
		else
		{
			bool bPathGenerated = UpdatePathCache(pDestPlot, iFlags);

			if (!bPathGenerated)
			{
				LOG_UNIT_MOVES_MESSAGE("Unable to Generate path");
				return 0;
			}

			pPathPlot = GetPathFirstPlot();
		}
	}

	bool bRejectMove = false;


	if(m_kLastPath.size() != 0)
	{
		const CvPathNode& kNode = m_kLastPath.front();


		if(iPrevETA >= 0 && kNode.m_iData2 > iPrevETA + 2)
		{
			LOG_UNIT_MOVES_MESSAGE_OSTR(std::string("Rejecting move iPrevETA=") << iPrevETA << std::string(", m_iData2=") << kNode.m_iData2);
			bRejectMove = true;
		}

		if(kNode.m_iData2 == 1 && !canMoveInto(*pDestPlot, MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE))
		{





			const UnitHandle pDefender = pDestPlot->getBestDefender(NO_PLAYER, getOwner(), this, true);
			if(!pDefender && !pDestPlot->isEnemyCity(*this) && canMoveInto(*pDestPlot, MOVEFLAG_ATTACK | MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE))
			{

				iFlags |= MOVE_UNITS_THROUGH_ENEMY;
			}

			const MissionData* pkMissionData = GetHeadMissionData();
			CvAssertMsg(pkMissionData, "Unit mission is null. Need to rethink this code!");
			if(pkMissionData != NULL && pkMissionData->iPushTurn != GC.getGame().getGameTurn())
			{
				LOG_UNIT_MOVES_MESSAGE_OSTR(std::string("Rejecting move pkMissionData->iPushTurn=") << pkMissionData->iPushTurn << std::string(", GC.getGame().getGameTurn()=") << GC.getGame().getGameTurn());
				bRejectMove = true;
			}
		}

		if(bRejectMove)
		{
			m_kLastPath.clear();

			PublishQueuedVisualizationMoves();
			return 0;
		}
	}


	bool bEndMove = (pPathPlot == pDestPlot);
	bool bMoved = UnitMove(pPathPlot, iFlags & MOVE_UNITS_THROUGH_ENEMY, NULL, bEndMove);

	int iETA = 1;
	uint uiCachedPathSize = m_kLastPath.size();
	if (uiCachedPathSize)
	{
		iETA = m_kLastPath[ 0 ].m_iData2;

		if (bMoved)
		{
			if(uiCachedPathSize > 1)
			{

				if (m_kLastPath[ uiCachedPathSize - 1 ].m_iData2 != m_kLastPath[ uiCachedPathSize - 2 ].m_iData2)
				{


					for (uint uiIndex = uiCachedPathSize - 1; uiIndex--; )
					{
						CvPathNode* pNode = &m_kLastPath[uiIndex];
						if (pNode->m_iData2 > 0)
							pNode->m_iData2 -= 1;
					}
				}

				m_kLastPath.pop_back();
			}
			else
				m_kLastPath.clear();
		}
		else
			m_kLastPath.clear();
	}

	return iETA;
}



bool CvUnit::UnitRoadTo(int iX, int iY, int iFlags)
{
	VALIDATE_OBJECT
	CvPlot* pPlot;
	RouteTypes eBestRoute;
	BuildTypes eBestBuild;

	if(!IsAutomated() || !at(iX, iY) || (GetLengthMissionQueue() == 1))
	{
		pPlot = plot();

		eBestRoute = GetBestBuildRoute(pPlot, &eBestBuild);

		if(eBestBuild != NO_BUILD)
		{
			UnitBuild(eBestBuild);
			return true;
		}
	}

	bool bWasMoveMade = ( UnitPathTo(iX, iY, iFlags, -1, true) > 0 );
	if(bWasMoveMade)
	{
		PublishQueuedVisualizationMoves();
	}

	return bWasMoveMade;
}




bool CvUnit::UnitBuild(BuildTypes eBuild)
{
	VALIDATE_OBJECT
	CvPlot* pPlot;
	bool bContinue;

	CvAssert(getOwner() != NO_PLAYER);
	CvAssertMsg(eBuild < GC.getNumBuildInfos(), "Invalid Build");

	bContinue = false;

	pPlot = plot();
	if(! pPlot)
	{
		return false;
	}

	CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
	if (pkBuildInfo)
	{
		ImprovementTypes eImprovement = (ImprovementTypes)pkBuildInfo->getImprovement();
		if(eImprovement != NO_IMPROVEMENT)
		{
			if(IsAutomated())
			{
				if((pPlot->getImprovementType() != NO_IMPROVEMENT) && (pPlot->getImprovementType() != (ImprovementTypes)(GC.getRUINS_IMPROVEMENT())))
				{
					ResourceTypes eResource = (ResourceTypes)pPlot->getNonObsoleteResourceType(GET_PLAYER(getOwner()).getTeam());
					if((eResource == NO_RESOURCE) || !GC.getImprovementInfo(eImprovement)->IsImprovementResourceTrade(eResource))
					{
						if(GC.getImprovementInfo(eImprovement)->GetImprovementPillage() != NO_IMPROVEMENT)
						{
							return false;
						}
					}
				}
			}
		}
	}

	CvAssertMsg(atPlot(*pPlot), "Unit is expected to be at pPlot");


	if(canBuild(pPlot, eBuild, false, false))
	{
		bContinue = true;

		if(build(eBuild))
		{
			bContinue = false;
		}
	}

	return bContinue;
}


bool CvUnit::CanDoInterfaceMode(InterfaceModeTypes eInterfaceMode, bool bTestVisibility)
{
	VALIDATE_OBJECT
	CvAssertMsg(eInterfaceMode != NO_INTERFACEMODE, "InterfaceMode is not assigned a valid value");

	if(IsBusy())
	{
		return false;
	}

	switch(eInterfaceMode)
	{
	case INTERFACEMODE_MOVE_TO:
		if((getDomainType() != DOMAIN_AIR) && (getDomainType() != DOMAIN_IMMOBILE) && (!IsImmobile()))
		{
			return true;
		}
		break;

	case INTERFACEMODE_MOVE_TO_TYPE:
		break;

	case INTERFACEMODE_MOVE_TO_ALL:
		break;

	case INTERFACEMODE_ROUTE_TO:
		if(canBuildRoute())
		{
			return true;
		}
		break;

	case INTERFACEMODE_AIRLIFT:
		if(canAirlift(plot()))
		{
			return true;
		}
		break;

	case INTERFACEMODE_NUKE:
		if(canNuke(plot()))
		{
			return true;
		}
		break;

	case INTERFACEMODE_PARADROP:
		if(canParadrop(plot(), bTestVisibility))
		{
			return true;
		}
		break;

	case INTERFACEMODE_RANGE_ATTACK:
		if(canRangeStrike() && getDomainType() != DOMAIN_AIR)
		{
			return true;
		}
		break;

	case INTERFACEMODE_CITY_RANGE_ATTACK:
		break;

	case INTERFACEMODE_ATTACK:
		if(IsCanAttackWithMove() && !isOutOfAttacks())
		{
			if(IsEnemyInMovementRange(false, IsCityAttackOnly()) || bTestVisibility)
			{
				return true;
			}
		}

		break;

	case INTERFACEMODE_AIRSTRIKE:
		if(getDomainType() == DOMAIN_AIR)
		{
			if(canRangeStrike())
			{
				return true;
			}
		}
		break;

	case INTERFACEMODE_AIR_SWEEP:
		if(getDomainType() == DOMAIN_AIR)
		{
			if(canAirSweep())
			{
				return true;
			}
		}
		break;

	case INTERFACEMODE_REBASE:
		if(getDomainType() == DOMAIN_AIR)
		{
			return true;
		}
		break;

	case INTERFACEMODE_EMBARK:
		if(canEmbark(plot()))
		{
			return true;
		}
		break;

	case INTERFACEMODE_DISEMBARK:
		if(canDisembark(plot()))
		{
			return true;
		}
		break;

	case INTERFACEMODE_GIFT_UNIT:
		if(CanDistanceGift((PlayerTypes) DLLUI->GetInterfaceModeValue()))
		{
			return true;
		}
		break;

	case INTERFACEMODE_ESTABLISH_TRADE_ROUTE:
		if (canMakeTradeRoute(plot()))
		{
			return true;
		}
	}

	return false;
}







void CvUnit::PushMission(MissionTypes eMission, int iData1, int iData2, int iFlags, bool bAppend, bool bManual, MissionAITypes eMissionAI, CvPlot* pMissionAIPlot, CvUnit* pMissionAIUnit)
{
	VALIDATE_OBJECT
	CvUnitMission::PushMission(this, eMission, iData1, iData2, iFlags, bAppend, bManual, eMissionAI, pMissionAIPlot, pMissionAIUnit);
}



void CvUnit::PopMission()
{
	VALIDATE_OBJECT
	CvUnitMission::PopMission(this);
}



void CvUnit::AutoMission()
{
	VALIDATE_OBJECT
	CvUnitMission::AutoMission(this);
}



void CvUnit::UpdateMission()
{
	VALIDATE_OBJECT
	CvUnitMission::UpdateMission(m_thisHandle);
}



CvPlot* CvUnit::LastMissionPlot()
{
	VALIDATE_OBJECT
	return CvUnitMission::LastMissionPlot(this);
}



bool CvUnit::CanStartMission(int iMission, int iData1, int iData2, CvPlot* pPlot, bool bTestVisible)
{
	VALIDATE_OBJECT
	return CvUnitMission::CanStartMission(this, iMission, iData1, iData2, pPlot, bTestVisible);
}



int CvUnit::GetMissionTimer() const
{
	VALIDATE_OBJECT
	return m_iMissionTimer;
}



void CvUnit::SetMissionTimer(int iNewValue)
{
	VALIDATE_OBJECT
	CvAssert(getOwner() != NO_PLAYER);

	int iOldTimer = m_iMissionTimer;

	m_iMissionTimer = iNewValue;
	CvAssert(GetMissionTimer() >= 0);

	int iNewTimer = m_iMissionTimer;

	if((iOldTimer == 0) != (iNewTimer == 0))
	{
		if((getOwner() == GC.getGame().getActivePlayer()) && IsSelected())
		{
			DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
		}

		if(iNewTimer == 0)
		{
			auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
			gDLL->GameplayUnitMissionEnd(pDllUnit.get());
		}
	}
}



void CvUnit::ChangeMissionTimer(int iChange)
{
	VALIDATE_OBJECT
	SetMissionTimer(GetMissionTimer() + iChange);
}



void CvUnit::ClearMissionQueue(int iUnitCycleTimer)
{
	VALIDATE_OBJECT
	CvAssert(getOwner() != NO_PLAYER);

	CvUnitMission::ClearMissionQueue(this, iUnitCycleTimer);
}



int CvUnit::GetLengthMissionQueue()	const
{
	VALIDATE_OBJECT
	return m_missionQueue.getLength();
}



const MissionData* CvUnit::GetHeadMissionData()
{
	VALIDATE_OBJECT
	if(m_missionQueue.getLength())
		return (m_missionQueue.head());
	return NULL;
}


#ifdef AUI_FIX_FFASTVECTOR_USE_UNSIGNED
const MissionData* CvUnit::GetMissionData(unsigned int iIndex)
#else
const MissionData* CvUnit::GetMissionData(int iIndex)
#endif
{
	VALIDATE_OBJECT
#ifdef AUI_FIX_FFASTVECTOR_USE_UNSIGNED
	if (iIndex < m_missionQueue.getLength())
#else
	if(iIndex >= 0 && iIndex < m_missionQueue.getLength())
#endif
		return m_missionQueue.getAt(iIndex);

	return NULL;
}



const MissionQueueNode* CvUnit::HeadMissionQueueNode() const
{
	VALIDATE_OBJECT
	if(m_missionQueue.getLength() > 0)
	{
		return m_missionQueue.head();
	}
	else
	{
		return NULL;
	}
}



MissionQueueNode* CvUnit::HeadMissionQueueNode()
{
	VALIDATE_OBJECT
	if(m_missionQueue.getLength() > 0)
	{
		return m_missionQueue.head();
	}
	else
	{
		return NULL;
	}
}



CvPlot* CvUnit::GetMissionAIPlot()
{
	VALIDATE_OBJECT
	return GC.getMap().plotCheckInvalid(m_iMissionAIX, m_iMissionAIY);
}



MissionAITypes CvUnit::GetMissionAIType()
{
	VALIDATE_OBJECT
	return m_eMissionAIType;
}



void CvUnit::SetMissionAI(MissionAITypes eNewMissionAI, CvPlot* pNewPlot, CvUnit* pNewUnit)
{
	VALIDATE_OBJECT
	m_eMissionAIType = eNewMissionAI;

	if(pNewPlot != NULL)
	{
		m_iMissionAIX = pNewPlot->getX();
		m_iMissionAIY = pNewPlot->getY();
	}
	else
	{
		m_iMissionAIX = INVALID_PLOT_COORD;
		m_iMissionAIY = INVALID_PLOT_COORD;
	}

	if(pNewUnit != NULL)
	{
		m_missionAIUnit = pNewUnit->GetIDInfo();
	}
	else
	{
		m_missionAIUnit.reset();
	}
}



CvUnit* CvUnit::GetMissionAIUnit()
{
	VALIDATE_OBJECT
	return getUnit(m_missionAIUnit);
}





bool CvUnit::IsCanAttackWithMove() const
{
	VALIDATE_OBJECT
	if(!IsCombatUnit())
	{
		return false;
	}

	return !isOnlyDefensive();
}



bool CvUnit::IsCanAttackRanged() const
{
	VALIDATE_OBJECT
	return (GetRange() > 0 && GetBaseRangedCombatStrength() > 0);
}



bool CvUnit::IsCanAttack() const
{
	VALIDATE_OBJECT
	return IsCanAttackWithMove() || IsCanAttackRanged();
}



bool CvUnit::IsCanAttackWithMoveNow() const
{
	VALIDATE_OBJECT
	if(!IsCanAttackWithMove())
	{
		return false;
	}



	DomainTypes eSourceDomain = getDomainType();
	CvPlot* pkPlot = plot();
	if (pkPlot->isCity() && eSourceDomain != DOMAIN_AIR)
	{
		IDInfo* pUnitNode = pkPlot->headUnitNode();
		int iCount = 0;
		while(pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
			if(pLoopUnit && pLoopUnit->IsCombatUnit() && pLoopUnit->getDomainType() == eSourceDomain)
			{
				iCount++;
			}

			pUnitNode = pkPlot->nextUnitNode(pUnitNode);
		}

		return iCount <= 1;
	}

#if defined(LEKMOD_WATER_WALK_IMPROVEMENT_RULES)

	if (pkPlot->IsAllowsWalkWater() && pkPlot->HasStackedLandAndNavalUnits())
	{
		return false;
	}
#endif

	return true;
}



bool CvUnit::IsCanDefend(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(pPlot == NULL)
	{
		pPlot = plot();
	}


	if(GetBaseCombatStrength() == 0)
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

	if(!pPlot->isValidDomainForAction(*this))
	{
		return false;
	}

	return true;
}


bool CvUnit::IsEnemyInMovementRange(bool bOnlyFortified, bool bOnlyCities)
{
	VALIDATE_OBJECT

	CvTeam& kTeam = GET_TEAM(getTeam());
	int iMovementRange = maxMoves() / GC.getMOVE_DENOMINATOR();



	bool bPotentialTargetFound = false;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
	{
		if(bPotentialTargetFound)
		{
			break;
		}

		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayerLoop);
		if(kTeam.isAtWar(kPlayer.getTeam()))
		{
			if(!bOnlyFortified)
			{
				CvCity* pLoopCity = NULL;
				int iLoop = 0;
				for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
				{
					CvPlot* pPlot = pLoopCity->plot();
					if(pPlot && pPlot->isVisible(getTeam()) && area() == pPlot->area())
					{
						if(plotDistance(getX(), getY(), pLoopCity->getX(), pLoopCity->getY()) <= iMovementRange)
						{
							bPotentialTargetFound = true;
							break;
						}
					}
				}
			}

			if(!bOnlyCities)
			{
				CvUnit* pLoopUnit = NULL;
				int iLoop = 0;
				for(pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
				{

					if(bOnlyFortified)
					{

						if(!pLoopUnit->IsFortifiedThisTurn())
						{
							continue;
						}
					}

					CvPlot* pPlot = pLoopUnit->plot();
					if(pPlot && pPlot->isVisible(getTeam()) && (area() == pPlot->area() || IsHoveringUnit()))
					{
						if(plotDistance(getX(), getY(), pLoopUnit->getX(), pLoopUnit->getY()) <= iMovementRange)
						{
							bPotentialTargetFound = true;
							break;
						}
					}
				}
			}
		}
	}

	if(!bPotentialTargetFound)
	{
		return false;
	}

	CvTwoLayerPathFinder& thePathfinder = GC.getInterfacePathFinder();


	thePathfinder.SetDestValidFunc(NULL);
	thePathfinder.SetValidFunc(UIPathValid);
	if(bOnlyFortified)
	{
		thePathfinder.SetIsPathDestFunc(AttackFortifiedPathDest);
	}
	else if(bOnlyCities)
	{
		thePathfinder.SetIsPathDestFunc(AttackCityPathDest);
	}
	else
	{
		thePathfinder.SetIsPathDestFunc(AttackPathDest);
	}


	thePathfinder.SetData(this);
	bool bCanFindPath = thePathfinder.GeneratePath(getX(), getY(), -1, -1, MOVE_DECLARE_WAR, false);


	thePathfinder.SetValidFunc(UIPathValid);
	thePathfinder.SetDestValidFunc(PathDestValid);
	thePathfinder.SetIsPathDestFunc(PathDest);
	thePathfinder.ForceReset();

	return bCanFindPath;
}





bool CvUnit::GeneratePath(const CvPlot* pToPlot, int iFlags, bool bReuse, int* piPathTurns) const
{
	VALIDATE_OBJECT


	CvAssertMsg(!(IsDoingPartialMove() && m_iFlags & UNITFLAG_EVALUATING_MISSION), "Repathing during a partial move will cause serious issues!");

	CvAssertMsg(pToPlot != NULL, "Passed in a NULL destination to GeneratePath");
	if(pToPlot == NULL)
		return false;

	bool bSuccess;

	CvTwoLayerPathFinder& kPathFinder = GC.getPathFinder();

	bSuccess = kPathFinder.GenerateUnitPath(this, getX(), getY(), pToPlot->getX(), pToPlot->getY(), iFlags, bReuse);


	m_uiLastPathCacheDest = pToPlot->GetPlotIndex();

	CvAStar::CopyPath(kPathFinder.GetLastNode(), m_kLastPath);

	if(m_kLastPath.size() != 0)
	{
		CvMap& kMap = GC.getMap();
		TeamTypes eTeam = getTeam();



		for (uint uiIndex = m_kLastPath.size(); uiIndex--; )
		{
			CvPathNode& kNode = m_kLastPath[uiIndex];
			CvPlot* pkPlot = kMap.plotCheckInvalid(kNode.m_iX, kNode.m_iY);
			if (pkPlot)
			{
				if (!pkPlot->isVisible(eTeam))
				{
					kNode.SetFlag(CvPathNode::PLOT_INVISIBLE);
					if (uiIndex < (m_kLastPath.size() - 2))
						m_kLastPath[uiIndex + 1].SetFlag(CvPathNode::PLOT_ADJACENT_INVISIBLE);



					CvPlot* pkPathDest = kMap.plot(m_kLastPath[0].m_iX, m_kLastPath[0].m_iY);
					if (pkPathDest != NULL && !pkPathDest->isVisible(eTeam))
						m_kLastPath[0].SetFlag(CvPathNode::PLOT_INVISIBLE);

					break;
				}
			}
		}
	}

	if(piPathTurns != NULL)
	{
		*piPathTurns = INT_MAX;

		if(bSuccess)
		{
			if(m_kLastPath.size() != 0)
			{
				*piPathTurns = m_kLastPath.front().m_iData2;
			}
		}
	}

	return bSuccess;
}



void CvUnit::ResetPath()
{
	VALIDATE_OBJECT
	GC.getPathFinder().ForceReset();
}



CvPlot* CvUnit::GetPathFirstPlot() const
{
	VALIDATE_OBJECT

	uint uiPathSize = m_kLastPath.size();
	if(uiPathSize > 0)
	{
		if(uiPathSize == 1)
		{
			const CvPathNode& kNode = m_kLastPath.front();
			return GC.getMap().plotCheckInvalid(kNode.m_iX, kNode.m_iY);
		}
		else
		{


			const CvPathNode& kNode = m_kLastPath[ uiPathSize - 2];
			return GC.getMap().plotCheckInvalid(kNode.m_iX, kNode.m_iY);
		}
	}

	CvAssert(false);

	return NULL;
}



CvPlot* CvUnit::GetPathLastPlot() const
{
	VALIDATE_OBJECT
	if(m_kLastPath.size() > 0)
	{
		const CvPathNode& kNode = m_kLastPath.front();
		return GC.getMap().plotCheckInvalid(kNode.m_iX, kNode.m_iY);
	}

	return NULL;
}



const CvPathNodeArray& CvUnit::GetPathNodeArray() const
{
	VALIDATE_OBJECT
	return m_kLastPath;
}



void CvUnit::ClearPathCache()
{
	m_kLastPath.setsize(0);
	m_uiLastPathCacheDest = (uint)-1;
}




CvPlot* CvUnit::GetPathEndTurnPlot() const
{
	VALIDATE_OBJECT

	if(m_kLastPath.size())
	{
		const CvPathNode* pNode = &m_kLastPath[0];

		if(m_kLastPath.size() == 1 || (pNode->m_iData2 == 1))
		{
			return GC.getMap().plotCheckInvalid(pNode->m_iX, pNode->m_iY);
		}

		for(uint uiIndex = 1; uiIndex < m_kLastPath.size(); ++uiIndex)
		{
			pNode = &m_kLastPath[uiIndex];
			if(pNode->m_iData2 == 1)
			{
				return GC.getMap().plotCheckInvalid(pNode->m_iX, pNode->m_iY);
			}
		}
	}

	CvAssert(false);

	return NULL;
}

CvPlot* CvUnit::GetPathAttackFromPlot(const CvPlot* pTargetPlot) const
{
	VALIDATE_OBJECT

	if (pTargetPlot == NULL)
		return NULL;

	if (plotDistance(getX(), getY(), pTargetPlot->getX(), pTargetPlot->getY()) == 1)
		return plot();

	CvTwoLayerPathFinder& kPathFinder = GC.getPathFinder();
	if (!kPathFinder.GenerateUnitPath(this, getX(), getY(), pTargetPlot->getX(), pTargetPlot->getY(), MOVEFLAG_ATTACK, false))
		return NULL;

	CvPathNodeArray kScratchPath;
	CvAStar::CopyPath(kPathFinder.GetLastNode(), kScratchPath);

	if (kScratchPath.size() < 2)
		return NULL;

	const CvPathNode& kNode = kScratchPath[1];
	return GC.getMap().plotCheckInvalid(kNode.m_iX, kNode.m_iY);
}


int CvUnit::SearchRange(int iRange) const
{
	VALIDATE_OBJECT
	if(iRange == 0)
	{
		return 0;
	}

	if(flatMovementCost() || (getDomainType() == DOMAIN_SEA))
	{
		return (iRange * baseMoves());
	}
	else
	{
		return ((iRange + 1) * (baseMoves() + 1));
	}
}


bool CvUnit::PlotValid(CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(isNoRevealMap() && willRevealByMove(*pPlot))
	{
		return false;
	}

	if(!canEnterTerrain(*pPlot))
	{
		return false;
	}

	switch(getDomainType())
	{
	case DOMAIN_SEA:
		if(pPlot->isWater() || canMoveAllTerrain())
		{
			return true;
		}
		else if(pPlot->isFriendlyCity(*this, true) && pPlot->isCoastalLand())
		{
			return true;
		}
		break;

	case DOMAIN_AIR:
		CvAssert(false);
		break;

	case DOMAIN_LAND:
#if defined(LEKMOD_HELICOPTER_EMBARK_FIX)
		if (pPlot->getArea() == getArea() || canMoveAllTerrain() || pPlot->IsAllowsWalkWater() || (IsHoveringUnit() && pPlot->isWater()))
#elif defined(AUI_UNIT_FIX_HOVERING_EMBARK)
		if (pPlot->getArea() == getArea() || canMoveAllTerrain() || pPlot->IsAllowsWalkWater() || (IsHoveringUnit() && pPlot->isWater() && pPlot->getTerrainType() != GC.getDEEP_WATER_TERRAIN()))
#else
		if(pPlot->getArea() == getArea() || canMoveAllTerrain() || pPlot->IsAllowsWalkWater())
#endif
		{
			return true;
		}
		break;

	case DOMAIN_IMMOBILE:
		CvAssert(false);
		break;

	default:
		CvAssert(false);
		break;
	}

	return false;
}




bool CvUnit::canAdvance(const CvPlot& plot, int iThreshold) const
{
	VALIDATE_OBJECT
	CvAssert(IsCombatUnit());
	CvAssert(getDomainType() != DOMAIN_AIR);
	CvAssert(getDomainType() != DOMAIN_IMMOBILE);

	if(plot.getNumVisibleEnemyDefenders(this) > iThreshold)
	{
		return false;
	}

	if(isNoCapture())
	{
		if(plot.isEnemyCity(*this))
		{
			return false;
		}
	}

	return true;
}


CvUnit* CvUnit::airStrikeTarget(CvPlot& targetPlot, bool bNoncombatAllowed) const
{
	VALIDATE_OBJECT
	UnitHandle pDefender;

	pDefender = targetPlot.getBestDefender(NO_PLAYER, getOwner(), this, true, false, false, bNoncombatAllowed);

	if(pDefender)
	{
		if(!pDefender->IsDead())
		{
			return pDefender.pointer();
		}
	}

	return NULL;
}


#ifdef AUI_UNIT_FIX_NO_RETREAT_ON_CIVILIAN_GUARD
bool CvUnit::CanWithdrawFromMelee(const CvUnit& attacker, const CvCombatInfo* pCombatInfo) const
#elif defined(AUI_CONSTIFY)
bool CvUnit::CanWithdrawFromMelee(const CvUnit& attacker) const
#else
bool CvUnit::CanWithdrawFromMelee(CvUnit& attacker)
#endif
{
	VALIDATE_OBJECT
#ifdef AUI_UNIT_FIX_NO_RETREAT_ON_CIVILIAN_GUARD
	int iNumDefendersInMyPlot = plot()->getNumDefenders(getOwner());
	if (IsCanDefend() && iNumDefendersInMyPlot <= 1 && (uint)iNumDefendersInMyPlot > plot()->getNumUnits() && 
		(!pCombatInfo || pCombatInfo->getDamageInflicted(BATTLE_UNIT_ATTACKER) < GetCurrHitPoints()))
		return false;
#endif
	int iWithdrawChance = getExtraWithdrawal();


	int iAttackerMovementRange = attacker.maxMoves() / GC.getMOVE_DENOMINATOR();
	if(iAttackerMovementRange > 0)
	{
		iWithdrawChance += (GC.getWITHDRAW_MOD_ENEMY_MOVES() * (iAttackerMovementRange - 2));
	}


	int iBlockedHexes = 0;
	CvPlot* pAttackerFromPlot = attacker.plot();
	DirectionTypes eAttackDirection = directionXY(pAttackerFromPlot, plot());
	int iBiases[3] = {0,-1,1};
	int x = plot()->getX();
	int y = plot()->getY();

	for(int i = 0; i < 3; i++)
	{
		int iMovementDirection = (NUM_DIRECTION_TYPES + eAttackDirection + iBiases[i]) % NUM_DIRECTION_TYPES;
		CvPlot* pDestPlot = plotDirection(x, y, (DirectionTypes) iMovementDirection);

		if(pDestPlot && !canMoveInto(*pDestPlot, MOVEFLAG_DESTINATION))
		{
			iBlockedHexes++;
		}
	}


	if(iBlockedHexes >= 3)
	{
		return false;
	}

	iWithdrawChance += (GC.getWITHDRAW_MOD_BLOCKED_TILE() * iBlockedHexes);

	int iRoll = GC.getGame().getJonRandNum(100, "Withdraw from Melee attempt");

	return iRoll < iWithdrawChance;
}


bool CvUnit::DoWithdrawFromMelee(CvUnit& attacker)
{
	VALIDATE_OBJECT
	CvPlot* pAttackerFromPlot = attacker.plot();
	DirectionTypes eAttackDirection = directionXY(pAttackerFromPlot, plot());

	int iRightOrLeftBias = (GC.getGame().getJonRandNum(100, "right or left bias") < 50) ? 1 : -1;
	int iBiases[5] = {0,-1,1,-2,2};
	int x = plot()->getX();
	int y = plot()->getY();


	for(int i = 0; i < 5; i++)
	{
		int iMovementDirection = (NUM_DIRECTION_TYPES + eAttackDirection + (iBiases[i] * iRightOrLeftBias)) % NUM_DIRECTION_TYPES;
		CvPlot* pDestPlot = plotDirection(x, y, (DirectionTypes) iMovementDirection);

		if(pDestPlot && canMoveInto(*pDestPlot, MOVEFLAG_DESTINATION))
		{
			setXY(pDestPlot->getX(), pDestPlot->getY(), false, false, true, false);
			return true;
		}
	}
	return false;
}


bool CvUnit::CanFallBackFromMelee(const CvUnit& attacker) const
{
	VALIDATE_OBJECT

	int iBlockedHexes = 0;
	CvPlot* pAttackerFromPlot = attacker.plot();

	DirectionTypes eAttackDirection = directionXY(pAttackerFromPlot, plot());
	int iBiases[3] = {0,-1,1};
	int x = plot()->getX();
	int y = plot()->getY();

	for(int i = 0; i < 3; i++)
	{
		int iMovementDirection = (NUM_DIRECTION_TYPES + eAttackDirection + iBiases[i]) % NUM_DIRECTION_TYPES;
		CvPlot* pDestPlot = plotDirection(x, y, (DirectionTypes) iMovementDirection);

		if(pDestPlot && !canMoveInto(*pDestPlot, MOVEFLAG_DESTINATION))
		{
			iBlockedHexes++;
		}
	}


	if(iBlockedHexes >= 3)
	{
		return false;
	}

	return true;
}


bool CvUnit::DoFallBackFromMelee(CvUnit& attacker)
{
	VALIDATE_OBJECT

	CvPlot* pAttackerFromPlot = attacker.plot();
	DirectionTypes eAttackDirection = directionXY(pAttackerFromPlot, plot());

	int iRightOrLeftBias = (GC.getGame().getJonRandNum(100, "right or left bias") < 50) ? 1 : -1;
	int iBiases[5] = {0,-1,1,-2,2};
	int x = plot()->getX();
	int y = plot()->getY();


	for(int i = 0; i < 5; i++)
	{
		int iMovementDirection = (NUM_DIRECTION_TYPES + eAttackDirection + (iBiases[i] * iRightOrLeftBias)) % NUM_DIRECTION_TYPES;
		CvPlot* pDestPlot = plotDirection(x, y, (DirectionTypes) iMovementDirection);

		if(pDestPlot && canMoveInto(*pDestPlot, MOVEFLAG_DESTINATION))
		{
			setXY(pDestPlot->getX(), pDestPlot->getY(), false, false, true, false);
			return true;
		}
	}
	return false;
}


UnitAITypes CvUnit::AI_getUnitAIType() const
{
	VALIDATE_OBJECT
	return m_eUnitAIType;
}


void CvUnit::AI_setUnitAIType(UnitAITypes eNewValue)
{
	VALIDATE_OBJECT
	CvAssertMsg(eNewValue != NO_UNITAI, "NewValue is not assigned a valid value");

	if(AI_getUnitAIType() != eNewValue)
	{
		m_eUnitAIType = eNewValue;
	}
}


void CvUnit::AI_promote()
{
	VALIDATE_OBJECT
	PromotionTypes eBestPromotion;
	int iValue;
	int iBestValue;
#ifdef AUI_WARNING_FIXES
	uint iI;
#else
	int iI;
#endif

	iBestValue = 0;
	eBestPromotion = NO_PROMOTION;

	for(iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		const PromotionTypes ePromotion(static_cast<PromotionTypes>(iI));

		if(canPromote(ePromotion, -1))
		{
			iValue = AI_promotionValue(ePromotion);
			if(GC.getLogging() && GC.getAILogging())
			{
				CvPromotionEntry* pkPromotionEntry = GC.getPromotionInfo(ePromotion);
				const char* szPromotionDesc = (pkPromotionEntry != NULL)? pkPromotionEntry->GetDescription() : "Unknown Promotion";

				CvString szMsg;
				szMsg.Format("Promotion, %s, For %s, Value: %d, Damage: %d",
				             szPromotionDesc, getName().GetCString(), iValue, getDamage());
				GET_PLAYER(m_eOwner).GetTacticalAI()->LogTacticalMessage(szMsg, true                          );
			}

			if(iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestPromotion = ePromotion;
			}
		}
	}

	if(eBestPromotion != NO_PROMOTION)
	{
		promote(eBestPromotion, -1);
		AI_promote();

		if(GC.getLogging() && GC.getAILogging())
		{
			CvString szMsg;
#ifdef AUI_WARNING_FIXES
			CvString szTemp;
			CvPromotionEntry* pBestPromotionInfo = GC.getPromotionInfo(eBestPromotion);
			if (pBestPromotionInfo)
				szTemp = pBestPromotionInfo->GetDescription();
			szMsg.Format("Promotion, %s, Received by %s, X: %d, Y: %d, Damage: %d",
						szTemp.GetCString(), getName().GetCString(), getX(), getY(), getDamage());
#else
			szMsg.Format("Promotion, %s, Received by %s, X: %d, Y: %d, Damage: %d",
			             GC.getPromotionInfo(eBestPromotion)->GetDescription(), getName().GetCString(), getX(), getY(), getDamage());
#endif
			GET_PLAYER(m_eOwner).GetTacticalAI()->LogTacticalMessage(szMsg, true                          );
		}
	}
}



int CvUnit::AI_promotionValue(PromotionTypes ePromotion)
{
	VALIDATE_OBJECT

	CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
	if(pkPromotionInfo == NULL)
	{

		CvAssert(pkPromotionInfo);
		return 0;
	}

	int iValue = 0;
	int iTemp;
	int iExtra;
#ifdef AUI_WARNING_FIXES
	uint iI;
#else
	int iI;
#endif


	CvFlavorManager* pFlavorMgr = GET_PLAYER(m_eOwner).GetFlavorManager();
	int iFlavorOffense = pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE"));
	int iFlavorDefense = pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DEFENSE"));
	int iFlavorRanged = pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RANGED"));
	int iFlavorRecon = pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RECON"));
	int iFlavorMobile = pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_MOBILE"));
	int iFlavorNaval = pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL"));
	int iFlavorAir = pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_AIR"));


	if(pkPromotionInfo->IsInstaHeal())
	{

		if(getDamage() >= (GetMaxHitPoints() / 2))
		{
			iValue += 1000;
		}
	}

	iTemp = pkPromotionInfo->GetOpenAttackPercent();
	if(iTemp != 0)
	{
		iExtra = getExtraOpenAttackPercent();
		iTemp *= (100 + iExtra * 2);
		iTemp /= 100;
		if(noDefensiveBonus())
		{
			iTemp *= 2;
		}
		iValue += iTemp + iFlavorOffense * 2;
	}

	iTemp = pkPromotionInfo->GetOpenDefensePercent();
	if(iTemp != 0)
	{
		iExtra = getExtraOpenDefensePercent() * 2;
		iTemp *= (100 + iExtra);
		iTemp /= 100;
		if(noDefensiveBonus())
		{
			iTemp *= 2;
		}
		iValue += iTemp + iFlavorDefense * 2;
	}

	iTemp = pkPromotionInfo->GetRoughAttackPercent();
	if(iTemp != 0)
	{
		iExtra = getExtraRoughAttackPercent();
		iTemp *= (100 + iExtra * 2);
		iTemp /= 100;
		if(!noDefensiveBonus())
		{
			iTemp *= 2;
		}
		iValue += iTemp + iFlavorOffense * 2;
	}

	iTemp = pkPromotionInfo->GetRoughDefensePercent();
	if(iTemp != 0)
	{
		iExtra = getExtraRoughDefensePercent() * 2;
		iTemp *= (100 + iExtra);
		iTemp /= 100;
		if(!noDefensiveBonus())
		{
			iTemp *= 2;
		}
		iValue += iTemp + iFlavorDefense * 2;
	}

	iTemp = pkPromotionInfo->GetOpenRangedAttackMod();
	if(iTemp != 0 && isRanged())
	{
		iExtra = getExtraOpenRangedAttackMod();
		iTemp *= (100 + iExtra * 2);
		iTemp /= 100;
		if(noDefensiveBonus())
		{
			iTemp *= 2;
		}
		iValue += iTemp + iFlavorRanged * 2;
	}

	iTemp = pkPromotionInfo->GetRoughRangedAttackMod();
	if(iTemp != 0 && isRanged())
	{
		iExtra = getExtraRoughRangedAttackMod() * 2;
		iTemp *= (100 + iExtra);
		iTemp /= 100;
		if(!noDefensiveBonus())
		{
			iTemp *= 2;
		}
		iValue += iTemp + iFlavorRanged * 2;
	}

	iTemp = pkPromotionInfo->GetVisibilityChange();
	if((AI_getUnitAIType() == UNITAI_EXPLORE_SEA) ||
	        (AI_getUnitAIType() == UNITAI_EXPLORE))
	{
		iValue += iTemp * (50 + iFlavorRecon * 2);
	}
	else
	{
		iValue += iTemp * (10 + iFlavorRecon * 2);
	}

	iTemp = pkPromotionInfo->GetCityAttackPercent();
	if(iTemp != 0)
	{
		iExtra = getExtraCityAttackPercent();
		iTemp *= (100 + iExtra * 2);
		iTemp /= 100;
		iValue += iTemp + iFlavorOffense * 2;
		if(isRanged())
		{
			iValue += iFlavorRanged * 2;
		}
	}

	iTemp = pkPromotionInfo->GetCityDefensePercent();
	if(iTemp != 0)
	{
		iExtra = getExtraCityDefensePercent();
		iTemp *= (100 + iExtra * 2);
		iTemp /= 100;
		iValue += iTemp + iFlavorDefense * 2;
		if((AI_getUnitAIType() == UNITAI_DEFENSE) ||
		        (AI_getUnitAIType() == UNITAI_COUNTER))
		{
			iValue *= 2;
		}
	}

	iTemp = pkPromotionInfo->GetAttackFortifiedMod();
	if(iTemp != 0 && isRanged())
	{
		iExtra = getExtraAttackFortifiedMod() * 2;
		iTemp *= (100 + iExtra);
		iTemp /= 100;
		iValue += iTemp + (iFlavorOffense * iFlavorRanged) * 2;
	}

	iTemp = pkPromotionInfo->GetEnemyHealChange();
	if((AI_getUnitAIType() == UNITAI_PARADROP) ||
	        (AI_getUnitAIType() == UNITAI_PIRATE_SEA))
	{
		iValue += iTemp * (50 + iFlavorOffense * 2);
	}
	else
	{
		iValue += iTemp * (10 + iFlavorOffense * 2);
	}

	iTemp = pkPromotionInfo->GetNeutralHealChange();
	if((AI_getUnitAIType() == UNITAI_EXPLORE) ||
	        (AI_getUnitAIType() == UNITAI_EXPLORE_SEA))
	{
		iValue += iTemp * (40 + iFlavorRecon * 2);
	}
	else
	{
		iValue += iTemp * (10 + iFlavorRecon * 2);
	}

	iTemp = pkPromotionInfo->GetFriendlyHealChange();
	if((AI_getUnitAIType() == UNITAI_DEFENSE) ||
	        (AI_getUnitAIType() == UNITAI_COUNTER))
	{
		iValue += iTemp * (20 + iFlavorDefense * 2);
	}
	else
	{
		iValue += iTemp * (10 + iFlavorDefense * 2);
	}

	iTemp = pkPromotionInfo->GetAdjacentTileHealChange();
	if((AI_getUnitAIType() == UNITAI_DEFENSE) ||
	        (AI_getUnitAIType() == UNITAI_COUNTER))
	{
		iValue += iTemp * (40 + iFlavorDefense * 2);
	}
	else
	{
		iValue += iTemp * (10 + iFlavorDefense * 2);
	}

	if(pkPromotionInfo->IsAmphib())
	{
		if((AI_getUnitAIType() == UNITAI_FAST_ATTACK) ||
		        (AI_getUnitAIType() == UNITAI_ATTACK))
		{
			iValue += 40 + iFlavorOffense * 2;
		}
		else
		{
			iValue += 10 + iFlavorOffense * 2;
		}
	}

	if(pkPromotionInfo->IsRiver())
	{
		if((AI_getUnitAIType() == UNITAI_FAST_ATTACK) ||
		        (AI_getUnitAIType() == UNITAI_ATTACK))
		{
			iValue += 40 + iFlavorOffense * 2;
		}
		else
		{
			iValue += 10 + iFlavorOffense * 2;
		}
	}

	iTemp = pkPromotionInfo->GetRangedDefenseMod();
	if(iTemp != 0)
	{
		iExtra = getExtraRangedDefenseModifier() * 2;
		iTemp *= (100 + iExtra);
		iTemp /= 100;

		if((AI_getUnitAIType() == UNITAI_DEFENSE) || (AI_getUnitAIType() == UNITAI_COUNTER) || (AI_getUnitAIType() == UNITAI_ATTACK))
		{
			iTemp *= 2;
		}

		if (maxMoves() / GC.getMOVE_DENOMINATOR() <= 2)
		{
			iTemp *= 2;
		}
		iValue += iTemp + iFlavorDefense * 2;
	}

	if(pkPromotionInfo->IsRangeAttackIgnoreLOS() && isRanged())
	{
		iValue += 75 + iFlavorRanged * 2;
	}

	iTemp = pkPromotionInfo->GetAttackWoundedMod();
	if(iTemp != 0 && isRanged())
	{
		iExtra = getExtraAttackWoundedMod() * 2;
		iTemp *= (100 + iExtra);
		iTemp /= 100;
		iValue += iTemp + (iFlavorOffense * iFlavorRanged) * 2;
	}

	iTemp = pkPromotionInfo->GetFlankAttackModifier();
	if(iTemp > 0)
	{
		iExtra = (2 * iFlavorOffense + iFlavorMobile) * maxMoves() / GC.getMOVE_DENOMINATOR();
		iExtra *= iTemp;
		iExtra /= 100;
		iValue += iExtra;
	}

	if(GC.getPromotionInfo(ePromotion)->IsHealOutsideFriendly() && getDomainType() == DOMAIN_SEA)
	{
		iValue += 50 + iFlavorNaval * 2;
	}

	iTemp = pkPromotionInfo->GetMovesChange();
	if((AI_getUnitAIType() == UNITAI_ATTACK_SEA) ||
	        (AI_getUnitAIType() == UNITAI_PIRATE_SEA) ||
	        (AI_getUnitAIType() == UNITAI_RESERVE_SEA) ||
	        (AI_getUnitAIType() == UNITAI_ESCORT_SEA) ||
	        (AI_getUnitAIType() == UNITAI_EXPLORE_SEA) ||
	        (AI_getUnitAIType() == UNITAI_ASSAULT_SEA) ||
	        (AI_getUnitAIType() == UNITAI_SETTLER_SEA) ||
	        (AI_getUnitAIType() == UNITAI_FAST_ATTACK) ||
	        (AI_getUnitAIType() == UNITAI_ATTACK) ||
	        (AI_getUnitAIType() == UNITAI_PARADROP))
	{
		iValue += iTemp * (50 + iFlavorMobile * 2);
	}
	else
	{
		iValue += iTemp * (30 + iFlavorMobile * 2);
	}

	if(pkPromotionInfo->IsAlwaysHeal())
	{
		if((AI_getUnitAIType() == UNITAI_ATTACK) ||
		        (AI_getUnitAIType() == UNITAI_CITY_BOMBARD) ||
		        (AI_getUnitAIType() == UNITAI_FAST_ATTACK) ||
		        (AI_getUnitAIType() == UNITAI_COUNTER) ||
		        (AI_getUnitAIType() == UNITAI_ATTACK_SEA) ||
		        (AI_getUnitAIType() == UNITAI_PIRATE_SEA) ||
		        (AI_getUnitAIType() == UNITAI_ESCORT_SEA) ||
		        (AI_getUnitAIType() == UNITAI_PARADROP))
		{
			iValue += 50 + iFlavorMobile * 2;
		}
		else
		{
			iValue += 30 + iFlavorMobile * 2;
		}
	}

	if(pkPromotionInfo->IsBlitz())
	{
		if((AI_getUnitAIType() == UNITAI_ATTACK) ||
		        (AI_getUnitAIType() == UNITAI_CITY_BOMBARD) ||
		        (AI_getUnitAIType() == UNITAI_FAST_ATTACK) ||
		        (AI_getUnitAIType() == UNITAI_COUNTER) ||
		        (AI_getUnitAIType() == UNITAI_ATTACK_SEA) ||
		        (AI_getUnitAIType() == UNITAI_PIRATE_SEA) ||
		        (AI_getUnitAIType() == UNITAI_ESCORT_SEA) ||
		        (AI_getUnitAIType() == UNITAI_PARADROP))
		{
			iValue += 50 + (iFlavorMobile + iFlavorOffense) * 2;
		}
		else
		{
			iValue += 20 + (iFlavorMobile + iFlavorOffense) * 2;
		}
	}

	if(pkPromotionInfo->IsCanMoveAfterAttacking())
	{
		if((AI_getUnitAIType() == UNITAI_ATTACK) ||
		        (AI_getUnitAIType() == UNITAI_CITY_BOMBARD) ||
		        (AI_getUnitAIType() == UNITAI_FAST_ATTACK) ||
		        (AI_getUnitAIType() == UNITAI_COUNTER) ||
		        (AI_getUnitAIType() == UNITAI_ATTACK_SEA) ||
		        (AI_getUnitAIType() == UNITAI_PIRATE_SEA) ||
		        (AI_getUnitAIType() == UNITAI_ESCORT_SEA) ||
		        (AI_getUnitAIType() == UNITAI_PARADROP))
		{
			iValue += 30 + (iFlavorMobile + iFlavorOffense) * 2;
		}
		else
		{
			iValue += 10 + (iFlavorMobile + iFlavorOffense) * 2;
		}
	}

	iTemp = pkPromotionInfo->GetExtraAttacks();
	if(iTemp != 0)
	{
		iValue += (iTemp * 200);
	}

	iTemp = pkPromotionInfo->GetRangeChange();
	if(isRanged())
	{
		iValue += (iTemp * 200);
	}

	iTemp = pkPromotionInfo->GetInterceptionCombatModifier();
	if(iTemp != 0 && canAirPatrol(NULL))
	{
		iExtra = GetInterceptionCombatModifier() * 2;
		iTemp *= (100 + iExtra);
		iTemp /= 100;
		iValue += iTemp + iFlavorAir * 2;
	}

	iTemp = pkPromotionInfo->GetAirSweepCombatModifier();
	if(iTemp != 0 && canAirSweep())
	{
		iExtra = GetAirSweepCombatModifier() * 2;
		iTemp *= (100 + iExtra);
		iTemp /= 100;
		iValue += iTemp + iFlavorAir * 2;
	}

	iTemp = pkPromotionInfo->GetNumInterceptionChange();
	if(iTemp != 0)
	{
		iValue += (iTemp * 200);
	}

	iTemp = pkPromotionInfo->GetInterceptionDefenseDamageModifier();
	if(iTemp != 0 && getDomainType() == DOMAIN_AIR)
	{
		iExtra = GetInterceptionDefenseDamageModifier() * 2;
		iTemp *= (100 + iExtra);
		iTemp /= 100;
		iValue += iTemp + iFlavorAir * 2;
	}

	iTemp = pkPromotionInfo->GetDefenseMod();
	if(iTemp != 0)
	{
		iExtra = getDefenseModifier() * 2;
		iTemp *= (100 + iExtra);
		iTemp /= 100;
		iValue += iTemp + iFlavorDefense * 2;
	}

	for(iI = 0; iI < GC.getNumTerrainInfos(); iI++)
	{
		const TerrainTypes eTerrain = static_cast<TerrainTypes>(iI);
		CvTerrainInfo* pkTerrainInfo = GC.getTerrainInfo(eTerrain);
		if(pkTerrainInfo)
		{
			iTemp = pkPromotionInfo->GetTerrainAttackPercent(iI);
			if(iTemp != 0)
			{
				iExtra = getExtraTerrainAttackPercent(eTerrain);
				iTemp *= (100 + iExtra * 2);
				iTemp /= 100;
				iValue += iTemp + iFlavorOffense * 2;
				if((AI_getUnitAIType() == UNITAI_ATTACK) ||
				        (AI_getUnitAIType() == UNITAI_FAST_ATTACK))
				{
					iValue *= 2;
				}
			}

			iTemp = pkPromotionInfo->GetTerrainDefensePercent(iI);
			if(iTemp != 0)
			{
				iExtra =  getExtraTerrainDefensePercent(eTerrain);
				iTemp *= (100 + iExtra);
				iTemp /= 100;
				iValue += iTemp + iFlavorDefense * 2;
				if((AI_getUnitAIType() == UNITAI_DEFENSE) ||
				        (AI_getUnitAIType() == UNITAI_COUNTER))
				{
					iValue *= 2;
				}
			}

			if(pkPromotionInfo->GetTerrainDoubleMove(iI))
			{
				if(AI_getUnitAIType() == UNITAI_EXPLORE)
				{
					iValue += 5 * (iFlavorRecon + iFlavorMobile);
				}
				else if((AI_getUnitAIType() == UNITAI_ATTACK) || (AI_getUnitAIType() == UNITAI_FAST_ATTACK))
				{
					iValue += 3 * (iFlavorOffense + iFlavorMobile);
				}
				else
				{
					iValue += 3 * iFlavorMobile;
				}
			}
		}
	}

	for(iI = 0; iI < GC.getNumFeatureInfos(); iI++)
	{
		const FeatureTypes eFeature = static_cast<FeatureTypes>(iI);
		CvFeatureInfo* pkFeatureInfo = GC.getFeatureInfo(eFeature);
		if(pkFeatureInfo)
		{
			iTemp = pkPromotionInfo->GetFeatureAttackPercent(iI);
			if(iTemp != 0)
			{
				iExtra = getExtraFeatureAttackPercent(eFeature);
				iTemp *= (100 + iExtra * 2);
				iTemp /= 100;
				iValue += iTemp + iFlavorOffense * 2;
				if((AI_getUnitAIType() == UNITAI_ATTACK) ||
				        (AI_getUnitAIType() == UNITAI_FAST_ATTACK))
				{
					iValue *= 2;
				}
			}

			iTemp = pkPromotionInfo->GetFeatureDefensePercent(iI);;
			if(iTemp != 0)
			{
				iExtra = getExtraFeatureDefensePercent(eFeature);
				iTemp *= (100 + iExtra * 2);
				iTemp /= 100;
				iValue += iTemp + iFlavorDefense * 2;
				if((AI_getUnitAIType() == UNITAI_DEFENSE) ||
				        (AI_getUnitAIType() == UNITAI_COUNTER))
				{
					iValue *= 2;
				}
			}

			if(pkPromotionInfo->GetFeatureDoubleMove(iI))
			{
				if(AI_getUnitAIType() == UNITAI_EXPLORE)
				{
					iValue += 5 * (iFlavorRecon + iFlavorMobile);
				}
				else if((AI_getUnitAIType() == UNITAI_ATTACK) || (AI_getUnitAIType() == UNITAI_FAST_ATTACK))
				{
					iValue += 3 * (iFlavorOffense + iFlavorMobile);
				}
				else
				{
					iValue += 3 * iFlavorMobile;
				}
			}
		}
	}

	int iOtherCombat = 0;
	int iSameCombat = 0;

	for(iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
	{
		const UnitCombatTypes eUnitCombat = static_cast<UnitCombatTypes>(iI);
		CvBaseInfo* pkUnitCombatInfo = GC.getUnitCombatClassInfo(eUnitCombat);
		if(pkUnitCombatInfo)
		{
			if(eUnitCombat == getUnitCombatType())
			{
				iSameCombat += unitCombatModifier(eUnitCombat);
			}
			else
			{
				iOtherCombat += unitCombatModifier(eUnitCombat);
			}
		}
	}

	for(iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
	{
		const UnitCombatTypes eUnitCombat = static_cast<UnitCombatTypes>(iI);
		CvBaseInfo* pkUnitCombatInfo = GC.getUnitCombatClassInfo(eUnitCombat);
		if(pkUnitCombatInfo)
		{
			iTemp = pkPromotionInfo->GetUnitCombatModifierPercent(iI);
			int iCombatWeight = 0;

			if((UnitCombatTypes)iI == getUnitCombatType())
			{
				if(iSameCombat >= iOtherCombat)
				{
					iCombatWeight = 70;
				}
				else
				{
					iCombatWeight = 30;
				}
			}
			else
			{

				if(unitCombatModifier(eUnitCombat) > 10)
				{
					iCombatWeight = 70;
				}
				else
				{
					iCombatWeight = 30;
				}
			}

			if((AI_getUnitAIType() == UNITAI_COUNTER) || (AI_getUnitAIType() == UNITAI_RANGED))
			{
				iValue += (iTemp * iCombatWeight) / 25;
			}
			else if((AI_getUnitAIType() == UNITAI_ATTACK) ||
			        (AI_getUnitAIType() == UNITAI_DEFENSE))
			{
				iValue += (iTemp * iCombatWeight) / 50;
			}
			else
			{
				iValue += (iTemp * iCombatWeight) / 100;
			}
		}
	}

	for(iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		iTemp = pkPromotionInfo->GetDomainModifierPercent(iI);
		if(AI_getUnitAIType() == UNITAI_COUNTER)
		{
			iValue += (iTemp * 2);
		}
		else if((AI_getUnitAIType() == UNITAI_ATTACK) ||
		        (AI_getUnitAIType() == UNITAI_DEFENSE))
		{
			iValue += iTemp;
		}
		else
		{
			iValue += (iTemp / 2);
		}
	}

	if(iValue > 0)
	{
		iValue += GC.getGame().getJonRandNum(15, "AI Promote");
	}

	return iValue;
}


GreatPeopleDirectiveTypes CvUnit::GetGreatPeopleDirective() const
{
	return m_eGreatPeopleDirectiveType;
}


void CvUnit::SetGreatPeopleDirective(GreatPeopleDirectiveTypes eDirective)
{
	m_eGreatPeopleDirectiveType = eDirective;
}



bool CvUnit::IsHasBeenPromotedFromGoody() const
{
	return m_iFlags & UNITFLAG_ALREADY_GOT_GOODY_UPGRADE;
}


void CvUnit::SetBeenPromotedFromGoody(bool bBeenPromoted)
{
	if(bBeenPromoted)
	{
		m_iFlags = m_iFlags | UNITFLAG_ALREADY_GOT_GOODY_UPGRADE;
	}
	else
	{
		m_iFlags = m_iFlags & ~UNITFLAG_ALREADY_GOT_GOODY_UPGRADE;
	}
}


bool CvUnit::IsHigherTechThan(UnitTypes otherUnit) const
{
	int iMyTechCost = 0;
	int iOtherTechCost = 0;

	TechTypes eMyTech = (TechTypes)getUnitInfo().GetPrereqAndTech();
	if(eMyTech != NO_TECH)
	{
		CvTechEntry* pEntry = GC.GetGameTechs()->GetEntry(eMyTech);
		if(pEntry)
		{
			iMyTechCost = pEntry->GetResearchCost();
		}
	}

	CvUnitEntry* pUnitEntry = GC.GetGameUnits()->GetEntry(otherUnit);
	if(pUnitEntry)
	{
		TechTypes eOtherTech = (TechTypes)pUnitEntry->GetPrereqAndTech();
		if(eOtherTech != NO_TECH)
		{
			CvTechEntry* pEntry = GC.GetGameTechs()->GetEntry(eOtherTech);
			if(pEntry)
			{
				iOtherTechCost = pEntry->GetResearchCost();
			}
		}
	}

	return iMyTechCost > iOtherTechCost;
}


bool CvUnit::IsLargerCivThan(const CvUnit* pOtherUnit) const
{
#ifdef NQ_COMBAT_BONUS_VS_SMALLER_CIV_FROM_POLICIES

	if (isBarbarian() || pOtherUnit->isBarbarian())
	{
		return false;
	}
#endif
	int iMyCities = 0;
	int iOtherCities = 0;

	CvPlayer& kPlayer = GET_PLAYER(getOwner());
	iMyCities = kPlayer.getNumCities();

	CvPlayer& kOtherPlayer = GET_PLAYER(pOtherUnit->getOwner());
	iOtherCities = kOtherPlayer.getNumCities();

	return iMyCities > iOtherCities;
}


void CvUnit::dispatchingNetMessage(bool dispatchingMessage)
{
	s_dispatchingNetMessage = dispatchingMessage;
}


bool CvUnit::dispatchingNetMessage()
{
	return s_dispatchingNetMessage;
}



bool CvUnit::canChangeVisibility() const
{
	return m_iMapLayer == DEFAULT_UNIT_MAP_LAYER;
}


std::string CvUnit::debugDump(const FAutoVariableBase&        ) const
{


	static const std::string empty("");
	return empty;
}


std::string CvUnit::stackTraceRemark(const FAutoVariableBase& var) const
{
	std::string result = "Game Turn : ";
	char gameTurnBuffer[8] = {0};
	int gameTurn = GC.getGame().getGameTurn();
	sprintf_s(gameTurnBuffer, "%d\0", gameTurn);
	result += gameTurnBuffer;
	result += "\nValue Before Change=" + FSerialization::toString(var);
	if(&var == &m_eActivityType)
	{
		result += "\nm_eActivityType changes based on canMove().\n";
		bool moves = getMoves() > 0;
		if(canMove())
		{
			result += "canMove() returned true because ";
			if(moves)
			{
				result += "getMoves() > 0\n";
				result += "---- Call Stack for m_iMoves last change: ----\n";
				result += m_iMoves.getStackTrace();
				result += "---- END STACK TRACE FOR m_iMoves ----\n";
			}
			else
			{
				result += "some unknown check was added there but not here in the remark code\n";
			}
		}
		else
		{
			result += "\ncanMove() return false because ";
			if(!moves)
			{
				result += "getMoves() == 0\n";
				result += "---- Call Stack for m_iMoves last change: ----\n";
				result += m_iMoves.getStackTrace();
				result += "---- END STACK TRACE FOR m_iMoves ----\n";

			}
			else
			{
				result += "some unknown check was added there but not here in the remark code\n";
			}
		}
	}
	return result;
}


DestructionNotification<UnitHandle>& CvUnit::getDestructionNotification()
{
	return m_destructionNotification;
}


FDataStream& operator<<(FDataStream& saveTo, const CvUnit& readFrom)
{
	readFrom.write(saveTo);
	return saveTo;
}


FDataStream& operator>>(FDataStream& loadFrom, CvUnit& writeTo)
{
	writeTo.read(loadFrom);
	return loadFrom;
}
