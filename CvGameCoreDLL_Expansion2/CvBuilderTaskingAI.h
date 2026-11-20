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
class ReachablePlots;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvBuilderTaskingAI
//!  \brief		Deals with what builders need to deal with
//
//!  Key Attributes:
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
struct PlotBuildScore
{
	PlotBuildScore() : m_iScore(0), m_iPotentialScore(0), m_iUnusedYieldScore(0), m_iScorePenalty(0), m_iAdjacencyPotentialScore(0), m_iTileYieldValue(0), m_iPlotLocalScore(0), m_iYieldGainBonus(0) {}
	PlotBuildScore(int iScore, int iPotentialScore = 0, int iUnusedYieldScore = 0, int iScorePenalty = 0)
	{
		m_iScore = iScore;
		m_iPotentialScore = iPotentialScore;
		m_iUnusedYieldScore = iUnusedYieldScore;
		m_iScorePenalty = iScorePenalty;
		m_iAdjacencyPotentialScore = 0;
		m_iTileYieldValue = 0;
		m_iPlotLocalScore = 0;
		m_iYieldGainBonus = 0;
	}

	PlotBuildScore operator+(const PlotBuildScore& rhs) const
	{
		return PlotBuildScore(m_iScore + rhs.m_iScore,
			m_iPotentialScore + rhs.m_iPotentialScore,
			m_iUnusedYieldScore + rhs.m_iUnusedYieldScore,
			m_iScorePenalty + rhs.m_iScorePenalty);
	};

	PlotBuildScore operator-(const PlotBuildScore& rhs) const
	{
		return PlotBuildScore(m_iScore - rhs.m_iScore,
			m_iPotentialScore - rhs.m_iPotentialScore,
			m_iUnusedYieldScore - rhs.m_iUnusedYieldScore,
			m_iScorePenalty - rhs.m_iScorePenalty);
	};

	bool operator==(const PlotBuildScore& rhs) const
	{
		return m_iScore == rhs.m_iScore
			&& m_iPotentialScore == rhs.m_iPotentialScore
			&& m_iUnusedYieldScore == rhs.m_iUnusedYieldScore
			&& m_iScorePenalty == rhs.m_iScorePenalty;
	}

	bool operator!=(const PlotBuildScore& rhs) const
	{
		return m_iScore != rhs.m_iScore
			|| m_iPotentialScore != rhs.m_iPotentialScore
			|| m_iUnusedYieldScore != rhs.m_iUnusedYieldScore
			|| m_iScorePenalty != rhs.m_iScorePenalty;
	}

	int GetPriorityScore() const
	{
		return m_iScore + (m_iPotentialScore / 3) - m_iScorePenalty - m_iUnusedYieldScore;
	}

	int GetMaxScore() const
	{
		return m_iScore + m_iPotentialScore;
	}

	// For ranking against builds in other plots
	int GetRankScore() const
	{
		return GetPriorityScore() - m_iPlotLocalScore + m_iYieldGainBonus;
	}

	int m_iScore; // base score
	int m_iPotentialScore; // potential bonuses this can give us in the future
	int m_iUnusedYieldScore; // yield score that probably will not be used by any city
	int m_iScorePenalty; // used to downprioritize builds
	int m_iAdjacencyPotentialScore; // part of m_iPotentialScore from improvements assumed in unimproved adjacent plots, only set by ScorePlotBuild
	int m_iTileYieldValue; // value of the plot's base yields after a great person build (without the improvement), only set by ScorePlotBuild
	int m_iPlotLocalScore; // part of m_iScore that only matters against other builds in the same plot, only set by ScorePlotBuild
	int m_iYieldGainBonus; // bonus for a big increase in the plot's own yields, only for ranking against other plots, only set by ScorePlotBuild
};
FDataStream& operator<<(FDataStream&, const PlotBuildScore&);
FDataStream& operator>>(FDataStream&, PlotBuildScore&);

struct BuilderDirective
{
	enum CLOSED_ENUM BuilderDirectiveType
	{
		BUILD_IMPROVEMENT_ON_RESOURCE, // enabling a special resource
		BUILD_IMPROVEMENT,			   // improving a tile
		BUILD_ROUTE,				   // build a route on a tile
		REPAIR_IMPROVEMENT,			   // repairing a pillaged improvement
		REPAIR_ROUTE,				   // repairing a pillaged route
		REMOVE_FEATURE,				   // remove a feature to improve production
		REMOVE_ROAD,				   // remove a road from a plot
		KEEP_IMPROVEMENT,			   // will not actually be executed but needed for planning
		NUM_DIRECTIVES ENUM_META_VALUE
	};

