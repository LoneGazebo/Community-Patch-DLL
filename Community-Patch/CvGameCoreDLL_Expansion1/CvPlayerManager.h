/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

// CvPlayerManager.h

#ifndef CVPLAYERMANAGER_H
#define CVPLAYERMANAGER_H

// Class (mostly static) that handles operations on groups of players.
class CvPlayerManager
{
public:

	//	Refresh all danger plots for players that have dirty danger plot structures.
	static	void	RefreshDangerPlots();
};
#endif
