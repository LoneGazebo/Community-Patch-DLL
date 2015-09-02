﻿// CustomMods.h
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
#define MOD_DLL_NAME "Community Patch v66 (PNM v51+)"
#define MOD_DLL_VERSION_NUMBER ((uint) 66)
#define MOD_DLL_VERSION_STATUS ""			// a (alpha), b (beta) or blank (released)
#define MOD_DLL_CUSTOM_BUILD_NAME ""


// Comment out this line to include all the achievements code (which don't work in modded games, so we don't need the code!)
#define NO_ACHIEVEMENTS

/// try to unify mountains and impassable terrain - only halfway implemented 
//#define MOD_BALANCE_CORE_SANE_IMPASSABILITY
/// simpler algorithm for scoring exploration plots
#define MOD_CORE_ALTERNATIVE_EXPLORE_SCORE
/// use globally unique ids for cities, units etc
#define MOD_BALANCE_CORE_GLOBAL_IDS
/// clean up defines for pathfinder
#define MOD_BALANCE_CORE_PATHFINDER_FLAGS

/// call stack tracing etc
#define MOD_BALANCE_CORE_DEBUGGING

#define AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
#define AUI_UNIT_FIX_BAD_BONUS_STACKS

//#define MOD_GLOBAL_MAX_MAJOR_CIVS 43
//disable this as it would make caching the danger calculation result much more complex - also not well tested, unclear benefit
//#define AUI_UNIT_GET_NTH_BEST_INTERCEPTOR

#define AUI_UNIT_EXTRA_ATTACKS_GETTER
#define AUI_TACTICAL_FIX_SCORE_GREAT_GENERAL_PLOT_NO_OVERLAP
#define AUI_UNIT_DO_AITYPE_FLIP
#define AUI_DANGER_PLOTS_REMADE
#define AUI_DANGER_PLOTS_ADD_DANGER_CONSIDER_TERRAIN_STRENGTH_MODIFICATION
#define AUI_DANGER_PLOTS_TWEAKED_RANGED
#define AUI_DANGER_PLOTS_FIX_SHOULD_IGNORE_UNIT_IGNORE_VISIBILITY_PLOT 
#define AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MINORS_SEE_MAJORS (5)
#define AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MAJORS_SEE_BARBARIANS_IN_FOG
#define AUI_DANGER_PLOTS_FIX_IS_DANGER_BY_RELATIONSHIP_ZERO_MINORS_IGNORE_ALL_NONWARRED

#define MOD_BALANCE_CORE_OBSERVER_CHANGES
	#define AUI_PLOT_OBSERVER_SEE_ALL_PLOTS
	#define AUI_GAME_OBSERVER_CAN_OPEN_CITIES
	#define AUI_PLOT_OBSERVER_NO_NW_POPUPS

#define AUI_ASTAR_FIX_NO_DUPLICATE_CALLS
#define AUI_WORKER_EVALUATE_WORKER_RETREAT_AND_BUILD 

#ifdef AUI_DANGER_PLOTS_REMADE
	#define AIR_ACTION_ATTACK		0
	#define AIR_ACTION_SWEEP		1
	#define AIR_ACTION_INTERCEPT	2
#endif

/// Performance: Don't generate long paths when we know we don't want them
#define AUI_ASTAR_TURN_LIMITER
/// Enables the Binomial Random Number Generator
#define AUI_BINOM_RNG
/// Reorders some checks to make sure ones that process faster get executed first (if they fail, then the function skips checking the slower ones)
#define AUI_ASTAR_FIX_FASTER_CHECKS
/// Gets the last node before the parent (used for planning melee attacks to know where they'd attack from)
#define AUI_ASTAR_GET_PENULTIMATE_NODE
/// The danger of a tile will only be considered when checking path nodes, not when checking the destination (stops units from freezing in panic)
#define AUI_ASTAR_FIX_CONSIDER_DANGER_ONLY_PATH
/// Flavors that weren't previously fetched but were still (attempted to be) used in processing later are now fetched
#define AUI_HOMELAND_FIX_ESTABLISH_HOMELAND_PRIORITIES_MISSING_FLAVORS
/// Disables the code that would start fortifying scouts if recon state was set as "enough"
#define AUI_HOMELAND_ALWAYS_MOVE_SCOUTS
/// Fixes the code that checks for cramped status (it always triggered originally, now it only triggers if we really are cramped)
#define AUI_GS_CONQUEST_FIX_CRAMPED
/// Removes the check for a cultural grand strategy that's a holdover from pre-BNW when cultural victories were won through policies, not tourism
#define AUI_ECONOMIC_FIX_EXPAND_LIKE_CRAZY_REMOVE_HOLDOVER_CULTURE_CHECK
/// Priorities for sneak attack military units are no longer artificially inflated at the highest difficulty levels
#define AUI_CITYSTRATEGY_CHOOSE_PRODUCTION_NO_HIGH_DIFFICULTY_SKEW
/// The AI wants an expensive tech if it's selecting a free tech
#define AUI_TECHAI_CHOOSE_NEXT_TECH_FREE_TECH_WANTS_EXPENSIVE
/// Buildings that contribute towards getting an ideology act as a unique building for the purposes of tech scoring
#define AUI_PLAYERTECHS_RESET_IDEOLOGY_UNLOCKERS_COUNT_AS_UNIQUE

/// The filter to filter out scouting units only applies to units whose default AI is scouting
#define AUI_OPERATION_FIX_FIND_BEST_FIT_RESERVE_CONSIDER_SCOUTING_NONSCOUTS
/// If a civilian retargets and an escort cannot get to the new target (ignoring units), then the operation is aborted
#define AUI_OPERATION_FIX_RETARGET_CIVILIAN_ABORT_IF_UNREACHABLE_ESCORT
/// GetClosestUnit() will now consider whether a unit can paradrop to the target location
#define AUI_OPERATION_GET_CLOSEST_UNIT_PARADROP
/// GetClosestUnit() will no longer terminate early after finding a single unit with "good enough" range (very important once roads start being used)
#define AUI_OPERATION_GET_CLOSEST_UNIT_NO_EARLY_BREAK
/// If two units have the same path distance in GetClosestUnit(), the one with the higher current strength wins (most effective when paired up with no early break)
#define AUI_OPERATION_GET_CLOSEST_UNIT_GET_STRONGEST

