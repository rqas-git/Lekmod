






#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreUtils.h"
#include "CvTacticalAnalysisMap.h"
#include "CvMilitaryAI.h"
#include "cvStopWatch.h"
#include "CvDiplomacyAI.h"

#include "LintFree.h"





CvTacticalAnalysisCell::CvTacticalAnalysisCell(void):
	m_pEnemyMilitary(NULL),
	m_pEnemyCivilian(NULL),
	m_pNeutralMilitary(NULL),
	m_pNeutralCivilian(NULL),
	m_pFriendlyMilitary(NULL),
	m_pFriendlyCivilian(NULL),
	m_iDefenseModifier(0),
	m_iDeploymentScore(0),
	m_eTargetType(AI_TACTICAL_TARGET_NONE),
	m_iDominanceZoneID(-1)
{
	Clear();
}


void CvTacticalAnalysisCell::Clear()
{
	ClearFlags();
	m_pEnemyMilitary = NULL;
	m_pEnemyCivilian = NULL;
	m_pNeutralMilitary = NULL;
	m_pNeutralCivilian = NULL;
	m_pFriendlyMilitary = NULL;
	m_pFriendlyCivilian = NULL;
	m_iDefenseModifier = 0;
	m_iDeploymentScore = 0;
	m_eTargetType = AI_TACTICAL_TARGET_NONE;
	m_iDominanceZoneID = -1;
}

bool CvTacticalAnalysisCell::CanUseForOperationGathering()
{
	if(IsImpassableTerrain() || IsImpassableTerritory() || GetEnemyMilitaryUnit() || GetNeutralMilitaryUnit() || GetNeutralCivilianUnit() || IsFriendlyTurnEndTile() || IsEnemyCity() || IsNeutralCity())
	{
		return false;
	}

	return true;
}

bool CvTacticalAnalysisCell::CanUseForOperationGatheringCheckWater(bool bWater)
{
	if(bWater != IsWater() || IsImpassableTerrain() || IsImpassableTerritory() || GetEnemyMilitaryUnit() || GetNeutralMilitaryUnit() || GetNeutralCivilianUnit() || IsFriendlyTurnEndTile() || IsEnemyCity() || IsNeutralCity())
	{
		return false;
	}

	return true;
}





CvTacticalDominanceZone::CvTacticalDominanceZone(void)
{
	m_iDominanceZoneID = -1;
	m_eTerritoryType = TACTICAL_TERRITORY_NONE;
	m_eDominanceFlag = TACTICAL_DOMINANCE_NO_UNITS_VISIBLE;
	m_eOwner = NO_PLAYER;
	m_iCityID = -1;
	m_iAreaID = 0;
	m_iFriendlyStrength = 0;
	m_iEnemyStrength = 0;
	m_iFriendlyRangedStrength = 0;
	m_iEnemyRangedStrength = 0;
	m_iFriendlyUnitCount = 0;
	m_iEnemyUnitCount = 0;
	m_iFriendlyRangedUnitCount = 0;
	m_iEnemyRangedUnitCount = 0;
	m_iEnemyNavalUnitCount = 0;
	m_iZoneValue = 0;
	m_iRangeClosestEnemyUnit = MAX_INT;
	m_bIsWater = false;
	m_bIsNavalInvasion = false;
	m_pTempZoneCenter = NULL;
}


CvCity* CvTacticalDominanceZone::GetClosestCity() const
{
	if(m_eOwner != NO_PLAYER)
	{
		return GET_PLAYER(m_eOwner).getCity(m_iCityID);
	}

	return NULL;
}


void CvTacticalDominanceZone::SetClosestCity(CvCity* pCity)
{
	if(pCity != NULL)
	{
		m_iCityID = pCity->GetID();
	}
	else
	{
		m_iCityID = -1;
	}
}


int CvTacticalDominanceZone::GetRangeClosestEnemyUnit() const
{
	return m_iRangeClosestEnemyUnit;
}


void CvTacticalDominanceZone::SetRangeClosestEnemyUnit(int iRange)
{
	m_iRangeClosestEnemyUnit = iRange;
}


TacticalMoveZoneType CvTacticalDominanceZone::GetZoneType() const
{
	if(m_eTerritoryType == TACTICAL_TERRITORY_FRIENDLY)
	{
		if(m_eDominanceFlag == TACTICAL_DOMINANCE_FRIENDLY)
		{
			return AI_TACTICAL_MOVE_ZONE_FRIENDLY_WINNING;
		}
		else if(m_eDominanceFlag == TACTICAL_DOMINANCE_EVEN)
		{
			return AI_TACTICAL_MOVE_ZONE_FRIENDLY_EVEN;
		}
		else
		{
			return AI_TACTICAL_MOVE_ZONE_FRIENDLY_LOSING;
		}
	}

	else if(m_eTerritoryType == TACTICAL_TERRITORY_ENEMY)
	{
		if(m_eDominanceFlag == TACTICAL_DOMINANCE_FRIENDLY)
		{
			return AI_TACTICAL_MOVE_ZONE_ENEMY_WINNING;
		}
		else if(m_eDominanceFlag == TACTICAL_DOMINANCE_EVEN)
		{
			return AI_TACTICAL_MOVE_ZONE_ENEMY_EVEN;
		}
		else
		{
			return AI_TACTICAL_MOVE_ZONE_ENEMY_LOSING;
		}
	}

	else
	{
		return AI_TACTICAL_MOVE_ZONE_UNOWNED;
	}
}




