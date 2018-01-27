// CustomMods.h
#pragma once

#ifndef CUSTOM_MODS_H
#define CUSTOM_MODS_H

// Custom #defines used by all DLLs
#include "..\CvGameCoreDLLUtil\include\CustomModsGlobal.h"

/****************************************************************************
 ****************************************************************************
 *****                                                                  *****
 *****                           IMPORTANT!!!                           *****
 *****                                                                  *****
 *****                        Modders take note!                        *****
 *****                                                                  *****
 *****  See the comment in CvDllVersion.h regarding the DLL GUID value  *****
 *****                                                                  *****
 *****                                                                  *****
 *****          This DLL is based on the 1.0.3.276 source code          *****
 *****                                                                  *****
 ****************************************************************************
 ****************************************************************************/
#define MOD_DLL_GUID {0xbf9bf7f0, 0xe078, 0x4d4e, { 0x8a, 0x3e, 0x84, 0x71, 0x2f, 0x85, 0xaa, 0x2b }} //{BF9BF7F0-E078-4d4e-8A3E-84712F85AA2B}
#define MOD_DLL_NAME "Community Patch v88 (PNM v51+)"
#define MOD_DLL_VERSION_NUMBER ((uint) 88)
#define MOD_DLL_VERSION_STATUS ""			// a (alpha), b (beta) or blank (released)
#define MOD_DLL_CUSTOM_BUILD_NAME ""

//////////////////////////
//MULTIPLAYER INSTRUCTIONS:
/////////////////////////////////////////

// FIRST: Comment out this line to include all the achievements code (which don't work in modded games, so we don't need the code!)
//#define NO_ACHIEVEMENTS
//adds active diplomacy to Multiplayer games
#define MOD_ACTIVE_DIPLOMACY			 gCustomMods.isACTIVE_DIPLOMACY()

/////////////////////////////////
//END MULTIPLAYER INSTRUCTIONS
////////////////////////////////////////

///////////////////////
// BATTLE ROYALE CODE
//////////////////////

//If you enable this, you can do an 'observer' mode human player (i.e. submarine in ice) to do the battle royale! Includes code for CSV export of data
//#define MOD_BATTLE_ROYALE

//If you enable this, the CS AI can settle more cities.
//#define MOD_MINOR_CAN_SETTLE

/// simpler algorithm for scoring exploration plots
#define MOD_CORE_ALTERNATIVE_EXPLORE_SCORE
/// use globally unique ids for cities, units etc
#define MOD_BALANCE_CORE_GLOBAL_IDS
/// ships on land tiles (city, fort) cannot attack
#define MOD_BALANCE_RANGED_ATTACK_ONLY_IN_NATIVE_DOMAIN

/// radical rewrite
#define MOD_BALANCE_CORE_NEW_TACTICAL_AI
#define MOD_CORE_NEW_DEPLOYMENT_LOGIC
//#define MOD_UNIT_KILL_STATS

/// visible tiles stay visible until the end of the turn
#define MOD_CORE_DELAYED_VISIBILITY

/// experimental performance improvement
#define MOD_CORE_CACHE_REACHABLE_PLOTS

/// unrevealed plots are impassable instead of passable by default
#define MOD_CORE_UNREVEALED_IMPASSABLE

/// for better multiplayer experience
#define MOD_CORE_REDUCE_RANDOMNESS

#define MOD_CORE_RESILIENT_PANTHEONS

///	air units take a flat amount of damage in each air strike (plus interceptions)
#define MOD_CORE_AIRCOMBAT_SIMPLIFIED

#define MOD_BALANCE_CORE_OBSERVER_CHANGES
	#define AUI_PLOT_OBSERVER_SEE_ALL_PLOTS
	#define AUI_GAME_OBSERVER_CAN_OPEN_CITIES
	#define AUI_PLOT_OBSERVER_NO_NW_POPUPS

/// Flavors that weren't previously fetched but were still (attempted to be) used in processing later are now fetched
#define AUI_HOMELAND_FIX_ESTABLISH_HOMELAND_PRIORITIES_MISSING_FLAVORS
/// Fixes the code that checks for cramped status (it always triggered originally, now it only triggers if we really are cramped)
#define AUI_GS_CONQUEST_FIX_CRAMPED
/// Priorities for sneak attack military units are no longer artificially inflated at the highest difficulty levels
#define AUI_CITYSTRATEGY_CHOOSE_PRODUCTION_NO_HIGH_DIFFICULTY_SKEW
/// The AI wants an expensive tech if it's selecting a free tech
#define AUI_TECHAI_CHOOSE_NEXT_TECH_FREE_TECH_WANTS_EXPENSIVE
/// Buildings that contribute towards getting an ideology act as a unique building for the purposes of tech scoring
#define AUI_PLAYERTECHS_RESET_IDEOLOGY_UNLOCKERS_COUNT_AS_UNIQUE

/// When calculating the expected damage on a target from a melee unit, the AI will now use pTargetPlot and pDefender parameters when appropriate (instead of having them as NULL)
#define AUI_TACTICAL_FIX_FIND_PARATROOPER_WITHIN_STRIKING_DISTANCE_MELEE_STRENGTH
/// When calculating the expected damage on a target from a melee unit, the AI will now use pFromPlot and pDefender parameters when appropriate (instead of having them as NULL)
#define AUI_TACTICAL_FIX_COMPUTE_EXPECTED_DAMAGE_MELEE
/// Air units will now properly factor in possible interceptions made at the tile
#define AUI_TACTICAL_FIX_COMPUTE_EXPECTED_DAMAGE_AIR_UNITS
/// Units that cannot caputre cities will not attempt to do so
#define AUI_TACTICAL_FIX_NO_CAPTURE
/// Civilians that are not moved with BarbarianCivilianEscortMove now move using passive barbarian move
#define AUI_TACTICAL_EXECUTE_BARBARIAN_MOVES_CIVILIANS_MOVE_PASSIVELY

/// If the international trade route would be to a minor, the gold and tech received by the minor do not count
#define AUI_TRADE_SCORE_INTERNATIONAL_MAX_DELTA_WITH_MINORS
/// Score for a trade route from religious pressure is now relative to how much pressure there already is at the city
#define AUI_TRADE_SCORE_INTERNATIONAL_RELATIVE_RELIGION_SCORING
/// Instead of simply doubling score if we want the tourism boost, the multiplier is based on our grand strategy
#define AUI_TRADE_SCORE_INTERNATIONAL_TOURISM_SCORE_USES_GRAND_STRATEGY
/// When prioritizing trade routes, the actual trade value of all three possible route types will be considered instead of prioritizing food > production > international
#define AUI_TRADE_UNBIASED_PRIORITIZE

/// Uses slightly modified algorithm for determining Diplomat Usefulness levels
#define AUI_VOTING_TWEAKED_DIPLOMAT_USEFULNESS
/// Uses a different algorithm for scoring voting on world ideology
#define AUI_VOTING_TWEAKED_WORLD_IDEOLOGY
/// Uses a different algorithm for scoring voting on world religion
#define AUI_VOTING_TWEAKED_WORLD_RELIGION

/// Checks all of player's cities for whether or not a city is within conversion target range, not just the player's capital
#define AUI_RELIGION_CONVERSION_TARGET_NOT_JUST_CAPITAL
/// Weighs different yield types differently depending on flavor and citizen value
#define AUI_RELIGION_SCORE_BELIEF_AT_PLOT_FLAVOR_YIELDS
/// When adding the terrain yield change of a belief, only do so if the current feature on the plot is additive (so eg. Dance with the Aurora won't be overvalued)
#define AUI_RELIGION_SCORE_BELIEF_AT_PLOT_SCORE_TERRAIN_CONSIDER_FEATURE
/// Weighs different yield types differently depending on flavor and citizen value
#define AUI_RELIGION_SCORE_BELIEF_AT_CITY_FLAVOR_YIELDS
/// Considers grand strategies when scoring things like beliefs that only function when at peace
#define AUI_RELIGION_SCORE_BELIEF_AT_CITY_CONSIDER_GRAND_STRATEGY
/// River happiness score will only be applied if the city being scored is actually on a river
#define AUI_RELIGION_FIX_SCORE_BELIEF_AT_CITY_RIVER_HAPPINESS
/// If a building for which yield improvement is being calculated is a wonder of any kind, divide the yield by the city count (so there's effective only one instance being scored in the civ)
#define AUI_RELIGION_SCORE_BELIEF_AT_CITY_YIELDS_FROM_WONDERS_COUNT_ONCE
/// When scoring a belief that unlocks faith purchases of units, disregard eras that have already passed
#define AUI_RELIGION_FIX_SCORE_BELIEF_FOR_PLAYER_UNLOCKS_UNITS_DISREGARD_OLD_ERAS
/// Divides a city's targetting score for missionaries by this value if passive pressure is enough to eventually convert the city
#define AUI_RELIGION_SCORE_CITY_FOR_MISSIONARY_DIVIDER_IF_PASSIVE_PRESSURE_ENOUGH (10)
/// When finding a nearby conversion target, cities that will convert to the AI's religion passively are ignored
#define AUI_RELIGION_HAVE_NEARBY_CONVERSION_TARGET_IGNORE_TARGET_THAT_WILL_CONVERT_PASSIVELY
/// Scales the non-spaceship scoring of Great Engineers with Wonder Competitiveness
#define AUI_RELIGION_GET_DESIRED_FAITH_GREAT_PERSON_ENGINEER_USES_WONDER_COMPETITIVENESS (1000.0 / 3.0)
/// Fixes the bug where the AI scores inquisitors if it already has enough, not when it needs them
#define AUI_RELIGION_FIX_GET_DESIRED_FAITH_GREAT_PERSON_INQUISITOR_CHECK
/// When comparing the final score for beliefs, the score of the lowest scored belief will be subtracted from all beliefs
#define AUI_RELIGION_RELATIVE_BELIEF_SCORE
/// Enhance score for prophet purchase if religion is not enhanced yet
#define AUI_RELIGION_FIX_DO_FAITH_PURCHASES_ENHANCE_RELIGION
/// Since Venice can purchase stuff at puppets, the function will no longer treat Venice's puppets as ordinary puppets
#define AUI_RELIGION_FIX_ARE_ALL_OUR_CITIES_HAVE_FAITH_BUILDING_VENICE_PUPPETS

// track how much damage a unit takes per turn in order to better predict whether it might die
#define MOD_CORE_PER_TURN_DAMAGE

// Uncomment this line to include the achievements hack code
// #define ACHIEVEMENT_HACKS
#if defined(ACHIEVEMENT_HACKS)
#undef NO_ACHIEVEMENTS
#else

// Comment out this line to include all the tutorials code
#define NO_TUTORIALS

// Comment out this line to switch off all custom mod logging
#define CUSTOMLOGDEBUG "CustomMods.log"
// Define to include the file name and line number in the log
#define CUSTOMLOGFILEINFO
// Define to include the function name in the log
#define CUSTOMLOGFUNCINFO

// Comment out this line to switch off all unified yield logging
// #define UNIFIEDLOGDEBUG "UnifiedYields.log"

// Comment out this line to remove minidumps - see http://forums.civfanatics.com/showthread.php?t=498919
// If minidumps are enabled, do NOT set GenerateDebugInfo=No (Props -> Config Props -> Linker -> Debugging)
#define MOD_DEBUG_MINIDUMP


// Comment these lines out to remove the associated code from the DLL,
// Alternatively, set the associated entries in the CustomModOptions table to disable(0) or enable(1) at load-time

// Enables the Espionage API - AFFECTS SAVE GAME DATA FORMAT
#define MOD_API_ESPIONAGE                           gCustomMods.isAPI_ESPIONAGE()
// Enables the Trade Routes API - AFFECTS SAVE GAME DATA FORMAT (v23)
#define MOD_API_TRADEROUTES                         gCustomMods.isAPI_TRADEROUTES()
// Enables the Religion API
#define MOD_API_RELIGION                            gCustomMods.isAPI_RELIGION()
// Enables the Plot Based Damage API (replaces fixed damage from mountains)
#define MOD_API_PLOT_BASED_DAMAGE                   gCustomMods.isAPI_PLOT_BASED_DAMAGE()
// Enables the Plot Yield tables (v35)
#define MOD_API_PLOT_YIELDS                         gCustomMods.isAPI_PLOT_YIELDS()
// Enables the Achievements table (v45)
#define MOD_API_ACHIEVEMENTS                        gCustomMods.isAPI_ACHIEVEMENTS()
// Enables the Extensions API
#define MOD_API_EXTENSIONS                          gCustomMods.isAPI_EXTENSIONS()
// Enables the LUA Extensions API
#define MOD_API_LUA_EXTENSIONS                      gCustomMods.isAPI_LUA_EXTENSIONS()
// Enables the Unified Yields extensions - thanks to bane_, JFD and Ulixes for extensive testing (v54)
#define MOD_API_UNIFIED_YIELDS                      (true)
// TODO - WH - Enabling consolidation introduces a double counting bug with faith added to buildings from a policy - which I don't have time to track down
#define MOD_API_UNIFIED_YIELDS_CONSOLIDATION        (true)
// Enables the Unified Yields (YIELD_TOURISM) extensions (v56)
#define MOD_API_UNIFIED_YIELDS_TOURISM              (true)
// Enables the Unified Yields (YIELD_GOLDEN_AGE_POINTS) extensions (v57)
#define MOD_API_UNIFIED_YIELDS_GOLDEN_AGE           (true)

// Push various hard-coded values controlling the game out into XML - see DB/CONFIG/GameInXml.sql for specific values -->
#define MOD_CONFIG_GAME_IN_XML                      gCustomMods.isCONFIG_GAME_IN_XML()
// Push various hard-coded values controlling the AI out into XML - see DB/CONFIG/AiInXml.sql for specific values
#define MOD_CONFIG_AI_IN_XML                        gCustomMods.isCONFIG_AI_IN_XML()

