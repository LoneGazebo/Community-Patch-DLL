/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_MAP_H
#define CIV5_MAP_H

//
//	FILE:	 CvMap.h
//	AUTHOR:  Soren Johnson
//  MODDED:  The Civ 5 team
//	PURPOSE: Game map class
//-----------------------------------------------------------------------------
//	Copyright (c) 2010 Firaxis Games, Inc. All rights reserved.
//-----------------------------------------------------------------------------
//


#include "CvArea.h"
#include "CvPlot.h"
#include "CvInfos.h"
#include "CvPlotManager.h"
#include "CvGameCoreUtils.h"

// a simplified version of CvArea for use primarily with the continent generation system
// one huge difference is that impassible terrain doesn't split a landmass like it would a CvArea
class CvLandmass
{
public:
	CvLandmass();
	virtual ~CvLandmass();

	void init(int iID, bool bWater);
	int GetID() const;
	void SetID(int iID);

	int getNumTiles() const;
	void changeNumTiles(int iChange);

	bool isWater() const;
	bool isLake() const;

	char GetContinentType() const;
	void SetContinentType(const char cContinent);

	void ChangeCentroidX(int iDelta);
	void ChangeCentroidY(int iDelta);
	int GetCentroidX();
	int GetCentroidY();

	// for serialization
	virtual void read(FDataStream& kStream);
	virtual void write(FDataStream& kStream) const;

protected:

	int m_iID;
	int m_iNumTiles;
	char m_cContinentType;
	bool m_bWater;
	int m_iCentroidX;
	int m_iCentroidY;

};

FDataStream& operator<<(FDataStream&, const CvLandmass&);
FDataStream& operator>>(FDataStream&, CvLandmass&);

const int ciMapHint_Naval = 1;		// 1 for primarily naval (this does NOT do offshore expansion), 
const int ciMapHint_Raze = 2;		// 2 for raze cities whenever possible,
const int ciMapHint_Offshore = 4;	// 4 for expand offshore
const int ciMapHint_NavalOffshore = 5; // combination of 1 and 5

inline int coordRange(int iCoord, int iRange, bool bWrap)
{
	if(bWrap)
	{
		if(iCoord < 0)
		{
			return (iRange + (iCoord % iRange));
		}
		else if(iCoord >= iRange)
		{
			return (iCoord % iRange);
		}
	}

	return iCoord;
}

class CvPlotManager;
typedef std::set<CvPlot*> DeferredFogPlots;
typedef std::map<PlayerTypes, std::map<int, int>> UnitKillCount;

//
// CvMap
//
class CvMap
{
public:

	CvMap();
	virtual ~CvMap();

	void init(CvMapInitData* pInitData = NULL);
	void setupGraphical();
	void reset(CvMapInitData* pInitData);
	void uninit();

protected:
	void setup();
	void InitPlots();

public:

	void erasePlots();
	void setRevealedPlots(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly = false);
	void setAllPlotTypes(PlotTypes ePlotType);

	void doTurn();

	void updateFog();
	void updateDeferredFog();
	void updateVisibility();
	void updateLayout(bool bDebug);
	void updateSight(bool bIncrement);
	void updateCenterUnit();
	void updateOwningCity(CvPlot* pPlot = 0, int iRange = 0);
	void updateYield();
	void updateAdjacency();

	void verifyUnitValidPlot();

	CvPlot* syncRandPlot(int iFlags = 0, int iArea = -1, int iMinUnitDistance = -1, int iTimeout = 100);

	CvCity* findCity(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, bool bSameArea = true, bool bCoastalOnly = false, TeamTypes eTeamAtWarWith = NO_TEAM, DirectionTypes eDirection = NO_DIRECTION, const CvCity* pSkipCity = NULL);
	CvUnit* findUnit(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, bool bReadyToSelect = false, bool bWorkers = false);
	CvPlot* findNearestStartPlot(int iX, int iY, PlayerTypes& eOwner);

	CvArea* findBiggestArea(bool bWater);

	int getMapFractalFlags();
	bool findWater(CvPlot* pPlot, int iRange, bool bFreshWater);

	inline int isPlot(int iX, int iY) const
	{
		return ((iX >= 0) && (iX < getGridWidth()) && (iY >= 0) && (iY < getGridHeight()));
	}

	inline int numPlots() const
	{
		return m_iGridSize;
	}

	inline int plotNum(int iX, int iY) const
	{
		return ((iY * getGridWidth()) + iX);
	}

