






#include "CvGameCoreDLLPCH.h"
#include "FFastVector.h"
#include "CvGameCoreUtils.h"
#include "CvHomelandAI.h"
#include "CvAStar.h"
#include "CvEconomicAI.h"
#include "CvWonderProductionAI.h"
#include "CvCitySpecializationAI.h"
#include "CvGrandStrategyAI.h"
#include "cvStopWatch.h"
#include "CvTypes.h"


#include "LintFree.h"

CvHomelandUnit::CvHomelandUnit() :
	m_iID(0)
	, m_iAuxData(0)
	, m_iMovesToTarget(0)
	, m_pTarget(NULL)
{
}






CvHomelandAI::CvHomelandAI(void)
{
#ifdef AUI_WARNING_FIXES
	m_pPlayer = NULL;
	Reset();
	m_iRandomRange = 0;
	m_iDefensiveMoveTurns = 0;
	m_iUpgradeMoveTurns = 0;
	m_fFlavorDampening = 0;
#endif
}


CvHomelandAI::~CvHomelandAI(void)
{
	Uninit();
}


void CvHomelandAI::Init(CvPlayer* pPlayer)
{

	m_pPlayer = pPlayer;

	Reset();


	m_iRandomRange = GC.getAI_HOMELAND_MOVE_PRIORITY_RANDOMNESS();
	m_iDefensiveMoveTurns = GC.getAI_HOMELAND_MAX_DEFENSIVE_MOVE_TURNS();
	m_iUpgradeMoveTurns = GC.getAI_HOMELAND_MAX_UPGRADE_MOVE_TURNS();
	m_fFlavorDampening = GC.getAI_TACTICAL_FLAVOR_DAMPENING_FOR_MOVE_PRIORITIZATION();
}


void CvHomelandAI::Uninit()
{
}


void CvHomelandAI::Reset()
{
	m_MovePriorityTurn = -1;
	m_CurrentBestMoveUnit = NULL;
	m_CurrentBestMoveHighPriorityUnit = NULL;
	m_iCurrentBestMoveUnitTurns = MAX_INT;
	m_iCurrentBestMoveHighPriorityUnitTurns = MAX_INT;
}


void CvHomelandAI::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_MovePriorityTurn;
}


void CvHomelandAI::Write(FDataStream& kStream)
{

	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << m_MovePriorityTurn;
}


void CvHomelandAI::RecruitUnits()
{
	CvUnit* pLoopUnit;
	int iLoop;

	m_CurrentTurnUnits.clear();


	for(pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{

		if(!pLoopUnit->TurnProcessed() && !pLoopUnit->isDelayedDeath() && pLoopUnit->AI_getUnitAIType() != UNITAI_UNKNOWN && pLoopUnit->canMove())
		{
			m_CurrentTurnUnits.push_back(pLoopUnit->GetID());
		}
	}
}


void CvHomelandAI::FindAutomatedUnits()
{
	CvUnit* pLoopUnit;
	int iLoop;

	m_CurrentTurnUnits.clear();


	for(pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{
		if(pLoopUnit->IsAutomated() && !pLoopUnit->TurnProcessed() && pLoopUnit->AI_getUnitAIType() != UNITAI_UNKNOWN && pLoopUnit->canMove())
		{
			m_CurrentTurnUnits.push_back(pLoopUnit->GetID());
		}
	}
}


void CvHomelandAI::DoTurn()
{
}


void CvHomelandAI::Update()
{
	AI_PERF_FORMAT("AI-perf.csv", ("Homeland AI, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()));


	if(!m_CurrentTurnUnits.empty())
	{

		CvEconomicAI* pEconomicAI = m_pPlayer->GetEconomicAI();
		pEconomicAI->UpdatePlots();


		EstablishHomelandPriorities();


		FindHomelandTargets();


		AssignHomelandMoves();
	}
}

typedef std::pair<int, CvPlot*> DistancePlotEntry;
typedef FFastVector<DistancePlotEntry> DistanceSortedPlotArray;


static bool SortUnitDistance(const DistancePlotEntry& kEntry1, const DistancePlotEntry& kEntry2)
{
	return kEntry1.first < kEntry2.first;
}

bool CvHomelandAI::IsAnyValidExploreMoves(const CvUnit* pUnit) const
{
	CvEconomicAI* pEconomicAI = m_pPlayer->GetEconomicAI();
	FFastVector<int>& aiExplorationPlots = pEconomicAI->GetExplorationPlots();
	FFastVector<int>& aiExplorationPlotRatings = pEconomicAI->GetExplorationPlotRatings();

	if (aiExplorationPlots.size() > 0)
	{
		int iUnitX = pUnit->getX();
		int iUnitY = pUnit->getY();


		DistanceSortedPlotArray aDistanceList;
		aDistanceList.reserve( aiExplorationPlots.size() );

		for(uint ui = 0; ui < aiExplorationPlots.size(); ui++)
		{
			int iPlot = aiExplorationPlots[ui];
			if(iPlot < 0)
			{
				continue;
			}

			CvPlot* pEvalPlot = GC.getMap().plotByIndex(iPlot);
			if(!pEvalPlot)
			{
				continue;
			}

			if(aiExplorationPlotRatings[ui] == 0)
			{
				continue;
			}

			int iDistX = abs( pEvalPlot->getX() - iUnitX );
			int iDistY = abs( pEvalPlot->getY() - iUnitY );

			aDistanceList.push_back(std::pair<int, CvPlot*>((iDistX*iDistX)+(iDistY*iDistY), pEvalPlot));

		}

		if (aDistanceList.size())
		{
			std::sort(aDistanceList.begin(), aDistanceList.end(), SortUnitDistance);

			for (DistanceSortedPlotArray::const_iterator itr = aDistanceList.begin(); itr != aDistanceList.end(); ++itr)
			{
				CvPlot* pEvalPlot = (*itr).second;
				if(!IsValidExplorerEndTurnPlot(pUnit, pEvalPlot))
				{
					continue;
				}

				bool bCanFindPath = GC.getPathFinder().GenerateUnitPath(pUnit, iUnitX, iUnitY, pEvalPlot->getX(), pEvalPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER           , true          );
				if(!bCanFindPath)
				{
					continue;
				}

				return true;
			}
		}
	}


	return false;
}




void CvHomelandAI::EstablishHomelandPriorities()
{
	int iPriority = 0;
	int iFlavorDefense = 0;
	int iFlavorOffense = 0;
	int iFlavorExpand = 0;
	int iFlavorImprove = 0;
	int iFlavorNavalImprove = 0;
	int iFlavorExplore = 0;
	int iFlavorGold = 0;
	int iFlavorScience = 0;
	int iFlavorWonder = 0;
	int iFlavorMilitaryTraining = 0;
	int iFlavorCulture = 0;

	m_MovePriorityList.clear();
	m_MovePriorityTurn = GC.getGame().getGameTurn();


	for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_DEFENSE")
		{
			iFlavorDefense = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
			iFlavorDefense = (int)((double)iFlavorDefense * m_fFlavorDampening);
		}
		if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_OFFENSE")
		{
			iFlavorOffense = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
			iFlavorOffense = (int)((double)iFlavorOffense * m_fFlavorDampening);
		}
		else if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_EXPANSION")
		{
			iFlavorExpand = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
		}
		else if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_RECON")
		{
			iFlavorExplore = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
			iFlavorExplore = (int)((double)iFlavorExplore * m_fFlavorDampening);
		}
		else if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_GOLD")
		{
			iFlavorGold = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
		}
		else if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_SCIENCE")
		{
			iFlavorScience = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
		}
		else if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_WONDER")
		{
			iFlavorWonder = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
		}
		else if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_MILITARY_TRAINING")
		{
			iFlavorMilitaryTraining = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
		}
		else if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_CULTURE")
		{
			iFlavorCulture = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
		}
	}


	for(int iI = AI_HOMELAND_MOVE_UNASSIGNED + 1; iI < NUM_AI_HOMELAND_MOVES; iI++)
	{

		switch((AIHomelandMove)iI)
		{
		case AI_HOMELAND_MOVE_EXPLORE:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_EXPLORE();
			break;
		case AI_HOMELAND_MOVE_EXPLORE_SEA:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_EXPLORE_SEA();
			break;
		case AI_HOMELAND_MOVE_SETTLE:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_SETTLE();
			break;
		case AI_HOMELAND_MOVE_GARRISON:

			if(m_pPlayer->GetPlayerPolicies()->HasPolicyEncouragingGarrisons())
			{
				iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_SENTRY() + 1;
			}
			else
			{
				iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_GARRISON();
			}
			break;
		case AI_HOMELAND_MOVE_HEAL:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_HEAL();
			break;
		case AI_HOMELAND_MOVE_TO_SAFETY:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_TO_SAFETY();
			break;
		case AI_HOMELAND_MOVE_MOBILE_RESERVE:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_MOBILE_RESERVE();
			break;
		case AI_HOMELAND_MOVE_SENTRY:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_SENTRY();
			break;
		case AI_HOMELAND_MOVE_WORKER:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_WORKER();
			break;
		case AI_HOMELAND_MOVE_WORKER_SEA:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_WORKER_SEA();
			break;
		case AI_HOMELAND_MOVE_PATROL:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_PATROL();
			break;
		case AI_HOMELAND_MOVE_UPGRADE:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_UPGRADE();
			break;
		case AI_HOMELAND_MOVE_ANCIENT_RUINS:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_ANCIENT_RUINS();
			break;
		case AI_HOMELAND_MOVE_GARRISON_CITY_STATE:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_GARRISON_CITY_STATE();
			break;
		case AI_HOMELAND_MOVE_WRITER:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_WRITER();
			break;
		case AI_HOMELAND_MOVE_ARTIST_GOLDEN_AGE:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_ARTIST();
			break;
		case AI_HOMELAND_MOVE_MUSICIAN:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_MUSICIAN();
			break;
		case AI_HOMELAND_MOVE_SCIENTIST_FREE_TECH:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_SCIENTIST_FREE_TECH();
			break;
		case AI_HOMELAND_MOVE_ENGINEER_HURRY:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_ENGINEER_HURRY();
			break;
		case AI_HOMELAND_MOVE_GENERAL_GARRISON:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_GENERAL_GARRISON();
			break;
		case AI_HOMELAND_MOVE_ADMIRAL_GARRISON:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_ADMIRAL_GARRISON();
			break;
		case AI_HOMELAND_MOVE_PROPHET_RELIGION:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_PROPHET_RELIGION();
			break;
		case AI_HOMELAND_MOVE_MISSIONARY:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_MISSIONARY();
			break;
		case AI_HOMELAND_MOVE_INQUISITOR:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_INQUISITOR();
			break;
		case AI_HOMELAND_MOVE_SPACESHIP_PART:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_SPACESHIP_PART();
			break;
		case AI_HOMELAND_MOVE_ADD_SPACESHIP_PART:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_ADD_SPACESHIP_PART();
			break;
		case AI_HOMELAND_MOVE_AIRCRAFT_TO_THE_FRONT:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_AIRCRAFT_TO_THE_FRONT();
			break;
		case AI_HOMELAND_MOVE_TREASURE:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_TREASURE();
			break;
		case AI_HOMELAND_MOVE_TRADE_UNIT:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_TRADE_UNIT();
			break;
		case AI_HOMELAND_MOVE_ARCHAEOLOGIST:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_ARCHAEOLOGIST();
			break;
		case AI_HOMELAND_MOVE_AIRLIFT:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_AIRLIFT();
			break;
		}


		if(iPriority >= 0)
		{

			switch((AIHomelandMove)iI)
			{

			case AI_HOMELAND_MOVE_GARRISON:
			case AI_HOMELAND_MOVE_HEAL:
			case AI_HOMELAND_MOVE_TO_SAFETY:
			case AI_HOMELAND_MOVE_MOBILE_RESERVE:
			case AI_HOMELAND_MOVE_SENTRY:
			case AI_HOMELAND_MOVE_GARRISON_CITY_STATE:
			case AI_HOMELAND_MOVE_GENERAL_GARRISON:
			case AI_HOMELAND_MOVE_ADMIRAL_GARRISON:
			case AI_HOMELAND_MOVE_AIRCRAFT_TO_THE_FRONT:
			case AI_HOMELAND_MOVE_TREASURE:


			case AI_HOMELAND_MOVE_MISSIONARY:
			case AI_HOMELAND_MOVE_INQUISITOR:
			case AI_HOMELAND_MOVE_PROPHET_RELIGION:
			case AI_HOMELAND_MOVE_SPACESHIP_PART:
			case AI_HOMELAND_MOVE_ADD_SPACESHIP_PART:
				iPriority += iFlavorDefense;
				break;


			case AI_HOMELAND_MOVE_EXPLORE:
			case AI_HOMELAND_MOVE_EXPLORE_SEA:
				iPriority += iFlavorExplore;
				break;

			case AI_HOMELAND_MOVE_SETTLE:
				iPriority += iFlavorExpand;
				break;

			case AI_HOMELAND_MOVE_WORKER:
				iPriority += iFlavorImprove;
				break;

			case AI_HOMELAND_MOVE_WORKER_SEA:
				iPriority += iFlavorNavalImprove;
				break;

			case AI_HOMELAND_MOVE_UPGRADE:
				iPriority += iFlavorMilitaryTraining;
				break;

			case AI_HOMELAND_MOVE_WRITER:
				iPriority += iFlavorCulture;
				break;

			case AI_HOMELAND_MOVE_ARTIST_GOLDEN_AGE:
				iPriority += iFlavorCulture;
				break;

			case AI_HOMELAND_MOVE_MUSICIAN:
				iPriority += iFlavorCulture;
				break;

			case AI_HOMELAND_MOVE_ANCIENT_RUINS:
				iPriority += iFlavorExplore;
				break;

			case AI_HOMELAND_MOVE_SCIENTIST_FREE_TECH:
				iPriority += iFlavorScience;
				break;

			case AI_HOMELAND_MOVE_ENGINEER_HURRY:
				iPriority += iFlavorWonder;
				break;

			case AI_HOMELAND_MOVE_TRADE_UNIT:
				iPriority += iFlavorGold;
				break;
			}


			CvHomelandMove move;
			move.m_eMoveType = (AIHomelandMove)iI;
			move.m_iPriority = iPriority;
			m_MovePriorityList.push_back(move);
		}
	}


	std::stable_sort(m_MovePriorityList.begin(), m_MovePriorityList.end());
}


void CvHomelandAI::FindHomelandTargets()
{
	int iI;
	CvPlot* pLoopPlot;
	CvHomelandTarget newTarget;


	m_TargetedCities.clear();
	m_TargetedSentryPoints.clear();
	m_TargetedForts.clear();
	m_TargetedNavalResources.clear();
	m_TargetedHomelandRoads.clear();
	m_TargetedAncientRuins.clear();
	m_TargetedAntiquitySites.clear();

	TeamTypes eTeam = m_pPlayer->getTeam();


	CvMap& theMap = GC.getMap();
	int iNumPlots = theMap.numPlots();
	for(iI = 0; iI < iNumPlots; iI++)
	{
		pLoopPlot = theMap.plotByIndexUnchecked(iI);

		if(pLoopPlot->isVisible(m_pPlayer->getTeam()))
		{


			CvCity* pCity = pLoopPlot->getPlotCity();
			if(pCity != NULL)
			{
				if(m_pPlayer->GetID() == pCity->getOwner())
				{

					UnitHandle pUnit = pLoopPlot->getBestDefender(m_pPlayer->GetID());
					if(!pUnit || !pUnit->isUnderTacticalControl())
					{
						newTarget.SetTargetType(AI_HOMELAND_TARGET_CITY);
						newTarget.SetTargetX(pLoopPlot->getX());
						newTarget.SetTargetY(pLoopPlot->getY());
						newTarget.SetAuxData((void*)pCity);
						newTarget.SetAuxIntData(pCity->getThreatValue());
						m_TargetedCities.push_back(newTarget);
					}
				}
			}


			else if(pLoopPlot->isWater() &&
			        pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
			{
				ResourceTypes eNonObsoleteResource = pLoopPlot->getNonObsoleteResourceType(eTeam);
				if(eNonObsoleteResource != NO_RESOURCE)
				{
					CvCity* pWorkingCity = pLoopPlot->getWorkingCity();
					if(NULL != pWorkingCity && pWorkingCity->getOwner() == m_pPlayer->GetID())
					{

						BuildTypes eBuild;
#ifdef AUI_WARNING_FIXES
						for (uint iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
#else
						for(int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
#endif
						{
							eBuild = ((BuildTypes)iJ);
							CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
							if(pkBuildInfo && pkBuildInfo->getImprovement() != NO_IMPROVEMENT && pkBuildInfo->IsWater())
							{
								if(GC.getImprovementInfo((ImprovementTypes) GC.getBuildInfo(eBuild)->getImprovement())->IsImprovementResourceTrade(eNonObsoleteResource))
								{
									newTarget.SetTargetType(AI_HOMELAND_TARGET_NAVAL_RESOURCE);
									newTarget.SetTargetX(pLoopPlot->getX());
									newTarget.SetTargetY(pLoopPlot->getY());
									newTarget.SetAuxData(pLoopPlot);
									newTarget.SetAuxIntData((int)eBuild);
									m_TargetedNavalResources.push_back(newTarget);
									break;
								}
							}
						}
					}
				}
			}


			else if(!m_pPlayer->isMinorCiv() && pLoopPlot->isGoody())
			{
				newTarget.SetTargetType(AI_HOMELAND_TARGET_ANCIENT_RUIN);
				newTarget.SetTargetX(pLoopPlot->getX());
				newTarget.SetTargetY(pLoopPlot->getY());
				newTarget.SetAuxData(pLoopPlot);
				m_TargetedAncientRuins.push_back(newTarget);
			}


			else if((pLoopPlot->getResourceType(eTeam) == GC.getARTIFACT_RESOURCE() || pLoopPlot->getResourceType(eTeam) == GC.getHIDDEN_ARTIFACT_RESOURCE()) && 
				!(pLoopPlot->getOwner() != NO_PLAYER && m_pPlayer->GetDiplomacyAI()->IsPlayerMadeNoDiggingPromise(pLoopPlot->getOwner())))
			{
				newTarget.SetTargetType(AI_HOMELAND_TARGET_ANTIQUITY_SITE);
				newTarget.SetTargetX(pLoopPlot->getX());
				newTarget.SetTargetY(pLoopPlot->getY());
				newTarget.SetAuxData(pLoopPlot);
				m_TargetedAntiquitySites.push_back(newTarget);
			}


			else if(pLoopPlot->isVisibleOtherUnit(m_pPlayer->GetID()))
			{
				CvUnit* pTargetUnit = pLoopPlot->getUnitByIndex(0);
				if(!pTargetUnit->isDelayedDeath() && atWar(eTeam, pTargetUnit->getTeam()) && !pTargetUnit->IsCanDefend())
				{
					newTarget.SetTargetType(AI_HOMELAND_TARGET_ANCIENT_RUIN);
					newTarget.SetTargetX(pLoopPlot->getX());
					newTarget.SetTargetY(pLoopPlot->getY());
					newTarget.SetAuxData(pLoopPlot);
					m_TargetedAncientRuins.push_back(newTarget);
				}
			}


			else if(!pLoopPlot->isWater() && (!pLoopPlot->isUnit() || pLoopPlot->getNumDefenders(m_pPlayer->GetID()) > 0))
			{

				if(pLoopPlot->getOwner() == m_pPlayer->GetID() ||
				        (pLoopPlot->isAdjacentTeam(eTeam, true              ) && pLoopPlot->getOwner() == NO_PLAYER))
				{

					int iOutsidePlots = pLoopPlot->GetNumAdjacentDifferentTeam(eTeam, true                 );

					if(iOutsidePlots > 0)
					{
						newTarget.SetTargetType(AI_HOMELAND_TARGET_SENTRY_POINT);
						newTarget.SetTargetX(pLoopPlot->getX());
						newTarget.SetTargetY(pLoopPlot->getY());
						newTarget.SetAuxData(pLoopPlot);


						int iWeight = iOutsidePlots * 100;
						iWeight += pLoopPlot->defenseModifier(eTeam, true);
						iWeight += m_pPlayer->GetPlotDanger(*pLoopPlot);

						CvCity* pFriendlyCity = m_pPlayer->GetClosestFriendlyCity(*pLoopPlot, 5                    );
						if(pFriendlyCity && pFriendlyCity->getOwner() == m_pPlayer->GetID())
						{
							iWeight += pFriendlyCity->getThreatValue() * pFriendlyCity->getPopulation() / 50;
							if(pFriendlyCity->isCapital())
							{
								iWeight = (iWeight * GC.getAI_MILITARY_CITY_THREAT_WEIGHT_CAPITAL()) / 100;
							}
						}

						if(pLoopPlot->isHills())
						{
							iWeight *= 2;
						}
						if(pLoopPlot->isCoastalLand())
						{
							iWeight /= 2;
						}

						newTarget.SetAuxIntData(iWeight);
						m_TargetedSentryPoints.push_back(newTarget);
					}
				}
			}


			else if(pLoopPlot->getTeam() == eTeam && pLoopPlot->isRoute())
			{
				newTarget.SetTargetType(AI_HOMELAND_TARGET_HOME_ROAD);
				newTarget.SetTargetX(pLoopPlot->getX());
				newTarget.SetTargetY(pLoopPlot->getY());
				newTarget.SetAuxData(pLoopPlot);
				m_TargetedHomelandRoads.push_back(newTarget);
			}
		}
	}


	EliminateAdjacentSentryPoints();
	EliminateAdjacentHomelandRoads();
	std::stable_sort(m_TargetedCities.begin(), m_TargetedCities.end());
}


void CvHomelandAI::AssignHomelandMoves()
{
	FStaticVector< CvHomelandMove, 64, true, c_eCiv5GameplayDLL >::iterator it;


	for(it = m_MovePriorityList.begin(); it != m_MovePriorityList.end() && !m_CurrentTurnUnits.empty(); ++it)
	{
		CvHomelandMove move = *it;

#if defined(AUI_PERF_LOGGING_FORMATTING_TWEAKS) && !defined(FINAL_RELEASE)
		static const char* aHomelandMoves[] = { "AI_HOMELAND_MOVE_UNASSIGNED",
			"AI_HOMELAND_MOVE_EXPLORE",
			"AI_HOMELAND_MOVE_EXPLORE_SEA",
			"AI_HOMELAND_MOVE_SETTLE",
			"AI_HOMELAND_MOVE_GARRISON",
			"AI_HOMELAND_MOVE_HEAL",
			"AI_HOMELAND_MOVE_TO_SAFETY",
			"AI_HOMELAND_MOVE_MOBILE_RESERVE",
			"AI_HOMELAND_MOVE_SENTRY",
			"AI_HOMELAND_MOVE_WORKER",
			"AI_HOMELAND_MOVE_WORKER_SEA",
			"AI_HOMELAND_MOVE_PATROL",
			"AI_HOMELAND_MOVE_UPGRADE",
			"AI_HOMELAND_MOVE_ANCIENT_RUINS",
			"AI_HOMELAND_MOVE_GARRISON_CITY_STATE",
			"AI_HOMELAND_MOVE_WRITER",
			"AI_HOMELAND_MOVE_ARTIST_GOLDEN_AGE",
			"AI_HOMELAND_MOVE_MUSICIAN",
			"AI_HOMELAND_MOVE_SCIENTIST_FREE_TECH",
			"AI_HOMELAND_MOVE_MERCHANT_TRADE",
			"AI_HOMELAND_MOVE_ENGINEER_HURRY",
			"AI_HOMELAND_MOVE_GENERAL_GARRISON",
			"AI_HOMELAND_MOVE_ADMIRAL_GARRISON",
			"AI_HOMELAND_MOVE_SPACESHIP_PART",
			"AI_HOMELAND_MOVE_AIRCRAFT_TO_THE_FRONT",
			"AI_HOMELAND_MOVE_TREASURE",
			"AI_HOMELAND_MOVE_PROPHET_RELIGION",
			"AI_HOMELAND_MOVE_MISSIONARY",
			"AI_HOMELAND_MOVE_INQUISITOR",
			"AI_HOMELAND_MOVE_TRADE_UNIT",
			"AI_HOMELAND_MOVE_ARCHAEOLOGIST",
			"AI_HOMELAND_MOVE_ADD_SPACESHIP_PART",
			"AI_HOMELAND_MOVE_AIRLIFT" };
		AI_PERF_FORMAT("AI-perf-home.csv", ("Move Type: %s (%d), Turn %03d, %s", (move.m_eMoveType > 0 && move.m_eMoveType < 33 ? aHomelandMoves[move.m_eMoveType] : "AI_HOMELAND_MOVE_NONE"), (int)move.m_eMoveType, GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()));
#else
		AI_PERF_FORMAT("AI-perf-tact.csv", ("Homeland Move: %d, Turn %03d, %s", (int)move.m_eMoveType, GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );
#endif

		switch(move.m_eMoveType)
		{
		case AI_HOMELAND_MOVE_EXPLORE:
			PlotExplorerMoves();
			break;
		case AI_HOMELAND_MOVE_EXPLORE_SEA:
			PlotExplorerSeaMoves();
			break;
		case AI_HOMELAND_MOVE_SETTLE:
			PlotFirstTurnSettlerMoves();
			break;
		case AI_HOMELAND_MOVE_GARRISON:
			PlotGarrisonMoves();
			break;
		case AI_HOMELAND_MOVE_HEAL:
			PlotHealMoves();
			break;
		case AI_HOMELAND_MOVE_TO_SAFETY:
			PlotMovesToSafety();
			break;
		case AI_HOMELAND_MOVE_MOBILE_RESERVE:
			PlotMobileReserveMoves();
			break;
		case AI_HOMELAND_MOVE_SENTRY:
			PlotSentryMoves();
			break;
		case AI_HOMELAND_MOVE_WORKER:
			PlotWorkerMoves();
			break;
		case AI_HOMELAND_MOVE_WORKER_SEA:
			PlotWorkerSeaMoves();
			break;
		case AI_HOMELAND_MOVE_PATROL:
			PlotPatrolMoves();
			break;
		case AI_HOMELAND_MOVE_UPGRADE:
			PlotUpgradeMoves();
			break;
		case AI_HOMELAND_MOVE_ANCIENT_RUINS:
			PlotAncientRuinMoves();
			break;
		case AI_HOMELAND_MOVE_GARRISON_CITY_STATE:
			PlotGarrisonMoves(true                   );
			break;
		case AI_HOMELAND_MOVE_WRITER:
			PlotWriterMoves();
			break;
		case AI_HOMELAND_MOVE_ARTIST_GOLDEN_AGE:
			PlotArtistMoves();
			break;
		case AI_HOMELAND_MOVE_MUSICIAN:
			PlotMusicianMoves();
			break;
		case AI_HOMELAND_MOVE_SCIENTIST_FREE_TECH:
			PlotScientistMoves();
			break;
		case AI_HOMELAND_MOVE_ENGINEER_HURRY:
			PlotEngineerMoves();
			break;
		case AI_HOMELAND_MOVE_MERCHANT_TRADE:
			PlotMerchantMoves();
			break;
		case AI_HOMELAND_MOVE_GENERAL_GARRISON:
			PlotGeneralMoves();
			break;
		case AI_HOMELAND_MOVE_ADMIRAL_GARRISON:
			PlotAdmiralMoves();
			break;
		case AI_HOMELAND_MOVE_PROPHET_RELIGION:
			PlotProphetMoves();
			break;
		case AI_HOMELAND_MOVE_MISSIONARY:
			PlotMissionaryMoves();
			break;
		case AI_HOMELAND_MOVE_INQUISITOR:
			PlotInquisitorMoves();
			break;
		case AI_HOMELAND_MOVE_AIRCRAFT_TO_THE_FRONT:
			PlotAircraftMoves();
			break;
		case AI_HOMELAND_MOVE_ADD_SPACESHIP_PART:
			PlotSSPartAdds();
			break;
		case AI_HOMELAND_MOVE_SPACESHIP_PART:
			PlotSSPartMoves();
			break;
		case AI_HOMELAND_MOVE_TREASURE:
			PlotTreasureMoves();
			break;
		case AI_HOMELAND_MOVE_TRADE_UNIT:
			PlotTradeUnitMoves();
			break;
		case AI_HOMELAND_MOVE_ARCHAEOLOGIST:
			PlotArchaeologistMoves();
			break;
		case AI_HOMELAND_MOVE_AIRLIFT:
			PlotAirliftMoves();
			break;
		}
	}

	ReviewUnassignedUnits();
}


void CvHomelandAI::PlotExplorerMoves()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_EXPLORE ||
#ifdef AUI_WARNING_FIXES
					(pUnit->IsAutomated() && pUnit->getDomainType() == DOMAIN_LAND && pUnit->GetAutomateType() == AUTOMATE_EXPLORE))
#else
			        pUnit->IsAutomated() && pUnit->getDomainType() == DOMAIN_LAND && pUnit->GetAutomateType() == AUTOMATE_EXPLORE)
#endif
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{

		ExecuteExplorerMoves();
		ExecuteExplorerMoves();
	}
}


void CvHomelandAI::PlotExplorerSeaMoves()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA ||
#ifdef AUI_WARNING_FIXES
					(pUnit->IsAutomated() && pUnit->getDomainType() == DOMAIN_SEA && pUnit->GetAutomateType() == AUTOMATE_EXPLORE))
#else
			        pUnit->IsAutomated() && pUnit->getDomainType() == DOMAIN_SEA && pUnit->GetAutomateType() == AUTOMATE_EXPLORE)
#endif
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{

		ExecuteExplorerMoves();
		ExecuteExplorerMoves();
	}
}


void CvHomelandAI::PlotFirstTurnSettlerMoves()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		bool bGoingToSettle = false;
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit && !pUnit->isHuman())
		{
			if(m_pPlayer->getNumCities() == 0 && m_CurrentMoveUnits.size() == 0)
			{
				if(pUnit->canFound(pUnit->plot()))
				{
					CvHomelandUnit unit;
					unit.SetID(pUnit->GetID());
					m_CurrentMoveUnits.push_back(unit);
					bGoingToSettle = true;
				}
			}
		}


		if(!bGoingToSettle && pUnit->isFound() && pUnit->getArmyID() == FFreeList::INVALID_INDEX)
		{
			pUnit->PushMission(CvTypes::getMISSION_SKIP());
			pUnit->finishMoves();
		}
	}

	if(!m_CurrentMoveUnits.empty())
	{
		ExecuteFirstTurnSettlerMoves();
	}
}


