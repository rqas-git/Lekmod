







#include "CvGameCoreDLLPCH.h"
#include "CvEconomicAI.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvMinorCivAI.h"
#include "CvGrandStrategyAI.h"
#include "CvMilitaryAI.h"
#include "Fireworks/FVariableSystem.h"
#include "CvImprovementClasses.h"
#include "CvAStar.h"
#include "CvCitySpecializationAI.h"
#include "CvTypes.h"
#include "cvStopWatch.h"


#include "LintFree.h"





CvEconomicAIStrategyXMLEntry::CvEconomicAIStrategyXMLEntry(void):
	m_piPlayerFlavorValue(NULL),
	m_piCityFlavorValue(NULL),
	m_iWeightThreshold(0),
	m_piPersonalityFlavorThresholdMod(NULL),
	m_bNoMinorCivs(false),
	m_iTechPrereq(NO_TECH),
	m_iTechObsolete(NO_TECH),
	m_iMinimumNumTurnsExecuted(0),
	m_iCheckTriggerTurnCount(0),
	m_iFirstTurnExecuted(0),
	m_eAdvisor(NO_ADVISOR_TYPE),
	m_iAdvisorCounselImportance(1)
{
}

CvEconomicAIStrategyXMLEntry::~CvEconomicAIStrategyXMLEntry(void)
{
	SAFE_DELETE_ARRAY(m_piPlayerFlavorValue);
	SAFE_DELETE_ARRAY(m_piCityFlavorValue);
	SAFE_DELETE_ARRAY(m_piPersonalityFlavorThresholdMod);
}

bool CvEconomicAIStrategyXMLEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;


	m_bNoMinorCivs					= kResults.GetBool("NoMinorCivs");
	m_iCheckTriggerTurnCount		= kResults.GetInt("CheckTriggerTurnCount");
	m_iMinimumNumTurnsExecuted		= kResults.GetInt("MinimumNumTurnsExecuted");
	m_iWeightThreshold				= kResults.GetInt("WeightThreshold");
	m_iFirstTurnExecuted            = kResults.GetInt("FirstTurnExecuted");

	const char* strAdvisor = kResults.GetText("Advisor");
	m_eAdvisor = NO_ADVISOR_TYPE;
	if(strAdvisor)
	{
		if(strcmp(strAdvisor, "MILITARY") == 0)
		{
			m_eAdvisor = ADVISOR_MILITARY;
		}
		else if(strcmp(strAdvisor, "ECONOMIC") == 0)
		{
			m_eAdvisor = ADVISOR_ECONOMIC;
		}
		else if(strcmp(strAdvisor, "FOREIGN") == 0)
		{
			m_eAdvisor = ADVISOR_FOREIGN;
		}
		else if(strcmp(strAdvisor, "SCIENCE") == 0)
		{
			m_eAdvisor = ADVISOR_SCIENCE;
		}
	}

	m_strAdvisorCounselText = kResults.GetText("AdvisorCounsel");
	m_iAdvisorCounselImportance = kResults.GetInt("AdvisorCounselImportance");


	const char* szTextVal = NULL;
	szTextVal = kResults.GetText("TechPrereq");
	m_iTechPrereq = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("TechObsolete");
	m_iTechObsolete = GC.getInfoTypeForString(szTextVal, true);



	const char* szType = GetType();
	kUtility.SetFlavors(m_piPlayerFlavorValue, "AIEconomicStrategy_Player_Flavors",
	                    "AIEconomicStrategyType", szType);
	kUtility.SetFlavors(m_piCityFlavorValue, "AIEconomicStrategy_City_Flavors",
		"AIEconomicStrategyType", szType);


	kUtility.SetFlavors(m_piPersonalityFlavorThresholdMod,
	                    "AIEconomicStrategy_PersonalityFlavorThresholdMods",
	                    "AIEconomicStrategyType", szType);

	return true;
}


int CvEconomicAIStrategyXMLEntry::GetPlayerFlavorValue(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piPlayerFlavorValue ? m_piPlayerFlavorValue[i] : -1;
}


int CvEconomicAIStrategyXMLEntry::GetCityFlavorValue(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piCityFlavorValue ? m_piCityFlavorValue[i] : -1;
}


int CvEconomicAIStrategyXMLEntry::GetWeightThreshold() const
{
	return m_iWeightThreshold;
}


int CvEconomicAIStrategyXMLEntry::GetPersonalityFlavorThresholdMod(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piPersonalityFlavorThresholdMod ? m_piPersonalityFlavorThresholdMod[i] : -1;
}


bool CvEconomicAIStrategyXMLEntry::IsNoMinorCivs() const
{
	return m_bNoMinorCivs;
}


int CvEconomicAIStrategyXMLEntry::GetTechPrereq() const
{
	return m_iTechPrereq;
}


int CvEconomicAIStrategyXMLEntry::GetTechObsolete() const
{
	return m_iTechObsolete;
}


int CvEconomicAIStrategyXMLEntry::GetMinimumNumTurnsExecuted() const
{
	return m_iMinimumNumTurnsExecuted;
}


int CvEconomicAIStrategyXMLEntry::GetCheckTriggerTurnCount() const
{
	return m_iCheckTriggerTurnCount;
}


int CvEconomicAIStrategyXMLEntry::GetFirstTurnExecuted() const
{
	return m_iFirstTurnExecuted;
}

AdvisorTypes CvEconomicAIStrategyXMLEntry::GetAdvisor() const
{
	return m_eAdvisor;
}

const char* CvEconomicAIStrategyXMLEntry::GetAdvisorCounselText() const
{
	return m_strAdvisorCounselText.c_str();
}

int CvEconomicAIStrategyXMLEntry::GetAdvisorCounselImportance() const
{
	return m_iAdvisorCounselImportance;
}





CvEconomicAIStrategyXMLEntries::CvEconomicAIStrategyXMLEntries(void)
{

}


CvEconomicAIStrategyXMLEntries::~CvEconomicAIStrategyXMLEntries(void)
{
	DeleteArray();
}


std::vector<CvEconomicAIStrategyXMLEntry*>& CvEconomicAIStrategyXMLEntries::GetEconomicAIStrategyEntries()
{
	return m_paAIStrategyEntries;
}


int CvEconomicAIStrategyXMLEntries::GetNumEconomicAIStrategies()
{
	return m_paAIStrategyEntries.size();
}


void CvEconomicAIStrategyXMLEntries::DeleteArray()
{
	for(std::vector<CvEconomicAIStrategyXMLEntry*>::iterator it = m_paAIStrategyEntries.begin(); it != m_paAIStrategyEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paAIStrategyEntries.clear();
}


CvEconomicAIStrategyXMLEntry* CvEconomicAIStrategyXMLEntries::GetEntry(int index)
{
	return m_paAIStrategyEntries[index];
}






CvEconomicAI::CvEconomicAI():
	m_pabUsingStrategy(NULL),
	m_paiTurnStrategyAdopted(NULL),
	m_aiTempFlavors(NULL),
	m_iEarlyCityNumberTarget(1)
{
}


CvEconomicAI::~CvEconomicAI(void)
{
	Uninit();
}


void CvEconomicAI::Init(CvEconomicAIStrategyXMLEntries* pAIStrategies, CvPlayer* pPlayer)
{

	m_pAIStrategies = pAIStrategies;

	m_pPlayer = pPlayer;


	FAssertMsg(m_pabUsingStrategy==NULL, "about to leak memory, CvStrategyAI::m_pabUsingStrategy");
	m_pabUsingStrategy = FNEW(bool[m_pAIStrategies->GetNumEconomicAIStrategies()], c_eCiv5GameplayDLL, 0);

	FAssertMsg(m_paiTurnStrategyAdopted==NULL, "about to leak memory, CvStrategyAI::m_paiTurnStrategyAdopted");
	m_paiTurnStrategyAdopted = FNEW(int[m_pAIStrategies->GetNumEconomicAIStrategies()], c_eCiv5GameplayDLL, 0);

	FAssertMsg(m_aiTempFlavors==NULL, "about to leak memory, CvStrategyAI::m_aiTempFlavors");
	m_aiTempFlavors = FNEW(int[GC.getNumFlavorTypes()], c_eCiv5GameplayDLL, 0);

	m_auiYields.clear();
#ifdef AUI_WARNING_FIXES
	m_auiYields.push_back_copy(0, uint(NUM_YIELD_TYPES));
#else
	m_auiYields.push_back_copy(-1, NUM_YIELD_TYPES);
#endif
	m_RequestedSavings.clear();

	Reset();
}


void CvEconomicAI::Uninit()
{
	SAFE_DELETE_ARRAY(m_pabUsingStrategy);
	SAFE_DELETE_ARRAY(m_paiTurnStrategyAdopted);
	SAFE_DELETE_ARRAY(m_aiTempFlavors);
}


void CvEconomicAI::Reset()
{
	int iI;

	for(iI = 0; iI < m_pAIStrategies->GetNumEconomicAIStrategies(); iI++)
	{
		m_pabUsingStrategy[iI] = false;
		m_paiTurnStrategyAdopted[iI] = -1;
	}

	for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
		m_auiYields[ui] = 0;
	}

	for(uint ui = 0; ui < m_aiExplorationPlots.size(); ui++)
	{
		m_aiExplorationPlots[ui] = -1;
		m_aiExplorationPlotRatings[ui] = -1;
	}

	for(uint ui = 0; ui < m_aiGoodyHutPlots.size(); ui++)
	{
		m_aiGoodyHutPlots[ui] = -1;
		m_aiGoodyHutUnitAssignments[ui].Clear();
	}

	m_bExplorationPlotsDirty = true;

	for(uint ui = 0; ui < NUM_PURCHASE_TYPES; ui++)
	{
		CvPurchaseRequest request;
		request.m_eType = (PurchaseType)ui;
		request.m_iAmount = 0;
		request.m_iPriority = 0;
		m_RequestedSavings.push_back(request);
	}

	m_iExplorersDisbanded = 0;
	m_eReconState = NO_RECON_STATE;
	m_eNavalReconState = NO_RECON_STATE;
	m_iLastTurnWorkerDisbanded = -1;
	m_iVisibleAntiquitySites = 0;


	m_iMinimumSettleFertility = GC.getAI_STRATEGY_MINIMUM_SETTLE_FERTILITY();



	m_iEarlyCityNumberTarget = GC.getAI_STRATEGY_EARLY_EXPANSION_NUM_CITIES_LIMIT();
}


void CvEconomicAI::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	int iEntriesToRead;

	FAssertMsg(m_pAIStrategies != NULL && m_pAIStrategies->GetNumEconomicAIStrategies() > 0, "Number of AIStrategies to serialize is expected to greater than 0");

	kStream >> iEntriesToRead;

#ifdef _MSC_VER
#pragma warning ( push )
#pragma warning ( disable : 6011 )
#endif
	ArrayWrapper<bool> wrapGetNumEconomicAIStrategies(iEntriesToRead, m_pabUsingStrategy);
#ifdef _MSC_VER
#pragma warning ( pop )
#endif

	kStream >> wrapGetNumEconomicAIStrategies;

	ArrayWrapper<int> wrapGetNumEconomicAIStrategies2(iEntriesToRead, m_paiTurnStrategyAdopted);
	kStream >> wrapGetNumEconomicAIStrategies2;

	for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
		kStream >> m_auiYields[ui];
	}

	kStream >> m_bExplorationPlotsDirty;

	kStream >> iEntriesToRead;

#define MAX_PLOT_ARRAY_SIZE	((152+1)*(96+1))
	int iMaxEntriesToRead = MIN(MAX_PLOT_ARRAY_SIZE, iEntriesToRead);

	m_aiExplorationPlots.resize(iMaxEntriesToRead);
	m_aiExplorationPlotRatings.resize(iMaxEntriesToRead);

	for(int i = 0; i < iMaxEntriesToRead; i++)
	{
		kStream >> m_aiExplorationPlots[i];
		kStream >> m_aiExplorationPlotRatings[i];
	}


	for(int i = iMaxEntriesToRead; i < iEntriesToRead; i++)
	{
		int iDummy;
		kStream >> iDummy;
		kStream >> iDummy;
	}


	kStream >> iEntriesToRead;
	iMaxEntriesToRead = MIN(MAX_PLOT_ARRAY_SIZE, iEntriesToRead);
	m_aiGoodyHutPlots.resize(iMaxEntriesToRead);
	m_aiGoodyHutUnitAssignments.resize(iMaxEntriesToRead);

	for(int i = 0; i < iMaxEntriesToRead; i++)
	{
		kStream >> m_aiGoodyHutPlots[i];
		m_aiGoodyHutUnitAssignments[i].Clear();
		kStream >> m_aiGoodyHutUnitAssignments[i].m_iUnitID;
	}


	for(int i = iMaxEntriesToRead; i < iEntriesToRead; i++)
	{
		int iDummy;
		kStream >> iDummy;
		kStream >> iDummy;
	}

	int iTemp;
	kStream >> iTemp;
	m_eReconState = (ReconState)iTemp;
	kStream >> iTemp;
	m_eNavalReconState = (ReconState)iTemp;

	kStream >> m_iExplorersDisbanded;
	kStream >> m_iLastTurnWorkerDisbanded;
	kStream >> m_iVisibleAntiquitySites;

	kStream >> iEntriesToRead;
	for(int i = 0; i < iEntriesToRead; i++)
	{
		kStream >> m_RequestedSavings[i];
	}
}


void CvEconomicAI::Write(FDataStream& kStream)
{

	uint uiVersion = 1;
	kStream << uiVersion;

	FAssertMsg(GC.getNumEconomicAIStrategyInfos() > 0, "Number of AIStrategies to serialize is expected to greater than 0");
	kStream << m_pAIStrategies->GetNumEconomicAIStrategies();
	kStream << ArrayWrapper<bool>(m_pAIStrategies->GetNumEconomicAIStrategies(), m_pabUsingStrategy);
	kStream << ArrayWrapper<int>(m_pAIStrategies->GetNumEconomicAIStrategies(), m_paiTurnStrategyAdopted);
	for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
		kStream << m_auiYields[ui];
	}

	kStream << m_bExplorationPlotsDirty;
	kStream << m_aiExplorationPlots.size();
	for(uint ui = 0; ui < m_aiExplorationPlots.size(); ui++)
	{
		kStream << m_aiExplorationPlots[ui];
		kStream << m_aiExplorationPlotRatings[ui];
	}

	kStream << m_aiGoodyHutPlots.size();
	for(uint ui = 0; ui < m_aiGoodyHutPlots.size(); ui++)
	{
		kStream << m_aiGoodyHutPlots[ui];
		kStream << m_aiGoodyHutUnitAssignments[ui].m_iUnitID;
	}

	kStream << (int)m_eReconState;
	kStream << (int)m_eNavalReconState;
	kStream << m_iExplorersDisbanded;
	kStream << m_iLastTurnWorkerDisbanded;
	kStream << m_iVisibleAntiquitySites;

	kStream << (int)m_RequestedSavings.size();
	for(uint ui = 0; ui < m_RequestedSavings.size(); ui++)
	{
		kStream << m_RequestedSavings[ui];
	}
}


CvPlayer* CvEconomicAI::GetPlayer()
{
	return m_pPlayer;
}


CvEconomicAIStrategyXMLEntries* CvEconomicAI::GetEconomicAIStrategies()
{
	return m_pAIStrategies;
}


