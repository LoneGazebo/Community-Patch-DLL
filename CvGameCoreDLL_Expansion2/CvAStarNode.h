/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

//
//  AUTHOR:  Casey O'Toole  --  8/21/2002
//           Brian Wade     --            hexified
//           Ed Beach       --  4/16/2009 moved into CvGameCoreDLL
//
//  PURPOSE: A* Pathfinding - based off of A* Explorer from "AI Game Programming Wisdom"
//

#ifndef		CVASTARNODE_H
#define		CVASTARNODE_H
#pragma		once

#include <unordered_map>

enum CvAStarNodeAddOp
{
	ASNC_INITIALADD,
	ASNC_OPENADD_UP,
	ASNC_CLOSEDADD_UP,
	ASNC_PARENTADD_UP,
	ASNC_NEWADD
};

enum CvAStarListType
{
	NO_CVASTARLIST = -1,

	CVASTARLIST_OPEN,
	CVASTARLIST_CLOSED,

	NUM_CVASTARLIST_TYPES
};

class CvUnit;
class CvPlot;

enum PathType
{
	PT_UNIT_MOVEMENT,			//path for a particular unit (stacking,ZoC,danger handled via flag)
	PT_UNIT_REACHABLE_PLOTS,	//all plots a unit can reach in N turns
	PT_GENERIC_REACHABLE_PLOTS, //all plots that can be reached in N turns without knowning the particular unit
	PT_GENERIC_SAME_AREA,		//plots must have the same area ID (ie only water or only land)
	PT_GENERIC_ANY_AREA,		//plots can have any area ID, simply need to be passable
	PT_GENERIC_SAME_AREA_WIDE,	//path must be 3 tiles wide (for armies)
	PT_GENERIC_ANY_AREA_WIDE,	//same for any area
	PT_TRADE_WATER,				//water trade (path or reachable plots if dest -1)
	PT_TRADE_LAND,				//land trade (path or reachable plots if dest -1)
	PT_BUILD_ROUTE,				//prospective route
	PT_AREA_CONNECTION,			//assign area IDs to connected plots (hack)
	PT_LANDMASS_CONNECTION,		//assign landmass IDs to connected plots (hack)
	PT_CITY_INFLUENCE,			//which plot is next for a city to expand it's borders
	PT_CITY_CONNECTION_LAND,	//is there a road or railroad between two points
	PT_CITY_CONNECTION_WATER,	//is there a sea connection between two points
	PT_CITY_CONNECTION_MIXED,	//is there a mixed land/sea connection between two points
	PT_AIR_REBASE,				//for aircraft, only plots with cities and carriers are allowed
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  STRUCT:     CvPathNodeCacheData
//
//  DESC:       Cached data for the node that the pathfinder cost functions can use.
//				This is somewhat specific to the cost function.  It greatly improves the speed of
//				the cost functions by not having to re-calculate/fetch the same data over and over again
//				which can total many millions of accesses over a full AI turn
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
struct CvPathNodeCacheData
{
	bool bIsRevealedToTeam:1;
	bool bPlotVisibleToTeam:1;
	bool bIsNonNativeDomain:1;
	bool bCanEnterTerrainIntermediate:1;
	bool bCanEnterTerrainPermanent:1;
	bool bCanEnterTerritory:1;
	bool bContainsOtherFriendlyTeamCity:1;
	bool bContainsEnemyCity:1;
	bool bContainsVisibleEnemy:1;
	bool bContainsVisibleEnemyDefender:1;
	bool bFriendlyUnitLimitReached:1;
	bool bIsValidRoute:1;

	int iMoveFlags;

	//tells when to update the cache ...
	unsigned short iGenerationID;

