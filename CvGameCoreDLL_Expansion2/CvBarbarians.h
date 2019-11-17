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
	static void DoBarbCampCleared(CvPlot* pPlot, PlayerTypes ePlayer);
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	static void DoBarbCityCleared(CvPlot* pPlot);
	static bool ShouldSpawnBarbFromCity(CvPlot* pPlot);
	static void DoCityAttacked(CvPlot* pPlot);
#endif
	static bool ShouldSpawnBarbFromCamp(CvPlot* pPlot);
	static void DoCampAttacked(CvPlot* pPlot);
	static void DoCampSpawnCounter();
	static void DoCamps();
	static void DoUnits();
	static void DoCampActivationNotice(CvPlot* pPlot);
	static void DoSpawnBarbarianUnit(CvPlot* pPlot, bool bIgnoreMaxBarbarians, bool bFinishMoves);

	static void MapInit(int iWorldNumPlots);
	static void Uninit();

	static void Read(FDataStream& kStream, uint uiParentVersion);
	static void Write(FDataStream& kStream);
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	static void DoCityActivationNotice(CvPlot* pPlot);
#endif

private:
	static bool CanBarbariansSpawn();
	static bool IsPlotValidForBarbCamp(CvPlot* pPlot);
	static UnitTypes GetRandomBarbarianUnitType(CvPlot* pPlot, UnitAITypes eUnitAI, ResourceTypes eNearbyResource = NO_RESOURCE);

	static short* m_aiPlotBarbCampSpawnCounter;
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	static short* m_aiPlotBarbCitySpawnCounter;
	static short* m_aiPlotBarbCityNumUnitsSpawned;
#endif
	static short* m_aiPlotBarbCampNumUnitsSpawned;

};

#endif // CVBARBARIANS_H