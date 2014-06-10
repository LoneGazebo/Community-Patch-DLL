#ifndef CVGAMEQUERIES_H
#define CVGAMEQUERIES_H

#pragma once

/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvEnums.h"
/// A utility class that contains some useful but not often used queries about data at 
/// the game level.  That is, not for a specific player or unit.
class CvGameQueries
{
public:
	static UnitClassTypes	GetLeastAdvancedUnitClassNobodyHas(bool bUseRandom = false);
	static int				GetTeamClosenessScore(int** aaiDistances, int* aiStartingLocs);
	static bool				AreUnitsSameType(UnitTypes eFirstUnitType, UnitTypes eSecondUnitType);

};

#endif // CVGAMEQUERIES_H