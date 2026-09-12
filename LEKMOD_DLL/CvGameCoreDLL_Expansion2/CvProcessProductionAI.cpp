






#include "CvGameCoreDLLPCH.h"
#include "CvProcessProductionAI.h"
#include "CvInfosSerializationHelper.h"


#include "LintFree.h"


CvProcessProductionAI::CvProcessProductionAI(CvCity* pCity):
	m_pCity(pCity)
{
}


CvProcessProductionAI::~CvProcessProductionAI(void)
{
}


void CvProcessProductionAI::Reset()
{
	m_ProcessAIWeights.clear();


#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < GC.getNumProcessInfos(); i++)
#else
	for (int i = 0; i < GC.getNumProcessInfos(); i++)
#endif
	{
		m_ProcessAIWeights.push_back(i, 0);
	}
}


void CvProcessProductionAI::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	int iWeight;


	m_ProcessAIWeights.clear();
	m_ProcessAIWeights.resize(GC.getNumProcessInfos());
#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < GC.getNumProcessInfos(); i++)
#else
	for(int i = 0; i < GC.getNumProcessInfos(); ++i)
#endif
		m_ProcessAIWeights.SetWeight(i, 0);


	int iNumProcess;
	kStream >> iNumProcess;
	for(int i = 0; i < iNumProcess; i++)
	{
		int iType = CvInfosSerializationHelper::ReadHashed(kStream);
		kStream >> iWeight;
		if (iType >= 0 && iType < m_ProcessAIWeights.size())
			m_ProcessAIWeights.SetWeight(iType, iWeight);
	}
}


void CvProcessProductionAI::Write(FDataStream& kStream) const
{

	uint uiVersion = 1;
	kStream << uiVersion;


	kStream << GC.getNumProcessInfos();
#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < GC.getNumProcessInfos(); i++)
#else
	for(int i = 0; i < GC.getNumProcessInfos(); i++)
#endif
	{
		CvInfosSerializationHelper::WriteHashed(kStream, GC.getProcessInfo((ProcessTypes)i));
		kStream << m_ProcessAIWeights.GetWeight(i);
	}
}


void CvProcessProductionAI::AddFlavorWeights(FlavorTypes eFlavor, int iWeight)
{
#ifdef AUI_WARNING_FIXES
	uint iProcess;
#else
	int iProcess;
#endif
	CvProcessInfo* entry(NULL);


	for(iProcess = 0; iProcess < GC.getNumProcessInfos(); iProcess++)
	{
		entry = GC.getProcessInfo((ProcessTypes)iProcess);
		if (entry)
		{

			m_ProcessAIWeights.IncreaseWeight(iProcess, entry->GetFlavorValue(eFlavor) * iWeight);
		}
	}
}


int CvProcessProductionAI::GetWeight(ProcessTypes eProject)
{
	return m_ProcessAIWeights.GetWeight(eProject);
}



void CvProcessProductionAI::LogPossibleBuilds()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString cityName;
		CvString strDesc;
		CvString strLogName;

		CvAssert(m_pCity);
		if(!m_pCity) return;


		playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
		cityName = m_pCity->getName();


		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(m_pCity->GetCityStrategyAI()->GetLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);
		CvAssert(pLog);
		if(!pLog) return;


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", ";


		for(int iI = 0; iI < m_Buildables.size(); iI++)
		{
			CvProcessInfo* pProcessInfo = GC.getProcessInfo((ProcessTypes)m_Buildables.GetElement(iI));
			strDesc = (pProcessInfo != NULL)? pProcessInfo->GetDescription() : "Unknown";
			strTemp.Format("Process, %s, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI));
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}
