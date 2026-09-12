






#pragma once

#ifndef CIV5_CITY_STRATEGY_AI_H
#define CIV5_CITY_STRATEGY_AI_H









class CvAICityStrategyEntry: public CvBaseInfo
{
public:
	CvAICityStrategyEntry();
	virtual ~CvAICityStrategyEntry();

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);


	int GetFlavorValue(int i) const;

	int GetWeightThreshold() const;
	int GetPersonalityFlavorThresholdMod(int i) const;

	int GetTechPrereq() const;
	int GetTechObsolete() const;
	int GetMinimumNumTurnsExecuted() const;
	int GetCheckTriggerTurnCount() const;

	bool IsNoMinorCivs() const;
	bool IsPermanent() const;

	AdvisorTypes GetAdvisor() const;
	const char* GetAdvisorCounselText() const;
	int GetAdvisorCounselImportance() const;

private:
	int* m_piFlavorValue;

	int m_iWeightThreshold;
	int* m_piPersonalityFlavorThresholdMod;

	int m_iTechPrereq;
	int m_iTechObsolete;
	int m_iMinimumNumTurnsExecuted;
	int m_iCheckTriggerTurnCount;

	bool m_bNoMinorCivs;
	bool m_bPermanent;

	AdvisorTypes m_eAdvisor;
	CvString m_strAdvisorCounselText;
	int m_iAdvisorCounselImportance;
};












class CvAICityStrategies
{
public:
	CvAICityStrategies(void);
	~CvAICityStrategies(void);


	std::vector<CvAICityStrategyEntry*>& GetAICityStrategyEntries();
	int GetNumAICityStrategies();
	CvAICityStrategyEntry* GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvAICityStrategyEntry*> m_paAICityStrategyEntries;
};

class CvBuildingProductionAI;
class CvUnitProductionAI;
class CvProcessProductionAI;

enum CityBuildableType
{
    NOT_A_CITY_BUILDABLE = 0,
    CITY_BUILDABLE_BUILDING,
    CITY_BUILDABLE_UNIT,
    CITY_BUILDABLE_PROJECT,
	CITY_BUILDABLE_PROCESS,
    CITY_BUILDABLE_UNIT_FOR_OPERATION,
    CITY_BUILDABLE_UNIT_FOR_ARMY,
};





struct CvCityBuildable
{
	CvCityBuildable() :
		m_eBuildableType(NOT_A_CITY_BUILDABLE)
		, m_iIndex(0)
		, m_iTurnsToConstruct(0)
	{
	}

	CityBuildableType m_eBuildableType;
	int m_iIndex;
	int m_iTurnsToConstruct;
};











class CvCityStrategyAI: public CvFlavorRecipient
{
public:
#ifdef AUI_CITY_FIX_COMPONENT_CONSTRUCTORS_CONTAIN_POINTERS
	CvCityStrategyAI(CvCity* pCity);
#else
	CvCityStrategyAI(void);
#endif
	~CvCityStrategyAI(void);

	void Init(CvAICityStrategies* pAICityStrategies, CvCity* pCity, bool bIsCity);
	void Uninit();
	void Reset();
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);


	void FlavorUpdate();


	void UpdateFlavorsForNewCity();


	CitySpecializationTypes GetSpecialization() const
	{
		return m_eSpecialization;
	};
	bool SetSpecialization(CitySpecializationTypes eSpecialization);
	CitySpecializationTypes GetDefaultSpecialization() const
	{
		return m_eDefaultSpecialization;
	};
	bool SetDefaultSpecialization(CitySpecializationTypes eDefaultSpecialization);
	void SpecializationFlavorChange(bool bTurnOn, CitySpecializationTypes eSpecialization);


	CvCity* GetCity();
	CvAICityStrategies* GetAICityStrategies();
	bool IsUsingCityStrategy(AICityStrategyTypes eStrategy);
	void SetUsingCityStrategy(AICityStrategyTypes eStrategy, bool bValue);
	int GetTurnCityStrategyAdopted(AICityStrategyTypes eStrategy);
	void SetTurnCityStrategyAdopted(AICityStrategyTypes eStrategy, int iValue);
	CvBuildingProductionAI* GetBuildingProductionAI();
	CvUnitProductionAI* GetUnitProductionAI();
	CvProjectProductionAI* GetProjectProductionAI();
	CvProcessProductionAI* GetProcessProductionAI();
	CvString GetLogFileName(CvString& playerName, CvString& cityName) const;

	bool IsYieldDeficient(YieldTypes yieldType);
	YieldTypes GetDeficientYield(void);
	double GetYieldAverage(YieldTypes eYieldType);
	double GetDeficientYieldValue(YieldTypes eYieldType);


	void ChooseProduction(bool bUseAsyncRandom, BuildingTypes eIgnoreBldg = NO_BUILDING, UnitTypes eIgnoreUnit = NO_UNIT);
	void DoTurn();


	void ResetBestYields();
	void UpdateBestYields();
#ifdef AUI_WARNING_FIXES
	unsigned int GetBestYieldAverageTimes100(YieldTypes eYield);
	int GetYieldDeltaTimes100(YieldTypes eYield);
#else
	unsigned short GetBestYieldAverageTimes100(YieldTypes eYield);
	short GetYieldDeltaTimes100(YieldTypes eYield);
#endif
	YieldTypes GetFocusYield();


	void LogHurry(HurryTypes iHurryType, int iHurryAmount, int iHurryAmountAvailable, int iTurnsSaved);
#ifdef AUI_WARNING_FIXES
	void LogCityProduction(const CvCityBuildable& Buildable, bool bRush);
#else
	void LogCityProduction(CvCityBuildable Buildable, bool bRush);
