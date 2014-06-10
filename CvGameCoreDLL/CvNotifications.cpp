/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvNotifications.h"
#include "CvPlayer.h"
#include "FStlContainerSerialization.h"
#include "ICvDLLUserInterface.h"
#include "CvEnumSerialization.h"
#include "CvDllPlot.h"

// Include this after all other headers.
#include "LintFree.h"

#define MAX_NOTIFICATIONS 100
static uint V1_IndexToHash[] = 
{
	NOTIFICATION_GENERIC,
	NOTIFICATION_TECH,
	NOTIFICATION_FREE_TECH,
	NOTIFICATION_POLICY,
	NOTIFICATION_PRODUCTION,
	NOTIFICATION_MET_MINOR,
	NOTIFICATION_MINOR,
	NOTIFICATION_MINOR_QUEST,
	NOTIFICATION_ENEMY_IN_TERRITORY,
	NOTIFICATION_CITY_RANGE_ATTACK,
	NOTIFICATION_BARBARIAN,
	NOTIFICATION_GOODY,
	NOTIFICATION_BUY_TILE,
	NOTIFICATION_CITY_GROWTH,
	NOTIFICATION_CITY_TILE,
	NOTIFICATION_DEMAND_RESOURCE,
	NOTIFICATION_UNIT_PROMOTION,
	NOTIFICATION_WONDER_COMPLETED_ACTIVE_PLAYER,
	NOTIFICATION_WONDER_COMPLETED,
	NOTIFICATION_WONDER_BEATEN,
	NOTIFICATION_GOLDEN_AGE_BEGUN_ACTIVE_PLAYER,
	NOTIFICATION_GOLDEN_AGE_ENDED_ACTIVE_PLAYER,
	NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER,
	NOTIFICATION_STARVING,
	NOTIFICATION_WAR_ACTIVE_PLAYER,
	NOTIFICATION_WAR,
	NOTIFICATION_PEACE_ACTIVE_PLAYER,
	NOTIFICATION_PEACE,
	NOTIFICATION_VICTORY,
	NOTIFICATION_UNIT_DIED,
	NOTIFICATION_CITY_LOST,
	NOTIFICATION_CAPITAL_LOST_ACTIVE_PLAYER,
	NOTIFICATION_CAPITAL_LOST,
	NOTIFICATION_CAPITAL_RECOVERED,
	NOTIFICATION_PLAYER_KILLED,
	NOTIFICATION_DISCOVERED_LUXURY_RESOURCE,
	NOTIFICATION_DISCOVERED_STRATEGIC_RESOURCE,
	NOTIFICATION_DISCOVERED_BONUS_RESOURCE,
	NOTIFICATION_DIPLO_VOTE,
	NOTIFICATION_RELIGION_RACE,
	NOTIFICATION_EXPLORATION_RACE,
	NOTIFICATION_DIPLOMACY_DECLARATION,
	NOTIFICATION_DEAL_EXPIRED_GPT,
	NOTIFICATION_DEAL_EXPIRED_RESOURCE,
	NOTIFICATION_DEAL_EXPIRED_OPEN_BORDERS,
	NOTIFICATION_DEAL_EXPIRED_DEFENSIVE_PACT,
	NOTIFICATION_DEAL_EXPIRED_RESEARCH_AGREEMENT,
	NOTIFICATION_DEAL_EXPIRED_TRADE_AGREEMENT,
	NOTIFICATION_TECH_AWARD,
	NOTIFICATION_PLAYER_DEAL,
	NOTIFICATION_PLAYER_DEAL_RECEIVED,
	NOTIFICATION_PLAYER_DEAL_RESOLVED,
	NOTIFICATION_PROJECT_COMPLETED,
	NOTIFICATION_REBELS,
	NOTIFICATION_FREE_POLICY,
	NOTIFICATION_FREE_GREAT_PERSON,
	NOTIFICATION_DENUNCIATION_EXPIRED,
	NOTIFICATION_FRIENDSHIP_EXPIRED,
	NOTIFICATION_RELIGION_FOUNDED_ACTIVE_PLAYER,
	NOTIFICATION_RELIGION_FOUNDED,
	NOTIFICATION_PANTHEON_FOUNDED_ACTIVE_PLAYER,
	NOTIFICATION_PANTHEON_FOUNDED,
	NOTIFICATION_FOUND_PANTHEON,
	NOTIFICATION_FOUND_RELIGION,
	NOTIFICATION_ENHANCE_RELIGION,
	NOTIFICATION_RELIGION_ENHANCED_ACTIVE_PLAYER,
	NOTIFICATION_RELIGION_ENHANCED,
	NOTIFICATION_SPY_CREATED_ACTIVE_PLAYER,
	NOTIFICATION_SPY_STOLE_TECH,
	NOTIFICATION_SPY_CANT_STEAL_TECH,
	NOTIFICATION_CAN_BUILD_MISSIONARY,
	NOTIFICATION_OTHER_PLAYER_NEW_ERA,
	NOTIFICATION_SPY_EVICTED,
	NOTIFICATION_RELIGION_SPREAD,
	NOTIFICATION_TECH_STOLEN_SPY_DETECTED,
	NOTIFICATION_TECH_STOLEN_SPY_IDENTIFIED,
	NOTIFICATION_SPY_WAS_KILLED,
	NOTIFICATION_SPY_KILLED_A_SPY,
	NOTIFICATION_SPY_REPLACEMENT,
	NOTIFICATION_MAYA_LONG_COUNT,
	NOTIFICATION_FAITH_GREAT_PERSON,
	NOTIFICATION_SPY_PROMOTION,
	NOTIFICATION_INTRIGUE_DECEPTION,
	NOTIFICATION_SPY_RIG_ELECTION_SUCCESS,
	NOTIFICATION_SPY_RIG_ELECTION_FAILURE,
	NOTIFICATION_SPY_RIG_ELECTION_ALERT,
	NOTIFICATION_SPY_YOU_STAGE_COUP_SUCCESS,
	NOTIFICATION_SPY_YOU_STAGE_COUP_FAILURE,
	NOTIFICATION_SPY_STAGE_COUP_SUCCESS,
	NOTIFICATION_SPY_STAGE_COUP_FAILURE,
	NOTIFICATION_INTRIGUE_BUILDING_SNEAK_ATTACK_ARMY,
	NOTIFICATION_INTRIGUE_BUILDING_SNEAK_ATTACK_AMPHIBIOUS,
	NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_KNOWN_CITY_UNKNOWN,
	NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_KNOWN_CITY_KNOWN,
	NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_YOU_CITY_UNKNOWN,
	NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_YOU_CITY_KNOWN,
	NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_UNKNOWN,
	NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_KNOWN_CITY_UNKNOWN,
	NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_KNOWN_CITY_KNOWN,
	NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_YOU_CITY_UNKNOWN,
	NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_YOU_CITY_KNOWN,
	NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_UNKNOWN,
	NOTIFICATION_RELIGION_ERROR,
	NOTIFICATION_AUTOMATIC_FAITH_PURCHASE_STOPPED,
	NOTIFICATION_EXPANSION_PROMISE_EXPIRED,
	NOTIFICATION_BORDER_PROMISE_EXPIRED,
	NOTIFICATION_TRADE_ROUTE,
	NOTIFICATION_TRADE_ROUTE_BROKEN,
	NOTIFICATION_RELIGION_SPREAD_NATURAL,
	NOTIFICATION_INTRIGUE_CONSTRUCTING_WONDER,
	NOTIFICATION_MINOR_BUYOUT,
	NOTIFICATION_REQUEST_RESOURCE,
	NOTIFICATION_LIBERATED_MAJOR_CITY,
	NOTIFICATION_RESURRECTED_MAJOR_CIV,
	NOTIFICATION_ADD_REFORMATION_BELIEF,
	NOTIFICATION_LEAGUE_CALL_FOR_PROPOSALS,
	NOTIFICATION_CHOOSE_ARCHAEOLOGY,
	NOTIFICATION_LEAGUE_CALL_FOR_VOTES,
	NOTIFICATION_CHOOSE_IDEOLOGY,
	NOTIFICATION_IDEOLOGY_CHOSEN,
	NOTIFICATION_DIPLOMAT_EJECTED,
	NOTIFICATION_INTERNATIONAL_TRADE_UNIT_PLUNDERED_TRADER,
	NOTIFICATION_INTERNATIONAL_TRADE_UNIT_PLUNDERED_TRADEE,
	NOTIFICATION_REFORMATION_BELIEF_ADDED_ACTIVE_PLAYER,
	NOTIFICATION_REFORMATION_BELIEF_ADDED,
	NOTIFICATION_GREAT_WORK_COMPLETED_ACTIVE_PLAYER,
	NOTIFICATION_LEAGUE_VOTING_DONE,
	NOTIFICATION_LEAGUE_VOTING_SOON,
	NOTIFICATION_CULTURE_VICTORY_SOMEONE_INFLUENTIAL,
	NOTIFICATION_CULTURE_VICTORY_WITHIN_TWO,
	NOTIFICATION_CULTURE_VICTORY_WITHIN_TWO_ACTIVE_PLAYER,
	NOTIFICATION_CULTURE_VICTORY_WITHIN_ONE,
	NOTIFICATION_CULTURE_VICTORY_WITHIN_ONE_ACTIVE_PLAYER,
	NOTIFICATION_CULTURE_VICTORY_NO_LONGER_INFLUENTIAL,
	NOTIFICATION_PLAYER_RECONNECTED,
	NOTIFICATION_PLAYER_DISCONNECTED,
	NOTIFICATION_TURN_MODE_SEQUENTIAL,
	NOTIFICATION_TURN_MODE_SIMULTANEOUS,
	NOTIFICATION_HOST_MIGRATION,
	NOTIFICATION_PLAYER_CONNECTING,
	NOTIFICATION_CITY_REVOLT_POSSIBLE,
	NOTIFICATION_CITY_REVOLT
};

