/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvDatabaseUtility.h"

// must be included after all other headers
#include "LintFree.h"

CvDatabaseUtility::CvDatabaseUtility()
{

}
//------------------------------------------------------------------------------
CvDatabaseUtility::~CvDatabaseUtility()
{
	//Remove any cached statements
	ClearResults();
}
//------------------------------------------------------------------------------
void CvDatabaseUtility::ClearResults()
{
	for(ResultsMap::const_iterator it = m_storedResults.begin(); it != m_storedResults.end(); ++it)
	{
		Database::Results* pResults = it->second;
		delete pResults;
	}

	m_storedResults.clear();
}
//------------------------------------------------------------------------------
void CvDatabaseUtility::ClearResults(const std::string& strKey)
{
	Database::Results* pResults = GetResults(strKey);
	if(pResults)
	{
		delete pResults;
		m_storedResults.erase(strKey);
	}
}
//------------------------------------------------------------------------------
Database::Results* CvDatabaseUtility::GetResults(const std::string& strKey)
{
	ResultsMap::const_iterator it = m_storedResults.find(strKey);
	if(it != m_storedResults.end())
	{
		it->second->Reset();
		return it->second;
	}

	return NULL;
}
//------------------------------------------------------------------------------
Database::Results* CvDatabaseUtility::PrepareResults(const std::string& strKey, const char* szStmt)
{
	Database::Results* pResults = new Database::Results();

	if(DB.Execute(*pResults, szStmt))
	{
		Database::Results* pOldResults = GetResults(strKey);
		if(pOldResults)
			delete pOldResults;

		m_storedResults[strKey] = pResults;
		return pResults;
	}
	else
	{
		delete pResults;
	}

	return NULL;
}
//------------------------------------------------------------------------------
bool CvDatabaseUtility::Initialize2DArray(int**&ppArray, const char* szTable1Name, const char* szTable2Name, int iDefault /* = 0 */)
{
	const int iCount1 = MaxRows(szTable1Name);
	const int iCount2 = MaxRows(szTable2Name);

	if(iCount1 <= 0 || iCount2 <= 0)
	{
		ppArray = NULL;
		CvAssertMsg(false, "Cannot initialize array to 0 size.");
		return false;
	}

	unsigned int iNumBytes = iCount1 * sizeof(int*) + iCount1 * iCount2 * sizeof(int);
	unsigned char* pData = FNEW( unsigned char[iNumBytes], c_eCiv5GameplayDLL, 0 );
	ppArray = (int**)pData;
	ppArray[0] = (int*)( pData + iCount1 * sizeof(int*) );
	for( int i = 0; i < iCount2; ++i )
	{
		ppArray[0][i] = iDefault;
	}
	for( int i = 1; i < iCount1; i++ )
	{
		ppArray[i] = ppArray[i-1] + iCount2;
		for( int j = 0; j < iCount2; ++j )
		{
			ppArray[i][j] = iDefault;
		}
	}

	return true;
}
//------------------------------------------------------------------------------
void CvDatabaseUtility::Initialize2DArray(int**& ppArray, const size_t iCount1, const size_t iCount2, int iDefault /*= 0*/ )
{
	if(iCount1 <= 0 || iCount2 <= 0)
	{
		ppArray = NULL;
		CvAssertMsg(false, "Cannot initialize array to 0 size.");
		return;
	}

	unsigned int iNumBytes = iCount1 * sizeof(int*) + iCount1 * iCount2 * sizeof(int);
	unsigned char* pData = FNEW( unsigned char[iNumBytes], c_eCiv5GameplayDLL, 0 );
	ppArray = (int**)pData;
	ppArray[0] = (int*)( pData + iCount1 * sizeof(int*) );
	for( unsigned int i = 0; i < iCount2; ++i )
	{
		ppArray[0][i] = iDefault;
	}
	for( unsigned int i = 1; i < iCount1; i++ )
	{
		ppArray[i] = ppArray[i-1] + iCount2;
		for( unsigned int j = 0; j < iCount2; ++j )
		{
			ppArray[i][j] = iDefault;
		}
	}
}
//------------------------------------------------------------------------------
void CvDatabaseUtility::SafeDelete2DArray( int**&ppArray )
{
	delete[] ppArray;
	ppArray = NULL;
}