// If A declares war on B that preempts a co-op war, A is locked into the war for the same number of turns as if they had agreed to the co-op war (v86) 
#define MOD_GLOBAL_EARLY_COOP_WAR_LOCK				gCustomMods.isGLOBAL_EARLY_COOP_WAR_LOCK()
// Changes the stacking limits based on what the tile is (city, fort, plain, etc) - AFFECTS SAVE GAME DATA FORMAT
#define MOD_GLOBAL_STACKING_RULES                   gCustomMods.isGLOBAL_STACKING_RULES()
// This is the "No More Civilian Traffic Jams (NMCTJs) Mod" by Pazyryk - see http://forums.civfanatics.com/showthread.php?t=519754 (v45)
#define MOD_GLOBAL_BREAK_CIVILIAN_1UPT              gCustomMods.isGLOBAL_BREAK_CIVILIAN_1UPT()
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_1UPT)
#define MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS     (MOD_GLOBAL_BREAK_CIVILIAN_1UPT && gCustomMods.isGLOBAL_BREAK_CIVILIAN_RESTRICTIONS())
#endif
// Great Generals and Admirals gained from combat experience spawn in the war-zone and not in a distant city
#define MOD_GLOBAL_LOCAL_GENERALS                   gCustomMods.isGLOBAL_LOCAL_GENERALS()
// Separates out the repair fleet and change port abilities of the Great Admiral (v61)
#define MOD_GLOBAL_SEPARATE_GREAT_ADMIRAL           gCustomMods.isGLOBAL_SEPARATE_GREAT_ADMIRAL()
// Permits units to have promotion trees different from their assigned CombatClass
#define MOD_GLOBAL_PROMOTION_CLASSES                gCustomMods.isGLOBAL_PROMOTION_CLASSES()
// Permits ships to enter coastal forts/citadels in friendly lands
#define MOD_GLOBAL_PASSABLE_FORTS                   gCustomMods.isGLOBAL_PASSABLE_FORTS()
// Permits ships to enter any forts/citadels (based on code supplied by 12monkeys)
#define MOD_GLOBAL_PASSABLE_FORTS_ANY               (MOD_GLOBAL_PASSABLE_FORTS && gCustomMods.isGLOBAL_PASSABLE_FORTS_ANY())
// Goody Huts can always give gold, stops the late-game issue where entering a goody hut can result in nothing being awarded (v22)
#define MOD_GLOBAL_ANYTIME_GOODY_GOLD               gCustomMods.isGLOBAL_ANYTIME_GOODY_GOLD()
// Give initial production boost for cities founded on forests, as if the forest had been chopped down by a worker
#define MOD_GLOBAL_CITY_FOREST_BONUS                gCustomMods.isGLOBAL_CITY_FOREST_BONUS()
// Give initial production boost for cities founded on jungle, as if the jungle had been chopped down by a worker (v72)
#define MOD_GLOBAL_CITY_JUNGLE_BONUS                gCustomMods.isGLOBAL_CITY_JUNGLE_BONUS()
// Enables rebasing to and airlifting to/from improvements (v74)
#define MOD_GLOBAL_RELOCATION                       gCustomMods.isGLOBAL_RELOCATION()
// Mountain plots return their terrain as TERRAIN_MOUNTAIN and any land unit may enter a mountain that has a road/rail route
#define MOD_GLOBAL_ALPINE_PASSES                    gCustomMods.isGLOBAL_ALPINE_PASSES()
// Permits City States to gift ships
#define MOD_GLOBAL_CS_GIFT_SHIPS                    gCustomMods.isGLOBAL_CS_GIFT_SHIPS()
// Permits units to upgrade in allied militaristic City States
#define MOD_GLOBAL_CS_UPGRADES                      gCustomMods.isGLOBAL_CS_UPGRADES()
// City States will only raze captured cities if they are very unhappy
#define MOD_GLOBAL_CS_RAZE_RARELY                   gCustomMods.isGLOBAL_CS_RAZE_RARELY()
// City States can be liberated after they have been "bought" (Austria's or Venice's UA)
#define MOD_GLOBAL_CS_LIBERATE_AFTER_BUYOUT         gCustomMods.isGLOBAL_CS_LIBERATE_AFTER_BUYOUT()
// City States give different gifts depending on their type (cultural, religious, maritime, etc)
#define MOD_GLOBAL_CS_GIFTS                         gCustomMods.isGLOBAL_CS_GIFTS()
// Units gifted from City States receive XP from their spawning city, not the CS capital (v84)
#define MOD_GLOBAL_CS_GIFTS_LOCAL_XP                gCustomMods.isGLOBAL_CS_GIFTS_LOCAL_XP()
// City States allied to a major behave as an overseas territory of that major (v39)
#define MOD_GLOBAL_CS_OVERSEAS_TERRITORY            gCustomMods.isGLOBAL_CS_OVERSEAS_TERRITORY()
// City States at war with each other but allied to the same major will declare peace (v39)
#define MOD_GLOBAL_CS_NO_ALLIED_SKIRMISHES          gCustomMods.isGLOBAL_CS_NO_ALLIED_SKIRMISHES()
// Mercantile City States acquired via a Merchant of Venice do not lose their unique resources (v22)
#define MOD_GLOBAL_VENICE_KEEPS_RESOURCES           gCustomMods.isGLOBAL_VENICE_KEEPS_RESOURCES()
// Units attacking from cities, forts or citadels will not follow-up if they kill the defender
#define MOD_GLOBAL_NO_FOLLOWUP_FROM_CITIES          gCustomMods.isGLOBAL_NO_FOLLOWUP_FROM_CITIES()
// Units that can move after attacking can also capture civilian units (eg workers in empty barbarian camps) (v32)
#define MOD_GLOBAL_CAPTURE_AFTER_ATTACKING          gCustomMods.isGLOBAL_CAPTURE_AFTER_ATTACKING()
// Trade routes can't be plundered on ocean tiles - too much sea to hide in, too many directions to escape in (v39)
#define MOD_GLOBAL_NO_OCEAN_PLUNDERING              gCustomMods.isGLOBAL_NO_OCEAN_PLUNDERING()
// Remove assembled spaceship parts from conquered capitals
#define MOD_GLOBAL_NO_CONQUERED_SPACESHIPS          gCustomMods.isGLOBAL_NO_CONQUERED_SPACESHIPS()
// Other player's settlers captured from Barbarians will sometimes remain as settlers
#define MOD_GLOBAL_GRATEFUL_SETTLERS                gCustomMods.isGLOBAL_GRATEFUL_SETTLERS()
// Units that can found a city take their religion with them (v34)
#define MOD_GLOBAL_RELIGIOUS_SETTLERS               gCustomMods.isGLOBAL_RELIGIOUS_SETTLERS()
// Route To will only build roads, or upgrade road to rail, for human players (v44)
#define MOD_GLOBAL_QUICK_ROUTES                     gCustomMods.isGLOBAL_QUICK_ROUTES()
// Subs under ice are immune to all attacks except from other subs
#define MOD_GLOBAL_SUBS_UNDER_ICE_IMMUNITY          gCustomMods.isGLOBAL_SUBS_UNDER_ICE_IMMUNITY()
// Paratroops take AA damage from hostile units
#define MOD_GLOBAL_PARATROOPS_AA_DAMAGE             gCustomMods.isGLOBAL_PARATROOPS_AA_DAMAGE()
// Nukes will melt ice
#define MOD_GLOBAL_NUKES_MELT_ICE                   gCustomMods.isGLOBAL_NUKES_MELT_ICE() 
// Great Works can generate different yields than just culture (v25)
#define MOD_GLOBAL_GREATWORK_YIELDTYPES             gCustomMods.isGLOBAL_GREATWORK_YIELDTYPES() 
// Great Artists, Writers and Musicians that do NOT create Great Works can be "reborn" (v84)
#define MOD_GLOBAL_NO_LOST_GREATWORKS               gCustomMods.isGLOBAL_NO_LOST_GREATWORKS() 
// Units of this type will not be gifted by City States (v46)
#define MOD_GLOBAL_EXCLUDE_FROM_GIFTS               gCustomMods.isGLOBAL_EXCLUDE_FROM_GIFTS()
// Units of this type may move after being upgraded (v46)
#define MOD_GLOBAL_MOVE_AFTER_UPGRADE               gCustomMods.isGLOBAL_MOVE_AFTER_UPGRADE()
// Units of this type may never embark (v46)
#define MOD_GLOBAL_CANNOT_EMBARK                    gCustomMods.isGLOBAL_CANNOT_EMBARK()
// Separates the Engineer, Scientist and Merchant GP counters (v52)
#define MOD_GLOBAL_SEPARATE_GP_COUNTERS             gCustomMods.isGLOBAL_SEPARATE_GP_COUNTERS()
// Removes free GP (from buildings, policies, traits, etc) from the GP counters (v61)
#define MOD_GLOBAL_TRULY_FREE_GP                    gCustomMods.isGLOBAL_TRULY_FREE_GP()
// No auto spawn great prophets for human players, must select pulldown menu in Lua when you are ready to spawn one, only pre-Industrial era
#define MOD_NO_AUTO_SPAWN_PROPHET					gCustomMods.isNO_AUTO_SPAWN_PROPHET()
// Change Assyria's trait to choosing a free tech upon city conquest
#define MOD_ALTERNATE_ASSYRIA_TRAIT					gCustomMods.isALTERNATE_ASSYRIA_TRAIT()
// No repairing tiles in foreign lands -- prevents an exploit where a human player can pillage and repair
#define MOD_NO_REPAIR_FOREIGN_LANDS					gCustomMods.isNO_REPAIR_FOREIGN_LANDS()
// No yield from Ice Features
#define MOD_NO_YIELD_ICE							gCustomMods.isNO_YIELD_ICE()
// No major civ gifting exploit
#define MOD_NO_MAJORCIV_GIFTING						gCustomMods.isNO_MAJORCIV_GIFTING()
// no healing on mountains if not a city plot
#define MOD_NO_HEALING_ON_MOUNTAINS					gCustomMods.isNO_HEALING_ON_MOUNTAINS()
// Allows pillaging of permanent improvements if at war
#define MOD_PILLAGE_PERMANENT_IMPROVEMENTS			gCustomMods.isPILLAGE_PERMANENT_IMPROVEMENTS()
// Tech bonuses from other teams require an embassy or spy in their capital and not from just having met them (v30)
#define MOD_DIPLOMACY_TECH_BONUSES                  gCustomMods.isDIPLOMACY_TECH_BONUSES()
// Human players will auto-denounce AI players before going to war with them (v39)
#define MOD_DIPLOMACY_AUTO_DENOUNCE                 gCustomMods.isDIPLOMACY_AUTO_DENOUNCE()
// Changes for the City State Diplomacy mod by Gazebo - AFFECTS SAVE GAME DATA FORMAT (v35)
#define MOD_DIPLOMACY_CITYSTATES                    gCustomMods.isDIPLOMACY_CITYSTATES()
#if defined(MOD_DIPLOMACY_CITYSTATES)
#define MOD_DIPLOMACY_CITYSTATES_QUESTS             (MOD_DIPLOMACY_CITYSTATES && gCustomMods.isDIPLOMACY_CITYSTATES_QUESTS())
#define MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS        (MOD_DIPLOMACY_CITYSTATES && gCustomMods.isDIPLOMACY_CITYSTATES_RESOLUTIONS())
#define MOD_DIPLOMACY_CITYSTATES_HURRY              (MOD_DIPLOMACY_CITYSTATES && gCustomMods.isDIPLOMACY_CITYSTATES_HURRY())
#endif

// To turn off or on ships firing while in Cities
#define MOD_SHIPS_FIRE_IN_CITIES_IMPROVEMENTS		 gCustomMods.isSHIPS_FIRE_IN_CITIES_IMPROVEMENTS()

// Ability to add new natural wonder features with graphics
#define MOD_PSEUDO_NATURAL_WONDER					gCustomMods.isPSEUDO_NATURAL_WONDER()

//Flips open borders to apply in opposite ways- you have to give open borders to gain the tourism bonus
#define MOD_BALANCE_FLIPPED_TOURISM_MODIFIER_OPEN_BORDERS

//Community Patch Info
#define MOD_COMMUNITY_PATCH							gCustomMods.isCOMMUNITY_PATCH()
#if defined(MOD_COMMUNITY_PATCH)
#define MOD_BALANCE_CORE							(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE())
#define MOD_CORE_DEBUGGING							(MOD_COMMUNITY_PATCH && gCustomMods.isCORE_DEBUGGING())
#define MOD_BALANCE_CORE_YIELDS						(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_YIELDS())
#define MOD_BALANCE_CORE_SPIES						(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_SPIES())
#define MOD_BALANCE_CORE_SPIES_ADVANCED				(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_SPIES_ADVANCED())
#define MOD_BALANCE_CORE_MILITARY					(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_MILITARY())
#define MOD_BALANCE_CORE_SETTLER					(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_SETTLER())
#define MOD_BALANCE_CORE_SETTLER_ADVANCED			(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_SETTLER_ADVANCED())
#define MOD_BALANCE_CORE_DEALS						(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_DEALS())
#define MOD_BALANCE_CORE_DEALS_ADVANCED				(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_DEALS_ADVANCED())
#define MOD_BALANCE_CORE_MINORS						(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_MINORS())
#define MOD_BALANCE_CORE_DIFFICULTY					(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_DIFFICULTY())
#define MOD_BALANCE_CORE_HAPPINESS					(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_HAPPINESS())
#define MOD_BALANCE_CORE_HAPPINESS_MODIFIERS		(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_HAPPINESS_MODIFIERS())
#define MOD_BALANCE_CORE_HAPPINESS_NATIONAL			(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_HAPPINESS_NATIONAL())
#define MOD_BALANCE_CORE_HAPPINESS_LUXURY			(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_HAPPINESS_LUXURY())
#define MOD_BALANCE_CORE_POP_REQ_BUILDINGS			(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_POP_REQ_BUILDINGS())
#define MOD_BALANCE_CORE_SETTLER_RESET_FOOD			(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_SETTLER_RESET_FOOD())
#define MOD_BALANCE_CORE_WONDERS_VARIABLE_REWARD	(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_WONDERS_VARIABLE_REWARD())
#define MOD_BALANCE_CORE_BELIEFS_RESOURCE			(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_BELIEFS_RESOURCE())
#define MOD_BALANCE_CORE_AFRAID_ANNEX				(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_AFRAID_ANNEX())
#define MOD_BALANCE_CORE_BUILDING_INSTANT_YIELD		(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_BUILDING_INSTANT_YIELD())
#define MOD_BALANCE_CORE_GRANDSTRATEGY_AI			(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_GRANDSTRATEGY_AI())
#define MOD_BALANCE_CORE_BELIEFS					(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_BELIEFS())
#define MOD_BALANCE_CORE_FOLLOWER_POP_WONDER		(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_FOLLOWER_POP_WONDER())
#define MOD_BALANCE_CORE_POLICIES					(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_POLICIES())
#define MOD_BALANCE_CORE_BARBARIAN_THEFT			(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_BARBARIAN_THEFT())
#define MOD_BALANCE_CORE_SETTLER_MOVE				(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_SETTLER_MOVE())
#define MOD_BALANCE_CORE_RESOURCE_FLAVORS			(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_RESOURCE_FLAVORS())
#define MOD_BALANCE_CORE_PURCHASE_COST_INCREASE		(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_PURCHASE_COST_INCREASE())
#define MOD_BALANCE_CORE_PUPPET_PURCHASE			(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_PUPPET_PURCHASE())
#define MOD_BALANCE_CORE_EMBARK_CITY_NO_COST		(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_EMBARK_CITY_NO_COST())
#define MOD_BALANCE_CORE_MINOR_PROTECTION			(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_MINOR_PROTECTION())
#define MOD_BALANCE_CORE_IDEOLOGY_START				(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_IDEOLOGY_START())
#define MOD_BALANCE_CORE_WONDER_COST_INCREASE		(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_WONDER_COST_INCREASE())
#define MOD_BALANCE_CORE_MINOR_CIV_GIFT				(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_MINOR_CIV_GIFT())
#define MOD_BALANCE_CORE_DIPLOMACY					(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_DIPLOMACY())
#define MOD_BALANCE_CORE_DIPLOMACY_ERA_INFLUENCE	(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_DIPLOMACY_ERA_INFLUENCE())
#define MOD_BALANCE_CORE_DIPLOMACY_ADVANCED			(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_DIPLOMACY_ADVANCED())
#define MOD_BALANCE_CORE_LUXURIES_TRAIT				(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_LUXURIES_TRAIT())
#define MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED (MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_MILITARY_PROMOTION_ADVANCED())
#define MOD_BALANCE_CORE_MILITARY_LOGGING			(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_MILITARY_LOGGING())
#define MOD_BALANCE_CORE_RESOURCE_MONOPOLIES		(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_RESOURCE_MONOPOLIES())
#define MOD_BALANCE_CORE_RESOURCE_MONOPOLIES_STRATEGIC		(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_RESOURCE_MONOPOLIES_STRATEGIC())
#define MOD_BALANCE_CORE_BUILDING_INVESTMENTS		(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_BUILDING_INVESTMENTS())
#define MOD_BALANCE_CORE_UNIT_INVESTMENTS			(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_UNIT_INVESTMENTS())
#define MOD_BALANCE_CORE_BUILDING_RESOURCE_MAINTENANCE		(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_BUILDING_RESOURCE_MAINTENANCE())
#define MOD_BALANCE_CORE_ENGINEER_HURRY				(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_ENGINEER_HURRY())
#define MOD_BALANCE_CORE_MAYA_CHANGE				(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_MAYA_CHANGE())
#define MOD_BALANCE_CORE_PORTUGAL_CHANGE			(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_PORTUGAL_CHANGE())
#define MOD_BALANCE_CORE_MINOR_VARIABLE_BULLYING	(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_MINOR_VARIABLE_BULLYING())
#define MOD_BALANCE_CORE_MINOR_PTP_MINIMUM_VALUE	(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_MINOR_PTP_MINIMUM_VALUE())
#define MOD_BALANCE_YIELD_SCALE_ERA					(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_YIELD_SCALE_ERA())
#define MOD_BALANCE_CORE_NEW_GP_ATTRIBUTES			(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_NEW_GP_ATTRIBUTES())
#define MOD_BALANCE_CORE_JFD						(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_JFD())
#define MOD_BALANCE_CORE_MILITARY_RESISTANCE		(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_MILITARY_RESISTANCE())
#define MOD_BALANCE_CORE_PANTHEON_RESET_FOUND		(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_PANTHEON_RESET_FOUND())
#define MOD_BALANCE_CORE_VICTORY_GAME_CHANGES		(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_VICTORY_GAME_CHANGES())
#define MOD_BALANCE_CORE_EVENTS						(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_EVENTS())
#define MOD_NO_RANDOM_TEXT_CIVS						(MOD_COMMUNITY_PATCH && gCustomMods.isNO_RANDOM_TEXT_CIVS())
#define MOD_BALANCE_RETROACTIVE_PROMOS				(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_RETROACTIVE_PROMOS())
#define MOD_BALANCE_NO_GAP_DURING_GA				(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_NO_GAP_DURING_GA())
#define MOD_BALANCE_DYNAMIC_UNIT_SUPPLY				(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_DYNAMIC_UNIT_SUPPLY())
#define MOD_BALANCE_CORE_UNIQUE_BELIEFS_ONLY_FOR_CIV	(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_UNIQUE_BELIEFS_ONLY_FOR_CIV())
#define MOD_BALANCE_DEFENSIVE_PACTS_AGGRESSION_ONLY	(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_DEFENSIVE_PACTS_AGGRESSION_ONLY())
#define MOD_BALANCE_CORE_SCALING_TRADE_DISTANCE	(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_SCALING_TRADE_DISTANCE())
#define MOD_BALANCE_CORE_SCALING_XP				(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_SCALING_XP())
#define MOD_BALANCE_CORE_HALF_XP_PURCHASE				(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_HALF_XP_PURCHASE())
#endif
// activate eureka for tech cost bonus 'quest'
#define MOD_CIV6_EUREKA								gCustomMods.isCIV6_EUREKAS()
// Add a "worker cost" to improvement and delete the worker when he expands all his "strength"
#define MOD_CIV6_WORKER								gCustomMods.isCIV6_TYPE_WORKER()
// Roads are created by trade routes.
#define MOD_CIV6_ROADS								gCustomMods.isCIV6_ROADS()
// scale city-state yield per city owned
#define MOD_CITY_STATE_SCALE						gCustomMods.isCITY_STATE_SCALE()
// allow anyone to choose his panthon, even if an other player has already choosed it.
#define MOD_ANY_PANTHEON						gCustomMods.isANY_PANTHEON()
// Changes melee ship units to be cargo carrying units with added promotions for ship and cargo
#define MOD_CARGO_SHIPS								gCustomMods.isCARGO_SHIPS()
// Changes for the CivIV Diplomacy Features mod by Putmalk - AFFECTS SAVE GAME DATA FORMAT (v36)
#define MOD_DIPLOMACY_CIV4_FEATURES                 gCustomMods.isDIPLOMACY_CIV4_FEATURES()
// Adds an option to Advanced Setup to allow gaining Great General and Great Admiral Points from fighting with Barbarians
#define MOD_BARBARIAN_GG_GA_POINTS					gCustomMods.isBARBARIAN_GG_GA_POINTS()
// Grants Celts maximum up to 3 unimproved adjacent forests for faith
#define MOD_ALTERNATE_CELTS							gCustomMods.isALTERNATE_CELTS()
// Permits units to earn GG/GA points from killing barbarians (v83)
#define MOD_TRAITS_GG_FROM_BARBARIANS               gCustomMods.isTRAITS_GG_FROM_BARBARIANS()
// Permits land units to cross ice - AFFECTS SAVE GAME DATA FORMAT
#define MOD_TRAITS_CROSSES_ICE                      gCustomMods.isTRAITS_CROSSES_ICE()
// Permits cities to work more rings - AFFECTS SAVE GAME DATA FORMAT
#define MOD_TRAITS_CITY_WORKING                     gCustomMods.isTRAITS_CITY_WORKING()
// Enables traits to be enabled/obsoleted via beliefs and policies (v77)
#define MOD_TRAITS_OTHER_PREREQS                    gCustomMods.isTRAITS_OTHER_PREREQS()
// Enables any belief to be selected, even if already taken (v46)
#define MOD_TRAITS_ANY_BELIEF                       gCustomMods.isTRAITS_ANY_BELIEF()
// Enables additional trade route related traits (v52)
#define MOD_TRAITS_TRADE_ROUTE_BONUSES              gCustomMods.isTRAITS_TRADE_ROUTE_BONUSES()
// Enables additional unit supply from traits (v78)
#define MOD_TRAITS_EXTRA_SUPPLY                     gCustomMods.isTRAITS_EXTRA_SUPPLY()