#if defined(AUI_WARNING_FIXES) || defined(AUI_CONSTIFY)
bool CvEconomicAI::IsUsingStrategy(EconomicAIStrategyTypes eStrategy) const
#else
bool CvEconomicAI::IsUsingStrategy(EconomicAIStrategyTypes eStrategy)
#endif
{
	return m_pabUsingStrategy[(int) eStrategy];
}


void CvEconomicAI::SetUsingStrategy(EconomicAIStrategyTypes eStrategy, bool bValue)
{
	if(m_pabUsingStrategy[eStrategy] != bValue)
	{
		m_pabUsingStrategy[eStrategy] = bValue;

		if(bValue)
		{
			SetTurnStrategyAdopted(eStrategy, GC.getGame().getGameTurn());
		}
		else
		{
			SetTurnStrategyAdopted(eStrategy, -1);
		}

		LogStrategy(eStrategy, bValue);
	}
}


#if defined(AUI_WARNING_FIXES) || defined(AUI_CONSTIFY)
int CvEconomicAI::GetTurnStrategyAdopted(EconomicAIStrategyTypes eStrategy) const
#else
int CvEconomicAI::GetTurnStrategyAdopted(EconomicAIStrategyTypes eStrategy)
#endif
{
	return m_paiTurnStrategyAdopted[(int) eStrategy];
}


void CvEconomicAI::SetTurnStrategyAdopted(EconomicAIStrategyTypes eStrategy, int iValue)
{
	if(m_paiTurnStrategyAdopted[(int) eStrategy] != iValue)
	{
		m_paiTurnStrategyAdopted[(int) eStrategy] = iValue;
	}
}




