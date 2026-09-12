






#include "CvGameCoreDLLPCH.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvInternalGameCoreUtils.h"
#include "CvDiplomacyAIEnums.h"
#include "CvEconomicAI.h"
#include "CvMilitaryAI.h"
#include "CvGrandStrategyAI.h"
#include "CvCitySpecializationAI.h"
#include "CvEspionageClasses.h"
#include "Fireworks/FVariableSystem.h"
#include "CvEnumSerialization.h"
#include "CvInfosSerializationHelper.h"
#include "cvStopWatch.h"

#include "LintFree.h"




CvAICityStrategyEntry::CvAICityStrategyEntry(void):
	m_piFlavorValue(NULL),
	m_iWeightThreshold(0),
	m_piPersonalityFlavorThresholdMod(NULL),
	m_iTechPrereq(NO_TECH),
	m_iTechObsolete(NO_TECH),
	m_iMinimumNumTurnsExecuted(0),
	m_iCheckTriggerTurnCount(0),
	m_bNoMinorCivs(false),
	m_bPermanent(false),
	m_eAdvisor(NO_ADVISOR_TYPE),
	m_iAdvisorCounselImportance(1)
{
}

CvAICityStrategyEntry::~CvAICityStrategyEntry(void)
{
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piPersonalityFlavorThresholdMod);
}

bool CvAICityStrategyEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;


	m_bNoMinorCivs				= kResults.GetBool("NoMinorCivs");
	m_bPermanent				= kResults.GetBool("Permanent");
	m_iCheckTriggerTurnCount	= kResults.GetInt("CheckTriggerTurnCount");
	m_iMinimumNumTurnsExecuted	= kResults.GetInt("MinimumNumTurnsExecuted");
	m_iWeightThreshold			= kResults.GetInt("WeightThreshold");

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


	const char* szTextVal;
	szTextVal = kResults.GetText("TechObsolete");
	m_iTechObsolete = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("TechPrereq");
	m_iTechPrereq = GC.getInfoTypeForString(szTextVal, true);


	const char* szType = GetType();
	kUtility.SetFlavors(m_piFlavorValue, "AICityStrategy_Flavors",
	                    "AICityStrategyType", szType);

	kUtility.SetFlavors(m_piPersonalityFlavorThresholdMod,
	                    "AICityStrategy_PersonalityFlavorThresholdMods",
	                    "AICityStrategyType", szType);

	return true;
}


int CvAICityStrategyEntry::GetFlavorValue(int i) const
{
	CvAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : -1;
}


int CvAICityStrategyEntry::GetWeightThreshold() const
{
	return m_iWeightThreshold;
}


int CvAICityStrategyEntry::GetPersonalityFlavorThresholdMod(int i) const
{
	CvAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piPersonalityFlavorThresholdMod ? m_piPersonalityFlavorThresholdMod[i] : -1;
}


int CvAICityStrategyEntry::GetTechPrereq() const
{
	return m_iTechPrereq;
}


int CvAICityStrategyEntry::GetTechObsolete() const
{
	return m_iTechObsolete;
}


int CvAICityStrategyEntry::GetMinimumNumTurnsExecuted() const
{
	return m_iMinimumNumTurnsExecuted;
}


int CvAICityStrategyEntry::GetCheckTriggerTurnCount() const
{
	return m_iCheckTriggerTurnCount;
}


bool CvAICityStrategyEntry::IsNoMinorCivs() const
{
	return m_bNoMinorCivs;
}


bool CvAICityStrategyEntry::IsPermanent() const
{
	return m_bPermanent;
}

AdvisorTypes CvAICityStrategyEntry::GetAdvisor() const
{
	return m_eAdvisor;
}

const char* CvAICityStrategyEntry::GetAdvisorCounselText() const
{
	return m_strAdvisorCounselText.c_str();
}

int CvAICityStrategyEntry::GetAdvisorCounselImportance() const
{
	return m_iAdvisorCounselImportance;
}







CvAICityStrategies::CvAICityStrategies(void)
{

}


CvAICityStrategies::~CvAICityStrategies(void)
{
	DeleteArray();
}


std::vector<CvAICityStrategyEntry*>& CvAICityStrategies::GetAICityStrategyEntries()
{
	return m_paAICityStrategyEntries;
}


int CvAICityStrategies::GetNumAICityStrategies()
{
	return m_paAICityStrategyEntries.size();
}


void CvAICityStrategies::DeleteArray()
{
	for(std::vector<CvAICityStrategyEntry*>::iterator it = m_paAICityStrategyEntries.begin(); it != m_paAICityStrategyEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paAICityStrategyEntries.clear();
}


CvAICityStrategyEntry* CvAICityStrategies::GetEntry(int index)
{
	return m_paAICityStrategyEntries[index];
}






#ifdef AUI_WARNING_FIXES
unsigned int CvCityStrategyAI::m_acBestYields[NUM_YIELD_TYPES][NUM_CITY_PLOTS] = {};
#else
unsigned char  CvCityStrategyAI::m_acBestYields[NUM_YIELD_TYPES][NUM_CITY_PLOTS];
#endif


#ifdef AUI_CITY_FIX_COMPONENT_CONSTRUCTORS_CONTAIN_POINTERS
CvCityStrategyAI::CvCityStrategyAI(CvCity* pCity):
	m_pCity(pCity),
#else
CvCityStrategyAI::CvCityStrategyAI():
#endif
	m_pabUsingCityStrategy(NULL),
	m_paiTurnCityStrategyAdopted(NULL),
	m_aiTempFlavors(NULL),
	m_pBuildingProductionAI(NULL),
	m_pUnitProductionAI(NULL),
	m_pProjectProductionAI(NULL),
	m_pProcessProductionAI(NULL),
#ifdef AUI_WARNING_FIXES
	m_pAICityStrategies(NULL),
	m_eSpecialization(NO_CITY_SPECIALIZATION),
	m_eDefaultSpecialization(NO_CITY_SPECIALIZATION),
	m_asBestYieldAverageTimes100(),
	m_asYieldDeltaTimes100(),
#endif
	m_eFocusYield((YieldTypes)NO_YIELD)
{
}


CvCityStrategyAI::~CvCityStrategyAI()
{
	Uninit();
}


void CvCityStrategyAI::Init(CvAICityStrategies* pAICityStrategies, CvCity* pCity, bool bIsCity)
{

	CvFlavorRecipient::Init();


	m_bIsCity = bIsCity;
	m_pAICityStrategies = pAICityStrategies;
	m_pCity = pCity;


	CvAssertMsg(m_pabUsingCityStrategy==NULL, "about to leak memory, CvCityAIStrategies::m_pabUsingCityStrategy");
	m_pabUsingCityStrategy = FNEW(bool[m_pAICityStrategies->GetNumAICityStrategies()], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paiTurnCityStrategyAdopted==NULL, "about to leak memory, CvCityAIStrategies::m_paiTurnCityStrategyAdopted");
	m_paiTurnCityStrategyAdopted = FNEW(int[m_pAICityStrategies->GetNumAICityStrategies()], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_aiTempFlavors==NULL, "about to leak memory, CvCityAIStrategies::m_aiTempFlavors");
	m_aiTempFlavors = FNEW(int[GC.getNumFlavorTypes()], c_eCiv5GameplayDLL, 0);


	m_pBuildingProductionAI = FNEW(CvBuildingProductionAI(pCity, pCity->GetCityBuildings()), c_eCiv5GameplayDLL, 0);
	m_pUnitProductionAI = FNEW(CvUnitProductionAI(pCity, GC.GetGameUnits()), c_eCiv5GameplayDLL, 0);
	m_pProjectProductionAI = FNEW(CvProjectProductionAI(pCity), c_eCiv5GameplayDLL, 0);
	m_pProcessProductionAI = FNEW(CvProcessProductionAI(pCity), c_eCiv5GameplayDLL, 0);


	Reset();
}


void CvCityStrategyAI::Uninit()
{

	CvFlavorRecipient::Uninit();


	SAFE_DELETE_ARRAY(m_pabUsingCityStrategy);
	SAFE_DELETE_ARRAY(m_paiTurnCityStrategyAdopted);
	SAFE_DELETE_ARRAY(m_aiTempFlavors);
	SAFE_DELETE(m_pBuildingProductionAI);
	SAFE_DELETE(m_pUnitProductionAI);
	SAFE_DELETE(m_pProjectProductionAI);
	SAFE_DELETE(m_pProcessProductionAI);
}


void CvCityStrategyAI::Reset()
{
	int iI;

	for(iI = 0; iI < m_pAICityStrategies->GetNumAICityStrategies(); iI++)
	{
		m_pabUsingCityStrategy[iI] = false;
		m_paiTurnCityStrategyAdopted[iI] = -1;
	}

	m_eSpecialization = NO_CITY_SPECIALIZATION;
	m_eDefaultSpecialization = NO_CITY_SPECIALIZATION;
	m_eFocusYield = (YieldTypes)NO_YIELD;


	m_pBuildingProductionAI->Reset();
	m_pUnitProductionAI->Reset();
	m_pProjectProductionAI->Reset();
	m_pProcessProductionAI->Reset();
}


void CvCityStrategyAI::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	CvAssertMsg(m_piLatestFlavorValues != NULL && GC.getNumFlavorTypes() > 0, "Number of flavor values to serialize is expected to greater than 0");

	int iNumFlavors;
	kStream >> iNumFlavors;
	ArrayWrapper<int> wrapLatestFlavor(iNumFlavors, m_piLatestFlavorValues);
	kStream >> wrapLatestFlavor;

	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabUsingCityStrategy, GC.getNumAICityStrategyInfos());
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiTurnCityStrategyAdopted, GC.getNumAICityStrategyInfos());

	kStream >> m_eSpecialization;
	kStream >> m_eDefaultSpecialization;
	m_eFocusYield = (YieldTypes)NO_YIELD;

	m_pBuildingProductionAI->Read(kStream);
	m_pUnitProductionAI->Read(kStream);
	m_pProjectProductionAI->Read(kStream);
	m_pProcessProductionAI->Read(kStream);
}


void CvCityStrategyAI::Write(FDataStream& kStream)
{

	uint uiVersion = 1;
	kStream << uiVersion;

	CvAssertMsg(GC.getNumFlavorTypes() > 0, "Number of flavor values to serialize is expected to greater than 0");
	int iNumFlavors = GC.getNumFlavorTypes();
	kStream << iNumFlavors;
	kStream << ArrayWrapper<int>(iNumFlavors, m_piLatestFlavorValues);

	int iNumStrategies = GC.getNumAICityStrategyInfos();

	CvInfosSerializationHelper::WriteHashedDataArray<AICityStrategyTypes, bool>(kStream, m_pabUsingCityStrategy, iNumStrategies);
	CvInfosSerializationHelper::WriteHashedDataArray<AICityStrategyTypes, int>(kStream, m_paiTurnCityStrategyAdopted, iNumStrategies);

	kStream << m_eSpecialization;
	kStream << m_eDefaultSpecialization;

	m_pBuildingProductionAI->Write(kStream);
	m_pUnitProductionAI->Write(kStream);
	m_pProjectProductionAI->Write(kStream);
	m_pProcessProductionAI->Write(kStream);
}


void CvCityStrategyAI::FlavorUpdate()
{

	m_pBuildingProductionAI->Reset();
	m_pUnitProductionAI->Reset();
	m_pProjectProductionAI->Reset();
	m_pProcessProductionAI->Reset();


	for(int iFlavor = 0; iFlavor < GC.getNumFlavorTypes(); iFlavor++)
	{
#ifdef AUI_CITYSTRATEGY_FIX_CHOOSE_PRODUCTION_PUPPETS_NULLIFY_BARRACKS
		if (GetCity()->IsPuppet() && (FlavorTypes)iFlavor == (FlavorTypes)GC.getInfoTypeForString("FLAVOR_MILITARY_TRAINING"))
			continue;
#endif
		int iFlavorValue = GetLatestFlavorValue((FlavorTypes)iFlavor);

#ifdef AUI_BUILDING_PRODUCTION_AI_CONSIDER_FREE_STUFF
		m_pUnitProductionAI->AddFlavorWeights((FlavorTypes)iFlavor, iFlavorValue);
#endif
		m_pBuildingProductionAI->AddFlavorWeights((FlavorTypes)iFlavor, iFlavorValue);
#ifndef AUI_BUILDING_PRODUCTION_AI_CONSIDER_FREE_STUFF
		m_pUnitProductionAI->AddFlavorWeights((FlavorTypes)iFlavor, iFlavorValue);
#endif
		m_pProjectProductionAI->AddFlavorWeights((FlavorTypes)iFlavor, iFlavorValue);
		m_pProcessProductionAI->AddFlavorWeights((FlavorTypes)iFlavor, iFlavorValue);
	}
}