// Permits cities to work more rings - AFFECTS SAVE GAME DATA FORMAT
#define MOD_POLICIES_CITY_WORKING                   gCustomMods.isPOLICIES_CITY_WORKING()

// Restricts a Team from passing into the next era before they have found all techs of their current era
#define MOD_ERA_RESTRICTION							gCustomMods.isERA_RESTRICTION()

// turn on or off yields for terrains that have a feature with GetPlayerTraits()->IsTradeRouteOnly
#define MOD_USE_TRADE_FEATURES						gCustomMods.isUSE_TRADE_FEATURES()

// Permits cities to work more rings - AFFECTS SAVE GAME DATA FORMAT
#define MOD_TECHS_CITY_WORKING                      gCustomMods.isTECHS_CITY_WORKING()

// Permits variable great general and admiral aura ranges (v83)
#define MOD_PROMOTIONS_AURA_CHANGE                  gCustomMods.isPROMOTIONS_AURA_CHANGE()
// Permits units to earn GG/GA points from killing barbarians (v83)
#define MOD_PROMOTIONS_GG_FROM_BARBARIANS           gCustomMods.isPROMOTIONS_GG_FROM_BARBARIANS()
// Permits variable recon ranges by creating extra recon range promotions (like extra sight range)
#define MOD_PROMOTIONS_VARIABLE_RECON               gCustomMods.isPROMOTIONS_VARIABLE_RECON()
// Permits land units to cross mountains (like the Carthage trait)
#define MOD_PROMOTIONS_CROSS_MOUNTAINS              gCustomMods.isPROMOTIONS_CROSS_MOUNTAINS()
// Permits coastal units to cross oceans
#define MOD_PROMOTIONS_CROSS_OCEANS                 gCustomMods.isPROMOTIONS_CROSS_OCEANS()
// Permits land units to cross ice
#define MOD_PROMOTIONS_CROSS_ICE                    gCustomMods.isPROMOTIONS_CROSS_ICE()
// Adds a HalfMove feature to the UnitPromotions_Terrains and UnitPromotions_Features tables
#define MOD_PROMOTIONS_HALF_MOVE                    gCustomMods.isPROMOTIONS_HALF_MOVE()
// Permits Deep Water (Ocean) embarkation for hovering units - AFFECTS SAVE GAME DATA FORMAT
#define MOD_PROMOTIONS_DEEP_WATER_EMBARKATION       gCustomMods.isPROMOTIONS_DEEP_WATER_EMBARKATION()
// Permits naval units to transfer their moves to Great Admirals (like land units can to Great Generals) (v39)
#define MOD_PROMOTIONS_FLAGSHIP                     gCustomMods.isPROMOTIONS_FLAGSHIP()
// Permits units to be named based on a promotion they have (v46)
#define MOD_PROMOTIONS_UNIT_NAMING                  gCustomMods.isPROMOTIONS_UNIT_NAMING()
// Permits units to receive a combat bonus from being near an improvement (v46)
#define MOD_PROMOTIONS_IMPROVEMENT_BONUS            gCustomMods.isPROMOTIONS_IMPROVEMENT_BONUS()

// Permit the focus (gold/production/culture) of puppet cities to be set (but not what is being built or how specialists are allocated)
#define MOD_UI_CITY_PRODUCTION                      gCustomMods.isUI_CITY_PRODUCTION()
// Permit human players to choose they own city tiles due to cultural expansion
#define MOD_UI_CITY_EXPANSION                       gCustomMods.isUI_CITY_EXPANSION()

// National Wonders (NW) requiring a building in every city exclude those being razed (in addition to excluding puppets) (v63)
#define MOD_BUILDINGS_NW_EXCLUDE_RAZING             gCustomMods.isBUILDINGS_NW_EXCLUDE_RAZING()
// Purchase of buildings in cities allows for any current production
#define MOD_BUILDINGS_PRO_RATA_PURCHASE             gCustomMods.isBUILDINGS_PRO_RATA_PURCHASE()
// Permits cities to work more rings - AFFECTS SAVE GAME DATA FORMAT
#define MOD_BUILDINGS_CITY_WORKING                  gCustomMods.isBUILDINGS_CITY_WORKING()

// Scales trade routes based on map size and game speed (v52)
#define MOD_TRADE_ROUTE_SCALING                     gCustomMods.isTRADE_ROUTE_SCALING()

// Permits wonder resource (ie Marble) trade routes to be established (v43)
#define MOD_TRADE_WONDER_RESOURCE_ROUTES            gCustomMods.isTRADE_WONDER_RESOURCE_ROUTES()

// Permits units to have no supply cost (v77)
#define MOD_UNITS_NO_SUPPLY                         gCustomMods.isUNITS_NO_SUPPLY()
// Permits units to have other than GameDefines.MAX_HIT_POINTS maximum hit points (v77)
#define MOD_UNITS_MAX_HP                            gCustomMods.isUNITS_MAX_HP()
// Enables the XP times 100 API (v77)
#define MOD_UNITS_XP_TIMES_100                      gCustomMods.isUNITS_XP_TIMES_100()
// Restricts worker suggestions to local tiles
#define MOD_UNITS_LOCAL_WORKERS                     gCustomMods.isUNITS_LOCAL_WORKERS()
// Hovering unit can only heal over land
#define MOD_UNITS_HOVERING_LAND_ONLY_HEAL           gCustomMods.isUNITS_HOVERING_LAND_ONLY_HEAL()

// Removes religion preference
#define MOD_RELIGION_NO_PREFERRENCES                gCustomMods.isRELIGION_NO_PREFERRENCES()
// Randomises religion choice (if preferred religion unavailable)
#define MOD_RELIGION_RANDOMISE                      gCustomMods.isRELIGION_RANDOMISE()
// Adds ConversionModifier and GlobalConversionModifier (in the same vein as espionage modifiers) to buildings - AFFECTS SAVE GAME DATA FORMAT
#define MOD_RELIGION_CONVERSION_MODIFIERS           gCustomMods.isRELIGION_CONVERSION_MODIFIERS()
// Keeps overflow faith from spawning a Great Prophet if the base spawn chance is 100% (v21)
#define MOD_RELIGION_KEEP_PROPHET_OVERFLOW          gCustomMods.isRELIGION_KEEP_PROPHET_OVERFLOW()
// Inquisitors will keep religion out of allied City State cities if positioned adjacent (v60)
#define MOD_RELIGION_ALLIED_INQUISITORS             gCustomMods.isRELIGION_ALLIED_INQUISITORS()
// Send purchase notifications at every boundary and not just the first (v42)
#define MOD_RELIGION_RECURRING_PURCHASE_NOTIFIY     gCustomMods.isRELIGION_RECURRING_PURCHASE_NOTIFIY()
// Adds support for the Belief_PlotYieldChanges table (v35)
#if defined(MOD_API_PLOT_YIELDS)
#define MOD_RELIGION_PLOT_YIELDS                    (gCustomMods.isRELIGION_PLOT_YIELDS() && MOD_API_PLOT_YIELDS)
#endif
// Adds support for Great People being purchased by faith to be specified on a policy (usually a finisher) and not hard-coded (v53)
#define MOD_RELIGION_POLICY_BRANCH_FAITH_GP         gCustomMods.isRELIGION_POLICY_BRANCH_FAITH_GP()
// Adds support for "local" religions (ie ones that only have influence within the civ's own territory) (v48)
#define MOD_RELIGION_LOCAL_RELIGIONS                gCustomMods.isRELIGION_LOCAL_RELIGIONS()

// Enables production to be stockpiled (v28)
#define MOD_PROCESS_STOCKPILE                       gCustomMods.isPROCESS_STOCKPILE()

// Fixes the AI's inability to use combat units as secondary workers (v26)
#define MOD_AI_SECONDARY_WORKERS                    gCustomMods.isAI_SECONDARY_WORKERS()
// Fixes the AI's inability to use combat units for founding cities (v26)
#define MOD_AI_SECONDARY_SETTLERS                   gCustomMods.isAI_SECONDARY_SETTLERS()

// Features from the "Smart AI mod" by Ninakoru - see http://forums.civfanatics.com/showthread.php?t=521955 (v50)
#define MOD_AI_SMART                                gCustomMods.isAI_SMART()

#if defined(MOD_AI_SMART)
// Use Great people more effectively, plant some improvements early, and later use GP powers (v50)
#define MOD_AI_SMART_GREAT_PEOPLE                   (MOD_AI_SMART && gCustomMods.isAI_SMART_GREAT_PEOPLE())
// Delay grand strategy bias until the Renaissance (v50)
#define MOD_AI_SMART_GRAND_STRATEGY                 (MOD_AI_SMART && gCustomMods.isAI_SMART_GRAND_STRATEGY())
// Stop making archaeologists sooner and also disband archaeologists if there are not valid targets (v50)
#define MOD_AI_SMART_ARCHAEOLOGISTS                 (MOD_AI_SMART && gCustomMods.isAI_SMART_ARCHAEOLOGISTS())
// Disband long obsolete units, eg triremes in industrial era (v50)
#define MOD_AI_SMART_DISBAND                        (MOD_AI_SMART && gCustomMods.isAI_SMART_DISBAND())
// Upgrade more units per turn if there are lots of units that can be upgraded. Also upgrade air units more often (v50)
#define MOD_AI_SMART_UPGRADES                       (MOD_AI_SMART && gCustomMods.isAI_SMART_UPGRADES())
// Units with at least 75% health will avoid healing (v50)
#define MOD_AI_SMART_HEALING                        (MOD_AI_SMART && gCustomMods.isAI_SMART_HEALING())
// Units won't randomly embark to water tiles (v50)
#define MOD_AI_SMART_FLEE_FROM_DANGER               (MOD_AI_SMART && gCustomMods.isAI_SMART_FLEE_FROM_DANGER())
// Ranged units are always able to move AND shoot on the same turn and should not attack over and over a city with 1 HP remaining. (v51)
#define MOD_AI_SMART_RANGED_UNITS                   (MOD_AI_SMART && gCustomMods.isAI_SMART_RANGED_UNITS())
// AI will hold planes back for interceptions and perform air sweep missions more efficiently, if enemy aircraft are nearby (v50)
#define MOD_AI_SMART_AIR_TACTICS                    (MOD_AI_SMART && gCustomMods.isAI_SMART_AIR_TACTICS())
// Quickens how fast the AI will critically ask for settler based also on difficulty and game speed properties.
#define MOD_AI_SMART_FASTER_CAPITAL_SETTLER_NEED_BY_DIFFICULTY_SPEED gCustomMods.isAI_SMART()
// Also check for water and non luxury resources for the locale tech multiplier, luxuries get double value.
#define MOD_AI_SMART_TECH_LOCALE_PRIORITY_CHECK_ALL_RESOURCES gCustomMods.isAI_SMART()
// Tech is calculated with flavors from current needs vs grand strategy player flavor, based on this.
// As difficulty is not taken into account, for standard speed will use a base of 500 turns even for 
// a deity AI player. The value even out at turn 250, witch is stupid due to AI bonuses the game could
// be near finished (if not finished) by then.
#define MOD_AI_SMART_TECH_GAME_PROGRESS_UPDATED_WITH_DIFFICULTY gCustomMods.isAI_SMART()
// As how tech flavor works and is calculated, it is easily zeroed specially at the beginning.
// Zeroed values will not make further calculations with resources and player priorities...
// So let's rescale them on a 2 to 10 range.
#define MOD_AI_SMART_TECH_FLAVOR_MINIMUM_VALUES     gCustomMods.isAI_SMART()
// When recursively propagating, use NUM_AND_TECH_PREREQS (6) instead of NUM_OR_TECH_PREREQS (3), while in 
// practice only Agriculture uses 4 and prereqs, could have unexpected flavors with a custom tech tree.
#define MOD_AI_SMART_TECH_FLAVOR_PROPAGATION_BUGFIX gCustomMods.isAI_SMART()
// Minor adjustement to avoid further calculations if flavor value is zero.
#define MOD_AI_SMART_OPTIMIZE_FLAVOR_WEIGHT_ROUNDTRIPS gCustomMods.isAI_SMART()

// Features from the "Smart AI V3 mod" by Ninakoru - see http://forums.civfanatics.com/showthread.php?t=562319 (v85)
#define MOD_AI_SMART_V3                             gCustomMods.isAI_SMART_V3()

#endif

//
//	 GameEvents.TradeRouteCompleted.Add(function( iOriginOwner, iOriginCity, iDestOwner, iDestCity, eDomain, eConnectionTradeType) end)
// Events sent when terraforming occurs (v33)
//   GameEvents.TerraformingMap.Add(function(iEvent, iLoad) end)
//   GameEvents.TerraformingPlot.Add(function(iEvent, iPlotX, iPlotY, iInfo, iNewValue, iOldValue, iNewExtra, iOldExtra) end)
#define MOD_EVENTS_TERRAFORMING                     gCustomMods.isEVENTS_TERRAFORMING()

// Events sent when plots change from worker actions (v44)
//   GameEvents.TileFeatureChanged.Add(function(iPlotX, iPlotY, iOwner, iOldFeature, iNewFeature) end)
//   GameEvents.TileImprovementChanged.Add(function(iPlotX, iPlotY, iOwner, iOldImprovement, iNewImprovement, bPillaged) end)
//   GameEvents.TileOwnershipChanged.Add(function(iPlotX, iPlotY, iOwner, iOldOwner) end) (v46)
//   GameEvents.TileRouteChanged.Add(function(iPlotX, iPlotY, iOwner, iOldRoute, iNewRoute, bPillaged) end)
#define MOD_EVENTS_TILE_IMPROVEMENTS                gCustomMods.isEVENTS_TILE_IMPROVEMENTS()

// Event sent when a plot is revealed (v58)
//   GameEvents.TileRevealed.Add(function(iPlotX, iPlotY, iteam, iFromTeam, bFirst) end)
#define MOD_EVENTS_TILE_REVEALED                    gCustomMods.isEVENTS_TILE_REVEALED()

// Event sent when a team circumnavigates the globe
//   GameEvents.CircumnavigatedGlobe.Add(function(iTeam) end)
#define MOD_EVENTS_CIRCUMNAVIGATION                 gCustomMods.isEVENTS_CIRCUMNAVIGATION()

// Event sent when the player enters a new era, see also NewEraPopup.lua and BUTTONPOPUP_NEW_ERA
//   GameEvents.TeamSetEra.Add(function(eTeam, eEra, bFirst) end)
#define MOD_EVENTS_NEW_ERA                          gCustomMods.isEVENTS_NEW_ERA()

// Event sent when the team discovers a new Natural Wonder
//   GameEvents.NaturalWonderDiscovered.Add(function(iTeam, iFeature, iX, iY, bFirst) end)
#define MOD_EVENTS_NW_DISCOVERY                     gCustomMods.isEVENTS_NW_DISCOVERY()

// Event sent during Game.DoFromUIDiploEvent, see also DiscussionDialog.lua
//   GameEvents.UiDiploEvent.Add(function(iEvent, iAIPlayer, iArg1, iArg2) end)
#define MOD_EVENTS_DIPLO_EVENTS                     gCustomMods.isEVENTS_DIPLO_EVENTS()

// Enhanced Diplomacy Modifiers events (v53)
//   GameEvents.GetDiploModifier.Add(function(iEvent, iFromPlayer, iToPlayer) return 0 end)
#define MOD_EVENTS_DIPLO_MODIFIERS                  gCustomMods.isEVENTS_DIPLO_MODIFIERS()

// Events sent on status change with City States
//   GameEvents.MinorFriendsChanged.Add(function(iMinor, iMajor, bIsFriend, iOldFriendship, iNewFriendship) end)
//   GameEvents.MinorAlliesChanged.Add(function(iMinor, iMajor, bIsAlly, iOldFriendship, iNewFriendship) end)
#define MOD_EVENTS_MINORS                           gCustomMods.isEVENTS_MINORS()

// Event sent upon a City State giving a gift (v73)
//   GameEvents.MinorGift.Add(function(iMinor, iMajor, iGift, iFriendshipBoost, 0, bFirstMajorCiv, false, szTxtKeySuffix) end)
//   GameEvents.MinorGiftUnit.Add(function(iMinor, iMajor, iUnitType) end)
#define MOD_EVENTS_MINORS_GIFTS                     gCustomMods.isEVENTS_MINORS_GIFTS()

