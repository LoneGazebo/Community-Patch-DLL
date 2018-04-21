/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvGlobals.h"
#include "CvPlayer.h"
#include "CvPlayerManager.h"
#include "CvDangerPlots.h"
//	-----------------------------------------------------------------------------------------------
//	Loop through all the players and update cached values
void CvPlayerManager::Refresh(bool bWarStateChanged)
{
	//include the barbarians!
	for(int iPlayerCivLoop = 0; iPlayerCivLoop < MAX_PLAYERS; iPlayerCivLoop++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iPlayerCivLoop;
		CvPlayer& kPlayer = GET_PLAYER(ePlayer);
		// Must be alive
		if(!kPlayer.isAlive())
			continue;

		//this is called after a declaration of war and after loading a savegame
		kPlayer.invalidatePlotFoundValues();
		kPlayer.SetDangerPlotsDirty();
		kPlayer.UpdateAreaEffectUnits();
		kPlayer.UpdateAreaEffectPlots();
		kPlayer.UpdateMilitaryStats();
		kPlayer.UpdateCurrentAndFutureWars();

		//only after loading, force danger update (only known enemy units are serialized)
		if(!bWarStateChanged && kPlayer.m_pDangerPlots)
			kPlayer.UpdateDangerPlots(true);


		if (bWarStateChanged)
			kPlayer.GetTacticalAI()->GetTacticalAnalysisMap()->Refresh(true);

		if (bWarStateChanged)
		{
			// Despite comments to the contrary, I think the cached trade path info might depend on war state. I may have read the code wrong.
			GC.getGame().GetGameTrade()->InvalidateTradePathCache(iPlayerCivLoop);
			// I am not 100% confident that the cache is asked to be updated often enough and may be invalid when read.
			// However, during testing I was only calling update and therefore nothing much was probably happening, so all of this could just be a waste of time...or not.
			// Invalidating in theoretically the right thing to do BUT since I did my testing with this here I want to leave it for the time being.
			GC.getGame().GetGameTrade()->UpdateTradePathCache(iPlayerCivLoop);
		}
	}
}