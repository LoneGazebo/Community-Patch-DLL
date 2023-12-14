/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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

	CvUnit* FindBestWorker(const map<int, ReachablePlots>& allWorkersReachablePlots, const CvPlot* pTarget) const;
	BuilderDirective EvaluateBuilder(CvUnit* pUnit, const map<int,ReachablePlots>& allWorkersReachablePlots);

	void AddImprovingResourcesDirectives(vector<OptionWithScore<BuilderDirective>>& directives, CvUnit* pUnit, CvPlot* pPlot, CvCity* pWorkingCity, int iMoveTurnsAway);
	void AddImprovingPlotsDirectives(vector<OptionWithScore<BuilderDirective>>& directives, CvUnit* pUnit, CvPlot* pPlot, CvCity* pWorkingCity, int iMoveTurnsAway);
	void AddRouteDirectives(vector<OptionWithScore<BuilderDirective>>& directives, CvUnit* pUnit, CvPlot* pPlot, CvCity* pWorkingCity, int iMoveTurnsAway);
	void AddRemoveRouteDirectives(vector<OptionWithScore<BuilderDirective>>& directives, CvUnit* pUnit, CvPlot* pPlot, CvCity* pWorkingCity, int iMoveTurnsAway);
	void AddChopDirectives(vector<OptionWithScore<BuilderDirective>>& directives, CvUnit* pUnit, CvPlot* pPlot, CvCity* pWorkingCity, int iMoveTurnsAway);
	void AddRepairTilesDirectives(vector<OptionWithScore<BuilderDirective>>& directives, CvUnit* pUnit, CvPlot* pPlot, CvCity* pWorkingCity, int iMoveTurnsAway);
	void AddScrubFalloutDirectives(vector<OptionWithScore<BuilderDirective>>& directives, CvUnit* pUnit, CvPlot* pPlot, CvCity* pWorkingCity, int iMoveTurnsAway);

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
	bool GetSameRouteBenefitFromTrait(CvPlot* pPlot, RouteTypes eRoute) const;

	int ScorePlotBuild(CvUnit* pUnit, CvPlot* pPlot, ImprovementTypes eImprovement, BuildTypes eBuild);

	BuildTypes GetBuildTypeFromImprovement(ImprovementTypes eImprovement);
	BuildTypes GetRepairBuild(void);
	FeatureTypes GetFalloutFeature(void);
	BuildTypes GetFalloutRemove(void);
	BuildTypes GetRemoveRoute(void);
	BuildTypes GetBuildRoute(void);

	static void LogInfo(const CvString& str, CvPlayer* pPlayer, bool bWriteToOutput = false);
	static void LogYieldInfo(const CvString& strNewLogStr, CvPlayer* pPlayer); //Log yield related info to BuilderTaskingYieldLog.csv.

	set<int> GetMainRoutePlots() const;
	set<int> GetShortcutRoutePlots() const;
	set<int> GetStrategicRoutePlots() const;

	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:

	void LogDirectives(vector<OptionWithScore<BuilderDirective>> directives, CvUnit* pUnit);
	void LogDirective(BuilderDirective directive, CvUnit* pUnit, int iWeight, bool bChosen = false);

	void ConnectCitiesToCapital(CvCity* pPlayerCapital, CvCity* pTargetCity, BuildTypes eBuild, RouteTypes eRoute, int iNetGoldTimes100);
	void ConnectCitiesForShortcuts(CvCity* pFirstCity, CvCity* pSecondCity, BuildTypes eBuild, RouteTypes eRoute, int iNetGoldTimes100);
	void ConnectCitiesForScenario(CvCity* pFirstCity, CvCity* pSecondCity, BuildTypes eBuild, RouteTypes eRoute);
	void ConnectPointsForStrategy(CvCity* pOriginCity, CvPlot* pTargetPlot, BuildTypes eBuild, RouteTypes eRoute, int iNetGoldTimes100);

	void UpdateCurrentPlotYields(CvPlot* pPlot);
	void UpdateProjectedPlotYields(CvPlot* pPlot, BuildTypes eBuild);
	bool AddRoutePlot(CvPlot* pPlot, RouteTypes eRoute, int iValue);
	int GetRouteValue(CvPlot* pPlot);

	void UpdateCanalPlots();

	CvPlayer* m_pPlayer;
	bool m_bLogging;

	//plotindex,type,value
	typedef std::tr1::unordered_map<int, pair<RouteTypes, int>> RoutePlotContainer; 
	RoutePlotContainer m_routeWantedPlots; //create route here. serialized
	RoutePlotContainer m_routeNeededPlots; //do not remove route here. serialized
	set<int> m_canalWantedPlots; //serialized

	set<int> m_mainRoutePlots; //route here between city and capital. serialized
	set<int> m_shortcutRoutePlots; //route here between two non-capital cities. serialized
	set<int> m_strategicRoutePlots; //route here for strategic reasons. serialized

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