// Events sent on interaction with City States (v68)
//   GameEvents.PlayerCanProtect.Add(function(iPlayer, iCS) return true end)
//   GameEvents.PlayerProtected.Add(function(iPlayer, iCS) end)
//   GameEvents.PlayerCanRevoke.Add(function(iPlayer, iCS) return true end)
//   GameEvents.PlayerRevoked.Add(function(iPlayer, iCS, bPledgeNowBroken) end)
//   GameEvents.PlayerCanBuyOut.Add(function(iPlayer, iCS) return true end)
//   GameEvents.PlayerBoughtOut.Add(function(iPlayer, iCS) end)
//   GameEvents.PlayerCanBullyGold.Add(function(iPlayer, iCS) return true end)
//   GameEvents.PlayerCanBullyUnit.Add(function(iPlayer, iCS) return true end)
//   GameEvents.PlayerBullied.Add(function(iPlayer, iCS, iGold, iUnitType, iPlotX, iPlotY) end)
//   GameEvents.PlayerCanGiftGold.Add(function(iPlayer, iCS) return true end)
//   GameEvents.PlayerCanGiftUnit.Add(function(iPlayer, iCS, iUnit) return true end)
//   GameEvents.PlayerCanGiftImprovement.Add(function(iPlayer, iCS) return true end)
//   GameEvents.PlayerGifted.Add(function(iPlayer, iCS, iGold, iUnitType, iPlotX, iPlotY) end)
//   GameEvents.PlayerCanTransitMinorCity.Add(function(iPlayer, iCS, iCity, iPlotX, iPlotY) return true end) (v74)
//   GameEvents.UnitCanTransitMinorCity.Add(function(iPlayer, iUnit, iCS, iCity, iPlotX, iPlotY) return true end) (v74)
#define MOD_EVENTS_MINORS_INTERACTION               gCustomMods.isEVENTS_MINORS_INTERACTION()

// Events sent by Barbarians (v68)
//   GameEvents.BarbariansCanFoundCamp.Add(function(iPlotX, iPlotY) return true end)
//   GameEvents.BarbariansCampFounded.Add(function(iPlotX, iPlotY) end)
//   GameEvents.BarbariansCampCleared.Add(function(iPlotX, iPlotY, iPlayer) end)
//   GameEvents.BarbariansCampCanSpawnUnit.Add(function(iPlotX, iPlotY) return true end)
//   GameEvents.BarbariansCampGetSpawnUnit.Add(function(iPlotX, iPlotY, iPrefUnitType) return iPrefUnitType end)
//   GameEvents.BarbariansSpawnedUnit.Add(function(iPlotX, iPlotY, iUnitType) end)
#define MOD_EVENTS_BARBARIANS                       gCustomMods.isEVENTS_BARBARIANS()

// Event sent when a Goody Hut is entered (v33)
//   GameEvents.GoodyHutCanNotReceive.Add(function(iPlayer, iUnit, eGoody, bPick) return false end)
//   GameEvents.GoodyHutReceivedBonus.Add(function(iPlayer, iUnit, eGoody, iX, iY) end)
#define MOD_EVENTS_GOODY_CHOICE                     gCustomMods.isEVENTS_GOODY_CHOICE()

// Events sent if a Goody Hut is giving a tech
//   GameEvents.GoodyHutCanResearch.Add(function(iPlayer, eTech) return true end)
//   GameEvents.GoodyHutTechResearched.Add(function(iPlayer, eTech) end)
#define MOD_EVENTS_GOODY_TECH                       gCustomMods.isEVENTS_GOODY_TECH()

// Event sent to allow Lua to override the AI's choice of tech (v30)
//   GameEvents.AiOverrideChooseNextTech.Add(function(iPlayer, bFreeTech) return iChoosenTech end)
#define MOD_EVENTS_AI_OVERRIDE_TECH                 gCustomMods.isEVENTS_AI_OVERRIDE_TECH()

// Events sent by Great People actions
//   GameEvents.GreatPersonExpended.Add(function(iPlayer, iUnit, iUnitType, iX, iY) end)
#define MOD_EVENTS_GREAT_PEOPLE                     gCustomMods.isEVENTS_GREAT_PEOPLE()

// Events sent when a player is about to found a religion
//   GameEvents.PlayerCanFoundPantheon.Add(function(iPlayer) return true end)
//   GameEvents.PlayerCanFoundReligion.Add(function(iPlayer, iCity) return true end)
//   GameEvents.GetReligionToFound.Add(function(iPlayer, iPreferredReligion, bIsAlreadyFounded) return iPreferredReligion end)
//   GameEvents.PantheonFounded.Add(function(iPlayer, iCapitalCity, iReligion, iBelief1) end)
//   GameEvents.ReligionFounded.Add(function(iPlayer, iHolyCity, iReligion, iBelief1, iBelief2, iBelief3, iBelief4, iBelief5) end)
//   GameEvents.ReligionEnhanced.Add(function(iPlayer, iReligion, iBelief1, iBelief2) end)
//   GameEvents.ReligionReformed.Add(function(iPlayer, iReligion, iBelief1) end) (v65)
#define MOD_EVENTS_FOUND_RELIGION                   gCustomMods.isEVENTS_FOUND_RELIGION()

// Events sent when choosing beliefs
//   GameEvents.PlayerCanHaveBelief.Add(function(iPlayer, iBelief) return true end)
//   GameEvents.ReligionCanHaveBelief.Add(function(iPlayer, iReligion, iBelief) return true end)
#define MOD_EVENTS_ACQUIRE_BELIEFS                  gCustomMods.isEVENTS_ACQUIRE_BELIEFS()

// Events sent to see if religion missions are valid (v46)
//   GameEvents.PlayerCanSpreadReligion.Add(function(iPlayer, iUnit, iPlotX, iPlotY) return true end)
//   GameEvents.PlayerCanRemoveHeresy.Add(function(iPlayer, iUnit, iPlotX, iPlotY) return true end)
#define MOD_EVENTS_RELIGION                         gCustomMods.isEVENTS_RELIGION()

// Events sents on espionage outcomes (v63)
//   GameEvents.EspionageResult.Add(function(iPlayer, iSpy, iResult, iCityX, iCityY) end)
//   GameEvents.EspionageState.Add(function(iPlayer, iSpy, iState, iCityX, iCityY) end)
//   GameEvents.EspionageCanMoveSpyTo.Add(function(iPlayer, iCityOwner, iCity) return true)
//   GameEvents.EspionageCanStageCoup.Add(function(iPlayer, iCityOwner, iCity) return true)
#define MOD_EVENTS_ESPIONAGE                         gCustomMods.isEVENTS_ESPIONAGE()

// Event sent to ascertain if a unit can start a paradrop from this tile
//   GameEvents.CanParadropFrom.Add(function(iPlayer, iUnit, iPlotX, iPlotY) return false end)
//   GameEvents.CannotParadropFrom.Add(function(iPlayer, iUnit, iPlotX, iPlotY) return false end)
//   GameEvents.ParadropAt.Add(function(iPlayer, iUnit, iFromX, iFromY, iToX, iToY) end)
#define MOD_EVENTS_PARADROPS                        gCustomMods.isEVENTS_PARADROPS()

// Event sent when a unit is created (v46)
//   GameEvents.UnitCreated.Add(function(iPlayer, iUnit, iUnitType, iPlotX, iPlotY) end)
#define MOD_EVENTS_UNIT_CREATED                     gCustomMods.isEVENTS_UNIT_CREATED()

// Event sent when a unit founds a city (v58)
//   GameEvents.UnitCityFounded.Add(function(iPlayer, iUnit, iUnitType, iPlotX, iPlotY) end)
#define MOD_EVENTS_UNIT_FOUNDED                     gCustomMods.isEVENTS_UNIT_FOUNDED()

// Event sent just before a unit is killed (via CvUnit::kill()) (v22)
//   GameEvents.UnitPrekill.Add(function(iPlayer, iUnit, iUnitType, iX, iY, bDelay, iByPlayer) end)
#define MOD_EVENTS_UNIT_PREKILL                     gCustomMods.isEVENTS_UNIT_PREKILL()

// Event sent as a unit is captured (v46)
//   GameEvents.UnitCaptured.Add(function(iByPlayer, iByUnit, iCapturedPlayer, iCapturedUnit, bWillBeKilled, iReason) end) (v69)
//   iReason - 0=Move, 1=Combat, 2=Trait (barb land), 3=Trait (barb naval), 4=Belief (barb)
//   GameEvents.UnitCaptureType.Add(function(iPlayer, iUnit, iUnitType, iByCiv) return iCaptureUnitType; end)
#define MOD_EVENTS_UNIT_CAPTURE                     gCustomMods.isEVENTS_UNIT_CAPTURE()

// Events sent as units perform actions (v86)
//   GameEvents.UnitCanPillage.Add(function(iPlayer, iUnit, iImprovement, iRoute) return true end)
//   GameEvents.UnitPillageGold.Add(function(iPlayer, iUnit, iImprovement, iGold) return iGold end)
#define MOD_EVENTS_UNIT_ACTIONS                     gCustomMods.isEVENTS_UNIT_ACTIONS()

// Events sent as units are promoted/upgraded
//   GameEvents.CanHavePromotion.Add(function(iPlayer, iUnit, iPromotionType) return true end)
//   GameEvents.UnitCanHavePromotion.Add(function(iPlayer, iUnit, iPromotionType) return true end)
//   GameEvents.UnitPromoted.Add(function(iPlayer, iUnit, iPromotionType) end)
//   GameEvents.CanHaveAnyUpgrade.Add(function(iPlayer, iUnit) return true end)
//   GameEvents.UnitCanHaveAnyUpgrade.Add(function(iPlayer, iUnit) return true end)
//   GameEvents.CanHaveUpgrade.Add(function(iPlayer, iUnit, iUnitClassType, iUnitType) return true end)
//   GameEvents.UnitCanHaveUpgrade.Add(function(iPlayer, iUnit, iUnitClassType, iUnitType) return true end)
//   GameEvents.UnitUpgraded.Add(function(iPlayer, iOldUnit, iNewUnit, bGoodyHut) end)
#define MOD_EVENTS_UNIT_UPGRADES                    gCustomMods.isEVENTS_UNIT_UPGRADES()

// Events sent as units are created (v51)
//   GameEvents.UnitCanHaveName.Add(function(iPlayer, iUnit, iName) return true end)
//   GameEvents.UnitCanHaveGreatWork.Add(function(iPlayer, iUnit, iGreatWork) return true end)
#define MOD_EVENTS_UNIT_DATA                        gCustomMods.isEVENTS_UNIT_DATA()

// Event sent to ascertain if a unit can move into a given plot - VERY, VERY CPU INTENSIVE!!!
//   GameEvents.CanMoveInto.Add(function(iPlayer, iUnit, iPlotX, iPlotY, bAttack, bDeclareWar) return true end)
#define MOD_EVENTS_CAN_MOVE_INTO                    gCustomMods.isEVENTS_CAN_MOVE_INTO()

// Event sent as ascertain if a trade route can be created (v68)
//   GameEvents.PlayerCanCreateTradeRoute.Add(function(iFromPlayer, iFromCity, iToPlayer, iToCity, iDomain, iConnectionType) return true end)
#define MOD_EVENTS_TRADE_ROUTES                     gCustomMods.isEVENTS_TRADE_ROUTES()

// Event sent as a trade route is plundered (v52)
//   GameEvents.PlayerPlunderedTradeRoute.Add(function(iPlayer, iUnit, iPlunderedGold, iFromPlayer, iFromCity, iToPlayer, iToCity, iRouteType, iRouteDomain) end)
#define MOD_EVENTS_TRADE_ROUTE_PLUNDERED            gCustomMods.isEVENTS_TRADE_ROUTE_PLUNDERED()

// Event sent when a nuke is fired
//   GameEvents.NuclearDetonation.Add(function(iPlayer, iX, iY, bWar, bBystanders) end)
#define MOD_EVENTS_NUCLEAR_DETONATION               gCustomMods.isEVENTS_NUCLEAR_DETONATION()

// Events sent about war and peace
//   GameEvents.PlayerCanDeclareWar.Add(function(iPlayer, iAgainstTeam) return true end)
//   GameEvents.IsAbleToDeclareWar.Add(function(iPlayer, iAgainstTeam) return true end) - deprecated, use PlayerCanDeclareWar
//   GameEvents.DeclareWar.Add(function(iPlayer, iAgainstTeam, bAggressor) end)
//   GameEvents.PlayerCanMakePeace.Add(function(iPlayer, iAgainstTeam) return true end)
//   GameEvents.IsAbleToMakePeace.Add(function(iPlayer, iAgainstTeam) return true end) - deprecated, use PlayerCanMakePeace
//   GameEvents.MakePeace.Add(function(iPlayer, iAgainstTeam, bPacifier) end)
#define MOD_EVENTS_WAR_AND_PEACE                    gCustomMods.isEVENTS_WAR_AND_PEACE()

// Events sent about resolutions (v51)
//   GameEvents.PlayerCanPropose.Add(function(iPlayer, iResolution, iChoice, bEnact) return true end)
//   GameEvents.ResolutionProposing.Add(function(iPlayer, iLeague) return false; end) (v88)
//   GameEvents.ResolutionVoting.Add(function(iPlayer, iLeague) return false; end) (v88)
//   GameEvents.ResolutionResult.Add(function(iResolution, iProposer, iChoice, bEnact, bPassed) end)
#define MOD_EVENTS_RESOLUTIONS                      gCustomMods.isEVENTS_RESOLUTIONS()

// Events sent about ideologies and tenets (v51)
//   GameEvents.PlayerCanAdoptIdeology.Add(function(iPlayer, iIdeology) return true end)
//   GameEvents.PlayerCanAdoptTenet.Add(function(iPlayer, iTenet) return true end)
#define MOD_EVENTS_IDEOLOGIES                       gCustomMods.isEVENTS_IDEOLOGIES()

// Events sent by plots (v30)
//   GameEvents.PlayerCanBuild.Add(function(iPlayer, iUnit, iX, iY, iBuild) return true end)
//   GameEvents.PlotCanImprove.Add(function(iX, iY, iImprovement) return true end)
//   GameEvents.PlayerBuilding.Add(function(iPlayer, iUnit, iX, iY, iBuild, bStarting) end) (v46)
//   GameEvents.PlayerBuilt.Add(function(iPlayer, iUnit, iX, iY, iBuild) end) (v46)
#define MOD_EVENTS_PLOT                             gCustomMods.isEVENTS_PLOT()

// Events sent by golden ages (v52)
//   GameEvents.PlayerGoldenAge.Add(function(iPlayer, bStart, iTurns) end)
//   GameEvents.PlayerEndOfMayaLongCount.Add(function(iPlayer, iBaktun, iBaktunPreviousTurn) end)
#define MOD_EVENTS_GOLDEN_AGE                       gCustomMods.isEVENTS_GOLDEN_AGE()

// Events sent after a city produces/buys something
//   GameEvents.CityTrained.Add(function(iPlayer, iCity, iUnit, bGold, bFaith) end)
//   GameEvents.CityConstructed.Add(function(iPlayer, iCity, iBuilding, bGold, bFaith) end)
//   GameEvents.CityCreated.Add(function(iPlayer, iCity, iProject, bGold, bFaith) end)
//   GameEvents.CityPrepared.Add(function(iPlayer, iCity, iSpecialist, iAmount, iNeeded) end) (v33)
//   GameEvents.CityBoughtPlot.Add(function(iPlayer, iCity, iPlotX, iPlotY, bGold, bCulture) end)
//   GameEvents.CitySoldBuilding.Add(function(iPlayer, iCity, iBuilding) end)
#define MOD_EVENTS_CITY                             gCustomMods.isEVENTS_CITY()

// Event sent after a civilization moves their capital (v73)
//   GameEvents.CapitalChanged.Add(function(iPlayer, iNewCapital, iOldCapital) end)
#define MOD_EVENTS_CITY_CAPITAL                     gCustomMods.isEVENTS_CITY_CAPITAL()

// Event sent to ascertain if a city can acquire a plot (v20)
//   GameEvents.CityCanAcquirePlot.Add(function(iPlayer, iCity, iPlotX, iPlotY) return true end)
#define MOD_EVENTS_CITY_BORDERS                     gCustomMods.isEVENTS_CITY_BORDERS()
	
// Event sent about city founding (v68)
//   GameEvents.PlayerCanFoundCity.Add(function(iPlayer, iPlotX, iPlotY) return true end)
//   GameEvents.PlayerCanFoundCityRegardless.Add(function(iPlayer, iPlotX, iPlotY) return false end)
#define MOD_EVENTS_CITY_FOUNDING                    gCustomMods.isEVENTS_CITY_FOUNDING()
	
// Event sent to ascertain if a player can liberate another player (v68)
//   GameEvents.PlayerCanLiberate.Add(function(iPlayer, iOtherPlayer) return true end)
//   GameEvents.PlayerLiberated.Add(function(iPlayer, iOtherPlayer, iCity) end)
#define MOD_EVENTS_LIBERATION                       gCustomMods.isEVENTS_LIBERATION()
	
// Event sent to ascertain if a player can over-ride the standard razing rules for the specified city and raze it anyway (v20)
//   GameEvents.PlayerCanRaze.Add(function(iPlayer, iCity) return false end)
#define MOD_EVENTS_CITY_RAZING                      gCustomMods.isEVENTS_CITY_RAZING()
	
// Event sent to ascertain if a city can perform airlifts (v68)
//   GameEvents.CityCanAirlift.Add(function(iPlayer, iCity) return false end)
#define MOD_EVENTS_CITY_AIRLIFT                     gCustomMods.isEVENTS_CITY_AIRLIFT()

