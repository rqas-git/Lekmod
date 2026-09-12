













#include <CvGameCoreDLLPCH.h>
#include "CvLuaSupport.h"
#include "CvLuaTeam.h"
#include "CvLuaTeamTech.h"


#define Method(Name)			\
	lua_pushcclosure(L, l##Name, 0);	\
	lua_setfield(L, t, #Name);


void CvLuaTeam::Register(lua_State* L)
{
	FLua::Details::CCallWithErrorHandling(L, pRegister);
}

void CvLuaTeam::HandleMissingInstance(lua_State* L)
{
	DefaultHandleMissingInstance(L);
}

void CvLuaTeam::PushMethods(lua_State* L, int t)
{
	Method(IsNone);

	Method(AddTeam);

	Method(CanChangeWarPeace);
	Method(CanDeclareWar);
	Method(DeclareWar);
	Method(MakePeace);
	Method(GetNumTurnsLockedIntoWar);
	Method(GetNumTurnsAtWar);
#ifdef LEKMOD_CITY_STATE_PEACE_LOCK_FROM_DECLARATION
	Method(IsCityStatePeaceLockFromOurDeclaration);
#endif
	Method(Meet);

	Method(GetScore);
	Method(GetPower);
	Method(GetDefensivePower);
	Method(GetNumNukeUnits);

	Method(GetTeamVotingForInDiplo);
	Method(GetProjectedVotesFromMinorAllies);
	Method(GetProjectedVotesFromLiberatedMinors);
	Method(GetProjectedVotesFromCivs);
	Method(GetTotalProjectedVotes);
	Method(GetTotalSecuredVotes);

	Method(GetAtWarCount);
	Method(GetHasMetCivCount);
	Method(HasMetHuman);
	Method(GetDefensivePactCount);

	Method(GetUnitClassMaking);
	Method(GetUnitClassCountPlusMaking);
	Method(GetBuildingClassMaking);
	Method(GetBuildingClassCountPlusMaking);

	Method(CountNumUnitsByArea);
	Method(CountNumCitiesByArea);
	Method(CountTotalPopulationByArea);
	Method(CountEnemyDangerByArea);

	Method(IsHuman);
	Method(IsBarbarian);

	Method(IsMinorCiv);
	Method(IsMinorCivWarmonger);

	Method(GetLeaderID);
	Method(GetSecretaryID);
	Method(GetHandicapType);
	Method(GetName);
	Method(GetNameKey);

	Method(GetNumMembers);
	Method(IsAlive);
	Method(IsEverAlive);
	Method(GetNumCities);
	Method(GetTotalPopulation);
	Method(GetTotalLand);
	Method(GetNukeInterception);
	Method(ChangeNukeInterception);

	Method(GetForceTeamVoteEligibilityCount);
	Method(IsForceTeamVoteEligible);
	Method(ChangeForceTeamVoteEligibilityCount);
	Method(GetExtraWaterSeeFromCount);
	Method(IsExtraWaterSeeFrom);
	Method(ChangeExtraWaterSeeFromCount);
	Method(GetMapTradingCount);
	Method(IsMapTrading);
	Method(ChangeMapTradingCount);
	Method(GetTechTradingCount);
	Method(IsTechTrading);
	Method(ChangeTechTradingCount);
	Method(GetGoldTradingCount);
	Method(IsGoldTrading);
	Method(ChangeGoldTradingCount);
	Method(GetAllowEmbassyTradingAllowedCount);
	Method(IsAllowEmbassyTradingAllowed);
	Method(ChangeAllowEmbassyTradingAllowedCount);
	Method(GetOpenBordersTradingAllowedCount);
	Method(IsOpenBordersTradingAllowed);
	Method(IsOpenBordersTradingAllowedWithTeam);
	Method(ChangeOpenBordersTradingAllowedCount);
	Method(GetDefensivePactTradingAllowedCount);
	Method(IsDefensivePactTradingAllowed);
	Method(IsDefensivePactTradingAllowedWithTeam);
	Method(ChangeDefensivePactTradingAllowedCount);
	Method(IsResearchAgreementTradingAllowed);
	Method(IsResearchAgreementTradingAllowedWithTeam);
	Method(IsTradeAgreementTradingAllowed);
	Method(GetPermanentAllianceTradingCount);
	Method(IsPermanentAllianceTrading);
	Method(ChangePermanentAllianceTradingCount);
	Method(GetBridgeBuildingCount);
	Method(IsBridgeBuilding);
	Method(ChangeBridgeBuildingCount);
	Method(GetWaterWorkCount);
	Method(IsWaterWork);
	Method(ChangeWaterWorkCount);

	Method(GetBorderObstacleCount);
	Method(IsBorderObstacle);
	Method(ChangeBorderObstacleCount);

	Method(IsMapCentering);
	Method(SetMapCentering);

	Method(GetID);

	Method(GetTechShareCount);
	Method(IsTechShare);
	Method(ChangeTechShareCount);

	Method(GetExtraMoves);
	Method(ChangeExtraMoves);

	Method(CanEmbark);
	Method(CanDefensiveEmbark);

	Method(IsHasMet);
	Method(IsAtWar);
	Method(IsPermanentWarPeace);
	Method(SetPermanentWarPeace);

	Method(GetLiberatedByTeam);
	Method(GetKilledByTeam);

	Method(HasEmbassyAtTeam);
	Method(IsAllowsOpenBordersToTeam);
	Method(IsForcePeace);
	Method(IsDefensivePact);
	Method(GetRouteChange);
	Method(ChangeRouteChange);
	Method(GetProjectCount);
	Method(GetProjectDefaultArtType);
	Method(SetProjectDefaultArtType);
	Method(GetProjectArtType);
	Method(SetProjectArtType);
	Method(IsProjectMaxedOut);
	Method(IsProjectAndArtMaxedOut);
	Method(ChangeProjectCount);
	Method(GetProjectMaking);
	Method(GetUnitClassCount);
	Method(IsUnitClassMaxedOut);
	Method(GetBuildingClassCount);
	Method(IsBuildingClassMaxedOut);
	Method(GetObsoleteBuildingCount);
	Method(IsObsoleteBuilding);

	Method(IsHasResearchAgreement);
	Method(IsHasTradeAgreement);

	Method(IsHasTech);
	Method(SetHasTech);

	Method(GetTeamTechs);

	Method(GetImprovementYieldChange);
	Method(ChangeImprovementYieldChange);

	Method(IsHomeOfUnitedNations);

	Method(GetVictoryCountdown);
	Method(GetVictoryDelay);
	Method(CanLaunch);

	Method(GetVictoryPoints);
	Method(ChangeVictoryPoints);

	Method(GetCurrentEra);
	Method(SetCurrentEra);

	Method(UpdateEmbarkGraphics);
}

const char* CvLuaTeam::GetTypeName()
{
	return "Team";
}






int CvLuaTeam::pRegister(lua_State* L)
{
	lua_getglobal(L, "Teams");
	if(lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setglobal(L, "Teams");
	}

	for(int i = 0; i < MAX_TEAMS; ++i)
	{
		CvTeam* pkTeam = &(GET_TEAM((TeamTypes)i));
		CvLuaTeam::Push(L, pkTeam);
		lua_rawseti(L, -2, i);
	}

	return 0;
}




int CvLuaTeam::lIsNone(lua_State* L)
{
#ifdef AUI_WARNING_FIXES
	const bool bDoesNotExist = (GetInstance(L, 1, false) == NULL);
	lua_pushboolean(L, bDoesNotExist ? 1 : 0);
#else
	const bool bDoesNotExist = (GetInstance(L, false) == NULL);
	lua_pushboolean(L, bDoesNotExist);
#endif

	return 1;
}




int CvLuaTeam::lAddTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::addTeam);
}




