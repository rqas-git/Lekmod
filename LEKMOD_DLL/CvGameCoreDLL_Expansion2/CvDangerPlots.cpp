






#include "CvGameCoreDLLPCH.h"
#include "CvDangerPlots.h"
#include "CvGameCoreUtils.h"
#include "CvAStar.h"
#include "CvEnumSerialization.h"
#include "CvDiplomacyAI.h"
#include "CvMilitaryAI.h"
#include "CvMinorCivAI.h"
#include "FireWorks/FRemark.h"


#include "LintFree.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4505 )
#endif

REMARK_GROUP("CvDangerPlots");


CvDangerPlots::CvDangerPlots(void)
	: m_ePlayer(NO_PLAYER)
#ifdef AUI_DANGER_PLOTS_FIX_USE_ARRAY_NOT_FFASTVECTOR
	, m_DangerPlots(NULL)
#else
	, m_bArrayAllocated(false)
#endif
	, m_bDirty(false)
{
	m_fMajorWarMod = GC.getAI_DANGER_MAJOR_APPROACH_WAR();
	m_fMajorHostileMod = GC.getAI_DANGER_MAJOR_APPROACH_HOSTILE();
	m_fMajorDeceptiveMod = GC.getAI_DANGER_MAJOR_APPROACH_DECEPTIVE();
	m_fMajorGuardedMod = GC.getAI_DANGER_MAJOR_APPROACH_GUARDED();
	m_fMajorAfraidMod = GC.getAI_DANGER_MAJOR_APPROACH_AFRAID();
	m_fMajorFriendlyMod = GC.getAI_DANGER_MAJOR_APPROACH_FRIENDLY();
	m_fMajorNeutralMod = GC.getAI_DANGER_MAJOR_APPROACH_NEUTRAL();
	m_fMinorNeutralrMod = GC.getAI_DANGER_MINOR_APPROACH_NEUTRAL();
	m_fMinorFriendlyMod = GC.getAI_DANGER_MINOR_APPROACH_FRIENDLY();
	m_fMinorBullyMod = GC.getAI_DANGER_MINOR_APPROACH_BULLY();
	m_fMinorConquestMod = GC.getAI_DANGER_MINOR_APPROACH_CONQUEST();
}


CvDangerPlots::~CvDangerPlots(void)
{
	Uninit();
}


void CvDangerPlots::Init(PlayerTypes ePlayer, bool bAllocate)
{
	Uninit();
	m_ePlayer = ePlayer;

	if(bAllocate)
	{
		int iGridSize = GC.getMap().numPlots();
		CvAssertMsg(iGridSize > 0, "iGridSize is zero");
#ifdef AUI_DANGER_PLOTS_FIX_USE_ARRAY_NOT_FFASTVECTOR
		m_DangerPlots = FNEW(uint[iGridSize], c_eCiv5GameplayDLL, 0);
		fill(m_DangerPlots, &m_DangerPlots[iGridSize - 1], 0);
#else
		m_DangerPlots.resize(iGridSize);
		m_bArrayAllocated = true;
		for(int i = 0; i < iGridSize; i++)
		{
			m_DangerPlots[i] = 0;
		}
#endif
	}
}


void CvDangerPlots::Uninit()
{
	m_ePlayer = NO_PLAYER;
#ifdef AUI_DANGER_PLOTS_FIX_USE_ARRAY_NOT_FFASTVECTOR
	if (m_DangerPlots)
		SAFE_DELETE_ARRAY(m_DangerPlots);
#else
	m_DangerPlots.clear();
	m_bArrayAllocated = false;
#endif
	m_bDirty = false;
}


