/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvDatabaseUtility.h
//!  \brief		Helper utility for GameDatabase.
//!
//!		This file the class definition of CvDatabaseUtility.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma once
#ifndef CVDATABASEUTILITY_H

class CvDatabaseUtility
{
public:
	CvDatabaseUtility();
	~CvDatabaseUtility();

	//! Clear stored results
	void ClearResults();
	void ClearResults(const std::string& strKey);

	//! Retrieves a cached result given an arbitrary string key
	Database::Results* GetResults(const std::string& strKey);

	//! Prepares a cached result given an arbitrary string key
	Database::Results* PrepareResults(const std::string& strKey, const char* szStmt);

	//!Allocates an array to count and zero's memory.
	void InitializeArray(int*& pArray, const size_t count, int iDefault = 0);
	void InitializeArray(bool*& pArray, const size_t count, bool bDefault = false);
	void InitializeArray(float*& pArray, const size_t count, float fDefault = 0.0f);
	void Initialize2DArray(int**& ppArray, const size_t iCount1, const size_t iCount2, int iDefault = 0);

	//!Allocates an array to Count(szTableName) and zero's memory.
	template<typename T>
void InitializeArray(T*& pArray, const char* szTableName, T default = (T)0);

	bool Initialize2DArray(int**& pArray, const char* szTable1Name, const char* szTable2Name, int iDefault = 0);
	static void SafeDelete2DArray(int**& pArray);


	//!Allocates an array to Count(szTypeTableName) and initializes to false.
	//!Then assigns pArray[Type.ID] = true for all resulting rows.
	bool PopulateArrayByExistence(bool*& pArray,	const char* szTypeTableName,
	                              const char* szDataTableName,
	                              const char* szTypeColumn,
	                              const char* szFilterColumn,
	                              const char* szFilterValue);

	//!Allocates an array to Count(szTypeTableName) and initializes to false.
	//!Then assigns pArray[i++] = Type.ID for all resulting rows.
	bool PopulateArrayByExistence(int*& pArray,	const char* szTypeTableName,
	                              const char* szDataTableName,
	                              const char* szTypeColumn,
	                              const char* szFilterColumn,
	                              const char* szFilterValue);

	//!Allocates an array to Count(szTypeTableName) and initializes to 0
	//!Then assigns pArray[Type.ID] = value for all resulting rows.
	bool PopulateArrayByValue(int*& pArray, const char* szTypeTableName,
	                          const char* szDataTableName,
	                          const char* szTypeColumn,
	                          const char* szFilterColumn,
	                          const char* szFilterValue,
	                          const char* szValueColumn,
							  int iDefaultValue = 0,
							  int iMinArraySize = 0);


	//------------------------------------------------------------------------------
	// Tables in Civ5 commonly have a Flavors array.
	// This method fetches that flavor data into an integer array.
	// CONDITIONS:
	//	*'Flavors' table must exist.
	//	*FlavorTable must have a 'FlavorType' column.
	// RETURNS:
	//	True on success.
	bool SetFlavors(int*& pFlavorsArray, const char* szTableName, const char* szFilterColumn, const char* szFilterValue, int iDefaultValue = 0);

	//------------------------------------------------------------------------------
	// Tables in Civ5 commonly have a Yields array.
	// This method fetches that yield data into an integer array.
	// CONDITIONS:
	//	*'Yields' table must exist.
	//	*YieldTable must have a 'YieldType' column.
	// RETURNS:
	//	True on success.
	bool SetYields(int*& pYieldsArray, const char* szTableName, const char* szFilterColumn, const char* szFilterValue);

	//! Retrieves the maximum number of a given column for a given table.
	int MaxRows(const char* szTableName);

	//! Returns the most recent database error message.
	const char* GetErrorMessage() const;

private:
	typedef stdext::hash_map<std::string, Database::Results*> ResultsMap;
	ResultsMap m_storedResults;
};

//------------------------------------------------------------------------------
// inline and template members
//------------------------------------------------------------------------------
inline void CvDatabaseUtility::InitializeArray(int*& pArray, const size_t count, int iDefault)
{
	CvAssertMsg(count > 0, "Initializing array to 0 or less items.");
	pArray = FNEW(int[count], c_eCiv5GameplayDLL, 0);
	if(iDefault == 0)
	{
		ZeroMemory(pArray, sizeof(int) * count);
	}
	else
	{
		for(size_t i = 0; i < count; ++i)
			pArray[i] = iDefault;
	}
}
//------------------------------------------------------------------------------
inline void CvDatabaseUtility::InitializeArray(bool*& pArray, const size_t count, bool bDefault)
{
	CvAssertMsg(count > 0, "Initializing array to 0 or less items.");
	pArray = FNEW(bool[count], c_eCiv5GameplayDLL, 0);
	if(bDefault == 0.0f)
	{
		ZeroMemory(pArray, sizeof(bool) * count);
	}
	else
	{
		for(size_t i = 0; i < count; ++i)
			pArray[i] = bDefault;
	}
}
//------------------------------------------------------------------------------
inline void CvDatabaseUtility::InitializeArray(float*& pArray, const size_t count, float fDefault)
{
	CvAssertMsg(count > 0, "Initializing array to 0 or less items.");
	pArray = FNEW(float[count], c_eCiv5GameplayDLL, 0);
	if(fDefault == 0.0f)
	{
		ZeroMemory(pArray, sizeof(float) * count);
	}
	else
	{
		for(size_t i = 0; i < count; ++i)
			pArray[i] = fDefault;
	}
}
//------------------------------------------------------------------------------
template<typename T>
inline void CvDatabaseUtility::InitializeArray(T*& pArray, const char* szTableName, T default)
{
	size_t count = MaxRows(szTableName);

	//This is a bit of a hack to support empty tables.
	//For example, the "Automates" table will be empty during a tutorial.
	//We cannot simply NULL out the array because WAY too much code blindly assumes SOMETHING will be there..
	//Instead we allocate room for 1 element and just roll with it.

	//CvAssertMsg(count > 0, DB.ErrorMessage());
	if(count == 0)
		count = 1;

	InitializeArray(pArray, count, default);
}

#endif //CVDATABASEUTILITY_H