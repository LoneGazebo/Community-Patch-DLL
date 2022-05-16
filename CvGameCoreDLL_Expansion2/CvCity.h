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
#include "CvSerialize.h"

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

struct SCityExtraYields
{
	vector<pair<TerrainTypes, int>> forTerrain, forXTerrain, forTerrainFromBuildings, forTerrainFromReligion;
	vector<pair<FeatureTypes, int>> forFeature, forXFeature, forFeatureFromBuildings, forFeatureFromReligion, forFeatureUnimproved;
	vector<pair<ImprovementTypes, int>> forImprovement;
	vector<pair<SpecialistTypes, int>> forSpecialist;
	vector<pair<ResourceTypes, int>> forResource;
	vector<pair<PlotTypes, int>> forPlot;
	vector<pair<YieldTypes, int>> forYield, forActualYield;
	vector<pair<BuildingClassTypes, int>> forLocalBuilding, forReligionBuilding;
};

struct SCityEventYields
{
	vector<pair<BuildingClassTypes, int>> forBuilding, forBuildingModifier;
	vector<pair<ImprovementTypes, int>> forImprovement;
	vector<pair<ResourceTypes, int>> forResource;
	vector<pair<SpecialistTypes, int>> forSpecialist;
	vector<pair<TerrainTypes, int>> forTerrain;
	vector<pair<FeatureTypes, int>> forFeature;
};

class CvCity
{

public:
	CvCity();
	virtual ~CvCity();

	enum eUpdateMode {
		YIELD_UPDATE_NONE, //don't do any bookkeeping
		YIELD_UPDATE_LOCAL, //update yields only (plot or city)
		YIELD_UPDATE_GLOBAL //update yields and player happiness
	};

	void init(int iID, PlayerTypes eOwner, int iX, int iY, bool bBumpUnits = true, bool bInitialFounding = true, ReligionTypes eInitialReligion = NO_RELIGION, const char* szName = NULL, CvUnitEntry* pkSettlerUnitEntry = NULL);
	void uninit();
	void reset(int iID = 0, PlayerTypes eOwner = NO_PLAYER, int iX = 0, int iY = 0, bool bConstructorCall = false);
	void setupGraphical();
	void setupWonderGraphics();
	void setupBuildingGraphics();
	void setupSpaceshipGraphics();

	void kill();
	void PreKill();
	void PostKill(bool bCapital, CvPlot* pPlot, int iWorkPlotDistance, PlayerTypes eOwner);

	void doTurn();

	bool isCitySelected();
	void updateYield(bool bRecalcPlotYields = true);
	void ResetGreatWorkYieldCache();
	CvPlayer* GetPlayer() const;

#if defined(MOD_BALANCE_CORE)
	void UpdateAllNonPlotYields(bool bIncludePlayerHappiness);
	void UpdateCityYields(YieldTypes eYield);
	void SetStaticYield(YieldTypes eYield, int iValue);
	int GetStaticYield(YieldTypes eYield) const;

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

	void UpdateGlobalStaticYields();
	void SetGlobalStaticYield(YieldTypes eYield, int iValue);
	int GetGlobalStaticYield(YieldTypes eYield) const;

	void SetStaticNeedAdditives(YieldTypes eYield, int iValue);
	int GetStaticNeedAdditives(YieldTypes eYield) const;

	int GetStaticTechDeviation() const;
	void UpdateStaticTechDeviation(int iValue);

	int GetLuxuryHappinessFromEmpire() const;
	void ChangeHappinessFromLuxuries(int iValue);
	void ResetHappinessFromLuxuries();

	int GetHappinessFromEmpire() const;
	void ChangeHappinessFromEmpire(int iValue);
	void ResetHappinessFromEmpire();

	int GetUnhappinessFromEmpire() const;
	void UpdateUnhappinessFromEmpire();
#endif

#if defined(MOD_BALANCE_CORE_EVENTS)
	void DoEvents(bool bEspionageOnly = false);
	bool IsCityEventValid(CityEventTypes eEvent);
	bool IsCityEventChoiceValid(CityEventChoiceTypes eEventChoice, CityEventTypes eParentEvent, bool bIgnoreActive = false);
	bool IsCityEventChoiceValidEspionage(CityEventChoiceTypes eEventChoice, CityEventTypes eEvent, int uiSpyIndex, PlayerTypes eSpyOwner);
	void DoCancelEventChoice(CityEventChoiceTypes eEventChoice);
	void DoStartEvent(CityEventTypes eEvent);
	void DoEventChoice(CityEventChoiceTypes eEventChoice, CityEventTypes eCityEvent = NO_EVENT_CITY, bool bSendMsg = true, int iEspionageValue = -1, PlayerTypes eSpyOwner = NO_PLAYER, CvCity* pOriginalCity = NULL);
	CvString GetScaledHelpText(CityEventChoiceTypes eEventChoice, bool bYieldsOnly, int iSpyIndex = -1, PlayerTypes eSpyOwner = NO_PLAYER, bool bFromEsp = false);
	CvString GetDisabledTooltip(CityEventChoiceTypes eEventChoice, int iSpyIndex = -1, PlayerTypes eSpyOwner = NO_PLAYER);

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

	void ChangeEventCityYieldModifier(YieldTypes eYield, int iValue);
	int GetEventCityYieldModifier(YieldTypes eYield) const;

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

	int maxXPValue() const;
#endif

	bool IsIndustrialRouteToCapitalConnected() const;
	void SetIndustrialRouteToCapitalConnected(bool bValue);

	void SetRouteToCapitalConnected(bool bValue, bool bIgnoreUpdate = false);
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
	bool IsWithinWorkRange(CvPlot* pPlot) const;

	void clearWorkingOverride(int iIndex);
	int countNumImprovedPlots(ImprovementTypes eImprovement = NO_IMPROVEMENT) const;
	int countNumImprovablePlots(ImprovementTypes eImprovement = NO_IMPROVEMENT, DomainTypes eDomain = DOMAIN_LAND) const;
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
	bool hasBuildingPrerequisites(BuildingTypes eBuilding) const;

	bool canTrain(UnitTypes eUnit, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, bool bWillPurchase = false, CvString* toolTipSink = NULL) const;
	bool canTrain(UnitCombatTypes eUnitCombat) const;
	bool canConstruct(BuildingTypes eBuilding, const std::vector<int>& vPreExistingBuildings, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, bool bWillPurchase = false, CvString* toolTipSink = NULL) const;
	bool canConstruct(BuildingTypes eBuilding, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, bool bWillPurchase = false, CvString* toolTipSink = NULL) const;
	bool canCreate(ProjectTypes eProject, bool bContinue = false, bool bTestVisible = false) const;
	bool canPrepare(SpecialistTypes eSpecialist, bool bContinue = false) const;
	bool canMaintain(ProcessTypes eProcess, bool bContinue = false) const;
	bool canJoinCity() const;

	int GetTerrainImprovementNeed() const;
	void UpdateTerrainImprovementNeed();

	const SCityExtraYields& GetYieldChanges(YieldTypes eYield) const { return m_yieldChanges[eYield]; }

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

	int GetYieldPerXFeatureFromBuildingsTimes100(FeatureTypes eFeature, YieldTypes eYield) const;
	void ChangeYieldPerXFeatureFromBuildingsTimes100(FeatureTypes eFeature, YieldTypes eYield, int iChange);

	void SetYieldPerXFeature(FeatureTypes eFeature, YieldTypes eYield, int iValue);
	void SetYieldPerXFeatureFromReligion(FeatureTypes eFeature, YieldTypes eYield, int iValue);
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

	int GetPlotExtraYield(PlotTypes ePlot, YieldTypes eYield) const;
	void ChangePlotExtraYield(PlotTypes ePlot, YieldTypes eYield, int iChange);

#if defined(MOD_BALANCE_CORE)
	bool IsHasFeatureLocal(FeatureTypes eFeature) const;
#endif
	bool IsHasResourceLocal(ResourceTypes eResource, bool bTestVisible) const;
#if defined(MOD_TRADE_WONDER_RESOURCE_ROUTES)
	int GetNumResourceLocal(ResourceTypes eResource, bool bImproved = false);
#endif
	int GetNumTotalResource(ResourceTypes eResource) const;
	void ChangeNumResourceLocal(ResourceTypes eResource, int iChange, bool bUnimproved = false);

	bool IsBuildingLocalResourceValid(BuildingTypes eBuilding, bool bTestVisible, CvString* toolTipSink = NULL) const;
	bool IsBuildingResourceMonopolyValid(BuildingTypes eBuilding, CvString* toolTipSink = NULL) const;
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

	int getFoodTurnsLeft(int iCorpMod = -1) const;
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
	int getFirstUnitOrder(UnitTypes eUnit) const;
	int getFirstBuildingOrder(BuildingTypes eBuilding) const;
	bool isBuildingInQueue(BuildingTypes eBuilding) const;
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
	int getProductionModifier(UnitTypes eUnit, _In_opt_ CvString* toolTipSink = NULL, bool bIgnoreHappiness = false) const;
	int getProductionModifier(BuildingTypes eBuilding, _In_opt_ CvString* toolTipSink = NULL) const;
	int getProductionModifier(ProjectTypes eProject, _In_opt_ CvString* toolTipSink = NULL) const;
	int getProductionModifier(SpecialistTypes eSpecialist, _In_opt_ CvString* toolTipSink = NULL) const;
	int getProductionModifier(ProcessTypes eProcess, _In_opt_ CvString* toolTipSink = NULL) const;

