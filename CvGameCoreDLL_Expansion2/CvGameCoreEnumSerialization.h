/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

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
namespace FSerialization
{
std::string toString(const YieldTypes&);
}