/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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
#define PATH_CITY_WEIGHT										(0) // slewis - reduced this to zero because we shouldn't avoid cities any more due to new garrison rules
#define PATH_DEFENSE_WEIGHT										(10)
#define PATH_TERRITORY_WEIGHT									(3)
#define PATH_STEP_WEIGHT										(2)
#define PATH_STRAIGHT_WEIGHT									(1)
#define PATH_PARTIAL_MOVE_WEIGHT								(500)
#define PATH_THROUGH_WATER										(1000)
#define PATH_STACKING_WEIGHT									(1000000)
#define PATH_CITY_AVOID_WEIGHT									(0) // slewis - reduced this to zero because we shouldn't avoid cities any more due to new garrison rules
#define	PATH_EXPLORE_NON_HILL_WEIGHT							(300)
#define PATH_EXPLORE_NON_REVEAL_WEIGHT							(10)
#define PATH_INCORRECT_EMBARKING_WEIGHT							(1000000)
#define PATH_BUILD_ROUTE_EXISTING_ROUTE_WEIGHT					(10)
//#define PATH_BUILD_ROUTE_RESOURCE_WEIGHT						(2)
#define PATH_BUILD_ROUTE_REMOVABLE_FEATURE_DISCOUNT				(0.8f)
#define PATH_BUILD_ROUTE_ALREADY_FLAGGED_DISCOUNT				(0.5f)
#define PATH_END_TURN_MOUNTAIN_WEIGHT							(1000000)
#define PATH_END_TURN_MISSIONARY_OTHER_TERRITORY				(150000)

#include <xmmintrin.h>

// until Tim is finished with AStar optimization
#define LINT_EXTRA_SUPPRESSIONS \
	4100 /* unreferenced formal parameter */ \
	4130 /* logical operation on address of a string constant */ \
	4189 /* local variable is initialized but not referenced */ \
	4239 /* nonstandard extension used */ \
	4238 /* nonstandard extension used : class rvalue used as lvalue */ \
	4505 /* unreferenced formal parameter */ \
	4512 /* assignment operator could not be generated */ \
	4702 /* unreachable code */ \
	4706 /* assignment within conditional expression */ \
	6001 /* Using uninitialized memory */ \
	6011 /* dereferencing NULL pointer.  */ \
	6246 /* Local declaration of 'variable' hides declaration of the same name in outer scope. For additional information, see previous declaration at line 'XXX'*/ \
	6262 /* Function uses 'xxxxx' bytes of stack: exceeds /analyze:stacksize'xxxxx'. Consider moving some data to heap */ \
	6302 /* Format string mismatch */ \
	6385 /* invalid data: accessing <buffer name>, the readable size is <size1> bytes, but <size2> bytes may be read: Lines: x, y */ \
	6386 /* Buffer overrun */
#include "LintFree.h"

#define PREFETCH_FASTAR_NODE(x) _mm_prefetch((const char*)x,  _MM_HINT_T0 ); _mm_prefetch(((const char*)x)+64,  _MM_HINT_T0 );
#define PREFETCH_FASTAR_CVPLOT(x) _mm_prefetch((const char*)x,  _MM_HINT_T0 ); _mm_prefetch(((const char*)x)+64,  _MM_HINT_T0 );
static void PrefetchRegionCvAStar(const char* pHead, const uint uiSize)
{
	const uint uiLoop = (uiSize+(64-1))/64;
	for(uint iCurr = 0; iCurr<uiLoop; ++iCurr)
	{
		_mm_prefetch(pHead+(iCurr*64),  _MM_HINT_T0);
	}
}

#if !defined(FINAL_RELESE)
//#define PATH_FINDER_LOGGING
#endif

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

	m_bIsMPCacheSafe = false;
	m_bDataChangeInvalidatesCache = false;
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
}

//	--------------------------------------------------------------------------------
/// Initializes the AStar algorithm
void CvAStar::Initialize(int iColumns, int iRows, bool bWrapX, bool bWrapY, CvAPointFunc IsPathDestFunc, CvAPointFunc DestValidFunc, CvAHeuristic HeuristicFunc, CvAStarFunc CostFunc, CvAStarFunc ValidFunc, CvAStarFunc NotifyChildFunc, CvAStarFunc NotifyListFunc, CvANumExtraChildren NumExtraChildrenFunc, CvAGetExtraChild GetExtraChildFunc, CvABegin InitializeFunc, CvAEnd UninitializeFunc, const void* pData)
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
	m_iXdest = iXdest;
	m_iYdest = iYdest;

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

//	--------------------------------------------------------------------------------
/// Link in a child
void CvAStar::LinkChild(CvAStarNode* node, CvAStarNode* check)
{
	int iKnownCost;

	iKnownCost = node->m_iKnownCost + udFunc(udCost, node, check, 0, m_pData);

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
		next->m_pPrev = addnode;
		addnode->m_pNext = next;
		m_pOpen = addnode;

		udFunc(udNotifyList, m_pOpen->m_pNext, m_pOpen, ASNL_STARTOPEN, m_pData);
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

	UnitPathCacheData* pCacheData = reinterpret_cast<UnitPathCacheData*>(finder->GetScratchBuffer());

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
}

