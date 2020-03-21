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
//
//  PURPOSE: A* Pathfinding - based off of A* Explorer from "AI Game Programming Wisdom"
//

#ifndef		CVASTAR_H
#define		CVASTAR_H
#pragma		once

#include "CvAStarNode.h"
#include <queue>

#define PATH_BASE_COST (120) //base cost per plot respectively movement point expended

class CvAStar;
typedef int(*CvAPointFunc)(int, int, const SPathFinderUserData&, const CvAStar*);
typedef int(*CvAHeuristic)(int, int, int, int, int, int);
typedef int(*CvAStarFunc)(CvAStarNode*, CvAStarNode*, CvAStarNodeAddOp, const SPathFinderUserData&, CvAStar*);
typedef int(*CvAStarConst1Func)(const CvAStarNode*, const CvAStarNode*, const SPathFinderUserData&, CvAStar*);
typedef int(*CvAStarConst2Func)(const CvAStarNode*, const CvAStarNode*, const SPathFinderUserData&, const CvAStar*);
typedef int(*CvAGetExtraChildren)(const CvAStarNode*, const CvAStar*, vector<pair<int,int>>&);
typedef void(*CvABegin)(const SPathFinderUserData&, CvAStar*);
typedef void(*CvAEnd)(const SPathFinderUserData&, CvAStar*);

enum TurnCountMode
{
	TC_GAMECORE,	//turns start at zero, continue with one after turn end
	TC_UI,			//turns start at zero, continue with one at turn end
	TC_DEBUG		//turns are in fact move costs. needs a modified PathHelpManager.lua to show it in the UI
};

