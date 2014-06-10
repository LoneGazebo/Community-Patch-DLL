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

	int GetPlotValue(CvPlot* pPlot, bool bUseAllowGrowthFlag);

	// Are this City's Citizens automated? (always true for AI civs)
	bool IsAutomated() const;
	void SetAutomated(bool bValue);

	bool IsNoAutoAssignSpecialists() const;
	void SetNoAutoAssignSpecialists(bool bValue);

	bool IsAvoidGrowth();
	bool IsForcedAvoidGrowth();
	void SetForcedAvoidGrowth(bool bAvoidGrowth);
	CityAIFocusTypes GetFocusType() const;
	void SetFocusType(CityAIFocusTypes eFocus);

	// Specialist AI
	bool IsAIWantSpecialistRightNow();
	BuildingTypes GetAIBestSpecialistBuilding(int& iSpecialistValue);
	int GetSpecialistValue(SpecialistTypes eSpecialist);
	bool IsBetterThanDefaultSpecialist(SpecialistTypes eSpecialist);

	// Citizen Assignment
	int GetNumUnassignedCitizens() const;
	void ChangeNumUnassignedCitizens(int iChange);
	int GetNumCitizensWorkingPlots() const;
	void ChangeNumCitizensWorkingPlots(int iChange);

	bool DoAddBestCitizenFromUnassigned();
	bool DoRemoveWorstCitizen(bool bRemoveForcedStatus = false, SpecialistTypes eDontChangeSpecialist = NO_SPECIALIST, int iCurrentCityPopulation = -1);

	void DoReallocateCitizens();

	CvPlot* GetBestCityPlotWithValue(int& iValue, bool bWantBest, bool bWantWorked);

	// Worked Plots
	bool IsWorkingPlot(const CvPlot* pPlot) const;
	void SetWorkingPlot(CvPlot* pPlot, bool bNewValue, bool bUseUnassignedPool = true);
	void DoAlterWorkingPlot(int iIndex);

	// Forced Working Plots (human override)
	bool IsForcedWorkingPlot(const CvPlot* pPlot) const;
	void SetForcedWorkingPlot(CvPlot* pPlot, bool bNewValue);

	void DoValidateForcedWorkingPlots();
	void DoDemoteWorstForcedWorkingPlot();

	int GetNumForcedWorkingPlots() const;
	void ChangeNumForcedWorkingPlots(int iChange);

	bool IsCanWork(CvPlot* pPlot) const;
	bool IsPlotBlockaded(CvPlot* pPlot) const;
	bool IsAnyPlotBlockaded() const;

	void DoVerifyWorkingPlot(CvPlot* pPlot);
	void DoVerifyWorkingPlots();

	// Helpful Stuff
	int GetCityIndexFromPlot(const CvPlot* pPlot) const;
	CvPlot* GetCityPlotFromIndex(int iIndex) const;

	// Specialists
	void DoSpecialists();

	bool IsCanAddSpecialistToBuilding(BuildingTypes eBuilding);
	void DoAddSpecialistToBuilding(BuildingTypes eBuilding, bool bForced);
	void DoRemoveSpecialistFromBuilding(BuildingTypes eBuilding, bool bForced, bool bEliminatePopulation = false);
	void DoRemoveAllSpecialistsFromBuilding(BuildingTypes eBuilding, bool bEliminatePopulation = false);
	bool DoRemoveWorstSpecialist(SpecialistTypes eDontChangeSpecialist, const BuildingTypes eDontRemoveFromBuilding = NO_BUILDING);

	int GetNumDefaultSpecialists() const;
	void ChangeNumDefaultSpecialists(int iChange);
	int GetNumForcedDefaultSpecialists() const;
	void ChangeNumForcedDefaultSpecialists(int iChange);

	int GetSpecialistCount(SpecialistTypes eIndex) const;
	int GetTotalSpecialistCount() const;

	int GetBuildingGreatPeopleRateChanges(SpecialistTypes eSpecialist) const;
	void ChangeBuildingGreatPeopleRateChanges(SpecialistTypes eSpecialist, int iChange);

	int GetSpecialistGreatPersonProgress(SpecialistTypes eIndex) const;
	int GetSpecialistGreatPersonProgressTimes100(SpecialistTypes eIndex) const;
	void ChangeSpecialistGreatPersonProgressTimes100(SpecialistTypes eIndex, int iChange);
	void DoResetSpecialistGreatPersonProgressTimes100(SpecialistTypes eIndex);

	int GetNumSpecialistsInBuilding(BuildingTypes eBuilding) const;
	int GetNumForcedSpecialistsInBuilding(BuildingTypes eBuilding) const;

	void DoClearForcedSpecialists();

	int GetNumSpecialistsAllowedByBuilding(const CvBuildingEntry& kBuilding);

	int GetSpecialistUpgradeThreshold(UnitClassTypes eUnitClass);
	void DoSpawnGreatPerson(UnitTypes eUnit, bool bIncrementCount, bool bCountAsProphet);

private:

	CvCity* m_pCity;

	bool m_bAutomated;
	bool m_bNoAutoAssignSpecialists;

	int m_iNumUnassignedCitizens;
	int m_iNumCitizensWorkingPlots;
	int m_iNumForcedWorkingPlots;

	CityAIFocusTypes m_eCityAIFocusTypes;
	bool m_bForceAvoidGrowth;

	bool m_pabWorkingPlot[NUM_CITY_PLOTS];
	bool m_pabForcedWorkingPlot[NUM_CITY_PLOTS];

	int m_iNumDefaultSpecialists;
	int m_iNumForcedDefaultSpecialists;
	int* m_aiSpecialistCounts;
	int* m_aiSpecialistGreatPersonProgressTimes100;
	int* m_aiNumSpecialistsInBuilding;
	int* m_aiNumForcedSpecialistsInBuilding;
	int* m_piBuildingGreatPeopleRateChanges;

	bool m_bInited;

};

#endif // CIV5_CITY_CITIZENS_H