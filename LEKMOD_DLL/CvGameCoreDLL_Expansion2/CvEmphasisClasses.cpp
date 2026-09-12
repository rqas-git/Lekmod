






#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "ICvDLLUserInterface.h"
#include "CvInfosSerializationHelper.h"


#include "LintFree.h"


CvEmphasisEntry::CvEmphasisEntry(void):
	m_bAvoidGrowth(false),
	m_bGreatPeople(false),
	m_piYieldModifiers(NULL)
{
}


CvEmphasisEntry::~CvEmphasisEntry(void)
{
	SAFE_DELETE_ARRAY(m_piYieldModifiers);
}


bool CvEmphasisEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	m_bAvoidGrowth = kResults.GetBool("AvoidGrowth");
	m_bGreatPeople = kResults.GetBool("GreatPeople");

	const char* szEmphasizeType = GetType();
	kUtility.SetYields(m_piYieldModifiers, "EmphasizeInfo_Yields", "EmphasizeType", szEmphasizeType);

	return true;
}


bool CvEmphasisEntry::IsAvoidGrowth() const
{
	return m_bAvoidGrowth;
}

bool CvEmphasisEntry::IsGreatPeople() const
{
	return m_bGreatPeople;
}

int CvEmphasisEntry::GetYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldModifiers ? m_piYieldModifiers[i] : -1;
}





CvEmphasisXMLEntries::CvEmphasisXMLEntries(void)
{

}


CvEmphasisXMLEntries::~CvEmphasisXMLEntries(void)
{
	DeleteArray();
}


std::vector<CvEmphasisEntry*>& CvEmphasisXMLEntries::GetEmphasisEntries()
{
	return m_paEmphasisEntries;
}


#ifdef AUI_WARNING_FIXES
uint CvEmphasisXMLEntries::GetNumEmphases() const
#else
int CvEmphasisXMLEntries::GetNumEmphases()
#endif
{
	return m_paEmphasisEntries.size();
}


void CvEmphasisXMLEntries::DeleteArray()
{
	for(std::vector<CvEmphasisEntry*>::iterator it = m_paEmphasisEntries.begin(); it != m_paEmphasisEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paEmphasisEntries.clear();
}


#ifdef AUI_WARNING_FIXES
_Ret_maybenull_ CvEmphasisEntry* CvEmphasisXMLEntries::GetEntry(uint index)
#else
CvEmphasisEntry* CvEmphasisXMLEntries::GetEntry(int index)
#endif
{
	return m_paEmphasisEntries[index];
}





#ifdef AUI_CITY_FIX_COMPONENT_CONSTRUCTORS_CONTAIN_POINTERS
CvCityEmphases::CvCityEmphases(CvCity* pCity) :
	m_aiEmphasizeYieldCount(), m_pCity(pCity), m_pEmphases(NULL), m_iEmphasizeAvoidGrowthCount(0), m_iEmphasizeGreatPeopleCount(0)
#elif defined(AUI_WARNING_FIXES)
CvCityEmphases::CvCityEmphases(CvCity* pCity) :
	m_aiEmphasizeYieldCount(), m_pCity(NULL), m_pEmphases(NULL), m_iEmphasizeAvoidGrowthCount(0), m_iEmphasizeGreatPeopleCount(0)
#else
CvCityEmphases::CvCityEmphases()
#endif
{
	m_pbEmphasize = NULL;
}


CvCityEmphases::~CvCityEmphases(void)
{

}


void CvCityEmphases::Init(CvEmphasisXMLEntries* pEmphases, CvCity* pCity)
{

	m_pEmphases = pEmphases;
	m_pCity = pCity;

	Reset();
}


void CvCityEmphases::Uninit()
{
	SAFE_DELETE_ARRAY(m_pbEmphasize);
}


void CvCityEmphases::Reset()
{
	Uninit();

	m_iEmphasizeAvoidGrowthCount = 0;
	m_iEmphasizeGreatPeopleCount = 0;

	for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiEmphasizeYieldCount[iI] = 0;
	}

	CvAssertMsg(m_pbEmphasize == NULL, "m_pbEmphasize not NULL!!!");
	CvAssertMsg(GC.getNumEmphasisInfos() > 0,  "GC.getNumEmphasizeInfos() is not greater than zero but an array is being allocated in CvCityEmphases::Reset");
#ifdef AUI_WARNING_FIXES
	SAFE_DELETE_ARRAY(m_pbEmphasize);
#endif
	m_pbEmphasize = FNEW(bool[GC.getNumEmphasisInfos()], c_eCiv5GameplayDLL, 0);
#ifdef AUI_WARNING_FIXES
	for (uint iI = 0; iI < GC.getNumEmphasisInfos(); iI++)
#else
	for(int iI = 0; iI < GC.getNumEmphasisInfos(); iI++)
#endif
	{
		m_pbEmphasize[iI] = false;
	}
}