enum NodeState
{
	NS_INITIAL_FINAL = 0,
	NS_CURRENT = 1,
	NS_VALID = 2,
	NS_INVALID = 3,
	NS_OBSOLETE = 4,
	NS_FORBIDDEN = 5,
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  CLASS:      CvAStar
//
//  DESC:       Generic pathfinding algorithm
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAStar
{

	//--------------------------------------- PUBLIC METHODS -------------------------------------------
public:
	CvAStar();
	~CvAStar();

	// allocate memory
	virtual void Initialize(int iColumns, int iRows, bool bWrapX, bool bWrapY);
	virtual void DeInit();
	virtual bool IsInitialized(int iXstart, int iYstart, int iXdest, int iYdest);

	// Generates a path
	bool FindPathWithCurrentConfiguration(int iXstart, int iYstart, int iXdest, int iYdest, const SPathFinderUserData& data);

	// Verifies if the given path is still valid
	bool VerifyPath(const SPath& path);

	// Get the result
	SPath GetCurrentPath(TurnCountMode eMode) const;

	inline bool HaveFlag(int iFlag) const
	{
		return ((m_sData.iFlags & iFlag)==iFlag);
	}

	const char* GetName() const { return m_strName.c_str(); }
	void SetName(const char* pName) { m_strName = pName; }

	const void* GetScratchBuffer() const { return m_ScratchBuffer; }
	void* GetScratchBufferDirty() { return m_ScratchBuffer; }

	inline void SetTempResult(int iMoves, int iTurns)
	{ 
		m_iMovesCached = iMoves; m_iTurnsCached = iTurns;
	}
	inline int GetCachedMoveCount() const
	{ 
		return m_iMovesCached;
	}
	inline int GetCachedTurnCount() const
	{ 
		return m_iTurnsCached;
	}

	inline int GetStartX() const
	{
		return m_iXstart;
	}

	inline int GetStartY() const
	{
		return m_iYstart;
	}

	inline int GetDestX() const
	{
		return m_iXdest;
	}

	inline int GetDestY() const
	{
		return m_iYdest;
	}

	inline bool IsPathStart(int iX, int iY) const
	{
		return ((m_iXstart == iX) && (m_iYstart == iY));
	}

	inline bool IsPathDest(int iX, int iY) const
	{
		return DestinationReached(iX,iY);
	}

	inline bool HasValidDestination() const
	{
		return isValid(m_iXdest,m_iYdest);
	}

	inline unsigned short GetCurrentGenerationID() const
	{
		return m_iCurrentGenerationID;
	}

	int GetExtraChildren(const CvAStarNode* node, vector<pair<int,int>>& out) const;

	void AddToOpen(CvAStarNode* addnode);

	inline const SPathFinderUserData& GetData() const
	{
		return m_sData;
	}

	//has to be public for the free functions to access it
	const CvAStarNode* GetNode(int iCol, int iRow) const;
	CvAStarNode* GetNodeMutable(int iCol, int iRow) const;

	inline bool IsApproximateMode() const
	{
		return HaveFlag(CvUnit::MOVEFLAG_APPROX_TARGET_RING1) || HaveFlag(CvUnit::MOVEFLAG_APPROX_TARGET_RING2);
	}

	virtual bool CanEndTurnAtNode(const CvAStarNode* temp) const = 0;
	virtual bool CommonNeighborIsPassable(const CvAStarNode* a, const CvAStarNode* b) const = 0;
	virtual bool AddStopNodeIfRequired(const CvAStarNode* current, const CvAStarNode* next) = 0;
	virtual bool DestinationReached(int iToX, int iToY) const = 0;
	virtual void NodeAddedToPath(CvAStarNode* parent, CvAStarNode* node, int iKnownCost, CvAStarNodeAddOp operation) = 0;

	//--------------------------------------- PROTECTED FUNCTIONS -------------------------------------------
protected:

	// Pure virtual method to set up function pointers. needs to be implemented in derived class
	virtual bool Configure(const SPathFinderUserData& config) = 0;

	// Failsafe. Check if the passed flags make sense
	virtual void SanitizeFlags() = 0;

	inline int GetNormalizedLength() const
	{
		if (m_pBest)
		{
			int iCost = m_pBest->m_iKnownCost;
			return iCost / m_iBasicPlotCost;
		}
		else
			return INT_MAX;
	}

	virtual void SetFunctionPointers(CvAPointFunc DestValidFunc, CvAHeuristic HeuristicFunc, CvAStarConst1Func CostFunc, CvAStarConst2Func ValidFunc,  
		CvAGetExtraChildren GetExtraChildrenFunc, CvABegin InitializeFunc, CvAEnd UninitializeFunc);

	void Reset();

	CvAStarNode* GetBest();

	void CreateChildren(CvAStarNode* node);
	NodeState LinkChild(CvAStarNode* node, CvAStarNode* check);
	void UpdateDownstreamNodes(CvAStarNode* node);

	inline int xRange(int iX) const;
	inline int yRange(int iY) const;
	inline bool isValid(int iX, int iY) const;

	void PrecalcNeighbors(CvAStarNode* node);

	inline int udFunc(CvAStarConst1Func func, const CvAStarNode* param1, const CvAStarNode* param2, const SPathFinderUserData& data);
	inline int udFunc(CvAStarConst2Func func, const CvAStarNode* param1, const CvAStarNode* param2, const SPathFinderUserData& data) const;

	//--------------------------------------- PROTECTED DATA -------------------------------------------
protected:
	CvAPointFunc udDestValid;				    // Determines destination is valid
	CvAHeuristic udHeuristic;				    // Determines heuristic cost
	CvAStarConst1Func udCost;					// Called when cost value is need
	CvAStarConst2Func udValid;					// Called to check validity of a coordinate
	CvAGetExtraChildren udGetExtraChildrenFunc; // Get the extra children nodes
	CvABegin udInitializeFunc;					// Called at the start, to initialize any run specific data
	CvAEnd udUninitializeFunc;					// Called at the end to uninitialize any run specific data

	SPathFinderUserData m_sData;				// Data passed back to functions
	unsigned short m_iCurrentGenerationID;		// the cache in each node is tagged with a generation ID which is incremented for each call

	int m_iColumns;	
	int m_iRows;
	int m_iXstart;
	int m_iYstart;
	int m_iXdest;
	int m_iYdest;
	int m_iDestHitCount;

	bool m_bWrapX;
	bool m_bWrapY;
	bool m_bHeapDirty;

	//vector performance is better than lists!
	std::vector<CvAStarNode*> m_openNodes;
	std::vector<CvAStarNode*> m_closedNodes;

	// The best node - just popped from open nodes. walking backwards from here gives the current path
	CvAStarNode* m_pBest;

	int m_iProcessedNodes;			// for statistics
	int m_iTestedNodes;
	int m_iRounds;

	CvAStarNode** m_ppaaNodes;
	CvAStarNode** m_ppaaNeighbors;

	// Scratch buffer
	char  m_ScratchBuffer[512];	// Will NOT be modified directly by CvAStar

	int m_iBasicPlotCost;		// for length normalization

	int m_iMovesCached;			// don't calculate this twice per node
	int m_iTurnsCached;

	//for debugging
	CvString m_strName;

	CRITICAL_SECTION m_cs;
};

inline int CvAStar::xRange(int iX) const
{
	if(m_bWrapX)
	{
		if(iX < 0)
		{
			return (m_iColumns + (iX % m_iColumns));
		}
		else if(iX >= m_iColumns)
		{
			return (iX % m_iColumns);
		}
		else
		{
			return iX;
		}
	}
	else
	{
		return iX;
	}
}


inline int CvAStar::yRange(int iY) const
{
	if(m_bWrapY)
	{
		if(iY < 0)
		{
			return (m_iRows + (iY % m_iRows));
		}
		else if(iY >= m_iRows)
		{
			return (iY % m_iRows);
		}
		else
		{
			return iY;
		}
	}
	else
	{
		return iY;
	}
}


inline bool CvAStar::isValid(int iX, int iY) const
{
	if((iX < 0) || (iX >= m_iColumns))
	{
		return false;
	}

	if((iY < 0) || (iY >= m_iRows))
	{
		return false;
	}

	return true;
}

inline int CvAStar::udFunc(CvAStarConst1Func func, const CvAStarNode* param1, const CvAStarNode* param2, const SPathFinderUserData& data)
{
	return (func) ? func(param1, param2, data, this) : PATH_BASE_COST;
}

inline int CvAStar::udFunc(CvAStarConst2Func func, const CvAStarNode* param1, const CvAStarNode* param2, const SPathFinderUserData& data) const
{
	return (func) ? func(param1, param2, data, this) : PATH_BASE_COST;
}

//-------------------------------------------------------------------------------------------------
// Derived class with some additional convenience functions
//-------------------------------------------------------------------------------------------------
class CvPathFinder : public CvAStar
{
public:
	virtual SPath GetPath(int iXstart, int iYstart, int iXdest, int iYdest, const SPathFinderUserData& data, TurnCountMode eMode=TC_GAMECORE);
	virtual SPath GetPath(const CvPlot* pStartPlot, const CvPlot* pEndPlot, const SPathFinderUserData& data, TurnCountMode eMode=TC_GAMECORE);
	virtual bool DoesPathExist(int iXstart, int iYstart, int iXdest, int iYdest, const SPathFinderUserData& data);
	virtual bool DoesPathExist(const CvPlot* pStartPlot, const CvPlot* pEndPlot, const SPathFinderUserData& data);
	virtual int GetPathLengthInPlots(int iXstart, int iYstart, int iXdest, int iYdest, const SPathFinderUserData& data);
	virtual int GetPathLengthInPlots(const CvPlot* pStartPlot, const CvPlot* pEndPlot, const SPathFinderUserData& data);
	virtual int GetPathLengthInTurns(int iXstart, int iYstart, int iXdest, int iYdest, const SPathFinderUserData& data);
	virtual int GetPathLengthInTurns(const CvPlot* pStartPlot, const CvPlot* pEndPlot, const SPathFinderUserData& data);
	virtual ReachablePlots GetPlotsInReach(int iXstart, int iYstart, const SPathFinderUserData& data);
	virtual ReachablePlots GetPlotsInReach(const CvPlot* pStartPlot, const SPathFinderUserData& data);
	virtual map<CvPlot*,SPath> GetMultiplePaths(const CvPlot* pStartPlot, vector<CvPlot*> vDestPlots, const SPathFinderUserData& data);
	virtual bool DestinationReached(int iToX, int iToY) const;
};

//-------------------------------------------------------------------------------------------------
// Derived class for abstract pathfinding without complex unit movement rules
//-------------------------------------------------------------------------------------------------
class CvStepFinder : public CvPathFinder
{
	virtual bool CanEndTurnAtNode(const CvAStarNode* temp) const;
	virtual bool CommonNeighborIsPassable(const CvAStarNode* a, const CvAStarNode* b) const;
	virtual bool AddStopNodeIfRequired(const CvAStarNode* current, const CvAStarNode* next);
	virtual void NodeAddedToPath(CvAStarNode* parent, CvAStarNode* node, int iKnownCost, CvAStarNodeAddOp operation);

protected:
	virtual bool Configure(const SPathFinderUserData& config);
	virtual void SanitizeFlags() {} //no-op
};

//-------------------------------------------------------------------------------------------------
// Since units may stop at any point on their path before their movement points are exhausted, we need to have two layers of nodes, one with a hypothetical stop, one without.
// Usecase is eg a unit blocking a road tile, so we cannot end the turn there. The best solution is to waste some movement points and pass the blocking unit in the next turn.
//-------------------------------------------------------------------------------------------------
class CvTwoLayerPathFinder: public CvPathFinder
{
public:
	CvTwoLayerPathFinder();
	~CvTwoLayerPathFinder();

