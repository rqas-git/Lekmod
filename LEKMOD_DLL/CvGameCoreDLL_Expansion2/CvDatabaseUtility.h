













#pragma once
#ifndef CVDATABASEUTILITY_H

class CvDatabaseUtility
{
public:
	CvDatabaseUtility();
	~CvDatabaseUtility();


	void ClearResults();
	void ClearResults(const std::string& strKey);


	Database::Results* GetResults(const std::string& strKey);


	Database::Results* GetOrPrepareResults(const std::string& strKey, const char* szStmt);


	Database::Results* PrepareResults(const std::string& strKey, const char* szStmt);

#ifdef LEKMOD_POST_DLC_DATA_LOADING

	void RefreshLanguageTextFallback();
#endif


	void InitializeArray(int*& pArray, const size_t count, int iDefault = 0);
	void InitializeArray(bool*& pArray, const size_t count, bool bDefault = false);
	void InitializeArray(float*& pArray, const size_t count, float fDefault = 0.0f);
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	bool Initialize2DArray(int**& ppArray, const size_t iCount1, const size_t iCount2, int iDefault = 0);
#else
	void Initialize2DArray(int**& ppArray, const size_t iCount1, const size_t iCount2, int iDefault = 0);
#endif


	template<typename T>
#if defined(LEKMOD_MACOS)
void InitializeArray(T*& pArray, const char* szTableName, T defaultValue = (T)0);
#else
void InitializeArray(T*& pArray, const char* szTableName, T default = (T)0);
#endif

	bool Initialize2DArray(int**& pArray, const char* szTable1Name, const char* szTable2Name, int iDefault = 0);
#ifdef AUI_DATABASE_UTILITY_PROPER_2D_ALLOCATION_AND_DESTRUCTION
	void SafeDelete2DArray(int**& ppArray, const char* szTable1Name);
	static void SafeDelete2DArray(int**& ppArray, const size_t iRowCount);
#else
	static void SafeDelete2DArray(int**& pArray);
#endif




	bool PopulateArrayByExistence(bool*& pArray,	const char* szTypeTableName,
	                              const char* szDataTableName,
	                              const char* szTypeColumn,
	                              const char* szFilterColumn,
	                              const char* szFilterValue);



	bool PopulateArrayByExistence(int*& pArray,	const char* szTypeTableName,
	                              const char* szDataTableName,
	                              const char* szTypeColumn,
	                              const char* szFilterColumn,
	                              const char* szFilterValue);



	bool PopulateArrayByValue(int*& pArray, const char* szTypeTableName,
	                          const char* szDataTableName,
	                          const char* szTypeColumn,
	                          const char* szFilterColumn,
	                          const char* szFilterValue,
	                          const char* szValueColumn,
							  int iDefaultValue = 0,
							  int iMinArraySize = 0);










	bool SetFlavors(int*& pFlavorsArray, const char* szTableName, const char* szFilterColumn, const char* szFilterValue, int iDefaultValue = 0);









	bool SetYields(int*& pYieldsArray, const char* szTableName, const char* szFilterColumn, const char* szFilterValue);


	void SetYieldMatrix(int**& pYieldsArray, const char* szDimensionTable, const char* szKey,
	                    const char* szQuery, const char* szFilterValue);


	int MaxRows(const char* szTableName);


	const char* GetErrorMessage() const;

private:
	typedef stdext::hash_map<std::string, Database::Results*> ResultsMap;
	ResultsMap m_storedResults;
};




inline void CvDatabaseUtility::InitializeArray(int*& pArray, const size_t count, int iDefault)
{
	CvAssertMsg(count > 0, "Initializing array to 0 or less items.");
#ifdef AUI_WARNING_FIXES
	delete[] pArray;
#endif
	pArray = FNEW(int[count], c_eCiv5GameplayDLL, 0);
	if(iDefault == 0)
	{
		ZeroMemory(pArray, sizeof(int) * count);
	}
	else
	{
		for(size_t i = 0; i < count; ++i)
			pArray[i] = iDefault;
	}
}

inline void CvDatabaseUtility::InitializeArray(bool*& pArray, const size_t count, bool bDefault)
{
	CvAssertMsg(count > 0, "Initializing array to 0 or less items.");
#ifdef AUI_WARNING_FIXES
	delete[] pArray;
#endif
	pArray = FNEW(bool[count], c_eCiv5GameplayDLL, 0);
#ifdef AUI_WARNING_FIXES
	if (bDefault == false)
#else
	if(bDefault == 0.0f)
#endif
	{
		ZeroMemory(pArray, sizeof(bool) * count);
	}
	else
	{
		for(size_t i = 0; i < count; ++i)
			pArray[i] = bDefault;
	}
}

inline void CvDatabaseUtility::InitializeArray(float*& pArray, const size_t count, float fDefault)
{
	CvAssertMsg(count > 0, "Initializing array to 0 or less items.");
#ifdef AUI_WARNING_FIXES
	delete[] pArray;
#endif
	pArray = FNEW(float[count], c_eCiv5GameplayDLL, 0);
	if(fDefault == 0.0f)
	{
		ZeroMemory(pArray, sizeof(float) * count);
	}
	else
	{
		for(size_t i = 0; i < count; ++i)
			pArray[i] = fDefault;
	}
}

template<typename T>
#if defined(LEKMOD_MACOS)
inline void CvDatabaseUtility::InitializeArray(T*& pArray, const char* szTableName, T defaultValue)
#else
inline void CvDatabaseUtility::InitializeArray(T*& pArray, const char* szTableName, T default)
#endif
{
	size_t count = MaxRows(szTableName);







	if(count == 0)
		count = 1;

#if defined(LEKMOD_MACOS)
	InitializeArray(pArray, count, defaultValue);
#else
	InitializeArray(pArray, count, default);
#endif
}

#endif