	int getProductionDifference(int iProductionNeeded, int iProduction, int iProductionModifier, bool bFoodProduction, bool bOverflow) const;
	int getCurrentProductionDifference(bool bIgnoreFood, bool bOverflow) const;
	int getRawProductionDifference(bool bIgnoreFood, bool bOverflow) const;
	int getProductionDifferenceTimes100(int iProductionNeeded, int iProduction, int iProductionModifier, bool bFoodProduction, bool bOverflow) const;
	int getCurrentProductionDifferenceTimes100(bool bIgnoreFood, bool bOverflow) const;
	int getRawProductionDifferenceTimes100(bool bIgnoreFood, bool bOverflow) const;
	int getExtraProductionDifference(int iExtra) const;
	int GetFoodProduction(int iExcessFood) const;
#if defined(MOD_BALANCE_CORE)
	int GetFoodProductionTimes100(int iExcessFoodTimes100) const;
#endif

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
	void processSpecialist(SpecialistTypes eSpecialist, int iChange, eUpdateMode updateMode);

	void UpdateReligion(ReligionTypes eNewMajority, bool bRecalcPlotYields = true);

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

	bool isHumanAutomated() const;

	bool isCapital() const;
	bool IsOriginalCapital() const;
	bool IsOriginalMajorCapital() const; // is the original capital of a major civ
	bool IsOriginalMinorCapital() const;
	bool IsOriginalCapitalForPlayer(PlayerTypes ePlayer) const;

	bool isCoastal(int iMinWaterSize = -1) const;
	bool isAddsFreshWater() const;
#if defined(MOD_BALANCE_CORE)
	int GetUnitPurchaseCooldown(bool bCivilian = false) const;
	void SetUnitPurchaseCooldown(bool bCivilian = false, int iValue = 0);
	void ChangeUnitPurchaseCooldown(bool bCivilian = false, int iValue = 0);
	int GetUnitPurchaseCooldownMod(bool bCivilian = false) const;
	void ChangeUnitPurchaseCooldownMod(bool bCivilian = false, int iValue = 0);
	int GetUnitFaithPurchaseCooldown(bool bCivilian = false) const;
	void SetUnitFaithPurchaseCooldown(bool bCivilian = false, int iValue = 0);
	void ChangeUnitFaithPurchaseCooldown(bool bCivilian = false, int iValue = 0);
	int GetBuildingPurchaseCooldown() const;
	void SetBuildingPurchaseCooldown(int iValue);
	void ChangeBuildingPurchaseCooldown(int iValue);
	void CheckForOperationUnits();
	void DoSellBuilding();
	void SetTraded(PlayerTypes ePlayer, bool bValue);
	bool IsTraded(PlayerTypes ePlayer);
	void SetIgnoredForExpansionBickering(PlayerTypes ePlayer, bool bValue);
	bool IsIgnoredForExpansionBickering(PlayerTypes ePlayer) const;
#endif
	int foodConsumptionSpecialistTimes100() const;
	int foodConsumption(bool bNoAngry = false, int iExtra = 0) const;
	int foodConsumptionTimes100(bool bNoAngry = false, int iExtra = 0) const;
	int foodDifference(bool bBottom = true, bool bJustCheckingStarve = false) const;
	int foodDifferenceTimes100(bool bBottom = true, bool bJustCheckingStarve = false, int iCorpMod = -1, CvString* toolTipSink = NULL) const;
	int growthThreshold() const;

	int getGrowthMods() const;
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
	//cities can be part of multiple areas
	bool HasAccessToArea(int iAreaID) const;
	bool HasSharedAreaWith(const CvCity* pOther, bool bAllowLand, bool bAllowWater) const;

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

#if defined(MOD_GLOBAL_CITY_AUTOMATON_WORKERS)
	int getPopulation(bool bIncludeAutomatons = false) const;
#else
	int getPopulation() const;
#endif

#if defined(MOD_BALANCE_CORE)
	void setPopulation(int iNewValue, bool bReassignPop = true, bool bNoBonus = false);
#else
	void setPopulation(int iNewValue, bool bReassignPop = true);
#endif
	void changePopulation(int iChange, bool bReassignPop = true, bool bIgnoreStaticUpdate = false);

	void setLowestRazingPop(int iValue);

#if defined(MOD_GLOBAL_CITY_AUTOMATON_WORKERS)
	int getAutomatons() const;
	void setAutomatons(int iNewValue, bool bReassignPop = true);
	void changeAutomatons(int iChange, bool bReassignPop = true);
#endif
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

	int getJONSCulturePerTurn(bool bStatic = true) const;
	int GetBaseJONSCulturePerTurn() const;

	int GetJONSCulturePerTurnFromBuildings() const;

	int GetJONSCulturePerTurnFromPolicies() const;
	void ChangeJONSCulturePerTurnFromPolicies(int iChange);

	int GetJONSCulturePerTurnFromSpecialists() const;
	void ChangeJONSCulturePerTurnFromSpecialists(int iChange);

	int GetJONSCulturePerTurnFromGreatWorks() const;

	int GetJONSCulturePerTurnFromTraits() const;
	void ChangeYieldFromTraits(YieldTypes eIndex, int iChange);
#if defined(MOD_BALANCE_CORE)
	int GetYieldPerTurnFromTraits(YieldTypes eYield) const;
#endif

	int GetJONSCulturePerTurnFromReligion() const;

	int GetJONSCulturePerTurnFromLeagues() const;

	int getCultureRateModifier() const;
	void changeCultureRateModifier(int iChange);
#if defined(MOD_BALANCE_CORE_POLICIES)
	int getBuildingClassCultureChange(BuildingClassTypes eIndex) const;
	void changeBuildingClassCultureChange(BuildingClassTypes eIndex, int iChange);
#endif
#if defined(MOD_BALANCE_CORE)
	void SetBaseTourism(int iValue);
	int GetBaseTourism() const;
	void SetBaseTourismBeforeModifiers(int iValue);
	int GetBaseTourismBeforeModifiers() const;
#endif
	// END Culture

	int getTourismRateModifier() const;
	void changeTourismRateModifier(int iChange);
#if defined(MOD_BALANCE_CORE)
	int GetFaithPerTurn(bool bStatic = true) const;
#else
	int GetFaithPerTurn() const;
#endif
	int GetFaithPerTurnFromBuildings() const;

	int GetFaithPerTurnFromPolicies() const;
	void ChangeFaithPerTurnFromPolicies(int iChange);

	int GetYieldPerXFeature(FeatureTypes eFeature, YieldTypes eYield) const;
	int GetYieldPerXFeatureFromReligion(FeatureTypes eFeature, YieldTypes eYield) const;
	int GetYieldPerTurnFromUnimprovedFeatures(FeatureTypes eFeature, YieldTypes eYield) const;

	int GetFaithPerTurnFromReligion() const;

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
#if defined(MOD_BUILDINGS_CITY_AUTOMATON_WORKERS)
	int GetCityAutomatonWorkersChange() const;
	void changeCityAutomatonWorkersChange(int iChange);
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
	void DoAnnex(bool bRaze = false);

	int GetHappinessFromPolicies(int iPopMod = 0) const;
	int GetHappinessFromReligion() const;
	void UpdateHappinessFromReligion();

	void UpdateHappinessFromBuildingClasses();
	int GetHappinessFromBuildingClasses() const;

	int GetLocalHappiness(int iPopMod = 0, bool bExcludeEmpireContributions = false) const;
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	int updateNetHappiness();
	int getHappinessDelta() const;
	int getHappinessThresholdMod(YieldTypes eYield, int iMod = 0, bool bForceGlobal = false) const;
	int getThresholdSubtractions(YieldTypes eYield) const;
	int getThresholdAdditions(/*YieldTypes eYield = NO_YIELD*/) const;
	int getUnhappyCitizenCount() const;
	int getPopThresholdMod() const;
	int getEmpireSizeMod() const;

	CvString GetCityUnhappinessBreakdown(bool bIncludeMedian = false, bool bFlavorText = false);
	CvString GetCityHappinessBreakdown();

	float GetMinYieldNeededToReduceUnhappiness(int iCreated, int iNeeded, int iFactor) const;
	float GetMinYieldNeededToIncreaseUnhappiness(int iCreated, int iNeeded, int iFactor) const;

	int getUnhappinessFromCultureYield(int iPopMod = 0) const;
	int getUnhappinessFromCultureNeeded(int iMod = 0, bool bForceGlobal = false) const;
	int getUnhappinessFromCultureRaw(int iLimit = INT_MAX, int iPopMod = 0, bool bForceGlobal = false) const;
	int getUnhappinessFromCulture(int iPopMod = 0, bool bForceGlobal = false) const;
	int getUnhappinessFromScienceYield(int iPopMod = 0) const;
	int getUnhappinessFromScienceNeeded(int iMod = 0, bool bForceGlobal = false) const;
	int getUnhappinessFromScienceRaw(int iLimit = INT_MAX, int iPopMod = 0, bool bForceGlobal = false) const;
	int getUnhappinessFromScience(int iPopMod = 0, bool bForceGlobal = false) const;
	//new name should be distress instead of defense, but it's a hassle to change now
	int getUnhappinessFromDefenseYield(int iPopMod = 0) const;
	int getUnhappinessFromDefenseNeeded(int iMod = 0, bool bForceGlobal = false) const;
	int getUnhappinessFromDefenseRaw(int iLimit = INT_MAX, int iPopMod = 0, bool bForceGlobal = false) const;
	int getUnhappinessFromDefense(int iPopMod = 0, bool bForceGlobal = false) const;
	int getUnhappinessFromGoldYield(int iPopMod = 0) const;
	int getUnhappinessFromGoldNeeded(int iMod = 0, bool bForceGlobal = false) const;
	int getUnhappinessFromGoldRaw(int iLimit = INT_MAX, int iPopMod = 0, bool bForceGlobal = false) const;
	int getUnhappinessFromGold(int iPopMod = 0, bool bForceGlobal = false) const;
	int getUnhappinessFromConnectionRaw(int iLimit = INT_MAX) const;
	int getUnhappinessFromConnection() const;
	int getUnhappinessFromPillagedRaw(int iLimit = INT_MAX) const;
	int getUnhappinessFromPillaged() const;
	int getUnhappinessFromStarvingRaw(int iLimit = INT_MAX) const;
	int getUnhappinessFromStarving() const;
	int getUnhappinessFromReligionRaw(int iLimit = INT_MAX) const;
	int getUnhappinessFromReligion() const;

