/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

//
//  FILE:    CvAStar.cpp
//
//  AUTHOR:  Casey O'Toole  --  8/27/2002
//  MOD:     Brian Wade     --  5/20/2008
//  MOD:     Ed Beach       --  4/16/2009 moved into CvGameCoreDLL
//

#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreUtils.h"
#include "CvAStar.h"
#include "ICvDLLUserInterface.h"
#include "CvMinorCivAI.h"
#include "CvDllInterfaces.h"
#include "cvStopWatch.h"
#include "CvUnitMovement.h"

#define PATH_MOVEMENT_WEIGHT									(1000)
#define PATH_RIVER_WEIGHT										(100)
#define PATH_DEFENSE_WEIGHT										(10)
#define PATH_TERRITORY_WEIGHT									(3)
#define PATH_STEP_WEIGHT										(1)
#define PATH_THROUGH_WATER										(10000)
#define	PATH_EXPLORE_NON_HILL_WEIGHT							(300)
#define PATH_EXPLORE_NON_REVEAL_WEIGHT							(10)
#define PATH_BUILD_ROUTE_EXISTING_ROUTE_WEIGHT					(10)
#define PATH_BUILD_ROUTE_ALREADY_FLAGGED_DISCOUNT				(0.5f)
#define PATH_END_TURN_MORTAL_DANGER_WEIGHT						(1000000)
#define PATH_END_TURN_MISSIONARY_OTHER_TERRITORY				(150000)
#define PATH_DO_NOT_USE_WEIGHT									(1000000000)

#if defined(MOD_BALANCE_CORE_MILITARY)
#define PATH_ENEMY_TERRITORY_WEIGHT								(100)
#endif

#include <xmmintrin.h>
#include "LintFree.h"

#define PREFETCH_FASTAR_NODE(x) _mm_prefetch((const char*)x,  _MM_HINT_T0 ); _mm_prefetch(((const char*)x)+64,  _MM_HINT_T0 );
#define PREFETCH_FASTAR_CVPLOT(x) _mm_prefetch((const char*)x,  _MM_HINT_T0 ); _mm_prefetch(((const char*)x)+64,  _MM_HINT_T0 );

//	--------------------------------------------------------------------------------
/// Constructor
CvAStar::CvAStar()
{
	udIsPathDest = NULL;
	udDestValid = NULL;
	udHeuristic = NULL;
	udCost = NULL;
	udValid = NULL;
	udNotifyChild = NULL;
	udNotifyList = NULL;
	udNumExtraChildrenFunc = NULL;
	udGetExtraChildFunc = NULL;
	udInitializeFunc = NULL;
	udUninitializeFunc = NULL;

	m_pData = NULL;

	m_pOpen = NULL;
	m_pOpenTail = NULL;
	m_pClosed = NULL;
	m_pBest = NULL;
	m_pStackHead = NULL;

	m_ppaaNodes = NULL;
	m_ppaaNeighbors = NULL;

#ifdef AUI_ASTAR_FIX_NO_DUPLICATE_CALLS
	m_iCurrentGenerationID = 0;
#endif

	m_bIsMPCacheSafe = false;

	m_iMaxTurns = INT_MAX;
	m_iMaxCost = INT_MAX;

#if defined(MOD_BALANCE_CORE)
	//for debugging
	m_strName = "AStar";
#endif
}

//	--------------------------------------------------------------------------------
/// Destructor
CvAStar::~CvAStar()
{
	DeInit();
}

//	--------------------------------------------------------------------------------
/// Frees allocated memory
void CvAStar::DeInit()
{
	if(m_ppaaNodes != NULL)
	{
		for(int iI = 0; iI < m_iColumns; iI++)
		{
			FFREEALIGNED(m_ppaaNodes[iI]);
		}

		FFREEALIGNED(m_ppaaNodes);
		m_ppaaNodes=0;
	}

	if (m_ppaaNeighbors)
	{
		delete [] m_ppaaNeighbors;
		m_ppaaNeighbors = NULL;
	}
}

//	--------------------------------------------------------------------------------
/// Initializes the AStar algorithm
void CvAStar::Initialize(int iColumns, int iRows, bool bWrapX, bool bWrapY, CvAPointFunc IsPathDestFunc, CvAPointFunc DestValidFunc, CvAHeuristic HeuristicFunc, CvAStarConst1Func CostFunc, CvAStarConst2Func ValidFunc, CvAStarFunc NotifyChildFunc, CvAStarFunc NotifyListFunc, CvANumExtraChildren NumExtraChildrenFunc, CvAGetExtraChild GetExtraChildFunc, CvABegin InitializeFunc, CvAEnd UninitializeFunc, const void* pData)
{
	int iI, iJ;

	DeInit();	// free old memory just in case

	udIsPathDest = IsPathDestFunc;
	udDestValid = DestValidFunc;
	udHeuristic = HeuristicFunc;
	udCost = CostFunc;
	udValid = ValidFunc;
	udNotifyChild = NotifyChildFunc;
	udNotifyList = NotifyListFunc;
	udNumExtraChildrenFunc = NumExtraChildrenFunc;
	udGetExtraChildFunc = GetExtraChildFunc;
	udInitializeFunc = InitializeFunc;
	udUninitializeFunc = UninitializeFunc;

	m_pData = pData;

	m_iColumns = iColumns;
	m_iRows = iRows;

	m_iXstart = -1;
	m_iYstart = -1;
	m_iXdest = -1;
	m_iYdest = -1;
	m_iInfo = 0;

	m_bWrapX = bWrapX;
	m_bWrapY = bWrapY;
	m_bForceReset = false;

	m_pOpen = NULL;
	m_pOpenTail = NULL;
	m_pClosed = NULL;
	m_pBest = NULL;
	m_pStackHead = NULL;

	m_ppaaNodes = reinterpret_cast<CvAStarNode**>(FMALLOCALIGNED(sizeof(CvAStarNode*)*m_iColumns, 64, c_eCiv5GameplayDLL, 0));
	for(iI = 0; iI < m_iColumns; iI++)
	{
		m_ppaaNodes[iI] = reinterpret_cast<CvAStarNode*>(FMALLOCALIGNED(sizeof(CvAStarNode)*m_iRows, 64, c_eCiv5GameplayDLL, 0));
		for(iJ = 0; iJ < m_iRows; iJ++)
		{
			new(&m_ppaaNodes[iI][iJ]) CvAStarNode();
			m_ppaaNodes[iI][iJ].m_iX = iI;
			m_ppaaNodes[iI][iJ].m_iY = iJ;
		}
	}
#if defined(MOD_BALANCE_CORE)
	m_ppaaNeighbors = new CvAStarNode*[m_iColumns*m_iRows*6];
	CvAStarNode** apNeighbors = m_ppaaNeighbors;

	for(iI = 0; iI < m_iColumns; iI++)
		for(iJ = 0; iJ < m_iRows; iJ++)
		{
			m_ppaaNodes[iI][iJ].m_apNeighbors = apNeighbors;
			apNeighbors += 6;
			PrecalcNeighbors( &(m_ppaaNodes[iI][iJ]) );
		}
#endif
}

//	--------------------------------------------------------------------------------
/// Generates a path from iXstart,iYstart to iXdest,iYdest
bool CvAStar::GeneratePath(int iXstart, int iYstart, int iXdest, int iYdest, int iInfo, bool bReuse)
{
	CvAStarNode* temp;
	int retval;

	const CvGame& game = GC.getGame();
	bool isMultiplayer = game.isNetworkMultiPlayer();
	bool discardCacheForMPGame = isMultiplayer && !m_bIsMPCacheSafe;

	if(m_bForceReset || (m_iXstart != iXstart) || (m_iYstart != iYstart) || (m_iInfo != iInfo) || discardCacheForMPGame)
		bReuse = false;

#ifdef AUI_ASTAR_FIX_NO_DUPLICATE_CALLS
	//this is the version number for the node cache
	m_iCurrentGenerationID++;
	if (m_iCurrentGenerationID==0xFFFF)
		m_iCurrentGenerationID = 1;
#endif

	m_iXdest = iXdest;
	m_iYdest = iYdest;
	m_iXstart = iXstart;
	m_iYstart = iYstart;
	m_iInfo = iInfo;

	if (udInitializeFunc)
		udInitializeFunc(m_pData, this);

	if(!isValid(iXstart, iYstart))
	{
		if (udUninitializeFunc)
			udUninitializeFunc(m_pData, this);
		return false;
	}

	PREFETCH_FASTAR_NODE(&(m_ppaaNodes[iXdest][iYdest]));

	if(!bReuse)
	{
		// XXX should we just be doing a memset here?
		if(m_pOpen)
		{
			while(m_pOpen)
			{
				temp = m_pOpen->m_pNext;
				m_pOpen->clear();
				m_pOpen = temp;
			}
		}

		if(m_pClosed)
		{
			while(m_pClosed)
			{
				temp = m_pClosed->m_pNext;
				m_pClosed->clear();
				m_pClosed = temp;
			}
		}

		PREFETCH_FASTAR_NODE(&(m_ppaaNodes[iXstart][iYstart]));

		m_pBest = NULL;
		m_pStackHead = NULL;

		m_bForceReset = false;

		temp = &(m_ppaaNodes[iXstart][iYstart]);

		temp->m_iKnownCost = 0;
		if(udHeuristic == NULL)
		{
			temp->m_iHeuristicCost = 0;
		}
		else
		{
			temp->m_iHeuristicCost = udHeuristic(m_iXstart, m_iYstart, m_iXdest, m_iYdest);
		}
		temp->m_iTotalCost = temp->m_iKnownCost + temp->m_iHeuristicCost;

		m_pOpen = temp;
		m_pOpenTail = temp;

		udFunc(udNotifyList, NULL, m_pOpen, ASNL_STARTOPEN, m_pData);
		udFunc(udValid, NULL, temp, 0, m_pData);
		udFunc(udNotifyChild, NULL, temp, ASNC_INITIALADD, m_pData);
	}

	if(udDestValid != NULL)
	{
		if(!udDestValid(iXdest, iYdest, m_pData, this))
		{
			if (udUninitializeFunc)
				udUninitializeFunc(m_pData, this);
			return false;
		}
	}

	if(isValid(m_iXdest, m_iYdest))
	{
		temp = &(m_ppaaNodes[m_iXdest][m_iYdest]);

		if(temp->m_eCvAStarListType == CVASTARLIST_CLOSED)
		{
			m_pBest = temp;
			if (udUninitializeFunc)
				udUninitializeFunc(m_pData, this);
			return true;
		}
	}

	retval = 0;

	while(retval == 0)
	{
		retval = Step();
	}

	if(retval == -1)
	{
		assert(m_pBest == NULL);
		if (udUninitializeFunc)
			udUninitializeFunc(m_pData, this);
		return false;
	}

#if defined(MOD_BALANCE_CORE_DEBUGGING)
	//debugging!
	if (false)
	{
		CvString fname = CvString::format( "PathfindingTurn%03d.txt", GC.getGame().getGameTurn() );
		FILogFile* pLog=LOGFILEMGR.GetLog( fname.c_str(), FILogFile::kDontTimeStamp );
		if (pLog && strcmp(GetName(),"generic pf")==0) //casting magic not safe for all types of pathfinder
		{
			CvUnit* pUnit = (CvUnit*)m_pData; 

			pLog->Msg( CvString::format("%s for %s (%d) from %d,%d to %d,%d for player %d\n", 
				GetName(),pUnit->getName().c_str(),pUnit->GetID(),m_iXstart,m_iYstart,m_iXdest,m_iYdest,GetCurrentPlayer() ).c_str() );

			gStackWalker.SetLog(pLog);
			gStackWalker.ShowCallstack();

			CvAStarNode* pCurrent = m_pBest;
			while (pCurrent!=NULL)
			{
				pLog->Msg( CvString::format("%d,%d cost %d,%d\n", pCurrent->m_iX,pCurrent->m_iY,pCurrent->m_iKnownCost,pCurrent->m_iHeuristicCost ).c_str() );
				pCurrent = pCurrent->m_pParent;
			}
		}
	}
#endif

	if (udUninitializeFunc)
		udUninitializeFunc(m_pData, this);
	return true;
}

//	--------------------------------------------------------------------------------
/// Takes one step in the algorithm
int CvAStar::Step()
{
	if((m_pBest = GetBest()) == NULL)
	{
		return -1;
	}

	CreateChildren(m_pBest); // needs to be done, even on the last node, to allow for re-use...

	if (m_pBest == NULL)	// There seems to be a case were this will come back NULL.
		return -1;

	if(IsPathDest(m_pBest->m_iX, m_pBest->m_iY))
	{
		return 1;
	}

	return 0;
}

//	--------------------------------------------------------------------------------
/// Returns best node
CvAStarNode* CvAStar::GetBest()
{
	CvAStarNode* temp;

	if(!m_pOpen)
	{
		return NULL;
	}

	temp = m_pOpen;

	m_pOpen = temp->m_pNext;
	if(m_pOpen != NULL)
	{
		m_pOpen->m_pPrev = NULL;
	}
	else
	{
		m_pOpenTail = NULL;
	}

	udFunc(udNotifyList, NULL, temp, ASNL_DELETEOPEN, m_pData);

	temp->m_eCvAStarListType = CVASTARLIST_CLOSED;

	temp->m_pNext = m_pClosed;
	if(m_pClosed != NULL)
	{
		m_pClosed->m_pPrev = temp;
	}
	m_pClosed = temp;

	udFunc(udNotifyList, NULL, m_pClosed, ASNL_ADDCLOSED, m_pData);

	return temp;
}
#if defined(MOD_BALANCE_CORE)
// --------------------
/// precompute neighbors for a node
void CvAStar::PrecalcNeighbors(CvAStarNode* node)
{
	int range = 6;
	int x, y;

	static int s_CvAStarChildHexX[6] = { 0, 1,  1,  0, -1, -1, };
	static int s_CvAStarChildHexY[6] = { 1, 0, -1, -1,  0,  1, };

	for(int i = 0; i < range; i++)
	{
		x = node->m_iX - ((node->m_iY >= 0) ? (node->m_iY>>1) : ((node->m_iY - 1)/2));
		x += s_CvAStarChildHexX[i];
		y = yRange(node->m_iY + s_CvAStarChildHexY[i]);
		x += ((y >= 0) ? (y>>1) : ((y - 1)/2));
		x = xRange(x);
		y = yRange(y);

		if(isValid(x, y))
			node->m_apNeighbors[i] = &(m_ppaaNodes[x][y]);
		else
			node->m_apNeighbors[i] = NULL;
	}
}

//	--------------------------------------------------------------------------------
/// Creates children for the node
void CvAStar::CreateChildren(CvAStarNode* node)
{
	CvAStarNode* check;
	int count = 6;
	int x, y;
	int i;

	for(i = 0; i < count; i++)
	{
		check = node->m_apNeighbors[i];

#if defined(MOD_BALANCE_CORE)
		//don't walk backwards!
		if(check && check!=node->m_pParent && udFunc(udValid, node, check, 0, m_pData))
#else
		if(check && udFunc(udValid, node, check, 0, m_pData))
#endif
		{
			LinkChild(node, check);
		}
	}

	if(udNumExtraChildrenFunc && udGetExtraChildFunc)
	{
		int iExtraChildren = udNumExtraChildrenFunc(node, this);
		for(int i = 0; i < iExtraChildren; i++)
		{
			udGetExtraChildFunc(node, i, x, y, this);
			PREFETCH_FASTAR_NODE(&(m_ppaaNodes[x][y]));

			if(isValid(x, y))
			{
				check = &(m_ppaaNodes[x][y]);

				if(udFunc(udValid, node, check, 0, m_pData))
				{
					LinkChild(node, check);
				}
			}
		}
	}
}
#else
//	--------------------------------------------------------------------------------
/// Creates children for the node
void CvAStar::CreateChildren(CvAStarNode* node)
{
	CvAStarNode* check;
	int range = 6;
	int x, y;
	int i;

	static int s_CvAStarChildHexX[6] = { 0, 1,  1,  0, -1, -1, };
	static int s_CvAStarChildHexY[6] = { 1, 0, -1, -1,  0,  1, };

	for(i = 0; i < range; i++)
	{
		x = node->m_iX - ((node->m_iY >= 0) ? (node->m_iY>>1) : ((node->m_iY - 1)/2));
		x += s_CvAStarChildHexX[i];
		y = yRange(node->m_iY + s_CvAStarChildHexY[i]);
		x += ((y >= 0) ? (y>>1) : ((y - 1)/2));
		x = xRange(x);

		PREFETCH_FASTAR_NODE(&(m_ppaaNodes[x][y]));
		if(isValid(x, y))
		{
			check = &(m_ppaaNodes[x][y]);

			if(udFunc(udValid, node, check, 0, m_pData))
			{
				LinkChild(node, check);
			}
		}
	}

	if(udNumExtraChildrenFunc && udGetExtraChildFunc)
	{
		int iExtraChildren = udNumExtraChildrenFunc(node, this);
		for(int i = 0; i < iExtraChildren; i++)
		{
			udGetExtraChildFunc(node, i, x, y, this);
			PREFETCH_FASTAR_NODE(&(m_ppaaNodes[x][y]));

			if(isValid(x, y))
			{
				check = &(m_ppaaNodes[x][y]);

				if(udFunc(udValid, node, check, 0, m_pData))
				{
					LinkChild(node, check);
				}
			}
		}
	}
}
#endif
//	--------------------------------------------------------------------------------
/// Link in a child
void CvAStar::LinkChild(CvAStarNode* node, CvAStarNode* check)
{
	// we would have to start a new turn to continue
	if(node->m_iMoves == 0)
		if (node->m_iTurns+1 > GetMaxTurns()) // path is getting too long ...
			return;

	int iKnownCost = udFunc(udCost, node, check, 0, m_pData);
	if (iKnownCost == PATH_DO_NOT_USE_WEIGHT)
		return; //don't even link it up, it's a dead end

	iKnownCost += node->m_iKnownCost;

	if (iKnownCost > GetMaxCost())
		return; // path is getting too expensive

	if(check->m_eCvAStarListType == CVASTARLIST_OPEN)
	{
		node->m_apChildren.push_back(check);
		node->m_iNumChildren++;

		if(iKnownCost < check->m_iKnownCost)
		{
			FAssert(node->m_pParent != check);

			check->m_pParent = node;
			check->m_iKnownCost = iKnownCost;
			check->m_iTotalCost = iKnownCost + check->m_iHeuristicCost;

			UpdateOpenNode(check);
			udFunc(udNotifyChild, node, check, ASNC_OPENADD_UP, m_pData);
		}
	}
	else if(check->m_eCvAStarListType == CVASTARLIST_CLOSED)
	{
		node->m_apChildren.push_back(check);
		node->m_iNumChildren++;

		if(iKnownCost < check->m_iKnownCost)
		{
			FAssert(node->m_pParent != check);
			check->m_pParent = node;
			check->m_iKnownCost = iKnownCost;
			check->m_iTotalCost = iKnownCost + check->m_iHeuristicCost;
			udFunc(udNotifyChild, node, check, ASNC_CLOSEDADD_UP, m_pData);

			UpdateParents(check);
		}
	}
	else
	{
		FAssert(check->m_eCvAStarListType == NO_CVASTARLIST);
		FAssert(node->m_pParent != check);
		check->m_pParent = node;
		check->m_iKnownCost = iKnownCost;
		if(udHeuristic == NULL)
		{
			check->m_iHeuristicCost = 0;
		}
		else
		{
			check->m_iHeuristicCost = udHeuristic(check->m_iX, check->m_iY, m_iXdest, m_iYdest);
		}
		check->m_iTotalCost = check->m_iKnownCost + check->m_iHeuristicCost;

		udFunc(udNotifyChild, node, check, ASNC_NEWADD, m_pData);

		AddToOpen(check);

		node->m_apChildren.push_back(check);
		node->m_iNumChildren++;
	}
}

