/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_START_POSITIONER_H
#define CIV5_START_POSITIONER_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvContinent
//!  \brief		A map area that might be the start area for one or more civs
//
//!  Key Attributes:
//!  - Created by CvStartPositioner
//!  - Used to decide how many civs are allocated to this continent
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvContinent
{
public:
	CvContinent()
	{
		m_uiFertility = 0;
		m_iRegionsAssigned = 0;
		m_uiFertilityNextRegion = 0;
		m_iAreaID = FFreeList::INVALID_INDEX;
	}

	CvContinent( const CvContinent& source )
	{
		m_uiFertility = source.m_uiFertility;
		m_iRegionsAssigned = source.m_iRegionsAssigned;
		m_uiFertilityNextRegion = source.m_uiFertilityNextRegion;
		m_iAreaID = source.m_iAreaID;
	}

	bool operator<(const CvContinent& continent)
	{
		return (m_uiFertilityNextRegion > continent.m_uiFertilityNextRegion);
	}

	void AddRegion()
	{
		m_iRegionsAssigned++;
		m_uiFertilityNextRegion = m_uiFertility / (m_iRegionsAssigned + 1);
	}

	int GetNumRegions() const
	{
		return m_iRegionsAssigned;
	}

	void SetFertility (int uiFertility)
	{
		m_uiFertility = uiFertility;
		m_uiFertilityNextRegion = uiFertility;
	}

	int GetFertility() const
	{
		return m_uiFertility;
	}

	void SetArea (int iAreaID)
	{
		m_iAreaID = iAreaID;
	}

	int GetArea () const
	{
		return m_iAreaID;
	}

private:
	int m_uiFertility;           // Total fertility of all plots in region
	int m_uiFertilityNextRegion; // Fertility if add one more region
	int m_iRegionsAssigned;		          // Number of regions
	int m_iAreaID;						  // Area ID
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvStartRegion
//!  \brief		A region of the map that can contain one major civ
//
//!  Key Attributes:
//!  - Created by CvStartPositioner
//!  - Each CvStartRegion should have approximately the same fertility
//!  - Several minor civs may also start within a region
//!  - We also make all islands/minor continents too small for a major civ into a start
//!    region since we may want a minor civ to start there
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvStartRegion
{
public:
	CvStartRegion()
	{
		m_uiFertility = 0;
		m_iAreaID = FFreeList::INVALID_INDEX;
		m_bLargeEnoughForMajorCiv = true;
		m_iNumCivsPlaced = 0;
	}

	CvStartRegion( const CvStartRegion& source )
	{
		m_Boundaries = source.m_Boundaries;
		m_uiFertility = source.m_uiFertility;
		m_iAreaID = source.m_iAreaID;
		m_bLargeEnoughForMajorCiv = source.m_bLargeEnoughForMajorCiv;
		m_iNumCivsPlaced = source.m_iNumCivsPlaced;
	}

	bool operator<(const CvStartRegion& continent)
	{
		return (m_uiFertility / (m_iNumCivsPlaced + 1) >
			    continent.m_uiFertility / (continent.m_iNumCivsPlaced + 1));
	}

	bool Contains(int iX, int iY)
	{
		if (iX > m_Boundaries.m_iEastEdge ||
			iX < m_Boundaries.m_iWestEdge ||
			iY > m_Boundaries.m_iNorthEdge ||
			iY < m_Boundaries.m_iSouthEdge)
		{
			return false;
		}
		return true;
	}

	CvAreaBoundaries m_Boundaries;  // Edges of region
	int m_uiFertility;     // Total fertility of all plots in region
	int m_iAreaID;				    // Area ID
	bool m_bLargeEnoughForMajorCiv;
	int m_iNumCivsPlaced;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvPlayerStartRank
//!  \brief		The ranking of how early this player should "draft" a start location
//
//!  Key Attributes:
//!  - Used in a vector when assigning start positions
//!  - Allows sorting by start position
//!  - Rank initially determined by field from Handicap XML ("StartingLocationPercent")
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPlayerStartRank
{
public:
	CvPlayerStartRank()
	{
		m_iPlayerID = NO_PLAYER;
		m_iRank = 0;
	}

	CvPlayerStartRank( const CvPlayerStartRank& startRankObj )
	{
		m_iPlayerID = startRankObj.m_iPlayerID;
		m_iRank = startRankObj.m_iRank;
	}

	bool operator<(const CvPlayerStartRank& startRankObj)
	{
		return (m_iRank < startRankObj.m_iRank);
	}

	int m_iPlayerID;
	int m_iRank;      // Set to "StartingLocationPercent" * 10 + (random number from 0 to 9)
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvStartPositioner
//!  \brief		Divides the map into regions of each fertility and places one major civ in each
//
//!  Key Attributes:
//!  - One instance for the entire game
//!  - Works with CvSiteEvaluatorForStart to compute fertility of each plot
//!  - Also divides minor civs between the regions and places them as well
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvStartPositioner
{
public:
	CvStartPositioner(void);
	~CvStartPositioner(void);

	virtual void Init(CvSiteEvaluatorForStart *pSiteEvaluator);

	void DivideMapIntoRegions(int iNumRegions);
	void ComputeFoundValues();
	void RankPlayerStartOrder();
	void AssignStartingLocations();
	int GetRegion(int iX, int iY);

private:
	void DivideContinentIntoRegions(CvContinent continent);
	void ComputeTileFertilityValues();
	void SubdivideRegion (CvStartRegion region, int numDivisions);
	void ChopIntoTwoRegions (bool bTaller, CvStartRegion *region, CvStartRegion *secondRegion, int iChopPercent);
	void ChopIntoThreeRegions (bool bTaller, CvStartRegion *region, CvStartRegion *secondRegion, CvStartRegion *thirdRegion);
	int ComputeRowFertility (int iAreaID, int xMin, int xMax, int yMin, int yMax);
	bool AddCivToRegion(int iPlayerIndex, CvStartRegion region, bool bRelaxFoodReq);
	bool TooCloseToAnotherCiv (CvPlot *pPlot);
	bool MeetsFoodRequirement (CvPlot *pPlot, TeamTypes eTeam, int iFoodRequirement);
	int StartingPlotRange() const;

	// Logging
	void LogStartPositionMessage(CvString strMsg);

	// References to other objects
	CvSiteEvaluatorForStart *m_pSiteEvaluator;

	// Internal data
	vector<CvContinent> m_ContinentVector;
	vector<CvStartRegion> m_StartRegionVector;
	vector<CvPlayerStartRank> m_PlayerOrder;
	vector<CvPlayerStartRank>::iterator m_PlayerOrderIter;
	int m_iRequiredSeparation;
	int m_iBestFoundValueOnMap;
};

#endif //CIV5_START_POSITIONER_H