	BuilderDirective() : m_eDirectiveType(NUM_DIRECTIVES), m_eBuild(NO_BUILD), m_eResource(NO_RESOURCE), m_bIsGreatPerson(false), m_bCanBuild(false), m_sX(-1), m_sY(-1), m_plotBuildScore(0), m_iPlanPriority(0) {}
	BuilderDirective(BuilderDirectiveType eDirective, BuildTypes eBuild, ResourceTypes eResource, bool bIsGreatPerson, bool bCanBuild, short sX, short sY, PlotBuildScore plotBuildScore, int iPlanPriority = 0)
	{
		m_eDirectiveType = eDirective;
		m_eBuild = eBuild;
		m_eResource = eResource;
		m_bIsGreatPerson = bIsGreatPerson;
		m_bCanBuild = bCanBuild;
		m_sX = sX;
		m_sY = sY;
		m_plotBuildScore = plotBuildScore;
		m_iPlanPriority = iPlanPriority;
	}

	BuilderDirectiveType m_eDirectiveType;

	BuildTypes m_eBuild;
	ResourceTypes m_eResource;
	bool m_bIsGreatPerson;
	bool m_bCanBuild;
	short m_sX;
	short m_sY;
	PlotBuildScore m_plotBuildScore;
	int m_iPlanPriority; // player set priority, directives with higher priority are always assigned first

	bool operator==(const BuilderDirective& rhs) const
	{
		return m_eBuild == rhs.m_eBuild && m_sX == rhs.m_sX && m_sY == rhs.m_sY;
	};
	bool operator<(const BuilderDirective& rhs) const
	{
		if (m_eDirectiveType != rhs.m_eDirectiveType)
			return m_eDirectiveType < rhs.m_eDirectiveType;
		if (m_eBuild != rhs.m_eBuild)
			return m_eBuild < rhs.m_eBuild;
		if (m_eResource != rhs.m_eResource)
			return m_eResource < rhs.m_eResource;
		if (m_sX != rhs.m_sX)
			return m_sX < rhs.m_sX;
		if (m_sY != rhs.m_sY)
			return m_sY < rhs.m_sY;
		return m_bIsGreatPerson < rhs.m_bIsGreatPerson;
	}
};
FDataStream& operator<<(FDataStream&, const BuilderDirective&);
FDataStream& operator>>(FDataStream&, BuilderDirective&);

const int MAX_WORKER_PLAN_PRIORITY = 3;
const int MANUAL_ROUTE_PLAN_PRIORITY = 1; // priority construction routes from the route planner

// Player made plan for a plot
struct SWorkerPlan
{
	SWorkerPlan() : m_eImprovement(NO_IMPROVEMENT), m_iPriority(0), m_bLock(false) {}
	SWorkerPlan(ImprovementTypes eImprovement, int iPriority, bool bLock) : m_eImprovement(eImprovement), m_iPriority(iPriority), m_bLock(bLock) {}

	ImprovementTypes m_eImprovement; // NO_IMPROVEMENT with lock means "leave this plot as it is"
	int m_iPriority; // 0 = built side by side with AI choices, higher = built before any AI choices
	bool m_bLock; // don't build anything else here while waiting for the planned improvement
};
FDataStream& operator<<(FDataStream&, const SWorkerPlan&);
FDataStream& operator>>(FDataStream&, SWorkerPlan&);

// Summary of what will happen in a plot, for the UI
struct SWorkerPlanOverview
{
	SWorkerPlanOverview() : m_iPlotIndex(-1), m_eAIImprovement(NO_IMPROVEMENT), m_eAIInterimImprovement(NO_IMPROVEMENT), m_iAIScore(0), m_iRank(-1), m_bHasPlan(false), m_eStatus(WORKER_PLAN_STATUS_NONE) {}

	int m_iPlotIndex;
	ImprovementTypes m_eAIImprovement; // best improvement according to the AI, may not be buildable yet
	ImprovementTypes m_eAIInterimImprovement; // what the AI builds meanwhile, if anything
	int m_iAIScore;
	int m_iRank; // position in the build queue, -1 if not queued
	bool m_bHasPlan;
	SWorkerPlan m_plan;
	WorkerPlanStatusTypes m_eStatus; // status of the player plan if any, else of the AI choice
};

