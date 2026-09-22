







#include "CvGameCoreDLLPCH.h"
#include "CvCitySpecializationAI.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvGrandStrategyAI.h"
#include "CvEconomicAI.h"
#include "CvMilitaryAI.h"
#include "CvEnumSerialization.h"
#include "CvWonderProductionAI.h"
#include "cvStopWatch.h"


#include "LintFree.h"






CvCitySpecializationXMLEntry::CvCitySpecializationXMLEntry(void):
	m_piFlavorValue(NULL),
	m_piYieldTargetTimes100(NULL),
	m_eYieldType(NO_YIELD),
	m_iSubtype(0),
	m_bWonder(false),
	m_bDefault(false),
	m_bMustBeCoastal(false),
	m_bOperationUnitProvider(false)
{
}

CvCitySpecializationXMLEntry::~CvCitySpecializationXMLEntry(void)
{
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piYieldTargetTimes100);
}

bool CvCitySpecializationXMLEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;


	m_iSubtype = kResults.GetInt("SubType");


	const char* szYieldType = kResults.GetText("YieldType");
	if(szYieldType != NULL)
	{
		m_eYieldType = (YieldTypes)GC.getInfoTypeForString(szYieldType);
	}


	const char* szType = GetType();
	kUtility.SetFlavors(m_piFlavorValue, "CitySpecialization_Flavors", "CitySpecializationType", szType);
	kUtility.SetYields(m_piYieldTargetTimes100, "CitySpecialization_TargetYields", "CitySpecializationType", szType);


	m_bWonder = kResults.GetBool("IsWonder");
	m_bDefault = kResults.GetBool("IsDefault");
	m_bMustBeCoastal = kResults.GetBool("MustBeCoastal");
	m_bOperationUnitProvider = kResults.GetBool("IsOperationUnitProvider");

	return true;
}


int CvCitySpecializationXMLEntry::GetFlavorValue(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : -1;
}


YieldTypes CvCitySpecializationXMLEntry::GetYieldType() const
{
	return m_eYieldType;
}

int CvCitySpecializationXMLEntry::GetYieldTargetTimes100(YieldTypes eYield) const
{
	return m_piYieldTargetTimes100[eYield];
}



int CvCitySpecializationXMLEntry::GetSubtype() const
{
	return m_iSubtype;
}


bool CvCitySpecializationXMLEntry::IsWonder() const
{
	return m_bWonder;
}


bool CvCitySpecializationXMLEntry::IsDefault() const
{
	return m_bDefault;
}


bool CvCitySpecializationXMLEntry::IsMustBeCoastal() const
{
	return m_bMustBeCoastal;
}


bool CvCitySpecializationXMLEntry::IsOperationUnitProvider() const
{
	return m_bOperationUnitProvider;
}





CvCitySpecializationXMLEntries::CvCitySpecializationXMLEntries(void)
{
#ifdef AUI_WARNING_FIXES
	m_CurrentIndex = 0;
	m_CurrentYield = NO_YIELD;
#endif
}


CvCitySpecializationXMLEntries::~CvCitySpecializationXMLEntries(void)
{
	DeleteArray();
}


std::vector<CvCitySpecializationXMLEntry*>& CvCitySpecializationXMLEntries::GetCitySpecializationEntries()
{
	return m_paCitySpecializationEntries;
}


int CvCitySpecializationXMLEntries::GetNumCitySpecializations()
{
	return m_paCitySpecializationEntries.size();
}


CvCitySpecializationXMLEntry* CvCitySpecializationXMLEntries::GetEntry(int index)
{
	return m_paCitySpecializationEntries[index];
}


CitySpecializationTypes CvCitySpecializationXMLEntries::GetFirstSpecializationForYield(YieldTypes eYield)
{
	m_CurrentYield = eYield;
	for(m_CurrentIndex = 0; m_CurrentIndex < (int)m_paCitySpecializationEntries.size(); m_CurrentIndex++)
	{
		if(m_paCitySpecializationEntries[m_CurrentIndex]->GetYieldType() == m_CurrentYield)
		{
			return (CitySpecializationTypes)m_CurrentIndex;
		}
	}
	return NO_CITY_SPECIALIZATION;
}


CitySpecializationTypes CvCitySpecializationXMLEntries::GetNextSpecializationForYield()
{
#ifdef AUI_WARNING_FIXES
	for (m_CurrentIndex = m_CurrentIndex + 1; m_CurrentIndex < (int)m_paCitySpecializationEntries.size(); m_CurrentIndex++)
#else
	for(m_CurrentIndex = m_CurrentIndex++; m_CurrentIndex < (int)m_paCitySpecializationEntries.size(); m_CurrentIndex++)
#endif
	{
		if(m_paCitySpecializationEntries[m_CurrentIndex]->GetYieldType() == m_CurrentYield)
		{
			return (CitySpecializationTypes)m_CurrentIndex;
		}
	}
	return NO_CITY_SPECIALIZATION;
}


int CvCitySpecializationXMLEntries::GetNumSpecializationsForYield(YieldTypes eYield)
{
	int iRtnValue = 0;
	for(int iI = 0; iI < (int)m_paCitySpecializationEntries.size(); iI++)
	{
		if(m_paCitySpecializationEntries[iI]->GetYieldType() == eYield)
		{
			iRtnValue++;
		}
	}

	return iRtnValue;
}


void CvCitySpecializationXMLEntries::DeleteArray()
{
	for(std::vector<CvCitySpecializationXMLEntry*>::iterator it = m_paCitySpecializationEntries.begin(); it != m_paCitySpecializationEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paCitySpecializationEntries.clear();
}






CvCitySpecializationAI::CvCitySpecializationAI():
	m_bSpecializationsDirty(false),
	m_bInterruptWonders(false),
	m_eNextSpecializationDesired(NO_CITY_SPECIALIZATION),
	m_eNextWonderDesired(NO_BUILDING),
	m_iWonderCityID(-1),
	m_iNextWonderWeight(0),
#ifdef AUI_WARNING_FIXES
	m_pPlayer(NULL),
	m_pSpecializations(NULL),
	m_bWonderChosen(false),
	m_iBestValue(),
	m_iNumSpecializationsForThisYield(),
	m_iNumSpecializationsForThisSubtype(),
#endif
	m_iLastTurnEvaluated(0)
{
}


CvCitySpecializationAI::~CvCitySpecializationAI(void)
{
	Uninit();
}


void CvCitySpecializationAI::Init(CvCitySpecializationXMLEntries* pSpecializations, CvPlayer* pPlayer)
{
	m_pSpecializations = pSpecializations;
	m_pPlayer = pPlayer;

	Reset();
}


void CvCitySpecializationAI::Uninit()
{
}


void CvCitySpecializationAI::Reset()
{
	m_bSpecializationsDirty = false;
	m_bInterruptWonders = false;
	m_eNextSpecializationDesired = NO_CITY_SPECIALIZATION;
	m_eNextWonderDesired = NO_BUILDING;
	m_iWonderCityID = -1;
	m_iNextWonderWeight = 0;
	m_YieldWeights.clear();
	m_ProductionSubtypeWeights.clear();
	m_iLastTurnEvaluated = 0;
}


void CvCitySpecializationAI::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_bSpecializationsDirty;
	kStream >> m_bInterruptWonders;
	kStream >> m_eNextSpecializationDesired;
	kStream >> (int&)m_eNextWonderDesired;
	kStream >> m_iWonderCityID;
	kStream >> m_iNextWonderWeight;

	if (uiVersion >= 2)
	{
		kStream >> m_iLastTurnEvaluated;
	}
	else
	{
		m_iLastTurnEvaluated = 0;
		m_bSpecializationsDirty = true;
	}
}


