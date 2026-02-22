// CustomMods.cpp
#include "CvGameCoreDLLPCH.h"

// must be included after all other headers
#include "LintFree.h"

CustomMods gCustomMods;

CustomMods::CustomMods() :
	m_bInit(false)
{
	// Note: MOD_OPT_DECL bit fields are initialized by preloadCache() before being accessed
	// The cppcheck warnings about uninitialized variables are false positives
}

// Helper function to parse variable arguments into CvLuaArgsHandle
void CustomMods::parseVariableArgs(CvLuaArgsHandle &args, const char* p, va_list vl) {
	// Iterate through the format string to handle each argument.
	for (const char* it = p; *it; ++it) {
		if (*it == 'b') {
			// Handle boolean arguments.
			args->Push(!!va_arg(vl, int));
		} else if (*it == 'i') {
			// Handle integer arguments.
			args->Push(va_arg(vl, int));
		} else {
			// Assume it's a string argument.
			char* s = va_arg(vl, char*);
			args->Push(s, strlen(s));
			break; // Exit after processing the first string argument.
		}
	}
}

// This function hooks an event with a variable number of arguments.
int CustomMods::eventHook(const char* szName, const char* p, ...) {
	// Initialize an argument handle for Lua.
	CvLuaArgsHandle args;

	// Initialize the variable argument list.
	va_list vl;
	va_start(vl, p);

	// Parse arguments using helper function.
	parseVariableArgs(args, p, vl);

	// Clean up the variable argument list.
	va_end(vl);

	// Call the event hook with the constructed arguments.
	return eventHook(szName, args);
}

// This function tests all conditions for an event with a variable number of arguments.
int CustomMods::eventTestAll(const char* szName, const char* p, ...) {
	// Initialize an argument handle for Lua.
	CvLuaArgsHandle args;

	// Initialize the variable argument list.
    va_list vl;
    va_start(vl, p);

	// Parse arguments using helper function.
	parseVariableArgs(args, p, vl);

	// Clean up the variable argument list.
	va_end(vl);

	// Call the event test function with the constructed arguments.
	return eventTestAll(szName, args);
}

// This function tests any condition for an event with a variable number of arguments.
int CustomMods::eventTestAny(const char* szName, const char* p, ...) {
	// Initialize an argument handle for Lua.
	CvLuaArgsHandle args;

	// Initialize the variable argument list.
	va_list vl;
	va_start(vl, p);

	// Parse arguments using helper function.
	parseVariableArgs(args, p, vl);

	// Clean up the variable argument list.
	va_end(vl);

	// Call the event test function with the constructed arguments.
	return eventTestAny(szName, args);
}

// This function accumulates results for an event with a variable number of arguments.
int CustomMods::eventAccumulator(int &iValue, const char* szName, const char* p, ...) {
	// Initialize an argument handle for Lua.
	CvLuaArgsHandle args;

	// Initialize the variable argument list.
	va_list vl;
	va_start(vl, p);

	// Parse arguments using helper function.
	parseVariableArgs(args, p, vl);

	// Clean up the variable argument list.
	va_end(vl);

	// Call the event accumulator with the constructed arguments.
	return eventAccumulator(iValue, szName, args);
}

int CustomMods::eventHook(const char* szName, CvLuaArgsHandle &args) {
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem) {
		bool bResult = false;
		if (LuaSupport::CallHook(pkScriptSystem, szName, args.get(), bResult)) {
			return GAMEEVENTRETURN_HOOK;
		}
	}

	return GAMEEVENTRETURN_NONE;
}

int CustomMods::eventTestAll(const char* szName, CvLuaArgsHandle &args) {
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem) {
		bool bResult = false;
		if (LuaSupport::CallTestAll(pkScriptSystem, szName, args.get(), bResult)) {
			if (bResult) {
				return GAMEEVENTRETURN_TRUE;
			} else {
				return GAMEEVENTRETURN_FALSE;
			}
		}
	}

	return GAMEEVENTRETURN_NONE;
}

int CustomMods::eventTestAny(const char* szName, CvLuaArgsHandle &args) {
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem) {
		bool bResult = false;
		if (LuaSupport::CallTestAny(pkScriptSystem, szName, args.get(), bResult)) {
			if (bResult) {
				return GAMEEVENTRETURN_TRUE;
			} else {
				return GAMEEVENTRETURN_FALSE;
			}
		}
	}

	return GAMEEVENTRETURN_NONE;
}

