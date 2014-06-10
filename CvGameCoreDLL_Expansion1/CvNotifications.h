/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_NOTIFICATIONS_H
#define CIV5_NOTIFICATIONS_H

class CvPlayer;

//////////////////////////////////////////////////////////////////////////
// GameCore DLL Specific Notification Type Definitions
//////////////////////////////////////////////////////////////////////////
#define NOTIFICATION_PLAYER_KICKED (NotificationTypes)0xABD3C7BA 

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvNotifications
//!  \brief		Maintains the gameplay notifications for a player
//
//!  Key Attributes:
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvNotifications
{
public:
	CvNotifications(void);
	~CvNotifications(void);

	void Init(PlayerTypes ePlayer);
	void Uninit(void);

	//// Serialization routines
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	void Update(void);
	void EndOfTurnCleanup(void);  // removing notifications at the end turns

	int  AddByName(const char* szNotificationName, const char* strMessage, const char* strSummary, int iX, int iY, int iGameDataIndex, int iExtraGameData=-1);
	int  Add(NotificationTypes eNotificationType, const char* strMessage, const char* strSummary, int iX, int iY, int iGameDataIndex, int iExtraGameData=-1);
	void Activate(int iLookupIndex);
	void Dismiss(int iLookupIndex, bool bUserInvoked);
	bool MayUserDismiss(int iLookupIndex);  // may the user dismiss this notification using the user interface without completing its action
	void Rebroadcast(void);  // rebroadcast active notification

	bool GetEndTurnBlockedType(EndTurnBlockingTypes& eBlockingType, int& iNotificationIndex);  // gets the type of blocking and the lookup index

	// access by the lua script
	int GetNumNotifications(void);
	CvString GetNotificationStr(int iZeroBasedIndex);      // ignores begin/end values
	CvString GetNotificationSummary(int iZeroBasedIndex);  // ignores begin/end values
	int GetNotificationID(int iZeroBasedIndex);	        // ignores begin/end values
	int GetNotificationTurn(int iZeroBasedIndex);	        // ignores begin/end values
	bool IsNotificationDismissed(int iZeroBasedIndex);     // ignores begin/end values

	struct Notification
	{
		void Clear();

		NotificationTypes m_eNotificationType;
		PlayerTypes m_ePlayerID;
		CvString m_strMessage;
		CvString m_strSummary;
		int m_iX;
		int m_iY;
		int m_iGameDataIndex;   // used for unit and city indexes (among other things)
		int m_iExtraGameData;   // used for even more data
		int m_iTurn;	        // internal use - which turn this event was created on
		int m_iLookupIndex;     // internal use - identifier to keep the connection between the ui and this system
		bool m_bNeedsBroadcast; // internal use - have we broadcast this message?
		bool m_bDismissed;		// internal use - has this message been dismissed?
	};

	void Activate(Notification& notification);

	bool IsNotificationRedundant(Notification& notification);  // if there is already a message in the queue similiar to this one

	bool IsNotificationExpired(int iIndex);
	bool IsNotificationEndOfTurnExpired(int iIndex);  // remove it from the end of the turn

	static void AddToPlayer(PlayerTypes ePlayer, NotificationTypes eNotificationType, const char* strMessage, const char* strSummary, int iX=-1, int iY=-1, int iGameDataIndex=-1, int iExtraGameData=-1);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:

	bool IsArrayFull();
	void RemoveOldestNotification();
	void IncrementBeginIndex();
	void IncrementEndIndex();

	PlayerTypes m_ePlayer;

	std::vector<Notification> m_aNotifications;
	int m_iNotificationsBeginIndex;
	int m_iNotificationsEndIndex;

	int m_iCurrentLookupIndex;
};

FDataStream& operator>>(FDataStream&, CvNotifications::Notification&);
FDataStream& operator<<(FDataStream&, const CvNotifications::Notification&);

#endif //CIV5_BUILDER_TASKING_AI_H
