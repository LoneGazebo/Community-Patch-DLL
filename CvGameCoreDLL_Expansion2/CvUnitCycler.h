/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#ifndef CVUNITCYCLER_H
#define CVUNITCYCLER_H
#pragma once
#include "LinkedList.h"

class CvUnitCycler
{
public:

	CvUnitCycler();
	CvUnit*	Cycle(CvUnit* pUnit, bool bForward, bool bWorkers, bool* pbWrap);
};

#endif
