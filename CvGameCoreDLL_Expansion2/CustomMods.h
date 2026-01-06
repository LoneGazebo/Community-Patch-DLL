// CustomMods.h
#pragma once

#ifndef CUSTOM_MODS_H
#define CUSTOM_MODS_H

// Custom #defines used by all DLLs
#include "../CvGameCoreDLLUtil/include/CustomModsGlobal.h"

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
#define MOD_DLL_NAME "Community Patch v148 (PNM v51+)"
#define MOD_DLL_VERSION_NUMBER ((uint) 148)
#define MOD_DLL_VERSION_STATUS ""			// a (alpha), b (beta) or blank (released)
#define MOD_DLL_CUSTOM_BUILD_NAME ""

/// IMPORTANT NOTES: Do NOT disable CustomModOptions by commenting out the defines in this file. This could cause the DLL to break. Instead, switch off the CustomModOption in the database by setting it to 0.
/// Do not use #if defined(MOD_OPTION) to selectively enable code elsewhere in the DLL. Use if (MOD_OPTION) instead - this will cause the code to only run if the associated option is set to 1 in the database.
/// Any instances of a #define MOD_OPTION with no call to gCustomMods next to it is legacy code and needs to be fixed.

/////////////////////////////////////////
// KEYSTONE OPTIONS
/////////////////////////////////////////

// All changes added by VP that aren't covered by another CustomModOption
#define MOD_BALANCE_VP												gCustomMods.isBALANCE_VP()

// Adds the ability for AI players to contact human players in multiplayer with trade deals
#define MOD_ACTIVE_DIPLOMACY										gCustomMods.isACTIVE_DIPLOMACY()

// Enables runtime interoperability features for enhanced multiplayer functionality
#define MOD_BIN_HOOKS												gCustomMods.isBIN_HOOKS()

// FUNCTIONALITY NOT GUARANTEED
// Allows Steam achievements to be obtained in single player games despite the game being modded
#define MOD_ENABLE_ACHIEVEMENTS										gCustomMods.isENABLE_ACHIEVEMENTS()

// Enables the Diplomacy AI Debug Mode: always reveals true opinion/approach/congress voting info for AI players, and also provides extra information about key players
#define MOD_DIPLO_DEBUG_MODE										gCustomMods.isDIPLO_DEBUG_MODE()

// Enables additional gamecore debugging functionality, but slows performance
#define MOD_CORE_DEBUGGING											gCustomMods.isCORE_DEBUGGING()

// Enables Squads for VP support
#define MOD_SQUADS													gCustomMods.isSQUADS()

// Comment out this line to remove minidumps - see http://forums.civfanatics.com/showthread.php?t=498919
// If minidumps are enabled, do NOT set GenerateDebugInfo=No (Props -> Config Props -> Linker -> Debugging)
#define MOD_DEBUG_MINIDUMP


/////////////////////////////////////////
// CORE BALANCE CHANGES
// Enabled by default in Community Patch Only and Vox Populi
// This is not a complete list of core balance changes - many more are integrated with no option to disable them
// Sorting order: most to least impactful
/////////////////////////////////////////

// Visible tiles stay visible until the end of the turn
#define MOD_CORE_DELAYED_VISIBILITY									gCustomMods.isCORE_DELAYED_VISIBILITY()

// Garrisons absorb some of the damage from non-aerial attacks against the city
#define MOD_CORE_GARRISON_DAMAGE_ABSORPTION							gCustomMods.isCORE_GARRISON_DAMAGE_ABSORPTION()

// Non-Venetian puppet cities can only build buildings from previous eras, and won't build non-defensive buildings if the empire has a Gold deficit
#define MOD_CORE_PUPPET_BUILDING_LIMITATIONS						gCustomMods.isCORE_PUPPET_BUILDING_LIMITATIONS()

// When a military unit retreats, civilian and embarked units on the same tile also retreat
#define MOD_CORE_CIVILIANS_RETREAT_WITH_MILITARY					gCustomMods.isCORE_CIVILIANS_RETREAT_WITH_MILITARY()

// Defensive Pacts don't end if you declare war on civs, other than the player you made the agreement with of course
#define MOD_CORE_PERSISTENT_DEFENSIVE_PACTS							gCustomMods.isCORE_PERSISTENT_DEFENSIVE_PACTS()

// Great Engineers' Hurry Production produces completed buildings on the same turn as the Great Engineer is expended
#define MOD_CORE_ENGINEER_HURRY										gCustomMods.isCORE_ENGINEER_HURRY()

// Naval units cannot do ranged attacks from cities or passable improvements
#define MOD_CORE_NO_NAVAL_RANGED_ATTACKS_FROM_CANALS				gCustomMods.isCORE_NO_NAVAL_RANGED_ATTACKS_FROM_CANALS()

// Prevents repairing improvements in foreign lands with Workers, blocking the pillage-repair loop exploit
#define MOD_CORE_NO_REPAIRING_FOREIGN_LANDS							gCustomMods.isCORE_NO_REPAIRING_FOREIGN_LANDS()

// Disallows gifting units to major civs, since the AI cannot effectively use this
#define MOD_CORE_NO_INTERMAJOR_UNIT_GIFTING							gCustomMods.isCORE_NO_INTERMAJOR_UNIT_GIFTING()

// Disallows healing on Mountain tiles, except in Cities
#define MOD_CORE_NO_HEALING_ON_MOUNTAINS							gCustomMods.isCORE_NO_HEALING_ON_MOUNTAINS()

// Tiles with Ice do not directly give any yields, even if one would be granted by Improvements, Policies, etc.
#define MOD_CORE_NO_YIELD_ICE										gCustomMods.isCORE_NO_YIELD_ICE()


/////////////////////////////////////////
// CORE USER INTERFACE CHANGES
// Enabled by default in Community Patch Only and Vox Populi
// Many more are integrated with no option to disable them
// Sorting order: most to least impactful
/////////////////////////////////////////

// Turns off some of the less important notifications
#define MOD_COREUI_REDUCE_NOTIFICATIONS								gCustomMods.isCOREUI_REDUCE_NOTIFICATIONS()

// Enables era-specific messages for civ influence; disabling this may be useful for mods like Unique Cultural Influence that add special notifications
#define MOD_COREUI_DIPLOMACY_ERA_INFLUENCE							gCustomMods.isCOREUI_DIPLOMACY_ERA_INFLUENCE()


/////////////////////////////////////////
// CORE AI CHANGES
// Enabled by default in Community Patch Only and Vox Populi
// Many more are integrated with no option to disable them
// Sorting order: category names by alphabetical order and options within a category by most to least impactful
/////////////////////////////////////////

// Humans can trade temporary items (gold per turn, resources, etcetera) for permanent items (lump sum gold, cities, etcetera) with AI players
#define MOD_DEALAI_HUMAN_PERMANENT_FOR_AI_TEMPORARY					gCustomMods.isDEALAI_HUMAN_PERMANENT_FOR_AI_TEMPORARY()

// Outside of peace deals or teammates, prohibits all AI players from buying cities that the seller originally founded
#define MOD_DEALAI_NO_PEACETIME_SELLING_FOUNDED_CITIES				gCustomMods.isDEALAI_NO_PEACETIME_SELLING_FOUNDED_CITIES()

// Gandhis who have nukes or have nuked someone and aren't vassals will do what they're best known for in the Civilization series ;)
#define MOD_DIPLOAI_ENABLE_NUCLEAR_GANDHI							gCustomMods.isDIPLOAI_ENABLE_NUCLEAR_GANDHI()

// AI civilizations are unable to fake having no disputes by displaying false modifiers in the opinion table (i.e. no contested borders, no competition with City-States, etc.)
// Does not prevent the AI from concealing their true Approach (i.e. pretending to be FRIENDLY or displaying "They desire friendly relations with our empire")
// NOTE: If AI is blocked from hiding opinion modifiers, e.g., through Transparent Diplomacy, they will already be blocked from faking them
#define MOD_DIPLOAI_HONEST_OPINION_MODIFIERS						(MOD_DIPLO_DEBUG_MODE || gCustomMods.isDIPLOAI_HONEST_OPINION_MODIFIERS() || gCustomMods.isDIPLOAI_SHOW_HIDDEN_OPINION_MODIFIERS())


/////////////////////////////////////////
// CORE PICK'N'MIX MODS
// Enabled by default in Community Patch Only and Vox Populi
// Other supported Pick'N'Mix mods are in separate sections below - many more are integrated with no option to disable them
// Sorting order: most to least impactful
/////////////////////////////////////////

// Removes free Great People (from buildings, policies, traits, etc.) from the Great People counters
#define MOD_GLOBAL_TRULY_FREE_GP									gCustomMods.isGLOBAL_TRULY_FREE_GP()

// City States can be liberated after they have been "bought" (Austria's or Venice's UA)
#define MOD_GLOBAL_CS_LIBERATE_AFTER_BUYOUT							gCustomMods.isGLOBAL_CS_LIBERATE_AFTER_BUYOUT()

// Great Artists, Writers and Musicians that do NOT create Great Works can be "reborn"
#define MOD_GLOBAL_NO_LOST_GREATWORKS								gCustomMods.isGLOBAL_NO_LOST_GREATWORKS()

// When a Great Prophet spawns with a 100% spawn chance, any overflow Faith is kept, not discarded
#define MOD_RELIGION_KEEP_PROPHET_OVERFLOW							gCustomMods.isRELIGION_KEEP_PROPHET_OVERFLOW()


/////////////////////////////////////////
// ENABLED PICK'N'MIX MODS
// Enabled by default in Vox Populi
// Sorting order: most to least impactful
/////////////////////////////////////////

// Separates the Great People counters for Engineers, Scientists, and Merchants
#define MOD_GLOBAL_SEPARATE_GP_COUNTERS								gCustomMods.isGLOBAL_SEPARATE_GP_COUNTERS()

// Permits ships to enter coastal forts/citadels in friendly lands
#define MOD_GLOBAL_PASSABLE_FORTS									gCustomMods.isGLOBAL_PASSABLE_FORTS()

// Paratroopers take interception damage from hostile anti-air units
#define MOD_GLOBAL_PARATROOPS_AA_DAMAGE								gCustomMods.isGLOBAL_PARATROOPS_AA_DAMAGE()

// Instead of always giving Gold, City-States give different first contact gifts depending on their trait (Cultural, Religious, Maritime, etc.)
#define MOD_GLOBAL_CS_GIFTS											gCustomMods.isGLOBAL_CS_GIFTS()

// Permits coastal City-States to gift Naval Units, as a quest reward or if Militaristic
#define MOD_GLOBAL_CS_GIFT_SHIPS									gCustomMods.isGLOBAL_CS_GIFT_SHIPS()

// Units gifted by City-States receive XP from their spawning city, not the City-State's capital
#define MOD_GLOBAL_CS_GIFTS_LOCAL_XP								gCustomMods.isGLOBAL_CS_GIFTS_LOCAL_XP()

// Give initial production boost for cities founded on forests, as if the forest had been chopped down by a worker
#define MOD_GLOBAL_CITY_FOREST_BONUS								gCustomMods.isGLOBAL_CITY_FOREST_BONUS()

// Give initial production boost for cities founded on jungles, as if the jungle had been chopped down by a worker
#define MOD_GLOBAL_CITY_JUNGLE_BONUS								gCustomMods.isGLOBAL_CITY_JUNGLE_BONUS()


/////////////////////////////////////////
// DISABLED PICK'N'MIX MODS
// Sorting order: alphabetical
/////////////////////////////////////////

// National Wonders (NW) requiring a building in every city exclude those being razed (in addition to excluding puppets)
#define MOD_BUILDINGS_NW_EXCLUDE_RAZING								gCustomMods.isBUILDINGS_NW_EXCLUDE_RAZING()

// Purchase of buildings in cities allows for any current production (current production is depreciated)
#define MOD_BUILDINGS_PRO_RATA_PURCHASE								gCustomMods.isBUILDINGS_PRO_RATA_PURCHASE()

// The tech cost reduction from other teams having researched a tech requires an embassy or spy and not just having met them
#define MOD_DIPLOMACY_TECH_BONUSES									gCustomMods.isDIPLOMACY_TECH_BONUSES()

// If City-States are allied to a major, other players cannot enter their territory unless they would be able to enter the major's territory
#define MOD_GLOBAL_CS_OVERSEAS_TERRITORY							gCustomMods.isGLOBAL_CS_OVERSEAS_TERRITORY()

// Permits units to upgrade in allied militaristic City-States, but increases the # of turns until the next free unit spawns by UPGRADE_EXTRA_TURNS_UNIT_SPAWN, scaling with game speed
#define MOD_GLOBAL_CS_UPGRADES										gCustomMods.isGLOBAL_CS_UPGRADES()

// Other players' Settlers recaptured from Barbarians will sometimes remain as Settlers
#define MOD_GLOBAL_GRATEFUL_SETTLERS								gCustomMods.isGLOBAL_GRATEFUL_SETTLERS()

// Great Generals and Admirals spawned from combat experience appear on top of the unit that triggered the spawn, and not in a distant city
// If LOCAL_GENERALS_NEAREST_CITY is also enabled, the general or admiral instead spawns in the closest city to the triggering unit
#define MOD_GLOBAL_LOCAL_GENERALS									gCustomMods.isGLOBAL_LOCAL_GENERALS()
#define MOD_LOCAL_GENERALS_NEAREST_CITY								(MOD_GLOBAL_LOCAL_GENERALS && gCustomMods.isLOCAL_GENERALS_NEAREST_CITY())

// Removes assembled spaceship parts from conquered capitals
#define MOD_GLOBAL_NO_CONQUERED_SPACESHIPS							gCustomMods.isGLOBAL_NO_CONQUERED_SPACESHIPS()

// Trade Routes can't be plundered on deep ocean tiles - too much sea to hide in, too many directions to escape in
#define MOD_GLOBAL_NO_OCEAN_PLUNDERING								gCustomMods.isGLOBAL_NO_OCEAN_PLUNDERING()

// Nukes melt Ice (same chance as for fallout to appear); however, as neither the 3D nor the SV map update as the Ice is removed, it's fairly pointless!
#define MOD_GLOBAL_NUKES_MELT_ICE									gCustomMods.isGLOBAL_NUKES_MELT_ICE()

// Allows purchasing buildings that have a Faith cost in puppet cities
#define MOD_GLOBAL_PURCHASE_FAITH_BUILDINGS_IN_PUPPETS				gCustomMods.isGLOBAL_PURCHASE_FAITH_BUILDINGS_IN_PUPPETS()

