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

#define SAFE_ESTIMATE_NUM_EXTRA_PLOTS 64

class CvPlayer;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvBuilderTaskingAI
//!  \brief		Deals with what builders need to deal with
//
//!  Key Attributes:
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
struct BuilderDirective
{
	typedef enum BuilderDirectiveType
	{
	    BUILD_IMPROVEMENT_ON_RESOURCE, // enabling a special resource
	    BUILD_IMPROVEMENT,			   // improving a tile
	    BUILD_ROUTE,				   // build a route on a tile
	    REPAIR,						   // repairing a pillaged route or improvement
	    CHOP,						   // remove a feature to improve production
	    REMOVE_ROAD,				   // remove a road from a plot
	    NUM_DIRECTIVES
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
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

	void Update(void);
	void UpdateRoutePlots(void);

	int CheckAlternativeWorkers(const std::vector<CvUnit*>& otherWorkers, const CvPlot* pTarget) const;

#if defined(MOD_UNITS_LOCAL_WORKERS) || defined(MOD_AI_SECONDARY_WORKERS)
	bool EvaluateBuilder(CvUnit* pUnit, BuilderDirective* paDirectives, UINT uaDirectives, bool bKeepOnlyBest = false, bool bOnlyEvaluateWorkersPlot = false, bool bLimit = false);
	int FindTurnsAway(CvUnit* pUnit, const CvPlot* pPlot, bool bLimit = false) const; // returns -1 if no path can be found, otherwise it returns the # of turns to get there
#else
	bool EvaluateBuilder(CvUnit* pUnit, BuilderDirective* paDirectives, UINT uaDirectives, bool bKeepOnlyBest = false, bool bOnlyEvaluateWorkersPlot = false);
	int FindTurnsAway(CvUnit* pUnit, CvPlot* pPlot);  // returns -1 if no path can be found, otherwise it returns the # of turns to get there
#endif

	void AddImprovingResourcesDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway);
	void AddImprovingPlotsDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway);
	void AddRouteDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway);
	void AddRemoveRouteDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway);
	void AddChopDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway);
	void AddRepairTilesDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway);
	void AddScrubFalloutDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway);

	bool ShouldBuilderConsiderPlot(CvUnit* pUnit, CvPlot* pPlot);  // determines all the logistics if the builder should get to the plot

	int GetBuildCostWeight(int iWeight, CvPlot* pPlot, BuildTypes eBuild);
	int GetBuildTimeWeight(CvUnit* pUnit, CvPlot* pPlot, BuildTypes eBuild, bool bIgnoreFeatureTime = false, int iAdditionalTime = 0);
	int GetResourceWeight(ResourceTypes eResource, ImprovementTypes eImprovement, int iQuantity);
	bool IsImprovementBeneficial(CvPlot* pPlot, const CvBuildInfo& kBuild, YieldTypes eYield, bool bIsBreakEvenOK = false);

	CvCity* getOwningCity(CvPlot* pPlot);
	bool DoesBuildHelpRush(CvUnit* pUnit, CvPlot* pPlot, BuildTypes eBuild);

#if defined(MOD_BALANCE_CORE)
	int ScorePlot(ImprovementTypes eImprovement, BuildTypes eBuild);
#else
	int ScorePlot();
#endif

	BuildTypes GetBuildTypeFromImprovement(ImprovementTypes eImprovement);
	//static YieldTypes GetDeficientYield (CvCity* pCity, bool bIgnoreHappiness = false); // this is different from the CityStrategy one because it checks unhappiness before declaring a food emergency
	BuildTypes GetRepairBuild(void);
	FeatureTypes GetFalloutFeature(void);
	BuildTypes GetFalloutRemove(void);

	static void LogInfo(CvString str, CvPlayer* pPlayer, bool bWriteToOutput = false);
	static void LogYieldInfo(CvString strNewLogStr, CvPlayer* pPlayer); //Log yield related info to BuilderTaskingYieldLog.csv.

	static CvWeightedVector<BuilderDirective, 100, true> m_aDirectives;
	static FStaticVector<int, SAFE_ESTIMATE_NUM_EXTRA_PLOTS, true, c_eCiv5GameplayDLL, 0> m_aiNonTerritoryPlots; // plots that we need to evaluate that are outside of our territory

	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:

	void LogFlavors(FlavorTypes eFlavor);
	void LogDirectives(CvUnit* pUnit);
	void LogDirective(BuilderDirective directive, CvUnit* pUnit, int iWeight, bool bChosen = false);

	void ConnectCitiesToCapital(CvCity* pPlayerCapital, CvCity* pTargetCity, RouteTypes eRoute, int iNetGoldTimes100);
	void ConnectCitiesForShortcuts(CvCity* pFirstCity, CvCity* pSecondCity, RouteTypes eRoute);
	void ConnectCitiesForScenario(CvCity* pFirstCity, CvCity* pSecondCity, RouteTypes eRoute);
	void ConnectPointsForStrategy(CvCity* pOriginCity, CvPlot* pTargetPlot, RouteTypes eRoute, int iNetGoldTimes100);

	void UpdateCurrentPlotYields(CvPlot* pPlot);
	void UpdateProjectedPlotYields(CvPlot* pPlot, BuildTypes eBuild);

	CvPlayer* m_pPlayer;
	BuildTypes m_eRepairBuild;
	set<int> m_aiPlots;
	bool m_bLogging;
	int m_iNumCities;

	CvPlot* m_pTargetPlot;
	int m_aiCurrentPlotYields[NUM_YIELD_TYPES];
	int m_aiProjectedPlotYields[NUM_YIELD_TYPES];

	FeatureTypes m_eFalloutFeature;
	BuildTypes m_eFalloutRemove;

	bool m_bKeepMarshes;
	bool m_bKeepJungle;
#if defined(MOD_BALANCE_CORE)
	bool m_bEvaluateAdjacent;
	bool m_bNoPermanentsAdjacentCity;
#endif
};

#endif //CIV5_BUILDER_TASKING_AI_H
