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
	BATCHED (TRANSACTIONAL) LOGGING
	---------------------------------------------------------------------------------------------------

	BeginLogRow()/execute() writes one row per call, which is ideal for occasional, low-volume logging.
	When you need to write many rows at once (for example, one row per map plot every turn), prefer the
	batched writer instead. It buffers rows in memory and writes them to stats.db in a single SQLite
	transaction, amortizing the per-row commit overhead across the whole batch.

	1) BEGIN A BATCH, ADD ROWS, FLUSH

	   Register the table exactly as above, then open a batch with BeginLogBatch(). For each row, call
	   BeginLogRow() on the batch, bind one value per declared column (in declaration order, exactly like
	   the single-row API), and finish the row with addRowToBatch() instead of execute(). When you are
	   done, call flush() to write whatever remains buffered:

	       if (MOD_SQLITE_LOGGING)
	       {
	           RegisterMapPlotsStateTable();
	           SqliteLogger::BatchWriter kBatch = GET_SQLITE_LOGGER().BeginLogBatch("MapPlotsState");
	           for (int i = 0; i < GC.getMap().numPlots(); ++i)
	           {
	               CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(i);
	               kBatch.BeginLogRow()
	                   .bind(pPlot->getX())          // plotX
	                   .bind(pPlot->getY())          // plotY
	                   .bind(szCityName)             // cityName
	                   .bind(szOwner)                // owner
	                   .bind(iRouteType)             // routeType
	                   .addRowToBatch();
	           }
	           kBatch.flush();                       // REQUIRED: writes any rows still buffered
	       }

	   The implicit GameId and Turn values are captured once when the batch is opened and reused for
	   every row, so (as with the single-row API) you never bind them yourself.

	2) WHEN ROWS ARE ACTUALLY WRITTEN

	   addRowToBatch() does not necessarily touch the database. Buffered rows are written to stats.db
	   (inside one transaction) at two moments:
	     - automatically, when the buffer reaches the SQLITE_LOGGING_BATCHED_BUFFER_ROWS_MAX row cap; and
	     - when you call flush() (which also runs whatever is left after the last auto-flush).

	   SQLITE_LOGGING_BATCHED_BUFFER_ROWS_MAX is a numeric Define (default 1024). Higher values buffer
	   more rows in memory before writing, which reduces total write time by paying the transaction cost
	   less often, at the expense of a larger peak memory footprint; lower values use less memory but
	   write more frequently. Because this DLL is 32-bit and memory-constrained in the late game, the
	   buffer is intentionally bounded by this cap rather than growing without limit.

	3) YOU MUST FLUSH BEFORE THE WRITER GOES OUT OF SCOPE

	   The BatchWriter does NOT flush automatically when it is destroyed: any rows still buffered at that
	   point are silently discarded. Always call flush() once you have finished adding rows (and before
	   the BatchWriter leaves scope) or those final rows will be lost. flush() is safe to call when the
	   buffer is empty and safe to call more than once.

	4) MISUSE IS SAFE

	   Like the single-row API, batched misuse is caught and turned into a safe no-op (with a debug
	   assert): binding the wrong type for a column, binding too many or too few values before
	   addRowToBatch(), or opening a batch for an unregistered table all skip the offending row (or the
	   whole batch) instead of writing bad data.

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
	  - BeginLogBatch() likewise returns a disabled, do-nothing BatchWriter when logging is disabled or
	    the table is not registered. A BatchWriter buffers rows and only writes them on an auto-flush
	    (at the SQLITE_LOGGING_BATCHED_BUFFER_ROWS_MAX cap) or an explicit flush(); rows still buffered
	    when it is destroyed are discarded, so callers must flush() before it leaves scope.
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

	//! Batched insert helper returned by SqliteLogger::BeginLogBatch.
	//! Rows are buffered in memory and written to stats.db in a single transaction, either when the
	//! buffer reaches SQLITE_LOGGING_BATCHED_BUFFER_ROWS_MAX rows or when flush() is called. The
	//! implicit GameId and Turn values are captured once for the whole batch, so every row shares them.
	//! NOTE: any rows still buffered when the BatchWriter is destroyed are silently discarded; callers
	//! are responsible for calling flush() before the writer goes out of scope.
	class BatchWriter
	{
	public:
		//! One buffered cell. Numeric values use the scalar members; TEXT values use strValue.
		//! Kept as a single flat type so a whole batch lives in one contiguous vector (low overhead,
		//! bounded by SQLITE_LOGGING_BATCHED_BUFFER_ROWS_MAX rows).
		struct BatchCell
		{
			BatchCell() : eType(Database::COLTYPE_NULL), iValue(0), dValue(0.0), strValue() {}
			Database::ColumnTypes eType;
			int iValue;            //!< INT / BOOL payload
			double dValue;         //!< FLOAT payload
			std::string strValue;  //!< TEXT payload
		};

		//! Fluent per-row builder. bind() validates each value against the next caller column and
		//! chains; addRowToBatch() appends the completed row to the batch (auto-flushing if full).
		class RowBuilder
		{
		public:
			RowBuilder& bind(int iValue);
			RowBuilder& bind(bool bValue);
			RowBuilder& bind(float fValue);
			RowBuilder& bind(double dValue);
			RowBuilder& bind(const char* szValue);
			RowBuilder& bind(const std::string& strValue);

			//! Commits the row currently being built to the in-memory batch buffer. If the buffer
			//! reaches its configured maximum, the whole batch is flushed in one transaction.
			void addRowToBatch();

		private:
			friend class BatchWriter;
			RowBuilder(BatchWriter* pkBatch, bool bEnabled);

			//! Validates that the next expected caller column matches eActual; advances on success.
			bool expectColumn(Database::ColumnTypes eActual);

			BatchWriter* m_pkBatch;
			bool m_bEnabled;
			bool m_bValid;
			int m_iBindIndex;
		};

		BatchWriter(SqliteLogger* pkOwner, const std::string& strTableName, const TableDef* pkSchema, Database::Results* pkResults, int iGameId, int iTurn, int iMaxRows, bool bEnabled);

		//! Begins building a new row. Bind one value per caller-declared column, then addRowToBatch().
		RowBuilder BeginLogRow();

		//! Writes all currently buffered rows to stats.db in a single transaction. Safe to call when
		//! the buffer is empty (no-op) and safe to call repeatedly.
		void flush();

	private:
		friend class RowBuilder;

		//! Number of caller-declared columns (schema size minus the implicit GameId/Turn columns).
		int CallerColumnCount() const;

		SqliteLogger* m_pkOwner;
		std::string m_strTableName;
		const TableDef* m_pkSchema;
		Database::Results* m_pkResults;
		int m_iGameId;
		int m_iTurn;
		int m_iMaxRows;
		bool m_bEnabled;

		std::vector<BatchCell> m_buffer;   //!< Completed rows, flattened (rows * caller columns).
		std::vector<BatchCell> m_scratch;  //!< Cells for the row currently being built.
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

	//! Begins a batched (transactional) insert for a previously registered table. Buffered rows are
	//! written in a single transaction when the buffer is full or flush() is called. Returns a disabled
	//! BatchWriter on error. The implicit GameId and Turn values are captured once for the whole batch.
	BatchWriter BeginLogBatch(const std::string& strTableName);

private:
	friend class BatchWriter;

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