void CvCityStrategyAI::UpdateFlavorsForNewCity()
{
	int iFlavorLoop;


	for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		m_aiTempFlavors[iFlavorLoop] = 0;
	}


#ifdef AUI_WARNING_FIXES
	for (uint iStrategyLoop = 0; iStrategyLoop < GC.getNumEconomicAIStrategyInfos(); iStrategyLoop++)
#else
	for(int iStrategyLoop = 0; iStrategyLoop < GC.getNumEconomicAIStrategyInfos(); iStrategyLoop++)
#endif
	{
		EconomicAIStrategyTypes eStrategy = (EconomicAIStrategyTypes) iStrategyLoop;
		CvEconomicAIStrategyXMLEntry* pStrategy = GC.getEconomicAIStrategyInfo(eStrategy);

		if(pStrategy)
		{

			if(GET_PLAYER(m_pCity->getOwner()).GetEconomicAI()->IsUsingStrategy(eStrategy))
			{
				for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
				{
					m_aiTempFlavors[iFlavorLoop] += pStrategy->GetCityFlavorValue(iFlavorLoop);
				}
			}
		}
	}
#ifdef AUI_WARNING_FIXES
	for (uint iStrategyLoop = 0; iStrategyLoop < GC.getNumMilitaryAIStrategyInfos(); iStrategyLoop++)
#else
	for(int iStrategyLoop = 0; iStrategyLoop < GC.getNumMilitaryAIStrategyInfos(); iStrategyLoop++)
#endif
	{
		MilitaryAIStrategyTypes eStrategy = (MilitaryAIStrategyTypes) iStrategyLoop;
		CvMilitaryAIStrategyXMLEntry* pStrategy = GC.getMilitaryAIStrategyInfo(eStrategy);

		if(pStrategy)
		{

			if(GET_PLAYER(m_pCity->getOwner()).GetMilitaryAI()->IsUsingStrategy(eStrategy))
			{
				for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
				{
					m_aiTempFlavors[iFlavorLoop] += pStrategy->GetCityFlavorValue(iFlavorLoop);
				}
			}
		}
	}

	ChangeFlavors(m_aiTempFlavors, true);

	LogFlavors();
}


bool CvCityStrategyAI::SetSpecialization(CitySpecializationTypes eSpecialization)
{
	if(eSpecialization != m_eSpecialization)
	{
		LogSpecializationChange(eSpecialization);


		SpecializationFlavorChange(false                   , m_eSpecialization);


		SpecializationFlavorChange(true                 , eSpecialization);

		m_eSpecialization = eSpecialization;

		return true;
	}

	return false;
}


bool CvCityStrategyAI::SetDefaultSpecialization(CitySpecializationTypes eDefaultSpecialization)
{
	if(eDefaultSpecialization != m_eDefaultSpecialization)
	{
		m_eDefaultSpecialization = eDefaultSpecialization;
		return true;
	}

	return false;
}


void CvCityStrategyAI::SpecializationFlavorChange(bool bTurnOn, CitySpecializationTypes eSpecialization)
{
	if(eSpecialization != NO_CITY_SPECIALIZATION)
	{
		CvCitySpecializationXMLEntry* pSpecialization = GC.getCitySpecializationInfo(eSpecialization);
		if(pSpecialization)
		{
			int iFlavorLoop;


			for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
			{
				m_aiTempFlavors[iFlavorLoop] = 0;
			}

			for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
			{
				if(bTurnOn)
				{
					m_aiTempFlavors[iFlavorLoop] += pSpecialization->GetFlavorValue(iFlavorLoop);
				}
				else
				{
					m_aiTempFlavors[iFlavorLoop] -= pSpecialization->GetFlavorValue(iFlavorLoop);
				}
			}

			ChangeFlavors(m_aiTempFlavors, true);

			LogFlavors();
		}
	}
}




CvCity* CvCityStrategyAI::GetCity()
{
	return m_pCity;
}


CvAICityStrategies* CvCityStrategyAI::GetAICityStrategies()
{
	return m_pAICityStrategies;
}


bool CvCityStrategyAI::IsUsingCityStrategy(AICityStrategyTypes eStrategy)
{
	return m_pabUsingCityStrategy[(int) eStrategy];
}


void CvCityStrategyAI::SetUsingCityStrategy(AICityStrategyTypes eStrategy, bool bValue)
{
	if(m_pabUsingCityStrategy[eStrategy] != bValue)
	{
		m_pabUsingCityStrategy[eStrategy] = bValue;

		if(bValue)
		{
			SetTurnCityStrategyAdopted(eStrategy, GC.getGame().getGameTurn());
		}
		else
		{
			SetTurnCityStrategyAdopted(eStrategy, -1);
		}

		LogStrategy(eStrategy, bValue);
	}
}


int CvCityStrategyAI::GetTurnCityStrategyAdopted(AICityStrategyTypes eStrategy)
{
	return m_paiTurnCityStrategyAdopted[(int) eStrategy];
}


void CvCityStrategyAI::SetTurnCityStrategyAdopted(AICityStrategyTypes eStrategy, int iValue)
{
	if(m_paiTurnCityStrategyAdopted[(int) eStrategy] != iValue)
	{
		m_paiTurnCityStrategyAdopted[(int) eStrategy] = iValue;
	}
}


CvBuildingProductionAI* CvCityStrategyAI::GetBuildingProductionAI()
{
	return m_pBuildingProductionAI;
}


CvUnitProductionAI* CvCityStrategyAI::GetUnitProductionAI()
{
	return m_pUnitProductionAI;
}


CvProjectProductionAI* CvCityStrategyAI::GetProjectProductionAI()
{
	return m_pProjectProductionAI;
}

CvProcessProductionAI* CvCityStrategyAI::GetProcessProductionAI()
{
	return m_pProcessProductionAI;
}


CvString CvCityStrategyAI::GetLogFileName(CvString& playerName, CvString& cityName) const
{
	CvString strLogName;


	if(GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "CityStrategyAILog_" + playerName + "_" + cityName + ".csv";
	}
	else
	{
		strLogName = "CityStrategyAILog.csv";
	}

	return strLogName;
}


static double citystrategyround(double x)
{
	return (x >= 0) ? floor(x + .5) : ceil(x - .5);
};


bool CvCityStrategyAI::IsYieldDeficient(YieldTypes eYieldType)
{
	double fDesiredYield = GetDeficientYieldValue(eYieldType);
	double fYieldAverage = GetYieldAverage(eYieldType);

	int iDesiredYield = (int)citystrategyround(fDesiredYield * 100);
	int iYieldAverage = (int)citystrategyround(fYieldAverage * 100);

	if(iYieldAverage < iDesiredYield)
	{
		return true;
	}
	else
	{
		return false;
	}
}


YieldTypes CvCityStrategyAI::GetDeficientYield(void)
{
	if(IsYieldDeficient(YIELD_FOOD))
	{
		return YIELD_FOOD;
	}
	else if(IsYieldDeficient(YIELD_PRODUCTION))
	{
		return YIELD_PRODUCTION;
	}
	else
	{
		for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
			YieldTypes yield = (YieldTypes)ui;
			if(IsYieldDeficient(yield))
			{
				return yield;
			}
		}
	}

	return NO_YIELD;
}


double CvCityStrategyAI::GetYieldAverage(YieldTypes eYieldType)
{
	CvPlayer* pPlayer = &GET_PLAYER(m_pCity->getOwner());
	CvPlotsVector& aiPlots = pPlayer->GetPlots();

	int iTilesWorked = 0;
	int iYieldAmount = 0;
	for(uint ui = 0; ui < aiPlots.size(); ui++)
	{

		if(aiPlots[ui] == -1)
		{
			break;
		}

		CvPlot* pPlot = GC.getMap().plotByIndex(aiPlots[ui]);
		if(!m_pCity->GetCityCitizens()->IsWorkingPlot(pPlot))
		{
			continue;
		}

		iTilesWorked++;
		iYieldAmount += pPlot->calculateYield(eYieldType);
	}

	double fRatio = 0.0;
	if(iTilesWorked > 0)
	{
		fRatio = iYieldAmount / (double)iTilesWorked;
	}

	return fRatio;
}


double CvCityStrategyAI::GetDeficientYieldValue(YieldTypes eYieldType)
{
	double fDesiredYield = -999.0;

	switch(eYieldType)
	{
	case YIELD_FOOD:
		fDesiredYield = GC.getAI_CITYSTRATEGY_YIELD_DEFICIENT_FOOD();
		break;
	case YIELD_PRODUCTION:
		fDesiredYield = GC.getAI_CITYSTRATEGY_YIELD_DEFICIENT_PRODUCTION();
		break;
	case YIELD_SCIENCE:
		fDesiredYield = GC.getAI_CITYSTRATEGY_YIELD_DEFICIENT_SCIENCE();
		break;
	case YIELD_GOLD:
		fDesiredYield = GC.getAI_CITYSTRATEGY_YIELD_DEFICIENT_GOLD();
		break;

	case YIELD_CULTURE:
		break;
	case YIELD_FAITH:
		break;
	default:
		FAssertMsg(false, "Yield type is not handled. What?");
		return false;
	}

	return fDesiredYield;
}


void CvCityStrategyAI::ChooseProduction(bool bUseAsyncRandom, BuildingTypes eIgnoreBldg                    , UnitTypes eIgnoreUnit                )
{
	RandomNumberDelegate fcn;
#ifdef AUI_WARNING_FIXES
	uint iBldgLoop, iUnitLoop, iProjectLoop, iProcessLoop;
	int iTempWeight;
#else
	int iBldgLoop, iUnitLoop, iProjectLoop, iProcessLoop, iTempWeight;
#endif
	CvCityBuildable buildable;
	CvCityBuildable selection;
	UnitTypes eUnitForOperation;
	UnitTypes eUnitForArmy;

	CvPlayerAI& kPlayer = GET_PLAYER(m_pCity->getOwner());
	CvDiplomacyAI* pDiploAI = kPlayer.GetDiplomacyAI();




	if(bUseAsyncRandom)
	{
		fcn = MakeDelegate(&GC.getGame(), &CvGame::getAsyncRandNum);
	}
	else
	{
		fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
	}


	m_Buildables.clear();

	EconomicAIStrategyTypes eStrategyEnoughSettlers = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_ENOUGH_EXPANSION");
	bool bEnoughSettlers = kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyEnoughSettlers);


	eUnitForOperation = m_pCity->GetUnitForOperation();
	if(eUnitForOperation != NO_UNIT)
	{
		buildable.m_eBuildableType = CITY_BUILDABLE_UNIT_FOR_OPERATION;
		buildable.m_iIndex = (int)eUnitForOperation;
		buildable.m_iTurnsToConstruct = GetCity()->getProductionTurnsLeft(eUnitForOperation, 0);
		iTempWeight = GC.getAI_CITYSTRATEGY_OPERATION_UNIT_BASE_WEIGHT();
		int iOffenseFlavor = kPlayer.GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE")) + kPlayer.GetMilitaryAI()->GetNumberOfTimesOpsBuildSkippedOver();
		iTempWeight += (GC.getAI_CITYSTRATEGY_OPERATION_UNIT_FLAVOR_MULTIPLIER() * iOffenseFlavor);

		if(GetSpecialization() != NO_CITY_SPECIALIZATION && GC.getCitySpecializationInfo(GetSpecialization())->IsOperationUnitProvider())
		{
			iTempWeight *= 5;
		}


		iTempWeight += m_pUnitProductionAI->GetWeight(eUnitForOperation);

		CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eUnitForOperation);
		if(pkUnitEntry && pkUnitEntry->GetDefaultUnitAIType() == UNITAI_SETTLE)
		{
			if(bEnoughSettlers)
			{
				iTempWeight = 0;
			}
			else
			{
				int iBestArea, iSecondBestArea;
				int iNumGoodAreas = kPlayer.GetBestSettleAreas(kPlayer.GetEconomicAI()->GetMinimumSettleFertility(), iBestArea, iSecondBestArea);
				if(iNumGoodAreas == 0)
				{
					iTempWeight = 0;
				}
			}
		}

		if (iTempWeight > 0)
		{
			m_Buildables.push_back(buildable, iTempWeight);
			kPlayer.GetMilitaryAI()->BumpNumberOfTimesOpsBuildSkippedOver();
		}

	}


	eUnitForArmy = kPlayer.GetMilitaryAI()->GetUnitForArmy(GetCity());
	if(eUnitForArmy != NO_UNIT)
	{
		buildable.m_eBuildableType = CITY_BUILDABLE_UNIT_FOR_ARMY;
		buildable.m_iIndex = (int)eUnitForArmy;
		buildable.m_iTurnsToConstruct = GetCity()->getProductionTurnsLeft(eUnitForArmy, 0);
		iTempWeight = GC.getAI_CITYSTRATEGY_ARMY_UNIT_BASE_WEIGHT();
		int iOffenseFlavor = kPlayer.GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE"));
		int iBonusMultiplier = max(1,GC.getGame().getHandicapInfo().GetID() - 5);
		iTempWeight += (GC.getAI_CITYSTRATEGY_OPERATION_UNIT_FLAVOR_MULTIPLIER() * iOffenseFlavor * iBonusMultiplier);

		iTempWeight += m_pUnitProductionAI->GetWeight(eUnitForArmy);

		if (iTempWeight > 0)
		{
			m_Buildables.push_back(buildable, iTempWeight);
		}
	}

