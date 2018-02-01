/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

// city.h

#ifndef CIV5_CITY_H
#define CIV5_CITY_H

#include "LinkedList.h"
#include "CvUnit.h"
#include "CvAIOperation.h"
#include "CvEnumSerialization.h"
#include "FStlContainerSerialization.h"
#include "FAutoVariable.h"
#include "CvPreGame.h"

// 0 = center of city, 1-6 = the edge of city on points, 7-12 = one tile out
#define NUM_CITY_BUILDING_DISPLAY_SLOTS (13)

class CvPlot;
class CvArea;
class CvGenericBuilding;
class CvCityBuildings;
class CvCityStrategyAI;
class CvCityCitizens;
class CvCityEmphases;
class CvCityReligions;
class CvCityEspionage;
class CvCityCulture;
class CvPlayer;

class CvCity
{

public:
	CvCity();
	virtual ~CvCity();

#if defined(MOD_API_EXTENSIONS)
	void init(int iID, PlayerTypes eOwner, int iX, int iY, bool bBumpUnits = true, bool bInitialFounding = true, ReligionTypes eInitialReligion = NO_RELIGION, const char* szName = NULL);
#else
	void init(int iID, PlayerTypes eOwner, int iX, int iY, bool bBumpUnits = true, bool bInitialFounding = true);
#endif
	void uninit();
	void reset(int iID = 0, PlayerTypes eOwner = NO_PLAYER, int iX = 0, int iY = 0, bool bConstructorCall = false);
	void setupGraphical();
	void setupWonderGraphics();
	void setupBuildingGraphics();
	void setupSpaceshipGraphics();


#if defined(MOD_GLOBAL_VENICE_KEEPS_RESOURCES)
	void kill(bool bVenice = false);
	void PreKill(bool bVenice = false);
#else
	void kill();
	void PreKill();
#endif
	void PostKill(bool bCapital, CvPlot* pPlot, PlayerTypes eOwner);

#if defined(MOD_BALANCE_CORE)
	CvPlayer* GetPlayer() const;
#else
	CvPlayer* GetPlayer();
#endif

	void doTurn();

	bool isCitySelected();
	bool canBeSelected() const;
	void updateSelectedCity();
#if defined(MOD_BALANCE_CORE)
	void updateYield(bool bSkipCity = false);
#else
	void updateYield();
#endif
#if defined(MOD_BALANCE_CORE)
	void UpdateCityYields(YieldTypes eYield);
	void SetStaticYield(YieldTypes eYield, int iValue);
	int GetStaticYield(YieldTypes eYield) const;

	void SetThreatRank(int iValue);
	int GetThreatRank() const;

	void SetCoastalThreatRank(int iValue);
	int GetCoastalThreatRank() const;

	void SetTradePriorityLand(int iValue);
	int GetTradePriorityLand(void) const;

	void SetTradePrioritySea(int iValue);
	int GetTradePrioritySea(void) const;

	void ChangeTradeRouteSeaDistanceModifier(int iValue);
	int GetTradeRouteSeaDistanceModifier() const;

	void ChangeTradeRouteLandDistanceModifier(int iValue);
	int GetTradeRouteLandDistanceModifier() const;

	int GetLongestPotentialTradeRoute(DomainTypes eDomain) const;
	void SetLongestPotentialTradeRoute(int iValue, DomainTypes eDomain);

	bool AreOurBordersTouching(PlayerTypes ePlayer);
#endif

#if defined(MOD_BALANCE_CORE_EVENTS)
	void DoEvents();
	bool IsCityEventValid(CityEventTypes eEvent);
	bool IsCityEventChoiceValid(CityEventChoiceTypes eEventChoice, CityEventTypes eParentEvent);
	void DoCancelEventChoice(CityEventChoiceTypes eEventChoice);
	void DoStartEvent(CityEventTypes eEvent);
	void DoEventChoice(CityEventChoiceTypes eEventChoice, CityEventTypes eCityEvent = NO_EVENT_CITY);
	CvString GetScaledHelpText(CityEventChoiceTypes eEventChoice, bool bYieldsOnly);
	CvString GetDisabledTooltip(CityEventChoiceTypes eEventChoice);

	void SetEventActive(CityEventTypes eEvent, bool bValue);
	bool IsEventActive(CityEventTypes eEvent) const;

	void SetEventChoiceActive(CityEventChoiceTypes eEventChoice, bool bValue);
	bool IsEventChoiceActive(CityEventChoiceTypes eEventChoice) const;

	int GetEventCooldown(CityEventTypes eEvent) const;
	void ChangeEventCooldown(CityEventTypes eEvent, int iValue);
	void SetEventCooldown(CityEventTypes eEvent, int iValue);

	void ChangeCityEventCooldown(int iValue);
	int GetCityEventCooldown() const;

	void IncrementEvent(CityEventTypes eEvent, int iValue);
	int GetEventIncrement(CityEventTypes eEvent) const;

	int GetEventChoiceDuration(CityEventChoiceTypes eEventChoice) const;
	void ChangeEventChoiceDuration(CityEventChoiceTypes eEventChoice, int iValue);
	void SetEventChoiceDuration(CityEventChoiceTypes eEventChoice, int iValue);

	void ChangeEventCityYield(YieldTypes eYield, int iValue);
	int GetEventCityYield(YieldTypes eYield) const;

	void ChangeEventBuildingClassYield(BuildingClassTypes eBuildingClass, YieldTypes eYield, int iValue);
	int GetEventBuildingClassCityYield(BuildingClassTypes eBuildingClass, YieldTypes eYield) const;

	void ChangeEventBuildingClassYieldModifier(BuildingClassTypes eBuildingClass, YieldTypes eYield, int iValue);
	int GetEventBuildingClassCityYieldModifier(BuildingClassTypes eBuildingClass, YieldTypes eYield) const;

	void ChangeEventImprovementYield(ImprovementTypes eImprovement, YieldTypes eYield, int iValue);
	int GetEventImprovementYield(ImprovementTypes eImprovement, YieldTypes eYield) const;

	void ChangeEventResourceYield(ResourceTypes eResource, YieldTypes eYield, int iValue);
	int GetEventResourceYield(ResourceTypes eResource, YieldTypes eYield) const;

	void ChangeEventSpecialistYield(SpecialistTypes eSpecialist, YieldTypes eYield, int iValue);
	int GetEventSpecialistYield(SpecialistTypes eSpecialist, YieldTypes eYield) const;

	void ChangeEventTerrainYield(TerrainTypes eTerrain, YieldTypes eYield, int iValue);
	int GetEventTerrainYield(TerrainTypes eTerrain, YieldTypes eYield) const;
	void ChangeEventFeatureYield(FeatureTypes eFeature, YieldTypes eYield, int iValue);
	int GetEventFeatureYield(FeatureTypes eFeature, YieldTypes eYield) const;

	bool IsEventChoiceFired(CityEventChoiceTypes eEventChoice) const;
	void SetEventChoiceFired(CityEventChoiceTypes eEventChoice, bool bValue);

	bool IsEventFired(CityEventTypes eEvent) const;
	void SetEventFired(CityEventTypes eEvent, bool bValue);

	void ChangeEventHappiness(int iValue);
	int GetEventHappiness() const;

	virtual void AI_DoEventChoice(CityEventTypes eEvent) = 0;
#endif

	bool IsIndustrialRouteToCapitalConnected() const;
	void SetIndustrialRouteToCapitalConnected(bool bValue);

	void SetRouteToCapitalConnected(bool bValue);
	bool IsRouteToCapitalConnected(void) const;

#if defined(MOD_GLOBAL_TRULY_FREE_GP)
	void createGreatGeneral(UnitTypes eGreatPersonUnit, bool bIsFree);
	void createGreatAdmiral(UnitTypes eGreatPersonUnit, bool bIsFree);
#else
	void createGreatGeneral(UnitTypes eGreatPersonUnit);
	void createGreatAdmiral(UnitTypes eGreatPersonUnit);
#endif

	CityTaskResult doTask(TaskTypes eTask, int iData1 = -1, int iData2 = -1, bool bOption = false, bool bAlt = false, bool bShift = false, bool bCtrl = false);

	void chooseProduction(UnitTypes eTrainUnit = NO_UNIT, BuildingTypes eConstructBuilding = NO_BUILDING, ProjectTypes eCreateProject = NO_PROJECT, bool bFinish = false, bool bFront = false);

	int getBuyPlotDistance() const;
	int getWorkPlotDistance(int iChange = 0) const;
	int GetNumWorkablePlots(int iChange = 0) const;

	void clearWorkingOverride(int iIndex);
	int countNumImprovedPlots(ImprovementTypes eImprovement = NO_IMPROVEMENT, bool bPotential = false) const;
	int countNumWaterPlots() const;
	int countNumRiverPlots() const;
	int countNumForestPlots() const;

	int findPopulationRank();
	int findBaseYieldRateRank(YieldTypes eYield);
	int findYieldRateRank(YieldTypes eYield);

	UnitTypes allUpgradesAvailable(UnitTypes eUnit, int iUpgradeCount = 0) const;
	bool isWorldWondersMaxed() const;
	bool isTeamWondersMaxed() const;
	bool isNationalWondersMaxed() const;
	bool isBuildingsMaxed() const;

	bool canTrain(UnitTypes eUnit, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, bool bWillPurchase = false, CvString* toolTipSink = NULL) const;
	bool canTrain(UnitCombatTypes eUnitCombat) const;
#if defined(MOD_API_EXTENSIONS)
	bool canConstruct(BuildingTypes eBuilding, const std::vector<int>& vPreExistingBuildings, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, bool bWillPurchase = false, CvString* toolTipSink = NULL) const;
	bool canConstruct(BuildingTypes eBuilding, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, bool bWillPurchase = false, CvString* toolTipSink = NULL) const;
#else
	bool canConstruct(BuildingTypes eBuilding, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, CvString* toolTipSink = NULL) const;
#endif
	bool canCreate(ProjectTypes eProject, bool bContinue = false, bool bTestVisible = false) const;
	bool canPrepare(SpecialistTypes eSpecialist, bool bContinue = false) const;
	bool canMaintain(ProcessTypes eProcess, bool bContinue = false) const;
	bool canJoin() const;

	bool IsFeatureSurrounded() const;
	void SetFeatureSurrounded(bool bValue);
	void DoUpdateFeatureSurrounded();

	int GetResourceExtraYield(ResourceTypes eResource, YieldTypes eYield) const;
	void ChangeResourceExtraYield(ResourceTypes eResource, YieldTypes eYield, int iChange);

	int GetFeatureExtraYield(FeatureTypes eFeature, YieldTypes eYield) const;
	void ChangeFeatureExtraYield(FeatureTypes eFeature, YieldTypes eYield, int iChange);

#if defined(MOD_BALANCE_CORE)
	int GetImprovementExtraYield(ImprovementTypes eImprovement, YieldTypes eYield) const;
	void ChangeImprovementExtraYield(ImprovementTypes eImprovement, YieldTypes eYield, int iChange);

	int GetExtraBuildingMaintenance() const;
	void SetExtraBuildingMaintenance(int iChange);

	int GetYieldPerXTerrain(TerrainTypes eTerrain, YieldTypes eYield) const;
	int GetYieldPerXTerrainFromReligion(TerrainTypes eTerrain, YieldTypes eYield) const;

	void UpdateYieldPerXTerrain(YieldTypes eYield, TerrainTypes eTerrain = NO_TERRAIN);
	void UpdateYieldPerXTerrainFromReligion(YieldTypes eYield, TerrainTypes eTerrain = NO_TERRAIN);

	void ChangeNumTerrainWorked(TerrainTypes eTerrain, int iChange);
	int GetNumTerrainWorked(TerrainTypes eTerrain);

	void ChangeNumFeaturelessTerrainWorked(TerrainTypes eTerrain, int iChange);
	int GetNumFeaturelessTerrainWorked(TerrainTypes eTerrain);

	void ChangeNumFeatureWorked(FeatureTypes eFeature, int iChange);
	int GetNumFeatureWorked(FeatureTypes eFeature);

	void ChangeNumResourceWorked(ResourceTypes eResource, int iChange);
	int GetNumResourceWorked(ResourceTypes eResource);

	void ChangeNumImprovementWorked(ImprovementTypes eImprovement, int iChange);
	int GetNumImprovementWorked(ImprovementTypes eImprovement);

	void SetYieldPerXTerrain(TerrainTypes eTerrain, YieldTypes eYield, int iValue);
	void SetYieldPerXTerrainFromReligion(TerrainTypes eTerrain, YieldTypes eYield, int iValue);

