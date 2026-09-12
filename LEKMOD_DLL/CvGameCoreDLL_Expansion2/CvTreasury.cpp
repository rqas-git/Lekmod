







#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreUtils.h"
#include "ICvDLLUserInterface.h"

#include "LintFree.h"






CvTreasury::CvTreasury():
	m_iGold(0),
	m_iGoldPerTurnFromDiplomacy(0),
	m_iExpensePerTurnUnitMaintenance(0),
	m_iExpensePerTurnUnitSupply(0),
	m_iCityConnectionGoldTimes100(0),
	m_iCityConnectionTradeRouteGoldModifier(0),
	m_iCityConnectionTradeRouteGoldChange(0),
	m_iBaseBuildingGoldMaintenance(0),
	m_iBaseImprovementGoldMaintenance(0),
	m_iLifetimeGrossGoldIncome(0),
#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
	m_iCurrentGoldChange(0),
	m_iCurrentGrossGoldChange(0),
#endif
	m_pPlayer(NULL)
{

}


CvTreasury::~CvTreasury()
{
}


void CvTreasury::Init(CvPlayer* pPlayer)
{
	m_pPlayer = pPlayer;

	m_iGold = 0;
	m_iGoldPerTurnFromDiplomacy = 0;
	m_iExpensePerTurnUnitMaintenance = 0;
	m_iExpensePerTurnUnitSupply = 0;
	m_iCityConnectionGoldTimes100 = 0;
	m_iCityConnectionTradeRouteGoldModifier = 0;
	m_iCityConnectionTradeRouteGoldChange = 0;
	m_iBaseBuildingGoldMaintenance = 0;
	m_iBaseImprovementGoldMaintenance = 0;
	m_iLifetimeGrossGoldIncome = 0;
#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
	m_iCurrentGoldChange = 0;
	m_iCurrentGrossGoldChange = 0;
#endif

	m_GoldBalanceForTurnTimes100.clear();
	m_GoldBalanceForTurnTimes100.reserve(750);
	m_GoldChangeForTurnTimes100.clear();
	m_GoldChangeForTurnTimes100.reserve(750);
}


void CvTreasury::Uninit()
{

}

#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
int CvTreasury::getGoldT100ForThisTurn() const
{
	return m_iCurrentGoldChange;
}

int CvTreasury::getGrossGoldForThisTurn() const
{
	return m_iCurrentGrossGoldChange;
}

void CvTreasury::cacheGoldT100ForThisTurn()
{
	m_iCurrentGoldChange = m_pPlayer->calculateGoldRateTimes100();
	m_iCurrentGrossGoldChange = CalculateGrossGold();
}
#endif


void CvTreasury::DoGold()
{
#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
	int iGoldChange = getGoldT100ForThisTurn();
#else
	int iGoldChange;

	iGoldChange = m_pPlayer->calculateGoldRateTimes100();
#endif

	int iGoldAfterThisTurn = iGoldChange + GetGoldTimes100();
	if(iGoldAfterThisTurn < 0)
	{
		SetGold(0);

		if(iGoldAfterThisTurn <=        GC.getDEFICIT_UNIT_DISBANDING_THRESHOLD() * 100)
			m_pPlayer->DoDeficit();
	}
	else
	{
		ChangeGoldTimes100(iGoldChange);
	}


#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
	int iGrossGoldChange = getGrossGoldForThisTurn();
#else
	int iGrossGoldChange = CalculateGrossGold();
#endif
	if(iGrossGoldChange > 0)
	{
		m_iLifetimeGrossGoldIncome += iGrossGoldChange;
	}

	FAssertMsg(m_GoldBalanceForTurnTimes100.size() <= (unsigned int) GC.getGame().getGameTurn(), "History of Gold Balances corrupted");
	if(m_GoldBalanceForTurnTimes100.size() < (unsigned int) GC.getGame().getGameTurn())
	{
		m_GoldBalanceForTurnTimes100.push_back(GetGoldTimes100());
	}

	FAssertMsg(m_GoldChangeForTurnTimes100.size() <= (unsigned int) GC.getGame().getGameTurn(), "History of Gold Changes corrupted");
	if(m_GoldChangeForTurnTimes100.size() < (unsigned int) GC.getGame().getGameTurn())
	{
		m_GoldChangeForTurnTimes100.push_back(iGoldChange);
	}

	if (m_pPlayer->isHuman() && !GC.getGame().isGameMultiPlayer())
	{
		int iGoldDelta = (GetGoldFromCitiesTimes100(false) - GetGoldFromCitiesTimes100(true)) / 100;
		if (iGoldDelta >= 200)
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_XP2_32);
		}
	}
}