	virtual void Initialize(int iColumns, int iRows, bool bWrapX, bool bWrapY);
	virtual void DeInit();

	//has to be public for the free functions to access it
	CvAStarNode* GetPartialMoveNode(int iCol, int iRow);
	virtual bool CanEndTurnAtNode(const CvAStarNode* temp) const;
	virtual bool CommonNeighborIsPassable(const CvAStarNode* a, const CvAStarNode* b) const;
	virtual bool AddStopNodeIfRequired(const CvAStarNode* current, const CvAStarNode* next);
	virtual void NodeAddedToPath(CvAStarNode* parent, CvAStarNode* node, int iKnownCost, CvAStarNodeAddOp operation);

protected:
	// set the function pointers which do the actual work
	virtual bool Configure(const SPathFinderUserData& config);
	// Failsafe. Check if the passed flags make sense
	virtual void SanitizeFlags();
private:
	CvAStarNode** m_ppaaPartialMoveNodes;
};

//this is for unit pathfinding (twolayer pathfinder)
int PathDestValid(int iToX, int iToY, const SPathFinderUserData& data, const CvAStar* finder);
int PathValid(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar* finder);
int PathHeuristic(int iCurrentX, int iCurrentY, int iNextX, int iNextY, int iDestX, int iDestY);
int PathCost(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, CvAStar* finder);

//this is for general pathfinding (step finder)
int StepHeuristic(int iCurrentX, int iCurrentY, int iNextX, int iNextY, int iDestX, int iDestY);
int StepDestValid(int iToX, int iToY, const SPathFinderUserData& data, const CvAStar* finder);
int StepValid(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar* finder);
int StepValidAnyArea(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar* finder);
int StepValidWide(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar* finder);
int StepValidWideAnyArea(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar* finder);
int StepCost(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, CvAStar* finder);

int CityConnectionLandValid(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar* finder);
int CityConnectionGetExtraChildren(const CvAStarNode* node, const CvAStar* finder, vector<pair<int,int>>& out);
int CityConnectionWaterValid(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar* finder);

int AreaValid(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar* finder);
int LandmassValid(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar* finder);

int InfluenceDestValid(int iToX, int iToY, const SPathFinderUserData& data, const CvAStar* finder);
int InfluenceValid(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar* finder);
int InfluenceCost(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, CvAStar* finder);

int BuildRouteCost(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, CvAStar* finder);
int BuildRouteValid(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar* finder);

int RebaseValid(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar* finder);
int RebaseGetExtraChildren(const CvAStarNode* node, const CvAStar* finder, vector<pair<int,int>>& out);

int TradeRouteLandPathCost(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, CvAStar* finder);
int TradeRouteLandValid(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar* finder);
int TradeRouteWaterPathCost(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, CvAStar* finder);
int TradeRouteWaterValid(const CvAStarNode* parent, const CvAStarNode* node, const SPathFinderUserData& data, const CvAStar* finder);

void UnitPathInitialize(const SPathFinderUserData& data, CvAStar* finder);
void UnitPathUninitialize(const SPathFinderUserData& data, CvAStar* finder);
void TradePathInitialize(const SPathFinderUserData& data, CvAStar* finder);
void TradePathUninitialize(const SPathFinderUserData& data, CvAStar* finder);

//helper functions
bool IsPlotConnectedToPlot(PlayerTypes ePlayer, CvPlot* pFromPlot, CvPlot* pToPlot, RouteTypes eRestrictRoute = ROUTE_ANY, bool bIgnoreHarbors = false, SPath* pPathOut = NULL);

namespace PathHelpers
{
	CvPlot* GetXPlotsFromEnd(const SPath& path, int iPlotsFromEnd, bool bLeaveEnemyTerritory);
	int CountPlotsOwnedByXInPath(const SPath& path, PlayerTypes ePlayer);
	int CountPlotsOwnedAnyoneInPath(const SPath& path, PlayerTypes eExceptPlayer);
	CvPlot* GetPathFirstPlot(const SPath& path);
	CvPlot* GetPathEndFirstTurnPlot(const SPath& path);
}

#endif	//CVASTAR_H
