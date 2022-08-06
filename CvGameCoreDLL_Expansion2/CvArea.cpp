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

	m_Boundaries.m_iNorthEdge = -1;
	m_Boundaries.m_iSouthEdge = -1;
	m_Boundaries.m_iEastEdge = -1;
	m_Boundaries.m_iWestEdge = -1;

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

void CvArea::UpdateBadPlotsCount(CvPlot* pPlot)
{
	if (!pPlot || pPlot->getArea() == GetID())
		return;

	if(isWater())
		return;

	if( (pPlot->getTerrainType() == TERRAIN_SNOW && pPlot->getResourceType() == NO_RESOURCE && pPlot->getFeatureType() == NO_FEATURE && !pPlot->isHills()) || 
		(pPlot->getFeatureType() == FEATURE_ICE) || 
		(pPlot->getTerrainType() == TERRAIN_DESERT && pPlot->getResourceType() == NO_RESOURCE && pPlot->getFeatureType() == NO_FEATURE && !pPlot->isHills() ))
	{
		m_iBadPlots++;
	}
}

void CvArea::FindBoundaries(const std::vector<bool>& occupiedCols, const std::vector<bool>& occupiedRows)
{
	//we know an area is contiguous, so there should be exactly one rising and one falling edge in each direction
	int colRise=-1; //west
	int colFall=-1; //east
	int rowRise=-1; //south
	int rowFall=-1; //north

	if (GC.getMap().isWrapX())
	{
		for (int i = 0; i < (int)occupiedCols.size(); i++)
		{
			//neighbor with wrapping
			int j = (i + 1) % occupiedCols.size();

			if (occupiedCols[i] == false && occupiedCols[j] == true)
				colRise = j;
			if (occupiedCols[i] == true && occupiedCols[j] == false)
				colFall = i;
		}
	}
	else
	{
		if (occupiedCols[0])
			colRise = 0;
		for (int i = 0; i < (int)occupiedCols.size()-1; i++)
		{
			//neighbor w/o wrapping
			int j = (i + 1);

			if (occupiedCols[i] == false && occupiedCols[j] == true)
				colRise = j;
			if (occupiedCols[i] == true && occupiedCols[j] == false)
				colFall = i;
		}
		if (occupiedCols[occupiedCols.size()-1])
			colFall = occupiedCols.size()-1;
	}

	if (GC.getMap().isWrapY())
	{
		for (int i = 0; i < (int)occupiedRows.size(); i++)
		{
			//neighbor with wrapping
			int j = (i + 1) % occupiedRows.size();

			if (occupiedRows[i] == false && occupiedRows[j] == true)
				rowRise = j;
			if (occupiedRows[i] == true && occupiedRows[j] == false)
				rowFall = i;
		}
	}
	else
	{
		if (occupiedRows[0])
			rowRise = 0;
		for (int i = 0; i < (int)occupiedRows.size()-1; i++)
		{
			//neighbor w/o wrapping
			int j = (i + 1);

			if (occupiedRows[i] == false && occupiedRows[j] == true)
				rowRise = j;
			if (occupiedRows[i] == true && occupiedRows[j] == false)
				rowFall = i;
		}
		if (occupiedRows[occupiedRows.size()-1])
			rowFall = occupiedRows.size()-1;
	}

	//catch degenerate cases
	if (colFall == colRise && colFall == -1)
	{
		//we did not find an edge ... may be all zeros or all ones?
		if (occupiedCols[0])
		{
			//this definition works with or without wrapping
			colRise = 0;
			colFall = (int)occupiedCols.size() - 1;
		}
		else
			//invalid
			return;
	}
	if (rowFall == rowRise && rowFall == -1)
	{
		//we did not find an edge ... may be all zeros or all ones?
		if (occupiedRows[0])
		{
			//this definition works with or without wrapping
			rowRise = 0;
			rowFall = (int)occupiedRows.size() - 1;
		}
		else
			//invalid
			return;
	}

	//now it's possible that we are crossing the coordinate wrap boundary
	//but we want the rise before the fall always. so fix it.
	if (colFall < colRise)
	{
		assert(GC.getMap().isWrapX());
		colFall += occupiedCols.size();
	}

	if (rowFall < rowRise)
	{
		assert(GC.getMap().isWrapY());
		rowFall += occupiedRows.size();
	}

	//store
	m_Boundaries.m_iEastEdge = colFall;
	m_Boundaries.m_iWestEdge = colRise;
	m_Boundaries.m_iNorthEdge = rowFall;
	m_Boundaries.m_iSouthEdge = rowRise;
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
	std::map<PlayerTypes, int>::const_iterator it = m_aiUnitsPerPlayer.find(eIndex);
	if (it != m_aiUnitsPerPlayer.end())
		return it->second;

	return 0;
}