#ifdef AUI_CITYSTRATEGY_FIX_CHOOSE_PRODUCTION_SLIDING_LOGISTIC_MAINTENANCE_SCALE
	bool bCapitolConnectedHasHarbor = false;
	if (GetCity()->IsPuppet() && !GetCity()->IsRouteToCapitalConnected())
	{
		int iLoop;
		const CvPlayer* pPlayer = GetCity()->GetPlayer();
		const CvCity* pLoopCity;
		for (uint uiBuildingTypes = 0; uiBuildingTypes < GC.GetGameBuildings()->GetNumBuildings(); uiBuildingTypes++)
		{
			const BuildingTypes eHarborBuilding = static_cast<BuildingTypes>(uiBuildingTypes);
			CvBuildingEntry* pkHarborBuildingInfo = GC.getBuildingInfo(eHarborBuilding);
			if (pkHarborBuildingInfo && pkHarborBuildingInfo->AllowsWaterRoutes())
			{
				for (pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop))
				{
					if (pLoopCity->IsRouteToCapitalConnected() && pLoopCity->GetCityBuildings()->GetNumActiveBuilding(eHarborBuilding) > 0)
					{
						bCapitolConnectedHasHarbor = true;
						goto EndHarborLoop;
					}
				}
			}
		}
	}
EndHarborLoop:;
#endif


	for(iBldgLoop = 0; iBldgLoop < GC.GetGameBuildings()->GetNumBuildings(); iBldgLoop++)
	{
		const BuildingTypes eLoopBuilding = static_cast<BuildingTypes>(iBldgLoop);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eLoopBuilding);


		if(pkBuildingInfo == NULL)
			continue;


#ifdef AUI_WARNING_FIXES
		if (iBldgLoop != uint(eIgnoreBldg) && m_pCity->canConstruct(eLoopBuilding))
#else
		if(iBldgLoop != eIgnoreBldg && m_pCity->canConstruct(eLoopBuilding))
