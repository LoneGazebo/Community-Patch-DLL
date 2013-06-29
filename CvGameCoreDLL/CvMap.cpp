/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//
//	FILE:	 CvMap.cpp
//	AUTHOR:  Soren Johnson
//	PURPOSE: Game map class
//

#include "CvGameCoreDLLPCH.h"
#include "CvMap.h"
#include "CvCity.h"
#include "CvGlobals.h"
#include "CvPlayerAI.h"
#include "CvRandom.h"
#include "CvGameCoreUtils.h"
#include "CvFractal.h"
#include "CvPlot.h"
#include "CvGameCoreUtils.h"
#include "CvMap.h"
#include "CvAStar.h"
#include "CvInfos.h"
#include "CvInfosSerializationHelper.h"
// for GUIDs
typedef struct tagMSG *LPMSG;
#include <objbase.h>

// must be included after all other headers
#include "LintFree.h"

#ifdef _GC_SUPPORT_CACHED_MAP_DATA
// Public static globals for fast access.  Only works if there is one instance of a CvMap!
int CvMap::ms_iGridW = 0;
int CvMap::ms_iGridH = 0;
bool CvMap::ms_bWrapX = false;
bool CvMap::ms_bWrapY = false;
#endif

//////////////////////////////////////////////////////////////////////////
// CvLandmass
//////////////////////////////////////////////////////////////////////////

//	--------------------------------------------------------------------------------
CvLandmass::CvLandmass()
{
	m_iID = -1;
	m_iNumTiles = 0;
	m_bWater = true;
	m_cContinentType = 0;
	m_iCentroidX = 0;
	m_iCentroidY = 0;
}

//	--------------------------------------------------------------------------------
CvLandmass::~CvLandmass()
{

}

//	--------------------------------------------------------------------------------
void CvLandmass::init(int iID, bool bWater)
{
	m_iID = iID;
	m_iNumTiles = 0;
	m_bWater = bWater;
	m_cContinentType = 0;
	m_iCentroidX = 0;
	m_iCentroidY = 0;
}

//	--------------------------------------------------------------------------------
int CvLandmass::GetID() const
{
	return m_iID;
}

//	--------------------------------------------------------------------------------
void CvLandmass::SetID(int iID)
{
	m_iID = iID;
}

//	--------------------------------------------------------------------------------
int CvLandmass::getNumTiles() const
{
	return m_iNumTiles;
}

//	--------------------------------------------------------------------------------
void CvLandmass::changeNumTiles(int iChange)
{
	bool bOldLake;

	if (iChange != 0)
	{
		bOldLake = isLake();

		m_iNumTiles = (m_iNumTiles + iChange);
		CvAssert(m_iNumTiles >= 0);

		if (bOldLake != isLake())
		{
			GC.getMap().updateYield();
		}
	}
}

//	--------------------------------------------------------------------------------
void CvLandmass::ChangeCentroidX(int iChange)
{
	m_iCentroidX = (m_iCentroidX + iChange);
}

//	--------------------------------------------------------------------------------
void CvLandmass::ChangeCentroidY(int iChange)
{
	m_iCentroidY = (m_iCentroidY + iChange);
}

//	--------------------------------------------------------------------------------
bool CvLandmass::isWater() const
{
	return m_bWater;
}

//	--------------------------------------------------------------------------------
bool CvLandmass::isLake() const
{
	return (m_bWater && (m_iNumTiles <= GC.getLAKE_MAX_AREA_SIZE()));
}

//	--------------------------------------------------------------------------------
char CvLandmass::GetContinentType() const
{
	return m_cContinentType;
}

//	--------------------------------------------------------------------------------
void CvLandmass::SetContinentType(const char cContinent)
{
	m_cContinentType = cContinent;
}

//	--------------------------------------------------------------------------------
int CvLandmass::GetCentroidX()
{
	if (m_iNumTiles > 0)
	{
		return m_iCentroidX / m_iNumTiles;

	}
	return -1;
}

//	--------------------------------------------------------------------------------
int CvLandmass::GetCentroidY()
{
	if (m_iNumTiles > 0)
	{
		return m_iCentroidY / m_iNumTiles;

	}
	return -1;
}


//	--------------------------------------------------------------------------------
void CvLandmass::read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_iID;
	kStream >> m_iNumTiles;

	kStream >> m_iCentroidX;
	kStream >> m_iCentroidY;

	kStream >> m_bWater;

	kStream >> m_cContinentType;
}

//	--------------------------------------------------------------------------------
void CvLandmass::write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << m_iID;
	kStream << m_iNumTiles;

	kStream << m_iCentroidX;
	kStream << m_iCentroidY;

	kStream << m_bWater;

	kStream << m_cContinentType;

}


//	--------------------------------------------------------------------------------
FDataStream & operator<<(FDataStream & saveTo, const CvLandmass & readFrom)
{
	readFrom.write(saveTo);
	return saveTo;
}

//	--------------------------------------------------------------------------------
FDataStream & operator>>(FDataStream & loadFrom, CvLandmass & writeTo)
{
	writeTo.read(loadFrom);
	return loadFrom;
}

static uint sgCvMapInstanceCount = 0;
//////////////////////////////////////////////////////////////////////////
// CvMap
//////////////////////////////////////////////////////////////////////////

// Public Functions...

//	--------------------------------------------------------------------------------
CvMap::CvMap()
{
	CvMapInitData defaultMapData;

	CvAssert(sgCvMapInstanceCount == 0);
	++sgCvMapInstanceCount;

	m_paiNumResource = NULL;
	m_paiNumResourceOnLand = NULL;

	m_pMapPlots = NULL;


	m_pYields = NULL;
	m_pFoundValue = NULL;
	m_pPlayerCityRadiusCount = NULL;
	m_pVisibilityCount = NULL;
	m_pRevealedOwner = NULL;
	m_pRevealedImprovementType = NULL;
	m_pRevealedRouteType = NULL;
	m_pNoSettling = NULL;
	m_pResourceForceReveal = NULL;

	m_iAIMapHints = 0;

	reset(&defaultMapData);
}


//	--------------------------------------------------------------------------------
CvMap::~CvMap()
{
	uninit();
	--sgCvMapInstanceCount;
}

