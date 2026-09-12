






#pragma once

#ifndef CIV5_NOTIFICATION_CLASSES_H
#define CIV5_NOTIFICATION_CLASSES_H












class CvNotificationEntry
{
public:
	CvNotificationEntry(void);
	~CvNotificationEntry(void);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);


	const char* GetTypeName();

private:
	CvString m_strNotificationType;
};











class CvNotificationXMLEntries
{
public:
	CvNotificationXMLEntries(void);
	~CvNotificationXMLEntries(void);

	typedef std::vector<CvNotificationEntry*> EntryArray;


	EntryArray& GetNotificationEntries();
#ifdef AUI_WARNING_FIXES
	uint GetNumNotifications() const;
#else
	int GetNumNotifications();
#endif
	_Ret_maybenull_ CvNotificationEntry* GetEntry(int index);
	_Ret_maybenull_ CvNotificationEntry* GetByID(uint hHash);
	_Ret_maybenull_ CvNotificationEntry* GetByString(const char* pszName);

	void DeleteArray();

private:
	EntryArray m_paNotificationEntries;
	typedef std::map<uint, int> EntryHashTable;
	EntryHashTable m_mEntries;

};

#endif