#endif

private:

	void ReweightByCost();


	void LogFlavors(FlavorTypes eFlavor = NO_FLAVOR);
	void LogStrategy(AICityStrategyTypes eStrategy, bool bValue);
	void LogPossibleBuilds();
	void LogSpecializationChange(CitySpecializationTypes eSpecialization);

	CvCity* m_pCity;
	CvAICityStrategies* m_pAICityStrategies;

	CitySpecializationTypes m_eSpecialization;
	CitySpecializationTypes m_eDefaultSpecialization;

	bool* m_pabUsingCityStrategy;
	int* m_paiTurnCityStrategyAdopted;
	int* m_aiTempFlavors;


	CvBuildingProductionAI* m_pBuildingProductionAI;
	CvUnitProductionAI* m_pUnitProductionAI;
	CvProjectProductionAI* m_pProjectProductionAI;
	CvProcessProductionAI* m_pProcessProductionAI;

	CvWeightedVector<CvCityBuildable, (SAFE_ESTIMATE_NUM_BUILDINGS + SAFE_ESTIMATE_NUM_UNITS), true> m_Buildables;

#ifdef AUI_WARNING_FIXES
	static unsigned int  m_acBestYields[NUM_YIELD_TYPES][NUM_CITY_PLOTS];
	unsigned int m_asBestYieldAverageTimes100[NUM_YIELD_TYPES];
	int m_asYieldDeltaTimes100[NUM_YIELD_TYPES];
#else
	static unsigned char  m_acBestYields[NUM_YIELD_TYPES][NUM_CITY_PLOTS];
	unsigned short m_asBestYieldAverageTimes100[NUM_YIELD_TYPES];
	short m_asYieldDeltaTimes100[NUM_YIELD_TYPES];
#endif
	YieldTypes m_eFocusYield;
};

namespace CityStrategyAIHelpers
{

int ReweightByTurnsLeft(int iOriginalWeight, int iTurnsLeft);
int GetWeightThresholdModifier(AICityStrategyTypes eStrategy, CvCity* pCity);


bool IsTestCityStrategy_TinyCity(CvCity* pCity);
bool IsTestCityStrategy_SmallCity(CvCity* pCity);
bool IsTestCityStrategy_MediumCity(CvCity* pCity);
bool IsTestCityStrategy_LargeCity(CvCity* pCity);
bool IsTestCityStrategy_Landlocked(CvCity* pCity);
bool IsTestCityStrategy_NeedTileImprovers(AICityStrategyTypes eStrategy, CvCity* pCity);
bool IsTestCityStrategy_WantTileImprovers(AICityStrategyTypes eStrategy, CvCity* pCity);
bool IsTestCityStrategy_EnoughTileImprovers(AICityStrategyTypes eStrategy, CvCity* pCity);
bool IsTestCityStrategy_NeedNavalGrowth(AICityStrategyTypes eStrategy, CvCity* pCity);
bool IsTestCityStrategy_NeedNavalTileImprovement(CvCity* pCity);
bool IsTestCityStrategy_EnoughNavalTileImprovement(CvCity* pCity);
bool IsTestCityStrategy_NeedImprovement(CvCity* pCity, YieldTypes yield);
bool IsTestCityStrategy_HaveTrainingFacility(CvCity* pCity);
bool IsTestCityStrategy_CapitalNeedSettler(AICityStrategyTypes eStrategy, CvCity* pCity);
bool IsTestCityStrategy_CapitalUnderThreat(CvCity* pCity);
bool IsTestCityStrategy_FirstCultureBuilding(CvCity* pCity);
bool IsTestCityStrategy_FirstCultureBuildingEmergency(CvCity* pCity);
bool IsTestCityStrategy_FirstScienceBuilding(CvCity* pCity);
bool IsTestCityStrategy_FirstGoldBuilding(CvCity* pCity);
bool IsTestCityStrategy_FirstFaithBuilding(CvCity* pCity);
bool IsTestCityStrategy_FirstProductionBuilding(CvCity* pCity);
bool IsTestCityStrategy_UnderBlockade(CvCity* pCity);
bool IsTestCityStrategy_IsPuppet(CvCity* pCity);
bool IsTestCityStrategy_MediumCityHighDifficulty(CvCity* pCity);
bool IsTestCityStrategy_OriginalCapital(CvCity* pCity);
bool IsTestCityStrategy_RiverCity(CvCity* pCity);
bool IsTestCityStrategy_HillCity(CvCity* pCity);
bool IsTestCityStrategy_MountainCity(CvCity* pCity);
bool IsTestCityStrategy_ForestCity(CvCity* pCity);
bool IsTestCityStrategy_JungleCity(CvCity* pCity);
bool IsTestCityStrategy_CoastCity(CvCity* pCity);
bool IsTestCityStrategy_ManyTechsStolen(CvCity* pCity);
bool IsTestCityStrategy_KeyScienceCity(CvCity* pCity);
bool IsTestCityStrategy_GoodGPCity(CvCity* pCity);
bool IsTestCityStrategy_NeedInternationalTradeRoute(CvCity* pCity, DomainTypes eDomain);
bool IsTestCityStrategy_NoNeedInternationalTradeRoute(CvCity* pCity, DomainTypes eDomain);
bool IsTestCityStrategy_IsInternationalTradeDestination(CvCity* pCity);
bool IsTestCityStrategy_IsInternationalTradeOrigin(CvCity* pCity);
bool IsTestCityStrategy_NeedCultureBuilding(CvCity* pCity);
bool IsTestCityStrategy_NeedTourismBuilding(CvCity *pCity);
bool IsTestCityStrategy_GoodAirliftCity(CvCity *pCity);
}

#endif