// Events sent to ascertain the bombard range for a city, and if indirect fire is allowed (v32)
//   GameEvents.GetBombardRange.Add(function(iPlayer, iCity) return (-1 * GameDefines.CITY_ATTACK_RANGE) end)
#define MOD_EVENTS_CITY_BOMBARD                     gCustomMods.isEVENTS_CITY_BOMBARD()

// Events sent to ascertain if one city is connected to another (v33)
//   GameEvents.CityConnections.Add(function(iPlayer, bDomestic) return false end)
//   GameEvents.CityConnected.Add(function(iPlayer, iCityX, iCityY, iToCityX, iToCityY, bDomestic) return false end)
#define MOD_EVENTS_CITY_CONNECTIONS                 gCustomMods.isEVENTS_CITY_CONNECTIONS()

// Events sent to ascertain if an area can have civ specific resources and to place those resources (v20)
//   GameEvents.AreaCanHaveAnyResource.Add(function(iPlayer, iArea) return true end)
//   GameEvents.PlaceResource.Add(function(iPlayer, iResource, iCount, iPlotX, iPlotY) end)
#define MOD_EVENTS_AREA_RESOURCES                   gCustomMods.isEVENTS_AREA_RESOURCES()

// Events sent to ascertain if a unit can airlift from/to a specific plot (v74)
//   GameEvents.CanAirliftFrom.Add(function(iPlayer, iUnit, iPlotX, iPlotY) return false end)
//   GameEvents.CanAirliftTo.Add(function(iPlayer, iUnit, iPlotX, iPlotY) return false end)
#define MOD_EVENTS_AIRLIFT                           gCustomMods.isEVENTS_AIRLIFT()

// Events sent to ascertain if a unit can rebase to a specific plot (either a city or a carrier)
//   GameEvents.CanLoadAt.Add(function(iPlayer, iUnit, iPlotX, iPlotY) return false end)
//   GameEvents.CanRebaseInCity.Add(function(iPlayer, iUnit, iPlotX, iPlotY) return false end)
//   GameEvents.CanRebaseTo.Add(function(iPlayer, iUnit, iPlotX, iPlotY, bIsCity) return false end)
//   GameEvents.RebaseTo.Add(function(iPlayer, iUnit, iPlotX, iPlotY) end)
#define MOD_EVENTS_REBASE                           gCustomMods.isEVENTS_REBASE()

// Event sent to see if a command is valid (v46)
//   GameEvents.CanDoCommand.Add(function(iPlayer, iUnit, iCommand, iData1, iData2, iPlotX, iPlotY, bTestVisible) return true end)
#define MOD_EVENTS_COMMAND                          gCustomMods.isEVENTS_COMMAND()

// Events sent for custom missions (v46)
//   CUSTOM_MISSION_NO_ACTION       = 0
//   CUSTOM_MISSION_ACTION          = 1
//   CUSTOM_MISSION_DONE            = 2
//   CUSTOM_MISSION_ACTION_AND_DONE = 3
//   GameEvents.CustomMissionPossible.Add(function(iPlayer, iUnit, iMission, iData1, iData2, iFlags=0, iTurn=-1, iPlotX, iPlotY, bTestVisible) return false end)
//   GameEvents.CustomMissionStart.Add(function(iPlayer, iUnit, iMission, iData1, iData2, iFlags, iTurn) return CUSTOM_MISSION_ACTION end)
//   GameEvents.CustomMissionSetActivity.Add(function(iPlayer, iUnit, iMission, iData1, iData2, iFlags, iTurn) return CUSTOM_MISSION_ACTION_AND_DONE end)
//   GameEvents.CustomMissionDoStep.Add(function(iPlayer, iUnit, iMission, iData1, iData2, iFlags, iTurn) return CUSTOM_MISSION_ACTION_AND_DONE end)
//   GameEvents.CustomMissionCompleted.Add(function(iPlayer, iUnit, iMission, iData1, iData2, iFlags, iTurn) return false end)
//   GameEvents.CustomMissionTargetPlot.Add(function(iPlayer, iUnit, iMission, iData1, iData2, iFlags, iTurn) return iPlotIndex end)
//   GameEvents.CustomMissionCameraTime.Add(function(iPlayer, iUnit, iMission, iData1, iData2, iFlags, iTurn) return iCameraTime end) -- iCameraTime is 0, 1, 5 or 10
//   GameEvents.CustomMissionTimerInc.Add(function(iPlayer, iUnit, iMission, iData1, iData2, iFlags, iTurn) return iTimerInc end)
#define MOD_EVENTS_CUSTOM_MISSIONS                  gCustomMods.isEVENTS_CUSTOM_MISSIONS()

// Events sent during combat (v68)
//   GameEvents.BattleStarted.Add(function(iType, iPlotX, iPlotY) end)
//   GameEvents.BattleJoined.Add(function(iPlayer, iUnitOrCity, iRole, bIsCity) end)
//   GameEvents.BattleFinished.Add(function() end)
#define MOD_EVENTS_BATTLES                          gCustomMods.isEVENTS_BATTLES()
//   GameEvents.BattleDamageDelta.Add(function(iRole, iBaseDamage) return 0 end)
#define MOD_EVENTS_BATTLES_DAMAGE                   (MOD_EVENTS_BATTLES && gCustomMods.isEVENTS_BATTLES_DAMAGE())

// Events generated by the RED (by Gedemon) dll mod code
//   Turn   --> PlayerEndTurnInitiated, PlayerEndTurnCompleted, TurnComplete
//   Combat --> PushingMissionTo, MustAbortAttack, CombatResult, CombatEnded
#define MOD_EVENTS_RED_TURN                         gCustomMods.isEVENTS_RED_TURN()
#define MOD_EVENTS_RED_COMBAT                       gCustomMods.isEVENTS_RED_COMBAT()
#define MOD_EVENTS_RED_COMBAT_MISSION               (MOD_EVENTS_RED_COMBAT && gCustomMods.isEVENTS_RED_COMBAT_MISSION())
#define MOD_EVENTS_RED_COMBAT_ABORT                 (MOD_EVENTS_RED_COMBAT && gCustomMods.isEVENTS_RED_COMBAT_ABORT())
#define MOD_EVENTS_RED_COMBAT_RESULT                (MOD_EVENTS_RED_COMBAT && gCustomMods.isEVENTS_RED_COMBAT_RESULT())
#define MOD_EVENTS_RED_COMBAT_ENDED                 (MOD_EVENTS_RED_COMBAT && gCustomMods.isEVENTS_RED_COMBAT_ENDED())

// Minor bug fixes (missing catch-all else clauses, etc) (v30 onwards)
#define MOD_BUGFIX_MINOR 							(true)
// Minor bug fixes in the Lua API (v86 onwards)
#define MOD_BUGFIX_LUA_API 							(true)
// Adds validation to setting plot values (type, terrain, feature, resource, improvement and route) to stop CTDs in the graphics engine (v86)
#define MOD_BUGFIX_PLOT_VALIDATION					(true)
// Fixes some minor issues with the random number generator (v83)
#define MOD_BUGFIX_RANDOM							(true)
// Fixes the spy name crash (v53)
#define MOD_BUGFIX_SPY_NAMES                        (true)
#define MOD_BUGFIX_RESEARCH_OVERFLOW                gCustomMods.isBUGFIX_RESEARCH_OVERFLOW()
// Fixes the bug where a city doesn't work its centre tile (v45)
#define MOD_BUGFIX_CITY_CENTRE_WORKING              (true)
// Fixes the bug of creating a capital from a puppet city, and leaving it that way (v73)
#define MOD_BUGFIX_NO_PUPPET_CAPITALS               (true)
// Adds missing policy events when adopting an ideology (v33)
#define MOD_BUGFIX_MISSING_POLICY_EVENTS			(true)
// Fixes trade routes sticking to coastal water when the player has the EmbarkAllWater trait (v33)
#define MOD_BUGFIX_TRADE_ROUTES_EMBARK_ALL_WATER	(true)
// Fixes the bug where Venice puppets it's own capital (v42)
#define MOD_BUGFIX_VENICE_PUPPETS_CAPITAL			(true)
// Fixes the bug in the Lua Plot:ChangeVisibilityCount() method where iChange is treated as a boolean and not a signed int (v23)
#define MOD_BUGFIX_LUA_CHANGE_VISIBILITY_COUNT      gCustomMods.isBUGFIX_LUA_CHANGE_VISIBILITY_COUNT()
// Fixes the bug that excludes spy pressure (Underground Sects) from the city banner tooltip display (v45)
#define MOD_BUGFIX_RELIGIOUS_SPY_PRESSURE           gCustomMods.isBUGFIX_RELIGIOUS_SPY_PRESSURE()
// Fixes the CanMoveAfterPurchase() bug where it is only tested for at one specific point in the code (v26)
#define MOD_BUGFIX_MOVE_AFTER_PURCHASE              gCustomMods.isBUGFIX_MOVE_AFTER_PURCHASE()
// Fixes the issues caused by using UNIT_XYZ instead of UNITCLASS_XYZ (v26)
#define MOD_BUGFIX_UNITCLASS_NOT_UNIT               gCustomMods.isBUGFIX_UNITCLASS_NOT_UNIT()
// Fixes the issues caused by using BUILDING_XYZ instead of BUILDINGCLASS_XYZ (v26)
#define MOD_BUGFIX_BUILDINGCLASS_NOT_BUILDING       gCustomMods.isBUGFIX_BUILDINGCLASS_NOT_BUILDING()
// Fixes the NumCitiesFreeFoodBuilding (policy finisher) bug where the civilization has a UB for the Aqueduct - AFFECTS SAVE GAME DATA FORMAT
#define MOD_BUGFIX_FREE_FOOD_BUILDING               gCustomMods.isBUGFIX_FREE_FOOD_BUILDING()
// Fixes the bug where the naval Civilization_FreeUnits start on land
#define MOD_BUGFIX_NAVAL_FREE_UNITS                 gCustomMods.isBUGFIX_NAVAL_FREE_UNITS()
// Fixes the bug where the naval units jump to the nearest city and not the nearest available non-lake water plot
#define MOD_BUGFIX_NAVAL_NEAREST_WATER              gCustomMods.isBUGFIX_NAVAL_NEAREST_WATER()
// Fixes the bug where stacked ranged units may attack out of cities but melee units may not
#define MOD_BUGFIX_CITY_STACKING                    gCustomMods.isBUGFIX_CITY_STACKING()
// Fixes the bug in goody hut messages that have parameters (v38)
#define MOD_BUGFIX_GOODY_HUT_MESSAGES               (true)
// Fixes the bug where Barb Camps ignore the ValidTerrains and ValidFeatures tables
#define MOD_BUGFIX_BARB_CAMP_TERRAINS               gCustomMods.isBUGFIX_BARB_CAMP_TERRAINS()
// Fixes the bug where Barb Camps won't spawn units if they are added via pPlot:SetImprovementType() (v21)
#define MOD_BUGFIX_BARB_CAMP_SPAWNING               gCustomMods.isBUGFIX_BARB_CAMP_SPAWNING()
// Fixes the bug where ranged combat (archers, catapults, ships and planes) against barbarians generates Great People XP (v43)
#define MOD_BUGFIX_BARB_GP_XP                       gCustomMods.isBUGFIX_BARB_GP_XP()
// Fixes the bug where you can't remove roads in no-mans-land originally built by a now dead player
#define MOD_BUGFIX_REMOVE_GHOST_ROUTES              gCustomMods.isBUGFIX_REMOVE_GHOST_ROUTES()
// Fixes healing units ignoring enemy units and sleeping units under direct fire remaining asleep - thanks to hulkster for highlighting the latter issue
#define MOD_BUGFIX_UNITS_AWAKE_IN_DANGER            gCustomMods.isBUGFIX_UNITS_AWAKE_IN_DANGER()
// Fixes workers stopping what they are doing at any hint of danger to only when they can see an enemy unit
#define MOD_BUGFIX_WORKERS_VISIBLE_DANGER           gCustomMods.isBUGFIX_WORKERS_VISIBLE_DANGER()
// Fixes the hard-coding of what builds remove which features (v45)
#define MOD_BUGFIX_FEATURE_REMOVAL                  gCustomMods.isBUGFIX_FEATURE_REMOVAL()
// Fixes the bug in calculating AA interception strength which takes terrain into account
#define MOD_BUGFIX_INTERCEPTOR_STRENGTH             gCustomMods.isBUGFIX_INTERCEPTOR_STRENGTH()
// Fixes the very dodgy maths in the calculation of a unit's power
#define MOD_BUGFIX_UNIT_POWER_CALC                  gCustomMods.isBUGFIX_UNIT_POWER_CALC()
// Fixes the Anti-Air non-domain bonuses issue, requires BUGFIX_UNIT_POWER_CALC to be enabled
#define MOD_BUGFIX_UNIT_POWER_BONUS_VS_DOMAIN_ONLY  gCustomMods.isBUGFIX_UNIT_POWER_BONUS_VS_DOMAIN_ONLY()
// Fixes the naval imbalance in a unit's power calculation, requires BUGFIX_UNIT_POWER_CALC to be enabled
#define MOD_BUGFIX_UNIT_POWER_NAVAL_CONSISTENCY     gCustomMods.isBUGFIX_UNIT_POWER_NAVAL_CONSISTENCY()
// Fixes the bug where units can upgrade even without any pre-req project being available (v22)
#define MOD_BUGFIX_UNIT_PREREQ_PROJECT              gCustomMods.isBUGFIX_UNIT_PREREQ_PROJECT()
// Fixes a bug where hovering units can be chosen as rebels! (v39)
#define MOD_BUGFIX_NO_HOVERING_REBELS               gCustomMods.isBUGFIX_NO_HOVERING_REBELS()
// Fixes some bugs/regressions that disable the effect of IsNoMinorCivs of some strategies
#define MOD_BUGFIX_MINOR_CIV_STRATEGIES				gCustomMods.isBUGFIX_MINOR_CIV_STRATEGIES()

#endif // ACHIEVEMENT_HACKS

//
// NOTHING BELOW HERE SHOULD NEED CHANGING
//

// Terraforming event types
enum TerraformingEventTypes {
	NO_TERRAFORMINGEVENT = -1,
	TERRAFORMINGEVENT_LOAD,
	TERRAFORMINGEVENT_AREA,
	TERRAFORMINGEVENT_LANDMASS,
	TERRAFORMINGEVENT_CONTINENT,
	TERRAFORMINGEVENT_PLOT,
	TERRAFORMINGEVENT_TERRAIN,
	TERRAFORMINGEVENT_FEATURE,
	TERRAFORMINGEVENT_RIVER,
	TERRAFORMINGEVENT_CITY,
	NUM_TERRAFORMINGEVENT_TYPES
};


// Battle event macros
enum BattleTypeTypes
{
	BATTLE_TYPE_MELEE,
	BATTLE_TYPE_RANGED,
	BATTLE_TYPE_AIR,
	BATTLE_TYPE_SWEEP,
	BATTLE_TYPE_PARADROP,
	BATTLE_TYPE_NUKE
};

#if defined(MOD_EVENTS_BATTLES)
#define BATTLE_STARTED(iType, pPlot)              if (MOD_EVENTS_BATTLES) { GAMEEVENTINVOKE_HOOK(GAMEEVENT_BattleStarted, iType, pPlot.getX(), pPlot.getY()); }
#define BATTLE_JOINED(pCombatant, iRole, bIsCity) if (MOD_EVENTS_BATTLES && pCombatant) { GAMEEVENTINVOKE_HOOK(GAMEEVENT_BattleJoined, (pCombatant)->getOwner(), (pCombatant)->GetID(), iRole, bIsCity); }
#define BATTLE_FINISHED()                         if (MOD_EVENTS_BATTLES) { GAMEEVENTINVOKE_HOOK(GAMEEVENT_BattleFinished); }
#else
#define BATTLE_STARTED(pPlot)            __noop
#define BATTLE_JOINED(pCombatant, iRole) __noop
#define BATTLE_FINISHED()                __noop
#endif
// Custom mod logger
#if defined(CUSTOMLOGDEBUG)
#if defined(CUSTOMLOGFILEINFO) && defined(CUSTOMLOGFUNCINFO)
#define CUSTOMLOG(sFmt, ...) {																					\
	CvString sMsg; CvString::format(sMsg, sFmt, __VA_ARGS__);													\
	CvString sLine; CvString::format(sLine, "%s[%i]: %s - %s", __FILE__, __LINE__, __FUNCTION__, sMsg.c_str());	\
	LOGFILEMGR.GetLog(CUSTOMLOGDEBUG, FILogFile::kDontTimeStamp)->Msg(sLine.c_str());							\
}
#endif
#if defined(CUSTOMLOGFILEINFO) && !defined(CUSTOMLOGFUNCINFO)
#define CUSTOMLOG(sFmt, ...) {																					\
	CvString sMsg; CvString::format(sMsg, sFmt, __VA_ARGS__);													\
	CvString sLine; CvString::format(sLine, "%s[%i] - %s", __FILE__, __LINE__, sMsg.c_str());					\
	LOGFILEMGR.GetLog(CUSTOMLOGDEBUG, FILogFile::kDontTimeStamp)->Msg(sLine.c_str());							\
}
#endif
#if !defined(CUSTOMLOGFILEINFO) && defined(CUSTOMLOGFUNCINFO)
#define CUSTOMLOG(sFmt, ...) {																					\
	CvString sMsg; CvString::format(sMsg, sFmt, __VA_ARGS__);													\
	CvString sLine; CvString::format(sLine, "%s - %s", __FUNCTION__, sMsg.c_str());								\
	LOGFILEMGR.GetLog(CUSTOMLOGDEBUG, FILogFile::kDontTimeStamp)->Msg(sLine.c_str());							\
}
#endif
#if !defined(CUSTOMLOGFILEINFO) && !defined(CUSTOMLOGFUNCINFO)
#define CUSTOMLOG(sFmt, ...) {																					\
	CvString sMsg; CvString::format(sMsg, sFmt, __VA_ARGS__);													\
	LOGFILEMGR.GetLog(CUSTOMLOGDEBUG, FILogFile::kDontTimeStamp)->Msg(sMsg.c_str());							\
}
#endif
#else
#define CUSTOMLOG(sFmt, ...) __noop
#endif