void CvEconomicAI::DoTurn()
{
	AI_PERF_FORMAT("AI-perf.csv", ("CvEconomicAI::DoTurn, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), GetPlayer()->getCivilizationShortDescription()) );

	UpdatePlots();
	LogMonitor();
	LogCityMonitor();


	DoReconState();
	DoAntiquitySites();

	int iStrategiesLoop = 0;


	for(iStrategiesLoop = 0; iStrategiesLoop < GetEconomicAIStrategies()->GetNumEconomicAIStrategies(); iStrategiesLoop++)
	{
		EconomicAIStrategyTypes eStrategy = (EconomicAIStrategyTypes) iStrategiesLoop;
		CvEconomicAIStrategyXMLEntry* pStrategy = GetEconomicAIStrategies()->GetEntry(iStrategiesLoop);


		if(m_pPlayer->isMinorCiv() && pStrategy->IsNoMinorCivs())
		{
			continue;
		}

		bool bTestStrategyStart = true;


		if(IsUsingStrategy(eStrategy))
		{
			bTestStrategyStart = false;
		}
		else
		{

			if(pStrategy->GetTechPrereq() != NO_TECH && !GET_TEAM(GetPlayer()->getTeam()).GetTeamTechs()->HasTech((TechTypes) pStrategy->GetTechPrereq()))
			{
				bTestStrategyStart = false;
			}


			if(bTestStrategyStart && pStrategy->GetTechObsolete() != NO_TECH && GET_TEAM(GetPlayer()->getTeam()).GetTeamTechs()->HasTech((TechTypes) pStrategy->GetTechObsolete()))
			{
				bTestStrategyStart = false;
			}


			if(GC.getGame().getGameTurn() < pStrategy->GetFirstTurnExecuted())
			{
				bTestStrategyStart = false;
			}
		}

		bool bTestStrategyEnd = false;


		if(IsUsingStrategy(eStrategy))
		{
			if(pStrategy->GetCheckTriggerTurnCount() > 0)
			{

				if((GC.getGame().getGameTurn() - GetTurnStrategyAdopted(eStrategy)) % pStrategy->GetCheckTriggerTurnCount() == 0)
				{
					bTestStrategyEnd = true;
				}
			}

			if(bTestStrategyEnd && pStrategy->GetMinimumNumTurnsExecuted() > 0)
			{

				if(GC.getGame().getGameTurn() < GetTurnStrategyAdopted(eStrategy) + pStrategy->GetMinimumNumTurnsExecuted())
				{
					bTestStrategyEnd = false;
				}
			}
		}




		if(bTestStrategyStart || bTestStrategyEnd)
		{
			bool bStrategyShouldBeActive = false;


			if(pStrategy->GetTechObsolete() != NO_TECH && GET_TEAM(GetPlayer()->getTeam()).GetTeamTechs()->HasTech((TechTypes) pStrategy->GetTechObsolete()))
			{
				bStrategyShouldBeActive = false;
			}

			else
			{
				CvString strStrategyName = (CvString) pStrategy->GetType();


				if(strStrategyName == "ECONOMICAISTRATEGY_NEED_RECON")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedRecon(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_ENOUGH_RECON")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_EnoughRecon(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_REALLY_NEED_RECON_SEA")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_ReallyNeedReconSea(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_NEED_RECON_SEA")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedReconSea(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_ENOUGH_RECON_SEA")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_EnoughReconSea(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_EARLY_EXPANSION")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_EarlyExpansion(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_ENOUGH_EXPANSION")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_EnoughExpansion(eStrategy, m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_NEED_HAPPINESS")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedHappiness(eStrategy, m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_NEED_HAPPINESS_CRITICAL")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedHappinessCritical(eStrategy, m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_CITIES_NEED_NAVAL_GROWTH")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_CitiesNeedNavalGrowth(eStrategy, m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_CITIES_NEED_NAVAL_TILE_IMPROVEMENT")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_CitiesNeedNavalTileImprovement(eStrategy, m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_FOUND_CITY")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_FoundCity(eStrategy, m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_TRADE_WITH_CITY_STATE")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_TradeWithCityState(eStrategy, m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_NEED_IMPROVEMENT_FOOD")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedImprovement(m_pPlayer, YIELD_FOOD);
				else if(strStrategyName == "ECONOMICAISTRATEGY_NEED_IMPROVEMENT_PRODUCTION")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedImprovement(m_pPlayer, YIELD_PRODUCTION);
				else if(strStrategyName == "ECONOMICAISTRATEGY_ONE_OR_FEWER_COASTAL_CITIES")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_OneOrFewerCoastalCities(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_LOSING_MONEY")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_LosingMoney(eStrategy, m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_HALT_GROWTH_BUILDINGS")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_HaltGrowthBuildings(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_TOO_MANY_UNITS")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_TooManyUnits(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_ISLAND_START")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_IslandStart(eStrategy, m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_ExpandToOtherContinents(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_REALLY_EXPAND_TO_OTHER_CONTINENTS")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_ReallyExpandToOtherContinents(m_pPlayer);				
				else if(strStrategyName == "ECONOMICAISTRATEGY_MOSTLY_ON_THE_COAST")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_MostlyOnTheCoast(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_EXPAND_LIKE_CRAZY")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_ExpandLikeCrazy(eStrategy, m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_GROW_LIKE_CRAZY")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_GrowLikeCrazy(eStrategy, m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_GS_CULTURE")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_GS_Culture(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_GS_CONQUEST")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_GS_Conquest(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_GS_DIPLOMACY")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_GS_Diplomacy(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_GS_SPACESHIP")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_GS_Spaceship(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_GS_SPACESHIP_HOMESTRETCH")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_GS_SpaceshipHomestretch(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_NAVAL_MAP")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NavalMap(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_OFFSHORE_EXPANSION_MAP")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_OffshoreExpansionMap(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_DEVELOPING_RELIGION")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_DevelopingReligion(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_TECH_LEADER")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_TechLeader(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_NEED_ARCHAEOLOGISTS")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedArchaeologists(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_ENOUGH_ARCHAEOLOGISTS")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_EnoughArchaeologists(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_NEED_MUSEUMS")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedMuseums(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_NEED_GUILDS")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedGuilds(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_CONCERT_TOUR")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_ConcertTour(eStrategy, m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_STARTED_PIETY")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_StartedPiety(m_pPlayer);


				else
				{
					bStrategyShouldBeActive = true;
				}


				ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
				if(pkScriptSystem && bStrategyShouldBeActive)
				{
					CvLuaArgsHandle args;
					args->Push(iStrategiesLoop);
					args->Push(m_pPlayer->GetID());



					bool bResult = false;
					if(LuaSupport::CallTestAll(pkScriptSystem, "EconomicStrategyCanActivate", args.get(), bResult))
					{

						if(bResult == false)
						{
							bStrategyShouldBeActive = false;
						}
					}
				}
			}


			bool bAdoptOrEndStrategy = false;


			if(bStrategyShouldBeActive)
			{
				if(bTestStrategyStart)
				{
					bAdoptOrEndStrategy = true;
				}
				else if(bTestStrategyEnd)
				{
					bAdoptOrEndStrategy = false;
				}
			}

			else
			{
				if(bTestStrategyStart)
				{
					bAdoptOrEndStrategy = false;
				}
				else if(bTestStrategyEnd)
				{
					bAdoptOrEndStrategy = true;
				}
			}


			if(bAdoptOrEndStrategy)
			{
				int iFlavorLoop;


				if(bTestStrategyStart)
				{
					SetUsingStrategy(eStrategy, true);

					for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
					{
						m_aiTempFlavors[iFlavorLoop] = pStrategy->GetPlayerFlavorValue(iFlavorLoop);
					}

					GetPlayer()->GetFlavorManager()->ChangeFlavors(m_aiTempFlavors, true);

					for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
					{
						m_aiTempFlavors[iFlavorLoop] = pStrategy->GetCityFlavorValue(iFlavorLoop);
					}

					GetPlayer()->GetFlavorManager()->ChangeFlavors(m_aiTempFlavors, false);
				}

				else if(bTestStrategyEnd)
				{
					SetUsingStrategy(eStrategy, false);

					for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
					{
						m_aiTempFlavors[iFlavorLoop] = -pStrategy->GetPlayerFlavorValue(iFlavorLoop);
					}

					GetPlayer()->GetFlavorManager()->ChangeFlavors(m_aiTempFlavors, true);

					for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
					{
						m_aiTempFlavors[iFlavorLoop] = -pStrategy->GetCityFlavorValue(iFlavorLoop);
					}

					GetPlayer()->GetFlavorManager()->ChangeFlavors(m_aiTempFlavors, false);
				}
			}
		}
	}

	if(!m_pPlayer->isHuman())
	{
		DoHurry();
		DoPlotPurchases();
		DisbandExtraWorkers();
		if (!m_pPlayer->isMinorCiv() && !m_pPlayer->isBarbarian())
		{
			DisbandExtraArchaeologists();
			m_pPlayer->GetCulture()->DoSwapGreatWorks();
		}
	}
}


CvCity* CvEconomicAI::GetBestGreatWorkCity(CvPlot *pStartPlot, GreatWorkType eGreatWork) const
{
	CvCity* pBestCity = NULL;
	CvCity* pLoopCity;
	int iLoop;
	int iBestDistance = MAX_INT;


	GreatWorkSlotType eGreatWorkSlot = CultureHelpers::GetGreatWorkSlot(eGreatWork);
	if (m_pPlayer->GetCulture()->HasAvailableGreatWorkSlot(eGreatWorkSlot))
	{
		for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			if (pLoopCity->getDamage() == 0)
			{
				int iDistance = plotDistance(pStartPlot->getX(), pStartPlot->getY(), pLoopCity->getX(), pLoopCity->getY());
				if(iDistance < iBestDistance)
				{
					iBestDistance = iDistance;
					pBestCity = pLoopCity;
				}
			}
		}
	}

	return pBestCity;
}

#ifdef AUI_WARNING_FIXES
void AppendToLog(CvString& strHeader, CvString& strLog, const CvString& strHeaderValue, const CvString& strValue)
#else
void AppendToLog(CvString& strHeader, CvString& strLog, CvString strHeaderValue, CvString strValue)
#endif
{
	strHeader += strHeaderValue;
	strHeader += ",";
	strLog += strValue;
	strLog += ",";
}

#ifdef AUI_WARNING_FIXES
void AppendToLog(CvString& strHeader, CvString& strLog, const CvString& strHeaderValue, int iValue)
#else
void AppendToLog(CvString& strHeader, CvString& strLog, CvString strHeaderValue, int iValue)
#endif
{
	strHeader += strHeaderValue;
	strHeader += ",";
	CvString str;
	str.Format("%d,", iValue);
	strLog += str;
}

#ifdef AUI_WARNING_FIXES
void AppendToLog(CvString& strHeader, CvString& strLog, const CvString& strHeaderValue, float fValue)
#else
void AppendToLog(CvString& strHeader, CvString& strLog, CvString strHeaderValue, float fValue)
#endif
{
	strHeader += strHeaderValue;
	strHeader += ",";
	CvString str;
	str.Format("%.2f,", fValue);
	strLog += str;
}

FFastVector<int>& CvEconomicAI::GetExplorationPlots()
{
	if(m_bExplorationPlotsDirty)
	{
		UpdatePlots();
	}

	return m_aiExplorationPlots;
}

FFastVector<int>& CvEconomicAI::GetExplorationPlotRatings()
{
	if(m_bExplorationPlotsDirty)
	{
		UpdatePlots();
	}

	return m_aiExplorationPlotRatings;
}

FFastVector<int>& CvEconomicAI::GetGoodyHutPlots()
{
	if(m_bExplorationPlotsDirty)
	{
		UpdatePlots();
	}

	return m_aiGoodyHutPlots;
}


CvPlot* CvEconomicAI::GetUnitTargetGoodyPlot(CvUnit* pUnit, CvPlot** ppkStepPlot            )
{
	if(m_bExplorationPlotsDirty)
	{
		UpdatePlots();
	}

	int iUnitID = pUnit->GetID();
	for(uint ui = 0; ui < m_aiGoodyHutUnitAssignments.size(); ui++)
	{
		if(iUnitID == m_aiGoodyHutUnitAssignments[ui].m_iUnitID)
		{
			if (ppkStepPlot)
			{
				int iStepPlotID = m_aiGoodyHutUnitAssignments[ui].m_iStepPlotID;
				*ppkStepPlot = (iStepPlotID != -1)?GC.getMap().plotByIndex(iStepPlotID):NULL;
			}
			return GC.getMap().plotByIndex(m_aiGoodyHutPlots[ui]);
		}
	}

	return NULL;
}




void CvEconomicAI::ClearUnitTargetGoodyStepPlot(CvUnit* pUnit)
{
	int iUnitID = pUnit->GetID();
	for(uint ui = 0; ui < m_aiGoodyHutUnitAssignments.size(); ui++)
	{
		if(iUnitID == m_aiGoodyHutUnitAssignments[ui].m_iUnitID)
		{
			m_aiGoodyHutUnitAssignments[ui].m_iStepPlotID = -1;
		}
	}
}


int CvEconomicAI::ScoreExplorePlot(CvPlot* pPlot, TeamTypes eTeam, int iRange, DomainTypes eDomainType)
{
	int iResultValue = 0;
	int iAdjacencyBonus = 1;
	int iBadScore = 10;
	int iGoodScore = 100;
	int iReallyGoodScore = 200;


	int iPlotX = pPlot->getX();
	int iPlotY = pPlot->getY();

	FAssertMsg(pPlot->isRevealed(eTeam), "Plot isn't revealed. This isn't good.");
	CvPlot* pEvalPlot = NULL;
#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX, iX;
	for (int iY = -iRange; iY <= iRange; iY++)
	{
		iMaxDX = iRange - MAX(0, iY);
		for (iX = -iRange - MIN(0, iY); iX <= iMaxDX; iX++)
		{

			pEvalPlot = plotXY(iPlotX, iPlotY, iX, iY);
#else
	for(int iX = -iRange; iX <= iRange; iX++)
	{
		for(int iY = -iRange; iY <= iRange; iY++)
		{
			pEvalPlot = plotXYWithRangeCheck(iPlotX, iPlotY, iX, iY, iRange);
#endif
			if(!pEvalPlot)
			{
				continue;
			}

			if(pEvalPlot == pPlot)
			{
				continue;
			}

			if(pEvalPlot->isRevealed(eTeam))
			{
				continue;
			}

#ifdef AUI_FIX_HEX_DISTANCE_INSTEAD_OF_PLOT_DISTANCE
			int iMainDistance = hexDistance(iX, iY);
#endif
			if(pEvalPlot->isAdjacentRevealed(eTeam))
			{
#ifdef AUI_FIX_HEX_DISTANCE_INSTEAD_OF_PLOT_DISTANCE
				if (iMainDistance > 1)
#else
				if(plotDistance(iPlotX, iPlotY, pEvalPlot->getX(), pEvalPlot->getY()) > 1)
#endif
				{
					CvPlot* pAdjacentPlot;
					bool bViewBlocked = true;
					for(int i = 0; i < NUM_DIRECTION_TYPES; ++i)
					{
						pAdjacentPlot = plotDirection(pEvalPlot->getX(), pEvalPlot->getY(), ((DirectionTypes)i));
						if(pAdjacentPlot != NULL)
						{
							if(pAdjacentPlot->isRevealed(eTeam))
							{
								int iDistance = plotDistance(iPlotX, iPlotY, pAdjacentPlot->getX(), pAdjacentPlot->getY());
								if(iDistance > iRange)
								{
									continue;
								}


								if(pPlot->canSeePlot(pEvalPlot, eTeam, iRange, NO_DIRECTION))
								{
									bViewBlocked = false;
								}

								if(!bViewBlocked)
								{
									break;
								}
							}
						}
					}

					if(bViewBlocked)
					{
						continue;
					}
				}



				switch(eDomainType)
				{
				case DOMAIN_SEA:
				{
					FeatureTypes eFeature = pEvalPlot->getFeatureType();
					if(pEvalPlot->isWater() || (eFeature != NO_FEATURE && GC.getFeatureInfo(eFeature)->isImpassable()))
					{
						iResultValue += iBadScore;
					}
					else if(pEvalPlot->isMountain() || pEvalPlot->isHills() || (eFeature != NO_FEATURE && GC.getFeatureInfo(eFeature)->getSeeThroughChange() > 0))
					{
						iResultValue += iGoodScore;
					}
					else
					{
						iResultValue += iReallyGoodScore;
					}
				}
				break;
				case DOMAIN_LAND:
					if(pEvalPlot->isMountain() || pEvalPlot->isWater())
					{
						iResultValue += iBadScore;
					}
					else if(pEvalPlot->isHills())
					{
						iResultValue += iReallyGoodScore;
					}
					else
					{
						iResultValue += iGoodScore;
					}
					break;
				}
			}
			else
			{
				iResultValue += iGoodScore;
			}

#ifdef AUI_FIX_HEX_DISTANCE_INSTEAD_OF_PLOT_DISTANCE
			iResultValue += (iRange - iMainDistance) * iAdjacencyBonus;
#else
			int iDistance = plotDistance(iPlotX, iPlotY, pEvalPlot->getX(), pEvalPlot->getY());
			iResultValue += (iRange - iDistance) * iAdjacencyBonus;
#endif
		}
	}

	return iResultValue;
}


void CvEconomicAI::StartSaveForPurchase(PurchaseType ePurchase, int iAmount, int iPriority)
{
	int iIndex = (int)ePurchase;
	CvPurchaseRequest request;
	request.m_eType = ePurchase;
	request.m_iAmount = iAmount;
	request.m_iPriority = iPriority;
	m_RequestedSavings[iIndex] = request;
}


#if defined(AUI_WARNING_FIXES) || defined(AUI_CONSTIFY)
bool CvEconomicAI::IsSavingForThisPurchase(PurchaseType ePurchase) const
#else
bool CvEconomicAI::IsSavingForThisPurchase(PurchaseType ePurchase)
#endif
{
	return (m_RequestedSavings[(int)ePurchase].m_iAmount > 0);
}


void CvEconomicAI::CancelSaveForPurchase(PurchaseType ePurchase)
{
	int iIndex = (int)ePurchase;
	CvPurchaseRequest request;
	request.m_eType = ePurchase;
	request.m_iAmount = 0;
	request.m_iPriority = 0;
	m_RequestedSavings[iIndex] = request;
}



bool CvEconomicAI::CanWithdrawMoneyForPurchase(PurchaseType ePurchase, int iAmount, int iPriority)
{
	int iBalance = m_pPlayer->GetTreasury()->GetGold();


	if(iPriority != -1)
	{
		int iIndex = (int)ePurchase;
		m_RequestedSavings[iIndex].m_iPriority = iPriority;
	}


	m_TempRequestedSavings = m_RequestedSavings;
	std::stable_sort(m_TempRequestedSavings.begin(), m_TempRequestedSavings.end());

	for(int iI = 0; iI < (int)m_TempRequestedSavings.size(); iI++)
	{
		CvPurchaseRequest request = m_TempRequestedSavings[iI];


		if(request.m_eType != ePurchase)
		{
			iBalance -= request.m_iAmount;


			if(iBalance <= 0)
			{
				return false;
			}
		}


		else if(request.m_eType == ePurchase)
		{
			return (iBalance >=iAmount);
		}
	}

	CvAssert(false);
	return false;
}


#if defined(AUI_WARNING_FIXES) || defined(AUI_CONSTIFY)
int CvEconomicAI::AmountAvailableForPurchase(PurchaseType ePurchase) const
#else
int CvEconomicAI::AmountAvailableForPurchase(PurchaseType ePurchase)
#endif
{
	int iBalance = m_pPlayer->GetTreasury()->GetGold();


#ifdef AUI_WARNING_FIXES
	FStaticVector<CvPurchaseRequest, NUM_PURCHASE_TYPES, true, c_eCiv5GameplayDLL, 0> vTempRequestedSavings = m_RequestedSavings;
	std::stable_sort(vTempRequestedSavings.begin(), vTempRequestedSavings.end());

	for (FStaticVector<CvPurchaseRequest, NUM_PURCHASE_TYPES, true, c_eCiv5GameplayDLL, 0>::iterator it = vTempRequestedSavings.begin(); it != vTempRequestedSavings.end(); ++it)
	{
		CvPurchaseRequest request = *it;
#else
	m_TempRequestedSavings = m_RequestedSavings;
	std::stable_sort(m_TempRequestedSavings.begin(), m_TempRequestedSavings.end());

	for(int iI = 0; iI < (int)m_TempRequestedSavings.size(); iI++)
	{
		CvPurchaseRequest request = m_TempRequestedSavings[iI];
#endif


		if(request.m_eType != ePurchase)
		{
			iBalance -= request.m_iAmount;


			if(iBalance <= 0)
			{
				return 0;
			}
		}


		else if(request.m_eType == ePurchase)
		{
			return (iBalance);
		}
	}

	CvAssert(false);
#ifdef AUI_WARNING_FIXES
	return iBalance;
#else
	return false;
#endif
}


#if defined(AUI_WARNING_FIXES) || defined(AUI_CONSTIFY)
double CvEconomicAI::GetWorkersToCitiesRatio() const
#else
double CvEconomicAI::GetWorkersToCitiesRatio()
#endif
{
	int iNumWorkers = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_WORKER, true, false);
	int iNumCities = m_pPlayer->getNumCities();
	double fCurrentRatio = iNumWorkers / (double)iNumCities;

	return fCurrentRatio;
}


#if defined(AUI_WARNING_FIXES) || defined(AUI_CONSTIFY)
double CvEconomicAI::GetImprovedToImprovablePlotsRatio() const
#else
double CvEconomicAI::GetImprovedToImprovablePlotsRatio()
#endif
{
	const CvPlotsVector& aiPlots = m_pPlayer->GetPlots();
	int iNumValidPlots = 0;
	int iNumImprovedPlots = 0;
	for(uint ui = 0; ui < aiPlots.size(); ui++)
	{
		if(aiPlots[ui] == -1)
		{
			continue;
		}

		const CvPlot* pPlot = GC.getMap().plotByIndex(aiPlots[ui]);
		if(!pPlot)
		{
			continue;
		}

		if(pPlot->isWater() || pPlot->isImpassable() || pPlot->isMountain() || pPlot->isCity())
		{
			continue;
		}

		iNumValidPlots++;

		if(pPlot->getImprovementType() != NO_IMPROVEMENT && !pPlot->IsImprovementPillaged())
		{
			iNumImprovedPlots++;
		}
	}

	if(iNumValidPlots <= 0)
	{
		return 1.0;
	}
	double fCurrentRatio = iNumImprovedPlots / (double)iNumValidPlots;

	return fCurrentRatio;
}



void CvEconomicAI::LogMonitor(void)
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
		strLogName = "EconomicMonitorLog_" + strPlayerName + ".csv";
	}
	else
	{
		strLogName = "EconomicMonitorLog.csv";
	}

	FILogFile* pLog;
	pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

	CvString str;


	AppendToLog(strHeader, strLog, "Civ Name", strPlayerName);


	AppendToLog(strHeader, strLog, "Turn", GC.getGame().getGameTurn());


	AppendToLog(strHeader, strLog, "# Cities", GetPlayer()->getNumCities());


	int iPop = 0;
	int iLoopCity = 0;
	CvCity* pLoopCity = NULL;
	for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iLoopCity))
	{
		iPop += pLoopCity->getPopulation();
	}
	AppendToLog(strHeader, strLog, "Pop", iPop);


	for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
		switch(ui)
		{
		case YIELD_FOOD:
			AppendToLog(strHeader, strLog, "Food", m_pPlayer->calculateTotalYield((YieldTypes)ui));
			break;
		case YIELD_PRODUCTION:
			AppendToLog(strHeader, strLog, "Production", m_pPlayer->calculateTotalYield((YieldTypes)ui));
			break;
		case YIELD_GOLD:
			AppendToLog(strHeader, strLog, "City Gold", m_pPlayer->calculateTotalYield((YieldTypes)ui));
			break;
		case YIELD_SCIENCE:
			AppendToLog(strHeader, strLog, "Science", m_pPlayer->calculateTotalYield((YieldTypes)ui));
			break;
		case YIELD_CULTURE:
			AppendToLog(strHeader, strLog, "Culture", m_pPlayer->GetTotalJONSCulturePerTurn());
			break;
		case YIELD_FAITH:
			AppendToLog(strHeader, strLog, "Faith", m_pPlayer->GetTotalFaithPerTurn());
			break;
		}
	}


	AppendToLog(strHeader, strLog, "Num Techs", GET_TEAM(GetPlayer()->getTeam()).GetTeamTechs()->GetNumTechsKnown());


	AppendToLog(strHeader, strLog, "Policies", GetPlayer()->GetPlayerPolicies()->GetNumPoliciesOwned());
	AppendToLog(strHeader, strLog, "Culture (lifetime)", GetPlayer()->GetJONSCultureEverGenerated());


	AppendToLog(strHeader, strLog, "Faith", GetPlayer()->GetFaith());


	int iWorkerCount = 0;
	CvUnit* pLoopUnit;
	int iLoopUnit;
	for(pLoopUnit = m_pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoopUnit))
	{
		if(pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER)
		{
			iWorkerCount++;
		}
	}
	AppendToLog(strHeader, strLog, "# Workers", iWorkerCount);
	AppendToLog(strHeader, strLog, "Positive Happiness",  m_pPlayer->GetHappiness());
	AppendToLog(strHeader, strLog, "Negative Happiness", m_pPlayer->GetUnhappiness());
	AppendToLog(strHeader, strLog, "Net Happiness", m_pPlayer->GetExcessHappiness());


	FFastVector<int, true> m_aiNumImprovements;
	m_aiNumImprovements.push_back_copy(-1, GC.getNumImprovementInfos());


	CvPlotsVector& aiPlots = m_pPlayer->GetPlots();


	int iTiles = 0;
	int iWorkedTiles = 0;
	int iImprovedTiles = 0;
	for(uint uiPlotIndex = 0; uiPlotIndex < aiPlots.size(); uiPlotIndex++)
	{

		if(aiPlots[uiPlotIndex] == -1)
		{
			break;
		}

		CvPlot* pPlot = GC.getMap().plotByIndex(aiPlots[uiPlotIndex]);
		if(!pPlot)
		{
			continue;
		}

		iTiles++;


		if(pPlot->getPlotCity())
		{
			continue;
		}

		if(pPlot->isBeingWorked())
		{
			iWorkedTiles++;
		}

		if(pPlot->getImprovementType() != NO_IMPROVEMENT)
		{
			iImprovedTiles++;
		}
	}

	AppendToLog(strHeader, strLog, "Tiles", iTiles);
	AppendToLog(strHeader, strLog, "Worked (non-city) tiles", iWorkedTiles);
	AppendToLog(strHeader, strLog, "Improved tiles", iImprovedTiles);

	AppendToLog(strHeader, strLog, "Trade Routes established", m_pPlayer->GetTrade()->GetNumTradeRoutesUsed(false));
	AppendToLog(strHeader, strLog, "Trade Routes available", (int)m_pPlayer->GetTrade()->GetNumTradeRoutesPossible());

	CvTreasury *pTreasury = m_pPlayer->GetTreasury();

	int iInternationalTradeGPT = pTreasury->GetGoldFromCitiesTimes100(false) - pTreasury->GetGoldFromCitiesTimes100(true);
	AppendToLog(strHeader, strLog, "Gold From Cities", pTreasury->GetGoldFromCitiesTimes100(true) / 100);
	AppendToLog(strHeader, strLog, "Gold From Trade Routes", (int)(iInternationalTradeGPT / 100));
	AppendToLog(strHeader, strLog, "Treasury", pTreasury->GetGold());
	AppendToLog(strHeader, strLog, "GPT - Connects", pTreasury->GetCityConnectionGold());
	AppendToLog(strHeader, strLog, "GPT - Diplo", pTreasury->GetGoldPerTurnFromDiplomacy());
	AppendToLog(strHeader, strLog, "GPT - Religion", pTreasury->GetGoldPerTurnFromReligion());
	AppendToLog(strHeader, strLog, "GPT - Traits", pTreasury->GetGoldPerTurnFromTraits());

	int iGoldFromCitiesMinusTR = pTreasury->GetGoldFromCitiesTimes100(true) / 100;
	int iGPTFromDiplomacy = pTreasury->GetGoldPerTurnFromDiplomacy();
	int iGoldFromCityConnect = pTreasury->GetCityConnectionGoldTimes100() / 100;
	int iGPTFromReligion = pTreasury->GetGoldPerTurnFromReligion();
	int iGPTFromTraits = pTreasury->GetGoldPerTurnFromTraits();
	int iTradeRouteGold = (int)(iInternationalTradeGPT / 100);
	int iTotalIncome = iGoldFromCitiesMinusTR + iGPTFromDiplomacy + iGoldFromCityConnect + iGPTFromReligion + iGPTFromTraits + iTradeRouteGold;
	AppendToLog(strHeader, strLog, "Total Income", iTotalIncome);


	int iExpenses = pTreasury->GetExpensePerTurnUnitMaintenance() + pTreasury->GetBuildingGoldMaintenance() + pTreasury->GetImprovementGoldMaintenance();
	AppendToLog(strHeader, strLog, "Unit Maint", pTreasury->GetExpensePerTurnUnitMaintenance());
	AppendToLog(strHeader, strLog, "Build Maint", pTreasury->GetBuildingGoldMaintenance());
	AppendToLog(strHeader, strLog, "Improve Maint", pTreasury->GetImprovementGoldMaintenance());
	AppendToLog(strHeader, strLog, "Total Expenses", iExpenses);
	AppendToLog(strHeader, strLog, "Net GPT", iTotalIncome - iExpenses);

	if(bBuildHeader)
	{
		pLog->Msg(strHeader);
	}
	pLog->Msg(strLog);
}

void CvEconomicAI::LogCityMonitor()
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


	CvPlotsVector& aiPlots = m_pPlayer->GetPlots();

	CvString strLog;


	CvString strPlayerName;
	strPlayerName = m_pPlayer->getCivilizationShortDescription();
	CvString strLogName;


	if(GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "EconomicCityMonitorLog_" + strPlayerName + ".csv";
	}
	else
	{
		strLogName = "EconomicCityMonitorLog.csv";
	}

	FILogFile* pLog;
	pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

	CvString str;

	float fRatio;


	int iLoopCity = 0;
	CvCity* pLoopCity = NULL;

	FFastVector<int> aiCityYields;
	aiCityYields.push_back_copy(-1, NUM_YIELD_TYPES);

	FFastVector<int> aiSpecialistsYields;
	aiSpecialistsYields.push_back_copy(-1, NUM_YIELD_TYPES);

	for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iLoopCity))
	{
		for(uint ui = 0; ui < aiCityYields.size(); ui++)
		{
			aiCityYields[ui] = 0;
		}

		for(uint ui = 0; ui < aiSpecialistsYields.size(); ui++)
		{
			aiSpecialistsYields[ui] = 0;
		}

		strHeader = "";
		strLog = "";


		AppendToLog(strHeader, strLog, "Civ Name", strPlayerName);


		AppendToLog(strHeader, strLog, "Turn", GC.getGame().getGameTurn());


		str = pLoopCity->getName();
		AppendToLog(strHeader, strLog, "City Name", str);


		AppendToLog(strHeader, strLog, "Population", pLoopCity->getPopulation());


		for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
			aiCityYields[ui] = pLoopCity->getYieldRate((YieldTypes)ui, false);
			switch(ui)
			{
			case YIELD_FOOD:
				AppendToLog(strHeader, strLog, "food", aiCityYields[ui]);
				break;
			case YIELD_PRODUCTION:
				AppendToLog(strHeader, strLog, "production", aiCityYields[ui]);
				break;
			case YIELD_SCIENCE:
				AppendToLog(strHeader, strLog, "science", aiCityYields[ui]);
				break;
			case YIELD_GOLD:
				AppendToLog(strHeader, strLog, "city gold", aiCityYields[ui]);
				break;
			case YIELD_CULTURE:
				AppendToLog(strHeader, strLog, "culture", aiCityYields[ui]);
				break;
			case YIELD_FAITH:
				AppendToLog(strHeader, strLog, "faith", aiCityYields[ui]);
				break;
			}
		}


		for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
			fRatio = 0.0f;
			if(pLoopCity->getPopulation() > 0)
			{
				fRatio = aiCityYields[ui] / (float)pLoopCity->getPopulation();
			}

			switch(ui)
			{
			case YIELD_FOOD:
				AppendToLog(strHeader, strLog, "food / pop", fRatio);
				break;
			case YIELD_PRODUCTION:
				AppendToLog(strHeader, strLog, "production / pop", fRatio);
				break;
			case YIELD_SCIENCE:
				AppendToLog(strHeader, strLog, "science / pop", fRatio);
				break;
			case YIELD_GOLD:
				AppendToLog(strHeader, strLog, "gold / pop", fRatio);
				break;
			case YIELD_CULTURE:
				AppendToLog(strHeader, strLog, "culture / pop", fRatio);
				break;
			case YIELD_FAITH:
				AppendToLog(strHeader, strLog, "faith / pop", fRatio);
				break;
			}
		}


		for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
			aiSpecialistsYields[ui] = pLoopCity->getExtraSpecialistYield((YieldTypes)ui);
			switch(ui)
			{
			case YIELD_FOOD:
				AppendToLog(strHeader, strLog, "food specialist", aiSpecialistsYields[ui]);
				break;
			case YIELD_PRODUCTION:
				AppendToLog(strHeader, strLog, "production specialist", aiSpecialistsYields[ui]);
				break;
			case YIELD_SCIENCE:
				AppendToLog(strHeader, strLog, "science specialist", aiSpecialistsYields[ui]);
				break;
			case YIELD_GOLD:
				AppendToLog(strHeader, strLog, "gold specialist", aiSpecialistsYields[ui]);
				break;
			case YIELD_CULTURE:
				AppendToLog(strHeader, strLog, "culture specialist", aiSpecialistsYields[ui]);
				break;
			case YIELD_FAITH:
				AppendToLog(strHeader, strLog, "faith specialist", aiSpecialistsYields[ui]);
				break;
			}
		}


		for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
			fRatio = 0.0f;
			if(aiCityYields[ui] > 0)
			{
				fRatio = aiSpecialistsYields[ui] / (float)aiCityYields[ui];
			}
			switch(ui)
			{
			case YIELD_FOOD:
				AppendToLog(strHeader, strLog, "food specialist / food", fRatio);
				break;
			case YIELD_PRODUCTION:
				AppendToLog(strHeader, strLog, "production specialist / production", fRatio);
				break;
			case YIELD_SCIENCE:
				AppendToLog(strHeader, strLog, "science specialist / science", fRatio);
				break;
			case YIELD_GOLD:
				AppendToLog(strHeader, strLog, "gold specialist / gold", fRatio);
				break;
			case YIELD_CULTURE:
				AppendToLog(strHeader, strLog, "culture specialist / culture", fRatio);
				break;
			case YIELD_FAITH:
				AppendToLog(strHeader, strLog, "faith specialist / faith", fRatio);
				break;
			}
		}



		int iTiles = 0;
		int iWorkedTiles = 0;
		int iImprovedTiles = 0;
		for(uint uiPlotIndex = 0; uiPlotIndex < aiPlots.size(); uiPlotIndex++)
		{

			if(aiPlots[uiPlotIndex] == -1)
			{
				break;
			}

			CvPlot* pPlot = GC.getMap().plotByIndex(aiPlots[uiPlotIndex]);
			if(!pPlot)
			{
				continue;
			}


			if(pPlot->getPlotCity())
			{
				continue;
			}

			if(!pLoopCity->GetCityCitizens()->IsCanWork(pPlot))
			{
				continue;
			}

			iTiles++;

			if(pPlot->getWorkingCity() == pLoopCity)
			{
				iWorkedTiles++;
			}

			if(pPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				iImprovedTiles++;
			}
		}

		AppendToLog(strHeader, strLog, "Tiles", iTiles);
		AppendToLog(strHeader, strLog, "Worked (non-city) tiles", iWorkedTiles);
		AppendToLog(strHeader, strLog, "Improved tiles", iImprovedTiles);



		fRatio = 0.0f;
		if(iWorkedTiles > 0)
		{
			fRatio = iImprovedTiles / (float)iWorkedTiles;
		}
		AppendToLog(strHeader, strLog, "improved / worked", fRatio);

		if(bBuildHeader)
		{
			bBuildHeader = false;
			pLog->Msg(strHeader);
		}
		pLog->Msg(strLog);
	}
}




