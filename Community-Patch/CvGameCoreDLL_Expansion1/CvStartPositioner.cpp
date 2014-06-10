/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreUtils.h"
#include "CvStartPositioner.h"
#include "CvMinorCivAI.h"

// include this after all other headers!
#include "LintFree.h"

//=====================================
// CvStartPositioner
//=====================================
/// Constructor
CvStartPositioner::CvStartPositioner(void)
{
	m_iRequiredSeparation = 0;
	m_iBestFoundValueOnMap = 0;
}

/// Destructor
CvStartPositioner::~CvStartPositioner(void)
{
}

/// Initialize
void CvStartPositioner::Init(CvSiteEvaluatorForStart* pSiteEvaluator)
{
	CvAssert(pSiteEvaluator);
	if(!pSiteEvaluator)
		return;

	m_pSiteEvaluator = pSiteEvaluator;
	m_pSiteEvaluator->ComputeFlavorMultipliers(NULL);  // Ignore flavors; this sets them to 1
}

/// Chop map into a number of regions of equal fertility
void CvStartPositioner::DivideMapIntoRegions(int iNumRegions)
{
	CvArea* pLoopArea(NULL);
	int iLoop;
	int iNumRegionsPlaced = 0;

	// Initialize
	m_ContinentVector.clear();
	m_StartRegionVector.clear();

	// Compute fertility for each plot
	ComputeTileFertilityValues();

	// Loop through each continent adding it to our list
	for(pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
	{
		// Throw out oceans and desert islands
		if(pLoopArea->getTotalFoundValue() > 0 && pLoopArea->getNumTiles() >= GC.getMIN_START_AREA_TILES())
		{
			CvContinent continent;
			continent.SetFertility(pLoopArea->getTotalFoundValue());
			continent.SetArea(pLoopArea->GetID());
			m_ContinentVector.push_back(continent);
		}
	}

	if(m_ContinentVector.size() > 0)
	{
		// Assign all the regions to continents
		while(iNumRegionsPlaced < iNumRegions)
		{
			std::sort(m_ContinentVector.begin(), m_ContinentVector.end());

			// Add a region to the first in our list
			m_ContinentVector[0].AddRegion();
			iNumRegionsPlaced++;
		}
	}

	// Divide the continents according to our algorithm
	for(unsigned int iI=0; iI < m_ContinentVector.size(); iI++)
	{
		DivideContinentIntoRegions(m_ContinentVector[iI]);
	}

	// Sort the regions by fertility
	std::sort(m_StartRegionVector.begin(), m_StartRegionVector.end());
}

/// Compute the value of having a city at each plot
void CvStartPositioner::ComputeFoundValues()
{
	CvPlot* pLoopPlot(NULL);

	// Progress through entire map
	for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		// Store in player 1 slot for now
		//   (Normally shouldn't be using a hard-coded player reference, but here in the pre-game initialization it is safe to do so.
		//    Allows us to reuse this data storage instead of jamming even more data into the CvPlot class that will never be used at run-time).
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
		CvAssert(pLoopPlot);
		if(!pLoopPlot) continue;
		int iValue = m_pSiteEvaluator->PlotFoundValue(pLoopPlot, NULL);
		pLoopPlot->setFoundValue((PlayerTypes)1, iValue);

		if(iValue > m_iBestFoundValueOnMap)
		{
			m_iBestFoundValueOnMap = iValue;
		}
	}
}

