






#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "CvTechAI.h"
#include "CvGameCoreUtils.h"
#include "CvInfosSerializationHelper.h"

#include "LintFree.h"


CvTechAI::CvTechAI(CvPlayerTechs* currentTechs):
	m_pCurrentTechs(currentTechs)
{
}


CvTechAI::~CvTechAI(void)
{
}


void CvTechAI::Reset()
{
	CvAssertMsg(m_pCurrentTechs != NULL, "Tech AI init failure: player tech data is NULL");
	if(m_pCurrentTechs != NULL)
	{
		CvAssertMsg(m_pCurrentTechs->GetTechs() != NULL, "Tech AI init failure: no tech data");

		m_TechAIWeights.clear();


#ifdef AUI_WARNING_FIXES
		for (uint i = 0; i < m_pCurrentTechs->GetTechs()->GetNumTechs(); i++)
#else
		for(int i = 0; i < m_pCurrentTechs->GetTechs()->GetNumTechs(); i++)
#endif
		{
			m_TechAIWeights.push_back(i, 0);
		}
	}
}

extern const char* ms_V0ExpansionTechTags[81];

void CvTechAI::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	int iWeight;

	CvAssertMsg(m_pCurrentTechs->GetTechs() != NULL, "Tech AI serialization failure: no tech data");
	CvAssertMsg(m_pCurrentTechs->GetTechs()->GetNumTechs() > 0, "Tech AI serialization failure: number of techs not greater than 0");


	m_TechAIWeights.clear();
	int iTechCount = m_pCurrentTechs->GetTechs()->GetNumTechs();
	m_TechAIWeights.resize(iTechCount);
	for(int i = 0; i < iTechCount; ++i)
		m_TechAIWeights.SetWeight(i, 0);

	int iCount;
	kStream >> iCount;

	for(int i = 0; i < iCount; i++)
	{
		int iIndex = CvInfosSerializationHelper::ReadHashed(kStream);
		kStream >> iWeight;
		if(iIndex >= 0 && iIndex < iCount)
			m_TechAIWeights.SetWeight(iIndex, iWeight);
	}
}


void CvTechAI::Write(FDataStream& kStream) const
{

	uint uiVersion = 1;
	kStream << uiVersion;

	CvAssertMsg(m_pCurrentTechs->GetTechs() != NULL, "Tech AI serialization failure: no tech data");
	CvAssertMsg(m_pCurrentTechs->GetTechs()->GetNumTechs() > 0, "Tech AI serialization failure: number of techs not greater than 0");

	uint uiCount = m_pCurrentTechs->GetTechs()->GetNumTechs();
	kStream << uiCount;


	for(uint i = 0; i < uiCount; i++)
	{
		CvInfosSerializationHelper::WriteHashed(kStream, (TechTypes)i);
		kStream << m_TechAIWeights.GetWeight(i);
	}
}


void CvTechAI::AddFlavorWeights(FlavorTypes eFlavor, int iWeight, int iPropagationPercent)
{
#ifdef AUI_WARNING_FIXES
	FFastVector<int, true> paiTempWeights;
	paiTempWeights.reserve(m_pCurrentTechs->GetTechs()->GetNumTechs());
#else
	int* paiTempWeights;


	paiTempWeights = (int*)_alloca(sizeof(int) * m_pCurrentTechs->GetTechs()->GetNumTechs());
#endif


#ifdef AUI_WARNING_FIXES
	for (uint iTech = 0; iTech < m_pCurrentTechs->GetTechs()->GetNumTechs(); iTech++)
#else
	for(int iTech = 0; iTech < m_pCurrentTechs->GetTechs()->GetNumTechs(); iTech++)
#endif
	{
		const TechTypes eTech = static_cast<TechTypes>(iTech);
		CvTechEntry* entry = m_pCurrentTechs->GetTechs()->GetEntry(iTech);
		if (entry)
		{
#ifdef AUI_WARNING_FIXES

			int iLoopWeight = entry->GetFlavorValue(eFlavor) * iWeight;


			iLoopWeight *= m_pCurrentTechs->GetPlayer()->GetPlayerTechs()->GetCivTechPriority(eTech);


			iLoopWeight *= m_pCurrentTechs->GetPlayer()->GetPlayerTechs()->GetLocaleTechPriority(eTech);

			paiTempWeights.push_back(iLoopWeight);
		}
		else
			paiTempWeights.push_back(0);
#else

			paiTempWeights[iTech] = entry->GetFlavorValue(eFlavor) * iWeight;


			paiTempWeights[iTech] *= m_pCurrentTechs->GetPlayer()->GetPlayerTechs()->GetCivTechPriority(eTech);


			paiTempWeights[iTech] *= m_pCurrentTechs->GetPlayer()->GetPlayerTechs()->GetLocaleTechPriority(eTech);
		}
#endif
	}


	WeightPrereqs(paiTempWeights, iPropagationPercent);


