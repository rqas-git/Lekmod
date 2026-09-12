






#include "CvGameCoreDLLPCH.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"


#include "LintFree.h"

CvProjectEntry::CvProjectEntry(void):
	m_piResourceQuantityRequirements(NULL),
	m_piVictoryThreshold(NULL),
	m_piVictoryMinThreshold(NULL),
	m_piProjectsNeeded(NULL),
	m_piFlavorValue(NULL)
{
}

CvProjectEntry::~CvProjectEntry(void)
{
	SAFE_DELETE_ARRAY(m_piResourceQuantityRequirements);
	SAFE_DELETE_ARRAY(m_piVictoryThreshold);
	SAFE_DELETE_ARRAY(m_piVictoryMinThreshold);
	SAFE_DELETE_ARRAY(m_piProjectsNeeded);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
}

bool CvProjectEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	m_iMaxGlobalInstances = kResults.GetInt("MaxGlobalInstances");
	m_iMaxTeamInstances = kResults.GetInt("MaxTeamInstances");
	m_iProductionCost = kResults.GetInt("Cost");
	m_iNukeInterception = kResults.GetInt("NukeInterception");
	m_iCultureBranchesRequired = kResults.GetInt("CultureBranchesRequired");
	m_iTechShare = kResults.GetInt("TechShare");
	m_iVictoryDelayPercent = kResults.GetInt("VictoryDelayPercent");

	m_bSpaceship = kResults.GetBool("Spaceship");
	m_bAllowsNukes = kResults.GetBool("AllowsNukes");

	m_strMovieArtDef = kResults.GetText("MovieDefineTag");

	const char* szVictoryPrereq = kResults.GetText("VictoryPrereq");
	m_iVictoryPrereq = GC.getInfoTypeForString(szVictoryPrereq, true);

	const char* szTechPrereq = kResults.GetText("TechPrereq");
	m_iTechPrereq = GC.getInfoTypeForString(szTechPrereq, true);

	const char* szEveryoneSpecialUnit = kResults.GetText("EveryoneSpecialUnit");
	m_iEveryoneSpecialUnit = GC.getInfoTypeForString(szEveryoneSpecialUnit, true);

	const char* szCreateSound = kResults.GetText("CreateSound");
	SetCreateSound(szCreateSound);

	const char* szAnyonePrereqProject = kResults.GetText("AnyonePrereqProject");
	m_iAnyoneProjectPrereq = GC.getInfoTypeForString(szAnyonePrereqProject, true);


	const char* szProjectType = GetType();
	kUtility.PopulateArrayByValue(m_piResourceQuantityRequirements, "Resources", "Project_ResourceQuantityRequirements", "ResourceType", "ProjectType", szProjectType, "Quantity");


	{
		const int iNumVictories = kUtility.MaxRows("Victories");

		kUtility.InitializeArray(m_piVictoryThreshold, iNumVictories);
		kUtility.InitializeArray(m_piVictoryMinThreshold, iNumVictories);

		Database::Results kDBResults;
		char szQuery[512] = {0};
		sprintf_s(szQuery, "select VictoryType, Threshold, MinThreshold from Project_VictoryThresholds where ProjectType = '%s';", szProjectType);
		if(DB.Execute(kDBResults, szQuery))
		{
			while(kDBResults.Step())
			{
				const char* szVictoryType = kDBResults.GetText("VictoryType");
				const int idx = GC.getInfoTypeForString(szVictoryType, true);

				const int iThreshold = kDBResults.GetInt("Threshold");
				const int iMinThreshold = kDBResults.GetInt("MinThreshold");

				m_piVictoryThreshold[idx] = iThreshold;
				m_piVictoryMinThreshold[idx] = iMinThreshold;
			}
		}
	}

	kUtility.SetFlavors(m_piFlavorValue, "Project_Flavors", "ProjectType", szProjectType);
	kUtility.PopulateArrayByValue(m_piProjectsNeeded, "Projects", "Project_Prereqs", "PrereqProjectType", "ProjectType", szProjectType, "AmountNeeded");

	return true;
}


int CvProjectEntry::GetVictoryPrereq() const
{
	return m_iVictoryPrereq;
}


int CvProjectEntry::GetTechPrereq() const
{
	return m_iTechPrereq;
}


int CvProjectEntry::GetAnyoneProjectPrereq() const
{
	return m_iAnyoneProjectPrereq;
}


void CvProjectEntry::SetAnyoneProjectPrereq(int i)
{
	m_iAnyoneProjectPrereq = i;
}


int CvProjectEntry::GetMaxGlobalInstances() const
{
	return m_iMaxGlobalInstances;
}


int CvProjectEntry::GetMaxTeamInstances() const
{
	return m_iMaxTeamInstances;
}