/// Take into account handicaps to rank the "draft order" for start positions
void CvStartPositioner::RankPlayerStartOrder()
{
	// Clear rankings
	m_PlayerOrder.clear();

	// Add each player
	for(int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		CvPlayerAI& player = GET_PLAYER((PlayerTypes)iI);

		if(player.isAlive())
		{
			if(player.getStartingPlot() == NULL)
			{
				CvPlayerStartRank playerRank;
				playerRank.m_iPlayerID = iI;

				// Following assumes that StartingLocationPercent in Handicap XML rises in increments of 10!
				//    We put all the major civs first (values 1 to 100), using a random number to rank players at the same handicap
				//    Then we put all the minor civs in next (values 100 to 1000).
				playerRank.m_iRank = player.getHandicapInfo().getStartingLocationPercent();
				if(player.isMinorCiv())
				{
					playerRank.m_iRank *= 10;
				}
				playerRank.m_iRank += GC.getGame().getJonRandNum(10, "Player Start Plot Draft Order");
				m_PlayerOrder.push_back(playerRank);
			}
		}
	}

	// Sort by rank
	std::sort(m_PlayerOrder.begin(), m_PlayerOrder.end());
}

/// Pick start positions for all civs
void CvStartPositioner::AssignStartingLocations()
{
	CvString strString;
	unsigned int iNextRegion = 0;
	int iPlayersPlaced = 0;
	int iMajorCivs = 0;
	for(size_t i = 0; i < m_PlayerOrder.size(); ++i)
	{
		if(!GET_PLAYER((PlayerTypes)m_PlayerOrder[i].m_iPlayerID).isMinorCiv())
			iMajorCivs++;
	}

	// MAJOR CIVS
	m_iRequiredSeparation = StartingPlotRange();

	// ***** Debug *****
	if(GC.getMap().getWorldSize() == WORLDSIZE_DEBUG)
	{
		m_iRequiredSeparation = 1;
	}

	while(iPlayersPlaced < iMajorCivs && iNextRegion < m_StartRegionVector.size())
	{
		if(m_StartRegionVector[iNextRegion].m_bLargeEnoughForMajorCiv)
		{
			strString.Format("Trying to place major civ with full separation of %d", m_iRequiredSeparation);
			LogStartPositionMessage(strString);
			bool success = AddCivToRegion(m_PlayerOrder[iPlayersPlaced].m_iPlayerID, m_StartRegionVector[iNextRegion], false);
			if(success)
			{
				iPlayersPlaced++;
				m_StartRegionVector[iNextRegion].m_iNumCivsPlaced++;
			}
		}
		iNextRegion++;
	}

	// MAJOR CIVS AGAIN (those that couldn't be placed normal start distance apart)

	int iHalfMinimumDist = GC.getMIN_CIV_STARTING_DISTANCE() / 2;
	while(iPlayersPlaced < iMajorCivs && m_iRequiredSeparation >= 0)
	{
		// Resort by fertility (based on the fact that some of these regions are filling up)
		std::sort(m_StartRegionVector.begin(), m_StartRegionVector.end());

		// Reduce start distance by 1 (the previous condition could get stuck)
		m_iRequiredSeparation--;
		iNextRegion = 0;

		while(iPlayersPlaced < iMajorCivs && iNextRegion < m_StartRegionVector.size())
		{
			if(m_StartRegionVector[iNextRegion].m_bLargeEnoughForMajorCiv)
			{
				strString.Format("Trying to place major civ with reduced separation of %d", m_iRequiredSeparation);
				LogStartPositionMessage(strString);
				bool success = AddCivToRegion(m_PlayerOrder[iPlayersPlaced].m_iPlayerID, m_StartRegionVector[iNextRegion], m_iRequiredSeparation < iHalfMinimumDist);
				if(success)
				{
					iPlayersPlaced++;
					m_StartRegionVector[iNextRegion].m_iNumCivsPlaced++;
				}
			}
			iNextRegion++;
		}
	}

	CvAssertMsg(iPlayersPlaced == iMajorCivs, "AssignStartingLocations() can't find enough starting locations for major civs");

	// MINOR CIVS
	m_iRequiredSeparation = StartingPlotRange();

	// Resort by fertility (based on the fact that some of these regions are filling up)
	std::sort(m_StartRegionVector.begin(), m_StartRegionVector.end());
	while(iPlayersPlaced < (int)m_PlayerOrder.size() && iNextRegion < m_StartRegionVector.size())
	{
		strString.Format("Trying to place minor civ with full separation of %d", m_iRequiredSeparation);
		LogStartPositionMessage(strString);
		bool success = AddCivToRegion(m_PlayerOrder[iPlayersPlaced].m_iPlayerID, m_StartRegionVector[iNextRegion], false);
		if(success)
		{
			iPlayersPlaced++;
			m_StartRegionVector[iNextRegion].m_iNumCivsPlaced++;

			// Sort again, now that the region in slot 0 has another civ placed there
			std::sort(m_StartRegionVector.begin(), m_StartRegionVector.end());
			iNextRegion = 0;
		}

		// Failed, proceed through list of regions
		else
		{
			iNextRegion++;
		}
	}

	// MINOR CIVS AGAIN (those that couldn't be placed normal start distance apart)
	while(iPlayersPlaced < (int)m_PlayerOrder.size() && m_iRequiredSeparation >= 0)
	{
		// Resort by fertility (based on the fact that some of these regions are filling up)
		std::sort(m_StartRegionVector.begin(), m_StartRegionVector.end());

		// Reduce start distance by 10%
		m_iRequiredSeparation--;
		iNextRegion = 0;

		while(iPlayersPlaced < (int)m_PlayerOrder.size() && iNextRegion < m_StartRegionVector.size())
		{
			strString.Format("Trying to place minor civ with reduced separation of %d", m_iRequiredSeparation);
			LogStartPositionMessage(strString);
			bool success = AddCivToRegion(m_PlayerOrder[iPlayersPlaced].m_iPlayerID, m_StartRegionVector[iNextRegion], false);
			if(success)
			{
				iPlayersPlaced++;
				m_StartRegionVector[iNextRegion].m_iNumCivsPlaced++;

				// Sort again, now that the region in slot 0 has another civ placed there
				std::sort(m_StartRegionVector.begin(), m_StartRegionVector.end());
				iNextRegion = 0;
			}

			// Failed, proceed through list of regions
			else
			{
				iNextRegion++;
			}
		}
	}
}