void CvHomelandAI::PlotGarrisonMoves(bool bCityStateOnly)
{

	if(bCityStateOnly != m_pPlayer->isMinorCiv())
	{
		return;
	}


	if(m_TargetedCities.size() > 0)
	{
		for(unsigned int iI = 0; iI < m_TargetedCities.size(); iI++)
		{
			CvPlot* pTarget = GC.getMap().plot(m_TargetedCities[iI].GetTargetX(), m_TargetedCities[iI].GetTargetY());
			CvCity* pCity = pTarget->getPlotCity();

			if(pCity && pCity->GetLastTurnGarrisonAssigned() < GC.getGame().getGameTurn())
			{

				FindUnitsForThisMove(AI_HOMELAND_MOVE_GARRISON, (iI == 0)              );

				if(m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
				{
					if(GetBestUnitToReachTarget(pTarget, m_iDefensiveMoveTurns))
					{
						ExecuteMoveToTarget(pTarget);

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Moving to garrison, X: %d, Y: %d, Priority: %d", m_TargetedCities[iI].GetTargetX(), m_TargetedCities[iI].GetTargetY(), m_TargetedCities[iI].GetAuxIntData());
							LogHomelandMessage(strLogString);
						}

						pCity->SetLastTurnGarrisonAssigned(GC.getGame().getGameTurn());
					}
				}
			}
		}
	}
}


void CvHomelandAI::PlotHealMoves()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit && !pUnit->isHuman())
		{

#ifdef AUI_HOMELAND_TWEAKED_HEAL_MOVES
			if (pUnit->healRate(pUnit->plot()) <= 0 || pUnit->isAlwaysHeal() || pUnit->isEmbarked())
				continue;
			if (((m_pPlayer->GetPlotDanger(*(pUnit->plot())) <= 0 && pUnit->GetCurrHitPoints() < pUnit->GetMaxHitPoints()) ||
				(m_pPlayer->GetPlotDanger(*(pUnit->plot())) > 0 && pUnit->GetCurrHitPoints() + pUnit->healRate(pUnit->plot()) < pUnit->GetMaxHitPoints())))
#else
			if(pUnit->GetCurrHitPoints() < pUnit->GetMaxHitPoints() && !pUnit->isEmbarked() && !pUnit->plot()->isCity())
#endif
			{

				if(pUnit->getDomainType() != DOMAIN_SEA || pUnit->plot()->IsFriendlyTerritory(m_pPlayer->GetID()))
				{
					if (!pUnit->IsUnderEnemyRangedAttack())
					{
						CvHomelandUnit unit;
						unit.SetID(pUnit->GetID());
						m_CurrentMoveUnits.push_back(unit);

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							CvString strTemp;

							strTemp = pUnit->getUnitInfo().GetDescription();
							strLogString.Format("%s healing at, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
						}
					}
				}
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteHeals();
	}
}


void CvHomelandAI::PlotMovesToSafety()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{

			CvPlot* pPlot = pUnit->plot();

			int iDangerLevel = m_pPlayer->GetPlotDanger(*pPlot);
			if(iDangerLevel > 0)
			{
				bool bAddUnit = false;



				if(!pUnit->IsCanDefend())
				{
					if (pUnit->IsAutomated() && pUnit->GetBaseCombatStrength() > 0)
					{

					}
					else
					{
						bAddUnit = true;
					}
				}


				else if(pUnit->GetCurrHitPoints() < pUnit->GetMaxHitPoints())
				{
					if(pUnit->isBarbarian())
					{

						if(pUnit->getDomainType() == DOMAIN_SEA)
						{
							bAddUnit = true;
						}
					}


					else if(pUnit->IsUnderEnemyRangedAttack() || pUnit->GetBaseCombatStrengthConsideringDamage() * 2 <= pUnit->GetBaseCombatStrength())
					{
						bAddUnit = true;
					}
				}


				else if(!pUnit->isBarbarian())
				{
					int iAcceptableDanger;
#ifdef AUI_HOMELAND_TWEAKED_ACCEPTABLE_DANGER
					iAcceptableDanger = pUnit->GetBaseCombatStrengthConsideringDamage() * (int)(AUI_HOMELAND_TWEAKED_ACCEPTABLE_DANGER + 0.5 +
						(100.0 - AUI_HOMELAND_TWEAKED_ACCEPTABLE_DANGER) * pow((double)pUnit->GetCurrHitPoints() / (double)pUnit->GetMaxHitPoints(), 2.0));
#else
					iAcceptableDanger = pUnit->GetBaseCombatStrengthConsideringDamage() * 100;
#endif
					if(iDangerLevel > iAcceptableDanger)
					{
						bAddUnit = true;
					}
				}

				if(bAddUnit)
				{

					CvHomelandUnit unit;
					unit.SetID(pUnit->GetID());
					m_CurrentMoveUnits.push_back(unit);
				}
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteMovesToSafestPlot();
	}
}


void CvHomelandAI::PlotMobileReserveMoves()
{

	if(!m_TargetedHomelandRoads.empty())
	{



		for(unsigned int iI = 0; iI < m_TargetedHomelandRoads.size(); iI++)
		{
			CvPlot* pTarget = GC.getMap().plot(m_TargetedHomelandRoads[iI].GetTargetX(), m_TargetedHomelandRoads[iI].GetTargetY());

			FindUnitsForThisMove(AI_HOMELAND_MOVE_MOBILE_RESERVE, (iI == 0)              );

			if(m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
			{
				if(GetBestUnitToReachTarget(pTarget, MAX_INT))
				{
					ExecuteMoveToTarget(pTarget);

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Moving to mobile reserve muster pt, X: %d, Y: %d", m_TargetedHomelandRoads[iI].GetTargetX(), m_TargetedHomelandRoads[iI].GetTargetY());
						LogHomelandMessage(strLogString);
					}
				}
			}
		}
	}
}


void CvHomelandAI::PlotSentryMoves()
{

	if(!m_TargetedSentryPoints.empty())
	{



		for(unsigned int iI = 0; iI < m_TargetedSentryPoints.size(); iI++)
		{
#ifndef AUI_PERF_LOGGING_FORMATTING_TWEAKS
			AI_PERF_FORMAT("Homeland-perf.csv", ("PlotSentryMoves, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );
#endif

			CvPlot* pTarget = GC.getMap().plot(m_TargetedSentryPoints[iI].GetTargetX(), m_TargetedSentryPoints[iI].GetTargetY());

			FindUnitsForThisMove(AI_HOMELAND_MOVE_SENTRY, (iI == 0)              );

			if(m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
			{
				if(GetBestUnitToReachTarget(pTarget, MAX_INT))
				{
					ExecuteMoveToTarget(pTarget);

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Moving to sentry point, X: %d, Y: %d, Priority: %d", m_TargetedSentryPoints[iI].GetTargetX(), m_TargetedSentryPoints[iI].GetTargetY(), m_TargetedSentryPoints[iI].GetAuxIntData());
						LogHomelandMessage(strLogString);
					}
				}
			}
		}
	}
}


void CvHomelandAI::PlotWorkerMoves()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_WORKER  ||
#ifdef AI_WORKER_EMBARKED_FIX
				(pUnit->IsAutomated() && (pUnit->getDomainType() == DOMAIN_LAND && (pUnit->getUnitInfo().GetUnitAIType(UNITAI_WORKER) || pUnit->getUnitInfo().GetUnitAIType(UNITAI_WORKER_SEA)) && pUnit->GetAutomateType() == AUTOMATE_BUILD)))
#else
			        pUnit->IsAutomated() && pUnit->getDomainType() == DOMAIN_LAND && pUnit->GetAutomateType() == AUTOMATE_BUILD)
#endif
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteWorkerMoves();
#ifdef AUI_HOMELAND_PLOT_WORKER_MOVES_ALSO_PLOTS_WORKER_DEFENSE
		FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL > tempList = m_CurrentMoveUnits;
		for (FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it = tempList.begin(); it != tempList.end(); ++it)
		{
			CvUnit* pTargetWorker = m_pPlayer->getUnit(it->GetID());

			if (pTargetWorker && !pTargetWorker->IsCombatUnit() && !pTargetWorker->isDelayedDeath() && !pTargetWorker->IsDead())
			{
				CvPlot* pTargetPlot = pTargetWorker->plot();

				FindUnitsForThisMove(AI_HOMELAND_MOVE_UNASSIGNED                                                , (it == tempList.begin())              );

				if (m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
				{
					if (GetBestUnitToReachTarget(pTargetPlot, 1))
					{
						ExecuteMoveToTarget(pTargetPlot);

						if (GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Moving to protect worker, X: %d, Y: %d", pTargetPlot->getX(), pTargetPlot->getY());
							LogHomelandMessage(strLogString);
						}
					}
				}
			}
		}
#endif
	}
}


void CvHomelandAI::PlotWorkerSeaMoves()
{
	ClearCurrentMoveUnits();

	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_WORKER_SEA  ||
#ifdef AUI_WARNING_FIXES
				(pUnit->IsAutomated() && pUnit->getDomainType() == DOMAIN_SEA && pUnit->GetAutomateType() == AUTOMATE_BUILD))
#else
#ifdef AI_WORKER_EMBARKED_FIX
				(pUnit->IsAutomated() && pUnit->getDomainType() == DOMAIN_SEA && pUnit->GetAutomateType() == AUTOMATE_BUILD))
				
#else
				(pUnit->IsAutomated() && pUnit->getDomainType() == DOMAIN_SEA && pUnit->GetAutomateType() == AUTOMATE_BUILD))
