/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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

#define ASNL_ADDOPEN		0
#define ASNL_STARTOPEN		1
#define ASNL_DELETEOPEN		2
#define ASNL_ADDCLOSED		3

#define ASNC_INITIALADD		0
#define ASNC_OPENADD_UP		1
#define ASNC_CLOSEDADD_UP	2
#define ASNC_PARENTADD_UP	3
#define ASNC_NEWADD			4

enum CvAStarListType
{
	NO_CVASTARLIST = -1,

	CVASTARLIST_OPEN,
	CVASTARLIST_CLOSED,

	NUM_CVASTARLIST_TYPES
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
	bool bIsWater:1;
	bool bCanEnterTerrain:1;
	bool bCanEnterTerritory:1;
	bool bContainsOtherFriendlyTeamCity:1;
	bool bContainsEnemyCity:1;
	bool bContainsVisibleEnemy:1;
	bool bContainsVisibleEnemyDefender:1;
	bool bFriendlyUnitLimitReached:1;
	bool bIsValidRoute:1;

	int iPlotDanger; 

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

	CvPathNode& operator =(const CvAStarNode& rhs)
	{
		m_iX = rhs.m_iX;
		m_iY = rhs.m_iY;
		m_iMoves = rhs.m_iMoves;
		m_iTurns = rhs.m_iTurns;
		m_iFlags = 0;
		return *this;
	}
};

class CvPathNodeArray : public FFastVector< CvPathNode, true, c_eMPoolTypeContainer >
{
public:

	const CvPathNode* GetTurnDest(int iTurn);
};

struct PrNodeIsBetter
{
	//greater than is intended! the lowest cost should be first
	bool operator()(const CvAStarNode* lhs, const CvAStarNode* rhs) const { return lhs->m_iTotalCost > rhs->m_iTotalCost; }
};

#endif	//CVASTARNODE_H