	//housekeeping
	CvPathNodeCacheData() { clear(); }
	void clear() { memset(this,0,sizeof(CvPathNodeCacheData)); }
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  CLASS:      CvAStarNode
//
//  DESC:       Used by CvAStar pathfinding class
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAStarNode
{
public:
	CvAStarNode();
	void clear();

	short m_iX, m_iY;	  // Coordinate position - persistent

	int m_iTotalCost;	  // Fitness (f)
	int m_iKnownCost;	  // Goal (g)
	int m_iHeuristicCost; // Heuristic (h)

	int m_iMoves;		//unit-specific, movement points left. if no unit is given, always zero
	int m_iTurns;		//unit-specific, how many turns does it take to get here. if no unit given: equal to number of plots in path up to here

	CvAStarNode* m_pParent;					// Parent in current path
	CvAStarListType m_eCvAStarListType;

	CvAStarNode* m_pStack;					// For Push/Pop Stack

	//nodes we could reach from this node - maybe be more than 6 because of "extrachildren"
	std::vector<CvAStarNode*> m_apChildren;

	//for faster neighbor lookup (potential children) - always 6 - persistent
	CvAStarNode** m_apNeighbors;

	CvPathNodeCacheData m_kCostCacheData;	// some things we want to calculate only once
};

//-------------------------------------------------------------------------------------------------
// All information which might be required for a given path
//-------------------------------------------------------------------------------------------------
struct SPathFinderUserData
{
	SPathFinderUserData() : ePathType(PT_GENERIC_ANY_AREA), iFlags(0), ePlayer(NO_PLAYER), iUnitID(0), iTypeParameter(-1), iMaxTurns(INT_MAX), iMaxNormalizedDistance(INT_MAX), iMinMovesLeft(0), iStartMoves(60) {}
	SPathFinderUserData(const CvUnit* pUnit, int iFlags=0, int iMaxTurns=INT_MAX);
	SPathFinderUserData(PlayerTypes ePlayer, PathType ePathType, int iTypeParameter=-1, int iMaxTurns=INT_MAX);

	//do not compare max turns and max cost ...
	bool operator==(const SPathFinderUserData& rhs) const 
		{ return ePathType==rhs.ePathType && iFlags==rhs.iFlags && ePlayer==rhs.ePlayer && iUnitID==rhs.iUnitID && iTypeParameter==rhs.iTypeParameter; }
	bool operator!=(const SPathFinderUserData& rhs) const { return !(*this==rhs); }

	PathType	ePathType;
	int			iTypeParameter;		//route type dependent parameter
	int			iFlags;				//see CvUnit::MOVEFLAG*
	PlayerTypes ePlayer;			//optional
	int			iUnitID;			//optional
	int			iMaxTurns;
	int			iMaxNormalizedDistance;
	int			iMinMovesLeft;
	int			iStartMoves;
	set<int>	plotsToIgnoreForZOC;
};

//-------------------------------------------------------------------------------------------------
// Simple structure to hold a pathfinding result
//-------------------------------------------------------------------------------------------------
struct SPathNode
{
	short x,y,turns,moves;

	//constructor
	SPathNode() : x(-1),y(-1),turns(0),moves(0) {}
	SPathNode(CvAStarNode* p)
	{
		x = p ? p->m_iX : -1;
		y = p ? p->m_iY : -1;
		turns = p ? p->m_iTurns : 0;
		moves = p ? p->m_iMoves : 0;
	}

	bool operator==(const SPathNode& other)
	{
		return x==other.x && y==other.y && turns==other.turns && moves==other.moves;
	}
};

struct SPath
{
	std::vector<SPathNode> vPlots;
	int iTotalCost;
	int iNormalizedDistance;
	int iTotalTurns;
	int iTurnGenerated;
	SPathFinderUserData sConfig;

	//constructor
	SPath() : iTotalCost(-1),iNormalizedDistance(-1),iTotalTurns(-1),iTurnGenerated(-1) {}

	//not quite a safe-bool, but good enough
	inline bool operator!() const { return vPlots.empty(); }

	//convenience
	inline int length() const { return vPlots.size(); }
	inline CvPlot* get(int i) const;