/// Find the region containing this plot (or -1 if no start region does)
int CvStartPositioner::GetRegion(int iX, int iY)
{
	int iRegion = 0;

	while(iRegion < (int)m_StartRegionVector.size())
	{
		if(m_StartRegionVector[iRegion].Contains(iX, iY))
		{
			return iRegion;
		}
		iRegion++;
	}

	return -1;
}

// PRIVATE METHODS

/// Compute the fertility of each tile on the map
void CvStartPositioner::DivideContinentIntoRegions(CvContinent continent)
{
	// Create a start region out of the entire continent
	CvStartRegion region;
	region.m_uiFertility = continent.GetFertility();
	region.m_iAreaID = continent.GetArea();
	region.m_Boundaries = GC.getMap().getArea(continent.GetArea())->getAreaBoundaries();

	// Make sure this is a continent that is getting a start region
	if(continent.GetNumRegions() > 0)
	{
		// If so, call recursive routine to subdivide it appropriately
		SubdivideRegion(region, continent.GetNumRegions());
	}

	// If the continent is too small for a major civ, it still could be useful for a minor
	else
	{
		region.m_bLargeEnoughForMajorCiv = false;
		m_StartRegionVector.push_back(region);
	}
}

/// Compute the fertility of each tile on the map
void CvStartPositioner::ComputeTileFertilityValues()
{
	CvArea* pLoopArea(NULL);
	int iLoop;
	CvPlot* pLoopPlot(NULL);
	int iI;
	int uiFertility;

	// Set all area fertilities to 0
	for(pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
	{
		pLoopArea->setTotalFoundValue(0);
	}

	// Now process through the map
	for(iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
		CvAssert(pLoopPlot);
		if(!pLoopPlot) continue;

		// Compute fertility and save off in player 0's found value slot
		//   (Normally shouldn't be using a hard-coded player reference, but here in the pre-game initialization it is safe to do so.
		//    Allows us to reuse this data storage instead of jamming even more data into the CvPlot class that will never be used at run-time).
		uiFertility = m_pSiteEvaluator->PlotFertilityValue(pLoopPlot);
		pLoopPlot->setFoundValue((PlayerTypes)0, uiFertility);

		if(uiFertility > 0)
		{
			// Add to total for area
			CvArea* pArea = GC.getMap().getArea(pLoopPlot->getArea());
			CvAssert(pArea);
			if(!pArea) continue;
			pArea->setTotalFoundValue(pArea->getTotalFoundValue() + uiFertility);
		}
	}
}

/// Recursive algorithm that chops regions up as required
void CvStartPositioner::SubdivideRegion(CvStartRegion region, int iNumDivisions)
{
	int iNumDivides = 0;
	int iLaterSubdivisions = 0;

	// Break recursion when number of divisions is down to 1
	if(iNumDivisions == 1)
	{
		// Store this as a final region
		m_StartRegionVector.push_back(region);
	}

	// Need to subdivide
	else if(iNumDivisions > 1)
	{
		// See if region is taller or wider
		bool bTaller = true;
		if((region.m_Boundaries.m_iNorthEdge - region.m_Boundaries.m_iSouthEdge) <
		        (region.m_Boundaries.m_iEastEdge - region.m_Boundaries.m_iWestEdge))
		{
			bTaller = false;
		}

		// If number of divisions is greater than 3...
		//
		// Number       First Divide     Each Subdivision
		// ------       ------------     ----------------
		//   4                2                 2
		//  5-6               3                 2
		//  7-8               2                 4
		//   9                3                 3
		// 10-12              3                 4
		// 13-16              2                 8
		// 17-18              3                 6
		// 19-20              2                10
		// 21-22              3                 8
		switch(iNumDivisions)
		{
		case 2:
			iNumDivides = 2;
			iLaterSubdivisions = 1;
			break;
		case 3:
			iNumDivides = 3;
			iLaterSubdivisions = 1;
			break;
		case 4:
			iNumDivides = 2;
			iLaterSubdivisions = 2;
			break;
		case 5:
		case 6:
			iNumDivides = 3;
			iLaterSubdivisions = 2;
			break;
		case 7:
		case 8:
			iNumDivides = 2;
			iLaterSubdivisions = 4;
			break;
		case 9:
			iNumDivides = 3;
			iLaterSubdivisions = 3;
			break;
		case 10:
		case 11:
		case 12:
			iNumDivides = 3;
			iLaterSubdivisions = 4;
			break;
		case 13:
		case 14:
		case 15:
		case 16:
			iNumDivides = 2;
			iLaterSubdivisions = 8;
			break;
		case 17:
		case 18:
			iNumDivides = 3;
			iLaterSubdivisions = 6;
			break;
		case 19:
		case 20:
			iNumDivides = 2;
			iLaterSubdivisions = 10;
			break;
		case 21:
		case 22:
			iNumDivides = 3;
			iLaterSubdivisions = 8;
			break;
		default:
			CvAssertMsg(false, "Trying to create regions for more than 18 major civs.");
		}

		if(iNumDivides == 2)
		{
			CvStartRegion secondRegion;
			ChopIntoTwoRegions(bTaller, &region, &secondRegion, 50);
			SubdivideRegion(region, iLaterSubdivisions);
			SubdivideRegion(secondRegion, iLaterSubdivisions);
		}
		else if(iNumDivides == 3)
		{
			CvStartRegion secondRegion;
			CvStartRegion thirdRegion;
			ChopIntoThreeRegions(bTaller, &region, &secondRegion, &thirdRegion);
			SubdivideRegion(region, iLaterSubdivisions);
			SubdivideRegion(secondRegion, iLaterSubdivisions);
			SubdivideRegion(thirdRegion, iLaterSubdivisions);
		}
	}
}

/// Performs the mechanics of dividing a region into two equal fertility subregions
void CvStartPositioner::ChopIntoTwoRegions(bool bTaller, CvStartRegion* region, CvStartRegion* secondRegion, int iChopPercent)
{
	CvAssert(region);
	if(!region) return;
	CvAssert(secondRegion);
	if(!secondRegion) return;

	int uiTargetFertility;
	int uiFertilitySoFar = 0;

	// Do some initialization on second region
	secondRegion->m_iAreaID = region->m_iAreaID;

	// How much fertility do we want in the first region?
	uiTargetFertility = region->m_uiFertility * iChopPercent / 100;

	// Taller than wide
	if(bTaller)
	{
		// We know that the second region will have the same width and same north edge
		secondRegion->m_Boundaries.m_iEastEdge = region->m_Boundaries.m_iEastEdge;
		secondRegion->m_Boundaries.m_iWestEdge = region->m_Boundaries.m_iWestEdge;
		secondRegion->m_Boundaries.m_iNorthEdge = region->m_Boundaries.m_iNorthEdge;

		// Move up one row at a time until we have exceeded the target fertility
		int iNorthEdge = region->m_Boundaries.m_iSouthEdge;
		while(uiFertilitySoFar < uiTargetFertility)
		{
			uiFertilitySoFar += ComputeRowFertility(region->m_iAreaID,
			                                        region->m_Boundaries.m_iWestEdge, region->m_Boundaries.m_iEastEdge, iNorthEdge, iNorthEdge);
			iNorthEdge++;
		}

		// Set edges
		region->m_Boundaries.m_iNorthEdge = iNorthEdge - 1;
		secondRegion->m_Boundaries.m_iSouthEdge = iNorthEdge;
	}

	// Wider than tall
	else
	{
		// We know that the second region will have the same height and same east edge
		secondRegion->m_Boundaries.m_iNorthEdge = region->m_Boundaries.m_iNorthEdge;
		secondRegion->m_Boundaries.m_iSouthEdge = region->m_Boundaries.m_iSouthEdge;
		secondRegion->m_Boundaries.m_iEastEdge = region->m_Boundaries.m_iEastEdge;

		// Move right one column at a time until we have exceeded the target fertility
		int iEastEdge = region->m_Boundaries.m_iWestEdge;
		while(uiFertilitySoFar < uiTargetFertility)
		{
			uiFertilitySoFar += ComputeRowFertility(region->m_iAreaID,
			                                        iEastEdge, iEastEdge, region->m_Boundaries.m_iSouthEdge, region->m_Boundaries.m_iNorthEdge);
			iEastEdge++;
		}

		// Set edges
		region->m_Boundaries.m_iEastEdge = iEastEdge - 1;
		secondRegion->m_Boundaries.m_iWestEdge = iEastEdge;
	}

	// Finish initializing regions
	secondRegion->m_uiFertility = region->m_uiFertility - uiFertilitySoFar;
	region->m_uiFertility = uiFertilitySoFar;
}

/// Performs the mechanics of dividing a region into three equal fertility subregions
void CvStartPositioner::ChopIntoThreeRegions(bool bTaller, CvStartRegion* region, CvStartRegion* secondRegion, CvStartRegion* thirdRegion)
{
	CvAssert(region);
	if(!region) return;
	CvAssert(secondRegion);
	if(!secondRegion) return;
	CvAssert(thirdRegion);
	if(!thirdRegion) return;

	// Chop off the first third
	ChopIntoTwoRegions(bTaller, region, secondRegion, 33);

	// Chop the bigger piece in half
	ChopIntoTwoRegions(bTaller, secondRegion, thirdRegion, 50);
}

/// Totals up the fertility of one row of data
int CvStartPositioner::ComputeRowFertility(int iAreaID, int xMin, int xMax, int yMin, int yMax)
{
	int rtnValue = 0;

	for(int iRow = yMin; iRow <= yMax; iRow++)
	{
		for(int iCol = xMin; iCol <= xMax; iCol++)
		{
			CvPlot* pPlot = GC.getMap().plot(iCol, iRow);

			if(pPlot && pPlot->getArea() == iAreaID)
			{
				// Retrieve from player 0's found value slot
				//   (Normally shouldn't be using a hard-coded player reference, but here in the pre-game initialization it is safe to do so.
				//    Allows us to reuse this data storage instead of jamming even more data into the CvPlot class that will never be used at run-time).
				rtnValue += pPlot->getFoundValue((PlayerTypes)0);
			}
		}
	}

	return rtnValue;
}

/// Pick a start position for a civ within a specific region
bool CvStartPositioner::AddCivToRegion(int iPlayerIndex, CvStartRegion region, bool bRelaxFoodReq)
{
	CvString strString;
	int uiBestFoundValue = 0;
	int uiPlotFoundValue;
	CvPlot* pBestPlot = NULL;
	CvPlot* pLoopPlot = NULL;
	int iMinorFoodReq = GC.getMINOR_CIV_FOOD_REQUIREMENT();
	int iMajorFoodReq = GC.getMAJOR_CIV_FOOD_REQUIREMENT();
	bool bIsMinorCiv = GET_PLAYER((PlayerTypes)iPlayerIndex).isMinorCiv();
	TeamTypes eTeam = GET_PLAYER((PlayerTypes)iPlayerIndex).getTeam();
	int iPercentOfBest = GC.getMIN_START_FOUND_VALUE_AS_PERCENT_OF_BEST();

	MinorCivTypes eMinorCivType = NO_MINORCIV;
	if(bIsMinorCiv)
	{
		eMinorCivType =GET_PLAYER((PlayerTypes) iPlayerIndex).GetMinorCivAI()->GetMinorCivType();
	}

	bool bDebugMap = GC.getMap().getWorldSize() == WORLDSIZE_DEBUG;

	// ***** Debug *****
	if(bDebugMap)
	{
		iMinorFoodReq = 0;
		iMajorFoodReq = 0;
	}

	// Find best found value in region
	for(int iRow = region.m_Boundaries.m_iSouthEdge; iRow <= region.m_Boundaries.m_iNorthEdge; iRow++)
	{
		for(int iCol = region.m_Boundaries.m_iWestEdge; iCol <= region.m_Boundaries.m_iEastEdge; iCol++)
		{
			pLoopPlot = GC.getMap().plot(iCol, iRow);

			// Make sure it actually *is* in region
			if(pLoopPlot && pLoopPlot->getArea() == region.m_iAreaID)
			{
				// Check food requirement
				if((bRelaxFoodReq && MeetsFoodRequirement(pLoopPlot, eTeam, iMinorFoodReq))
				        || (bIsMinorCiv && MeetsFoodRequirement(pLoopPlot, eTeam, iMinorFoodReq))
				        || MeetsFoodRequirement(pLoopPlot, eTeam, iMajorFoodReq))
				{
					// Retrieve from player 1's found value slot
					//   (Normally shouldn't be using a hard-coded player reference, but here in the pre-game initialization it is safe to do so.
					//    Allows us to reuse this data storage instead of jamming even more data into the CvPlot class that will never be used at run-time).
					uiPlotFoundValue = pLoopPlot->getFoundValue((PlayerTypes)1);

					// If we're a Maritime Minor Civ then decrease the value of non-coastal starts
					if(bIsMinorCiv)
					{
						if(GC.getMinorCivInfo(eMinorCivType)->GetMinorCivTrait() == MINOR_CIV_TRAIT_MARITIME)
						{
							if(!pLoopPlot->isCoastalLand(GC.getLAKE_MAX_AREA_SIZE()))
							{
								uiPlotFoundValue /= 2;
							}
						}
					}

					// Above the minimum found value?
					if(bDebugMap || ((int)uiPlotFoundValue > (int)m_iBestFoundValueOnMap * iPercentOfBest / 100))
					{
						// Check distance
						if(!TooCloseToAnotherCiv(pLoopPlot))
						{
							// Best so far?
							if(bDebugMap || uiPlotFoundValue > uiBestFoundValue)
							{
								uiBestFoundValue = uiPlotFoundValue;
								pBestPlot = pLoopPlot;
							}
						}
					}
				}
			}
		}
	}

	if(pBestPlot != NULL)
	{
		GET_PLAYER((PlayerTypes)iPlayerIndex).setStartingPlot(pBestPlot);
		strString.Format("Adding player, id = %d, plot x = %d, y = %d, fertility = %u",
		                 iPlayerIndex, pBestPlot->getX(), pBestPlot->getY(), uiBestFoundValue);
		LogStartPositionMessage(strString);
		return true;
	}
	else
	{
		return false;
	}
}

/// Returns true if illegal start position
bool CvStartPositioner::TooCloseToAnotherCiv(CvPlot* pPlot)
{
	int iI;
	bool rtnValue = false;
	CvPlot* pStartPlot(NULL);

	CvAssert(pPlot);
	if(!pPlot)
		return rtnValue;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			pStartPlot = GET_PLAYER((PlayerTypes)iI).getStartingPlot();
			if(pStartPlot != NULL)
			{
				// If in same area, use full distance
				if(pStartPlot->getArea() == pPlot->getArea())
				{
					if(plotDistance(pPlot->getX(), pPlot->getY(),
					                pStartPlot->getX(), pStartPlot->getY()) < m_iRequiredSeparation)
					{
						return true;
					}
				}

				// If in another area, use a fraction of full distance
				else
				{
					int iSeparationIfOnAnotherContinent = m_iRequiredSeparation * GC.getMIN_DISTANCE_OTHER_AREA_PERCENT() / 100;
					if(plotDistance(pPlot->getX(), pPlot->getY(),
					                pStartPlot->getX(), pStartPlot->getY()) < iSeparationIfOnAnotherContinent)
					{
						return true;
					}
				}
			}
		}
	}

	return rtnValue;
}