int CustomMods::eventAccumulator(int &iValue, const char* szName, CvLuaArgsHandle &args) {
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem) {
		if (LuaSupport::CallAccumulator(pkScriptSystem, szName, args.get(), iValue)) {
			return GAMEEVENTRETURN_TRUE;
		}
	}

	return GAMEEVENTRETURN_NONE;
}


// Update CustomModOptions table from references in CustomModPostDefines
// Based on code in CvDllDatabaseUtility::PerformDatabasePostProcessing()
void CustomMods::prefetchCache() {
	Database::Connection* db = GC.GetGameDatabase();
	db->BeginTransaction();

	Database::Results kInsert;
	db->Execute(kInsert, "INSERT OR REPLACE INTO CustomModOptions(Name, Value) VALUES(?, ?)");

	Database::Results kPostDefines;
	db->SelectAll(kPostDefines, "CustomModPostDefines");
	while (kPostDefines.Step()) {
		Database::Results kLookup;
		char szSQL[512];
		sprintf_s(szSQL, "select ROWID from %s where Type = '%s' LIMIT 1", kPostDefines.GetText("Table"), kPostDefines.GetText("Type"));

		if (db->Execute(kLookup, szSQL)) {
			if (kLookup.Step()) {
				kInsert.Bind(1, kPostDefines.GetText("Name"));
				kInsert.Bind(2, kLookup.GetInt(0));
				kInsert.Step();
				kInsert.Reset();
			}
		}
	}

	db->EndTransaction();
}

void CustomMods::preloadCache() {
	prefetchCache();

	(void) getOption("EVENTS_CIRCUMNAVIGATION");
}

void CustomMods::reloadCache() {
	m_bInit = false;

	preloadCache();
}

int CustomMods::getOption(const char* szOption, int defValue)
{
	return getOption(string(szOption), defValue);
}

