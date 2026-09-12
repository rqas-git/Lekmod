

#pragma once 


class FXmlTraverser;

namespace Database
{
	class ResultsCache;
	class XMLSerializer
	{
	public:
		CvGameDatabaseAPI XMLSerializer(Connection& database);
		CvGameDatabaseAPI ~XMLSerializer();




		CvGameDatabaseAPI bool Load(const wchar_t* wszFilename);
		CvGameDatabaseAPI bool Load(const wchar_t* wszFilename, ResultsCache& kResultsCache);






		CvGameDatabaseAPI bool LoadFromMemory(const wchar_t* wszFilename, char* szBuffer, size_t lenBuffer);
		CvGameDatabaseAPI bool LoadFromMemory(const wchar_t* wszFilename, char* szBuffer, size_t lenBuffer, ResultsCache& kResultsCache);



		CvGameDatabaseAPI bool UseTransactions() const;
		CvGameDatabaseAPI void UseTransactions(bool bUse);
		
		CvGameDatabaseAPI const char* ErrorMessage() const;

		CvGameDatabaseAPI void SetPackageTag(const char* szTag);
		CvGameDatabaseAPI void AddTableToPackageTagList(const char* szTag);
		CvGameDatabaseAPI void RemoveTableFromPackageTagList(const char* szTag);
		CvGameDatabaseAPI void ClearPackageTagList();

	protected:
		enum StringType
		{
			STRINGTYPE_UNKNOWN	= -1,
			STRINGTYPE_NULL		= 0,
			STRINGTYPE_TEXT		= 1,
			STRINGTYPE_INT		= 2,
			STRINGTYPE_REAL		= 3,			
		};

		enum InsertConflictClause
		{
			NONE,
			OR_ABORT,
			OR_IGNORE,
			OR_REPLACE,
		};

		void SetErrorMessage(const char* szMsg);
		StringType GetStringType(const char* szString, const size_t lenString);

		bool BuildTable(FXmlTraverser& kXml);
		bool IndexTable(FXmlTraverser& kXml);
		bool UpdateTable(FXmlTraverser& kXml, ResultsCache& kResultsCache);
		bool DeleteMissingReferences(FXmlTraverser& kXml);
	
		bool InsertRow(FXmlTraverser& kRow, ResultsCache& kResultsCache, const char* szTableName, size_t lenTableName, InsertConflictClause clause = NONE);
		bool InsertOrIgnoreRow(FXmlTraverser& kRow, ResultsCache& kResultsCache, const char* szTableName, size_t lenTableName);
		bool ReplaceRow(FXmlTraverser& kRow, ResultsCache& kResultsCache, const char* szTableName, size_t lenTableName);
		bool DeleteRow(FXmlTraverser& kRow, ResultsCache& kResultsCache, const char* szTableName, size_t lenTableName);
		bool UpdateRow(FXmlTraverser& kRows, ResultsCache& kResultsCache, const char* szTableName, size_t lenTableName);



		bool LoadFileIntoBuffer(const wchar_t* wszFilename, char*& pkFilebuffer, size_t& pkNewBufferSize, size_t& kOutFileSize);

		void PostColumnInsert(std::stringstream& columns, std::list<std::string> &values, const char* szTableName, size_t lenTableName);

	private:
		XMLSerializer(const XMLSerializer&);
		XMLSerializer& operator=(const XMLSerializer&);

		char m_szCurrentFileName[256];

		char m_szErrorMessage[512];
		Connection& m_db;

		char* m_pkFileBuffer;
		size_t m_lenFileBuffer;

		bool m_bUseTransactions;

		typedef std::list<std::string> PackageTagTableList;
		PackageTagTableList		m_lstPackageTagTables;
		std::string				m_szPackageTag;
	};

}




inline const char* Database::XMLSerializer::ErrorMessage() const
{
	return m_szErrorMessage;
}