/// When calculating the expected damage on a target from a melee unit, the AI will now use pTargetPlot and pDefender parameters when appropriate (instead of having them as NULL)
#define AUI_TACTICAL_FIX_FIND_UNITS_WITHIN_STRIKING_DISTANCE_MELEE_STRENGTH
/// When calculating the expected damage on a target from a melee unit, the AI will now use pDefender and pCity parameters when appropriate (instead of having them as NULL)
#define AUI_TACTICAL_FIX_FIND_UNITS_WITHIN_STRIKING_DISTANCE_RANGED_STRENGTH
/// When calculating the expected damage on a target from a melee unit, the AI will now use pTargetPlot and pDefender parameters when appropriate (instead of having them as NULL)
#define AUI_TACTICAL_FIX_FIND_PARATROOPER_WITHIN_STRIKING_DISTANCE_MELEE_STRENGTH
/// When calculating the expected damage on a target from a melee unit, the AI will now use pFromPlot and pDefender parameters when appropriate (instead of having them as NULL)
#define AUI_TACTICAL_FIX_COMPUTE_EXPECTED_DAMAGE_MELEE
/// Air units will now properly factor in possible interceptions made at the tile
#define AUI_TACTICAL_FIX_COMPUTE_EXPECTED_DAMAGE_AIR_UNITS
/// Units that cannot caputre cities will not attempt to do so
#define AUI_TACTICAL_FIX_NO_CAPTURE
/// Fixes poor placement of ranged units with a range of 1 (eg. machine guns)
#define AUI_TACTICAL_FIX_CLOSE_ON_TARGET_MELEE_RANGE
/// When checking the embark safety of a plot, use the plot the unit will be moving to instead of the target plot
#define AUI_TACTICAL_FIX_MOVE_TO_USING_SAFE_EMBARK_CORRECT_PLOT
/// When choosing a plot to move to, the plot's score no longer needs to be positive, it just needs to be more than the great general's current plot score
#define AUI_TACTICAL_MOVE_GREAT_GENERAL_ONLY_REQUIRE_POSITIVE_DELTA
/// Fixes the incorrect logic behind determining whether a unit could make it to an operation's next muster point in time
#define AUI_TACTICAL_FIX_ALL_OPERATION_MOVES_CATCH_UP_TURNS
/// When finding naval units to move to a target, the AI no longer ignores units for its pathfinder portion (so it now essentially works the same way as the land-based one)
#define AUI_TACTICAL_FIX_FIND_CLOSEST_NAVAL_OPERATION_UNIT_DOESNT_IGNORE_UNITS
/// Distance dropoff only starts taking place at 4 tile range instead of immediately
#define AUI_TACTICAL_ANALYSIS_MAP_CALCULATE_MILITARY_STRENGTHS_LIMITED_DISTANCE_DROPOFF
/// Uses pathfinding turns instead of raw distance for strength multipliers - extremely slow!
//#define AUI_TACTICAL_ANALYSIS_MAP_CALCULATE_MILITARY_STRENGTHS_USE_PATHFINDER
/// If a barbarian is already at the targetted plot, patrol around the target instead
#define AUI_TACTICAL_EXECUTE_BARBARIAN_MOVES_PATROL_IF_ON_TARGET
/// Civilians that are not moved with BarbarianCivilianEscortMove now move using passive barbarian move
#define AUI_TACTICAL_EXECUTE_BARBARIAN_MOVES_CIVILIANS_MOVE_PASSIVELY
/// Some robustness fixes
#define AUI_TACTICAL_FIX_PLOT_NAVAL_ESCORT_OPERATION_MOVES_POSSIBLE_NULL_POINTER

/// Adds a minimum danger amount for each plot, to discourage long routes
#define AUI_TRADE_SCORE_TRADE_ROUTE_BASE_DANGER (1)
/// When scoring trade routes, divides by the base-(value) log of the total danger instead of by the actual total danger
#define AUI_TRADE_SCORE_TRADE_ROUTE_DIVIDE_BY_LOG_TOTAL_DANGER (2.0)
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
/// If the first adjusted value is out of bounds, keep rerolling with the amount with which it is out of bounds until we remain in bounds
#define AUI_DIPLOMACY_GET_RANDOM_PERSONALITY_WEIGHT_USE_REROLLS
#if defined(AUI_BINOM_RNG)
	/// When modifying a personality value (eg. Boldness, Wonder Competitiveness), the AI will use the binomial RNG for a normal distribution instead of a flat one
	#define AUI_DIPLOMACY_GET_RANDOM_PERSONALITY_WEIGHT_USES_BINOM_RNG
	/// When rolling about whether to contact a player with a statement or not, the AI will use the binomial RNG for a normal distribution instead of a flat one
	#define AUI_DIPLOMACY_DO_STATEMENT_USES_BINOM_RNG (7)
	/// When adding an extra, random value to the score of a possible cooperative war, the maximum value added will be the AI's boldness instead of a set number
	#define AUI_DIPLOMACY_GET_COOP_WAR_SCORE_MAX_RANDOM_VALUE_IS_BOLDNESS
	/// When adding an extra, random value to the turns the AI will wait before submitting to another demand, the AI will use the binomial RNG for a normal distribution instead of a flat one
	#define AUI_DIPLOMACY_DO_DEMAND_MADE_USES_BINOM_RNG
	/// When adding an extra, random value to the score of whether a request not to settle near lands is acceptable, the AI will use the binomial RNG for a normal distribution instead of a flat one
	#define AUI_DIPLOMACY_IS_DONT_SETTLE_ACCEPTABLE_USES_BINOM_RNG
	/// When adding an extra, random value to the score of whether a DoF is acceptable, the AI will use the binomial RNG for a normal distribution instead of a flat one
	#define AUI_DIPLOMACY_IS_DOF_ACCEPTABLE_USES_BINOM_RNG (5)
	/// When adding an extra, random value to the score of whether to denounce a player, the AI will use the binomial RNG for a normal distribution instead of a flat one
	#define AUI_DIPLOMACY_GET_DENOUNCE_WEIGHT_USES_BINOM_RNG (5)
#endif