// Units that can found a city take their religion with them
// If RELIGIOUS_SETTLERS_WEIGHTED=1, it's instead a weighted random choice, with each religion having a weight equal to its local followers
#define MOD_GLOBAL_RELIGIOUS_SETTLERS								gCustomMods.isGLOBAL_RELIGIOUS_SETTLERS()
#define MOD_RELIGIOUS_SETTLERS_WEIGHTED								(MOD_GLOBAL_RELIGIOUS_SETTLERS && gCustomMods.isRELIGIOUS_SETTLERS_WEIGHTED())

// Submarines under ice are immune to ranged attacks, except from other submarines
#define MOD_GLOBAL_SUBS_UNDER_ICE_IMMUNITY							gCustomMods.isGLOBAL_SUBS_UNDER_ICE_IMMUNITY()

// Enables production to be stockpiled
#define MOD_PROCESS_STOCKPILE										gCustomMods.isPROCESS_STOCKPILE()

// Inquisitors will keep foreign religions out of allied City-State cities if positioned adjacent
#define MOD_RELIGION_ALLIED_INQUISITORS								gCustomMods.isRELIGION_ALLIED_INQUISITORS()

// Removes religion preference
// Kept the typo for backwards compatibility (sigh)
#define MOD_RELIGION_NO_PREFERENCES									(gCustomMods.isRELIGION_NO_PREFERENCES() || gCustomMods.isRELIGION_NO_PREFERRENCES())
// Randomises religion choice if preferred religion is unavailable
#define MOD_RELIGION_RANDOMISE										gCustomMods.isRELIGION_RANDOMISE()

// Send purchase notifications at every boundary and not just the first
#define MOD_RELIGION_RECURRING_PURCHASE_NOTIFY						gCustomMods.isRELIGION_RECURRING_PURCHASE_NOTIFY()

// Permit human players to choose their own city tiles due to cultural expansion
#define MOD_UI_CITY_EXPANSION										gCustomMods.isUI_CITY_EXPANSION()

// Hovering unit can only heal over land
#define MOD_UNITS_HOVERING_LAND_ONLY_HEAL							gCustomMods.isUNITS_HOVERING_LAND_ONLY_HEAL()

// Changes the stacking limits based on what the tile is (city, fort, plain, etc.)
// FIXME: Disabled for now; this needs to be compared to the VMC DLL to see if it still works properly
#define MOD_GLOBAL_STACKING_RULES									(false)

// Route To will only build roads, or upgrade road to rail, for human players
// FIXME: Disabled for now because Route To Mode is broken
#define MOD_GLOBAL_QUICK_ROUTES										(false)


/////////////////////////////////////////
// VOX POPULI BALANCE CHANGES
// Enabled by default in Vox Populi
// Sorting order: civs, victory, then most to least impactful
/////////////////////////////////////////

// Indonesia's unique luxuries spawn around the city instead of under the city tile, and also appear when conquering a city
#define MOD_BALANCE_ALTERNATE_INDONESIA_TRAIT						gCustomMods.isBALANCE_ALTERNATE_INDONESIA_TRAIT()

// The Maya cannot obtain a Great Prophet from their trait unless they have a religion (owned or majority), or the 9th or later Baktuns have been reached
#define MOD_BALANCE_ALTERNATE_MAYA_TRAIT							gCustomMods.isBALANCE_ALTERNATE_MAYA_TRAIT()

// Siam's trait now affects Resources, Happiness, Unit Gift frequency, and all Yields from City-States; it no longer gives bonus XP
#define MOD_BALANCE_ALTERNATE_SIAM_TRAIT							gCustomMods.isBALANCE_ALTERNATE_SIAM_TRAIT()

// Beliefs with a RequiredCivilization only provide their benefits to that civilization
#define MOD_BALANCE_UNIQUE_BELIEFS_ONLY_FOR_CIV						gCustomMods.isBALANCE_UNIQUE_BELIEFS_ONLY_FOR_CIV()

// Culture Victory also requires an Ideology with a Content Population and the Citizen Earth Protocol in order to win
#define MOD_BALANCE_CULTURE_VICTORY_CHANGES							gCustomMods.isBALANCE_CULTURE_VICTORY_CHANGES()

// Purchasing buildings with Gold instead invests in them, reducing their Production cost by 50% (25% for Wonders); also reduces the final Gold cost of buildings by 40%
#define MOD_BALANCE_BUILDING_INVESTMENTS							gCustomMods.isBALANCE_BUILDING_INVESTMENTS()

// Resources can give monopoly bonuses
#define MOD_BALANCE_RESOURCE_MONOPOLIES								gCustomMods.isBALANCE_RESOURCE_MONOPOLIES()

// Enables/disables Strategic Monopolies, does nothing if BALANCE_RESOURCE_MONOPOLIES=0
#define MOD_BALANCE_STRATEGIC_RESOURCE_MONOPOLIES					gCustomMods.isBALANCE_STRATEGIC_RESOURCE_MONOPOLIES()

// Instead of enslaving a Worker, the "heavy tribute" option awards the same amount of Gold as normal tribute, plus half of all personal quest rewards the City-State is currently offering
#define MOD_BALANCE_HEAVY_TRIBUTE									gCustomMods.isBALANCE_HEAVY_TRIBUTE()

// Adds military strength, connection, and Influence requirements for pledging to protect a City-State, and enables the PtP revocation system
#define MOD_BALANCE_MINOR_PROTECTION_REQUIREMENTS					gCustomMods.isBALANCE_MINOR_PROTECTION_REQUIREMENTS()

// Increases city HP for each Citizen, removes garrisoned unit strength from city ranged strikes, removes Technologies' and (for major civs only) Citizens' effect on city strength
#define MOD_BALANCE_CITY_STRENGTH_SWITCH							gCustomMods.isBALANCE_CITY_STRENGTH_SWITCH()

// Allows all civs to form City Connections along Rivers
#define MOD_BALANCE_RIVER_CITY_CONNECTIONS							gCustomMods.isBALANCE_RIVER_CITY_CONNECTIONS()

// Enables the Spy Point system and the number of spies scaling with map size
#define MOD_BALANCE_SPY_POINTS										gCustomMods.isBALANCE_SPY_POINTS()

// Enables VP-specific adjustments to air warfare, such as units and cities having air defense
#define MOD_BALANCE_AIR_UNIT_CHANGES								gCustomMods.isBALANCE_AIR_UNIT_CHANGES()

// Ranged attacks on injured units no longer get a bonus for the unit being injured (fewer targets -> harder to hit)
#define MOD_BALANCE_RANGED_DEFENSE_UNIT_HEALTH						gCustomMods.isBALANCE_RANGED_DEFENSE_UNIT_HEALTH()

// Land COMBAT units blockade undefended adjacent tiles
#define MOD_BALANCE_LAND_UNITS_ADJACENT_BLOCKADE					gCustomMods.isBALANCE_LAND_UNITS_ADJACENT_BLOCKADE()

// Only units with UNITCOMBAT_RECON or UnitPromotions.GainsXPFromScouting (as well as city border growth) can claim Ancient Ruins
#define MOD_BALANCE_RECON_ONLY_ANCIENT_RUINS						gCustomMods.isBALANCE_RECON_ONLY_ANCIENT_RUINS()

// Inquisitors don't block spread but instead reduce its effectiveness by 50%
#define MOD_BALANCE_INQUISITOR_NERF									gCustomMods.isBALANCE_INQUISITOR_NERF()

// Lack of Strategic Resources causes units to be unable to heal, rather than decreasing their combat strength
#define MOD_BALANCE_RESOURCE_SHORTAGE_UNIT_HEALING					gCustomMods.isBALANCE_RESOURCE_SHORTAGE_UNIT_HEALING()

// Lack of Strategic Resources causes buildings to refund the excess Strategic Resources consumed, at the cost of a 2% increase to empire-wide Building Maintenance per resource over limit
#define MOD_BALANCE_RESOURCE_SHORTAGE_BUILDING_REFUND				gCustomMods.isBALANCE_RESOURCE_SHORTAGE_BUILDING_REFUND()

// Promotions from buildings are now retroactively applied to all units originating from that city
#define MOD_BALANCE_RETROACTIVE_PROMOTIONS							gCustomMods.isBALANCE_RETROACTIVE_PROMOTIONS()

// Puppet cities don't use up Global Happiness (in VP), don't generate Unhappiness from most sources (except Specialists), and don't generate any Local Happiness (except Venice)
// They generate flat Unhappiness per X citizens, no longer generate GPP points, don't count for the Technology cost penalty, and generate full Unit Supply for Venice
#define MOD_BALANCE_PUPPET_CHANGES									gCustomMods.isBALANCE_PUPPET_CHANGES()

// Cities that are being razed will spawn partisans (free units) for the opposing player if still at war, or Barbarians if not still at war
#define MOD_BALANCE_RAZING_CREATES_PARTISANS						gCustomMods.isBALANCE_RAZING_CREATES_PARTISANS()

// Halve starting XP for combat units purchased with Gold
#define MOD_BALANCE_HALF_XP_GOLD_PURCHASES							gCustomMods.isBALANCE_HALF_XP_GOLD_PURCHASES()

// Purchased COMBAT units do not have full health when the city is damaged
#define MOD_BALANCE_PURCHASED_UNIT_DAMAGE							gCustomMods.isBALANCE_PURCHASED_UNIT_DAMAGE()

// Settler units (Units.Food=1) reduce population by 1 when completed
#define MOD_BALANCE_SETTLERS_CONSUME_POPULATION						gCustomMods.isBALANCE_SETTLERS_CONSUME_POPULATION()

// Barbarian Encampments can spawn on visible tiles
#define MOD_BALANCE_ENCAMPMENTS_SPAWN_ON_VISIBLE_TILES				gCustomMods.isBALANCE_ENCAMPMENTS_SPAWN_ON_VISIBLE_TILES()

// Barbarians can raid adjacent cities and steal yields
#define MOD_BALANCE_BARBARIAN_THEFT									gCustomMods.isBALANCE_BARBARIAN_THEFT()

// Increases the Production cost of World Wonders based on the number of World Wonders the player already owns
#define MOD_BALANCE_WORLD_WONDER_COST_INCREASE						gCustomMods.isBALANCE_WORLD_WONDER_COST_INCREASE()

// Proximity malus to the yields of international Trade Routes, scaling down from the furthest available route from the origin city
// Traits.NoTradeRouteProximityPenalty removes this malus if the player with the trait owns the origin or destination city
#define MOD_BALANCE_TRADE_ROUTE_PROXIMITY_PENALTY					gCustomMods.isBALANCE_TRADE_ROUTE_PROXIMITY_PENALTY()

// Players cannot send more than one Trade Route to the same destination city
// Traits.NoAnnexing removes this restriction if the player with the trait owns the origin or destination city
#define MOD_BALANCE_TRADE_ROUTE_DESTINATION_RESTRICTION				gCustomMods.isBALANCE_TRADE_ROUTE_DESTINATION_RESTRICTION()

// Allows for various consolation prizes when someone else beats a player to a World Wonder, instead of always converting to Gold
#define MOD_BALANCE_WONDERS_VARIABLE_CONSOLATION					gCustomMods.isBALANCE_WONDERS_VARIABLE_CONSOLATION()

// Flips the Open Borders Tourism modifier to apply in the opposite way - you have to give Open Borders to gain the Tourism bonus
#define MOD_BALANCE_FLIPPED_OPEN_BORDERS_TOURISM					gCustomMods.isBALANCE_FLIPPED_OPEN_BORDERS_TOURISM()

// Blocks Great Musicians' Concert Tour ability while at war with the target
#define MOD_BALANCE_NO_WARTIME_CONCERT_TOURS						gCustomMods.isBALANCE_NO_WARTIME_CONCERT_TOURS()

// Workers receive a 50% work rate penalty for 50 turns when they are captured
#define MOD_BALANCE_PRISONERS_OF_WAR								gCustomMods.isBALANCE_PRISONERS_OF_WAR()

// Increases the Gold cost of buildings (1% per 3 Techs researched) and units (1% per 2 Techs researched); also decreases the final Gold cost of units by 20%
#define MOD_BALANCE_PURCHASE_COST_ADJUSTMENTS						gCustomMods.isBALANCE_PURCHASE_COST_ADJUSTMENTS()

// Vote Commitments cannot be cancelled by war; if the resolution becomes invalid, the votes are abstained instead of returned to the civ
#define MOD_BALANCE_PERMANENT_VOTE_COMMITMENTS						gCustomMods.isBALANCE_PERMANENT_VOTE_COMMITMENTS()

// Changes to City-State Quests (Faith Contest now based on followers converted, Kill City-State now mutual and solvable by allying both City-States)
// Also disables City-States' resistance to conversion if an active Spread Religion quest is being fulfilled (correct player and religion)
#define MOD_BALANCE_QUEST_CHANGES									gCustomMods.isBALANCE_QUEST_CHANGES()

// Halves the pantheon follower reduction that occurs when a city receives pressure from an actual religion, making pantheons stick around a little longer
#define MOD_BALANCE_RESILIENT_PANTHEONS								gCustomMods.isBALANCE_RESILIENT_PANTHEONS()


/////////////////////////////////////////
// OTHER BALANCE OPTIONS
// Sorting order: alphabetical
/////////////////////////////////////////

// Alters Assyria's conquest trait so that the player always chooses a free tech upon city conquest
#define MOD_BALANCE_ALTERNATE_ASSYRIA_TRAIT							gCustomMods.isBALANCE_ALTERNATE_ASSYRIA_TRAIT()

// Grants Celts maximum up to 3 unimproved adjacent forests for faith
#define MOD_BALANCE_ALTERNATE_CELTS_TRAIT							gCustomMods.isBALANCE_ALTERNATE_CELTS_TRAIT()

// Pantheons can be picked even if they are already picked by someone else
#define MOD_BALANCE_ANY_PANTHEON									gCustomMods.isBALANCE_ANY_PANTHEON()

// City-State Influence scales with era and Yields scale with the City-State's number of cities
#define MOD_BALANCE_CITY_STATE_SCALE								gCustomMods.isBALANCE_CITY_STATE_SCALE()

// Maintains an even ratio of City-States with each trait or personality type at game start
// If both options are combined, each group of City-States with the same trait (e.g., all Cultured CS) will have an even ratio of personality types
#define MOD_BALANCE_CITY_STATE_TRAITS								gCustomMods.isBALANCE_CITY_STATE_TRAITS()
#define MOD_BALANCE_CITY_STATE_PERSONALITIES						gCustomMods.isBALANCE_CITY_STATE_PERSONALITIES()

// Great Generals & Admirals provide combat bonuses for only the era they born and the following era
#define MOD_BALANCE_ERA_RESTRICTED_GENERALS							gCustomMods.isBALANCE_ERA_RESTRICTED_GENERALS()

// Restricts a Team from passing into the next era before they have found all techs of their current era
#define MOD_BALANCE_ERA_RESTRICTION									gCustomMods.isBALANCE_ERA_RESTRICTION()