/// Serialization read
FDataStream & operator>>(FDataStream & loadFrom, CvNotifications::Notification & writeTo)
{
	loadFrom >> writeTo.m_eNotificationType;
	loadFrom >> writeTo.m_strMessage;
	loadFrom >> writeTo.m_strSummary;
	loadFrom >> writeTo.m_iX;
	loadFrom >> writeTo.m_iY;
	loadFrom >> writeTo.m_iGameDataIndex;
	loadFrom >> writeTo.m_iExtraGameData;
	loadFrom >> writeTo.m_iTurn;
	loadFrom >> writeTo.m_iLookupIndex;
	loadFrom >> writeTo.m_bDismissed;
	loadFrom >> writeTo.m_ePlayerID;
	writeTo.m_bNeedsBroadcast = true; // all loads should re-broadcast their events
	// loadFrom >> writeTo.m_bBroadcast;

	return loadFrom;
}

/// Serialization write
FDataStream & operator<<(FDataStream & saveTo, const CvNotifications::Notification & readFrom)
{
	saveTo << readFrom.m_eNotificationType;
	saveTo << readFrom.m_strMessage;
	saveTo << readFrom.m_strSummary;
	saveTo << readFrom.m_iX;
	saveTo << readFrom.m_iY;
	saveTo << readFrom.m_iGameDataIndex;
	saveTo << readFrom.m_iExtraGameData;
	saveTo << readFrom.m_iTurn;
	saveTo << readFrom.m_iLookupIndex;
	saveTo << readFrom.m_bDismissed;
	saveTo << readFrom.m_ePlayerID;
	// this is not saved because we want to re-broadcast on load
	// saveTo << writeTo.m_bBroadcast;

	return saveTo;
}

void CvNotifications::Notification::Clear ()
{
	m_eNotificationType = NO_NOTIFICATION_TYPE;
	m_strMessage = "";
	m_strSummary = "";
	m_iX = -1;
	m_iY = -1;
	m_iGameDataIndex = -1;
	m_iTurn = -1;
	m_iLookupIndex = -1;
	m_bNeedsBroadcast = false;
	m_bDismissed = false;
}

/// Constructor
CvNotifications::CvNotifications(void)
{
	Uninit();
}

/// Destructor
CvNotifications::~CvNotifications(void)
{
	Uninit();
}

/// Init
void CvNotifications::Init (PlayerTypes ePlayer)
{
	Uninit();
	m_ePlayer = ePlayer;

	m_aNotifications.resize(MAX_NOTIFICATIONS);
	for (uint ui = 0; ui < m_aNotifications.size(); ui++)
	{
		m_aNotifications[ui].Clear();
	}

	m_iNotificationsBeginIndex = 0;
	m_iNotificationsEndIndex = 0;
}

/// Uninit
void CvNotifications::Uninit (void)
{
	m_ePlayer = NO_PLAYER;
	m_iCurrentLookupIndex = 0;
	m_aNotifications.clear();

	m_iNotificationsBeginIndex = -1;
	m_iNotificationsEndIndex = -1;
}