//	--------------------------------------------------------------------------------
void CvArea::changeUnitsPerPlayer(PlayerTypes eIndex, int iChange)
{
	std::map<PlayerTypes, int>::iterator it = m_aiUnitsPerPlayer.find(eIndex);
	if (it != m_aiUnitsPerPlayer.end())
	{
		it->second += iChange;
		if (it->second == 0)
			m_aiUnitsPerPlayer.erase(it);
	}
	else
		m_aiUnitsPerPlayer.insert(std::make_pair(eIndex,iChange));

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

	for (std::map<PlayerTypes, int>::const_iterator it = m_aiUnitsPerPlayer.begin(); it != m_aiUnitsPerPlayer.end(); ++it)
		if (kPlayer.IsAtWarWith(it->first))
			iRtnValue += it->second;

	return iRtnValue;
}

//	--------------------------------------------------------------------------------
//	Get the number of cities for the specified player in the area
int CvArea::getCitiesPerPlayer(PlayerTypes eIndex) const
{
	std::map<PlayerTypes, int>::const_iterator it = m_aiCitiesPerPlayer.find(eIndex);
	if (it != m_aiCitiesPerPlayer.end())
		return it->second;

	return 0;
}


//	--------------------------------------------------------------------------------
void CvArea::changeCitiesPerPlayer(PlayerTypes eIndex, int iChange)
{
	std::map<PlayerTypes, int>::iterator it = m_aiCitiesPerPlayer.find(eIndex);
	if (it != m_aiCitiesPerPlayer.end())
	{
		it->second += iChange;
		if (it->second == 0)
			m_aiCitiesPerPlayer.erase(it);
	}
	else
		m_aiCitiesPerPlayer.insert(std::make_pair(eIndex,iChange));

	m_iNumCities += iChange;
}


//	--------------------------------------------------------------------------------
//	What is the population for the specified player in the area?
int CvArea::getPopulationPerPlayer(PlayerTypes eIndex) const
{
	std::map<PlayerTypes, int>::const_iterator it = m_aiPopulationPerPlayer.find(eIndex);
	if (it != m_aiPopulationPerPlayer.end())
		return it->second;

	return 0;
}


//	--------------------------------------------------------------------------------
void CvArea::changePopulationPerPlayer(PlayerTypes eIndex, int iChange)
{
	std::map<PlayerTypes, int>::iterator it = m_aiPopulationPerPlayer.find(eIndex);
	if (it != m_aiPopulationPerPlayer.end())
	{
		it->second += iChange;
		if (it->second == 0)
			m_aiPopulationPerPlayer.erase(it);
	}
	else
		m_aiPopulationPerPlayer.insert(std::make_pair(eIndex,iChange));

	m_iTotalPopulation += iChange;
}


//	--------------------------------------------------------------------------------
//	What is the number of free specialist in the area for the specified player?
int CvArea::getFreeSpecialist(PlayerTypes eIndex) const
{
	std::map<PlayerTypes, int>::const_iterator it = m_aiFreeSpecialist.find(eIndex);
	if (it != m_aiFreeSpecialist.end())
		return it->second;

	return 0;
}


//	--------------------------------------------------------------------------------
void CvArea::changeFreeSpecialist(PlayerTypes eIndex, int iChange)
{
	std::map<PlayerTypes, int>::iterator it = m_aiFreeSpecialist.find(eIndex);
	if (it != m_aiFreeSpecialist.end())
	{
		it->second += iChange;
		if (it->second == 0)
			m_aiFreeSpecialist.erase(it);
	}
	else
		m_aiFreeSpecialist.insert(std::make_pair(eIndex,iChange));
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
	m_iTotalFoundValue = std::min(INT_MAX/2,std::max(0,iNewValue));
	CvAssert(m_iTotalFoundValue >= 0);
}

//	--------------------------------------------------------------------------------
//	Get the number of revealed tiles (plots) for the specified team
int CvArea::getNumRevealedTiles(TeamTypes eIndex) const
{
	std::map<TeamTypes, int>::const_iterator it = m_aiNumRevealedTiles.find(eIndex);
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
	std::map<TeamTypes, int>::iterator it = m_aiNumRevealedTiles.find(eIndex);
	if (it != m_aiNumRevealedTiles.end())
	{
		it->second += iChange;
		if (it->second == 0)
			m_aiNumRevealedTiles.erase(it);
	}
	else
		m_aiNumRevealedTiles.insert(std::make_pair(eIndex,iChange));
}

//	--------------------------------------------------------------------------------
CvCity* CvArea::getTargetCity(PlayerTypes eIndex) const
{
	std::map<PlayerTypes, IDInfo>::const_iterator it = m_aTargetCities.find(eIndex);
	if (it != m_aTargetCities.end())
		return ::GetPlayerCity(it->second);

	return NULL;
}

//	--------------------------------------------------------------------------------
void CvArea::setTargetCity(PlayerTypes eIndex, CvCity* pNewValue)
{
	std::map<PlayerTypes, IDInfo>::iterator it = m_aTargetCities.find(eIndex);
	if (it != m_aTargetCities.end())
	{
		if (pNewValue == NULL)
			m_aTargetCities.erase(it);
		else
			it->second = IDInfo(eIndex,pNewValue->GetID());
	}
	else if (pNewValue != NULL)
		m_aTargetCities.insert(std::make_pair(eIndex,IDInfo(eIndex,pNewValue->GetID())));
}