int CvCityEmphases::GetEmphasizeAvoidGrowthCount()
{
	return m_iEmphasizeAvoidGrowthCount;
}


bool CvCityEmphases::IsEmphasizeAvoidGrowth()
{
	return (GetEmphasizeAvoidGrowthCount() > 0);
}


int CvCityEmphases::GetEmphasizeYieldCount(YieldTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	if(eIndex >= 0 && eIndex < NUM_YIELD_TYPES)
		return m_aiEmphasizeYieldCount[eIndex];
	return 0;
}


bool CvCityEmphases::IsEmphasizeYield(YieldTypes eIndex)
{
	return (GetEmphasizeYieldCount(eIndex) > 0);
}


bool CvCityEmphases::IsEmphasize(EmphasizeTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumEmphasisInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(m_pbEmphasize != NULL, "m_pbEmphasize is not expected to be equal with NULL");

	if(!m_pbEmphasize) return false;

	return m_pbEmphasize[eIndex];
}


void CvCityEmphases::SetEmphasize(EmphasizeTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumEmphasisInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(IsEmphasize(eIndex) != bNewValue)
	{
		m_pbEmphasize[eIndex] = bNewValue;

		CvEmphasisEntry* pkEmphasis = GC.getEmphasisInfo(eIndex);

#ifdef AUI_WARNING_FIXES
		if (!pkEmphasis)
			return;
#endif

		if(pkEmphasis->IsAvoidGrowth())
		{
			m_iEmphasizeAvoidGrowthCount += ((IsEmphasize(eIndex)) ? 1 : -1);
			CvAssert(GetEmphasizeAvoidGrowthCount() >= 0);
		}

		if(pkEmphasis->IsGreatPeople())
		{
			m_iEmphasizeGreatPeopleCount += ((IsEmphasize(eIndex)) ? 1 : -1);
		}

		for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			if(pkEmphasis->GetYieldChange(iI))
			{
				m_aiEmphasizeYieldCount[iI] += ((IsEmphasize(eIndex)) ? 1 : -1);
				CvAssert(GetEmphasizeYieldCount((YieldTypes)iI) >= 0);
			}
		}

		if((m_pCity->getOwner() == GC.getGame().getActivePlayer()) && m_pCity->isCitySelected())
		{
			GC.GetEngineUserInterface()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}


void CvCityEmphases::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_iEmphasizeAvoidGrowthCount;
	kStream >> m_iEmphasizeGreatPeopleCount;

	kStream >> m_aiEmphasizeYieldCount;

	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pbEmphasize, GC.getNumEmphasisInfos());
}


void CvCityEmphases::Write(FDataStream& kStream)
{

	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << m_iEmphasizeAvoidGrowthCount;
	kStream << m_iEmphasizeGreatPeopleCount;

	kStream << m_aiEmphasizeYieldCount;

	CvInfosSerializationHelper::WriteHashedDataArray<EmphasizeTypes, bool>(kStream, m_pbEmphasize, GC.getNumEmphasisInfos());
}