// Era scaling for Great Engineer & Great Merchant yields
#define MOD_BALANCE_GREAT_PEOPLE_ERA_SCALING						gCustomMods.isBALANCE_GREAT_PEOPLE_ERA_SCALING()

// City-States' unit supply is equal to their handicap amount, which can be modified by globals based on trait, personality, and number of cities. No other unit supply bonuses/penalties apply.
#define MOD_BALANCE_MINOR_UNIT_SUPPLY_HANDICAP						gCustomMods.isBALANCE_MINOR_UNIT_SUPPLY_HANDICAP()

// Prophets do not auto spawn once you hit the Faith threshold (pre-Industrial Era only), must be set to 'Save for Great Prophet' in ReligiousOverview.lua
#define MOD_BALANCE_NO_AUTO_SPAWN_PROPHET							gCustomMods.isBALANCE_NO_AUTO_SPAWN_PROPHET()

// Disable ranged strikes from cities
#define MOD_BALANCE_NO_CITY_RANGED_ATTACK							gCustomMods.isBALANCE_NO_CITY_RANGED_ATTACK()

// Disables Golden Age Point acquisition during Golden Ages
#define MOD_BALANCE_NO_GAP_DURING_GA								gCustomMods.isBALANCE_NO_GAP_DURING_GA()

// GP and similar improvements remove the yields from resources (but still connect them)
#define MOD_BALANCE_NO_RESOURCE_YIELDS_FROM_GP_IMPROVEMENT			gCustomMods.isBALANCE_NO_RESOURCE_YIELDS_FROM_GP_IMPROVEMENT()

// Disables passive religious pressure unless there's a Trade Route or City Connection
#define MOD_BALANCE_PASSIVE_SPREAD_BY_CONNECTION					gCustomMods.isBALANCE_PASSIVE_SPREAD_BY_CONNECTION()

// Civilizations benefit from their pantheon even after being converted to another religion
#define MOD_BALANCE_PERMANENT_PANTHEONS								gCustomMods.isBALANCE_PERMANENT_PANTHEONS()

// Players can pillage permanent improvements if at war
#define MOD_BALANCE_PILLAGE_PERMANENT_IMPROVEMENTS					gCustomMods.isBALANCE_PILLAGE_PERMANENT_IMPROVEMENTS()

// Great Prophets spawn in a random city, partially based on Faith generation
#define MOD_BALANCE_RANDOMIZED_GREAT_PROPHET_SPAWNS					gCustomMods.isBALANCE_RANDOMIZED_GREAT_PROPHET_SPAWNS()

// Relaxes territory check: if active, units can always pass through foreign territory if they don't end the turn there
#define MOD_BALANCE_RELAXED_BORDER_CHECK							gCustomMods.isBALANCE_RELAXED_BORDER_CHECK()

// Interprets double movement as "no extra cost" and half movement as "one extra cost"
// Also disables the application of fake roads in woodland (Iroquois trait) to unit movement
#define MOD_BALANCE_SANE_UNIT_MOVEMENT_COST							gCustomMods.isBALANCE_SANE_UNIT_MOVEMENT_COST()

// Settlers reset the city's Growth after they are created
#define MOD_BALANCE_SETTLERS_RESET_GROWTH							gCustomMods.isBALANCE_SETTLERS_RESET_GROWTH()

// VP: Unhappiness is NOT capped by city Population
#define MOD_BALANCE_UNCAPPED_UNHAPPINESS							gCustomMods.isBALANCE_UNCAPPED_UNHAPPINESS()

// Units are invested at 50% (see define) of their production cost, instead of being built outright
#define MOD_BALANCE_UNIT_INVESTMENTS								gCustomMods.isBALANCE_UNIT_INVESTMENTS()

// XP is granted only on a unit's first attack
#define MOD_BALANCE_XP_ON_FIRST_ATTACK								gCustomMods.isBALANCE_XP_ON_FIRST_ATTACK()

// Changes melee ship units to be cargo carrying units with added promotions for ship and cargo
// FIXME: Disabled for now; this needs to be examined to see if it still works properly
#define MOD_CARGO_SHIPS												(false)

// Halve starting XP for combat units purchased with Faith
#define MOD_BALANCE_HALF_XP_FAITH_PURCHASES							gCustomMods.isBALANCE_HALF_XP_FAITH_PURCHASES()


/////////////////////////////////////////
// OTHER USER INTERFACE OPTIONS
// Sorting order: alphabetical
/////////////////////////////////////////

// Make the UI show moves remaining in the form (exact movement points / 60) for the tile currently being moused over
#define MOD_UI_DISPLAY_PRECISE_MOVEMENT_POINTS						gCustomMods.isUI_DISPLAY_PRECISE_MOVEMENT_POINTS()

// Enables Hidamari's override forcing civs to use civ-specific text only
#define MOD_UI_NO_RANDOM_CIV_DIALOGUE								gCustomMods.isUI_NO_RANDOM_CIV_DIALOGUE()

// Turn off animations for Diplomats, Missionaries, Great Persons, etc. when playing with the Quick Movement game option enabled
#define MOD_UI_QUICK_ANIMATIONS										gCustomMods.isUI_QUICK_ANIMATIONS()


/////////////////////////////////////////
// OTHER AI OPTIONS
// Sorting order: AI control options, then alphabetical
/////////////////////////////////////////

// If enabled, the AI will take control of specified aspects of human players' civilizations
#define MOD_AI_CONTROL_CITY_MANAGEMENT								gCustomMods.isAI_CONTROL_CITY_MANAGEMENT()
#define MOD_AI_CONTROL_CITY_PRODUCTION								gCustomMods.isAI_CONTROL_CITY_PRODUCTION()
#define MOD_AI_CONTROL_DIPLOMACY									gCustomMods.isAI_CONTROL_DIPLOMACY()
#define MOD_AI_CONTROL_ECONOMY										gCustomMods.isAI_CONTROL_ECONOMY()
#define MOD_AI_CONTROL_ESPIONAGE									gCustomMods.isAI_CONTROL_ESPIONAGE()
#define MOD_AI_CONTROL_EVENT_CHOICE									gCustomMods.isAI_CONTROL_EVENT_CHOICE()
#define MOD_AI_CONTROL_FAITH_SPENDING								gCustomMods.isAI_CONTROL_FAITH_SPENDING()
#define MOD_AI_CONTROL_GREAT_PERSON_CHOICE							gCustomMods.isAI_CONTROL_GREAT_PERSON_CHOICE()
#define MOD_AI_CONTROL_POLICY_CHOICE								gCustomMods.isAI_CONTROL_POLICY_CHOICE()
#define MOD_AI_CONTROL_RELIGION_CHOICE								gCustomMods.isAI_CONTROL_RELIGION_CHOICE()
#define MOD_AI_CONTROL_TECH_CHOICE									gCustomMods.isAI_CONTROL_TECH_CHOICE()
#define MOD_AI_CONTROL_TOURISM										gCustomMods.isAI_CONTROL_TOURISM()
#define MOD_AI_CONTROL_UNIT_PROMOTIONS								gCustomMods.isAI_CONTROL_UNIT_PROMOTIONS()
#define MOD_AI_CONTROL_UNITS										gCustomMods.isAI_CONTROL_UNITS()
#define MOD_AI_CONTROL_WORLD_CONGRESS								gCustomMods.isAI_CONTROL_WORLD_CONGRESS()

// Gives human players the same difficulty bonuses as the AI
#define MOD_HUMAN_USES_AI_HANDICAP									gCustomMods.isHUMAN_USES_AI_HANDICAP()

// Causes human players to be treated as AIs for the purposes of One City Challenge and some instances of humans getting special treatment by the AI (e.g., DIPLOAI_PASSIVE_MODE, or AI not spreading religion to human teammates)
#define MOD_HUMAN_USES_AI_MECHANICS									gCustomMods.isHUMAN_USES_AI_MECHANICS()

// Danger plot valuation: ignore ZOC only for those units which are likely to be killed by the enemy (alternatively ignore ZOC for all owned units)
#define MOD_COMBATAI_TWO_PASS_DANGER								gCustomMods.isCOMBATAI_TWO_PASS_DANGER()

// Prevents the AI from trading Cities, including in peace deals
#define MOD_DEALAI_DISABLE_CITY_TRADES								gCustomMods.isDEALAI_DISABLE_CITY_TRADES()

// Prevents the AI from trading Third Party War
#define MOD_DEALAI_DISABLE_WAR_BRIBES								gCustomMods.isDEALAI_DISABLE_WAR_BRIBES()

// Removes all restrictions on trading temporary items for permanent items; this is abusable by humans if enabled
#define MOD_DEALAI_GLOBAL_PERMANENT_FOR_TEMPORARY					gCustomMods.isDEALAI_GLOBAL_PERMANENT_FOR_TEMPORARY()

// Greatly increases the AI's aggression
#define MOD_DIPLOAI_AGGRESSIVE_MODE									gCustomMods.isDIPLOAI_AGGRESSIVE_MODE()

// Disables Aggressive Mode being automatically enabled (setting 2) if only Domination and/or Time Victories are enabled
#define MOD_DIPLOAI_DISABLE_DOMINATION_ONLY_AGGRESSION				gCustomMods.isDIPLOAI_DISABLE_DOMINATION_ONLY_AGGRESSION()

// No AI opinion modifiers or values will appear in the table, except a short message explaining their visible approach
#define MOD_DIPLOAI_HIDE_OPINION_TABLE								(!MOD_DIPLO_DEBUG_MODE && gCustomMods.isDIPLOAI_HIDE_OPINION_TABLE())

// If enabled, limits how much randomness is allowed when selecting default Primary and Secondary Victory Pursuits
#define MOD_DIPLOAI_LIMIT_VICTORY_PURSUIT_RANDOMIZATION				gCustomMods.isDIPLOAI_LIMIT_VICTORY_PURSUIT_RANDOMIZATION()

// Diplomacy flavors for AI players will always be set to their database value, clamped between 1 and 10
#define MOD_DIPLOAI_NO_FLAVOR_RANDOMIZATION							gCustomMods.isDIPLOAI_NO_FLAVOR_RANDOMIZATION()

// Prevents the AI from declaring war
#define MOD_DIPLOAI_PASSIVE_MODE									gCustomMods.isDIPLOAI_PASSIVE_MODE()

// Shows the number value of opinion modifiers, but doesn't show hidden modifiers
// Also shows LeagueAI scoring for World Congress proposals
#define MOD_DIPLOAI_SHOW_ALL_OPINION_VALUES							(MOD_DIPLO_DEBUG_MODE || (!MOD_DIPLOAI_HIDE_OPINION_TABLE && gCustomMods.isDIPLOAI_SHOW_ALL_OPINION_VALUES()))

// Include HandicapInfos.HumanOpinionChange in the opinion modifier table
#define MOD_DIPLOAI_SHOW_BASE_HUMAN_OPINION							(MOD_DIPLO_DEBUG_MODE || (!MOD_DIPLOAI_HIDE_OPINION_TABLE && gCustomMods.isDIPLOAI_SHOW_BASE_HUMAN_OPINION()))

// Shows all hidden opinion modifiers (except HandicapInfos.HumanOpinionChange), but not the number value
#define MOD_DIPLOAI_SHOW_HIDDEN_OPINION_MODIFIERS					(MOD_DIPLO_DEBUG_MODE || (!MOD_DIPLOAI_HIDE_OPINION_TABLE && gCustomMods.isDIPLOAI_SHOW_HIDDEN_OPINION_MODIFIERS()))

// AI civilizations will not send some messages to human players
// DIPLOAI_SHUT_UP is a global toggle; the options below it selectively disable certain types of messages
#define MOD_DIPLOAI_SHUT_UP											gCustomMods.isDIPLOAI_SHUT_UP()
#define MOD_DIPLOAI_SHUT_UP_COMPLIMENTS								(MOD_DIPLOAI_SHUT_UP || gCustomMods.isDIPLOAI_SHUT_UP_COMPLIMENTS())
#define MOD_DIPLOAI_SHUT_UP_COOP_WAR_OFFERS							(MOD_DIPLOAI_SHUT_UP || gCustomMods.isDIPLOAI_SHUT_UP_COOP_WAR_OFFERS())
#define MOD_DIPLOAI_SHUT_UP_DEMANDS									(MOD_DIPLOAI_SHUT_UP || gCustomMods.isDIPLOAI_SHUT_UP_DEMANDS())
#define MOD_DIPLOAI_SHUT_UP_FRIENDSHIP_OFFERS						(MOD_DIPLOAI_SHUT_UP || gCustomMods.isDIPLOAI_SHUT_UP_FRIENDSHIP_OFFERS())
#define MOD_DIPLOAI_SHUT_UP_GIFT_OFFERS								(MOD_DIPLOAI_SHUT_UP || gCustomMods.isDIPLOAI_SHUT_UP_GIFT_OFFERS())
#define MOD_DIPLOAI_SHUT_UP_HELP_REQUESTS							(MOD_DIPLOAI_SHUT_UP || gCustomMods.isDIPLOAI_SHUT_UP_HELP_REQUESTS())
#define MOD_DIPLOAI_SHUT_UP_INDEPENDENCE_REQUESTS					(MOD_DIPLOAI_SHUT_UP || gCustomMods.isDIPLOAI_SHUT_UP_INDEPENDENCE_REQUESTS())
#define MOD_DIPLOAI_SHUT_UP_INSULTS									(MOD_DIPLOAI_SHUT_UP || gCustomMods.isDIPLOAI_SHUT_UP_INSULTS())
#define MOD_DIPLOAI_SHUT_UP_PEACE_OFFERS							(MOD_DIPLOAI_SHUT_UP || gCustomMods.isDIPLOAI_SHUT_UP_PEACE_OFFERS())
#define MOD_DIPLOAI_SHUT_UP_TRADE_OFFERS							(MOD_DIPLOAI_SHUT_UP || gCustomMods.isDIPLOAI_SHUT_UP_TRADE_OFFERS())
#define MOD_DIPLOAI_SHUT_UP_TRADE_RENEWALS							(MOD_DIPLOAI_SHUT_UP || gCustomMods.isDIPLOAI_SHUT_UP_TRADE_RENEWALS())

// The AI will never vote for others (EXCEPT their team leader) as host of the World Congress
#define MOD_LEAGUEAI_NO_OTHER_HOST_VOTES							gCustomMods.isLEAGUEAI_NO_OTHER_HOST_VOTES()

// The AI will never vote for others (EXCEPT their team leader) for World Leader / Global Hegemony
#define MOD_LEAGUEAI_NO_OTHER_WORLD_LEADER_VOTES					gCustomMods.isLEAGUEAI_NO_OTHER_WORLD_LEADER_VOTES()