void CvCitySpecializationAI::Write(FDataStream& kStream) const
{

	uint uiVersion = 2;
	kStream << uiVersion;

	kStream << m_bSpecializationsDirty;
	kStream << m_bInterruptWonders;
	kStream << m_eNextSpecializationDesired;
	kStream << m_eNextWonderDesired;
	kStream << m_iWonderCityID;
	kStream << m_iNextWonderWeight;
	kStream << m_iLastTurnEvaluated;
}


CvPlayer* CvCitySpecializationAI::GetPlayer()
{
	return m_pPlayer;
}


CvCitySpecializationXMLEntries* CvCitySpecializationAI::GetCitySpecializations()
{
	return m_pSpecializations;
}


void CvCitySpecializationAI::DoTurn()
{
	AI_PERF_FORMAT("AI-perf.csv", ("CvCitySpecializationAI::DoTurn, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), GetPlayer()->getCivilizationShortDescription()) );

	int iCityLoop = 0;


	if(m_pPlayer->isHuman())
	{
		return;
	}


	if(m_pPlayer->isMinorCiv())
	{
		return;
	}


	if(GC.getGame().getGameTurn() < GC.getAI_CITY_SPECIALIZATION_EARLIEST_TURN())
	{
		return;
	}


	if(m_pPlayer->getNumCities() < 2)
	{
		return;
	}


	if(m_bSpecializationsDirty || (m_iLastTurnEvaluated + GC.getAI_CITY_SPECIALIZATION_REEVALUATION_INTERVAL() > GC.getGame().getGameTurn()))
	{
		m_eNextWonderDesired = m_pPlayer->GetWonderProductionAI()->ChooseWonder(false                    , true                           , m_iNextWonderWeight);
		WeightSpecializations();
		AssignSpecializations();
		m_bSpecializationsDirty = false;
		m_iLastTurnEvaluated = GC.getGame().getGameTurn();


		if(m_bInterruptWonders)
		{
			CvCity* pLoopCity = NULL;
			for(pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
			{
				if(!pLoopCity->IsBuildingUnitForOperation())
				{
					pLoopCity->AI_chooseProduction(true                      );
				}
			}
		}


		m_bInterruptWonders = false;
	}
}


void CvCitySpecializationAI::SetSpecializationsDirty(CitySpecializationUpdateType eUpdateType)
{

	if(!m_pPlayer->isMinorCiv())
	{
		m_bSpecializationsDirty = true;
		LogSpecializationUpdate(eUpdateType);

		switch(eUpdateType)
		{
		case SPECIALIZATION_UPDATE_NOW_AT_WAR:
		case SPECIALIZATION_UPDATE_MY_CITY_CAPTURED:
			m_bInterruptWonders = true;
			break;
		default:


			break;
		}
	}

	return;
}


CvCity* CvCitySpecializationAI::GetWonderBuildCity() const
{
	CvCity* pRtnValue = NULL;
	if(m_iWonderCityID != -1)
	{
		pRtnValue = m_pPlayer->getCity(m_iWonderCityID);
	}
	return pRtnValue;
}




void CvCitySpecializationAI::WeightSpecializations()
{
	int iFoodYieldWeight = 0;
	int iProductionYieldWeight = 0;
	int iGoldYieldWeight = 0;
	int iScienceYieldWeight = 0;
	int iGeneralEconomicWeight = 0;


	m_YieldWeights.clear();
	m_ProductionSubtypeWeights.clear();


	if(m_pPlayer->getCapitalCity() != NULL)
	{
		int iFlavorExpansion = 0;
		int iFlavorWonder = 0;
		int iFlavorGold = 0;
		int iFlavorScience = 0;
		int iFlavorSpaceship = 0;

		iFlavorExpansion = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_EXPANSION"));
		if(iFlavorExpansion < 0) iFlavorExpansion = 0;
		iFlavorWonder = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_WONDER"));
		if(iFlavorWonder < 0) iFlavorWonder = 0;
		iFlavorGold = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GOLD"));
		if(iFlavorGold < 0) iFlavorGold = 0;
		iFlavorScience = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SCIENCE"));
		if(iFlavorScience < 0) iFlavorScience = 0;
		iFlavorSpaceship = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SPACESHIP"));
		if(iFlavorSpaceship < 0) iFlavorSpaceship = 0;




		CvArea* pArea = GC.getMap().getArea(m_pPlayer->getCapitalCity()->getArea());
		int iNumUnownedTiles = pArea->getNumUnownedTiles();
		int iNumCities = m_pPlayer->getNumCities();
		int iNumSettlers = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_SETTLE, true);
		EconomicAIStrategyTypes eStrategy = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EARLY_EXPANSION");
		if(eStrategy != NO_ECONOMICAISTRATEGY && m_pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategy))
		{
			iFoodYieldWeight += GC.getAI_CITY_SPECIALIZATION_FOOD_WEIGHT_EARLY_EXPANSION()          ;
		}
		iFoodYieldWeight += iFlavorExpansion * GC.getAI_CITY_SPECIALIZATION_FOOD_WEIGHT_FLAVOR_EXPANSION()        ;
		iFoodYieldWeight += (iNumUnownedTiles * 100) / pArea->getNumTiles() * GC.getAI_CITY_SPECIALIZATION_FOOD_WEIGHT_PERCENT_CONTINENT_UNOWNED()        ;;
		iFoodYieldWeight += iNumCities * GC.getAI_CITY_SPECIALIZATION_FOOD_WEIGHT_NUM_CITIES()          ;
		iFoodYieldWeight += iNumSettlers * GC.getAI_CITY_SPECIALIZATION_FOOD_WEIGHT_NUM_SETTLERS()          ;
		if((iNumCities + iNumSettlers) == 1)
		{
			iFoodYieldWeight *= 3;
		}
		if(iFoodYieldWeight < 0) iFoodYieldWeight = 0;


		iProductionYieldWeight = WeightProductionSubtypes(iFlavorWonder, iFlavorSpaceship);


		int iLandDisputeLevel = m_pPlayer->GetDiplomacyAI()->GetTotalLandDisputeLevel();
		iGoldYieldWeight += iFlavorGold * GC.getAI_CITY_SPECIALIZATION_GOLD_WEIGHT_FLAVOR_GOLD()         ;
		iGoldYieldWeight += iLandDisputeLevel * GC.getAI_CITY_SPECIALIZATION_GOLD_WEIGHT_LAND_DISPUTE()         ;


		iScienceYieldWeight += iFlavorScience * GC.getAI_CITY_SPECIALIZATION_SCIENCE_WEIGHT_FLAVOR_SCIENCE()         ;
		iScienceYieldWeight += iFlavorSpaceship * GC.getAI_CITY_SPECIALIZATION_SCIENCE_WEIGHT_FLAVOR_SPACESHIP()         ;


		iGeneralEconomicWeight = GC.getAI_CITY_SPECIALIZATION_GENERAL_ECONOMIC_WEIGHT()          ;


#ifdef AUI_WARNING_FIXES
		for (uint iGrandStrategyLoop = 0; iGrandStrategyLoop < GC.getNumAIGrandStrategyInfos(); iGrandStrategyLoop++)
#else
		for(int iGrandStrategyLoop = 0; iGrandStrategyLoop < GC.getNumAIGrandStrategyInfos(); iGrandStrategyLoop++)
#endif
		{
			CvAIGrandStrategyXMLEntry* grandStrategy = GC.getAIGrandStrategyInfo((AIGrandStrategyTypes)iGrandStrategyLoop);
			if(grandStrategy)
			{
#ifdef AUI_WARNING_FIXES
				if(iGrandStrategyLoop == (uint)m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy())
#else
				if(iGrandStrategyLoop == m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy())
#endif
				{
					iFoodYieldWeight +=	grandStrategy->GetSpecializationBoost(YIELD_FOOD);
					iGoldYieldWeight += grandStrategy->GetSpecializationBoost(YIELD_GOLD);
					iScienceYieldWeight += grandStrategy->GetSpecializationBoost(YIELD_SCIENCE);
				}
			}
		}


		m_YieldWeights.push_back(YIELD_FOOD, iFoodYieldWeight);
		m_YieldWeights.push_back(YIELD_PRODUCTION, iProductionYieldWeight);
		m_YieldWeights.push_back(YIELD_GOLD, iGoldYieldWeight);
		m_YieldWeights.push_back(YIELD_SCIENCE, iScienceYieldWeight);
		m_YieldWeights.push_back(NO_YIELD, iGeneralEconomicWeight);


		LogSpecializationWeights();
	}

	return;
}


int CvCitySpecializationAI::WeightProductionSubtypes(int iFlavorWonder, int iFlavorSpaceship)
{
	bool bCriticalDefenseOn = false;

	int iMilitaryTrainingWeight = 0;
	int iEmergencyUnitWeight = 0;
	int iSeaWeight = 0;
	int iWonderWeight = 0;
	int iSpaceshipWeight = 0;

	int iFlavorOffense = 0;
	iFlavorOffense = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE"));

	int iUnitsRequested = m_pPlayer->GetNumUnitsNeededToBeBuilt();


	iMilitaryTrainingWeight += (iFlavorOffense * GC.getAI_CITY_SPECIALIZATION_PRODUCTION_TRAINING_PER_OFFENSE())         ;
	iMilitaryTrainingWeight += (m_pPlayer->GetDiplomacyAI()->GetPersonalityMajorCivApproachBias(MAJOR_CIV_APPROACH_WAR) * GC.getAI_CITY_SPECIALIZATION_PRODUCTION_TRAINING_PER_PERSONALITY()         );


	iEmergencyUnitWeight += iUnitsRequested * GC.getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_OPERATIONAL_UNITS_REQUESTED()         ;
	iEmergencyUnitWeight += m_pPlayer->GetMilitaryAI()->GetNumberCivsAtWarWith() * GC.getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_CIVS_AT_WAR_WITH()          ;


	AICityStrategyTypes eCityStrategy = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_CAPITAL_UNDER_THREAT");
	CvCity* pCapital;
	pCapital = m_pPlayer->getCapitalCity();
	if(pCapital && eCityStrategy != NO_AICITYSTRATEGY && pCapital->GetCityStrategyAI()->IsUsingCityStrategy(eCityStrategy))
	{
		iEmergencyUnitWeight += GC.getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_CAPITAL_THREAT()         ;
	}


	MilitaryAIStrategyTypes eStrategy = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_WAR_MOBILIZATION");
	if(eStrategy != NO_MILITARYAISTRATEGY && m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategy))
	{
		iMilitaryTrainingWeight += GC.getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_WAR_MOBILIZATION()          ;
	}
	eStrategy = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_EMPIRE_DEFENSE");
	if(eStrategy != NO_MILITARYAISTRATEGY && m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategy))
	{
		iEmergencyUnitWeight += GC.getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_EMPIRE_DEFENSE()          ;
	}
	eStrategy = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL");
	if(eStrategy != NO_MILITARYAISTRATEGY && m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategy))
	{
		bCriticalDefenseOn = true;
		iEmergencyUnitWeight += GC.getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_EMPIRE_DEFENSE_CRITICAL()           ;
	}


	eStrategy = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_ENOUGH_MILITARY_UNITS");
	if(eStrategy != NO_MILITARYAISTRATEGY && m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategy))
	{
		iMilitaryTrainingWeight = 0;
		iEmergencyUnitWeight = 0;
	}

	eStrategy = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_NAVAL_UNITS");
	if(eStrategy != NO_MILITARYAISTRATEGY && m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategy))
	{
		iSeaWeight += GC.getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEED_NAVAL_UNITS()         ;
	}
	eStrategy = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_NAVAL_UNITS_CRITICAL");
	if(eStrategy != NO_MILITARYAISTRATEGY && m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategy))
	{
		iSeaWeight += GC.getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEED_NAVAL_UNITS_CRITICAL()          ;
	}
	eStrategy = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_ENOUGH_NAVAL_UNITS");
	if(eStrategy != NO_MILITARYAISTRATEGY && m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategy))
	{
		iSeaWeight = 0;
	}


	int iWonderFlavorWeight = iFlavorWonder * GC.getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_FLAVOR_WONDER()          ;
	int iWeightOfWonders = (int)(m_iNextWonderWeight * GC.getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEXT_WONDER());
	iWonderWeight = min(iWonderFlavorWeight, iWeightOfWonders);
	iWonderWeight = max(iWonderWeight, 0);


	if(bCriticalDefenseOn)
	{
		iWonderWeight /= 2;
	}

	if(CanBuildSpaceshipParts())
	{
		iSpaceshipWeight += iFlavorSpaceship * GC.getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_FLAVOR_SPACESHIP()        ;
	}

