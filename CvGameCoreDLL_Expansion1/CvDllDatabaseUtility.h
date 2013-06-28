/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//
//  FILE:    CvXMLLoadUtility.h
//
//  PURPOSE: Group of functions to load in the xml files for Civilization 5
//
#pragma once

#include "CvInfos.h"
#include "CvGlobals.h"
#include "ICvDLLDatabaseUtility.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvDllDatabaseUtility
//
//  DESC:   Group of functions to load in the XML files for Civilization 5
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvDllDatabaseUtility: public ICvDLLDatabaseUtility1
{
public:
	CvDllDatabaseUtility(Database::Connection& db);
	~CvDllDatabaseUtility();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	//! See CvDLLXmlLoadUtil::CacheGameDatabseData
	bool DLLCALL CacheGameDatabaseData();

	//! See CvDLLXmlLoadUtil::FlushGameDatabaseData
	bool DLLCALL FlushGameDatabaseData();

	//! Perform any post processing on the database.
	//! NOTE: This is executed after all mods have been applied.
	//!	NOTE: This is executed AFTER the database has been loaded from cache or built.
	bool DLLCALL PerformDatabasePostProcessing();

private:
	void DLLCALL Destroy();

	//! Prefetch the game data into local arrays.
	bool PrefetchGameData();

	//! Validation routines
	bool ValidateGameDatabase();
	bool ValidatePrefetchProcess();

	bool UpdatePlayableCivilizationCounts();

	// loads calls various functions that load XML files that in turn load relevant global variables
	bool SetGlobals();

	// loads global defines
	bool LoadGlobalDefines();

	// function that sets the number of strings in a list, initializes the string to the correct length, and fills it from the
	// current XML file, it assumes that the current node is the parent node of the string list children
	bool SetStringList(CvString** ppszStringArray, int* piSize);

	template<class T>
	bool PrefetchCollection(std::vector<T*>& kCollection, const char* tableName);

	//
	// special cases of set class info which don't use the template because of extra code they have
	//
	bool SetGlobalActionInfo();

	template <class T>
	void orderHotkeyInfo(int** ppiSortedIndex, T* pHotkeyInfos, int iLength);
	void orderHotkeyInfo(int** ppiSortedIndex, int* pHotkeyIndex, int iLength);

	void LogMsg(const char* format, ...) const;

	//! Database members
	Database::Connection&	m_kGameplayDatabase;

	//! Flag to denote whether we need to cache game database data.
	bool m_bGameDatabaseNeedsCaching;

	//! Reference counting
	unsigned int m_uiRefCount;
};

//
/////////////////////////// inlines / templates
//////////////////////////////////////////////////////////////////////////
template<class T>
bool CvDllDatabaseUtility::PrefetchCollection(std::vector<T*>& kCollection, const char* tableName)
{
	cvStopWatch kPerfTest(tableName, "xml-perf.log");

	//First release any previous data in the collection.
	for(std::vector<T*>::iterator it = kCollection.begin(); it != kCollection.end(); ++it)
	{
		T* pkItem = (*it);
		delete pkItem;
	}
	kCollection.clear();

	//Repopulate!
	size_t index = 0;
	Database::Results kResults;
	CvDatabaseUtility kUtility;

	if(DB.SelectWhere(kResults, tableName, "ID > -1 ORDER BY ID"))
	{
		while(kResults.Step())
		{
			size_t Id = kResults.GetInt("ID");
			CvAssertMsg(index <= Id, "This should never happen!")

			while(Id > index)
			{
				kCollection.push_back(NULL);
				index++;
			}

			T* pkItem = FNEW(T, c_eCiv5GameplayDLL, 0);
			pkItem->CacheResults(kResults, kUtility);
			kCollection.push_back(pkItem);
			index++;
		}
	}
	else
	{
		char szErrorMsg[512];
		sprintf_s(szErrorMsg, "PrefetchCollection: Cannot find table '%s'.  Error - %s", tableName, DB.ErrorMessage());
		CvAssertMsg(false, szErrorMsg);

	}

	return true;
}
