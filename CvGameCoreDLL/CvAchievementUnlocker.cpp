/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//	CvAchievementUnlocker
//
//	Utility methods to test the parameters for individual achievements.
//	Most of the tests in this file are for achievements that have multiple parameters
//	that can be achieved in any order.  This keeps the code in a single place.
//	It is not strictly necessary to put all achievement tests in this file especially if the
//	test requires many parameters or is adequately tested in a single location in the game core.

#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreUtils.h"
#include "CvAchievementUnlocker.h"

#ifdef FINAL_RELEASE
 #define DEBUG_RELEASE_VALUE(a, b) (b)
#else
 #define DEBUG_RELEASE_VALUE(a, b) (a)
#endif

int CvAchievementUnlocker::ms_iNumImprovementsPillagedPerTurn = 0;
//	---------------------------------------------------------------------------
//	Test the conditions for the ACHIEVEMENT_PSG
bool CvAchievementUnlocker::Check_PSG()
{
	const int PSG_STAT_MATCH_VALUE = 100;

	int32 iGeneralsStat = 0;
	int32 iWinsStat = 0;
	int32 iArmyStat = 0;
	if ( gDLL->GetSteamStat( ESTEAMSTAT_GREATGENERALS, &iGeneralsStat ) &&
		 gDLL->GetSteamStat( ESTEAMSTAT_TOTAL_WINS, &iWinsStat ) &&
	     gDLL->GetSteamStat( ESTEAMSTAT_STANDINGARMY, &iArmyStat ) )
	{
		if ( iGeneralsStat >= PSG_STAT_MATCH_VALUE && iWinsStat >= PSG_STAT_MATCH_VALUE && iArmyStat >= PSG_STAT_MATCH_VALUE )
		{
			gDLL->UnlockAchievement( ACHIEVEMENT_PSG );
			return true;
		}
	}
	return false;
}
//------------------------------------------------------------------------------
void CvAchievementUnlocker::FarmImprovementPillaged()
{
	ms_iNumImprovementsPillagedPerTurn++;

	if(ms_iNumImprovementsPillagedPerTurn >= 9)
		gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_04_PILLAGE);
}
//------------------------------------------------------------------------------
void CvAchievementUnlocker::EndTurn()
{
	//Reset the per turn counters.
	ms_iNumImprovementsPillagedPerTurn = 0;
}

//------------------------------------------------------------------------------
void CvAchievementUnlocker::AlexanderConquest(PlayerTypes ePlayer)
{
	//Test For Alexander Conquest
	CvGame& kGame = GC.getGame();
	if (ePlayer == kGame.getActivePlayer())
	{
		CvString szLeaderName = (CvString)GET_PLAYER(ePlayer).getLeaderTypeKey();
		if(szLeaderName == "LEADER_ALEXANDER")
		{
			if(kGame.getGameTurnYear() <= 350)
			{
				for(int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
				{
					CvPlayer* pPlayer = &GET_PLAYER((PlayerTypes) iPlayerLoop);

					//All known players must be dead and killed by us
					if(GET_TEAM(pPlayer->getTeam()).isHasMet(GET_PLAYER(kGame.getActivePlayer()).getTeam()))
					{
						if(!pPlayer->isBarbarian() && !pPlayer->isMinorCiv())
						{
							if(pPlayer->isAlive() && pPlayer->GetID() != GET_PLAYER(kGame.getActivePlayer()).GetID())
							{
								return;	// Nope.
							}
						}
					}
				}
				// Yep.
				gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_CONQUEST_WORLD);
			}
		}
	}
}
