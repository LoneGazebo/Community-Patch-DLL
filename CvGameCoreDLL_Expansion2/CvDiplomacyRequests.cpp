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
#if defined(MOD_ACTIVE_DIPLOMACY)
#include "CvDealAI.h"
#endif

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
	m_eNextAIPlayer = NO_PLAYER;
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
	MOD_SERIALIZE_INIT_READ(kStream);

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
	MOD_SERIALIZE_INIT_WRITE(kStream);

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
#if defined(MOD_ACTIVE_DIPLOMACY)
	if(GC.getGame().isReallyNetworkMultiPlayer() && MOD_ACTIVE_DIPLOMACY)
	{
		if (HasActiveRequest()) 
		{
			return;
		}

		PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
		// If we are active, send out the requests
		if (m_ePlayer == eActivePlayer && GET_PLAYER(eActivePlayer).isTurnActive() && GET_PLAYER(eActivePlayer).isAlive())
		{
			// JdH => handle requests from one player first...
			if (m_eRequestActiveFromPlayer != NO_PLAYER)
			{
				ActivateNext();
			}
		}
	}
	else
	{
		PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
		// If we are active, send out the requests
		if(m_aRequests.size() && m_ePlayer == eActivePlayer && GET_PLAYER(eActivePlayer).isTurnActive())
		{
			CvDiplomacyRequests::Request& kRequest = m_aRequests.front();

			// Make sure the player this is from is still alive.
			if(kRequest.m_eFromPlayer != NO_PLAYER && GET_PLAYER(kRequest.m_eFromPlayer).isAlive())
			{
				Send(kRequest.m_eFromPlayer, kRequest.m_eDiploType, kRequest.m_strMessage, kRequest.m_eAnimationType, kRequest.m_iExtraGameData);
			}
			m_aRequests.pop_front();
		}
	}
#else
	PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
	// If we are active, send out the requests
	if(m_aRequests.size() && m_ePlayer == eActivePlayer && GET_PLAYER(eActivePlayer).isTurnActive())
	{
		CvDiplomacyRequests::Request& kRequest = m_aRequests.front();

		// Make sure the player this is from is still alive.
		if(kRequest.m_eFromPlayer != NO_PLAYER && GET_PLAYER(kRequest.m_eFromPlayer).isAlive())
		{
			Send(kRequest.m_eFromPlayer, kRequest.m_eDiploType, kRequest.m_strMessage, kRequest.m_eAnimationType, kRequest.m_iExtraGameData);
		}
		m_aRequests.pop_front();
	}
#endif
}
//	----------------------------------------------------------------------------
//	Called from within CvPlayer at the beginning of the turn
void CvDiplomacyRequests::BeginTurn(void)
{
#if defined(MOD_ACTIVE_DIPLOMACY)
	if(GC.getGame().isReallyNetworkMultiPlayer() && MOD_ACTIVE_DIPLOMACY)
	{
		// JdH: change requests to notifications
		{
			CvPlayer& kTo = GET_PLAYER(m_ePlayer);
			CvNotifications* pNotifications = kTo.GetNotifications();
			for (RequestList::iterator iter = m_aRequests.begin(); iter != m_aRequests.end(); ++iter)
			{
				if (iter->m_iLookupIndex < 0)
				{
					CvPlayer& kFrom = GET_PLAYER(iter->m_eFromPlayer);
					CvString leaderMessage = CvString::format("%s: %s", kFrom.getName(), iter->m_strMessage.c_str());
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_MP_DIPLO_CONTACT_SUMMARY");
					strSummary << kFrom.getCivilizationShortDescriptionKey();
					iter->m_iLookupIndex = pNotifications->Add(NOTIFICATION_PLAYER_DEAL_RECEIVED, leaderMessage, strSummary.toUTF8(), iter->m_eFromPlayer, -2, GET_TEAM(kFrom.getTeam()).getLeaderID(), -1);
				}
			}
		}
	}
	else
	{
		m_eNextAIPlayer = (PlayerTypes)0;
	}
#else
	m_eNextAIPlayer = (PlayerTypes)0;
#endif
}

