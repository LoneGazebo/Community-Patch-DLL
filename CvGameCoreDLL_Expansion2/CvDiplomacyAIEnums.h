/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef DIPLOMACY_AI_ENUMS_H
#define DIPLOMACY_AI_ENUMS_H

// WARNING: The order of many of these values is very important (many places in the code use > or < ENUM_VALUE to make important determinations.) Do not change unless you know what you're doing!

enum MajorCivOpinionTypes
{
	NO_MAJOR_CIV_OPINION = -1,

	MAJOR_CIV_OPINION_UNFORGIVABLE,
	MAJOR_CIV_OPINION_ENEMY,
	MAJOR_CIV_OPINION_COMPETITOR,
	MAJOR_CIV_OPINION_NEUTRAL,
	MAJOR_CIV_OPINION_FAVORABLE,
	MAJOR_CIV_OPINION_FRIEND,
	MAJOR_CIV_OPINION_ALLY,

	NUM_MAJOR_CIV_OPINIONS,
};

enum MajorCivApproachTypes
{
	NO_MAJOR_CIV_APPROACH = -1,

	MAJOR_CIV_APPROACH_WAR,
	MAJOR_CIV_APPROACH_HOSTILE,
	MAJOR_CIV_APPROACH_DECEPTIVE,
	MAJOR_CIV_APPROACH_GUARDED,
	MAJOR_CIV_APPROACH_AFRAID,
	MAJOR_CIV_APPROACH_FRIENDLY,
	MAJOR_CIV_APPROACH_NEUTRAL,

	NUM_MAJOR_CIV_APPROACHES,
};

enum MinorCivApproachTypes
{
	NO_MINOR_CIV_APPROACH = -1,

	MINOR_CIV_APPROACH_IGNORE,
	MINOR_CIV_APPROACH_FRIENDLY,
	MINOR_CIV_APPROACH_PROTECTIVE,
	MINOR_CIV_APPROACH_CONQUEST,
	MINOR_CIV_APPROACH_BULLY,

	NUM_MINOR_CIV_APPROACHES,
};

enum WarFaceTypes
{
	NO_WAR_FACE_TYPE = -1,

	WAR_FACE_HOSTILE,
	WAR_FACE_GUARDED,
	WAR_FACE_NEUTRAL,
	WAR_FACE_FRIENDLY,

	NUM_WAR_FACES,
};

enum WarStateTypes
{
	NO_WAR_STATE_TYPE = -1,

	WAR_STATE_NEARLY_DEFEATED,
	WAR_STATE_DEFENSIVE,
	WAR_STATE_STALEMATE,
	WAR_STATE_CALM,
	WAR_STATE_OFFENSIVE,
	WAR_STATE_NEARLY_WON,

	NUM_WAR_STATES,
};

enum WarProjectionTypes
{
	NO_WAR_PROJECTION_TYPE = -1,

	WAR_PROJECTION_DESTRUCTION,
	WAR_PROJECTION_DEFEAT,
	WAR_PROJECTION_STALEMATE,
	WAR_PROJECTION_UNKNOWN,
	WAR_PROJECTION_GOOD,
	WAR_PROJECTION_VERY_GOOD,

	NUM_WAR_PROJECTION_TYPES,
};

enum WarGoalTypes
{
	NO_WAR_GOAL_TYPE = -1,

	WAR_GOAL_DEMAND,
	WAR_GOAL_PREPARE,
	WAR_GOAL_CONQUEST,
	WAR_GOAL_DAMAGE,
	WAR_GOAL_PEACE,

	NUM_WAR_GOALS,
};

enum StrengthTypes
{
	NO_STRENGTH_VALUE = -1,

	STRENGTH_PATHETIC,
	STRENGTH_WEAK,
	STRENGTH_POOR,
	STRENGTH_AVERAGE,
	STRENGTH_STRONG,
	STRENGTH_POWERFUL,
	STRENGTH_IMMENSE,

