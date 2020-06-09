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
#include <numeric>
#include "LintFree.h"

//PATH_BASE_COST is defined in AStar.h (value 100) - a simple moves costs 6000!
#define PATH_ATTACK_WEIGHT										(200)	//per percent penalty on attack
#define PATH_DEFENSE_WEIGHT										(20)	//per percent defense bonus on turn end plot
#define PATH_STEP_WEIGHT										(10)	//relatively small
#define	PATH_EXPLORE_NON_HILL_WEIGHT							(1000)	//per hill plot we fail to visit
#define PATH_EXPLORE_NON_REVEAL_WEIGHT							(1000)	//per (neighboring) plot we fail to reveal
#define PATH_BUILD_ROUTE_REUSE_EXISTING_WEIGHT					(20)	//accept four plots detour to save on maintenance
#define PATH_END_TURN_FOREIGN_TERRITORY							(PATH_BASE_COST*10)		//per turn end plot outside of our territory
#define PATH_END_TURN_NO_ROUTE									(PATH_BASE_COST*10)		//when in doubt, prefer to end the turn on a plot with a route
#define PATH_END_TURN_WATER										(PATH_BASE_COST*55)		//embarkation should be avoided (land units only)
#define PATH_END_TURN_INVISIBLE_WEIGHT							(PATH_BASE_COST*10)		//when in doubt, prefer routes through visible areas
#define PATH_END_TURN_LOW_DANGER_WEIGHT							(PATH_BASE_COST*45)		//one of these is worth 1.5 plots of detour (more for faster units)
#define PATH_END_TURN_HIGH_DANGER_WEIGHT						(PATH_BASE_COST*75)		//one of these is worth 2.5 plots of detour (more for faster units)
#define PATH_END_TURN_MORTAL_DANGER_WEIGHT						(PATH_BASE_COST*115)	//one of these is worth 3.5 plots of detour (more for faster units)
#define PATH_END_TURN_MISSIONARY_OTHER_TERRITORY				(PATH_BASE_COST*155)	//don't make it even so we don't get ties
#define PATH_ASSUMED_MAX_DEFENSE								(100)	//MAX_DEFENSE * DEFENSE_WEIGHT + END_TURN_FOREIGN_TERRITORY + END_TURN_NO_ROUTE should be smaller than END_TURN_WATER
#define NORM_COST_BASE											(100)	//we use a fixed point format for normalized path cost

//for debugging
int giKnownCostWeight = 1;
int giHeuristicCostWeight = 1;
int giLastStartIndex = 0;

unsigned int saiRuntimeHistogram[100] = {0};

struct SLogNode
{
	SLogNode( NodeState _type, int _round, int _x, int _y, int _kc, int _hc, int _turns, int _moves ) : 
		type(_type), x(_x), y(_y), round(_round), kc(_kc), hc(_hc), t(_turns), m(_moves)
	{
		if (type == NS_INVALID)
		{
			kc = -1; hc = -1; t = -1; m = -1;
		}
	}
	int x, y, round, kc, hc, t, m;
	NodeState type; 

private:
	SLogNode();
};

std::vector<SLogNode> svPathLog;

//debugging help
void DumpNodeList(const std::vector<CvAStarNode*>& nodes)
{
	for (std::vector<CvAStarNode*>::const_iterator it=nodes.begin(); it!=nodes.end(); ++it)
	{
		CvAStarNode* pNode = *it;
		OutputDebugString( CvString::format("x %02d, y %02d, t %02d, m %03d, tc %d\n",pNode->m_iX,pNode->m_iY,pNode->m_iTurns,pNode->m_iMoves,pNode->m_iTotalCost).c_str() );
	}
}

//	--------------------------------------------------------------------------------
/// Constructor
CvAStar::CvAStar()
{
	m_iColumns = 0;
	m_iRows = 0;
	m_iXstart = 0;
	m_iYstart = 0;
	m_iXdest = 0;
	m_iYdest = 0;
	m_iDestHitCount = 0;

	m_bWrapX = false;
	m_bWrapY = false;
	m_bHeapDirty = false;

	udDestValid = NULL;
	udHeuristic = NULL;
	udCost = NULL;
	udValid = NULL;
	udGetExtraChildrenFunc = NULL;
	udInitializeFunc = NULL;
	udUninitializeFunc = NULL;

	m_pBest = NULL;
	m_ppaaNodes = NULL;
	m_ppaaNeighbors = NULL;

	m_iCurrentGenerationID = 0;
	m_iProcessedNodes = 0;
	m_iTestedNodes = 0;
	m_iRounds = 0;
	m_iBasicPlotCost = 0;
	m_iMovesCached = 0;
	m_iTurnsCached = 0;

	//for debugging
	m_strName = "AStar";

	//this matches the default setting for SPathFinderUserData
	SetFunctionPointers(StepDestValid, StepHeuristic, StepCost, StepValidAnyArea, NULL, NULL, NULL);

	InitializeCriticalSection(&m_cs);
}

//	--------------------------------------------------------------------------------
/// Destructor
CvAStar::~CvAStar()
{
	DeInit();
	DeleteCriticalSection(&m_cs);
}

//	--------------------------------------------------------------------------------
/// Frees allocated memory
void CvAStar::DeInit()
{
	m_openNodes.clear();
	m_closedNodes.clear();

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
void CvAStar::Initialize(int iColumns, int iRows, bool bWrapX, bool bWrapY)
{
	DeInit();	// free old memory just in case

	m_iColumns = iColumns;
	m_iRows = iRows;

	m_iXstart = -1;
	m_iYstart = -1;
	m_iXdest = -1;
	m_iYdest = -1;
	m_iDestHitCount = 0;

	m_bWrapX = bWrapX;
	m_bWrapY = bWrapY;
	m_bHeapDirty = false;

	m_pBest = NULL;

	m_iTestedNodes = 0;
	m_iProcessedNodes = 0;
	m_iRounds = 0;

	m_iBasicPlotCost = 1;

	m_ppaaNodes = reinterpret_cast<CvAStarNode**>(FMALLOCALIGNED(sizeof(CvAStarNode*)*m_iColumns, 64, c_eCiv5GameplayDLL, 0));
	for(int iI = 0; iI < m_iColumns; iI++)
	{
		m_ppaaNodes[iI] = reinterpret_cast<CvAStarNode*>(FMALLOCALIGNED(sizeof(CvAStarNode)*m_iRows, 64, c_eCiv5GameplayDLL, 0));
		for(int iJ = 0; iJ < m_iRows; iJ++)
		{
			new(&m_ppaaNodes[iI][iJ]) CvAStarNode();
			m_ppaaNodes[iI][iJ].m_iX = iI;
			m_ppaaNodes[iI][iJ].m_iY = iJ;
		}
	}

	m_ppaaNeighbors = new CvAStarNode*[m_iColumns*m_iRows*6];
	CvAStarNode** apNeighbors = m_ppaaNeighbors;

	for(int iI = 0; iI < m_iColumns; iI++)
	{
		for(int iJ = 0; iJ < m_iRows; iJ++)
		{
			m_ppaaNodes[iI][iJ].m_apNeighbors = apNeighbors;
			apNeighbors += 6;
			PrecalcNeighbors( GetNodeMutable(iI,iJ) );
		}
	}
}

bool CvAStar::IsInitialized(int iXstart, int iYstart, int iXdest, int iYdest)
{
	return (m_ppaaNodes != NULL) &&
		m_iColumns > iXstart &&
		m_iColumns > iXdest &&
		m_iRows > iYstart &&
		m_iRows > iYdest;
}

void CvAStar::SetFunctionPointers(CvAPointFunc DestValidFunc, CvAHeuristic HeuristicFunc, CvAStarConst1Func CostFunc, CvAStarConst2Func ValidFunc,  
						CvAGetExtraChildren GetExtraChildrenFunc, CvABegin InitializeFunc, CvAEnd UninitializeFunc)
{
	udDestValid = DestValidFunc;
	udHeuristic = HeuristicFunc;
	udCost = CostFunc;
	udValid = ValidFunc;
	udGetExtraChildrenFunc = GetExtraChildrenFunc;
	udInitializeFunc = InitializeFunc;
	udUninitializeFunc = UninitializeFunc;
}

void CvAStar::Reset()
{
	m_pBest = NULL;
	m_iDestHitCount = 0;

	//reset previously used nodes
	for (std::vector<CvAStarNode*>::iterator it=m_openNodes.begin(); it!=m_openNodes.end(); ++it)
		(*it)->clear();
	for (std::vector<CvAStarNode*>::iterator it=m_closedNodes.begin(); it!=m_closedNodes.end(); ++it)
		(*it)->clear();
	m_openNodes.clear();
	m_closedNodes.clear();

	//debug helpers
	m_iProcessedNodes = 0;
	m_iTestedNodes = 0;
	m_iRounds = 0;
	svPathLog.clear();
}

//	--------------------------------------------------------------------------------
// Generates a path from iXstart,iYstart to iXdest,iYdest
// private method - not threadsafe!
bool CvAStar::FindPathWithCurrentConfiguration(int iXstart, int iYstart, int iXdest, int iYdest, const SPathFinderUserData& data)
{
	if (data.ePathType != m_sData.ePathType)
		return false;

	if (!IsInitialized(iXstart, iYstart, iXdest, iYdest))
		return false;

	//this is the version number for the node cache
	m_iCurrentGenerationID++;
	if (m_iCurrentGenerationID==0xFFFF)
		m_iCurrentGenerationID = 1;

	m_sData = data;
	m_iXdest = iXdest;
	m_iYdest = iYdest;
	m_iXstart = iXstart;
	m_iYstart = iYstart;

	SanitizeFlags();
	Reset();

	if(!isValid(iXstart, iYstart))
		return false;

	if(udInitializeFunc)
		udInitializeFunc(m_sData, this);

	if(udDestValid && !udDestValid(iXdest, iYdest, m_sData, this))
	{
		if (udUninitializeFunc)
			udUninitializeFunc(m_sData, this);
		return false;
	}

	//set up first node
	NodeAddedToPath(NULL, GetNodeMutable(iXstart,iYstart), 0, ASNC_INITIALADD);

#if defined(MOD_CORE_DEBUGGING)
	cvStopWatch timer("pathfinder",NULL,0,true);
	timer.StartPerfTest();
	if (MOD_CORE_DEBUGGING)
	{
		svPathLog.push_back( SLogNode( NS_INITIAL_FINAL, 0, m_iXstart, m_iYstart, 0, 0, 0, 0 ) );
		svPathLog.push_back( SLogNode( NS_INITIAL_FINAL, 0, m_iXdest, m_iYdest, 0, 0, 0, 0 ) );
	}
#endif

	//here the magic happens
	bool bSuccess = false;
	while(1)
	{
		m_iRounds++;

		m_pBest = GetBest();

		if (m_pBest==NULL)
			//search exhausted
			break;
		else if (IsPathDest(m_pBest->m_iX,m_pBest->m_iY))
		{
			//we did it!
			bSuccess = true;
			break;
		}
		else if (m_iDestHitCount>2 && !IsApproximateMode())
		{
			//touched the target several times, but no success yet?
			//that's fishy. take what we have and don't waste any more time
			m_pBest = GetNodeMutable(iXdest,iYdest);
			bSuccess = true;
			break;
		}

		CreateChildren(m_pBest);
	}

#if defined(MOD_CORE_DEBUGGING)
	//debugging!
	timer.EndPerfTest();
	int iBin = min(99,int(timer.GetDeltaInSeconds()*1000));
	saiRuntimeHistogram[iBin]++;

	if ( timer.GetDeltaInSeconds()>0.2 && data.ePathType!=PT_UNIT_REACHABLE_PLOTS && data.ePathType!=PT_GENERIC_REACHABLE_PLOTS )
	{
		//debug hook
		int iStartIndex = GC.getMap().plotNum(m_iXstart, m_iYstart);
#if defined(VPDEBUG)
		if (iStartIndex==giLastStartIndex && iStartIndex>0)
		{
			OutputDebugString("Repeated pathfinding start\n");
			gStackWalker.ShowCallstack();
		}
#endif
		giLastStartIndex = iStartIndex;

		int iNumPlots = GC.getMap().numPlots();
		CvUnit* pUnit = m_sData.iUnitID>0 ? GET_PLAYER(m_sData.ePlayer).getUnit(m_sData.iUnitID) : NULL;

		//in some cases we have no destination plot, so exhaustion is not always a "fail"
		OutputDebugString( CvString::format("Run %d: Path type %d %s (%s from %d,%d to %d,%d - flags %d), tested %d, processed %d nodes in %d rounds (%d%% of map) in %.2f ms\n", 
			m_iCurrentGenerationID, m_sData.ePathType, bSuccess?"found":"not found", pUnit ? pUnit->getName().c_str() : "unknown",
			m_iXstart, m_iYstart, m_iXdest, m_iYdest, m_sData.iFlags, m_iTestedNodes, m_iProcessedNodes, m_iRounds, 
			(100*m_iProcessedNodes)/iNumPlots, timer.GetDeltaInSeconds()*1000 ).c_str() );

		if (MOD_CORE_DEBUGGING)
		{
			CvString fname = CvString::format( "PathfindingRun%06d.txt", m_iCurrentGenerationID );
			FILogFile* pLog=LOGFILEMGR.GetLog( fname.c_str(), FILogFile::kDontTimeStamp );
			if (pLog) 
			{
				if (pUnit)
				{
					pLog->Msg( CvString::format("# %s for %s (%d) from %d,%d to %d,%d for player %d, flags %d\n", 
						GetName(),pUnit->getName().c_str(),pUnit->GetID(),m_iXstart,m_iYstart,m_iXdest,m_iYdest,pUnit->getOwner(),m_sData.iFlags ).c_str() );
				}
				else
				{
					pLog->Msg( CvString::format("# %s from %d,%d to %d,%d for player %d, type %d, flags %d\n", 
						GetName(),m_iXstart,m_iYstart,m_iXdest,m_iYdest,m_sData.ePlayer,m_sData.ePathType,m_sData.iFlags ).c_str() );
				}

#ifdef STACKWALKER
				//gStackWalker.SetLog(pLog);
				//gStackWalker.ShowCallstack();
#endif

				for (size_t i=0; i<svPathLog.size(); i++)
					pLog->Msg( CvString::format("%d,%d,%d,%d,%d,%d,%d,%d\n", svPathLog[i].round,svPathLog[i].type,svPathLog[i].x,svPathLog[i].y,
						svPathLog[i].kc,svPathLog[i].hc,svPathLog[i].t,svPathLog[i].m ).c_str() );
			}
		}
	}
#endif

	if (udUninitializeFunc)
		udUninitializeFunc(m_sData, this);

	return bSuccess;
}

//	--------------------------------------------------------------------------------
/// Returns best node
CvAStarNode* CvAStar::GetBest()
{
	if (m_openNodes.empty())
		return NULL;

	//make sure heap order is valid after all the updates in the previous round
	if (m_bHeapDirty)
	{
		std::make_heap(m_openNodes.begin(),m_openNodes.end(),PrNodeIsBetter());
		m_bHeapDirty = false;
	}

	std::pop_heap(m_openNodes.begin(),m_openNodes.end(),PrNodeIsBetter());
	CvAStarNode* temp = m_openNodes.back();	m_openNodes.pop_back();

	//move the node to the closed list
	temp->m_bIsOpen = false;
	m_closedNodes.push_back(temp);
	return temp;
}

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
			node->m_apNeighbors[i] = GetNodeMutable(x,y);
		else
			node->m_apNeighbors[i] = NULL;
	}
}

void LogNodeAction(CvAStarNode* node, int iRound, NodeState state)
{
	node; iRound; state;
//#if defined(MOD_CORE_DEBUGGING)
//	if (MOD_CORE_DEBUGGING && svPathLog.size()<10000)
//		svPathLog.push_back(SLogNode(state, iRound, node->m_iX, node->m_iY, node->m_iKnownCost, node->m_iHeuristicCost, node->m_iTurns, node->m_iMoves));
//#endif
}

