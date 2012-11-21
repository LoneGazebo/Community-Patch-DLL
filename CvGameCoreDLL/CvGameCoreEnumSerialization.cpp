/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreEnumSerialization.h"

FDataStream & operator<<(FDataStream& saveTo, const YieldTypes& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
//------------------------------------------------------------------------------
FDataStream & operator>>(FDataStream& loadFrom, YieldTypes& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<YieldTypes>(v);
	return loadFrom;
}
//------------------------------------------------------------------------------
namespace FSerialization
{
	std::string toString(const YieldTypes & v)
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
		default:
			return std::string("INVALID ENUM VALUE");
			break;
		}
		return std::string("INVALID ENUM VALUE");
	}


}