	NUM_STRENGTH_VALUES,
};

enum TargetValueTypes
{
	NO_TARGET_VALUE = -1,

	TARGET_VALUE_IMPOSSIBLE,
	TARGET_VALUE_BAD,
	TARGET_VALUE_AVERAGE,
	TARGET_VALUE_FAVORABLE,
	TARGET_VALUE_SOFT,

	NUM_TARGET_VALUES,
};

enum WarDamageLevelTypes
{
	NO_WAR_DAMAGE_LEVEL_VALUE = -1,

	WAR_DAMAGE_LEVEL_NONE,
	WAR_DAMAGE_LEVEL_MINOR,
	WAR_DAMAGE_LEVEL_MAJOR,
	WAR_DAMAGE_LEVEL_SERIOUS,
	WAR_DAMAGE_LEVEL_CRIPPLED,

	NUM_WAR_DAMAGE_LEVEL_TYPES,
};

enum AggressivePostureTypes
{
	NO_AGGRESSIVE_POSTURE_TYPE = -1,

	AGGRESSIVE_POSTURE_NONE,
	AGGRESSIVE_POSTURE_LOW,
	AGGRESSIVE_POSTURE_MEDIUM,
	AGGRESSIVE_POSTURE_HIGH,
	AGGRESSIVE_POSTURE_INCREDIBLE,

	NUM_AGGRESSIVE_POSTURE_TYPES,
};

enum ThreatTypes
{
	NO_THREAT_VALUE = -1,

	THREAT_NONE,
	THREAT_MINOR,
	THREAT_MAJOR,
	THREAT_SEVERE,
	THREAT_CRITICAL,

	NUM_THREAT_VALUES,
};

enum DisputeLevelTypes
{
	NO_DISPUTE_LEVEL = -1,

	DISPUTE_LEVEL_NONE,
	DISPUTE_LEVEL_WEAK,
	DISPUTE_LEVEL_STRONG,
	DISPUTE_LEVEL_FIERCE,

	NUM_DISPUTE_LEVELS,
};

enum BlockLevelTypes
{
	NO_BLOCK_LEVEL = -1,

	BLOCK_LEVEL_NONE,
	BLOCK_LEVEL_WEAK,
	BLOCK_LEVEL_STRONG,
	BLOCK_LEVEL_FIERCE,

	NUM_BLOCK_LEVELS,
};

enum DoFLevelTypes
{
	NO_DOF_TYPE = -1,

	DOF_TYPE_UNTRUSTWORTHY,
	DOF_TYPE_NEW,
	DOF_TYPE_FRIENDS,
	DOF_TYPE_ALLIES,
	DOF_TYPE_BATTLE_BROTHERS,

	NUM_DOF_TYPES,
};

enum DiploPersonalityTypes
{
	NO_DIPLO_PERSONALITY_TYPE = -1,

	DIPLO_PERSONALITY_CONQUEROR,
	DIPLO_PERSONALITY_DIPLOMAT,
	DIPLO_PERSONALITY_CULTURAL,
	DIPLO_PERSONALITY_SCIENTIST,

	NUM_DIPLO_PERSONALITY_TYPES,
};

enum VictoryFocusTypes
{
	NO_VICTORY_FOCUS_TYPE = -1,

	VICTORY_FOCUS_DOMINATION,
	VICTORY_FOCUS_DIPLOMATIC,
	VICTORY_FOCUS_CULTURE,
	VICTORY_FOCUS_SCIENCE,

	NUM_VICTORY_FOCUS_TYPES,
};

enum PromiseStates
{
	NO_PROMISE_STATE = -1,

	PROMISE_STATE_MADE,
	PROMISE_STATE_IGNORED,
	PROMISE_STATE_BROKEN,

	NUM_PROMISE_STATES,
};

enum ModifierTypes
{
	NO_MODIFIER_TYPE = -1,