//	--------------------------------------------------------------------------------
/// Creates children for the node
void CvAStar::CreateChildren(CvAStarNode* node)
{
	LogNodeAction(node, m_iRounds, NS_CURRENT);

	//one is enough, theoretically we could add one for every neighbor
	bool bHaveStopNodeHere = false;

	//direct neighbors
	for(int i = 0; i < 6; i++)
	{
		CvAStarNode* check = node->m_apNeighbors[i];
		if (!check)
			continue;

		m_iTestedNodes++;
		if(udFunc(udValid, node, check, m_sData))
		{
			NodeState result = LinkChild(node, check);
			
			if (result==NS_VALID)
			{
				m_iProcessedNodes++;

				//keep track of how often we've come close to the destination
				if (IsPathDest(check->m_iX, check->m_iY))
					m_iDestHitCount++;
			}

			LogNodeAction(node, m_iRounds, result);

			//if we are doing unit pathfinding, maybe we need to do a voluntary stop on the parent node
			if (!bHaveStopNodeHere)
				bHaveStopNodeHere = AddStopNodeIfRequired(node,check);
		}
	}

	//wormholes, eg harbors
	if(udGetExtraChildrenFunc)
	{
		vector<pair<int, int>> extraChildren;
		GetExtraChildren(node,extraChildren);
		for(size_t i = 0; i < extraChildren.size(); i++)
		{
			int x = extraChildren[i].first;
			int y = extraChildren[i].second;

			if (isValid(x, y))
			{
				CvAStarNode* check = GetNodeMutable(x,y);
				if (!check || check == node->m_pParent)
					continue;

				if (udFunc(udValid, node, check, m_sData))
				{
					NodeState result = LinkChild(node, check);

					if (result == NS_VALID)
						m_iProcessedNodes++;

					LogNodeAction(node, m_iRounds, result);
				}

				if (IsPathDest(check->m_iX, check->m_iY))
					m_iDestHitCount++;
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Link in a child
NodeState CvAStar::LinkChild(CvAStarNode* node, CvAStarNode* check)
{
	//we would have to start a new turn to continue
	if(node->m_iMoves == 0)
		if (node->m_iTurns+1 >= m_sData.iMaxTurns) // path is getting too long ...
			return NS_FORBIDDEN;

	//seems innocent, but is very important
	int iKnownCost = udFunc(udCost, node, check, m_sData);

	//invalid cost function
	if (iKnownCost < 0)
		return NS_FORBIDDEN;

	//calculate the cumulative cost up to here
	iKnownCost += node->m_iKnownCost;

	//check termination because of total cost / normalized length - could use total cost here if heuristic is admissible
	if (m_sData.iMaxNormalizedDistance!= INT_MAX && iKnownCost > m_sData.iMaxNormalizedDistance*m_iBasicPlotCost)
		return NS_FORBIDDEN;

	//final check. there may have been a previous path here.
	//in that case we want to keep the one with the lower total cost, which should correspond to the shorter one
	if (check->m_iKnownCost > 0 && iKnownCost >= check->m_iKnownCost)
		return NS_OBSOLETE;

	//safety check for loops. compare coords in case of two layer pathfinder
	if (node->m_pParent && node->m_pParent->m_iX == check->m_iX && node->m_pParent->m_iY == check->m_iY)
		return NS_OBSOLETE;

	//remember the connection
	node->m_apChildren.push_back(check);

	//is the new node already on the open list? update it
	if(check->m_bIsOpen)
	{
		if(iKnownCost < check->m_iKnownCost)
		{
			//heap order will be restored when calling getBest
			m_bHeapDirty = true;
			NodeAddedToPath(node, check, iKnownCost, ASNC_OPENADD_UP);
		}
	}
	//is the new node on the closed list?
	else if(check->m_iTotalCost > -1)
	{
		if(iKnownCost < check->m_iKnownCost)
		{
			NodeAddedToPath(node, check, iKnownCost, ASNC_CLOSEDADD_UP);
			UpdateDownstreamNodes(check);
		}
	}
	//new node is previously untouched
	else
	{
		NodeAddedToPath(node, check, iKnownCost, ASNC_NEWADD);
	}
	
	return NS_VALID;
}

//  --------------------------------------------------------------------------------
int CvAStar::GetExtraChildren(const CvAStarNode* node, vector<pair<int,int>>& out) const
{
	if (udGetExtraChildrenFunc)
	{
		udGetExtraChildrenFunc(node, this, out);
		return (int)out.size();
	}

	return 0;
}

//	--------------------------------------------------------------------------------
/// Add node to open list
void CvAStar::AddToOpen(CvAStarNode* addnode)
{
	addnode->m_bIsOpen = true;
	m_openNodes.push_back(addnode);
	std::push_heap(m_openNodes.begin(),m_openNodes.end(),PrNodeIsBetter());
}

const CvAStarNode * CvAStar::GetNode(int iCol, int iRow) const
{
	return &(m_ppaaNodes[iCol][iRow]);
}

CvAStarNode * CvAStar::GetNodeMutable(int iCol, int iRow) const
{
	return &(m_ppaaNodes[iCol][iRow]);
}

//	--------------------------------------------------------------------------------
/// Refresh parent node (after linking in a child)
void CvAStar::UpdateDownstreamNodes(CvAStarNode* node)
{
	//simple bfs
	std::vector<CvAStarNode*> storedNodes;

	CvAStarNode* parent = node;
	while(parent != NULL)
	{
		for(size_t i = 0; i < parent->m_apChildren.size(); i++)
		{
			CvAStarNode* kid = parent->m_apChildren[i];

			int iKnownCost = (parent->m_iKnownCost + udFunc(udCost, parent, kid, m_sData));

			if(iKnownCost < kid->m_iKnownCost)
			{
				//heap order will be restored when calling getBest()
				if (kid->m_bIsOpen)
					m_bHeapDirty = true;

				//also need to check closed nodes, they might have open children!
				NodeAddedToPath(parent, kid, iKnownCost, ASNC_PARENTADD_UP);

				if (std::find(storedNodes.begin(),storedNodes.end(),kid) == storedNodes.end())
					storedNodes.push_back(kid);
			}
		}

		if (storedNodes.empty())
			parent = NULL;
		else
		{
			parent = storedNodes.back();
			storedNodes.pop_back();
		}
	}
}

//	--------------------------------------------------------------------------------
/// Get the whole path
SPath CvAStar::GetCurrentPath(TurnCountMode eMode) const
{
	SPath ret;
	ret.iTurnSliceGenerated = GC.getGame().getTurnSlice();
	ret.sConfig = m_sData;

	CvAStarNode* pNode = m_pBest;
	if (!pNode)
	{
		return ret;
	}

	ret.iTotalCost = pNode->m_iKnownCost;
	ret.iNormalizedDistanceRaw = (pNode->m_iKnownCost * SPath::getNormalizedDistanceBase()) / m_iBasicPlotCost + 1;
	//switch counting convention. if zero moves left, consider this as plus one turns
	ret.iTotalTurns = pNode->m_iTurns + (pNode->m_iMoves==0 ? 1 : 0);

	//walk backwards ...
	while(pNode != NULL)
	{
		//failsafe
		if (pNode == pNode->m_pParent)
			break;
		//another failsafe - turns should be decreasing! if not we have a loop
		if (!ret.vPlots.empty() && pNode->m_iTurns > ret.vPlots.back().turns)
			break;

		//default mode TC_GAMECORE
		ret.vPlots.push_back(SPathNode(pNode));

		//switch counting convention. if zero moves left, consider this as plus one turns
		if (eMode==TC_UI && ret.vPlots.back().moves == 0)
			ret.vPlots.back().turns++;
		//overwrite the turns with the costs
		else if (eMode==TC_DEBUG)
			ret.vPlots.back().turns = (short)pNode->m_iKnownCost;

		pNode = pNode->m_pParent;
	}

	//make it so that the destination comes last
	std::reverse(ret.vPlots.begin(),ret.vPlots.end());
	return ret;
}

//	--------------------------------------------------------------------------------
/// check if a stored path is still viable
bool CvAStar::VerifyPath(const SPath& path)
{
	bool bHadLock = gDLL->HasGameCoreLock();
	if(!bHadLock)
		gDLL->GetGameCoreLock();

	//set the right config
	if (!Configure(path.sConfig))
	{
		if(!bHadLock)
			gDLL->ReleaseGameCoreLock();
		return false;
	}

	//a single plot is always valid
	if (path.vPlots.size()<2)
	{
		if(!bHadLock)
			gDLL->ReleaseGameCoreLock();
		return false;
	}

	m_sData = path.sConfig;
	if (udInitializeFunc)
		udInitializeFunc(m_sData,this);

	bool bResult = true;
	int iKnownCost = 0;
	for (size_t i=1; i<path.vPlots.size(); i++)
	{
		CvAStarNode& current = m_ppaaNodes[ path.vPlots[i-1].x ][ path.vPlots[i-1].y ];
		CvAStarNode& next = m_ppaaNodes[ path.vPlots[i].x ][ path.vPlots[i].y ];

		if ( udFunc(udValid, &current, &next, m_sData) )
		{
			iKnownCost += udFunc(udCost, &current, &next, m_sData);
			if (iKnownCost > path.iTotalCost)
			{
				bResult = false;
				break;
			}
		}
		else
		{
			bResult = false;
			break;
		}
	}

	if (udUninitializeFunc)
		udUninitializeFunc(m_sData,this);

	if(!bHadLock)
		gDLL->ReleaseGameCoreLock();
	return bResult;
}

//C-STYLE NON-MEMBER FUNCTIONS

//-------------------------------------------------------------------------------------
// A structure holding some unit values that are invariant during a path plan operation
struct UnitPathCacheData
{
	CvUnit* pUnit;

	int m_aBaseMoves[NUM_DOMAIN_TYPES];
	PlayerTypes m_ePlayerID;
	TeamTypes m_eTeamID;
	DomainTypes m_eDomainType;

	bool m_bAIControl;
	bool m_bIsNoRevealMap;
	bool m_bCanEverEmbark;
	bool m_bIsEmbarked;
	bool m_bCanAttack;
	bool m_bDoDanger;

	inline int baseMoves(DomainTypes eType) const { return m_aBaseMoves[eType]; }
	inline PlayerTypes getOwner() const { return m_ePlayerID; }
	inline TeamTypes getTeam() const { return m_eTeamID; }
	inline DomainTypes getDomainType() const { return m_eDomainType; }
	inline bool isAIControl() const { return m_bAIControl; }
	inline bool isNoRevealMap() const { return m_bIsNoRevealMap; }
	inline bool CanEverEmbark() const { return m_bCanEverEmbark; }
	inline bool isEmbarked() const { return m_bIsEmbarked; }
	inline bool IsCanAttack() const { return m_bCanAttack; }
	inline bool doDanger() const { return m_bDoDanger; }
};

//-------------------------------------------------------------------------------------
// get all information which is constant during a path planning operation
void UnitPathInitialize(const SPathFinderUserData& data, CvAStar* finder)
{
	UnitPathCacheData* pCacheData = reinterpret_cast<UnitPathCacheData*>(finder->GetScratchBufferDirty());

	CvUnit* pUnit = GET_PLAYER(data.ePlayer).getUnit(data.iUnitID);
	pCacheData->pUnit = pUnit;

	for (int i = 0; i < NUM_DOMAIN_TYPES; ++i)
		pCacheData->m_aBaseMoves[i] = pUnit->baseMoves((DomainTypes)i);

	pCacheData->m_ePlayerID = pUnit->getOwner();
	pCacheData->m_eTeamID = pUnit->getTeam();
	pCacheData->m_eDomainType = pUnit->getDomainType();
	pCacheData->m_bAIControl = !pUnit->isHuman() || pUnit->IsAutomated();
	pCacheData->m_bIsNoRevealMap = pUnit->isNoRevealMap();
	pCacheData->m_bCanEverEmbark = pUnit->CanEverEmbark();
	pCacheData->m_bIsEmbarked = pUnit->isEmbarked();
	pCacheData->m_bCanAttack = pUnit->IsCanAttack() && !pUnit->isOutOfAttacks();
	pCacheData->m_bDoDanger = !finder->HaveFlag(CvUnit::MOVEFLAG_IGNORE_DANGER);
}

//	--------------------------------------------------------------------------------
void UnitPathUninitialize(const SPathFinderUserData&, CvAStar*)
{

}

//-------------------------------------------------------------------------------------
// get all information which depends on a particular node. 
// this is versioned, so we don't need to recalculate during the same pathfinding operation
void UpdateNodeCacheData(CvAStarNode* node, const CvUnit* pUnit, const CvAStar* finder)
{
	if (!node || !pUnit)
		return;

	const UnitPathCacheData* pCacheData = reinterpret_cast<const UnitPathCacheData*>(finder->GetScratchBuffer());
	CvPathNodeCacheData& kToNodeCacheData = node->m_kCostCacheData;
	if (kToNodeCacheData.iGenerationID==finder->GetCurrentGenerationID())
		return;

	const CvPlot* pPlot = GC.getMap().plotUnchecked(node->m_iX, node->m_iY);
	TeamTypes eUnitTeam = pUnit->getTeam();
	CvTeam& kUnitTeam = GET_TEAM(eUnitTeam);
	TeamTypes ePlotTeam = pPlot->getTeam();

	kToNodeCacheData.bIsRevealedToTeam = pPlot->isRevealed(eUnitTeam) || (finder->HaveFlag(CvUnit::MOVEFLAG_PRETEND_ALL_REVEALED));
	kToNodeCacheData.bPlotVisibleToTeam = pPlot->isVisible(eUnitTeam);
	kToNodeCacheData.bIsNonNativeDomain = !pUnit->isNativeDomain(pPlot); 
	kToNodeCacheData.bIsValidRoute = pPlot->isValidRoute(pUnit);

	kToNodeCacheData.bIsNonEnemyCity = false;
	kToNodeCacheData.bIsEnemyCity = false;
	if (kToNodeCacheData.bIsRevealedToTeam)
	{
		CvCity* pCity = pPlot->getPlotCity();
		if (pCity  && pUnit->getOwner() != pCity->getOwner())
		{
			if (kUnitTeam.isAtWar(pCity->getTeam()))
				kToNodeCacheData.bIsEnemyCity = true;
			else
				kToNodeCacheData.bIsNonEnemyCity = true;
		}
	}

	kToNodeCacheData.bIsVisibleEnemyUnit = false;
	kToNodeCacheData.bIsVisibleEnemyCombatUnit = false;
	kToNodeCacheData.bIsVisibleNeutralCombatUnit = false;
	kToNodeCacheData.bUnitStackingLimitReached = false;

	bool bPlotOccupancyOverride = false;
	if (kToNodeCacheData.bPlotVisibleToTeam)
	{
		if (finder->HaveFlag(CvUnit::MOVEFLAG_SELECTIVE_ZOC))
		{
			const PlotIndexContainer& ignorePlots = finder->GetData().plotsToIgnoreForZOC;
			bPlotOccupancyOverride = ( std::find(ignorePlots.begin(),ignorePlots.end(),pPlot->GetPlotIndex()) != ignorePlots.end());
		}

		if (!bPlotOccupancyOverride)
		{
			kToNodeCacheData.bIsVisibleEnemyUnit = pPlot->isVisibleEnemyUnit(pUnit);
			kToNodeCacheData.bIsVisibleEnemyCombatUnit = pPlot->isVisibleEnemyDefender(pUnit);
		}

		kToNodeCacheData.bIsVisibleNeutralCombatUnit = pPlot->isNeutralUnit(pUnit->getOwner(), true, true);
		kToNodeCacheData.bUnitStackingLimitReached = !pUnit->CanStackUnitAtPlot(pPlot);
	}


	//small hack to prevent civilians from stacking although they could
	if (finder->HaveFlag(CvUnit::MOVEFLAG_DONT_STACK_WITH_NEUTRAL) && pPlot->isNeutralUnit(pUnit->getOwner(),true,true))
		kToNodeCacheData.bUnitStackingLimitReached = true;

	//do not use DestinationReached() here, approximate destination won't do
	bool bIsDestination = node->m_iX == finder->GetDestX() && node->m_iY == finder->GetDestY() || !finder->HasValidDestination();

	//use the flags mostly as provided
	//destination will be handled later once we know whether we would like to end the turn here
	//attack only applies to the true (non-approximate) destination or to any plot if we don't have a destination (reachable plots)
	int iMoveFlags = finder->GetData().iFlags & ~CvUnit::MOVEFLAG_ATTACK & ~CvUnit::MOVEFLAG_DESTINATION;
	if (bIsDestination)
	{
		//special checks for attack flag
		if (pCacheData->IsCanAttack())
		{
			if (pUnit->isRanged())
			{
				//ranged units can capture a civilian by moving but need the attack flag to do it
				if (kToNodeCacheData.bIsVisibleEnemyUnit && !kToNodeCacheData.bIsVisibleEnemyCombatUnit)
					iMoveFlags |= CvUnit::MOVEFLAG_ATTACK;
			}
			else
			{
				//melee units attack enemy cities and units 
				if (kToNodeCacheData.bIsVisibleEnemyUnit || kToNodeCacheData.bIsEnemyCity || bPlotOccupancyOverride)
					iMoveFlags |= CvUnit::MOVEFLAG_ATTACK;
			}
		}
	}

	kToNodeCacheData.iMoveFlags = iMoveFlags;
	kToNodeCacheData.bCanEnterTerrainIntermediate = pUnit->canEnterTerrain(*pPlot,iMoveFlags); //assuming we will _not_ stop here
	kToNodeCacheData.bCanEnterTerrainPermanent = pUnit->canEnterTerrain(*pPlot,iMoveFlags|CvUnit::MOVEFLAG_DESTINATION); //assuming we will stop here
	kToNodeCacheData.bCanEnterTerritoryIntermediate = finder->HaveFlag(CvUnit::MOVEFLAG_IGNORE_RIGHT_OF_PASSAGE) || pUnit->canEnterTerritory(ePlotTeam, false);
	kToNodeCacheData.bCanEnterTerritoryPermanent = finder->HaveFlag(CvUnit::MOVEFLAG_IGNORE_RIGHT_OF_PASSAGE) || pUnit->canEnterTerritory(ePlotTeam, true);

	//precompute this. it only depends on this one plot, so we don't have to do this in PathCost()
	kToNodeCacheData.plotMovementCostMultiplier = CvUnitMovement::GetMovementCostMultiplierFromPromotions(pUnit,pPlot);
	kToNodeCacheData.plotMovementCostAdder = CvUnitMovement::GetMovementCostAdderFromPromotions(pUnit,pPlot);

	//done!
	kToNodeCacheData.iGenerationID = finder->GetCurrentGenerationID();
}

//	--------------------------------------------------------------------------------
bool CvPathFinder::DestinationReached(int iToX, int iToY) const
{
	//catch the "reachable plots" case without a valid destination
	if (iToX==-1 || iToY==-1)
		return false;

	if ( HaveFlag(CvUnit::MOVEFLAG_APPROX_TARGET_RING2) )
	{
		if (HaveFlag(CvUnit::MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN))
			if (GetNode(iToX,iToY)->m_kCostCacheData.bIsNonNativeDomain)
				return false;

		if (!CanEndTurnAtNode(GetNode(iToX, iToY)))
			return false;

		//the main check
		if (::plotDistance(iToX, iToY, GetDestX(), GetDestY()) > 2)
			return false;

		//need to make sure there are no mountains/ice plots in between
		return CommonNeighborIsPassable(GetNode(iToX, iToY), GetNode(GetDestX(), GetDestY()));
	}
	else if ( HaveFlag(CvUnit::MOVEFLAG_APPROX_TARGET_RING1) )
	{
		if (HaveFlag(CvUnit::MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN))
			if (GetNode(iToX,iToY)->m_kCostCacheData.bIsNonNativeDomain)
				return false;

		if (!CanEndTurnAtNode(GetNode(iToX, iToY)))
			return false;

		return ::plotDistance(iToX,iToY,GetDestX(),GetDestY()) < 2;
	}
	else
		return iToX==GetDestX() && iToY==GetDestY();
}

//	--------------------------------------------------------------------------------
/// Standard path finder - is this end point for the path valid?
int PathDestValid(int iToX, int iToY, const SPathFinderUserData&, const CvAStar* finder)
{
	CvPlot* pToPlot = GC.getMap().plotCheckInvalid(iToX, iToY);

	//do not use the node data cache here - it is not set up yet - only the unit data cache is available
	const UnitPathCacheData* pCacheData = reinterpret_cast<const UnitPathCacheData*>(finder->GetScratchBuffer());
	CvUnit* pUnit = pCacheData->pUnit;
	TeamTypes eTeam = pCacheData->getTeam();

	if(pToPlot == NULL || pUnit == NULL)
		return FALSE;

	if(pUnit->plot() == pToPlot)
		return TRUE;

	if(pUnit->IsImmobile())
		return FALSE;

	//in this case we don't know the real target plot yet, need to rely on PathValid() checks later 
	if(finder->IsApproximateMode()) 
		return true;

	//checks which need visibility (logically so we don't leak information)
	if (pToPlot->isVisible(eTeam))
	{
		//use the flags mostly as provided - attack needs manual handling though
		int iMoveFlags = finder->GetData().iFlags & ~CvUnit::MOVEFLAG_ATTACK;

		//checking the destination!
		iMoveFlags |= CvUnit::MOVEFLAG_DESTINATION;

		//special checks for attack flag
		if (pCacheData->IsCanAttack())
		{
			if (pUnit->isRanged())
			{
				//ranged units can capture a civilian by moving but need the attack flag to do it
				if (pToPlot->isVisibleEnemyUnit(pUnit) && !pToPlot->isVisibleEnemyDefender(pUnit))
					iMoveFlags |= CvUnit::MOVEFLAG_ATTACK;
			}
			else
			{
				//melee units can attack enemy cities and units
				if (pToPlot->isVisibleEnemyUnit(pUnit) || pToPlot->isEnemyCity(*pUnit))
					iMoveFlags |= CvUnit::MOVEFLAG_ATTACK;
			}
		}

		if (!pUnit->canMoveInto(*pToPlot,iMoveFlags))
			return FALSE;
	}

	//checks which need a revealed plot (logically so we don't leak information)
	if (pToPlot->isRevealed(eTeam))
	{
		//check terrain and territory - only if not visible, otherwise it has been checked above already
		if (!pToPlot->isVisible(eTeam))
		{
			if(!pUnit->canEnterTerrain(*pToPlot,CvUnit::MOVEFLAG_DESTINATION))
				return FALSE;

			if(!finder->HaveFlag(CvUnit::MOVEFLAG_IGNORE_RIGHT_OF_PASSAGE) && !pUnit->canEnterTerritory(pToPlot->getTeam(),true))
				return FALSE;
		}

		if ( (finder->HaveFlag(CvUnit::MOVEFLAG_NO_EMBARK) || !pUnit->CanEverEmbark()) && pToPlot->needsEmbarkation(pUnit))
			return FALSE;

		if(pUnit->IsCombatUnit())
		{
			CvCity* pCity = pToPlot->getPlotCity();
			if(pCity)
			{
				if(pCacheData->getOwner() != pCity->getOwner() && !GET_TEAM(eTeam).isAtWar(pCity->getTeam()))
				{
					return FALSE;
				}
			}
		}
	}
	else
	{
		if(pCacheData->isNoRevealMap())
		{
			return FALSE;
		}
	}

	return TRUE;
}

//	--------------------------------------------------------------------------------
/// Standard path finder - determine heuristic cost
int PathHeuristic(int /*iCurrentX*/, int /*iCurrentY*/, int iNextX, int iNextY, int iDestX, int iDestY)
{
	//for the heuristic to be admissible, it needs to never overestimate the cost of reaching the target
	//a regular step by a unit costs PATH_BASE_COST*MOVE_DENOMINATOR/MOVES_PER_TURN
	//for a fast unit on a road, moves per turn can be high ... let's assume 15
	return plotDistance(iNextX, iNextY, iDestX, iDestY)*PATH_BASE_COST*4;
}

//	--------------------------------------------------------------------------------
/// Standard path finder - cost for ending the turn on a given plot
int PathEndTurnCost(CvPlot* pToPlot, const CvPathNodeCacheData& kToNodeCacheData, const UnitPathCacheData* pUnitDataCache, int iTurnsInFuture, bool bAbortInDanger)
{
	//human knows best, don't try to be smart
	if (!pUnitDataCache->isAIControl())
		return kToNodeCacheData.bIsNonNativeDomain && pUnitDataCache->IsCanAttack() ? PATH_STEP_WEIGHT : 0;

	int iCost = 0;

	CvUnit* pUnit = pUnitDataCache->pUnit;
	TeamTypes eUnitTeam = pUnitDataCache->getTeam();
	DomainTypes eUnitDomain = pUnitDataCache->getDomainType();

	if(pUnit->IsCombatUnit())
	{
		iCost += (PATH_DEFENSE_WEIGHT * std::max(0, (PATH_ASSUMED_MAX_DEFENSE - ((pUnit->noDefensiveBonus()) ? 0 : pToPlot->defenseModifier(eUnitTeam, false, false)))));
	}

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
			iCost += (PATH_BASE_COST * pToPlot->getExtraMovePathCost());
	}

	if (pUnit->isHasPromotion((PromotionTypes)GC.getPROMOTION_UNWELCOME_EVANGELIST()))
	{
		// Avoid being in a territory that we are not welcome in
		PlayerTypes ePlotOwner = pToPlot->getOwner();
		TeamTypes ePlotTeam = pToPlot->getTeam();
		if (ePlotTeam != NO_TEAM && ePlotTeam!=eUnitTeam && !GET_TEAM(ePlotTeam).IsAllowsOpenBordersToTeam(eUnitTeam) && !GET_PLAYER(ePlotOwner).isMinorCiv())
		{
			iCost += PATH_END_TURN_MISSIONARY_OTHER_TERRITORY;
		}
	}
	else if(pToPlot->isOwned() && pToPlot->getTeam() != eUnitTeam)
	{
		iCost += PATH_END_TURN_FOREIGN_TERRITORY;
	}
	else if (!pToPlot->isOwned())
	{
		iCost += PATH_END_TURN_FOREIGN_TERRITORY/2;
	}

	// If we are a land unit and we are ending the turn on water, make the cost a little higher 
	// so that we favor staying on land or getting back to land as quickly as possible
	if(eUnitDomain == DOMAIN_LAND && kToNodeCacheData.bIsNonNativeDomain)
	{
		if ( pToPlot->getTeam() != eUnitTeam )
			iCost += PATH_END_TURN_WATER;
		else
			iCost += PATH_END_TURN_WATER / 2;
	}

	// when in doubt we prefer to end our turn on a route
	if (!kToNodeCacheData.bIsValidRoute)
		iCost += PATH_END_TURN_NO_ROUTE;

	//danger check (potentially recursive because of GetDanger uses the pathfinder!)
	//careful with additional conditions here ...
	if ( pUnitDataCache->doDanger() )
	{
		//invisible plots might be dangerous without us knowing
		if (!pToPlot->isVisible(eUnitTeam))
			iCost += PATH_END_TURN_INVISIBLE_WEIGHT;

		//calculcate danger. this is expensive but the last result is cached for each plot
		//note: it includes an overkill factor because usually not all enemy units will attack this one unit
		int iPlotDanger = pUnit->GetDanger(pToPlot);

		//we should give more weight to the first end-turn plot, the danger values for future stops are less concrete
		int iFutureFactor = std::max(1,4-iTurnsInFuture);

		if (pUnit->IsCombatUnit())
		{
			//combat units can still tolerate some danger
			//embarkation is handled implicitly because danger value will be higher
			if (iPlotDanger >= pUnit->GetCurrHitPoints()*3)
				iCost += PATH_END_TURN_MORTAL_DANGER_WEIGHT*iFutureFactor;
			else if (iPlotDanger >= pUnit->GetCurrHitPoints())
				iCost += PATH_END_TURN_HIGH_DANGER_WEIGHT*iFutureFactor;
			else if (iPlotDanger > pUnit->GetCurrHitPoints()/3)
				iCost += PATH_END_TURN_LOW_DANGER_WEIGHT*iFutureFactor;
		}
		else //civilian
		{
			if (iPlotDanger == INT_MAX && iTurnsInFuture < 2)
				return -1; //don't ever do this
			else if (iPlotDanger > pUnit->GetCurrHitPoints())
				iCost += PATH_END_TURN_HIGH_DANGER_WEIGHT*4*iFutureFactor;
			else if (iPlotDanger > 0) //note that fog will cause some danger on adjacent plots
				iCost += PATH_END_TURN_LOW_DANGER_WEIGHT*iFutureFactor;
		}

		//sometimes we need to abort if there are only bad options around
		if (bAbortInDanger && iTurnsInFuture < 2 && iPlotDanger > pUnit->GetCurrHitPoints() / 2)
			return -1;
	}

	return iCost;
}

//	--------------------------------------------------------------------------------
/// Standard path finder - compute cost of a move
int PathCost(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, CvAStar* finder)
{
	int iStartMoves = parent->m_iMoves;
	int iTurns = parent->m_iTurns;

	const CvPathNodeCacheData& kToNodeCacheData = node->m_kCostCacheData;
	const CvPathNodeCacheData& kFromNodeCacheData = parent->m_kCostCacheData;

	CvMap& kMap = GC.getMap();
	int iFromPlotX = parent->m_iX;
	int iFromPlotY = parent->m_iY;
	CvPlot* pFromPlot = kMap.plotUnchecked(iFromPlotX, iFromPlotY);

	int iToPlotX = node->m_iX;
	int iToPlotY = node->m_iY;
	CvPlot* pToPlot = kMap.plotUnchecked(iToPlotX, iToPlotY);
	bool bIsPathDest = finder->IsPathDest(iToPlotX, iToPlotY);
	bool bCheckZOC =  !finder->HaveFlag(CvUnit::MOVEFLAG_IGNORE_ZOC);
	bool bCheckStacking = !finder->HaveFlag(CvUnit::MOVEFLAG_IGNORE_STACKING) || kToNodeCacheData.bIsVisibleNeutralCombatUnit; //always check stacking for neutral units

	const UnitPathCacheData* pUnitDataCache = reinterpret_cast<const UnitPathCacheData*>(finder->GetScratchBuffer());
	CvUnit* pUnit = pUnitDataCache->pUnit;

	TeamTypes eUnitTeam = pUnitDataCache->getTeam();
	DomainTypes eUnitDomain = pUnitDataCache->getDomainType();

	//this is quite tricky with passable ice plots which can be either water or land
	bool bToPlotIsWater = kToNodeCacheData.bIsNonNativeDomain || (eUnitDomain==DOMAIN_SEA && pToPlot->isWater());
	bool bFromPlotIsWater = kFromNodeCacheData.bIsNonNativeDomain || (eUnitDomain==DOMAIN_SEA && pToPlot->isWater());

	if (iStartMoves==0)
	{
		// inconspicuous but important
		iTurns++;

		//hand out new moves
		iStartMoves = parent->m_iStartMovesForTurn*GC.getMOVE_DENOMINATOR();
	}

	//calculate move cost
	int iMovementCost = 0;
	if( (node->m_kCostCacheData.bIsVisibleEnemyCombatUnit && !finder->HaveFlag(CvUnit::MOVEFLAG_IGNORE_ENEMIES)) || node->m_kCostCacheData.bIsEnemyCity)
		//if the unit would end its turn, we spend all movement points. even if we can move after attacking, we can't assume we will kill the enemy
		iMovementCost = iStartMoves;
	else
	{
		int iMaxMoves = pUnitDataCache->baseMoves(pToPlot->getDomain())*GC.getMOVE_DENOMINATOR(); //important, use the cached value

		if (bCheckZOC)
		{
			if (finder->HaveFlag(CvUnit::MOVEFLAG_SELECTIVE_ZOC))
				iMovementCost = CvUnitMovement::MovementCostSelectiveZOC(pUnit, pFromPlot, pToPlot, iStartMoves, iMaxMoves, kToNodeCacheData.plotMovementCostMultiplier, kToNodeCacheData.plotMovementCostAdder, data.plotsToIgnoreForZOC);
			else
				iMovementCost = CvUnitMovement::MovementCost(pUnit, pFromPlot, pToPlot, iStartMoves, iMaxMoves, kToNodeCacheData.plotMovementCostMultiplier, kToNodeCacheData.plotMovementCostAdder);
		}
		else
			iMovementCost = CvUnitMovement::MovementCostNoZOC(pUnit, pFromPlot, pToPlot, iStartMoves, iMaxMoves, kToNodeCacheData.plotMovementCostMultiplier, kToNodeCacheData.plotMovementCostAdder);
	}

	// how much is left over?
	int iMovesLeft = iStartMoves - iMovementCost;

	// although we could do an early termination here if we see that MovesLeft is smaller than the value currently set in the node,
	// we don't do it for simplicity. in that rare case we still finish this function and sort it out in LinkChild().
	// also, be careful with such checks as they may break the secondary stop nodes in the two layer pathfinding.

	//important: store the remaining moves so we don't have to recalculate later (e.g. in PathAdd)
	//can't write to node just yet, before we know whether this transition is good or not
	finder->SetTempResult(iMovesLeft,iTurns);

	//base cost. normalize by movement speed of the unit so that faster movement is cheaper.
	//we want to optimize the number of turns for the path, not movement points spent!
	int iCost = (PATH_BASE_COST * iMovementCost) / parent->m_iStartMovesForTurn;
	
	//when in doubt prefer the shorter path
	iCost += PATH_STEP_WEIGHT;

	//when in doubt avoid domain changes because too many of them look stupid
	if (bFromPlotIsWater != bToPlotIsWater)
		iCost += PATH_STEP_WEIGHT;

	//do we end the turn here
	if(iMovesLeft == 0)
	{
		// check whether we're allowed to end the turn in this plot
		if (kToNodeCacheData.bIsRevealedToTeam)
		{
			if (!kToNodeCacheData.bCanEnterTerrainPermanent || !kToNodeCacheData.bCanEnterTerritoryPermanent || kToNodeCacheData.bIsNonEnemyCity)
			return -1; //forbidden
		}

		// check stacking (if visible)
		if (kToNodeCacheData.bPlotVisibleToTeam && bCheckStacking && kToNodeCacheData.bUnitStackingLimitReached && iTurns==0)
			return -1; //forbidden

		//extra cost for ending the turn on various types of undesirable plots
		if (!bIsPathDest)
		{
			//important to not add the extra cost for the requested destination
			int iEndTurnCost = PathEndTurnCost(pToPlot, kToNodeCacheData, pUnitDataCache, node->m_iTurns, finder->HaveFlag(CvUnit::MOVEFLAG_AI_ABORT_IN_DANGER));
			if (iEndTurnCost < 0)
				return -1;

			iCost += iEndTurnCost;
		}
		else
		{
			//apply this part even if it's the explicit target
			if (pUnitDataCache->isAIControl() && pUnit->isHasPromotion((PromotionTypes)GC.getPROMOTION_UNWELCOME_EVANGELIST()))
			{
				// Avoid being in a territory that we are not welcome in
				PlayerTypes ePlotOwner = pToPlot->getOwner();
				TeamTypes ePlotTeam = pToPlot->getTeam();
				if (ePlotTeam != NO_TEAM && ePlotTeam != eUnitTeam && !GET_TEAM(ePlotTeam).IsAllowsOpenBordersToTeam(eUnitTeam) && !GET_PLAYER(ePlotOwner).isMinorCiv())
				{
					iCost += PATH_END_TURN_MISSIONARY_OTHER_TERRITORY;
				}
			}
		}
	}

	if(finder->HaveFlag(CvUnit::MOVEFLAG_MAXIMIZE_EXPLORE))
	{
		if(!pToPlot->isHills()) //maybe better check seeFromLevel?
		{
			iCost += PATH_EXPLORE_NON_HILL_WEIGHT;
		}

		int iUnseenPlots = pToPlot->getNumAdjacentNonrevealed(eUnitTeam);
		if(!pToPlot->isRevealed(eUnitTeam))
		{
			iUnseenPlots += 1;
		}

		iCost += (7 - iUnseenPlots) * PATH_EXPLORE_NON_REVEAL_WEIGHT;
	}

	if(pUnitDataCache->IsCanAttack() && bIsPathDest)
	{
		//AI makes sure to use defensive bonuses etc. humans have to do it manually ... it's part of the fun!
		if(node->m_kCostCacheData.bIsVisibleEnemyCombatUnit && pUnitDataCache->isAIControl())
		{
			iCost += (PATH_DEFENSE_WEIGHT * std::max(0, (PATH_ASSUMED_MAX_DEFENSE - ((pUnit->noDefensiveBonus()) ? 0 : pFromPlot->defenseModifier(eUnitTeam, false, false)))));

			//avoid river attack penalty
			if(!pUnit->isRiverCrossingNoPenalty() && pFromPlot->isRiverCrossing(directionXY(pFromPlot, pToPlot)))
				iCost += (PATH_ATTACK_WEIGHT * -(GC.getRIVER_ATTACK_MODIFIER()));

			//avoid disembarkation penalty
			if (bFromPlotIsWater && !bToPlotIsWater && !pUnit->isAmphibious())
				iCost += (PATH_ATTACK_WEIGHT * -(GC.getAMPHIB_ATTACK_MODIFIER()));
		}
	}

	return iCost;
}

//	---------------------------------------------------------------------------
/// Standard path finder - check validity of a coordinate
int PathValid(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData&, const CvAStar* finder)
{
	// If this is the first node in the path, it is always valid (starting location)
	if (parent == NULL)
		return TRUE;

	// Cached values for this node that we will use
	const CvPathNodeCacheData& kToNodeCacheData = node->m_kCostCacheData;
	const CvPathNodeCacheData& kFromNodeCacheData = parent->m_kCostCacheData;
	const UnitPathCacheData* pCacheData = reinterpret_cast<const UnitPathCacheData*>(finder->GetScratchBuffer());
	CvUnit* pUnit = pCacheData->pUnit;
	TeamTypes eUnitTeam = pCacheData->getTeam();
	bool bCheckStacking = !finder->HaveFlag(CvUnit::MOVEFLAG_IGNORE_STACKING) || kFromNodeCacheData.bIsVisibleNeutralCombatUnit; //always check stacking for neutral units

#if defined(MOD_CORE_UNREVEALED_IMPASSABLE)
	if (!kToNodeCacheData.bIsRevealedToTeam && !pUnit->isHuman() && !finder->HaveFlag(CvUnit::MOVEFLAG_PRETEND_ALL_REVEALED) && pUnit->AI_getUnitAIType()!=UNITAI_EXPLORE)
		return FALSE;
#endif

	bool bNextNodeHostile = kToNodeCacheData.bIsEnemyCity || kToNodeCacheData.bIsVisibleEnemyCombatUnit;
	bool bNextNodeVisibleToTeam = kToNodeCacheData.bPlotVisibleToTeam;

	// we would run into an enemy or run into unknown territory, so we must be able to end the turn on the _parent_ plot
	if (bNextNodeHostile || !bNextNodeVisibleToTeam)
	{
		//don't leak information
		if (kFromNodeCacheData.bIsRevealedToTeam)
		{
			// most importantly, we need to be able to end the turn there
			if(!kFromNodeCacheData.bCanEnterTerrainPermanent || !kFromNodeCacheData.bCanEnterTerritoryPermanent)
				return FALSE;

#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
			if(!MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS || pCacheData->m_bCanAttack)
#else
			if(true)
#endif
			{
				if (kFromNodeCacheData.bIsNonEnemyCity)
					return FALSE;

				//special: cannot attack out of a fort or city with melee ships (ranged ships cannot attack from cities as well)
				if (bNextNodeHostile && kFromNodeCacheData.bIsNonNativeDomain && pUnit->getDomainType()==DOMAIN_SEA)
					return FALSE;
			}

			// check stacking (if visible)
			if (kFromNodeCacheData.bPlotVisibleToTeam && bCheckStacking && kFromNodeCacheData.bUnitStackingLimitReached)
				return FALSE;
		}
	}

	CvMap& theMap = GC.getMap();
	CvPlot* pFromPlot = theMap.plotUnchecked(parent->m_iX, parent->m_iY);
	CvPlot* pToPlot = theMap.plotUnchecked(node->m_iX, node->m_iY);

	//some checks about units etc. they need to be visible, else we leak information in the UI
	if (kToNodeCacheData.bPlotVisibleToTeam)
	{
		//some quick checks first (redundant with canMoveInto but faster)
		if(!kToNodeCacheData.bCanEnterTerrainIntermediate || !kToNodeCacheData.bCanEnterTerritoryIntermediate)
			return FALSE;
		//make sure we could stack
		if(kToNodeCacheData.bIsVisibleEnemyUnit && kToNodeCacheData.bIsVisibleNeutralCombatUnit)
			return FALSE;
		//we check stacking once we know whether we end the turn here (in PathCost)
		if(!pUnit->canMoveInto(*pToPlot, kToNodeCacheData.iMoveFlags))
			return FALSE;
	}

	//some checks about terrain etc. needs to be revealed, otherwise we leak information in the UI
	if (kToNodeCacheData.bIsRevealedToTeam)
	{
		// if we can't enter the plot even temporarily, that's it. 
		// if we can enter, there's another check in PathCost once we know whether we need to stay here
		if(!kToNodeCacheData.bCanEnterTerrainIntermediate)
			return FALSE;
		if(!kToNodeCacheData.bCanEnterTerritoryIntermediate)
			return FALSE;

		//do not use DestinationReached() here, approximate destination won't do (also we don't use MOVEFLAG_DESTINATION in pathfinder)
		bool bIsDestination = node->m_iX == finder->GetDestX() && node->m_iY == finder->GetDestY() || !finder->HasValidDestination();

		//don't allow moves through enemy cities (but allow them as attack targets for melee)
		if (kToNodeCacheData.bIsEnemyCity && !(bIsDestination && pUnit->IsCanAttackWithMove()))
			return FALSE;

		if(pCacheData->CanEverEmbark())
		{
			//don't embark if forbidden - but move along if already on water plot
			if (finder->HaveFlag(CvUnit::MOVEFLAG_NO_EMBARK) && kToNodeCacheData.bIsNonNativeDomain && !kFromNodeCacheData.bIsNonNativeDomain)
				return FALSE;

			//embark required and possible?
			if(!kFromNodeCacheData.bIsNonNativeDomain && kToNodeCacheData.bIsNonNativeDomain && kToNodeCacheData.bIsRevealedToTeam)
			{
				if (!pUnit->canEmbarkOnto(*pFromPlot, *pToPlot, true, kToNodeCacheData.iMoveFlags))
					return FALSE;

				//in addition to the danger check (which increases path cost), a hard exclusion if the enemy navy dominates the area
				if ( pCacheData->isAIControl() && pUnit->IsCombatUnit() && pToPlot->GetNumEnemyUnitsAdjacent(eUnitTeam,DOMAIN_SEA)>0)
					return FALSE;
			}

			//disembark required and possible?
			if(kFromNodeCacheData.bIsNonNativeDomain && !kToNodeCacheData.bIsNonNativeDomain && kToNodeCacheData.bIsRevealedToTeam)
			{ 
				if (!pUnit->canDisembarkOnto(*pFromPlot, *pToPlot, true, kToNodeCacheData.iMoveFlags))
					return FALSE;
			}
		}

		//normally we would be able to enter enemy territory if at war
		if( kToNodeCacheData.iMoveFlags & CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY )
		{
			if(pToPlot->isOwned() && atWar(pToPlot->getTeam(), eUnitTeam))
				return FALSE;
		}

		//ocean allowed?
		if ( kToNodeCacheData.iMoveFlags & CvUnit::MOVEFLAG_NO_OCEAN )
		{
			if (pToPlot->getTerrainType() == TERRAIN_OCEAN)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

//	--------------------------------------------------------------------------------
/// Node was added to the path
void CvTwoLayerPathFinder::NodeAddedToPath(CvAStarNode* parent, CvAStarNode* node, int iKnownCost, CvAStarNodeAddOp operation)
{
	const UnitPathCacheData* pCacheData = reinterpret_cast<const UnitPathCacheData*>(GetScratchBuffer());
	const CvUnit* pUnit = pCacheData->pUnit;
	bool bUpdateCacheForNeighbors = false;

	//failsafe
	if (node == parent)
		return;

	//make sure the costs are up to date
	node->m_pParent = parent;
	node->m_iKnownCost = iKnownCost;
	//compute heuristic cost only once ...
	if (node->m_iHeuristicCost == 0 && udHeuristic && isValid(m_iXdest, m_iYdest))
	{
		int iHeuristicCost = parent ?
			udHeuristic(parent->m_iX, parent->m_iY, node->m_iX, node->m_iY, m_iXdest, m_iYdest):
			udHeuristic(m_iXstart, m_iYstart, node->m_iX, node->m_iY, m_iXdest, m_iYdest);
		node->m_iHeuristicCost = iHeuristicCost;
	}
	node->m_iTotalCost = node->m_iKnownCost*giKnownCostWeight + node->m_iHeuristicCost*giHeuristicCostWeight;

	if(operation == ASNC_INITIALADD)
	{
		//in this case we did not call PathCost() before, so we have to set the initial values here
		node->m_iMoves = GetData().iStartMoves;
		node->m_iTurns = 0;
		node->m_iStartMovesForTurn = pCacheData->baseMoves(pUnit->plot()->getDomain());

		AddToOpen(node);
		UpdateNodeCacheData(node,pUnit,this);
		bUpdateCacheForNeighbors = true;
	}
	else
	{
		//these were already calculated in PathCost, don't do the work again
		node->m_iMoves = GetCachedMoveCount();
		node->m_iTurns = GetCachedTurnCount();

		if (node->m_iMoves == 0)
		{
			//if necessary manually update the start turn moves field
			CvPlot* pPlot = GC.getMap().plotUnchecked(node->m_iX, node->m_iY);
			node->m_iStartMovesForTurn = pCacheData->baseMoves(pPlot->getDomain());
		}
		else if (parent) //should always be true
		{
			//re-use parent value if we can continue the turn
			node->m_iStartMovesForTurn = parent->m_iStartMovesForTurn;
		}

		//otherwise we don't need to update the cache, it has already been done in a previous call
		if (operation == ASNC_NEWADD)
		{
			bUpdateCacheForNeighbors = true;
			AddToOpen(node);
		}
	}

	if (bUpdateCacheForNeighbors)
	{
		//update cache for also all possible children
		for(int i = 0; i < 6; i++)
		{
			CvAStarNode* neighbor = node->m_apNeighbors[i];
			UpdateNodeCacheData(neighbor,pUnit,this);
		}

		//non local neighbors
		if (udGetExtraChildrenFunc)
		{
			vector<pair<int, int>> extraChildren;
			GetExtraChildren(node, extraChildren);
			for (size_t i = 0; i < extraChildren.size(); i++)
			{
				int x = extraChildren[i].first;
				int y = extraChildren[i].second;

				if (isValid(x, y))
				{
					CvAStarNode* neighbor = GetNodeMutable(x, y);
					UpdateNodeCacheData(neighbor, pUnit, this);
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Step path finder - is this end point for the path valid?
int StepDestValid(int iToX, int iToY, const SPathFinderUserData&, const CvAStar* finder)
{
	CvMap& kMap = GC.getMap();
	CvPlot* pFromPlot = kMap.plotCheckInvalid(finder->GetStartX(), finder->GetStartY());
	CvPlot* pToPlot = kMap.plotCheckInvalid(iToX, iToY);

	if (!pFromPlot || !pToPlot)
		return FALSE;

	if(pFromPlot->getArea() != pToPlot->getArea())
	{
		bool bAllow = false;

		//be a little lenient with cities
		if (pFromPlot->isCity())
		{
			CvCity* pCity = pFromPlot->getPlotCity();
			if (pCity->isMatchingArea(pToPlot))
				bAllow = true;
		}

		if (pToPlot->isCity())
		{
			CvCity* pCity = pToPlot->getPlotCity();
			if (pCity->isMatchingArea(pFromPlot))
				bAllow = true;
		}

		if (!bAllow && finder->HaveFlag(CvUnit::MOVEFLAG_APPROX_TARGET_RING1))
		{
			std::vector<int> vAreas = pToPlot->getAllAdjacentAreas();
			bAllow = (std::find(vAreas.begin(),vAreas.end(),pFromPlot->getArea()) != vAreas.end());
			//cities are even more complicated ...
			if (!bAllow && pFromPlot->isCity())
			{
				std::vector<int> cityAreas = pFromPlot->getAllAdjacentAreas();
				std::vector<int> shared(MAX(vAreas.size(), cityAreas.size()));
				std::vector<int>::iterator result = std::set_intersection(vAreas.begin(), vAreas.end(), cityAreas.begin(), cityAreas.end(), shared.begin());
				bAllow = (result != shared.begin());
			}
		}

		if (!bAllow)
			return FALSE;
	}

	return TRUE;
}

//estimate the approximate movement cost for a unit
int StepCostEstimate(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData&, CvAStar*)
{
	CvPlot* pFromPlot = GC.getMap().plotUnchecked(parent->m_iX, parent->m_iY);
	CvPlot* pToPlot = GC.getMap().plotUnchecked(node->m_iX, node->m_iY);

	int iScale = 100;
	bool bIsValidRoute = pFromPlot->isRoute() && !pFromPlot->IsRoutePillaged() && pToPlot->isRoute() && !pToPlot->IsRoutePillaged();

	//todo: is there a fast way to check whether embarkation / deep water embarkation is allowed?
	if (bIsValidRoute)
		iScale = 67;
	else if (pToPlot->isImpassable())
		iScale = 400;
	else if (pToPlot->isRoughGround())
		iScale = 200;
	else if (pFromPlot->isWater() != pToPlot->isWater() && !pFromPlot->isCity() && !pToPlot->isCity())
		iScale = 200; //dis/embarkation
	else if (pFromPlot->isWater() && pToPlot->isWater())
		iScale = 67; //movement on water is usually faster

	return PATH_BASE_COST*iScale/100;
}

//	--------------------------------------------------------------------------------
/// Default heuristic cost
int StepHeuristic(int /*iCurrentX*/, int /*iCurrentY*/, int iNextX, int iNextY, int iDestX, int iDestY)
{
	//todo: do we need tiebreaks here? cross product between step direction and target direction?
	return plotDistance(iNextX, iNextY, iDestX, iDestY) * PATH_BASE_COST/2;
}

//	--------------------------------------------------------------------------------
/// Step path finder - compute cost of a path
int StepCost(const CvAStarNode*, const CvAStarNode* node, const SPathFinderUserData&, CvAStar*)
{
	CvPlot* pNewPlot = GC.getMap().plotUnchecked(node->m_iX, node->m_iY);

	//when in doubt, avoid rough plots
	int iCost = PATH_BASE_COST;
	if (pNewPlot->isRoughGround() && (!pNewPlot->isRoute() || pNewPlot->IsRoutePillaged())) 
		iCost += PATH_BASE_COST/10;
		
	return iCost;
}


//	--------------------------------------------------------------------------------
/// Step path finder - check validity of a coordinate
int StepValidGeneric(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar* finder, bool bAnyArea, bool bWide)
{
	if(parent == NULL)
		return TRUE;

	PlayerTypes ePlayer = data.ePlayer;
	PlayerTypes eEnemy = (PlayerTypes)data.iTypeParameter; //we pretend we can enter this player's plots even if we're not at war
	TeamTypes eMyTeam = (ePlayer!=NO_PLAYER) ? GET_PLAYER(ePlayer).getTeam() : NO_TEAM;

	CvMap& kMap = GC.getMap();
	CvPlot* pToPlot = kMap.plotUnchecked(node->m_iX, node->m_iY);
	CvPlot* pFromPlot = kMap.plotUnchecked(parent->m_iX, parent->m_iY);

	if (!pFromPlot || !pToPlot)
		return FALSE;

#if defined(MOD_CORE_UNREVEALED_IMPASSABLE)
	if (eMyTeam!=NO_TEAM && !pToPlot->isRevealed(eMyTeam))
		return FALSE;
#endif

	//this is the important check here - stay within the same area
	if(!bAnyArea && pFromPlot->getArea() != pToPlot->getArea())
	{
		bool bAllowStep = false;

		//be a little lenient with cities - on the first and last leg!
		bool bAllowAreaChange = !parent->m_pParent || finder->IsPathDest(node->m_iX, node->m_iY);
		if (bAllowAreaChange)
		{
			if (pFromPlot->isCity())
			{
				CvCity* pCity = pFromPlot->getPlotCity();
				if (pCity->isMatchingArea(pToPlot))
					bAllowStep = true;
			}

			if (pToPlot->isCity())
			{
				CvCity* pCity = pToPlot->getPlotCity();
				if (pCity->isMatchingArea(pFromPlot))
					bAllowStep = true;
			}
		}

		if (!bAllowStep)
			return FALSE;
	}

	//if we have a given player, check their particular impassability (depends on techs etc)
	if(!pToPlot->isValidMovePlot(ePlayer,false))
		return FALSE;

	//are we allowed to use ocean plots?
	if (finder->HaveFlag(CvUnit::MOVEFLAG_NO_OCEAN) && pToPlot->getTerrainType() == TERRAIN_OCEAN)
		return FALSE;

	//territory check
	if (ePlayer != NO_PLAYER && pToPlot->isOwned())
	{
		if (pToPlot->getOwner() != eEnemy)
		{
			if (finder->HaveFlag(CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY) && GET_TEAM(pToPlot->getTeam()).isAtWar(eMyTeam))
				return FALSE;

			if (!finder->HaveFlag(CvUnit::MOVEFLAG_IGNORE_RIGHT_OF_PASSAGE) && !pToPlot->IsFriendlyTerritory(ePlayer))
				return FALSE;
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

		if (!rl || !StepValidGeneric(parent,rl,data,finder,bAnyArea,false))
			return false;
		if (!rr || !StepValidGeneric(parent,rr,data,finder,bAnyArea,false))
			return false;
	}

	return TRUE;
}

int StepValid(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar* finder)
{
	return StepValidGeneric(parent,node,data,finder,false,false);
}
int StepValidAnyArea(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar* finder)
{
	return StepValidGeneric(parent,node,data,finder,true,false);
}
int StepValidWide(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar* finder)
{
	return StepValidGeneric(parent,node,data,finder,false,true);
}
int StepValidWideAnyArea(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar* finder)
{
	return StepValidGeneric(parent,node,data,finder,true,true);
}

//	--------------------------------------------------------------------------------
/// Step path finder - add a new node to the path. count the steps as turns
void CvStepFinder::NodeAddedToPath(CvAStarNode* parent, CvAStarNode* node, int iKnownCost, CvAStarNodeAddOp operation)
{
	//failsafe
	if (node == parent)
		return;

	//fix up the most important fields first
	node->m_pParent = parent;
	node->m_iKnownCost = iKnownCost;
	//compute heuristic cost only once ...
	if (node->m_iHeuristicCost == 0 && udHeuristic && isValid(m_iXdest, m_iYdest))
	{
		int iHeuristicCost = parent ?
			udHeuristic(parent->m_iX, parent->m_iY, node->m_iX, node->m_iY, m_iXdest, m_iYdest):
			udHeuristic(m_iXstart, m_iYstart, node->m_iX, node->m_iY, m_iXdest, m_iYdest);
		node->m_iHeuristicCost = iHeuristicCost;
	}
	node->m_iTotalCost = node->m_iKnownCost*giKnownCostWeight + node->m_iHeuristicCost*giHeuristicCostWeight;

	//moves always zero for stepfinder
	node->m_iMoves = 0;

	if(operation == ASNC_INITIALADD)
	{
		node->m_iTurns = 0;
		AddToOpen(node);
	}
	else
	{
		if (m_sData.ePathType == PT_GENERIC_REACHABLE_PLOTS)
		//assume a unit has 2*PATH_BASE_COST movement points per turn
			node->m_iTurns = unsigned short((node->m_iKnownCost + PATH_BASE_COST) / (2 * PATH_BASE_COST));
	else
		node->m_iTurns = (parent->m_iTurns + 1);

		if (operation == ASNC_NEWADD)
			AddToOpen(node);
	}
}

//	--------------------------------------------------------------------------------
/// Influence path finder - is this end point for the path valid?
int InfluenceDestValid(int iToX, int iToY, const SPathFinderUserData& data, const CvAStar* finder)
{
	//allow undefined destination plots
	if (iToX==-1 && iToY==-1)
		return TRUE;

	CvMap& kMap = GC.getMap();
	CvPlot* pFromPlot = kMap.plotCheckInvalid(finder->GetStartX(), finder->GetStartY());
	CvPlot* pToPlot = kMap.plotCheckInvalid(iToX, iToY);

	if (!pFromPlot || !pToPlot)
		return FALSE;

	if(plotDistance(*pFromPlot,*pToPlot) > data.iTypeParameter)
		return FALSE;

	//can only claim ocean tiles after we can cross oceans
	if (pToPlot->isDeepWater() && data.ePlayer != NO_PLAYER)
	{
		CvPlayer& kPlayer = GET_PLAYER(data.ePlayer);
		if (!kPlayer.CanCrossOcean() && !GET_TEAM(kPlayer.getTeam()).canEmbarkAllWaterPassage())
			return FALSE;
	}

	return TRUE;
}

//	--------------------------------------------------------------------------------
/// Influence path finder - compute cost of a path
int InfluenceCost(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData&, CvAStar* finder)
{
	//failsafe
	if (!parent || !node)
		return 0;

	int iCost = 1;
	int iExtraCost = 0;

	CvMap& kMap = GC.getMap();
	CvPlot* pToPlot = kMap.plotUnchecked(node->m_iX, node->m_iY);
	CvPlot* pFromPlot = kMap.plotUnchecked(parent->m_iX, parent->m_iY);
	CvPlot* pSourcePlot = kMap.plotUnchecked(finder->GetStartX(), finder->GetStartY());

	//ignore plots which are not adjacent to our territory
	if (pFromPlot->getOwner() != finder->GetData().ePlayer)
		return -1;

	//look how hard it is to cross the border
	if (pToPlot->getOwner() != pSourcePlot->getOwner())
	{
		bool bIsRoute = (pFromPlot->isRoute() && !pFromPlot->IsRoutePillaged() && pToPlot->isRoute() && !pToPlot->IsRoutePillaged());

		//rivers are natural borders
		if(pFromPlot->isRiverCrossing(directionXY(pFromPlot, pToPlot)) && !bIsRoute)
			iCost += GC.getINFLUENCE_RIVER_COST();

		//going through foreign territory is expensive (we already check that we don't own it)
		if(pToPlot->getOwner() != NO_PLAYER)
			iCost += 15;

		//plot type dependent cost. should really be handled via terrain, but ok for now
		if (pToPlot->isHills())
			iExtraCost = max(iExtraCost,GC.getINFLUENCE_HILL_COST());
		if (pToPlot->isMountain() && !pToPlot->IsNaturalWonder())
			iExtraCost = max(iExtraCost,GC.getINFLUENCE_MOUNTAIN_COST());

		//ignore this if there's a resource here
		if (pToPlot->getResourceType(pSourcePlot->getTeam())==NO_RESOURCE)
		{
			//hack: treat a lake like plains - water has a higher influence cost
			CvTerrainInfo* pTerrain = GC.getTerrainInfo(pToPlot->isLake() ? TERRAIN_PLAINS : pToPlot->getTerrainType());
			CvFeatureInfo* pFeature = GC.getFeatureInfo(pToPlot->getFeatureType());
			if (pFeature)
				iExtraCost = max(iExtraCost, pFeature->getInfluenceCost());
			else if (pTerrain)
				iExtraCost = max(iExtraCost, pTerrain->getInfluenceCost());
		}

		//going along routes is cheaper
		if (bIsRoute)
			iExtraCost /= 2;
	}
	else 
	{
		//inside our territory everything costs the same unless it's a mountain
		if (pToPlot->isImpassable(pSourcePlot->getTeam()))
			iCost += GC.getINFLUENCE_MOUNTAIN_COST();
	}

	return max(1,iCost+iExtraCost)*PATH_BASE_COST;
}


//	--------------------------------------------------------------------------------
/// Influence path finder - check validity of a coordinate
int InfluenceValid(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar* finder)
{
	if(parent == NULL)
		return TRUE;

	CvPlot* pOrigin = GC.getMap().plotUnchecked(finder->GetStartX(), finder->GetStartY());
	CvPlot* pToPlot = GC.getMap().plotUnchecked(node->m_iX, node->m_iY);
	if (!pOrigin || !pToPlot)
		return FALSE;

	if(plotDistance(*pOrigin,*pToPlot) > data.iTypeParameter)
		return FALSE;

	//can only claim ocean tiles after we can cross oceans
	if (pToPlot->isDeepWater() && data.ePlayer!=NO_PLAYER)
	{
		CvPlayer& kPlayer = GET_PLAYER(data.ePlayer);
		if (!kPlayer.CanCrossOcean() && !GET_TEAM(kPlayer.getTeam()).canEmbarkAllWaterPassage())
			return FALSE;
	}

	return TRUE;
}

//	--------------------------------------------------------------------------------
int CityConnectionGetExtraChildren(const CvAStarNode* node, const CvAStar* finder, vector<pair<int,int>>& out)
{
	out.clear();

	CvPlayerAI& kPlayer = GET_PLAYER(finder->GetData().ePlayer);
	TeamTypes eTeam = kPlayer.getTeam();
	CvPlot* pPlot = GC.getMap().plotCheckInvalid(node->m_iX, node->m_iY);
	if(!pPlot)
		return 0;

	CvCity* pFirstCity = pPlot->getPlotCity();

	// if there isn't a city there or the city isn't on our team
	if(!pFirstCity || pFirstCity->getTeam() != eTeam)
		return 0;

	const CvCityConnections::SingleCityConnectionStore& cityConnections = kPlayer.GetCityConnections()->GetDirectConnectionsFromCity(pFirstCity);
	for (CvCityConnections::SingleCityConnectionStore::const_iterator it=cityConnections.begin(); it!=cityConnections.end(); ++it)
	{
		//we only care about water connections here because they are not normal routes
		if (it->second & CvCityConnections::CONNECTION_HARBOR)
		{
			CvCity* pSecondCity = GET_PLAYER(PlayerTypes(it->first.first)).getCity(it->first.second);
			if (pSecondCity)
				out.push_back(make_pair(pSecondCity->getX(), pSecondCity->getY()));
		}
	}

	return (int)out.size();
}

//	---------------------------------------------------------------------------
/// Route path finder - check validity of a coordinate
int CityConnectionLandValid(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar*)
{
	if(parent == NULL || data.ePlayer==NO_PLAYER)
		return TRUE;

	PlayerTypes ePlayer = data.ePlayer;
	RouteTypes eRoute = (RouteTypes)data.iTypeParameter;

	CvPlot* pNewPlot = GC.getMap().plotUnchecked(node->m_iX, node->m_iY);
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	RouteTypes ePlotRoute = pNewPlot->getRouteType();

	if(pNewPlot->IsRoutePillaged())
		ePlotRoute = NO_ROUTE;

	if(ePlotRoute == NO_ROUTE)
	{
		//what else can count as road depends on the player type
		if(kPlayer.GetPlayerTraits()->IsRiverTradeRoad() && pNewPlot->isRiver())
				ePlotRoute = ROUTE_ROAD;
		if(kPlayer.GetPlayerTraits()->IsWoodlandMovementBonus() && (pNewPlot->getFeatureType() == FEATURE_FOREST || pNewPlot->getFeatureType() == FEATURE_JUNGLE))
				ePlotRoute = ROUTE_ROAD;
	}

	if(ePlotRoute == NO_ROUTE)
	{
		return FALSE;
	}
	else if ( eRoute == ROUTE_ANY || ePlotRoute >= eRoute ) //a railroad is also a road!
	{
		//finally check plot ownership
		PlayerTypes ePlotOwnerPlayer = pNewPlot->getOwner();
		if (ePlotOwnerPlayer != NO_PLAYER && ePlotOwnerPlayer != data.ePlayer)
		{
			if (GET_PLAYER(ePlotOwnerPlayer).isMajorCiv())
				//major player without open borders is not ok
				return pNewPlot->IsFriendlyTerritory(ePlayer);
			else
				//minor player is ok as long as no war
				return kPlayer.IsAtPeaceWith(ePlotOwnerPlayer);
		}

		return TRUE;
	}

	return FALSE;
}

//	--------------------------------------------------------------------------------
/// Water route valid finder - check the validity of a coordinate
int CityConnectionWaterValid(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar*)
{
	if(parent == NULL)
		return TRUE;

	PlayerTypes ePlayer = data.ePlayer;
	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	CvPlot* pNewPlot = GC.getMap().plotUnchecked(node->m_iX, node->m_iY);

	if(!pNewPlot || !pNewPlot->isRevealed(eTeam))
		return FALSE;

	if (!pNewPlot->isWater() && !pNewPlot->isCityOrPassableImprovement(ePlayer,true))
		return FALSE;

	//finally check plot ownership
	PlayerTypes ePlotOwnerPlayer = pNewPlot->getOwner();
	if (ePlotOwnerPlayer != NO_PLAYER && ePlotOwnerPlayer != data.ePlayer)
	{
		if (GET_PLAYER(ePlotOwnerPlayer).isMajorCiv())
		{
			//major player without open borders is not ok
			if (!pNewPlot->IsFriendlyTerritory(ePlayer))
				return FALSE;
		}
		else
		{
			//minor player is ok as long as no war
			if (GET_PLAYER(ePlayer).IsAtWarWith(ePlotOwnerPlayer))
				return FALSE;
		}
	}

	return TRUE;
}

//	--------------------------------------------------------------------------------
/// Build route cost
int BuildRouteCost(const CvAStarNode* /*parent*/, const CvAStarNode* node, const SPathFinderUserData& data, CvAStar*)
{
	CvPlot* pPlot = GC.getMap().plotUnchecked(node->m_iX, node->m_iY);

	if(pPlot->getRouteType() != NO_ROUTE)
		//do not check if the type matches exactly - put railroads over roads
		return PATH_BUILD_ROUTE_REUSE_EXISTING_WEIGHT;
	else
	{
		// if the tile already been tagged for building a road, then provide a discount
		if(pPlot->GetBuilderAIScratchPadTurn() >= (GC.getGame().getGameTurn()-1) && 
			pPlot->GetBuilderAIScratchPadPlayer() == data.ePlayer &&
			pPlot->GetBuilderAIScratchPadRoute() != NO_ROUTE
			)
			return PATH_BASE_COST/2;

		//should we prefer rough terrain because the gain in movement points is greater?
		int iCost = PATH_BASE_COST;

		//can't build anything on mountain
		if (pPlot->isMountain())
			iCost++;

		//prefer plots without resources so we can build more villages
		if(pPlot->getResourceType()!=NO_RESOURCE)
			iCost++;

		return iCost;
	}
}

//	--------------------------------------------------------------------------------
/// Build Route path finder - check validity of a coordinate
int BuildRouteValid(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar*)
{
	if(parent == NULL || data.ePlayer == NO_PLAYER)
		return TRUE;

	PlayerTypes ePlayer = data.ePlayer;
	CvPlayer& thisPlayer = GET_PLAYER(ePlayer);
	bool bThisPlayerIsMinor = thisPlayer.isMinorCiv();

	//can we build it?
	RouteTypes eRoute = (RouteTypes)data.iTypeParameter;
	if (eRoute > thisPlayer.getBestRoute())
		return FALSE;

	CvPlot* pNewPlot = GC.getMap().plotUnchecked(node->m_iX, node->m_iY);
	if(!bThisPlayerIsMinor && !(pNewPlot->isRevealed(thisPlayer.getTeam())))
		return FALSE;

	if(pNewPlot->isWater())
		return FALSE;

	if(!pNewPlot->isValidMovePlot(ePlayer))
		return FALSE;

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

	//too dangerous, might be severed any time
	if (ePlotOwnerPlayer == NO_PLAYER && pNewPlot->IsAdjacentOwnedByTeamOtherThan(thisPlayer.getTeam()))
		return FALSE;

	return TRUE;
}


//	--------------------------------------------------------------------------------
/// Area path finder - check validity of a coordinate
int AreaValid(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData&, const CvAStar*)
{
	if(parent == NULL)
	{
		return TRUE;
	}

	CvMap& kMap = GC.getMap();

	//this is independent of any team!
	if(kMap.plotUnchecked(parent->m_iX, parent->m_iY)->isImpassable() != kMap.plotUnchecked(node->m_iX, node->m_iY)->isImpassable())
		return FALSE;

	return kMap.plotUnchecked(parent->m_iX, parent->m_iY)->isWater() == kMap.plotUnchecked(node->m_iX, node->m_iY)->isWater();
}

//	--------------------------------------------------------------------------------
/// Area path finder - check validity of a coordinate
int LandmassValid(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData&, const CvAStar*)
{
	if(parent == NULL)
	{
		return TRUE;
	}

	CvMap& kMap = GC.getMap();
	return kMap.plotUnchecked(parent->m_iX, parent->m_iY)->isWater() == kMap.plotUnchecked(node->m_iX, node->m_iY)->isWater();
}

// DERIVED CLASSES (which have more convenient ways to access our various pathfinders)

//	--------------------------------------------------------------------------------
/// Constructor
CvTwoLayerPathFinder::CvTwoLayerPathFinder()
{
	m_ppaaPartialMoveNodes = NULL;

	//this is our default path type
	m_sData.ePathType = PT_UNIT_MOVEMENT;
	SetFunctionPointers(PathDestValid, PathHeuristic, PathCost, PathValid, NULL, UnitPathInitialize, UnitPathUninitialize);

#if defined(MOD_BALANCE_CORE)
	//for debugging
	m_strName = "TwoLayerAStar";
#endif
}

//	--------------------------------------------------------------------------------
/// Destructor
CvTwoLayerPathFinder::~CvTwoLayerPathFinder()
{
	DeInit();
}

//	--------------------------------------------------------------------------------
/// Allocate memory, zero variables
void CvTwoLayerPathFinder::Initialize(int iColumns, int iRows, bool bWrapX, bool bWrapY)
{
	DeInit();

	CvAStar::Initialize(iColumns, iRows, bWrapX, bWrapY);

	m_ppaaPartialMoveNodes = FNEW(CvAStarNode*[m_iColumns], c_eCiv5GameplayDLL, 0);
	for(int iI = 0; iI < m_iColumns; iI++)
	{
		m_ppaaPartialMoveNodes[iI] = FNEW(CvAStarNode[m_iRows], c_eCiv5GameplayDLL, 0);
		for(int iJ = 0; iJ < m_iRows; iJ++)
		{
			m_ppaaPartialMoveNodes[iI][iJ].m_iX = iI;
			m_ppaaPartialMoveNodes[iI][iJ].m_iY = iJ;
		}
	}

	//re-use the base layer neighbors here!
	CvAStarNode** apNeighbors = m_ppaaNeighbors;
	for(int iI = 0; iI < m_iColumns; iI++)
	{
		for(int iJ = 0; iJ < m_iRows; iJ++)
		{
			//neighbors have already been precalculated in base class
			m_ppaaPartialMoveNodes[iI][iJ].m_apNeighbors = apNeighbors;
			apNeighbors += 6;
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
//	version for unit pathing
bool CvTwoLayerPathFinder::CanEndTurnAtNode(const CvAStarNode* temp) const
{
	if (!temp)
		return false;

	if (temp->m_kCostCacheData.bIsRevealedToTeam)
		if (!temp->m_kCostCacheData.bCanEnterTerrainPermanent || !temp->m_kCostCacheData.bCanEnterTerritoryPermanent || temp->m_kCostCacheData.bIsNonEnemyCity)
			return false;

	if (temp->m_kCostCacheData.bPlotVisibleToTeam && temp->m_kCostCacheData.bUnitStackingLimitReached)
		if ((temp->m_kCostCacheData.iMoveFlags & CvUnit::MOVEFLAG_IGNORE_STACKING) == 0 || temp->m_kCostCacheData.bIsVisibleNeutralCombatUnit) //never ignore stacking for neutral units
			return false;

	if (temp->m_kCostCacheData.bPlotVisibleToTeam && !(temp->m_kCostCacheData.iMoveFlags & CvUnit::MOVEFLAG_ATTACK)) 
		if (temp->m_kCostCacheData.bIsEnemyCity || (temp->m_kCostCacheData.bIsVisibleEnemyCombatUnit && !temp->m_kCostCacheData.iMoveFlags && CvUnit::MOVEFLAG_IGNORE_ENEMIES))
			return false;

	return true;
}

bool CvTwoLayerPathFinder::CommonNeighborIsPassable(const CvAStarNode * a, const CvAStarNode * b) const
{
	//assume a and b are not direct neighbors ... don't check it because it's usually redundant
	if (!a || !b)
		return false;

	for (int i = 0; i < 6; i++)
	{
		CvAStarNode* check = a->m_apNeighbors[i];
		if (check && plotDistance(check->m_iX, check->m_iY, b->m_iX, b->m_iY) == 1)
			if (check->m_kCostCacheData.bCanEnterTerrainIntermediate && check->m_kCostCacheData.bCanEnterTerritoryIntermediate)
				return true;
	}

	return false;
}

// check if it makes sense to stop on the current node voluntarily (because the next one is not suitable for stopping)
bool CvTwoLayerPathFinder::AddStopNodeIfRequired(const CvAStarNode* current, const CvAStarNode* next)
{
	//we're stopping anyway - nothing to do
	if (current->m_iMoves == 0)
		return false;

	if (HaveFlag(CvUnit::MOVEFLAG_NO_STOPNODES))
		return false;

	//stop nodes don't make sense if we're only after the reachable plots
	if (!HasValidDestination())
		return false;

	//can't stop - nothing to do
	if (!CanEndTurnAtNode(current))
		return false;

	const UnitPathCacheData* pUnitDataCache = reinterpret_cast<const UnitPathCacheData*>(GetScratchBuffer());

	//there are two conditions where we might want to end the turn before proceeding
	// - next nodes is temporarily blocked because of stacking
	// - one or more tiles which cannot be entered permanently are ahead
	// - we would suffer attrition

	bool bBlockAhead = 
		!HaveFlag(CvUnit::MOVEFLAG_IGNORE_STACKING) && //obvious
		pUnitDataCache->isAIControl() &&	//only for AI units, for humans it's confusing and they can handle it anyway
		current->m_iTurns < 1 &&			//only in the first turn, otherwise the block will likely have moved
		next->m_iMoves == 0 &&				//only if we would need to end the turn on the next plot
		!next->m_kCostCacheData.bIsVisibleNeutralCombatUnit && //don't let ourselves be blocked by other players' units
		next->m_kCostCacheData.bUnitStackingLimitReached; //finally

	bool bTempPlotAhead =
		!next->m_kCostCacheData.bCanEnterTerrainPermanent;

	bool bAttrition = false;
	if (pUnitDataCache->pUnit && pUnitDataCache->pUnit->isHasPromotion((PromotionTypes)GC.getPROMOTION_UNWELCOME_EVANGELIST()))
	{
		CvPlot* pCurrentPlot = GC.getMap().plotUnchecked(current->m_iX, current->m_iY);
		CvPlot* pNextPlot = GC.getMap().plotUnchecked(next->m_iX, next->m_iY);
		bool bAttritionCurrent = (pCurrentPlot->isOwned() && !pCurrentPlot->IsFriendlyTerritory(pUnitDataCache->m_ePlayerID));
		bool bAttritionNext = (pNextPlot->isOwned() && !pNextPlot->IsFriendlyTerritory(pUnitDataCache->m_ePlayerID));

		bAttrition = (!bAttritionCurrent && bAttritionNext);
	}

	if (bBlockAhead || bTempPlotAhead || bAttrition)
	{
		CvPlot* pToPlot = GC.getMap().plot(current->m_iX, current->m_iY);
		int iEndTurnCost = PathEndTurnCost(pToPlot, current->m_kCostCacheData, pUnitDataCache, current->m_iTurns, HaveFlag(CvUnit::MOVEFLAG_AI_ABORT_IN_DANGER));
		if (iEndTurnCost < 0)
			return false;

		CvAStarNode* pStopNode = GetPartialMoveNode(current->m_iX, current->m_iY);
		UpdateNodeCacheData( pStopNode,pUnitDataCache->pUnit,this );

		//assume a stop here - do not add the cost for the wasted movement points!
		pStopNode->m_iMoves = 0;
		pStopNode->m_iTurns = current->m_iTurns;
		pStopNode->m_iHeuristicCost = current->m_iHeuristicCost;
		pStopNode->m_iStartMovesForTurn = current->m_iStartMovesForTurn;

		//cost is the same plus a little bit to encourage going the full distance when in doubt
		pStopNode->m_iKnownCost = current->m_iKnownCost + PATH_STEP_WEIGHT;
		pStopNode->m_iKnownCost += iEndTurnCost;
		pStopNode->m_iKnownCost += GC.getMOVE_DENOMINATOR() * PATH_BASE_COST; //some fixed cost for the forfeited movement points

		//we sort the nodes by total cost!
		pStopNode->m_iTotalCost = pStopNode->m_iKnownCost*giKnownCostWeight + pStopNode->m_iHeuristicCost*giHeuristicCostWeight;
		pStopNode->m_pParent = current->m_pParent;
		pStopNode->m_kCostCacheData = current->m_kCostCacheData;
		
		AddToOpen(pStopNode);
		return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
/// can do only certain types of path here
bool CvTwoLayerPathFinder::Configure(const SPathFinderUserData& config)
{
	//there is no good place to do this but we need to make sure the dangerplots are not dirty
	//otherwise there will be a recursive pathfinding call with unpredictable results
	if (config.ePlayer != NO_PLAYER && !HaveFlag(CvUnit::MOVEFLAG_IGNORE_DANGER))
	{
		CvUnit* pUnit = GET_PLAYER(config.ePlayer).getUnit(config.iUnitID);
		if (pUnit) //force an update before starting the actual pathfinding
			GET_PLAYER(config.ePlayer).GetPlotDanger(*pUnit->plot(), pUnit, UnitIdContainer());
	}

	switch(config.ePathType)
	{
	case PT_UNIT_MOVEMENT:
		SetFunctionPointers(PathDestValid, PathHeuristic, PathCost, PathValid, NULL, UnitPathInitialize, UnitPathUninitialize);
		m_iBasicPlotCost = PATH_BASE_COST*GC.getMOVE_DENOMINATOR();
		break;
	case PT_UNIT_REACHABLE_PLOTS:
		SetFunctionPointers(NULL, PathHeuristic, PathCost, PathValid, NULL, UnitPathInitialize, UnitPathUninitialize);
		m_iBasicPlotCost = PATH_BASE_COST*GC.getMOVE_DENOMINATOR();
		break;
	default:
		//not implemented here
		return false;
	}

	m_sData.ePathType = config.ePathType;
	return true;
}

void CvTwoLayerPathFinder::SanitizeFlags()
{
	if (m_sData.ePlayer==NO_PLAYER)
		return;

	CvUnit* pUnit = GET_PLAYER(m_sData.ePlayer).getUnit(m_sData.iUnitID);
	if (!pUnit)
		return;

	//ignore this flag if we'd be stuck otherwise
	if (m_sData.iFlags & CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY && GET_PLAYER(pUnit->getOwner()).IsAtWarWith(pUnit->plot()->getOwner()))
		m_sData.iFlags &= ~CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY;

}

//	--------------------------------------------------------------------------------
//default version for step paths - m_kCostCacheData is not valid
bool CvStepFinder::CanEndTurnAtNode(const CvAStarNode*) const
{
	return true; //can't check this without knowing the unit
}

bool CvStepFinder::CommonNeighborIsPassable(const CvAStarNode *, const CvAStarNode *) const
{
	return true; //can't check this without knowing the unit
}

//nothing to do in the stepfinder
bool CvStepFinder::AddStopNodeIfRequired(const CvAStarNode*, const CvAStarNode*)
{
	return false;
}

//////////////////////////////////////////////////////////////////////////
// CvPathFinder convenience functions
//////////////////////////////////////////////////////////////////////////
bool CvStepFinder::Configure(const SPathFinderUserData& config)
{
	switch(config.ePathType)
	{
	case PT_GENERIC_REACHABLE_PLOTS:
		SetFunctionPointers(NULL, StepHeuristic, StepCostEstimate, StepValidAnyArea, NULL, NULL, NULL);
		m_iBasicPlotCost = PATH_BASE_COST;
		break;
	case PT_GENERIC_SAME_AREA:
		SetFunctionPointers(StepDestValid, StepHeuristic, StepCost, StepValid, NULL, NULL, NULL);
		m_iBasicPlotCost = PATH_BASE_COST;
		break;
	case PT_GENERIC_ANY_AREA:
		SetFunctionPointers(NULL, StepHeuristic, StepCost, StepValidAnyArea, NULL, NULL, NULL);
		m_iBasicPlotCost = PATH_BASE_COST;
		break;
	case PT_GENERIC_SAME_AREA_WIDE:
		SetFunctionPointers(StepDestValid, StepHeuristic, StepCost, StepValidWide, NULL, NULL, NULL);
		m_iBasicPlotCost = PATH_BASE_COST;
		break;
	case PT_GENERIC_ANY_AREA_WIDE:
		SetFunctionPointers(NULL, StepHeuristic, StepCost, StepValidWideAnyArea, NULL, NULL, NULL);
		m_iBasicPlotCost = PATH_BASE_COST;
		break;
	case PT_TRADE_WATER:
		SetFunctionPointers(NULL, PathHeuristic, TradeRouteWaterPathCost, TradeRouteWaterValid, NULL, TradePathInitialize, TradePathUninitialize);
		m_iBasicPlotCost = PATH_BASE_COST;
		break;
	case PT_TRADE_LAND:
		SetFunctionPointers(NULL, StepHeuristic, TradeRouteLandPathCost, TradeRouteLandValid, NULL, TradePathInitialize, TradePathUninitialize);
		m_iBasicPlotCost = PATH_BASE_COST;
		break;
	case PT_BUILD_ROUTE:
		SetFunctionPointers(NULL, NULL, BuildRouteCost, BuildRouteValid, NULL, NULL, NULL);
		m_iBasicPlotCost = PATH_BASE_COST;
		break;
	case PT_BUILD_ROUTE_MIXED:
		SetFunctionPointers(NULL, NULL, BuildRouteCost, BuildRouteValid, CityConnectionGetExtraChildren, NULL, NULL);
		m_iBasicPlotCost = PATH_BASE_COST;
		break;
	case PT_AREA_CONNECTION:
		SetFunctionPointers(NULL, NULL, NULL, AreaValid, NULL, NULL, NULL);
		m_iBasicPlotCost = PATH_BASE_COST;
		break;
	case PT_LANDMASS_CONNECTION:
		SetFunctionPointers(NULL, NULL, NULL, LandmassValid, NULL, NULL, NULL);
		m_iBasicPlotCost = PATH_BASE_COST;
		break;
	case PT_CITY_INFLUENCE:
		SetFunctionPointers(InfluenceDestValid, StepHeuristic, InfluenceCost, InfluenceValid, NULL, NULL, NULL);
		m_iBasicPlotCost = PATH_BASE_COST;
		break;
	case PT_CITY_CONNECTION_LAND:
		SetFunctionPointers(NULL, StepHeuristic, NULL, CityConnectionLandValid, NULL, NULL, NULL);
		m_iBasicPlotCost = PATH_BASE_COST;
		break;
	case PT_CITY_CONNECTION_WATER:
		SetFunctionPointers(NULL, StepHeuristic, NULL, CityConnectionWaterValid, NULL, NULL, NULL);
		m_iBasicPlotCost = PATH_BASE_COST;
		break;
	case PT_CITY_CONNECTION_MIXED:
		SetFunctionPointers(NULL, StepHeuristic, NULL, CityConnectionLandValid, CityConnectionGetExtraChildren, NULL, NULL);
		m_iBasicPlotCost = PATH_BASE_COST;
		break;
	case PT_AIR_REBASE:
		SetFunctionPointers(NULL, StepHeuristic, NULL, RebaseValid, RebaseGetExtraChildren, UnitPathInitialize, UnitPathUninitialize);
		m_iBasicPlotCost = PATH_BASE_COST;
		break;
	default:
		//not implemented here
		return false;
	}

	m_sData.ePathType = config.ePathType;
	return true;
}

//	--------------------------------------------------------------------------------
/// configure the pathfinder and do the magic
///	atomic call, should be threadsafe
SPath CvPathFinder::GetPath(int iXstart, int iYstart, int iXdest, int iYdest, const SPathFinderUserData& data, TurnCountMode eMode)
{
	//make sure we don't call this from dll and lua at the same time
	bool bHadLock = gDLL->HasGameCoreLock();
	if(!bHadLock)
		gDLL->GetGameCoreLock();

	SPath result;
	if (Configure(data) && CvAStar::FindPathWithCurrentConfiguration(iXstart, iYstart, iXdest, iYdest, data))
		result = CvAStar::GetCurrentPath(eMode);

	if(!bHadLock)
		gDLL->ReleaseGameCoreLock();
	return result;
}

//wrapper for CvPlot*
SPath CvPathFinder::GetPath(const CvPlot* pStartPlot, const CvPlot* pEndPlot, const SPathFinderUserData& data, TurnCountMode eMode)
{
	if(pStartPlot == NULL || pEndPlot == NULL)
		return SPath();

	return GetPath(pStartPlot->getX(), pStartPlot->getY(), pEndPlot->getX(), pEndPlot->getY(), data, eMode);
}

//	--------------------------------------------------------------------------------
/// Check for existence of path between two points
bool CvPathFinder::DoesPathExist(int iXstart, int iYstart, int iXdest, int iYdest, const SPathFinderUserData& data)
{
	SPath path = GetPath(iXstart, iYstart, iXdest, iYdest, data);
	
	return !path.vPlots.empty();
}

//wrapper for CvPlot*
bool CvPathFinder::DoesPathExist(const CvPlot* pStartPlot, const CvPlot* pEndPlot, const SPathFinderUserData& data)
{
	if(pStartPlot == NULL || pEndPlot == NULL)
		return false;

	return DoesPathExist(pStartPlot->getX(), pStartPlot->getY(), pEndPlot->getX(), pEndPlot->getY(), data);
}

int CvPathFinder::GetPathLengthInPlots(int iXstart, int iYstart, int iXdest, int iYdest, const SPathFinderUserData & data)
{
	SPath path = GetPath(iXstart, iYstart, iXdest, iYdest, data);

	if (!path)
		return -1;
	else
		return path.vPlots.size();
}

int CvPathFinder::GetPathLengthInPlots(const CvPlot * pStartPlot, const CvPlot * pEndPlot, const SPathFinderUserData & data)
{
	if(pStartPlot == NULL || pEndPlot == NULL)
		return -1;

	return GetPathLengthInPlots(pStartPlot->getX(), pStartPlot->getY(), pEndPlot->getX(), pEndPlot->getY(), data);
}

int CvPathFinder::GetPathLengthInTurns(int iXstart, int iYstart, int iXdest, int iYdest, const SPathFinderUserData & data)
{
	SPath path = GetPath(iXstart, iYstart, iXdest, iYdest, data);

	if (!path)
		return -1;
	else
		return path.iTotalTurns;
}

int CvPathFinder::GetPathLengthInTurns(const CvPlot * pStartPlot, const CvPlot * pEndPlot, const SPathFinderUserData & data)
{
	if(pStartPlot == NULL || pEndPlot == NULL)
		return -1;

	return GetPathLengthInTurns(pStartPlot->getX(), pStartPlot->getY(), pEndPlot->getX(), pEndPlot->getY(), data);
}

//	--------------------------------------------------------------------------------
/// get all plots which can be reached in a certain amount of turns
ReachablePlots CvPathFinder::GetPlotsInReach(int iXstart, int iYstart, const SPathFinderUserData& data)
{
	//make sure we don't call this from dll and lua at the same time
	bool bHadLock = gDLL->HasGameCoreLock();
	if(!bHadLock)
		gDLL->GetGameCoreLock();

	if (!Configure(data))
	{
		if(!bHadLock)
			gDLL->ReleaseGameCoreLock();
		return ReachablePlots();
	}

	ReachablePlots plots;
	
	//there is no destination! the return value will always be false
	CvAStar::FindPathWithCurrentConfiguration(iXstart, iYstart, -1, -1, data);

	//iterate all previously touched nodes
	for (std::vector<CvAStarNode*>::const_iterator it=m_closedNodes.begin(); it!=m_closedNodes.end(); ++it)
	{
		CvAStarNode* temp = *it;

		bool bValid = true;

		if (temp->m_iTurns > data.iMaxTurns)
			bValid = false;
		else if (temp->m_iTurns == data.iMaxTurns && temp->m_iMoves < data.iMinMovesLeft)
			bValid = false;

		//need to check this here, during pathfinding we don't know that we're not just moving through
		//this is practially a PathDestValid check after the fact. also compare the PathCost turn end checks.
		bValid = bValid && CanEndTurnAtNode(temp);

		if (bValid)
		{
			int iNormalizedDistanceRaw = (temp->m_iKnownCost*SPath::getNormalizedDistanceBase()) / m_iBasicPlotCost + 1;
			plots.insert( SMovePlot(GC.getMap().plotNum(temp->m_iX, temp->m_iY),temp->m_iTurns,temp->m_iMoves,iNormalizedDistanceRaw) );
		}
	}

	if(!bHadLock)
		gDLL->ReleaseGameCoreLock();
	return plots;
}

ReachablePlots CvPathFinder::GetPlotsInReach(const CvPlot * pStartPlot, const SPathFinderUserData & data)
{
	if (!pStartPlot)
		return ReachablePlots();

	return GetPlotsInReach(pStartPlot->getX(),pStartPlot->getY(),data);
}

map<CvPlot*,SPath> CvPathFinder::GetMultiplePaths(const CvPlot* pStartPlot, vector<CvPlot*> vDestPlots, const SPathFinderUserData& data)
{
	//make sure we don't call this from dll and lua at the same time
	bool bHadLock = gDLL->HasGameCoreLock();
	if(!bHadLock)
		gDLL->GetGameCoreLock();

	map<CvPlot*,SPath> result;

	if (!Configure(data) || !pStartPlot)
	{
		if(!bHadLock)
			gDLL->ReleaseGameCoreLock();
		return result;
	}

	//sort for fast search
	struct PrSortByPlotIndex
	{
		bool operator()(const CvPlot* lhs, const CvPlot* rhs) const { return lhs->GetPlotIndex() < rhs->GetPlotIndex(); }
	};
	std::sort( vDestPlots.begin(), vDestPlots.end(), PrSortByPlotIndex() );

	//there is no destination! the return value will always be false
	CvAStar::FindPathWithCurrentConfiguration(pStartPlot->getX(),pStartPlot->getY(), -1, -1, data);

	//iterate all previously touched nodes
	for (std::vector<CvAStarNode*>::const_iterator it=m_closedNodes.begin(); it!=m_closedNodes.end(); ++it)
	{
		CvAStarNode* temp = *it;

		if (temp->m_iTurns > data.iMaxTurns)
			continue;
		if (temp->m_iTurns == data.iMaxTurns && temp->m_iMoves < data.iMinMovesLeft)
			continue;

		std::pair<std::vector<CvPlot*>::iterator,std::vector<CvPlot*>::iterator> bounds =
			std::equal_range( vDestPlots.begin(), vDestPlots.end(), GC.getMap().plot(temp->m_iX,temp->m_iY), PrSortByPlotIndex() );

		if (bounds.first != bounds.second)
		{
			//need to check this here, during pathfinding we don't know that we're not just moving through
			//this is practially a PathDestValid check after the fact. also compare the PathCost turn end checks.
			if (CanEndTurnAtNode(temp))
			{
				SPath path;
				path.iTurnSliceGenerated = GC.getGame().getTurnSlice();
				path.sConfig = m_sData;
				path.iTotalCost = temp->m_iKnownCost;
				path.iNormalizedDistanceRaw = (temp->m_iKnownCost * SPath::getNormalizedDistanceBase())  / m_iBasicPlotCost + 1;
				path.iTotalTurns = temp->m_iTurns;

				CvAStarNode* node = temp;
				while (node)
				{
					path.vPlots.push_back( SPathNode(node) );

					//failsafe
					if (node == node->m_pParent || path.vPlots.size() > 10000)
						break;

					node = node->m_pParent;
				}

				//make it so that the destination comes last
				std::reverse(path.vPlots.begin(),path.vPlots.end());

				//store it
				result[ *bounds.first ] = path;
			}

			//don't need to check this again
			vDestPlots.erase(bounds.first);
		}
	}

	if(!bHadLock)
		gDLL->ReleaseGameCoreLock();
	return result;
}

//	--------------------------------------------------------------------------------
/// Get the plot X from the end of the step path
CvPlot* PathHelpers::GetXPlotsFromEnd(const SPath& path, int iPlotsFromEnd, bool bLeaveEnemyTerritory)
{
	CvPlot* currentPlot = NULL;
	PlayerTypes eEnemy = (PlayerTypes)path.sConfig.iTypeParameter;

	int iPathLen = path.vPlots.size();
	int iIndex = iPathLen-iPlotsFromEnd;
	while (iIndex>=0)
	{
		currentPlot = path.get(iIndex);
	
		// Was an enemy specified and we don't want this plot to be in enemy territory?
		if (bLeaveEnemyTerritory && eEnemy != NO_PLAYER && currentPlot->getOwner() == eEnemy)
			iIndex--;
		else
			break;
	}

	return currentPlot;
}

//	--------------------------------------------------------------------------------
int PathHelpers::CountPlotsOwnedByXInPath(const SPath& path, PlayerTypes ePlayer)
{
	int iCount = 0;
	for (int i=0; i<path.length(); i++)
	{
		CvPlot* currentPlot = path.get(i);

		// Check and see if this plot has the right owner
		if(currentPlot->getOwner() == ePlayer)
			iCount++;
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int PathHelpers::CountPlotsOwnedAnyoneInPath(const SPath& path, PlayerTypes eExceptPlayer)
{
	int iCount = 0;
	for (int i=0; i<path.length(); i++)
	{
		CvPlot* currentPlot = path.get(i);

		// Check and see if this plot has an owner that isn't us.
		if(currentPlot->getOwner() != eExceptPlayer && currentPlot->getOwner() != NO_PLAYER)
			iCount++;
	}

	return iCount;
}

//	--------------------------------------------------------------------------------
/// Retrieve first node of path
CvPlot* PathHelpers::GetPathFirstPlot(const SPath& path)
{
	//this is tricky - the first plot is actually the starting point, so we return the second one!
	return path.get(1);
}

//	--------------------------------------------------------------------------------
/// Return the furthest plot we can get to this turn that is on the path
// compare with PathNodeArray::GetEndTurnPlot()
CvPlot* PathHelpers::GetPathEndFirstTurnPlot(const SPath& path)
{
	int iNumNodes = path.vPlots.size();
	if (iNumNodes>1)
	{
		//return the plot before the next turn starts
		for (int i=1; i<iNumNodes; i++)
		{
			if ( path.vPlots[i].turns>1 )
				return path.get(i-1);
		}

		//if all plots are within the turn, return the last one
		return path.get(iNumNodes-1);
	}
	else if (iNumNodes==1)
		//not much choice
		return path.get(0);

	//empty path ...
	return NULL;
}

//	---------------------------------------------------------------------------
int RebaseValid(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData&, const CvAStar* finder)
{
	if(parent == NULL)
		return TRUE;

	CvPlot* pNewPlot = GC.getMap().plotUnchecked(node->m_iX, node->m_iY);
	const UnitPathCacheData* pCacheData = reinterpret_cast<const UnitPathCacheData*>(finder->GetScratchBuffer());
	const CvUnit* pUnit = pCacheData->pUnit;
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
int RebaseGetExtraChildren(const CvAStarNode* node, const CvAStar* finder, vector<pair<int,int>>& out)
{
	out.clear();
	CvPlayer& kPlayer = GET_PLAYER(finder->GetData().ePlayer);

	CvPlot* pPlot = GC.getMap().plotCheckInvalid(node->m_iX, node->m_iY);
	if(!pPlot)
		return 0;

	CvCity* pCity = pPlot->getPlotCity();
	if (!pCity)
		return 0;

	// if there is a city and the city is on our team
	std::vector<int> vNeighbors = pCity->GetClosestFriendlyNeighboringCities();
	for (size_t i=0; i<vNeighbors.size(); i++)
	{
		CvCity* pSecondCity = kPlayer.getCity(vNeighbors[i]);
		if (pSecondCity)
			out.push_back(make_pair(pSecondCity->getX(), pSecondCity->getY()));
	}

	std::vector<int> vAttachedUnits = pCity->GetAttachedUnits();
	for (size_t i=0; i<vAttachedUnits.size(); i++)
	{
		CvUnit* pCarrier = kPlayer.getUnit(vAttachedUnits[i]);
		if (pCarrier)
			out.push_back(make_pair(pCarrier->getX(), pCarrier->getY()));
	}

	return (int)out.size();
}

// A structure holding some unit values that are invariant during a path plan operation
struct TradePathCacheData
{
	PlayerTypes m_ePlayer;
	TeamTypes m_eTeam;
	bool m_bCanCrossOcean:1;
	bool m_bCanCrossMountain:1;
	bool m_bIsRiverTradeRoad:1;
	bool m_bIsWoodlandMovementBonus:1;

	inline PlayerTypes GetPlayer() const { return m_ePlayer; }
	inline TeamTypes GetTeam() const { return m_eTeam; }
	inline bool CanCrossOcean() const { return m_bCanCrossOcean; }
	inline bool CanCrossMountain() const { return m_bCanCrossMountain; }
	inline bool IsRiverTradeRoad() const { return m_bIsRiverTradeRoad; }
	inline bool IsWoodlandMovementBonus() const { return m_bIsWoodlandMovementBonus; }
};

//	--------------------------------------------------------------------------------
void TradePathInitialize(const SPathFinderUserData& data, CvAStar* finder)
{
	TradePathCacheData* pCacheData = reinterpret_cast<TradePathCacheData*>(finder->GetScratchBufferDirty());

	if (data.ePlayer!=NO_PLAYER)
	{
		CvPlayer& kPlayer = GET_PLAYER(data.ePlayer);
		pCacheData->m_ePlayer = data.ePlayer;
		pCacheData->m_eTeam = kPlayer.getTeam();
		pCacheData->m_bCanCrossOcean = kPlayer.CanCrossOcean() || GET_TEAM(kPlayer.getTeam()).canEmbarkAllWaterPassage();
		pCacheData->m_bCanCrossMountain = kPlayer.CanCrossMountain();

		CvPlayerTraits* pPlayerTraits = kPlayer.GetPlayerTraits();
		if (pPlayerTraits)
		{
			pCacheData->m_bIsRiverTradeRoad = pPlayerTraits->IsRiverTradeRoad();
			pCacheData->m_bIsWoodlandMovementBonus = pPlayerTraits->IsWoodlandMovementBonus();
		}
		else
		{
			pCacheData->m_bIsRiverTradeRoad = false;
			pCacheData->m_bIsWoodlandMovementBonus = false;
		}
	}
	else
	{
		pCacheData->m_ePlayer = NO_PLAYER;
		pCacheData->m_eTeam = NO_TEAM;
		pCacheData->m_bCanCrossOcean = false;
		pCacheData->m_bCanCrossMountain = false;
		pCacheData->m_bIsRiverTradeRoad = false;
		pCacheData->m_bIsWoodlandMovementBonus = false;
	}

}

//	--------------------------------------------------------------------------------
void TradePathUninitialize(const SPathFinderUserData&, CvAStar*)
{

}

//	--------------------------------------------------------------------------------
int TradeRouteLandPathCost(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData&, CvAStar* finder)
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

	int iCost = PATH_BASE_COST;

	// no route
	int iRouteFactor = 1;

	// super duper low costs for moving along routes - don't check for pillaging
	if (pFromPlot->getRouteType() == ROUTE_RAILROAD && pToPlot->isRoute())
		iRouteFactor = (pToPlot->getRouteType() == ROUTE_RAILROAD) ? 4 : 3;
	else if (pFromPlot->getRouteType() == ROUTE_ROAD && pToPlot->isRoute())
		iRouteFactor = 3; //can't get better than this even if next plot is railroad
	// low costs for moving along rivers
	else if (pFromPlot->isRiver() && pToPlot->isRiver() && (pFromPlot->isCity() || pToPlot->isCity() || !(pFromPlot->isRiverCrossing(directionXY(pFromPlot, pToPlot)))))
		iRouteFactor = 2;
	// Iroquois ability
	else if ((eFeature == FEATURE_FOREST || eFeature == FEATURE_JUNGLE) && pCacheData->IsWoodlandMovementBonus())
		iRouteFactor = 2;

	// apply route discount
	iCost /= iRouteFactor;

	//try to avoid rough plots
	if (pToPlot->isRoughGround() && iRouteFactor==1)
		iCost += PATH_BASE_COST/2;

	//bonus for oasis
	if (eFeature == FEATURE_OASIS && iRouteFactor==1)
		iCost -= PATH_BASE_COST/4;
	
	// avoid enemy lands
	TeamTypes eToPlotTeam = pToPlot->getTeam();
	if (pCacheData->GetTeam()!=NO_TEAM && eToPlotTeam != NO_TEAM && GET_TEAM(pCacheData->GetTeam()).isAtWar(eToPlotTeam))
		iCost += PATH_BASE_COST*10;

	return iCost;
}

//	--------------------------------------------------------------------------------
int TradeRouteLandValid(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData&, const CvAStar* finder)
{
	if(parent == NULL)
		return TRUE;

	const TradePathCacheData* pCacheData = reinterpret_cast<const TradePathCacheData*>(finder->GetScratchBuffer());
	CvMap& kMap = GC.getMap();
	CvPlot* pToPlot = kMap.plotUnchecked(node->m_iX, node->m_iY);

	if (pToPlot->isCity())
	{
		return TRUE;
	}

	if (pToPlot->isWater() || !pToPlot->isRevealed(pCacheData->GetTeam()))
	{
		return FALSE;
	}

	if (pToPlot->getImprovementType()==(ImprovementTypes)GC.getBARBARIAN_CAMP_IMPROVEMENT())
	{
		return FALSE;
	}

	if(!pToPlot->isValidMovePlot( pCacheData->GetPlayer(), false ))
	{
		return FALSE;
	}

	return TRUE;
}

//	--------------------------------------------------------------------------------

int TradeRouteWaterPathCost(const CvAStarNode*, const CvAStarNode* node, const SPathFinderUserData&, CvAStar* finder)
{
	CvMap& kMap = GC.getMap();
	const TradePathCacheData* pCacheData = reinterpret_cast<const TradePathCacheData*>(finder->GetScratchBuffer());

	int iToPlotX = node->m_iX;
	int iToPlotY = node->m_iY;
	CvPlot* pToPlot = kMap.plotUnchecked(iToPlotX, iToPlotY);

	int iCost = PATH_BASE_COST;

	// prefer the coastline (not identical with coastal water)
	if (pToPlot->isWater() && !pToPlot->isAdjacentToLand())
		iCost += PATH_BASE_COST/4;

	// avoid cities (just for the looks)
	if (pToPlot->isCityOrPassableImprovement(pCacheData->GetPlayer(),false))
		iCost += PATH_BASE_COST*2;

	// avoid enemy territory
	TeamTypes eToPlotTeam = pToPlot->getTeam();
	if (pCacheData->GetTeam()!=NO_TEAM && eToPlotTeam!=NO_TEAM && GET_TEAM(pCacheData->GetTeam()).isAtWar(eToPlotTeam))
		iCost += PATH_BASE_COST*10;

	return iCost;
}

//	--------------------------------------------------------------------------------
int TradeRouteWaterValid(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData&, const CvAStar* finder)
{
	if(parent == NULL)
		return TRUE;

	const TradePathCacheData* pCacheData = reinterpret_cast<const TradePathCacheData*>(finder->GetScratchBuffer());

	CvMap& kMap = GC.getMap();
	CvPlot* pNewPlot = kMap.plotUnchecked(node->m_iX, node->m_iY);

	if (!pNewPlot->isRevealed(pCacheData->GetTeam()))
		return FALSE;

	//ice in unowned territory is not allowed
	if (pNewPlot->isIce() && !pNewPlot->isOwned())
		return FALSE;

	//ocean needs trait or tech
	if (pNewPlot->isDeepWater())
		return pCacheData->CanCrossOcean();

	//coast is always ok
	if (pNewPlot->isShallowWater())
		return TRUE;

	//check passable improvements
	if(pNewPlot->isCityOrPassableImprovement(pCacheData->GetPlayer(),false) && pNewPlot->isAdjacentToShallowWater() )
		return TRUE;

	return FALSE;
}

//	---------------------------------------------------------------------------
CvPlot* CvPathNodeArray::GetTurnDestinationPlot(int iTurn) const
{
	if (empty() || iTurn<0)
		return NULL;

	//walk backwards and return the first (last) match
	for (size_t i = size(); i != 0; i--)
	{
		const CvPathNode& thisNode = at(i-1);
		if (thisNode.m_iTurns == iTurn)
			return GC.getMap().plotUnchecked(thisNode.m_iX, thisNode.m_iY);
	}

	return NULL;
}

CvPlot* CvPathNodeArray::GetFinalPlot() const
{
	if (empty())
		return NULL;

	return GC.getMap().plotUnchecked( back().m_iX, back().m_iY );
}

CvPlot* CvPathNodeArray::GetFirstPlot() const
{
	if (empty())
		return NULL;

	return GC.getMap().plotUnchecked( front().m_iX, front().m_iY );
}

CvPlot * CvPathNodeArray::GetPlotByIndex(int iIndex) const
{
	if (iIndex >= 0 && iIndex < (int)size())
		return GC.getMap().plotUnchecked( at(iIndex).m_iX, at(iIndex).m_iY );

	return NULL;
}

//	---------------------------------------------------------------------------
bool IsPlotConnectedToPlot(PlayerTypes ePlayer, CvPlot* pFromPlot, CvPlot* pToPlot, RouteTypes eRestrictRoute, bool bIgnoreHarbors, SPath* pPathOut)
{
	if (ePlayer==NO_PLAYER || pFromPlot==NULL || pToPlot==NULL)
		return false;

	SPathFinderUserData data(ePlayer, bIgnoreHarbors ? PT_CITY_CONNECTION_LAND : PT_CITY_CONNECTION_MIXED, eRestrictRoute);
	SPath result;
	if (!pPathOut)
		pPathOut = &result;

	*pPathOut = GC.GetStepFinder().GetPath(pFromPlot->getX(), pFromPlot->getY(), pToPlot->getX(), pToPlot->getY(), data);
	return pPathOut->iTotalCost != -1;
}

//	---------------------------------------------------------------------------
//convenience constructor
SPathFinderUserData::SPathFinderUserData(const CvUnit* pUnit, int _iFlags, int _iMaxTurns)
{
	ePathType = PT_UNIT_MOVEMENT;
	iFlags = _iFlags;
	iMaxTurns = _iMaxTurns;
	ePlayer = pUnit ? pUnit->getOwner() : NO_PLAYER;
	iUnitID = pUnit ? pUnit->GetID() : 0;
	iTypeParameter = -1; //typical invalid enum
	iMaxNormalizedDistance = INT_MAX;
	iMinMovesLeft = 0;
	iStartMoves = pUnit->getMoves();
}

//	---------------------------------------------------------------------------
//convenience constructor
SPathFinderUserData::SPathFinderUserData(PlayerTypes _ePlayer, PathType _ePathType, int _iTypeParameter, int _iMaxTurns)
{
	ePathType = _ePathType;
	iFlags = 0;
	ePlayer = _ePlayer;
	iUnitID = 0;
	iTypeParameter = _iTypeParameter;
	iMaxTurns = _iMaxTurns;
	iMaxNormalizedDistance = INT_MAX;
	iMinMovesLeft = 0;
	iStartMoves = 0;
}

CvPlot * SPath::get(int i) const
{
	if (i<(int)vPlots.size())
		return GC.getMap().plotUnchecked(vPlots[i].x,vPlots[i].y);

	return NULL;
}

int SPath::getNormalizedDistanceBase()
{
	return NORM_COST_BASE;
}

ReachablePlots::iterator ReachablePlots::find(int iPlotIndex)
{
	std::tr1::unordered_map<int,size_t>::iterator it = lookup.find(iPlotIndex);
	if (it!=lookup.end())
		return storage.begin() + it->second;

	return storage.end();
}

ReachablePlots::const_iterator ReachablePlots::find(int iPlotIndex) const
{
	std::tr1::unordered_map<int,size_t>::const_iterator it = lookup.find(iPlotIndex);
	if (it!=lookup.end())
		return storage.begin() + it->second;

	return storage.end();
}

void ReachablePlots::insert(const SMovePlot& plot)
{
	lookup[plot.iPlotIndex] = storage.size();
	storage.push_back(plot);
}
