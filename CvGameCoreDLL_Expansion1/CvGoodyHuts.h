/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#ifndef CVGOODYHUTS_H
#define CVGOODYHUTS_H

#pragma once

#define NUM_GOODIES_REMEMBERED (3)

class CvGoodyHuts
{
public:
	static void DoPlayerReceivedGoody(PlayerTypes ePlayer, GoodyTypes eGoody);
	static bool IsCanPlayerReceiveGoody(PlayerTypes ePlayer, GoodyTypes eGoody);
	static bool IsHasPlayerReceivedGoodyLately(PlayerTypes ePlayer, GoodyTypes eGoody);

	static void Reset();
	static void Uninit();

	static void Read(FDataStream& kStream, uint uiParentVersion);
	static void Write(FDataStream& kStream);

private:
	static int** m_aaiPlayerGoodyHutResults;

};

#endif