//	--------------------------------------------------------------------------------
void CvMap::InitPlots()
{

	m_pMapPlots = FNEW(CvPlot[numPlots()], c_eCiv5GameplayDLL, 0);
	//alcoate all the mmemory we need up front

	int iNumPlots = numPlots();

	m_pYields					= FNEW(short[NUM_YIELD_TYPES*iNumPlots], c_eCiv5GameplayDLL, 0);
	m_pFoundValue				= FNEW(int[REALLY_MAX_PLAYERS*iNumPlots], c_eCiv5GameplayDLL, 0);
	m_pPlayerCityRadiusCount	= FNEW(char[REALLY_MAX_PLAYERS*iNumPlots], c_eCiv5GameplayDLL, 0);
	m_pVisibilityCount			= FNEW(short[REALLY_MAX_TEAMS*iNumPlots], c_eCiv5GameplayDLL, 0);
	m_pRevealedOwner			= FNEW(char[REALLY_MAX_TEAMS*iNumPlots], c_eCiv5GameplayDLL, 0);
	m_pRevealedImprovementType  = FNEW(short[REALLY_MAX_TEAMS*iNumPlots], c_eCiv5GameplayDLL, 0);
	m_pRevealedRouteType		= FNEW(short[REALLY_MAX_TEAMS*iNumPlots], c_eCiv5GameplayDLL, 0);
	m_pNoSettling				= FNEW(bool[MAX_MAJOR_CIVS*iNumPlots], c_eCiv5GameplayDLL, 0);
	m_pResourceForceReveal		= FNEW(bool[REALLY_MAX_TEAMS*iNumPlots], c_eCiv5GameplayDLL, 0);

	memset(m_pYields, 0, NUM_YIELD_TYPES*iNumPlots*sizeof(short));
	memset(m_pFoundValue, 0, REALLY_MAX_PLAYERS*iNumPlots*sizeof(int) );
	memset(m_pPlayerCityRadiusCount, 0, REALLY_MAX_PLAYERS*iNumPlots*sizeof(char));
	memset(m_pVisibilityCount, 0,REALLY_MAX_TEAMS*iNumPlots *sizeof(short) );
	memset(m_pRevealedOwner, -1 ,REALLY_MAX_TEAMS*iNumPlots *sizeof(char) );
	memset(m_pRevealedImprovementType, 0,REALLY_MAX_TEAMS*iNumPlots *sizeof(short) );
	memset(m_pRevealedRouteType, 0,REALLY_MAX_TEAMS*iNumPlots *sizeof(short) );
	memset(m_pNoSettling, 0,MAX_MAJOR_CIVS*iNumPlots *sizeof(bool) );
	memset(m_pResourceForceReveal, 0,REALLY_MAX_TEAMS*iNumPlots *sizeof(bool) );


	short *pYields					= m_pYields;
	int   *pFoundValue				= m_pFoundValue;
	char  *pPlayerCityRadiusCount   = m_pPlayerCityRadiusCount;
	short *pVisibilityCount			= m_pVisibilityCount;
	char  *pRevealedOwner			= m_pRevealedOwner;
	short *pRevealedImprovementType = m_pRevealedImprovementType;
	short *pRevealedRouteType		= m_pRevealedRouteType;
	bool  *pNoSettling				= m_pNoSettling;
	bool  *pResourceForceReveal		= m_pResourceForceReveal;

	for(int i = 0; i < iNumPlots; i++)
	{
		m_pMapPlots[i].m_aiYield				= pYields;
		m_pMapPlots[i].m_aiFoundValue			= pFoundValue;
		m_pMapPlots[i].m_aiPlayerCityRadiusCount= pPlayerCityRadiusCount;
		m_pMapPlots[i].m_aiVisibilityCount		= pVisibilityCount;
		m_pMapPlots[i].m_aiRevealedOwner		= pRevealedOwner;

		m_pMapPlots[i].m_aeRevealedImprovementType	= pRevealedImprovementType;
		m_pMapPlots[i].m_aeRevealedRouteType		= pRevealedRouteType;
		m_pMapPlots[i].m_abNoSettling				= pNoSettling;

		m_pMapPlots[i].m_abResourceForceReveal		= pResourceForceReveal;


		pYields					+= NUM_YIELD_TYPES;
		pFoundValue				+= REALLY_MAX_PLAYERS;
		pPlayerCityRadiusCount  += REALLY_MAX_PLAYERS;
		pVisibilityCount		+= REALLY_MAX_TEAMS;
		pRevealedOwner			+= REALLY_MAX_TEAMS;
		pRevealedImprovementType+= REALLY_MAX_TEAMS;
		pRevealedRouteType		+= REALLY_MAX_TEAMS;
		pNoSettling				+= MAX_MAJOR_CIVS;
		pResourceForceReveal	+= REALLY_MAX_TEAMS;

	}
}

//	--------------------------------------------------------------------------------
void CvMap::init(CvMapInitData* pInitInfo/*=NULL*/)
{
	int iX, iY;

	CvString strMemState = CvString::format("CvMap::init begin - world size=%s, climate=%s, sealevel=%s",
		getWorldInfo().GetDescription(),
		getClimateInfo().GetDescription(),
		getSeaLevelInfo().GetDescription());


	//--------------------------------
	// Init saved data
	reset(pInitInfo);

	//--------------------------------
	// Init containers
	m_areas.Init();
	m_landmasses.Init();

	//--------------------------------
	// Init non-saved data
	setup();

	//--------------------------------
	// Init other game data
	InitPlots();

	int iW = getGridWidth();
	int iH = getGridHeight();


	for (iX = 0; iX < iW; iX++)
	{
		for (iY = 0; iY < iH; iY++)
		{
			plotUnchecked(iX, iY)->init(iX, iY);
		}
	}
}


//	--------------------------------------------------------------------------------
void CvMap::uninit()
{
	SAFE_DELETE_ARRAY(m_paiNumResource);
	SAFE_DELETE_ARRAY(m_paiNumResourceOnLand);

	SAFE_DELETE_ARRAY(m_pMapPlots);


	SAFE_DELETE_ARRAY(m_pYields);
	SAFE_DELETE_ARRAY(m_pFoundValue);
	SAFE_DELETE_ARRAY(m_pPlayerCityRadiusCount);
	SAFE_DELETE_ARRAY(m_pVisibilityCount);
	SAFE_DELETE_ARRAY(m_pRevealedOwner);
	SAFE_DELETE_ARRAY(m_pRevealedImprovementType);
	SAFE_DELETE_ARRAY(m_pRevealedRouteType);
	SAFE_DELETE_ARRAY(m_pNoSettling);
	SAFE_DELETE_ARRAY(m_pResourceForceReveal);

	m_iGridWidth = 0;
	m_iGridHeight = 0;
	m_iGridSize = 0;
	m_iLandPlots = 0;
	m_iOwnedPlots = 0;
	m_iNumNaturalWonders = 0;

	m_areas.Uninit();
	m_landmasses.Uninit();
}

//	--------------------------------------------------------------------------------
// FUNCTION: reset()
// Initializes data members that are serialized.
void CvMap::reset(CvMapInitData* pInitInfo)
{
	int iI;

	//--------------------------------
	// Uninit class
	uninit();

	m_iAIMapHints = 0;
	//
	// set grid size
	// initially set in terrain cell units
	//
	m_iGridWidth = (CvPreGame::worldSize() != NO_WORLDSIZE) ?  getWorldInfo().getGridWidth (): 0;	//todotw:tcells wide
	m_iGridHeight = (CvPreGame::worldSize() != NO_WORLDSIZE) ? getWorldInfo().getGridHeight (): 0;

	// allow grid size override
	if (pInitInfo)
	{
		m_iGridWidth	= pInitInfo->m_iGridW;
		m_iGridHeight	= pInitInfo->m_iGridH;
	}

	m_iGridSize = m_iGridHeight * m_iGridWidth;
	m_iLandPlots = 0;
	m_iOwnedPlots = 0;
	m_iNumNaturalWonders = 0;

	if (pInitInfo)
	{
		m_iTopLatitude = pInitInfo->m_iTopLatitude;
		m_iBottomLatitude = pInitInfo->m_iBottomLatitude;
	}

	m_iTopLatitude = std::min(m_iTopLatitude, 90);
	m_iTopLatitude = std::max(m_iTopLatitude, -90);
	m_iBottomLatitude = std::min(m_iBottomLatitude, 90);
	m_iBottomLatitude = std::max(m_iBottomLatitude, -90);

	//
	// set wrapping
	//
	m_bWrapX = true;
	m_bWrapY = false;

	CoCreateGuid( &m_guid );

	if (pInitInfo)
	{
		m_bWrapX = pInitInfo->m_bWrapX;
		m_bWrapY = pInitInfo->m_bWrapY;
	}

#ifdef _GC_SUPPORT_CACHED_MAP_DATA
	ms_iGridW = m_iGridWidth;
	ms_iGridH = m_iGridHeight;
	ms_bWrapX = m_bWrapX;
	ms_bWrapY = m_bWrapY;
#endif

	int iNumResourceInfos = GC.getNumResourceInfos();
	if (iNumResourceInfos)
	{
		CvAssertMsg((0 < GC.getNumResourceInfos()), "GC.getNumResourceInfos() is not greater than zero but an array is being allocated in CvMap::reset");
		CvAssertMsg(m_paiNumResource==NULL, "mem leak m_paiNumResource");
		m_paiNumResource = FNEW(int[iNumResourceInfos], c_eCiv5GameplayDLL, 0);
		CvAssertMsg(m_paiNumResourceOnLand==NULL, "mem leak m_paiNumResourceOnLand");
		m_paiNumResourceOnLand = FNEW(int[iNumResourceInfos], c_eCiv5GameplayDLL, 0);
		for (iI = 0; iI < iNumResourceInfos; iI++)
		{
			m_paiNumResource[iI] = 0;
			m_paiNumResourceOnLand[iI] = 0;
		}
	}

	m_areas.RemoveAll();
	m_landmasses.RemoveAll();

	m_vDeferredFogPlots.clear();

	gDLL->DoMapSetup(numPlots());
}


