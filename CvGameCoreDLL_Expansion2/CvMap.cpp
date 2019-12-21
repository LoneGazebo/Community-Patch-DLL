/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

//	AUTHOR:  Soren Johnson
//	PURPOSE: Game map class

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
typedef struct tagMSG* LPMSG;
#include <objbase.h>

// must be included after all other headers
#include "LintFree.h"

//////////////////////////////////////////////////////////////////////////////
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
	if(iChange != 0)
	{
		bool bOldLake = isLake();

		m_iNumTiles = (m_iNumTiles + iChange);
		CvAssert(m_iNumTiles >= 0);

		if(bOldLake != isLake())
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
	if(m_iNumTiles > 0)
	{
		return m_iCentroidX / m_iNumTiles;

	}
	return -1;
}

//	--------------------------------------------------------------------------------
int CvLandmass::GetCentroidY()
{
	if(m_iNumTiles > 0)
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
	MOD_SERIALIZE_INIT_READ(kStream);

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
	MOD_SERIALIZE_INIT_WRITE(kStream);

	kStream << m_iID;
	kStream << m_iNumTiles;

	kStream << m_iCentroidX;
	kStream << m_iCentroidY;

	kStream << m_bWater;

	kStream << m_cContinentType;

}


//	--------------------------------------------------------------------------------
FDataStream& operator<<(FDataStream& saveTo, const CvLandmass& readFrom)
{
	readFrom.write(saveTo);
	return saveTo;
}

//	--------------------------------------------------------------------------------
FDataStream& operator>>(FDataStream& loadFrom, CvLandmass& writeTo)
{
	writeTo.read(loadFrom);
	return loadFrom;
}

static uint sgCvMapInstanceCount = 0;
//////////////////////////////////////////////////////////////////////////////

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
#if defined(MOD_BALANCE_CORE)
	m_pPlotNeighbors = NULL;
	memset(m_apShuffledNeighbors,0,sizeof(CvPlot*)*6);
#endif

	//memory slabs to be shared between all the plots
	m_pYields = NULL;
	m_pPlayerCityRadiusCount = NULL;
	m_pVisibilityCount = NULL;
	m_pRevealedOwner = NULL;
	m_pRevealed = NULL;
	m_pRevealedImprovementType = NULL;
	m_pRevealedRouteType = NULL;
	m_pResourceForceReveal = NULL;
#if defined(MOD_BALANCE_CORE)
	m_pIsImpassable = NULL;
	m_pIsStrategic = NULL;
#endif

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
	int iNumPlots = numPlots();
	m_pMapPlots = FNEW(CvPlot[iNumPlots], c_eCiv5GameplayDLL, 0);

	//have to include barbarian here ...
	int iNumTeams = MAX_TEAMS;

	//allocate all the memory we need up front
	m_pYields = FNEW(uint8[NUM_YIELD_TYPES*iNumPlots], c_eCiv5GameplayDLL, 0);
	memset(m_pYields, 0, NUM_YIELD_TYPES*iNumPlots*sizeof(uint8));
	m_pPlayerCityRadiusCount = FNEW(uint8[iNumTeams*iNumPlots], c_eCiv5GameplayDLL, 0);
	memset(m_pPlayerCityRadiusCount, 0, iNumTeams*iNumPlots*sizeof(uint8));
	m_pVisibilityCount = FNEW(uint8[iNumTeams*iNumPlots], c_eCiv5GameplayDLL, 0);
	memset(m_pVisibilityCount, 0, iNumTeams*iNumPlots *sizeof(uint8));
	m_pVisibilityCountThisTurnMax = FNEW(uint8[iNumTeams*iNumPlots], c_eCiv5GameplayDLL, 0);
	memset(m_pVisibilityCountThisTurnMax, 0, iNumTeams*iNumPlots * sizeof(uint8));

	m_pRevealedOwner = FNEW(char[iNumTeams*iNumPlots], c_eCiv5GameplayDLL, 0);
	memset(m_pRevealedOwner, -1, iNumTeams*iNumPlots *sizeof(char));
	m_pRevealed = FNEW(bool[iNumTeams*iNumPlots], c_eCiv5GameplayDLL, 0);
	memset(m_pRevealed, 0, iNumTeams*iNumPlots *sizeof(bool));
	m_pRevealedImprovementType = FNEW(char[iNumTeams*iNumPlots], c_eCiv5GameplayDLL, 0);
	memset(m_pRevealedImprovementType, 0, iNumTeams*iNumPlots *sizeof(char));
	m_pRevealedRouteType = FNEW(char[iNumTeams*iNumPlots], c_eCiv5GameplayDLL, 0);
	memset(m_pRevealedRouteType, 0, iNumTeams*iNumPlots *sizeof(char));
	m_pResourceForceReveal = FNEW(bool[iNumTeams*iNumPlots], c_eCiv5GameplayDLL, 0);
	memset(m_pResourceForceReveal, 0, iNumTeams*iNumPlots *sizeof(bool));
#if defined(MOD_BALANCE_CORE)
	m_pIsImpassable = FNEW(bool[iNumTeams*iNumPlots], c_eCiv5GameplayDLL, 0);
	memset(m_pIsImpassable, 0, iNumTeams*iNumPlots *sizeof(bool));

	m_pIsStrategic = FNEW(bool[iNumTeams*iNumPlots], c_eCiv5GameplayDLL, 0);
	memset(m_pIsStrategic, 0, iNumTeams*iNumPlots *sizeof(bool));
#endif

	uint8* pYields = m_pYields;
	uint8* pPlayerCityRadiusCount = m_pPlayerCityRadiusCount;
	uint8* pVisibilityCount = m_pVisibilityCount;
	uint8* pVisibilityCountThisTurnMax = m_pVisibilityCountThisTurnMax;
	char* pRevealedOwner = m_pRevealedOwner;
	char* pRevealedImprovementType = m_pRevealedImprovementType;
	char* pRevealedRouteType = m_pRevealedRouteType;
	bool* pResourceForceReveal = m_pResourceForceReveal;
#if defined(MOD_BALANCE_CORE)
	bool* pIsImpassable = m_pIsImpassable;
	bool* pIsStrategic = m_pIsStrategic;
#endif

	for(int i = 0; i < iNumPlots; i++)
	{
		m_pMapPlots[i].m_aiYield = pYields;
		m_pMapPlots[i].m_aiPlayerCityRadiusCount = pPlayerCityRadiusCount;
		m_pMapPlots[i].m_aiVisibilityCount = pVisibilityCount;
		m_pMapPlots[i].m_aiVisibilityCountThisTurnMax = pVisibilityCountThisTurnMax;
		m_pMapPlots[i].m_aiRevealedOwner = pRevealedOwner;
		m_pMapPlots[i].m_aeRevealedImprovementType = pRevealedImprovementType;
		m_pMapPlots[i].m_aeRevealedRouteType = pRevealedRouteType;
		m_pMapPlots[i].m_abResourceForceReveal = pResourceForceReveal;
#if defined(MOD_BALANCE_CORE)
		m_pMapPlots[i].m_abIsImpassable = pIsImpassable;
		m_pMapPlots[i].m_abStrategicRoute = pIsStrategic;
#endif

		pYields					+= NUM_YIELD_TYPES;
		pPlayerCityRadiusCount  += iNumTeams;
		pVisibilityCount		+= iNumTeams;
		pVisibilityCountThisTurnMax += iNumTeams;
		pRevealedOwner			+= iNumTeams;
		pRevealedImprovementType+= iNumTeams;
		pRevealedRouteType		+= iNumTeams;
		pResourceForceReveal	+= iNumTeams;
#if defined(MOD_BALANCE_CORE)
		pIsImpassable			+= iNumTeams;
		m_pIsStrategic			+= iNumTeams;
#endif

	}

	m_kPlotManager.Init(getGridWidth(), getGridHeight());

