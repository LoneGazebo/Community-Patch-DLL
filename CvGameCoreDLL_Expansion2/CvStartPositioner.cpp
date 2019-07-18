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

bool PlotTooCloseToAnotherCiv(CvPlot* pPlot, int iReqSeparation);
bool PlotMeetsFoodRequirement(CvPlot* pPlot, PlayerTypes ePlayer, int iFoodRequirement);

//=====================================
// CvStartPositioner
//=====================================
/// Constructor
CvStartPositioner::CvStartPositioner(CvSiteEvaluatorForStart * pSiteEvaluator)
{
	m_iRequiredSeparation = 0;
	m_iBestFoundValueOnMap = 0;
	m_pSiteEvaluator = pSiteEvaluator;
	if (m_pSiteEvaluator)
		m_pSiteEvaluator->ComputeFlavorMultipliers(NULL);  // Ignore flavors; this sets them to 1
}

/// Destructor
CvStartPositioner::~CvStartPositioner(void)
{
}

void CvStartPositioner::Run(int iNumRegions)
{
	// Divide the map into equal fertility plots
	DivideMapIntoRegions(iNumRegions);

	// Compute the value of a city in each plot
	ComputeFoundValues();

	// Position the players
	AssignStartingLocations();
}

/// Chop map into a number of regions of equal fertility
void CvStartPositioner::DivideMapIntoRegions(int iNumRegions)
{
	CUSTOMLOG("CvStartPositioner::DivideMapIntoRegions(%i)", iNumRegions);
	int iLoop;
	int iNumRegionsPlaced = 0;

	// Initialize
	m_ContinentVector.clear();
	m_StartRegionVector.clear();

	// Compute fertility for each plot
	ComputeTileFertilityValues();

	// Loop through each continent adding it to our list
	for(CvArea* pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
	{
		// Throw out oceans and desert islands
		if(pLoopArea->getNumTiles() >= GC.getMIN_START_AREA_TILES())
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
			std::stable_sort(m_ContinentVector.begin(), m_ContinentVector.end());

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
	std::stable_sort(m_StartRegionVector.begin(), m_StartRegionVector.end());
}

/// Compute the value of having a city at each plot
void CvStartPositioner::ComputeFoundValues()
{
	CUSTOMLOG("CvStartPositioner::ComputeFoundValues()");

	// Progress through entire map
	unsigned int iSum = 0, iValidPlots = 0;
	for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		// Store found value in player 1 slot for now
		//   (Normally shouldn't be using a hard-coded player reference, but here in the pre-game initialization it is safe to do so.
		//    Allows us to reuse this data storage instead of jamming even more data into the CvPlot class that will never be used at run-time).
		CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);

		int iValue = m_pSiteEvaluator->PlotFoundValue(pLoopPlot, NULL, vector<int>());
		pLoopPlot->setFoundValue((PlayerTypes)1, iValue);

		if(iValue > m_iBestFoundValueOnMap)
		{
			m_iBestFoundValueOnMap = iValue;
		}

		if (iValue > 0)
		{
			//avoid overflow - also we just need a rough estimate
			iSum += (iValue/1000);
			iValidPlots++;
		}
	}

	int iAvg = (iSum / iValidPlots) * 1000;
	OutputDebugString( CvString::format("Average city site value without flavor is %d\n",iAvg).c_str() );
}

/// Take into account handicaps to rank the "draft order" for start positions
vector<CvPlayerStartRank> GetPlayerStartOrder()
{
	vector<CvPlayerStartRank> playerOrder;

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
				playerRank.m_iRank += GC.getGame().getJonRandNum(10, "starting position");
				playerOrder.push_back(playerRank);
			}
		}
	}

	// Sort by rank
	std::stable_sort(playerOrder.begin(), playerOrder.end());

	return playerOrder;
}