	int GetYieldPerXTerrainFromBuildingsTimes100(TerrainTypes eTerrain, YieldTypes eYield) const;
	void ChangeYieldPerXTerrainFromBuildingsTimes100(TerrainTypes eTerrain, YieldTypes eYield, int iChange);

	void SetYieldPerXFeature(FeatureTypes eFeature, YieldTypes eYield, int iValue);
	void SetYieldPerXUnimprovedFeature(FeatureTypes eFeature, YieldTypes eYield, int iValue);

	void UpdateYieldPerXFeature(YieldTypes eYield, FeatureTypes eFeature = NO_FEATURE);
	void UpdateYieldPerXUnimprovedFeature(YieldTypes eYield, FeatureTypes eFeature = NO_FEATURE);

	int getHurryModifier(HurryTypes eIndex) const;
	void changeHurryModifier(HurryTypes eIndex, int iChange);

	int getSpecialistExtraYield(SpecialistTypes eIndex1, YieldTypes eIndex2) const;
	void changeSpecialistExtraYield(SpecialistTypes eIndex1, YieldTypes eIndex2, int iChange);
#endif
	int GetTerrainExtraYield(TerrainTypes eTerrain, YieldTypes eYield) const;
	void ChangeTerrainExtraYield(TerrainTypes eTerrain, YieldTypes eYield, int iChange);

#if defined(MOD_API_UNIFIED_YIELDS) && defined(MOD_API_PLOT_YIELDS)
	int GetPlotExtraYield(PlotTypes ePlot, YieldTypes eYield) const;
	void ChangePlotExtraYield(PlotTypes ePlot, YieldTypes eYield, int iChange);
#endif
#if defined(MOD_BALANCE_CORE)
	bool IsHasFeatureLocal(FeatureTypes eFeature) const;
#endif
	bool IsHasResourceLocal(ResourceTypes eResource, bool bTestVisible) const;
#if defined(MOD_API_EXTENSIONS) || defined(MOD_TRADE_WONDER_RESOURCE_ROUTES)
	int GetNumResourceLocal(ResourceTypes eResource, bool bImproved = false, bool bNoImprovement = false);
#endif
	void ChangeNumResourceLocal(ResourceTypes eResource, int iChange);

	bool IsBuildingLocalResourceValid(BuildingTypes eBuilding, bool bTestVisible, CvString* toolTipSink = NULL) const;
#if defined(MOD_BALANCE_CORE_DEALS)
	bool IsBuildingResourceMonopolyValid(BuildingTypes eBuilding, CvString* toolTipSink = NULL) const;
#endif
#if defined(MOD_BALANCE_CORE)
	bool IsBuildingFeatureValid(BuildingTypes eBuilding, CvString* toolTipSink = NULL) const;
#endif
	// Resource Demanded

	ResourceTypes GetResourceDemanded(bool bHideUnknown = true) const;
	void SetResourceDemanded(ResourceTypes eResource);
	void DoPickResourceDemanded(bool bCurrentResourceInvalid = true);
	void DoTestResourceDemanded();

	void DoSeedResourceDemandedCountdown();
	int GetResourceDemandedCountdown() const;
	void SetResourceDemandedCountdown(int iValue);
	void ChangeResourceDemandedCountdown(int iChange);

	// End Resource Demanded

	int getFoodTurnsLeft() const;
	bool isProduction() const;
	bool isProductionLimited() const;
	bool isProductionUnit() const;
	bool isProductionBuilding() const;
	bool isProductionProject() const;
	bool isProductionSpecialist() const;
	bool isProductionProcess() const;

	bool canContinueProduction(OrderData order);
	int getProductionExperience(UnitTypes eUnit = NO_UNIT);
	void addProductionExperience(CvUnit* pUnit, bool bConscript = false, bool bGoldPurchase = false);

	UnitTypes getProductionUnit() const;
	UnitAITypes getProductionUnitAI() const;
	BuildingTypes getProductionBuilding() const;
	ProjectTypes getProductionProject() const;
	SpecialistTypes getProductionSpecialist() const;
	ProcessTypes getProductionProcess() const;
	const char* getProductionName() const;
	const char* getProductionNameKey() const;
	int getGeneralProductionTurnsLeft() const;

	bool isFoodProduction() const;
	bool isFoodProduction(UnitTypes eUnit) const;
	int getFirstUnitOrder(UnitTypes eUnit) const;
	int getFirstBuildingOrder(BuildingTypes eBuilding) const;
	int getFirstProjectOrder(ProjectTypes eProject) const;
	int getFirstSpecialistOrder(SpecialistTypes eSpecialist) const;
	int getNumTrainUnitAI(UnitAITypes eUnitAI) const;

	int getProduction() const;
	int getProductionTimes100() const;
	int getProductionNeeded() const;
	int getProductionNeeded(UnitTypes eUnit) const;
	int getProductionNeeded(BuildingTypes eBuilding) const;
	int getProductionNeeded(ProjectTypes eProject) const;
	int getProductionNeeded(SpecialistTypes eSpecialist) const;
	int getProductionTurnsLeft() const;
	int getProductionTurnsLeft(UnitTypes eUnit, int iNum) const;
	int getProductionTurnsLeft(BuildingTypes eBuilding, int iNum) const;
	int getProductionTurnsLeft(ProjectTypes eProject, int iNum) const;
	int getProductionTurnsLeft(SpecialistTypes eSpecialist, int iNum) const;
#if defined(MOD_PROCESS_STOCKPILE)
	int getProductionNeeded(ProcessTypes eProcess) const;
	int getProductionTurnsLeft(ProcessTypes eProcess, int iNum) const;
#endif
#if defined(MOD_BALANCE_CORE)
	void SetBuildingInvestment(BuildingClassTypes eBuildingClass, bool bValue);
	bool IsBuildingInvestment(BuildingClassTypes eBuildingClass) const;

	void SetUnitInvestment(UnitClassTypes eUnitClass, bool bValue);
	bool IsUnitInvestment(UnitClassTypes eUnitClass) const;

	void SetBuildingConstructed(BuildingClassTypes eBuildingClass, bool bValue);
	bool IsBuildingConstructed(BuildingClassTypes eBuildingClass) const;
#endif
	int GetPurchaseCost(UnitTypes eUnit);
	int GetFaithPurchaseCost(UnitTypes eUnit, bool bIncludeBeliefDiscounts);
	int GetPurchaseCost(BuildingTypes eBuilding);
	int GetFaithPurchaseCost(BuildingTypes eBuilding);
	int GetPurchaseCost(ProjectTypes eProject);
	int GetPurchaseCostFromProduction(int iProduction);

	int getProductionTurnsLeft(int iProductionNeeded, int iProduction, int iFirstProductionDifference, int iProductionDifference) const;
	void setProduction(int iNewValue);
	void changeProduction(int iChange);
	void setProductionTimes100(int iNewValue);
	void changeProductionTimes100(int iChange);

	int getProductionModifier(_In_opt_ CvString* toolTipSink = NULL) const;
	int getGeneralProductionModifiers(_In_opt_ CvString* toolTipSink = NULL) const;
	int getProductionModifier(UnitTypes eUnit, _In_opt_ CvString* toolTipSink = NULL) const;
	int getProductionModifier(BuildingTypes eBuilding, _In_opt_ CvString* toolTipSink = NULL) const;
	int getProductionModifier(ProjectTypes eProject, _In_opt_ CvString* toolTipSink = NULL) const;
	int getProductionModifier(SpecialistTypes eSpecialist, _In_opt_ CvString* toolTipSink = NULL) const;
	int getProductionModifier(ProcessTypes eProcess, _In_opt_ CvString* toolTipSink = NULL) const;

	int getOverflowProductionDifference(int iProductionNeeded, int iProduction, int iProductionModifier, int iDiff, int iModifiedProduction) const;
	int getProductionDifference(int iProductionNeeded, int iProduction, int iProductionModifier, bool bFoodProduction, bool bOverflow) const;
	int getCurrentProductionDifference(bool bIgnoreFood, bool bOverflow) const;
	int getRawProductionDifference(bool bIgnoreFood, bool bOverflow) const;
	int getProductionDifferenceTimes100(int iProductionNeeded, int iProduction, int iProductionModifier, bool bFoodProduction, bool bOverflow) const;
	int getCurrentProductionDifferenceTimes100(bool bIgnoreFood, bool bOverflow) const;
	int getRawProductionDifferenceTimes100(bool bIgnoreFood, bool bOverflow) const;
	int getExtraProductionDifference(int iExtra) const;
	int GetFoodProduction(int iExcessFood) const;

	bool canHurry(HurryTypes eHurry, bool bTestVisible = false) const;
	void hurry(HurryTypes eHurry);

	UnitTypes getConscriptUnit() const;
	CvUnit* initConscriptedUnit();
	int getConscriptPopulation() const;
	int conscriptMinCityPopulation() const;
	bool canConscript() const;
	void conscript();

	int getResourceYieldRateModifier(YieldTypes eIndex, ResourceTypes eResource) const;

	void processResource(ResourceTypes eResource, int iChange);
#if defined(MOD_BALANCE_CORE)
	void processBuilding(BuildingTypes eBuilding, int iChange, bool bFirst, bool bObsolete = false, bool bApplyingAllCitiesBonus = false, bool bNoBonus = false);
#else
	void processBuilding(BuildingTypes eBuilding, int iChange, bool bFirst, bool bObsolete = false, bool bApplyingAllCitiesBonus = false);
#endif
	void processProcess(ProcessTypes eProcess, int iChange);
#if defined(MOD_BALANCE_CORE)
	void processSpecialist(SpecialistTypes eSpecialist, int iChange, bool bSkip = false);
#else
	void processSpecialist(SpecialistTypes eSpecialist, int iChange);
#endif

#if defined(MOD_BALANCE_CORE)
	void UpdateReligion(ReligionTypes eNewMajority, bool bRecalcPlotYields=true);
#else
	void UpdateReligion(ReligionTypes eNewMajority);
#endif
#if defined(MOD_BALANCE_CORE)
	bool HasPaidAdoptionBonus(ReligionTypes eReligion) const;
	void SetPaidAdoptionBonus(ReligionTypes eReligion, bool bNewValue);
#endif

	int GetReligiousPressureModifier(ReligionTypes eReligion) const;
	void SetReligiousPressureModifier(ReligionTypes eReligion, int iNewValue);
	void ChangeReligiousPressureModifier(ReligionTypes eReligion, int iNewValue);

	int GetCultureFromSpecialist(SpecialistTypes eSpecialist) const;

	const CvHandicapInfo& getHandicapInfo() const;
	HandicapTypes getHandicapType() const;

	const CvCivilizationInfo& getCivilizationInfo() const;
	CivilizationTypes getCivilizationType() const;

	LeaderHeadTypes getPersonalityType() const;

	ArtStyleTypes getArtStyleType() const;

	CitySizeTypes getCitySizeType() const;

	bool isBarbarian() const;
	bool isHuman() const;
	bool isVisible(TeamTypes eTeam, bool bDebug) const;

	bool isCapital() const;
	bool IsOriginalCapital() const;
	bool IsOriginalMajorCapital() const; // is the original capital of a major civ

	bool isCoastal(int iMinWaterSize = -1) const;
#if defined(MOD_API_EXTENSIONS)
	bool isAddsFreshWater() const;
#endif
#if defined(MOD_BALANCE_CORE)
	int GetUnitPurchaseCooldown(bool bCivilian = false) const;
	void SetUnitPurchaseCooldown(bool bCivilian = false, int iValue = 0);
	void ChangeUnitPurchaseCooldown(bool bCivilian = false, int iValue = 0);
	int GetBuildingPurchaseCooldown() const;
	void SetBuildingPurchaseCooldown(int iValue);
	void ChangeBuildingPurchaseCooldown(int iValue);
	void CheckForOperationUnits();
	void DoSellBuilding();
	void SetTraded(PlayerTypes ePlayer, bool bValue);
	bool IsTraded(PlayerTypes ePlayer);
#endif
#if defined(MOD_API_EXTENSIONS)
	int foodConsumptionSpecialistTimes100() const;
#endif
	int foodConsumption(bool bNoAngry = false, int iExtra = 0) const;
	int foodDifference(bool bBottom = true) const;
	int foodDifferenceTimes100(bool bBottom = true, CvString* toolTipSink = NULL) const;
	int growthThreshold() const;
#if defined(MOD_BALANCE_CORE)
	int GetUnhappinessFromCitySpecialists();
#endif

	int productionLeft() const;
	int hurryCost(HurryTypes eHurry, bool bExtra) const;
	int getHurryCostModifier(HurryTypes eHurry, bool bIgnoreNew = false) const;
	int hurryGold(HurryTypes eHurry) const;
	int hurryPopulation(HurryTypes eHurry) const;
	int hurryProduction(HurryTypes eHurry) const;
	int maxHurryPopulation() const;

