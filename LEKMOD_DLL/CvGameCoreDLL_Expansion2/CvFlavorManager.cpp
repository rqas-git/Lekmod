






#include "CvGameCoreDLLPCH.h"
#include "CvFlavorManager.h"
#include "CvMinorCivAI.h"


#include "LintFree.h"


CvFlavorRecipient::CvFlavorRecipient():
	m_piLatestFlavorValues(NULL)
#ifdef AUI_WARNING_FIXES
	, m_bIsCity(false)
#endif
{

}


CvFlavorRecipient::~CvFlavorRecipient()
{

}


void CvFlavorRecipient::Init()
{
	int iNumFlavors = GC.getNumFlavorTypes();
	m_piLatestFlavorValues = FNEW(int[iNumFlavors], c_eCiv5GameplayDLL, 0);
	memset(m_piLatestFlavorValues, 0, iNumFlavors * sizeof(int));
}


void CvFlavorRecipient::Uninit()
{
	SAFE_DELETE_ARRAY(m_piLatestFlavorValues);
}


bool CvFlavorRecipient::IsCity()
{
	return m_bIsCity;
}


void CvFlavorRecipient::SetFlavors(int* piUpdatedFlavorValues)
{
	CvAssertMsg(piUpdatedFlavorValues != NULL, "Invalid array of flavor deltas passed to flavor recipient");

	if(!piUpdatedFlavorValues) return;

	int iNumFlavors = GC.getNumFlavorTypes();
	for(int iI = 0; iI < iNumFlavors; iI++)
	{
		bool bLogFlavor = false;

		if(m_piLatestFlavorValues[iI] != piUpdatedFlavorValues[iI])
		{
			bLogFlavor = true;
		}

		m_piLatestFlavorValues[iI] = piUpdatedFlavorValues[iI];


	}

	FlavorUpdate();
}


void CvFlavorRecipient::ChangeFlavors(int* piDeltaFlavorValues, bool bDontLog)
{

	CvAssertMsg(piDeltaFlavorValues != NULL, "Invalid array of flavor deltas passed to flavor recipient");

	if(!piDeltaFlavorValues) return;

	int iFlavorMinValue =           GC.getFLAVOR_MIN_VALUE();
	int iFlavorMaxValue =          GC.getFLAVOR_MAX_VALUE();

	int iNumFlavors = GC.getNumFlavorTypes();
	for(int iI = 0; iI < iNumFlavors; iI++)
	{
		if(piDeltaFlavorValues[iI] != 0)
		{
			m_piLatestFlavorValues[iI] += piDeltaFlavorValues[iI];


			if(m_piLatestFlavorValues[iI] < iFlavorMinValue)
			{
				m_piLatestFlavorValues[iI] = iFlavorMinValue;
			}
			else if(m_piLatestFlavorValues[iI] > iFlavorMaxValue)
			{
				m_piLatestFlavorValues[iI] = iFlavorMaxValue;
			}

			if(!bDontLog)
			{
				LogFlavors((FlavorTypes) iI);
			}
		}
	}

	FlavorUpdate();
}


int CvFlavorRecipient::GetLatestFlavorValue(FlavorTypes eFlavor, bool bAllowNegative)
{
	CvAssertMsg(eFlavor > -1, "Out of bounds.");
	CvAssertMsg(eFlavor < GC.getNumFlavorTypes(), "Out of bounds.");

	if(m_piLatestFlavorValues[eFlavor] < 0 && !bAllowNegative)
	{
		return 0;
	}

	return m_piLatestFlavorValues[eFlavor];
}


CvFlavorManager::CvFlavorManager(void):
	m_piPersonalityFlavor(NULL),
	m_piActiveFlavor(NULL)
#ifdef AUI_WARNING_FIXES
	, m_pPlayer(NULL)
#endif
{

}


CvFlavorManager::~CvFlavorManager(void)
{
	Uninit();
}