/// Serialization read
void CvNotifications::Read (FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_ePlayer;
	kStream >> m_iCurrentLookupIndex;
	kStream >> m_iNotificationsBeginIndex;
	kStream >> m_iNotificationsEndIndex;

	for (uint ui = 0; ui < MAX_NOTIFICATIONS; ui++)
	{
		kStream >> m_aNotifications[ui];
		if (uiVersion <= 1)
		{
			// Translate the old index the hash ID.
			int iIndex = (int)(m_aNotifications[ui].m_eNotificationType);
			if (iIndex >= 0 && iIndex < sizeof(V1_IndexToHash)/sizeof(uint))
				m_aNotifications[ui].m_eNotificationType = (NotificationTypes)V1_IndexToHash[iIndex];
		}
	}
}

/// Serialization write
void CvNotifications::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 2;
	kStream << uiVersion;

	// need to serialize notification list
	kStream << m_ePlayer;
	kStream << m_iCurrentLookupIndex;
	kStream << m_iNotificationsBeginIndex;
	kStream << m_iNotificationsEndIndex;

	for (uint ui = 0; ui < MAX_NOTIFICATIONS; ui++)
	{
		kStream << m_aNotifications[ui];
	}
}

/// Update - called from within CvPlayer
void CvNotifications::Update (void)
{
	int iIndex = m_iNotificationsBeginIndex;
	while (iIndex != m_iNotificationsEndIndex)
	{
		Notification& kNotification = m_aNotifications[iIndex];

		if (!kNotification.m_bDismissed)
		{
			if (IsNotificationExpired(iIndex))
			{
				Dismiss(kNotification.m_iLookupIndex, /*bUserInvoked*/ false);
				//GC.GetEngineUserInterface()->RemoveNotification(kNotification.m_iLookupIndex);
				//kNotification.m_bDismissed = true;
			}
			else
			{
				if (kNotification.m_bNeedsBroadcast)
				{
					// If the notification is for the 'active' player and that active player actually has his turn active or its not hotseat, then show the notification, else wait.
					// The 'active' player is only set to a human and during the AI turn, the 'active' player is the last human to do their turn.
					if(kNotification.m_ePlayerID == GC.getGame().getActivePlayer() && (!CvPreGame::isHotSeatGame() || GET_PLAYER(GC.getGame().getActivePlayer()).isTurnActive()) )
					{
						GC.GetEngineUserInterface()->AddNotification(kNotification.m_iLookupIndex, kNotification.m_eNotificationType, kNotification.m_strMessage.c_str(), kNotification.m_strSummary.c_str(), kNotification.m_iGameDataIndex, kNotification.m_iExtraGameData, m_ePlayer, kNotification.m_iX, kNotification.m_iY);
						kNotification.m_bNeedsBroadcast = false;
					}
				}
			}
		}

		iIndex++;

		if (iIndex >= (int)m_aNotifications.size())
		{
			iIndex = 0;
		}
	}
}

// EndOfTurnCleanup - called from within CvPlayer at the end of turn
void CvNotifications::EndOfTurnCleanup (void)
{
	int iIndex = m_iNotificationsBeginIndex;
	while (iIndex != m_iNotificationsEndIndex)
	{
		if (!m_aNotifications[iIndex].m_bDismissed)
		{
			if (IsNotificationEndOfTurnExpired(iIndex))
			{
				Dismiss(m_aNotifications[iIndex].m_iLookupIndex, /*bUserInvoked*/ false);
			}
		}

		iIndex++;

		if (iIndex >= (int)m_aNotifications.size())
		{
			iIndex = 0;
		}
	}
}

/// Adds a new notification to the list
int CvNotifications::AddByName(const char* pszNotificationName, const char* strMessage, const char* strSummary, int iX, int iY, int iGameDataIndex, int iExtraGameData)
{
	if (pszNotificationName && pszNotificationName[0] != 0)
	{
		return Add((NotificationTypes) FString::Hash(pszNotificationName), strMessage, strSummary, iX, iY, iGameDataIndex, iExtraGameData);
	}
	return -1;
}

/// Adds a new notification to the list
int CvNotifications::Add(NotificationTypes eNotificationType, const char* strMessage, const char* strSummary, int iX, int iY, int iGameDataIndex, int iExtraGameData)
{
	// if the player is not human, do not record
	if (!GET_PLAYER(m_ePlayer).isHuman())
	{
		return -1;
	}

	// If we're in debug mode, don't do anything
	if (GC.getGame().isDebugMode())
		return -1;

	Notification newNotification;
	newNotification.Clear();
	newNotification.m_ePlayerID = m_ePlayer;
	newNotification.m_eNotificationType = eNotificationType;
	newNotification.m_strMessage = strMessage;
	newNotification.m_strSummary = strSummary;
	newNotification.m_iX = iX;
	newNotification.m_iY = iY;
	newNotification.m_iGameDataIndex = iGameDataIndex;
	newNotification.m_iExtraGameData = iExtraGameData;
	newNotification.m_iTurn = GC.getGame().getGameTurn();
	newNotification.m_iLookupIndex = m_iCurrentLookupIndex;
	newNotification.m_bNeedsBroadcast = true;
	newNotification.m_bDismissed = false;

	if (IsNotificationRedundant(newNotification))
	{
		// redundant notification
		return -1;
	}

	if (IsArrayFull())
	{
		RemoveOldestNotification();
	}

	m_aNotifications[m_iNotificationsEndIndex] = newNotification;

	if (GC.getGame().isFinalInitialized())
	{
		// If the notification is for the 'active' player and that active player actually has his turn active or its not hotseat, then show the notification, else wait
		// The 'active' player is only set to a human and during the AI turn, the 'active' player is the last human to do their turn.
		if(newNotification.m_ePlayerID == GC.getGame().getActivePlayer() && (!CvPreGame::isHotSeatGame() || GET_PLAYER(GC.getGame().getActivePlayer()).isTurnActive()) )
		{
			GC.GetEngineUserInterface()->AddNotification(newNotification.m_iLookupIndex, newNotification.m_eNotificationType, newNotification.m_strMessage.c_str(), newNotification.m_strSummary.c_str(), newNotification.m_iGameDataIndex, newNotification.m_iExtraGameData, m_ePlayer, iX, iY);

			// Don't show effect with production notification
			if (eNotificationType != NOTIFICATION_PRODUCTION)
			{
				CvPlot* pPlot = GC.getMap().plot(iX, iY);
				if(pPlot != NULL)
				{
					auto_ptr<ICvPlot1> pDllPlot(new CvDllPlot(pPlot));
					gDLL->GameplayDoFX(pDllPlot.get());
				}
				else
				{
					gDLL->GameplayDoFX(NULL);
				}


			}
			m_aNotifications[m_iNotificationsEndIndex].m_bNeedsBroadcast = false;
		}

		gDLL->GameplayMinimapNotification(iX, iY, m_iCurrentLookupIndex+1);	// The index is used to uniquely identify each flashing dot on the minimap. We're adding 1 since the selected unit is always 0. It ain't pretty, but it'll work
	}

	IncrementEndIndex();

	m_iCurrentLookupIndex++;

	switch (newNotification.m_eNotificationType)
	{
	case NOTIFICATION_POLICY:
	case NOTIFICATION_UNIT_PROMOTION:
		{
			GC.getGame().endTurnTimerSemaphoreIncrement();
			break;
		}
	default:
		break;
	}
	return newNotification.m_iLookupIndex;
}

