/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_BUILDER_TASKING_AI_H
#define CIV5_BUILDER_TASKING_AI_H

class CvPlayer;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvBuilderTaskingAI
//!  \brief		Deals with what builders need to deal with
//
//!  Key Attributes:
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
struct BuilderDirective
{
	enum CLOSED_ENUM BuilderDirectiveType
	{
	    BUILD_IMPROVEMENT_ON_RESOURCE, // enabling a special resource
	    BUILD_IMPROVEMENT,			   // improving a tile
	    BUILD_ROUTE,				   // build a route on a tile
	    REPAIR,						   // repairing a pillaged route or improvement
	    CHOP,						   // remove a feature to improve production
	    REMOVE_ROAD,				   // remove a road from a plot
	    NUM_DIRECTIVES ENUM_META_VALUE
	};

	BuilderDirective() :
		m_eDirective(NUM_DIRECTIVES)
		, m_eBuild(NO_BUILD)
		, m_eResource(NO_RESOURCE)
		, m_sX(-1)
		, m_sY(-1)
		, m_sMoveTurnsAway(-1)
	{
	}

	BuilderDirectiveType m_eDirective;

	BuildTypes m_eBuild;
	ResourceTypes m_eResource;
	short m_sX;
	short m_sY;
	//int m_iGoldCost;
	short m_sMoveTurnsAway;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvBuilderTaskingAI
//!  \brief		Deals with what builders need to deal with
//
//!  Key Attributes:
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBuilderTaskingAI
{
public:
	CvBuilderTaskingAI(void);
	~CvBuilderTaskingAI(void);

	void Init(CvPlayer* pPlayer);
	void Uninit(void);

	//// Serialization routines
	template<typename BuilderTaskingAI, typename Visitor>
	static void Serialize(BuilderTaskingAI& builderTaskingAI, Visitor& visitor);
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	void Update(void);
	void UpdateRoutePlots(void);

	CvUnit* FindBestWorker(const map<CvUnit*, ReachablePlots>& allWorkersReachablePlots, const CvPlot* pTarget) const;
	BuilderDirective EvaluateBuilder(CvUnit* pUnit, const map<CvUnit*,ReachablePlots>& allWorkersReachablePlots);

	void AddImprovingResourcesDirectives(CvUnit* pUnit, CvPlot* pPlot, CvCity* pWorkingCity, int iMoveTurnsAway);
	void AddImprovingPlotsDirectives(CvUnit* pUnit, CvPlot* pPlot, CvCity* pWorkingCity, int iMoveTurnsAway);
	void AddRouteDirectives(CvUnit* pUnit, CvPlot* pPlot, CvCity* pWorkingCity, int iMoveTurnsAway);
	void AddRemoveRouteDirectives(CvUnit* pUnit, CvPlot* pPlot, CvCity* pWorkingCity, int iMoveTurnsAway);
	void AddChopDirectives(CvUnit* pUnit, CvPlot* pPlot, CvCity* pWorkingCity, int iMoveTurnsAway);
	void AddRepairTilesDirectives(CvUnit* pUnit, CvPlot* pPlot, CvCity* pWorkingCity, int iMoveTurnsAway);
	void AddScrubFalloutDirectives(CvUnit* pUnit, CvPlot* pPlot, CvCity* pWorkingCity, int iMoveTurnsAway);

	bool ShouldBuilderConsiderPlot(CvUnit* pUnit, CvPlot* pPlot);  // determines all the logistics if the builder should get to the plot

	int GetBuildCostWeight(int iWeight, CvPlot* pPlot, BuildTypes eBuild);
	int GetBuildTimeWeight(CvUnit* pUnit, CvPlot* pPlot, BuildTypes eBuild, bool bIgnoreFeatureTime = false, int iAdditionalTime = 0);
	int GetResourceWeight(ResourceTypes eResource, ImprovementTypes eImprovement, int iQuantity);

	bool DoesBuildHelpRush(CvUnit* pUnit, CvPlot* pPlot, BuildTypes eBuild);
	bool WantRouteAtPlot(const CvPlot* pPlot) const; //build it
	bool NeedRouteAtPlot(const CvPlot* pPlot) const; //keep it
	RouteTypes GetRouteTypeWantedAtPlot(const CvPlot* pPlot) const;
	RouteTypes GetRouteTypeNeededAtPlot(const CvPlot* pPlot) const;
	bool WantCanalAtPlot(const CvPlot* pPlot) const; //build it and keep it

	int ScorePlotBuild(CvPlot* pPlot, ImprovementTypes eImprovement, BuildTypes eBuild);

	BuildTypes GetBuildTypeFromImprovement(ImprovementTypes eImprovement);
	BuildTypes GetRepairBuild(void);
	FeatureTypes GetFalloutFeature(void);
	BuildTypes GetFalloutRemove(void);
	BuildTypes GetRemoveRoute(void);
	BuildTypes GetBuildRoute(void);

	static void LogInfo(const CvString& str, CvPlayer* pPlayer, bool bWriteToOutput = false);
	static void LogYieldInfo(const CvString& strNewLogStr, CvPlayer* pPlayer); //Log yield related info to BuilderTaskingYieldLog.csv.

	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:

	void LogFlavors(FlavorTypes eFlavor);
	void LogDirectives(CvUnit* pUnit);
	void LogDirective(BuilderDirective directive, CvUnit* pUnit, int iWeight, bool bChosen = false);

	void ConnectCitiesToCapital(CvCity* pPlayerCapital, CvCity* pTargetCity, RouteTypes eRoute, int iNetGoldTimes100);
	void ConnectCitiesForShortcuts(CvCity* pFirstCity, CvCity* pSecondCity, RouteTypes eRoute, int iNetGoldTimes100);
	void ConnectCitiesForScenario(CvCity* pFirstCity, CvCity* pSecondCity, RouteTypes eRoute);
	void ConnectPointsForStrategy(CvCity* pOriginCity, CvPlot* pTargetPlot, RouteTypes eRoute, int iNetGoldTimes100);

	void UpdateCurrentPlotYields(CvPlot* pPlot);
	void UpdateProjectedPlotYields(CvPlot* pPlot, BuildTypes eBuild);
	void AddRoutePlot(CvPlot* pPlot, RouteTypes eRoute, int iValue);
	int GetRouteValue(CvPlot* pPlot);
	bool GetSameRouteBenifitFromTrait(CvPlot* pPlot, RouteTypes eRoute);
	bool IsSafeForRoute(CvPlot* pPlot);

	void UpdateCanalPlots();

	CvPlayer* m_pPlayer;
	bool m_bLogging;
	vector<OptionWithScore<BuilderDirective>> m_aDirectives;

	//plotindex,type,value
	typedef std::tr1::unordered_map<int, pair<RouteTypes, int>> RoutePlotContainer;
	RoutePlotContainer m_routeWantedPlots; //create route here. serialized
	RoutePlotContainer m_routeNeededPlots; //do not remove route here. serialized
	set<int> m_canalWantedPlots; //serialized

	int m_aiCurrentPlotYields[NUM_YIELD_TYPES];
	int m_aiProjectedPlotYields[NUM_YIELD_TYPES];

	//caching
	BuildTypes m_eRepairBuild;
	FeatureTypes m_eFalloutFeature;
	BuildTypes m_eFalloutRemove;
	BuildTypes m_eRemoveRouteBuild;
	BuildTypes m_eRouteBuild;

	//some player dependent flags for unique improvements
	bool m_bKeepMarshes;
	bool m_bKeepJungle;
	bool m_bEvaluateAdjacent;
	bool m_bMayPutGPTINextToCity;
};

FDataStream& operator>>(FDataStream&, CvBuilderTaskingAI&);
FDataStream& operator<<(FDataStream&, const CvBuilderTaskingAI&);

#endif //CIV5_BUILDER_TASKING_AI_H
