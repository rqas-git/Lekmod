#pragma once 

#if !defined(NO_LUA_SUPPORT)

namespace Database{ namespace Scripting{

	class Lua
	{
	public:



		CvGameDatabaseAPI static void PushDatabase( lua_State* L, Database::Connection& dbConn );
		

		CvGameDatabaseAPI static Database::Results* PushDatabaseQuery(lua_State* L, Database::Connection* db, const char* szSQL);


		CvGameDatabaseAPI static void PushDatabaseTable(lua_State* L, Database::Connection& dbConn, const char* szTableName);


		CvGameDatabaseAPI static void PushDatabaseRow(lua_State* L, Database::Results* pResults);


		CvGameDatabaseAPI static int lGetMemoryUsage( lua_State* L);


		CvGameDatabaseAPI static int lCollectMemoryUsage( lua_State* L);
		
	private:
		

		static void PushSubTable(lua_State* L, const char* tableName);


		static int lFinalizeDatabaseQuery(lua_State* L);


		static int lIterateTable(lua_State* L);


		static int lInvalidColumnCheck(lua_State* L);
		static int lNewIndexDatabaseRow(lua_State* L);


		static int lCallDatabaseTable(lua_State* L);
		static int lIndexDatabaseTable(lua_State* L);
		static int lIndexDatabaseTableStub(lua_State* L);
		static int lLengthOfDatabaseTable(lua_State* L);
		static int lLengthOfDatabaseTableStub(lua_State* L);
		static int lNewIndexDatabaseTable(lua_State* L);


		static int lIndexDatabase(lua_State* L);
	};

}}

#endif
