/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvMinorCivAI.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvInternalGameCoreUtils.h"
#include "CvEnumSerialization.h"
#include "CvNotifications.h"
#include "CvDiplomacyAI.h"
#include "CvDllInterfaces.h"

// must be included after all other headers
#include "LintFree.h"

//======================================================================================================
//					CvMinorCivAI
//======================================================================================================
CvMinorCivAI::CvMinorCivAI ()
{
}
//------------------------------------------------------------------------------
CvMinorCivAI::~CvMinorCivAI (void)
{
	Uninit();
}

/// Initialize
void CvMinorCivAI::Init(CvPlayer *pPlayer)
{
	m_pPlayer = pPlayer;
	m_minorCivType = CvPreGame::minorCivType(m_pPlayer->GetID());

	Reset();
}

/// Deallocate memory created in initialize
void CvMinorCivAI::Uninit()
{
}

/// Reset AIStrategy status array to all false
void CvMinorCivAI::Reset()
{
	m_ePersonality = NO_MINOR_CIV_PERSONALITY_TYPE;
	m_eStatus = NO_MINOR_CIV_STATUS_TYPE;

	m_iTurnsSinceThreatenedByBarbarians = -1;

	m_eAlly = NO_PLAYER;

	int iI, iJ;

	for (iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		m_abWarQuestAgainstMajor[iI] = false;

		for (iJ = 0; iJ < MAX_MAJOR_CIVS; iJ++)
		{
			m_aaiNumEnemyUnitsLeftToKillByMajor[iI][iJ] = -1;
		}

		m_abRouteConnectionEstablished[iI] = false;

		m_aiFriendshipWithMajorTimes100[iI] = 0;
		m_aiAngerFreeIntrusionCounter[iI] = 0;
		m_aiPlayerQuests[iI] = NO_MINOR_CIV_QUEST_TYPE;
		m_aiQuestData1[iI] = -1;
		m_aiQuestData2[iI] = -1;
		m_aiQuestCountdown[iI] = -1;
		m_aiUnitSpawnCounter[iI] = -1;
		m_aiNumUnitsGifted[iI] = 0;
		m_abUnitSpawningDisabled[iI] = false;
		m_abMajorIntruding[iI] = false;
		m_abEverFriends[iI] = false;
		m_aiMajorScratchPad[iI] = 0;
	}

	for (iI = 0; iI < REALLY_MAX_TEAMS; iI++)
	{
		m_abPermanentWar[iI] = false;
	}

	if (GetPlayer()->isMinorCiv())
	{
		CvPlot* pLoopPlot;
		TeamTypes eTeam = GetPlayer()->getTeam();
		int iNumPlotsInEntireWorld = GC.getMap().numPlots();
		for (int iLoopPlot = 0; iLoopPlot < iNumPlotsInEntireWorld; iLoopPlot++)
		{
			pLoopPlot = GC.getMap().plotByIndexUnchecked(iLoopPlot);
			if (pLoopPlot)
				pLoopPlot->setRevealed(eTeam, true);
		}
	}

}

/// Serialization read
void CvMinorCivAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_ePersonality;
	kStream >> m_eStatus;

	kStream >> m_iTurnsSinceThreatenedByBarbarians;

	kStream >> m_eAlly;

	kStream >> m_abWarQuestAgainstMajor;

	kStream >> m_aaiNumEnemyUnitsLeftToKillByMajor;

	kStream >> m_abRouteConnectionEstablished;

	kStream >> m_aiFriendshipWithMajorTimes100;

	if (uiVersion >= 5)
		kStream >> m_aiAngerFreeIntrusionCounter;
	else
	{
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			m_aiAngerFreeIntrusionCounter[iPlayerLoop] = 0;
	}

	kStream >> m_aiPlayerQuests;
	kStream >> m_aiQuestData1;
	kStream >> m_aiQuestData2;
	kStream >> m_aiQuestCountdown;
	kStream >> m_aiUnitSpawnCounter;

	if (uiVersion > 3)
		kStream >> m_aiNumUnitsGifted;
	else
	{
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			m_aiNumUnitsGifted[iPlayerLoop] = 0;
	}

	if (uiVersion <= 1)
	{
//		int aiGreatPeopleSpawnCounter[MAX_MAJOR_CIVS];
//		kStream >> aiGreatPeopleSpawnCounter;
	}
	kStream >> m_abUnitSpawningDisabled;
	kStream >> m_abMajorIntruding;
	kStream >> m_abEverFriends;
	kStream >> m_abPermanentWar;
}

/// Serialization write
void CvMinorCivAI::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 5;
	kStream << uiVersion;

	kStream << m_ePersonality;
	kStream << m_eStatus;

	kStream << m_iTurnsSinceThreatenedByBarbarians;

	kStream << m_eAlly;

	kStream << m_abWarQuestAgainstMajor;

	kStream << m_aaiNumEnemyUnitsLeftToKillByMajor;

	kStream << m_abRouteConnectionEstablished;

	kStream << m_aiFriendshipWithMajorTimes100;
	kStream << m_aiAngerFreeIntrusionCounter;
	kStream << m_aiPlayerQuests;
	kStream << m_aiQuestData1;
	kStream << m_aiQuestData2;
	kStream << m_aiQuestCountdown;
	kStream << m_aiUnitSpawnCounter;
	kStream << m_aiNumUnitsGifted;
	kStream << m_abUnitSpawningDisabled;
	kStream << m_abMajorIntruding;
	kStream << m_abEverFriends;
	kStream << m_abPermanentWar;
}

/// Returns the Player object this MinorCivAI is associated with
CvPlayer* CvMinorCivAI::GetPlayer()
{
	return m_pPlayer;
}

/// Returns the MinorCivType this Minor is playing as (e.g. Scotland, Switzerland, etc.)
MinorCivTypes CvMinorCivAI::GetMinorCivType() const
{
	//	return m_minorCivType;
	return CvPreGame::minorCivType(m_pPlayer->GetID());
}

/// What is the personality of this Minor
MinorCivPersonalityTypes CvMinorCivAI::GetPersonality() const
{
	return m_ePersonality;
}

/// Picks a random Personality for this minor
void CvMinorCivAI::DoPickPersonality()
{

	FlavorTypes eFlavorCityDefense = NO_FLAVOR;
	FlavorTypes eFlavorDefense = NO_FLAVOR;
	FlavorTypes eFlavorOffense = NO_FLAVOR;
	for (int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_CITY_DEFENSE")
		{
			eFlavorCityDefense = (FlavorTypes)iFlavorLoop;
		}
		if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_DEFENSE")
		{
			eFlavorDefense = (FlavorTypes)iFlavorLoop;
		}
		if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_OFFENSE")
		{
			eFlavorOffense = (FlavorTypes)iFlavorLoop;
		}
	}

	CvFlavorManager* pFlavorManager = m_pPlayer->GetFlavorManager();
	int* pFlavors = pFlavorManager->GetAllPersonalityFlavors();

	MinorCivPersonalityTypes eRandPersonality = (MinorCivPersonalityTypes) GC.getGame().getJonRandNum(NUM_MINOR_CIV_PERSONALITY_TYPES, "Minor Civ AI: Picking Personality for this Game (should happen only once per player)");
	m_ePersonality = eRandPersonality;

	switch (eRandPersonality)
	{
	case MINOR_CIV_PERSONALITY_FRIENDLY:
		pFlavors[eFlavorCityDefense] = pFlavorManager->GetAdjustedValue(pFlavors[eFlavorCityDefense], -2, 0, 10);
		pFlavors[eFlavorDefense] = pFlavorManager->GetAdjustedValue(pFlavors[eFlavorDefense], -2, 0, 10);
		pFlavors[eFlavorOffense] = pFlavorManager->GetAdjustedValue(pFlavors[eFlavorOffense], -2, 0, 10);
		pFlavorManager->ResetToBasePersonality();
		break;
	case MINOR_CIV_PERSONALITY_HOSTILE:
		pFlavors[eFlavorCityDefense] = pFlavorManager->GetAdjustedValue(pFlavors[eFlavorCityDefense], 2, 0, 10);
		pFlavors[eFlavorDefense] = pFlavorManager->GetAdjustedValue(pFlavors[eFlavorDefense], 2, 0, 10);
		pFlavors[eFlavorOffense] = pFlavorManager->GetAdjustedValue(pFlavors[eFlavorOffense], 2, 0, 10);
		pFlavorManager->ResetToBasePersonality();
		break;
	}
}

/// What is this civ's trait?
MinorCivTraitTypes CvMinorCivAI::GetTrait() const
{
	CvMinorCivInfo* pkMinorCivInfo = GC.getMinorCivInfo(GetMinorCivType());
	if(pkMinorCivInfo)
	{
		return (MinorCivTraitTypes) pkMinorCivInfo->GetMinorCivTrait();
	}

	return NO_MINOR_CIV_TRAIT_TYPE;
}


// ******************************
// Main functions
// ******************************


/// Processed every turn
void CvMinorCivAI::DoTurn()
{
	if (GetPlayer()->isMinorCiv())
	{
		DoTurnStatus();

		DoFriendship();

		DoTestBarbarianQuest();
		DoTestWarWithMajorQuest();
		DoTestProxyWarNotification();

		DoTurnQuests();

		DoUnitSpawnTurn();

		DoIntrusion();
	}
}


/// Minor is now dead or alive (haha, get it?)
void CvMinorCivAI::DoChangeAliveStatus(bool bAlive)
{
	bool bFriends;
	bool bAllies;

	int iBestFriendship = 0;
	PlayerTypes eBestPlayer = NO_PLAYER;

	PlayerTypes ePlayer;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		bFriends = false;
		bAllies = false;

		// Add or remove bonuses depending on bAlive status
		if (IsFriendshipAboveFriendsThreshold(GetFriendshipWithMajor(ePlayer)))
			bFriends = true;

		if (GetFriendshipWithMajor(ePlayer) > iBestFriendship)
		{
			iBestFriendship = GetFriendshipWithMajor(ePlayer);
			eBestPlayer = ePlayer;
		}

		if (GetAlly() == ePlayer)
		{
			bAllies = true;
			eBestPlayer = ePlayer;
		}

		if (bFriends || bAllies)
			DoSetBonus(ePlayer, bAlive, bFriends, bAllies);

		// If we're not alive we can't have a quest to fight a major off
		if (!bAlive)
		{
			SetWarQuestWithMajorActive(ePlayer, false);
			EndActiveQuestForPlayer(ePlayer);
		}
	}

	// Now alive
	if (bAlive)
	{
		if (GetAlly() == NO_PLAYER)
		{
			if (eBestPlayer != NO_PLAYER && iBestFriendship > GetAlliesThreshold())
			{
				DoSetBonus(eBestPlayer, bAlive, /*bFriends*/ false, /*bAllies*/ true);
				SetAlly(eBestPlayer);
			}
		}
	}
	// Now dead
	else
	{
		SetTurnsSinceThreatenedByBarbarians(-1);
	}
}


/// First contact
void CvMinorCivAI::DoFirstContactWithMajor(TeamTypes eTeam)
{
	// This guy's a warmonger so we DoW him
	if (IsPeaceBlocked(eTeam))
	{
		GET_TEAM(GetPlayer()->getTeam()).declareWar(eTeam);
	}
	// Normal diplo
	else
	{
		int iGoldGift = 0;
		bool bFirstMajorCiv = false;

		// If this guy has been mean then no Gold gifts
		if (!GET_TEAM(eTeam).IsMinorCivAggressor())
		{
			// Hasn't met anyone yet?
			if (GET_TEAM(GetPlayer()->getTeam()).getHasMetCivCount(true) == 0)
			{
				iGoldGift = /*60*/ GC.getMINOR_CIV_CONTACT_GOLD_FIRST();
				bFirstMajorCiv = true;
			}
			else
			{
				iGoldGift = /*30*/ GC.getMINOR_CIV_CONTACT_GOLD_OTHER();
			}
		}

		PlayerTypes eEnemy;
		int iEnemyLoop;

		PlayerTypes ePlayer;
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			ePlayer = (PlayerTypes) iPlayerLoop;

			if (GET_PLAYER(ePlayer).getTeam() == eTeam)
			{
				// Gold gift
				GET_PLAYER(ePlayer).GetTreasury()->ChangeGold(iGoldGift);

				// Need to seed quest counter?
				if (GC.getGame().getElapsedGameTurns() > GetFirstPossibleTurnForQuests())
				{
					DoSeedQuestCountdown(ePlayer);
				}

				// Barbarian Quest is already active, let the player know!
				if (GetTurnsSinceThreatenedByBarbarians() >= 0 && GetTurnsSinceThreatenedByBarbarians() < 10)
				{
					// Don't show notification if player is too far away
					if (IsPlayerCloseEnoughForBarbQuestAnnouncement(ePlayer))
						DoLaunchBarbariansQuestForPlayerNotification(ePlayer);
				}

				// Humans intrude automatically upon meeting
				if (GET_PLAYER(ePlayer).isHuman())
				{
					SetMajorIntruding(ePlayer, true);
				}
				else
				{
					SetMajorIntruding(ePlayer, false);
				}

				// Notification for War quest
				for (iEnemyLoop = 0; iEnemyLoop < MAX_MAJOR_CIVS; iEnemyLoop++)
				{
					eEnemy = (PlayerTypes) iEnemyLoop;

					// Quest is active
					if (IsWarQuestWithMajorActive(eEnemy))
					{
						// Still at war
						if (IsAtWarWithPlayersTeam(eEnemy))
						{
							DoLaunchWarQuestForPlayerNotification(ePlayer, eEnemy);
						}
					}
				}

				// Greeting for active human player
				if (ePlayer == GC.getGame().getActivePlayer())
				{
					if (!GC.getGame().isNetworkMultiPlayer())	// KWG: Should this be !GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS)
					{
						CvPopupInfo kPopupInfo(BUTTONPOPUP_CITY_STATE_GREETING, GetPlayer()->GetID(), iGoldGift, -1, 0, bFirstMajorCiv);
						GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
						// We are adding a popup that the player must make a choice in, make sure they are not in the end-turn phase.
						CancelActivePlayerEndTurn();
					}

					// update the mouseover text for the city-state's city banners
					int iLoop = 0;
					CvCity* pLoopCity = NULL;
					for (pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
					{
						if (pLoopCity->plot()->isRevealed(eTeam))
						{
							auto_ptr<ICvCity1> pDllLoopCity = GC.WrapCityPointer(pLoopCity);
							GC.GetEngineUserInterface()->SetSpecificCityInfoDirty(pDllLoopCity.get(), CITY_UPDATE_TYPE_BANNER);
						}
					}
				}
			}
		}
	}
}

/// Are we at war with a minor and not allied with anyone?
void CvMinorCivAI::DoTestEndWarsVSMinors(PlayerTypes eOldAlly, PlayerTypes eNewAlly)
{
	if (eOldAlly == NO_PLAYER)
		return;

	if (!GetPlayer()->isAlive())
		return;

	PlayerTypes eOtherMinor;
	int iOtherMinorLoop;
	PlayerTypes eOtherAlly;
	bool bForcedWar;

	TeamTypes eLoopTeam;
	for (int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
	{
		eLoopTeam = (TeamTypes) iTeamLoop;

		// Another Minor
		if (!GET_TEAM(eLoopTeam).isMinorCiv())
			continue;

		// They not alive!
		if (!GET_TEAM(eLoopTeam).isAlive())
			continue;

		// At war with him
		if (!GET_TEAM(GetPlayer()->getTeam()).isAtWar(eLoopTeam))
			continue;

		if (eOldAlly != NO_PLAYER)
		{
			// Old ally wasn't at war
			if (!GET_TEAM(GET_PLAYER(eOldAlly).getTeam()).isAtWar(eLoopTeam))
				continue;
		}

		if (eNewAlly != NO_PLAYER)
		{
			// New ally IS at war
			if (GET_TEAM(GET_PLAYER(eNewAlly).getTeam()).isAtWar(eLoopTeam))
				continue;
		}

		// Make sure this guy isn't allied with someone at war with us
		bForcedWar = false;

		for (iOtherMinorLoop = 0; iOtherMinorLoop < MAX_CIV_TEAMS; iOtherMinorLoop++)
		{
			eOtherMinor = (PlayerTypes) iOtherMinorLoop;

			// Other minor is on this team
			if (GET_PLAYER(eOtherMinor).getTeam() == eLoopTeam)
			{
				eOtherAlly = GET_PLAYER(eOtherMinor).GetMinorCivAI()->GetAlly();
				if (eOtherAlly != NO_PLAYER)
				{
					// This guy's ally at war with us?
					if (GET_TEAM(GET_PLAYER(eOtherAlly).getTeam()).isAtWar(GetPlayer()->getTeam()))
					{
						bForcedWar = true;
						break;
					}
				}
			}

		}

		if (bForcedWar)
			continue;

		if (IsPermanentWar(eLoopTeam))
			continue;

		GET_TEAM(GetPlayer()->getTeam()).makePeace(eLoopTeam);
	}
}

/// Update what our status is
void CvMinorCivAI::DoTurnStatus()
{
	int iWeight = 0;

	PlayerProximityTypes eProximity;

	CvPlayer* pPlayer;
	CvTeam* pTeam;
	PlayerTypes ePlayer;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;
		pPlayer = &GET_PLAYER(ePlayer);
		pTeam = &GET_TEAM(pPlayer->getTeam());

		eProximity = pPlayer->GetProximityToPlayer(GetPlayer()->GetID());

		// Check how close the player is
		switch (eProximity)
		{
		// DISTANT: Elevated if we're at war
		case PLAYER_PROXIMITY_DISTANT:
			break;
// 			if (IsAtWarWithPlayersTeam(ePlayer))
// 			{
// 				iWeight += 10;
// 			}

		// FAR: Elevated if they're an aggressor OR we're at war (note the ELSE IF)
		case PLAYER_PROXIMITY_FAR:
			break;
// 			if (pTeam->IsMinorCivAggressor())
// 			{
// 				iWeight += 10;
// 			}
// 			else if (IsAtWarWithPlayersTeam(ePlayer))
// 			{
// 				iWeight += 10;
// 			}

		// CLOSE: Elevated if they're an aggressor, critical if we're at war
		case PLAYER_PROXIMITY_CLOSE:
			if (pTeam->IsMinorCivAggressor())
			{
				iWeight += 10;
			}
			if (pTeam->IsMinorCivWarmonger())
			{
				iWeight += 20;
			}
			break;

		// NEIGHBORS: Pretty much anything makes the situation critical
		case PLAYER_PROXIMITY_NEIGHBORS:
			if (pTeam->IsMinorCivAggressor())
			{
				iWeight += 20;
			}
			if (IsAtWarWithPlayersTeam(ePlayer))
			{
				iWeight += 20;
			}
			break;
		default:
			break;
		}
	}

	// Do the final math
	if (iWeight >= 20)
	{
		m_eStatus = MINOR_CIV_STATUS_CRITICAL;
	}
	else if (iWeight >= 10)
	{
		m_eStatus = MINOR_CIV_STATUS_ELEVATED;
	}
	else
	{
		m_eStatus = MINOR_CIV_STATUS_NORMAL;
	}
}

/// What is our status
MinorCivStatusTypes CvMinorCivAI::GetStatus() const
{
	return m_eStatus;
}


/// Notifications
void CvMinorCivAI::AddNotification(Localization::String strString, Localization::String strSummaryString, PlayerTypes ePlayer, int iX, int iY)
{
	if (iX == -1 && iY == -1)
	{
		CvCity* capCity = GetPlayer()->getCapitalCity();

		if (capCity != NULL)
		{
			iX = capCity->getX();
			iY = capCity->getY();
		}
	}

	CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
	if (pNotifications)
	{
		pNotifications->Add(NOTIFICATION_MINOR, strString.toUTF8(), strSummaryString.toUTF8(), iX, iY, GetPlayer()->GetID());
	}
}

/// Quest Notifications
void CvMinorCivAI::AddQuestNotification(Localization::String strString, Localization::String strSummaryString, PlayerTypes ePlayer, int iX, int iY)
{
	if (iX == -1 && iY == -1)
	{
		CvCity* capCity = GetPlayer()->getCapitalCity();

		if (capCity != NULL)
		{
			iX = capCity->getX();
			iY = capCity->getY();
		}
	}

	CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
	if (pNotifications)
	{
		CvString pTempString = strString.toUTF8();
		pTempString += "[NEWLINE][NEWLINE]";
		pTempString += Localization::Lookup("TXT_KEY_MINOR_QUEST_BLOCKING_TT").toUTF8();

		pNotifications->Add(NOTIFICATION_MINOR_QUEST, pTempString, strSummaryString.toUTF8(), iX, iY, GetPlayer()->GetID());
	}
}



