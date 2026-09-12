
#pragma once

#include <fstream>
#include <unordered_map>

#include "IDatabaseLogger.h"

#define DB_NUM_PAGES			5000
#define DB_NUM_THREADS			1
#define DB_PAGECACHE_SIZE		1168


struct sqlite3;
struct sqlite3_stmt;


namespace Database{

	class Results;
	class Connection;


	static void Error(const char* szErrorMsg);


	enum OpenFlags
	{

		OPEN_READONLY	= 0x00000001,


		OPEN_READWRITE	= 0x00000002,


		OPEN_CREATE		= 0x00000004,


		OPEN_NOMUTEX	= 0x00008000,


		OPEN_FULLMUTEX	= 0x00010000,
	};




	class Command
	{
	public:
		Command(const Connection* connection = 0, sqlite3_stmt* stmt = 0):
			connection(connection), stmt(stmt)
		{}

		const Connection* connection;
		sqlite3_stmt* stmt;
	};
	


	class Connection
	{
	public:
		CvGameDatabaseAPI Connection();
		CvGameDatabaseAPI Connection(const char* filename, int flags = 0);
		CvGameDatabaseAPI ~Connection();

		CvGameDatabaseAPI bool Open(const char* filename, int flags = 0);
		CvGameDatabaseAPI void Close();
		

		CvGameDatabaseAPI void BeginTransaction();
		CvGameDatabaseAPI void EndTransaction();
		CvGameDatabaseAPI void RollbackTransaction();

		CvGameDatabaseAPI void BeginDeferredTransaction();
		CvGameDatabaseAPI void BeginImmediateTransaction();
		CvGameDatabaseAPI void BeginExclusiveTransaction();
		CvGameDatabaseAPI void CommitTransaction();


		CvGameDatabaseAPI bool SetSavePoint(const char* savePoint);
		CvGameDatabaseAPI bool RollbackToSavePoint(const char* savePoint);


		CvGameDatabaseAPI int Count(const char* tableName, bool bCache = true);


		CvGameDatabaseAPI void ClearCountCache();


		CvGameDatabaseAPI bool SelectAt(Results& kResults, const char* tableName, int rowID);
		CvGameDatabaseAPI bool SelectAt(Results& kResults, const char* tableName, const char* columnName, const char* value);
		CvGameDatabaseAPI bool SelectAt(Results& kResults, const char* tableName, const char* columnName, int value);
		CvGameDatabaseAPI bool SelectAt(Results& kResults, const char* tableName, const char* columnName, double value);

		CvGameDatabaseAPI bool SelectAll(Results& kResults, const char* tableName);
		
		CvGameDatabaseAPI bool SelectWhere(Results& kResults, const char* tableName, const char* condition);


		CvGameDatabaseAPI bool Execute(Results& kResults, const char* szCommand, int lenCommand = -1) const;
		CvGameDatabaseAPI bool Execute(const char* szCommand, int lenCommand = -1) const;


		CvGameDatabaseAPI bool ExecuteMultiple(const char* szCommands, int lenCommands = -1) const;


		CvGameDatabaseAPI int TotalChanges() const;


		CvGameDatabaseAPI void Analyze() const;


		CvGameDatabaseAPI void Vacuum() const;


		CvGameDatabaseAPI int StatementCount() const;


		CvGameDatabaseAPI bool SetBusyTimeout(int ms);

		CvGameDatabaseAPI const char* StatementSQL(int index) const;


		bool Report(int status) const;


		CvGameDatabaseAPI void SetLogger(IDatabaseLogger* pkLogger);
		CvGameDatabaseAPI void LogMessage(const char* szMessage) const;
		CvGameDatabaseAPI void LogWarning(const char* szWarning) const;
		CvGameDatabaseAPI void LogError(const char* szError) const;


		CvGameDatabaseAPI const char* CalculateMemoryStats();


		CvGameDatabaseAPI bool ValidateFKConstraints(bool bAllowNULL = true) const;

		CvGameDatabaseAPI void* operator new(size_t tSize);
		CvGameDatabaseAPI void operator delete(void* pMem);
		

		sqlite3* GetSQLite3() const;


		CvGameDatabaseAPI int ErrorCode() const;
		CvGameDatabaseAPI const char* ErrorMessage() const;


		static bool AcquireCommand(Command& command, const char* szSQL, int lenSQL = -1);
		static void ReleaseCommand(Command& command);


	protected:
		static void InitMemoryManagement();

	private:

		Connection(const Connection&);
		Connection& operator=(const Connection&);

		static bool ms_bMemoryManagerInitialized;


		sqlite3* m_db;

#if defined(LEKMOD_MACOS)



		unsigned char m_hostStorage[176];
#else

		std::string m_strMemoryStats;


		std::tr1::unordered_map<std::string, sqlite3_stmt*> m_hshCountStatements;
		std::tr1::unordered_map<std::string, int> m_hshCountValues;
#endif

		static char ms_pPageCacheBuffer[DB_PAGECACHE_SIZE * DB_NUM_PAGES];
		static char ms_pScratchBuffer[DB_PAGECACHE_SIZE * DB_NUM_THREADS * 6];	

#if !defined(LEKMOD_MACOS)
		mutable std::auto_ptr<IDatabaseLogger> m_pkDatabaseLogger;
#endif
	};
}
