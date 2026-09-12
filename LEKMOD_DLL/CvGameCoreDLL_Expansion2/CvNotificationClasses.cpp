






#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"


#include "LintFree.h"


CvNotificationEntry::CvNotificationEntry(void)
{
	m_strNotificationType = "";
}


CvNotificationEntry::~CvNotificationEntry(void)
{
}


bool CvNotificationEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility&)
{
	m_strNotificationType = kResults.GetText("NotificationType");
	return true;
}


const char* CvNotificationEntry::GetTypeName()
{
	return m_strNotificationType.c_str();
}





CvNotificationXMLEntries::CvNotificationXMLEntries(void)
{

}


CvNotificationXMLEntries::~CvNotificationXMLEntries(void)
{
	DeleteArray();
}


std::vector<CvNotificationEntry*>& CvNotificationXMLEntries::GetNotificationEntries()
{
	return m_paNotificationEntries;
}


#ifdef AUI_WARNING_FIXES
uint CvNotificationXMLEntries::GetNumNotifications() const
#else
int CvNotificationXMLEntries::GetNumNotifications()
#endif
{
	return m_paNotificationEntries.size();
}


void CvNotificationXMLEntries::DeleteArray()
{
	for(std::vector<CvNotificationEntry*>::iterator it = m_paNotificationEntries.begin(); it != m_paNotificationEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paNotificationEntries.clear();
}


#ifdef AUI_WARNING_FIXES
_Ret_maybenull_ CvNotificationEntry* CvNotificationXMLEntries::GetEntry(int index)
#else
CvNotificationEntry* CvNotificationXMLEntries::GetEntry(int index)
#endif
{
	FAssert(index < static_cast<int>(m_paNotificationEntries.size()));

	if(index < static_cast<int>(m_paNotificationEntries.size()))
		return m_paNotificationEntries[index];
	return NULL;
}


#ifdef AUI_WARNING_FIXES
_Ret_maybenull_ CvNotificationEntry* CvNotificationXMLEntries::GetByID(uint hHash)
#else
CvNotificationEntry* CvNotificationXMLEntries::GetByID(uint hHash)
#endif
{
	EntryHashTable::iterator itr = m_mEntries.find(hHash);
	if (itr != m_mEntries.end())
		return GetEntry((*itr).second);
	return NULL;
}


#ifdef AUI_WARNING_FIXES
_Ret_maybenull_ CvNotificationEntry* CvNotificationXMLEntries::GetByString(const char* pszName)
#else
CvNotificationEntry* CvNotificationXMLEntries::GetByString(const char* pszName)
#endif
{
	if (pszName && pszName[0] != 0)
		return GetByID(FString::Hash(pszName));
	return NULL;
}
