/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvArea.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvGlobals.h"
#include "CvPlayerAI.h"
#include "CvTeam.h"
#include "CvGameCoreUtils.h"
#include "CvInfos.h"

#include "ICvDLLUserInterface.h"
#include "CvEnumSerialization.h"

#include "CvInfosSerializationHelper.h"
// include after all other headers
#include "LintFree.h"

// Public Functions...
//	--------------------------------------------------------------------------------
CvArea::CvArea()
{
	reset(0, false);
}


//	--------------------------------------------------------------------------------
CvArea::~CvArea()
{
	uninit();
}


//	--------------------------------------------------------------------------------
void CvArea::init(int iID, bool bWater)
{
	//--------------------------------
	// Init saved data
	reset(iID, bWater);

	//--------------------------------
	// Init non-saved data

	//--------------------------------
	// Init other game data
}


//	--------------------------------------------------------------------------------
void CvArea::uninit()
{
}


//	--------------------------------------------------------------------------------
// FUNCTION: reset()
// Initializes data members that are serialized.
void CvArea::reset(int iID, bool bWater)
{
	//--------------------------------
	// Uninit class
	uninit();

	m_iID = iID;
	m_iNumTiles = 0;
	m_iNumOwnedTiles = 0;
	m_iNumRiverEdges = 0;
	m_iNumUnits = 0;
	m_iNumCities = 0;
	m_iTotalPopulation = 0;
	m_iNumStartingPlots = 0;

	m_iNumNaturalWonders = 0;
	m_iTotalFoundValue = 0;
	m_iBadPlots = 0;

	m_Boundaries.m_iNorthEdge = 0;
	m_Boundaries.m_iSouthEdge = 0;
	m_Boundaries.m_iEastEdge = 0;
	m_Boundaries.m_iWestEdge = 0;

	m_bWater = bWater;
	m_bMountains = false;

	m_aiUnitsPerPlayer.clear();
	m_aiCitiesPerPlayer.clear();
	m_aiPopulationPerPlayer.clear();
	m_aiFreeSpecialist.clear();
	m_aiNumRevealedTiles.clear();
	m_aTargetCities.clear();
	m_aaiYieldRateModifier.clear();

	m_aiNumResources.clear();
	m_aiNumImprovements.clear();
}

//	--------------------------------------------------------------------------------
void CvArea::SetID(int iID)
{
	m_iID = iID;
}