// unhappiness from public opinion scales linearly with dissatisfaction
#define MOD_CORE_SMOOTH_PUBLIC_OPINION

// Uncomment this line to include the achievements hack code
// #define ACHIEVEMENT_HACKS
#if defined(ACHIEVEMENT_HACKS)
#undef NO_ACHIEVEMENTS
#else

// Comment out this line to include all the tutorials code
#define NO_TUTORIALS

// Comment out this line to switch off all custom mod logging
#define CUSTOMLOGDEBUG "CustomMods.log"
// true/false to include/exclude file name and line number in the log
#define CUSTOMLOGFILEINFO true

// Comment out this line to switch off all unified yield logging
#define UNIFIEDLOGDEBUG "UnifiedYields.log"

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
#define MOD_API_UNIFIED_YIELDS_CONSOLIDATION        (true)
// Enables the Unified Yields (YIELD_TOURISM) extensions (v56)
#define MOD_API_UNIFIED_YIELDS_TOURISM              (true)
// Enables the Unified Yields (YIELD_GOLDEN_AGE_POINTS) extensions (v57)
#define MOD_API_UNIFIED_YIELDS_GOLDEN_AGE           (true)

// Push various hard-coded values controlling the game out into XML - see DB/CONFIG/GameInXml.sql for specific values -->
#define MOD_CONFIG_GAME_IN_XML                      gCustomMods.isCONFIG_GAME_IN_XML()
// Push various hard-coded values controlling the AI out into XML - see DB/CONFIG/AiInXml.sql for specific values
#define MOD_CONFIG_AI_IN_XML                        gCustomMods.isCONFIG_AI_IN_XML()

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
// Permit cities to work tiles up to MAXIMUM_ACQUIRE_PLOT_DISTANCE - WARNING! Cities take 2.5 times as much memory/file space
#define MOD_GLOBAL_CITY_WORKING                     gCustomMods.isGLOBAL_CITY_WORKING()
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
// Modified blockade logic (v52)
// The standard logic says "if there is an enemy ship within two hexes of a port, it is blockaded"
// HOWEVER - this includes the situations where
//   a) an enemy ship on the other side of a two-wide strip of land can blockade a port (ie enemy-land-port-water blockades the port)
//   b) a port with two non-adjacent exits (ie on a one-wide strip of land) can be blockaded by a non-adjacent enemy (ie enemy-water-port-water blockades the port)
// What is needed is a check for every adjacent water plot to the port being blockaded, not a simple check of the port itself
#define MOD_GLOBAL_ADJACENT_BLOCKADES               gCustomMods.isGLOBAL_ADJACENT_BLOCKADES()
// Adjacent allied ships block blockades by enemy ships 2 or more tiles away
#define MOD_GLOBAL_ALLIES_BLOCK_BLOCKADES           gCustomMods.isGLOBAL_ALLIES_BLOCK_BLOCKADES()
// Embarked combat units only blockade adjacent tiles
#define MOD_GLOBAL_SHORT_EMBARKED_BLOCKADES         gCustomMods.isGLOBAL_SHORT_EMBARKED_BLOCKADES()
// Other player's settlers captured from Barbarians will sometimes remain as settlers
#define MOD_GLOBAL_GRATEFUL_SETTLERS                gCustomMods.isGLOBAL_GRATEFUL_SETTLERS()
// Units that can found a city take their religion with them (v34)
#define MOD_GLOBAL_RELIGIOUS_SETTLERS               gCustomMods.isGLOBAL_RELIGIOUS_SETTLERS()
// Route To will only build roads, or upgrade road to rail, for human players (v44)
#define MOD_GLOBAL_QUICK_ROUTES                     gCustomMods.isGLOBAL_QUICK_ROUTES()
// Subs under ice are immune to all attacks except from other subs
#define MOD_GLOBAL_SUBS_UNDER_ICE_IMMUNITY          gCustomMods.isGLOBAL_SUBS_UNDER_ICE_IMMUNITY()
// Paratroops can move/attack after the drop, if the drop is not at max range
#define MOD_GLOBAL_PARATROOPS_MOVEMENT              gCustomMods.isGLOBAL_PARATROOPS_MOVEMENT()
// Paratroops take AA damage from hostile units
#define MOD_GLOBAL_PARATROOPS_AA_DAMAGE             gCustomMods.isGLOBAL_PARATROOPS_AA_DAMAGE()
// Nukes will melt ice
#define MOD_GLOBAL_NUKES_MELT_ICE                   gCustomMods.isGLOBAL_NUKES_MELT_ICE() 
// Great Works can generate different yields than just culture (v25)
#define MOD_GLOBAL_GREATWORK_YIELDTYPES             gCustomMods.isGLOBAL_GREATWORK_YIELDTYPES() 
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

//Flips open borders to apply in opposite ways- you have to give open borders to gain the tourism bonus
#define MOD_BALANCE_FLIPPED_TOURISM_MODIFIER_OPEN_BORDERS

//Community Patch Info
#define MOD_COMMUNITY_PATCH							gCustomMods.isCOMMUNITY_PATCH()
#if defined(MOD_COMMUNITY_PATCH)
#define MOD_BALANCE_CORE							(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE())
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
#define MOD_BALANCE_CORE_DIPLOMACY_ADVANCED			(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_DIPLOMACY_ADVANCED())
#define MOD_BALANCE_CORE_LUXURIES_TRAIT				(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_LUXURIES_TRAIT())
#define MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED (MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_MILITARY_PROMOTION_ADVANCED())
#define MOD_BALANCE_CORE_MILITARY_LOGGING			(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_MILITARY_LOGGING())
#define MOD_BALANCE_CORE_RESOURCE_MONOPOLIES		(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_RESOURCE_MONOPOLIES())
#define MOD_BALANCE_CORE_BUILDING_INVESTMENTS		(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_BUILDING_INVESTMENTS())
#define MOD_BALANCE_CORE_ENGINEER_HURRY				(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_ENGINEER_HURRY())
#define MOD_BALANCE_CORE_MAYA_CHANGE				(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_MAYA_CHANGE())
#define MOD_BALANCE_CORE_PORTUGAL_CHANGE			(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_PORTUGAL_CHANGE())
#define MOD_BALANCE_CORE_MINOR_VARIABLE_BULLYING	(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_MINOR_VARIABLE_BULLYING())
#define MOD_BALANCE_YIELD_SCALE_ERA		(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_SPECIALIST_FOOD_SCALE_ERA())
#define MOD_BALANCE_CORE_NEW_GP_ATTRIBUTES				(MOD_COMMUNITY_PATCH && gCustomMods.isBALANCE_CORE_NEW_GP_ATTRIBUTES())
#endif