#endif
#endif
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator moveUnitIt;
	for(moveUnitIt = m_CurrentMoveUnits.begin(); moveUnitIt != m_CurrentMoveUnits.end(); ++moveUnitIt)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(moveUnitIt->GetID());
		int iTargetIndex = -1;
		int iTargetMoves = MAX_INT;


		for (unsigned int iI = 0; iI < m_TargetedNavalResources.size(); iI++)
		{

			CvPlot* pTarget = GC.getMap().plot(m_TargetedNavalResources[iI].GetTargetX(), m_TargetedNavalResources[iI].GetTargetY());

			if (!pUnit->canBuild(pTarget, (BuildTypes)m_TargetedNavalResources[iI].GetAuxIntData()))
			{
				continue;
			}

			int iMoves = TurnsToReachTarget(pUnit.pointer(), pTarget);
			if (iMoves < iTargetMoves)
			{
				iTargetMoves = iMoves;
				iTargetIndex = iI;
			}
		}

		if (iTargetIndex != -1)
		{

			int iTargetX = m_TargetedNavalResources[iTargetIndex].GetTargetX();
			int iTargetY = m_TargetedNavalResources[iTargetIndex].GetTargetY();
			CvPlot* pTarget = GC.getMap().plot(iTargetX, iTargetY);

			bool bResult = false;
			if(pUnit->UnitPathTo(iTargetX, iTargetY, 0) > 0)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), iTargetX, iTargetY);
				if(pUnit->plot() == pTarget)
				{
					pUnit->PushMission(CvTypes::getMISSION_BUILD(), m_TargetedNavalResources[iTargetIndex].GetAuxIntData(), -1, 0, (pUnit->GetLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pTarget);
					bResult = true;
				}
				else
				{
					pUnit->finishMoves();
				}


				UnitProcessed(m_CurrentMoveUnits.begin()->GetID());
			}
			else
			{
				if(pUnit->plot() == pTarget)
				{
					pUnit->PushMission(CvTypes::getMISSION_BUILD(), m_TargetedNavalResources[iTargetIndex].GetAuxIntData(), -1, 0, (pUnit->GetLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pTarget);
					bResult = true;
				}
			}
		
			if (bResult)
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Harvesting naval resource at, X: %d, Y: %d", m_TargetedNavalResources[iTargetIndex].GetTargetX(), m_TargetedNavalResources[iTargetIndex].GetTargetY());
					LogHomelandMessage(strLogString);
				}
			}
			else
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Moving toward naval resource at, X: %d, Y: %d", m_TargetedNavalResources[iTargetIndex].GetTargetX(), m_TargetedNavalResources[iTargetIndex].GetTargetY());
					LogHomelandMessage(strLogString);
				}
			}
		}
	}
}


void CvHomelandAI::PlotPatrolMoves()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit && !pUnit->isHuman() && pUnit->getDomainType() != DOMAIN_AIR && !pUnit->isTrade())
		{
#ifdef AUI_HOMELAND_FIND_PATROL_MOVES_CIVILIANS_PATROL_TO_SAFETY
			if (!pUnit->IsCombatUnit())
			{
				MoveCivilianToSafety(pUnit.pointer());
				continue;
			}
#endif
			CvPlot* pTarget = FindPatrolTarget(pUnit.pointer());
			if(pTarget)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				unit.SetTarget(pTarget);
				m_CurrentMoveUnits.push_back(unit);
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp;

					strTemp = pUnit->getUnitInfo().GetDescription();
					strLogString.Format("%s (%d) patrolling to, X: %d, Y: %d, Current X: %d, Current Y: %d", strTemp.GetCString(), pUnit->GetID(), pTarget->getX(), pTarget->getY(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecutePatrolMoves();
	}
}


void CvHomelandAI::PlotUpgradeMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator moveUnitIt;
	ResourceTypes eResource;
	int iNumResource;
	int iNumResourceInUnit;
	bool bMissingResource;

	ClearCurrentMoveUnits();
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{

		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit && !pUnit->isHuman() && pUnit->getArmyID() == -1)
		{

			UnitTypes eUpgradeUnitType = pUnit->GetUpgradeUnitType();
			if(eUpgradeUnitType != NO_UNIT)
			{

				TechTypes ePrereqTech = (TechTypes) GC.getUnitInfo(eUpgradeUnitType)->GetPrereqAndTech();
				if(ePrereqTech == NO_TECH || GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech(ePrereqTech))
				{

					bMissingResource = false;
#ifdef AUI_WARNING_FIXES
					for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos() && !bMissingResource; iResourceLoop++)
#else
					for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos() && !bMissingResource; iResourceLoop++)
#endif
					{
						eResource = (ResourceTypes) iResourceLoop;
						iNumResource = GC.getUnitInfo(eUpgradeUnitType)->GetResourceQuantityRequirement(eResource);
						if (iNumResource > 0)
						{
							iNumResourceInUnit = pUnit->getUnitInfo().GetResourceQuantityRequirement(eResource);
							if (m_pPlayer->getNumResourceAvailable(eResource) + iNumResourceInUnit < iNumResource)
							{
								bMissingResource = true;
							}
						}
					}

					if(!bMissingResource)
					{
						CvHomelandUnit unit;
						unit.SetID(pUnit->GetID());


						int iPriority = UPGRADE_THIS_TURN_PRIORITY_BOOST - GC.getUnitInfo(pUnit->getUnitType())->GetPower();


						if(pUnit->CanUpgradeRightNow(false))
						{
							iPriority += UPGRADE_THIS_TURN_PRIORITY_BOOST;
						}


						else if(pUnit->getOwner() == pUnit->plot()->getOwner())
						{
							iPriority += UPGRADE_IN_TERRITORY_PRIORITY_BOOST;
						}

						unit.SetAuxIntData(iPriority);
						m_CurrentMoveUnits.push_back(unit);
					}
				}
			}
		}
	}


	if(m_CurrentMoveUnits.size() == 0)
	{
		if(!m_pPlayer->GetEconomicAI()->IsSavingForThisPurchase(PURCHASE_TYPE_UNIT_UPGRADE))
		{
			m_pPlayer->GetEconomicAI()->CancelSaveForPurchase(PURCHASE_TYPE_UNIT_UPGRADE);
		}
	}

	else
	{

		std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end(), HomelandAIHelpers::CvHomelandUnitAuxIntReverseSort);

		int iFlavorMilitaryTraining = 0;
		for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes() && iFlavorMilitaryTraining == 0; iFlavorLoop++)
		{
			if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_MILITARY_TRAINING")
			{
				iFlavorMilitaryTraining = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)iFlavorLoop);
			}
		}

		iFlavorMilitaryTraining = max(1,iFlavorMilitaryTraining/3);
		int iBonusUpgrades = max(0,GC.getGame().getHandicapInfo().GetID() - 5);
		iFlavorMilitaryTraining += iBonusUpgrades;


		int iNumUpgraded = 0;
		for(moveUnitIt = m_CurrentMoveUnits.begin(); moveUnitIt != m_CurrentMoveUnits.end(); ++moveUnitIt)
		{
			UnitHandle pUnit = m_pPlayer->getUnit(moveUnitIt->GetID());
			if(pUnit->CanUpgradeRightNow(false))
			{
				CvUnit* pNewUnit = pUnit->DoUpgrade();
				UnitProcessed(pUnit->GetID());
				UnitProcessed(pNewUnit->GetID());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
#ifdef AUI_WARNING_FIXES
					CvString strTemp1 = pUnit->getUnitInfo().GetDescription();
					CvString strTemp2 = pNewUnit->getUnitInfo().GetDescription();
#else
					CvString strTemp1, strTemp2;
					strTemp1 = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
					strTemp2 = GC.getUnitInfo(pNewUnit->getUnitType())->GetDescription();
#endif
					strLogString.Format("Upgrading unit from type %s to type %s, X: %d, Y: %d", strTemp1.GetCString(), strTemp2.GetCString(), pNewUnit->getX(), pNewUnit->getY());
					LogHomelandMessage(strLogString);
				}

				iNumUpgraded++;
				if(iNumUpgraded >= iFlavorMilitaryTraining)
				{
					return;
				}
			}
		}

		if(iNumUpgraded > 0)
		{
			return;
		}


		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());

		int iAmountRequired = pUnit->upgradePrice(pUnit->GetUpgradeUnitType());
		bool bRequiresGold = (iAmountRequired > 0);
		int iGoldPriority = 0;

		if(bRequiresGold)
		{

			int iCurrentFlavorMilitaryTraining = 0;
			for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes() && iCurrentFlavorMilitaryTraining == 0; iFlavorLoop++)
			{
				if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_MILITARY_TRAINING")
				{
					iCurrentFlavorMilitaryTraining = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
				}
			}
			iGoldPriority = GC.getAI_GOLD_PRIORITY_UPGRADE_BASE();
			iGoldPriority += GC.getAI_GOLD_PRIORITY_UPGRADE_PER_FLAVOR_POINT() * iCurrentFlavorMilitaryTraining;
#ifndef AUI_WARNING_FIXES
		}


		if(bRequiresGold)
		{
#endif
			m_pPlayer->GetEconomicAI()->CancelSaveForPurchase(PURCHASE_TYPE_UNIT_UPGRADE);
			m_pPlayer->GetEconomicAI()->StartSaveForPurchase(PURCHASE_TYPE_UNIT_UPGRADE, iAmountRequired, iGoldPriority);
		}


		if(pUnit->plot()->getOwner() == pUnit->getOwner())
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
#ifdef AUI_WARNING_FIXES
				CvString strTemp = pUnit->getUnitInfo().GetDescription();
#else
				CvString strTemp;

				strTemp = pUnit->getUnitInfo().GetDescription();
#endif
				strLogString.Format("Need gold for %s upgrade, GOLD: Available = %d, Needed = %d, Priority = %d",
				                    strTemp.GetCString(), m_pPlayer->GetTreasury()->GetGold(), iAmountRequired, iGoldPriority);
				LogHomelandMessage(strLogString);
			}
		}

		else
		{

			CvCity* pUpgradeCity = NULL;
			int iBestDistance = MAX_INT;
			for(unsigned int iI = 0; iI < m_TargetedCities.size(); iI++)
			{
				CvPlot* pTarget = GC.getMap().plot(m_TargetedCities[iI].GetTargetX(), m_TargetedCities[iI].GetTargetY());
				CvCity* pCity = pTarget->getPlotCity();

				int iDistance = plotDistance(pCity->getX(), pCity->getY(), pUnit->getX(), pUnit->getY());

				if(iDistance < iBestDistance)
				{
					iBestDistance = iDistance;
					pUpgradeCity = pCity;
				}
			}

			if(pUpgradeCity)
			{
				if(MoveToEmptySpaceNearTarget(pUnit.pointer(), pUpgradeCity->plot()))
				{
					pUnit->finishMoves();
					UnitProcessed(pUnit->GetID());

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						CvString strTemp;
#ifdef AUI_WARNING_FIXES
						CvUnitEntry* pTempUnitInfo = GC.getUnitInfo(pUnit->getUnitType());
						if (pTempUnitInfo)
							strTemp = pTempUnitInfo->GetDescription();
#else
						strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
#endif
						strLogString.Format("Moving %s for upgrade at %s, GOLD: Available = %d, Needed = %d, Priority = %d, Dist = %d",
						                    strTemp.GetCString(), pUpgradeCity->getName().GetCString(),
						                    m_pPlayer->GetTreasury()->GetGold(), iAmountRequired, iGoldPriority, iBestDistance);
						LogHomelandMessage(strLogString);
					}
				}
			}
		}
	}

	return;
}


void CvHomelandAI::PlotAncientRuinMoves()
{

	if(!m_TargetedAncientRuins.empty())
	{



		for(unsigned int iI = 0; iI < m_TargetedAncientRuins.size(); iI++)
		{
			CvPlot* pTarget = GC.getMap().plot(m_TargetedAncientRuins[iI].GetTargetX(), m_TargetedAncientRuins[iI].GetTargetY());

			FindUnitsForThisMove(AI_HOMELAND_MOVE_ANCIENT_RUINS, (iI == 0)              );

			if(m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
			{
				if(GetBestUnitToReachTarget(pTarget, m_iDefensiveMoveTurns))
				{
					ExecuteMoveToTarget(pTarget);

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Moving to goody hut (non-explorer), X: %d, Y: %d", m_TargetedAncientRuins[iI].GetTargetX(), m_TargetedAncientRuins[iI].GetTargetY());
						LogHomelandMessage(strLogString);
					}
				}
			}
		}
	}
}


void CvHomelandAI::PlotWriterMoves()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if (pUnit->AI_getUnitAIType() == UNITAI_WRITER)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteWriterMoves();
	}
}


void CvHomelandAI::PlotArtistMoves()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if (pUnit->AI_getUnitAIType() == UNITAI_ARTIST)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteArtistMoves();
	}
}


void CvHomelandAI::PlotMusicianMoves()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if (pUnit->AI_getUnitAIType() == UNITAI_MUSICIAN)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteMusicianMoves();
	}
}


void CvHomelandAI::PlotScientistMoves()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_SCIENTIST)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteScientistMoves();
	}
}


void CvHomelandAI::PlotEngineerMoves()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_ENGINEER)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteEngineerMoves();
	}
}


void CvHomelandAI::PlotMerchantMoves()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_MERCHANT)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteMerchantMoves();
	}
}


void CvHomelandAI::PlotProphetMoves()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_PROPHET)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteProphetMoves();
	}
}


void CvHomelandAI::PlotMissionaryMoves()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_MISSIONARY)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteMissionaryMoves();
	}
}


void CvHomelandAI::PlotInquisitorMoves()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_INQUISITOR)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteInquisitorMoves();
	}
}


void CvHomelandAI::PlotSSPartMoves()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_SPACESHIP_PART)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteSSPartMoves();
	}
}


void CvHomelandAI::PlotSSPartAdds()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_SPACESHIP_PART)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteSSPartAdds();
	}
}


void CvHomelandAI::PlotTreasureMoves()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_TREASURE)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteTreasureMoves();
	}
}


void CvHomelandAI::PlotGeneralMoves()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_GENERAL)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteGeneralMoves();
	}
}


void CvHomelandAI::PlotAdmiralMoves()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_ADMIRAL)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteAdmiralMoves();
	}
}


void CvHomelandAI::PlotAircraftMoves()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->getDomainType() == DOMAIN_AIR && pUnit->getDamage() < 50)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteAircraftMoves();
	}
}


void CvHomelandAI::PlotTradeUnitMoves()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_TRADE_UNIT)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteTradeUnitMoves();
	}
}


void CvHomelandAI::PlotArchaeologistMoves()
{
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_ARCHAEOLOGIST)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteArchaeologistMoves();
	}
}


void CvHomelandAI::PlotAirliftMoves()
{
	if (m_pPlayer->isHuman())
	{
		return;
	}


	vector<CvCity *> aAirliftCities;
	CvCity *pLoopCity;
	int iCityLoop;
	for (pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
	{
		if (pLoopCity->CanAirlift())
		{
			aAirliftCities.push_back(pLoopCity);
		}
	}
	if (aAirliftCities.size() < 2)
	{
		return;
	}


	vector<CvPlot *> aAirliftPlots;
	CvTacticalAnalysisMap* pMap = GC.getGame().GetTacticalAnalysisMap();
	CvTacticalDominanceZone *pZone;
	vector<CvCity *>::const_iterator it;
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = aAirliftCities.begin(); it != aAirliftCities.end(); ++it)
#else
	for (it = aAirliftCities.begin(); it != aAirliftCities.end(); it++)
#endif
	{
		pZone = pMap->GetZoneByCity(*it, false);
		if (pZone && (pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY || pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_NO_UNITS_VISIBLE))
		{
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				CvPlot *pLoopPlot = plotDirection((*it)->getX(), (*it)->getY(), ((DirectionTypes)iI));
				if (pLoopPlot != NULL && !pLoopPlot->isWater() && !pLoopPlot->isMountain() && pLoopPlot->getOwner() == m_pPlayer->GetID())
				{
					UnitHandle pBestDefender = pLoopPlot->getBestDefender(m_pPlayer->GetID());
					if (pBestDefender)
					{
						if (std::find(m_CurrentTurnUnits.begin(), m_CurrentTurnUnits.end(), pBestDefender->GetID()) != m_CurrentTurnUnits.end())
						{
							pBestDefender->finishMoves();
							UnitProcessed(pBestDefender->GetID());
							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("Remaining in place for future airlift - %s, X: %d, Y: %d", pBestDefender->getName().GetCString(), pBestDefender->getX(), pBestDefender->getY());
								LogHomelandMessage(strLogString);
							}
						}
					}
					else
					{
						aAirliftPlots.push_back(pLoopPlot);
					}
				}
			}
		}
	}

	vector<CvPlot *>::const_iterator plotIt;
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (plotIt = aAirliftPlots.begin(); plotIt != aAirliftPlots.end(); ++plotIt)
#else
	for (plotIt = aAirliftPlots.begin(); plotIt != aAirliftPlots.end(); plotIt++)
#endif
	{
		FindUnitsForThisMove(AI_HOMELAND_MOVE_MOBILE_RESERVE, true);

		if(m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
		{
			CvPlot *pTarget = *plotIt;

			if (GetBestUnitToReachTarget(pTarget, MAX_INT))
			{
				ExecuteMoveToTarget(pTarget);

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Moving to airlift origin plot, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
	}
}


void CvHomelandAI::ReviewUnassignedUnits()
{

	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			pUnit->PushMission(CvTypes::getMISSION_SKIP());
			pUnit->SetTurnProcessed(true);

			CvString strTemp;
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pUnit->getUnitType());
			if(pkUnitInfo)
			{
				strTemp = pkUnitInfo->GetDescription();
				CvString strLogString;
				strLogString.Format("Unassigned %s at, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}
		}
	}
}


void CvHomelandAI::ExecuteFirstTurnSettlerMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(pUnit)
		{
			pUnit->PushMission(CvTypes::getMISSION_FOUND());
			UnitProcessed(pUnit->GetID());
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Founded city at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}
		}
	}
}

#define PATH_PLAN_LAST
typedef CvWeightedVector<CvPlot*, 100, true> WeightedPlotVector;