void CvEconomicAI::DoHurry()
{
	int iLoop = 0;
	OrderData* pOrder = 0;

	CvCity* pLoopCity = 0;
	int iTurnsSaved = 0;
	int iHurryAmount = 0;
	int iHurryAmountAvailable = 0;
#ifdef AUI_WARNING_FIXES
	uint iI = 0;
#else
	int iI = 0;
#endif

	CvCity* pBestHurryCity = NULL;
	int iBestHurryTurnsSaved = 0;
	int iBestHurryAmount = 0;
	int iBestHurryAmountAvailable = 0;
	HurryTypes eBestHurryType = NO_HURRY;


	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{

		pOrder = pLoopCity->getOrderFromQueue(0);


		if(pOrder != NULL && pOrder->bRush)
		{
			for(iI = 0; iI < GC.getNumHurryInfos(); iI++)
			{
				iHurryAmount = 0;

				if(pLoopCity->canHurry((HurryTypes)iI) &&
					pLoopCity->hurryProduction((HurryTypes)iI) > 0)
				{
					iTurnsSaved = pLoopCity->getProductionTurnsLeft() - 1;
					if(iTurnsSaved > 0)
					{
#ifdef AUI_WARNING_FIXES
						CvHurryInfo* pHurryInfo = GC.getHurryInfo((HurryTypes)iI);
						if (!pHurryInfo)
							continue;
						if (pHurryInfo->getGoldPerProduction() > 0)
#else
						if(GC.getHurryInfo((HurryTypes)iI)->getGoldPerProduction() > 0)
#endif
						{




						}
#ifdef AUI_WARNING_FIXES
						else if (pHurryInfo->getProductionPerPopulation() > 0)
#else
						else if(GC.getHurryInfo((HurryTypes)iI)->getProductionPerPopulation() > 0)
#endif
						{

							if(m_pPlayer->IsEmpireUnhappy())
							{
								iHurryAmount = pLoopCity->hurryPopulation((HurryTypes)iI);
								iHurryAmountAvailable = pLoopCity->getPopulation();
							}
						}


						if(iHurryAmount > 0 && iTurnsSaved >= iBestHurryTurnsSaved)
						{
							iBestHurryTurnsSaved = iTurnsSaved;
							iBestHurryAmount = iHurryAmount;
							iBestHurryAmountAvailable = iHurryAmountAvailable;
							pBestHurryCity = pLoopCity;
							eBestHurryType = (HurryTypes)iI;
						}
					}
				}
			}
		}
	}


	if(pBestHurryCity != NULL)
	{
		pBestHurryCity->hurry(eBestHurryType);
		pBestHurryCity->GetCityStrategyAI()->LogHurry(eBestHurryType, iBestHurryAmount, iBestHurryAmountAvailable, iBestHurryTurnsSaved);
	}
}


void CvEconomicAI::DoPlotPurchases()
{
	CvCity* pLoopCity = 0;
	CvCity* pBestCity = NULL;
	int iBestX = -1;
	int iBestY = -1;
	int iTempX = 0, iTempY = 0;

	int iScore = 0;
	int iLoop = 0;


	if(m_pPlayer->isMinorCiv())
	{
		return;
	}


	MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
	if(eStrategyAtWar != NO_MILITARYAISTRATEGY)
	{
		if(m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyAtWar))
		{
			return;
		}
	}


	int iBestScore =         GC.getAI_GOLD_PRIORITY_MINIMUM_PLOT_BUY_VALUE();
	int iCurrentCost = m_pPlayer->GetBuyPlotCost();
	int iGoldForHalfCost =          GC.getAI_GOLD_BALANCE_TO_HALVE_PLOT_BUY_MINIMUM();
	int iBalance = m_pPlayer->GetTreasury()->GetGold();



	if(iCurrentCost < iBalance && iGoldForHalfCost > iCurrentCost)
	{

		int iDiscountPercent = 50 * (iBalance - iCurrentCost) / (iGoldForHalfCost - iCurrentCost);
		iBestScore = iBestScore - (iBestScore * iDiscountPercent / 100);


		for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			if(pLoopCity->CanBuyAnyPlot())
			{
				iScore = pLoopCity->GetBuyPlotScore(iTempX, iTempY);

				if(iScore > iBestScore)
				{
					pBestCity = pLoopCity;
					iBestScore = iScore;
					iBestX = iTempX;
					iBestY = iTempY;
				}
			}
		}

		if(pBestCity != NULL)
		{
			if(iBestX != -1 && iBestY != -1)
			{
				int iCost = pBestCity->GetBuyPlotCost(iBestX, iBestY);

				if(CanWithdrawMoneyForPurchase(PURCHASE_TYPE_TILE, iCost, iBestScore))
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Buying plot, X: %d, Y: %d, Cost: %d, Balance (before buy): %d, Priority: %d", iBestX, iBestY,
						                    iCost, m_pPlayer->GetTreasury()->GetGold(), iBestScore);
						m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
					}
					pBestCity->BuyPlot(iBestX, iBestY);
				}
			}
		}
	}
}



void CvEconomicAI::DoReconState()
{
#ifdef AUI_WARNING_FIXES
	uint iPlotLoop;
	int iDirectionLoop, iUnitLoop;
#else
	int iPlotLoop, iDirectionLoop, iUnitLoop;
#endif
	CvPlot* pPlot;
	CvPlot* pAdjacentPlot;
	CvUnit* pLoopUnit;
	bool bIsLand;
	bool bIsCoastalWater;

	if(GetPlayer()->isMinorCiv())
	{
		m_eReconState = RECON_STATE_ENOUGH;
		m_eNavalReconState = RECON_STATE_ENOUGH;
		return;
	}



	int iNumLandPlotsRevealed = 1;
	int iNumLandPlotsWithAdjacentFog = 1;


	int iNumCoastalTilesRevealed = 1;
	int iNumCoastalTilesWithAdjacentFog = 1;

	bool bNeedToLookAtDeepWaterAlso = GET_TEAM(m_pPlayer->getTeam()).canEmbarkAllWaterPassage();


	for(iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

		if(pPlot->isRevealed(m_pPlayer->getTeam()))
		{
			bIsLand = false;
			bIsCoastalWater = false;


			if(!pPlot->isWater())
			{
				bIsLand = true;
				iNumLandPlotsRevealed++;
			}
			else if(pPlot->isShallowWater() || bNeedToLookAtDeepWaterAlso)
			{
				bIsCoastalWater = true;
				iNumCoastalTilesRevealed++;
			}


			for(iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
			{
				pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iDirectionLoop));

				if(pAdjacentPlot != NULL)
				{

					if(pAdjacentPlot->isWater())
					{

						if((bNeedToLookAtDeepWaterAlso || pAdjacentPlot->isShallowWater()) && !pAdjacentPlot->isRevealed(m_pPlayer->getTeam()))
						{
							iNumCoastalTilesWithAdjacentFog++;
							break;
						}
					}
					else
					{

						if(!pAdjacentPlot->isRevealed(m_pPlayer->getTeam()))
						{
							iNumLandPlotsWithAdjacentFog++;
							break;
						}
					}
				}
			}
		}
	}




	int iNumExploringUnits = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_EXPLORE, true, false) + m_iExplorersDisbanded;
	int iStrategyWeight =         GC.getAI_STRATEGY_EARLY_EXPLORATION_STARTING_WEIGHT();
	int iWeightThreshold = 110;
	iWeightThreshold -= m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RECON")) *
	                           GC.getAI_STRATEGY_EARLY_EXPLORATION_WEIGHT_PER_FLAVOR();


	if(iWeightThreshold > 100)
	{
		iWeightThreshold = 100;
	}

	iStrategyWeight *= iNumLandPlotsWithAdjacentFog;
	int iNumExplorerDivisor = iNumExploringUnits +       GC.getAI_STRATEGY_EARLY_EXPLORATION_EXPLORERS_WEIGHT_DIVISOR();
	iStrategyWeight /= (iNumExplorerDivisor * iNumExplorerDivisor);
	iStrategyWeight /= (int)sqrt((double)iNumLandPlotsRevealed);

	if(iStrategyWeight > iWeightThreshold)
	{
		m_eReconState = RECON_STATE_NEEDED;
	}
	else
	{
		if(iStrategyWeight > (iWeightThreshold / 4))
		{
			m_eReconState = RECON_STATE_NEUTRAL;
		}
		else
		{
			m_eReconState = RECON_STATE_ENOUGH;


			bool bSkipFirst = GC.getGame().getGameTurn() < 100;
			for(pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
			{
				if(pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE && pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_ATTACK))
				{
					if(bSkipFirst)
					{
						bSkipFirst = false;
					}
					else
					{
						pLoopUnit->AI_setUnitAIType(UNITAI_ATTACK);
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Assigning exploring %s back to attack AI, X: %d, Y: %d", pLoopUnit->getName().GetCString(), pLoopUnit->getX(), pLoopUnit->getY());
							m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
						}
					}
				}
			}
		}
	}




	CvCity* pLoopCity;
	int iCityLoop;
	bool bFoundCoastalCity = false;
	for(pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL && !bFoundCoastalCity; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
	{
		if(pLoopCity->isCoastal())
		{
			bFoundCoastalCity = true;
		}
	}

	if(!bFoundCoastalCity)
	{
		m_eNavalReconState = RECON_STATE_ENOUGH;
	}

	else
	{

		iNumExploringUnits = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_EXPLORE_SEA, true, true);
		iStrategyWeight =         GC.getAI_STRATEGY_EARLY_EXPLORATION_STARTING_WEIGHT();
		iWeightThreshold = 110;
		iWeightThreshold -= m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL_RECON")) *
		                           GC.getAI_STRATEGY_EARLY_EXPLORATION_WEIGHT_PER_FLAVOR();


		if(iWeightThreshold > 100)
		{
			iWeightThreshold = 100;
		}

		iStrategyWeight *= iNumCoastalTilesWithAdjacentFog;
		iNumExplorerDivisor = iNumExploringUnits +       GC.getAI_STRATEGY_EARLY_EXPLORATION_EXPLORERS_WEIGHT_DIVISOR();
		iStrategyWeight /= (iNumExplorerDivisor * iNumExplorerDivisor);
		iStrategyWeight /= (int)sqrt((double)iNumCoastalTilesRevealed);

		if(iStrategyWeight > iWeightThreshold                                                                       )
		{
			m_eNavalReconState = RECON_STATE_NEEDED;
		}
		else
		{
			if(iStrategyWeight > (iWeightThreshold / 4))
			{
				m_eNavalReconState = RECON_STATE_NEUTRAL;
			}
			else
			{
				m_eNavalReconState = RECON_STATE_ENOUGH;


				bool bSkipFirst = (m_eNavalReconState == RECON_STATE_NEUTRAL);
				for(pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
				{
					if(pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA && pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_ATTACK_SEA))
					{
						if(bSkipFirst)
						{
							bSkipFirst = false;
						}
						else
						{
							pLoopUnit->AI_setUnitAIType(UNITAI_ATTACK_SEA);
							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("Assigning naval explorer back to attack sea AI to %s, X: %d, Y: %d", pLoopUnit->getName().GetCString(), pLoopUnit->getX(), pLoopUnit->getY());
								m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
							}
						}
					}
				}
			}
		}
	}
}