/////////////////////////////////////////
// API EXTENSIONS
// Sorting order: alphabetical
/////////////////////////////////////////

// If true, additional promotions are enabled which may cause a significant slowdown if there are many units in the game
#define MOD_API_AREA_EFFECT_PROMOTIONS								gCustomMods.isAPI_AREA_EFFECT_PROMOTIONS()

// If true, no Lua hooks are called from the gamecore, increasing performance and stability
#define MOD_API_DISABLE_LUA_HOOKS									gCustomMods.isAPI_DISABLE_LUA_HOOKS()

// Enable this if you need hovercraft support, but it costs performance
#define MOD_API_HOVERING_UNITS										gCustomMods.isAPI_HOVERING_UNITS()

// When checking for a building's prerequisites in a city for Building_ClassesNeededInCity, loop through all buildings in the city to find a building belonging in the class, rather than the shortcut method of only checking if there is an entry in Civilization_BuildingClassOverrides.
// A more thorough check for mods which replace buildings using Lua. Otherwise, the prerequisite for Building_ClassesNeededInCity will not be fulfilled correctly.
#define MOD_BUILDINGS_THOROUGH_PREREQUISITES						gCustomMods.isBUILDINGS_THOROUGH_PREREQUISITES()

// Various new tables and logics for improvements. Currently, this is required to activate:
// - Column 'CreatesFeature' in 'Improvements'.
// - Column 'RandResourceChance' in 'Improvements'.
// - Column 'RemoveWhenComplete' in 'Improvements'.
// - Column 'QuantityRequirement' in 'Improvement_ResourceTypes'.
#define MOD_IMPROVEMENTS_EXTENSIONS									gCustomMods.isIMPROVEMENTS_EXTENSIONS()

// When enabled, the full map state will be dumped to disk every turn. May cause performance degradation.
#define MOD_LOG_MAP_STATE											gCustomMods.isLOG_MAP_STATE()

// Various new tables and logics for plots. Currently, this is required to activate:
// - Table 'Plot_AdjacentFeatureYieldChanges'. Columns are 'PlotType', 'FeatureType', 'YieldType', 'Yield', 'IgnoreNaturalWonderPlots'.
#define MOD_PLOTS_EXTENSIONS										gCustomMods.isPLOTS_EXTENSIONS()

// Permits naval units to transfer their moves to Great Admirals (like land units can to Great Generals)
#define MOD_PROMOTIONS_FLAGSHIP										gCustomMods.isPROMOTIONS_FLAGSHIP()

// Permits units to receive a combat bonus from being near an improvement
#define MOD_PROMOTIONS_IMPROVEMENT_BONUS							gCustomMods.isPROMOTIONS_IMPROVEMENT_BONUS()

// Various new tables and logics for beliefs. Currently, this is required to activate:
// - Table 'Belief_FreePromotions'. Columns are 'BeliefType', 'PromotionType'.
// - Table 'Belief_YieldFromImprovementBuild'. Columns are 'BeliefType', 'YieldType', 'Yield', 'IsEraScaling'.
// - Table 'Belief_YieldFromPillage'. Columns are 'BeliefType', 'YieldType', 'Yield', 'IsEraScaling'.
#define MOD_RELIGION_EXTENSIONS										gCustomMods.isRELIGION_EXTENSIONS()

// Adds support for "local" religions (i.e. ones that only have influence within the civ's own territory)
#define MOD_RELIGION_LOCAL_RELIGIONS								gCustomMods.isRELIGION_LOCAL_RELIGIONS()

// Enables the Resource_UnitCombatProductionCostModifiersLocal and Resource_BuildingProductionCostModifiersLocal tables
#define MOD_RESOURCES_PRODUCTION_COST_MODIFIERS						gCustomMods.isRESOURCES_PRODUCTION_COST_MODIFIERS()

// Permits internal Gold trade routes (still needs the appropriate traits, etc.)
#define MOD_TRADE_INTERNAL_GOLD_ROUTES								gCustomMods.isTRADE_INTERNAL_GOLD_ROUTES()

// Permits Wonder resource (i.e. Marble) Trade Routes to be established
#define MOD_TRADE_WONDER_RESOURCE_ROUTES							gCustomMods.isTRADE_WONDER_RESOURCE_ROUTES()

// Enables trait for trade route production siphon, intended for Colonialist Legacies' - Phillipine Republic for VP (Trait_TradeRouteProductionSiphon)
#define MOD_TRAITS_TRADE_ROUTE_PRODUCTION_SIPHON					gCustomMods.isTRAITS_TRADE_ROUTE_PRODUCTION_SIPHON()

// Enables trait for yield from route movement in foreign territory, intended for Colonialist Legacies' - Phillipine Republic for VP (Trait_YieldFromRouteMovementInForeignTerritory)
#define MOD_TRAITS_YIELD_FROM_ROUTE_MOVEMENT_IN_FOREIGN_TERRITORY	gCustomMods.isTRAITS_YIELD_FROM_ROUTE_MOVEMENT_IN_FOREIGN_TERRITORY()

// For debugging only
#define MOD_UNIT_KILL_STATS											gCustomMods.isUNIT_KILL_STATS()

// Enables the table Unit_ResourceQuantityTotals
#define MOD_UNITS_RESOURCE_QUANTITY_TOTALS							gCustomMods.isUNITS_RESOURCE_QUANTITY_TOTALS()

// Enables WHoward's Military Log
#define MOD_WH_MILITARY_LOG											gCustomMods.isWH_MILITARY_LOG()

// Enables units providing a yield bonus to cities
#define MOD_YIELD_MODIFIER_FROM_UNITS								gCustomMods.isYIELD_MODIFIER_FROM_UNITS()


/////////////////////////////////////////
// EVENTS
// Sorting order: alphabetical
/////////////////////////////////////////

// Events sent when choosing beliefs
//   GameEvents.PlayerCanHaveBelief.Add(function(iPlayer, iBelief) return true end)
//   GameEvents.ReligionCanHaveBelief.Add(function(iPlayer, iReligion, iBelief) return true end)
#define MOD_EVENTS_ACQUIRE_BELIEFS									gCustomMods.isEVENTS_ACQUIRE_BELIEFS()

// Event sent to allow Lua to override the AI's choice of tech
//   GameEvents.AiOverrideChooseNextTech.Add(function(iPlayer, bFreeTech) return iChoosenTech end)
#define MOD_EVENTS_AI_OVERRIDE_TECH									gCustomMods.isEVENTS_AI_OVERRIDE_TECH()

// Events sent to ascertain if a unit can airlift from/to a specific plot
//   GameEvents.CanAirliftFrom.Add(function(iPlayer, iUnit, iPlotX, iPlotY) return false end)
//   GameEvents.CanAirliftTo.Add(function(iPlayer, iUnit, iPlotX, iPlotY) return false end)
#define MOD_EVENTS_AIRLIFT											gCustomMods.isEVENTS_AIRLIFT()

// Events sent to ascertain if an area can have civ specific resources and to place those resources
//   GameEvents.AreaCanHaveAnyResource.Add(function(iPlayer, iArea) return true end)
//   GameEvents.PlaceResource.Add(function(iPlayer, iResource, iCount, iPlotX, iPlotY) end)
#define MOD_EVENTS_AREA_RESOURCES									gCustomMods.isEVENTS_AREA_RESOURCES()

// Events sent by Barbarians
//   GameEvents.BarbariansCanFoundCamp.Add(function(iPlotX, iPlotY) return true end)
//   GameEvents.BarbariansCampFounded.Add(function(iPlotX, iPlotY) end)
//   GameEvents.BarbariansCampCleared.Add(function(iPlotX, iPlotY, iPlayer) end)
//   GameEvents.BarbariansCampCanSpawnUnit.Add(function(iPlotX, iPlotY) return true end)
//   GameEvents.BarbariansCampGetSpawnUnit.Add(function(iPlotX, iPlotY, iPrefUnitType) return iPrefUnitType end)
//   GameEvents.BarbariansSpawnedUnit.Add(function(iPlotX, iPlotY, iUnitType) end)
#define MOD_EVENTS_BARBARIANS										gCustomMods.isEVENTS_BARBARIANS()

// Events sent during combat
//   GameEvents.BattleStarted.Add(function(iType, iPlotX, iPlotY) end)
//   GameEvents.BattleJoined.Add(function(iPlayer, iUnitOrCity, iRole, bIsCity) end)
//   GameEvents.BattleFinished.Add(function() end)
#define MOD_EVENTS_BATTLES											gCustomMods.isEVENTS_BATTLES()
//   GameEvents.BattleDamageDelta.Add(function(iRole, iBaseDamage) return 0 end)
#define MOD_EVENTS_BATTLES_DAMAGE									(MOD_EVENTS_BATTLES && gCustomMods.isEVENTS_BATTLES_DAMAGE())

// Event sent to ascertain if a unit can move into a given plot; VERY, VERY CPU INTENSIVE!!!
//   GameEvents.CanMoveInto.Add(function(iPlayer, iUnit, iPlotX, iPlotY, bAttack, bDeclareWar) return true end)
#define MOD_EVENTS_CAN_MOVE_INTO									gCustomMods.isEVENTS_CAN_MOVE_INTO()

// Event sent when a team circumnavigates the globe
//   GameEvents.CircumnavigatedGlobe.Add(function(iTeam) end)
#define MOD_EVENTS_CIRCUMNAVIGATION									gCustomMods.isEVENTS_CIRCUMNAVIGATION()

// Events sent after a city produces/buys something
//   GameEvents.CityTrained.Add(function(iPlayer, iCity, iUnit, bGold, bFaith) end)
//   GameEvents.CityConstructed.Add(function(iPlayer, iCity, iBuilding, bGold, bFaith) end)
//   GameEvents.CityCreated.Add(function(iPlayer, iCity, iProject, bGold, bFaith) end)
//   GameEvents.CityPrepared.Add(function(iPlayer, iCity, iSpecialist, iAmount, iNeeded) end)
//   GameEvents.CityBoughtPlot.Add(function(iPlayer, iCity, iPlotX, iPlotY, bGold, bCulture) end)
//   GameEvents.CitySoldBuilding.Add(function(iPlayer, iCity, iBuilding) end)
#define MOD_EVENTS_CITY												gCustomMods.isEVENTS_CITY()
	
// Event sent to ascertain if a city can perform airlifts
//   GameEvents.CityCanAirlift.Add(function(iPlayer, iCity) return false end)
#define MOD_EVENTS_CITY_AIRLIFT										gCustomMods.isEVENTS_CITY_AIRLIFT()

// Events sent to ascertain the bombard range for a city, and if indirect fire is allowed
//   GameEvents.GetBombardRange.Add(function(iPlayer, iCity) return (-1 * GameDefines.CITY_ATTACK_RANGE) end)
#define MOD_EVENTS_CITY_BOMBARD										gCustomMods.isEVENTS_CITY_BOMBARD()

// Event sent to ascertain if a city can acquire a plot
//   GameEvents.CityCanAcquirePlot.Add(function(iPlayer, iCity, iPlotX, iPlotY) return true end)
#define MOD_EVENTS_CITY_BORDERS										gCustomMods.isEVENTS_CITY_BORDERS()

// Event sent after a civilization moves their capital
//   GameEvents.CapitalChanged.Add(function(iPlayer, iNewCapital, iOldCapital) end)
#define MOD_EVENTS_CITY_CAPITAL										gCustomMods.isEVENTS_CITY_CAPITAL()

// Events sent to ascertain if one city is connected to another
//   GameEvents.CityConnections.Add(function(iPlayer, bDomestic) return false end)
//   GameEvents.CityConnected.Add(function(iPlayer, iCityX, iCityY, iToCityX, iToCityY, bDomestic) return false end)
#define MOD_EVENTS_CITY_CONNECTIONS									gCustomMods.isEVENTS_CITY_CONNECTIONS()

// Events sent about city founding
//   GameEvents.PlayerCanFoundCity.Add(function(iPlayer, iPlotX, iPlotY) return true end)
//   GameEvents.PlayerCanFoundCityRegardless.Add(function(iPlayer, iPlotX, iPlotY) return false end)
#define MOD_EVENTS_CITY_FOUNDING									gCustomMods.isEVENTS_CITY_FOUNDING()

// Event sent to ascertain if a player can override the standard razing rules for the specified city and raze it anyway
//   GameEvents.PlayerCanRaze.Add(function(iPlayer, iCity) return false end)
#define MOD_EVENTS_CITY_RAZING										gCustomMods.isEVENTS_CITY_RAZING()

// Event sent to see if a command is valid
//   GameEvents.CanDoCommand.Add(function(iPlayer, iUnit, iCommand, iData1, iData2, iPlotX, iPlotY, bTestVisible) return true end)
#define MOD_EVENTS_COMMAND											gCustomMods.isEVENTS_COMMAND()

// Events sent for custom missions
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
#define MOD_EVENTS_CUSTOM_MISSIONS									gCustomMods.isEVENTS_CUSTOM_MISSIONS()

// Event sent during Game.DoFromUIDiploEvent, see also DiscussionDialog.lua
//   GameEvents.UiDiploEvent.Add(function(iEvent, iAIPlayer, iArg1, iArg2, int iArg3, int iArg4) end)
#define MOD_EVENTS_DIPLO_EVENTS										gCustomMods.isEVENTS_DIPLO_EVENTS()

// Enhanced Diplomacy Modifiers events
//   GameEvents.GetDiploModifier.Add(function(iEvent, iFromPlayer, iToPlayer) return 0 end)
//   GameEvents.AiFlatApproachChange.Add(function(iPlayer, iApproachType) return iChange end)
//   GameEvents.AiPercentApproachMod.Add(function(iPlayer, iApproachType) return iMod end)
#define MOD_EVENTS_DIPLO_MODIFIERS									gCustomMods.isEVENTS_DIPLO_MODIFIERS()

// Events sent on espionage outcomes
//   GameEvents.EspionageResult.Add(function(iPlayer, iSpy, iResult, iCityX, iCityY) end)
//   GameEvents.EspionageState.Add(function(iPlayer, iSpy, iState, iCityX, iCityY) end)
//   GameEvents.EspionageCanMoveSpyTo.Add(function(iPlayer, iCityOwner, iCity) return true)
//   GameEvents.EspionageCanStageCoup.Add(function(iPlayer, iCityOwner, iCity) return true)
#define MOD_EVENTS_ESPIONAGE										gCustomMods.isEVENTS_ESPIONAGE()

