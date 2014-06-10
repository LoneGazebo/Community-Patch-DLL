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
	bool bPlotVisibleToTeam:1;
	bool bIsMountain:1;
	bool bIsWater:1;
	bool bCanEnterTerrain:1;
	bool bIsRevealedToTeam:1;
	bool bContainsOtherFriendlyTeamCity:1;
	bool bContainsEnemyCity:1;
	bool bContainsVisibleEnemy:1;
	bool bContainsVisibleEnemyDefender:1;
	int	iNumFriendlyUnitsOfType;
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
	CvAStarNode()
	{
		m_iX = -1;
		m_iY = -1;
		m_iTotalCost = 0;
		m_iKnownCost = 0;
		m_iHeuristicCost = 0;
		m_iNumChildren = 0;
		m_iData1 = 0;
		m_iData2 = 0;

		m_bOnStack = false;

		m_eCvAStarListType = NO_CVASTARLIST;

		m_pParent = NULL;
		m_pNext = NULL;
		m_pPrev = NULL;
		m_pStack = NULL;
	}

	void clear()
	{
		m_iTotalCost = 0;
		m_iKnownCost = 0;
		m_iHeuristicCost = 0;
		m_iNumChildren = 0;
		m_iData1 = 0;
		m_iData2 = 0;

		m_bOnStack = false;

		m_eCvAStarListType = NO_CVASTARLIST;

		m_pParent = NULL;
		m_pNext = NULL;
		m_pPrev = NULL;
		m_pStack = NULL;

		m_apChildren.clear();
	}

	int m_iTotalCost;	  // Fitness (f)
	int m_iKnownCost;	  // Goal (g)
	int m_iHeuristicCost; // Heuristic (h)
	int m_iData1;
	int m_iData2;

	CvAStarListType m_eCvAStarListType;

	CvAStarNode* m_pParent;
	CvAStarNode* m_pNext;					// For Open and Closed lists
	CvAStarNode* m_pPrev;					// For Open and Closed lists
	CvAStarNode* m_pStack;					// For Push/Pop Stack

	FStaticVector<CvAStarNode*, 6, false, c_eCiv5GameplayDLL, 0> m_apChildren;

	short m_iX, m_iY;         // Coordinate position
	short m_iNumChildren;
	bool m_bOnStack;

	CvPathNodeCacheData m_kCostCacheData;
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
		m_iData1 = 0;
		m_iData2 = 0;
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

	int m_iData1;
	int m_iData2;
	int m_iFlags;
	short m_iX, m_iY;         // Coordinate position

	bool GetFlag(int eFlag) const { return (m_iFlags & eFlag) != 0; }
	void SetFlag(int eFlag) { m_iFlags |= eFlag; }
	void ClearFlag(int eFlag) { m_iFlags = (m_iFlags & ~eFlag); }

	CvPathNode& operator =(const CvAStarNode& rhs)
	{
		m_iX = rhs.m_iX;
		m_iY = rhs.m_iY;
		m_iData1 = rhs.m_iData1;
		m_iData2 = rhs.m_iData2;
		m_iFlags = 0;
		return *this;
	}
};

class CvPathNodeArray : public FFastVector< CvPathNode, true, c_eMPoolTypeContainer >
{
public:

	const CvPathNode* GetTurnDest(int iTurn);
};

#endif	//CVASTARNODE_H