	int plotX(int iIndex) const;
	int plotY(int iIndex) const;

	int maxPlotDistance();

	inline int getGridWidth() const
	{
		return m_iGridWidth;
	}
	inline int getGridHeight() const
	{
		return m_iGridHeight;
	}

	int getLandPlots();
	void changeLandPlots(int iChange);

	int getOwnedPlots();
	void changeOwnedPlots(int iChange);

	int getTopLatitude();
	int getBottomLatitude();

	inline bool isWrapX() const
	{
		return m_bWrapX;
	}

	inline bool isWrapY() const
	{
		return m_bWrapY;
	}

	WorldSizeTypes getWorldSize();
	ClimateTypes getClimate();
	SeaLevelTypes getSeaLevel();

	const CvWorldInfo& getWorldInfo() const;
	const CvClimateInfo& getClimateInfo() const;
	const CvSeaLevelInfo& getSeaLevelInfo() const;

	// Resources
	int getRandomResourceQuantity(ResourceTypes eIndex);

	int getNumResources(ResourceTypes eIndex);
	void changeNumResources(ResourceTypes eIndex, int iChange);
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	void setNumResources(ResourceTypes eIndex);
#endif

	int getNumResourcesOnLand(ResourceTypes eIndex);
	void changeNumResourcesOnLand(ResourceTypes eIndex, int iChange);

	/// Plot accessors
	__forceinline CvPlot* plotByIndex(int iIndex) const
	{
		return (((iIndex >= 0) && (iIndex < (numPlots()))) ? &(m_pMapPlots[iIndex]) : NULL);
	}
	__forceinline CvPlot* plotByIndexUnchecked(int iIndex) const
	{
		return &m_pMapPlots[iIndex];
	}

	__forceinline CvPlot* plot(int iX, int iY) const
	{
		if ((iX == INVALID_PLOT_COORD) || (iY == INVALID_PLOT_COORD))
		{
			return NULL;
		}
		int iMapX = coordRange(iX, getGridWidth(), isWrapX());
		int iMapY = coordRange(iY, getGridHeight(), isWrapY());
		return ((isPlot(iMapX, iMapY)) ? &(m_pMapPlots[plotNum(iMapX, iMapY)]) : NULL);
	}
	__forceinline CvPlot* plotCheckInvalid(int iX, int iY) const
	{
		if ((iX == INVALID_PLOT_COORD) || (iY == INVALID_PLOT_COORD) || iX==-1 || iY==-1)
		{
			return NULL;
		}
		return &(m_pMapPlots[plotNum(iX, iY)]);
	}

	__forceinline CvPlot* plotUnchecked(int iX, int iY) const
	{
		return &(m_pMapPlots[plotNum(iX, iY)]);
	}
#if defined(MOD_BALANCE_CORE)
	CvPlot** getNeighborsUnchecked(const CvPlot* pPlot) const
	{
		return m_pPlotNeighbors + plotNum(pPlot->getX(), pPlot->getY())*(NUM_DIRECTION_TYPES + 2);
	}
	CvPlot** getNeighborsUnchecked(int iIndex) const
	{
		return m_pPlotNeighbors + iIndex*(NUM_DIRECTION_TYPES + 2);
	}

	CvPlot** getNeighborsShuffled(const CvPlot* pPlot);
	CvPlot* getNeighborUnchecked(int iX, int iY, DirectionTypes eDir) const
	{
		return m_pPlotNeighbors[plotNum(iX, iY)*(NUM_DIRECTION_TYPES + 2) + (int)eDir];
	}

	void PrecalcNeighbors();
#endif

	CvPlotManager& plotManager() { return m_kPlotManager; }
	DeferredFogPlots& deferredFogPlots() { return m_vDeferredFogPlots; }

	/// Areas
	int getIndexAfterLastArea();
	int getNumAreas();
	int getNumLandAreas();
	CvArea* getArea(int iID);
	CvArea* addArea();
	void deleteArea(int iID);
	CvArea* firstArea(int* pIterIdx, bool bRev=false);
	CvArea* nextArea(int* pIterIdx, bool bRev=false);

	void recalculateAreas();
	void calculateAreas();

	// Landmass
	int getIndexAfterLastLandmass();
	int getNumLandmasses();
	int getNumLandLandmasses();
	CvLandmass* getLandmass(int iID);
	CvLandmass* addLandmass();
	void deleteLandmass(int iID);
	CvLandmass* firstLandmass(int* pIterIdx, bool bRev=false);
	CvLandmass* nextLandmass(int* pIterIdx, bool bRev=false);
	void recalculateLandmasses();
	void calculateLandmasses();