//	----------------------------------------------------------------------------
//	Called from within CvPlayer at the end of turn
void CvDiplomacyRequests::EndTurn(void)
{
	m_eNextAIPlayer = NO_PLAYER;
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
#if defined(MOD_ACTIVE_DIPLOMACY)
	if(GC.getGame().isReallyNetworkMultiPlayer() && MOD_ACTIVE_DIPLOMACY)
	{
		BeginTurn(); // this adds notifications, if necessary
		Update(); // this activates requests if appropriate
	}
#endif
	return true;
}
#if defined(MOD_ACTIVE_DIPLOMACY)
//	----------------------------------------------------------------------------
//	Called from within CvPlayer at the end of turn
void CvDiplomacyRequests::CheckRemainingNotifications()
{
	if (m_aRequests.size() > 0)
	{
		RequestList::iterator iter = m_aRequests.begin();
		while (iter != m_aRequests.end())
		{
			if (iter->m_iLookupIndex >= 0)
			{
				CvDeal* pDeal = GC.getGame().GetGameDeals().GetProposedMPDeal(iter->m_eFromPlayer, m_ePlayer, false);
				if (!pDeal)
				{
					iter = m_aRequests.erase(iter);
					continue;
				}

				bool bBad = pDeal->AreAllTradeItemsValid();
				if (!bBad && !CvPreGame::isHuman(iter->m_eFromPlayer))
				{
					int iDealValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer;
					bool bCantMatchOffer;
					bBad = GET_PLAYER(iter->m_eFromPlayer).GetDealAI()->IsDealWithHumanAcceptable(pDeal, m_ePlayer, /*Passed by reference*/ iDealValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer, &bCantMatchOffer, false);
				}
				if (bBad)
				{
					GC.getGame().GetGameDeals().RemoveProposedDeal(iter->m_eFromPlayer, m_ePlayer, NULL, false);

					if (iter->m_iLookupIndex >= 0)
						GET_PLAYER(m_ePlayer).GetNotifications()->Dismiss(iter->m_iLookupIndex, false);

					iter = m_aRequests.erase(iter);

					CvPlayerAI& kFromPlayer = GET_PLAYER(iter->m_eFromPlayer);
					Localization::String strMessage;
					Localization::String strSummary;

					strSummary = Localization::Lookup("TXT_KEY_DEAL_WITHDRAWN");
					strMessage = Localization::Lookup("TXT_KEY_DEAL_WITHDRAWN_BY_THEM");
					strMessage << kFromPlayer.getNickName();
					GET_PLAYER(m_ePlayer).GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), iter->m_eFromPlayer, -1, -1);
				}
			}
			++iter;
		}
	}
}
//	----------------------------------------------------------------------------
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
	
	// we remove the first proposed deal and use it as the scratch deal ...
	if (!(CvPreGame::isHuman(m_ePlayer) && CvPreGame::isHuman(eFrom)))
	{
		// ... but only for AI to human requests
		if (!GC.getGame().GetGameDeals().RemoveProposedDeal(eFrom, m_ePlayer, &kDeal, false))
		{
			// Well, that should never happen ...
			// TODO: log error
			m_aRequests.erase(requestIter);
			return;
		}
	}

	auto_ptr<ICvDeal1> pDeal = GC.WrapDealPointer(&kDeal);
	DLLUI->SetScratchDeal(pDeal.get());

	if (requestIter->m_iLookupIndex >= 0)
	{
		// we had a notification: Cancel it
		GET_PLAYER(m_ePlayer).GetNotifications()->Dismiss(requestIter->m_iLookupIndex, false);
	}

	// Send the request
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
	//GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}
//	----------------------------------------------------------------------------
void CvDiplomacyRequests::ActivateAllFrom(PlayerTypes eFromPlayer)
{
	CvAssertMsg(!HasActiveRequest(), "There should not be any active requests, if the player was able to start Diplomacy with AI ...");
	m_eRequestActiveFromPlayer = eFromPlayer;
	Update();
}
#endif
//	----------------------------------------------------------------------------
//	Send the request immediately
void CvDiplomacyRequests::Send(PlayerTypes eFromPlayer, DiploUIStateTypes eDiploType, const char* pszMessage, LeaderheadAnimationTypes eAnimationType, int iExtraGameData /*= -1*/)
{
	gDLL->GameplayDiplomacyAILeaderMessage(eFromPlayer, eDiploType, pszMessage, eAnimationType, iExtraGameData);
	m_eRequestActiveFromPlayer = eFromPlayer;
	m_bRequestActive = true;
}

//	----------------------------------------------------------------------------
PlayerTypes CvDiplomacyRequests::GetNextAIPlayer() const
{
	return m_eNextAIPlayer;
}

