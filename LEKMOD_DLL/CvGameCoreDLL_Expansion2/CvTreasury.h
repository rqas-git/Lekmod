






#pragma once

#ifndef CV_TREASURY_H
#define CV_TREASURY_H










class CvTreasury
{
public:
	CvTreasury();
	virtual ~CvTreasury();
	void Init(CvPlayer* pPlayer);
	void Uninit();

	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
	int getGoldT100ForThisTurn() const;
	int getGrossGoldForThisTurn() const;
	void cacheGoldT100ForThisTurn();
#endif
	void DoGold();


	int GetGold() const;
	void SetGold(int iNewValue);
	void ChangeGold(int iChange);
	int GetGoldTimes100() const;
	void SetGoldTimes100(int iNewValue);
	void ChangeGoldTimes100(int iChange);


	int GetGoldFromCities() const;
	int GetGoldFromCitiesTimes100(bool bExcludeTradeRoutes = false) const;


	int GetGoldPerTurnFromDiplomacy() const;
	void SetGoldPerTurnFromDiplomacy(int iValue);
	void ChangeGoldPerTurnFromDiplomacy(int iChange);


	int GetCityConnectionRouteGoldTimes100(CvCity* pNonCapitalCity) const;
	int GetCityConnectionGold() const;
	int GetCityConnectionGoldTimes100() const;
	void DoUpdateCityConnectionGold();


	int GetCityConnectionTradeRouteGoldModifier() const;
	void ChangeCityConnectionTradeRouteGoldModifier(int iChange);
	int GetCityConnectionTradeRouteGoldChange() const;
	void ChangeCityConnectionTradeRouteGoldChange(int iChange);
	bool HasCityConnectionRouteBetweenCities(CvCity* pFirstCity, CvCity* pSecondCity, bool bBestRoute = false) const;


	int GetGoldPerTurnFromTradeRoutes() const;
	int GetGoldPerTurnFromTradeRoutesTimes100() const;
	int GetGoldPerTurnFromTraits() const;


	int GetGoldPerTurnFromReligion() const;


	int CalculateGrossGold();
	int CalculateGrossGoldTimes100();
	int GetLifetimeGrossGold();
	int CalculateBaseNetGold();
	int CalculateBaseNetGoldTimes100();
	int CalculateUnitCost(int& iFreeUnits, int& iPaidUnits, int& iBaseUnitCost, int& iExtraCost);
	int CalculateUnitSupply(int& iPaidUnits, int& iBaseSupplyCost);
	int CalculatePreInflatedCosts();
	int CalculateInflationRate();
	int CalculateInflatedCosts();
	int GetExpensePerTurnUnitMaintenance()
	{
		return m_iExpensePerTurnUnitMaintenance;
	}
	int GetExpensePerTurnUnitSupply()
	{
		return m_iExpensePerTurnUnitSupply;
	}


	int GetBuildingGoldMaintenance() const;
	int GetBaseBuildingGoldMaintenance() const;
	void SetBaseBuildingGoldMaintenance(int iValue);
	void ChangeBaseBuildingGoldMaintenance(int iChange);

	int GetImprovementGoldMaintenance() const;
	int GetBaseImprovementGoldMaintenance() const;
	void SetBaseImprovementGoldMaintenance(int iValue);
	void ChangeBaseImprovementGoldMaintenance(int iChange);


	double AverageIncome(int iTurns);
	void CvTreasury::LogExpenditure(CvString strExpenditure, int iAmount, int iColumn);

protected:
	CvPlayer* m_pPlayer;
	int m_iGold;
	int m_iGoldPerTurnFromDiplomacy;
	int m_iExpensePerTurnUnitMaintenance;
	int m_iExpensePerTurnUnitSupply;
	int m_iCityConnectionGoldTimes100;
	int m_iCityConnectionTradeRouteGoldModifier;
	int m_iCityConnectionTradeRouteGoldChange;

	int m_iBaseBuildingGoldMaintenance;
	int m_iBaseImprovementGoldMaintenance;

	int m_iLifetimeGrossGoldIncome;
#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
	int m_iCurrentGoldChange;
	int m_iCurrentGrossGoldChange;
#endif

	std::vector<int> m_GoldBalanceForTurnTimes100;
	std::vector<int> m_GoldChangeForTurnTimes100;
};

namespace TreasuryHelpers
{
	void AppendToLog (CvString& strHeader, CvString& strLog, CvString strHeaderValue, CvString strValue);
	void AppendToLog (CvString& strHeader, CvString& strLog, CvString strHeaderValue, int iValue);
	void AppendToLog (CvString& strHeader, CvString& strLog, CvString strHeaderValue, float fValue);
}

#endif
