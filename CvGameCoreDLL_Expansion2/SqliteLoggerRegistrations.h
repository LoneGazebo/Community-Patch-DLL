/*
	-------------------------------------------------------------------------------------------------------
	SqliteLoggerRegistrations - shared table registration helpers for stats.db tables.

	Keep SQLite table registration helpers in this header so any gameplay file that needs to write to an
	existing table can include one import and reuse the same one-time registration logic. This avoids
	duplicating table schemas across translation units and makes multi-file logging changes safer.
	------------------------------------------------------------------------------------------------------- */

#pragma once

#include "SqliteLogger.h"

inline void RegisterTechChoicesTable()
{
	if (!MOD_SQLITE_LOGGING)
		return;

	static bool bRegistered = false;
	if (!bRegistered)
	{
		bRegistered = true;
		TableDef kColumns;
		kColumns.push_back(ColumnDef("Civ", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Technology", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Action", Database::COLTYPE_TEXT));
		GET_SQLITE_LOGGER().RegisterTable("TechChoices", kColumns);
	}
}

inline void RegisterReligionChoicesTable()
{
	if (!MOD_SQLITE_LOGGING)
		return;

	static bool bRegistered = false;
	if (!bRegistered)
	{
		bRegistered = true;
		TableDef kColumns;
		kColumns.push_back(ColumnDef("Civ", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Action", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Belief", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("BeliefType", Database::COLTYPE_TEXT));
		GET_SQLITE_LOGGER().RegisterTable("ReligionChoices", kColumns);
	}
}

inline void RegisterPolicyChoicesTable()
{
	if (!MOD_SQLITE_LOGGING)
		return;

	static bool bRegistered = false;
	if (!bRegistered)
	{
		bRegistered = true;
		TableDef kColumns;
		kColumns.push_back(ColumnDef("Civ", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Era", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("Branch", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Policy", Database::COLTYPE_TEXT));
		GET_SQLITE_LOGGER().RegisterTable("PolicyChoices", kColumns);
	}
}

inline void RegisterMilitarySummaryTable()
{
	if (!MOD_SQLITE_LOGGING)
		return;

	static bool bRegistered = false;
	if (!bRegistered)
	{
		bRegistered = true;
		TableDef kColumns;
		kColumns.push_back(ColumnDef("Civ", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Era", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("Cities", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("Settlers", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("LandUnits", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("LandArmySize", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("RecLandOffensive", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("NavalUnits", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("NavalArmySize", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("RecNavyOffensive", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("MeleeUnits", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("MobileUnits", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("ReconUnits", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("ArcherUnits", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("SiegeUnits", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("SkirmisherUnits", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("AllLandRanged", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("AntiAirUnits", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("MeleeNavalUnits", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("RangedNavalUnits", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("Submarines", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("Carriers", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("TotalAirUnits", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("BomberUnits", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("FighterUnits", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("Nukes", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("Missiles", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("RecTotal", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("TotalMilitaryUnits", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("SupplyLimit", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("OutOfSupply", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("WarWearinessSupplyReduction", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("NoSupplyUnits", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("WarCount", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("MostEndangeredCity", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Danger", Database::COLTYPE_INT));
		GET_SQLITE_LOGGER().RegisterTable("MilitarySummary", kColumns);
	}
}

inline void RegisterWorldStateLogTable()
{
	if (!MOD_SQLITE_LOGGING)
		return;

	static bool bRegistered = false;
	if (!bRegistered)
	{
		bRegistered = true;
		TableDef kColumns;
		kColumns.push_back(ColumnDef("GsConquest", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("GsSpaceship", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("GsDiplo", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("GsCulture", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("OpinionAlly", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("OpinionFriend", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("OpinionFavorable", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("OpinionNeutral", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("OpinionCompetitor", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("OpinionEnemy", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("OpinionUnforgivable", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("ApproachWar", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("ApproachHostile", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("ApproachDeceptive", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("ApproachGuarded", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("ApproachAfraid", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("ApproachFriendly", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("ApproachNeutral", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("MinorIgnore", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("MinorProtective", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("MinorConquest", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("MinorBully", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("BasicNeedsMedian", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("GoldMedian", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("ScienceMedian", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("CultureMedian", Database::COLTYPE_INT));
		GET_SQLITE_LOGGER().RegisterTable("WorldStateLog", kColumns);
	}
}

inline void RegisterGameResultTable()
{
	if (!MOD_SQLITE_LOGGING)
		return;

	static bool bRegistered = false;
	if (!bRegistered)
	{
		bRegistered = true;
		TableDef kColumns;
		kColumns.push_back(ColumnDef("Civ", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Score", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("VictoryType", Database::COLTYPE_TEXT));
		GET_SQLITE_LOGGER().RegisterTable("GameResult", kColumns);
	}
}

// Per-turn snapshot of which era each civ is in, one row per civ per turn.
inline void RegisterCivTurnEraTable()
{
	if (!MOD_SQLITE_LOGGING)
		return;

	static bool bRegistered = false;
	if (!bRegistered)
	{
		bRegistered = true;
		TableDef kColumns;
		kColumns.push_back(ColumnDef("civ", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("era", Database::COLTYPE_INT));
		GET_SQLITE_LOGGER().RegisterTable("civ_turn_era", kColumns);
	}
}

inline void RegisterMapPlotsStateTable()
{
	if (!MOD_SQLITE_LOGGING)
		return;

	static bool bRegistered = false;
	if (!bRegistered)
	{
		bRegistered = true;
		TableDef kColumns;
		kColumns.push_back(ColumnDef("plotX", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("plotY", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("cityName", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("owner", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("routeType", Database::COLTYPE_INT));
		GET_SQLITE_LOGGER().RegisterTable("MapPlotsState", kColumns);
	}
}

// Per-turn snapshot of every unit owned by each major civ, one row per unit.
inline void RegisterMapUnitsStateTable()
{
	if (!MOD_SQLITE_LOGGING)
		return;

	static bool bRegistered = false;
	if (!bRegistered)
	{
		bRegistered = true;
		TableDef kColumns;
		kColumns.push_back(ColumnDef("owner", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("plotX", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("plotY", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("unitID", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("unitName", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("unitMaxHP", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("unitCurrHP", Database::COLTYPE_INT));
		GET_SQLITE_LOGGER().RegisterTable("MapUnitsState", kColumns);
	}
}

// Long-format per-building yield breakdown, one row per (city, constructed building, yield).
// BaseYieldTimes100   = the building's own intrinsic flat yield (incl. population/era/per-tile and
//                       similar self-scalers) PLUS the extra yields the building grants to the tiles
//                       this city is actually working (resource/luxury/terrain/feature/improvement/
//                       sea/lake/river/city-connection plot yields).
// BonusYieldTimes100  = the extra flat yield layered on top of the base by external sources such as
//                       beliefs, policies, traits, corporations, leagues, events and other buildings.
// Percentage-based modifiers and instant/event yields are intentionally excluded (tracked elsewhere).
inline void RegisterBuildingYieldsTable()
{
	if (!MOD_SQLITE_LOGGING)
		return;

	static bool bRegistered = false;
	if (!bRegistered)
	{
		bRegistered = true;
		TableDef kColumns;
		kColumns.push_back(ColumnDef("Era", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("Civ", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("City", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Building", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Yield", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("BaseYieldTimes100", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("BonusYieldTimes100", Database::COLTYPE_INT));
		GET_SQLITE_LOGGER().RegisterTable("BuildingYields", kColumns);
	}
}

// Long-format per-belief yield breakdown, one row per (civ, belief, source, yield) per turn.
inline void RegisterReligionBeliefYieldsTable()
{
	if (!MOD_SQLITE_LOGGING)
		return;

	static bool bRegistered = false;
	if (!bRegistered)
	{
		bRegistered = true;
		TableDef kColumns;
		kColumns.push_back(ColumnDef("Era", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("Civ", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Belief", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("BeliefType", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("IsReligionOwner", Database::COLTYPE_BOOL));
		kColumns.push_back(ColumnDef("Source", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Yield", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("YieldTimes100", Database::COLTYPE_INT));
		GET_SQLITE_LOGGER().RegisterTable("ReligionBeliefYields", kColumns);
	}
}

// Per-turn overview of each civ's controlled building inventory (one row per building type).
inline void RegisterBuildingsOverviewTable()
{
	if (!MOD_SQLITE_LOGGING)
		return;

	static bool bRegistered = false;
	if (!bRegistered)
	{
		bRegistered = true;
		TableDef kColumns;
		kColumns.push_back(ColumnDef("Era", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("Civ", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Building", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Count", Database::COLTYPE_INT));
		GET_SQLITE_LOGGER().RegisterTable("BuildingsOverview", kColumns);
	}
}

// Long-format instant-yield breakdown, one row per (building, city, yield) at the moment an
// instant yield fires (citizen born, building constructed, policy unlocked, tile pillaged, etc.).
// EventType is a short label derived from the instant-yield source table (Birth, PolicyUnlock,
// Pillage, ...). YieldTimes100 is the building's attributed share of the granted yield, multiplied
// by 100 so that fractional per-building contributions are preserved.
inline void RegisterBuildingInstantYieldsTable()
{
	if (!MOD_SQLITE_LOGGING)
		return;

	static bool bRegistered = false;
	if (!bRegistered)
	{
		bRegistered = true;
		TableDef kColumns;
		kColumns.push_back(ColumnDef("Era", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("Civ", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("City", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Building", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("EventType", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Yield", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("YieldTimes100", Database::COLTYPE_INT));
		GET_SQLITE_LOGGER().RegisterTable("BuildingInstantYields", kColumns);
	}
}

// Long-format difficulty-bonus yield breakdown, one row per (civ, trigger, yield) each time an AI player receives an AI difficulty bonus
inline void RegisterHandicapYieldsTable()
{
	if (!MOD_SQLITE_LOGGING)
		return;

	static bool bRegistered = false;
	if (!bRegistered)
	{
		bRegistered = true;
		TableDef kColumns;
		kColumns.push_back(ColumnDef("Civ", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Era", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("CityCount", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("Trigger", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Yield", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("YieldTimes100", Database::COLTYPE_INT));
		GET_SQLITE_LOGGER().RegisterTable("HandicapYields", kColumns);
	}
}

// Long-format instant-yield breakdown, one row per (civ, instant-yield type, yield) at the moment an instant yield fires.
inline void RegisterInstantYieldsTable()
{
	if (!MOD_SQLITE_LOGGING)
		return;

	static bool bRegistered = false;
	if (!bRegistered)
	{
		bRegistered = true;
		TableDef kColumns;
		kColumns.push_back(ColumnDef("Civ", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Era", Database::COLTYPE_INT));
		kColumns.push_back(ColumnDef("Type", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Yield", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("YieldTimes100", Database::COLTYPE_INT));
		GET_SQLITE_LOGGER().RegisterTable("InstantYields", kColumns);
	}
}