// ******************************
// Fend off Barbarians
// ******************************


/// Barbarians threatening this Minor?
void CvMinorCivAI::DoTestBarbarianQuest()
{
	// Increment counter - this is only used when sending notifications to players
	if (GetTurnsSinceThreatenedByBarbarians() >= 0)
	{
		ChangeTurnsSinceThreatenedByBarbarians(1);

		// Long enough to have expired?
		if (GetTurnsSinceThreatenedByBarbarians() >= 30)
			SetTurnsSinceThreatenedByBarbarians(-1);
	}

	// Not already threatened?
	if (GetTurnsSinceThreatenedByBarbarians() == -1)
	{
		if (GetNumThreateningBarbarians() >= 2)
		{
			// Wasn't under attack before, but is now!
			SetTurnsSinceThreatenedByBarbarians(0);

			PlayerTypes ePlayer;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				ePlayer = (PlayerTypes) iPlayerLoop;

				if (IsPlayerCloseEnoughForBarbQuestAnnouncement(ePlayer))
				{
					DoLaunchBarbariansQuestForPlayerNotification(ePlayer);
				}
			}
		}
	}
}

/// Barbs in or near our borders?
int CvMinorCivAI::GetNumThreateningBarbarians()
{
	int iCount = 0;

	int iLoop;
	for (CvUnit* pLoopUnit = GET_PLAYER(BARBARIAN_PLAYER).firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = GET_PLAYER(BARBARIAN_PLAYER).nextUnit(&iLoop))
	{
		if (pLoopUnit->IsBarbarianUnitThreateningMinor(GetPlayer()->GetID()))
			iCount++;
	}

	return iCount;
}

/// Player killed a threatening barb, so reward him!
void CvMinorCivAI::DoThreateningBarbKilled(PlayerTypes eKillingPlayer, int iX, int iY)
{
	CvAssertMsg(eKillingPlayer >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eKillingPlayer < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");

	ChangeFriendshipWithMajor(eKillingPlayer, /*5*/ GC.getFRIENDSHIP_PER_BARB_KILLED());

	ChangeAngerFreeIntrusionCounter(eKillingPlayer, 5);

	Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BARB_KILLED");
	strMessage << GetPlayer()->getNameKey();
	Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SM_MINOR_BARB_KILLED");
	strSummary << GetPlayer()->getNameKey();

	AddNotification(strMessage, strSummary, eKillingPlayer, iX, iY);
}

/// How long has this Minor been under attack from Barbs?
int CvMinorCivAI::GetTurnsSinceThreatenedByBarbarians() const
{
	return m_iTurnsSinceThreatenedByBarbarians;
}

/// How long has this Minor been under attack from Barbs?
void CvMinorCivAI::SetTurnsSinceThreatenedByBarbarians(int iValue)
{
	if (GetTurnsSinceThreatenedByBarbarians() != iValue)
		m_iTurnsSinceThreatenedByBarbarians = iValue;
}

/// How long has this Minor been under attack from Barbs?
void CvMinorCivAI::ChangeTurnsSinceThreatenedByBarbarians(int iChange)
{
	SetTurnsSinceThreatenedByBarbarians(GetTurnsSinceThreatenedByBarbarians() + iChange);
}


/// Is player close enough for the game to tell him about the Barb Quest
bool CvMinorCivAI::IsPlayerCloseEnoughForBarbQuestAnnouncement(PlayerTypes eMajor)
{
	CvCity* pCapital = GetPlayer()->getCapitalCity();

	// Minor must have Capital
	if (pCapital == NULL)
	{
		return false;
	}

	// Has Minor met this player yet?
	if (IsHasMetPlayer(eMajor))
	{
		bool bCloseEnoughForQuest = false;

		CvCity* pMajorsCapital = GET_PLAYER(eMajor).getCapitalCity();

		if (pMajorsCapital != NULL)
		{
			if (pCapital->getArea() == pMajorsCapital->getArea())
			{
				return true;
			}

			if (!bCloseEnoughForQuest)
			{
				int iDistance = plotDistance(pCapital->getX(), pCapital->getY(), pMajorsCapital->getX(), pMajorsCapital->getY());

				if (iDistance <= /*50*/ GC.getMAX_DISTANCE_MINORS_BARB_QUEST())
				{
					return true;
				}
			}
		}
	}

	return false;
}

/// Launch Barbarians Quest
void CvMinorCivAI::DoLaunchBarbariansQuestForPlayerNotification(PlayerTypes ePlayer)
{
	if (ePlayer == GC.getGame().getActivePlayer())
	{
		if (!IsAtWarWithPlayersTeam(ePlayer))
		{
			Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BARBS_QUEST");
			strMessage << GetPlayer()->getNameKey();
			Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_BARBS_QUEST");
			strSummary << GetPlayer()->getNameKey();

			AddQuestNotification(strMessage, strSummary, ePlayer);
		}
	}
}



// ******************************
// ***** War with Major Quest *****
// ******************************



/// Quest to help defend from war with Major
void CvMinorCivAI::DoTestWarWithMajorQuest()
{
	TeamTypes eEnemyTeam;
	PlayerTypes eEnemy;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eEnemy = (PlayerTypes) iPlayerLoop;
		eEnemyTeam = GET_PLAYER(eEnemy).getTeam();

		bool bAtWar = IsAtWarWithPlayersTeam(eEnemy);

		// Quest is active
		if (IsWarQuestWithMajorActive(eEnemy))
		{
			// If they're dead now, cancel the quest
			if (!GET_PLAYER(eEnemy).isAlive())
				SetWarQuestWithMajorActive(eEnemy, false);

			// Not at war any more
			if (!bAtWar)
			{
				SetWarQuestWithMajorActive(eEnemy, false);

				if (GET_TEAM(GetPlayer()->getTeam()).isHasMet(GC.getGame().getActiveTeam()))
				{
					if (!IsAtWarWithPlayersTeam(GC.getGame().getActivePlayer()) && eEnemyTeam != GC.getGame().getActiveTeam())
					{
						Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_WAR_QUEST_LOST_CHANCE");
						strMessage << GetPlayer()->getNameKey() << GET_PLAYER(eEnemy).getNameKey();
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_WAR_QUEST_LOST_CHANCE");
						strSummary << GetPlayer()->getNameKey();

						AddQuestNotification(strMessage, strSummary, GC.getGame().getActivePlayer());
					}
				}
			}
		}
	}
}


/// Time to send out a "Help us with Units" notification?
void CvMinorCivAI::DoTestProxyWarNotification()
{
	for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
		PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
		CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
		CvTeam* pNotifyTeam = &GET_TEAM(kCurNotifyPlayer.getTeam());

		Localization::String strMessage;
		Localization::String strSummary;

		if(pNotifyTeam->isHasMet(GetPlayer()->getTeam()))
		{
			PlayerTypes eEnemyLeader;
			TeamTypes eEnemyTeam;
			for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
			{
				PlayerTypes eMajorLoop = (PlayerTypes) iMajorLoop;
				CvPlayer* pMajorLoop = &GET_PLAYER(eMajorLoop);
				CvAssertMsg(pMajorLoop, "Error sending out proxy war notification from a city-state. Please send Anton your save file and version.");
				if (pMajorLoop)
				{
					eEnemyTeam = pMajorLoop->getTeam();

					// Minor is at war
					if(GET_TEAM(GetPlayer()->getTeam()).isAtWar(eEnemyTeam))
					{
						// Notify player must NOT be at war with either the major or the minor
						if(!pNotifyTeam->isAtWar(eEnemyTeam) && !pNotifyTeam->isAtWar(GetPlayer()->getTeam()))
						{
							// Don't send out notification here for Warmonger - we centralize this elsewhere (so that players don't get spammed with 10 Notifications)
							if(!IsPeaceBlocked(eEnemyTeam))
							{
								if(GET_TEAM(GetPlayer()->getTeam()).GetNumTurnsAtWar(eEnemyTeam) == /*2*/ GC.getTXT_KEY_MINOR_GIFT_UNITS_REMINDER())  //antonjs: consider: this text key is hacked to act like a value, fix it for clarity
								{
									eEnemyLeader = GET_TEAM(eEnemyTeam).getLeaderID();
									CvPlayer* pEnemyLeader = &GET_PLAYER(eEnemyLeader);

									// Do some additional checks to safeguard against weird scenario cases (ex. major and minor on same team, major is dead)
									if (pEnemyLeader)
									{
										if (!pEnemyLeader->isMinorCiv() && pEnemyLeader->isAlive())
										{
											strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_WAR_UNIT_HELP");
											strMessage << GetPlayer()->getCivilizationShortDescriptionKey() << pEnemyLeader->getCivilizationShortDescriptionKey();
											strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_WAR_UNIT_HELP");
											strSummary << GetPlayer()->getCivilizationShortDescriptionKey();

											AddQuestNotification(strMessage.toUTF8(), strSummary.toUTF8(), eNotifyPlayer);
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}


/// Quest to help defend from war with Major
void CvMinorCivAI::DoLaunchWarWithMajorQuest(TeamTypes eAttackingTeam)
{
	int iQuestPlayerLoop;
	PlayerTypes eQuestPlayer;

	PlayerTypes eEnemy;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eEnemy = (PlayerTypes) iPlayerLoop;

		if (GET_PLAYER(eEnemy).isAlive() && GET_PLAYER(eEnemy).getTeam() == eAttackingTeam)
		{
			SetWarQuestWithMajorActive(eEnemy, true);

			// Number of Units that must be killed to fulfill this quest
			int iNumUnitsToKill;
			int iTotalMilitaryUnits = 0;

			int iLoop;
			for (CvUnit* pLoopUnit = GET_PLAYER(eEnemy).firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = GET_PLAYER(eEnemy).nextUnit(&iLoop))
			{
				if (pLoopUnit->IsCombatUnit())
				{
					iTotalMilitaryUnits++;
				}
			}

			iNumUnitsToKill = iTotalMilitaryUnits / /*4*/ GC.getWAR_QUEST_UNITS_TO_KILL_DIVISOR();

			int iMinUnitsToKill = /*3*/ GC.getWAR_QUEST_MIN_UNITS_TO_KILL();
			if (iNumUnitsToKill < iMinUnitsToKill)
			{
				iNumUnitsToKill = iMinUnitsToKill;
			}

			for (iQuestPlayerLoop = 0; iQuestPlayerLoop < MAX_MAJOR_CIVS; iQuestPlayerLoop++)
			{
				eQuestPlayer = (PlayerTypes) iQuestPlayerLoop;
				SetNumEnemyUnitsLeftToKillByMajor(eQuestPlayer, eEnemy, iNumUnitsToKill);
			}

			if (GET_TEAM(GetPlayer()->getTeam()).isHasMet(GC.getGame().getActiveTeam()))
			{
				DoLaunchWarQuestForPlayerNotification(GC.getGame().getActivePlayer(), eEnemy);
			}
		}
	}
}

/// Is a Quest active to beat down on a Major we're at war with?
bool CvMinorCivAI::IsWarQuestWithAnyoneActive() const
{
	PlayerTypes ePlayer;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if (IsWarQuestWithMajorActive(ePlayer))
		{
			return true;
		}
	}

	return false;
}

/// Is a Quest active to beat down on a Major we're at war with?
bool CvMinorCivAI::IsWarQuestWithMajorActive(PlayerTypes eEnemy) const
{
	CvAssertMsg(eEnemy >= 0, "eEnemy is expected to be non-negative (invalid Index)");
	CvAssertMsg(eEnemy < MAX_MAJOR_CIVS, "eEnemy is expected to be within maximum bounds (invalid Index)");
	if(eEnemy < 0 || eEnemy >= MAX_MAJOR_CIVS) return false;

	return m_abWarQuestAgainstMajor[eEnemy];
}

/// Sets a Quest active to beat down on a Major we're at war with (or not)
void CvMinorCivAI::SetWarQuestWithMajorActive(PlayerTypes eEnemy, bool bValue)
{
	CvAssertMsg(eEnemy >= 0, "eEnemy is expected to be non-negative (invalid Index)");
	CvAssertMsg(eEnemy < MAX_MAJOR_CIVS, "eEnemy is expected to be within maximum bounds (invalid Index)");

	if(eEnemy < 0 || eEnemy >= MAX_MAJOR_CIVS) return;

	if (m_abWarQuestAgainstMajor[eEnemy] != bValue)
	{
		m_abWarQuestAgainstMajor[eEnemy] = bValue;

		// Quest turned off
		if (!bValue)
		{
			PlayerTypes ePlayer;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				ePlayer = (PlayerTypes) iPlayerLoop;

				SetNumEnemyUnitsLeftToKillByMajor(ePlayer, eEnemy, -1);
			}
		}
	}
}

/// Test condition
void CvMinorCivAI::DoUnitDeathWarQuestImplications(PlayerTypes eLosingPlayer, PlayerTypes eKillingPlayer)
{
	// If we haven't met this player then he can't complete quests for us!
	if (!IsHasMetPlayer(eKillingPlayer))
	{
		return;
	}

	if (eLosingPlayer < MAX_MAJOR_CIVS && eKillingPlayer < MAX_MAJOR_CIVS)
	{
		if (IsWarQuestWithMajorActive(eLosingPlayer))
		{
			// If we're already at war with someone then we don't want their help
			if (!IsAtWarWithPlayersTeam(eKillingPlayer))
			{
				ChangeNumEnemyUnitsLeftToKillByMajor(eKillingPlayer, eLosingPlayer, -1);

				// Mission accomplished!
				if (GetNumEnemyUnitsLeftToKillByMajor(eKillingPlayer, eLosingPlayer) == 0)
				{
					SetNumEnemyUnitsLeftToKillByMajor(eKillingPlayer, eLosingPlayer, -1);

					int iFriendshipChange = /*100*/ GC.getWAR_QUEST_COMPLETE_FRIENDSHIP();
					ChangeFriendshipWithMajor(eKillingPlayer, iFriendshipChange, /*bFromQuest*/ true);

					if (eKillingPlayer == GC.getGame().getActivePlayer())
					{
						Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_WAR_QUEST_COMPLETED");
						strMessage << GetPlayer()->getNameKey() << GET_PLAYER(eLosingPlayer).getNameKey();
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_WAR_QUEST_COMPLETED");
						strSummary << GetPlayer()->getNameKey();

						AddQuestNotification(strMessage, strSummary, eKillingPlayer);
					}
				}
			}
		}
	}
}

/// How many enemy Units has this Major killed for us?
int CvMinorCivAI::GetNumEnemyUnitsLeftToKillByMajor(PlayerTypes eMajor, PlayerTypes eEnemy) const
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return -1; // as set in Reset()

	CvAssertMsg(eEnemy >= 0, "eEnemy is expected to be non-negative (invalid Index)");
	CvAssertMsg(eEnemy < MAX_MAJOR_CIVS, "eEnemy is expected to be within maximum bounds (invalid Index)");
	if(eEnemy < 0 || eEnemy >= MAX_MAJOR_CIVS) return -1; // as set in Reset()

	return m_aaiNumEnemyUnitsLeftToKillByMajor[eMajor][eEnemy];
}

/// Sets how many enemy Units this Major has killed for us
void CvMinorCivAI::SetNumEnemyUnitsLeftToKillByMajor(PlayerTypes eMajor, PlayerTypes eEnemy, int iNum)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;

	CvAssertMsg(eEnemy >= 0, "eEnemy is expected to be non-negative (invalid Index)");
	CvAssertMsg(eEnemy < MAX_MAJOR_CIVS, "eEnemy is expected to be within maximum bounds (invalid Index)");
	if(eEnemy < 0 || eEnemy >= MAX_MAJOR_CIVS) return;

	m_aaiNumEnemyUnitsLeftToKillByMajor[eMajor][eEnemy] = iNum;
}

/// Changes how many enemy Units this Major has killed for us
void CvMinorCivAI::ChangeNumEnemyUnitsLeftToKillByMajor(PlayerTypes eMajor, PlayerTypes eEnemy, int iChange)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eEnemy >= 0, "eEnemy is expected to be non-negative (invalid Index)");
	CvAssertMsg(eEnemy < MAX_MAJOR_CIVS, "eEnemy is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		SetNumEnemyUnitsLeftToKillByMajor(eMajor, eEnemy, GetNumEnemyUnitsLeftToKillByMajor(eMajor, eEnemy) + iChange);
	}
}

/// Launch War Quest
void CvMinorCivAI::DoLaunchWarQuestForPlayerNotification(PlayerTypes ePlayer, PlayerTypes eEnemy)
{
	if (ePlayer == GC.getGame().getActivePlayer())
	{
		if (!IsAtWarWithPlayersTeam(ePlayer))
		{
			int iNumUnitsToKill = GetNumEnemyUnitsLeftToKillByMajor(ePlayer, eEnemy);

			Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_WAR_QUEST");
			strMessage << GetPlayer()->getNameKey() << GET_PLAYER(eEnemy).getCivilizationShortDescriptionKey() << iNumUnitsToKill;
			Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_WAR_QUEST");
			strSummary << GetPlayer()->getNameKey() << GET_PLAYER(eEnemy).getCivilizationShortDescriptionKey();

			AddQuestNotification(strMessage, strSummary, ePlayer);
		}
	}
}


// ******************************
// ***** Quests *****
// ******************************



/// Update turn for Quests
void CvMinorCivAI::DoTurnQuests()
{
	int iQuestFirstPossibleTurn = GetFirstPossibleTurnForQuests();

	// Loop through all majors, see if they're valid
	PlayerTypes ePlayer;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if (IsHasMetPlayer(ePlayer))
		{
			// Increment turns since last quest
			if (GetQuestCountdown(ePlayer) > 0)
			{
				ChangeQuestCountdown(ePlayer, -1);
			}

			// Unlock turn - seed the counter
			if (GC.getGame().getElapsedGameTurns() == iQuestFirstPossibleTurn)
			{
				DoSeedQuestCountdown(ePlayer);
			}
			// Can't start a quest too early
			else if (GC.getGame().getElapsedGameTurns() > iQuestFirstPossibleTurn || QUEST_DEBUGGING)
			{
				if (!IsAtWarWithPlayersTeam(ePlayer))
				{
					// No quest yet
					if (GetActiveQuestForPlayer(ePlayer) == NO_MINOR_CIV_QUEST_TYPE)
					{
						DoTestQuestStart(ePlayer);
					}
					// Active Quest
					else
					{
						DoTestQuestComplete(ePlayer);
					}
				}
			}

			DoTestQuestObsolete(ePlayer);
		}
	}
}

/// What is the first possible turn of the game we can fire off a Quest?
int CvMinorCivAI::GetFirstPossibleTurnForQuests()
{
	return /*30*/ GC.getMINOR_CIV_QUEST_FIRST_POSSIBLE_TURN();
}

/// Start a Quest?
void CvMinorCivAI::DoTestQuestStart(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	// Can't start a new quest too quickly after an old one has ended
	if (GetQuestCountdown(ePlayer) > 0)
	{
		return;
	}

	FStaticVector<MinorCivQuestTypes, NUM_MINOR_CIV_QUEST_TYPES, true, c_eCiv5GameplayDLL, 0> veValidQuests;

	int iCount, iCountLoop;

	MinorCivQuestTypes eQuest;
	for (int iQuestLoop = 0; iQuestLoop < NUM_MINOR_CIV_QUEST_TYPES; iQuestLoop++)
	{
		eQuest = (MinorCivQuestTypes) iQuestLoop;

		if (IsValidQuestForPlayer(ePlayer, eQuest))
		{
			// What is the bias for this player wanting this particular quest? Queue up multiple copies (default is 10)
			iCount = GetPersonalityQuestBias(eQuest);

			for (iCountLoop = 0; iCountLoop < iCount; iCountLoop++)
			{
				veValidQuests.push_back(eQuest);
			}
		}
	}

	// No valid Quests
	if (veValidQuests.size() == 0)
	{
		return;
	}

	int iRandIndex = GC.getGame().getJonRandNum(veValidQuests.size(), "Picking random quest for Minor to give to a player.");
	eQuest = veValidQuests[iRandIndex];

	DoStartQuest(ePlayer, eQuest);
}

/// Begin quest
void CvMinorCivAI::DoStartQuest(PlayerTypes ePlayer, MinorCivQuestTypes eQuest)
{
	SetActiveQuestForPlayer(ePlayer, eQuest);

	Localization::String strMessage;
	Localization::String strSummary;
	int iNotificationX = -1;
	int iNotificationY = -1;

	// Build a Route
	if (eQuest == MINOR_CIV_QUEST_ROUTE)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_START_ROUTE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_START_ROUTE");
	}
	// Kill a Camp
	else if (eQuest == MINOR_CIV_QUEST_KILL_CAMP)
	{
		CvPlot* pPlot = GetBestNearbyCampToKill();

		FAssertMsg(pPlot != NULL, "MINOR CIV AI: Somehow we're starting a quest to kill a barb camp but we can't find one nearby. Please send Jon this with your last 5 autosaves and what changelist # you're playing. Oh, and you're about to crash.");

		SetQuestData1(ePlayer, pPlot->getX());
		SetQuestData2(ePlayer, pPlot->getY());

		pPlot->setRevealed(GET_PLAYER(ePlayer).getTeam(), true);
		pPlot->setRevealedImprovementType(GET_PLAYER(ePlayer).getTeam(), pPlot->getImprovementType());

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_KILL_CAMP");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_KILL_CAMP");
		iNotificationX = pPlot->getX();
		iNotificationY = pPlot->getY();
	}
	// Connect a Resource
	else if (eQuest == MINOR_CIV_QUEST_CONNECT_RESOURCE)
	{
		ResourceTypes eResource = GetNearbyResourceForQuest(ePlayer);

		FAssertMsg(eResource != NO_RESOURCE, "MINOR CIV AI: For some reason we got NO_RESOURCE when starting a quest for a major to find a Resource. Please send Jon this with your last 5 autosaves and what changelist # you're playing. Bad things are probably going to happen.");

		SetQuestData1(ePlayer, eResource);

		const char* strResourceName = GC.getResourceInfo(eResource)->GetDescription();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_CONNECT_RESOURCE");
		strMessage << strResourceName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CONNECT_RESOURCE");
		strSummary << strResourceName;
	}
	// Construct a Wonder
	else if (eQuest == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
	{
		BuildingTypes eWonder = GetBestWonderForQuest(ePlayer);

		FAssertMsg(eWonder != NO_BUILDING, "MINOR CIV AI: For some reason we got NO_BUILDING when starting a quest for a major to find a Wonder. Please send Jon this with your last 5 autosaves and what changelist # you're playing. Bad things are probably going to happen.");

		SetQuestData1(ePlayer, eWonder);

		const char* strBuildingName = GC.getBuildingInfo(eWonder)->GetDescription();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_CONSTRUCT_WONDER");
		strMessage << strBuildingName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CONSTRUCT_WONDER");
		strSummary << strBuildingName;
	}
	// Great Person
	else if (eQuest == MINOR_CIV_QUEST_GREAT_PERSON)
	{
		UnitTypes eUnit = GetBestGreatPersonForQuest(ePlayer);

		FAssertMsg(eUnit != NO_UNIT, "MINOR CIV AI: For some reason we got NO_UNIT when starting a quest for a major to find a Great Person. Please send Jon this with your last 5 autosaves and what changelist # you're playing. Bad things are probably going to happen.");

		SetQuestData1(ePlayer, eUnit);
		SetQuestData2(ePlayer, 0);	// This will be used to keep track of whether or not the quest is completed yet

		const char* strUnitName = GC.getUnitInfo(eUnit)->GetDescription();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_GREAT_PERSON");
		strMessage << strUnitName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_GREAT_PERSON");
		strSummary << strUnitName;
	}
	// Kill another City State
	else if (eQuest == MINOR_CIV_QUEST_KILL_CITY_STATE)
	{
		PlayerTypes eTargetCityState = GetBestCityStateTarget();

		FAssertMsg(eTargetCityState != NO_PLAYER, "MINOR CIV AI: For some reason we got NO_PLAYER when starting a quest for a major to kill a City State. Please send Jon this with your last 5 autosaves and what changelist # you're playing. Bad things are probably going to happen.");

		SetQuestData1(ePlayer, eTargetCityState);

		const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_KILL_CITY_STATE");
		strMessage << strTargetNameKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_KILL_CITY_STATE");
		strSummary << strTargetNameKey;
	}
	// Find another player's territory
	else if (eQuest == MINOR_CIV_QUEST_FIND_PLAYER)
	{
		PlayerTypes ePlayerToFind = GetBestPlayerToFind(ePlayer);

		FAssertMsg(ePlayerToFind != NO_PLAYER, "MINOR CIV AI: For some reason we got NO_PLAYER when starting a quest for a major to find a player. Please send Jon this with your last 5 autosaves and what changelist # you're playing. Bad things are probably going to happen.");

		SetQuestData1(ePlayer, ePlayerToFind);

		const char* strCivKey = GET_PLAYER(ePlayerToFind).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_FIND_PLAYER");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_FIND_PLAYER");
		strSummary << strCivKey;
	}
	// Find a Natural Wonder
	else if (eQuest == MINOR_CIV_QUEST_FIND_NATURAL_WONDER)
	{
		int iNumWondersAlreadyFound = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetNumNaturalWondersDiscovered();
		SetQuestData1(ePlayer, iNumWondersAlreadyFound);

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_FIND_NATURAL_WONDER");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_FIND_NATURAL_WONDER");
	}

	strMessage << GetPlayer()->getNameKey();
	strSummary << GetPlayer()->getNameKey();
	AddQuestNotification(strMessage, strSummary, ePlayer, iNotificationX, iNotificationY);
}