//	----------------------------------------------------------------------------
void CvDiplomacyRequests::SetNextAIPlayer(PlayerTypes eNextPlayer)
{
	m_eNextAIPlayer = eNextPlayer;
}

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
#if defined(MOD_ACTIVE_DIPLOMACY)
	if(GC.getGame().isReallyNetworkMultiPlayer() && MOD_ACTIVE_DIPLOMACY)
	{
		for (RequestList::const_iterator iter = m_aRequests.begin(); iter != m_aRequests.end(); ++iter)
		{
			if (iter->m_eFromPlayer == eFromPlayer)
				return true;
		}
	}
	else
	{
		return m_bRequestActive && m_eRequestActiveFromPlayer == eFromPlayer;
	}
	return m_bRequestActive && m_eRequestActiveFromPlayer == eFromPlayer;
#else
	return m_bRequestActive && m_eRequestActiveFromPlayer == eFromPlayer;
#endif
}

#if defined(MOD_ACTIVE_DIPLOMACY)
//	---------------------------------------------------------------------------
//	Have all the AIs do a diplomacy evaluation with the turn active human players.
//	Please note that the destination player may not be the active player.
//	static
void CvDiplomacyRequests::DoAIMPDiplomacyWithHumans()
{
	if (s_aDiploHumans.size() == 0)
		return;

	// just loop through all ai players and to diplomacy with active humans
	for (int i = 0; i < MAX_CIV_PLAYERS; ++i)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)i);
		if (kPlayer.isAlive() && !kPlayer.isHuman() && !kPlayer.isMinorCiv() && !kPlayer.isBarbarian())
		{
			kPlayer.GetDiplomacyAI()->DoTurn(DIPLO_HUMAN_PLAYERS);
		}
	}

	s_aDiploHumans.clear();
}
/*static*/ std::vector<PlayerTypes> CvDiplomacyRequests::s_aDiploHumans;
#endif
//	----------------------------------------------------------------------------
//	Send a request from a player to another player.
//	If the toPlayer is the active human player, it will be sent right away, else
//	it will be queued.
// static
void CvDiplomacyRequests::SendRequest(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, DiploUIStateTypes eDiploType, const char* pszMessage, LeaderheadAnimationTypes eAnimationType, int iExtraGameData /*= -1*/)
{
#if defined(MOD_ACTIVE_DIPLOMACY)
	if(GC.getGame().isReallyNetworkMultiPlayer() && MOD_ACTIVE_DIPLOMACY)
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
			if (!(CvPreGame::isHuman(eFromPlayer) && CvPreGame::isHuman(eToPlayer)))
			{
				// dummy deal for AI to Human requests
				GC.getGame().GetGameDeals().AddProposedDeal(CvDeal(eFromPlayer, eToPlayer));
			}
			pkDiploRequests->Add(eFromPlayer, eDiploType, pszMessage, eAnimationType, iExtraGameData);
			// JdH <=
		}
	}
	else
	{
		CvPlayer& kPlayer = GET_PLAYER(eToPlayer);
		CvDiplomacyRequests* pkDiploRequests = kPlayer.GetDiplomacyRequests();
		if(pkDiploRequests)
		{
			if(!CvPreGame::isNetworkMultiplayerGame() && GC.getGame().getActivePlayer() == eToPlayer)
			{
				// Target is the active player, just send it right now
				pkDiploRequests->Send(eFromPlayer, eDiploType, pszMessage, eAnimationType, iExtraGameData);
			}
			else
				pkDiploRequests->Add(eFromPlayer, eDiploType, pszMessage, eAnimationType, iExtraGameData);
		}
	}
#else
	CvPlayer& kPlayer = GET_PLAYER(eToPlayer);
	CvDiplomacyRequests* pkDiploRequests = kPlayer.GetDiplomacyRequests();
	if(pkDiploRequests)
	{
		if(!CvPreGame::isNetworkMultiplayerGame() && GC.getGame().getActivePlayer() == eToPlayer)
		{
			// Target is the active player, just send it right now
			pkDiploRequests->Send(eFromPlayer, eDiploType, pszMessage, eAnimationType, iExtraGameData);
		}
		else
			pkDiploRequests->Add(eFromPlayer, eDiploType, pszMessage, eAnimationType, iExtraGameData);
	}
#endif
}

