/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDiplomacyRequests.h"
#include "CvPlayer.h"
#include "FStlContainerSerialization.h"
#include "ICvDLLUserInterface.h"
#include "CvEnumSerialization.h"
#include "CvDiplomacyAI.h"
#include "CvDllInterfaces.h"

// Include this after all other headers.
#include "LintFree.h"

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvDiplomacyRequests::Request& writeTo)
{
	loadFrom >> writeTo.m_eDiploType;
	loadFrom >> writeTo.m_eAnimationType;
	loadFrom >> writeTo.m_strMessage;
	loadFrom >> writeTo.m_iExtraGameData;
	loadFrom >> writeTo.m_iTurn;
	loadFrom >> writeTo.m_iLookupIndex;
	loadFrom >> writeTo.m_eFromPlayer;

	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvDiplomacyRequests::Request& readFrom)
{
	saveTo << readFrom.m_eDiploType;
	saveTo << readFrom.m_eAnimationType;
	saveTo << readFrom.m_strMessage;
	saveTo << readFrom.m_iExtraGameData;
	saveTo << readFrom.m_iTurn;
	saveTo << readFrom.m_iLookupIndex;
	saveTo << readFrom.m_eFromPlayer;

	return saveTo;
}

void CvDiplomacyRequests::Request::Clear()
{
	m_eDiploType = NO_DIPLO_UI_STATE;
	m_eAnimationType = NO_LEADERHEAD_ANIM;
	m_eFromPlayer = NO_PLAYER;
	m_strMessage = "";
	m_iTurn = -1;
	m_iLookupIndex = -1;
}

/// Constructor
CvDiplomacyRequests::CvDiplomacyRequests(void)
{
	Uninit();
}

/// Destructor
CvDiplomacyRequests::~CvDiplomacyRequests(void)
{
	Uninit();
}

/// Init
void CvDiplomacyRequests::Init(PlayerTypes ePlayer)
{
	Uninit();
	m_ePlayer = ePlayer;

	m_aRequests.clear();
}

/// Uninit
void CvDiplomacyRequests::Uninit(void)
{
	m_ePlayer = NO_PLAYER;
	m_aRequests.clear();
	m_bRequestActive = false;
	m_eRequestActiveFromPlayer = NO_PLAYER;
}

/// Serialization read
void CvDiplomacyRequests::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_ePlayer;

	uint uiListSize;
	kStream >> uiListSize;

	if(uiListSize > 0)
	{
		for(uint ui = 0; ui < uiListSize; ui++)
		{
			m_aRequests.push_back(Request());
			kStream >> m_aRequests.back();
		}
	}
}

/// Serialization write
void CvDiplomacyRequests::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;

	// need to serialize notification list
	kStream << m_ePlayer;

	kStream << m_aRequests.size();

	for(RequestList::const_iterator i = m_aRequests.begin(); i != m_aRequests.end(); ++i)
	{
		kStream << (*i);
	}
}

//	----------------------------------------------------------------------------
/// Update - called from within CvPlayer
void CvDiplomacyRequests::Update(void)
{
	if (HasActiveRequest())
		return;

	PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
	// If we are active, send out the requests
	if (m_ePlayer == eActivePlayer && GET_PLAYER(eActivePlayer).isTurnActive())
	{
		// JdH => handle requests from one player first...
		if (m_eRequestActiveFromPlayer != NO_PLAYER)
		{
			ActivateNext();
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}


	}
}

//	----------------------------------------------------------------------------
//	Called from within CvPlayer at the beginning of the turn
void CvDiplomacyRequests::BeginTurn(void)
{
	// JdH => change requests to notifications
	{
		CvPlayer& kTo = GET_PLAYER(m_ePlayer);
		CvNotifications* pNotifications = kTo.GetNotifications();
		for (RequestList::iterator iter = m_aRequests.begin(); iter != m_aRequests.end(); ++iter)
		{
			if (iter->m_iLookupIndex < 0)
			{
				CvPlayer& kFrom = GET_PLAYER(iter->m_eFromPlayer);
				CvString leaderMessage = CvString::format("%s: %s", kFrom.getName(), iter->m_strMessage.c_str());
				iter->m_iLookupIndex = pNotifications->Add(NOTIFICATION_PLAYER_DEAL_RECEIVED, leaderMessage, kFrom.getCivilizationDescription(), iter->m_eFromPlayer, -2, -1, -1);
			}
		}
	}
	// JdH <=
}