CvTacticalAnalysisMap::CvTacticalAnalysisMap(void) :
	m_pPlots(NULL),
	m_iDominancePercentage(25),
	m_iUnitStrengthMultiplier(5),
	m_iTacticalRange(8),
	m_iBestFriendlyRange(0),
	m_bIgnoreLOS(false),
	m_pPlayer(NULL),
	m_iNumPlots(0)
{
	m_bIsBuilt = false;
	m_iTurnBuilt = -1;
	m_bAtWar = false;
	m_DominanceZones.clear();
}


CvTacticalAnalysisMap::~CvTacticalAnalysisMap(void)
{
	SAFE_DELETE_ARRAY(m_pPlots);
}


void CvTacticalAnalysisMap::Init(int iNumPlots)
{

	AI_PERF("AI-perf-tact.csv", "CvTacticalAnalysisMap::Init()" );

	if(m_pPlots)
	{
		SAFE_DELETE_ARRAY(m_pPlots);
	}
	m_pPlots = FNEW(CvTacticalAnalysisCell[iNumPlots], c_eCiv5GameplayDLL, 0);
	m_iNumPlots = iNumPlots;

	m_iDominancePercentage = GC.getAI_TACTICAL_MAP_DOMINANCE_PERCENTAGE();
}


void CvTacticalAnalysisMap::RefreshDataForNextPlayer(CvPlayer* pPlayer)
{
	if(m_pPlots)
	{
		if(pPlayer != m_pPlayer || m_iTurnBuilt < GC.getGame().getGameTurn())
		{
			m_pPlayer = pPlayer;
			m_iTurnBuilt = GC.getGame().getGameTurn();
			m_iTacticalRange = ((GC.getAI_TACTICAL_RECRUIT_RANGE() + GC.getGame().getCurrentEra()) * 2) / 3;
			m_iUnitStrengthMultiplier = GC.getAI_TACTICAL_MAP_UNIT_STRENGTH_MULTIPLIER() * m_iTacticalRange;

#ifdef AUI_PERF_LOGGING_FORMATTING_TWEAKS
			AI_PERF_FORMAT("AI-perf.csv", ("Tactical Analysis Map, Turn %03d, %s", GC.getGame().getGameTurn(), m_pPlayer->getCivilizationShortDescription()));
#else
			AI_PERF_FORMAT("AI-perf.csv", ("Tactical Analysis Map, Turn %d, %s", GC.getGame().getGameTurn(), m_pPlayer->getCivilizationShortDescription()) );
#endif

			m_bIsBuilt = false;



			if(!m_pPlayer->isBarbarian())
			{
				m_DominanceZones.clear();

				AddTemporaryZones();

#ifdef AUI_WARNING_FIXES
				for (uint iI = 0; iI < GC.getMap().numPlots(); iI++)
#else
				for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
#endif
				{
					CvAssertMsg((iI < m_iNumPlots), "Plot to be accessed exceeds allocation!");

					CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iI);
					if(pPlot == NULL)
					{

						m_pPlots[iI].Clear();
					}
					else
					{
						if(PopulateCell(iI, pPlot))
						{
							AddToDominanceZones(iI, &m_pPlots[iI]);
						}
					}
				}

				CalculateMilitaryStrengths();
				PrioritizeZones();
				LogZones();
				BuildEnemyUnitList();
				MarkCellsNearEnemy();

				m_bIsBuilt = true;
			}
		}
	}
}