	int getJFDSpecialUnhappinessSources() const;

	int getUnhappinessAggregated() const;
	int getUnhappinessFromSpecialists(int iSpecialists) const;

	CvString getPotentialHappinessWithGrowth();
	int getPotentialHappinessWithGrowthVal() const;

	CvString getPotentialUnhappinessWithGrowth();
	int getPotentialUnhappinessWithGrowthVal() const;

	int GetNumPillagedPlots() const;
	void SetNumPillagedPlots(int iValue);
	void ChangeNumPillagedPlots(int iValue);

	int GetGrowthFromEvent() const;
	void ChangeGrowthFromEvent(int iValue);

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

	bool IsOwedFoodBuilding() const;
	void SetOwedFoodBuilding(bool bNewValue);

#if defined(MOD_BALANCE_CORE)
	void ChangeBorderObstacleCity(int iNewValue);
	int GetBorderObstacleLand() const;
	void SetBorderObstacleCity(int iValue);

	void ChangeBorderObstacleWater(int iNewValue);
	int GetBorderObstacleWater() const;
	void SetBorderObstacleWater(int iValue);

	void ChangeDeepWaterTileDamage(int iNewValue);
	int GetDeepWaterTileDamage() const;

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
	//check water or land, no domain for both
	bool IsBlockaded(DomainTypes eDomain) const;

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

	void SetNoWarmonger(bool bValue);
	bool IsNoWarmongerYet();