//	--------------------------------------------------------------------------------
void UnitPathUninitialize(const void* pointer, CvAStar* finder)
{

}
//	--------------------------------------------------------------------------------
int PathDest(int iToX, int iToY, const void* pointer, CvAStar* finder)
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
int PathDestValid(int iToX, int iToY, const void* pointer, CvAStar* finder)
{
	CvUnit* pUnit;
	CvPlot* pToPlot;
	bool bAIControl;

	pToPlot = GC.getMap().plotCheckInvalid(iToX, iToY);
	FAssert(pToPlot != NULL);

	pUnit = ((CvUnit*)pointer);
	const UnitPathCacheData* pCacheData = reinterpret_cast<const UnitPathCacheData*>(finder->GetScratchBuffer());

	if(pToPlot == NULL || pUnit == NULL)
		return FALSE;

	if(pUnit->plot() == pToPlot)
	{
		return TRUE;
	}

	if(pToPlot->isMountain() && (!pCacheData->isHuman() || pCacheData->IsAutomated()))
	{
		return FALSE;
	}

	if(pCacheData->IsImmobile())
	{
		return FALSE;
	}

	if ((finder->GetInfo() & CvUnit::MOVEFLAG_STAY_ON_LAND) && pToPlot->isWater())
	{
		return FALSE;
	}

	bAIControl = pCacheData->IsAutomated();

	if(bAIControl)
	{
		if(!(finder->GetInfo() & MOVE_UNITS_IGNORE_DANGER))
		{
			if(!pUnit->IsCombatUnit() || pUnit->getArmyID() == FFreeList::INVALID_INDEX)
			{
				if(GET_PLAYER(pUnit->getOwner()).GetPlotDanger(*pToPlot) > 0)
				{
					return FALSE;
				}
			}
		}

		if(pCacheData->getDomainType() == DOMAIN_LAND)
		{
			int iGroupAreaID = pUnit->area()->GetID();
			if(pToPlot->getArea() != iGroupAreaID)
			{
				if(!(pToPlot->isAdjacentToArea(iGroupAreaID)) && !pUnit->CanEverEmbark())
				{
					return FALSE;
				}
			}
		}
	}

	TeamTypes eTeam = pCacheData->getTeam();
	bool bToPlotRevealed = pToPlot->isRevealed(eTeam);
	if(!bToPlotRevealed)
	{
		if(pCacheData->isNoRevealMap())
		{
			return FALSE;
		}
	}

	if(bToPlotRevealed)
	{
		CvCity* pCity = pToPlot->getPlotCity();
		if(pCity)
		{
			if(pCacheData->getOwner() != pCity->getOwner() && !GET_TEAM(eTeam).isAtWar(pCity->getTeam()) && !(finder->GetInfo() & MOVE_IGNORE_STACKING))
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

		if(finder->GetInfo() & MOVE_IGNORE_STACKING)
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


//	--------------------------------------------------------------------------------
/// Standard path finder - determine heuristic cost
int PathHeuristic(int iFromX, int iFromY, int iToX, int iToY)
{
	return (plotDistance(iFromX, iFromY, iToX, iToY) * PATH_MOVEMENT_WEIGHT);
}

//	--------------------------------------------------------------------------------
/// Standard path finder - compute cost of a path
int PathCost(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
	CvMap& kMap = GC.getMap();
	int iFromPlotX = parent->m_iX;
	int iFromPlotY = parent->m_iY;
	CvPlot* pFromPlot = kMap.plotUnchecked(iFromPlotX, iFromPlotY);

	int iToPlotX = node->m_iX;
	int iToPlotY = node->m_iY;
	CvPlot* pToPlot = kMap.plotUnchecked(iToPlotX, iToPlotY);

	CvUnit* pUnit = ((CvUnit*)pointer);
	const UnitPathCacheData* pCacheData = reinterpret_cast<const UnitPathCacheData*>(finder->GetScratchBuffer());

	DomainTypes eUnitDomain = pCacheData->getDomainType();

	CvAssertMsg(eUnitDomain != DOMAIN_AIR, "pUnit->getDomainType() is not expected to be equal with DOMAIN_AIR");

	bool bToPlotIsWater = pToPlot->isWater();
	int iMax;
	if(parent->m_iData1 > 0)
	{
		iMax = parent->m_iData1;
	}
	else
	{
		if (CvUnitMovement::ConsumesAllMoves(pUnit, pFromPlot, pToPlot) || CvUnitMovement::IsSlowedByZOC(pUnit, pFromPlot, pToPlot))
		{
			// The movement would consume all moves, get the moves we will forfeit based on the source plot, rather than
			// the destination plot.  This fixes issues where a land unit that has more movement points on water than on land
			// would have a very high cost to move onto water if their first move of the turn was at the edge of the water.
			iMax = pCacheData->baseMoves((pFromPlot->isWater())?DOMAIN_SEA:DOMAIN_LAND) * GC.getMOVE_DENOMINATOR();
		}
		else
			iMax = pCacheData->baseMoves(bToPlotIsWater?DOMAIN_SEA:DOMAIN_LAND) * GC.getMOVE_DENOMINATOR();
	}

	// Get the cost of moving to the new plot, passing in our max moves or the moves we have left, in case the movementCost 
	// method wants to burn all our remaining moves.  This is needed because our remaining moves for this segment of the path
	// may be larger or smaller than the baseMoves if some moves have already been used or if the starting domain (LAND/SEA)
	// of the path segment is different from the destination plot.
	int iCost = CvUnitMovement::MovementCost(pUnit, pFromPlot, pToPlot, pCacheData->baseMoves((pToPlot->isWater() || pCacheData->isEmbarked())?DOMAIN_SEA:pCacheData->getDomainType()), pCacheData->maxMoves(), iMax);

	TeamTypes eUnitTeam = pCacheData->getTeam();
	bool bMaximizeExplore = finder->GetInfo() & MOVE_MAXIMIZE_EXPLORE;

	int iMovesLeft = iMax - iCost;
	// Is the cost greater than our max?
	if (iMovesLeft < 0)
	{
		// Yes, we will still let the move happen, but that is the end of the turn.
		iCost = iMax;
		iMovesLeft = 0;
	}

	if(iMovesLeft == 0)
	{
		iCost = (PATH_MOVEMENT_WEIGHT * iCost);

		if(eUnitDomain == DOMAIN_LAND && !pFromPlot->isWater() && bToPlotIsWater && !pUnit->canEmbarkOnto(*pFromPlot, *pToPlot, true))
		{
			iCost += PATH_INCORRECT_EMBARKING_WEIGHT;
		}

		if(bMaximizeExplore)
		{
			if(!pToPlot->isHills())
			{
				iCost += PATH_EXPLORE_NON_HILL_WEIGHT;
			}
		}

		// Damage caused by features (mods)
		if(0 != GC.getPATH_DAMAGE_WEIGHT())
		{
			if(pToPlot->getFeatureType() != NO_FEATURE)
			{
				iCost += (GC.getPATH_DAMAGE_WEIGHT() * std::max(0, GC.getFeatureInfo(pToPlot->getFeatureType())->getTurnDamage())) / GC.getMAX_HIT_POINTS();
			}

			if(pToPlot->getExtraMovePathCost() > 0)
			{
				iCost += (PATH_MOVEMENT_WEIGHT * pToPlot->getExtraMovePathCost());
			}
		}

		// Penalty for stacking
		if(GC.getPLOT_UNIT_LIMIT() > 0 && !(finder->GetInfo() & MOVE_IGNORE_STACKING))
		{
			// Check to see if any units are present at this full-turn move plot... if the player can see what's there
			if(pToPlot->getNumFriendlyUnitsOfType(pUnit) >= GC.getPLOT_UNIT_LIMIT())
			{
				iCost += PATH_STACKING_WEIGHT;
			}
		}

		// Penalty for ending a turn on a mountain
		if(pToPlot->isMountain())
		{
			// We want to discourage AIs and automated units from exhausting their movement on a mountain, but if the unit is manually controlled by the human, let them do what they want.
			if (!pCacheData->isHuman() || pCacheData->IsAutomated())
			{
				iCost += PATH_END_TURN_MOUNTAIN_WEIGHT;
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

#if PATH_CITY_AVOID_WEIGHT != 0
		if(pToPlot->getPlotCity() && !(pToPlot->getX() == finder->GetDestX() && pToPlot->getY() == finder->GetDestY()))
		{
			iCost += PATH_CITY_AVOID_WEIGHT; // slewis - this should be zeroed out currently
		}
#endif
	}
	else
	{
		iCost = (PATH_MOVEMENT_WEIGHT * iCost);
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
		if(iMovesLeft == 0)
		{
			iCost += (PATH_DEFENSE_WEIGHT * std::max(0, (200 - ((pUnit->noDefensiveBonus()) ? 0 : pToPlot->defenseModifier(eUnitTeam, false)))));
		}

		if(pCacheData->IsAutomated())
		{
			if(pCacheData->IsCanAttack())
			{
				if(finder->IsPathDest(iToPlotX, iToPlotY))
				{
					if(pToPlot->isVisibleEnemyDefender(pUnit))
					{
						iCost += (PATH_DEFENSE_WEIGHT * std::max(0, (200 - ((pUnit->noDefensiveBonus()) ? 0 : pFromPlot->defenseModifier(eUnitTeam, false)))));

						// I guess we may as well be the garrison
#if PATH_CITY_WEIGHT != 0
						if(!(pFromPlot->isCity()))
						{
							iCost += PATH_CITY_WEIGHT;
						}
#endif

						if(pFromPlot->isRiverCrossing(directionXY(iFromPlotX, iFromPlotY, iToPlotX, iToPlotY)))
						{
							if(!(pUnit->isRiverCrossingNoPenalty()))
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

//	---------------------------------------------------------------------------
/// Standard path finder - check validity of a coordinate
int PathValid(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
	CvMap& theMap = GC.getMap();

	CvPlot* pToPlot = theMap.plotUnchecked(node->m_iX, node->m_iY);
	PREFETCH_FASTAR_CVPLOT(reinterpret_cast<char*>(pToPlot));

	CvUnit* pUnit = ((CvUnit*)pointer);
	const UnitPathCacheData* pCacheData = reinterpret_cast<const UnitPathCacheData*>(finder->GetScratchBuffer());
	TeamTypes eUnitTeam = pCacheData->getTeam();
	PlayerTypes unit_owner = pCacheData->getOwner();

	CvAssertMsg(eUnitTeam != NO_TEAM, "The unit's team should be a vaild value");
	if (eUnitTeam == NO_TEAM)
	{
		eUnitTeam = GC.getGame().getActiveTeam();
	}

	CvTeam& kUnitTeam = GET_TEAM(eUnitTeam);

	// Cache values for this node that we will use in the loop
	CvPathNodeCacheData& kToNodeCacheData = node->m_kCostCacheData;
	kToNodeCacheData.bPlotVisibleToTeam = pToPlot->isVisible(eUnitTeam);
	kToNodeCacheData.iNumFriendlyUnitsOfType = pToPlot->getNumFriendlyUnitsOfType(pUnit);
	kToNodeCacheData.bIsMountain = pToPlot->isMountain();
	kToNodeCacheData.bIsWater = pToPlot->isWater();
	kToNodeCacheData.bCanEnterTerrain = pUnit->canEnterTerrain(*pToPlot, CvUnit::MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE);
	kToNodeCacheData.bIsRevealedToTeam = pToPlot->isRevealed(eUnitTeam);
	kToNodeCacheData.bContainsOtherFriendlyTeamCity = false;
	CvCity* pCity = pToPlot->getPlotCity();
	if(pCity)
	{
		if(unit_owner != pCity->getOwner() && !kUnitTeam.isAtWar(pCity->getTeam()))
			kToNodeCacheData.bContainsOtherFriendlyTeamCity = true;
	}
	kToNodeCacheData.bContainsEnemyCity = pToPlot->isEnemyCity(*pUnit);
	kToNodeCacheData.bContainsVisibleEnemy = pToPlot->isVisibleEnemyUnit(pUnit);
	kToNodeCacheData.bContainsVisibleEnemyDefender = pToPlot->getBestDefender(NO_PLAYER, unit_owner, pUnit).pointer() != NULL;

	// If this is the first node in the path, it is always valid (starting location)
	if (parent == NULL)
	{
		return TRUE;
	}

	CvPlot* pFromPlot = theMap.plotUnchecked(parent->m_iX, parent->m_iY);
	PREFETCH_FASTAR_CVPLOT(reinterpret_cast<char*>(pFromPlot));

	// pulling invariants out of the loop
	bool bAIControl = pCacheData->IsAutomated();
	int iUnitX = pUnit->getX();
	int iUnitY = pUnit->getY();
	DomainTypes unit_domain_type = pCacheData->getDomainType();
	bool bUnitIsCombat           = pUnit->IsCombatUnit();
	bool bIsHuman				 = pCacheData->isHuman();
	int iFinderInfo              = finder->GetInfo();
	CvPlot* pUnitPlot            = pUnit->plot();
	int iFinderIgnoreStacking    = iFinderInfo & MOVE_IGNORE_STACKING;
	int iUnitPlotLimit           = GC.getPLOT_UNIT_LIMIT();
	bool bFromPlotOwned          = pFromPlot->isOwned();
	TeamTypes eFromPlotTeam      = pFromPlot->getTeam();

	// We have determined that this node is not the origin above (parent == NULL)
	CvAStarNode* pNode = node;
	bool bPreviousNodeHostile = false;
	bool bPreviousVisibleToTeam = kToNodeCacheData.bPlotVisibleToTeam;
	int iDestX = finder->GetDestX();
	int iDestY = finder->GetDestY();
	int iNodeX = node->m_iX;
	int iNodeY = node->m_iY;
	int iOldNumTurns = -1;
	int iNumTurns;

	// First run special case for checking "node" since it doesn't have a parent set yet
	bool bFirstRun = true;

	// Have to calculate this specially because the node passed into this function doesn't yet have data stored it in (hasn't reached pathAdd yet)
	int iStartMoves = parent->m_iData1;
	iNumTurns = parent->m_iData2;

	if(iStartMoves == 0)
	{
		iNumTurns++;
	}

	iOldNumTurns = -1;

	CvPlot* pPlot = NULL;

	// Get a reference to the parent node cache data
	CvPathNodeCacheData& kFromNodeCacheData = parent->m_kCostCacheData;

	// Loop through the current path until we find the path origin.
	// This validates the path with the inclusion of the new path node.  We must do this because of the rules of where a unit can finish a turn.
	// Please note that this can be an expensive loop as the path gets longer and longer, do as little work as possible in validating each node.  
	// If there is an invariant value that needs to be fetched from the plot or unit for the node, please do the calculation and put it in the node's data cache.
	while(pNode != NULL)
	{
		PREFETCH_FASTAR_NODE(pNode->m_pParent);

		CvPathNodeCacheData& kNodeCacheData = pNode->m_kCostCacheData;
		// This is a safeguard against the algorithm believing a plot to be impassable before actually knowing it (mid-search)
		if(iOldNumTurns != -1 || (iDestX == iNodeX && iDestY == iNodeY))
		{
			// This plot is of greater distance than previously, so we know the unit is ending its turn here (pNode), or it's trying to attack through a unit (and might end up on this tile if an attack fails to kill the enemy)
			if(iNumTurns != iOldNumTurns || bPreviousNodeHostile || !bPreviousVisibleToTeam)
			{
				// Don't count origin, or else a unit will block its own movement!
				if(iNodeX != iUnitX || iNodeY != iUnitY)
				{
					if(kNodeCacheData.bPlotVisibleToTeam)
					{
						// Check to see if any units are present at this full-turn move plot... if the player can see what's there
						if(kNodeCacheData.iNumFriendlyUnitsOfType >= iUnitPlotLimit && !(iFinderIgnoreStacking))
						{
							return FALSE;
						}

						if (kNodeCacheData.bIsMountain && !(iFinderIgnoreStacking) && (!bIsHuman || bAIControl))
						{
							return FALSE;
						}

						if(kNodeCacheData.bIsMountain && !kNodeCacheData.bCanEnterTerrain)
						{
							return FALSE;
						}

						if ((iFinderInfo & CvUnit::MOVEFLAG_STAY_ON_LAND) && kNodeCacheData.bIsWater)
						{
							return FALSE;
						}
					}

					if(kNodeCacheData.bIsRevealedToTeam)
					{
						if (kNodeCacheData.bContainsOtherFriendlyTeamCity && !(iFinderIgnoreStacking))
							return FALSE;
					}
				}
			}
		}

		bPreviousNodeHostile = false;
		if(kNodeCacheData.bContainsEnemyCity)
		{
			bPreviousNodeHostile = true;
		}
		// Prevents units from passing through one another on its way to attack another unit
		else if(kNodeCacheData.bContainsVisibleEnemy)
		{
			// except when attacking an unguarded civilian unit
			if(kNodeCacheData.bContainsVisibleEnemyDefender)
			{
				bPreviousNodeHostile = true;
			}
		}

		bPreviousVisibleToTeam = kNodeCacheData.bPlotVisibleToTeam;
		// JON - Special case for the original node passed into this function because it's not yet linked to any parent
		if(pNode == node && bFirstRun)
		{
			pNode = parent;
			bFirstRun = false;
		}
		else
		{
			pNode = pNode->m_pParent;
		}

		if(pNode != NULL)
		{
			iNodeX = pNode->m_iX;
			iNodeY = pNode->m_iY;
			iOldNumTurns = iNumTurns;
			iNumTurns = pNode->m_iData2;
		}
	}

	// slewis - moved this up so units can't move directly into the water. Not 100% sure this is the right solution.
	if(unit_domain_type == DOMAIN_LAND)
	{
		if(!kFromNodeCacheData.bIsWater && kToNodeCacheData.bIsWater && kToNodeCacheData.bIsRevealedToTeam && !pUnit->canEmbarkOnto(*pFromPlot, *pToPlot, true))
		{
			if(!pUnit->IsHoveringUnit() && !pUnit->canMoveAllTerrain())
			{
				return FALSE;
			}
		}
	}

	if(!bUnitIsCombat && unit_domain_type != DOMAIN_AIR)
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

	// slewis - Added to catch when the unit is adjacent to an enemy unit while it is stacked with a friendly unit.
	//          The logic above (with bPreviousNodeHostile) catches this problem with a path that's longer than one step
	//          but does not catch when the path is only one step.
	if(bUnitIsCombat && unit_domain_type != DOMAIN_AIR && pUnitPlot->isAdjacent(pToPlot) && kToNodeCacheData.bContainsVisibleEnemy && !(iFinderIgnoreStacking))
	{
		if(kToNodeCacheData.bContainsVisibleEnemyDefender)
		{
			if(pUnitPlot->getNumFriendlyUnitsOfType(pUnit) > iUnitPlotLimit)
			{
				return FALSE;
			}
		}
	}

	if(pUnitPlot == pFromPlot)
	{
		return TRUE;
	}

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

	if(bAIControl)
	{
		if((parent->m_iData2 > 1) || (parent->m_iData1 == 0))
		{
			if(!(iFinderInfo & MOVE_UNITS_IGNORE_DANGER))
			{
				if(!bUnitIsCombat || pUnit->getArmyID() == FFreeList::INVALID_INDEX)
				{
					if(GET_PLAYER(unit_owner).GetPlotDanger(*pFromPlot) > 0)
					{
						return FALSE;
					}
				}
			}
		}
	}

	// slewis - added AI check and embark check to prevent units from moving into unexplored areas
	if(bAIControl || kFromNodeCacheData.bIsRevealedToTeam || pCacheData->isEmbarked() || !bIsHuman)
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



//	--------------------------------------------------------------------------------
/// Standard path finder - add a new path
int PathAdd(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
	int iMoves = MAX_INT;
	CvUnit* pUnit = ((CvUnit*)pointer);
	const UnitPathCacheData* pCacheData = reinterpret_cast<const UnitPathCacheData*>(finder->GetScratchBuffer());

	int iTurns;

	if(data == ASNC_INITIALADD)
	{
		iTurns = 1;
		iMoves = std::min(iMoves, pUnit->movesLeft());
	}
	else
	{
		CvMap& kMap = GC.getMap();
		CvPlot* pFromPlot = kMap.plotUnchecked(parent->m_iX, parent->m_iY);
		CvPlot* pToPlot = kMap.plotUnchecked(node->m_iX, node->m_iY);

		int iStartMoves = parent->m_iData1;
		iTurns = parent->m_iData2;

		if(iStartMoves == 0)
		{
			iTurns++;
			iStartMoves = pCacheData->baseMoves(pToPlot->isWater()?DOMAIN_SEA:DOMAIN_LAND) * GC.getMOVE_DENOMINATOR();
		}

		// We can't use maxMoves, because that checks where the unit is currently, and we're plotting a path so we have to see
		// what the max moves would be like if the unit was already at the desired location.
		if (CvUnitMovement::ConsumesAllMoves(pUnit, pFromPlot, pToPlot) || CvUnitMovement::IsSlowedByZOC(pUnit, pFromPlot, pToPlot))
		{
			iMoves = 0;
		}
		else
		{
			iMoves = std::min(iMoves, std::max(0, iStartMoves - CvUnitMovement::MovementCost(pUnit, pFromPlot, pToPlot, pCacheData->baseMoves((pToPlot->isWater() || pCacheData->isEmbarked())?DOMAIN_SEA:pCacheData->getDomainType()), pCacheData->maxMoves(), iStartMoves)));
		}
	}

	FAssertMsg(iMoves >= 0, "iMoves is expected to be non-negative (invalid Index)");

	node->m_iData1 = iMoves;
	node->m_iData2 = iTurns;

	return 1;
}

//	--------------------------------------------------------------------------------
/// Two layer path finder - if add a new open node with movement left, add a second one assuming stop for turn here
int PathNodeAdd(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
	CvAStarNode* pNode;

	if(data == ASNL_ADDOPEN || data == ASNL_STARTOPEN)
	{
		// Are there movement points left and we're worried about stacking or mountains?
		if(node->m_iData1 > 0 && !finder->IsPathDest(node->m_iX, node->m_iY) && (!(finder->GetInfo() & MOVE_IGNORE_STACKING) || GC.getMap().plotUnchecked(node->m_iX, node->m_iY)->isMountain()))
		{
			// Retrieve another node
			CvTwoLayerPathFinder* twoLayerFinder = static_cast<CvTwoLayerPathFinder*>(finder);
			pNode = twoLayerFinder->GetPartialMoveNode(node->m_iX, node->m_iY);
			pNode->m_iData1 = 0;   // Zero out movement
			pNode->m_iData2 = node->m_iData2;
			pNode->m_iHeuristicCost = node->m_iHeuristicCost;
			pNode->m_iKnownCost = node->m_iKnownCost + (PATH_MOVEMENT_WEIGHT * node->m_iData1);
			pNode->m_iTotalCost = node->m_iTotalCost;
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
/// Ignore units path finder - is this end point for the path valid?
int IgnoreUnitsDestValid(int iToX, int iToY, const void* pointer, CvAStar* finder)
{
	CvUnit* pUnit;
	CvPlot* pToPlot;
	bool bAIControl;
	CvMap& kMap = GC.getMap();

	pToPlot = kMap.plotUnchecked(iToX, iToY);

	pUnit = ((CvUnit*)pointer);
	const UnitPathCacheData* pCacheData = reinterpret_cast<const UnitPathCacheData*>(finder->GetScratchBuffer());

	CvPlot* pUnitPlot = kMap.plotUnchecked(pUnit->getX(), pUnit->getY());
	if(pUnitPlot == pToPlot)
	{
		return TRUE;
	}

	if(pCacheData->IsImmobile())
	{
		return FALSE;
	}

	if(pToPlot->isMountain() && (!pCacheData->isHuman() || pCacheData->IsAutomated()))
	{
		return FALSE;
	}

	if ((finder->GetInfo() & CvUnit::MOVEFLAG_STAY_ON_LAND) && pToPlot->isWater())
	{
		return FALSE;
	}

	bAIControl = pCacheData->IsAutomated();

	if(bAIControl)
	{
		if(pCacheData->getDomainType() == DOMAIN_LAND)
		{
			int iGroupAreaID = pUnit->area()->GetID();
			if(pToPlot->getArea() != iGroupAreaID)
			{
				if(!(pToPlot->isAdjacentToArea(iGroupAreaID)))
				{
					return FALSE;
				}
			}
		}
	}

	TeamTypes eUnitTeam = pUnit->getTeam();

	if(!pToPlot->isRevealed(eUnitTeam))
	{
		if(pCacheData->isNoRevealMap())
		{
			return FALSE;
		}
	}

	if(bAIControl || pToPlot->isRevealed(eUnitTeam))
	{
		if(!pUnit->canEnterTerrain(*pToPlot, CvUnit::MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE) || !pUnit->canEnterTerritory(eUnitTeam))
		{
			return FALSE;
		}
	}
	return TRUE;
}


//	--------------------------------------------------------------------------------
/// Ignore units path finder - compute cost of a path
int IgnoreUnitsCost(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
	CvUnit* pUnit;
	int iCost;
	int iMax;

	CvMap& kMap = GC.getMap();
	int iFromPlotX = parent->m_iX;
	int iFromPlotY = parent->m_iY;
	CvPlot* pFromPlot = kMap.plotUnchecked(iFromPlotX, iFromPlotY);

	int iToPlotX = node->m_iX;
	int iToPlotY = node->m_iY;
	CvPlot* pToPlot = kMap.plotUnchecked(iToPlotX, iToPlotY);

	pUnit = ((CvUnit*)pointer);
	const UnitPathCacheData* pCacheData = reinterpret_cast<const UnitPathCacheData*>(finder->GetScratchBuffer());

	CvAssertMsg(pUnit->getDomainType() != DOMAIN_AIR, "pUnit->getDomainType() is not expected to be equal with DOMAIN_AIR");

	if(parent->m_iData1 > 0)
	{
		iMax = parent->m_iData1;
	}
	else
	{
		iMax = pCacheData->baseMoves(pToPlot->isWater()?DOMAIN_SEA:DOMAIN_LAND) * GC.getMOVE_DENOMINATOR();
	}

	// Get the cost of moving to the new plot, passing in our max moves or the moves we have left, in case the movementCost 
	// method wants to burn all our remaining moves.  This is needed because our remaining moves for this segment of the path
	// may be larger or smaller than the baseMoves if some moves have already been used or if the starting domain (LAND/SEA)
	// of the path segment is different from the destination plot.
	iCost = CvUnitMovement::MovementCostNoZOC(pUnit, pFromPlot, pToPlot, pCacheData->baseMoves((pToPlot->isWater() || pCacheData->isEmbarked())?DOMAIN_SEA:pCacheData->getDomainType()), pCacheData->maxMoves(), iMax);

	TeamTypes eUnitTeam = pUnit->getTeam();

	int iMovesLeft = iMax - iCost;
	// Is the cost greater than our max?
	if (iMovesLeft < 0)
	{
		// Yes, we will still let the move happen, but that is the end of the turn.
		iCost = iMax;
		iMovesLeft = 0;
	}

	if(iMovesLeft == 0)
	{
		iCost = (PATH_MOVEMENT_WEIGHT * iCost);

		if(!pFromPlot->isWater() && pToPlot->isWater() && !pUnit->canEmbarkOnto(*pFromPlot, *pToPlot, true))
		{
			iCost += PATH_INCORRECT_EMBARKING_WEIGHT;
		}

		if(pToPlot->getTeam() != eUnitTeam)
		{
			iCost += PATH_TERRITORY_WEIGHT;
		}

		if(finder->GetInfo() & MOVE_MAXIMIZE_EXPLORE)
		{
			if(!pToPlot->isHills())
			{
				iCost += PATH_EXPLORE_NON_HILL_WEIGHT;
			}
		}

		// Damage caused by features (mods)
		if(0 != GC.getPATH_DAMAGE_WEIGHT())
		{
			if(pToPlot->getFeatureType() != NO_FEATURE)
			{
				iCost += (GC.getPATH_DAMAGE_WEIGHT() * std::max(0, GC.getFeatureInfo(pToPlot->getFeatureType())->getTurnDamage())) / GC.getMAX_HIT_POINTS();
			}

			if(pToPlot->getExtraMovePathCost() > 0)
			{
				iCost += (PATH_MOVEMENT_WEIGHT * pToPlot->getExtraMovePathCost());
			}
		}

		if(pToPlot->getPlotCity() && !(pToPlot->getX() == finder->GetDestX() && pToPlot->getY() == finder->GetDestY()))
		{
			iCost += PATH_CITY_AVOID_WEIGHT; // slewis - this should be zeroed out currently
		}
	}
	else
	{
		iCost = (PATH_MOVEMENT_WEIGHT * iCost);
	}

	if(finder->GetInfo() & MOVE_MAXIMIZE_EXPLORE)
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
	if(pCacheData->getDomainType() == DOMAIN_LAND && pToPlot->isWater() && (!pUnit->isHuman() || pUnit->IsAutomated()))
	{
		iCost += PATH_THROUGH_WATER;
	}

	if(pUnit->IsCombatUnit())
	{
		if(iMovesLeft == 0)
		{
			iCost += (PATH_DEFENSE_WEIGHT * std::max(0, (200 - ((pUnit->noDefensiveBonus()) ? 0 : pToPlot->defenseModifier(eUnitTeam, false)))));
		}

		if(pCacheData->IsAutomated())
		{
			if(pCacheData->IsCanAttack())
			{
				if(finder->IsPathDest(pToPlot->getX(), pToPlot->getY()))
				{
					if(pToPlot->isVisibleEnemyDefender(pUnit))
					{
						iCost += (PATH_DEFENSE_WEIGHT * std::max(0, (200 - ((pUnit->noDefensiveBonus()) ? 0 : pFromPlot->defenseModifier(eUnitTeam, false)))));

						// I guess we may as well be the garrison
						if(!(pFromPlot->isCity()))
						{
							iCost += PATH_CITY_WEIGHT;
						}

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


//	--------------------------------------------------------------------------------
/// Ignore units path finder - check validity of a coordinate
int IgnoreUnitsValid(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
	CvUnit* pUnit;
	CvPlot* pFromPlot;
	CvPlot* pToPlot;
	bool bAIControl;

	if(parent == NULL)
	{
		return TRUE;
	}

	CvMap& theMap = GC.getMap();

	pFromPlot = theMap.plotUnchecked(parent->m_iX, parent->m_iY);
	pToPlot = theMap.plotUnchecked(node->m_iX, node->m_iY);

	pUnit = ((CvUnit*)pointer);
	const UnitPathCacheData* pCacheData = reinterpret_cast<const UnitPathCacheData*>(finder->GetScratchBuffer());

	TeamTypes eUnitTeam = pCacheData->getTeam();

	CvPlot* pUnitPlot = theMap.plotUnchecked(pUnit->getX(), pUnit->getY());

	// slewis - moved this up so units can't move directly into the water. Not 100% sure this is the right solution.
	if(pCacheData->getDomainType() == DOMAIN_LAND)
	{
		if(!pFromPlot->isWater() && pToPlot->isWater() && pToPlot->isRevealed(eUnitTeam) && !pUnit->canEmbarkOnto(*pFromPlot, *pToPlot, true))
		{
			return FALSE;
		}
	}

	if(pUnitPlot == pFromPlot)
	{
		return TRUE;
	}

	if(finder->GetInfo() & MOVE_TERRITORY_NO_UNEXPLORED)
	{
		if(!(pFromPlot->isRevealed(eUnitTeam)))
		{
			return FALSE;
		}

		if(pFromPlot->isOwned())
		{
			if(pFromPlot->getTeam() != eUnitTeam)
			{
				return FALSE;
			}
		}
	}

	if(finder->GetInfo() & MOVE_TERRITORY_NO_ENEMY)
	{
		if(pFromPlot->isOwned())
		{
			if(atWar(pFromPlot->getTeam(), eUnitTeam))
			{
				return FALSE;
			}
		}
	}

	bAIControl = pUnit->IsAutomated();

	// slewis - added AI check and embark check to prevent units from moving into unexplored areas
	if(bAIControl || (pFromPlot->isRevealed(eUnitTeam) || pCacheData->isEmbarked()) || !pCacheData->isHuman())
	{
		if(!pUnit->canEnterTerrain(*pToPlot, CvUnit::MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE) || !pUnit->canEnterTerritory(eUnitTeam))
		{
			return FALSE;
		}
	}

	return TRUE;
}

//	--------------------------------------------------------------------------------
/// Ignore units path finder - add a new path
int IgnoreUnitsPathAdd(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
	int iTurns;

	CvUnit* pUnit = ((CvUnit*)pointer);
	const UnitPathCacheData* pCacheData = reinterpret_cast<const UnitPathCacheData*>(finder->GetScratchBuffer());
	int iMoves = MAX_INT;

	if(data == ASNC_INITIALADD)
	{
		iTurns = 1;
		iMoves = std::min(iMoves, pUnit->movesLeft());
	}
	else
	{
		CvPlot* pFromPlot = GC.getMap().plotUnchecked(parent->m_iX, parent->m_iY);
		CvPlot* pToPlot = GC.getMap().plotUnchecked(node->m_iX, node->m_iY);

		int iStartMoves = parent->m_iData1;
		iTurns = parent->m_iData2;

		if(iStartMoves == 0)
		{
			iTurns++;
			iStartMoves = pCacheData->baseMoves(pToPlot->isWater()?DOMAIN_SEA:DOMAIN_LAND) * GC.getMOVE_DENOMINATOR();
		}

		// We can't use maxMoves, because that checks where the unit is currently, and we're plotting a path so we have to see
		// what the max moves would be like if the unit was already at the desired location.
		iMoves = std::min(iMoves, std::max(0, iStartMoves - CvUnitMovement::MovementCostNoZOC(pUnit, pFromPlot, pToPlot, pCacheData->baseMoves((pToPlot->isWater() || pCacheData->isEmbarked())?DOMAIN_SEA:pCacheData->getDomainType()), pCacheData->maxMoves())));
	}

	FAssertMsg(iMoves >= 0, "iMoves is expected to be non-negative (invalid Index)");

	node->m_iData1 = iMoves;
	node->m_iData2 = iTurns;

	return 1;
}


//	--------------------------------------------------------------------------------
/// Step path finder - is this end point for the path valid?
int StepDestValid(int iToX, int iToY, const void* pointer, CvAStar* finder)
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
int StepCost(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
	return 1;
}


//	--------------------------------------------------------------------------------
/// Step path finder - check validity of a coordinate
int StepValid(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
	if(parent == NULL)
	{
		return TRUE;
	}

	int iFlags = finder->GetInfo();
	PlayerTypes ePlayer = (PlayerTypes)(iFlags & 0xFF);

	PlayerTypes eEnemy = *(PlayerTypes*)pointer;

	CvPlayer& thisPlayer = GET_PLAYER(ePlayer);

	CvMap& kMap = GC.getMap();
	CvPlot* pNewPlot = kMap.plotUnchecked(node->m_iX, node->m_iY);

	if(kMap.plotUnchecked(parent->m_iX, parent->m_iY)->getArea() != pNewPlot->getArea())
	{
		return FALSE;
	}

	if(pNewPlot->isImpassable())
	{
		return FALSE;
	}

	// Ocean hex and team can't navigate on oceans?
	if (!GET_TEAM(thisPlayer.getTeam()).getEmbarkedAllWaterPassage())
	{
		if (pNewPlot->getTerrainType() == TERRAIN_OCEAN)
		{
			return FALSE;
		}
	}

	PlayerTypes ePlotOwnerPlayer = pNewPlot->getOwner();
	if (ePlotOwnerPlayer != NO_PLAYER && ePlotOwnerPlayer != eEnemy && !pNewPlot->IsFriendlyTerritory(ePlayer))
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

	return TRUE;
}


//	--------------------------------------------------------------------------------
/// Step path finder - check validity of a coordinate (special case that allows any area)
int StepValidAnyArea(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
	if(parent == NULL)
	{
		return TRUE;
	}

	int iFlags = finder->GetInfo();
	PlayerTypes ePlayer = (PlayerTypes)(iFlags & 0xFF);

	PlayerTypes eEnemy = *(PlayerTypes*)pointer;

	CvPlayer& thisPlayer = GET_PLAYER(ePlayer);

	CvMap& kMap = GC.getMap();
	CvPlot* pNewPlot = kMap.plotUnchecked(node->m_iX, node->m_iY);

	//if(kMap.plotUnchecked(parent->m_iX, parent->m_iY)->getArea() != pNewPlot->getArea())
	//{
	//	return FALSE;
	//}

	if(pNewPlot->isImpassable())
	{
		return FALSE;
	}

	// Ocean hex and team can't navigate on oceans?
	if (!GET_TEAM(thisPlayer.getTeam()).getEmbarkedAllWaterPassage())
	{
		if (pNewPlot->getTerrainType() == TERRAIN_OCEAN)
		{
			return FALSE;
		}
	}

	PlayerTypes ePlotOwnerPlayer = pNewPlot->getOwner();
	if (ePlotOwnerPlayer != NO_PLAYER && ePlotOwnerPlayer != eEnemy && !pNewPlot->IsFriendlyTerritory(ePlayer))
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

	return TRUE;
}


//	--------------------------------------------------------------------------------
/// Step path finder - add a new path
int StepAdd(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
	if(data == ASNC_INITIALADD)
	{
		node->m_iData1 = 0;
	}
	else
	{
		node->m_iData1 = (parent->m_iData1 + 1);
	}

	FAssertMsg(node->m_iData1 >= 0, "node->m_iData1 is expected to be non-negative (invalid Index)");

	return 1;
}


//	--------------------------------------------------------------------------------
/// Influence path finder - is this end point for the path valid?
int InfluenceDestValid(int iToX, int iToY, const void* pointer, CvAStar* finder)
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
int InfluenceCost(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
	int iCost = 0;
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
				iCost += 15;
		}

		if(pFromPlot->isRiverCrossing(directionXY(pFromPlot, pToPlot)))
			iCost += GC.getINFLUENCE_RIVER_COST();

		// Hill cost
		if(pToPlot->isHills())
			iCost += GC.getINFLUENCE_HILL_COST();
		// Mountain Cost
		else if(pToPlot->isMountain())
			iCost += GC.getINFLUENCE_MOUNTAIN_COST();
		// Not a hill or mountain - use the terrain cost
		else
		{
			iCost += GC.getTerrainInfo(pToPlot->getTerrainType())->getInfluenceCost();
			iCost += ((pToPlot->getFeatureType() == NO_FEATURE) ? 0 : GC.getFeatureInfo(pToPlot->getFeatureType())->getInfluenceCost());
		}
	}
	else
	{
		iCost = 1;
	}
	iCost = std::max(1,iCost);
	return iCost;
}


//	--------------------------------------------------------------------------------
/// Influence path finder - check validity of a coordinate
int InfluenceValid(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
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
int InfluenceAdd(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
	if(data == ASNC_INITIALADD)
	{
		node->m_iData1 = 0;
	}
	else
	{
		node->m_iData1 = (parent->m_iData1 + 1/*influenceCost(parent,node,data,pointer,finder)*/);
	}

	FAssertMsg(node->m_iData1 >= 0, "node->m_iData1 is expected to be non-negative (invalid Index)");

	return 1;
}


//	--------------------------------------------------------------------------------
// Route - Return the x, y plot of the node that we want to access
int RouteGetExtraChild(CvAStarNode* node, int iIndex, int& iX, int& iY, CvAStar* finder)
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

		if(pRouteInfo->m_cRouteState & CvCityConnections::HAS_WATER_ROUTE)
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
int RouteValid(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
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
int RouteGetNumExtraChildren(CvAStarNode* node,  CvAStar* finder)
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

		if(pRouteInfo->m_cRouteState & CvCityConnections::HAS_WATER_ROUTE)
		{
			iResultNum++;
		}
	}

	return iResultNum;
}

//	--------------------------------------------------------------------------------
/// Water route valid finder - check the validity of a coordinate
int WaterRouteValid(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
	CvPlot* pNewPlot;

	if(parent == NULL)
	{
		return TRUE;
	}

	PlayerTypes ePlayer = (PlayerTypes)(finder->GetInfo());
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
int BuildRouteCost(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
	CvPlot* pPlot = GC.getMap().plotUnchecked(node->m_iX, node->m_iY);
	int iFlags = finder->GetInfo();
	PlayerTypes ePlayer = (PlayerTypes)(iFlags & 0xFF);
	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	int iRoute = iFlags & 0xFF00;
	iRoute = iRoute >> 8;
	iRoute = iRoute - 1;
	RouteTypes eRoute = (RouteTypes)(iRoute);

	if(pPlot->getRouteType() != NO_ROUTE)
	{
		int iReturnValue = PATH_BUILD_ROUTE_EXISTING_ROUTE_WEIGHT;
		if(pPlot->getRouteType() == eRoute)
		{
			iReturnValue = 1;
		}
		return iReturnValue;
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
int BuildRouteValid(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
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

	if(pNewPlot->isImpassable() || pNewPlot->isMountain())
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
int AreaValid(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
	if(parent == NULL)
	{
		return TRUE;
	}

	CvMap& kMap = GC.getMap();
	if(kMap.plotUnchecked(parent->m_iX, parent->m_iY)->isImpassable() != kMap.plotUnchecked(node->m_iX, node->m_iY)->isImpassable())
	{
		return FALSE;
	}

	return ((kMap.plotUnchecked(parent->m_iX, parent->m_iY)->isWater() == kMap.plotUnchecked(node->m_iX, node->m_iY)->isWater()) ? TRUE : FALSE);
}


//	--------------------------------------------------------------------------------
/// Area path finder - callback routine when node added to open/closed list
int JoinArea(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
	if(data == ASNL_ADDCLOSED)
	{
		GC.getMap().plotUnchecked(node->m_iX, node->m_iY)->setArea(finder->GetInfo());
	}

	return 1;
}


//	--------------------------------------------------------------------------------
/// Area path finder - check validity of a coordinate
int LandmassValid(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
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
int JoinLandmass(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
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
void CvTwoLayerPathFinder::Initialize(int iColumns, int iRows, bool bWrapX, bool bWrapY, CvAPointFunc IsPathDestFunc, CvAPointFunc DestValidFunc, CvAHeuristic HeuristicFunc, CvAStarFunc CostFunc, CvAStarFunc ValidFunc, CvAStarFunc NotifyChildFunc, CvAStarFunc NotifyListFunc, CvABegin InitializeFunc, CvAEnd UninitializeFunc, const void* pData)
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
		if((pNode->m_pParent == NULL) || (pNode->m_iData2 == 1))
		{
			return GC.getMap().plotUnchecked(pNode->m_iX, pNode->m_iY);
		}

		while(pNode->m_pParent != NULL)
		{
			if(pNode->m_pParent->m_iData2 == 1)
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
bool CvTwoLayerPathFinder::GenerateUnitPath(const CvUnit* pkUnit, int iXstart, int iYstart, int iXdest, int iYdest, int iInfo /*= 0*/, bool bReuse /*= false*/)
{
	if (pkUnit)
	{
		CvAssert(gDLL->IsGameCoreThread() || !gDLL->IsGameCoreExecuting());
		SetData(pkUnit);
		bool bResult = GeneratePath(iXstart, iYstart, iXdest, iYdest, iInfo, bReuse);
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			uint uiChecksum = CRC_INIT;
			// Loop through the nodes and make a checksum
			CvAStarNode* pNode = GetLastNode();

			// Starting at the end, loop until we find a plot from this owner
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

	if(pStartPlot == NULL || pEndPlot == NULL || pStartPlot->getArea() != pEndPlot->getArea())
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
			return pPathfinderNode->m_iData1;
		}
	}

	return -1;
}

//	--------------------------------------------------------------------------------
/// Check for existence of step path between two points
bool CvStepPathFinder::DoesPathExist(PlayerTypes ePlayer, PlayerTypes eEnemy, CvPlot* pStartPlot, CvPlot* pEndPlot)
{
	if(pStartPlot == NULL || pEndPlot == NULL || pStartPlot->getArea() != pEndPlot->getArea())
	{
		return false;
	}

	SetData(&eEnemy);
	return GeneratePath(pStartPlot->getX(), pStartPlot->getY(), pEndPlot->getX(), pEndPlot->getY(), ePlayer, false);
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
			if (eEnemy != NO_PLAYER && bLeaveEnemyTerritory)
			{
				// Loop until we leave enemy territory
				for (int i = 0; i < (iPathLen - iNumSteps) && currentPlot->getOwner() == eEnemy; i++)
				{
					if (pNode->m_pParent != NULL)
					{
						// Move to the previous plot on the path
						pNode = pNode->m_pParent;
						currentPlot = kMap.plotUnchecked(pNode->m_iX, pNode->m_iY);
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

//	--------------------------------------------------------------------------------
/// Check for existence of step path between two points
bool CvIgnoreUnitsPathFinder::DoesPathExist(CvUnit& unit, CvPlot* pStartPlot, CvPlot* pEndPlot)
{
	m_pCurNode = NULL;

	if(pStartPlot == NULL || pEndPlot == NULL)
	{
		return false;
	}

	SetData(&unit);
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
		iPathDistance = pNode->m_iData2;
	}

	return iPathDistance;
}

//	--------------------------------------------------------------------------------
/// Returns the last plot along the step path owned by a specific player
CvPlot* CvIgnoreUnitsPathFinder::GetLastOwnedPlot(CvPlot* pStartPlot, CvPlot* pEndPlot, PlayerTypes iOwner) const
{
	CvAStarNode* pNode;

	// Generate path
	if(GC.getIgnoreUnitsPathFinder().GeneratePath(pStartPlot->getX(), pStartPlot->getX(), pEndPlot->getX(), pEndPlot->getX(), 0, false))
	{
		pNode = GC.getIgnoreUnitsPathFinder().GetLastNode();

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
	CvAStarNode* pNode;

	pNode = GC.getIgnoreUnitsPathFinder().GetLastNode();

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
	CvAStarNode* pNode;

	pNode = GC.getIgnoreUnitsPathFinder().GetLastNode();

	if(NULL != pNode)
	{
		CvMap& kMap = GC.getMap();
		if((pNode->m_pParent == NULL) || (pNode->m_iData2 == 1))
		{
			return kMap.plotUnchecked(pNode->m_iX, pNode->m_iY);
		}

		while(pNode->m_pParent != NULL)
		{
			if(pNode->m_pParent->m_iData2 == 1)
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
/// Get final plot on path [should be used after a call to DoesPathExist()]
CvPlot* CvIgnoreUnitsPathFinder::GetLastPlot()
{
	CvPlot* pPlot = NULL;

	CvAStarNode* pNode = GC.getIgnoreUnitsPathFinder().GetLastNode();
	if(pNode != NULL)
	{
		pPlot = GC.getMap().plot(pNode->m_iX, pNode->m_iY);

		// Save off node for future calls to GetPreviousPlot()
		m_pCurNode = pNode;
	}

	return pPlot;
}

//	--------------------------------------------------------------------------------
/// Get final plot on path [should be used after a call to DoesPathExist()]
CvPlot* CvIgnoreUnitsPathFinder::GetPreviousPlot()
{
	CvPlot* pPlot = NULL;

	if(m_pCurNode != NULL)
	{
		m_pCurNode = m_pCurNode->m_pParent;

		if(m_pCurNode != NULL)
		{
			pPlot = GC.getMap().plot(m_pCurNode->m_iX, m_pCurNode->m_iY);
		}
	}

	return pPlot;
}

//	--------------------------------------------------------------------------------
/// UI path finder - check validity of a coordinate
int UIPathValid(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
	if (parent == NULL)
	{
		return TRUE;
	}

	if(node->m_iData2 > 3)
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
					return FALSE;
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
		if(node->m_iData2 < 2 /*&& node->m_eCvAStarListType == NO_CVASTARLIST*/)
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
	if(node && node->m_iData2 < 2)
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
int AttackPathDestEval(int iToX, int iToY, const void* pointer, CvAStar* finder, bool bOnlyFortified, bool bOnlyCity)
{
	CvUnit* pUnit = ((CvUnit*)pointer);
	CvAssertMsg(pUnit, "pUnit should be a value");
	CvPlot* pPlot = GC.getMap().plot(iToX, iToY);
	CvAStarNode* pNode = finder->GetLastNode();

	if(pPlot->isVisible(pUnit->getTeam()) && (pPlot->isVisibleEnemyUnit(pUnit) || pPlot->isEnemyCity(*pUnit)) && pNode && pNode->m_iData2 < 2)
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
int AttackPathDest(int iToX, int iToY, const void* pointer, CvAStar* finder)
{
	return AttackPathDestEval(iToX, iToY, pointer, finder, false, false);
}

//	--------------------------------------------------------------------------------
/// Destination is valid if there is a fortified unit there
int AttackFortifiedPathDest(int iToX, int iToY, const void* pointer, CvAStar* finder)
{
	return AttackPathDestEval(iToX, iToY, pointer, finder, true, false);
}

//	--------------------------------------------------------------------------------
/// Destination is valid if there is a city there
int AttackCityPathDest(int iToX, int iToY, const void* pointer, CvAStar* finder)
{
	return AttackPathDestEval(iToX, iToY, pointer, finder, false, true);
}

//	---------------------------------------------------------------------------
int TacticalAnalysisMapPathValid(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
	CvMap& theMap = GC.getMap();

	CvPlot* pToPlot = theMap.plotUnchecked(node->m_iX, node->m_iY);
	PREFETCH_FASTAR_CVPLOT(reinterpret_cast<char*>(pToPlot));

	CvUnit* pUnit = ((CvUnit *)pointer);
	const UnitPathCacheData* pCacheData = reinterpret_cast<const UnitPathCacheData*>(finder->GetScratchBuffer());

	TeamTypes eUnitTeam = pCacheData->getTeam();
	PlayerTypes unit_owner = pCacheData->getOwner();
	CvAssertMsg(eUnitTeam != NO_TEAM, "The unit's team should be a vaild value");
	if (eUnitTeam == NO_TEAM)
	{
		eUnitTeam = GC.getGame().getActiveTeam();
	}

	CvTeam& kUnitTeam = GET_TEAM(eUnitTeam);

	CvTacticalAnalysisMap* pTAMap = GC.getGame().GetTacticalAnalysisMap();
	FAssert(pTAMap != NULL);
	CvTacticalAnalysisCell* pToPlotCell = pTAMap->GetCell(pToPlot->GetPlotIndex());
	FAssert(pToPlotCell != NULL);

	// Cache the data for the node
	CvPathNodeCacheData& kToNodeCacheData = node->m_kCostCacheData;
	kToNodeCacheData.bPlotVisibleToTeam = pToPlotCell->IsVisible();
	kToNodeCacheData.iNumFriendlyUnitsOfType = pToPlot->getNumFriendlyUnitsOfType(pUnit);
	kToNodeCacheData.bIsMountain = pToPlot->isMountain();
	kToNodeCacheData.bIsWater = pToPlotCell->IsWater();
	kToNodeCacheData.bCanEnterTerrain = pUnit->canEnterTerrain(*pToPlot, CvUnit::MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE);
	kToNodeCacheData.bIsRevealedToTeam = pToPlotCell->IsRevealed();
	kToNodeCacheData.bContainsOtherFriendlyTeamCity = false;
	if(pToPlotCell->IsCity())
	{
		CvCity* pCity = pToPlot->getPlotCity();
		if(pCity)
		{
			if(unit_owner != pCity->getOwner() && !kUnitTeam.isAtWar(pCity->getTeam()))
				kToNodeCacheData.bContainsOtherFriendlyTeamCity = true;
		}
	}
	kToNodeCacheData.bContainsEnemyCity = pToPlotCell->IsEnemyCity();
	kToNodeCacheData.bContainsVisibleEnemy = pToPlotCell->GetEnemyMilitaryUnit() != NULL;
	kToNodeCacheData.bContainsVisibleEnemyDefender = pToPlot->getBestDefender(NO_PLAYER, unit_owner, pUnit).pointer() != NULL;

	// If this is the first node in the path, it is always valid (starting location)
	if (parent == NULL)
	{
		return TRUE;
	}

	CvPlot* pFromPlot = theMap.plotUnchecked(parent->m_iX, parent->m_iY);
	PREFETCH_FASTAR_CVPLOT(reinterpret_cast<char*>(pFromPlot));

	CvTacticalAnalysisCell* pFromPlotCell = pTAMap->GetCell(pFromPlot->GetPlotIndex());
	FAssert(pFromPlotCell != NULL);

	bool bAIControl = pUnit->IsAutomated();

	// pulling invariants out of the loop
	int iUnitX = pUnit->getX();
	int iUnitY = pUnit->getY();
	DomainTypes unit_domain_type = pCacheData->getDomainType();
	bool bUnitIsCombat           = pUnit->IsCombatUnit();
	bool bIsHuman				 = pCacheData->isHuman();
	int iFinderInfo              = finder->GetInfo();
	CvPlot* pUnitPlot            = pUnit->plot();
	int iFinderIgnoreStacking    = iFinderInfo & MOVE_IGNORE_STACKING;
	int iUnitPlotLimit           = GC.getPLOT_UNIT_LIMIT();
	bool bFromPlotOwned          = !pFromPlotCell->IsUnclaimedTerritory();
	TeamTypes eFromPlotTeam      = pFromPlot->getTeam();

	// We have determined that this node is not the origin above (parent == NULL)
	CvAStarNode* pNode = node;
	bool bPreviousNodeHostile = false;
	int iDestX = finder->GetDestX();
	int iDestY = finder->GetDestY();
	int iNodeX = node->m_iX;
	int iNodeY = node->m_iY;
	int iOldNumTurns = -1;
	int iNumTurns;
	TeamTypes eTeam = eUnitTeam; // this may get modified later is eTEam == NO_TEAM

	// First run special case for checking "node" since it doesn't have a parent set yet
	bool bFirstRun = true;

	// Have to calculate this specially because the node passed into this function doesn't yet have data stored it in (hasn't reached pathAdd yet)
	int iStartMoves = parent->m_iData1;
	iNumTurns = parent->m_iData2;

	if(iStartMoves == 0)
	{
		iNumTurns++;
	}

	iOldNumTurns = -1;

	CvPlot* pPlot = NULL;

	// Get a reference to the parent node cache data
	CvPathNodeCacheData& kFromNodeCacheData = parent->m_kCostCacheData;

	// Loop through the current path until we find the path origin.
	// This validates the path with the inclusion of the new path node.  We must do this because of the rules of where a unit can finish a turn.
	// Please note that this can be an expensive loop as the path gets longer and longer, do as little work as possible in validating each node.  
	// If there is an invariant value that needs to be fetched from the plot or unit for the node, please do the calculation and put it in the node's data cache.
	while(pNode != NULL)
	{
		PREFETCH_FASTAR_NODE(pNode->m_pParent);

		CvPathNodeCacheData& kNodeCacheData = pNode->m_kCostCacheData;
		// This is a safeguard against the algorithm believing a plot to be impassable before actually knowing it (mid-search)
		if(iOldNumTurns != -1 || (iDestX == iNodeX && iDestY == iNodeY))
		{
			// This plot is of greater distance than previously, so we know the unit is ending its turn here (pNode), or it's trying to attack through a unit (and might end up on this tile if an attack fails to kill the enemy)
			if(iNumTurns != iOldNumTurns || bPreviousNodeHostile)
			{
				// Don't count origin, or else a unit will block its own movement!
				if(iNodeX != iUnitX || iNodeY != iUnitY)
				{
					// PREFETCH_FASTAR_CVPLOT(reinterpret_cast<char*>(pPlot));
					if(kNodeCacheData.bPlotVisibleToTeam)
					{
						// Check to see if any units are present at this full-turn move plot... if the player can see what's there
						if(kNodeCacheData.iNumFriendlyUnitsOfType >= iUnitPlotLimit && !(iFinderIgnoreStacking))
						{
							return FALSE;
						}

						if (kNodeCacheData.bIsMountain && !(iFinderIgnoreStacking) && (!bIsHuman || bAIControl))
						{
							return FALSE;
						}

						if(kNodeCacheData.bIsMountain && !kNodeCacheData.bCanEnterTerrain)	// only doing canEnterTerrain on mountain plots because it is expensive, though it probably should always be called and some other checks in this loop could be removed.
						{
							return FALSE;
						}

						if ((finder->GetInfo() & CvUnit::MOVEFLAG_STAY_ON_LAND) && kNodeCacheData.bIsWater)
						{
							return FALSE;
						}
					}

					if(kNodeCacheData.bIsRevealedToTeam)
					{
						if (kNodeCacheData.bContainsOtherFriendlyTeamCity && !(iFinderIgnoreStacking))
							return FALSE;
					}
				}
			}
		}

		bPreviousNodeHostile = false;
		if(kNodeCacheData.bContainsEnemyCity)
		{
			bPreviousNodeHostile = true;
		}
		// Prevents units from passing through one another on its way to attack another unit
		else if(kNodeCacheData.bContainsVisibleEnemy)
		{
			// except when attacking an unguarded civilian unit
			if(kNodeCacheData.bContainsVisibleEnemyDefender)
			{
				bPreviousNodeHostile = true;
			}
		}

		// JON - Special case for the original node passed into this function because it's not yet linked to any parent
		if(pNode == node && bFirstRun)
		{
			pNode = parent;
			bFirstRun = false;
		}
		else
		{
			pNode = pNode->m_pParent;
		}

		if(pNode != NULL)
		{

			iNodeX = pNode->m_iX;
			iNodeY = pNode->m_iY;
			iOldNumTurns = iNumTurns;
			iNumTurns = pNode->m_iData2;
		}
	}

	// slewis - moved this up so units can't move directly into the water. Not 100% sure this is the right solution.
	if(unit_domain_type == DOMAIN_LAND)
	{
		if(!kFromNodeCacheData.bIsWater && kToNodeCacheData.bIsWater && kToNodeCacheData.bIsRevealedToTeam && !pUnit->canEmbarkOnto(*pFromPlot, *pToPlot, true))
		{
			if(!pUnit->IsHoveringUnit() && !pUnit->canMoveAllTerrain())
			{
				return FALSE;
			}
		}
	}

	if(!bUnitIsCombat && unit_domain_type != DOMAIN_AIR)
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

	// slewis - Added to catch when the unit is adjacent to an enemy unit while it is stacked with a friendly unit.
	//          The logic above (with bPreviousNodeHostile) catches this problem with a path that's longer than one step
	//          but does not catch when the path is only one step.
	if(bUnitIsCombat && unit_domain_type != DOMAIN_AIR && pUnitPlot->isAdjacent(pToPlot) && kToNodeCacheData.bContainsVisibleEnemy && !(iFinderIgnoreStacking))
	{
		if(kToNodeCacheData.bContainsVisibleEnemyDefender)
		{
			if(pUnitPlot->getNumFriendlyUnitsOfType(pUnit) > iUnitPlotLimit)
			{
				return FALSE;
			}
		}
	}

	if(pUnitPlot == pFromPlot)
	{
		return TRUE;
	}

	if(iFinderInfo & MOVE_TERRITORY_NO_UNEXPLORED)
	{
		if(!kFromNodeCacheData.bIsRevealedToTeam)
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

	if(bAIControl)
	{
		if((parent->m_iData2 > 1) || (parent->m_iData1 == 0))
		{
			if(!(iFinderInfo & MOVE_UNITS_IGNORE_DANGER))
			{
				if(!bUnitIsCombat || pUnit->getArmyID() == FFreeList::INVALID_INDEX)
				{
					if(GET_PLAYER(unit_owner).GetPlotDanger(*pFromPlot) > 0)
					{
						return FALSE;
					}
				}
			}
		}
	}

	// slewis - added AI check and embark check to prevent units from moving into unexplored areas
	if(bAIControl || !bIsHuman || kFromNodeCacheData.bIsRevealedToTeam || pCacheData->isEmbarked())
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

//	---------------------------------------------------------------------------
int FindValidDestinationDest(int iToX, int iToY, const void* pointer, CvAStar* finder)
{
	CvUnit* pUnit = ((CvUnit*)pointer);
	CvPlot* pToPlot = GC.getMap().plotUnchecked(iToX, iToY);

	if(pToPlot->getNumFriendlyUnitsOfType(pUnit) >= GC.getPLOT_UNIT_LIMIT())
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
int FindValidDestinationPathValid(CvAStarNode* parent, CvAStarNode* node, int data, const void* pointer, CvAStar* finder)
{
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

	if(node->m_iData2 > 3)
	{
		return FALSE;
	}

	return TRUE;
}

//	--------------------------------------------------------------------------------
/// Can a unit reach this destination in "X" turns of movement (pass in 0 if need to make it in 1 turn with movement left)?
// ***
// *** WARNING - The optimization below (so that TurnsToReachTarget() doesn't get called too often breaks down when we get to RR.  We need to address this!
// ***
bool CanReachInXTurns(UnitHandle pUnit, CvPlot* pTarget, int iTurns, bool bIgnoreUnits, int* piTurns /* = NULL */)
{
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
}

//	--------------------------------------------------------------------------------
/// How many turns will it take a unit to get to a target plot (returns MAX_INT if can't reach at all; returns 0 if makes it in 1 turn and has movement left)
// Should call it with bIgnoreStacking true if want foolproof way to see if can make it in 0 turns (since that way doesn't open
// open the 2nd layer of the pathfinder)
int TurnsToReachTarget(UnitHandle pUnit, CvPlot* pTarget, bool bReusePaths, bool bIgnoreUnits, bool bIgnoreStacking)
{
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
			GC.getIgnoreUnitsPathFinder().SetData(pUnit.pointer());
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

			bool bSuccess;

			GC.GetTacticalAnalysisMapFinder().SetData(pUnit.pointer());
			bSuccess = GC.GetTacticalAnalysisMapFinder().GeneratePath(pUnit->getX(), pUnit->getY(), pTarget->getX(), pTarget->getY(), iFlags, bReusePaths);
			if(bSuccess)
			{
				pNode = GC.GetTacticalAnalysisMapFinder().GetLastNode();
			}
		}

		if(pNode)
		{
			rtnValue = pNode->m_iData2;
			if(rtnValue == 1)
			{
				if(pNode->m_iData1 > 0)
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
			if (kNode.m_iData2 == iTurn)
				return &kNode;
			return NULL;
		}
		else
		{
			// Is this node the correct turn and the next node is a turn after it?
			if (kNode.m_iData2 == iTurn && at(i-1).m_iData2 > iTurn)
				return &kNode;
		}
	}

	return NULL;
}