// Changes for the CivIV Diplomacy Features mod by Putmalk - AFFECTS SAVE GAME DATA FORMAT (v36)
#define MOD_DIPLOMACY_CIV4_FEATURES                 gCustomMods.isDIPLOMACY_CIV4_FEATURES()

// Permits land units to cross ice - AFFECTS SAVE GAME DATA FORMAT
#define MOD_TRAITS_CROSSES_ICE                      gCustomMods.isTRAITS_CROSSES_ICE()
// Permits cities to work more rings - AFFECTS SAVE GAME DATA FORMAT
#define MOD_TRAITS_CITY_WORKING                     gCustomMods.isTRAITS_CITY_WORKING()
// Enables any belief to be selected, even if already taken (v46)
#define MOD_TRAITS_ANY_BELIEF                       gCustomMods.isTRAITS_ANY_BELIEF()
// Enables additional trade route related traits (v52)
#define MOD_TRAITS_TRADE_ROUTE_BONUSES              gCustomMods.isTRAITS_TRADE_ROUTE_BONUSES()

// Permits cities to work more rings - AFFECTS SAVE GAME DATA FORMAT
#define MOD_POLICIES_CITY_WORKING                   gCustomMods.isPOLICIES_CITY_WORKING()

// Permits cities to work more rings - AFFECTS SAVE GAME DATA FORMAT
#define MOD_TECHS_CITY_WORKING                      gCustomMods.isTECHS_CITY_WORKING()

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

// Restricts worker suggestions to local tiles
#define MOD_UNITS_LOCAL_WORKERS                     gCustomMods.isUNITS_LOCAL_WORKERS()
// Hovering unit can only heal over land
#define MOD_UNITS_HOVERING_LAND_ONLY_HEAL           gCustomMods.isUNITS_HOVERING_LAND_ONLY_HEAL()
// Permits hovering units to attack coastal shipping
#define MOD_UNITS_HOVERING_COASTAL_ATTACKS          gCustomMods.isUNITS_HOVERING_COASTAL_ATTACKS()

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
// Omit obsolete/no value items as part of a deal if asked to balance things out (v50)
#define MOD_AI_SMART_DEALS                          (MOD_AI_SMART && gCustomMods.isAI_SMART_DEALS())
// Use Great people more effectively, plant some improvements early, and later use GP powers (v50)
#define MOD_AI_SMART_GREAT_PEOPLE                   (MOD_AI_SMART && gCustomMods.isAI_SMART_GREAT_PEOPLE())
// Delay grand strategy bias until the Renaissance (v50)
#define MOD_AI_SMART_GRAND_STRATEGY                 (MOD_AI_SMART && gCustomMods.isAI_SMART_GRAND_STRATEGY())
// Make better policy choices ignoring grand strategy until medieval and giving less importance to opening branches vs unlocked branches (v50)
#define MOD_AI_SMART_POLICY_CHOICE                  (MOD_AI_SMART && gCustomMods.isAI_SMART_POLICY_CHOICE())
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
// Improves the AI's melee tactics (v51)
#define MOD_AI_SMART_MELEE_TACTICS                  (MOD_AI_SMART && gCustomMods.isAI_SMART_MELEE_TACTICS())
#endif

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

// Event sent when a Goody Hut is entered (v33)
//   GameEvents.GoodyHutCanNotReceive.Add(function(iPlayer, iUnit, eGoody, bPick) return false end)
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
//   GameEvents.UnitCaptureType.Add(function(iPlayer, iUnit, iUnitType, iByCiv) return iCaptureUnitType; end)
#define MOD_EVENTS_UNIT_CAPTURE                     gCustomMods.isEVENTS_UNIT_CAPTURE()

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
//   GameEvents.ResolutionResult.Add(function(iResolution, iChoice, bEnact, bPassed) end)
#define MOD_EVENTS_RESOLUTIONS                      gCustomMods.isEVENTS_RESOLUTIONS()

// Events sent about ideologies and tenets (v51)
//   GameEvents.PlayerCanAdopyIdeology.Add(function(iPlayer, iIdeology) return true end)
//   GameEvents.PlayerCanAdopyTenet.Add(function(iPlayer, iTenet) return true end)
#define MOD_EVENTS_IDEOLOGIES                       gCustomMods.isEVENTS_IDEOLOGIES()

// Events sent by plots (v30)
//   GameEvents.PlayerCanBuild.Add(function(iPlayer, iUnit, iX, iY, iBuild) return true end)
//   GameEvents.PlotCanImprove.Add(function(iX, iY, iImprovement) return true end)
//   GameEvents.PlayerBuilding.Add(function(iPlayer, iUnit, iX, iY, iBuild, bStarting) end) (v46)
//   GameEvents.PlayerBuilt.Add(function(iPlayer, iUnit, iX, iY, iBuild) end) (v46)
#define MOD_EVENTS_PLOT                             gCustomMods.isEVENTS_PLOT()

// Events sent by golden ages (v52)
//   GameEvents.PlayerGoldenAge.Add(function(iPlayer, bStart, iTurns) end)
#define MOD_EVENTS_GOLDEN_AGE                       gCustomMods.isEVENTS_GOLDEN_AGE()

// Events sent after a city produces/buys something
//   GameEvents.CityTrained.Add(function(iPlayer, iCity, iUnit, bGold, bFaith) end)
//   GameEvents.CityConstructed.Add(function(iPlayer, iCity, iBuilding, bGold, bFaith) end)
//   GameEvents.CityCreated.Add(function(iPlayer, iCity, iProject, bGold, bFaith) end)
//   GameEvents.CityPrepared.Add(function(iPlayer, iCity, iSpecialist, bGold, bFaith) end) (v33)
//   GameEvents.CityBoughtPlot.Add(function(iPlayer, iCity, iPlotX, iPlotY, bGold, bCulture) end)
//   GameEvents.CitySoldBuilding.Add(function(iPlayer, iCity, iBuilding) end)
#define MOD_EVENTS_CITY                             gCustomMods.isEVENTS_CITY()

