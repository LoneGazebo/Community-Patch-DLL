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

	BuilderDirective() : m_eDirectiveType(NUM_DIRECTIVES), m_eBuild(NO_BUILD), m_eResource(NO_RESOURCE), m_sX(-1), m_sY(-1), m_iScore(-1) {}
	BuilderDirective(BuilderDirectiveType eDirective, BuildTypes eBuild, ResourceTypes eResource, short sX, short sY, int iScore)
	{
		m_eDirectiveType = eDirective;
		m_eBuild = eBuild;
		m_eResource = eResource;
		m_sX = sX;
		m_sY = sY;
		m_iScore = iScore;
	}

	BuilderDirectiveType m_eDirectiveType;

	BuildTypes m_eBuild;
	ResourceTypes m_eResource;
	short m_sX;
	short m_sY;
	int m_iScore;
	//int m_iGoldCost;
	//short m_sMoveTurnsAway;
	bool operator==(const BuilderDirective& rhs) const
	{
		return m_eDirectiveType == rhs.m_eDirectiveType && m_eBuild == rhs.m_eBuild && m_eResource == rhs.m_eResource && m_sX == rhs.m_sX && m_sY == rhs.m_sY && m_iScore == rhs.m_iScore;
	};
	bool operator<(const BuilderDirective& rhs) const
	{
		return m_eDirectiveType < rhs.m_eDirectiveType || m_eBuild < rhs.m_eBuild || m_eResource < rhs.m_eResource || m_sX < rhs.m_sX || m_sY < rhs.m_sY || m_iScore < rhs.m_iScore;
	};
};
FDataStream& operator<<(FDataStream&, const BuilderDirective&);
FDataStream& operator>>(FDataStream&, BuilderDirective&);

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
	void UpdateImprovementPlots(void);

	bool CanUnitPerformDirective(CvUnit* pUnit, BuilderDirective eDirective);
	int GetBuilderNumTurnsAway(CvUnit* pUnit, BuilderDirective eDirective, int iMaxDistance=INT_MAX);
	int GetTurnsToBuild(const CvUnit* pUnit, BuildTypes eBuild, CvPlot* pPlot) const;
	vector<BuilderDirective> GetDirectives();
	bool ExecuteWorkerMove(CvUnit* pUnit, BuilderDirective aDirective);

	void AddImprovingResourcesDirective(vector<OptionWithScore<BuilderDirective>> &aDirectives, CvPlot* pPlot, CvCity* pWorkingCity, const vector<BuildTypes> aBuildsToConsider, int iMinScore);
	void AddImprovingPlotsDirective(vector<OptionWithScore<BuilderDirective>> &aDirectives, CvPlot* pPlot, CvCity* pWorkingCity, const vector<BuildTypes> aBuildsToConsider, int iMinScore);
	void AddRouteOrRepairDirective(vector<OptionWithScore<BuilderDirective>>& aDirectives, CvPlot* pPlot, RouteTypes eRoute, int iValue, RoutePurpose ePurpose);
	void AddRouteDirective(vector<OptionWithScore<BuilderDirective>>& aDirectives, CvPlot* pPlot, RouteTypes eRoute, int iValue);
	void AddRemoveRouteDirective(vector<OptionWithScore<BuilderDirective>> &aDirectives, CvPlot* pPlot, int iNetGoldTimes100);
	void AddChopDirectives(vector<OptionWithScore<BuilderDirective>> &aDirectives, CvPlot* pPlot, CvCity* pWorkingCity);
	void AddRepairImprovementDirective(vector<OptionWithScore<BuilderDirective>>& aDirectives, CvPlot* pPlot, CvCity* pWorkingCity);
	void AddRepairRouteDirective(vector<OptionWithScore<BuilderDirective>>& aDirectives, CvPlot* pPlot, RouteTypes eRoute, int iValue);
	void AddScrubFalloutDirectives(vector<OptionWithScore<BuilderDirective>> &aDirectives, CvPlot* pPlot, CvCity* pWorkingCity);

	bool ShouldAnyBuilderConsiderPlot(CvPlot* pPlot);  // general checks for whether the plot should be considered
	bool ShouldBuilderConsiderPlot(CvUnit* pUnit, CvPlot* pPlot);  // specific checks for this particular worker

	int GetResourceWeight(ResourceTypes eResource, int iQuantity, int iAdditionalOwned=0);

	bool DoesBuildHelpRush(CvPlot* pPlot, BuildTypes eBuild);
	pair<RouteTypes, int> GetBestRouteTypeAndValue(const CvPlot* pPlot) const;
	bool IsRoutePlanned(CvPlot* pPlot, RouteTypes eRoute, RoutePurpose ePurpose) const;
	bool WantCanalAtPlot(const CvPlot* pPlot) const; //build it and keep it
	bool WillNeverBuildVillageOnPlot(CvPlot* pPlot, RouteTypes eRoute, bool bIgnoreUnowned) const;
	ImprovementTypes SavePlotForUniqueImprovement(CvPlot* pPlot) const;

	int ScorePlotBuild(CvPlot* pPlot, ImprovementTypes eImprovement, BuildTypes eBuild, SBuilderState sState=SBuilderState());
	int GetTotalRouteBuildTime(const CvUnit* pUnit, const CvPlot* pPlot) const;

	BuildTypes GetBuildTypeFromImprovement(ImprovementTypes eImprovement) const;
	BuildTypes GetRepairBuild(void);
	FeatureTypes GetFalloutFeature(void);
	BuildTypes GetFalloutRemove(void);
	BuildTypes GetRemoveRoute(void);
	BuildTypes GetBuildRoute(RouteTypes eRoute) const;

	BuilderDirective GetAssignedDirective(CvUnit* pUnit);
	void SetAssignedDirective(CvUnit* pUnit, BuilderDirective eDirective);

	static void LogInfo(const CvString& str, CvPlayer* pPlayer, bool bWriteToOutput = false);
	static void LogYieldInfo(const CvString& strNewLogStr, CvPlayer* pPlayer); //Log yield related info to BuilderTaskingYieldLog.csv.

	bool IsMainRoutePlot(const CvPlot* pPlot) const;
	bool IsShortcutRoutePlot(const CvPlot* pPlot) const;
	bool IsStrategicRoutePlot(const CvPlot* pPlot) const;

	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:

	typedef pair<int, int> PlotPair;
	typedef pair<PlotPair, RouteTypes> PlannedRoute;

	void LogDirectives(vector<OptionWithScore<BuilderDirective>> directives);
	void LogDirective(BuilderDirective directive, int iWeight, bool bChosen = false);

	void ConnectCitiesToCapital(CvCity* pPlayerCapital, CvCity* pTargetCity, BuildTypes eBuild, RouteTypes eRoute);
	void ConnectCitiesForShortcuts(CvCity* pFirstCity, CvCity* pSecondCity, BuildTypes eBuild, RouteTypes eRoute);
	void ConnectCitiesForScenario(CvCity* pFirstCity, CvCity* pSecondCity, BuildTypes eBuild, RouteTypes eRoute);
	void ConnectPointsForStrategy(CvCity* pOriginCity, CvPlot* pTargetPlot, BuildTypes eBuild, RouteTypes eRoute);

	vector<OptionWithScore<BuilderDirective>> GetRouteDirectives();
	vector<OptionWithScore<BuilderDirective>> GetImprovementDirectives();

	void UpdateCurrentPlotYields(CvPlot* pPlot);
	void UpdateProjectedPlotYields(CvPlot* pPlot, BuildTypes eBuild, bool bIgnoreCityConnection);

	bool IsPlannedRouteForPurpose(const CvPlot* pPlot, RoutePurpose ePurpose) const;
	void AddRoutePlots(CvPlot* pStartPlot, CvPlot* pTargetPlot, RouteTypes eRoute, int iValue, const SPath& path, RoutePurpose ePurpose);
	int GetMoveCostWithRoute(const CvPlot* pFromPlot, const CvPlot* pToPlot, RouteTypes eFromPlotRoute, RouteTypes eToPlotRoute);
	int GetPlotYieldModifierTimes100(CvPlot* pPlot, YieldTypes eYield);
	void GetPathValues(const SPath& path, RouteTypes eRoute, int& iVillageBonusesIfCityConnected, int& iTotalMoveCost, int& iNumRoadsNeededToBuild);

	int GetRouteBuildTime(PlannedRoute plannedRoute, const CvUnit* pUnit = (CvUnit*)NULL) const;
	bool CvBuilderTaskingAI::IsRouteCompleted(PlannedRoute plannedRoute) const;

	void UpdateCanalPlots();

	PlotPair GetPlotPair(int iPlotId1, int iPlotId2);

	CvPlayer* m_pPlayer;
	bool m_bLogging;

	typedef pair<RouteTypes,int> RoutePlot;
	map<PlannedRoute, int> m_plannedRouteAdditiveValues;
	map<PlannedRoute, int> m_plannedRouteNonAdditiveValues;
	map<PlannedRoute, vector<int>> m_plannedRoutePlots;
	map<PlannedRoute, RoutePurpose> m_plannedRoutePurposes;
	set<pair<RoutePlot, RoutePurpose>> m_anyRoutePlanned;
	map<int, RoutePlot> m_bestRouteTypeAndValue; //serialized
	map<int, RoutePurpose> m_plotRoutePurposes; //serialized
	map<int, PlannedRoute> m_bestRouteForPlot;
	set<int> m_canalWantedPlots; //serialized
	vector<BuilderDirective> m_directives;
	map<int, BuilderDirective> m_assignedDirectives;

	int m_aiCurrentPlotYields[NUM_YIELD_TYPES];
	int m_aiProjectedPlotYields[NUM_YIELD_TYPES];

	//caching
	BuildTypes m_eRepairBuild;
	FeatureTypes m_eFalloutFeature;
	BuildTypes m_eFalloutRemove;
	BuildTypes m_eRemoveRouteBuild;

	//some player dependent flags for unique improvements
	pair<ImprovementTypes,TechTypes> m_aeSaveFeatureForImprovementUntilTech[NUM_FEATURE_TYPES]; // serialized, can't be recreated on game load (eID is NO_PLAYER)
	ImprovementTypes m_eSaveCityAdjacentForImprovement; // serialized
	ImprovementTypes m_eSaveCoastalForImprovement; // serialized
	ImprovementTypes m_eSaveHillsForImprovement; // serialized
};

FDataStream& operator>>(FDataStream&, CvBuilderTaskingAI&);
FDataStream& operator<<(FDataStream&, const CvBuilderTaskingAI&);

#endif //CIV5_BUILDER_TASKING_AI_H