// Parts of an improvement's score, for the UI. Keep in sync with GetScorePartKey.
enum ImprovementScorePart
{
	SCORE_PART_YIELDS, // the plot's own yields
	SCORE_PART_FORT, // forts away from the border count half their yields
	SCORE_PART_WOODLAND, // forests for woodland movement
	SCORE_PART_SPAWN_YIELDS, // yields of a resource it may spawn nearby
	SCORE_PART_NEIGHBORS, // yields for worked neighbors
	SCORE_PART_NEIGHBORS_UNWORKED, // yields for unworked neighbors
	SCORE_PART_ADJACENCY, // adjacency with the improvements assumed nearby
	SCORE_PART_FUTURE, // future techs and policies
	SCORE_PART_KEEP, // keeping what's built
	SCORE_PART_CLAIM, // claims the tile
	SCORE_PART_THEFT_BLOCK, // protects the tile from theft
	SCORE_PART_THEFT_RISK, // great person improvements may be lost to theft
	SCORE_PART_GREAT_PERSON_RATE,
	SCORE_PART_CHOP,
	SCORE_PART_DOMAIN, // unit production and experience
	SCORE_PART_RESOURCE,
	SCORE_PART_MONOPOLY,
	SCORE_PART_ROUTE, // doesn't fit the planned roads
	SCORE_PART_RANDOM_RESOURCE,
	SCORE_PART_SPAWN_CLAIM, // a spawned resource may claim a tile
	SCORE_PART_DEFENSE,
	SCORE_PART_COMBAT_BONUS,
	SCORE_PART_CANAL,
	SCORE_PART_CULTURE_BOMB,
	SCORE_PART_MAINTENANCE,
	SCORE_PART_OTHER, // anything not broken down, and rounding
	NUM_SCORE_PARTS
};

// Score parts as computed by ScorePlotBuildBase
struct SScoreBreakdown
{
	SScoreBreakdown();
	int m_aiScore[NUM_SCORE_PARTS]; // parts of PlotBuildScore::m_iScore
	int m_aiPotential[NUM_SCORE_PARTS]; // parts of PlotBuildScore::m_iPotentialScore
	int m_aiYieldTimes100[NUM_YIELD_TYPES]; // the plot's own yields
};

// An improvement that could be planned in a plot, for the UI
struct SImprovementOption
{
	SImprovementOption();

	ImprovementTypes m_eImprovement;
	BuildTypes m_eBuild;
	bool m_bCanPlan;
	bool m_bCanBuildNow;
	TechTypes m_eTechPrereq;
	int m_iScore;
	int m_iRankScore; // what it's ranked by against other plots
	int m_iPlotLocalScore; // in m_iScore, but not in m_iRankScore
	int m_iYieldGainBonus; // in m_iRankScore, but not in m_iScore
	int m_aiScoreParts[NUM_SCORE_PARTS]; // add up to m_iScore
	int m_aiYieldTimes100[NUM_YIELD_TYPES]; // the plot's own yields
	bool m_bYieldsUnused; // no city will likely work the plot, so its yields don't count
	int m_iGreatPersonPenalty; // great person improvements are ranked by m_iScore minus this, what the area loses (or the best other option here)
	CvString m_strReason; // why it can't be planned or built now, localized
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
	void UpdateImprovementPlots(void);

	bool CanUnitPerformDirective(CvUnit* pUnit, BuilderDirective eDirective, bool bTestEra = false);
	int GetTurnsToBuild(const CvUnit* pUnit, BuildTypes eBuild, const CvPlot* pPlot) const;
	vector<BuilderDirective> GetDirectives();
	bool ExecuteWorkerMove(CvUnit* pUnit, BuilderDirective aDirective);

	void AddImprovingPlotsDirective(vector<OptionWithScore<BuilderDirective>> &aDirectives, CvPlot* pPlot, CvCity* pWorkingCity, const vector<BuildTypes> aBuildsToConsider);
	void AddRouteOrRepairDirective(vector<OptionWithScore<BuilderDirective>>& aDirectives, CvPlot* pPlot, RouteTypes eRoute, PlotBuildScore plotBuildScore, RoutePurpose ePurpose);
	void AddRouteDirective(vector<OptionWithScore<BuilderDirective>>& aDirectives, CvPlot* pPlot, RouteTypes eRoute, PlotBuildScore plotBuildScore);
	void AddRemoveRouteDirective(vector<OptionWithScore<BuilderDirective>> &aDirectives, CvPlot* pPlot, int iNetGoldTimes100);
	void AddChopDirectives(vector<OptionWithScore<BuilderDirective>> &aDirectives, CvPlot* pPlot, CvCity* pWorkingCity);
	void AddRepairImprovementDirective(vector<OptionWithScore<BuilderDirective>>& aDirectives, CvPlot* pPlot, CvCity* pWorkingCity);
	void AddRepairRouteDirective(vector<OptionWithScore<BuilderDirective>>& aDirectives, CvPlot* pPlot, RouteTypes eRoute, PlotBuildScore plotBuildScore);
	void AddScrubFalloutDirectives(vector<OptionWithScore<BuilderDirective>> &aDirectives, CvPlot* pPlot, CvCity* pWorkingCity);