int CvLuaTeam::lCanChangeWarPeace(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::canChangeWarPeace);
}



int CvLuaTeam::lCanDeclareWar(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::canDeclareWar);
}



int CvLuaTeam::lDeclareWar(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::declareWar);
}



int CvLuaTeam::lMakePeace(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::makePeace);
}



int CvLuaTeam::lGetNumTurnsLockedIntoWar(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::GetNumTurnsLockedIntoWar);
}



int CvLuaTeam::lGetNumTurnsAtWar(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::GetNumTurnsAtWar);
}

#ifdef LEKMOD_CITY_STATE_PEACE_LOCK_FROM_DECLARATION


int CvLuaTeam::lIsCityStatePeaceLockFromOurDeclaration(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsCityStatePeaceLockFromOurDeclaration);
}
#endif



int CvLuaTeam::lMeet(lua_State* L)
{
	CvTeam* pkTeam = GetInstance(L);
	TeamTypes eOtherTeam = (TeamTypes)lua_tointeger(L, 2);
	const bool bSuppressMessages = lua_toboolean(L, 3);

	pkTeam->meet(eOtherTeam, bSuppressMessages);
	return 0;
}


int CvLuaTeam::lGetScore(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::GetScore);
}



int CvLuaTeam::lGetPower(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getPower);
}



