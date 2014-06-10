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
	static void DoBarbCampCleared(CvPlot* pPlot);
	static bool ShouldSpawnBarbFromCamp(CvPlot* pPlot);
	static void DoCampAttacked(CvPlot* pPlot);
	static void BeginTurn();
	static void DoCamps();
	static void DoUnits();

	static void MapInit(int iWorldNumPlots);
	static void Uninit();

	static void Read(FDataStream& kStream, uint uiParentVersion);
	static void Write(FDataStream& kStream);

private:
	static bool CanBarbariansSpawn();
	static bool IsPlotValidForBarbCamp(CvPlot* pPlot);
	static UnitTypes GetRandomBarbarianUnitType(CvArea* pArea, UnitAITypes eUnitAI);
	static void DoSpawnBarbarianUnit(CvPlot* pPlot);
	static void DoCampActivationNotice(CvPlot* pPlot);

	static short* m_aiPlotBarbCampSpawnCounter;
	static short* m_aiPlotBarbCampNumUnitsSpawned;

	static FStaticVector<DirectionTypes, 6, true, c_eCiv5GameplayDLL, 0> m_aeValidBarbSpawnDirections;
};

#endif // CVBARBARIANS_H