void CvHomelandAI::ExecuteExplorerMoves()
{
	bool bFoundNearbyExplorePlot = false;
	CvEconomicAI* pEconomicAI = m_pPlayer->GetEconomicAI();

	pEconomicAI->UpdatePlots();

#if defined(PATH_PLAN_LAST)
	WeightedPlotVector aBestPlotList;
	aBestPlotList.reserve(100);
#endif

	CvTwoLayerPathFinder& kPathFinder = GC.getPathFinder();
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit || pUnit->TurnProcessed())
		{
			continue;
		}

		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if(pkScriptSystem)
		{
			CvLuaArgsHandle args;
			args->Push(pUnit->getOwner());
			args->Push(pUnit->GetID());

			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "UnitGetSpecialExploreTarget", args.get(), bResult);

			if(bResult)
			{
				continue;
			}
		}

		if (!m_pPlayer->isHuman() && pUnit->CanStartMission(CvTypes::getMISSION_SELL_EXOTIC_GOODS(), -1, -1))
		{

			float fRewardFactor = pUnit->calculateExoticGoodsDistanceFactor(pUnit->plot());
			if (fRewardFactor >= 0.5f)
			{
				pUnit->PushMission(CvTypes::getMISSION_SELL_EXOTIC_GOODS());
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("UnitID: %d used Sell Exotic Goods, X: %d, Y: %d", pUnit->GetID(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}

		int iUnitX = pUnit->getX();
		int iUnitY = pUnit->getY();

		CvPlot* pkStepPlot = NULL;
		CvPlot* pGoodyPlot = pEconomicAI->GetUnitTargetGoodyPlot(pUnit.pointer(), &pkStepPlot);
		if (pGoodyPlot)
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("UnitID: %d has goody target, X: %d, Y: %d", pUnit->GetID(), pGoodyPlot->getX(), pGoodyPlot->getY());
				LogHomelandMessage(strLogString);
			}
		}

#ifdef AUI_WARNING_FIXES
		if (pGoodyPlot && (pGoodyPlot->isGoody(m_pPlayer->getTeam()) || (pGoodyPlot->HasBarbarianCamp() && !pGoodyPlot->isVisibleEnemyDefender(pUnit.pointer()))))
#else
		if(pGoodyPlot && (pGoodyPlot->isGoody(m_pPlayer->getTeam()) || (pGoodyPlot->HasBarbarianCamp()) && !pGoodyPlot->isVisibleEnemyDefender(pUnit.pointer())))
#endif
		{
			bool bCanFindPath = false;
			if (pkStepPlot)
			{
				if (IsValidExplorerEndTurnPlot(pUnit.pointer(), pkStepPlot))
					bCanFindPath = true;


				pEconomicAI->ClearUnitTargetGoodyStepPlot(pUnit.pointer());
			}

			if (!pkStepPlot || !bCanFindPath)
			{
				bCanFindPath = kPathFinder.GenerateUnitPath(pUnit.pointer(), iUnitX, iUnitY, pGoodyPlot->getX(), pGoodyPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER           , true          );
				if(bCanFindPath)
				{
					pkStepPlot = kPathFinder.GetPathEndTurnPlot();
				}
			}

			if (bCanFindPath)
			{
				if(pkStepPlot)
				{
					CvAssert(!pUnit->atPlot(*pkStepPlot));
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("UnitID: %d Moving to goody hut, X: %d, Y: %d, from X: %d Y: %d", pUnit->GetID(), pkStepPlot->getX(), pkStepPlot->getY(), pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);
					}
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pkStepPlot->getX(), pkStepPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER, false, false, MISSIONAI_EXPLORE, pkStepPlot);
#ifndef AUI_HOMELAND_FIX_EXECUTE_EXPLORER_MOVES_MOVE_AFTER_GOODY
					pUnit->finishMoves();
					UnitProcessed(pUnit->GetID());
#endif
				}
				else
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("UnitID: %d No end turn plot to goody from, X: %d, Y: %d", pUnit->GetID(), pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);
					}
				}

				continue;

			}
			else
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("UnitID: %d Can't find path to goody from, X: %d, Y: %d", pUnit->GetID(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}

		CvPlot* pBestPlot = NULL;
		int iBestPlotScore = 0;

#if defined(PATH_PLAN_LAST)
		aBestPlotList.clear();
#endif
		TeamTypes eTeam = pUnit->getTeam();
		int iBaseSightRange = pUnit->getUnitInfo().GetBaseSightRange();

		int iMovementRange = pUnit->movesLeft() / GC.getMOVE_DENOMINATOR();



		if (pUnit->movesLeft() > 0 && iMovementRange == 0)
		{
			iMovementRange = 1;
		}

#ifdef AUI_HEXSPACE_DX_LOOPS
		int iMaxDX, iDX;
		CvPlot* pEvalPlot;
		for (int iDY = -iMovementRange; iDY <= iMovementRange; iDY++)
		{
			iMaxDX = iMovementRange - MAX(0, iDY);
			for (iDX = -iMovementRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
			{

				pEvalPlot = plotXY(iUnitX, iUnitY, iDX, iDY);
#else
		for(int iX = -iMovementRange; iX <= iMovementRange; iX++)
		{
			for(int iY = -iMovementRange; iY <= iMovementRange; iY++)
			{
				CvPlot* pEvalPlot = plotXYWithRangeCheck(iUnitX, iUnitY, iX, iY, iMovementRange);
#endif
				if(!pEvalPlot)
				{
					continue;
				}

				if(!IsValidExplorerEndTurnPlot(pUnit.pointer(), pEvalPlot))
				{
					continue;
				}

#if !defined(PATH_PLAN_LAST)
				bool bCanFindPath = kPathFinder.GenerateUnitPath(pUnit.pointer(), pUnit->getX(), pUnit->getY(), pEvalPlot->getX(), pEvalPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER           , true          );
				if(!bCanFindPath)
				{
					continue;
				}

				CvAStarNode* pNode = kPathFinder.GetLastNode();
				int iDistance = pNode->m_iData2;
				if(iDistance > 1)
				{
					continue;
				}
#endif

				DomainTypes eDomain = pUnit->getDomainType();
				int iScore = CvEconomicAI::ScoreExplorePlot(pEvalPlot, eTeam, iBaseSightRange, eDomain);
				if(iScore > 0)
				{
					if (eDomain == DOMAIN_LAND)
					{
						if (pEvalPlot->isHills())
						{
							iScore += 50;
						}
						if (pUnit->IsEmbarkAllWater() && !pEvalPlot->isShallowWater())
						{
							iScore += 200;
						}
					}
					else if (eDomain == DOMAIN_SEA)
					{
						if(pUnit->canSellExoticGoods(pEvalPlot))
						{
							float fRewardFactor = pUnit->calculateExoticGoodsDistanceFactor(pEvalPlot);
							if (fRewardFactor >= 0.75f)
							{
								iScore += 150;
							}
							else if (fRewardFactor >= 0.5f)
							{
								iScore += 75;
							}
						}

						if(pEvalPlot->isAdjacentToLand())
						{
							iScore += 200;
						}
					}

#if defined(PATH_PLAN_LAST)
					aBestPlotList.push_back(pEvalPlot, iScore);
#endif
				}

#if !defined(PATH_PLAN_LAST)
				if(iScore > iBestPlotScore)
				{
					pBestPlot = pEvalPlot;
					iBestPlotScore = iScore;
					bFoundNearbyExplorePlot = true;
				}
#endif
			}
		}

#if defined(PATH_PLAN_LAST)
		uint uiListSize;
		if ((uiListSize = aBestPlotList.size()) > 0)
		{
			aBestPlotList.SortItems();
			for (uint i = 0; i < uiListSize; ++i )	
			{
				CvPlot* pPlot = aBestPlotList.GetElement(i);
				bool bCanFindPath = kPathFinder.GenerateUnitPath(pUnit.pointer(), iUnitX, iUnitY, pPlot->getX(), pPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER           , true          );
				if(!bCanFindPath)
				{
					continue;
				}

				CvAStarNode* pNode = GC.getPathFinder().GetLastNode();
				int iDistance = pNode->m_iData2;
				if(iDistance <= 1)
				{
					pBestPlot = pPlot;
					break;
				}
			}
		}
#endif

		if(!pBestPlot && iMovementRange > 0)
		{
			FFastVector<int>& aiExplorationPlots = pEconomicAI->GetExplorationPlots();
			if (aiExplorationPlots.size() > 0)
			{
				FFastVector<int>& aiExplorationPlotRatings = pEconomicAI->GetExplorationPlotRatings();

				aBestPlotList.clear();
				aBestPlotList.reserve(aiExplorationPlots.size());

				iBestPlotScore = 0;

				for(uint ui = 0; ui < aiExplorationPlots.size(); ui++)
				{
					int iPlot = aiExplorationPlots[ui];
					if(iPlot < 0)
					{
						continue;
					}

#ifdef AUI_HEXSPACE_DX_LOOPS
					pEvalPlot = GC.getMap().plotByIndex(iPlot);
#else
					CvPlot* pEvalPlot = GC.getMap().plotByIndex(iPlot);
#endif
					if(!pEvalPlot)
					{
						continue;
					}

					int iPlotScore = 0;

					if(!IsValidExplorerEndTurnPlot(pUnit.pointer(), pEvalPlot))
					{
						continue;
					}

					int iRating = aiExplorationPlotRatings[ui];

#if defined(PATH_PLAN_LAST)
					int iDistance = plotDistance(iUnitX, iUnitY, pEvalPlot->getX(), pEvalPlot->getY());
					int iEstimateTurns = iDistance / iMovementRange;
					if(iEstimateTurns == 0)
					{
						iPlotScore = 1000 * iRating;
					}
					else
					{
						iPlotScore = (1000 * iRating) / iEstimateTurns;
					}

					aBestPlotList.push_back(pEvalPlot, iPlotScore);
#else

					bool bCanFindPath = GC.getPathFinder().GenerateUnitPath(pUnit.pointer(), iUnitX, iUnitY, pEvalPlot->getX(), pEvalPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER           , true          );
					if(!bCanFindPath)
					{
						continue;
					}

					CvAStarNode* pNode = GC.getPathFinder().GetLastNode();
					int iDistance = pNode->m_iData2;
					if(iDistance == 0)
					{
						iPlotScore = 1000 * iRating;
					}
					else
					{
						iPlotScore = (1000 * iRating) / iDistance;
					}

					if(iPlotScore > iBestPlotScore)
					{
						CvPlot* pEndTurnPlot = GC.getPathFinder().GetPathEndTurnPlot();
						if(pEndTurnPlot == pUnit->plot())
						{
							pBestPlot = NULL;
							iBestPlotScore = iPlotScore;
						}
						else if(IsValidExplorerEndTurnPlot(pUnit.pointer(), pEndTurnPlot))
						{
							pBestPlot = pEndTurnPlot;
							iBestPlotScore = iPlotScore;
						}
						else
						{

							continue;
						}
					}
#endif
				}

#if defined(PATH_PLAN_LAST)
				if ((uiListSize = aBestPlotList.size()) > 0)
				{
					aBestPlotList.SortItems();
					for (uint i = 0; i < uiListSize ; ++i )	
					{
						CvPlot* pPlot = aBestPlotList.GetElement(i);
						bool bCanFindPath = GC.getPathFinder().GenerateUnitPath(pUnit.pointer(), iUnitX, iUnitY, pPlot->getX(), pPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER           , true          );
						if(!bCanFindPath)
						{
							continue;
						}

						CvPlot* pEndTurnPlot = GC.getPathFinder().GetPathEndTurnPlot();
						if(pEndTurnPlot == pUnit->plot())
						{
							continue;
						}
						else if(IsValidExplorerEndTurnPlot(pUnit.pointer(), pEndTurnPlot))
						{
							pBestPlot = pEndTurnPlot;
							break;
						}
						else
						{

							continue;
						}
					}
				}
#endif
			}
		}

		if(pBestPlot)
		{
			CvAssertMsg(!pUnit->atPlot(*pBestPlot), "Exploring unit is already at the best place to explore");
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER, false, false, MISSIONAI_EXPLORE, pBestPlot);


			if(pUnit->getMoves() <= 0)
			{
				UnitProcessed(pUnit->GetID());
			}

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				if(bFoundNearbyExplorePlot)
				{
					strLogString.Format("UnitID: %d Explored to nearby target, To X: %d, Y: %d, From X: %d, Y: %d", pUnit->GetID(), pUnit->getX(), pUnit->getY(), iUnitX, iUnitY);
				}
				else
				{
					strLogString.Format("UnitID: %d Explored to distant target, To X: %d, Y: %d, From X: %d, Y: %d", pUnit->GetID(), pUnit->getX(), pUnit->getY(), iUnitX, iUnitY);
				}
				LogHomelandMessage(strLogString);
			}
		}
		else
		{
			if(pUnit->isHuman())
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("UnitID: %d Explorer (human) found no target, X: %d, Y: %d", pUnit->GetID(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
				pUnit->SetAutomateType(NO_AUTOMATE);
				UnitProcessed(pUnit->GetID());
			}
			else
			{

				if(pUnit->AI_getUnitAIType() == UNITAI_EXPLORE)
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("UnitID: %d Explorer (AI) found no target, X: %d, Y: %d", pUnit->GetID(), pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);
					}

					CvCity* pLoopCity;
					int iLoop;
					bool bFoundPath = false;
					for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
					{
#ifdef AUI_ASTAR_MINOR_OPTIMIZATION
						if (GC.getIgnoreUnitsPathFinder().DoesPathExist(pUnit.pointer(), pUnit->plot(), pLoopCity->plot()))
#else
						if(GC.getIgnoreUnitsPathFinder().DoesPathExist(*(pUnit), pUnit->plot(), pLoopCity->plot()))
#endif
						{
							bFoundPath = true;
							break;
						}
					}
					if(!bFoundPath)
					{
						CvString strLogString;
						strLogString.Format("UnitID: %d Disbanding explorer, X: %d, Y: %d", pUnit->GetID(), pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);

						UnitProcessed(pUnit->GetID());
#ifdef AUI_HOMELAND_FIX_EXECUTE_EXPLORER_MOVES_DISBAND
						pUnit->scrap();
#else
						pUnit->kill(true);
#endif
						m_pPlayer->GetEconomicAI()->IncrementExplorersDisbanded();
					}
				}
				else if(pUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA)
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("UnitID: %d Sea explorer (AI) found no target, X: %d, Y: %d", pUnit->GetID(), pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);
					}
				}
			}
		}
	}
}


void CvHomelandAI::ExecuteWorkerMoves()
{
	CvString strLogString;

	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(pUnit)
		{
			CvPlot* pPlot = pUnit->plot();

			AI_PERF_FORMAT("Homeland-ExecuteWorkerMoves-perf.csv", ("ExecuteWorkerMoves, Turn %03d, %s, Unit %d, at x=%d, y=%d", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription(), pUnit->GetID(), pUnit->getX(), pUnit->getY()) );

			if(pPlot && m_pPlayer->IsPlotUnderImmediateThreat(*pPlot) && !pPlot->getBestDefender(m_pPlayer->GetID()))
			{
				if(MoveCivilianToSafety(pUnit.pointer()))
				{
					if(GC.getLogging() && GC.GetBuilderAILogging())
					{

						CvString strFileName = "BuilderTaskingLog.csv";
						FILogFile* pLog;
						pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);


						CvString strLog;
						CvString strTemp;

						CvString strPlayerName;
						strPlayerName = m_pPlayer->getCivilizationShortDescription();
						strLog += strPlayerName;
						strLog += ",";

						strTemp.Format("%d,", GC.getGame().getGameTurn());
						strLog += strTemp;

						strTemp.Format("%d,", pUnit->GetID());
						strLog += strTemp;

						strLog += "1st Safety,";

						pLog->Msg(strLog);
					}

					pUnit->finishMoves();
					UnitProcessed(pUnit->GetID());
					continue;
				}
			}

			bool bActionPerformed = ExecuteWorkerMove(pUnit.pointer());
			if(bActionPerformed)
			{
				continue;
			}


			if(MoveCivilianToSafety(pUnit.pointer(), true                 ))
			{
				if(GC.getLogging() && GC.GetBuilderAILogging())
				{

					CvString strFileName = "BuilderTaskingLog.csv";
					FILogFile* pLog;
					pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);


					CvString strLog;
					CvString strTemp;

					CvString strPlayerName;
					strPlayerName = m_pPlayer->getCivilizationShortDescription();
					strLog += strPlayerName;
					strLog += ",";

					strTemp.Format("%d,", GC.getGame().getGameTurn());
					strLog += strTemp;

					strTemp.Format("%d,", pUnit->GetID());
					strLog += strTemp;

					strLog += "2nd Safety,";

					pLog->Msg(strLog);
				}


				pUnit->PushMission(CvTypes::getMISSION_SKIP());
				if (!m_pPlayer->isHuman())
				{
					pUnit->finishMoves();
				}
				UnitProcessed(pUnit->GetID());
				continue;
			}

			pUnit->PushMission(CvTypes::getMISSION_SKIP());
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());
		}
	}
}


bool CvHomelandAI::ExecuteWorkerSeaMoves(CvHomelandTarget target, CvPlot* pTarget)
{
	bool bRtnValue = false;


	UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits.begin()->GetID());
	if(pUnit)
	{
		if(pUnit->UnitPathTo(pTarget->getX(), pTarget->getY(), 0) > 0)
		{
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
			if(pUnit->plot() == pTarget)
			{
				pUnit->PushMission(CvTypes::getMISSION_BUILD(), (int)target.GetAuxIntData(), -1, 0, (pUnit->GetLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pTarget);
				bRtnValue = true;
			}
			else
			{
				pUnit->finishMoves();
			}


			UnitProcessed(m_CurrentMoveUnits.begin()->GetID());
		}
	}
	return bRtnValue;
}


void CvHomelandAI::ExecuteHeals()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(pUnit)
		{
			if(pUnit->isFortifyable())
			{
				pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
				pUnit->SetFortifiedThisTurn(true);
			}
			else
			{
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
			}
			UnitProcessed(it->GetID());
		}
	}
}


void CvHomelandAI::ExecuteMovesToSafestPlot()
{
	int iDanger;

	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		WeightedPlotVector aBestPlotList;
		aBestPlotList.reserve(100);

		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			CvPlot* pBestPlot = NULL;

#ifdef AUI_HOMELAND_FIX_EXECUTE_MOVES_TO_SAFEST_PLOT_USE_GAME_MOVEMENT_RANGE
			int iRange = pUnit->baseMoves();
#else
			int iRange = pUnit->getUnitInfo().GetMoves();
#endif
#ifdef AUI_ASTAR_TWEAKED_OPTIMIZED_BUT_CAN_STILL_USE_ROADS
			IncreaseMoveRangeForRoads(pUnit.pointer(), iRange);
#endif


#ifdef AUI_HEXSPACE_DX_LOOPS
			int iMaxDX, iX;
			CvPlot* pPlot;
			for (int iY = -iRange; iY <= iRange; iY++)
			{
				iMaxDX = iRange - MAX(0, iY);
				for (iX = -iRange - MIN(0, iY); iX <= iMaxDX; iX++)
				{

					pPlot = plotXY(pUnit->getX(), pUnit->getY(), iX, iY);
#else
			for(int iX = -iRange; iX <= iRange; iX++)
			{
				for(int iY = -iRange; iY <= iRange; iY++)
				{
					CvPlot* pPlot = GC.getMap().plot(pUnit->getX() + iX, pUnit->getY() + iY);
#endif
					if(pPlot == NULL)
					{
						continue;
					}

#ifndef AUI_HEXSPACE_DX_LOOPS
					if (plotDistance(pPlot->getX(), pPlot->getY(), pUnit->getX(), pUnit->getY()) > iRange)
					{
						continue;
					}
#endif







					iDanger = m_pPlayer->GetPlotDanger(*pPlot);
					bool bIsZeroDanger = (iDanger <= 0);
					bool bIsInCity = pPlot->isFriendlyCity(*pUnit, false);
					bool bIsInCover = (pPlot->getNumDefenders(m_pPlayer->GetID()) > 0) && !pUnit->IsCanDefend();
					bool bIsInTerritory = (pPlot->getTeam() == m_pPlayer->getTeam());
#ifdef AUI_HOMELAND_FIX_EXECUTE_MOVES_TO_SAFEST_PLOT_NO_EMBARK_SUICIDE
					bool bNeedEmbark = ((pUnit->getDomainType() == DOMAIN_LAND) && (!pUnit->plot()->isWater()) && (pPlot->isWater()));
#endif

					#define MAX_DANGER_VALUE	100000
					#define PREFERENCE_LEVEL(x, y) (x * MAX_DANGER_VALUE) + ((MAX_DANGER_VALUE - 1) - y)

					CvAssert(iDanger < MAX_DANGER_VALUE);

					int iScore;
					if(bIsInCity)
					{
						iScore = PREFERENCE_LEVEL(5, iDanger);
					}
					else if(bIsZeroDanger)
					{
						if (bIsInTerritory)
							iScore = PREFERENCE_LEVEL(4, iDanger);
						else
							iScore = PREFERENCE_LEVEL(3, iDanger);
					}
					else if(bIsInCover)
					{
						iScore = PREFERENCE_LEVEL(2, iDanger);
					}
					else if(bIsInTerritory)
					{
						iScore = PREFERENCE_LEVEL(1, iDanger);
					}

					else 
					{
						iScore = PREFERENCE_LEVEL(0, iDanger);
					}

#ifdef AUI_HOMELAND_FIX_EXECUTE_MOVES_TO_SAFEST_PLOT_NO_EMBARK_SUICIDE

					if (bNeedEmbark && !bIsInCover)
					{
						iScore = PREFERENCE_LEVEL(0, iDanger);
					}
#endif

					aBestPlotList.push_back(pPlot, iScore);
				}
			}

			aBestPlotList.SortItems();



			#ifdef EXECUTEMOVESTOSAFESTPLOT_FAILURE_LIMIT
			int iFailureLimit = 10;
			#endif
			uint uiListSize;
			if ((uiListSize = aBestPlotList.size()) > 0)
			{
				aBestPlotList.SortItems();
				for (uint i = 0; i < uiListSize; ++i )	
				{
#ifdef AUI_HEXSPACE_DX_LOOPS
					pPlot = aBestPlotList.GetElement(i);
#else
					CvPlot* pPlot = aBestPlotList.GetElement(i);
#endif

					if(CanReachInXTurns(pUnit, pPlot, 1))
					{
						pBestPlot = pPlot;
						break;
					}
					#ifdef EXECUTEMOVESTOSAFESTPLOT_FAILURE_LIMIT
					else
					{
						if (iFailureLimit-- == 0)
							break;
					}
					#endif
				}
			}

			if(pBestPlot != NULL)
			{

				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY(), MOVE_UNITS_IGNORE_DANGER);
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp;
#ifdef AUI_WARNING_FIXES
					CvUnitEntry* pTempUnitInfo = GC.getUnitInfo(pUnit->getUnitType());
					if (pTempUnitInfo)
						strTemp = pTempUnitInfo->GetDescription();
#else
					strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
#endif
					strLogString.Format("Moving %s to safety, X: %d, Y: %d", strTemp.GetCString(), pBestPlot->getX(), pBestPlot->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
	}
}


void CvHomelandAI::ExecutePatrolMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		CvPlot* pTarget = it->GetTarget();
		if(pUnit && pTarget)
		{
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());
		}
	}
}