#ifdef AUI_WARNING_FIXES
	for (uint iGrandStrategyLoop = 0; iGrandStrategyLoop < GC.getNumAIGrandStrategyInfos(); iGrandStrategyLoop++)
#else
	for(int iGrandStrategyLoop = 0; iGrandStrategyLoop < GC.getNumAIGrandStrategyInfos(); iGrandStrategyLoop++)
#endif
	{
		CvAIGrandStrategyXMLEntry* grandStrategy = GC.getAIGrandStrategyInfo((AIGrandStrategyTypes)iGrandStrategyLoop);
		if(grandStrategy)
		{
#ifdef AUI_WARNING_FIXES
			if (iGrandStrategyLoop == (uint)m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy())
#else
			if(iGrandStrategyLoop == m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy())
#endif
			{
				if(grandStrategy->GetSpecializationBoost(YIELD_PRODUCTION) > 0)
				{
					if(grandStrategy->GetFlavorValue((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE")) > 0)
					{
						iMilitaryTrainingWeight += grandStrategy->GetSpecializationBoost(YIELD_PRODUCTION);
					}
					else if(grandStrategy->GetFlavorValue((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SPACESHIP")) > 0)
					{
						iSpaceshipWeight += grandStrategy->GetSpecializationBoost(YIELD_PRODUCTION);
					}
				}
			}
		}
	}


	m_ProductionSubtypeWeights.push_back(PRODUCTION_SPECIALIZATION_MILITARY_TRAINING, iMilitaryTrainingWeight);
	m_ProductionSubtypeWeights.push_back(PRODUCTION_SPECIALIZATION_EMERGENCY_UNITS, iEmergencyUnitWeight);
	m_ProductionSubtypeWeights.push_back(PRODUCTION_SPECIALIZATION_MILITARY_NAVAL, iSeaWeight);
	m_ProductionSubtypeWeights.push_back(PRODUCTION_SPECIALIZATION_WONDER, iWonderWeight);
	m_ProductionSubtypeWeights.push_back(PRODUCTION_SPECIALIZATION_SPACESHIP, iSpaceshipWeight);

	return iMilitaryTrainingWeight + iEmergencyUnitWeight + iSeaWeight + iWonderWeight + iSpaceshipWeight;
}


void CvCitySpecializationAI::AssignSpecializations()
{
	int iI;
	CitySpecializationTypes eSpecialization;
	CitySpecializationData cityData;
	list<CitySpecializationData> citiesWithoutSpecialization;
	list<CitySpecializationTypes>::iterator it;
	list<CitySpecializationTypes>::iterator iterEnd;
	list<CitySpecializationData>::iterator cityIter;
	list<CitySpecializationData>::iterator cityIterEnd;
	list<CitySpecializationData>::iterator bestCity;

	m_eNextSpecializationDesired = NO_CITY_SPECIALIZATION;
	citiesWithoutSpecialization.clear();

	CitySpecializationTypes eWonderSpecialiation = GetWonderSpecialization();


	SelectSpecializations();


	CvCity* pLoopCity;
	int iLoop;
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity->IsPuppet())
		{
			pLoopCity->GetCityStrategyAI()->SetSpecialization(GetEconomicDefaultSpecialization());
			LogSpecializationAssignment(pLoopCity, GetEconomicDefaultSpecialization(), true);
			continue;
		}


		if(m_bWonderChosen && pLoopCity->GetID() == m_iWonderCityID)
		{
			it = find(m_SpecializationsNeeded.begin(), m_SpecializationsNeeded.end(), eWonderSpecialiation);
			if(it != m_SpecializationsNeeded.end())
			{
				m_SpecializationsNeeded.erase(it);
				pLoopCity->GetCityStrategyAI()->SetSpecialization(eWonderSpecialiation);
				LogSpecializationAssignment(pLoopCity, eWonderSpecialiation, true);
				continue;
			}
		}


		eSpecialization = pLoopCity->GetCityStrategyAI()->GetDefaultSpecialization();
		it = find(m_SpecializationsNeeded.begin(), m_SpecializationsNeeded.end(), eSpecialization);
		if(it != m_SpecializationsNeeded.end())
		{
			m_SpecializationsNeeded.erase(it);
			pLoopCity->GetCityStrategyAI()->SetSpecialization(eSpecialization);
			LogSpecializationAssignment(pLoopCity, eSpecialization);
		}
		else
		{

			eSpecialization = pLoopCity->GetCityStrategyAI()->GetSpecialization();
			it = find(m_SpecializationsNeeded.begin(), m_SpecializationsNeeded.end(), eSpecialization);
			if(it != m_SpecializationsNeeded.end())
			{
				m_SpecializationsNeeded.erase(it);
				pLoopCity->GetCityStrategyAI()->SetSpecialization(eSpecialization);
				LogSpecializationAssignment(pLoopCity, eSpecialization);
			}


			else
			{
				cityData.m_eID = pLoopCity->GetID();
				for(iI = 0; iI <= YIELD_SCIENCE; iI++)
				{
					if(iI == YIELD_SCIENCE)
					{
						cityData.m_iWeight[iI] = PlotValueForScience(pLoopCity->plot());
					}
					else
					{
						cityData.m_iWeight[iI] = PlotValueForSpecificYield(pLoopCity->plot(), (YieldTypes)iI);
					}
					cityData.m_iWeight[iI] = AdjustValueBasedOnBuildings(pLoopCity, (YieldTypes)iI, cityData.m_iWeight[iI]);
					if(cityData.m_iWeight[iI] < 0)
					{
						cityData.m_iWeight[iI] = 0;
					}
				}
				citiesWithoutSpecialization.push_back(cityData);

				LogCity(pLoopCity, cityData);

			}
		}
	}

	FAssert(citiesWithoutSpecialization.size() + 1 == m_SpecializationsNeeded.size());




	if(citiesWithoutSpecialization.empty())
	{
		it = m_SpecializationsNeeded.begin();
		m_eNextSpecializationDesired = *it;
		LogNextSpecialization(m_eNextSpecializationDesired);
		return;
	}


	bool bAllGeneral = true;
	it = m_SpecializationsNeeded.begin();
	iterEnd = m_SpecializationsNeeded.end();
	for(; it != iterEnd; ++it)
	{
		CitySpecializationTypes eType = *it;
		if(eType != GetEconomicDefaultSpecialization())
		{
			bAllGeneral = false;
		}
	}

	if(bAllGeneral)
	{
		m_eNextSpecializationDesired = GetEconomicDefaultSpecialization();
		LogNextSpecialization(m_eNextSpecializationDesired);
		m_SpecializationsNeeded.erase(m_SpecializationsNeeded.begin());
	}

	else
	{

		FindBestSites();


		int iCurrentDelta;
		int iBestDelta[YIELD_SCIENCE + 1];
		for(iI = 0; iI <= YIELD_SCIENCE; iI++)
		{
			iBestDelta[iI] = MIN_INT;
		}
		cityIter = citiesWithoutSpecialization.begin();
		cityIterEnd = citiesWithoutSpecialization.end();
		for(; cityIter != cityIterEnd; ++cityIter)
		{
			cityData = *cityIter;
			for(iI = 0; iI <= YIELD_SCIENCE; iI++)
			{
				iCurrentDelta = cityData.m_iWeight[iI] - m_iBestValue[iI];
				if(iCurrentDelta > iBestDelta[iI])
				{
					iBestDelta[iI] = iCurrentDelta;
				}
			}
		}


		CvWeightedVector<int, YIELD_SCIENCE+1, true> yieldImprovements;
		for(iI = 0; iI <= YIELD_SCIENCE; iI++)
		{
			int iImprovementWithNewCity;
			if(iBestDelta[iI] > 0)
			{
				iImprovementWithNewCity = 0;
			}
			else
			{
				iImprovementWithNewCity = -iBestDelta[iI];
			}
			yieldImprovements.push_back(iI, iImprovementWithNewCity);
		}
		yieldImprovements.SortItems();


		bool bFoundIt = false;
		for(iI = 0; iI < yieldImprovements.size(); iI++)
		{
			YieldTypes eMostImprovedYield = (YieldTypes)yieldImprovements.GetElement(iI);


			it = m_SpecializationsNeeded.begin();
			iterEnd = m_SpecializationsNeeded.end();
			for(; it != iterEnd; ++it)
			{
				CitySpecializationTypes eType = *it;
				YieldTypes eYield = GC.getCitySpecializationInfo(eType)->GetYieldType();
				if(eYield == eMostImprovedYield)
				{
					m_eNextSpecializationDesired = eType;
					LogNextSpecialization(m_eNextSpecializationDesired);
					m_SpecializationsNeeded.erase(it);
					bFoundIt = true;
					break;
				}
			}
			if(bFoundIt)
			{
				break;
			}
		}
	}

	FAssert(citiesWithoutSpecialization.size() == m_SpecializationsNeeded.size());


	it = m_SpecializationsNeeded.begin();
	iterEnd = m_SpecializationsNeeded.end();
	for(; it != iterEnd; ++it)
	{
		const CitySpecializationTypes eType = *it;
		CvCitySpecializationXMLEntry* pkCitySpecializationEntry = GC.getCitySpecializationInfo(eType);
		if(pkCitySpecializationEntry == NULL)
			continue;

		YieldTypes eYield = pkCitySpecializationEntry->GetYieldType();
		bool bCoastal = pkCitySpecializationEntry->IsMustBeCoastal();
		bestCity = citiesWithoutSpecialization.end();


		int iBestValue = -1;
		cityIter = citiesWithoutSpecialization.begin();
		cityIterEnd = citiesWithoutSpecialization.end();
		for(; cityIter != cityIterEnd; ++cityIter)
		{
			cityData = *cityIter;

			if(bCoastal && !m_pPlayer->getCity(cityData.m_eID)->isCoastal(100))
			{
				continue;
			}

			if(eYield == NO_YIELD)
			{

				int iCityValue = 0;
				for(iI = 0; iI <= YIELD_SCIENCE; iI++)
				{
					iCityValue += cityData.m_iWeight[iI];
				}
				if(iCityValue > iBestValue)
				{
					iBestValue = iCityValue;
					bestCity = cityIter;
				}
			}
			else
			{
				if(cityData.m_iWeight[(int)eYield] > iBestValue)
				{
					iBestValue = cityData.m_iWeight[(int)eYield];
					bestCity = cityIter;
				}
			}
		}


		if(bestCity != citiesWithoutSpecialization.end())
		{
			CvCity* pCity = m_pPlayer->getCity(bestCity->m_eID);
			pCity->GetCityStrategyAI()->SetSpecialization(eType);
			LogSpecializationAssignment(pCity, eType);
			citiesWithoutSpecialization.erase(bestCity);
		}


		else
		{
			CvCity* pCity = m_pPlayer->getCity(citiesWithoutSpecialization.begin()->m_eID);
			pCity->GetCityStrategyAI()->SetSpecialization(GetEconomicDefaultSpecialization());
			LogSpecializationAssignment(pCity, GetEconomicDefaultSpecialization());
			citiesWithoutSpecialization.erase(citiesWithoutSpecialization.begin());
		}
	}

	return;
}


void CvCitySpecializationAI::SelectSpecializations()
{
	CitySpecializationTypes eSpecialization;
	unsigned int iSpecializationsToAssign = (unsigned int)m_pPlayer->getNumCities() - (unsigned int) m_pPlayer->GetNumPuppetCities() + 1;
	int iOldWeight;
	int iNewWeight;
	int iReductionAmount;

	m_SpecializationsNeeded.clear();
	m_bWonderChosen = false;


	for(int iI = 0; iI < NUM_SPECIALIZATION_YIELDS; iI++)
	{
		m_iNumSpecializationsForThisYield[iI] = 0;
	}
	for(int iI = 0; iI < NUM_PRODUCTION_SPECIALIZATION_SUBTYPES; iI++)
	{
		m_iNumSpecializationsForThisSubtype[iI] = 0;
	}

	CvCity* pkWonderBuildCity = GetWonderBuildCity();
	CvBuildingEntry* pkProductionBuildingInfo = NULL;
	if(NULL != pkWonderBuildCity && pkWonderBuildCity->getProductionBuilding() != NO_BUILDING)
	{
		pkProductionBuildingInfo = GC.getBuildingInfo(pkWonderBuildCity->getProductionBuilding());
	}


	if(!m_bInterruptWonders && NULL != pkProductionBuildingInfo && m_pPlayer->GetWonderProductionAI()->IsWonder(*pkProductionBuildingInfo))
	{
		m_SpecializationsNeeded.push_back(GetWonderSpecialization());
		m_iNumSpecializationsForThisYield[1 + (int)YIELD_PRODUCTION]++;
		iOldWeight = m_YieldWeights.GetWeight(YIELD_PRODUCTION);
		iReductionAmount = m_ProductionSubtypeWeights.GetWeight(GetWonderSubtype());
		m_YieldWeights.SetWeight(YIELD_PRODUCTION, (iOldWeight - iReductionAmount));


		m_ProductionSubtypeWeights.SetWeight(GetWonderSubtype(), 0);
		m_bWonderChosen = true;
	}
	else
	{
		m_iWonderCityID = -1;
	}


	while(m_SpecializationsNeeded.size() < iSpecializationsToAssign)
	{

		m_YieldWeights.SortItems();


		YieldTypes eYield = m_YieldWeights.GetElement(0);
		if(GC.GetGameCitySpecializations()->GetNumSpecializationsForYield(eYield) > 1)
		{
			if(eYield == YIELD_PRODUCTION)
			{
				eSpecialization = SelectProductionSpecialization(iReductionAmount);

				iOldWeight = m_YieldWeights.GetWeight(0);
				m_iNumSpecializationsForThisYield[1 + (int)eYield]++;
				iNewWeight = iOldWeight - iReductionAmount;
				m_YieldWeights.SetWeight(0, iNewWeight);
			}
			else
			{
				FAssertMsg(false, "Code does not support > 1 specialization for yields other than production.");
			}
		}
		else
		{
			eSpecialization = GC.GetGameCitySpecializations()->GetFirstSpecializationForYield(eYield);


			iOldWeight = m_YieldWeights.GetWeight(0);
			m_iNumSpecializationsForThisYield[1 + (int)eYield]++;
			iNewWeight = iOldWeight * m_iNumSpecializationsForThisYield[1 + (int)eYield] / (m_iNumSpecializationsForThisYield[1 + (int)eYield] + 1);
			m_YieldWeights.SetWeight(0, iNewWeight);
		}
		m_SpecializationsNeeded.push_back(eSpecialization);
	}
}


CitySpecializationTypes CvCitySpecializationAI::SelectProductionSpecialization(int& iReductionAmount)
{
	CitySpecializationTypes eSpecialization;


	m_ProductionSubtypeWeights.SortItems();

	ProductionSpecializationSubtypes eSubtype = m_ProductionSubtypeWeights.GetElement(0);

	int iNumSubTypes = 0;
	eSpecialization = GC.GetGameCitySpecializations()->GetFirstSpecializationForYield(YIELD_PRODUCTION);
	while(iNumSubTypes != (int)eSubtype)
	{
		eSpecialization = GC.GetGameCitySpecializations()->GetNextSpecializationForYield();
		iNumSubTypes++;
		FAssertMsg(eSpecialization != NO_CITY_SPECIALIZATION, "Production specializations in XML doesn't match NUM_PRODUCTION_SPECIALIZATION_SUBTYPES");
	}


	if(eSubtype == GetWonderSubtype())
	{
		CvCity* pCity = FindBestWonderCity();
		if(pCity != NULL)
		{
			m_iWonderCityID = pCity->GetID();
			m_bWonderChosen = true;
		}


		else
		{
			eSpecialization = GetEconomicDefaultSpecialization();
		}
		iReductionAmount = m_ProductionSubtypeWeights.GetWeight(0);
		m_ProductionSubtypeWeights.SetWeight(0, 0);
	}

	else
	{

		int iOldWeight = m_ProductionSubtypeWeights.GetWeight(0);
		m_iNumSpecializationsForThisSubtype[(int)iNumSubTypes]++;
		int iNewWeight = iOldWeight * m_iNumSpecializationsForThisSubtype[(int)iNumSubTypes] / (m_iNumSpecializationsForThisSubtype[(int)iNumSubTypes] + 1);
		iReductionAmount = iOldWeight - iNewWeight;
		m_ProductionSubtypeWeights.SetWeight(0, iNewWeight);
	}

	return eSpecialization;
}


CitySpecializationTypes CvCitySpecializationAI::GetWonderSpecialization() const
{
	CvCitySpecializationXMLEntry* pEntry;

#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumCitySpecializationInfos(); iI++)
#else
	for(int iI = 0; iI < GC.getNumCitySpecializationInfos(); iI++)
#endif
	{
		pEntry = GC.GetGameCitySpecializations()->GetEntry(iI);
		if(pEntry->IsWonder())
		{
			return (CitySpecializationTypes)iI;
		}
	}

	return (CitySpecializationTypes)-1;
}


CitySpecializationTypes CvCitySpecializationAI::GetEconomicDefaultSpecialization() const
{
	CvCitySpecializationXMLEntry* pEntry;

#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumCitySpecializationInfos(); iI++)
#else
	for(int iI = 0; iI < GC.getNumCitySpecializationInfos(); iI++)
#endif
	{
		pEntry = GC.GetGameCitySpecializations()->GetEntry(iI);
		if(pEntry->IsDefault())
		{
			return (CitySpecializationTypes)iI;
		}
	}

	return (CitySpecializationTypes)-1;
}


int CvCitySpecializationAI::GetWonderSubtype() const
{
	CvCitySpecializationXMLEntry* pEntry;
	int iSubType = -1;

#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumCitySpecializationInfos(); iI++)
#else
	for(int iI = 0; iI < GC.getNumCitySpecializationInfos(); iI++)
#endif
	{
		pEntry = GC.GetGameCitySpecializations()->GetEntry(iI);
		if(pEntry->GetYieldType() == YIELD_PRODUCTION)
		{
			iSubType++;
		}
		if(pEntry->IsWonder())
		{
			return iSubType;
		}
	}

	return iSubType;
}


CvCity* CvCitySpecializationAI::FindBestWonderCity() const
{
	CvCity* pBestCity = NULL;
	CvCity* pLoopCity;
	int iLoop;
	int iBestProduction = 0;
	int iProduction;


	pLoopCity = NULL;

	CvBuildingEntry* pkProductionBuildingInfo = NULL;
	if(pLoopCity != NULL && pLoopCity->getProductionBuilding() != NO_BUILDING)
	{
		pkProductionBuildingInfo = GC.getBuildingInfo(pLoopCity->getProductionBuilding());
	}

	if(pkProductionBuildingInfo && m_pPlayer->GetWonderProductionAI()->IsWonder(*pkProductionBuildingInfo))
	{
		if(!pLoopCity->IsPuppet())
		{
			return pLoopCity;
		}
	}
	else if(m_eNextWonderDesired != NO_BUILDING)
	{
		for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			if(!pLoopCity->IsPuppet())
			{
				if(pLoopCity->canConstruct(m_eNextWonderDesired))
				{
					iProduction = pLoopCity->getCurrentProductionDifference(true, false);
					if(pLoopCity->GetCityStrategyAI()->GetDefaultSpecialization() == GetWonderSpecialization())
					{
						iProduction = (iProduction * 3) / 2;
					}


					iProduction = (iProduction * (100 + pLoopCity->GetWonderProductionModifier())) / 100;

					if(iProduction > iBestProduction)
					{
						pBestCity = pLoopCity;
						iBestProduction = iProduction;
					}
				}
			}
		}
	}

	return pBestCity;
}


void CvCitySpecializationAI::FindBestSites()
{
	CvPlot* pPlot;
	int iPlotValue;
#ifdef AUI_WARNING_FIXES
	uint iPlotLoop;
#else
	int iPlotLoop;
#endif
	CvCity* pNearestCity;


	for(int iI = 0; iI <= YIELD_SCIENCE; iI++)
	{
		m_iBestValue[iI] = 0;
	}


	int iEvalDistance = GC.getSETTLER_EVALUATION_DISTANCE() / 2;

	CvSiteEvaluatorForSettler* pSiteEval = GC.getGame().GetSettlerSiteEvaluator();
	for(iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

		if(pSiteEval->CanFound(pPlot, m_pPlayer, true))
		{

			pNearestCity = GC.getMap().findCity(pPlot->getX(), pPlot->getY(), m_pPlayer->GetID(), NO_TEAM, true                );
			if(pNearestCity != NULL)
			{
				if(plotDistance(pPlot->getX(), pPlot->getY(), pNearestCity->getX(), pNearestCity->getY()) <= iEvalDistance)
				{
					for(int iI = 0; iI <= YIELD_SCIENCE; iI++)
					{
						if(iI != YIELD_SCIENCE)
						{
							iPlotValue = PlotValueForSpecificYield(pPlot, (YieldTypes)iI);
						}
						else
						{
							iPlotValue = PlotValueForScience(pPlot);
						}

						if(iPlotValue > m_iBestValue[iI])
						{
							m_iBestValue[iI] = iPlotValue;
						}
					}
				}
			}
		}
	}

	LogBestSites();

	return;
}


int CvCitySpecializationAI::PlotValueForSpecificYield(CvPlot* pPlot, YieldTypes eYield)
{
	int iTotalPotentialYield = 0;
	int iMultiplier = 0;
	int iPotentialYield = 0;
	int iFirstRingMultiplier = GC.getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_FIRST_RING();
	int iSecondRingMultiplier = GC.getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_SECOND_RING();
	int iThirdRingMultiplier = GC.getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_THIRD_RING();


	for(int iI = 0; iI < GC.getAI_CITY_SPECIALIZATION_YIELD_NUM_TILES_CONSIDERED(); iI++)
	{
		if(iI != CITY_HOME_PLOT)
		{
			CvPlot* pLoopPlot = plotCity(pPlot->getX(), pPlot->getY(), iI);
			if(pLoopPlot != NULL)
			{
				iPotentialYield = pLoopPlot->getYield(eYield);


				if(pLoopPlot->isOwned() && pLoopPlot->getOwner() != m_pPlayer->GetID())
				{
					iMultiplier = 0;
				}
				else
				{
					int iDistance = plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pPlot->getX(), pPlot->getY());
					if(iDistance == 1)
					{
						iMultiplier = iFirstRingMultiplier;
					}
					else if(iDistance == 2)
					{
						iMultiplier = iSecondRingMultiplier;
					}
					else if(iDistance == 3)
					{
						iMultiplier = iThirdRingMultiplier;
					}
				}
				iTotalPotentialYield += iPotentialYield * iMultiplier;
			}
		}
	}

	return iTotalPotentialYield;
}


int CvCitySpecializationAI::PlotValueForScience(CvPlot* pPlot)
{


	int iTotalFoodYield = 0;
	int iTotalClearTileWeight = 0;
	int iMultiplier = 0;
	int iPotentialYield = 0;
	int iFirstRingMultiplier = GC.getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_FIRST_RING();
	int iSecondRingMultiplier = GC.getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_SECOND_RING();
	int iThirdRingMultiplier = GC.getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_THIRD_RING();


	for(int iI = 0; iI < GC.getAI_CITY_SPECIALIZATION_YIELD_NUM_TILES_CONSIDERED(); iI++)
	{
		bool bIsClear = false;

		if(iI != CITY_HOME_PLOT)
		{
			CvPlot* pLoopPlot = plotCity(pPlot->getX(), pPlot->getY(), iI);
			if(pLoopPlot != NULL)
			{
				if(pLoopPlot->getResourceType() == NO_RESOURCE)
				{
					if(pLoopPlot->getFeatureType() == NO_FEATURE)
					{
						if(!pLoopPlot->isHills())
						{
							if(pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
							{
								bIsClear = true;
							}
						}
					}
				}

				iPotentialYield = pLoopPlot->getYield(YIELD_FOOD) + pLoopPlot->getYield(YIELD_SCIENCE);


				if(pLoopPlot->isOwned() && pLoopPlot->getOwner() != m_pPlayer->GetID())
				{
					iMultiplier = 0;
				}
				else
				{
					int iDistance = plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pPlot->getX(), pPlot->getY());
					if(iDistance == 1)
					{
						iMultiplier = iFirstRingMultiplier;
					}
					else if(iDistance == 2)
					{
						iMultiplier = iSecondRingMultiplier;
					}
					else if(iDistance == 3)
					{
						iMultiplier = iThirdRingMultiplier;
					}
				}

				iTotalFoodYield += iPotentialYield * iMultiplier;
				if(bIsClear)
				{
					iTotalClearTileWeight += iMultiplier;
				}
			}
		}
	}

	return iTotalFoodYield + iTotalClearTileWeight;
}


int CvCitySpecializationAI::AdjustValueBasedOnBuildings(CvCity* pCity, YieldTypes eYield, int iInitialValue)
{
	int iRtnValue;


	iRtnValue = iInitialValue * (100 + pCity->getYieldRateModifier(eYield)) / 100;


	int iYieldPerPop = pCity->GetYieldPerPopTimes100(eYield);
	if(iYieldPerPop > 0)
	{
		iRtnValue = iRtnValue * (100 + iYieldPerPop) / 100;
	}


	int iYieldPerReligion = pCity->GetYieldPerReligionTimes100(eYield);
	if(iYieldPerPop > 0)
	{
		iRtnValue = iRtnValue * (100 + (iYieldPerReligion * pCity->GetCityReligions()->GetNumReligionsWithFollowers())) / 100;
	}


	int iYieldChanges = pCity->GetBaseYieldRateFromBuildings(eYield);
	if(iYieldChanges > 0)
	{

		iRtnValue = iRtnValue * (100 + (iYieldChanges * 20)) / 100;
	}


	switch(eYield)
	{
	case YIELD_FOOD:
	{
		int iMaxFoodKeptPercent = pCity->getMaxFoodKeptPercent();
		if(iMaxFoodKeptPercent >= 100)
		{
			iMaxFoodKeptPercent = 99;
		}
		iRtnValue *= 100 / (100 - iMaxFoodKeptPercent);
	}
	break;

	case YIELD_PRODUCTION:

		if(pCity->getDomainFreeExperience(DOMAIN_LAND) > 0)
		{
			iRtnValue *= 2;
		}
		break;

	case YIELD_GOLD:
		break;

	case YIELD_SCIENCE:
		break;
	}

	return iRtnValue;
}


bool CvCitySpecializationAI::CanBuildSpaceshipParts()
{
	int iLoop;


	CvCity* pCity = m_pPlayer->firstCity(&iLoop);


#ifdef AUI_WARNING_FIXES
	for (uint iUnitLoop = 0; iUnitLoop < GC.GetGameUnits()->GetNumUnits(); iUnitLoop++)
#else
	for(int iUnitLoop = 0; iUnitLoop < GC.GetGameUnits()->GetNumUnits(); iUnitLoop++)
#endif
	{
		const UnitTypes eUnit = static_cast<UnitTypes>(iUnitLoop);
		CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eUnit);
		if(pkUnitEntry)
		{
			if(pkUnitEntry->GetSpaceshipProject() != NO_PROJECT)
			{

				if(pCity->canTrain((UnitTypes)iUnitLoop))
				{
					return true;
				}
			}
		}
	}
	return false;
}


