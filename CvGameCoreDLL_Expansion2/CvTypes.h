/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

//This utility caches commonly used type ID's from the database.
namespace CvTypes
{
void AcquireTypes(Database::Connection& db);

//MissionTypes
const MissionTypes getMISSION_MOVE_TO();
const MissionTypes getMISSION_ROUTE_TO();
const MissionTypes getMISSION_MOVE_TO_UNIT();
const MissionTypes getMISSION_SWAP_UNITS();
const MissionTypes getMISSION_SKIP();
const MissionTypes getMISSION_SLEEP();
const MissionTypes getMISSION_ALERT();
const MissionTypes getMISSION_FORTIFY();
const MissionTypes getMISSION_GARRISON();
const MissionTypes getMISSION_SET_UP_FOR_RANGED_ATTACK();
const MissionTypes getMISSION_EMBARK();
const MissionTypes getMISSION_DISEMBARK();
const MissionTypes getMISSION_AIRPATROL();
const MissionTypes getMISSION_HEAL();
const MissionTypes getMISSION_AIRLIFT();
const MissionTypes getMISSION_NUKE();
const MissionTypes getMISSION_PARADROP();
const MissionTypes getMISSION_AIR_SWEEP();
const MissionTypes getMISSION_REBASE();
const MissionTypes getMISSION_RANGE_ATTACK();
const MissionTypes getMISSION_PILLAGE();
const MissionTypes getMISSION_FOUND();
const MissionTypes getMISSION_JOIN();
const MissionTypes getMISSION_CONSTRUCT();
const MissionTypes getMISSION_DISCOVER();
const MissionTypes getMISSION_HURRY();
const MissionTypes getMISSION_TRADE();
const MissionTypes getMISSION_BUY_CITY_STATE();
const MissionTypes getMISSION_REPAIR_FLEET();
const MissionTypes getMISSION_SPACESHIP();
const MissionTypes getMISSION_CULTURE_BOMB();
const MissionTypes getMISSION_FOUND_RELIGION();
const MissionTypes getMISSION_GOLDEN_AGE();
const MissionTypes getMISSION_BUILD();
const MissionTypes getMISSION_LEAD();
const MissionTypes getMISSION_DIE_ANIMATION();
const MissionTypes getMISSION_BEGIN_COMBAT();
const MissionTypes getMISSION_END_COMBAT();
const MissionTypes getMISSION_AIRSTRIKE();
const MissionTypes getMISSION_SURRENDER();
const MissionTypes getMISSION_CAPTURED();
const MissionTypes getMISSION_IDLE();
const MissionTypes getMISSION_DIE();
const MissionTypes getMISSION_DAMAGE();
const MissionTypes getMISSION_MULTI_SELECT();
const MissionTypes getMISSION_MULTI_DESELECT();
const MissionTypes getMISSION_WAIT_FOR();
const MissionTypes getMISSION_SPREAD_RELIGION();
const MissionTypes getMISSION_ENHANCE_RELIGION();
const MissionTypes getMISSION_REMOVE_HERESY();
const MissionTypes getMISSION_ESTABLISH_TRADE_ROUTE();
const MissionTypes getMISSION_PLUNDER_TRADE_ROUTE();
const MissionTypes getMISSION_GREAT_WORK();
const MissionTypes getMISSION_CHANGE_TRADE_UNIT_HOME_CITY();
const MissionTypes getMISSION_SELL_EXOTIC_GOODS();
const MissionTypes getMISSION_GIVE_POLICIES();
const MissionTypes getMISSION_ONE_SHOT_TOURISM();
const MissionTypes getMISSION_CHANGE_ADMIRAL_PORT();
const unsigned int getNUM_MISSION_TYPES();

const GreatWorkArtifactClass getARTIFACT_ANCIENT_RUIN();
const GreatWorkArtifactClass getARTIFACT_BARBARIAN_CAMP();
const GreatWorkArtifactClass getARTIFACT_BATTLE_RANGED();
const GreatWorkArtifactClass getARTIFACT_BATTLE_MELEE(); 
const GreatWorkArtifactClass getARTIFACT_RAZED_CITY();
const GreatWorkArtifactClass getARTIFACT_WRITING();

const GreatWorkSlotType getGREAT_WORK_SLOT_ART_ARTIFACT();
const GreatWorkSlotType getGREAT_WORK_SLOT_LITERATURE();
const GreatWorkSlotType getGREAT_WORK_SLOT_MUSIC();

}