void CvNotifications::Activate (int iLookupIndex)
{
	int iIndex = m_iNotificationsBeginIndex;
	while (iIndex != m_iNotificationsEndIndex)
	{
		if (m_aNotifications[iIndex].m_iLookupIndex == iLookupIndex)
		{
			Activate(m_aNotifications[iIndex]);
			break;
		}

		iIndex++;
		if (iIndex >= (int)m_aNotifications.size())
		{
			iIndex = 0;
		}
	}
}

void CvNotifications::Dismiss (int iLookupIndex, bool bUserInvoked)
{
	int iIndex = m_iNotificationsBeginIndex;
	while (iIndex != m_iNotificationsEndIndex)
	{
		if (m_aNotifications[iIndex].m_iLookupIndex == iLookupIndex)
		{
			m_aNotifications[iIndex].m_bDismissed = true;
			GC.GetEngineUserInterface()->RemoveNotification(m_aNotifications[iIndex].m_iLookupIndex, m_ePlayer);

			switch (m_aNotifications[iIndex].m_eNotificationType)
			{
			case NOTIFICATION_POLICY:
				{
					if (m_ePlayer == GC.getGame().getActivePlayer() && bUserInvoked)
					{
						GC.GetEngineUserInterface()->SetPolicyNotificationSeen(true);
					}
				}
			case NOTIFICATION_UNIT_PROMOTION:
				{
					GC.getGame().endTurnTimerSemaphoreDecrement();
					break;
				}
			default:
				break;
			}

			break;
		}

		iIndex++;
		if (iIndex >= (int)m_aNotifications.size())
		{
			iIndex = 0;
		}
	}
}

bool CvNotifications::MayUserDismiss(int iLookupIndex)
{
	int iIndex = m_iNotificationsBeginIndex;
	while (iIndex != m_iNotificationsEndIndex)
	{
		if (m_aNotifications[iIndex].m_iLookupIndex == iLookupIndex)
		{
			switch (m_aNotifications[iIndex].m_eNotificationType)
			{
			case NOTIFICATION_DIPLO_VOTE:
			case NOTIFICATION_PRODUCTION:
			case NOTIFICATION_TECH:
			case NOTIFICATION_FREE_TECH:
			case NOTIFICATION_FREE_POLICY:
			case NOTIFICATION_FREE_GREAT_PERSON:
				return false;
				break;

			case NOTIFICATION_POLICY:
				if (GC.getGame().isOption(GAMEOPTION_POLICY_SAVING))
				{
					return true;
					break;
				}
				else
				{
					return false;
					break;
				}

			default:
				return true;
				break;
			}
		}

		iIndex++;
		if (iIndex >= (int)m_aNotifications.size())
		{
			iIndex = 0;
		}
	}

	return false;
}

void CvNotifications::Rebroadcast (void)
{
	int iIndex = m_iNotificationsBeginIndex;
	while (iIndex != m_iNotificationsEndIndex)
	{
		if (!m_aNotifications[iIndex].m_bDismissed)
		{
			m_aNotifications[iIndex].m_bNeedsBroadcast = true;
		}

		iIndex++;
		if (iIndex >= (int)m_aNotifications.size())
		{
			iIndex = 0;
		}
	}
}

bool CvNotifications::GetEndTurnBlockedType (EndTurnBlockingTypes& eBlockingType, int& iNotificationIndex)
{
	eBlockingType = NO_ENDTURN_BLOCKING_TYPE;
	iNotificationIndex = -1;

	int iIndex = m_iNotificationsBeginIndex;
	while (iIndex != m_iNotificationsEndIndex)
	{
		if (!m_aNotifications[iIndex].m_bDismissed)
		{
			switch (m_aNotifications[iIndex].m_eNotificationType)
			{
			case NOTIFICATION_CITY_RANGE_ATTACK:
			{
				bool automaticallyEndTurns = GC.getGame().isGameMultiPlayer() ? GC.GetEngineUserInterface()->IsMPAutoEndTurnEnabled() : GC.GetEngineUserInterface()->IsSPAutoEndTurnEnabled();
				if(automaticallyEndTurns)
				{//City range attacks only block turns if the player is using auto end turn.
					eBlockingType = ENDTURN_BLOCKING_CITY_RANGE_ATTACK;
					iNotificationIndex = m_aNotifications[iIndex].m_iLookupIndex;
					return true;
				}
				break;
			}

			case NOTIFICATION_DIPLO_VOTE:
				eBlockingType = ENDTURN_BLOCKING_DIPLO_VOTE;
				iNotificationIndex = m_aNotifications[iIndex].m_iLookupIndex;
				return true;
				break;

			case NOTIFICATION_MINOR_QUEST:
				if (m_aNotifications[iIndex].m_iExtraGameData != 1)
				{
					eBlockingType = ENDTURN_BLOCKING_MINOR_QUEST;
					iNotificationIndex = m_aNotifications[iIndex].m_iLookupIndex;
					return true;
				}
				break;

			case NOTIFICATION_PRODUCTION:
				eBlockingType = ENDTURN_BLOCKING_PRODUCTION;
				iNotificationIndex = m_aNotifications[iIndex].m_iLookupIndex;
				return true;
				break;

			case NOTIFICATION_POLICY:
				eBlockingType = ENDTURN_BLOCKING_POLICY;
				iNotificationIndex = m_aNotifications[iIndex].m_iLookupIndex;
				return true;
				break;

			case NOTIFICATION_FREE_POLICY:
				eBlockingType = ENDTURN_BLOCKING_FREE_POLICY;
				iNotificationIndex = m_aNotifications[iIndex].m_iLookupIndex;
				return true;
				break;

			case NOTIFICATION_TECH:
				eBlockingType = ENDTURN_BLOCKING_RESEARCH;
				iNotificationIndex = m_aNotifications[iIndex].m_iLookupIndex;
				return true;
				break;

			case NOTIFICATION_FREE_TECH:
				eBlockingType = ENDTURN_BLOCKING_FREE_TECH;
				iNotificationIndex = m_aNotifications[iIndex].m_iLookupIndex;
				return true;
				break;

			case NOTIFICATION_FREE_GREAT_PERSON:
				eBlockingType = ENDTURN_BLOCKING_FREE_ITEMS;
				iNotificationIndex = m_aNotifications[iIndex].m_iLookupIndex;
				return true;
				break;

			default:
				// these notifications don't block, so don't return a blocking type
				break;
			}
		}

		iIndex++;
		if (iIndex >= (int)m_aNotifications.size())
		{
			iIndex = 0;
		}
	}

	return false;
}

