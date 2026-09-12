






#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "CvBuildingProductionAI.h"
#include "CvInfosSerializationHelper.h"


#include "LintFree.h"


CvBuildingProductionAI::CvBuildingProductionAI(CvCity* pCity, CvCityBuildings* pCityBuildings):
	m_pCity(pCity),
	m_pCityBuildings(pCityBuildings)
{
}


CvBuildingProductionAI::~CvBuildingProductionAI(void)
{
}


void CvBuildingProductionAI::Reset()
{
	CvAssertMsg(m_pCityBuildings != NULL, "Building Production AI init failure: city buildings are NULL");

	m_BuildingAIWeights.clear();


	if(m_pCityBuildings)
	{
#ifdef AUI_WARNING_FIXES
		for (uint i = 0; i < m_pCityBuildings->GetBuildings()->GetNumBuildings(); i++)
#else
		for(int i = 0; i < m_pCityBuildings->GetBuildings()->GetNumBuildings(); i++)
#endif
		{
			m_BuildingAIWeights.push_back(i, 0);
		}
	}
}


void CvBuildingProductionAI::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;


	m_BuildingAIWeights.clear();


	if(m_pCityBuildings)
	{
#ifdef AUI_WARNING_FIXES
		for (uint i = 0; i < m_pCityBuildings->GetBuildings()->GetNumBuildings(); i++)
#else
		for(int i = 0; i < m_pCityBuildings->GetBuildings()->GetNumBuildings(); i++)
#endif
		{
			m_BuildingAIWeights.push_back(i, 0);
		}

		int iNumEntries;
		FStringFixedBuffer(sTemp, 64);
		int iType;

		kStream >> iNumEntries;

		for(int iI = 0; iI < iNumEntries; iI++)
		{
			bool bValid = true;
			iType = CvInfosSerializationHelper::ReadHashed(kStream, &bValid);
			if(iType != -1 || !bValid)
			{
				int iWeight;
				kStream >> iWeight;
				if(iType != -1)
				{
					m_BuildingAIWeights.IncreaseWeight(iType, iWeight);
				}
				else
				{
					CvString szError;
					szError.Format("LOAD ERROR: Building Type not found");
					GC.LogMessage(szError.GetCString());
					CvAssertMsg(false, szError);
				}
			}
		}
	}
	else
	{
		CvAssertMsg(m_pCityBuildings != NULL, "Building Production AI init failure: city buildings are NULL");
	}
}


void CvBuildingProductionAI::Write(FDataStream& kStream)
{
	CvAssertMsg(m_pCityBuildings != NULL, "Building Production AI init failure: city buildings are NULL");


	uint uiVersion = 1;
	kStream << uiVersion;

	if(m_pCityBuildings)
	{
		int iNumBuildings = m_pCityBuildings->GetBuildings()->GetNumBuildings();
		kStream << iNumBuildings;


		for(int iI = 0; iI < iNumBuildings; iI++)
		{
			const BuildingTypes eBuilding = static_cast<BuildingTypes>(iI);
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if(pkBuildingInfo)
			{
				CvInfosSerializationHelper::WriteHashed(kStream, pkBuildingInfo);
				kStream << m_BuildingAIWeights.GetWeight(iI);
			}
			else
			{
				kStream << (int)0;
			}
		}
	}
}


