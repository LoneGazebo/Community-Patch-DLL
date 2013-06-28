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

enum MajorCivOpinionTypes
{
    NO_MAJOR_CIV_OPINION_TYPE = -1,

    MAJOR_CIV_OPINION_UNFORGIVABLE,
    MAJOR_CIV_OPINION_ENEMY,
    MAJOR_CIV_OPINION_COMPETITOR,
    MAJOR_CIV_OPINION_NEUTRAL,
    MAJOR_CIV_OPINION_FAVORABLE,
    MAJOR_CIV_OPINION_FRIEND,
    MAJOR_CIV_OPINION_ALLY,

    NUM_MAJOR_CIV_OPINION_TYPES,
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
    WAR_FACE_NEUTRAL,
    WAR_FACE_FRIENDLY,

    NUM_WAR_FACES,
};

enum WarStateTypes
{
    NO_WAR_STATE_TYPE = -1,

    // WARNING: the order of these values is very important, do not change unless you know what you're doing!
    WAR_STATE_NEARLY_DEFEATED,
    WAR_STATE_DEFENSIVE,
    WAR_STATE_STALEMATE,
    WAR_STATE_CALM,
    WAR_STATE_OFFENSIVE,
    WAR_STATE_NEARLY_WON,
    // WARNING: the order of these values is very important, do not change unless you know what you're doing!

    NUM_WAR_STATES,
};

enum WarProjectionTypes
{
    NO_WAR_PROJECTION_TYPE = -1,

    // WARNING: the order of these values is very important, do not change unless you know what you're doing!
    WAR_PROJECTION_DESTRUCTION,
    WAR_PROJECTION_DEFEAT,
    WAR_PROJECTION_STALEMATE,
    WAR_PROJECTION_UNKNOWN,
    WAR_PROJECTION_GOOD,
    WAR_PROJECTION_VERY_GOOD,
    // WARNING: the order of these values is very important, do not change unless you know what you're doing!

    NUM_WAR_PROJECTION_TYPES,
};

enum WarGoalTypes
{
    NO_WAR_GOAL_TYPE = -1,

    // WARNING: the order of these values is very important, do not change unless you know what you're doing!
    WAR_GOAL_DEMAND,
    WAR_GOAL_PREPARE,
    WAR_GOAL_CONQUEST,
    WAR_GOAL_DAMAGE,
    WAR_GOAL_PEACE,
    // WARNING: the order of these values is very important, do not change unless you know what you're doing!

    NUM_WAR_GOALS,
};

enum StrengthTypes
{
    NO_STRENGTH_VALUE = -1,

    // WARNING: the order of these values is very important, do not change unless you know what you're doing!
    STRENGTH_PATHETIC,
    STRENGTH_WEAK,
    STRENGTH_POOR,
    STRENGTH_AVERAGE,
    STRENGTH_STRONG,
    STRENGTH_POWERFUL,
    STRENGTH_IMMENSE,
    // WARNING: the order of these values is very important, do not change unless you know what you're doing!

    NUM_STRENGTH_VALUES,
};

enum TargetValueTypes
{
    NO_TARGET_VALUE = -1,

    // WARNING: the order of these values is very important, do not change unless you know what you're doing!
    TARGET_VALUE_IMPOSSIBLE,
    TARGET_VALUE_BAD,
    TARGET_VALUE_AVERAGE,
    TARGET_VALUE_FAVORABLE,
    TARGET_VALUE_SOFT,
    // WARNING: the order of these values is very important, do not change unless you know what you're doing!

    NUM_TARGET_VALUES,
};

enum WarDamageLevelTypes
{
    NO_WAR_DAMAGE_LEVEL_VALUE = -1,

    // WARNING: the order of these values is very important, do not change unless you know what you're doing!
    WAR_DAMAGE_LEVEL_NONE,
    WAR_DAMAGE_LEVEL_MINOR,
    WAR_DAMAGE_LEVEL_MAJOR,
    WAR_DAMAGE_LEVEL_SERIOUS,
    WAR_DAMAGE_LEVEL_CRIPPLED,
    // WARNING: the order of these values is very important, do not change unless you know what you're doing!

    NUM_WAR_DAMAGE_LEVEL_TYPES,
};

enum AggressivePostureTypes
{
    NO_AGGRESSIVE_POSTURE_TYPE = -1,

    // WARNING: the order of these values is very important, do not change unless you know what you're doing!
    AGGRESSIVE_POSTURE_NONE,
    AGGRESSIVE_POSTURE_LOW,
    AGGRESSIVE_POSTURE_MEDIUM,
    AGGRESSIVE_POSTURE_HIGH,
    AGGRESSIVE_POSTURE_INCREDIBLE,
    // WARNING: the order of these values is very important, do not change unless you know what you're doing!

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

    // WARNING: the order of these values is very important, do not change unless you know what you're doing!
    DISPUTE_LEVEL_NONE,
    DISPUTE_LEVEL_WEAK,
    DISPUTE_LEVEL_STRONG,
    DISPUTE_LEVEL_FIERCE,
    // WARNING: the order of these values is very important, do not change unless you know what you're doing!

    NUM_DISPUTE_LEVELS,
};

enum AgreementLevelTypes
{
    NO_AGREEMENT_LEVEL = -1,

    AGREEMENT_LEVEL_NONE,
    AGREEMENT_LEVEL_WEAK,
    AGREEMENT_LEVEL_STRONG,
    AGREEMENT_LEVEL_FIERCE,

    NUM_AGREEMENT_LEVELS,
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

enum PeaceTreatyTypes
{
    NO_PEACE_TREATY_TYPE = -1,

    // WARNING: the order of these values is very important, do not change unless you know what you're doing!
    PEACE_TREATY_WHITE_PEACE,
    PEACE_TREATY_ARMISTICE,
	PEACE_TREATY_SETTLEMENT,
	PEACE_TREATY_BACKDOWN,
	PEACE_TREATY_SUBMISSION,
    PEACE_TREATY_SURRENDER,
	PEACE_TREATY_CESSION,
    PEACE_TREATY_CAPITULATION,
    PEACE_TREATY_UNCONDITIONAL_SURRENDER,
    // WARNING: the order of these values is very important, do not change unless you know what you're doing!

    NUM_PEACE_TREATY_TYPES,
};

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

enum DealOfferResponseTypes
{
    NO_DEAL_RESPONSE_TYPE = -1,

    // WARNING: the order of these values is very important, do not change unless you know what you're doing!
    DEAL_RESPONSE_GENEROUS,
    DEAL_RESPONSE_ACCEPTABLE,
    DEAL_RESPONSE_UNACCEPTABLE,
    DEAL_RESPONSE_INSULTING,
    // WARNING: the order of these values is very important, do not change unless you know what you're doing!

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

    NUM_DEMAND_RESPONSES,
};

enum PublicDeclarationTypes
{
    NO_PUBLIC_DECLARATION_TYPE = -1,

    PUBLIC_DECLARATION_PROTECT_MINOR,
    PUBLIC_DECLARATION_ABANDON_MINOR,

    NUM_PUBLIC_DECLARATION_TYPES,
};

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

enum CoopWarStates
{
    NO_COOP_WAR_STATE = -1,

    // WARNING: the order of these values is very important, do not change unless you know what you're doing!
    COOP_WAR_STATE_REJECTED,
    COOP_WAR_STATE_SOON,
    COOP_WAR_STATE_ACCEPTED,
    // WARNING: the order of these values is very important, do not change unless you know what you're doing!

    NUM_COOP_WAR_STATES,
};

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

#endif	// DIPLOMACY_AI_ENUMS_H
