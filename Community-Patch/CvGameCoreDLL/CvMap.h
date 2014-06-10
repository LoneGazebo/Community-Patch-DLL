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

//  MODDED:  The Civ 5 team
//	PURPOSE: Game map class

#include "CvArea.h"
#include "CvPlot.h"
#include "CvInfos.h"

// a simplified version of CvArea for use primarily with the continent generation system
// one huge difference is that impassible terrain doesn't split a landmass like it would a CvArea
class CvLandmass
{
public:
	CvLandmass();
	~CvLandmass();

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

FDataStream & operator<<(FDataStream &, const CvLandmass &);
FDataStream & operator>>(FDataStream &, CvLandmass &);



class CvAStar;

inline int coordRange(int iCoord, int iRange, bool bWrap)
{
	if (bWrap)
	{
   		if (iCoord < 0 )
   		{
   			return (iRange + (iCoord % iRange));
   		}
   		else if (iCoord >= iRange)
   		{
   			return (iCoord % iRange);
   		}
	}

	return iCoord;
}

//
// CvMap
//
class CvMap
{
public:

	CvMap();
	~CvMap();

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
	void updateWorkingCity(CvPlot* pPlot=0, int iRange=0);
	void updateYield();

	void verifyUnitValidPlot();

	CvPlot* syncRandPlot(int iFlags = 0, int iArea = -1, int iMinUnitDistance = -1, int iTimeout = 100);

	CvCity* findCity(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, bool bSameArea = true, bool bCoastalOnly = false, TeamTypes eTeamAtWarWith = NO_TEAM, DirectionTypes eDirection = NO_DIRECTION, const CvCity* pSkipCity = NULL);
	CvUnit* findUnit(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, bool bReadyToSelect = false, bool bWorkers = false);

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
		if ((iX <= INVALID_PLOT_COORD) || (iY <= INVALID_PLOT_COORD))
		{
			return NULL;
		}
		return &(m_pMapPlots[plotNum(iX, iY)]);
	}

	__forceinline CvPlot* plotUnchecked(int iX, int iY) const
	{
		return &(m_pMapPlots[plotNum(iX, iY)]);
	}

	/// Areas
	int getIndexAfterLastArea();
	int getNumAreas();
	int getNumLandAreas();
	CvArea* getArea(int iID);
	CvArea* addArea();
	void deleteArea(int iID);
	CvArea* firstArea(int *pIterIdx, bool bRev=false);
	CvArea* nextArea(int *pIterIdx, bool bRev=false);

	void recalculateAreas();
	void calculateAreas();

	/// Landmass
	int getIndexAfterLastLandmass();
	int getNumLandmasses();
	int getNumLandLandmasses();
	CvLandmass* getLandmass(int iID);
	CvLandmass* addLandmass();
	void deleteLandmass(int iID);
	CvLandmass* firstLandmass(int *pIterIdx, bool bRev=false);
	CvLandmass* nextLandmass(int *pIterIdx, bool bRev=false);
	void recalculateLandmasses();
	void calculateLandmasses();

	int calculateInfluenceDistance(CvPlot *pSource, CvPlot *pDest, int iMaxRange, bool bCorrectButSlower=true);
	/// this is the default "continent stamper" a given lua map script can use it or not
	void DefaultContinentStamper();

	// Serialization:
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;
	GUID GetGUID() const {return m_guid;};

	int Validate();

	void rebuild(int iGridW, int iGridH, int iTopLatitude, int iBottomLatitude, bool bWrapX, bool bWrapY, WorldSizeTypes eWorldSize, ClimateTypes eClimate, SeaLevelTypes eSeaLevel);

	// Natural Wonders stuff
	int GetNumNaturalWonders() const;
	void ChangeNumNaturalWonders(int iChange);

	void DoPlaceNaturalWonders();

	void ChangeAIMapHint(int iMapHint);
	int GetAIMapHint();
	// End Natural Wonders stuff

	typedef FStaticVector<CvPlot*, 1000, true, c_eCiv5GameplayDLL, 1> DeferredPlotArray;
	DeferredPlotArray m_vDeferredFogPlots; // don't serialize me

protected:

	int m_iGridWidth;
	int m_iGridHeight;
	int m_iGridSize; // not serialized as it is always w*h
	int m_iLandPlots;
	int m_iOwnedPlots;
	int m_iTopLatitude;
	int m_iBottomLatitude;
	int m_iNumNaturalWonders;

	int m_iAIMapHints; // currently only 0 for normal, 1 for primarily naval, 2 for raze cities whenever possible, 3 for both
					   // Replace this with BITFLAGS as soon as we can!

	bool m_bWrapX;
	bool m_bWrapY;

	int* m_paiNumResource;
	int* m_paiNumResourceOnLand;

	CvPlot* m_pMapPlots;

	short *m_pYields;
	int   *m_pFoundValue;
	char  *m_pPlayerCityRadiusCount;
	short *m_pVisibilityCount;
	char  *m_pRevealedOwner;
	short *m_pRevealedImprovementType;
	short *m_pRevealedRouteType;
	bool  *m_pNoSettling;
	bool *m_pResourceForceReveal;

	FFreeListTrashArray<CvArea> m_areas;
	FFreeListTrashArray<CvLandmass> m_landmasses;

	GUID m_guid;
};

#endif
