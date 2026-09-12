

#pragma once 

#include <unordered_map>

namespace Database{

class ResultsCache
{

public:
	CvGameDatabaseAPI ResultsCache(Connection& db);
	CvGameDatabaseAPI ~ResultsCache();


	CvGameDatabaseAPI void Clear();
	CvGameDatabaseAPI void Clear(const std::string& strKey);


	CvGameDatabaseAPI Database::Connection& Connection();


	CvGameDatabaseAPI Results* Get(const std::string& strKey);


	CvGameDatabaseAPI Results* Prepare(const std::string& strKey, const char* szStmt, int lenStmt = -1);


	CvGameDatabaseAPI const char* ErrorMessage() const;

private:
	ResultsCache(const ResultsCache&);
	ResultsCache& operator=(const ResultsCache&);

	Database::Connection& m_db;

	typedef std::tr1::unordered_map<std::string, Results*> ResultsMap;
	ResultsMap m_storedResults;
};


}
