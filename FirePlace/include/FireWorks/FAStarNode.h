//------------------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//  FILE:    FAStarNode.h
//
//  AUTHOR:  Casey O'Toole  --  8/21/2002
//
//  PURPOSE: A* Pathfinding - based off of A* Explorer from "AI Game Programming Wisdom"
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2007 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#ifndef		FASTARNODE_H
#define		FASTARNODE_H
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

enum FAStarListType
{
	NO_FASTARLIST = -1,

	FASTARLIST_OPEN,
	FASTARLIST_CLOSED,

	NUM_FASTARLIST_TYPES
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  CLASS:      FAStarNode
//
//  DESC:       Used by FAStar pathfinding class
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class FAStarNode
{
public:
	FAStarNode():
		m_iX(-1),
		m_iY(-1)
	{
		clear();
	}

	void clear()
	{
		m_iTotalCost = 0;
		m_iKnownCost = 0;
		m_iHeuristicCost = 0;
		m_iNumChildren = 0;
		m_iData1 = 0;
		m_iData2 = 0;
		m_fData1 = 0.0f;

		m_bOnStack = false;

		m_eFAStarListType = NO_FASTARLIST;

		m_pParent = NULL;
		m_pNext = NULL;
		m_pPrev = NULL;
		m_pStack = NULL;

		for (int i = 0; i < 8; i++)
		{
			m_apChildren[i] = NULL;
		}
	}

	int m_iX, m_iY;         // Coordinate position
	int m_iTotalCost;		// Fitness (f)
	int m_iKnownCost;		// Goal (g)
	int m_iHeuristicCost;   // Heuristic (h)
	int m_iNumChildren;
	int m_iData1;
	int m_iData2;
	float m_fData1;

	bool m_bOnStack;

	FAStarListType m_eFAStarListType;

	FAStarNode* m_pParent;
	FAStarNode* m_pNext;					// For Open and Closed lists
	FAStarNode* m_pPrev;					// For Open and Closed lists
	FAStarNode* m_pStack;					// For Push/Pop Stack

	FAStarNode* m_apChildren[8];
};


#endif	//FASTARNODE_H
