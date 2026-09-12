






#ifndef CVUNITCOMBAT_H
#define CVUNITCOMBAT_H

#pragma once

#include "CvUnit.h"
#include "CvPlot.h"


class CvUnitCombat
{
public:

	enum ATTACK_RESULT
	{
	    ATTACK_ABORTED		= 0,
	    ATTACK_COMPLETED	= 1,
	    ATTACK_QUEUED		= 2
	};

	enum ATTACK_OPTION
	{
	    ATTACK_OPTION_NONE	= 0,
	    ATTACK_OPTION_NO_DEFENSIVE_SUPPORT = 1
	};

	static void GenerateMeleeCombatInfo(CvCombatInfo* pkCombatInfo);
	static void GenerateRangedCombatInfo(CvCombatInfo* pkCombatInfo);
	static void GenerateAirCombatInfo(CvCombatInfo* pkCombatInfo);
	static void GenerateAirSweepCombatInfo(CvCombatInfo* pkCombatInfo);
	static void GenerateNuclearCombatInfo(CvCombatInfo* pkCombatInfo);

	static void ResolveCombat(const CvCombatInfo& kInfo, uint uiParentEventID = 0);

	static ATTACK_RESULT Attack(CvUnit& kAttacker, CvPlot& targetPlot, ATTACK_OPTION eOption);
	static ATTACK_RESULT AttackRanged(CvUnit& kAttacker, int iX, int iY, ATTACK_OPTION eOption);
	static ATTACK_RESULT AttackAir(CvUnit& kAttacker, CvPlot& targetPlot, ATTACK_OPTION eOption);
	static ATTACK_RESULT AttackAirSweep(CvUnit& kAttacker, CvPlot& targetPlot, ATTACK_OPTION eOption);
	static ATTACK_RESULT AttackCity(CvUnit& kAttacker, CvPlot& plot, ATTACK_OPTION eOption);
	static ATTACK_RESULT AttackNuclear(CvUnit& kAttacker, int iX, int iY, ATTACK_OPTION eOption);


	static CvUnit*		GetFireSupportUnit(PlayerTypes eDefender, int iDefendX, int iDefendY, int iAttackX, int iAttackY);
	static uint			ApplyNuclearExplosionDamage(CvPlot* pkTargetPlot, int iDamageLevel, CvUnit* pkAttacker = NULL);

protected:
	static void ResolveRangedUnitVsCombat(const CvCombatInfo& kInfo, uint uiParentEventID);
	static void ResolveRangedCityVsUnitCombat(const CvCombatInfo& kCombatInfo, uint uiParentEventID);
	static void ResolveRangedCityVsCityCombat(const CvCombatInfo& kCombatInfo, uint uiParentEventID);
	static void ResolveMeleeCombat(const CvCombatInfo& kInfo, uint uiParentEventID);
	static void ResolveCityMeleeCombat(const CvCombatInfo& kCombatInfo, uint uiParentEventID);
	static void ResolveAirUnitVsCombat(const CvCombatInfo& kInfo, uint uiParentEventID);
	static void ResolveAirSweep(const CvCombatInfo& kInfo, uint uiParentEventID);
	static void ResolveNuclearCombat(const CvCombatInfo& kCombatInfo, uint uiParentEventID);

	static void GenerateNuclearExplosionDamage(CvPlot* pkTargetPlot, int iDamageLevel, CvUnit* pkAttacker, CvCombatMemberEntry* pkDamageArray, int* piDamageMembers, int iMaxDamageMembers);
	static uint ApplyNuclearExplosionDamage(const CvCombatMemberEntry* pkDamageArray, int iDamageMembers, CvUnit* pkAttacker, CvPlot* pkTargetPlot, int iDamageLevel);

	static void ApplyPostCombatTraitEffects(CvUnit* pkWinner, CvUnit* pkLoser);
	static void ApplyPostCityCombatEffects(CvUnit* pkAttacker, CvCity* pkDefender, int iAttackerDamageInflicted);
};

#endif