// Event sent to ascertain if a city can acquire a plot (v20)
//   GameEvents.CityCanAcquirePlot.Add(function(iPlayer, iCity, iPlotX, iPlotY) return true end)
#define MOD_EVENTS_CITY_BORDERS                     gCustomMods.isEVENTS_CITY_BORDERS()
	
// Event sent to ascertain if a player can over-ride the standard razing rules for the specified city and raze it anyway (v20)
//   GameEvents.PlayerCanRaze.Add(function(iPlayer, iCity) return false end)
#define MOD_EVENTS_CITY_RAZING                      gCustomMods.isEVENTS_CITY_RAZING()
	
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

// Events sent to ascertain if a unit can rebase to a specific plot (either a city or a carrier)
//   GameEvents.CanLoadAt.Add(function(iPlayer, iUnit, iPlotX, iPlotY) return false end)
//   GameEvents.CanRebaseInCity.Add(function(iPlayer, iUnit, iPlotX, iPlotY) return false end)
//   GameEvents.CanRebaseTo.Add(function(iPlayer, iUnit, iPlotX, iPlotY, bIsCity) return false end)
//   GameEvents.RebaseTo.Add(function(iPlayer, iUnit, iPlotX, iPlotY) end)
#define MOD_EVENTS_REBASE                           gCustomMods.isEVENTS_REBASE()

// Event sent to see if a command is valid (v46)
//   GameEvents.PlayerCanDoCommand.Add(function(iPlayer, iUnit, iCommand, iData1, iData2, iPlotX, iPlotY, bTestVisible) return true end)
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
//   GameEvents.CustomMissionCycleTime.Add(function(iPlayer, iUnit, iMission, iData1, iData2, iFlags, iTurn) return iCameraTime end) -- iCameraTime is 0, 1, 5 or 10
//   GameEvents.CustomMissionTimerInc.Add(function(iPlayer, iUnit, iMission, iData1, iData2, iFlags, iTurn) return iTimerInc end)
#define MOD_EVENTS_CUSTOM_MISSIONS                          gCustomMods.isEVENTS_CUSTOM_MISSIONS()

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
// Fixes the spy name crash (v53)
#define MOD_BUGFIX_SPY_NAMES                        (true)
// Fixes the research overflow bug/exploit (v52)
#define MOD_BUGFIX_RESEARCH_OVERFLOW                gCustomMods.isBUGFIX_RESEARCH_OVERFLOW()
// Fixes the bug where a city doesn't work its centre tile (v45)
#define MOD_BUGFIX_CITY_CENTRE_WORKING              (true)
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
// Fixes a bug in the pathfinder code for hovering units at the seaside!
#define MOD_BUGFIX_HOVERING_PATHFINDER              gCustomMods.isBUGFIX_HOVERING_PATHFINDER()
// Fixes a bug in the pathfinder code for embarking
#define MOD_BUGFIX_EMBARKING_PATHFINDER             gCustomMods.isBUGFIX_EMBARKING_PATHFINDER()


#endif // ACHIEVEMENT_HACKS


//
// MOD_PROMOTIONS_XYZ changes manage/grant the promotions and may affect the game saving code
// MOD_PATHFINDER_XYZ changes only affect the path finding code so can be disabled if necessary
//

#if defined(MOD_PROMOTIONS_CROSS_MOUNTAINS)
#define MOD_PATHFINDER_CROSS_MOUNTAINS         MOD_PROMOTIONS_CROSS_MOUNTAINS
#endif
#if defined(MOD_PROMOTIONS_CROSS_OCEANS)
#define MOD_PATHFINDER_CROSS_OCEANS            MOD_PROMOTIONS_CROSS_OCEANS
#endif
#if defined(MOD_PROMOTIONS_CROSS_ICE)
#define MOD_PATHFINDER_CROSS_ICE               MOD_PROMOTIONS_CROSS_ICE
#endif
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
#define MOD_PATHFINDER_DEEP_WATER_EMBARKATION  MOD_PROMOTIONS_DEEP_WATER_EMBARKATION
#endif

// ONLY CHANGE THIS IF YOU TRULY KNOW WHAT YOU ARE DOING IN THE PATHFINDER CODE!!!
#if defined(MOD_PATHFINDER_CROSS_ICE) || defined(MOD_PATHFINDER_DEEP_WATER_EMBARKATION)
#define MOD_PATHFINDER_TERRAFIRMA
#endif


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


// Custom mod logger
#if defined(CUSTOMLOGDEBUG)
#define CUSTOMLOG(sFmt, ...) {																		\
	CvString sMsg; CvString::format(sMsg, sFmt, __VA_ARGS__);										\
	if (CUSTOMLOGFILEINFO) {																		\
		CvString sLine; CvString::format(sLine, "%s: %i - %s", __FILE__, __LINE__, sMsg.c_str());	\
		LOGFILEMGR.GetLog(CUSTOMLOGDEBUG, FILogFile::kDontTimeStamp)->Msg(sLine.c_str());			\
	} else {																						\
		LOGFILEMGR.GetLog(CUSTOMLOGDEBUG, FILogFile::kDontTimeStamp)->Msg(sMsg.c_str());			\
	}																								\
}
#else
#define CUSTOMLOG(sFmt, ...) __noop
#endif

// Unified yields logger
#if defined(UNIFIEDLOGDEBUG)
#define UNIFIEDLOG(sFmt, ...) {																		\
	CvString sMsg; CvString::format(sMsg, sFmt, __VA_ARGS__);										\
	CvString sLine; CvString::format(sLine, "%s: %i - %s", __FILE__, __LINE__, sMsg.c_str());	    \
	LOGFILEMGR.GetLog(UNIFIEDLOGDEBUG, FILogFile::kDontTimeStamp)->Msg(sLine.c_str());			    \
}
#else
#define UNIFIEDLOG(sFmt, ...) __noop
#endif


