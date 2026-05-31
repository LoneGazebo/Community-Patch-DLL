/*	-------------------------------------------------------------------------------------------------------
	SqliteLogger implementation. See SqliteLogger.h for documentation and usage examples.
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "SqliteLogger.h"
#include "CustomMods.h"

#include <string.h>

// must be included after all other headers
#include "LintFree.h"

//	-----------------------------------------------------------------------------------------------
SqliteLogger::SqliteLogger() :
	m_kConnection(),
	m_bOpenAttempted(false),
	m_bOpen(false),
	m_registeredTableSchemas(),
	m_preparedInserts()
{
}

//	-----------------------------------------------------------------------------------------------
SqliteLogger::~SqliteLogger()
{
	for (InsertMap::iterator it = m_preparedInserts.begin(); it != m_preparedInserts.end(); ++it)
	{
		delete it->second;
	}
	m_preparedInserts.clear();

	if (m_bOpen)
	{
		m_kConnection.Close();
		m_bOpen = false;
	}
}

//	-----------------------------------------------------------------------------------------------
SqliteLogger& SqliteLogger::getInstance()
{
	static SqliteLogger s_kInstance;
	return s_kInstance;
}

//	-----------------------------------------------------------------------------------------------
const char* SqliteLogger::SqlTypeName(Database::ColumnTypes eType)
{
	switch (eType)
	{
	case Database::COLTYPE_INT:
		return "INTEGER";
	case Database::COLTYPE_FLOAT:
		return "REAL";
	case Database::COLTYPE_TEXT:
		return "TEXT";
	case Database::COLTYPE_BOOL:
		return "BOOLEAN";
	case Database::COLTYPE_NULL:
	default:
		return "NULL";
	}
}

//	-----------------------------------------------------------------------------------------------
bool SqliteLogger::EnsureOpen()
{
	if (!MOD_SQLITE_LOGGING)
		return false;

	if (m_bOpen)
		return true;

	if (m_bOpenAttempted)
		return false;

	m_bOpenAttempted = true;

	CvString strDatabasePath = gDLL->GetCacheFolderPath();
	strDatabasePath += "stats.db";

	if (m_kConnection.Open(strDatabasePath.c_str(), Database::OPEN_CREATE | Database::OPEN_READWRITE | Database::OPEN_FULLMUTEX))
	{
		m_bOpen = true;
		EnsureDictionaryTable();
	}
	else
	{
		ASSERT(false, "SqliteLogger: failed to open stats.db");
		m_bOpen = false;
	}

	return m_bOpen;
}

//	-----------------------------------------------------------------------------------------------
void SqliteLogger::EnsureDictionaryTable()
{
	if (!m_bOpen)
		return;

	if (!m_kConnection.Execute(
		"CREATE TABLE IF NOT EXISTS uuid_dictionary ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"uuid_hex TEXT UNIQUE NOT NULL)"))
	{
		ASSERT(false, "SqliteLogger: failed to create uuid_dictionary table");
	}
}

//	-----------------------------------------------------------------------------------------------
int SqliteLogger::ResolveGameId(const std::string& strUuidHex)
{
	if (!EnsureOpen())
		return 0;

	if (strUuidHex.empty())
		return 0;

	// Insert the UUID if it is not already present. The UNIQUE constraint makes this idempotent.
	{
		Database::Results kInsert;
		if (m_kConnection.Execute(kInsert, "INSERT OR IGNORE INTO uuid_dictionary (uuid_hex) VALUES (?)"))
		{
			kInsert.Bind(1, strUuidHex.c_str());
			kInsert.Execute();
			kInsert.Reset();
		}
	}

	// Look up the (now guaranteed) row to retrieve its auto-incremented id.
	int iId = 0;
	{
		Database::Results kSelect;
		if (m_kConnection.Execute(kSelect, "SELECT id FROM uuid_dictionary WHERE uuid_hex = ?"))
		{
			kSelect.Bind(1, strUuidHex.c_str());
			if (kSelect.Step())
				iId = kSelect.GetInt(0);
			kSelect.Reset();
		}
	}

	return iId;
}

//	-----------------------------------------------------------------------------------------------
bool SqliteLogger::IsEnabled()
{
	return EnsureOpen();
}

//	-----------------------------------------------------------------------------------------------
bool SqliteLogger::TableSchemaMatches(const std::string& strTableName, const TableDef& kColumns)
{
	std::string strSql = "PRAGMA table_info(\"";
	strSql += strTableName;
	strSql += "\")";

	Database::Results kResults;
	if (!m_kConnection.Execute(kResults, strSql.c_str()))
		return false;

	size_t uiColumn = 0;
	while (kResults.Step())
	{
		// PRAGMA table_info columns: 0=cid, 1=name, 2=type, ...
		const char* szName = kResults.GetText(1);
		const char* szType = kResults.GetText(2);

		if (uiColumn >= kColumns.size())
		{
			// Live table has more columns than expected.
			kResults.Reset();
			return false;
		}

		const ColumnDef& kExpected = kColumns[uiColumn];
		if (szName == NULL || _stricmp(szName, kExpected.name.c_str()) != 0)
		{
			kResults.Reset();
			return false;
		}
		if (szType == NULL || _stricmp(szType, SqlTypeName(kExpected.type)) != 0)
		{
			kResults.Reset();
			return false;
		}

		++uiColumn;
	}

	kResults.Reset();

	// Match only if every expected column was found and there were no extras.
	return (uiColumn == kColumns.size()) && (uiColumn > 0);
}

//	-----------------------------------------------------------------------------------------------
void SqliteLogger::CreateTable(const std::string& strTableName, const TableDef& kColumns)
{
	std::string strSql = "CREATE TABLE \"";
	strSql += strTableName;
	strSql += "\" (";

	for (size_t i = 0; i < kColumns.size(); ++i)
	{
		if (i > 0)
			strSql += ", ";

		strSql += "\"";
		strSql += kColumns[i].name;
		strSql += "\" ";
		strSql += SqlTypeName(kColumns[i].type);
	}

	strSql += ")";

	if (!m_kConnection.Execute(strSql.c_str()))
	{
		ASSERT(false, "SqliteLogger: failed to create table");
	}
}

//	-----------------------------------------------------------------------------------------------
void SqliteLogger::RegisterTable(const std::string& strTableName, const TableDef& kColumns)
{
	if (!EnsureOpen())
		return;

	if (kColumns.empty())
	{
		ASSERT(false, "SqliteLogger: cannot register a table with no columns");
		return;
	}

	// Every table implicitly starts with a GameId (INT) and Turn (INT) column. These are
	// prepended to the caller-supplied schema here and bound automatically in BeginLogRow(),
	// so callers must neither declare nor bind them. GameId is an integer id into uuid_dictionary.
	TableDef kFullColumns;
	kFullColumns.reserve(kColumns.size() + 2);
	kFullColumns.push_back(ColumnDef("GameId", Database::COLTYPE_INT));
	kFullColumns.push_back(ColumnDef("Turn", Database::COLTYPE_INT));
	for (size_t i = 0; i < kColumns.size(); ++i)
		kFullColumns.push_back(kColumns[i]);

	// Reject duplicate column names (case-insensitive). This also catches callers that try to
	// re-declare the implicit GameId/Turn columns.
	for (size_t i = 0; i < kFullColumns.size(); ++i)
	{
		for (size_t j = i + 1; j < kFullColumns.size(); ++j)
		{
			if (_stricmp(kFullColumns[i].name.c_str(), kFullColumns[j].name.c_str()) == 0)
			{
				ASSERT(false, "SqliteLogger: duplicate column name in table definition (note: GameId and Turn are implicit and must not be declared)");
				return;
			}
		}
	}

	// Drop any cached prepared insert; it may be stale if we recreate the table below.
	InsertMap::iterator itInsert = m_preparedInserts.find(strTableName);
	if (itInsert != m_preparedInserts.end())
	{
		delete itInsert->second;
		m_preparedInserts.erase(itInsert);
	}

	if (!TableSchemaMatches(strTableName, kFullColumns))
	{
		// Table is missing or has a different schema: drop (if present) and (re)create.
		std::string strDrop = "DROP TABLE IF EXISTS \"";
		strDrop += strTableName;
		strDrop += "\"";
		m_kConnection.Execute(strDrop.c_str());

		CreateTable(strTableName, kFullColumns);
	}

	m_registeredTableSchemas[strTableName] = kFullColumns;
}

//	-----------------------------------------------------------------------------------------------
Database::Results* SqliteLogger::GetOrCreateInsert(const std::string& strTableName)
{
	InsertMap::iterator it = m_preparedInserts.find(strTableName);
	if (it != m_preparedInserts.end())
		return it->second;

	SchemaMap::iterator itSchema = m_registeredTableSchemas.find(strTableName);
	if (itSchema == m_registeredTableSchemas.end())
		return NULL;

	const TableDef& kColumns = itSchema->second;

	std::string strSql = "INSERT INTO \"";
	strSql += strTableName;
	strSql += "\" VALUES (";
	for (size_t i = 0; i < kColumns.size(); ++i)
	{
		if (i > 0)
			strSql += ",";
		strSql += "?";
	}
	strSql += ")";

	Database::Results* pkResults = new Database::Results();
	if (!m_kConnection.Execute(*pkResults, strSql.c_str()))
	{
		ASSERT(false, "SqliteLogger: failed to prepare insert statement");
		delete pkResults;
		return NULL;
	}

	m_preparedInserts[strTableName] = pkResults;
	return pkResults;
}

//	-----------------------------------------------------------------------------------------------
SqliteLogger::Statement SqliteLogger::BeginLogRow(const std::string& strTableName)
{
	if (!EnsureOpen())
		return Statement(this, strTableName, NULL, NULL, false);

	SchemaMap::iterator itSchema = m_registeredTableSchemas.find(strTableName);
	if (itSchema == m_registeredTableSchemas.end())
	{
		ASSERT(false, "SqliteLogger: BeginLogRow called for an unregistered table");
		return Statement(this, strTableName, NULL, NULL, false);
	}

	Database::Results* pkResults = GetOrCreateInsert(strTableName);
	if (pkResults == NULL)
		return Statement(this, strTableName, NULL, NULL, false);

	// Clear any state/bindings from the previous row before starting a new one.
	pkResults->Reset();

	// The implicit GameId and Turn columns lead every table; bind them automatically here so that
	// callers only ever bind their own columns (in declaration order).
	Statement kStatement(this, strTableName, pkResults, &itSchema->second, true);
	kStatement.bind(GC.getGame().getGameDatabaseId());
	kStatement.bind(GC.getGame().getElapsedGameTurns());
	return kStatement;
}

//	===============================================================================================
//	SqliteLogger::Statement
//	===============================================================================================

//	-----------------------------------------------------------------------------------------------
SqliteLogger::Statement::Statement(SqliteLogger* pkOwner, const std::string& strTableName, Database::Results* pkResults, const TableDef* pkSchema, bool bEnabled) :
	m_pkOwner(pkOwner),
	m_strTableName(strTableName),
	m_pkResults(pkResults),
	m_pkSchema(pkSchema),
	m_bEnabled(bEnabled),
	m_bValid(bEnabled),
	m_iBindIndex(0)
{
}

//	-----------------------------------------------------------------------------------------------
bool SqliteLogger::Statement::expectColumn(Database::ColumnTypes eActual)
{
	if (!m_bEnabled || !m_bValid)
		return false;

	if (m_pkSchema == NULL || m_iBindIndex >= (int)m_pkSchema->size())
	{
		ASSERT(false, "SqliteLogger: too many values bound for table");
		m_bValid = false;
		return false;
	}

	const Database::ColumnTypes eExpected = (*m_pkSchema)[m_iBindIndex].type;

	// The bind() overloads normalize float and double to COLTYPE_FLOAT, so an exact match is sufficient.
	const bool bMatch = (eExpected == eActual);

	if (!bMatch)
	{
		ASSERT(false, "SqliteLogger: bound value type does not match the registered column type");
		m_bValid = false;
		return false;
	}

	return true;
}

//	-----------------------------------------------------------------------------------------------
SqliteLogger::Statement& SqliteLogger::Statement::bind(int iValue)
{
	if (expectColumn(Database::COLTYPE_INT))
	{
		m_pkResults->Bind(m_iBindIndex + 1, iValue);
		++m_iBindIndex;
	}
	return *this;
}

//	-----------------------------------------------------------------------------------------------
SqliteLogger::Statement& SqliteLogger::Statement::bind(bool bValue)
{
	if (expectColumn(Database::COLTYPE_BOOL))
	{
		m_pkResults->Bind(m_iBindIndex + 1, bValue ? 1 : 0);
		++m_iBindIndex;
	}
	return *this;
}

//	-----------------------------------------------------------------------------------------------
SqliteLogger::Statement& SqliteLogger::Statement::bind(float fValue)
{
	if (expectColumn(Database::COLTYPE_FLOAT))
	{
		m_pkResults->Bind(m_iBindIndex + 1, fValue);
		++m_iBindIndex;
	}
	return *this;
}

//	-----------------------------------------------------------------------------------------------
SqliteLogger::Statement& SqliteLogger::Statement::bind(double dValue)
{
	if (expectColumn(Database::COLTYPE_FLOAT))
	{
		m_pkResults->Bind(m_iBindIndex + 1, dValue);
		++m_iBindIndex;
	}
	return *this;
}

//	-----------------------------------------------------------------------------------------------
SqliteLogger::Statement& SqliteLogger::Statement::bind(const char* szValue)
{
	if (expectColumn(Database::COLTYPE_TEXT))
	{
		m_pkResults->Bind(m_iBindIndex + 1, (szValue != NULL) ? szValue : "");
		++m_iBindIndex;
	}
	return *this;
}

//	-----------------------------------------------------------------------------------------------
SqliteLogger::Statement& SqliteLogger::Statement::bind(const std::string& strValue)
{
	if (expectColumn(Database::COLTYPE_TEXT))
	{
		m_pkResults->Bind(m_iBindIndex + 1, strValue.c_str());
		++m_iBindIndex;
	}
	return *this;
}

//	-----------------------------------------------------------------------------------------------
void SqliteLogger::Statement::execute()
{
	if (!m_bEnabled || !m_bValid || m_pkResults == NULL || m_pkSchema == NULL)
		return;

	if (m_iBindIndex != (int)m_pkSchema->size())
	{
		ASSERT(false, "SqliteLogger: execute() called before all columns were bound");
		return;
	}

	if (!m_pkResults->Execute())
	{
		ASSERT(false, "SqliteLogger: failed to execute insert");
	}

	// Leave the statement ready for the next row.
	m_pkResults->Reset();
}