/// Quest complete?
void CvMinorCivAI::DoTestQuestComplete(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	bool bQuestComplete = false;
	Localization::String strMessage;
	Localization::String strSummary;

	MinorCivQuestTypes eQuest = GetActiveQuestForPlayer(ePlayer);

	// BUILD A ROUTE
	if (eQuest == MINOR_CIV_QUEST_ROUTE)
	{
		if (m_pPlayer->IsCapitalConnectedToPlayer(ePlayer))
		{
			bQuestComplete = true;

			// Route exists!
			SetRouteConnectionEstablished(ePlayer, true);

			int iFriendshipChange = /*50*/ GC.getMINOR_QUEST_FRIENDSHIP_ROUTE();
			ChangeFriendshipWithMajor(ePlayer, iFriendshipChange, /*bFromQuest*/ true);

			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_ROUTE_CONNECTION");
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_ROUTE_CONNECTION");
		}
	}

	// KILL A CAMP
	else if (eQuest == MINOR_CIV_QUEST_KILL_CAMP)
	{
		int iX = GetQuestData1(ePlayer);
		int iY = GetQuestData2(ePlayer);
		CvPlot* pPlot = GC.getMap().plot(iX, iY);

		// No longer a camp here?
		if (pPlot->getImprovementType() != GC.getBARBARIAN_CAMP_IMPROVEMENT())
		{
			// Did this guy clear it?
			if (pPlot->GetPlayerThatClearedBarbCampHere() == ePlayer)
			{
				bQuestComplete = true;

				int iFriendshipChange = /*50*/ GC.getMINOR_QUEST_FRIENDSHIP_KILL_CAMP();
				ChangeFriendshipWithMajor(ePlayer, iFriendshipChange, /*bFromQuest*/ true);

				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_KILL_CAMP");
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_KILL_CAMP");
			}
		}
	}

	// CONNECT A RESOURCE
	else if (eQuest == MINOR_CIV_QUEST_CONNECT_RESOURCE)
	{
		ResourceTypes eResource = (ResourceTypes) GetQuestData1(ePlayer);

		// Player found has the Resource?
		if (GET_PLAYER(ePlayer).getNumResourceTotal(eResource, /*bIncludeImport*/ true) > 0)
		{
			bQuestComplete = true;

			int iFriendshipChange = /*40*/ GC.getMINOR_QUEST_FRIENDSHIP_CONNECT_RESOURCE();
			ChangeFriendshipWithMajor(ePlayer, iFriendshipChange, /*bFromQuest*/ true);

			const char* strResourceName = GC.getResourceInfo(eResource)->GetDescriptionKey();

			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONNECT_RESOURCE");
			strMessage << strResourceName;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_CONNECT_RESOURCE");
			strSummary << strResourceName;
		}
	}

	// CONSTRUCT A WONDER
	else if (eQuest == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
	{
		BuildingTypes eWonder = (BuildingTypes) GetQuestData1(ePlayer);

		// Player built the Wonder?
		if (GET_PLAYER(ePlayer).countNumBuildings(eWonder) > 0)
		{
			bQuestComplete = true;

			int iFriendshipChange = /*40*/ GC.getMINOR_QUEST_FRIENDSHIP_CONSTRUCT_WONDER();
			ChangeFriendshipWithMajor(ePlayer, iFriendshipChange, /*bFromQuest*/ true);

			const char* strBuildingName = GC.getBuildingInfo(eWonder)->GetDescription();

			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONSTRUCT_WONDER");
			strMessage << strBuildingName;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_CONSTRUCT_WONDER");
			strSummary << strBuildingName;
		}
	}

	// GREAT PERSON
	else if (eQuest == MINOR_CIV_QUEST_GREAT_PERSON)
	{
		UnitTypes eUnit = (UnitTypes) GetQuestData1(ePlayer);
		bool bComplete = (bool) GetQuestData2(ePlayer);	// This is set elsewhere

		// Player built the Wonder?
		if (bComplete)
		{
			bQuestComplete = true;

			int iFriendshipChange = /*40*/ GC.getMINOR_QUEST_FRIENDSHIP_GREAT_PERSON();
			ChangeFriendshipWithMajor(ePlayer, iFriendshipChange, /*bFromQuest*/ true);

			const char* strUnitName = GC.getUnitInfo(eUnit)->GetDescriptionKey();

			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_GREAT_PERSON");
			strMessage << strUnitName;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_GREAT_PERSON");
			strSummary << strUnitName;
		}
	}

	// KILL ANOTHER CITY STATE
	else if (eQuest == MINOR_CIV_QUEST_KILL_CITY_STATE)
	{
		PlayerTypes eTargetCityState = (PlayerTypes) GetQuestData1(ePlayer);

		// Player killed the City State?
		if (GET_TEAM(GET_PLAYER(eTargetCityState).getTeam()).GetKilledByTeam() == GET_PLAYER(ePlayer).getTeam())
		{
			bQuestComplete = true;

			int iFriendshipChange = /*80*/ GC.getMINOR_QUEST_FRIENDSHIP_KILL_CITY_STATE();
			ChangeFriendshipWithMajor(ePlayer, iFriendshipChange, /*bFromQuest*/ true);

			const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_KILL_CITY_STATE");
			strMessage << strTargetNameKey;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_KILL_CITY_STATE");
			strSummary << strTargetNameKey;
		}
	}

	// FIND ANOTHER PLAYER
	else if (eQuest == MINOR_CIV_QUEST_FIND_PLAYER)
	{
		PlayerTypes ePlayerToFind = (PlayerTypes) GetQuestData1(ePlayer);

		// Player found the target?
		if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).IsHasFoundPlayersTerritory(ePlayerToFind))
		{
			bQuestComplete = true;

			int iFriendshipChange = /*35*/ GC.getMINOR_QUEST_FRIENDSHIP_FIND_PLAYER();
			ChangeFriendshipWithMajor(ePlayer, iFriendshipChange, /*bFromQuest*/ true);

			const char* strCivKey = GET_PLAYER(ePlayerToFind).getCivilizationShortDescriptionKey();

			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_FIND_PLAYER");
			strMessage << strCivKey;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_FIND_PLAYER");
			strSummary << strCivKey;
		}
	}

	// FIND NATURAL WONDER
	else if (eQuest == MINOR_CIV_QUEST_FIND_NATURAL_WONDER)
	{
		int iNumWondersFoundBefore = GetQuestData1(ePlayer);

		// Player found a new NW?
		if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetNumNaturalWondersDiscovered() > iNumWondersFoundBefore)
		{
			bQuestComplete = true;

			int iFriendshipChange = /*40*/ GC.getMINOR_QUEST_FRIENDSHIP_FIND_NATURAL_WONDER();
			ChangeFriendshipWithMajor(ePlayer, iFriendshipChange, /*bFromQuest*/ true);

			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_FIND_NATURAL_WONDER");
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_FIND_NATURAL_WONDER");
		}
	}

	// Quest complete! Do all the maintenance stuff afterwards
	if (bQuestComplete)
	{
		DoSeedQuestCountdown(ePlayer);

		strMessage << GetPlayer()->getNameKey();
		strSummary << GetPlayer()->getNameKey();
		AddQuestNotification(strMessage, strSummary, ePlayer);

		EndActiveQuestForPlayer(ePlayer);
	}
}

/// No longer valid?
void CvMinorCivAI::DoTestQuestObsolete(PlayerTypes ePlayer)
{
	bool bQuestEnded = false;

	Localization::String strMessage;
	Localization::String strSummary;

	MinorCivQuestTypes eQuest = GetActiveQuestForPlayer(ePlayer);

	// City-state wanted us to clear a camp
	if (eQuest == MINOR_CIV_QUEST_KILL_CAMP)
	{
		int iX = GetQuestData1(ePlayer);
		int iY = GetQuestData2(ePlayer);
		CvPlot* pPlot = GC.getMap().plot(iX, iY);

		// Camp that was here is gone
		if (!pPlot->HasBarbarianCamp())
		{
			bQuestEnded = true;

			strMessage = Localization::Lookup("TXT_KEY_NTFN_QUEST_ENDED_KILL_CAMP");
			strSummary = Localization::Lookup("TXT_KEY_NTFN_QUEST_ENDED_KILL_CAMP_S");
		}
	}

	// CONSTRUCT A WONDER
	else if (eQuest == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
	{
		BuildingTypes eWonder = (BuildingTypes) GetQuestData1(ePlayer);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eWonder);
		if(pkBuildingInfo)
		{
			const char* strBuildingName = pkBuildingInfo->GetDescriptionKey();
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
			{
				const PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;

				// Someone else built the Wonder?
				if (ePlayer != eLoopPlayer && GET_PLAYER(eLoopPlayer).countNumBuildings(eWonder) > 0)
				{
					bQuestEnded = true;

					strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_CONSTRUCT_WONDER");
					strMessage << strBuildingName;
					strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_CONSTRUCT_WONDER");
					strSummary << strBuildingName;

					break;
				}
			}
		}
	}

	// KILL ANOTHER CITY STATE
	else if (eQuest == MINOR_CIV_QUEST_KILL_CITY_STATE)
	{
		PlayerTypes eTargetCityState = (PlayerTypes) GetQuestData1(ePlayer);

		// Someone else killed the City State?
		if (!GET_PLAYER(eTargetCityState).isAlive() && GET_TEAM(GET_PLAYER(eTargetCityState).getTeam()).GetKilledByTeam() != GET_PLAYER(ePlayer).getTeam())
		{
			bQuestEnded = true;

			const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_KILL_CITY_STATE");
			strMessage << strTargetNameKey;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_KILL_CITY_STATE");
			strSummary << strTargetNameKey;
		}
	}

	if (bQuestEnded)
	{
		DoSeedQuestCountdown(ePlayer);

		strMessage << GetPlayer()->getNameKey();
		strSummary << GetPlayer()->getNameKey();
		AddQuestNotification(strMessage, strSummary, ePlayer);

		EndActiveQuestForPlayer(ePlayer);
	}
}


/// Is eQuest valid for ePlayer?
bool CvMinorCivAI::IsValidQuestForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eQuest)
{
	CvCity* pMinorsCapital = GetPlayer()->getCapitalCity();
	CvCity* pMajorsCapital = GET_PLAYER(ePlayer).getCapitalCity();

	// Both players must have a capital
	if (pMinorsCapital == NULL || pMajorsCapital == NULL)
	{
		return false;
	}

	// BUILD A ROUTE
	if (eQuest == MINOR_CIV_QUEST_ROUTE)
	{
		if (QUEST_DEBUGGING)
			return false;

		if (GC.getQUEST_DISABLED_ROUTE() == 1)
			return false;

		// Don't ask for this if we've never been friends
		if (!IsEverFriends(ePlayer))
			return false;

		// Can't have completed this Quest already
		if (IsRouteConnectionEstablished(ePlayer))
			return false;

		// Must be in the same area
		if (pMinorsCapital->getArea() != pMajorsCapital->getArea())
			return false;
	}
	// KILL A CAMP
	else if (eQuest == MINOR_CIV_QUEST_KILL_CAMP)
	{
		if (QUEST_DEBUGGING)
			return false;

		if (GC.getQUEST_DISABLED_KILL_CAMP() == 1)
			return false;

		// Must be in the same area
		if (pMinorsCapital->getArea() != pMajorsCapital->getArea())
			return false;

		// Any nearby camps?
		if (GetBestNearbyCampToKill() == NULL)
			return false;
	}
	// CONNECT A RESOURCE
	else if (eQuest == MINOR_CIV_QUEST_CONNECT_RESOURCE)
	{
		if (QUEST_DEBUGGING)
			return false;

		if (GC.getQUEST_DISABLED_CONNECT_RESOURCE() == 1)
			return false;

		// Don't ask for this if we've never been friends
		if (!IsEverFriends(ePlayer))
			return false;

		ResourceTypes eResource = GetNearbyResourceForQuest(ePlayer);

		if (eResource == NO_RESOURCE)
			return false;
	}
	// CONSTRUCT A WONDER
	else if (eQuest == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
	{
		if (QUEST_DEBUGGING)
			return false;

		if (GC.getQUEST_DISABLED_CONSTRUCT_WONDER() == 1)
			return false;

		// Don't ask for this if we've never been friends
		if (!IsEverFriends(ePlayer))
			return false;

		BuildingTypes eWonder = GetBestWonderForQuest(ePlayer);

		if (eWonder == NO_BUILDING)
			return false;
	}
	// GREAT PERSON
	else if (eQuest == MINOR_CIV_QUEST_GREAT_PERSON)
	{
		if (QUEST_DEBUGGING)
			return false;

		if (GC.getQUEST_DISABLED_GREAT_PERSON() == 1)
			return false;

		// Don't ask for this if we've never been friends
		if (!IsEverFriends(ePlayer))
			return false;

		UnitTypes eUnit = GetBestGreatPersonForQuest(ePlayer);

		if (eUnit == NO_UNIT)
			return false;
	}
	// KILL ANOTHER CITY STATE
	else if (eQuest == MINOR_CIV_QUEST_KILL_CITY_STATE)
	{
		if (QUEST_DEBUGGING)
			return false;

		if (GC.getQUEST_DISABLED_KILL_CITY_STATE() == 1)
			return false;

		// Friendly City States don't give out this quest
		if (GetPersonality() == MINOR_CIV_PERSONALITY_FRIENDLY)
			return false;

		PlayerTypes eTargetCityState = GetBestCityStateTarget();

		if (eTargetCityState == NO_PLAYER)
			return false;
	}
	// FIND ANOTHER PLAYER
	else if (eQuest == MINOR_CIV_QUEST_FIND_PLAYER)
	{
		if (QUEST_DEBUGGING)
			return false;

		if (GC.getQUEST_DISABLED_FIND_PLAYER() == 1)
			return false;

		// Don't ask for this if we've never been friends
		if (!IsEverFriends(ePlayer))
			return false;

		PlayerTypes eTarget = GetBestPlayerToFind(ePlayer);

		if (eTarget == NO_PLAYER)
			return false;
	}
	// FIND NATURAL WONDER
	else if (eQuest == MINOR_CIV_QUEST_FIND_NATURAL_WONDER)
	{
		//if (QUEST_DEBUGGING)
		//	return false;

		if (GC.getQUEST_DISABLED_NATURAL_WONDER() == 1)
			return false;

		// Don't ask for this if we've never been friends
		if (!IsEverFriends(ePlayer))
			return false;

		if (!IsGoodTimeForNaturalWonderQuest(ePlayer))
			return false;
	}

	return true;
}

/// What Quest is active for ePlayer?
MinorCivQuestTypes CvMinorCivAI::GetActiveQuestForPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return NO_MINOR_CIV_QUEST_TYPE;

	return (MinorCivQuestTypes) m_aiPlayerQuests[ePlayer];
}

/// Sets what Quest is active for ePlayer
void CvMinorCivAI::SetActiveQuestForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eQuest)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	CvAssertMsg(eQuest >= NO_MINOR_CIV_QUEST_TYPE, "eQuest is expected to be non-negative (invalid Index)");
	CvAssertMsg(eQuest < NUM_MINOR_CIV_QUEST_TYPES, "eQuest is expected to be within maximum bounds (invalid Index)");
	if(eQuest < NO_MINOR_CIV_QUEST_TYPE || eQuest >= NUM_MINOR_CIV_QUEST_TYPES) return;

	m_aiPlayerQuests[ePlayer] = eQuest;
}

/// Get rid of the current quest
void CvMinorCivAI::EndActiveQuestForPlayer(PlayerTypes ePlayer)
{
	SetActiveQuestForPlayer(ePlayer, NO_MINOR_CIV_QUEST_TYPE);
	SetQuestData1(ePlayer, -1);
	SetQuestData2(ePlayer, -1);
}

