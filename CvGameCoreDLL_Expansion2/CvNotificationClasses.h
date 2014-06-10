/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_NOTIFICATION_CLASSES_H
#define CIV5_NOTIFICATION_CLASSES_H

// Forward definitions

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvNotificationEntry
//!  \brief		A single type of notification
//
//!  Key Attributes:
//!  - Populated from XML\Misc\Notifications.xml
//!  - Array of these contained in CvNotificationXMLEntries class
//!  - DOES NOT INHERIT FROM CvBaseInfo, may my soul be damned
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvNotificationEntry
{
public:
	CvNotificationEntry(void);
	~CvNotificationEntry(void);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	// Accessor Functions (Non-Arrays)
	const char* GetTypeName();

private:
	CvString m_strNotificationType;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvNotificationXMLEntries
//!  \brief		Game-wide information about the notification types
//
//! Key Attributes:
//! - Plan is it will be contained in CvGameRules object within CvGame class
//!  - Populated from XML\Misc\Notifications.xml
//! - Contains an array of CvNotificationEntry from the above XML file
//! - One instance for the entire game
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvNotificationXMLEntries
{
public:
	CvNotificationXMLEntries(void);
	~CvNotificationXMLEntries(void);

	typedef std::vector<CvNotificationEntry*> EntryArray;

	// Accessor functions
	EntryArray& GetNotificationEntries();
	int GetNumNotifications();
	_Ret_maybenull_ CvNotificationEntry* GetEntry(int index);
	_Ret_maybenull_ CvNotificationEntry* GetByID(uint hHash);
	_Ret_maybenull_ CvNotificationEntry* GetByString(const char* pszName);

	void DeleteArray();

private:
	EntryArray m_paNotificationEntries;
	typedef std::map<uint, int> EntryHashTable;
	EntryHashTable m_mEntries;

};

#endif //CIV5_NOTIFICATION_CLASSES_H