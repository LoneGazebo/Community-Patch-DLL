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

// -------------------------------------------------------------------------------------------------
// DiplomacyLog: one row per diplomatic event between two civs, written at the moment the event
// happens (an event log, not a per-turn snapshot). Civ1 is the acting civ and Civ2 the civ it acts
// on, so a row reads "Civ1 <Action> Civ2": "Rome DECLARED_WAR Greece", "Rome BECAME_VASSAL Greece".
// Team-level events (war, peace, vassalage, pacts, embassies, open borders, first contact) produce
// one row per (member of team 1, member of team 2) pair; with the usual one-civ teams that is exactly
// one row per event. Wars with the barbarians are never logged.
//
//   Civ1 / Civ2   short civ names, the same strings the other stats.db tables use for "Civ"
//   Action        uppercase event tag, see the list below
//   Detail        qualifier for the Action ("" when there is none)
//   ThirdCiv      a third civ involved in the event ("" when there is none): the coop-war target, the
//                 pact partner / master / ally that dragged Civ1 into a war, the originator of a peace
//                 that Civ1 followed, or the ally a City-State replaced / was replaced by
//   Civ1Minor /   true when that civ is a City-State; filter on these for major-civ-only analyses
//   Civ2Minor
//
// Actions (Civ1 -> Civ2) and their Detail values:
//   MET                     first contact
//   DECLARED_WAR            AGGRESSION | BACKSTAB (Civ1 had a Declaration of Friendship with Civ2) |
//                           DEFENSIVE_PACT (ThirdCiv = pact partner being defended) |
//                           VASSAL_OBLIGATION (ThirdCiv = master whose war Civ1 joined) |
//                           MINOR_ALLY (City-State Civ1 joining its ally's war; ThirdCiv = the ally)
//   MADE_PEACE              TREATY | FOLLOWED_ALLY (vassal or City-State ally following a peace made
//                           above it; ThirdCiv = who made the original peace)
//   DECLARED_FRIENDSHIP     FRIENDS (first DoF between these two civs)
//   RENEWED_FRIENDSHIP      ALLIES | BATTLE_BROTHERS (a new DoF between civs that were friends before;
//                           the level counts how many DoFs they have had)
//   ENDED_FRIENDSHIP        EXPIRED (ran its full duration) | BROKEN (denounce, war, or explicitly ended)
//   DENOUNCED               "" | BACKSTAB (Civ1 denounced a civ it had a DoF with)
//   DENOUNCEMENT_EXPIRED
//   BECAME_VASSAL           VOLUNTARY | CAPITULATED                    (Civ1 = new vassal, Civ2 = master)
//   ENDED_VASSALAGE         PEACEFUL | REBELLION (independence by war)  (Civ1 = ex-vassal, Civ2 = ex-master)
//   LIBERATED_VASSAL        master Civ1 granted vassal Civ2 its independence
//   DEFENSIVE_PACT / ENDED_DEFENSIVE_PACT
//   RESEARCH_AGREEMENT / ENDED_RESEARCH_AGREEMENT    Detail on end: "" (expired) | CANCELLED (war etc.)
//   OPENED_BORDERS / CLOSED_BORDERS                  Civ1 lets Civ2's units into its territory
//   OPENED_EMBASSY / CLOSED_EMBASSY                  Civ1 has an embassy with Civ2
//   COOP_WAR                AGREED | STARTED | CANCELLED | REJECTED | WARNED_TARGET; ThirdCiv = target
//   MINOR_ALLY_GAINED / MINOR_ALLY_LOST              major Civ1 became / stopped being City-State Civ2's ally
//   PLEDGED_PROTECTION / REVOKED_PROTECTION          Detail on revoke: "" | PLEDGE_BROKEN
//   BULLIED_MINOR           GOLD | UNIT | ANNEX (successful tribute demands only)
//   LIBERATED_CIV           RESURRECTION (Civ2 was dead) | CITY (a city returned to living Civ2) | SPHERE_REMOVAL
inline void RegisterDiplomacyLogTable()
{
	if (!MOD_SQLITE_LOGGING)
		return;

	static bool bRegistered = false;
	if (!bRegistered)
	{
		bRegistered = true;
		TableDef kColumns;
		kColumns.push_back(ColumnDef("Civ1", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Civ2", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Action", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Detail", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("ThirdCiv", Database::COLTYPE_TEXT));
		kColumns.push_back(ColumnDef("Civ1Minor", Database::COLTYPE_BOOL));
		kColumns.push_back(ColumnDef("Civ2Minor", Database::COLTYPE_BOOL));
		GET_SQLITE_LOGGER().RegisterTable("DiplomacyLog", kColumns);
	}
}

// Writes one DiplomacyLog row for "eCiv1 <szAction> eCiv2". Safe to call unconditionally: it is a
// no-op when SQLite logging is disabled, and rows involving an invalid player, the barbarians, an
// observer slot (which "meets" everyone), or a civ acting on itself are skipped. Relies on the
// precompiled header for GET_PLAYER, like the rest of this header relies on it for MOD_SQLITE_LOGGING.
inline void LogDiplomacyEvent(PlayerTypes eCiv1, PlayerTypes eCiv2, const char* szAction, const char* szDetail = "", PlayerTypes eThirdCiv = NO_PLAYER)
{
	if (!MOD_SQLITE_LOGGING)
		return;

	if (eCiv1 <= NO_PLAYER || eCiv1 >= MAX_CIV_PLAYERS || eCiv2 <= NO_PLAYER || eCiv2 >= MAX_CIV_PLAYERS || eCiv1 == eCiv2)
		return;

	const CvPlayer& kCiv1 = GET_PLAYER(eCiv1);
	const CvPlayer& kCiv2 = GET_PLAYER(eCiv2);
	if (kCiv1.isBarbarian() || kCiv2.isBarbarian() || kCiv1.isObserver() || kCiv2.isObserver())
		return;

	const char* szThirdCiv = "";
	if (eThirdCiv > NO_PLAYER && eThirdCiv < MAX_CIV_PLAYERS && eThirdCiv != eCiv1 && eThirdCiv != eCiv2)
		szThirdCiv = GET_PLAYER(eThirdCiv).getCivilizationShortDescription();

	RegisterDiplomacyLogTable();
	GET_SQLITE_LOGGER().BeginLogRow("DiplomacyLog")
		.bind(kCiv1.getCivilizationShortDescription())   // Civ1
		.bind(kCiv2.getCivilizationShortDescription())   // Civ2
		.bind(szAction)                                  // Action
		.bind(szDetail != NULL ? szDetail : "")          // Detail
		.bind(szThirdCiv)                                // ThirdCiv
		.bind(kCiv1.isMinorCiv())                        // Civ1Minor
		.bind(kCiv2.isMinorCiv())                        // Civ2Minor
		.execute();
}

// Team-level variant of LogDiplomacyEvent: one row for every pair of living members, one from each
// team, with eTeam1's member as Civ1. Used for events the game tracks per team (war, peace, vassalage,
// pacts, embassies, open borders, first contact).
inline void LogDiplomacyEventTeams(TeamTypes eTeam1, TeamTypes eTeam2, const char* szAction, const char* szDetail = "", PlayerTypes eThirdCiv = NO_PLAYER)
{
	if (!MOD_SQLITE_LOGGING)
		return;

	if (eTeam1 <= NO_TEAM || eTeam1 >= MAX_TEAMS || eTeam2 <= NO_TEAM || eTeam2 >= MAX_TEAMS || eTeam1 == eTeam2)
		return;

	const std::vector<PlayerTypes>& vTeam1 = GET_TEAM(eTeam1).getPlayers();
	const std::vector<PlayerTypes>& vTeam2 = GET_TEAM(eTeam2).getPlayers();
	for (size_t i = 0; i < vTeam1.size(); i++)
	{
		if (!GET_PLAYER(vTeam1[i]).isAlive())
			continue;

		for (size_t j = 0; j < vTeam2.size(); j++)
		{
			if (!GET_PLAYER(vTeam2[j]).isAlive())
				continue;

			LogDiplomacyEvent(vTeam1[i], vTeam2[j], szAction, szDetail, eThirdCiv);
		}
	}
}