void CvEconomicAI::DoAntiquitySites()
{
	int iNumSites = 0;
#ifdef AUI_WARNING_FIXES
	uint iPlotLoop;
#else
	int iPlotLoop;
#endif
	CvPlot *pPlot;
	ResourceTypes eArtifactResourceType = static_cast<ResourceTypes>(GC.getARTIFACT_RESOURCE());
	ResourceTypes eHiddenArtifactResourceType = static_cast<ResourceTypes>(GC.getHIDDEN_ARTIFACT_RESOURCE());

	for(iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);
		if(pPlot->isRevealed(m_pPlayer->getTeam()))
		{
			if (pPlot->getResourceType(m_pPlayer->getTeam()) == eArtifactResourceType ||
				pPlot->getResourceType(m_pPlayer->getTeam()) == eHiddenArtifactResourceType)
			{
				iNumSites++;
			}
		}
	}

	m_iVisibleAntiquitySites = iNumSites;
}

void CvEconomicAI::DisbandExtraWorkers()
{

	EconomicAIStrategyTypes eStrategyLosingMoney = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY");
	bool bInDeficit = m_pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyLosingMoney);

	int iGoldSpentOnUnits = m_pPlayer->GetTreasury()->GetExpensePerTurnUnitMaintenance();
	int iAverageGoldPerUnit = iGoldSpentOnUnits / (max(1,m_pPlayer->getNumUnits()));

	if(!bInDeficit && iAverageGoldPerUnit <= 4)
	{
		return;
	}



	double fWorstCaseRatio = 0.25;
	int iNumWorkers = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_WORKER, true, false);
	int iNumCities = m_pPlayer->getNumCities();

	double fCurrentRatio = iNumWorkers / (double)iNumCities;
	if(fCurrentRatio <= fWorstCaseRatio || iNumWorkers == 1)
	{
		return;
	}

	const CvPlotsVector& aiPlots = m_pPlayer->GetPlots();
	int iNumValidPlots = 0;
	int iNumImprovedPlots = 0;
	for(uint ui = 0; ui < aiPlots.size(); ui++)
	{
		if(aiPlots[ui] == -1)
		{
			continue;
		}

		const CvPlot* pPlot = GC.getMap().plotByIndex(aiPlots[ui]);
		if(!pPlot)
		{
			continue;
		}

		if(pPlot->isWater() || pPlot->isImpassable() || pPlot->isMountain() || pPlot->isCity())
		{
			continue;
		}

		iNumValidPlots++;

		if(pPlot->getImprovementType() != NO_IMPROVEMENT && !pPlot->IsImprovementPillaged())
		{
			iNumImprovedPlots++;
		}
	}


	if(iNumValidPlots <= 0)
	{
		return;
	}

	int iNumUnimprovedPlots = iNumValidPlots - iNumImprovedPlots;


	double fRatio = iNumImprovedPlots / (double)iNumValidPlots;
	if(fRatio < 2/(double)3)
	{
		return;
	}

	int iWorkersPerUnimprovedPlot = 5;
	int iMinWorkers = iNumUnimprovedPlots / iWorkersPerUnimprovedPlot;
	if((iNumUnimprovedPlots % iWorkersPerUnimprovedPlot) > 0)
	{
		iMinWorkers += 1;
	}

	CvCity* pCapital = m_pPlayer->getCapitalCity();
	if(!pCapital)
	{
		return;
	}

	int iLoop;
	CvCity* pCity;
	for(pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pCity == pCapital)
		{
			continue;
		}

		if(pCapital->area() == pCity->area() && !pCity->IsRouteToCapitalConnected())
		{
			iMinWorkers += 1;
		}
	}


	if(iNumWorkers <= iMinWorkers)
	{
		return;
	}

	m_iLastTurnWorkerDisbanded = GC.getGame().getGameTurn();

	CvUnit* pUnit = FindWorkerToScrap();
	if(!pUnit)
	{
		return;
	}

	pUnit->scrap();
	LogScrapUnit(pUnit, iNumWorkers, iNumCities, iNumImprovedPlots, iNumValidPlots);
}
void CvEconomicAI::DisbandExtraArchaeologists(){
	int iNumSites = GC.getGame().GetNumArchaeologySites();
	double dMaxRatio = .5;
	int iNumArchaeologists = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_ARCHAEOLOGIST, true);
	PolicyTypes eExpFinisher = (PolicyTypes) GC.getInfoTypeForString("POLICY_EXPLORATION_FINISHER", true                );
	if (eExpFinisher != NO_POLICY)	
	{
		if (m_pPlayer->GetPlayerPolicies()->HasPolicy(eExpFinisher))
		{
			iNumSites += GC.getGame().GetNumHiddenArchaeologySites();
		}
	}
	
	CvUnit* pUnit;
	UnitTypes eArch = (UnitTypes) GC.getInfoTypeForString("UNIT_ARCHAEOLOGIST", true                );
	if(eArch == NO_UNIT){
		return;
	}
	if ((double)iNumSites * dMaxRatio + 1 < iNumArchaeologists ){
		pUnit = FindArchaeologistToScrap();
	
		if(!pUnit)
		{
			return;
		}
	
		pUnit->scrap();
		LogScrapUnit(pUnit, iNumArchaeologists, iNumSites, 0, 0);
	}
}


void CvEconomicAI::UpdatePlots()
{

	for(uint ui = 0; ui < m_aiExplorationPlots.size(); ui++)
	{
		m_aiExplorationPlots[ui] = -1;
		m_aiExplorationPlotRatings[ui] = -1;
	}

	for(uint ui = 0; ui < m_aiGoodyHutPlots.size(); ui++)
	{
		m_aiGoodyHutPlots[ui] = -1;
		m_aiGoodyHutUnitAssignments[ui].Clear();
	}


	int iTotalX = 0;
	int iTotalY = 0;
	int iCityCount = 0;
	int iLoopCity = 0;
	CvCity* pLoopCity = NULL;
	for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
	{
		iTotalX += pLoopCity->getX();
		iTotalY += pLoopCity->getY();
		iCityCount++;
	}

	int iCivCenterX = -1;
	int iCivCenterY = -1;
	if(iCityCount > 0)
	{
		iCivCenterX = iTotalX / iCityCount;
		iCivCenterY = iTotalY / iCityCount;
	}

	uint uiExplorationPlotIndex = 0;
	uint uiGoodyHutPlotIndex = 0;
	TeamTypes ePlayerTeam = m_pPlayer->getTeam();

	CvPlot* pPlot;
#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < GC.getMap().numPlots(); i++)
#else
	for(int i = 0; i < GC.getMap().numPlots(); i++)
#endif
	{
		pPlot = GC.getMap().plotByIndexUnchecked(i);
		if(pPlot == NULL)
		{
			continue;
		}

		if(!pPlot->isRevealed(ePlayerTeam))
		{
			continue;
		}

		if(pPlot->isRevealedGoody(ePlayerTeam) && !pPlot->isVisibleEnemyUnit(m_pPlayer->GetID()))
		{
			if(m_aiGoodyHutPlots.size() <= uiGoodyHutPlotIndex)
			{
				m_aiGoodyHutPlots.push_back(-1);
				m_aiGoodyHutUnitAssignments.push_back(GoodyHutUnitAssignment(-1, -1));
			}
			m_aiGoodyHutPlots[uiGoodyHutPlotIndex] = i;
			m_aiGoodyHutUnitAssignments[uiGoodyHutPlotIndex].Clear();
			++uiGoodyHutPlotIndex;
		}
		if(pPlot->HasBarbarianCamp())
		{
			if(pPlot->getNumDefenders(BARBARIAN_PLAYER) == 0)
			{
				if(m_aiGoodyHutPlots.size() <= uiGoodyHutPlotIndex)
				{
					m_aiGoodyHutPlots.push_back(-1);
					m_aiGoodyHutUnitAssignments.push_back(GoodyHutUnitAssignment(-1, -1));
				}
				m_aiGoodyHutPlots[uiGoodyHutPlotIndex] = i;
				m_aiGoodyHutUnitAssignments[uiGoodyHutPlotIndex].Clear();
				++uiGoodyHutPlotIndex;
			}
		}

		DomainTypes eDomain = DOMAIN_LAND;
		if(pPlot->isWater())
		{
			eDomain = DOMAIN_SEA;
		}

		int iScore = ScoreExplorePlot(pPlot, ePlayerTeam, 1, eDomain);
		if(iScore <= 0)
		{
			continue;
		}


		if(m_aiExplorationPlots.size() <= uiExplorationPlotIndex)
		{
			m_aiExplorationPlots.push_back(-1);
			m_aiExplorationPlotRatings.push_back(-1);
		}

		m_aiExplorationPlots[uiExplorationPlotIndex] = i;
		m_aiExplorationPlotRatings[uiExplorationPlotIndex] = iScore;
		uiExplorationPlotIndex++;
	}




	CvUnit* pLoopUnit;
	int iLoopUnit;
	m_apExplorers.clear();
	for(pLoopUnit = m_pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoopUnit))
	{

		if(m_pPlayer->isHuman() && !pLoopUnit->IsAutomated())
		{
			continue;
		}

		if(pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA)
		{
			continue;
		}

		if(pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE && pLoopUnit->GetMissionAIType() != MISSIONAI_EXPLORE)
		{
			continue;
		}

		if(pLoopUnit->getArmyID() != FFreeList::INVALID_INDEX)
		{
			continue;
		}

		m_apExplorers.push_back(pLoopUnit);
	}

	if(m_apExplorers.size() >= m_aiGoodyHutPlots.size())
	{
		AssignExplorersToHuts();
	}
	else
	{
		AssignHutsToExplorers();
	}

	m_bExplorationPlotsDirty = false;
}

#define PATH_PLAN_LAST
typedef CvWeightedVector<CvUnit*, 50, true> WeightedUnitVector;

void CvEconomicAI::AssignExplorersToHuts()
{
#if defined(PATH_PLAN_LAST)
	WeightedUnitVector aBestUnitList;
	aBestUnitList.reserve(m_apExplorers.size());
#endif

	CvTwoLayerPathFinder& kPathFinder = GC.getPathFinder();
	for(uint uiGoodyPlots = 0; uiGoodyPlots < m_aiGoodyHutPlots.size(); uiGoodyPlots++)
	{
		if(m_aiGoodyHutPlots[uiGoodyPlots] == -1)
		{
			continue;
		}

		CvPlot* pGoodyPlot = GC.getMap().plotByIndex(m_aiGoodyHutPlots[uiGoodyPlots]);
#if !defined(PATH_PLAN_LAST)
		int iClosestEstimateTurns = MAX_INT;
#endif
		int iUnitID = NO_UNIT;
		int iStepPlotID = -1;

		aBestUnitList.clear();

		for(uint uiExplorer = 0; uiExplorer < m_apExplorers.size(); uiExplorer++)
		{
			CvUnit* pUnit = m_apExplorers[uiExplorer];

			int iDistance = plotDistance(pUnit->getX(), pUnit->getY(), pGoodyPlot->getX(), pGoodyPlot->getY());

#if defined(PATH_PLAN_LAST)
			if(pUnit->maxMoves() >= 1)
			{
				int iEstimateTurns = iDistance / (pUnit->maxMoves() / GC.getMOVE_DENOMINATOR());
				aBestUnitList.push_back(pUnit, iEstimateTurns);
			}
#else
			int iEstimateTurns = MAX_INT;
			if(pUnit->maxMoves() >= 1)
			{
				iEstimateTurns = iDistance / (pUnit->maxMoves() / GC.getMOVE_DENOMINATOR());
			}

			if(iEstimateTurns < iClosestEstimateTurns)
			{

				bool bCanFindPath = kPathFinder.GenerateUnitPath(pUnit, pUnit->getX(), pUnit->getY(), pGoodyPlot->getX(), pGoodyPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER           , true          );
				if(bCanFindPath)
				{
					iClosestEstimateTurns = iEstimateTurns;
					iUnitID = pUnit->GetID();
					CvPlot* pPlot = kPathFinder.GetPathEndTurnPlot();
					if(pPlot)
						iStepPlotID = pPlot->GetPlotIndex();
				}
			}
#endif
		}

#if defined(PATH_PLAN_LAST)
		uint uiListSize;
		if ((uiListSize = aBestUnitList.size()) > 0)
		{
			aBestUnitList.SortItems();
			for (uint i = uiListSize; i--; )
			{
				CvUnit* pUnit = aBestUnitList.GetElement(i);
				bool bCanFindPath = kPathFinder.GenerateUnitPath(pUnit, pUnit->getX(), pUnit->getY(), pGoodyPlot->getX(), pGoodyPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER           , true          );
				if(bCanFindPath)
				{
					iUnitID = pUnit->GetID();

					CvPlot* pPlot = kPathFinder.GetPathEndTurnPlot();
					if(pPlot)
						iStepPlotID = pPlot->GetPlotIndex();
					break;
				}
			}
		}
#endif

		if(iUnitID != NO_UNIT)
		{
			m_aiGoodyHutUnitAssignments[uiGoodyPlots] = GoodyHutUnitAssignment(iUnitID, iStepPlotID);

			FFastVector<CvUnit*> tempExplorers = m_apExplorers;
			m_apExplorers.clear();
			for(uint uiExplorer = 0; uiExplorer < tempExplorers.size(); uiExplorer++)
			{
				if(tempExplorers[uiExplorer]->GetID() != iUnitID)
				{
					m_apExplorers.push_back(tempExplorers[uiExplorer]);
				}
			}
		}
	}
}