//	--------------------------------------------------------------------------------
int CvArea::getYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2) const
{
	std::map<PlayerTypes, std::vector<int>>::const_iterator it = m_aaiYieldRateModifier.find(eIndex1);
	if (it != m_aaiYieldRateModifier.end())
		return it->second[eIndex2];

	return 0;
}


//	--------------------------------------------------------------------------------
void CvArea::changeYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	if (iChange == 0)
		return;

	std::map<PlayerTypes,std::vector<int>>::iterator it = m_aaiYieldRateModifier.find(eIndex1);
	if (it != m_aaiYieldRateModifier.end())
	{
		it->second[eIndex2] += iChange;

		//check if it's all zero and can be removed
		int iSum = 0;
		for (std::vector<int>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
			iSum += abs(*it2);

		if ( iSum == 0)
			m_aaiYieldRateModifier.erase(it);
	}
	else
		m_aaiYieldRateModifier.insert(std::make_pair(eIndex1,std::vector<int>(NUM_YIELD_TYPES,0)));


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
	std::map<ResourceTypes, int>::const_iterator it = m_aiNumResources.find(eResource);
	if (it != m_aiNumResources.end())
		return it->second;

	return 0;
}


//	---------------------------------------------------------------------------
//	Get the total number of resources in the specified area.
int CvArea::getNumTotalResources() const
{
	int iTotal = 0;

	for (std::map<ResourceTypes, int>::const_iterator it = m_aiNumResources.begin(); it != m_aiNumResources.end(); ++it)
		iTotal += it->second;

	return iTotal;
}


//	--------------------------------------------------------------------------------
void CvArea::changeNumResources(ResourceTypes eResource, int iChange)
{
	std::map<ResourceTypes, int>::iterator it = m_aiNumResources.find(eResource);
	if (it != m_aiNumResources.end())
	{
		it->second += iChange;
		if (it->second == 0)
			m_aiNumResources.erase(it);
	}
	else
		m_aiNumResources.insert(std::make_pair(eResource,iChange));
}


//	--------------------------------------------------------------------------------
//	Get the number of the specified improvements in the area
int CvArea::getNumImprovements(ImprovementTypes eImprovement) const
{
	std::map<ImprovementTypes, int>::const_iterator it = m_aiNumImprovements.find(eImprovement);
	if (it != m_aiNumImprovements.end())
		return it->second;

	return 0;
}


//	--------------------------------------------------------------------------------
void CvArea::changeNumImprovements(ImprovementTypes eImprovement, int iChange)
{
	std::map<ImprovementTypes, int>::iterator it = m_aiNumImprovements.find(eImprovement);
	if (it != m_aiNumImprovements.end())
	{
		it->second += iChange;
		if (it->second == 0)
			m_aiNumImprovements.erase(it);
	}
	else
		m_aiNumImprovements.insert(std::make_pair(eImprovement,iChange));
}

//	--------------------------------------------------------------------------------
CvAreaBoundaries CvArea::getAreaBoundaries() const
{
	return m_Boundaries;
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

	return std::max(iTopLatitude, iBottomLatitude);
}

//	--------------------------------------------------------------------------------
/// What is the smallest latitude value of this Area?
int CvArea::GetAreaMinLatitude()
{
	int iTopLatitude, iBottomLatitude;
	GetTopAndBottomLatitudes(iTopLatitude, iBottomLatitude);

	return std::min(iTopLatitude, iBottomLatitude);
}

//	--------------------------------------------------------------------------------
template<typename Area, typename Visitor>
void CvArea::Serialize(Area& area, Visitor& visitor)
{
	visitor(area.m_iID);
	visitor(area.m_iNumTiles);
	visitor(area.m_iNumOwnedTiles);
	visitor(area.m_iNumRiverEdges);
	visitor(area.m_iNumUnits);
	visitor(area.m_iNumCities);
	visitor(area.m_iTotalPopulation);
	visitor(area.m_iNumStartingPlots);

	visitor(area.m_iNumNaturalWonders);
	visitor(area.m_iTotalFoundValue);
	visitor(area.m_iBadPlots);

	visitor(area.m_Boundaries.m_iNorthEdge);
	visitor(area.m_Boundaries.m_iSouthEdge);
	visitor(area.m_Boundaries.m_iEastEdge);
	visitor(area.m_Boundaries.m_iWestEdge);

	visitor(area.m_bWater);
	visitor(area.m_bMountains);

	visitor(area.m_aiUnitsPerPlayer);
	visitor(area.m_aiCitiesPerPlayer);
	visitor(area.m_aiPopulationPerPlayer);
	visitor(area.m_aiFreeSpecialist);
	visitor(area.m_aiNumRevealedTiles);
	visitor(area.m_aTargetCities);
	visitor(area.m_aaiYieldRateModifier);

	visitor(area.m_aiNumResources);
	visitor(area.m_aiNumImprovements);
}

//	--------------------------------------------------------------------------------
void CvArea::read(FDataStream& kStream)
{
	// Init saved data
	reset();

	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}


//	--------------------------------------------------------------------------------
void CvArea::write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
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