void CvHomelandAI::ExecuteMoveToTarget(CvPlot* pTarget)
{
	AI_PERF_FORMAT("Homeland-ExecuteMove-perf.csv", ("ExecuteMoveToTarget, %d, %d, Turn %03d, %s", pTarget->getX(), pTarget->getY(), GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );


	CvUnit *pBestUnit = NULL;
	if (m_CurrentBestMoveHighPriorityUnit)
	{

		if (m_CurrentBestMoveUnit && m_iCurrentBestMoveUnitTurns < m_iCurrentBestMoveHighPriorityUnitTurns)
			pBestUnit = m_CurrentBestMoveUnit;
		else
			pBestUnit = m_CurrentBestMoveHighPriorityUnit;
	}
	else
		pBestUnit = m_CurrentBestMoveUnit;

	if (pBestUnit)
	{
		if(pBestUnit->plot() == pTarget && pBestUnit->canFortify(pBestUnit->plot()))
		{
			pBestUnit->PushMission(CvTypes::getMISSION_FORTIFY());
			pBestUnit->SetFortifiedThisTurn(true);
			UnitProcessed(pBestUnit->GetID());
			return;
		}
		else
		{

			pBestUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
			pBestUnit->finishMoves();
			UnitProcessed(pBestUnit->GetID());
			return;
		}
	}

	MoveUnitsArray::iterator it;


	for(it = m_CurrentMoveHighPriorityUnits.begin(); it != m_CurrentMoveHighPriorityUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());


		if(m_CurrentMoveUnits.size() > 0 && m_CurrentMoveUnits.begin()->GetMovesToTarget() < it->GetMovesToTarget())
		{
			break;
		}

		if(pUnit)
		{
			if(pUnit->plot() == pTarget && pUnit->canFortify(pUnit->plot()))
			{
				pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
				pUnit->SetFortifiedThisTurn(true);
				UnitProcessed(it->GetID());
				return;
			}

			else if(it->GetMovesToTarget() < GC.getAI_HOMELAND_ESTIMATE_TURNS_DISTANCE() || TurnsToReachTarget(pUnit, pTarget) != MAX_INT)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
				pUnit->finishMoves();
				UnitProcessed(it->GetID());
				return;
			}
		}
	}


	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(pUnit)
		{
			if(pUnit->plot() == pTarget && pUnit->canFortify(pUnit->plot()))
			{
				pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
				pUnit->SetFortifiedThisTurn(true);
				UnitProcessed(it->GetID());
				return;
			}

			else if(it->GetMovesToTarget() < GC.getAI_HOMELAND_ESTIMATE_TURNS_DISTANCE() || TurnsToReachTarget(pUnit, pTarget) != MAX_INT)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
				pUnit->finishMoves();
				UnitProcessed(it->GetID());
				return;
			}
		}
	}
}


void CvHomelandAI::ExecuteWriterMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch(eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_CULTURE_BLAST:
			ExecuteCultureBlast(pUnit.pointer());
			break;

		case GREAT_PEOPLE_DIRECTIVE_USE_POWER:
			{

				GreatWorkType eGreatWorkType = pUnit->GetGreatWork();
				CvCity* pTargetCity = m_pPlayer->GetEconomicAI()->GetBestGreatWorkCity(pUnit->plot(), eGreatWorkType);


				if (pTargetCity == NULL)
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Great writer can't find a Great Work slot. Running to safety");
						LogHomelandMessage(strLogString);
					}

					MoveCivilianToSafety(pUnit.pointer());
				}
				else
				{

					if (plotDistance(pUnit->getX(), pUnit->getY(), pTargetCity->getX(), pTargetCity->getY()) <= 1)
					{
						pUnit->PushMission(CvTypes::getMISSION_GREAT_WORK());
						UnitProcessed(pUnit->GetID());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Creating Great Work at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
							continue;
						}
					}


					else
					{

						CvPlot *pBestTarget = NULL;
						int iBestTurns = MAX_INT;
						int iTurns;
						iTurns = TurnsToReachTarget(pUnit, pTargetCity->plot());
						if (iTurns < iBestTurns)
						{
							pBestTarget = pTargetCity->plot();
						}
						for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
						{
							CvPlot* pAdjacentPlot = plotDirection(pTargetCity->getX(), pTargetCity->getY(), ((DirectionTypes)iI));
							if(pAdjacentPlot != NULL)
							{
								iTurns = TurnsToReachTarget(pUnit, pAdjacentPlot);
								if (iTurns < iBestTurns)
								{
									pBestTarget = pAdjacentPlot;
									iBestTurns = iTurns;
								}
							}
						}

						if (pBestTarget)
						{

							if (iBestTurns == 0)
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestTarget->getX(), pBestTarget->getY());
								pUnit->PushMission(CvTypes::getMISSION_GREAT_WORK());
								UnitProcessed(pUnit->GetID());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving and creating Great Work at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
									LogHomelandMessage(strLogString);
									continue;
								}
							}


							else
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestTarget->getX(), pBestTarget->getY());
								pUnit->finishMoves();
								UnitProcessed(it->GetID());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving Great Writer toward Great Work city at, X: %d, Y: %d", pBestTarget->getX(),  pBestTarget->getY());
									LogHomelandMessage(strLogString);
								}
							}
						}
						else
						{
							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("Could not find a target for Great Writer at, X: %d, Y: %d", pUnit->getX(),  pUnit->getY());
								LogHomelandMessage(strLogString);
							}
						}
					}
				}
			}	
			break;

		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit.pointer());
			break;
		}
	}
}


void CvHomelandAI::ExecuteArtistMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch(eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE:
			ExecuteGoldenAgeMove(pUnit.pointer());
			break;

		case GREAT_PEOPLE_DIRECTIVE_USE_POWER:
			{

				GreatWorkType eGreatWorkType = pUnit->GetGreatWork();
				CvCity* pTargetCity = m_pPlayer->GetEconomicAI()->GetBestGreatWorkCity(pUnit->plot(), eGreatWorkType);


				if (pTargetCity == NULL)
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Great artist can't find a Great Work slot. Running to safety");
						LogHomelandMessage(strLogString);
					}

					MoveCivilianToSafety(pUnit.pointer());
				}
				else
				{

					if (plotDistance(pUnit->getX(), pUnit->getY(), pTargetCity->getX(), pTargetCity->getY()) <= 1)
					{
						pUnit->PushMission(CvTypes::getMISSION_GREAT_WORK());
						UnitProcessed(pUnit->GetID());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Creating Great Work at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
							continue;
						}
					}


					else
					{

						CvPlot *pBestTarget = NULL;
						int iBestTurns = MAX_INT;
						int iTurns;
						iTurns = TurnsToReachTarget(pUnit, pTargetCity->plot());
						if (iTurns < iBestTurns)
						{
							pBestTarget = pTargetCity->plot();
						}
						for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
						{
							CvPlot* pAdjacentPlot = plotDirection(pTargetCity->getX(), pTargetCity->getY(), ((DirectionTypes)iI));
							if(pAdjacentPlot != NULL)
							{
								iTurns = TurnsToReachTarget(pUnit, pAdjacentPlot);
								if (iTurns < iBestTurns)
								{
									pBestTarget = pAdjacentPlot;
									iBestTurns = iTurns;
								}
							}
						}

						if (pBestTarget)
						{

							if (iBestTurns == 0)
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestTarget->getX(), pBestTarget->getY());
								pUnit->PushMission(CvTypes::getMISSION_GREAT_WORK());
								UnitProcessed(pUnit->GetID());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving and creating Great Work at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
									LogHomelandMessage(strLogString);
									continue;
								}
							}


							else
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestTarget->getX(), pBestTarget->getY());
								pUnit->finishMoves();
								UnitProcessed(it->GetID());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving Great Artist toward Great Work city at, X: %d, Y: %d", pBestTarget->getX(),  pBestTarget->getY());
									LogHomelandMessage(strLogString);
								}
							}
						}
						else
						{
							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("Could not find a target for Great Artist at, X: %d, Y: %d", pUnit->getX(),  pUnit->getY());
								LogHomelandMessage(strLogString);
							}
						}
					}
				}
			}	
			break;

		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit.pointer());
			break;
		}
	}
}


void CvHomelandAI::ExecuteMusicianMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch(eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_TOURISM_BLAST:
			break;

		case GREAT_PEOPLE_DIRECTIVE_USE_POWER:
			{

				GreatWorkType eGreatWorkType = pUnit->GetGreatWork();
				CvCity* pTargetCity = m_pPlayer->GetEconomicAI()->GetBestGreatWorkCity(pUnit->plot(), eGreatWorkType);


				if (pTargetCity == NULL)
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Great musician can't find a Great Work slot. Running to safety");
						LogHomelandMessage(strLogString);
					}

					MoveCivilianToSafety(pUnit.pointer());
				}
				else
				{

					if (plotDistance(pUnit->getX(), pUnit->getY(), pTargetCity->getX(), pTargetCity->getY()) <= 1)
					{
						pUnit->PushMission(CvTypes::getMISSION_GREAT_WORK());
						UnitProcessed(pUnit->GetID());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Creating Great Work at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
							continue;
						}
					}


					else
					{

						CvPlot *pBestTarget = NULL;
						int iBestTurns = MAX_INT;
						int iTurns;
						iTurns = TurnsToReachTarget(pUnit, pTargetCity->plot());
						if (iTurns < iBestTurns)
						{
							pBestTarget = pTargetCity->plot();
						}
						for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
						{
							CvPlot* pAdjacentPlot = plotDirection(pTargetCity->getX(), pTargetCity->getY(), ((DirectionTypes)iI));
							if(pAdjacentPlot != NULL)
							{
								iTurns = TurnsToReachTarget(pUnit, pAdjacentPlot);
								if (iTurns < iBestTurns)
								{
									pBestTarget = pAdjacentPlot;
									iBestTurns = iTurns;
								}
							}
						}

						if (pBestTarget)
						{

							if (iBestTurns == 0)
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestTarget->getX(), pBestTarget->getY());
								pUnit->PushMission(CvTypes::getMISSION_GREAT_WORK());
								UnitProcessed(pUnit->GetID());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving and creating Great Work at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
									LogHomelandMessage(strLogString);
									continue;
								}
							}


							else
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestTarget->getX(), pBestTarget->getY());
								pUnit->finishMoves();
								UnitProcessed(it->GetID());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving Great Musician toward Great Work city at, X: %d, Y: %d", pBestTarget->getX(),  pBestTarget->getY());
									LogHomelandMessage(strLogString);
								}
							}
						}
						else
						{
							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("Could not find a target for Great Musician at, X: %d, Y: %d", pUnit->getX(),  pUnit->getY());
								LogHomelandMessage(strLogString);
							}
						}
					}
				}
			}	
			break;

		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit.pointer());
			break;
		}
	}
}


void CvHomelandAI::ExecuteScientistMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch(eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE:
			ExecuteGoldenAgeMove(pUnit.pointer());
			break;
		case GREAT_PEOPLE_DIRECTIVE_USE_POWER:
			pUnit->PushMission(CvTypes::getMISSION_DISCOVER());
			UnitProcessed(pUnit->GetID());
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Great scientist discovered free tech at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}
			break;
		case GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT:
			ExecuteWorkerMove(pUnit.pointer());
			break;
		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit.pointer());
			break;
		}
	}
}


void CvHomelandAI::ExecuteEngineerMoves()
{
	CvCity* pWonderCity;
	int iTurnsToTarget;

	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch(eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT:
			ExecuteWorkerMove(pUnit.pointer());
			break;
		case GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE:
			ExecuteGoldenAgeMove(pUnit.pointer());
			break;
		case GREAT_PEOPLE_DIRECTIVE_USE_POWER:
		{

			int iNextWonderWeight;
			CvCity* pCityToBuildAt = 0;
			BuildingTypes eNextWonderDesired = m_pPlayer->GetWonderProductionAI()->ChooseWonderForGreatEngineer(false, iNextWonderWeight, pCityToBuildAt);


			if(eNextWonderDesired == NO_BUILDING)
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Great builder can't find a good wonder to build. Running to safety");
					LogHomelandMessage(strLogString);
				}

				MoveCivilianToSafety(pUnit.pointer());
			}
			else
			{
				bool bForceWonderCity = true;


				pWonderCity = m_pPlayer->GetCitySpecializationAI()->GetWonderBuildCity();
				if(pWonderCity)
				{
					int iProductionSoFar = pWonderCity->getProduction();
					int iProductionRemaining = pWonderCity->getProductionNeeded(eNextWonderDesired);

					if(pWonderCity->getProductionBuilding() == eNextWonderDesired && iProductionSoFar * 3 < iProductionRemaining)
					{

						int iTurnsRemaining = pWonderCity->getProductionTurnsLeft();
						iTurnsToTarget = TurnsToReachTarget(pUnit, pWonderCity->plot(), true                , true);
						if(iTurnsToTarget * 3 < iTurnsRemaining)
						{
							bForceWonderCity = false;


							if(pWonderCity->getProductionBuilding() == eNextWonderDesired && iTurnsToTarget == 0 && pUnit->plot() == pWonderCity->plot())
							{
								pUnit->PushMission(CvTypes::getMISSION_HURRY());
								UnitProcessed(pUnit->GetID());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Great Engineer hurrying wonder chosen by city specialization AI at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
									LogHomelandMessage(strLogString);
									continue;
								}
							}


							else
							{
								if(GetBestUnitToReachTarget(pWonderCity->plot(), MAX_INT))
								{
									ExecuteMoveToTarget(pWonderCity->plot());

									if(GC.getLogging() && GC.getAILogging())
									{
										CvString strLogString;
										strLogString.Format("Moving Great Engineer to city specialization wonder city at, X: %d, Y: %d", pWonderCity->getX(),  pWonderCity->getY());
										LogHomelandMessage(strLogString);
									}
								}
							}
						}
					}
				}

				if(bForceWonderCity)
				{
					pWonderCity = pCityToBuildAt;

					if(pWonderCity)
					{
						iTurnsToTarget = TurnsToReachTarget(pUnit, pWonderCity->plot(), false                , true);


						if(iTurnsToTarget == 0 && pUnit->plot() == pWonderCity->plot())
						{

							pWonderCity->pushOrder(ORDER_CONSTRUCT, eNextWonderDesired, -1, false, false, false);

							if (pWonderCity->getProductionTurnsLeft() > 1)
							{

								pUnit->PushMission(CvTypes::getMISSION_HURRY());
								UnitProcessed(pUnit->GetID());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Great Engineer hurrying free wonder at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
									LogHomelandMessage(strLogString);
									continue;
								}
							}
							else
							{
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Great Engineer not needed to hurry 1-turn wonder at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
									LogHomelandMessage(strLogString);
									continue;
								}
							}
						}


						else
						{
							if(GetBestUnitToReachTarget(pWonderCity->plot(), MAX_INT))
							{
								ExecuteMoveToTarget(pWonderCity->plot());

								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving Great Engineer for free wonder to city at, X: %d, Y: %d", pWonderCity->getX(),  pWonderCity->getY());
									LogHomelandMessage(strLogString);
								}
							}
						}

					}
				}
			}
		}
		break;
		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit.pointer());
			break;
		}
	}
}

void CvHomelandAI::ExecuteMerchantMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch(eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE:
			ExecuteGoldenAgeMove(pUnit.pointer());
			break;
		case GREAT_PEOPLE_DIRECTIVE_USE_POWER:

			break;
		case GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT:
			ExecuteWorkerMove(pUnit.pointer());
			break;
		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit.pointer());
			break;
		}
	}
}

void CvHomelandAI::ExecuteProphetMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch(eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT:
			ExecuteWorkerMove(pUnit.pointer());
			break;

		case GREAT_PEOPLE_DIRECTIVE_USE_POWER:


#if !defined(TRAITIFY)
			if(pUnit->CanFoundReligion(pUnit->plot()))
#else
			if (pUnit->CanFoundReligion(pUnit->plot()) && !GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->IsBannedUnitMission((MissionTypes)GC.getInfoTypeForString("MISSION_FOUND_RELIGION")))
#endif
			{
				CvString strLogString;
				strLogString.Format("Great Prophet founding a religion, X: %d, Y: %d", pUnit->plot()->getX(), pUnit->plot()->getY());
				LogHomelandMessage(strLogString);

				pUnit->DoFoundReligion();
			}


#if !defined(TRAITIFY)
			else if (pUnit->CanEnhanceReligion(pUnit->plot()))
#else
			else if (pUnit->CanEnhanceReligion(pUnit->plot()) && !GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->IsBannedUnitMission((MissionTypes)GC.getInfoTypeForString("MISSION_ENHANCE_RELIGION")))
#endif
			{
				CvString strLogString;
				strLogString.Format("Great Prophet enhancing a religion, X: %d, Y: %d", pUnit->plot()->getX(), pUnit->plot()->getY());
				LogHomelandMessage(strLogString);

				pUnit->DoEnhanceReligion();
			}


			else
			{
				CvCity* pLoopCity;
				int iLoopCity = 0;
				int iBestTurns = MAX_INT;
				CvPlot* pBestCityPlot = NULL;
				for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
				{
					bool bSkipCity = false;

					CvPlot* pTarget = pLoopCity->plot();
#ifdef AUI_WARNING_FIXES
					if (!pTarget)
						continue;
					for (uint iUnitLoop = 0; iUnitLoop < pTarget->getNumUnits(); iUnitLoop++)
#else
					for(int iUnitLoop = 0; iUnitLoop < pTarget->getNumUnits(); iUnitLoop++)
#endif
					{

						if(!pTarget->getUnitByIndex(iUnitLoop)->IsCombatUnit())
						{
							bSkipCity = true;
							break;
						}
					}

					if(!bSkipCity)
					{
						int iTurns = TurnsToReachTarget(pUnit, pTarget);
						if(iTurns < iBestTurns)
						{
							iBestTurns = iTurns;
							pBestCityPlot = pTarget;
						}
					}
				}

				if(pBestCityPlot)
				{
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestCityPlot->getX(), pBestCityPlot->getY());
					pUnit->finishMoves();
					UnitProcessed(pUnit->GetID());

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Moving Great Prophet to nearest city without civilian, X: %d, Y: %d", pBestCityPlot->getX(), pBestCityPlot->getY());
						LogHomelandMessage(strLogString);
					}
				}
				else
				{
					pUnit->finishMoves();
					UnitProcessed(pUnit->GetID());

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("No place to move Great Prophet at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);
					}
				}
			}

			break;

		case GREAT_PEOPLE_DIRECTIVE_SPREAD_RELIGION:
			{
				int iTargetTurns;
				CvPlot* pTarget = m_pPlayer->GetReligionAI()->ChooseProphetTargetPlot(pUnit, &iTargetTurns);
				if(pTarget)
				{
					if(pUnit->plot() == pTarget)
					{
						pUnit->PushMission(CvTypes::getMISSION_SPREAD_RELIGION());

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Spreading religion, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
							LogHomelandMessage(strLogString);
						}
					}
					else if(iTargetTurns < 1)
					{
						pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());

						if(pUnit->plot() == pTarget && pUnit->canMove())
						{
							pUnit->PushMission(CvTypes::getMISSION_SPREAD_RELIGION());

							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("Move to spread religion, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
								LogHomelandMessage(strLogString);
							}
						}
						else
						{
							CvAssertMsg(false, "Internal error with Prophet spread religion AI move, contact Ed.");
						}
					}
					else
					{
						
						m_CurrentBestMoveHighPriorityUnit = NULL;
						m_CurrentBestMoveUnit = m_pPlayer->getUnit(it->GetID());
						ExecuteMoveToTarget(pTarget);

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Moving to plot adjacent to conversion city, X: %d, Y: %d, Currently at, X: %d, Y: %d", pTarget->getX(), pTarget->getY(), pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
						}
					}
				}
			}
			break;

		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit.pointer());
			break;
		}
	}
}