	bool hasActiveWorldWonder() const;

	inline int GetID() const
	{
		return m_iID;
	}

	int getIndex() const;
	IDInfo GetIDInfo() const;
	void SetID(int iID);

	inline int getX() const
	{
		return m_iX;
	}

	inline int getY() const
	{
		return m_iY;
	}

	bool at(int iX, int iY) const;
	bool at(CvPlot* pPlot) const;
	CvPlot* plot() const;
	int getArea() const;
	//(coastal) cities can be part of multiple areas
	bool isAdjacentToArea(int iAreaID) const;
	bool isMatchingArea(const CvPlot* pTestPlot) const;
	bool hasSharedAdjacentArea(const CvCity* pOtherCity) const;

	void SetGarrison(CvUnit* pUnit);
	bool HasGarrison() const;
	CvUnit* GetGarrisonedUnit() const;

	CvPlot* getRallyPlot() const;
	void setRallyPlot(CvPlot* pPlot);

	int getGameTurnFounded() const;
	void setGameTurnFounded(int iNewValue);

	int getGameTurnAcquired() const;
	void setGameTurnAcquired(int iNewValue);

	int getGameTurnLastExpanded() const;
	void setGameTurnLastExpanded(int iNewValue);

#if defined(MOD_BALANCE_CORE)
	int GetAdditionalFood() const;
	void SetAdditionalFood(int iValue);
#endif

	int getPopulation() const;
#if defined(MOD_BALANCE_CORE)
	void setPopulation(int iNewValue, bool bReassignPop = true, bool bNoBonus = false);
#else
	void setPopulation(int iNewValue, bool bReassignPop = true);
#endif
	void changePopulation(int iChange, bool bReassignPop = true);

	long getRealPopulation() const;

	int getHighestPopulation() const;
	void setHighestPopulation(int iNewValue);

	int getNumGreatPeople() const;
	void changeNumGreatPeople(int iChange);

	int getBaseGreatPeopleRate() const;
	int getGreatPeopleRate() const;
	int getTotalGreatPeopleRateModifier() const;
	void changeBaseGreatPeopleRate(int iChange);
	int getGreatPeopleRateModifier() const;
	void changeGreatPeopleRateModifier(int iChange);

	// Culture stuff

	int GetJONSCultureStored() const;
	void SetJONSCultureStored(int iValue);
	void ChangeJONSCultureStored(int iChange);

	int GetJONSCultureLevel() const;
	void SetJONSCultureLevel(int iValue);
	void ChangeJONSCultureLevel(int iChange);
	void DoJONSCultureLevelIncrease();
	int GetJONSCultureThreshold() const;

#if defined(MOD_BALANCE_CORE)
	int getJONSCulturePerTurn(bool bStatic = true) const;
#else
	int getJONSCulturePerTurn() const;
#endif

	int GetBaseJONSCulturePerTurn() const;

	int GetJONSCulturePerTurnFromBuildings() const;
#if !defined(MOD_API_UNIFIED_YIELDS_CONSOLIDATION)
	void ChangeJONSCulturePerTurnFromBuildings(int iChange);
#endif

	int GetJONSCulturePerTurnFromPolicies() const;
	void ChangeJONSCulturePerTurnFromPolicies(int iChange);

	int GetJONSCulturePerTurnFromSpecialists() const;
	void ChangeJONSCulturePerTurnFromSpecialists(int iChange);

	int GetJONSCulturePerTurnFromGreatWorks() const;

	int GetJONSCulturePerTurnFromTraits() const;
#if defined(MOD_BALANCE_CORE)
	int GetYieldPerTurnFromTraits(YieldTypes eYield) const;
#endif

	int GetJONSCulturePerTurnFromReligion() const;
#if !defined(MOD_API_UNIFIED_YIELDS_CONSOLIDATION)
	void ChangeJONSCulturePerTurnFromReligion(int iChange);
#endif

	int GetJONSCulturePerTurnFromLeagues() const;

	int getCultureRateModifier() const;
	void changeCultureRateModifier(int iChange);
#if defined(MOD_BALANCE_CORE_POLICIES)
	int getBuildingClassCultureChange(BuildingClassTypes eIndex) const;
	void changeBuildingClassCultureChange(BuildingClassTypes eIndex, int iChange);
#endif
#if defined(MOD_BALANCE_CORE)
	void SetBaseTourism(int iChange);
	int GetBaseTourism() const;
	void SetBaseTourismBeforeModifiers(int iChange);
	int GetBaseTourismBeforeModifiers() const;
#endif
	// END Culture

#if defined(MOD_API_EXTENSIONS)
	int getTourismRateModifier() const;
	void changeTourismRateModifier(int iChange);
#endif
#if defined(MOD_BALANCE_CORE)
	int GetFaithPerTurn(bool bStatic = true) const;
#else
	int GetFaithPerTurn() const;
#endif
	int GetFaithPerTurnFromBuildings() const;
#if !defined(MOD_API_UNIFIED_YIELDS_CONSOLIDATION)
	void ChangeFaithPerTurnFromBuildings(int iChange);
#endif

	int GetFaithPerTurnFromPolicies() const;
	void ChangeFaithPerTurnFromPolicies(int iChange);

#if defined(MOD_API_UNIFIED_YIELDS)
	int GetYieldPerXFeature(FeatureTypes eFeature, YieldTypes eYield) const;
	int GetYieldPerTurnFromUnimprovedFeatures(FeatureTypes eFeature, YieldTypes eYield) const;
#else
	int GetFaithPerTurnFromTraits() const;
#endif

	int GetFaithPerTurnFromReligion() const;
#if !defined(MOD_API_UNIFIED_YIELDS_CONSOLIDATION)
	void ChangeFaithPerTurnFromReligion(int iChange);
#endif

	int getNumWorldWonders() const;
	void changeNumWorldWonders(int iChange);

	int getNumTeamWonders() const;
	void changeNumTeamWonders(int iChange);

	int getNumNationalWonders() const;
	void changeNumNationalWonders(int iChange);

	int GetWonderProductionModifier() const;
	void ChangeWonderProductionModifier(int iChange);

	int GetLocalResourceWonderProductionMod(BuildingTypes eBuilding, CvString* toolTipSink = NULL) const;

	int getCapturePlunderModifier() const;
	void changeCapturePlunderModifier(int iChange);

	int getPlotCultureCostModifier() const;
	void changePlotCultureCostModifier(int iChange);

	int getPlotBuyCostModifier() const;
	void changePlotBuyCostModifier(int iChange);
#if defined(MOD_BUILDINGS_CITY_WORKING)
	int GetCityWorkingChange() const;
	void changeCityWorkingChange(int iChange);
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	bool IsVassalLevyEra() const;
#endif
	int getHealRate() const;
	void changeHealRate(int iChange);

	int GetEspionageModifier() const;
	void ChangeEspionageModifier(int iChange);

#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
	int GetConversionModifier() const;
	void ChangeConversionModifier(int iChange);
#endif

	bool IsNoOccupiedUnhappiness() const;
	int GetNoOccupiedUnhappinessCount() const;
	void ChangeNoOccupiedUnhappinessCount(int iChange);

#if defined(HH_MOD_BUILDINGS_FRUITLESS_PILLAGE)
	bool IsLocalGainlessPillage() const;
	void ChangeLocalGainlessPillageCount(int iChange);
#endif
	int getFood() const;
	int getFoodTimes100() const;
	void setFood(int iNewValue);
	void setFoodTimes100(int iNewValue);
	void changeFood(int iChange);
	void changeFoodTimes100(int iChange);

	int getFoodKept() const;
	void setFoodKept(int iNewValue);
	void changeFoodKept(int iChange);

	int getMaxFoodKeptPercent() const;
	void changeMaxFoodKeptPercent(int iChange);

	int getOverflowProduction() const;
	void setOverflowProduction(int iNewValue);
	void changeOverflowProduction(int iChange);
	int getOverflowProductionTimes100() const;
	void setOverflowProductionTimes100(int iNewValue);
	void changeOverflowProductionTimes100(int iChange);

	int getFeatureProduction()const;
	void setFeatureProduction(int iNewValue);
	void changeFeatureProduction(int iChange);

	int getMilitaryProductionModifier() const;
	void changeMilitaryProductionModifier(int iChange);

	int getSpaceProductionModifier() const;
	void changeSpaceProductionModifier(int iChange);

	int getFreeExperience() const;
	void changeFreeExperience(int iChange);

	bool CanAirlift() const;

	int GetMaxAirUnits() const;
	void ChangeMaxAirUnits(int iChange);

	int getCitySupplyModifier() const;
	void changeCitySupplyModifier(int iChange);

	int getCitySupplyFlat() const;
	void changeCitySupplyFlat(int iChange);

	void SetProductionRoutes(bool bValue);
	bool IsProductionRoutes() const;

	void SetFoodRoutes(bool bValue);
	bool IsFoodRoutes() const;

	void SetAllowPuppetPurchase(bool bValue);
	bool IsAllowPuppetPurchase() const;

	int getNukeModifier() const;
	void changeNukeModifier(int iChange);

	int GetTradeRouteTargetBonus() const;
	void ChangeTradeRouteTargetBonus(int iChange);

	int GetTradeRouteRecipientBonus() const;
	void ChangeTradeRouteRecipientBonus(int iChange);

	int GetTradeRouteSeaGoldBonus() const;
	void ChangeTradeRouteSeaGoldBonus(int iChange);

	int GetTradeRouteLandGoldBonus() const;
	void ChangeTradeRouteLandGoldBonus(int iChange);

	int GetNumTradeRouteBonus() const;
	void ChangeNumTradeRouteBonus(int iChange);

	int GetCityConnectionTradeRouteGoldModifier() const;
	void ChangeCityConnectionTradeRouteGoldModifier(int iChange);

	bool isAreaBorderObstacle() const;

	bool IsResistance() const;
	int GetResistanceTurns() const;
	void ChangeResistanceTurns(int iChange);
	void DoResistanceTurn();

	bool IsRazing() const;
	int GetRazingTurns() const;
	void ChangeRazingTurns(int iChange);
	bool DoRazingTurn();

	bool IsOccupied() const;
	void SetOccupied(bool bValue);

	bool IsPuppet() const;
	void SetPuppet(bool bValue);
	void DoCreatePuppet();
	void DoAnnex();

	int GetLocalHappiness() const;
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	void UpdateComboHappiness();
	void SetComboHappiness(int iValue);
	int GetComboUnhappiness() const;
	void SetBuildingClassHappinessFromReligion(int iValue);
	int GetBuildingClassHappinessFromReligion() const;
	void UpdateBuildingClassHappinessFromReligion();
	int getHappinessDelta() const;
	int getThresholdSubtractions(YieldTypes eYield, int iMod = 0) const;
	int getThresholdAdditions(YieldTypes eYield = NO_YIELD) const;
	int getUnhappyCitizenCount() const;

	int getUnhappinessFromCultureYield() const;
	int getUnhappinessFromCultureNeeded(int iMod = 0) const;
	int getUnhappinessFromCultureRaw(int iLimit = INT_MAX) const;
	int getUnhappinessFromCulture() const;
	int getUnhappinessFromScienceYield() const;
	int getUnhappinessFromScienceNeeded(int iMod = 0) const;
	int getUnhappinessFromScienceRaw(int iLimit = INT_MAX) const;
	int getUnhappinessFromScience() const;
	int getUnhappinessFromDefenseYield() const;
	int getUnhappinessFromDefenseNeeded(int iMod = 0) const;
	int getUnhappinessFromDefenseRaw(int iLimit = INT_MAX) const;
	int getUnhappinessFromDefense() const;
	int getUnhappinessFromGoldYield() const;
	int getUnhappinessFromGoldNeeded(int iMod = 0) const;
	int getUnhappinessFromGoldRaw(int iLimit = INT_MAX) const;
	int getUnhappinessFromGold() const;
	int getUnhappinessFromConnectionRaw(int iLimit = INT_MAX) const;
	int getUnhappinessFromConnection() const;
	int getUnhappinessFromPillagedRaw(int iLimit = INT_MAX) const;
	int getUnhappinessFromPillaged() const;
	int getUnhappinessFromStarvingRaw(int iLimit = INT_MAX) const;
	int getUnhappinessFromStarving() const;
	int getUnhappinessFromReligionRaw(int iLimit = INT_MAX) const;
	int getUnhappinessFromReligion() const;

	int getUnhappinessAggregated() const;
	
	int GetNumPillagedPlots() const;
	void SetNumPillagedPlots(int iValue);
	void ChangeNumPillagedPlots(int iValue);