	bool ShouldAnyBuilderConsiderPlot(const CvPlot* pPlot) const;  // general checks for whether the plot should be considered
	bool ShouldBuilderConsiderPlot(CvUnit* pUnit, CvPlot* pPlot);  // specific checks for this particular worker

	int GetResourceWeight(ResourceTypes eResource, int iQuantity);

	bool DoesBuildHelpRush(CvPlot* pPlot, BuildTypes eBuild);
	pair<RouteTypes, int> GetBestRouteTypeAndValue(const CvPlot* pPlot) const;
	bool IsRoutePlanned(CvPlot* pPlot, RouteTypes eRoute, RoutePurpose ePurpose) const;
	bool WantCanalAtPlot(const CvPlot* pPlot) const; //build it and keep it
	int GetRouteImprovementGainPercent(const CvPlot* pPlot) const;
	ImprovementTypes SavePlotForUniqueImprovement(const CvPlot* pPlot) const;

	PlotBuildScore ScorePlotBuild(CvPlot* pPlot, ImprovementTypes eImprovement, BuildTypes eBuild, const SBuilderState& sState = SBuilderState::DefaultInstance(), RouteTypes eAssumedRoute = NUM_ROUTE_TYPES);
	int GetTotalRouteBuildTime(const CvUnit* pUnit, const CvPlot* pPlot) const;

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

	int GetResourceSpawnWorkableChance(CvPlot* pPlot, int& iTileClaimChance);

	void UpdateCityWorstPlots(const CvCity* pCity, const SBuilderState& sState);

	void UpdateGreatPersonDirectives(vector<OptionWithScore<BuilderDirective>>& aDirectives) const;

	// Player made plans
	const SWorkerPlan* GetWorkerPlan(const CvPlot* pPlot) const;
	void SetWorkerPlan(const CvPlot* pPlot, ImprovementTypes eImprovement, int iPriority, bool bLock);
	void ClearWorkerPlans();
	void RefreshPlans();
	int GetWorkerPlanRevision() const;
	void InvalidateWorkerPlanUI();
	vector<SWorkerPlanOverview> GetWorkerPlanOverview() const;
	vector<SImprovementOption> GetImprovementOptions(CvPlot* pPlot);
	static const char* GetScorePartKey(ImprovementScorePart ePart);
	bool CanPlanImprovement(const CvPlot* pPlot, ImprovementTypes eImprovement, CvString* pReason = NULL) const;
	ImprovementTypes GetAgreedImprovement(const CvPlot* pPlot) const;
	static int GetGreatPersonWeight(int iPriorityScore, int iTileYieldValue, int iMoveTurns);
	bool IsWorkerPlanSetting(WorkerPlanSettingTypes eSetting) const;
	static bool IsValidWorkerPlanSetting(int iSetting);
	bool IsDirectiveAllowedBySettings(const BuilderDirective& directive) const;
	static void SortDirectivesByPlanPriority(vector<OptionWithScore<BuilderDirective>>& aDirectives);

	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:

	typedef pair<int, int> PlotPair;
	typedef pair<PlotPair, pair<RouteTypes, bool>> PlannedRoute;

	void LogDirectives(vector<OptionWithScore<BuilderDirective>> directives);
	void LogDirective(BuilderDirective directive, int iWeight, bool bChosen = false);

	void ConnectCitiesToCapital(CvCity* pPlayerCapital, CvCity* pTargetCity, BuildTypes eBuild, RouteTypes eRoute);
	void ConnectCitiesForShortcuts(CvCity* pCity1, CvCity* pCity2, BuildTypes eBuild, RouteTypes eRoute);
	void ConnectCitiesForScenario(CvCity* pCity1, CvCity* pCity2, BuildTypes eBuild, RouteTypes eRoute);
	void ConnectPointsForStrategy(CvCity* pOriginCity, CvPlot* pTargetPlot, BuildTypes eBuild, RouteTypes eRoute);
	void ConnectForeignCitiesForYields(CvCity* pForeignCity, BuildTypes eBuild, RouteTypes eRoute);