void CvDangerPlots::UpdateDanger(bool bPretendWarWithAllCivs, bool bIgnoreVisibility)
{

#ifdef AUI_DANGER_PLOTS_FIX_USE_ARRAY_NOT_FFASTVECTOR
	if (!m_DangerPlots)
#else
	if(!m_bArrayAllocated)
#endif
	{
		return;
	}


	int iGridSize = GC.getMap().numPlots();
	CvAssertMsg(iGridSize == m_DangerPlots.size(), "iGridSize does not match number of DangerPlots");
#ifdef AUI_DANGER_PLOTS_FIX_USE_ARRAY_NOT_FFASTVECTOR
	fill(m_DangerPlots, &m_DangerPlots[iGridSize - 1], 0);
#else
	for(int i = 0; i < iGridSize; i++)
	{
		m_DangerPlots[i] = 0;
	}
#endif

	CvPlayer& thisPlayer = GET_PLAYER(m_ePlayer);
	TeamTypes thisTeam = thisPlayer.getTeam();


	for(int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		PlayerTypes ePlayer = (PlayerTypes)iPlayer;
		CvPlayer& loopPlayer = GET_PLAYER(ePlayer);
		TeamTypes eTeam = loopPlayer.getTeam();

		if(!loopPlayer.isAlive())
		{
			continue;
		}

		if(eTeam == thisTeam)
		{
			continue;
		}

		if(ShouldIgnorePlayer(ePlayer) && !bPretendWarWithAllCivs)
		{
			continue;
		}


		int iLoop;
		CvUnit* pLoopUnit = NULL;
		for(pLoopUnit = loopPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = loopPlayer.nextUnit(&iLoop))
		{
			if(ShouldIgnoreUnit(pLoopUnit, bIgnoreVisibility))
			{
				continue;
			}

			int iRange = pLoopUnit->baseMoves();
			if(pLoopUnit->canRangeStrike())
			{
				iRange += pLoopUnit->GetRange();
			}

			CvPlot* pUnitPlot = pLoopUnit->plot();
			AssignUnitDangerValue(pLoopUnit, pUnitPlot);
			CvPlot* pLoopPlot = NULL;

#ifdef AUI_HEXSPACE_DX_LOOPS
			for (int iDY = -iRange; iDY <= iRange; iDY++)
			{
				int iMaxDX = iRange - MAX(0, iDY);
				for (int iDX = -iRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
				{

					pLoopPlot = plotXY(pUnitPlot->getX(), pUnitPlot->getY(), iDX, iDY);
#else
			for(int iDX = -(iRange); iDX <= iRange; iDX++)
			{
				for(int iDY = -(iRange); iDY <= iRange; iDY++)
				{
					pLoopPlot = plotXYWithRangeCheck(pUnitPlot->getX(), pUnitPlot->getY(), iDX, iDY, iRange);
#endif
					if(!pLoopPlot || pLoopPlot == pUnitPlot)
					{
						continue;
					}

					if(!pLoopUnit->canMoveOrAttackInto(*pLoopPlot) && !pLoopUnit->canRangeStrikeAt(pLoopPlot->getX(),pLoopPlot->getY()))
					{
						continue;
					}

					AssignUnitDangerValue(pLoopUnit, pLoopPlot);
				}
			}
		}


		CvCity* pLoopCity;
		for(pLoopCity = loopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = loopPlayer.nextCity(&iLoop))
		{
			if(ShouldIgnoreCity(pLoopCity, bIgnoreVisibility))
			{
				continue;
			}

			int iRange = GC.getCITY_ATTACK_RANGE();
			CvPlot* pCityPlot = pLoopCity->plot();
			AssignCityDangerValue(pLoopCity, pCityPlot);
			CvPlot* pLoopPlot = NULL;

#ifdef AUI_HEXSPACE_DX_LOOPS
			int iMaxDX, iDX;
			for (int iDY = -iRange; iDY <= iRange; iDY++)
			{
				iMaxDX = iRange - MAX(0, iDY);
				for (iDX = -iRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
				{

					pLoopPlot = plotXY(pCityPlot->getX(), pCityPlot->getY(), iDX, iDY);
#else
			for(int iDX = -(iRange); iDX <= iRange; iDX++)
			{
				for(int iDY = -(iRange); iDY <= iRange; iDY++)
				{
					pLoopPlot = plotXYWithRangeCheck(pCityPlot->getX(), pCityPlot->getY(), iDX, iDY, iRange);
#endif
					if(!pLoopPlot)
					{
						continue;
					}

					AssignCityDangerValue(pLoopCity, pLoopPlot);
				}
			}
		}
	}


	int iCitadelValue = GetDangerValueOfCitadel();
#ifdef AUI_WARNING_FIXES
	uint iPlotLoop;
#else
	int iPlotLoop;
#endif
	CvPlot* pPlot, *pAdjacentPlot;
	for(iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

		if(pPlot->isRevealed(thisTeam))
		{
			ImprovementTypes eImprovement = pPlot->getRevealedImprovementType(thisTeam);
			if(eImprovement != NO_IMPROVEMENT && GC.getImprovementInfo(eImprovement)->GetNearbyEnemyDamage() > 0)
			{
				if(!ShouldIgnoreCitadel(pPlot, bIgnoreVisibility))
				{
					for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
					{
						pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

						if(pAdjacentPlot != NULL)
						{
							AddDanger(pAdjacentPlot->getX(), pAdjacentPlot->getY(), iCitadelValue, true);
						}
					}
				}
			}
		}
	}


	CvCity* pLoopCity;
	int iLoopCity = 0;
	for(pLoopCity = thisPlayer.firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = thisPlayer.nextCity(&iLoopCity))
	{
		int iThreatValue = GetCityDanger(pLoopCity);
		pLoopCity->SetThreatValue(iThreatValue);
	}

	m_bDirty = false;
}


void CvDangerPlots::AddDanger(int iPlotX, int iPlotY, int iValue, bool bWithinOneMove)
{
	const int idx = iPlotX + iPlotY * GC.getMap().getGridWidth();
#ifdef AUI_DANGER_PLOTS_FIX_ADD_DANGER_WITHIN_ONE_MOVE
	iValue &= ~0x1;
#else
	if (iValue > 0)
	{
		if (bWithinOneMove)
		{
			iValue |= 0x1;
		}
		else
		{
			iValue &= ~0x1;
		}
	}
#endif

	m_DangerPlots[idx] += iValue;
#ifdef AUI_DANGER_PLOTS_FIX_ADD_DANGER_WITHIN_ONE_MOVE
	if (bWithinOneMove)
	{
		m_DangerPlots[idx] |= 0x1;
	}
#endif
}


int CvDangerPlots::GetDanger(const CvPlot& pPlot) const
{
	const int idx = pPlot.getX() + pPlot.getY() * GC.getMap().getGridWidth();
	return m_DangerPlots[idx];
}


bool CvDangerPlots::IsUnderImmediateThreat(const CvPlot& pPlot) const
{
	return GetDanger(pPlot) & 0x1;
}


int CvDangerPlots::GetCityDanger(CvCity* pCity)
{
	CvAssertMsg(pCity, "pCity is null");
	if(!pCity) return 0;

	CvAssertMsg(pCity->getOwner() == m_ePlayer, "City does not belong to us");

	CvPlot* pPlot = pCity->plot();
	int iEvalRange = GC.getAI_DIPLO_PLOT_RANGE_FROM_CITY_HOME_FRONT();

	int iDangerValue = 0;

#ifdef AUI_HEXSPACE_DX_LOOPS
	CvPlot* pEvalPlot;
	for (int iDY = -iEvalRange; iDY <= iEvalRange; iDY++)
	{
		int iMaxDX = iEvalRange - MAX(0, iDY);
		for (int iDX = -iEvalRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
		{

			pEvalPlot = plotXY(pPlot->getX(), pPlot->getY(), iDX, iDY);
#else
	for(int iX = -iEvalRange; iX <= iEvalRange; iX++)
	{
		for(int iY = -iEvalRange; iY <= iEvalRange; iY++)
		{
			CvPlot* pEvalPlot = plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iX, iY, iEvalRange);
#endif
			if(!pEvalPlot)
			{
				continue;
			}

			iDangerValue += GetDanger(*pEvalPlot);
		}
	}

	return iDangerValue;
}

int CvDangerPlots::ModifyDangerByRelationship(PlayerTypes ePlayer, CvPlot* pPlot, int iDanger)
{
	CvAssertMsg(pPlot, "No plot passed in?");
	bool bIgnoreInFriendlyTerritory = false;
	int iResult = iDanger;


	if(GET_TEAM(GET_PLAYER(m_ePlayer).getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
	{
		return iResult;
	}


	if(GET_PLAYER(m_ePlayer).isHuman())
	{
		return 0;
	}

	if(GET_PLAYER(m_ePlayer).isMinorCiv())
	{
		if(!GET_TEAM(GET_PLAYER(m_ePlayer).getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
		{
			bIgnoreInFriendlyTerritory = true;
		}
	}
	else if(!GET_PLAYER(ePlayer).isMinorCiv())
	{

		switch(GET_PLAYER(m_ePlayer).GetDiplomacyAI()->GetMajorCivApproach(ePlayer,                       false))
		{
		case MAJOR_CIV_APPROACH_WAR:
			iResult = (int)(iResult * m_fMajorWarMod);
			break;
		case MAJOR_CIV_APPROACH_HOSTILE:
			iResult = (int)(iResult * m_fMajorHostileMod);
			bIgnoreInFriendlyTerritory = true;
			break;
		case MAJOR_CIV_APPROACH_DECEPTIVE:
			iResult = (int)(iResult * m_fMajorDeceptiveMod);
			bIgnoreInFriendlyTerritory = true;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iResult = (int)(iResult * m_fMajorGuardedMod);
			bIgnoreInFriendlyTerritory = true;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iResult = (int)(iResult * m_fMajorAfraidMod);
			bIgnoreInFriendlyTerritory = true;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iResult = (int)(iResult * m_fMajorFriendlyMod);
			bIgnoreInFriendlyTerritory = true;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			iResult = (int)(iResult * m_fMajorNeutralMod);
			bIgnoreInFriendlyTerritory = true;
			break;
		}
	}
	else
	{
		switch(GET_PLAYER(m_ePlayer).GetDiplomacyAI()->GetMinorCivApproach(ePlayer))
		{
		case MINOR_CIV_APPROACH_IGNORE:
			iResult = (int)(iResult * m_fMinorNeutralrMod);
			bIgnoreInFriendlyTerritory = true;
			break;
		case MINOR_CIV_APPROACH_FRIENDLY:
			iResult = (int)(iResult * m_fMinorFriendlyMod);
			bIgnoreInFriendlyTerritory = true;
			break;
		case MINOR_CIV_APPROACH_BULLY:
			iResult = (int)(iResult * m_fMinorBullyMod);
			break;
		case MINOR_CIV_APPROACH_CONQUEST:
			iResult = (int)(iResult * m_fMinorConquestMod);
			break;
		}
	}



	if(pPlot && pPlot->getOwner() == m_ePlayer && bIgnoreInFriendlyTerritory)
	{
		iResult = 0;
	}

	return iResult;
}




bool CvDangerPlots::IsDangerByRelationshipZero(PlayerTypes ePlayer, CvPlot* pPlot)
{
	CvAssertMsg(pPlot, "No plot passed in?");
	bool bIgnoreInFriendlyTerritory = false;
#ifdef AUI_DANGER_PLOTS_IS_DANGER_BY_RELATIONSHIP_ZERO_MINORS_DO_NOT_IGNORE_TRESSPASSERS
	bool bConsiderInFriendlyTerritory = false;
#endif


	if(GET_TEAM(GET_PLAYER(m_ePlayer).getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
	{
		return false;
	}


	if(GET_PLAYER(m_ePlayer).isHuman())
	{
		return true;
	}

	bool bResultMultiplierIsZero = false;
	if(GET_PLAYER(m_ePlayer).isMinorCiv())
	{
		if(!GET_TEAM(GET_PLAYER(m_ePlayer).getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
		{
#ifdef AUI_DANGER_PLOTS_IS_DANGER_BY_RELATIONSHIP_ZERO_MINORS_DO_NOT_IGNORE_TRESSPASSERS
			bConsiderInFriendlyTerritory = true;
#ifndef AUI_DANGER_PLOTS_FIX_IS_DANGER_BY_RELATIONSHIP_ZERO_MINORS_IGNORE_ALL_NONWARRED
			bIgnoreInFriendlyTerritory = true;
#endif
#elif defined(AUI_DANGER_PLOTS_FIX_IS_DANGER_BY_RELATIONSHIP_ZERO_MINORS_IGNORE_ALL_NONWARRED)
			return true;
#else
			bIgnoreInFriendlyTerritory = true;
#endif
		}
	}
	else if(!GET_PLAYER(ePlayer).isMinorCiv())
	{

		switch(GET_PLAYER(m_ePlayer).GetDiplomacyAI()->GetMajorCivApproach(ePlayer,                       false))
		{
		case MAJOR_CIV_APPROACH_WAR:
			bResultMultiplierIsZero = m_fMajorWarMod == 0.f;
			break;
		case MAJOR_CIV_APPROACH_HOSTILE:
			bResultMultiplierIsZero = m_fMajorHostileMod == 0.f;
			bIgnoreInFriendlyTerritory = true;
			break;
		case MAJOR_CIV_APPROACH_DECEPTIVE:
			bResultMultiplierIsZero = m_fMajorDeceptiveMod == 0.f;
			bIgnoreInFriendlyTerritory = true;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			bResultMultiplierIsZero = m_fMajorGuardedMod == 0.f;
			bIgnoreInFriendlyTerritory = true;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			bResultMultiplierIsZero = m_fMajorAfraidMod == 0.f;
			bIgnoreInFriendlyTerritory = true;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			bResultMultiplierIsZero = m_fMajorFriendlyMod == 0.f;
			bIgnoreInFriendlyTerritory = true;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			bResultMultiplierIsZero = m_fMajorNeutralMod == 0.f;
			bIgnoreInFriendlyTerritory = true;
			break;
		}
	}
	else
	{
		switch(GET_PLAYER(m_ePlayer).GetDiplomacyAI()->GetMinorCivApproach(ePlayer))
		{
		case MINOR_CIV_APPROACH_IGNORE:
			bResultMultiplierIsZero = m_fMinorNeutralrMod == 0.f;
			bIgnoreInFriendlyTerritory = true;
			break;
		case MINOR_CIV_APPROACH_FRIENDLY:
			bResultMultiplierIsZero = m_fMinorFriendlyMod == 0.f;
			bIgnoreInFriendlyTerritory = true;
			break;
		case MINOR_CIV_APPROACH_BULLY:
			bResultMultiplierIsZero = (m_fMinorBullyMod == 0.f);
			break;
		case MINOR_CIV_APPROACH_CONQUEST:
			bResultMultiplierIsZero = m_fMinorConquestMod == 0.f;
			break;
		}
	}



#ifdef AUI_DANGER_PLOTS_IS_DANGER_BY_RELATIONSHIP_ZERO_MINORS_DO_NOT_IGNORE_TRESSPASSERS
	if (pPlot && pPlot->getOwner() == m_ePlayer)
	{
		if (bConsiderInFriendlyTerritory)
			return false;
		if (bIgnoreInFriendlyTerritory)
			return true;
	}
#else
	if(pPlot && pPlot->getOwner() == m_ePlayer && bIgnoreInFriendlyTerritory)
	{
		return true;
	}
#endif

	return bResultMultiplierIsZero;
}



#ifdef AUI_CONSTIFY
bool CvDangerPlots::ShouldIgnorePlayer(PlayerTypes ePlayer) const
#else
bool CvDangerPlots::ShouldIgnorePlayer(PlayerTypes ePlayer)
#endif
{
	if(GET_PLAYER(m_ePlayer).isMinorCiv() != GET_PLAYER(ePlayer).isMinorCiv() && !GET_PLAYER(ePlayer).isBarbarian() && !GET_PLAYER(m_ePlayer).isBarbarian())
	{
		CvPlayer* pMinor = NULL;
		CvPlayer* pMajor;

		if(GET_PLAYER(m_ePlayer).isMinorCiv())
		{
			pMinor = &GET_PLAYER(m_ePlayer);
			pMajor = &GET_PLAYER(ePlayer);
		}
		else
		{
			pMinor = &GET_PLAYER(ePlayer);
			pMajor = &GET_PLAYER(m_ePlayer);
		}

		if(pMinor->GetMinorCivAI()->IsFriends(pMajor->GetID()))
		{
			return true;
		}


		if (!GET_PLAYER(m_ePlayer).isMinorCiv())
		{
			TeamTypes eMajorTeam = pMajor->getTeam();
			TeamTypes eMinorTeam = pMinor->getTeam();
			if (!GET_TEAM(eMajorTeam).isAtWar(eMinorTeam))
			{
				return true;
			}
		}
	}

	return false;
}


#ifdef AUI_CONSTIFY
bool CvDangerPlots::ShouldIgnoreUnit(const CvUnit* pUnit, bool bIgnoreVisibility) const
#else
bool CvDangerPlots::ShouldIgnoreUnit(CvUnit* pUnit, bool bIgnoreVisibility)
#endif
{
	if(!pUnit->IsCanAttack())
	{
		return true;
	}

#if defined(AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MINORS_SEE_MAJORS)
	if (pUnit->isInvisible(GET_PLAYER(m_ePlayer).getTeam(), false))
	{
		return true;
	}

	if (GET_PLAYER(m_ePlayer).isMinorCiv() && !GET_PLAYER(pUnit->getOwner()).isMinorCiv() && !pUnit->isBarbarian() &&
		GET_PLAYER(m_ePlayer).GetClosestFriendlyCity(*pUnit->plot(), AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MINORS_SEE_MAJORS))
		bIgnoreVisibility = true;
#endif

#ifdef AUI_DANGER_PLOTS_FIX_SHOULD_IGNORE_UNIT_IGNORE_VISIBILITY_PLOT
	if (!pUnit->plot()->isVisible(GET_PLAYER(m_ePlayer).getTeam()) && !bIgnoreVisibility)
#else
	if(!pUnit->plot()->isVisible(GET_PLAYER(m_ePlayer).getTeam()))
#endif
	{
		return true;
	}

#if !defined(AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MINORS_SEE_MAJORS)
	if(pUnit->isInvisible(GET_PLAYER(m_ePlayer).getTeam(), false))
	{
		return true;
	}
#endif

	CvPlot* pPlot = pUnit->plot();
	CvAssertMsg(pPlot, "Plot is null?")

	if(NULL != pPlot && !pPlot->isVisibleOtherUnit(m_ePlayer) && !bIgnoreVisibility)
	{
		return true;
	}


	if(pUnit->getDomainType() == DOMAIN_AIR)
	{
		return true;
	}

	return false;
}


#ifdef AUI_CONSTIFY
bool CvDangerPlots::ShouldIgnoreCity(const CvCity* pCity, bool bIgnoreVisibility) const
#else
bool CvDangerPlots::ShouldIgnoreCity(CvCity* pCity, bool bIgnoreVisibility)
#endif
{

	if(!pCity->isRevealed(GET_PLAYER(m_ePlayer).getTeam(), false)  && !bIgnoreVisibility)
	{
		return true;
	}

	return false;
}


#ifdef AUI_CONSTIFY
bool CvDangerPlots::ShouldIgnoreCitadel(const CvPlot* pCitadelPlot, bool bIgnoreVisibility) const
#else
bool CvDangerPlots::ShouldIgnoreCitadel(CvPlot* pCitadelPlot, bool bIgnoreVisibility)
#endif
{

	if(!pCitadelPlot->isRevealed(GET_PLAYER(m_ePlayer).getTeam())  && !bIgnoreVisibility)
	{
		return true;
	}

	PlayerTypes eOwner = pCitadelPlot->getOwner();
	if(eOwner != NO_PLAYER)
	{

		if(eOwner == m_ePlayer)
		{
			return true;
		}

		if(!atWar(GET_PLAYER(m_ePlayer).getTeam(), GET_PLAYER(eOwner).getTeam()))
		{
			return true;
		}
	}

	return false;
}



void CvDangerPlots::AssignUnitDangerValue(CvUnit* pUnit, CvPlot* pPlot)
{

	int iCombatValueCalc = 100;
	int iBaseUnitCombatValue = pUnit->GetBaseCombatStrengthConsideringDamage() * iCombatValueCalc;

	if(iBaseUnitCombatValue > 0)
	{

		if(!IsDangerByRelationshipZero(pUnit->getOwner(), pPlot))
		{




			CvIgnoreUnitsPathFinder& kPathFinder = GC.getIgnoreUnitsPathFinder();
			kPathFinder.SetData(pUnit);

			int iPlotX = pPlot->getX();
			int iPlotY = pPlot->getY();

			if(!kPathFinder.GeneratePath(pUnit->getX(), pUnit->getY(), iPlotX, iPlotY, 0, true           ))
			{
				return;
			}

			CvAStarNode* pNode = kPathFinder.GetLastNode();
			int iTurnsAway = pNode->m_iData2;
			iTurnsAway = max(iTurnsAway, 1);

			int iUnitCombatValue = iBaseUnitCombatValue / iTurnsAway;
			iUnitCombatValue = ModifyDangerByRelationship(pUnit->getOwner(), pPlot, iUnitCombatValue);
			AddDanger(iPlotX, iPlotY, iUnitCombatValue, iTurnsAway <= 1);
		}
	}
}



void CvDangerPlots::AssignCityDangerValue(CvCity* pCity, CvPlot* pPlot)
{
	int iCombatValue = pCity->getStrengthValue();
	iCombatValue = ModifyDangerByRelationship(pCity->getOwner(), pPlot, iCombatValue);
	AddDanger(pPlot->getX(), pPlot->getY(), iCombatValue, false);
}


int CvDangerPlots::GetDangerValueOfCitadel() const
{

	CvMilitaryAI* pMilitaryAI = GET_PLAYER(m_ePlayer).GetMilitaryAI();
	int iPower = pMilitaryAI->GetPowerOfStrongestBuildableUnit(DOMAIN_LAND);


	return iPower * 50;
}


void CvDangerPlots::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_ePlayer;
#ifdef AUI_DANGER_PLOTS_FIX_USE_ARRAY_NOT_FFASTVECTOR
	bool bArrayAllocated = false;
	kStream >> bArrayAllocated;
#else
	kStream >> m_bArrayAllocated;
#endif

	int iGridSize;
	kStream >> iGridSize;

#ifdef AUI_DANGER_PLOTS_FIX_USE_ARRAY_NOT_FFASTVECTOR
	if (m_DangerPlots)
		SAFE_DELETE_ARRAY(m_DangerPlots);
	if (bArrayAllocated)
	{
		m_DangerPlots = FNEW(uint[iGridSize], c_eCiv5GameplayDLL, 0);
#else
	m_DangerPlots.resize(iGridSize);
#endif
	for(int i = 0; i < iGridSize; i++)
	{
		kStream >> m_DangerPlots[i];
	}
#ifdef AUI_DANGER_PLOTS_FIX_USE_ARRAY_NOT_FFASTVECTOR
	}
#endif

	m_bDirty = false;
}


void CvDangerPlots::Write(FDataStream& kStream) const
{

	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << m_ePlayer;
#ifdef AUI_DANGER_PLOTS_FIX_USE_ARRAY_NOT_FFASTVECTOR
	kStream << (m_DangerPlots != NULL);
#else
	kStream << m_bArrayAllocated;
#endif

	int iGridSize = GC.getMap().getGridWidth() * GC.getMap().getGridHeight();
	kStream << iGridSize;
#ifdef AUI_DANGER_PLOTS_FIX_USE_ARRAY_NOT_FFASTVECTOR
	if (m_DangerPlots)
	{
		for (int i = 0; i < iGridSize; i++)
		{
			kStream << m_DangerPlots[i];
		}
	}
	else
	{
		for (int i = 0; i < iGridSize; i++)
		{
			kStream << (uint)0;
		}
	}
#else
	for(int i = 0; i < iGridSize; i++)
	{
		kStream << m_DangerPlots[i];
	}
#endif
}


void CvDangerPlots::SetDirty()
{
	m_bDirty = true;
}