int CvTreasury::GetGold() const
{
	return m_iGold / 100;
}


void CvTreasury::SetGold(int iNewValue)
{
	SetGoldTimes100(iNewValue * 100);
}


void CvTreasury::ChangeGold(int iChange)
{
	ChangeGoldTimes100(iChange*100);
}


int CvTreasury::GetGoldTimes100() const
{
	return m_iGold;
}


void CvTreasury::SetGoldTimes100(int iNewValue)
{

	if(GetGoldTimes100() != iNewValue)
	{
		if(iNewValue < 0)
		{
			CvAssertMsg(false, "GAMEPLAY: Player is being set to a negative Gold value. Please send Jon this with your last 5 autosaves.");
		}

		m_iGold = iNewValue;

		if(m_pPlayer->GetID() == GC.getGame().getActivePlayer())
		{












			GC.GetEngineUserInterface()->setDirty(MiscButtons_DIRTY_BIT, true);
			GC.GetEngineUserInterface()->setDirty(SelectionButtons_DIRTY_BIT, true);
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


void CvTreasury::ChangeGoldTimes100(int iChange)
{
	SetGoldTimes100(GetGoldTimes100() + iChange);
}


int CvTreasury::GetGoldFromCities() const
{
	return GetGoldFromCitiesTimes100() / 100;
}


int CvTreasury::GetGoldFromCitiesTimes100(bool bExcludeTradeRoutes) const
{
#if !defined(STANDARDIZE_YIELDS)
	int iGold = 0;

	CvCity* pLoopCity;

	int iLoop;
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		iGold += pLoopCity->getYieldRateTimes100(YIELD_GOLD, bExcludeTradeRoutes);
	}

	return iGold;
#else
	return m_pPlayer->getYieldFromCitiesTimes100(YIELD_GOLD, bExcludeTradeRoutes);
#endif
}


int CvTreasury::GetGoldPerTurnFromDiplomacy() const
{
#if !defined(STANDARDIZE_YIELDS)
	return m_iGoldPerTurnFromDiplomacy;
#else
	return m_pPlayer->GetYieldPerTurnFromMisc(YIELD_GOLD);
#endif
}


void CvTreasury::SetGoldPerTurnFromDiplomacy(int iValue)
{
#if !defined(STANDARDIZE_YIELDS)
	m_iGoldPerTurnFromDiplomacy = iValue;
#else
	m_pPlayer->ChangeYieldPerTurnFromMisc(YIELD_GOLD, iValue - GetGoldPerTurnFromDiplomacy());
#endif
}


void CvTreasury::ChangeGoldPerTurnFromDiplomacy(int iChange)
{
	SetGoldPerTurnFromDiplomacy(GetGoldPerTurnFromDiplomacy() + iChange);
}


int CvTreasury::GetCityConnectionRouteGoldTimes100(CvCity* pNonCapitalCity) const
{
	CvCity* pCapitalCity = m_pPlayer->getCapitalCity();
	if(!pNonCapitalCity || pNonCapitalCity == pCapitalCity || pCapitalCity == NULL)
	{
		return 0;
	}

	int iGold = 0;

	int iTradeRouteBaseGold =         GC.getTRADE_ROUTE_BASE_GOLD();
	int iTradeRouteCapitalGoldMultiplier =       GC.getTRADE_ROUTE_CAPITAL_POP_GOLD_MULTIPLIER();
	int iTradeRouteCityGoldMultiplier =         GC.getTRADE_ROUTE_CITY_POP_GOLD_MULTIPLIER();

	iGold += iTradeRouteBaseGold;
	iGold += (pCapitalCity->getPopulation() * iTradeRouteCapitalGoldMultiplier);
	iGold += (pNonCapitalCity->getPopulation() * iTradeRouteCityGoldMultiplier);
	iGold += GetCityConnectionTradeRouteGoldChange() * 100;

	if(GetCityConnectionTradeRouteGoldModifier() != 0)
	{
		iGold *= (100 + GetCityConnectionTradeRouteGoldModifier());
		iGold /= 100;
	}

	return iGold;
}



int CvTreasury::GetCityConnectionGold() const
{
	return m_iCityConnectionGoldTimes100 / 100;
}


int CvTreasury::GetCityConnectionGoldTimes100() const
{
	return m_iCityConnectionGoldTimes100;
}


void CvTreasury::DoUpdateCityConnectionGold()
{
	int iNumGold = 0;

	CvCity* pCapitalCity = m_pPlayer->getCapitalCity();


	if(pCapitalCity != NULL && m_pPlayer->getNumCities() > 1)
	{
		CvCity* pLoopCity;

		int iLoop;
		for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			if(pLoopCity != pCapitalCity)
			{
				if(HasCityConnectionRouteBetweenCities(pCapitalCity, pLoopCity))
				{
					iNumGold += GetCityConnectionRouteGoldTimes100(pLoopCity);
				}
			}
		}
	}

	m_iCityConnectionGoldTimes100 = iNumGold;
}



int CvTreasury::GetCityConnectionTradeRouteGoldModifier() const
{
	return m_iCityConnectionTradeRouteGoldModifier;
}


void CvTreasury::ChangeCityConnectionTradeRouteGoldModifier(int iChange)
{
	if(iChange != 0)
	{
		m_iCityConnectionTradeRouteGoldModifier += iChange;

		DoUpdateCityConnectionGold();
	}
}


int CvTreasury::GetCityConnectionTradeRouteGoldChange() const
{
	return m_iCityConnectionTradeRouteGoldChange;
}


void CvTreasury::ChangeCityConnectionTradeRouteGoldChange(int iChange)
{
	if(iChange != 0)
	{
		m_iCityConnectionTradeRouteGoldChange += iChange;

		DoUpdateCityConnectionGold();
	}
}


bool CvTreasury::HasCityConnectionRouteBetweenCities(CvCity* pFirstCity, CvCity* pSecondCity, bool bBestRoute) const
{
	CvCityConnections* pCityConnections = m_pPlayer->GetCityConnections();
	FASSERT(pCityConnections, "m_pCityConnections is null");
	if(!pCityConnections)
	{

		return FALSE;
	}

	FASSERT(pFirstCity, "pFirstCity is null. Invalid!");
	FASSERT(pSecondCity, "pSecondCity is null. Invalid!");
	if(!pFirstCity || !pSecondCity)
	{

		return FALSE;
	}

	FASSERT(pFirstCity != pSecondCity, "Cities are the same and are by default connected?");
	if(pFirstCity == pSecondCity)
	{

		return FALSE;
	}

	int iFirstCityIndex = -1;
	int iSecondCityIndex = -1;
	for(uint ui = 0; ui < pCityConnections->m_aiCityPlotIDs.size(); ui++)
	{
		if(pFirstCity->plot()->GetPlotIndex() == pCityConnections->m_aiCityPlotIDs[ui])
		{
			iFirstCityIndex = ui;
		}

		if(pSecondCity->plot()->GetPlotIndex() == pCityConnections->m_aiCityPlotIDs[ui])
		{
			iSecondCityIndex = ui;
		}

		if(iFirstCityIndex >= 0 && iSecondCityIndex >= 0)
		{
			break;
		}
	}

	if(iFirstCityIndex < 0 || iSecondCityIndex < 0)
	{

		return FALSE;
	}

	CvCityConnections::RouteInfo* pRouteInfo = pCityConnections->GetRouteInfo(iFirstCityIndex, iSecondCityIndex);
	if(pRouteInfo)
	{
		if(bBestRoute)
		{
			return pRouteInfo->m_cRouteState & CvCityConnections::HAS_BEST_ROUTE;
		}
		else
		{
			return pRouteInfo->m_cRouteState & CvCityConnections::HAS_ANY_ROUTE;
		}
	}

	return FALSE;
}


int CvTreasury::GetGoldPerTurnFromTradeRoutes() const
{

	return 0;

}


int CvTreasury::GetGoldPerTurnFromTradeRoutesTimes100() const
{
	return 0;

}


int CvTreasury::GetGoldPerTurnFromTraits() const
{
	return m_pPlayer->getYieldFromTraitsTimes100(YIELD_GOLD);
}


int CvTreasury::GetGoldPerTurnFromReligion() const
{
#if !defined(STANDARDIZE_YIELDS)
	int iGoldFromReligion = 0;

	CvGameReligions* pReligions = GC.getGame().GetGameReligions();


	ReligionTypes eFoundedReligion = pReligions->GetFounderBenefitsReligion(m_pPlayer->GetID());
	if(eFoundedReligion != NO_RELIGION)
	{
		const CvReligion* pReligion = pReligions->GetReligion(eFoundedReligion, NO_PLAYER);
		if(pReligion)
		{
			int iGoldPerFollowingCity = pReligion->m_Beliefs.GetGoldPerFollowingCity();
			iGoldFromReligion += (pReligions->GetNumCitiesFollowing(eFoundedReligion) * iGoldPerFollowingCity);

			int iGoldPerXFollowers = pReligion->m_Beliefs.GetGoldPerXFollowers();
			if(iGoldPerXFollowers > 0)
			{
				iGoldFromReligion += (pReligions->GetNumFollowers(eFoundedReligion) / iGoldPerXFollowers);
			}
		}
	}

	return iGoldFromReligion;
#else
	return m_pPlayer->getYieldFromReligionTimes100(YIELD_GOLD, m_pPlayer->getYieldTimes100(YIELD_GOLD, true)) / 100;
#endif
}


int CvTreasury::CalculateGrossGold()
{
	return CalculateGrossGoldTimes100() / 100;
}


int CvTreasury::CalculateGrossGoldTimes100()
{
	int iNetGold;


	iNetGold = GetGoldFromCitiesTimes100();


	iNetGold += GetGoldPerTurnFromDiplomacy() * 100;


	iNetGold += GetCityConnectionGoldTimes100();


	iNetGold += GetGoldPerTurnFromReligion() * 100;


	iNetGold += GetGoldPerTurnFromTraits();

	return iNetGold;
}


int CvTreasury::GetLifetimeGrossGold()
{
	return m_iLifetimeGrossGoldIncome;
}


int CvTreasury::CalculateBaseNetGold()
{
	return CalculateBaseNetGoldTimes100() / 100;
}


int CvTreasury::CalculateBaseNetGoldTimes100()
{
	int iNetGold = CalculateGrossGoldTimes100();


	iNetGold -= CalculateInflatedCosts() * 100;

	return iNetGold;
}



int CvTreasury::CalculateUnitCost(int& iFreeUnits, int& iPaidUnits, int& iBaseUnitCost, int& iExtraCost)
{

	if(m_pPlayer->getNumCities() == 0)
	{
		return 0;
	}

	int iSupport = 0;

	CvHandicapInfo& playerHandicap = m_pPlayer->getHandicapInfo();
	iFreeUnits = playerHandicap.getGoldFreeUnits();


	iFreeUnits += m_pPlayer->GetNumMaintenanceFreeUnits();
	iFreeUnits += m_pPlayer->getBaseFreeUnits();

	iPaidUnits = max(0, m_pPlayer->getNumUnits() - iFreeUnits);

	iBaseUnitCost = iPaidUnits * m_pPlayer->getGoldPerUnitTimes100();


	int iLandUnitMod = m_pPlayer->GetPlayerTraits()->GetLandUnitMaintenanceModifier();
	if(iLandUnitMod != 0)
	{
		int iLandUnits = m_pPlayer->GetNumUnitsWithDomain(DOMAIN_LAND, true                  );
		int iFreeLandUnits = m_pPlayer->GetNumMaintenanceFreeUnits(DOMAIN_LAND, true);
		int iPaidLandUnits = iLandUnits - iFreeLandUnits;
		iBaseUnitCost += (iLandUnitMod * iPaidLandUnits * m_pPlayer->getGoldPerUnitTimes100()) / 100;
	}


	int iNavalUnitMod = m_pPlayer->GetPlayerTraits()->GetNavalUnitMaintenanceModifier();
	if(iNavalUnitMod != 0)
	{
		int iNavalUnits = m_pPlayer->GetNumUnitsWithDomain(DOMAIN_SEA, true                  );
		int iFreeNavalUnits = m_pPlayer->GetNumMaintenanceFreeUnits(DOMAIN_SEA, true);
		int iPaidNavalUnits = iNavalUnits - iFreeNavalUnits;
		iBaseUnitCost += (iNavalUnitMod * iPaidNavalUnits * m_pPlayer->getGoldPerUnitTimes100()) / 100;
	}


#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
#else
	for(int iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
#endif
	{
		const UnitCombatTypes eUnitCombatClass = static_cast<UnitCombatTypes>(iI);
		CvBaseInfo* pkUnitCombatClassInfo = GC.getUnitCombatClassInfo(eUnitCombatClass);
		if(pkUnitCombatClassInfo)
		{
			int iModifier = m_pPlayer->GetPlayerTraits()->GetMaintenanceModifierUnitCombat(eUnitCombatClass);
			if (iModifier != 0)
			{
				int iNumUnits = m_pPlayer->GetNumUnitsWithUnitCombat(eUnitCombatClass);
				int iCost = iNumUnits * m_pPlayer->getGoldPerUnitTimes100(); 
				int iModifiedCost = iNumUnits * m_pPlayer->getGoldPerUnitTimes100() * (100 + iModifier) / 100; 
				

				iBaseUnitCost += (iModifiedCost - iCost);
			}
		}
	}

	iExtraCost = m_pPlayer->getExtraUnitCost() * 100;

	iSupport = iBaseUnitCost + iExtraCost;


	double fGameProgressFactor = double(GC.getGame().getElapsedGameTurns()) / GC.getGame().getDefaultEstimateEndTurn();


	double fMultiplyFactor = 1.0 + (fGameProgressFactor*       GC.getUNIT_MAINTENANCE_GAME_MULTIPLIER());

	double fExponentialFactor = 1.0 + (fGameProgressFactor /       GC.getUNIT_MAINTENANCE_GAME_EXPONENT_DIVISOR());

	double fTempCost = fMultiplyFactor * iSupport;
	fTempCost /= 100;

	double dFinalCost = pow(fTempCost, fExponentialFactor);


	if(m_pPlayer->GetUnitGoldMaintenanceMod() != 0)
	{
		dFinalCost *= (100 + m_pPlayer->GetUnitGoldMaintenanceMod());
		dFinalCost /= 100;
	}


	if(m_pPlayer->isHuman())
	{
		dFinalCost *= playerHandicap.getUnitCostPercent();
		dFinalCost /= 100;
	}

	else if(!m_pPlayer->IsAITeammateOfHuman())
	{
		dFinalCost *= GC.getGame().getHandicapInfo().getAIUnitCostPercent();
		dFinalCost /= 100;
	}



	return std::max(0, int(dFinalCost));
}


int CvTreasury::CalculateUnitSupply(int& iPaidUnits, int& iBaseSupplyCost)
{
	int iSupply;

	iPaidUnits = std::max(0, (m_pPlayer->getNumOutsideUnits() -       GC.getINITIAL_FREE_OUTSIDE_UNITS()));


	iBaseSupplyCost = iPaidUnits*       GC.getINITIAL_OUTSIDE_UNIT_GOLD_PERCENT();
	iBaseSupplyCost /= 100;

	iSupply = iBaseSupplyCost;

	CvHandicapInfo& playerHandicap = m_pPlayer->getHandicapInfo();
	iSupply *= playerHandicap.getUnitCostPercent();
	iSupply /= 100;

	if(!m_pPlayer->isHuman() && !m_pPlayer->IsAITeammateOfHuman() && !m_pPlayer->isBarbarian())
	{



		iSupply *= std::max(0, ((GC.getGame().getHandicapInfo().getAIPerEraModifier() * m_pPlayer->GetCurrentEra()) + 100));
		iSupply /= 100;
	}


	double fGameProgressFactor = float(GC.getGame().getElapsedGameTurns()) / GC.getGame().getEstimateEndTurn();


	double fMultiplyFactor = 1.0 + (fGameProgressFactor*       GC.getUNIT_MAINTENANCE_GAME_MULTIPLIER());

	double fExponentialFactor = 1.0 + (fGameProgressFactor /       GC.getUNIT_MAINTENANCE_GAME_EXPONENT_DIVISOR());

	double fTempCost = fMultiplyFactor * iSupply;
	int iFinalCost = (int) pow(fTempCost, fExponentialFactor);


	if(m_pPlayer->GetUnitSupplyMod() != 0)
	{
		iFinalCost *= (100 + m_pPlayer->GetUnitSupplyMod());
		iFinalCost /= 100;
	}

	CvAssert(iFinalCost >= 0);

	return iFinalCost;
}


int CvTreasury::CalculatePreInflatedCosts()
{
	int iFreeUnits;
	int iPaidUnits;
	int iBaseUnitCost;
	int iExtraCost;
	int iBaseSupplyCost;

	m_iExpensePerTurnUnitMaintenance = CalculateUnitCost(iFreeUnits, iPaidUnits, iBaseUnitCost, iExtraCost);
	m_iExpensePerTurnUnitSupply = CalculateUnitSupply(iPaidUnits, iBaseSupplyCost);

	int iTotalCosts = 0;

	iTotalCosts += m_iExpensePerTurnUnitMaintenance;
	iTotalCosts += m_iExpensePerTurnUnitSupply;
	iTotalCosts += GetBuildingGoldMaintenance();
	iTotalCosts += GetImprovementGoldMaintenance();

	return iTotalCosts;
}


int CvTreasury::CalculateInflationRate()
{
	CvGame& kGame = GC.getGame();

	CvHandicapInfo& playerHandicap = m_pPlayer->getHandicapInfo();
	CvHandicapInfo& gameHandicap = kGame.getHandicapInfo();
	CvGameSpeedInfo& gameSpeedInfo = kGame.getGameSpeedInfo();

	int iTurns = ((kGame.getGameTurn() + kGame.getElapsedGameTurns()) / 2);
	iTurns += gameSpeedInfo.getInflationOffset();

	if(iTurns <= 0)
	{
		return 0;
	}

	int iInflationPerTurnTimes10000 = gameSpeedInfo.getInflationPercent();
	iInflationPerTurnTimes10000 *= playerHandicap.getInflationPercent();
	iInflationPerTurnTimes10000 /= 100;

	int iModifier = 0;

	if(!m_pPlayer->isHuman() && !m_pPlayer->isBarbarian())
	{
		int iAIModifier = gameHandicap.getAIInflationPercent();
		iAIModifier *= std::max(0, ((gameHandicap.getAIPerEraModifier() * m_pPlayer->GetCurrentEra()) + 100));
		iAIModifier /= 100;

		iModifier += iAIModifier - 100;
	}

	iInflationPerTurnTimes10000 *= std::max(0, 100 + iModifier);
	iInflationPerTurnTimes10000 /= 100;


	int iRatePercent = (iTurns * iInflationPerTurnTimes10000) / 100;
	iRatePercent += (iTurns * (iTurns - 1) * iInflationPerTurnTimes10000 * iInflationPerTurnTimes10000) / 2000000;

	CvAssert(iRatePercent >= 0);

	return iRatePercent;
}


int CvTreasury::CalculateInflatedCosts()
{
	int iCosts = CalculatePreInflatedCosts();




	return iCosts;
}


int CvTreasury::GetBuildingGoldMaintenance() const
{
	int iMaintenance = GetBaseBuildingGoldMaintenance();


	iMaintenance *= (100 + m_pPlayer->GetBuildingGoldMaintenanceMod());
	iMaintenance /= 100;


	CvHandicapInfo& playerHandicap = m_pPlayer->getHandicapInfo();




	if(m_pPlayer->isHuman())
	{
		iMaintenance *= playerHandicap.getBuildingCostPercent();
		iMaintenance /= 100;
	}

	else if(!m_pPlayer->IsAITeammateOfHuman())
	{
		iMaintenance *= GC.getGame().getHandicapInfo().getAIBuildingCostPercent();
		iMaintenance /= 100;
	}


	iMaintenance *= GC.getGame().getStartEraInfo().getBuildingMaintenancePercent();
	iMaintenance /= 100;

	return iMaintenance;
}


int CvTreasury::GetBaseBuildingGoldMaintenance() const
{
	return m_iBaseBuildingGoldMaintenance;
}


void CvTreasury::SetBaseBuildingGoldMaintenance(int iValue)
{
	m_iBaseBuildingGoldMaintenance = iValue;

	if(m_iBaseBuildingGoldMaintenance < 0)
		m_iBaseBuildingGoldMaintenance = 0;

	CvAssertMsg(m_iBaseBuildingGoldMaintenance >= 0, "Building Maintenance is negative somehow. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
}


void CvTreasury::ChangeBaseBuildingGoldMaintenance(int iChange)
{
	SetBaseBuildingGoldMaintenance(GetBaseBuildingGoldMaintenance() + iChange);
}


int CvTreasury::GetImprovementGoldMaintenance() const
{
	int iMaintenance = m_iBaseImprovementGoldMaintenance;


	iMaintenance *= (100 + m_pPlayer->GetRouteGoldMaintenanceMod());
	iMaintenance /= 100;


	iMaintenance *= m_pPlayer->getHandicapInfo().getRouteCostPercent();
	iMaintenance /= 100;

	return iMaintenance;
}


int CvTreasury::GetBaseImprovementGoldMaintenance() const
{
	return m_iBaseImprovementGoldMaintenance;
}


void CvTreasury::SetBaseImprovementGoldMaintenance(int iValue)
{
	if(GetBaseImprovementGoldMaintenance() != iValue)
	{
		m_iBaseImprovementGoldMaintenance = iValue;

		CvAssertMsg(m_iBaseImprovementGoldMaintenance >= 0, "Improvement Maintenance is negative somehow. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		if(m_iBaseImprovementGoldMaintenance < 0)
			m_iBaseImprovementGoldMaintenance = 0;

		if(m_pPlayer->GetID() == GC.getGame().getActivePlayer())
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	}
}


void CvTreasury::ChangeBaseImprovementGoldMaintenance(int iChange)
{
	SetBaseImprovementGoldMaintenance(GetBaseImprovementGoldMaintenance() + iChange);
}


double CvTreasury::AverageIncome(int iTurns)
{
	CvAssertMsg(iTurns > 0, "Invalid number of turns parameter");

	if(m_GoldChangeForTurnTimes100.size() > 0)
	{
		int iSamples = 0;
		int iIndex = m_GoldChangeForTurnTimes100.size() - 1;
		int iTotal = 0;

		while(iSamples < iTurns)
		{
			iTotal += m_GoldChangeForTurnTimes100[iIndex];
			iSamples++;
		}

		return ((double)iTotal / (double)iSamples / 100);
	}

	return 0;
}

void CvTreasury::LogExpenditure(CvString strExpenditure, int iAmount, int iColumn)
{
	if(!(GC.getLogging() && GC.getAILogging()))
	{
		return;
	}


	if(m_pPlayer->isMinorCiv())
	{
		return;
	}

	static bool bFirstRun = true;
	bool bBuildHeader = false;
	CvString strHeader;
	if(bFirstRun)
	{
		bFirstRun = false;
		bBuildHeader = true;
	}

	CvString strLog;


	CvString strPlayerName;
	strPlayerName = m_pPlayer->getCivilizationShortDescription();
	CvString strLogName;


	if(GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "ExpenditureLog_" + strPlayerName + ".csv";
	}
	else
	{
		strLogName = "ExpenditureLog.csv";
	}


	FILogFile* pLog;
	pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

	CvString str;


	TreasuryHelpers::AppendToLog(strHeader, strLog, "Civ Name", strPlayerName);


	TreasuryHelpers::AppendToLog(strHeader, strLog, "Turn", GC.getGame().getGameTurn());


	TreasuryHelpers::AppendToLog(strHeader, strLog, "Treasury Before Purchase", m_pPlayer->GetTreasury()->GetGold());
	

	if (iColumn == 1){
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Cost of Plot:", iAmount);
	}
	else{
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Cost of Plot:", 0);
	}
	

	if (iColumn == 2){
		TreasuryHelpers::AppendToLog(strHeader, strLog, "City Bought:", strExpenditure);
		TreasuryHelpers::AppendToLog(strHeader, strLog, "City Spent:", iAmount);
	}
	else{
		TreasuryHelpers::AppendToLog(strHeader, strLog, "City Bought:", "");
		TreasuryHelpers::AppendToLog(strHeader, strLog, "City Spent:", 0);
	}
	

	if (iColumn == 3){
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Unit Upgraded:", strExpenditure);
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Spent on Upgrade:", iAmount);
	}
	else{
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Unit Upgrade:", "");
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Spent on Upgrade:", 0);
	}


	if (iColumn == 4){
		TreasuryHelpers::AppendToLog(strHeader, strLog, "City State Gold Gifted:", strExpenditure);
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Gold Gift Amount:", iAmount);
	}
	else{
		TreasuryHelpers::AppendToLog(strHeader, strLog, "City State Gold Gifted:", "");
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Gold Gift Amount:", 0);
	}
	

	if (iColumn == 5){
		TreasuryHelpers::AppendToLog(strHeader, strLog, "City State Improved:", strExpenditure);
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Gold for Improvement Amount:", iAmount);
	}
	else{
		TreasuryHelpers::AppendToLog(strHeader, strLog, "City State Improved:", "");
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Gold for Improvement Amount:", 0);
	}


	if (iColumn == 6){
		TreasuryHelpers::AppendToLog(strHeader, strLog, "City State Bought:", strExpenditure);
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Bought for:", iAmount);
	}
	else{
		TreasuryHelpers::AppendToLog(strHeader, strLog, "City State Bought:", "");
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Bought for:", 0);
	}
	

	if (iColumn == 7){
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Emergency Unit:", strExpenditure);
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Emergency Unit Cost:", iAmount);
	}
	else{
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Emergency Unit:", "");
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Emergency Unit Cost:", 0);
	}
	

	if (iColumn == 8){
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Emergency Building:", strExpenditure);
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Emergency Building Cost:", iAmount);
	}
	else{
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Emergency Building:", "");
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Emergency Building Cost:", 0);
	}


	if (iColumn == 9){
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Research Agreement with:", strExpenditure);
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Research Agreement Cost:", iAmount);
	}
	else{
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Research Agreement with:", "");
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Research Agreement Cost:", 0);
	}

	if(bBuildHeader)
	{
		pLog->Msg(strHeader);
	}
	pLog->Msg(strLog);
}


void CvTreasury::Read(FDataStream& kStream)
{

	uint uiVersion;

	kStream >> uiVersion;

	kStream >> m_iGold;
	kStream >> m_iGoldPerTurnFromDiplomacy;
	kStream >> m_iExpensePerTurnUnitMaintenance;
	kStream >> m_iExpensePerTurnUnitSupply;
	kStream >> m_iCityConnectionGoldTimes100;
	kStream >> m_iCityConnectionTradeRouteGoldModifier;
	kStream >> m_iCityConnectionTradeRouteGoldChange;
	kStream >> m_iBaseBuildingGoldMaintenance;
	kStream >> m_iBaseImprovementGoldMaintenance;
	kStream >> m_GoldBalanceForTurnTimes100;
	kStream >> m_GoldChangeForTurnTimes100;
	kStream >> m_iLifetimeGrossGoldIncome;
}


void CvTreasury::Write(FDataStream& kStream)
{

	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << m_iGold;
	kStream << m_iGoldPerTurnFromDiplomacy;
	kStream << m_iExpensePerTurnUnitMaintenance;
	kStream << m_iExpensePerTurnUnitSupply;
	kStream << m_iCityConnectionGoldTimes100;
	kStream << m_iCityConnectionTradeRouteGoldModifier;
	kStream << m_iCityConnectionTradeRouteGoldChange;
	kStream << m_iBaseBuildingGoldMaintenance;
	kStream << m_iBaseImprovementGoldMaintenance;
	kStream << m_GoldBalanceForTurnTimes100;
	kStream << m_GoldChangeForTurnTimes100;
	kStream << m_iLifetimeGrossGoldIncome;
}

void TreasuryHelpers::AppendToLog(CvString& strHeader, CvString& strLog, CvString strHeaderValue, CvString strValue)
{
	strHeader += strHeaderValue;
	strHeader += ",";
	strLog += strValue;
	strLog += ",";
}

void TreasuryHelpers::AppendToLog(CvString& strHeader, CvString& strLog, CvString strHeaderValue, int iValue)
{
	strHeader += strHeaderValue;
	strHeader += ",";
	CvString str;
	str.Format("%d,", iValue);
	strLog += str;
}

void TreasuryHelpers::AppendToLog(CvString& strHeader, CvString& strLog, CvString strHeaderValue, float fValue)
{
	strHeader += strHeaderValue;
	strHeader += ",";
	CvString str;
	str.Format("%.2f,", fValue);
	strLog += str;
}