/// How more or less likely is a player to offer a particular quest type based on his personality?
int CvMinorCivAI::GetPersonalityQuestBias(MinorCivQuestTypes eQuest)
{
	// Multiply by 10 to give us some more fidelity. We'll remove it at the bottom
	int iCount = 10 * /*10*/ GC.getMINOR_CIV_QUEST_WEIGHT_DEFAULT();

	MinorCivPersonalityTypes ePersonality = GetPersonality() ;
	MinorCivTraitTypes eTrait = GetTrait();

	// ROUTE
	if (eQuest == MINOR_CIV_QUEST_ROUTE)
	{
		if (ePersonality == MINOR_CIV_PERSONALITY_FRIENDLY)		// Friendly
		{
			iCount *= /*200*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_ROUTE();
			iCount /= 100;
		}
		if (ePersonality == MINOR_CIV_PERSONALITY_HOSTILE)		// Hostile
		{
			iCount *= /*20*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_ROUTE();
			iCount /= 100;
		}
		if (eTrait == MINOR_CIV_TRAIT_MARITIME)						// Maritime
		{
			iCount *= /*150*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_ROUTE();
			iCount /= 100;
		}
	}

	// KILL A CAMP
	else if (eQuest == MINOR_CIV_QUEST_KILL_CAMP)
	{
		if (eTrait == MINOR_CIV_TRAIT_MILITARISTIC)					// Militaristic
		{
			iCount *= /*300*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_KILL_CAMP();
			iCount /= 100;
		}
	}

	// CONNECT A RESOURCE
	else if (eQuest == MINOR_CIV_QUEST_CONNECT_RESOURCE)
	{
		if (eTrait == MINOR_CIV_TRAIT_MARITIME)						// Maritime
		{
			iCount *= /*300*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_CONNECT_RESOURCE();
			iCount /= 100;
		}
	}

	// CONSTRUCT A WONDER
	else if (eQuest == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
	{
		if (eTrait == MINOR_CIV_TRAIT_CULTURED)						// Cultured
		{
			iCount *= /*300*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_CONSTRUCT_WONDER();
			iCount /= 100;
		}
	}

	// GREAT PERSON
	else if (eQuest == MINOR_CIV_QUEST_GREAT_PERSON)
	{
		if (eTrait == MINOR_CIV_TRAIT_CULTURED)						// Cultured
		{
			iCount *= /*300*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_GREAT_PERSON();
			iCount /= 100;
		}
	}

	// KILL ANOTHER CITY STATE
	else if (eQuest == MINOR_CIV_QUEST_KILL_CITY_STATE)
	{
		if (eTrait == MINOR_CIV_TRAIT_MILITARISTIC)					// Militaristic
		{
			iCount *= /*200*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_KILL_CITY_STATE();
			iCount /= 100;
		}
		if (ePersonality == MINOR_CIV_PERSONALITY_HOSTILE)		// Hostile
		{
			iCount *= /*200*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_KILL_CITY_STATE();
			iCount /= 100;
		}
		if (ePersonality == MINOR_CIV_PERSONALITY_NEUTRAL)		// Neutral
		{
			iCount *= /*40*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_NEUTRAL_KILL_CITY_STATE();
			iCount /= 100;
		}
	}

	// FIND ANOTHER PLAYER
	else if (eQuest == MINOR_CIV_QUEST_FIND_PLAYER)
	{
		if (eTrait == MINOR_CIV_TRAIT_MARITIME)						// Maritime
		{
			iCount *= /*300*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_FIND_PLAYER();
			iCount /= 100;
		}
	}

	// FIND NATURAL WONDER
	else if (eQuest == MINOR_CIV_QUEST_FIND_NATURAL_WONDER)
	{
		if (eTrait == MINOR_CIV_TRAIT_MILITARISTIC)					// Militaristic
		{
			iCount *= /*50*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_FIND_NATURAL_WONDER();
			iCount /= 100;
		}
		if (ePersonality == MINOR_CIV_PERSONALITY_HOSTILE)		// Hostile
		{
			iCount *= /*30*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_FIND_NATURAL_WONDER();
			iCount /= 100;
		}
	}

	return iCount / 10;
}

/// How long before we trigger a new quest?
void CvMinorCivAI::DoSeedQuestCountdown(PlayerTypes ePlayer)
{
	int iNumTurns = /*10*/ GC.getMINOR_CIV_QUEST_MIN_TURNS_BETWEEN();

	int iRand = /*25*/ GC.getMINOR_CIV_QUEST_RAND_TURNS_BETWEEN();

	// Hostile civs don't hand out quests as often
	if (GetPersonality() == MINOR_CIV_PERSONALITY_HOSTILE)
	{
		iRand *= /*200*/ GC.getMINOR_CIV_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER();
		iRand /= 100;
	}

	iNumTurns += GC.getGame().getJonRandNum(iRand, "Random # of turns for Minor Civ quest counter.");

	// If the Quests JUST got unlocked, apply another rand to it
	if (GC.getGame().getElapsedGameTurns() == GetFirstPossibleTurnForQuests())
	{
		iRand = /*20*/ GC.getMINOR_CIV_QUEST_FIRST_POSSIBLE_TURN_RAND();
		iNumTurns += GC.getGame().getJonRandNum(iRand, "Random # of turns for Minor Civ quest counter - first time.");
	}

	// Modify for Game Speed
	iNumTurns *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iNumTurns /= 100;

	SetQuestCountdown(ePlayer, iNumTurns);
}

/// How many turns since the last Quest ended?
int CvMinorCivAI::GetQuestCountdown(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1; // as set during Reset()

	return m_aiQuestCountdown[ePlayer];
}

/// Sets How many turns since the last Quest ended
void CvMinorCivAI::SetQuestCountdown(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_aiQuestCountdown[ePlayer] = iValue;
}

/// Changes many turns since the last Quest ended
void CvMinorCivAI::ChangeQuestCountdown(PlayerTypes ePlayer, int iChange)
{
	SetQuestCountdown(ePlayer, GetQuestCountdown(ePlayer) + iChange);
}

/// What is Data member 1
int CvMinorCivAI::GetQuestData1(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1; // as set during Reset()

	return m_aiQuestData1[ePlayer];
}

/// Sets What is Data member 1 is
void CvMinorCivAI::SetQuestData1(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_aiQuestData1[ePlayer] = iValue;
}

/// What is Data member 2
int CvMinorCivAI::GetQuestData2(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1; // as set during Reset()

	return m_aiQuestData2[ePlayer];
}

/// Sets What is Data member 2 is
void CvMinorCivAI::SetQuestData2(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_aiQuestData2[ePlayer] = iValue;
}

/// Has a Route has been established?
bool CvMinorCivAI::IsRouteConnectionEstablished(PlayerTypes eMajor) const
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;

	return m_abRouteConnectionEstablished[eMajor];
}

/// Sets a Route to be established
void CvMinorCivAI::SetRouteConnectionEstablished(PlayerTypes eMajor, bool bValue)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;

	if (m_abRouteConnectionEstablished[eMajor] != bValue)
	{
		m_abRouteConnectionEstablished[eMajor] = bValue;
	}
}

/// Any Camps near us?
CvPlot* CvMinorCivAI::GetBestNearbyCampToKill()
{
	CvCity* pCapital = GetPlayer()->getCapitalCity();

	// Minor must have Capital
	if (pCapital == NULL)
	{
		return NULL;
	}

	FStaticVector<int, 32, true, c_eCiv5GameplayDLL, 0>	viPlotIndexes; // 32 camps in 8 hex radius should be enough

	int iRange = /*8*/ GC.getMINOR_CIV_QUEST_KILL_CAMP_RANGE();

	CvPlot* pLoopPlot;

	// Loop in all plots in range
	int iDX, iDY;
	for (iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for (iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pLoopPlot = plotXY(pCapital->getX(), pCapital->getY(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				// Camp must be in the same Area as us
				if (pLoopPlot->getArea() != pCapital->getArea())
				{
					continue;
				}

				if (plotDistance(pCapital->getX(), pCapital->getY(), pLoopPlot->getX(), pLoopPlot->getY()) <= iRange)
				{
					// Can't be owned by anyone
					if (pLoopPlot->getOwner() == NO_PLAYER)
					{
						// Camp here?
						if (pLoopPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
						{
							viPlotIndexes.push_back(pLoopPlot->GetPlotIndex());
						}
					}
				}
			}
		}
	}

	// Didn't find any nearby
	if (viPlotIndexes.size() == 0)
	{
		return NULL;
	}

	int iRandIndex = GC.getGame().getJonRandNum(viPlotIndexes.size(), "Finding random Barb Camp for Minor to give out a quest to kill.");
	CvPlot* pBestPlot = GC.getMap().plotByIndex(viPlotIndexes[iRandIndex]);

	return pBestPlot;
}

/// Find a Resource that a Minor would want a major to connect
ResourceTypes CvMinorCivAI::GetNearbyResourceForQuest(PlayerTypes ePlayer)
{
	ResourceTypes eBestResource = NO_RESOURCE;

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	if (GET_PLAYER(ePlayer).getStartingPlot() != NULL)
	{
		CvArea* pPlayerArea = GC.getMap().getArea(GET_PLAYER(ePlayer).getStartingPlot()->getArea());

		FStaticVector<ResourceTypes, 64, true, c_eCiv5GameplayDLL, 0> veValidResources; // 64 resources should be way more than enough
		TechTypes eRevealTech;
		TechTypes eConnectTech;

		// Loop through all Resources and see if they're useful
		ResourceTypes eResource;
		for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			eResource = (ResourceTypes) iResourceLoop;

			const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			// Must not be a plain ol' bonus resource
			if (pkResourceInfo == NULL || GC.getResourceInfo(eResource)->getResourceUsage() == RESOURCEUSAGE_BONUS)
			{
				continue;
			}

			// Minor can't already have this Resource
			if (GetPlayer()->getNumResourceTotal(eResource, /*bIncludeImport*/ true) > 0)
			{
				continue;
			}

			// Player can't already have this Resource
			if (GET_PLAYER(ePlayer).getNumResourceTotal(eResource, /*bIncludeImport*/ true) > 0)
			{
				continue;
			}

			// Player has to be able to see it
			eRevealTech = (TechTypes) GC.getResourceInfo(eResource)->getTechReveal();
			if (!GET_TEAM(eTeam).GetTeamTechs()->HasTech(eRevealTech))
			{
				continue;
			}

			// Player has to be able to use it
			eConnectTech = (TechTypes) GC.getResourceInfo(eResource)->getTechCityTrade();
			if (!GET_TEAM(eTeam).GetTeamTechs()->HasTech(eConnectTech))
			{
				continue;
			}

			// Must be this Resource on the player's area
			if (pPlayerArea->getNumResources(eResource) == 0)
			{
				continue;
			}

			veValidResources.push_back(eResource);
		}

		// Didn't find any valid Resources
		if (veValidResources.size() == 0)
		{
			return NO_RESOURCE;
		}

		int iRandIndex = GC.getGame().getJonRandNum(veValidResources.size(), "Finding random Resource for Minor to give out a quest to connect.");
		eBestResource = veValidResources[iRandIndex];
	}

	return eBestResource;
}

/// Best wonder for a Quest given to ePlayer?
BuildingTypes CvMinorCivAI::GetBestWonderForQuest(PlayerTypes ePlayer)
{
	BuildingTypes eBestWonder;

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	FStaticVector<BuildingTypes, 50, true, c_eCiv5GameplayDLL, 0> veValidBuildings; // 50 wonders should be overkill

	TechTypes ePrereqTech;

	int iWorldPlayerLoop;
	PlayerTypes eWorldPlayer;
	CvCity* pLoopCity;
	int iCityLoop;
	int iWonderProgress;
	int iCompletionThreshold = /*25*/ GC.getMINOR_CIV_QUEST_WONDER_COMPLETION_THRESHOLD();
	bool bFoundWonderTooFarAlong;

	// Loop through all Buildings and see if they're useful
	for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

		//Skip if NULL
		if(pkBuildingInfo == NULL)
			continue;

		bFoundWonderTooFarAlong = false;

		const BuildingClassTypes eBuildingClass = (BuildingClassTypes)pkBuildingInfo->GetBuildingClassType();

		// Make sure it is available in a late game start
		if (pkBuildingInfo->GetMaxStartEra() != NO_ERA)
		{
			if (GC.getGame().getStartEra() > pkBuildingInfo->GetMaxStartEra())
			{
				continue;
			}
		}

		// Must be a wonder
		if (!isWorldWonderClass(pkBuildingInfo->GetBuildingClassInfo()))
		{
			continue;
		}

		// Player needs prereq tech
		ePrereqTech = (TechTypes) pkBuildingInfo->GetPrereqAndTech();
		if (!GET_TEAM(eTeam).GetTeamTechs()->HasTech(ePrereqTech))
		{
			continue;
		}

		// Can't already have been built
		if (GC.getGame().isBuildingClassMaxedOut(eBuildingClass))
		{
			continue;
		}

		// Someone CAN be building this wonder right now, but they can't be more than a certain % of the way done (25% by default)
		for (iWorldPlayerLoop = 0; iWorldPlayerLoop < MAX_MAJOR_CIVS; iWorldPlayerLoop++)
		{
			eWorldPlayer = (PlayerTypes) iWorldPlayerLoop;

			for (pLoopCity = GET_PLAYER(eWorldPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eWorldPlayer).nextCity(&iCityLoop))
			{
				iWonderProgress = pLoopCity->GetCityBuildings()->GetBuildingProduction(eBuilding);

				if (iWonderProgress * 100 / pLoopCity->getProductionNeeded(eBuilding) >= iCompletionThreshold)
				{
					bFoundWonderTooFarAlong = true;
					break;
				}
			}
			if (bFoundWonderTooFarAlong)
			{
				break;
			}
		}
		if (bFoundWonderTooFarAlong)
		{
			continue;
		}

		veValidBuildings.push_back(eBuilding);
	}

	// Didn't find any valid Wonders
	if (veValidBuildings.size() == 0)
	{
		return NO_BUILDING;
	}

	int iRandIndex = GC.getGame().getJonRandNum(veValidBuildings.size(), "Finding random Wonder for Minor to give out a quest to construct.");
	eBestWonder = veValidBuildings[iRandIndex];

	return eBestWonder;
}


/// Find a Great Person that a Minor would want a major to spawn
UnitTypes CvMinorCivAI::GetBestGreatPersonForQuest(PlayerTypes ePlayer)
{
	UnitTypes eBestUnit = NO_UNIT;

	SpecialUnitTypes eSpecialUnitGreatPerson = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_PEOPLE");

	FStaticVector<UnitTypes, 8, true, c_eCiv5GameplayDLL, 0> veValidUnits;

	// Loop through all Units and see if they're useful
	for (int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
	{
		const UnitTypes eUnit = static_cast<UnitTypes>(iUnitLoop);
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);

		if(pkUnitInfo == NULL)
			continue;

		// Can't be able to train it
		if (pkUnitInfo->GetProductionCost() != -1)
		{
			continue;
		}

		// Must be a Great Person (defined in SpecialUnitType in Unit XML)
		if (pkUnitInfo->GetSpecialUnitType() != eSpecialUnitGreatPerson)
		{
			continue;
		}

		// Must be a Great Person for this player's civ
		if (!GET_PLAYER(ePlayer).canTrain(eUnit, false /*bContinue*/, false /*bTestVisible*/, true /*bIgnoreCost*/))
		{
			continue;
		}

		bool bAlreadyHasUnit = false;

		// Player can't already have this Unit
		int iLoop;
		for (CvUnit* pLoopUnit = GET_PLAYER(ePlayer).firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = GET_PLAYER(ePlayer).nextUnit(&iLoop))
		{
			if (pLoopUnit->getUnitType() == eUnit)
			{
				bAlreadyHasUnit = true;
				break;
			}
		}
		if (bAlreadyHasUnit)
		{
			continue;
		}

		veValidUnits.push_back(eUnit);
	}

	// Didn't find any valid Units
	if (veValidUnits.size() == 0)
	{
		return NO_UNIT;
	}

	int iRandIndex = GC.getGame().getJonRandNum(veValidUnits.size(), "Finding random Great Person for Minor to give out a quest to spawn.");
	eBestUnit = veValidUnits[iRandIndex];

	return eBestUnit;
}

/// Anyone that this City State would want to kill?
PlayerTypes CvMinorCivAI::GetBestCityStateTarget()
{
	PlayerTypes eBestCityStateTarget = NO_PLAYER;

	PlayerProximityTypes eClosestProximity = PLAYER_PROXIMITY_DISTANT;

	// First, loop through the Minors in the game to what the closest proximity is to any of the players
	int iTargetLoop;
	PlayerTypes eTarget;
	for (iTargetLoop = MAX_MAJOR_CIVS; iTargetLoop < MAX_CIV_PLAYERS; iTargetLoop++)
	{
		eTarget = (PlayerTypes) iTargetLoop;

		if (!GET_PLAYER(eTarget).isAlive())
			continue;

		if (GetPlayer()->getTeam() == GET_PLAYER(eTarget).getTeam())
			continue;

		if (GetPlayer()->GetProximityToPlayer(eTarget) > eClosestProximity)
		{
			eClosestProximity = GetPlayer()->GetProximityToPlayer(eTarget);
		}
	}

	// Found nobody, or only people far away
	if (eClosestProximity == PLAYER_PROXIMITY_DISTANT)
	{
		return NO_PLAYER;
	}

	FStaticVector<PlayerTypes, MAX_CIV_PLAYERS, true, c_eCiv5GameplayDLL, 0> veValidTargets;

	// Now loop through and come up with a list of valid players based on the proximity we found out earlier
	for (iTargetLoop = MAX_MAJOR_CIVS; iTargetLoop < MAX_CIV_PLAYERS; iTargetLoop++)
	{
		eTarget = (PlayerTypes) iTargetLoop;

		if (!GET_PLAYER(eTarget).isAlive())
			continue;

		if (GetPlayer()->getTeam() == GET_PLAYER(eTarget).getTeam())
			continue;

		if (GetPlayer()->GetProximityToPlayer(eTarget) == eClosestProximity)
		{
			veValidTargets.push_back(eTarget);
		}
	}

	// Didn't find any valid Target players
	if (veValidTargets.size() == 0)
		return NO_PLAYER;

	int iRandIndex = GC.getGame().getJonRandNum(veValidTargets.size(), "Finding random City State Target for Minor to give out a quest to kill.");
	eBestCityStateTarget = veValidTargets[iRandIndex];

	return eBestCityStateTarget;
}

/// Has this minor asked any other player to go after eMinor?
bool CvMinorCivAI::IsWantsMinorDead(PlayerTypes eMinor)
{
	PlayerTypes eMajor;
	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;

		// Major must be alive
		if (!GET_PLAYER(eMajor).isAlive())
			continue;

		// Right quest?
		if (GetActiveQuestForPlayer(eMajor) != MINOR_CIV_QUEST_KILL_CITY_STATE)
			continue;

		// Right minor?
		if (GetQuestData1(eMajor) != eMinor)
			continue;

		return true;
	}

	return false;
}

/// Any good players to ask ePlayer to find?
PlayerTypes CvMinorCivAI::GetBestPlayerToFind(PlayerTypes ePlayer)
{
	PlayerTypes eBestTargetPlayer = NO_PLAYER;

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
	CvTeam* pTeam = &GET_TEAM(eTeam);

	FStaticVector<PlayerTypes, MAX_CIV_PLAYERS, true, c_eCiv5GameplayDLL, 0> veValidTargets;

	// First, loop through majors and see if the player has met them
	PlayerTypes eTargetMajor;
	TeamTypes eTargetTeam;
	for (int iTargetMajorLoop = 0; iTargetMajorLoop < MAX_MAJOR_CIVS; iTargetMajorLoop++)
	{
		eTargetMajor = (PlayerTypes) iTargetMajorLoop;
		eTargetTeam = GET_PLAYER(eTargetMajor).getTeam();

		// Target needs to be alive
		if (!GET_PLAYER(eTargetMajor).isAlive())
		{
			continue;
		}

		// Don't have the player find anyone on his team...
		if (eTargetTeam == eTeam)
		{
			continue;
		}

		// Player must already know the target
		if (!pTeam->isHasMet(eTargetTeam))
		{
			continue;
		}

		// Player can't have seen this guy's territory before
		if (pTeam->IsHasFoundPlayersTerritory(eTargetMajor))
		{
			continue;
		}

		veValidTargets.push_back(eTargetMajor);
	}

	// Didn't find any valid Target players
	if (veValidTargets.size() == 0)
	{
		return NO_PLAYER;
	}

	int iRandIndex = GC.getGame().getJonRandNum(veValidTargets.size(), "Finding random Major player Target for Minor to give out a quest to find.");
	eBestTargetPlayer = veValidTargets[iRandIndex];

	return eBestTargetPlayer;
}

