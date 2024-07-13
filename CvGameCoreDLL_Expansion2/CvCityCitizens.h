/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_CITY_CITIZENS_H
#define CIV5_CITY_CITIZENS_H

#include "CvEnumMap.h"

#include <bitset>

struct SPrecomputedExpensiveNumbers
{
	bool bNeedUpdate;
	int iFoodRateTimes100;
	int iFoodConsumptionTimes100;
	int iFoodCorpMod;
	int iFamine;
	int iDistress;
	int iPoverty;
	int iIlliteracy;
	int iBoredom;
	int iUrbanization;
	int iGrowthMod;
	int iLocalHappiness;
	int iLocalUnhappiness;
	int iHappinessOtherCities;
	int iUnhappinessOtherCities;
	int iBasicNeedsRateChangeForReducedDistress;
	int iGoldRateChangeForReducedPoverty;
	int iScienceRateChangeForReducedIlliteracy;
	int iCultureRateChangeForReducedBoredom;
	int iBasicNeedsRateChangeForIncreasedDistress;
	int iGoldRateChangeForIncreasedPoverty;
	int iScienceRateChangeForIncreasedIlliteracy;
	int iCultureRateChangeForIncreasedBoredom;
	int iOtherUnhappiness;
	bool bWantArt;
	bool bWantScience;
	bool bWantDiplo;
	int iNetGold;

	vector<int> iSpecialistGPPRates;
	vector<bool> bAnySpecialistInOtherCity;
	vector<int> iYieldChangeAnySpecialist;

	vector<vector<int>> bonusForXTerrain; //updated on demand only
	vector<vector<int>> bonusForXFeature; //updated on demand only

	SPrecomputedExpensiveNumbers();
	void update(CvCity* pCity, bool bInsideLoop=false);
};

// used for the yield changes corresponding to a change in tile/specialist assignment
// consists of: vector of yields, vector of specialist GPP points, number of specialists added/removed (for unhappiness calculation)
struct YieldAndGPPList
{
	vector<int>yield;
	vector<int>iNumSpecialists;

	YieldAndGPPList();
	YieldAndGPPList negative();
	bool isNoPositivePlotYield();

	bool operator==(const YieldAndGPPList& other) const
	{
		return (other.yield == yield && other.iNumSpecialists == iNumSpecialists);
	}
};

YieldAndGPPList operator+(const YieldAndGPPList&, const YieldAndGPPList&);

// structure to describe a single tile change. can be a plot or a specialist (including laborer)
struct TileChange
{
	CvPlot* plot;
	SpecialistTypes specialist;