void CvFlavorManager::Init(CvPlayer* pPlayer)
{
	unsigned int iI;


	m_pPlayer = pPlayer;


#ifdef AUI_WARNING_FIXES
	SAFE_DELETE_ARRAY(m_piPersonalityFlavor);
	SAFE_DELETE_ARRAY(m_piActiveFlavor);
#endif
	m_piPersonalityFlavor = FNEW(int[GC.getNumFlavorTypes()], c_eCiv5GameplayDLL, 0);
	m_piActiveFlavor = FNEW(int[GC.getNumFlavorTypes()], c_eCiv5GameplayDLL, 0);
	m_FlavorTargetList.get_allocator().Reserve((3*64)+100);


	Reset();


	PlayerTypes p = pPlayer->GetID();
	if(p != NO_PLAYER)
	{
		SlotStatus s = CvPreGame::slotStatus(p);
		if((s == SS_TAKEN || s == SS_COMPUTER) && !pPlayer->isBarbarian())
		{

			if(!pPlayer->isHuman())
			{
				LeaderHeadTypes type = pPlayer->getPersonalityType();
				if(type != NO_LEADER)
				{
					CvLeaderHeadInfo* pkLeaderHeadInfo = GC.getLeaderHeadInfo(type);
					if(pkLeaderHeadInfo)
					{
						int iDefaultFlavorValue = GC.getDEFAULT_FLAVOR_VALUE();
						unsigned int uiNumFlavorTypes = GC.getNumFlavorTypes();

						for(iI = 0; iI < uiNumFlavorTypes; iI++)
						{

							if(!pPlayer->isMinorCiv())
							{
								m_piPersonalityFlavor[iI] = pkLeaderHeadInfo->getFlavorValue(iI);
							}

							else
							{
								m_piPersonalityFlavor[iI] = GC.getMinorCivInfo(pPlayer->GetMinorCivAI()->GetMinorCivType())->getFlavorValue(iI);
							}


							if(m_piPersonalityFlavor[iI] == -1)
							{
								m_piPersonalityFlavor[iI] = iDefaultFlavorValue;
							}
						}
					}
				}


				RandomizeWeights();
			}


			else
			{
				int iDefaultFlavorValue = GC.getDEFAULT_FLAVOR_VALUE();
				unsigned int uiNumFlavors = GC.getNumFlavorTypes();
				for(iI = 0; iI < uiNumFlavors; iI++)
				{
					m_piPersonalityFlavor[iI] = iDefaultFlavorValue;
				}
			}


			BroadcastBaseFlavors();

			ResetToBasePersonality();

			if(GC.getLogging() && GC.getAILogging())
			{
				LogFlavors();
			}
		}
	}
}


void CvFlavorManager::Uninit()
{
	SAFE_DELETE_ARRAY(m_piPersonalityFlavor);
	SAFE_DELETE_ARRAY(m_piActiveFlavor);
	m_FlavorTargetList.clear();
}


void CvFlavorManager::Reset()
{
	int iI;

	for(iI = 0; iI < GC.getNumFlavorTypes(); iI++)
	{
		m_piPersonalityFlavor[iI] = 0;
		m_piActiveFlavor[iI] = 0;
	}
}


void CvFlavorManager::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	CvAssertMsg(GC.getNumFlavorTypes() > 0, "Number of flavors to serialize is expected to greater than 0)");

	int iNumFlavors;
	kStream >> iNumFlavors;

	ArrayWrapper<int> wrapm_piPersonalityFlavor(iNumFlavors, m_piPersonalityFlavor);
	kStream >> wrapm_piPersonalityFlavor;

	ArrayWrapper<int> wrapm_piActiveFlavor(iNumFlavors, m_piActiveFlavor);
	kStream >> wrapm_piActiveFlavor;
}


void CvFlavorManager::Write(FDataStream& kStream)
{

	uint uiVersion = 1;
	kStream << uiVersion;

	CvAssertMsg(GC.getNumFlavorTypes() > 0, "Number of flavors to serialize is expected to greater than 0)");
	kStream << GC.getNumFlavorTypes();
	kStream << ArrayWrapper<int>(GC.getNumFlavorTypes(), m_piPersonalityFlavor);
	kStream << ArrayWrapper<int>(GC.getNumFlavorTypes(), m_piActiveFlavor);
}


void CvFlavorManager::AddFlavorRecipient(CvFlavorRecipient* pTargetObject, bool bPropegateFlavorValues)
{

	m_FlavorTargetList.push_back(pTargetObject);


	if(m_piPersonalityFlavor != NULL && bPropegateFlavorValues)
	{
		pTargetObject->SetFlavors(m_piPersonalityFlavor);
	}
}