//	--------------------------------------------------------------------------------
/// Add node to open list
void CvAStar::AddToOpen(CvAStarNode* addnode)
{
	CvAStarNode* node;

	addnode->m_eCvAStarListType = CVASTARLIST_OPEN;

	if(!m_pOpen)
	{
		m_pOpen = addnode;
		m_pOpenTail = addnode;
		m_pOpen->m_pNext = NULL;
		m_pOpen->m_pPrev = NULL;

		udFunc(udNotifyList, NULL, addnode, ASNL_STARTOPEN, m_pData);

		return;
	}

	if(addnode->m_iTotalCost <= m_pOpen->m_iTotalCost)
	{
		addnode->m_pNext = m_pOpen;
		m_pOpen->m_pPrev = addnode;
		m_pOpen = addnode;

		udFunc(udNotifyList, m_pOpen->m_pNext, m_pOpen, ASNL_STARTOPEN, m_pData);
	}
	else if(addnode->m_iTotalCost >= m_pOpenTail->m_iTotalCost)
	{
		addnode->m_pPrev = m_pOpenTail;
		m_pOpenTail->m_pNext = addnode;
		m_pOpenTail = addnode;

		udFunc(udNotifyList, addnode->m_pPrev, addnode, ASNL_ADDOPEN, m_pData);
	}
	else if(abs(addnode->m_iTotalCost-m_pOpenTail->m_iTotalCost) < abs(addnode->m_iTotalCost-m_pOpen->m_iTotalCost))  //(addnode->m_iTotalCost > m_iOpenListAverage) // let's start at the end and work forwards
	{
		CvAStarNode* next;
		node = m_pOpenTail;
		next = NULL;

		while(node)
		{
			if(addnode->m_iTotalCost < node->m_iTotalCost)
			{
				next = node;
				node = node->m_pPrev;
			}
			else
			{
				if(next)
				{
					next->m_pPrev = addnode;
					addnode->m_pNext = next;
					addnode->m_pPrev = node;
					node->m_pNext = addnode;
					if(node->m_pNext == NULL)
					{
						m_pOpenTail = node;
					}

					udFunc(udNotifyList, addnode->m_pPrev, addnode, ASNL_ADDOPEN, m_pData);
				}
				else // we should just add it to the end of the list
				{
					addnode->m_pPrev = m_pOpenTail;
					m_pOpenTail->m_pNext = addnode;
					m_pOpenTail = addnode;

					udFunc(udNotifyList, addnode->m_pPrev, addnode, ASNL_ADDOPEN, m_pData);
				}

				return;
			}
		}

		// we made it to the start of this list - insert it at the beginning - we shouldn't ever get here, but...
		if (next)
		{
			next->m_pPrev = addnode;
			addnode->m_pNext = next;
			m_pOpen = addnode;

			udFunc(udNotifyList, m_pOpen->m_pNext, m_pOpen, ASNL_STARTOPEN, m_pData);
		}
	}
	else // let's start at the beginning as it should be closer
	{
		CvAStarNode* prev;
		node = m_pOpen;
		prev = NULL;

		while(node)
		{
			if(addnode->m_iTotalCost > node->m_iTotalCost)
			{
				prev = node;
				node = node->m_pNext;
			}
			else
			{
				if(prev)
				{
					prev->m_pNext = addnode;
					addnode->m_pPrev = prev;
					addnode->m_pNext = node;
					node->m_pPrev = addnode;
					if(node->m_pNext == NULL)
					{
						m_pOpenTail = node;
					}

					udFunc(udNotifyList, prev, addnode, ASNL_ADDOPEN, m_pData);
				}
				else
				{
					addnode->m_pNext = m_pOpen;
					m_pOpen->m_pPrev = addnode;
					m_pOpen = addnode;

					udFunc(udNotifyList, m_pOpen->m_pNext, m_pOpen, ASNL_STARTOPEN, m_pData);
				}

				return;
			}
		}

		// we made it to the end of this list - insert it at the end - we shouldn't ever get here, but...
		prev->m_pNext = addnode;
		addnode->m_pPrev = prev;
		m_pOpenTail = addnode;

		udFunc(udNotifyList, prev, addnode, ASNL_ADDOPEN, m_pData);
	}
}

//	--------------------------------------------------------------------------------
/// Connect in a node
void CvAStar::UpdateOpenNode(CvAStarNode* node)
{
	CvAStarNode* temp;

	FAssert(node->m_eCvAStarListType == CVASTARLIST_OPEN);

	if((node->m_pPrev != NULL) && (node->m_iTotalCost < node->m_pPrev->m_iTotalCost))
	{
		// have node free float for now
		node->m_pPrev->m_pNext = node->m_pNext;
		if(node->m_pNext)
		{
			node->m_pNext->m_pPrev = node->m_pPrev;
		}
		else
		{
			m_pOpenTail = node->m_pPrev;
		}
		// scoot down the list till we find where node goes (without connecting up as we go)
		temp = node->m_pPrev;
		while((temp != NULL) && (node->m_iTotalCost < temp->m_iTotalCost))
		{
			temp = temp->m_pPrev;
		}
		// connect node up
		if(temp != NULL)
		{
			node->m_pNext = temp->m_pNext;
			node->m_pPrev = temp;
			if(temp->m_pNext)
			{
				temp->m_pNext->m_pPrev = node;
			}
			temp->m_pNext = node;
		}
		else
		{
			node->m_pNext = m_pOpen;
			node->m_pPrev = NULL;
			if(node->m_pNext)
			{
				node->m_pNext->m_pPrev = node;
			}
			m_pOpen = node;
		}
	}
}

//	--------------------------------------------------------------------------------
/// Refresh parent node (after linking in a child)
void CvAStar::UpdateParents(CvAStarNode* node)
{
	CvAStarNode* kid;
	CvAStarNode* parent;
	int iKnownCost;
	int iNumChildren;
	int i;

	FAssert(m_pStackHead == NULL);

	parent = node;

	while(parent != NULL)
	{
		iNumChildren = parent->m_iNumChildren;

		for(i = 0; i < iNumChildren; i++)
		{
			kid = parent->m_apChildren[i];

			iKnownCost = (parent->m_iKnownCost + udFunc(udCost, parent, kid, 0, m_pData));

			if(iKnownCost < kid->m_iKnownCost)
			{
				kid->m_iKnownCost = iKnownCost;
				kid->m_iTotalCost = kid->m_iKnownCost + kid->m_iHeuristicCost;
				FAssert(parent->m_pParent != kid);
				kid->m_pParent = parent;
				if(kid->m_eCvAStarListType == CVASTARLIST_OPEN)
				{
					UpdateOpenNode(kid);
				}
				udFunc(udNotifyChild, parent, kid, ASNC_PARENTADD_UP, m_pData);

				Push(kid);
			}
		}

		parent = Pop();
	}
}

//	--------------------------------------------------------------------------------
/// Push a node on the stack
void CvAStar::Push(CvAStarNode* node)
{
	if(node->m_bOnStack)
	{
		return;
	}

	if(m_pStackHead == NULL)
	{
		m_pStackHead = &(m_ppaaNodes[node->m_iX][node->m_iY]);
	}
	else
	{
		FAssert(node->m_pStack == NULL);
		node->m_pStack = m_pStackHead;
		m_pStackHead = node;
	}

	node->m_bOnStack = true;
}

//	--------------------------------------------------------------------------------
/// Pop a node from the stack
CvAStarNode* CvAStar::Pop()
{
	CvAStarNode* node;

	if(m_pStackHead == NULL)
	{
		return NULL;
	}

	node = m_pStackHead;
	m_pStackHead = m_pStackHead->m_pStack;
	node->m_pStack = NULL;

	node->m_bOnStack = false;

	return node;
}

SPath CvAStar::GetPath() const
{
	SPath ret;
	ret.iCost = INT_MAX;
	ret.iTurnGenerated = GC.getGame().getGameTurn();

	CvAStarNode* pNode = GetLastNode();
	if (!pNode)
	{
		return ret;
	}

	ret.iCost = pNode->m_iKnownCost;

	//walk backwards ...
	while(pNode != NULL)
	{
		ret.vPlots.push_back( std::make_pair(pNode->m_iX, pNode->m_iY) );
		pNode = pNode->m_pParent;
	}

	std::reverse(ret.vPlots.begin(),ret.vPlots.end());
	return ret;
}

bool CvAStar::VerifyPath(const SPath& path)
{
	if (path.vPlots.size()<2)
		return false;

	int iKnownCost = 0;
	for (size_t i=1; i<path.vPlots.size(); i++)
	{
		CvAStarNode& current = m_ppaaNodes[ path.vPlots[i-1].first ][ path.vPlots[i-1].second ];
		CvAStarNode& next = m_ppaaNodes[ path.vPlots[i].first ][ path.vPlots[i].second ];

		if ( udFunc(udValid, &current, &next, 0, m_pData) )
		{
			iKnownCost += udFunc(udCost, &current, &next, 0, m_pData);
			if (iKnownCost > path.iCost)
				return false;
		}
		else
			return false;
	}

	return true;
}

//C-STYLE NON-MEMBER FUNCTIONS

// A structure holding some unit values that are invariant during a path plan operation
struct UnitPathCacheData
{
	int m_aBaseMoves[NUM_DOMAIN_TYPES];
	int m_iMaxMoves;
	PlayerTypes m_ePlayerID;
	TeamTypes m_eTeamID;
	DomainTypes m_eDomainType;

	bool m_bIsHuman;
	bool m_bIsAutomated;
	bool m_bIsImmobile;
	bool m_bIsNoRevealMap;
	bool m_bCanEverEmbark;
	bool m_bIsEmbarked;
	bool m_bCanAttack;
#ifdef AUI_DANGER_PLOTS_REMADE
	bool m_bDoDanger;
	inline bool DoDanger() const { return m_bDoDanger; }
#endif
	inline int baseMoves(DomainTypes eType) const { return m_aBaseMoves[eType]; }
	inline int maxMoves() const { return m_iMaxMoves; }
	inline PlayerTypes getOwner() const { return m_ePlayerID; }
	inline TeamTypes getTeam() const { return m_eTeamID; }
	inline DomainTypes getDomainType() const { return m_eDomainType; }
	inline bool isHuman() const { return m_bIsHuman; }
	inline bool IsAutomated() const { return m_bIsAutomated; }
	inline bool IsImmobile() const { return m_bIsImmobile; }
	inline bool isNoRevealMap() const { return m_bIsNoRevealMap; }
	inline bool CanEverEmbark() const { return m_bCanEverEmbark; }
	inline bool isEmbarked() const { return m_bIsEmbarked; }
	inline bool IsCanAttack() const { return m_bCanAttack; }
};

//	--------------------------------------------------------------------------------
void UnitPathInitialize(const void* pointer, CvAStar* finder)
{
	CvUnit* pUnit = ((CvUnit*)pointer);

	UnitPathCacheData* pCacheData = reinterpret_cast<UnitPathCacheData*>(finder->GetScratchBufferDirty());

	for (int i = 0; i < NUM_DOMAIN_TYPES; ++i)
	{
		pCacheData->m_aBaseMoves[i] = pUnit->baseMoves((DomainTypes)i);
	}

	pCacheData->m_iMaxMoves = pUnit->maxMoves();

	pCacheData->m_ePlayerID = pUnit->getOwner();
	pCacheData->m_eTeamID = pUnit->getTeam();
	pCacheData->m_eDomainType = pUnit->getDomainType();
	pCacheData->m_bIsHuman = pUnit->isHuman();
	pCacheData->m_bIsAutomated = pUnit->IsAutomated();
	pCacheData->m_bIsImmobile = pUnit->IsImmobile();
	pCacheData->m_bIsNoRevealMap = pUnit->isNoRevealMap();
	pCacheData->m_bCanEverEmbark = pUnit->CanEverEmbark();
	pCacheData->m_bIsEmbarked = pUnit->isEmbarked();
	pCacheData->m_bCanAttack = pUnit->IsCanAttack();

#ifdef AUI_DANGER_PLOTS_REMADE
	pCacheData->m_bDoDanger = (pCacheData->m_bIsAutomated || !pCacheData->isHuman()) && (!(finder->GetInfo() & MOVE_UNITS_IGNORE_DANGER)) && (!pUnit->IsCombatUnit() || pUnit->getArmyID() == -1);
#endif
}

//	--------------------------------------------------------------------------------
void UnitPathUninitialize(const void*, CvAStar*)
{

}

#if defined(AUI_ASTAR_FIX_NO_DUPLICATE_CALLS)
void UpdateNodeCacheData(CvAStarNode* node, const CvUnit* pUnit, bool bDoDanger, unsigned short iGenerationID)
{
	if (!node || !pUnit)
		return;

	CvPathNodeCacheData& kToNodeCacheData = node->m_kCostCacheData;
	if (kToNodeCacheData.iGenerationID==iGenerationID)
		return;

	const CvPlot* pPlot = GC.getMap().plotUnchecked(node->m_iX, node->m_iY);
	TeamTypes eUnitTeam = pUnit->getTeam();
	CvTeam& kUnitTeam = GET_TEAM(eUnitTeam);

	kToNodeCacheData.bPlotVisibleToTeam = pPlot->isVisible(eUnitTeam);
	kToNodeCacheData.iNumFriendlyUnitsOfType = pPlot->getNumFriendlyUnitsOfType(pUnit);
	kToNodeCacheData.bIsMountain = pPlot->isMountain();
	kToNodeCacheData.bIsWater = (pPlot->isWater() && !pPlot->IsAllowsWalkWater());
	kToNodeCacheData.bCanEnterTerrain = pUnit->canEnterTerrain(*pPlot, CvUnit::MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE);
	kToNodeCacheData.bIsRevealedToTeam = pPlot->isRevealed(eUnitTeam);
	kToNodeCacheData.bContainsOtherFriendlyTeamCity = false;
	CvCity* pCity = pPlot->getPlotCity();
	if (pCity)
	{
		if (pUnit->getOwner() != pCity->getOwner() && !kUnitTeam.isAtWar(pCity->getTeam()))
			kToNodeCacheData.bContainsOtherFriendlyTeamCity = true;
	}
	kToNodeCacheData.bContainsEnemyCity = pPlot->isEnemyCity(*pUnit);
	if (kToNodeCacheData.bPlotVisibleToTeam)
	{
		kToNodeCacheData.bContainsVisibleEnemy = pPlot->isVisibleEnemyUnit(pUnit);
		kToNodeCacheData.bContainsVisibleEnemyDefender = pPlot->isVisibleEnemyDefender(pUnit);
	}
	else
	{
		kToNodeCacheData.bContainsVisibleEnemy = false;
		kToNodeCacheData.bContainsVisibleEnemyDefender = false;
	}

#if defined(MOD_GLOBAL_STACKING_RULES)
	kToNodeCacheData.iUnitPlotLimit = pPlot->getUnitLimit();
#endif
#if defined(MOD_PATHFINDER_TERRAFIRMA)
	kToNodeCacheData.bIsTerraFirma = pPlot->isTerraFirma(pUnit) && !pPlot->IsAllowsWalkWater();
#endif
#ifdef AUI_DANGER_PLOTS_REMADE
	if (bDoDanger)
		kToNodeCacheData.iPlotDanger = GET_PLAYER(pUnit->getOwner()).GetPlotDanger(*pPlot, pUnit);
	else
		kToNodeCacheData.iPlotDanger = 0;
#endif

	//done!
	kToNodeCacheData.iGenerationID = iGenerationID;
}
#endif

//	--------------------------------------------------------------------------------
int PathDest(int iToX, int iToY, const void*, const CvAStar* finder)
{
	if(iToX == finder->GetDestX() && iToY == finder->GetDestY())
	{
		return true;
	}
	else
	{
		return false;
	}
}