//	----------------------------------------------------------------------------
//	Called from within CvPlayer at the end of turn
void CvDiplomacyRequests::EndTurn(void)
{
	// JdH => we remove all pending requests that are not from humans at the end of the turn
	GC.getGame().GetGameDeals().DoCancelAllProposedDealsWithPlayer(m_ePlayer, DIPLO_AI_PLAYERS);
	RequestList::iterator iter = m_aRequests.begin();
	while (iter != m_aRequests.end())
	{
		if (CvPreGame::isHuman(iter->m_eFromPlayer))
			++iter;
		else
		{
			if (iter->m_iLookupIndex >= 0)
			{
				// we had a notification: Cancel it
				GET_PLAYER(m_ePlayer).GetNotifications()->Dismiss(iter->m_iLookupIndex, false);
			}
			iter = m_aRequests.erase(iter);
		}
	}
	// JdH <=
}

//	----------------------------------------------------------------------------
/// Adds a new notification to the list
bool CvDiplomacyRequests::Add(PlayerTypes eFromPlayer, DiploUIStateTypes eDiploType, const char* pszMessage, LeaderheadAnimationTypes eAnimationType, int iExtraGameData /*= -1*/)
{
	// Queue it up
	m_aRequests.push_back(Request());
	Request& newRequest = m_aRequests.back();

	newRequest.Clear();
	newRequest.m_eFromPlayer = eFromPlayer;
	newRequest.m_eDiploType = eDiploType;
	newRequest.m_strMessage = pszMessage;
	newRequest.m_iExtraGameData = iExtraGameData;
	newRequest.m_eAnimationType = eAnimationType;
	newRequest.m_iTurn = GC.getGame().getGameTurn();
	if (GET_PLAYER(m_ePlayer).isTurnActive())
	{
		BeginTurn(); // this adds notifications, if necessary
		Update(); // this activates requests if appropriate
	}
	return true;
}
// JdH => new request functions
void CvDiplomacyRequests::ActivateNext()
{
	if (HasActiveRequest())
		return;

	if (m_eRequestActiveFromPlayer == NO_PLAYER)
		return;

	CvAssert(GC.getGame().getActivePlayer() == m_ePlayer);
	CvAssert(GET_PLAYER(m_ePlayer).isTurnActive());

	RequestList::iterator requestIter;
	for (requestIter = m_aRequests.begin(); requestIter != m_aRequests.end(); ++requestIter)
	{
		if (requestIter->m_eFromPlayer == m_eRequestActiveFromPlayer)
			goto foundRequest;
	}

	// no request found
	m_eRequestActiveFromPlayer = NO_PLAYER;
	return;

foundRequest:
	static CvDeal kDeal;
	PlayerTypes eFrom = requestIter->m_eFromPlayer;
	
	// we remove the first proposed deal and use it as the scratch deal
	if (!GC.getGame().GetGameDeals().RemoveProposedDeal(eFrom, m_ePlayer, &kDeal, false))
	{
		JDHLOG(jdh::WARNING, "No deal found! That should never happen!");
		kDeal.ClearItems();
	}

	auto_ptr<ICvDeal1> pDeal = GC.WrapDealPointer(&kDeal);
	DLLUI->SetScratchDeal(pDeal.get());

	if (requestIter->m_iLookupIndex >= 0)
	{
		// we had a notification: Cancel it
		GET_PLAYER(m_ePlayer).GetNotifications()->Dismiss(requestIter->m_iLookupIndex, false);
	}

	//	Send the request
	m_bRequestActive = true;
	DiploUIStateTypes eDiploType = requestIter->m_eDiploType;
	if (GET_PLAYER(requestIter->m_eFromPlayer).isHuman())
	{
		// disable leader root for human players (meeting, denouncing etc...)
		eDiploType = DIPLO_UI_STATE_BLANK_DISCUSSION;
	}
	m_eRequestActiveFromPlayer = eFrom;
	gDLL->GameplayDiplomacyAILeaderMessage(eFrom, eDiploType, requestIter->m_strMessage, requestIter->m_eAnimationType, requestIter->m_iExtraGameData);
	m_aRequests.erase(requestIter);
}
//	----------------------------------------------------------------------------
void CvDiplomacyRequests::ActivateAllFrom(PlayerTypes eFromPlayer)
{
	CvAssertMsg(!HasActiveRequest(), "There should not be any active requests, if the player was able to start Diplomacy with AI ...");
	m_eRequestActiveFromPlayer = eFromPlayer;
	Update();
}
// JdH <=
//	----------------------------------------------------------------------------
bool CvDiplomacyRequests::HasPendingRequests() const
{
	return !m_aRequests.empty() || m_bRequestActive;
}