void CvHomelandAI::ExecuteGeneralMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	CvPlot* pHolyCityPlot = NULL;
	CvCity* pHolyCity = NULL;


	bool bHaveApolloInCapital = false;
	ProjectTypes eApolloProgram = (ProjectTypes) GC.getSPACE_RACE_TRIGGER_PROJECT();
	if(eApolloProgram != NO_PROJECT)
	{
		if(GET_TEAM(m_pPlayer->getTeam()).getProjectCount(eApolloProgram) > 0)
		{
			bHaveApolloInCapital = true;
		}
	}


	bool bKeepHolyCityClear = false;
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	ReligionTypes eMyReligion = pReligions->GetReligionCreatedByPlayer(m_pPlayer->GetID());
	const CvReligion* pMyReligion = pReligions->GetReligion(eMyReligion, m_pPlayer->GetID());
	if(pMyReligion)
	{
		pHolyCityPlot = GC.getMap().plot(pMyReligion->m_iHolyCityX, pMyReligion->m_iHolyCityY);
		pHolyCity = pHolyCityPlot->getPlotCity();
		if(pHolyCity && (pHolyCity->getOwner() == m_pPlayer->GetID()))
		{
			bKeepHolyCityClear = true;
		}
	}

	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}


		if (pUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_USE_POWER)
		{
			int iValue = 0;
			CvPlot* pTargetPlot = GET_PLAYER(m_pPlayer->GetID()).FindBestArtistTargetPlot(pUnit.pointer(), iValue);
			if(pTargetPlot)
			{
				if(pUnit->plot() == pTargetPlot)
				{

					BuildTypes eSelectedBuildType = NO_BUILD;
					BuildTypes eBuild;
#ifdef AUI_WARNING_FIXES
					for (uint iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
#else
					int iBuildIndex;
					for(iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
#endif
					{
						eBuild = (BuildTypes)iBuildIndex;
						CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
						if(pkBuild == NULL)
							continue;
						
						if(!pUnit->canBuild(pTargetPlot, eBuild))
						{
							continue;
						}
	
						eSelectedBuildType = eBuild;
						break;
					}

					CvAssertMsg(eSelectedBuildType != NO_BUILD, "Great General trying to build something it doesn't qualify for");
					if (eSelectedBuildType != NO_BUILD)
					{
						pUnit->PushMission(CvTypes::getMISSION_BUILD(), eSelectedBuildType, -1, 0, (pUnit->GetLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pTargetPlot);
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Great General culture bombed/citadel'd at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
						}
					}
				}
				else
				{
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
					pUnit->finishMoves();
					UnitProcessed(pUnit->GetID());

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Great general moving to culture bomb/citadel at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);
					}
				}
				continue;
			}
		}


		if(pUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE)
		{
			ExecuteGoldenAgeMove(pUnit.pointer());
			continue;
		}




		CvPlot* pUnitPlot =  pUnit->plot();
		if(pUnitPlot->isFriendlyCity(*pUnit, false) && (!bHaveApolloInCapital || !pUnitPlot->getPlotCity()->isCapital()) && (!bKeepHolyCityClear || pUnitPlot != pHolyCityPlot))
		{
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Great General remaining as garrison for %s, X: %d, Y: %d", pUnit->plot()->getPlotCity()->getName().GetCString(), pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}
		}

		else
		{
			CvCity* pLoopCity;
			int iLoopCity = 0;
			int iBestTurns = MAX_INT;
			CvPlot* pBestCityPlot = NULL;
			for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
			{
				if(bHaveApolloInCapital && pLoopCity->isCapital())
				{
					continue;
				}

				if(bKeepHolyCityClear && pLoopCity == pHolyCity)
				{
					continue;
				}

				bool bSkipCity = false;

				CvPlot* pTarget = pLoopCity->plot();
#ifdef AUI_WARNING_FIXES
				if (!pTarget)
					continue;
				for (uint iUnitLoop = 0; iUnitLoop < pTarget->getNumUnits(); iUnitLoop++)
#else
				for(int iUnitLoop = 0; iUnitLoop < pTarget->getNumUnits(); iUnitLoop++)
#endif
				{

					if(pTarget->getUnitByIndex(iUnitLoop)->AI_getUnitAIType() == UNITAI_GENERAL)
					{
						bSkipCity = true;
						break;
					}
					else if(pTarget->getUnitByIndex(iUnitLoop)->AI_getUnitAIType() == UNITAI_ADMIRAL)
					{
						bSkipCity = true;
						break;
					}
				}

				if(!bSkipCity)
				{
					int iTurns = TurnsToReachTarget(pUnit, pTarget);
					if(iTurns < iBestTurns)
					{
						iBestTurns = iTurns;
						pBestCityPlot = pTarget;
					}
				}
			}

			if(pBestCityPlot)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestCityPlot->getX(), pBestCityPlot->getY());
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Moving Great General to city garrison, X: %d, Y: %d", pBestCityPlot->getX(), pBestCityPlot->getY());
					LogHomelandMessage(strLogString);
				}
			}
			else
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("No place to move Great General at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
	}
}


void CvHomelandAI::ExecuteAdmiralMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	CvPlot* pHolyCityPlot = NULL;
	CvCity* pHolyCity = NULL;


	bool bHaveApolloInCapital = false;
	ProjectTypes eApolloProgram = (ProjectTypes) GC.getSPACE_RACE_TRIGGER_PROJECT();
	if(eApolloProgram != NO_PROJECT)
	{
		if(GET_TEAM(m_pPlayer->getTeam()).getProjectCount(eApolloProgram) > 0)
		{
			bHaveApolloInCapital = true;
		}
	}


	bool bKeepHolyCityClear = false;
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	ReligionTypes eMyReligion = pReligions->GetReligionCreatedByPlayer(m_pPlayer->GetID());
	const CvReligion* pMyReligion = pReligions->GetReligion(eMyReligion, m_pPlayer->GetID());
	if(pMyReligion)
	{
		pHolyCityPlot = GC.getMap().plot(pMyReligion->m_iHolyCityX, pMyReligion->m_iHolyCityY);
		pHolyCity = pHolyCityPlot->getPlotCity();
		if(pHolyCity && pHolyCity->isCoastal() && pHolyCity->getOwner() == m_pPlayer->GetID())
		{
			bKeepHolyCityClear = true;
		}
	}

	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		if(pUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE)
		{
			ExecuteGoldenAgeMove(pUnit.pointer());
			continue;
		}

		bool bNotAtFriendlyCity = !pUnit->plot()->isCity() || pUnit->plot()->getOwner() != pUnit->getOwner();


		CvCity* pLoopCity;
		int iLoopCity = 0;
		CvWeightedVector<CvCity *, SAFE_ESTIMATE_NUM_CITIES, true> weightedCityList;
		for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
		{
			if (bHaveApolloInCapital && pLoopCity->isCapital())
			{
				continue;
			}

			if (bKeepHolyCityClear && pLoopCity == pHolyCity)
			{
				continue;
			}

			if (!pLoopCity->isCoastal())
			{
				continue;
			}


			bool bSkipCity = false;
			CvPlot* pTarget = pLoopCity->plot();
#ifdef AUI_WARNING_FIXES
			for (uint iUnitLoop = 0; iUnitLoop < pTarget->getNumUnits(); iUnitLoop++)
#else
			for(int iUnitLoop = 0; iUnitLoop < pTarget->getNumUnits(); iUnitLoop++)
#endif
			{
				CvUnit *pLoopUnit = pTarget->getUnitByIndex(iUnitLoop);
				if(pLoopUnit->AI_getUnitAIType() == UNITAI_GENERAL && pLoopUnit->GetID() != pUnit->GetID())
				{
					bSkipCity = true;
					break;
				}
				else if(pLoopUnit->AI_getUnitAIType() == UNITAI_ADMIRAL && pLoopUnit->GetID() != pUnit->GetID())
				{
					bSkipCity = true;
					break;
				}
			}
			if(bSkipCity)
			{
				continue;
			}

			int iTurns = TurnsToReachTarget(pUnit, pLoopCity->plot());


			if (bNotAtFriendlyCity)
			{
				if (iTurns == MAX_INT)
				{
					continue;
				}
			}


			int iWeight = pTarget->GetSizeLargestAdjacentWater();


			if (pLoopCity->getDamage() > 0)
			{
				iWeight /= pLoopCity->getDamage();
			}


			if (iTurns != MAX_INT)
			{
				iWeight -= iTurns;
			}

			weightedCityList.push_back(pLoopCity, iWeight);
		}

		weightedCityList.SortItems();
		if (weightedCityList.size() > 0)
		{
			CvCity *pChosenCity = weightedCityList.GetElement(0);


			if (pUnit->plot() == pChosenCity->plot())
			{
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Great Admiral still in most favored city of %s, X: %d, Y: %d", pChosenCity->getName().GetCString(), pChosenCity->getX(), pChosenCity->getY());
					LogHomelandMessage(strLogString);
				}
			}
			

			else if (!bNotAtFriendlyCity)
			{
				pUnit->PushMission(CvTypes::getMISSION_CHANGE_ADMIRAL_PORT(), pChosenCity->getX(), pChosenCity->getY());
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Transferring Great Admiral to city of %s, X: %d, Y: %d", pChosenCity->getName().GetCString(), pChosenCity->getX(), pChosenCity->getY());
					LogHomelandMessage(strLogString);
				}
			}


			else
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pChosenCity->getX(), pChosenCity->getY());
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Moving Great Admiral normally to city of %s, X: %d, Y: %d", pChosenCity->getName().GetCString(), pChosenCity->getX(), pChosenCity->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
		else
		{
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("No place to move Great Admiral at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}
		}
	}
}


void CvHomelandAI::ExecuteMissionaryMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		int iTargetTurns;
		CvPlot* pTarget = m_pPlayer->GetReligionAI()->ChooseMissionaryTargetPlot(pUnit, &iTargetTurns);
		if(pTarget)
		{
			if(pUnit->plot() == pTarget)
			{
				pUnit->PushMission(CvTypes::getMISSION_SPREAD_RELIGION());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Spreading religion, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
					LogHomelandMessage(strLogString);
				}
			}
			else if(iTargetTurns < 1)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());

				if(pUnit->plot() == pTarget && pUnit->canMove())
				{
					pUnit->PushMission(CvTypes::getMISSION_SPREAD_RELIGION());

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Move to spread religion, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
						LogHomelandMessage(strLogString);
					}
				}
				else
				{
					CvAssertMsg(false, "Internal error with Missionary AI move, contact Ed.");
				}
			}
			else
			{
				m_CurrentBestMoveHighPriorityUnit = NULL;
				m_CurrentBestMoveUnit = m_pPlayer->getUnit(it->GetID());
				ExecuteMoveToTarget(pTarget);

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Moving to plot adjacent to conversion city, X: %d, Y: %d, Currently at, X: %d, Y: %d", pTarget->getX(), pTarget->getY(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
	}
}


void CvHomelandAI::ExecuteInquisitorMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		int iTargetTurns;
		CvPlot* pTarget = m_pPlayer->GetReligionAI()->ChooseInquisitorTargetPlot(pUnit, &iTargetTurns);
		if(pTarget)
		{
			if(pUnit->plot() == pTarget)
			{
				pUnit->PushMission(CvTypes::getMISSION_REMOVE_HERESY());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Removing heresy, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
					LogHomelandMessage(strLogString);
				}
			}
			else if(iTargetTurns < 1)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());

				if(pUnit->plot() == pTarget && pUnit->canMove())
				{
					pUnit->PushMission(CvTypes::getMISSION_REMOVE_HERESY());

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Move to remove heresy, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
						LogHomelandMessage(strLogString);
					}
				}
				else
				{
					CvAssertMsg(false, "Internal error with Inquisitor AI move, contact Ed.");
				}
			}
			else
			{
				m_CurrentBestMoveHighPriorityUnit = NULL;
				m_CurrentBestMoveUnit = m_pPlayer->getUnit(it->GetID());
				ExecuteMoveToTarget(pTarget);

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Moving to plot adjacent to heresy removal city, X: %d, Y: %d, Currently at, X: %d, Y: %d", pTarget->getX(), pTarget->getY(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
	}
}


void CvHomelandAI::ExecuteSSPartMoves()
{
	CvCity* pLoopCity;
	CvCity* pCapitalCity = NULL;
	int iLoopCity = 0;
	for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
	{
		if(pLoopCity->isCapital())
		{
			pCapitalCity = pLoopCity;
			break;
		}
	}

	if(pCapitalCity != NULL)
	{
		FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
		for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
		{
			UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
			if(!pUnit)
			{
				continue;
			}

			if(pUnit->plot() != pCapitalCity->plot())
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pCapitalCity->getX(), pCapitalCity->getY());
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp;
					strTemp = pUnit->getUnitInfo().GetDescription();
					strLogString.Format("Moving %s to spaceship - now at, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
	}
}


void CvHomelandAI::ExecuteSSPartAdds()
{
	CvCity* pLoopCity;
	CvCity* pCapitalCity = NULL;
	int iLoopCity = 0;
	for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
	{
		if(pLoopCity->isCapital())
		{
			pCapitalCity = pLoopCity;
			break;
		}
	}

	if(pCapitalCity != NULL)
	{
		FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
		for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
		{
			UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
			if(!pUnit)
			{
				continue;
			}

			if(pUnit->plot() == pCapitalCity->plot())
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp;
					strTemp = pUnit->getUnitInfo().GetDescription();
					strLogString.Format("Adding %s to spaceship, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}

				pUnit->PushMission(CvTypes::getMISSION_SPACESHIP());
				UnitProcessed(pUnit->GetID());
			}

			else if (TurnsToReachTarget(pUnit, pCapitalCity->plot()) == 0)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pCapitalCity->getX(), pCapitalCity->getY());
				pUnit->PushMission(CvTypes::getMISSION_SPACESHIP());
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp;
					strTemp = pUnit->getUnitInfo().GetDescription();
					strLogString.Format("Moving %s to spaceship and adding it directly in", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
	}
}


void CvHomelandAI::ExecuteTreasureMoves()
{
	CvCity* pLoopCity;
	CvCity* pCapitalCity = NULL;
	int iLoopCity = 0;
	for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
	{
		if(pLoopCity->isCapital())
		{
			pCapitalCity = pLoopCity;
			break;
		}
	}

	if(pCapitalCity != NULL)
	{
		FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
		for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
		{
			UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
			if(!pUnit)
			{
				continue;
			}

			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pCapitalCity->getX(), pCapitalCity->getY());
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				CvString strTemp;
				strTemp = pUnit->getUnitInfo().GetDescription();
				strLogString.Format("Moving %s to capital - now at, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}
		}
	}
}


void CvHomelandAI::ExecuteAircraftMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;

	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		CvPlot* pUnitPlot = pUnit->plot();
		CvUnit* pTransportUnit = NULL;
		CvPlot* pBestPlot = NULL;
		int iMostDangerous = 0;


		int iLoopUnit = 0;
		for(CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoopUnit))
		{
			CvPlot* pLoopUnitPlot = pLoopUnit->plot();

			if(pLoopUnit->getDamage() > (GC.getMAX_HIT_POINTS() / 5))
			{
				continue;
			}

			if(pBestPlot != pUnitPlot && !pUnit->canRebaseAt(pUnitPlot, pLoopUnitPlot->getX(),pLoopUnitPlot->getY()))
			{
				continue;
			}

			if(!pUnit->canLoadUnit(*pLoopUnit, *pLoopUnitPlot))
			{
				continue;
			}

			int iPlotDanger = m_pPlayer->GetPlotDanger(*pLoopUnitPlot);
			if (pLoopUnit->getArmyID() != FFreeList::INVALID_INDEX)
			{
				iPlotDanger += 5000;
			}
			if(iPlotDanger >= iMostDangerous)
			{
				iMostDangerous = iPlotDanger;
				pBestPlot = pLoopUnitPlot;
				pTransportUnit = pLoopUnit;
			}

		}

		CvCity* pLoopCity;
		int iLoopCity = 0;
		for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
		{
			CvPlot* pTarget = pLoopCity->plot();

			if(pLoopCity->getDamage() > (pLoopCity->GetMaxHitPoints() / 5))
			{
				continue;
			}

			if(pBestPlot != pUnitPlot && !pUnit->canRebaseAt(pUnitPlot,pTarget->getX(),pTarget->getY()))
			{
				continue;
			}

			if (pLoopCity->IsRazing())
			{
				continue;
			}

			int iPlotDanger = m_pPlayer->GetPlotDanger(*pTarget);
			if(iPlotDanger >= iMostDangerous)
			{
				iMostDangerous = iPlotDanger;
				pBestPlot = pTarget;
				pTransportUnit = NULL;
			}
		}

		if(pBestPlot && pBestPlot != pUnitPlot)
		{
			pUnit->PushMission(CvTypes::getMISSION_REBASE(), pBestPlot->getX(), pBestPlot->getY());
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				CvString strTemp, strTemp2;
#ifdef AUI_WARNING_FIXES
				strTemp = pUnit->getUnitInfo().GetDescription();
				if (pTransportUnit)
				{
					strTemp2 = pTransportUnit->getUnitInfo().GetDescription();
#else
				strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
				if (pTransportUnit)
				{
					strTemp2 = GC.getUnitInfo(pTransportUnit->getUnitType())->GetDescription();
#endif
				}

				if (pBestPlot->getPlotCity())
				{
					strLogString.Format("Rebasing %s to city garrison, X: %d, Y: %d", strTemp.GetCString(), pBestPlot->getX(), pBestPlot->getY());
				}
				else
				{
					strLogString.Format("Rebasing %s onto %s, X: %d, Y: %d", strTemp.GetCString(), strTemp2.GetCString(), pBestPlot->getX(), pBestPlot->getY());
				}
				LogHomelandMessage(strLogString);
			}
		}
		else
		{
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());

			if(GC.getLogging() && GC.getAILogging())
			{
#ifdef AUI_WARNING_FIXES
				CvString strTemp = pUnit->getUnitInfo().GetDescription();
#else
				CvString strTemp;
				strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
#endif
				CvString strLogString;
				strLogString.Format("No better place to move %s at, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}
		}
	}
}