// Events sent when a player is about to found a religion
//   GameEvents.PlayerCanFoundPantheon.Add(function(iPlayer) return true end)
//   GameEvents.PlayerCanFoundReligion.Add(function(iPlayer, iCity) return true end)
//   GameEvents.GetReligionToFound.Add(function(iPlayer, iPreferredReligion, bIsAlreadyFounded) return iPreferredReligion end)
//   GameEvents.PantheonFounded.Add(function(iPlayer, iCapitalCity, iReligion, iBelief1) end)
//   GameEvents.ReligionFounded.Add(function(iPlayer, iHolyCity, iReligion, iBelief1, iBelief2, iBelief3, iBelief4, iBelief5) end)
//   GameEvents.ReligionEnhanced.Add(function(iPlayer, iReligion, iBelief1, iBelief2) end)
//   GameEvents.ReligionReformed.Add(function(iPlayer, iReligion, iBelief1) end)
#define MOD_EVENTS_FOUND_RELIGION									gCustomMods.isEVENTS_FOUND_RELIGION()

// Event sent when Game is saved (from Pazyryk)
//   GameEvents.GameSave.Add(function() end)
#define EA_EVENT_GAME_SAVE											gCustomMods.isEVENTS_GAME_SAVE()

// Event sent by golden ages
//   GameEvents.PlayerGoldenAge.Add(function(iPlayer, bStart, iTurns) end)
//   GameEvents.PlayerEndOfMayaLongCount.Add(function(iPlayer, iBaktun, iBaktunPreviousTurn) end)
#define MOD_EVENTS_GOLDEN_AGE										gCustomMods.isEVENTS_GOLDEN_AGE()

// Event sent when a Goody Hut is entered
//   GameEvents.GoodyHutCanNotReceive.Add(function(iPlayer, iUnit, eGoody, bPick) return false end)
//   GameEvents.GoodyHutReceivedBonus.Add(function(iPlayer, iUnit, eGoody, iX, iY) end)
#define MOD_EVENTS_GOODY_CHOICE										gCustomMods.isEVENTS_GOODY_CHOICE()

// Events sent if a Goody Hut is giving a tech
//   GameEvents.GoodyHutCanResearch.Add(function(iPlayer, eTech) return true end)
//   GameEvents.GoodyHutTechResearched.Add(function(iPlayer, eTech) end)
#define MOD_EVENTS_GOODY_TECH										gCustomMods.isEVENTS_GOODY_TECH()

// Events sent by Great People actions
//   GameEvents.GreatPersonExpended.Add(function(iPlayer, iUnit, iUnitType, iX, iY) end)
#define MOD_EVENTS_GREAT_PEOPLE										gCustomMods.isEVENTS_GREAT_PEOPLE()

// Events sent about ideologies and tenets
//   GameEvents.PlayerCanAdoptIdeology.Add(function(iPlayer, iIdeology) return true end)
//   GameEvents.PlayerCanAdoptTenet.Add(function(iPlayer, iTenet) return true end)
#define MOD_EVENTS_IDEOLOGIES										gCustomMods.isEVENTS_IDEOLOGIES()

// Event sent to ascertain if a player can liberate another player
//   GameEvents.PlayerCanLiberate.Add(function(iPlayer, iOtherPlayer) return true end)
//   GameEvents.PlayerLiberated.Add(function(iPlayer, iOtherPlayer, iCity) end)
#define MOD_EVENTS_LIBERATION										gCustomMods.isEVENTS_LIBERATION()

// Events sent on status change with City-States
//   GameEvents.MinorFriendsChanged.Add(function(iMinor, iMajor, bIsFriend, iOldFriendship, iNewFriendship) end)
//   GameEvents.MinorAlliesChanged.Add(function(iMinor, iMajor, bIsAlly, iOldFriendship, iNewFriendship) end)
#define MOD_EVENTS_MINORS											gCustomMods.isEVENTS_MINORS()

// Event sent upon a City-State giving a gift
//   GameEvents.MinorGift.Add(function(iMinor, iMajor, iGift, iFriendshipBoost, 0, bFirstMajorCiv, false, szTxtKeySuffix) end)
//   GameEvents.MinorGiftUnit.Add(function(iMinor, iMajor, iUnitType) end)
#define MOD_EVENTS_MINORS_GIFTS										gCustomMods.isEVENTS_MINORS_GIFTS()

// Events sent on interaction with City-States
//   GameEvents.PlayerCanProtect.Add(function(iPlayer, iCS) return true end)
//   GameEvents.PlayerProtected.Add(function(iPlayer, iCS) end)
//   GameEvents.PlayerCanRevoke.Add(function(iPlayer, iCS) return true end)
//   GameEvents.PlayerRevoked.Add(function(iPlayer, iCS, bPledgeNowBroken) end)
//   GameEvents.PlayerCanBuyOut.Add(function(iPlayer, iCS) return true end)
//   GameEvents.PlayerBoughtOut.Add(function(iPlayer, iCS) end)
//   GameEvents.PlayerCanBullyGold.Add(function(iPlayer, iCS) return true end)
//   GameEvents.PlayerCanBullyUnit.Add(function(iPlayer, iCS) return true end)
//   GameEvents.PlayerBullied.Add(function(iPlayer, iCS, iValue, eUnitType, iPlotX, iPlotY, eYield) end)
//   GameEvents.PlayerCanGiftGold.Add(function(iPlayer, iCS) return true end)
//   GameEvents.PlayerCanGiftUnit.Add(function(iPlayer, iCS, iUnit) return true end)
//   GameEvents.PlayerCanGiftImprovement.Add(function(iPlayer, iCS) return true end)
//   GameEvents.PlayerGifted.Add(function(iPlayer, iCS, iGold, iUnitType, iPlotX, iPlotY) end)
//   GameEvents.PlayerCanTransitMinorCity.Add(function(iPlayer, iCS, iCity, iPlotX, iPlotY) return true end)
//   GameEvents.UnitCanTransitMinorCity.Add(function(iPlayer, iUnit, iCS, iCity, iPlotX, iPlotY) return true end)
#define MOD_EVENTS_MINORS_INTERACTION								gCustomMods.isEVENTS_MINORS_INTERACTION()

// Event sent when the player enters a new era, see also NewEraPopup.lua and BUTTONPOPUP_NEW_ERA
//   GameEvents.TeamSetEra.Add(function(eTeam, eEra, bFirst) end)
#define MOD_EVENTS_NEW_ERA											gCustomMods.isEVENTS_NEW_ERA()

// Event sent when a nuke is fired
//   GameEvents.NuclearDetonation.Add(function(iPlayer, iX, iY, bWar, bBystanders) end)
#define MOD_EVENTS_NUCLEAR_DETONATION								gCustomMods.isEVENTS_NUCLEAR_DETONATION()

// Event sent when the team discovers a new Natural Wonder
//   GameEvents.NaturalWonderDiscovered.Add(function(iTeam, iFeature, iX, iY, bFirst) end)
#define MOD_EVENTS_NW_DISCOVERY										gCustomMods.isEVENTS_NW_DISCOVERY()

// Events sent before and after a paradrop
//   GameEvents.CanParadropFrom.Add(function(iPlayer, iUnit, iPlotX, iPlotY) return false end)
//   GameEvents.CannotParadropFrom.Add(function(iPlayer, iUnit, iPlotX, iPlotY) return false end)
//   GameEvents.ParadropAt.Add(function(iPlayer, iUnit, iFromX, iFromY, iToX, iToY) end)
#define MOD_EVENTS_PARADROPS										gCustomMods.isEVENTS_PARADROPS()

// Event sent by ending turn
//   GameEvents.PlayerDoneTurn.Add(function(iPlayer) end)
#define MOD_EVENTS_PLAYER_TURN										gCustomMods.isEVENTS_PLAYER_TURN()

// Events sent by plots
//   GameEvents.PlayerCanBuild.Add(function(iPlayer, iUnit, iX, iY, iBuild) return true end)
//   GameEvents.PlotCanImprove.Add(function(iX, iY, iImprovement) return true end)
//   GameEvents.PlayerBuilding.Add(function(iPlayer, iUnit, iX, iY, iBuild, bStarting) end)
//   GameEvents.PlayerBuilt.Add(function(iPlayer, iUnit, iX, iY, iBuild) end)
#define MOD_EVENTS_PLOT												gCustomMods.isEVENTS_PLOT()

// Events sent to ascertain if a unit can rebase to a specific plot (either a city or a carrier)
//   GameEvents.CanLoadAt.Add(function(iPlayer, iUnit, iPlotX, iPlotY) return false end)
//   GameEvents.CanRebaseInCity.Add(function(iPlayer, iUnit, iPlotX, iPlotY) return false end)
//   GameEvents.CanRebaseTo.Add(function(iPlayer, iUnit, iPlotX, iPlotY, bIsCity) return false end)
//   GameEvents.RebaseTo.Add(function(iPlayer, iUnit, iPlotX, iPlotY) end)
#define MOD_EVENTS_REBASE											gCustomMods.isEVENTS_REBASE()

// Events generated by the RED (by Gedemon) dll mod code
//   Turn   --> PlayerEndTurnInitiated, PlayerEndTurnCompleted, TurnComplete
//   Combat --> PushingMissionTo, MustAbortAttack, CombatResult, CombatEnded
#define MOD_EVENTS_RED_TURN											gCustomMods.isEVENTS_RED_TURN()
#define MOD_EVENTS_RED_COMBAT										gCustomMods.isEVENTS_RED_COMBAT()
#define MOD_EVENTS_RED_COMBAT_MISSION								(MOD_EVENTS_RED_COMBAT && gCustomMods.isEVENTS_RED_COMBAT_MISSION())
#define MOD_EVENTS_RED_COMBAT_ABORT									(MOD_EVENTS_RED_COMBAT && gCustomMods.isEVENTS_RED_COMBAT_ABORT())
#define MOD_EVENTS_RED_COMBAT_RESULT								(MOD_EVENTS_RED_COMBAT && gCustomMods.isEVENTS_RED_COMBAT_RESULT())
#define MOD_EVENTS_RED_COMBAT_ENDED									(MOD_EVENTS_RED_COMBAT && gCustomMods.isEVENTS_RED_COMBAT_ENDED())

// Events sent to see if religion missions are valid
//   GameEvents.PlayerCanSpreadReligion.Add(function(iPlayer, iUnit, iPlotX, iPlotY) return true end)
//   GameEvents.PlayerCanRemoveHeresy.Add(function(iPlayer, iUnit, iPlotX, iPlotY) return true end)
#define MOD_EVENTS_RELIGION											gCustomMods.isEVENTS_RELIGION()

// Events sent about resolutions
//   GameEvents.PlayerCanPropose.Add(function(iPlayer, iResolution, iChoice, bEnact) return true end)
//   GameEvents.ResolutionProposing.Add(function(iPlayer, iLeague) return false; end)
//   GameEvents.ResolutionVoting.Add(function(iPlayer, iLeague) return false; end)
//   GameEvents.ResolutionResult.Add(function(iResolution, iProposer, iChoice, bEnact, bPassed) end)
#define MOD_EVENTS_RESOLUTIONS										gCustomMods.isEVENTS_RESOLUTIONS()

// Events sent when terraforming occurs
//   GameEvents.TerraformingMap.Add(function(iEvent, iLoad) end)
//   GameEvents.TerraformingPlot.Add(function(iEvent, iPlotX, iPlotY, iInfo, iNewValue, iOldValue, iNewExtra, iOldExtra) end)
#define MOD_EVENTS_TERRAFORMING										gCustomMods.isEVENTS_TERRAFORMING()

// Events sent when plots change from worker actions
//   GameEvents.TileFeatureChanged.Add(function(iPlotX, iPlotY, iOwner, iOldFeature, iNewFeature) end)
//   GameEvents.TileImprovementChanged.Add(function(iPlotX, iPlotY, iOwner, iOldImprovement, iNewImprovement, bPillaged) end)
//   GameEvents.TileOwnershipChanged.Add(function(iPlotX, iPlotY, iOwner, iOldOwner) end)
//   GameEvents.TileRouteChanged.Add(function(iPlotX, iPlotY, iOwner, iOldRoute, iNewRoute, bPillaged) end)
#define MOD_EVENTS_TILE_IMPROVEMENTS								gCustomMods.isEVENTS_TILE_IMPROVEMENTS()

// Event sent when a plot is revealed
//   GameEvents.TileRevealed.Add(function(iPlotX, iPlotY, iTeam, iFromTeam, bFirst, iUnitOwner, iUnit) end)
#define MOD_EVENTS_TILE_REVEALED									gCustomMods.isEVENTS_TILE_REVEALED()

// Event sent as a trade route is plundered
//   GameEvents.PlayerPlunderedTradeRoute.Add(function(iPlayer, iUnit, iPlunderedGold, iFromPlayer, iFromCity, iToPlayer, iToCity, iRouteType, iRouteDomain) end)
#define MOD_EVENTS_TRADE_ROUTE_PLUNDERED							gCustomMods.isEVENTS_TRADE_ROUTE_PLUNDERED()

// Events sent to ascertain whether a Trade Route can be created, or after one is completed
//   GameEvents.PlayerCanCreateTradeRoute.Add(function(iFromPlayer, iFromCity, iToPlayer, iToCity, iDomain, iConnectionType) return true end)
//	 GameEvents.TradeRouteCompleted.Add(function( iOriginOwner, iOriginCity, iDestOwner, iDestCity, eDomain, eConnectionTradeType) end)
#define MOD_EVENTS_TRADE_ROUTES										gCustomMods.isEVENTS_TRADE_ROUTES()

// Events sent as units perform actions
//   GameEvents.UnitCanPillage.Add(function(iPlayer, iUnit, iImprovement, iRoute) return true end)
//   GameEvents.UnitPillageGold.Add(function(iPlayer, iUnit, iImprovement, iGold) return iGold end)
#define MOD_EVENTS_UNIT_ACTIONS										gCustomMods.isEVENTS_UNIT_ACTIONS()

// Events sent as a unit is captured
//   GameEvents.UnitCaptured.Add(function(iByPlayer, iByUnit, iCapturedPlayer, iCapturedUnit, bWillBeKilled, iReason) end)
//   iReason - 0=Move, 1=Combat, 2=Trait (barb land), 3=Trait (barb naval), 4=Belief (barb)
//   GameEvents.UnitCaptureType.Add(function(iPlayer, iUnit, iUnitType, iByCiv) return iCaptureUnitType; end)
#define MOD_EVENTS_UNIT_CAPTURE										gCustomMods.isEVENTS_UNIT_CAPTURE()

// Event sent as units are converted (wherever CvUnit::convert() is called, e.g., upgrade or barbarian capture)
//   GameEvents.UnitConverted.Add(function(iOldPlayer, iNewPlayer, iOldUnit, iNewUnit, bIsUpgrade) return true end)
#define MOD_EVENTS_UNIT_CONVERTS									gCustomMods.isEVENTS_UNIT_CONVERTS()

// Event sent when a unit is created
//   GameEvents.UnitCreated.Add(function(iPlayer, iUnit, iUnitType, iPlotX, iPlotY) end)
#define MOD_EVENTS_UNIT_CREATED										gCustomMods.isEVENTS_UNIT_CREATED()