int CvLuaTeam::lGetDefensivePower(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getDefensivePower);
}



int CvLuaTeam::lGetNumNukeUnits(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getNumNukeUnits);
}



int CvLuaTeam::lGetTeamVotingForInDiplo(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::GetTeamVotingForInDiplo);
}



int CvLuaTeam::lGetProjectedVotesFromMinorAllies(lua_State *L)
{
	return BasicLuaMethod(L, &CvTeam::GetProjectedVotesFromMinorAllies);
}



int CvLuaTeam::lGetProjectedVotesFromLiberatedMinors(lua_State *L)
{
	return BasicLuaMethod(L, &CvTeam::GetProjectedVotesFromLiberatedMinors);
}



int CvLuaTeam::lGetProjectedVotesFromCivs(lua_State *L)
{
	return BasicLuaMethod(L, &CvTeam::GetProjectedVotesFromCivs);
}



int CvLuaTeam::lGetTotalProjectedVotes(lua_State *L)
{
	return BasicLuaMethod(L, &CvTeam::GetTotalProjectedVotes);
}



int CvLuaTeam::lGetTotalSecuredVotes(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::GetTotalSecuredVotes);
}



int CvLuaTeam::lGetAtWarCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getAtWarCount);
}



int CvLuaTeam::lGetHasMetCivCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getHasMetCivCount);
}



int CvLuaTeam::lHasMetHuman(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::hasMetHuman);
}



int CvLuaTeam::lGetDefensivePactCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getDefensivePactCount);
}



int CvLuaTeam::lGetUnitClassMaking(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getUnitClassMaking);
}



int CvLuaTeam::lGetUnitClassCountPlusMaking(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getUnitClassCountPlusMaking);
}



int CvLuaTeam::lGetBuildingClassMaking(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getBuildingClassMaking);
}



int CvLuaTeam::lGetBuildingClassCountPlusMaking(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getBuildingClassCountPlusMaking);
}




int CvLuaTeam::lCountNumUnitsByArea(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::countNumUnitsByArea);
}



int CvLuaTeam::lCountNumCitiesByArea(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::countNumCitiesByArea);
}



int CvLuaTeam::lCountTotalPopulationByArea(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::countTotalPopulationByArea);
}



int CvLuaTeam::lCountEnemyDangerByArea(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::countEnemyDangerByArea);
}



int CvLuaTeam::lIsHuman(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isHuman);
}



int CvLuaTeam::lIsBarbarian(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isBarbarian);
}



int CvLuaTeam::lIsMinorCiv(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isMinorCiv);
}



int CvLuaTeam::lIsMinorCivWarmonger(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsMinorCivWarmonger);
}



int CvLuaTeam::lGetLeaderID(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getLeaderID);
}



int CvLuaTeam::lGetSecretaryID(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getSecretaryID);
}



int CvLuaTeam::lGetHandicapType(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getHandicapType);
}



int CvLuaTeam::lGetName(lua_State* L)
{
	CvTeam* pkTeam = GetInstance(L);

	lua_pushstring(L, pkTeam->getName());
	return 1;
}



int CvLuaTeam::lGetNameKey(lua_State* L)
{
	CvTeam* pkTeam = GetInstance(L);

	lua_pushstring(L, pkTeam->getNameKey());
	return 1;
}


int CvLuaTeam::lGetNumMembers(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getNumMembers);
}



int CvLuaTeam::lIsAlive(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isAlive);
}



int CvLuaTeam::lIsEverAlive(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isEverAlive);
}



int CvLuaTeam::lGetNumCities(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getNumCities);
}



int CvLuaTeam::lGetTotalPopulation(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getTotalPopulation);
}



int CvLuaTeam::lGetTotalLand(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getTotalLand);
}



int CvLuaTeam::lGetNukeInterception(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getNukeInterception);
}



int CvLuaTeam::lChangeNukeInterception(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeNukeInterception);
}




int CvLuaTeam::lGetForceTeamVoteEligibilityCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getForceTeamVoteEligibilityCount);
}



int CvLuaTeam::lIsForceTeamVoteEligible(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isForceTeamVoteEligible);
}



int CvLuaTeam::lChangeForceTeamVoteEligibilityCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeForceTeamVoteEligibilityCount);
}



int CvLuaTeam::lGetExtraWaterSeeFromCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getExtraWaterSeeFromCount);
}



int CvLuaTeam::lIsExtraWaterSeeFrom(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isExtraWaterSeeFrom);
}



int CvLuaTeam::lChangeExtraWaterSeeFromCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeExtraWaterSeeFromCount);
}