void CvEconomicAI::AssignHutsToExplorers()
{
	FFastVector<unsigned int> aiHutIndices;


	for(uint uiGoodyPlots = 0; uiGoodyPlots < m_aiGoodyHutPlots.size(); uiGoodyPlots++)
	{
		aiHutIndices.push_back(uiGoodyPlots);
	}

	for(uint uiExplorer = 0; uiExplorer < m_apExplorers.size(); uiExplorer++)
	{
		CvUnit* pUnit = m_apExplorers[uiExplorer];
		uint uiHutIndex = MAX_INT;
		int iClosestEstimateTurns = MAX_INT;

		for(uint uiGoody = 0; uiGoody < aiHutIndices.size(); uiGoody++)
		{
			if(m_aiGoodyHutPlots[aiHutIndices[uiGoody]] == -1)
			{
				continue;
			}

			CvPlot* pGoodyPlot = GC.getMap().plotByIndex(m_aiGoodyHutPlots[aiHutIndices[uiGoody]]);
			int iDistance = plotDistance(pUnit->getX(), pUnit->getY(), pGoodyPlot->getX(), pGoodyPlot->getY());

			int iEstimateTurns = MAX_INT;
			if(pUnit->maxMoves() >= 1)
			{
				iEstimateTurns = iDistance / (pUnit->maxMoves() / GC.getMOVE_DENOMINATOR());
			}

			if(iEstimateTurns < iClosestEstimateTurns)
			{

				bool bCanFindPath = GC.getPathFinder().GenerateUnitPath(pUnit, pUnit->getX(), pUnit->getY(), pGoodyPlot->getX(), pGoodyPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER           , true          );
				if(bCanFindPath)
				{
					iClosestEstimateTurns = iEstimateTurns;
					uiHutIndex = aiHutIndices[uiGoody];
				}
			}
		}

		if(uiHutIndex != MAX_INT)
		{
			m_aiGoodyHutUnitAssignments[uiHutIndex] = GoodyHutUnitAssignment( pUnit->GetID(), -1);

			FFastVector<unsigned int> tempHutIndices = aiHutIndices;
			aiHutIndices.clear();
			for(uint uiHut = 0; uiHut < tempHutIndices.size(); uiHut++)
			{
				if(tempHutIndices[uiHut] != uiHutIndex)
				{
					aiHutIndices.push_back(tempHutIndices[uiHut]);
				}
			}
		}
	}
}

CvUnit* CvEconomicAI::FindWorkerToScrap()
{
	CvUnit* pLoopUnit = NULL;
	int iUnitLoop = 0;


	for(pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
	{
		if(!pLoopUnit)
		{
			continue;
		}
		UnitTypes eWorker = (UnitTypes) GC.getInfoTypeForString("UNIT_WORKER");
		if(pLoopUnit->getDomainType() == DOMAIN_LAND && pLoopUnit->getUnitType() == eWorker && !pLoopUnit->IsCombatUnit() && pLoopUnit->getSpecialUnitType() == NO_SPECIALUNIT)
		{
			return pLoopUnit;
		}
	}

	return NULL;
}

CvUnit* CvEconomicAI::FindArchaeologistToScrap()
{
	CvUnit* pLoopUnit = NULL;
	int iUnitLoop = 0;


	for(pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
	{
		if(!pLoopUnit)
		{
			continue;
		}
		UnitTypes eArch = (UnitTypes) GC.getInfoTypeForString("UNIT_ARCHAEOLOGIST", true);
		if(pLoopUnit->getUnitType() == eArch)
		{
			return pLoopUnit;
		}
	}

	return NULL;
}


void CvEconomicAI::LogStrategy(EconomicAIStrategyTypes eStrategy, bool bValue)
{
	if(GC.getLogging() && GC.getAILogging())
	{

		CvString playerName = GetPlayer()->getCivilizationShortDescription();


		CvString strLogName = "FlavorAILog.csv";
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "FlavorAILog_" + playerName + ".csv";
		}

		FILogFile* pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		CvString strBaseString;
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		CvString strOutBuf = strBaseString;


		CvEconomicAIStrategyXMLEntry* pEconomicAIStrategyEntry = GC.getEconomicAIStrategyInfo(eStrategy);
		if(pEconomicAIStrategyEntry != NULL)
		{
			CvString strTemp;
			strTemp.Format("%s, %d", pEconomicAIStrategyEntry->GetType(), bValue);
			strOutBuf += strTemp;
		}

		pLog->Msg(strOutBuf);



		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "TechAILog_" + playerName + ".csv";
			pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);
			pLog->Msg(strOutBuf);

			CvString cityName;

			CvCity* pLoopCity;
			int iLoop;

			for(pLoopCity = GetPlayer()->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iLoop))
			{
				cityName = pLoopCity->getName();
				strLogName = "CityStrategyAILog_" + playerName + "_" + cityName + ".csv";
				pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);
				pLog->Msg(strOutBuf);
			}
		}
		else
		{
			pLog = LOGFILEMGR.GetLog("CityStrategyAILog.csv", FILogFile::kDontTimeStamp);
			pLog->Msg(strOutBuf);
			pLog = LOGFILEMGR.GetLog("TechAILog.csv", FILogFile::kDontTimeStamp);
			pLog->Msg(strOutBuf);
		}
	}
}


void CvEconomicAI::LogScrapUnit(UnitHandle pUnit, int iNumWorkers, int iNumCities, int iNumImprovedPlots, int iNumValidPlots)
{
	if(!GC.getLogging() || !GC.getAILogging())
	{
		return;
	}

	CvString strLogString;
	strLogString.Format("Disbanding worker. %s, X: %d, Y: %d, iNumWorkers: %d, iNumCities: %d, improved/valid plots: %d/%d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY(), iNumWorkers, iNumCities, iNumImprovedPlots, iNumValidPlots);
	m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
}

FDataStream& operator<<(FDataStream& saveTo, const CvPurchaseRequest& readFrom)
{
	saveTo << (int)readFrom.m_eType;
	saveTo << readFrom.m_iAmount;
	saveTo << readFrom.m_iPriority;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, CvPurchaseRequest& writeTo)
{
	int iTemp;
	loadFrom >> iTemp;
	writeTo.m_eType = (PurchaseType)iTemp;
	loadFrom >> writeTo.m_iAmount;
	loadFrom >> writeTo.m_iPriority;
	return loadFrom;
}









int EconomicAIHelpers::GetWeightThresholdModifier(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	int iWeightThresholdModifier = 0;


	for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		FlavorTypes eFlavor = (FlavorTypes) iFlavorLoop;
		int iPersonalityFlavor = pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor);
		int iStrategyFlavorMod = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy)->GetPersonalityFlavorThresholdMod(eFlavor);

		iWeightThresholdModifier += (iPersonalityFlavor * iStrategyFlavorMod);
	}

	return iWeightThresholdModifier;
}


bool EconomicAIHelpers::IsAreaSafeForQuickColony(int iAreaID, CvPlayer* pPlayer)
{
	if(iAreaID == -1)
	{
		return false;
	}


	CvCity* pCapitalCity = pPlayer->getCapitalCity();
	if(pCapitalCity != NULL)
	{
		if(iAreaID == pCapitalCity->getArea())
		{
			return false;
		}
	}

	CvArea* pArea = GC.getMap().getArea(iAreaID);
	int iBeginSearchX = pArea->getAreaBoundaries().m_iWestEdge;
	int iBeginSearchY = pArea->getAreaBoundaries().m_iSouthEdge;
	int iEndSearchX   = pArea->getAreaBoundaries().m_iEastEdge;
	int iEndSearchY   = pArea->getAreaBoundaries().m_iNorthEdge;

	for(int iPlotX = iBeginSearchX; iPlotX <= iEndSearchX; iPlotX++)
	{
		for(int iPlotY = iBeginSearchY; iPlotY <= iEndSearchY; iPlotY++)
		{
			CvPlot* pPlot = GC.getMap().plotCheckInvalid(iPlotX, iPlotY);
			if(!pPlot)
			{
				continue;
			}

			if(pPlot->getArea() != iAreaID)
			{
				continue;
			}

			if(pPlot->isVisibleEnemyUnit(pPlayer->GetID()))
			{
				return false;
			}
		}
	}
	return true;
}


bool EconomicAIHelpers::IsTestStrategy_NeedRecon(CvPlayer* pPlayer)
{

	MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
	if(eStrategyAtWar != NO_MILITARYAISTRATEGY)
	{
		if(pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyAtWar))
		{
			return false;
		}
	}

	return (pPlayer->GetEconomicAI()->GetReconState() == RECON_STATE_NEEDED);
}


bool EconomicAIHelpers::IsTestStrategy_EnoughRecon(CvPlayer* pPlayer)
{
	return (pPlayer->GetEconomicAI()->GetReconState() == RECON_STATE_ENOUGH);
}


bool EconomicAIHelpers::IsTestStrategy_ReallyNeedReconSea(CvPlayer* pPlayer)
{
	if(pPlayer->GetEconomicAI()->GetNavalReconState() == RECON_STATE_NEEDED)
	{
		if(GET_TEAM(pPlayer->getTeam()).canEmbarkAllWaterPassage())
		{
			CvUnit* pLoopUnit;
			CvCity* pLoopCity;
			int iLoop;


			for(pLoopUnit = pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iLoop))
			{
				if(pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA && !pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
				{
					return false;
				}
			}


			PromotionTypes eOceanImpassablePromotion = NO_PROMOTION;
#ifdef AUI_WARNING_FIXES
			for (uint iI = 0; iI < GC.getNumPromotionInfos(); iI++)
#else
			for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
#endif
			{
				const PromotionTypes eLoopPromotion = static_cast<PromotionTypes>(iI);
				CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eLoopPromotion);
				if(pkPromotionInfo)
				{
					if(pkPromotionInfo->GetTerrainImpassable(TERRAIN_OCEAN) && pkPromotionInfo->GetTerrainPassableTech(TERRAIN_OCEAN) == -1)
					{
						eOceanImpassablePromotion = eLoopPromotion;
						break;
					}
				}
			}


			for(pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop))
			{
				if(pLoopCity->isProductionUnit())
				{
					CvUnitEntry* pkUnitEntry = GC.getUnitInfo(pLoopCity->getProductionUnit());
					if(pkUnitEntry)
					{
						if(pkUnitEntry->GetDomainType() == DOMAIN_SEA && pkUnitEntry->GetDefaultUnitAIType() == UNITAI_EXPLORE_SEA)
						{
							if(!pkUnitEntry->GetFreePromotions(eOceanImpassablePromotion))
							{
								return false;
							}
						}
					}
				}
			}
			return true;
		}
		else if(GET_TEAM(pPlayer->getTeam()).canEmbark())
		{
			CvUnit* pLoopUnit;
			CvCity* pLoopCity;
			int iLoop;


			for(pLoopUnit = pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iLoop))
			{
				if(pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA)
				{
					return false;
				}
			}


			for(pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop))
			{
				if(pLoopCity->isProductionUnit())
				{
					CvUnitEntry* pkUnitEntry = GC.getUnitInfo(pLoopCity->getProductionUnit());
					if(pkUnitEntry)
					{
						if(pkUnitEntry->GetDomainType() == DOMAIN_SEA && pkUnitEntry->GetDefaultUnitAIType() == UNITAI_EXPLORE_SEA)
						{
							return false;
						}
					}
				}
			}
			return true;
		}
	}

	return false;
}


bool EconomicAIHelpers::IsTestStrategy_NeedReconSea(CvPlayer* pPlayer)
{

	MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_LOSING_WARS");
	if(eStrategyAtWar != NO_MILITARYAISTRATEGY)
	{
		if(pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyAtWar))
		{
			return false;
		}
	}

	return (pPlayer->GetEconomicAI()->GetNavalReconState() == RECON_STATE_NEEDED);
}


bool EconomicAIHelpers::IsTestStrategy_EnoughReconSea(CvPlayer* pPlayer)
{
	return (pPlayer->GetEconomicAI()->GetNavalReconState() == RECON_STATE_ENOUGH);
}


bool EconomicAIHelpers::IsTestStrategy_NavalMap(CvPlayer* pPlayer)
{
	if(pPlayer->isHuman())
	{
		return false;
	}

	return (GC.getMap().GetAIMapHint() & 1);
}


bool EconomicAIHelpers::IsTestStrategy_OffshoreExpansionMap(CvPlayer* pPlayer)
{
	if(pPlayer->isHuman())
	{
		return false;
	}

	return (GC.getMap().GetAIMapHint() & 4);
}


bool EconomicAIHelpers::IsTestStrategy_DevelopingReligion(CvPlayer* pPlayer)
{
	if(pPlayer->isHuman())
	{
		return false;
	}


	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	if(pReligions->HasCreatedReligion(pPlayer->GetID()))
	{
		return true;
	}


	if(pReligions->HasCreatedPantheon(pPlayer->GetID()))
	{
		if(pReligions->GetNumReligionsStillToFound() > 0)
		{
			return true;
		}
	}

	return false;
}


bool EconomicAIHelpers::IsTestStrategy_TechLeader(CvPlayer* pPlayer)
{
	if (pPlayer->isHuman())
	{
		return false;
	}


	if (pPlayer->GetEspionageAI()->m_iTurnEspionageStarted == -1)
	{
		return false;
	}

	int iMyTechNum = GET_TEAM(pPlayer->getTeam()).GetTeamTechs()->GetNumTechsKnown();

	int iNumOtherPlayers = 0;
	int iNumPlayersAheadInTech = 0;
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		PlayerTypes ePlayer = (PlayerTypes)ui;
		if(!GET_PLAYER(ePlayer).isAlive())
		{
			continue;
		}

		if (ePlayer == pPlayer->GetID())
		{
			continue;
		}

		iNumOtherPlayers++;
		int iNumTechs = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetTeamTechs()->GetNumTechsKnown();
		if (iNumTechs > iMyTechNum)
		{
			iNumPlayersAheadInTech++;
		}
	}

	if (iNumOtherPlayers > 0)
	{
		FlavorTypes eFlavorEspionage = NO_FLAVOR;
		for (int iFlavor = 0; iFlavor < GC.getNumFlavorTypes(); iFlavor++)
		{
			FlavorTypes eFlavor = (FlavorTypes)iFlavor;
			if (GC.getFlavorTypes(eFlavor) == "FLAVOR_ESPIONAGE")
			{
				eFlavorEspionage = eFlavor;
			}
		}
		CvAssertMsg(eFlavorEspionage != NO_FLAVOR, "Could not find espionage flavor!");

		float fRatio = iNumPlayersAheadInTech / (float)iNumOtherPlayers;
		float fCutOff = (0.05f * pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavorEspionage));

		if (fRatio < fCutOff)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}