#ifdef AUI_WARNING_FIXES
	for (uint iTech = 0; iTech < m_pCurrentTechs->GetTechs()->GetNumTechs(); iTech++)
#else
	for(int iTech = 0; iTech < m_pCurrentTechs->GetTechs()->GetNumTechs(); iTech++)
#endif
	{
		CvTechEntry* entry = m_pCurrentTechs->GetTechs()->GetEntry(iTech);
		if(entry)
		{
			m_TechAIWeights.IncreaseWeight(iTech, paiTempWeights[iTech]);
		}
	}
}



TechTypes CvTechAI::ChooseNextTech(CvPlayer *pPlayer, bool bFreeTech)
{
	RandomNumberDelegate fcn;
	TechTypes rtnValue = NO_TECH;
#ifdef AUI_WARNING_FIXES
	uint iTechLoop;
#else
	int iTechLoop;
#endif




	fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);


	m_ResearchableTechs.clear();


	for(iTechLoop = 0; iTechLoop < m_pCurrentTechs->GetTechs()->GetNumTechs(); iTechLoop++)
	{
		if(m_pCurrentTechs->CanResearch((TechTypes)iTechLoop))
		{

			if(bFreeTech)
			{
				if(m_pCurrentTechs->CanResearchForFree((TechTypes)iTechLoop))
				{
					m_ResearchableTechs.push_back(iTechLoop, m_TechAIWeights.GetWeight(iTechLoop));
				}
			}
			else
			{
				m_ResearchableTechs.push_back(iTechLoop, m_TechAIWeights.GetWeight(iTechLoop));
			}
		}
	}


	if(!bFreeTech)
		ReweightByCost(pPlayer);

	m_ResearchableTechs.SortItems();
	LogPossibleResearch();


	if(m_ResearchableTechs.GetTotalWeight() > 0)
	{
		int iNumChoices =GC.getGame().getHandicapInfo().GetTechNumOptions();
		rtnValue = (TechTypes) m_ResearchableTechs.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing tech from Top Choices");
		LogResearchChoice(rtnValue);
	}

	return rtnValue;
}


TechTypes CvTechAI::RecommendNextTech(CvPlayer *pPlayer, TechTypes eIgnoreTech                )
{
	TechTypes rtnValue = NO_TECH;
#ifdef AUI_WARNING_FIXES
	uint iTechLoop;
#else
	int iTechLoop;
#endif


	m_ResearchableTechs.clear();


	for(iTechLoop = 0; iTechLoop < m_pCurrentTechs->GetTechs()->GetNumTechs(); iTechLoop++)
	{



#ifdef AUI_WARNING_FIXES
		if (m_pCurrentTechs->CanResearch((TechTypes)iTechLoop) && (TechTypes)iTechLoop != eIgnoreTech)
#else
		if(m_pCurrentTechs->CanResearch((TechTypes) iTechLoop) && iTechLoop != eIgnoreTech)
#endif
		{
			m_ResearchableTechs.push_back(iTechLoop, m_TechAIWeights.GetWeight(iTechLoop));
		}
	}

	ReweightByCost(pPlayer);
	m_ResearchableTechs.SortItems();
	LogPossibleResearch();


	if(m_ResearchableTechs.GetTotalWeight() > 0)
	{

		rtnValue = (TechTypes) m_ResearchableTechs.GetElement(0);
		LogResearchChoice(rtnValue);
	}

	return rtnValue;
}


int CvTechAI::GetWeight(TechTypes eTech)
{
	return m_TechAIWeights.GetWeight(eTech);
}

struct LeaderWithNumTechs
{
	int m_iLeader;
	int m_iNumTechs;
};

struct LeaderWithNumTechsEval
{
	bool operator()(LeaderWithNumTechs const& a, LeaderWithNumTechs const& b) const
	{
		return a.m_iNumTechs > b.m_iNumTechs;
	}
};

float CvTechAI::GetTechRatio()
{

	std::vector<LeaderWithNumTechs> aLeaderWithNumTechs;
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		PlayerTypes ePlayer = (PlayerTypes)ui;
		if(!GET_PLAYER(ePlayer).isAlive())
		{
			continue;
		}

		LeaderWithNumTechs kEvalChunk;
		kEvalChunk.m_iLeader = ui;
		kEvalChunk.m_iNumTechs = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetTeamTechs()->GetNumTechsKnown();
		aLeaderWithNumTechs.push_back(kEvalChunk);
	}

	std::stable_sort(aLeaderWithNumTechs.begin(), aLeaderWithNumTechs.end(), LeaderWithNumTechsEval());

	int iPlayerIndexInList = -1;
	for(uint ui = 0; ui < aLeaderWithNumTechs.size(); ui++)
	{
		if(aLeaderWithNumTechs[ui].m_iLeader == m_pCurrentTechs->GetPlayer()->GetID())
		{
			iPlayerIndexInList = ui;
			break;
		}
	}

	CvAssertMsg(iPlayerIndexInList != -1, "Could not find player in list");
	CvAssertMsg(aLeaderWithNumTechs.size() >= 2, "Only one player in the game? Huh?");
	float fTechPositionRatio = 0.0f;
	if(aLeaderWithNumTechs.size() >= 2)
	{
		int iNumerator = iPlayerIndexInList;
		int iDenominator = aLeaderWithNumTechs.size() - 1;
		fTechPositionRatio = iNumerator / (float)iDenominator;
	}

	return fTechPositionRatio;
}