// Message wrappers
#define SHOW_PLAYER_MESSAGE(pPlayer, szMessage)       DLLUI->AddMessage(0, pPlayer->GetID(), false, GC.getEVENT_MESSAGE_TIME(), szMessage)
#define SHOW_CITY_MESSAGE(pCity, ePlayer, szMessage)  DLLUI->AddCityMessage(0, pCity->GetIDInfo(), ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szMessage)
#define SHOW_UNIT_MESSAGE(pUnit, ePlayer, szMessage)  DLLUI->AddUnitMessage(0, pUnit->GetIDInfo(), ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szMessage)
#define SHOW_PLOT_MESSAGE(pPlot, ePlayer, szMessage)  DLLUI->AddPlotMessage(0, pPlot->GetPlotIndex(), ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szMessage)
#define SHOW_PLOT_POPUP(pPlot, ePlayer, szMessage, fDelay)  pPlot->showPopupText(ePlayer, szMessage)


// GlobalDefines wrappers
#define GD_INT_DECL(name)       int m_i##name
#define GD_INT_DEF(name)        inline int get##name() { return m_i##name; }
#define GD_INT_INIT(name, def)  m_i##name(def)
#define GD_INT_CACHE(name)      m_i##name = getDefineINT(#name); CUSTOMLOG("<Defines>: %s = %i", #name, m_i##name)
#define GD_INT_GET(name)        GC.get##name()


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

#define GAMEEVENT_AiOverrideChooseNextTech	"AiOverrideChooseNextTech",		"ib"
#define GAMEEVENT_AreaCanHaveAnyResource	"AreaCanHaveAnyResource",		"ii"
#define GAMEEVENT_CanDoCommand				"CanDoCommand",					"iiiiiiib"
#define GAMEEVENT_CanHaveAnyUpgrade			"CanHaveAnyUpgrade",			"ii"
#define GAMEEVENT_CanHavePromotion			"CanHavePromotion",				"iii"
#define GAMEEVENT_CanHaveUpgrade			"CanHaveUpgrade",				"iiii"
#define GAMEEVENT_CanLoadAt					"CanLoadAt",					"iiii"
#define GAMEEVENT_CanMoveInto				"CanMoveInto",					"iiiibb"
#define GAMEEVENT_CannotParadropFrom		"CannotParadropFrom",			"iiii"
#define GAMEEVENT_CanParadropFrom			"CanParadropFrom",				"iiii"
#define GAMEEVENT_CanRebaseInCity			"CanRebaseInCity",				"iiii"
#define GAMEEVENT_CanRebaseTo				"CanRebaseTo",					"iiiib"
#define GAMEEVENT_CircumnavigatedGlobe		"CircumnavigatedGlobe",			"i"
#define GAMEEVENT_CityBoughtPlot			"CityBoughtPlot",				"iiiibb"
#define GAMEEVENT_CityCanAcquirePlot		"CityCanAcquirePlot",			"iiii"
#define GAMEEVENT_CityConnected				"CityConnected",				"iiiiib"
#define GAMEEVENT_CityConnections			"CityConnections",				"ib"
#define GAMEEVENT_CityConstructed			"CityConstructed",				"iiibb"
#define GAMEEVENT_CityCreated				"CityCreated",					"iiibb"
#define GAMEEVENT_CityPrepared				"CityPrepared",					"iiibb"
#define GAMEEVENT_CitySoldBuilding			"CitySoldBuilding",				"iii"
#define GAMEEVENT_CityTrained				"CityTrained",					"iiibb"
#define GAMEEVENT_CustomMissionCompleted	"CustomMissionCompleted",		"iiiiiii"
#define GAMEEVENT_CustomMissionCameraTime	"CustomMissionCameraTime",		"iiiiiii"
#define GAMEEVENT_CustomMissionDoStep		"CustomMissionDoStep",			"iiiiiii"
#define GAMEEVENT_CustomMissionPossible		"CustomMissionPossible",		"iiiiiiiiib"
#define GAMEEVENT_CustomMissionStart		"CustomMissionStart",			"iiiiiii"
#define GAMEEVENT_CustomMissionSetActivity	"CustomMissionSetActivity",		"iiiiiii"
#define GAMEEVENT_CustomMissionTargetPlot	"CustomMissionTargetPlot",		"iiiiiii"
#define GAMEEVENT_CustomMissionTimerInc		"CustomMissionTimerInc",		"iiiiiii"
#define GAMEEVENT_DeclareWar				"DeclareWar",					"iib"
#define GAMEEVENT_EspionageResult			"EspionageResult",				"iiiii"
#define GAMEEVENT_EspionageState			"EspionageState",				"iiiii"
#define GAMEEVENT_GetDiploModifier			"GetDiploModifier",				"iii"
#define GAMEEVENT_GetBombardRange			"GetBombardRange",				"ii"
#define GAMEEVENT_GetReligionToFound		"GetReligionToFound",			"iib"
#define GAMEEVENT_GoodyHutCanNotReceive		"GoodyHutCanNotReceive",		"iiib"
#define GAMEEVENT_GoodyHutCanResearch		"GoodyHutCanResearch",			"ii"
#define GAMEEVENT_GoodyHutTechResearched	"GoodyHutTechResearched",		"ii"
#define GAMEEVENT_GreatPersonExpended		"GreatPersonExpended",			"iiiii"
#define GAMEEVENT_IsAbleToDeclareWar		"IsAbleToDeclareWar",			"ii"
#define GAMEEVENT_IsAbleToMakePeace			"IsAbleToMakePeace",			"ii"
#define GAMEEVENT_MakePeace					"MakePeace",					"iib"
#define GAMEEVENT_MinorAlliesChanged		"MinorAlliesChanged",			"iibii"
#define GAMEEVENT_MinorFriendsChanged		"MinorFriendsChanged",			"iibii"
#define GAMEEVENT_NaturalWonderDiscovered	"NaturalWonderDiscovered",		"iiiib"
#define GAMEEVENT_NuclearDetonation			"NuclearDetonation",			"iiibb"
#define GAMEEVENT_PantheonFounded			"PantheonFounded",				"iiii"
#define GAMEEVENT_ParadropAt				"ParadropAt",					"iiiiii"
#define GAMEEVENT_PlaceResource				"PlaceResource",				"iiiii"
#define GAMEEVENT_PlayerBuilding			"PlayerBuilding",				"iiiiib"
#define GAMEEVENT_PlayerBuilt				"PlayerBuilt",					"iiiii"
#define GAMEEVENT_PlayerCanAdoptIdeology	"PlayerCanAdoptIdeology",		"ii"
#define GAMEEVENT_PlayerCanAdoptTenet		"PlayerCanAdoptTenet",			"ii"
#define GAMEEVENT_PlayerCanBuild			"PlayerCanBuild",				"iiiii"
#define GAMEEVENT_PlayerCanDeclareWar		"PlayerCanDeclareWar",			"ii"
#define GAMEEVENT_PlayerCanFoundPantheon	"PlayerCanFoundPantheon",		"i"
#define GAMEEVENT_PlayerCanFoundReligion	"PlayerCanFoundReligion",		"ii"
#define GAMEEVENT_PlayerCanHaveBelief		"PlayerCanHaveBelief",			"ii"
#define GAMEEVENT_PlayerCanMakePeace		"PlayerCanMakePeace",			"ii"
#define GAMEEVENT_PlayerCanPropose			"PlayerCanPropose",				"iiib"
#define GAMEEVENT_PlayerCanRaze				"PlayerCanRaze",				"ii"
#define GAMEEVENT_PlayerCanRemoveHeresy		"PlayerCanRemoveHeresy",		"iiii"
#define GAMEEVENT_PlayerCanSpreadReligion	"PlayerCanSpreadReligion",		"iiii"
#define GAMEEVENT_PlayerGoldenAge			"PlayerGoldenAge",				"ibi"
#define GAMEEVENT_PlayerPlunderedTradeRoute	"PlayerPlunderedTradeRoute",	"iiiiiiiii"
#define GAMEEVENT_PlotCanImprove			"PlotCanImprove",				"iii"
#define GAMEEVENT_RebaseTo					"RebaseTo",						"iiii"
#define GAMEEVENT_ReligionCanHaveBelief		"ReligionCanHaveBelief",		"iii"
#define GAMEEVENT_ReligionEnhanced			"ReligionEnhanced",				"iiii"
#define GAMEEVENT_ReligionFounded			"ReligionFounded",				"iiiiiiii"
#define GAMEEVENT_ReligionReformed			"ReligionReformed",				"iiiiiii"
#define GAMEEVENT_ResolutionResult			"ResolutionResult",				"iibb"
#define GAMEEVENT_TeamSetEra				"TeamSetEra",					"iib"
#define GAMEEVENT_TerraformingMap			"TerraformingMap",				"ii"
#define GAMEEVENT_TerraformingPlot			"TerraformingPlot",				"iiiiiiii"
#define GAMEEVENT_TileFeatureChanged		"TileFeatureChanged",			"iiiii"
#define GAMEEVENT_TileImprovementChanged	"TileImprovementChanged",		"iiiiib"
#define GAMEEVENT_TileOwnershipChanged		"TileOwnershipChanged",			"iiii"
#define GAMEEVENT_TileRevealed				"TileRevealed",					"iiiib"
#define GAMEEVENT_TileRouteChanged			"TileRouteChanged",				"iiiiib"
#define GAMEEVENT_UiDiploEvent				"UiDiploEvent",					"iiii"
#define GAMEEVENT_UnitCanHaveAnyUpgrade		"UnitCanHaveAnyUpgrade",		"ii"
#define GAMEEVENT_UnitCanHaveGreatWork		"UnitCanHaveGreatWork",			"iii"
#define GAMEEVENT_UnitCanHaveName			"UnitCanHaveName",				"iii"
#define GAMEEVENT_UnitCanHavePromotion		"UnitCanHavePromotion",			"iii"
#define GAMEEVENT_UnitCanHaveUpgrade		"UnitCanHaveUpgrade",			"iiii"
#define GAMEEVENT_UnitCaptureType			"UnitCaptureType",				"iiii"
#define GAMEEVENT_UnitCityFounded			"UnitCityFounded",				"iiiii"
#define GAMEEVENT_UnitCreated				"UnitCreated",					"iiiii"
#define GAMEEVENT_UnitPrekill				"UnitPrekill",					"iiiiibi"
#define GAMEEVENT_UnitPromoted				"UnitPromoted",					"iii"
#define GAMEEVENT_UnitUpgraded				"UnitUpgraded",					"iiib"


