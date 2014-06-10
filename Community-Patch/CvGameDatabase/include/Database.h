
#pragma once

#include <fstream>
#include <unordered_map>

#include "IDatabaseLogger.h"

#define DB_NUM_PAGES			5000
#define DB_NUM_THREADS			1
#define DB_PAGECACHE_SIZE		1168

//SQLite Forward declarations
struct sqlite3;
struct sqlite3_stmt;


namespace Database{

	class Results;		//Forward declaration.
	class Connection;	//Forward declaration.

	//!Error reporting mechanism used by all Database classes.
	static void Error(const char* szErrorMsg);

	//! Open Flags used for opening a database.
	enum OpenFlags
	{
		//! Opens the database as read only.
		OPEN_READONLY	= 0x00000001,

		//! Opens the database for read/write access.
		OPEN_READWRITE	= 0x00000002,

		//! Creates a new database.
		OPEN_CREATE		= 0x00000004,

		//! Disables thread safety.
		OPEN_NOMUTEX	= 0x00008000,

		//! Enables thread safety.
		OPEN_FULLMUTEX	= 0x00010000,
	};

	//! Prepared database command.
	//! Currently just a container but will later contain
	//! methods useful for binding custom parameters to the command.
	class Command
	{
	public:
		Command(const Connection* connection = 0, sqlite3_stmt* stmt = 0):
			connection(connection), stmt(stmt)
		{}

		const Connection* connection;
		sqlite3_stmt* stmt;
	};
	
	//! The main database class.
	//! Represents a single database.
	class Connection
	{
	public:
		CvGameDatabaseAPI Connection();
		CvGameDatabaseAPI Connection(const char* filename, int flags = 0);
		CvGameDatabaseAPI ~Connection();

		CvGameDatabaseAPI bool Open(const char* filename, int flags = 0);
		CvGameDatabaseAPI void Close();
		
		//Transactions
		CvGameDatabaseAPI void BeginTransaction();
		CvGameDatabaseAPI void EndTransaction();
		CvGameDatabaseAPI void RollbackTransaction();

		CvGameDatabaseAPI void BeginDeferredTransaction();
		CvGameDatabaseAPI void BeginImmediateTransaction();
		CvGameDatabaseAPI void BeginExclusiveTransaction();
		CvGameDatabaseAPI void CommitTransaction();

		//SavePoints
		CvGameDatabaseAPI bool SetSavePoint(const char* savePoint);
		CvGameDatabaseAPI bool RollbackToSavePoint(const char* savePoint);

		//Row counts
		CvGameDatabaseAPI int Count(const char* tableName, bool bCache = true);

		//! Clear all cached values and prepared statements from Count().
		CvGameDatabaseAPI void ClearCountCache();

		//Selection
		CvGameDatabaseAPI bool SelectAt(Results& kResults, const char* tableName, int rowID);
		CvGameDatabaseAPI bool SelectAt(Results& kResults, const char* tableName, const char* columnName, const char* value);
		CvGameDatabaseAPI bool SelectAt(Results& kResults, const char* tableName, const char* columnName, int value);
		CvGameDatabaseAPI bool SelectAt(Results& kResults, const char* tableName, const char* columnName, double value);

		CvGameDatabaseAPI bool SelectAll(Results& kResults, const char* tableName);
		
		CvGameDatabaseAPI bool SelectWhere(Results& kResults, const char* tableName, const char* condition);

		//! Executes a database command.
		CvGameDatabaseAPI bool Execute(Results& kResults, const char* szCommand, int lenCommand = -1) const;
		CvGameDatabaseAPI bool Execute(const char* szCommand, int lenCommand = -1) const;

		//! Executes multiple statements that are combined.
		CvGameDatabaseAPI bool ExecuteMultiple(const char* szCommands, int lenCommands = -1) const;

		//! Retrieves the total number of changes since database was opened.
		CvGameDatabaseAPI int TotalChanges() const;

		//! Analyzes the database and creates a stats table.
		CvGameDatabaseAPI void Analyze() const;

		//! Shrinks database by removing unused allocated space.
		CvGameDatabaseAPI void Vacuum() const;

		//! Statement Information
		CvGameDatabaseAPI int StatementCount() const;

		//! Busy Timeout
		CvGameDatabaseAPI bool SetBusyTimeout(int ms);

		CvGameDatabaseAPI const char* StatementSQL(int index) const;

		//! Used for error reporting.  Returns false on error.
		bool Report(int status) const;

		//! Push notification logging methods
		CvGameDatabaseAPI void SetLogger(IDatabaseLogger* pkLogger);
		CvGameDatabaseAPI void LogMessage(const char* szMessage) const;
		CvGameDatabaseAPI void LogWarning(const char* szWarning) const;
		CvGameDatabaseAPI void LogError(const char* szError) const;

		//! Returns memory statistics used for logging
		CvGameDatabaseAPI const char* CalculateMemoryStats();

		//! Validates foreign key constraints
		CvGameDatabaseAPI bool ValidateFKConstraints(bool bAllowNULL = true) const;

		CvGameDatabaseAPI void* operator new(size_t tSize);
		CvGameDatabaseAPI void operator delete(void* pMem);
		
		//BEGIN INTERNAL USE ONLY
		sqlite3* GetSQLite3() const;

		//! Error message handling 
		CvGameDatabaseAPI int ErrorCode() const;
		CvGameDatabaseAPI const char* ErrorMessage() const;

		//! Prepared statement cache management
		static bool AcquireCommand(Command& command, const char* szSQL, int lenSQL = -1);
		static void ReleaseCommand(Command& command);
		//END INTERNAL USE ONLY

	protected:
		static void InitMemoryManagement();

	private:
		//Degenerate copy and assignment
		Connection(const Connection&);
		Connection& operator=(const Connection&);

		static bool ms_bMemoryManagerInitialized;

		//! Pointer to sqlite3 database.
		sqlite3* m_db;

		//! Statistics buffer used for logging out memory stats
		std::string m_strMemoryStats;

		//! Hash map of count statements indexed by table name.
		std::tr1::unordered_map<std::string, sqlite3_stmt*> m_hshCountStatements;
		std::tr1::unordered_map<std::string, int> m_hshCountValues;

		static char ms_pPageCacheBuffer[DB_PAGECACHE_SIZE * DB_NUM_PAGES];
		static char ms_pScratchBuffer[DB_PAGECACHE_SIZE * DB_NUM_THREADS * 6];	

		mutable std::auto_ptr<IDatabaseLogger> m_pkDatabaseLogger;
	};
}