void CvBuildingProductionAI::AddFlavorWeights(FlavorTypes eFlavor, int iWeight)
{
#ifdef AUI_POLICY_BUILDING_CLASS_FLAVOR_MODIFIERS
	CvPlayer* pPlayer = m_pCity->GetPlayer();
	CvPlayerPolicies* pPlayerPolicies = NULL;
	if (pPlayer)
		pPlayerPolicies = pPlayer->GetPlayerPolicies();
#endif
#ifdef AUI_BELIEF_BUILDING_CLASS_FLAVOR_MODIFIERS
	const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(m_pCity->GetCityReligions()->GetReligiousMajority(), m_pCity->getOwner());
#endif
	CvBuildingXMLEntries* pkBuildings = m_pCityBuildings->GetBuildings();


#ifdef AUI_WARNING_FIXES
	for (uint iBuilding = 0; iBuilding < m_pCityBuildings->GetBuildings()->GetNumBuildings(); iBuilding++)
#else
	for(int iBuilding = 0; iBuilding < m_pCityBuildings->GetBuildings()->GetNumBuildings(); iBuilding++)
#endif
	{
		CvBuildingEntry* entry = pkBuildings->GetEntry(iBuilding);
		if(entry)
		{

#if defined(AUI_POLICY_BUILDING_CLASS_FLAVOR_MODIFIERS) || defined(AUI_BELIEF_BUILDING_CLASS_FLAVOR_MODIFIERS) || defined(AUI_BUILDING_PRODUCTION_AI_LUA_FLAVOR_WEIGHTS) || defined(AUI_BUILDING_PRODUCTION_AI_CONSIDER_FREE_STUFF)
			int iFlavorValue = entry->GetFlavorValue(eFlavor);
#endif
#ifdef AUI_POLICY_BUILDING_CLASS_FLAVOR_MODIFIERS
			if (pPlayerPolicies)
			{
				for (int iI = 0; iI < GC.getNumPolicyInfos(); iI++)
				{
					PolicyTypes ePolicy = static_cast<PolicyTypes>(iI);
					CvPolicyEntry* pPolicy = GC.getPolicyInfo(ePolicy);
					if (pPolicy && pPlayerPolicies->HasPolicy(ePolicy))
					{
						iFlavorValue += pPolicy->GetBuildingClassFlavorChanges(entry->GetBuildingClassType(), eFlavor);
					}
				}
			}
#endif
#ifdef AUI_BELIEF_BUILDING_CLASS_FLAVOR_MODIFIERS
			if (pReligion)
			{
				pReligion->m_Beliefs.GetBuildingClassFlavorChange(static_cast<BuildingClassTypes>(entry->GetBuildingClassType()), eFlavor);
			}
#endif
#ifdef AUI_BUILDING_PRODUCTION_AI_CONSIDER_FREE_STUFF
#ifdef AUI_WARNING_FIXES
			for (uint iI = 0; iI < GC.getNumUnitInfos(); iI++)
#else
			for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
#endif
			{
				int iNumFreeUnits = entry->GetNumFreeUnits(iI);
				if (iNumFreeUnits > 0)
				{
					iFlavorValue += iNumFreeUnits * m_pCity->GetCityStrategyAI()->GetUnitProductionAI()->GetWeight((UnitTypes)iI);
				}
			}
#endif
#if defined(AUI_POLICY_BUILDING_CLASS_FLAVOR_MODIFIERS) || defined(AUI_BELIEF_BUILDING_CLASS_FLAVOR_MODIFIERS) || defined(AUI_BUILDING_PRODUCTION_AI_LUA_FLAVOR_WEIGHTS) || defined(AUI_BUILDING_PRODUCTION_AI_CONSIDER_FREE_STUFF)
			m_BuildingAIWeights.IncreaseWeight(iBuilding, iFlavorValue * iWeight);
#else
			m_BuildingAIWeights.IncreaseWeight(iBuilding, entry->GetFlavorValue(eFlavor) * iWeight);
#endif
		}
	}
}


