






#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "CvWonderProductionAI.h"
#include "CvGameCoreUtils.h"
#include "CvInternalGameCoreUtils.h"
#include "CvCitySpecializationAI.h"
#include "CvMinorCivAI.h"
#include "CvDiplomacyAI.h"
#include "CvInfosSerializationHelper.h"

#include "LintFree.h"



CvWonderProductionAI::CvWonderProductionAI(CvPlayer* pPlayer, CvBuildingXMLEntries* pBuildings):
	m_pPlayer(pPlayer),
	m_pBuildings(pBuildings)
{
}


CvWonderProductionAI::~CvWonderProductionAI(void)
{
}


void CvWonderProductionAI::Init(CvBuildingXMLEntries* pBuildings, CvPlayer* pPlayer, bool bIsCity)
{

	CvFlavorRecipient::Init();


	m_pBuildings = pBuildings;
	m_pPlayer = pPlayer;
	m_bIsCity = bIsCity;

	Reset();
}


void CvWonderProductionAI::Reset()
{
	CvAssertMsg(m_pBuildings != NULL, "Wonder Production AI init failure: building entries are NULL");


	m_WonderAIWeights.clear();


	if(m_pBuildings)
	{
#ifdef AUI_WARNING_FIXES
		for (uint i = 0; i < m_pBuildings->GetNumBuildings(); i++)
#else
		for(int i = 0; i < m_pBuildings->GetNumBuildings(); i++)
#endif
		{
			m_WonderAIWeights.push_back(i, 0);
		}
	}
}