//	--------------------------------------------------------------------------------
/// Standard path finder - is this end point for the path valid?
int PathDestValidGeneric(int iToX, int iToY, const void* pointer, const CvAStar* finder, bool bCheckStacking)
{
	CvPlot* pToPlot = GC.getMap().plotCheckInvalid(iToX, iToY);
	FAssert(pToPlot != NULL);

	CvUnit* pUnit = ((CvUnit*)pointer);
	const UnitPathCacheData* pCacheData = reinterpret_cast<const UnitPathCacheData*>(finder->GetScratchBuffer());

	if(pToPlot == NULL || pUnit == NULL)
		return FALSE;

	if(pUnit->plot() == pToPlot)
	{
		return TRUE;
	}

	if(pToPlot->isMountain() && 
		!pUnit->canEnterTerrain(*pToPlot,CvUnit::MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE) && 
		(!pCacheData->isHuman() || pCacheData->IsAutomated()))
	{
		return FALSE;
	}

	if(pCacheData->IsImmobile())
	{
		return FALSE;
	}

#if defined(MOD_BALANCE_CORE_PATHFINDER_FLAGS)
	if ((finder->GetInfo() & MOVE_NO_EMBARK) && (pToPlot->isWater() && !pToPlot->IsAllowsWalkWater()))
#else
	if ((finder->GetInfo() & CvUnit::MOVEFLAG_STAY_ON_LAND) && (pToPlot->isWater() && !pToPlot->IsAllowsWalkWater()))
#endif
	{
		return FALSE;
	}

	bCheckStacking = bCheckStacking && ((finder->GetInfo() & MOVE_IGNORE_STACKING)==0);
	bool bAIControl = pCacheData->IsAutomated();
	TeamTypes eTeam = pCacheData->getTeam();
	bool bToPlotRevealed = pToPlot->isRevealed(eTeam);
	if(!bToPlotRevealed)
	{
		if(pCacheData->isNoRevealMap())
		{
			return FALSE;
		}
	}

#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
	if(bToPlotRevealed && (!MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS || pUnit->IsCombatUnit()))
#else
	if(bToPlotRevealed)
#endif
	{
		CvCity* pCity = pToPlot->getPlotCity();
		if(pCity)
		{
			if(pCacheData->getOwner() != pCity->getOwner() && !GET_TEAM(eTeam).isAtWar(pCity->getTeam()) && bCheckStacking)
			{
				return FALSE;
			}
		}
	}

	if(bAIControl || bToPlotRevealed)
	{
		// assume that we can change our embarking state
		byte bMoveFlags = CvUnit::MOVEFLAG_DESTINATION | CvUnit::MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE;

		if((pUnit->IsDeclareWar() || (finder->GetInfo() & MOVE_DECLARE_WAR)))
		{
			bMoveFlags |= CvUnit::MOVEFLAG_ATTACK;
		}

		if(!bCheckStacking) 
		{
			bMoveFlags |= CvUnit::MOVEFLAG_IGNORE_STACKING;
		}

		if(!(pUnit->canMoveOrAttackInto(*pToPlot, bMoveFlags)))
		{
			return FALSE;
		}
	}
	return TRUE;
}


int PathDestValid(int iToX, int iToY, const void* pointer, const CvAStar* finder)
{
	return PathDestValidGeneric(iToX,iToY,pointer,finder,true);
}

int IgnoreUnitsDestValid(int iToX, int iToY, const void* pointer, const CvAStar* finder)
{
	return PathDestValidGeneric(iToX,iToY,pointer,finder,false);
}

//	--------------------------------------------------------------------------------
/// Standard path finder - determine heuristic cost
int PathHeuristic(int iFromX, int iFromY, int iToX, int iToY)
{
	return (plotDistance(iFromX, iFromY, iToX, iToY) * PATH_MOVEMENT_WEIGHT);
}

//	--------------------------------------------------------------------------------
/// Standard path finder - compute cost of a path
int PathCostGeneric(const CvAStarNode* parent, CvAStarNode* node, int, const void* pointer, const CvAStar* finder, bool bWithZOC)
{
	int iStartMoves = parent->m_iMoves;
	int iTurns = parent->m_iTurns;

	//check if we already found a better way ...
	if (node->m_iTurns>0)
	{
		if (node->m_iTurns<iTurns)
			return PATH_DO_NOT_USE_WEIGHT;

		if (node->m_iTurns==iTurns && node->m_iMoves>iStartMoves)
			return PATH_DO_NOT_USE_WEIGHT;
	}

	CvMap& kMap = GC.getMap();
	int iFromPlotX = parent->m_iX;
	int iFromPlotY = parent->m_iY;
	CvPlot* pFromPlot = kMap.plotUnchecked(iFromPlotX, iFromPlotY);

	int iToPlotX = node->m_iX;
	int iToPlotY = node->m_iY;
	CvPlot* pToPlot = kMap.plotUnchecked(iToPlotX, iToPlotY);

	CvUnit* pUnit = ((CvUnit*)pointer);
	const UnitPathCacheData* pCacheData = reinterpret_cast<const UnitPathCacheData*>(finder->GetScratchBuffer());

	TeamTypes eUnitTeam = pCacheData->getTeam();
	bool bMaximizeExplore = finder->GetInfo() & MOVE_MAXIMIZE_EXPLORE;
	DomainTypes eUnitDomain = pCacheData->getDomainType();
	CvAssertMsg(eUnitDomain != DOMAIN_AIR, "pUnit->getDomainType() is not expected to be equal with DOMAIN_AIR");

#if defined(MOD_PATHFINDER_TERRAFIRMA)
	bool bToPlotIsWater = !pToPlot->isTerraFirma(pUnit) && !pToPlot->IsAllowsWalkWater();
	bool bFromPlotIsWater = !pToPlot->isTerraFirma(pUnit) && !pToPlot->IsAllowsWalkWater();
#else
	bool bToPlotIsWater = pToPlot->isWater() && !pToPlot->IsAllowsWalkWater();
#endif

	//if we would have to start a new turn
	if (iStartMoves==0)
	{
		iTurns++;

		if (CvUnitMovement::ConsumesAllMoves(pUnit, pFromPlot, pToPlot) || (bWithZOC && CvUnitMovement::IsSlowedByZOC(pUnit, pFromPlot, pToPlot)))
		{
			// The movement would consume all moves, get the moves we will forfeit based on the source plot, rather than
			// the destination plot.  This fixes issues where a land unit that has more movement points on water than on land
			// would have a very high cost to move onto water if their first move of the turn was at the edge of the water.
			iStartMoves = pCacheData->baseMoves(bFromPlotIsWater?DOMAIN_SEA:DOMAIN_LAND) * GC.getMOVE_DENOMINATOR();
		}
		else
			iStartMoves = pCacheData->baseMoves(bToPlotIsWater?DOMAIN_SEA:DOMAIN_LAND) * GC.getMOVE_DENOMINATOR();
	}

	// Get the cost of moving to the new plot, passing in our max moves or the moves we have left, in case the movementCost 
	// method wants to burn all our remaining moves.  This is needed because our remaining moves for this segment of the path
	// may be larger or smaller than the baseMoves if some moves have already been used or if the starting domain (LAND/SEA)
	// of the path segment is different from the destination plot.
	DomainTypes eDomain = (pToPlot->isWater() || pCacheData->isEmbarked()) ? DOMAIN_SEA : eUnitDomain;
	int iCost = 0;
	
	if (bWithZOC)
		iCost = CvUnitMovement::MovementCost(pUnit, pFromPlot, pToPlot, pCacheData->baseMoves(eDomain), pCacheData->maxMoves(), iStartMoves);
	else
		iCost = CvUnitMovement::MovementCostNoZOC(pUnit, pFromPlot, pToPlot, pCacheData->baseMoves(eDomain), pCacheData->maxMoves(), iStartMoves);

	int iMovesLeft = iStartMoves - iCost;
	// Is the cost greater than our max?
	if (iMovesLeft < 0)
	{
		// Yes, we will still let the move happen, but that is the end of the turn.
		iCost = iStartMoves;
		iMovesLeft = 0;
	}

	//check again if we already found a better way ...
	if (node->m_iTurns>0)
	{
		if (node->m_iTurns<iTurns)
			return PATH_DO_NOT_USE_WEIGHT;

		if (node->m_iTurns==iTurns && node->m_iMoves>iMovesLeft)
			return PATH_DO_NOT_USE_WEIGHT;
	}

	//important: store the remaining moves in the node so we don't have to recalculate later
	node->m_iMoves = iMovesLeft;
	node->m_iTurns = iTurns;

	//base cost
	iCost = (PATH_MOVEMENT_WEIGHT * iCost);

	//extra cost for ending the turn on various types of undesirable plots
	if(iMovesLeft == 0)
	{
		if(pUnit->IsCombatUnit())
		{
			iCost += (PATH_DEFENSE_WEIGHT * std::max(0, (200 - ((pUnit->noDefensiveBonus()) ? 0 : pToPlot->defenseModifier(eUnitTeam, false)))));
		}

		if(bMaximizeExplore)
		{
			if(!pToPlot->isHills())
			{
				iCost += PATH_EXPLORE_NON_HILL_WEIGHT;
			}
		}

#if defined(MOD_BALANCE_CORE_MILITARY)
		if (finder->GetInfo() & MOVE_MINIMIZE_ENEMY_TERRITORY)
		{
			if (pToPlot->getOwner()!=NO_PLAYER)
			{
				CvTeam& plotTeam = GET_TEAM(GET_PLAYER(pToPlot->getOwner()).getTeam());
				if (!plotTeam.IsAllowsOpenBordersToTeam(GET_PLAYER(pUnit->getOwner()).getTeam()))
					iCost += PATH_ENEMY_TERRITORY_WEIGHT;
			}
		}
#endif

		// Damage caused by features (mods)
		if(0 != GC.getPATH_DAMAGE_WEIGHT())
		{
			if(pToPlot->getFeatureType() != NO_FEATURE)
			{
#if defined(MOD_API_PLOT_BASED_DAMAGE)
				if (MOD_API_PLOT_BASED_DAMAGE) {
					iCost += (GC.getPATH_DAMAGE_WEIGHT() * std::max(0, pToPlot->getTurnDamage(pUnit->ignoreTerrainDamage(), pUnit->ignoreFeatureDamage(), pUnit->extraTerrainDamage(), pUnit->extraFeatureDamage()))) / GC.getMAX_HIT_POINTS();
				} else {
#endif
					iCost += (GC.getPATH_DAMAGE_WEIGHT() * std::max(0, GC.getFeatureInfo(pToPlot->getFeatureType())->getTurnDamage())) / GC.getMAX_HIT_POINTS();
#if defined(MOD_API_PLOT_BASED_DAMAGE)
				}
#endif
			}

			if(pToPlot->getExtraMovePathCost() > 0)
			{
				iCost += (PATH_MOVEMENT_WEIGHT * pToPlot->getExtraMovePathCost());
			}
		}

		if (pUnit->isHasPromotion((PromotionTypes)GC.getPROMOTION_UNWELCOME_EVANGELIST()))
		{
			// Avoid being in a territory that we are not welcome in, unless the human is manually controlling the unit.
			if (!pCacheData->isHuman() || pCacheData->IsAutomated())
			{
				// Also, ignore the penalty if the destination of the path is in the same team's territory, no sense in avoiding a place we want to get to.				
				PlayerTypes ePlotOwner = pToPlot->getOwner();
				CvPlot* pDestPlot = (finder->GetDestX() >= 0 && finder->GetDestY() >= 0)?kMap.plotCheckInvalid(finder->GetDestX(), finder->GetDestY()):NULL;
				if (!pDestPlot || pDestPlot->getOwner() != ePlotOwner)
				{
					TeamTypes ePlotTeam = pToPlot->getTeam();
					if (ePlotOwner != NO_PLAYER && !GET_PLAYER(ePlotOwner).isMinorCiv() && ePlotTeam != pCacheData->getTeam() && !GET_TEAM(ePlotTeam).IsAllowsOpenBordersToTeam(pCacheData->getTeam()))
					{
						iCost += PATH_END_TURN_MISSIONARY_OTHER_TERRITORY;
					}
				}
			}
		}
		else
		{
			if(pToPlot->getTeam() != eUnitTeam)
			{
				iCost += PATH_TERRITORY_WEIGHT;
			}
		}

#if defined(MOD_BALANCE_CORE)
		//danger check
		if (!pCacheData->isHuman() || pCacheData->IsAutomated())
		{
			if( (finder->GetInfo() & MOVE_UNITS_IGNORE_DANGER)==0 )
			{
				int iPlotDanger = node->m_kCostCacheData.iPlotDanger;

				if (iPlotDanger == MAX_INT)
					iCost += PATH_END_TURN_MORTAL_DANGER_WEIGHT;
				else if (pUnit->IsCombatUnit() && !pUnit->isEmbarked())
				{
					//combat units can still tolerate some danger - whether with an army or without
					if (iPlotDanger >= pUnit->GetCurrHitPoints()/2)
						iCost += PATH_END_TURN_MORTAL_DANGER_WEIGHT;
				}
				else if (iPlotDanger>0)
					//for civilians the danger==MAX_INT condition should apply, but better be safe
					iCost += PATH_END_TURN_MORTAL_DANGER_WEIGHT;
			}
		}
#endif

	}

	if(bMaximizeExplore)
	{
		int iUnseenPlots = pToPlot->getNumAdjacentNonrevealed(eUnitTeam);
		if(!pToPlot->isRevealed(eUnitTeam))
		{
			iUnseenPlots += 1;
		}

		iCost += (7 - iUnseenPlots) * PATH_EXPLORE_NON_REVEAL_WEIGHT;
	}

	// If we are a land unit and we are moving through the water, make the cost a little higher so that
	// we favor staying on land or getting back to land as quickly as possible because it is dangerous to
	// be on the water.  Don't add this penalty if the unit is human controlled however, we will assume they want
	// the best path, rather than the safest.
	if(eUnitDomain == DOMAIN_LAND && bToPlotIsWater && (!pCacheData->isHuman() || pCacheData->IsAutomated()))
	{
		iCost += PATH_THROUGH_WATER;
	}

	if(pUnit->IsCombatUnit())
	{
		if(pCacheData->IsAutomated())
		{
			if(pCacheData->IsCanAttack())
			{
				if(finder->IsPathDest(iToPlotX, iToPlotY))
				{
					if(pToPlot->isVisibleEnemyDefender(pUnit))
					{
						iCost += (PATH_DEFENSE_WEIGHT * std::max(0, (200 - ((pUnit->noDefensiveBonus()) ? 0 : pFromPlot->defenseModifier(eUnitTeam, false)))));

						if(!(pUnit->isRiverCrossingNoPenalty()))
						{
							if(pFromPlot->isRiverCrossing(directionXY(pFromPlot, pToPlot)))
							{
								iCost += (PATH_RIVER_WEIGHT * -(GC.getRIVER_ATTACK_MODIFIER()));
								iCost += (PATH_MOVEMENT_WEIGHT * iMovesLeft);
							}
						}
					}
				}
			}
		}
	}

	FAssert(iCost != MAX_INT);

	iCost += PATH_STEP_WEIGHT;

	FAssert(iCost > 0);

	return iCost;
}

int PathCost(const CvAStarNode* parent, CvAStarNode* node, int, const void* pointer, const CvAStar* finder)
{
	return PathCostGeneric(parent,node,0,pointer,finder,true);
}

int IgnoreUnitsCost(const CvAStarNode* parent, CvAStarNode* node, int, const void* pointer, const CvAStar* finder)
{
	return PathCostGeneric(parent,node,0,pointer,finder,false);
}