int CustomMods::getOption(const string& sOption, int defValue)
{
	if (!m_bInit)
	{
		const char* szBadPrefix = "MOD_";

		Database::Results kResults;
		DB.SelectAll(kResults, MOD_DB_TABLE);

		while (kResults.Step())
		{
			const char* szName = kResults.GetText(MOD_DB_COL_NAME);
			const int iDbUpdates = kResults.GetInt(MOD_DB_COL_DBUPDATES);
			int iValue = kResults.GetInt(MOD_DB_COL_VALUE);

			bool bPrefixError = (strncmp(szName, szBadPrefix, strlen(szBadPrefix)) == 0);

			if (iValue && iDbUpdates)
			{
				Database::Results kUpdates;
				char szQuery[512] = {0};

				// Did the required mods to the database occur?  We'll assume they didn't, unless proven otherwise!
				bool bOK = false;

				sprintf_s(szQuery, "Name='%s' AND Value=1", szName);
				if (DB.SelectWhere(kUpdates, MOD_DB_UPDATES, szQuery))
				{
					if (kUpdates.Step())
					{
						// BINGO!  We have our updates
						bOK = true;
					} else
					{
						// All is not lost as there could be BOTH xml and sql updates
						sprintf_s(szQuery, "Name='%s_SQL' AND Value=1", szName);
						if (DB.SelectWhere(kUpdates, MOD_DB_UPDATES, szQuery))
						{
							if (kUpdates.Step())
							{
								sprintf_s(szQuery, "Name='%s_XML' AND Value=1", szName);
								if (DB.SelectWhere(kUpdates, MOD_DB_UPDATES, szQuery))
								{
									if (kUpdates.Step())
									{
										// BINGO!  We have BOTH our updates
										bOK = true;
									}
								}
							}
						}
					}
				}

				if (bOK)
				{
					CUSTOMLOG("%s: %s appears to have the required database updates", (bPrefixError ? "PREFIX ERROR" : "Cache"), szName);
				}
				else
				{
					CUSTOMLOG("%s: %s has missing database updates!", (bPrefixError ? "PREFIX ERROR" : "Cache"), szName);
					iValue = 0;
				}
			}

			CUSTOMLOG("%s: %s = %d", (bPrefixError ? "PREFIX ERROR" : "Cache"), szName, iValue);
			m_options[string(szName)] = iValue;
		}

		// Keystone Options
		MOD_OPT_CACHE(BALANCE_VP);
		MOD_OPT_CACHE(ACTIVE_DIPLOMACY);
		MOD_OPT_CACHE(BIN_HOOKS);
		MOD_OPT_CACHE(ENABLE_ACHIEVEMENTS);
		MOD_OPT_CACHE(DIPLO_DEBUG_MODE);
		MOD_OPT_CACHE(CORE_DEBUGGING);
		MOD_OPT_CACHE(SQUADS);

		// Core Balance Changes
		MOD_OPT_CACHE(CORE_DELAYED_VISIBILITY);
		MOD_OPT_CACHE(CORE_GARRISON_DAMAGE_ABSORPTION);
		MOD_OPT_CACHE(CORE_PUPPET_BUILDING_LIMITATIONS);
		MOD_OPT_CACHE(CORE_CIVILIANS_RETREAT_WITH_MILITARY);
		MOD_OPT_CACHE(CORE_PERSISTENT_DEFENSIVE_PACTS);
		MOD_OPT_CACHE(CORE_ENGINEER_HURRY);
		MOD_OPT_CACHE(CORE_NO_NAVAL_RANGED_ATTACKS_FROM_CANALS);
		MOD_OPT_CACHE(CORE_NO_REPAIRING_FOREIGN_LANDS);
		MOD_OPT_CACHE(CORE_NO_INTERMAJOR_UNIT_GIFTING);
		MOD_OPT_CACHE(CORE_NO_HEALING_ON_MOUNTAINS);
		MOD_OPT_CACHE(CORE_NO_YIELD_ICE);

		// Core User Interface Changes
		MOD_OPT_CACHE(COREUI_REDUCE_NOTIFICATIONS);
		MOD_OPT_CACHE(COREUI_DIPLOMACY_ERA_INFLUENCE);

		// Core AI Changes
		MOD_OPT_CACHE(DEALAI_HUMAN_PERMANENT_FOR_AI_TEMPORARY);
		MOD_OPT_CACHE(DEALAI_NO_PEACETIME_SELLING_FOUNDED_CITIES);
		MOD_OPT_CACHE(DIPLOAI_ENABLE_NUCLEAR_GANDHI);
		MOD_OPT_CACHE(DIPLOAI_HONEST_OPINION_MODIFIERS);

		// Core Pick'N'Mix Mods
		MOD_OPT_CACHE(GLOBAL_TRULY_FREE_GP);
		MOD_OPT_CACHE(GLOBAL_CS_LIBERATE_AFTER_BUYOUT);
		MOD_OPT_CACHE(GLOBAL_NO_LOST_GREATWORKS);
		MOD_OPT_CACHE(RELIGION_KEEP_PROPHET_OVERFLOW);

		// Enabled Pick'N'Mix Mods (VP Only)
		MOD_OPT_CACHE(GLOBAL_SEPARATE_GP_COUNTERS);
		MOD_OPT_CACHE(GLOBAL_PASSABLE_FORTS);
		MOD_OPT_CACHE(GLOBAL_PARATROOPS_AA_DAMAGE);
		MOD_OPT_CACHE(GLOBAL_CS_GIFTS);
		MOD_OPT_CACHE(GLOBAL_CS_GIFT_SHIPS);
		MOD_OPT_CACHE(GLOBAL_CS_GIFTS_LOCAL_XP);
		MOD_OPT_CACHE(GLOBAL_CITY_FOREST_BONUS);
		MOD_OPT_CACHE(GLOBAL_CITY_JUNGLE_BONUS);

		// Disabled Pick'N'Mix Mods
		MOD_OPT_CACHE(BUILDINGS_NW_EXCLUDE_RAZING);
		MOD_OPT_CACHE(BUILDINGS_PRO_RATA_PURCHASE);
		MOD_OPT_CACHE(DIPLOMACY_TECH_BONUSES);
		MOD_OPT_CACHE(GLOBAL_CS_OVERSEAS_TERRITORY);
		MOD_OPT_CACHE(GLOBAL_CS_UPGRADES);
		MOD_OPT_CACHE(GLOBAL_GRATEFUL_SETTLERS);
		MOD_OPT_CACHE(GLOBAL_LOCAL_GENERALS);
		MOD_OPT_CACHE(LOCAL_GENERALS_NEAREST_CITY);
		MOD_OPT_CACHE(GLOBAL_NO_CONQUERED_SPACESHIPS);
		MOD_OPT_CACHE(GLOBAL_NO_OCEAN_PLUNDERING);
		MOD_OPT_CACHE(GLOBAL_NUKES_MELT_ICE);
		MOD_OPT_CACHE(GLOBAL_PURCHASE_FAITH_BUILDINGS_IN_PUPPETS);
		MOD_OPT_CACHE(GLOBAL_RELIGIOUS_SETTLERS);
		MOD_OPT_CACHE(RELIGIOUS_SETTLERS_WEIGHTED);
		MOD_OPT_CACHE(GLOBAL_SUBS_UNDER_ICE_IMMUNITY);
		MOD_OPT_CACHE(PROCESS_STOCKPILE);
		MOD_OPT_CACHE(RELIGION_ALLIED_INQUISITORS);
		MOD_OPT_CACHE(RELIGION_NO_PREFERENCES);
		MOD_OPT_CACHE(RELIGION_NO_PREFERRENCES); // typo version kept for backwards compatibility
		MOD_OPT_CACHE(RELIGION_RANDOMISE);
		MOD_OPT_CACHE(RELIGION_RECURRING_PURCHASE_NOTIFY);
		MOD_OPT_CACHE(UI_CITY_EXPANSION);
		MOD_OPT_CACHE(UNITS_HOVERING_LAND_ONLY_HEAL);
		MOD_OPT_CACHE(GLOBAL_STACKING_RULES); // disabled
		MOD_OPT_CACHE(GLOBAL_QUICK_ROUTES); // disabled

		// Vox Populi Balance Changes
		MOD_OPT_CACHE(BALANCE_ALTERNATE_INCA_TRAIT);
		MOD_OPT_CACHE(BALANCE_ALTERNATE_INDONESIA_TRAIT);
		MOD_OPT_CACHE(BALANCE_ALTERNATE_IROQUOIS_TRAIT);
		MOD_OPT_CACHE(BALANCE_ALTERNATE_MAYA_TRAIT);
		MOD_OPT_CACHE(BALANCE_ALTERNATE_SIAM_TRAIT);
		MOD_OPT_CACHE(BALANCE_UNIQUE_BELIEFS_ONLY_FOR_CIV);
		MOD_OPT_CACHE(BALANCE_CULTURE_VICTORY_CHANGES);
		MOD_OPT_CACHE(BALANCE_BUILDING_INVESTMENTS);
		MOD_OPT_CACHE(BALANCE_RESOURCE_MONOPOLIES);
		MOD_OPT_CACHE(BALANCE_STRATEGIC_RESOURCE_MONOPOLIES);
		MOD_OPT_CACHE(BALANCE_HEAVY_TRIBUTE);
		MOD_OPT_CACHE(BALANCE_MINOR_PROTECTION_REQUIREMENTS);
		MOD_OPT_CACHE(BALANCE_CITY_STRENGTH_SWITCH);
		MOD_OPT_CACHE(BALANCE_RIVER_CITY_CONNECTIONS);
		MOD_OPT_CACHE(BALANCE_SPY_POINTS);
		MOD_OPT_CACHE(BALANCE_AIR_UNIT_CHANGES);
		MOD_OPT_CACHE(BALANCE_RANGED_DEFENSE_UNIT_HEALTH);
		MOD_OPT_CACHE(BALANCE_LAND_UNITS_ADJACENT_BLOCKADE);
		MOD_OPT_CACHE(BALANCE_RECON_ONLY_ANCIENT_RUINS);
		MOD_OPT_CACHE(BALANCE_INQUISITOR_NERF);
		MOD_OPT_CACHE(BALANCE_RESOURCE_SHORTAGE_UNIT_HEALING);
		MOD_OPT_CACHE(BALANCE_RESOURCE_SHORTAGE_BUILDING_REFUND);
		MOD_OPT_CACHE(BALANCE_RETROACTIVE_PROMOTIONS);
		MOD_OPT_CACHE(BALANCE_PUPPET_CHANGES);
		MOD_OPT_CACHE(BALANCE_RAZING_CREATES_PARTISANS);
		MOD_OPT_CACHE(BALANCE_HALF_XP_GOLD_PURCHASES);
		MOD_OPT_CACHE(BALANCE_PURCHASED_UNIT_DAMAGE);
		MOD_OPT_CACHE(BALANCE_SETTLERS_CONSUME_POPULATION);
		MOD_OPT_CACHE(BALANCE_ENCAMPMENTS_SPAWN_ON_VISIBLE_TILES);
		MOD_OPT_CACHE(BALANCE_BARBARIAN_THEFT);
		MOD_OPT_CACHE(BALANCE_WORLD_WONDER_COST_INCREASE);
		MOD_OPT_CACHE(BALANCE_TRADE_ROUTE_PROXIMITY_PENALTY);
		MOD_OPT_CACHE(BALANCE_TRADE_ROUTE_DESTINATION_RESTRICTION);
		MOD_OPT_CACHE(BALANCE_WONDERS_VARIABLE_CONSOLATION);
		MOD_OPT_CACHE(BALANCE_FLIPPED_OPEN_BORDERS_TOURISM);
		MOD_OPT_CACHE(BALANCE_NO_WARTIME_CONCERT_TOURS);
		MOD_OPT_CACHE(BALANCE_PRISONERS_OF_WAR);
		MOD_OPT_CACHE(BALANCE_PURCHASE_COST_ADJUSTMENTS);
		MOD_OPT_CACHE(BALANCE_PERMANENT_VOTE_COMMITMENTS);
		MOD_OPT_CACHE(BALANCE_QUEST_CHANGES);
		MOD_OPT_CACHE(BALANCE_RESILIENT_PANTHEONS);

		// Other Balance Options
		MOD_OPT_CACHE(BALANCE_ALTERNATE_ASSYRIA_TRAIT);
		MOD_OPT_CACHE(BALANCE_ALTERNATE_CELTS_TRAIT);
		MOD_OPT_CACHE(BALANCE_ANY_PANTHEON);
		MOD_OPT_CACHE(BALANCE_CITY_STATE_SCALE);
		MOD_OPT_CACHE(BALANCE_CITY_STATE_TRAITS);
		MOD_OPT_CACHE(BALANCE_CITY_STATE_PERSONALITIES);
		MOD_OPT_CACHE(BALANCE_ERA_RESTRICTED_GENERALS);
		MOD_OPT_CACHE(BALANCE_ERA_RESTRICTION);
		MOD_OPT_CACHE(BALANCE_GREAT_PEOPLE_ERA_SCALING);
		MOD_OPT_CACHE(BALANCE_MINOR_UNIT_SUPPLY_HANDICAP);
		MOD_OPT_CACHE(BALANCE_NO_AUTO_SPAWN_PROPHET);
		MOD_OPT_CACHE(BALANCE_NO_CITY_RANGED_ATTACK);
		MOD_OPT_CACHE(BALANCE_NO_GAP_DURING_GA);
		MOD_OPT_CACHE(BALANCE_NO_RESOURCE_YIELDS_FROM_GP_IMPROVEMENT);
		MOD_OPT_CACHE(BALANCE_PASSIVE_SPREAD_BY_CONNECTION);
		MOD_OPT_CACHE(BALANCE_PERMANENT_PANTHEONS);
		MOD_OPT_CACHE(BALANCE_PILLAGE_PERMANENT_IMPROVEMENTS);
		MOD_OPT_CACHE(BALANCE_RANDOMIZED_GREAT_PROPHET_SPAWNS);
		MOD_OPT_CACHE(BALANCE_RELAXED_BORDER_CHECK);
		MOD_OPT_CACHE(BALANCE_SANE_UNIT_MOVEMENT_COST);
		MOD_OPT_CACHE(BALANCE_SETTLERS_RESET_GROWTH);
		MOD_OPT_CACHE(BALANCE_UNCAPPED_UNHAPPINESS);
		MOD_OPT_CACHE(BALANCE_UNIT_INVESTMENTS);
		MOD_OPT_CACHE(BALANCE_XP_ON_FIRST_ATTACK);
		MOD_OPT_CACHE(CARGO_SHIPS); // disabled
		MOD_OPT_CACHE(BALANCE_HALF_XP_FAITH_PURCHASES);

		// Other User Interface Options
		MOD_OPT_CACHE(UI_DISPLAY_PRECISE_MOVEMENT_POINTS);
		MOD_OPT_CACHE(UI_NO_RANDOM_CIV_DIALOGUE);
		MOD_OPT_CACHE(UI_QUICK_ANIMATIONS);

		// Other AI Options
		MOD_OPT_CACHE(AI_CONTROL_CITY_MANAGEMENT);
		MOD_OPT_CACHE(AI_CONTROL_CITY_PRODUCTION);
		MOD_OPT_CACHE(AI_CONTROL_DIPLOMACY);
		MOD_OPT_CACHE(AI_CONTROL_ECONOMY);
		MOD_OPT_CACHE(AI_CONTROL_ESPIONAGE);
		MOD_OPT_CACHE(AI_CONTROL_EVENT_CHOICE);
		MOD_OPT_CACHE(AI_CONTROL_FAITH_SPENDING);
		MOD_OPT_CACHE(AI_CONTROL_GREAT_PERSON_CHOICE);
		MOD_OPT_CACHE(AI_CONTROL_POLICY_CHOICE);
		MOD_OPT_CACHE(AI_CONTROL_RELIGION_CHOICE);
		MOD_OPT_CACHE(AI_CONTROL_TECH_CHOICE);
		MOD_OPT_CACHE(AI_CONTROL_TOURISM);
		MOD_OPT_CACHE(AI_CONTROL_UNIT_PROMOTIONS);
		MOD_OPT_CACHE(AI_CONTROL_UNITS);
		MOD_OPT_CACHE(AI_CONTROL_WORLD_CONGRESS);
		MOD_OPT_CACHE(HUMAN_USES_AI_HANDICAP);
		MOD_OPT_CACHE(HUMAN_USES_AI_MECHANICS);
		MOD_OPT_CACHE(COMBATAI_TWO_PASS_DANGER);
		MOD_OPT_CACHE(DEALAI_DISABLE_CITY_TRADES);
		MOD_OPT_CACHE(DEALAI_DISABLE_WAR_BRIBES);
		MOD_OPT_CACHE(DEALAI_GLOBAL_PERMANENT_FOR_TEMPORARY);
		MOD_OPT_CACHE(DIPLOAI_AGGRESSIVE_MODE);
		MOD_OPT_CACHE(DIPLOAI_DISABLE_DOMINATION_ONLY_AGGRESSION);
		MOD_OPT_CACHE(DIPLOAI_HIDE_OPINION_TABLE);
		MOD_OPT_CACHE(DIPLOAI_LIMIT_VICTORY_PURSUIT_RANDOMIZATION);
		MOD_OPT_CACHE(DIPLOAI_NO_FLAVOR_RANDOMIZATION);
		MOD_OPT_CACHE(DIPLOAI_PASSIVE_MODE);
		MOD_OPT_CACHE(DIPLOAI_SHOW_ALL_OPINION_VALUES);
		MOD_OPT_CACHE(DIPLOAI_SHOW_BASE_HUMAN_OPINION);
		MOD_OPT_CACHE(DIPLOAI_SHOW_HIDDEN_OPINION_MODIFIERS);
		MOD_OPT_CACHE(DIPLOAI_SHUT_UP);
		MOD_OPT_CACHE(DIPLOAI_SHUT_UP_COMPLIMENTS);
		MOD_OPT_CACHE(DIPLOAI_SHUT_UP_COOP_WAR_OFFERS);
		MOD_OPT_CACHE(DIPLOAI_SHUT_UP_DEMANDS);
		MOD_OPT_CACHE(DIPLOAI_SHUT_UP_FRIENDSHIP_OFFERS);
		MOD_OPT_CACHE(DIPLOAI_SHUT_UP_GIFT_OFFERS);
		MOD_OPT_CACHE(DIPLOAI_SHUT_UP_HELP_REQUESTS);
		MOD_OPT_CACHE(DIPLOAI_SHUT_UP_INDEPENDENCE_REQUESTS);
		MOD_OPT_CACHE(DIPLOAI_SHUT_UP_INSULTS);
		MOD_OPT_CACHE(DIPLOAI_SHUT_UP_PEACE_OFFERS);
		MOD_OPT_CACHE(DIPLOAI_SHUT_UP_TRADE_OFFERS);
		MOD_OPT_CACHE(DIPLOAI_SHUT_UP_TRADE_RENEWALS);
		MOD_OPT_CACHE(LEAGUEAI_NO_OTHER_HOST_VOTES);
		MOD_OPT_CACHE(LEAGUEAI_NO_OTHER_WORLD_LEADER_VOTES);

		// API Extensions
		MOD_OPT_CACHE(API_AREA_EFFECT_PROMOTIONS);
		MOD_OPT_CACHE(API_DISABLE_LUA_HOOKS);
		MOD_OPT_CACHE(API_HOVERING_UNITS);
		MOD_OPT_CACHE(BUILDINGS_THOROUGH_PREREQUISITES);
		MOD_OPT_CACHE(IMPROVEMENTS_EXTENSIONS);
		MOD_OPT_CACHE(LOG_MAP_STATE);
		MOD_OPT_CACHE(PLOTS_EXTENSIONS);
		MOD_OPT_CACHE(PROMOTIONS_FLAGSHIP);
		MOD_OPT_CACHE(PROMOTIONS_IMPROVEMENT_BONUS);
		MOD_OPT_CACHE(RELIGION_EXTENSIONS);
		MOD_OPT_CACHE(RELIGION_LOCAL_RELIGIONS);
		MOD_OPT_CACHE(RESOURCES_PRODUCTION_COST_MODIFIERS);
		MOD_OPT_CACHE(TRADE_INTERNAL_GOLD_ROUTES);
		MOD_OPT_CACHE(TRADE_WONDER_RESOURCE_ROUTES);
		MOD_OPT_CACHE(TRAITS_TRADE_ROUTE_PRODUCTION_SIPHON);
		MOD_OPT_CACHE(TRAITS_YIELD_FROM_ROUTE_MOVEMENT_IN_FOREIGN_TERRITORY);
		MOD_OPT_CACHE(UNIT_KILL_STATS);
		MOD_OPT_CACHE(UNITS_RESOURCE_QUANTITY_TOTALS);
		MOD_OPT_CACHE(WH_MILITARY_LOG);
		MOD_OPT_CACHE(YIELD_MODIFIER_FROM_UNITS);

		// Events
		MOD_OPT_CACHE(EVENTS_ACQUIRE_BELIEFS);
		MOD_OPT_CACHE(EVENTS_AI_OVERRIDE_TECH);
		MOD_OPT_CACHE(EVENTS_AIRLIFT);
		MOD_OPT_CACHE(EVENTS_AREA_RESOURCES);
		MOD_OPT_CACHE(EVENTS_BARBARIANS);
		MOD_OPT_CACHE(EVENTS_BATTLES);
		MOD_OPT_CACHE(EVENTS_BATTLES_DAMAGE);
		MOD_OPT_CACHE(EVENTS_CAN_MOVE_INTO);
		MOD_OPT_CACHE(EVENTS_CIRCUMNAVIGATION);
		MOD_OPT_CACHE(EVENTS_CITY);
		MOD_OPT_CACHE(EVENTS_CITY_AIRLIFT);
		MOD_OPT_CACHE(EVENTS_CITY_BOMBARD);
		MOD_OPT_CACHE(EVENTS_CITY_BORDERS);
		MOD_OPT_CACHE(EVENTS_CITY_CAPITAL);
		MOD_OPT_CACHE(EVENTS_CITY_CONNECTIONS);
		MOD_OPT_CACHE(EVENTS_CITY_FOUNDING);
		MOD_OPT_CACHE(EVENTS_CITY_RAZING);
		MOD_OPT_CACHE(EVENTS_COMMAND);
		MOD_OPT_CACHE(EVENTS_CUSTOM_MISSIONS);
		MOD_OPT_CACHE(EVENTS_DIPLO_EVENTS);
		MOD_OPT_CACHE(EVENTS_DIPLO_MODIFIERS);
		MOD_OPT_CACHE(EVENTS_ESPIONAGE);
		MOD_OPT_CACHE(EVENTS_FOUND_RELIGION);
		MOD_OPT_CACHE(EVENTS_GAME_SAVE);
		MOD_OPT_CACHE(EVENTS_GOLDEN_AGE);
		MOD_OPT_CACHE(EVENTS_GOODY_CHOICE);
		MOD_OPT_CACHE(EVENTS_GOODY_TECH);
		MOD_OPT_CACHE(EVENTS_GREAT_PEOPLE);
		MOD_OPT_CACHE(EVENTS_IDEOLOGIES);
		MOD_OPT_CACHE(EVENTS_LIBERATION);
		MOD_OPT_CACHE(EVENTS_MINORS);
		MOD_OPT_CACHE(EVENTS_MINORS_GIFTS);
		MOD_OPT_CACHE(EVENTS_MINORS_INTERACTION);
		MOD_OPT_CACHE(EVENTS_NEW_ERA);
		MOD_OPT_CACHE(EVENTS_NUCLEAR_DETONATION);
		MOD_OPT_CACHE(EVENTS_NW_DISCOVERY);
		MOD_OPT_CACHE(EVENTS_PARADROPS);
		MOD_OPT_CACHE(EVENTS_PLAYER_TURN);
		MOD_OPT_CACHE(EVENTS_PLOT);
		MOD_OPT_CACHE(EVENTS_REBASE);
		MOD_OPT_CACHE(EVENTS_RED_TURN);
		MOD_OPT_CACHE(EVENTS_RED_COMBAT);
		MOD_OPT_CACHE(EVENTS_RED_COMBAT_MISSION);
		MOD_OPT_CACHE(EVENTS_RED_COMBAT_ABORT);
		MOD_OPT_CACHE(EVENTS_RED_COMBAT_RESULT);
		MOD_OPT_CACHE(EVENTS_RED_COMBAT_ENDED);
		MOD_OPT_CACHE(EVENTS_RELIGION);
		MOD_OPT_CACHE(EVENTS_RESOLUTIONS);
		MOD_OPT_CACHE(EVENTS_TERRAFORMING);
		MOD_OPT_CACHE(EVENTS_TILE_IMPROVEMENTS);
		MOD_OPT_CACHE(EVENTS_TILE_REVEALED);
		MOD_OPT_CACHE(EVENTS_TRADE_ROUTE_PLUNDERED);
		MOD_OPT_CACHE(EVENTS_TRADE_ROUTES);
		MOD_OPT_CACHE(EVENTS_UNIT_ACTIONS);
		MOD_OPT_CACHE(EVENTS_UNIT_CAPTURE);
		MOD_OPT_CACHE(EVENTS_UNIT_CONVERTS);
		MOD_OPT_CACHE(EVENTS_UNIT_CREATED);
		MOD_OPT_CACHE(EVENTS_UNIT_DATA);
		MOD_OPT_CACHE(EVENTS_UNIT_FOUNDED);
		MOD_OPT_CACHE(EVENTS_UNIT_PREKILL);
		MOD_OPT_CACHE(EVENTS_UNIT_RANGEATTACK);
		MOD_OPT_CACHE(EVENTS_UNIT_UPGRADES);
		MOD_OPT_CACHE(EVENTS_WAR_AND_PEACE);

		// Slated for Deletion
		MOD_OPT_CACHE(AI_UNIT_PRODUCTION);
		MOD_OPT_CACHE(BALANCE_BOMBARD_RANGE_BUILDINGS);
		MOD_OPT_CACHE(BALANCE_NEW_GREAT_PERSON_ATTRIBUTES);
		MOD_OPT_CACHE(LINKED_MOVEMENT);
		MOD_OPT_CACHE(PROMOTIONS_DEEP_WATER_EMBARKATION);

		// Modmodders' Options
		MOD_OPT_CACHE(ALTERNATIVE_DIFFICULTY);
		MOD_OPT_CACHE(ATTRITION);
		MOD_OPT_CACHE(CIV6_EUREKAS);
		MOD_OPT_CACHE(CIV6_ROADS);
		MOD_OPT_CACHE(CIV6_WORKER);
		MOD_OPT_CACHE(ISKA_HERITAGE);
		MOD_OPT_CACHE(ISKA_PANTHEONS);
		MOD_OPT_CACHE(ISKA_GOLDENAGEPOINTS_TO_PRESTIGE);
		MOD_OPT_CACHE(NOT_FOR_SALE);
		MOD_OPT_CACHE(ROUTE_PLANNER);
		MOD_OPT_CACHE(BALANCE_CORE_JFD);

		m_bInit = true;
	}

	if (m_options.find(sOption) == m_options.end())
		return defValue;

	return m_options[sOption];
}

int CustomMods::getCivOption(const char* szCiv, const char* szName, int defValue)
{
	return getOption(string(szCiv) + "_" + szName, getOption(szName, defValue));
}

void CheckSentinel(uint value)
{
    if (value == 0xDEADBEEF)
        return; //everything ok

    CUSTOMLOG("Deserialization Error\n");

#if defined(VPDEBUG)
    char debugMsg[256];
    sprintf(debugMsg, "Sentinel value mismatch: Expected 0xDEADBEEF, Got 0x%08X\n", value);
    OutputDebugString(debugMsg);
#endif
}