// Events sent as units are created
//   GameEvents.UnitCanHaveName.Add(function(iPlayer, iUnit, iName) return true end)
//   GameEvents.UnitCanHaveGreatWork.Add(function(iPlayer, iUnit, iGreatWork) return true end)
#define MOD_EVENTS_UNIT_DATA										gCustomMods.isEVENTS_UNIT_DATA()

// Event sent when a unit founds a city
//   GameEvents.UnitCityFounded.Add(function(iPlayer, iUnit, iUnitType, iPlotX, iPlotY) end)
#define MOD_EVENTS_UNIT_FOUNDED										gCustomMods.isEVENTS_UNIT_FOUNDED()

// Event sent just before a unit is killed (via CvUnit::kill())
//   GameEvents.UnitPrekill.Add(function(iPlayer, iUnit, iUnitType, iX, iY, bDelay, iByPlayer) end)
#define MOD_EVENTS_UNIT_PREKILL										gCustomMods.isEVENTS_UNIT_PREKILL()

// Events sent before and after a unit's ranged attack on a tile
//   GameEvents.UnitCanRangeAttackAt.Add(function(iPlayer, iUnit, iPlotX, iPlotY, bNeedWar) return false end)
//   GameEvents.UnitRangeAttackAt.Add(function(iPlayer, iUnit, iPlotX, iPlotY) return 0; end)
#define MOD_EVENTS_UNIT_RANGEATTACK									gCustomMods.isEVENTS_UNIT_RANGEATTACK()

// Events sent as units are promoted/upgraded
//   GameEvents.CanHavePromotion.Add(function(iPlayer, iUnit, iPromotionType) return true end)
//   GameEvents.UnitCanHavePromotion.Add(function(iPlayer, iUnit, iPromotionType) return true end)
//   GameEvents.UnitPromoted.Add(function(iPlayer, iUnit, iPromotionType) end)
//   GameEvents.CanHaveAnyUpgrade.Add(function(iPlayer, iUnit) return true end)
//   GameEvents.UnitCanHaveAnyUpgrade.Add(function(iPlayer, iUnit) return true end)
//   GameEvents.CanHaveUpgrade.Add(function(iPlayer, iUnit, iUnitClassType, iUnitType) return true end)
//   GameEvents.UnitCanHaveUpgrade.Add(function(iPlayer, iUnit, iUnitClassType, iUnitType) return true end)
//   GameEvents.UnitUpgraded.Add(function(iPlayer, iOldUnit, iNewUnit, bGoodyHut) end)
#define MOD_EVENTS_UNIT_UPGRADES									gCustomMods.isEVENTS_UNIT_UPGRADES()

// Events sent about war and peace
//   GameEvents.PlayerCanDeclareWar.Add(function(iPlayer, iAgainstTeam) return true end)
//   GameEvents.IsAbleToDeclareWar.Add(function(iPlayer, iAgainstTeam) return true end) - deprecated, use PlayerCanDeclareWar
//   GameEvents.DeclareWar.Add(function(iPlayer, iAgainstTeam, bAggressor) end)
//   GameEvents.PlayerCanMakePeace.Add(function(iPlayer, iAgainstTeam) return true end)
//   GameEvents.IsAbleToMakePeace.Add(function(iPlayer, iAgainstTeam) return true end) - deprecated, use PlayerCanMakePeace
//   GameEvents.MakePeace.Add(function(iPlayer, iAgainstTeam, bPacifier) end)
#define MOD_EVENTS_WAR_AND_PEACE									gCustomMods.isEVENTS_WAR_AND_PEACE()


/////////////////////////////////////////
// SLATED FOR DELETION
/////////////////////////////////////////

#define MOD_AI_UNIT_PRODUCTION										gCustomMods.isAI_UNIT_PRODUCTION()
#define MOD_BALANCE_BOMBARD_RANGE_BUILDINGS							gCustomMods.isBALANCE_BOMBARD_RANGE_BUILDINGS()
#define MOD_BALANCE_NEW_GREAT_PERSON_ATTRIBUTES						gCustomMods.isBALANCE_NEW_GREAT_PERSON_ATTRIBUTES()
#define MOD_LINKED_MOVEMENT											gCustomMods.isLINKED_MOVEMENT()
#define MOD_PROMOTIONS_DEEP_WATER_EMBARKATION						gCustomMods.isPROMOTIONS_DEEP_WATER_EMBARKATION()


/////////////////////////////////////////
// MODMODDERS' OPTIONS
// Sorting order: alphabetical, but JFD is last
/////////////////////////////////////////

// Changes in Milae's Alternative Difficulty Mod
#define MOD_ALTERNATIVE_DIFFICULTY									gCustomMods.isALTERNATIVE_DIFFICULTY()

// Units take damage in enemy lands
#define MOD_ATTRITION												gCustomMods.isATTRITION()

// Activate eureka for tech cost bonus 'quest'
#define MOD_CIV6_EUREKAS											gCustomMods.isCIV6_EUREKAS()

// Roads are built automatically when a trade route ends
#define MOD_CIV6_ROADS												gCustomMods.isCIV6_ROADS()

// Adds a "worker cost" to improvements and deletes the worker when he expands all his "strength"
#define MOD_CIV6_WORKER												gCustomMods.isCIV6_WORKER()

// Enables Iska Heritage modmod support
#define MOD_ISKA_HERITAGE											gCustomMods.isISKA_HERITAGE()
#define MOD_ISKA_PANTHEONS											gCustomMods.isISKA_PANTHEONS()
#define MOD_ISKA_GOLDENAGEPOINTS_TO_PRESTIGE						gCustomMods.isISKA_GOLDENAGEPOINTS_TO_PRESTIGE()

// Enables Not For Sale modmod support
#define MOD_NOT_FOR_SALE											gCustomMods.isNOT_FOR_SALE()

// Enables Route Planner modmod support
#define MOD_ROUTE_PLANNER											gCustomMods.isROUTE_PLANNER()

// Enables support for JFD's Mods
#define MOD_BALANCE_CORE_JFD										gCustomMods.isBALANCE_CORE_JFD()


// Comment out this line to switch off all custom mod logging
#define CUSTOMLOGDEBUG "CustomMods.log"
// Define to include the file name and line number in the log
#define CUSTOMLOGFILEINFO
// Define to include the function name in the log
#define CUSTOMLOGFUNCINFO


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

// Player military event loggers
#define MILITARYLOG(eForPlayer, sMessage, pPlot, eOtherPlayer)                  \
	if (MOD_WH_MILITARY_LOG) {                                                  \
		GET_PLAYER(eForPlayer).AddMilitaryEvent(sMessage, pPlot, eOtherPlayer); \
	}

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

#define BATTLE_STARTED(iType, pPlot)              if (MOD_EVENTS_BATTLES) { GAMEEVENTINVOKE_HOOK(GAMEEVENT_BattleStarted, iType, (pPlot).getX(), (pPlot).getY()); }
#define BATTLE_JOINED(pCombatant, iRole, bIsCity) if (MOD_EVENTS_BATTLES && (pCombatant)) { GAMEEVENTINVOKE_HOOK(GAMEEVENT_BattleJoined, (pCombatant)->getOwner(), (pCombatant)->GetID(), iRole, bIsCity); }
#define BATTLE_FINISHED()                         if (MOD_EVENTS_BATTLES) { GAMEEVENTINVOKE_HOOK(GAMEEVENT_BattleFinished); }

// Custom mod logger
#if defined(CUSTOMLOGDEBUG)
#if defined(CUSTOMLOGFILEINFO) && defined(CUSTOMLOGFUNCINFO)
#define CUSTOMLOG(sFmt, ...) {																					\
	CvString sMsg; CvString::format(sMsg, sFmt, __VA_ARGS__);													\
	CvString sLine; CvString::format(sLine, "%s[%i]: %s - %s", __FILE__, __LINE__, __FUNCTION__, sMsg.c_str());	\
	LOGFILEMGR.GetLog(CUSTOMLOGDEBUG, FILogFile::kDontTimeStamp)->Msg(sLine.c_str());							\
	sLine += '\n'; OutputDebugString(sLine.c_str());																			\
}
#endif
#if defined(CUSTOMLOGFILEINFO) && !defined(CUSTOMLOGFUNCINFO)
#define CUSTOMLOG(sFmt, ...) {																					\
	CvString sMsg; CvString::format(sMsg, sFmt, __VA_ARGS__);													\
	CvString sLine; CvString::format(sLine, "%s[%i] - %s", __FILE__, __LINE__, sMsg.c_str());					\
	LOGFILEMGR.GetLog(CUSTOMLOGDEBUG, FILogFile::kDontTimeStamp)->Msg(sLine.c_str());							\
	sLine += '\n'; OutputDebugString(sLine.c_str());																			\
}
#endif
#if !defined(CUSTOMLOGFILEINFO) && defined(CUSTOMLOGFUNCINFO)
#define CUSTOMLOG(sFmt, ...) {																					\
	CvString sMsg; CvString::format(sMsg, sFmt, __VA_ARGS__);													\
	CvString sLine; CvString::format(sLine, "%s - %s", __FUNCTION__, sMsg.c_str());								\
	LOGFILEMGR.GetLog(CUSTOMLOGDEBUG, FILogFile::kDontTimeStamp)->Msg(sLine.c_str());							\
	sLine += '\n'; OutputDebugString(sLine.c_str());																			\
}
#endif
#if !defined(CUSTOMLOGFILEINFO) && !defined(CUSTOMLOGFUNCINFO)
#define CUSTOMLOG(sFmt, ...) {																					\
	CvString sMsg; CvString::format(sMsg, sFmt, __VA_ARGS__);													\
	LOGFILEMGR.GetLog(CUSTOMLOGDEBUG, FILogFile::kDontTimeStamp)->Msg(sMsg.c_str());							\
	sMsg += '\n'; OutputDebugString(sMsg.c_str());																			\
}
#endif
#else
#define CUSTOMLOG(sFmt, ...) __noop
#endif

#define LIVELOG(sFmt, ...) {																					\
	CvString sMsg; CvString::format(sMsg, sFmt, __VA_ARGS__);													\
	CvString sLine; CvString::format(sLine, "%s[%i]: %s - %s", __FILE__, __LINE__, __FUNCTION__, sMsg.c_str());	\
	sLine += '\n'; OutputDebugString(sLine.c_str());																			\
}

// GlobalDefines wrappers
#define GD_INT_DECL(name)         int m_i##name
#define GD_INT_DEF(name)          inline int get##name() const { return m_i##name; }
#define GD_INT_MEMBER(name) public: GD_INT_DEF(name) private: GD_INT_DECL(name)
#define GD_INT_INIT(name, def)    m_i##name(def)
#define GD_INT_CACHE(name)        getDatabaseValue(#name,m_i##name)
#define GD_INT_GET(name)          GC.get##name()

#define GD_FLOAT_DECL(name)       float m_f##name
#define GD_FLOAT_DEF(name)        inline float get##name() const { return m_f##name; }
#define GD_FLOAT_MEMBER(name) public: GD_FLOAT_DEF(name) private: GD_FLOAT_DECL(name)
#define GD_FLOAT_INIT(name, def)  m_f##name(def)
#define GD_FLOAT_CACHE(name)      getDatabaseValue(#name,m_f##name)
#define GD_FLOAT_GET(name)        GC.get##name()


// Message wrappers
#define SHOW_PLAYER_MESSAGE(pPlayer, szMessage)       if (pPlayer) DLLUI->AddMessage(0, (pPlayer)->GetID(), false, GD_INT_GET(EVENT_MESSAGE_TIME), szMessage)
#define SHOW_CITY_MESSAGE(pCity, ePlayer, szMessage)  if (pCity) DLLUI->AddCityMessage(0, (pCity)->GetIDInfo(), ePlayer, false, GD_INT_GET(EVENT_MESSAGE_TIME), szMessage)
#define SHOW_UNIT_MESSAGE(pUnit, ePlayer, szMessage)  if (pUnit) DLLUI->AddUnitMessage(0, (pUnit)->GetIDInfo(), ePlayer, false, GD_INT_GET(EVENT_MESSAGE_TIME), szMessage)
#define SHOW_PLOT_MESSAGE(pPlot, ePlayer, szMessage)  if (pPlot) DLLUI->AddPlotMessage(0, (pPlot)->GetPlotIndex(), ePlayer, false, GD_INT_GET(EVENT_MESSAGE_TIME), szMessage)
#define SHOW_PLOT_POPUP(pPlot, ePlayer, szMessage)  if (pPlot) (pPlot)->showPopupText(ePlayer, szMessage)


// LUA API wrappers
#define LUAAPIEXTN(method, type, ...) static int l##method(lua_State* L)
#define LUAAPIIMPL(object, method) int CvLua##object::l##method(lua_State* L) { return BasicLuaMethod(L, &Cv##object::method); }
#define LUAAPIINLINE(method, hasMethod, type) inline bool method() const { return hasMethod(type); }


// Game Event wrappers
#include "Lua/CvLuaSupport.h"
#define GAMEEVENTINVOKE_HOOK    gCustomMods.eventHook
#define GAMEEVENTINVOKE_TESTANY gCustomMods.eventTestAny
#define GAMEEVENTINVOKE_TESTALL gCustomMods.eventTestAll
#define GAMEEVENTINVOKE_VALUE   gCustomMods.eventAccumulator