void CvCitySpecializationAI::LogSpecializationWeights()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strPlayerName;
		CvString strBaseString;
		CvString strYieldString;
		CvString strWeightString;
		CvString strOutBuf;
		FILogFile* pLog;


		strPlayerName = GetPlayer()->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", Empire-wide specialization weight, ";

		for(int iI = 0; iI < NUM_SPECIALIZATION_YIELDS; iI++)
		{
			if(iI > YIELD_SCIENCE)
			{
				strYieldString = "General Economy";
			}
			else
			{
				CvYieldInfo* pYieldInfo = GC.getYieldInfo((YieldTypes)m_YieldWeights.GetElement(iI));
				if(pYieldInfo != NULL)
				{
					strYieldString = pYieldInfo->GetDescription();
				}
			}
			strWeightString.Format("%d", m_YieldWeights.GetWeight(iI));
			strOutBuf = strBaseString + strYieldString + ", " + strWeightString;
			pLog->Msg(strOutBuf);
		}

		for(int iI = 0; iI < NUM_PRODUCTION_SPECIALIZATION_SUBTYPES; iI++)
		{
			strYieldString.Format("Production Specialization: %d", iI);
			strWeightString.Format("%d", m_ProductionSubtypeWeights.GetWeight(iI));
			strOutBuf = strBaseString + strYieldString + ", " + strWeightString;
			pLog->Msg(strOutBuf);
		}
	}
}


