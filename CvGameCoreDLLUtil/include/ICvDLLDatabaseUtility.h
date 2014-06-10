#pragma once

#include "CvDllInterfaces.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      ICvDLLDatabaseUtility1
//!  \brief     DLL exposed methods for loading gameplay database data
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class ICvDLLDatabaseUtility1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvDLLDatabaseUtility1; }

	// Loads all Database data into game structures.
	// NOTE: Assumes all data is already in database.
	//! Returns true on success or if data was already cached, false on error.
	virtual bool DLLCALL CacheGameDatabaseData() = 0;

	//! Flushes all cached game database data.
	//! Returns true if Flush was successful, false on error. 
	virtual bool DLLCALL FlushGameDatabaseData() = 0;

	//! Perform any post processing on the database.
	//! NOTE: This is executed after all mods have been applied.
	//!	NOTE: This is executed AFTER the database has been loaded from cache or built.
	virtual bool DLLCALL PerformDatabasePostProcessing() = 0;

};