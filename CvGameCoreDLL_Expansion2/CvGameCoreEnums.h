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
#if defined(MOD_API_UNIFIED_YIELDS_TOURISM)
    YIELD_TOURISM,
#endif
#if defined(MOD_API_UNIFIED_YIELDS_GOLDEN_AGE)
    YIELD_GOLDEN_AGE_POINTS,
#endif
#if defined(MOD_BALANCE_CORE_YIELDS)
	YIELD_GREAT_GENERAL_POINTS,
	YIELD_GREAT_ADMIRAL_POINTS,
	YIELD_POPULATION,
	YIELD_CULTURE_LOCAL,
#endif
#if defined(MOD_BALANCE_CORE_JFD)
	YIELD_JFD_HEALTH,
	YIELD_JFD_DISEASE,
	YIELD_JFD_CRIME,
	YIELD_JFD_LOYALTY,
	YIELD_JFD_SOVEREIGNTY,
#endif

    NUM_YIELD_TYPES
};

// Popups specific to this DLL

// Hashed values.  Use FStringHashGen to create!
#define BUTTONPOPUP_LEAGUE_OVERVIEW						((ButtonPopupTypes)0x41D0F622)
#define BUTTONPOPUP_DECLAREWAR_PLUNDER_TRADE_ROUTE		((ButtonPopupTypes)0xFCB501AF)
#define BUTTONPOPUP_CHOOSE_ARCHAEOLOGY					((ButtonPopupTypes)0x0752FFBE)
#define BUTTONPOPUP_CHOOSE_IDEOLOGY						((ButtonPopupTypes)0xF604A676)
#define BUTTONPOPUP_CHOOSE_INTERNATIONAL_TRADE_ROUTE	((ButtonPopupTypes)0x920D58CC)
#define BUTTONPOPUP_CULTURE_OVERVIEW					((ButtonPopupTypes)0xE81563A9)
#define BUTTONPOPUP_CHOOSE_TRADE_UNIT_NEW_HOME			((ButtonPopupTypes)0x10BD8E0F)
#define BUTTONPOPUP_GREAT_WORK_COMPLETED_ACTIVE_PLAYER	((ButtonPopupTypes)0x3D7CCE59)
#define BUTTONPOPUP_TRADE_ROUTE_OVERVIEW				((ButtonPopupTypes)0x7D23885D)
#define BUTTONPOPUP_LEAGUE_PROJECT_COMPLETED			((ButtonPopupTypes)0xCAA4FBA2)
#define BUTTONPOPUP_CHOOSE_GOODY_HUT_REWARD				((ButtonPopupTypes)0xFF412A6F)
#define BUTTONPOPUP_LEAGUE_SPLASH						((ButtonPopupTypes)0xA53EBF16)
#define BUTTONPOPUP_CHOOSE_ADMIRAL_PORT					((ButtonPopupTypes)0x4D1B317C)