/// Natural Wonder available to find that's not TOO easy to find?
bool CvMinorCivAI::IsGoodTimeForNaturalWonderQuest(PlayerTypes ePlayer)
{
	CvPlayer* pPlayer = &GET_PLAYER(ePlayer);
	CvTeam* pTeam = &GET_TEAM(pPlayer->getTeam());

	// No starting plot?
	if (pPlayer->getStartingPlot() == NULL)
	{
		return false;
	}

	// Player's already found them all
	if (pTeam->GetNumNaturalWondersDiscovered() == GC.getMap().GetNumNaturalWonders())
	{
		return false;
	}

	// Player hasn't yet found all the NWs in his area
	int iNumNaturalWondersInStartingArea = GC.getMap().getArea(pPlayer->getStartingPlot()->getArea())->GetNumNaturalWonders();
	if (pPlayer->GetNumNaturalWondersDiscoveredInArea() < iNumNaturalWondersInStartingArea)
	{
		return false;
	}

	return true;
}



// ******************************
// ***** Friendship *****
// ******************************



/// Per-turn friendship stuff
void CvMinorCivAI::DoFriendship()
{
	int iFriendship;

	int iChangeThisTurn;

	//int iFriendshipDecay;

	int iMinimumFriendship;

	Localization::String strMessage;
	Localization::String strSummary;
	const char* strMinorsNameKey = GetPlayer()->getNameKey();

	PlayerTypes ePlayer;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		// Normal relations - not at war
		if (!IsAtWarWithPlayersTeam(ePlayer))
		{
			iFriendship = GetFriendshipWithMajor(ePlayer);

			iChangeThisTurn = GetFriendshipChangePerTurnTimes100(ePlayer);

			if (iFriendship != 0)
			{
				// Positive friendship decays over time
				if (iFriendship > 0)
				{
					// If we haven't met the player yet, then don't reduce Friendship. There can be cases where this is set BEFORE players meet, and we want to keep that value as it is
					if (IsHasMetPlayer(ePlayer))
					{
						ChangeFriendshipWithMajorTimes100(ePlayer, iChangeThisTurn);

						// Are we about to lose any status?
						if (IsAllies(ePlayer))
						{
							if (GetFriendshipWithMajorTimes100(ePlayer) + iChangeThisTurn < GetAlliesThreshold() * 100)
							{
								strMessage = Localization::Lookup("TXT_KEY_NTFN_CITY_STATE_ALMOST_NOT_ALLIES");
								strMessage << strMinorsNameKey;
								strSummary = Localization::Lookup("TXT_KEY_NTFN_CITY_STATE_ALMOST_SM");
								strSummary << strMinorsNameKey;

								AddNotification(strMessage, strSummary, ePlayer);
							}
							if(!GC.getGame().isGameMultiPlayer() && GET_PLAYER(ePlayer).isHuman())
							{
								gDLL->UnlockAchievement( ACHIEVEMENT_CITYSTATE_ALLY );
							}

						}
						else if (IsFriends(ePlayer))
						{
							if (GetFriendshipWithMajorTimes100(ePlayer) + iChangeThisTurn < GetFriendsThreshold() * 100)
							{
								strMessage = Localization::Lookup("TXT_KEY_NTFN_CITY_STATE_ALMOST_NOT_FRIENDS");
								strMessage << strMinorsNameKey;
								strSummary = Localization::Lookup("TXT_KEY_NTFN_CITY_STATE_ALMOST_SM");
								strSummary << strMinorsNameKey;

								AddNotification(strMessage, strSummary, ePlayer);
							}
						}

						// If it dropped negative set it back up to 0
						if (GetFriendshipWithMajor(ePlayer) < 0)
							SetFriendshipWithMajor(ePlayer, 0);
					}
				}
				// Negative Friendship reverses over time
				else
				{
					ChangeFriendshipWithMajorTimes100(ePlayer, iChangeThisTurn);
				}
			}

			// Is there a minimum Friendship (Policies, etc.)
			iMinimumFriendship = GET_PLAYER(ePlayer).GetMinorFriendshipMinimum();
			if (iMinimumFriendship != 0)
			{
				if (GetFriendshipWithMajor(ePlayer) < iMinimumFriendship)
				{
					SetFriendshipWithMajor(ePlayer, iMinimumFriendship);
				}
			}
		}
		// At War!
		else
		{
		}
	}
}

/// How much does friendship drop per turn with ePlayer
int CvMinorCivAI::GetFriendshipChangePerTurnTimes100(PlayerTypes ePlayer) const
{
	CvPlayer &kPlayer = GET_PLAYER(ePlayer);

	int iChangeThisTurn;

	int iTraitDecayMod = kPlayer.GetPlayerTraits()->GetCityStateFriendshipModifier();

	// Positive friendship decays over time
	if (GetFriendshipWithMajor(ePlayer) > 0)
	{
		// Hostile Minors have Friendship decay quicker
		if (GetPersonality() == MINOR_CIV_PERSONALITY_HOSTILE)
			iChangeThisTurn = /*-150*/ GC.getMINOR_FRIENDSHIP_DROP_PER_TURN_HOSTILE();
		// Aggressor!
		else if (GET_TEAM(kPlayer.getTeam()).IsMinorCivAggressor())
			iChangeThisTurn = /*-200*/ GC.getMINOR_FRIENDSHIP_DROP_PER_TURN_AGGRESSOR();
		// Normal decay
		else
			iChangeThisTurn = /*-100*/ GC.getMINOR_FRIENDSHIP_DROP_PER_TURN();

		// Decay modified (Policies, etc.) Reset to 100 if it's 0
		int iDecayMod = 100 + GET_PLAYER(ePlayer).GetMinorFriendshipDecayMod();

		iChangeThisTurn *= iDecayMod;
		iChangeThisTurn /= 100;

		// Decay may be reduced by traits
		if (iTraitDecayMod > 0)
		{
			iChangeThisTurn *= 100;
			iChangeThisTurn /= (100 + iTraitDecayMod);
		}
	}

	// Negative friendship
	else
	{
		iChangeThisTurn = /*100*/ GC.getMINOR_FRIENDSHIP_NEGATIVE_INCREASE_PER_TURN();

		// Recovery boosted by traits
		if (iTraitDecayMod >  0)
		{
			iChangeThisTurn *= (100 + iTraitDecayMod);
			iChangeThisTurn /= 100;
		}
	}

	// Game Speed Mod
	iChangeThisTurn *= GC.getGame().getGameSpeedInfo().getGoldGiftMod();
	iChangeThisTurn /= 100;

	return iChangeThisTurn;
}

/// What is the level of Friendship between this Minor and the requested Major Civ?
int CvMinorCivAI::GetFriendshipWithMajorTimes100(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0; // as defined during Reset()

	return m_aiFriendshipWithMajorTimes100[ePlayer];
}

/// Sets the level of Friendship between this Minor and the specified Major Civ
void CvMinorCivAI::SetFriendshipWithMajorTimes100(PlayerTypes ePlayer, int iNum)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	int iOldFriendship = m_aiFriendshipWithMajorTimes100[ePlayer];

	m_aiFriendshipWithMajorTimes100[ePlayer] = iNum;

	int iMinimumFriendship = /*-60*/ GC.getMINOR_FRIENDSHIP_AT_WAR();
	if (GetFriendshipWithMajor(ePlayer) < iMinimumFriendship)
		m_aiFriendshipWithMajorTimes100[ePlayer] = iMinimumFriendship * 100;

	int iNewFriendship = m_aiFriendshipWithMajorTimes100[ePlayer];

	DoFriendshipChangeEffects(ePlayer, iOldFriendship/100, iNewFriendship/100);

	// Mark that we've once been friends
	if (IsFriends(ePlayer))
		SetEverFriends(ePlayer, true);

	// JON: Disable this, it's not working 100% of the time... if you go decay below friends level then this will force players to declare war to enter a formerly-friendly City State's territory
	// If a Human is doing something to be friendly with a Minor, change his intrusion state
	//if (GET_PLAYER(ePlayer).isHuman())
	//{
	//	if (iNewFriendship > 0 && iNewFriendship > iOldFriendship)
	//	{
	//		SetMajorIntruding(ePlayer, false);
	//	}
	//	// Turn intruding back on if we're down to 0 Friendship
	//	else if (iNewFriendship == 0)
	//	{
	//		SetMajorIntruding(ePlayer, true);
	//	}
	//}

	// Update City banners if this is the active player
	if (ePlayer == GC.getGame().getActivePlayer())
	{
		GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
	}
}

/// Changes the level of Friendship between this Minor and the specified Major Civ
void CvMinorCivAI::ChangeFriendshipWithMajorTimes100(PlayerTypes ePlayer, int iChange, bool bFromQuest)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		// If this friendship was earned from a Quest, then we might apply a modifier to it
		if (bFromQuest && iChange > 0)
		{
			if (GET_PLAYER(ePlayer).getMinorQuestFriendshipMod() != 0)
			{
				iChange *= (100 + GET_PLAYER(ePlayer).getMinorQuestFriendshipMod());
				iChange /= 100;
			}
		}

		SetFriendshipWithMajorTimes100(ePlayer, GetFriendshipWithMajorTimes100(ePlayer) + iChange);
	}
}

/// What is the level of Friendship between this Minor and the requested Major Civ?
int CvMinorCivAI::GetFriendshipWithMajor(PlayerTypes ePlayer) const
{
	return GetFriendshipWithMajorTimes100(ePlayer) / 100;
}

/// Sets the level of Friendship between this Minor and the specified Major Civ
void CvMinorCivAI::SetFriendshipWithMajor(PlayerTypes ePlayer, int iNum)
{
	SetFriendshipWithMajorTimes100(ePlayer, iNum * 100);
}

/// Changes the level of Friendship between this Minor and the specified Major Civ
void CvMinorCivAI::ChangeFriendshipWithMajor(PlayerTypes ePlayer, int iChange, bool bFromQuest)
{
	ChangeFriendshipWithMajorTimes100(ePlayer, iChange * 100, bFromQuest);
}

/// How many turns left does this player have of anger-free intrusion?
int CvMinorCivAI::GetAngerFreeIntrusionCounter(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0; // as defined during Reset()

	return m_aiAngerFreeIntrusionCounter[ePlayer];
}

/// How many turns left does this player have of anger-free intrusion?
void CvMinorCivAI::SetAngerFreeIntrusionCounter(PlayerTypes ePlayer, int iNum)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_aiAngerFreeIntrusionCounter[ePlayer] = iNum;
}

/// How many turns left does this player have of anger-free intrusion?
void CvMinorCivAI::ChangeAngerFreeIntrusionCounter(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	SetAngerFreeIntrusionCounter(ePlayer, GetAngerFreeIntrusionCounter(ePlayer) + iChange);
}

/// Update Best Relations Resource Bonus
void CvMinorCivAI::DoUpdateAlliesResourceBonus(PlayerTypes eNewAlly, PlayerTypes eOldAlly)
{
	FAssertMsg(eNewAlly != NO_PLAYER || eOldAlly != NO_PLAYER, "MINOR CIV AI: Updating Allied resource bonus and both players are not defined!");

	// Change gifted Resources
	ResourceTypes eResource;
	ResourceUsageTypes eUsage;
	int iResourceQuantity;
	for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		eResource = (ResourceTypes) iResourceLoop;

		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		if (pkResourceInfo == NULL)
			continue;

		eUsage = pkResourceInfo->getResourceUsage();

		if (eUsage == RESOURCEUSAGE_STRATEGIC || eUsage == RESOURCEUSAGE_LUXURY)
		{
			// Someone is losing the bonus :(
			if (eOldAlly != NO_PLAYER)
			{
				iResourceQuantity = GetPlayer()->getResourceExport(eResource);

				if (iResourceQuantity > 0)
				{
					GET_PLAYER(eOldAlly).changeResourceFromMinors(eResource, -iResourceQuantity);
					GetPlayer()->changeResourceExport(eResource, -iResourceQuantity);
				}
			}

			// Someone new is getting the bonus :D
			if (eNewAlly != NO_PLAYER)
			{
				iResourceQuantity = GetPlayer()->getNumResourceTotal(eResource);

				if (iResourceQuantity > 0)
				{
					GET_PLAYER(eNewAlly).changeResourceFromMinors(eResource, iResourceQuantity);
					GetPlayer()->changeResourceExport(eResource, iResourceQuantity);
				}
			}
		}
	}
}

/// The most Friendship any player has with this Minor
int CvMinorCivAI::GetMostFriendshipWithAnyMajor(PlayerTypes& eBestPlayer)
{
	int iMostFriendship = 0;
	PlayerTypes eMajor;

	int iFriendship;

	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;

		if (IsHasMetPlayer(eMajor))
		{
			iFriendship = GetFriendshipWithMajor(eMajor);

			if (iFriendship > iMostFriendship)
			{
				eBestPlayer = eMajor;
				iMostFriendship = iFriendship;
			}
		}
	}

	return iMostFriendship;
}

/// Who has the best relations with us right now?
PlayerTypes CvMinorCivAI::GetAlly() const
{
	return m_eAlly;
}

/// Sets who has the best relations with us right now
void CvMinorCivAI::SetAlly(PlayerTypes eNewAlly)
{
	CvAssertMsg(eNewAlly >= NO_PLAYER, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eNewAlly < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	CvMap& theMap = GC.getMap();
	int iNumPlots = GC.getMap().numPlots();

	PlayerTypes eOldAlly = GetAlly();

	int iPlotVisRange = GC.getPLOT_VISIBILITY_RANGE();

	if (eOldAlly != NO_PLAYER)
	{
		for (int iI = 0; iI < iNumPlots; iI++)
		{
			CvPlot* pPlot = theMap.plotByIndexUnchecked(iI);
			if (pPlot->getOwner() == m_pPlayer->GetID())
			{
				pPlot->changeAdjacentSight( GET_PLAYER(eOldAlly).getTeam(), iPlotVisRange, false, NO_INVISIBLE, NO_DIRECTION, false);
			}
		}
		if (eOldAlly == GC.getGame().getActivePlayer())
		{
			theMap.updateDeferredFog();
		}
	}

	m_eAlly = eNewAlly;

	// Seed the GP counter?
	if (eNewAlly != NO_PLAYER)
	{
		CvPlayerAI& kNewAlly = GET_PLAYER(eNewAlly);

		// share the visibility with my ally (and his team-mates)
		for (int iI = 0; iI < iNumPlots; iI++)
		{
			CvPlot* pPlot = theMap.plotByIndexUnchecked(iI);
			if (pPlot->getOwner() == m_pPlayer->GetID())
			{
				pPlot->changeAdjacentSight( kNewAlly.getTeam(), iPlotVisRange, true, NO_INVISIBLE, NO_DIRECTION, false);
			}
		}

		for (int iPolicyLoop = 0; iPolicyLoop < GC.getNumPolicyInfos(); iPolicyLoop++)
		{
			const PolicyTypes eLoopPolicy = static_cast<PolicyTypes>(iPolicyLoop);
			CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(eLoopPolicy);
			if(pkPolicyInfo)
			{
				if (kNewAlly.GetPlayerPolicies()->HasPolicy(eLoopPolicy) && !kNewAlly.GetPlayerPolicies()->IsPolicyBlocked(eLoopPolicy))
				{
					// This is the policy we want!
					if (pkPolicyInfo->IsMinorGreatPeopleAllies())
					{
						if (kNewAlly.GetGreatPeopleSpawnCounter() <= 0)
							kNewAlly.DoSeedGreatPeopleSpawnCounter();
						else
							kNewAlly.DoApplyNewAllyGPBonus();
					}
				}
			}
		}
	}

	// Alter who gets this guy's resources
	DoUpdateAlliesResourceBonus(eNewAlly, eOldAlly);

	// Declare war on Ally's enemies
	if (eNewAlly != NO_PLAYER)
	{
		CvPlayerAI& kNewAlly = GET_PLAYER(eNewAlly);
		CvTeam& kNewAllyTeam = GET_TEAM(kNewAlly.getTeam());
		CvTeam& kOurTeam = GET_TEAM(GetPlayer()->getTeam());

		TeamTypes eLoopTeam;
		for (int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
		{
			eLoopTeam = (TeamTypes) iTeamLoop;

			if (!GET_TEAM(eLoopTeam).isAlive())
				continue;

			if (kNewAllyTeam.isAtWar(eLoopTeam))
				kOurTeam.declareWar(eLoopTeam);
		}
	}

	DoTestEndWarsVSMinors(eOldAlly, eNewAlly);

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pPlayer->GetID());
		args->Push(eOldAlly);
		args->Push(eNewAlly);

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "SetAlly", args.get(), bResult);
	}
}


/// Is ePlayer Allies with this minor?
bool CvMinorCivAI::IsAllies(PlayerTypes ePlayer) const
{
	return m_eAlly == ePlayer;
}

/// Is ePlayer Friends with this minor?
bool CvMinorCivAI::IsFriends(PlayerTypes ePlayer) const
{
	return IsFriendshipAboveFriendsThreshold(GetFriendshipWithMajor(ePlayer));
}

/// Has ePlayer ever been Friends with this minor?
bool CvMinorCivAI::IsEverFriends(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	return m_abEverFriends[ePlayer];
}

/// Has ePlayer ever been Friends with this minor?
void CvMinorCivAI::SetEverFriends(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_abEverFriends[ePlayer] = bValue;
}


/// Are we about to lose our status? (used in Diplo AI)
bool CvMinorCivAI::IsCloseToNotBeingAllies(PlayerTypes ePlayer)
{
	int iBuffer = GetFriendshipWithMajor(ePlayer) - GetAlliesThreshold();

	if (iBuffer >= 0 && iBuffer < /*8*/ GC.getMINOR_FRIENDSHIP_CLOSE_AMOUNT())
		return true;

	return false;
}

/// Are we about to lose our status? (used in Diplo AI)
bool CvMinorCivAI::IsCloseToNotBeingFriends(PlayerTypes ePlayer)
{
	int iBuffer = GetFriendshipWithMajor(ePlayer) - GetFriendsThreshold();

	if (iBuffer >= 0 && iBuffer < /*8*/ GC.getMINOR_FRIENDSHIP_CLOSE_AMOUNT())
		return true;

	return false;
}

/// What level of Friendship does a player have with this Minor?
int CvMinorCivAI::GetFriendshipLevelWithMajor(PlayerTypes ePlayer)
{
	if (IsAllies(ePlayer))
	{
		return 2;
	}
	else if (IsFriends(ePlayer))
	{
		return 1;
	}

	return 0;
}


/// Friendship needed for next level of bonuses
int CvMinorCivAI::GetFriendshipNeededForNextLevel(PlayerTypes ePlayer)
{
	if (!IsFriends(ePlayer))
	{
		return GetFriendsThreshold();
	}
	else if (!IsAllies(ePlayer))
	{
		return GetAlliesThreshold();
	}

	return 0;
}