	bool operator==(const SPath& other)
	{
		return iTotalCost==other.iTotalCost && iNormalizedDistance==other.iNormalizedDistance && 
				iTotalTurns==other.iTotalTurns && vPlots.size()==other.vPlots.size();
	}
};

struct SMovePlot
{
	int iPlotIndex;
	int iTurns;
	int iMovesLeft;
	int iNormalizedDistance;

	SMovePlot(int iIndex) : iPlotIndex(iIndex), iTurns(0), iMovesLeft(0), iNormalizedDistance(0) {}
	SMovePlot(int iIndex, int iTurns_, int iMovesLeft_, int iNormalizedDistance_) : 
		iPlotIndex(iIndex), iTurns(iTurns_), iMovesLeft(iMovesLeft_), iNormalizedDistance(iNormalizedDistance_) {}

	//this ignores the turns/moves so std::find with just a plot index should work
	bool operator==(const SMovePlot& rhs) const { return iPlotIndex==rhs.iPlotIndex; }
	bool operator<(const SMovePlot& rhs) const { return iPlotIndex<rhs.iPlotIndex; }
};

class ReachablePlots
{
public:
	typedef std::vector<SMovePlot>::iterator iterator;
	typedef std::vector<SMovePlot>::const_iterator const_iterator;
	
	ReachablePlots() {}
	
	iterator begin() { return storage.begin(); }
	const_iterator begin() const { return storage.begin(); }
	iterator end() { return storage.end(); }
	const_iterator end() const { return storage.end(); }

	size_t size() const { return storage.size(); }
	bool empty() const { return storage.empty(); }
	void clear() { storage.clear(); lookup.clear(); }

	iterator find(int iPlotIndex);
	const_iterator find(int iPlotIndex) const;
	void insert(const SMovePlot& plot);

protected:
	std::tr1::unordered_map<int,size_t> lookup;
	std::vector<SMovePlot> storage;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  CLASS:      CvPathNode
//
//  DESC:       A node in a generated path.  This is a sub-set of the full CvAStarNode.
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPathNode
{
public:
	CvPathNode()
	{
		clear();
	}

	void clear()
	{
		m_iX = -1;
		m_iY = -1;
		m_iMoves = 0;
		m_iTurns = 0;
		m_iFlags = 0;
	}

	enum Flags
	{
		// If set the plot that the path node points to was not visible to the team that generated the path at the time of creation.  
		// For efficiency, all further nodes should be assumed to have been possibly invisible as well.
		PLOT_INVISIBLE						= (1 << 0),
		// A convenience flag, signifying that the next plot in the path was invisible at the time of generation.  It is at this plot you most likely need to regenerated the path
		// if the unit is going to continue into the 'invisible' plots, this is because if you wait until you enter an invisible plot, you may need to stop because it reveals a blocking
		// plot and the unit may be stop on a plot it is not allowed to stop on.
		PLOT_ADJACENT_INVISIBLE				= (1 << 1)
	};

	int m_iMoves;
	int m_iTurns;
	int m_iFlags;
	short m_iX, m_iY;         // Coordinate position

	bool GetFlag(int eFlag) const { return (m_iFlags & eFlag) != 0; }
	void SetFlag(int eFlag) { m_iFlags |= eFlag; }
	void ClearFlag(int eFlag) { m_iFlags = (m_iFlags & ~eFlag); }

	CvPathNode& operator =(const SPathNode& rhs)
	{
		m_iX = rhs.x;
		m_iY = rhs.y;
		m_iTurns = rhs.turns;
		m_iMoves = rhs.moves;
		m_iFlags = 0;
		return *this;
	}
};

class CvPathNodeArray : public std::deque<CvPathNode>
{
public:
	CvPlot* GetTurnDestinationPlot(int iTurn) const;
	CvPlot* GetFinalPlot() const;
	CvPlot* GetFirstPlot() const;
};

struct PrNodeIsBetter
{
	//greater than is intended! the lowest cost should be first
	bool operator()(const CvAStarNode* lhs, const CvAStarNode* rhs) const { return lhs->m_iTotalCost > rhs->m_iTotalCost; }
};

#endif	//CVASTARNODE_H