//	---------------------------------------------------------------------------
/// Standard path finder - check validity of a coordinate
int PathValidGeneric(const CvAStarNode* parent, const CvAStarNode* node, int, const void* pointer, const CvAStar* finder, bool bCheckStacking)
{
	// If this is the first node in the path, it is always valid (starting location)
	if (parent == NULL)
		return TRUE;

	CvMap& theMap = GC.getMap();
	CvPlot* pFromPlot = theMap.plotUnchecked(parent->m_iX, parent->m_iY);
	CvPlot* pToPlot = theMap.plotUnchecked(node->m_iX, node->m_iY);

	CvUnit* pUnit = ((CvUnit*)pointer);
	const UnitPathCacheData* pCacheData = reinterpret_cast<const UnitPathCacheData*>(finder->GetScratchBuffer());
	TeamTypes eUnitTeam = pCacheData->getTeam();
	PlayerTypes unit_owner = pCacheData->getOwner();

	// Cache values for this node that we will use in the loop
	const CvPathNodeCacheData& kToNodeCacheData = node->m_kCostCacheData;
	const CvPathNodeCacheData& kFromNodeCacheData = parent->m_kCostCacheData;

	// pulling invariants out of the loop
	DomainTypes unitDomain		= pCacheData->getDomainType();
	bool bUnitIsCombat          = pUnit->IsCombatUnit();
	int iFinderInfo             = finder->GetInfo();

	bCheckStacking				= bCheckStacking & ((iFinderInfo & MOVE_IGNORE_STACKING)==0);

	// We have determined that this node is not the origin above (parent == NULL)
	bool bNewTurn = (parent->m_iMoves==0);
	bool bNextNodeHostile = kToNodeCacheData.bContainsEnemyCity || kToNodeCacheData.bContainsVisibleEnemyDefender;
	bool bNextNodeVisibleToTeam = kToNodeCacheData.bPlotVisibleToTeam;
	int iNodeX = parent->m_iX;
	int iNodeY = parent->m_iY;
	int iUnitX = pUnit->getX();
	int iUnitY = pUnit->getY();

	// we would start a new turn or run into an enemy or run into unknown territory, so we must be able to end the turn on the parent plot
	if (bNewTurn || bNextNodeHostile || !bNextNodeVisibleToTeam)
	{
		// however, the checks only apply if the plot is visible
		if (kFromNodeCacheData.bPlotVisibleToTeam)
		{
			// check stacking (if visible)
			if(bCheckStacking && kFromNodeCacheData.iNumFriendlyUnitsOfType >= kFromNodeCacheData.iUnitPlotLimit)
			{
				// Don't count origin, or else a unit will block its own movement!
				if(iNodeX != iUnitX || iNodeY != iUnitY)
					return FALSE;
			}

			// check impassable terrain
			if(kFromNodeCacheData.bIsMountain && !kFromNodeCacheData.bCanEnterTerrain)
			{
				return FALSE;
			}

#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
			if(!MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS || bUnitIsCombat)
#else
			if(true)
#endif
			{
				if (kFromNodeCacheData.bContainsOtherFriendlyTeamCity)
					return FALSE;
			}
		}
	}

	// slewis - moved this up so units can't move directly into the water. Not 100% sure this is the right solution.
	if(unitDomain == DOMAIN_LAND)
	{
#if defined(MOD_BALANCE_CORE_PATHFINDER_FLAGS)
		if ((iFinderInfo & MOVE_NO_EMBARK) && kToNodeCacheData.bIsWater && kFromNodeCacheData.bIsTerraFirma)
#else
		if ((iFinderInfo & CvUnit::MOVEFLAG_STAY_ON_LAND) && kToNodeCacheData.bIsWater)
#endif
		{
			return FALSE;
		}

#if defined(MOD_PATHFINDER_TERRAFIRMA)
		bool bFromTerraFirma = kFromNodeCacheData.bIsTerraFirma;
		bool bToWater = !kToNodeCacheData.bIsTerraFirma;
		if( bFromTerraFirma && bToWater && kToNodeCacheData.bIsRevealedToTeam && !pUnit->canEmbarkOnto(*pFromPlot, *pToPlot, true))
#else
		if(!kFromNodeCacheData.bIsWater && kToNodeCacheData.bIsWater && kToNodeCacheData.bIsRevealedToTeam && !pUnit->canEmbarkOnto(*pFromPlot, *pToPlot, true))
#endif
		{
#if defined(MOD_PATHFINDER_DEEP_WATER_EMBARKATION)
			if(!pUnit->canMoveAllTerrain() && !pToPlot->IsAllowsWalkWater())
#else
			if(!pUnit->IsHoveringUnit() && !pUnit->canMoveAllTerrain() && !pToPlot->IsAllowsWalkWater())
#endif
			{
				return FALSE;
			}
		}
	}

#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
	if (!MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS) {
#endif
	if(bCheckStacking && !bUnitIsCombat && unitDomain != DOMAIN_AIR)
	{
		const PlayerTypes eUnitPlayer = unit_owner;
		const int iUnitCount = pToPlot->getNumUnits();
		for(int iUnit = 0; iUnit < iUnitCount; ++iUnit)
		{
			const CvUnit* pToPlotUnit = pToPlot->getUnitByIndex(iUnit);
			if(pToPlotUnit != NULL && pToPlotUnit->getOwner() != eUnitPlayer)
			{
				return FALSE; // Plot occupied by another player
			}
		}
	}
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
	}
#endif

	bool bFromPlotOwned          = pFromPlot->isOwned();
	TeamTypes eFromPlotTeam      = pFromPlot->getTeam();
	if(iFinderInfo & MOVE_TERRITORY_NO_UNEXPLORED)
	{
		if(!(kFromNodeCacheData.bIsRevealedToTeam))
		{
			return FALSE;
		}

		if(bFromPlotOwned)
		{
			if(eFromPlotTeam != eUnitTeam)
			{
				return FALSE;
			}
		}
	}

	if(iFinderInfo & MOVE_TERRITORY_NO_ENEMY)
	{
		if(bFromPlotOwned)
		{
			if(atWar(eFromPlotTeam, eUnitTeam))
			{
				return FALSE;
			}
		}
	}

	// slewis - added AI check and embark check to prevent units from moving into unexplored areas
	if(pCacheData->IsAutomated() || kFromNodeCacheData.bIsRevealedToTeam || pCacheData->isEmbarked() || !pCacheData->isHuman())
	{
		if(iFinderInfo & MOVE_UNITS_THROUGH_ENEMY)
		{
			if(!(pUnit->canMoveOrAttackInto(*pFromPlot, CvUnit::MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE)))
			{
				return FALSE;
			}
		}
		else
		{
			if(!(pUnit->canMoveThrough(*pFromPlot, CvUnit::MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE)))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

int PathValid(const CvAStarNode* parent, const CvAStarNode* node, int, const void* pointer, const CvAStar* finder)
{
	return PathValidGeneric(parent,node,0,pointer,finder,true);
}

int IgnoreUnitsValid(const CvAStarNode* parent, const CvAStarNode* node, int, const void* pointer, const CvAStar* finder)
{
	return PathValidGeneric(parent,node,0,pointer,finder,false);
}

//	--------------------------------------------------------------------------------
/// Standard path finder - add a new path
int PathAdd(CvAStarNode*, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
	const CvUnit* pUnit = ((const CvUnit*)pointer);
	const UnitPathCacheData* pCacheData = reinterpret_cast<const UnitPathCacheData*>(finder->GetScratchBuffer());

	if(data == ASNC_INITIALADD)
	{
		//in this case we did not call PathCost() before, so we have to set the initial values here
		node->m_iMoves = pUnit->movesLeft();
		node->m_iTurns = 1;
	}

	//update cache for new node and all possible children
	UpdateNodeCacheData(node,pUnit,pCacheData->DoDanger(),finder->GetCurrentGenerationID());

	for(int i = 0; i < 6; i++)
	{
		CvAStarNode* neighbor = node->m_apNeighbors[i];
		if(neighbor && neighbor!=node->m_pParent)
			UpdateNodeCacheData(neighbor,pUnit,pCacheData->DoDanger(),finder->GetCurrentGenerationID());
	}

	for(int i = 0; i < finder->GetNumExtraChildren(node); i++)
	{
		CvAStarNode* neighbor = finder->GetExtraChild(node,i);
		if (neighbor)
			UpdateNodeCacheData(neighbor,pUnit,pCacheData->DoDanger(),finder->GetCurrentGenerationID());
	}

	return 1;
}

//	--------------------------------------------------------------------------------
/// Two layer path finder - if add a new open node with movement left, add a second one assuming stop for turn here
int PathNodeAdd(CvAStarNode* /*parent*/, CvAStarNode* node, int data, const void*, CvAStar* finder)
{
	CvAStarNode* pNode;

	if(data == ASNL_ADDOPEN || data == ASNL_STARTOPEN)
	{
		// Are there movement points left and we're worried about stacking or mountains?
		if(node->m_iMoves > 0 && !finder->IsPathDest(node->m_iX, node->m_iY) && (!(finder->GetInfo() & MOVE_IGNORE_STACKING) || GC.getMap().plotUnchecked(node->m_iX, node->m_iY)->isMountain()))
		{
			// Retrieve another node
			CvTwoLayerPathFinder* twoLayerFinder = static_cast<CvTwoLayerPathFinder*>(finder);
			pNode = twoLayerFinder->GetPartialMoveNode(node->m_iX, node->m_iY);
			pNode->m_iMoves = 0;   // Zero out movement
			pNode->m_iTurns = node->m_iTurns;
			pNode->m_iHeuristicCost = node->m_iHeuristicCost;
			//wasting movement points is bad
			pNode->m_iKnownCost = node->m_iKnownCost + (PATH_MOVEMENT_WEIGHT * node->m_iMoves);
#if defined(MOD_CORE_PATHFINDER)
			//if we decide to stop here, make sure we're allowed to
			if( (finder->GetInfo() & MOVE_UNITS_IGNORE_DANGER)==0 )
			{
				const UnitPathCacheData* pCacheData = reinterpret_cast<const UnitPathCacheData*>(finder->GetScratchBuffer());
				if (!pCacheData->isHuman() || pCacheData->IsAutomated())
				{
					if (node->m_kCostCacheData.iPlotDanger==INT_MAX)
						pNode->m_iKnownCost += PATH_END_TURN_MORTAL_DANGER_WEIGHT;
				}
			}

			//we sort the nodes by total cost!
			pNode->m_iTotalCost = pNode->m_iKnownCost + pNode->m_iHeuristicCost;
#else
			pNode->m_iTotalCost = node->m_iTotalCost;
#endif
			pNode->m_iX = node->m_iX;
			pNode->m_iY = node->m_iY;
			pNode->m_pParent = node->m_pParent;
			pNode->m_eCvAStarListType = CVASTARLIST_OPEN;
			pNode->m_kCostCacheData = node->m_kCostCacheData;
			finder->AddToOpen(pNode);
		}
	}

	return 1;
}

//	--------------------------------------------------------------------------------
/// Step path finder - is this end point for the path valid?
int StepDestValid(int iToX, int iToY, const void*, const CvAStar* finder)
{
	CvPlot* pFromPlot;
	CvPlot* pToPlot;

	CvMap& kMap = GC.getMap();
	pFromPlot = kMap.plotUnchecked(finder->GetStartX(), finder->GetStartY());
	pToPlot = kMap.plotUnchecked(iToX, iToY);

	if(pFromPlot->getArea() != pToPlot->getArea())
	{
		return FALSE;
	}

	return TRUE;
}

//	--------------------------------------------------------------------------------
/// Step path finder - determine heuristic cost
int StepHeuristic(int iFromX, int iFromY, int iToX, int iToY)
{
	return plotDistance(iFromX, iFromY, iToX, iToY);
}


//	--------------------------------------------------------------------------------
/// Step path finder - compute cost of a path
int StepCost(const CvAStarNode*, CvAStarNode* node, int, const void*, const CvAStar*)
{
	CvPlot* pNewPlot = GC.getMap().plotUnchecked(node->m_iX, node->m_iY);
	
	return pNewPlot->isRoughGround() ? 2 : 1;
}


//	--------------------------------------------------------------------------------
/// Step path finder - check validity of a coordinate
int StepValidGeneric(const CvAStarNode* parent, const CvAStarNode* node, int, const void* pointer, const CvAStar* finder, bool bAnyArea, bool bWide)
{
	if(parent == NULL || pointer==NULL)
	{
		return FALSE;
	}

	int iFlags = finder->GetInfo();
	PlayerTypes ePlayer = (PlayerTypes)(iFlags & 0xFF);
	PlayerTypes eEnemy = *(PlayerTypes*)pointer;
	CvPlayer& thisPlayer = GET_PLAYER(ePlayer);

	CvMap& kMap = GC.getMap();
	CvPlot* pToPlot = kMap.plotUnchecked(node->m_iX, node->m_iY);
	CvPlot* pFromPlot = kMap.plotUnchecked(parent->m_iX, parent->m_iY);

	//this is the important check here - stay within the same area
	if(!bAnyArea && pFromPlot->getArea() != pToPlot->getArea())
	{
		return FALSE;
	}

#if defined(MOD_BALANCE_CORE)
	if(pToPlot->isImpassable(thisPlayer.getTeam()) || (pToPlot->isMountain() && !thisPlayer.GetPlayerTraits()->IsMountainPass()))
#else
	if(pToPlot->isImpassable() || pToPlot->isMountain())
#endif
	{
		return FALSE;
	}

	// Ocean hex and team can't navigate on oceans?
	if (!GET_TEAM(thisPlayer.getTeam()).getEmbarkedAllWaterPassage())
	{
		if (pToPlot->getTerrainType() == TERRAIN_OCEAN)
		{
			return FALSE;
		}
	}

	PlayerTypes ePlotOwnerPlayer = pToPlot->getOwner();
	if (ePlotOwnerPlayer != NO_PLAYER && ePlotOwnerPlayer != eEnemy && !pToPlot->IsFriendlyTerritory(ePlayer))
	{
		CvPlayer& plotOwnerPlayer = GET_PLAYER(ePlotOwnerPlayer);
		bool bPlotOwnerIsMinor = plotOwnerPlayer.isMinorCiv();

		if(!bPlotOwnerIsMinor)
		{
			TeamTypes eMyTeam = thisPlayer.getTeam();
			TeamTypes ePlotOwnerTeam = plotOwnerPlayer.getTeam();

			if(!atWar(eMyTeam, ePlotOwnerTeam))
			{
				return FALSE;
			}
		}
	}

	//for multi-unit formations it makes sense to have a wide path
	if (bWide)
	{
		//direction looking backward!
		DirectionTypes eRear = directionXY(pToPlot,pFromPlot);

		int eRearLeft = (int(eRear) + 5) % 6; 
		int eRearRight = (int(eRear) + 1) % 6;
		const CvAStarNode* rl = node->m_apNeighbors[eRearLeft];
		const CvAStarNode* rr = node->m_apNeighbors[eRearRight];

		if (!StepValidGeneric(parent,rl,0,pointer,finder,bAnyArea,false))
			return false;
		if (!StepValidGeneric(parent,rr,0,pointer,finder,bAnyArea,false))
			return false;
	}

	return TRUE;
}

int StepValid(const CvAStarNode* parent, const CvAStarNode* node, int, const void* pointer, const CvAStar* finder)
{
	return StepValidGeneric(parent,node,0,pointer,finder,false,false);
}
int StepValidAnyArea(const CvAStarNode* parent, const CvAStarNode* node, int, const void* pointer, const CvAStar* finder)
{
	return StepValidGeneric(parent,node,0,pointer,finder,true,false);
}
int StepValidWide(const CvAStarNode* parent, const CvAStarNode* node, int, const void* pointer, const CvAStar* finder)
{
	return StepValidGeneric(parent,node,0,pointer,finder,false,true);
}
int StepValidWideAnyArea(const CvAStarNode* parent, const CvAStarNode* node, int, const void* pointer, const CvAStar* finder)
{
	return StepValidGeneric(parent,node,0,pointer,finder,true,true);
}

//	--------------------------------------------------------------------------------
/// Step path finder - add a new path
int StepAdd(CvAStarNode* parent, CvAStarNode* node, int data, const void*, CvAStar*)
{
	if(data == ASNC_INITIALADD)
	{
		node->m_iTurns = 0;
	}
	else
	{
		node->m_iTurns = (parent->m_iTurns + 1);
	}

	node->m_iMoves = 0;
	return 1;
}

//	--------------------------------------------------------------------------------
/// Influence path finder - is this end point for the path valid?
int InfluenceDestValid(int iToX, int iToY, const void* pointer, const CvAStar* finder)
{
	CvPlot* pFromPlot;
	CvPlot* pToPlot;

	CvMap& kMap = GC.getMap();
	pFromPlot = kMap.plotUnchecked(finder->GetStartX(), finder->GetStartY());
	pToPlot = kMap.plotUnchecked(iToX, iToY);

	if(pointer)
	{
		int iRange = abs(*(int*)pointer);
		if(plotDistance(pFromPlot->getX(),pFromPlot->getY(),pToPlot->getX(),pToPlot->getY()) > iRange)
		{
			return FALSE;
		}
	}

	return TRUE;
}


//	--------------------------------------------------------------------------------
/// Influence path finder - determine heuristic cost
int InfluenceHeuristic(int iFromX, int iFromY, int iToX, int iToY)
{
	return plotDistance(iFromX, iFromY, iToX, iToY);
}

//	--------------------------------------------------------------------------------
/// Influence path finder - compute cost of a path
int InfluenceCost(const CvAStarNode* parent, CvAStarNode* node, int, const void* pointer, const CvAStar* finder)
{
	int iCost = 0;
	bool bDifferentOwner = false;
	if(parent->m_pParent || GC.getUSE_FIRST_RING_INFLUENCE_TERRAIN_COST())
	{
		CvMap& kMap = GC.getMap();
		CvPlot* pFromPlot = kMap.plotUnchecked(parent->m_iX, parent->m_iY);
		CvPlot* pToPlot = kMap.plotUnchecked(node->m_iX, node->m_iY);
		CvPlot* pSourcePlot = kMap.plotUnchecked(finder->GetStartX(), finder->GetStartY());

		int iRange = 0;
		if(pointer)
		{
			iRange = *(int*)pointer;
		}
		if(iRange >= 0)
		{
			if(pToPlot->getOwner() != NO_PLAYER && pSourcePlot->getOwner() != NO_PLAYER && pToPlot->getOwner() != pSourcePlot->getOwner())
				bDifferentOwner = true;
		}

		if(pFromPlot->isRiverCrossing(directionXY(pFromPlot, pToPlot)))
			iCost += GC.getINFLUENCE_RIVER_COST();

		// Mountain Cost
		if(pToPlot->isMountain())
			iCost += GC.getINFLUENCE_MOUNTAIN_COST();
		// Not a mountain - use the terrain cost
		else
		{
			// Hill cost
			if(pToPlot->isHills())
				iCost += GC.getINFLUENCE_HILL_COST();
			iCost += GC.getTerrainInfo(pToPlot->getTerrainType())->getInfluenceCost();
			iCost += ((pToPlot->getFeatureType() == NO_FEATURE) ? 0 : GC.getFeatureInfo(pToPlot->getFeatureType())->getInfluenceCost());
		}
	}
	else
	{
		iCost = 1;
	}
	iCost = std::max(1,iCost);
	iCost = std::min(3,iCost);
	if (bDifferentOwner)
	{
		iCost += 15;
	}
	return iCost;
}


//	--------------------------------------------------------------------------------
/// Influence path finder - check validity of a coordinate
int InfluenceValid(const CvAStarNode* parent, const CvAStarNode* node, int, const void*, const CvAStar*)
{
	CvPlot* pNewPlot;

	if(parent == NULL)
	{
		return TRUE;
	}

	pNewPlot = GC.getMap().plotCheckInvalid(node->m_iX, node->m_iY);

	if(pNewPlot == NULL)
	{
		return FALSE;
	}

	// todo: a check to see if we are within the theoretical influence range would be great

	return TRUE;
}


//	--------------------------------------------------------------------------------
/// Influence path finder - add a new path
int InfluenceAdd(CvAStarNode* parent, CvAStarNode* node, int data, const void*, CvAStar*)
{
	if(data == ASNC_INITIALADD)
	{
		node->m_iTurns = 0;
	}
	else
	{
		node->m_iTurns = (parent->m_iTurns + 1/*influenceCost(parent,node,data,pointer,finder)*/);
	}

	node->m_iMoves = 0;
	return 1;
}


//	--------------------------------------------------------------------------------
// Route - Return the x, y plot of the node that we want to access
int RouteGetExtraChild(const CvAStarNode* node, int iIndex, int& iX, int& iY, const CvAStar* finder)
{
	iX = -1;
	iY = -1;

	PlayerTypes ePlayer = ((PlayerTypes)(finder->GetInfo() & 0xFF));
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	TeamTypes eTeam = kPlayer.getTeam();
	CvPlot* pPlot = GC.getMap().plotCheckInvalid(node->m_iX, node->m_iY);

	if(!pPlot)
	{
		return 0;
	}

	CvCity* pCity = pPlot->getPlotCity();

	// if there isn't a city there or the city isn't on our team
	if(!pCity || pCity->getTeam() != eTeam)
	{
		return 0;
	}

	int iValidCount = 0;
	CvCityConnections* pCityConnections = kPlayer.GetCityConnections();

	uint uiFirstCityIndex = pCityConnections->GetIndexFromCity(pCity);
	for(uint uiSecondCityIndex = 0; uiSecondCityIndex < pCityConnections->m_aiCityPlotIDs.size(); uiSecondCityIndex++)
	{
		if(uiFirstCityIndex == uiSecondCityIndex)
		{
			continue;
		}

		CvCityConnections::RouteInfo* pRouteInfo = pCityConnections->GetRouteInfo(uiFirstCityIndex, uiSecondCityIndex);
		if(!pRouteInfo)
		{
			continue;
		}

		// get the two cities
		CvCity* pFirstCity  = pCityConnections->GetCityFromIndex(uiFirstCityIndex);
		CvCity* pSecondCity = pCityConnections->GetCityFromIndex(uiSecondCityIndex);

		if(!pFirstCity || !pSecondCity)
		{
			continue;
		}

#if defined(MOD_EVENTS_CITY_CONNECTIONS)
		if(pRouteInfo->m_cRouteState & CvCityConnections::HAS_INDIRECT_ROUTE)
#else
		if(pRouteInfo->m_cRouteState & CvCityConnections::HAS_WATER_ROUTE)
#endif
		{
			if(iValidCount == iIndex)
			{
				iX = pSecondCity->getX();
				iY = pSecondCity->getY();
				return 1;
			}
			iValidCount++;
		}
	}

	return 0;
}

//	---------------------------------------------------------------------------
/// Route path finder - check validity of a coordinate
/// This function does not require the global Tactical Analysis Map.
int RouteValid(const CvAStarNode* parent, const CvAStarNode* node, int, const void*, const CvAStar* finder)
{
	CvPlot* pNewPlot;

	if(parent == NULL)
	{
		return TRUE;
	}

	int iFlags = finder->GetInfo();
	PlayerTypes ePlayer = (PlayerTypes)(iFlags & 0xFF);
	pNewPlot = GC.getMap().plotUnchecked(node->m_iX, node->m_iY);

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	if((iFlags & MOVE_ROUTE_ALLOW_UNEXPLORED) == 0 && !(pNewPlot->isRevealed(kPlayer.getTeam())))
	{
		return FALSE;
	}

#if defined(MOD_BALANCE_CORE)
	RouteTypes eRouteType = pNewPlot->getRouteType();

	if(pNewPlot->IsRoutePillaged())
		eRouteType = NO_ROUTE;

	if (eRouteType==NO_ROUTE)
	{
		//what else can count as road depends on the player type
		if(kPlayer.GetPlayerTraits()->IsRiverTradeRoad())
		{
			if(pNewPlot->isRiver())
				eRouteType = ROUTE_ROAD;
		}
		if(kPlayer.GetPlayerTraits()->IsMountainPass())
		{
			if(pNewPlot->isMountain())
				eRouteType = ROUTE_ROAD;
		}
		if(kPlayer.GetPlayerTraits()->IsMoveFriendlyWoodsAsRoad())
		{
			if(pNewPlot->getOwner() == ePlayer)
				if(pNewPlot->getFeatureType() == FEATURE_FOREST || pNewPlot->getFeatureType() == FEATURE_JUNGLE)
					eRouteType = ROUTE_ROAD;
		}
	}

#else
	if(kPlayer.GetPlayerTraits()->IsMoveFriendlyWoodsAsRoad())
	{
		if(pNewPlot->getOwner() == ePlayer)
		{
			if(pNewPlot->getFeatureType() == FEATURE_FOREST || pNewPlot->getFeatureType() == FEATURE_JUNGLE)
			{
				return TRUE;
			}
		}
	}

	RouteTypes eRouteType = pNewPlot->getRouteType();
	if(eRouteType == NO_ROUTE)
	{
		return FALSE;
	}
	if(pNewPlot->IsRoutePillaged())
	{
		return FALSE;
	}
#endif

	if(!pNewPlot->IsFriendlyTerritory(ePlayer))
	{
		PlayerTypes ePlotOwnerPlayer = pNewPlot->getOwner();
		if(ePlotOwnerPlayer != NO_PLAYER)
		{
			PlayerTypes eMajorPlayer = NO_PLAYER;
			PlayerTypes eMinorPlayer = NO_PLAYER;
			CvPlayer& kPlotOwner = GET_PLAYER(ePlotOwnerPlayer);
			if(kPlayer.isMinorCiv() && !kPlotOwner.isMinorCiv())
			{
				eMajorPlayer = ePlotOwnerPlayer;
				eMinorPlayer = ePlayer;
			}
			else if(kPlotOwner.isMinorCiv() && !kPlayer.isMinorCiv())
			{
				eMajorPlayer = ePlayer;
				eMinorPlayer = ePlotOwnerPlayer;
			}
			else
			{
				return FALSE;
			}

			if(!GET_PLAYER(eMinorPlayer).GetMinorCivAI()->IsActiveQuestForPlayer(eMajorPlayer, MINOR_CIV_QUEST_ROUTE))
			{
				return FALSE;
			}
		}
	}

	if(finder->GetInfo() & MOVE_ANY_ROUTE)
	{
#if defined(MOD_EVENTS_CITY_CONNECTIONS)
		// Cities always have the best route, which permits "harbour to harbour" connections before The Wheel
		if(pNewPlot->isCity())
		{
			return TRUE;
		}
#endif

		// if the player can't build
		if(kPlayer.getBestRoute() == NO_ROUTE)
		{
			return FALSE;
		}

		if(eRouteType != NO_ROUTE)
		{
			return TRUE;
		}
	}
	else
	{
		int iRoute = iFlags & 0xFF00;
		iRoute = iRoute >> 8;
		iRoute = iRoute - 1;
		RouteTypes eRequiredRoute = (RouteTypes)(iRoute);
		if(eRouteType == eRequiredRoute)
		{
			return TRUE;
		}
	}

	return FALSE;
}

//	---------------------------------------------------------------------------
// Route - find the number of additional children. In this case, the node is at a city, push all other cities that the city has a water connection to
// This function does not require the global Tactical Analysis Map.
int RouteGetNumExtraChildren(const CvAStarNode* node, const CvAStar* finder)
{
	PlayerTypes ePlayer = ((PlayerTypes)(finder->GetInfo() & 0xFF));
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	TeamTypes eTeam = kPlayer.getTeam();
	CvPlot* pPlot = GC.getMap().plotCheckInvalid(node->m_iX, node->m_iY);

	if(!pPlot)
	{
		return 0;
	}

	// slewis - don't allow the minor civ to use harbors
	if(kPlayer.isMinorCiv())
	{
		return 0;
	}

	CvCityConnections* pCityConnections = kPlayer.GetCityConnections();
	if(pCityConnections->IsEmpty())
	{
		return 0;
	}

	int iResultNum = 0;

	CvCity* pCity = pPlot->getPlotCity();

	// if there isn't a city there or the city isn't on our team
	if(!pCity || pCity->getTeam() != eTeam)
	{
		return 0;
	}

	uint uiFirstCityIndex = pCityConnections->GetIndexFromCity(pCity);
	if(uiFirstCityIndex >= pCityConnections->m_aiCityPlotIDs.size())
	{
		CvAssertMsg(false, "City index out of bounds");
		return 0;
	}

	for(uint uiSecondCityIndex = 0; uiSecondCityIndex < pCityConnections->m_aiCityPlotIDs.size(); uiSecondCityIndex++)
	{
		if(uiFirstCityIndex == uiSecondCityIndex)
		{
			continue;
		}

		CvCityConnections::RouteInfo* pRouteInfo = pCityConnections->GetRouteInfo(uiFirstCityIndex, uiSecondCityIndex);
		if(!pRouteInfo)
		{
			continue;
		}

		// get the two cities
		CvCity* pFirstCity  = pCityConnections->GetCityFromIndex(uiFirstCityIndex);
		CvCity* pSecondCity = pCityConnections->GetCityFromIndex(uiSecondCityIndex);

		if(!pFirstCity || !pSecondCity)
		{
			continue;
		}

#if defined(MOD_EVENTS_CITY_CONNECTIONS)
		if(pRouteInfo->m_cRouteState & CvCityConnections::HAS_INDIRECT_ROUTE)
#else
		if(pRouteInfo->m_cRouteState & CvCityConnections::HAS_WATER_ROUTE)
#endif
		{
			iResultNum++;
		}
	}

	return iResultNum;
}

//	--------------------------------------------------------------------------------
/// Water route valid finder - check the validity of a coordinate
int WaterRouteValid(const CvAStarNode* parent, const CvAStarNode* node, int, const void*, const CvAStar* finder)
{
	CvPlot* pNewPlot;

	if(parent == NULL)
	{
		return TRUE;
	}

	PlayerTypes ePlayer = (PlayerTypes)(finder->GetInfo() & 0xFF);
	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	pNewPlot = GC.getMap().plotUnchecked(node->m_iX, node->m_iY);

	if(!(pNewPlot->isRevealed(eTeam)))
	{
		return FALSE;
	}

	CvCity* pCity = pNewPlot->getPlotCity();
	if(pCity && pCity->getTeam() == eTeam)
	{
		return TRUE;
	}

	if(pNewPlot->isWater())
	{
		return TRUE;
	}

	return FALSE;
}

//	--------------------------------------------------------------------------------
/// Build route cost
int BuildRouteCost(const CvAStarNode* /*parent*/, CvAStarNode* node, int, const void*, const CvAStar* finder)
{
	CvPlot* pPlot = GC.getMap().plotUnchecked(node->m_iX, node->m_iY);
	int iFlags = finder->GetInfo();
	PlayerTypes ePlayer = (PlayerTypes)(iFlags & 0xFF);

	int iRoute = iFlags & 0xFF00;
	iRoute = iRoute >> 8;
	iRoute = iRoute - 1;
	RouteTypes eRoute = (RouteTypes)(iRoute);

	if(pPlot->getRouteType() != NO_ROUTE)
	{
		if(pPlot->getRouteType() == eRoute)
			return 1;
		else
			return PATH_BUILD_ROUTE_EXISTING_ROUTE_WEIGHT;
	}

	int iMaxValue = 1500;

	// if the plot is on a removable feature, it tends to be a good idea to build a road here
	int iMovementCost = ((pPlot->getFeatureType() == NO_FEATURE) ? GC.getTerrainInfo(pPlot->getTerrainType())->getMovementCost() : GC.getFeatureInfo(pPlot->getFeatureType())->getMovementCost());

	// calculate the max value based on how much of a movement increase we get
	if(iMovementCost + 1 != 0)
	{
		iMaxValue = iMaxValue / 2 + iMaxValue / (iMovementCost + 1);
	}

	// if the tile already been tagged for building a road, then provide a discount
	if(pPlot->GetBuilderAIScratchPadTurn() == GC.getGame().getGameTurn() && pPlot->GetBuilderAIScratchPadPlayer() == ePlayer)
	{
		iMaxValue = (int)(iMaxValue * PATH_BUILD_ROUTE_ALREADY_FLAGGED_DISCOUNT);
	}

	return iMaxValue;
}

//	--------------------------------------------------------------------------------
/// Build Route path finder - check validity of a coordinate
int BuildRouteValid(const CvAStarNode* parent, const CvAStarNode* node, int, const void*, const CvAStar* finder)
{
	CvPlot* pNewPlot;

	if(parent == NULL)
	{
		return TRUE;
	}

	int iFlags = finder->GetInfo();
	PlayerTypes ePlayer = (PlayerTypes)(iFlags & 0xFF);

	CvPlayer& thisPlayer = GET_PLAYER(ePlayer);
	bool bThisPlayerIsMinor = thisPlayer.isMinorCiv();

	pNewPlot = GC.getMap().plotUnchecked(node->m_iX, node->m_iY);
	if(!bThisPlayerIsMinor && !(pNewPlot->isRevealed(thisPlayer.getTeam())))
	{
		return FALSE;
	}

	if(pNewPlot->isWater())
	{
		return FALSE;
	}
#if defined(MOD_BALANCE_CORE)
	if(pNewPlot->isImpassable(thisPlayer.getTeam()) || (pNewPlot->isMountain() && !thisPlayer.GetPlayerTraits()->IsMountainPass()))
#else
	if(pNewPlot->isImpassable() || pNewPlot->isMountain())
#endif
	{
		return FALSE;
	}

	PlayerTypes ePlotOwnerPlayer = pNewPlot->getOwner();
	if(ePlotOwnerPlayer != NO_PLAYER && !pNewPlot->IsFriendlyTerritory(ePlayer))
	{
		PlayerTypes eMajorPlayer = NO_PLAYER;
		PlayerTypes eMinorPlayer = NO_PLAYER;
		bool bPlotOwnerIsMinor = GET_PLAYER(ePlotOwnerPlayer).isMinorCiv();
		if(bThisPlayerIsMinor && !bPlotOwnerIsMinor)
		{
			eMajorPlayer = ePlotOwnerPlayer;
			eMinorPlayer = ePlayer;
		}
		else if(bPlotOwnerIsMinor && !bThisPlayerIsMinor)
		{
			eMajorPlayer = ePlayer;
			eMinorPlayer = ePlotOwnerPlayer;
		}
		else
		{
			return FALSE;
		}

		if(!GET_PLAYER(eMinorPlayer).GetMinorCivAI()->IsActiveQuestForPlayer(eMajorPlayer, MINOR_CIV_QUEST_ROUTE))
		{
			return FALSE;
		}
	}

	return TRUE;
}


//	--------------------------------------------------------------------------------
/// Area path finder - check validity of a coordinate
int AreaValid(const CvAStarNode* parent, const CvAStarNode* node, int, const void*, const CvAStar*)
{
	if(parent == NULL)
	{
		return TRUE;
	}

	CvMap& kMap = GC.getMap();

	//this is independent of any team!
	if(kMap.plotUnchecked(parent->m_iX, parent->m_iY)->isImpassable() != kMap.plotUnchecked(node->m_iX, node->m_iY)->isImpassable())
	{
		return FALSE;
	}

	return ((kMap.plotUnchecked(parent->m_iX, parent->m_iY)->isWater() == kMap.plotUnchecked(node->m_iX, node->m_iY)->isWater()) ? TRUE : FALSE);
}


//	--------------------------------------------------------------------------------
/// Area path finder - callback routine when node added to open/closed list
int JoinArea(CvAStarNode*, CvAStarNode* node, int data, const void*, CvAStar* finder)
{
	if(data == ASNL_ADDCLOSED)
	{
		GC.getMap().plotUnchecked(node->m_iX, node->m_iY)->setArea(finder->GetInfo());
	}

	return 1;
}


//	--------------------------------------------------------------------------------
/// Area path finder - check validity of a coordinate
int LandmassValid(const CvAStarNode* parent, const CvAStarNode* node, int, const void*, const CvAStar*)
{
	if(parent == NULL)
	{
		return TRUE;
	}

	CvMap& kMap = GC.getMap();
	return ((kMap.plotUnchecked(parent->m_iX, parent->m_iY)->isWater() == kMap.plotUnchecked(node->m_iX, node->m_iY)->isWater()) ? TRUE : FALSE);
}


//	--------------------------------------------------------------------------------
/// Area path finder - callback routine when node added to open/closed list
int JoinLandmass(CvAStarNode*, CvAStarNode* node, int data, const void*, CvAStar* finder)
{
	if(data == ASNL_ADDCLOSED)
	{
		GC.getMap().plotUnchecked(node->m_iX, node->m_iY)->setLandmass(finder->GetInfo());
	}

	return 1;
}


// DERIVED CLASSES (which have more convenient ways to access our various pathfinders)

//	--------------------------------------------------------------------------------
/// Constructor
CvTwoLayerPathFinder::CvTwoLayerPathFinder()
{
	CvAStar::CvAStar();
	m_ppaaPartialMoveNodes = NULL;

#if defined(MOD_BALANCE_CORE)
	//for debugging
	m_strName = "TwoLayerAStar";
#endif
}

//	--------------------------------------------------------------------------------
/// Destructor
CvTwoLayerPathFinder::~CvTwoLayerPathFinder()
{
	CvAStar::DeInit();

	DeInit();
}

//	--------------------------------------------------------------------------------
/// Allocate memory, zero variables
void CvTwoLayerPathFinder::Initialize(int iColumns, int iRows, bool bWrapX, bool bWrapY, CvAPointFunc IsPathDestFunc, CvAPointFunc DestValidFunc, CvAHeuristic HeuristicFunc, CvAStarConst1Func CostFunc, CvAStarConst2Func ValidFunc, CvAStarFunc NotifyChildFunc, CvAStarFunc NotifyListFunc, CvABegin InitializeFunc, CvAEnd UninitializeFunc, const void* pData)
{
	int iI, iJ;

	DeInit();

	CvAStar::Initialize(iColumns, iRows, bWrapX, bWrapY, IsPathDestFunc, DestValidFunc, HeuristicFunc, CostFunc, ValidFunc, NotifyChildFunc, NotifyListFunc, NULL, NULL, InitializeFunc, UninitializeFunc, pData);

	m_ppaaPartialMoveNodes = FNEW(CvAStarNode*[m_iColumns], c_eCiv5GameplayDLL, 0);
	for(iI = 0; iI < m_iColumns; iI++)
	{
		m_ppaaPartialMoveNodes[iI] = FNEW(CvAStarNode[m_iRows], c_eCiv5GameplayDLL, 0);
		for(iJ = 0; iJ < m_iRows; iJ++)
		{
			m_ppaaPartialMoveNodes[iI][iJ].m_iX = iI;
			m_ppaaPartialMoveNodes[iI][iJ].m_iY = iJ;
		}
	}
#if defined(MOD_BALANCE_CORE)
	CvAStarNode** apNeighbors = m_ppaaNeighbors;
	for(iI = 0; iI < m_iColumns; iI++)
		for(iJ = 0; iJ < m_iRows; iJ++)
		{
			//neighbors have already been precalculated in base class
			m_ppaaPartialMoveNodes[iI][iJ].m_apNeighbors = apNeighbors;
			apNeighbors += 6;
		}
#endif
};

//	--------------------------------------------------------------------------------
/// Frees allocated memory
void CvTwoLayerPathFinder::DeInit()
{
	CvAStar::DeInit();

	if(m_ppaaPartialMoveNodes != NULL)
	{
		for(int iI = 0; iI < m_iColumns; iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaaPartialMoveNodes[iI]);
		}

		SAFE_DELETE_ARRAY(m_ppaaPartialMoveNodes);
	}
}

//	--------------------------------------------------------------------------------
/// Return a node from the second layer of A-star nodes (for the partial moves)
CvAStarNode* CvTwoLayerPathFinder::GetPartialMoveNode(int iCol, int iRow)
{
	return &(m_ppaaPartialMoveNodes[iCol][iRow]);
}

//	--------------------------------------------------------------------------------
/// Return the furthest plot we can get to this turn that is on the path
CvPlot* CvTwoLayerPathFinder::GetPathEndTurnPlot() const
{
	CvAStarNode* pNode;

	pNode = m_pBest;

	if(NULL != pNode)
	{
		if((pNode->m_pParent == NULL) || (pNode->m_iTurns == 1))
		{
			return GC.getMap().plotUnchecked(pNode->m_iX, pNode->m_iY);
		}

		while(pNode->m_pParent != NULL)
		{
			if(pNode->m_pParent->m_iTurns == 1)
			{
				return GC.getMap().plotUnchecked(pNode->m_pParent->m_iX, pNode->m_pParent->m_iY);
			}

			pNode = pNode->m_pParent;
		}
	}

	FAssert(false);

	return NULL;
}

//	--------------------------------------------------------------------------------
// Path logging
static void LogPathGeneration(const CvUnit *pkUnit, CvString& strMsg)
{
	if(GC.getLogging() && GC.getAILogging() && pkUnit)
	{
		CvString strOutBuf;
		CvString strBaseString;

		CvPlayer& kPlayer = GET_PLAYER(pkUnit->getOwner());
		const char* pszPlayerName = kPlayer.getCivilizationShortDescription();
		FILogFile* pLog = LOGFILEMGR.GetLog((gDLL->IsGameCoreThread())?"AStar_GC.log":"AStar_APP.log", FILogFile::kDontTimeStamp, "Game Turn, Player, Unit, From X, From Y, To X, To Y, Info, Checksum");

		// Get the leading info for this line
		strBaseString.Format("%03d, %s, UnitID: %d, ", GC.getGame().getElapsedGameTurns(), (pszPlayerName)?pszPlayerName:"?", pkUnit->GetID());
		strOutBuf = strBaseString + strMsg;
		pLog->Msg(strOutBuf);
	}
}

//	--------------------------------------------------------------------------------
/// Generate a path, using the supplied unit as the data
#ifdef AUI_ASTAR_TURN_LIMITER
bool CvTwoLayerPathFinder::DoesPathExist(CvUnit* pUnit, CvPlot* pStartPlot, CvPlot* pEndPlot, int iFlags, int iTargetTurns)
{
	if(pUnit == NULL || pStartPlot == NULL || pEndPlot == NULL)
	{
		return false;
	}

	return GenerateUnitPath(pUnit, pStartPlot->getX(), pStartPlot->getY(), pEndPlot->getX(), pEndPlot->getY(), iFlags, false, iTargetTurns);
}


bool CvTwoLayerPathFinder::GenerateUnitPath(const CvUnit* pkUnit, int iXstart, int iYstart, int iXdest, int iYdest, int iInfo, bool bReuse, int iTargetTurns)
#else
bool CvTwoLayerPathFinder::GenerateUnitPath(const CvUnit* pkUnit, int iXstart, int iYstart, int iXdest, int iYdest, int iInfo /*= 0*/, bool bReuse /*= false*/)
#endif // AUI_ASTAR_TURN_LIMITER
{
	if (pkUnit)
	{
		CvAssert(gDLL->IsGameCoreThread() || !gDLL->IsGameCoreExecuting());
#ifdef AUI_ASTAR_TURN_LIMITER
		SetData(pkUnit, iTargetTurns);
#else
		SetData(pkUnit);
#endif // AUI_ASTAR_TURN_LIMITER
		bool bResult = GeneratePath(iXstart, iYstart, iXdest, iYdest, iInfo, bReuse);

#if defined(MOD_BALANCE_CORE_DEBUGGING)
		if(false)
		{
			CvString strLogString;
			uint uiChecksum = CRC_INIT;
			// Loop through the nodes and make a checksum
			CvAStarNode* pNode = GetLastNode();
			while(pNode != NULL)
			{
				// Just do the X/Y for now
				uiChecksum = g_CRC32.Calc( &pNode->m_iX, sizeof( pNode->m_iX ), uiChecksum );
				uiChecksum = g_CRC32.Calc( &pNode->m_iY, sizeof( pNode->m_iY ), uiChecksum );
		
				pNode = pNode->m_pParent;
			}

			strLogString.Format("%d, %d, %d, %d, %d, %8x", iXstart, iYstart, iXdest, iYdest, iInfo, uiChecksum);
			LogPathGeneration(pkUnit, strLogString);
		}
#endif

		return bResult;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
// CvStepPathFinder
//////////////////////////////////////////////////////////////////////////

//	--------------------------------------------------------------------------------
/// Get distance between two plots on same land mass (return -1 if plots are in different areas)
int CvStepPathFinder::GetStepDistanceBetweenPoints(PlayerTypes ePlayer, PlayerTypes eEnemy, CvPlot* pStartPlot, CvPlot* pEndPlot)
{
	CvAStarNode* pPathfinderNode;

	if(pStartPlot == NULL || pEndPlot == NULL)
	{
		return -1;
	}

	SetData(&eEnemy);
	bool bPathfinderSuccess = GeneratePath(pStartPlot->getX(), pStartPlot->getY(), pEndPlot->getX(), pEndPlot->getY(), ePlayer, false);
	if(bPathfinderSuccess)
	{
		pPathfinderNode = GetLastNode();

		if(pPathfinderNode != NULL)
		{
			return pPathfinderNode->m_iTurns;
		}
	}

	return -1;
}

//	--------------------------------------------------------------------------------
/// Check for existence of step path between two points
bool CvStepPathFinder::DoesPathExist(PlayerTypes ePlayer, PlayerTypes eEnemy, bool bWidePath, CvPlot* pStartPlot, CvPlot* pEndPlot)
{
	if(pStartPlot == NULL || pEndPlot == NULL || pStartPlot->getArea() != pEndPlot->getArea())
	{
		return false;
	}

	SetData(&eEnemy);
	SetValidFunc(bWidePath ? StepValidWide : StepValid);

	bool bResult = GeneratePath(pStartPlot->getX(), pStartPlot->getY(), pEndPlot->getX(), pEndPlot->getY(), ePlayer, false);

	SetValidFunc( StepValid );
	return bResult;
}

//	--------------------------------------------------------------------------------
/// Returns the last plot along the step path owned by a specific player
CvPlot* CvStepPathFinder::GetLastOwnedPlot(PlayerTypes ePlayer, PlayerTypes eEnemy, CvPlot* pStartPlot, CvPlot* pEndPlot) const
{
	CvAStarNode* pNode;
	int iNumSteps;

	// Generate step path
	iNumSteps = GC.getStepFinder().GetStepDistanceBetweenPoints(ePlayer, eEnemy, pStartPlot, pEndPlot);
	if(iNumSteps != -1)
	{
		pNode = GC.getStepFinder().GetLastNode();

		// Starting at the end, loop until we find a plot from this owner
		CvMap& kMap = GC.getMap();
		while(pNode != NULL)
		{
			CvPlot* currentPlot;
			currentPlot = kMap.plotUnchecked(pNode->m_iX, pNode->m_iY);

#if defined(MOD_BALANCE_CORE_SANE_IMPASSABILITY)
			if(currentPlot->isImpassable())
			{
				continue;
			}
#endif
			// Check and see if this plot has the right owner
			if(currentPlot->getOwner() == ePlayer)
			{
				return currentPlot;
			}

			// Move to the previous plot on the path
			pNode = pNode->m_pParent;
		}
	}

	return NULL;
}

//	--------------------------------------------------------------------------------
/// Get the plot X from the end of the step path
CvPlot* CvStepPathFinder::GetXPlotsFromEnd(PlayerTypes ePlayer, PlayerTypes eEnemy, CvPlot* pStartPlot, CvPlot* pEndPlot, int iPlotsFromEnd, bool bLeaveEnemyTerritory) const
{
	CvAStarNode* pNode;
	CvPlot* currentPlot = NULL;
	int iNumSteps;
	int iPathLen;

	// Generate step path
	iPathLen = GC.getStepFinder().GetStepDistanceBetweenPoints(ePlayer, eEnemy, pStartPlot, pEndPlot);
	iNumSteps = ::min(iPlotsFromEnd, iPathLen);

	if(iNumSteps != -1)
	{
		pNode = GC.getStepFinder().GetLastNode();

		if(pNode != NULL)
		{
			// Starting at the end, loop the correct number of times back
			for(int i = 0; i < iNumSteps; i++)
			{
				if(pNode->m_pParent != NULL)
				{
					// Move to the previous plot on the path
					pNode = pNode->m_pParent;
				}
			}

			CvMap& kMap = GC.getMap();
			currentPlot = kMap.plotUnchecked(pNode->m_iX, pNode->m_iY);

			// Was an enemy specified and we don't want this plot to be in enemy territory?
			if (eEnemy != NO_PLAYER && bLeaveEnemyTerritory && currentPlot)
			{
				// Loop until we leave enemy territory
				for (int i = 0; i < (iPathLen - iNumSteps) && currentPlot->getOwner() == eEnemy; i++)
				{
					if (pNode->m_pParent != NULL)
					{
						// Move to the previous plot on the path
						pNode = pNode->m_pParent;
						currentPlot = kMap.plotUnchecked(pNode->m_iX, pNode->m_iY);
#if defined(MOD_BALANCE_CORE)
						CvPlot* pAdjacentPlot = NULL;
						if(currentPlot != NULL)
						{
							if(currentPlot->isImpassable(GET_PLAYER(ePlayer).getTeam()))
							{
								// Find an adjacent tile
								for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
								{
									pAdjacentPlot = plotDirection(currentPlot->getX(), currentPlot->getY(), ((DirectionTypes)iDirectionLoop));
									if(pAdjacentPlot != NULL && !pAdjacentPlot->isImpassable(GET_PLAYER(ePlayer).getTeam()))
									{
										currentPlot = pAdjacentPlot;
										break;
									}
								}
							}
						}
#endif
					}
					else
					{
						break;
					}
				}
			}
		}
	}

	return currentPlot;
}

#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
/// Returns the last plot along the step path owned by a specific player
int CvStepPathFinder::CountPlotsOwnedByXInPath(PlayerTypes ePlayer) const
{
	int iCount = 0;
	CvAStarNode* pNode = GC.getStepFinder().GetLastNode();

	// Starting at the end, loop until we find a plot from this owner
	CvMap& kMap = GC.getMap();
	while(pNode != NULL)
	{
		CvPlot* currentPlot;
		currentPlot = kMap.plotUnchecked(pNode->m_iX, pNode->m_iY);

		// Check and see if this plot has the right owner
		if(currentPlot->getOwner() == ePlayer)
			iCount++;

		// Move to the previous plot on the path
		pNode = pNode->m_pParent;
	}

	return iCount;
}
//	--------------------------------------------------------------------------------
/// Returns the last plot along the step path owned by a specific player
int CvStepPathFinder::CountPlotsOwnedAnyoneInPath(PlayerTypes ePlayer) const
{
	int iCount = 0;
	CvAStarNode* pNode = GC.getStepFinder().GetLastNode();

	// Starting at the end, loop until we find a plot from this owner
	CvMap& kMap = GC.getMap();
	while(pNode != NULL)
	{
		CvPlot* currentPlot;
		currentPlot = kMap.plotUnchecked(pNode->m_iX, pNode->m_iY);

		// Check and see if this plot has an owner that isn't us.
		if(currentPlot->getOwner() != ePlayer && currentPlot->getOwner() != NO_PLAYER)
			iCount++;

		// Move to the previous plot on the path
		pNode = pNode->m_pParent;
	}

	return iCount;
}
#endif


#ifdef AUI_ASTAR_TURN_LIMITER
//	--------------------------------------------------------------------------------
/// Check for existence of step path between two points
bool CvIgnoreUnitsPathFinder::DoesPathExist(CvUnit& unit, CvPlot* pStartPlot, CvPlot* pEndPlot, int iTargetTurns)
#else
bool CvIgnoreUnitsPathFinder::DoesPathExist(CvUnit& unit, CvPlot* pStartPlot, CvPlot* pEndPlot)
#endif
{
	if(pStartPlot == NULL || pEndPlot == NULL)
	{
		return false;
	}

#ifdef AUI_ASTAR_TURN_LIMITER
	SetData(&unit, iTargetTurns);
#else
	SetData(&unit);
#endif // AUI_ASTAR_TURN_LIMITER

	return GeneratePath(pStartPlot->getX(), pStartPlot->getY(), pEndPlot->getX(), pEndPlot->getY(), 0, true /*bReuse*/);
}

//	--------------------------------------------------------------------------------
/// Get length of last path computed by path finder in turns [should be used after a call to DoesPathExist()]
int CvIgnoreUnitsPathFinder::GetPathLength()
{
	int iPathDistance = MAX_INT;

	CvAStarNode* pNode = GetLastNode();
	if(pNode != NULL)
	{
		iPathDistance = pNode->m_iTurns;
	}

	return iPathDistance;
}

//	--------------------------------------------------------------------------------
/// Returns the last plot along the step path owned by a specific player
CvPlot* CvIgnoreUnitsPathFinder::GetLastOwnedPlot(CvPlot* pStartPlot, CvPlot* pEndPlot, PlayerTypes iOwner)
{
	// Generate path
	if(GeneratePath(pStartPlot->getX(), pStartPlot->getX(), pEndPlot->getX(), pEndPlot->getX(), 0, false))
	{
		CvAStarNode* pNode = GetLastNode();

		// Starting at the end, loop until we find a plot from this owner
		CvMap& kMap = GC.getMap();
		while(pNode != NULL)
		{
			CvPlot* currentPlot;
			currentPlot = kMap.plotUnchecked(pNode->m_iX, pNode->m_iY);

			// Check and see if this plot has the right owner
			if(currentPlot->getOwner() == iOwner)
			{
				return currentPlot;
			}

			// Move to the previous plot on the path
			pNode = pNode->m_pParent;
		}
	}

	return NULL;
}

//	--------------------------------------------------------------------------------
/// Retrieve first node of path
CvPlot* CvIgnoreUnitsPathFinder::GetPathFirstPlot() const
{
	CvAStarNode* pNode = GetLastNode();

	CvMap& kMap = GC.getMap();
	if(pNode->m_pParent == NULL)
	{
		return kMap.plotUnchecked(pNode->m_iX, pNode->m_iY);
	}

	while(pNode != NULL)
	{
		if(pNode->m_pParent->m_pParent == NULL)
		{
			return kMap.plotUnchecked(pNode->m_iX, pNode->m_iY);
		}

		pNode = pNode->m_pParent;
	}

	FAssert(false);

	return NULL;
}

//	--------------------------------------------------------------------------------
/// Return the furthest plot we can get to this turn that is on the path
CvPlot* CvIgnoreUnitsPathFinder::GetPathEndTurnPlot() const
{
	CvAStarNode* pNode = GetLastNode();

	if(NULL != pNode)
	{
		CvMap& kMap = GC.getMap();
		if((pNode->m_pParent == NULL) || (pNode->m_iTurns == 1))
		{
			return kMap.plotUnchecked(pNode->m_iX, pNode->m_iY);
		}

		while(pNode->m_pParent != NULL)
		{
			if(pNode->m_pParent->m_iTurns == 1)
			{
				return kMap.plotUnchecked(pNode->m_pParent->m_iX, pNode->m_pParent->m_iY);
			}

			pNode = pNode->m_pParent;
		}
	}

	FAssert(false);

	return NULL;
}

//	--------------------------------------------------------------------------------
/// UI path finder - check validity of a coordinate
int UIPathValid(const CvAStarNode* parent, const CvAStarNode* node, int data, const void* pointer, const CvAStar* finder)
{
	if (parent == NULL)
	{
		return TRUE;
	}

	if(node->m_iTurns > 3)
	{
		return FALSE;
	}

	CvPlot* pToPlot = GC.getMap().plotUnchecked(node->m_iX, node->m_iY);

	CvUnit* pUnit = ((CvUnit*)pointer);

	if(!pToPlot->isRevealed(pUnit->getTeam()))
	{
		if(pUnit->getNoRevealMapCount() > 0)
		{
			return FALSE;
		}
	}

	if(pToPlot->isVisible(pUnit->getTeam()) && pToPlot->isVisibleEnemyUnit(pUnit))
	{
		if (!pUnit->canMoveInto(*pToPlot, CvUnit::MOVEFLAG_ATTACK))
			return FALSE;
	}

	if(pUnit->getDomainType() == DOMAIN_LAND)
	{
		int iGroupAreaID = pUnit->getArea();
		if(pToPlot->getArea() != iGroupAreaID)
		{
			if(!(pToPlot->isAdjacentToArea(iGroupAreaID)))
			{
				// antonjs: Added for Smoky Skies scenario. Allows move range to show correctly for airships,
				// which move over land and sea plots equally (canMoveAllTerrain)
				if (!pUnit->canMoveAllTerrain())
				{
#if defined(MOD_BUGFIX_HOVERING_PATHFINDER)
					if (!(pUnit->IsHoveringUnit() && (pToPlot->isShallowWater() || pToPlot->getFeatureType() == FEATURE_ICE))) {
						return FALSE;
					}
#else
					return FALSE;
#endif
				}
			}
		}
	}

	if(!pUnit->canEnterTerrain(*pToPlot, CvUnit::MOVEFLAG_ATTACK))
	{
		return FALSE;
	}

	if(!PathValid(parent,node,data,pointer,finder))
	{
		return FALSE;
	}

	return TRUE;
}

//	--------------------------------------------------------------------------------
/// UI path finder - add a new path and send out a message
int UIPathAdd(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
	PathAdd(parent, node, data, pointer, finder);
	if(node)
	{
		if(node->m_iTurns < 2 /*&& node->m_eCvAStarListType == NO_CVASTARLIST*/)
		{
			CvPlot* pPlot = GC.getMap().plot(node->m_iX, node->m_iY);
			if(pPlot)
			{
				auto_ptr<ICvPlot1> pDllPlot = GC.WrapPlotPointer(pPlot);
				GC.GetEngineUserInterface()->AddHexToUIRange(pDllPlot.get());
			}
		}
	}

	return 1;
}

//	--------------------------------------------------------------------------------
int AttackPathAdd(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
	PathAdd(parent, node, data, pointer, finder);
	if(node && node->m_iTurns < 2)
	{
		CvPlot* pPlot = GC.getMap().plot(node->m_iX, node->m_iY);

		auto_ptr<ICvUnit1> pDllUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
		CvUnit* pUnit = GC.UnwrapUnitPointer(pDllUnit.get());
		CvAssertMsg(pUnit, "pUnit should be a value");

		if(pUnit && pPlot)
		{
			if(pPlot->isVisible(pUnit->getTeam()) && (pPlot->isVisibleEnemyUnit(pUnit) || pPlot->isEnemyCity(*pUnit)))
			{
				auto_ptr<ICvPlot1> pDllPlot = GC.WrapPlotPointer(pPlot);
				GC.GetEngineUserInterface()->AddHexToUIRange(pDllPlot.get());
			}
		}
	}

	return 1;
}

//	--------------------------------------------------------------------------------
int AttackPathDestEval(int iToX, int iToY, const void* pointer, const CvAStar* finder, bool bOnlyFortified, bool bOnlyCity)
{
	CvUnit* pUnit = ((CvUnit*)pointer);
	CvAssertMsg(pUnit, "pUnit should be a value");
	CvPlot* pPlot = GC.getMap().plot(iToX, iToY);
	CvAStarNode* pNode = finder->GetLastNode();

	if(pPlot->isVisible(pUnit->getTeam()) && (pPlot->isVisibleEnemyUnit(pUnit) || pPlot->isEnemyCity(*pUnit)) && pNode && pNode->m_iTurns < 2)
	{
		if (pUnit->canMoveInto(*pPlot, CvUnit::MOVEFLAG_ATTACK))
		{
			if(bOnlyFortified)
			{
				CvUnit* pEnemyUnit = pPlot->getVisibleEnemyDefender(pUnit->getOwner());
				if(pEnemyUnit && pEnemyUnit->IsFortifiedThisTurn())
				{
					return TRUE;
				}
			}
			else if(bOnlyCity)
			{
				if(pPlot->isEnemyCity(*pUnit))
				{
					return TRUE;
				}
			}
			else
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

//	--------------------------------------------------------------------------------
/// Destination is valid if there is an enemy unit there
int AttackPathDest(int iToX, int iToY, const void* pointer, const CvAStar* finder)
{
	return AttackPathDestEval(iToX, iToY, pointer, finder, false, false);
}

//	--------------------------------------------------------------------------------
/// Destination is valid if there is a fortified unit there
int AttackFortifiedPathDest(int iToX, int iToY, const void* pointer, const CvAStar* finder)
{
	return AttackPathDestEval(iToX, iToY, pointer, finder, true, false);
}

//	--------------------------------------------------------------------------------
/// Destination is valid if there is a city there
int AttackCityPathDest(int iToX, int iToY, const void* pointer, const CvAStar* finder)
{
	return AttackPathDestEval(iToX, iToY, pointer, finder, false, true);
}

#if defined(MOD_CORE_PATHFINDER)
//	---------------------------------------------------------------------------
int RebaseValid(const CvAStarNode* parent, const CvAStarNode* node, int, const void* pointer, const CvAStar*)
{
	if(parent == NULL)
		return TRUE;

	CvPlot* pNewPlot = GC.getMap().plotUnchecked(node->m_iX, node->m_iY);

	CvUnit* pUnit = ((CvUnit*)pointer);
	if (!pUnit)
		return FALSE;

	//distance
	if (plotDistance(node->m_iX, node->m_iY, parent->m_iX, parent->m_iY) > pUnit->GetRange())
		return FALSE;

	//capacity
	if (pNewPlot->isCity() && pNewPlot->getPlotCity()->getOwner()==pUnit->getOwner())
	{
		int iUnitsThere = pNewPlot->countNumAirUnits( GET_PLAYER(pUnit->getOwner()).getTeam() );
		if (iUnitsThere < pNewPlot->getPlotCity()->GetMaxAirUnits())
			return TRUE;
	}
	else
	{
		IDInfo* pUnitNode = pNewPlot->headUnitNode();

		// Loop through all units on this plot
		while(pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(*pUnitNode);
			pUnitNode = pNewPlot->nextUnitNode(pUnitNode);
			
			if (pUnit->canLoad(*(pLoopUnit->plot())))
				return TRUE;
		}
	}

	return FALSE;
}

//	---------------------------------------------------------------------------
int RebaseGetNumExtraChildren(const CvAStarNode* node, const CvAStar* finder)
{
	PlayerTypes ePlayer = ((PlayerTypes)(finder->GetInfo() & 0xFF));
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	TeamTypes eTeam = kPlayer.getTeam();
	CvPlot* pPlot = GC.getMap().plotCheckInvalid(node->m_iX, node->m_iY);

	if(!pPlot)
	{
		return 0;
	}

	CvCity* pCity = pPlot->getPlotCity();

	//might be a carrier outside of a city
	if (!pCity)
		pCity = kPlayer.GetClosestCity(pPlot);

	// if there is a city and the city is on our team
	if(pCity && pCity->getTeam()==eTeam)
	{
		std::vector<int> vNeighbors = pCity->GetClosestFriendlyNeighboringCities();
		std::vector<int> vAttachedUnits = pCity->GetAttachedUnits();

		return (int)vNeighbors.size()+vAttachedUnits.size();
	}

	return 0;
}

//	---------------------------------------------------------------------------
int RebaseGetExtraChild(const CvAStarNode* node, int iIndex, int& iX, int& iY, const CvAStar* finder)
{
	iX = -1;
	iY = -1;

	PlayerTypes ePlayer = ((PlayerTypes)(finder->GetInfo() & 0xFF));
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	TeamTypes eTeam = kPlayer.getTeam();
	CvPlot* pPlot = GC.getMap().plotCheckInvalid(node->m_iX, node->m_iY);

	if(!pPlot || iIndex<0)
	{
		return 0;
	}

	CvCity* pCity = pPlot->getPlotCity();

	//might be a carrier outside of a city
	if (!pCity)
		pCity = kPlayer.GetClosestCity(pPlot);

	// if there is a city and the city is on our team
	if(pCity && pCity->getTeam()==eTeam)
	{
		std::vector<int> vNeighbors = pCity->GetClosestFriendlyNeighboringCities();
		std::vector<int> vAttachedUnits = pCity->GetAttachedUnits();

		if ( (size_t)iIndex<vNeighbors.size())
		{
			CvCity* pSecondCity = kPlayer.getCity(vNeighbors[iIndex]);
			if (pSecondCity)
			{
				iX = pSecondCity->getX();
				iY = pSecondCity->getY();
				return 1;
			}
		}
		else if ( (size_t)iIndex<vNeighbors.size()+vAttachedUnits.size() )
		{
			CvUnit* pCarrier = kPlayer.getUnit(vAttachedUnits[iIndex-vNeighbors.size()]);
			if (pCarrier)
			{
				iX = pCarrier->plot()->getX();
				iY = pCarrier->plot()->getY();
				return 1;
			}
		}
	}

	return 0;
}
#endif

//	---------------------------------------------------------------------------
int FindValidDestinationDest(int iToX, int iToY, const void* pointer, const CvAStar*)
{
	CvUnit* pUnit = ((CvUnit*)pointer);
	CvPlot* pToPlot = GC.getMap().plotUnchecked(iToX, iToY);

#if defined(MOD_GLOBAL_STACKING_RULES)
	if(pToPlot->getNumFriendlyUnitsOfType(pUnit) >= pToPlot->getUnitLimit())
#else
	if(pToPlot->getNumFriendlyUnitsOfType(pUnit) >= GC.getPLOT_UNIT_LIMIT())
#endif
	{
		return false;
	}

	if(pToPlot->getNumVisibleEnemyDefenders(pUnit) > 0)
	{
		return false;
	}

	// can't capture the unit with a non-combat unit
	if(!pUnit->IsCombatUnit() && pToPlot->isVisibleEnemyUnit(pUnit))
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
int FindValidDestinationPathValid(const CvAStarNode* parent, const CvAStarNode* node, int data, const void* pointer, const CvAStar* finder)
{
	if (node->m_iTurns > 3)
	{
		return FALSE;
	}

	CvUnit* pUnit = ((CvUnit*)pointer);
	CvPlot* pToPlot = GC.getMap().plotUnchecked(node->m_iX, node->m_iY);

	if(!pUnit->canEnterTerrain(*pToPlot, CvUnit::MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE))
	{
		return FALSE;
	}

	if(!PathValid(parent,node,data,pointer,finder))
	{
		return FALSE;
	}

	return TRUE;
}


//	--------------------------------------------------------------------------------
/// Can a unit reach this destination in "X" turns of movement? (pass in 0 if need to make it in 1 turn with movement left)
bool CanReachInXTurns(UnitHandle pUnit, CvPlot* pTarget, int iTurns, bool bIgnoreUnits, int* piTurns /* = NULL */)
{
#if defined(MOD_BALANCE_CORE)
	if (!pUnit || !pTarget)
		return false;

	int iTurnsCalculated = TurnsToReachTarget(pUnit, pTarget, false /*bReusePaths*/, bIgnoreUnits, false, iTurns);
	if (piTurns)
		*piTurns = iTurnsCalculated;

	return (iTurnsCalculated <= iTurns);
#else
	int iDistance;

	if(!pTarget)
	{
		return false;
	}

	// Compare distance to movement rate
	iDistance = plotDistance(pUnit->getX(), pUnit->getY(), pTarget->getX(), pTarget->getY());
	// KWG: If the unit is a land unit that can embark, baseMoves() is only going to give correct value if the starting and ending locations
	//		are in the same domain (LAND vs. SEA) and no transition occurs.

	if(iTurns == 0 && iDistance >= pUnit->baseMoves())
	{
		return false;
	}
	else if(iTurns > 0 && iDistance > (pUnit->baseMoves() * iTurns))
	{
		return false;
	}
	// Distance not too far, now use pathfinder
	else
	{
		int iTurnsCalculated = TurnsToReachTarget(pUnit, pTarget, false /*bReusePaths*/, bIgnoreUnits);
		if (piTurns)
			*piTurns = iTurnsCalculated;
		return (iTurnsCalculated <= iTurns);
	}
#endif
}

//	--------------------------------------------------------------------------------
/// How many turns will it take a unit to get to a target plot (returns MAX_INT if can't reach at all; returns 0 if makes it in 1 turn and has movement left)
#ifdef AUI_ASTAR_TURN_LIMITER
int TurnsToReachTarget(const UnitHandle pUnit, const CvPlot* pTarget, bool bReusePaths, bool bIgnoreUnits, bool bIgnoreStacking, int iTargetTurns)
#else
int TurnsToReachTarget(UnitHandle pUnit, CvPlot* pTarget, bool bReusePaths, bool bIgnoreUnits, bool bIgnoreStacking)
#endif // AUI_ASTAR_TURN_LIMITER
{

#if defined(MOD_BALANCE_CORE)
	//make sure that iTargetTurns is valid
	iTargetTurns = max(1,iTargetTurns);

	if (iTargetTurns!=INT_MAX)
	{
		//see how far we could move in optimal circumstances
		int	iDistance = plotDistance(pUnit->getX(), pUnit->getY(), pTarget->getX(), pTarget->getY());

		//default range
		int iMoves = pUnit->baseMoves() + pUnit->getExtraMoves();
		int iRange = iMoves * iTargetTurns;

		//catch stupid cases
		if (iRange<=0)
			return pTarget==pUnit->plot() ? INT_MAX : 0;

		//but routes increase it
		if (pUnit->getDomainType()==DOMAIN_LAND && !pUnit->flatMovementCost() )
		{
			CvTeam& kTeam = GET_TEAM(pUnit->getTeam());
			RouteTypes eBestRouteType = kTeam.GetBestPossibleRoute();
			CvRouteInfo* pRouteInfo = GC.getRouteInfo(eBestRouteType);
			if (pRouteInfo &&  (pRouteInfo->getMovementCost() + kTeam.getRouteChange(eBestRouteType) != 0))
			{
				int iMultiplier = GC.getMOVE_DENOMINATOR() / (pRouteInfo->getMovementCost() + kTeam.getRouteChange(eBestRouteType));

				if (pUnit->plot()->getRouteType()!=NO_ROUTE)
					//standing directly on a route
					iRange = iMoves * iTargetTurns * iMultiplier;
				else
					//need to move at least one plot in the first turn at full cost to get to the route - speed optimization for railroad and low turn count
					iRange = 1 + (iMoves-1)*iMultiplier + iMoves*(iTargetTurns-1)*iMultiplier;
			}
		}

		if (iDistance>iRange)
			return INT_MAX;
	}
#endif


	int rtnValue = MAX_INT;
	CvAStarNode* pNode = NULL;

	if(pTarget == pUnit->plot())
	{
		return 0;
	}

	if(pUnit)
	{
#ifdef PATH_FINDER_LOGGING
		CvString strBaseString;
		cvStopWatch kTimer(strBaseString, "Pathfinder.csv");
#endif

		if(bIgnoreUnits)
		{
#ifdef AUI_ASTAR_TURN_LIMITER
			GC.getIgnoreUnitsPathFinder().SetData(pUnit.pointer(), iTargetTurns);
#else
			GC.getIgnoreUnitsPathFinder().SetData(pUnit.pointer());
#endif // AUI_ASTAR_TURN_LIMITER
			if (GC.getIgnoreUnitsPathFinder().GeneratePath(pUnit->getX(), pUnit->getY(), pTarget->getX(), pTarget->getY(), 0, bReusePaths))
			{
				pNode = GC.getIgnoreUnitsPathFinder().GetLastNode();
			}
		}
		else
		{
			int iFlags = MOVE_UNITS_IGNORE_DANGER;
			if(bIgnoreStacking)
			{
				iFlags |= MOVE_IGNORE_STACKING;
			}

			CvAssertMsg(pTarget != NULL, "Passed in a NULL destination to GeneratePath");
			if(pTarget == NULL)
			{
				return false;
			}

#if defined(MOD_CORE_PATHFINDER)
			CvTwoLayerPathFinder& kPathFinder = GC.getPathFinder();
			if (kPathFinder.GenerateUnitPath(pUnit.pointer(), pUnit->getX(), pUnit->getY(), pTarget->getX(), pTarget->getY(), iFlags, bReusePaths, iTargetTurns))
			{
				pNode = GC.getPathFinder().GetLastNode();
			}
#endif
		}

		if(pNode)
		{
			rtnValue = pNode->m_iTurns;
			if(rtnValue == 1)
			{
				if(pNode->m_iMoves > 0)
				{
					rtnValue = 0;
				}
			}
		}

#ifdef PATH_FINDER_LOGGING
		// NOTE: because I'm creating the string after the cvStopWatch, the time it takes to create the string will be in the timer.
		strBaseString.Format("TurnsToReachTarget, Turn %03d, Player: %d, Unit: %d, From X: %d, Y: %d, To X: %d, Y: %d, reuse=%d, ignoreUnits=%d, ignoreStacking=%d, turns=%d", GC.getGame().getElapsedGameTurns(), (int)pUnit->getOwner(), pUnit->GetID(), pUnit->getX(), pUnit->getY(), pTarget->getX(), pTarget->getY(), bReusePaths?1:0, bIgnoreUnits?1:0, bIgnoreStacking?1:0, rtnValue);
		kTimer.SetText(strBaseString);
#endif
	}

	return rtnValue;
}

/// slewis's fault

// A structure holding some unit values that are invariant during a path plan operation
struct TradePathCacheData
{
	CvTeam* m_pTeam;
	bool m_bCanEmbarkAllWaterPassage;
	bool m_bIsRiverTradeRoad;
#if defined(MOD_BALANCE_CORE)
	bool m_bIsMountainPass;
#endif
	bool m_bIsMoveFriendlyWoodsAsRoad;

	inline CvTeam& getTeam() const { return *m_pTeam; }
	inline bool CanEmbarkAllWaterPassage() const { return m_bCanEmbarkAllWaterPassage; }
	inline bool IsRiverTradeRoad() const { return m_bIsRiverTradeRoad; }
	inline bool IsMoveFriendlyWoodsAsRoad() const { return m_bIsMoveFriendlyWoodsAsRoad; }
#if defined(MOD_BALANCE_CORE)
	inline bool IsMountainPass() const { return m_bIsMountainPass; }
#endif
};

//	--------------------------------------------------------------------------------
void TradePathInitialize(const void*, CvAStar* finder)
{
	PlayerTypes ePlayer = (PlayerTypes)(finder->GetInfo() & 0xFF);

	TradePathCacheData* pCacheData = reinterpret_cast<TradePathCacheData*>(finder->GetScratchBufferDirty());

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	TeamTypes eTeam = kPlayer.getTeam();
	pCacheData->m_pTeam = &GET_TEAM(eTeam);
#if defined(MOD_BUGFIX_TRADE_ROUTES_EMBARK_ALL_WATER)
	pCacheData->m_bCanEmbarkAllWaterPassage = pCacheData->m_pTeam->canEmbarkAllWaterPassage() || kPlayer.GetPlayerTraits()->IsEmbarkedAllWater();
#else
	pCacheData->m_bCanEmbarkAllWaterPassage = pCacheData->m_pTeam->canEmbarkAllWaterPassage();
#endif

	CvPlayerTraits* pPlayerTraits = kPlayer.GetPlayerTraits();
	if (pPlayerTraits)
	{
		pCacheData->m_bIsRiverTradeRoad = pPlayerTraits->IsRiverTradeRoad();
		pCacheData->m_bIsMoveFriendlyWoodsAsRoad = pPlayerTraits->IsMoveFriendlyWoodsAsRoad();
#if defined(MOD_BALANCE_CORE)
		pCacheData->m_bIsMountainPass = pPlayerTraits->IsMountainPass();
#endif
	}
	else
	{
		pCacheData->m_bIsRiverTradeRoad = false;
		pCacheData->m_bIsMoveFriendlyWoodsAsRoad = false;
#if defined(MOD_BALANCE_CORE)
		pCacheData->m_bIsMountainPass = false;
#endif
	}

}

//	--------------------------------------------------------------------------------
void TradePathUninitialize(const void*, CvAStar*)
{

}

//	--------------------------------------------------------------------------------
int TradeRouteHeuristic(int iFromX, int iFromY, int iToX, int iToY)
{
#if defined(MOD_CORE_TRADE_NATURAL_ROUTES)
	return plotDistance(iFromX, iFromY, iToX, iToY) * MOD_CORE_TRADE_NATURAL_ROUTES_TILE_BASE_COST/4;
#else
	return plotDistance(iFromX, iFromY, iToX, iToY) * 100;
#endif
}

//	--------------------------------------------------------------------------------
int TradeRouteLandPathCost(const CvAStarNode* parent, CvAStarNode* node, int, const void*, const CvAStar* finder)
{
	CvMap& kMap = GC.getMap();
	int iFromPlotX = parent->m_iX;
	int iFromPlotY = parent->m_iY;
	CvPlot* pFromPlot = kMap.plotUnchecked(iFromPlotX, iFromPlotY);

	int iToPlotX = node->m_iX;
	int iToPlotY = node->m_iY;
	CvPlot* pToPlot = kMap.plotUnchecked(iToPlotX, iToPlotY);

	const TradePathCacheData* pCacheData = reinterpret_cast<const TradePathCacheData*>(finder->GetScratchBuffer());
	FeatureTypes eFeature = pToPlot->getFeatureType();

#if defined(MOD_CORE_TRADE_NATURAL_ROUTES)
	int iCost = MOD_CORE_TRADE_NATURAL_ROUTES_TILE_BASE_COST;

	// super duper low costs for moving along routes - don't check for pillaging
	if (pFromPlot->getRouteType() != NO_ROUTE && pToPlot->getRouteType() != NO_ROUTE)
		iCost = iCost / 4;
	// low costs for moving along rivers
	else if (pFromPlot->isRiver() && pToPlot->isRiver() && !(pFromPlot->isRiverCrossing(directionXY(pFromPlot, pToPlot))))
		iCost = iCost / 2;
	// Iroquios ability
	else if ((eFeature == FEATURE_FOREST || eFeature == FEATURE_JUNGLE) && pCacheData->IsMoveFriendlyWoodsAsRoad())
		iCost = iCost / 2;
	// Mountain pass
	else if (pToPlot->isMountain() && pCacheData->IsMountainPass())
		iCost = iCost / 2;

	//try to avoid these plots
	if (pToPlot->isHills() || eFeature == FEATURE_FOREST || eFeature == FEATURE_JUNGLE || eFeature == FEATURE_ICE)
		iCost += MOD_CORE_TRADE_NATURAL_ROUTES_TILE_BASE_COST/10;

	//prefer oasis
	if (eFeature != FEATURE_OASIS)
		iCost += MOD_CORE_TRADE_NATURAL_ROUTES_TILE_BASE_COST/10;
	
	TeamTypes eToPlotTeam = pToPlot->getTeam();
	if (pCacheData->getTeam().GetID() != eToPlotTeam)
	{
		//try to stick to friendly territory
		if (pToPlot->getOwner()==NO_PLAYER || !pCacheData->getTeam().IsAllowsOpenBordersToTeam(eToPlotTeam))
			iCost += MOD_CORE_TRADE_NATURAL_ROUTES_TILE_BASE_COST/10;
	}

	// avoid enemy lands
	if (eToPlotTeam != NO_TEAM && pCacheData->getTeam().isAtWar(eToPlotTeam))
	{
		iCost += MOD_CORE_TRADE_NATURAL_ROUTES_TILE_BASE_COST*10;
	}
#else
	int iBaseCost = 100;
	int iCost = iBaseCost;

	// super duper low costs for moving along routes
	if (pFromPlot->getRouteType() != NO_ROUTE && pToPlot->getRouteType() != NO_ROUTE)
	{
		iCost = iCost / 2;
	}
	//// super low costs for moving along rivers
	else if (pCacheData->IsRiverTradeRoad() && pFromPlot->isRiver() && pToPlot->isRiver())
	{
		iCost = iCost / 2;
	}
	// Iroquios ability
	else if ((eFeature == FEATURE_FOREST || eFeature == FEATURE_JUNGLE) && pCacheData->IsMoveFriendlyWoodsAsRoad())
	{
		iCost = iCost / 2;
	}
#if defined(MOD_BALANCE_CORE)
	else if (pToPlot->isMountain() && pCacheData->IsMountainPass())
	{
		iCost = iCost / 2;
	}
#endif
	else
	{
		bool bFeaturePenalty = false;
		if (eFeature == FEATURE_FOREST || eFeature == FEATURE_JUNGLE || eFeature == FEATURE_ICE)
		{
			bFeaturePenalty = true;
		}

		if (pToPlot->isHills() || bFeaturePenalty)
		{
			iCost += 1;
		}

		// extra cost for not going to an oasis! (this encourages routes to go through oasis)
		if (eFeature != FEATURE_OASIS)
		{
			iCost += 1;
		}
	}

	if (pToPlot->isWater() && !pToPlot->IsAllowsWalkWater())
	{
		iCost += 1000;
	}
	
	// Penalty for ending a turn on a mountain
#if defined(MOD_BALANCE_CORE_SANE_IMPASSABILITY)
	if(pToPlot->isImpassable())
#else
	if(pToPlot->isImpassable() || pToPlot->isMountain())
#endif
	{
		iCost += 1000;
	}
#endif

	FAssert(iCost != MAX_INT);
	FAssert(iCost > 0);

	return iCost;
}

//	--------------------------------------------------------------------------------
int TradeRouteLandValid(const CvAStarNode* parent, const CvAStarNode* node, int, const void*, const CvAStar*)
{
	if(parent == NULL)
		return TRUE;

	CvMap& kMap = GC.getMap();
	CvPlot* pToPlot = kMap.plotUnchecked(node->m_iX, node->m_iY);
	CvPlot* pFromPlot = kMap.plotUnchecked(parent->m_iX, parent->m_iY);

	if (pToPlot->isWater())
	{
		return FALSE;
	}

	if(pFromPlot->getArea() != pToPlot->getArea())
	{
		return FALSE;
	}

	if (pToPlot->getImprovementType()==(ImprovementTypes)GC.getBARBARIAN_CAMP_IMPROVEMENT())
	{
		return FALSE;
	}

	PlayerTypes eOwner = pToPlot->getOwner();
	if(eOwner != NO_PLAYER)
	{
		if(pToPlot->isImpassable(GET_PLAYER(eOwner).getTeam()) || (pToPlot->isMountain() && !GET_PLAYER(eOwner).GetPlayerTraits()->IsMountainPass()))
		{
			return FALSE;
		}
	}
	else
	{
		if(pToPlot->isMountain() || pToPlot->isImpassable(BARBARIAN_TEAM))
		{
			return FALSE;
		}
	}

	return TRUE;
}

//	--------------------------------------------------------------------------------
/// slewis's fault
int TradeRouteWaterPathCost(const CvAStarNode* parent, CvAStarNode* node, int, const void*, const CvAStar* finder)
{
	CvMap& kMap = GC.getMap();
	const TradePathCacheData* pCacheData = reinterpret_cast<const TradePathCacheData*>(finder->GetScratchBuffer());

	int iFromPlotX = parent->m_iX;
	int iFromPlotY = parent->m_iY;
	CvPlot* pFromPlot = kMap.plotUnchecked(iFromPlotX, iFromPlotY);

	int iToPlotX = node->m_iX;
	int iToPlotY = node->m_iY;
	CvPlot* pToPlot = kMap.plotUnchecked(iToPlotX, iToPlotY);

#if defined(MOD_CORE_TRADE_NATURAL_ROUTES)
	int iCost = 0;

	if (pToPlot->isWater())
	{
		//avoid ocean initially
		if (pToPlot->getTerrainType() != (TerrainTypes)GC.getSHALLOW_WATER_TERRAIN()) //quicker isShallowWater test, since we already know the plot is water
			if (!pCacheData->CanEmbarkAllWaterPassage())
				iCost += MOD_CORE_TRADE_NATURAL_ROUTES_TILE_BASE_COST*4;

		//try to stick to coast
		bool bIsAdjacentToLand = pFromPlot->isAdjacentToLand_Cached() && pToPlot->isAdjacentToLand_Cached();
		if (bIsAdjacentToLand)
			iCost += MOD_CORE_TRADE_NATURAL_ROUTES_TILE_BASE_COST/2;
		else
			iCost += MOD_CORE_TRADE_NATURAL_ROUTES_TILE_BASE_COST;
	}
	else
	{
#if defined(MOD_GLOBAL_PASSABLE_FORTS)
		CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(pToPlot->getImprovementType());
		bool bIsPassable = MOD_GLOBAL_PASSABLE_FORTS && pkImprovementInfo != NULL && pkImprovementInfo->IsMakesPassable();
		bool bIsCityOrPassable = (pToPlot->isCity() || bIsPassable);
		if(!bIsCityOrPassable)
			iCost += MOD_CORE_TRADE_NATURAL_ROUTES_TILE_BASE_COST*100;
		else
			iCost += MOD_CORE_TRADE_NATURAL_ROUTES_TILE_BASE_COST;
#else
		if (!pToPlot->isCity())
			iCost += 10000;
#endif
	}
	if(pFromPlot != NULL && pFromPlot->getOwner() != NO_PLAYER)
	{
		if(pToPlot->isImpassable(GET_PLAYER(pFromPlot->getOwner()).getTeam()))
		{
			iCost += MOD_CORE_TRADE_NATURAL_ROUTES_TILE_BASE_COST*100;
		}
	}
	else if(pFromPlot != NULL)
	{
		if(pToPlot->isImpassable(BARBARIAN_TEAM))
		{
			iCost += MOD_CORE_TRADE_NATURAL_ROUTES_TILE_BASE_COST*100;
		}
	}


	// avoid enemy lands
	TeamTypes eToPlotTeam = pToPlot->getTeam();
	if (eToPlotTeam != NO_TEAM && pCacheData->getTeam().isAtWar(eToPlotTeam))
	{
		iCost += MOD_CORE_TRADE_NATURAL_ROUTES_TILE_BASE_COST*10;
	}

	if (pCacheData->getTeam().GetID() != eToPlotTeam)
	{
		//try to stick to friendly territory
		if (pToPlot->getOwner()==NO_PLAYER || !pCacheData->getTeam().IsAllowsOpenBordersToTeam(eToPlotTeam))
			iCost += MOD_CORE_TRADE_NATURAL_ROUTES_TILE_BASE_COST/10;
	}

#else

	int iBaseCost = 100;
	int iCost = iBaseCost;
#if defined(MOD_GLOBAL_PASSABLE_FORTS)
	ImprovementTypes eImprovement = pToPlot->getImprovementType();
	CvImprovementEntry* pkImprovementInfo = NULL;
	if(eImprovement != NO_IMPROVEMENT)
	{
		pkImprovementInfo = GC.getImprovementInfo(eImprovement);
	}
	bool bIsPassable = MOD_GLOBAL_PASSABLE_FORTS && pkImprovementInfo != NULL && pkImprovementInfo->IsMakesPassable();
	bool bIsCityOrPassable = (pToPlot->isCity() || bIsPassable);
	if(!bIsCityOrPassable)
#else
	if (!pToPlot->isCity())
#endif
	{
		bool bIsAdjacentToLand = pFromPlot->isAdjacentToLand_Cached() && pToPlot->isAdjacentToLand_Cached();
		if (!bIsAdjacentToLand)
		{
			iCost += 1;
		}

		// if is enemy tile, avoid
		TeamTypes eToPlotTeam = pToPlot->getTeam();
		if (eToPlotTeam != NO_TEAM && pCacheData->getTeam().isAtWar(eToPlotTeam))
		{
			iCost += 1000; // slewis - is this too prohibitive? Too cheap?
		}

		if (!pToPlot->isWater())
		{
			iCost += 1000;
		}
		else
		{
			if (pToPlot->getTerrainType() != (TerrainTypes) GC.getSHALLOW_WATER_TERRAIN())	// Quicker isShallowWater test, since we already know the plot is water
			{
				if (!pCacheData->CanEmbarkAllWaterPassage())
				{
					iCost += 1000;
				}
			}
		}

		if(pToPlot->isImpassable())
		{
			iCost += 1000;
		}
	}
#endif

	FAssert(iCost != MAX_INT);
	FAssert(iCost > 0);

	return iCost;
}

//	--------------------------------------------------------------------------------
int TradeRouteWaterValid(const CvAStarNode* parent, const CvAStarNode* node, int, const void*, const CvAStar* finder)
{
	if(parent == NULL)
	{
		return TRUE;
	}

	const TradePathCacheData* pCacheData = reinterpret_cast<const TradePathCacheData*>(finder->GetScratchBuffer());

	CvMap& kMap = GC.getMap();
	CvPlot* pNewPlot = kMap.plotUnchecked(node->m_iX, node->m_iY);
#if defined(MOD_GLOBAL_PASSABLE_FORTS)
	ImprovementTypes eImprovement = pNewPlot->getImprovementType();
	CvImprovementEntry* pkImprovementInfo = NULL;
	if(eImprovement != NO_IMPROVEMENT)
	{
		pkImprovementInfo = GC.getImprovementInfo(eImprovement);
	}
	bool bIsPassable = MOD_GLOBAL_PASSABLE_FORTS && pkImprovementInfo != NULL && pkImprovementInfo->IsMakesPassable();
	bool bIsCityOrPassable = (pNewPlot->isCity() || bIsPassable);
	if(!bIsCityOrPassable)
#else
	if (!pNewPlot->isCity())
#endif
	{
		if (!pNewPlot->isWater())
		{
			return FALSE;
		}

		if (pNewPlot->getTerrainType() != (TerrainTypes) GC.getSHALLOW_WATER_TERRAIN())	// Quicker shallow water test since we know that the plot is water already
		{
			if (!pCacheData->CanEmbarkAllWaterPassage())
			{
				return FALSE;
			}
		}

		CvPlot* pParentPlot = kMap.plotUnchecked(parent->m_iX, parent->m_iY);
#if defined(MOD_GLOBAL_PASSABLE_FORTS)
		ImprovementTypes eImprovement = pParentPlot->getImprovementType();
		CvImprovementEntry* pkImprovementInfo = NULL;
		if(eImprovement != NO_IMPROVEMENT)
		{
			pkImprovementInfo = GC.getImprovementInfo(eImprovement);
		}
		bool bIsPassable = MOD_GLOBAL_PASSABLE_FORTS && pkImprovementInfo != NULL && pkImprovementInfo->IsMakesPassable();
		bool bIsCityOrPassable = (pParentPlot->isCity() || bIsPassable);
		if(!bIsCityOrPassable)
#else
		if (!pParentPlot->isCity())
#endif
		{
			if(pParentPlot->getArea() != pNewPlot->getArea())
			{
				return FALSE;
			}
		}
#if defined(MOD_BALANCE_CORE)
		if(pParentPlot != NULL)
		{
			PlayerTypes ePlayer = pParentPlot->getOwner();
			if(ePlayer != NO_PLAYER)
			{
				if(pParentPlot->isImpassable(GET_PLAYER(ePlayer).getTeam()))
				{
					return FALSE;
				}
			}
			else
			{
				if(pParentPlot->isImpassable(BARBARIAN_TEAM))
				{
					return FALSE;
				}
			}
		}
		else
		{
			if(pNewPlot->isImpassable(BARBARIAN_TEAM))
			{
				return FALSE;
			}
		}

#else
		if(pNewPlot->isImpassable())
		{
			return FALSE;
		}
#endif
	}

	return TRUE;
}

#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
/// trade route path finder - count path length
int TradeRoutePathAdd(CvAStarNode* parent, CvAStarNode* node, int data, const void*, CvAStar*)
{
	int	iTurns = (data == ASNC_INITIALADD) ? 0 : parent->m_iTurns;

	node->m_iMoves = 0;
	node->m_iTurns = iTurns+1;

	return 1;
}
#endif

//	--------------------------------------------------------------------------------
// Copy the supplied node and its parent nodes into an array of simpler path nodes for caching purposes.
// It is ok to pass in NULL, the resulting array will contain zero elements
//static
void CvAStar::CopyPath(const CvAStarNode* pkEndNode, CvPathNodeArray& kPathArray)
{
	if(pkEndNode != NULL)
	{
		const CvAStarNode* pkNode = pkEndNode;

		// Count the number of nodes
		uint uiNodeCount = 1;

		while(pkNode->m_pParent != NULL)
		{
			++uiNodeCount;
			pkNode = pkNode->m_pParent;
		}

		kPathArray.setsize(uiNodeCount);

		pkNode = pkEndNode;
		kPathArray[0] = *pkNode;

		uint uiIndex = 1;
		while(pkNode->m_pParent != NULL)
		{
			pkNode = pkNode->m_pParent;
			kPathArray[uiIndex++] = *pkNode;
		}
	}
	else
		kPathArray.setsize(0);	// Setting the size to 0 rather than clearing so that the array data is not deleted.  Helps with memory thrashing.
}

//	---------------------------------------------------------------------------
const CvPathNode* CvPathNodeArray::GetTurnDest(int iTurn)
{
	for (uint i = size(); i--; )
	{
		const CvPathNode& kNode = at(i);
		if (i == 0)
		{
			// Last node, only return it if it is the desired turn
			if (kNode.m_iTurns == iTurn)
				return &kNode;
			return NULL;
		}
		else
		{
			// Is this node the correct turn and the next node is a turn after it?
			if (kNode.m_iTurns == iTurn && at(i-1).m_iTurns > iTurn)
				return &kNode;
		}
	}

	return NULL;
}

#if defined(MOD_BALANCE_CORE)
//	---------------------------------------------------------------------------
bool IsPlotConnectedToPlot(PlayerTypes ePlayer, CvPlot* pFromPlot, CvPlot* pToPlot, RouteTypes eRestrictRoute, bool bIgnoreHarbors)
{
	if (ePlayer==NO_PLAYER || pFromPlot==NULL || pToPlot==NULL)
		return false;
	
	int iPathfinderFlags = ePlayer;
	if(eRestrictRoute == NO_ROUTE)
	{
		iPathfinderFlags |= MOVE_ANY_ROUTE;
	}
	else
	{
		// assuming that there are fewer than 256 players
		int iRouteValue = eRestrictRoute + 1;
		iPathfinderFlags |= (iRouteValue << 8);
	}

	if (bIgnoreHarbors)
	{
		GC.getRouteFinder().SetNumExtraChildrenFunc(NULL);
		GC.getRouteFinder().SetExtraChildGetterFunc(NULL);
	}
	else
	{
		//better make sure
		GC.getRouteFinder().SetNumExtraChildrenFunc(RouteGetNumExtraChildren);
		GC.getRouteFinder().SetExtraChildGetterFunc(RouteGetExtraChild);
	}

	GC.getRouteFinder().ForceReset();
	bool bReturnValue = GC.getRouteFinder().GeneratePath(pFromPlot->getX(), pFromPlot->getY(), pToPlot->getX(), pToPlot->getY(), iPathfinderFlags, false);

	if (bIgnoreHarbors)
	{
		// restore the harbor route pathfinder
		GC.getRouteFinder().SetNumExtraChildrenFunc(RouteGetNumExtraChildren);
		GC.getRouteFinder().SetExtraChildGetterFunc(RouteGetExtraChild);
	}

	return bReturnValue;
}
#endif