int CvProjectEntry::GetProductionCost() const
{
	return m_iProductionCost;
}


int CvProjectEntry::GetNukeInterception() const
{
	return m_iNukeInterception;
}


int CvProjectEntry::GetCultureBranchesRequired() const
{
	return m_iCultureBranchesRequired;
}


int CvProjectEntry::GetTechShare() const
{
	return m_iTechShare;
}


int CvProjectEntry::GetEveryoneSpecialUnit() const
{
	return m_iEveryoneSpecialUnit;
}


int CvProjectEntry::GetVictoryDelayPercent() const
{
	return m_iVictoryDelayPercent;
}


int CvProjectEntry::GetFlavorValue(int i) const
{
	CvAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumFlavorTypes() && m_piFlavorValue)
	{
		return  m_piFlavorValue[i];
	}

	return 0;
}


bool CvProjectEntry::IsSpaceship() const
{
	return m_bSpaceship;
}


bool CvProjectEntry::IsAllowsNukes() const
{
	return m_bAllowsNukes;
}


const char* CvProjectEntry::GetMovieArtDef() const
{
	return m_strMovieArtDef;
}


const char* CvProjectEntry::GetCreateSound() const
{
	return m_strCreateSound;
}


void CvProjectEntry::SetCreateSound(const char* szVal)
{
	m_strCreateSound = szVal;
}




#ifdef AUI_WARNING_FIXES
int CvProjectEntry::GetResourceQuantityRequirement(uint i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");

	if (i < GC.getNumResourceInfos() && m_piResourceQuantityRequirements)
#else
int CvProjectEntry::GetResourceQuantityRequirement(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumResourceInfos() && m_piResourceQuantityRequirements)
#endif
	{
		return  m_piResourceQuantityRequirements[i];
	}

	return -1;
}


#ifdef AUI_WARNING_FIXES
int CvProjectEntry::GetVictoryThreshold(uint i) const
{
	CvAssertMsg(i < GC.getNumVictoryInfos(), "Index out of bounds");

	if (i < GC.getNumVictoryInfos() && m_piVictoryThreshold)
#else
int CvProjectEntry::GetVictoryThreshold(int i) const
{
	CvAssertMsg(i < GC.getNumVictoryInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumVictoryInfos() && m_piVictoryThreshold)
#endif
	{
		return  m_piVictoryThreshold[i];
	}

	return -1;
}


#ifdef AUI_WARNING_FIXES
int CvProjectEntry::GetVictoryMinThreshold(uint i) const
{
	CvAssertMsg(i < GC.getNumVictoryInfos(), "Index out of bounds");

	if (i < GC.getNumVictoryInfos())
#else
int CvProjectEntry::GetVictoryMinThreshold(int i) const
{
	CvAssertMsg(i < GC.getNumVictoryInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumVictoryInfos())
#endif
	{
		if(m_piVictoryMinThreshold && m_piVictoryMinThreshold[i] != 0)
		{
			return m_piVictoryMinThreshold[i];
		}

		return GetVictoryThreshold(i);
	}

	return 0;
}


#ifdef AUI_WARNING_FIXES
int CvProjectEntry::GetProjectsNeeded(uint i) const
{
	CvAssertMsg(i < GC.getNumProjectInfos(), "Index out of bounds");

	if (i < GC.getNumProjectInfos() && m_piProjectsNeeded)
#else
int CvProjectEntry::GetProjectsNeeded(int i) const
{
	CvAssertMsg(i < GC.getNumProjectInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumProjectInfos() && m_piProjectsNeeded)
#endif
	{
		return m_piProjectsNeeded[i];
	}

	return 0;
}





CvProjectXMLEntries::CvProjectXMLEntries(void)
{

}


CvProjectXMLEntries::~CvProjectXMLEntries(void)
{
	DeleteArray();
}


std::vector<CvProjectEntry*>& CvProjectXMLEntries::GetProjectEntries()
{
	return m_paProjectEntries;
}


#ifdef AUI_WARNING_FIXES
uint CvProjectXMLEntries::GetNumProjects() const
#else
int CvProjectXMLEntries::GetNumProjects()
#endif
{
	return m_paProjectEntries.size();
}


void CvProjectXMLEntries::DeleteArray()
{
	for(std::vector<CvProjectEntry*>::iterator it = m_paProjectEntries.begin(); it != m_paProjectEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paProjectEntries.clear();
}


#ifdef AUI_WARNING_FIXES
_Ret_maybenull_ CvProjectEntry* CvProjectXMLEntries::GetEntry(uint index)
#else
CvProjectEntry* CvProjectXMLEntries::GetEntry(int index)
#endif
{
	return m_paProjectEntries[index];
}