/// What happens when Friendship changes?
void CvMinorCivAI::DoFriendshipChangeEffects(PlayerTypes ePlayer, int iOldFriendship, int iNewFriendship)
{
	// Can't get bonuses when we're dead!
	if (!GetPlayer()->isAlive())
		return;

	Localization::String strMessage;
	Localization::String strSummary;

	bool bAdd = false;
	bool bFriends = false;
	bool bAllies = false;

	bool bWasAboveFriendsThreshold = IsFriendshipAboveFriendsThreshold(iOldFriendship);
	bool bNowAboveFriendsThreshold = IsFriendshipAboveFriendsThreshold(iNewFriendship);

	// Add Friends Bonus
	if (!bWasAboveFriendsThreshold && bNowAboveFriendsThreshold)
	{
		bAdd = true;
		bFriends = true;

		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem)
		{
			CvLuaArgsHandle args;
			args->Push(m_pPlayer->GetID());
			args->Push(ePlayer);
			args->Push(true);
			args->Push(iOldFriendship);
			args->Push(iNewFriendship);

			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "MinorFriendsChanged", args.get(), bResult);
		}
	}
	// Remove Friends bonus
	else if (bWasAboveFriendsThreshold && !bNowAboveFriendsThreshold)
	{
		bAdd = false;
		bFriends = true;

		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem)
		{
			CvLuaArgsHandle args;
			args->Push(m_pPlayer->GetID());
			args->Push(ePlayer);
			args->Push(false);
			args->Push(iOldFriendship);
			args->Push(iNewFriendship);

			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "MinorFriendsChanged", args.get(), bResult);
		}
	}

	// Resolve Allies status
	bool bWasAboveAlliesThreshold = IsFriendshipAboveAlliesThreshold(iOldFriendship);
	bool bNowAboveAlliesThreshold = IsFriendshipAboveAlliesThreshold(iNewFriendship);

	PlayerTypes eOldAlly = GetAlly();

	// No old ally and our friendship is now above the threshold, OR our friendship is now higher than a previous ally
	if ((eOldAlly == NO_PLAYER && !bWasAboveAlliesThreshold && bNowAboveAlliesThreshold)
		|| (eOldAlly != NO_PLAYER && GetFriendshipWithMajor(ePlayer) > GetFriendshipWithMajor(eOldAlly)))
	{
		bAdd = true;
		bAllies = true;


		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem) 
		{
			CvLuaArgsHandle args;
			args->Push(m_pPlayer->GetID());
			args->Push(ePlayer);
			args->Push(true);
			args->Push(iOldFriendship);
			args->Push(iNewFriendship);

			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "MinorFriendsChanged", args.get(), bResult);
		}
	}
	// Remove Allies bonus
	else if (eOldAlly == ePlayer && bWasAboveAlliesThreshold && !bNowAboveAlliesThreshold)
	{
		bAdd = false;
		bAllies = true;

		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem) 
		{
			CvLuaArgsHandle args;
			args->Push(m_pPlayer->GetID());
			args->Push(ePlayer);
			args->Push(false);
			args->Push(iOldFriendship);
			args->Push(iNewFriendship);

			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "MinorFriendsChanged", args.get(), bResult);
		}
	}

	// Make changes to bonuses here, and send out notifications
	if (bFriends || bAllies)
		DoSetBonus(ePlayer, bAdd, bFriends, bAllies);

	// Now actually changed Allied status, since we needed the old player in effect to create the notifications in the function above us
	if (bAllies)
	{
		if (bAdd)
			SetAlly(ePlayer);
		else
			SetAlly(NO_PLAYER);	// We KNOW no one else can be higher, so set the Ally to NO_PLAYER
	}


	//int iNumResourceTypes = 0;

	//std::vector<ResourceTypes> veResources;

	//ResourceTypes eResource;
	//ResourceUsageTypes eUsage;
	//int iResourceQuantity;
	//for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	//{
	//	eResource = (ResourceTypes) iResourceLoop;
	//	iResourceQuantity = GetPlayer()->getNumResourceTotal(eResource);

	//	if (iResourceQuantity > 0)
	//	{
	//		eUsage = GC.getResourceInfo(eResource)->getResourceUsage();

	//		if (eUsage == RESOURCEUSAGE_STRATEGIC || eUsage == RESOURCEUSAGE_LUXURY)
	//		{
	//			veResources.push_back(eResource);
	//			iNumResourceTypes++;
	//		}
	//	}
	//}

	//Localization::String strMessage;

	//// Minor hasn't hooked up any Luxuries or Strategics yet
	//if (iNumResourceTypes == 0)
	//{
	//	strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_GAINED_BEST_RELATIONS_BONUS_NONE");
	//	strMessage << strMinorsNameKey;
	//}
	//// Minor has something connected
	//else
	//{
	//	CvString strResourceNames = GC.getResourceInfo(veResources[0])->GetDescription();
	//	int i = 1;
	//	while (i < iNumResourceTypes)
	//	{
	//		strResourceNames += ", ";
	//		strResourceNames += GC.getResourceInfo(veResources[i++])->GetDescription();
	//	}

	//	strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_GAINED_BEST_RELATIONS_BONUS_SOME");
	//	strMessage << strMinorsNameKey << strResourceNames.c_str();
	//}

	//Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_GAINED_BEST_RELATIONS_BONUS");
	//strSummary << strMinorsNameKey;














	//// Add Allies Bonus
	//if (!bWasAboveAlliesThreshold && bNowAboveAlliesThreshold)
	//{
	//	bAdd = true;
	//	bAllies = true;

	//	iOldFriendshipLevel = 1;
	//	iNewFriendshipLevel = 2;
	//}
	//// Remove Allies bonus
	//else if (bWasAboveAlliesThreshold && !bNowAboveAlliesThreshold)
	//{
	//	bAdd = false;
	//	bAllies = true;

	//	iOldFriendshipLevel = 2;
	//	iNewFriendshipLevel = 1;
	//}

	// NOTIFICATION FOR OTHER PLAYERS IN THE GAME

	//if (ePlayer != GC.getGame().getActivePlayer())
	//{
	//	CvTeam* pActiveTeam = &GET_TEAM(GC.getGame().getActiveTeam());
	//	TeamTypes eMinorTeam = GetPlayer()->getTeam();

	//	// Has the active player met this minor
	//	if (pActiveTeam->isHasMet(eMinorTeam))
	//	{
	//		const char* strMinorsNameKey = GetPlayer()->getNameKey();
	//		const char* strThisMajorsNameKey;

	//		// Active player has met this major
	//		TeamTypes eOldBFFTeam = GET_PLAYER(ePlayer).getTeam();
	//		if (pActiveTeam->isHasMet(eOldBFFTeam))
	//		{
	//			strThisMajorsNameKey = GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey();
	//		}
	//		// Active player has NOT met the old BFF
	//		else
	//		{
	//			strThisMajorsNameKey = "TXT_KEY_UNMET_PLAYER";
	//		}

	//		bool bSomethingChanged = false;

	//		// Now Allies
	//		if (iNewFriendshipLevel >= 2 && iOldFriendshipLevel < 2)
	//		{
	//			bSomethingChanged = true;

	//			strMessage = Localization::Lookup("TXT_KEY_NTFN_MINOR_NOW_ALLIES_ALL");
	//			strMessage << strThisMajorsNameKey << strMinorsNameKey;
	//			strSummary = Localization::Lookup("TXT_KEY_NTFN_SMMRY_MINOR_NOW_ALLIES_ALL");
	//			strSummary << strMinorsNameKey << strThisMajorsNameKey;

	//		}
	//		// Were Allies, now Friends
	//		else if (iNewFriendshipLevel == 1 && iOldFriendshipLevel >= 2)
	//		{
	//			bSomethingChanged = true;

	//			strMessage = Localization::Lookup("TXT_KEY_NTFN_MINOR_WERE_ALLIES_NOW_FRIENDS_ALL");
	//			strMessage << strThisMajorsNameKey << strMinorsNameKey;
	//			strSummary = Localization::Lookup("TXT_KEY_NTFN_SMMRY_MINOR_NOW_FRIENDS_ALL");
	//			strSummary << strMinorsNameKey << strThisMajorsNameKey;
	//		}
	//		// Now Friends
	//		else if (iNewFriendshipLevel >= 1 && iOldFriendshipLevel < 1)
	//		{
	//			bSomethingChanged = true;

	//			strMessage = Localization::Lookup("TXT_KEY_NTFN_MINOR_NOW_FRIENDS_ALL");
	//			strMessage << strThisMajorsNameKey << strMinorsNameKey;
	//			strSummary = Localization::Lookup("TXT_KEY_NTFN_SMMRY_MINOR_NOW_FRIENDS_ALL");
	//			strSummary << strMinorsNameKey << strThisMajorsNameKey;
	//		}
	//		// No longer Friends
	//		else if (iNewFriendshipLevel == 0 && iOldFriendshipLevel == 1)
	//		{
	//			bSomethingChanged = true;

	//			strMessage = Localization::Lookup("TXT_KEY_NTFN_MINOR_WERE_FRIENDS_ALL");
	//			strMessage << strThisMajorsNameKey << strMinorsNameKey;
	//			strSummary = Localization::Lookup("TXT_KEY_NTFN_SMMRY_MINOR_WERE_FRIENDS_ALL");
	//			strSummary << strMinorsNameKey << strThisMajorsNameKey;
	//		}

	//		if (bSomethingChanged)
	//		{
	//			AddNotification(strMessage, strSummary, GC.getGame().getActivePlayer());
	//		}
	//	}
	//}
}


/// Is the player above the "Friends" threshold?
bool CvMinorCivAI::IsFriendshipAboveFriendsThreshold(int iFriendship) const
{
	int iFriendshipThresholdFriends = GetFriendsThreshold();

	if (iFriendship >= iFriendshipThresholdFriends)
	{
		return true;
	}

	return false;
}


/// What is the friends threshold?
int CvMinorCivAI::GetFriendsThreshold() const
{
	return /*30*/ GC.getFRIENDSHIP_THRESHOLD_FRIENDS();
}

/// Is the player above the treshold to get the Allies bonus?
bool CvMinorCivAI::IsFriendshipAboveAlliesThreshold(int iFriendship) const
{
	int iFriendshipThresholdAllies = GetAlliesThreshold();

	if (iFriendship >= iFriendshipThresholdAllies)
	{
		return true;
	}

	return false;
}

/// What is the allies threshold?
int CvMinorCivAI::GetAlliesThreshold() const
{
	return /*60*/ GC.getFRIENDSHIP_THRESHOLD_ALLIES();
}

/// Sets a major to get a Bonus (or not) - set both bFriends and bAllies to be true if you're adding/removing both states at once
void CvMinorCivAI::DoSetBonus(PlayerTypes ePlayer, bool bAdd, bool bFriends, bool bAllies, bool bPassedBySomeone, PlayerTypes eNewAlly)
{
	Localization::String strDetailedInfo;
	Localization::String strMessage;
	Localization::String strSummary;

	MinorCivTraitTypes eTrait = GetTrait();

	// Cultured
	if (eTrait == MINOR_CIV_TRAIT_CULTURED)
	{
		int iCultureBonusAmount = 0;

		if (bFriends)	// Friends bonus
			iCultureBonusAmount += GetCultureFriendshipBonus(ePlayer);
		if (bAllies)		// Allies bonus
			iCultureBonusAmount += GetCultureAlliesBonus(ePlayer);
		if (!bAdd)		// Flip amount of we're taking bonuses away
			iCultureBonusAmount = -iCultureBonusAmount;

		// Modify the bonus if called for by our trait
		int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();
		if (iModifier > 0)
		{
			iCultureBonusAmount *= (iModifier + 100);
			iCultureBonusAmount /= 100;
		}

		GET_PLAYER(ePlayer).ChangeJONSCulturePerTurnFromMinorCivs(iCultureBonusAmount);

		if (bAllies && bAdd)		// Now Allies (includes jump from nothing through Friends to Allies)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_CULTURE");
			strDetailedInfo << iCultureBonusAmount;
		}
		else if (bFriends && bAdd)		// Now Friends
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_CULTURE");
			strDetailedInfo << iCultureBonusAmount;
		}
		else if (!bAdd)		// Bonus diminished (or removed)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LOST_CULTURE");
			strDetailedInfo << iCultureBonusAmount;
		}
	}

	// Militaristic
	else if (eTrait == MINOR_CIV_TRAIT_MILITARISTIC)
	{
		if (bAdd)
		{
			// Seed Counter if it hasn't been done yet in this game. We don't have to undo this at any point because the counter is not processed if we are no longer Friends
			if (GetUnitSpawnCounter(ePlayer) == -1)
				DoSeedUnitSpawnCounter(ePlayer, /*bBias*/ true);
		}

		if (bAllies && bAdd)		// Now Allies (includes jump from nothing through Friends to Allies)
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_MILITARISTIC");
		else if (bFriends && bAdd)		// Now Friends
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_MILITARISTIC");
		else if (bFriends && !bAdd)		// No longer Friends (includes drop from Allies down to nothing) - this should be before the Allies check!
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LOST_FRIENDS_MILITARISTIC");
		else if (bAllies && !bAdd)		// No longer Allies
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LOST_ALLIES_MILITARISTIC");
	}

	// Maritime
	else if (eTrait == MINOR_CIV_TRAIT_MARITIME)
	{
		int iCapitalFoodTimes100 = 0;
		int iOtherCitiesFoodTimes100 = 0;

		if (bFriends)	// Friends bonus
		{
			iCapitalFoodTimes100 += GetFriendsCapitalFoodBonus(ePlayer);
			iOtherCitiesFoodTimes100 += GetFriendsOtherCityFoodBonus(ePlayer);
		}
		if (bAllies)		// Allies bonus
		{
			iCapitalFoodTimes100 += GetAlliesCapitalFoodBonus(ePlayer);
			iOtherCitiesFoodTimes100 += GetAlliesOtherCityFoodBonus(ePlayer);
		}

		if (!bAdd)		// Flip amount of we're taking bonuses away
		{
			iCapitalFoodTimes100 = -iCapitalFoodTimes100;
			iOtherCitiesFoodTimes100 = -iOtherCitiesFoodTimes100;
		}

		GET_PLAYER(ePlayer).ChangeCapitalYieldChange(YIELD_FOOD, iCapitalFoodTimes100);
		GET_PLAYER(ePlayer).ChangeCityYieldChange(YIELD_FOOD, iOtherCitiesFoodTimes100);

		// Now that we've changed the gameplay, add together the two so the DISPLAY looks right
		iCapitalFoodTimes100 += iOtherCitiesFoodTimes100;
		float fCapitalFood = float(iCapitalFoodTimes100) / 100;
		float fOtherCitiesFood = float(iOtherCitiesFoodTimes100) / 100;
		//iCapitalFood += iOtherCitiesFood;

		if (bAllies && bAdd)		// Now Allies (includes jump from nothing through Friends to Allies)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_MARITIME");
			strDetailedInfo << fCapitalFood << fOtherCitiesFood;
		}
		else if (bFriends && bAdd)		// Now Friends
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_MARITIME");
			strDetailedInfo << fCapitalFood << fOtherCitiesFood;
		}
		else if (!bAdd)		// Bonus diminished (or removed)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LOST_MARITIME");
			strDetailedInfo << fCapitalFood << fOtherCitiesFood;
		}
	}

	PlayerTypes eOldAlly = GetAlly();
	TeamTypes eOldAllyTeam = eOldAlly != NO_PLAYER ? GET_PLAYER(eOldAlly).getTeam() : NO_TEAM;

	// Should we remove the Ally bonus from another player?
	if (bAdd && bAllies)
	{
		if (eOldAlly != NO_PLAYER && ePlayer != eOldAlly)
			DoSetBonus(eOldAlly, /*bAdd*/ false, /*bFriends*/ false, /*bAllies*/ true, /*bPassedBySomeone*/ true, ePlayer);
	}

	// *******************************************
	// NOTIFICATIONS FOR OTHER PLAYERS IN THE GAME
	// *******************************************
	Localization::String strMessageOthers;
	Localization::String strSummaryOthers;

	// We need to do this because this function is recursive, and if we're UNDOING someone else, we don't yet know who the new guy is because it hasn't been set yet
	if(bPassedBySomeone)
		ePlayer = eNewAlly;

	for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
		PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
		CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
		CvTeam* pNotifyTeam = &GET_TEAM(kCurNotifyPlayer.getTeam());
		TeamTypes eNewAllyTeam = GET_PLAYER(ePlayer).getTeam();
		const char* strNewBestPlayersNameKey;

		// Notify player has met the new Ally
		if(pNotifyTeam->isHasMet(eNewAllyTeam))
			strNewBestPlayersNameKey = GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey();
		// Notify player has NOT met the new Ally
		else
			strNewBestPlayersNameKey = "TXT_KEY_UNMET_PLAYER";

		const char* strOldBestPlayersNameKey = "";

		// Someone got passed up
		if(eOldAlly != NO_PLAYER)
		{
			// Notify player has met the old Ally
			if(pNotifyTeam->isHasMet(eOldAllyTeam))
				strOldBestPlayersNameKey = GET_PLAYER(eOldAlly).getCivilizationShortDescriptionKey();
			// Notify player has NOT met the old Ally
			else
				strOldBestPlayersNameKey = "TXT_KEY_UNMET_PLAYER";
		}

		const char* strMinorsNameKey = GetPlayer()->getNameKey();
		TeamTypes eMinorTeam = GetPlayer()->getTeam();

		// Adding/Increasing bonus
		if(bAdd)
		{
			// Jumped up to Allies (either from Neutral or from Friends, or passing another player)
			if(bAllies)
			{
				if(ePlayer != eNotifyPlayer)
				{
					// Has the notify player met this minor
					if(pNotifyTeam->isHasMet(eMinorTeam))
					{
						// Someone got passed up
						if(eOldAlly != NO_PLAYER && eOldAlly != ePlayer)
						{
							strMessageOthers = Localization::Lookup("TXT_KEY_NTFN_MINOR_NEW_BEST_RELATIONS_ALL");
							strMessageOthers << strNewBestPlayersNameKey << strOldBestPlayersNameKey << strMinorsNameKey;
							strSummaryOthers = Localization::Lookup("TXT_KEY_NTFN_SMMRY_MINOR_BEST_RELATIONS_ALL");
							strSummaryOthers << strMinorsNameKey;
						}
						// No one previously had the bonus
						else
						{
							strMessageOthers = Localization::Lookup("TXT_KEY_NTFN_MINOR_NOW_BEST_RELATIONS_ALL");
							strMessageOthers << strNewBestPlayersNameKey << strMinorsNameKey;
							strSummaryOthers = Localization::Lookup("TXT_KEY_NTFN_SMMRY_MINOR_NOW_ALLIES_ALL");
							strSummaryOthers << strMinorsNameKey << strNewBestPlayersNameKey;
						}

						// If we're being passed by someone, then don't display this message... we'll roll it into a later one
						if(eOldAlly != eNotifyPlayer)
							AddNotification(strMessageOthers.toUTF8(), strSummaryOthers.toUTF8(), eNotifyPlayer);
					}
				}
			}
		}
		// Removing/Reducing bonus
		else
		{
			// Dropped from Allies
			if(bAllies)
			{
				if(ePlayer != eNotifyPlayer)
				{
					if(pNotifyTeam->isHasMet(eMinorTeam))
					{
						// Only show this message for normal friendship decay
						if(!bPassedBySomeone)
						{
							const char* strOldAllyNameKey;

							// Notify player has met the old Ally
							if(pNotifyTeam->isHasMet(eOldAllyTeam))
								strOldAllyNameKey = GET_PLAYER(eOldAlly).getCivilizationShortDescriptionKey();
							// Notify player has NOT met the old Ally
							else
								strOldAllyNameKey = "TXT_KEY_UNMET_PLAYER";

							strMessageOthers = Localization::Lookup("TXT_KEY_NTFN_MINOR_BEST_RELATIONS_LOST_ALL");
							strMessageOthers << strOldAllyNameKey << strMinorsNameKey;
							strSummaryOthers = Localization::Lookup("TXT_KEY_NTFN_SMMRY_MINOR_BEST_RELATIONS_LOST_ALL");
							strSummaryOthers << strMinorsNameKey << strOldAllyNameKey;

							AddNotification(strMessageOthers.toUTF8(), strSummaryOthers.toUTF8(), eNotifyPlayer);
						}
					}
				}
			}
		}
	}
}