#define GAMEEVENTRETURN_NONE  (-1)
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
#define GAMEEVENT_CityProjectComplete			"CityProjectComplete",			"iii"
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
#define GAMEEVENT_PlayerBullied					"PlayerBullied",				"iiiiiii"
#define GAMEEVENT_PlayerCanDisband				"PlayerCanDisband",				"i"
#define GAMEEVENT_PlayerCanDisbandUnit			"PlayerCanDisbandUnit",			"ii"
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
#define GAMEEVENT_PlayerDoneTurn				"PlayerDoneTurn",				"i"
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
#define GAMEEVENT_UnitCanRangeAttackAt			"UnitCanRangeAttackAt",			"iiiib"
#define GAMEEVENT_UnitCanTransitMinorCity		"UnitCanTransitMinorCity",		"iiiiii"
#define GAMEEVENT_UnitCaptured					"UnitCaptured",					"iiiibi"
#define GAMEEVENT_UnitCaptureType				"UnitCaptureType",				"iiii"
#define GAMEEVENT_UnitCityFounded				"UnitCityFounded",				"iiiii"
#define GAMEEVENT_UnitCreated					"UnitCreated",					"iiiii"
#define GAMEEVENT_UnitPillageGold				"UnitPillageGold",				"iiii"
#define GAMEEVENT_UnitPrekill					"UnitPrekill",					"iiiiibi"
#define GAMEEVENT_UnitPromoted					"UnitPromoted",					"iii"
#define GAMEEVENT_UnitRangeAttackAt				"UnitRangeAttackAt",			"iiii"
#define GAMEEVENT_UnitUpgraded					"UnitUpgraded",					"iiib"
#define GAMEEVENT_UnitConverted					"UnitConverted",				"iiiib"
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
#define GAMEEVENT_CityEndsWLTKD				"CityEndsWLTKD", "iiii"
#define GAMEEVENT_CityExtendsWLTKD			"CityExtendsWLTKD", "iiii"
#define GAMEEVENT_CityRazed					"CityRazed", "iii"
#define GAMEEVENT_CityInvestedBuilding		"CityInvestedBuilding", "iiii"
#define GAMEEVENT_CityInvestedUnit			"CityInvestedUnit", "iiii"
#define GAMEEVENT_ConquerorValidBuilding	"ConquerorValidBuilding", "iiii"
#define GAMEEVENT_GameSave					"GameSave",					""
#define GAMEEVENT_AiFlatApproachChange		"AiFlatApproachChange",		"ii"
#define GAMEEVENT_AiPercentApproachMod		"AiPercentApproachMod",		"ii"
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
#define GAMEEVENT_CityFlipped				"CityFlipped", "iii"
#define GAMEEVENT_CityFlipChance			"CityFlipChance", "ii"
#define GAMEEVENT_CityFlipRecipientChance	"CityFlipChance", "iii"
#define GAMEEVENT_PlayerAnarchy				"PlayerAnarchyBegins", "iii"

// Serialization wrappers
#define MOD_SERIALIZE

#if defined(MOD_SERIALIZE)

//to help debug errors
void CheckSentinel(uint);

#define MOD_SERIALIZE_INIT_READ_NO_SENTINEL(stream) uint uiDllSaveVersion; (stream) >> uiDllSaveVersion;
#define MOD_SERIALIZE_INIT_READ(stream) uint uiDllSaveVersion, uiSentinel; (stream) >> uiDllSaveVersion; (stream) >> uiSentinel; CheckSentinel(uiSentinel);
#define MOD_SERIALIZE_READ(version, stream, member, def) if (uiDllSaveVersion >= (version)) { (stream) >> (member); } else { (member) = def; }
#define MOD_SERIALIZE_READ_AUTO(version, stream, member, size, def)   \
	if (uiDllSaveVersion >= (version)) {                                \
		(stream) >> (member);                                             \
	} else {                                                          \
		for (int iI = 0; iI < (size); iI++) { (member).setAt(iI, def); }  \
	}
#define MOD_SERIALIZE_READ_ARRAY(version, stream, member, type, size, def)	\
	if (uiDllSaveVersion >= (version)) {										\
		ArrayWrapper<type> wrapper(size, member); (stream) >> wrapper;		\
	} else {																\
		for (int iI = 0; iI < (size); iI++) { (member)[iI] = def; }			\
	}
#define MOD_SERIALIZE_READ_HASH(version, stream, member, type, size, def)		\
	if (uiDllSaveVersion >= (version)) {											\
		CvInfosSerializationHelper::ReadHashedDataArray(stream, member, size);	\
	} else {																	\
		for (int iI = 0; iI < (size); iI++) { (member)[iI] = def; }				\
	}

#define MOD_SERIALIZE_INIT_WRITE_NO_SENTINEL(stream) uint uiDllSaveVersion = MOD_DLL_VERSION_NUMBER; (stream) << uiDllSaveVersion;
#define MOD_SERIALIZE_INIT_WRITE(stream) uint uiDllSaveVersion = MOD_DLL_VERSION_NUMBER; (stream) << uiDllSaveVersion; (stream) << 0xDEADBEEF;
#define MOD_SERIALIZE_WRITE(stream, member) ASSERT(uiDllSaveVersion == MOD_DLL_VERSION_NUMBER); (stream) << member
#define MOD_SERIALIZE_WRITE_AUTO(stream, member) ASSERT(uiDllSaveVersion == MOD_DLL_VERSION_NUMBER); (stream) << member
#define MOD_SERIALIZE_WRITE_ARRAY(stream, member, type, size) ASSERT(uiDllSaveVersion == MOD_DLL_VERSION_NUMBER); (stream) << ArrayWrapper<type>(size, member)
#define MOD_SERIALIZE_WRITE_CONSTARRAY(stream, member, type, size) ASSERT(uiDllSaveVersion == MOD_DLL_VERSION_NUMBER); (stream) << ArrayWrapperConst<type>(size, member)
#define MOD_SERIALIZE_WRITE_HASH(stream, member, type, size, obj) ASSERT(uiDllSaveVersion == MOD_DLL_VERSION_NUMBER); CvInfosSerializationHelper::WriteHashedDataArray<obj, type>(stream, member, size)
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
#define MOD_OPT_DECL(name)  protected: bool m_b##name:1; public: inline bool is##name() { return m_b##name; }
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

private:
	// Helper function to parse variable arguments into CvLuaArgsHandle
	void parseVariableArgs(CvLuaArgsHandle &args, const char* p, va_list vl);