void CvFlavorManager::RemoveFlavorRecipient(CvFlavorRecipient* pTargetObject)
{
#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (Flavor_List::iterator iter = m_FlavorTargetList.begin(); iter != m_FlavorTargetList.end(); ++iter)
	{
		if (*iter == pTargetObject)
		{
			m_FlavorTargetList.erase(iter);
			return;
		}
#else
	Flavor_List::iterator iter = m_FlavorTargetList.begin();
	Flavor_List::iterator end  = m_FlavorTargetList.end();

	while(iter != end)
	{
		if(*iter == pTargetObject)
		{
			m_FlavorTargetList.erase(iter);
			return;
		}
		iter++;
#endif
	}
}


void CvFlavorManager::ChangeFlavors(int* piDeltaFlavorValues, bool	bPlayerLevelUpdate)
{
	CvAssertMsg(piDeltaFlavorValues != NULL, "Invalid array of flavor deltas passed to flavor manager");

	if(!piDeltaFlavorValues) return;

	if (bPlayerLevelUpdate)
	{
		int iFlavorMinValue =           GC.getFLAVOR_MIN_VALUE();
		int iFlavorMaxValue =          GC.getFLAVOR_MAX_VALUE();

		int iNumFlavors = GC.getNumFlavorTypes();
		for(int iI = 0; iI < iNumFlavors; iI++)
		{
			if(piDeltaFlavorValues[iI] != 0)
			{
				m_piActiveFlavor[iI] += piDeltaFlavorValues[iI];


				if(m_piActiveFlavor[iI] < iFlavorMinValue)
				{
					m_piActiveFlavor[iI] =iFlavorMinValue;
				}
				else if(m_piActiveFlavor[iI] > iFlavorMaxValue)
				{
					m_piActiveFlavor[iI] = iFlavorMaxValue;
				}

				LogFlavors((FlavorTypes) iI);
			}
		}
	}

	BroadcastFlavors(piDeltaFlavorValues, bPlayerLevelUpdate);
}


void CvFlavorManager::ResetToBasePersonality()
{
	int iI;

	for(iI = 0; iI < GC.getNumFlavorTypes(); iI++)
	{
		m_piActiveFlavor[iI] = m_piPersonalityFlavor[iI];
	}

	BroadcastBaseFlavors();
}


void CvFlavorManager::AdjustWeightsForMap()
{
	int iTotalLandTiles;
	int iNumPlayers;
	double iTilesPerPlayer;
	double fAdjust;
	int iAdjust;

	iTotalLandTiles = GC.getMap().getLandPlots();
	iNumPlayers = GC.getGame().countMajorCivsAlive();

	if(iNumPlayers > 0)
	{
		int iNumFlavorTypes = GC.getNumFlavorTypes();

		iTilesPerPlayer = (double)iTotalLandTiles / (double)iNumPlayers;






		fAdjust = log10(iTilesPerPlayer) - GC.getFLAVOR_STANDARD_LOG10_TILES_PER_PLAYER();
		fAdjust *= (double)GC.getFLAVOR_EXPANDGROW_COEFFICIENT();
		iAdjust = (int)fAdjust;

		int iFlavorMaxValue =        GC.getPERSONALITY_FLAVOR_MAX_VALUE();
		int iFlavorMinValue =       GC.getPERSONALITY_FLAVOR_MIN_VALUE();

		int iExpansionIndex = GC.getInfoTypeForString("FLAVOR_EXPANSION");
		int iGrowthIndex = GC.getInfoTypeForString("FLAVOR_GROWTH");


		CvAssert(iExpansionIndex >= 0 && iExpansionIndex < iNumFlavorTypes);
		if (iExpansionIndex >= 0 && iExpansionIndex < iNumFlavorTypes)
		{
			m_piPersonalityFlavor[iExpansionIndex] += iAdjust;
			if(m_piPersonalityFlavor[iExpansionIndex] > iFlavorMaxValue)
			{
				m_piPersonalityFlavor[iExpansionIndex] = iFlavorMaxValue;
			}
		}


		CvAssert(iGrowthIndex >= 0 && iGrowthIndex < iNumFlavorTypes);
		if (iGrowthIndex >= 0 && iGrowthIndex < iNumFlavorTypes)
		{
			m_piPersonalityFlavor[iGrowthIndex] -= iAdjust;
			if(m_piPersonalityFlavor[iGrowthIndex] < iFlavorMinValue)
			{
				m_piPersonalityFlavor[iGrowthIndex] = iFlavorMinValue;
			}
		}


		ResetToBasePersonality();
	}
}


int CvFlavorManager::GetIndividualFlavor(FlavorTypes eType)
{
	CvAssert((int)eType >= 0 && (int)eType < GC.getNumFlavorTypes());
	return m_piActiveFlavor[eType];
}


int* CvFlavorManager::GetAllFlavors()
{
	return m_piActiveFlavor;
}


int CvFlavorManager::GetPersonalityIndividualFlavor(FlavorTypes eType)
{
	CvAssert((int)eType >= 0 && (int)eType < GC.getNumFlavorTypes());
	return m_piPersonalityFlavor[eType];
}


int* CvFlavorManager::GetAllPersonalityFlavors()
{
	return m_piPersonalityFlavor;
}




void CvFlavorManager::RandomizeWeights()
{
	int iI;
	int iMin, iMax, iPlusMinus;

	iMin =       GC.getPERSONALITY_FLAVOR_MIN_VALUE();
	iMax =        GC.getPERSONALITY_FLAVOR_MAX_VALUE();
	iPlusMinus =       GC.getFLAVOR_RANDOMIZATION_RANGE();

	for(iI = 0; iI < GC.getNumFlavorTypes(); iI++)
	{

#ifdef AUI_FLAVOR_MANAGER_FIX_RANDOMIZE_WEIGHTS_ZEROED_OUT_FLAVOR
		if (m_piPersonalityFlavor[iI] >= 0)
#else
		if(m_piPersonalityFlavor[iI] != 0)
#endif
		{
			m_piPersonalityFlavor[iI] = GetAdjustedValue(m_piPersonalityFlavor[iI], iPlusMinus, iMin, iMax);
		}
	}
}

#ifdef AUI_FLAVOR_MANAGER_GET_ADJUSTED_VALUE_USES_BINOM_RNG

#else

#endif
int CvFlavorManager::GetAdjustedValue(int iOriginalValue, int iPlusMinus, int iMin, int iMax)
{
	int iAdjust;
	int iRtnValue;

#ifdef AUI_FLAVOR_MANAGER_GET_ADJUSTED_VALUE_USE_REROLLS
	int iReroll = 0;
#endif
#ifdef AUI_FLAVOR_MANAGER_FIX_GET_ADJUSTED_VALUE_NEGATIVE_PLUSMINUS
	iPlusMinus = abs(iPlusMinus);
#endif

#ifdef AUI_FLAVOR_MANAGER_GET_ADJUSTED_VALUE_USES_BINOM_RNG
	iAdjust = GC.getGame().getJonRandNumBinom((iPlusMinus * 2 + 1), "Adjusting Personality Flavor");
#else
	iAdjust = GC.getGame().getJonRandNum((iPlusMinus * 2 + 1), "Adjusting Personality Flavor");
#endif
	iRtnValue = iOriginalValue + iAdjust - iPlusMinus;

	if(iRtnValue < iMin)
#ifdef AUI_FLAVOR_MANAGER_GET_ADJUSTED_VALUE_USE_REROLLS
	{
		iReroll = iMin - iRtnValue;
#endif
		iRtnValue = iMin;
#ifdef AUI_FLAVOR_MANAGER_GET_ADJUSTED_VALUE_USE_REROLLS
}
#endif
	else if(iRtnValue > iMax)
#ifdef AUI_FLAVOR_MANAGER_GET_ADJUSTED_VALUE_USE_REROLLS
	{
		iReroll = iMax - iRtnValue;
#endif
		iRtnValue = iMax;
#ifdef AUI_FLAVOR_MANAGER_GET_ADJUSTED_VALUE_USE_REROLLS
	}

	if (iReroll != 0)
	{
		iRtnValue = GetAdjustedValue(iRtnValue, iReroll, iMin, iMax);
	}
#endif

	return iRtnValue;
}


void CvFlavorManager::BroadcastFlavors(int* piDeltaFlavorValues, bool bPlayerLevelUpdate)
{
	Flavor_List::iterator it;

#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_FlavorTargetList.begin(); it != m_FlavorTargetList.end(); ++it)
#else
	for(it = m_FlavorTargetList.begin(); it != m_FlavorTargetList.end(); it++)
#endif
	{
		if(bPlayerLevelUpdate && !(*it)->IsCity())
		{
			(*it)->ChangeFlavors(piDeltaFlavorValues);
		}
		else if (!bPlayerLevelUpdate && (*it)->IsCity())
		{
			(*it)->ChangeFlavors(piDeltaFlavorValues);
		}
	}
}


void CvFlavorManager::BroadcastBaseFlavors()
{
	Flavor_List::iterator it;

#ifdef AUI_ITERATOR_POSTFIX_INCREMENT_OPTIMIZATIONS
	for (it = m_FlavorTargetList.begin(); it != m_FlavorTargetList.end(); ++it)
#else
	for(it = m_FlavorTargetList.begin(); it != m_FlavorTargetList.end(); it++)
#endif
	{
		(*it)->SetFlavors(m_piPersonalityFlavor);
	}
}

void CvFlavorManager::LogFlavors(FlavorTypes eFlavor)
{
	CvString strOutBuf;
	CvString strBaseString;
	CvString strTemp;
	CvString playerName;
	CvString strDesc;
	CvString strLogName;

	if(GC.getLogging() && GC.getAILogging())
	{

		playerName = m_pPlayer->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "FlavorAILog_" + playerName + ".csv";
		}
		else
		{
			strLogName = "FlavorAILog.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";


		if(eFlavor == NO_FLAVOR)
		{
			for(int iI = 0; iI < GC.getNumFlavorTypes(); iI++)
			{


				{
					strTemp.Format("Flavor, %s, %d", GC.getFlavorTypes((FlavorTypes)iI).GetCString(), GetIndividualFlavor((FlavorTypes) iI));
					strOutBuf = strBaseString + strTemp;
					pLog->Msg(strOutBuf);
				}
			}
		}
		else
		{
			strTemp.Format("Flavor, %s, %d", GC.getFlavorTypes(eFlavor).GetCString(), GetIndividualFlavor(eFlavor));
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}
