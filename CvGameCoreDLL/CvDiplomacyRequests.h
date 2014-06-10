/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_DIPLOREQUESTS_H
#define CIV5_DIPLOREQUESTS_H

class CvPlayer;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvDiplomacyRequests
//!  \brief		Manager class for Diplomacy Requests between the AI and a human
//				Similar to the CvNotifications class, but unlike notifications, diplomacy
//				requests must be handled in the order received and one at a time.
//
//!  Key Attributes:
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvDiplomacyRequests
{
public:
	CvDiplomacyRequests(void);
	~CvDiplomacyRequests(void);

	void Init(PlayerTypes ePlayer);
	void Uninit(void);

	//// Serialization routines
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	PlayerTypes GetNextAIPlayer() const;
	void SetNextAIPlayer(PlayerTypes eNextPlayer);
	bool HasPendingRequests() const;
	bool HasActiveRequest() const;
	bool HasActiveRequestFrom(PlayerTypes eFromPlayer) const;

	void Update (void);
	void BeginTurn(void);
	void EndTurn(void);

	bool  Add(PlayerTypes ePlayerID, DiploUIStateTypes eDiploType, const char* pszMessage, LeaderheadAnimationTypes eAnimationType, int iExtraGameData = -1);
	void  ActiveRequestComplete();

	struct Request
	{
		void Clear();

		DiploUIStateTypes			m_eDiploType;
		PlayerTypes					m_eFromPlayer;		// Who the diplo request is from
		CvString					m_strMessage;
		LeaderheadAnimationTypes	m_eAnimationType;
		int							m_iTurn;			// internal use - which turn this event was created on
		int							m_iLookupIndex;     // internal use - identifier to keep the connection between the ui and this system
		int							m_iExtraGameData;
	};

	void Activate (Request& kRequest);

	static void DoAIDiplomacy(PlayerTypes eTargetPlayer);

	static void SendRequest(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, DiploUIStateTypes eDiploType, const char* pszMessage, LeaderheadAnimationTypes eAnimationType, int iExtraGameData = -1);
	static void SendDealRequest(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, CvDeal* pkDeal, DiploUIStateTypes eDiploType, const char* pszMessage, LeaderheadAnimationTypes eAnimationType);

	static bool HasActiveDiploRequestWithHuman(PlayerTypes eSourcePlayer);

	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:

	void Send(PlayerTypes eFromPlayer, DiploUIStateTypes eDiploType, const char* pszMessage, LeaderheadAnimationTypes eAnimationType, int iExtraGameData = -1);

	PlayerTypes m_ePlayer;
	PlayerTypes	m_eNextAIPlayer;		/// The next AI player to ask if they want to do diplomacy with us (humans only).

	typedef std::list<Request> RequestList;
	RequestList m_aRequests;
	PlayerTypes	m_bRequestActiveFromPlayer;	/// If a request is active, this is who it is from
	bool		m_bRequestActive;		/// If true, a request in being processed
};

FDataStream & operator>>(FDataStream &, CvDiplomacyRequests::Request &);
FDataStream & operator<<(FDataStream &, const CvDiplomacyRequests::Request &);

#endif
