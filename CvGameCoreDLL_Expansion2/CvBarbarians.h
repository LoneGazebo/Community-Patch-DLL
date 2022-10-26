/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#ifndef CVBARBARIANS_H
#define CVBARBARIANS_H

#pragma once

#include "CvPlot.h"

class CvBarbarians
{
public:
	static void MapInit(int iWorldNumPlots);
	static void Uninit();

	template<typename Visitor>
	static void Serialize(Visitor& visitor);
	static void Read(FDataStream& kStream);
	static void Write(FDataStream& kStream);

	// Memory values
	static int GetBarbSpawnerCounter(CvPlot* pPlot);
	static void SetBarbSpawnerCounter(CvPlot* pPlot, int iNewValue);
	static void ChangeBarbSpawnerCounter(CvPlot* pPlot, int iChange);
	static void DoBarbSpawnerAttacked(CvPlot* pPlot);
	static bool ShouldSpawnBarbFromCity(CvPlot* pPlot);
	static bool ShouldSpawnBarbFromCamp(CvPlot* pPlot);
	static int GetBarbSpawnerNumUnitsSpawned(CvPlot* pPlot);
	static void SetBarbSpawnerNumUnitsSpawned(CvPlot* pPlot, int iNewValue);
	static void ChangeBarbSpawnerNumUnitsSpawned(CvPlot* pPlot, int iChange);

	// Barb Spawner Management
	static void ActivateBarbSpawner(CvPlot* pPlot);
	static void DoBarbCampCleared(CvPlot* pPlot, PlayerTypes ePlayer, CvUnit* pUnit = NULL);
	static void DoBarbCityCleared(CvPlot* pPlot);

	// Barb Special Attacks
	static bool DoStealFromCity(CvUnit* pUnit, CvCity* pCity);
	static bool DoTakeOverCityState(CvCity* pCity);

	// Turn Stuff
	static void DoCamps();
	static void DoUnits();

	// Unleash the beast
	static void DoSpawnBarbarianUnit(CvPlot* pPlot, bool bIgnoreMaxBarbarians, bool bFinishMoves);

private:
	static bool CanBarbariansSpawn();
	static UnitTypes GetRandomBarbarianUnitType(CvPlot* pPlot, UnitAITypes eUnitAI, ResourceTypes eNearbyResource = NO_RESOURCE);
	static short* m_aiBarbSpawnerCounter;
	static short* m_aiBarbSpawnerNumUnitsSpawned;
};

#endif // CVBARBARIANS_H