//	--------------------------------------------------------------------------------
int CvArea::calculateTotalBestNatureYield() const
{
	int iCount = 0;

	CvMap& theMap = GC.getMap();
	int iNumPlots = theMap.numPlots();
	for(int iI = 0; iI < iNumPlots; iI++)
	{
		CvPlot* pLoopPlot = theMap.plotByIndexUnchecked(iI);

		if(pLoopPlot->getArea() == GetID())
		{
			iCount += pLoopPlot->calculateTotalBestNatureYield(NO_PLAYER);
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int CvArea::countCoastalLand() const
{
	if(isWater())
	{
		return 0;
	}

	int iCount = 0;

	CvMap& theMap = GC.getMap();
	int iNumPlots = theMap.numPlots();
	for(int iI = 0; iI < iNumPlots; iI++)
	{
		CvPlot* pLoopPlot = theMap.plotByIndexUnchecked(iI);

		if(pLoopPlot->getArea() == GetID())
		{
			if(pLoopPlot->isCoastalLand())
			{
				iCount++;
			}
		}
	}

	return iCount;
}

//	--------------------------------------------------------------------------------
int CvArea::countNumUniqueResourceTypes() const
{
	int iCount = 0;
	int numRIs = GC.getNumResourceInfos();

	for(int iI = 0; iI < numRIs; iI++)
	{
		CvResourceInfo* resource = GC.getResourceInfo((ResourceTypes)iI);
		if(NULL != resource && resource->isOneArea())
		{
			iCount++;
		}
	}

	return iCount;
}

//	--------------------------------------------------------------------------------
int CvArea::getNumTiles() const
{
	return m_iNumTiles;
}

//	--------------------------------------------------------------------------------
void CvArea::changeNumTiles(int iChange)
{
	m_iNumTiles = (m_iNumTiles + iChange);
	CvAssert(m_iNumTiles >= 0);
}


//	--------------------------------------------------------------------------------
int CvArea::getNumOwnedTiles() const
{
	return m_iNumOwnedTiles;
}


//	--------------------------------------------------------------------------------
int CvArea::getNumUnownedTiles() const
{
	return (m_iNumTiles - getNumOwnedTiles());
}


//	--------------------------------------------------------------------------------
void CvArea::changeNumOwnedTiles(int iChange)
{
	m_iNumOwnedTiles = (m_iNumOwnedTiles + iChange);
	CvAssert(getNumOwnedTiles() >= 0);
	CvAssert(getNumUnownedTiles() >= 0);
}


//	--------------------------------------------------------------------------------
int CvArea::getNumRiverEdges() const
{
	return m_iNumRiverEdges;
}


//	--------------------------------------------------------------------------------
void CvArea::changeNumRiverEdges(int iChange)
{
	m_iNumRiverEdges = (m_iNumRiverEdges + iChange);
	CvAssert(getNumRiverEdges() >= 0);
}


//	--------------------------------------------------------------------------------
int CvArea::getNumUnits() const
{
	return m_iNumUnits;
}


//	--------------------------------------------------------------------------------
int CvArea::getNumCities() const
{
	return m_iNumCities;
}


//	--------------------------------------------------------------------------------
int CvArea::getTotalPopulation() const
{
	return m_iTotalPopulation;
}


//	--------------------------------------------------------------------------------
int CvArea::getNumStartingPlots() const
{
	return m_iNumStartingPlots;
}

//	--------------------------------------------------------------------------------
void CvArea::changeNumStartingPlots(int iChange)
{
	m_iNumStartingPlots = m_iNumStartingPlots + iChange;
	CvAssert(getNumStartingPlots() >= 0);
}

#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
int CvArea::GetNumBadPlots() const
{
	return m_iBadPlots;
}
void CvArea::CalcNumBadPlots()
{
	CvPlot* pLoopPlot;
	int iI;

	if(isWater())
	{
		return;
	}

	m_iBadPlots = 0;

	CvMap& theMap = GC.getMap();
	int iNumPlots = theMap.numPlots();
	for(iI = 0; iI < iNumPlots; iI++)
	{
		pLoopPlot = theMap.plotByIndexUnchecked(iI);

		if(pLoopPlot->getArea() == GetID())
		{
			if( (pLoopPlot->getTerrainType() == TERRAIN_SNOW && pLoopPlot->getResourceType() == NO_RESOURCE && pLoopPlot->getFeatureType() == NO_FEATURE && !pLoopPlot->isHills()) || 
				(pLoopPlot->getFeatureType() == FEATURE_ICE) || 
				(pLoopPlot->getTerrainType() == TERRAIN_DESERT && pLoopPlot->getResourceType() == NO_RESOURCE && pLoopPlot->getFeatureType() == NO_FEATURE && !pLoopPlot->isHills() ))
			{
				m_iBadPlots++;
			}
		}
	}
}
#endif

//	--------------------------------------------------------------------------------
/// How many Natural Wonders are there in the world?
int CvArea::GetNumNaturalWonders() const
{
	return m_iNumNaturalWonders;
}

//	--------------------------------------------------------------------------------
/// Changes count on how many Natural Wonders are there in the world
void CvArea::ChangeNumNaturalWonders(int iChange)
{
	m_iNumNaturalWonders += iChange;
}

//	--------------------------------------------------------------------------------
//	Is the area made up of water plots?
bool CvArea::isWater() const
{
	return m_bWater;
}

//	--------------------------------------------------------------------------------
//	Is the area made up of mountain plots?
bool CvArea::IsMountains() const
{
	return m_bMountains;
}

//	--------------------------------------------------------------------------------
void CvArea::SetMountains(bool bValue)
{
	if(m_bMountains != bValue)
	{
		m_bMountains = bValue;
	}
}


//	--------------------------------------------------------------------------------
//	How many units for the specified player are in the area?
int CvArea::getUnitsPerPlayer(PlayerTypes eIndex) const
{
	map<PlayerTypes, int>::const_iterator it = m_aiUnitsPerPlayer.find(eIndex);
	if (it != m_aiUnitsPerPlayer.end())
		return it->second;

	return 0;
}


//	--------------------------------------------------------------------------------
void CvArea::changeUnitsPerPlayer(PlayerTypes eIndex, int iChange)
{
	map<PlayerTypes, int>::iterator it = m_aiUnitsPerPlayer.find(eIndex);
	if (it != m_aiUnitsPerPlayer.end())
	{
		it->second += iChange;
		if (it->second == 0)
			m_aiUnitsPerPlayer.erase(it);
	}
	else
		m_aiUnitsPerPlayer.insert(make_pair(eIndex,iChange));

	m_iNumUnits += iChange;
}

//	--------------------------------------------------------------------------------
//	How many units that are the enemy of the specified player area in the area?
int CvArea::getEnemyUnits(PlayerTypes eIndex) const
{
	if (eIndex == NO_PLAYER)
		return 0;

	int iRtnValue = 0;
	CvPlayer& kPlayer = GET_PLAYER(eIndex);

	for (map<PlayerTypes, int>::const_iterator it = m_aiUnitsPerPlayer.begin(); it != m_aiUnitsPerPlayer.end(); ++it)
		if (kPlayer.IsAtWarWith(it->first))
			iRtnValue += it->second;

	return iRtnValue;
}

//	--------------------------------------------------------------------------------
//	Get the number of cities for the specified player in the area
int CvArea::getCitiesPerPlayer(PlayerTypes eIndex) const
{
	map<PlayerTypes, int>::const_iterator it = m_aiCitiesPerPlayer.find(eIndex);
	if (it != m_aiCitiesPerPlayer.end())
		return it->second;

	return 0;
}


//	--------------------------------------------------------------------------------
void CvArea::changeCitiesPerPlayer(PlayerTypes eIndex, int iChange)
{
	map<PlayerTypes, int>::iterator it = m_aiCitiesPerPlayer.find(eIndex);
	if (it != m_aiCitiesPerPlayer.end())
	{
		it->second += iChange;
		if (it->second == 0)
			m_aiCitiesPerPlayer.erase(it);
	}
	else
		m_aiCitiesPerPlayer.insert(make_pair(eIndex,iChange));

	m_iNumCities += iChange;
}


//	--------------------------------------------------------------------------------
//	What is the population for the specified player in the area?
int CvArea::getPopulationPerPlayer(PlayerTypes eIndex) const
{
	map<PlayerTypes, int>::const_iterator it = m_aiPopulationPerPlayer.find(eIndex);
	if (it != m_aiPopulationPerPlayer.end())
		return it->second;

	return 0;
}


//	--------------------------------------------------------------------------------
void CvArea::changePopulationPerPlayer(PlayerTypes eIndex, int iChange)
{
	map<PlayerTypes, int>::iterator it = m_aiPopulationPerPlayer.find(eIndex);
	if (it != m_aiPopulationPerPlayer.end())
	{
		it->second += iChange;
		if (it->second == 0)
			m_aiPopulationPerPlayer.erase(it);
	}
	else
		m_aiPopulationPerPlayer.insert(make_pair(eIndex,iChange));

	m_iTotalPopulation += iChange;
}


//	--------------------------------------------------------------------------------
//	What is the number of free specialist in the area for the specified player?
int CvArea::getFreeSpecialist(PlayerTypes eIndex) const
{
	map<PlayerTypes, int>::const_iterator it = m_aiFreeSpecialist.find(eIndex);
	if (it != m_aiFreeSpecialist.end())
		return it->second;

	return 0;
}


//	--------------------------------------------------------------------------------
void CvArea::changeFreeSpecialist(PlayerTypes eIndex, int iChange)
{
	map<PlayerTypes, int>::iterator it = m_aiFreeSpecialist.find(eIndex);
	if (it != m_aiFreeSpecialist.end())
	{
		it->second += iChange;
		if (it->second == 0)
			m_aiFreeSpecialist.erase(it);
	}
	else
		m_aiFreeSpecialist.insert(make_pair(eIndex,iChange));
}

//	---------------------------------------------------------------------------
//	Get the total 'found' value for the area.  This is the desirability of
//	an area for settlement.
int CvArea::getTotalFoundValue() const
{
	return m_iTotalFoundValue;
}


//	--------------------------------------------------------------------------------
void CvArea::setTotalFoundValue(int iNewValue)
{
	//avoid overflow
	m_iTotalFoundValue = min(INT_MAX/2,max(0,iNewValue));
	CvAssert(m_iTotalFoundValue >= 0);
}

//	--------------------------------------------------------------------------------
//	Get the number of revealed tiles (plots) for the specified team
int CvArea::getNumRevealedTiles(TeamTypes eIndex) const
{
	map<TeamTypes, int>::const_iterator it = m_aiNumRevealedTiles.find(eIndex);
	if (it != m_aiNumRevealedTiles.end())
		return it->second;

	return 0;
}

//	---------------------------------------------------------------------------
//	Get the number of un-revealed tiles (plots) for the specified team
int CvArea::getNumUnrevealedTiles(TeamTypes eIndex) const
{
	return (m_iNumTiles - getNumRevealedTiles(eIndex));
}


//	--------------------------------------------------------------------------------
void CvArea::changeNumRevealedTiles(TeamTypes eIndex, int iChange)
{
	map<TeamTypes, int>::iterator it = m_aiNumRevealedTiles.find(eIndex);
	if (it != m_aiNumRevealedTiles.end())
	{
		it->second += iChange;
		if (it->second == 0)
			m_aiNumRevealedTiles.erase(it);
	}
	else
		m_aiNumRevealedTiles.insert(make_pair(eIndex,iChange));
}

//	--------------------------------------------------------------------------------
CvCity* CvArea::getTargetCity(PlayerTypes eIndex) const
{
	map<PlayerTypes, IDInfo>::const_iterator it = m_aTargetCities.find(eIndex);
	if (it != m_aTargetCities.end())
		return ::GetPlayerCity(it->second);

	return NULL;
}

//	--------------------------------------------------------------------------------
void CvArea::setTargetCity(PlayerTypes eIndex, CvCity* pNewValue)
{
	map<PlayerTypes, IDInfo>::iterator it = m_aTargetCities.find(eIndex);
	if (it != m_aTargetCities.end())
	{
		if (pNewValue == NULL)
			m_aTargetCities.erase(it);
		else
			it->second = IDInfo(eIndex,pNewValue->GetID());
	}
	else if (pNewValue != NULL)
		m_aTargetCities.insert(make_pair(eIndex,IDInfo(eIndex,pNewValue->GetID())));
}


//	--------------------------------------------------------------------------------
int CvArea::getYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2) const
{
	map<PlayerTypes, vector<int>>::const_iterator it = m_aaiYieldRateModifier.find(eIndex1);
	if (it != m_aaiYieldRateModifier.end())
		return it->second[eIndex2];

	return 0;
}


//	--------------------------------------------------------------------------------
void CvArea::changeYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	if (iChange == 0)
		return;

	map<PlayerTypes,vector<int>>::iterator it = m_aaiYieldRateModifier.find(eIndex1);
	if (it != m_aaiYieldRateModifier.end())
	{
		it->second[eIndex2] += iChange;

		//check if it's all zero and can be removed
		int iSum = 0;
		for (vector<int>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
			iSum += abs(*it2);

		if ( iSum == 0)
			m_aaiYieldRateModifier.erase(it);
	}
	else
		m_aaiYieldRateModifier.insert(make_pair(eIndex1,vector<int>(NUM_YIELD_TYPES,0)));


	CvPlayer& thisPlayer = GET_PLAYER(eIndex1);
	thisPlayer.invalidateYieldRankCache(eIndex2);

	if(thisPlayer.getTeam() == GC.getGame().getActiveTeam())
	{
		GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
	}
}


//	--------------------------------------------------------------------------------
//	Get the number of resources of the specified type in the area.
int CvArea::getNumResources(ResourceTypes eResource) const
{
	map<ResourceTypes, int>::const_iterator it = m_aiNumResources.find(eResource);
	if (it != m_aiNumResources.end())
		return it->second;

	return 0;
}


//	---------------------------------------------------------------------------
//	Get the total number of resources in the specified area.
int CvArea::getNumTotalResources() const
{
	int iTotal = 0;

	for (map<ResourceTypes, int>::const_iterator it = m_aiNumResources.begin(); it != m_aiNumResources.end(); ++it)
		iTotal += it->second;

	return iTotal;
}


//	--------------------------------------------------------------------------------
void CvArea::changeNumResources(ResourceTypes eResource, int iChange)
{
	map<ResourceTypes, int>::iterator it = m_aiNumResources.find(eResource);
	if (it != m_aiNumResources.end())
	{
		it->second += iChange;
		if (it->second == 0)
			m_aiNumResources.erase(it);
	}
	else
		m_aiNumResources.insert(make_pair(eResource,iChange));
}


//	--------------------------------------------------------------------------------
//	Get the number of the specified improvements in the area
int CvArea::getNumImprovements(ImprovementTypes eImprovement) const
{
	map<ImprovementTypes, int>::const_iterator it = m_aiNumImprovements.find(eImprovement);
	if (it != m_aiNumImprovements.end())
		return it->second;

	return 0;
}


//	--------------------------------------------------------------------------------
void CvArea::changeNumImprovements(ImprovementTypes eImprovement, int iChange)
{
	map<ImprovementTypes, int>::iterator it = m_aiNumImprovements.find(eImprovement);
	if (it != m_aiNumImprovements.end())
	{
		it->second += iChange;
		if (it->second == 0)
			m_aiNumImprovements.erase(it);
	}
	else
		m_aiNumImprovements.insert(make_pair(eImprovement,iChange));
}

//	--------------------------------------------------------------------------------
CvAreaBoundaries CvArea::getAreaBoundaries() const
{
	return m_Boundaries;
}

//	--------------------------------------------------------------------------------
void CvArea::setAreaBoundaries(CvAreaBoundaries newBoundaries)
{
	m_Boundaries = newBoundaries;
}

//	--------------------------------------------------------------------------------
/// What are the top and bottom latitudes of this Area (passed by reference)
void CvArea::GetTopAndBottomLatitudes(int& iTopLatitude, int& iBottomLatitude)
{
	int iGridHeight = GC.getMap().getGridHeight();
	if(GC.getMap().isWrapX() || !(GC.getMap().isWrapY()))
	{
		iTopLatitude = ((m_Boundaries.m_iNorthEdge * 100) / iGridHeight);
		iBottomLatitude = ((m_Boundaries.m_iSouthEdge * 100) / iGridHeight);
	}
	else
	{
		iTopLatitude = ((m_Boundaries.m_iEastEdge * 100) / iGridHeight);
		iBottomLatitude = ((m_Boundaries.m_iWestEdge * 100) / iGridHeight);
	}

	int iMapTop = GC.getMap().getTopLatitude();
	int iMapBottom = GC.getMap().getBottomLatitude();

	iTopLatitude = ((iTopLatitude * (iMapTop - iMapBottom)) / 100);
	iBottomLatitude = ((iBottomLatitude * (iMapTop - iMapBottom)) / 100);

	iTopLatitude = abs(iTopLatitude + iMapBottom);
	iBottomLatitude = abs(iBottomLatitude + iMapBottom);
}

//	--------------------------------------------------------------------------------
/// What is the largest latitude value of this Area?
int CvArea::GetAreaMaxLatitude()
{
	int iTopLatitude, iBottomLatitude;
	GetTopAndBottomLatitudes(iTopLatitude, iBottomLatitude);

	return max(iTopLatitude, iBottomLatitude);
}

//	--------------------------------------------------------------------------------
/// What is the smallest latitude value of this Area?
int CvArea::GetAreaMinLatitude()
{
	int iTopLatitude, iBottomLatitude;
	GetTopAndBottomLatitudes(iTopLatitude, iBottomLatitude);

	return min(iTopLatitude, iBottomLatitude);
}

//	--------------------------------------------------------------------------------
void CvArea::read(FDataStream& kStream)
{
	// Init saved data
	reset();

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	kStream >> m_iID;
	kStream >> m_iNumTiles;
	kStream >> m_iNumOwnedTiles;
	kStream >> m_iNumRiverEdges;
	kStream >> m_iNumUnits;
	kStream >> m_iNumCities;
	kStream >> m_iTotalPopulation;
	kStream >> m_iNumStartingPlots;

	kStream >> m_iNumNaturalWonders;
	kStream >> m_iTotalFoundValue;
	kStream >> m_iBadPlots;

	kStream >> m_Boundaries.m_iNorthEdge;
	kStream >> m_Boundaries.m_iSouthEdge;
	kStream >> m_Boundaries.m_iEastEdge;
	kStream >> m_Boundaries.m_iWestEdge;

	kStream >> m_bWater;
	kStream >> m_bMountains;

	kStream >> m_aiUnitsPerPlayer;
	kStream >> m_aiCitiesPerPlayer;
	kStream >> m_aiPopulationPerPlayer;
	kStream >> m_aiFreeSpecialist;
	kStream >> m_aiNumRevealedTiles;
	kStream >> m_aTargetCities;
	kStream >> m_aaiYieldRateModifier;

	kStream >> m_aiNumResources;
	kStream >> m_aiNumImprovements;
}


//	--------------------------------------------------------------------------------
void CvArea::write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	kStream << m_iID;
	kStream << m_iNumTiles;
	kStream << m_iNumOwnedTiles;
	kStream << m_iNumRiverEdges;
	kStream << m_iNumUnits;
	kStream << m_iNumCities;
	kStream << m_iTotalPopulation;
	kStream << m_iNumStartingPlots;

	kStream << m_iNumNaturalWonders;
	kStream << m_iTotalFoundValue;
	kStream << m_iBadPlots;

	kStream << m_Boundaries.m_iNorthEdge;
	kStream << m_Boundaries.m_iSouthEdge;
	kStream << m_Boundaries.m_iEastEdge;
	kStream << m_Boundaries.m_iWestEdge;

	kStream << m_bWater;
	kStream << m_bMountains;

	kStream << m_aiUnitsPerPlayer;
	kStream << m_aiCitiesPerPlayer;
	kStream << m_aiPopulationPerPlayer;
	kStream << m_aiFreeSpecialist;
	kStream << m_aiNumRevealedTiles;
	kStream << m_aTargetCities;
	kStream << m_aaiYieldRateModifier;

	kStream << m_aiNumResources;
	kStream << m_aiNumImprovements;
}

//	--------------------------------------------------------------------------------
FDataStream& operator<<(FDataStream& saveTo, const CvArea& readFrom)
{
	readFrom.write(saveTo);
	return saveTo;
}

//	--------------------------------------------------------------------------------
FDataStream& operator>>(FDataStream& loadFrom, CvArea& writeTo)
{
	writeTo.read(loadFrom);
	return loadFrom;
}