	MODIFIER_TYPE_NORMAL,
	MODIFIER_TYPE_STACKED,
	MODIFIER_TYPE_DIMINISHING,

	NUM_MODIFIER_TYPES,
};

enum TheftTypes
{
	NO_THEFT_TYPE = -1,

	THEFT_TYPE_CULTURE_BOMB,
	THEFT_TYPE_CONVERSION,
	THEFT_TYPE_ARTIFACT,
	THEFT_TYPE_SPY,
	THEFT_TYPE_TRADE_ROUTE,
	THEFT_TYPE_PLOT,

	NUM_THEFT_TYPES,
};

enum GuessConfidenceTypes
{
	NO_GUESS_CONFIDENCE_TYPE = -1,

	GUESS_CONFIDENCE_UNSURE,
	GUESS_CONFIDENCE_LIKELY,
	GUESS_CONFIDENCE_POSITIVE,

	NUM_GUESS_CONFIDENCE_TYPES,
};

enum StateAllWars
{
	STATE_ALL_WARS_NEUTRAL = 0,
	STATE_ALL_WARS_WINNING,
	STATE_ALL_WARS_LOSING,

	NUM_STATES_ALL_WARS,
};

enum PeaceTreatyTypes
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

	NUM_PEACE_TREATY_TYPES,
};

enum DealOfferResponseTypes
{
	NO_DEAL_RESPONSE_TYPE = -1,

	DEAL_RESPONSE_GENEROUS,
	DEAL_RESPONSE_ACCEPTABLE,
	DEAL_RESPONSE_UNACCEPTABLE,
	DEAL_RESPONSE_INSULTING,

	NUM_DEAL_RESPONSES,
};

enum DemandResponseTypes
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

	NUM_DEMAND_RESPONSES,
};

enum VassalTreatmentTypes
{
	NO_VASSAL_TREATMENT = -1,

	VASSAL_TREATMENT_CONTENT,
	VASSAL_TREATMENT_DISAGREE,
	VASSAL_TREATMENT_MISTREATED,
	VASSAL_TREATMENT_UNHAPPY,
	VASSAL_TREATMENT_ENSLAVED,

	NUM_VASSAL_TREATMENT_TYPES
};

enum MoveTroopsResponseTypes
{
	NO_MOVE_TROOPS_RESPONSE = -1,

	MOVE_TROOPS_RESPONSE_ACCEPT,
	MOVE_TROOPS_RESPONSE_NEUTRAL,
	MOVE_TROOPS_RESPONSE_REFUSE,

	NUM_MOVE_TROOPS_RESPONSES,
};

enum PublicDeclarationTypes
{
	NO_PUBLIC_DECLARATION_TYPE = -1,

	PUBLIC_DECLARATION_PROTECT_MINOR,
	PUBLIC_DECLARATION_ABANDON_MINOR,

	NUM_PUBLIC_DECLARATION_TYPES,
};

enum CoopWarStates
{
	NO_COOP_WAR_STATE = -1,

	COOP_WAR_STATE_WARNED_TARGET,
	COOP_WAR_STATE_REJECTED,
	COOP_WAR_STATE_PREPARING,
	COOP_WAR_STATE_READY,
	COOP_WAR_STATE_ONGOING,

	NUM_COOP_WAR_STATES,
};

inline FDataStream& operator<<(FDataStream& saveTo, const DiploPersonalityTypes& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
inline FDataStream& operator>>(FDataStream& loadFrom, DiploPersonalityTypes& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<DiploPersonalityTypes>(v);
	return loadFrom;
}

inline FDataStream& operator<<(FDataStream& saveTo, const VictoryFocusTypes& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
inline FDataStream& operator>>(FDataStream& loadFrom, VictoryFocusTypes& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<VictoryFocusTypes>(v);
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
#endif	// DIPLOMACY_AI_ENUMS_H