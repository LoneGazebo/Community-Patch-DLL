/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreEnumSerialization.h"

FDataStream& operator<<(FDataStream& saveTo, const AITacticalMove& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
FDataStream& operator<<(FDataStream& saveTo, const AIHomelandMove& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}

//------------------------------------------------------------------------------
FDataStream& operator>>(FDataStream& loadFrom, AITacticalMove& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<AITacticalMove>(v);
	return loadFrom;
}
FDataStream& operator>>(FDataStream& loadFrom, AIHomelandMove& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<AIHomelandMove>(v);
	return loadFrom;
}

//------------------------------------------------------------------------------
FDataStream& operator<<(FDataStream& saveTo, const YieldTypes& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, YieldTypes& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<YieldTypes>(v);
	return loadFrom;
}
//------------------------------------------------------------------------------
namespace FSerialization
{
std::string toString(const YieldTypes& v)
{
	switch(v)
	{
	case NO_YIELD:
		return std::string("NO_YIELD");
		break;
	case YIELD_FOOD:
		return std::string("YIELD_FOOD");
		break;
	case YIELD_PRODUCTION:
		return std::string("YIELD_PRODUCTION");
		break;
	case YIELD_GOLD:
		return std::string("YIELD_GOLD");
		break;
	case YIELD_SCIENCE:
		return std::string("YIELD_SCIENCE");
		break;
	case YIELD_CULTURE:
		return std::string("YIELD_CULTURE");
		break;
	case YIELD_FAITH:
		return std::string("YIELD_FAITH");
		break;
#if defined(MOD_API_UNIFIED_YIELDS_TOURISM)
	case YIELD_TOURISM:
		return std::string("YIELD_TOURISM");
		break;
#endif
#if defined(MOD_API_UNIFIED_YIELDS_GOLDEN_AGE)
	case YIELD_GOLDEN_AGE_POINTS:
		return std::string("YIELD_GOLDEN_AGE_POINTS");
		break;
#endif
#if defined(MOD_BALANCE_CORE_YIELDS)
	case YIELD_GREAT_GENERAL_POINTS:
		return std::string("YIELD_GREAT_GENERAL_POINTS");
		break;
	case YIELD_GREAT_ADMIRAL_POINTS:
		return std::string("YIELD_GREAT_ADMIRAL_POINTS");
		break;
	case YIELD_POPULATION:
		return std::string("YIELD_POPULATION");
		break;
	case YIELD_CULTURE_LOCAL:
		return std::string("YIELD_CULTURE_LOCAL");
		break;
#endif
#if defined(MOD_BALANCE_CORE_JFD)
	case YIELD_JFD_HEALTH:
		return std::string("YIELD_JFD_HEALTH");
		break;
	case YIELD_JFD_DISEASE:
		return std::string("YIELD_JFD_DISEASE");
		break;
#endif
	default:
		return std::string("INVALID ENUM VALUE");
		break;
	}
	return std::string("INVALID ENUM VALUE");
}


}