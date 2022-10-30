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

#include <vector>

typedef std::vector<int> PlotIndexContainer; //no good place to put this

enum CvAStarNodeAddOp
{
	ASNC_INITIALADD,
	ASNC_OPENADD_UP,
	ASNC_CLOSEDADD_UP,
	ASNC_PARENTADD_UP,
	ASNC_NEWADD
};

class CvUnit;
class CvPlot;

enum PathType
{
	//------- unit pathing
	PT_UNIT_MOVEMENT,			//path for a particular unit (stacking,ZoC,danger handled via flag)
	//------- step path
	PT_GENERIC_SAME_AREA,		//plots must have the same area ID (ie only water or only land)
	PT_GENERIC_SAME_AREA_WIDE,	//path must be 3 tiles wide
	PT_ARMY_LAND,				//step path land only
	PT_ARMY_WATER,				//step path water only (and cities)
	PT_ARMY_MIXED,				//step path land and water. allow water/land transition but not back!
	PT_TRADE_LAND,				//land trade (path or reachable plots if dest -1)
	PT_TRADE_WATER,				//water trade (path or reachable plots if dest -1)
	PT_BUILD_ROUTE,				//prospective route, land only
	PT_BUILD_ROUTE_MIXED,		//prospective route, allow harbors
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
	bool bCanEnterTerritoryIntermediate:1;
	bool bCanEnterTerritoryPermanent:1;
	bool bIsNonEnemyCity:1;
	bool bIsEnemyCity:1;
	bool bIsVisibleEnemyUnit:1;
	bool bIsVisibleEnemyCombatUnit:1;
	bool bIsVisibleNeutralCombatUnit:1;
	bool bUnitStackingLimitReached:1;
	bool bIsValidRoute:1;

	int iMoveFlags;

	//for performance, precompute the effect of plot type, terrain and feature
	unsigned short plotMovementCostMultiplier;
	unsigned short plotMovementCostAdder;

	//tell us when to update the cache ...
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

	short m_iX, m_iY;							// Coordinate position - persistent

	long m_iKnownCost;							// Goal (g)
	long m_iHeuristicCost;						// Heuristic (h)
	long m_iTotalCost;							// Fitness (f)

	unsigned short m_iMoves;					// unit-specific, movement points left. if no unit is given, always zero
	unsigned short m_iTurns;					// unit-specific, how many turns does it take to get here. if no unit given: equal to number of plots in path up to here
	unsigned short m_iStartMovesForTurn;		// needed for move cost normalization on domain change

	bool m_bIsOpen;								// Is this node on the open or closed list?
	CvAStarNode* m_pParent;						// Parent in current path

	CvAStarNode** m_apNeighbors; 				// For faster neighbor lookup (potential children) - always 6 - not affected by clear()
	std::vector<CvAStarNode*> m_apChildren;		// Nodes we could reach from this node - maybe be more than 6 because of "extrachildren"

	CvPathNodeCacheData m_kCostCacheData;		// some things we want to calculate only once
};

//-------------------------------------------------------------------------------------------------
// All information which might be required for a given path
//-------------------------------------------------------------------------------------------------
struct SPathFinderUserData
{
	SPathFinderUserData() : ePathType(PT_GENERIC_SAME_AREA), iFlags(0), ePlayer(NO_PLAYER), iUnitID(0), iTypeParameter(-1), iMaxTurns(INT_MAX), iMaxNormalizedDistance(INT_MAX), iMinMovesLeft(0), iStartMoves(60) {}
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
	PlotIndexContainer plotsToIgnoreForZOC;
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
		x = p != 0 ? p->m_iX : -1;
		y = p != 0 ? p->m_iY : -1;
		turns = p != 0 ? p->m_iTurns : 0;
		moves = p != 0 ? p->m_iMoves : 0;
	}

	bool operator==(const SPathNode& other) const
	{
		return x==other.x && y==other.y && turns==other.turns && moves==other.moves;
	}
};