	int GetGrowthFromTourism() const;
	void SetGrowthFromTourism(int iValue);
	void ChangeGrowthFromTourism(int iValue);
	void UpdateGrowthFromTourism();
#endif
	int GetHappinessFromBuildings() const;
	int GetBaseHappinessFromBuildings() const;
	void ChangeBaseHappinessFromBuildings(int iChange);
	int GetUnmoddedHappinessFromBuildings() const;
	void ChangeUnmoddedHappinessFromBuildings(int iChange);
#if defined(MOD_BALANCE_CORE)
	void ChangeLocalUnhappinessMod(int iChange);
	int GetLocalUnhappinessMod() const;
#endif

	bool IsIgnoreCityForHappiness() const;
	void SetIgnoreCityForHappiness(bool bValue);

	BuildingTypes ChooseFreeCultureBuilding() const;
	BuildingTypes ChooseFreeFoodBuilding() const;

	int getCitySizeBoost() const;
	void setCitySizeBoost(int iBoost);

	bool isNeverLost() const;
	void setNeverLost(bool bNewValue);

	bool isDrafted() const;
	void setDrafted(bool bNewValue);

	bool IsOwedCultureBuilding() const;
	void SetOwedCultureBuilding(bool bNewValue);

#if defined(MOD_BUGFIX_FREE_FOOD_BUILDING)
	bool IsOwedFoodBuilding() const;
	void SetOwedFoodBuilding(bool bNewValue);
#endif
#if defined(MOD_BALANCE_CORE)
	void ChangeBorderObstacleCity(int iNewValue);
	int GetBorderObstacleCity() const;
	void SetBorderObstacleCity(int iValue);

	void ChangeBorderObstacleWater(int iNewValue);
	int GetBorderObstacleWater() const;
	void SetBorderObstacleWater(int iValue);

	void ChangeNearbyMountains(int iNewValue);
	int GetNearbyMountains() const;
	void SetNearbyMountains(int iValue);
#endif
#if defined(MOD_BALANCE_CORE)
	bool IsOwedChosenBuilding(BuildingClassTypes eBuildingClass) const;
	void SetOwedChosenBuilding(BuildingClassTypes eBuildingClass, bool bNewValue);
#endif

	//check both water and land
	bool IsBlockadedWaterAndLand() const;
	//check water or land
	bool IsBlockaded(bool bWater) const;

	int GetWeLoveTheKingDayCounter() const;
	void SetWeLoveTheKingDayCounter(int iValue);
	void ChangeWeLoveTheKingDayCounter(int iChange, bool bUATrigger = false);

	int GetLastTurnGarrisonAssigned() const;
	int GetNumThingsProduced() const;

	bool isProductionAutomated() const;
	void setProductionAutomated(bool bNewValue, bool bClear);

	bool isLayoutDirty() const;
	void setLayoutDirty(bool bNewValue);

	inline PlayerTypes getOwner() const
	{
		return m_eOwner;
	}

	TeamTypes getTeam() const;

	PlayerTypes getPreviousOwner() const;
	void setPreviousOwner(PlayerTypes eNewValue);

	PlayerTypes getOriginalOwner() const;
	void setOriginalOwner(PlayerTypes eNewValue);

	PlayerTypes GetPlayersReligion() const;
	void SetPlayersReligion(PlayerTypes eNewValue);
#if defined(MOD_BALANCE_CORE)
	void SetNoWarmonger(bool bValue);
	bool IsNoWarmongerYet();

	void ChangeNumTimesOwned(PlayerTypes ePlayer, int iValue);
	void SetNumTimesOwned(PlayerTypes ePlayer, int iValue);
	int GetNumTimesOwned(PlayerTypes ePlayer);
#endif
	// Yield

	int getSeaPlotYield(YieldTypes eIndex) const;
	void changeSeaPlotYield(YieldTypes eIndex, int iChange);

	int getRiverPlotYield(YieldTypes eIndex) const;
	void changeRiverPlotYield(YieldTypes eIndex, int iChange);

	int getLakePlotYield(YieldTypes eIndex) const;
	void changeLakePlotYield(YieldTypes eIndex, int iChange);

	int getSeaResourceYield(YieldTypes eIndex) const;
	void changeSeaResourceYield(YieldTypes eIndex, int iChange);

#if defined(MOD_API_UNIFIED_YIELDS)
	int GetYieldPerTurnFromReligion(YieldTypes eYield) const;
	void UpdateSpecialReligionYields(YieldTypes eYield);
	int GetSpecialReligionYields(YieldTypes eIndex) const;
	void SetSpecialReligionYields(YieldTypes eIndex, int iValue);
#endif

	int getBaseYieldRateModifier(YieldTypes eIndex, int iExtra = 0, CvString* toolTipSink = NULL) const;
#if defined(MOD_BALANCE_CORE)
	int getYieldRate(YieldTypes eIndex, bool bIgnoreTrade, bool bStatic = true) const;
	int getYieldRateTimes100(YieldTypes eIndex, bool bIgnoreTrade, bool bStatic = true) const;
#else
	int getYieldRate(YieldTypes eIndex, bool bIgnoreTrade) const;
	int getYieldRateTimes100(YieldTypes eIndex, bool bIgnoreTrade) const;
#endif
#if defined(MOD_PROCESS_STOCKPILE)
	int getBasicYieldRateTimes100(YieldTypes eIndex, bool bIgnoreTrade) const;
#endif

#if defined(MOD_BALANCE_CORE)
	void updateEconomicValue();
	int getEconomicValue(PlayerTypes ePossibleNewOwner);
#endif
#if defined(MOD_BALANCE_CORE_SPIES)
	void SetRank(int iRank);
	int GetRank() const;

	void SetTurnsSinceLastRankMessage(int iTurns);
	void ChangeTurnsSinceLastRankMessage(int iTurns);
	int GetTurnsSinceLastRankMessage() const;

	void DoRankIncreaseWarning(int iRank, bool bNotify);
	void SetEspionageRanking(int iPotential, bool bNotify);
#endif
	// Base Yield
	int getBaseYieldRate(YieldTypes eIndex) const;
#if defined(MOD_BALANCE_CORE)
	void UpdateCityScienceFromYield(YieldTypes eIndex, int iModifiedYield);
#endif

#if defined(MOD_DIPLOMACY_CITYSTATES)
	int GetBaseScienceFromArt() const;
#endif

#if defined(MOD_GLOBAL_GREATWORK_YIELDTYPES) || defined(MOD_API_UNIFIED_YIELDS)
	int GetBaseYieldRateFromGreatWorks(YieldTypes eIndex) const;
#endif

	int GetBaseYieldRateFromTerrain(YieldTypes eIndex) const;
	void ChangeBaseYieldRateFromTerrain(YieldTypes eIndex, int iChange);

	int GetBaseYieldRateFromBuildings(YieldTypes eIndex) const;
	void ChangeBaseYieldRateFromBuildings(YieldTypes eIndex, int iChange);

	int GetBaseYieldRateFromSpecialists(YieldTypes eIndex) const;
	void ChangeBaseYieldRateFromSpecialists(YieldTypes eIndex, int iChange);

	int GetBaseYieldRateFromMisc(YieldTypes eIndex) const;
	void ChangeBaseYieldRateFromMisc(YieldTypes eIndex, int iChange);

	int GetBaseYieldRateFromProcess(YieldTypes eIndex) const;

#if defined(MOD_DIPLOMACY_CITYSTATES)
	int GetBaseYieldRateFromLeague(YieldTypes eIndex) const;
	void ChangeBaseYieldRateFromLeague(YieldTypes eIndex, int iChange);

	void ChangeTotalArtsyAid(int iChange);
	int GetTotalArtsyAid() const;
	void SetTotalArtsyAid(int iValue);

	void ChangeTotalScienceyAid(int iChange);
	int GetTotalScienceyAid() const;
	void SetTotalScienceyAid(int iValue);

	void ChangeTotalGreatWorkAid(int iChange);
	int GetTotalGreatWorkAid() const;
	void SetTotalGreatWorkAid(int iValue);
#endif
#if defined(MOD_DIPLOMACY_CITYSTATES) || defined(MOD_BALANCE_CORE)
	int GetGrowthExtraYield(YieldTypes eIndex) const;
	void ChangeGrowthExtraYield(YieldTypes eIndex, int iChange);
#endif
#if defined(MOD_BALANCE_CORE)
	int GetYieldFromVictory(YieldTypes eIndex) const;
	void ChangeYieldFromVictory(YieldTypes eIndex, int iChange);

	int GetYieldFromPillage(YieldTypes eIndex) const;
	void ChangeYieldFromPillage(YieldTypes eIndex, int iChange);

	int GetGoldenAgeYieldMod(YieldTypes eIndex) const;
	void ChangeGoldenAgeYieldMod(YieldTypes eIndex, int iChange);

	int GetYieldFromWLTKD(YieldTypes eIndex) const;
	void ChangeYieldFromWLTKD(YieldTypes eIndex, int iChange);

	int GetThemingYieldBonus(YieldTypes eIndex) const;
	void ChangeThemingYieldBonus(YieldTypes eIndex, int iChange);

	int GetYieldFromSpyAttack(YieldTypes eIndex) const;
	void ChangeYieldFromSpyAttack(YieldTypes eIndex, int iChange);

	int GetYieldFromSpyDefense(YieldTypes eIndex) const;
	void ChangeYieldFromSpyDefense(YieldTypes eIndex, int iChange);

	int GetYieldFromConstruction(YieldTypes eIndex) const;
	void ChangeYieldFromConstruction(YieldTypes eIndex, int iChange);

	int GetYieldFromTech(YieldTypes eIndex) const;
	void ChangeYieldFromTech(YieldTypes eIndex, int iChange);

	int GetYieldFromBirth(YieldTypes eIndex) const;
	void ChangeYieldFromBirth(YieldTypes eIndex, int iChange);
	int GetYieldFromUnitProduction(YieldTypes eIndex) const;
	void ChangeYieldFromUnitProduction(YieldTypes eIndex, int iChange);
	int GetYieldFromBorderGrowth(YieldTypes eIndex) const;
	void ChangeYieldFromBorderGrowth(YieldTypes eIndex, int iChange);
	int GetYieldFromPolicyUnlock(YieldTypes eIndex) const;
	void ChangeYieldFromPolicyUnlock(YieldTypes eIndex, int iChange);
	int GetYieldFromPurchase(YieldTypes eIndex) const;
	void ChangeYieldFromPurchase(YieldTypes eIndex, int iChange);

	int GetYieldFromUnitLevelUp(YieldTypes eIndex) const;
	void ChangeYieldFromUnitLevelUp(YieldTypes eIndex, int iChange);

	int GetYieldPerAlly(YieldTypes eIndex) const;
	void ChangeYieldPerAlly(YieldTypes eYield, int iChange);

	int GetYieldPerFriend(YieldTypes eIndex) const;
	void ChangeYieldPerFriend(YieldTypes eYield, int iChange);

	int GetBuildingScienceFromYield(YieldTypes eIndex1) const;
	void ChangeBuildingScienceFromYield(YieldTypes eIndex, int iChange);

	int GetScienceFromYield(YieldTypes eIndex1) const;
	void SetScienceFromYield(YieldTypes eIndex1, int iChange);

	void ChangeSpecialistRateModifier(SpecialistTypes eSpecialist, int iChange);
	int GetSpecialistRateModifier(SpecialistTypes eSpecialist) const;
#endif

#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
	int GetPovertyUnhappiness() const;
	void ChangePovertyUnhappiness(int iChange);

	int GetDefenseUnhappiness() const;
	void ChangeDefenseUnhappiness(int iChange);

	int GetUnculturedUnhappiness() const;
	void ChangeUnculturedUnhappiness(int iChange);

	int GetIlliteracyUnhappiness() const;
	void ChangeIlliteracyUnhappiness(int iChange);

	int GetMinorityUnhappiness() const;
	void ChangeMinorityUnhappiness(int iChange);
#endif

	int GetBaseYieldRateFromReligion(YieldTypes eIndex) const;
	void ChangeBaseYieldRateFromReligion(YieldTypes eIndex, int iChange);

#if defined(MOD_BALANCE_CORE)
	int GetBaseYieldRateFromCSAlliance(YieldTypes eIndex) const;
	void ChangeBaseYieldRateFromCSAlliance(YieldTypes eIndex, int iChange);
	void SetBaseYieldRateFromCSAlliance(YieldTypes eIndex, int iValue);

	int GetBaseYieldRateFromCSFriendship(YieldTypes eIndex) const;
	void ChangeBaseYieldRateFromCSFriendship(YieldTypes eIndex, int iChange);
	void SetBaseYieldRateFromCSFriendship(YieldTypes eIndex, int iValue);

