











#pragma once

#include "CvInfos.h"
#include "CvGlobals.h"
#include "ICvDLLDatabaseUtility.h"








class CvDllDatabaseUtility: public ICvDLLDatabaseUtility1
{
public:
	CvDllDatabaseUtility(Database::Connection& db);
	~CvDllDatabaseUtility();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);


	bool DLLCALL CacheGameDatabaseData();


	bool DLLCALL FlushGameDatabaseData();




	bool DLLCALL PerformDatabasePostProcessing();

#ifdef LEKMOD_POST_DLC_DATA_LOADING

	bool PerformPostDLCLoading();


	void ProcessXMLFiles(const std::wstring& wszPath, Database::XMLSerializer& serializer);
#endif

private:
	void DLLCALL Destroy();


	bool PrefetchGameData();


	bool ValidateGameDatabase();
	bool ValidatePrefetchProcess();

	bool UpdatePlayableCivilizationCounts();


	bool SetGlobals();


	bool LoadGlobalDefines();



	bool SetStringList(CvString** ppszStringArray, int* piSize);

	template<class T>
	bool PrefetchCollection(std::vector<T*>& kCollection, const char* tableName);




	bool SetGlobalActionInfo();

	template <class T>
	void orderHotkeyInfo(int** ppiSortedIndex, T* pHotkeyInfos, int iLength);
	void orderHotkeyInfo(int** ppiSortedIndex, int* pHotkeyIndex, int iLength);

	void LogMsg(const char* format, ...) const;


	Database::Connection&	m_kGameplayDatabase;


	bool m_bGameDatabaseNeedsCaching;


	unsigned int m_uiRefCount;
};




template<class T>
bool CvDllDatabaseUtility::PrefetchCollection(std::vector<T*>& kCollection, const char* tableName)
{
	cvStopWatch kPerfTest(tableName, "xml-perf.log");


#if defined(LEKMOD_MACOS)
	for(typename std::vector<T*>::iterator it = kCollection.begin(); it != kCollection.end(); ++it)
#else
	for(std::vector<T*>::iterator it = kCollection.begin(); it != kCollection.end(); ++it)
#endif
	{
		T* pkItem = (*it);
		delete pkItem;
	}
	kCollection.clear();


	size_t index = 0;
	Database::Results kResults;
	CvDatabaseUtility kUtility;

	if(DB.SelectWhere(kResults, tableName, "ID > -1 ORDER BY ID"))
	{
		while(kResults.Step())
		{
			size_t Id = kResults.GetInt("ID");
			CvAssertMsg(index <= Id, "This should never happen!")

			while(Id > index)
			{
				kCollection.push_back(NULL);
				index++;
			}

			T* pkItem = FNEW(T, c_eCiv5GameplayDLL, 0);
			pkItem->CacheResults(kResults, kUtility);
			kCollection.push_back(pkItem);
			index++;
		}
	}
	else
	{
		char szErrorMsg[512];
		sprintf_s(szErrorMsg, "PrefetchCollection: Cannot find table '%s'.  Error - %s", tableName, DB.ErrorMessage());
		CvAssertMsg(false, szErrorMsg);

	}

	return true;
}