#if defined(MOD_BALANCE_CORE)
	//this will be used for fast lookup of neighbors
	//the trick is that NO_DIRECTION is not -1 but NUM_DIRECTION_TYPES+1
	//therefore we need to allocate (NUM_DIRECTION_TYPES+2) pointers per plot
	m_pPlotNeighbors = FNEW(CvPlot*[iNumPlots*(NUM_DIRECTION_TYPES + 2)], c_eCiv5GameplayDLL, 0);
	memset(m_pPlotNeighbors, 0, iNumPlots*(NUM_DIRECTION_TYPES + 2)*sizeof(CvPlot*));
	PrecalcNeighbors();
#endif
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
	m_areas.RemoveAll();
	m_landmasses.RemoveAll();

	//--------------------------------
	// Init non-saved data
	setup();

	//--------------------------------
	// Init other game data
	InitPlots();

	int iW = getGridWidth();
	int iH = getGridHeight();
#if defined(MOD_BALANCE_CORE)
	for(iY = 0; iY < iH; iY++)
	{
		for(iX = 0; iX < iW; iX++)
		{
			plotUnchecked(iX, iY)->init(iX, iY);
		}
	}
}

void CvMap::PrecalcNeighbors()
{
	int iW = getGridWidth();
	int iH = getGridHeight();

	CvPlot** pNeighbors = m_pPlotNeighbors;
	if (!pNeighbors)
		return;

	int iNX, iNY, iHX, iHY;

	for(int iY = 0; iY < iH; iY++)
	{
		for(int iX = 0; iX < iW; iX++)
		{
			//set up neighborhood indices
			for(int iDirection = 0; iDirection < NUM_DIRECTION_TYPES+2; iDirection++)
			{
				// convert to hex-space and back
				iHX = xToHexspaceX(iX , iY);
				iHY = iY;
				iHX += GC.getPlotDirectionX()[iDirection];
				iHY += GC.getPlotDirectionY()[iDirection];

				iNX = hexspaceXToX(iHX, iHY);
				iNY = iHY;
				iNX = coordRange(iNX, iW, m_bWrapX);
				iNY = coordRange(iNY, iH, m_bWrapY);

				pNeighbors[iDirection] = isPlot(iNX, iNY) ? (m_pMapPlots+plotNum(iNX,iNY)) : NULL;
			}

			pNeighbors += (NUM_DIRECTION_TYPES+2);
		}
	}
}
#else
	for(iX = 0; iX < iW; iX++)
	{
		for(iY = 0; iY < iH; iY++)
		{
			plotUnchecked(iX, iY)->init(iX, iY);
		}
	}
}
#endif

//	--------------------------------------------------------------------------------
CvPlot** CvMap::getNeighborsShuffled(const CvPlot* pPlot)
{
	if (!pPlot)
		return NULL;

	int aiShuffle[3][6] = {
		{ 4, 5, 2, 1, 3, 0 },
		{ 3, 0, 4, 1, 2, 5 },
		{ 1, 2, 4, 5, 0, 3 } };

	int iShuffleType = GC.getGame().getSmallFakeRandNum(3, *pPlot);
	int iBaseIndex = plotNum(pPlot->getX(), pPlot->getY())*(NUM_DIRECTION_TYPES + 2);

	for (int i = 0; i < NUM_DIRECTION_TYPES; i++)
		m_apShuffledNeighbors[i] = m_pPlotNeighbors[iBaseIndex + aiShuffle[iShuffleType][i]];

	return m_apShuffledNeighbors;
}

//	--------------------------------------------------------------------------------
void CvMap::uninit()
{
	SAFE_DELETE_ARRAY(m_paiNumResource);
	SAFE_DELETE_ARRAY(m_paiNumResourceOnLand);

	SAFE_DELETE_ARRAY(m_pMapPlots);
	SAFE_DELETE_ARRAY(m_pYields);
	SAFE_DELETE_ARRAY(m_pPlayerCityRadiusCount);
	SAFE_DELETE_ARRAY(m_pVisibilityCount);
	SAFE_DELETE_ARRAY(m_pRevealedOwner);
	SAFE_DELETE_ARRAY(m_pRevealed);
	SAFE_DELETE_ARRAY(m_pRevealedImprovementType);
	SAFE_DELETE_ARRAY(m_pRevealedRouteType);
	SAFE_DELETE_ARRAY(m_pResourceForceReveal);

#if defined(MOD_BALANCE_CORE)
	SAFE_DELETE_ARRAY(m_pIsImpassable);
	SAFE_DELETE_ARRAY(m_pPlotNeighbors);
	SAFE_DELETE_ARRAY(m_pIsStrategic);
#endif

	m_iGridWidth = 0;
	m_iGridHeight = 0;
	m_iGridSize = 0;
	m_iLandPlots = 0;
	m_iOwnedPlots = 0;
	m_iNumNaturalWonders = 0;

	m_kPlotManager.Uninit();
}