	void SetYieldFromMinors(YieldTypes eYield, int iValue);
	int GetYieldFromMinors(YieldTypes eYield) const;

	int GetReligiousTradeModifier() const;
	void ChangeReligiousTradeModifier(int iChange);
	void SetReligiousTradeModifier(int iValue);

	int GetCityAirStrikeDefense() const;
	void ChangeCityAirStrikeDefense(int iChange);
	void SetCityAirStrikeDefense(int iValue);

	int GetFreeBuildingTradeTargetCity() const;
	void ChangeFreeBuildingTradeTargetCity(int iChange);
	void SetFreeBuildingTradeTargetCity(int iValue);

	int GetTradeRouteCityMod(YieldTypes eIndex) const;

	int GetGPRateModifierPerXFranchises() const;

	bool IsHeadquarters() const;
	bool IsHasOffice() const;
	bool IsHasFranchise(CorporationTypes eCorporation) const;

	int GetBuildingYieldChangeFromCorporationFranchises(BuildingClassTypes eBuildingClass, YieldTypes eIndex) const;
	
	int GetYieldChangeFromCorporationFranchises(YieldTypes eIndex) const;
	void UpdateYieldFromCorporationFranchises(YieldTypes eIndex);
	void SetYieldChangeFromCorporationFranchises(YieldTypes eIndex, int iValue);

	int GetResourceQuantityPerXFranchises(ResourceTypes eResource) const;
	void ChangeResourceQuantityPerXFranchises(ResourceTypes eResource, int iChange);
	void SetResourceQuantityPerXFranchises(ResourceTypes eResource, int iValue);

	int GetLandTourismBonus() const;
	void ChangeLandTourismBonus(int iChange);
	void SetLandTourismBonus(int iValue);

	int GetSeaTourismBonus() const;
	void ChangeSeaTourismBonus(int iChange);
	void SetSeaTourismBonus(int iValue);

	int GetSeaTourismFromEvent();
	int GetLandTourismFromEvent();

	int GetAlwaysHeal() const;
	void ChangeAlwaysHeal(int iChange);
	void SetAlwaysHeal(int iValue);

	void ChangeResourceDiversityModifier(int iChange);
	int GetResourceDiversityModifier() const;

	bool IsBastion() const;
	void SetBastion(bool bValue);
	void TestBastion();

	void DoBarbIncursion();
#endif
#if defined(MOD_BALANCE_CORE_SPIES)
	void ChangeBlockBuildingDestruction(int iNewValue);
	void SetBlockBuildingDestruction(int iNewValue);
	int GetBlockBuildingDestruction() const;

	void ChangeBlockWWDestruction(int iNewValue);
	void SetBlockWWDestruction(int iNewValue);
	int GetBlockWWDestruction() const;

	void ChangeBlockUDestruction(int iNewValue);
	void SetBlockUDestruction(int iNewValue);
	int GetBlockUDestruction() const;

	void ChangeBlockGPDestruction(int iNewValue);
	void SetBlockGPDestruction(int iNewValue);
	int GetBlockGPDestruction() const;

	void ChangeBlockRebellion(int iNewValue);
	void SetBlockRebellion(int iNewValue);
	int GetBlockRebellion() const;

	void ChangeBlockUnrest(int iNewValue);
	void SetBlockUnrest(int iNewValue);
	int GetBlockUnrest() const;

	void ChangeBlockScience(int iNewValue);
	void SetBlockScience(int iNewValue);
	int GetBlockScience() const;

	void ChangeBlockGold(int iNewValue);
	void SetBlockGold(int iNewValue);
	int GetBlockGold() const;
#endif
#if defined(MOD_BALANCE_CORE)
	void SetPurchased(BuildingClassTypes eBuildingClass, bool bValue);
	bool IsPurchased(BuildingClassTypes eBuildingClass);

	void SetBestForWonder(BuildingClassTypes eBuildingClass, bool bValue);
	bool IsBestForWonder(BuildingClassTypes eBuildingClass);
#endif
	// END Base Yield

	int GetYieldPerPopTimes100(YieldTypes eIndex) const;
	void ChangeYieldPerPopTimes100(YieldTypes eIndex, int iChange);

	int GetYieldPerReligionTimes100(YieldTypes eIndex) const;
	void ChangeYieldPerReligionTimes100(YieldTypes eIndex, int iChange);

	int getYieldRateModifier(YieldTypes eIndex) const;
	void changeYieldRateModifier(YieldTypes eIndex, int iChange);

#if defined(MOD_BALANCE_CORE_POLICIES)
	int getReligionBuildingYieldRateModifier(BuildingClassTypes eIndex1, YieldTypes eIndex2)	const;
	void changeReligionBuildingYieldRateModifier(BuildingClassTypes eIndex1, YieldTypes eIndex2, int iChange);

	int getLocalBuildingClassYield(BuildingClassTypes eIndex1, YieldTypes eIndex2)	const;
	void changeLocalBuildingClassYield(BuildingClassTypes eIndex1, YieldTypes eIndex2, int iChange);
#endif

	int getPowerYieldRateModifier(YieldTypes eIndex) const;
	void changePowerYieldRateModifier(YieldTypes eIndex, int iChange);

	int getResourceYieldRateModifier(YieldTypes eIndex) const;
	void changeResourceYieldRateModifier(YieldTypes eIndex, int iChange);

	int getHappinessModifier(YieldTypes eIndex) const;

	int getExtraSpecialistYield(YieldTypes eIndex) const;
	int getExtraSpecialistYield(YieldTypes eIndex, SpecialistTypes eSpecialist) const;
	void updateExtraSpecialistYield(YieldTypes eYield);
	void updateExtraSpecialistYield();

	int getProductionToYieldModifier(YieldTypes eIndex) const;
	void changeProductionToYieldModifier(YieldTypes eIndex, int iChange);

	int GetTradeYieldModifier(YieldTypes eIndex, CvString* toolTipSink = NULL) const;

	// END Yield

	int getDomainFreeExperience(DomainTypes eIndex) const;
	void changeDomainFreeExperience(DomainTypes eIndex, int iChange);

	int getDomainFreeExperienceFromGreatWorks(DomainTypes eIndex) const;
#if defined(MOD_BALANCE_CORE)
	int getDomainFreeExperienceFromGreatWorksGlobal(DomainTypes eIndex) const;
#endif
	
	int getDomainProductionModifier(DomainTypes eIndex) const;
	void changeDomainProductionModifier(DomainTypes eIndex, int iChange);

	bool isEverOwned(PlayerTypes eIndex) const;
	void setEverOwned(PlayerTypes eIndex, bool bNewValue);

	bool isRevealed(TeamTypes eIndex, bool bDebug) const;
	bool setRevealed(TeamTypes eIndex, bool bNewValue);

	const CvString getName() const;
	const char* getNameKey() const;
	void setName(const char* szNewValue, bool bFound = false);
	void doFoundMessage();

	bool IsExtraLuxuryResources();
	void SetExtraLuxuryResources(int iNewValue);
	void ChangeExtraLuxuryResources(int iChange);

	CvCityBuildings* GetCityBuildings() const;

	int getProjectProduction(ProjectTypes eIndex) const;
	void setProjectProduction(ProjectTypes eIndex, int iNewValue);
	void changeProjectProduction(ProjectTypes eIndex, int iChange);
	int getProjectProductionTimes100(ProjectTypes eIndex) const;
	void setProjectProductionTimes100(ProjectTypes eIndex, int iNewValue);
	void changeProjectProductionTimes100(ProjectTypes eIndex, int iChange);

	int getSpecialistProduction(SpecialistTypes eIndex) const;
	void setSpecialistProduction(SpecialistTypes eIndex, int iNewValue);
	void changeSpecialistProduction(SpecialistTypes eIndex, int iChange);
	int getSpecialistProductionTimes100(SpecialistTypes eIndex) const;
	void setSpecialistProductionTimes100(SpecialistTypes eIndex, int iNewValue);
	void changeSpecialistProductionTimes100(SpecialistTypes eIndex, int iChange);

#if defined(MOD_PROCESS_STOCKPILE)
	int getProcessProduction(ProcessTypes eIndex) const;
	int getProcessProductionTimes100(ProcessTypes eIndex) const;
#endif

	int getUnitProduction(UnitTypes eIndex) const;
	void setUnitProduction(UnitTypes eIndex, int iNewValue);
	void changeUnitProduction(UnitTypes eIndex, int iChange);
	int getUnitProductionTimes100(UnitTypes eIndex) const;
	void setUnitProductionTimes100(UnitTypes eIndex, int iNewValue);
	void changeUnitProductionTimes100(UnitTypes eIndex, int iChange);

	int getUnitProductionTime(UnitTypes eIndex) const;
	void setUnitProductionTime(UnitTypes eIndex, int iNewValue);
	void changeUnitProductionTime(UnitTypes eIndex, int iChange);

	int getUnitCombatFreeExperience(UnitCombatTypes eIndex) const;
	void changeUnitCombatFreeExperience(UnitCombatTypes eIndex, int iChange);

	int getUnitCombatProductionModifier(UnitCombatTypes eIndex) const;
	void changeUnitCombatProductionModifier(UnitCombatTypes eIndex, int iChange);

	int getFreePromotionCount(PromotionTypes eIndex) const;
	bool isFreePromotion(PromotionTypes eIndex) const;
	void changeFreePromotionCount(PromotionTypes eIndex, int iChange);

	int getSpecialistFreeExperience() const;
	void changeSpecialistFreeExperience(int iChange);

	void updateStrengthValue();
	int getStrengthValue(bool bForRangeStrike = false) const;
	int GetPower() const;

	int getDamage() const;
	void setDamage(int iValue, bool noMessage=false);
	void changeDamage(int iChange);

	bool isMadeAttack() const;
	void setMadeAttack(bool bNewValue);

#if defined(MOD_EVENTS_CITY_BOMBARD)
	int getBombardRange(bool& bIndirectFireAllowed) const;
	int getBombardRange() const;
#endif

	void ChangeNumTimesAttackedThisTurn(PlayerTypes ePlayer, int iValue);
	int GetNumTimesAttackedThisTurn(PlayerTypes ePlayer) const;

	bool canRangeStrike() const;
	bool CanRangeStrikeNow() const;
	bool IsHasBuildingThatAllowsRangeStrike() const;

	bool canRangeStrikeAt(int iX, int iY) const;
	CityTaskResult rangeStrike(int iX, int iY);
	CvUnit* rangedStrikeTarget(const CvPlot* pPlot) const;
	bool canRangedStrikeTarget(const CvPlot& targetPlot) const;

	int rangeCombatUnitDefense(_In_ const CvUnit* pDefender, const CvPlot* pInPlot = NULL) const;
	int rangeCombatDamage(const CvUnit* pDefender, CvCity* pCity = NULL, bool bIncludeRand = true, const CvPlot* pInPlot = NULL) const;

	int GetAirStrikeDefenseDamage(const CvUnit* pAttacker, bool bIncludeRand = true) const;

	void DoNearbyEnemy();

#if defined(MOD_BALANCE_CORE_DEALS)
	bool IsInDanger(PlayerTypes eEnemy) const;
#endif

	void IncrementUnitStatCount(CvUnit* pUnit);
	void CheckForAchievementBuilding(BuildingTypes eBuilding);
	bool AreAllUnitsBuilt();

	// Plot acquisition

	bool CanBuyPlot(int iPlotX, int iPlotY, bool bIgnoreCost = false);
	bool CanBuyAnyPlot(void);
	CvPlot* GetNextBuyablePlot(bool bForPurchase);
	void GetBuyablePlotList(std::vector<int>& aiPlotList, bool bForPurchase, int nChoices=3);
	int GetBuyPlotCost(int iPlotX, int iPlotY) const;
	void BuyPlot(int iPlotX, int iPlotY);
	void DoAcquirePlot(int iPlotX, int iPlotY);
	int GetBuyPlotScore(int& iBestX, int& iBestY);
	int GetIndividualPlotScore(const CvPlot* pPlot) const;

	int GetCheapestPlotInfluenceDistance() const;
	void SetCheapestPlotInfluenceDistance(int iValue);
	void DoUpdateCheapestPlotInfluenceDistance();

	// End plot acquisition

	bool isValidBuildingLocation(BuildingTypes eIndex) const;

	void SetThreatValue(int iThreatValue);
	int getThreatValue(void);

	void clearOrderQueue();
	void pushOrder(OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend, bool bRush=false);
	void popOrder(int iNum, bool bFinish = false, bool bChoose = false);
	void swapOrder(int iNum);
	void startHeadOrder();
	void stopHeadOrder();
	int getOrderQueueLength();
	OrderData* getOrderFromQueue(int iIndex);
	const OrderData* nextOrderQueueNode(const OrderData* pNode) const;
	OrderData* nextOrderQueueNode(OrderData* pNode);
	const OrderData* headOrderQueueNode() const;
	OrderData* headOrderQueueNode();
	const OrderData* tailOrderQueueNode() const;
	bool CleanUpQueue(void);  // remove items in the queue that are no longer valid

