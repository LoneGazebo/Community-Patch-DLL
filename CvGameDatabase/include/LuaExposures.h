#pragma once 

#if !defined(NO_LUA_SUPPORT)

namespace Database{ namespace Scripting{

	class Lua
	{
	public:
		//! Pushes database exposures onto the Lua stack.
		//! NOTE: dbConn must remain active until this instance is removed from Lua.
		//! On error, will utilize Lua's error reporting mechanism.
		CvGameDatabaseAPI static void PushDatabase( lua_State* L, Database::Connection& dbConn );
		
		//! Generates and pushes a database query onto the stack.
		CvGameDatabaseAPI static Database::Results* PushDatabaseQuery(lua_State* L, Database::Connection* db, const char* szSQL);

		//! Pushes a database table onto the stack.
		CvGameDatabaseAPI static void PushDatabaseTable(lua_State* L, Database::Connection& dbConn, const char* szTableName);

		//! Pushes a single database row onto the stack.
		CvGameDatabaseAPI static void PushDatabaseRow(lua_State* L, Database::Results* pResults);

		//! Returns a table containing memory usage information.
		CvGameDatabaseAPI static int lGetMemoryUsage( lua_State* L);

		//! Performs a deeper scan of the internal heap to obtain memory usage statistics.
		CvGameDatabaseAPI static int lCollectMemoryUsage( lua_State* L);
		
	private:
		
		//! Generates and pushes a subtable onto the stack.
		static void PushSubTable(lua_State* L, const char* tableName);

		//! Database Query meta-methods.
		static int lFinalizeDatabaseQuery(lua_State* L);

		//! Auxiliary method for iterating a database table.
		static int lIterateTable(lua_State* L);

		// Database Row meta-methods.
		static int lInvalidColumnCheck(lua_State* L);
		static int lNewIndexDatabaseRow(lua_State* L);

		// Database Table meta-methods.
		static int lCallDatabaseTable(lua_State* L);
		static int lIndexDatabaseTable(lua_State* L);
		static int lIndexDatabaseTableStub(lua_State* L);
		static int lLengthOfDatabaseTable(lua_State* L);
		static int lLengthOfDatabaseTableStub(lua_State* L);
		static int lNewIndexDatabaseTable(lua_State* L);

		// Database meta-methods.
		static int lIndexDatabase(lua_State* L);
	};

}}	//namespace Database::Scripting

#endif //!defined( NO_LUA_SUPPORT )