//	--------------------------------------------------------------------------------
// FUNCTION: reset()
// Initializes data members that are serialized.
void CvMap::reset(CvMapInitData* pInitInfo)
{
	//--------------------------------
	// Uninit class
	uninit();

	m_iAIMapHints = 0;
	//
	// set grid size
	// initially set in terrain cell units
	//
	m_iGridWidth = (CvPreGame::worldSize() != NO_WORLDSIZE) ?  getWorldInfo().getGridWidth(): 0;
	m_iGridHeight = (CvPreGame::worldSize() != NO_WORLDSIZE) ? getWorldInfo().getGridHeight(): 0;

	// allow grid size override
	if(pInitInfo)
	{
		m_iGridWidth	= pInitInfo->m_iGridW;
		m_iGridHeight	= pInitInfo->m_iGridH;
	}

	m_iGridSize = m_iGridHeight * m_iGridWidth;

	m_iLandPlots = 0;
	m_iOwnedPlots = 0;
	m_iNumNaturalWonders = 0;

	if(pInitInfo)
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

	CoCreateGuid(&m_guid);

	if(pInitInfo)
	{
		m_bWrapX = pInitInfo->m_bWrapX;
		m_bWrapY = pInitInfo->m_bWrapY;
	}

	int iNumResourceInfos = GC.getNumResourceInfos();
	if(iNumResourceInfos)
	{
		CvAssertMsg((0 < GC.getNumResourceInfos()), "GC.getNumResourceInfos() is not greater than zero but an array is being allocated in CvMap::reset");
		CvAssertMsg(m_paiNumResource==NULL, "mem leak m_paiNumResource");
		m_paiNumResource = FNEW(int[iNumResourceInfos], c_eCiv5GameplayDLL, 0);
		CvAssertMsg(m_paiNumResourceOnLand==NULL, "mem leak m_paiNumResourceOnLand");
		m_paiNumResourceOnLand = FNEW(int[iNumResourceInfos], c_eCiv5GameplayDLL, 0);
		for(int iI = 0; iI < iNumResourceInfos; iI++)
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
	GC.InitializePathfinders(getGridWidth(), getGridHeight(), isWrapX(), isWrapY());
}

//////////////////////////////////////
// graphical only setup
//////////////////////////////////////
void CvMap::setupGraphical()
{
	if(!GC.IsGraphicsInitialized())
		return;

	if(m_pMapPlots != NULL)
	{
		for(int iI = 0; iI < numPlots(); iI++)
		{
			plotByIndexUnchecked(iI)->setupGraphical();
		}
	}
}


//	--------------------------------------------------------------------------------
void CvMap::erasePlots()
{
	for(int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndexUnchecked(iI)->erase(true/*bEraseUnitsIfWater*/);
	}
}


//	--------------------------------------------------------------------------------
void CvMap::setRevealedPlots(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly)
{
	for(int iI = 0; iI < numPlots(); iI++)
	{
#if defined(MOD_API_EXTENSIONS)
		plotByIndexUnchecked(iI)->setRevealed(eTeam, bNewValue, NULL, bTerrainOnly);
#else
		plotByIndexUnchecked(iI)->setRevealed(eTeam, bNewValue, bTerrainOnly);
#endif
	}
}


//	--------------------------------------------------------------------------------
void CvMap::setAllPlotTypes(PlotTypes ePlotType)
{

	for(int i=0; i<numPlots(); i++)
	{
		plotByIndexUnchecked(i)->setPlotType(ePlotType, false, false);
	}

	recalculateAreas();
}


//	--------------------------------------------------------------------------------
void CvMap::doTurn()
{
	m_plotPopupCount.clear();

	for(int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndexUnchecked(iI)->doTurn();
	}
}


//	--------------------------------------------------------------------------------
void CvMap::updateFog()
{
	for(int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndexUnchecked(iI)->updateFog();
	}
}


//	--------------------------------------------------------------------------------
void CvMap::updateDeferredFog()
{
	for(DeferredFogPlots::iterator it=m_vDeferredFogPlots.begin(); it!= m_vDeferredFogPlots.end(); ++it)
		(*it)->updateFog();

	m_vDeferredFogPlots.clear();
}


//	--------------------------------------------------------------------------------
void CvMap::updateVisibility()
{
	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndexUnchecked(iI)->updateVisibility();
	}
}

//	--------------------------------------------------------------------------------
void CvMap::updateLayout(bool bDebug)
{
	for(int iI = 0; iI < numPlots(); iI++)
	{
		CvPlot* pThisPlot = plotByIndexUnchecked(iI);
		if(pThisPlot && pThisPlot->isLayoutDirty())
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
	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndexUnchecked(iI)->updateSight(bIncrement);
	}
}


//	--------------------------------------------------------------------------------
void CvMap::updateCenterUnit()
{
	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndexUnchecked(iI)->updateCenterUnit();
	}
}


//	--------------------------------------------------------------------------------
void CvMap::updateOwningCity(CvPlot* pPlot, int iRange)
{
	if(pPlot && iRange > 0)
	{
		for(int iX = -iRange; iX <= iRange; iX++)
		{
			for(int iY = -iRange; iY <= iRange; iY++)
			{
				CvPlot* pLoopPlot = plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iX, iY, iRange);
				if(pLoopPlot)
				{
					pLoopPlot->updateOwningCity();
				}
			}
		}
	}
	else
	{
		for(int iI = 0; iI < numPlots(); iI++)
		{
			plotByIndexUnchecked(iI)->updateOwningCity();
		}
	}
}


//	--------------------------------------------------------------------------------
void CvMap::updateYield()
{
	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndexUnchecked(iI)->updateWaterFlags();
		plotByIndexUnchecked(iI)->updateYield();
	}
}

//	--------------------------------------------------------------------------------
//	Update the adjacency cache values
void CvMap::updateAdjacency()
{
	GC.getMap().ClearPlotsAtRange(NULL);
}