void CvCitySpecializationAI::LogSpecializationAssignment(CvCity* pCity, CitySpecializationTypes eType, bool bWonderCity)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strPlayerName;
		CvString strCityName;
		CvString strSpecialization;
		FILogFile* pLog;


		strPlayerName = GetPlayer()->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";

		strCityName = pCity->getName();
		strSpecialization.Format("New Specialization Type: %d", (int)eType);

		strOutBuf = strBaseString + strCityName + ", " + strSpecialization;
		if(bWonderCity)
		{
			strOutBuf += ", WonderCity";
		}

		pLog->Msg(strOutBuf);
	}
}


void CvCitySpecializationAI::LogSpecializationUpdate(CitySpecializationUpdateType eUpdate)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strPlayerName;
		CvString strTypeString;
		FILogFile* pLog;


		strPlayerName = GetPlayer()->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";

		switch(eUpdate)
		{
		case SPECIALIZATION_UPDATE_NEW_GRAND_STRATEGY:
		{
			CvAIGrandStrategyXMLEntry* pGrandStrategyInfo = GC.getAIGrandStrategyInfo(m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy());
			if(NULL != pGrandStrategyInfo)
			{
				strTypeString.Format("Update: New grand strategy: %s", pGrandStrategyInfo->GetType());
			}
		}
		break;
		case SPECIALIZATION_UPDATE_NOW_AT_WAR:
			strTypeString = "Update: New war";
			break;
		case SPECIALIZATION_UPDATE_CITY_FOUNDED:
			strTypeString = "Update: City founded";
			break;
		case SPECIALIZATION_UPDATE_MY_CITY_CAPTURED:
			strTypeString = "Update: My city captured";
			break;
		case SPECIALIZATION_UPDATE_ENEMY_CITY_CAPTURED:
			strTypeString = "Update: Enemy city captured";
			break;
		case SPECIALIZATION_UPDATE_STRATEGY_NOW_ON:
			strTypeString = "Update: War strategy turned on";
			break;
		case SPECIALIZATION_UPDATE_STRATEGY_NOW_OFF:
			strTypeString = "Update: War strategy turned off";
			break;
		case SPECIALIZATION_UPDATE_RESEARCH_COMPLETE:
			strTypeString = "Update: Tech research complete";
			break;
		case SPECIALIZATION_UPDATE_WONDER_BUILT_BY_RIVAL:
			strTypeString = "Update: wonder built by rival, WONDER";
		}
		strOutBuf = strBaseString + strTypeString;
		pLog->Msg(strOutBuf);
	}
}