#endif
		{
			buildable.m_eBuildableType = CITY_BUILDABLE_BUILDING;
			buildable.m_iIndex = iBldgLoop;
			buildable.m_iTurnsToConstruct = GetCity()->getProductionTurnsLeft(eLoopBuilding, 0);

			iTempWeight = m_pBuildingProductionAI->GetWeight(eLoopBuilding);


			if(pkBuildingInfo->IsDiplomaticVoting())
			{
				int iVotesNeededToWin = GC.getGame().GetVotesNeededForDiploVictory();
				int iSecuredVotes = 0;
				TeamTypes myTeamID = kPlayer.getTeam();
				PlayerTypes myPlayerID = kPlayer.GetID();


				PlayerTypes eLoopPlayer;
				TeamTypes eLoopTeam;
				for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
				{
					eLoopPlayer = (PlayerTypes) iPlayerLoop;

					if(GET_PLAYER(eLoopPlayer).isAlive())
					{
						eLoopTeam = GET_PLAYER(eLoopPlayer).getTeam();


						if(GET_TEAM(eLoopTeam).GetLiberatedByTeam() == myTeamID)
						{
							iSecuredVotes++;
						}


						else if(GET_PLAYER(eLoopPlayer).isMinorCiv())
						{

							if(GET_PLAYER(eLoopPlayer).GetMinorCivAI()->GetAlly() == myPlayerID)
							{
								iSecuredVotes++;
							}
						}
					}
				}

				int iNumberOfPlayersWeNeedToBuyOff = MAX(0, iVotesNeededToWin - iSecuredVotes);

				if(!pDiploAI  || !pDiploAI->IsGoingForDiploVictory() || kPlayer.GetTreasury()->GetGold() < iNumberOfPlayersWeNeedToBuyOff * 500)
				{
					iTempWeight = 0;
#ifdef AUI_CITYSTRATEGY_PUPPETS_ALLOW_BAD_BUILDS_IF_NO_OTHER_CHOICE
					continue;
#endif
				}
			}

#ifdef NQM_AI_GIMP_NO_WORLD_WONDERS
			if ((GC.getGame().isOption("GAMEOPTION_AI_TWEAKS") || GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_WORLD_WONDER")) && isWorldWonderClass(pkBuildingInfo->GetBuildingClassInfo()))
			{
				iTempWeight = 0;
				continue;
			}
#endif


			if(GetCity()->IsPuppet())
			{
				const CvBuildingClassInfo& kBuildingClassInfo = pkBuildingInfo->GetBuildingClassInfo();

				if(isWorldWonderClass(kBuildingClassInfo) || isTeamWonderClass(kBuildingClassInfo) || isNationalWonderClass(kBuildingClassInfo) || isLimitedWonderClass(kBuildingClassInfo))
				{
					iTempWeight = 0;
#ifdef AUI_CITYSTRATEGY_PUPPETS_ALLOW_BAD_BUILDS_IF_NO_OTHER_CHOICE
					continue;
#endif
				}
#ifndef AUI_CITYSTRATEGY_FIX_CHOOSE_PRODUCTION_PUPPETS_NULLIFY_BARRACKS

				if(pkBuildingInfo->GetDomainFreeExperience(DOMAIN_LAND))
				{
					iTempWeight = 0;
				}
#endif
#ifdef AUI_CITYSTRATEGY_FIX_CHOOSE_PRODUCTION_SLIDING_LOGISTIC_MAINTENANCE_SCALE
				int iEffectiveMaintenanceT100 = pkBuildingInfo->GetGoldMaintenance() * (100 + GetCity()->GetPlayer()->GetBuildingGoldMaintenanceMod());
				int iBonusGoldFromYield = pkBuildingInfo->GetYieldChange(YIELD_GOLD) + GetCity()->getPopulation() * pkBuildingInfo->GetYieldChangePerPop(YIELD_GOLD);
				iBonusGoldFromYield += GetCity()->GetCityBuildings()->GetBuildingYieldChange((BuildingClassTypes)pkBuildingInfo->GetBuildingClassType(), YIELD_GOLD);
				iBonusGoldFromYield *= GetCity()->getYieldRateModifier(YIELD_GOLD) + pkBuildingInfo->GetYieldModifier(YIELD_GOLD);
				iEffectiveMaintenanceT100 -= iBonusGoldFromYield;

				int iBonusGoldFromModifier = GetCity()->getBaseYieldRate(YIELD_GOLD) * 100;
				iBonusGoldFromModifier += GetCity()->GetYieldPerPopTimes100(YIELD_GOLD) * GetCity()->getPopulation();
				iBonusGoldFromModifier += GetCity()->GetYieldPerReligionTimes100(YIELD_GOLD) * GetCity()->GetCityReligions()->GetNumReligionsWithFollowers();
				iBonusGoldFromModifier *= pkBuildingInfo->GetYieldModifier(YIELD_GOLD);
				iEffectiveMaintenanceT100 -= iBonusGoldFromModifier;

				if (bCapitolConnectedHasHarbor && pkBuildingInfo->AllowsWaterRoutes())
				{
					iEffectiveMaintenanceT100 -= GetCity()->GetPlayer()->GetTreasury()->GetCityConnectionRouteGoldTimes100(GetCity());
				}
				
				iTempWeight *= int(2.0 / (1.0 + exp(double(iEffectiveMaintenanceT100) / 200.0)) + 0.5);
#else

				if(pkBuildingInfo->GetGoldMaintenance() == 0)
				{
					iTempWeight *= 2;
				}
#endif

				int iNumResources = GC.getNumResourceInfos();
				for(int iResourceLoop = 0; iResourceLoop < iNumResources; iResourceLoop++)
				{
					if(pkBuildingInfo->GetResourceQuantityRequirement(iResourceLoop) > 0)
					{
						iTempWeight = 0;
#ifdef AUI_CITYSTRATEGY_PUPPETS_ALLOW_BAD_BUILDS_IF_NO_OTHER_CHOICE
						break;
#endif
					}
				}
#ifdef AUI_CITYSTRATEGY_PUPPETS_ALLOW_BAD_BUILDS_IF_NO_OTHER_CHOICE
				iTempWeight += 1;
#endif
			}
			if(iTempWeight > 0)
				m_Buildables.push_back(buildable, iTempWeight);
		}
	}


	if(!GetCity()->IsPuppet())
	{

		for(iUnitLoop = 0; iUnitLoop < GC.GetGameUnits()->GetNumUnits(); iUnitLoop++)
		{

#ifdef AUI_WARNING_FIXES
			if (iUnitLoop != uint(eIgnoreUnit) &&
#else
			if(iUnitLoop != eIgnoreUnit &&
#endif

			        m_pCity->canTrain((UnitTypes)iUnitLoop))
			{
				buildable.m_eBuildableType = CITY_BUILDABLE_UNIT;
				buildable.m_iIndex = iUnitLoop;
				buildable.m_iTurnsToConstruct = GetCity()->getProductionTurnsLeft((UnitTypes)iUnitLoop, 0);

				iTempWeight = m_pUnitProductionAI->GetWeight((UnitTypes)iUnitLoop);

				CvUnitEntry* pkUnitEntry = GC.getUnitInfo((UnitTypes)iUnitLoop);
				if(pkUnitEntry && pkUnitEntry->GetDefaultUnitAIType() == UNITAI_SETTLE)
				{
					if(bEnoughSettlers)
					{
						iTempWeight = 0;
					}
					else
					{
						int iBestArea, iSecondBestArea;
						int iNumGoodAreas = kPlayer.GetBestSettleAreas(kPlayer.GetEconomicAI()->GetMinimumSettleFertility(), iBestArea, iSecondBestArea);
						if(iNumGoodAreas == 0)
						{
							iTempWeight = 0;
						}
					}
				}


				if (pkUnitEntry)
				{
					DomainTypes eDomain = (DomainTypes) pkUnitEntry->GetDomainType();
					if (eDomain == DOMAIN_SEA && pkUnitEntry->GetDefaultUnitAIType() != UNITAI_WORKER_SEA)
					{
						CvArea* pBiggestNearbyBodyOfWater = m_pCity->waterArea();
						if (pBiggestNearbyBodyOfWater)
						{
							int iWaterTiles = pBiggestNearbyBodyOfWater->getNumTiles();
							int iNumUnitsofMine = pBiggestNearbyBodyOfWater->getUnitsPerPlayer(m_pCity->getOwner());
							if (iNumUnitsofMine * 5 > iWaterTiles)
							{
								iTempWeight = 0;
							}
						}
						else
						{
							iTempWeight = 0;
						}
					}
				}


				if(iTempWeight > 0)
					m_Buildables.push_back(buildable, iTempWeight);
			}
		}


		for(iProjectLoop = 0; iProjectLoop < GC.GetGameProjects()->GetNumProjects(); iProjectLoop++)
		{
#ifdef NQM_AI_GIMP_NO_WORLD_WONDERS
			if ((GC.getGame().isOption("GAMEOPTION_AI_TWEAKS") || GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_WORLD_WONDER")) && isWorldProject((ProjectTypes)iProjectLoop))
				continue;
#endif
			if(m_pCity->canCreate((ProjectTypes)iProjectLoop))
			{
				buildable.m_eBuildableType = CITY_BUILDABLE_PROJECT;
				buildable.m_iIndex = iProjectLoop;
				buildable.m_iTurnsToConstruct = GetCity()->getProductionTurnsLeft((ProjectTypes)iProjectLoop, 0);
				m_Buildables.push_back(buildable, m_pProjectProductionAI->GetWeight((ProjectTypes)iProjectLoop));
			}
		}
	}


	bool bIsVenice = kPlayer.GetPlayerTraits()->IsNoAnnexing();
	if (!GetCity()->IsPuppet() || bIsVenice)
	{	

		if (!GET_PLAYER(m_pCity->getOwner()).isMinorCiv())
		{

			int iBaseYield = GetCity()->getBaseYieldRate(YIELD_PRODUCTION) * 100;
			iBaseYield += (GetCity()->GetYieldPerPopTimes100(YIELD_PRODUCTION) * GetCity()->getPopulation());
			int iModifiedYield = iBaseYield * GetCity()->getBaseYieldRateModifier(YIELD_PRODUCTION);
			iModifiedYield /= 100;

			if (iModifiedYield >= 8)
			{
				for (iProcessLoop = 0; iProcessLoop < GC.getNumProcessInfos(); iProcessLoop++)
				{
					ProcessTypes eProcess = (ProcessTypes)iProcessLoop;
					if (m_pCity->canMaintain(eProcess))
					{
						iTempWeight = m_pProcessProductionAI->GetWeight((ProcessTypes)iProcessLoop);
						buildable.m_eBuildableType = CITY_BUILDABLE_PROCESS;
						buildable.m_iIndex = iProcessLoop;
						buildable.m_iTurnsToConstruct = 1;
						m_Buildables.push_back(buildable, iTempWeight);
					}
				}
			}
		}
	}

	ReweightByCost();

	m_Buildables.SortItems();

	LogPossibleBuilds();

	if(m_Buildables.GetTotalWeight() > 0)
	{

		int iNumChoices = GC.getGame().getHandicapInfo().GetCityProductionNumOptions();
		selection = m_Buildables.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing city build from Top Choices");
		int iRushIfMoreThanXTurns = GC.getAI_ATTEMPT_RUSH_OVER_X_TURNS_TO_BUILD();
		if(GET_PLAYER(m_pCity->getOwner()).isMinorCiv())
		{
			iRushIfMoreThanXTurns *= GC.getMINOR_CIV_PRODUCTION_PERCENT();
			iRushIfMoreThanXTurns /= 100;
		}

		bool bRush = selection.m_iTurnsToConstruct > iRushIfMoreThanXTurns;
		LogCityProduction(selection, bRush);

		switch(selection.m_eBuildableType)
		{
		case CITY_BUILDABLE_UNIT:
		case CITY_BUILDABLE_UNIT_FOR_ARMY:
		{
			UnitTypes eUnitType = (UnitTypes) selection.m_iIndex;
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnitType);
			if(pkUnitInfo)
			{
				UnitAITypes eUnitAI = (UnitAITypes) pkUnitInfo->GetDefaultUnitAIType();
				GetCity()->pushOrder(ORDER_TRAIN, eUnitType, eUnitAI, false, true, false, bRush);
			}
		}
		break;

		case CITY_BUILDABLE_BUILDING:
		{
			BuildingTypes eBuildingType = (BuildingTypes) selection.m_iIndex;
			GetCity()->pushOrder(ORDER_CONSTRUCT, eBuildingType, -1, false, true, false, bRush);
		}
		break;

		case CITY_BUILDABLE_PROJECT:
		{
			ProjectTypes eProjectType = (ProjectTypes) selection.m_iIndex;
			GetCity()->pushOrder(ORDER_CREATE, eProjectType, -1, false, true, false, bRush);
		}
		break;

		case CITY_BUILDABLE_PROCESS:
		{
			ProcessTypes eProcessType = (ProcessTypes)selection.m_iIndex;
			GetCity()->pushOrder(ORDER_MAINTAIN, eProcessType, -1, false, true, false, false);
		}

		case CITY_BUILDABLE_UNIT_FOR_OPERATION:
		{
			GetCity()->CommitToBuildingUnitForOperation();
			kPlayer.GetMilitaryAI()->ResetNumberOfTimesOpsBuildSkippedOver();
		}
		break;
		}

	}

	return;
}


void CvCityStrategyAI::DoTurn()
{
	AI_PERF_FORMAT("City-AI-perf.csv", ("CvCityStrategyAI::DoTurn, Turn %03d, %s, %s", GC.getGame().getElapsedGameTurns(), m_pCity->GetPlayer()->getCivilizationShortDescription(), m_pCity->getName().c_str()) );

	int iCityStrategiesLoop = 0;


	for(iCityStrategiesLoop = 0; iCityStrategiesLoop < GetAICityStrategies()->GetNumAICityStrategies(); iCityStrategiesLoop++)
	{
		AICityStrategyTypes eCityStrategy = (AICityStrategyTypes) iCityStrategiesLoop;
		CvAICityStrategyEntry* pCityStrategy = GetAICityStrategies()->GetEntry(iCityStrategiesLoop);


		if(GET_PLAYER(GetCity()->getOwner()).isMinorCiv() && pCityStrategy->IsNoMinorCivs())
		{
			continue;
		}

		bool bTestCityStrategyStart = true;


		if(IsUsingCityStrategy(eCityStrategy))
		{
			bTestCityStrategyStart = false;
		}
		else
		{

			if(pCityStrategy->GetTechPrereq() != NO_TECH && !GET_TEAM(GetCity()->getTeam()).GetTeamTechs()->HasTech((TechTypes) pCityStrategy->GetTechPrereq()))
			{
				bTestCityStrategyStart = false;
			}


			if(bTestCityStrategyStart && pCityStrategy->GetTechObsolete() != NO_TECH && GET_TEAM(GetCity()->getTeam()).GetTeamTechs()->HasTech((TechTypes) pCityStrategy->GetTechObsolete()))
			{
				bTestCityStrategyStart = false;
			}
		}

		bool bTestCityStrategyEnd = false;


		if(IsUsingCityStrategy(eCityStrategy))
		{

			if(!pCityStrategy->IsPermanent())
			{
				if(pCityStrategy->GetCheckTriggerTurnCount() > 0)
				{

					if((GC.getGame().getGameTurn() - GetTurnCityStrategyAdopted(eCityStrategy)) % pCityStrategy->GetCheckTriggerTurnCount() == 0)
					{
						bTestCityStrategyEnd = true;
					}
				}

				if(bTestCityStrategyEnd && pCityStrategy->GetMinimumNumTurnsExecuted() > 0)
				{

					if(GC.getGame().getGameTurn() < GetTurnCityStrategyAdopted(eCityStrategy) + pCityStrategy->GetMinimumNumTurnsExecuted())
					{
						bTestCityStrategyEnd = false;
					}
				}
			}
		}




		if(bTestCityStrategyStart || bTestCityStrategyEnd)
		{
			bool bStrategyShouldBeActive = false;


			if(pCityStrategy->GetTechObsolete() != NO_TECH && GET_TEAM(GetCity()->getTeam()).GetTeamTechs()->HasTech((TechTypes) pCityStrategy->GetTechObsolete()))
			{
				bStrategyShouldBeActive = false;
			}

			else
			{
				CvString strStrategyName = (CvString) pCityStrategy->GetType();


				if(strStrategyName == "AICITYSTRATEGY_TINY_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_TinyCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_SMALL_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_SmallCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_MEDIUM_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_MediumCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_LARGE_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_LargeCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_LANDLOCKED")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_Landlocked(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_NEED_TILE_IMPROVERS")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedTileImprovers(eCityStrategy, GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_WANT_TILE_IMPROVERS")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_WantTileImprovers(eCityStrategy, GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_ENOUGH_TILE_IMPROVERS")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_EnoughTileImprovers(eCityStrategy, GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_NEED_NAVAL_GROWTH")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedNavalGrowth(eCityStrategy, GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_NEED_NAVAL_TILE_IMPROVEMENT")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedNavalTileImprovement(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_ENOUGH_NAVAL_TILE_IMPROVEMENT")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_EnoughNavalTileImprovement(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_NEED_IMPROVEMENT_FOOD")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedImprovement(GetCity(), YIELD_FOOD);
				else if(strStrategyName == "AICITYSTRATEGY_NEED_IMPROVEMENT_PRODUCTION")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedImprovement(GetCity(), YIELD_PRODUCTION);
				else if(strStrategyName == "AICITYSTRATEGY_HAVE_TRAINING_FACILITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_HaveTrainingFacility(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_CAPITAL_NEED_SETTLER")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_CapitalNeedSettler(eCityStrategy, GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_CAPITAL_UNDER_THREAT")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_CapitalUnderThreat(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_FIRST_CULTURE_BUILDING_EMERGENCY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_FirstCultureBuildingEmergency(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_FIRST_CULTURE_BUILDING")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_FirstCultureBuilding(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_FIRST_SCIENCE_BUILDING")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_FirstScienceBuilding(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_FIRST_GOLD_BUILDING")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_FirstGoldBuilding(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_FIRST_FAITH_BUILDING")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_FirstFaithBuilding(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_FIRST_PRODUCTION_BUILDING")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_FirstProductionBuilding(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_UNDER_BLOCKADE")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_UnderBlockade(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_IS_PUPPET")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_IsPuppet(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_MEDIUM_CITY_HIGH_DIFFICULTY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_MediumCityHighDifficulty(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_ORIGINAL_CAPITAL")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_OriginalCapital(GetCity());

				else if(strStrategyName == "AICITYSTRATEGY_RIVER_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_RiverCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_MOUNTAIN_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_MountainCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_FOREST_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_ForestCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_HILL_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_HillCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_JUNGLE_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_JungleCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_COAST_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_CoastCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_MANY_TECHS_STOLEN")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_ManyTechsStolen(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_KEY_SCIENCE_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_KeyScienceCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_GOOD_GP_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_GoodGPCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_NEED_INTERNATIONAL_LAND_TRADE_ROUTE")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedInternationalTradeRoute(GetCity(), DOMAIN_LAND);
				else if(strStrategyName == "AICITYSTRATEGY_NO_NEED_INTERNATIONAL_LAND_TRADE_ROUTE")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NoNeedInternationalTradeRoute(GetCity(), DOMAIN_LAND);
				else if(strStrategyName == "AICITYSTRATEGY_NEED_INTERNATIONAL_SEA_TRADE_ROUTE")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedInternationalTradeRoute(GetCity(), DOMAIN_SEA);
				else if(strStrategyName == "AICITYSTRATEGY_NO_NEED_INTERNATIONAL_SEA_TRADE_ROUTE")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NoNeedInternationalTradeRoute(GetCity(), DOMAIN_SEA);
				else if(strStrategyName == "AICITYSTRATEGY_INTERNATIONAL_TRADE_DESTINATION")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_IsInternationalTradeDestination(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_INTERNATIONAL_TRADE_ORIGIN")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_IsInternationalTradeOrigin(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_NEED_CULTURE_BUILDING")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedCultureBuilding(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_NEED_TOURISM_BUILDING")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedTourismBuilding(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_GOOD_AIRLIFT_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_GoodAirliftCity(GetCity());


				ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
				if(pkScriptSystem && bStrategyShouldBeActive)
				{
					CvLuaArgsHandle args;
					args->Push(iCityStrategiesLoop);
					args->Push(GetCity()->getOwner());
					args->Push(GetCity()->GetID());



					bool bResult = false;
					if(LuaSupport::CallTestAll(pkScriptSystem, "CityStrategyCanActivate", args.get(), bResult))
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
				if(bTestCityStrategyStart)
				{
					bAdoptOrEndStrategy = true;
				}
				else if(bTestCityStrategyEnd)
				{
					bAdoptOrEndStrategy = false;
				}
			}

			else
			{
				if(bTestCityStrategyStart)
				{
					bAdoptOrEndStrategy = false;
				}
				else if(bTestCityStrategyEnd)
				{
					bAdoptOrEndStrategy = true;
				}
			}


			if(bAdoptOrEndStrategy)
			{
				int iFlavorLoop;


				if(bTestCityStrategyStart)
				{
					SetUsingCityStrategy(eCityStrategy, true);

					const int iFlavorMinValue =           GC.getFLAVOR_MIN_VALUE();
					const int iFlavorMaxValue =          GC.getFLAVOR_MAX_VALUE();

					const int iNumFlavors = GC.getNumFlavorTypes();
					for(iFlavorLoop = 0; iFlavorLoop < iNumFlavors; iFlavorLoop++)
					{
						if(pCityStrategy->GetFlavorValue(iFlavorLoop) != 0)
						{
							m_piLatestFlavorValues[iFlavorLoop] += pCityStrategy->GetFlavorValue(iFlavorLoop);

							if(m_piLatestFlavorValues[iFlavorLoop] < iFlavorMinValue)
							{
								m_piLatestFlavorValues[iFlavorLoop] = iFlavorMinValue;
							}
							else if(m_piLatestFlavorValues[iFlavorLoop] > iFlavorMaxValue)
							{
								m_piLatestFlavorValues[iFlavorLoop] = iFlavorMaxValue;
							}

							LogFlavors((FlavorTypes) iFlavorLoop);
						}
					}

					FlavorUpdate();
				}

				else if(bTestCityStrategyEnd)
				{
					SetUsingCityStrategy(eCityStrategy, false);

					const int iFlavorMinValue =           GC.getFLAVOR_MIN_VALUE();
					const int iFlavorMaxValue =          GC.getFLAVOR_MAX_VALUE();

					const int iNumFlavors = GC.getNumFlavorTypes();
					for(iFlavorLoop = 0; iFlavorLoop < iNumFlavors; iFlavorLoop++)
					{
						if(pCityStrategy->GetFlavorValue(iFlavorLoop) != 0)
						{
							m_piLatestFlavorValues[iFlavorLoop] -= pCityStrategy->GetFlavorValue(iFlavorLoop);

							if(m_piLatestFlavorValues[iFlavorLoop] < iFlavorMinValue)
							{
								m_piLatestFlavorValues[iFlavorLoop] = iFlavorMinValue;
							}
							else if(m_piLatestFlavorValues[iFlavorLoop] > iFlavorMaxValue)
							{
								m_piLatestFlavorValues[iFlavorLoop] = iFlavorMaxValue;
							}

							LogFlavors((FlavorTypes) iFlavorLoop);
						}
					}

					FlavorUpdate();
				}
			}
		}
	}
}

void CvCityStrategyAI::ResetBestYields()
{
	for(uint uiYields = 0; uiYields < NUM_YIELD_TYPES; uiYields++)
	{
		for(uint uiPlots = 0; uiPlots < NUM_CITY_PLOTS - 1; uiPlots++)
		{
			m_acBestYields[uiYields][uiPlots] = MAX_UNSIGNED_CHAR;
		}

		m_asBestYieldAverageTimes100[uiYields] = MAX_SHORT;
	}
}

void CvCityStrategyAI::UpdateBestYields()
{
	m_eFocusYield = (YieldTypes)NO_YIELD;

	ResetBestYields();

	int iPopulationToEvaluate = min(m_pCity->getPopulation() + 2, NUM_CITY_PLOTS);
	CvPlot* pPlot = NULL;
	uint uiPlotsEvaluated = 0;

	struct ReverseSort
	{
		bool operator()(unsigned char ucA, unsigned char ucB)
		{
			return ucA > ucB;
		}
	};

	for(int iPlotLoop = 0; iPlotLoop < NUM_CITY_PLOTS; iPlotLoop++)
	{






		pPlot = m_pCity->GetCityCitizens()->GetCityPlotFromIndex(iPlotLoop);
		if(!pPlot)
		{
			continue;
		}

		CvCity* pWorkingCity = pPlot->getWorkingCity();
		if(!pWorkingCity || pWorkingCity->GetID() != m_pCity->GetID())
		{
			continue;
		}

		for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
		{
			YieldTypes eYield = (YieldTypes)iYield;
			int iYieldAmount = pPlot->getYield(eYield);

			m_acBestYields[iYield][uiPlotsEvaluated] = iYieldAmount;
		}
		uiPlotsEvaluated++;
	}

	for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		std::stable_sort(m_acBestYields[iYield], m_acBestYields[iYield] + uiPlotsEvaluated, ReverseSort());
	}

	int iSlotsToEvaluate = MIN((int)uiPlotsEvaluated, iPopulationToEvaluate);
	CvAssertMsg(iSlotsToEvaluate > 0, "iSlotsToEvaluate is zero, trying to div by 0");
	if(iSlotsToEvaluate <= 0)
	{
		return;
	}

	for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		int iYieldSum = 0;
		for(int iSlot = 0; iSlot < iSlotsToEvaluate; iSlot++)
		{
			iYieldSum += (m_acBestYields[iYield][iSlot] * 100);
		}


		if(iYield == YIELD_FOOD)
		{
			int iCityYieldSum = 0;
			iCityYieldSum = m_pCity->plot()->getYield(YIELD_FOOD);
			CvYieldInfo* pYield = GC.getYieldInfo(YIELD_FOOD);
			if(pYield)
			{
				iCityYieldSum -= pYield->getMinCity();
			}

			CvCityBuildings* pCityBuildings = m_pCity->GetCityBuildings();
			BuildingTypes eBuilding;
#ifdef AUI_WARNING_FIXES
			for (uint iI = 0; iI < GC.getNumBuildingInfos(); iI++)
#else
			for(int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
#endif
			{
				eBuilding = (BuildingTypes) iI;
				CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
				if(pkBuildingInfo)
				{
					if(pCityBuildings->GetNumBuilding(eBuilding) > 0)
					{
						iCityYieldSum += pkBuildingInfo->GetYieldChange(iYield);
					}
				}
			}

			iYieldSum += iCityYieldSum * 100;
		}

		m_asBestYieldAverageTimes100[iYield] = iYieldSum / iSlotsToEvaluate;

		CvAssertMsg(m_asBestYieldAverageTimes100[iYield] < 750, "Crazy high yield");
	}

	CitySpecializationTypes eSpecialization = m_pCity->GetCityStrategyAI()->GetSpecialization();
	if(eSpecialization == NO_CITY_SPECIALIZATION)
	{
		if(m_pCity->GetPlayer()->isHuman())
		{

			CityAIFocusTypes eCityAIFocusTypes = m_pCity->GetCityCitizens()->GetFocusType();

			CvString strLookup;
			switch(eCityAIFocusTypes)
			{
			case CITY_AI_FOCUS_TYPE_FOOD:
				strLookup = "CITYSPECIALIZATION_SETTLER_PUMP";
				break;
			case CITY_AI_FOCUS_TYPE_GOLD:
				strLookup = "CITYSPECIALIZATION_COMMERCE";
				break;
			}

			if(!strLookup.IsEmpty())
			{
				std::vector<CvCitySpecializationXMLEntry*> aCitySpecializations = GC.getCitySpecializationInfo();
				for(uint ui = 0; ui < aCitySpecializations.size(); ui++)
				{
					CvCitySpecializationXMLEntry* pEntry = aCitySpecializations[ui];
					if(pEntry->GetType() == strLookup)
					{
						eSpecialization = (CitySpecializationTypes)ui;
						break;
					}
				}
			}
		}


		if(eSpecialization == NO_CITY_SPECIALIZATION)
		{
			std::vector<CvCitySpecializationXMLEntry*> m_aCitySpecializations;
			m_aCitySpecializations = GC.getCitySpecializationInfo();
			for(uint ui = 0; ui < m_aCitySpecializations.size(); ui++)
			{
				if(m_aCitySpecializations[ui]->IsDefault())
				{
					eSpecialization = (CitySpecializationTypes)ui;
					break;
				}
			}
		}
	}

	CvCitySpecializationXMLEntry* pEntry = NULL;
	if(eSpecialization != NO_CITY_SPECIALIZATION)
	{
		pEntry = GC.getCitySpecializationInfo(eSpecialization);
	}

	if(!pEntry)
	{
		return;
	}

	for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		m_asYieldDeltaTimes100[iYield] = m_asBestYieldAverageTimes100[iYield] - pEntry->GetYieldTargetTimes100((YieldTypes)iYield);
	}

	m_eFocusYield = pEntry->GetYieldType();
}

#ifdef AUI_WARNING_FIXES
unsigned int CvCityStrategyAI::GetBestYieldAverageTimes100(YieldTypes eYield)
#else
unsigned short CvCityStrategyAI::GetBestYieldAverageTimes100(YieldTypes eYield)
#endif
{
	return m_asBestYieldAverageTimes100[eYield];
}

#ifdef AUI_WARNING_FIXES
int CvCityStrategyAI::GetYieldDeltaTimes100(YieldTypes eYield)
#else
short CvCityStrategyAI::GetYieldDeltaTimes100(YieldTypes eYield)
#endif
{
	return m_asYieldDeltaTimes100[eYield];
}

YieldTypes CvCityStrategyAI::GetFocusYield()
{
	return m_eFocusYield;
}

void CvCityStrategyAI::LogHurry(HurryTypes iHurryType, int iHurryAmount, int iHurryAmountAvailable, int iTurnsSaved)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString cityName;
		CvString strDesc;


		playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
		cityName = m_pCity->getName();


		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", ";

		if(iHurryType == 0)
		{
			strTemp.Format("Pop rushed: %s, Spent: %d, Available: %d, Turns Saved: %d", m_pCity->getProductionName(), iHurryAmount, iHurryAmountAvailable, iTurnsSaved);
		}
		else
		{
			strTemp.Format("Gold rushed: %s, Spent: %d, Available: %d, Turns Saved: %d", m_pCity->getProductionName(), iHurryAmount, iHurryAmountAvailable, iTurnsSaved);
		}

		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}
}





void CvCityStrategyAI::ReweightByCost()
{
	CvCityBuildable buildable;

	for(int iI = 0; iI < m_Buildables.size(); iI++)
	{
		buildable = m_Buildables.GetElement(iI);


		int iNewWeight = CityStrategyAIHelpers::ReweightByTurnsLeft(m_Buildables.GetWeight(iI), buildable.m_iTurnsToConstruct);
		m_Buildables.SetWeight(iI, iNewWeight);
	}
}


void CvCityStrategyAI::LogFlavors(FlavorTypes eFlavor)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString cityName;
		CvString strDesc;


		playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
		cityName = m_pCity->getName();


		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", ";


		if(eFlavor == NO_FLAVOR)
		{
			for(int iI = 0; iI < GC.getNumFlavorTypes(); iI++)
			{


				{
					strTemp.Format("Flavor, %s, %d", GC.getFlavorTypes((FlavorTypes)iI).GetCString(), m_piLatestFlavorValues[iI]);
					strOutBuf = strBaseString + strTemp;
					pLog->Msg(strOutBuf);
				}
			}
		}
		else
		{
			strTemp.Format("Flavor, %s, %d", GC.getFlavorTypes(eFlavor).GetCString(), m_piLatestFlavorValues[eFlavor]);
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}


void CvCityStrategyAI::LogStrategy(AICityStrategyTypes eStrategy, bool bValue)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;


		CvString playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
		CvString cityName = m_pCity->getName();


		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", ";

		strOutBuf = strBaseString;


		CvAICityStrategyEntry* pStrategyEntry = GC.getAICityStrategyInfo(eStrategy);
		if(pStrategyEntry != NULL)
		{
			CvString strTemp;
			strTemp.Format("%s, %d", pStrategyEntry->GetType(), bValue);
			strOutBuf += strTemp;
		}

		pLog->Msg(strOutBuf);
	}
}


void CvCityStrategyAI::LogPossibleBuilds()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString cityName;
		CvString strDesc;


		playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
		cityName = m_pCity->getName();


		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", ";


		for(int iI = 0; iI < m_Buildables.size(); iI++)
		{
			CvCityBuildable buildable = m_Buildables.GetElement(iI);

			switch(buildable.m_eBuildableType)
			{
			case CITY_BUILDABLE_BUILDING:
			{
				CvBuildingEntry* pEntry = GC.GetGameBuildings()->GetEntry(buildable.m_iIndex);
				if(pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Building, %s, %d, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_UNIT:
			{
				CvUnitEntry* pEntry = GC.GetGameUnits()->GetEntry(buildable.m_iIndex);
				if(pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Unit, %s, %d, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_PROJECT:
			{
				CvProjectEntry* pEntry = GC.GetGameProjects()->GetEntry(buildable.m_iIndex);
				if(pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Project, %s, %d, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_PROCESS:
			{
				CvProcessInfo* pProcess = GC.getProcessInfo((ProcessTypes)buildable.m_iIndex);
				if (pProcess != NULL)
				{
					strDesc = pProcess->GetDescription();
					strTemp.Format("Process, %s, %d, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_UNIT_FOR_OPERATION:
			{
				CvUnitEntry* pEntry = GC.GetGameUnits()->GetEntry(buildable.m_iIndex);
				if(pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Operation unit, %s, %d, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_UNIT_FOR_ARMY:
			{
				CvUnitEntry* pEntry = GC.GetGameUnits()->GetEntry(buildable.m_iIndex);
				if(pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Army unit, %s, %d, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			}
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}


#ifdef AUI_WARNING_FIXES
void CvCityStrategyAI::LogCityProduction(const CvCityBuildable& buildable, bool bRush)
#else
void CvCityStrategyAI::LogCityProduction(CvCityBuildable buildable, bool bRush)
#endif
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString cityName;
		CvString strDesc;


		playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
		cityName = m_pCity->getName();

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", ";

		switch(buildable.m_eBuildableType)
		{
		case CITY_BUILDABLE_BUILDING:
		{
			CvBuildingEntry* pEntry = GC.GetGameBuildings()->GetEntry(buildable.m_iIndex);
			if(pEntry != NULL)
			{
				strDesc = pEntry->GetDescription();
			}
		}
		break;
		case CITY_BUILDABLE_UNIT:
		case CITY_BUILDABLE_UNIT_FOR_OPERATION:
		case CITY_BUILDABLE_UNIT_FOR_ARMY:
		{
			CvUnitEntry* pEntry = GC.GetGameUnits()->GetEntry(buildable.m_iIndex);
			if(pEntry != NULL)
			{
				strDesc = pEntry->GetDescription();

			}
		}
		break;
		case CITY_BUILDABLE_PROJECT:
		{
			CvProjectEntry* pEntry = GC.GetGameProjects()->GetEntry(buildable.m_iIndex);
			if(pEntry != NULL)
			{
				strDesc = pEntry->GetDescription();

			}
		}
		break;
		case CITY_BUILDABLE_PROCESS:
		{
			CvProcessInfo* pEntry = GC.getProcessInfo((ProcessTypes)buildable.m_iIndex);
			if (pEntry != NULL)
			{
				strDesc = pEntry->GetDescription();
			}
		}
		break;
		}

		if(bRush)
		{
			strTemp.Format("SEED: %d, CHOSEN: %s, Rush if possible, TURNS: %d", GC.getGame().getJonRand().getSeed(), strDesc.c_str(), buildable.m_iTurnsToConstruct);
		}
		else
		{
			strTemp.Format("SEED: %d, CHOSEN: %s, Do not rush, TURNS: %d", GC.getGame().getJonRand().getSeed(), strDesc.c_str(), buildable.m_iTurnsToConstruct);
		}

		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}
}

void CvCityStrategyAI::LogSpecializationChange(CitySpecializationTypes eSpecialization)
{
	if(GC.getLogging() && GC.getAILogging())
	{

		CvString playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
		CvString cityName = m_pCity->getName();

		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);


		CvString strBaseString;
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", ";

		CvString strOutBuf = strBaseString;

		CvCitySpecializationXMLEntry* pCitySpecializationInfo = GC.getCitySpecializationInfo(eSpecialization);
		if(pCitySpecializationInfo != NULL)
		{
			const YieldTypes eYieldType = pCitySpecializationInfo->GetYieldType();

			CvString strYieldString = "Unknown";
			if(eYieldType == NO_YIELD)
			{
				strYieldString = "General Economy";
			}
			else
			{
				CvYieldInfo* pYieldInfo = GC.getYieldInfo(eYieldType);
				if(pYieldInfo != NULL)
				{
					strYieldString = pYieldInfo->GetDescription();
				}
			}

			CvString strTemp = "NEW SPECIALIZATION: Yield Type = ";
			strOutBuf += strTemp + strYieldString;
		}

		pLog->Msg(strOutBuf);
	}
}









int CityStrategyAIHelpers::ReweightByTurnsLeft(int iOriginalWeight, int iTurnsLeft)
{

	double fAdditionalTurnCostFactor = GC.getAI_PRODUCTION_WEIGHT_MOD_PER_TURN_LEFT() * iTurnsLeft;
	double fTotalCostFactor = GC.getAI_PRODUCTION_WEIGHT_BASE_MOD() + fAdditionalTurnCostFactor;
	double fWeightDivisor = pow((double) iTurnsLeft, fTotalCostFactor);























	return int(double(iOriginalWeight) / fWeightDivisor);
}


int CityStrategyAIHelpers::GetWeightThresholdModifier(AICityStrategyTypes eStrategy, CvCity* pCity)
{
	int iWeightThresholdModifier = 0;


	for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		FlavorTypes eFlavor = (FlavorTypes) iFlavorLoop;
		int iPersonalityFlavor = GET_PLAYER(pCity->getOwner()).GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor);
		int iStrategyFlavorMod = pCity->GetCityStrategyAI()->GetAICityStrategies()->GetEntry(eStrategy)->GetPersonalityFlavorThresholdMod(eFlavor);

		iWeightThresholdModifier += (iPersonalityFlavor * iStrategyFlavorMod);
	}

	return iWeightThresholdModifier;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_TinyCity(CvCity* pCity)
{

	if(pCity->getPopulation() < GC.getAI_CITYSTRATEGY_SMALL_CITY_POP_THRESHOLD())
	{
		return true;
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_SmallCity(CvCity* pCity)
{

	if(pCity->getPopulation() >= GC.getAI_CITYSTRATEGY_SMALL_CITY_POP_THRESHOLD() &&
	        pCity->getPopulation() < GC.getAI_CITYSTRATEGY_MEDIUM_CITY_POP_THRESHOLD())
	{
		return true;
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_MediumCity(CvCity* pCity)
{

	if (pCity->isCapital())
	{
		return false;
	}


	if(pCity->getPopulation() >= GC.getAI_CITYSTRATEGY_MEDIUM_CITY_POP_THRESHOLD() &&
	        pCity->getPopulation() < GC.getAI_CITYSTRATEGY_LARGE_CITY_POP_THRESHOLD())
	{
		return true;
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_LargeCity(CvCity* pCity)
{

	if(pCity->getPopulation() >= GC.getAI_CITYSTRATEGY_LARGE_CITY_POP_THRESHOLD())
	{
		return true;
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_Landlocked(CvCity* pCity)
{

	if(!pCity->isCoastal())
	{
		return true;
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_NeedTileImprovers(AICityStrategyTypes eStrategy, CvCity* pCity)
{
	CvPlayer& kPlayer = GET_PLAYER(pCity->getOwner());
#ifdef AUI_CITYSTRATEGY_DONT_EMPHASIZE_WORKERS_IF_NO_MILITARY
	if (kPlayer.getNumMilitaryUnits() - kPlayer.GetNumUnitsWithUnitAI(UNITAI_EXPLORE) <= 0)
	{
		return false;
	}
#endif
	int iCurrentNumCities = kPlayer.getNumCities();

	int iLastTurnWorkerDisbanded = kPlayer.GetEconomicAI()->GetLastTurnWorkerDisbanded();
#ifdef AUI_CITYSTRATEGY_FIX_TILE_IMPROVERS_LAST_DISBAND_WORKER_TURN_SCALE
	if (iLastTurnWorkerDisbanded >= 0 && GC.getGame().getGameTurn() - iLastTurnWorkerDisbanded <= 25 * GC.getGame().getEstimateEndTurn() / 500)
#else
	if(iLastTurnWorkerDisbanded >= 0 && GC.getGame().getGameTurn() - iLastTurnWorkerDisbanded <= 25)
#endif
	{
		return false;
	}

	int iNumWorkers = kPlayer.GetNumUnitsWithUnitAI(UNITAI_WORKER, true, false);

	if(kPlayer.isMinorCiv())
	{
		if(iNumWorkers >= iCurrentNumCities)
			return false;
	}
	else
	{
		int iNumCities = max(1, (iCurrentNumCities * 3) / 4);
		if(iNumWorkers >= iNumCities)
			return false;

		if(kPlayer.GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
			return false;
	}


	if(iCurrentNumCities <= 1)
	{
		CvMilitaryAI* pMilitaryAI =kPlayer.GetMilitaryAI();
		MilitaryAIStrategyTypes eStrategyKillBarbs = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_ERADICATE_BARBARIANS");
		if(eStrategyKillBarbs != NO_MILITARYAISTRATEGY)
		{
			if(pMilitaryAI->IsUsingStrategy(eStrategyKillBarbs))
			{
				return false;
			}
		}
	}

	CvAICityStrategyEntry* pCityStrategy = pCity->GetCityStrategyAI()->GetAICityStrategies()->GetEntry(eStrategy);

	int iModdedNumWorkers = iNumWorkers*        pCityStrategy->GetWeightThreshold() / 100;
	int iModdedNumCities = iCurrentNumCities + kPlayer.countCitiesFeatureSurrounded();


	if(iModdedNumWorkers <= iModdedNumCities || iModdedNumWorkers == 0)
	{

		int iDesperateTurn =        GC.getAI_CITYSTRATEGY_NEED_TILE_IMPROVERS_DESPERATE_TURN();

		iDesperateTurn *= GC.getGame().getGameSpeedInfo().getTrainPercent();
		iDesperateTurn /= 100;

		if(GC.getGame().getElapsedGameTurns() > iDesperateTurn)
			return true;
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_WantTileImprovers(AICityStrategyTypes eStrategy, CvCity* pCity)
{
	CvPlayer& kPlayer = GET_PLAYER(pCity->getOwner());
#ifdef AUI_CITYSTRATEGY_DONT_EMPHASIZE_WORKERS_IF_NO_MILITARY
	if (kPlayer.getNumMilitaryUnits() - kPlayer.GetNumUnitsWithUnitAI(UNITAI_EXPLORE) <= 0)
	{
		return false;
	}
#endif
	int iLastTurnWorkerDisbanded = kPlayer.GetEconomicAI()->GetLastTurnWorkerDisbanded();
#ifdef AUI_CITYSTRATEGY_FIX_TILE_IMPROVERS_LAST_DISBAND_WORKER_TURN_SCALE
	if (iLastTurnWorkerDisbanded >= 0 && GC.getGame().getGameTurn() - iLastTurnWorkerDisbanded <= 10 * GC.getGame().getEstimateEndTurn() / 500)
#else
	if(iLastTurnWorkerDisbanded >= 0 && GC.getGame().getGameTurn() - iLastTurnWorkerDisbanded <= 10)
#endif
	{
		return false;
	}

	if(!GET_PLAYER(pCity->getOwner()).isMinorCiv())
	{

		if(GET_PLAYER(pCity->getOwner()).GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
			return false;
	}

	int iNumWorkers = kPlayer.GetNumUnitsWithUnitAI(UNITAI_WORKER, true, false);
	if(iNumWorkers >= ((kPlayer.getNumCities() *  3) / 2) + 1)
		return false;













	if(pCity->getPopulation() >=       GC.getAI_CITYSTRATEGY_WANT_TILE_IMPROVERS_MINIMUM_SIZE())
	{
		int iNumBuilders = kPlayer.GetNumUnitsWithUnitAI(UNITAI_WORKER, true, false);


		if(iNumBuilders < 1)
		{
			return true;
		}

		CvAICityStrategyEntry* pCityStrategy = pCity->GetCityStrategyAI()->GetAICityStrategies()->GetEntry(eStrategy);

		int iWeightThresholdModifier = CityStrategyAIHelpers::GetWeightThresholdModifier(eStrategy, pCity);
		int iPerCityThreshold = pCityStrategy->GetWeightThreshold() + iWeightThresholdModifier;


		CvPlot* pLoopPlot;
		int iNumResources = 0;
		int iNumImprovedResources = 0;

		for(int iPlotLoop = 0; iPlotLoop < NUM_CITY_PLOTS; iPlotLoop++)
		{
			pLoopPlot = plotCity(pCity->getX(), pCity->getY(), iPlotLoop);

			if(pLoopPlot != NULL)
			{
				if(pLoopPlot->getOwner() == pCity->getOwner())
				{
					if(!pLoopPlot->isWater())
					{
						ResourceTypes eResource = pLoopPlot->getResourceType(kPlayer.getTeam());
						if(eResource == NO_RESOURCE)
						{
							continue;
						}


						ImprovementTypes eCorrectImprovement = NO_IMPROVEMENT;
						BuildTypes eCorrectBuild = NO_BUILD;
#ifdef AUI_WARNING_FIXES
						for (uint iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
#else
						int iBuildIndex;
						for(iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
#endif
						{
							const BuildTypes eBuild = static_cast<BuildTypes>(iBuildIndex);
							CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
							if(pkBuildInfo)
							{
								const ImprovementTypes eImprovement = (ImprovementTypes)pkBuildInfo->getImprovement();


								if(eImprovement != NO_IMPROVEMENT)
								{
									CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
									if(pkImprovementInfo)
									{
										if(pkImprovementInfo->IsImprovementResourceTrade(eResource))
										{
											eCorrectBuild = eBuild;
											eCorrectImprovement = eImprovement;
											break;
										}
									}
								}
							}

						}


						if(eCorrectBuild == NO_BUILD || eCorrectImprovement == NO_IMPROVEMENT)
						{
							continue;
						}



						if(!kPlayer.canBuild(pLoopPlot, eCorrectBuild, false, false))
						{
							if(pLoopPlot->getImprovementType() == eCorrectImprovement)
							{
								iNumImprovedResources++;
								iNumResources++;
							}

							continue;
						}

						iNumResources++;
					}
				}
			}
		}

		bool bManyUnimproveResources = (2 * (iNumResources - iNumImprovedResources)) > iNumResources;
		int iMultiplier = kPlayer.getNumCities();
		iMultiplier += kPlayer.countCitiesFeatureSurrounded();
		if(bManyUnimproveResources)
			iMultiplier += 1;

		int iNumSettlers = kPlayer.GetNumUnitsWithUnitAI(UNITAI_SETTLE, true, false);
		iMultiplier += iNumSettlers;

		int iWeightThreshold = (iPerCityThreshold * iMultiplier);


		if((iNumBuilders * 100) < iWeightThreshold)
		{


			return (kPlayer.calculateGoldRate() > -10);






















		}
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_EnoughTileImprovers(AICityStrategyTypes eStrategy, CvCity* pCity)
{
	CvPlayer& kPlayer = GET_PLAYER(pCity->getOwner());
#ifdef AUI_CITYSTRATEGY_DONT_EMPHASIZE_WORKERS_IF_NO_MILITARY
	if (kPlayer.getNumMilitaryUnits() - kPlayer.GetNumUnitsWithUnitAI(UNITAI_EXPLORE) <= 0)
	{
		return true;
	}
#endif
	int iLastTurnWorkerDisbanded = kPlayer.GetEconomicAI()->GetLastTurnWorkerDisbanded();
#ifdef AUI_CITYSTRATEGY_FIX_TILE_IMPROVERS_LAST_DISBAND_WORKER_TURN_SCALE
	if (iLastTurnWorkerDisbanded >= 0 && GC.getGame().getGameTurn() - iLastTurnWorkerDisbanded <= 10 * GC.getGame().getEstimateEndTurn() / 500)
#else
	if(iLastTurnWorkerDisbanded >= 0 && GC.getGame().getGameTurn() - iLastTurnWorkerDisbanded <= 10)
#endif
	{
		return true;
	}

	AICityStrategyTypes eNeedImproversStrategy = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_NEED_TILE_IMPROVERS");

#ifdef AUI_WARNING_FIXES
	if (eNeedImproversStrategy != NO_AICITYSTRATEGY)
#else
	if(eNeedImproversStrategy != NO_ECONOMICAISTRATEGY)
#endif
	{
		if(pCity->GetCityStrategyAI()->IsUsingCityStrategy(eNeedImproversStrategy))
			return false;
	}

	int iNumBuilders = kPlayer.GetNumUnitsWithUnitAI(UNITAI_WORKER, true, false);


	if(GET_PLAYER(pCity->getOwner()).isMinorCiv())
	{
		if(iNumBuilders >= kPlayer.getNumCities())
			return true;
	}

	CvAICityStrategyEntry* pCityStrategy = pCity->GetCityStrategyAI()->GetAICityStrategies()->GetEntry(eStrategy);

	int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pCity);
	int iPerCityThreshold = pCityStrategy->GetWeightThreshold() + iWeightThresholdModifier;

	int iModdedNumCities = kPlayer.getNumCities() + kPlayer.countCitiesFeatureSurrounded();
	int iWeightThreshold = (iPerCityThreshold * iModdedNumCities);


	if((iNumBuilders * 100) >= iWeightThreshold)
	{
		return true;
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_NeedNavalGrowth(AICityStrategyTypes eStrategy, CvCity* pCity)
{
	int iNumOceanPlots = 0;
	int iNumTotalWorkablePlots = 0;

	CvPlot* pLoopPlot;


	for(int iPlotLoop = 0; iPlotLoop < NUM_CITY_PLOTS; iPlotLoop++)
	{
		pLoopPlot = plotCity(pCity->getX(), pCity->getY(), iPlotLoop);

		if(pLoopPlot != NULL)
		{
			if(pLoopPlot->getOwner() == pCity->getOwner())
			{
				if(!pLoopPlot->isCity())
				{
					iNumTotalWorkablePlots++;

					if(pLoopPlot->isWater() && !pLoopPlot->isLake())
					{
						iNumOceanPlots++;;
					}
				}
			}
		}
	}

	if(iNumTotalWorkablePlots > 0)
	{
		CvAICityStrategyEntry* pCityStrategy = pCity->GetCityStrategyAI()->GetAICityStrategies()->GetEntry(eStrategy);
		int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pCity);
		int iWeightThreshold = pCityStrategy->GetWeightThreshold() + iWeightThresholdModifier;



		if((iNumOceanPlots * 100) / iNumTotalWorkablePlots >= iWeightThreshold)
		{
			return true;
		}
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_NeedNavalTileImprovement(CvCity* pCity)
{
	int iNumUnimprovedWaterResources = 0;

	CvPlot* pLoopPlot;


	for(int iPlotLoop = 0; iPlotLoop < NUM_CITY_PLOTS; iPlotLoop++)
	{
		pLoopPlot = plotCity(pCity->getX(), pCity->getY(), iPlotLoop);

		if(pLoopPlot != NULL)
		{
			if(pLoopPlot->getOwner() == pCity->getOwner())
			{
				if(pLoopPlot->isWater())
				{

					if(pCity->GetCityCitizens()->IsCanWork(pLoopPlot))
					{

						if(pLoopPlot->getResourceType() != NO_RESOURCE && pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
						{
							iNumUnimprovedWaterResources++;
						}
					}
				}
			}
		}
	}

	int iNumWaterTileImprovers = GET_PLAYER(pCity->getOwner()).GetNumUnitsWithUnitAI(UNITAI_WORKER_SEA, true, true);


	if(iNumUnimprovedWaterResources > iNumWaterTileImprovers)
	{
#ifdef AI_WORKER_EMBARKED_FIX
		if ( iNumWaterTileImprovers >= 1 )
		{
			return false;
		}
		else {
		return true;
		}
#else
		return true;
#endif
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_EnoughNavalTileImprovement(CvCity* pCity)
{
	AICityStrategyTypes eStrategyNeedNavalTileImprovement = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_NEED_NAVAL_TILE_IMPROVEMENT");

#ifdef AUI_WARNING_FIXES
	if (eStrategyNeedNavalTileImprovement != NO_AICITYSTRATEGY)
#else
	if(eStrategyNeedNavalTileImprovement != NO_ECONOMICAISTRATEGY)
#endif
	{
		if(!pCity->GetCityStrategyAI()->IsUsingCityStrategy(eStrategyNeedNavalTileImprovement))
		{
			return true;
		}
	}
#ifdef AI_WORKER_EMBARKED_FIX
	int iX = pCity->getX(); int iY = pCity->getY(); CvPlayerAI& iOwner = GET_PLAYER(pCity->getOwner());

	int iNumWorkersHere = 0;
	int iCanImprove = 0;
	for (int iCityPlotLoop = 0; iCityPlotLoop < RING5_PLOTS; iCityPlotLoop++)
	{
		CvPlot* pLoopPlot = plotCity(iX, iY, iCityPlotLoop);


		if (pLoopPlot == NULL || pLoopPlot->getOwner() != pCity->getOwner())
		{
			continue;
		}

		if(pLoopPlot->getImprovementType() == NO_IMPROVEMENT && !pLoopPlot->isImpassable() && pLoopPlot->isWater())
		{
			CvUnit* pLoopUnit;
			for(int iUnitLoop = 0; iUnitLoop < pLoopPlot->getNumUnits(); iUnitLoop++)
			{

				pLoopUnit = pLoopPlot->getUnitByIndex(iUnitLoop);
				if(pLoopUnit != NULL && pLoopUnit->getOwner() == pCity->GetID() && pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER_SEA)
				{
					iNumWorkersHere++;
				}
			}
			for(int iI = 0; iI < GC.getNumBuildInfos(); ++iI)
			{
				CvBuildInfo* pkBuildInfo = GC.getBuildInfo((BuildTypes) iI);
				if(!pkBuildInfo)
				{
					continue;
				}
				ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo((BuildTypes) iI)->getImprovement();
				if(eImprovement != NO_IMPROVEMENT)
				{
					CvImprovementEntry* pkEntry = GC.getImprovementInfo(eImprovement);
					if(pkEntry->IsCreatedByGreatPerson())
						continue;
				}


				if(GET_PLAYER(pCity->getOwner()).canBuild(pLoopPlot, (BuildTypes)iI))
				{
					iCanImprove++;
					break;
				}
			}
		}
	}

	if(iCanImprove <= 0)
	{
		return true;
	}

	if(iNumWorkersHere > iCanImprove)
	{
		return true;
	}
#endif

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_NeedImprovement(CvCity* pCity, YieldTypes yield)
{
	if(pCity->GetCityStrategyAI()->GetDeficientYield() == yield)
	{
		return true;
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_HaveTrainingFacility(CvCity* pCity)
{
	if(pCity->getDomainFreeExperience(DOMAIN_LAND) > 0)
	{
		return true;
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_CapitalNeedSettler(AICityStrategyTypes eStrategy, CvCity* pCity)
{
	if(pCity->isCapital())
	{
		CvPlayer& kPlayer = GET_PLAYER(pCity->getOwner());

		if(!kPlayer.isMinorCiv())
		{
			int iNumCities = kPlayer.getNumCities();
			int iSettlersOnMapOrBuild = kPlayer.GetNumUnitsWithUnitAI(UNITAI_SETTLE, true);
			int iCitiesPlusSettlers = iNumCities + iSettlersOnMapOrBuild;

			if((iCitiesPlusSettlers) < 3)
			{

				AICityStrategyTypes eUnderThreat = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_CAPITAL_UNDER_THREAT");
				if(eUnderThreat != NO_AICITYSTRATEGY)
				{
					if(GC.getGame().getGameTurn() > 50 && pCity->GetCityStrategyAI()->IsUsingCityStrategy(eUnderThreat))
					{
						return false;
					}
				}

				MilitaryAIStrategyTypes eMilStrategy = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_WAR_MOBILIZATION");
				if(eMilStrategy != NO_MILITARYAISTRATEGY && kPlayer.GetMilitaryAI()->IsUsingStrategy(eMilStrategy))
				{

					return false;
				}

				CvAICityStrategyEntry* pCityStrategy = pCity->GetCityStrategyAI()->GetAICityStrategies()->GetEntry(eStrategy);
				int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pCity);
				int iWeightThreshold = pCityStrategy->GetWeightThreshold() + iWeightThresholdModifier;

				int iGameTurn = GC.getGame().getGameTurn();
				if((iCitiesPlusSettlers == 1 && (iGameTurn * 4) > iWeightThreshold) ||
					(iCitiesPlusSettlers == 2 && (iGameTurn * 2) > iWeightThreshold) || 
					(iCitiesPlusSettlers == 3 && iGameTurn > iWeightThreshold) )
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_CapitalUnderThreat(CvCity* pCity)
{
	if (pCity->isCapital())
	{
		CvPlayer &kPlayer = GET_PLAYER(pCity->getOwner());

		bool bAtPeace = GET_TEAM(kPlayer.getTeam()).getAtWarCount(false) == 0;

		if (!bAtPeace && !kPlayer.isMinorCiv())
		{
			CvCity *pMostThreatened = kPlayer.GetMilitaryAI()->GetMostThreatenedCity();
			if (pMostThreatened == pCity && pMostThreatened->getThreatValue() > 200)
			{
				return true;
			}
		}
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_FirstCultureBuilding(CvCity* pCity)
{
	if(pCity->GetJONSCulturePerTurnFromBuildings() <= 0)
	{
		return true;
	}

	if(pCity->isCapital() && pCity->GetJONSCulturePerTurnFromBuildings() <= 1)
	{
		return true;
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_FirstCultureBuildingEmergency(CvCity* pCity)
{
	if(pCity->getPopulation() >= 5 && pCity->GetJONSCulturePerTurnFromBuildings() <= 0)
	{
		return true;
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_FirstScienceBuilding(CvCity* pCity)
{
	if(pCity->getPopulation() >= 4 && pCity->GetYieldPerPopTimes100(YIELD_SCIENCE) == 0)
	{
		return true;
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_FirstGoldBuilding(CvCity* pCity)
{
	if(pCity->getPopulation() >= 4 && pCity->getYieldRateModifier(YIELD_GOLD) == 0)
	{
		return true;
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_FirstProductionBuilding(CvCity* pCity)
{
	if(pCity->getPopulation() >= 4 && pCity->GetBaseYieldRateFromBuildings(YIELD_PRODUCTION) == 0 && pCity->getYieldRateModifier(YIELD_PRODUCTION) == 0)
	{
		return true;
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_FirstFaithBuilding(CvCity* pCity)
{
	CvPlayer& kPlayer = GET_PLAYER(pCity->getOwner());
	FlavorTypes eFlavor = (FlavorTypes)GC.getInfoTypeForString("FLAVOR_RELIGION");

	int iReligionFlavor = kPlayer.GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor);

	if(pCity->GetFaithPerTurnFromBuildings() > 0)
	{
		return false;
	}


	if(pCity->getPopulation() < 2)
	{
		return false;
	}


	if((iReligionFlavor > 4 && kPlayer.GetReligions()->HasCreatedPantheon()) || iReligionFlavor > 6)
	{
		return true;
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_UnderBlockade(CvCity* pCity)
{
	if(pCity->GetCityCitizens()->IsAnyPlotBlockaded()                                                    )
	{
		return true;
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_IsPuppet(CvCity* pCity)
{
	if(pCity->IsPuppet())
	{
		return true;
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_MediumCityHighDifficulty(CvCity* pCity)
{
	if(pCity->getPopulation() >= 5 && GC.getGame().getHandicapInfo().GetID() > 4)
	{
		return true;
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_OriginalCapital(CvCity* pCity)
{
	if(pCity->IsOriginalCapital())
	{
		return true;
	}

	return false;
}



bool CityStrategyAIHelpers::IsTestCityStrategy_RiverCity(CvCity* pCity)
{
	if(pCity->plot()->isRiver())
	{
		return true;
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_HillCity(CvCity* pCity)
{

	const int iRange = 2;
	int iNumHills = 0;
	CvPlot* pPlot = pCity->plot();

#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX, iDX;
	CvPlot* pLoopPlot;
	for (int iDY = -iRange; iDY <= iRange; iDY++)
	{
		iMaxDX = iRange - MAX(0, iDY);
		for (iDX = -iRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
		{

			pLoopPlot = plotXY(pPlot->getX(), pPlot->getY(), iDX, iDY);
#else
	for(int iDX = -iRange; iDX <= iRange; iDX++)
	{
		for(int iDY = -iRange; iDY <= iRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iDX, iDY, iRange);
#endif
			if(pLoopPlot)
			{
				if(pLoopPlot->isHills() && pLoopPlot->getOwner() == pPlot->getOwner())
				{
					iNumHills++;
					if(iNumHills > 1)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_MountainCity(CvCity* pCity)
{

	CvPlot* pPlot = pCity->plot();
	for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; iDirectionLoop++)
	{
		CvPlot* pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iDirectionLoop));
		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->isMountain())
			{
				return true;
			}
		}
	}


	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_ForestCity(CvCity* pCity)
{

	const int iRange = 2;
	int iNumForests = 0;
	CvPlot* pPlot = pCity->plot();

#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX, iDX;
	CvPlot* pLoopPlot;
	for (int iDY = -iRange; iDY <= iRange; iDY++)
	{
		iMaxDX = iRange - MAX(0, iDY);
		for (iDX = -iRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
		{

			pLoopPlot = plotXY(pPlot->getX(), pPlot->getY(), iDX, iDY);
#else
	for(int iDX = -iRange; iDX <= iRange; iDX++)
	{
		for(int iDY = -iRange; iDY <= iRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iDX, iDY, iRange);
#endif
			if(pLoopPlot)
			{

				if(pLoopPlot->getFeatureType() == FEATURE_FOREST && pLoopPlot->getOwner() == pPlot->getOwner())
				{
					iNumForests++;
					if(iNumForests > 1)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}


bool CityStrategyAIHelpers::IsTestCityStrategy_JungleCity(CvCity* pCity)
{

	const int iRange = 2;
	int iNumJungles = 0;
	CvPlot* pPlot = pCity->plot();

#ifdef AUI_HEXSPACE_DX_LOOPS
	int iMaxDX, iDX;
	CvPlot* pLoopPlot;
	for (int iDY = -iRange; iDY <= iRange; iDY++)
	{
		iMaxDX = iRange - MAX(0, iDY);
		for (iDX = -iRange - MIN(0, iDY); iDX <= iMaxDX; iDX++)
		{

			pLoopPlot = plotXY(pPlot->getX(), pPlot->getY(), iDX, iDY);
#else
	for(int iDX = -iRange; iDX <= iRange; iDX++)
	{
		for(int iDY = -iRange; iDY <= iRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iDX, iDY, iRange);
#endif
			if(pLoopPlot)
			{

				if(pLoopPlot->getFeatureType() == FEATURE_JUNGLE && pLoopPlot->getOwner() == pPlot->getOwner())
				{
					iNumJungles++;
					if(iNumJungles > 1)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}



bool CityStrategyAIHelpers::IsTestCityStrategy_CoastCity(CvCity* pCity)
{
	if(pCity->plot()->isCoastalLand())
	{
		return true;
	}

	return false;
}

bool CityStrategyAIHelpers::IsTestCityStrategy_ManyTechsStolen(CvCity* pCity)
{
	PlayerTypes ePlayer = pCity->getOwner();
	CvEspionageAI* pEspionageAI = GET_PLAYER(ePlayer).GetEspionageAI();
	CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
	float fRatio = 0.0;
	int iTurnsOfEspionage = GC.getGame().getGameTurn() - pEspionageAI->m_iTurnEspionageStarted;
	if (pEspionageAI->m_iTurnEspionageStarted != 0)
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
		
		fRatio = pCityEspionage->m_aiNumTimesCityRobbed[ePlayer] / (float)(iTurnsOfEspionage);
	}

	if (fRatio > 0.0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CityStrategyAIHelpers::IsTestCityStrategy_KeyScienceCity(CvCity* pCity)
{
	if (GET_PLAYER(pCity->getOwner()).GetEspionageAI()->m_iTurnEspionageStarted == -1)
	{
		return false;
	}

	PlayerTypes ePlayer = pCity->getOwner();
	CvCity* pLoopCity = NULL;
	int iLoop = 0;
	int iNumBetterScienceCities = 0;
	int iNumOtherCities = 0;
	int iCityScienceOutput = pCity->getYieldRateTimes100(YIELD_SCIENCE, false);

	for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		iNumOtherCities++;

		if (pLoopCity == pCity)
		{
			continue;
		}

		if (pLoopCity->getYieldRateTimes100(YIELD_SCIENCE, false) > iCityScienceOutput)
		{
			iNumBetterScienceCities++;
		}
	}

	if (iNumOtherCities > 0)
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

		float fRatio = iNumBetterScienceCities / (float)iNumOtherCities;
		float fCutOff = (0.05f * GET_PLAYER(ePlayer).GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavorEspionage));

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


bool CityStrategyAIHelpers::IsTestCityStrategy_GoodGPCity(CvCity* pCity)
{


	int iTotalGPPChange = 0;

#ifdef AUI_WARNING_FIXES
	for (uint iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
#else
	for (int iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
#endif
	{
		const SpecialistTypes eSpecialist = static_cast<SpecialistTypes>(iSpecialistLoop);
		CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
		if(pkSpecialistInfo)
		{

			if (pkSpecialistInfo->getGreatPeopleUnitClass() != NO_UNITCLASS)
			{
				int iCount = pCity->GetCityCitizens()->GetSpecialistCount(eSpecialist);


				int iGPPChange = pkSpecialistInfo->getGreatPeopleRateChange() * iCount * 100;


				iGPPChange += pCity->GetCityCitizens()->GetBuildingGreatPeopleRateChanges(eSpecialist) * 100;

				if (iGPPChange > 0)
				{
					int iMod = 0;


					iMod += pCity->getGreatPeopleRateModifier();


					iMod += pCity->GetPlayer()->getGreatPeopleRateModifier();


					if ((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_SCIENTIST"))
					{
						iMod += pCity->GetPlayer()->GetPlayerTraits()->GetGreatScientistRateModifier();
						iMod += pCity->GetPlayer()->getGreatScientistRateModifier();
					}
					else if((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_WRITER"))
					{
						iMod += pCity->GetPlayer()->getGreatWriterRateModifier();
					}					
					else if((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_ARTIST"))
					{
						iMod += pCity->GetPlayer()->getGreatArtistRateModifier();
					}					
					else if((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_MUSICIAN"))
					{
						iMod += pCity->GetPlayer()->getGreatMusicianRateModifier();
#ifdef NQ_PRODUCTION_TO_GREAT_MUSICIANS_MODIFIER_FROM_POLICIES
						int iProductionToGreatMusiciansModifier = pCity->GetPlayer()->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_PRODUCTION_TO_GREAT_MUSICIANS_MODIFIER);
						if (iProductionToGreatMusiciansModifier > 0)
						{
							iMod += pCity->GetPlayer()->calculateTotalYield(YIELD_PRODUCTION) * iProductionToGreatMusiciansModifier / 100;
						}
#endif
					}
					else if((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_MERCHANT"))
					{
#if defined(TRAITIFY)
						iMod += pCity->GetPlayer()->GetPlayerTraits()->GetGreatMerchantRateModifier();
#endif
						iMod += pCity->GetPlayer()->getGreatMerchantRateModifier();
					}
					else if((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_ENGINEER"))
					{
#if defined(TRAITIFY)
						iMod += pCity->GetPlayer()->GetPlayerTraits()->GetGreatEngineerRateModifier();
#endif
						iMod += pCity->GetPlayer()->getGreatEngineerRateModifier();
					}

					iGPPChange *= (100 + iMod);
					iGPPChange /= 100;

					iTotalGPPChange += iGPPChange;
				}
			}
		}
	}

	if (iTotalGPPChange >= 800)
	{
		return true;
	}

	return false;
}

bool CityStrategyAIHelpers::IsTestCityStrategy_NeedInternationalTradeRoute (CvCity* pCity, DomainTypes eDomain)
{
	PlayerTypes ePlayer = pCity->getOwner();
	CvPlayerTrade* pTrade = GET_PLAYER(ePlayer).GetTrade();

	if (pTrade->GetNumTradeRoutesRemaining(false) <= 0)
	{
		return false;
	}

	if (pTrade->GetNumPotentialConnections(pCity, eDomain) <= 0)
	{
		return false;
	}

	return true;
}

bool CityStrategyAIHelpers::IsTestCityStrategy_NoNeedInternationalTradeRoute (CvCity* pCity, DomainTypes eDomain)
{
	PlayerTypes ePlayer = pCity->getOwner();
	CvPlayerTrade* pTrade = GET_PLAYER(ePlayer).GetTrade();

	if (pTrade->GetNumTradeRoutesRemaining(false) <= 0)
	{
		return true;
	}

	if (pTrade->GetNumPotentialConnections(pCity, eDomain) <= 0)
	{
		return true;
	}

	return false;
}

bool CityStrategyAIHelpers::IsTestCityStrategy_IsInternationalTradeDestination(CvCity* pCity)
{
	int iNumTimesDestination = GC.getGame().GetGameTrade()->GetNumTimesDestinationCity(pCity, true);
	if (iNumTimesDestination >= 2)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CityStrategyAIHelpers::IsTestCityStrategy_IsInternationalTradeOrigin(CvCity* pCity)
{
	int iNumTimesOrigin = GC.getGame().GetGameTrade()->GetNumTimesOriginCity(pCity, true);
	if (iNumTimesOrigin >= 2)
	{
		return true;
	}
	else
	{
		return false;
	}	
}

bool CityStrategyAIHelpers::IsTestCityStrategy_NeedCultureBuilding(CvCity *pCity)
{
	CvPlayer &kPlayer = GET_PLAYER(pCity->getOwner());

	GreatWorkSlotType eSlotType = pCity->GetCityCulture()->GetSlotTypeFirstAvailableCultureBuilding();

	if (eSlotType != NO_GREAT_WORK_SLOT)
	{
		int iSlotsOpen = kPlayer.GetCulture()->GetNumGreatWorkSlots(eSlotType);

		if (iSlotsOpen <= 2)
		{
			return true;
		}
	}

	return false;
}

bool CityStrategyAIHelpers::IsTestCityStrategy_NeedTourismBuilding(CvCity *pCity)
{
	int iTourismValue = 0;
#ifdef AUI_WARNING_FIXES
	const CvCityCulture* pCityCulture = pCity->GetCityCulture();
	iTourismValue += pCityCulture->GetCultureFromWonders();
	iTourismValue += pCityCulture->GetCultureFromNaturalWonders();
	iTourismValue += pCityCulture->GetCultureFromImprovements();
	iTourismValue += pCityCulture->GetBaseTourism();
#else
	iTourismValue += pCity->GetCityCulture()->GetCultureFromWonders();
	iTourismValue += pCity->GetCityCulture()->GetCultureFromNaturalWonders();
	iTourismValue += pCity->GetCityCulture()->GetCultureFromImprovements();
	iTourismValue += pCity->GetCityCulture()->GetBaseTourism();
#endif

	if (iTourismValue > 10)
	{
		return true;
	}

	return false;
}

bool CityStrategyAIHelpers::IsTestCityStrategy_GoodAirliftCity(CvCity *pCity)
{
#ifdef AUI_WARNING_FIXES
	if (!pCity)
		return false;
#endif
	if (pCity->isCapital())
	{
		return true;
	}

	CvPlayer &kPlayer = GET_PLAYER(pCity->getOwner());
	CvCity *pCapital = kPlayer.getCapitalCity();
#ifdef AUI_WARNING_FIXES
	if (!pCapital)
		return false;
	if (pCity->getArea() != pCapital->getArea())
#else
	if (pCity && pCity->getArea() != pCapital->getArea())
#endif
	{
		return true;
	}

	if (plotDistance (pCity->getX(), pCity->getY(), pCapital->getX(), pCapital->getY()) > 20)
	{
		return true;
	}

	return false;
}