//	--------------------------------------------------------------------------------
void CvMap::verifyUnitValidPlot()
{
	for (int iI = 0; iI < numPlots(); iI++)
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

	while(iCount < iTimeout)
	{
		iCount++;
		pTestPlot = plotCheckInvalid(GC.getGame().getJonRandNum(getGridWidth(), "Rand Plot Width"), GC.getGame().getJonRandNum(getGridHeight(), "Rand Plot Height"));

		CvAssertMsg(pTestPlot != NULL, "TestPlot is not assigned a valid value");

		if(!pTestPlot) continue;

		if((iArea == -1) || (pTestPlot->getArea() == iArea))
		{
			bValid = true;

			if(bValid)
			{
				if(iMinUnitDistance != -1)
				{
					for(iDX = -(iMinUnitDistance); iDX <= iMinUnitDistance; iDX++)
					{
						for(iDY = -(iMinUnitDistance); iDY <= iMinUnitDistance; iDY++)
						{
							pLoopPlot	= plotXYWithRangeCheck(pTestPlot->getX(), pTestPlot->getY(), iDX, iDY, iMinUnitDistance);

							if(pLoopPlot != NULL)
							{
								if(pLoopPlot->isUnit())
								{
									bValid = false;
								}
							}
						}
					}
				}
			}

			if(bValid)
			{
				if(iFlags & RANDPLOT_LAND)
				{
					if(pTestPlot->isWater())
					{
						bValid = false;
					}
				}
			}

			if(bValid)
			{
				if(iFlags & RANDPLOT_UNOWNED)
				{
					if(pTestPlot->isOwned())
					{
						bValid = false;
					}
				}
			}

			if(bValid)
			{
				if(iFlags & RANDPLOT_ADJACENT_UNOWNED)
				{
					if(pTestPlot->isAdjacentOwned())
					{
						bValid = false;
					}
				}
			}

			if(bValid)
			{
				if(iFlags & RANDPLOT_ADJACENT_LAND)
				{
					if(!(pTestPlot->isAdjacentToLand(false)))
					{
						bValid = false;
					}
				}
			}

			if(bValid)
			{
				if(iFlags & RANDPLOT_PASSIBLE)
				{
#if defined(MOD_BALANCE_CORE)
					if(pTestPlot->isImpassable(BARBARIAN_TEAM))
#else
					if(pTestPlot->isImpassable())
#endif
					{
						bValid = false;
					}
				}
			}

			if(bValid)
			{
				if(iFlags & RANDPLOT_NOT_VISIBLE_TO_CIV)
				{
					if(pTestPlot->isVisibleToCivTeam())
					{
						bValid = false;
					}
				}
			}

			if(bValid)
			{
				if(iFlags & RANDPLOT_NOT_CITY)
				{
					if(pTestPlot->isCity())
					{
						bValid = false;
					}
				}
			}

			if(bValid)
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

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& thisPlayer = GET_PLAYER((PlayerTypes)iI);
		if(thisPlayer.isAlive())
		{
			if((eOwner == NO_PLAYER) || (iI == eOwner))
			{
				if((eTeam == NO_TEAM) || (thisPlayer.getTeam() == eTeam))
				{
					for(pLoopCity = thisPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = thisPlayer.nextCity(&iLoop))
					{
						if(!bSameArea || pLoopCity->isMatchingArea(pCheckPlot))
						{
							if(!bCoastalOnly || pLoopCity->isCoastal())
							{
								if((eTeamAtWarWith == NO_TEAM) || atWar(thisPlayer.getTeam(), eTeamAtWarWith))
								{
									if((eDirection == NO_DIRECTION) || (estimateDirection(iX,iY,pLoopCity->getX(),pLoopCity->getY()) == eDirection))
									{
										if((pSkipCity == NULL) || (pLoopCity != pSkipCity))
										{
											iValue = plotDistance(iX, iY, pLoopCity->getX(), pLoopCity->getY());

											if(iValue < iBestValue)
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

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& thisPlayer = GET_PLAYER((PlayerTypes)iI);
		if(thisPlayer.isAlive())
		{
			if((eOwner == NO_PLAYER) || (iI == eOwner))
			{
				for(pLoopUnit = thisPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = thisPlayer.nextUnit(&iLoop))
				{
					if(!bReadyToSelect || pLoopUnit->ReadyToSelect())
					{
						if(!bWorkers || pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER || pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER_SEA)
						{
							iValue = plotDistance(iX, iY, pLoopUnit->getX(), pLoopUnit->getY());

							if(iValue < iBestValue)
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
CvPlot* CvMap::findNearestStartPlot(int iX, int iY, PlayerTypes& eOwner)
{
	eOwner = NO_PLAYER;
	int iBestValue;
	CvPlot* pBestStartPlot = 0;

	iBestValue = INT_MAX;

	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& thisPlayer = GET_PLAYER((PlayerTypes)iI);
		if(thisPlayer.isEverAlive())
		{
			CvPlot* pStartPlot = thisPlayer.getStartingPlot();
			if (pStartPlot)
			{
				int iValue = plotDistance(iX, iY, pStartPlot->getX(), pStartPlot->getY());
				if(iValue < iBestValue)
				{
					iBestValue = iValue;
					pBestStartPlot = pStartPlot;
					eOwner = static_cast<PlayerTypes>(iI);
				}
			}
		}
	}
	return pBestStartPlot;
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
		if(pLoopArea->isWater() == bWater)
		{
			iValue = pLoopArea->getNumTiles();

			if(iValue > iBestValue)
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
	if(isWrapX())
	{
		wrapX = (int)CvFractal::FRAC_WRAP_X;
	}

	int wrapY = 0;
	if(isWrapY())
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

	for(iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for(iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pLoopPlot = plotXYWithRangeCheck(iPlotX, iPlotY, iDX, iDY, iRange);

			if(pLoopPlot != NULL)
			{
				if(bFreshWater)
				{
					if(pLoopPlot->isFreshWater(false))
					{
						return true;
					}
				}
				else
				{
					if(pLoopPlot->isWater())
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
	for(int iLoop = 0; iLoop < GC.getNUM_RESOURCE_QUANTITY_TYPES(); iLoop++)
	{
		if(thisResourceInfo->getResourceQuantityType(iLoop) <= 0)
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
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
//	--------------------------------------------------------------------------------
void CvMap::setNumResources(ResourceTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eIndex);
	int iResourceQuantity = 0;
	if(pkResourceInfo != NULL && pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
	{
		CvMap& kMap = GC.getMap();
		int iWorldNumPlots = kMap.numPlots();
		for (int iPlotLoop = 0; iPlotLoop < iWorldNumPlots; iPlotLoop++)
		{
			CvPlot* pPlot = kMap.plotByIndex(iPlotLoop);
			if(pPlot != NULL)
			{
				if(pPlot->getResourceType() == eIndex)
				{
					iResourceQuantity += pPlot->getNumResource();
				}
			}
		}
		m_paiNumResource[eIndex] = iResourceQuantity;
	}
}
#endif
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
		if(!(pLoopArea->isWater()))
		{
			iNumLandAreas++;
		}
	}

	return iNumLandAreas;
}


//	--------------------------------------------------------------------------------
CvArea* CvMap::getArea(int iID)
{
	return m_areas.Get(iID);
}


//	--------------------------------------------------------------------------------
CvArea* CvMap::addArea()
{
	//do not use TContainer::Add here, it uses the global ID counter which we don't need here
	CvArea* pNew = new CvArea();
	pNew->SetID( m_areas.GetCount()+1 );
	m_areas.Load(pNew);
	return pNew;
}


//	--------------------------------------------------------------------------------
void CvMap::deleteArea(int iID)
{
	m_areas.Remove(iID);
}


//	--------------------------------------------------------------------------------
CvArea* CvMap::firstArea(int* pIterIdx, bool bRev)
{
	if (bRev)
		*pIterIdx = m_areas.GetCount()-1;
	else
		*pIterIdx = 0;
	return m_areas.GetAt(*pIterIdx);
}


//	--------------------------------------------------------------------------------
CvArea* CvMap::nextArea(int* pIterIdx, bool bRev)
{
	if (bRev)
		(*pIterIdx)--;
	else
		(*pIterIdx)++;
	return m_areas.GetAt(*pIterIdx);
}


//	--------------------------------------------------------------------------------
void CvMap::recalculateAreas()
{
	int iI;

	int iNumPlots = numPlots();
	for(iI = 0; iI < iNumPlots; iI++)
	{
		plotByIndexUnchecked(iI)->setArea(-1);
		plotByIndexUnchecked(iI)->updateImpassable();
	}

	m_areas.RemoveAll();

	calculateAreas();

	recalculateLandmasses();
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
	MOD_SERIALIZE_INIT_READ(kStream);

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

	CvAssertMsg((0 < GC.getNumResourceInfos()), "GC.getNumResourceInfos() is not greater than zero but an array is being allocated");
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiNumResource, GC.getNumResourceInfos());
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiNumResourceOnLand, GC.getNumResourceInfos());

	if(numPlots() > 0)
	{
		InitPlots();
		int iI;
		for(iI = 0; iI < numPlots(); iI++)
		{
			m_pMapPlots[iI].read(kStream);
		}
	}

	// call the read of the free list CvArea class allocations
	kStream >> m_areas;

	kStream >> m_landmasses;

	kStream >> m_iAIMapHints;

	setup();

	updateAdjacency();

	gDLL->DoMapSetup(numPlots());

#if defined(MOD_EVENTS_TERRAFORMING)
	if (MOD_EVENTS_TERRAFORMING) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_TerraformingMap, TERRAFORMINGEVENT_LOAD, 1);
	}
#endif
}

//	--------------------------------------------------------------------------------
// save object to a stream
// used during save
//
void CvMap::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

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
	for(iI = 0; iI < numPlots(); iI++)
	{
		m_pMapPlots[iI].write(kStream);
	}

	// call the read of the free list CvArea class allocations
	kStream << m_areas;

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

void CvMap::calculateAreas()
{
	CvPlot* pLoopPlot;
	CvArea* pArea;
	int iArea;
	int iI;

	for(iI = 0; iI < numPlots(); iI++)
	{
		pLoopPlot = plotByIndexUnchecked(iI);
		CvAssertMsg(pLoopPlot != NULL, "LoopPlot is not assigned a valid value");

		if(!pLoopPlot) 
			continue;

		if(pLoopPlot->getArea() == -1)
		{
			pArea = addArea();
			pArea->init(pArea->GetID(), pLoopPlot->isWater());

			pArea->SetMountains(pLoopPlot->isMountain());

			iArea = pArea->GetID();

			pLoopPlot->setArea(iArea);

			CvAreaBoundaries boundaries;
			boundaries.m_iEastEdge = pLoopPlot->getX();
			boundaries.m_iWestEdge = pLoopPlot->getX();
			boundaries.m_iNorthEdge = pLoopPlot->getY();
			boundaries.m_iSouthEdge = pLoopPlot->getY();

			SPathFinderUserData data(NO_PLAYER, PT_AREA_CONNECTION);
			ReachablePlots result = GC.GetStepFinder().GetPlotsInReach(pLoopPlot->getX(), pLoopPlot->getY(), data);

			for (ReachablePlots::iterator it = result.begin(); it != result.end(); ++it)
			{
				CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);
				pPlot->setArea(iArea);

				boundaries.m_iWestEdge = MIN(boundaries.m_iWestEdge,pPlot->getX());
				boundaries.m_iEastEdge = MAX(boundaries.m_iEastEdge,pPlot->getX());
				boundaries.m_iSouthEdge = MIN(boundaries.m_iSouthEdge,pPlot->getY());
				boundaries.m_iNorthEdge = MAX(boundaries.m_iNorthEdge,pPlot->getY());
			}

			pArea->setAreaBoundaries(boundaries);
#if defined(MOD_BALANCE_CORE)
			pArea->CalcNumBadPlots();
#endif
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
	for(int iFeatureLoop = 0; iFeatureLoop < iNumFeatureInfos; iFeatureLoop++)
	{
		eFeature = (FeatureTypes) iFeatureLoop;
		CvFeatureInfo* feature = GC.getFeatureInfo(eFeature);
#if defined(MOD_PSEUDO_NATURAL_WONDER)
		if(feature && feature->IsNaturalWonder(true))
#else
		if(feature && feature->IsNaturalWonder())
#endif
		{
			eNWFeature = eFeature;

			// hackery - this should be more data driven but the engine side isn't

			if(strcmp(feature->GetType(), "FEATURE_EVEREST") == 0)
			{
				featureEverest = eFeature;
			}
			else if(strcmp(feature->GetType(), "FEATURE_CRATER") == 0)
			{
				featureCrater = eFeature;
			}
			else if(strcmp(feature->GetType(), "FEATURE_TITICACA") == 0)
			{
				featureLake = eFeature;
			}
			else if(strcmp(feature->GetType(), "FEATURE_FUJI") == 0)
			{
				featureFuji = eFeature;
			}
			else if(strcmp(feature->GetType(), "FEATURE_MESA") == 0)
			{
				featureMesa = eFeature;
			}
			else if(strcmp(feature->GetType(), "FEATURE_REEF") == 0)
			{
				featureReef = eFeature;
			}
			else if(strcmp(feature->GetType(), "FEATURE_VOLCANO") == 0)
			{
				featureVolcano = eFeature;
			}

			nwCount++;
		}
	}

	// No Natural Wonder Feature, abort!
	if(eNWFeature == NO_FEATURE)
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

	for(pLoopArea = firstArea(&iLoop); pLoopArea != NULL; pLoopArea = nextArea(&iLoop))
	{
		if(!pLoopArea->isWater())
		{
			if(!pLoopArea->IsMountains())
			{
				iBottomLatitude = pLoopArea->GetAreaMinLatitude();

				if(iBottomLatitude <= iMaxLatitudeForNW)
				{
					iNumValidAreas++;
				}
			}
		}
	}

	int iMaxNWsOneArea = 2;

	// If we have more Natural Wonders than we have Areas then we have to handle things differently - more can be placed in one Area than normal
	if(iNumNaturalWondersToAdd > iNumValidAreas)
	{
		iMaxNWsOneArea = iNumNaturalWondersToAdd - iNumValidAreas + 1;
	}

	while(iNumNaturalWondersToAdd > 0 && iCount < 10000)
	{
		iCount++;

		iPlotRand = GC.getGame().getJonRandNum(iNumMapPlots, "Randomly Placing Natural Wonders");

		pRandPlot = plotByIndex(iPlotRand);

		// No Water Tiles
		if(pRandPlot->isWater())
		{
			continue;
		}

		// No River Tiles either (don't want them carved up)
		if(pRandPlot->isRiver())
		{
			continue;
		}

		// No Tiles with Resources
		if(pRandPlot->getResourceType() != NO_RESOURCE)
		{
			continue;
		}

		// No Tiles with Improvements (Goody Huts)
		if(pRandPlot->getImprovementType() != NO_IMPROVEMENT)
		{
			continue;
		}

		// Can't have too many NWs in one Area
		if(getArea(pRandPlot->getArea())->GetNumNaturalWonders() >= iMaxNWsOneArea)
		{
			continue;
		}

		// Has to be in the central band of the map.  If the latitude is too extreme we might end up on a islelet surrounded by Ice or something
		if(pRandPlot->getLatitude() > /*65*/ iMaxLatitudeForNW)
		{
			continue;
		}

		// Can't be a starting Plot for ANY player (including Minors)
		if(pRandPlot->isStartingPlot())
		{
			continue;
		}

		bool bValid;

		// Does it have to be close to the coast?
		if(iCoastDistance != -1)
		{
			bValid = false;

			for(iPlotLoopX = -iCoastDistance; iPlotLoopX <= iCoastDistance; iPlotLoopX++)
			{
				for(iPlotLoopY = -iCoastDistance; iPlotLoopY <= iCoastDistance; iPlotLoopY++)
				{
					pLoopPlot = plotXYWithRangeCheck(pRandPlot->getX(), pRandPlot->getY(), iPlotLoopX, iPlotLoopY, iCoastDistance);

					if(pLoopPlot != NULL)
					{
						if(pLoopPlot->isWater())
						{
							if(!pLoopPlot->isLake(false))
							{
								// Found a Plot within 2 plots of "the Ocean"
								bValid = true;
								break;
							}
						}
					}
				}

				if(bValid)
				{
					break;
				}
			}

			// Plot not within 2 of Ocean
			if(!bValid)
			{
				continue;
			}
		}

		bValid = true;

		// Can't be too close to another Natural Wonder
		for(iPlotLoopX = -iAnotherNWDistance; iPlotLoopX <= iAnotherNWDistance; iPlotLoopX++)
		{
			for(iPlotLoopY = -iAnotherNWDistance; iPlotLoopY <= iAnotherNWDistance; iPlotLoopY++)
			{
				pLoopPlot = plotXYWithRangeCheck(pRandPlot->getX(), pRandPlot->getY(), iPlotLoopX, iPlotLoopY, iAnotherNWDistance);

				if(pLoopPlot != NULL)
				{
					if(pLoopPlot->IsNaturalWonder())
					{
						// Found a NW too close
						bValid = false;
						break;
					}
				}
			}

			if(!bValid)
			{
				break;
			}
		}

		// Natural Wonder too close
		if(!bValid)
		{
			continue;
		}

		// see if we can add the volcano
		if(featureVolcano != NO_FEATURE)
		{
			if(!pRandPlot->isAdjacentToLand(false))
			{
				pRandPlot->setPlotType(PLOT_MOUNTAIN);
				pRandPlot->setFeatureType(featureVolcano);
				iNumNaturalWondersToAdd--;
				featureVolcano = NO_FEATURE;	// Prevent this one from being placed again
				continue;
			}
		}

		// see if we can add the reef
		if(featureReef != NO_FEATURE)
		{
			if(!pRandPlot->isMountain() && pRandPlot->isAdjacentToShallowWater())
			{
				if(pRandPlot->area()->getNumTiles() == 2)
				{
					pRandPlot->setPlotType(PLOT_MOUNTAIN);
					pRandPlot->setFeatureType(featureReef);
					for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
					{
						CvPlot* pAdjacentPlot = plotDirection(pRandPlot->getX(), pRandPlot->getY(), ((DirectionTypes)iI));
						if(pAdjacentPlot != NULL)
						{
							if(!(pAdjacentPlot->isWater()))
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
		if(featureLake != NO_FEATURE)
		{
			if(pRandPlot->isHills())
			{
				int iHillCount = 0;
				// if we have a couple of hills nearby already
				for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
				{
					CvPlot* pAdjacentPlot = plotDirection(pRandPlot->getX(), pRandPlot->getY(), ((DirectionTypes)iI));
					if(pAdjacentPlot != NULL)
					{
						if((pAdjacentPlot->isHills()))
						{
							iHillCount++;
						}
					}
				}
				if(iHillCount > 2)
				{
					// we are stamping the terrain
					for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
					{
						CvPlot* pAdjacentPlot = plotDirection(pRandPlot->getX(), pRandPlot->getY(), ((DirectionTypes)iI));
						if(pAdjacentPlot != NULL)
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
		if(featureEverest != NO_FEATURE)
		{
			if(pRandPlot->isMountain() && GC.getGame().getMapRandNum(100, "Natural Wonder") > 75)
			{
				// we are stamping the terrain
				for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
				{
					CvPlot* pAdjacentPlot = plotDirection(pRandPlot->getX(), pRandPlot->getY(), ((DirectionTypes)iI));
					if(pAdjacentPlot != NULL)
					{
						if(iI & 1)
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
		if(pRandPlot->isCoastalLand(-1,false))
		{
			continue;
		}

		int iDice = GC.getGame().getMapRandNum(100, "Natural Wonder");
		if(iDice > 66)
		{
			if(featureCrater != NO_FEATURE)
			{
				pRandPlot->setPlotType(PLOT_MOUNTAIN);
				pRandPlot->setFeatureType(featureCrater);
				iNumNaturalWondersToAdd--;
				featureCrater = NO_FEATURE;	// Prevent this one from being placed again
				continue;
			}
		}
		else if(iDice > 25)
		{
			if(featureFuji != NO_FEATURE)
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
			if(featureMesa != NO_FEATURE)
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

	recalculateLandmasses();

	CvLandmass* pLoopMass;
	int iLoop;

	// find the 4 (because that is how many art styles we have) biggest continents - if there aren't 4, oh well
	for(pLoopMass = firstLandmass(&iLoop); pLoopMass != NULL; pLoopMass = nextLandmass(&iLoop))
	{
		if(!pLoopMass->isWater())
		{
			int iBestContinent;
			int iNumTiles = pLoopMass->getNumTiles();
			for(iBestContinent=0; iBestContinent<iNumContinentTypes; iBestContinent++)
			{
				if(iNumTiles >= iMinimumThresholdToBeAContinentIMHO && iNumTiles > iLargeContinentsSize[iBestContinent])
				{
					// push the rest of the tiles down one and store this
					for(int iloop=iNumContinentTypes-1; iloop>iBestContinent; iloop--)
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
	int iLargeContinentsArtStyle[iNumContinentTypes] = {0,1,2,3};
	shuffleArray(iLargeContinentsArtStyle, 4, GC.getGame().getMapRand());
	for(int i=0; i < iNumContinentTypes; i++)
	{
		iLargeContinentsArtStyle[i]++;
	}

	for(int i=0; i < iNumContinentTypes; i++)
	{
		if(iLargeContinentsID[i] != -1)
			iNumLargeContinents++;
	}

	int iLargeContinentsX[iNumContinentTypes] = {0,0,0,0};
	int iLargeContinentsY[iNumContinentTypes] = {0,0,0,0};
	for(int i=0; i < iNumLargeContinents; i++)
	{
		CvLandmass* pLandmass = getLandmass(iLargeContinentsID[i]);
		iLargeContinentsX[i] = pLandmass->GetCentroidX();
		iLargeContinentsY[i] = pLandmass->GetCentroidY();
	}

	// for all landmasses in the game
	for(pLoopMass = firstLandmass(&iLoop); pLoopMass != NULL; pLoopMass = nextLandmass(&iLoop))
	{
		int iX = pLoopMass->GetCentroidX();
		int iY = pLoopMass->GetCentroidY();
		// if water continent type = 0
		if(pLoopMass->isWater())
		{
			pLoopMass->SetContinentType(0);
		}
		else // if land
		{
			// continent type = continent type of the closest of the "big four"
			int iBestPlotDistance = 999999;
			for(int i=0; i < iNumLargeContinents; i++)
			{
				int iPlotDistance = plotDistance(iX, iY, iLargeContinentsX[i], iLargeContinentsY[i]);
				if(iPlotDistance < iBestPlotDistance)
				{
					iBestPlotDistance = iPlotDistance;
					pLoopMass->SetContinentType(iLargeContinentsArtStyle[i]);
				}
			}
		}
	}

	// now stamp all plots with the relevant data
	for(int iI = 0; iI < numPlots(); iI++)
	{
		CvPlot* pLoopPlot = plotByIndexUnchecked(iI);
		CvLandmass* pLandmass = getLandmass(pLoopPlot->getLandmass());
		if(pLandmass != NULL)
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
		if(!(pLoopLandmass->isWater()))
		{
			iNumLandLandmasses++;
		}
	}

	return iNumLandLandmasses;
}


//	--------------------------------------------------------------------------------
CvLandmass* CvMap::getLandmass(int iID)
{
	return m_landmasses.Get(iID);
}


//	--------------------------------------------------------------------------------
CvLandmass* CvMap::addLandmass()
{
	//do not use TContainer::Add here, it uses the global ID counter which we don't need here
	CvLandmass* pNew = new CvLandmass();
	pNew->SetID( m_landmasses.GetCount()+1 );
	m_landmasses.Load(pNew);
	return pNew;
}


//	--------------------------------------------------------------------------------
void CvMap::deleteLandmass(int iID)
{
	m_landmasses.Remove(iID);
}


//	--------------------------------------------------------------------------------
CvLandmass* CvMap::firstLandmass(int* pIterIdx, bool bRev)
{
	if (bRev)
		*pIterIdx = m_landmasses.GetCount()-1;
	else
		*pIterIdx = 0;
	return m_landmasses.GetAt(*pIterIdx);
}


//	--------------------------------------------------------------------------------
CvLandmass* CvMap::nextLandmass(int* pIterIdx, bool bRev)
{
	if (bRev)
		(*pIterIdx)--;
	else
		(*pIterIdx)++;
	return m_landmasses.GetAt(*pIterIdx);
}

//	--------------------------------------------------------------------------------
void CvMap::recalculateLandmasses()
{
	int iNumPlots = numPlots();
	for(int iI = 0; iI < iNumPlots; iI++)
	{
		plotByIndexUnchecked(iI)->updateWaterFlags();
		plotByIndexUnchecked(iI)->setLandmass(-1);
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

	for(int iI = 0; iI < numPlots(); iI++)
	{
		pLoopPlot = plotByIndexUnchecked(iI);
		if(pLoopPlot->getLandmass() == -1)
		{
			pLandmass = addLandmass();
			pLandmass->init(pLandmass->GetID(), pLoopPlot->isWater());

			iLandmassID = pLandmass->GetID();

			pLoopPlot->setLandmass(iLandmassID);

			SPathFinderUserData data(NO_PLAYER, PT_LANDMASS_CONNECTION);
			ReachablePlots result = GC.GetStepFinder().GetPlotsInReach(pLoopPlot->getX(), pLoopPlot->getY(), data);

			for (ReachablePlots::iterator it = result.begin(); it != result.end(); ++it)
			{
				CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);
				pPlot->setLandmass(iLandmassID);
			}
		}
	}

	// KWG: Rebuild the yields here.  Yes, this is called during the landmass rebuild process if the landmass' 'lake' field changes, but
	//      there is a problem with that. The yield bonus for a lake is dependent on the proximity to a plot that is a lake, and not the general landmass
	//		'lake' flag.  During recalculation, a neighboring plot's 'lake' flag may not be set yet because it is in a landmass that has yet to be calculated
	//		resulting in the wrong yield being applied to a plot.
	updateYield();
}

//	---------------------------------------------------------------------------
int CvMap::Validate()
{
	//SS: Patch Trade Connections.
	//Just after the BNW Launch trade connection data could be corrupted due to razing cities.
	//This function is used because it's called immediately after serialization.
	//Iterate through all trade connections.
	//Clear the connection if the source or dest city does not exist.
	CvGameTrade* pGameTrade = GC.getGame().GetGameTrade();
	if(pGameTrade)
	{
		for (uint ui = 0; ui < pGameTrade->GetNumTradeConnections(); ui++)
		{
			const TradeConnection& connection = pGameTrade->GetTradeConnection(ui);
			if(connection.m_iID > -1)
			{
				CvPlot* pOriginPlot = plot(connection.m_iOriginX, connection.m_iOriginY);
				if(pOriginPlot)
				{
					if(pOriginPlot->getPlotCity() == NULL)
					{
#if defined(MOD_BUGFIX_MINOR)
						pGameTrade->ClearAllCityTradeRoutes(pOriginPlot, true);
#else
						pGameTrade->ClearAllCityTradeRoutes(pOriginPlot);
#endif
						continue;
					}
				}

				CvPlot* pDestPlot = plot(connection.m_iDestX, connection.m_iDestY);
				if(pDestPlot)
				{
					if(pDestPlot->getPlotCity() == NULL)
					{
#if defined(MOD_BUGFIX_MINOR)
						pGameTrade->ClearAllCityTradeRoutes(pDestPlot, true);
#else
						pGameTrade->ClearAllCityTradeRoutes(pDestPlot);
#endif
						continue;
					}
				}
			}
		}
	}



	int iErrors = 0;
	for(int iI = 0; iI < numPlots(); iI++)
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

#if defined(MOD_UNIT_KILL_STATS)
int CvMap::GetUnitKillCount(PlayerTypes ePlayer, int iPlotIndex)
{
	if (killCount.find(ePlayer) != killCount.end())
		if (killCount[ePlayer].find(iPlotIndex) != killCount[ePlayer].end())
			return killCount[ePlayer][iPlotIndex];

	return 0;
}

void CvMap::IncrementUnitKillCount(PlayerTypes ePlayer, int iPlotIndex)
{
	if (iPlotIndex >= numPlots())
		return;

	const int KILLCOUNT_GRANULARITY = 100;

	if (killCount.find(ePlayer) != killCount.end())
	{
		if (killCount[ePlayer].find(iPlotIndex) != killCount[ePlayer].end())
			killCount[ePlayer][iPlotIndex] += KILLCOUNT_GRANULARITY;
		else
			killCount[ePlayer][iPlotIndex] = KILLCOUNT_GRANULARITY;
	}
	else
		killCount[ePlayer][iPlotIndex] = KILLCOUNT_GRANULARITY;
}

void CvMap::ExportUnitKillCount(PlayerTypes ePlayer)
{
	if (killCount.find(ePlayer) != killCount.end())
	{
		std::vector<int> v(numPlots(), 0);
		for (UnitKillCount::value_type::second_type::iterator it = killCount[ePlayer].begin(); it != killCount[ePlayer].end(); ++it)
			v[it->first] = it->second;

		ofstream out(CvString::format("c:\\temp\\UnitKillCount_Player%d.csv", ePlayer).c_str());
		if (out)
		{
			out << "#x,y,terrain,count\n";
			for (int i=0; i<numPlots(); i++)
			{
				CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(i);
				out << pPlot->getX() << "," << pPlot->getY() << "," << pPlot->getTerrainType() << "," << v[i] << "\n";
			}
		}
		out.close();
	}
}

void CvMap::DoKillCountDecay(float fDecayFactor)
{
	for (UnitKillCount::iterator itPlayer = killCount.begin(); itPlayer != killCount.end(); ++itPlayer)
		for (UnitKillCount::value_type::second_type::iterator itPlot = itPlayer->second.begin(); itPlot != itPlayer->second.end(); ++itPlot)
			itPlot->second = int(itPlot->second*fDecayFactor);
}
#endif

void CvMap::ClearPlotsAtRange(const CvPlot* pPlot)
{
	if (pPlot == NULL)
	{
		m_vPlotsWithLineOfSightFromPlot2.clear();
		m_vPlotsWithLineOfSightFromPlot3.clear();
		m_vPlotsWithLineOfSightToPlot2.clear();
		m_vPlotsWithLineOfSightToPlot3.clear();
	}
	else
	{
		m_vPlotsWithLineOfSightFromPlot2.erase(pPlot->GetPlotIndex());
		m_vPlotsWithLineOfSightFromPlot3.erase(pPlot->GetPlotIndex());
		m_vPlotsWithLineOfSightToPlot2.erase(pPlot->GetPlotIndex());
		m_vPlotsWithLineOfSightToPlot3.erase(pPlot->GetPlotIndex());
	}
}

std::vector<CvPlot*> CvMap::GetPlotsAtRange(const CvPlot* pPlot, int iRange, bool bFromPlot, bool bWithLOS)
{
	if (!pPlot)
		return vector<CvPlot*>();

	//for now, we can only do up to range 5
	if (iRange<1 || iRange>5)
		OutputDebugString("GetPlotsAtRangeX() called with invalid parameter\n");

	iRange = max(1, iRange);
	iRange = min(5, iRange);

	if (bWithLOS)
	{
		switch (iRange)
		{
		case 1:
			{
				//just take all direct neighbors
				CvPlot** aDirectNeighbors = getNeighborsUnchecked(pPlot);
				return vector<CvPlot*>(aDirectNeighbors, aDirectNeighbors + NUM_DIRECTION_TYPES);
			}
		case 2:
			if (bFromPlot)
			{
				PlotNeighborLookup::iterator it = m_vPlotsWithLineOfSightFromPlot2.find(pPlot->GetPlotIndex());
				if (it != m_vPlotsWithLineOfSightFromPlot2.end())
					return it->second;

				//not found? update cache
				m_vPlotsWithLineOfSightFromPlot2[pPlot->GetPlotIndex()] = vector<CvPlot*>();
				for (int i = RING1_PLOTS; i<RING2_PLOTS; i++)
				{
					CvPlot* pLoopPlot = iterateRingPlots(pPlot, i);
					if (!pLoopPlot)
						continue;

					if (pPlot->canSeePlot(pLoopPlot, NO_TEAM, 2, NO_DIRECTION))
						m_vPlotsWithLineOfSightFromPlot2[pPlot->GetPlotIndex()].push_back(pLoopPlot);
				}
			}
			else
			{
				PlotNeighborLookup::iterator it = m_vPlotsWithLineOfSightToPlot2.find(pPlot->GetPlotIndex());
				if (it != m_vPlotsWithLineOfSightToPlot2.end())
					return it->second;

				//not found? update cache
				m_vPlotsWithLineOfSightToPlot2[pPlot->GetPlotIndex()] = vector<CvPlot*>();
				for (int i = RING1_PLOTS; i<RING2_PLOTS; i++)
				{
					CvPlot* pLoopPlot = iterateRingPlots(pPlot, i);
					if (!pLoopPlot)
						continue;

					if (pLoopPlot->canSeePlot(pPlot, NO_TEAM, 2, NO_DIRECTION))
						m_vPlotsWithLineOfSightToPlot2[pPlot->GetPlotIndex()].push_back(pLoopPlot);
				}
			}
		case 3:
			if (bFromPlot)
			{
				PlotNeighborLookup::iterator it = m_vPlotsWithLineOfSightFromPlot3.find(pPlot->GetPlotIndex());
				if (it != m_vPlotsWithLineOfSightFromPlot3.end())
					return it->second;

				//not found? update cache
				m_vPlotsWithLineOfSightFromPlot3[pPlot->GetPlotIndex()] = vector<CvPlot*>();
				for (int i = RING2_PLOTS; i<RING3_PLOTS; i++)
				{
					CvPlot* pLoopPlot = iterateRingPlots(pPlot, i);
					if (!pLoopPlot)
						continue;

					if (pPlot->canSeePlot(pLoopPlot, NO_TEAM, 3, NO_DIRECTION))
						m_vPlotsWithLineOfSightFromPlot3[pPlot->GetPlotIndex()].push_back(pLoopPlot);
				}
			}
			else
			{
				PlotNeighborLookup::iterator it = m_vPlotsWithLineOfSightToPlot3.find(pPlot->GetPlotIndex());
				if (it != m_vPlotsWithLineOfSightToPlot3.end())
					return it->second;

				//not found? update cache
				m_vPlotsWithLineOfSightToPlot3[pPlot->GetPlotIndex()] = vector<CvPlot*>();
				for (int i = RING2_PLOTS; i<RING3_PLOTS; i++)
				{
					CvPlot* pLoopPlot = iterateRingPlots(pPlot, i);
					if (!pLoopPlot)
						continue;

					if (pLoopPlot->canSeePlot(pPlot, NO_TEAM, 3, NO_DIRECTION))
						m_vPlotsWithLineOfSightToPlot3[pPlot->GetPlotIndex()].push_back(pLoopPlot);
				}
			}
		case 4:
		case 5:
		{
			//no caching here
			vector<CvPlot*> vResult;
			vResult.reserve( RING_PLOTS[iRange] - RING_PLOTS[iRange-1]);
			for (int i = RING_PLOTS[iRange-1]; i < RING_PLOTS[iRange]; i++)
			{
				CvPlot* pCandidate = iterateRingPlots(pPlot, i);
				if (!pCandidate)
					continue;

				if (bFromPlot)
				{
					if (pPlot->canSeePlot(pCandidate, NO_TEAM, 2, NO_DIRECTION))
						vResult.push_back(pCandidate);
				}
				else
				{
					if (pCandidate->canSeePlot(pPlot, NO_TEAM, 2, NO_DIRECTION))
						vResult.push_back(pCandidate);
				}
			}
			return vResult;
		}

		}
	}
	else //no LOS - not cached, rarely accessed (should be only for units with indirect fire promotion)
	{
		switch (iRange)
		{
		case 1:
		{
			//just take all direct neighbors
			CvPlot** aDirectNeighbors = getNeighborsUnchecked(pPlot);
			return vector<CvPlot*>(aDirectNeighbors, aDirectNeighbors + NUM_DIRECTION_TYPES);
		}
		case 2:
		case 3:
		case 4:
		case 5:
		{
			vector<CvPlot*> vResult;
			vResult.reserve( RING_PLOTS[iRange] - RING_PLOTS[iRange-1]);
			for (int i = RING_PLOTS[iRange-1]; i < RING_PLOTS[iRange]; i++)
			{
				CvPlot* pCandidate = iterateRingPlots(pPlot, i);
				if (pCandidate)
					vResult.push_back(pCandidate);
			}
			return vResult;
		}
		}
	}

	return vector<CvPlot*>();
}

int CvMap::GetPopupCount(int iPlotIndex)
{
	return m_plotPopupCount[iPlotIndex];
}

void CvMap::IncreasePopupCount(int iPlotIndex)
{
	m_plotPopupCount[iPlotIndex] += 1;
}