/// Does it have a good food plot?
bool CvStartPositioner::MeetsFoodRequirement(CvPlot* pPlot, TeamTypes eTeam, int iFoodRequirement)
{
	CvPlot* pLoopPlot(NULL);
	bool bFoundFoodPlot = false;

	CvAssert(pPlot);
	if(!pPlot)
		return bFoundFoodPlot;

	for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));
		if(pLoopPlot == NULL)
		{
			return false;   // Right at map edge
		}
		else
		{
			if(pLoopPlot->calculateNatureYield(YIELD_FOOD, eTeam, false) >= iFoodRequirement)
			{
				bFoundFoodPlot = true;
				break;
			}
		}
	}
	return bFoundFoodPlot;
}

/// How far apart should our civs start?
int CvStartPositioner::StartingPlotRange() const
{
	int iRange;
	int iNumMinors, iNumMajors;
	int iExpectedCities;

	// Start with the range as a percentage of the maximum path length across the map
	iRange = (GC.getMap().maxPlotDistance() + 10);
	iRange *= GC.getSTARTING_DISTANCE_PERCENT();
	iRange /= 100;

	// Now compute how many cities we expect on the map (we'll say just 1 for each minor civ)
	iNumMajors = GC.getGame().countMajorCivsAlive();
	iNumMinors = GC.getGame().countCivPlayersAlive() - iNumMajors;
	iExpectedCities = GC.getMap().getWorldInfo().getTargetNumCities() * iNumMajors;
	iExpectedCities += iNumMinors;
	iExpectedCities = std::max(1,iExpectedCities);

	// Adjust range compared to how many plots we have per city
	iRange *= GC.getMap().getLandPlots();
	iRange /= (NUM_CITY_PLOTS / 2); // NUM_CITY_PLOTS; -- I am assuming that some tiles will be watter or unused
	iRange /= iExpectedCities;

	// Used to be a Python hook (minStartingDistanceModifier) here

	return std::max(iRange, GC.getMIN_CIV_STARTING_DISTANCE());
}

/// Log current status of the operation
void CvStartPositioner::LogStartPositionMessage(CvString strMsg)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog("StartPositionLog.csv", FILogFile::kDontTimeStamp);
		pLog->Msg(strMsg);
	}
}