int CvNotifications::GetNumNotifications (void)
{
	if (m_iNotificationsEndIndex >= m_iNotificationsBeginIndex)
	{
		int iValue = m_iNotificationsEndIndex - m_iNotificationsBeginIndex;
		return iValue;
	}

	int iValue = (m_aNotifications.size() - m_iNotificationsBeginIndex) + m_iNotificationsEndIndex;
	return iValue;
}

CvString CvNotifications::GetNotificationStr (int iZeroBasedIndex) // ignores the begin/end values
{
	int iRealIndex = (m_iNotificationsBeginIndex + iZeroBasedIndex) % m_aNotifications.size();
	return m_aNotifications[iRealIndex].m_strMessage;
}

CvString CvNotifications::GetNotificationSummary (int iZeroBasedIndex)
{
	int iRealIndex = (m_iNotificationsBeginIndex + iZeroBasedIndex) % m_aNotifications.size();
	return m_aNotifications[iRealIndex].m_strSummary;
}


int CvNotifications::GetNotificationID (int iZeroBasedIndex) // ignores begin/end values
{
	int iRealIndex = (m_iNotificationsBeginIndex + iZeroBasedIndex) % m_aNotifications.size();
	return m_aNotifications[iRealIndex].m_iLookupIndex;
}

int CvNotifications::GetNotificationTurn (int iZeroBasedIndex)
{
	int iRealIndex = (m_iNotificationsBeginIndex + iZeroBasedIndex) % m_aNotifications.size();
	return m_aNotifications[iRealIndex].m_iTurn;
}

bool CvNotifications::IsNotificationDismissed (int iZeroBasedIndex)
{
	int iRealIndex = (m_iNotificationsBeginIndex + iZeroBasedIndex) % m_aNotifications.size();
	return m_aNotifications[iRealIndex].m_bDismissed;
}


void CvNotifications::Activate (Notification& notification)
{
	GC.GetEngineUserInterface()->ActivateNotification(notification.m_iLookupIndex, notification.m_eNotificationType, notification.m_strMessage, notification.m_iX, notification.m_iY, notification.m_iGameDataIndex, notification.m_iExtraGameData, m_ePlayer);

	gDLL->GameplayMinimapNotification(notification.m_iX, notification.m_iY, notification.m_iLookupIndex+1);	// The index is used to uniquely identify each flashing dot on the minimap. We're adding 1 since the selected unit is always 0. It ain't pretty, but it'll work

	switch (notification.m_eNotificationType)
	{
	case NOTIFICATION_WONDER_COMPLETED_ACTIVE_PLAYER:
		{
			CvPopupInfo kPopup( BUTTONPOPUP_WONDER_COMPLETED_ACTIVE_PLAYER, notification.m_iGameDataIndex, notification.m_iExtraGameData, notification.m_iX, notification.m_iY );
			GC.GetEngineUserInterface()->AddPopup( kPopup );
        }
        break;
	case NOTIFICATION_BUY_TILE:
		{
			// Jon say - do like Sid would!
			CvCity* pCity = GET_PLAYER(m_ePlayer).getCapitalCity();
			if (pCity)
			{
				auto_ptr<ICvPlot1> pDllPlot = GC.WrapPlotPointer(pCity->plot());
				GC.GetEngineUserInterface()->lookAt(pDllPlot.get(), CAMERALOOKAT_NORMAL);
			}
		}
		break;
	case NOTIFICATION_TECH:
	case NOTIFICATION_FREE_TECH:
		{
			CvPopupInfo kPopup(BUTTONPOPUP_CHOOSETECH, m_ePlayer, notification.m_iGameDataIndex, notification.m_iExtraGameData);
			strcpy_s(kPopup.szText, notification.m_strMessage);
			GC.GetEngineUserInterface()->AddPopup(kPopup);
		}
		break;
	case NOTIFICATION_TECH_AWARD:
		{
			if (notification.m_iExtraGameData != -1)
			{
				CvPopupInfo kPopup(BUTTONPOPUP_TECH_AWARD, m_ePlayer, notification.m_iGameDataIndex, notification.m_iExtraGameData);
				strcpy_s(kPopup.szText, notification.m_strMessage);
				GC.GetEngineUserInterface()->AddPopup(kPopup);
			}
		}
		break;
	case NOTIFICATION_POLICY:
	case NOTIFICATION_FREE_POLICY:
		{
			CvPopupInfo kPopup(BUTTONPOPUP_CHOOSEPOLICY, m_ePlayer);
			GC.GetEngineUserInterface()->AddPopup(kPopup);
		}
		break;
	case NOTIFICATION_DIPLO_VOTE:
		{
			CvPopupInfo kPopup(BUTTONPOPUP_DIPLO_VOTE, m_ePlayer);
			GC.GetEngineUserInterface()->AddPopup(kPopup);
		}
		break;
	case NOTIFICATION_MINOR_QUEST:
		{
			CvPlot* pPlot = GC.getMap().plot(notification.m_iX, notification.m_iY);

			auto_ptr<ICvPlot1> pDllPlot = GC.WrapPlotPointer(pPlot);

			GC.GetEngineUserInterface()->lookAt(pDllPlot.get(), CAMERALOOKAT_NORMAL);
			gDLL->GameplayDoFX(pDllPlot.get());

			GC.GetEngineUserInterface()->SetTempString(notification.m_strMessage);

			CvPopupInfo kPopup(BUTTONPOPUP_CITY_STATE_MESSAGE, notification.m_iGameDataIndex);
			GC.GetEngineUserInterface()->AddPopup(kPopup);

			notification.m_iExtraGameData = 1; // hack to allow this to be dismissed

			//Dismiss(notification.m_iLookupIndex, /*bUserInvoked*/ false);
		}
		break;
	case NOTIFICATION_PRODUCTION:
		{
			CvCity* pCity = GC.getMap().plot(notification.m_iX, notification.m_iY)->getPlotCity();//GET_PLAYER(m_ePlayer).getCity(notification.m_iGameDataIndex);
			if (!pCity)
			{
				return;
			}

			CvPopupInfo kPopupInfo(BUTTONPOPUP_CHOOSEPRODUCTION);

			kPopupInfo.iData1 = pCity->GetID();

			kPopupInfo.bOption2 = false;	// Not in purchase mode

			// slewis - do we need the stuff below?
			//kPopupInfo.setOption1(false);

			OrderTypes eOrder = (OrderTypes) notification.m_iGameDataIndex;
			int iItemID = notification.m_iExtraGameData;
			kPopupInfo.iData2 = eOrder;
			kPopupInfo.iData3 = iItemID;

			GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
		}
		break;
	case NOTIFICATION_UNIT_PROMOTION:
		{
			UnitHandle pUnit = GET_PLAYER(m_ePlayer).getUnit(notification.m_iExtraGameData);
			if (pUnit)
			{
				CvPlot* pPlot = pUnit->plot();
				if (pPlot)
				{
					auto_ptr<ICvPlot1> pDllPlot = GC.WrapPlotPointer(pPlot);
					auto_ptr<ICvUnit1> pDllUnit = GC.WrapUnitPointer(pUnit.pointer());

					GC.GetEngineUserInterface()->lookAt(pDllPlot.get(), CAMERALOOKAT_NORMAL);
					GC.GetEngineUserInterface()->selectUnit(pDllUnit.get(), false);
					gDLL->GameplayDoFX(pDllPlot.get());
				}
			}
		}
		break;
	case NOTIFICATION_PLAYER_DEAL:
        {
            GC.GetEngineUserInterface()->OpenPlayerDealScreen( (PlayerTypes) notification.m_iX );
        }
        break;
	case NOTIFICATION_PLAYER_DEAL_RECEIVED:
		{
			GC.GetEngineUserInterface()->OpenPlayerDealScreen( (PlayerTypes) notification.m_iX );
		}
		break;
	case NOTIFICATION_FREE_GREAT_PERSON:
		{
			if(GET_PLAYER(m_ePlayer).GetNumFreeGreatPeople() > 0)
			{
				CvPopupInfo kPopup(BUTTONPOPUP_CHOOSE_FREE_GREAT_PERSON, m_ePlayer, notification.m_iGameDataIndex, notification.m_iExtraGameData);
				GC.GetEngineUserInterface()->AddPopup(kPopup);
			}
		}
		break;
	default:	// Default behavior is to move the camera to the X,Y passed in
		{
			CvPlot* pPlot = GC.getMap().plot(notification.m_iX, notification.m_iY);
			if (pPlot)
			{
				auto_ptr<ICvPlot1> pDllPlot = GC.WrapPlotPointer(pPlot);

				GC.GetEngineUserInterface()->lookAt(pDllPlot.get(), CAMERALOOKAT_NORMAL);
				gDLL->GameplayDoFX(pDllPlot.get());
			}
		}
		break;
	}
}