void CvWonderProductionAI::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	int iWeight;

	CvAssertMsg(m_piLatestFlavorValues != NULL && GC.getNumFlavorTypes() > 0, "Number of flavor values to serialize is expected to greater than 0");

	int iNumFlavors;
	kStream >> iNumFlavors;

	ArrayWrapper<int> wrapm_piLatestFlavorValues(iNumFlavors, m_piLatestFlavorValues);
	kStream >> wrapm_piLatestFlavorValues;

	CvAssertMsg(m_pBuildings != NULL, "Wonder Production AI init failure: building entries are NULL");


	m_WonderAIWeights.clear();


	if(m_pBuildings)
	{
#ifdef AUI_WARNING_FIXES
		for (uint i = 0; i < m_pBuildings->GetNumBuildings(); i++)
#else
		for(int i = 0; i < m_pBuildings->GetNumBuildings(); i++)
#endif
		{
			m_WonderAIWeights.push_back(i, 0);
		}

		int iNumEntries;
		int iType;

		kStream >> iNumEntries;

		for(int iI = 0; iI < iNumEntries; iI++)
		{
			bool bValid = true;
			iType = CvInfosSerializationHelper::ReadHashed(kStream, &bValid);
			if(iType != -1 || !bValid)
			{
				kStream >> iWeight;
				if(iType != -1)
				{
					m_WonderAIWeights.IncreaseWeight(iType, iWeight);
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
}


void CvWonderProductionAI::Write(FDataStream& kStream) const
{

	uint uiVersion = 1;
	kStream << uiVersion;

	CvAssertMsg(m_piLatestFlavorValues != NULL && GC.getNumFlavorTypes() > 0, "Number of flavor values to serialize is expected to greater than 0");
	kStream << GC.getNumFlavorTypes();
	kStream << ArrayWrapper<int>(GC.getNumFlavorTypes(), m_piLatestFlavorValues);

	if(m_pBuildings)
	{
		int iNumBuildings = m_pBuildings->GetNumBuildings();
		kStream << iNumBuildings;


		for(int iI = 0; iI < iNumBuildings; iI++)
		{
			const BuildingTypes eBuilding = static_cast<BuildingTypes>(iI);
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if(pkBuildingInfo)
			{
				CvInfosSerializationHelper::WriteHashed(kStream, pkBuildingInfo);
				kStream << m_WonderAIWeights.GetWeight(iI);
			}
			else
			{
				kStream << (int)0;
			}
		}
	}
	else
	{
		CvAssertMsg(m_pBuildings != NULL, "Wonder Production AI init failure: building entries are NULL");
	}
}


void CvWonderProductionAI::FlavorUpdate()
{

	for(int iFlavor = 0; iFlavor < GC.getNumFlavorTypes(); iFlavor++)
	{
		int iFlavorValue = GetLatestFlavorValue((FlavorTypes)iFlavor);
		AddFlavorWeights((FlavorTypes)iFlavor, iFlavorValue);
	}
}


void CvWonderProductionAI::AddFlavorWeights(FlavorTypes eFlavor, int iWeight)
{
#ifdef AUI_PER_CITY_WONDER_PRODUCTION_AI
#ifdef AUI_POLICY_BUILDING_CLASS_FLAVOR_MODIFIERS
	CvPlayer* pPlayer = m_pCity->GetPlayer();
	CvPlayerPolicies* pPlayerPolicies = NULL;
	if (pPlayer)
		pPlayerPolicies = pPlayer->GetPlayerPolicies();
#endif
#ifdef AUI_BELIEF_BUILDING_CLASS_FLAVOR_MODIFIERS
	const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(m_pCity->GetCityReligions()->GetReligiousMajority(), m_pCity->getOwner());
#endif
#else
#ifdef AUI_POLICY_BUILDING_CLASS_FLAVOR_MODIFIERS
	CvPlayer* pPlayer = m_pPlayer;
	CvPlayerPolicies* pPlayerPolicies = NULL;
	if (pPlayer)
		pPlayerPolicies = pPlayer->GetPlayerPolicies();
#endif
#ifdef AUI_BELIEF_BUILDING_CLASS_FLAVOR_MODIFIERS
	CvCity* pCapitol = m_pPlayer->getCapitalCity();
	const CvReligion* pReligion = NULL;
	if (pCapitol)
		pReligion = GC.getGame().GetGameReligions()->GetReligion(pCapitol->GetCityReligions()->GetReligiousMajority(), m_pPlayer->GetID());
#endif
#endif

#ifdef AUI_WARNING_FIXES
	for (uint iBldg = 0; iBldg < m_pBuildings->GetNumBuildings(); iBldg++)
#else
	for(int iBldg = 0; iBldg < m_pBuildings->GetNumBuildings(); iBldg++)
#endif
	{
		CvBuildingEntry* entry = m_pBuildings->GetEntry(iBldg);
		if(entry)
		{
			CvBuildingEntry& kBuilding = *entry;
			if(IsWonder(kBuilding))
			{

#if defined(AUI_POLICY_BUILDING_CLASS_FLAVOR_MODIFIERS) || defined(AUI_BELIEF_BUILDING_CLASS_FLAVOR_MODIFIERS) || defined(AUI_BUILDING_PRODUCTION_AI_LUA_FLAVOR_WEIGHTS) || (defined(AUI_BUILDING_PRODUCTION_AI_CONSIDER_FREE_STUFF) && defined(AUI_PER_CITY_WONDER_PRODUCTION_AI))
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
#if defined(AUI_BUILDING_PRODUCTION_AI_CONSIDER_FREE_STUFF) && defined(AUI_PER_CITY_WONDER_PRODUCTION_AI)
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
#endif
#if defined(AUI_POLICY_BUILDING_CLASS_FLAVOR_MODIFIERS) || defined(AUI_BELIEF_BUILDING_CLASS_FLAVOR_MODIFIERS) || defined(AUI_BUILDING_PRODUCTION_AI_LUA_FLAVOR_WEIGHTS) || (defined(AUI_BUILDING_PRODUCTION_AI_CONSIDER_FREE_STUFF) && defined(AUI_PER_CITY_WONDER_PRODUCTION_AI))
				m_WonderAIWeights.IncreaseWeight(iBldg, iFlavorValue * iWeight);
#else
				m_WonderAIWeights.IncreaseWeight(iBldg, kBuilding.GetFlavorValue(eFlavor) * iWeight);
#endif
			}
		}
	}
}


int CvWonderProductionAI::GetWeight(BuildingTypes eBldg)
{
#if defined(AUI_BUILDING_PRODUCTION_AI_CONSIDER_FREE_STUFF) && defined(AUI_PER_CITY_WONDER_PRODUCTION_AI)
	int iWeight = m_WonderAIWeights.GetWeight(eBldg);
	CvBuildingEntry* entry = m_pBuildings->GetEntry(eBldg);
	if (entry)
	{
		CvPlayer* pPlayer = m_pCity->GetPlayer();
		int iLoop = 0;

		BuildingTypes eFreeBuildingThisCity = static_cast<BuildingTypes>(entry->GetFreeBuildingThisCity());
		if (eFreeBuildingThisCity != NO_BUILDING)
		{
			if (m_pCity->GetCityBuildings()->GetNumBuilding(eFreeBuildingThisCity) == 0)
				iWeight += m_pCity->GetCityStrategyAI()->GetBuildingProductionAI()->GetWeight(eFreeBuildingThisCity);
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
	return m_WonderAIWeights.GetWeight(eBldg);
#endif
}


BuildingTypes CvWonderProductionAI::ChooseWonder(bool bUseAsyncRandom, bool bAdjustForOtherPlayers, int& iWonderWeight)
{
#ifdef AUI_WARNING_FIXES
	uint iBldgLoop;
#else
	int iBldgLoop;
#endif
	int iWeight;
	int iTurnsRequired;
	int iEstimatedProductionPerTurn;
	int iCityLoop;
	RandomNumberDelegate fcn;
	BuildingTypes eSelection;


	if(bUseAsyncRandom)
	{
		fcn = MakeDelegate(&GC.getGame(), &CvGame::getAsyncRandNum);
	}
	else
	{
		fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
	}


	m_Buildables.clear();


	CvCity* pWonderCity = m_pPlayer->GetCitySpecializationAI()->GetWonderBuildCity();
	if(pWonderCity == NULL)
	{
		pWonderCity = m_pPlayer->firstCity(&iCityLoop);
	}

	CvAssertMsg(pWonderCity, "Trying to choose the next wonder to build and the player has no cities!");
	if(pWonderCity == NULL)
		return NO_BUILDING;

	iEstimatedProductionPerTurn = pWonderCity->getCurrentProductionDifference(true, false);
	if(iEstimatedProductionPerTurn < 1)
	{
		iEstimatedProductionPerTurn = 1;
	}


	for(iBldgLoop = 0; iBldgLoop < GC.GetGameBuildings()->GetNumBuildings(); iBldgLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBldgLoop);
		CvBuildingEntry* pkBuildingInfo = m_pBuildings->GetEntry(eBuilding);
		if(pkBuildingInfo)
		{
			CvBuildingEntry& kBuilding = *pkBuildingInfo;
			const CvBuildingClassInfo& kBuildingClassInfo = kBuilding.GetBuildingClassInfo();
#ifdef NQM_AI_GIMP_NO_WORLD_WONDERS
			if ((GC.getGame().isOption("GAMEOPTION_AI_TWEAKS") || GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_WORLD_WONDER")) && isWorldWonderClass(kBuildingClassInfo))
			{
				continue;
			}
#endif


			if(IsWonder(kBuilding) && HaveCityToBuild((BuildingTypes)iBldgLoop))
			{
				iTurnsRequired = std::max(1, kBuilding.GetProductionCost() / iEstimatedProductionPerTurn);


				bool bAlreadyStarted = pWonderCity->GetCityBuildings()->GetBuildingProduction(eBuilding) > 0;
				int iTempWeight = bAlreadyStarted ? m_WonderAIWeights.GetWeight(iBldgLoop) * 25 : m_WonderAIWeights.GetWeight(iBldgLoop);


				if(pkBuildingInfo->IsDiplomaticVoting())
				{
					int iVotesNeededToWin = GC.getGame().GetVotesNeededForDiploVictory();
					int iSecuredVotes = 0;
					TeamTypes myTeamID = m_pPlayer->getTeam();
					PlayerTypes myPlayerID = m_pPlayer->GetID();


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

					if(!m_pPlayer->GetDiplomacyAI() || !m_pPlayer->GetDiplomacyAI()->IsGoingForDiploVictory() || m_pPlayer->GetTreasury()->GetGold() < iNumberOfPlayersWeNeedToBuyOff * 500 )
					{
						iTempWeight = 0;
					}
				}

				iWeight = CityStrategyAIHelpers::ReweightByTurnsLeft(iTempWeight, iTurnsRequired);

				if(bAdjustForOtherPlayers && ::isWorldWonderClass(kBuildingClassInfo))
				{

					int iNumOthersConstructing = 0;
					for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
					{
						PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
						if(GET_PLAYER(eLoopPlayer).getBuildingClassMaking((BuildingClassTypes)kBuilding.GetBuildingClassType()) > 0)
						{
							iNumOthersConstructing++;
						}
					}
					iWeight = iWeight / (1 + iNumOthersConstructing);
				}

				m_Buildables.push_back(iBldgLoop, iWeight);
			}
		}
	}


	if(m_Buildables.size() > 0)
	{
		m_Buildables.SortItems();
		LogPossibleWonders();

		if(m_Buildables.GetTotalWeight() > 0)
		{
			int iNumChoices = GC.getGame().getHandicapInfo().GetCityProductionNumOptions();
			eSelection = (BuildingTypes)m_Buildables.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing wonder from Top Choices");
			iWonderWeight = m_Buildables.GetTotalWeight();
			return eSelection;
		}


		else
		{
			return NO_BUILDING;
		}
	}


	else
	{
		return NO_BUILDING;
	}
}



BuildingTypes CvWonderProductionAI::ChooseWonderForGreatEngineer(bool bUseAsyncRandom, int& iWonderWeight, CvCity*& pCityToBuildAt)
{
#ifdef AUI_WARNING_FIXES
	uint iBldgLoop;
#else
	int iBldgLoop;
#endif
	int iWeight;
	int iCityLoop;
	RandomNumberDelegate fcn;
	BuildingTypes eSelection;

	pCityToBuildAt = 0;
	iWonderWeight = 0;


	if (bUseAsyncRandom)
	{
		fcn = MakeDelegate(&GC.getGame(), &CvGame::getAsyncRandNum);
	}
	else
	{
		fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
	}


	m_Buildables.clear();


	CvCity* pWonderCity = m_pPlayer->GetCitySpecializationAI()->GetWonderBuildCity();
	if (pWonderCity == NULL)
	{
		pWonderCity = m_pPlayer->firstCity(&iCityLoop);
	}

	CvAssertMsg(pWonderCity, "Trying to choose the next wonder to build and the player has no cities!");
	if (pWonderCity == NULL)
		return NO_BUILDING;


	for (iBldgLoop = 0; iBldgLoop < GC.GetGameBuildings()->GetNumBuildings(); iBldgLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBldgLoop);
		CvBuildingEntry* pkBuildingInfo = m_pBuildings->GetEntry(eBuilding);
		if (pkBuildingInfo)
		{
			CvBuildingEntry& kBuilding = *pkBuildingInfo;
#ifdef NQM_AI_GIMP_NO_WORLD_WONDERS
			if ((GC.getGame().isOption("GAMEOPTION_AI_TWEAKS") || GC.getGame().isOption("GAMEOPTION_AI_GIMP_NO_WORLD_WONDER")) && isWorldWonderClass(kBuilding.GetBuildingClassInfo()))
			{
				continue;
			}
#endif

			if (IsWonder(kBuilding) && HaveCityToBuild((BuildingTypes)iBldgLoop))
			{
				iWeight = m_WonderAIWeights.GetWeight((UnitTypes)iBldgLoop);

				if(pkBuildingInfo->IsDiplomaticVoting())
				{
					int iVotesNeededToWin = GC.getGame().GetVotesNeededForDiploVictory();
					int iSecuredVotes = 0;
					TeamTypes myTeamID = m_pPlayer->getTeam();
					PlayerTypes myPlayerID = m_pPlayer->GetID();


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

					if(!m_pPlayer->GetDiplomacyAI() || !m_pPlayer->GetDiplomacyAI()->IsGoingForDiploVictory() || m_pPlayer->GetTreasury()->GetGold() < iNumberOfPlayersWeNeedToBuyOff * 500 )
					{
						iWeight = 0;
					}
				}

				m_Buildables.push_back(iBldgLoop, iWeight);
			}
		}
	}


	if(m_Buildables.size() > 0)
	{
		m_Buildables.SortItems();
		LogPossibleWonders();

		if(m_Buildables.GetTotalWeight() > 0)
		{
			int iNumChoices = 1;
			eSelection = (BuildingTypes)m_Buildables.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing wonder from Top Choices");
			iWonderWeight = m_Buildables.GetTotalWeight();


			if (pWonderCity->canConstruct(eSelection))
			{
				pCityToBuildAt = pWonderCity;
			}

			else
			{
				CvCity* pLoopCity;
				int iLoop;
				for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
				{
					if(pLoopCity->canConstruct(eSelection))
					{
						pCityToBuildAt = pLoopCity;
						break;
					}
				}
			}

			return eSelection;
		}


		else
		{
			return NO_BUILDING;
		}
	}


	else
	{
		return NO_BUILDING;
	}
}



void CvWonderProductionAI::LogPossibleWonders()
{
	if(GC.getLogging() && GC.getAILogging())
	{

		CvString playerName = m_pPlayer->getCivilizationShortDescription();


		FILogFile* pLog = LOGFILEMGR.GetLog(m_pPlayer->GetCitySpecializationAI()->GetLogFileName(playerName), FILogFile::kDontTimeStamp);


		CvString strBaseString;
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";




		for(int iI = 0; iI < m_Buildables.size(); iI++)
		{
			CvString strOutBuf = strBaseString;

			CvBuildingEntry* pEntry = GC.GetGameBuildings()->GetEntry(m_Buildables.GetElement(iI));
			if(pEntry != NULL)
			{
				CvString strDesc = pEntry->GetDescription();
				CvString strTemp;
				strTemp.Format("Wonder, %s, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI));
				strOutBuf += strTemp;

			}

			pLog->Msg(strOutBuf);
		}
	}
}


void CvWonderProductionAI::LogFlavors(FlavorTypes)
{
}


bool CvWonderProductionAI::IsWonder(const CvBuildingEntry& kBuilding) const
{
	const CvBuildingClassInfo& kBuildingClass = kBuilding.GetBuildingClassInfo();

	if(::isWorldWonderClass(kBuildingClass) ||
	        ::isTeamWonderClass(kBuildingClass) ||
	        ::isNationalWonderClass(kBuildingClass))
	{
		return true;
	}
	return false;
}




bool CvWonderProductionAI::HaveCityToBuild(BuildingTypes eBuilding) const
{
	CvCity* pLoopCity;
	int iLoop;
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity->canConstruct(eBuilding))
		{
			return true;
		}
	}
	return false;
}