void CvCitySpecializationAI::LogNextSpecialization(CitySpecializationTypes eType)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strPlayerName;
		CvString strTypeString;
		CvString strYieldString;
		FILogFile* pLog;


		strPlayerName = GetPlayer()->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";

		strTypeString.Format("Next Specialization: %d", (int)eType);

		strOutBuf = strBaseString + strTypeString;
		pLog->Msg(strOutBuf);
	}
}

void CvCitySpecializationAI::LogBestSites()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strPlayerName;
		CvString strYieldString;
		CvString strWeightString;
		FILogFile* pLog;


		strPlayerName = GetPlayer()->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";


		for(int iI = 0; iI <= YIELD_SCIENCE; iI++)
		{
			CvYieldInfo* pYieldInfo = GC.getYieldInfo((YieldTypes)iI);
			if(pYieldInfo != NULL)
			{
				strYieldString = pYieldInfo->GetDescription();
			}

			strWeightString.Format(", Best site value: %d", m_iBestValue[iI]);
			strOutBuf = strBaseString + strYieldString + strWeightString;
			pLog->Msg(strOutBuf);
		}
	}
}

#ifdef AUI_WARNING_FIXES
void CvCitySpecializationAI::LogCity(CvCity* pCity, const CitySpecializationData& data)
#else
void CvCitySpecializationAI::LogCity(CvCity* pCity, CitySpecializationData data)
#endif
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strPlayerName;
		CvString strCityName;
		CvString strYieldString;
		CvString strWeightString;
		FILogFile* pLog;


		strPlayerName = GetPlayer()->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);
		strCityName = pCity->getName();


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", " + strCityName + ", ";


		for(int iI = 0; iI <= YIELD_SCIENCE; iI++)
		{
			CvYieldInfo* pYieldInfo = GC.getYieldInfo((YieldTypes)iI);
			if(pYieldInfo != NULL)
			{
				strYieldString = pYieldInfo->GetDescription();
			}

			strWeightString.Format(", Value: %d", data.m_iWeight[iI]);
			strOutBuf = strBaseString + strYieldString + strWeightString;
			pLog->Msg(strOutBuf);
		}
	}
}


CvString CvCitySpecializationAI::GetLogFileName(CvString& playerName) const
{
	CvString strLogName;


	if(GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "CitySpecializationLog_" + playerName + ".csv";
	}
	else
	{
		strLogName = "CitySpecializationLog.csv";
	}

	return strLogName;
}