struct SPath
{
	std::vector<SPathNode> vPlots;
	int iTotalCost;
	int iNormalizedDistanceRaw; //fixed point float!
	int iTotalTurns;
	int iTurnSliceGenerated;
	SPathFinderUserData sConfig;

	//constructor
	SPath() : iTotalCost(-1),iNormalizedDistanceRaw(-1),iTotalTurns(-1),iTurnSliceGenerated(-1) {}

	//not quite a safe-bool, but good enough
	inline bool operator!() const { return vPlots.empty(); }

	//convenience
	inline int length() const { return vPlots.size(); }
	CvPlot* get(int i) const;

	//flip the order of the plots
	void invert();

	static int getNormalizedDistanceBase();

	bool operator==(const SPath& other) const
	{
		return iTotalCost==other.iTotalCost && iNormalizedDistanceRaw==other.iNormalizedDistanceRaw && 
				iTotalTurns==other.iTotalTurns && vPlots.size()==other.vPlots.size();
	}
};

struct SMovePlot
{
	int iPlotIndex;
	int iPathLength; //turns or steps, depending on pathfinder
	int iMovesLeft;
	int iNormalizedDistanceRaw; //fixed point float

	SMovePlot(int iIndex) : iPlotIndex(iIndex), iPathLength(0), iMovesLeft(0), iNormalizedDistanceRaw(0) {}
	SMovePlot(int iIndex, int iLength_, int iMovesLeft_, int iNormalizedDistance_) : 
		iPlotIndex(iIndex), iPathLength(iLength_), iMovesLeft(iMovesLeft_), iNormalizedDistanceRaw(iNormalizedDistance_) {}

	//this ignores the turns/moves so std::find with just a plot index should work
	bool operator==(const SMovePlot& rhs) const { return iPlotIndex==rhs.iPlotIndex; }
	bool operator<(const SMovePlot& rhs) const { return iPlotIndex<rhs.iPlotIndex; }

	//for the step finder normally turns==steps, but sometimes we want effective path length from cost
	int effectivePathLength(int iMovesPerTurn) const;
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
	void insertWithIndex(const SMovePlot& plot);
	void insertNoIndex(const SMovePlot& plot);
	void createIndex();

	bool operator==(const ReachablePlots& rhs) const { return storage == rhs.storage && lookup == rhs.lookup; }
	bool operator!=(const ReachablePlots& rhs) const { return storage != rhs.storage || lookup != rhs.lookup; }

protected:
	vector<pair<int,size_t>> lookup;
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
	}

	int m_iMoves;
	int m_iTurns;
	short m_iX, m_iY;

	CvPathNode(const SPathNode& rhs)
	{
		m_iX = rhs.x;
		m_iY = rhs.y;
		m_iTurns = rhs.turns;
		m_iMoves = rhs.moves;
	}
};

FDataStream & operator >> (FDataStream & kStream, CvPathNode & node);
FDataStream & operator << (FDataStream & kStream, const CvPathNode & node);

class CvPathNodeArray : public std::deque<CvPathNode>
{
public:
	CvPlot* GetTurnDestinationPlot(int iTurn) const;
	CvPlot* GetFinalPlot() const;
	CvPlot* GetFirstPlot() const;
	CvPlot* GetPlotByIndex(int iIndex) const;

	friend FDataStream& operator<<(FDataStream& saveTo, const CvPathNodeArray& readFrom);
	friend FDataStream& operator>>(FDataStream& loadFrom, CvPathNodeArray& writeTo);
};

struct PrNodeIsBetter
{
	//greater than is intended! the lowest cost should be first
	bool operator()(const CvAStarNode* lhs, const CvAStarNode* rhs) const
	{ 
		//apparently there's a rule, when total cost is equal, prefer lower h (heuristic cost)
		return lhs->m_iTotalCost > rhs->m_iTotalCost || (lhs->m_iTotalCost == rhs->m_iTotalCost && lhs->m_iHeuristicCost > rhs->m_iHeuristicCost); 
	}
};

#endif	//CVASTARNODE_H