//	----------------------------------------------------------------------------
bool CvDiplomacyRequests::HasActiveRequest() const
{
	return m_bRequestActive;
}

//	----------------------------------------------------------------------------
bool CvDiplomacyRequests::HasActiveRequestFrom(PlayerTypes eFromPlayer) const
{
	return m_bRequestActive && m_eRequestActiveFromPlayer == eFromPlayer;
}
//	----------------------------------------------------------------------------
bool CvDiplomacyRequests::HasRequestFrom(PlayerTypes eFromPlayer) const
{
	for (RequestList::const_iterator iter = m_aRequests.begin(); iter != m_aRequests.end(); ++iter)
	{
		if (iter->m_eFromPlayer == eFromPlayer)
			return true;
	}

	return false;
}

//	----------------------------------------------------------------------------
//	Send a request from a player to another player.
//	If the toPlayer is the active human player, it will be sent right away, else
//	it will be queued.
// static
void CvDiplomacyRequests::SendRequest(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, DiploUIStateTypes eDiploType, const char* pszMessage, LeaderheadAnimationTypes eAnimationType, int iExtraGameData /*= -1*/)
{
	if (GC.getGame().isNetworkMultiPlayer() && eToPlayer != GC.getGame().getActivePlayer())
	{
		return;
	}
	CvPlayer& kPlayer = GET_PLAYER(eToPlayer);
	CvDiplomacyRequests* pkDiploRequests = kPlayer.GetDiplomacyRequests();
	if(pkDiploRequests)
	{
		// JdH => add now handles everything, from direct sending to adding notifications...
		GC.getGame().GetGameDeals().AddProposedDeal(CvDeal(eFromPlayer, eToPlayer)); // propose dummy deal
		pkDiploRequests->Add(eFromPlayer, eDiploType, pszMessage, eAnimationType, iExtraGameData);
		// JdH <=
	}
}

//	----------------------------------------------------------------------------
//	Request for a deal
//static
void CvDiplomacyRequests::SendDealRequest(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, CvDeal* pkDeal, DiploUIStateTypes eDiploType, const char* pszMessage, LeaderheadAnimationTypes eAnimationType)
{
	CvAssert(eFromPlayer != NO_PLAYER);
	CvAssertMsg(!GET_PLAYER(eFromPlayer).isHuman(), __FUNCTION__ " must not be used by a human player!");

	if (GC.getGame().isNetworkMultiPlayer() && eToPlayer != GC.getGame().getActivePlayer())
	{
		return;
	}

	CvPlayer& kTo = GET_PLAYER(eToPlayer);
	CvDiplomacyRequests* pDiploRequests = kTo.GetDiplomacyRequests();
	if (pDiploRequests && pkDeal)
	{
		CvAssert(pkDeal->GetFromPlayer() == eFromPlayer);
		CvAssert(pkDeal->GetToPlayer() == eToPlayer);
		GC.getGame().GetGameDeals().AddProposedDeal(*pkDeal); // propose the deal (needed for activation...)
		pDiploRequests->Add(eFromPlayer, eDiploType, pszMessage, eAnimationType, -1);
	}
}

//	---------------------------------------------------------------------------
//	Have all the AIs do a diplomacy evaluation with the turn active human players.
//	Please note that the destination player may not be the active player.
//	static
void CvDiplomacyRequests::DoAIDiplomacyWithHumans()
{
	// just loop through all ai players and to diplomacy with active humans
	for (int i = 0; i < MAX_CIV_PLAYERS; ++i)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)i);
		if (kPlayer.isAlive() && !kPlayer.isHuman() && !kPlayer.isMinorCiv() && !kPlayer.isBarbarian())
		{
			kPlayer.GetDiplomacyAI()->DoTurn(DIPLO_HUMAN_PLAYERS);
		}
	}
}

//	----------------------------------------------------------------------------
void CvDiplomacyRequests::ActiveRequestComplete()
{
	m_bRequestActive = false;
}

//	---------------------------------------------------------------------------
// Return true if the supplied player has a diplo request with a human.
//static 
bool CvDiplomacyRequests::HasDiploRequestWithHuman(PlayerTypes eSourcePlayer)
{
	for (int i = 0; i < MAX_CIV_PLAYERS; ++i)
	{
		CvPlayer& kTargetPlayer = GET_PLAYER((PlayerTypes)i);
		if (kTargetPlayer.isHuman() && kTargetPlayer.isAlive() && (PlayerTypes)i != eSourcePlayer)
		{
			if (kTargetPlayer.GetDiplomacyRequests()->HasRequestFrom(eSourcePlayer))
				return true;
		}
	}
	return false;
}