//------------------------------------------------------------------------------
bool CvDatabaseUtility::PopulateArrayByExistence(bool*& pArray, const char* szTypeTableName, const char* szDataTableName, const char* szTypeColumn, const char* szFilterColumn, const char* szFilterValue)
{
	InitializeArray(pArray, MaxRows(szTypeTableName), false);

	std::string strKey = "_PABE_";
	strKey.append(szTypeTableName);
	strKey.append(szDataTableName);
	strKey.append(szFilterColumn);

	Database::Results* pResults = GetResults(strKey);
	if(pResults == NULL)
	{
		char szSQL[512];
		sprintf_s(szSQL, "select %s.ID from %s inner join %s on %s = %s.Type where %s = ?", szTypeTableName, szDataTableName, szTypeTableName, szTypeColumn, szTypeTableName, szFilterColumn);

		pResults = PrepareResults(strKey, szSQL);
		if(pResults == NULL)
			return false;
	}

	if(!pResults->Bind(1, szFilterValue, false))
	{
		CvAssertMsg(false, GetErrorMessage());
		return false;
	}

	while(pResults->Step())
	{
		const int idx = pResults->GetInt(0);
		pArray[idx] = true;
	}

	pResults->Reset();

	return true;
}
//------------------------------------------------------------------------------
bool CvDatabaseUtility::PopulateArrayByExistence(int*& pArray, const char* szTypeTableName, const char* szDataTableName, const char* szTypeColumn, const char* szFilterColumn, const char* szFilterValue)
{
	InitializeArray(pArray, MaxRows(szTypeTableName), -1);

	std::string strKey = "_PABE_";
	strKey.append(szTypeTableName);
	strKey.append(szDataTableName);
	strKey.append(szFilterColumn);

	Database::Results* pResults = GetResults(strKey);
	if(pResults == NULL)
	{
		char szSQL[512];
		sprintf_s(szSQL, "select %s.ID from %s inner join %s on %s = %s.Type where %s = ?", szTypeTableName, szDataTableName, szTypeTableName, szTypeColumn, szTypeTableName, szFilterColumn);
		pResults = PrepareResults(strKey, szSQL);
		if(pResults == NULL)
			return false;
	}

	if(!pResults->Bind(1, szFilterValue, false))
	{
		CvAssertMsg(false, GetErrorMessage());
		return false;
	}

	int idx = 0;
	while(pResults->Step())
	{
		pArray[idx++] = pResults->GetInt(0);
	}

	pResults->Reset();

	return true;
}
//------------------------------------------------------------------------------
bool CvDatabaseUtility::PopulateArrayByValue(int*& pArray, const char* szTypeTableName, const char* szDataTableName, const char* szTypeColumn, const char* szFilterColumn, const char* szFilterValue, const char* szValueColumn, int iDefaultValue /* = 0 */, int iMinArraySize /* = 0 */)
{
	int iSize = MaxRows(szTypeTableName);
	InitializeArray(pArray, (iSize<iMinArraySize)?iMinArraySize:iSize, iDefaultValue);

	std::string strKey = "_PABV_";
	strKey.append(szTypeTableName);
	strKey.append(szDataTableName);
	strKey.append(szFilterColumn);
	strKey.append(szValueColumn);

	Database::Results* pResults = GetResults(strKey);
	if(pResults == NULL)
	{
		char szSQL[512];
		sprintf_s(szSQL, "select %s.ID, %s from %s inner join %s on %s = %s.Type where %s = ?", szTypeTableName, szValueColumn, szDataTableName, szTypeTableName, szTypeColumn, szTypeTableName, szFilterColumn);
		pResults = PrepareResults(strKey, szSQL);
		if(pResults == NULL)
			return false;
	}

	if(!pResults->Bind(1, szFilterValue, false))
	{
		CvAssertMsg(false, GetErrorMessage());
		return false;
	}
	while(pResults->Step())
	{
		const int idx = pResults->GetInt(0);
		const int value = pResults->GetInt(1);
		pArray[idx] = value;
	}

	pResults->Reset();

	return true;
}
//------------------------------------------------------------------------------
bool CvDatabaseUtility::SetFlavors(int*& pFlavorsArray,
								   const char* szTableName,
								   const char* szFilterColumn,
								   const char* szFilterValue,
								   int iDefaultValue)
{
	return PopulateArrayByValue(pFlavorsArray, "Flavors", szTableName,
						"FlavorType", szFilterColumn, szFilterValue, "Flavor", iDefaultValue);
}
//------------------------------------------------------------------------------
bool CvDatabaseUtility::SetYields(int*& pYieldsArray,
								  const char* szTableName,
								  const char* szFilterColumn,
								  const char* szFilterValue)
{
	return PopulateArrayByValue(pYieldsArray, "Yields", szTableName,
						"YieldType", szFilterColumn, szFilterValue, "Yield");
}
//------------------------------------------------------------------------------
int CvDatabaseUtility::MaxRows(const char* szTableName)
{
	char szSQL[256] = {0};
	sprintf_s(szSQL, "SELECT max(rowid) from %s", szTableName);
	Database::Results kResults;
	int maxValue = 0;
	if(DB.Execute(kResults, szSQL))
	{
		if(kResults.Step())
		{
			//Since some rowid's start at 0 in Civ, let's increase this # by 1.
			maxValue = kResults.GetInt(0) + 1;
		}
	}

	return maxValue;
}
//------------------------------------------------------------------------------
const char* CvDatabaseUtility::GetErrorMessage() const
{
	return DB.ErrorMessage();
}
//------------------------------------------------------------------------------