public:
	void prefetchCache();
	void preloadCache();
	void reloadCache();
	int getOption(const char* szOption, int defValue = 0);
	int getOption(const std::string& sOption, int defValue = 0);
	int getCivOption(const char* szCiv, const char* szName, int defValue = 0);

	// Keystone Options
	MOD_OPT_DECL(BALANCE_VP);
	MOD_OPT_DECL(ACTIVE_DIPLOMACY);
	MOD_OPT_DECL(BIN_HOOKS);
	MOD_OPT_DECL(ENABLE_ACHIEVEMENTS);
	MOD_OPT_DECL(DIPLO_DEBUG_MODE);
	MOD_OPT_DECL(CORE_DEBUGGING);
	MOD_OPT_DECL(SQUADS);

	// Core Balance Changes
	MOD_OPT_DECL(CORE_DELAYED_VISIBILITY);
	MOD_OPT_DECL(CORE_GARRISON_DAMAGE_ABSORPTION);
	MOD_OPT_DECL(CORE_PUPPET_BUILDING_LIMITATIONS);
	MOD_OPT_DECL(CORE_CIVILIANS_RETREAT_WITH_MILITARY);
	MOD_OPT_DECL(CORE_PERSISTENT_DEFENSIVE_PACTS);
	MOD_OPT_DECL(CORE_ENGINEER_HURRY);
	MOD_OPT_DECL(CORE_NO_NAVAL_RANGED_ATTACKS_FROM_CANALS);
	MOD_OPT_DECL(CORE_NO_REPAIRING_FOREIGN_LANDS);
	MOD_OPT_DECL(CORE_NO_INTERMAJOR_UNIT_GIFTING);
	MOD_OPT_DECL(CORE_NO_HEALING_ON_MOUNTAINS);
	MOD_OPT_DECL(CORE_NO_YIELD_ICE);

	// Core User Interface Changes
	MOD_OPT_DECL(COREUI_REDUCE_NOTIFICATIONS);
	MOD_OPT_DECL(COREUI_DIPLOMACY_ERA_INFLUENCE);

	// Core AI Changes
	MOD_OPT_DECL(DEALAI_HUMAN_PERMANENT_FOR_AI_TEMPORARY);
	MOD_OPT_DECL(DEALAI_NO_PEACETIME_SELLING_FOUNDED_CITIES);
	MOD_OPT_DECL(DIPLOAI_ENABLE_NUCLEAR_GANDHI);
	MOD_OPT_DECL(DIPLOAI_HONEST_OPINION_MODIFIERS);

	// Core Pick'N'Mix Mods
	MOD_OPT_DECL(GLOBAL_TRULY_FREE_GP);
	MOD_OPT_DECL(GLOBAL_CS_LIBERATE_AFTER_BUYOUT);
	MOD_OPT_DECL(GLOBAL_NO_LOST_GREATWORKS);
	MOD_OPT_DECL(RELIGION_KEEP_PROPHET_OVERFLOW);

	// Enabled Pick'N'Mix Mods (VP Only)
	MOD_OPT_DECL(GLOBAL_SEPARATE_GP_COUNTERS);
	MOD_OPT_DECL(GLOBAL_PASSABLE_FORTS);
	MOD_OPT_DECL(GLOBAL_PARATROOPS_AA_DAMAGE);
	MOD_OPT_DECL(GLOBAL_CS_GIFTS);
	MOD_OPT_DECL(GLOBAL_CS_GIFT_SHIPS);
	MOD_OPT_DECL(GLOBAL_CS_GIFTS_LOCAL_XP);
	MOD_OPT_DECL(GLOBAL_CITY_FOREST_BONUS);
	MOD_OPT_DECL(GLOBAL_CITY_JUNGLE_BONUS);

	// Disabled Pick'N'Mix Mods
	MOD_OPT_DECL(BUILDINGS_NW_EXCLUDE_RAZING);
	MOD_OPT_DECL(BUILDINGS_PRO_RATA_PURCHASE);
	MOD_OPT_DECL(DIPLOMACY_TECH_BONUSES);
	MOD_OPT_DECL(GLOBAL_CS_OVERSEAS_TERRITORY);
	MOD_OPT_DECL(GLOBAL_CS_UPGRADES);
	MOD_OPT_DECL(GLOBAL_GRATEFUL_SETTLERS);
	MOD_OPT_DECL(GLOBAL_LOCAL_GENERALS);
	MOD_OPT_DECL(LOCAL_GENERALS_NEAREST_CITY);
	MOD_OPT_DECL(GLOBAL_NO_CONQUERED_SPACESHIPS);
	MOD_OPT_DECL(GLOBAL_NO_OCEAN_PLUNDERING);
	MOD_OPT_DECL(GLOBAL_NUKES_MELT_ICE);
	MOD_OPT_DECL(GLOBAL_PURCHASE_FAITH_BUILDINGS_IN_PUPPETS);
	MOD_OPT_DECL(GLOBAL_RELIGIOUS_SETTLERS);
	MOD_OPT_DECL(RELIGIOUS_SETTLERS_WEIGHTED);
	MOD_OPT_DECL(GLOBAL_SUBS_UNDER_ICE_IMMUNITY);
	MOD_OPT_DECL(PROCESS_STOCKPILE);
	MOD_OPT_DECL(RELIGION_ALLIED_INQUISITORS);
	MOD_OPT_DECL(RELIGION_NO_PREFERENCES);
	MOD_OPT_DECL(RELIGION_NO_PREFERRENCES); // typo version kept for backwards compatibility
	MOD_OPT_DECL(RELIGION_RANDOMISE);
	MOD_OPT_DECL(RELIGION_RECURRING_PURCHASE_NOTIFY);
	MOD_OPT_DECL(UI_CITY_EXPANSION);
	MOD_OPT_DECL(UNITS_HOVERING_LAND_ONLY_HEAL);
	MOD_OPT_DECL(GLOBAL_STACKING_RULES); // disabled
	MOD_OPT_DECL(GLOBAL_QUICK_ROUTES); // disabled

	// Vox Populi Balance Changes
	MOD_OPT_DECL(BALANCE_ALTERNATE_INDONESIA_TRAIT);
	MOD_OPT_DECL(BALANCE_ALTERNATE_MAYA_TRAIT);
	MOD_OPT_DECL(BALANCE_ALTERNATE_SIAM_TRAIT);
	MOD_OPT_DECL(BALANCE_UNIQUE_BELIEFS_ONLY_FOR_CIV);
	MOD_OPT_DECL(BALANCE_CULTURE_VICTORY_CHANGES);
	MOD_OPT_DECL(BALANCE_BUILDING_INVESTMENTS);
	MOD_OPT_DECL(BALANCE_RESOURCE_MONOPOLIES);
	MOD_OPT_DECL(BALANCE_STRATEGIC_RESOURCE_MONOPOLIES);
	MOD_OPT_DECL(BALANCE_HEAVY_TRIBUTE);
	MOD_OPT_DECL(BALANCE_MINOR_PROTECTION_REQUIREMENTS);
	MOD_OPT_DECL(BALANCE_CITY_STRENGTH_SWITCH);
	MOD_OPT_DECL(BALANCE_RIVER_CITY_CONNECTIONS);
	MOD_OPT_DECL(BALANCE_SPY_POINTS);
	MOD_OPT_DECL(BALANCE_AIR_UNIT_CHANGES);
	MOD_OPT_DECL(BALANCE_RANGED_DEFENSE_UNIT_HEALTH);
	MOD_OPT_DECL(BALANCE_LAND_UNITS_ADJACENT_BLOCKADE);
	MOD_OPT_DECL(BALANCE_RECON_ONLY_ANCIENT_RUINS);
	MOD_OPT_DECL(BALANCE_INQUISITOR_NERF);
	MOD_OPT_DECL(BALANCE_RESOURCE_SHORTAGE_UNIT_HEALING);
	MOD_OPT_DECL(BALANCE_RESOURCE_SHORTAGE_BUILDING_REFUND);
	MOD_OPT_DECL(BALANCE_RETROACTIVE_PROMOTIONS);
	MOD_OPT_DECL(BALANCE_PUPPET_CHANGES);
	MOD_OPT_DECL(BALANCE_RAZING_CREATES_PARTISANS);
	MOD_OPT_DECL(BALANCE_HALF_XP_GOLD_PURCHASES);
	MOD_OPT_DECL(BALANCE_PURCHASED_UNIT_DAMAGE);
	MOD_OPT_DECL(BALANCE_SETTLERS_CONSUME_POPULATION);
	MOD_OPT_DECL(BALANCE_ENCAMPMENTS_SPAWN_ON_VISIBLE_TILES);
	MOD_OPT_DECL(BALANCE_BARBARIAN_THEFT);
	MOD_OPT_DECL(BALANCE_WORLD_WONDER_COST_INCREASE);
	MOD_OPT_DECL(BALANCE_TRADE_ROUTE_PROXIMITY_PENALTY);
	MOD_OPT_DECL(BALANCE_TRADE_ROUTE_DESTINATION_RESTRICTION);
	MOD_OPT_DECL(BALANCE_WONDERS_VARIABLE_CONSOLATION);
	MOD_OPT_DECL(BALANCE_FLIPPED_OPEN_BORDERS_TOURISM);
	MOD_OPT_DECL(BALANCE_NO_WARTIME_CONCERT_TOURS);
	MOD_OPT_DECL(BALANCE_PRISONERS_OF_WAR);
	MOD_OPT_DECL(BALANCE_PURCHASE_COST_ADJUSTMENTS);
	MOD_OPT_DECL(BALANCE_PERMANENT_VOTE_COMMITMENTS);
	MOD_OPT_DECL(BALANCE_QUEST_CHANGES);
	MOD_OPT_DECL(BALANCE_RESILIENT_PANTHEONS);

	// Other Balance Options
	MOD_OPT_DECL(BALANCE_ALTERNATE_ASSYRIA_TRAIT);
	MOD_OPT_DECL(BALANCE_ALTERNATE_CELTS_TRAIT);
	MOD_OPT_DECL(BALANCE_ANY_PANTHEON);
	MOD_OPT_DECL(BALANCE_CITY_STATE_SCALE);
	MOD_OPT_DECL(BALANCE_CITY_STATE_TRAITS);
	MOD_OPT_DECL(BALANCE_CITY_STATE_PERSONALITIES);
	MOD_OPT_DECL(BALANCE_ERA_RESTRICTED_GENERALS);
	MOD_OPT_DECL(BALANCE_ERA_RESTRICTION);
	MOD_OPT_DECL(BALANCE_GREAT_PEOPLE_ERA_SCALING);
	MOD_OPT_DECL(BALANCE_MINOR_UNIT_SUPPLY_HANDICAP);
	MOD_OPT_DECL(BALANCE_NO_AUTO_SPAWN_PROPHET);
	MOD_OPT_DECL(BALANCE_NO_CITY_RANGED_ATTACK);
	MOD_OPT_DECL(BALANCE_NO_GAP_DURING_GA);
	MOD_OPT_DECL(BALANCE_NO_RESOURCE_YIELDS_FROM_GP_IMPROVEMENT);
	MOD_OPT_DECL(BALANCE_PASSIVE_SPREAD_BY_CONNECTION);
	MOD_OPT_DECL(BALANCE_PERMANENT_PANTHEONS);
	MOD_OPT_DECL(BALANCE_PILLAGE_PERMANENT_IMPROVEMENTS);
	MOD_OPT_DECL(BALANCE_RANDOMIZED_GREAT_PROPHET_SPAWNS);
	MOD_OPT_DECL(BALANCE_RELAXED_BORDER_CHECK);
	MOD_OPT_DECL(BALANCE_SANE_UNIT_MOVEMENT_COST);
	MOD_OPT_DECL(BALANCE_SETTLERS_RESET_GROWTH);
	MOD_OPT_DECL(BALANCE_UNCAPPED_UNHAPPINESS);
	MOD_OPT_DECL(BALANCE_UNIT_INVESTMENTS);
	MOD_OPT_DECL(BALANCE_XP_ON_FIRST_ATTACK);
	MOD_OPT_DECL(CARGO_SHIPS); // disabled
	MOD_OPT_DECL(BALANCE_HALF_XP_FAITH_PURCHASES);

	// Other User Interface Options
	MOD_OPT_DECL(UI_DISPLAY_PRECISE_MOVEMENT_POINTS);
	MOD_OPT_DECL(UI_NO_RANDOM_CIV_DIALOGUE);
	MOD_OPT_DECL(UI_QUICK_ANIMATIONS);

	// Other AI Options
	MOD_OPT_DECL(AI_CONTROL_CITY_MANAGEMENT);
	MOD_OPT_DECL(AI_CONTROL_CITY_PRODUCTION);
	MOD_OPT_DECL(AI_CONTROL_DIPLOMACY);
	MOD_OPT_DECL(AI_CONTROL_ECONOMY);
	MOD_OPT_DECL(AI_CONTROL_ESPIONAGE);
	MOD_OPT_DECL(AI_CONTROL_EVENT_CHOICE);
	MOD_OPT_DECL(AI_CONTROL_FAITH_SPENDING);
	MOD_OPT_DECL(AI_CONTROL_GREAT_PERSON_CHOICE);
	MOD_OPT_DECL(AI_CONTROL_POLICY_CHOICE);
	MOD_OPT_DECL(AI_CONTROL_RELIGION_CHOICE);
	MOD_OPT_DECL(AI_CONTROL_TECH_CHOICE);
	MOD_OPT_DECL(AI_CONTROL_TOURISM);
	MOD_OPT_DECL(AI_CONTROL_UNIT_PROMOTIONS);
	MOD_OPT_DECL(AI_CONTROL_UNITS);
	MOD_OPT_DECL(AI_CONTROL_WORLD_CONGRESS);
	MOD_OPT_DECL(HUMAN_USES_AI_HANDICAP);
	MOD_OPT_DECL(HUMAN_USES_AI_MECHANICS);
	MOD_OPT_DECL(COMBATAI_TWO_PASS_DANGER);
	MOD_OPT_DECL(DEALAI_DISABLE_CITY_TRADES);
	MOD_OPT_DECL(DEALAI_DISABLE_WAR_BRIBES);
	MOD_OPT_DECL(DEALAI_GLOBAL_PERMANENT_FOR_TEMPORARY);
	MOD_OPT_DECL(DIPLOAI_AGGRESSIVE_MODE);
	MOD_OPT_DECL(DIPLOAI_DISABLE_DOMINATION_ONLY_AGGRESSION);
	MOD_OPT_DECL(DIPLOAI_HIDE_OPINION_TABLE);
	MOD_OPT_DECL(DIPLOAI_LIMIT_VICTORY_PURSUIT_RANDOMIZATION);
	MOD_OPT_DECL(DIPLOAI_NO_FLAVOR_RANDOMIZATION);
	MOD_OPT_DECL(DIPLOAI_PASSIVE_MODE);
	MOD_OPT_DECL(DIPLOAI_SHOW_ALL_OPINION_VALUES);
	MOD_OPT_DECL(DIPLOAI_SHOW_BASE_HUMAN_OPINION);
	MOD_OPT_DECL(DIPLOAI_SHOW_HIDDEN_OPINION_MODIFIERS);
	MOD_OPT_DECL(DIPLOAI_SHUT_UP);
	MOD_OPT_DECL(DIPLOAI_SHUT_UP_COMPLIMENTS);
	MOD_OPT_DECL(DIPLOAI_SHUT_UP_COOP_WAR_OFFERS);
	MOD_OPT_DECL(DIPLOAI_SHUT_UP_DEMANDS);
	MOD_OPT_DECL(DIPLOAI_SHUT_UP_FRIENDSHIP_OFFERS);
	MOD_OPT_DECL(DIPLOAI_SHUT_UP_GIFT_OFFERS);
	MOD_OPT_DECL(DIPLOAI_SHUT_UP_HELP_REQUESTS);
	MOD_OPT_DECL(DIPLOAI_SHUT_UP_INDEPENDENCE_REQUESTS);
	MOD_OPT_DECL(DIPLOAI_SHUT_UP_INSULTS);
	MOD_OPT_DECL(DIPLOAI_SHUT_UP_PEACE_OFFERS);
	MOD_OPT_DECL(DIPLOAI_SHUT_UP_TRADE_OFFERS);
	MOD_OPT_DECL(DIPLOAI_SHUT_UP_TRADE_RENEWALS);
	MOD_OPT_DECL(LEAGUEAI_NO_OTHER_HOST_VOTES);
	MOD_OPT_DECL(LEAGUEAI_NO_OTHER_WORLD_LEADER_VOTES);

	// API Extensions
	MOD_OPT_DECL(API_AREA_EFFECT_PROMOTIONS);
	MOD_OPT_DECL(API_DISABLE_LUA_HOOKS);
	MOD_OPT_DECL(API_HOVERING_UNITS);
	MOD_OPT_DECL(BUILDINGS_THOROUGH_PREREQUISITES);
	MOD_OPT_DECL(IMPROVEMENTS_EXTENSIONS);
	MOD_OPT_DECL(LOG_MAP_STATE);
	MOD_OPT_DECL(PLOTS_EXTENSIONS);
	MOD_OPT_DECL(PROMOTIONS_FLAGSHIP);
	MOD_OPT_DECL(PROMOTIONS_IMPROVEMENT_BONUS);
	MOD_OPT_DECL(RELIGION_EXTENSIONS);
	MOD_OPT_DECL(RELIGION_LOCAL_RELIGIONS);
	MOD_OPT_DECL(RESOURCES_PRODUCTION_COST_MODIFIERS);
	MOD_OPT_DECL(TRADE_INTERNAL_GOLD_ROUTES);
	MOD_OPT_DECL(TRADE_WONDER_RESOURCE_ROUTES);
	MOD_OPT_DECL(TRAITS_TRADE_ROUTE_PRODUCTION_SIPHON);
	MOD_OPT_DECL(TRAITS_YIELD_FROM_ROUTE_MOVEMENT_IN_FOREIGN_TERRITORY);
	MOD_OPT_DECL(UNIT_KILL_STATS);
	MOD_OPT_DECL(UNITS_RESOURCE_QUANTITY_TOTALS);
	MOD_OPT_DECL(WH_MILITARY_LOG);
	MOD_OPT_DECL(YIELD_MODIFIER_FROM_UNITS);

	// Events
	MOD_OPT_DECL(EVENTS_ACQUIRE_BELIEFS);
	MOD_OPT_DECL(EVENTS_AI_OVERRIDE_TECH);
	MOD_OPT_DECL(EVENTS_AIRLIFT);
	MOD_OPT_DECL(EVENTS_AREA_RESOURCES);
	MOD_OPT_DECL(EVENTS_BARBARIANS);
	MOD_OPT_DECL(EVENTS_BATTLES);
	MOD_OPT_DECL(EVENTS_BATTLES_DAMAGE);
	MOD_OPT_DECL(EVENTS_CAN_MOVE_INTO);
	MOD_OPT_DECL(EVENTS_CIRCUMNAVIGATION);
	MOD_OPT_DECL(EVENTS_CITY);
	MOD_OPT_DECL(EVENTS_CITY_AIRLIFT);
	MOD_OPT_DECL(EVENTS_CITY_BOMBARD);
	MOD_OPT_DECL(EVENTS_CITY_BORDERS);
	MOD_OPT_DECL(EVENTS_CITY_CAPITAL);
	MOD_OPT_DECL(EVENTS_CITY_CONNECTIONS);
	MOD_OPT_DECL(EVENTS_CITY_FOUNDING);
	MOD_OPT_DECL(EVENTS_CITY_RAZING);
	MOD_OPT_DECL(EVENTS_COMMAND);
	MOD_OPT_DECL(EVENTS_CUSTOM_MISSIONS);
	MOD_OPT_DECL(EVENTS_DIPLO_EVENTS);
	MOD_OPT_DECL(EVENTS_DIPLO_MODIFIERS);
	MOD_OPT_DECL(EVENTS_ESPIONAGE);
	MOD_OPT_DECL(EVENTS_FOUND_RELIGION);
	MOD_OPT_DECL(EVENTS_GAME_SAVE);
	MOD_OPT_DECL(EVENTS_GOLDEN_AGE);
	MOD_OPT_DECL(EVENTS_GOODY_CHOICE);
	MOD_OPT_DECL(EVENTS_GOODY_TECH);
	MOD_OPT_DECL(EVENTS_GREAT_PEOPLE);
	MOD_OPT_DECL(EVENTS_IDEOLOGIES);
	MOD_OPT_DECL(EVENTS_LIBERATION);
	MOD_OPT_DECL(EVENTS_MINORS);
	MOD_OPT_DECL(EVENTS_MINORS_GIFTS);
	MOD_OPT_DECL(EVENTS_MINORS_INTERACTION);
	MOD_OPT_DECL(EVENTS_NEW_ERA);
	MOD_OPT_DECL(EVENTS_NUCLEAR_DETONATION);
	MOD_OPT_DECL(EVENTS_NW_DISCOVERY);
	MOD_OPT_DECL(EVENTS_PARADROPS);
	MOD_OPT_DECL(EVENTS_PLAYER_TURN);
	MOD_OPT_DECL(EVENTS_PLOT);
	MOD_OPT_DECL(EVENTS_REBASE);
	MOD_OPT_DECL(EVENTS_RED_TURN);
	MOD_OPT_DECL(EVENTS_RED_COMBAT);
	MOD_OPT_DECL(EVENTS_RED_COMBAT_MISSION);
	MOD_OPT_DECL(EVENTS_RED_COMBAT_ABORT);
	MOD_OPT_DECL(EVENTS_RED_COMBAT_RESULT);
	MOD_OPT_DECL(EVENTS_RED_COMBAT_ENDED);
	MOD_OPT_DECL(EVENTS_RELIGION);
	MOD_OPT_DECL(EVENTS_RESOLUTIONS);
	MOD_OPT_DECL(EVENTS_TERRAFORMING);
	MOD_OPT_DECL(EVENTS_TILE_IMPROVEMENTS);
	MOD_OPT_DECL(EVENTS_TILE_REVEALED);
	MOD_OPT_DECL(EVENTS_TRADE_ROUTE_PLUNDERED);
	MOD_OPT_DECL(EVENTS_TRADE_ROUTES);
	MOD_OPT_DECL(EVENTS_UNIT_ACTIONS);
	MOD_OPT_DECL(EVENTS_UNIT_CAPTURE);
	MOD_OPT_DECL(EVENTS_UNIT_CONVERTS);
	MOD_OPT_DECL(EVENTS_UNIT_CREATED);
	MOD_OPT_DECL(EVENTS_UNIT_DATA);
	MOD_OPT_DECL(EVENTS_UNIT_FOUNDED);
	MOD_OPT_DECL(EVENTS_UNIT_PREKILL);
	MOD_OPT_DECL(EVENTS_UNIT_RANGEATTACK);
	MOD_OPT_DECL(EVENTS_UNIT_UPGRADES);
	MOD_OPT_DECL(EVENTS_WAR_AND_PEACE);

	// Slated for Deletion
	MOD_OPT_DECL(AI_UNIT_PRODUCTION);
	MOD_OPT_DECL(BALANCE_BOMBARD_RANGE_BUILDINGS);
	MOD_OPT_DECL(BALANCE_NEW_GREAT_PERSON_ATTRIBUTES);
	MOD_OPT_DECL(LINKED_MOVEMENT);
	MOD_OPT_DECL(PROMOTIONS_DEEP_WATER_EMBARKATION);

	// Modmodders' Options
	MOD_OPT_DECL(ALTERNATIVE_DIFFICULTY);
	MOD_OPT_DECL(ATTRITION);
	MOD_OPT_DECL(CIV6_EUREKAS);
	MOD_OPT_DECL(CIV6_ROADS);
	MOD_OPT_DECL(CIV6_WORKER);
	MOD_OPT_DECL(ISKA_HERITAGE);
	MOD_OPT_DECL(ISKA_PANTHEONS);
	MOD_OPT_DECL(ISKA_GOLDENAGEPOINTS_TO_PRESTIGE);
	MOD_OPT_DECL(NOT_FOR_SALE);
	MOD_OPT_DECL(ROUTE_PLANNER);
	MOD_OPT_DECL(BALANCE_CORE_JFD);

protected:
	bool m_bInit;
	std::map<std::string, int> m_options;
};

extern CustomMods gCustomMods;
#endif
