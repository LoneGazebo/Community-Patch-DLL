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

struct SPrecomputedExpensiveNumbers
{
	int iExcessFoodTimes100;
	int iFoodCorpMod;
	int iUnhappinessFromGold;
	int iUnhappinessFromScience;
	int iUnhappinessFromCulture;
	int iUnhappinessFromReligion;
	int iUnhappinessFromDistress;

	SPrecomputedExpensiveNumbers(CvCity* pCity);
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

	void Init(CvCity* pCity);
	void Uninit();
	void Reset();
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

	CvCity* GetCity();
	CvPlayer* GetPlayer();
	PlayerTypes GetOwner() const;
	TeamTypes GetTeam() const;

	void DoFoundCity();
	void DoTurn();

	int GetBonusPlotValue(CvPlot* pPlot, YieldTypes eYield);
	int GetPlotValue(CvPlot* pPlot, SPrecomputedExpensiveNumbers store);

	// Are this City's Citizens automated? (always true for AI civs)
	bool IsAutomated() const;
	void SetAutomated(bool bValue);

	bool IsNoAutoAssignSpecialists() const;
	void SetNoAutoAssignSpecialists(bool bValue, bool bReallocate = false);

	bool IsAvoidGrowth();
	bool IsForcedAvoidGrowth();
	void SetForcedAvoidGrowth(bool bAvoidGrowth, bool bReallocate = false);
	CityAIFocusTypes GetFocusType() const;
	void SetFocusType(CityAIFocusTypes eFocus, bool bReallocate = false);

	// Specialist AI
	bool IsAIWantSpecialistRightNow();
	BuildingTypes GetAIBestSpecialistBuilding(int& iSpecialistValue, std::map<SpecialistTypes, int>& specialistValueCache, bool bLogging = false);
	BuildingTypes GetAIBestSpecialistCurrentlyInBuilding(int& iSpecialistValue, std::map<SpecialistTypes, int>& specialistValueCache);
	int GetSpecialistValue(SpecialistTypes eSpecialist, int iExcessFoodTimes100); //precompute some expensive values
	bool IsBetterThanDefaultSpecialist(SpecialistTypes eSpecialist);
	bool CanCreateSpecialist();

	// Citizen Assignment
	int GetNumUnassignedCitizens() const;
	void ChangeNumUnassignedCitizens(int iChange);
	int GetNumCitizensWorkingPlots() const;
	void ChangeNumCitizensWorkingPlots(int iChange);

	bool DoAddBestCitizenFromUnassigned();
	bool DoAddBestCitizenFromUnassignedEx(std::map<SpecialistTypes, int>& specialistValueCache, bool bLogging = false, bool bNoSpecialists = false, bool bUpdateNow = true);
	bool DoRemoveWorstCitizen(bool bRemoveForcedStatus = false, SpecialistTypes eDontChangeSpecialist = NO_SPECIALIST, int iCurrentCityPopulation = -1, bool bUpdateNow = true);

	void SetBlockade(bool bValue);
	bool IsBlockade();
	void SetDirty(bool bValue);
	bool IsDirty();
	void DoReallocateCitizens(bool bForce = false, bool bLogging = false);

	bool NeedReworkCitizens();
	CvPlot* GetBestCityPlotWithValue(int& iValue, bool bWantBest, bool bWantWorked, bool bForced = false, bool Logging = false);

	// Worked Plots
	bool IsWorkingPlot(int iRelativeIndex) const;
	bool IsWorkingPlot(const CvPlot* pPlot) const;
	void SetWorkingPlot(CvPlot* pPlot, bool bNewValue, bool bUseUnassignedPool = true, bool bUpdateNow = true);
	void DoAlterWorkingPlot(int iIndex);

	// Forced Working Plots (human override)
	bool IsForcedWorkingPlot(const CvPlot* pPlot) const;
	void SetForcedWorkingPlot(CvPlot* pPlot, bool bNewValue);

	bool DoValidateForcedWorkingPlots();
	bool DoDemoteWorstForcedWorkingPlot();

	int GetNumForcedWorkingPlots() const;
	void ChangeNumForcedWorkingPlots(int iChange);

	bool IsCanWork(CvPlot* pPlot) const;
	bool IsAnyPlotBlockaded() const;

	void DoVerifyWorkingPlots();

	const std::vector<int>& GetWorkedPlots() const { return m_vWorkedPlots; }

	// Helpful Stuff
	int GetCityIndexFromPlot(const CvPlot* pPlot) const;
	CvPlot* GetCityPlotFromIndex(int iIndex) const;

	// Specialists
	void DoSpecialists();

	int GetSpecialistRate(SpecialistTypes eSpecialist);
	bool IsCanAddSpecialistToBuilding(BuildingTypes eBuilding);
	void DoAddSpecialistToBuilding(BuildingTypes eBuilding, bool bForced, bool bUpdateNow = true);
	void DoRemoveSpecialistFromBuilding(BuildingTypes eBuilding, bool bForced, bool bEliminatePopulation = false, bool bUpdateNow = true);
	void DoRemoveAllSpecialistsFromBuilding(BuildingTypes eBuilding, bool bEliminatePopulation = false);
	bool DoRemoveWorstSpecialist(SpecialistTypes eDontChangeSpecialist, const BuildingTypes eDontRemoveFromBuilding = NO_BUILDING, bool bUpdateNow = true);

	int GetNumDefaultSpecialists() const;
	void ChangeNumDefaultSpecialists(int iChange, bool bUpdateNow = true);
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

	int GetSpecialistUpgradeThreshold(UnitClassTypes eUnitClass);
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
	void DoSpawnGreatPerson(UnitTypes eUnit, bool bIncrementCount, bool bCountAsProphet, bool bIsFree);
#else
	void DoSpawnGreatPerson(UnitTypes eUnit, bool bIncrementCount, bool bCountAsProphet);
#endif

	YieldTypes GetFocusTypeYield(CityAIFocusTypes eFocus);

private:

	CvCity* m_pCity;

	bool m_bAutomated;
	bool m_bNoAutoAssignSpecialists;
	bool m_bIsDirty;
	bool m_bIsBlockaded;

	int m_iNumUnassignedCitizens;
	int m_iNumCitizensWorkingPlots;
	int m_iNumForcedWorkingPlots;

	CityAIFocusTypes m_eCityAIFocusTypes;
	bool m_bForceAvoidGrowth;
	bool m_bDummy;

	bool m_pabWorkingPlot[MAX_CITY_PLOTS];
	bool m_pabForcedWorkingPlot[MAX_CITY_PLOTS];
	std::vector<int> m_vWorkedPlots;

	int m_iNumDefaultSpecialists;
	int m_iNumForcedDefaultSpecialists;
	int* m_aiSpecialistCounts;
	int* m_aiSpecialistSlots;
	int* m_aiSpecialistGreatPersonProgressTimes100;
	int* m_aiNumSpecialistsInBuilding;
	int* m_aiNumForcedSpecialistsInBuilding;
	int* m_piBuildingGreatPeopleRateChanges;

	bool m_bInited;

};

#endif // CIV5_CITY_CITIZENS_H