	int GetNumTimesOwned(PlayerTypes ePlayer) const;
	void SetNumTimesOwned(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesOwned(PlayerTypes ePlayer, int iValue);
	bool isEverOwned(PlayerTypes ePlayer) const;

	// Yield

	int getSeaPlotYield(YieldTypes eIndex) const;
	void changeSeaPlotYield(YieldTypes eIndex, int iChange);

	int getRiverPlotYield(YieldTypes eIndex) const;
	void changeRiverPlotYield(YieldTypes eIndex, int iChange);

	int getLakePlotYield(YieldTypes eIndex) const;
	void changeLakePlotYield(YieldTypes eIndex, int iChange);

	int getSeaResourceYield(YieldTypes eIndex) const;
	void changeSeaResourceYield(YieldTypes eIndex, int iChange);

	int GetYieldPerTurnFromReligion(YieldTypes eYield) const;
	void UpdateSpecialReligionYields(YieldTypes eYield);
	int GetSpecialReligionYields(YieldTypes eIndex) const;
	void SetSpecialReligionYields(YieldTypes eIndex, int iValue);

	int getBaseYieldRateModifier(YieldTypes eIndex, int iExtra = 0, CvString* toolTipSink = NULL) const;
#if defined(MOD_BALANCE_CORE)
	int getYieldRate(YieldTypes eIndex, bool bIgnoreTrade, bool bStatic = true) const;
	int getYieldRateTimes100(YieldTypes eIndex, bool bIgnoreTrade, bool bStatic = true) const;
#else
	int getYieldRate(YieldTypes eIndex, bool bIgnoreTrade) const;
	int getYieldRateTimes100(YieldTypes eIndex, bool bIgnoreTrade) const;
#endif
#if defined(MOD_PROCESS_STOCKPILE)
	int getBasicYieldRateTimes100(YieldTypes eIndex) const;
#endif

#if defined(MOD_BALANCE_CORE)
	void updateEconomicValue();
	int getEconomicValue(PlayerTypes ePossibleNewOwner);
	void setEconomicValue(PlayerTypes ePossibleNewOwner, int iValue);
#endif

	int GetContestedPlotScore(PlayerTypes eOtherPlayer) const;

#if defined(MOD_BALANCE_CORE_SPIES)
	int GetEspionageRanking() const;
	int GetEspionageRankingForEspionage(PlayerTypes ePlayer = NO_PLAYER, CityEventChoiceTypes eEventChoice = NO_EVENT_CHOICE_CITY) const;
	void ChangeEspionageRanking(int iRank, bool bNotify);
	void ResetEspionageRanking();
	void InitEspionageRanking();
	void SetEspionageRanking(int iValue);

	void SetTurnsSinceLastRankMessage(int iTurns);
	void ChangeTurnsSinceLastRankMessage(int iTurns);
	int GetTurnsSinceLastRankMessage() const;
#endif
	// Base Yield
	int getBaseYieldRate(YieldTypes eIndex) const;
#if defined(MOD_BALANCE_CORE)
	void UpdateCityYieldFromYield(YieldTypes eIndex1, YieldTypes eIndex2, int iModifiedYield);
#endif

#if defined(MOD_DIPLOMACY_CITYSTATES)
	int GetBaseScienceFromArt() const;
#endif

	int GetBaseYieldRateFromGreatWorks(YieldTypes eIndex) const;

	int GetBaseYieldRateFromTerrain(YieldTypes eIndex) const;
	void ChangeBaseYieldRateFromTerrain(YieldTypes eIndex, int iChange);
	void SetBaseYieldRateFromTerrain(YieldTypes eIndex, int iValue);

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

	int GetYieldFromVictoryGlobal(YieldTypes eIndex) const;
	void ChangeYieldFromVictoryGlobal(YieldTypes eIndex, int iChange);

	int GetYieldFromPillage(YieldTypes eIndex) const;
	void ChangeYieldFromPillage(YieldTypes eIndex, int iChange);

	int GetYieldFromPillageGlobal(YieldTypes eIndex) const;
	void ChangeYieldFromPillageGlobal(YieldTypes eIndex, int iChange);

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

	int GetYieldFromFaithPurchase(YieldTypes eIndex) const;
	void ChangeYieldFromFaithPurchase(YieldTypes eIndex, int iChange);

	int GetYieldFromUnitLevelUp(YieldTypes eIndex) const;
	void ChangeYieldFromUnitLevelUp(YieldTypes eIndex, int iChange);

	int GetYieldPerAlly(YieldTypes eIndex) const;
	void ChangeYieldPerAlly(YieldTypes eYield, int iChange);

	int GetYieldPerFriend(YieldTypes eIndex) const;
	void ChangeYieldPerFriend(YieldTypes eYield, int iChange);

	int GetBuildingYieldFromYield(YieldTypes eIndex1, YieldTypes eIndex2) const;
	void ChangeBuildingYieldFromYield(YieldTypes eIndex, YieldTypes eIndex2, int iChange);

	int GetYieldFromInternalTREnd(YieldTypes eIndex1) const;
	void ChangeYieldFromInternalTREnd(YieldTypes eIndex, int iChange);

	int GetYieldFromInternalTR(YieldTypes eIndex1) const;
	void ChangeYieldFromInternalTR(YieldTypes eIndex, int iChange);

	int GetYieldFromProcessModifier(YieldTypes eIndex1) const;
	void ChangeYieldFromProcessModifier(YieldTypes eIndex, int iChange);

	int GetRealYieldFromYield(YieldTypes eIndex1, YieldTypes eIndex2) const;
	void SetRealYieldFromYield(YieldTypes eIndex1, YieldTypes eIndex2, int iChange);

	void ChangeSpecialistRateModifier(SpecialistTypes eSpecialist, int iChange);
	int GetSpecialistRateModifier(SpecialistTypes eSpecialist) const;
#endif

#if defined(MOD_BALANCE_CORE)
	int GetGreatPersonProgressFromConstruction(GreatPersonTypes eGreatPerson, EraTypes eEra) const;
	void ChangeGreatPersonProgressFromConstruction(GreatPersonTypes eGreatPerson, EraTypes eEra, int iChange);
#endif

#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
	void ChangeEmpireNeedsModifier(int iChange);
	int GetEmpireNeedsModifier() const;

	int GetFlatNeedsReduction(YieldTypes eYield) const;
	void ChangeFlatNeedsReduction(YieldTypes eYield, int iValue);

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

	int GetResourceQuantityFromPOP(ResourceTypes eResource) const;
	void ChangeResourceQuantityFromPOP(ResourceTypes eResource, int iChange);
	void SetResourceQuantityFromPOP(ResourceTypes eResource, int iValue);

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

	void ChangeNoUnhappfromXSpecialists(int iChange);
	int GetNoUnhappfromXSpecialists() const;

	bool isBorderCity() const;

	void DoBarbIncursion();
#endif

	void changeNukeInterceptionChance(int iValue);
	int getNukeInterceptionChance() const;
#if defined(MOD_BALANCE_CORE)
	void SetPurchased(BuildingClassTypes eBuildingClass, bool bValue);
	bool IsPurchased(BuildingClassTypes eBuildingClass);

	void SetBestForWonder(BuildingClassTypes eBuildingClass, bool bValue);
	bool IsBestForWonder(BuildingClassTypes eBuildingClass);
#endif
	// END Base Yield

	int GetYieldPerPopTimes100(YieldTypes eIndex) const;
	void ChangeYieldPerPopTimes100(YieldTypes eIndex, int iChange);

#if defined(MOD_BALANCE_CORE)
	int GetYieldPerPopInEmpireTimes100(YieldTypes eIndex) const;
	void ChangeYieldPerPopInEmpireTimes100(YieldTypes eIndex, int iChange);
#endif

	int GetYieldPerReligionTimes100(YieldTypes eIndex) const;
	void ChangeYieldPerReligionTimes100(YieldTypes eIndex, int iChange);

	int getYieldRateModifier(YieldTypes eIndex) const;
	void changeYieldRateModifier(YieldTypes eIndex, int iChange);

#if defined(MOD_BALANCE_CORE_POLICIES)
	int getReligionBuildingYieldRateModifier(BuildingClassTypes eIndex1, YieldTypes eIndex2)	const;
	void changeReligionBuildingYieldRateModifier(BuildingClassTypes eIndex1, YieldTypes eIndex2, int iChange);

	int getLocalBuildingClassYield(BuildingClassTypes eIndex1, YieldTypes eIndex2)	const;
	void changeLocalBuildingClassYield(BuildingClassTypes eIndex1, YieldTypes eIndex2, int iChange);

	void ChangeGreatWorkYieldChange(YieldTypes eYield, int iValue);
	int GetGreatWorkYieldChange(YieldTypes eYield) const;
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

	bool isEverLiberated(PlayerTypes eIndex) const;
	void setEverLiberated(PlayerTypes eIndex, bool bNewValue);

	bool isRevealed(TeamTypes eIndex, bool bDebug, bool bAdjacentIsGoodEnough) const;
	bool setRevealed(TeamTypes eIndex, bool bNewValue);

	const CvString getName() const;
	const CvString getNameNoSpace() const;
	const char* getNameKey() const;
	void setName(const char* szNewValue, bool bFound = false, bool bForceChange = false);

	void setCaptureData(int iTemp[5], bool bTemp[2]);
	std::vector<int> getCaptureDataInt() const;
	std::vector<bool> getCaptureDataBool() const;
	void resetCaptureData();
	bool isPendingCapture() const;

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
	vector<PromotionTypes> getFreePromotions() const;
	void changeFreePromotionCount(PromotionTypes eIndex, int iChange);

#if defined(MOD_BALANCE_CORE)
	void SetRetroactivePromotion(PromotionTypes ePromotion);
#endif

	int getSpecialistFreeExperience() const;
	void changeSpecialistFreeExperience(int iChange);

	void updateStrengthValue();
	int getStrengthValue(bool bForRangeStrike = false, bool bIgnoreBuildingDefense = false, const CvUnit* pDefender = NULL) const;
	int GetPower() const;

	int getDamage() const;
	void setDamage(int iValue, bool noMessage = false);
	void changeDamage(int iChange);

	bool isMadeAttack() const;
	void setMadeAttack(bool bNewValue);

#if defined(MOD_EVENTS_CITY_BOMBARD)
	int getBombardRange(bool& bIndirectFireAllowed) const;
	int getBombardRange() const;
#endif

	int getCityBuildingBombardRange() const;
	void changeCityBuildingBombardRange(int iValue);
	bool getCityIndirectFire() const;
	void changeCityIndirectFire(int iValue);

	int getCityBuildingRangeStrikeModifier() const;
	void changeCityBuildingRangeStrikeModifier(int iValue);

	void ChangeNumTimesAttackedThisTurn(PlayerTypes ePlayer, int iValue);
	int GetNumTimesAttackedThisTurn(PlayerTypes ePlayer) const;

	bool canRangeStrike() const;
	bool CanRangeStrikeNow() const;
	bool IsHasBuildingThatAllowsRangeStrike() const;

	bool canRangeStrikeAt(int iX, int iY) const;
	CityTaskResult rangeStrike(int iX, int iY);
	CvUnit* rangedStrikeTarget(const CvPlot* pPlot) const;
	bool canRangedStrikeTarget(const CvPlot& targetPlot) const;
	CvUnit* getBestRangedStrikeTarget() const;

	int rangeCombatUnitDefense(_In_ const CvUnit* pDefender, const CvPlot* pInPlot = NULL, bool bQuickAndDirty = false) const;
	int rangeCombatDamage(const CvUnit* pDefender, bool bIncludeRand = true, const CvPlot* pInPlot = NULL, bool bQuickAndDirty = false) const;

	int GetAirStrikeDefenseDamage(const CvUnit* pAttacker, bool bIncludeRand = true) const;

	bool IsEnemyInRange(int iRange, bool bMustBeAbleToAttack);
	void DoNearbyEnemy();

	bool IsInDanger(PlayerTypes eEnemy) const;
	bool IsInDangerFromPlayers(vector<PlayerTypes>& vWarAllies) const;

	void IncrementUnitStatCount(CvUnit* pUnit);
	void CheckForAchievementBuilding(BuildingTypes eBuilding);
	bool AreAllUnitsBuilt();

	// Plot acquisition

	bool CanBuyPlot(int iPlotX, int iPlotY, bool bIgnoreCost = false);
	bool CanBuyAnyPlot(void);
	CvPlot* GetNextBuyablePlot(bool bForPurchase);
	void GetBuyablePlotList(std::vector<int>& aiPlotList, bool bForPurchase, int nChoices = 3);
	int GetBuyPlotCost(int iPlotX, int iPlotY) const;
	void BuyPlot(int iPlotX, int iPlotY);
	void DoAcquirePlot(int iPlotX, int iPlotY);
	int GetBuyPlotScore(int& iBestX, int& iBestY);
	int GetIndividualPlotScore(const CvPlot* pPlot) const;

	int GetCheapestPlotInfluenceDistance() const;
	void SetCheapestPlotInfluenceDistance(int iValue);
	void DoUpdateCheapestPlotInfluenceDistance();
	int calculateInfluenceDistance(CvPlot* pDest, int iMaxRange) const;

	// End plot acquisition

	bool isValidBuildingLocation(BuildingTypes eIndex) const;

	void setThreatValue(int iThreatValue);
	int getThreatValue() const;

	void clearOrderQueue();
	void pushOrder(OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend, bool bRush = false);
	void popOrder(int iNum, bool bFinish = false, bool bChoose = false);
	void swapOrder(int iNum);
	bool hasOrder(OrderTypes eOrder, int iData1, int iData2) const;
	void startHeadOrder();
	void stopHeadOrder();
	int getOrderQueueLength() const;
	const OrderData* getOrderFromQueue(int iIndex) const;
	OrderData* getOrderFromQueue(int iIndex);
	const OrderData* nextOrderQueueNode(const OrderData* pNode) const;
	OrderData* nextOrderQueueNode(OrderData* pNode);
	const OrderData* headOrderQueueNode() const;
	OrderData* headOrderQueueNode();
	const OrderData* tailOrderQueueNode() const;
	bool CleanUpQueue(void);  // remove items in the queue that are no longer valid

	void produce(UnitTypes eTrainUnit, UnitAITypes eTrainAIUnit = NO_UNITAI, bool bCanOverflow = true);
	void produce(BuildingTypes eConstructBuilding, bool bCanOverflow = true);
	void produce(ProjectTypes eCreateProject, bool bCanOverflow = true);
	void produce(SpecialistTypes eSpecialist, bool bCanOverflow = true);

	CvUnit* CreateUnit(UnitTypes eUnitType, UnitAITypes eAIType = NO_UNITAI, UnitCreationReason eReason = REASON_DEFAULT, bool bUseToSatisfyOperation = true, bool bIsPurchase = false);
	bool CreateBuilding(BuildingTypes eBuildType);
	bool CreateProject(ProjectTypes eProjectType);

	void changeProjectCount(ProjectTypes eProject, int iValue);
	int getProjectCount(ProjectTypes eProject) const;

	CvPlot* GetPlotForNewUnit(UnitTypes eUnitType, bool bAllowCenterPlot=true) const;
	bool CanPlaceUnitHere(UnitTypes eUnitType) const;
	bool IsCanPurchase(bool bTestPurchaseCost, bool bTestTrainable, UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectType, YieldTypes ePurchaseYield); //slow version
	bool IsCanPurchase(const std::vector<int>& vPreExistingBuildings, bool bTestPurchaseCost, bool bTestTrainable, UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectType, YieldTypes ePurchaseYield); //fast version
	void Purchase(UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectType, YieldTypes ePurchaseYield);

	CvCityStrategyAI* GetCityStrategyAI() const;
	CvCityCitizens* GetCityCitizens() const;
	CvCityReligions* GetCityReligions() const;
	CvCityEmphases* GetCityEmphases() const;
	CvCityEspionage* GetCityEspionage() const;
	CvCityCulture* GetCityCulture() const;

	template<typename City, typename Visitor>
	static void Serialize(City& city, Visitor& visitor);
	void read(FDataStream& kStream);
	void write(FDataStream& kStream) const;

	virtual void AI_init() = 0;
	virtual void AI_reset() = 0;
	virtual void AI_doTurn() = 0;
	virtual void AI_chooseProduction(bool bInterruptWonders, bool bInterruptBuildings) = 0;
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
	const CvSyncArchive<CvCity>& getSyncArchive() const;
	CvSyncArchive<CvCity>& getSyncArchive();
	std::string debugDump(const FAutoVariableBase&) const;
	std::string stackTraceRemark(const FAutoVariableBase&) const;

	bool			IsBusy() const;
	// Combat related
	const CvUnit* getCombatUnit() const;
	CvUnit* getCombatUnit();
	void			setCombatUnit(CvUnit* pUnit, bool bAttacking = false);
	void			clearCombat();
	bool			isFighting() const;

	bool HasBelief(BeliefTypes iBeliefType) const;
	bool HasBuilding(BuildingTypes iBuildingType) const;
	bool HasBuildingClass(BuildingClassTypes iBuildingClassType) const;
	bool HasAnyWonder() const;
	bool HasWonder(BuildingTypes iBuildingType) const;
	bool IsBuildingWorldWonder() const;
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
	int CountImprovement(ImprovementTypes iImprovementType, bool bOnlyCreated = false) const;
	int CountWorkedImprovement(ImprovementTypes iImprovementType) const;
	int CountPlotType(PlotTypes iPlotType) const;
	int CountWorkedPlotType(PlotTypes iPlotType) const;
	int CountResource(ResourceTypes iResourceType) const;
	int CountWorkedResource(ResourceTypes iResourceType) const;
	int CountTerrain(TerrainTypes iTerrainType) const;
	int CountWorkedTerrain(TerrainTypes iTerrainType) const;
	int CountAllOwnedTerrain(TerrainTypes iTerrainType) const;

#if defined(MOD_CORE_PER_TURN_DAMAGE)
	int addDamageReceivedThisTurn(int iDamage);
	void flipDamageReceivedPerTurn();
	bool isInDangerOfFalling() const;
	bool isUnderSiege() const;
	int getDamageTakenLastTurn() const;
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

	int GetSappedTurns() const;
	void ChangeSappedTurns(int iValue);
	void SetSappedTurns(int iValue);

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

protected:
	SYNC_ARCHIVE_MEMBER(CvCity)

	PlayerTypes m_eOwner;
	int m_iX;
	int m_iY;
	int m_iID;

	int m_iRallyX;
	int m_iRallyY;
	int m_iGameTurnFounded;
	int m_iGameTurnAcquired;
	int m_iGameTurnLastExpanded;
	int m_iPopulation;
#if defined(MOD_GLOBAL_CITY_AUTOMATON_WORKERS)
	int m_iAutomatons;
#endif
	int m_iHighestPopulation;
	int m_iExtraHitPoints;

	int m_iNumGreatPeople;
	int m_iBaseGreatPeopleRate;
	int m_iGreatPeopleRateModifier;
	int m_iJONSCultureStored;
	int m_iJONSCultureLevel;
	int m_iJONSCulturePerTurnFromPolicies;
	int m_iJONSCulturePerTurnFromSpecialists;
	std::vector<int> m_iaAddedYieldPerTurnFromTraits;
	int m_iFaithPerTurnFromPolicies;
#if defined(MOD_BALANCE_CORE)
	int m_iAdditionalFood;
	int m_iCityBuildingBombardRange;
	int m_iCityIndirectFire;
	int m_iCityBuildingRangeStrikeModifier;
#endif
	int m_iCultureRateModifier;
	int m_iNumWorldWonders;
	int m_iNumTeamWonders;
	int m_iNumNationalWonders;
	int m_iWonderProductionModifier;
	int m_iCapturePlunderModifier;
	int m_iPlotCultureCostModifier;
	int m_iPlotBuyCostModifier;
#if defined(MOD_BUILDINGS_CITY_WORKING)
	int m_iCityWorkingChange;
	int m_iCitySupplyModifier;
	int m_iCitySupplyFlat;
	bool m_bAllowsProductionTradeRoutes;
	bool m_bAllowsFoodTradeRoutes;
	bool m_bAllowPuppetPurchase;
#endif
#if defined(MOD_BUILDINGS_CITY_AUTOMATON_WORKERS)
	int m_iCityAutomatonWorkersChange;
#endif
	int m_iMaintenance;
	int m_iHealRate;
	int m_iNoOccupiedUnhappinessCount;
#if defined(HH_MOD_BUILDINGS_FRUITLESS_PILLAGE)
	int m_iLocalGainlessPillageCount;
#endif
	int m_iFood;
	int m_iMaxFoodKeptPercent;
	int m_iOverflowProduction;
	int m_iFeatureProduction;
	int m_iMilitaryProductionModifier;
	int m_iSpaceProductionModifier;
	int m_iFreeExperience;
	int m_iMaxAirUnits;
	int m_iNukeModifier;
	int m_iTradeRouteTargetBonus;
	int m_iTradeRouteRecipientBonus;
	int m_iTradeRouteSeaGoldBonus;
	int m_iTradeRouteLandGoldBonus;
	int m_iNumTradeRouteBonus;
	int m_iCityConnectionTradeRouteGoldModifier;
	int m_iCitySizeBoost;
	int m_iSpecialistFreeExperience;
	int m_iStrengthValue;
	int m_iDamage;
	int m_iThreatValue;
	int m_hGarrison;  // unused
	mutable int m_hGarrisonOverride; //only temporary, not serialized
	int m_iResourceDemanded;
	int m_iWeLoveTheKingDayCounter;
	int m_iLastTurnGarrisonAssigned;
	int m_iThingsProduced; // total number of units, buildings, wonders, etc. this city has constructed
	int m_iDemandResourceCounter;
	int m_iResistanceTurns;
	int m_iRazingTurns;
	int m_iLowestRazingPop;
	int m_iCountExtraLuxuries;
	int m_iCheapestPlotInfluenceDistance;
	int m_iEspionageModifier;
#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
	int m_iConversionModifier;
#endif

	OperationSlot m_unitBeingBuiltForOperation;

	bool m_bNeverLost;
	bool m_bDrafted;
	bool m_bProductionAutomated;
	bool m_bLayoutDirty;
	bool m_bMadeAttack;
	bool m_bOccupied;
	bool m_bPuppet;
	bool m_bIgnoreCityForHappiness;
	bool m_bIndustrialRouteToCapital; //also set for water connection once railroad is available
	int m_iTerrainImprovementNeed;

	PlayerTypes m_ePreviousOwner;
	PlayerTypes m_eOriginalOwner;
	PlayerTypes m_ePlayersReligion;

	std::vector<int> m_aiSeaPlotYield;
	std::vector<int> m_aiRiverPlotYield;
	std::vector<int> m_aiLakePlotYield;
	std::vector<int> m_aiSeaResourceYield;
	std::vector<int> m_aiBaseYieldRateFromTerrain;
	std::vector<int> m_aiBaseYieldRateFromBuildings;
	std::vector<int> m_aiBaseYieldRateFromSpecialists;
	std::vector<int> m_aiBaseYieldRateFromMisc;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	std::vector<int> m_aiBaseYieldRateFromLeague;
	int m_iTotalScienceyAid;
	int m_iTotalArtsyAid;
	int m_iTotalGreatWorkAid;
#endif
#if defined(MOD_DIPLOMACY_CITYSTATES) || defined(MOD_BALANCE_CORE)
	std::vector<int> m_aiChangeGrowthExtraYield;
#endif
#if defined(MOD_BALANCE_CORE)
	int m_iHappinessFromEmpire;
	int m_iHappinessFromLuxuries;
	int m_iUnhappinessFromEmpire;
	int m_iStaticTechDeviation;
	std::vector<int> m_aiNumProjects;
	std::vector<int> m_aiStaticGlobalYield;
	std::vector<int> m_aiStaticNeedAdditives;
	std::vector<int> m_aiLongestPotentialTradeRoute;
	std::vector<int> m_aiNumTimesAttackedThisTurn;
	std::vector<int> m_aiYieldFromKnownPantheons;
	std::vector<int> m_aiYieldFromVictory;
	std::vector<int> m_aiYieldFromVictoryGlobal;
	std::vector<int> m_aiYieldFromPillage;
	std::vector<int> m_aiYieldFromPillageGlobal;
	std::vector<int> m_aiGoldenAgeYieldMod;
	std::vector<int> m_aiYieldFromWLTKD;
	std::vector<int> m_aiYieldFromConstruction;
	std::vector<int> m_aiYieldFromTech;
	std::vector<int> m_aiYieldFromBirth;
	std::vector<int> m_aiYieldFromUnitProduction;
	std::vector<int> m_aiYieldFromBorderGrowth;
	std::vector<int> m_aiYieldFromPolicyUnlock;
	std::vector<int> m_aiYieldFromPurchase;
	std::vector<int> m_aiYieldFromFaithPurchase;
	std::vector<int> m_aiYieldFromUnitLevelUp;
	std::vector<int> m_aiYieldPerAlly;
	std::vector<int> m_aiYieldPerFriend;
	std::vector<int> m_aiYieldFromInternalTREnd;
	std::vector<int> m_aiYieldFromInternalTR;
	std::vector<int> m_aiYieldFromProcessModifier;
	std::vector<int> m_aiSpecialistRateModifier;
	std::vector<int> m_aiThemingYieldBonus;
	std::vector<int> m_aiYieldFromSpyAttack;
	std::vector<int> m_aiYieldFromSpyDefense;
	std::vector<int> m_aiNumTimesOwned;
	std::vector<int> m_aiStaticCityYield;
	int m_iTradePriorityLand;
	int m_iTradePrioritySea;
	int m_iUnitPurchaseCooldown;
	int m_iUnitPurchaseCooldownCivilian;
	int m_iUnitPurchaseCooldownMod;
	int m_iUnitPurchaseCooldownCivilianMod;
	int m_iUnitFaithPurchaseCooldown;
	int m_iUnitFaithPurchaseCooldownCivilian;
	int m_iBuildingPurchaseCooldown;
	int m_iReligiousTradeModifier;
	int m_iCityAirStrikeDefense;
	int m_iFreeBuildingTradeTargetCity;
	int m_iBaseTourism;
	int m_iBaseTourismBeforeModifiers;
	int m_iBorderObstacleCity;
	int m_iBorderObstacleWater;
	int m_iDeepWaterTileDamage;
	int m_iNumNearbyMountains;
	int m_iLocalUnhappinessMod;
	bool m_bNoWarmonger;
	int m_iCitySpyRank;
	int m_iTurnsSinceRankAnnouncement;
	int m_iChangePovertyUnhappiness;
	int m_iEmpireNeedsModifier;
	int m_iChangeDefenseUnhappiness;
	int m_iChangeUnculturedUnhappiness;
	int m_iChangeIlliteracyUnhappiness;
	int m_iChangeMinorityUnhappiness;
	int m_iTradeRouteSeaDistanceModifier;
	int m_iTradeRouteLandDistanceModifier;
	int m_iNukeInterceptionChance;
	std::vector<int> m_aiEconomicValue;
#endif
	std::vector<int> m_aiBaseYieldRateFromReligion;
#if defined(MOD_BALANCE_CORE)
	std::vector<int> m_aiBaseYieldRateFromCSAlliance;
	std::vector<int> m_aiBaseYieldRateFromCSFriendship;
	std::vector<int> m_aiYieldFromMinors;
	std::vector<int> m_aiResourceQuantityPerXFranchises;
	std::vector<int> m_aiYieldChangeFromCorporationFranchises;
	std::vector<int> m_aiResourceQuantityFromPOP;
	std::vector<int> m_aiNeedsFlatReduction;
	int m_iLandTourismBonus;
	int m_iSeaTourismBonus;
	int m_iAlwaysHeal;
	int m_iResourceDiversityModifier;
	int m_iNoUnhappfromXSpecialists;
	int m_aiStaticNeedsUpdateTurn;
	std::vector<int> m_aiGreatWorkYieldChange;
#endif
	std::vector<int> m_aiYieldRateModifier;
	std::vector<int> m_aiYieldPerPop;
#if defined(MOD_BALANCE_CORE)
	std::map<int, int> m_aiYieldPerPopInEmpire;
#endif
	std::vector<int> m_aiYieldPerReligion;
	std::vector<int> m_aiPowerYieldRateModifier;
	std::vector<int> m_aiResourceYieldRateModifier;
	std::vector<int> m_aiExtraSpecialistYield;
	std::vector<int> m_aiProductionToYieldModifier;
	std::vector<int> m_aiDomainFreeExperience;
	std::vector<int> m_aiDomainProductionModifier;

	std::vector<bool> m_abEverLiberated;
#if defined(MOD_BALANCE_CORE)
	std::vector<bool> m_abIsBestForWonder;
	std::vector<bool> m_abIsPurchased;
	std::vector<bool> m_abTraded;
	std::vector<bool> m_abIgnoredForExpansionBickering;
	std::vector<bool> m_abPaidAdoptionBonus;
	std::vector<int> m_aiReligiousPressureModifier;
	int m_iExtraBuildingMaintenance;
	std::vector<int> m_paiNumTerrainWorked;
	std::vector<int> m_paiNumFeaturelessTerrainWorked;
	std::vector<int> m_paiNumFeatureWorked;
	std::vector<int> m_paiNumResourceWorked;
	std::vector<int> m_paiNumImprovementWorked;
#endif
	CvString m_strScriptData;

#if defined(MOD_CORE_PER_TURN_DAMAGE)
	int m_iDamageTakenThisTurn;
	int m_iDamageTakenLastTurn;
#endif

	std::vector<int> m_paiNoResource;
	std::vector<int> m_paiFreeResource;
	std::vector<int> m_paiNumResourcesLocal;
	std::vector<int> m_paiNumUnimprovedResourcesLocal;
	std::vector<int> m_paiProjectProduction;
	std::vector<int> m_paiSpecialistProduction;
	std::vector<int> m_paiUnitProduction;
	std::vector<int> m_paiUnitProductionTime;
	std::vector<int> m_paiSpecialistCount;
	std::vector<int> m_paiMaxSpecialistCount;
	std::vector<int> m_paiForceSpecialistCount;
	std::vector<int> m_paiFreeSpecialistCount;
	std::vector<int> m_paiImprovementFreeSpecialists;
	std::vector<int> m_paiUnitCombatFreeExperience;
	std::vector<int> m_paiUnitCombatProductionModifier;
	std::map<PromotionTypes, int> m_paiFreePromotionCount;
#if defined(MOD_BALANCE_CORE_POLICIES)
	std::vector<int> m_paiBuildingClassCulture;
	std::vector<int> m_paiHurryModifier;
#endif

#if defined(MOD_BALANCE_CORE)
	int m_iHappinessDelta;
	int m_iPillagedPlots;
	int m_iGrowthEvent;
	int m_iGrowthFromTourism;
	int m_iBuildingClassHappiness;
	int m_iReligionHappiness;
	std::vector<int> m_vClosestNeighbors;
	std::vector<int> m_vAttachedUnits;
#endif

	int m_iBaseHappinessFromBuildings;
	int m_iUnmoddedHappinessFromBuildings;

	bool m_bRouteToCapitalConnectedLastTurn;
	bool m_bRouteToCapitalConnectedThisTurn;

	CvString m_strName;

	bool m_bOwedCultureBuilding;
	bool m_bOwedFoodBuilding;

	mutable FFastSmallFixedList< OrderData, 25, true, c_eCiv5GameplayDLL > m_orderQueue;

	vector<SCityExtraYields> m_yieldChanges; //[NUM_YIELD_TYPES]

#if defined(MOD_BALANCE_CORE)
	std::map<std::pair<int, int>, short> m_ppiGreatPersonProgressFromConstruction;
#endif
#if defined(MOD_BALANCE_CORE_EVENTS)
	std::vector<int> m_aiEventCooldown;
	std::vector<bool> m_abEventActive;
	std::vector<bool> m_abEventChoiceActive;
	std::vector<bool> m_abEventChoiceFired;
	std::vector<bool> m_abEventFired;
	std::vector<int> m_aiEventChoiceDuration;
	std::vector<int> m_aiEventIncrement;
	std::vector<int> m_aiEventCityYield;
	std::vector<int> m_aiEventCityYieldModifier;
	int m_iEventHappiness;
	int m_iCityEventCooldown;
	vector<SCityEventYields> m_eventYields; //[NUM_YIELD_TYPES]
#endif

#if defined(MOD_BALANCE_CORE)
	bool m_bIsColony;
	int m_iProvinceLevel;
	int m_iOrganizedCrime;
	int m_iResistanceCounter;
	int m_iPlagueCounter;
	int m_iPlagueTurns;
	int m_iPlagueType;
	int m_iSappedTurns;
	int m_iLoyaltyCounter;
	int m_iDisloyaltyCounter;
	int m_iLoyaltyStateType;
	std::vector<int> m_aiYieldModifierFromHappiness;
	std::vector<int> m_aiYieldModifierFromHealth;
	std::vector<int> m_aiYieldModifierFromCrime;
	std::vector<int> m_aiYieldModifierFromDevelopment;
	std::vector<int> m_aiYieldFromHappiness;
	std::vector<int> m_aiYieldFromHealth;
	std::vector<int> m_aiYieldFromCrime;
	std::vector<int> m_aiYieldFromDevelopment;
	std::vector<int> m_aiTempCaptureData;
	std::vector<bool> m_abTempCaptureData;
	bool m_bIsPendingCapture;
#endif

	CvCityBuildings* m_pCityBuildings;
	CvCityStrategyAI* m_pCityStrategyAI;
	CvCityCitizens* m_pCityCitizens;
	CvCityReligions* m_pCityReligions;
	CvCityEmphases* m_pEmphases;
	CvCityEspionage* m_pCityEspionage;
	CvCityCulture* m_pCityCulture;

	// CACHE: cache frequently used values
	int m_iPopulationRank;
	bool m_bPopulationRankValid;
	std::vector<int> m_aiBaseYieldRank;
	std::vector<bool> m_abBaseYieldRankValid;
	std::vector<int> m_aiYieldRank;
	std::vector<bool> m_abYieldRankValid;
#if defined(MOD_BALANCE_CORE)
	std::vector<bool> m_abOwedChosenBuilding;
	std::vector<bool> m_abBuildingInvestment;
	std::vector<bool> m_abUnitInvestment;
	std::vector<bool> m_abBuildingConstructed;
#endif

	//cache for great work yields, they are need often during citizen re-assignment but they don't change
	mutable vector<int> m_GwYieldCache; //not serialized

	IDInfo m_combatUnit;		// The unit the city is in combat with

	void doGrowth();
	void doProduction(bool bAllowNoProduction);
	void doProcess();
	void doDecay();
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
	void OverrideGarrison(const CvUnit* pUnit) const;
	friend class CvCityGarrisonOverride;
};

namespace FSerialization
{
	void SyncCities();
	void ClearCityDeltas();
}

SYNC_ARCHIVE_BEGIN(CvCity)
SYNC_ARCHIVE_VAR(PlayerTypes, m_eOwner)
SYNC_ARCHIVE_VAR(int, m_iX)
SYNC_ARCHIVE_VAR(int, m_iY)
SYNC_ARCHIVE_VAR(int, m_iID)
SYNC_ARCHIVE_VAR(int, m_iRallyX)
SYNC_ARCHIVE_VAR(int, m_iRallyY)
SYNC_ARCHIVE_VAR(int, m_iGameTurnFounded)
SYNC_ARCHIVE_VAR(int, m_iGameTurnAcquired)
SYNC_ARCHIVE_VAR(int, m_iGameTurnLastExpanded)
SYNC_ARCHIVE_VAR(int, m_iPopulation)
SYNC_ARCHIVE_VAR(int, m_iHighestPopulation)
SYNC_ARCHIVE_VAR(int, m_iExtraHitPoints)
SYNC_ARCHIVE_VAR(int, m_iNumGreatPeople)
SYNC_ARCHIVE_VAR(int, m_iBaseGreatPeopleRate)
SYNC_ARCHIVE_VAR(int, m_iGreatPeopleRateModifier)
SYNC_ARCHIVE_VAR(int, m_iJONSCultureStored)
SYNC_ARCHIVE_VAR(int, m_iJONSCultureLevel)
SYNC_ARCHIVE_VAR(int, m_iJONSCulturePerTurnFromPolicies)
SYNC_ARCHIVE_VAR(int, m_iJONSCulturePerTurnFromSpecialists)
SYNC_ARCHIVE_VAR(std::vector<int>, m_iaAddedYieldPerTurnFromTraits)
SYNC_ARCHIVE_VAR(int, m_iFaithPerTurnFromPolicies)
SYNC_ARCHIVE_VAR(int, m_iAdditionalFood)
SYNC_ARCHIVE_VAR(int, m_iCityBuildingBombardRange)
SYNC_ARCHIVE_VAR(int, m_iCityIndirectFire)
SYNC_ARCHIVE_VAR(int, m_iCityBuildingRangeStrikeModifier)
SYNC_ARCHIVE_VAR(int, m_iCultureRateModifier)
SYNC_ARCHIVE_VAR(int, m_iNumWorldWonders)
SYNC_ARCHIVE_VAR(int, m_iNumTeamWonders)
SYNC_ARCHIVE_VAR(int, m_iNumNationalWonders)
SYNC_ARCHIVE_VAR(int, m_iWonderProductionModifier)
SYNC_ARCHIVE_VAR(int, m_iCapturePlunderModifier)
SYNC_ARCHIVE_VAR(int, m_iPlotCultureCostModifier)
SYNC_ARCHIVE_VAR(int, m_iPlotBuyCostModifier)
SYNC_ARCHIVE_VAR(int, m_iCityWorkingChange)
SYNC_ARCHIVE_VAR(int, m_iCitySupplyModifier)
SYNC_ARCHIVE_VAR(int, m_iCitySupplyFlat)
SYNC_ARCHIVE_VAR(bool, m_bAllowsProductionTradeRoutes)
SYNC_ARCHIVE_VAR(bool, m_bAllowsFoodTradeRoutes)
SYNC_ARCHIVE_VAR(bool, m_bAllowPuppetPurchase)
SYNC_ARCHIVE_VAR(int, m_iCityAutomatonWorkersChange)
SYNC_ARCHIVE_VAR(int, m_iMaintenance)
SYNC_ARCHIVE_VAR(int, m_iHealRate)
SYNC_ARCHIVE_VAR(int, m_iNoOccupiedUnhappinessCount)
SYNC_ARCHIVE_VAR(int, m_iLocalGainlessPillageCount)
SYNC_ARCHIVE_VAR(int, m_iFood)
SYNC_ARCHIVE_VAR(int, m_iMaxFoodKeptPercent)
SYNC_ARCHIVE_VAR(int, m_iOverflowProduction)
SYNC_ARCHIVE_VAR(int, m_iFeatureProduction)
SYNC_ARCHIVE_VAR(int, m_iMilitaryProductionModifier)
SYNC_ARCHIVE_VAR(int, m_iSpaceProductionModifier)
SYNC_ARCHIVE_VAR(int, m_iFreeExperience)
SYNC_ARCHIVE_VAR(int, m_iMaxAirUnits)
SYNC_ARCHIVE_VAR(int, m_iNukeModifier)
SYNC_ARCHIVE_VAR(int, m_iTradeRouteTargetBonus)
SYNC_ARCHIVE_VAR(int, m_iTradeRouteRecipientBonus)
SYNC_ARCHIVE_VAR(int, m_iTradeRouteSeaGoldBonus)
SYNC_ARCHIVE_VAR(int, m_iTradeRouteLandGoldBonus)
SYNC_ARCHIVE_VAR(int, m_iNumTradeRouteBonus)
SYNC_ARCHIVE_VAR(int, m_iCityConnectionTradeRouteGoldModifier)
SYNC_ARCHIVE_VAR(int, m_iCitySizeBoost)
SYNC_ARCHIVE_VAR(int, m_iSpecialistFreeExperience)
SYNC_ARCHIVE_VAR(int, m_iStrengthValue)
SYNC_ARCHIVE_VAR(int, m_iDamage)
SYNC_ARCHIVE_VAR(int, m_iThreatValue)
SYNC_ARCHIVE_VAR(int, m_hGarrison)
SYNC_ARCHIVE_VAR(int, m_iResourceDemanded)
SYNC_ARCHIVE_VAR(int, m_iWeLoveTheKingDayCounter)
SYNC_ARCHIVE_VAR(int, m_iLastTurnGarrisonAssigned)
SYNC_ARCHIVE_VAR(int, m_iThingsProduced)
SYNC_ARCHIVE_VAR(int, m_iDemandResourceCounter)
SYNC_ARCHIVE_VAR(int, m_iResistanceTurns)
SYNC_ARCHIVE_VAR(int, m_iRazingTurns)
SYNC_ARCHIVE_VAR(int, m_iLowestRazingPop)
SYNC_ARCHIVE_VAR(int, m_iCountExtraLuxuries)
SYNC_ARCHIVE_VAR(int, m_iCheapestPlotInfluenceDistance)
SYNC_ARCHIVE_VAR(int, m_iEspionageModifier)
SYNC_ARCHIVE_VAR(int, m_iConversionModifier)
SYNC_ARCHIVE_VAR(bool, m_bNeverLost)
SYNC_ARCHIVE_VAR(bool, m_bDrafted)
SYNC_ARCHIVE_VAR(bool, m_bProductionAutomated)
SYNC_ARCHIVE_VAR(bool, m_bLayoutDirty)
SYNC_ARCHIVE_VAR(bool, m_bMadeAttack)
SYNC_ARCHIVE_VAR(bool, m_bOccupied)
SYNC_ARCHIVE_VAR(bool, m_bPuppet)
SYNC_ARCHIVE_VAR(bool, m_bIgnoreCityForHappiness)
SYNC_ARCHIVE_VAR(bool, m_bIndustrialRouteToCapital)
SYNC_ARCHIVE_VAR(int, m_iTerrainImprovementNeed)
SYNC_ARCHIVE_VAR(PlayerTypes, m_ePreviousOwner)
SYNC_ARCHIVE_VAR(PlayerTypes, m_eOriginalOwner)
SYNC_ARCHIVE_VAR(PlayerTypes, m_ePlayersReligion)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiSeaPlotYield)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiRiverPlotYield)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiLakePlotYield)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiSeaResourceYield)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiBaseYieldRateFromTerrain)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiBaseYieldRateFromBuildings)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiBaseYieldRateFromSpecialists)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiBaseYieldRateFromMisc)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiBaseYieldRateFromLeague)
SYNC_ARCHIVE_VAR(int, m_iTotalScienceyAid)
SYNC_ARCHIVE_VAR(int, m_iTotalArtsyAid)
SYNC_ARCHIVE_VAR(int, m_iTotalGreatWorkAid)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiChangeGrowthExtraYield)
SYNC_ARCHIVE_VAR(int, m_iHappinessFromEmpire)
SYNC_ARCHIVE_VAR(int, m_iHappinessFromLuxuries)
SYNC_ARCHIVE_VAR(int, m_iUnhappinessFromEmpire)
SYNC_ARCHIVE_VAR(int, m_iStaticTechDeviation)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiNumProjects)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiStaticGlobalYield)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiStaticNeedAdditives)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiLongestPotentialTradeRoute)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiNumTimesAttackedThisTurn)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromKnownPantheons)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromVictory)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromVictoryGlobal)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromPillage)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromPillageGlobal)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiGoldenAgeYieldMod)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromWLTKD)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromConstruction)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromTech)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromBirth)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromUnitProduction)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromBorderGrowth)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromPolicyUnlock)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromPurchase)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromFaithPurchase)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromUnitLevelUp)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldPerAlly)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldPerFriend)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromInternalTREnd)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromInternalTR)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromProcessModifier)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiSpecialistRateModifier)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiThemingYieldBonus)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromSpyAttack)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromSpyDefense)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiNumTimesOwned)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiStaticCityYield)
SYNC_ARCHIVE_VAR(int, m_iTradePriorityLand)
SYNC_ARCHIVE_VAR(int, m_iTradePrioritySea)
SYNC_ARCHIVE_VAR(int, m_iUnitPurchaseCooldown)
SYNC_ARCHIVE_VAR(int, m_iUnitPurchaseCooldownCivilian)
SYNC_ARCHIVE_VAR(int, m_iUnitPurchaseCooldownMod)
SYNC_ARCHIVE_VAR(int, m_iUnitPurchaseCooldownCivilianMod)
SYNC_ARCHIVE_VAR(int, m_iUnitFaithPurchaseCooldown)
SYNC_ARCHIVE_VAR(int, m_iUnitFaithPurchaseCooldownCivilian)
SYNC_ARCHIVE_VAR(int, m_iBuildingPurchaseCooldown)
SYNC_ARCHIVE_VAR(int, m_iReligiousTradeModifier)
SYNC_ARCHIVE_VAR(int, m_iCityAirStrikeDefense)
SYNC_ARCHIVE_VAR(int, m_iFreeBuildingTradeTargetCity)
SYNC_ARCHIVE_VAR(int, m_iBaseTourism)
SYNC_ARCHIVE_VAR(int, m_iBaseTourismBeforeModifiers)
SYNC_ARCHIVE_VAR(int, m_iBorderObstacleCity)
SYNC_ARCHIVE_VAR(int, m_iBorderObstacleWater)
SYNC_ARCHIVE_VAR(int, m_iDeepWaterTileDamage)
SYNC_ARCHIVE_VAR(int, m_iNumNearbyMountains)
SYNC_ARCHIVE_VAR(int, m_iLocalUnhappinessMod)
SYNC_ARCHIVE_VAR(bool, m_bNoWarmonger)
SYNC_ARCHIVE_VAR(int, m_iCitySpyRank)
SYNC_ARCHIVE_VAR(int, m_iTurnsSinceRankAnnouncement)
SYNC_ARCHIVE_VAR(int, m_iChangePovertyUnhappiness)
SYNC_ARCHIVE_VAR(int, m_iEmpireNeedsModifier)
SYNC_ARCHIVE_VAR(int, m_iChangeDefenseUnhappiness)
SYNC_ARCHIVE_VAR(int, m_iChangeUnculturedUnhappiness)
SYNC_ARCHIVE_VAR(int, m_iChangeIlliteracyUnhappiness)
SYNC_ARCHIVE_VAR(int, m_iChangeMinorityUnhappiness)
SYNC_ARCHIVE_VAR(int, m_iTradeRouteSeaDistanceModifier)
SYNC_ARCHIVE_VAR(int, m_iTradeRouteLandDistanceModifier)
SYNC_ARCHIVE_VAR(int, m_iNukeInterceptionChance)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiEconomicValue)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiBaseYieldRateFromReligion)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiBaseYieldRateFromCSAlliance)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiBaseYieldRateFromCSFriendship)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromMinors)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiResourceQuantityPerXFranchises)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldChangeFromCorporationFranchises)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiResourceQuantityFromPOP)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiNeedsFlatReduction)
SYNC_ARCHIVE_VAR(int, m_iLandTourismBonus)
SYNC_ARCHIVE_VAR(int, m_iSeaTourismBonus)
SYNC_ARCHIVE_VAR(int, m_iAlwaysHeal)
SYNC_ARCHIVE_VAR(int, m_iResourceDiversityModifier)
SYNC_ARCHIVE_VAR(int, m_iNoUnhappfromXSpecialists)
SYNC_ARCHIVE_VAR(int, m_aiStaticNeedsUpdateTurn)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiGreatWorkYieldChange)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldRateModifier)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldPerPop)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldPerReligion)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiPowerYieldRateModifier)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiResourceYieldRateModifier)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiExtraSpecialistYield)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiProductionToYieldModifier)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiDomainFreeExperience)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiDomainProductionModifier)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abEverLiberated)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abIsBestForWonder)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abIsPurchased)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abTraded)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abIgnoredForExpansionBickering)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abPaidAdoptionBonus)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiReligiousPressureModifier)
SYNC_ARCHIVE_VAR(int, m_iExtraBuildingMaintenance)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiNumTerrainWorked)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiNumFeaturelessTerrainWorked)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiNumFeatureWorked)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiNumResourceWorked)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiNumImprovementWorked)
SYNC_ARCHIVE_VAR(CvString, m_strScriptData)
SYNC_ARCHIVE_VAR(int, m_iDamageTakenThisTurn)
SYNC_ARCHIVE_VAR(int, m_iDamageTakenLastTurn)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiNoResource)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiFreeResource)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiNumResourcesLocal)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiNumUnimprovedResourcesLocal)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiProjectProduction)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiSpecialistProduction)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiUnitProduction)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiUnitProductionTime)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiSpecialistCount)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiMaxSpecialistCount)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiForceSpecialistCount)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiFreeSpecialistCount)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiImprovementFreeSpecialists)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiUnitCombatFreeExperience)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiUnitCombatProductionModifier)
SYNC_ARCHIVE_VAR(SYNC_ARCHIVE_VAR_TYPE(std::map<PromotionTypes, int>), m_paiFreePromotionCount)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiBuildingClassCulture)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiHurryModifier)
SYNC_ARCHIVE_VAR(int, m_iHappinessDelta)
SYNC_ARCHIVE_VAR(int, m_iPillagedPlots)
SYNC_ARCHIVE_VAR(int, m_iGrowthEvent)
SYNC_ARCHIVE_VAR(int, m_iGrowthFromTourism)
SYNC_ARCHIVE_VAR(int, m_iBuildingClassHappiness)
SYNC_ARCHIVE_VAR(int, m_iReligionHappiness)
SYNC_ARCHIVE_VAR(std::vector<int>, m_vClosestNeighbors)
SYNC_ARCHIVE_VAR(int, m_iBaseHappinessFromBuildings)
SYNC_ARCHIVE_VAR(int, m_iUnmoddedHappinessFromBuildings)
SYNC_ARCHIVE_VAR(bool, m_bRouteToCapitalConnectedLastTurn)
SYNC_ARCHIVE_VAR(bool, m_bRouteToCapitalConnectedThisTurn)
SYNC_ARCHIVE_VAR(CvString, m_strName)
SYNC_ARCHIVE_VAR(bool, m_bOwedCultureBuilding)
SYNC_ARCHIVE_VAR(bool, m_bOwedFoodBuilding)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiEventCooldown)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abEventActive)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abEventChoiceActive)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abEventChoiceFired)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abEventFired)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiEventChoiceDuration)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiEventIncrement)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiEventCityYield)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiEventCityYieldModifier)
SYNC_ARCHIVE_VAR(int, m_iEventHappiness)
SYNC_ARCHIVE_VAR(int, m_iCityEventCooldown)
SYNC_ARCHIVE_VAR(bool, m_bIsColony)
SYNC_ARCHIVE_VAR(int, m_iProvinceLevel)
SYNC_ARCHIVE_VAR(int, m_iOrganizedCrime)
SYNC_ARCHIVE_VAR(int, m_iResistanceCounter)
SYNC_ARCHIVE_VAR(int, m_iPlagueCounter)
SYNC_ARCHIVE_VAR(int, m_iPlagueTurns)
SYNC_ARCHIVE_VAR(int, m_iPlagueType)
SYNC_ARCHIVE_VAR(int, m_iSappedTurns)
SYNC_ARCHIVE_VAR(int, m_iLoyaltyCounter)
SYNC_ARCHIVE_VAR(int, m_iDisloyaltyCounter)
SYNC_ARCHIVE_VAR(int, m_iLoyaltyStateType)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldModifierFromHappiness)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldModifierFromHealth)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldModifierFromCrime)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldModifierFromDevelopment)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromHappiness)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromHealth)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromCrime)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromDevelopment)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiTempCaptureData)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abTempCaptureData)
SYNC_ARCHIVE_VAR(bool, m_bIsPendingCapture)
SYNC_ARCHIVE_VAR(int, m_iPopulationRank)
SYNC_ARCHIVE_VAR(bool, m_bPopulationRankValid)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiBaseYieldRank)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abBaseYieldRankValid)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldRank)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abYieldRankValid)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abOwedChosenBuilding)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abBuildingInvestment)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abUnitInvestment)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abBuildingConstructed)
SYNC_ARCHIVE_END()

//just a guard class so we never forget to unset the garrison override
class CvCityGarrisonOverride
{
public:
	CvCityGarrisonOverride(const CvCity* pCity, const CvUnit* pUnit)
	{
		m_pCity = pCity;
		//can only garrison units of the city owner!
		if (m_pCity && pUnit && m_pCity->getOwner() == pUnit->getOwner())
			m_pCity->OverrideGarrison(pUnit);
	}
	~CvCityGarrisonOverride()
	{
		if (m_pCity)
			m_pCity->OverrideGarrison(0);
	}
protected:
	const CvCity* m_pCity;
};

#endif

bool isUnitTypeFoodProduction(PlayerTypes ePlayer, UnitTypes eUnit);

FDataStream& operator>>(FDataStream& loadFrom, SCityExtraYields& writeTo);
FDataStream& operator<<(FDataStream& saveTo, const SCityExtraYields& readFrom);
FDataStream& operator>>(FDataStream& loadFrom, SCityEventYields& writeTo);
FDataStream& operator<<(FDataStream& saveTo, const SCityEventYields& readFrom);