//	----------------------------------------------------------------------------
//	Request for a deal
//static
void CvDiplomacyRequests::SendDealRequest(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, CvDeal* pkDeal, DiploUIStateTypes eDiploType, const char* pszMessage, LeaderheadAnimationTypes eAnimationType)
{
#if defined(MOD_ACTIVE_DIPLOMACY)
	if(GC.getGame().isReallyNetworkMultiPlayer() && MOD_ACTIVE_DIPLOMACY)
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
	else
	{
		// Deals must currently happen on the active player's turn...
		if(GC.getGame().getActivePlayer() == eToPlayer && pkDeal->GetNumItems() > 0)
		{
			auto_ptr<ICvDeal1> pDeal = GC.WrapDealPointer(pkDeal);
			GC.GetEngineUserInterface()->SetScratchDeal(pDeal.get());
			SendRequest(eFromPlayer, eToPlayer, eDiploType, pszMessage, eAnimationType, -1);
		}
	}
#else
	// Deals must currently happen on the active player's turn...
	if(GC.getGame().getActivePlayer() == eToPlayer && pkDeal->GetNumItems() > 0)
	{
		auto_ptr<ICvDeal1> pDeal = GC.WrapDealPointer(pkDeal);
		GC.GetEngineUserInterface()->SetScratchDeal(pDeal.get());
		SendRequest(eFromPlayer, eToPlayer, eDiploType, pszMessage, eAnimationType, -1);
	}
#endif
}

//	---------------------------------------------------------------------------
//	Have all the AIs do a diplomacy evaluation with the supplied player.
//	Please note that the destination player may not be the active player.
//	static
void CvDiplomacyRequests::DoAIDiplomacy(PlayerTypes eTargetPlayer)
{
	if(eTargetPlayer != NO_PLAYER)
	{
		ICvUserInterface2* pkIFace = GC.GetEngineUserInterface();
		// WARNING: Processing depends on the state of the interface!
		CvAssert(!CvPreGame::isNetworkMultiplayerGame());

		// Don't process while a modal dialog is up or another diplo or popup is up.
		if(pkIFace->IsModalStackEmpty() && !pkIFace->isDiploOrPopupWaiting() && !pkIFace->isCityScreenUp())
		{
			CvPlayer& kTargetPlayer = GET_PLAYER((PlayerTypes) eTargetPlayer);
			CvDiplomacyRequests* pkTargetRequests = kTargetPlayer.GetDiplomacyRequests();
			if(pkTargetRequests)
			{
				if(!pkTargetRequests->HasPendingRequests())
				{
					PlayerTypes ePlayerLoop = pkTargetRequests->GetNextAIPlayer();
					if(ePlayerLoop != NO_PLAYER)
					{
						int iMaxCivPlayers = MAX_CIV_PLAYERS;
						while((int)ePlayerLoop < iMaxCivPlayers)
						{
							CvPlayer& kPlayer = GET_PLAYER(ePlayerLoop);

							if(eTargetPlayer != ePlayerLoop && kPlayer.isAlive() && !kPlayer.isHuman() && !kPlayer.isMinorCiv() && !kPlayer.isBarbarian())
							{
								kPlayer.GetDiplomacyAI()->DoTurn((DiplomacyPlayerType)eTargetPlayer);
								if(pkTargetRequests->HasPendingRequests())
								{
									ePlayerLoop = (PlayerTypes)((int)ePlayerLoop + 1);
									break;
								}
							}

							ePlayerLoop = (PlayerTypes)((int)ePlayerLoop + 1);
						}

						pkTargetRequests->SetNextAIPlayer((((int)ePlayerLoop >= iMaxCivPlayers)?NO_PLAYER:ePlayerLoop));
					}
				}
			}
		}
	}

}

//	----------------------------------------------------------------------------
void CvDiplomacyRequests::ActiveRequestComplete()
{
	m_bRequestActive = false;
	m_eRequestActiveFromPlayer = NO_PLAYER;
}

//	---------------------------------------------------------------------------
// Return true if the supplied player has an active diplo request with a human.
// The diplo requests are stored on the target player, so we have to check each player
// Overall, this really only needs to check the active player, since this is not currently valid in MP
// but it will be one less thing to change if AI initiated diplo is ever added to MP.
//static 
bool CvDiplomacyRequests::HasActiveDiploRequestWithHuman(PlayerTypes eSourcePlayer)
{
	for (int i = 0; i < MAX_CIV_PLAYERS; ++i)
	{
		CvPlayer& kTargetPlayer = GET_PLAYER((PlayerTypes)i);
		if (kTargetPlayer.isHuman() && kTargetPlayer.isAlive() && (PlayerTypes)i != eSourcePlayer)
		{
			if (kTargetPlayer.GetDiplomacyRequests()->HasActiveRequestFrom(eSourcePlayer))
				return true;
		}
	}
	return false;
}