//	--------------------------------------------------------------------------------
// FUNCTION: setup()
// Initializes all data that is not serialized but needs to be initialized after loading.
void CvMap::setup()
{
	GC.getPathFinder().Initialize(getGridWidth(), getGridHeight(), isWrapX(), isWrapY(), PathDest, PathDestValid, PathHeuristic, PathCost, PathValid, PathAdd, PathNodeAdd, UnitPathInitialize, UnitPathUninitialize, NULL);
	GC.getPathFinder().SetDataChangeInvalidatesCache(true);
	GC.getInterfacePathFinder().Initialize(getGridWidth(), getGridHeight(), isWrapX(), isWrapY(), PathDest, PathDestValid, PathHeuristic, PathCost, PathValid, PathAdd, PathNodeAdd, UnitPathInitialize, UnitPathUninitialize, NULL);
	GC.getInterfacePathFinder().SetDataChangeInvalidatesCache(true);
	GC.getIgnoreUnitsPathFinder().Initialize(getGridWidth(), getGridHeight(), isWrapX(), isWrapY(), PathDest,  IgnoreUnitsDestValid, PathHeuristic, IgnoreUnitsCost, IgnoreUnitsValid, IgnoreUnitsPathAdd, NULL, NULL, NULL, UnitPathInitialize, UnitPathUninitialize, NULL);
	GC.getIgnoreUnitsPathFinder().SetDataChangeInvalidatesCache(true);
	GC.getStepFinder().Initialize(getGridWidth(), getGridHeight(), isWrapX(), isWrapY(), PathDest, StepDestValid, StepHeuristic, StepCost, StepValid, StepAdd, NULL, NULL, NULL, NULL, NULL, NULL);
	GC.getRouteFinder().Initialize(getGridWidth(), getGridHeight(), isWrapX(), isWrapY(), PathDest, NULL, NULL, NULL, RouteValid, NULL, NULL, RouteGetNumExtraChildren, RouteGetExtraChild, NULL, NULL, NULL);
	GC.GetWaterRouteFinder().Initialize(getGridWidth(), getGridHeight(), isWrapX(), isWrapY(), PathDest, NULL, NULL, NULL, WaterRouteValid, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	GC.getAreaFinder().Initialize(getGridWidth(), getGridHeight(), isWrapX(), isWrapY(), PathDest, NULL, NULL, NULL, AreaValid, NULL, JoinArea, NULL, NULL, NULL, NULL, NULL);
	GC.getInfluenceFinder().Initialize(getGridWidth(), getGridHeight(), isWrapX(), isWrapY(), PathDest, InfluenceDestValid, InfluenceHeuristic, InfluenceCost, InfluenceValid, InfluenceAdd, NULL, NULL, NULL, NULL, NULL, NULL);
	GC.GetBuildRouteFinder().Initialize(getGridWidth(), getGridHeight(), isWrapX(), isWrapY(), PathDest, NULL, NULL, BuildRouteCost, BuildRouteValid, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	GC.GetTacticalAnalysisMapFinder().Initialize(getGridWidth(), getGridHeight(), isWrapX(), isWrapY(), PathDest, PathDestValid, PathHeuristic, PathCost, TacticalAnalysisMapPathValid, PathAdd, PathNodeAdd, UnitPathInitialize, UnitPathUninitialize, NULL);
	GC.GetTacticalAnalysisMapFinder().SetDataChangeInvalidatesCache(true);
}


//////////////////////////////////////
// graphical only setup
//////////////////////////////////////
void CvMap::setupGraphical()
{
	if (!GC.IsGraphicsInitialized())
		return;

	if (m_pMapPlots != NULL)
	{
		int iI;
		for (iI = 0; iI < numPlots(); iI++)
		{
			plotByIndexUnchecked(iI)->setupGraphical();
		}
	}
}

//	--------------------------------------------------------------------------------
void CvMap::erasePlots()
{
	int iI;

	for (iI = 0; iI < numPlots(); iI++)
	{
		plotByIndexUnchecked(iI)->erase();
	}
}


//	--------------------------------------------------------------------------------
void CvMap::setRevealedPlots(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly)
{
	int iI;

	for (iI = 0; iI < numPlots(); iI++)
	{
		plotByIndexUnchecked(iI)->setRevealed(eTeam, bNewValue, bTerrainOnly);
	}
}


//	--------------------------------------------------------------------------------
void CvMap::setAllPlotTypes(PlotTypes ePlotType)
{

	for(int i=0;i<numPlots();i++)
	{
		plotByIndexUnchecked(i)->setPlotType(ePlotType, false, false);
	}

	recalculateAreas();
}


//	--------------------------------------------------------------------------------
void CvMap::doTurn()
{
	int iI;

	for (iI = 0; iI < numPlots(); iI++)
	{
		plotByIndexUnchecked(iI)->doTurn();
	}
}


//	--------------------------------------------------------------------------------
void CvMap::updateFog()
{
	int iI;

	for (iI = 0; iI < numPlots(); iI++)
	{
		plotByIndexUnchecked(iI)->updateFog();
	}
}


//	--------------------------------------------------------------------------------
void CvMap::updateDeferredFog()
{
	for (uint uiI=0; uiI < m_vDeferredFogPlots.size(); uiI++)
	{
		m_vDeferredFogPlots[uiI]->updateFog();
	}
	m_vDeferredFogPlots.clear();
}


//	--------------------------------------------------------------------------------
void CvMap::updateVisibility()
{
	int iI;

	for (iI = 0; iI < numPlots(); iI++)
	{
		plotByIndexUnchecked(iI)->updateVisibility();
	}
}

//	--------------------------------------------------------------------------------
void CvMap::updateLayout(bool bDebug)
{
	for (int iI = 0; iI < numPlots(); iI++)
	{
		CvPlot* pThisPlot = plotByIndexUnchecked(iI);
		if (pThisPlot && pThisPlot->isLayoutDirty())
		{
			pThisPlot->updateLayout(bDebug);
			pThisPlot->setLayoutDirty(false);
		}
	}
	DLLUI->setDirty(PlotData_DIRTY_BIT,false);
}

//	--------------------------------------------------------------------------------
void CvMap::updateSight(bool bIncrement)
{
	int iI;

	for (iI = 0; iI < numPlots(); iI++)
	{
		plotByIndexUnchecked(iI)->updateSight(bIncrement);
	}
}


//	--------------------------------------------------------------------------------
void CvMap::updateCenterUnit()
{
	int iI;

	for (iI = 0; iI < numPlots(); iI++)
	{
		plotByIndexUnchecked(iI)->updateCenterUnit();
	}
}


//	--------------------------------------------------------------------------------
void CvMap::updateWorkingCity(CvPlot* pPlot, int iRange)
{
	if (pPlot && iRange > 0)
	{
		for (int iX = -iRange; iX <= iRange; iX++)
		{
			for (int iY = -iRange; iY <= iRange; iY++)
			{
				CvPlot* pLoopPlot = plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iX, iY, iRange);
				if (pLoopPlot)
				{
					pLoopPlot->updateWorkingCity();
				}
			}
		}
	}
	else
	{
		for (int iI = 0; iI < numPlots(); iI++)
		{
			plotByIndexUnchecked(iI)->updateWorkingCity();
		}
	}
}


//	--------------------------------------------------------------------------------
void CvMap::updateYield()
{
	int iI;

	for (iI = 0; iI < numPlots(); iI++)
	{
		plotByIndexUnchecked(iI)->updateYield();
	}
}


//	--------------------------------------------------------------------------------
void CvMap::verifyUnitValidPlot()
{
	int iI;

	for (iI = 0; iI < numPlots(); iI++)
	{
		plotByIndexUnchecked(iI)->verifyUnitValidPlot();
	}
}


//	--------------------------------------------------------------------------------
CvPlot* CvMap::syncRandPlot(int iFlags, int iArea, int iMinUnitDistance, int iTimeout)
{
	CvPlot* pPlot;
	CvPlot* pTestPlot;
	CvPlot* pLoopPlot;
	bool bValid;
	int iCount;
	int iDX, iDY;

	pPlot = NULL;

	iCount = 0;

	while (iCount < iTimeout)
	{
		iCount++;
		pTestPlot = plotCheckInvalid(GC.getGame().getJonRandNum(getGridWidth(), "Rand Plot Width"), GC.getGame().getJonRandNum(getGridHeight(), "Rand Plot Height"));

		CvAssertMsg(pTestPlot != NULL, "TestPlot is not assigned a valid value");

		if(!pTestPlot) continue;

		if ((iArea == -1) || (pTestPlot->getArea() == iArea))
		{
			bValid = true;

			if (bValid)
			{
				if (iMinUnitDistance != -1)
				{
					for (iDX = -(iMinUnitDistance); iDX <= iMinUnitDistance; iDX++)
					{
						for (iDY = -(iMinUnitDistance); iDY <= iMinUnitDistance; iDY++)
						{
							pLoopPlot	= plotXYWithRangeCheck(pTestPlot->getX(), pTestPlot->getY(), iDX, iDY, iMinUnitDistance);

							if (pLoopPlot != NULL)
							{
								if (pLoopPlot->isUnit())
								{
									bValid = false;
								}
							}
						}
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_LAND)
				{
					if (pTestPlot->isWater())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_UNOWNED)
				{
					if (pTestPlot->isOwned())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_ADJACENT_UNOWNED)
				{
					if (pTestPlot->isAdjacentOwned())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_ADJACENT_LAND)
				{
					if (!(pTestPlot->isAdjacentToLand()))
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_PASSIBLE)
				{
					if (pTestPlot->isImpassable())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_NOT_VISIBLE_TO_CIV)
				{
					if (pTestPlot->isVisibleToCivTeam())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_NOT_CITY)
				{
					if (pTestPlot->isCity())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				pPlot = pTestPlot;
				break;
			}
		}
	}

	return pPlot;
}

//	--------------------------------------------------------------------------------
CvCity* CvMap::findCity(int iX, int iY, PlayerTypes eOwner, TeamTypes eTeam, bool bSameArea, bool bCoastalOnly, TeamTypes eTeamAtWarWith, DirectionTypes eDirection, const CvCity* pSkipCity)
{
	CvCity* pLoopCity;
	CvCity* pBestCity;
	int iValue;
	int iBestValue;
	int iLoop;
	int iI;
	CvPlot* pCheckPlot = plot(iX, iY);

	CvAssertMsg(pCheckPlot != NULL, "Passed in an invalid plot to findCity");
	if (pCheckPlot == NULL)
		return NULL;

	// XXX look for barbarian cities???

	iBestValue = MAXINT;
	pBestCity = NULL;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& thisPlayer = GET_PLAYER((PlayerTypes)iI);
		if (thisPlayer.isAlive())
		{
			if ((eOwner == NO_PLAYER) || (iI == eOwner))
			{
				if ((eTeam == NO_TEAM) || (thisPlayer.getTeam() == eTeam))
				{
					for (pLoopCity = thisPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = thisPlayer.nextCity(&iLoop))
					{
						if (!bSameArea || (pLoopCity->area() == pCheckPlot->area()) || (bCoastalOnly && (pLoopCity->waterArea() == pCheckPlot->area())))
						{
							if (!bCoastalOnly || pLoopCity->isCoastal())
							{
								if ((eTeamAtWarWith == NO_TEAM) || atWar(thisPlayer.getTeam(), eTeamAtWarWith))
								{
									if ((eDirection == NO_DIRECTION) || (estimateDirection(dxWrap(pLoopCity->getX() - iX), dyWrap(pLoopCity->getY() - iY)) == eDirection))
									{
										if ((pSkipCity == NULL) || (pLoopCity != pSkipCity))
										{
											iValue = plotDistance(iX, iY, pLoopCity->getX(), pLoopCity->getY());

											if (iValue < iBestValue)
											{
												iBestValue = iValue;
												pBestCity = pLoopCity;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return pBestCity;
}


//	--------------------------------------------------------------------------------
CvUnit* CvMap::findUnit(int iX, int iY, PlayerTypes eOwner, bool bReadyToSelect, bool bWorkers)
{
	CvUnit* pLoopUnit;
	CvUnit* pBestUnit;
	int iValue;
	int iBestValue;
	int iLoop;
	int iI;

	iBestValue = INT_MAX;
	pBestUnit = NULL;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& thisPlayer = GET_PLAYER((PlayerTypes)iI);
		if (thisPlayer.isAlive())
		{
			if ((eOwner == NO_PLAYER) || (iI == eOwner))
			{
				for(pLoopUnit = thisPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = thisPlayer.nextUnit(&iLoop))
				{
					if (!bReadyToSelect || pLoopUnit->ReadyToSelect())
					{
						if (!bWorkers || pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER || pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER_SEA)
						{
							iValue = plotDistance(iX, iY, pLoopUnit->getX(), pLoopUnit->getY());

							if (iValue < iBestValue)
							{
								iBestValue = iValue;
								pBestUnit = pLoopUnit;
							}
						}
					}
				}
			}
		}
	}

	return pBestUnit;
}


//	--------------------------------------------------------------------------------
CvArea* CvMap::findBiggestArea(bool bWater)
{
	CvArea* pLoopArea;
	CvArea* pBestArea;
	int iValue;
	int iBestValue;
	int iLoop;

	iBestValue = 0;
	pBestArea = NULL;

	for(pLoopArea = firstArea(&iLoop); pLoopArea != NULL; pLoopArea = nextArea(&iLoop))
	{
		if (pLoopArea->isWater() == bWater)
		{
			iValue = pLoopArea->getNumTiles();

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestArea = pLoopArea;
			}
		}
	}

	return pBestArea;
}


//	--------------------------------------------------------------------------------
int CvMap::getMapFractalFlags()
{
	int wrapX = 0;
	if (isWrapX())
	{
		wrapX = (int)CvFractal::FRAC_WRAP_X;
	}

	int wrapY = 0;
	if (isWrapY())
	{
		wrapY = (int)CvFractal::FRAC_WRAP_Y;
	}

	return (wrapX | wrapY);
}


//	--------------------------------------------------------------------------------
//	Check plots for wetlands or seaWater.  Returns true if found
bool CvMap::findWater(CvPlot* pPlot, int iRange, bool bFreshWater)
{
	CvPlot* pLoopPlot;
	int iDX, iDY;
	int iPlotX = pPlot->getX();
	int iPlotY = pPlot->getY();

	for (iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for (iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pLoopPlot = plotXYWithRangeCheck(iPlotX, iPlotY, iDX, iDY, iRange);

			if (pLoopPlot != NULL)
			{
				if (bFreshWater)
				{
					if (pLoopPlot->isFreshWater())
					{
						return true;
					}
				}
				else
				{
					if (pLoopPlot->isWater())
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
int CvMap::plotX(int iIndex) const
{
	return (iIndex % getGridWidth());
}


//	--------------------------------------------------------------------------------
int CvMap::plotY(int iIndex) const
{
	return (iIndex / getGridWidth());
}


//	--------------------------------------------------------------------------------
int CvMap::maxPlotDistance()
{
	return std::max(1, plotDistance(0, 0, ((isWrapX()) ? (getGridWidth() / 2) : (getGridWidth() - 1)), ((isWrapY()) ? (getGridHeight() / 2) : (getGridHeight() - 1))));
}

//	--------------------------------------------------------------------------------
int CvMap::getLandPlots()
{
	return m_iLandPlots;
}


//	--------------------------------------------------------------------------------
void CvMap::changeLandPlots(int iChange)
{
	m_iLandPlots = (m_iLandPlots + iChange);
	CvAssert(getLandPlots() >= 0);
}


//	--------------------------------------------------------------------------------
int CvMap::getOwnedPlots()
{
	return m_iOwnedPlots;
}


//	--------------------------------------------------------------------------------
void CvMap::changeOwnedPlots(int iChange)
{
	m_iOwnedPlots = (m_iOwnedPlots + iChange);
	CvAssert(getOwnedPlots() >= 0);
}


//	--------------------------------------------------------------------------------
int CvMap::getTopLatitude()
{
	return m_iTopLatitude;
}


//	--------------------------------------------------------------------------------
int CvMap::getBottomLatitude()
{
	return m_iBottomLatitude;
}

//	--------------------------------------------------------------------------------
WorldSizeTypes CvMap::getWorldSize()
{
	return CvPreGame::worldSize();
}

//	--------------------------------------------------------------------------------
const CvWorldInfo& CvMap::getWorldInfo() const
{
	return CvPreGame::worldInfo();
}

//	--------------------------------------------------------------------------------
ClimateTypes CvMap::getClimate()
{
	return CvPreGame::climate();
}

//	--------------------------------------------------------------------------------
const CvClimateInfo& CvMap::getClimateInfo() const
{
	return CvPreGame::climateInfo();
}

//	--------------------------------------------------------------------------------
SeaLevelTypes CvMap::getSeaLevel()
{
	return CvPreGame::seaLevel();
}

//	--------------------------------------------------------------------------------
const CvSeaLevelInfo& CvMap::getSeaLevelInfo() const
{
	return CvPreGame::seaLevelInfo();
}

//	--------------------------------------------------------------------------------
int CvMap::getRandomResourceQuantity(ResourceTypes eIndex)
{
	CvResourceInfo* thisResourceInfo = GC.getResourceInfo(eIndex);
	if(thisResourceInfo == NULL)
		return 0;

	int iNumRands = 0;

	// Find how many types of resources quantities there can be to pick from
	for (int iLoop = 0; iLoop < GC.getNUM_RESOURCE_QUANTITY_TYPES(); iLoop++)
	{
		if (thisResourceInfo->getResourceQuantityType(iLoop) <= 0)
		{
			break;
		}

		iNumRands++;
	}

	CvAssertMsg(iNumRands > 0, "Resource should have at least 1 Quantity type to choose from")

	int iRand = GC.getGame().getJonRandNum(iNumRands, "Picking from random Resource Quantity types");

	return thisResourceInfo->getResourceQuantityType(iRand);
}

//	--------------------------------------------------------------------------------
int CvMap::getNumResources(ResourceTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiNumResource[eIndex];
}


//	--------------------------------------------------------------------------------
void CvMap::changeNumResources(ResourceTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiNumResource[eIndex] = (m_paiNumResource[eIndex] + iChange);
	CvAssert(getNumResources(eIndex) >= 0);
}


//	--------------------------------------------------------------------------------
int CvMap::getNumResourcesOnLand(ResourceTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiNumResourceOnLand[eIndex];
}


//	--------------------------------------------------------------------------------
void CvMap::changeNumResourcesOnLand(ResourceTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiNumResourceOnLand[eIndex] = (m_paiNumResourceOnLand[eIndex] + iChange);
	CvAssert(getNumResourcesOnLand(eIndex) >= 0);
}

//	--------------------------------------------------------------------------------
int CvMap::getIndexAfterLastArea()
{
	return m_areas.GetIndexAfterLast();
}


//	--------------------------------------------------------------------------------
int CvMap::getNumAreas()
{
	return m_areas.GetCount();
}


//	--------------------------------------------------------------------------------
int CvMap::getNumLandAreas()
{
	CvArea* pLoopArea;
	int iNumLandAreas;
	int iLoop;

	iNumLandAreas = 0;

	for(pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
	{
		if (!(pLoopArea->isWater()))
		{
			iNumLandAreas++;
		}
	}

	return iNumLandAreas;
}


//	--------------------------------------------------------------------------------
CvArea* CvMap::getArea(int iID)
{
	return m_areas.GetAt(iID);
}


//	--------------------------------------------------------------------------------
CvArea* CvMap::addArea()
{
	return m_areas.Add();
}


//	--------------------------------------------------------------------------------
void CvMap::deleteArea(int iID)
{
	m_areas.RemoveAt(iID);
}


//	--------------------------------------------------------------------------------
CvArea* CvMap::firstArea(int *pIterIdx, bool bRev)
{
	return !bRev ? m_areas.BeginIter(pIterIdx) : m_areas.EndIter(pIterIdx);
}


//	--------------------------------------------------------------------------------
CvArea* CvMap::nextArea(int *pIterIdx, bool bRev)
{
	return !bRev ? m_areas.NextIter(pIterIdx) : m_areas.PrevIter(pIterIdx);
}


//	--------------------------------------------------------------------------------
void CvMap::recalculateAreas()
{
	int iI;

	int iNumPlots = numPlots();
	for (iI = 0; iI < iNumPlots; iI++)
	{
		plotByIndexUnchecked(iI)->setArea(FFreeList::INVALID_INDEX);
	}

	m_areas.RemoveAll();

	calculateAreas();

	recalculateLandmasses();
}


//	--------------------------------------------------------------------------------
int CvMap::calculateInfluenceDistance(CvPlot *pSource, CvPlot *pDest, int iMaxRange, bool bCorrectButSlower)
{
	CvAStarNode* pNode;

	if (pSource == NULL || pDest == NULL)
	{
		return -1;
	}

	GC.getInfluenceFinder().SetData(&iMaxRange);
	if (GC.getInfluenceFinder().GeneratePath(pSource->getX(), pSource->getY(), pDest->getX(), pDest->getY(), 0, !bCorrectButSlower))
	{
		pNode = GC.getInfluenceFinder().GetLastNode();

		if (pNode != NULL)
		{
			return pNode->m_iKnownCost;
		}
	}

	return -1; // no passable path exists
}



//	--------------------------------------------------------------------------------
//
// read object from a stream
// used during load
//
void CvMap::Read(FDataStream& kStream)
{
	CvMapInitData defaultMapData;

	// Init data before load
	reset(&defaultMapData);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_iGridWidth;
	kStream >> m_iGridHeight;
	m_iGridSize = m_iGridHeight * m_iGridWidth;
	kStream >> m_iLandPlots;
	kStream >> m_iOwnedPlots;
	kStream >> m_iNumNaturalWonders;
	kStream >> m_iTopLatitude;
	kStream >> m_iBottomLatitude;

	kStream >> m_bWrapX;
	kStream >> m_bWrapY;

	kStream >> m_guid.Data1;
	kStream >> m_guid.Data2;
	kStream >> m_guid.Data3;
	ArrayWrapper<unsigned char> wrapm_guid(8, m_guid.Data4);
	kStream >> wrapm_guid;

#ifdef _GC_SUPPORT_CACHED_MAP_DATA
	ms_iGridW = m_iGridWidth;
	ms_iGridH = m_iGridHeight;
	ms_bWrapX = m_bWrapX;
	ms_bWrapY = m_bWrapY;
#endif

	CvAssertMsg((0 < GC.getNumResourceInfos()), "GC.getNumResourceInfos() is not greater than zero but an array is being allocated");
	if (uiVersion <= 3)
	{
		CvInfosSerializationHelper::ReadV0DataArray(kStream, m_paiNumResource, GC.getNumResourceInfos(), CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0ResourceTags));
		CvInfosSerializationHelper::ReadV0DataArray(kStream, m_paiNumResourceOnLand, GC.getNumResourceInfos(), CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0ResourceTags));
	}
	else
	{
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiNumResource, GC.getNumResourceInfos());
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiNumResourceOnLand, GC.getNumResourceInfos());
	}


	if (numPlots() > 0)
	{
		InitPlots();
		int iI;
		for (iI = 0; iI < numPlots(); iI++)
		{
			m_pMapPlots[iI].read(kStream);
		}
	}

	// call the read of the free list CvArea class allocations
	kStream >> m_areas;

	kStream >> m_landmasses;

	m_iAIMapHints = 0;
	if (uiVersion > 1)
	{
		kStream >> m_iAIMapHints;
	}

	setup();

	gDLL->DoMapSetup(numPlots());
}

//	--------------------------------------------------------------------------------
// save object to a stream
// used during save
//
void CvMap::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 4;
	kStream << uiVersion;

	kStream << m_iGridWidth;
	kStream << m_iGridHeight;
	kStream << m_iLandPlots;
	kStream << m_iOwnedPlots;
	kStream << m_iNumNaturalWonders;
	kStream << m_iTopLatitude;
	kStream << m_iBottomLatitude;
	kStream << m_bWrapX;
	kStream << m_bWrapY;

	kStream << m_guid.Data1;
	kStream << m_guid.Data2;
	kStream << m_guid.Data3;
	kStream << ArrayWrapper<const unsigned char>(8, m_guid.Data4);

	CvAssertMsg((0 < GC.getNumResourceInfos()), "GC.getNumResourceInfos() is not greater than zero but an array is being allocated");
	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes>(kStream, m_paiNumResource, GC.getNumResourceInfos());
	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes>(kStream, m_paiNumResourceOnLand, GC.getNumResourceInfos());

	int iI;
	for (iI = 0; iI < numPlots(); iI++)
	{
		m_pMapPlots[iI].write(kStream);
	}

	// call the read of the free list CvArea class allocations
	kStream << m_areas;
	//WriteStreamableFFreeListTrashArray(m_areas, kStream);

	kStream << m_landmasses;

	kStream << m_iAIMapHints;

}


//	--------------------------------------------------------------------------------
//
// used for loading WB maps
//
void CvMap::rebuild(int iGridW, int iGridH, int iTopLatitude, int iBottomLatitude, bool bWrapX, bool bWrapY, WorldSizeTypes eWorldSize, ClimateTypes eClimate, SeaLevelTypes eSeaLevel)
{
	CvMapInitData initData(iGridW, iGridH, iTopLatitude, iBottomLatitude, bWrapX, bWrapY);

	// Set init core data
	CvPreGame::setWorldSize(eWorldSize, true);
	CvPreGame::setClimate(eClimate);
	CvPreGame::setSeaLevel(eSeaLevel);

	// Init map
	init(&initData);
}


//////////////////////////////////////////////////////////////////////////
// Protected Functions...
//////////////////////////////////////////////////////////////////////////

//	--------------------------------------------------------------------------------
void CvMap::calculateAreas()
{
	CvPlot* pLoopPlot;
	CvArea* pArea;
	int iArea;
	int iI;

	for (iI = 0; iI < numPlots(); iI++)
	{
		pLoopPlot = plotByIndexUnchecked(iI);
		//gDLL->callUpdater();
		CvAssertMsg(pLoopPlot != NULL, "LoopPlot is not assigned a valid value");

		if(!pLoopPlot) continue;

		if (pLoopPlot->getArea() == FFreeList::INVALID_INDEX)
		{
			pArea = addArea();
			pArea->init(pArea->GetID(), pLoopPlot->isWater());

			pArea->SetMountains(pLoopPlot->isMountain());

			iArea = pArea->GetID();

			pLoopPlot->setArea(iArea);

			GC.getAreaFinder().GeneratePath(pLoopPlot->getX(), pLoopPlot->getY(), -1, -1, iArea);

			CvAreaBoundaries boundaries;
			boundaries.m_iEastEdge = pLoopPlot->getX();
			boundaries.m_iWestEdge = pLoopPlot->getX();
			boundaries.m_iNorthEdge = pLoopPlot->getY();
			boundaries.m_iSouthEdge = pLoopPlot->getY();
			pArea->setAreaBoundaries(boundaries);
		}

		// Update area boundaries if they've grown
		else
		{
			int iX, iY;
			CvAreaBoundaries boundaries = getArea(pLoopPlot->getArea())->getAreaBoundaries();
			iX = pLoopPlot->getX();
			iY = pLoopPlot->getY();

			if (iX < boundaries.m_iWestEdge)
			{
				boundaries.m_iWestEdge = iX;
			}
			else if (iX > boundaries.m_iEastEdge)
			{
				boundaries.m_iEastEdge = iX;
			}
			else if (iY < boundaries.m_iSouthEdge)
			{
				boundaries.m_iSouthEdge = iY;
			}
			else if (iY > boundaries.m_iNorthEdge)
			{
				boundaries.m_iNorthEdge = iY;
			}
			getArea(pLoopPlot->getArea())->setAreaBoundaries(boundaries);
		}
	}
}


//	--------------------------------------------------------------------------------
/// How many Natural Wonders are there in the world?
int CvMap::GetNumNaturalWonders() const
{
	return m_iNumNaturalWonders;
}

//	--------------------------------------------------------------------------------
/// Changes count on how many Natural Wonders are there in the world
void CvMap::ChangeNumNaturalWonders(int iChange)
{
	m_iNumNaturalWonders += iChange;
}

//	--------------------------------------------------------------------------------
/// Put Natural Wonders on the map - this could be a LUA script
void CvMap::DoPlaceNaturalWonders()
{
	FeatureTypes eNWFeature = NO_FEATURE;
	FeatureTypes eFeature;

	FeatureTypes featureFuji = NO_FEATURE;
	FeatureTypes featureEverest = NO_FEATURE;
	FeatureTypes featureReef = NO_FEATURE;
	FeatureTypes featureCrater = NO_FEATURE;
	FeatureTypes featureLake = NO_FEATURE;
	FeatureTypes featureMesa = NO_FEATURE;
	FeatureTypes featureVolcano = NO_FEATURE;

	int iNumFeatureInfos = GC.getNumFeatureInfos();
	int nwCount = 0;
	for (int iFeatureLoop = 0; iFeatureLoop < iNumFeatureInfos; iFeatureLoop++)
	{
		eFeature = (FeatureTypes) iFeatureLoop;
		CvFeatureInfo* feature = GC.getFeatureInfo(eFeature);
		if (feature && feature->IsNaturalWonder())
		{
			eNWFeature = eFeature;

			// hackery - this should be more data driven but the engine side isn't

			if ( strcmp(feature->GetType(), "FEATURE_EVEREST") == 0 )
			{
				featureEverest = eFeature;
			}
			else if ( strcmp(feature->GetType(), "FEATURE_CRATER") == 0 )
			{
				featureCrater = eFeature;
			}
			else if ( strcmp(feature->GetType(), "FEATURE_TITICACA") == 0 )
			{
				featureLake = eFeature;
			}
			else if ( strcmp(feature->GetType(), "FEATURE_FUJI") == 0 )
			{
				featureFuji = eFeature;
			}
			else if ( strcmp(feature->GetType(), "FEATURE_MESA") == 0 )
			{
				featureMesa = eFeature;
			}
			else if ( strcmp(feature->GetType(), "FEATURE_REEF") == 0 )
			{
				featureReef = eFeature;
			}
			else if ( strcmp(feature->GetType(), "FEATURE_VOLCANO") == 0 )
			{
				featureVolcano = eFeature;
			}

			nwCount++;
		}
	}

	// No Natural Wonder Feature, abort!
	if (eNWFeature == NO_FEATURE)
	{
		return;
	}

	int iNumNaturalWondersToAdd = getWorldInfo().getNumNaturalWonders();

	int iCoastDistance = -1;	// Should be in XML
	int iAnotherNWDistance = 5;	// Should be in XML

	int iCount = 0;

	int iPlotRand;
	CvPlot* pRandPlot;

	int iPlotLoopX;
	int iPlotLoopY;
	CvPlot* pLoopPlot;

	int iNumMapPlots = numPlots();

	// Determine the max number of NWs we can have in a single Area
	CvArea* pLoopArea;
	int iLoop;
	int iNumValidAreas = 0;

	int iMaxLatitudeForNW = 65;

	int iBottomLatitude;

	for (pLoopArea = firstArea(&iLoop); pLoopArea != NULL; pLoopArea = nextArea(&iLoop))
	{
		if (!pLoopArea->isWater())
		{
			if (!pLoopArea->IsMountains())
			{
				iBottomLatitude = pLoopArea->GetAreaMinLatitude();

				if (iBottomLatitude <= iMaxLatitudeForNW)
				{
					iNumValidAreas++;
				}
			}
		}
	}

	int iMaxNWsOneArea = 2;

	// If we have more Natural Wonders than we have Areas then we have to handle things differently - more can be placed in one Area than normal
	if (iNumNaturalWondersToAdd > iNumValidAreas)
	{
		iMaxNWsOneArea = iNumNaturalWondersToAdd - iNumValidAreas + 1;
	}

	while (iNumNaturalWondersToAdd > 0 && iCount < 10000)
	{
		iCount++;

		iPlotRand = GC.getGame().getJonRandNum(iNumMapPlots, "Randomly Placing Natural Wonders");

		pRandPlot = plotByIndex(iPlotRand);

		// No Water Tiles
		if (pRandPlot->isWater())
		{
			continue;
		}

		// No River Tiles either (don't want them carved up)
		if (pRandPlot->isRiver())
		{
			continue;
		}

		// No Tiles with Resources
		if (pRandPlot->getResourceType() != NO_RESOURCE)
		{
			continue;
		}

		// No Tiles with Improvements (Goody Huts)
		if (pRandPlot->getImprovementType() != NO_IMPROVEMENT)
		{
			continue;
		}

		// Can't have too many NWs in one Area
		if (getArea(pRandPlot->getArea())->GetNumNaturalWonders() >= iMaxNWsOneArea)
		{
			continue;
		}

		// Has to be in the central band of the map.  If the latitude is too extreme we might end up on a islelet surrounded by Ice or something
		if (pRandPlot->getLatitude() > /*65*/ iMaxLatitudeForNW)
		{
			continue;
		}

		// Can't be a starting Plot for ANY player (including Minors)
		if (pRandPlot->isStartingPlot())
		{
			continue;
		}

		bool bValid;

		// Does it have to be close to the coast?
		if (iCoastDistance != -1)
		{
			bValid = false;

			for (iPlotLoopX = -iCoastDistance; iPlotLoopX <= iCoastDistance; iPlotLoopX++)
			{
				for (iPlotLoopY = -iCoastDistance; iPlotLoopY <= iCoastDistance; iPlotLoopY++)
				{
					pLoopPlot = plotXYWithRangeCheck(pRandPlot->getX(), pRandPlot->getY(), iPlotLoopX, iPlotLoopY, iCoastDistance);

					if (pLoopPlot != NULL)
					{
						if (pLoopPlot->isWater())
						{
							if (!pLoopPlot->isLake())
							{
								// Found a Plot within 2 plots of "the Ocean"
								bValid = true;
								break;
							}
						}
					}
				}

				if (bValid)
				{
					break;
				}
			}

			// Plot not within 2 of Ocean
			if (!bValid)
			{
				continue;
			}
		}

		bValid = true;

		// Can't be too close to another Natural Wonder
		for (iPlotLoopX = -iAnotherNWDistance; iPlotLoopX <= iAnotherNWDistance; iPlotLoopX++)
		{
			for (iPlotLoopY = -iAnotherNWDistance; iPlotLoopY <= iAnotherNWDistance; iPlotLoopY++)
			{
				pLoopPlot = plotXYWithRangeCheck(pRandPlot->getX(), pRandPlot->getY(), iPlotLoopX, iPlotLoopY, iAnotherNWDistance);

				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->IsNaturalWonder())
					{
						// Found a NW too close
						bValid = false;
						break;
					}
				}
			}

			if (!bValid)
			{
				break;
			}
		}

		// Natural Wonder too close
		if (!bValid)
		{
			continue;
		}

		// see if we can add the volcano
		if (featureVolcano != NO_FEATURE)
		{
			if (!pRandPlot->isAdjacentToLand())
			{
				pRandPlot->setPlotType(PLOT_MOUNTAIN);
				pRandPlot->setFeatureType(featureVolcano);
				iNumNaturalWondersToAdd--;
				featureVolcano = NO_FEATURE;	// Prevent this one from being placed again
				continue;
			}
		}

		// see if we can add the reef
		if (featureReef != NO_FEATURE)
		{
			if (!pRandPlot->isMountain() && pRandPlot->isAdjacentToShallowWater())
			{
				if (pRandPlot->area()->getNumTiles() == 2)
				{
					pRandPlot->setPlotType(PLOT_MOUNTAIN);
					pRandPlot->setFeatureType(featureReef);
					for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
					{
						CvPlot* pAdjacentPlot = plotDirection(pRandPlot->getX(), pRandPlot->getY(), ((DirectionTypes)iI));
						if (pAdjacentPlot != NULL)
						{
							if (!(pAdjacentPlot->isWater()))
							{
								pAdjacentPlot->setPlotType(PLOT_MOUNTAIN);
								pAdjacentPlot->setFeatureType(featureReef);
								break;
							}
						}
					}
					iNumNaturalWondersToAdd--;
					featureReef = NO_FEATURE;	// Prevent this one from being placed again
					continue;
				}
			}
		}

		// see if we can add the lake
		if (featureLake != NO_FEATURE)
		{
			if (pRandPlot->isHills())
			{
				int iHillCount = 0;
				// if we have a couple of hills nearby already
				for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
				{
					CvPlot* pAdjacentPlot = plotDirection(pRandPlot->getX(), pRandPlot->getY(), ((DirectionTypes)iI));
					if (pAdjacentPlot != NULL)
					{
						if ((pAdjacentPlot->isHills()))
						{
							iHillCount++;
						}
					}
				}
				if (iHillCount > 2)
				{
					// we are stamping the terrain
					for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
					{
						CvPlot* pAdjacentPlot = plotDirection(pRandPlot->getX(), pRandPlot->getY(), ((DirectionTypes)iI));
						if (pAdjacentPlot != NULL)
						{
							pAdjacentPlot->setPlotType(PLOT_HILLS);
							pAdjacentPlot->setTerrainType((TerrainTypes)(GC.getLAND_TERRAIN()));
						}
					}
					pRandPlot->setPlotType(PLOT_MOUNTAIN);
					pRandPlot->setFeatureType(featureLake);
					iNumNaturalWondersToAdd--;
					featureLake = NO_FEATURE;	// Prevent this one from being placed again
					continue;
				}
			}
		}

		// see if we can add Everest
		if (featureEverest != NO_FEATURE)
		{
			if (pRandPlot->isMountain() && GC.getGame().getMapRandNum(100, "Natural Wonder") > 75)
			{
				// we are stamping the terrain
				for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
				{
					CvPlot* pAdjacentPlot = plotDirection(pRandPlot->getX(), pRandPlot->getY(), ((DirectionTypes)iI));
					if (pAdjacentPlot != NULL)
					{
						if (iI & 1)
						{
							pAdjacentPlot->setPlotType(PLOT_MOUNTAIN);
						}
						else
						{
							pAdjacentPlot->setPlotType(PLOT_HILLS);
							pAdjacentPlot->setTerrainType((TerrainTypes)(GC.getLAND_TERRAIN()));
						}
					}
				}
				pRandPlot->setPlotType(PLOT_MOUNTAIN);
				pRandPlot->setFeatureType(featureEverest);
				iNumNaturalWondersToAdd--;
				featureEverest = NO_FEATURE;	// Prevent this one from being placed again
				continue;
			}
		}

		// randomly pick one of the other three - but not if this is a coastal plot, because they look terrible there
		if (pRandPlot->isCoastalLand())
		{
			continue;
		}

		int iDice = GC.getGame().getMapRandNum(100, "Natural Wonder");
		if (iDice > 66)
		{
			if (featureCrater != NO_FEATURE)
			{
				pRandPlot->setPlotType(PLOT_MOUNTAIN);
				pRandPlot->setFeatureType(featureCrater);
				iNumNaturalWondersToAdd--;
				featureCrater = NO_FEATURE;	// Prevent this one from being placed again
				continue;
			}
		}
		else if (iDice > 25)
		{
			if (featureFuji != NO_FEATURE)
			{
				pRandPlot->setPlotType(PLOT_MOUNTAIN);
				pRandPlot->setFeatureType(featureFuji);
				iNumNaturalWondersToAdd--;
				featureFuji = NO_FEATURE;	// Prevent this one from being placed again
				continue;
			}
		}
		else
		{
			if (featureMesa != NO_FEATURE)
			{
				pRandPlot->setPlotType(PLOT_MOUNTAIN);
				pRandPlot->setFeatureType(featureMesa);
				iNumNaturalWondersToAdd--;
				featureMesa = NO_FEATURE;	// Prevent this one from being placed again
				continue;
			}
		}
	}
}

//	--------------------------------------------------------------------------------
void CvMap::DefaultContinentStamper()
{
	const int iNumContinentTypes = 4;
	const int iMinimumThresholdToBeAContinentIMHO = 10;

	int iNumLargeContinents = 0;
	int iLargeContinentsID[iNumContinentTypes] = {-1,-1,-1,-1};
	int iLargeContinentsSize[iNumContinentTypes] = {0,0,0,0};
	int iLargeContinentsArtStyle[iNumContinentTypes] = {0,0,0,0};
	int iLargeContinentsX[iNumContinentTypes] = {0,0,0,0};
	int iLargeContinentsY[iNumContinentTypes] = {0,0,0,0};

	recalculateLandmasses();

	CvLandmass* pLoopMass;
	int iLoop;

	// find the 4 (because that is how many art styles we have) biggest continents - if there aren't 4, oh well
	for (pLoopMass = firstLandmass(&iLoop); pLoopMass != NULL; pLoopMass = nextLandmass(&iLoop))
	{
		if (!pLoopMass->isWater())
		{
			int iBestContinent;
			int iNumTiles = pLoopMass->getNumTiles();
			for (iBestContinent=0;iBestContinent<iNumContinentTypes;iBestContinent++)
			{
				if (iNumTiles >= iMinimumThresholdToBeAContinentIMHO && iNumTiles > iLargeContinentsSize[iBestContinent])
				{
					// push the rest of the tiles down one and store this
					for (int iloop=iNumContinentTypes-1;iloop>iBestContinent;iloop--)
					{
						iLargeContinentsID[iloop] = iLargeContinentsID[iloop-1];
						iLargeContinentsSize[iloop] = iLargeContinentsSize[iloop-1];
					}
					iLargeContinentsID[iBestContinent] = pLoopMass->GetID();
					iLargeContinentsSize[iBestContinent] = iNumTiles;
					break;
				}
			}
		}
	}

	// for each of these large continents give them unique art style (randomly)
	shuffleArray(iLargeContinentsArtStyle, 4, GC.getGame().getMapRand());
	for (int i=0; i < iNumContinentTypes; i++)
	{
		iLargeContinentsArtStyle[i]++;
	}

	for (int i=0; i < iNumContinentTypes; i++)
	{
		if (iLargeContinentsID[i] != -1)
			iNumLargeContinents++;
	}

	for (int i=0; i < iNumLargeContinents; i++)
	{
		CvLandmass* pLandmass = getLandmass(iLargeContinentsID[i]);
		iLargeContinentsX[i] = pLandmass->GetCentroidX();
		iLargeContinentsY[i] = pLandmass->GetCentroidY();
	}

	// for all landmasses in the game
	for (pLoopMass = firstLandmass(&iLoop); pLoopMass != NULL; pLoopMass = nextLandmass(&iLoop))
	{
		int iX = pLoopMass->GetCentroidX();
		int iY = pLoopMass->GetCentroidY();
		// if water continent type = 0
		if (pLoopMass->isWater())
		{
			pLoopMass->SetContinentType(0);
		}
		else // if land
		{
			// continent type = continent type of the closest of the "big four"
			int iBestPlotDistance = 999999;
			for (int i=0; i < iNumLargeContinents; i++)
			{
				int iPlotDistance = plotDistance(iX, iY, iLargeContinentsX[i], iLargeContinentsY[i]);
				if (iPlotDistance < iBestPlotDistance)
				{
					iBestPlotDistance = iPlotDistance;
					pLoopMass->SetContinentType(iLargeContinentsArtStyle[i]);
				}
			}
		}
	}

	// now stamp all plots with the relevant data
	for (int iI = 0; iI < numPlots(); iI++)
	{
		CvPlot* pLoopPlot = plotByIndexUnchecked(iI);
		CvLandmass* pLandmass = getLandmass(pLoopPlot->getLandmass());
		if (pLandmass != NULL)
		{
			pLoopPlot->SetContinentType(pLandmass->GetContinentType());
		}
	}
}

//	--------------------------------------------------------------------------------
int CvMap::getIndexAfterLastLandmass()
{
	return m_landmasses.GetIndexAfterLast();
}


//	--------------------------------------------------------------------------------
int CvMap::getNumLandmasses()
{
	return m_landmasses.GetCount();
}


//	--------------------------------------------------------------------------------
int CvMap::getNumLandLandmasses()
{
	CvLandmass* pLoopLandmass;
	int iNumLandLandmasses;
	int iLoop;

	iNumLandLandmasses = 0;

	for(pLoopLandmass = GC.getMap().firstLandmass(&iLoop); pLoopLandmass != NULL; pLoopLandmass = GC.getMap().nextLandmass(&iLoop))
	{
		if (!(pLoopLandmass->isWater()))
		{
			iNumLandLandmasses++;
		}
	}

	return iNumLandLandmasses;
}


//	--------------------------------------------------------------------------------
CvLandmass* CvMap::getLandmass(int iID)
{
	return m_landmasses.GetAt(iID);
}


//	--------------------------------------------------------------------------------
CvLandmass* CvMap::addLandmass()
{
	return m_landmasses.Add();
}


//	--------------------------------------------------------------------------------
void CvMap::deleteLandmass(int iID)
{
	m_landmasses.RemoveAt(iID);
}


//	--------------------------------------------------------------------------------
CvLandmass* CvMap::firstLandmass(int *pIterIdx, bool bRev)
{
	return !bRev ? m_landmasses.BeginIter(pIterIdx) : m_landmasses.EndIter(pIterIdx);
}


//	--------------------------------------------------------------------------------
CvLandmass* CvMap::nextLandmass(int *pIterIdx, bool bRev)
{
	return !bRev ? m_landmasses.NextIter(pIterIdx) : m_landmasses.PrevIter(pIterIdx);
}

//	--------------------------------------------------------------------------------
void CvMap::recalculateLandmasses()
{
	int iNumPlots = numPlots();
	for (int iI = 0; iI < iNumPlots; iI++)
	{
		plotByIndexUnchecked(iI)->setLandmass(FFreeList::INVALID_INDEX);
	}

	m_landmasses.RemoveAll();

	calculateLandmasses();
}

//	--------------------------------------------------------------------------------
void CvMap::calculateLandmasses()
{
	CvPlot* pLoopPlot;
	CvLandmass* pLandmass;
	int iLandmassID;

	CvAStar& thePathfinder = GC.getAreaFinder();

	// change the area pathfinder to use these funcs instead
	thePathfinder.SetValidFunc(LandmassValid);
	thePathfinder.SetNotifyListFunc(JoinLandmass);

	for (int iI = 0; iI < numPlots(); iI++)
	{
		pLoopPlot = plotByIndexUnchecked(iI);
		if (pLoopPlot->getLandmass() == FFreeList::INVALID_INDEX)
		{
			pLandmass = addLandmass();
			pLandmass->init(pLandmass->GetID(), pLoopPlot->isWater());

			iLandmassID = pLandmass->GetID();

			pLoopPlot->setLandmass(iLandmassID);

			thePathfinder.GeneratePath(pLoopPlot->getX(), pLoopPlot->getY(), -1, -1, iLandmassID);
		}
	}
	thePathfinder.SetValidFunc(AreaValid);
	thePathfinder.SetNotifyListFunc(JoinArea);

	// KWG: Rebuild the yields here.  Yes, this is called during the landmass rebuild process if the landmass' 'lake' field changes, but
	//      there is a problem with that. The yield bonus for a lake is dependent on the proximity to a plot that is a lake, and not the general landmass
	//		'lake' flag.  During recalculation, a neighboring plot's 'lake' flag may not be set yet because it is in a landmass that has yet to be calculated
	//		resulting in the wrong yield being applied to a plot.
	updateYield();
}

//	---------------------------------------------------------------------------
int CvMap::Validate()
{
	int iErrors = 0;
	for (int iI = 0; iI < numPlots(); iI++)
	{
		CvPlot* pLoopPlot = plotByIndexUnchecked(iI);
		iErrors |= pLoopPlot->Validate(*this);
	}

	return iErrors;
}

//	--------------------------------------------------------------------------------
void CvMap::ChangeAIMapHint(int iMapHint)
{
	m_iAIMapHints |= iMapHint;
}

//	--------------------------------------------------------------------------------
int CvMap::GetAIMapHint()
{
	return m_iAIMapHints;
}
