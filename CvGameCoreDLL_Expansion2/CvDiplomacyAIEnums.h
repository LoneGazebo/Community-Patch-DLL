/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef DIPLOMACY_AI_ENUMS_H
#define DIPLOMACY_AI_ENUMS_H

// WARNING: The order of many of these values is very important (many places in the code use > or < ENUM_VALUE to make important determinations.) Do not change unless you know what you're doing!

enum CLOSED_ENUM CivOpinionTypes
{
	NO_CIV_OPINION = -1,

	CIV_OPINION_UNFORGIVABLE,
	CIV_OPINION_ENEMY,
	CIV_OPINION_COMPETITOR,
	CIV_OPINION_NEUTRAL,
	CIV_OPINION_FAVORABLE,
	CIV_OPINION_FRIEND,
	CIV_OPINION_ALLY,

	NUM_CIV_OPINIONS ENUM_META_VALUE,
};

enum CLOSED_ENUM CivApproachTypes
{
	NO_CIV_APPROACH = -1,

	CIV_APPROACH_WAR,
	CIV_APPROACH_HOSTILE,
	CIV_APPROACH_DECEPTIVE,
	CIV_APPROACH_GUARDED,
	CIV_APPROACH_AFRAID,
	CIV_APPROACH_NEUTRAL,
	CIV_APPROACH_FRIENDLY,

	NUM_CIV_APPROACHES ENUM_META_VALUE,
};

enum CLOSED_ENUM WarStateTypes
{
	NO_WAR_STATE_TYPE = -1,

	WAR_STATE_NEARLY_DEFEATED,
	WAR_STATE_DEFENSIVE,
	WAR_STATE_TROUBLED,
	WAR_STATE_STALEMATE,
	WAR_STATE_CALM,
	WAR_STATE_OFFENSIVE,
	WAR_STATE_NEARLY_WON,

	NUM_WAR_STATES ENUM_META_VALUE,
};

enum CLOSED_ENUM StrengthTypes
{
	NO_STRENGTH_VALUE = -1,

	STRENGTH_PATHETIC,
	STRENGTH_WEAK,
	STRENGTH_POOR,
	STRENGTH_AVERAGE,
	STRENGTH_STRONG,
	STRENGTH_POWERFUL,
	STRENGTH_IMMENSE,

	NUM_STRENGTH_VALUES ENUM_META_VALUE,
};

enum CLOSED_ENUM TargetValueTypes
{
	NO_TARGET_VALUE = -1,

	TARGET_VALUE_IMPOSSIBLE,
	TARGET_VALUE_BAD,
	TARGET_VALUE_AVERAGE,
	TARGET_VALUE_FAVORABLE,
	TARGET_VALUE_SOFT,

	NUM_TARGET_VALUES ENUM_META_VALUE,
};

enum CLOSED_ENUM AggressivePostureTypes
{
	NO_AGGRESSIVE_POSTURE_TYPE = -1,

	AGGRESSIVE_POSTURE_NONE,
	AGGRESSIVE_POSTURE_LOW,
	AGGRESSIVE_POSTURE_MEDIUM,
	AGGRESSIVE_POSTURE_HIGH,
	AGGRESSIVE_POSTURE_INCREDIBLE,

	NUM_AGGRESSIVE_POSTURE_TYPES ENUM_META_VALUE,
};

enum CLOSED_ENUM ThreatTypes
{
	NO_THREAT_VALUE = -1,

	THREAT_NONE,
	THREAT_MINOR,
	THREAT_MAJOR,
	THREAT_SEVERE,
	THREAT_CRITICAL,

	NUM_THREAT_VALUES ENUM_META_VALUE,
};

enum CLOSED_ENUM DisputeLevelTypes
{
	NO_DISPUTE_LEVEL = -1,

	DISPUTE_LEVEL_NONE,
	DISPUTE_LEVEL_WEAK,
	DISPUTE_LEVEL_STRONG,
	DISPUTE_LEVEL_FIERCE,

	NUM_DISPUTE_LEVELS ENUM_META_VALUE,
};

enum CLOSED_ENUM BlockLevelTypes
{
	NO_BLOCK_LEVEL = -1,

	BLOCK_LEVEL_NONE,
	BLOCK_LEVEL_WEAK,
	BLOCK_LEVEL_STRONG,
	BLOCK_LEVEL_FIERCE,

	NUM_BLOCK_LEVELS ENUM_META_VALUE,
};

enum CLOSED_ENUM DoFLevelTypes
{
	NO_DOF_TYPE = -1,

	DOF_TYPE_UNTRUSTWORTHY,
	DOF_TYPE_NEW,
	DOF_TYPE_FRIENDS,
	DOF_TYPE_ALLIES,
	DOF_TYPE_BATTLE_BROTHERS,