	int CreateUnit(UnitTypes eUnitType, UnitAITypes eAIType = NO_UNITAI, bool bUseToSatisfyOperation=true, bool bIsPurchase = false);
	bool CreateBuilding(BuildingTypes eBuildType);
	bool CreateProject(ProjectTypes eProjectType);

	CvPlot* GetPlotForNewUnit(UnitTypes eUnitType) const;
	bool CanPlaceUnitHere(UnitTypes eUnitType) const;
	bool IsCanPurchase(bool bTestPurchaseCost, bool bTestTrainable, UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectType, YieldTypes ePurchaseYield);
	void Purchase(UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectType, YieldTypes ePurchaseYield);

	PlayerTypes getLiberationPlayer() const;
	void liberate();

	CvCityStrategyAI* GetCityStrategyAI() const;
	CvCityCitizens* GetCityCitizens() const;
	CvCityReligions* GetCityReligions() const;
	CvCityEmphases* GetCityEmphases() const;
	CvCityEspionage* GetCityEspionage() const;
	CvCityCulture* GetCityCulture() const;

	void read(FDataStream& kStream);
	void write(FDataStream& kStream) const;

	virtual void AI_init() = 0;
	virtual void AI_reset() = 0;
	virtual void AI_doTurn() = 0;
#if defined(MOD_BALANCE_CORE)
	virtual void AI_chooseProduction(bool bInterruptWonders, bool bInterruptBuildings) = 0;
#else
	virtual void AI_chooseProduction(bool bInterruptWonders) = 0;
#endif
	virtual bool AI_isChooseProductionDirty() = 0;
	virtual void AI_setChooseProductionDirty(bool bNewValue) = 0;

	virtual int AI_GetNumPlotsAcquiredByOtherPlayer(PlayerTypes ePlayer) const = 0;
	virtual void AI_ChangeNumPlotsAcquiredByOtherPlayer(PlayerTypes ePlayer, int iChange) = 0;

	void invalidatePopulationRankCache();
	void invalidateYieldRankCache(YieldTypes eYield = NO_YIELD);

	UnitTypes GetUnitForOperation();
	virtual bool IsBuildingUnitForOperation() const
	{
		return m_unitBeingBuiltForOperation.IsValid();
	}

	const char* GetCityBombardEffectTag() const;
	uint GetCityBombardEffectTagHash() const;

	int GetExtraHitPoints() const;
	void ChangeExtraHitPoints(int iValue);

	int GetMaxHitPoints() const;
	const FAutoArchive& getSyncArchive() const;
	FAutoArchive& getSyncArchive();
	std::string debugDump(const FAutoVariableBase&) const;
	std::string stackTraceRemark(const FAutoVariableBase&) const;

	bool			IsBusy() const;
	// Combat related
	const CvUnit*	getCombatUnit() const;
	CvUnit*			getCombatUnit();
	void			setCombatUnit(CvUnit* pUnit, bool bAttacking = false);
	void			clearCombat();
	bool			isFighting() const;

#if defined(MOD_API_EXTENSIONS)
	bool HasBelief(BeliefTypes iBeliefType) const;
	bool HasBuilding(BuildingTypes iBuildingType) const;
	bool HasBuildingClass(BuildingClassTypes iBuildingClassType) const;
	bool HasAnyWonder() const;
	bool HasWonder(BuildingTypes iBuildingType) const;
	bool IsCivilization(CivilizationTypes iCivilizationType) const;
	bool HasFeature(FeatureTypes iFeatureType) const;
	bool HasWorkedFeature(FeatureTypes iFeatureType) const;
	bool HasAnyNaturalWonder() const;
	bool HasNaturalWonder(FeatureTypes iFeatureType) const;
	bool HasImprovement(ImprovementTypes iImprovementType) const;
	bool HasWorkedImprovement(ImprovementTypes iImprovementType) const;
	bool HasPlotType(PlotTypes iPlotType) const;
	bool HasWorkedPlotType(PlotTypes iPlotType) const;
	bool HasAnyReligion() const;
	bool HasReligion(ReligionTypes iReligionType) const;
	bool HasResource(ResourceTypes iResourceType) const;
	bool HasWorkedResource(ResourceTypes iResourceType) const;
	//just an alias for lua
	bool IsConnectedToCapital() const { return IsRouteToCapitalConnected(); }
	bool IsConnectedTo(CvCity* pCity) const;
	bool HasSpecialistSlot(SpecialistTypes iSpecialistType) const;
	bool HasSpecialist(SpecialistTypes iSpecialistType) const;
	bool HasTerrain(TerrainTypes iTerrainType) const;
	bool HasWorkedTerrain(TerrainTypes iTerrainType) const;
	bool HasAnyDomesticTradeRoute(bool bOutgoing = true) const;
	bool HasAnyInternationalTradeRoute(bool bOutgoing = true) const;
	bool HasTradeRouteToAnyCity() const;
	bool HasTradeRouteTo(CvCity* pCity) const;
	bool HasTradeRouteFromAnyCity() const;
	bool HasTradeRouteFrom(CvCity* pCity) const;
	bool IsOnFeature(FeatureTypes iFeatureType) const;
	bool IsAdjacentToFeature(FeatureTypes iFeatureType) const;
	bool IsWithinDistanceOfFeature(FeatureTypes iFeatureType, int iDistance) const;
#if defined(MOD_BALANCE_CORE)
	bool IsWithinDistanceOfUnit(UnitTypes eOtherUnit, int iDistance, bool bIsFriendly, bool bIsEnemy) const;
	bool IsWithinDistanceOfUnitClass(UnitClassTypes eUnitClass, int iDistance, bool bIsFriendly, bool bIsEnemy) const;
	bool IsWithinDistanceOfUnitCombatType(UnitCombatTypes eUnitCombat, int iDistance, bool bIsFriendly, bool bIsEnemy) const;
	bool IsWithinDistanceOfUnitPromotion(PromotionTypes eUnitPromotion, int iDistance, bool bIsFriendly, bool bIsEnemy) const;
#endif
	bool IsOnImprovement(ImprovementTypes iImprovementType) const;
	bool IsAdjacentToImprovement(ImprovementTypes iImprovementType) const;
	bool IsWithinDistanceOfImprovement(ImprovementTypes iImprovementType, int iDistance) const;
	bool IsOnPlotType(PlotTypes iPlotType) const;
	bool IsAdjacentToPlotType(PlotTypes iPlotType) const;
	bool IsWithinDistanceOfPlotType(PlotTypes iPlotType, int iDistance) const;
	bool IsOnResource(ResourceTypes iResourceType) const;
	bool IsAdjacentToResource(ResourceTypes iResourceType) const;
	bool IsWithinDistanceOfResource(ResourceTypes iResourceType, int iDistance) const;
	bool IsOnTerrain(TerrainTypes iTerrainType) const;
	bool IsAdjacentToTerrain(TerrainTypes iTerrainType) const;
	bool IsWithinDistanceOfTerrain(TerrainTypes iTerrainType, int iDistance) const;
	int CountNumWorkedFeature(FeatureTypes eFeature);
	int CountNumWorkedImprovement(ImprovementTypes eImprovement, bool IgnorePillaged = true);
	int CountNumWorkedResource(ResourceTypes eResource);
	int CountNumImprovement(ImprovementTypes eImprovement);
	int CountNumWorkedRiverTiles(TerrainTypes eTerrain = NO_TERRAIN);
	int CountFeature(FeatureTypes iFeatureType) const;
	int CountWorkedFeature(FeatureTypes iFeatureType) const;
	int CountImprovement(ImprovementTypes iImprovementType) const;
	int CountWorkedImprovement(ImprovementTypes iImprovementType) const;
	int CountPlotType(PlotTypes iPlotType) const;
	int CountWorkedPlotType(PlotTypes iPlotType) const;
	int CountResource(ResourceTypes iResourceType) const;
	int CountWorkedResource(ResourceTypes iResourceType) const;
	int CountTerrain(TerrainTypes iTerrainType) const;
	int CountWorkedTerrain(TerrainTypes iTerrainType) const;
#endif

#if defined(MOD_CORE_PER_TURN_DAMAGE)
	int addDamageReceivedThisTurn(int iDamage);
	void flipDamageReceivedPerTurn();
	bool isInDangerOfFalling() const;
	bool isUnderSiege() const;
#endif

#if defined(MOD_BALANCE_CORE)
	//the closest friendly cities - up to 6 entries 
	const std::vector<int>& GetClosestFriendlyNeighboringCities() const;
	void UpdateClosestFriendlyNeighbors();

	//temporary mapping from city to related units. not serialized!
	void AttachUnit(CvUnit* pUnit);
	void ClearAttachedUnits();
	const std::vector<int>& GetAttachedUnits() const;
#endif

#if defined(MOD_BALANCE_CORE_JFD)
	bool IsColony() const;
	void SetColony(bool bValue);

	int GetProvinceLevel() const;
	void SetProvinceLevel(int iValue);

	int GetOrganizedCrime() const;
	void SetOrganizedCrime(int iValue);
	bool HasOrganizedCrime();

	void ChangeResistanceCounter(int iValue);
	void SetResistanceCounter(int iValue);
	int GetResistanceCounter() const;

	void ChangePlagueCounter(int iValue);
	void SetPlagueCounter(int iValue);
	int GetPlagueCounter() const;

	int GetPlagueTurns() const;
	void ChangePlagueTurns(int iValue); //Set in city::doturn
	void SetPlagueTurns(int iValue);

	int GetPlagueType() const;
	void SetPlagueType(int iValue);
	bool HasPlague();

	void ChangeLoyaltyCounter(int iValue);
	void SetLoyaltyCounter(int iValue);
	int GetLoyaltyCounter() const;

	void ChangeDisloyaltyCounter(int iValue);
	void SetDisloyaltyCounter(int iValue);
	int GetDisloyaltyCounter() const;

	int GetLoyaltyState() const;
	void SetLoyaltyState(int iLoyalty);

	void SetYieldModifierFromHappiness(YieldTypes eYield, int iValue);
	int GetYieldModifierFromHappiness(YieldTypes eYield) const;

	void SetYieldModifierFromHealth(YieldTypes eYield, int iValue);
	int GetYieldModifierFromHealth(YieldTypes eYield) const;

	void SetYieldModifierFromCrime(YieldTypes eYield, int iValue);
	int GetYieldModifierFromCrime(YieldTypes eYield) const;

	void SetYieldModifierFromDevelopment(YieldTypes eYield, int iValue);
	int GetYieldModifierFromDevelopment(YieldTypes eYield) const;

	void SetYieldFromHappiness(YieldTypes eYield, int iValue);
	int GetYieldFromHappiness(YieldTypes eYield) const;

	void SetYieldFromHealth(YieldTypes eYield, int iValue);
	int GetYieldFromHealth(YieldTypes eYield) const;

	void SetYieldFromCrime(YieldTypes eYield, int iValue);
	int GetYieldFromCrime(YieldTypes eYield) const;

	void SetYieldFromDevelopment(YieldTypes eYield, int iValue);
	int GetYieldFromDevelopment(YieldTypes eYield) const;
#endif

	int iScratch; // know the scope of your validity

protected:
	FAutoArchiveClassContainer<CvCity> m_syncArchive;

	FAutoVariable<CvString, CvCity> m_strNameIAmNotSupposedToBeUsedAnyMoreBecauseThisShouldNotBeCheckedAndWeNeedToPreserveSaveGameCompatibility;
	FAutoVariable<PlayerTypes, CvCity> m_eOwner;
	FAutoVariable<int, CvCity> m_iX;
	FAutoVariable<int, CvCity> m_iY;
	FAutoVariable<int, CvCity> m_iID;

	FAutoVariable<int, CvCity> m_iRallyX;
	FAutoVariable<int, CvCity> m_iRallyY;
	FAutoVariable<int, CvCity> m_iGameTurnFounded;
	FAutoVariable<int, CvCity> m_iGameTurnAcquired;
	FAutoVariable<int, CvCity> m_iGameTurnLastExpanded;
	FAutoVariable<int, CvCity> m_iPopulation;
	FAutoVariable<int, CvCity> m_iHighestPopulation;
	FAutoVariable<int, CvCity> m_iExtraHitPoints;

