/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"

// must be included after all other headers
#include "LintFree.h"

/// Constructor
CvNotificationEntry::CvNotificationEntry()
    : m_strNotificationType("")
{
}

/// Destructor
CvNotificationEntry::~CvNotificationEntry(void)
{
}

/// Read from XML file (pass 1)
bool CvNotificationEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility&)
{
	m_strNotificationType = kResults.GetText("NotificationType");
	return true;
}

/// Get the type name that we can use to look up in other files
const char* CvNotificationEntry::GetType()
{
	return m_strNotificationType.c_str();
}

//=====================================
// CvNotificationXMLEntries
//=====================================
/// Constructor
CvNotificationXMLEntries::CvNotificationXMLEntries(void)
{

}

/// Destructor
CvNotificationXMLEntries::~CvNotificationXMLEntries(void)
{
	DeleteArray();
}

/// Returns vector of notification entries
std::vector<CvNotificationEntry*>& CvNotificationXMLEntries::GetNotificationEntries()
{
	return m_paNotificationEntries;
}

/// Number of defined notification
int CvNotificationXMLEntries::GetNumNotifications()
{
	return m_paNotificationEntries.size();
}

/// Clear policy entries
void CvNotificationXMLEntries::DeleteArray()
{
	for(std::vector<CvNotificationEntry*>::iterator it = m_paNotificationEntries.begin(); it != m_paNotificationEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paNotificationEntries.clear();
}

/// Get a specific entry
CvNotificationEntry* CvNotificationXMLEntries::GetEntry(int index)
{
	FAssert(index < static_cast<int>(m_paNotificationEntries.size()));

	if(index >= 0 && index < static_cast<int>(m_paNotificationEntries.size()))
		return m_paNotificationEntries[index];
	return NULL;
}

// Get an entry by ID (hash)
CvNotificationEntry* CvNotificationXMLEntries::GetByID(uint hHash)
{
	EntryHashTable::iterator itr = m_mEntries.find(hHash);
	if (itr != m_mEntries.end())
		return GetEntry((*itr).second);
	return NULL;
}

// Get an entry by name
CvNotificationEntry* CvNotificationXMLEntries::GetByString(const char* pszName)
{
	if (pszName && pszName[0] != 0)
		return GetByID(FString::Hash(pszName));
	return NULL;
}