int CvLuaTeam::lGetMapTradingCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getMapTradingCount);
}



int CvLuaTeam::lIsMapTrading(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isMapTrading);
}



int CvLuaTeam::lChangeMapTradingCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeMapTradingCount);
}



int CvLuaTeam::lGetTechTradingCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getTechTradingCount);
}



int CvLuaTeam::lIsTechTrading(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isTechTrading);
}



int CvLuaTeam::lChangeTechTradingCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeTechTradingCount);
}



int CvLuaTeam::lGetGoldTradingCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getGoldTradingCount);
}



int CvLuaTeam::lIsGoldTrading(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isGoldTrading);
}



int CvLuaTeam::lChangeGoldTradingCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeGoldTradingCount);
}


int CvLuaTeam::lGetAllowEmbassyTradingAllowedCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getAllowEmbassyTradingAllowedCount);
}


int CvLuaTeam::lIsAllowEmbassyTradingAllowed(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isAllowEmbassyTradingAllowed);
}


int CvLuaTeam::lChangeAllowEmbassyTradingAllowedCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeAllowEmbassyTradingAllowedCount);
}



int CvLuaTeam::lGetOpenBordersTradingAllowedCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getOpenBordersTradingAllowedCount);
}



int CvLuaTeam::lIsOpenBordersTradingAllowed(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isOpenBordersTradingAllowed);
}



int CvLuaTeam::lIsOpenBordersTradingAllowedWithTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isOpenBordersTradingAllowedWithTeam);
}



int CvLuaTeam::lChangeOpenBordersTradingAllowedCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeOpenBordersTradingAllowedCount);
}



int CvLuaTeam::lGetDefensivePactTradingAllowedCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getDefensivePactTradingAllowedCount);
}



int CvLuaTeam::lIsDefensivePactTradingAllowed(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isDefensivePactTradingAllowed);
}



int CvLuaTeam::lIsDefensivePactTradingAllowedWithTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isDefensivePactTradingAllowedWithTeam);
}



int CvLuaTeam::lChangeDefensivePactTradingAllowedCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeDefensivePactTradingAllowedCount);
}



int CvLuaTeam::lIsResearchAgreementTradingAllowed(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsResearchAgreementTradingAllowed);
}



int CvLuaTeam::lIsResearchAgreementTradingAllowedWithTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsResearchAgreementTradingAllowedWithTeam);
}



int CvLuaTeam::lIsTradeAgreementTradingAllowed(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsTradeAgreementTradingAllowed);
}



int CvLuaTeam::lGetPermanentAllianceTradingCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getPermanentAllianceTradingCount);
}



int CvLuaTeam::lIsPermanentAllianceTrading(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isPermanentAllianceTrading);
}



int CvLuaTeam::lChangePermanentAllianceTradingCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changePermanentAllianceTradingCount);
}



int CvLuaTeam::lGetBridgeBuildingCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getBridgeBuildingCount);
}



int CvLuaTeam::lIsBridgeBuilding(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isBridgeBuilding);
}



int CvLuaTeam::lChangeBridgeBuildingCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeBridgeBuildingCount);
}



int CvLuaTeam::lGetWaterWorkCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getWaterWorkCount);
}



int CvLuaTeam::lIsWaterWork(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isWaterWork);
}



int CvLuaTeam::lChangeWaterWorkCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeWaterWorkCount);
}



int CvLuaTeam::lGetBorderObstacleCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getBorderObstacleCount);
}



int CvLuaTeam::lIsBorderObstacle(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isBorderObstacle);
}



int CvLuaTeam::lChangeBorderObstacleCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeBorderObstacleCount);
}




int CvLuaTeam::lIsMapCentering(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isMapCentering);
}



int CvLuaTeam::lSetMapCentering(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::setMapCentering);
}




int CvLuaTeam::lGetID(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::GetID);
}



int CvLuaTeam::lGetTechShareCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getTechShareCount);
}



int CvLuaTeam::lIsTechShare(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isTechShare);
}



int CvLuaTeam::lChangeTechShareCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeTechShareCount);
}




int CvLuaTeam::lGetExtraMoves(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getExtraMoves);
}



int CvLuaTeam::lChangeExtraMoves(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeExtraMoves);
}


int CvLuaTeam::lCanEmbark(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::canEmbark);
}


int CvLuaTeam::lCanDefensiveEmbark(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::canDefensiveEmbark);
}



int CvLuaTeam::lIsHasMet(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isHasMet);
}



int CvLuaTeam::lIsAtWar(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isAtWar);
}



int CvLuaTeam::lIsPermanentWarPeace(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isPermanentWarPeace);
}



