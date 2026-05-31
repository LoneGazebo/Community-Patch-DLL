/*	-------------------------------------------------------------------------------------------------------
	SqliteLogger - lightweight singleton for writing gameplay statistics to a local SQLite database.

	This utility mirrors the existing CSV statistics logging (e.g. WorldState_Log.csv) but writes
	structured rows into a single local "stats.db" SQLite database living in the game cache folder.

	All database work is a no-op unless the MOD_SQLITE_LOGGING mod option is enabled, so the logger is
	always safe to call from gameplay code without additional guards.

	The logger is C++03 compatible and is reached through the GET_SQLITE_LOGGER() convenience macro,
	following the same style as GET_PLAYER / GET_TEAM.

	---------------------------------------------------------------------------------------------------
	USAGE
	---------------------------------------------------------------------------------------------------

	1) REGISTER A TABLE

	   Every table is described once by a registration helper that lives in SqliteLoggerRegistrations.h.
	   Keeping all helpers in that single header lets any gameplay file write to a table by including
	   one import, without duplicating the schema across translation units. Add a helper there using the
	   run-once pattern below, then include the header wherever you log:

	       static void RegisterTechChoicesTable()
	       {
	           if (!MOD_SQLITE_LOGGING)
	               return;

	           static bool bRegistered = false;
	           if (!bRegistered)
	           {
	               bRegistered = true;
	               TableDef kColumns;
	               kColumns.push_back(ColumnDef("Civ",        Database::COLTYPE_TEXT));
	               kColumns.push_back(ColumnDef("Technology", Database::COLTYPE_TEXT));
	               kColumns.push_back(ColumnDef("Action",     Database::COLTYPE_TEXT));
	               GET_SQLITE_LOGGER().RegisterTable("TechChoices", kColumns);
	           }
	       }

	   You declare only your own columns. The logger automatically prepends two leading columns to every
	   table, which you must NOT declare and must NOT bind:

	       GameId  INT    -- a compact id referencing the shared uuid_dictionary table (see below)
	       Turn    INT    -- GC.getGame().getElapsedGameTurns(), the current game turn

	   So the table created above is actually: GameId, Turn, Civ, Technology, Action.

	   GameId is an integer key into a shared "uuid_dictionary" table that maps each game's full UUID to
	   a small auto-incremented integer. Stat rows store that compact id instead of repeating the full
	   UUID string; the mapping is resolved for you via ResolveGameId() on every write.

	2) LOG A ROW

	   Call the matching Register*() helper (cheap after the first call), then BeginLogRow() and bind one
	   value per column you declared, in declaration order. The implicit GameId and Turn values are bound
	   for you, so you begin with your first declared column and finish with execute():

	       if (MOD_SQLITE_LOGGING)
	       {
	           RegisterTechChoicesTable();
	           GET_SQLITE_LOGGER().BeginLogRow("TechChoices")
	               .bind(strPlayerName.c_str())   // Civ
	               .bind(szTechType)              // Technology
	               .bind("CHOSEN")                // Action
	               .execute();
	       }

	3) COLUMN TYPES

	   Booleans and ints are distinct column types. A COLTYPE_BOOL column expects bind(bool):

	       TableDef kFlags;
	       kFlags.push_back(ColumnDef("AtWar",  Database::COLTYPE_BOOL));
	       GET_SQLITE_LOGGER().RegisterTable("WarLog", kFlags); // -> GameId, Turn, AtWar

	       GET_SQLITE_LOGGER().BeginLogRow("WarLog").bind(true).execute();

	4) MISUSE IS SAFE

	   Misuse is caught and turned into a safe no-op (an assert fires in debug builds):

	       // Wrong type for the first declared column (expected TEXT, got int) -> row is not written:
	       GET_SQLITE_LOGGER().BeginLogRow("TechChoices").bind(42); // ...invalid

	       // Too few binds -> execute() refuses to write:
	       GET_SQLITE_LOGGER().BeginLogRow("TechChoices").bind("France").bind("TECH_POTTERY").execute(); // fails

	       // Unregistered table -> BeginLogRow returns a disabled statement that does nothing:
	       GET_SQLITE_LOGGER().BeginLogRow("noSuchTable").bind(1).execute(); // no-op

	       // Duplicate column names (including re-declaring the implicit GameId/Turn) -> registration
	       // is refused and the table is not created:
	       TableDef kBad;
	       kBad.push_back(ColumnDef("Turn", Database::COLTYPE_INT)); // clashes with implicit Turn
	       GET_SQLITE_LOGGER().RegisterTable("badTable", kBad); // no-op

	---------------------------------------------------------------------------------------------------
	BEHAVIOR NOTES
	---------------------------------------------------------------------------------------------------
	  - Every table implicitly begins with GameId (INT) and Turn (INT). They are added by
	    RegisterTable() and bound by BeginLogRow() automatically; callers neither declare nor bind them.
	    The GameId value is resolved through ResolveGameId() into the uuid_dictionary table. That table
	    stores each game's UUID as a 32-char uppercase hex string (dashes stripped) in a TEXT column,
	    mapped to the compact integer GameId that stat rows actually reference.
	  - Column names must be unique (case-insensitive) across the implicit and caller-supplied columns.
	    A duplicate name causes RegisterTable() to assert and refuse to create the table.
	  - RegisterTable() creates the table in stats.db if it does not exist. If an existing table's
	    schema differs from the supplied TableDef (including the implicit columns), the table is
	    dropped and recreated.
	  - When MOD_SQLITE_LOGGING is disabled, RegisterTable()/BeginLogRow() do nothing and stats.db is
	    never created or modified.
	------------------------------------------------------------------------------------------------------- */