void CvTacticalAnalysisMap::BuildEnemyUnitList()
{
	CvTacticalAnalysisEnemy enemy;
	m_EnemyUnits.clear();

	for(int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		const PlayerTypes ePlayer = (PlayerTypes)iPlayer;
		CvPlayer& kPlayer = GET_PLAYER(ePlayer);
		const TeamTypes eTeam = kPlayer.getTeam();


		if(kPlayer.isAlive() && GET_TEAM(eTeam).isAtWar(m_pPlayer->getTeam()))
		{
			int iLoop;
			CvUnit* pLoopUnit = NULL;
			for(pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
			{

				if(pLoopUnit->IsCanAttack())
				{
					m_EnemyUnits.push_back(pLoopUnit);
				}
			}
		}
	}
}


void CvTacticalAnalysisMap::MarkCellsNearEnemy()
{
	int iDistance;


#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getMap().numPlots(); iI++)
#else
	for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
#endif
	{
		bool bMarkedIt = false;

		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iI);
		if(m_pPlots[iI].IsRevealed() && !m_pPlots[iI].IsImpassableTerrain() && !m_pPlots[iI].IsImpassableTerritory())
		{

			if(!m_pPlots[iI].IsFriendlyCity())
			{
				if(!pPlot->isVisibleToEnemyTeam(m_pPlayer->getTeam()))
				{
					m_pPlots[iI].SetNotVisibleToEnemy(true);
				}
				else
				{
					for(unsigned int iUnitIndex = 0;  iUnitIndex < m_EnemyUnits.size() && !bMarkedIt; iUnitIndex++)
					{
						CvUnit* pUnit = m_EnemyUnits[iUnitIndex];
						if(pUnit->getArea() == pPlot->getArea())
						{

							iDistance = plotDistance(pUnit->getX(), pUnit->getY(), pPlot->getX(), pPlot->getY());
							if(iDistance == 0)
							{
								m_pPlots[iI].SetSubjectToAttack(true);
								m_pPlots[iI].SetEnemyCanMovePast(true);
								bMarkedIt = true;
							}


							else if(iDistance <= pUnit->baseMoves())
							{
								int iTurnsToReach;
								iTurnsToReach = TurnsToReachTarget(pUnit, pPlot, true                , true                 );
								if(iTurnsToReach <= 1)
								{
									m_pPlots[iI].SetSubjectToAttack(true);
								}
								if(iTurnsToReach == 0)
								{
									m_pPlots[iI].SetEnemyCanMovePast(true);
									bMarkedIt = true;
								}
							}
						}
					}


					if(!m_pPlots[iI].IsSubjectToAttack())
					{
						CvPlot* pAdjacentPlot;
						for(int jJ = 0; jJ < NUM_DIRECTION_TYPES; jJ++)
						{
							pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)jJ));
							if(pAdjacentPlot != NULL && pAdjacentPlot->getOwner() != NO_PLAYER)
							{
								if(atWar(m_pPlayer->getTeam(), GET_PLAYER(pAdjacentPlot->getOwner()).getTeam()))
								{
									ImprovementTypes eImprovement = pAdjacentPlot->getImprovementType();
									if(eImprovement != NO_IMPROVEMENT && GC.getImprovementInfo(eImprovement)->GetNearbyEnemyDamage() > 0)
									{
										m_pPlots[iI].SetSubjectToAttack(true);
										break;
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


void CvTacticalAnalysisMap::ClearDynamicFlags()
{
#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getMap().numPlots(); iI++)
#else
	for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
#endif
	{

		m_pPlots[iI].SetWithinRangeOfTarget(false);
		m_pPlots[iI].SetHelpsProvidesFlankBonus(false);
		m_pPlots[iI].SetSafeForDeployment(false);
		m_pPlots[iI].SetDeploymentScore(0);
	}
}


void CvTacticalAnalysisMap::SetTargetBombardCells(CvPlot* pTarget, int iRange, bool bIgnoreLOS)
{
	int iDX, iDY;
	CvPlot* pLoopPlot;
	int iPlotIndex;
#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX;
	for (iDY = -iRange; iDY <= iRange; iDY++)
	{
		iMaxDX = iRange - MAX(0, iDY);
		for (iDX = -iRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
#else
	int iPlotDistance;

	for(iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for(iDY = -(iRange); iDY <= iRange; iDY++)
#endif
		{
#ifdef AUI_HEXSPACE_DX_LOOPS
			if (iDX == 0 && iDY == 0)
				continue;
#endif
			pLoopPlot = plotXY(pTarget->getX(), pTarget->getY(), iDX, iDY);
			if(pLoopPlot != NULL)
			{
#ifndef AUI_HEXSPACE_DX_LOOPS
#ifdef AUI_FIX_HEX_DISTANCE_INSTEAD_OF_PLOT_DISTANCE
				iPlotDistance = hexDistance(iDX, iDY);
#else
				iPlotDistance = plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pTarget->getX(), pTarget->getY());
#endif
				if(iPlotDistance > 0 && iPlotDistance <= iRange)
#endif
				{
					iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
					if(m_pPlots[iPlotIndex].IsRevealed() && !m_pPlots[iPlotIndex].IsImpassableTerrain() && !m_pPlots[iPlotIndex].IsImpassableTerritory())
					{
						if(!m_pPlots[iPlotIndex].IsEnemyCity() && !m_pPlots[iPlotIndex].IsNeutralCity())
						{
							if(bIgnoreLOS || pLoopPlot->canSeePlot(pTarget, m_pPlayer->getTeam(), iRange, NO_DIRECTION))
							{
								m_pPlots[iPlotIndex].SetWithinRangeOfTarget(true);
							}
						}
					}
				}
			}
		}
	}
}


void CvTacticalAnalysisMap::SetTargetFlankBonusCells(CvPlot* pTarget)
{
	CvPlot* pLoopPlot;
	int iPlotIndex;


	if(pTarget->isWater())
	{
		return;
	}

	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(pTarget->getX(), pTarget->getY(), ((DirectionTypes)iI));
		if(pLoopPlot != NULL)
		{
			iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
			if(m_pPlots[iPlotIndex].IsRevealed() && !m_pPlots[iPlotIndex].IsImpassableTerrain() && !m_pPlots[iPlotIndex].IsImpassableTerritory())
			{
				if(!m_pPlots[iPlotIndex].IsFriendlyCity() && !m_pPlots[iPlotIndex].IsEnemyCity() && !m_pPlots[iPlotIndex].IsNeutralCity())
				{
					if(!m_pPlots[iPlotIndex].IsFriendlyTurnEndTile() && m_pPlots[iPlotIndex].GetEnemyMilitaryUnit() == NULL)
					{
						m_pPlots[iPlotIndex].SetHelpsProvidesFlankBonus(true);
					}
				}
			}
		}
	}
}




void CvTacticalAnalysisMap::AddTemporaryZones()
{
	CvTemporaryZone* pZone;
	CvTacticalAI* pTacticalAI = m_pPlayer->GetTacticalAI();

	if(pTacticalAI)
	{
		pTacticalAI->DropObsoleteZones();

		pZone = pTacticalAI->GetFirstTemporaryZone();
		while(pZone)
		{

			if(pZone->GetTargetType() != AI_TACTICAL_TARGET_CITY)
			{
				CvPlot* pPlot = GC.getMap().plot(pZone->GetX(), pZone->GetY());
				if(pPlot)
				{
					CvTacticalDominanceZone newZone;
					newZone.SetDominanceZoneID(m_DominanceZones.size());
					newZone.SetTerritoryType(TACTICAL_TERRITORY_TEMP_ZONE);
					newZone.SetOwner(NO_PLAYER);
					newZone.SetAreaID(pPlot->getArea());
					newZone.SetWater(pPlot->isWater());
					newZone.SetTempZoneCenter(pPlot);
					newZone.SetNavalInvasion(pZone->IsNavalInvasion());
					m_DominanceZones.push_back(newZone);
				}
			}

			pZone = pTacticalAI->GetNextTemporaryZone();
		}
	}
}


bool CvTacticalAnalysisMap::PopulateCell(int iIndex, CvPlot* pPlot)
{
	CvUnit* pLoopUnit;
#ifdef AUI_WARNING_FIXES
	uint iUnitLoop;
#else
	int iUnitLoop;
#endif
	CvTacticalAnalysisCell& cell = m_pPlots[iIndex];

	cell.Clear();

	cell.SetRevealed(pPlot->isRevealed(m_pPlayer->getTeam()));
	cell.SetVisible(pPlot->isVisible(m_pPlayer->getTeam()));
	cell.SetImpassableTerrain(pPlot->isImpassable() || pPlot->isMountain());
	cell.SetWater(pPlot->isWater());
	cell.SetOcean(pPlot->isWater() && !pPlot->isShallowWater());

	bool bImpassableTerritory = false;
	if(pPlot->isOwned())
	{
		TeamTypes eMyTeam = m_pPlayer->getTeam();
		TeamTypes ePlotTeam = pPlot->getTeam();

		if(eMyTeam != ePlotTeam && !GET_TEAM(eMyTeam).isAtWar(ePlotTeam) && !GET_TEAM(ePlotTeam).IsAllowsOpenBordersToTeam(eMyTeam))
		{
			bImpassableTerritory = true;
		}
		else if(pPlot->isCity())
		{
			if(pPlot->getOwner() == m_pPlayer->GetID())
			{
				cell.SetFriendlyCity(true);
			}
			else if(GET_TEAM(eMyTeam).isAtWar(ePlotTeam))
			{
				cell.SetEnemyCity(true);
			}
			else
			{
				cell.SetNeutralCity(true);
			}
		}

		if(m_pPlayer->GetID() == pPlot->getOwner())
		{
			cell.SetOwnTerritory(true);
		}

		if(GET_TEAM(eMyTeam).isFriendlyTerritory(ePlotTeam))
		{
			cell.SetFriendlyTerritory(true);
		}

		if(GET_TEAM(ePlotTeam).isAtWar(ePlotTeam))
		{
			cell.SetEnemyTerritory(true);
		}
	}
	else
	{
		cell.SetUnclaimedTerritory(true);
	}

	cell.SetImpassableTerritory(bImpassableTerritory);
	cell.SetDefenseModifier(pPlot->defenseModifier(NO_TEAM, true));

	if(pPlot->getNumUnits() > 0)
	{
		for(iUnitLoop = 0; iUnitLoop < pPlot->getNumUnits(); iUnitLoop++)
		{
			pLoopUnit = pPlot->getUnitByIndex(iUnitLoop);
			if(!pLoopUnit) continue;
			if(pLoopUnit->getOwner() == m_pPlayer->GetID())
			{
				if(pLoopUnit->IsCombatUnit())
				{

					cell.SetFriendlyMilitaryUnit(pLoopUnit);
				}
				else
				{

					cell.SetFriendlyCivilianUnit(pLoopUnit);
				}
			}
			else if(pLoopUnit->isEnemy(m_pPlayer->getTeam()))
			{
				if(pLoopUnit->IsCombatUnit())
				{

					cell.SetEnemyMilitaryUnit(pLoopUnit);
				}
				else
				{

					cell.SetEnemyCivilianUnit(pLoopUnit);
				}
			}
			else
			{
				if(pLoopUnit->IsCombatUnit())
				{

					cell.SetNeutralMilitaryUnit(pLoopUnit);
				}
				else
				{

					cell.SetNeutralCivilianUnit(pLoopUnit);
				}
			}
		}
	}


	bool bAdd = true;
	if(cell.IsImpassableTerrain() || cell.IsImpassableTerritory() || !cell.IsRevealed())
	{
		bAdd = false;
	}
	return bAdd;
}


void CvTacticalAnalysisMap::AddToDominanceZones(int iIndex, CvTacticalAnalysisCell* pCell)
{
	CvPlot* pPlot = GC.getMap().plotByIndex(iIndex);


	m_TempZone.SetAreaID(pPlot->getArea());
	m_TempZone.SetOwner(pPlot->getOwner());
	m_TempZone.SetWater(pPlot->isWater());
	if(!pPlot->isOwned())
	{
		m_TempZone.SetTerritoryType(TACTICAL_TERRITORY_NO_OWNER);
	}
	else if(pPlot->getTeam() == m_pPlayer->getTeam())
	{
		m_TempZone.SetTerritoryType(TACTICAL_TERRITORY_FRIENDLY);
	}
	else if(GET_TEAM(m_pPlayer->getTeam()).isAtWar(pPlot->getTeam()))
	{
		m_TempZone.SetTerritoryType(TACTICAL_TERRITORY_ENEMY);
	}
	else
	{
		m_TempZone.SetTerritoryType(TACTICAL_TERRITORY_NEUTRAL);
	}
	m_TempZone.SetClosestCity(NULL);
	if(m_TempZone.GetTerritoryType() == TACTICAL_TERRITORY_ENEMY ||
	        m_TempZone.GetTerritoryType() == TACTICAL_TERRITORY_NEUTRAL ||
	        m_TempZone.GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
	{
		int iLoop;
		int iBestDistance = MAX_INT;
		CvCity* pBestCity = NULL;

		for(CvCity* pLoopCity = GET_PLAYER(m_TempZone.GetOwner()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(m_TempZone.GetOwner()).nextCity(&iLoop))
		{
			int iDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pPlot->getX(), pPlot->getY());
			if(iDistance < iBestDistance)
			{
				iBestDistance = iDistance;
				pBestCity = pLoopCity;
			}
		}

		if(pBestCity != NULL)
		{
			m_TempZone.SetClosestCity(pBestCity);
		}
	}


	CvTacticalDominanceZone* pZone = FindExistingZone(pPlot);
	if(!pZone)
	{

		m_TempZone.SetDominanceZoneID(m_DominanceZones.size());
		m_DominanceZones.push_back(m_TempZone);
		pZone = &m_DominanceZones[m_DominanceZones.size() - 1];
	}


	if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_NO_OWNER ||
	        pZone->GetTerritoryType() == TACTICAL_TERRITORY_TEMP_ZONE)
	{
		CvUnit* pFriendlyUnit = pCell->GetFriendlyMilitaryUnit();
		if(pFriendlyUnit)
		{
			if(pFriendlyUnit->getDomainType() == DOMAIN_AIR ||
			        (pFriendlyUnit->getDomainType() == DOMAIN_LAND && !pZone->IsWater()) ||
			        (pFriendlyUnit->getDomainType() == DOMAIN_SEA && pZone->IsWater()))
			{
				int iStrength = pFriendlyUnit->GetBaseCombatStrengthConsideringDamage();
				if(iStrength == 0 && pFriendlyUnit->isEmbarked() && !pZone->IsWater())
				{
					iStrength = pFriendlyUnit->GetBaseCombatStrength(true);
				}
				pZone->AddFriendlyStrength(iStrength * m_iUnitStrengthMultiplier);
#if !defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
				pZone->AddFriendlyRangedStrength(pFriendlyUnit->GetMaxRangedCombatStrength(NULL,           NULL, true, true));
#else
				CvCombatInfo kCombatInfo;
				kCombatInfo.setUnit(BATTLE_UNIT_ATTACKER, pFriendlyUnit);
				kCombatInfo.setAttackIsRanged(true);
				pZone->AddFriendlyRangedStrength(pFriendlyUnit->GetMaxRangedCombatStrength(kCombatInfo));
#endif
				if(pFriendlyUnit->GetRange() > GetBestFriendlyRange())
				{
					SetBestFriendlyRange(pFriendlyUnit->GetRange());
				}
				if(pFriendlyUnit->IsRangeAttackIgnoreLOS())
				{
					SetIgnoreLOS(true);
				}
				pZone->AddFriendlyUnitCount(1);
				if(pFriendlyUnit->isRanged())
				{
					pZone->AddFriendlyRangedUnitCount(1);
				}
			}
		}

		CvUnit* pEnemyUnit = pCell->GetEnemyMilitaryUnit();
		if(pEnemyUnit)
		{
			if(pEnemyUnit->getDomainType() == DOMAIN_AIR ||
			        (pEnemyUnit->getDomainType() == DOMAIN_LAND && !pZone->IsWater()) ||
			        (pEnemyUnit->getDomainType() == DOMAIN_SEA && pZone->IsWater()))
			{
				int iStrength = pEnemyUnit->GetBaseCombatStrengthConsideringDamage();
				if(iStrength == 0 && pEnemyUnit->isEmbarked() && !pZone->IsWater())
				{
					iStrength = pEnemyUnit->GetBaseCombatStrength(true);
				}
				pZone->AddEnemyStrength(iStrength * m_iUnitStrengthMultiplier);
#if !defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
				pZone->AddEnemyRangedStrength(pEnemyUnit->GetMaxRangedCombatStrength(NULL,           NULL, true, true));
#else
				CvCombatInfo kCombatInfo;
				kCombatInfo.setUnit(BATTLE_UNIT_ATTACKER, pEnemyUnit);
				kCombatInfo.setAttackIsRanged(true);
				pZone->AddEnemyRangedStrength(pEnemyUnit->GetMaxRangedCombatStrength(kCombatInfo));
#endif
				pZone->AddEnemyUnitCount(1);
				if(pEnemyUnit->isRanged())
				{
					pZone->AddEnemyRangedUnitCount(1);
				}
				if (pEnemyUnit->getDomainType() == DOMAIN_SEA)
				{
					pZone->AddEnemyNavalUnitCount(1);
				}
			}
		}
	}


	pCell->SetDominanceZone(pZone->GetDominanceZoneID());
}


void CvTacticalAnalysisMap::CalculateMilitaryStrengths()
{

	CvTacticalDominanceZone* pZone;
	CvCity* pClosestCity = NULL;
	int iDistance;
	int iMultiplier;
	int iLoop;
	CvUnit* pLoopUnit;
	TeamTypes eTeam;

	eTeam = m_pPlayer->getTeam();

	for(unsigned int iI = 0; iI < m_DominanceZones.size(); iI++)
	{
		pZone = &m_DominanceZones[iI];

		if(pZone->GetTerritoryType() != TACTICAL_TERRITORY_NO_OWNER)
		{
			pClosestCity = pZone->GetClosestCity();
			if(pClosestCity)
			{

				int iCityHitPoints = pClosestCity->GetMaxHitPoints() - pClosestCity->getDamage();
				int iStrength = m_iTacticalRange * pClosestCity->getStrengthValue() * iCityHitPoints / GC.getMAX_CITY_HIT_POINTS();
				if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
				{
					pZone->AddFriendlyStrength(iStrength);
					pZone->AddFriendlyRangedStrength(pClosestCity->getStrengthValue());
				}
				else
				{
					pZone->AddEnemyStrength(iStrength);
					pZone->AddEnemyRangedStrength(pClosestCity->getStrengthValue());
				}


				for(pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
				{
					if(pLoopUnit->IsCombatUnit())
					{
						if(pLoopUnit->getDomainType() == DOMAIN_AIR ||
						        (pLoopUnit->getDomainType() == DOMAIN_LAND && !pZone->IsWater()) ||
						        (pLoopUnit->getDomainType() == DOMAIN_SEA && pZone->IsWater()))
						{
							iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pClosestCity->getX(), pClosestCity->getY());
							if (iDistance <= m_iTacticalRange)
							{
								iMultiplier = (m_iTacticalRange + 4 - iDistance);
								if(iMultiplier > 0)
								{
									int iUnitStrength = pLoopUnit->GetBaseCombatStrengthConsideringDamage();
									if(iUnitStrength == 0 && pLoopUnit->isEmbarked() && !pZone->IsWater())
									{
										iUnitStrength = pLoopUnit->GetBaseCombatStrength(true);
									}
									pZone->AddFriendlyStrength(iUnitStrength * iMultiplier * m_iUnitStrengthMultiplier);
#if !defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
									pZone->AddFriendlyRangedStrength(pLoopUnit->GetMaxRangedCombatStrength(NULL,           NULL, true, true));
#else
									CvCombatInfo kCombatInfo;
									kCombatInfo.setUnit(BATTLE_UNIT_ATTACKER, pLoopUnit);
									kCombatInfo.setAttackIsRanged(true);
									pZone->AddFriendlyRangedStrength(pLoopUnit->GetMaxRangedCombatStrength(kCombatInfo));
#endif
									if(pLoopUnit->GetRange() > GetBestFriendlyRange())
									{
										SetBestFriendlyRange(pLoopUnit->GetRange());
									}
									if(pLoopUnit->IsRangeAttackIgnoreLOS())
									{
										SetIgnoreLOS(true);
									}
									pZone->AddFriendlyUnitCount(1);
									if(pLoopUnit->isRanged())
									{
										pZone->AddFriendlyRangedUnitCount(1);
									}
								}
							}
						}
					}
				}


				for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
				{
					CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayerLoop);
					if(GET_TEAM(eTeam).isAtWar(kPlayer.getTeam()))
					{
						for(pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
						{
							if(pLoopUnit->IsCombatUnit())
							{
								if(pLoopUnit->getDomainType() == DOMAIN_AIR ||
								        (pLoopUnit->getDomainType() == DOMAIN_LAND && !pZone->IsWater()) ||
								        (pLoopUnit->getDomainType() == DOMAIN_SEA && pZone->IsWater()))
								{
									CvPlot* pPlot;
									pPlot = pLoopUnit->plot();
									if(pPlot)
									{
										bool bVisible = true;
										iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pClosestCity->getX(), pClosestCity->getY());
										if (iDistance <= m_iTacticalRange)
										{
											iMultiplier = (m_iTacticalRange + 4 - iDistance);
											if(!pPlot->isVisible(eTeam) && !pPlot->isAdjacentVisible(eTeam, false))
											{
												bVisible = false;
											}
											if(iMultiplier > 0)
											{
												int iUnitStrength = pLoopUnit->GetBaseCombatStrengthConsideringDamage();
												if(iUnitStrength == 0 && pLoopUnit->isEmbarked() && !pZone->IsWater())
												{
													iUnitStrength = pLoopUnit->GetBaseCombatStrength(true);
												}

												if(!bVisible)
												{
													iUnitStrength /= 2;
												}

												pZone->AddEnemyStrength(iUnitStrength * iMultiplier * m_iUnitStrengthMultiplier);
#if !defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
												int iRangedStrength = pLoopUnit->GetMaxRangedCombatStrength(NULL,           NULL, true, true);
#else
												CvCombatInfo kCombatInfo;
												kCombatInfo.setUnit(BATTLE_UNIT_ATTACKER, pLoopUnit);
												kCombatInfo.setAttackIsRanged(true);
												int iRangedStrength = pLoopUnit->GetMaxRangedCombatStrength(kCombatInfo);
#endif
												if(!bVisible)
												{
													iRangedStrength /= 2;
												}

												pZone->AddEnemyRangedStrength(iRangedStrength);

												if(bVisible)
												{
													pZone->AddEnemyUnitCount(1);
													if(iDistance < pZone->GetRangeClosestEnemyUnit())
													{
														pZone->SetRangeClosestEnemyUnit(iDistance);
													}
													if(pLoopUnit->isRanged())
													{
														pZone->AddEnemyRangedUnitCount(1);
													}
													if(pLoopUnit->getDomainType() == DOMAIN_SEA)
													{
														pZone->AddEnemyNavalUnitCount(1);
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
			}
		}
	}
}


void CvTacticalAnalysisMap::PrioritizeZones()
{

	CvTacticalDominanceZone* pZone;
	int iBaseValue;
	int iMultiplier;
	CvCity* pClosestCity = NULL;

	for(unsigned int iI = 0; iI < m_DominanceZones.size(); iI++)
	{

		pZone = &m_DominanceZones[iI];
		eTacticalDominanceFlags eDominance = ComputeDominance(pZone);


		iBaseValue = 1;


		if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_TEMP_ZONE)
		{
			iMultiplier = 1000;
		}
		else
		{
			pClosestCity = pZone->GetClosestCity();

			if(pClosestCity)
			{
				iBaseValue += (1 + (int)sqrt((float)pZone->GetClosestCity()->getPopulation()));

				if(pClosestCity->isCapital() && !pClosestCity->GetPlayer()->isMinorCiv())
				{
					iBaseValue *= 2;
				}

				if(m_pPlayer->GetTacticalAI()->IsTemporaryZoneCity(pClosestCity))
				{
					iBaseValue *= 20;
				}

				else if (pZone->GetClosestCity()->isVisible(m_pPlayer->getTeam(), false))
				{
					iBaseValue *= 4;


					int iMaxDamageMultiplier = 10;
					int iDamage = pClosestCity->getDamage();
					if (iDamage > (pClosestCity->GetMaxHitPoints() / iMaxDamageMultiplier))
					{
						iBaseValue *= (int)((iDamage + 1) * 10 / pClosestCity->GetMaxHitPoints());
					}
				}
			}

			if(!pZone->IsWater())
			{
				iBaseValue *= 3;
			}


			iMultiplier = 1;
			if(eDominance == TACTICAL_DOMINANCE_ENEMY)
			{
				if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY)
				{
					iMultiplier = 2;
				}
				else if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
				{
					iMultiplier = 6;
				}
			}
			else if(eDominance == TACTICAL_DOMINANCE_EVEN)
			{
				if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY)
				{
					iMultiplier = 4;
				}
				else if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
				{
					iMultiplier = 4;
				}
			}
			else if(eDominance == TACTICAL_DOMINANCE_FRIENDLY)
			{
				if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY)
				{
					iMultiplier = 8;
				}
				else if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
				{
					iMultiplier = 1;
				}
			}
			if(!m_pPlayer->isMinorCiv())
			{
				if(m_pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_WINNING)
				{
					if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY)
					{
						iMultiplier *= 2;
					}
				}
				else if(m_pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
				{
					if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
					{
						iMultiplier *= 4;
					}
				}
			}
		}


		if((iBaseValue * iMultiplier) <= 0)
		{
			FAssertMsg((iBaseValue * iMultiplier) > 0, "Invalid Dominance Zone Value");
		}
		pZone->SetDominanceZoneValue(iBaseValue * iMultiplier);
	}

	std::stable_sort(m_DominanceZones.begin(), m_DominanceZones.end());
}


void CvTacticalAnalysisMap::LogZones()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString szLogMsg;
		CvTacticalDominanceZone* pZone;

		for(unsigned int iI = 0; iI < m_DominanceZones.size(); iI++)
		{
			pZone = &m_DominanceZones[iI];

			szLogMsg.Format("Zone ID: %d, Area ID: %d, Value: %d, FRIENDLY Str: %d (%d), Ranged: %d (%d), ENEMY Str: %d (%d), Ranged: %d (%d), Closest Enemy: %d",
			                pZone->GetDominanceZoneID(), pZone->GetAreaID(), pZone->GetDominanceZoneValue(),
			                pZone->GetFriendlyStrength(), pZone->GetFriendlyUnitCount(), pZone->GetFriendlyRangedStrength(), pZone->GetFriendlyRangedUnitCount(),
			                pZone->GetEnemyStrength(), pZone->GetEnemyUnitCount(), pZone->GetEnemyRangedStrength(), pZone->GetEnemyRangedUnitCount(), pZone->GetRangeClosestEnemyUnit());
			if(pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY)
			{
				szLogMsg += ", Friendly";
			}
			else if(pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_ENEMY)
			{
				szLogMsg += ", Enemy";
			}
			else if(pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_EVEN)
			{
				szLogMsg += ", Even";
			}
			else if(pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_NO_UNITS_VISIBLE)
			{
				szLogMsg += ", No Units Visible";
			}

			if(pZone->IsWater())
			{
				szLogMsg += ", Water";
			}
			else
			{
				szLogMsg += ", Land";
			}

			if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_TEMP_ZONE)
			{
				szLogMsg += ", Temporary Zone";
			}
			else if(pZone->GetClosestCity())
			{
				szLogMsg += ", " + pZone->GetClosestCity()->getName();
				if (m_pPlayer->GetTacticalAI()->IsTemporaryZoneCity(pZone->GetClosestCity()))
				{
					szLogMsg += " (Temp)";
				}
			}

			m_pPlayer->GetTacticalAI()->LogTacticalMessage(szLogMsg, true                          );
		}
	}
}


CvTacticalDominanceZone* CvTacticalAnalysisMap::FindExistingZone(CvPlot* pPlot)
{
	CvTacticalDominanceZone* pZone;

	for(unsigned int iI = 0; iI < m_DominanceZones.size(); iI++)
	{
		pZone = &m_DominanceZones[iI];


		if((pZone->GetTerritoryType() == TACTICAL_TERRITORY_TEMP_ZONE) &&
		        (m_TempZone.GetTerritoryType() == TACTICAL_TERRITORY_NO_OWNER || m_TempZone.GetTerritoryType() == TACTICAL_TERRITORY_NEUTRAL) &&
		        (plotDistance(pPlot->getX(), pPlot->getY(), pZone->GetTempZoneCenter()->getX(), pZone->GetTempZoneCenter()->getY()) <= m_iTacticalRange))
		{
			return pZone;
		}


		if((pZone->GetTerritoryType() == TACTICAL_TERRITORY_NO_OWNER || pZone->GetTerritoryType() == TACTICAL_TERRITORY_NEUTRAL) &&
		        (m_TempZone.GetTerritoryType() == TACTICAL_TERRITORY_NO_OWNER || m_TempZone.GetTerritoryType() == TACTICAL_TERRITORY_NEUTRAL))
		{
			if(pZone->GetAreaID() == m_TempZone.GetAreaID())
			{
				return pZone;
			}
		}


		if(pZone->GetTerritoryType() == m_TempZone.GetTerritoryType() &&
		        pZone->GetOwner() == m_TempZone.GetOwner() &&
		        pZone->GetAreaID() == m_TempZone.GetAreaID() &&
		        pZone->GetClosestCity() == m_TempZone.GetClosestCity())
		{
			return pZone;
		}
	}

	return NULL;
}


CvTacticalDominanceZone* CvTacticalAnalysisMap::GetZone(int iIndex)
{
	if(iIndex < 0 || iIndex >= (int)m_DominanceZones.size())
		return 0;
	return &m_DominanceZones[iIndex];
}


CvTacticalDominanceZone* CvTacticalAnalysisMap::GetZoneByCity(CvCity* pCity, bool bWater)
{
	CvTacticalDominanceZone* pZone;
	for(int iI = 0; iI < GetNumZones(); iI++)
	{
		pZone = GetZone(iI);
		if(pZone->GetClosestCity() == pCity && pZone->IsWater() == bWater)
		{
			return pZone;
		}
	}

	return NULL;
}


bool CvTacticalAnalysisMap::IsInEnemyDominatedZone(CvPlot* pPlot)
{
	CvTacticalAnalysisCell* pCell;
	int iPlotIndex;
	CvTacticalDominanceZone* pZone;

	iPlotIndex = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
	pCell = GetCell(iPlotIndex);

	for(int iI = 0; iI < GetNumZones(); iI++)
	{
		pZone = GetZone(iI);
		if(pZone->GetDominanceZoneID() == pCell->GetDominanceZone())
		{
			return (pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_ENEMY);
		}
	}

	return false;
}


eTacticalDominanceFlags CvTacticalAnalysisMap::ComputeDominance(CvTacticalDominanceZone* pZone)
{

	if(pZone->GetTerritoryType() != TACTICAL_TERRITORY_ENEMY && pZone->GetEnemyUnitCount() <= 0)
	{
		pZone->SetDominanceFlag(TACTICAL_DOMINANCE_NO_UNITS_VISIBLE);
	}

	else
	{

		if(pZone->GetEnemyStrength() <= 0)
		{
			pZone->SetDominanceFlag(TACTICAL_DOMINANCE_FRIENDLY);
		}
		else
		{
			int iRatio = (pZone->GetFriendlyStrength()  * 100) / pZone->GetEnemyStrength();
			if(iRatio > 100 + m_iDominancePercentage)
			{
				pZone->SetDominanceFlag(TACTICAL_DOMINANCE_FRIENDLY);
			}
			else if(iRatio < 100 - m_iDominancePercentage)
			{
				pZone->SetDominanceFlag(TACTICAL_DOMINANCE_ENEMY);
			}
			else
			{
				pZone->SetDominanceFlag(TACTICAL_DOMINANCE_EVEN);
			}
		}
	}

	return pZone->GetDominanceFlag();
}