bool EconomicAIHelpers::IsTestStrategy_EarlyExpansion(CvPlayer* pPlayer)
{
	int iDesiredCities;
	int iFlavorExpansion = 0;
	int iFlavorGrowth = 0;

#ifdef NQM_AI_GIMP_NO_BUILDING_SETTLERS
	if ((pPlayer->isHuman() && GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE)) || (!pPlayer->isHuman() && (GC.getGame().isOption("GAMEOPTION_AI_TWEAKS") || GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_BUILDING_SETTLERS"))))
#else
	if(GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && pPlayer->isHuman())
#endif
	{
		return false;
	}

	iDesiredCities = pPlayer->GetEconomicAI()->GetEarlyCityNumberTarget();
	for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes() && (iFlavorExpansion == 0 || iFlavorGrowth == 0); iFlavorLoop++)
	{
		if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_EXPANSION")
		{
			iFlavorExpansion = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)iFlavorLoop);
		}
		else if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_GROWTH")
		{
			iFlavorGrowth = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)iFlavorLoop);
		}
	}

	iDesiredCities = (iDesiredCities * iFlavorExpansion) / max(iFlavorGrowth, 1);
	int iDifficulty = max(0,GC.getGame().getHandicapInfo().GetID() - 3);
	iDesiredCities += iDifficulty;


	const int iDefaultNumTiles = 80*52;
	iDesiredCities = (iDesiredCities * GC.getMap().numPlots()) / iDefaultNumTiles;


	if(pPlayer->getCapitalCity() != NULL)
	{


		{
			CvArea* pArea = GC.getMap().getArea(pPlayer->getCapitalCity()->getArea());


			int iBestArea, iSecondBestArea;
			pPlayer->GetBestSettleAreas(pPlayer->GetEconomicAI()->GetMinimumSettleFertility(), iBestArea, iSecondBestArea);
			if(iBestArea == pArea->GetID() || iSecondBestArea == pArea->GetID())
			{
				int iNumOwnedTiles = pArea->getNumOwnedTiles();
				int iNumUnownedTiles = pArea->getNumUnownedTiles();
				int iNumTiles = max(1,pArea->getNumTiles());

				int iOwnageRatio = iNumOwnedTiles * 100 / iNumTiles;
				int iNumCities = pPlayer->getNumCities() - pPlayer->GetNumPuppetCities();
				int iSettlersOnMap = pPlayer->GetNumUnitsWithUnitAI(UNITAI_SETTLE, true);

				if(iOwnageRatio < GC.getAI_STRATEGY_AREA_IS_FULL_PERCENT()
				        && (iNumCities + iSettlersOnMap) < iDesiredCities
				        && iNumUnownedTiles >= GC.getAI_STRATEGY_EARLY_EXPANSION_NUM_UNOWNED_TILES_REQUIRED())
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool EconomicAIHelpers::IsTestStrategy_EnoughExpansion(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	int iBestArea;
	int iSecondBestArea;

#ifdef NQM_AI_GIMP_NO_BUILDING_SETTLERS
	if ((pPlayer->isHuman() && GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE)) || (!pPlayer->isHuman() && (GC.getGame().isOption("GAMEOPTION_AI_TWEAKS") || GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_BUILDING_SETTLERS"))))
#else
	if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && pPlayer->isHuman())
#endif
	{
		return true;
	}

	int iNumSettleAreas = pPlayer->GetBestSettleAreas(pPlayer->GetEconomicAI()->GetMinimumSettleFertility(), iBestArea, iSecondBestArea);
	if (iNumSettleAreas == 0)
	{
		return true;
	}

	int iNumExtraSettlers = 0;

	if (!pPlayer->IsEmpireUnhappy())
	{


		EconomicAIStrategyTypes eStrategyExpandToOtherContinents = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_REALLY_EXPAND_TO_OTHER_CONTINENTS");
		if (eStrategyExpandToOtherContinents != NO_ECONOMICAISTRATEGY)
		{
			if (pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyExpandToOtherContinents))
			{
				++iNumExtraSettlers;
				if (GC.getMap().GetAIMapHint() & 4)
				{
					++iNumExtraSettlers;
				}
			}
		}


		int iFlavorExpansion = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_EXPANSION"));
		if (iFlavorExpansion > 6)
		{
			++iNumExtraSettlers;
		}
		if (iFlavorExpansion > 8)
		{
			++iNumExtraSettlers;
		}

	}

	int iSettlersOnMap = pPlayer->GetNumUnitsWithUnitAI(UNITAI_SETTLE, true);

	CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
	int iMaxSettlers =       pStrategy->GetWeightThreshold();

	if (iSettlersOnMap >= iMaxSettlers + iNumExtraSettlers)
	{
		return true;
	}

	return false;
}


bool EconomicAIHelpers::IsTestStrategy_NeedHappiness(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	if(GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return false;
	}

	if(pPlayer->getTotalPopulation() > 0 && pPlayer->GetUnhappiness() > 0)
	{
		int iExcessHappiness = pPlayer->GetExcessHappiness();

		CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
		int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pPlayer);


		int iDivisor =       pStrategy->GetWeightThreshold();
		iWeightThresholdModifier /= iDivisor;

		if(iExcessHappiness <= iWeightThresholdModifier)
			return true;
	}

	return false;
}


bool EconomicAIHelpers::IsTestStrategy_NeedHappinessCritical(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	if(GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return false;
	}


	if(pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
		return false;

	if(pPlayer->getTotalPopulation() > 0 && pPlayer->GetUnhappiness() > 0)
	{
		int iExcessHappiness = pPlayer->GetExcessHappiness();

		CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
		int iThreshold =        pStrategy->GetWeightThreshold();

		if(iExcessHappiness <= iThreshold)
			return true;
	}

	return false;
}


bool EconomicAIHelpers::IsTestStrategy_CitiesNeedNavalGrowth(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	int iNumCitiesNeedNavalGrowth = 0;

	CvCity* pLoopCity;
	int iLoop;

	AICityStrategyTypes eStrategyNeedNavalGrowth = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_NEED_NAVAL_GROWTH");


	if(eStrategyNeedNavalGrowth == NO_AICITYSTRATEGY)
	{
		return false;
	}

	for(pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity->GetCityStrategyAI()->IsUsingCityStrategy(eStrategyNeedNavalGrowth))
		{
			iNumCitiesNeedNavalGrowth++;
		}
	}

	if(iNumCitiesNeedNavalGrowth > 0)
	{
		CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
		int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pPlayer);
		int iWeightThreshold = pStrategy->GetWeightThreshold() + iWeightThresholdModifier;

		int iCurrentWeight = (pPlayer->getNumCities() - 1) * 10;
		iCurrentWeight /= iWeightThreshold;





		if(iNumCitiesNeedNavalGrowth > iCurrentWeight)
		{
			return true;
		}
	}

	return false;
}


bool EconomicAIHelpers::IsTestStrategy_CitiesNeedNavalTileImprovement(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	int iNumCitiesNeedNavalTileImprovement = 0;

	CvCity* pLoopCity;
	int iLoop;

	AICityStrategyTypes eStrategyNeedNavalTileImprovement = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_NEED_NAVAL_TILE_IMPROVEMENT");


	if(eStrategyNeedNavalTileImprovement == NO_AICITYSTRATEGY)
	{
		return false;
	}

	for(pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity->GetCityStrategyAI()->IsUsingCityStrategy(eStrategyNeedNavalTileImprovement))
		{
			iNumCitiesNeedNavalTileImprovement++;
		}
	}

	if(iNumCitiesNeedNavalTileImprovement > 0)
	{
		CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
		int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pPlayer);
		int iWeightThreshold = pStrategy->GetWeightThreshold() + iWeightThresholdModifier;

		int iCurrentWeight = (pPlayer->getNumCities() - 1) * 10;
		iCurrentWeight /= iWeightThreshold;




		if(iNumCitiesNeedNavalTileImprovement > iCurrentWeight)
		{
			return true;
		}
	}

	return false;
}




bool EconomicAIHelpers::IsTestStrategy_FoundCity(EconomicAIStrategyTypes              , CvPlayer* pPlayer)
{
	int iUnitLoop;
	CvUnit* pLoopUnit;
	CvUnit* pFirstSettler = 0;
	int iLooseSettler = 0;

	int iFirstSettlerArea = -1;
	int iBestArea;
	int iSecondBestArea;
	int iNumAreas;
	int iArea = -1;

#ifdef NQM_AI_GIMP_NO_BUILDING_SETTLERS
	if ((pPlayer->isHuman() && GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE)) || (!pPlayer->isHuman() && (GC.getGame().isOption("GAMEOPTION_AI_TWEAKS") || GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_BUILDING_SETTLERS"))))
#else
	if(GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && pPlayer->isHuman())
#endif
	{
		return false;
	}


	if(!pPlayer->isHuman())
	{

		for(pLoopUnit = pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iUnitLoop))
		{
			if(pLoopUnit != NULL)
			{
				if(pLoopUnit->AI_getUnitAIType() == UNITAI_SETTLE)
				{
					if(pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX)
					{
						iLooseSettler++;
						iFirstSettlerArea = pLoopUnit->getArea();
						pFirstSettler = pLoopUnit;
						break;
					}
				}
			}
		}


		if (iLooseSettler && pPlayer->getNumCities() >= 1)
		{
			iNumAreas = pPlayer->GetBestSettleAreas(pPlayer->GetEconomicAI()->GetMinimumSettleFertility(), iBestArea, iSecondBestArea);
			if(iNumAreas == 0)
			{
				return false;
			}

			bool bCanEmbark = GET_TEAM(pPlayer->getTeam()).canEmbark() || pPlayer->GetPlayerTraits()->IsEmbarkedAllWater();
			bool bWantEscort = false;


			if (bCanEmbark && (pPlayer->getNumCities() > 1))
			{
				int iRandArea = GC.getGame().getJonRandNum(6, "Randomly choose an area to settle");

				if (iRandArea <= 1)
				{
					iArea = -1;
					CvPlot* pPlot = pPlayer->GetBestSettlePlot(pFirstSettler, bWantEscort, -1);
					if (!pPlot)
					{
						bWantEscort = true;
					}
				}
				else if (iRandArea == 2)
				{
					iArea = iSecondBestArea;
					bWantEscort = IsAreaSafeForQuickColony(iArea, pPlayer);
				}
				else
				{
					iArea = iBestArea;
					bWantEscort = IsAreaSafeForQuickColony(iArea, pPlayer);
				}

				if (bWantEscort)
				{
					pPlayer->addAIOperation(AI_OPERATION_FOUND_CITY, NO_PLAYER, iArea);
				}
				else
				{
					pPlayer->addAIOperation(AI_OPERATION_QUICK_COLONIZE, NO_PLAYER, iArea);
				}

				return true;
			}
			else
			{
				pPlayer->addAIOperation(AI_OPERATION_FOUND_CITY, NO_PLAYER, iBestArea);
				return true;
			}
		}
	}

	return false;
}



bool EconomicAIHelpers::IsTestStrategy_TradeWithCityState(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	int iUnitLoop;
	CvUnit* pLoopUnit;
	int iLooseMerchant = 0;
	int iStrategyWeight = 0;


	if(!pPlayer->isHuman())
	{

		for(pLoopUnit = pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iUnitLoop))
		{
			if(pLoopUnit != NULL)
			{
				if(pLoopUnit->AI_getUnitAIType() == UNITAI_MERCHANT && pLoopUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_USE_POWER)
				{
					if(pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX)
					{
						iLooseMerchant++;
					}
				}
			}
		}

		CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
		iStrategyWeight = iLooseMerchant * 10;
		int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pPlayer);
		int iWeightThreshold = pStrategy->GetWeightThreshold() + iWeightThresholdModifier;

		if(iStrategyWeight >= iWeightThreshold)
		{

			pPlayer->addAIOperation(AI_OPERATION_MERCHANT_DELEGATION);


			return true;
		}
	}

	return false;
}


bool EconomicAIHelpers::IsTestStrategy_ConcertTour(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	int iUnitLoop;
	CvUnit* pLoopUnit;
	int iLooseMusician = 0;
	int iStrategyWeight = 0;


	if(!pPlayer->isHuman())
	{

		for(pLoopUnit = pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iUnitLoop))
		{
			if(pLoopUnit != NULL)
			{
				if(pLoopUnit->AI_getUnitAIType() == UNITAI_MUSICIAN && pLoopUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_TOURISM_BLAST)
				{
					if(pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX)
					{
						iLooseMusician++;
					}
				}
			}
		}

		CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
		iStrategyWeight = iLooseMusician * 10;
		int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pPlayer);
		int iWeightThreshold = pStrategy->GetWeightThreshold() + iWeightThresholdModifier;

		if(iStrategyWeight >= iWeightThreshold)
		{

			pPlayer->addAIOperation(AI_OPERATION_CONCERT_TOUR);


			return true;
		}
	}

	return false;
}


bool EconomicAIHelpers::IsTestStrategy_NeedImprovement(CvPlayer* pPlayer, YieldTypes eYield)
{

	AICityStrategyTypes eCityStrategy = NO_AICITYSTRATEGY;
	switch(eYield)
	{
	case YIELD_FOOD:
		eCityStrategy = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_NEED_IMPROVEMENT_FOOD");
		break;
	case YIELD_PRODUCTION:
		eCityStrategy = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_NEED_IMPROVEMENT_PRODUCTION");
		break;
	}

	FAssertMsg(eCityStrategy != NO_AICITYSTRATEGY, "No strategy found. What?");
	if(eCityStrategy == NO_AICITYSTRATEGY)
	{
		return false;
	}


	int iNumCities = pPlayer->getNumCities();
	if(iNumCities == 0)
	{
		return false;
	}

	int iCityLoop = 0;
	CvCity* pLoopCity = NULL;
	int iNumCitiesConcerned = 0;
	for(pLoopCity = pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iCityLoop))
	{
		if(pLoopCity->GetCityStrategyAI()->IsUsingCityStrategy(eCityStrategy))
		{
			iNumCitiesConcerned++;
		}
	}

	double fWarningRatio = GC.getAI_STRATEGY_NEED_IMPROVEMENT_CITY_RATIO();


	if((iNumCitiesConcerned / (double)iNumCities) < fWarningRatio)
	{
		return false;
	}


	int iUnitLoop = 0;
	CvUnit* pLoopUnit = NULL;
	CvUnit* pBuilder = NULL;
	for(pLoopUnit = pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iUnitLoop))
	{
		if(pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER)
		{
			pBuilder = pLoopUnit;
			break;
		}
	}



	if(!pBuilder)
	{
		return false;
	}




	BuildTypes eBuild;