	FAutoVariable<int, CvCity> m_iNumGreatPeople;
	FAutoVariable<int, CvCity> m_iBaseGreatPeopleRate;
	FAutoVariable<int, CvCity> m_iGreatPeopleRateModifier;
	FAutoVariable<int, CvCity> m_iJONSCultureStored;
	FAutoVariable<int, CvCity> m_iJONSCultureLevel;
#if !defined(MOD_API_UNIFIED_YIELDS_CONSOLIDATION)
	FAutoVariable<int, CvCity> m_iJONSCulturePerTurnFromBuildings;
#endif
	FAutoVariable<int, CvCity> m_iJONSCulturePerTurnFromPolicies;
	FAutoVariable<int, CvCity> m_iJONSCulturePerTurnFromSpecialists;
#if !defined(MOD_API_UNIFIED_YIELDS_CONSOLIDATION)
	FAutoVariable<int, CvCity> m_iJONSCulturePerTurnFromReligion;
#endif
#if !defined(MOD_API_UNIFIED_YIELDS_CONSOLIDATION)
	FAutoVariable<int, CvCity> m_iFaithPerTurnFromBuildings;
#endif
	FAutoVariable<int, CvCity> m_iFaithPerTurnFromPolicies;
#if !defined(MOD_API_UNIFIED_YIELDS_CONSOLIDATION)
	FAutoVariable<int, CvCity> m_iFaithPerTurnFromReligion;
#endif
#if defined(MOD_BALANCE_CORE)
	FAutoVariable<int, CvCity> m_iAdditionalFood;
#endif
	FAutoVariable<int, CvCity> m_iCultureRateModifier;
	FAutoVariable<int, CvCity> m_iNumWorldWonders;
	FAutoVariable<int, CvCity> m_iNumTeamWonders;
	FAutoVariable<int, CvCity> m_iNumNationalWonders;
	FAutoVariable<int, CvCity> m_iWonderProductionModifier;
	FAutoVariable<int, CvCity> m_iCapturePlunderModifier;
	FAutoVariable<int, CvCity> m_iPlotCultureCostModifier;
	FAutoVariable<int, CvCity> m_iPlotBuyCostModifier;
#if defined(MOD_BUILDINGS_CITY_WORKING)
	FAutoVariable<int, CvCity> m_iCityWorkingChange;
	FAutoVariable<int, CvCity> m_iCitySupplyModifier;
	FAutoVariable<int, CvCity> m_iCitySupplyFlat;
	FAutoVariable<bool, CvCity> m_bAllowsProductionTradeRoutes;
	FAutoVariable<bool, CvCity> m_bAllowsFoodTradeRoutes;
	FAutoVariable<bool, CvCity> m_bAllowPuppetPurchase;
#endif
	FAutoVariable<int, CvCity> m_iMaintenance;
	FAutoVariable<int, CvCity> m_iHealRate;
	FAutoVariable<int, CvCity> m_iNoOccupiedUnhappinessCount;
#if defined(HH_MOD_BUILDINGS_FRUITLESS_PILLAGE)
	FAutoVariable<int, CvCity> m_iLocalGainlessPillageCount;
#endif
	FAutoVariable<int, CvCity> m_iFood;
	FAutoVariable<int, CvCity> m_iFoodKept;
	FAutoVariable<int, CvCity> m_iMaxFoodKeptPercent;
	FAutoVariable<int, CvCity> m_iOverflowProduction;
	FAutoVariable<int, CvCity> m_iFeatureProduction;
	FAutoVariable<int, CvCity> m_iMilitaryProductionModifier;
	FAutoVariable<int, CvCity> m_iSpaceProductionModifier;
	FAutoVariable<int, CvCity> m_iFreeExperience;
	FAutoVariable<int, CvCity> m_iCurrAirlift; // unused
	FAutoVariable<int, CvCity> m_iMaxAirUnits;
	FAutoVariable<int, CvCity> m_iAirModifier; // unused
	FAutoVariable<int, CvCity> m_iNukeModifier;
	FAutoVariable<int, CvCity> m_iTradeRouteTargetBonus;
	FAutoVariable<int, CvCity> m_iTradeRouteRecipientBonus;
	FAutoVariable<int, CvCity> m_iTradeRouteSeaGoldBonus;
	FAutoVariable<int, CvCity> m_iTradeRouteLandGoldBonus;
	FAutoVariable<int, CvCity> m_iNumTradeRouteBonus;
	FAutoVariable<int, CvCity> m_iCityConnectionTradeRouteGoldModifier;
	FAutoVariable<int, CvCity> m_iCultureUpdateTimer;
	FAutoVariable<int, CvCity> m_iCitySizeBoost;
	FAutoVariable<int, CvCity> m_iSpecialistFreeExperience;
	FAutoVariable<int, CvCity> m_iStrengthValue;
	FAutoVariable<int, CvCity> m_iDamage;
	FAutoVariable<int, CvCity> m_iThreatValue;
	FAutoVariable<int, CvCity> m_hGarrison;  // unused
	int m_hGarrisonOverride; //only temporary, not serialized
	FAutoVariable<int, CvCity> m_iResourceDemanded;
	FAutoVariable<int, CvCity> m_iWeLoveTheKingDayCounter;
	FAutoVariable<int, CvCity> m_iLastTurnGarrisonAssigned;
	FAutoVariable<int, CvCity> m_iThingsProduced; // total number of units, buildings, wonders, etc. this city has constructed
	FAutoVariable<int, CvCity> m_iDemandResourceCounter;
	FAutoVariable<int, CvCity> m_iResistanceTurns;
	FAutoVariable<int, CvCity> m_iRazingTurns;
	FAutoVariable<int, CvCity> m_iCountExtraLuxuries;
	FAutoVariable<int, CvCity> m_iCheapestPlotInfluenceDistance;
	FAutoVariable<int, CvCity> m_iEspionageModifier;
#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
	FAutoVariable<int, CvCity> m_iConversionModifier;
#endif

	OperationSlot m_unitBeingBuiltForOperation;

	FAutoVariable<bool, CvCity> m_bNeverLost;
	FAutoVariable<bool, CvCity> m_bDrafted;
	FAutoVariable<bool, CvCity> m_bAirliftTargeted;   // unused
	FAutoVariable<bool, CvCity> m_bProductionAutomated;
	FAutoVariable<bool, CvCity> m_bLayoutDirty;
	FAutoVariable<bool, CvCity> m_bMadeAttack;
	FAutoVariable<bool, CvCity> m_bOccupied;
	FAutoVariable<bool, CvCity> m_bPuppet;
	FAutoVariable<bool, CvCity> m_bIgnoreCityForHappiness;
	FAutoVariable<bool, CvCity> m_bIndustrialRouteToCapital;
	FAutoVariable<bool, CvCity> m_bFeatureSurrounded;

	FAutoVariable<PlayerTypes, CvCity> m_ePreviousOwner;
	FAutoVariable<PlayerTypes, CvCity> m_eOriginalOwner;
	FAutoVariable<PlayerTypes, CvCity> m_ePlayersReligion;

	FAutoVariable<std::vector<int>, CvCity> m_aiSeaPlotYield;
	FAutoVariable<std::vector<int>, CvCity> m_aiRiverPlotYield;
	FAutoVariable<std::vector<int>, CvCity> m_aiLakePlotYield;
	FAutoVariable<std::vector<int>, CvCity> m_aiSeaResourceYield;
	FAutoVariable<std::vector<int>, CvCity> m_aiBaseYieldRateFromTerrain;
	FAutoVariable<std::vector<int>, CvCity> m_aiBaseYieldRateFromBuildings;
	FAutoVariable<std::vector<int>, CvCity> m_aiBaseYieldRateFromSpecialists;
	FAutoVariable<std::vector<int>, CvCity> m_aiBaseYieldRateFromMisc;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	FAutoVariable<std::vector<int>, CvCity> m_aiBaseYieldRateFromLeague;
	FAutoVariable<int, CvCity> m_iTotalScienceyAid;
	FAutoVariable<int, CvCity> m_iTotalArtsyAid;
	FAutoVariable<int, CvCity> m_iTotalGreatWorkAid;
#endif
#if defined(MOD_DIPLOMACY_CITYSTATES) || defined(MOD_BALANCE_CORE)
	FAutoVariable<std::vector<int>, CvCity> m_aiChangeGrowthExtraYield;
#endif
#if defined(MOD_BALANCE_CORE)
	FAutoVariable<std::vector<int>, CvCity> m_aiLongestPotentialTradeRoute;
	FAutoVariable<std::vector<int>, CvCity> m_aiNumTimesAttackedThisTurn;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldFromKnownPantheons;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldFromVictory;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldFromPillage;
	FAutoVariable<std::vector<int>, CvCity> m_aiGoldenAgeYieldMod;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldFromWLTKD;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldFromConstruction;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldFromTech;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldFromBirth;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldFromUnitProduction;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldFromBorderGrowth;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldFromPolicyUnlock;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldFromPurchase;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldFromUnitLevelUp;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldPerAlly;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldPerFriend;
	FAutoVariable<std::vector<int>, CvCity> m_aiScienceFromYield;
	FAutoVariable<std::vector<int>, CvCity> m_aiBuildingScienceFromYield;
	FAutoVariable<std::vector<int>, CvCity> m_aiSpecialistRateModifier;
	FAutoVariable<std::vector<int>, CvCity> m_aiThemingYieldBonus;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldFromSpyAttack;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldFromSpyDefense;
	FAutoVariable<std::vector<int>, CvCity> m_aiNumTimesOwned;
	FAutoVariable<std::vector<int>, CvCity> m_aiStaticCityYield;
	FAutoVariable<int, CvCity> m_iThreatRank;
	FAutoVariable<int, CvCity> m_iCoastalThreatRank;
	FAutoVariable<int, CvCity> m_iTradePriorityLand;
	FAutoVariable<int, CvCity> m_iTradePrioritySea;
	FAutoVariable<int, CvCity> m_iDummy;
	FAutoVariable<int, CvCity> m_iUnitPurchaseCooldown;
	FAutoVariable<int, CvCity> m_iUnitPurchaseCooldownCivilian;
	FAutoVariable<int, CvCity> m_iBuildingPurchaseCooldown;
	FAutoVariable<int, CvCity> m_iReligiousTradeModifier;
	FAutoVariable<int, CvCity> m_iCityAirStrikeDefense;
	FAutoVariable<int, CvCity> m_iFreeBuildingTradeTargetCity;
	FAutoVariable<int, CvCity> m_iBaseTourism;
	FAutoVariable<int, CvCity> m_iBaseTourismBeforeModifiers;
	FAutoVariable<int, CvCity> m_iBorderObstacleCity;
	FAutoVariable<int, CvCity> m_iBorderObstacleWater;
	FAutoVariable<int, CvCity> m_iNumNearbyMountains;
	FAutoVariable<int, CvCity> m_iLocalUnhappinessMod;
	FAutoVariable<bool, CvCity> m_bNoWarmonger;
	FAutoVariable<int, CvCity> m_iBlockBuildingDestruction;
	FAutoVariable<int, CvCity> m_iBlockWWDestruction;
	FAutoVariable<int, CvCity> m_iBlockUDestruction;
	FAutoVariable<int, CvCity> m_iBlockGPDestruction;
	FAutoVariable<int, CvCity> m_iBlockRebellion;
	FAutoVariable<int, CvCity> m_iBlockUnrest;
	FAutoVariable<int, CvCity> m_iBlockScience;
	FAutoVariable<int, CvCity> m_iBlockGold;
	FAutoVariable<int, CvCity> m_iCityRank;
	FAutoVariable<int, CvCity> m_iTurnsSinceRankAnnouncement;
	FAutoVariable<int, CvCity> m_iChangePovertyUnhappiness;
	FAutoVariable<int, CvCity> m_iChangeDefenseUnhappiness;
	FAutoVariable<int, CvCity> m_iChangeUnculturedUnhappiness;
	FAutoVariable<int, CvCity> m_iChangeIlliteracyUnhappiness;
	FAutoVariable<int, CvCity> m_iChangeMinorityUnhappiness;
	FAutoVariable<int, CvCity> m_iTradeRouteSeaDistanceModifier;
	FAutoVariable<int, CvCity> m_iTradeRouteLandDistanceModifier;
	FAutoVariable<std::vector<int>, CvCity> m_aiEconomicValue;
#endif
	FAutoVariable<std::vector<int>, CvCity> m_aiBaseYieldRateFromReligion;
#if defined(MOD_BALANCE_CORE)
	FAutoVariable<std::vector<int>, CvCity> m_aiBaseYieldRateFromCSAlliance;
	FAutoVariable<std::vector<int>, CvCity> m_aiBaseYieldRateFromCSFriendship;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldFromMinors;
	FAutoVariable<std::vector<int>, CvCity> m_aiResourceQuantityPerXFranchises;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldChangeFromCorporationFranchises;
	FAutoVariable<int, CvCity> m_iLandTourismBonus;
	FAutoVariable<int, CvCity> m_iSeaTourismBonus;
	FAutoVariable<int, CvCity> m_iAlwaysHeal;
	FAutoVariable<int, CvCity> m_iResourceDiversityModifier;
	FAutoVariable<bool, CvCity> m_bIsBastion;
#endif
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldRateModifier;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldPerPop;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldPerReligion;
	FAutoVariable<std::vector<int>, CvCity> m_aiPowerYieldRateModifier;
	FAutoVariable<std::vector<int>, CvCity> m_aiResourceYieldRateModifier;
	FAutoVariable<std::vector<int>, CvCity> m_aiExtraSpecialistYield;
	FAutoVariable<std::vector<int>, CvCity> m_aiProductionToYieldModifier;
	FAutoVariable<std::vector<int>, CvCity> m_aiDomainFreeExperience;
	FAutoVariable<std::vector<int>, CvCity> m_aiDomainProductionModifier;