/// Major Civs intruding in our lands?
void CvMinorCivAI::DoIntrusion()
{
	PlayerTypes eMajor;
	int iMajorLoop;
	for (iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;

		if (GetAngerFreeIntrusionCounter(eMajor) > 0)
		{
			ChangeAngerFreeIntrusionCounter(eMajor, -1);
		}
	}

	// If this minor has an active quest, don't worry about intruders
	if (IsWarQuestWithAnyoneActive())
		return;

	// If there are barbs nearby then don't worry about other players
	if (GetNumThreateningBarbarians() > 0)
		return;

	CvPlot* pLoopPlot;
	const IDInfo* pUnitNode;
	const CvUnit* pLoopUnit;

	// Set up scratch pad so that we can use it to send out Notifications
	for (iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;
		SetMajorScratchPad(eMajor, 0);
	}

	// Look at how many Units each Major Civ has in the Minor's Territory
	for (int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

		// Plot owned by this Minor?
		if (pLoopPlot->getOwner() == GetPlayer()->GetID())
		{
			pUnitNode = pLoopPlot->headUnitNode();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(*pUnitNode);
				pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

				// Does this unit not cause anger?
				if (pLoopUnit && pLoopUnit->IsAngerFreeUnit())
					continue;

				//// Don't look at Unit's this Minor wants for a Quest!
				//if (!IsPeaceQuestCompletedByMajor((PlayerTypes) pLoopUnit->getOwner()))
				//{
				//	if (GetPeaceQuestWidget() == MINORCIVQUESTWIDGET_UNITCLASS || GetWarQuestWidget() == MINORCIVQUESTWIDGET_UNITCLASS)
				//	{
				//		if (GetPeaceQuestWidgetID() == pLoopUnit->getUnitClassType() || GetWarQuestWidgetID() == pLoopUnit->getUnitClassType())
				//		{
				//			continue;
				//		}
				//	}
				//}

				// Does this Unit belong to a Major?
				if (pLoopUnit && pLoopUnit->getOwner() < MAX_MAJOR_CIVS)
				{
					// If player has been granted Open Borders or has a friendship with minors bonus, then the Minor doesn't care about intrusion
					if (!IsPlayerHasOpenBorders(pLoopUnit->getOwner()) && GetAngerFreeIntrusionCounter(pLoopUnit->getOwner()) == 0)
					{
						// If the player is at war with the Minor then don't bother
						if (!IsAtWarWithPlayersTeam(pLoopUnit->getOwner()))
						{
							ChangeFriendshipWithMajor(pLoopUnit->getOwner(), /*-6*/ GC.getFRIENDSHIP_PER_UNIT_INTRUDING());

							// only modify if the unit isn't automated nor having a pending order
							if (!pLoopUnit->IsAutomated() && pLoopUnit->GetLengthMissionQueue() == 0)
							{
								SetMajorScratchPad(pLoopUnit->getOwner(), 1);
							}

							//if (!IsMajorIntruding((PlayerTypes) pLoopUnit->getOwner()))
							//{
							//	SetMajorIntruding((PlayerTypes) pLoopUnit->getOwner(), true);
							//	ChangeNumTurnsMajorHasIntruded((PlayerTypes) pLoopUnit->getOwner(), 1);
							//}
						}
					}
				}
			}
		}
	}

	// Now send out Notifications (if necessary)
	Localization::String strMessage;
	Localization::String strSummary;

	for (iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;

		if (GetMajorScratchPad(eMajor) > 0)
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_INTRUSION");
			strMessage << GetPlayer()->getNameKey();
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_INTRUSION");
			strSummary << GetPlayer()->getNameKey();

			AddNotification(strMessage, strSummary, eMajor);
		}
	}
}

/// Is a Major Civ mackin on our turf?
bool CvMinorCivAI::IsMajorIntruding(PlayerTypes eMajor) const
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor< 0 || eMajor >= MAX_MAJOR_CIVS) return false;

	return m_abMajorIntruding[eMajor];
}

/// Sets a Major to be intruding in our territory
void CvMinorCivAI::SetMajorIntruding(PlayerTypes eMajor, bool bValue)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;

	if (IsMajorIntruding(eMajor) != bValue)
	{
		m_abMajorIntruding[eMajor] = bValue;
	}
}


/// Is a player allowed to be inside someone else's borders?
bool CvMinorCivAI::IsPlayerHasOpenBorders(PlayerTypes ePlayer)
{
	// Special trait?
	if (IsPlayerHasOpenBordersAutomatically(ePlayer))
		return true;

	return IsFriends(ePlayer);
}

/// Is a player allowed to be inside someone else's borders automatically?
bool CvMinorCivAI::IsPlayerHasOpenBordersAutomatically(PlayerTypes ePlayer)
{
	// Special trait?
	if (GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateFriendshipModifier() > 0)
		return true;

	return false;
}


/// Major liberates a Minor by recapturing its City!
void CvMinorCivAI::DoLiberationByMajor(PlayerTypes eMajor)
{
	Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LIBERATION");
	strMessage << GetPlayer()->getNameKey() << GET_PLAYER(eMajor).getNameKey();
	Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_LIBERATION");
	strSummary << GetPlayer()->getNameKey();

	PlayerTypes ePlayer;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		ePlayer = (PlayerTypes) iI;

		// Liberating player gets his own special notification
		if (ePlayer != eMajor)
		{
			if (GET_PLAYER(ePlayer).isAlive())
			{
				// Has this player met the minor?
				if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GetPlayer()->getTeam()))
				{
					CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
					if (pNotifications)
					{
						pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strMessage.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
					}
				}
			}
		}
	}

	strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LIBERATION_YOU");
	strMessage << GetPlayer()->getNameKey();
	strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_LIBERATION");
	strSummary << GetPlayer()->getNameKey();

	AddNotification(strMessage, strSummary, eMajor);

	int iFriendshipChange = /*150*/ GC.getMINOR_LIBERATION_FRIENDSHIP();
	ChangeFriendshipWithMajor(eMajor, iFriendshipChange);
}



// ******************************
// ***** Friendship - with Benefits *****
// ******************************



/// Someone changed eras - does this affect their bonuses?
bool CvMinorCivAI::DoMajorCivEraChange(PlayerTypes ePlayer, EraTypes eNewEra)
{
	bool bSomethingChanged = false;

	MinorCivTraitTypes eTrait = GetTrait();

	// MARITIME
	if (eTrait == MINOR_CIV_TRAIT_MARITIME)
	{
		// Friends
		if (IsFriends(ePlayer))
		{
			int iOldFood, iNewFood;

			// Capital
			iOldFood = GetFriendsCapitalFoodBonus(ePlayer);
			iNewFood = GetFriendsCapitalFoodBonus(ePlayer, eNewEra);

			if (iOldFood != iNewFood)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).ChangeCapitalYieldChange(YIELD_FOOD, iNewFood - iOldFood);
			}

			// Other Cities
			iOldFood = GetFriendsOtherCityFoodBonus(ePlayer);
			iNewFood = GetFriendsOtherCityFoodBonus(ePlayer, eNewEra);

			if (iOldFood != iNewFood)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).ChangeCityYieldChange(YIELD_FOOD, iNewFood - iOldFood);
			}
		}

		// Allies
		if (IsAllies(ePlayer))
		{
			int iOldFood, iNewFood;

			// Capital
			iOldFood = GetAlliesCapitalFoodBonus(ePlayer);
			iNewFood = GetAlliesCapitalFoodBonus(ePlayer);

			if (iOldFood != iNewFood)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).ChangeCapitalYieldChange(YIELD_FOOD, iNewFood - iOldFood);
			}

			// Other Cities
			iOldFood = GetAlliesOtherCityFoodBonus(ePlayer);
			iNewFood = GetAlliesOtherCityFoodBonus(ePlayer);

			if (iOldFood != iNewFood)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).ChangeCityYieldChange(YIELD_FOOD, iNewFood - iOldFood);
			}
		}
	}

	// CULTURED
	if (eTrait == MINOR_CIV_TRAIT_CULTURED)
	{
		// Friends
		if (IsFriends(ePlayer))
		{
			int iOldCulture, iNewCulture;

			iOldCulture = GetCultureFriendshipBonus(ePlayer);
			iNewCulture = GetCultureFriendshipBonus(ePlayer, eNewEra);

			if (iOldCulture != iNewCulture)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).ChangeJONSCulturePerTurnFromMinorCivs(iNewCulture - iOldCulture);
			}
		}

		// Allies
		if (IsAllies(ePlayer))
		{
			int iOldCulture, iNewCulture;

			iOldCulture = GetCultureAlliesBonus(ePlayer);
			iNewCulture = GetCultureAlliesBonus(ePlayer, eNewEra);

			if (iOldCulture != iNewCulture)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).ChangeJONSCulturePerTurnFromMinorCivs(iNewCulture - iOldCulture);
			}
		}
	}

	return bSomethingChanged;
}

// Science bonus when friends with a minor
int CvMinorCivAI::GetScienceFriendshipBonus ()
{
	int iResult = GetScienceFriendshipBonusTimes100();
	iResult /= 100;

	return iResult;
}

int CvMinorCivAI::GetScienceFriendshipBonusTimes100 ()
{
	int iResult = GET_PLAYER(m_pPlayer->GetID()).GetScienceTimes100() * /*25*/ GC.getMINOR_CIV_SCIENCE_BONUS_MULTIPLIER();
	iResult /= 100;

	return iResult;
}

/// How much are we getting RIGHT NOW (usually 0)
int CvMinorCivAI::GetCurrentScienceFriendshipBonusTimes100(PlayerTypes ePlayer)
{
	if (GET_PLAYER(ePlayer).IsGetsScienceFromPlayer(GetPlayer()->GetID()))
		return GetScienceFriendshipBonusTimes100();

	return 0;
}

// Culture bonus when Friends with a minor
int CvMinorCivAI::GetCultureFriendshipBonus (PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	EraTypes eCurrentEra = eAssumeEra;
	if (eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);

	// Industrial era or Later
	if (eCurrentEra >= eIndustrial)
		return /*10*/ GC.getFRIENDS_CULTURE_BONUS_AMOUNT_INDUSTRIAL();

	// Medieval era or later
	else if (eCurrentEra >= eMedieval)
		return /*6*/ GC.getFRIENDS_CULTURE_BONUS_AMOUNT_MEDIEVAL();

	// Pre-Medieval
	else
		return /*4*/ GC.getFRIENDS_CULTURE_BONUS_AMOUNT_ANCIENT();
}

// Culture bonus when Allies with a minor
int CvMinorCivAI::GetCultureAlliesBonus (PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	EraTypes eCurrentEra = eAssumeEra;
	if (eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);

	// Industrial era or Later
	if (eCurrentEra >= eIndustrial)
		return /*10*/ GC.getALLIES_CULTURE_BONUS_AMOUNT_INDUSTRIAL();

	// Medieval era or later
	else if (eCurrentEra >= eMedieval)
		return /*6*/ GC.getALLIES_CULTURE_BONUS_AMOUNT_MEDIEVAL();

	// Pre-Medieval
	else
		return /*4*/ GC.getALLIES_CULTURE_BONUS_AMOUNT_ANCIENT();
}

/// How much are we getting RIGHT NOW (usually 0)
int CvMinorCivAI::GetCurrentCultureBonus (PlayerTypes ePlayer)
{
	// This guy isn't Cultured
	if (GetTrait() != MINOR_CIV_TRAIT_CULTURED)
		return 0;

	int iAmount = 0;

	if (IsAllies(ePlayer))
		iAmount += GetCultureAlliesBonus(ePlayer);

	if (IsFriends(ePlayer))
		iAmount += GetCultureFriendshipBonus(ePlayer);

	return iAmount;
}


// Happiness bonus when friends with a minor
int CvMinorCivAI::GetHappinessFriendshipBonus (PlayerTypes /* ePlayer */)
{
	int iResult = 0;

	// Minor is most friendly with this player
	//if (GetAlly() == ePlayer)
	//{
	//	iResult = /*2*/ GC.getMINOR_CIV_BEST_RELATIONS_HAPPINESS_BONUS();
	//	iResult += (m_pPlayer->getTotalPopulation() + 1) / 2;
	//}

	return iResult;
}

// Food bonus when Friends with a minor - additive with general city bonus
int CvMinorCivAI::GetFriendsCapitalFoodBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	int iBonus;

	EraTypes eCurrentEra = eAssumeEra;
	if (eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);

	// Medieval era or sooner
	if (eCurrentEra < eRenaissance)
		iBonus = /*200*/ GC.getFRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE();

	// Renaissance era or later
	else
		iBonus = /*200*/ GC.getFRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_POST_RENAISSANCE();

	// Modify the bonus if called for by our trait
	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();
	if (iModifier > 0)
	{
		iBonus *= (iModifier + 100);
		iBonus /= 100;
	}

	return iBonus;
}

// Food bonus when Friends with a minor
int CvMinorCivAI::GetFriendsOtherCityFoodBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	int iBonus;

	EraTypes eCurrentEra = eAssumeEra;
	if (eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);

	// Medieval era or sooner
	if (eCurrentEra < eRenaissance)
		iBonus = /*0*/ GC.getFRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE();

	// Renaissance era or later
	else
		iBonus = /*0*/ GC.getFRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_POST_RENAISSANCE();

	// Modify the bonus if called for by our trait
	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();
	if (iModifier > 0)
	{
		iBonus *= (iModifier + 100);
		iBonus /= 100;
	}

	return iBonus;
}

// Food bonus when Allies with a minor - additive with general city bonus
int CvMinorCivAI::GetAlliesCapitalFoodBonus(PlayerTypes ePlayer)
{
	int iBonus = /*100*/ GC.getALLIES_CAPITAL_FOOD_BONUS_AMOUNT();

	// Modify the bonus if called for by our trait
	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();
	if (iModifier > 0)
	{
		iBonus *= (iModifier + 100);
		iBonus /= 100;
	}

	return iBonus;
}

// Food bonus when Allies with a minor
int CvMinorCivAI::GetAlliesOtherCityFoodBonus(PlayerTypes ePlayer)
{
	int iBonus = /*100*/ GC.getALLIES_OTHER_CITIES_FOOD_BONUS_AMOUNT();

	// Modify the bonus if called for by our trait
	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();
	if (iModifier > 0)
	{
		iBonus *= (iModifier + 100);
		iBonus /= 100;
	}

	return iBonus;
}

/// How much are we getting RIGHT NOW (usually 0)
int CvMinorCivAI::GetCurrentCapitalFoodBonus (PlayerTypes ePlayer)
{
	// This guy isn't Maritime
	if (GetTrait() != MINOR_CIV_TRAIT_MARITIME)
		return 0;

	int iAmount = 0;

	if (IsAllies(ePlayer))
	{
		iAmount += GetAlliesCapitalFoodBonus(ePlayer);
		iAmount += GetAlliesOtherCityFoodBonus(ePlayer);
	}

	if (IsFriends(ePlayer))
	{
		iAmount += GetFriendsCapitalFoodBonus(ePlayer);
		iAmount += GetFriendsOtherCityFoodBonus(ePlayer);
	}

	return iAmount;
}

/// How much are we getting RIGHT NOW (usually 0)
int CvMinorCivAI::GetCurrentOtherCityFoodBonus (PlayerTypes ePlayer)
{
	// This guy isn't Maritime
	if (GetTrait() != MINOR_CIV_TRAIT_MARITIME)
		return 0;

	int iAmount = 0;

	if (IsAllies(ePlayer))
		iAmount += GetAlliesOtherCityFoodBonus(ePlayer);

	if (IsFriends(ePlayer))
		iAmount += GetFriendsOtherCityFoodBonus(ePlayer);

	return iAmount;
}

// Figures out how long before we spawn a free unit for ePlayer
void CvMinorCivAI::DoSeedUnitSpawnCounter(PlayerTypes ePlayer, bool bBias)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	int iNumTurns = /*19*/ GC.getFRIENDS_BASE_TURNS_UNIT_SPAWN();

	// If relations are at allied level then reduce spawn counter
	if (IsAllies(ePlayer))
		iNumTurns += /*-3*/ GC.getALLIES_EXTRA_TURNS_UNIT_SPAWN();

	int iRand = /*3*/ GC.getFRIENDS_RAND_TURNS_UNIT_SPAWN();
	iNumTurns += GC.getGame().getJonRandNum(iRand, "Rand turns for Friendly Minor unit spawn");

	// If we're biasing the result then decrease the number of turns
	if (bBias)
	{
		iNumTurns *= /*50*/ GC.getUNIT_SPAWN_BIAS_MULTIPLIER();
		iNumTurns /= 100;
	}

	// Modify for Game Speed
	iNumTurns *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iNumTurns /= 100;

	// Modify for policies
	CvPlayer &kPlayer = GET_PLAYER(ePlayer);
	int iPolicyMod = kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_UNIT_FREQUENCY_MODIFIER);
	if (iPolicyMod > 0)
	{
		if (GET_TEAM(kPlayer.getTeam()).HasCommonEnemy(m_pPlayer->getTeam()))
		{
			iNumTurns *= 100;
			iNumTurns /= (100 + iPolicyMod);
		}
	}

	SetUnitSpawnCounter(ePlayer, iNumTurns);
}

// How long before we spawn a free unit for ePlayer?
int CvMinorCivAI::GetUnitSpawnCounter(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1; // as defined during Reset()

	return m_aiUnitSpawnCounter[ePlayer];
}

// Sets how long before we spawn a free unit for ePlayer
void CvMinorCivAI::SetUnitSpawnCounter(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_aiUnitSpawnCounter[ePlayer] = iValue;
}

// Changes how long before we spawn a free unit for ePlayer
void CvMinorCivAI::ChangeUnitSpawnCounter(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	SetUnitSpawnCounter(ePlayer, GetUnitSpawnCounter(ePlayer) + iChange);
}

/// Allowed to spawn Units for ePlayer?
bool CvMinorCivAI::IsUnitSpawningAllowed(PlayerTypes ePlayer)
{
	// Must have met ePlayer
	if (!IsHasMetPlayer(ePlayer))
		return false;

	// Must be Militaristic
	if (GetTrait() != MINOR_CIV_TRAIT_MILITARISTIC)
		return false;

	// Can't be at war!
	if (IsAtWarWithPlayersTeam(ePlayer))
		return false;

	// Must be Friends
	if (!IsFriends(ePlayer))
		return false;

	// We must be alive
	if (!GetPlayer()->isAlive())
		return false;

	// They must be alive
	if (!GET_PLAYER(ePlayer).isAlive())
		return false;

	return true;
}

/// Has the player chosen to disable Unit spawning?
bool CvMinorCivAI::IsUnitSpawningDisabled(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	return m_abUnitSpawningDisabled[ePlayer];
}

/// Set the player chosen to disable Unit spawning
void CvMinorCivAI::SetUnitSpawningDisabled(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_abUnitSpawningDisabled[ePlayer] = bValue;
}

/// Create a unit
void CvMinorCivAI::DoSpawnUnit(PlayerTypes ePlayer)
{
	if (!IsUnitSpawningDisabled(ePlayer))
	{
		CvCity* pCapital = GetPlayer()->getCapitalCity();

		// Minor must have Capital
		if (pCapital == NULL)
		{
			FAssertMsg(false, "MINOR CIV AI: Trying to spawn a Unit for a major civ but the minor has no capital. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

			return;
		}

		int iX = pCapital->getX();
		int iY = pCapital->getY();

		// Pick Unit type
		//UnitClassTypes eUnitClass = NO_UNITCLASS;
		UnitTypes eUnit = GC.getGame().GetRandomSpawnUnitType(ePlayer, /*bIncludeUUs*/ true, /*bIncludeRanged*/ true);

		// Spawn Unit
		if (eUnit != NO_UNIT)
		{
			CvUnit* pNewUnit = GET_PLAYER(ePlayer).initUnit(eUnit, iX, iY);

			// If player trait is to enhance minor bonuses, give this unit some free experience
			if (GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier() > 0)
			{
				pNewUnit->changeExperience(GC.getMAX_EXPERIENCE_PER_COMBAT());
			}

			if (!pNewUnit->jumpToNearestValidPlot())
				pNewUnit->kill(false);
			else
			{
				if (GetPlayer()->getCapitalCity())
					GetPlayer()->getCapitalCity()->addProductionExperience(pNewUnit);

				Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_STATE_UNIT_SPAWN");
				strMessage << GetPlayer()->getNameKey();
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_STATE_UNIT_SPAWN");
				strSummary << GetPlayer()->getNameKey();

				AddNotification(strMessage, strSummary, ePlayer, pNewUnit->getX(), pNewUnit->getY());
			}
		}
	}

	// Reseed counter
	DoSeedUnitSpawnCounter(ePlayer);
}

/// Time to spawn a Unit?
void CvMinorCivAI::DoUnitSpawnTurn()
{
	// Loop through all players and see if we should give them a Unit
	PlayerTypes eMajor;
	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;

		if (IsUnitSpawningAllowed(eMajor))
		{
			// Tick down
			if (GetUnitSpawnCounter(eMajor) > 0)
			{
				ChangeUnitSpawnCounter(eMajor, -1);
			}

			// Time to spawn!
			if (GetUnitSpawnCounter(eMajor) == 0)
			{
				DoSpawnUnit(eMajor);
			}
		}
	}
}

/// What is the average number of turns between unit spawns?
int CvMinorCivAI::GetCurrentSpawnEstimate(PlayerTypes ePlayer)
{
	// Not friends
	if (!IsFriends(ePlayer))
		return 0;

	// This guy isn't militaristic
	if (GetTrait() != MINOR_CIV_TRAIT_MILITARISTIC)
		return 0;

	int iNumTurns = /*19*/ GC.getFRIENDS_BASE_TURNS_UNIT_SPAWN() * 100;

	// If relations are at allied level then reduce spawn counter
	if (IsAllies(ePlayer))
		iNumTurns += /*-3*/ (GC.getALLIES_EXTRA_TURNS_UNIT_SPAWN() * 100);

	int iRand = /*3*/ GC.getFRIENDS_RAND_TURNS_UNIT_SPAWN() * 100;
	iNumTurns += (iRand / 2);

	return iNumTurns / 100;
}



// ******************************
// ***** General Minor Civ Stuff *****
// ******************************



// How many units has ePlayer gifted this minor? (used to prevent unit spam for influence exploits)
int CvMinorCivAI::GetNumUnitsGifted(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1; // as defined during Reset()

	return m_aiNumUnitsGifted[ePlayer];
}