	NUM_DOF_TYPES ENUM_META_VALUE,
};

enum CLOSED_ENUM VictoryPursuitTypes
{
	NO_VICTORY_PURSUIT = -1,

	VICTORY_PURSUIT_DOMINATION,
	VICTORY_PURSUIT_DIPLOMACY,
	VICTORY_PURSUIT_CULTURE,
	VICTORY_PURSUIT_SCIENCE,

	NUM_VICTORY_PURSUITS ENUM_META_VALUE,
};

enum CLOSED_ENUM PromiseStates
{
	NO_PROMISE_STATE = -1,

	PROMISE_STATE_MADE,
	PROMISE_STATE_IGNORED,
	PROMISE_STATE_BROKEN,

	NUM_PROMISE_STATES ENUM_META_VALUE,
};

enum CLOSED_ENUM ModifierTypes
{
	NO_MODIFIER_TYPE = -1,

	MODIFIER_TYPE_NORMAL,
	MODIFIER_TYPE_STACKED,
	MODIFIER_TYPE_DIMINISHING,

	NUM_MODIFIER_TYPES ENUM_META_VALUE,
};

enum CLOSED_ENUM TheftTypes
{
	NO_THEFT_TYPE = -1,

	THEFT_TYPE_CULTURE_BOMB,
	THEFT_TYPE_CONVERSION,
	THEFT_TYPE_ARTIFACT,
	THEFT_TYPE_SPY,
	THEFT_TYPE_TRADE_ROUTE,
	THEFT_TYPE_PLOT,

	NUM_THEFT_TYPES ENUM_META_VALUE,
};

enum CLOSED_ENUM GuessConfidenceTypes
{
	NO_GUESS_CONFIDENCE_TYPE = -1,

	GUESS_CONFIDENCE_UNSURE,
	GUESS_CONFIDENCE_LIKELY,
	GUESS_CONFIDENCE_POSITIVE,

	NUM_GUESS_CONFIDENCE_TYPES ENUM_META_VALUE,
};

enum CLOSED_ENUM StateAllWars
{
	STATE_ALL_WARS_NEUTRAL = 0,
	STATE_ALL_WARS_WINNING,
	STATE_ALL_WARS_LOSING,

	NUM_STATES_ALL_WARS ENUM_META_VALUE,
};

enum CLOSED_ENUM PeaceBlockReasons
{
	NO_PEACE_BLOCK_REASON = -1,

	PEACE_BLOCK_REASON_ALWAYS_WAR,
	PEACE_BLOCK_REASON_VASSALAGE,
	PEACE_BLOCK_REASON_TOO_SOON,
	PEACE_BLOCK_REASON_CITY_JUST_CAPTURED,
	PEACE_BLOCK_REASON_AT_WAR_WITH_ALLY,
	PEACE_BLOCK_REASON_WAR_DEAL,
	PEACE_BLOCK_REASON_NO_ENEMY_CAPITAL,
	PEACE_BLOCK_REASON_COOP_WAR_AGAINST_DEFENSIVE_PACT,
	PEACE_BLOCK_REASON_SCENARIO,

	NUM_PEACE_BLOCK_REASONS ENUM_META_VALUE,
};

enum CLOSED_ENUM PeaceTreatyTypes
{
	NO_PEACE_TREATY_TYPE = -1,

	PEACE_TREATY_WHITE_PEACE,
	PEACE_TREATY_ARMISTICE,
	PEACE_TREATY_SETTLEMENT,
	PEACE_TREATY_BACKDOWN,
	PEACE_TREATY_SUBMISSION,
	PEACE_TREATY_SURRENDER,
	PEACE_TREATY_CESSION,
	PEACE_TREATY_CAPITULATION,
	PEACE_TREATY_UNCONDITIONAL_SURRENDER,

	NUM_PEACE_TREATY_TYPES ENUM_META_VALUE,
};

enum CLOSED_ENUM CoopWarStates
{
	NO_COOP_WAR_STATE = -1,

	COOP_WAR_STATE_WARNED_TARGET,
	COOP_WAR_STATE_REJECTED,
	COOP_WAR_STATE_PREPARING,
	COOP_WAR_STATE_ONGOING,

	NUM_COOP_WAR_STATES ENUM_META_VALUE,
};

enum CLOSED_ENUM PublicDeclarationTypes
{
	NO_PUBLIC_DECLARATION_TYPE = -1,

	PUBLIC_DECLARATION_PROTECT_MINOR,
	PUBLIC_DECLARATION_ABANDON_MINOR,

	NUM_PUBLIC_DECLARATION_TYPES ENUM_META_VALUE,
};