// Unified yields logger
#if defined(UNIFIEDLOGDEBUG)
#define UNIFIEDLOG(sFmt, ...) {																		\
	CvString sMsg; CvString::format(sMsg, sFmt, __VA_ARGS__);										\
	CvString sLine; CvString::format(sLine, "%s[%i] - %s", __FILE__, __LINE__, sMsg.c_str());	    \
	LOGFILEMGR.GetLog(UNIFIEDLOGDEBUG, FILogFile::kDontTimeStamp)->Msg(sLine.c_str());			    \
}
#else
#define UNIFIEDLOG(sFmt, ...) __noop
#endif


// Message wrappers
#define SHOW_PLAYER_MESSAGE(pPlayer, szMessage)       if (pPlayer) DLLUI->AddMessage(0, pPlayer->GetID(), false, GC.getEVENT_MESSAGE_TIME(), szMessage)
#define SHOW_CITY_MESSAGE(pCity, ePlayer, szMessage)  if (pCity) DLLUI->AddCityMessage(0, pCity->GetIDInfo(), ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szMessage)
#define SHOW_UNIT_MESSAGE(pUnit, ePlayer, szMessage)  if (pUnit) DLLUI->AddUnitMessage(0, pUnit->GetIDInfo(), ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szMessage)
#define SHOW_PLOT_MESSAGE(pPlot, ePlayer, szMessage)  if (pPlot) DLLUI->AddPlotMessage(0, pPlot->GetPlotIndex(), ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szMessage)
#define SHOW_PLOT_POPUP(pPlot, ePlayer, szMessage, fDelay)  if (pPlot) pPlot->showPopupText(ePlayer, szMessage)

// GlobalDefines wrappers
#define GD_INT_DECL(name)         int m_i##name
#define GD_INT_DEF(name)          inline int get##name() { return m_i##name; }
#define GD_INT_INIT(name, def)    m_i##name(def)
#define GD_INT_CACHE(name)        m_i##name = getDefineINT(#name); CUSTOMLOG("<Defines>: %s = %i", #name, m_i##name)
#define GD_INT_GET(name)          GC.get##name()

#define GD_FLOAT_DECL(name)       float m_f##name
#define GD_FLOAT_DEF(name)        inline float get##name() { return m_f##name; }
#define GD_FLOAT_INIT(name, def)  m_f##name(def)
#define GD_FLOAT_CACHE(name)      m_f##name = getDefineFLOAT(#name); CUSTOMLOG("<Defines>: %s = %f", #name, m_f##name)
#define GD_FLOAT_GET(name)        GC.get##name()


// LUA API wrappers
#define LUAAPIEXTN(method, type, ...) static int l##method(lua_State* L)
#define LUAAPIIMPL(object, method) int CvLua##object::l##method(lua_State* L) { return BasicLuaMethod(L, &Cv##object::##method); }
#define LUAAPIINLINE(method, hasMethod, type) inline bool method() const { return hasMethod(type); }


// Game Event wrappers
#include "Lua\CvLuaSupport.h"
#define GAMEEVENTINVOKE_HOOK    gCustomMods.eventHook
#define GAMEEVENTINVOKE_TESTANY gCustomMods.eventTestAny
#define GAMEEVENTINVOKE_TESTALL gCustomMods.eventTestAll
#define GAMEEVENTINVOKE_VALUE   gCustomMods.eventAccumulator

#define GAMEEVENTRETURN_NONE  -1
#define GAMEEVENTRETURN_FALSE  0
#define GAMEEVENTRETURN_TRUE   1
#define GAMEEVENTRETURN_HOOK   GAMEEVENTRETURN_TRUE
#define GAMEEVENTRETURN_VALUE  GAMEEVENTRETURN_TRUE

// Pairs of event names and (optional) parameter types
//    iii means three int params
//    ibi means an int, then a bool, then a final int
//    s means a string (char*) and is only allowed as the last parameter
#define GAMEEVENT_AiOverrideChooseNextTech		"AiOverrideChooseNextTech",		"ib"
#define GAMEEVENT_AreaCanHaveAnyResource		"AreaCanHaveAnyResource",		"ii"
#define GAMEEVENT_BarbariansCanFoundCamp		"BarbariansCanFoundCamp",		"ii"
#define GAMEEVENT_BarbariansCampFounded			"BarbariansCampFounded",		"ii"
#define GAMEEVENT_BarbariansCampCleared			"BarbariansCampCleared",		"iii"
#define GAMEEVENT_BarbariansCampCanSpawnUnit	"BarbariansCampCanSpawnUnit",	"ii"
#define GAMEEVENT_BarbariansCampGetSpawnUnit	"BarbariansCampGetSpawnUnit",	"iii"
#define GAMEEVENT_BarbariansSpawnedUnit			"BarbariansSpawnedUnit",		"iii"
#define GAMEEVENT_BattleDamageDelta				"BattleDamageDelta",			"ii"
#define GAMEEVENT_BattleFinished				"BattleFinished",				""
#define GAMEEVENT_BattleJoined					"BattleJoined",					"iiib"
#define GAMEEVENT_BattleStarted					"BattleStarted",				"iii"
#define GAMEEVENT_CanAirliftFrom				"CanAirliftFrom",				"iiii"
#define GAMEEVENT_CanAirliftTo					"CanAirliftTo",					"iiii"
#define GAMEEVENT_CanDoCommand					"CanDoCommand",					"iiiiiiib"
#define GAMEEVENT_CanHaveAnyUpgrade				"CanHaveAnyUpgrade",			"ii"
#define GAMEEVENT_CanHavePromotion				"CanHavePromotion",				"iii"
#define GAMEEVENT_CanHaveUpgrade				"CanHaveUpgrade",				"iiii"
#define GAMEEVENT_CanLoadAt						"CanLoadAt",					"iiii"
#define GAMEEVENT_CanMoveInto					"CanMoveInto",					"iiiibb"
#define GAMEEVENT_CannotParadropFrom			"CannotParadropFrom",			"iiii"
#define GAMEEVENT_CanParadropFrom				"CanParadropFrom",				"iiii"
#define GAMEEVENT_CanRebaseInCity				"CanRebaseInCity",				"iiii"
#define GAMEEVENT_CanRebaseTo					"CanRebaseTo",					"iiiib"
#define GAMEEVENT_CapitalChanged				"CapitalChanged",				"iii"
#define GAMEEVENT_CircumnavigatedGlobe			"CircumnavigatedGlobe",			"i"
#define GAMEEVENT_CityBoughtPlot				"CityBoughtPlot",				"iiiibb"
#define GAMEEVENT_CityCanAcquirePlot			"CityCanAcquirePlot",			"iiii"
#define GAMEEVENT_CityCanAirlift				"CityCanAirlift",				"ii"
#define GAMEEVENT_CityConnected					"CityConnected",				"iiiiib"
#define GAMEEVENT_CityPuppeted					"CityPuppeted",					"ii"
#define GAMEEVENT_CityConnections				"CityConnections",				"ib"
#define GAMEEVENT_CityConstructed				"CityConstructed",				"iiibb"
#define GAMEEVENT_CityCreated					"CityCreated",					"iiibb"
#define GAMEEVENT_CityPrepared					"CityPrepared",					"iiiii"
#define GAMEEVENT_CitySoldBuilding				"CitySoldBuilding",				"iii"
#define GAMEEVENT_CityTrained					"CityTrained",					"iiibb"
#define GAMEEVENT_CustomMissionCompleted		"CustomMissionCompleted",		"iiiiiii"
#define GAMEEVENT_CustomMissionCameraTime		"CustomMissionCameraTime",		"iiiiiii"
#define GAMEEVENT_CustomMissionDoStep			"CustomMissionDoStep",			"iiiiiii"
#define GAMEEVENT_CustomMissionPossible			"CustomMissionPossible",		"iiiiiiiiib"
#define GAMEEVENT_CustomMissionStart			"CustomMissionStart",			"iiiiiii"
#define GAMEEVENT_CustomMissionSetActivity		"CustomMissionSetActivity",		"iiiiiii"
#define GAMEEVENT_CustomMissionTargetPlot		"CustomMissionTargetPlot",		"iiiiiii"
#define GAMEEVENT_CustomMissionTimerInc			"CustomMissionTimerInc",		"iiiiiii"
#define GAMEEVENT_DeclareWar					"DeclareWar",					"iib"
#define GAMEEVENT_EspionageCanMoveSpyTo			"EspionageCanMoveSpyTo",		"iii"
#define GAMEEVENT_EspionageCanStageCoup			"EspionageCanStageCoup",		"iii"
#define GAMEEVENT_EspionageResult				"EspionageResult",				"iiiii"
#define GAMEEVENT_ElectionResultSuccess			"ElectionResultSuccess",		"iiiii"
#define GAMEEVENT_ElectionResultFailure			"ElectionResultFailure",		"iiiii"
#define GAMEEVENT_EspionageNotificationData		"EspionageNotificationData",	"iiiiiiiibiibi"
#define GAMEEVENT_EspionageState				"EspionageState",				"iiiii"
#define GAMEEVENT_GetDiploModifier				"GetDiploModifier",				"iii"
#define GAMEEVENT_GetBombardRange				"GetBombardRange",				"ii"
#define GAMEEVENT_GetReligionToFound			"GetReligionToFound",			"iib"
#define GAMEEVENT_GetBeliefToFound				"GetBeliefToFound",				"ii"
#define GAMEEVENT_GoodyHutCanNotReceive			"GoodyHutCanNotReceive",		"iiib"
#define GAMEEVENT_GoodyHutReceivedBonus			"GoodyHutReceivedBonus",		"iiiii"
#define GAMEEVENT_GoodyHutCanResearch			"GoodyHutCanResearch",			"ii"
#define GAMEEVENT_GoodyHutTechResearched		"GoodyHutTechResearched",		"ii"
#define GAMEEVENT_GreatPersonExpended			"GreatPersonExpended",			"iiiii"
#define GAMEEVENT_IsAbleToDeclareWar			"IsAbleToDeclareWar",			"ii"
#define GAMEEVENT_IsAbleToMakePeace				"IsAbleToMakePeace",			"ii"
#define GAMEEVENT_MakePeace						"MakePeace",					"iib"
#define GAMEEVENT_MinorAlliesChanged			"MinorAlliesChanged",			"iibii"
#define GAMEEVENT_MinorFriendsChanged			"MinorFriendsChanged",			"iibii"
#define GAMEEVENT_MinorGift						"MinorGift",					"iiiiibbs"
#define GAMEEVENT_MinorGiftUnit					"MinorGiftUnit",				"iii"
#define GAMEEVENT_NaturalWonderDiscovered		"NaturalWonderDiscovered",		"iiiibii"
#define GAMEEVENT_NuclearDetonation				"NuclearDetonation",			"iiibb"
#define GAMEEVENT_PantheonFounded				"PantheonFounded",				"iiii"
#define GAMEEVENT_ParadropAt					"ParadropAt",					"iiiiii"
#define GAMEEVENT_PlaceResource					"PlaceResource",				"iiiii"
#define GAMEEVENT_PlayerBoughtOut				"PlayerBoughtOut",				"ii"
#define GAMEEVENT_PlayerBuilding				"PlayerBuilding",				"iiiiib"
#define GAMEEVENT_PlayerBuilt					"PlayerBuilt",					"iiiii"
#define GAMEEVENT_PlayerBullied					"PlayerBullied",				"iiiiii"
#define GAMEEVENT_PlayerCanAdoptIdeology		"PlayerCanAdoptIdeology",		"ii"
#define GAMEEVENT_PlayerCanAdoptTenet			"PlayerCanAdoptTenet",			"ii"
#define GAMEEVENT_PlayerCanBuild				"PlayerCanBuild",				"iiiii"
#define GAMEEVENT_PlayerCanBullyGold			"PlayerCanBullyGold",			"ii"
#define GAMEEVENT_PlayerCanBullyUnit			"PlayerCanBullyUnit",			"ii"
#define GAMEEVENT_PlayerCanBuyOut				"PlayerCanBuyOut",				"ii"
#define GAMEEVENT_PlayerCanCreateTradeRoute		"PlayerCanCreateTradeRoute",	"iiiiii"
#define GAMEEVENT_PlayerTradeRouteCompleted		"PlayerTradeRouteCompleted",	"iiiiii"
#define GAMEEVENT_PlayerCanDeclareWar			"PlayerCanDeclareWar",			"ii"
#define GAMEEVENT_PlayerCanFoundCity			"PlayerCanFoundCity",			"iii"
#define GAMEEVENT_PlayerCanFoundCityRegardless	"PlayerCanFoundCityRegardless",	"iii"
#define GAMEEVENT_PlayerCanFoundPantheon		"PlayerCanFoundPantheon",		"i"
#define GAMEEVENT_PlayerCanFoundReligion		"PlayerCanFoundReligion",		"ii"
#define GAMEEVENT_PlayerCanGiftGold				"PlayerCanGiftGold",			"ii"
#define GAMEEVENT_PlayerCanGiftImprovement		"PlayerCanGiftImprovement",		"ii"
#define GAMEEVENT_PlayerCanGiftUnit				"PlayerCanGiftUnit",			"iii"
#define GAMEEVENT_PlayerCanHaveBelief			"PlayerCanHaveBelief",			"ii"
#define GAMEEVENT_PlayerCanLiberate				"PlayerCanLiberate",			"ii"
#define GAMEEVENT_PlayerCanMakePeace			"PlayerCanMakePeace",			"ii"
#define GAMEEVENT_PlayerCanPropose				"PlayerCanPropose",				"iiib"
#define GAMEEVENT_PlayerCanProtect				"PlayerCanProtect",				"ii"
#define GAMEEVENT_PlayerCanRaze					"PlayerCanRaze",				"ii"
#define GAMEEVENT_PlayerCanRemoveHeresy			"PlayerCanRemoveHeresy",		"iiii"
#define GAMEEVENT_PlayerCanRevoke				"PlayerCanRevoke",				"ii"
#define GAMEEVENT_PlayerCanSpreadReligion		"PlayerCanSpreadReligion",		"iiii"
#define GAMEEVENT_PlayerCanTransitMinorCity		"PlayerCanTransitMinorCity",	"iiiii"
#define GAMEEVENT_PlayerGifted					"PlayerGifted",					"iiiiii"
#define GAMEEVENT_PlayerGoldenAge				"PlayerGoldenAge",				"ibi"
#define GAMEEVENT_PlayerEndOfMayaLongCount		"PlayerEndOfMayaLongCount",		"iii"
#define GAMEEVENT_PlayerLiberated				"PlayerLiberated",				"iii"
#define GAMEEVENT_PlayerPlunderedTradeRoute		"PlayerPlunderedTradeRoute",	"iiiiiiiii"
#define GAMEEVENT_PlayerProtected				"PlayerProtected",				"ii"
#define GAMEEVENT_PlayerRevoked					"PlayerRevoked",				"iib"
#define GAMEEVENT_PlotCanImprove				"PlotCanImprove",				"iii"
#define GAMEEVENT_RebaseTo						"RebaseTo",						"iiii"
#define GAMEEVENT_ReligionCanHaveBelief			"ReligionCanHaveBelief",		"iii"
#define GAMEEVENT_ReligionEnhanced				"ReligionEnhanced",				"iiii"
#define GAMEEVENT_ReligionFounded				"ReligionFounded",				"iiiiiiii"
#define GAMEEVENT_ReligionReformed				"ReligionReformed",				"iii"
#define GAMEEVENT_ResolutionProposing			"ResolutionProposing",			"ii"
#define GAMEEVENT_ResolutionResult				"ResolutionResult",				"iiibb"
#define GAMEEVENT_ResolutionVoting				"ResolutionVoting",				"ii"
#define GAMEEVENT_TeamSetEra					"TeamSetEra",					"iib"
#define GAMEEVENT_TerraformingMap				"TerraformingMap",				"ii"
#define GAMEEVENT_TerraformingPlot				"TerraformingPlot",				"iiiiiiii"
#define GAMEEVENT_TileFeatureChanged			"TileFeatureChanged",			"iiiii"
#define GAMEEVENT_TileImprovementChanged		"TileImprovementChanged",		"iiiiib"
#define GAMEEVENT_TileOwnershipChanged			"TileOwnershipChanged",			"iiii"
#define GAMEEVENT_TileRevealed					"TileRevealed",					"iiiibii"
#define GAMEEVENT_TileRouteChanged				"TileRouteChanged",				"iiiiib"
#define GAMEEVENT_UiDiploEvent					"UiDiploEvent",					"iiii"
#define GAMEEVENT_UnitCanHaveAnyUpgrade			"UnitCanHaveAnyUpgrade",		"ii"
#define GAMEEVENT_UnitCanHaveGreatWork			"UnitCanHaveGreatWork",			"iii"
#define GAMEEVENT_UnitCanHaveName				"UnitCanHaveName",				"iii"
#define GAMEEVENT_UnitCanHavePromotion			"UnitCanHavePromotion",			"iii"
#define GAMEEVENT_UnitCanHaveUpgrade			"UnitCanHaveUpgrade",			"iiii"
#define GAMEEVENT_UnitCanPillage				"UnitCanPillage",				"iiii"
#define GAMEEVENT_UnitCanTransitMinorCity		"UnitCanTransitMinorCity",		"iiiiii"
#define GAMEEVENT_UnitCaptured					"UnitCaptured",					"iiiibi"
#define GAMEEVENT_UnitCaptureType				"UnitCaptureType",				"iiii"
#define GAMEEVENT_UnitCityFounded				"UnitCityFounded",				"iiiii"
#define GAMEEVENT_UnitCreated					"UnitCreated",					"iiiii"
#define GAMEEVENT_UnitPillageGold				"UnitPillageGold",				"iiii"
#define GAMEEVENT_UnitPrekill					"UnitPrekill",					"iiiiibi"
#define GAMEEVENT_UnitPromoted					"UnitPromoted",					"iii"
#define GAMEEVENT_UnitUpgraded					"UnitUpgraded",					"iiib"
#define GAMEEVENT_IdeologyAdopted				"IdeologyAdopted",				"ii"
#define GAMEEVENT_IdeologySwitched				"IdeologySwitched",				"iii"
//JFD
#define GAMEEVENT_PietyChanged              "PietyChange",                  "iii"
#define GAMEEVENT_PietyRateChanged          "PietyRateChange",              "iii"
#define GAMEEVENT_PlayerSecularizes         "PlayerSecularizes",            "iib"
#define GAMEEVENT_StateReligionAdopted      "PlayerStateReligionAdopted",   "iii"
#define GAMEEVENT_StateReligionChanged      "PlayerStateReligionChanged",   "iii"
#define GAMEEVENT_PlayerAdoptsGovernment    "PlayerAdoptsGovernment",       "ii"
#define GAMEEVENT_ReformCooldownChanges     "ReformCooldownChanges",        "ii"
#define GAMEEVENT_GovernmentCooldownChanges "GovernmentCooldownChanges",    "ii"
#define GAMEEVENT_ReformCooldownRateChanges     "ReformCooldownRateChanges",        "ii"
#define GAMEEVENT_GovernmentCooldownRateChanges "GovernmentCooldownRateChanges",    "ii"
#define GAMEEVENT_GreatWorkCreated          "GreatWorkCreated",             "iii"
#define GAMEEVENT_PlayerAdoptsCurrency		"PlayerAdoptsCurrency", "iii"
#define GAMEEVENT_GetMinorCivAllyBonus		"GetMinorCivAllyBonus", "iiiibb"
#define GAMEEVENT_GetMinorCivFriendBonus	"GetMinorCivFriendBonus", "iiiibb"
//Contracts
#define GAMEEVENT_GetNumContractUnit		"GetNumContractUnit", "ii"
#define GAMEEVENT_GetContractMaintenance	"GetContractMaintenance", "ii"
#define GAMEEVENT_GetContractDuration		"GetContractDuration", "ii"
#define GAMEEVENT_ContractStarted			"ContractStarted", "iiii"
#define GAMEEVENT_ContractEnded				"ContractEnded", "ii"
#define GAMEEVENT_ContractsRefreshed		"ContractsRefreshed", ""
//CID
#define GAMEEVENT_ProvinceLevelChanged      "ProvinceLevelChanged", "iiii"
#define GAMEEVENT_LoyaltyStateChanged       "LoyaltyStateChanged", "iiii"
//Other
#define GAMEEVENT_CityBeginsWLTKD			"CityBeginsWLTKD", "iiii"
#define GAMEEVENT_CityRazed					"CityRazed", "iii"
#define GAMEEVENT_CityInvestedBuilding		"CityInvestedBuilding", "iiii"
#define GAMEEVENT_CityInvestedUnit			"CityInvestedUnit", "iiii"
// City and Player Events
#define GAMEEVENT_EventActivated			"EventActivated", "ii"
#define GAMEEVENT_CityEventActivated		"CityEventActivated", "iii"
#define GAMEEVENT_EventChoiceActivated		"EventChoiceActivated", "ii"
#define GAMEEVENT_CityEventChoiceActivated	"CityEventChoiceActivated", "iii"
#define GAMEEVENT_EventChoiceEnded			"EventChoiceEnded", "ii"
#define GAMEEVENT_CityEventChoiceEnded		"CityEventChoiceEnded", "iii"
#define GAMEEVENT_OverrideAIEventChoice		"OverrideAIEventChoice", "ii"
#define GAMEEVENT_OverrideAICityEventChoice	"OverrideAICityEventChoice", "iii"
#define GAMEEVENT_OverrideAIEvent			"OverrideAIEvent", "ii"
#define GAMEEVENT_OverrideAICityEvent		"OverrideAICityEvent", "iii"
#define GAMEEVENT_CityEventCanActivate		"CityEventCanActivate", "iii"
#define GAMEEVENT_EventCanActivate		    "EventCanActivate", "ii"
#define GAMEEVENT_EventChoiceCanTake		"EventChoiceCanTake", "ii"
#define GAMEEVENT_CityEventChoiceCanTake    "CityEventChoiceCanTake", "iii"
#define GAMEEVENT_EventCanTake				"EventCanTake", "ii"
#define GAMEEVENT_CityEventCanTake			"CityEventCanTake", "iii"
#define GAMEEVENT_EventUnitCreated			"EventUnitCreated", "iii"