	void ShortcutConnectionHelper(CvCity* pCity1, CvCity* pCity2, BuildTypes eBuild, RouteTypes eRoute, int iPlotDistance, bool bUseRivers);

	vector<OptionWithScore<BuilderDirective>> GetRouteDirectives();
	vector<OptionWithScore<BuilderDirective>> GetImprovementDirectives();
	void UpdateFutureYields(const vector<BuildTypes>& aPossibleBuilds);
	int GetFutureYields(ImprovementTypes eImprovement, YieldTypes eYield);

	int GetBestNonGPDirectiveScoreInPlot(const CvPlot* pPlot, const vector<OptionWithScore<BuilderDirective>>& aDirectives) const;

	void UpdateCurrentPlotYields(const CvPlot* pPlot);
	void UpdateProjectedPlotYields(const CvPlot* pPlot, BuildTypes eBuild, RouteTypes eForceCityConnection);

	void UpdateAllCityWorstPlots();
	int GetWorstWorkedPlotValue(const CvCity* pCity);
	int GetPlotYieldValueSimplified(const CvPlot* pPlot, BuildTypes eBuild);

	bool IsPlannedRouteForPurpose(const CvPlot* pPlot, RoutePurpose ePurpose) const;
	void AddRoutePlots(CvPlot* pStartPlot, CvPlot* pTargetPlot, RouteTypes eRoute, int iValue, const SPath& path, RoutePurpose ePurpose, bool bUseRivers);
	int GetMoveCostWithRoute(const CvPlot* pFromPlot, const CvPlot* pToPlot, RouteTypes eFromPlotRoute, RouteTypes eToPlotRoute);
	void GetPathValues(const SPath& path, RouteTypes eRoute, int& iVillageBonusesIfCityConnected, int& iPlotBonusesIfCityConnected, int& iMovementBonus, int& iNumRoadsNeededToBuild);

	int GetRouteBuildTime(PlannedRoute plannedRoute, const CvUnit* pUnit = (CvUnit*)NULL) const;
	int GetRouteMissingTiles(PlannedRoute plannedRoute) const;
	int GetRouteCompletionBonus(PlannedRoute plannedRoute) const;

	void SetupExtraXAdjacentPlots();

	void AgreeOnAdjacentImprovements(vector<OptionWithScore<BuilderDirective>>& aDirectives, const map<int, ImprovementTypes>& previousImprovements);
	int GetAssumedAdjacencyScore(const CvPlot* pPlot, ImprovementTypes eImprovement, const map<int, ImprovementTypes>& assumedImprovements, const SBuilderState& sState = SBuilderState::DefaultInstance()) const;
	void ApplyAssumedAdjacency(PlotBuildScore& score, const CvPlot* pPlot, ImprovementTypes eImprovement, const SBuilderState& sState) const;
	void ApplyPlanMinScore(PlotBuildScore& score, const CvPlot* pPlot, ImprovementTypes eImprovement) const;
	int TryAgreementChange(const map<int, vector<size_t>>& plotOptions, const vector<OptionWithScore<BuilderDirective>>& aDirectives,
		const map<int, ImprovementTypes>& assumedImprovements, const set<int>& fixedPlots, int iPlotIndex, ImprovementTypes eImprovement,
		map<int, ImprovementTypes>& trialImprovements, set<int>& trialChangedPlots) const;
	void ValueGreatPersonImprovements(const map<int, vector<size_t>>& plotOptions, const vector<OptionWithScore<BuilderDirective>>& aDirectives,
		const map<int, ImprovementTypes>& assumedImprovements, const set<int>& fixedPlots, const map<int, ImprovementTypes>& greatPersonTargets,
		const set<pair<int, ImprovementTypes>>& alwaysTry, map<pair<int, ImprovementTypes>, map<int, ImprovementTypes>>* pTrialLayouts);
	void ClaimGreatPersons(const vector<OptionWithScore<BuilderDirective>>& aDirectives, const map<int, vector<size_t>>& plotOptions, const map<int, ImprovementTypes>& previousImprovements,
		map<int, ReachablePlots>& reachablePlots, vector<int>& freeUnits, map<int, pair<int, ImprovementTypes>>& previousTargets);
	void ChooseGreatPersonPlots(const map<int, vector<size_t>>& plotOptions, const vector<OptionWithScore<BuilderDirective>>& aDirectives,
		map<int, ImprovementTypes>& assumedImprovements, set<int>& fixedPlots,
		const map<int, ReachablePlots>& reachablePlots, vector<int>& freeUnits, map<int, pair<int, ImprovementTypes>>& previousTargets);
	int GetGreatPersonPenalty(const BuilderDirective& directive, const vector<OptionWithScore<BuilderDirective>>& aDirectives) const;
	bool IsGreatPersonPlotChosen(int iPlotIndex) const;
	void SettleAdjacencyAgreement(const map<int, vector<size_t>>& plotOptions, const vector<OptionWithScore<BuilderDirective>>& aDirectives,
		map<int, ImprovementTypes>& assumedImprovements, const set<int>& fixedPlots, set<int>& worklist, set<int>& changedPlots, bool bLog) const;
	void SetupExtraXAdjacentPlotsForBuild(BuildTypes eBuild, ImprovementTypes eImprovement, int iAdjacencyRequirement);