bool CvHomelandAI::MoveCivilianToSafety(CvUnit* pUnit, bool bIgnoreUnits)
{
	int iSearchRange = pUnit->SearchRange(1);


	WeightedPlotVector aBestPlotList;
	aBestPlotList.reserve( ((iSearchRange * 2) + 1) * 2 );

#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX, iDX;
	CvPlot* pLoopPlot;
	for (int iDY = -iSearchRange; iDY <= iSearchRange; iDY++)
	{
		iMaxDX = iSearchRange - MAX(0, iDY);
		for (iDX = -iSearchRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
		{

			pLoopPlot = plotXY(pUnit->getX(), pUnit->getY(), iDX, iDY);
#else
	for(int iDX = -(iSearchRange); iDX <= iSearchRange; iDX++)
	{
		for(int iDY = -(iSearchRange); iDY <= iSearchRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXYWithRangeCheck(pUnit->getX(), pUnit->getY(), iDX, iDY, iSearchRange);
#endif
			if(!pLoopPlot)
			{
				continue;
			}

			if(!pUnit->PlotValid(pLoopPlot))
			{
				continue;
			}

			if(pLoopPlot->isVisibleEnemyUnit(pUnit))
			{
				continue;
			}

			int iValue = 0;
			if(pLoopPlot->getOwner() != NO_PLAYER && GET_PLAYER(pLoopPlot->getOwner()).getTeam() == m_pPlayer->getTeam())
			{

				iValue += 1;
			}

			CvCity* pCity = pLoopPlot->getPlotCity();
			if(pCity && pCity->getTeam() == pUnit->getTeam())
			{
				iValue += pCity->getStrengthValue() * (GC.getMAX_CITY_DEFENSE_DAMAGE() - pCity->getDamage());
			}
			else if(!bIgnoreUnits)
			{
				IDInfo* pUnitNode = pLoopPlot->headUnitNode();
				while(pUnitNode != NULL)
				{
					CvUnit* pLoopUnit = ::getUnit(*pUnitNode);
					pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

					if(pLoopUnit && pLoopUnit->getOwner() == pUnit->getOwner())
					{
						if(pLoopUnit->IsCanDefend())
						{
							if(pLoopUnit != pUnit)
							{
								if(pLoopUnit->isWaiting() || !(pLoopUnit->canMove()))
								{
#if !defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
									iValue += pLoopUnit->GetMaxDefenseStrength(pLoopPlot, NULL) * pLoopUnit->GetCurrHitPoints();
#else
									CvCombatInfo kCombatInfo;
									kCombatInfo.setUnit(BATTLE_UNIT_DEFENDER, pLoopUnit);
									kCombatInfo.setPlot(pLoopPlot);
									iValue += pLoopUnit->GetMaxDefenseStrength(kCombatInfo) * pLoopUnit->GetCurrHitPoints();
#endif
								}
							}
						}
					}
				}
			}

			iValue -= m_pPlayer->GetPlotDanger(*pLoopPlot);

			aBestPlotList.push_back(pLoopPlot, iValue);
		}
	}


	CvPlot* pBestPlot = NULL;
	uint uiListSize;
	if ((uiListSize = aBestPlotList.size()) > 0)
	{
		aBestPlotList.SortItems();
		for (uint i = 0; i < uiListSize; ++i )	
		{
			CvPlot* pPlot = aBestPlotList.GetElement(i);

			int iPathTurns;
			if(!pUnit->GeneratePath(pPlot, MOVE_UNITS_IGNORE_DANGER, true, &iPathTurns))
			{
				continue;
			}


			if(iPathTurns > 1)
			{
				continue;
			}

			pBestPlot = pPlot;
			break;
		}
	}

	if(pBestPlot != NULL)
	{
		if(pUnit->atPlot(*pBestPlot))
		{
			if (pUnit->canHold(pBestPlot))
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
#ifdef AUI_WARNING_FIXES
					CvString strTemp = pUnit->getUnitInfo().GetDescription();
#else
					CvString strTemp;
					strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
#endif
					strLogString.Format("%s (%d) tried to move to safety but is at the best spot, X: %d, Y: %d", strTemp.GetCString(), pUnit->GetID(), pBestPlot->getX(), pBestPlot->getY());
					LogHomelandMessage(strLogString);
				}

				pUnit->PushMission(CvTypes::getMISSION_SKIP());
				return true;
			}
			else
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
#ifdef AUI_WARNING_FIXES
					CvString strTemp = pUnit->getUnitInfo().GetDescription();
#else
					CvString strTemp;
					strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
#endif
					strLogString.Format("%s (%d) tried to move to safety but cannot hold in current location, X: %d, Y: %d", strTemp.GetCString(), pUnit->GetID(), pBestPlot->getX(), pBestPlot->getY());
					LogHomelandMessage(strLogString);
				}
				pUnit->SetAutomateType(NO_AUTOMATE);
			}
		}
		else
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
#ifdef AUI_WARNING_FIXES
				CvString strTemp = pUnit->getUnitInfo().GetDescription();
#else
				CvString strTemp;
				strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
#endif
				strLogString.Format("%s (%d) moving to safety, X: %d, Y: %d", strTemp.GetCString(), pUnit->GetID(), pBestPlot->getX(), pBestPlot->getY());
				LogHomelandMessage(strLogString);
			}

			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY(), MOVE_UNITS_IGNORE_DANGER);
			return true;
		}
	}
	else
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			CvString strTemp;
			strTemp = pUnit->getUnitInfo().GetDescription();
			strLogString.Format("%s (%d) tried to move to a safe point but couldn't find a good place to go", strTemp.GetCString(), pUnit->GetID());
			LogHomelandMessage(strLogString);
		}
	}


	return false;
}


void CvHomelandAI::ExecuteTradeUnitMoves()
{
	CvTradeAI* pkTradeAI = m_pPlayer->GetTradeAI();

	static TradeConnectionList aTradeConnections;
	pkTradeAI->PrioritizeTradeRoutes(aTradeConnections);

	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		int iOriginPlotIndex = MAX_INT;
		int iDestPlotIndex = MAX_INT;
		TradeConnectionType eTradeConnectionType = NUM_TRADE_CONNECTION_TYPES;
		bool bDisband = false;
		bool bSuccess = m_pPlayer->GetTradeAI()->ChooseTradeUnitTargetPlot(pUnit, iOriginPlotIndex, iDestPlotIndex, eTradeConnectionType, bDisband, aTradeConnections);
		if (bSuccess)
		{
			if (bDisband)
			{
				pUnit->kill(true);
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Disbanding unit because we want different domains for our trade units");
					LogHomelandMessage(strLogString);
				}
			}
			else
			{
				CvPlot* pOriginPlot = GC.getMap().plotByIndex(iOriginPlotIndex);
				if (pOriginPlot != pUnit->plot())
				{
					pUnit->PushMission(CvTypes::getMISSION_CHANGE_TRADE_UNIT_HOME_CITY(), pOriginPlot->getX(), pOriginPlot->getY());
					if(GC.getLogging() && GC.getAILogging())
					{
						CvPlot* pPlot = GC.getMap().plotByIndex(iDestPlotIndex);
						CvString strLogString;
						strLogString.Format("Changing trade route home city, X: %d, Y: %d", pPlot->getX(), pPlot->getY());
						LogHomelandMessage(strLogString);
					}
				}
				else
				{
					pUnit->PushMission(CvTypes::getMISSION_ESTABLISH_TRADE_ROUTE(), iDestPlotIndex, eTradeConnectionType);
					if(GC.getLogging() && GC.getAILogging())
					{
						CvPlot* pPlot = GC.getMap().plotByIndex(iDestPlotIndex);
						CvString strLogString;

						switch (eTradeConnectionType)
						{
						case TRADE_CONNECTION_FOOD:
							strLogString.Format("Establishing trade route, X: %d, Y: %d, food", pPlot->getX(), pPlot->getY());
							break;
						case TRADE_CONNECTION_INTERNATIONAL:
							strLogString.Format("Establishing trade route, X: %d, Y: %d, gold", pPlot->getX(), pPlot->getY());
							break;
						case TRADE_CONNECTION_PRODUCTION:
							strLogString.Format("Establishing trade route, X: %d, Y: %d, production", pPlot->getX(), pPlot->getY());
							break;
						}

						LogHomelandMessage(strLogString);
					}
				}
			}
		}
		else
		{
			pUnit->PushMission(CvTypes::getMISSION_SKIP());
			pUnit->finishMoves();			
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Trade unit idling");
				LogHomelandMessage(strLogString);
			}
		}
	}
}


void CvHomelandAI::ExecuteArchaeologistMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		CvPlot* pTarget = FindArchaeologistTarget(pUnit);
		if (pTarget)
		{
			BuildTypes eBuild = (BuildTypes)GC.getInfoTypeForString("BUILD_ARCHAEOLOGY_DIG");
			if(pUnit->UnitPathTo(pTarget->getX(), pTarget->getY(), 0) > 0)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Archaeologist moving to site at, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
					LogHomelandMessage(strLogString);
				}

				if(pUnit->plot() == pTarget)
				{
					pUnit->PushMission(CvTypes::getMISSION_BUILD(), eBuild, -1, 0, (pUnit->GetLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pTarget);
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Archaeologist creating dig at, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
						LogHomelandMessage(strLogString);
					}
				}
				else
				{
					pUnit->finishMoves();
				}
			}

			UnitProcessed(pUnit->GetID());
		}
	}
}


void CvHomelandAI::EliminateAdjacentSentryPoints()
{

	std::stable_sort(m_TargetedSentryPoints.begin(), m_TargetedSentryPoints.end());


	std::vector<CvHomelandTarget> tempPoints;
	tempPoints = m_TargetedSentryPoints;


	m_TargetedSentryPoints.clear();


	std::vector<CvHomelandTarget>::iterator it, it2;
	for(it = tempPoints.begin(); it != tempPoints.end(); ++it)
	{
		bool bFoundAdjacent = false;


		for(it2 = m_TargetedSentryPoints.begin(); it2 != m_TargetedSentryPoints.end(); ++it2)
		{
			if(plotDistance(it->GetTargetX(), it->GetTargetY(), it2->GetTargetX(), it2->GetTargetY()) == 1)
			{
				bFoundAdjacent = true;
				break;
			}
		}

		if(!bFoundAdjacent)
		{
			m_TargetedSentryPoints.push_back(*it);
		}
	}
}


void CvHomelandAI::EliminateAdjacentHomelandRoads()
{

	std::vector<CvHomelandTarget> tempPoints;
	tempPoints = m_TargetedHomelandRoads;


	m_TargetedHomelandRoads.clear();


	std::vector<CvHomelandTarget>::iterator it, it2;
	for(it = tempPoints.begin(); it != tempPoints.end(); ++it)
	{
		bool bFoundAdjacent = false;


		for(it2 = m_TargetedHomelandRoads.begin(); it2 != m_TargetedHomelandRoads.end(); ++it2)
		{
			if(plotDistance(it->GetTargetX(), it->GetTargetY(), it2->GetTargetX(), it2->GetTargetY()) == 1)
			{
				bFoundAdjacent = true;
				break;
			}
		}

		if(!bFoundAdjacent)
		{
			m_TargetedHomelandRoads.push_back(*it);
		}
	}
}


bool CvHomelandAI::FindWorkersInSameArea(CvPlot* pTarget, BuildTypes eBuild)
{
	bool rtnValue = false;
	ClearCurrentMoveUnits();


	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pLoopUnit = m_pPlayer->getUnit(*it);
		if(pLoopUnit)
		{

			if(pLoopUnit->IsCanAttack())
			{
				continue;
			}


			if(pLoopUnit->canBuild(pTarget, eBuild))
			{
				CvHomelandUnit unit;
				int iMoves = TurnsToReachTarget(pLoopUnit.pointer(), pTarget);

				if(iMoves != MAX_INT)
				{
					unit.SetID(pLoopUnit->GetID());
					unit.SetMovesToTarget(iMoves);
					m_CurrentMoveUnits.push_back(unit);
					rtnValue = true;
				}
			}
		}
	}


	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return rtnValue;
}


bool CvHomelandAI::FindUnitsForThisMove(AIHomelandMove eMove, bool bFirstTime)
{
	bool rtnValue = false;

	if(bFirstTime)
	{
		ClearCurrentMoveUnits();
		ClearCurrentMoveHighPriorityUnits();


		for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
		{
			UnitHandle pLoopUnit = m_pPlayer->getUnit(*it);
			if(pLoopUnit && !pLoopUnit->isHuman())
			{

				if(!pLoopUnit->IsCombatUnit())
				{
					continue;
				}

#ifndef AUI_HOMELAND_ALWAYS_MOVE_SCOUTS

				if(pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE && m_pPlayer->GetEconomicAI()->GetReconState() != RECON_STATE_ENOUGH)
				{
					continue;
				}
#endif

				bool bSuitableUnit = false;
				bool bHighPriority = false;

				switch(eMove)
				{
				case AI_HOMELAND_MOVE_GARRISON:
				case AI_HOMELAND_MOVE_GARRISON_CITY_STATE:

					if(pLoopUnit->isRanged() && !pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_CITY_BOMBARD))
					{
						bSuitableUnit = true;
						bHighPriority = true;
					}


					else if(pLoopUnit->IsCanAttack())
					{

						if(pLoopUnit->getDefenseModifier() == 0 &&
						        pLoopUnit->getAttackModifier() == 0 &&
						        pLoopUnit->getExtraCombatPercent() == 0)
						{
							bSuitableUnit = true;
						}
					}
					break;

				case AI_HOMELAND_MOVE_SENTRY:

					if(!pLoopUnit->isRanged() && !pLoopUnit->noDefensiveBonus())
					{
						bSuitableUnit = true;


						if(pLoopUnit->getExtraVisibilityRange() > 0)
						{
							bHighPriority = true;
						}
					}
					else if(pLoopUnit->noDefensiveBonus() && pLoopUnit->getExtraVisibilityRange() > 0)
					{
						bSuitableUnit = true;
						bHighPriority = true;
					}
					break;

				case AI_HOMELAND_MOVE_MOBILE_RESERVE:

					if(pLoopUnit->isRanged() || pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_FAST_ATTACK))
					{
						bSuitableUnit = true;
						bHighPriority = true;
					}
					else if(pLoopUnit->IsCanAttack())
					{
						bSuitableUnit = true;
					}
					break;

				case AI_HOMELAND_MOVE_ANCIENT_RUINS:

					if(pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_FAST_ATTACK))
					{
						bSuitableUnit = true;
						bHighPriority = true;
					}
					else if(pLoopUnit->IsCanAttack())
					{
						bSuitableUnit = true;
					}
					break;
#ifdef AUI_HOMELAND_PLOT_WORKER_MOVES_ALSO_PLOTS_WORKER_DEFENSE
				case AI_HOMELAND_MOVE_UNASSIGNED:

					bSuitableUnit = pLoopUnit->IsCombatUnit();
					break;
#endif
				}


				if(bSuitableUnit)
				{
					CvHomelandUnit unit;
					unit.SetID(pLoopUnit->GetID());

					if(bHighPriority)
					{
						m_CurrentMoveHighPriorityUnits.push_back(unit);
					}
					else
					{
						m_CurrentMoveUnits.push_back(unit);
					}
					rtnValue = true;
				}
			}
		}
	}


	else
	{
		FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL > tempList;
		FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
		std::list<int>::iterator it2;


		tempList = m_CurrentMoveUnits;
		ClearCurrentMoveUnits();
		for(it = tempList.begin(); it != tempList.end(); ++it)
		{
			it2 = find(m_CurrentTurnUnits.begin(), m_CurrentTurnUnits.end(), (int)it->GetID());
			if(it2 != m_CurrentTurnUnits.end())
			{
				m_CurrentMoveUnits.push_back(*it);
				rtnValue = true;
			}
		}


		tempList = m_CurrentMoveHighPriorityUnits;
		ClearCurrentMoveHighPriorityUnits();
		for(it = tempList.begin(); it != tempList.end(); ++it)
		{
			it2 = find(m_CurrentTurnUnits.begin(), m_CurrentTurnUnits.end(), (int)it->GetID());
			if(it2 != m_CurrentTurnUnits.end())
			{
				m_CurrentMoveHighPriorityUnits.push_back(*it);
				rtnValue = true;
			}
		}
	}

	return rtnValue;
}


CvPlot* CvHomelandAI::FindPatrolTarget(CvUnit* pUnit)
{
	CvPlot* pAdjacentPlot;
	CvPlot* pBestPlot;
	int iValue;
	int iBestValue;
	int iI;

	iBestValue = 0;
	pBestPlot = NULL;

#ifdef AUI_HOMELAND_FIND_PATROL_TARGET_DESIRES_BORDER_AND_ROUTE_AND_CIVILIAN_GUARD
	int iValueBonus;
	if (pUnit->IsCombatUnit())
	{
		pBestPlot = pUnit->plot();
		const IDInfo* pUnitNode = pBestPlot->headUnitNode();
		const UnitHandle pLoopUnit;

		while (pUnitNode != NULL)
		{
			pLoopUnit = GetPlayerUnit(*pUnitNode);
			if (pLoopUnit && !pLoopUnit->IsCombatUnit())
			{
				if (!pLoopUnit->canMove())
					return pBestPlot;
				else
				{
					iBestValue = 20001;
					break;
				}
			}
			pUnitNode = pBestPlot->nextUnitNode(pUnitNode);
		}
	}
#endif

#if defined(AUI_HOMELAND_TWEAKED_FIND_PATROL_TARGET_CIVILIAN_NO_DANGER)
	int iMyDanger = m_pPlayer->GetPlotDanger(*(pUnit->plot()));
#endif

	for(iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pAdjacentPlot = plotDirection(pUnit->getX(), pUnit->getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			if(pUnit->PlotValid(pAdjacentPlot))
			{
				if(!(pAdjacentPlot->isVisibleEnemyUnit(pUnit)))
				{
					if(pUnit->GeneratePath(pAdjacentPlot, 0, true))
					{
#ifdef AUI_HOMELAND_FIND_PATROL_TARGET_DESIRES_BORDER_AND_ROUTE_AND_CIVILIAN_GUARD
						iValueBonus = 0;
#endif
						iValue = (1 + GC.getGame().getJonRandNum(10000, "AI Patrol"));


						if(pAdjacentPlot->getOwner() == pUnit->getOwner())
						{
#ifdef AUI_HOMELAND_FIND_PATROL_TARGET_DESIRES_BORDER_AND_ROUTE_AND_CIVILIAN_GUARD
							iValueBonus = 5000;
#ifdef AUI_HOMELAND_FIND_PATROL_MOVES_CIVILIANS_PATROL_TO_SAFETY
							if (pAdjacentPlot->isAdjacentPlayer(NO_PLAYER) || pAdjacentPlot->IsAdjacentOwnedByOtherTeam(m_pPlayer->getTeam()) || pAdjacentPlot->isValidRoute(pUnit))
#else
							if ((pUnit->IsCombatUnit() && (pAdjacentPlot->isAdjacentPlayer(NO_PLAYER) || pAdjacentPlot->IsAdjacentOwnedByOtherTeam(m_pPlayer->getTeam()))) ||
								(pAdjacentPlot->isValidRoute(pUnit) && (!pAdjacentPlot->isCity() || !pUnit->IsCombatUnit())))
#endif
							{
								iValueBonus += 5000;
							}
#else
							iValue += 10000;
#endif
						}

#if defined(AUI_HOMELAND_TWEAKED_FIND_PATROL_TARGET_CIVILIAN_NO_DANGER)
						if (!pUnit->IsCombatUnit())
						{
							int iDanger = m_pPlayer->GetPlotDanger(*pAdjacentPlot);
							if (iDanger > 0)
							{
								if (iMyDanger > 0)
								{
									iValue -= iDanger;
#ifdef AUI_HOMELAND_FIND_PATROL_TARGET_DESIRES_BORDER_AND_ROUTE_AND_CIVILIAN_GUARD
									iValueBonus = 0;
#else

									if (pAdjacentPlot->getOwner() == pUnit->getOwner())
									{
										iValue -= 9999;
									}
#endif
								}
								else
								{
									iValue = -1;
								}
							}
						}
						else
						{
							const IDInfo* pUnitNode = pAdjacentPlot->headUnitNode();
							const UnitHandle pLoopUnit;

							while (pUnitNode != NULL)
							{
								pLoopUnit = GetPlayerUnit(*pUnitNode);
								if (pLoopUnit && !pLoopUnit->IsCombatUnit())
								{
									if (!pLoopUnit->canMove())
										iValueBonus += 100000;
									else
										iValueBonus += 10000;
									break;
								}
								pUnitNode = pAdjacentPlot->nextUnitNode(pUnitNode);
							}
						}
#endif
#ifdef AUI_HOMELAND_FIND_PATROL_TARGET_DESIRES_BORDER_AND_ROUTE_AND_CIVILIAN_GUARD
						iValue += iValueBonus;
#endif

						if(GC.getLogging() && GC.getAILogging()){
							CvString strLogString;
							strLogString.Format("Adjacent Patrol Plot Score, %i, %i, %i", iValue, pAdjacentPlot->getX(), pAdjacentPlot->getY());
							LogPatrolMessage(strLogString, pUnit);
						}

						if(iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestPlot = pAdjacentPlot;
						}
					}
					else
					{
						if(GC.getLogging() && GC.getAILogging()){
							CvString strLogString;
							strLogString.Format("Adjacent Patrol Plot !GeneratePath(), , %i, %i", pAdjacentPlot->getX(), pAdjacentPlot->getY());
							LogPatrolMessage(strLogString, pUnit);
						}
					}
				}
				else
				{
					if(GC.getLogging() && GC.getAILogging()){
						CvString strLogString;
						strLogString.Format("Adjacent Patrol Plot !isVisibleEnemyUnit(), , %i, %i", pAdjacentPlot->getX(), pAdjacentPlot->getY());
						LogPatrolMessage(strLogString, pUnit);
					}
				}
			}
			else
			{
				if(GC.getLogging() && GC.getAILogging()){
					CvString strLogString;
					strLogString.Format("Adjacent Patrol Plot not valid, , %i, %i", pAdjacentPlot->getX(), pAdjacentPlot->getY());
					LogPatrolMessage(strLogString, pUnit);
				}
			}
		}
		else
		{
			if(GC.getLogging() && GC.getAILogging()){
				CvString strLogString;
				strLogString.Format("Adjacent Patrol Plot not found");
				LogPatrolMessage(strLogString, pUnit);
			}
		}
	}

	if(pBestPlot != NULL)
	{
		if(GC.getLogging() && GC.getAILogging()){
			CvString strLogString;
			strLogString.Format("Patrol Target FOUND, %i, %i, %i", iBestValue, pBestPlot->getX(), pBestPlot->getY());
			LogPatrolMessage(strLogString, pUnit);
		}

		CvAssert(!pUnit->atPlot(*pBestPlot));
		return pBestPlot;
	}
	else
	{
		if(GC.getLogging() && GC.getAILogging()){
			CvString strLogString;
			strLogString.Format("Patrol Target NOT FOUND");
			LogPatrolMessage(strLogString, pUnit);
		}
	}

	return NULL;
}