	/// this is the default "continent stamper" a given lua map script can use it or not
	void DefaultContinentStamper();

	// Serialization:
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;
	GUID GetGUID() const
	{
		return m_guid;
	};

	int Validate();

	void rebuild(int iGridW, int iGridH, int iTopLatitude, int iBottomLatitude, bool bWrapX, bool bWrapY, WorldSizeTypes eWorldSize, ClimateTypes eClimate, SeaLevelTypes eSeaLevel);

	// Natural Wonders stuff
	int GetNumNaturalWonders() const;
	void ChangeNumNaturalWonders(int iChange);

	void DoPlaceNaturalWonders();

	void ChangeAIMapHint(int iMapHint);
	int GetAIMapHint();
	// End Natural Wonders stuff

	void ClearPlotsAtRange(const CvPlot* pPlot);
	std::vector<CvPlot*> GetPlotsAtRange(const CvPlot* pPlot, int iRange, bool bFromPlot, bool bWithLOS);

	int GetPopupCount(int iPlotIndex);
	void IncreasePopupCount(int iPlotIndex);

#if defined(MOD_UNIT_KILL_STATS)
	int GetUnitKillCount(PlayerTypes ePlayer, int iPlotIndex);
	void IncrementUnitKillCount(PlayerTypes ePlayer, int iPlotIndex);
	void ExportUnitKillCount(PlayerTypes ePlayer);
	void DoKillCountDecay(float fDecayFactor = 0.98);
#endif

protected:

	int m_iGridWidth;
	int m_iGridHeight;
	int m_iGridSize; // not serialized as it is always w*h
	int m_iLandPlots;
	int m_iOwnedPlots;
	int m_iTopLatitude;
	int m_iBottomLatitude;
	int m_iNumNaturalWonders;

	int m_iAIMapHints;	// currently only:
						// 0 for normal, 
						// 1 for primarily naval (this does NOT do offshore expansion), 
						// 2 for raze cities whenever possible,
						// 4 for expand offshore (previously 1 was naval and expansion, but I am splitting the behaviors, to get the old behavior use 4+1)

	bool m_bWrapX;
	bool m_bWrapY;

	int* m_paiNumResource;
	int* m_paiNumResourceOnLand;

	CvPlot* m_pMapPlots;
#if defined(MOD_BALANCE_CORE)
	CvPlot** m_pPlotNeighbors;			//precomputed neighbors for each plot
	CvPlot* m_apShuffledNeighbors[6];	//scratchpad for shuffled access to neighbors
#endif

	uint8* m_pYields;
	uint8* m_pPlayerCityRadiusCount;
	uint8* m_pVisibilityCount;				//actual vis count
	uint8* m_pVisibilityCountThisTurnMax;	//maximum vis count this turn
	char*  m_pRevealedOwner;
#if defined(MOD_BALANCE_CORE)
	bool*  m_pIsImpassable;
	bool* m_pIsStrategic;
#endif
	bool* m_pRevealed;
	char* m_pRevealedImprovementType;
	char* m_pRevealedRouteType;
	bool* m_pResourceForceReveal;

	TContainer<CvArea> m_areas;
	TContainer<CvLandmass> m_landmasses;

	//store non-zero values outside of CvPlot because it will be zero almost all the time
	typedef map<int, vector<unsigned char>> PlotInvisibleVisibilityLookup;
	map<TeamTypes, PlotInvisibleVisibilityLookup> m_invisibleVisibilityCount;

	GUID m_guid;

	CvPlotManager	m_kPlotManager;

	DeferredFogPlots m_vDeferredFogPlots; // don't serialize me

	//caching some semi-static data, not serialized
	typedef map<int, vector<CvPlot*>> PlotNeighborLookup;
	PlotNeighborLookup m_vPlotsWithLineOfSightFromPlot2;
	PlotNeighborLookup m_vPlotsWithLineOfSightFromPlot3;
	PlotNeighborLookup m_vPlotsWithLineOfSightToPlot2;
	PlotNeighborLookup m_vPlotsWithLineOfSightToPlot3;

	map<int, int> m_plotPopupCount; //not serialized

#if defined(MOD_UNIT_KILL_STATS)
	// player -> plot index -> number of owned units killed
	UnitKillCount killCount;
#endif
};

#endif
