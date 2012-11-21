
#pragma once 

#include <list>
#include <hash_map>
#include <string>

namespace Database{

	enum ColumnTypes
	{
		COLTYPE_NULL	= 5,	//SQLITE_NULL
		COLTYPE_INT		= 1,	//SQLITE_INTEGER
		COLTYPE_FLOAT	= 2,	//SQLITE_FLOAT
		COLTYPE_TEXT	= 3,	//SQLITE_TEXT
		COLTYPE_BOOL	= 6,	//NON SQLITE ENUM
	};

	class Results 
	{
	public:
		CvGameDatabaseAPI Results(const char* szColumns = NULL);
		CvGameDatabaseAPI Results(const Results& copy);
		CvGameDatabaseAPI virtual ~Results();

		//! Retrieves the columns used by results (default: *).
		CvGameDatabaseAPI virtual const char* GetColumns() const;

		//! Sets the columns used by results.
		CvGameDatabaseAPI void SetColumns(const char* szColumns); 

		//! Step and Execute function exactly the same but are meant for different
		//! use cases.  Use Execute when you do not expect any rows to be returned.
		//! Use Step when you expect one or more rows to be returned.
		CvGameDatabaseAPI bool Execute();
		//! Similar to execute, but will not return an error for constraint failures.
		//! This is useful when using "insert or abort" commands.
		CvGameDatabaseAPI bool TryExecute();
		CvGameDatabaseAPI bool Step();

		//! Resets the statement and also clears all bound arguments.
		CvGameDatabaseAPI bool Reset();
		//! Resets the statement and also clears all bound arguments.
		CvGameDatabaseAPI bool TryReset();

		//! Releases the command used by the results.
		CvGameDatabaseAPI void Release();

		// Bind methods used to bind literal values

		//! Bind a UTF-8 string to the indexed parameter.
		//! If bMakeCopy is true, an internal copy of the string will be made.
		CvGameDatabaseAPI bool Bind(int idx, const char* szValue, int lenValue, bool bMakeCopy = true);
		CvGameDatabaseAPI bool Bind(int idx, const char* szValue, bool bMakeCopy = true);

		CvGameDatabaseAPI bool Bind(int idx, const wchar_t* wszValue, int lenValue, bool bMakeCopy = true);
		CvGameDatabaseAPI bool Bind(int idx, const wchar_t* wszValue, bool bMakeCopy = true);

		CvGameDatabaseAPI bool Bind(int idx, double dValue);
		CvGameDatabaseAPI bool Bind(int idx, float fValue);
		CvGameDatabaseAPI bool Bind(int idx, int iValue);
		CvGameDatabaseAPI bool Bind(int idx, __int64 iValue);
		CvGameDatabaseAPI bool BindNULL(int idx);
		
		CvGameDatabaseAPI int ColumnCount() const;

		CvGameDatabaseAPI const char* ColumnName(int iColumn);

		CvGameDatabaseAPI const char*	ColumnTypeName(int iColumn);
		CvGameDatabaseAPI ColumnTypes	ColumnType(int iColumn);

		CvGameDatabaseAPI bool	HasColumn(const char* szColumnName);
		CvGameDatabaseAPI int	ColumnPosition(const char* szColumnName);

		CvGameDatabaseAPI const char*		GetText(int iColumn);
		CvGameDatabaseAPI const wchar_t*	GetText16(int iColumn);
		CvGameDatabaseAPI bool				GetBool(int iColumn);
		CvGameDatabaseAPI int				GetInt(int iColumn);
		CvGameDatabaseAPI __int64			GetInt64(int iColumn);
		CvGameDatabaseAPI float				GetFloat(int iColumn);
		CvGameDatabaseAPI double			GetDouble(int iColumn);

		CvGameDatabaseAPI void GetValue(int iColumn, bool& bValue);
		CvGameDatabaseAPI void GetValue(int iColumn, int& iValue);
		CvGameDatabaseAPI void GetValue(int iColumn, __int64& iValue);
		CvGameDatabaseAPI void GetValue(int iColumn, float& fValue);
		CvGameDatabaseAPI void GetValue(int iColumn, double& dValue);
		CvGameDatabaseAPI void GetValue(int iColumn, const char*& zValue);
		CvGameDatabaseAPI void GetValue(int iColumn, const wchar_t*& wszValue);

		CvGameDatabaseAPI const char*		GetText(const char* szColumn);
		CvGameDatabaseAPI const wchar_t*	GetText16(const char* szColumn);
		CvGameDatabaseAPI bool				GetBool(const char* szColumn);
		CvGameDatabaseAPI int				GetInt(const char* szColumn);
		CvGameDatabaseAPI __int64			GetInt64(const char* szColumn);
		CvGameDatabaseAPI float				GetFloat(const char* szColumn);
		CvGameDatabaseAPI double			GetDouble(const char* szColumn);

		CvGameDatabaseAPI void GetValue(const char* szColumn, bool& bValue);
		CvGameDatabaseAPI void GetValue(const char* szColumn, int& iValue);
		CvGameDatabaseAPI void GetValue(const char* szColumn, __int64& iValue);
		CvGameDatabaseAPI void GetValue(const char* szColumn, float& fValue);
		CvGameDatabaseAPI void GetValue(const char* szColumn, double& dValue);
		CvGameDatabaseAPI void GetValue(const char* szColumn, const char*& zValue);
		CvGameDatabaseAPI void GetValue(const char* szColumn, const wchar_t*& wszValue);

		CvGameDatabaseAPI bool SingleQuery() const;
		CvGameDatabaseAPI void SingleQuery(bool bSingle);

		CvGameDatabaseAPI void* operator new(size_t tSize);
		CvGameDatabaseAPI void operator delete(void* pMem);

		//(INTERNAL USE ONLY)
		//! Attaches a SQLite3 statement to results.
		bool SetCommand(Command& command);

	protected:
		CvGameDatabaseAPI void HashColumnPositions();

	private:
		Command m_Command;
		const char* m_szColumns;
		bool m_bSingleQuery;

		stdext::hash_map<std::string, int> m_hshColumnPositions;
	};

	class SingleResult : public Results
	{
	public:
		CvGameDatabaseAPI SingleResult(const char* szColumns = NULL);
	};

} //namespace Database