bool CvNotifications::IsNotificationRedundant (Notification& notification)
{
	switch (notification.m_eNotificationType)
	{
	case NOTIFICATION_TECH:
		{
			int iIndex = m_iNotificationsBeginIndex;
			while (iIndex != m_iNotificationsEndIndex)
			{
				if (notification.m_eNotificationType == m_aNotifications[iIndex].m_eNotificationType)
				{
					if (!notification.m_bDismissed && !m_aNotifications[iIndex].m_bDismissed)
					{
						// we've already added a tech notification, don't need another
						return true;
					}
				}


				iIndex++;
				if (iIndex >= (int)m_aNotifications.size())
				{
					iIndex = 0;
				}
			}

			return false;
		}
		break;

	case NOTIFICATION_FREE_TECH:
		{
			int iIndex = m_iNotificationsBeginIndex;
			while (iIndex != m_iNotificationsEndIndex)
			{
				if (notification.m_eNotificationType == m_aNotifications[iIndex].m_eNotificationType)
				{
					if (!notification.m_bDismissed && !m_aNotifications[iIndex].m_bDismissed)
					{
						// we've already added a free tech notification, don't need another
						return true;
					}
				}


				iIndex++;
				if (iIndex >= (int)m_aNotifications.size())
				{
					iIndex = 0;
				}
			}

			return false;
		}
		break;

	case NOTIFICATION_POLICY:
		{
			int iIndex = m_iNotificationsBeginIndex;
			while (iIndex != m_iNotificationsEndIndex)
			{
				if (notification.m_eNotificationType == m_aNotifications[iIndex].m_eNotificationType)
				{
					if (!notification.m_bDismissed && !m_aNotifications[iIndex].m_bDismissed)
					{
						// we've already added a policy notification, don't need another
						return true;
					}
				}


				iIndex++;
				if (iIndex >= (int)m_aNotifications.size())
				{
					iIndex = 0;
				}
			}

			return false;
		}
		break;

	case NOTIFICATION_FREE_POLICY:
		{
			int iIndex = m_iNotificationsBeginIndex;
			while (iIndex != m_iNotificationsEndIndex)
			{
				if (notification.m_eNotificationType == m_aNotifications[iIndex].m_eNotificationType)
				{
					if (!notification.m_bDismissed && !m_aNotifications[iIndex].m_bDismissed)
					{
						// we've already added a tech notification, don't need another
						return true;
					}
				}


				iIndex++;
				if (iIndex >= (int)m_aNotifications.size())
				{
					iIndex = 0;
				}
			}

			return false;
		}
		break;

	case NOTIFICATION_PRODUCTION:
		{
			int iIndex = m_iNotificationsBeginIndex;
			while (iIndex != m_iNotificationsEndIndex)
			{
				if (notification.m_eNotificationType == m_aNotifications[iIndex].m_eNotificationType &&
					notification.m_iX == m_aNotifications[iIndex].m_iX &&
					notification.m_iY == m_aNotifications[iIndex].m_iY)
				{
					if (!notification.m_bDismissed && !m_aNotifications[iIndex].m_bDismissed)
					{
						// we've already added a notification for this city to the notification system, so don't add another one
						return true;
					}
				}

				iIndex++;
				if (iIndex >= (int)m_aNotifications.size())
				{
					iIndex = 0;
				}
			}

			return false;
		}
		break;

	case NOTIFICATION_ENEMY_IN_TERRITORY:
		{
			int iIndex = m_iNotificationsBeginIndex;
			while (iIndex != m_iNotificationsEndIndex)
			{
				// Only one "enemy in territory" notification at a time
				if (notification.m_eNotificationType == m_aNotifications[iIndex].m_eNotificationType)
				{
					if (!notification.m_bDismissed && !m_aNotifications[iIndex].m_bDismissed)
					{
						return true;
					}
				}

				iIndex++;
				if (iIndex >= (int)m_aNotifications.size())
				{
					iIndex = 0;
				}
			}

			return false;
		}
		break;

	case NOTIFICATION_UNIT_PROMOTION:
		{
			int iIndex = m_iNotificationsBeginIndex;
			while (iIndex != m_iNotificationsEndIndex)
			{
				if (notification.m_eNotificationType == m_aNotifications[iIndex].m_eNotificationType)
				{
					if (notification.m_iExtraGameData == m_aNotifications[iIndex].m_iExtraGameData)
					{
						if (!notification.m_bDismissed && !m_aNotifications[iIndex].m_bDismissed)
						{
							// we've already added a notification for this unit to the notification system, so don't add another one
							return true;
						}
					}
				}

				iIndex++;
				if (iIndex >= (int)m_aNotifications.size())
				{
					iIndex = 0;
				}
			}

			return false;
		}
		break;

	case NOTIFICATION_DIPLOMACY_DECLARATION:
		{
			PlayerTypes eOurPlayer1 = (PlayerTypes) notification.m_iGameDataIndex;
			PlayerTypes eOurPlayer2 = (PlayerTypes) notification.m_iExtraGameData;

			// Notification is NOT being used to inform of a DoF or Denouncement (otherwise there would be valid players in these slots)
			if (eOurPlayer1 == -1 || eOurPlayer2 == -1)
				return false;

			PlayerTypes eCheckingPlayer1, eCheckingPlayer2;

			int iIndex = m_iNotificationsBeginIndex;
			while (iIndex != m_iNotificationsEndIndex)
			{
				if (notification.m_eNotificationType == m_aNotifications[iIndex].m_eNotificationType)
				{
					eCheckingPlayer1 = (PlayerTypes) m_aNotifications[iIndex].m_iGameDataIndex;
					eCheckingPlayer2 = (PlayerTypes) m_aNotifications[iIndex].m_iExtraGameData;

					// Players match - we already have a notification with this player combo
					if ((eOurPlayer1 == eCheckingPlayer1 && eOurPlayer2 == eCheckingPlayer2) ||
						(eOurPlayer1 == eCheckingPlayer2 && eOurPlayer2 == eCheckingPlayer1))
					{
						return true;
					}
				}

				iIndex++;
				if (iIndex >= (int)m_aNotifications.size())
					iIndex = 0;
			}

			return false;
		}
		break;

	default:
		return false;
		break;
	}
}

