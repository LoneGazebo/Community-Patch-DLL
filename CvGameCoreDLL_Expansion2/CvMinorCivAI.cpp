/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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
#include "CvDllPlot.h"
#include "cvStopWatch.h"
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
#include "CvEconomicAI.h"
#include "CvMilitaryAI.h"
#endif

// must be included after all other headers
#include "LintFree.h"

//======================================================================================================
//					CvMinorCivQuest
//======================================================================================================

// Default Constructor
CvMinorCivQuest::CvMinorCivQuest()
{
	m_eMinor = NO_PLAYER;
	m_eAssignedPlayer = NO_PLAYER;
	m_eType = NO_MINOR_CIV_QUEST_TYPE;
	m_iStartTurn = NO_TURN; /* -1 */
	m_iData1 = NO_QUEST_DATA; /* -1 */
	m_iData2 = NO_QUEST_DATA; /* -1 */
	m_bHandled = false;
}

// Constructor
CvMinorCivQuest::CvMinorCivQuest(PlayerTypes eMinor, PlayerTypes eAssignedPlayer, MinorCivQuestTypes eType)
{
	m_eMinor = eMinor;
	m_eAssignedPlayer = eAssignedPlayer;
	m_eType = eType;
	m_iStartTurn = NO_TURN; /* -1 */
	m_iData1 = NO_QUEST_DATA; /* -1 */
	m_iData2 = NO_QUEST_DATA; /* -1 */
	m_bHandled = false;
}

CvMinorCivQuest::~CvMinorCivQuest()
{
}

// The minor that gave this quest
PlayerTypes CvMinorCivQuest::GetMinor() const
{
	return m_eMinor;
}

// The player the quest was given to
PlayerTypes CvMinorCivQuest::GetPlayerAssignedTo() const
{
	return m_eAssignedPlayer;
}

// Type of quest
MinorCivQuestTypes CvMinorCivQuest::GetType() const
{
	return m_eType;
}

// Turn that quest began
int CvMinorCivQuest::GetStartTurn() const
{
	return m_iStartTurn;
}

// Turn that quest ends, based on start turn.  Some quests have no specified end turn.
int CvMinorCivQuest::GetEndTurn() const
{
	CvAssertMsg(m_iStartTurn != NO_TURN, "GetEndTurn called for a quest, but the start turn was not initialized!");

	int iLength = 0;

	if(m_eType == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{
		iLength = GC.getMINOR_QUEST_STANDARD_CONTEST_LENGTH();
	}

	else if(m_eType == MINOR_CIV_QUEST_CONTEST_FAITH)
	{
		iLength = GC.getMINOR_QUEST_STANDARD_CONTEST_LENGTH();
	}

	else if(m_eType == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		iLength = GC.getMINOR_QUEST_STANDARD_CONTEST_LENGTH();
	}

	else if(m_eType == MINOR_CIV_QUEST_INVEST)
	{
		iLength = GC.getMINOR_QUEST_STANDARD_CONTEST_LENGTH();
	}

	else if(m_eType == MINOR_CIV_QUEST_GIVE_GOLD)
	{
		iLength = GC.getMINOR_QUEST_STANDARD_CONTEST_LENGTH();
	}

	else if(m_eType == MINOR_CIV_QUEST_PLEDGE_TO_PROTECT)
	{
		iLength = GC.getMINOR_QUEST_STANDARD_CONTEST_LENGTH();
	}

	else if(m_eType == MINOR_CIV_QUEST_BULLY_CITY_STATE)
	{
		iLength = GC.getMINOR_QUEST_STANDARD_CONTEST_LENGTH();
	}

	else if(m_eType == MINOR_CIV_QUEST_DENOUNCE_MAJOR)
	{
		iLength = GC.getMINOR_QUEST_STANDARD_CONTEST_LENGTH();
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	else if(m_eType == MINOR_CIV_QUEST_INFLUENCE)
	{
		iLength = GC.getMINOR_QUEST_STANDARD_CONTEST_LENGTH();
	}
	else if(m_eType == MINOR_CIV_QUEST_CONTEST_TOURISM)
	{
		iLength = GC.getMINOR_QUEST_STANDARD_CONTEST_LENGTH();
	}
	else if(m_eType == MINOR_CIV_QUEST_HORDE)
	{
		//20 turns - not much!
		iLength = (GC.getMINOR_QUEST_REBELLION_TIMER());
	}
	else if(m_eType == MINOR_CIV_QUEST_REBELLION)
	{
		//20 turns - not much!
		iLength = (GC.getMINOR_QUEST_REBELLION_TIMER());
	}
#endif

	// Other quests are not time-sensitive
	else
	{
		return NO_TURN;
	}

	// Modify for Game Speed
	iLength *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iLength /= 100;

	return m_iStartTurn + iLength;
}

// How many turns left until quest is over?
int CvMinorCivQuest::GetTurnsRemaining(int iCurrentTurn) const
{
	int iStartTurn = GetStartTurn();
	int iEndTurn = GetEndTurn();

	if (iStartTurn == NO_TURN)
		return NO_TURN;

	if (iEndTurn == NO_TURN)
		return NO_TURN;

	return (iEndTurn - iCurrentTurn);
}

// Quest-specific data 1
int CvMinorCivQuest::GetPrimaryData() const
{
	return m_iData1;
}

// Quest-specific data 2
int CvMinorCivQuest::GetSecondaryData() const
{
	return m_iData2;
}

// Influence gained if the quest is completed
int CvMinorCivQuest::GetInfluenceReward() const
{
	int iReward = 0;

	switch(m_eType)
	{
	case MINOR_CIV_QUEST_ROUTE:
		iReward = /*50*/ GC.getMINOR_QUEST_FRIENDSHIP_ROUTE();
		break;
	case MINOR_CIV_QUEST_KILL_CAMP:
		iReward = /*50*/ GC.getMINOR_QUEST_FRIENDSHIP_KILL_CAMP();
		break;
	case MINOR_CIV_QUEST_CONNECT_RESOURCE:
		iReward = /*40*/ GC.getMINOR_QUEST_FRIENDSHIP_CONNECT_RESOURCE();
		break;
	case MINOR_CIV_QUEST_CONSTRUCT_WONDER:
		iReward = /*40*/ GC.getMINOR_QUEST_FRIENDSHIP_CONSTRUCT_WONDER();
		break;
	case MINOR_CIV_QUEST_GREAT_PERSON:
		iReward = /*40*/ GC.getMINOR_QUEST_FRIENDSHIP_GREAT_PERSON();
		break;
	case MINOR_CIV_QUEST_KILL_CITY_STATE:
		iReward = /*80*/ GC.getMINOR_QUEST_FRIENDSHIP_KILL_CITY_STATE();
		break;
	case MINOR_CIV_QUEST_FIND_PLAYER:
		iReward = /*35*/ GC.getMINOR_QUEST_FRIENDSHIP_FIND_PLAYER();
		break;
	case MINOR_CIV_QUEST_FIND_NATURAL_WONDER:
		iReward = /*40*/ GC.getMINOR_QUEST_FRIENDSHIP_FIND_NATURAL_WONDER();
		break;
	case MINOR_CIV_QUEST_GIVE_GOLD:
		iReward = /*20*/ GC.getMINOR_QUEST_FRIENDSHIP_GIVE_GOLD();
		break;
	case MINOR_CIV_QUEST_PLEDGE_TO_PROTECT:
		iReward = /*20*/ GC.getMINOR_QUEST_FRIENDSHIP_PLEDGE_TO_PROTECT();
		break;
	case MINOR_CIV_QUEST_CONTEST_CULTURE:
		iReward = /*40*/ GC.getMINOR_QUEST_FRIENDSHIP_CONTEST_CULTURE();
		break;
	case MINOR_CIV_QUEST_CONTEST_FAITH:
		iReward = /*40*/ GC.getMINOR_QUEST_FRIENDSHIP_CONTEST_FAITH();
		break;
	case MINOR_CIV_QUEST_CONTEST_TECHS:
		iReward = /*40*/ GC.getMINOR_QUEST_FRIENDSHIP_CONTEST_TECHS();
		break;
	case MINOR_CIV_QUEST_INVEST:
		// Reward is indirect; increased gains from gold gifts
		iReward = /*0*/ GC.getMINOR_QUEST_FRIENDSHIP_INVEST();
		break;
	case MINOR_CIV_QUEST_BULLY_CITY_STATE:
		iReward = /*40*/ GC.getMINOR_QUEST_FRIENDSHIP_BULLY_CITY_STATE();
		break;
	case MINOR_CIV_QUEST_DENOUNCE_MAJOR:
		iReward = /*40*/ GC.getMINOR_QUEST_FRIENDSHIP_DENOUNCE_MAJOR();
		break;
	case MINOR_CIV_QUEST_SPREAD_RELIGION:
		iReward = /*40*/ GC.getMINOR_QUEST_FRIENDSHIP_SPREAD_RELIGION();
		break;
	case MINOR_CIV_QUEST_TRADE_ROUTE:
		iReward = /*40*/ GC.getMINOR_QUEST_FRIENDSHIP_TRADE_ROUTE();
		break;
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	case MINOR_CIV_QUEST_WAR:
		iReward = /*50*/ GC.getMINOR_QUEST_FRIENDSHIP_WAR();
		break;
	case MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER:
		iReward = /*40*/ GC.getMINOR_QUEST_FRIENDSHIP_CONSTRUCT_WONDER();
		break;
	case MINOR_CIV_QUEST_FIND_CITY_STATE:
		iReward = /*40*/ GC.getMINOR_QUEST_FRIENDSHIP_FIND_CITY_STATE();
		break;
	case MINOR_CIV_QUEST_INFLUENCE:
		// Reward is indirect; increased gains from gold gifts
		iReward = /*0*/ GC.getMINOR_QUEST_FRIENDSHIP_INVEST();
		break;
	case MINOR_CIV_QUEST_CONTEST_TOURISM:
		iReward = /*40*/ GC.getMINOR_QUEST_FRIENDSHIP_TOURISM();
		break;
	case MINOR_CIV_QUEST_ARCHAEOLOGY:
		iReward = /*50*/ GC.getMINOR_QUEST_FRIENDSHIP_ARCHAEOLOGY();
		break;
	case MINOR_CIV_QUEST_CIRCUMNAVIGATION:
		iReward = /*50*/ GC.getMINOR_QUEST_FRIENDSHIP_CIRCUMNAVIGATION();
		break;
	case MINOR_CIV_QUEST_LIBERATION:
		iReward = /*80*/ GC.getMINOR_QUEST_FRIENDSHIP_KILL_CITY_STATE();
		break;
	case MINOR_CIV_QUEST_HORDE:
		iReward = /*0*/ GC.getMINOR_QUEST_FRIENDSHIP_INVEST();
		break;
	case MINOR_CIV_QUEST_REBELLION:
		iReward = /*0*/ GC.getMINOR_QUEST_FRIENDSHIP_INVEST();
		break;
#endif
	default:
		iReward = 0;
		break;
	}

	return iReward;
}

// Assumes that only one contest of a particular Type is active from this minor at any given time.
// Otherwise, the results across different spans of contest will be calculated together here.
int CvMinorCivQuest::GetContestValueForPlayer(PlayerTypes ePlayer)
{
	MinorCivQuestTypes eType = GetType();
	int iValue = -1;
	CvPlayer* pMinor = &GET_PLAYER(GetMinor());
	if(!pMinor)
	{
		CvAssertMsg(false, "Error when calculating contest standings - could not find minor civ! Please send Anton your save file and version.");
		return iValue;
	}

	// Player must actually be in the contest to have a score!
	if(!pMinor->GetMinorCivAI()->IsActiveQuestForPlayer(ePlayer, eType))
		return iValue;

	if(eType == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{
		int iStartCulture = pMinor->GetMinorCivAI()->GetQuestData1(ePlayer, eType);
		int iEndCulture = GET_PLAYER(ePlayer).GetJONSCultureEverGenerated();
		iValue = iEndCulture - iStartCulture;
	}
	else if(eType == MINOR_CIV_QUEST_CONTEST_FAITH)
	{
		int iStartFaith = pMinor->GetMinorCivAI()->GetQuestData1(ePlayer, eType);
		int iEndFaith = GET_PLAYER(ePlayer).GetFaithEverGenerated();
		iValue = iEndFaith - iStartFaith;
	}
	else if(eType == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		int iStartTechs = pMinor->GetMinorCivAI()->GetQuestData1(ePlayer, eType);
		int iEndTechs = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetTeamTechs()->GetNumTechsKnown();
		iValue = iEndTechs - iStartTechs;
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	else if(eType == MINOR_CIV_QUEST_CONTEST_TOURISM)
	{
		iValue = GET_PLAYER(ePlayer).GetCulture()->GetTourism();
	}
#endif

	return iValue;
}

int CvMinorCivQuest::GetContestValueForLeader()
{
	MinorCivQuestTypes eType = GetType();
	int iHighestValue = -1;

	if(eType == MINOR_CIV_QUEST_CONTEST_CULTURE ||
	        eType == MINOR_CIV_QUEST_CONTEST_FAITH ||
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	        eType == MINOR_CIV_QUEST_CONTEST_TOURISM ||
#endif
	        eType == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		// What is the largest value a participant has for this contest?
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes ePlayerLoop = (PlayerTypes) iPlayerLoop;

			int iPlayerValue = GetContestValueForPlayer(ePlayerLoop);
			if(iPlayerValue > iHighestValue)
			{
				iHighestValue = iPlayerValue;
			}
		}
	}

	return iHighestValue;
}

// Returns a list, since ties are allowed
CivsList CvMinorCivQuest::GetContestLeaders()
{
	MinorCivQuestTypes eType = GetType();
	CivsList veTiedForLead;
	int iHighestValue = GetContestValueForLeader();

	if(eType == MINOR_CIV_QUEST_CONTEST_CULTURE ||
	        eType == MINOR_CIV_QUEST_CONTEST_FAITH ||
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
			eType == MINOR_CIV_QUEST_CONTEST_TOURISM ||
#endif
	        eType == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes ePlayerLoop = (PlayerTypes) iPlayerLoop;
			int iPlayerValue = GetContestValueForPlayer(ePlayerLoop);
			CvAssertMsg(iPlayerValue <= iHighestValue, "Calculation error for calculating the leaders of a contest! Please send Anton your save file and version.");
			if(iPlayerValue == iHighestValue)
				veTiedForLead.push_back(ePlayerLoop);
		}
	}

	return veTiedForLead;
}

// Is this player currently the contest leader?  (if no ePlayer is passed in, checks the player this instance of the quest is assigned to)
bool CvMinorCivQuest::IsContestLeader(PlayerTypes ePlayer)
{
	if(ePlayer == NO_PLAYER)
		ePlayer = GetPlayerAssignedTo();

	MinorCivQuestTypes eType = GetType();
	CvPlayer* pMinor = &GET_PLAYER(GetMinor());
	if(!pMinor)
	{
		CvAssertMsg(false, "Error when calculating contest standings - could not find minor civ! Please send Anton your save file and version.");
		return false;
	}
	// Player must actually be in the contest!
	if(!pMinor->GetMinorCivAI()->IsActiveQuestForPlayer(ePlayer, eType))
		return false;

	if(eType == MINOR_CIV_QUEST_CONTEST_CULTURE ||
	        eType == MINOR_CIV_QUEST_CONTEST_FAITH ||
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
			eType == MINOR_CIV_QUEST_CONTEST_TOURISM ||
#endif
	        eType == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		CivsList veTiedForLead = GetContestLeaders();
		for(uint ui = 0; ui < veTiedForLead.size(); ui++)
		{
			if(ePlayer == veTiedForLead[ui])
			{
				return true;
			}
		}
	}

	return false;
}


// Checks if the conditions for completing the quest are met.  Does not actually complete the quest.
bool CvMinorCivQuest::IsComplete()
{
	CvPlayer* pMinor = &GET_PLAYER(m_eMinor);
	CvPlayer* pAssignedPlayer = &GET_PLAYER(m_eAssignedPlayer);

	if(!pMinor)
	{
		CvAssertMsg(false, "Minor player not found! Please send Anton your save file and version.");
		return false;
	}
	if(!pAssignedPlayer)
	{
		CvAssertMsg(false, "Assigned player not found! Please send Anton your save file and version.");
		return false;
	}

	if(m_eType == MINOR_CIV_QUEST_ROUTE)
	{
		if (pAssignedPlayer->IsCapitalConnectedToPlayer(m_eMinor))
		{
			return true;
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_KILL_CAMP)
	{
		int iX = m_iData1;
		int iY = m_iData2;
		CvPlot* pPlot = GC.getMap().plot(iX, iY);

		if(pPlot)
		{
			// No longer a camp here?
			if(pPlot->getImprovementType() != GC.getBARBARIAN_CAMP_IMPROVEMENT())
			{
				// Did this guy clear it?
				if(pPlot->GetPlayerThatClearedBarbCampHere() == m_eAssignedPlayer)
				{
					return true;
				}
			}
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_CONNECT_RESOURCE)
	{
		ResourceTypes eResource = (ResourceTypes) m_iData1;

		// Player has the Resource?
		if(pAssignedPlayer->getNumResourceTotal(eResource, /*bIncludeImport*/ true) > 0)
		{
			return true;
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
	{
		BuildingTypes eWonder = (BuildingTypes) m_iData1;

		// Player built the Wonder?
		if(pAssignedPlayer->countNumBuildings(eWonder) > 0)
		{
			return true;
		}
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	else if(m_eType == MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER)
	{
		BuildingTypes eNationalWonder = (BuildingTypes) m_iData1;

		// Player built the Wonder?
		if(pAssignedPlayer->countNumBuildings(eNationalWonder) > 0)
		{
			return true;
		}
	}
#endif
	else if(m_eType == MINOR_CIV_QUEST_GREAT_PERSON)
	{
		UnitTypes eUnit = (UnitTypes) m_iData1;

		// Player has the Great Person?
		int iLoop;
		for(CvUnit* pLoopUnit = pAssignedPlayer->firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = pAssignedPlayer->nextUnit(&iLoop))
		{
			if(pLoopUnit->getUnitType() == eUnit)
			{
				return true;
			}
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_KILL_CITY_STATE)
	{
		PlayerTypes eTargetCityState = (PlayerTypes) m_iData1;
		CvPlayer* pTargetCityState = &GET_PLAYER(eTargetCityState);
		if(pTargetCityState)
		{
			CvTeam* pTargetCityStateTeam = &GET_TEAM(pTargetCityState->getTeam());
			if(pTargetCityStateTeam)
			{
				// Player killed the City State?
				if(pTargetCityStateTeam->GetKilledByTeam() == pAssignedPlayer->getTeam())
				{
					return true;
				}
			}
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_FIND_PLAYER)
	{
		PlayerTypes ePlayerToFind = (PlayerTypes) m_iData1;

		// Player found the target player?
		if(GET_TEAM(pAssignedPlayer->getTeam()).IsHasFoundPlayersTerritory(ePlayerToFind))
		{
			return true;
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_FIND_NATURAL_WONDER)
	{
		int iNumWondersFoundBefore = m_iData1;

		// Player found a new NW?
		if(GET_TEAM(pAssignedPlayer->getTeam()).GetNumNaturalWondersDiscovered() > iNumWondersFoundBefore)
		{
			return true;
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_GIVE_GOLD)
	{
		int iGoldGiftedBefore = m_iData2;

		// Has the player given gold since the quest began?
		if(pMinor->GetMinorCivAI()->GetNumGoldGifted(m_eAssignedPlayer) > iGoldGiftedBefore)
		{
			return true;
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_PLEDGE_TO_PROTECT)
	{
		// Has the player pledged?
		if(pMinor->GetMinorCivAI()->IsProtectedByMajor(m_eAssignedPlayer))
		{
			return true;
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{
		// Is it time to compare the score?
		if(GetEndTurn() == GC.getGame().getGameTurn())
			if(IsContestLeader(GetPlayerAssignedTo()))
				return true;
	}
	else if(m_eType == MINOR_CIV_QUEST_CONTEST_FAITH)
	{
		// Is it time to compare the score?
		if(GetEndTurn() == GC.getGame().getGameTurn())
			if(IsContestLeader(GetPlayerAssignedTo()))
				return true;
	}
	else if(m_eType == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		// Is it time to compare the score?
		if(GetEndTurn() == GC.getGame().getGameTurn())
			if(IsContestLeader(GetPlayerAssignedTo()))
				return true;
	}
	else if(m_eType == MINOR_CIV_QUEST_INVEST)
	{
		if(GetEndTurn() == GC.getGame().getGameTurn())
			return true;
	}
	else if(m_eType == MINOR_CIV_QUEST_BULLY_CITY_STATE)
	{
		// Has the player bullied since the quest began?
		PlayerTypes eTargetMinor = (PlayerTypes) m_iData1;
		CvPlayer* pTargetMinor = &GET_PLAYER(eTargetMinor);
		if(pTargetMinor)
		{
			int iMostRecentBullyTurn = pTargetMinor->GetMinorCivAI()->GetTurnLastBulliedByMajor(m_eAssignedPlayer);
			if(iMostRecentBullyTurn > m_iData2)
				return true;
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_DENOUNCE_MAJOR)
	{
		// Has the player denounced that major yet?
		PlayerTypes eTargetMajor = (PlayerTypes) m_iData1;
		CvPlayer* pTargetMajor = &GET_PLAYER(eTargetMajor);
		if(pTargetMajor)
		{
			if(pAssignedPlayer->GetDiplomacyAI()->IsDenouncedPlayer(eTargetMajor))
				return true;
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_SPREAD_RELIGION)
	{
		// Does the CS have the right majority religion?
		ReligionTypes eReligion = (ReligionTypes) m_iData1;
		if(pMinor->getCapitalCity())
		{
			if(pMinor->getCapitalCity()->GetCityReligions()->GetReligiousMajority() == eReligion)
				return true;
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_TRADE_ROUTE)
	{
		if(GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(m_eMinor,m_eAssignedPlayer))
			return true;
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	else if(m_eType == MINOR_CIV_QUEST_WAR)
	{
		// Has the player gone to war with that major yet?
		PlayerTypes eTargetMajor = (PlayerTypes) m_iData1;
		CvTeam* pTeam = &GET_TEAM(GET_PLAYER(m_eAssignedPlayer).getTeam());
		TeamTypes eTargetTeam;
		eTargetTeam = GET_PLAYER(eTargetMajor).getTeam();

		if(pTeam->isAtWar(eTargetTeam))
		{
			return true;		
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_FIND_CITY_STATE)
	{
		PlayerTypes eTargetMinor = (PlayerTypes) m_iData1;
		CvTeam* pTeam = &GET_TEAM(GET_PLAYER(m_eAssignedPlayer).getTeam());
		TeamTypes eTargetTeam = GET_PLAYER(eTargetMinor).getTeam();

		// Player found the target minor?
		if(pTeam->isHasMet(eTargetTeam))
		{
			return true;
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_INFLUENCE)
	{
		if(GetEndTurn() == GC.getGame().getGameTurn())
			return true;
	}
	else if(m_eType == MINOR_CIV_QUEST_CONTEST_TOURISM)
	{
		//Time to see who has the most Tourism!
		if(GetEndTurn() == GC.getGame().getGameTurn())
			if(IsContestLeader(GetPlayerAssignedTo()))
				return true;
	}
	else if(m_eType == MINOR_CIV_QUEST_ARCHAEOLOGY)
	{
		int iX = m_iData1;
		int iY = m_iData2;
		CvPlot* pPlot = GC.getMap().plot(iX, iY);

		if(pPlot)
		{
			// No longer a dig here?
			if(pPlot->getResourceType() != GC.getARTIFACT_RESOURCE())
			{
				// Did this guy clear it?
				if(pPlot->GetPlayerThatClearedDigHere() == m_eAssignedPlayer)
				{
					return true;
				}
			}
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_CIRCUMNAVIGATION)
	{
		// Player team circumnavigated the world?
		if(GC.getGame().GetTeamThatCircumnavigated() == GET_PLAYER(m_eAssignedPlayer).getTeam())
		{
			return true;
		}
	}
	else if(m_eType == MINOR_CIV_QUEST_LIBERATION)
	{
		PlayerTypes eTargetCityState = (PlayerTypes) m_iData1;
		CvPlayer* pTargetCityState = &GET_PLAYER(eTargetCityState);
		if(pTargetCityState)
		{
			CvTeam* pTargetCityStateTeam = &GET_TEAM(pTargetCityState->getTeam());
			if(pTargetCityStateTeam)
			{
				// Player liberated the City State?
				if(pTargetCityStateTeam->GetLiberatedByTeam() == pAssignedPlayer->getTeam())
				{
					return true;
				}
			}
		}
	}
	//No barbarians in the city-state, so you win!
	else if(m_eType == MINOR_CIV_QUEST_HORDE)
	{
		if((GetEndTurn() == GC.getGame().getGameTurn()) && pMinor->GetMinorCivAI()->GetNumThreateningBarbarians() <= 0)
			return true;
	}
	//You eliminated all of the rebels.
	else if(m_eType == MINOR_CIV_QUEST_REBELLION)
	{
		if((GetEndTurn() == GC.getGame().getGameTurn()) && (pMinor->GetMinorCivAI()->GetNumThreateningBarbarians() <= 0))
			return true;
	}
#endif

	return false;
}

/// Is this quest now revoked (ie. because the player bullied us)?
bool CvMinorCivQuest::IsRevoked()
{
	if(GET_PLAYER(m_eMinor).GetMinorCivAI()->IsRecentlyBulliedByMajor(m_eAssignedPlayer))
	{
		if(m_eType == MINOR_CIV_QUEST_ROUTE)
			return true;
		if(m_eType == MINOR_CIV_QUEST_CONNECT_RESOURCE)
			return true;
		if(m_eType == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
			return true;
		if(m_eType == MINOR_CIV_QUEST_GREAT_PERSON)
			return true;
		if(m_eType == MINOR_CIV_QUEST_FIND_PLAYER)
			return true;
		if(m_eType == MINOR_CIV_QUEST_FIND_NATURAL_WONDER)
			return true;
		if(m_eType == MINOR_CIV_QUEST_GIVE_GOLD)
			return true;
		if(m_eType == MINOR_CIV_QUEST_PLEDGE_TO_PROTECT)
			return true;
		if(m_eType == MINOR_CIV_QUEST_INVEST)
			return true;
		if(m_eType == MINOR_CIV_QUEST_BULLY_CITY_STATE)
			return true;
		if(m_eType == MINOR_CIV_QUEST_DENOUNCE_MAJOR)
			return true;
		if(m_eType == MINOR_CIV_QUEST_SPREAD_RELIGION)
			return true;
		if(m_eType == MINOR_CIV_QUEST_TRADE_ROUTE)
			return true;
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
		if(m_eType == MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER)
			return true;
		if(m_eType == MINOR_CIV_QUEST_WAR)
			return true;
		if(m_eType == MINOR_CIV_QUEST_FIND_CITY_STATE)
			return true;
		if(m_eType == MINOR_CIV_QUEST_INFLUENCE)
			return true;
		if(m_eType == MINOR_CIV_QUEST_CIRCUMNAVIGATION)
			return true;
#endif
	}
	
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	if (MOD_DIPLOMACY_CITYSTATES_QUESTS) {
		//No longer allies?
		PlayerTypes eAlly = GET_PLAYER(m_eMinor).GetMinorCivAI()->GetAlly();
		if(eAlly != NO_PLAYER)
		{
			if(!GET_PLAYER(GetMinor()).GetMinorCivAI()->IsAllies(eAlly))
			{
				if(m_eType == MINOR_CIV_QUEST_REBELLION)
					return true;
			}
		}
	}
#endif

	return false;
}

/// Is this quest now expired (ie. time limit is up or condition is no longer valid)?
bool CvMinorCivQuest::IsExpired()
{
	// If this quest type has an end turn, have we passed it?
	if(GetEndTurn() != NO_TURN && GC.getGame().getGameTurn() > GetEndTurn())
	{
		return true;
	}

	// Build a Route
	if(m_eType == MINOR_CIV_QUEST_ROUTE)
	{
	}

	// City-state wanted us to clear a camp
	if(m_eType == MINOR_CIV_QUEST_KILL_CAMP)
	{
		int iX = GetPrimaryData();
		int iY = GetSecondaryData();
		CvPlot* pPlot = GC.getMap().plot(iX, iY);

		if(pPlot)
		{
			// Camp that was here is gone
			if(!pPlot->HasBarbarianCamp())
			{
				// Someone cleared it, and it wasn't us
				if(pPlot->GetPlayerThatClearedBarbCampHere() != NO_PLAYER && pPlot->GetPlayerThatClearedBarbCampHere() != m_eAssignedPlayer)
				{
					return true;
				}
			}
		}
	}

	// Connect a resource
	if(m_eType == MINOR_CIV_QUEST_CONNECT_RESOURCE)
	{
	}

	// CONSTRUCT A WONDER
	else if(m_eType == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
	{
		BuildingTypes eWonder = (BuildingTypes) GetPrimaryData();
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eWonder);
		CvAssertMsg(pkBuildingInfo, "Building info not expected to be FALSE! Please send Anton your save file and version.");
		if(!pkBuildingInfo) return false;

		for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			const PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
			CvPlayer* pLoopPlayer = &GET_PLAYER(eLoopPlayer);

			if(pLoopPlayer)
			{
				// Someone built the wonder, and it wasn't us
				if(m_eAssignedPlayer != eLoopPlayer && pLoopPlayer->countNumBuildings(eWonder) > 0)
				{
					return true;
				}
			}
		}
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	// City-state wanted us to dig
	else if(m_eType == MINOR_CIV_QUEST_ARCHAEOLOGY)
	{
		int iX = GetPrimaryData();
		int iY = GetSecondaryData();
		CvPlot* pPlot = GC.getMap().plot(iX, iY);

		if(pPlot)
		{
			// Dig that was here is gone
			if(!pPlot->HasDig())
			{
				// Someone dug it, and it wasn't us
				if(pPlot->GetPlayerThatClearedDigHere() != NO_PLAYER && pPlot->GetPlayerThatClearedDigHere() != m_eAssignedPlayer)
				{
					return true;
				}
			}
		}
	}
#endif

	// Spawn a Great Person
	else if(m_eType == MINOR_CIV_QUEST_GREAT_PERSON)
	{
	}

	// KILL ANOTHER CITY STATE
	else if(m_eType == MINOR_CIV_QUEST_KILL_CITY_STATE)
	{
		PlayerTypes eTargetCityState = (PlayerTypes) GetPrimaryData();
		CvPlayer* pTargetCityState = &GET_PLAYER(eTargetCityState);

		if(pTargetCityState)
		{
			CvTeam* pTargetCityStateTeam = &GET_TEAM(pTargetCityState->getTeam());
			if(pTargetCityStateTeam)
			{
				// Someone killed the City State, and it wasn't us
				if(!pTargetCityState->isAlive() && pTargetCityStateTeam->GetKilledByTeam() != GET_PLAYER(m_eAssignedPlayer).getTeam())
				{
					return true;
				}
			}
		}
	}

	// Find a Player
	else if(m_eType == MINOR_CIV_QUEST_FIND_PLAYER)
	{
		PlayerTypes eTargetPlayer = (PlayerTypes) GetPrimaryData();
		CvPlayer* pTargetPlayer = &GET_PLAYER(eTargetPlayer);

		if(pTargetPlayer)
		{
			// Someone killed the Player
			if(!GET_PLAYER(eTargetPlayer).isAlive())
				return true;
		}
	}

	// Find a Natural Wonder
	else if(m_eType == MINOR_CIV_QUEST_FIND_NATURAL_WONDER)
	{
	}

	// Give Gold
	else if(m_eType == MINOR_CIV_QUEST_GIVE_GOLD)
	{
	}

	// Pledge to Protect
	else if(m_eType == MINOR_CIV_QUEST_PLEDGE_TO_PROTECT)
	{
	}

	// Contest Culture
	else if(m_eType == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{
		if(GC.getGame().getGameTurn() == GetEndTurn() && !IsComplete())
			return true;
	}

	// Contest Faith
	else if(m_eType == MINOR_CIV_QUEST_CONTEST_FAITH)
	{
		if(GC.getGame().getGameTurn() == GetEndTurn() && !IsComplete())
			return true;
	}

	// Contest Techs
	else if(m_eType == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		if(GC.getGame().getGameTurn() == GetEndTurn() && !IsComplete())
			return true;
	}

	// Invest
	else if(m_eType == MINOR_CIV_QUEST_INVEST)
	{
	}

	// Bully target City-State
	else if(m_eType == MINOR_CIV_QUEST_BULLY_CITY_STATE)
	{
		PlayerTypes eTargetCityState = (PlayerTypes) GetPrimaryData();
		CvPlayer* pTargetCityState = &GET_PLAYER(eTargetCityState);

		if(pTargetCityState)
		{
			// Someone killed the City State...ouch
			if(!pTargetCityState->isAlive())
				return true;
		}
	}

	// Denounce target Major
	else if(m_eType == MINOR_CIV_QUEST_DENOUNCE_MAJOR)
	{
		PlayerTypes eTargetPlayer = (PlayerTypes) GetPrimaryData();
		CvPlayer* pTargetPlayer = &GET_PLAYER(eTargetPlayer);

		if(pTargetPlayer)
		{
			// Someone killed the Major
			if(!pTargetPlayer->isAlive())
				return true;
		}
	}

	// Spread your religion to us
	else if(m_eType == MINOR_CIV_QUEST_SPREAD_RELIGION)
	{
		//antonjs: consider: if holy city is lost
	}
	
	// Trade Route
	else if(m_eType == MINOR_CIV_QUEST_TRADE_ROUTE)
	{
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	// War on other Major
	else if(m_eType == MINOR_CIV_QUEST_WAR)
	{
		PlayerTypes eTargetPlayer = (PlayerTypes) GetPrimaryData();
		CvPlayer* pTargetPlayer = &GET_PLAYER(eTargetPlayer);

		if(pTargetPlayer)
		{
			// Someone killed the Major
			if(!pTargetPlayer->isAlive())
				return true;
		}
	}
	// FIND ANOTHER CITY STATE
	else if(m_eType == MINOR_CIV_QUEST_FIND_CITY_STATE)
	{
		PlayerTypes eTargetCityState = (PlayerTypes) GetPrimaryData();
		CvPlayer* pTargetCityState = &GET_PLAYER(eTargetCityState);

		if(pTargetCityState)
		{
			CvTeam* pTargetCityStateTeam = &GET_TEAM(pTargetCityState->getTeam());
			if(pTargetCityStateTeam)
			{
				// Someone killed the City State, and it wasn't us
				if(!pTargetCityState->isAlive() && pTargetCityStateTeam->GetKilledByTeam() != GET_PLAYER(m_eAssignedPlayer).getTeam())
				{
					return true;
				}
			}
		}
	}
	// Influence
	else if(m_eType == MINOR_CIV_QUEST_INFLUENCE)
	{
		if(GC.getGame().getGameTurn() == GetEndTurn() && !IsComplete())
			return true;
	}
	// Tourism
	else if(m_eType == MINOR_CIV_QUEST_CONTEST_TOURISM)
	{
		if(GC.getGame().getGameTurn() == GetEndTurn() && !IsComplete())
			return true;
	}
	// Circumnavigation
	else if(m_eType == MINOR_CIV_QUEST_CIRCUMNAVIGATION)
	{
		// Another player circumnavigated the world?
		if((GC.getGame().GetTeamThatCircumnavigated() != NO_TEAM) && (GC.getGame().GetTeamThatCircumnavigated() != GET_PLAYER(m_eAssignedPlayer).getTeam()))
		{
			return true;
		}
	}
	// LIBERATE A CITY STATE
	else if(m_eType == MINOR_CIV_QUEST_LIBERATION)
	{
		PlayerTypes eTargetCityState = (PlayerTypes) GetPrimaryData();
		CvPlayer* pTargetCityState = &GET_PLAYER(eTargetCityState);
		TeamTypes eLiberatedTeam = GET_PLAYER(eTargetCityState).getTeam();
		CvPlot* pPlot = pTargetCityState->getStartingPlot();
		PlayerTypes eFirstConqueror = pTargetCityState->GetCapitalConqueror();
		// Who liberated them?
		TeamTypes eLiberatorTeam = GET_TEAM(eLiberatedTeam).GetLiberatedByTeam();


		if(pTargetCityState)
		{
			// Someone liberated the City State, and it wasn't us
			if(pTargetCityState->isAlive() && (eLiberatorTeam != GET_PLAYER(m_eAssignedPlayer).getTeam()))
			{
				return true;
			}
			// We conquered the city-state. Oops.
			if(!pTargetCityState->isAlive() && (pPlot->getOwner() == m_eAssignedPlayer))
			{
				return true;
			}
			// We can't liberate this city-state for some reason.
			if(!pTargetCityState->isAlive() && !GET_PLAYER(m_eAssignedPlayer).CanLiberatePlayerCity(eTargetCityState))
			{
				return true;
			}
			//Someone else conquered the conquerors - things just got weird. Let's cancel this quest.
			if(!pTargetCityState->isAlive() && (pPlot->getOwner() != eFirstConqueror))
			{
				return true;
			}
		}
	}
	// The Horde is still in the City-State's threat-radius - oh no!
	else if(m_eType == MINOR_CIV_QUEST_HORDE)
	{
		if((GC.getGame().getGameTurn() == GetEndTurn()) && (GET_PLAYER(GetMinor()).GetMinorCivAI()->GetNumThreateningBarbarians() > 0))
		{
			return true;
		}
	}
	//Are there still rebels milling about? You lose!
	else if(m_eType == MINOR_CIV_QUEST_REBELLION)
	{
		if((GC.getGame().getGameTurn() == GetEndTurn()) && ((GET_PLAYER(GetMinor()).GetMinorCivAI()->GetNumThreateningBarbarians() > 0)))
		{
			return true;
		}
	}
#endif

	return false;
}

bool CvMinorCivQuest::IsObsolete()
{
	return (IsRevoked() || IsExpired());
}

// The end of this quest has been handled, no effects should happen, and it is marked to be deleted
bool CvMinorCivQuest::IsHandled()
{
	return m_bHandled;
}

void CvMinorCivQuest::SetHandled(bool bValue)
{
	m_bHandled = bValue;
}

// Initializes data to track quest progress and sends notification to player.
// NOTE: Some types initialize data using randomness here. So two otherwise equivalent quests may be initialized with different data.
// NOTE: Should only be called once.
void CvMinorCivQuest::DoStartQuest(int iStartTurn)
{
	m_iStartTurn = iStartTurn;

	CvPlayer* pMinor = &GET_PLAYER(m_eMinor);
	CvPlayer* pAssignedPlayer = &GET_PLAYER(m_eAssignedPlayer);

	Localization::String strMessage;
	Localization::String strSummary;
	int iNotificationX = -1;
	int iNotificationY = -1;

	// Build a Route
	if(m_eType == MINOR_CIV_QUEST_ROUTE)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_START_ROUTE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_START_ROUTE");
	}
	// Kill a Camp
	else if(m_eType == MINOR_CIV_QUEST_KILL_CAMP)
	{
		CvPlot* pPlot = pMinor->GetMinorCivAI()->GetBestNearbyCampToKill();

		FAssertMsg(pPlot != NULL, "MINOR CIV AI: Somehow we're starting a quest to kill a barb camp but we can't find one nearby. Please send Jon this with your last 5 autosaves and what changelist # you're playing. Oh, and you're about to crash.");

		m_iData1 = pPlot->getX();
		m_iData2 = pPlot->getY();

		pPlot->setRevealed(pAssignedPlayer->getTeam(), true);
		pPlot->setRevealedImprovementType(pAssignedPlayer->getTeam(), pPlot->getImprovementType());

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_KILL_CAMP");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_KILL_CAMP");
		iNotificationX = pPlot->getX();
		iNotificationY = pPlot->getY();
	}
	// Connect a Resource
	else if(m_eType == MINOR_CIV_QUEST_CONNECT_RESOURCE)
	{
		ResourceTypes eResource = pMinor->GetMinorCivAI()->GetNearbyResourceForQuest(m_eAssignedPlayer);

		FAssertMsg(eResource != NO_RESOURCE, "MINOR CIV AI: For some reason we got NO_RESOURCE when starting a quest for a major to find a Resource. Please send Jon this with your last 5 autosaves and what changelist # you're playing. Bad things are probably going to happen.");

		m_iData1 = eResource;

		const char* strResourceName = GC.getResourceInfo(eResource)->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_CONNECT_RESOURCE");
		strMessage << strResourceName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CONNECT_RESOURCE");
		strSummary << strResourceName;
	}
	// Construct a Wonder
	else if(m_eType == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
	{
		BuildingTypes eWonder = pMinor->GetMinorCivAI()->GetBestWonderForQuest(m_eAssignedPlayer);

		FAssertMsg(eWonder != NO_BUILDING, "MINOR CIV AI: For some reason we got NO_BUILDING when starting a quest for a major to find a Wonder. Please send Jon this with your last 5 autosaves and what changelist # you're playing. Bad things are probably going to happen.");

		m_iData1 = eWonder;

		const char* strBuildingName = GC.getBuildingInfo(eWonder)->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_CONSTRUCT_WONDER");
		strMessage << strBuildingName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CONSTRUCT_WONDER");
		strSummary << strBuildingName;
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	// Construct a National Wonder
	else if(m_eType == MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER)
	{
		BuildingTypes eNationalWonder = pMinor->GetMinorCivAI()->GetBestNationalWonderForQuest(m_eAssignedPlayer);

		FAssertMsg(eNationalWonder != NO_BUILDING, "MINOR CIV AI: For some reason we got NO_BUILDING when starting a quest for a major to find a Wonder.");

		m_iData1 = eNationalWonder;

		const char* strBuildingName = GC.getBuildingInfo(eNationalWonder)->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_CONSTRUCT_NATIONAL_WONDER");
		strMessage << strBuildingName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CONSTRUCT_NATIONAL_WONDER");
		strSummary << strBuildingName;
	}
#endif
	// Great Person
	else if(m_eType == MINOR_CIV_QUEST_GREAT_PERSON)
	{
		UnitTypes eUnit = pMinor->GetMinorCivAI()->GetBestGreatPersonForQuest(m_eAssignedPlayer);

		FAssertMsg(eUnit != NO_UNIT, "MINOR CIV AI: For some reason we got NO_UNIT when starting a quest for a major to find a Great Person. Please send Jon this with your last 5 autosaves and what changelist # you're playing. Bad things are probably going to happen.");

		m_iData1 = eUnit;

		const char* strUnitName = GC.getUnitInfo(eUnit)->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_GREAT_PERSON");
		strMessage << strUnitName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_GREAT_PERSON");
		strSummary << strUnitName;
	}
	// Kill another City State
	else if(m_eType == MINOR_CIV_QUEST_KILL_CITY_STATE)
	{
		PlayerTypes eTargetCityState = pMinor->GetMinorCivAI()->GetBestCityStateTarget(m_eAssignedPlayer);

		FAssertMsg(eTargetCityState != NO_PLAYER, "MINOR CIV AI: For some reason we got NO_PLAYER when starting a quest for a major to kill a City State. Please send Jon this with your last 5 autosaves and what changelist # you're playing. Bad things are probably going to happen.");

		m_iData1 = eTargetCityState;

		const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_KILL_CITY_STATE");
		strMessage << strTargetNameKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_KILL_CITY_STATE");
		strSummary << strTargetNameKey;
	}
	// Find another player's territory
	else if(m_eType == MINOR_CIV_QUEST_FIND_PLAYER)
	{
		PlayerTypes ePlayerToFind = pMinor->GetMinorCivAI()->GetBestPlayerToFind(m_eAssignedPlayer);

		FAssertMsg(ePlayerToFind != NO_PLAYER, "MINOR CIV AI: For some reason we got NO_PLAYER when starting a quest for a major to find a player. Please send Jon this with your last 5 autosaves and what changelist # you're playing. Bad things are probably going to happen.");

		m_iData1 = ePlayerToFind;

		const char* strCivKey = GET_PLAYER(ePlayerToFind).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_FIND_PLAYER");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_FIND_PLAYER");
		strSummary << strCivKey;
	}
	// Find a Natural Wonder
	else if(m_eType == MINOR_CIV_QUEST_FIND_NATURAL_WONDER)
	{
		int iNumWondersAlreadyFound = GET_TEAM(pAssignedPlayer->getTeam()).GetNumNaturalWondersDiscovered();

		m_iData1 = iNumWondersAlreadyFound;

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_FIND_NATURAL_WONDER");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_FIND_NATURAL_WONDER");
	}
	// Give a gift of gold
	else if(m_eType == MINOR_CIV_QUEST_GIVE_GOLD)
	{
		PlayerTypes eMostRecentBully = pMinor->GetMinorCivAI()->GetMostRecentBullyForQuest();
		int iGoldAlreadyGiven = pMinor->GetMinorCivAI()->GetNumGoldGifted(m_eAssignedPlayer);

		CvAssertMsg(eMostRecentBully != NO_PLAYER, "MINOR CIV AI: eMostRecentBully should not be NO_PLAYER when giving a Give Gold quest! Please send Anton your save file and version.");

		m_iData1 = eMostRecentBully;
		m_iData2 = iGoldAlreadyGiven;

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_GIVE_GOLD");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_GIVE_GOLD");

		if(eMostRecentBully != NO_PLAYER)
		{
			const char* strCivKey = GET_PLAYER(eMostRecentBully).getCivilizationShortDescriptionKey();
			strMessage << strCivKey;
		}
		else
		{
			const char* strCivKey = "Nobody";
			strMessage << strCivKey;
		}
	}
	// Pledge to protect them
	else if(m_eType == MINOR_CIV_QUEST_PLEDGE_TO_PROTECT)
	{
		PlayerTypes eMostRecentBully = pMinor->GetMinorCivAI()->GetMostRecentBullyForQuest();

		CvAssertMsg(eMostRecentBully != NO_PLAYER, "MINOR CIV AI: eMostRecentBully should not be NO_PLAYER when giving a Pledge to Protect quest! Please send Anton your save file and version.");

		m_iData1 = eMostRecentBully;

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_PLEDGE_TO_PROTECT");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_PLEDGE_TO_PROTECT");

		if(eMostRecentBully != NO_PLAYER)
		{
			const char* strCivKey = GET_PLAYER(eMostRecentBully).getCivilizationShortDescriptionKey();
			strMessage << strCivKey;
		}
		else
		{
			const char* strCivKey = "Nobody";
			strMessage << strCivKey;
		}
	}
	// Culture contest
	else if(m_eType == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{
		int iStartingCulture = pAssignedPlayer->GetJONSCultureEverGenerated();

		m_iData1 = iStartingCulture;

		int iTurnsRemaining = GetEndTurn() - GC.getGame().getGameTurn();
		int iTurnsDuration = GetEndTurn() - GetStartTurn();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_CONTEST_CULTURE");
		strMessage << iTurnsRemaining;
		strMessage << iTurnsDuration;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CONTEST_CULTURE");
	}
	// Faith contest
	else if(m_eType == MINOR_CIV_QUEST_CONTEST_FAITH)
	{
		int iStartingFaith = pAssignedPlayer->GetFaithEverGenerated();

		m_iData1 = iStartingFaith;

		int iTurnsRemaining = GetEndTurn() - GC.getGame().getGameTurn();
		int iTurnsDuration = GetEndTurn() - GetStartTurn();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_CONTEST_FAITH");
		strMessage << iTurnsRemaining;
		strMessage << iTurnsDuration;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CONTEST_FAITH");
	}
	// Techs contest
	else if(m_eType == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		int iStartingTechs = GET_TEAM(pAssignedPlayer->getTeam()).GetTeamTechs()->GetNumTechsKnown();

		m_iData1 = iStartingTechs;

		int iTurnsRemaining = GetEndTurn() - GC.getGame().getGameTurn();
		int iTurnsDuration = GetEndTurn() - GetStartTurn();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_CONTEST_TECHS");
		strMessage << iTurnsRemaining;
		strMessage << iTurnsDuration;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CONTEST_TECHS");
	}
	// Invest
	else if(m_eType == MINOR_CIV_QUEST_INVEST)
	{
		int iGoldAlreadyGiven = pMinor->GetMinorCivAI()->GetNumGoldGifted(m_eAssignedPlayer);

		m_iData1 = iGoldAlreadyGiven;

		int iTurnsRemaining = GetEndTurn() - GC.getGame().getGameTurn();
		int iBoostPercentage = 50;//antonjs: todo: XML

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_INVEST");
		strMessage << iTurnsRemaining;
		strMessage << iBoostPercentage;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_INVEST");
	}
	// Bully target City-State
	else if(m_eType == MINOR_CIV_QUEST_BULLY_CITY_STATE)
	{
		PlayerTypes eTargetMinor = pMinor->GetMinorCivAI()->GetBestCityStateTarget(m_eAssignedPlayer);
		CvAssertMsg(eTargetMinor != NO_PLAYER, "MINOR CIV AI: eTargetMinor should not be NO_PLAYER when giving a Bully CS quest! Please send Anton your save file and version.");
		int iLastBullyTurn = GET_PLAYER(eTargetMinor).GetMinorCivAI()->GetTurnLastBulliedByMajor(m_eAssignedPlayer);

		m_iData1 = eTargetMinor;
		m_iData2 = iLastBullyTurn;

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_BULLY_CITY_STATE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_BULLY_CITY_STATE");

		if(eTargetMinor != NO_PLAYER)
		{
			const char* strCivKey = GET_PLAYER(eTargetMinor).getCivilizationShortDescriptionKey();
			strMessage << strCivKey;
			strSummary << strCivKey;
		}
		else
		{
			const char* strCivKey = "Nobody";
			strMessage << strCivKey;
			strSummary << strCivKey;
		}
	}
	// Denounce target Major
	else if(m_eType == MINOR_CIV_QUEST_DENOUNCE_MAJOR)
	{
		PlayerTypes eMostRecentBully = pMinor->GetMinorCivAI()->GetMostRecentBullyForQuest();

		CvAssertMsg(eMostRecentBully != NO_PLAYER, "MINOR CIV AI: eMostRecentBully should not be NO_PLAYER when giving a Denounce Major quest! Please send Anton your save file and version.");

		m_iData1 = eMostRecentBully;

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_DENOUNCE_MAJOR");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_DENOUNCE_MAJOR");

		if(eMostRecentBully != NO_PLAYER)
		{
			const char* strCivKey = GET_PLAYER(eMostRecentBully).getCivilizationShortDescriptionKey();
			strMessage << strCivKey;
			strSummary << strCivKey;
		}
		else
		{
			const char* strCivKey = "Nobody";
			strMessage << strCivKey;
			strSummary << strCivKey;
		}
	}
	// Spread your religion to us
	else if(m_eType == MINOR_CIV_QUEST_SPREAD_RELIGION)
	{
		ReligionTypes eReligion = GC.getGame().GetGameReligions()->GetReligionCreatedByPlayer(m_eAssignedPlayer);

		CvAssertMsg(eReligion != NO_RELIGION, "MINOR CIV AI: eReligion should not be NO_RELIGION when giving a Spread Religion quest! Please send Anton your save file and version.");

		m_iData1 = eReligion;

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_SPREAD_RELIGION");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_SPREAD_RELIGION");

		if(eReligion != NO_RELIGION)
		{
			const CvReligion* pkReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);
			CvString strReligion = pkReligion->GetName(); // Not a key, already localized (may be custom name)
			strMessage << strReligion;
			strSummary << strReligion;
		}
		else
		{
			const char* strReligion = "No Religion";
			strMessage << strReligion;
			strSummary << strReligion;
		}
	}
	// Connect a trade Route
	else if(m_eType == MINOR_CIV_QUEST_TRADE_ROUTE)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_START_TRADE_ROUTE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_START_TRADE_ROUTE");
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	// War with other Major
	else if(m_eType == MINOR_CIV_QUEST_WAR)
	{
		PlayerTypes eMostRecentBully = pMinor->GetMinorCivAI()->GetMostRecentBullyForQuest();

		CvAssertMsg(eMostRecentBully != NO_PLAYER, "MINOR CIV AI: eMostRecentBully should not be NO_PLAYER when giving a War on Major quest!");

		m_iData1 = eMostRecentBully;

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_START_WAR");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_START_WAR");

		if(eMostRecentBully != NO_PLAYER)
		{
			const char* strCivKey = GET_PLAYER(eMostRecentBully).getCivilizationShortDescriptionKey();
			strMessage << strCivKey;
			strSummary << strCivKey;
		}
		else
		{
			const char* strCivKey = "Nobody";
			strMessage << strCivKey;
			strSummary << strCivKey;
		}

		//Let's issue an attack request.
		pAssignedPlayer->GetMilitaryAI()->RequestBasicAttack(eMostRecentBully, 1);
	}
	// Find a City State
	else if(m_eType == MINOR_CIV_QUEST_FIND_CITY_STATE)
	{
		PlayerTypes eTargetCityState = pMinor->GetMinorCivAI()->GetBestCityStateMeetTarget(m_eAssignedPlayer);

		FAssertMsg(eTargetCityState != NO_PLAYER, "MINOR CIV AI: For some reason we got NO_PLAYER when starting a quest for a major to kill a City State. Please send Jon this with your last 5 autosaves and what changelist # you're playing. Bad things are probably going to happen.");

		m_iData1 = eTargetCityState;

		const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_FIND_CITY_STATE");
		strMessage << strTargetNameKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_FIND_CITY_STATE");
		strSummary << strTargetNameKey;

		//Let's issue a recon request.
		EconomicAIStrategyTypes eNavalRecon = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_RECON_SEA");
		if(!pAssignedPlayer->GetEconomicAI()->IsUsingStrategy(eNavalRecon))
		{
			pAssignedPlayer->GetEconomicAI()->SetUsingStrategy(eNavalRecon, 1);
		}
	}
	// Influence
	else if(m_eType == MINOR_CIV_QUEST_INFLUENCE)
	{

		int iTurnsRemaining = GetEndTurn() - GC.getGame().getGameTurn();
		int iBoostPercentage = GC.getINFLUENCE_MINOR_QUEST_BOOST();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_INFLUENCE");
		strMessage << iTurnsRemaining;
		strMessage << iBoostPercentage;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_INFLUENCE");
	}
	// Tourism
	else if(m_eType == MINOR_CIV_QUEST_CONTEST_TOURISM)
	{
		int iValue = pAssignedPlayer->GetCulture()->GetTourism();
		m_iData1 = iValue;

		int iTurnsRemaining = GetEndTurn() - GC.getGame().getGameTurn();
		int iTurnsDuration = GetEndTurn() - GetStartTurn();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_TOURISM");
		strMessage << iTurnsRemaining;
		strMessage << iTurnsDuration;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_TOURISM");
	}
	// Archaeology
	else if(m_eType == MINOR_CIV_QUEST_ARCHAEOLOGY)
	{
		CvPlot* pPlot = pMinor->GetMinorCivAI()->GetBestNearbyDig();

		FAssertMsg(pPlot != NULL, "MINOR CIV AI: Somehow we're starting a quest to dig but we can't find one nearby.");

		m_iData1 = pPlot->getX();
		m_iData2 = pPlot->getY();

		pPlot->setRevealed(pAssignedPlayer->getTeam(), true);
		pPlot->setRevealedImprovementType(pAssignedPlayer->getTeam(), pPlot->getImprovementType());

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ARCHAEOLOGY");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ARCHAEOLOGY");

		iNotificationX = pPlot->getX();
		iNotificationY = pPlot->getY();

	}
	// CIRCUMNAVIGATION
	else if(m_eType == MINOR_CIV_QUEST_CIRCUMNAVIGATION)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_CIRCUMNAVIGATION");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CIRCUMNAVIGATION");

		//Let's issue a recon request.
		EconomicAIStrategyTypes eNavalRecon = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_RECON_SEA");
		if(!pAssignedPlayer->GetEconomicAI()->IsUsingStrategy(eNavalRecon))
		{
			pAssignedPlayer->GetEconomicAI()->SetUsingStrategy(eNavalRecon, 1);
		}
	}
	// Liberate a City State
	else if(m_eType == MINOR_CIV_QUEST_LIBERATION)
	{
		PlayerTypes eTargetCityState = pMinor->GetMinorCivAI()->GetBestCityStateLiberate(m_eAssignedPlayer);

		FAssertMsg(eTargetCityState != NO_PLAYER, "MINOR CIV AI: For some reason we got NO_PLAYER when starting a quest for a major to liberate a City State.");

		m_iData1 = eTargetCityState;

		const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_LIBERATION");
		strMessage << strTargetNameKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_LIBERATION");
		strSummary << strTargetNameKey;

		//Let's issue an attack request.
		pAssignedPlayer->GetMilitaryAI()->RequestBasicAttack(eTargetCityState, 1);
	}
	// Horde
	else if(m_eType == MINOR_CIV_QUEST_HORDE)
	{
		int iTurnsRemaining = GetEndTurn() - GC.getGame().getGameTurn();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_HORDE");
		strMessage << iTurnsRemaining;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_HORDE");
		
		if(!pMinor->GetMinorCivAI()->IsHordeActive())
		{
			pMinor->GetMinorCivAI()->SetTurnsSinceRebellion(GC.getMINOR_QUEST_REBELLION_TIMER());
			pMinor->GetMinorCivAI()->SetHordeActive(true);
			pMinor->GetMinorCivAI()->DoRebellion();
		}

			//Tell the AI to get over there!
			if(!pAssignedPlayer->isHuman())
			{
				pAssignedPlayer->addAIOperation(AI_OPERATION_ALLY_DEFENSE, NO_PLAYER, pMinor->getCapitalCity()->getArea(), pMinor->getCapitalCity(), pMinor->getCapitalCity());
			}

	}
	// Rebellion
	else if(m_eType == MINOR_CIV_QUEST_REBELLION)
	{
		int iTurnsRemaining = GetEndTurn() - GC.getGame().getGameTurn();
		PlayerTypes eMajor = pMinor->GetMinorCivAI()->GetAlly();
		if(eMajor != NO_PLAYER)
		{
			PolicyBranchTypes ePreferredIdeology = GET_PLAYER(eMajor).GetCulture()->GetPublicOpinionPreferredIdeology();
			const char* strCivKey = GET_PLAYER(eMajor).getCivilizationShortDescriptionKey();

			if(ePreferredIdeology == GC.getPOLICY_BRANCH_FREEDOM())
			{
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_REBELLION_FREEDOM");
				strMessage << iTurnsRemaining;
				strMessage << strCivKey;
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_REBELLION");
				strSummary << strCivKey;
			}

			else if(ePreferredIdeology == GC.getPOLICY_BRANCH_ORDER())
			{
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_REBELLION_ORDER");
				strMessage << iTurnsRemaining;
				strMessage << strCivKey;
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_REBELLION");
				strSummary << strCivKey;
			}

			else if(ePreferredIdeology == GC.getPOLICY_BRANCH_AUTOCRACY())
			{
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_REBELLION_AUTOCRACY");
				strMessage << iTurnsRemaining;
				strMessage << strCivKey;
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_REBELLION");
				strSummary << strCivKey;
			}
		}
		else
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_REBELLION_GENERAL_BLANK");
			strMessage << iTurnsRemaining;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_REBELLION_BLANK");
		}

		if(!pMinor->GetMinorCivAI()->IsRebellionActive())
		{
			pMinor->GetMinorCivAI()->SetTurnsSinceRebellion(GC.getMINOR_QUEST_REBELLION_TIMER());
			pMinor->GetMinorCivAI()->SetRebellionActive(true);
			pMinor->GetMinorCivAI()->DoRebellion();
		}

		//Tell the AI to get over there!
		if(eMajor != NO_PLAYER && !GET_PLAYER(eMajor).isHuman())
		{
			pAssignedPlayer->addAIOperation(AI_OPERATION_ALLY_DEFENSE, BARBARIAN_PLAYER, pMinor->getCapitalCity()->getArea(), pMinor->getCapitalCity(), pMinor->getCapitalCity());
		}
	}
#endif

	strMessage << pMinor->getNameKey();
	strSummary << pMinor->getNameKey();
	pMinor->GetMinorCivAI()->AddQuestNotification(strMessage.toUTF8(), strSummary.toUTF8(), m_eAssignedPlayer, iNotificationX, iNotificationY);
}

// Begins the quest, but with client-provided data rather than initializing data internally.
// Should only be called when starting a quest for a player that has an equivalent quest which already exists for a different player (global quests).
void CvMinorCivQuest::DoStartQuestUsingExistingData(CvMinorCivQuest* pExistingQuest)
{
	CvPlayer* pMinor = &GET_PLAYER(m_eMinor);
	CvPlayer* pAssignedPlayer = &GET_PLAYER(m_eAssignedPlayer);

	CvAssertMsg(pMinor != NULL, "pMinor should not be NULL. Please send Anton your save file and version.");
	CvAssertMsg(pAssignedPlayer != NULL, "pAssignedPlayer should not be NULL. Please send Anton your save file and version.");
	CvAssertMsg(pExistingQuest != NULL, "pExistingQuest should not be NULL. Please send Anton your save file and version.");
	if(pMinor == NULL || pAssignedPlayer == NULL || pExistingQuest == NULL) return;

	Localization::String strMessage;
	Localization::String strSummary;
	int iNotificationX = -1;
	int iNotificationY = -1;

	// Kill a Camp - This quest needs to have the data for the same camp as the existing quest
	if(m_eType == MINOR_CIV_QUEST_KILL_CAMP)
	{
		m_iStartTurn = pExistingQuest->GetStartTurn();

		int iCampX = pExistingQuest->GetPrimaryData();
		int iCampY = pExistingQuest->GetSecondaryData();

		CvPlot* pPlot = GC.getMap().plot(iCampX, iCampY);

		if(!pPlot)
		{
			CvAssertMsg(false, "We're starting a quest to kill a barb camp using an existing quest's barb camp data, but the data is bad. Please send Anton your save file and version.");
			return;
		}
		if(pPlot->getImprovementType() != GC.getBARBARIAN_CAMP_IMPROVEMENT())
		{
			CvAssertMsg(false, "We're starting a quest to kill a barb camp using an existing quest's barb camp data, but there's no camp there anymore. Please send Anton your save file and version.");
			return;
		}

		m_iData1 = iCampX;
		m_iData2 = iCampY;

		pPlot->setRevealed(pAssignedPlayer->getTeam(), true);
		pPlot->setRevealedImprovementType(pAssignedPlayer->getTeam(), pPlot->getImprovementType());

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_KILL_CAMP");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_KILL_CAMP");
		iNotificationX = pPlot->getX();
		iNotificationY = pPlot->getY();

		strMessage << pMinor->getNameKey();
		strSummary << pMinor->getNameKey();
		pMinor->GetMinorCivAI()->AddQuestNotification(strMessage.toUTF8(), strSummary.toUTF8(), m_eAssignedPlayer, iNotificationX, iNotificationY);
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	else if(m_eType == MINOR_CIV_QUEST_ARCHAEOLOGY)
	{
		m_iStartTurn = pExistingQuest->GetStartTurn();

		int iDigX = pExistingQuest->GetPrimaryData();
		int iDigY = pExistingQuest->GetSecondaryData();

		CvPlot* pPlot = GC.getMap().plot(iDigX, iDigY);

		if(!pPlot)
		{
			CvAssertMsg(false, "We're starting a quest to find an archaeology dig using an existing quest's data, but the data is bad.");
			return;
		}
		if(pPlot->getResourceType() != GC.getARTIFACT_RESOURCE())
		{
			CvAssertMsg(false, "We're starting a quest to find an archaeology dig using an existing quest's data, but there's no dig there anymore.");
			return;
		}

		m_iData1 = iDigX;
		m_iData2 = iDigY;

		pPlot->setRevealed(pAssignedPlayer->getTeam(), true);
		pPlot->SetResourceForceReveal(pAssignedPlayer->getTeam(), pPlot->getResourceType());

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ARCHAEOLOGY");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ARCHAEOLOGY");
		iNotificationX = pPlot->getX();
		iNotificationY = pPlot->getY();

		strMessage << pMinor->getNameKey();
		strSummary << pMinor->getNameKey();
		pMinor->GetMinorCivAI()->AddQuestNotification(strMessage.toUTF8(), strSummary.toUTF8(), m_eAssignedPlayer, iNotificationX, iNotificationY);
	}
#endif
	// Other global quests (ie. contests) - Quest data is initialized as normal except for the start turn, which was in the past
	else if(pMinor->GetMinorCivAI()->IsGlobalQuest(pExistingQuest->GetType()))
	{
		DoStartQuest(pExistingQuest->GetStartTurn());
	}

	// Personal quests - Should not be started from an existing quest's data!!
	else
	{
		CvAssertMsg(false, "Trying to start a personal quest using existing quest data.  This should not be done.  Please send Anton your save file and version.");
		DoStartQuest(pExistingQuest->GetStartTurn());
	}
}

// Awards influence and sends notification to player.
// Should only be called once.
bool CvMinorCivQuest::DoFinishQuest()
{
	if (!IsComplete())
		return false;

	if (IsHandled())
		return false;

	SetHandled(true); // We are handling the end of the quest, and this should only happen once

	CvPlayer* pMinor = &GET_PLAYER(m_eMinor);

	bool bWasFriends = pMinor->GetMinorCivAI()->IsFriends(m_eAssignedPlayer);
	bool bWasAllies = pMinor->GetMinorCivAI()->IsAllies(m_eAssignedPlayer);
	PlayerTypes eOldAlly = pMinor->GetMinorCivAI()->GetAlly();
	int iOldInf = pMinor->GetMinorCivAI()->GetEffectiveFriendshipWithMajor(m_eAssignedPlayer);

	pMinor->GetMinorCivAI()->ChangeFriendshipWithMajor(m_eAssignedPlayer, GetInfluenceReward(), /*bFromQuest*/ true);
	
	bool bNowFriends = pMinor->GetMinorCivAI()->IsFriends(m_eAssignedPlayer);
	bool bNowAllies = pMinor->GetMinorCivAI()->IsAllies(m_eAssignedPlayer);
	PlayerTypes eNewAlly = pMinor->GetMinorCivAI()->GetAlly();
	int iNewInf = pMinor->GetMinorCivAI()->GetEffectiveFriendshipWithMajor(m_eAssignedPlayer);
	int iInfChange = iNewInf - iOldInf;

	Localization::String strMessage;
	Localization::String strSummary;
	CivsList veNamesToShow;

	// BUILD A ROUTE
	if(m_eType == MINOR_CIV_QUEST_ROUTE)
	{
		// Route exists!
		pMinor->GetMinorCivAI()->SetRouteConnectionEstablished(m_eAssignedPlayer, true);

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_ROUTE_CONNECTION");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_ROUTE_CONNECTION");
	}

	// KILL A CAMP
	else if(m_eType == MINOR_CIV_QUEST_KILL_CAMP)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_KILL_CAMP");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_KILL_CAMP");
	}

	// CONNECT A RESOURCE
	else if(m_eType == MINOR_CIV_QUEST_CONNECT_RESOURCE)
	{
		ResourceTypes eResource = (ResourceTypes) GetPrimaryData();
		const char* strResourceName = GC.getResourceInfo(eResource)->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONNECT_RESOURCE");
		strMessage << strResourceName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_CONNECT_RESOURCE");
		strSummary << strResourceName;
	}

	// CONSTRUCT A WONDER
	else if(m_eType == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
	{
		BuildingTypes eWonder = (BuildingTypes) GetPrimaryData();
		const char* strBuildingName = GC.getBuildingInfo(eWonder)->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONSTRUCT_WONDER");
		strMessage << strBuildingName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_CONSTRUCT_WONDER");
		strSummary << strBuildingName;
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	// CONSTRUCT A NATIONAL WONDER
	else if(m_eType == MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER)
	{
		BuildingTypes eNationalWonder = (BuildingTypes) GetPrimaryData();
		const char* strBuildingName = GC.getBuildingInfo(eNationalWonder)->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONSTRUCT_NATIONAL_WONDER");
		strMessage << strBuildingName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_CONSTRUCT_NATIONAL_WONDER");
		strSummary << strBuildingName;
	}
#endif

	// GREAT PERSON
	else if(m_eType == MINOR_CIV_QUEST_GREAT_PERSON)
	{
		UnitTypes eUnit = (UnitTypes) GetPrimaryData();
		const char* strUnitName = GC.getUnitInfo(eUnit)->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_GREAT_PERSON");
		strMessage << strUnitName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_GREAT_PERSON");
		strSummary << strUnitName;
	}

	// KILL ANOTHER CITY STATE
	else if(m_eType == MINOR_CIV_QUEST_KILL_CITY_STATE)
	{
		PlayerTypes eTargetCityState = (PlayerTypes) GetPrimaryData();
		const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_KILL_CITY_STATE");
		strMessage << strTargetNameKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_KILL_CITY_STATE");
		strSummary << strTargetNameKey;
	}

	// FIND ANOTHER PLAYER
	else if(m_eType == MINOR_CIV_QUEST_FIND_PLAYER)
	{
		PlayerTypes ePlayerFound = (PlayerTypes) GetPrimaryData();
		const char* strCivKey = GET_PLAYER(ePlayerFound).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_FIND_PLAYER");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_FIND_PLAYER");
		strSummary << strCivKey;
	}

	// FIND NATURAL WONDER
	else if(m_eType == MINOR_CIV_QUEST_FIND_NATURAL_WONDER)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_FIND_NATURAL_WONDER");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_FIND_NATURAL_WONDER");
	}

	// Give gold
	else if(m_eType == MINOR_CIV_QUEST_GIVE_GOLD)
	{
		PlayerTypes eMostRecentBully = (PlayerTypes) GetPrimaryData();
		const char* strCivKey = "Nobody";
		if(eMostRecentBully != NO_PLAYER)
			strCivKey = GET_PLAYER(eMostRecentBully).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_GIVE_GOLD");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_GIVE_GOLD");
	}

	// Pledge to protect
	else if(m_eType == MINOR_CIV_QUEST_PLEDGE_TO_PROTECT)
	{
		PlayerTypes eMostRecentBully = (PlayerTypes) GetPrimaryData();
		const char* strCivKey = "Nobody";
		if(eMostRecentBully != NO_PLAYER)
			strCivKey = GET_PLAYER(eMostRecentBully).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_PLEDGE_TO_PROTECT");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_PLEDGE_TO_PROTECT");
	}

	// Culture contest
	else if(m_eType == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONTEST_CULTURE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_CONTEST_CULTURE");
		veNamesToShow = GetContestLeaders();
	}

	// Faith contest
	else if(m_eType == MINOR_CIV_QUEST_CONTEST_FAITH)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONTEST_FAITH");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_CONTEST_FAITH");
		veNamesToShow = GetContestLeaders();
	}

	// Techs contest
	else if(m_eType == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONTEST_TECHS");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_CONTEST_TECHS");
		veNamesToShow = GetContestLeaders();
	}

	// Invest
	else if(m_eType == MINOR_CIV_QUEST_INVEST)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_INVEST");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_INVEST");
	}

	// Bully target City-State
	else if(m_eType == MINOR_CIV_QUEST_BULLY_CITY_STATE)
	{
		PlayerTypes eTargetMinor = (PlayerTypes) GetPrimaryData();
		const char* strCivKey = "Nobody";
		if(eTargetMinor != NO_PLAYER)
			strCivKey = GET_PLAYER(eTargetMinor).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_BULLY_CITY_STATE");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_BULLY_CITY_STATE");
		strSummary << strCivKey;
	}

	// Denounce target Major
	else if(m_eType == MINOR_CIV_QUEST_DENOUNCE_MAJOR)
	{
		PlayerTypes eTargetMajor = (PlayerTypes) GetPrimaryData();
		const char* strCivKey = "Nobody";
		if(eTargetMajor != NO_PLAYER)
			strCivKey = GET_PLAYER(eTargetMajor).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_DENOUNCE_MAJOR");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_DENOUNCE_MAJOR");
		strSummary << strCivKey;
	}

	// Spread your religion to us
	else if(m_eType == MINOR_CIV_QUEST_SPREAD_RELIGION)
	{
		ReligionTypes eReligion = (ReligionTypes) GetPrimaryData();
		CvString strReligionKey = "Atheism";
		if(eReligion != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);
			if(pReligion != NULL)
			{
				strReligionKey = pReligion->GetName();
			}	
		}

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_SPREAD_RELIGION");
		strMessage << strReligionKey.c_str();
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_SPREAD_RELIGION");
		strSummary << strReligionKey.c_str();
	}

	// Connect A Trade Route
	if(m_eType == MINOR_CIV_QUEST_TRADE_ROUTE)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_TRADE_ROUTE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_TRADE_ROUTE");
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	// War with major
	if(m_eType == MINOR_CIV_QUEST_WAR)
	{
		PlayerTypes eTargetMajor = (PlayerTypes) GetPrimaryData();
		const char* strCivKey = "Nobody";
		if(eTargetMajor != NO_PLAYER)
			strCivKey = GET_PLAYER(eTargetMajor).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_WAR");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_WAR");
	}
	// Find City State
	else if(m_eType == MINOR_CIV_QUEST_FIND_CITY_STATE)
	{
		PlayerTypes eTargetCityState = (PlayerTypes) GetPrimaryData();
		const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_FIND_CITY_STATE");
		strMessage << strTargetNameKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_FIND_CITY_STATE");
		strSummary << strTargetNameKey;
	}
	// Influence
	else if(m_eType == MINOR_CIV_QUEST_INFLUENCE)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_INFLUENCE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_INFLUENCE");
	}
	// Tourism
	else if(m_eType == MINOR_CIV_QUEST_CONTEST_TOURISM)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_TOURISM");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_TOURISM");
	}
	// Archaeology
	else if(m_eType == MINOR_CIV_QUEST_ARCHAEOLOGY)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_ARCHAEOLOGY");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_ARCHAEOLOGY");
	}
	// Circumnavigation
	else if(m_eType == MINOR_CIV_QUEST_CIRCUMNAVIGATION)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CIRCUMNAVIGATION");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_CIRCUMNAVIGATION");
	}
	// LIBERATE A CITY STATE
	else if(m_eType == MINOR_CIV_QUEST_LIBERATION)
	{
		PlayerTypes eTargetCityState = (PlayerTypes) GetPrimaryData();
		const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_LIBERATION");
		strMessage << strTargetNameKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_LIBERATION");
		strSummary << strTargetNameKey;
	}
	// Horde
	else if(m_eType == MINOR_CIV_QUEST_HORDE)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_HORDE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_HORDE");
		
		if(pMinor->GetMinorCivAI()->IsHordeActive())
		{
			pMinor->GetMinorCivAI()->SetTurnsSinceRebellion(0);
			pMinor->GetMinorCivAI()->SetHordeActive(false);
			pMinor->GetMinorCivAI()->SetCooldownSpawn(30);
		}

		//Update Military AI
		int iOperationID = AI_OPERATION_ALLY_DEFENSE;
			CvAIOperation* pThisOperation = GET_PLAYER(m_eAssignedPlayer).getAIOperation(iOperationID);
			if(pThisOperation)
			{
				GET_PLAYER(m_eAssignedPlayer).getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
			}
	}
	// Rebellion
	else if(m_eType == MINOR_CIV_QUEST_REBELLION)
	{		
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_REBELLION");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_REBELLION");

		if(pMinor->GetMinorCivAI()->IsRebellionActive())
		{
			pMinor->GetMinorCivAI()->SetTurnsSinceRebellion(0);
			pMinor->GetMinorCivAI()->SetRebellionActive(false);
			pMinor->GetMinorCivAI()->SetCooldownSpawn(30);
		}
	
		//Update Military AI
		int iOperationID = AI_OPERATION_ALLY_DEFENSE;
		CvAIOperation* pThisOperation = GET_PLAYER(m_eAssignedPlayer).getAIOperation(iOperationID);
		if(pThisOperation)
		{
			GET_PLAYER(m_eAssignedPlayer).getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
		}
	}
#endif

	// Update the UI with the changed data, in case it is open
	if(m_eAssignedPlayer == GC.getGame().getActivePlayer())
	{
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	}

	strMessage << pMinor->getNameKey();
	strMessage << iInfChange;
	strSummary << pMinor->getNameKey();

	CvString sMessage = strMessage.toUTF8();
	CvString sSummary = strSummary.toUTF8();

	// This quest involved multiple minors, so grab their names for the notification
	if(veNamesToShow.size() > 0)
	{
		sMessage = sMessage + pMinor->GetMinorCivAI()->GetNamesListAsString(veNamesToShow);
	}

	// This quest reward changed our status, so grab that info for the notification
	if ((!bWasFriends && bNowFriends) || (!bWasAllies && bNowAllies))
	{
		pair<CvString, CvString> statusChangeStrings = pMinor->GetMinorCivAI()->GetStatusChangeNotificationStrings(m_eAssignedPlayer, /*bAdd*/true, bNowFriends, bNowAllies, eOldAlly, eNewAlly);
		sMessage = sMessage + "[NEWLINE][NEWLINE]" + statusChangeStrings.first;
	}

	pMinor->GetMinorCivAI()->AddQuestNotification(sMessage, sSummary, m_eAssignedPlayer);

	return true;
}

// Do any cleanup and notifications for when a quest is cancelled (ex. becomes obsolete)
bool CvMinorCivQuest::DoCancelQuest()
{
	if (IsHandled())
		return false;

	SetHandled(true); // We are handling the end of the quest, and this should only happen once

	CvPlayer* pMinor = &GET_PLAYER(m_eMinor);

	// Why is the quest cancelled?  Will affect which notification message we send
	bool bRevoked = IsRevoked();
	bool bExpired = IsExpired();

	Localization::String strMessage;
	Localization::String strSummary;
	CivsList veNamesToShow;

	// If quest was revoked due to bullying, notification is handled elsewhere (to allow condensing)
	if(bRevoked)
	{
	}
	// If quest expired "naturally", send a notification particular to the expiration conditions of the quest
	else if(bExpired)
	{
		// City-state wanted us to clear a camp
		if(m_eType == MINOR_CIV_QUEST_KILL_CAMP)
		{
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
			if (MOD_DIPLOMACY_CITYSTATES_QUESTS) {
				//Minor punishment- don't ignore city-state quests!
				pMinor->GetMinorCivAI()->ChangeFriendshipWithMajor(m_eAssignedPlayer, -(GetInfluenceReward() / 2), /*bFromQuest*/ true);
			}
#endif
			strMessage = Localization::Lookup("TXT_KEY_NTFN_QUEST_ENDED_KILL_CAMP");
			strSummary = Localization::Lookup("TXT_KEY_NTFN_QUEST_ENDED_KILL_CAMP_S");
		}
		// CONSTRUCT A WONDER
		else if(m_eType == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
		{
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
			if (MOD_DIPLOMACY_CITYSTATES_QUESTS) {
				//Minor punishment- don't ignore city-state quests!
				pMinor->GetMinorCivAI()->ChangeFriendshipWithMajor(m_eAssignedPlayer, -(GetInfluenceReward() / 2), /*bFromQuest*/ true);
			}
#endif
			BuildingTypes eWonder = (BuildingTypes) GetPrimaryData();
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eWonder);
			CvAssertMsg(pkBuildingInfo, "Building info not expected to be FALSE! Please send Anton your save file and version.");
			if(!pkBuildingInfo) 
				return false;

			const char* strBuildingName = pkBuildingInfo->GetDescriptionKey();

			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_CONSTRUCT_WONDER");
			strMessage << strBuildingName;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_CONSTRUCT_WONDER");
			strSummary << strBuildingName;
		}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
		// TOURISM CONTEST
		else if(m_eType == MINOR_CIV_QUEST_CONTEST_TOURISM)
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_TOURISM");
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_CONTEST_TOURISM");
			veNamesToShow = GetContestLeaders();
		}
		
		// City-state wanted us to dig
		else if(m_eType == MINOR_CIV_QUEST_ARCHAEOLOGY)
		{
			//Minor punishment- don't ignore city-state quests!
			pMinor->GetMinorCivAI()->ChangeFriendshipWithMajor(m_eAssignedPlayer, -(GetInfluenceReward() / 2), /*bFromQuest*/ true);

			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_ARCHAEOLOGY");
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_ARCHAEOLOGY");
		}

		// City-state wanted us to circumnavigate
		else if(m_eType == MINOR_CIV_QUEST_CIRCUMNAVIGATION)
		{
			//Minor punishment- don't ignore city-state quests!
			pMinor->GetMinorCivAI()->ChangeFriendshipWithMajor(m_eAssignedPlayer, -(GetInfluenceReward() / 2), /*bFromQuest*/ true);

			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_CIRCUMNAVIGATION");
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_CIRCUMNAVIGATION");
		}
		
		// City-state wanted us to liberate another city-state and we failed
		else if(m_eType == MINOR_CIV_QUEST_LIBERATION)
		{
			//Minor punishment- don't ignore city-state quests!
			pMinor->GetMinorCivAI()->ChangeFriendshipWithMajor(m_eAssignedPlayer, -(GetInfluenceReward() / 2), /*bFromQuest*/ true);
			PlayerTypes eTargetCityState = (PlayerTypes) GetPrimaryData();

			const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_LIBERATION");
			strMessage << strTargetNameKey;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_LIBERATION");
			strSummary << strTargetNameKey;
		}

		else if(m_eType == MINOR_CIV_QUEST_HORDE)
		{
			//City Sacked! Don't ignore city-state quests!
			
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_HORDE");
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_HORDE");
		
			if(pMinor->GetMinorCivAI()->IsHordeActive())
			{
				pMinor->GetMinorCivAI()->SetHordeActive(false);
				pMinor->GetMinorCivAI()->SetSacked(true);
				pMinor->GetMinorCivAI()->SetTurnsSinceRebellion(0);
				pMinor->GetMinorCivAI()->SetCooldownSpawn(50);
			}

			//Update Military AI
			int iOperationID = AI_OPERATION_ALLY_DEFENSE;
			CvAIOperation* pThisOperation = GET_PLAYER(m_eAssignedPlayer).getAIOperation(iOperationID);
			if(pThisOperation)
			{
				GET_PLAYER(m_eAssignedPlayer).getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
			}
		}

		else if(m_eType == MINOR_CIV_QUEST_REBELLION)
		{
			//City Defects! Don't ignore city-state quests!

			PlayerTypes eMajor = pMinor->GetMinorCivAI()->GetAlly();
			if(eMajor != NO_PLAYER)
			{
				const char* strCivKey = GET_PLAYER(eMajor).getCivilizationShortDescriptionKey();

				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_REBELLION");
				strMessage << strCivKey;
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_REBELLION");
			}
			else
			{
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_REBELLION_BLANK");
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_REBELLION");
			}

			if(pMinor->GetMinorCivAI()->IsRebellionActive())
			{
				pMinor->GetMinorCivAI()->SetRebellionActive(false);
				pMinor->GetMinorCivAI()->SetRebellion(true);
				pMinor->GetMinorCivAI()->SetTurnsSinceRebellion(0);
				pMinor->GetMinorCivAI()->SetCooldownSpawn(50);
			}
			//Update Military AI
			int iOperationID = AI_OPERATION_ALLY_DEFENSE;
			CvAIOperation* pThisOperation = GET_PLAYER(m_eAssignedPlayer).getAIOperation(iOperationID);
			if(pThisOperation)
			{
				GET_PLAYER(m_eAssignedPlayer).getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
			}
		}
#endif

		// KILL ANOTHER CITY STATE
		else if(m_eType == MINOR_CIV_QUEST_KILL_CITY_STATE)
		{
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
			if (MOD_DIPLOMACY_CITYSTATES_QUESTS) {
				//Minor punishment- don't ignore city-state quests!
				pMinor->GetMinorCivAI()->ChangeFriendshipWithMajor(m_eAssignedPlayer, -(GetInfluenceReward() / 2), /*bFromQuest*/ true);
			}
#endif
			PlayerTypes eTargetCityState = (PlayerTypes) GetPrimaryData();

			const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_KILL_CITY_STATE");
			strMessage << strTargetNameKey;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_KILL_CITY_STATE");
			strSummary << strTargetNameKey;
		}

		// Contest Culture
		else if(m_eType == MINOR_CIV_QUEST_CONTEST_CULTURE)
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_CULTURE");
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_CONTEST_CULTURE");
			veNamesToShow = GetContestLeaders();
		}

		// Contest Faith
		else if(m_eType == MINOR_CIV_QUEST_CONTEST_FAITH)
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_FAITH");
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_CONTEST_FAITH");
			veNamesToShow = GetContestLeaders();
		}

		// Contest Techs
		else if(m_eType == MINOR_CIV_QUEST_CONTEST_TECHS)
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_TECHS");
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_CONTEST_TECHS");
			veNamesToShow = GetContestLeaders();
		}

		// General "Quest Expired" catch statement
		else
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_OTHER");
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_OTHER");
		}

		strMessage << pMinor->getNameKey();
		strSummary << pMinor->getNameKey();

		CvString sMessage = strMessage.toUTF8();
		CvString sSummary = strSummary.toUTF8();

		// This quest involved multiple winners, so grab their names for the notification
		if(veNamesToShow.size() > 0)
		{
			sMessage = sMessage + pMinor->GetMinorCivAI()->GetNamesListAsString(veNamesToShow);
		}

		pMinor->GetMinorCivAI()->AddQuestNotification(sMessage, sSummary, m_eAssignedPlayer);
	}

	return true;
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvMinorCivQuest& writeTo)
{
	uint uiVersion;
	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	loadFrom >> writeTo.m_eType;
	loadFrom >> writeTo.m_iStartTurn;
	loadFrom >> writeTo.m_iData1;
	loadFrom >> writeTo.m_iData2;
	if (uiVersion >= 2)
	{
		loadFrom >> writeTo.m_bHandled;
	}
	else
	{
		writeTo.m_bHandled = false;
	}

	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvMinorCivQuest& readFrom)
{
	uint uiVersion = 2;
	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);

	saveTo << readFrom.m_eType;
	saveTo << readFrom.m_iStartTurn;
	saveTo << readFrom.m_iData1;
	saveTo << readFrom.m_iData2;
	saveTo << readFrom.m_bHandled;

	return saveTo;
}


//======================================================================================================
//					CvMinorCivAI
//======================================================================================================
CvMinorCivAI::CvMinorCivAI()
{
}
//------------------------------------------------------------------------------
CvMinorCivAI::~CvMinorCivAI(void)
{
	Uninit();
}

/// Initialize
void CvMinorCivAI::Init(CvPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
#if defined(MOD_GLOBAL_MAX_MAJOR_CIVS)
	m_minorCivType = GetMinorCivType();
#else
	m_minorCivType = CvPreGame::minorCivType(m_pPlayer->GetID());
#endif

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
	m_eUniqueUnit = NO_UNIT;

	m_iTurnsSinceThreatenedByBarbarians = -1;
	m_iGlobalQuestCountdown = -1;

	m_eAlly = NO_PLAYER;
	m_iTurnAllied = -1;
	m_eMajorBoughtOutBy = NO_PLAYER;
	m_bDisableNotifications = false;

	int iI, iJ;

	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		m_abWarQuestAgainstMajor[iI] = false;

		for(iJ = 0; iJ < MAX_MAJOR_CIVS; iJ++)
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
		m_aiNumGoldGifted[iI] = 0;
		m_aiTurnLastBullied[iI] = -1;
		m_aiTurnLastPledged[iI] = -1;
		m_aiTurnLastBrokePledge[iI] = -1;
		m_abUnitSpawningDisabled[iI] = false;
		m_abMajorIntruding[iI] = false;
		m_abEverFriends[iI] = false;
		m_abPledgeToProtect[iI] = false;
		m_aiMajorScratchPad[iI] = 0;
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
		m_bIsSacked = false;
		m_bIsRebellion = false;
		m_iIsRebellionCountdown = 0;
		m_bIsRebellionActive = 0;
		m_bIsHordeActive = 0;
		m_iCooldownSpawn = 0;
#endif
#if defined(MOD_BALANCE_CORE_MINORS)
		m_abIsJerk[iI] = false;
		m_aiJerk[iI] = 0;
#endif
	}

	for(iI = 0; iI < REALLY_MAX_TEAMS; iI++)
	{
		m_abPermanentWar[iI] = false;
		m_abWaryOfTeam[iI] = false;
	}

	ResetQuestList();

	if(GetPlayer()->isMinorCiv())
	{
		CvPlot* pLoopPlot;
		TeamTypes eTeam = GetPlayer()->getTeam();
		int iNumPlotsInEntireWorld = GC.getMap().numPlots();
		for(int iLoopPlot = 0; iLoopPlot < iNumPlotsInEntireWorld; iLoopPlot++)
		{
			pLoopPlot = GC.getMap().plotByIndexUnchecked(iLoopPlot);
			if(pLoopPlot)
				pLoopPlot->setRevealed(eTeam, true);
		}
	}

}

void CvMinorCivAI::ResetQuestList()
{
	//antonjs: todo: this is called when PlayerTypes is -1, why? does it cause inefficiency or unused memory?

	for(uint iPlayerEntry = 0; iPlayerEntry < m_QuestsGiven.size(); iPlayerEntry++)
	{
		m_QuestsGiven[iPlayerEntry].clear();
	}

	m_QuestsGiven.clear();

	for(int iNewEntry = 0; iNewEntry < MAX_MAJOR_CIVS; iNewEntry++)
	{
		QuestListForPlayer vQuestList;
		m_QuestsGiven.push_back(vQuestList);
	}
}

/// Serialization read
void CvMinorCivAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	kStream >> m_ePersonality;
	kStream >> m_eStatus;
	kStream >> m_eUniqueUnit;

	kStream >> m_iTurnsSinceThreatenedByBarbarians;

	kStream >> m_iGlobalQuestCountdown;

	kStream >> m_eAlly;

	kStream >> m_iTurnAllied;

	kStream >> m_eMajorBoughtOutBy;

	kStream >> m_abWarQuestAgainstMajor;

	kStream >> m_aaiNumEnemyUnitsLeftToKillByMajor;

	kStream >> m_abRouteConnectionEstablished;

	kStream >> m_aiFriendshipWithMajorTimes100;

	kStream >> m_aiAngerFreeIntrusionCounter;

	kStream >> m_aiPlayerQuests;
	kStream >> m_aiQuestData1;
	kStream >> m_aiQuestData2;
	kStream >> m_aiQuestCountdown;
	kStream >> m_aiUnitSpawnCounter;

	kStream >> m_aiNumUnitsGifted;

	kStream >> m_aiNumGoldGifted;

	kStream >> m_aiTurnLastBullied;

	kStream >> m_aiTurnLastPledged;
	kStream >> m_aiTurnLastBrokePledge;

	kStream >> m_abUnitSpawningDisabled;
	kStream >> m_abMajorIntruding;
	kStream >> m_abEverFriends;

	kStream >> m_abPledgeToProtect;

	kStream >> m_abPermanentWar;

	kStream >> m_abWaryOfTeam;

	// List of quests given
	ResetQuestList();

	int iPlayerEntriesToRead;
	//antonjs: consider: change this; always read in iPlayerEntriesToRead
	iPlayerEntriesToRead = MAX_MAJOR_CIVS;

	for(int iPlayerLoop = 0; iPlayerLoop < iPlayerEntriesToRead; iPlayerLoop++)
	{
		int iQuestEntriesToRead;
		kStream >> iQuestEntriesToRead;
		CvMinorCivQuest tempQuest;
		for(int iQuestLoop = 0; iQuestLoop < iQuestEntriesToRead; iQuestLoop++)
		{
			kStream >> tempQuest;

			tempQuest.m_eMinor = GetPlayer()->GetID();
			tempQuest.m_eAssignedPlayer = (PlayerTypes) iPlayerLoop;

			m_QuestsGiven[iPlayerLoop].push_back(tempQuest);
		}
	}
	CvAssertMsg(m_QuestsGiven.size() == MAX_MAJOR_CIVS, "Number of entries in minor's quest list does not match MAX_MAJOR_CIVS when read from memory!");

	kStream >> m_bDisableNotifications;
}

/// Serialization write
void CvMinorCivAI::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	kStream << m_ePersonality;
	kStream << m_eStatus;
	kStream << m_eUniqueUnit; // Version 14

	kStream << m_iTurnsSinceThreatenedByBarbarians;

	kStream << m_iGlobalQuestCountdown; // Version 11

	kStream << m_eAlly;
	kStream << m_iTurnAllied;

	kStream << m_eMajorBoughtOutBy; // Version 16

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
	kStream << m_aiNumGoldGifted;
	kStream << m_aiTurnLastBullied;
	kStream << m_aiTurnLastPledged;
	kStream << m_aiTurnLastBrokePledge;
	kStream << m_abUnitSpawningDisabled;
	kStream << m_abMajorIntruding;
	kStream << m_abEverFriends;
	kStream << m_abPledgeToProtect;
	kStream << m_abPermanentWar;
	kStream << m_abWaryOfTeam; // Version 12

	// List of quests given
	CvAssertMsg(m_QuestsGiven.size() == MAX_MAJOR_CIVS, "Number of entries in minor's quest list does not match MAX_MAJOR_CIVS when writing to memory!");
	QuestListForAllPlayers::const_iterator itr_player;
	for(itr_player = m_QuestsGiven.begin(); itr_player != m_QuestsGiven.end(); itr_player++)
	{
		QuestListForPlayer* pvQuestList = (QuestListForPlayer*)itr_player;
		int iQuestEntriesToWrite = pvQuestList->size();
		kStream << iQuestEntriesToWrite;
		QuestListForPlayer::const_iterator itr_quest;
		for(itr_quest = pvQuestList->begin(); itr_quest != pvQuestList->end(); itr_quest++)
		{
			kStream << *itr_quest;
		}
	}

	kStream << m_bDisableNotifications;
}

/// Pick the minor civ's personality and any special traits (ie. unique unit for Militaristic)
void CvMinorCivAI::DoPickInitialItems()
{
	// Personality
	DoPickPersonality();

	// Unique unit
	DoPickUniqueUnit();
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
#if defined(MOD_GLOBAL_MAX_MAJOR_CIVS)
	return CvPreGame::minorCivType((PlayerTypes) (m_pPlayer->GetID() + (MAX_PREGAME_MAJOR_CIVS - MAX_MAJOR_CIVS)));
#else
	return CvPreGame::minorCivType(m_pPlayer->GetID());
#endif
}

/// What is the personality of this Minor
MinorCivPersonalityTypes CvMinorCivAI::GetPersonality() const
{
	return m_ePersonality;
}

#if defined(MOD_API_EXTENSIONS)
/// Set a Personality for this minor
void CvMinorCivAI::SetPersonality(MinorCivPersonalityTypes ePersonality)
{
	m_ePersonality = ePersonality;
}
#endif

/// Picks a random Personality for this minor
void CvMinorCivAI::DoPickPersonality()
{

	FlavorTypes eFlavorCityDefense = NO_FLAVOR;
	FlavorTypes eFlavorDefense = NO_FLAVOR;
	FlavorTypes eFlavorOffense = NO_FLAVOR;
	for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_CITY_DEFENSE")
		{
			eFlavorCityDefense = (FlavorTypes)iFlavorLoop;
		}
		if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_DEFENSE")
		{
			eFlavorDefense = (FlavorTypes)iFlavorLoop;
		}
		if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_OFFENSE")
		{
			eFlavorOffense = (FlavorTypes)iFlavorLoop;
		}
	}

	CvFlavorManager* pFlavorManager = m_pPlayer->GetFlavorManager();
	int* pFlavors = pFlavorManager->GetAllPersonalityFlavors();

	MinorCivPersonalityTypes eRandPersonality = (MinorCivPersonalityTypes) GC.getGame().getJonRandNum(NUM_MINOR_CIV_PERSONALITY_TYPES, "Minor Civ AI: Picking Personality for this Game (should happen only once per player)");
#if defined(MOD_API_EXTENSIONS)
	SetPersonality(eRandPersonality);
#else
	m_ePersonality = eRandPersonality;
#endif

	switch(eRandPersonality)
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

/// Does this civ have a unique unit? (only for Militaristic)
bool CvMinorCivAI::IsHasUniqueUnit() const
{
	return (m_eUniqueUnit != NO_UNIT);
}

/// What is this civ's unique unit? (only for Militaristic)
UnitTypes CvMinorCivAI::GetUniqueUnit() const
{
	return m_eUniqueUnit;
}

/// Override picked unique unit (only for Militaristic)
void CvMinorCivAI::SetUniqueUnit(UnitTypes eUnit)
{
	CvAssertMsg(GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC, "Setting a unique unit for a non-Militaristic City-State.  It will never be spawned without additional scripting.");
	CvUnitEntry* pInfo = GC.getUnitInfo(eUnit);
	CvAssertMsg(pInfo, "Setting a unique unit for a City-State that the game core could not identify. Please send Anton your save file and version.");
	if (pInfo)
	{
		m_eUniqueUnit = eUnit;
	}
}

/// Picks a unique unit for Militaristic city-states
void CvMinorCivAI::DoPickUniqueUnit()
{
	m_eUniqueUnit = NO_UNIT;
	if (GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
	{
		// Units from our starting era or before would be no fun because players won't get the chance to use them
		m_eUniqueUnit = GC.getGame().GetRandomUniqueUnitType(/*bIncludeCivsInGame*/false, /*bIncludeStartEraUnits*/false, /*bIncludeOldEras*/false, /*bIncludeRanged*/true);
	}
}


// ******************************
// Main functions
// ******************************


/// Processed every turn
void CvMinorCivAI::DoTurn()
{
	AI_PERF_FORMAT("AI-perf.csv", ("CvMinorCivAI::DoTurn, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), GetPlayer()->getCivilizationShortDescription()) );
	if(GetPlayer()->isMinorCiv())
	{
		DoTurnStatus();

		DoElection();
		DoFriendship();

		DoTestThreatenedAnnouncement();
		DoTestProxyWarAnnouncement();

		DoTurnQuests();

		DoUnitSpawnTurn();

		DoIntrusion();
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
		if (MOD_DIPLOMACY_CITYSTATES_QUESTS) {
			if(IsSacked())
			{
				DoSack();
			}
			if(IsRebellion())
			{
				DoDefection();
			}
			if(GetTurnsSinceRebellion() > 0)
			{
				int iRebellionSpawn = GetTurnsSinceRebellion();

				ChangeTurnsSinceRebellion(-1);

				//Rebel Spawn - once every 4 turns (on default speed)
				if ((iRebellionSpawn == /*16*/(GC.getMINOR_QUEST_REBELLION_TIMER() * 80) / 100)
				|| (iRebellionSpawn == /*12*/(GC.getMINOR_QUEST_REBELLION_TIMER() * 60) / 100)
				|| (iRebellionSpawn == /*8*/ (GC.getMINOR_QUEST_REBELLION_TIMER() * 40) / 100)
				|| (iRebellionSpawn == /*4*/ (GC.getMINOR_QUEST_REBELLION_TIMER() * 20) / 100))
				{
					GetPlayer()->GetMinorCivAI()->DoRebellion();
				}
			}
		}
#endif
#if defined(MOD_BALANCE_CORE_MINORS)
	if (MOD_BALANCE_CORE_MINORS) 
	{
		TeamTypes eLoopTeam;
		for(int iTeamLoop = 0; iTeamLoop < REALLY_MAX_TEAMS; iTeamLoop++)
		{
			eLoopTeam = (TeamTypes) iTeamLoop;
			if(IsJerk(eLoopTeam))
			{
				ChangeJerk(eLoopTeam, -1);
				if(GetJerk(eLoopTeam) <= 0)
				{
					SetIsJerk(eLoopTeam, false);
				}
			}
		}
	}
#endif
	}
}


/// Minor is now dead or alive (haha, get it?)
void CvMinorCivAI::DoChangeAliveStatus(bool bAlive)
{
	if (!bAlive)
	{
		// Final check for quests
		DoTestActiveQuests(/*bTestComplete*/ true, /*bTestObsolete*/ true);

		std::vector<int> vNewInfluence;
		for (int i = 0; i < MAX_MAJOR_CIVS; ++i)
		{
			PlayerTypes e = (PlayerTypes)i;

			// Cancel quests and PtPs
			DoChangeProtectionFromMajor(e, false);
			EndAllActiveQuestsForPlayer(e);

			// Calculate new influence levels (don't set here, since that could create a false temporary ally)
			int iOldInfluence = GetBaseFriendshipWithMajor(e);
			int iNewInfluence = iOldInfluence;
			if (IsFriendshipAboveAlliesThreshold(iOldInfluence))
			{
				iNewInfluence = GC.getFRIENDSHIP_ALLIES_ON_DEATH();
			}
			else if (IsFriendshipAboveFriendsThreshold(iOldInfluence))
			{
				iNewInfluence = GC.getFRIENDSHIP_FRIENDS_ON_DEATH();
			}
			else if (iOldInfluence > GC.getFRIENDSHIP_THRESHOLD_NEUTRAL())
			{
				iNewInfluence = GC.getFRIENDSHIP_NEUTRAL_ON_DEATH();
			}
			vNewInfluence.push_back(iNewInfluence);
		}

		// Set new influence values
		SetDisableNotifications(true);
		for (unsigned int i = 0; i < vNewInfluence.size(); ++i)
		{
			PlayerTypes e = (PlayerTypes)i;
			// special workaround to allow status changes despite minor already being dead
			DoFriendshipChangeEffects(e, GetEffectiveFriendshipWithMajor(e), vNewInfluence.at(i), /*bFromQuest*/false, /*bIgnoreMinorDeath*/true);
			SetFriendshipWithMajor(e, vNewInfluence.at(i));
		}
		SetDisableNotifications(false);
	}

	// Apply or Remove any active bonuses
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iPlayerLoop;

		bool bFriends = false;
		bool bAllies = false;
		if(IsFriendshipAboveFriendsThreshold(GetEffectiveFriendshipWithMajor(ePlayer)))
		{
			bFriends = true;
		}
		if(GetAlly() == ePlayer)
		{
			CvAssertMsg(bAlive, "A Minor about to die still has an Ally, when it should have none.");
			bAllies = true;
		}
		if(bFriends || bAllies)
		{
			DoSetBonus(ePlayer, bAlive, bFriends, bAllies);
		}
	}

	// Death - Reset the cached ally and barbarian threat counter
	if (!bAlive)
	{
		bool bHasAlly = GetAlly() != NO_PLAYER;
		CvAssertMsg(!bHasAlly, "A Minor about to die still has an Ally, when it should have none.");
		if(bHasAlly)
		{
			SetAlly(NO_PLAYER);
		}

		SetTurnsSinceThreatenedAnnouncement(-1);
	}
}


/// First contact
void CvMinorCivAI::DoFirstContactWithMajor(TeamTypes eTeam, bool bSuppressMessages)
{
	// Set intrusion flag (used for unit movement)
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iPlayerLoop;
		if(GET_PLAYER(ePlayer).getTeam() == eTeam)
		{
			if(GET_PLAYER(ePlayer).isHuman())
			{
				// Humans intrude automatically upon meeting (allowing them to enter our territory if they choose)
				SetMajorIntruding(ePlayer, true);
			}
			else
			{
				SetMajorIntruding(ePlayer, false);
			}
		}
	}

	// This guy's a warmonger or at war with our ally, so we DoW him
	if(IsPeaceBlocked(eTeam))
	{
#if defined(MOD_EVENTS_WAR_AND_PEACE)
		GET_TEAM(GetPlayer()->getTeam()).declareWar(eTeam, false, GetPlayer()->GetID());
#else
		GET_TEAM(GetPlayer()->getTeam()).declareWar(eTeam);
#endif
	}
	// Normal diplo
	else
	{
#if defined(MOD_GLOBAL_CS_GIFTS)
		// Ideally we want the actual meeting player, but we'll have to settle for the lead player of the team
		// most of the time they will be one and the same
		PlayerTypes eMeetingPlayer = GET_TEAM(eTeam).getLeaderID();
		CvPlayer* pMeetingPlayer = &GET_PLAYER(eMeetingPlayer);
		if (!(pMeetingPlayer->isMinorCiv() || pMeetingPlayer->isBarbarian())) {
			MinorCivTraitTypes eTrait = GetTrait();
			MinorCivPersonalityTypes eRealPersonality = GetPersonality();
			MinorCivPersonalityTypes eFakePersonality = eRealPersonality;

			int iFriendshipBoost = 0;
			int iCultureGift = 0;
			int iFoodGift = 0;
			int iUnitGift = 0;

			int iGift = 0;
			char* szTxtKeySuffix = "UNKNOWN";
#endif
			int iGoldGift = 0;
			int iFaithGift = 0;
			bool bFirstMajorCiv = false;

			// If this guy has been mean then no Gold gifts
			if(!GET_TEAM(eTeam).IsMinorCivAggressor())
			{
#if defined(MOD_GLOBAL_CS_GIFTS)
				// Gift depends on the type of city state met
				if (MOD_GLOBAL_CS_GIFTS) {
					bFirstMajorCiv = (GET_TEAM(GetPlayer()->getTeam()).getHasMetCivCount(true) == 0);
					iFriendshipBoost = GC.getMINOR_CIV_FIRST_CONTACT_BONUS_FRIENDSHIP(); // All CS types give an initial friendship boost

					if (eTrait == MINOR_CIV_TRAIT_CULTURED) {
						iCultureGift = GC.getMINOR_CIV_FIRST_CONTACT_BONUS_CULTURE();
						szTxtKeySuffix = "CULTURE";
					} else if (eTrait == MINOR_CIV_TRAIT_RELIGIOUS) {
						iFaithGift = GC.getMINOR_CIV_FIRST_CONTACT_BONUS_FAITH();
						szTxtKeySuffix = "FAITH";
					} else if (eTrait == MINOR_CIV_TRAIT_MERCANTILE) {
						iGoldGift = GC.getMINOR_CIV_FIRST_CONTACT_BONUS_GOLD();
						szTxtKeySuffix = "GOLD";
					} else if (eTrait == MINOR_CIV_TRAIT_MARITIME) {
						iFoodGift = GC.getMINOR_CIV_FIRST_CONTACT_BONUS_FOOD();
						szTxtKeySuffix = "FOOD";
					} else if (eTrait == MINOR_CIV_TRAIT_MILITARISTIC) {
						iUnitGift = GC.getMINOR_CIV_FIRST_CONTACT_BONUS_UNIT();
						szTxtKeySuffix = "UNIT";
					}
				}
#endif

				// Hasn't met anyone yet?
				if(GET_TEAM(GetPlayer()->getTeam()).getHasMetCivCount(true) == 0)
				{
#if defined(MOD_GLOBAL_CS_GIFTS)
					// If we're not using the new gift system, just stick with the default gold and faith gifts
					if (!MOD_GLOBAL_CS_GIFTS) {
#endif
						iGoldGift = /*60*/ GC.getMINOR_CIV_CONTACT_GOLD_FIRST();
						if(GetTrait() == MINOR_CIV_TRAIT_RELIGIOUS)
							iFaithGift = 8; //antonjs: todo: XML
#if defined(MOD_GLOBAL_CS_GIFTS)
					}
#endif
					
					bFirstMajorCiv = true;
				}
				else
				{
#if defined(MOD_GLOBAL_CS_GIFTS)
					if (MOD_GLOBAL_CS_GIFTS) {
						// Reduce gifts if we're not the first team to meet the CS
						int iBonusMultiplier = GC.getMINOR_CIV_FIRST_CONTACT_SUBSEQUENT_TEAM_MULTIPLIER();
						int iBonusDivisor = GC.getMINOR_CIV_FIRST_CONTACT_SUBSEQUENT_TEAM_DIVISOR();

						if (iBonusDivisor == 0) {
							iFriendshipBoost = 0;
						} else {
							iFriendshipBoost = iFriendshipBoost * iBonusMultiplier / iBonusDivisor;
						}

						iCultureGift = iCultureGift / iBonusDivisor;
						iFaithGift = iFaithGift / iBonusDivisor;

						iGoldGift = 0;
						iFoodGift = 0;
						iUnitGift = 0;
					} else {
#endif
						iGoldGift = /*30*/ GC.getMINOR_CIV_CONTACT_GOLD_OTHER();
						if(GetTrait() == MINOR_CIV_TRAIT_RELIGIOUS)
							iFaithGift = 4; //antonjs: todo: XML
#if defined(MOD_GLOBAL_CS_GIFTS)
					}
#endif
				}

#if defined(MOD_GLOBAL_CS_GIFTS)
				if (MOD_GLOBAL_CS_GIFTS) {
					if (eRealPersonality == MINOR_CIV_PERSONALITY_IRRATIONAL) {
						// Assumes MINOR_CIV_PERSONALITY_IRRATIONAL is the last entry in the enum
						eFakePersonality = (MinorCivPersonalityTypes) GC.getGame().getJonRandNum(NUM_MINOR_CIV_PERSONALITY_TYPES-1, "Minor Civ AI: Picking fake personality for irrational CS");
					}
					
		 			// Personality modifiers - friendly = x1.5, hostile = x0.5
					if (eFakePersonality == MINOR_CIV_PERSONALITY_FRIENDLY) {
						int iBonusMultiplier = GC.getMINOR_CIV_FIRST_CONTACT_FRIENDLY_BONUS_MULTIPLIER();
						int iBonusDivisor = GC.getMINOR_CIV_FIRST_CONTACT_FRIENDLY_BONUS_DIVISOR();

						if (iBonusDivisor == 0) {
							iFriendshipBoost = 0;
							
							iCultureGift = 0;
							iFaithGift = 0;
							iGoldGift = 0;
							iFoodGift = 0;
						} else {
							iFriendshipBoost = iFriendshipBoost * iBonusMultiplier / iBonusDivisor;
							
							iCultureGift = iCultureGift * iBonusMultiplier / iBonusDivisor;
							iFaithGift = iFaithGift * iBonusMultiplier / iBonusDivisor;
							iGoldGift = iGoldGift * iBonusMultiplier / iBonusDivisor;
							iFoodGift = iFoodGift * iBonusMultiplier / iBonusDivisor;
						}

						int iUnitMultiplier = GC.getMINOR_CIV_FIRST_CONTACT_FRIENDLY_UNIT_MULTIPLIER();
						int iUnitDivisor = GC.getMINOR_CIV_FIRST_CONTACT_FRIENDLY_UNIT_DIVISOR();

						if (iUnitDivisor == 0) {
							iUnitGift = 0;
						} else {
							iUnitGift = iUnitGift * iUnitMultiplier / iUnitDivisor;
						}
					} else if (eFakePersonality == MINOR_CIV_PERSONALITY_HOSTILE) {
						int iBonusMultiplier = GC.getMINOR_CIV_FIRST_CONTACT_HOSTILE_BONUS_MULTIPLIER();
						int iBonusDivisor = GC.getMINOR_CIV_FIRST_CONTACT_HOSTILE_BONUS_DIVISOR();

						if (iBonusDivisor == 0) {
							iFriendshipBoost = 0;
							
							iCultureGift = 0;
							iFaithGift = 0;
							iGoldGift = 0;
							iFoodGift = 0;
						} else {
							iFriendshipBoost = iFriendshipBoost * iBonusMultiplier / iBonusDivisor;

							iCultureGift = iCultureGift * iBonusMultiplier / iBonusDivisor;
							iFaithGift = iFaithGift * iBonusMultiplier / iBonusDivisor;
							iGoldGift = iGoldGift * iBonusMultiplier / iBonusDivisor;
							iFoodGift = iFoodGift * iBonusMultiplier / iBonusDivisor;
						}

						int iUnitMultiplier = GC.getMINOR_CIV_FIRST_CONTACT_HOSTILE_UNIT_MULTIPLIER();
						int iUnitDivisor = GC.getMINOR_CIV_FIRST_CONTACT_HOSTILE_UNIT_DIVISOR();

						if (iUnitDivisor == 0) {
							iUnitGift = 0;
						} else {
							iUnitGift = iUnitGift * iUnitMultiplier / iUnitDivisor;
						}
					}
				}
#endif
			}

#if defined(MOD_GLOBAL_CS_GIFTS)
			if (MOD_GLOBAL_CS_GIFTS) {
				CUSTOMLOG("CS Gift: Id %i, Trait %i, Personality %i: %sFriendship=%i, Gold=%i, Culture=%i, Faith=%i, Food=%i, Unit=%i", GetPlayer()->GetID(), eTrait, eRealPersonality, (bFirstMajorCiv ? "First " : ""), iFriendshipBoost, iGoldGift, iCultureGift, iFaithGift, iFoodGift, iUnitGift);
			}
#endif

	 		PlayerTypes ePlayer;
 			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				ePlayer = (PlayerTypes) iPlayerLoop;

				if(GET_PLAYER(ePlayer).getTeam() == eTeam)
				{
#if defined(MOD_GLOBAL_CS_GIFTS)
					if (MOD_GLOBAL_CS_GIFTS) {
						// Give the friendship boost to this team member
						ChangeFriendshipWithMajor(ePlayer, iFriendshipBoost, /*bFromQuest*/ false);

						// Give the gifts to this team member
						GET_PLAYER(ePlayer).GetTreasury()->ChangeGold(iGoldGift);
						GET_PLAYER(ePlayer).changeJONSCulture(iCultureGift);
						GET_PLAYER(ePlayer).ChangeFaith(iFaithGift);

						iGift = iGoldGift + iCultureGift + iFaithGift;

						// Food and unit gifts only go to the player who actually met the CS
						if (eMeetingPlayer == ePlayer) {
							int iBonusMultiplier = GC.getMINOR_CIV_FIRST_CONTACT_PLAYER_MULTIPLIER();
							int iBonusDivisor = GC.getMINOR_CIV_FIRST_CONTACT_PLAYER_DIVISOR();

							// Give the friendship boost again to this team member (ie the meeting player gets twice the friendship boost)
							int iExtraFriendship = (iFriendshipBoost * iBonusMultiplier / iBonusDivisor) - iFriendshipBoost;
							ChangeFriendshipWithMajor(ePlayer, iExtraFriendship, /*bFromQuest*/ false);
							iFriendshipBoost = iFriendshipBoost + iExtraFriendship;  // Need this adjusting for the popup dialog

							CvPlayer* pPlayer = &GET_PLAYER(ePlayer);
							if (eTrait == MINOR_CIV_TRAIT_MILITARISTIC) {
								if (iUnitGift > 0) {
									if (GC.getGame().getJonRandNum(100, "Minor Civ AI: Decide if we give a unit to the meeting player") < iUnitGift) {
										CvUnit* pUnit = DoSpawnUnit(ePlayer, true, true);
										if (pUnit != NULL) {
											pUnit->changeExperience(pPlayer->GetCurrentEra() * GC.getMINOR_CIV_FIRST_CONTACT_XP_PER_ERA() + GC.getGame().getJonRandNum(GC.getMINOR_CIV_FIRST_CONTACT_XP_RANDOM(), "Minor Civ AI: Random XP for unit"));
											iGift = pUnit->getUnitType();
										}
									}
								}
							} else if (eTrait == MINOR_CIV_TRAIT_MARITIME && pPlayer->getCapitalCity() != NULL) {
								iGift = iGift + iFoodGift;

								if (iFoodGift > 0) {
									CvCity* pMinorCapital = GetPlayer()->getCapitalCity();
									CvPlot* pPlot = (pMinorCapital == NULL) ? GetPlayer()->getStartingPlot() : pMinorCapital->plot();

									CvCity* pBestCity = NULL;

									if (pPlayer->GetCurrentEra() < GC.getInfoTypeForString("ERA_MEDIEVAL") && pPlayer->getCapitalCity()->plot()->getArea() == pPlot->getArea()) {
										// Pre-Medieval and on the same landmass, just add the food to the capital
										pBestCity = pPlayer->getCapitalCity();
									} else {
										// Ripped from CvPlayer::receiveGoody()
										int iDistance;
										int iBestCityDistance = -1;

										CvCity* pLoopCity;
										int iLoop;
										// Find the closest City to us
										for (pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop)) {
											iDistance = plotDistance(pPlot->getX(), pPlot->getY(), pLoopCity->getX(), pLoopCity->getY());

											if (iBestCityDistance == -1 || iDistance < iBestCityDistance) {
												iBestCityDistance = iDistance;
												pBestCity = pLoopCity;
											}
										}
									}

									if (pBestCity != NULL) {
										pBestCity->changeFood(iFoodGift);
									}
								}
							}
						}
					} else {
#endif
						// Gold gift
						GET_PLAYER(ePlayer).GetTreasury()->ChangeGold(iGoldGift);

						// Faith gift
						if(iFaithGift > 0)
							GET_PLAYER(ePlayer).ChangeFaith(iFaithGift);
#if defined(MOD_GLOBAL_CS_GIFTS)
					}
#endif

					// Need to seed quest counter?
					if(GC.getGame().getElapsedGameTurns() > GetFirstPossibleTurnForPersonalQuests())
					{
						DoTestSeedQuestCountdownForPlayer(ePlayer);
					}

					// See if Threatening Barbarians event is active
					if(GetTurnsSinceThreatenedAnnouncement() >= 0 && GetTurnsSinceThreatenedAnnouncement() < 10)
					{
						DoTestThreatenedAnnouncementForPlayer(ePlayer);
					}

					// See if Proxy War event is active
					DoTestProxyWarAnnouncementOnFirstContact(ePlayer);

					// See if there are any quests you can join now
					DoTestQuestsOnFirstContact(ePlayer);

					// Greeting for active human player
					if(ePlayer == GC.getGame().getActivePlayer() && !bSuppressMessages)
					{
						if(!GC.getGame().isNetworkMultiPlayer())	// KWG: Should this be !GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS)
						{
#if defined(MOD_GLOBAL_CS_GIFTS)
							/* Current assignments of members of popupInfo
							 *   Data1 is the player id
							 *   Data2 is the gold gift value
							 *   Data3 is the faith gift value
							 *   Option1 is first met
							 *   Option2 is nil
							 *   Text is nil
							 *
							 * Updated assignments of members of popupInfo
							 *   Data1 is the player id (unchanged)
							 *   Data2 is the gift "value" (Gold/Culture/Faith/Food amount, UnitId)
							 *   Data3 is the friendship boost
							 *   Option1 is first met (unchanged)
							 *   Option2 is nil (unchanged)
							 *   Text is suffix for the TXT_KEY_ to format with
							 */
							if (MOD_GLOBAL_CS_GIFTS) {
								CvPopupInfo kPopupInfo(BUTTONPOPUP_CITY_STATE_GREETING, GetPlayer()->GetID(), iGift, iFriendshipBoost, 0, bFirstMajorCiv);
								strcpy_s(kPopupInfo.szText, szTxtKeySuffix);
								GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
							} else {
#endif
								CvPopupInfo kPopupInfo(BUTTONPOPUP_CITY_STATE_GREETING, GetPlayer()->GetID(), iGoldGift, iFaithGift, 0, bFirstMajorCiv);
								GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
#if defined(MOD_GLOBAL_CS_GIFTS)
							}
#endif
							// We are adding a popup that the player must make a choice in, make sure they are not in the end-turn phase.
							CancelActivePlayerEndTurn();
						}

						// update the mouseover text for the city-state's city banners
						int iLoop = 0;
						CvCity* pLoopCity = NULL;
						for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
						{
							if(pLoopCity->plot()->isRevealed(eTeam))
							{
								auto_ptr<ICvCity1> pDllLoopCity = GC.WrapCityPointer(pLoopCity);
								GC.GetEngineUserInterface()->SetSpecificCityInfoDirty(pDllLoopCity.get(), CITY_UPDATE_TYPE_BANNER);
							}
						}
					}
				}
			}
#if defined(MOD_GLOBAL_CS_GIFTS)
		}
#endif
	}
}

/// Are we at war with a minor and not allied with anyone?
void CvMinorCivAI::DoTestEndWarsVSMinors(PlayerTypes eOldAlly, PlayerTypes eNewAlly)
{
	if(eOldAlly == NO_PLAYER)
		return;

	if(!GetPlayer()->isAlive())
		return;

	PlayerTypes eOtherMinor;
	int iOtherMinorLoop;
	PlayerTypes eOtherAlly;
	bool bForcedWar;

	TeamTypes eLoopTeam;
	for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
	{
		eLoopTeam = (TeamTypes) iTeamLoop;

		// Another Minor
		if(!GET_TEAM(eLoopTeam).isMinorCiv())
			continue;

		// They not alive!
		if(!GET_TEAM(eLoopTeam).isAlive())
			continue;

		// At war with him
		if(!GET_TEAM(GetPlayer()->getTeam()).isAtWar(eLoopTeam))
			continue;

		if(eOldAlly != NO_PLAYER)
		{
			// Old ally wasn't at war
			if(!GET_TEAM(GET_PLAYER(eOldAlly).getTeam()).isAtWar(eLoopTeam))
				continue;
		}

		if(eNewAlly != NO_PLAYER)
		{
			// New ally IS at war
			if(GET_TEAM(GET_PLAYER(eNewAlly).getTeam()).isAtWar(eLoopTeam))
				continue;
		}

		// Make sure this guy isn't allied with someone at war with us
		bForcedWar = false;

		for(iOtherMinorLoop = 0; iOtherMinorLoop < MAX_CIV_TEAMS; iOtherMinorLoop++)
		{
			eOtherMinor = (PlayerTypes) iOtherMinorLoop;

			// Other minor is on this team
			if(GET_PLAYER(eOtherMinor).getTeam() == eLoopTeam)
			{
				eOtherAlly = GET_PLAYER(eOtherMinor).GetMinorCivAI()->GetAlly();
				if(eOtherAlly != NO_PLAYER)
				{
					// This guy's ally at war with us?
					if(GET_TEAM(GET_PLAYER(eOtherAlly).getTeam()).isAtWar(GetPlayer()->getTeam()))
					{
						bForcedWar = true;
						break;
					}
				}
			}

		}

		if(bForcedWar)
			continue;

		if(IsPermanentWar(eLoopTeam))
			continue;

#if defined(MOD_EVENTS_WAR_AND_PEACE)
		GET_TEAM(GetPlayer()->getTeam()).makePeace(eLoopTeam, true, false, GetPlayer()->GetID());
#else
		GET_TEAM(GetPlayer()->getTeam()).makePeace(eLoopTeam);
#endif
	}
}

#if defined(MOD_GLOBAL_CS_NO_ALLIED_SKIRMISHES)
/// Are we at war with a minor also allied to our new BFF?
void CvMinorCivAI::DoTestEndSkirmishes(PlayerTypes eNewAlly)
{
	if(eNewAlly != NO_PLAYER)
	{
		if(!GetPlayer()->isAlive())
			return;

		for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
		{
			TeamTypes eLoopTeam = (TeamTypes) iTeamLoop;

			// Another Minor
			if(!GET_TEAM(eLoopTeam).isMinorCiv())
				continue;

			// They're not alive!
			if(!GET_TEAM(eLoopTeam).isAlive())
				continue;

			// At war with them
			if(!GET_TEAM(GetPlayer()->getTeam()).isAtWar(eLoopTeam))
				continue;

			if(IsPermanentWar(eLoopTeam))
				continue;

			// New ally IS at war (how???)
			if(GET_TEAM(GET_PLAYER(eNewAlly).getTeam()).isAtWar(eLoopTeam))
				continue;

			for(int iOtherMinorLoop = MAX_MAJOR_CIVS; iOtherMinorLoop < MAX_CIV_PLAYERS; iOtherMinorLoop++)
			{
				PlayerTypes eOtherMinor = (PlayerTypes) iOtherMinorLoop;
				CvPlayer& kOtherMinor = GET_PLAYER(eOtherMinor);

				// Other minor is on this team
				if(kOtherMinor.isAlive() && kOtherMinor.getTeam() == eLoopTeam)
				{
					if(kOtherMinor.GetMinorCivAI()->GetAlly() == eNewAlly)
					{
						// We are at war with our new ally's ally!
						CUSTOMLOG("CS %i is at war with CS %i but they share the same ally %i - making peace", GetPlayer()->GetID(), iOtherMinorLoop, ((int) eNewAlly));
#if defined(MOD_EVENTS_WAR_AND_PEACE)
						GET_TEAM(GetPlayer()->getTeam()).makePeace(eLoopTeam, true, false, GetPlayer()->GetID());
#else
						GET_TEAM(GetPlayer()->getTeam()).makePeace(eLoopTeam);
#endif
					}
				}
			}
		}
	}
}
#endif

/// Update what our status is
void CvMinorCivAI::DoTurnStatus()
{
	int iWeight = 0;

	PlayerProximityTypes eProximity;

	CvPlayer* pPlayer;
	CvTeam* pTeam;
	PlayerTypes ePlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;
		pPlayer = &GET_PLAYER(ePlayer);
		pTeam = &GET_TEAM(pPlayer->getTeam());

		eProximity = pPlayer->GetProximityToPlayer(GetPlayer()->GetID());

		// Check how close the player is
		switch(eProximity)
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
			if(pTeam->IsMinorCivAggressor())
			{
				iWeight += 10;
			}
			if(pTeam->IsMinorCivWarmonger())
			{
				iWeight += 20;
			}
			break;

			// NEIGHBORS: Pretty much anything makes the situation critical
		case PLAYER_PROXIMITY_NEIGHBORS:
			if(pTeam->IsMinorCivAggressor())
			{
				iWeight += 20;
			}
			if(IsAtWarWithPlayersTeam(ePlayer))
			{
				iWeight += 20;
			}
			break;
		default:
			break;
		}
	}

	// Do the final math
	if(iWeight >= 20)
	{
		m_eStatus = MINOR_CIV_STATUS_CRITICAL;
	}
	else if(iWeight >= 10)
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

/// We have a new city! Add any special starting resources we get.
void CvMinorCivAI::DoAddStartingResources(CvPlot* pCityPlot)
{
	CvAssertMsg(pCityPlot != NULL, "City's plot should not be NULL. Please send Anton your save file and version.");
	if (pCityPlot == NULL) return;

	MinorCivTraitTypes eTrait = GetTrait();

	// Mercantile
	if (eTrait == MINOR_CIV_TRAIT_MERCANTILE)
	{
		bool bAddUniqueLuxury = true;
		
		// Only for our first city
		if (GetPlayer()->getNumCities() > 1)
		{
			bAddUniqueLuxury = false;
		}
	
		// Only if there isn't already a unique luxury on this plot
		ResourceTypes ePlotResource = pCityPlot->getResourceType();
		if (ePlotResource != NO_RESOURCE)
		{
			CvResourceInfo* pkPlotResourceInfo = GC.getResourceInfo(ePlotResource);
			if (pkPlotResourceInfo && pkPlotResourceInfo->isOnlyMinorCivs())
			{
				bAddUniqueLuxury = false;
			}
		}

		// Pick and add a unique luxury
		if (bAddUniqueLuxury)
		{
			FStaticVector< ResourceTypes, 64, true, c_eCiv5GameplayDLL > veUniqueLuxuries;
			for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				const ResourceTypes eResourceLoop = (ResourceTypes) iResourceLoop;
				CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResourceLoop);
				if(pkResourceInfo)
				{
					if(pkResourceInfo->isOnlyMinorCivs())
					{
						veUniqueLuxuries.push_back(eResourceLoop);
					}
				}
			}

			pCityPlot->setResourceType(NO_RESOURCE, 0, true);
			if (veUniqueLuxuries.size() > 0)
			{
				int iRoll = GC.getGame().getJonRandNum(veUniqueLuxuries.size(), "Rolling for Mercantile city-state special luxury"); // range = [0, size - 1]
				int iQuantity = GC.getMINOR_CIV_MERCANTILE_RESOURCES_QUANTITY();
				ResourceTypes eSpecialLuxury = veUniqueLuxuries[iRoll];

				pCityPlot->setResourceType(eSpecialLuxury, iQuantity, true);
			}
		}
	}
}

/// Our city got destroyed or taken! Do cleanup for special starting resources we had.
#if defined(MOD_GLOBAL_VENICE_KEEPS_RESOURCES)
void CvMinorCivAI::DoRemoveStartingResources(CvPlot* pCityPlot, bool bVenice)
#else
void CvMinorCivAI::DoRemoveStartingResources(CvPlot* pCityPlot)
#endif
{
	CvAssertMsg(pCityPlot != NULL, "City's plot should not be NULL. Please send Anton your save file and version.");
	if (pCityPlot == NULL) return;

	MinorCivTraitTypes eTrait = GetTrait();

	// Mercantile
	if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
	{
#if defined(MOD_GLOBAL_VENICE_KEEPS_RESOURCES)
		pCityPlot->removeMinorResources(bVenice);
#else
		bool bRemoveUniqueLuxury = false;

		if (GC.getMINOR_CIV_MERCANTILE_RESOURCES_KEEP_ON_CAPTURE_DISABLED() == 1)
			bRemoveUniqueLuxury = true;

		if (bRemoveUniqueLuxury)
		{
			ResourceTypes eOldResource = pCityPlot->getResourceType();
			if (eOldResource != NO_RESOURCE)
			{
				CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eOldResource);
				if (pkResourceInfo && pkResourceInfo->isOnlyMinorCivs())
				{
					pCityPlot->setResourceType(NO_RESOURCE, 0, true);
				}
			}
		}
#endif
	}
}

/// Notifications
void CvMinorCivAI::AddNotification(CvString sString, CvString sSummaryString, PlayerTypes ePlayer, int iX, int iY)
{
	if(iX == -1 && iY == -1)
	{
		CvCity* capCity = GetPlayer()->getCapitalCity();

		if(capCity != NULL)
		{
			iX = capCity->getX();
			iY = capCity->getY();
		}
	}

	CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
	if(pNotifications)
	{
		pNotifications->Add(NOTIFICATION_MINOR, sString, sSummaryString, iX, iY, GetPlayer()->GetID());
	}
}

/// Quest Notifications
void CvMinorCivAI::AddQuestNotification(CvString sString, CvString sSummaryString, PlayerTypes ePlayer, int iX, int iY, bool bNewQuest)
{
	CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
	if(pNotifications)
	{
		sString += "[NEWLINE][NEWLINE]";
		sString += Localization::Lookup("TXT_KEY_MINOR_QUEST_BLOCKING_TT").toUTF8();

		if (bNewQuest)
			pNotifications->Add(NOTIFICATION_MINOR_QUEST, sString, sSummaryString, iX, iY, GetPlayer()->GetID(), 1);
		else
			pNotifications->Add(NOTIFICATION_MINOR_QUEST, sString, sSummaryString, iX, iY, GetPlayer()->GetID());
	}
}

/// Buyout notifications
void CvMinorCivAI::AddBuyoutNotification(CvString sString, CvString sSummaryString, PlayerTypes ePlayer, int iX, int iY)
{
	if(iX == -1 && iY == -1)
	{
		CvCity* capCity = GetPlayer()->getCapitalCity();

		if(capCity != NULL)
		{
			iX = capCity->getX();
			iY = capCity->getY();
		}
	}

	CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
	if(pNotifications)
	{
		pNotifications->Add(NOTIFICATION_MINOR_BUYOUT, sString, sSummaryString, iX, iY, GetPlayer()->GetID());
	}
}

// ******************************
// Threatened by Barbarians event
// ******************************

/// Are there threatening barbarians that ePlayer can get credit for clearing out?
/// NOTE: This event can be active even if we haven't sent ePlayer a notification (ex. is only one barb, ePlayer is far away)
bool CvMinorCivAI::IsThreateningBarbariansEventActiveForPlayer(PlayerTypes ePlayer)
{
	if (GetPlayer()->isAlive())
	{
		if (IsHasMetPlayer(ePlayer) && !IsAtWarWithPlayersTeam(ePlayer))
		{
			if (GetNumThreateningBarbarians() > 0)
			{
				return true;
			}
		}
	}
	
	return false;
}

/// Barbs in or near our borders?
int CvMinorCivAI::GetNumThreateningBarbarians()
{
	int iCount = 0;

	int iLoop;
	for(CvUnit* pLoopUnit = GET_PLAYER(BARBARIAN_PLAYER).firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = GET_PLAYER(BARBARIAN_PLAYER).nextUnit(&iLoop))
	{
		if(pLoopUnit->IsBarbarianUnitThreateningMinor(GetPlayer()->GetID()))
			iCount++;
	}

	return iCount;
}

/// Barbarians threatening this Minor?
void CvMinorCivAI::DoTestThreatenedAnnouncement()
{
	// Increment counter - this is only used when sending notifications to players
	if(GetTurnsSinceThreatenedAnnouncement() >= 0)
	{
		ChangeTurnsSinceThreatenedAnnouncement(1);

		// Long enough to have expired?
		if(GetTurnsSinceThreatenedAnnouncement() >= 30)
			SetTurnsSinceThreatenedAnnouncement(-1);
	}

	// Not already threatened?
	if(GetTurnsSinceThreatenedAnnouncement() == -1)
	{
		if(GetNumThreateningBarbarians() >= GC.getMINOR_CIV_THREATENED_BY_BARBS_NOTIF_THRESHOLD())
		{
			// Wasn't under attack before, but is now!
			SetTurnsSinceThreatenedAnnouncement(0);

			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				DoTestThreatenedAnnouncementForPlayer((PlayerTypes) iPlayerLoop);
			}
		}
	}
}

/// How long has this Minor been under attack from Barbs?
int CvMinorCivAI::GetTurnsSinceThreatenedAnnouncement() const
{
	return m_iTurnsSinceThreatenedByBarbarians;
}

/// How long has this Minor been under attack from Barbs?
void CvMinorCivAI::SetTurnsSinceThreatenedAnnouncement(int iValue)
{
	if(GetTurnsSinceThreatenedAnnouncement() != iValue)
		m_iTurnsSinceThreatenedByBarbarians = iValue;
}

/// How long has this Minor been under attack from Barbs?
void CvMinorCivAI::ChangeTurnsSinceThreatenedAnnouncement(int iChange)
{
	SetTurnsSinceThreatenedAnnouncement(GetTurnsSinceThreatenedAnnouncement() + iChange);
}

/// Barbarians are invading...ask ePlayer for help!
void CvMinorCivAI::DoTestThreatenedAnnouncementForPlayer(PlayerTypes ePlayer)
{
	if (GetPlayer()->isAlive())
	{
		CvCity* pCapital = GetPlayer()->getCapitalCity();
		if (pCapital)
		{
			int iX = pCapital->getX();
			int iY = pCapital->getY();
			if (IsThreateningBarbariansEventActiveForPlayer(ePlayer))
			{
				// Is ePlayer is close to us?
				if (IsPlayerCloseEnoughForThreatenedAnnouncement(ePlayer))
				{
					// Are there enough barbs to warrant sending a message about it?
					if (GetNumThreateningBarbarians() >= GC.getMINOR_CIV_THREATENED_BY_BARBS_NOTIF_THRESHOLD())
					{
						Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BARBS_QUEST");
						strMessage << GetPlayer()->getNameKey();
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_BARBS_QUEST");
						strSummary << GetPlayer()->getNameKey();
						AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), ePlayer, iX, iY);
					}
				}
			}
		}
	}
}

/// Is player close enough for the game to send a notification about the barbarian threat?
bool CvMinorCivAI::IsPlayerCloseEnoughForThreatenedAnnouncement(PlayerTypes eMajor)
{
	CvCity* pCapital = GetPlayer()->getCapitalCity();

	// Minor must have Capital
	if(pCapital == NULL)
	{
		return false;
	}

	// Has Minor met this player yet?
	if(IsHasMetPlayer(eMajor))
	{
		bool bCloseEnoughForQuest = false;

		CvCity* pMajorsCapital = GET_PLAYER(eMajor).getCapitalCity();

		if(pMajorsCapital != NULL)
		{
			if(pCapital->getArea() == pMajorsCapital->getArea())
			{
				return true;
			}

			if(!bCloseEnoughForQuest)
			{
				int iDistance = plotDistance(pCapital->getX(), pCapital->getY(), pMajorsCapital->getX(), pMajorsCapital->getY());

				if(iDistance <= /*50*/ GC.getMAX_DISTANCE_MINORS_BARB_QUEST())
				{
					return true;
				}
			}
		}
	}

	return false;
}

/// Player killed a threatening barb, so reward him!
/// NOTE: Player can get credit even if we haven't sent him a notification (ex. is only one barb, player is far away)
void CvMinorCivAI::DoThreateningBarbKilled(PlayerTypes eKillingPlayer, int iX, int iY)
{
	CvAssertMsg(eKillingPlayer >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eKillingPlayer < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");

	if (IsThreateningBarbariansEventActiveForPlayer(eKillingPlayer))
	{
		ChangeFriendshipWithMajor(eKillingPlayer, /*12*/ GC.getFRIENDSHIP_PER_BARB_KILLED());

		ChangeAngerFreeIntrusionCounter(eKillingPlayer, 5);

		Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BARB_KILLED");
		strMessage << GetPlayer()->getNameKey();
		Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SM_MINOR_BARB_KILLED");
		strSummary << GetPlayer()->getNameKey();

		AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), eKillingPlayer, iX, iY);
	}
}


// ******************************
// ***** Proxy War event *****
// ******************************

/// Time to send out a "Help us with Units" notification?
void CvMinorCivAI::DoTestProxyWarAnnouncement()
{
	for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
		PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
		CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
		CvTeam* pNotifyTeam = &GET_TEAM(kCurNotifyPlayer.getTeam());
		if (!pNotifyTeam->isHasMet(GetPlayer()->getTeam()))
		{
			continue;
		}

		for (int i = 0; i < MAX_MAJOR_CIVS; i++)
		{
			PlayerTypes eOtherMajor = (PlayerTypes) i;
			if (IsProxyWarActiveForMajor(eNotifyPlayer, eOtherMajor))
			{
				CvPlayer* pOtherMajor = &GET_PLAYER(eOtherMajor);
				CvAssertMsg(pOtherMajor, "Error sending out proxy war notification from a city-state. Please send Anton your save file and version.");
				if (pOtherMajor)
				{
					TeamTypes eEnemyTeam = pOtherMajor->getTeam();
					PlayerTypes eEnemyTeamLeader = GET_TEAM(eEnemyTeam).getLeaderID();
					CvPlayer* pEnemyTeamLeader = &GET_PLAYER(eEnemyTeamLeader);
					CvAssert(pEnemyTeamLeader);
					if (pEnemyTeamLeader && GET_TEAM(GetPlayer()->getTeam()).GetNumTurnsAtWar(eEnemyTeam) == 1)
					{
						Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_WAR_UNIT_HELP");
						strMessage << GetPlayer()->getCivilizationShortDescriptionKey() << pEnemyTeamLeader->getCivilizationShortDescriptionKey();
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_WAR_UNIT_HELP");
						strSummary << GetPlayer()->getCivilizationShortDescriptionKey();

						AddQuestNotification(strMessage.toUTF8(), strSummary.toUTF8(), eNotifyPlayer);

						break;
					}
				}
			}
		}
	}
}

void CvMinorCivAI::DoTestProxyWarAnnouncementOnFirstContact(PlayerTypes eMajor)
{
	for (int i = 0; i < MAX_MAJOR_CIVS; i++)
	{
		PlayerTypes eOtherMajor = (PlayerTypes) i;
		if (IsProxyWarActiveForMajor(eMajor, eOtherMajor))
		{
			CvPlayer* pOtherMajor = &GET_PLAYER(eOtherMajor);
			CvAssertMsg(pOtherMajor, "Error sending out proxy war notification from a city-state. Please send Anton your save file and version.");
			if (pOtherMajor)
			{
				TeamTypes eEnemyTeam = pOtherMajor->getTeam();
				PlayerTypes eEnemyTeamLeader = GET_TEAM(eEnemyTeam).getLeaderID();
				CvPlayer* pEnemyTeamLeader = &GET_PLAYER(eEnemyTeamLeader);
				CvAssert(pEnemyTeamLeader);
				if (pEnemyTeamLeader)
				{
					Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_WAR_UNIT_HELP");
					strMessage << GetPlayer()->getCivilizationShortDescriptionKey() << pEnemyTeamLeader->getCivilizationShortDescriptionKey();
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_WAR_UNIT_HELP");
					strSummary << GetPlayer()->getCivilizationShortDescriptionKey();

					AddQuestNotification(strMessage.toUTF8(), strSummary.toUTF8(), eMajor);

					break;
				}
			}
		}
	}
}

bool CvMinorCivAI::IsProxyWarActiveForMajor(PlayerTypes eMajor, PlayerTypes eOtherMajor)
{
	CvAssert(eMajor >= 0 && eMajor < MAX_MAJOR_CIVS);
	CvAssert(eOtherMajor >= 0 && eOtherMajor < MAX_MAJOR_CIVS);
	if (eMajor != eOtherMajor && eMajor >= 0 && eMajor < MAX_MAJOR_CIVS && eOtherMajor >= 0 && eOtherMajor < MAX_MAJOR_CIVS)
	{
		TeamTypes eOtherTeam = GET_PLAYER(eOtherMajor).getTeam();
		CvAssert(eOtherTeam != NO_TEAM);
		if (eOtherTeam != NO_TEAM)
		{
			// eMajor is at peace with us and eOtherMajor
			if (GET_PLAYER(eMajor).isAlive() && !IsAtWarWithPlayersTeam(eMajor) && !GET_TEAM(eOtherTeam).isAtWar(GET_PLAYER(eMajor).getTeam()))
			{
				// eOtherMajor is at war with us
				if (GET_PLAYER(eOtherMajor).isAlive() && IsAtWarWithPlayersTeam(eOtherMajor) && !IsPeaceBlocked(eOtherTeam))
				{
					// Do some additional checks to safeguard against weird scenario cases (ex. major and minor on same team, major is dead)
					PlayerTypes eOtherTeamLeader = GET_TEAM(eOtherTeam).getLeaderID();
					CvPlayer* pOtherTeamLeader = &GET_PLAYER(eOtherTeamLeader);
					if (pOtherTeamLeader && !pOtherTeamLeader->isMinorCiv() && pOtherTeamLeader->isAlive())
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool CvMinorCivAI::IsProxyWarActiveForMajor(PlayerTypes eMajor)
{
	CvAssert(eMajor >= 0 && eMajor < MAX_MAJOR_CIVS);
	for (int i = 0; i < MAX_MAJOR_CIVS; i++)
	{
		if (IsProxyWarActiveForMajor(eMajor, (PlayerTypes)i))
		{
			return true;
		}
	}
	return false;
}


// ******************************
// ***** Quests *****
// ******************************

/// Update turn for Quests
void CvMinorCivAI::DoTurnQuests()
{
	// ********************
	// Check Current Quests
	// ********************
	DoTestActiveQuests(/*bTestComplete*/ true, /*bTestObsolete*/ true);
	DoQuestsCleanup();

	// ********************
	// Give Global Quests
	// ********************
	int iFirstTurnForGlobalQuests = GetFirstPossibleTurnForGlobalQuests();

	// Increment turns since last quest
	if(GetGlobalQuestCountdown() > 0)
	{
		ChangeGlobalQuestCountdown(-1);
	}

	// Unlock turn - seed the counter
	if(GC.getGame().getElapsedGameTurns() == iFirstTurnForGlobalQuests)
	{
		DoTestSeedGlobalQuestCountdown();
	}

	if(GC.getGame().getElapsedGameTurns() > iFirstTurnForGlobalQuests || ENABLE_QUESTS_AT_START)
	{
		DoTestStartGlobalQuest();
	}

	// ********************
	// Give Personal Quests
	// ********************
	int iFirstTurnForPersonalQuests = GetFirstPossibleTurnForPersonalQuests();

	PlayerTypes ePlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(IsHasMetPlayer(ePlayer))
		{
			// Increment turns since last quest
			if(GetQuestCountdownForPlayer(ePlayer) > 0)
			{
				ChangeQuestCountdownForPlayer(ePlayer, -1);
			}

			// Unlock turn - seed the counter
			if(GC.getGame().getElapsedGameTurns() == iFirstTurnForPersonalQuests)
			{
				DoTestSeedQuestCountdownForPlayer(ePlayer);
			}
			// Can't start a quest too early
			else if(GC.getGame().getElapsedGameTurns() > iFirstTurnForPersonalQuests || ENABLE_QUESTS_AT_START)
			{
				if(!IsAtWarWithPlayersTeam(ePlayer))
				{
					DoTestStartPersonalQuest(ePlayer);
				}
			}
		}
	}
}

/// What is the first possible turn of the game we can fire off a Quest for a player?
int CvMinorCivAI::GetFirstPossibleTurnForPersonalQuests() const
{
	return /*30*/ GC.getMINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN();
}

/// What is the first possible turn of the game we can give out global Quests, that are for multiple players?
int CvMinorCivAI::GetFirstPossibleTurnForGlobalQuests() const
{
	return /*30*/ GC.getMINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN();
}

/// What is the maximum number of player-specific quests a player can have at one time?
int CvMinorCivAI::GetMaxActivePersonalQuestsForPlayer() const
{
	return 2; //antonjs: todo: constant/XML, link with size of typedef vectors in header file
}

/// What is the maximum number of global (multiple player) quests this minor can have active at one time?
int CvMinorCivAI::GetMaxActiveGlobalQuests() const
{
	return 1; //antonjs: todo: constant/XML, link with size of typedef vectors in header file
}

/// See if it is time to start a global (multiple player) quest
void CvMinorCivAI::DoTestStartGlobalQuest()
{
	if(GetGlobalQuestCountdown() > 0)
	{
		return;
	}

	if(GetNumActiveGlobalQuests() >= GetMaxActiveGlobalQuests())
	{
		return;
	}

	// Pick a valid quest
	FStaticVector<MinorCivQuestTypes, NUM_MINOR_CIV_QUEST_TYPES, true, c_eCiv5GameplayDLL, 0> veValidQuests;
	MinorCivQuestTypes eQuest;
	PlayerTypes ePlayer;
	for(int iQuestLoop = 0; iQuestLoop < NUM_MINOR_CIV_QUEST_TYPES; iQuestLoop++)
	{
		eQuest = (MinorCivQuestTypes) iQuestLoop;
		if(IsEnabledQuest(eQuest))
		{
			if(IsGlobalQuest(eQuest))
			{
				// Are there enough players for this quest to be given out?
				int iNumValidPlayers = 0;
				for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
				{
					ePlayer = (PlayerTypes) iPlayerLoop;
					if(IsValidQuestForPlayer(ePlayer, eQuest))
						iNumValidPlayers++;
				}
				if(iNumValidPlayers < GetMinPlayersNeededForQuest(eQuest))
					continue;

				// What is the bias for this minor favoring this particular quest? Queue up multiple copies (default is 10)
				int iCount = GetPersonalityQuestBias(eQuest);
				for(int iCountLoop = 0; iCountLoop < iCount; iCountLoop++)
				{
					veValidQuests.push_back(eQuest);
				}
			}
		}
	}

	// No valid quests
	if(veValidQuests.size() == 0)
	{
		return;
	}

	// There are valid quests, so pick one at random
	int iRandIndex = GC.getGame().getJonRandNum(veValidQuests.size(), "Picking random global quest for Minor to give to players.");
	eQuest = veValidQuests[iRandIndex];

	// Give out the quest
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;
		if(IsValidQuestForPlayer(ePlayer, eQuest))
		{
			// Since we are adding each instance of the quest separately, global quests should not rely on random choice of data
			AddQuestForPlayer(ePlayer, eQuest, GC.getGame().getGameTurn());
		}
	}

	// Check if we need to seed the countdown timer to allow for another quest
	DoTestSeedGlobalQuestCountdown();
}

/// See if it is time to start a personal quest for this player
void CvMinorCivAI::DoTestStartPersonalQuest(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	// Can't start a new quest too quickly after an old one has ended
	if(GetQuestCountdownForPlayer(ePlayer) > 0)
	{
		return;
	}

	// Can't start a new quest if we already have several active ones
	if(GetNumActivePersonalQuestsForPlayer(ePlayer) >= GetMaxActivePersonalQuestsForPlayer())
	{
		return;
	}

	FStaticVector<MinorCivQuestTypes, NUM_MINOR_CIV_QUEST_TYPES, true, c_eCiv5GameplayDLL, 0> veValidQuests;

	int iCount, iCountLoop;

	MinorCivQuestTypes eQuest;
	for(int iQuestLoop = 0; iQuestLoop < NUM_MINOR_CIV_QUEST_TYPES; iQuestLoop++)
	{
		eQuest = (MinorCivQuestTypes) iQuestLoop;

		if(IsValidQuestForPlayer(ePlayer, eQuest) && IsPersonalQuest(eQuest))
		{
			// What is the bias for this player wanting this particular quest? Queue up multiple copies (default is 10)
			iCount = GetPersonalityQuestBias(eQuest);

			for(iCountLoop = 0; iCountLoop < iCount; iCountLoop++)
			{
				veValidQuests.push_back(eQuest);
			}
		}
	}

	// No valid Quests
	if(veValidQuests.size() == 0)
	{
		return;
	}

	int iRandIndex = GC.getGame().getJonRandNum(veValidQuests.size(), "Picking random quest for Minor to give to a player.");
	eQuest = veValidQuests[iRandIndex];

	AddQuestForPlayer(ePlayer, eQuest, GC.getGame().getGameTurn());

	// Check if we need to seed the countdown timer to allow for another quest
	DoTestSeedQuestCountdownForPlayer(ePlayer);
}

/// Begin a quest, initializing it with data
void CvMinorCivAI::AddQuestForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eType, int iStartTurn)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	CvAssertMsg(eType >= NO_MINOR_CIV_QUEST_TYPE, "eType is expected to be non-negative (invalid Index)");
	CvAssertMsg(eType < NUM_MINOR_CIV_QUEST_TYPES, "eType is expected to be within maximum bounds (invalid Index)");
	if(eType < NO_MINOR_CIV_QUEST_TYPE || eType >= NUM_MINOR_CIV_QUEST_TYPES) return;

	// ePlayer can only have at most one quest of each type
	CvAssertMsg(!IsActiveQuestForPlayer(ePlayer, eType), "ePlayer already has a quest of this type!");
	if(IsActiveQuestForPlayer(ePlayer, eType)) return;

	CvMinorCivQuest newQuest(GetPlayer()->GetID(), ePlayer, eType);
	newQuest.DoStartQuest(iStartTurn);
	m_QuestsGiven[ePlayer].push_back(newQuest);

	// Log quest received
	GET_PLAYER(ePlayer).GetDiplomacyAI()->LogMinorCivQuestReceived(GetPlayer()->GetID(), GetEffectiveFriendshipWithMajorTimes100(ePlayer), GetEffectiveFriendshipWithMajorTimes100(ePlayer), eType);
}

/// Begin a quest that is already underway, copying over quest data
void CvMinorCivAI::AddQuestCopyForPlayer(PlayerTypes ePlayer, CvMinorCivQuest* pQuest)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	CvAssertMsg(pQuest != NULL, "pQuest should not be NULL. Please send Anton your save file and version.");
	if(pQuest == NULL) return;

	// ePlayer can only have at most one quest of each type
	CvAssertMsg(!IsActiveQuestForPlayer(ePlayer, pQuest->GetType()), "ePlayer already has a quest of this type!");
	if(IsActiveQuestForPlayer(ePlayer, pQuest->GetType())) return;

	CvMinorCivQuest newQuest(GetPlayer()->GetID(), ePlayer, pQuest->GetType());
	newQuest.DoStartQuestUsingExistingData(pQuest);
	m_QuestsGiven[ePlayer].push_back(newQuest);

	// Log quest received
	GET_PLAYER(ePlayer).GetDiplomacyAI()->LogMinorCivQuestReceived(GetPlayer()->GetID(), GetEffectiveFriendshipWithMajorTimes100(ePlayer), GetEffectiveFriendshipWithMajorTimes100(ePlayer), pQuest->GetType());
}


/// Called on first contact with major.  Have major join global quests that are already in progress (ex. contests).
/// Works on the assumption that this minor can only give out one of each type of global quest at a time.
/// Works on the assumption that any quests eMajor would be given in this way are valid for eMajor without having to check.
void CvMinorCivAI::DoTestQuestsOnFirstContact(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;

	//antonjs: consider: make this less hacky
	if(GetNumActiveGlobalQuests() > 0)
	{
		std::set<MinorCivQuestTypes> setGlobalQuests;

		MinorCivQuestTypes eQuestType;
		for(uint iPlayerLoop = 0; iPlayerLoop < m_QuestsGiven.size(); iPlayerLoop++)
		{
			for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[iPlayerLoop].size(); iQuestLoop++)
			{
				CvMinorCivQuest quest = m_QuestsGiven[iPlayerLoop][iQuestLoop];
				CvMinorCivQuest* pQuest = &quest;
				if(!pQuest)
				{
					CvAssertMsg(false, "pQuest should not be NULL. Please send Anton your save file and version.");
					continue;
				}
				eQuestType = pQuest->GetType();
				if(IsGlobalQuest(eQuestType))
				{
					// Haven't addressed this quest type yet?
					if(setGlobalQuests.find(eQuestType) == setGlobalQuests.end())
					{
						// Can we give a copy of this quest to this player?
						if(IsValidQuestCopyForPlayer(eMajor, pQuest))
						{
							AddQuestCopyForPlayer(eMajor, pQuest);
							setGlobalQuests.insert(eQuestType);
						}
					}
				}
			}
		}
	}
}

// Check all active quests for all players, processing and deleting ones that are complete or obsolete.
void CvMinorCivAI::DoTestActiveQuests(bool bTestComplete, bool bTestObsolete)
{
	if (bTestComplete)
		DoCompletedQuests();
	
	if (bTestObsolete)
		DoObsoleteQuests();
}

// Check all active quests for ePlayer, processing and deleting ones that are complete or obsolete.
// If no quest type is specified, will check all quest types.
void CvMinorCivAI::DoTestActiveQuestsForPlayer(PlayerTypes ePlayer, bool bTestComplete, bool bTestObsolete, MinorCivQuestTypes eQuest)
{
	if (bTestComplete)
		DoCompletedQuestsForPlayer(ePlayer, eQuest);
	
	if (bTestObsolete)
		DoObsoleteQuestsForPlayer(ePlayer, eQuest);
}

// Check for quests that have been completed, process them beginning with the player with highest resulting influence.
void CvMinorCivAI::DoCompletedQuests()
{
	WeightedCivsList vePlayers = CalculateFriendshipFromQuests();

	// Resolve quests starting with player that will end up with most influence, to avoid multiple ally swaps and awkward war declarations
	for (int iMajorIndex = 0; iMajorIndex < vePlayers.size(); iMajorIndex++)
	{
		PlayerTypes eMajor = vePlayers.GetElement(iMajorIndex);
		DoCompletedQuestsForPlayer(eMajor);
	}
}


// Return a sorted weighted vector of player influence values accounting for quest rewards they are about to receive.
WeightedCivsList CvMinorCivAI::CalculateFriendshipFromQuests()
{
	WeightedCivsList vePlayerInfluences;
	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		PlayerTypes eMajorLoop = (PlayerTypes) iMajorLoop;
		int iInfTimes100 = GetEffectiveFriendshipWithMajorTimes100(eMajorLoop);
		if (IsHasMetPlayer(eMajorLoop) && !IsAtWarWithPlayersTeam(eMajorLoop))
		{
			QuestListForPlayer::iterator itr_quest;
			for (itr_quest = m_QuestsGiven[eMajorLoop].begin(); itr_quest != m_QuestsGiven[eMajorLoop].end(); itr_quest++)
			{
				if (itr_quest->IsComplete())
				{
					iInfTimes100 += (itr_quest->GetInfluenceReward() * 100);
				}
			}
		}
		int iWeight = iInfTimes100 + abs(GC.getMINOR_FRIENDSHIP_AT_WAR() * 100); // Weights cannot be negative, so shift all values up by minimum influence amount
		vePlayerInfluences.push_back(eMajorLoop, iWeight);
	}

	vePlayerInfluences.SortItems();
	return vePlayerInfluences;
}

// Process completed quests that are active, and seed countdowns if needed.
// If no quest type is specified, will check all quest types.
void CvMinorCivAI::DoCompletedQuestsForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eSpecifyQuestType)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	bool bCheckAllQuests = true;
	if (eSpecifyQuestType > NO_MINOR_CIV_QUEST_TYPE && eSpecifyQuestType < NUM_MINOR_CIV_QUEST_TYPES)
		bCheckAllQuests = false;

	QuestListForPlayer::iterator itr_quest;
	for (itr_quest = m_QuestsGiven[ePlayer].begin(); itr_quest != m_QuestsGiven[ePlayer].end(); itr_quest++)
	{
		if (bCheckAllQuests || itr_quest->GetType() == eSpecifyQuestType)
		{
			if (itr_quest->IsComplete())
			{
				int iOldFriendshipTimes100 = GetEffectiveFriendshipWithMajorTimes100(ePlayer);
				bool bCompleted = itr_quest->DoFinishQuest();
				int iNewFriendshipTimes100 = GetEffectiveFriendshipWithMajorTimes100(ePlayer);
				
				if (bCompleted)
				{
					GET_PLAYER(ePlayer).GetDiplomacyAI()->LogMinorCivQuestFinished(GetPlayer()->GetID(), iOldFriendshipTimes100, iNewFriendshipTimes100, itr_quest->GetType());
				}
			}
		}
	}
}

// Check for quests that are obsolete and process them for each player in no particular order.
void CvMinorCivAI::DoObsoleteQuests()
{
	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		PlayerTypes eMajorLoop = (PlayerTypes) iMajorLoop;
		DoObsoleteQuestsForPlayer(eMajorLoop);
	}
}

// Process obsolete quests that are active, and seed countdowns if needed.
// If no quest type is specified, will check all quest types.
void CvMinorCivAI::DoObsoleteQuestsForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eSpecifyQuestType)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	bool bCheckAllQuests = true;
	if (eSpecifyQuestType > NO_MINOR_CIV_QUEST_TYPE && eSpecifyQuestType < NUM_MINOR_CIV_QUEST_TYPES)
		bCheckAllQuests = false;

	bool bQuestRevokedFromBullying = false;

	QuestListForPlayer::iterator itr_quest;
	for(itr_quest = m_QuestsGiven[ePlayer].begin(); itr_quest != m_QuestsGiven[ePlayer].end(); itr_quest++)
	{
		if(bCheckAllQuests || itr_quest->GetType() == eSpecifyQuestType)
		{
			if(itr_quest->IsObsolete())
			{
				int iOldFriendshipTimes100 = GetEffectiveFriendshipWithMajorTimes100(ePlayer);
				bool bCancelled = itr_quest->DoCancelQuest();
				int iNewFriendshipTimes100 = GetEffectiveFriendshipWithMajorTimes100(ePlayer);
				
				if (bCancelled)
				{
					if(itr_quest->IsRevoked())
						bQuestRevokedFromBullying = true;

					GET_PLAYER(ePlayer).GetDiplomacyAI()->LogMinorCivQuestCancelled(GetPlayer()->GetID(), iOldFriendshipTimes100, iNewFriendshipTimes100, itr_quest->GetType());
				}
			}
		}
	}

	// If quest(s) were revoked because of bullying, send out a notification
	if(bQuestRevokedFromBullying)
	{
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
		if (MOD_DIPLOMACY_CITYSTATES_QUESTS) {
			//Minor punishment- don't bully city-states!
			SetFriendshipWithMajor(ePlayer, GC.getMINOR_FRIENDSHIP_AT_WAR(), /*bFromQuest*/ true);
		}
#endif
		Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_REVOKED");
		Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_REVOKED");
		strMessage << GetPlayer()->getNameKey();
		strSummary << GetPlayer()->getNameKey();
		AddQuestNotification(strMessage.toUTF8(), strSummary.toUTF8(), ePlayer);
	}
}

void CvMinorCivAI::DoQuestsCleanup()
{
	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		PlayerTypes eMajorLoop = (PlayerTypes) iMajorLoop;
		DoQuestsCleanupForPlayer(eMajorLoop);
	}
}

// Deletes active completed and/or obsolete quests from memory, and resets the countdown timer if needed.
void CvMinorCivAI::DoQuestsCleanupForPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	bool bPersonalQuestDone = false;
	bool bGlobalQuestDone = false;

	QuestListForPlayer::iterator itr_quest;
	for (itr_quest = m_QuestsGiven[ePlayer].begin(); itr_quest != m_QuestsGiven[ePlayer].end(); itr_quest++)
	{
		if (itr_quest->IsHandled())
		{
			MinorCivQuestTypes eQuestType = itr_quest->GetType();
			if (IsPersonalQuest(eQuestType))
				bPersonalQuestDone = true;
			if (IsGlobalQuest(eQuestType))
				bGlobalQuestDone = true;

			m_QuestsGiven[ePlayer].erase(itr_quest);
			itr_quest--;
		}
	}

	// Check if we need to seed the countdown timers
	if (bPersonalQuestDone)
		DoTestSeedQuestCountdownForPlayer(ePlayer);
	if (bGlobalQuestDone)
		DoTestSeedGlobalQuestCountdown();
}

// Is this quest enabled at all?
bool CvMinorCivAI::IsEnabledQuest(MinorCivQuestTypes eQuest)
{
	// BUILD A ROUTE
	if(eQuest == MINOR_CIV_QUEST_ROUTE)
	{
		if(GC.getQUEST_DISABLED_ROUTE() == 1)
			return false;
	}
	// KILL A CAMP
	else if(eQuest == MINOR_CIV_QUEST_KILL_CAMP)
	{
		if(GC.getQUEST_DISABLED_KILL_CAMP() == 1)
			return false;
	}
	// CONNECT A RESOURCE
	else if(eQuest == MINOR_CIV_QUEST_CONNECT_RESOURCE)
	{
		if(GC.getQUEST_DISABLED_CONNECT_RESOURCE() == 1)
			return false;
	}
	// CONSTRUCT A WONDER
	else if(eQuest == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
	{
		if(GC.getQUEST_DISABLED_CONSTRUCT_WONDER() == 1)
			return false;
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	// CONSTRUCT A NATIONAL WONDER
	else if(eQuest == MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER)
	{
		if(!MOD_DIPLOMACY_CITYSTATES_QUESTS || GC.getQUEST_DISABLED_CONSTRUCT_WONDER() == 1)
			return false;
	}
#endif
	// GREAT PERSON
	else if(eQuest == MINOR_CIV_QUEST_GREAT_PERSON)
	{
		if(GC.getQUEST_DISABLED_GREAT_PERSON() == 1)
			return false;
	}
	// KILL ANOTHER CITY STATE
	else if(eQuest == MINOR_CIV_QUEST_KILL_CITY_STATE)
	{
		if(GC.getQUEST_DISABLED_KILL_CITY_STATE() == 1)
			return false;
	}
	// FIND ANOTHER PLAYER
	else if(eQuest == MINOR_CIV_QUEST_FIND_PLAYER)
	{
		if(GC.getQUEST_DISABLED_FIND_PLAYER() == 1)
			return false;
	}
	// FIND NATURAL WONDER
	else if(eQuest == MINOR_CIV_QUEST_FIND_NATURAL_WONDER)
	{
		if(GC.getQUEST_DISABLED_NATURAL_WONDER() == 1)
			return false;
	}
	// GOLD GIFT
	else if(eQuest == MINOR_CIV_QUEST_GIVE_GOLD)
	{
		if(GC.getQUEST_DISABLED_GIVE_GOLD() == 1)
			return false;
	}
	// PLEDGE TO PROTECT
	else if(eQuest == MINOR_CIV_QUEST_PLEDGE_TO_PROTECT)
	{
		if(GC.getQUEST_DISABLED_PLEDGE_TO_PROTECT() == 1)
			return false;
	}
	// CONTEST CULTURE
	else if(eQuest == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{
		if(GC.getGame().isOption(GAMEOPTION_NO_POLICIES))
			return false;

		if(GC.getQUEST_DISABLED_CONTEST_CULTURE() == 1)
			return false;
	}
	// CONTEST FAITH
	else if(eQuest == MINOR_CIV_QUEST_CONTEST_FAITH)
	{
		if(GC.getGame().isOption(GAMEOPTION_NO_RELIGION))
			return false;

		if(GC.getQUEST_DISABLED_CONTEST_FAITH() == 1)
			return false;
	}
	// CONTEST TECHS
	else if(eQuest == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		if(GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
			return false;

		if(GC.getQUEST_DISABLED_CONTEST_TECHS() == 1)
			return false;
	}
	// Invest
	else if(eQuest == MINOR_CIV_QUEST_INVEST)
	{
		if(GC.getQUEST_DISABLED_INVEST() == 1)
			return false;
	}
	// Bully target City-State
	else if(eQuest == MINOR_CIV_QUEST_BULLY_CITY_STATE)
	{
		if(GC.getQUEST_DISABLED_BULLY_CITY_STATE() == 1)
			return false;
	}
	// Denounce target Major
	else if(eQuest == MINOR_CIV_QUEST_DENOUNCE_MAJOR)
	{
		if(GC.getQUEST_DISABLED_DENOUNCE_MAJOR() == 1)
			return false;
	}
	// Spread your religion to us
	else if(eQuest == MINOR_CIV_QUEST_SPREAD_RELIGION)
	{
		if(GC.getGame().isOption(GAMEOPTION_NO_RELIGION))
			return false;

		if(GC.getQUEST_DISABLED_SPREAD_RELIGION() == 1)
			return false;
	}
	// Trade Route
	else if(eQuest == MINOR_CIV_QUEST_TRADE_ROUTE)
	{
		if(GC.getQUEST_DISABLED_TRADE_ROUTE() == 1)
			return false;
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	// War with other major
	else if(eQuest == MINOR_CIV_QUEST_WAR)
	{
		if(!MOD_DIPLOMACY_CITYSTATES_QUESTS || GC.getQUEST_DISABLED_WAR() == 1)
			return false;
	}
	// Find City State
	else if(eQuest == MINOR_CIV_QUEST_FIND_CITY_STATE)
	{
		if(!MOD_DIPLOMACY_CITYSTATES_QUESTS || GC.getQUEST_DISABLED_FIND_CITY_STATE() == 1)
			return false;
	}
	// Invest
	else if(eQuest == MINOR_CIV_QUEST_INFLUENCE)
	{
		if(!MOD_DIPLOMACY_CITYSTATES_QUESTS || GC.getQUEST_DISABLED_INFLUENCE() == 1)
			return false;
	}
	// Tourism
	else if(eQuest == MINOR_CIV_QUEST_CONTEST_TOURISM)
	{
		if(!MOD_DIPLOMACY_CITYSTATES_QUESTS || GC.getQUEST_DISABLED_TOURISM() == 1)
			return false;
	}
	// Archaeology
	else if(eQuest == MINOR_CIV_QUEST_ARCHAEOLOGY)
	{
		if(!MOD_DIPLOMACY_CITYSTATES_QUESTS || GC.getQUEST_DISABLED_ARCHAEOLOGY() == 1)
			return false;
	}
	// Circumnavigation
	else if(eQuest == MINOR_CIV_QUEST_CIRCUMNAVIGATION)
	{
		if(!MOD_DIPLOMACY_CITYSTATES_QUESTS || GC.getQUEST_DISABLED_CIRCUMNAVIGATION() == 1)
			return false;
	}
	// Circumnavigation
	else if(eQuest == MINOR_CIV_QUEST_LIBERATION)
	{
		if(!MOD_DIPLOMACY_CITYSTATES_QUESTS || GC.getQUEST_DISABLED_LIBERATION() == 1)
			return false;
	}
	// Barbarian Horde
	else if(eQuest == MINOR_CIV_QUEST_HORDE)
	{
		if(!MOD_DIPLOMACY_CITYSTATES_QUESTS || GC.getQUEST_DISABLED_HORDE() == 1)
			return false;
	}
	// Rebellion
	else if(eQuest == MINOR_CIV_QUEST_REBELLION)
	{
		if(!MOD_DIPLOMACY_CITYSTATES_QUESTS || GC.getQUEST_DISABLED_REBELLION() == 1)
			return false;
	}
#endif

	return true;
}

/// Is eQuest valid for this minor to give to ePlayer?
bool CvMinorCivAI::IsValidQuestForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eQuest)
{
	// The quest type must be enabled
	if(!IsEnabledQuest(eQuest))
		return false;

	// Somebody's dead, that's no good
	if(!GET_PLAYER(ePlayer).isAlive() || !GetPlayer()->isAlive())
		return false;

	// No quests are valid if we are at war
	if(IsAtWarWithPlayersTeam(ePlayer))
		return false;

	// No quests are valid if we don't know you yet
	if(!IsHasMetPlayer(ePlayer))
		return false;

	// This player cannot already have an active quest of the same type
	if(IsActiveQuestForPlayer(ePlayer, eQuest))
		return false;

	// Both players must have a capital
	CvCity* pMinorsCapital = GetPlayer()->getCapitalCity();
	CvCity* pMajorsCapital = GET_PLAYER(ePlayer).getCapitalCity();
	if(pMinorsCapital == NULL || pMajorsCapital == NULL)
		return false;

	// BUILD A ROUTE
	if(eQuest == MINOR_CIV_QUEST_ROUTE)
	{
		// This player must not have bullied us recently
		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;

		// Must have been friends at some point
		if(!IsEverFriends(ePlayer))
			return false;

		// Cannot already have a route
		if (GET_PLAYER(ePlayer).IsCapitalConnectedToPlayer(GetPlayer()->GetID()))
			return false;

		// Can't have completed this Quest already
		if(IsRouteConnectionEstablished(ePlayer))
			return false;

		// Must have a city close to the minor in the same Area
		const int iMaxRouteDistance = 8; //antonjs: todo: xml
		CvPlot* pMinorsPlot = pMinorsCapital->plot();
		if(pMinorsPlot == NULL)
			return false;

		int iLoop;
		CvCity* pLoopCity;
		bool bInRange = false;
		for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
		{
			if(pMinorsPlot->getArea() != pLoopCity->getArea())
				continue;
			int iDistance = plotDistance(pMinorsPlot->getX(), pMinorsPlot->getY(), pLoopCity->getX(), pLoopCity->getY());
			if(iDistance < iMaxRouteDistance)
				bInRange = true;
		}

		if(!bInRange)
			return false;
	}
	// KILL A CAMP
	else if(eQuest == MINOR_CIV_QUEST_KILL_CAMP)
	{
		// Any nearby camps?
		if(GetBestNearbyCampToKill() == NULL)
			return false;
	}
	// CONNECT A RESOURCE
	else if(eQuest == MINOR_CIV_QUEST_CONNECT_RESOURCE)
	{
		// This player must not have bullied us recently
		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;

		ResourceTypes eResource = GetNearbyResourceForQuest(ePlayer);

		if(eResource == NO_RESOURCE)
			return false;
	}
	// CONSTRUCT A WONDER
	else if(eQuest == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
	{
		// This player must not have bullied us recently
		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;

		BuildingTypes eWonder = GetBestWonderForQuest(ePlayer);

		if(eWonder == NO_BUILDING)
			return false;
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	// CONSTRUCT A NATIONAL WONDER
	else if(eQuest == MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER)
	{
		// This player must not have bullied us recently
		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;

		BuildingTypes eNationalWonder = GetBestNationalWonderForQuest(ePlayer);

		if(eNationalWonder == NO_BUILDING)
			return false;
	}
#endif
	// GREAT PERSON
	else if(eQuest == MINOR_CIV_QUEST_GREAT_PERSON)
	{
		// This player must not have bullied us recently
		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;

		UnitTypes eUnit = GetBestGreatPersonForQuest(ePlayer);

		if(eUnit == NO_UNIT)
			return false;
	}
	// KILL ANOTHER CITY STATE
	else if(eQuest == MINOR_CIV_QUEST_KILL_CITY_STATE)
	{
		// Friendly City States don't give out this quest
		if(GetPersonality() == MINOR_CIV_PERSONALITY_FRIENDLY)
			return false;

		PlayerTypes eTargetCityState = GetBestCityStateTarget(ePlayer);

		if(eTargetCityState == NO_PLAYER)
			return false;
	}
	// FIND ANOTHER PLAYER
	else if(eQuest == MINOR_CIV_QUEST_FIND_PLAYER)
	{
		// This player must not have bullied us recently
		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;

		PlayerTypes eTarget = GetBestPlayerToFind(ePlayer);

		if(eTarget == NO_PLAYER)
			return false;
	}
	// FIND NATURAL WONDER
	else if(eQuest == MINOR_CIV_QUEST_FIND_NATURAL_WONDER)
	{
		// This player must not have bullied us recently
		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;

		if(!IsGoodTimeForNaturalWonderQuest(ePlayer))
			return false;
	}
	// GOLD GIFT
	else if(eQuest == MINOR_CIV_QUEST_GIVE_GOLD)
	{
		// We don't need help if we've never been bullied
		if(!IsEverBulliedByAnyMajor())
			return false;

		// This player must not have bullied us recently
		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;

		// This player must not be the most recent bully
		if(GetMostRecentBullyForQuest() == ePlayer)
			return false;

		if(!IsGoodTimeForGiveGoldQuest())
			return false;
	}
	// PLEDGE TO PROTECT
	else if(eQuest == MINOR_CIV_QUEST_PLEDGE_TO_PROTECT)
	{
		// We don't need help if we've never been bullied
		if(!IsEverBulliedByAnyMajor())
			return false;

		// This player must not have bullied us recently
		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;

		// This player must not be the most recent bully
		if(GetMostRecentBullyForQuest() == ePlayer)
			return false;

		// Only if we aren't already protected by this major, and the major is allowed to start
		if(!CanMajorStartProtection(ePlayer))
			return false;

		if(!IsGoodTimeForPledgeToProtectQuest())
			return false;
	}
	// CONTEST CULTURE
	else if(eQuest == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{
	}
	// CONTEST FAITH
	else if(eQuest == MINOR_CIV_QUEST_CONTEST_FAITH)
	{
	}
	// CONTEST TECHS
	else if(eQuest == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
	}
	// Invest
	else if(eQuest == MINOR_CIV_QUEST_INVEST)
	{
		// This player must not have bullied us recently
		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;
	}
	// Bully target City-State
	else if(eQuest == MINOR_CIV_QUEST_BULLY_CITY_STATE)
	{
		// This player must not have bullied us recently
		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;

		PlayerTypes eTargetCityState = GetBestCityStateTarget(ePlayer);

		if(eTargetCityState == NO_PLAYER)
			return false;
	}
	// Denounce target Major
	else if(eQuest == MINOR_CIV_QUEST_DENOUNCE_MAJOR)
	{
		// We don't need help if we've never been bullied
		if(!IsEverBulliedByAnyMajor())
			return false;

		// This player must not have bullied us recently
		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;

		// Is there a recent bully?
		if(!IsGoodTimeForDenounceMajorQuest())
			return false;

		// Don't ask in always war games
		if(GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR))
			return false;
		
		PlayerTypes eMostRecentBully = GetMostRecentBullyForQuest();

		if(eMostRecentBully == NO_PLAYER)
			return false;

		// This player must not be the most recent bully
		if(eMostRecentBully == ePlayer)
			return false;

		// This player must not be teammates with the most recent bully (cannot denounce)
		if(GET_PLAYER(ePlayer).getTeam() == GET_PLAYER(eMostRecentBully).getTeam())
			return false;

		// This player must have met the most recent bully
		if(!GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GET_PLAYER(eMostRecentBully).getTeam()))
			return false;

		// This player must not be at war with the most recent bully (cannot denounce)
		if(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(GET_PLAYER(eMostRecentBully).getTeam()))
			return false;

		// Humans are unable to denounce each other
		if(GET_PLAYER(ePlayer).isHuman() && GET_PLAYER(eMostRecentBully).isHuman())
			return false;

		// This player must not have already denounced the most recent bully
		if(GET_PLAYER(ePlayer).GetDiplomacyAI()->IsDenouncedPlayer(GetMostRecentBullyForQuest()))
			return false;
	}
	// Spread your religion to us
	else if(eQuest == MINOR_CIV_QUEST_SPREAD_RELIGION)
	{
		// This player must not have bullied us recently
		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;

		// Player must have founded a religion
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
		// but not a local one
		if(!GC.getGame().GetGameReligions()->HasCreatedReligion(ePlayer, true))
#else
		if(!GC.getGame().GetGameReligions()->HasCreatedReligion(ePlayer))
#endif
			return false;

		// Minor must not already share player's religion
		if(IsSameReligionAsMajor(ePlayer))
			return false;
	}
	// Trade route
	else if(eQuest == MINOR_CIV_QUEST_TRADE_ROUTE)
	{
		// This player must not have bullied us recently
		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;

		// This player must not already be connected
		if(GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(ePlayer, GetPlayer()->GetID()))
			return false;

		// This player must be able to build a trade route either by land or sea
		if(!GC.getGame().GetGameTrade()->CanCreateTradeRoute(ePlayer,GetPlayer()->GetID(), DOMAIN_LAND) &&
			!GC.getGame().GetGameTrade()->CanCreateTradeRoute(ePlayer,GetPlayer()->GetID(), DOMAIN_SEA))
			return false;
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	// War Major
	else if(eQuest == MINOR_CIV_QUEST_WAR)
	{

		// This player must not have bullied us recently
		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;

		// Is there a recent bully?
		if(!IsGoodTimeForWarMajorQuest())
			return false;

		// Don't ask in always war games
		if(GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR))
			return false;
		
		PlayerTypes eMostRecentBully = GetMostRecentBullyForQuest();

		// This player must not be the ally
		if(eMostRecentBully == GetAlly())
			return false;

		// This player must not be teammates with the most recent bully (cannot denounce)
		if(GET_PLAYER(ePlayer).getTeam() == GET_PLAYER(eMostRecentBully).getTeam())
			return false;

		// This player must have met the most recent bully
		if(!GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GET_PLAYER(eMostRecentBully).getTeam()))
			return false;

		// This player must not be at war with the most recent bully (cannot denounce)
		if(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(GET_PLAYER(eMostRecentBully).getTeam()))
			return false;
	}
	// FIND ANOTHER CITY STATE
	else if(eQuest == MINOR_CIV_QUEST_FIND_CITY_STATE)
	{
		// This player must not have bullied us recently
		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;

		PlayerTypes eTargetCityState = GetBestCityStateMeetTarget(ePlayer);

		if(eTargetCityState == NO_PLAYER)
			return false;

		//Don't create this quest until a player has entered the Middle Ages
		EraTypes eAssumeEra = NO_ERA;
		EraTypes eCurrentEra = eAssumeEra;
		if(eCurrentEra == NO_ERA)
			eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();
			
			EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);

		// Renaissance era or Later
		if(eCurrentEra <= eMedieval)
		{
			return false;
		}
	}
	// Influence
	else if(eQuest == MINOR_CIV_QUEST_INFLUENCE)
	{
		// This player must not have bullied us recently
		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;
	}
	// Tourism Contest
	else if(eQuest == MINOR_CIV_QUEST_CONTEST_TOURISM)
	{
		//Don't create this quest until a player has entered the Renaissance
		EraTypes eAssumeEra = NO_ERA;
		EraTypes eCurrentEra = eAssumeEra;
		if(eCurrentEra == NO_ERA)
			eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();
			
			EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);

		// Renaissance era or Later
		if(eCurrentEra < eRenaissance)
		{
			return false;
		}
	}
	// Archaeology
	else if(eQuest == MINOR_CIV_QUEST_ARCHAEOLOGY)
	{
		// Any nearby camps?
		if(GetBestNearbyDig() == NULL)
			return false;
	}
	// Circumnavigation
	else if(eQuest == MINOR_CIV_QUEST_CIRCUMNAVIGATION)
	{
		if(!GC.getGame().circumnavigationAvailable())
		{
			// We can't issue circumnavigation quests
			return false;
		}

		//Give this quest out once the player can cross oceans.
		if(!GET_TEAM(GET_PLAYER(ePlayer).getTeam()).canEmbarkAllWaterPassage())
		{
			return false;
		}
	}
	// LIBERATE A CITY STATE
	else if(eQuest == MINOR_CIV_QUEST_LIBERATION)
	{
		PlayerTypes eTargetCityState = GetBestCityStateLiberate(ePlayer);

		if(eTargetCityState == NO_PLAYER)
		{
			return false;
		}
		if(!GET_PLAYER(ePlayer).CanLiberatePlayer(eTargetCityState))
		{
			return false;
		}
	}
	// BARBARIAN HORDE!
	else if(eQuest == MINOR_CIV_QUEST_HORDE)
	{
		if(GC.getGame().isOption(GAMEOPTION_NO_BARBARIANS))
		{
			return false;
		}

		if(SpawnHorde() == NO_PLAYER)
		{
			return false;
		}
	}
	// Rebellion!
	else if(eQuest == MINOR_CIV_QUEST_REBELLION)
	{
		if(GC.getGame().isOption(GAMEOPTION_NO_BARBARIANS))
		{
			return false;
		}
		if(SpawnRebels() == NO_PLAYER)
		{
			return false;
		}
	}
#endif

	return true;
}

/// Can we give a copy of pQuest to ePlayer (ie. late join to global quest)?
bool CvMinorCivAI::IsValidQuestCopyForPlayer(PlayerTypes ePlayer, CvMinorCivQuest* pQuest)
{
	if(!pQuest)
	{
		CvAssertMsg(false, "Checking validity of copying an existing quest, but pQuest is NULL. Please send Anton your save file and version.");
		return false;
	}

	MinorCivQuestTypes eQuestType = pQuest->GetType();

	// The quest type must be enabled
	if(!IsEnabledQuest(eQuestType))
		return false;

	// Somebody's dead, that's no good
	if(!GET_PLAYER(ePlayer).isAlive() || !GetPlayer()->isAlive())
		return false;

	// No quests are valid if we are at war
	if(IsAtWarWithPlayersTeam(ePlayer))
		return false;

	// antonjs: todo: dehackify
	if(!IsHasMetPlayer(ePlayer) && !GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GET_TEAM(GetPlayer()->getTeam()).GetID()))
		return false;

	// This player cannot already have an active quest of the same type
	if(IsActiveQuestForPlayer(ePlayer, eQuestType))
		return false;

	// Both players must have a capital
	CvCity* pMinorsCapital = GetPlayer()->getCapitalCity();
	CvCity* pMajorsCapital = GET_PLAYER(ePlayer).getCapitalCity();
	if(pMinorsCapital == NULL || pMajorsCapital == NULL)
		return false;

	// KILL A CAMP - Is the camp in the existing quest still around?
	if(eQuestType == MINOR_CIV_QUEST_KILL_CAMP)
	{
		int iCampX = pQuest->GetPrimaryData();
		int iCampY = pQuest->GetSecondaryData();
		CvPlot* pPlot = GC.getMap().plot(iCampX, iCampY);
		if(!pPlot)
			return false;
		if(pPlot->getImprovementType() != GC.getBARBARIAN_CAMP_IMPROVEMENT())
			return false;
	}
	// CONTEST CULTURE
	else if(eQuestType == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{
	}
	// CONTEST FAITH
	else if(eQuestType == MINOR_CIV_QUEST_CONTEST_FAITH)
	{
	}
	// CONTEST TECHS
	else if(eQuestType == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
	}
	// Invest
	else if(eQuestType == MINOR_CIV_QUEST_INVEST)
	{
		// This player must not have bullied us recently
		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	// Influence
	else if(eQuestType == MINOR_CIV_QUEST_INFLUENCE)
	{
		// This player must not have bullied us recently
		if(IsRecentlyBulliedByMajor(ePlayer))
			return false;
	}
	// Tourism
	else if(eQuestType == MINOR_CIV_QUEST_CONTEST_TOURISM)
	{
	}
	// Archaeology- is the dig in the existing quest still around?
	else if(eQuestType == MINOR_CIV_QUEST_ARCHAEOLOGY)
	{
		int iDigX = pQuest->GetPrimaryData();
		int iDigY = pQuest->GetSecondaryData();
	
		CvPlot* pPlot = GC.getMap().plot(iDigX, iDigY);
		if(!pPlot)
			return false;
		if(pPlot->getResourceType() != GC.getARTIFACT_RESOURCE())
			return false;
	}
	else if(eQuestType == MINOR_CIV_QUEST_CIRCUMNAVIGATION)
	{
	}
	else if(eQuestType == MINOR_CIV_QUEST_HORDE)
	{
	}
	else if(eQuestType == MINOR_CIV_QUEST_REBELLION)
	{
	}
#endif
	// Personal quests - This should not be done, just create a new quest from scratch!!
	else
	{
		CvAssertMsg(false, "Checking validity of copying a personal quest using existing quest data.  This should not be done.  Please send Anton your save file and version.");
		return IsValidQuestForPlayer(ePlayer, eQuestType);
	}

	return true;
}


bool CvMinorCivAI::IsGlobalQuest(MinorCivQuestTypes eQuest) const
{
	if(eQuest == MINOR_CIV_QUEST_KILL_CAMP)
		return true;

	if(eQuest == MINOR_CIV_QUEST_CONTEST_CULTURE)
		return true;

	if(eQuest == MINOR_CIV_QUEST_CONTEST_FAITH)
		return true;

	if(eQuest == MINOR_CIV_QUEST_CONTEST_TECHS)
		return true;

	if(eQuest == MINOR_CIV_QUEST_INVEST)
		return true;

#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	if(eQuest == MINOR_CIV_QUEST_INFLUENCE)
		return true;

	if(eQuest == MINOR_CIV_QUEST_CONTEST_TOURISM)
		return true;

	if(eQuest == MINOR_CIV_QUEST_ARCHAEOLOGY)
		return true;

	if(eQuest == MINOR_CIV_QUEST_CIRCUMNAVIGATION)
		return true;

	if(eQuest == MINOR_CIV_QUEST_HORDE)
		return true;

	if(eQuest == MINOR_CIV_QUEST_REBELLION)
		return true;
#endif

	return false;
}

bool CvMinorCivAI::IsPersonalQuest(MinorCivQuestTypes eQuest) const
{
	return (!IsGlobalQuest(eQuest));
}

int CvMinorCivAI::GetMinPlayersNeededForQuest(MinorCivQuestTypes eQuest) const
{
	int iPlayersNeeded = 1;

	if(eQuest == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{
		iPlayersNeeded = 3; //antonjs: todo: XML
	}

	else if(eQuest == MINOR_CIV_QUEST_CONTEST_FAITH)
	{
		iPlayersNeeded = 3; //antonjs: todo: XML
	}

	else if(eQuest == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		iPlayersNeeded = 3; //antonjs: todo: XML
	}

	else if(eQuest == MINOR_CIV_QUEST_INVEST)
	{
		iPlayersNeeded = 2; //antonjs: todo: XML
	}

#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	else if(eQuest == MINOR_CIV_QUEST_INFLUENCE)
	{
		iPlayersNeeded = 2;
	}

	else if(eQuest == MINOR_CIV_QUEST_CONTEST_TOURISM)
	{
		iPlayersNeeded = 3;
	}
#endif

	int iMajorsEverAlive = GC.getGame().countMajorCivsEverAlive();
	iPlayersNeeded = min(iPlayersNeeded, iMajorsEverAlive);

	return iPlayersNeeded;
}

/// How more or less likely is this minor to offer a particular quest type based on its personality?
int CvMinorCivAI::GetPersonalityQuestBias(MinorCivQuestTypes eQuest)
{
	// Multiply by 10 to give us some more fidelity. We'll remove it at the bottom
	int iCount = 10 * /*10*/ GC.getMINOR_CIV_QUEST_WEIGHT_DEFAULT();

	MinorCivPersonalityTypes ePersonality = GetPersonality();
	MinorCivTraitTypes eTrait = GetTrait();

	// ******************
	// Personal Quests
	// ******************

	// ROUTE
	if(eQuest == MINOR_CIV_QUEST_ROUTE)
	{
		if(ePersonality == MINOR_CIV_PERSONALITY_FRIENDLY)		// Friendly
		{
			iCount *= /*200*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_ROUTE();
			iCount /= 100;
		}
		if(ePersonality == MINOR_CIV_PERSONALITY_HOSTILE)		// Hostile
		{
			iCount *= /*20*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_ROUTE();
			iCount /= 100;
		}
		if(eTrait == MINOR_CIV_TRAIT_MARITIME)						// Maritime
		{
			iCount *= /*120*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_ROUTE();
			iCount /= 100;
		}
		if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
		{
			iCount *= /*150*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_ROUTE();
			iCount /= 100;
		}
	}

	// CONNECT A RESOURCE
	else if(eQuest == MINOR_CIV_QUEST_CONNECT_RESOURCE)
	{
		if(eTrait == MINOR_CIV_TRAIT_MARITIME)						// Maritime
		{
			iCount *= /*200*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_CONNECT_RESOURCE();
			iCount /= 100;
		}
		if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
		{
			iCount *= /*300*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_CONNECT_RESOURCE();
			iCount /= 100;
		}
	}

	// CONSTRUCT A WONDER
	else if(eQuest == MINOR_CIV_QUEST_CONSTRUCT_WONDER)
	{
		if(eTrait == MINOR_CIV_TRAIT_CULTURED)						// Cultured
		{
			iCount *= /*300*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_CONSTRUCT_WONDER();
			iCount /= 100;
		}
	}

	// GREAT PERSON
	else if(eQuest == MINOR_CIV_QUEST_GREAT_PERSON)
	{
		if(eTrait == MINOR_CIV_TRAIT_CULTURED)						// Cultured
		{
			iCount *= /*300*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_GREAT_PERSON();
			iCount /= 100;
		}
	}

	// KILL ANOTHER CITY STATE
	else if(eQuest == MINOR_CIV_QUEST_KILL_CITY_STATE)
	{
		if(eTrait == MINOR_CIV_TRAIT_MILITARISTIC)					// Militaristic
		{
			iCount *= /*200*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_KILL_CITY_STATE();
			iCount /= 100;
		}
		if(ePersonality == MINOR_CIV_PERSONALITY_HOSTILE)		// Hostile
		{
			iCount *= /*200*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_KILL_CITY_STATE();
			iCount /= 100;
		}
		if(ePersonality == MINOR_CIV_PERSONALITY_NEUTRAL)		// Neutral
		{
			iCount *= /*40*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_NEUTRAL_KILL_CITY_STATE();
			iCount /= 100;
		}
	}

	// FIND ANOTHER PLAYER
	else if(eQuest == MINOR_CIV_QUEST_FIND_PLAYER)
	{
		if(eTrait == MINOR_CIV_TRAIT_MARITIME)						// Maritime
		{
			iCount *= /*300*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_FIND_PLAYER();
			iCount /= 100;
		}
		if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
		{
			iCount *= /*200*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_FIND_PLAYER();
			iCount /= 100;
		}
	}

	// FIND NATURAL WONDER
	else if(eQuest == MINOR_CIV_QUEST_FIND_NATURAL_WONDER)
	{
		if(eTrait == MINOR_CIV_TRAIT_MILITARISTIC)					// Militaristic
		{
			iCount *= /*50*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_FIND_NATURAL_WONDER();
			iCount /= 100;
		}
		if(ePersonality == MINOR_CIV_PERSONALITY_HOSTILE)		// Hostile
		{
			iCount *= /*30*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_FIND_NATURAL_WONDER();
			iCount /= 100;
		}
	}

	// GOLD GIFT
	else if(eQuest == MINOR_CIV_QUEST_GIVE_GOLD)
	{
		//antonjs: consider: split quest bias into two categories, so that we can guarantee a support quest when a CS is bullied recently

		if(eTrait == MINOR_CIV_TRAIT_MILITARISTIC)  // We are tough, we don't need your charity
		{
			iCount *= /*200*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_GIVE_GOLD();
			iCount /= 100;
		}
		else if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)  // Money is the answer to everything!
		{
			iCount *= /*350*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_GIVE_GOLD();
			iCount /= 100;
		}
		else
		{
			iCount *= /*300*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_GIVE_GOLD();
			iCount /= 100;
		}
	}

	// PLEDGE TO PROTECT
	else if(eQuest == MINOR_CIV_QUEST_PLEDGE_TO_PROTECT)
	{
		if(eTrait == MINOR_CIV_TRAIT_MILITARISTIC)  // We are tough, we don't need your charity
		{
			iCount *= /*200*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_PLEDGE_TO_PROTECT();
			iCount /= 100;
		}
		else if(eTrait == MINOR_CIV_TRAIT_CULTURED)  // Military?  We don't have one of those!
		{
			iCount *= /*350*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_PLEDGE_TO_PROTECT();
			iCount /= 100;
		}
		else
		{
			iCount *= /*300*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_PLEDGE_TO_PROTECT();
			iCount /= 100;
		}
	}

	// Bully target City-State
	else if(eQuest == MINOR_CIV_QUEST_BULLY_CITY_STATE)
	{
		if(ePersonality == MINOR_CIV_PERSONALITY_HOSTILE)
		{
			iCount *= 200; //antonjs: todo: XML
			iCount /= 100;
		}
		else if(ePersonality == MINOR_CIV_PERSONALITY_IRRATIONAL)
		{
			iCount *= 150; //antonjs: todo: xml
			iCount /= 100;
		}
		else if(ePersonality == MINOR_CIV_PERSONALITY_FRIENDLY)
		{
			iCount *= 30; //antonjs: todo: xml
			iCount /= 100;
		}
	}

	// Denounce target Major
	else if(eQuest == MINOR_CIV_QUEST_DENOUNCE_MAJOR)
	{
		if(eTrait == MINOR_CIV_TRAIT_RELIGIOUS)
		{
			iCount *= 250; //antonjs: todo: xml
			iCount /= 100;
		}
		else if(eTrait == MINOR_CIV_TRAIT_MARITIME)
		{
			iCount *= 200; //antonjs: todo: xml
			iCount /= 100;
		}
		else
		{
			iCount *= 150; //antonjs: todo: xml
			iCount /= 100;
		}
	}

	// Spread your religion to us
	else if(eQuest == MINOR_CIV_QUEST_SPREAD_RELIGION)
	{
		if(eTrait == MINOR_CIV_TRAIT_RELIGIOUS)
		{
			iCount *= 300; //antonjs: todo: xml
			iCount /= 100;
		}
	}

	// Trade Route
	else if(eQuest == MINOR_CIV_QUEST_TRADE_ROUTE)
	{
		if(eTrait == MINOR_CIV_TRAIT_MARITIME)
		{
			iCount *= 200; //xml
			iCount /= 100;
		}
		else if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
		{
			iCount *= 200; //xml
			iCount /= 100;
		}
	}

#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	// War with Major
	else if(eQuest == MINOR_CIV_QUEST_WAR)
	{
		if(eTrait == MINOR_CIV_TRAIT_MILITARISTIC) //How dare they bully us!
		{
			iCount *= GC.getMINOR_CIV_QUEST_WAR_MILITARISTIC_VALUE(); //xml
			iCount /= 100;
		}
		else if(ePersonality == MINOR_CIV_PERSONALITY_HOSTILE) //Arrrgh!
		{
			iCount *= GC.getMINOR_CIV_QUEST_WAR_HOSTILE_VALUE(); //xml
			iCount /= 100;
		}
		else if(eTrait == MINOR_CIV_TRAIT_CULTURED) //Help!!
		{
			iCount *= GC.getMINOR_CIV_QUEST_WAR_CULTURED_VALUE(); //xml
			iCount /= 100;
		}
		else
		{
			iCount *= 125; //xml
			iCount /= 100;
		}
	}
	// CONSTRUCT A NATIONAL WONDER
	else if(eQuest == MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER)
	{
		if(eTrait == MINOR_CIV_TRAIT_CULTURED)						// Cultured
		{
			iCount *= GC.getMINOR_CIV_QUEST_NATIONAL_WONDER_CULTURED_VALUE();
			iCount /= 100;
		}
		else
		{
			iCount *= 75;
			iCount /= 100;
		}
	}
	// FIND ANOTHER MINOR
	else if(eQuest == MINOR_CIV_QUEST_FIND_CITY_STATE)
	{
		if(eTrait == MINOR_CIV_TRAIT_MARITIME)						// Maritime
		{
			iCount *= GC.getMINOR_CIV_QUEST_FIND_CITYSTATE_MARITIME_VALUE();
			iCount /= 100;
		}
		else if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
		{
			iCount *= GC.getMINOR_CIV_QUEST_FIND_CITYSTATE_MERCANTILE_VALUE();
			iCount /= 100;
		}
	}
	else if(eQuest == MINOR_CIV_QUEST_ARCHAEOLOGY)
	{
		if(eTrait == MINOR_CIV_TRAIT_MILITARISTIC) //Recover the spoils of ancient war!
		{
			iCount *= GC.getMINOR_CIV_QUEST_ARCHAEOLOGY_MILITARISTIC_VALUE();
			iCount /= 100;
		}
		else if(eTrait == MINOR_CIV_TRAIT_RELIGIOUS) //Religious relics, you say?
		{
			iCount *= GC.getMINOR_CIV_QUEST_ARCHAEOLOGY_RELIGIOUS_VALUE();
			iCount /= 100;
		}
		else
		{
			iCount *= 75;
			iCount /= 100;
		}
	}
	else if(eQuest == MINOR_CIV_QUEST_CIRCUMNAVIGATION)
	{
		if(eTrait == MINOR_CIV_TRAIT_MARITIME)	//We are the masters of the sea!					
		{
			iCount *= GC.getMINOR_CIV_QUEST_CIRCUMNAVIGATION_MARITIME_VALUE();
			iCount /= 100;
		}
	}
	// LIBERATE A CITY STATE
	else if(eQuest == MINOR_CIV_QUEST_LIBERATION)
	{
		if(eTrait == MINOR_CIV_TRAIT_RELIGIOUS)					// Let us all be peaceful!
		{
			iCount *= GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_RELIGIOUS_LIBERATION();
			iCount /= 100;
		}
		if(ePersonality == MINOR_CIV_PERSONALITY_FRIENDLY)		// Our friends were in that city!
		{
			iCount *= GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_LIBERATION();
			iCount /= 100;
		}
		if(ePersonality == MINOR_CIV_TRAIT_MILITARISTIC)		// War beckons - will you answer?
		{
			iCount *= GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_LIBERATION();
			iCount /= 100;
		}
		else
		{
			iCount += 50;
			iCount /= 100;
		}
	}
#endif

	// ******************
	// Global Quests
	// ******************

	// CONTEST CULTURE
	else if(eQuest == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{
		if(eTrait == MINOR_CIV_TRAIT_CULTURED)
		{
			iCount *= 200; //antonjs: todo: XML
			iCount /= 100;
		}
	}
	// CONTEST FAITH
	else if(eQuest == MINOR_CIV_QUEST_CONTEST_FAITH)
	{
		if(eTrait == MINOR_CIV_TRAIT_RELIGIOUS)
		{
			iCount *= 200; //antonjs: todo: XML
			iCount /= 100;
		}
		else
		{
			iCount *= 50; //antonjs: todo: XML
			iCount /= 100;
		}
	}
	// CONTEST TECHS
	else if(eQuest == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		if(eTrait == MINOR_CIV_TRAIT_RELIGIOUS)
		{
			iCount += 50; //antonjs: todo: XML
			iCount /= 100;
		}
	}

	// KILL A CAMP
	else if(eQuest == MINOR_CIV_QUEST_KILL_CAMP)
	{
		iCount *= 300;
		iCount /= 100;

		if(eTrait == MINOR_CIV_TRAIT_MILITARISTIC)					// Militaristic
		{
			iCount *= /*300*/ GC.getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_KILL_CAMP();
			iCount /= 100;
		}
	}

	// Invest
	else if(eQuest == MINOR_CIV_QUEST_INVEST)
	{
		if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
		{
			iCount *= 150; //antonjs: todo: XML
			iCount /= 100;
		}
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	// Influence
	else if(eQuest == MINOR_CIV_QUEST_INFLUENCE)
	{
		if(ePersonality == MINOR_CIV_PERSONALITY_HOSTILE) //Leave us alone!
		{
			iCount += GC.getMINOR_CIV_QUEST_INFLUENCE_HOSTILE_VALUE();
			iCount /= 100;
		}
		else
		{
			iCount *= 70;
			iCount /= 100;
		}
	}
	// Tourism
	else if(eQuest == MINOR_CIV_QUEST_CONTEST_TOURISM)
	{
		if(eTrait == MINOR_CIV_PERSONALITY_FRIENDLY)
		{
			iCount *= GC.getMINOR_CIV_QUEST_TOURISM_FRIENDLY_VALUE();
			iCount /= 100;
		}
	}
	// DESTROY A HORDE
	else if(eQuest == MINOR_CIV_QUEST_HORDE)
	{
		iCount *= GC.getBARBARIAN_HORDE_FREQUENCY();
		iCount /= 100;
	}

	// Rebellion!
	else if(eQuest == MINOR_CIV_QUEST_REBELLION)
	{
		iCount *= GC.getQUEST_REBELLION_FREQUENCY();
		iCount /= 100;
	}
#endif

	return iCount / 10;
}

/// ASSUMPTION: There is no more than one active quest of any given MinorCivQuestTypes
int CvMinorCivAI::GetNumActiveGlobalQuests() const
{
	//antonjs: todo: reevaluate this for efficiency and for use of std::set

	std::set<MinorCivQuestTypes> setGlobalQuests;

	for(uint iPlayerLoop = 0; iPlayerLoop < m_QuestsGiven.size(); iPlayerLoop++)
	{
		for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[iPlayerLoop].size(); iQuestLoop++)
		{
			if(IsGlobalQuest(m_QuestsGiven[iPlayerLoop][iQuestLoop].GetType()))
			{
				setGlobalQuests.insert(m_QuestsGiven[iPlayerLoop][iQuestLoop].GetType());
			}
		}
	}

	return setGlobalQuests.size();
}

int CvMinorCivAI::GetNumActiveQuestsForPlayer(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0;

	return m_QuestsGiven[ePlayer].size();
}

int CvMinorCivAI::GetNumActivePersonalQuestsForPlayer(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0;

	int iCount = 0;
	for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(IsPersonalQuest(m_QuestsGiven[ePlayer][iQuestLoop].GetType()))
		{
			iCount++;
		}
	}

	return iCount;
}

bool CvMinorCivAI::IsActiveQuestForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eType)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	CvAssertMsg(eType >= NO_MINOR_CIV_QUEST_TYPE, "eType is expected to be non-negative (invalid Index)");
	CvAssertMsg(eType < NUM_MINOR_CIV_QUEST_TYPES, "eType is expected to be within maximum bounds (invalid Index)");
	if(eType < NO_MINOR_CIV_QUEST_TYPE || eType >= NUM_MINOR_CIV_QUEST_TYPES) return false;

	for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			return true;
		}
	}

	return false;
}

/// Get rid of all active quests
void CvMinorCivAI::EndAllActiveQuestsForPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	//antonjs: todo: instead, call for cancel quest (with flag for no notif)
	if(GetNumActiveQuestsForPlayer(ePlayer) > 0)
		m_QuestsGiven[ePlayer].clear();
}

int CvMinorCivAI::GetNumDisplayedQuestsForPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0;

	int iValue = 0;
	
	for (uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(!m_QuestsGiven[ePlayer][iQuestLoop].IsHandled())
		{
			iValue++;
		}
	}

	return iValue;
}

bool CvMinorCivAI::IsDisplayedQuestForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eType)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	CvAssertMsg(eType >= NO_MINOR_CIV_QUEST_TYPE, "eType is expected to be non-negative (invalid Index)");
	CvAssertMsg(eType < NUM_MINOR_CIV_QUEST_TYPES, "eType is expected to be within maximum bounds (invalid Index)");
	if(eType < NO_MINOR_CIV_QUEST_TYPE || eType >= NUM_MINOR_CIV_QUEST_TYPES) return false;

	for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			if(!m_QuestsGiven[ePlayer][iQuestLoop].IsHandled())
			{
				return true;
			}
		}
	}

	return false;
}

void CvMinorCivAI::DoTestSeedGlobalQuestCountdown(bool bForceSeed)
{
	if(!bForceSeed)
	{
		// Only seed if there is a potential spot for a global quest
		if(GetNumActiveGlobalQuests() >= GetMaxActiveGlobalQuests())
		{
			return;
		}

		// Only seed if the countdown is not already active
		if(GetGlobalQuestCountdown() > 0)
		{
			return;
		}
	}

	int iNumTurns = 0;

	// Quests are now available for the first time?
	if(GC.getGame().getElapsedGameTurns() == GetFirstPossibleTurnForGlobalQuests())
	{
		int iRand = /*20*/ GC.getMINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN_RAND();
		iNumTurns += GC.getGame().getJonRandNum(iRand, "Random # of turns for Minor Civ global quest counter - first time.");
	}
	else
	{
		iNumTurns += /*40*/ GC.getMINOR_CIV_GLOBAL_QUEST_MIN_TURNS_BETWEEN();

		int iRand = /*25*/ GC.getMINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN();
		if(GetPersonality() == MINOR_CIV_PERSONALITY_HOSTILE)
		{
			iRand *= /*200*/ GC.getMINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER();
			iRand /= 100;
		}
		iNumTurns += GC.getGame().getJonRandNum(iRand, "Random # of turns for Minor Civ global quest counter.");
	}

	// Modify for Game Speed
	iNumTurns *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iNumTurns /= 100;

	SetGlobalQuestCountdown(iNumTurns);
}

int CvMinorCivAI::GetGlobalQuestCountdown()
{
	return m_iGlobalQuestCountdown;
}

void CvMinorCivAI::SetGlobalQuestCountdown(int iValue)
{
	m_iGlobalQuestCountdown = iValue;
}

void CvMinorCivAI::ChangeGlobalQuestCountdown(int iChange)
{
	SetGlobalQuestCountdown(GetGlobalQuestCountdown() + iChange);
}

/// How long before we trigger a new quest?
void CvMinorCivAI::DoTestSeedQuestCountdownForPlayer(PlayerTypes ePlayer, bool bForceSeed)
{
	if(!bForceSeed)
	{
		// Only seed if there is a potential spot for a personal quest for this player
		if(GetNumActivePersonalQuestsForPlayer(ePlayer) >= GetMaxActivePersonalQuestsForPlayer())
		{
			return;
		}

		// Only seed if the countdown for this player is not already active
		if(GetQuestCountdownForPlayer(ePlayer) > 0)
		{
			return;
		}
	}

	int iNumTurns = 0;

	// Quests are now available for the first time?
	if(GC.getGame().getElapsedGameTurns() == GetFirstPossibleTurnForPersonalQuests())
	{
		int iRand = /*20*/ GC.getMINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN_RAND();
		iNumTurns += GC.getGame().getJonRandNum(iRand, "Random # of turns for Minor Civ personal quest counter - first time.");
	}
	else
	{
		iNumTurns += /*20*/ GC.getMINOR_CIV_PERSONAL_QUEST_MIN_TURNS_BETWEEN();

		int iRand = /*25*/ GC.getMINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN();
		if(GetPersonality() == MINOR_CIV_PERSONALITY_HOSTILE)
		{
			iRand *= /*200*/ GC.getMINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER();
			iRand /= 100;
		}
		iNumTurns += GC.getGame().getJonRandNum(iRand, "Random # of turns for Minor Civ personal quest counter.");
	}

	// Modify for Game Speed
	iNumTurns *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iNumTurns /= 100;

	SetQuestCountdownForPlayer(ePlayer, iNumTurns);
}

/// How many turns since the last Quest ended?
int CvMinorCivAI::GetQuestCountdownForPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1; // as set during Reset()

	return m_aiQuestCountdown[ePlayer];
}

/// Sets How many turns since the last Quest ended
void CvMinorCivAI::SetQuestCountdownForPlayer(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_aiQuestCountdown[ePlayer] = iValue;
}

/// Changes many turns since the last Quest ended
void CvMinorCivAI::ChangeQuestCountdownForPlayer(PlayerTypes ePlayer, int iChange)
{
	SetQuestCountdownForPlayer(ePlayer, GetQuestCountdownForPlayer(ePlayer) + iChange);
}

/// For debugging and testing only. Adds eQuest if able, ignoring quest countdown timers.
bool CvMinorCivAI::AddQuestIfAble(PlayerTypes eMajor, MinorCivQuestTypes eQuest)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;

	CvAssertMsg(eQuest >= NO_MINOR_CIV_QUEST_TYPE, "eQuest is expected to be non-negative (invalid Index)");
	CvAssertMsg(eQuest < NUM_MINOR_CIV_QUEST_TYPES, "eQuest is expected to be within maximum bounds (invalid Index)");
	if(eQuest < NO_MINOR_CIV_QUEST_TYPE || eQuest >= NUM_MINOR_CIV_QUEST_TYPES) return false;

	if(!IsHasMetPlayer(eMajor))
		return false;

	if(IsAtWarWithPlayersTeam(eMajor))
		return false;

	if (!IsValidQuestForPlayer(eMajor, eQuest))
		return false;

	if (IsActiveQuestForPlayer(eMajor, eQuest))
		return false;

	CvAssert(IsPersonalQuest(eQuest) || IsGlobalQuest(eQuest));
	if (IsPersonalQuest(eQuest))
	{
		if(GetNumActivePersonalQuestsForPlayer(eMajor) >= GetMaxActivePersonalQuestsForPlayer())
		{
			return false;
		}

		AddQuestForPlayer(eMajor, eQuest, GC.getGame().getGameTurn());
		return true;
	}
	else if (IsGlobalQuest(eQuest))
	{
		if(GetNumActiveGlobalQuests() >= GetMaxActiveGlobalQuests())
		{
			return false;
		}

		int iNumValidPlayers = 0;
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes ePlayerLoop = (PlayerTypes) iPlayerLoop;
			if(IsValidQuestForPlayer(ePlayerLoop, eQuest))
				iNumValidPlayers++;
		}
		if(iNumValidPlayers < GetMinPlayersNeededForQuest(eQuest))
			return false;

		for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes ePlayerLoop = (PlayerTypes) iPlayerLoop;
			if(IsValidQuestForPlayer(ePlayerLoop, eQuest))
				AddQuestForPlayer(ePlayerLoop, eQuest, GC.getGame().getGameTurn());
		}
		return true;
	}

	return false;
}

/// What is Data member 1
int CvMinorCivAI::GetQuestData1(PlayerTypes ePlayer, MinorCivQuestTypes eType) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) CvMinorCivQuest::NO_QUEST_DATA;

	for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			return m_QuestsGiven[ePlayer][iQuestLoop].GetPrimaryData();
		}
	}

	return CvMinorCivQuest::NO_QUEST_DATA;
}

/// What is Data member 2
int CvMinorCivAI::GetQuestData2(PlayerTypes ePlayer, MinorCivQuestTypes eType) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return CvMinorCivQuest::NO_QUEST_DATA;

	for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			return m_QuestsGiven[ePlayer][iQuestLoop].GetSecondaryData();
		}
	}

	return CvMinorCivQuest::NO_QUEST_DATA;
}

int CvMinorCivAI::GetQuestTurnsRemaining(PlayerTypes ePlayer, MinorCivQuestTypes eType, int iGameTurn) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return CvMinorCivQuest::NO_TURN;

	for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			return m_QuestsGiven[ePlayer][iQuestLoop].GetTurnsRemaining(iGameTurn);
		}
	}

	return CvMinorCivQuest::NO_TURN;
}

bool CvMinorCivAI::IsContestLeader(PlayerTypes ePlayer, MinorCivQuestTypes eType)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	for (uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if (m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			return m_QuestsGiven[ePlayer][iQuestLoop].IsContestLeader(ePlayer);
		}
	}

	return false;
}

int CvMinorCivAI::GetContestValueForLeader(MinorCivQuestTypes eType)
{
	PlayerTypes eParticipant;
	for (int iMajor = 0; iMajor < MAX_MAJOR_CIVS; iMajor++)
	{
		eParticipant = (PlayerTypes) iMajor;
		for (uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[eParticipant].size(); iQuestLoop++)
		{
			if (m_QuestsGiven[eParticipant][iQuestLoop].GetType() == eType)
			{
				return m_QuestsGiven[eParticipant][iQuestLoop].GetContestValueForLeader();
			}
		}
	}

	return -1;
}

int CvMinorCivAI::GetContestValueForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eType)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	for (uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if (m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			return m_QuestsGiven[ePlayer][iQuestLoop].GetContestValueForPlayer(ePlayer);
		}
	}

	return -1;
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

	if(m_abRouteConnectionEstablished[eMajor] != bValue)
	{
		m_abRouteConnectionEstablished[eMajor] = bValue;
	}
}

/// Any Camps near us?
/// NOTE: This should pick a camp deterministically, given the current implementation of distributing global quests
CvPlot* CvMinorCivAI::GetBestNearbyCampToKill()
{
	CvCity* pCapital = GetPlayer()->getCapitalCity();

	// Minor must have Capital
	if(pCapital == NULL)
	{
		return NULL;
	}

	CvWeightedVector<int, 64, true> viPlotIndexes; // 64 camps in 12 hex radius should be enough

	int iRange = /*12*/ GC.getMINOR_CIV_QUEST_KILL_CAMP_RANGE();

	CvPlot* pLoopPlot;

	// Loop in all plots in range
	int iDX, iDY;
	for(iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for(iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pLoopPlot = plotXY(pCapital->getX(), pCapital->getY(), iDX, iDY);

			if(pLoopPlot != NULL)
			{
				// Camp must be in the same Area as us
				if(pLoopPlot->getArea() != pCapital->getArea())
				{
					continue;
				}

				int iDistance = plotDistance(pCapital->getX(), pCapital->getY(), pLoopPlot->getX(), pLoopPlot->getY());

				if(iDistance <= iRange)
				{
					// Can't be owned by anyone
					if(pLoopPlot->getOwner() == NO_PLAYER)
					{
						// Camp here?
						if(pLoopPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
						{
							int iWeight = 1 + (iRange - iDistance); // Closer camps have higher weight
							viPlotIndexes.push_back(pLoopPlot->GetPlotIndex(), iWeight);
						}
					}
				}
			}
		}
	}

	// Didn't find any nearby
	if(viPlotIndexes.size() == 0)
	{
		return NULL;
	}

	// Choose the best plot
	viPlotIndexes.SortItems();
	CvPlot* pBestPlot = GC.getMap().plotByIndex(viPlotIndexes.GetElement(0));

	return pBestPlot;
}

#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
/// Any dig sites near us?
/// NOTE: This should pick a dig deterministically, given the current implementation of distributing global quests
CvPlot* CvMinorCivAI::GetBestNearbyDig()
{
	CvCity* pCapital = GetPlayer()->getCapitalCity();

	// Minor must have Capital
	if(pCapital == NULL)
	{
		return NULL;
	}

	CvWeightedVector<int, 64, true> viPlotIndexes; // 64 camps in 12 hex radius should be enough

	int iRange = 12;

	CvPlot* pLoopPlot;

	// Loop in all plots in range
	int iDX, iDY;
	for(iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for(iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pLoopPlot = plotXY(pCapital->getX(), pCapital->getY(), iDX, iDY);

			if(pLoopPlot != NULL)
			{
				int iDistance = plotDistance(pCapital->getX(), pCapital->getY(), pLoopPlot->getX(), pLoopPlot->getY());

				if(iDistance <= iRange)
				{
					// Dig site here?
					if(pLoopPlot->getResourceType() == GC.getARTIFACT_RESOURCE())
					{
						int iWeight = 1 + (iRange - iDistance); // Closer digs have higher weight
						viPlotIndexes.push_back(pLoopPlot->GetPlotIndex(), iWeight);
					}
				}
			}
		}
	}

	// Didn't find any nearby
	if(viPlotIndexes.size() == 0)
	{
		return NULL;
	}

	// Choose the best plot
	viPlotIndexes.SortItems();
	CvPlot* pBestPlot = GC.getMap().plotByIndex(viPlotIndexes.GetElement(0));

	return pBestPlot;
}

/// Horde Spawning Time!
/// NOTE: This should tell us if it is time to spawn a horde - it is based on the wealth and size of CSs - stronger = better target!
PlayerTypes CvMinorCivAI::SpawnHorde()
{
	if(!IsValidRebellion())
	{
		return NO_PLAYER;
	}
	PlayerTypes pActiveMinor = GetPlayer()->GetID();
	EraTypes eAssumeEra = NO_ERA;
	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)

		eCurrentEra = GET_TEAM(GET_PLAYER(pActiveMinor).getTeam()).GetCurrentEra();
		
		EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);
		EraTypes eClassical = (EraTypes) GC.getInfoTypeForString("ERA_CLASSICAL", true);

	//This is just for Classical/Medieval/Renaissance Eras.
	if(eCurrentEra < eClassical)
	{
		return NO_PLAYER;
	}
	else if(eCurrentEra > eRenaissance)
	{
		return NO_PLAYER;;
	}

	int iTarget = 0;

	//Top 20%
	int iTopTier = (GC.getGame().GetNumMinorCivsEver() / 4);
	if(iTopTier <= 0)
	{
		iTopTier = 1;
	}

	CvWeightedVector<PlayerTypes, MAX_CIV_PLAYERS, true> veMinorRankings;
	
	for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		PlayerTypes eMinorLoop = (PlayerTypes) iMinorLoop;

		CvPlayer* pMinorLoop = &GET_PLAYER(eMinorLoop);

		CvCity* pCity = pMinorLoop->getCapitalCity();
		
		//Let's see if our CS is juicy and vulnerable.
		if(pMinorLoop->isAlive() && pMinorLoop->getStartingPlot()->getOwner() == eMinorLoop && pMinorLoop->isMinorCiv())
		{
			CvPlot* pPlot;

			CvCityCitizens* pCitizens = pCity->GetCityCitizens();

			int iPlots = 0;
			int iWater = 0;
			int iImpassable = 0;

			// How easy to access is this minor? We'll ignore island/mountainous CSs for this quest, to help the AI.
#if defined(MOD_GLOBAL_CITY_WORKING)
			for(int iPlotLoop = 1; iPlotLoop < pCity->GetNumWorkablePlots(); iPlotLoop++)
#else
			for(int iPlotLoop = 1; iPlotLoop < NUM_CITY_PLOTS; iPlotLoop++)
#endif
			{
				pPlot = pCitizens->GetCityPlotFromIndex(iPlotLoop);

				if(pPlot)
				{
					if(pPlot->isWater())
					{
						iWater++;
					}
					if(pPlot->isImpassable() || pPlot->isMountain())
					{
						iImpassable++;
					}
				}
				iPlots++;
			}
			//50% Water? Abort. Probably an island.
			if(iWater >= (iPlots / 2))
			{
				continue;
			}

			//50% Mountains? Abort. Probably Minas Tirith.
			if(iImpassable >= (iPlots / 2))
			{
				continue;
			}

			//Baseline is population.
			iTarget = pCity->getPopulation();

			// Gold increases proclivity.
			iTarget += (pMinorLoop->GetTreasury()->GetGold() / 10);
			iTarget += pMinorLoop->GetTreasury()->GetImprovementGoldMaintenance();
			iTarget += pMinorLoop->GetTreasury()->CalculateBaseNetGold();
			iTarget += pMinorLoop->GetTrade()->GetNumDifferentTradingPartners();

			//Less military units = higher score.
			iTarget -= pMinorLoop->getNumMilitaryUnits();

			if(iTarget > 0)
			{
				veMinorRankings.push_back(eMinorLoop, iTarget);
			}
		}
	}

	veMinorRankings.SortItems();
	if(veMinorRankings.size() != 0)
	{
		for(int iRanking = 0; iRanking < veMinorRankings.size(); iRanking++)
		{
			if(veMinorRankings.GetElement(iRanking) == pActiveMinor)
			{
				//Are we in the top 25% of CSs? If so, the quest can fire.
				if(iRanking <= iTopTier)
				{
					return pActiveMinor;
				}
			}
		}
	}
	
	return NO_PLAYER;
}

/// Rebel Spawning Time!
/// NOTE: This should tell us if it is time to spawn rebels
PlayerTypes CvMinorCivAI::SpawnRebels()
{
	if(!IsValidRebellion())
	{
		return NO_PLAYER;
	}

	PlayerTypes pActiveMinor = GetPlayer()->GetID();
	PlayerTypes ePlayer = GetPlayer()->GetMinorCivAI()->GetAlly();

	int iRebelBuildUp = 0;

	PublicOpinionTypes eOpinionInMyCiv = m_pPlayer->GetCulture()->GetPublicOpinionType();
	PlayerProximityTypes eProximity;
	eProximity = GET_PLAYER(pActiveMinor).GetProximityToPlayer(ePlayer);

	//Base value is influence with CS / 100 (i.e. 80 -> 8).
	int iRebelBoilPoint = GetPlayer()->GetMinorCivAI()->GetEffectiveFriendshipWithMajor(ePlayer);
	iRebelBoilPoint /= 10;
	
	//Hard cap at 250 influence (25)
	if(iRebelBoilPoint > 25)
	{
		iRebelBoilPoint = 25;
	}
	
	if(ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).isAlive() && !GET_PLAYER(ePlayer).isMinorCiv())
	{
		if(GET_PLAYER(ePlayer).IsEmpireUnhappy())
		{
			iRebelBuildUp += (GET_PLAYER(ePlayer).GetExcessHappiness() * -1);	
		}
		if(GET_PLAYER(ePlayer).IsEmpireVeryUnhappy())
		{
			iRebelBoilPoint /= 2;
		}
		if(GET_PLAYER(ePlayer).GetCulture()->GetPublicOpinionUnhappiness() > 0)
		{
			iRebelBuildUp += GET_PLAYER(ePlayer).GetCulture()->GetPublicOpinionUnhappiness();
		}
		if (eOpinionInMyCiv >= PUBLIC_OPINION_CIVIL_RESISTANCE)
		{
			iRebelBoilPoint /= 2;
		}
		//How close are we? Further away = higher chance of rebellion.
		if(eProximity >= PLAYER_PROXIMITY_CLOSE)
		{
			iRebelBoilPoint += ((iRebelBoilPoint * 125) / 100);
		}
		else if(eProximity < PLAYER_PROXIMITY_CLOSE)
		{
			iRebelBoilPoint += ((iRebelBoilPoint * 100) / 125);
		}
		TeamTypes eLoopTeam;
		int iWar = 0;
		//CSs don't like war.
		for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
		{
			eLoopTeam = (TeamTypes) iTeamLoop;

			if(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(eLoopTeam))
			{
				iWar++;
			}
		}
		if(iWar > 0)
		{
			iRebelBuildUp += iWar;
		}

		iRebelBuildUp += GC.getGame().getJonRandNum(GC.getGame().getCurrentEra(), "Rebel possibility rand roll");

		if(iRebelBuildUp >= iRebelBoilPoint)
		{
			return pActiveMinor;
		}
	}

	return NO_PLAYER;
}

void CvMinorCivAI::SetSacked(bool bValue)
{
	if(m_bIsSacked != bValue)
	{
		m_bIsSacked = bValue;
	}
}

bool CvMinorCivAI::IsSacked()
{
	return m_bIsSacked;
}

bool CvMinorCivAI::IsRebellion()
{
	return m_bIsRebellion;
}

void CvMinorCivAI::SetRebellion(bool bValue)
{
	if(m_bIsRebellion != bValue)
	{
		m_bIsRebellion = bValue;
	}
}

void CvMinorCivAI::ChangeTurnsSinceRebellion(int iChange)
{
	SetTurnsSinceRebellion(GetTurnsSinceRebellion() + iChange);
}

int CvMinorCivAI::GetTurnsSinceRebellion() const
{
	return m_iIsRebellionCountdown;
}

void CvMinorCivAI::SetTurnsSinceRebellion(int iValue)
{
	if(GetTurnsSinceRebellion() != iValue)
		m_iIsRebellionCountdown = iValue;
}

void CvMinorCivAI::DoRebellion()
{
	// In hundreds
	int iNumRebels = (GetPlayer()->getNumMilitaryUnits() * 100); //Based on number of military units of CS.
	int iExtraRoll = 150; //1+ Rebels maximum
	iExtraRoll += (GC.getGame().getCurrentEra() * 50); //Increase possible rebel spawns as game continues.
	iNumRebels += GC.getGame().getJonRandNum(iExtraRoll, "Rebel count rand roll");
	iNumRebels /= 100;
	CvGame& theGame = GC.getGame();
	PlayerTypes eMinor = GetPlayer()->GetID();

	// Find a city to pop up a bad man
	CvCity* pBestCity = GetPlayer()->getCapitalCity();

	// Found a place to set up an uprising?
	if(pBestCity != NULL)
	{
		int iBestPlot = -1;
		int iBestPlotWeight = -1;
		CvPlot* pPlot;

		CvCityCitizens* pCitizens = pBestCity->GetCityCitizens();

		// Start at 1, since ID 0 is the city plot itself
#if defined(MOD_GLOBAL_CITY_WORKING)
		for(int iPlotLoop = 1; iPlotLoop < pBestCity->GetNumWorkablePlots(); iPlotLoop++)
#else
		for(int iPlotLoop = 1; iPlotLoop < NUM_CITY_PLOTS; iPlotLoop++)
#endif
		{
			pPlot = pCitizens->GetCityPlotFromIndex(iPlotLoop);

			if(!pPlot)		// Should be valid, but make sure
				continue;

			// Can't be impassable
			if(pPlot->isImpassable() || pPlot->isMountain())
				continue;

			// Can't be water
			if(pPlot->isWater())
				continue;

			// Can't be ANOTHER city
			if(pPlot->isCity())
				continue;

			// Don't place on a plot where a unit is already standing
			if(pPlot->getNumUnits() > 0)
				continue;

			int iTempWeight = theGame.getJonRandNum(10, "Uprising rand plot location.");

			// Add weight if there's an improvement here!
			if(pPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				iTempWeight += 4;

				// If also a a resource, even more weight!
				if(pPlot->getResourceType(GetPlayer()->getTeam()) != NO_RESOURCE)
					iTempWeight += 3;
			}
			
			// Don't pick plots that aren't ours
			if(pPlot->getOwner() != eMinor)
				iTempWeight = -1;

			// Add weight if there's a defensive bonus for this plot
			if(pPlot->defenseModifier(BARBARIAN_TEAM, false, false))
				iTempWeight += 4;

			if(iTempWeight > iBestPlotWeight)
			{
				iBestPlotWeight = iTempWeight;
				iBestPlot = iPlotLoop;
			}
		}

		// Found valid plot
		if(iBestPlot != -1)
		{
			// Make barbs able to enter ANYONE'S territory
			theGame.SetBarbarianReleaseTurn(0);

			pPlot = pCitizens->GetCityPlotFromIndex(iBestPlot);

			// Pick a unit type - shoudl give us more melee than ranged
			UnitTypes eUnit = theGame.GetRandomSpawnUnitType(eMinor, /*bIncludeUUs*/ true, /*bIncludeRanged*/ true);
			UnitTypes emUnit = theGame.GetRandomSpawnUnitType(eMinor, /*bIncludeUUs*/ true, /*bIncludeRanged*/ false);

			// Init unit
			GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pPlot->getX(), pPlot->getY());
			iNumRebels--;	// Reduce the count since we just added the seed rebel

			// Loop until all rebels are placed
			do
			{
				iNumRebels--;

				// Init unit
				CvUnit* pmUnit = GET_PLAYER(BARBARIAN_PLAYER).initUnit(emUnit, pPlot->getX(), pPlot->getY());
				CvAssert(pmUnit);
				if (pmUnit)
				{
					if (!pmUnit->jumpToNearestValidPlotWithinRange(3))
						pmUnit->kill(false);		// Could not find a spot!
				}

				iNumRebels--;

				// Init unit
				CvUnit* pUnit = GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pPlot->getX(), pPlot->getY());
				CvAssert(pUnit);
				if (pUnit)
				{
					if (!pUnit->jumpToNearestValidPlotWithinRange(3))
						pUnit->kill(false);		// Could not find a spot!
				}
			}
			while(iNumRebels > 0);
		}
	}
}

bool CvMinorCivAI::IsValidRebellion()
{
	//Test to keep quests from firing over and over if ended.
	if(GetCooldownSpawn() > 0)
	{
		ChangeCooldownSpawn(-1);
		if(GetCooldownSpawn() != 0 )
		{
			return false;
		}
	}

	int iActiveRebellions = 0;

	//Call every minor, otherwise no minor is aware of what other minors have given out.

	for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		PlayerTypes eMinorLoop = (PlayerTypes) iMinorLoop;

		CvPlayer* pMinorLoop = &GET_PLAYER(eMinorLoop);

		if (pMinorLoop && pMinorLoop->isAlive() && pMinorLoop->isMinorCiv() && (pMinorLoop != GetPlayer()))
		{
			if(pMinorLoop->GetMinorCivAI()->IsHordeActive())
			{
				iActiveRebellions++;
			}
			if(pMinorLoop->GetMinorCivAI()->IsRebellionActive())
			{
				iActiveRebellions++;
			}
		}
	}

	int iActivePlayers = GC.getGame().countMajorCivsAlive();
	//Let's make this more granular.
	iActiveRebellions  *= 100;
	iActivePlayers *= 10;

	int iMaxQuests = (iActiveRebellions / iActivePlayers);

	//If there are more quests active than 50% of all civs, the quest should abort, as that's too many for the AI to handle.
	if(iMaxQuests >= 5)
	{
		return false;
	}
	return true;
}

bool CvMinorCivAI::IsRebellionActive()
{
	return m_bIsRebellionActive;
}

void CvMinorCivAI::SetRebellionActive(bool bValue)
{
	if(m_bIsRebellionActive != bValue)
	{
		m_bIsRebellionActive = bValue;
	}
}

bool CvMinorCivAI::IsHordeActive()
{
	return m_bIsHordeActive;
}

void CvMinorCivAI::SetHordeActive(bool bValue)
{
	if(m_bIsHordeActive != bValue)
	{
		m_bIsHordeActive = bValue;
	}
}

//Cooldown
void CvMinorCivAI::ChangeCooldownSpawn(int iChange)
{
	SetCooldownSpawn(GetCooldownSpawn() + iChange);
}

int CvMinorCivAI::GetCooldownSpawn() const
{
	return m_iCooldownSpawn;
}

void CvMinorCivAI::SetCooldownSpawn(int iValue)
{
	if(GetCooldownSpawn() != iValue)
		m_iCooldownSpawn = iValue;
}
#endif

/// Find a Resource that a Minor would want a major to connect
ResourceTypes CvMinorCivAI::GetNearbyResourceForQuest(PlayerTypes ePlayer)
{
	ResourceTypes eBestResource = NO_RESOURCE;

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	if(GET_PLAYER(ePlayer).getStartingPlot() != NULL)
	{
		CvArea* pPlayerArea = GC.getMap().getArea(GET_PLAYER(ePlayer).getStartingPlot()->getArea());

		FStaticVector<ResourceTypes, 64, true, c_eCiv5GameplayDLL, 0> veValidResources; // 64 resources should be way more than enough
		TechTypes eRevealTech;
		TechTypes eConnectTech;

		// Loop through all Resources and see if they're useful
		ResourceTypes eResource;
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			eResource = (ResourceTypes) iResourceLoop;

			const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			// Must not be a plain ol' bonus resource
			if(pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_BONUS)
			{
				continue;
			}

			// Minor can't already have this Resource
			if(GetPlayer()->getNumResourceTotal(eResource, /*bIncludeImport*/ true) > 0)
			{
				continue;
			}

			// Player can't already have this Resource
			if(GET_PLAYER(ePlayer).getNumResourceTotal(eResource, /*bIncludeImport*/ true) > 0)
			{
				continue;
			}

			// Player has to be able to see it
			eRevealTech = (TechTypes) pkResourceInfo->getTechReveal();
			if(!GET_TEAM(eTeam).GetTeamTechs()->HasTech(eRevealTech))
			{
				continue;
			}
			int iRevealPolicy = pkResourceInfo->getPolicyReveal();
			if (iRevealPolicy != NO_POLICY && !(GET_PLAYER(ePlayer).GetPlayerPolicies()->HasPolicy((PolicyTypes)iRevealPolicy)))
			{
				continue;
			}

			// Player has to be able to use it
			eConnectTech = (TechTypes) pkResourceInfo->getTechCityTrade();
			if(!GET_TEAM(eTeam).GetTeamTechs()->HasTech(eConnectTech))
			{
				continue;
			}

			// Must be this Resource on the player's area
			if(pPlayerArea->getNumResources(eResource) == 0)
			{
				continue;
			}

			veValidResources.push_back(eResource);
		}

		// Didn't find any valid Resources
		if(veValidResources.size() == 0)
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

	FStaticVector<BuildingTypes, 50, true, c_eCiv5GameplayDLL, 0> veValidBuildings; // 50 wonders should be overkill

	int iWorldPlayerLoop;
	PlayerTypes eWorldPlayer;
	CvCity* pLoopCity;
	int iCityLoop;
	int iWonderProgress;
	int iCompletionThreshold = /*25*/ GC.getMINOR_CIV_QUEST_WONDER_COMPLETION_THRESHOLD();
	bool bFoundWonderTooFarAlong;

	// Loop through all Buildings and see if they're useful
	for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

		//Skip if NULL
		if(pkBuildingInfo == NULL)
			continue;

		bFoundWonderTooFarAlong = false;

		// Must be a wonder
		if(!isWorldWonderClass(pkBuildingInfo->GetBuildingClassInfo()))
		{
			continue;
		}

		// Must be able to build it
		if(!GET_PLAYER(ePlayer).canConstruct(eBuilding))
		{
			continue;
		}

		// Someone CAN be building this wonder right now, but they can't be more than a certain % of the way done (25% by default)
		for(iWorldPlayerLoop = 0; iWorldPlayerLoop < MAX_MAJOR_CIVS; iWorldPlayerLoop++)
		{
			eWorldPlayer = (PlayerTypes) iWorldPlayerLoop;

			for(pLoopCity = GET_PLAYER(eWorldPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eWorldPlayer).nextCity(&iCityLoop))
			{
				iWonderProgress = pLoopCity->GetCityBuildings()->GetBuildingProduction(eBuilding);

				if(iWonderProgress * 100 / pLoopCity->getProductionNeeded(eBuilding) >= iCompletionThreshold)
				{
					bFoundWonderTooFarAlong = true;
					break;
				}
			}
			if(bFoundWonderTooFarAlong)
			{
				break;
			}
		}
		if(bFoundWonderTooFarAlong)
		{
			continue;
		}

		veValidBuildings.push_back(eBuilding);
	}

	// Didn't find any valid Wonders
	if(veValidBuildings.size() == 0)
	{
		return NO_BUILDING;
	}

	int iRandIndex = GC.getGame().getJonRandNum(veValidBuildings.size(), "Finding random Wonder for Minor to give out a quest to construct.");
	eBestWonder = veValidBuildings[iRandIndex];

	return eBestWonder;
}

#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
/// Best national wonder for a Quest given to ePlayer?
BuildingTypes CvMinorCivAI::GetBestNationalWonderForQuest(PlayerTypes ePlayer)
{
	BuildingTypes eBestNationalWonder;

	FStaticVector<BuildingTypes, 15, true, c_eCiv5GameplayDLL, 0> veValidBuildings; // 50 wonders should be overkill

	int iWorldPlayerLoop;
	PlayerTypes eWorldPlayer;

	// Loop through all Buildings and see if they're useful
	for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

		//Skip if NULL
		if(pkBuildingInfo == NULL)
			continue;

		// Must be a wonder
		if(!isNationalWonderClass(pkBuildingInfo->GetBuildingClassInfo()))
		{
			continue;
		}

		// Must be able to build it
		if(!GET_PLAYER(ePlayer).canConstruct(eBuilding))
		{
			continue;
		}

		// Is not a recycling center
		if (pkBuildingInfo->GetBuildingClassType() == (BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_RECYCLING_CENTER"))
		{
			continue;
		}	

		for(iWorldPlayerLoop = 0; iWorldPlayerLoop < MAX_MAJOR_CIVS; iWorldPlayerLoop++)
		{
			eWorldPlayer = (PlayerTypes) iWorldPlayerLoop;

			// If we didn't found a religion, no Grand Temple
			if(GC.getGame().GetGameReligions()->GetReligionCreatedByPlayer(eWorldPlayer) == NO_RELIGION)
			{
				if (pkBuildingInfo->GetBuildingClassType() == (BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_GRAND_TEMPLE"))
				{
					continue;
				}
			}
		}

		veValidBuildings.push_back(eBuilding);
	}

	// Didn't find any valid Wonders
	if(veValidBuildings.size() == 0)
	{
		return NO_BUILDING;
	}

	int iRandIndex = GC.getGame().getJonRandNum(veValidBuildings.size(), "Finding random Wonder for Minor to give out a quest to construct.");
	eBestNationalWonder = veValidBuildings[iRandIndex];

	return eBestNationalWonder;
}

/// Anyone that this City State would want to liberate?
/// NOTE: This makes a random choice, and is not guaranteed to return the same target if called multiple times.
/// It will, however, return NO_PLAYER reliably if there are no valid targets for eForPlayer.
PlayerTypes CvMinorCivAI::GetBestCityStateLiberate(PlayerTypes eForPlayer)
{
	CvAssertMsg(eForPlayer >= 0, "eForPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eForPlayer < MAX_MAJOR_CIVS, "eForPlayer is expected to be within maximum bounds (invalid Index)");

	PlayerTypes eBestCityStateLiberate = NO_PLAYER;

	PlayerProximityTypes eClosestProximity = PLAYER_PROXIMITY_CLOSE;

	// First, loop through the Minors in the game to what the closest proximity is to any of the players
	int iTargetLoop;
	PlayerTypes eTarget;
	for(iTargetLoop = MAX_MAJOR_CIVS; iTargetLoop < MAX_CIV_PLAYERS; iTargetLoop++)
	{
		eTarget = (PlayerTypes) iTargetLoop;

		if(GET_PLAYER(eTarget).isAlive())
			continue;

		if(GetPlayer()->getTeam() == GET_PLAYER(eTarget).getTeam())
			continue;

		if(GetPlayer()->GetProximityToPlayer(eTarget) > eClosestProximity)
		{
			eClosestProximity = GetPlayer()->GetProximityToPlayer(eTarget);
		}
	}

	// Found nobody, or only people far away
	if(eClosestProximity == PLAYER_PROXIMITY_DISTANT)
	{
		return NO_PLAYER;
	}

	FStaticVector<PlayerTypes, MAX_CIV_PLAYERS, true, c_eCiv5GameplayDLL, 0> veValidTargets;

	// Now loop through and come up with a list of valid players based on the proximity we found out earlier
	for(iTargetLoop = MAX_MAJOR_CIVS; iTargetLoop < MAX_CIV_PLAYERS; iTargetLoop++)
	{
		eTarget = (PlayerTypes) iTargetLoop;
		TeamTypes eConqueredTeam = GET_PLAYER(eTarget).getTeam();
		TeamTypes eConquerorTeam = GET_TEAM(eConqueredTeam).GetKilledByTeam();
	

		if(GET_PLAYER(eTarget).isAlive())
			continue;

		if(GetPlayer()->getTeam() == GET_PLAYER(eTarget).getTeam())
			continue;

		if(!GET_TEAM(GET_PLAYER(eForPlayer).getTeam()).isHasMet(GET_PLAYER(eTarget).getTeam()))
			continue;

		if(!GET_PLAYER(eForPlayer).CanLiberatePlayer(eTarget))
			continue;

		if(GET_PLAYER(eForPlayer).getTeam() == eConquerorTeam)
			continue;

		if(GetPlayer()->GetProximityToPlayer(eTarget) == eClosestProximity)
		{
			veValidTargets.push_back(eTarget);
		}
	}

	// Didn't find any valid Target players
	if(veValidTargets.size() == 0)
		return NO_PLAYER;

	int iRandIndex = GC.getGame().getJonRandNum(veValidTargets.size(), "Finding random City State Target for Minor to give out a quest to liberate.");
	eBestCityStateLiberate = veValidTargets[iRandIndex];

	return eBestCityStateLiberate;
}
#endif

/// Find a Great Person that a Minor would want a major to spawn
UnitTypes CvMinorCivAI::GetBestGreatPersonForQuest(PlayerTypes ePlayer)
{
	UnitTypes eBestUnit = NO_UNIT;

	SpecialUnitTypes eSpecialUnitGreatPerson = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_PEOPLE");

	FStaticVector<UnitTypes, 8, true, c_eCiv5GameplayDLL, 0> veValidUnits;

	// Loop through all Units and see if they're useful
	for(int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
	{
		const UnitTypes eUnit = static_cast<UnitTypes>(iUnitLoop);
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);

		if(pkUnitInfo == NULL)
			continue;

		// Can't be able to train it
		if(pkUnitInfo->GetProductionCost() != -1)
		{
			continue;
		}

		// Must be a Great Person (defined in SpecialUnitType in Unit XML)
		if(pkUnitInfo->GetSpecialUnitType() != eSpecialUnitGreatPerson)
		{
			continue;
		}

		// Must be a Great Person for this player's civ
		if(!GET_PLAYER(ePlayer).canTrain(eUnit, false /*bContinue*/, false /*bTestVisible*/, true /*bIgnoreCost*/))
		{
			continue;
		}

		// Must be a Great Person that can be spawned in the current game
		if(GC.getGame().isOption(GAMEOPTION_NO_RELIGION))
		{
			if(pkUnitInfo->IsFoundReligion())
			{
				continue;
			}
		}
		if(GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
		{
			if(pkUnitInfo->GetBaseBeakersTurnsToCount() > 0)
			{
				continue;
			}
		}

		bool bAlreadyHasUnit = false;

		// Player can't already have this Unit
		int iLoop;
		for(CvUnit* pLoopUnit = GET_PLAYER(ePlayer).firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = GET_PLAYER(ePlayer).nextUnit(&iLoop))
		{
			if(pLoopUnit->getUnitType() == eUnit)
			{
				bAlreadyHasUnit = true;
				break;
			}
		}
		if(bAlreadyHasUnit)
		{
			continue;
		}

		veValidUnits.push_back(eUnit);
	}

	// Didn't find any valid Units
	if(veValidUnits.size() == 0)
	{
		return NO_UNIT;
	}

	int iRandIndex = GC.getGame().getJonRandNum(veValidUnits.size(), "Finding random Great Person for Minor to give out a quest to spawn.");
	eBestUnit = veValidUnits[iRandIndex];

	return eBestUnit;
}

/// Anyone that this City State would want to kill?
/// NOTE: This makes a random choice, and is not guaranteed to return the same target if called multiple times.
/// It will, however, return NO_PLAYER reliably if there are no valid targets for eForPlayer.
PlayerTypes CvMinorCivAI::GetBestCityStateTarget(PlayerTypes eForPlayer)
{
	CvAssertMsg(eForPlayer >= 0, "eForPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eForPlayer < MAX_MAJOR_CIVS, "eForPlayer is expected to be within maximum bounds (invalid Index)");

	PlayerTypes eBestCityStateTarget = NO_PLAYER;

	PlayerProximityTypes eClosestProximity = PLAYER_PROXIMITY_DISTANT;

	// First, loop through the Minors in the game to what the closest proximity is to any of the players
	int iTargetLoop;
	PlayerTypes eTarget;
	for(iTargetLoop = MAX_MAJOR_CIVS; iTargetLoop < MAX_CIV_PLAYERS; iTargetLoop++)
	{
		eTarget = (PlayerTypes) iTargetLoop;

		if(!GET_PLAYER(eTarget).isAlive())
			continue;

		if(GetPlayer()->getTeam() == GET_PLAYER(eTarget).getTeam())
			continue;

		if(GetPlayer()->GetProximityToPlayer(eTarget) > eClosestProximity)
		{
			eClosestProximity = GetPlayer()->GetProximityToPlayer(eTarget);
		}
	}

	// Found nobody, or only people far away
	if(eClosestProximity == PLAYER_PROXIMITY_DISTANT)
	{
		return NO_PLAYER;
	}

	FStaticVector<PlayerTypes, MAX_CIV_PLAYERS, true, c_eCiv5GameplayDLL, 0> veValidTargets;

	// Now loop through and come up with a list of valid players based on the proximity we found out earlier
	for(iTargetLoop = MAX_MAJOR_CIVS; iTargetLoop < MAX_CIV_PLAYERS; iTargetLoop++)
	{
		eTarget = (PlayerTypes) iTargetLoop;

		if(!GET_PLAYER(eTarget).isAlive())
			continue;

		if(GetPlayer()->getTeam() == GET_PLAYER(eTarget).getTeam())
			continue;

		if(!GET_TEAM(GET_PLAYER(eForPlayer).getTeam()).isHasMet(GET_PLAYER(eTarget).getTeam()))
			continue;

		if(GetPlayer()->GetProximityToPlayer(eTarget) == eClosestProximity)
		{
			veValidTargets.push_back(eTarget);
		}
	}

	// Didn't find any valid Target players
	if(veValidTargets.size() == 0)
		return NO_PLAYER;

	int iRandIndex = GC.getGame().getJonRandNum(veValidTargets.size(), "Finding random City State Target for Minor to give out a quest to kill.");
	eBestCityStateTarget = veValidTargets[iRandIndex];

	return eBestCityStateTarget;
}

// Returns the PlayerTypes enum of the most recent valid bully, NO_PLAYER if there isn't one
PlayerTypes CvMinorCivAI::GetMostRecentBullyForQuest() const
{
	PlayerTypes eBully = NO_PLAYER;
	int iTurn = -1;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		// Bully must still be alive
		if(!GET_PLAYER((PlayerTypes)iPlayerLoop).isAlive())
			continue;

		if(m_aiTurnLastBullied[iPlayerLoop] > iTurn)
		{
			iTurn = m_aiTurnLastBullied[iPlayerLoop];
			eBully = (PlayerTypes)iPlayerLoop;
		}
	}

	return eBully;
}

/// Has this minor asked any other player to go after eMinor?
bool CvMinorCivAI::IsWantsMinorDead(PlayerTypes eMinor)
{
	PlayerTypes eMajor;
	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;

		// Major must be alive
		if(!GET_PLAYER(eMajor).isAlive())
			continue;

		// Has the quest?
		if(!IsActiveQuestForPlayer(eMajor, MINOR_CIV_QUEST_KILL_CITY_STATE))
			continue;

		// Right minor?
		if(GetQuestData1(eMajor, MINOR_CIV_QUEST_KILL_CITY_STATE) != eMinor)
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
	for(int iTargetMajorLoop = 0; iTargetMajorLoop < MAX_MAJOR_CIVS; iTargetMajorLoop++)
	{
		eTargetMajor = (PlayerTypes) iTargetMajorLoop;
		eTargetTeam = GET_PLAYER(eTargetMajor).getTeam();

		// Target needs to be alive
		if(!GET_PLAYER(eTargetMajor).isAlive())
		{
			continue;
		}

		// Don't have the player find anyone on his team...
		if(eTargetTeam == eTeam)
		{
			continue;
		}

		// Player must already know the target
		if(!pTeam->isHasMet(eTargetTeam))
		{
			continue;
		}

		// Player can't have seen this guy's territory before
		if(pTeam->IsHasFoundPlayersTerritory(eTargetMajor))
		{
			continue;
		}

		veValidTargets.push_back(eTargetMajor);
	}

	// Didn't find any valid Target players
	if(veValidTargets.size() == 0)
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
	if(pPlayer->getStartingPlot() == NULL)
	{
		return false;
	}

	// Player's already found them all
	if(pTeam->GetNumNaturalWondersDiscovered() == GC.getMap().GetNumNaturalWonders())
	{
		return false;
	}

	// Player hasn't yet found all the NWs in his area
	int iNumNaturalWondersInStartingArea = GC.getMap().getArea(pPlayer->getStartingPlot()->getArea())->GetNumNaturalWonders();
	if(pPlayer->GetNumNaturalWondersDiscoveredInArea() < iNumNaturalWondersInStartingArea)
	{
		return false;
	}

	return true;
}

#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
/// Any distant City States that this City State would want to meet?
/// NOTE: This makes a random choice, and is not guaranteed to return the same target if called multiple times.
/// It will, however, return NO_PLAYER reliably if there are no valid targets for eForPlayer.
PlayerTypes CvMinorCivAI::GetBestCityStateMeetTarget(PlayerTypes eForPlayer)
{
	CvAssertMsg(eForPlayer >= 0, "eForPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eForPlayer < MAX_MAJOR_CIVS, "eForPlayer is expected to be within maximum bounds (invalid Index)");

	PlayerTypes eBestCityStateTarget = NO_PLAYER;
	CvTeam* pTeam = &GET_TEAM(GetPlayer()->getTeam());

	PlayerProximityTypes eClosestProximity = PLAYER_PROXIMITY_CLOSE;

	// First, loop through the Minors in the game to what the closest proximity is to any of the players
	int iTargetLoop;
	PlayerTypes eTarget;
	for(iTargetLoop = MAX_MAJOR_CIVS; iTargetLoop < MAX_CIV_PLAYERS; iTargetLoop++)
	{
		eTarget = (PlayerTypes) iTargetLoop;

		if(!GET_PLAYER(eTarget).isAlive())
			continue;

		if(GetPlayer()->getTeam() == GET_PLAYER(eTarget).getTeam())
			continue;

		// Player can't have seen this guy's territory before
		if(pTeam->IsHasFoundPlayersTerritory(eTarget))
		{
			continue;
		}

		if(GetPlayer()->GetProximityToPlayer(eTarget) <= eClosestProximity)
		{
			eClosestProximity = GetPlayer()->GetProximityToPlayer(eTarget);
		}
	}

	// Found nobody, or only people too close
	if(eClosestProximity == PLAYER_PROXIMITY_NEIGHBORS)
	{
		return NO_PLAYER;
	}

	FStaticVector<PlayerTypes, MAX_CIV_PLAYERS, true, c_eCiv5GameplayDLL, 0> veValidTargets;

	// Now loop through and come up with a list of valid players based on the proximity we found out earlier
	for(iTargetLoop = MAX_MAJOR_CIVS; iTargetLoop < MAX_CIV_PLAYERS; iTargetLoop++)
	{
		eTarget = (PlayerTypes) iTargetLoop;

		if(!GET_PLAYER(eTarget).isAlive())
			continue;

		if(GetPlayer()->getTeam() == GET_PLAYER(eTarget).getTeam())
			continue;

		if(GET_TEAM(GET_PLAYER(eForPlayer).getTeam()).isHasMet(GET_PLAYER(eTarget).getTeam()))
			continue;

		if(GetPlayer()->GetProximityToPlayer(eTarget) == eClosestProximity)
		{
			veValidTargets.push_back(eTarget);
		}
	}

	// Didn't find any valid Target players
	if(veValidTargets.size() == 0)
		return NO_PLAYER;

	int iRandIndex = GC.getGame().getJonRandNum(veValidTargets.size(), "Finding random City State Target for Minor to give out a quest to kill.");
	eBestCityStateTarget = veValidTargets[iRandIndex];

	return eBestCityStateTarget;
}
#endif

/// The minor civ has been bullied recently and could use some help?
bool CvMinorCivAI::IsGoodTimeForGiveGoldQuest()
{
	if(IsRecentlyBulliedByAnyMajor())
	{
		return true;
	}

	return false;
}

/// The minor civ has been bullied recently and could use some help?
bool CvMinorCivAI::IsGoodTimeForPledgeToProtectQuest()
{
	if(IsRecentlyBulliedByAnyMajor())
	{
		return true;
	}

	return false;
}

/// The minor civ has been bullied recently and could use some help?
bool CvMinorCivAI::IsGoodTimeForDenounceMajorQuest()
{
	if(IsRecentlyBulliedByAnyMajor())
	{
		return true;
	}

	return false;
}

#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
/// The minor civ has been bullied recently and could use some help?
bool CvMinorCivAI::IsGoodTimeForWarMajorQuest()
{
	if(IsRecentlyBulliedByAnyMajor())
	{
		return true;
	}

	return false;
}
#endif


// ******************************
// ***** Friendship *****
// ******************************



/// Per-turn friendship stuff
void CvMinorCivAI::DoFriendship()
{
	Localization::String strMessage;
	Localization::String strSummary;
	const char* strMinorsNameKey = GetPlayer()->getNameKey();

	PlayerTypes ePlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(GET_PLAYER(ePlayer).isAlive())
		{
			// Update friendship even if the player hasn't met us yet, since we may have heard things through the grapevine (Wary Of, SP, etc.)

			// Look at the base friendship (not counting war status etc.) and change it
			int iOldFriendship = GetBaseFriendshipWithMajor(ePlayer);
			int iChangeThisTurn = GetFriendshipChangePerTurnTimes100(ePlayer);
			int iFriendshipAnchor = GetFriendshipAnchorWithMajor(ePlayer);
			int iNewFriendship = iOldFriendship + (iChangeThisTurn / 100);
			if(iOldFriendship >= iFriendshipAnchor && iNewFriendship < iFriendshipAnchor)
			{
				// If we are at or above anchor, don't let the decay dip us below it
				SetFriendshipWithMajor(ePlayer, iFriendshipAnchor);
			}
			else if (iChangeThisTurn != 0)
			{
				ChangeFriendshipWithMajorTimes100(ePlayer, iChangeThisTurn);
			}
			else
			{
				// Friendship amount doesn't change, but ally state could have (ex. current ally decays below our level)
				DoFriendshipChangeEffects(ePlayer, iOldFriendship, iNewFriendship);
			}

			// Notification for status changes
			if(GetPlayer()->isAlive() && IsHasMetPlayer(ePlayer))
			{
				const int iTurnsWarning = 2;
				const int iAlliesThreshold = GetAlliesThreshold() * 100;
				const int iFriendsThreshold = GetFriendsThreshold() * 100;
				int iEffectiveFriendship = GetEffectiveFriendshipWithMajorTimes100(ePlayer);
				if(IsAllies(ePlayer))
				{
					if(iEffectiveFriendship + (iTurnsWarning * iChangeThisTurn) < iAlliesThreshold &&
						iEffectiveFriendship + ((iTurnsWarning-1) * iChangeThisTurn) >= iAlliesThreshold)
					{
						strMessage = Localization::Lookup("TXT_KEY_NTFN_CITY_STATE_ALMOST_NOT_ALLIES");
						strMessage << strMinorsNameKey;
						strSummary = Localization::Lookup("TXT_KEY_NTFN_CITY_STATE_ALMOST_SM");
						strSummary << strMinorsNameKey;

						AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), ePlayer);
					}
#if !defined(NO_ACHIEVEMENTS)
					if(!GC.getGame().isGameMultiPlayer() && GET_PLAYER(ePlayer).isHuman())
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_CITYSTATE_ALLY);
					}
#endif

				}
				else if(IsFriends(ePlayer))
				{
					if(iEffectiveFriendship + (iTurnsWarning * iChangeThisTurn) < iFriendsThreshold &&
						iEffectiveFriendship + ((iTurnsWarning-1) * iChangeThisTurn) >= iFriendsThreshold)
					{
						strMessage = Localization::Lookup("TXT_KEY_NTFN_CITY_STATE_ALMOST_NOT_FRIENDS");
						strMessage << strMinorsNameKey;
						strSummary = Localization::Lookup("TXT_KEY_NTFN_CITY_STATE_ALMOST_SM");
						strSummary << strMinorsNameKey;

						AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), ePlayer);
					}
				}
			}
		}
	}
}

/// How much does friendship drop per turn with ePlayer?
/// Note that this does not pay attention to whether ePlayer is at war
/// with this minor, in which case there is no friendship change per turn.
int CvMinorCivAI::GetFriendshipChangePerTurnTimes100(PlayerTypes ePlayer)
{
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	int iChangeThisTurn = 0;

	// Modifier to rate based on traits and religion
	int iTraitMod = kPlayer.GetPlayerTraits()->GetCityStateFriendshipModifier();
	int iReligionMod = 0;
	if (IsSameReligionAsMajor(ePlayer))
		iReligionMod += /*50*/ GC.getMINOR_FRIENDSHIP_RATE_MOD_SHARED_RELIGION();

	// Relation to anchor point?
	int iBaseFriendship = GetBaseFriendshipWithMajor(ePlayer);
	int iFriendshipAnchor = GetFriendshipAnchorWithMajor(ePlayer);
	if (iBaseFriendship == iFriendshipAnchor)
	{
		// Change rate is 0
	}
	else if (iBaseFriendship > iFriendshipAnchor)
	{
		// Hostile Minors have Friendship decay quicker
		if(GetPersonality() == MINOR_CIV_PERSONALITY_HOSTILE)
			iChangeThisTurn += /*-150*/ GC.getMINOR_FRIENDSHIP_DROP_PER_TURN_HOSTILE();
		// Aggressor!
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
		//Decay if capital is taking damage during war (CSs are fickle allies if they're on the recieving end of war).
		if(MOD_DIPLOMACY_CITYSTATES_QUESTS && IsAllies(ePlayer))
		{
			if(GetPlayer()->getCapitalCity() != NULL)
			{
				if(GetPlayer()->getCapitalCity()->getDamage() > 0)
				{
					iChangeThisTurn += /*-600*/ (GC.getMINOR_FRIENDSHIP_DROP_PER_TURN_AGGRESSOR() * 3);
				}
			}
		}
#endif
		else if(GET_TEAM(kPlayer.getTeam()).IsMinorCivAggressor())
			iChangeThisTurn += /*-200*/ GC.getMINOR_FRIENDSHIP_DROP_PER_TURN_AGGRESSOR();
		// Normal decay
		else
			iChangeThisTurn += /*-100*/ GC.getMINOR_FRIENDSHIP_DROP_PER_TURN();

		// Decay modified (Trait, policies, shared religion, etc.)
		int iDecayMod = 100;
		iDecayMod += GET_PLAYER(ePlayer).GetMinorFriendshipDecayMod();
		iDecayMod += (-1) * (iTraitMod / 2);
		iDecayMod += (-1) * (iReligionMod / 2);
		
		if (iDecayMod < 0)
			iDecayMod = 0;

		iChangeThisTurn *= iDecayMod;
		iChangeThisTurn /= 100;
	}
	else
	{
		iChangeThisTurn += /*100*/ GC.getMINOR_FRIENDSHIP_NEGATIVE_INCREASE_PER_TURN();

		// Recovery modified (Trait, policies, shared religion, etc.)
		int iRecoveryMod = 100;
		iRecoveryMod += iTraitMod;
		iRecoveryMod += iReligionMod;
		
		if (iRecoveryMod < 0)
			iRecoveryMod = 0;

		iChangeThisTurn *= iRecoveryMod;
		iChangeThisTurn /= 100;
	}

	// Shift on top of base rate
	if (GET_TEAM(kPlayer.getTeam()).isHasMet(GetPlayer()->getTeam()))
	{
		int iShift = 0;

		if (kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_PROTECTED_MINOR_INFLUENCE) != 0)
		{
			if (GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(ePlayer, GetPlayer()->GetID()))
			{
				iShift += kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_PROTECTED_MINOR_INFLUENCE);
			}
		}

		if (CanMajorBullyGold(ePlayer))
		{
			iShift += kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_AFRAID_INFLUENCE);
			iShift += kPlayer.GetPlayerTraits()->GetAfraidMinorPerTurnInfluence();
		}
		
		if (iShift != 0)
		{
			iChangeThisTurn += iShift;
		}
	}

	// Mod everything by game speed
	iChangeThisTurn *= GC.getGame().getGameSpeedInfo().getGoldGiftMod();
	iChangeThisTurn /= 100;

#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	//No CS drop for allies - Cold War fun!
	if(MOD_DIPLOMACY_CITYSTATES_QUESTS && IsAllies(ePlayer))
	{
		PlayerTypes eLoopPlayer;
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;
			if(eLoopPlayer != NO_PLAYER)
			{
				if(GC.getGame().GetGameLeagues()->IsIdeologyEmbargoed(ePlayer, eLoopPlayer))
				{
					iChangeThisTurn = 0;
				}
			}
		}
	}
#endif

	return iChangeThisTurn;
}

// What is the level of Friendship between this Minor and the requested Major Civ?
// Takes things like war status into account
// NOTE: Not const because of need to check war status
int CvMinorCivAI::GetEffectiveFriendshipWithMajorTimes100(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0; // as defined during Reset()

	// Are we at war?
	if(IsAtWarWithPlayersTeam(ePlayer))
		return (100 * /*-60*/GC.getMINOR_FRIENDSHIP_AT_WAR());

	return GetBaseFriendshipWithMajorTimes100(ePlayer);
}

// What is the raw, stored level of Friendship between this Minor and the requested Major Civ?
int CvMinorCivAI::GetBaseFriendshipWithMajorTimes100(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0; // as defined during Reset()

	return m_aiFriendshipWithMajorTimes100[ePlayer];
}

/// Sets the base level of Friendship between this Minor and the specified Major Civ
void CvMinorCivAI::SetFriendshipWithMajorTimes100(PlayerTypes ePlayer, int iNum, bool bFromQuest)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	int iOldEffectiveFriendship = GetEffectiveFriendshipWithMajorTimes100(ePlayer);

	m_aiFriendshipWithMajorTimes100[ePlayer] = iNum;

	int iMinimumFriendship = GC.getMINOR_FRIENDSHIP_AT_WAR();
	if(GetBaseFriendshipWithMajor(ePlayer) < iMinimumFriendship)
		m_aiFriendshipWithMajorTimes100[ePlayer] = iMinimumFriendship * 100;

	int iNewEffectiveFriendship = GetEffectiveFriendshipWithMajorTimes100(ePlayer);

	// Has the friendship in effect changed?
	if(iOldEffectiveFriendship != iNewEffectiveFriendship)
	{
		DoFriendshipChangeEffects(ePlayer, iOldEffectiveFriendship/100, iNewEffectiveFriendship/100, bFromQuest);
	}

	// Update City banners and game info if this is the active player
	if(ePlayer == GC.getGame().getActivePlayer())
	{
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
	}
}

/// Changes the base level of Friendship between this Minor and the specified Major Civ
void CvMinorCivAI::ChangeFriendshipWithMajorTimes100(PlayerTypes ePlayer, int iChange, bool bFromQuest)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		// If this friendship was earned from a Quest, then we might apply a modifier to it
		if(bFromQuest && iChange > 0)
		{
			if(GET_PLAYER(ePlayer).getMinorQuestFriendshipMod() != 0)
			{
				iChange *= (100 + GET_PLAYER(ePlayer).getMinorQuestFriendshipMod());
				iChange /= 100;
			}
		}

		SetFriendshipWithMajorTimes100(ePlayer, GetBaseFriendshipWithMajorTimes100(ePlayer) + iChange, bFromQuest);
	}
}

// What is the level of Friendship between this Minor and the requested Major Civ?
// Takes war status into account
// NOTE: Not const because of need to check war status
int CvMinorCivAI::GetEffectiveFriendshipWithMajor(PlayerTypes ePlayer)
{
	return GetEffectiveFriendshipWithMajorTimes100(ePlayer) / 100;
}

// What is the raw, stored level of Friendship between this Minor and the requested Major Civ?
int CvMinorCivAI::GetBaseFriendshipWithMajor(PlayerTypes ePlayer) const
{
	return GetBaseFriendshipWithMajorTimes100(ePlayer) / 100;
}

/// Sets the base level of Friendship between this Minor and the specified Major Civ
void CvMinorCivAI::SetFriendshipWithMajor(PlayerTypes ePlayer, int iNum, bool bFromQuest)
{
	SetFriendshipWithMajorTimes100(ePlayer, iNum * 100, bFromQuest);
}

/// Changes the base level of Friendship between this Minor and the specified Major Civ
void CvMinorCivAI::ChangeFriendshipWithMajor(PlayerTypes ePlayer, int iChange, bool bFromQuest)
{
	ChangeFriendshipWithMajorTimes100(ePlayer, iChange * 100, bFromQuest);
}

/// What is the resting point of Influence this major has?  Affected by religion, social policies, Wary Of, etc.
int CvMinorCivAI::GetFriendshipAnchorWithMajor(PlayerTypes eMajor)
{
	int iAnchor = GC.getMINOR_FRIENDSHIP_ANCHOR_DEFAULT();

	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if (eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return iAnchor;

	CvPlayer* pMajor = &GET_PLAYER(eMajor);
	CvAssertMsg(pMajor, "MINOR CIV AI: pMajor not expected to be NULL.  Please send Anton your save file and version.");
	if (!pMajor) return iAnchor;

	// Pledge to Protect
	if (IsProtectedByMajor(eMajor))
	{
		iAnchor += GC.getMINOR_FRIENDSHIP_ANCHOR_MOD_PROTECTED();
	}

	// Wary Of?
	if (IsWaryOfTeam(pMajor->getTeam()))
	{
		iAnchor += GC.getMINOR_FRIENDSHIP_ANCHOR_MOD_WARY_OF();
	}
#if defined(MOD_BALANCE_CORE_MINORS)
	if (MOD_BALANCE_CORE_MINORS) 
	{
		//Anchor reduced by 10 based on num minor civs attacked. Only counts for CSs not allied to or friends with.
		int iAttacks = GET_TEAM(pMajor->getTeam()).GetNumMinorCivsAttacked();
		if (iAttacks > 0)
		{
			if(!IsFriends(pMajor->GetID()) || !IsAllies(pMajor->GetID()))
			{
				iAnchor += (GC.getBALANCE_MINOR_ANCHOR_ATTACK() /*-10*/ * iAttacks);
			}
		}
	}
#endif

	// Social Policies
	iAnchor += pMajor->GetMinorFriendshipAnchorMod();

	// Religion
	CvPlayerReligions* pMajorReligions = pMajor->GetReligions();
	CvAssertMsg(pMajorReligions, "MINOR CIV AI: pMajorReligions not expected to be NULL.  Please send Anton your save file and version.");
	if (!pMajorReligions) return iAnchor;
	CvCity* pMinorCapital = GetPlayer()->getCapitalCity();
	if (!pMinorCapital) return iAnchor; // Happens when city was just captured, after buyout, etc., so just return the anchor value we have
	CvCityReligions* pMinorCapitalReligions = pMinorCapital->GetCityReligions();
	CvAssertMsg(pMinorCapitalReligions, "MINOR CIV AI: pMinorCapitalReligions not expected to be NULL.  Please send Anton your save file and version.");
	if (!pMinorCapitalReligions) return iAnchor;
	iAnchor += pMajorReligions->GetCityStateMinimumInfluence(pMinorCapitalReligions->GetReligiousMajority());

	return iAnchor;
}

/// Resets the base level of Friendship to zero
void CvMinorCivAI::ResetFriendshipWithMajor(PlayerTypes ePlayer)
{
	// If ePlayer isn't a major civ then there is no influence value to reset, so just return
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return; // as defined during Reset()

	int iOldFriendship = GetEffectiveFriendshipWithMajor(ePlayer);
	const int iResetFriendship = 0;
	if(GetPlayer()->isAlive())
	{
		SetFriendshipWithMajor(ePlayer, iResetFriendship);
	}
	else
	{
		// special workaround to allow status changes despite minor already being dead
		DoFriendshipChangeEffects(ePlayer, iOldFriendship, iResetFriendship, /*bFromQuest*/false, /*bIgnoreMinorDeath*/true);
		SetFriendshipWithMajor(ePlayer, iResetFriendship);
	}

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
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		eResource = (ResourceTypes) iResourceLoop;

		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		if (pkResourceInfo == NULL)
			continue;

		eUsage = pkResourceInfo->getResourceUsage();

		if(eUsage == RESOURCEUSAGE_STRATEGIC || eUsage == RESOURCEUSAGE_LUXURY)
		{
			// Someone is losing the bonus :(
			if(eOldAlly != NO_PLAYER)
			{
				iResourceQuantity = GetPlayer()->getResourceExport(eResource);

				if(iResourceQuantity > 0)
				{
					GET_PLAYER(eOldAlly).changeResourceFromMinors(eResource, -iResourceQuantity);
					GetPlayer()->changeResourceExport(eResource, -iResourceQuantity);
				}
			}

			// Someone new is getting the bonus :D
			if(eNewAlly != NO_PLAYER)
			{
				iResourceQuantity = GetPlayer()->getNumResourceTotal(eResource);

				if(iResourceQuantity > 0)
				{
					GET_PLAYER(eNewAlly).changeResourceFromMinors(eResource, iResourceQuantity);
					GetPlayer()->changeResourceExport(eResource, iResourceQuantity);
				}
			}
		}
	}
}

/// The most Friendship (effective, not base) any player has with this Minor
int CvMinorCivAI::GetMostFriendshipWithAnyMajor(PlayerTypes& eBestPlayer)
{
	int iMostFriendship = 0;
	PlayerTypes eMajor;

	int iFriendship;

	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;

		if(IsHasMetPlayer(eMajor))
		{
			iFriendship = GetEffectiveFriendshipWithMajor(eMajor);

			if(iFriendship > iMostFriendship)
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

	if(eOldAlly != NO_PLAYER)
	{
		for(int iI = 0; iI < iNumPlots; iI++)
		{
			CvPlot* pPlot = theMap.plotByIndexUnchecked(iI);
			if(pPlot->getOwner() == m_pPlayer->GetID())
			{
				pPlot->changeAdjacentSight(GET_PLAYER(eOldAlly).getTeam(), iPlotVisRange, false, NO_INVISIBLE, NO_DIRECTION, false);
			}
		}
		if(eOldAlly == GC.getGame().getActivePlayer())
		{
			theMap.updateDeferredFog();
		}
	}

	m_eAlly = eNewAlly;
	m_iTurnAllied = GC.getGame().getGameTurn();

	// Seed the GP counter?
	if(eNewAlly != NO_PLAYER)
	{
		CvPlayerAI& kNewAlly = GET_PLAYER(eNewAlly);

		// share the visibility with my ally (and his team-mates)
		for(int iI = 0; iI < iNumPlots; iI++)
		{
			CvPlot* pPlot = theMap.plotByIndexUnchecked(iI);
			if(pPlot->getOwner() == m_pPlayer->GetID())
			{
				pPlot->changeAdjacentSight(kNewAlly.getTeam(), iPlotVisRange, true, NO_INVISIBLE, NO_DIRECTION, false);
			}
		}

		for(int iPolicyLoop = 0; iPolicyLoop < GC.getNumPolicyInfos(); iPolicyLoop++)
		{
			const PolicyTypes eLoopPolicy = static_cast<PolicyTypes>(iPolicyLoop);
			CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(eLoopPolicy);
			if(pkPolicyInfo)
			{
				if(kNewAlly.GetPlayerPolicies()->HasPolicy(eLoopPolicy) && !kNewAlly.GetPlayerPolicies()->IsPolicyBlocked(eLoopPolicy))
				{
					// This is the policy we want!
					if(pkPolicyInfo->IsMinorGreatPeopleAllies())
					{
						if(kNewAlly.GetGreatPeopleSpawnCounter() <= 0)
							kNewAlly.DoSeedGreatPeopleSpawnCounter();
						else
							kNewAlly.DoApplyNewAllyGPBonus();
					}
				}
			}
		}

		//Achievement Test
		kNewAlly.GetPlayerAchievements().AlliedWithCityState(GetPlayer()->GetID());;
	}

	// Alter who gets this guy's resources
	DoUpdateAlliesResourceBonus(eNewAlly, eOldAlly);

	// Declare war on Ally's enemies
	if(eNewAlly != NO_PLAYER)
	{
		CvPlayerAI& kNewAlly = GET_PLAYER(eNewAlly);
		CvTeam& kNewAllyTeam = GET_TEAM(kNewAlly.getTeam());
		CvTeam& kOurTeam = GET_TEAM(GetPlayer()->getTeam());

		TeamTypes eLoopTeam;
		for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
		{
			eLoopTeam = (TeamTypes) iTeamLoop;

			if(!GET_TEAM(eLoopTeam).isAlive())
				continue;

			if(kNewAllyTeam.isAtWar(eLoopTeam))
#if defined(MOD_EVENTS_WAR_AND_PEACE)
				kOurTeam.declareWar(eLoopTeam, false, GetPlayer()->GetID());
#else
				kOurTeam.declareWar(eLoopTeam);
#endif
		}
	}

	DoTestEndWarsVSMinors(eOldAlly, eNewAlly);
#if defined(MOD_GLOBAL_CS_NO_ALLIED_SKIRMISHES)
	if (MOD_GLOBAL_CS_NO_ALLIED_SKIRMISHES) {
		DoTestEndSkirmishes(eNewAlly);
	}
#endif

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

/// How many turns has the alliance been active?
int CvMinorCivAI::GetAlliedTurns() const
{
	int iRtnValue = -1;

	if (m_eAlly != NO_PLAYER)
	{
		iRtnValue = GC.getGame().getGameTurn() - m_iTurnAllied;
	}

	return iRtnValue;
}

/// Is ePlayer Allies with this minor?
bool CvMinorCivAI::IsAllies(PlayerTypes ePlayer) const
{
	return m_eAlly == ePlayer;
}

/// Is ePlayer Friends with this minor?
bool CvMinorCivAI::IsFriends(PlayerTypes ePlayer)
{
	return IsFriendshipAboveFriendsThreshold(GetEffectiveFriendshipWithMajor(ePlayer));
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
	int iBuffer = GetEffectiveFriendshipWithMajor(ePlayer) - GetAlliesThreshold();

	if(iBuffer >= 0 && iBuffer < /*8*/ GC.getMINOR_FRIENDSHIP_CLOSE_AMOUNT())
		return true;

	return false;
}

/// Are we about to lose our status? (used in Diplo AI)
bool CvMinorCivAI::IsCloseToNotBeingFriends(PlayerTypes ePlayer)
{
	int iBuffer = GetEffectiveFriendshipWithMajor(ePlayer) - GetFriendsThreshold();

	if(iBuffer >= 0 && iBuffer < /*8*/ GC.getMINOR_FRIENDSHIP_CLOSE_AMOUNT())
		return true;

	return false;
}

/// What level of Friendship does a player have with this Minor?
int CvMinorCivAI::GetFriendshipLevelWithMajor(PlayerTypes ePlayer)
{
	if(IsAllies(ePlayer))
	{
		return 2;
	}
	else if(IsFriends(ePlayer))
	{
		return 1;
	}

	return 0;
}


/// Friendship needed for next level of bonuses
int CvMinorCivAI::GetFriendshipNeededForNextLevel(PlayerTypes ePlayer)
{
	if(!IsFriends(ePlayer))
	{
		return GetFriendsThreshold();
	}
	else if(!IsAllies(ePlayer))
	{
		return GetAlliesThreshold();
	}

	return 0;
}

/// What happens when Friendship changes?
void CvMinorCivAI::DoFriendshipChangeEffects(PlayerTypes ePlayer, int iOldFriendship, int iNewFriendship, bool bFromQuest, bool bIgnoreMinorDeath)
{
	// Can't give out bonuses if we're dead!
	if(!bIgnoreMinorDeath && !GetPlayer()->isAlive())
		return;

	Localization::String strMessage;
	Localization::String strSummary;

	bool bAdd = false;
	bool bFriends = false;
	bool bAllies = false;

	bool bWasAboveFriendsThreshold = IsFriendshipAboveFriendsThreshold(iOldFriendship);
	bool bNowAboveFriendsThreshold = IsFriendshipAboveFriendsThreshold(iNewFriendship);

	// If we are Friends now, mark that we've been Friends at least once this game
	if(bNowAboveFriendsThreshold)
#if defined(MOD_BALANCE_CORE_MINORS)
		if (MOD_BALANCE_CORE_MINORS) 
		{
			//Small Possibility of reducing # of minor civs attacked via friendship the first time we become friends with them.
			if(!IsEverFriends(ePlayer) && (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetNumMinorCivsAttacked() > 0))
			{
				int iForgive = GC.getGame().getJonRandNum(100, "Rand turns for Minor forgiveness");
				if(iForgive > GC.getBALANCE_CS_FORGIVENESS_CHANCE())
				{
					GET_TEAM(GET_PLAYER(ePlayer).getTeam()).ChangeNumMinorCivsAttacked(-1);
				}
			}
		}
#endif

		SetEverFriends(ePlayer, true);
	// Add Friends Bonus
	if(!bWasAboveFriendsThreshold && bNowAboveFriendsThreshold)
	{
		bAdd = true;
		bFriends = true;

#if defined(MOD_EVENTS_MINORS)
		if (MOD_EVENTS_MINORS) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_MinorFriendsChanged, m_pPlayer->GetID(), ePlayer, true, iOldFriendship, iNewFriendship);
		}
#endif
	}
	// Remove Friends bonus
	else if(bWasAboveFriendsThreshold && !bNowAboveFriendsThreshold)
	{
		bAdd = false;
		bFriends = true;

#if defined(MOD_EVENTS_MINORS)
		if (MOD_EVENTS_MINORS) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_MinorFriendsChanged, m_pPlayer->GetID(), ePlayer, false, iOldFriendship, iNewFriendship);
		}
#endif
	}

	// Resolve Allies status
	bool bWasAboveAlliesThreshold = IsFriendshipAboveAlliesThreshold(iOldFriendship);
	bool bNowAboveAlliesThreshold = IsFriendshipAboveAlliesThreshold(iNewFriendship);

	PlayerTypes eOldAlly = GetAlly();

	// No old ally and our friendship is now above the threshold, OR our friendship is now higher than a previous ally
	if((eOldAlly == NO_PLAYER && bNowAboveAlliesThreshold)
	        || (eOldAlly != NO_PLAYER && GetEffectiveFriendshipWithMajor(ePlayer) > GetEffectiveFriendshipWithMajor(eOldAlly)))
	{
		bAdd = true;
		bAllies = true;

#if defined(MOD_EVENTS_MINORS)
		if (MOD_EVENTS_MINORS) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_MinorAlliesChanged, m_pPlayer->GetID(), ePlayer, true, iOldFriendship, iNewFriendship);
		}
#endif
	}
	// Remove Allies bonus
	else if(eOldAlly == ePlayer && bWasAboveAlliesThreshold && !bNowAboveAlliesThreshold)
	{
		bAdd = false;
		bAllies = true;

#if defined(MOD_EVENTS_MINORS)
		if (MOD_EVENTS_MINORS) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_MinorAlliesChanged, m_pPlayer->GetID(), ePlayer, false, iOldFriendship, iNewFriendship);
		}
#endif
	}

	// Make changes to bonuses here. Only send notifications if this change is not related to quests (otherwise it is rolled into quest notification)
	if(bFriends || bAllies)
		DoSetBonus(ePlayer, bAdd, bFriends, bAllies, /*bSuppressNotifications*/ bFromQuest);

	// Now actually changed Allied status, since we needed the old player in effect to create the notifications in the function above us
	if(bAllies)
	{
		if(bAdd)
			SetAlly(ePlayer);
		else
			SetAlly(NO_PLAYER);	// We KNOW no one else can be higher, so set the Ally to NO_PLAYER
	}
}


/// Is the player above the "Friends" threshold?
bool CvMinorCivAI::IsFriendshipAboveFriendsThreshold(int iFriendship) const
{
	int iFriendshipThresholdFriends = GetFriendsThreshold();

	if(iFriendship >= iFriendshipThresholdFriends)
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

	if(iFriendship >= iFriendshipThresholdAllies)
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
void CvMinorCivAI::DoSetBonus(PlayerTypes ePlayer, bool bAdd, bool bFriends, bool bAllies, bool bSuppressNotifications, bool bPassedBySomeone, PlayerTypes eNewAlly)
{
	MinorCivTraitTypes eTrait = GetTrait();

	// Cultured
	if(eTrait == MINOR_CIV_TRAIT_CULTURED)
	{
	}
	// Militaristic
	else if(eTrait == MINOR_CIV_TRAIT_MILITARISTIC)
	{
		if(bAdd)
		{
			// Seed Counter if it hasn't been done yet in this game. We don't have to undo this at any point because the counter is not processed if we are no longer Friends
			if(GetUnitSpawnCounter(ePlayer) == -1)
				DoSeedUnitSpawnCounter(ePlayer, /*bBias*/ true);
		}
	}
	// Maritime
	else if(eTrait == MINOR_CIV_TRAIT_MARITIME)
	{
		int iCapitalFoodTimes100 = 0;
		int iOtherCitiesFoodTimes100 = 0;

		if(bFriends)	// Friends bonus
		{
			iCapitalFoodTimes100 += GetFriendsCapitalFoodBonus(ePlayer);
			iOtherCitiesFoodTimes100 += GetFriendsOtherCityFoodBonus(ePlayer);
		}
		if(bAllies)		// Allies bonus
		{
			iCapitalFoodTimes100 += GetAlliesCapitalFoodBonus(ePlayer);
			iOtherCitiesFoodTimes100 += GetAlliesOtherCityFoodBonus(ePlayer);
		}

		if(!bAdd)		// Flip amount of we're taking bonuses away
		{
			iCapitalFoodTimes100 = -iCapitalFoodTimes100;
			iOtherCitiesFoodTimes100 = -iOtherCitiesFoodTimes100;
		}

		GET_PLAYER(ePlayer).ChangeCapitalYieldChange(YIELD_FOOD, iCapitalFoodTimes100);
		GET_PLAYER(ePlayer).ChangeCityYieldChange(YIELD_FOOD, iOtherCitiesFoodTimes100);
	}
	// Mercantile
	else if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
	{
		GET_PLAYER(ePlayer).DoUpdateHappiness();
	}
	// Religious
	if(eTrait == MINOR_CIV_TRAIT_RELIGIOUS)
	{
	}

	if(ePlayer == GC.getGame().getActivePlayer())
	{
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	}

	CvString strDetailedInfo = GetStatusChangeDetails(ePlayer, bAdd, bFriends, bAllies);

	PlayerTypes eOldAlly = GetAlly();
	TeamTypes eOldAllyTeam = eOldAlly != NO_PLAYER ? GET_PLAYER(eOldAlly).getTeam() : NO_TEAM;

	// Should we remove the Ally bonus from another player?
	if(bAdd && bAllies)
	{
		if(eOldAlly != NO_PLAYER && ePlayer != eOldAlly)
		{
			DoSetBonus(eOldAlly, /*bAdd*/ false, /*bFriends*/ false, /*bAllies*/ true, /*bSuppressNotifications*/ false, /*bPassedBySomeone*/ true, ePlayer);
		}
	}

	// *******************************************
	// NOTIFICATIONS FOR THIS PLAYER
	// *******************************************
	// We're not displaying notifications at all
	if (IsDisableNotifications())
	{
		return;
	}

	if (!bSuppressNotifications)
	{
		pair<CvString, CvString> notifStrings = GetStatusChangeNotificationStrings(ePlayer, bAdd, bFriends, bAllies, eOldAlly, (bAdd && bAllies) ? ePlayer : eNewAlly);
		AddNotification(notifStrings.first, notifStrings.second, ePlayer);
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
	for(iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;

		if(GetAngerFreeIntrusionCounter(eMajor) > 0)
		{
			ChangeAngerFreeIntrusionCounter(eMajor, -1);
		}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
		if(MOD_DIPLOMACY_CITYSTATES_QUESTS && (GetPlayer()->GetMinorCivAI()->IsActiveQuestForPlayer(eMajor, MINOR_CIV_QUEST_HORDE) || GetPlayer()->GetMinorCivAI()->IsActiveQuestForPlayer(eMajor, MINOR_CIV_QUEST_REBELLION)))
		{
			return;
		}
#endif
	}

	// If there are barbs nearby then don't worry about other players
	if(GetNumThreateningBarbarians() > 0)
		return;

	CvPlot* pLoopPlot;
	const IDInfo* pUnitNode;
	const CvUnit* pLoopUnit;

	// Set up scratch pad so that we can use it to send out Notifications
	for(iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;
		SetMajorScratchPad(eMajor, 0);
	}

	// Look at how many Units each Major Civ has in the Minor's Territory
	for(int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

		// Plot owned by this Minor?
		if(pLoopPlot->getOwner() == GetPlayer()->GetID())
		{
			pUnitNode = pLoopPlot->headUnitNode();

			while(pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(*pUnitNode);
				pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

				// Does this unit not cause anger?
				if(pLoopUnit && pLoopUnit->IsAngerFreeUnit())
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
				if(pLoopUnit && pLoopUnit->getOwner() < MAX_MAJOR_CIVS)
				{
					// If player has been granted Open Borders or has a friendship with minors bonus, then the Minor doesn't care about intrusion
					if(!IsPlayerHasOpenBorders(pLoopUnit->getOwner()) && GetAngerFreeIntrusionCounter(pLoopUnit->getOwner()) == 0)
					{
						// If the player is at war with the Minor then don't bother
						if(!IsAtWarWithPlayersTeam(pLoopUnit->getOwner()))
						{
							// Ignore if the player trait allows us to intrude without angering
							if(!GET_PLAYER(pLoopUnit->getOwner()).GetPlayerTraits()->IsAngerFreeIntrusionOfCityStates())
							{
								ChangeFriendshipWithMajor(pLoopUnit->getOwner(), /*-6*/ GC.getFRIENDSHIP_PER_UNIT_INTRUDING());

								// only modify if the unit isn't automated nor having a pending order
								if(!pLoopUnit->IsAutomated() && pLoopUnit->GetLengthMissionQueue() == 0)
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
	}

	// Now send out Notifications (if necessary)
	Localization::String strMessage;
	Localization::String strSummary;

	for(iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;

		if(GetMajorScratchPad(eMajor) > 0)
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_INTRUSION");
			strMessage << GetPlayer()->getNameKey();
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_INTRUSION");
			strSummary << GetPlayer()->getNameKey();

			AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), eMajor);
		}
	}
}

#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
//Sack a city-state
void CvMinorCivAI::DoSack()
{
	CvPlot* pPlot = GetPlayer()->getCapitalCity()->plot();
	CvGame& theGame = GC.getGame();
	PlayerTypes eMinor = GetPlayer()->GetID();

	UnitTypes eUnit = theGame.GetRandomSpawnUnitType(eMinor, /*bIncludeUUs*/ false, /*bIncludeRanged*/ true);
	// Init unit
	GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pPlot->getX(), pPlot->getY());

	SetSacked(false);
}
//Do Defection
void CvMinorCivAI::DoDefection()
{
	//Quest Over - Player loses all influence, rival ideology followers gain alliance.
	PlayerTypes eOldAlly = GetPlayer()->GetMinorCivAI()->GetAlly();
	PolicyBranchTypes ePreferredIdeology = GET_PLAYER(eOldAlly).GetCulture()->GetPublicOpinionPreferredIdeology();
	GetPlayer()->GetMinorCivAI()->SetFriendshipWithMajor(eOldAlly, GC.getMINOR_FRIENDSHIP_AT_WAR(), true);
	GetPlayer()->GetMinorCivAI()->EndAllActiveQuestsForPlayer(eOldAlly);

	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		PlayerTypes eMajorLoop = (PlayerTypes) iMajorLoop;

		if((GET_PLAYER(eMajorLoop).GetPlayerPolicies()->GetLateGamePolicyTree() == ePreferredIdeology) && (ePreferredIdeology != NO_POLICY_BRANCH_TYPE))
		{
			GetPlayer()->GetMinorCivAI()->ChangeFriendshipWithMajor(eMajorLoop, (GetPlayer()->GetMinorCivAI()->GetAlliesThreshold() + 5), true);
		}
	}
	CvCity* pCity = GetPlayer()->getCapitalCity();
	CvPlot* pPlot;
	CvCityCitizens* pCitizens = pCity->GetCityCitizens();
	//Let's kill off any barbs remaining.
#if defined(MOD_GLOBAL_CITY_WORKING)
	for(int iPlotLoop = 1; iPlotLoop < pCity->GetNumWorkablePlots(); iPlotLoop++)
#else
	for(int iPlotLoop = 1; iPlotLoop < NUM_CITY_PLOTS; iPlotLoop++)
#endif
	{
		pPlot = pCitizens->GetCityPlotFromIndex(iPlotLoop);

		if(pPlot->getNumUnits() > 0)
		{
			// Get the current list of units because we will possibly be moving them out of the plot's list
			IDInfoVector currentUnits;
			if (pPlot->getUnits(&currentUnits) > 0)
			{
				for(IDInfoVector::const_iterator itr = currentUnits.begin(); itr != currentUnits.end(); ++itr)
				{
					CvUnit* pLoopUnit = (CvUnit*)GetPlayerUnit(*itr);

					if(pLoopUnit && pLoopUnit->isBarbarian())
					{
						pLoopUnit->kill(false);
					}
				}
			}
		}
	}

	//Reset Rebellion, otherwise they just keep spawning forever.
	SetRebellion(false);

	//Resistance, for flavor.
	GetPlayer()->getCapitalCity()->ChangeResistanceTurns(5);

}
#endif

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

	if(IsMajorIntruding(eMajor) != bValue)
	{
		m_abMajorIntruding[eMajor] = bValue;
	}
}


/// Is a player allowed to be inside someone else's borders?
bool CvMinorCivAI::IsPlayerHasOpenBorders(PlayerTypes ePlayer)
{
	// Special trait?
	if(IsPlayerHasOpenBordersAutomatically(ePlayer))
		return true;

	return IsFriends(ePlayer);
}

/// Is a player allowed to be inside someone else's borders automatically?
bool CvMinorCivAI::IsPlayerHasOpenBordersAutomatically(PlayerTypes ePlayer)
{
	// Special trait?
	if(GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateFriendshipModifier() > 0)
		return true;

	return false;
}


/// Major liberates a Minor by recapturing its City!
void CvMinorCivAI::DoLiberationByMajor(PlayerTypes eLiberator, TeamTypes eConquerorTeam)
{
#if defined(MOD_BUGFIX_MINOR)
	// This is a bug fix as the Lua API can be used to by-pass the bought-out tests and liberate the minor regardless

	// Clear the "bought out by" indicator
	SetMajorBoughtOutBy(NO_PLAYER);
#endif

	Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LIBERATION");
	strMessage << GetPlayer()->getNameKey() << GET_PLAYER(eLiberator).getNameKey();
	Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_LIBERATION");
	strSummary << GetPlayer()->getNameKey();

	int iHighestOtherMajorInfluence = GC.getMINOR_FRIENDSHIP_AT_WAR();

	PlayerTypes ePlayer;
	for(int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		ePlayer = (PlayerTypes) iI;

		if(ePlayer != eLiberator)
		{
			int iInfluence = GetBaseFriendshipWithMajor(ePlayer);
			if(iInfluence > iHighestOtherMajorInfluence)
				iHighestOtherMajorInfluence = iInfluence;

			if(GET_PLAYER(ePlayer).isAlive())
			{
				if(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GetPlayer()->getTeam()))
				{
					// Influence for other players - Were you the one that conquered us before?
					if(GET_PLAYER(ePlayer).getTeam() == eConquerorTeam)
					{
						SetFriendshipWithMajor(ePlayer, GC.getMINOR_FRIENDSHIP_AT_WAR());
					}

					// Notification for other players
					CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
					if(pNotifications)
					{
						pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strMessage.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
					}
				}
			}
		}
	}

	// Influence for liberator - raise to ally status
	int iNewInfluence = max(iHighestOtherMajorInfluence + GC.getMINOR_LIBERATION_FRIENDSHIP(), GetBaseFriendshipWithMajor(eLiberator) + GC.getMINOR_LIBERATION_FRIENDSHIP());
	iNewInfluence = max(GetAlliesThreshold(), iNewInfluence); // Must be at least enough to make us allies
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	//Was this liberated from a barbarian? Less influence for you!
	if(MOD_DIPLOMACY_CITYSTATES_QUESTS && GET_PLAYER(BARBARIAN_PLAYER).getTeam() == eConquerorTeam)
	{
		iNewInfluence /= 2;
	}
#endif
	SetFriendshipWithMajor(eLiberator, iNewInfluence);

	// Notification for liberator
	strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LIBERATION_YOU");
	strMessage << GetPlayer()->getNameKey();
	strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_LIBERATION");
	strSummary << GetPlayer()->getNameKey();
	AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), eLiberator);
}

void CvMinorCivAI::DoChangeProtectionFromMajor(PlayerTypes eMajor, bool bProtect, bool bPledgeNowBroken)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;

	if(bProtect == IsProtectedByMajor(eMajor)) return;

	if(bProtect)
	{
		if(!CanMajorProtect(eMajor))
		{
			return;
		}
		SetTurnLastPledgedProtectionByMajor(eMajor, GC.getGame().getGameTurn());
	}
	else
	{
		if(bPledgeNowBroken)
		{
			SetTurnLastPledgeBrokenByMajor(eMajor, GC.getGame().getGameTurn());
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
			//Don't take this lightly!
			if (MOD_DIPLOMACY_CITYSTATES_QUESTS) 
				SetFriendshipWithMajor(eMajor, GC.getMINOR_FRIENDSHIP_AT_WAR(), false);
			else
#endif
				ChangeFriendshipWithMajorTimes100(eMajor, GC.getMINOR_FRIENDSHIP_DROP_DISHONOR_PLEDGE_TO_PROTECT());
		}
	}

	m_abPledgeToProtect[eMajor] = bProtect;

	// In case we had a Pledge to Protect quest active, complete it now
	DoTestActiveQuestsForPlayer(eMajor, /*bTestComplete*/ true, /*bTestObsolete*/ false, MINOR_CIV_QUEST_PLEDGE_TO_PROTECT);

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
}

bool CvMinorCivAI::CanMajorProtect(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;

	// If at war, may not protect
	if(GET_TEAM(GET_PLAYER(eMajor).getTeam()).isAtWar(GetPlayer()->getTeam()))
		return false;

	// Must have positive INF
	if(GetEffectiveFriendshipWithMajor(eMajor) < /*0*/ GC.getFRIENDSHIP_THRESHOLD_CAN_PLEDGE_TO_PROTECT())
		return false;

	// Must not be too soon after a previous pledge was broken
	int iCurrentTurn = GC.getGame().getGameTurn();
	int iLastPledgeBrokenTurn = GetTurnLastPledgeBrokenByMajor(eMajor);
	const int iGracePeriod = 20; //antonjs: todo: xml
	if(iLastPledgeBrokenTurn >= 0 && iLastPledgeBrokenTurn + iGracePeriod > iCurrentTurn)
		return false;

	return true;
}

bool CvMinorCivAI::CanMajorStartProtection(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;

	return (CanMajorProtect(eMajor) && !IsProtectedByMajor(eMajor));
}

bool CvMinorCivAI::CanMajorWithdrawProtection(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;

	if (!IsProtectedByMajor(eMajor))
		return false;

	// Pledge is locked in for a certain time
	int iCurrentTurn = GC.getGame().getGameTurn();
	int iLastPledgeTurn = GetTurnLastPledgedProtectionByMajor(eMajor);
	const int iGracePeriod = 10; //antonjs: todo: xml
	if (iLastPledgeTurn >= 0 && iLastPledgeTurn + iGracePeriod > iCurrentTurn)
		return false;

	return true;
}

bool CvMinorCivAI::IsProtectedByMajor(PlayerTypes eMajor) const
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;

	return m_abPledgeToProtect[eMajor];
}

bool CvMinorCivAI::IsProtectedByAnyMajor() const
{
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		if(IsProtectedByMajor((PlayerTypes)iPlayerLoop))
			return true;
	return false;
}

int CvMinorCivAI::GetTurnLastPledgedProtectionByMajor(PlayerTypes eMajor) const
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return -1;

	return m_aiTurnLastPledged[eMajor];
}

void CvMinorCivAI::SetTurnLastPledgedProtectionByMajor(PlayerTypes eMajor, int iTurn)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;

	m_aiTurnLastPledged[eMajor] = iTurn;
}

int CvMinorCivAI::GetTurnLastPledgeBrokenByMajor(PlayerTypes eMajor) const
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return -1;

	return m_aiTurnLastBrokePledge[eMajor];
}

void CvMinorCivAI::SetTurnLastPledgeBrokenByMajor(PlayerTypes eMajor, int iTurn)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;

	m_aiTurnLastBrokePledge[eMajor] = iTurn;
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
	if(eTrait == MINOR_CIV_TRAIT_MARITIME)
	{
		// Friends
		if(IsFriends(ePlayer))
		{
			int iOldFood, iNewFood;

			// Capital
			iOldFood = GetFriendsCapitalFoodBonus(ePlayer);
			iNewFood = GetFriendsCapitalFoodBonus(ePlayer, eNewEra);

			if(iOldFood != iNewFood)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).ChangeCapitalYieldChange(YIELD_FOOD, iNewFood - iOldFood);
			}

			// Other Cities
			iOldFood = GetFriendsOtherCityFoodBonus(ePlayer);
			iNewFood = GetFriendsOtherCityFoodBonus(ePlayer, eNewEra);

			if(iOldFood != iNewFood)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).ChangeCityYieldChange(YIELD_FOOD, iNewFood - iOldFood);
			}
		}

		// Allies
		if(IsAllies(ePlayer))
		{
			int iOldFood, iNewFood;

			// Capital
			iOldFood = GetAlliesCapitalFoodBonus(ePlayer);
			iNewFood = GetAlliesCapitalFoodBonus(ePlayer);

			if(iOldFood != iNewFood)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).ChangeCapitalYieldChange(YIELD_FOOD, iNewFood - iOldFood);
			}

			// Other Cities
			iOldFood = GetAlliesOtherCityFoodBonus(ePlayer);
			iNewFood = GetAlliesOtherCityFoodBonus(ePlayer);

			if(iOldFood != iNewFood)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).ChangeCityYieldChange(YIELD_FOOD, iNewFood - iOldFood);
			}
		}
	}

	// CULTURED
	else if(eTrait == MINOR_CIV_TRAIT_CULTURED)
	{
		// Friends
		if(IsFriends(ePlayer))
		{
			int iOldCulture = GetCultureFlatFriendshipBonus(ePlayer) + GetCulturePerBuildingFriendshipBonus(ePlayer);
			int iNewCulture = GetCultureFlatFriendshipBonus(ePlayer, eNewEra) + GetCulturePerBuildingFriendshipBonus(ePlayer, eNewEra);

			if(iOldCulture != iNewCulture)
			{
				bSomethingChanged = true;
			}
		}

		// Allies
		if(IsAllies(ePlayer))
		{
			int iOldCulture = GetCultureFlatAlliesBonus(ePlayer) + GetCulturePerBuildingAlliesBonus(ePlayer);
			int iNewCulture = GetCultureFlatAlliesBonus(ePlayer, eNewEra) + GetCulturePerBuildingAlliesBonus(ePlayer, eNewEra);

			if(iOldCulture != iNewCulture)
			{
				bSomethingChanged = true;
			}
		}
	}

	// MERCANTILE
	else if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
	{
		// Friends
		if(IsFriends(ePlayer))
		{
			int iOldHappiness, iNewHappiness;

			iOldHappiness = GetHappinessFlatFriendshipBonus(ePlayer) + GetHappinessPerLuxuryFriendshipBonus(ePlayer);
			iNewHappiness = GetHappinessFlatFriendshipBonus(ePlayer, eNewEra) + GetHappinessPerLuxuryFriendshipBonus(ePlayer, eNewEra);

			if(iOldHappiness != iNewHappiness)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).DoUpdateHappiness();
			}
		}

		// Allies
		if(IsAllies(ePlayer))
		{
			int iOldHappiness, iNewHappiness;

			iOldHappiness = GetHappinessFlatAlliesBonus(ePlayer) + GetHappinessPerLuxuryAlliesBonus(ePlayer);
			iNewHappiness = GetHappinessFlatAlliesBonus(ePlayer, eNewEra) + GetHappinessPerLuxuryAlliesBonus(ePlayer, eNewEra);

			if(iOldHappiness != iNewHappiness)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).DoUpdateHappiness();
			}
		}
	}

	// RELIGIOUS
	else if(eTrait == MINOR_CIV_TRAIT_RELIGIOUS)
	{
		// Friends
		if(IsFriends(ePlayer))
		{
			int iOldFaith = GetFaithFlatFriendshipBonus(ePlayer);
			int iNewFaith = GetFaithFlatFriendshipBonus(ePlayer, eNewEra);

			if(iOldFaith != iNewFaith)
			{
				bSomethingChanged = true;
			}
		}

		// Allies
		if(IsAllies(ePlayer))
		{
			int iOldFaith = GetFaithFlatAlliesBonus(ePlayer);
			int iNewFaith = GetFaithFlatAlliesBonus(ePlayer, eNewEra);

			if(iOldFaith != iNewFaith)
			{
				bSomethingChanged = true;
			}
		}
	}

	return bSomethingChanged;
}

// Science bonus when friends with a minor
int CvMinorCivAI::GetScienceFriendshipBonus()
{
	int iResult = GetScienceFriendshipBonusTimes100();
	iResult /= 100;

	return iResult;
}

int CvMinorCivAI::GetScienceFriendshipBonusTimes100()
{
	int iResult = GET_PLAYER(m_pPlayer->GetID()).GetScienceTimes100() * /*25*/ GC.getMINOR_CIV_SCIENCE_BONUS_MULTIPLIER();
	iResult /= 100;

	return iResult;
}

/// How much are we getting RIGHT NOW (usually 0)
int CvMinorCivAI::GetCurrentScienceFriendshipBonusTimes100(PlayerTypes ePlayer)
{
	if(GET_PLAYER(ePlayer).IsGetsScienceFromPlayer(GetPlayer()->GetID()))
		return GetScienceFriendshipBonusTimes100();

	return 0;
}

// Flat culture bonus when Friends with a minor
int CvMinorCivAI::GetCultureFlatFriendshipBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	//antonjs: consider: here it was assumed that this CS is cultured type, which is nice for me prototyping, but later maybe add in a check for this
	int iCultureBonus = 0;

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);

	// Industrial era or Later
	if(eCurrentEra >= eIndustrial)
	{
		iCultureBonus += /*10*/ GC.getFRIENDS_CULTURE_BONUS_AMOUNT_INDUSTRIAL();
	}

	// Medieval era or later
	else if(eCurrentEra >= eMedieval)
	{
		iCultureBonus += /*6*/ GC.getFRIENDS_CULTURE_BONUS_AMOUNT_MEDIEVAL();
	}

	// Pre-Medieval
	else
	{
		iCultureBonus += /*4*/ GC.getFRIENDS_CULTURE_BONUS_AMOUNT_ANCIENT();
	}

	return iCultureBonus;
}

// Flat culture bonus when Allies with a minor
int CvMinorCivAI::GetCultureFlatAlliesBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	//antonjs: consider: here it was assumed that this CS is cultured type, which is nice for me prototyping, but later maybe add in a check for this
	int iCultureBonus = 0;

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);

	// Industrial era or Later
	if(eCurrentEra >= eIndustrial)
	{
		iCultureBonus += /*10*/ GC.getALLIES_CULTURE_BONUS_AMOUNT_INDUSTRIAL();
	}

	// Medieval era or later
	else if(eCurrentEra >= eMedieval)
	{
		iCultureBonus += /*6*/ GC.getALLIES_CULTURE_BONUS_AMOUNT_MEDIEVAL();
	}

	// Pre-Medieval
	else
	{
		iCultureBonus += /*4*/ GC.getALLIES_CULTURE_BONUS_AMOUNT_ANCIENT();
	}

	return iCultureBonus;
}

/// Flat-rate culture bonus
int CvMinorCivAI::GetCurrentCultureFlatBonus(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_PLAYERS) return 0;

	// Don't give a bonus to a minor civ player
	if(ePlayer >= MAX_MAJOR_CIVS)
		return 0;

	// Only give a bonus if we are Cultural trait
	if(GetTrait() != MINOR_CIV_TRAIT_CULTURED)
		return 0;

	int iAmount = 0;

	if(IsAllies(ePlayer))
		iAmount += GetCultureFlatAlliesBonus(ePlayer);

	if(IsFriends(ePlayer))
		iAmount += GetCultureFlatFriendshipBonus(ePlayer);

	// Modify the bonus if called for by our trait
	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();
	if(iModifier > 0)
	{
		iAmount *= (iModifier + 100);
		iAmount /= 100;
	}

	return iAmount;
}

//antonjs: This feature was prototyped, but later removed. Rewrite this function to add the bonus back in.
/// Cumulative per building culture bonus from friendship with a minor
int CvMinorCivAI::GetCulturePerBuildingFriendshipBonus(PlayerTypes /*ePlayer*/, EraTypes /*eAssumeEra*/)
{
	return 0;
}

//antonjs: This feature was prototyped, but later removed. Rewrite this function to add the bonus back in.
/// Cumulative per building culture bonus from being allies with a minor
int CvMinorCivAI::GetCulturePerBuildingAlliesBonus(PlayerTypes /*ePlayer*/, EraTypes /*eAssumeEra*/)
{
	return 0;
}

//antonjs: This feature was prototyped, but later removed. It will return 0 (no bonus).
int CvMinorCivAI::GetCurrentCulturePerBuildingBonus(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_PLAYERS) return 0;

	// Don't give a bonus to a minor civ player
	if(ePlayer >= MAX_MAJOR_CIVS)
		return 0;

	// Only give a bonus if we are Cultural trait
	if(GetTrait() != MINOR_CIV_TRAIT_CULTURED)
		return 0;

	int iAmount = 0;

	if(IsAllies(ePlayer))
		iAmount += GetCulturePerBuildingAlliesBonus(ePlayer);

	if(IsFriends(ePlayer))
		iAmount += GetCulturePerBuildingFriendshipBonus(ePlayer);

	// Modify the bonus if called for by our trait
	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();
	if(iModifier > 0)
	{
		iAmount *= (iModifier + 100);
		iAmount /= 100;
	}

	return iAmount;
}

int CvMinorCivAI::GetCurrentCultureBonus(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_PLAYERS) return 0;

	int iAmount = 0;

	iAmount += GetCurrentCultureFlatBonus(ePlayer);
	iAmount += GetCurrentCulturePerBuildingBonus(ePlayer); //antonjs: This feature was prototyped, but later removed. Its value is 0 (no bonus).

	return iAmount;
}

/// Flat happiness bonus from friendship with a minor
int CvMinorCivAI::GetHappinessFlatFriendshipBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);

	// Industrial era or Later
	if(eCurrentEra >= eIndustrial)
		return GC.getFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_INDUSTRIAL();

	// Medieval era or later
	else if(eCurrentEra >= eMedieval)
		return GC.getFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_MEDIEVAL();

	// Pre-Medieval
	else
		return GC.getFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_ANCIENT();
}

/// Flat happiness bonus from being allies with a minor
int CvMinorCivAI::GetHappinessFlatAlliesBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);

	// Industrial era or Later
	if(eCurrentEra >= eIndustrial)
		return GC.getALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_INDUSTRIAL();

	// Medieval era or later
	else if(eCurrentEra >= eMedieval)
		return GC.getALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_MEDIEVAL();

	// Pre-Medieval
	else
		return GC.getALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_ANCIENT();
}

/// Flat happiness bonus currently in effect
int CvMinorCivAI::GetCurrentHappinessFlatBonus(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_PLAYERS) return 0;

	// Minor civ players do not get a bonus
	if(ePlayer >= MAX_MAJOR_CIVS)
		return 0;

	// Only for Mercantile trait minors
	if(GetTrait() != MINOR_CIV_TRAIT_MERCANTILE)
		return 0;

	int iAmount = 0;
	if(IsAllies(ePlayer))
		iAmount += GetHappinessFlatAlliesBonus(ePlayer);
	if(IsFriends(ePlayer))
		iAmount += GetHappinessFlatFriendshipBonus(ePlayer);
	return iAmount;
}

/// Cumulative per luxury happiness bonus from friendship with a minor
int CvMinorCivAI::GetHappinessPerLuxuryFriendshipBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	//antonjs: consider: optimize
	int iNumLuxuries = 0;
	ResourceTypes eResource;
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		eResource = (ResourceTypes) iResourceLoop;

		if(GET_PLAYER(ePlayer).getNumResourceAvailable(eResource) > 0)
		{
			const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			if(pkResourceInfo != NULL && pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
			{
				iNumLuxuries++;
			}
		}
	}

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);

	// Industrial era or Later
	if(eCurrentEra >= eIndustrial)
		return (iNumLuxuries * GC.getFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_INDUSTRIAL());

	// Medieval era or later
	else if(eCurrentEra >= eMedieval)
		return (iNumLuxuries * GC.getFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_MEDIEVAL());

	// Pre-Medieval
	else
		return (iNumLuxuries * GC.getFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_ANCIENT());
}

/// Cumulative per luxury happiness bonus from being allies with a minor
int CvMinorCivAI::GetHappinessPerLuxuryAlliesBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	//antonjs: consider: optimize
	int iNumLuxuries = 0;
	ResourceTypes eResource;
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		eResource = (ResourceTypes) iResourceLoop;

		if(GET_PLAYER(ePlayer).getNumResourceAvailable(eResource) > 0)
		{
			const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			if(pkResourceInfo != NULL && pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
			{
				iNumLuxuries++;
			}
		}
	}

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);

	// Industrial era or Later
	if(eCurrentEra >= eIndustrial)
		return (iNumLuxuries * GC.getALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_INDUSTRIAL());

	// Medieval era or later
	else if(eCurrentEra >= eMedieval)
		return (iNumLuxuries * GC.getALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_MEDIEVAL());

	// Pre-Medieval
	else
		return (iNumLuxuries * GC.getALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_ANCIENT());
}

/// Per luxury happiness bonus currently in effect
int CvMinorCivAI::GetCurrentHappinessPerLuxuryBonus(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_PLAYERS) return 0;

	// Minor civ players do not get a bonus
	if(ePlayer >= MAX_MAJOR_CIVS)
		return 0;

	// Only for Mercantile trait minors
	if(GetTrait() != MINOR_CIV_TRAIT_MERCANTILE)
		return 0;

	int iAmount = 0;
	if(IsAllies(ePlayer))
		iAmount += GetHappinessPerLuxuryAlliesBonus(ePlayer);
	if(IsFriends(ePlayer))
		iAmount += GetHappinessPerLuxuryFriendshipBonus(ePlayer);
	return iAmount;
}

/// Total happiness bonus from this minor civ for this player
int CvMinorCivAI::GetCurrentHappinessBonus(PlayerTypes ePlayer)
{
	int iValue = 0;

	iValue += GetCurrentHappinessFlatBonus(ePlayer);
	iValue += GetCurrentHappinessPerLuxuryBonus(ePlayer);

	return iValue;
}

int CvMinorCivAI::GetFaithFlatFriendshipBonus(PlayerTypes ePlayer, EraTypes eAssumeEra) const
{
	int iFaithBonus = 0;

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);
	EraTypes eClassical = (EraTypes) GC.getInfoTypeForString("ERA_CLASSICAL", true);

	// Industrial era or later
	if(eCurrentEra >= eIndustrial)
	{
		iFaithBonus += GC.getFRIENDS_FAITH_FLAT_BONUS_AMOUNT_INDUSTRIAL();
	}

	// Renaissance era
	else if(eCurrentEra >= eRenaissance)
	{
		iFaithBonus += GC.getFRIENDS_FAITH_FLAT_BONUS_AMOUNT_RENAISSANCE();
	}

	// Medieval era
	else if(eCurrentEra >= eMedieval)
	{
		iFaithBonus += GC.getFRIENDS_FAITH_FLAT_BONUS_AMOUNT_MEDIEVAL();
	}

	// Classical era
	else if(eCurrentEra >= eClassical)
	{
		iFaithBonus += GC.getFRIENDS_FAITH_FLAT_BONUS_AMOUNT_CLASSICAL();
	}

	// Ancient era
	else
	{
		iFaithBonus += GC.getFRIENDS_FAITH_FLAT_BONUS_AMOUNT_ANCIENT();
	}

	return iFaithBonus;
}

int CvMinorCivAI::GetFaithFlatAlliesBonus(PlayerTypes ePlayer, EraTypes eAssumeEra) const
{
	int iFaithBonus = 0;

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);
	EraTypes eClassical = (EraTypes) GC.getInfoTypeForString("ERA_CLASSICAL", true);

	// Industrial era or later
	if(eCurrentEra >= eIndustrial)
	{
		iFaithBonus += GC.getALLIES_FAITH_FLAT_BONUS_AMOUNT_INDUSTRIAL();
	}

	// Renaissance era
	else if(eCurrentEra >= eRenaissance)
	{
		iFaithBonus += GC.getALLIES_FAITH_FLAT_BONUS_AMOUNT_RENAISSANCE();
	}

	// Medieval era
	else if(eCurrentEra >= eMedieval)
	{
		iFaithBonus += GC.getALLIES_FAITH_FLAT_BONUS_AMOUNT_MEDIEVAL();
	}

	// Classical era
	else if(eCurrentEra >= eClassical)
	{
		iFaithBonus += GC.getALLIES_FAITH_FLAT_BONUS_AMOUNT_CLASSICAL();
	}

	// Ancient era
	else
	{
		iFaithBonus += GC.getALLIES_FAITH_FLAT_BONUS_AMOUNT_ANCIENT();
	}

	return iFaithBonus;
}

int CvMinorCivAI::GetCurrentFaithFlatBonus(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_PLAYERS) return 0;

	// Minor civ players do not get a bonus
	if(ePlayer >= MAX_MAJOR_CIVS)
		return 0;

	// Only for Religious trait minors
	if(GetTrait() != MINOR_CIV_TRAIT_RELIGIOUS)
		return 0;

	int iAmount = 0;
	if(IsAllies(ePlayer))
		iAmount += GetFaithFlatAlliesBonus(ePlayer);
	if(IsFriends(ePlayer))
		iAmount += GetFaithFlatFriendshipBonus(ePlayer);

	// Modify the bonus if called for by our trait
	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();
	if(iModifier > 0)
	{
		iAmount *= (iModifier + 100);
		iAmount /= 100;
	}

	return iAmount;
}

/// Total faith bonus from this minor civ for this player
int CvMinorCivAI::GetCurrentFaithBonus(PlayerTypes ePlayer)
{
	int iValue = 0;

	iValue += GetCurrentFaithFlatBonus(ePlayer);

	return iValue;
}

// Food bonus when Friends with a minor - additive with general city bonus
int CvMinorCivAI::GetFriendsCapitalFoodBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	int iBonus;

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);

	// Medieval era or sooner
	if(eCurrentEra < eRenaissance)
		iBonus = /*200*/ GC.getFRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE();

	// Renaissance era or later
	else
		iBonus = /*200*/ GC.getFRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_POST_RENAISSANCE();

	// Modify the bonus if called for by our trait
	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();
	if(iModifier > 0)
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
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);

	// Medieval era or sooner
	if(eCurrentEra < eRenaissance)
		iBonus = /*0*/ GC.getFRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE();

	// Renaissance era or later
	else
		iBonus = /*0*/ GC.getFRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_POST_RENAISSANCE();

	// Modify the bonus if called for by our trait
	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();
	if(iModifier > 0)
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
	if(iModifier > 0)
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
	if(iModifier > 0)
	{
		iBonus *= (iModifier + 100);
		iBonus /= 100;
	}

	return iBonus;
}

/// How much are we getting RIGHT NOW (usually 0)
int CvMinorCivAI::GetCurrentCapitalFoodBonus(PlayerTypes ePlayer)
{
	// This guy isn't Maritime
	if(GetTrait() != MINOR_CIV_TRAIT_MARITIME)
		return 0;

	int iAmount = 0;

	if(IsAllies(ePlayer))
	{
		iAmount += GetAlliesCapitalFoodBonus(ePlayer);
		iAmount += GetAlliesOtherCityFoodBonus(ePlayer);
	}

	if(IsFriends(ePlayer))
	{
		iAmount += GetFriendsCapitalFoodBonus(ePlayer);
		iAmount += GetFriendsOtherCityFoodBonus(ePlayer);
	}

	return iAmount;
}

/// How much are we getting RIGHT NOW (usually 0)
int CvMinorCivAI::GetCurrentOtherCityFoodBonus(PlayerTypes ePlayer)
{
	// This guy isn't Maritime
	if(GetTrait() != MINOR_CIV_TRAIT_MARITIME)
		return 0;

	int iAmount = 0;

	if(IsAllies(ePlayer))
		iAmount += GetAlliesOtherCityFoodBonus(ePlayer);

	if(IsFriends(ePlayer))
		iAmount += GetFriendsOtherCityFoodBonus(ePlayer);

	return iAmount;
}

// Figures out how long before we spawn a free unit for ePlayer
void CvMinorCivAI::DoSeedUnitSpawnCounter(PlayerTypes ePlayer, bool bBias)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	int iNumTurns = GetSpawnBaseTurns(ePlayer);

	// Add some randomness
	int iRand = /*3*/ GC.getFRIENDS_RAND_TURNS_UNIT_SPAWN();
	iNumTurns += GC.getGame().getJonRandNum(iRand, "Rand turns for Friendly Minor unit spawn");

	// If we're biasing the result then decrease the number of turns
	if(bBias)
	{
		iNumTurns *= /*50*/ GC.getUNIT_SPAWN_BIAS_MULTIPLIER();
		iNumTurns /= 100;
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
	if(!IsHasMetPlayer(ePlayer))
		return false;

	// Must be Militaristic
	if(GetTrait() != MINOR_CIV_TRAIT_MILITARISTIC)
		return false;

	// Can't be at war!
	if(IsAtWarWithPlayersTeam(ePlayer))
		return false;

	// Must be Friends
	if(!IsFriends(ePlayer))
		return false;

	// We must be alive
	if(!GetPlayer()->isAlive())
		return false;

	// They must be alive
	if(!GET_PLAYER(ePlayer).isAlive())
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
#if defined(MOD_GLOBAL_CS_GIFTS)
CvUnit* CvMinorCivAI::DoSpawnUnit(PlayerTypes eMajor, bool bLocal, bool bExplore)
#else
void CvMinorCivAI::DoSpawnUnit(PlayerTypes eMajor)
#endif
{
#if defined(MOD_GLOBAL_CS_GIFTS)
	CvUnit* pSpawnUnit = NULL;
#endif

	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
#if defined(MOD_GLOBAL_CS_GIFTS)
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return NULL;
#else
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;
#endif

	if(!IsUnitSpawningDisabled(eMajor))
	{
		// Minor must have Capital
		CvCity* pMinorCapital = GetPlayer()->getCapitalCity();
		if(pMinorCapital == NULL)
		{
			FAssertMsg(false, "MINOR CIV AI: Trying to spawn a Unit for a major civ but the minor has no capital. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
#if defined(MOD_GLOBAL_CS_GIFTS)
			return NULL;
#else
			return;
#endif
		}
		CvPlot* pMinorCapitalPlot = pMinorCapital->plot();
		if(pMinorCapitalPlot == NULL)
		{
			CvAssertMsg(false, "MINOR CIV AI: Trying to spawn a Unit for a major civ but the minor's capital has no plot. Please send Anton your save file and version.");
#if defined(MOD_GLOBAL_CS_GIFTS)
			return NULL;
#else
			return;
#endif
		}

		CvCity* pMajorCity = GET_PLAYER(eMajor).GetClosestFriendlyCity(*pMinorCapitalPlot, MAX_INT);

		int iX = pMinorCapital->getX();
		int iY = pMinorCapital->getY();
#if defined(MOD_GLOBAL_CS_GIFTS)
		if(!bLocal && pMajorCity != NULL)
#else
		if(pMajorCity != NULL)
#endif
		{
			iX = pMajorCity->getX();
			iY = pMajorCity->getY();
		}

		// Pick Unit type
		UnitTypes eUnit = NO_UNIT;
		if (GetAlly() == eMajor)
		{	
			// Should we give our unique unit?
			bool bUseUniqueUnit = false;
			UnitTypes eUniqueUnit = GetUniqueUnit();
			if (eUniqueUnit != NO_UNIT)
			{
				CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUniqueUnit);
				if (pkUnitInfo)
				{
					// Ally must have unit's prereq tech
					TechTypes ePrereqTech = (TechTypes) pkUnitInfo->GetPrereqAndTech();
					if (ePrereqTech == NO_TECH || GET_TEAM(GET_PLAYER(eMajor).getTeam()).GetTeamTechs()->HasTech(ePrereqTech))
					{
						// Ally must NOT have unit's obsolete tech
						TechTypes eObsoleteTech = (TechTypes) pkUnitInfo->GetObsoleteTech();
						if (eObsoleteTech == NO_TECH || !GET_TEAM(GET_PLAYER(eMajor).getTeam()).GetTeamTechs()->HasTech(eObsoleteTech))
						{
							bUseUniqueUnit = true;
						}
					}
				}
			}
			
#if defined(MOD_GLOBAL_CS_GIFTS)
			if (bExplore) {
#if defined(MOD_GLOBAL_CS_GIFT_SHIPS)
				eUnit = GC.getGame().GetCsGiftSpawnUnitType(eMajor, GetPlayer()->getCapitalCity()->plot()->isCoastalLand() && MOD_GLOBAL_CS_GIFT_SHIPS);
#else
				eUnit = GC.getGame().GetCsGiftSpawnUnitType(eMajor);
#endif
			} else {
#endif
				if (bUseUniqueUnit)
				{
					eUnit = eUniqueUnit;
				}
				else
				{
#if defined(MOD_GLOBAL_CS_GIFT_SHIPS)
					eUnit = GC.getGame().GetCompetitiveSpawnUnitType(eMajor, /*bIncludeUUs*/ false, /*bIncludeRanged*/true, MOD_GLOBAL_CS_GIFT_SHIPS);
#else
					eUnit = GC.getGame().GetCompetitiveSpawnUnitType(eMajor, /*bIncludeUUs*/ false, /*bIncludeRanged*/true);
#endif
				}
#if defined(MOD_GLOBAL_CS_GIFTS)
			}
#endif
		}
		else
		{
#if defined(MOD_GLOBAL_CS_GIFTS)
			if (bExplore) {
#if defined(MOD_GLOBAL_CS_GIFT_SHIPS)
				eUnit = GC.getGame().GetCsGiftSpawnUnitType(eMajor, GetPlayer()->getCapitalCity()->plot()->isCoastalLand() && MOD_GLOBAL_CS_GIFT_SHIPS);
#else
				eUnit = GC.getGame().GetCsGiftSpawnUnitType(eMajor);
#endif
			} else {
#endif
#if defined(MOD_GLOBAL_CS_GIFT_SHIPS)
				eUnit = GC.getGame().GetCompetitiveSpawnUnitType(eMajor, /*bIncludeUUs*/ false, /*bIncludeRanged*/true, MOD_GLOBAL_CS_GIFT_SHIPS);
#else
				eUnit = GC.getGame().GetCompetitiveSpawnUnitType(eMajor, /*bIncludeUUs*/ false, /*bIncludeRanged*/true);
#endif
#if defined(MOD_GLOBAL_CS_GIFTS)
			}
#endif
		}

		// Spawn Unit
		if(eUnit != NO_UNIT)
		{
			CvUnit* pNewUnit = GET_PLAYER(eMajor).initUnit(eUnit, iX, iY);

			// If player trait is to enhance minor bonuses, give this unit some free experience
			if(GET_PLAYER(eMajor).GetPlayerTraits()->GetCityStateBonusModifier() > 0)
			{
				pNewUnit->changeExperience(GC.getMAX_EXPERIENCE_PER_COMBAT());
			}

			if (pNewUnit->jumpToNearestValidPlot())
			{
				if(GetPlayer()->getCapitalCity())
					GetPlayer()->getCapitalCity()->addProductionExperience(pNewUnit);

				Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_STATE_UNIT_SPAWN");
				strMessage << GetPlayer()->getNameKey();
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_STATE_UNIT_SPAWN");
				strSummary << GetPlayer()->getNameKey();

				AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), eMajor, pNewUnit->getX(), pNewUnit->getY());

#if defined(MOD_GLOBAL_CS_GIFTS)
				pSpawnUnit = pNewUnit;
#endif
			}
			else
				pNewUnit->kill(false);	// Could not find a spot!
		}
	}

	// Reseed counter
	DoSeedUnitSpawnCounter(eMajor);

#if defined(MOD_GLOBAL_CS_GIFTS)
	return pSpawnUnit;
#endif
}

/// Time to spawn a Unit?
void CvMinorCivAI::DoUnitSpawnTurn()
{
	// Loop through all players and see if we should give them a Unit
	PlayerTypes eMajor;
	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;

		if(IsUnitSpawningAllowed(eMajor))
		{
			// Tick down
			if(GetUnitSpawnCounter(eMajor) > 0)
			{
				ChangeUnitSpawnCounter(eMajor, -1);
			}

			// Time to spawn!
			if(GetUnitSpawnCounter(eMajor) == 0)
			{
				DoSpawnUnit(eMajor);
			}
		}
	}
}

/// What is the base number of turns between unit spawns, before randomness is applied?
int CvMinorCivAI::GetSpawnBaseTurns(PlayerTypes ePlayer)
{
	// Not friends
	if(!IsFriends(ePlayer))
		return 0;

	// This guy isn't militaristic
	if(GetTrait() != MINOR_CIV_TRAIT_MILITARISTIC)
		return 0;

	int iNumTurns = /*19*/ GC.getFRIENDS_BASE_TURNS_UNIT_SPAWN() * 100;

	// If relations are at allied level then reduce spawn counter
	if(IsAllies(ePlayer))
		iNumTurns += /*-3*/ (GC.getALLIES_EXTRA_TURNS_UNIT_SPAWN() * 100);

	// Modify for Game Speed
	iNumTurns *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iNumTurns /= 100;

	// Modify for policies
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	int iPolicyMod = kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_UNIT_FREQUENCY_MODIFIER);
	if(iPolicyMod > 0)
	{
		if(GET_TEAM(kPlayer.getTeam()).HasCommonEnemy(m_pPlayer->getTeam()))
		{
			iNumTurns *= 100;
			iNumTurns /= (100 + iPolicyMod);
		}
	}

	return iNumTurns / 100;
}

/// What is the average number of turns between unit spawns?
int CvMinorCivAI::GetCurrentSpawnEstimate(PlayerTypes ePlayer)
{
	// Not friends
	if(!IsFriends(ePlayer))
		return 0;

	// This guy isn't militaristic
	if(GetTrait() != MINOR_CIV_TRAIT_MILITARISTIC)
		return 0;

	int iNumTurns = GetSpawnBaseTurns(ePlayer) * 100;

	int iRand = /*3*/ GC.getFRIENDS_RAND_TURNS_UNIT_SPAWN() * 100;
	iNumTurns += (iRand / 2);

	return iNumTurns / 100;
}

/// Has this minor been bought out by someone?
bool CvMinorCivAI::IsBoughtOut() const
{
	PlayerTypes eBuyoutPlayer = GetMajorBoughtOutBy();
	return (eBuyoutPlayer != NO_PLAYER);
}

/// What player has bought out this minor?
PlayerTypes CvMinorCivAI::GetMajorBoughtOutBy() const
{
	return m_eMajorBoughtOutBy;
}

/// This minor has been bought out by a major civ
void CvMinorCivAI::SetMajorBoughtOutBy(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");

	m_eMajorBoughtOutBy = eMajor;
}

/// Can this minor be bought out by this major?  (Austria UA)
bool CvMinorCivAI::CanMajorBuyout(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;

	// Is alive?
	if (!GET_PLAYER(eMajor).isAlive() || !GetPlayer()->isAlive())
		return false;
	
	// Has the trait or the policy?
	if(!GET_PLAYER(eMajor).IsAbleToAnnexCityStates())
		return false;

	// Not at war?
	if(GET_TEAM(GetPlayer()->getTeam()).isAtWar(GET_PLAYER(eMajor).getTeam()))
		return false;

	// Allies?
	if(!IsAllies(eMajor))
		return false;

	// Allied long enough?
	if (GetAlliedTurns() < GC.getMINOR_CIV_BUYOUT_TURNS())
	{
		return false;
	}

	// Has enough gold?
	const int iBuyoutCost = GetBuyoutCost(eMajor);
	if(GET_PLAYER(eMajor).GetTreasury()->GetGold() < iBuyoutCost)
		return false;

	return true;
}

int CvMinorCivAI::GetBuyoutCost(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return -1;

	int iGold = GC.getMINOR_CIV_BUYOUT_COST();

	// Game Speed Mod
	iGold *= GC.getGame().getGameSpeedInfo().getGoldPercent();
	iGold /= 100;

	// Add in the scrap cost of all this minor's units
	int iScrapCost = 0;
	int iUnitLoop;
	CvUnit *pLoopUnit;
	for (pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
	{
		iScrapCost += pLoopUnit->GetScrapGold();
	}
	iGold += iScrapCost;

	// Rounding
	int iVisibleDivisor = /*5*/ GC.getMINOR_CIV_GOLD_GIFT_VISIBLE_DIVISOR();
	iGold /= iVisibleDivisor;
	iGold *= iVisibleDivisor;

	return iGold;
}

void CvMinorCivAI::DoBuyout(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;

	if(!CanMajorBuyout(eMajor))
		return;
	

#if !defined(NO_ACHIEVEMENTS)
	//Nigerian Prince Achievement
	MinorCivTypes eBornu =(MinorCivTypes) GC.getInfoTypeForString("MINOR_CIV_BORNU", true /*bHideAssert*/);
	MinorCivTypes  eSokoto =(MinorCivTypes) GC.getInfoTypeForString("MINOR_CIV_SOKOTO", true /*bHideAssert*/);
	bool bUsingXP2Scenario2 = gDLL->IsModActivated(CIV5_XP2_SCENARIO2_MODID);

	if (GET_PLAYER(eMajor).isHuman() && bUsingXP2Scenario2 && (GetPlayer()->GetMinorCivAI()->GetMinorCivType() == eBornu || GetPlayer()->GetMinorCivAI()->GetMinorCivType() == eSokoto ))
		gDLL->UnlockAchievement(ACHIEVEMENT_XP2_54);
#endif


	// Pay the cost
	const int iBuyoutCost = GetBuyoutCost(eMajor);
	GET_PLAYER(eMajor).GetTreasury()->LogExpenditure(GetPlayer()->GetMinorCivAI()->GetNamesListAsString(0), iBuyoutCost,6);
	GET_PLAYER(eMajor).GetTreasury()->ChangeGold(-iBuyoutCost);

	int iNumUnits = 0;
	int iCapitalX = 0;
	int iCapitalY = 0;
	DoAcquire(eMajor, iNumUnits, iCapitalX, iCapitalY);

	GET_PLAYER(eMajor).GetDiplomacyAI()->LogMinorCivBuyout(GetPlayer()->GetID(), iBuyoutCost, /*bSaving*/ false);

	// Show special notifications
	int iCoinToss = GC.getGame().getJonRandNum(2, "Coin toss roll to determine flavor message for minor civ buyout notification.");
	Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BUYOUT_TT_1");
	if (iCoinToss == 0) // Is it a boy or a girl?
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BUYOUT_TT_2");
	strMessage << GET_PLAYER(eMajor).getCivilizationShortDescriptionKey();
	strMessage << GetPlayer()->getCivilizationShortDescriptionKey();

	Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BUYOUT");
	strSummary << GET_PLAYER(eMajor).getCivilizationShortDescriptionKey();
	strSummary << GetPlayer()->getCivilizationShortDescriptionKey();

	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		PlayerTypes eMajorLoop = (PlayerTypes) iMajorLoop;
		if (IsHasMetPlayer(eMajorLoop))
		{
			AddBuyoutNotification(strMessage.toUTF8(), strSummary.toUTF8(), eMajorLoop, iCapitalX, iCapitalY);
		}
	}

	

	CvPlayerAI& kMajorPlayer = GET_PLAYER(eMajor);
	kMajorPlayer.GetPlayerAchievements().BoughtCityState(iNumUnits);
}

#if defined(MOD_GLOBAL_VENICE_KEEPS_RESOURCES)
void CvMinorCivAI::DoAcquire(PlayerTypes eMajor, int &iNumUnits, int& iCapitalX, int& iCapitalY, bool bVenice)
#else
void CvMinorCivAI::DoAcquire(PlayerTypes eMajor, int &iNumUnits, int& iCapitalX, int& iCapitalY)
#endif
{
	// Take their units
	CvUnit* pLoopUnit = NULL;
	int iLoopUnit;
	iNumUnits = 0;
	for(pLoopUnit = GetPlayer()->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = GetPlayer()->nextUnit(&iLoopUnit))
	{
		GET_PLAYER(eMajor).DoDistanceGift(GetPlayer()->GetID(), pLoopUnit);
		iNumUnits++;
	}

	// Take all their cities, don't show notifications
	SetDisableNotifications(true);
	iCapitalX = -1;
	iCapitalY = -1;
	FStaticVector<CvCity*, 16, false, c_eCiv5GameplayDLL, 0> vpCitiesToAcquire;
	int iLoopCity;
	for (CvCity* pLoopCity = GetPlayer()->firstCity(&iLoopCity, true); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iLoopCity, true))
	{
		vpCitiesToAcquire.push_back(pLoopCity);
	}
	for (uint iI = 0; iI < vpCitiesToAcquire.size(); iI++)
	{
		CvCity* pCity = vpCitiesToAcquire[iI];
		CvAssertMsg(pCity, "pCity should not be NULL. Please send Anton your save file and version.");
		if (pCity)
		{
			if (pCity->isCapital())
			{
				iCapitalX = pCity->getX();
				iCapitalY = pCity->getY();
			}
#if defined(MOD_GLOBAL_VENICE_KEEPS_RESOURCES)
			GET_PLAYER(eMajor).acquireCity(pCity, false, true, bVenice); // deletes pCity, don't reuse the pointer
#else
			GET_PLAYER(eMajor).acquireCity(pCity, false, true); // deletes pCity, don't reuse the pointer
#endif
		}
	}
	SetDisableNotifications(false);

	SetMajorBoughtOutBy(eMajor);

	GC.getGame().DoUpdateDiploVictory();

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);

}

// ******************************
// ***** Bullying *****
// ******************************

int CvMinorCivAI::GetBullyGoldAmount(PlayerTypes /*eBullyPlayer*/)
{
	int iGold = GC.getMINOR_BULLY_GOLD();
	int iGoldGrowthFactor = 350; //antonjs: todo: XML

	// Add gold, more if later in game
	float fGameProgressFactor = ((float) GC.getGame().getElapsedGameTurns() / (float) GC.getGame().getEstimateEndTurn());
	CvAssertMsg(fGameProgressFactor >= 0.0f, "fGameProgressFactor is not expected to be negative! Please send Anton your save file and version.");
	if(fGameProgressFactor > 1.0f)
		fGameProgressFactor = 1.0f;

	iGold += (int)(fGameProgressFactor * iGoldGrowthFactor);

	// UA, SP Mods

	// Game Speed Mod
	iGold *= GC.getGame().getGameSpeedInfo().getGoldGiftMod(); //antonjs: consider: separate XML
	iGold /= 100;

	// Rounding
	int iVisibleDivisor = /*5*/ GC.getMINOR_CIV_GOLD_GIFT_VISIBLE_DIVISOR(); //antonjs: consider: separate XML
	iGold /= iVisibleDivisor;
	iGold *= iVisibleDivisor;

	return iGold;
}

// Calculates a basic score for whether the major can bully this minor based on many factors.
// Negative score if bully attempt is a failure, zero or positive if success.
// May be modified after return, if the task is easier or harder (ex. bully a worker vs. bully gold)
int CvMinorCivAI::CalculateBullyMetric(PlayerTypes eBullyPlayer, bool bForUnit, CvString* sTooltipSink)
{
	CvString sFactors = "";

	int iScore = 0;
	const int iFailScore = -300;

	CvAssertMsg(GetPlayer()->GetID() != eBullyPlayer, "Minor civ and bully civ not expected to have the same ID!");
	if(GetPlayer()->GetID() == eBullyPlayer)
		return iFailScore;

	CvAssertMsg(eBullyPlayer >= 0, "eBullyPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBullyPlayer < MAX_MAJOR_CIVS, "eBullyPlayer is expected to be within maximum bounds (invalid Index)");
	if(eBullyPlayer < 0 || eBullyPlayer >= MAX_MAJOR_CIVS)
		return iFailScore;

	// Can't bully the dead
	if(!GetPlayer()->isAlive())
		return iFailScore;

	// **************************
	// Global military power ranking of major
	//
	// +0 ~ +75
	// **************************
	CvWeightedVector<PlayerTypes, MAX_MAJOR_CIVS, true> veMilitaryRankings;
	PlayerTypes eMajorLoop;
	int iGlobalMilitaryScore = 0;
	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajorLoop = (PlayerTypes) iMajorLoop;
		if(GET_PLAYER(eMajorLoop).isAlive() && !GET_PLAYER(eMajorLoop).isMinorCiv())
		{
			veMilitaryRankings.push_back(eMajorLoop, GET_PLAYER(eMajorLoop).GetMilitaryMight()); // Don't recalculate within a turn, can cause inconsistency
		}
	}
	CvAssertMsg(veMilitaryRankings.size() > 0, "WeightedVector of military might rankings not expected to be size 0");
	veMilitaryRankings.SortItems();
	for(int iRanking = 0; iRanking < veMilitaryRankings.size(); iRanking++)
	{
		if(veMilitaryRankings.GetElement(iRanking) == eBullyPlayer)
		{
			float fRankRatio = (float)(veMilitaryRankings.size() - iRanking) / (float)(veMilitaryRankings.size());
			iGlobalMilitaryScore = (int)(fRankRatio * 75); // A score between 75*(1 / num majors alive) and 75, with the highest rank major getting 75
			iScore += iGlobalMilitaryScore;
			break;
		}
	}
	
	if (sTooltipSink)
	{
		Localization::String strPositiveFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_POSITIVE");
		strPositiveFactor << iGlobalMilitaryScore;
		strPositiveFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_GLOBAL_MILITARY";
		sFactors += strPositiveFactor.toUTF8();
	}

	// **************************
	// Local military power comparison
	//
	// +0 ~ +125
	// **************************
	int iComparisonRadius = std::max(GC.getMap().getGridWidth() / 10, 5);
	CvCity* pMinorCapital = GetPlayer()->getCapitalCity();
	if(pMinorCapital == NULL)
		return iFailScore;
	CvPlot* pMinorCapitalPlot = pMinorCapital->plot();
	if(pMinorCapitalPlot == NULL)
		return iFailScore;
	int iMinorCapitalX = pMinorCapitalPlot->getX();
	int iMinorCapitalY = pMinorCapitalPlot->getY();
	int iMinorLocalPower = 0;
	int iBullyLocalPower = 0;
	CvPlot* pLoopPlot;
	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;

	// Include the minor's city power
	iMinorLocalPower += pMinorCapital->GetPower();

	for(int iDX = -iComparisonRadius; iDX <= iComparisonRadius; iDX++)
	{
		for(int iDY = -iComparisonRadius; iDY <= iComparisonRadius; iDY++)
		{
			pLoopPlot = ::plotXYWithRangeCheck(iMinorCapitalX, iMinorCapitalY, iDX, iDY, iComparisonRadius);

			if(pLoopPlot != NULL)
			{
				// If there are Units here, loop through them
				if(pLoopPlot->getNumUnits() > 0)
				{
					pUnitNode = pLoopPlot->headUnitNode();

					while(pUnitNode != NULL)
					{
						pLoopUnit = ::getUnit(*pUnitNode);
						pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

						// Is a combat unit
						if(pLoopUnit && pLoopUnit->IsCombatUnit())
						{
							if(pLoopUnit->getOwner() == eBullyPlayer)
							{
								iBullyLocalPower += pLoopUnit->GetPower();
							}
							else if(pLoopUnit->getOwner() == GetPlayer()->GetID())
							{
								iMinorLocalPower += pLoopUnit->GetPower();
							}
						}
					}
				}

			}
		}
	}
	float fLocalPowerRatio = (float)iBullyLocalPower / (float)iMinorLocalPower;
	int iLocalPowerScore = 0;
	if(fLocalPowerRatio >= 3.0)
	{
		iLocalPowerScore += 125;
	}
	else if(fLocalPowerRatio >= 2.0)
	{
		iLocalPowerScore += 100;
	}
	else if(fLocalPowerRatio >= 1.5)
	{
		iLocalPowerScore += 75;
	}
	else if(fLocalPowerRatio >= 1.0)
	{
		iLocalPowerScore += 50;
	}
	else if(fLocalPowerRatio >= 0.5)
	{
		iLocalPowerScore += 25;
	}
	iScore += iLocalPowerScore;
	
	if (sTooltipSink)
	{
		Localization::String strPositiveFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_POSITIVE");
		strPositiveFactor << iLocalPowerScore;
		strPositiveFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_MILITARY_PRESENCE";
		sFactors += strPositiveFactor.toUTF8();
	}

	// **************************
	// Social Policies
	//
	// Modifier to positive scores
	// **************************
	int iPoliciesScore = 0;
	int iPoliciesMod = GET_PLAYER(eBullyPlayer).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_MINOR_BULLY_SCORE_MODIFIER);
	if (iPoliciesMod != 0)
	{
		iPoliciesScore += iGlobalMilitaryScore;
		iPoliciesScore += iLocalPowerScore;

		iPoliciesScore *= iPoliciesMod;
		iPoliciesScore /= 100;
	}
	if (sTooltipSink && iPoliciesScore != 0)
	{
		Localization::String strPositiveFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_POSITIVE");
		strPositiveFactor << iPoliciesScore;
		strPositiveFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_GUNBOAT_DIPLOMACY";
		sFactors += strPositiveFactor.toUTF8();
	}
	iScore += iPoliciesScore;

	// **************************
	// Base Reluctance
	//
	// -110
	// **************************
	const int iBaseReluctanceScore = -110;
	
	if (sTooltipSink)
	{
		Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
		strNegativeFactor << iBaseReluctanceScore;
		strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_BASE_RELUCTANCE";
		sFactors += strNegativeFactor.toUTF8();
	}

	iScore += iBaseReluctanceScore;

	// **************************
	// Current influence of major
	//
	// -999 ~ -0
	// **************************
	if(GetEffectiveFriendshipWithMajor(eBullyPlayer) < GC.getFRIENDSHIP_THRESHOLD_CAN_BULLY())
	{
		int iInfluenceScore = iFailScore;
		iScore += iInfluenceScore;
		if (sTooltipSink)
		{
			Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
			strNegativeFactor << iInfluenceScore;
			strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_LOW_INFLUENCE";
			sFactors += strNegativeFactor.toUTF8();
		}
	}

	// **************************
	// Previous bully attempts
	//
	// -300 ~ -0
	// **************************
	int iLastBullyTurn = GetTurnLastBulliedByMajor(eBullyPlayer);
	if(iLastBullyTurn >= 0)
	{
		if(iLastBullyTurn + 10 >= GC.getGame().getGameTurn())
		{
			int iBulliedVeryRecentlyScore = iFailScore;
			iScore += iBulliedVeryRecentlyScore;
			if (sTooltipSink)
			{
				Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
				strNegativeFactor << iBulliedVeryRecentlyScore;
				strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_BULLIED_VERY_RECENTLY";
				sFactors += strNegativeFactor.toUTF8();
			}
		}
		else if(iLastBullyTurn + 20 >= GC.getGame().getGameTurn())
		{
			int iBulliedRecentlyScore = -40;
			iScore += iBulliedRecentlyScore;
			if (sTooltipSink)
			{
				Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
				strNegativeFactor << iBulliedRecentlyScore;
				strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_BULLIED_RECENTLY";
				sFactors += strNegativeFactor.toUTF8();
			}
		}
	}

	// **************************
	// Tribute type
	//
	// -30 ~ -0
	// **************************
	if (bForUnit)
	{
		int iUnitScore = -30;
		iScore += iUnitScore;
		if (sTooltipSink)
		{
			Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
			strNegativeFactor << iUnitScore;
			strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_UNIT_RELUCTANCE";
			sFactors += strNegativeFactor.toUTF8();
		}
	}

	// **************************
	// City-State population
	//
	// -300 ~ -0
	// **************************
	if (bForUnit)
	{
		if (GetPlayer()->getCapitalCity() == NULL || GetPlayer()->getCapitalCity()->getPopulation() < 4)
		{
			int iPopulationScore = iFailScore;
			iScore += iPopulationScore;
			if (sTooltipSink)
			{
				Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
				strNegativeFactor << iPopulationScore;
				strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_LOW_POPULATION";
				sFactors += strNegativeFactor.toUTF8();
			}
		}
	}

	// **************************
	// Passive Support from other majors
	//
	// -10 ~ -0
	// **************************
	if(GetAlly() != NO_PLAYER && GetAlly() != eBullyPlayer)
	{
		int iAllyScore = -10;
		iScore += iAllyScore;
		if (sTooltipSink)
		{
			Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
			strNegativeFactor << iAllyScore;
			strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_ALLIES";
			sFactors += strNegativeFactor.toUTF8();
		}
	}

	// **************************
	// Pledges of Protection from other majors
	//
	// -20 ~ -0
	// **************************
	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		int iProtectionScore = 0;
		eMajorLoop = (PlayerTypes) iMajorLoop;
		if(eMajorLoop != eBullyPlayer && IsProtectedByMajor(eMajorLoop))
		{
			iProtectionScore += -20;
			iScore += iProtectionScore;
			if (sTooltipSink)
			{
				Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
				strNegativeFactor << iProtectionScore;
				strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_PLEDGES_TO_PROTECT";
				sFactors += strNegativeFactor.toUTF8();
			}
			break;
		}
	}

	// **************************
	// Minor Civ Type
	//
	// -20 ~ -0
	// **************************
	if(GetPersonality() == MINOR_CIV_PERSONALITY_HOSTILE)
	{
		int iHostileScore = -10;
		iScore += iHostileScore;
		if (sTooltipSink)
		{
			Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
			strNegativeFactor << iHostileScore;
			strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_HOSTILE";
			sFactors += strNegativeFactor.toUTF8();
		}
	}
	if(GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
	{
		int iMilitaristicScore = -10;
		iScore += iMilitaristicScore;
		if (sTooltipSink)
		{
			Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
			strNegativeFactor << iMilitaristicScore;
			strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_MILITARISTIC";
			sFactors += strNegativeFactor.toUTF8();
		}
	}

	if (sTooltipSink != NULL)
	{
		(*sTooltipSink) += sFactors;
	}

	return iScore;
}

bool CvMinorCivAI::CanMajorBullyGold(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	// Can't bully us if we're dead!
	if(!GetPlayer()->isAlive())
		return false;

	int iScore = CalculateBullyMetric(ePlayer, /*bForUnit*/false);
	return CanMajorBullyGold(ePlayer, iScore);
}

// In case client wants to specify a metric beforehand (ie. they calculated it on their end, for logging purposes etc.)
bool CvMinorCivAI::CanMajorBullyGold(PlayerTypes ePlayer, int iSpecifiedBullyMetric)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	// Can't bully us if we're dead!
	if(!GetPlayer()->isAlive())
		return false;

	return (iSpecifiedBullyMetric >= 0);
}

CvString CvMinorCivAI::GetMajorBullyGoldDetails(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return "";
	
	CvString sFactors = "";
	int iScore = CalculateBullyMetric(ePlayer, /*bForUnit*/false, &sFactors);
	bool bCanBully = CanMajorBullyGold(ePlayer, iScore);

	Localization::String sFear = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_AFRAID");
	if (!bCanBully)
	{
		sFear = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_RESILIENT");
	}
	sFear << iScore;

	Localization::String sResult = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_GOLD_TT");
	sResult << sFear.toUTF8() << sFactors;

	return sResult.toUTF8();
}

bool CvMinorCivAI::CanMajorBullyUnit(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	// Can't bully us if we're dead!
	if(!GetPlayer()->isAlive())
		return false;

	int iScore = CalculateBullyMetric(ePlayer, /*bForUnit*/true);
	return CanMajorBullyUnit(ePlayer, iScore);
}

// In case client wants to specify a metric beforehand (ie. they calculated it on their end, for logging purposes etc.)
bool CvMinorCivAI::CanMajorBullyUnit(PlayerTypes ePlayer, int iSpecifiedBullyMetric)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	// Can't bully us if we're dead!
	if(!GetPlayer()->isAlive())
		return false;

	return (iSpecifiedBullyMetric >= 0);
}

CvString CvMinorCivAI::GetMajorBullyUnitDetails(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return "";

	CvString sFactors = "";
	int iScore = CalculateBullyMetric(ePlayer, /*bForUnit*/true, &sFactors);
	bool bCanBully = CanMajorBullyUnit(ePlayer, iScore);
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
	UnitTypes eUnitType = GET_PLAYER(ePlayer).GetSpecificUnitType("UNITCLASS_WORKER");
#else
	UnitTypes eUnitType = (UnitTypes) GC.getInfoTypeForString("UNIT_WORKER"); //antonjs: todo: XML/function
#endif
	CvUnitEntry* pUnitInfo = GC.getUnitInfo(eUnitType);
	CvAssert(pUnitInfo);
	if (!pUnitInfo)
		return "";

	Localization::String sFear = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_AFRAID");
	if (!bCanBully)
	{
		sFear = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_RESILIENT");
	}
	sFear << iScore;

	Localization::String sResult = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_UNIT_TT");
	sResult << sFear.toUTF8() << sFactors << pUnitInfo->GetDescriptionKey();

	return sResult.toUTF8();
}

void CvMinorCivAI::DoMajorBullyGold(PlayerTypes eBully, int iGold)
{
	CvAssertMsg(eBully >= 0, "eBully is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBully < MAX_MAJOR_CIVS, "eBully is expected to be within maximum bounds (invalid Index)");
	if(eBully < 0 || eBully >= MAX_MAJOR_CIVS) return;

	int iBullyMetric = CalculateBullyMetric(eBully, /*bForUnit*/false);
	bool bSuccess = CanMajorBullyGold(eBully, iBullyMetric);
	int iOldFriendshipTimes100 = GetEffectiveFriendshipWithMajorTimes100(eBully);

	if(bSuccess)
	{
		CvAssertMsg(iGold >= 0, "iGold is expected to be non-negative. Please send Anton your save file and version.");
	
		if(GC.getGame().getActivePlayer() == eBully)
		{
			int iBullyGold = 0;
			gDLL->GetSteamStat(ESTEAMSTAT_BULLIEDGOLD, &iBullyGold);

			iBullyGold += iGold;

			gDLL->SetSteamStat(ESTEAMSTAT_BULLIEDGOLD, iBullyGold);
		}

		GET_PLAYER(eBully).GetTreasury()->ChangeGold(iGold);
		DoBulliedByMajorReaction(eBully, GC.getMINOR_FRIENDSHIP_DROP_BULLY_GOLD_SUCCESS());
	}

	// Logging
	GET_PLAYER(eBully).GetDiplomacyAI()->LogMinorCivBullyGold(GetPlayer()->GetID(), iOldFriendshipTimes100, GetEffectiveFriendshipWithMajorTimes100(eBully), iGold, bSuccess, iBullyMetric);

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}

void CvMinorCivAI::DoMajorBullyUnit(PlayerTypes eBully, UnitTypes eUnitType)
{
	CvAssertMsg(eBully >= 0, "eBully is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBully < MAX_MAJOR_CIVS, "eBully is expected to be within maximum bounds (invalid Index)");
	if(eBully < 0 || eBully >= MAX_MAJOR_CIVS) return;

	int iBullyMetric = CalculateBullyMetric(eBully, /*bForUnit*/true);
	bool bSuccess = CanMajorBullyUnit(eBully, iBullyMetric);
	int iOldFriendshipTimes100 = GetEffectiveFriendshipWithMajorTimes100(eBully);

	if(bSuccess)
	{
		if(eUnitType == NO_UNIT)
		{
			CvAssertMsg(false, "eUnitType is not expected to be NO_UNIT. Please send Anton your save file and version.");
			return;
		}

		// Minor must have Capital
		CvCity* pCapital = GetPlayer()->getCapitalCity();
		if(pCapital == NULL)
		{
			CvAssertMsg(false, "Trying to spawn a Unit for a major civ but the minor has no capital. Please send Anton your save file and version.");
			return;
		}

		int iX = pCapital->getX();
		int iY = pCapital->getY();

		CvUnit* pNewUnit = GET_PLAYER(eBully).initUnit(eUnitType, iX, iY);
		if (pNewUnit->jumpToNearestValidPlot())
		{
			pNewUnit->finishMoves(); // The given unit cannot move this turn

			if(GetPlayer()->getCapitalCity())
				GetPlayer()->getCapitalCity()->addProductionExperience(pNewUnit);

			DoBulliedByMajorReaction(eBully, GC.getMINOR_FRIENDSHIP_DROP_BULLY_WORKER_SUCCESS());
		}
		else
			pNewUnit->kill(false);	// Could not find a spot for the unit!
	}

	// Logging
	GET_PLAYER(eBully).GetDiplomacyAI()->LogMinorCivBullyUnit(GetPlayer()->GetID(), iOldFriendshipTimes100, GetEffectiveFriendshipWithMajorTimes100(eBully), eUnitType, bSuccess, iBullyMetric);

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}

// We were just bullied, how do we react?
void CvMinorCivAI::DoBulliedByMajorReaction(PlayerTypes eBully, int iInfluenceChangeTimes100)
{
	CvAssertMsg(eBully >= 0, "eBully is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBully < MAX_MAJOR_CIVS, "eBully is expected to be within maximum bounds (invalid Index)");
	if(eBully < 0 || eBully >= MAX_MAJOR_CIVS) return;

	CvPlayer* pBully = &GET_PLAYER(eBully);
	CvAssertMsg(pBully, "pBully not expected to be NULL. Please send Anton your save file and version.");
	if (!pBully) return;

	SetTurnLastBulliedByMajor(eBully, GC.getGame().getGameTurn());
	ChangeFriendshipWithMajorTimes100(eBully, iInfluenceChangeTimes100);

	// In case we have quests that bullying makes obsolete, check now
	DoTestActiveQuests(/*bTestComplete*/ false, /*bTestObsolete*/ true);

	// Inform alive majors who have met the bully
	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		PlayerTypes eMajorLoop = (PlayerTypes) iMajorLoop;
		CvPlayer* pMajorLoop = &GET_PLAYER(eMajorLoop);
		if (!pMajorLoop) continue;

		if(pMajorLoop->isAlive())
		{
			if(GET_TEAM(pMajorLoop->getTeam()).isHasMet(pBully->getTeam()))
			{
				pMajorLoop->GetDiplomacyAI()->DoPlayerBulliedSomeone(eBully, GetPlayer()->GetID());
			}
		}
	}

	// Inform other alive minors, in case they had a quest that this fulfills
	for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		PlayerTypes eMinorLoop = (PlayerTypes) iMinorLoop;
		if (eMinorLoop == GetPlayer()->GetID()) continue;

		CvPlayer* pMinorLoop = &GET_PLAYER(eMinorLoop);
		if (!pMinorLoop) continue;

		if (pMinorLoop->isAlive() && pMinorLoop->GetMinorCivAI()->IsHasMetPlayer(eBully))
		{
			pMinorLoop->GetMinorCivAI()->DoTestActiveQuestsForPlayer(eBully, /*bTestComplete*/ true, /*bTestObsolete*/ false, MINOR_CIV_QUEST_BULLY_CITY_STATE);
		}
	}
}

bool CvMinorCivAI::IsEverBulliedByAnyMajor() const
{
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		if(IsEverBulliedByMajor((PlayerTypes)iPlayerLoop))
			return true;
	return false;
}

bool CvMinorCivAI::IsEverBulliedByMajor(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	return (m_aiTurnLastBullied[ePlayer] >= 0); // -1 means never bullied
}

bool CvMinorCivAI::IsRecentlyBulliedByAnyMajor() const
{
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		if(IsRecentlyBulliedByMajor((PlayerTypes)iPlayerLoop))
			return true;
	return false;
}

// Checks whether this minor has been bullied by a major recently
/* "Recently" is defined to be an interval of turns during which the
* major and the minor consider the bully action to be a dominant part
* of their diplomatic relationship.  For example, a major will likely not
* give a gift of gold to a recently bullied minor.  A minor will likely not
* ask for assistance from a major that recently bullied them.
*
* This is not intended to influence the success of a bully attempt, but rather
* other situations that arise from a bully attempt.
*/
bool CvMinorCivAI::IsRecentlyBulliedByMajor(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	const int iRecentlyBulliedTurnInterval = 20; //antonjs: todo: constant/XML
	return (m_aiTurnLastBullied[ePlayer] >= 0 && m_aiTurnLastBullied[ePlayer] >= (GC.getGame().getGameTurn() - iRecentlyBulliedTurnInterval)); // -1 means never bullied
}

int CvMinorCivAI::GetTurnLastBulliedByMajor(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1;

	return m_aiTurnLastBullied[ePlayer];
}

void CvMinorCivAI::SetTurnLastBulliedByMajor(PlayerTypes ePlayer, int iTurn)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_aiTurnLastBullied[ePlayer] = iTurn;
}

// ****************
// *** Election ***
// ****************
void CvMinorCivAI::DoElection()
{
	// if it is not an election turn, don't process!
	if(GC.getGame().GetTurnsUntilMinorCivElection() != 0)
	{
		return;
	}

	CvWeightedVector<PlayerTypes, MAX_MAJOR_CIVS, true> wvVotes;
	Firaxis::Array<CvEspionageSpy*, MAX_MAJOR_CIVS> apSpy;
	CvCity* pCapital = GetPlayer()->getCapitalCity();
	if(!pCapital)
	{
		return;
	}

	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		PlayerTypes eEspionagePlayer = (PlayerTypes)ui;
		CvPlayerEspionage* pPlayerEspionage = GET_PLAYER(eEspionagePlayer).GetEspionage();
		int iVotes = 0;
		int iLoop;
		apSpy[ui] = NULL;

		if (!GET_PLAYER(eEspionagePlayer).isAlive())
		{
			continue;
		}

		for(CvCity* pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
		{
			CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
			int iSpyID = pCityEspionage->m_aiSpyAssignment[eEspionagePlayer];
			// if no spies are assigned here, continue
			if(iSpyID == -1)
			{
				continue;
			}

			// on election day, evaluate spy to be reassigned
			pPlayerEspionage->m_aSpyList[iSpyID].m_bEvaluateReassignment = true;

			// if the spy assigned here is not rigging the election yet, continue
			if(pPlayerEspionage->m_aSpyList[iSpyID].m_eSpyState != SPY_STATE_RIG_ELECTION)
			{
				continue;
			}

			apSpy[ui] = &(pPlayerEspionage->m_aSpyList[iSpyID]);

			iVotes += (pCityEspionage->m_aiAmount[eEspionagePlayer] * (100 + m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_RIGGING_ELECTION_MODIFIER))) / 100;

			// now that votes are counted, remove the progress from the spy
			pCityEspionage->ResetProgress(eEspionagePlayer);

			int iRate = pPlayerEspionage->CalcPerTurn(SPY_STATE_RIG_ELECTION, pCity, iSpyID);
			int iGoal = pPlayerEspionage->CalcRequired(SPY_STATE_RIG_ELECTION, pCity, iSpyID);
			pCityEspionage->SetActivity(eEspionagePlayer, 0, iRate, iGoal);
			pCityEspionage->SetLastProgress(eEspionagePlayer, iRate);
		}

		if(iVotes > 0)
		{
			wvVotes.push_back(eEspionagePlayer, iVotes);
		}
	}

	if(wvVotes.size() > 0)
	{
		RandomNumberDelegate fcn;
		fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
		PlayerTypes eElectionWinner = wvVotes.ChooseByWeight(&fcn, "Choosing CS election winner by weight");

		for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
		{
			PlayerTypes ePlayer = (PlayerTypes)ui;

			if(ePlayer == eElectionWinner)
			{
				CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
				if(pNotifications)
				{
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_SUCCESS_S");
					strSummary << pCapital->getNameKey();
					Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_SUCCESS");
					strNotification << GET_PLAYER(ePlayer).GetEspionage()->GetSpyRankName(apSpy[ui]->m_eRank);
#if defined(MOD_BUGFIX_SPY_NAMES)
					strNotification << apSpy[ui]->GetSpyName(&GET_PLAYER(ePlayer));
#else
					strNotification << GET_PLAYER(ePlayer).getCivilizationInfo().getSpyNames(apSpy[ui]->m_iName);
#endif
					strNotification << pCapital->getNameKey();
					pNotifications->Add(NOTIFICATION_SPY_RIG_ELECTION_SUCCESS, strNotification.toUTF8(), strSummary.toUTF8(), pCapital->getX(), pCapital->getY(), -1);
				}

				ChangeFriendshipWithMajor(ePlayer, GC.getESPIONAGE_INFLUENCE_GAINED_FOR_RIGGED_ELECTION(), false);

#if !defined(NO_ACHIEVEMENTS)
				//Achievements!
				if(ePlayer == GC.getGame().getActivePlayer())
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_XP1_14);
				}
#endif
			}
			else
			{
				int iFriendship = GetEffectiveFriendshipWithMajor(ePlayer);
				int iRelationshipAnchor = GetFriendshipAnchorWithMajor(ePlayer);
				bool bFriends = IsFriends(ePlayer);
				bool bMet = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(m_pPlayer->getTeam());

				// if they have a spy in the city
				if(apSpy[ui] != NULL)
				{
					CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
					if(pNotifications)
					{
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_FAILURE_S");
						strSummary << pCapital->getNameKey();
						Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_FAILURE");
						strNotification << GET_PLAYER(ePlayer).GetEspionage()->GetSpyRankName(apSpy[ui]->m_eRank);
#if defined(MOD_BUGFIX_SPY_NAMES)
						strNotification << apSpy[ui]->GetSpyName(&GET_PLAYER(ePlayer));
#else
						strNotification << GET_PLAYER(ePlayer).getCivilizationInfo().getSpyNames(apSpy[ui]->m_iName);
#endif
						strNotification << pCapital->getNameKey();
						strNotification << GET_PLAYER(eElectionWinner).getCivilizationShortDescriptionKey();
						pNotifications->Add(NOTIFICATION_SPY_RIG_ELECTION_FAILURE, strNotification.toUTF8(), strSummary.toUTF8(), pCapital->getX(), pCapital->getY(), -1);
					}
				}
				else if (bMet && (bFriends || iFriendship > iRelationshipAnchor))
				{
					// no spy in the city, so just give them an alert that scenanigans are going on
					CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
					if(pNotifications)
					{
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_ALERT_S");
						strSummary << pCapital->getNameKey();
						Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_ALERT");
						strNotification << pCapital->getNameKey();
						pNotifications->Add(NOTIFICATION_SPY_RIG_ELECTION_ALERT, strNotification.toUTF8(), strSummary.toUTF8(), pCapital->getX(), pCapital->getY(), -1);
					}
				}

				if (GetEffectiveFriendshipWithMajorTimes100(ePlayer) > 0)
				{
					int iDiminishAmount = min(GC.getESPIONAGE_INFLUENCE_LOST_FOR_RIGGED_ELECTION() * 100, GetEffectiveFriendshipWithMajorTimes100(ePlayer));
					ChangeFriendshipWithMajorTimes100(ePlayer, -iDiminishAmount, false);
				}
			}
		}
	}

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
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

void CvMinorCivAI::DoUnitGiftFromMajor(PlayerTypes eFromPlayer, CvUnit* pGiftUnit, bool bDistanceGift)
{
	CvAssertMsg(eFromPlayer >= 0, "eFromPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eFromPlayer < MAX_MAJOR_CIVS, "eFromPlayer is expected to be within maximum bounds (invalid Index)");
	if (eFromPlayer < 0 || eFromPlayer >= MAX_MAJOR_CIVS) return;

	CvAssertMsg(pGiftUnit != NULL, "pGiftUnit is NULL");
	if (pGiftUnit == NULL) return;

	ChangeNumUnitsGifted(eFromPlayer, 1);

	// Influence
	int iInfluence = GetFriendshipFromUnitGift(eFromPlayer, pGiftUnit->IsGreatPerson(), bDistanceGift);
	ChangeFriendshipWithMajor(eFromPlayer, iInfluence);

	// We can't keep Great Person units
	if(pGiftUnit->IsGreatPerson())
	{
		pGiftUnit->kill(false);
	}
}

int CvMinorCivAI::GetFriendshipFromUnitGift(PlayerTypes eFromPlayer, bool bGreatPerson, bool /*bDistanceGift*/)
{
	CvAssertMsg(eFromPlayer >= 0, "eFromPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eFromPlayer < MAX_MAJOR_CIVS, "eFromPlayer is expected to be within maximum bounds (invalid Index)");
	if (eFromPlayer < 0 || eFromPlayer >= MAX_MAJOR_CIVS) return 0;

	int iInfluence = 0;
	CvPlayer& kFromPlayer = GET_PLAYER(eFromPlayer);

	// Influence
	if (bGreatPerson)
	{
		int iGPInfluence = kFromPlayer.GetPlayerTraits()->GetGreatPersonGiftInfluence();
		if (iGPInfluence > 0)
		{
			iInfluence += iGPInfluence;
		}
	}
	else
	{
		iInfluence += GC.getFRIENDSHIP_PER_UNIT_GIFTED();

		// War state
		if (IsProxyWarActiveForMajor(eFromPlayer))
		{
			iInfluence += GC.getFRIENDSHIP_PER_UNIT_GIFTED();
		}
		
		// Policies
		int iMilitaryInfluence = kFromPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_MILITARY_UNIT_GIFT_INFLUENCE);
		if (iMilitaryInfluence > 0)
		{
			iInfluence += iMilitaryInfluence;
		}
	}

	return iInfluence;
}

// How much gold has ePlayer gifted this minor?
int CvMinorCivAI::GetNumGoldGifted(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0;  // as defined in Reset()
	return m_aiNumGoldGifted[ePlayer];
}

void CvMinorCivAI::SetNumGoldGifted(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;
	m_aiNumGoldGifted[ePlayer] = iValue;
}

void CvMinorCivAI::ChangeNumGoldGifted(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;
	SetNumGoldGifted(ePlayer, GetNumGoldGifted(ePlayer) + iChange);
}


/// Major Civ gifted some Gold to this Minor
void CvMinorCivAI::DoGoldGiftFromMajor(PlayerTypes ePlayer, int iGold)
{
	if(GET_PLAYER(ePlayer).GetTreasury()->GetGold() >= iGold)
	{
		int iFriendshipChange = GetFriendshipFromGoldGift(ePlayer, iGold);
		if(iFriendshipChange > 0)
			GET_PLAYER(ePlayer).GetTreasury()->LogExpenditure(GetPlayer()->GetMinorCivAI()->GetNamesListAsString(0), iGold,4);

		GET_PLAYER(ePlayer).GetTreasury()->ChangeGold(-iGold);
		
		ChangeNumGoldGifted(ePlayer, iGold);
		
		ChangeFriendshipWithMajor(ePlayer, iFriendshipChange);

		// In case we had a Gold Gift quest active, complete it now
		DoTestActiveQuestsForPlayer(ePlayer, /*bTestComplete*/ true, /*bTestObsolete*/ false, MINOR_CIV_QUEST_GIVE_GOLD);
	}

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}

/// How many friendship points gained from a gift of Gold
int CvMinorCivAI::GetFriendshipFromGoldGift(PlayerTypes eMajor, int iGold)
{
	// The more Gold you spend the more Friendship you get!
	iGold = (int) pow((double) iGold, (double) /*1.01*/ GC.getGOLD_GIFT_FRIENDSHIP_EXPONENT());
	// The higher this divisor the less Friendship is gained
	int iFriendship = int(iGold / /*9.8*/ GC.getGOLD_GIFT_FRIENDSHIP_DIVISOR());

	// Game progress factor based on how far into the game we are
	double fGameProgressFactor = float(GC.getGame().getElapsedGameTurns()) / GC.getGame().getEstimateEndTurn();
	fGameProgressFactor = min(fGameProgressFactor, 1.0); // Don't count above 1.0, otherwise it will end up negative!
	
	// Tweak factor slightly, otherwise Gold will do literally NOTHING once we reach the end of the game!
	fGameProgressFactor *= /*2*/ GC.getMINOR_CIV_GOLD_GIFT_GAME_MULTIPLIER();
	fGameProgressFactor /= /*3*/ GC.getMINOR_CIV_GOLD_GIFT_GAME_DIVISOR();
	fGameProgressFactor = 1 - fGameProgressFactor;

	iFriendship = (int)(iFriendship * fGameProgressFactor);


	// Mod (Policies, etc.)
	int iFriendshipMod = GET_PLAYER(eMajor).getMinorGoldFriendshipMod();
	iFriendshipMod += GET_PLAYER(eMajor).GetReligions()->GetCityStateInfluenceModifier();
	if(iFriendshipMod != 0)
	{
		iFriendship *= (100 + iFriendshipMod);
		iFriendship /= 100;
	}

	// Game Speed Mod
	iFriendship *= GC.getGame().getGameSpeedInfo().getGoldGiftMod();
	iFriendship /= 100;

	//antonjs: todo: refine
	if(IsActiveQuestForPlayer(eMajor, MINOR_CIV_QUEST_INVEST))
	{
		int iBoostPercentage = 50; //antonjs: todo: xml
		iFriendship *= 100 + iBoostPercentage;
		iFriendship /= 100;
	}

	// Friendship gained should always be positive
	iFriendship = max(iFriendship, /*5*/ GC.getMINOR_CIV_GOLD_GIFT_MINIMUM_FRIENDSHIP_REWARD());

	// Round the number so it's pretty
	int iVisibleDivisor = /*5*/ GC.getMINOR_CIV_GOLD_GIFT_VISIBLE_DIVISOR();
	iFriendship /= iVisibleDivisor;
	iFriendship *= iVisibleDivisor;

	return iFriendship;
}

// Can this major gift us a tile improvement?
bool CvMinorCivAI::CanMajorGiftTileImprovement(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;
	CvPlayer* pPlayer = &GET_PLAYER(eMajor);
	if(pPlayer == NULL)
	{
		CvAssertMsg(false, "pPlayer not expected to be NULL. Please send Anton your save file and version.");
		return false;
	}

	// Must have enough gold
	const int iCost = GetGiftTileImprovementCost(eMajor);
	if(pPlayer->GetTreasury()->GetGold() < iCost)
		return false;

	// Must own an improveable plot
	bool bHasValidPlot = false;
	CvPlotsVector& aiPlots = GetPlayer()->GetPlots();
	for(uint ui = 0; ui < aiPlots.size(); ui++)
	{
		// at the end of the plot list
		if(aiPlots[ui] == -1)
		{
			break;
		}

		CvPlot* pPlot = GC.getMap().plotByIndex(aiPlots[ui]);
		if (IsLackingGiftableTileImprovementAtPlot(eMajor, pPlot->getX(), pPlot->getY()))
		{
			bHasValidPlot = true;
			break;
		}
	}

	if (!bHasValidPlot)
		return false;

	return true;
}

// Does this plot lack a resource tile improvement that eMajor has access to?
bool CvMinorCivAI::IsLackingGiftableTileImprovementAtPlot(PlayerTypes eMajor, int iPlotX, int iPlotY)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;
	CvPlot* pPlot = GC.getMap().plot(iPlotX, iPlotY);
	if(pPlot == NULL)
	{
		CvAssertMsg(false, "pPlot not expected to be NULL, invalid coordinates. Please send Anton your save file and version.");
		return false;
	}

	// Only allowed to improve plots that the minor actually owns
	if(pPlot->getOwner() != GetPlayer()->GetID())
		return false;

	// Only allowed to improve Luxury and Strategic resources
	ResourceTypes eResource = pPlot->getResourceType();
	if(eResource == NO_RESOURCE)
	{
		return false;
	}
	else
	{
		ResourceUsageTypes eUsage = GC.getResourceInfo(eResource)->getResourceUsage();
		if(eUsage != RESOURCEUSAGE_STRATEGIC && eUsage != RESOURCEUSAGE_LUXURY)
		{
			return false;
		}
	}

	ImprovementTypes eImprovement = pPlot->getImprovementTypeNeededToImproveResource(eMajor, false /*bTestOwner*/);

	// There must be a valid improvement for the player to build
	if(eImprovement == NO_IMPROVEMENT)
		return false;

	return true;
}

// Convenience wrapper function
bool CvMinorCivAI::CanMajorGiftTileImprovementAtPlot(PlayerTypes eMajor, int iPlotX, int iPlotY)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;

	return (CanMajorGiftTileImprovement(eMajor) && IsLackingGiftableTileImprovementAtPlot(eMajor, iPlotX, iPlotY));
}

int CvMinorCivAI::GetGiftTileImprovementCost(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return -1;

	return GC.getMINOR_CIV_TILE_IMPROVEMENT_GIFT_COST();
}

void CvMinorCivAI::DoTileImprovementGiftFromMajor(PlayerTypes eMajor, int iPlotX, int iPlotY)
{
	if(!CanMajorGiftTileImprovementAtPlot(eMajor, iPlotX, iPlotY))
	{
		return;
	}
	CvPlot* pPlot = GC.getMap().plot(iPlotX, iPlotY);
	if(pPlot == NULL)
	{
		CvAssertMsg(false, "pPlot not expected to be NULL, invalid coordinates. Please send Anton your save file and version.");
		return;
	}
	CvPlayer* pPlayer = &GET_PLAYER(eMajor);
	if(pPlayer == NULL)
	{
		CvAssertMsg(false, "pPlayer not expected to be NULL. Please send Anton your save file and version.");
		return;
	}

	ImprovementTypes eImprovement = pPlot->getImprovementTypeNeededToImproveResource(eMajor, /*bTestOwner*/false);
	if(eImprovement == NO_IMPROVEMENT)
	{
		return;
	}

	pPlot->setImprovementType(eImprovement, eMajor);
#if defined(MOD_BUGFIX_MINOR)
	// Clear the pillage state on this plot (eg Minor builds a farm, barbs pillage it,
	// minor discovers iron on the plot, player pays to build a mine, but the plot is still pillaged!)
#if defined(MOD_EVENTS_TILE_IMPROVEMENTS)
	pPlot->SetImprovementPillaged(false, false);
#else
	pPlot->SetImprovementPillaged(false);
#endif
#endif

	// VFX
	auto_ptr<ICvPlot1> pDllPlot(new CvDllPlot(pPlot));
	gDLL->GameplayDoFX(pDllPlot.get());

	const int iCost = GetGiftTileImprovementCost(eMajor);
	GET_PLAYER(eMajor).GetTreasury()->LogExpenditure(GetPlayer()->GetMinorCivAI()->GetNamesListAsString(0), iCost,5);
	pPlayer->GetTreasury()->ChangeGold(-iCost);
}

/// Now at war with eTeam
void CvMinorCivAI::DoNowAtWarWithTeam(TeamTypes eTeam)
{
	int iOldFriendship;
	int iWarFriendship = /*-60*/ GC.getMINOR_FRIENDSHIP_AT_WAR();

	PlayerTypes ePlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(GET_PLAYER(ePlayer).getTeam() == eTeam)
		{
			// Friendship change
			iOldFriendship = GetBaseFriendshipWithMajor(ePlayer);
			DoFriendshipChangeEffects(ePlayer, iOldFriendship, iWarFriendship);

			// Revoke PtP is there was one
			if(IsProtectedByMajor(ePlayer))
			{
				DoChangeProtectionFromMajor(ePlayer, false);
			}

			// Revoke quests if there were any
			if(GetNumActiveQuestsForPlayer(ePlayer) > 0)
			{
				EndAllActiveQuestsForPlayer(ePlayer);
			}

			// Nullify Quests - Deprecated?
			//GET_PLAYER((PlayerTypes) iMinorCivLoop).GetMinorCivAI()->SetPeaceQuestCompletedByMajor((PlayerTypes) iMajorCivLoop, true);
			//GET_PLAYER((PlayerTypes) iMinorCivLoop).GetMinorCivAI()->SetWarQuestCompletedByMajor((PlayerTypes) iMajorCivLoop, true);

			//// Is this player declaring war also already a Bully?  If so, he's gonna regret it
			//GET_PLAYER((PlayerTypes) iMinorCivLoop).GetMinorCivAI()->DoBullyDeclareWar((PlayerTypes) iMajorCivLoop);
		}
		else if (!IsAtWarWithPlayersTeam(ePlayer) && GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(eTeam))
		{
			// If ePlayer is also at war with eTeam, we might shorten the unit spawn timer
			if (IsFriends(ePlayer) && GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
			{
				int iBaseSpawnTurns = GetSpawnBaseTurns(ePlayer); // May be significantly less now with common enemy due to social policy
				iBaseSpawnTurns = MAX(iBaseSpawnTurns, 1);
				if (iBaseSpawnTurns < GetUnitSpawnCounter(ePlayer))
				{
					SetUnitSpawnCounter(ePlayer, iBaseSpawnTurns);
				}
			}
		}
	}

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}

/// Now at peace with eTeam
void CvMinorCivAI::DoNowPeaceWithTeam(TeamTypes eTeam)
{
	int iWarFriendship = /*-60*/ GC.getMINOR_FRIENDSHIP_AT_WAR();
	int iNewFriendship;

	PlayerTypes ePlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(GET_PLAYER(ePlayer).getTeam() == eTeam)
		{
			// Friendship change
			iNewFriendship = GetBaseFriendshipWithMajor(ePlayer);
			DoFriendshipChangeEffects(ePlayer, iWarFriendship, iNewFriendship);
		}
	}

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}

/// Will this AI allow peace with ePlayer?
bool CvMinorCivAI::IsPeaceBlocked(TeamTypes eTeam) const
{
	// Permanent war?
	if(IsPermanentWar(eTeam))
		return true;

#if defined(MOD_BALANCE_CORE_MINORS)
	if (MOD_BALANCE_CORE_MINORS) 
	{
		if(IsJerk(eTeam))
		{
			return true;
		}
	}
#endif

	// Allies with someone at war with this guy?
	PlayerTypes eMajor;
	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;

		// Major must be alive
		if(!GET_PLAYER(eMajor).isAlive())
			continue;

		// Must be allies
		if(!IsAllies(eMajor))
			continue;

		// Ally must be at war with this team
		if(!GET_TEAM(GET_PLAYER(eMajor).getTeam()).isAtWar(eTeam))
			continue;

		return true;
	}

	return false;
}

/// eTeam declared war on us
void CvMinorCivAI::DoTeamDeclaredWarOnMe(TeamTypes eEnemyTeam)
{
	CvTeam* pEnemyTeam = &GET_TEAM(eEnemyTeam);
	CivsList veMinorsNowWary;
	int iRand;

	// Since eEnemyTeam was the aggressor, drop the base influence to the minimum
	for(int iEnemyMajorLoop = 0; iEnemyMajorLoop < MAX_MAJOR_CIVS; iEnemyMajorLoop++)
	{
		PlayerTypes eEnemyMajorLoop = (PlayerTypes) iEnemyMajorLoop;
		if(!GET_PLAYER(eEnemyMajorLoop).isAlive())
			continue;
		if(GET_PLAYER(eEnemyMajorLoop).getTeam() != eEnemyTeam)
			continue;		

		//antonjs: consider: forcibly revoke PtP here instead, and have negative INF / broken PtP fallout
		
		SetFriendshipWithMajor(eEnemyMajorLoop, GC.getMINOR_FRIENDSHIP_AT_WAR());
	}

	//antonjs: todo: xml, rename xml to indicate it is for WaryOf, not Permanent War
	// Minor Civ Warmonger
	if(pEnemyTeam->IsMinorCivWarmonger())
	{
		if(!IsWaryOfTeam(eEnemyTeam))
		{
			SetWaryOfTeam(eEnemyTeam, true);
			veMinorsNowWary.push_back(GetPlayer()->GetID());
		}
		if(ENABLE_PERMANENT_WAR)
			SetPermanentWar(eEnemyTeam, true);
	}
	// Minor Civ Aggressor - chance of permanent war
	else if(pEnemyTeam->IsMinorCivAggressor())
	{
		iRand = GC.getGame().getJonRandNum(100, "MINOR CIV AI: Become Wary Of aggressor roll.");

		if(iRand < /*50*/ GC.getPERMANENT_WAR_AGGRESSOR_CHANCE())
		{
			if(!IsWaryOfTeam(eEnemyTeam))
			{
				SetWaryOfTeam(eEnemyTeam, true);
				veMinorsNowWary.push_back(GetPlayer()->GetID());
			}
			if(ENABLE_PERMANENT_WAR)
				SetPermanentWar(eEnemyTeam, true);
		}
	}

	// See if other minors will declare war
	if(pEnemyTeam->IsMinorCivAggressor())
	{
		int iChance;

		PlayerProximityTypes eProximity;

		int iAttackingMajorPlayer;
		PlayerTypes eAttackingMajorPlayer;
		bool bAttackerIsAlly;

		CvPlayer* pOtherMinorCiv;
		PlayerTypes eOtherMinorCiv;
		for(int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
		{
			eOtherMinorCiv = (PlayerTypes) iMinorCivLoop;
			pOtherMinorCiv = &GET_PLAYER((eOtherMinorCiv));

			iChance = 0;

			// Must be alive
			if(!pOtherMinorCiv->isAlive())
				continue;

			// Must be a different minor
			if(eOtherMinorCiv == GetPlayer()->GetID())
				continue;

			// Ignore minors that want THIS minor dead
			if(pOtherMinorCiv->GetMinorCivAI()->IsWantsMinorDead(GetPlayer()->GetID()))
				continue;

			bAttackerIsAlly = false;

			// Ignore minors that are allied to the attacker
			for(iAttackingMajorPlayer = 0; iAttackingMajorPlayer < MAX_MAJOR_CIVS; iAttackingMajorPlayer++)
			{
				eAttackingMajorPlayer = (PlayerTypes) iAttackingMajorPlayer;

				// Not on this team
				if(GET_PLAYER(eAttackingMajorPlayer).getTeam() != eEnemyTeam)
					continue;

				// Not alive
				if(!GET_PLAYER(eAttackingMajorPlayer).isAlive())
					continue;

				if(pOtherMinorCiv->GetMinorCivAI()->GetAlly() == eAttackingMajorPlayer)
				{
					bAttackerIsAlly = true;
					break;
				}
			}

			if(bAttackerIsAlly)
				continue;

			// Closer to the minor the more likely war is
			eProximity = pOtherMinorCiv->GetProximityToPlayer(GetPlayer()->GetID());

			// Warmonger
			if(pEnemyTeam->IsMinorCivWarmonger())
			{
				if(eProximity == PLAYER_PROXIMITY_DISTANT)
					iChance += /*25*/ GC.getPERMANENT_WAR_OTHER_WARMONGER_CHANCE_DISTANT();
				else if(eProximity == PLAYER_PROXIMITY_FAR)
					iChance += /*50*/ GC.getPERMANENT_WAR_OTHER_WARMONGER_CHANCE_FAR();
				else if(eProximity == PLAYER_PROXIMITY_CLOSE)
					iChance += /*75*/ GC.getPERMANENT_WAR_OTHER_WARMONGER_CHANCE_CLOSE();
				else if(eProximity == PLAYER_PROXIMITY_NEIGHBORS)
					iChance += /*100*/ GC.getPERMANENT_WAR_OTHER_WARMONGER_CHANCE_NEIGHBORS();
			}
			// Aggressor
			else
			{
				if(eProximity == PLAYER_PROXIMITY_DISTANT)
					iChance += /*0*/ GC.getPERMANENT_WAR_OTHER_CHANCE_DISTANT();
				else if(eProximity == PLAYER_PROXIMITY_FAR)
					iChance += /*0*/ GC.getPERMANENT_WAR_OTHER_CHANCE_FAR();
				else if(eProximity == PLAYER_PROXIMITY_CLOSE)
					iChance += /*20*/ GC.getPERMANENT_WAR_OTHER_CHANCE_CLOSE();
				else if(eProximity == PLAYER_PROXIMITY_NEIGHBORS)
					iChance += /*50*/ GC.getPERMANENT_WAR_OTHER_CHANCE_NEIGHBORS();
			}

			// If the minor is already at war, then there's a chance of it turning into permanent war
			if(GET_TEAM(pOtherMinorCiv->getTeam()).isAtWar(eEnemyTeam))
				iChance += /*50*/ GC.getPERMANENT_WAR_OTHER_AT_WAR();

			iRand = GC.getGame().getJonRandNum(100, "MINOR CIV AI: Third party minor to become Wary Of aggressor roll.");
			if(iRand < iChance)
			{
				if(!pOtherMinorCiv->GetMinorCivAI()->IsWaryOfTeam(eEnemyTeam))
				{
					pOtherMinorCiv->GetMinorCivAI()->SetWaryOfTeam(eEnemyTeam, true);
					veMinorsNowWary.push_back(eOtherMinorCiv);
				}
				if(ENABLE_PERMANENT_WAR)
					pOtherMinorCiv->GetMinorCivAI()->SetPermanentWar(eEnemyTeam, true);
			}
		}
	}

	if(veMinorsNowWary.size() > 0)
	{
		int iMinimumFriendshipMod = 20; //antonjs: todo: xml
		Localization::String strTemp;

		strTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_MINORS_NOW_WARY");
		CvString strSummary = strTemp.toUTF8();
		strTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_MINORS_NOW_WARY_TT");
		strTemp << iMinimumFriendshipMod;
		CvString strMessage = strTemp.toUTF8();

		for(int iEnemyMajorLoop = 0; iEnemyMajorLoop < MAX_MAJOR_CIVS; iEnemyMajorLoop++)
		{
			PlayerTypes eEnemyMajorLoop = (PlayerTypes) iEnemyMajorLoop;
			if(!GET_PLAYER(eEnemyMajorLoop).isAlive())
				continue;
			if(GET_PLAYER(eEnemyMajorLoop).getTeam() != eEnemyTeam)
				continue;

			strMessage = strMessage + GetNamesListAsString(veMinorsNowWary);
			AddNotification(strMessage, strSummary, eEnemyMajorLoop);
		}
	}
#if defined(MOD_BALANCE_CORE_MINORS)
	if(MOD_BALANCE_CORE_MINORS) 
	{
		SetIsJerk(eEnemyTeam, true);
		SetJerk(eEnemyTeam, /*30*/ GC.getBALANCE_CS_WAR_COOLDOWN_RATE());
	}
#endif

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}

/// Permanent War with eTeam?
bool CvMinorCivAI::IsPermanentWar(TeamTypes eTeam) const
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	if(eTeam < 0 || eTeam >= MAX_TEAMS) return false;

	return m_abPermanentWar[eTeam];
}

/// Permanent War with eTeam?
void CvMinorCivAI::SetPermanentWar(TeamTypes eTeam, bool bValue)
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	if(eTeam < 0 || eTeam >= MAX_TEAMS) return;

	if(ENABLE_PERMANENT_WAR)
		m_abPermanentWar[eTeam] = bValue;
}

// Wary of eTeam?
bool CvMinorCivAI::IsWaryOfTeam(TeamTypes eTeam) const
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	if(eTeam < 0 || eTeam >= MAX_TEAMS) return false;

	return m_abWaryOfTeam[eTeam];
}

// Wary of eTeam?
void CvMinorCivAI::SetWaryOfTeam(TeamTypes eTeam, bool bValue)
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	if(eTeam < 0 || eTeam >= MAX_TEAMS) return;

	if(IsWaryOfTeam(eTeam) == bValue)
		return;

	m_abWaryOfTeam[eTeam] = bValue;
}
#if defined(MOD_BALANCE_CORE_MINORS)
//Did you attack me FOR NO REASON? JERK!
bool CvMinorCivAI::IsJerk(TeamTypes eTeam) const
{
	CvAssertMsg(eTeam >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < REALLY_MAX_TEAMS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(eTeam < 0 || eTeam >= REALLY_MAX_TEAMS) return 0;  // as defined in Reset()
	return m_abIsJerk[eTeam];
}

void CvMinorCivAI::SetIsJerk(TeamTypes eTeam, bool bValue)
{
	CvAssertMsg(eTeam >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < REALLY_MAX_TEAMS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(m_abIsJerk[eTeam] != bValue)
	{
		m_abIsJerk[eTeam] = bValue;
	}
}

//JERK COOLDOWN RATE

void CvMinorCivAI::ChangeJerk(TeamTypes eTeam, int iChange)
{
	SetJerk(eTeam, GetJerk(eTeam) + iChange);
}
int CvMinorCivAI::GetJerk(TeamTypes eTeam) const
{
	CvAssertMsg(eTeam >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_CIV_TEAMS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(eTeam < 0 || eTeam >= MAX_CIV_TEAMS) return 0;  // as defined in Reset()
	return m_aiJerk[eTeam];
}
void CvMinorCivAI::SetJerk(TeamTypes eTeam, int iValue)
{
	if(GetJerk(eTeam) != iValue)
		m_aiJerk[eTeam] = iValue;
}
#endif

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
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		eResource = (ResourceTypes) iResourceLoop;

		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		// Must not be a Bonus resource
		if(pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_BONUS)
			continue;

		// We must have it
		if(GetPlayer()->getNumResourceTotal(eResource, /*bIncludeImport*/ false) == 0)
			continue;

		// They must not have it
		if(GET_PLAYER(eMajor).getNumResourceTotal(eResource, /*bIncludeImport*/ false) > 0)
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

	for(int iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
	{
		const TechTypes eTech = static_cast<TechTypes>(iTechLoop);
		CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
		if(pkTechInfo == NULL)
			continue;

		// Player doesn't already have Tech
		if(!kTeam.GetTeamTechs()->HasTech(eTech))
		{
			// Player can research this Tech
			if(kPlayer.GetPlayerTechs()->CanResearch(eTech))
			{
				iValue = pkTechInfo->GetResearchCost();

				// Reduce value of a Tech if it's already in progress
				iProgress = kTeam.GetTeamTechs()->GetResearchProgress(eTech);

				if(iProgress > 0)
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
	if(TechVector.size() == 1)
	{
		return (TechTypes) TechVector.GetElement(0);
	}
	else if(TechVector.size() == 0)
	{
		return NO_TECH;
	}

	TechVector.SortItems();

	// Our rough estimate is that 20 is a good ceiling for the max Tech value
	if(iRoughTechValue > 20)
	{
		iRoughTechValue = 20;
	}

	int iIndex = (TechVector.size() - 1) * iRoughTechValue / 20;

	return (TechTypes) TechVector.GetElement(iIndex);
}

/// Checks to see if the majority religion of the city-state is the religion that this major has founded
bool CvMinorCivAI::IsSameReligionAsMajor(PlayerTypes eMajor)
{
	CvPlayer* pkPlayer = GetPlayer();
	if(pkPlayer)
	{
		CvCity* pkCity = pkPlayer->getCapitalCity();
		if(pkCity)
		{
			ReligionTypes eMinorReligion = pkCity->GetCityReligions()->GetReligiousMajority();
			ReligionTypes eMajorReligion = GC.getGame().GetGameReligions()->GetReligionCreatedByPlayer(eMajor);

			if(eMinorReligion != NO_RELIGION &&
			        eMajorReligion != NO_RELIGION &&
			        eMinorReligion == eMajorReligion)
			{
				return true;
			}
		}
	}
	return false;
}

CvString CvMinorCivAI::GetStatusChangeDetails(PlayerTypes ePlayer, bool bAdd, bool bFriends, bool bAllies)
{
	Localization::String strDetailedInfo;

	MinorCivTraitTypes eTrait = GetTrait();

	if(eTrait == MINOR_CIV_TRAIT_CULTURED)
	{
		int iCultureBonusAmount = 0;
		if (bFriends)
		{
			iCultureBonusAmount += GetCultureFlatFriendshipBonus(ePlayer) + GetCulturePerBuildingFriendshipBonus(ePlayer);
		}
		if (bAllies)
		{
			iCultureBonusAmount += GetCultureFlatAlliesBonus(ePlayer) + GetCulturePerBuildingAlliesBonus(ePlayer);
		}
		if (!bAdd)
		{
			iCultureBonusAmount = -iCultureBonusAmount;
		}

		if(bAllies && bAdd)		// Now Allies (includes jump from nothing through Friends to Allies)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_CULTURE");
			strDetailedInfo << iCultureBonusAmount;
		}
		else if(bFriends && bAdd)		// Now Friends
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_CULTURE");
			strDetailedInfo << iCultureBonusAmount;
		}
		else if(!bAdd)		// Bonus diminished (or removed)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LOST_CULTURE");
			strDetailedInfo << iCultureBonusAmount;
		}
	}
	else if(eTrait == MINOR_CIV_TRAIT_MILITARISTIC)
	{
		if(bAllies && bAdd)		// Now Allies (includes jump from nothing through Friends to Allies)
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_MILITARISTIC");
		else if(bFriends && bAdd)		// Now Friends
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_MILITARISTIC");
		else if(bFriends && !bAdd)		// No longer Friends (includes drop from Allies down to nothing) - this should be before the Allies check!
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LOST_FRIENDS_MILITARISTIC");
		else if(bAllies && !bAdd)		// No longer Allies
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LOST_ALLIES_MILITARISTIC");
	}
	else if(eTrait == MINOR_CIV_TRAIT_MARITIME)
	{
		int iCapitalFoodTimes100 = 0;
		int iOtherCitiesFoodTimes100 = 0;

		if(bFriends)	// Friends bonus
		{
			iCapitalFoodTimes100 += GetFriendsCapitalFoodBonus(ePlayer);
			iOtherCitiesFoodTimes100 += GetFriendsOtherCityFoodBonus(ePlayer);
		}
		if(bAllies)		// Allies bonus
		{
			iCapitalFoodTimes100 += GetAlliesCapitalFoodBonus(ePlayer);
			iOtherCitiesFoodTimes100 += GetAlliesOtherCityFoodBonus(ePlayer);
		}

		if(!bAdd)		// Flip amount of we're taking bonuses away
		{
			iCapitalFoodTimes100 = -iCapitalFoodTimes100;
			iOtherCitiesFoodTimes100 = -iOtherCitiesFoodTimes100;
		}

		// Now that we've changed the gameplay, add together the two so the DISPLAY looks right
		iCapitalFoodTimes100 += iOtherCitiesFoodTimes100;
		float fCapitalFood = float(iCapitalFoodTimes100) / 100;
		float fOtherCitiesFood = float(iOtherCitiesFoodTimes100) / 100;
		//iCapitalFood += iOtherCitiesFood;

		if(bAllies && bAdd)		// Now Allies (includes jump from nothing through Friends to Allies)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_MARITIME");
			strDetailedInfo << fCapitalFood << fOtherCitiesFood;
		}
		else if(bFriends && bAdd)		// Now Friends
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_MARITIME");
			strDetailedInfo << fCapitalFood << fOtherCitiesFood;
		}
		else if(!bAdd)		// Bonus diminished (or removed)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LOST_MARITIME");
			strDetailedInfo << fCapitalFood << fOtherCitiesFood;
		}
	}
	else if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
	{
		int iHappinessBonus = 0;

		if(bFriends)	// Friends bonus
		{
			iHappinessBonus += GetHappinessFlatFriendshipBonus(ePlayer) + GetHappinessPerLuxuryFriendshipBonus(ePlayer);
		}
		if(bAllies)		// Allies bonus
		{
			iHappinessBonus += GetHappinessFlatAlliesBonus(ePlayer) + GetHappinessPerLuxuryAlliesBonus(ePlayer);
		}
		if(!bAdd)		// Flip amount of we're taking bonuses away
		{
			iHappinessBonus = -iHappinessBonus;
		}

		if(bAllies && bAdd)		// Now Allies (includes jump from nothing through Friends to Allies)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_MERCANTILE");
			strDetailedInfo << iHappinessBonus;
		}
		else if(bFriends && bAdd)		// Now Friends
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_MERCANTILE");
			strDetailedInfo << iHappinessBonus;
		}
		else if(!bAdd)		// Bonus diminished (or removed)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LOST_MERCANTILE");
			strDetailedInfo << iHappinessBonus;
		}
	}

	// Religious
	if(eTrait == MINOR_CIV_TRAIT_RELIGIOUS)
	{
		int iFaithBonusAmount = 0;
		if (bFriends)
		{
			iFaithBonusAmount += GetFaithFlatFriendshipBonus(ePlayer);
		}
		if (bAllies)
		{
			iFaithBonusAmount += GetFaithFlatAlliesBonus(ePlayer);
		}
		if (!bAdd)
		{
			iFaithBonusAmount = -iFaithBonusAmount;
		}

		if(bAllies && bAdd)		// Now Allies (includes jump from nothing through Friends to Allies)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_RELIGIOUS");
			strDetailedInfo << iFaithBonusAmount;
		}
		else if(bFriends && bAdd)		// Now Friends
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_RELIGIOUS");
			strDetailedInfo << iFaithBonusAmount;
		}
		else if(!bAdd)		// Bonus diminished (or removed)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LOST_RELIGIOUS");
			strDetailedInfo << iFaithBonusAmount;
		}
	}

	return strDetailedInfo.toUTF8();
}

pair<CvString, CvString> CvMinorCivAI::GetStatusChangeNotificationStrings(PlayerTypes ePlayer, bool bAdd, bool bFriends, bool bAllies, PlayerTypes eOldAlly, PlayerTypes eNewAlly)
{
	Localization::String strMessage;
	Localization::String strSummary;

	CvTeam* pTeam = &GET_TEAM(GET_PLAYER(ePlayer).getTeam());
	CvAssertMsg(pTeam, "pTeam not expected to be NULL. Please send Anton your save file and version.");

	const char* strMinorsNameKey = GetPlayer()->getNameKey();

	// Adding/Increasing bonus
	if(bAdd)
	{
		// Jumped up to Allies (either from Neutral or from Friends, or passing another player)
		if(bAllies)
		{
			// BASE ALLIES MESSAGE

			// No previous Ally (or it was us)
			if(eOldAlly == NO_PLAYER || eOldAlly == ePlayer)
			{
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_BASE");
			}
			// We're passing someone
			else
			{
				CvAssertMsg(eOldAlly != NO_PLAYER, "eOldAlly not expected to be NO_PLAYER here. Please send Anton your save file and version.");
				const char* strOldBestPlayersNameKey = "TXT_KEY_UNMET_PLAYER";
				TeamTypes eOldAllyTeam = GET_PLAYER(eOldAlly).getTeam();
				if(pTeam->isHasMet(eOldAllyTeam))
					strOldBestPlayersNameKey = GET_PLAYER(eOldAlly).getCivilizationShortDescriptionKey();

				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_BASE_PASSED");
				strMessage << strOldBestPlayersNameKey;
			}

			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_ALLIES_STATUS");

			// Build Resource info
			int iNumResourceTypes = 0;
			FStaticVector<ResourceTypes, 64, true, c_eCiv5GameplayDLL, 0> veResources;
			ResourceTypes eResource;
			ResourceUsageTypes eUsage;
			int iResourceQuantity;
			for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				eResource = (ResourceTypes) iResourceLoop;
				iResourceQuantity = GetPlayer()->getNumResourceTotal(eResource);

				if(iResourceQuantity > 0)
				{
					const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);					
					if (pkResourceInfo != NULL)
					{
						eUsage = GC.getResourceInfo(eResource)->getResourceUsage();

						if(eUsage == RESOURCEUSAGE_STRATEGIC || eUsage == RESOURCEUSAGE_LUXURY)
						{
							veResources.push_back(eResource);
							iNumResourceTypes++;
						}
					}
				}
			}
			// APPEND RESOURCE INFO
			Localization::String strResourceDetails;
			if(iNumResourceTypes == 0)
			{
				strResourceDetails = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_GAINED_BEST_RELATIONS_BONUS_NONE");
				strResourceDetails << strMinorsNameKey;
			}
			else
			{
				CvString strResourceNames = GC.getResourceInfo(veResources[0])->GetDescription();
				int i = 1;
				while(i < iNumResourceTypes)
				{
					strResourceNames += ", ";
					strResourceNames += GC.getResourceInfo(veResources[i++])->GetDescription();
				}

				strResourceDetails = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_GAINED_BEST_RELATIONS_BONUS_SOME");
				strResourceDetails << strResourceNames.c_str();
			}
			strMessage << strResourceDetails.toUTF8();
		}
		// Went from Neutral to Friends
		else if(bFriends)
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_BASE");
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_FRIENDS_STATUS");
		}
	}
	// Removing/Reducing bonus
	else
	{
		// Dropped from Allies
		if(bAllies)
		{
			// Normal friendship decay
			if(eNewAlly == NO_PLAYER)
			{
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_ALLIES_LOST");
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_ALLIES_STATUS_LOST");
			}
			// Someone passed us up
			else
			{
				CvAssertMsg(eNewAlly != NO_PLAYER, "eNewAlly not expected to be NO_PLAYER here. Please send Anton your save file and version.");
				CvAssertMsg(eNewAlly != ePlayer, "eNewAlly not expected to be same as ePlayer here. Please send Anton your save file and version.");
				const char* strNewBestPlayersNameKey = "TXT_KEY_UNMET_PLAYER";
				TeamTypes eNewAllyTeam = GET_PLAYER(eNewAlly).getTeam();
				if(pTeam->isHasMet(eNewAllyTeam))
					strNewBestPlayersNameKey = GET_PLAYER(eNewAlly).getCivilizationShortDescriptionKey();

				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_ALLIES_PASSED");
				strMessage << strNewBestPlayersNameKey;
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_ALLIES_STATUS_PASSED");
				strSummary << strNewBestPlayersNameKey;
			}
		}
		// Dropped down to Neutral from Friends (case of Allies down to Neutral not handled well... let's hope it doesn't happen often!)
		else if(bFriends)
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_FRIENDS_LOST_BASE");
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_FRIENDS_STATUS_LOST");
		}
	}

	CvString strDetailedInfo = GetStatusChangeDetails(ePlayer, bAdd, bFriends, bAllies);
	strMessage << GetPlayer()->getNameKey() << strDetailedInfo;
	strSummary << GetPlayer()->getNameKey();

	pair<CvString, CvString> notifStrings = pair<CvString, CvString>(strMessage.toUTF8(), strSummary.toUTF8());
	return notifStrings;
}

// Return a string with the list of players' names, each on its own line (for notifications)
CvString CvMinorCivAI::GetNamesListAsString(CivsList veNames)
{
	CvString s = "";
	if (veNames.size() > 0)
	{
		for (uint ui = 0; ui < veNames.size(); ui++)
		{
			PlayerTypes ePlayerLoop = veNames[ui];
			CvPlayer* pPlayerLoop = &GET_PLAYER(ePlayerLoop);
			if (pPlayerLoop)
			{
				Localization::String sName = Localization::Lookup(pPlayerLoop->getCivilizationShortDescriptionKey());
				s = s + "[NEWLINE]" + sName.toUTF8();
			}
		}
	}
	return s;
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

/// Have notifications been (temporarily) disabled?
bool CvMinorCivAI::IsDisableNotifications() const
{
	return m_bDisableNotifications;
}

/// Disable notifications for minor civs for now
void CvMinorCivAI::SetDisableNotifications(bool bDisableNotifications)
{
	if (m_bDisableNotifications != bDisableNotifications)
	{
		m_bDisableNotifications = bDisableNotifications;
	}
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
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
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

FDataStream& operator<<(FDataStream& saveTo, const MinorCivStatusTypes& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, MinorCivStatusTypes& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<MinorCivStatusTypes>(v);
	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const MinorCivPersonalityTypes& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, MinorCivPersonalityTypes& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<MinorCivPersonalityTypes>(v);
	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const MinorCivQuestTypes& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, MinorCivQuestTypes& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<MinorCivQuestTypes>(v);
	return loadFrom;
}