// Serialization wrappers
#define MOD_SERIALIZE

#if defined(MOD_SERIALIZE)

//to help debug errors
void CheckSentinel(uint);

#define MOD_SERIALIZE_INIT_READ_NO_SENTINEL(stream) uint uiDllSaveVersion; stream >> uiDllSaveVersion;
#define MOD_SERIALIZE_INIT_READ(stream) uint uiDllSaveVersion, uiSentinel; stream >> uiDllSaveVersion; stream >> uiSentinel; CheckSentinel(uiSentinel);
#define MOD_SERIALIZE_READ(version, stream, member, def) if (uiDllSaveVersion >= version) { stream >> member; } else { member = def; }
#define MOD_SERIALIZE_READ_AUTO(version, stream, member, size, def)   \
	if (uiDllSaveVersion >= version) {                                \
		stream >> member;                                             \
	} else {                                                          \
		for (int iI = 0; iI < size; iI++) { member.setAt(iI, def); }  \
	}
#define MOD_SERIALIZE_READ_ARRAY(version, stream, member, type, size, def)	\
	if (uiDllSaveVersion >= version) {										\
		ArrayWrapper<type> wrapper(size, member); stream >> wrapper;		\
	} else {																\
		for (int iI = 0; iI < size; iI++) { (member)[iI] = def; }			\
	}
#define MOD_SERIALIZE_READ_HASH(version, stream, member, type, size, def)		\
	if (uiDllSaveVersion >= version) {											\
		CvInfosSerializationHelper::ReadHashedDataArray(stream, member, size);	\
	} else {																	\
		for (int iI = 0; iI < size; iI++) { (member)[iI] = def; }				\
	}

#define MOD_SERIALIZE_INIT_WRITE_NO_SENTINEL(stream) uint uiDllSaveVersion = MOD_DLL_VERSION_NUMBER; stream << uiDllSaveVersion;
#define MOD_SERIALIZE_INIT_WRITE(stream) uint uiDllSaveVersion = MOD_DLL_VERSION_NUMBER; stream << uiDllSaveVersion; stream << 0xDEADBEEF;
#define MOD_SERIALIZE_WRITE(stream, member) CvAssert(uiDllSaveVersion == MOD_DLL_VERSION_NUMBER); stream << member
#define MOD_SERIALIZE_WRITE_AUTO(stream, member) CvAssert(uiDllSaveVersion == MOD_DLL_VERSION_NUMBER); stream << member
#define MOD_SERIALIZE_WRITE_ARRAY(stream, member, type, size) CvAssert(uiDllSaveVersion == MOD_DLL_VERSION_NUMBER); stream << ArrayWrapper<type>(size, member)
#define MOD_SERIALIZE_WRITE_CONSTARRAY(stream, member, type, size) CvAssert(uiDllSaveVersion == MOD_DLL_VERSION_NUMBER); stream << ArrayWrapperConst<type>(size, member)
#define MOD_SERIALIZE_WRITE_HASH(stream, member, type, size, obj) CvAssert(uiDllSaveVersion == MOD_DLL_VERSION_NUMBER); CvInfosSerializationHelper::WriteHashedDataArray<obj, type>(stream, member, size)
#else
#define MOD_SERIALIZE_INIT_READ(stream) __noop
#define MOD_SERIALIZE_READ(version, stream, member, def) __noop
#define MOD_SERIALIZE_READ_AUTO(version, stream, member, size, def) __noop
#define MOD_SERIALIZE_READ_ARRAY(version, stream, member, type, size, def) __noop
#define MOD_SERIALIZE_READ_HASH(version, stream, member, type, size, def) __noop
#define MOD_SERIALIZE_INIT_WRITE(stream) __noop
#define MOD_SERIALIZE_WRITE(stream, member) __noop
#define MOD_SERIALIZE_WRITE_AUTO(stream, member) __noop
#define MOD_SERIALIZE_WRITE_ARRAY(stream, member, type, size) __noop
#define MOD_SERIALIZE_WRITE_ARRAYCONST(stream, member, type, size) __noop
#define MOD_SERIALIZE_WRITE_HASH(stream, member, type, size) __noop
#endif


// Custom database table name and columns
#define MOD_DB_TABLE "CustomModOptions"
#define MOD_DB_COL_NAME "Name"
#define MOD_DB_COL_VALUE "Value"
#define MOD_DB_COL_CLASS "Class"
#define MOD_DB_COL_DBUPDATES "DbUpdates"
#define MOD_DB_UPDATES "CustomModDbUpdates"


// CustomMod option wrappers
#define MOD_OPT_DECL(name)  protected: bool m_b##name; public: inline bool is##name() { return m_b##name; }
#define MOD_OPT_CACHE(name) m_b##name = (m_options[string(#name)] == 1);


// Class used to cache the database control settings and provide utility functions
class CustomMods {
public:
	CustomMods();

	int eventHook(const char* szName, const char* p, ...);
	int eventTestAll(const char* szName, const char* p, ...);
	int eventTestAny(const char* szName, const char* p, ...);
	int eventAccumulator(int& iValue, const char* szName, const char* p, ...);

	// CvLuaArgsHandle in the next four methos MUST be passed by reference (&args)
	int eventHook(const char* szName, CvLuaArgsHandle &args);
	int eventTestAll(const char* szName, CvLuaArgsHandle &args);
	int eventTestAny(const char* szName, CvLuaArgsHandle &args);
	int eventAccumulator(int& iValue, const char* szName, CvLuaArgsHandle &args);

	void prefetchCache();
	void preloadCache();
	void reloadCache();
	int getOption(const char* szName, int defValue = 0);
	int getOption(std::string sName, int defValue = 0);
	int getCivOption(const char* szCiv, const char* szName, int defValue = 0);

	MOD_OPT_DECL(GLOBAL_EARLY_COOP_WAR_LOCK);
	MOD_OPT_DECL(CORE_DEBUGGING);
	MOD_OPT_DECL(GLOBAL_STACKING_RULES);
	MOD_OPT_DECL(GLOBAL_BREAK_CIVILIAN_1UPT);
	MOD_OPT_DECL(GLOBAL_BREAK_CIVILIAN_RESTRICTIONS);
	MOD_OPT_DECL(GLOBAL_LOCAL_GENERALS);
	MOD_OPT_DECL(GLOBAL_SEPARATE_GREAT_ADMIRAL);
	MOD_OPT_DECL(GLOBAL_PROMOTION_CLASSES);
	MOD_OPT_DECL(GLOBAL_PASSABLE_FORTS);
	MOD_OPT_DECL(GLOBAL_PASSABLE_FORTS_ANY);
	MOD_OPT_DECL(GLOBAL_ANYTIME_GOODY_GOLD);
	MOD_OPT_DECL(GLOBAL_CITY_FOREST_BONUS);
	MOD_OPT_DECL(GLOBAL_CITY_JUNGLE_BONUS);
	MOD_OPT_DECL(GLOBAL_CITY_WORKING);
	MOD_OPT_DECL(GLOBAL_RELOCATION);
	MOD_OPT_DECL(GLOBAL_ALPINE_PASSES);
	MOD_OPT_DECL(GLOBAL_CS_GIFT_SHIPS);
	MOD_OPT_DECL(GLOBAL_CS_UPGRADES);
	MOD_OPT_DECL(GLOBAL_CS_RAZE_RARELY);
	MOD_OPT_DECL(GLOBAL_CS_LIBERATE_AFTER_BUYOUT);
	MOD_OPT_DECL(GLOBAL_CS_GIFTS);
	MOD_OPT_DECL(GLOBAL_CS_GIFTS_LOCAL_XP);
	MOD_OPT_DECL(GLOBAL_CS_OVERSEAS_TERRITORY);
	MOD_OPT_DECL(GLOBAL_CS_NO_ALLIED_SKIRMISHES);
	MOD_OPT_DECL(GLOBAL_VENICE_KEEPS_RESOURCES);
	MOD_OPT_DECL(GLOBAL_CS_MARRIAGE_KEEPS_RESOURCES);
	MOD_OPT_DECL(GLOBAL_NO_FOLLOWUP_FROM_CITIES);
	MOD_OPT_DECL(GLOBAL_CAPTURE_AFTER_ATTACKING);
	MOD_OPT_DECL(GLOBAL_NO_OCEAN_PLUNDERING);
	MOD_OPT_DECL(GLOBAL_NO_CONQUERED_SPACESHIPS);
	MOD_OPT_DECL(GLOBAL_GRATEFUL_SETTLERS);
	MOD_OPT_DECL(GLOBAL_RELIGIOUS_SETTLERS);
	MOD_OPT_DECL(GLOBAL_QUICK_ROUTES);
	MOD_OPT_DECL(GLOBAL_SUBS_UNDER_ICE_IMMUNITY);
	MOD_OPT_DECL(GLOBAL_PARATROOPS_AA_DAMAGE);
	MOD_OPT_DECL(GLOBAL_NUKES_MELT_ICE); 
	MOD_OPT_DECL(GLOBAL_GREATWORK_YIELDTYPES); 
	MOD_OPT_DECL(GLOBAL_NO_LOST_GREATWORKS); 
	MOD_OPT_DECL(GLOBAL_EXCLUDE_FROM_GIFTS);
	MOD_OPT_DECL(GLOBAL_MOVE_AFTER_UPGRADE);
	MOD_OPT_DECL(GLOBAL_CANNOT_EMBARK);
	MOD_OPT_DECL(GLOBAL_SEPARATE_GP_COUNTERS);
	MOD_OPT_DECL(GLOBAL_TRULY_FREE_GP);
	MOD_OPT_DECL(NO_AUTO_SPAWN_PROPHET);
	MOD_OPT_DECL(ALTERNATE_ASSYRIA_TRAIT);
	MOD_OPT_DECL(NO_REPAIR_FOREIGN_LANDS);
	MOD_OPT_DECL(NO_YIELD_ICE);
	MOD_OPT_DECL(NO_MAJORCIV_GIFTING);
	MOD_OPT_DECL(NO_HEALING_ON_MOUNTAINS);
	MOD_OPT_DECL(PILLAGE_PERMANENT_IMPROVEMENTS);
	MOD_OPT_DECL(DIPLOMACY_TECH_BONUSES);
	MOD_OPT_DECL(DIPLOMACY_AUTO_DENOUNCE);
	MOD_OPT_DECL(DIPLOMACY_NO_LEADERHEADS);
	MOD_OPT_DECL(DIPLOMACY_CITYSTATES); 
	MOD_OPT_DECL(DIPLOMACY_CITYSTATES_QUESTS); 
	MOD_OPT_DECL(DIPLOMACY_CITYSTATES_RESOLUTIONS); 
	MOD_OPT_DECL(DIPLOMACY_CITYSTATES_HURRY);
	MOD_OPT_DECL(SHIPS_FIRE_IN_CITIES_IMPROVEMENTS);
	MOD_OPT_DECL(PSEUDO_NATURAL_WONDER);
	MOD_OPT_DECL(COMMUNITY_PATCH);
	MOD_OPT_DECL(BALANCE_CORE);
	MOD_OPT_DECL(BALANCE_CORE_YIELDS);
	MOD_OPT_DECL(BALANCE_CORE_SPIES);
	MOD_OPT_DECL(BALANCE_CORE_SPIES_ADVANCED);
	MOD_OPT_DECL(BALANCE_CORE_MILITARY);
	MOD_OPT_DECL(BALANCE_CORE_SETTLER);
	MOD_OPT_DECL(BALANCE_CORE_SETTLER_ADVANCED);
	MOD_OPT_DECL(BALANCE_CORE_DEALS);
	MOD_OPT_DECL(BALANCE_CORE_DEALS_ADVANCED);
	MOD_OPT_DECL(BALANCE_CORE_MINORS);
	MOD_OPT_DECL(BALANCE_CORE_DIFFICULTY);
	MOD_OPT_DECL(BALANCE_CORE_HAPPINESS);
	MOD_OPT_DECL(BALANCE_CORE_HAPPINESS_MODIFIERS);
	MOD_OPT_DECL(BALANCE_CORE_HAPPINESS_NATIONAL);
	MOD_OPT_DECL(BALANCE_CORE_HAPPINESS_LUXURY);
	MOD_OPT_DECL(BALANCE_CORE_POP_REQ_BUILDINGS);
	MOD_OPT_DECL(BALANCE_CORE_SETTLER_RESET_FOOD);
	MOD_OPT_DECL(BALANCE_CORE_WONDERS_VARIABLE_REWARD);
	MOD_OPT_DECL(BALANCE_CORE_BELIEFS_RESOURCE);
	MOD_OPT_DECL(BALANCE_CORE_AFRAID_ANNEX);
	MOD_OPT_DECL(BALANCE_CORE_BUILDING_INSTANT_YIELD);
	MOD_OPT_DECL(BALANCE_CORE_GRANDSTRATEGY_AI);
	MOD_OPT_DECL(BALANCE_CORE_BELIEFS);
	MOD_OPT_DECL(BALANCE_CORE_FOLLOWER_POP_WONDER);
	MOD_OPT_DECL(BALANCE_CORE_POLICIES);
	MOD_OPT_DECL(BALANCE_CORE_BARBARIAN_THEFT);
	MOD_OPT_DECL(BALANCE_CORE_SETTLER_MOVE);
	MOD_OPT_DECL(BALANCE_CORE_RESOURCE_FLAVORS);
	MOD_OPT_DECL(BALANCE_CORE_PURCHASE_COST_INCREASE);
	MOD_OPT_DECL(BALANCE_CORE_PUPPET_PURCHASE);
	MOD_OPT_DECL(BALANCE_CORE_EMBARK_CITY_NO_COST);
	MOD_OPT_DECL(BALANCE_CORE_MINOR_PROTECTION);
	MOD_OPT_DECL(BALANCE_CORE_IDEOLOGY_START);
	MOD_OPT_DECL(BALANCE_CORE_WONDER_COST_INCREASE);
	MOD_OPT_DECL(BALANCE_CORE_MINOR_CIV_GIFT);
	MOD_OPT_DECL(BALANCE_CORE_DIPLOMACY);
	MOD_OPT_DECL(BALANCE_CORE_DIPLOMACY_ERA_INFLUENCE);
	MOD_OPT_DECL(BALANCE_CORE_DIPLOMACY_ADVANCED);
	MOD_OPT_DECL(BALANCE_CORE_LUXURIES_TRAIT);
	MOD_OPT_DECL(BALANCE_CORE_MILITARY_PROMOTION_ADVANCED);
	MOD_OPT_DECL(BALANCE_CORE_MILITARY_LOGGING);
	MOD_OPT_DECL(BALANCE_CORE_RESOURCE_MONOPOLIES);
	MOD_OPT_DECL(BALANCE_CORE_RESOURCE_MONOPOLIES_STRATEGIC);
	MOD_OPT_DECL(BALANCE_CORE_BUILDING_INVESTMENTS);
	MOD_OPT_DECL(BALANCE_CORE_UNIT_INVESTMENTS);
	MOD_OPT_DECL(BALANCE_CORE_BUILDING_RESOURCE_MAINTENANCE);
	MOD_OPT_DECL(BALANCE_CORE_ENGINEER_HURRY);
	MOD_OPT_DECL(BALANCE_CORE_MAYA_CHANGE);
	MOD_OPT_DECL(BALANCE_CORE_PORTUGAL_CHANGE);
	MOD_OPT_DECL(BALANCE_CORE_MINOR_VARIABLE_BULLYING);
	MOD_OPT_DECL(BALANCE_CORE_MINOR_PTP_MINIMUM_VALUE);
	MOD_OPT_DECL(BALANCE_YIELD_SCALE_ERA);
	MOD_OPT_DECL(BALANCE_CORE_NEW_GP_ATTRIBUTES);
	MOD_OPT_DECL(BALANCE_CORE_JFD);
	MOD_OPT_DECL(BALANCE_CORE_MILITARY_RESISTANCE);
	MOD_OPT_DECL(BALANCE_CORE_PANTHEON_RESET_FOUND);
	MOD_OPT_DECL(BALANCE_CORE_VICTORY_GAME_CHANGES);
	MOD_OPT_DECL(BALANCE_CORE_EVENTS);
	MOD_OPT_DECL(NO_RANDOM_TEXT_CIVS);
	MOD_OPT_DECL(BALANCE_RETROACTIVE_PROMOS);
	MOD_OPT_DECL(BALANCE_NO_GAP_DURING_GA);
	MOD_OPT_DECL(BALANCE_DYNAMIC_UNIT_SUPPLY);
	MOD_OPT_DECL(BALANCE_CORE_UNIQUE_BELIEFS_ONLY_FOR_CIV);
	MOD_OPT_DECL(BALANCE_DEFENSIVE_PACTS_AGGRESSION_ONLY);
	MOD_OPT_DECL(BALANCE_CORE_SCALING_TRADE_DISTANCE);
	MOD_OPT_DECL(BALANCE_CORE_SCALING_XP);
	MOD_OPT_DECL(BALANCE_CORE_HALF_XP_PURCHASE);