	TileChange();
	TileChange(CvPlot* pPlot);
	TileChange(SpecialistTypes eSpecialist);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvCityCitizens
//!  \brief		Keeps track of Citizens and Specialists in a City
//
//!  Key Attributes:
//!  - One instance for each city
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCityCitizens
{
public:
	CvCityCitizens(void);
	~CvCityCitizens(void);

	enum ePlotSelectionMode
	{
		eBEST_UNWORKED_NO_OVERRIDE,
		eBEST_UNWORKED_ALLOW_OVERRIDE,
		eWORST_WORKED_UNFORCED,
		eWORST_WORKED_FORCED,
		eWORST_WORKED_ANY,
	};

	void Init(CvCity* pCity);
	void Uninit();
	void Reset();
	template<typename CityCitizens, typename Visitor>
	static void Serialize(CityCitizens& cityCitizens, Visitor& visitor);
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	CvCity* GetCity();
	CvPlayer* GetPlayer() const;
	PlayerTypes GetOwner() const;
	TeamTypes GetTeam() const;

	void DoFoundCity();
	void DoTurn();

	int GetBonusPlotValue(CvPlot* pPlot, YieldTypes eYield, SPrecomputedExpensiveNumbers& cache);
	bool CityShouldEmphasizeFood(int iAssumedExcessFood) const;
	bool CityShouldEmphasizeProduction() const;

	// Are this City's Citizens automated? (always true for AI civs)
	bool IsAutomated() const;
	void SetAutomated(bool bValue);

	bool IsNoAutoAssignSpecialists() const;
	void SetNoAutoAssignSpecialists(bool bValue, bool bReset = false);

	int GetExcessFoodThreshold100() const;
	bool IsAvoidGrowth() const;
	bool IsForcedAvoidGrowth() const;
	bool SetForcedAvoidGrowth(bool bAvoidGrowth, bool bReallocate = false);
	CityAIFocusTypes GetFocusType() const;
	bool SetFocusType(CityAIFocusTypes eFocus, bool bReallocate = false);

	BuildingTypes GetAIBestSpecialistBuilding(int& iSpecialistValue, SPrecomputedExpensiveNumbers& cache, bool bLogging = false);
	BuildingTypes GetAIWorstSpecialistCurrentlyInBuilding(int& iSpecialistValue, SPrecomputedExpensiveNumbers& cache);
	int GetSpecialistGPPRate(SpecialistTypes eSpecialist, SPrecomputedExpensiveNumbers& cache);

	YieldAndGPPList GetPlotYields(CvPlot* pPlot, SPrecomputedExpensiveNumbers& cache);
	YieldAndGPPList GetSpecialistYields(SpecialistTypes eSpecialist);

	int GetBaseValuePerYield(YieldTypes eYield, SPrecomputedExpensiveNumbers& cache, bool bAssumeStarving = false, bool bAssumeBelowGrowthThreshold = false, bool bAssumeInDebt = false) const;
	int GetBaseValuePerGPP(SpecialistTypes eSpecialist, SPrecomputedExpensiveNumbers& cache) const;

	int GetPlotValueQuick(CvPlot* pPlot, bool bAdd, SPrecomputedExpensiveNumbers& cache, bool bAssumeStarving = false, bool bAssumeBelowGrowthThreshold = false, bool bAssumeInDebt = false);
	int GetPlotValue(CvPlot* pPlot, bool bAdd, SPrecomputedExpensiveNumbers& cache);
	int GetSpecialistValueQuick(SpecialistTypes eSpecialist, bool bAdd, SPrecomputedExpensiveNumbers& cache, bool bAssumeStarving = false, bool bAssumeBelowGrowthThreshold = false, bool bAssumeInDebt = false);
	int GetSpecialistValue(SpecialistTypes eSpecialist, bool bAdd, SPrecomputedExpensiveNumbers& cache);
	int ScoreYieldChangeQuick(YieldAndGPPList yieldChanges, SPrecomputedExpensiveNumbers& cache, bool bAssumeStarving = false, bool bAssumeBelowGrowthThreshold = false, bool bAssumeInDebt = false);
	int ScoreYieldChange(YieldAndGPPList yieldChanges, SPrecomputedExpensiveNumbers& cache);

	vector<TileChange> GetBestOptionsQuick(int iNumOptions, bool bAdd, SPrecomputedExpensiveNumbers& cache, bool bAssumeStarving, bool bAssumeBelowGrowthThreshold, bool bAssumeInDebt, bool bIncludePlots = true, bool bIncludeSpecialists = true, bool bNoTwoOptionsWithSameYields = false);

	// Citizen Assignment
	int GetNumUnassignedCitizens() const;
	void ChangeNumUnassignedCitizens(int iChange);
	int GetNumCitizensWorkingPlots() const;
	void ChangeNumCitizensWorkingPlots(int iChange);

	void DoInitialAssigment(bool bAssumeStarving, bool bAssumeBelowGrowthThreshold, bool bLogging);
	bool DoAddBestCitizenFromUnassigned(CvCity::eUpdateMode updateMode, bool bLogging = false, bool NoSpecialists = false);
	bool DoRemoveWorstCitizen(CvCity::eUpdateMode updateMode, bool bRemoveForcedStatus = false, SpecialistTypes eDontChangeSpecialist = NO_SPECIALIST);
	void DoApplyTileChange(TileChange tileChange, bool bAdd, bool bLogging = false);

	void SetDirty(bool bValue);
	bool IsDirty() const;
	void DoReallocateCitizens(bool bForce = false, bool bLogging = false);

	void OptimizeWorkedPlots(bool bLogging);
	bool NeedReworkCitizens();
	CvPlot* GetBestCityPlotWithValue(int& iChosenValue, ePlotSelectionMode eMode, SPrecomputedExpensiveNumbers& cache, bool Logging = false);

	// Worked Plots
	bool IsWorkingPlot(int iIndex) const;
	bool IsWorkingPlot(const CvPlot* pPlot) const;
	void SetWorkingPlot(CvPlot* pPlot, bool bNewValue, CvCity::eUpdateMode updateMode);
	void DoAlterWorkingPlot(int iIndex);

	// Forced Working Plots (human override)
	bool IsForcedWorkingPlot(int iRelativeIndex) const;
	bool IsForcedWorkingPlot(const CvPlot* pPlot) const;
	void SetForcedWorkingPlot(CvPlot* pPlot, bool bNewValue);

	void DoValidateForcedWorkingPlots();
	bool DoDemoteWorstForcedWorkingPlot();

	int GetNumForcedWorkingPlots() const;
	void ChangeNumForcedWorkingPlots(int iChange);

	bool IsCanWorkWithOverride(CvPlot* pPlot) const;
	bool IsCanWork(CvPlot* pPlot) const;
	bool IsBlockaded(CvPlot* pPlot) const;
	void SetBlockaded(CvPlot* pPlot);
	void ClearBlockades();
	bool AnyPlotBlockaded() const;

	bool DoVerifyWorkingPlots();

	const std::vector<int>& GetWorkedPlots() const { return m_vWorkedPlots; }

	// Helpful Stuff
	int GetCityIndexFromPlot(const CvPlot* pPlot) const;
	CvPlot* GetCityPlotFromIndex(int iIndex) const;

	// Specialists
	void DoSpecialists();

	int GetSpecialistRate(SpecialistTypes eSpecialist);
	bool IsCanAddSpecialistToBuilding(BuildingTypes eBuilding);
	void DoAddSpecialistToBuilding(BuildingTypes eBuilding, bool bForced, CvCity::eUpdateMode updateMode);
	void DoRemoveSpecialistFromBuilding(BuildingTypes eBuilding, bool bForced, CvCity::eUpdateMode updateMode);
	void DoRemoveAllSpecialistsFromBuilding(BuildingTypes eBuilding, CvCity::eUpdateMode updateMode);
	bool DoRemoveWorstSpecialist(SpecialistTypes eDontChangeSpecialist, bool bForced, const BuildingTypes eDontRemoveFromBuilding, CvCity::eUpdateMode updateMode);

	int GetNumDefaultSpecialists() const;
	void ChangeNumDefaultSpecialists(int iChange, CvCity::eUpdateMode updateMode);
	int GetNumForcedDefaultSpecialists() const;
	void ChangeNumForcedDefaultSpecialists(int iChange);

	int GetSpecialistCount(SpecialistTypes eIndex) const;
	int GetTotalSpecialistCount() const;
	int GetSpecialistSlots(SpecialistTypes eIndex) const;
	void ChangeNumSpecialistSlots(SpecialistTypes eIndex, int iValue);
	int GetSpecialistSlotsTotal() const;

	int GetBuildingGreatPeopleRateChanges(SpecialistTypes eSpecialist) const;
	void ChangeBuildingGreatPeopleRateChanges(SpecialistTypes eSpecialist, int iChange);

	int GetSpecialistGreatPersonProgress(SpecialistTypes eIndex) const;
	int GetSpecialistGreatPersonProgressTimes100(SpecialistTypes eIndex) const;
	void ChangeSpecialistGreatPersonProgressTimes100(SpecialistTypes eIndex, int iChange, bool bCheckForSpawn = false);
	void DoResetSpecialistGreatPersonProgressTimes100(SpecialistTypes eIndex, int iAmountToRemove);

	int GetNumSpecialistsInBuilding(BuildingTypes eBuilding) const;
	int GetNumForcedSpecialistsInBuilding(BuildingTypes eBuilding) const;

	void DoClearForcedSpecialists();

	int GetNumSpecialistsAllowedByBuilding(const CvBuildingEntry& kBuilding);

	int GetSpecialistUpgradeThreshold(UnitClassTypes eUnitClass) const;

	void DoSpawnGreatPerson(UnitTypes eUnit, bool bIncrementCount, bool bCountAsProphet, bool bIsFree);

	YieldTypes GetFocusTypeYield(CityAIFocusTypes eFocus);

private:

	CvCity* m_pCity;

	bool m_bAutomated;
	bool m_bNoAutoAssignSpecialists;
	bool m_bIsDirty;

	int m_iNumUnassignedCitizens;
	int m_iNumCitizensWorkingPlots;
	int m_iNumForcedWorkingPlots;

	CityAIFocusTypes m_eCityAIFocusTypes;
	bool m_bForceAvoidGrowth;

	bool m_pabWorkingPlot[MAX_CITY_PLOTS];
	bool m_pabForcedWorkingPlot[MAX_CITY_PLOTS];
	std::vector<int> m_vWorkedPlots;
	std::vector<int> m_vBlockadedPlots; //for caching, not serialized

	int m_iNumDefaultSpecialists;
	int m_iNumForcedDefaultSpecialists;
	CvEnumMap<SpecialistTypes, int> m_aiSpecialistCounts;
	CvEnumMap<SpecialistTypes, int> m_aiSpecialistSlots;
	CvEnumMap<SpecialistTypes, int> m_aiSpecialistGreatPersonProgressTimes100;
	CvEnumMap<BuildingTypes, int> m_aiNumSpecialistsInBuilding;
	CvEnumMap<BuildingTypes, int> m_aiNumForcedSpecialistsInBuilding;
	CvEnumMap<SpecialistTypes, int> m_piBuildingGreatPeopleRateChanges;
};

FDataStream& operator>>(FDataStream&, CvCityCitizens&);
FDataStream& operator<<(FDataStream&, const CvCityCitizens&);

#endif // CIV5_CITY_CITIZENS_H