bool CvNotifications::IsNotificationExpired (int iIndex)
{
	switch (m_aNotifications[iIndex].m_eNotificationType)
	{
	case NOTIFICATION_BUY_TILE:
		{
			if (GET_PLAYER(m_ePlayer).GetTreasury()->GetGold() < GET_PLAYER(m_ePlayer).GetBuyPlotCost())
			{
				return true;
			}
		}
		break;

	case NOTIFICATION_BARBARIAN:
		{
			CvPlot* pPlot = GC.getMap().plot(m_aNotifications[iIndex].m_iX, m_aNotifications[iIndex].m_iY);
			if (!pPlot->HasBarbarianCamp())
			{
				return true;
			}
		}
		break;

	case NOTIFICATION_CITY_RANGE_ATTACK:
		{
			int iCityID = m_aNotifications[iIndex].m_iGameDataIndex;
			CvCity* pCity = GET_PLAYER(m_ePlayer).getCity(iCityID);

			if (pCity == NULL)
				return true;

			else if(!pCity->CanRangeStrikeNow())
				return true;
		}
		break;

	case NOTIFICATION_GOODY:
		{
			CvPlot* pPlot = GC.getMap().plot(m_aNotifications[iIndex].m_iX, m_aNotifications[iIndex].m_iY);
			if (!pPlot->isGoody(GET_PLAYER(m_ePlayer).getTeam()))
			{
				return true;
			}
		}
		break;

	case NOTIFICATION_TECH:
		{
			CvPlayerAI& kPlayer = GET_PLAYER(m_ePlayer);
			CvPlayerTechs* pkPlayerTechs = kPlayer.GetPlayerTechs();
			if (pkPlayerTechs->GetCurrentResearch() != NO_TECH)
			{
				return true;
			}

			int iNotificationIndex = m_iNotificationsBeginIndex;
			while (iNotificationIndex != m_iNotificationsEndIndex)
			{
				if (NOTIFICATION_FREE_TECH == m_aNotifications[iNotificationIndex].m_eNotificationType)
				{
					if (!m_aNotifications[iNotificationIndex].m_bDismissed)
					{
						return true;
					}
				}

				iNotificationIndex++;
				if (iNotificationIndex >= (int)m_aNotifications.size())
				{
					iNotificationIndex = 0;
				}
			}

			//Expire this notification if there are no more techs that can be researched at this time.
			return pkPlayerTechs->GetNumTechsCanBeResearched() == 0;
		}
		break;
	case NOTIFICATION_FREE_TECH:
		{
			CvPlayerAI& kPlayer = GET_PLAYER(m_ePlayer);
			if (kPlayer.GetNumFreeTechs() == 0)
			{
				return true;
			}
			else
			{
				//Expire this notification if there are no more techs that can be researched at this time.
				return kPlayer.GetPlayerTechs()->GetNumTechsCanBeResearched() == 0;
			}
		}
		break;

	case NOTIFICATION_FREE_POLICY:
		{
			if ((GET_PLAYER(m_ePlayer).getJONSCulture() < GET_PLAYER(m_ePlayer).getNextPolicyCost() && GET_PLAYER(m_ePlayer).GetNumFreePolicies() == 0) || GET_PLAYER(m_ePlayer).IsAnarchy())
			{
				return true;
			}
		}
		break;

	case NOTIFICATION_FREE_GREAT_PERSON:
		{
			if(GET_PLAYER(m_ePlayer).GetNumFreeGreatPeople() == 0)
			{
				return true;
			}
		}
		break;

	case NOTIFICATION_POLICY:
		{
			if ((GET_PLAYER(m_ePlayer).getJONSCulture() < GET_PLAYER(m_ePlayer).getNextPolicyCost() && GET_PLAYER(m_ePlayer).GetNumFreePolicies() == 0) || GET_PLAYER(m_ePlayer).IsAnarchy())
			{
				return true;
			}
		}
		break;
	case NOTIFICATION_PRODUCTION:
		{
			//CvCity* pCity = GET_PLAYER(m_ePlayer).getCity(m_aNotifications[iIndex].m_iGameDataIndex);
			CvCity* pCity = GC.getMap().plot(m_aNotifications[iIndex].m_iX, m_aNotifications[iIndex].m_iY)->getPlotCity();//GET_PLAYER(m_ePlayer).getCity(notification.m_iGameDataIndex);

			// if the city no longer exists
			if (!pCity)
			{
				return true;
			}

			// if the city is a puppet
			if (pCity->IsPuppet())
			{
				return true;
			}

			// if the city doesn't belong to the active player
			if (pCity->getOwner() != GC.getGame().getActivePlayer())
			{
				return true;
			}

			// City has chosen something
			if (pCity->getOrderQueueLength() > 0)
			{
				return true;
			}
		}
		break;
	case NOTIFICATION_DIPLO_VOTE:
		{
			TeamTypes eTeam = GET_PLAYER(m_ePlayer).getTeam();

			// Vote from this team registered
			if (GC.getGame().GetVoteCast(eTeam) != NO_TEAM)
			{
				return true;
			}

			// Votes from ALL teams registered. This is necessary in addition to the above if block, because if this player is the last to vote
			// then everything gets reset immediately, and it'll be NO_TEAM by the time this function is tested again
			if (GC.getGame().GetNumVictoryVotesExpected() == 0)
			{
				return true;
			}
		}
		break;
	case NOTIFICATION_UNIT_PROMOTION:
		{
			UnitHandle pUnit = GET_PLAYER(m_ePlayer).getUnit(m_aNotifications[iIndex].m_iExtraGameData);
			if (!pUnit || !pUnit->isPromotionReady())
			{
				return true;
			}
		}
		break;
	case NOTIFICATION_PLAYER_DEAL:
		{
			CvGame &game = GC.getGame();
			CvGameDeals *pDeals = game.GetGameDeals();

			if (!pDeals->ProposedDealExists(m_ePlayer, (PlayerTypes)(m_aNotifications[iIndex].m_iX)))
			{
				return true;
			}
		}
		break;
	case NOTIFICATION_PLAYER_DEAL_RECEIVED:
		{
			CvGame &game = GC.getGame();
			CvGameDeals *pDeals = game.GetGameDeals();

			if (!pDeals->ProposedDealExists((PlayerTypes)(m_aNotifications[iIndex].m_iX),  m_ePlayer))
			{
				return true;
			}
		}
		break;
	case NOTIFICATION_DEMAND_RESOURCE:
		{
			// if this is a "you ran out of this resource" demand resource.
			// I did this so not to break the save format
			if(m_aNotifications[iIndex].m_iX == -1 && m_aNotifications[iIndex].m_iY == -1)
			{
				if(GET_PLAYER(m_ePlayer).getNumResourceAvailable((ResourceTypes)m_aNotifications[iIndex].m_iGameDataIndex, true) >= 0)
				{
					return true;
				}
			}
		}
		break;

	case NOTIFICATION_PLAYER_CONNECTING:
	{
		if(!gDLL->IsPlayerHotJoining(m_aNotifications[iIndex].m_iGameDataIndex)){
			//Player has finished hot joining.  Remove this notification, we'll add a NOTIFICATION_PLAYER_RECONNECTED in NetProxy::OnHotJoinComplete().
			return true;
		}
	}
	break;

	default:	// don't expire
		{
			return false;
		}
		break;
	}

	return false;
}