// Serialization wrappers
#define MOD_SERIALIZE

#if defined(MOD_SERIALIZE)
#define MOD_SERIALIZE_INIT_READ(stream) uint uiDllSaveVersion; stream >> uiDllSaveVersion
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
#define MOD_SERIALIZE_INIT_WRITE(stream) uint uiDllSaveVersion = MOD_DLL_VERSION_NUMBER; stream << uiDllSaveVersion
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
	MOD_OPT_DECL(GLOBAL_CITY_WORKING);
	MOD_OPT_DECL(GLOBAL_ALPINE_PASSES);
	MOD_OPT_DECL(GLOBAL_CS_GIFT_SHIPS);
	MOD_OPT_DECL(GLOBAL_CS_UPGRADES);
	MOD_OPT_DECL(GLOBAL_CS_RAZE_RARELY);
	MOD_OPT_DECL(GLOBAL_CS_LIBERATE_AFTER_BUYOUT);
	MOD_OPT_DECL(GLOBAL_CS_GIFTS);
	MOD_OPT_DECL(GLOBAL_CS_OVERSEAS_TERRITORY);
	MOD_OPT_DECL(GLOBAL_CS_NO_ALLIED_SKIRMISHES);
	MOD_OPT_DECL(GLOBAL_VENICE_KEEPS_RESOURCES);
	MOD_OPT_DECL(GLOBAL_NO_FOLLOWUP_FROM_CITIES);
	MOD_OPT_DECL(GLOBAL_CAPTURE_AFTER_ATTACKING);
	MOD_OPT_DECL(GLOBAL_NO_OCEAN_PLUNDERING);
	MOD_OPT_DECL(GLOBAL_NO_CONQUERED_SPACESHIPS);
	MOD_OPT_DECL(GLOBAL_ADJACENT_BLOCKADES);
	MOD_OPT_DECL(GLOBAL_ALLIES_BLOCK_BLOCKADES);
	MOD_OPT_DECL(GLOBAL_SHORT_EMBARKED_BLOCKADES);
	MOD_OPT_DECL(GLOBAL_GRATEFUL_SETTLERS);
	MOD_OPT_DECL(GLOBAL_RELIGIOUS_SETTLERS);
	MOD_OPT_DECL(GLOBAL_QUICK_ROUTES);
	MOD_OPT_DECL(GLOBAL_SUBS_UNDER_ICE_IMMUNITY);
	MOD_OPT_DECL(GLOBAL_PARATROOPS_MOVEMENT);
	MOD_OPT_DECL(GLOBAL_PARATROOPS_AA_DAMAGE);
	MOD_OPT_DECL(GLOBAL_NUKES_MELT_ICE); 
	MOD_OPT_DECL(GLOBAL_GREATWORK_YIELDTYPES); 
	MOD_OPT_DECL(GLOBAL_EXCLUDE_FROM_GIFTS);
	MOD_OPT_DECL(GLOBAL_MOVE_AFTER_UPGRADE);
	MOD_OPT_DECL(GLOBAL_CANNOT_EMBARK);
	MOD_OPT_DECL(GLOBAL_SEPARATE_GP_COUNTERS);
	MOD_OPT_DECL(GLOBAL_TRULY_FREE_GP);
	
	MOD_OPT_DECL(DIPLOMACY_TECH_BONUSES);
	MOD_OPT_DECL(DIPLOMACY_AUTO_DENOUNCE);
	MOD_OPT_DECL(DIPLOMACY_CITYSTATES); 
	MOD_OPT_DECL(DIPLOMACY_CITYSTATES_QUESTS); 
	MOD_OPT_DECL(DIPLOMACY_CITYSTATES_RESOLUTIONS); 
	MOD_OPT_DECL(DIPLOMACY_CITYSTATES_HURRY);

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
	MOD_OPT_DECL(BALANCE_CORE_DIPLOMACY_ADVANCED);
	MOD_OPT_DECL(BALANCE_CORE_LUXURIES_TRAIT);
	MOD_OPT_DECL(BALANCE_CORE_MILITARY_PROMOTION_ADVANCED);
	MOD_OPT_DECL(BALANCE_CORE_MILITARY_LOGGING);
	MOD_OPT_DECL(BALANCE_CORE_RESOURCE_MONOPOLIES);
	MOD_OPT_DECL(BALANCE_CORE_BUILDING_INVESTMENTS);
	MOD_OPT_DECL(BALANCE_CORE_ENGINEER_HURRY);
	MOD_OPT_DECL(BALANCE_CORE_MAYA_CHANGE);
	MOD_OPT_DECL(BALANCE_CORE_PORTUGAL_CHANGE);
	MOD_OPT_DECL(BALANCE_CORE_MINOR_VARIABLE_BULLYING);
	MOD_OPT_DECL(BALANCE_SPECIALIST_FOOD_SCALE_ERA);
	MOD_OPT_DECL(BALANCE_CORE_NEW_GP_ATTRIBUTES);

	MOD_OPT_DECL(DIPLOMACY_CIV4_FEATURES); 

	MOD_OPT_DECL(TRAITS_CROSSES_ICE);
	MOD_OPT_DECL(TRAITS_CITY_WORKING);
	MOD_OPT_DECL(TRAITS_ANY_BELIEF);
	MOD_OPT_DECL(TRAITS_TRADE_ROUTE_BONUSES);

	MOD_OPT_DECL(POLICIES_CITY_WORKING);

	MOD_OPT_DECL(TECHS_CITY_WORKING);

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

	MOD_OPT_DECL(AI_SMART);
	MOD_OPT_DECL(AI_SMART_DEALS);
	MOD_OPT_DECL(AI_SMART_GREAT_PEOPLE);
	MOD_OPT_DECL(AI_SMART_GRAND_STRATEGY);
	MOD_OPT_DECL(AI_SMART_POLICY_CHOICE);
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
	MOD_OPT_DECL(EVENTS_CITY_BORDERS);
	MOD_OPT_DECL(EVENTS_CITY_RAZING);
	MOD_OPT_DECL(EVENTS_CITY_BOMBARD);
	MOD_OPT_DECL(EVENTS_CITY_CONNECTIONS);
	MOD_OPT_DECL(EVENTS_AREA_RESOURCES);
	MOD_OPT_DECL(EVENTS_PARADROPS);
	MOD_OPT_DECL(EVENTS_UNIT_CREATED);
	MOD_OPT_DECL(EVENTS_UNIT_FOUNDED);
	MOD_OPT_DECL(EVENTS_UNIT_PREKILL);
	MOD_OPT_DECL(EVENTS_UNIT_CAPTURE);
	MOD_OPT_DECL(EVENTS_CAN_MOVE_INTO);
	MOD_OPT_DECL(EVENTS_UNIT_UPGRADES);
	MOD_OPT_DECL(EVENTS_UNIT_DATA);
	MOD_OPT_DECL(EVENTS_TRADE_ROUTE_PLUNDERED);
	MOD_OPT_DECL(EVENTS_WAR_AND_PEACE);
	MOD_OPT_DECL(EVENTS_RESOLUTIONS);
	MOD_OPT_DECL(EVENTS_IDEOLOGIES);
	MOD_OPT_DECL(EVENTS_NUCLEAR_DETONATION);
	MOD_OPT_DECL(EVENTS_REBASE);
	MOD_OPT_DECL(EVENTS_COMMAND);
	MOD_OPT_DECL(EVENTS_CUSTOM_MISSIONS);

	MOD_OPT_DECL(EVENTS_RED_TURN);
	MOD_OPT_DECL(EVENTS_RED_COMBAT);
	MOD_OPT_DECL(EVENTS_RED_COMBAT_MISSION);
	MOD_OPT_DECL(EVENTS_RED_COMBAT_ABORT);
	MOD_OPT_DECL(EVENTS_RED_COMBAT_RESULT);
	MOD_OPT_DECL(EVENTS_RED_COMBAT_ENDED);

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

protected:
	bool m_bInit;
	std::map<std::string, int> m_options;
};

extern CustomMods gCustomMods;
#endif