/// Pick start positions for all civs
void CvStartPositioner::AssignStartingLocations()
{
	CUSTOMLOG("CvStartPositioner::AssignStartingLocations()");
	CvString strString;
	unsigned int iNextRegion = 0;
	int iPlayersPlaced = 0;
	int iMajorCivs = 0;

	vector<CvPlayerStartRank> playerOrder = GetPlayerStartOrder();
	for(size_t i = 0; i < playerOrder.size(); ++i)
	{
		if(!GET_PLAYER((PlayerTypes)playerOrder[i].m_iPlayerID).isMinorCiv())
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
			bool success = AddCivToRegion(playerOrder[iPlayersPlaced].m_iPlayerID, m_StartRegionVector[iNextRegion], false);
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
		std::stable_sort(m_StartRegionVector.begin(), m_StartRegionVector.end());

		// Reduce start distance by 1 (the previous condition could get stuck)
		m_iRequiredSeparation--;
		iNextRegion = 0;

		while(iPlayersPlaced < iMajorCivs && iNextRegion < m_StartRegionVector.size())
		{
			if(m_StartRegionVector[iNextRegion].m_bLargeEnoughForMajorCiv)
			{
				strString.Format("Trying to place major civ with reduced separation of %d", m_iRequiredSeparation);
				LogStartPositionMessage(strString);
				bool success = AddCivToRegion(playerOrder[iPlayersPlaced].m_iPlayerID, m_StartRegionVector[iNextRegion], m_iRequiredSeparation < iHalfMinimumDist);
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
	std::stable_sort(m_StartRegionVector.begin(), m_StartRegionVector.end());
	while(iPlayersPlaced < (int)playerOrder.size() && iNextRegion < m_StartRegionVector.size())
	{
		strString.Format("Trying to place minor civ with full separation of %d", m_iRequiredSeparation);
		LogStartPositionMessage(strString);
		bool success = AddCivToRegion(playerOrder[iPlayersPlaced].m_iPlayerID, m_StartRegionVector[iNextRegion], false);
		if(success)
		{
			iPlayersPlaced++;
			m_StartRegionVector[iNextRegion].m_iNumCivsPlaced++;

			// Sort again, now that the region in slot 0 has another civ placed there
			std::stable_sort(m_StartRegionVector.begin(), m_StartRegionVector.end());
			iNextRegion = 0;
		}

		// Failed, proceed through list of regions
		else
		{
			iNextRegion++;
		}
	}

	// MINOR CIVS AGAIN (those that couldn't be placed normal start distance apart)
	while(iPlayersPlaced < (int)playerOrder.size() && m_iRequiredSeparation >= 0)
	{
		// Resort by fertility (based on the fact that some of these regions are filling up)
		std::stable_sort(m_StartRegionVector.begin(), m_StartRegionVector.end());

		// Reduce start distance by 10%
		m_iRequiredSeparation--;
		iNextRegion = 0;

		while(iPlayersPlaced < (int)playerOrder.size() && iNextRegion < m_StartRegionVector.size())
		{
			strString.Format("Trying to place minor civ with reduced separation of %d", m_iRequiredSeparation);
			LogStartPositionMessage(strString);
			bool success = AddCivToRegion(playerOrder[iPlayersPlaced].m_iPlayerID, m_StartRegionVector[iNextRegion], false);
			if(success)
			{
				iPlayersPlaced++;
				m_StartRegionVector[iNextRegion].m_iNumCivsPlaced++;

				// Sort again, now that the region in slot 0 has another civ placed there
				std::stable_sort(m_StartRegionVector.begin(), m_StartRegionVector.end());
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
	// Set all area fertilities to 0
	int iLoop;
	for(CvArea* pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
	{
		pLoopArea->setTotalFoundValue(0);
	}

	// Now process through the map
	for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);

		// Compute fertility and save off in player 0's found value slot
		//   (Normally shouldn't be using a hard-coded player reference, but here in the pre-game initialization it is safe to do so.
		//    Allows us to reuse this data storage instead of jamming even more data into the CvPlot class that will never be used at run-time).
		int iFertility = m_pSiteEvaluator->PlotFertilityValue(pLoopPlot);
		pLoopPlot->setFoundValue((PlayerTypes)0, iFertility);

		if(iFertility > 0)
		{
			// Add to total for area
			CvArea* pArea = GC.getMap().getArea(pLoopPlot->getArea());
			CvAssert(pArea);
			if(!pArea) 
				continue;
			pArea->setTotalFoundValue(pArea->getTotalFoundValue() + iFertility);
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

#if defined(MOD_GLOBAL_MAX_MAJOR_CIVS)
		// CUSTOMLOG("CvStartPositioner::SubdivideRegion into %i divisions (by algorithm)", iNumDivisions);
		// Gives a slightly different pattern to the hard-coded switch below, but it really doesn't matter
		if (iNumDivisions > 32) {
			iNumDivides = 2;
			iLaterSubdivisions = (iNumDivisions+1) / 2;
		} else if (iNumDivisions % 3 == 0) {
			iNumDivides = 3;
			iLaterSubdivisions = iNumDivisions / 3;
		} else if (iNumDivisions % 2 == 0) {
			iNumDivides = 2;
			iLaterSubdivisions = iNumDivisions / 2;
		} else if ((iNumDivisions+1) % 3 == 0) {
			iNumDivides = 3;
			iLaterSubdivisions = (iNumDivisions+1) / 3;
		} else {
			iNumDivides = 2;
			iLaterSubdivisions = (iNumDivisions+1) / 2;
		}

		// DUH!!!  I'm so stooopid that I can't work out a simple algorithm for this, jeez Firaxis, employ some apes with brains!
#else
		// CUSTOMLOG("CvStartPositioner::SubdivideRegion into %i divisions (by switch)", iNumDivisions);
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
#endif
		// CUSTOMLOG(" divide now %i, divide later %i", iNumDivides, iLaterSubdivisions);

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
		while(uiFertilitySoFar < uiTargetFertility && iNorthEdge <= region->m_Boundaries.m_iNorthEdge)
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
		while(uiFertilitySoFar < uiTargetFertility && iEastEdge <= region->m_Boundaries.m_iEastEdge)
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
				// Retrieve fertility from player 0's found value slot
				//   (Normally shouldn't be using a hard-coded player reference, but here in the pre-game initialization it is safe to do so.
				//    Allows us to reuse this data storage instead of jamming even more data into the CvPlot class that will never be used at run-time).
				int iValue = pPlot->getFoundValue((PlayerTypes)0);
				if (iValue>0)
					rtnValue += iValue;
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
	PlayerTypes ePlayer = (PlayerTypes)iPlayerIndex;
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
				if((bRelaxFoodReq && PlotMeetsFoodRequirement(pLoopPlot, ePlayer, iMinorFoodReq))
				        || (bIsMinorCiv && PlotMeetsFoodRequirement(pLoopPlot, ePlayer, iMinorFoodReq))
				        || PlotMeetsFoodRequirement(pLoopPlot, ePlayer, iMajorFoodReq))
				{
					// Retrieve found value from player 1's found value slot
					//   (Normally shouldn't be using a hard-coded player reference, but here in the pre-game initialization it is safe to do so.
					//    Allows us to reuse this data storage instead of jamming even more data into the CvPlot class that will never be used at run-time).
					uiPlotFoundValue = pLoopPlot->getFoundValue((PlayerTypes)1);
					if (uiPlotFoundValue<1)
						continue;

#if defined(MOD_BALANCE_CORE_SETTLER)
					if ((bIsMinorCiv && GC.getMinorCivInfo(eMinorCivType)->GetMinorCivTrait() == MINOR_CIV_TRAIT_MARITIME) ||
						(!bIsMinorCiv && GC.getCivilizationInfo(GET_PLAYER((PlayerTypes)iPlayerIndex).getCivilizationType())->isCoastalCiv()))
					{
#else
					// If we're a Maritime Minor Civ then decrease the value of non-coastal starts
					if(bIsMinorCiv)
					{
						if(GC.getMinorCivInfo(eMinorCivType)->GetMinorCivTrait() == MINOR_CIV_TRAIT_MARITIME)
#endif // MOD_BALANCE_CORE_SETTLER
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
						if(!PlotTooCloseToAnotherCiv(pLoopPlot,m_iRequiredSeparation))
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
bool PlotTooCloseToAnotherCiv(CvPlot* pPlot, int iRequiredSeparation)
{
	bool rtnValue = false;

	CvAssert(pPlot);
	if(!pPlot)
		return rtnValue;

	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			CvPlot* pStartPlot = GET_PLAYER((PlayerTypes)iI).getStartingPlot();
			if(pStartPlot != NULL)
			{
				// If in same area, use full distance
				if(pStartPlot->getArea() == pPlot->getArea())
				{
					if(plotDistance(pPlot->getX(), pPlot->getY(),
					                pStartPlot->getX(), pStartPlot->getY()) < iRequiredSeparation)
					{
						return true;
					}
				}

				// If in another area, use a fraction of full distance
				else
				{
					int iSeparationIfOnAnotherContinent = iRequiredSeparation * GC.getMIN_DISTANCE_OTHER_AREA_PERCENT() / 100;
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
bool PlotMeetsFoodRequirement(CvPlot* pPlot, PlayerTypes ePlayer, int iFoodRequirement)
{
	bool bFoundFoodPlot = false;

	CvAssert(pPlot);
	if(!pPlot)
		return bFoundFoodPlot;

	for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));
		if(pLoopPlot == NULL)
		{
			return false;   // Right at map edge
		}
		else
		{
			if(pLoopPlot->calculateNatureYield(YIELD_FOOD, ePlayer, false) >= iFoodRequirement)
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

	iRange /= (AVG_CITY_PLOTS / 2); // Use the average number of plots per city (this is the default 3-ring value)
	iRange /= iExpectedCities;

	// Used to be a Python hook (minStartingDistanceModifier) here

	CUSTOMLOG("CvStartPositioner::StartingPlotRange() = %i", std::max(iRange, GC.getMIN_CIV_STARTING_DISTANCE()));
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

struct SStartRegion
{
	int iID;
	vector<int> vPlots; //indices of the plots making up this region
	vector<int> vNeighbors; //indices of neighboring regions (not plots)
	int iTotalWorth;

	SStartRegion(int iIndex = 0, int iPlotWorth = 0)
	{  
		iID = iIndex;
		vPlots = vector<int>(1, iIndex);
		iTotalWorth = iPlotWorth;
	}

	void mergeWith(SStartRegion& other) 
	{ 
		iTotalWorth += other.iTotalWorth; 
		vPlots.insert(vPlots.end(), other.vPlots.begin(), other.vPlots.end()); 
		vNeighbors.insert(vNeighbors.end(), other.vNeighbors.begin(), other.vNeighbors.end());

		//remove obsolete neighbors
		sort(vNeighbors.begin(), vNeighbors.end());
		vNeighbors.erase(remove(vNeighbors.begin(), vNeighbors.end(), iID), vNeighbors.end());
		vNeighbors.erase(remove(vNeighbors.begin(), vNeighbors.end(), other.iID), vNeighbors.end());
		vNeighbors.erase(unique(vNeighbors.begin(), vNeighbors.end()), vNeighbors.end());
	}

	bool operator<(const SStartRegion& rhs) const { return iTotalWorth > rhs.iTotalWorth; }
};

CvStartPositionerMerge::CvStartPositionerMerge(CvSiteEvaluatorForStart * pSiteEvaluator)
{
	m_pSiteEvaluator = pSiteEvaluator;
}

CvStartPositionerMerge::~CvStartPositionerMerge(void)
{
}

int getRegionDistanceMeasure(const SStartRegion& a, const SStartRegion& b)
{
	if (a.vPlots.empty() || b.vPlots.empty())
		return 0;

	int ax = 0;
	int ay = 0;
	int bx = 0;
	int by = 0;
	//get the center of mass for both regions
	for (size_t i = 0; i < a.vPlots.size(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(a.vPlots[i]);
		ax += pPlot->getX();
		ay += pPlot->getY();
	}
	for (size_t i = 0; i < b.vPlots.size(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(b.vPlots[i]);
		bx += pPlot->getX();
		by += pPlot->getY();
	}
	ax = (ax * 100) / a.vPlots.size();
	ay = (ay * 100) / a.vPlots.size();
	bx = (bx * 100) / b.vPlots.size();
	by = (by * 100) / b.vPlots.size();

	return (int)sqrtf(float(ax - bx)*(ax - bx) + float(ay - by)*(ay - by));
}

int findNeighborIdToMerge(map<int, SStartRegion>::iterator self, const map<int,SStartRegion>& regions)
{
	vector<int> neighbors = self->second.vNeighbors;

	int iWorstNeighborScore = INT_MAX;
	int iWorstNeighborRegionId = -1;
	for (size_t i = 0; i < neighbors.size(); i++)
	{
		map<int, SStartRegion>::const_iterator neighbor = regions.find(neighbors[i]);
		if (neighbor == regions.end())
			continue; //may happen if the neighbor is already very large and has been removed

		//consider distance as well to encourage compact regions
		int iDist = getRegionDistanceMeasure(self->second, neighbor->second);
		if (neighbor->second.iTotalWorth*iDist < iWorstNeighborScore)
		{
			iWorstNeighborScore = neighbor->second.iTotalWorth * iDist;
			iWorstNeighborRegionId = neighbor->first;
		}
	}

	return iWorstNeighborRegionId;
}

void CvStartPositionerMerge::Run(int iNumRegionsRequired)
{
	map<int,SStartRegion> regions;
	int iGlobalPlotWorth = 0;

	//set up initial regions
	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
		int iPlotWorth = m_pSiteEvaluator->PlotFertilityValue(pLoopPlot,true);
		
		//the region id is the plot index of the original plot
		if (iPlotWorth > 0)
		{
			regions[iI] = SStartRegion(iI, iPlotWorth);
			iGlobalPlotWorth += iPlotWorth;
		}
	}

	//set up initial neighborhood relations
	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
		map<int, SStartRegion>::iterator it = regions.find(pLoopPlot->GetPlotIndex());
		if (it == regions.end())
			continue; //can happen for bad plots without corresponding region

		CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(pLoopPlot);
		for (int i = 0; i < NUM_DIRECTION_TYPES; i++)
		{
			if (!aNeighbors[i])
				continue;

			map<int, SStartRegion>::iterator it = regions.find(aNeighbors[i]->GetPlotIndex());
			if (it != regions.end())
				it->second.vNeighbors.push_back( pLoopPlot->GetPlotIndex() );
		}
	}

	//start the merge process
	vector<SStartRegion> regionsWeCantMerge;
	vector<SStartRegion> regionsLargeEnough;

	while ( regions.size() > (size_t)iNumRegionsRequired )
	{
		//find the worst region we can merge
		map<int, SStartRegion>::iterator worstRegion = regions.end();

		for (map<int, SStartRegion>::iterator it = regions.begin(); it != regions.end(); ++it)
		{
			if (it->second.iTotalWorth > iGlobalPlotWorth/iNumRegionsRequired)
			{
				regionsLargeEnough.push_back(it->second);
				it = regions.erase(it);
				if (it == regions.end())
					break;
			}

			if (it->second.vNeighbors.empty())
			{
				regionsWeCantMerge.push_back(it->second);
				it = regions.erase(it);
				if (it == regions.end())
					break;
			}

			if (worstRegion == regions.end() || it->second.iTotalWorth < worstRegion->second.iTotalWorth)
				worstRegion = it;
		}
		if (worstRegion == regions.end())
			break; //error

		//merge the worst region with a suitable neighbor
		int neighborId = findNeighborIdToMerge(worstRegion,regions);
		if (neighborId == -1)
			break; //error
		map<int, SStartRegion>::iterator neighbor = regions.find(neighborId);

		//debugging
		if (true)
		{
			CvString msg = CvString::format("merging id %d (size %d, score %d) with id %d (size %d, score %d). nregions %d\n",
				worstRegion->second.iID, worstRegion->second.vPlots.size(), worstRegion->second.iTotalWorth, 
				neighbor->second.iID, neighbor->second.vPlots.size(), neighbor->second.iTotalWorth, 
				regions.size());
			OutputDebugString(msg.c_str());
		}

		//do the actual merge
		neighbor->second.mergeWith( worstRegion->second );

		//clean up neighbor references
		for (size_t i = 0; i < worstRegion->second.vNeighbors.size(); i++)
		{
			map<int, SStartRegion>::iterator it2 = regions.find( worstRegion->second.vNeighbors[i] );
			if (it2 == regions.end())
				continue; //should not happen!

			replace(it2->second.vNeighbors.begin(), it2->second.vNeighbors.end(), worstRegion->first, neighbor->first);
			//there might be a duplicate now, but that's ok and will be fixed after the next merge
		}

		//we don't need you around here anymore
		regions.erase(worstRegion);
	}

	//log the result
	if (true)
	{
		FILogFile* pLog = LOGFILEMGR.GetLog("StartRegions.txt", FILogFile::kDontTimeStamp);
		if (pLog)
		{
			pLog->Msg("#x,y,terrain,region\n");
			for (map<int, SStartRegion>::iterator it = regions.begin(); it != regions.end(); ++it)
			{
				for (size_t j = 0; j < it->second.vPlots.size(); j++)
				{
					CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(it->second.vPlots[j]);
					CvString dump = CvString::format("%d,%d,%d,%d\n",
						pPlot->getX(), pPlot->getY(), pPlot->getTerrainType(), it->first);

					pLog->Msg(dump.c_str());
				}
			}
		}
	}

	//now map players to regions
	//there should be one region per major player
	//the minors have to squeeze in later
	//...
}