	FAutoVariable<std::vector<bool>, CvCity> m_abEverOwned;
#if defined(MOD_BALANCE_CORE)
	FAutoVariable<std::vector<bool>, CvCity> m_abIsBestForWonder;
	FAutoVariable<std::vector<bool>, CvCity> m_abIsPurchased;
	FAutoVariable<std::vector<bool>, CvCity> m_abTraded;
	FAutoVariable<std::vector<bool>, CvCity> m_abPaidAdoptionBonus;
	FAutoVariable<std::vector<int>, CvCity> m_aiReligiousPressureModifier;
	FAutoVariable<int, CvCity> m_iExtraBuildingMaintenance;
	FAutoVariable<std::vector<int>, CvCity> m_paiNumTerrainWorked;
	FAutoVariable<std::vector<int>, CvCity> m_paiNumFeaturelessTerrainWorked;
	FAutoVariable<std::vector<int>, CvCity> m_paiNumFeatureWorked;
	FAutoVariable<std::vector<int>, CvCity> m_paiNumResourceWorked;
	FAutoVariable<std::vector<int>, CvCity> m_paiNumImprovementWorked;
#endif
	FAutoVariable<std::vector<bool>, CvCity> m_abRevealed;

	FAutoVariable<CvString, CvCity> m_strScriptData;

#if defined(MOD_CORE_PER_TURN_DAMAGE)
	FAutoVariable<int, CvCity> m_iDamageTakenThisTurn;
	FAutoVariable<int, CvCity> m_iDamageTakenLastTurn;
#endif

	FAutoVariable<std::vector<int>, CvCity> m_paiNoResource;
	FAutoVariable<std::vector<int>, CvCity> m_paiFreeResource;
	FAutoVariable<std::vector<int>, CvCity> m_paiNumResourcesLocal;
	FAutoVariable<std::vector<int>, CvCity> m_paiProjectProduction;
	FAutoVariable<std::vector<int>, CvCity> m_paiSpecialistProduction;
	FAutoVariable<std::vector<int>, CvCity> m_paiUnitProduction;
	FAutoVariable<std::vector<int>, CvCity> m_paiUnitProductionTime;
	FAutoVariable<std::vector<int>, CvCity> m_paiSpecialistCount;
	FAutoVariable<std::vector<int>, CvCity> m_paiMaxSpecialistCount;
	FAutoVariable<std::vector<int>, CvCity> m_paiForceSpecialistCount;
	FAutoVariable<std::vector<int>, CvCity> m_paiFreeSpecialistCount;
	FAutoVariable<std::vector<int>, CvCity> m_paiImprovementFreeSpecialists;
	FAutoVariable<std::vector<int>, CvCity> m_paiUnitCombatFreeExperience;
	FAutoVariable<std::vector<int>, CvCity> m_paiUnitCombatProductionModifier;
	FAutoVariable<std::vector<int>, CvCity> m_paiFreePromotionCount;
#if defined(MOD_BALANCE_CORE_POLICIES)
	FAutoVariable<std::vector<int>, CvCity> m_paiBuildingClassCulture;
	FAutoVariable<std::vector<int>, CvCity> m_paiHurryModifier;
#endif

#if defined(MOD_BALANCE_CORE)
	FAutoVariable<int, CvCity> m_iComboUnhappiness;
	FAutoVariable<int, CvCity> m_iPillagedPlots;
	FAutoVariable<int, CvCity> m_iGrowthFromTourism;
	FAutoVariable<int, CvCity> m_iBuildingClassHappinessFromReligion;
	FAutoVariable<std::vector<int>, CvCity> m_vClosestNeighbors;
	std::vector<int> m_vAttachedUnits;
#endif

	FAutoVariable<int, CvCity> m_iBaseHappinessFromBuildings;
	FAutoVariable<int, CvCity> m_iUnmoddedHappinessFromBuildings;

	FAutoVariable<bool, CvCity> m_bRouteToCapitalConnectedLastTurn;
	FAutoVariable<bool, CvCity> m_bRouteToCapitalConnectedThisTurn;

	FAutoVariable<CvString, CvCity> m_strName;

	FAutoVariable<bool, CvCity> m_bOwedCultureBuilding;

#if defined(MOD_BUGFIX_FREE_FOOD_BUILDING)
	FAutoVariable<bool, CvCity> m_bOwedFoodBuilding;
#endif

	mutable FFastSmallFixedList< OrderData, 25, true, c_eCiv5GameplayDLL > m_orderQueue;

	int** m_aaiBuildingSpecialistUpgradeProgresses;
	int** m_ppaiResourceYieldChange;
	int** m_ppaiFeatureYieldChange;
#if defined(MOD_BALANCE_CORE)
	int** m_ppaiImprovementYieldChange;
	FAutoVariable< std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > >, CvCity> m_ppaaiSpecialistExtraYield;
#endif
	int** m_ppaiTerrainYieldChange;
#if defined(MOD_API_UNIFIED_YIELDS) && defined(MOD_API_PLOT_YIELDS)
	int** m_ppaiPlotYieldChange;
	int** m_ppaiYieldPerXTerrainFromBuildings;
	int** m_ppaiYieldPerXTerrainFromReligion;
	int** m_ppaiYieldPerXTerrain;
	int** m_ppaiYieldPerXFeature;
	int** m_ppaiYieldPerXUnimprovedFeature;
#endif
#if defined(MOD_API_UNIFIED_YIELDS) && defined(MOD_API_PLOT_YIELDS)
	int** m_ppaiReligionBuildingYieldRateModifier;
	int** m_ppaiLocalBuildingClassYield;
#endif
#if defined(MOD_BALANCE_CORE_EVENTS)
	FAutoVariable<std::vector<int>, CvCity> m_aiEventCooldown;
	FAutoVariable<std::vector<bool>, CvCity> m_abEventActive;
	FAutoVariable<std::vector<bool>, CvCity> m_abEventChoiceActive;
	FAutoVariable<std::vector<bool>, CvCity> m_abEventChoiceFired;
	FAutoVariable<std::vector<bool>, CvCity> m_abEventFired;
	FAutoVariable<std::vector<int>, CvCity> m_aiEventChoiceDuration;
	FAutoVariable<std::vector<int>, CvCity> m_aiEventIncrement;
	FAutoVariable<std::vector<int>, CvCity> m_aiEventCityYield;
	FAutoVariable<int, CvCity> m_iEventHappiness;
	FAutoVariable<int, CvCity> m_iCityEventCooldown;
	int** m_ppaiEventBuildingClassYield;
	int** m_ppaiEventBuildingClassYieldModifier;
	int** m_ppaiEventImprovementYield;
	int** m_ppaiEventResourceYield;
	int** m_ppaiEventSpecialistYield;
	int** m_ppaiEventTerrainYield;
	int** m_ppaiEventFeatureYield;
#endif

#if defined(MOD_BALANCE_CORE_JFD)
	FAutoVariable<bool, CvCity> m_bIsColony;
	FAutoVariable<int, CvCity> m_iProvinceLevel;
	FAutoVariable<int, CvCity> m_iOrganizedCrime;
	FAutoVariable<int, CvCity> m_iResistanceCounter;
	FAutoVariable<int, CvCity> m_iPlagueCounter;
	FAutoVariable<int, CvCity> m_iPlagueTurns;
	FAutoVariable<int, CvCity> m_iPlagueType;
	FAutoVariable<int, CvCity> m_iLoyaltyCounter;
	FAutoVariable<int, CvCity> m_iDisloyaltyCounter;
	FAutoVariable<int, CvCity> m_iLoyaltyStateType;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldModifierFromHappiness;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldModifierFromHealth;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldModifierFromCrime;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldModifierFromDevelopment;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldFromHappiness;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldFromHealth;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldFromCrime;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldFromDevelopment;
#endif

	CvCityBuildings* m_pCityBuildings;
	CvCityStrategyAI* m_pCityStrategyAI;
	CvCityCitizens* m_pCityCitizens;
	CvCityReligions* m_pCityReligions;
	CvCityEmphases* m_pEmphases;
	CvCityEspionage* m_pCityEspionage;
	CvCityCulture* m_pCityCulture;

	mutable int m_bombardCheckTurn;

	// CACHE: cache frequently used values
	FAutoVariable<int, CvCity> m_iPopulationRank;
	FAutoVariable<bool, CvCity> m_bPopulationRankValid;
	FAutoVariable<std::vector<int>, CvCity> m_aiBaseYieldRank;
	FAutoVariable<std::vector<bool>, CvCity> m_abBaseYieldRankValid;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldRank;
	FAutoVariable<std::vector<bool>, CvCity> m_abYieldRankValid;
#if defined(MOD_BALANCE_CORE)
	FAutoVariable<std::vector<bool>, CvCity> m_abOwedChosenBuilding;
	FAutoVariable<std::vector<bool>, CvCity> m_abBuildingInvestment;
	FAutoVariable<std::vector<bool>, CvCity> m_abUnitInvestment;
	FAutoVariable<std::vector<bool>, CvCity> m_abBuildingConstructed;
#endif

	IDInfo m_combatUnit;		// The unit the city is in combat with

	void doGrowth();
	void doProduction(bool bAllowNoProduction);
	void doProcess();
	void doDecay();
	void doGreatPeople();
	void doMeltdown();
	bool doCheckProduction();

	int getExtraProductionDifference(int iExtra, UnitTypes eUnit) const;
	int getExtraProductionDifference(int iExtra, BuildingTypes eBuilding) const;
	int getExtraProductionDifference(int iExtra, ProjectTypes eProject) const;
	int getExtraProductionDifference(int iExtra, int iModifier) const;
	int getHurryCostModifier(HurryTypes eHurry, UnitTypes eUnit, bool bIgnoreNew) const;
	int getHurryCostModifier(HurryTypes eHurry, BuildingTypes eBuilding, bool bIgnoreNew) const;
	int getHurryCostModifier(HurryTypes eHurry, int iBaseModifier, int iProduction, bool bIgnoreNew) const;
	int getHurryCost(HurryTypes eHurry, bool bExtra, UnitTypes eUnit, bool bIgnoreNew) const;
	int getHurryCost(HurryTypes eHurry, bool bExtra, BuildingTypes eBuilding, bool bIgnoreNew) const;
	int getHurryCost(bool bExtra, int iProductionLeft, int iHurryModifier, int iModifier) const;
	int getHurryPopulation(HurryTypes eHurry, int iHurryCost) const;
	int getHurryGold(HurryTypes eHurry, int iHurryCost, int iFullCost) const;
	bool canHurryUnit(HurryTypes eHurry, UnitTypes eUnit, bool bIgnoreNew) const;
	bool canHurryBuilding(HurryTypes eHurry, BuildingTypes eBuilding, bool bIgnoreNew) const;

protected:
	//we can pretend a garrison in this city, but only for limited time
	void OverrideGarrison(const CvUnit* pUnit);
	friend class CvCityGarrisonOverride;
};

namespace FSerialization
{
void SyncCities();
void ClearCityDeltas();
}

//just a guard class so we never forget to unset the garrison override
class CvCityGarrisonOverride
{
public:
	CvCityGarrisonOverride(CvCity* pCity, const CvUnit* pUnit)
	{
		m_pCity = pCity;
		//can only garrison units of the city owner!
		if (m_pCity && pUnit && m_pCity->getOwner()==pUnit->getOwner())
			m_pCity->OverrideGarrison(pUnit);
	}
	~CvCityGarrisonOverride()
	{
		if (m_pCity)
			m_pCity->OverrideGarrison(0);
	}
protected:
	CvCity* m_pCity;
};

#endif