int CvBuildingProductionAI::GetWeight(BuildingTypes eBuilding)
{
#ifdef AUI_BUILDING_PRODUCTION_AI_CONSIDER_FREE_STUFF
	CvBuildingXMLEntries* pkBuildings = m_pCityBuildings->GetBuildings();
	int iWeight = m_BuildingAIWeights.GetWeight(eBuilding);
	CvBuildingEntry* entry = pkBuildings->GetEntry(eBuilding);
	if (entry)
	{
		CvPlayer* pPlayer = m_pCity->GetPlayer();
		int iLoop = 0;

		BuildingTypes eFreeBuildingThisCity = static_cast<BuildingTypes>(entry->GetFreeBuildingThisCity());
		if (eFreeBuildingThisCity != NO_BUILDING)
		{
			if (m_pCityBuildings->GetNumBuilding(eFreeBuildingThisCity) == 0)
				iWeight += m_BuildingAIWeights.GetWeight(eFreeBuildingThisCity);
		}

		BuildingClassTypes eFreeBuildingClassAllCities = static_cast<BuildingClassTypes>(entry->GetFreeBuildingClass());
		if (eFreeBuildingClassAllCities != NO_BUILDINGCLASS)
		{
			BuildingTypes eFreeBuilding = static_cast<BuildingTypes>(m_pCity->getCivilizationInfo().getCivilizationBuildings(eFreeBuildingClassAllCities));
			for (CvCity* pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop))
			{
				if (pLoopCity->GetCityBuildings()->GetNumBuilding(eFreeBuilding) == 0)
					iWeight += pLoopCity->GetCityStrategyAI()->GetBuildingProductionAI()->GetWeight(eFreeBuilding);
			}
		}

#ifdef AUI_WARNING_FIXES
		for (uint iI = 0; iI < GC.getNumUnitInfos(); iI++)
#else
		for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
#endif
		{
			int iNumFreeUnits = entry->GetNumFreeUnits(iI);
			if (iNumFreeUnits > 0)
			{
				iWeight += iNumFreeUnits * m_pCity->GetCityStrategyAI()->GetUnitProductionAI()->GetWeight((UnitTypes)iI);
			}
		}

		if (entry->GetInstantMilitaryIncrease())
		{
			FFastVector<UnitTypes, true, c_eCiv5GameplayDLL> aExtraUnits;
			for (CvUnit* pLoopUnit = pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iLoop))
			{
				if (pLoopUnit->getDomainType() == DOMAIN_LAND && pLoopUnit->IsCombatUnit())
				{
					UnitTypes eCurrentUnitType = pLoopUnit->getUnitType();


					bool bAddUnit = true;
					for (uint ui = 0; ui < aExtraUnits.size(); ui++)
					{
						if (aExtraUnits[ui] == eCurrentUnitType)
						{
							bAddUnit = false;
							break;
						}
					}
					if (bAddUnit)
					{
						aExtraUnits.push_back(eCurrentUnitType);
					}
				}
			}
			for (uint ui = 0; ui < aExtraUnits.size(); ui++)
			{
				iWeight += m_pCity->GetCityStrategyAI()->GetUnitProductionAI()->GetWeight(aExtraUnits[ui]);
			}
		}
	}
	return iWeight;
#else
	return m_BuildingAIWeights.GetWeight(eBuilding);
#endif
}


BuildingTypes CvBuildingProductionAI::RecommendBuilding()
{
#ifdef AUI_WARNING_FIXES
	uint iBldgLoop;
#else
	int iBldgLoop;
#endif
	int iWeight;
	int iTurnsLeft;


	m_Buildables.clear();


	for(iBldgLoop = 0; iBldgLoop < GC.GetGameBuildings()->GetNumBuildings(); iBldgLoop++)
	{

		if(m_pCity->canConstruct((BuildingTypes)iBldgLoop))
		{

			iTurnsLeft = m_pCity->getProductionTurnsLeft((BuildingTypes) iBldgLoop, 0);
			iWeight = CityStrategyAIHelpers::ReweightByTurnsLeft(m_BuildingAIWeights.GetWeight((BuildingTypes)iBldgLoop), iTurnsLeft);
			m_Buildables.push_back(iBldgLoop, iWeight);
		}
	}


	if(m_Buildables.size() > 0)
	{
		m_Buildables.SortItems();
		LogPossibleBuilds();
		return (BuildingTypes)m_Buildables.GetElement(0);
	}


	else
	{
		return NO_BUILDING;
	}
}


void CvBuildingProductionAI::LogPossibleBuilds()
{
	if(GC.getLogging() && GC.getAILogging())
	{

		CvString playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
		CvString cityName = m_pCity->getName();


		FILogFile* pLog = LOGFILEMGR.GetLog(m_pCity->GetCityStrategyAI()->GetLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);


		CvString strBaseString;
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", ";


		CvBuildingXMLEntries* pGameBuildings = GC.GetGameBuildings();
		if(pGameBuildings != NULL)
		{
			for(int iI = 0; iI < m_Buildables.size(); iI++)
			{
				CvBuildingEntry* pBuildingEntry = pGameBuildings->GetEntry(m_Buildables.GetElement(iI));;
				if(pBuildingEntry != NULL)
				{
					CvString strTemp;
					strTemp.Format("Building, %s, %d", pBuildingEntry->GetDescription(), m_Buildables.GetWeight(iI));
					CvString strOutBuf = strBaseString + strTemp;
					pLog->Msg(strOutBuf);
				}
			}
		}
	}
}