int CvLuaTeam::lSetPermanentWarPeace(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::setPermanentWarPeace);
}


int CvLuaTeam::lGetLiberatedByTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::GetLiberatedByTeam);
}


int CvLuaTeam::lGetKilledByTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::GetKilledByTeam);
}

int CvLuaTeam::lHasEmbassyAtTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::HasEmbassyAtTeam);
}


int CvLuaTeam::lIsAllowsOpenBordersToTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsAllowsOpenBordersToTeam);
}



int CvLuaTeam::lIsForcePeace(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isForcePeace);
}



int CvLuaTeam::lIsDefensivePact(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsHasDefensivePact);
}



int CvLuaTeam::lGetRouteChange(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getRouteChange);
}



int CvLuaTeam::lChangeRouteChange(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeRouteChange);
}



int CvLuaTeam::lGetProjectCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getProjectCount);
}



int CvLuaTeam::lGetProjectDefaultArtType(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getProjectDefaultArtType);
}



int CvLuaTeam::lSetProjectDefaultArtType(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::setProjectDefaultArtType);
}



int CvLuaTeam::lGetProjectArtType(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getProjectArtType);
}



int CvLuaTeam::lSetProjectArtType(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::setProjectArtType);
}



int CvLuaTeam::lIsProjectMaxedOut(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isProjectMaxedOut);
}



int CvLuaTeam::lIsProjectAndArtMaxedOut(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isProjectAndArtMaxedOut);
}



int CvLuaTeam::lChangeProjectCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeProjectCount);
}



int CvLuaTeam::lGetProjectMaking(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getProjectMaking);
}



int CvLuaTeam::lGetUnitClassCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getUnitClassCount);
}



int CvLuaTeam::lIsUnitClassMaxedOut(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isUnitClassMaxedOut);
}



int CvLuaTeam::lGetBuildingClassCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getBuildingClassCount);
}



int CvLuaTeam::lIsBuildingClassMaxedOut(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isBuildingClassMaxedOut);
}



int CvLuaTeam::lGetObsoleteBuildingCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getObsoleteBuildingCount);
}



int CvLuaTeam::lIsObsoleteBuilding(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isObsoleteBuilding);
}



int CvLuaTeam::lIsHasResearchAgreement(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsHasResearchAgreement);
}



int CvLuaTeam::lIsHasTradeAgreement(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsHasTradeAgreement);
}



int CvLuaTeam::lIsHasTech(lua_State* L)
{
	CvTeam* pkTeam = GetInstance(L);
	const TechTypes eTech = (TechTypes)lua_tointeger(L, 2);

	lua_pushboolean(L, pkTeam->GetTeamTechs()->HasTech(eTech));
	return 1;
}



int CvLuaTeam::lSetHasTech(lua_State* L)
{
	CvTeam* pkTeam = GetInstance(L);
	const TechTypes eIndex = (TechTypes)lua_tointeger(L, 2);
	const bool bNewValue = lua_toboolean(L, 3);
	const PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 4);
	const bool bFirst = lua_toboolean(L, 4);
	const bool bAnnounce = lua_toboolean(L, 5);

	pkTeam->setHasTech(eIndex, bNewValue, ePlayer, bFirst, bAnnounce);
	return 0;
}



int CvLuaTeam::lGetTeamTechs(lua_State* L)
{
	CvTeam* pkTeam = GetInstance(L);
	CvTeamTechs* pkTeamTechs = pkTeam->GetTeamTechs();

	CvLuaTeamTech::Push(L, pkTeamTechs);
	return 1;
}



int CvLuaTeam::lGetImprovementYieldChange(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getImprovementYieldChange);
}



int CvLuaTeam::lChangeImprovementYieldChange(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeImprovementYieldChange);
}




int CvLuaTeam::lIsHomeOfUnitedNations(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsHomeOfUnitedNations);
}



int CvLuaTeam::lGetVictoryCountdown(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getVictoryCountdown);
}



int CvLuaTeam::lGetVictoryDelay(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getVictoryDelay);
}



int CvLuaTeam::lCanLaunch(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::canLaunch);
}




int CvLuaTeam::lGetVictoryPoints(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getVictoryPoints);
}



int CvLuaTeam::lChangeVictoryPoints(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeVictoryPoints);
}



int CvLuaTeam::lGetCurrentEra(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::GetCurrentEra);
}


int CvLuaTeam::lSetCurrentEra(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::SetCurrentEra);
}


int CvLuaTeam::lUpdateEmbarkGraphics(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::UpdateEmbarkGraphics);
}