bool CvNotifications::IsNotificationEndOfTurnExpired (int iIndex)
{
	switch (m_aNotifications[iIndex].m_eNotificationType)
	{
	case NOTIFICATION_POLICY:
	case NOTIFICATION_FREE_POLICY:
	case NOTIFICATION_TECH:
	case NOTIFICATION_FREE_TECH:
	case NOTIFICATION_PRODUCTION:
	case NOTIFICATION_DIPLO_VOTE:
    case NOTIFICATION_PLAYER_DEAL:
	case NOTIFICATION_PLAYER_DEAL_RECEIVED:
	case NOTIFICATION_FREE_GREAT_PERSON:
	case NOTIFICATION_PLAYER_RECONNECTED:
	case NOTIFICATION_PLAYER_DISCONNECTED:
	case NOTIFICATION_TURN_MODE_SEQUENTIAL:
	case NOTIFICATION_TURN_MODE_SIMULTANEOUS:
	case NOTIFICATION_HOST_MIGRATION:
	case NOTIFICATION_PLAYER_CONNECTING:
	case NOTIFICATION_PLAYER_KICKED:
		return false;
		break;

	default:
		return true;
		break;
	}
}

bool CvNotifications::IsArrayFull()
{
	int iAdjustedEndIndex = m_iNotificationsEndIndex + 1;
	if (iAdjustedEndIndex >= (int)m_aNotifications.size())
	{
		iAdjustedEndIndex = 0;
	}

	if (iAdjustedEndIndex == m_iNotificationsBeginIndex)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CvNotifications::RemoveOldestNotification ()
{
	// if the notification is somehow active, dismiss it
	if (!m_aNotifications[m_iNotificationsBeginIndex].m_bDismissed)
	{
		Dismiss(m_aNotifications[m_iNotificationsBeginIndex].m_iLookupIndex, /*bUserInvoked*/ false);
	}
	m_aNotifications[m_iNotificationsBeginIndex].Clear();
	IncrementBeginIndex();
}

void CvNotifications::IncrementBeginIndex ()
{
	m_iNotificationsBeginIndex++;
	if (m_iNotificationsBeginIndex >= (int)m_aNotifications.size())
	{
		m_iNotificationsBeginIndex = 0;
	}
}

void CvNotifications::IncrementEndIndex ()
{
	m_iNotificationsEndIndex++;
	if (m_iNotificationsEndIndex >= (int)m_aNotifications.size())
	{
		m_iNotificationsEndIndex = 0;
	}
}

//	---------------------------------------------------------------------------
// static
void CvNotifications::AddToPlayer(PlayerTypes ePlayer, NotificationTypes eNotificationType, const char* strMessage, const char* strSummary, int iX/*=-1*/, int iY/*=-1*/, int iGameDataIndex/*=-1*/, int iExtraGameData/*=-1*/)
{
	if(ePlayer != NO_PLAYER)
	{
		CvPlayer& kPlayer = GET_PLAYER(ePlayer);

		if(kPlayer.isLocalPlayer())
		{
			CvNotifications* pNotifications = kPlayer.GetNotifications();
			if(pNotifications)
			{
				pNotifications->Add(eNotificationType, strMessage, strSummary, iX, iY, iGameDataIndex, iExtraGameData);
			}
		}
	}
}
