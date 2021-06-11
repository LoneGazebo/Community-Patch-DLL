/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

template<typename T, typename E>
inline FDataStream& WriteBasicEnum(FDataStream& saveTo, const E& e)
{
	saveTo << static_cast<T>(e);
	return saveTo;
}

template<typename T, typename E>
inline FDataStream& ReadBasicEnum(FDataStream& loadFrom, E& e)
{
	T v;
	loadFrom >> v;
	e = static_cast<E>(v);
	return loadFrom;
}

FDataStream& operator<<(FDataStream&, const TerrainTypes&);
FDataStream& operator>>(FDataStream&, TerrainTypes&);

FDataStream& operator<<(FDataStream&, const FeatureTypes&);
FDataStream& operator>>(FDataStream&, FeatureTypes&);

FDataStream& operator<<(FDataStream&, const PromotionTypes&);
FDataStream& operator>>(FDataStream&, PromotionTypes&);

FDataStream& operator<<(FDataStream&, const YieldTypes&);
FDataStream& operator>>(FDataStream&, YieldTypes&);

FDataStream& operator<<(FDataStream&, const SpecialistTypes&);
FDataStream& operator>>(FDataStream&, SpecialistTypes&);

FDataStream& operator<<(FDataStream&, const PlotTypes&);
FDataStream& operator>>(FDataStream&, PlotTypes&);

FDataStream& operator<<(FDataStream&, const GreatPersonTypes&);
FDataStream& operator>>(FDataStream&, GreatPersonTypes&);

FDataStream& operator<<(FDataStream&, const MonopolyTypes&);
FDataStream& operator>>(FDataStream&, MonopolyTypes&);

FDataStream& operator<<(FDataStream&, const DomainTypes&);
FDataStream& operator>>(FDataStream&, DomainTypes&);

FDataStream& operator<<(FDataStream&, const TradeConnectionType&);
FDataStream& operator>>(FDataStream&, TradeConnectionType&);

FDataStream& operator<<(FDataStream&, const GreatWorkType&);
FDataStream& operator>>(FDataStream&, GreatWorkType&);

FDataStream& operator<<(FDataStream&, const GreatWorkClass&);
FDataStream& operator>>(FDataStream&, GreatWorkClass&);

FDataStream& operator<<(FDataStream&, const ProjectTypes&);
FDataStream& operator>>(FDataStream&, ProjectTypes&);

FDataStream& operator<<(FDataStream&, const ProcessTypes&);
FDataStream& operator>>(FDataStream&, ProcessTypes&);

FDataStream& operator<<(FDataStream&, const AIGrandStrategyTypes&);
FDataStream& operator>>(FDataStream&, AIGrandStrategyTypes&);

FDataStream& operator<<(FDataStream&, const GreatWorkArtifactClass&);
FDataStream& operator>>(FDataStream&, GreatWorkArtifactClass&);

template<typename Enum, typename T>
inline FDataStream& operator<<(FDataStream& saveTo, const CvEnum<Enum, T>& readFrom)
{
	return saveTo << readFrom.raw();
}
template<typename Enum, typename T>
inline FDataStream& operator>>(FDataStream& loadFrom, CvEnum<Enum, T>& writeTo)
{
	return loadFrom >> writeTo.raw();
}

namespace FSerialization
{
std::string toString(const YieldTypes&);
}