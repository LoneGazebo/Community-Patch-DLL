/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#pragma once

enum YieldTypes
{
    NO_YIELD = -1,

    YIELD_FOOD,
    YIELD_PRODUCTION,
    YIELD_GOLD,
    YIELD_SCIENCE,
    YIELD_CULTURE,
    YIELD_FAITH,

    NUM_YIELD_TYPES
};