enum CLOSED_ENUM DealOfferResponseTypes
{
	NO_DEAL_RESPONSE_TYPE = -1,

	DEAL_RESPONSE_GENEROUS,
	DEAL_RESPONSE_ACCEPTABLE,
	DEAL_RESPONSE_UNACCEPTABLE,
	DEAL_RESPONSE_INSULTING,

	NUM_DEAL_RESPONSES ENUM_META_VALUE,
};

enum CLOSED_ENUM DemandResponseTypes
{
	NO_DEMAND_RESPONSE_TYPE = -1,

	DEMAND_RESPONSE_ACCEPT,
	DEMAND_RESPONSE_REFUSE_WEAK,
	DEMAND_RESPONSE_REFUSE_HOSTILE,
	DEMAND_RESPONSE_REFUSE_TOO_MUCH,
	DEMAND_RESPONSE_REFUSE_TOO_SOON,
	DEMAND_RESPONSE_GIFT_ACCEPT,
	DEMAND_RESPONSE_GIFT_REFUSE_TOO_MUCH,
	DEMAND_RESPONSE_GIFT_REFUSE_TOO_SOON,

	NUM_DEMAND_RESPONSES ENUM_META_VALUE,
};

enum CLOSED_ENUM ShareOpinionResponseTypes
{
	NO_SHARE_OPINION_RESPONSE = -1,

	SHARE_OPINION_RESPONSE_ACCEPTED,
	SHARE_OPINION_RESPONSE_REFUSED,

	NUM_SHARE_OPINION_RESPONSES ENUM_META_VALUE,
};

enum CLOSED_ENUM MoveTroopsResponseTypes
{
	NO_MOVE_TROOPS_RESPONSE = -1,

	MOVE_TROOPS_RESPONSE_ACCEPT,
	MOVE_TROOPS_RESPONSE_NEUTRAL,
	MOVE_TROOPS_RESPONSE_REFUSE,

	NUM_MOVE_TROOPS_RESPONSES ENUM_META_VALUE,
};

enum CLOSED_ENUM VassalTreatmentTypes
{
	NO_VASSAL_TREATMENT = -1,

	VASSAL_TREATMENT_CONTENT,
	VASSAL_TREATMENT_DISAGREE,
	VASSAL_TREATMENT_MISTREATED,
	VASSAL_TREATMENT_UNHAPPY,
	VASSAL_TREATMENT_ENSLAVED,

	NUM_VASSAL_TREATMENT_TYPES ENUM_META_VALUE
};

inline FDataStream& operator<<(FDataStream& saveTo, const VictoryPursuitTypes& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
inline FDataStream& operator>>(FDataStream& loadFrom, VictoryPursuitTypes& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<VictoryPursuitTypes>(v);
	return loadFrom;
}

inline FDataStream& operator<<(FDataStream& saveTo, const CoopWarStates& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
inline FDataStream& operator>>(FDataStream& loadFrom, CoopWarStates& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<CoopWarStates>(v);
	return loadFrom;
}

inline FDataStream& operator<<(FDataStream& saveTo, const StateAllWars& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
inline FDataStream& operator>>(FDataStream& loadFrom, StateAllWars& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<StateAllWars>(v);
	return loadFrom;
}

inline FDataStream& operator<<(FDataStream& saveTo, const PeaceTreatyTypes& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
inline FDataStream& operator>>(FDataStream& loadFrom, PeaceTreatyTypes& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<PeaceTreatyTypes>(v);
	return loadFrom;
}

inline FDataStream& operator<<(FDataStream& saveTo, const DiploStatementTypes& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
inline FDataStream& operator>>(FDataStream& loadFrom, DiploStatementTypes& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<DiploStatementTypes>(v);
	return loadFrom;
}

inline FDataStream& operator<<(FDataStream& saveTo, const PublicDeclarationTypes& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
inline FDataStream& operator>>(FDataStream& loadFrom, PublicDeclarationTypes& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<PublicDeclarationTypes>(v);
	return loadFrom;
}

// Retained for LUA compatibility but not used...
enum CLOSED_ENUM MajorCivApproachTypes
{
	NO_MAJOR_CIV_APPROACH = -1,

	MAJOR_CIV_APPROACH_WAR,
	MAJOR_CIV_APPROACH_HOSTILE,
	MAJOR_CIV_APPROACH_DECEPTIVE,
	MAJOR_CIV_APPROACH_GUARDED,
	MAJOR_CIV_APPROACH_AFRAID,
	MAJOR_CIV_APPROACH_NEUTRAL,
	MAJOR_CIV_APPROACH_FRIENDLY,

	NUM_MAJOR_CIV_APPROACHES ENUM_META_VALUE,
};
#endif	// DIPLOMACY_AI_ENUMS_H