#pragma once

#ifndef CIV5_SQLITE_LOGGER_H
#define CIV5_SQLITE_LOGGER_H

#include <string>
#include <vector>
#include <map>

#include "CvGameDatabase.h"

//! Convenience accessor for the SqliteLogger singleton (mirrors GET_PLAYER / GET_TEAM).
#define GET_SQLITE_LOGGER SqliteLogger::getInstance

//! Describes a single column in a logged table.
struct ColumnDef
{
	ColumnDef() : name(), type(Database::COLTYPE_NULL) {}
	ColumnDef(const std::string& name_, Database::ColumnTypes type_) : name(name_), type(type_) {}

	std::string name;
	Database::ColumnTypes type;
};

//! Ordered list of columns describing a table's schema.
typedef std::vector<ColumnDef> TableDef;

//! Singleton manager of the local stats.db SQLite database.
class SqliteLogger
{
public:
	//! Fluent insert builder returned by SqliteLogger::BeginLogRow.
	//! Each bind() call validates the value against the next expected column type and chains.
	//! execute() writes the row, or becomes a no-op if the statement is invalid/incomplete.
	class Statement
	{
	public:
		Statement(SqliteLogger* pkOwner, const std::string& strTableName, Database::Results* pkResults, const TableDef* pkSchema, bool bEnabled);

		Statement& bind(int iValue);
		Statement& bind(bool bValue);
		Statement& bind(float fValue);
		Statement& bind(double dValue);
		Statement& bind(const char* szValue);
		Statement& bind(const std::string& strValue);

		void execute();

	private:
		//! Validates that the next expected column matches eActual; advances on success.
		bool expectColumn(Database::ColumnTypes eActual);

		SqliteLogger* m_pkOwner;
		std::string m_strTableName;
		Database::Results* m_pkResults;
		const TableDef* m_pkSchema;
		bool m_bEnabled;
		bool m_bValid;
		int m_iBindIndex;
	};

	//! Returns the process-wide singleton instance.
	static SqliteLogger& getInstance();

	//! True when SQLite logging is active (MOD_SQLITE_LOGGING enabled and database opened).
	bool IsEnabled();

	//! Resolves a game UUID (32-char uppercase hex, dashes stripped) to its compact integer id in the
	//! uuid_dictionary table, inserting it if necessary. Returns 0 when logging is disabled or on error.
	int ResolveGameId(const std::string& strUuidHex);

	//! Registers (and creates/recreates if needed) a table with the given schema. No-op when disabled.
	//! Implicit leading GameId (INT), mapping to GUID, and Turn (INT) columns are prepended automatically; callers must
	//! not include them in kColumns. Duplicate column names (case-insensitive) are rejected.
	void RegisterTable(const std::string& strTableName, const TableDef& kColumns);

	//! Begins a row insert for a previously registered table. Returns a disabled Statement on error.
	//! The implicit GameId and Turn values are bound automatically, so the caller binds only its own
	//! columns, starting with the first column it declared in RegisterTable().
	Statement BeginLogRow(const std::string& strTableName);

private:
	SqliteLogger();
	~SqliteLogger();
	SqliteLogger(const SqliteLogger&);
	SqliteLogger& operator=(const SqliteLogger&);

	//! Lazily opens stats.db. Returns true if the database is open and logging is enabled.
	bool EnsureOpen();

	//! Creates the uuid_dictionary table if it does not already exist.
	void EnsureDictionaryTable();

	//! Compares the live table schema (via PRAGMA table_info) against the supplied definition.
	bool TableSchemaMatches(const std::string& strTableName, const TableDef& kColumns);

	//! Issues CREATE TABLE for the supplied definition.
	void CreateTable(const std::string& strTableName, const TableDef& kColumns);

	//! Builds and caches the prepared INSERT statement for a registered table.
	Database::Results* GetOrCreateInsert(const std::string& strTableName);

	//! Maps a ColumnTypes value to its SQLite declared type name.
	static const char* SqlTypeName(Database::ColumnTypes eType);

	typedef std::map<std::string, TableDef> SchemaMap;
	typedef std::map<std::string, Database::Results*> InsertMap;

	Database::Connection m_kConnection;
	bool m_bOpenAttempted;
	bool m_bOpen;

	SchemaMap m_registeredTableSchemas;
	InsertMap m_preparedInserts;
};

#endif // CIV5_SQLITE_LOGGER_H