	void UpdateCanalPlots();

	bool PlotHasSpecialImprovement(const CvPlot* pPlot) const;

	PlotBuildScore ScorePlotBuildBase(CvPlot* pPlot, ImprovementTypes eImprovement, BuildTypes eBuild, const SBuilderState& sState, SScoreBreakdown* pBreakdown = NULL, RouteTypes eAssumedRoute = NUM_ROUTE_TYPES);
	void UpdateRouteImprovementGain(CvPlot* pPlot, const vector<OptionWithScore<BuilderDirective>>& aDirectives, size_t iFirstDirective, RouteTypes eRoute);
	BuildTypes GetBuildForImprovement(const CvPlot* pPlot, ImprovementTypes eImprovement) const;
	bool IsWorkerPlanEnforced(const CvPlot* pPlot, const SWorkerPlan& plan) const;
	bool IsImprovementAllowedByPlan(const CvPlot* pPlot, ImprovementTypes eImprovement) const;
	bool RemovesFeatureNeededByPlan(const CvPlot* pPlot, BuildTypes eBuild) const;
	bool HasUnitForBuild(BuildTypes eBuild) const;
	bool IsBuildPlannable(const CvPlot* pPlot, BuildTypes eBuild) const;
	ImprovementTypes GetPlannedImprovement(const CvPlot* pPlot) const;
	int GetPlanBlocker(const CvPlot* pPlot, ImprovementTypes eImprovement) const;

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
	map<const CvCity*, int> m_worstCityPlotValues;
	std::tr1::unordered_map<ImprovementTypes, std::tr1::unordered_map<YieldTypes, int>> m_futureYieldBonuses;

	int m_aiCurrentPlotYields[NUM_YIELD_TYPES];
	int m_aiProjectedPlotYields[NUM_YIELD_TYPES];

	//caching
	BuildTypes m_eRepairBuild;
	FeatureTypes m_eFalloutFeature;
	BuildTypes m_eFalloutRemove;
	BuildTypes m_eRemoveRouteBuild;

	//some player dependent flags for unique improvements
	vector<ImprovementTypes> m_uniqueImprovements; // serialized

	std::tr1::unordered_map<ImprovementTypes, std::tr1::unordered_set<const CvPlot*>> m_extraPlotsForXAdjacentImprovements;

	map<pair<int, ImprovementTypes>, int> m_greatPersonLosses; // what the area loses if a great person improvement is built in a plot, set in Update
	set<pair<int, ImprovementTypes>> m_greatPersonChoices; // where great persons go (or the player planned), other great person directives are dropped, set in Update
	bool m_bGreatPersonChoicesMade;
	map<int, ImprovementTypes> m_assumedImprovements; // serialized, what unimproved plots are expected to get, agreed between neighbors
	map<int, pair<ImprovementTypes, int>> m_routeImprovementGains; // serialized, plot index -> best route improvement (e.g. village) and how much (percent) it beats the alternatives there
	map<int, SWorkerPlan> m_workerPlans; // serialized, plot index -> plan
	int m_iWorkerPlanRevision; // bumped when plans or directives change, lets the UI know when to refresh
};

FDataStream& operator>>(FDataStream&, CvBuilderTaskingAI&);
FDataStream& operator<<(FDataStream&, const CvBuilderTaskingAI&);

#endif //CIV5_BUILDER_TASKING_AI_H