	MOD_OPT_DECL(CIV6_WORKER);
	MOD_OPT_DECL(CIV6_ROADS);
	MOD_OPT_DECL(CIV6_EUREKAS);
	MOD_OPT_DECL(CITY_STATE_SCALE);
	MOD_OPT_DECL(ANY_PANTHEON);

	MOD_OPT_DECL(DIPLOMACY_CIV4_FEATURES);
	MOD_OPT_DECL(CARGO_SHIPS);
	MOD_OPT_DECL(BARBARIAN_GG_GA_POINTS);
	MOD_OPT_DECL(ALTERNATE_CELTS);
	MOD_OPT_DECL(TRAITS_GG_FROM_BARBARIANS);
	MOD_OPT_DECL(TRAITS_CROSSES_ICE);
	MOD_OPT_DECL(TRAITS_CITY_WORKING);
	MOD_OPT_DECL(TRAITS_OTHER_PREREQS);
	MOD_OPT_DECL(TRAITS_ANY_BELIEF);
	MOD_OPT_DECL(TRAITS_TRADE_ROUTE_BONUSES);
	MOD_OPT_DECL(TRAITS_EXTRA_SUPPLY);

	MOD_OPT_DECL(POLICIES_CITY_WORKING);
	MOD_OPT_DECL(ERA_RESTRICTION);
	MOD_OPT_DECL(USE_TRADE_FEATURES);
	MOD_OPT_DECL(TECHS_CITY_WORKING);

	MOD_OPT_DECL(PROMOTIONS_AURA_CHANGE);
	MOD_OPT_DECL(PROMOTIONS_GG_FROM_BARBARIANS);
	MOD_OPT_DECL(PROMOTIONS_VARIABLE_RECON);
	MOD_OPT_DECL(PROMOTIONS_CROSS_MOUNTAINS);
	MOD_OPT_DECL(PROMOTIONS_CROSS_OCEANS);
	MOD_OPT_DECL(PROMOTIONS_CROSS_ICE);
	MOD_OPT_DECL(PROMOTIONS_HALF_MOVE);
	MOD_OPT_DECL(PROMOTIONS_DEEP_WATER_EMBARKATION);
	MOD_OPT_DECL(PROMOTIONS_FLAGSHIP);
	MOD_OPT_DECL(PROMOTIONS_UNIT_NAMING);
	MOD_OPT_DECL(PROMOTIONS_IMPROVEMENT_BONUS);

	MOD_OPT_DECL(UI_CITY_PRODUCTION);
	MOD_OPT_DECL(UI_CITY_EXPANSION);

	MOD_OPT_DECL(BUILDINGS_NW_EXCLUDE_RAZING);
	MOD_OPT_DECL(BUILDINGS_PRO_RATA_PURCHASE);
	MOD_OPT_DECL(BUILDINGS_CITY_WORKING);

	MOD_OPT_DECL(TRADE_ROUTE_SCALING);
	MOD_OPT_DECL(TRADE_WONDER_RESOURCE_ROUTES);

	MOD_OPT_DECL(UNITS_NO_SUPPLY);
	MOD_OPT_DECL(UNITS_MAX_HP);
	MOD_OPT_DECL(UNITS_XP_TIMES_100);
	MOD_OPT_DECL(UNITS_LOCAL_WORKERS);
	MOD_OPT_DECL(UNITS_HOVERING_LAND_ONLY_HEAL);
	MOD_OPT_DECL(UNITS_HOVERING_COASTAL_ATTACKS);

	MOD_OPT_DECL(RELIGION_NO_PREFERRENCES);
	MOD_OPT_DECL(RELIGION_RANDOMISE);
	MOD_OPT_DECL(RELIGION_CONVERSION_MODIFIERS);
	MOD_OPT_DECL(RELIGION_KEEP_PROPHET_OVERFLOW);
	MOD_OPT_DECL(RELIGION_ALLIED_INQUISITORS);
	MOD_OPT_DECL(RELIGION_RECURRING_PURCHASE_NOTIFIY);
	MOD_OPT_DECL(RELIGION_PLOT_YIELDS);
	MOD_OPT_DECL(RELIGION_POLICY_BRANCH_FAITH_GP);
	MOD_OPT_DECL(RELIGION_LOCAL_RELIGIONS);

	MOD_OPT_DECL(PROCESS_STOCKPILE);

	MOD_OPT_DECL(AI_SECONDARY_WORKERS);
	MOD_OPT_DECL(AI_SECONDARY_SETTLERS);
	MOD_OPT_DECL(AI_SMART_V3);
	MOD_OPT_DECL(AI_SMART);
	MOD_OPT_DECL(AI_SMART_DEALS);
	MOD_OPT_DECL(AI_SMART_GREAT_PEOPLE);
	MOD_OPT_DECL(AI_SMART_GRAND_STRATEGY);
	MOD_OPT_DECL(AI_SMART_ARCHAEOLOGISTS);
	MOD_OPT_DECL(AI_SMART_DISBAND);
	MOD_OPT_DECL(AI_SMART_UPGRADES);
	MOD_OPT_DECL(AI_SMART_HEALING);
	MOD_OPT_DECL(AI_SMART_FLEE_FROM_DANGER);
	MOD_OPT_DECL(AI_SMART_RANGED_UNITS);
	MOD_OPT_DECL(AI_SMART_AIR_TACTICS);
	MOD_OPT_DECL(AI_SMART_MELEE_TACTICS);

	MOD_OPT_DECL(EVENTS_TERRAFORMING);
	MOD_OPT_DECL(EVENTS_TILE_IMPROVEMENTS);
	MOD_OPT_DECL(EVENTS_TILE_REVEALED);
	MOD_OPT_DECL(EVENTS_CIRCUMNAVIGATION);
	MOD_OPT_DECL(EVENTS_NEW_ERA);
	MOD_OPT_DECL(EVENTS_NW_DISCOVERY);
	MOD_OPT_DECL(EVENTS_DIPLO_EVENTS);
	MOD_OPT_DECL(EVENTS_DIPLO_MODIFIERS);
	MOD_OPT_DECL(EVENTS_MINORS);
	MOD_OPT_DECL(EVENTS_MINORS_GIFTS);
	MOD_OPT_DECL(EVENTS_MINORS_INTERACTION);
	MOD_OPT_DECL(EVENTS_QUESTS);
	MOD_OPT_DECL(EVENTS_BARBARIANS);
	MOD_OPT_DECL(EVENTS_GOODY_CHOICE);
	MOD_OPT_DECL(EVENTS_GOODY_TECH);
	MOD_OPT_DECL(EVENTS_AI_OVERRIDE_TECH);
	MOD_OPT_DECL(EVENTS_GREAT_PEOPLE);
	MOD_OPT_DECL(EVENTS_FOUND_RELIGION);
	MOD_OPT_DECL(EVENTS_ACQUIRE_BELIEFS);
	MOD_OPT_DECL(EVENTS_RELIGION);
	MOD_OPT_DECL(EVENTS_ESPIONAGE);
	MOD_OPT_DECL(EVENTS_PLOT);
	MOD_OPT_DECL(EVENTS_GOLDEN_AGE);
	MOD_OPT_DECL(EVENTS_CITY);
	MOD_OPT_DECL(EVENTS_CITY_CAPITAL);
	MOD_OPT_DECL(EVENTS_CITY_BORDERS);
	MOD_OPT_DECL(EVENTS_LIBERATION);
	MOD_OPT_DECL(EVENTS_CITY_FOUNDING);
	MOD_OPT_DECL(EVENTS_CITY_RAZING);
	MOD_OPT_DECL(EVENTS_CITY_AIRLIFT);
	MOD_OPT_DECL(EVENTS_CITY_BOMBARD);
	MOD_OPT_DECL(EVENTS_CITY_CONNECTIONS);
	MOD_OPT_DECL(EVENTS_AREA_RESOURCES);
	MOD_OPT_DECL(EVENTS_PARADROPS);
	MOD_OPT_DECL(EVENTS_UNIT_CREATED);
	MOD_OPT_DECL(EVENTS_UNIT_FOUNDED);
	MOD_OPT_DECL(EVENTS_UNIT_PREKILL);
	MOD_OPT_DECL(EVENTS_UNIT_CAPTURE);
	MOD_OPT_DECL(EVENTS_CAN_MOVE_INTO);
	MOD_OPT_DECL(EVENTS_UNIT_ACTIONS);
	MOD_OPT_DECL(EVENTS_UNIT_UPGRADES);
	MOD_OPT_DECL(EVENTS_UNIT_DATA);
	MOD_OPT_DECL(EVENTS_TRADE_ROUTES);
	MOD_OPT_DECL(EVENTS_TRADE_ROUTE_PLUNDERED);
	MOD_OPT_DECL(EVENTS_WAR_AND_PEACE);
	MOD_OPT_DECL(EVENTS_RESOLUTIONS);
	MOD_OPT_DECL(EVENTS_IDEOLOGIES);
	MOD_OPT_DECL(EVENTS_NUCLEAR_DETONATION);
	MOD_OPT_DECL(EVENTS_AIRLIFT);
	MOD_OPT_DECL(EVENTS_REBASE);
	MOD_OPT_DECL(EVENTS_COMMAND);
	MOD_OPT_DECL(EVENTS_CUSTOM_MISSIONS);
	MOD_OPT_DECL(EVENTS_BATTLES);
	MOD_OPT_DECL(EVENTS_BATTLES_DAMAGE);

	MOD_OPT_DECL(EVENTS_RED_TURN);
	MOD_OPT_DECL(EVENTS_RED_COMBAT);
	MOD_OPT_DECL(EVENTS_RED_COMBAT_MISSION);
	MOD_OPT_DECL(EVENTS_RED_COMBAT_ABORT);
	MOD_OPT_DECL(EVENTS_RED_COMBAT_RESULT);
	MOD_OPT_DECL(EVENTS_RED_COMBAT_ENDED);
	MOD_OPT_DECL(ACTIVE_DIPLOMACY);
	MOD_OPT_DECL(API_ESPIONAGE);
	MOD_OPT_DECL(API_TRADEROUTES);
	MOD_OPT_DECL(API_RELIGION);
	MOD_OPT_DECL(API_PLOT_BASED_DAMAGE);
	MOD_OPT_DECL(API_PLOT_YIELDS);
	MOD_OPT_DECL(API_ACHIEVEMENTS);
	MOD_OPT_DECL(API_EXTENSIONS);
	MOD_OPT_DECL(API_LUA_EXTENSIONS);

	MOD_OPT_DECL(CONFIG_GAME_IN_XML);
	MOD_OPT_DECL(CONFIG_AI_IN_XML);

	MOD_OPT_DECL(BUGFIX_RESEARCH_OVERFLOW);
	MOD_OPT_DECL(BUGFIX_LUA_CHANGE_VISIBILITY_COUNT);
	MOD_OPT_DECL(BUGFIX_RELIGIOUS_SPY_PRESSURE);
	MOD_OPT_DECL(BUGFIX_MOVE_AFTER_PURCHASE);
	MOD_OPT_DECL(BUGFIX_UNITCLASS_NOT_UNIT);
	MOD_OPT_DECL(BUGFIX_BUILDINGCLASS_NOT_BUILDING);
	MOD_OPT_DECL(BUGFIX_FREE_FOOD_BUILDING);
	MOD_OPT_DECL(BUGFIX_NAVAL_FREE_UNITS);
	MOD_OPT_DECL(BUGFIX_NAVAL_NEAREST_WATER);
	MOD_OPT_DECL(BUGFIX_CITY_STACKING);
	MOD_OPT_DECL(BUGFIX_BARB_CAMP_TERRAINS);
	MOD_OPT_DECL(BUGFIX_BARB_CAMP_SPAWNING);
	MOD_OPT_DECL(BUGFIX_BARB_GP_XP);
	MOD_OPT_DECL(BUGFIX_REMOVE_GHOST_ROUTES);
	MOD_OPT_DECL(BUGFIX_UNITS_AWAKE_IN_DANGER);
	MOD_OPT_DECL(BUGFIX_WORKERS_VISIBLE_DANGER);
	MOD_OPT_DECL(BUGFIX_FEATURE_REMOVAL);
	MOD_OPT_DECL(BUGFIX_INTERCEPTOR_STRENGTH);
	MOD_OPT_DECL(BUGFIX_UNIT_POWER_CALC);
	MOD_OPT_DECL(BUGFIX_UNIT_POWER_BONUS_VS_DOMAIN_ONLY);
	MOD_OPT_DECL(BUGFIX_UNIT_POWER_NAVAL_CONSISTENCY);
	MOD_OPT_DECL(BUGFIX_UNIT_PREREQ_PROJECT);
	MOD_OPT_DECL(BUGFIX_NO_HOVERING_REBELS);
	MOD_OPT_DECL(BUGFIX_HOVERING_PATHFINDER);
	MOD_OPT_DECL(BUGFIX_EMBARKING_PATHFINDER);
	MOD_OPT_DECL(BUGFIX_MINOR_CIV_STRATEGIES);

protected:
	bool m_bInit;
	std::map<std::string, int> m_options;
};

extern CustomMods gCustomMods;
#endif