bool CvHomelandAI::GetClosestUnitByTurnsToTarget(CvHomelandAI::MoveUnitsArray &kMoveUnits, CvPlot* pTarget, int iMaxTurns, CvUnit** ppClosestUnit, int* piClosestTurns)
{
	CvUnit* pBestUnit = NULL;
	int iMinTurns = MAX_INT;
	MoveUnitsArray::iterator it;

	int iFailedPaths = 0;

	const int MAX_FAILED_PATHS = 2;

	const int EARLY_OUT_FAILED_PATH_DISTANCE = 12;



	for(it = kMoveUnits.begin(); it != kMoveUnits.end(); ++it)
	{
		UnitHandle pLoopUnit = m_pPlayer->getUnit(it->GetID());
		if (pLoopUnit)
		{
			int iDistance = it->GetMovesToTarget();
			if (iDistance == MAX_INT)
				continue;
			int iMoves = TurnsToReachTarget(pLoopUnit.pointer(), pTarget);
			it->SetMovesToTarget(iMoves);

			if (iMoves != MAX_INT)
			{

				if (iDistance == 0 || (iMoves <= iDistance && iMoves <= iMaxTurns && iMoves < iMinTurns))
				{
					pBestUnit = pLoopUnit.pointer();
					iMinTurns = iMoves;
					break;
				}

				if (iMoves < iMinTurns)
				{
					pBestUnit = pLoopUnit.pointer();
					iMinTurns = iMoves;
				}


				if (iDistance >= GC.getAI_HOMELAND_ESTIMATE_TURNS_DISTANCE())
					break;
			}
			else
			{
				++iFailedPaths;
				if (iFailedPaths >= MAX_FAILED_PATHS)
					break;
				if (iDistance >= EARLY_OUT_FAILED_PATH_DISTANCE)
					break;
			}
		}
	}

	*ppClosestUnit = pBestUnit;
	*piClosestTurns = iMinTurns;
	return pBestUnit != NULL;
}



bool CvHomelandAI::GetBestUnitToReachTarget(CvPlot* pTarget, int iMaxTurns)
{
	AI_PERF_FORMAT("Homeland-GetBest-perf.csv", ("GetBestUnitToReachTarget, %d, %d, Turn %03d, %s", pTarget->getX(), pTarget->getY(), GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	UnitHandle pLoopUnit;
	MoveUnitsArray::iterator it;


	int iTargetX = pTarget->getX();
	int iTargetY = pTarget->getY();
	

	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		pLoopUnit = m_pPlayer->getUnit(it->GetID());
		if(pLoopUnit)
		{

			if(pLoopUnit->getDomainType() == DOMAIN_SEA && !pTarget->isWater() ||
				pLoopUnit->getDomainType() == DOMAIN_LAND && pTarget->isWater())
			{
				it->SetMovesToTarget(MAX_INT);
				continue;
			}


			if(!pLoopUnit->canMoveInto(*pTarget, CvUnit::MOVEFLAG_DESTINATION | CvUnit::MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE))
			{
				it->SetMovesToTarget(MAX_INT);
				continue;
			}

			int iPlotDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), iTargetX, iTargetY);
			it->SetMovesToTarget(iPlotDistance);
		}
	}


	for(it = m_CurrentMoveHighPriorityUnits.begin(); it != m_CurrentMoveHighPriorityUnits.end(); ++it)
	{
		pLoopUnit = m_pPlayer->getUnit(it->GetID());
		if(pLoopUnit)
		{

			if(pLoopUnit->getDomainType() == DOMAIN_SEA && !pTarget->isWater() ||
				pLoopUnit->getDomainType() == DOMAIN_LAND && pTarget->isWater())
			{
				it->SetMovesToTarget(MAX_INT);
				continue;
			}


			if(!pLoopUnit->canMoveInto(*pTarget, CvUnit::MOVEFLAG_DESTINATION | CvUnit::MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE))
			{
				it->SetMovesToTarget(MAX_INT);
				continue;
			}

			int iPlotDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), iTargetX, iTargetY);
			it->SetMovesToTarget(iPlotDistance);
		}
	}


	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());
	std::stable_sort(m_CurrentMoveHighPriorityUnits.begin(), m_CurrentMoveHighPriorityUnits.end());


	GetClosestUnitByTurnsToTarget(m_CurrentMoveUnits, pTarget, iMaxTurns, &m_CurrentBestMoveUnit, &m_iCurrentBestMoveUnitTurns);
	GetClosestUnitByTurnsToTarget(m_CurrentMoveHighPriorityUnits, pTarget, iMaxTurns, &m_CurrentBestMoveHighPriorityUnit, &m_iCurrentBestMoveHighPriorityUnitTurns);

	return m_CurrentBestMoveHighPriorityUnit != NULL || m_CurrentBestMoveUnit != NULL;
}


bool CvHomelandAI::MoveToEmptySpaceNearTarget(CvUnit* pUnit, CvPlot* pTarget, bool bLand)
{
	CvPlot* pLoopPlot;


	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(pTarget->getX(), pTarget->getY(), ((DirectionTypes)iI));
		if(pLoopPlot != NULL && pLoopPlot->isWater() != bLand)
		{

			if(!pLoopPlot->getBestDefender(m_pPlayer->GetID()))
			{

				if(!pLoopPlot->getBestDefender(NO_PLAYER, m_pPlayer->GetID()))
				{

					if(pUnit->GeneratePath(pLoopPlot))
					{

						pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pLoopPlot->getX(), pLoopPlot->getY());
						return true;
					}
				}
			}
		}
	}
	return false;
}


CvCity* CvHomelandAI::ChooseBestFreeWonderCity(BuildingTypes eWonder, UnitHandle pEngineer)
{
	CvCity* pBestCity = NULL;
	CvCity* pLoopCity;
	int iLoop;
	int iBestTurns = MAX_INT;
	int iTurns;

	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity->canConstruct(eWonder))
		{
			iTurns = TurnsToReachTarget(pEngineer, pLoopCity->plot(), true                );
			if(iTurns < iBestTurns)
			{
				pBestCity = pLoopCity;
				iBestTurns = iTurns;
			}
		}
	}

	return pBestCity;
}


CvPlot* CvHomelandAI::FindArchaeologistTarget(CvUnit *pUnit)
{
	CvPlot *pBestTarget = NULL;
	int iBestTurns = MAX_INT;


	std::vector<CvHomelandTarget>::iterator it;
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_TargetedAntiquitySites.begin(); it != m_TargetedAntiquitySites.end(); ++it)
#else
	for (it = m_TargetedAntiquitySites.begin(); it != m_TargetedAntiquitySites.end(); it++)
#endif
	{
		CvPlot* pTarget = GC.getMap().plot(it->GetTargetX(), it->GetTargetY());
		if (m_pPlayer->GetPlotDanger(*pTarget) == 0)
		{
			int iTurns = TurnsToReachTarget(pUnit, pTarget);

			if (iTurns < iBestTurns)
			{
				pBestTarget = pTarget;
				iBestTurns = iTurns;
			}
		}
	}


	if (pBestTarget)
	{
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
		for (it = m_TargetedAntiquitySites.begin(); it != m_TargetedAntiquitySites.end(); ++it)
#else
		for (it = m_TargetedAntiquitySites.begin(); it != m_TargetedAntiquitySites.end(); it++)
#endif
		{
			if (it->GetTargetX() == pBestTarget->getX() && it->GetTargetY() == pBestTarget->getY())
			{
				m_TargetedAntiquitySites.erase(it);
				break;
			}
		}
	}

	return pBestTarget;
}



void CvHomelandAI::LogHomelandMessage(CvString& strMsg)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp, szTemp2;
		CvString strPlayerName;
		FILogFile* pLog;

		strPlayerName = m_pPlayer->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";
		strOutBuf = strBaseString + strMsg;
		pLog->Msg(strOutBuf);
	}
}

void CvHomelandAI::LogPatrolMessage( CvString& strMsg, CvUnit* pPatrolUnit )
{
	if(!pPatrolUnit || !(GC.getLogging() && GC.getAILogging()))
	{
		return;
	}


	CvString strFileName = "PlayerHomelandAIPatrolLog.csv";
	FILogFile* pLog;
	pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);

	CvString strLog, strTemp;

	CvString strPlayerName;
	strPlayerName = m_pPlayer->getCivilizationShortDescription();
	strLog += strPlayerName;
	strLog += ",";

	strTemp.Format("%d,", GC.getGame().getGameTurn());
	strLog += strTemp;

	strTemp.Format("%s (%d),", pPatrolUnit->getUnitInfo().GetDescription(), pPatrolUnit->GetID());
	strLog += strTemp;

	strLog += strMsg;
	pLog->Msg(strLog);
}


void CvHomelandAI::UnitProcessed(int iID)
{
	CvUnit* pUnit;

	m_CurrentTurnUnits.remove(iID);
	pUnit = m_pPlayer->getUnit(iID);

	CvAssert(pUnit);
	if(!pUnit) return;

	pUnit->SetTurnProcessed(true);
}

bool CvHomelandAI::ExecuteWorkerMove(CvUnit* pUnit)
{
	const UINT ciDirectiveSize = 1;
	BuilderDirective aDirective[ ciDirectiveSize ];


	bool bHasDirective = m_pPlayer->GetBuilderTaskingAI()->EvaluateBuilder(pUnit, aDirective, ciDirectiveSize);
	if(bHasDirective)
	{
		switch(aDirective[0].m_eDirective)
		{
		case BuilderDirective::BUILD_IMPROVEMENT_ON_RESOURCE:
		case BuilderDirective::BUILD_IMPROVEMENT:
		case BuilderDirective::REPAIR:
		case BuilderDirective::BUILD_ROUTE:
		case BuilderDirective::CHOP:
		case BuilderDirective::REMOVE_ROAD:
		{
			CvPlot* pPlot = GC.getMap().plot(aDirective[0].m_sX, aDirective[0].m_sY);
			MissionTypes eMission = NO_MISSION;
			if(pUnit->getX() == aDirective[0].m_sX && pUnit->getY() == aDirective[0].m_sY)
			{
				eMission = CvTypes::getMISSION_BUILD();
			}
			else
			{
#ifdef AUI_HOMELAND_FIX_EXECUTE_WORKER_MOVE_MOVE_AND_BUILD
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), aDirective[0].m_sX, aDirective[0].m_sY, 0, false, false, MISSIONAI_BUILD, pPlot);
				if (pUnit->canMove())
				{
					eMission = CvTypes::getMISSION_BUILD();
				}
				else
				{
					eMission = CvTypes::getMISSION_MOVE_TO();
					pUnit->finishMoves();
					UnitProcessed(pUnit->GetID());
				}
#else
				eMission = CvTypes::getMISSION_MOVE_TO();
#endif
			}

			if(GC.getLogging() && GC.GetBuilderAILogging())
			{

				CvString strFileName = "BuilderTaskingLog.csv";
				FILogFile* pLog;
				pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);


				CvString strLog;
				CvString strTemp;

				CvString strPlayerName;
				strPlayerName = m_pPlayer->getCivilizationShortDescription();
				strLog += strPlayerName;
				strLog += ",";

				strTemp.Format("%d,", GC.getGame().getGameTurn());
				strLog += strTemp;

				strTemp.Format("%d,", pUnit->GetID());
				strLog += strTemp;

				switch(aDirective[0].m_eDirective)
				{
				case BuilderDirective::BUILD_IMPROVEMENT_ON_RESOURCE:
					strLog += "On resource,";
					break;
				case BuilderDirective::BUILD_IMPROVEMENT:
					strLog += "On plot,";
					break;
				case BuilderDirective::REPAIR:
					strLog += "Repairing,";
					break;
				case BuilderDirective::BUILD_ROUTE:
					strLog += "Building route,";
					break;
				case BuilderDirective::CHOP:
					strLog += "Removing resource for production,";
					break;
				case BuilderDirective::REMOVE_ROAD:
					strLog += "Removing road,";
					break;
				}

				if(eMission == CvTypes::getMISSION_BUILD())
				{
					if(aDirective[0].m_eDirective == BuilderDirective::REPAIR)
					{
						if(pPlot->IsImprovementPillaged())
						{
							strLog += "Repairing improvement";
						}
						else
						{
							strLog += "Repairing route";
						}
					}
					else if(aDirective[0].m_eDirective == BuilderDirective::BUILD_ROUTE)
					{
						strLog += "Building route,";
					}
					else if(aDirective[0].m_eDirective == BuilderDirective::BUILD_IMPROVEMENT || aDirective[0].m_eDirective == BuilderDirective::BUILD_IMPROVEMENT_ON_RESOURCE)
					{
						strLog += "Building improvement,";
					}
					else if(aDirective[0].m_eDirective == BuilderDirective::CHOP)
					{
						strLog += "Removing feature for production,";
					}
					else
					{
						strLog += "Removing road,";
					}
				}
				else
				{
					strLog += "Moving to location,";
				}

				pLog->Msg(strLog);
			}

			if(eMission == CvTypes::getMISSION_BUILD())
			{

				bool bPushMission = true;
				const MissionData* pkMissionData = pUnit->GetHeadMissionData();
				if(pkMissionData != NULL)
				{
					if(pkMissionData->eMissionType == eMission && pkMissionData->iData1 == aDirective[0].m_eBuild)
					{
						bPushMission = false;
					}
				}

				if(bPushMission)
				{
					pUnit->PushMission(CvTypes::getMISSION_BUILD(), aDirective[0].m_eBuild, -1, 0, (pUnit->GetLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pPlot);
				}

				CvAssertMsg(!pUnit->ReadyToMove(), "Worker did not do their mission this turn. Could cause game to hang.");
				if(pUnit->ReadyToMove())
				{
					pUnit->finishMoves();
				}
				UnitProcessed(pUnit->GetID());
			}
#ifndef AUI_HOMELAND_FIX_EXECUTE_WORKER_MOVE_MOVE_AND_BUILD
			else
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), aDirective[0].m_sX, aDirective[0].m_sY, 0, false, false, MISSIONAI_BUILD, pPlot);
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());
			}
#endif

			return true;
		}
		break;
		}
	}
	else
	{
		if(GC.getLogging() && GC.GetBuilderAILogging())
		{
			CvString strFileName = "BuilderTaskingLog.csv";
			FILogFile* pLog;
			pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);
			pLog->Msg("builder has no directive");
		}
	}

	return false;
}

bool CvHomelandAI::ExecuteCultureBlast(CvUnit* pUnit)
{
	if (pUnit->canGivePolicies(pUnit->plot()))
	{
		pUnit->PushMission(CvTypes::getMISSION_GIVE_POLICIES());
		UnitProcessed(pUnit->GetID());

		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			CvString strTemp;
			strTemp = pUnit->getUnitInfo().GetDescription();
			strLogString.Format("%s blasted culture with unit at, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
			LogHomelandMessage(strLogString);
		}
	}
	else
	{
		MoveCivilianToSafety(pUnit);
	}

	return true;
}

bool CvHomelandAI::ExecuteGoldenAgeMove(CvUnit* pUnit)
{
	if (pUnit->canGoldenAge(pUnit->plot()))
	{
		pUnit->PushMission(CvTypes::getMISSION_GOLDEN_AGE());
		UnitProcessed(pUnit->GetID());

		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			CvString strTemp;
			strTemp = pUnit->getUnitInfo().GetDescription();
			strLogString.Format("%s started golden age with unit at, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
			LogHomelandMessage(strLogString);
		}
	}
	else
	{
		MoveCivilianToSafety(pUnit);
	}

	return true;
}

bool CvHomelandAI::IsValidExplorerEndTurnPlot(const CvUnit* pUnit, CvPlot* pPlot) const
{
	if(pUnit->atPlot(*pPlot))
	{
		return false;
	}

	if(!pPlot->isRevealed(pUnit->getTeam()))
	{
		return false;
	}

	DomainTypes eDomain = pUnit->getDomainType();

	if(pPlot->area() != pUnit->area())
	{
		if(!pUnit->CanEverEmbark())
		{
			if(!(eDomain == DOMAIN_SEA && pPlot->isWater()))
			{
				return false;
			}
		}
	}


	CvCity* pCity = pPlot->getPlotCity();
	if(pCity && pCity->getOwner() != pUnit->getOwner())
	{
		return false;
	}

	if(!pUnit->canMoveInto(*pPlot, CvUnit::MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE))
	{
		return false;
	}

	return true;
}


bool CvHomelandAI::ExecuteSpecialExploreMove(CvUnit* pUnit, CvPlot* pTargetPlot)
{
	bool bCanFindPath = GC.getPathFinder().GenerateUnitPath(pUnit, pUnit->getX(), pUnit->getY(), pTargetPlot->getX(), pTargetPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER           , true          );
	if(bCanFindPath)
	{
		CvPlot* pPlot = GC.getPathFinder().GetPathEndTurnPlot();
		if(pPlot)
		{
			CvAssert(!pUnit->atPlot(*pPlot));
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("UnitID: %d Moving to script explore target, X: %d, Y: %d, from X: %d Y: %d", pUnit->GetID(), pPlot->getX(), pPlot->getY(), pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pPlot->getX(), pPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER, false, false, MISSIONAI_EXPLORE, pPlot);
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());

			return true;
		}
	}
	return false;
}


CvString CvHomelandAI::GetLogFileName(CvString& playerName) const
{
	CvString strLogName;


	if(GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "PlayerHomelandAILog_" + playerName + ".csv";
	}
	else
	{
		strLogName = "PlayerHomelandAILog.csv";
	}

	return strLogName;
}


void CvHomelandAI::ClearCurrentMoveUnits()
{
	m_CurrentMoveUnits.clear();
	m_CurrentBestMoveUnit = NULL;
	m_iCurrentBestMoveUnitTurns = MAX_INT;
}


void CvHomelandAI::ClearCurrentMoveHighPriorityUnits()
{
	m_CurrentMoveHighPriorityUnits.clear();
	m_CurrentBestMoveHighPriorityUnit = NULL;
	m_iCurrentBestMoveHighPriorityUnitTurns = MAX_INT;
}

CvUnit* m_CurrentBestHighPriorityMoveUnit;
int m_iCurrentBestHighPriorityMoveUnitTurns;




bool HomelandAIHelpers::CvHomelandUnitAuxIntSort(CvHomelandUnit obj1, CvHomelandUnit obj2)
{
	return obj1.GetAuxIntData() < obj2.GetAuxIntData();
}

bool HomelandAIHelpers::CvHomelandUnitAuxIntReverseSort(CvHomelandUnit obj1, CvHomelandUnit obj2)
{
	return obj1.GetAuxIntData() > obj2.GetAuxIntData();
}
