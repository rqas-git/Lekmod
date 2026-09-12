






#include "CvGameCoreDLLPCH.h"
#include "CvProjectProductionAI.h"


#include "LintFree.h"


CvProjectProductionAI::CvProjectProductionAI(CvCity* pCity):
	m_pCity(pCity)
{
}


CvProjectProductionAI::~CvProjectProductionAI(void)
{
}


void CvProjectProductionAI::Reset()
{
	m_ProjectAIWeights.clear();


#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < GC.GetGameProjects()->GetNumProjects(); i++)
#else
	for(int i = 0; i < GC.GetGameProjects()->GetNumProjects(); i++)
#endif
	{
		m_ProjectAIWeights.push_back(i, 0);
	}
}


void CvProjectProductionAI::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	int iWeight;


	m_ProjectAIWeights.clear();


#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < GC.GetGameProjects()->GetNumProjects(); i++)
#else
	for(int i = 0; i < GC.GetGameProjects()->GetNumProjects(); i++)
#endif
	{
		kStream >> iWeight;
		m_ProjectAIWeights.push_back(i, iWeight);
	}
}


void CvProjectProductionAI::Write(FDataStream& kStream) const
{

	uint uiVersion = 1;
	kStream << uiVersion;


#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < GC.GetGameProjects()->GetNumProjects(); i++)
#else
	for(int i = 0; i < GC.GetGameProjects()->GetNumProjects(); i++)
#endif
	{
		kStream << m_ProjectAIWeights.GetWeight(i);
	}
}


void CvProjectProductionAI::AddFlavorWeights(FlavorTypes eFlavor, int iWeight)
{
#ifdef AUI_WARNING_FIXES
	uint iProject;
#else
	int iProject;
#endif
	CvProjectEntry* entry(NULL);


	for(iProject = 0; iProject < GC.GetGameProjects()->GetNumProjects(); iProject++)
	{
		entry = GC.GetGameProjects()->GetEntry(iProject);
		if(entry)
		{

			m_ProjectAIWeights.IncreaseWeight(iProject, entry->GetFlavorValue(eFlavor) * iWeight);
		}
	}
}


int CvProjectProductionAI::GetWeight(ProjectTypes eProject)
{
	return m_ProjectAIWeights.GetWeight(eProject);
}


ProjectTypes CvProjectProductionAI::RecommendProject()
{
	CvAssert(m_pCity);
	if(!m_pCity)
		return NO_PROJECT;

#ifdef AUI_WARNING_FIXES
	uint iProjectLoop;
#else
	int iProjectLoop;
#endif
	int iWeight;
	int iTurnsLeft;


	m_Buildables.clear();


	for(iProjectLoop = 0; iProjectLoop < GC.GetGameProjects()->GetNumProjects(); iProjectLoop++)
	{

		if(m_pCity->canCreate((ProjectTypes)iProjectLoop))
		{

			iTurnsLeft = m_pCity->getProductionTurnsLeft((ProjectTypes) iProjectLoop, 0);
			iWeight = CityStrategyAIHelpers::ReweightByTurnsLeft(m_ProjectAIWeights.GetWeight((ProjectTypes)iProjectLoop), iTurnsLeft);
			m_Buildables.push_back(iProjectLoop, iWeight);
		}
	}


	if(m_Buildables.size() > 0)
	{
		m_Buildables.SortItems();
		LogPossibleBuilds();
		return (ProjectTypes)m_Buildables.GetElement(0);
	}


	else
	{
		return NO_PROJECT;
	}
}


void CvProjectProductionAI::LogPossibleBuilds()
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
			CvProjectEntry* pProjectEntry = GC.GetGameProjects()->GetEntry(m_Buildables.GetElement(iI));

			strDesc = (pProjectEntry != NULL)? pProjectEntry->GetDescription() : "Unknown";
			strTemp.Format("Project, %s, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI));
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}