// How many units has ePlayer gifted this minor? (used to prevent unit spam for influence exploits)
void CvMinorCivAI::SetNumUnitsGifted(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_aiNumUnitsGifted[ePlayer] = iValue;
}

// How many units has ePlayer gifted this minor? (used to prevent unit spam for influence exploits)
void CvMinorCivAI::ChangeNumUnitsGifted(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	SetNumUnitsGifted(ePlayer, GetNumUnitsGifted(ePlayer) + iChange);
}



/// Major Civ gifted some Gold to this Minor
void CvMinorCivAI::DoGoldGiftFromMajor(PlayerTypes ePlayer, int iGold)
{
	if (GET_PLAYER(ePlayer).GetTreasury()->GetGold() >= iGold)
	{
		GET_PLAYER(ePlayer).GetTreasury()->ChangeGold(-iGold);

		int iFriendshipChange = GetFriendshipFromGoldGift(ePlayer, iGold);

		ChangeFriendshipWithMajor(ePlayer, iFriendshipChange);
	}
}

/// How many friendship points gained from a gift of Gold
int CvMinorCivAI::GetFriendshipFromGoldGift(PlayerTypes eMajor, int iGold)
{
	// The more Gold you spend the more Friendship you get!
	iGold = (int) pow((double) iGold, (double) /*1.01*/ GC.getGOLD_GIFT_FRIENDSHIP_EXPONENT());
	// The higher this divisor the less Friendship is gained
	int iFriendship = int(iGold / /*6.3*/ GC.getGOLD_GIFT_FRIENDSHIP_DIVISOR());

	// Game progress factor ranges from 0.0 to 1.0 based on how far into the game we are
	double fGameProgressFactor = float(GC.getGame().getElapsedGameTurns()) / GC.getGame().getEstimateEndTurn();
	// Tweak factor slightly, otherwise Gold will do literally NOTHING once we reach the end of the game!
	fGameProgressFactor *= /*2*/ GC.getMINOR_CIV_GOLD_GIFT_GAME_MULTIPLIER();
	fGameProgressFactor /= /*3*/ GC.getMINOR_CIV_GOLD_GIFT_GAME_DIVISOR();
	// Now flip the value so we can use it as a multiplier.  If we're on turn 0 this will make the factor 0.  Last turn will be 0.5
	fGameProgressFactor = 1 - fGameProgressFactor;

	iFriendship = (int)(iFriendship * fGameProgressFactor);

	//int iFriendshipGameProgressChange = iFriendship * GC.getGame().getElapsedGameTurns() / GC.getGame().getEstimateEndTurn() * 2 / 3;
	//iFriendship -= iFriendshipGameProgressChange;

	// Mod (Policies, etc.)
	int iFriendshipMod = GET_PLAYER(eMajor).getMinorGoldFriendshipMod();
	if (iFriendshipMod != 0)
	{
		iFriendship *= (100 + iFriendshipMod);
		iFriendship /= 100;
	}

	// Game Speed Mod
	iFriendship *= GC.getGame().getGameSpeedInfo().getGoldGiftMod();
	iFriendship /= 100;

	// Round the number so it's pretty
	int iVisibleDivisor = /*5*/ GC.getMINOR_CIV_GOLD_GIFT_VISIBLE_DIVISOR();
	iFriendship /= iVisibleDivisor;
	iFriendship *= iVisibleDivisor;

	return iFriendship;
}

/// Now at war with eTeam
void CvMinorCivAI::DoNowAtWarWithTeam(TeamTypes eTeam)
{
	int iWarFriendship = /*-60*/ GC.getMINOR_FRIENDSHIP_AT_WAR();

	PlayerTypes ePlayer;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if (GET_PLAYER(ePlayer).getTeam() == eTeam)
		{
			// Friendship loss
			SetFriendshipWithMajor(ePlayer, iWarFriendship);

			// Nullify Quests - Deprecated?
			//GET_PLAYER((PlayerTypes) iMinorCivLoop).GetMinorCivAI()->SetPeaceQuestCompletedByMajor((PlayerTypes) iMajorCivLoop, true);
			//GET_PLAYER((PlayerTypes) iMinorCivLoop).GetMinorCivAI()->SetWarQuestCompletedByMajor((PlayerTypes) iMajorCivLoop, true);

			//// Is this player declaring war also already a Bully?  If so, he's gonna regret it
			//GET_PLAYER((PlayerTypes) iMinorCivLoop).GetMinorCivAI()->DoBullyDeclareWar((PlayerTypes) iMajorCivLoop);
		}
	}
}

/// Now at peace with eTeam
void CvMinorCivAI::DoNowPeaceWithTeam()
{
	DoTestWarWithMajorQuest();
}

/// Will this AI allow peace with ePlayer?
bool CvMinorCivAI::IsPeaceBlocked(TeamTypes eTeam) const
{
	// Permanent war?
	if (IsPermanentWar(eTeam))
		return true;

	// Allies with someone at war with this guy?
	PlayerTypes eMajor;
	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;

		// Major must be alive
		if (!GET_PLAYER(eMajor).isAlive())
			continue;

		// Must be allies
		if (!IsAllies(eMajor))
			continue;

		// Ally must be at war with this team
		if (!GET_TEAM(GET_PLAYER(eMajor).getTeam()).isAtWar(eTeam))
			continue;

		return true;
	}

	return false;
}

/// eTeam declared war on us
void CvMinorCivAI::DoTeamDeclaredWarOnMe(TeamTypes eEnemyTeam)
{
	CvTeam* pEnemyTeam = &GET_TEAM(eEnemyTeam);

	int iRand;

	// Minor Civ Warmonger - permanent war time
	if (pEnemyTeam->IsMinorCivWarmonger())
		SetPermanentWar(eEnemyTeam, true);

	// Minor Civ Aggressor - chance of permanent war
	else if (pEnemyTeam->IsMinorCivAggressor())
	{
		iRand = GC.getGame().getJonRandNum(100, "MINOR CIV AI: Aggressor to permanent war roll.");

		if (iRand < /*50*/ GC.getPERMANENT_WAR_AGGRESSOR_CHANCE())
			SetPermanentWar(eEnemyTeam, true);
	}

	// See if other minors will declare war
	if (pEnemyTeam->IsMinorCivAggressor())
	{
		int iChance;

		PlayerProximityTypes eProximity;

		int iAttackingMajorPlayer;
		PlayerTypes eAttackingMajorPlayer;
		bool bAttackerIsAlly;

		CvPlayer* pOtherMinorCiv;
		PlayerTypes eOtherMinorCiv;
		for (int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
		{
			eOtherMinorCiv = (PlayerTypes) iMinorCivLoop;
			pOtherMinorCiv = &GET_PLAYER((eOtherMinorCiv));

			iChance = 0;

			// Must be alive
			if (!pOtherMinorCiv->isAlive())
				continue;

			// Ignore minors that want THIS minor dead
			if (pOtherMinorCiv->GetMinorCivAI()->IsWantsMinorDead(GetPlayer()->GetID()))
				continue;

			bAttackerIsAlly = false;

			// Ignore minors that are allied to the attacker
			for (iAttackingMajorPlayer = 0; iAttackingMajorPlayer < MAX_MAJOR_CIVS; iAttackingMajorPlayer++)
			{
				eAttackingMajorPlayer = (PlayerTypes) iAttackingMajorPlayer;

				// Not on this team
				if (GET_PLAYER(eAttackingMajorPlayer).getTeam() != eEnemyTeam)
					continue;

				// Not alive
				if (!GET_PLAYER(eAttackingMajorPlayer).isAlive())
					continue;

				if (pOtherMinorCiv->GetMinorCivAI()->GetAlly() == eAttackingMajorPlayer)
				{
					bAttackerIsAlly = true;
					break;
				}
			}

			if (bAttackerIsAlly)
				continue;

			// Closer to the minor the more likely war is
			eProximity = pOtherMinorCiv->GetProximityToPlayer(GetPlayer()->GetID());

			// Warmonger
			if (pEnemyTeam->IsMinorCivWarmonger())
			{
				if (eProximity == PLAYER_PROXIMITY_DISTANT)
					iChance += /*25*/ GC.getPERMANENT_WAR_OTHER_WARMONGER_CHANCE_DISTANT();
				else if (eProximity == PLAYER_PROXIMITY_FAR)
					iChance += /*50*/ GC.getPERMANENT_WAR_OTHER_WARMONGER_CHANCE_FAR();
				else if (eProximity == PLAYER_PROXIMITY_CLOSE)
					iChance += /*75*/ GC.getPERMANENT_WAR_OTHER_WARMONGER_CHANCE_CLOSE();
				else if (eProximity == PLAYER_PROXIMITY_NEIGHBORS)
					iChance += /*100*/ GC.getPERMANENT_WAR_OTHER_WARMONGER_CHANCE_NEIGHBORS();
			}
			// Aggressor
			else
			{
				if (eProximity == PLAYER_PROXIMITY_DISTANT)
					iChance += /*0*/ GC.getPERMANENT_WAR_OTHER_CHANCE_DISTANT();
				else if (eProximity == PLAYER_PROXIMITY_FAR)
					iChance += /*0*/ GC.getPERMANENT_WAR_OTHER_CHANCE_FAR();
				else if (eProximity == PLAYER_PROXIMITY_CLOSE)
					iChance += /*20*/ GC.getPERMANENT_WAR_OTHER_CHANCE_CLOSE();
				else if (eProximity == PLAYER_PROXIMITY_NEIGHBORS)
					iChance += /*50*/ GC.getPERMANENT_WAR_OTHER_CHANCE_NEIGHBORS();
			}

			// If the minor is already at war, then there's a chance of it turning into permanent war
			if (GET_TEAM(pOtherMinorCiv->getTeam()).isAtWar(eEnemyTeam))
				iChance += /*50*/ GC.getPERMANENT_WAR_OTHER_AT_WAR();

			iRand = GC.getGame().getJonRandNum(100, "MINOR CIV AI: Third party minor permanent war roll.");
			if (iRand < iChance)
			{
				GET_TEAM(pOtherMinorCiv->getTeam()).declareWar(eEnemyTeam);
				pOtherMinorCiv->GetMinorCivAI()->SetPermanentWar(eEnemyTeam, true);
			}
		}
	}
}

/// Permanent War with ePlayer?
bool CvMinorCivAI::IsPermanentWar(TeamTypes eTeam) const
{
	CvAssertMsg(eTeam >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(eTeam < 0 || eTeam >= MAX_TEAMS) return false;

	return m_abPermanentWar[eTeam];
}

/// Permanent War with ePlayer?
void CvMinorCivAI::SetPermanentWar(TeamTypes eTeam, bool bValue)
{
	CvAssertMsg(eTeam >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(eTeam < 0 || eTeam >= MAX_TEAMS) return;

	m_abPermanentWar[eTeam] = bValue;
}


// ******************************
// ***** Misc Helper Functions *****
// ******************************



/// Has met another Player?
bool CvMinorCivAI::IsHasMetPlayer(PlayerTypes ePlayer)
{
	return GET_TEAM(GetPlayer()->getTeam()).isHasMet(GET_PLAYER(ePlayer).getTeam());
}

/// At war with a Player?
bool CvMinorCivAI::IsAtWarWithPlayersTeam(PlayerTypes ePlayer)
{
	return GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(GetPlayer()->getTeam());
}

/// How many resources does this minor own that eMajor doesn't?
int CvMinorCivAI::GetNumResourcesMajorLacks(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	int iNumTheyLack = 0;

	// Loop through all resources to see what this minor has
	ResourceTypes eResource;
	for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		eResource = (ResourceTypes) iResourceLoop;

		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		// Must not be a Bonus resource
		if (pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_BONUS)
			continue;

		// We must have it
		if (GetPlayer()->getNumResourceTotal(eResource, /*bIncludeImport*/ false) == 0)
			continue;

		// They must not have it
		if (GET_PLAYER(eMajor).getNumResourceTotal(eResource, /*bIncludeImport*/ false) > 0)
			continue;

		iNumTheyLack++;
	}

	return iNumTheyLack;
}

/// Helper function which returns a "Good" Tech that a Player doesn't have but CAN currently research
TechTypes CvMinorCivAI::GetGoodTechPlayerDoesntHave(PlayerTypes ePlayer, int iRoughTechValue) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	CvWeightedVector<int, SAFE_ESTIMATE_NUM_XML_WIDGETS, true> TechVector;
	int iValue, iProgress;


	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvTeam kTeam = GET_TEAM(kPlayer.getTeam());

	for (int iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
	{
		const TechTypes eTech = static_cast<TechTypes>(iTechLoop);
		CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
		if(pkTechInfo == NULL)
			continue;

		// Player doesn't already have Tech
		if (!kTeam.GetTeamTechs()->HasTech(eTech))
		{
			// Player can research this Tech
			if (kPlayer.GetPlayerTechs()->CanResearch(eTech))
			{
				iValue = pkTechInfo->GetResearchCost();

				// Reduce value of a Tech if it's already in progress
				iProgress = kTeam.GetTeamTechs()->GetResearchProgress(eTech);

				if (iProgress > 0)
				{
					iValue -= iProgress;
				}

				// Random factor so that the same thing isn't always picked
				iValue += GC.getGame().getJonRandNum(iValue / 4, "Minor Civ Quest Reward: Tech - Adding random weight to Tech Reward");

				TechVector.push_back(iTechLoop, iValue);
			}
		}
	}

	// If there's only one option return it... this will help prevent divide by zero stuff later
	if (TechVector.size() == 1)
	{
		return (TechTypes) TechVector.GetElement(0);
	}
	else if (TechVector.size() == 0)
	{
		return NO_TECH;
	}

	TechVector.SortItems();

	// Our rough estimate is that 20 is a good ceiling for the max Tech value
	if (iRoughTechValue > 20)
	{
		iRoughTechValue = 20;
	}

	int iIndex = (TechVector.size() - 1) * iRoughTechValue / 20;

	return (TechTypes) TechVector.GetElement(iIndex);
}

/// Get Scratch Pad for Major Civs
int CvMinorCivAI::GetMajorScratchPad(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0; // as defined during Reset()
	return m_aiMajorScratchPad[ePlayer];
}

/// Set Scratch Pad for Major Civs
void CvMinorCivAI::SetMajorScratchPad(PlayerTypes ePlayer, int iNum)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_aiMajorScratchPad[ePlayer] = iNum;
}

//======================================================================================================
//					CvMinorCivInfo
//======================================================================================================
CvMinorCivInfo::CvMinorCivInfo() :
m_iDefaultPlayerColor(NO_PLAYERCOLOR),
m_iArtStyleType(NO_ARTSTYLE),
m_iMinorCivTrait(NO_MINOR_CIV_TRAIT_TYPE),
m_piFlavorValue(NULL)
{
}
//------------------------------------------------------------------------------
CvMinorCivInfo::~CvMinorCivInfo()
{
	SAFE_DELETE_ARRAY(m_piFlavorValue);
}
//------------------------------------------------------------------------------
int CvMinorCivInfo::getDefaultPlayerColor() const
{
	return m_iDefaultPlayerColor;
}
//------------------------------------------------------------------------------
int CvMinorCivInfo::getArtStyleType() const
{
	return m_iArtStyleType;
}
//------------------------------------------------------------------------------
const char* CvMinorCivInfo::getArtStylePrefix() const
{
	return m_strArtStylePrefix.c_str();
}
//------------------------------------------------------------------------------
const char* CvMinorCivInfo::getArtStyleSuffix() const
{
	return m_strArtStyleSuffix.c_str();
}
//------------------------------------------------------------------------------
int CvMinorCivInfo::getNumCityNames() const
{
	return m_vCityNames.size();
}
//------------------------------------------------------------------------------
const char* CvMinorCivInfo::getShortDescription() const
{
	return m_wstrShortDescription.c_str();
}
//------------------------------------------------------------------------------
const char* CvMinorCivInfo::getShortDescriptionKey() const
{
	return m_strShortDescriptionKey.c_str();
}
//------------------------------------------------------------------------------
const char* CvMinorCivInfo::getShortDescriptionKeyWide() const
{
	return m_wstrShortDescriptionKey.c_str();
}
//------------------------------------------------------------------------------
void CvMinorCivInfo::setShortDescriptionKey(const char* szVal)
{
	m_strShortDescriptionKey = szVal;
	m_wstrShortDescriptionKey = szVal;
	m_wstrShortDescription = GetLocalizedText(m_strShortDescriptionKey.c_str());
}
//------------------------------------------------------------------------------
const char* CvMinorCivInfo::getAdjective()	const
{
	return m_wstrAdjective.c_str();
}
//------------------------------------------------------------------------------
const char* CvMinorCivInfo::getAdjectiveKey() const
{
	return m_strAdjectiveKey.c_str();
}
//------------------------------------------------------------------------------
const char* CvMinorCivInfo::getAdjectiveKeyWide() const
{
	return m_wstrAdjectiveKey.c_str();
}
//------------------------------------------------------------------------------
void CvMinorCivInfo::setAdjectiveKey(const char* szVal)
{
	m_strAdjectiveKey = szVal;
	m_wstrAdjectiveKey = szVal;
	m_wstrAdjective = GetLocalizedText(m_strAdjectiveKey.c_str());
}
//------------------------------------------------------------------------------
const char* CvMinorCivInfo::getFlagTexture() const
{
	return NULL;
}
//------------------------------------------------------------------------------
const char* CvMinorCivInfo::getArtDefineTag() const
{
	return m_strArtDefineTag;
}
//------------------------------------------------------------------------------
void CvMinorCivInfo::setArtDefineTag(const char* szVal)
{
	m_strArtDefineTag = szVal;
}
//------------------------------------------------------------------------------
int CvMinorCivInfo::GetMinorCivTrait() const
{
	return m_iMinorCivTrait;
}
//------------------------------------------------------------------------------
int CvMinorCivInfo::getFlavorValue(int i) const
{
	CvAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}
//------------------------------------------------------------------------------
const std::string& CvMinorCivInfo::getCityNames(int i) const
{
	return m_vCityNames[i];
}
//------------------------------------------------------------------------------
void CvMinorCivInfo::setArtStylePrefix(const char* szVal)
{
	m_strArtStylePrefix = szVal;
}
//------------------------------------------------------------------------------
void CvMinorCivInfo::setArtStyleSuffix(const char* szVal)
{
	m_strArtStyleSuffix = szVal;
}
//------------------------------------------------------------------------------
bool CvMinorCivInfo::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if (!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	//Basic Properties
	const char* szTextVal = NULL;

	szTextVal = kResults.GetText("ShortDescription");
	setShortDescriptionKey(szTextVal);

	szTextVal = kResults.GetText("Adjective");
	setAdjectiveKey(szTextVal);

	szTextVal = kResults.GetText("ArtDefineTag");
	setArtDefineTag(szTextVal);

	//References
	szTextVal = kResults.GetText("DefaultPlayerColor");
	m_iDefaultPlayerColor = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("ArtStyleType");
	m_iArtStyleType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("ArtStylePrefix");
	setArtStylePrefix(szTextVal);

	szTextVal = kResults.GetText("ArtStyleSuffix");
	setArtStyleSuffix(szTextVal);

	szTextVal = kResults.GetText("MinorCivTrait");
	m_iMinorCivTrait = GC.getInfoTypeForString(szTextVal, true);

	//Arrays
	const char* szType = GetType();
	kUtility.SetFlavors(m_piFlavorValue, "MinorCivilization_Flavors", "MinorCivType", szType, -1);

	//City Names
	{
		m_vCityNames.clear();

		std::string strKey = "MinorCiv - CityName";
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select CityName from MinorCivilization_CityNames where MinorCivType = ?");
		}

		pResults->Bind(1, szType, -1, false);

		while(pResults->Step())
		{
			m_vCityNames.push_back(pResults->GetText(0));
		}

		pResults->Reset();
	}

	return true;
}

FDataStream & operator<<(FDataStream & saveTo, const MinorCivStatusTypes & readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
FDataStream & operator>>(FDataStream & loadFrom, MinorCivStatusTypes & writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<MinorCivStatusTypes>(v);
	return loadFrom;
}

FDataStream & operator<<(FDataStream & saveTo, const MinorCivPersonalityTypes & readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
FDataStream & operator>>(FDataStream & loadFrom, MinorCivPersonalityTypes & writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<MinorCivPersonalityTypes>(v);
	return loadFrom;
}

FDataStream & operator<<(FDataStream & saveTo, const MinorCivQuestTypes & readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
FDataStream & operator>>(FDataStream & loadFrom, MinorCivQuestTypes & writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<MinorCivQuestTypes>(v);
	return loadFrom;
}
