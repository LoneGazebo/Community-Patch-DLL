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
void CvPlayerManager::Refresh(bool /*bWarDeclaration*/)
{
	//include the barbarians!
	for(int iPlayerCivLoop = 0; iPlayerCivLoop < MAX_PLAYERS; iPlayerCivLoop++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iPlayerCivLoop;

		CvPlayer& kPlayer = GET_PLAYER(ePlayer);
		// Must be alive
		if(!kPlayer.isAlive())
			continue;

#if defined(MOD_BALANCE_CORE)
		//this is called after a declaration of war and after loading a savegame
		kPlayer.invalidatePlotFoundValues();
		kPlayer.SetDangerPlotsDirty();
		kPlayer.UpdateAreaEffectUnits();
		kPlayer.UpdateFractionOriginalCapitalsUnderControl();
		kPlayer.UpdateCurrentAndFutureWars();
#else
		if(kPlayer.m_pDangerPlots && kPlayer.m_pDangerPlots->IsDirty())
			kPlayer.UpdateDangerPlots();
#endif
	}
}