#ifdef AUI_WARNING_FIXES
void CvTechAI::WeightPrereqs(FFastVector<int, true> paiTempWeights, int iPropagationPercent)
{
	uint iTechLoop;
#else
void CvTechAI::WeightPrereqs(int* paiTempWeights, int iPropagationPercent)
{
	int iTechLoop;
#endif


	for(iTechLoop = 0; iTechLoop < m_pCurrentTechs->GetTechs()->GetNumTechs(); iTechLoop++)
	{

		if(paiTempWeights[iTechLoop] > 0)
		{
			PropagateWeights(iTechLoop, paiTempWeights[iTechLoop], iPropagationPercent, 0);
		}
	}
}


void CvTechAI::PropagateWeights(int iTech, int iWeight, int iPropagationPercent, int iPropagationLevel)
{
	CvTechEntry* pkTechInfo = m_pCurrentTechs->GetTechs()->GetEntry(iTech);
	if(pkTechInfo)
	{
		if(iPropagationLevel < GC.getTECH_WEIGHT_PROPAGATION_LEVELS())
		{
			int iPropagatedWeight = iWeight * iPropagationPercent / 100;


			for(int iI = 0; iI < GC.getNUM_OR_TECH_PREREQS(); iI++)
			{

				int iPrereq = pkTechInfo->GetPrereqAndTechs(iI);
				if(iPrereq != NO_TECH)
				{



					m_TechAIWeights.IncreaseWeight(iPrereq, iPropagatedWeight);


					if(iPropagatedWeight > 0)
					{
						PropagateWeights(iPrereq, iPropagatedWeight, iPropagationPercent, iPropagationLevel++);
					}
				}
				else
				{
					break;
				}

			}
		}
	}
}


void CvTechAI::ReweightByCost(CvPlayer *pPlayer)
{
	TechTypes eTech;


	bool bNeedExpensiveTechs = pPlayer->getOverflowResearchTimes100() > (pPlayer->GetScienceTimes100() * 2);

	for(int iI = 0; iI < m_ResearchableTechs.size(); iI++)
	{
		eTech = (TechTypes) m_ResearchableTechs.GetElement(iI);
		int iTurnsLeft = 0;

		iTurnsLeft = m_pCurrentTechs->GetResearchTurnsLeft(eTech, true);

		double fWeightDivisor;


		double fAdditionalTurnCostFactor = GC.getAI_RESEARCH_WEIGHT_MOD_PER_TURN_LEFT() * iTurnsLeft;
		double fTotalCostFactor = GC.getAI_RESEARCH_WEIGHT_BASE_MOD() + fAdditionalTurnCostFactor;

		fWeightDivisor = pow((double) iTurnsLeft, fTotalCostFactor);

		int iNewWeight;
		if (bNeedExpensiveTechs)
		{
			iNewWeight = int(double(m_ResearchableTechs.GetWeight(iI)) * fWeightDivisor);
		}
		else
		{
			iNewWeight = int(double(m_ResearchableTechs.GetWeight(iI)) / fWeightDivisor);
		}


		m_ResearchableTechs.SetWeight(iI, iNewWeight);
	}
}


void CvTechAI::LogPossibleResearch()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strDesc;


		playerName = m_pCurrentTechs->GetPlayer()->getCivilizationShortDescription();

		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";


		for(int iI = 0; iI < m_ResearchableTechs.size(); iI++)
		{
			TechTypes eTech = (TechTypes) m_ResearchableTechs.GetElement(iI);
			int iWeight = m_ResearchableTechs.GetWeight(iI);

			CvTechEntry* pTechEntry = GC.getTechInfo(eTech);
			const char* szTechType = (pTechEntry != NULL)? pTechEntry->GetType() : "Unknown Tech";

			strTemp.Format("%s, %d", szTechType, iWeight);

			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}


void CvTechAI::LogResearchChoice(TechTypes eTech)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString playerName;
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString strDesc;


		playerName = m_pCurrentTechs->GetPlayer()->getCivilizationShortDescription();

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		CvTechEntry* pTechEntry = GC.getTechInfo(eTech);
		const char* szTechType = (pTechEntry != NULL)? pTechEntry->GetType() : "Unknown Tech";

		strTemp.Format("CHOSEN, %s", szTechType);

		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}
}


CvString CvTechAI::GetLogFileName(CvString& playerName) const
{
	CvString strLogName;


	if(GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "TechAILog_" + playerName + ".csv";
	}
	else
	{
		strLogName = "TechAILog.csv";
	}

	return strLogName;
}