#ifdef AUI_WARNING_FIXES
	for (uint iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
#else
	int iBuildIndex;
	for(iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
#endif
	{
		eBuild = (BuildTypes)iBuildIndex;
		CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
		if(pkBuildInfo)
		{
			if(pkBuildInfo->getTechPrereq() != NO_TECH)
			{
				if(!(GET_TEAM(pPlayer->getTeam()).GetTeamTechs()->HasTech((TechTypes)GC.getBuildInfo(eBuild)->getTechPrereq())))
				{

					continue;
				}
			}

			ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(eBuild)->getImprovement();
			if(eImprovement == NO_IMPROVEMENT)
			{
				continue;
			}

			CvPlotsVector& aPlots = pPlayer->GetPlots();
			bool bCanBuild = false;
			for(uint ui = 0; ui < aPlots.size(); ui++)
			{
				if(aPlots[ui] < 0)
				{
					break;
				}

				CvPlot* pPlot = GC.getMap().plotByIndex(aPlots[ui]);
				if(pPlayer->canBuild(pPlot, eBuild, false             , false                 , false              ))
				{
					bCanBuild = true;
					break;
				}
			}

			if(!bCanBuild)
			{
				continue;
			}


			if(GC.getImprovementInfo(eImprovement)->GetYieldChange(eYield) > 0)
			{
				return false;
			}
		}
	}

	return true;
}


bool EconomicAIHelpers::IsTestStrategy_OneOrFewerCoastalCities(CvPlayer* pPlayer)
{
	int iCityLoop = 0;
	CvCity* pLoopCity = NULL;
	int iNumCoastalCities = 0;
	for(pLoopCity = pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iCityLoop))
	{
		if(pLoopCity->isCoastal())
		{
			iNumCoastalCities++;
		}
	}
	return iNumCoastalCities <= 1;
}


bool EconomicAIHelpers::IsTestStrategy_LosingMoney(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
	int iInterval = pStrategy->GetMinimumNumTurnsExecuted();


	if(GC.getGame().getGameTurn() <= iInterval)
	{
		return false;
	}


	return (pPlayer->GetTreasury()->AverageIncome(iInterval) < (double)pStrategy->GetWeightThreshold()        );
}


bool EconomicAIHelpers::IsTestStrategy_HaltGrowthBuildings(CvPlayer* pPlayer)
{
	BuildingTypes eNextDesiredWonder = pPlayer->GetCitySpecializationAI()->GetNextWonderDesired();


	if(eNextDesiredWonder != NO_BUILDING)
	{
		CvBuildingXMLEntries* pkGameBuildings = GC.GetGameBuildings();
		CvBuildingEntry* pkBuildingInfo = pkGameBuildings->GetEntry(eNextDesiredWonder);
		if(pkBuildingInfo)
		{
			BuildingClassTypes eBuildingClass = (BuildingClassTypes)pkBuildingInfo->GetFreeBuildingClass();
			if(eBuildingClass != NO_BUILDINGCLASS)
			{
				CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
				{
					if(pkBuildingClassInfo)
					{
						BuildingTypes eBuilding = (BuildingTypes)pkBuildingClassInfo->getDefaultBuildingIndex();
						if(eBuilding != NO_BUILDING)
						{
							CvBuildingEntry* pkFreeBuildingInfo = pkGameBuildings->GetEntry(eBuilding);
							if(pkFreeBuildingInfo)
							{
								if(pkFreeBuildingInfo->GetYieldChange(YIELD_FOOD) > 0)
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


bool EconomicAIHelpers::IsTestStrategy_TooManyUnits(CvPlayer* pPlayer)
{
	return (pPlayer->GetUnitProductionMaintenanceMod()) != 0;
}


bool EconomicAIHelpers::IsTestStrategy_IslandStart(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	int iStartArea;
	CvPlot* pLoopPlot;
	int iCoastalTiles = 0;
	int iRevealedCoastalTiles = 0;

	CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);


	if(GC.getGame().getGameTurn() < 25 && pPlayer->getStartingPlot())
	{
		if(GET_TEAM(pPlayer->getTeam()).getCanEmbarkCount() < 1)
		{
			iStartArea = pPlayer->getStartingPlot()->getArea();


#ifdef AUI_WARNING_FIXES
			for (uint iI = 0; iI < GC.getMap().numPlots(); iI++)
#else
			for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
#endif
			{
				pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
				if(pLoopPlot->getArea() == iStartArea)
				{
					if(pLoopPlot->isCoastalLand())
					{
						iCoastalTiles++;
					}
					if(pLoopPlot->isRevealed(pPlayer->getTeam()))
					{
						iRevealedCoastalTiles++;
					}
				}
			}

			if((iRevealedCoastalTiles * 100 / (iCoastalTiles + 1)) > GC.getAI_STRATEGY_ISLAND_START_COAST_REVEAL_PERCENT() &&
			        GC.getMap().getArea(iStartArea)->getNumTiles() < pStrategy->GetWeightThreshold())
			{
				return true;
			}
		}
	}

	return false;
}


bool EconomicAIHelpers::IsTestStrategy_ExpandToOtherContinents(CvPlayer* pPlayer)
{
#ifdef NQM_AI_GIMP_NO_BUILDING_SETTLERS
	if ((pPlayer->isHuman() && GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE)) || (!pPlayer->isHuman() && (GC.getGame().isOption("GAMEOPTION_AI_TWEAKS") || GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_BUILDING_SETTLERS"))))
#else
	if(GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && pPlayer->isHuman())
#endif
	{
		return false;
	}


	EconomicAIStrategyTypes eStrategyIslandStart = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_ISLAND_START");
	if(eStrategyIslandStart != NO_ECONOMICAISTRATEGY)
	{
		if(pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyIslandStart))
		{
			return false;
		}
	}


	EconomicAIStrategyTypes eEarlyExpansion = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EARLY_EXPANSION");
	if(eEarlyExpansion != NO_ECONOMICAISTRATEGY)
	{
		if(pPlayer->GetEconomicAI()->IsUsingStrategy(eEarlyExpansion))
		{
			return false;
		}
	}


	MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_LOSING_WARS");
	if(eStrategyAtWar != NO_MILITARYAISTRATEGY)
	{
		if(pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyAtWar))
		{
			return false;
		}
	}

	if(pPlayer->getCapitalCity() != NULL)
	{
		CvArea* pArea = GC.getMap().getArea(pPlayer->getCapitalCity()->getArea());


		int iBestArea, iSecondBestArea;
		pPlayer->GetBestSettleAreas(pPlayer->GetEconomicAI()->GetMinimumSettleFertility(), iBestArea, iSecondBestArea);

		if((iBestArea != pArea->GetID() && iBestArea != -1) || (iSecondBestArea != pArea->GetID() && iSecondBestArea != -1))
		{
			return true;
		}
	}

	return false;
}



bool EconomicAIHelpers::IsTestStrategy_ReallyExpandToOtherContinents(CvPlayer* pPlayer)
{
#ifdef NQM_AI_GIMP_NO_BUILDING_SETTLERS
	if ((pPlayer->isHuman() && GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE)) || (!pPlayer->isHuman() && (GC.getGame().isOption("GAMEOPTION_AI_TWEAKS") || GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_BUILDING_SETTLERS"))))
#else
	if(GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && pPlayer->isHuman())
#endif
	{
		return false;
	}


	EconomicAIStrategyTypes eStrategyExpandToOtherContinents = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS");
	if(eStrategyExpandToOtherContinents != NO_ECONOMICAISTRATEGY)
	{
		if(!pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyExpandToOtherContinents))
		{
			return false;
		}
	}

	MilitaryAIStrategyTypes eStrategyLosingAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_LOSING_WARS");
	if(eStrategyLosingAtWar != NO_MILITARYAISTRATEGY)
	{
		if(pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyLosingAtWar))
		{
			return false;
		}
	}


	if ((GC.getMap().GetAIMapHint() & 4) == 0)
	{
		MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
		if(eStrategyAtWar != NO_MILITARYAISTRATEGY)
		{
			if(pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyAtWar))
			{
				return false;
			}
		}
	}

	int iFlavorGrowth = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GROWTH"));
	int iFlavorExpansion = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_EXPANSION"));

	if (iFlavorGrowth < iFlavorExpansion && pPlayer->getCapitalCity() != NULL && !pPlayer->IsEmpireUnhappy())
	{

		int iBestArea, iSecondBestArea;
		pPlayer->GetBestSettleAreas(pPlayer->GetEconomicAI()->GetMinimumSettleFertility(), iBestArea, iSecondBestArea);
		if ((iBestArea != pPlayer->getCapitalCity()->getArea() && iBestArea != -1))
		{
			return true;
		}
	}

	return false;
}



bool EconomicAIHelpers::IsTestStrategy_MostlyOnTheCoast(CvPlayer* pPlayer)
{

	int iInlandPop = 0;
	int iCoastalPop = 0;
	int iLoopCity = 0;
	CvCity* pLoopCity = NULL;
	for(pLoopCity = pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoopCity))
	{
		if(pLoopCity->isCoastal(100))
		{
			iCoastalPop += pLoopCity->getPopulation();
		}
		else
		{
			iInlandPop += pLoopCity->getPopulation();
		}
	}
	return (iCoastalPop > 0 && iCoastalPop >= iInlandPop);
}

bool EconomicAIHelpers::IsTestStrategy_ExpandLikeCrazy(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
#ifdef NQM_AI_GIMP_NO_BUILDING_SETTLERS
	if ((pPlayer->isHuman() && GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE)) || (!pPlayer->isHuman() && (GC.getGame().isOption("GAMEOPTION_AI_TWEAKS") || GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_BUILDING_SETTLERS"))))
#else
	if(GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && pPlayer->isHuman())
#endif
	{
		return false;
	}


	AIGrandStrategyTypes eGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");
	if(eGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if(pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == eGrandStrategy)
		{

			if(GC.getMap().getWorldInfo().GetNumCitiesPolicyCostMod() > 0)
			{
				return false;
			}
		}
	}

	int iFlavorExpansion = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_EXPANSION"));
	CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
	if(iFlavorExpansion >= pStrategy->GetWeightThreshold())
	{
		return true;
	}

	return false;
}

bool EconomicAIHelpers::IsTestStrategy_GrowLikeCrazy(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	int iFlavorGrowth = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GROWTH"));
	CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
	if(iFlavorGrowth >= pStrategy->GetWeightThreshold())
	{
		return true;
	}

	return false;
}





bool EconomicAIHelpers::IsTestStrategy_GS_Culture(CvPlayer* pPlayer)
{
	AIGrandStrategyTypes eGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");

	if(eGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if(pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == eGrandStrategy)
		{
			return true;
		}
	}

	return false;
}


bool EconomicAIHelpers::IsTestStrategy_GS_Conquest(CvPlayer* pPlayer)
{
	AIGrandStrategyTypes eGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST");

	if(eGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if(pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == eGrandStrategy)
		{
			return true;
		}
	}

	return false;
}


bool EconomicAIHelpers::IsTestStrategy_GS_Diplomacy(CvPlayer* pPlayer)
{
	AIGrandStrategyTypes eGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_UNITED_NATIONS");

	if(eGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if(pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == eGrandStrategy)
		{
			return true;
		}
	}

	return false;
}


bool EconomicAIHelpers::IsTestStrategy_GS_Spaceship(CvPlayer* pPlayer)
{
	AIGrandStrategyTypes eGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_SPACESHIP");

	if(eGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if(pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == eGrandStrategy)
		{
			return true;
		}
	}

	return false;
}


bool EconomicAIHelpers::IsTestStrategy_GS_SpaceshipHomestretch(CvPlayer* pPlayer)
{

	ProjectTypes eApolloProgram = (ProjectTypes) GC.getInfoTypeForString("PROJECT_APOLLO_PROGRAM", true);
	if(eApolloProgram != NO_PROJECT)
	{
		if(GET_TEAM(pPlayer->getTeam()).getProjectCount(eApolloProgram) > 0)
		{
			return true;
		}
	}

	return false;
}


bool EconomicAIHelpers::IsTestStrategy_NeedArchaeologists(CvPlayer* pPlayer)
{
	int iNumSites = pPlayer->GetEconomicAI()->GetVisibleAntiquitySites();
	int iNumArchaeologists = pPlayer->GetNumUnitsWithUnitAI(UNITAI_ARCHAEOLOGIST, true);

	if (iNumSites > iNumArchaeologists)
	{
		return true;
	}

	return false;
}


bool EconomicAIHelpers::IsTestStrategy_EnoughArchaeologists(CvPlayer* pPlayer)
{
	int iNumSites = GC.getGame().GetNumArchaeologySites();
	double iMaxRatio = .5;
	int iNumArchaeologists = pPlayer->GetNumUnitsWithUnitAI(UNITAI_ARCHAEOLOGIST, true);
	PolicyTypes eExpFinisher = (PolicyTypes) GC.getInfoTypeForString("POLICY_EXPLORATION_FINISHER", true                );
	
	if (eExpFinisher != NO_POLICY)
	{
		if (pPlayer->GetPlayerPolicies()->HasPolicy(eExpFinisher))
		{
			iNumSites += GC.getGame().GetNumHiddenArchaeologySites();
		}
	}
		
	if ((double)iNumSites * iMaxRatio + 1 < iNumArchaeologists)
	{
		return true;
	}

	return false;
}


bool EconomicAIHelpers::IsTestStrategy_NeedMuseums(CvPlayer* pPlayer)
{
	int iNumSites = pPlayer->GetEconomicAI()->GetVisibleAntiquitySites();
	GreatWorkSlotType eArtArtifactSlot = CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT();
	int iNumGreatWorkSlots = pPlayer->GetCulture()->GetNumAvailableGreatWorkSlots(eArtArtifactSlot);

	if (iNumSites > iNumGreatWorkSlots)
	{
		return true;
	}

	return false;
}


bool EconomicAIHelpers::IsTestStrategy_NeedGuilds(CvPlayer* pPlayer)
{
	CvTeam &kTeam = GET_TEAM(pPlayer->getTeam());

	BuildingTypes eWritersGuild = (BuildingTypes)GC.getInfoTypeForString("BUILDING_WRITERS_GUILD", true);
	BuildingTypes eArtistsGuild = (BuildingTypes)GC.getInfoTypeForString("BUILDING_ARTISTS_GUILD", true);
	BuildingTypes eMusiciansGuild = (BuildingTypes)GC.getInfoTypeForString("BUILDING_MUSICIANS_GUILD", true);

	CvBuildingEntry *pkBuilding;
	pkBuilding = GC.getBuildingInfo(eWritersGuild);
	if (pkBuilding)
	{
		if (kTeam.GetTeamTechs()->HasTech((TechTypes)pkBuilding->GetPrereqAndTech()))
		{
			if (pPlayer->GetFirstCityWithBuildingClass((BuildingClassTypes)pkBuilding->GetBuildingClassType()) == NULL)
			{
				return true;
			}
		}
	}
	pkBuilding = GC.getBuildingInfo(eArtistsGuild);
	if (pkBuilding)
	{
		if (kTeam.GetTeamTechs()->HasTech((TechTypes)pkBuilding->GetPrereqAndTech()))
		{
			if (pPlayer->GetFirstCityWithBuildingClass((BuildingClassTypes)pkBuilding->GetBuildingClassType()) == NULL)
			{
				return true;
			}
		}
	}
	pkBuilding = GC.getBuildingInfo(eMusiciansGuild);
	if (pkBuilding)
	{
		if (kTeam.GetTeamTechs()->HasTech((TechTypes)pkBuilding->GetPrereqAndTech()))
		{
			if (pPlayer->GetFirstCityWithBuildingClass((BuildingClassTypes)pkBuilding->GetBuildingClassType()) == NULL)
			{
				return true;
			}
		}
	}

	return false;
}


bool EconomicAIHelpers::IsTestStrategy_StartedPiety(CvPlayer* pPlayer)
{
	bool bRtnValue = false;

	PolicyBranchTypes eBranch = (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_PIETY", true);
	if (eBranch != NO_POLICY_BRANCH_TYPE)
	{
		if (pPlayer->GetPlayerPolicies()->IsPolicyBranchUnlocked(eBranch))
		{
			return true;
		}
	}

	return bRtnValue;
}
