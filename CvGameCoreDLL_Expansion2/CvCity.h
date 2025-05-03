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
	vector<pair<ImprovementTypes, fraction>> fromImprovement, fromImprovementLocal, fromImprovementGlobal;
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

struct SPlotStats
{
	SPlotStats();

	vector<int> vTerrainCount;
	vector<int> vFeatureCount;
	vector<int> vResourceCount;
	vector<int> vImprovementCount;
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

	void UpdateCachedYieldMedians();
	void SetCachedBasicNeedsMedian(int iValue);
	void SetCachedGoldMedian(int iValue);
	void SetCachedScienceMedian(int iValue);
	void SetCachedCultureMedian(int iValue);
	void SetCachedTechNeedModifier(int iValue);
	void SetCachedEmpireSizeModifier(int iValue);
	void SetYieldMediansCachedTurn(int iTurn);
	int GetCachedBasicNeedsMedian() const;
	int GetCachedGoldMedian() const;
	int GetCachedScienceMedian() const;
	int GetCachedCultureMedian() const;
	int GetCachedTechNeedModifier() const;
	int GetCachedEmpireSizeModifier() const;
	int GetYieldMediansCachedTurn() const;

	int GetHappinessFromEmpire() const;
	void ChangeHappinessFromEmpire(int iValue);
	void ResetHappinessFromEmpire();

	int GetUnhappinessFromEmpire() const;
	void UpdateUnhappinessFromEmpire();
#endif

#if defined(MOD_BALANCE_CORE_EVENTS)
	void DoEvents(bool bEspionageOnly = false);
	bool IsCityEventValid(CityEventTypes eEvent);
	bool IsCityEventChoiceValid(CityEventChoiceTypes eChosenEventChoice, CityEventTypes eParentEvent, bool bIgnoreActive = false, bool bIgnorePlayer = false);
	bool IsCityEventChoiceValidEspionage(CityEventChoiceTypes eEventChoice, CityEventTypes eEvent, int uiSpyIndex, PlayerTypes eSpyOwner, bool bIgnoreActive = false, bool bIgnoreNetworkPoints = false);
	void DoCancelEventChoice(CityEventChoiceTypes eChosenEventChoice);
	CvString GetScaledSpyEffectText(CityEventChoiceTypes eEventChoice, bool bSpyMissionEnd, TechTypes eTech = NO_TECH, int iGWID = -1, int iAmountStolen = -1);
	void DoStartEvent(CityEventTypes eChosenEvent, bool bSendMsg);
	void DoEventChoice(CityEventChoiceTypes eEventChoice, CityEventTypes eCityEvent = NO_EVENT_CITY, bool bSendMsg = true, int iSpyID = -1, PlayerTypes eSpyOwner = NO_PLAYER);
	CvString GetScaledHelpText(CityEventChoiceTypes eEventChoice, bool bYieldsOnly, int iSpyIndex = -1, PlayerTypes eSpyOwner = NO_PLAYER, bool bSpyMissionEnd = false);
	CvString GetDisabledTooltip(CityEventChoiceTypes eChosenEventChoice, int iSpyIndex = -1, PlayerTypes eSpyOwner = NO_PLAYER);

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

	void ChangeEventBuildingClassYield(BuildingClassTypes eIndex1, YieldTypes eIndex2, int iChange);
	int GetEventBuildingClassCityYield(BuildingClassTypes eIndex1, YieldTypes eIndex2) const;

	void ChangeEventBuildingClassYieldModifier(BuildingClassTypes eIndex1, YieldTypes eIndex2, int iChange);
	int GetEventBuildingClassCityYieldModifier(BuildingClassTypes eIndex1, YieldTypes eIndex2) const;

	void ChangeEventImprovementYield(ImprovementTypes eImprovement, YieldTypes eIndex2, int iChange);
	int GetEventImprovementYield(ImprovementTypes eImprovement, YieldTypes eIndex2) const;

	void ChangeEventResourceYield(ResourceTypes eResource, YieldTypes eIndex2, int iChange);
	int GetEventResourceYield(ResourceTypes eResource, YieldTypes eIndex2) const;

	void ChangeEventSpecialistYield(SpecialistTypes eSpecialist, YieldTypes eIndex2, int iChange);
	int GetEventSpecialistYield(SpecialistTypes eSpecialist, YieldTypes eIndex2) const;

	void ChangeEventTerrainYield(TerrainTypes eTerrain, YieldTypes eIndex2, int iChange);
	int GetEventTerrainYield(TerrainTypes eTerrain, YieldTypes eIndex2) const;
	void ChangeEventFeatureYield(FeatureTypes eFeature, YieldTypes eIndex2, int iChange);
	int GetEventFeatureYield(FeatureTypes eFeature, YieldTypes eIndex2) const;

	bool IsEventChoiceFired(CityEventChoiceTypes eEventChoice) const;
	void SetEventChoiceFired(CityEventChoiceTypes eEventChoice, bool bValue);

	bool IsEventFired(CityEventTypes eEvent) const;
	void SetEventFired(CityEventTypes eEvent, bool bValue);

	void ChangeEventHappiness(int iValue);
	int GetEventHappiness() const;

	void CheckActivePlayerEvents();
	void ApplyPlayerEventChoice(const EventChoiceTypes eEventChoice);

	virtual void AI_DoEventChoice(CityEventTypes eEvent) = 0;

	int maxXPValue() const;
#endif

	bool IsIndustrialRouteToCapitalConnected() const;
	void SetIndustrialRouteToCapitalConnected(bool bValue);

	void SetRouteToCapitalConnected(bool bValue, bool bSuppressReligionUpdate = false);
	bool IsRouteToCapitalConnected(void) const;

	void createGreatGeneral(UnitTypes eGreatPersonUnit, bool bIsFree);
	void createGreatAdmiral(UnitTypes eGreatPersonUnit, bool bIsFree);

	CityTaskResult doTask(TaskTypes eTask, int iData1 = -1, int iData2 = -1, bool bOption = false, bool bAlt = false, bool bShift = false, bool bCtrl = false);

	void chooseProduction(UnitTypes eTrainUnit = NO_UNIT, BuildingTypes eConstructBuilding = NO_BUILDING, ProjectTypes eCreateProject = NO_PROJECT, bool bFinish = false, bool bFront = false);

	int getBuyPlotDistance() const;
	int getWorkPlotDistance(int iChange = 0) const;
	int GetNumWorkablePlots(int iChange = 0) const;
	bool IsWithinWorkRange(const CvPlot* pPlot) const;

	void clearWorkingOverride(int iIndex);
	int countNumImprovedPlots(ImprovementTypes eImprovement = NO_IMPROVEMENT) const;
	int countNumImprovablePlots(ImprovementTypes eImprovement = NO_IMPROVEMENT, DomainTypes eDomain = DOMAIN_LAND) const;
	int countNumWaterPlots() const;
	int countNumLakePlots() const;
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
	bool canCreate(ProjectTypes eProject, bool bContinue = false, bool bTestVisible = false, CvString* toolTipSink = NULL) const;
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

	void ChangeNumTerrainWorked(TerrainTypes eTerrain, int iChange);
	int GetNumTerrainWorked(TerrainTypes eTerrain);

	void ChangeNumFeaturelessTerrainWorked(TerrainTypes eTerrain, int iChange);
	int GetNumFeaturelessTerrainWorked(TerrainTypes eTerrain);

	void ChangeNumFeatureWorked(FeatureTypes eFeature, int iChange);
	int GetNumFeatureWorked(FeatureTypes eFeature);

	void ChangeNumImprovementWorked(ImprovementTypes eImprovement, int iChange);
	int GetNumImprovementWorked(ImprovementTypes eImprovement);

	int GetYieldFromUnimprovedFeatures(YieldTypes eYield) const;
	void SetYieldFromUnimprovedFeatures(YieldTypes eYield, int iNewValue);

	int GetImprovementCount(ImprovementTypes eImprovement);
	void ChangeImprovementCount(ImprovementTypes eImprovement, int iChange);

	int GetYieldPerXTerrainFromBuildingsTimes100(TerrainTypes eTerrain, YieldTypes eYield) const;
	void ChangeYieldPerXTerrainFromBuildingsTimes100(TerrainTypes eTerrain, YieldTypes eYield, int iChange);
	int GetYieldPerXTerrain(TerrainTypes eTerrain, YieldTypes eYield) const;
	void SetYieldPerXTerrain(TerrainTypes eTerrain, YieldTypes eYield, int iValue);
	void UpdateYieldPerXTerrain(YieldTypes eYield, TerrainTypes eTerrain = NO_TERRAIN);

	int GetYieldPerXTerrainFromReligion(TerrainTypes eTerrain, YieldTypes eYield) const;
	void SetYieldPerXTerrainFromReligion(TerrainTypes eTerrain, YieldTypes eYield, int iValue);
	void UpdateYieldPerXTerrainFromReligion(YieldTypes eYield, TerrainTypes eTerrain = NO_TERRAIN);

	int GetYieldPerXFeatureFromBuildingsTimes100(FeatureTypes eFeature, YieldTypes eYield) const;
	void ChangeYieldPerXFeatureFromBuildingsTimes100(FeatureTypes eFeature, YieldTypes eYield, int iChange);
	int GetYieldPerXFeature(FeatureTypes eFeature, YieldTypes eYield) const;
	void SetYieldPerXFeature(FeatureTypes eFeature, YieldTypes eYield, int iValue);
	int GetYieldPerXFeatureFromReligion(FeatureTypes eFeature, YieldTypes eYield) const;
	void SetYieldPerXFeatureFromReligion(FeatureTypes eFeature, YieldTypes eYield, int iValue);
	void UpdateYieldPerXFeature(YieldTypes eYield, FeatureTypes eFeature = NO_FEATURE);

	void UpdateYieldPerXUnimprovedFeature(YieldTypes eYield);

	fraction GetYieldPerXImprovementLocal(ImprovementTypes eImprovement, YieldTypes eYield) const;
	void ChangeYieldPerXImprovementLocal(ImprovementTypes eImprovement, YieldTypes eYield, fraction fChange);
	fraction GetYieldPerXImprovementGlobal(ImprovementTypes eImprovement, YieldTypes eYield) const;
	void ChangeYieldPerXImprovementGlobal(ImprovementTypes eImprovement, YieldTypes eYield, fraction fChange);
	fraction GetYieldPerXImprovement(ImprovementTypes eImprovement, YieldTypes eYield) const;
	void SetYieldPerXImprovement(ImprovementTypes eImprovement, YieldTypes eYield, fraction fValue);
	void UpdateYieldPerXImprovement(YieldTypes eYield, ImprovementTypes eImprovement = NO_IMPROVEMENT);

	int getHurryModifier(HurryTypes eIndex) const;
	void changeHurryModifier(HurryTypes eIndex, int iChange);

	int getSpecialistExtraYield(SpecialistTypes eIndex1, YieldTypes eIndex2) const;
	void changeSpecialistExtraYield(SpecialistTypes eIndex1, YieldTypes eIndex2, int iChange);
#endif
	int GetTerrainExtraYield(TerrainTypes eTerrain, YieldTypes eYield) const;
	void ChangeTerrainExtraYield(TerrainTypes eTerrain, YieldTypes eYield, int iChange);

	int GetPlotExtraYield(PlotTypes ePlot, YieldTypes eYield) const;
	void ChangePlotExtraYield(PlotTypes ePlot, YieldTypes eYield, int iChange);

	std::set<int> GetAccomplishmentsWithBonuses() const;
	void AddToAccomplishmentsWithBonuses(AccomplishmentTypes eAccomplishment);

	std::set<int> GetPlotList() const;
	void AddToPlotList(CvPlot* pPlot);
	void RemoveFromPlotList(CvPlot* pPlot);

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

	void GetPlotsBoostedByBuilding(std::vector<int>& aiPlotList, BuildingTypes eBuilding);
	int GetNumHiddenBuildings() const;
	bool IsBuildingHidden(BuildingTypes eBuilding) const;

	void SetBuildingHidden(BuildingTypes eBuilding);
	void ClearHiddenBuildings();


#if defined(MOD_BALANCE_CORE)
	bool IsBuildingFeatureValid(BuildingTypes eBuilding, CvString* toolTipSink = NULL) const;
#endif
	// Resource Demanded

	ResourceTypes GetResourceDemanded(bool bHideUnknown = true) const;
	void SetResourceDemanded(ResourceTypes eResource);
	void DoPickResourceDemanded();
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
	bool isProductionProcess() const;

	bool canContinueProduction(OrderData order);
	int getProductionExperience(UnitTypes eUnit = NO_UNIT) const;
	void addProductionExperience(CvUnit* pUnit, bool bHalveXP = false, bool bGoldPurchase = false) const;

	UnitTypes getProductionUnit() const;
	UnitAITypes getProductionUnitAI() const;
	BuildingTypes getProductionBuilding() const;
	ProjectTypes getProductionProject() const;
	ProcessTypes getProductionProcess() const;
	const char* getProductionName() const;
	const char* getProductionNameKey() const;
	int getGeneralProductionTurnsLeft() const;

	bool isProductionSpaceshipPart() const;
	bool isFoodProduction() const;
	int getFirstUnitOrder(UnitTypes eUnit) const;
	int getFirstBuildingOrder(BuildingTypes eBuilding) const;
	bool isBuildingInQueue(BuildingTypes eBuilding) const;
	int getFirstProjectOrder(ProjectTypes eProject) const;
	int getNumTrainUnitAI(UnitAITypes eUnitAI) const;

	int getProduction() const;
	int getProductionTimes100() const;
	int getProductionNeeded() const;
	int getProductionNeeded(UnitTypes eUnit, bool bIgnoreInvestment = false) const;
	int getProductionNeeded(BuildingTypes eBuilding, bool bIgnoreInvestment = false) const;
	int getProductionNeeded(ProjectTypes eProject) const;
	int getProductionTurnsLeft() const;
	int getProductionTurnsLeft(UnitTypes eUnit, int iNum) const;
	int getProductionTurnsLeft(BuildingTypes eBuilding, int iNum) const;
	int getProductionTurnsLeft(ProjectTypes eProject, int iNum) const;
#if defined(MOD_PROCESS_STOCKPILE)
	int getProductionNeeded(ProcessTypes eProcess) const;
	int getProductionTurnsLeft(ProcessTypes eProcess, int iNum) const;
#endif
#if defined(MOD_BALANCE_CORE)
	void SetBuildingInvestment(BuildingClassTypes eBuildingClass, bool bNewValue);
	int GetBuildingCostInvestmentReduction(BuildingClassTypes eBuildingClass) const;
	bool IsBuildingInvestment(BuildingClassTypes eBuildingClass) const;

	bool IsProcessInternationalProject(ProcessTypes eProcess) const;

	void SetUnitInvestment(UnitClassTypes eUnitClass, bool bNewValue);
	int GetUnitCostInvestmentReduction(UnitClassTypes eUnitClass) const;
	bool IsUnitInvestment(UnitClassTypes eUnitClass) const;

	void SetBuildingConstructed(BuildingClassTypes eBuildingClass, bool bNewValue);
	bool IsBuildingConstructed(BuildingClassTypes eBuildingClass) const;
#endif
	int GetPurchaseCost(UnitTypes eUnit);
	int GetFaithPurchaseCost(UnitTypes eUnit, bool bIncludeBeliefDiscounts);
	int GetPurchaseCost(BuildingTypes eBuilding);
	int GetFaithPurchaseCost(BuildingTypes eBuilding);
	int GetPurchaseCost(ProjectTypes eProject);
	int GetPurchaseCostFromProduction(int iProduction);

	int getProductionTurnsLeft(int iProductionNeeded, int iProduction, int iProductionDifference, bool bIncludeOverflow) const;
	void setProduction(int iNewValue);
	void changeProduction(int iChange);
	void setProductionTimes100(int iNewValue);
	void changeProductionTimes100(int iChange);

	int getCurrentProductionModifier(_In_opt_ CvString* toolTipSink = NULL) const;
	int getGeneralProductionModifiers(_In_opt_ CvString* toolTipSink = NULL) const;
	int getProductionModifier(UnitTypes eUnit, _In_opt_ CvString* toolTipSink = NULL, bool bIgnoreHappiness = false) const;
	int getProductionModifier(BuildingTypes eBuilding, _In_opt_ CvString* toolTipSink = NULL) const;
	int getProductionModifier(ProjectTypes eProject, _In_opt_ CvString* toolTipSink = NULL) const;
	int getProductionModifier(ProcessTypes eProcess, _In_opt_ CvString* toolTipSink = NULL) const;

	int getTotalOverflowProductionTimes100() const;
	int GetFoodProductionTimes100(int iExcessFoodTimes100) const;

	SPlotStats getPlotStats() const;
	int getResourceYieldRateModifier(YieldTypes eIndex, ResourceTypes eResource) const;

	void processResource(ResourceTypes eResource, int iChange);
	void processBuilding(BuildingTypes eBuilding, int iChange, bool bFirst, bool bObsolete = false, bool bApplyingAllCitiesBonus = false, bool bNoBonus = false);
	void processProcess(ProcessTypes eProcess, int iChange);
	void processSpecialist(SpecialistTypes eSpecialist, int iChange, eUpdateMode updateMode);

	void UpdateReligion(ReligionTypes eNewMajority, bool bRecalcPlotYields = true);
	void UpdateReligiousYieldFromSpecialist(bool bFirstOneAdded);

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
	PlayerTypes GetOwnerForDominationVictory() const;

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
	int getFoodConsumptionNonSpecialistTimes100() const;
	int getFoodConsumptionSpecialistTimes100() const;
	int getFoodConsumptionTimes100(bool bIgnoreProcess = false, bool bAssumeNoReductionForNonSpecialists = false) const;
	int growthThreshold() const;

	int getGrowthMods(CvString* toolTipSink = NULL, int iAssumedLocalHappinessChange = 0) const;
#if defined(MOD_BALANCE_CORE)
	int GetNumFreeSpecialists();
	int GetUnhappinessFromCitySpecialists();
#endif

	int productionLeft() const;

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
	//landmasses can also be water bodies
	bool HasAccessToLandmassOrOcean(int iLandmassID) const;
	bool HasSharedLandmassWith(const CvCity* pOther, bool bAllowLand, bool bAllowWater) const;

	bool NeedsGarrison() const;
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

	int getPopulation(bool bIncludeAutomatons = false) const;
	void setPopulation(int iNewValue, bool bReassignPop = true, bool bNoBonus = false);
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

	int getBaseGreatPeopleRate() const;
	int getGreatPeopleRate() const;
	int getTotalGreatPeopleRateModifier() const;
	void changeBaseGreatPeopleRate(int iChange);
	int getGreatPeopleRateModifier() const;
	void changeGreatPeopleRateModifier(int iChange);
	int getGPRateModifierPerMarriage() const;
	void changeGPRateModifierPerMarriage(int iChange);
	int getGPRateModifierPerLocalTheme() const;
	void changeGPRateModifierPerLocalTheme(int iChange);

	int GetImprovementGreatPersonRateModifier() const;
	int GetReligionGreatPersonRateModifier(GreatPersonTypes eGreatPerson) const;

	int GetGPPOnCitizenBirth() const;
	void ChangeGPPOnCitizenBirth(int iChange);

	// Culture stuff

	int GetJONSCultureStoredTimes100() const;
	void SetJONSCultureStoredTimes100(int iValue);
	void ChangeJONSCultureStored(int iChange);
	void ChangeJONSCultureStoredTimes100(int iChange);

	int GetJONSCultureLevel() const;
	void SetJONSCultureLevel(int iValue);
	void ChangeJONSCultureLevel(int iChange);
	void DoJONSCultureLevelIncrease();
	int GetJONSCultureThreshold() const;

	int GetBaseYieldRateFromPolicies(YieldTypes eYield) const;
	void ChangeBaseYieldRateFromPolicies(YieldTypes eYield, int iChange);

	void ChangeYieldFromTraits(YieldTypes eIndex, int iChange);
	int GetYieldPerTurnFromTraits(YieldTypes eYield) const;

	int GetJONSCulturePerTurnFromLeagues() const;

	int getCultureRateModifier() const;
	void changeCultureRateModifier(int iChange);
#if defined(MOD_BALANCE_CORE_POLICIES)
	int getBuildingClassCultureChange(BuildingClassTypes eIndex) const;
	void changeBuildingClassCultureChange(BuildingClassTypes eIndex, int iChange);
#endif

	// END Culture

	int getTourismRateModifier() const;
	void changeTourismRateModifier(int iChange);

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

	int GetDiplomatInfluenceBoost() const;
	void ChangeDiplomatInfluenceBoost(int iChange);

	int GetBorderGrowthRateIncreaseTotal(CvString* tooltipSink = NULL) const;

	int GetBorderGrowthRateIncrease() const;
	void ChangeBorderGrowthRateIncrease(int iChange);

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
	int getHealRate() const;
	void changeHealRate(int iChange);

	int GetEspionageModifier() const;
	void ChangeEspionageModifier(int iChange);

	int GetSpySecurityModifier() const;
	void ChangeSpySecurityModifier(int iChange);

	int GetSpySecurityModifierPerXPop() const;
	void ChangeSpySecurityModifierPerXPop(int iChange);

	fraction GetDefensePerWonder() const;
	void ChangeDefensePerWonder(fraction fChange);

#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
	int GetConversionModifier() const;
	void ChangeConversionModifier(int iChange);
#endif

	bool IsNoOccupiedUnhappiness() const;
	int GetNoOccupiedUnhappinessCount() const;
	void ChangeNoOccupiedUnhappinessCount(int iChange);

	int GetFoodBonusPerCityMajorityFollower() const;
	void ChangeFoodBonusPerCityMajorityFollower(int iChange);

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

	int updateNetHappiness();
	int getHappinessDelta() const;
	int GetAllNeedsModifier(bool bForceRecalc) const;
	int getUnhappyCitizenCount() const;
	int GetCitySizeModifier() const;
	int GetEmpireSizeModifier() const;
	int GetReducedEmpireSizeModifier(bool bForceRecalc, bool bCityOnly) const;
	int GetNeedModifierForYield(YieldTypes eYield) const;
	int GetTotalNeedModifierForYield(YieldTypes eYield, bool bForceRecalc) const;
	int GetCityNeedModifierForYield(YieldTypes eYield) const;

	CvString GetCityUnhappinessBreakdown(bool bIncludeMedian = false, bool bCityBanner = false);
	CvString GetCityHappinessBreakdown();

	int GetUnhappinessFromOccupation() const;
	int GetUnhappinessFromFamine() const;
	int GetUnhappinessFromPillagedTiles() const;
	int GetDistress(bool bForceRecalc, int iAssumedExtraYieldRate = 0) const;
	int GetDistressRaw(bool bForceRecalc, int iAssumedExtraYieldRate = 0) const;
	float GetBasicNeedsMedian(bool bForceRecalc, int iAdditionalModifier) const;
	int GetPoverty(bool bForceRecalc, int iAssumedExtraYieldRate = 0) const;
	int GetPovertyRaw(bool bForceRecalc, int iAssumedExtraYieldRate = 0) const;
	float GetGoldMedian(bool bForceRecalc, int iAdditionalModifier) const;
	int GetIlliteracy(bool bForceRecalc, int iAssumedExtraYieldRate = 0) const;
	int GetIlliteracyRaw(bool bForceRecalc, int iAssumedExtraYieldRate = 0) const;
	float GetScienceMedian(bool bForceRecalc, int iAdditionalModifier) const;
	int GetBoredom(bool bForceRecalc, int iAssumedExtraYieldRate = 0) const;
	int GetBoredomRaw(bool bForceRecalc, int iAssumedExtraYieldRate = 0) const;
	float GetCultureMedian(bool bForceRecalc, int iAdditionalModifier) const;
	int GetUnhappinessFromReligiousUnrest() const;
	int GetUnhappinessFromIsolation() const;

	int getJFDSpecialUnhappinessSources() const;

	int GetUnhappinessAggregated() const;
	int getUnhappinessFromSpecialists(int iSpecialists) const;

	CvString GetPotentialHappinessWithGrowth();
	int GetPotentialHappinessWithGrowthVal() const;

	CvString getPotentialUnhappinessWithGrowth();
	int getPotentialUnhappinessWithGrowthVal() const;

	int GetNumPillagedPlots() const;
	void SetNumPillagedPlots(int iValue);
	void ChangeNumPillagedPlots(int iValue);

	int GetGrowthFromEvent() const;
	void ChangeGrowthFromEvent(int iValue);

	int GetEventGPPFromSpecialists() const;
	void ChangeEventGPPFromSpecialists(int iValue);

	void AddEventGPPFromSpecialistsCounter(int iExpiryTurn, int iGPP);
	void UpdateEventGPPFromSpecialistsCounters();

	int GetGrowthFromTourism() const;
	void SetGrowthFromTourism(int iValue);
	void ChangeGrowthFromTourism(int iValue);
	void UpdateGrowthFromTourism();

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

	int GetExperiencePerGoldenAge() const;
	void ChangeExperiencePerGoldenAge(int iChange);
	int GetExperiencePerGoldenAgeCap() const;
	void ChangeExperiencePerGoldenAgeCap(int iChange);
	int GetWLTKDFromProject(ProjectTypes eProject) const;
	void ChangeWLTKDFromProject(ProjectTypes eProject, int iNumTurns);
	int GetExperienceFromPreviousGoldenAges() const;
	void ChangeExperienceFromPreviousGoldenAges(int iChange);

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
	void ChangeBorderObstacleCity(int iChange);
	int GetBorderObstacleLand() const;
	bool IsBorderObstacleLand() const;
	void SetBorderObstacleCity(int iValue);

	void ChangeBorderObstacleWater(int iChange);
	int GetBorderObstacleWater() const;
	bool IsBorderObstacleWater() const;
	void SetBorderObstacleWater(int iValue);

	void ChangeDeepWaterTileDamage(int iChange);
	int GetDeepWaterTileDamage() const;

	void ChangeNearbyMountains(int iChange);
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

	int GetResourceDemandedCounter() const;
	void SetResourceDemandedCounter(int iValue);
	void ChangeResourceDemandedCounter(int iChange);

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

	void ChangeNoStarvationNonSpecialist(int iValue);
	bool IsNoStarvationNonSpecialist() const;

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

	void UpdateSpecialReligionYields(YieldTypes eYield);
	int GetSpecialReligionYieldsTimes100(YieldTypes eIndex) const;
	void SetSpecialReligionYieldsTimes100(YieldTypes eIndex, int iChange);

#if defined(MOD_BALANCE_CORE)
	void updateEconomicValue();
	int getEconomicValue(PlayerTypes ePossibleOwner);
	void setEconomicValue(PlayerTypes ePossibleOwner, int iValue);

	// Instant Yield History
	void ChangeInstantYieldTotal(YieldTypes eYield, int iValue);
	int GetInstantYieldTotal(YieldTypes eYield);

	void ChangeUnitClassTrainingAllowed(UnitClassTypes eUnitClass, int iValue);
	int GetUnitClassTrainingAllowed(UnitClassTypes eUnitClass) const;
	map<UnitClassTypes, int> GetUnitClassTrainingAllowed() const;

	std::vector<CvPlot*>GetPlotsClaimedByBuilding(BuildingTypes eBuilding) const;
#endif

	int GetContestedPlotScore(PlayerTypes eOtherPlayer) const;

	// Yield calculations
	int getBaseYieldRateTimes100(const YieldTypes eYield, CvString* tooltipSink = NULL) const;
	int getBaseYieldRateModifier(YieldTypes eIndex, int iExtra = 0, CvString* toolTipSink = NULL) const;

	int GetPostModifierYieldRateTimes100(const YieldTypes eYield, bool bIgnoreTrade, bool bIgnoreProcess, bool bIgnoreFoodConsumption, bool bAssumeFoodProduction, CvString* tooltipSink = NULL) const;

	int getYieldRateTimes100(YieldTypes eYield, bool bIgnoreTrade, bool bIgnoreProcess, int iAssumeExtraModifier, bool bAssumeFoodProduction, bool bIgnoreFoodConsumption, bool bIgnoreGrowthMods, CvString* tooltipSink = NULL) const;
	int getYieldRateTimes100(YieldTypes eYield, bool bIgnoreTrade = false, bool bIgnoreProcess = false, bool bUseCachedValue = true, CvString* tooltipSink = NULL) const;

	int getFoodPerTurnBeforeConsumptionTimes100(bool bIgnoreProcess = false) const;

	int getProductionPerTurnForUnitTimes100(UnitTypes eUnit) const;
	int getProductionPerTurnForBuildingTimes100(BuildingTypes eBuilding) const;
	int getProductionPerTurnForProjectTimes100(ProjectTypes eProject) const;
	int getProductionPerTurnForProcessTimes100(ProcessTypes eProcess) const;
	int getRawProductionPerTurnTimes100() const;


	void UpdateCityYieldFromYield();
	int GetTotalYieldFromYieldTimes100(YieldTypes eYield) const;

	int GetBaseYieldRateFromGreatWorksTimes100(YieldTypes eIndex) const;

	int GetBaseYieldRateFromTerrain(YieldTypes eIndex) const;
	void ChangeBaseYieldRateFromTerrain(YieldTypes eIndex, int iChange);
	void SetBaseYieldRateFromTerrain(YieldTypes eIndex, int iValue);

	int GetBaseYieldRateFromBuildings(YieldTypes eIndex) const;
	void ChangeBaseYieldRateFromBuildings(YieldTypes eIndex, int iChange);

	int GetBaseYieldRateFromSpecialists(YieldTypes eIndex) const;
	void ChangeBaseYieldRateFromSpecialists(YieldTypes eIndex, int iChange);

	int GetBaseYieldRateFromMisc(YieldTypes eIndex) const;
	void ChangeBaseYieldRateFromMisc(YieldTypes eIndex, int iChange);

	int GetBaseYieldRatePermanentWLTKDTimes100(YieldTypes eIndex) const;
	void ChangeBaseYieldRatePermanentWLTKDTimes100(YieldTypes eIndex, int iChange);

	int GetBaseYieldRateFromProcessTimes100(YieldTypes eIndex) const;

	int GetBaseYieldRateFromLeague(YieldTypes eIndex) const;
	void ChangeBaseYieldRateFromLeague(YieldTypes eIndex, int iChange);

	int GetGrowthExtraYield(YieldTypes eIndex) const;
	void ChangeGrowthExtraYield(YieldTypes eIndex, int iChange);

	int GetYieldFromPassingTR(YieldTypes eIndex) const;
	void ChangeYieldFromPassingTR(YieldTypes eIndex, int iChange);

#if defined(MOD_BALANCE_CORE)
	int GetYieldFromVictory(YieldTypes eIndex) const;
	void ChangeYieldFromVictory(YieldTypes eIndex, int iChange);

	int GetYieldFromVictoryGlobal(YieldTypes eIndex) const;
	void ChangeYieldFromVictoryGlobal(YieldTypes eIndex, int iChange);

	int GetYieldFromVictoryGlobalEraScaling(YieldTypes eIndex) const;
	void ChangeYieldFromVictoryGlobalEraScaling(YieldTypes eIndex, int iChange);

	int GetYieldFromVictoryGlobalInGoldenAge(YieldTypes eIndex) const;
	void ChangeYieldFromVictoryGlobalInGoldenAge(YieldTypes eIndex, int iChange);

	int GetYieldFromVictoryGlobalInGoldenAgeEraScaling(YieldTypes eIndex) const;
	void ChangeYieldFromVictoryGlobalInGoldenAgeEraScaling(YieldTypes eIndex, int iChange);

	int GetYieldFromPillage(YieldTypes eIndex) const;
	void ChangeYieldFromPillage(YieldTypes eIndex, int iChange);

	int GetYieldFromPillageGlobal(YieldTypes eIndex) const;
	void ChangeYieldFromPillageGlobal(YieldTypes eIndex, int iChange);

	int GetYieldFromGoldenAgeStart(YieldTypes eIndex) const;
	void ChangeYieldFromGoldenAgeStart(YieldTypes eIndex, int iChange);

	int GetYieldChangePerGoldenAge(YieldTypes eIndex) const;
	void ChangeYieldChangePerGoldenAge(YieldTypes eIndex, int iChange);

	int GetYieldChangePerGoldenAgeCap(YieldTypes eIndex) const;
	void ChangeYieldChangePerGoldenAgeCap(YieldTypes eIndex, int iChange);

	int GetYieldFromPreviousGoldenAges(YieldTypes eIndex) const;
	void ChangeYieldFromPreviousGoldenAges(YieldTypes eIndex, int iChange);

	int GetGoldenAgeYieldMod(YieldTypes eIndex) const;
	void ChangeGoldenAgeYieldMod(YieldTypes eIndex, int iChange);

	int GetYieldChangesPerLocalTheme(YieldTypes eIndex) const;
	void ChangeYieldChangesPerLocalTheme(YieldTypes eIndex, int iChange);

	int GetYieldFromUnitGiftGlobal(YieldTypes eIndex) const;
	void ChangeYieldFromUnitGiftGlobal(YieldTypes eIndex, int iChange);

	int GetInstantYieldFromWLTKDStart(YieldTypes eIndex) const;
	void ChangeInstantYieldFromWLTKDStart(YieldTypes eIndex, int iChange);

	int GetYieldFromWLTKD(YieldTypes eIndex) const;
	void ChangeYieldFromWLTKD(YieldTypes eIndex, int iChange);

	int GetThemingYieldBonus(YieldTypes eIndex) const;
	void ChangeThemingYieldBonus(YieldTypes eIndex, int iChange);

	int GetYieldFromSpyAttack(YieldTypes eIndex) const;
	void ChangeYieldFromSpyAttack(YieldTypes eIndex, int iChange);

	int GetYieldFromSpyDefense(YieldTypes eIndex) const;
	void ChangeYieldFromSpyDefense(YieldTypes eIndex, int iChange);

	int GetYieldFromSpyIdentify(YieldTypes eIndex) const;
	void ChangeYieldFromSpyIdentify(YieldTypes eIndex, int iChange);

	int GetYieldFromSpyDefenseOrID(YieldTypes eIndex) const;
	void ChangeYieldFromSpyDefenseOrID(YieldTypes eIndex, int iChange);

	int GetYieldFromSpyRigElection(YieldTypes eIndex) const;
	void ChangeYieldFromSpyRigElection(YieldTypes eIndex, int iChange);

	int GetYieldChangesPerCityStrengthTimes100(YieldTypes eIndex) const;
	void ChangeYieldChangesPerCityStrengthTimes100(YieldTypes eIndex, int iChange);

	int GetYieldFromConstruction(YieldTypes eIndex) const;
	void ChangeYieldFromConstruction(YieldTypes eIndex, int iChange);

	int GetYieldFromTech(YieldTypes eIndex) const;
	void ChangeYieldFromTech(YieldTypes eIndex, int iChange);

	int GetYieldFromBirth(YieldTypes eIndex) const;
	void ChangeYieldFromBirth(YieldTypes eIndex, int iChange);
	int GetYieldFromBirthEraScaling(YieldTypes eIndex) const;
	void ChangeYieldFromBirthEraScaling(YieldTypes eIndex, int iChange);
	int GetYieldFromUnitProduction(YieldTypes eIndex) const;
	void ChangeYieldFromUnitProduction(YieldTypes eIndex, int iChange);
	int GetYieldFromBorderGrowth(YieldTypes eIndex) const;
	void ChangeYieldFromBorderGrowth(YieldTypes eIndex, int iChange);
	int GetYieldFromPolicyUnlock(YieldTypes eIndex) const;
	void ChangeYieldFromPolicyUnlock(YieldTypes eIndex, int iChange);
	int GetYieldFromPurchase(YieldTypes eIndex) const;
	void ChangeYieldFromPurchase(YieldTypes eIndex, int iChange);

	int GetYieldFromPurchaseGlobal(YieldTypes eIndex) const;
	void ChangeYieldFromPurchaseGlobal(YieldTypes eIndex, int iChange);

	int GetYieldFromFaithPurchase(YieldTypes eIndex) const;
	void ChangeYieldFromFaithPurchase(YieldTypes eIndex, int iChange);

	int GetYieldFromUnitLevelUp(YieldTypes eIndex) const;
	void ChangeYieldFromUnitLevelUp(YieldTypes eIndex, int iChange);

	int GetYieldFromUnitLevelUpGlobal(YieldTypes eIndex) const;
	void ChangeYieldFromUnitLevelUpGlobal(YieldTypes eIndex, int iChange);

	int GetYieldFromCombatExperienceTimes100(YieldTypes eIndex) const;
	void ChangeYieldFromCombatExperienceTimes100(YieldTypes eIndex, int iChange);

	int GetYieldPerAlly(YieldTypes eIndex) const;
	void ChangeYieldPerAlly(YieldTypes eIndex, int iChange);

	int GetYieldPerFriend(YieldTypes eIndex) const;
	void ChangeYieldPerFriend(YieldTypes eIndex, int iChange);

	int GetBuildingYieldFromYield(YieldTypes eIndex1, YieldTypes eIndex2) const;
	void ChangeBuildingYieldFromYield(YieldTypes eIndex, YieldTypes eIndex2, int iValue);

	int GetYieldFromInternationalTREnd(YieldTypes eIndex1) const;
	void ChangeYieldFromInternationalTREnd(YieldTypes eIndex1, int iChange);

	int GetYieldFromInternalTREnd(YieldTypes eIndex1) const;
	void ChangeYieldFromInternalTREnd(YieldTypes eIndex, int iChange);

	int GetYieldFromInternalTR(YieldTypes eIndex1) const;
	void ChangeYieldFromInternalTR(YieldTypes eIndex, int iChange);

	int GetYieldFromProcessModifier(YieldTypes eIndex1) const;
	void ChangeYieldFromProcessModifier(YieldTypes eIndex, int iChange);

	int GetYieldFromLongCount(YieldTypes eIndex1) const;
	void ChangeYieldFromLongCount(YieldTypes eIndex1, int iChange);


	int GetRealYieldFromYieldTimes100(YieldTypes eIndex1, YieldTypes eIndex2) const;
	void SetRealYieldFromYieldTimes100(YieldTypes eIndex1, YieldTypes eIndex2, int iValue);

	int GetYieldFromGPBirthScaledWithWriterBulb(YieldTypes eIndex1) const;
	void ChangeYieldFromGPBirthScaledWithWriterBulb(YieldTypes eIndex1, int iChange);

	int GetYieldFromGPBirthScaledWithArtistBulb(YieldTypes eIndex1) const;
	void ChangeYieldFromGPBirthScaledWithArtistBulb(YieldTypes eIndex1, int iChange);

	map<GreatPersonTypes, map<std::pair<YieldTypes, YieldTypes>, int>> GetYieldFromGPBirthScaledWithPerTurnYieldMap() const;
	int GetYieldFromGPBirthScaledWithPerTurnYield(GreatPersonTypes eGreatPerson, YieldTypes eYieldIn, YieldTypes eYieldOut) const;
	void ChangeYieldFromGPBirthScaledWithPerTurnYield(GreatPersonTypes eGreatPerson, std::pair<YieldTypes, YieldTypes> eYieldPair, int iChange);

	int GetSpecialistRateModifierFromBuildings(SpecialistTypes eSpecialist) const;
	void ChangeSpecialistRateModifierFromBuildings(SpecialistTypes eSpecialist, int iChange);
#endif

#if defined(MOD_BALANCE_CORE)
	int GetGreatPersonProgressFromConstruction(GreatPersonTypes eGreatPerson, EraTypes eEra) const;
	void ChangeGreatPersonProgressFromConstruction(GreatPersonTypes eGreatPerson, EraTypes eEra, int iChange);
#endif

	int GetEmpireSizeModifierReduction() const;
	void ChangeEmpireSizeModifierReduction(int iChange);

	int GetDistressFlatReduction() const;
	void ChangeDistressFlatReduction(int iChange);

	int GetPovertyFlatReduction() const;
	void ChangePovertyFlatReduction(int iChange);

	int GetIlliteracyFlatReduction() const;
	void ChangeIlliteracyFlatReduction(int iChange);

	int GetBoredomFlatReduction() const;
	void ChangeBoredomFlatReduction(int iChange);

	int GetReligiousUnrestFlatReduction() const;
	void ChangeReligiousUnrestFlatReduction(int iChange);

	int GetBasicNeedsMedianModifier() const;
	void ChangeBasicNeedsMedianModifier(int iChange);

	int GetGoldMedianModifier() const;
	void ChangeGoldMedianModifier(int iChange);

	int GetScienceMedianModifier() const;
	void ChangeScienceMedianModifier(int iChange);

	int GetCultureMedianModifier() const;
	void ChangeCultureMedianModifier(int iChange);

	int GetReligiousUnrestModifier() const;
	void ChangeReligiousUnrestModifier(int iChange);

	int GetBaseYieldRateFromReligionTimes100(YieldTypes eIndex) const;
	void ChangeBaseYieldRateFromReligion(YieldTypes eIndex, int iChange);

#if defined(MOD_BALANCE_CORE)
	int GetBaseYieldRateFromCSAlliance(YieldTypes eIndex) const;
	void ChangeBaseYieldRateFromCSAlliance(YieldTypes eIndex, int iChange);
	void SetBaseYieldRateFromCSAlliance(YieldTypes eIndex, int iValue);

	int GetEffectiveYieldRateFromCSAllianceTimes100(YieldTypes eIndex) const;

	int GetBaseYieldRateFromCSFriendship(YieldTypes eIndex) const;
	void ChangeBaseYieldRateFromCSFriendship(YieldTypes eIndex, int iChange);
	void SetBaseYieldRateFromCSFriendship(YieldTypes eIndex, int iValue);

	int GetEffectiveYieldRateFromCSFriendshipTimes100(YieldTypes eIndex) const;

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
	void SetYieldChangeFromCorporationFranchises(YieldTypes eIndex, int iTotal);

	int GetResourceQuantityPerXFranchises(ResourceTypes eResource) const;
	void ChangeResourceQuantityPerXFranchises(ResourceTypes eResource, int iChange);
	void SetResourceQuantityPerXFranchises(ResourceTypes eResource, int iValue);

	int GetResourceQuantityFromPOP(ResourceTypes eResource) const;
	void ChangeResourceQuantityFromPOP(ResourceTypes eResource, int iChange);
	void SetResourceQuantityFromPOP(ResourceTypes eResource, int iValue);

	int GetLandTourismBonus() const;
	void ChangeLandTourismBonus(int iChange);
	void SetLandTourismBonus(int iChange);

	int GetSeaTourismBonus() const;
	void ChangeSeaTourismBonus(int iChange);
	void SetSeaTourismBonus(int iChange);

	int GetSeaTourismFromEvent();
	int GetLandTourismFromEvent();

	void ChangeNumPreviousSpyMissions(int iChange);
	int GetNumPreviousSpyMissions() const;

	int CalculateCitySecurity(CvString* toolTipSink = NULL) const;

	int GetAlwaysHeal() const;
	void ChangeAlwaysHeal(int iChange);
	void SetAlwaysHeal(int iChange);

	void ChangeResourceDiversityModifier(int iChange);
	int GetResourceDiversityModifier() const;

	void ChangeNoUnhappfromXSpecialists(int iChange);
	int GetNoUnhappfromXSpecialists() const;

	bool isBorderCity() const;
	bool isBorderCity(vector<PlayerTypes>& vUnfriendlyMajors) const;
#endif

	void changeNukeInterceptionChance(int iNewValue);
	int getNukeInterceptionChance() const;
#if defined(MOD_BALANCE_CORE)
	void SetPurchased(BuildingClassTypes eBuildingClass, bool bValue);
	bool IsPurchased(BuildingClassTypes eBuildingClass);

	void SetBestForWonder(BuildingClassTypes eBuildingClass, bool bValue);
	bool IsBestForWonder(BuildingClassTypes eBuildingClass);
	int GetYieldRateFromBuildingsEraScalingTimes100(YieldTypes eIndex) const;
	void ChangeYieldRateFromBuildingsEraScalingTimes100(YieldTypes eIndex, int iChange);
#endif
	// END Base Yield

	int GetYieldPerPopTimes100(YieldTypes eIndex) const;
	void ChangeYieldPerPopTimes100(YieldTypes eIndex, int iChange);

	fraction GetYieldPerBuilding(YieldTypes eIndex) const;
	void ChangeYieldPerBuilding(YieldTypes eIndex, fraction iChange);

	fraction GetYieldPerTile(YieldTypes eIndex) const;
	void ChangeYieldPerTile(YieldTypes eIndex, fraction iChange);

	fraction GetYieldPerCityStateStrategicResource(YieldTypes eIndex) const;
	void ChangeYieldPerCityStateStrategicResource(YieldTypes eIndex, fraction iChange);

	int GetYieldPerPopInEmpireTimes100(YieldTypes eIndex) const;
	void ChangeYieldPerPopInEmpireTimes100(YieldTypes eIndex, int iChange);
	int GetYieldFromIndustrialCityConnection(YieldTypes eIndex) const;

	int GetTechEnhancedYields(TechTypes eTech, YieldTypes eYield) const;
	bool TechEnhancesAnyYield(TechTypes eTech) const;
	void ChangeTechEnhancedYields(TechTypes eTech, YieldTypes eYield, int iChange);

	std::map<int, std::map<int, int>> GetYieldsFromAccomplishmentsMap() const;
	int GetYieldsFromAccomplishments(AccomplishmentTypes eAccomplishment, YieldTypes eYield) const;
	void ChangeYieldsFromAccomplishments(AccomplishmentTypes eAccomplishment, YieldTypes eYield, int iChange);

	std::map<pair<GreatPersonTypes, EraTypes>, int> GetGreatPersonPointFromConstructionMap() const;
	int GetGreatPersonPointFromConstruction(GreatPersonTypes eGreatPerson, EraTypes eEra) const;
	void ChangeGreatPersonPointFromConstruction(pair<GreatPersonTypes, EraTypes> pGreatPersonEra, int iChange);

	int GetYieldPerReligionTimes100(YieldTypes eIndex) const;
	void ChangeYieldPerReligionTimes100(YieldTypes eIndex, int iChange);

	int getYieldRateModifier(YieldTypes eIndex) const;
	void changeYieldRateModifier(YieldTypes eIndex, int iChange);

	int GetLuxuryExtraYield(YieldTypes eIndex) const;
	void ChangeLuxuryExtraYield(YieldTypes eIndex, int iChange);

#if defined(MOD_BALANCE_CORE_POLICIES)
	int getReligionBuildingYieldRateModifier(BuildingClassTypes eBuilding, YieldTypes eYield)	const;
	void changeReligionBuildingYieldRateModifier(BuildingClassTypes eBuilding, YieldTypes eYield, int iChange);

	int getLocalBuildingClassYield(BuildingClassTypes eBuilding, YieldTypes eYield)	const;
	void changeLocalBuildingClassYield(BuildingClassTypes eBuilding, YieldTypes eYield, int iChange);

	void ChangeGreatWorkYieldChange(YieldTypes eIndex, int iChange);
	int GetGreatWorkYieldChange(YieldTypes eIndex) const;
#endif

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
	void SetRetroactivePromotion(PromotionTypes eIndex);
#endif

	int getSpecialistFreeExperience() const;
	void changeSpecialistFreeExperience(int iChange);

	int getAdjacentUnitsDefenseMod() const;

	void updateStrengthValue();
	int getStrengthValue(bool bForRangeStrike = false, bool bIgnoreBuildings = false, const CvUnit* pDefender = NULL) const;
	int GetPower() const;

	int getDamage() const;
	void setDamage(int iValue, bool noMessage = false);
	void changeDamage(int iChange);

	int GetDamagePermyriad(PlayerTypes ePlayer) const;
	void ChangeDamagePermyriad(PlayerTypes ePlayer, int iChange);
	void SetDamagePermyriad(PlayerTypes ePlayer, int iValue);

	int GetWarValue() const;

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

	int getGarrisonRangedAttackModifier() const;
	void changeGarrisonRangedAttackModifier(int iValue);

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

	bool CanBuyPlot(int iPlotX, int iPlotY, bool bIgnoreCost = false) const;
	bool CanBuyAnyPlot(void);
	CvPlot* GetNextBuyablePlot(bool bForPurchase);
	void GetBuyablePlotList(std::vector<int>& aiPlotList, bool bForPurchase, int nChoices = 3);
	int GetBuyPlotCost(int iPlotX, int iPlotY) const;
	void BuyPlot(int iPlotX, int iPlotY, bool bAutomaticPurchaseFromBuilding = false);
	void DoAcquirePlot(int iPlotX, int iPlotY);
	int GetBuyPlotScore(int& iBestX, int& iBestY);
	int GetIndividualPlotScore(const CvPlot* pPlot) const;

	int GetCheapestPlotInfluenceDistance() const;
	void SetCheapestPlotInfluenceDistance(int iValue);
	void DoUpdateCheapestPlotInfluenceDistance();
	int calculateInfluenceDistance(CvPlot* pDest, int iMaxRange) const;
	void UpdateYieldsFromExistingFriendsAndAllies(bool bRemove);

	// End plot acquisition

	bool isValidBuildingLocation(BuildingTypes eBuilding) const;

	void setThreatValue(int iThreatValue);
	int getThreatValue() const;

	void clearOrderQueue();
	void pushOrder(OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend, bool bRush = false);
	void popOrder(int iNum, bool bFinish = false, bool bChoose = false);
	void swapOrder(int iNum);
	bool hasOrder(OrderTypes eOrder, int iData1, int iData2) const;
	bool hasOrder(OrderTypes eOrder, int iData1) const;
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

	CvUnit* CreateUnit(UnitTypes eUnitType, UnitAITypes eAIType = NO_UNITAI, UnitCreationReason eReason = REASON_DEFAULT);
	bool CreateBuilding(BuildingTypes eBuildingType);
	bool CreateProject(ProjectTypes eProjectType);

	void changeProjectCount(ProjectTypes eProject, int iValue);
	int getProjectCount(ProjectTypes eProject) const;

	CvPlot* GetPlotForNewUnit(UnitTypes eUnitType, bool bAllowCenterPlot=true) const;
	bool CanPlaceUnitHere(UnitTypes eUnitType) const;
	bool IsCanPurchase(bool bTestPurchaseCost, bool bTestTrainable, UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectType, YieldTypes ePurchaseYield); //slow version
	bool IsCanPurchase(const std::vector<int>& vPreExistingBuildings, bool bTestPurchaseCost, bool bTestTrainable, UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectType, YieldTypes ePurchaseYield); //fast version
	CvUnit* PurchaseUnit(UnitTypes eUnitType, YieldTypes ePurchaseYield);
	bool PurchaseBuilding(BuildingTypes eBuildingType, YieldTypes ePurchaseYield);
	bool PurchaseProject(ProjectTypes eProjectType, YieldTypes ePurchaseYield);

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

	int getDamageReductionFlat() const;
	void changeDamageReductionFlat(int iChange);

	int GetMaxHitPoints() const;
	const CvSyncArchive<CvCity>& getSyncArchive() const;
	CvSyncArchive<CvCity>& getSyncArchive();
	std::string debugDump(const FAutoVariableBase&) const;
	std::string stackTraceRemark(const FAutoVariableBase&) const;

	bool IsBusy() const;
	// Combat related
	const CvUnit* getCombatUnit() const;
	CvUnit* getCombatUnit();
	void setCombatUnit(CvUnit* pCombatUnit, bool bAttacking = false);
	void clearCombat();
	bool isFighting() const;

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
	bool IsIndustrialConnectedToCapital() const { return IsIndustrialRouteToCapitalConnected(); }
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
	int CountNumWorkedFeature(FeatureTypes iFeatureType);
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
	int GetConnectionGoldTimes100() const;

#if defined(MOD_CORE_PER_TURN_DAMAGE)
	int addDamageReceivedThisTurn(int iDamage, CvUnit* pAttacker = NULL);
	void flipDamageReceivedPerTurn();
	bool isInDangerOfFalling(bool bExtraCareful=false) const;
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
	void SetSappedTurns(int iValue);
	void ChangeSappedTurns(int iValue);

	int GetBuildingProductionBlockedTurns() const;
	void SetBuildingProductionBlockedTurns(int iValue);
	void ChangeBuildingProductionBlockedTurns(int iValue);

	int GetNoTourismTurns() const;
	void SetNoTourismTurns(int iValue);
	void ChangeNoTourismTurns(int iValue);

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

	void ChangeVassalLevyEra(int iChange);
	int GetVassalLevyEra() const;

	void SpawnFreeUnit(UnitTypes eUnit);
	int SpawnPlayerUnitsNearby(const PlayerTypes ePlayer, const int iNumber, const bool bIncludeUUs = false, bool bIncludeShips = false, const bool bNoResource = false) const;

	bool SetNumFreeBuilding(const BuildingTypes eBuilding, const int iValue, const bool bRefund = true, const bool bValidate = true);
	BuildingTypes GetBuildingTypeFromClass(const BuildingClassTypes eBuildingClass, const bool bFallback = false) const;
	void AddFreeCapitalBuildings(const bool bRemoveFromCurrent = false);

	bool IsNukeKillable(int iNukeLevel);

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

	int m_iBaseGreatPeopleRate;
	int m_iGreatPeopleRateModifier;
	int m_iGPRateModifierPerMarriage;
	int m_iGPRateModifierPerLocalTheme;
	int m_iGPPOnCitizenBirth;
	int m_iJONSCultureStoredTimes100;
	int m_iJONSCultureLevel;
	std::vector<int> m_iaAddedYieldPerTurnFromTraits;
	std::vector<int> m_aiBaseYieldRateFromPolicies;
#if defined(MOD_BALANCE_CORE)
	int m_iAdditionalFood;
	int m_iCityBuildingBombardRange;
	int m_iCityIndirectFire;
	int m_iCityBuildingRangeStrikeModifier;
	int m_iGarrisonRangedAttackModifier;
#endif
	int m_iCultureRateModifier;
	int m_iNumWorldWonders;
	int m_iNumTeamWonders;
	int m_iNumNationalWonders;
	int m_iWonderProductionModifier;
	int m_iCapturePlunderModifier;
	int m_iDiplomatInfluenceBoost;
	int m_iBorderGrowthRateIncrease;
	int m_iPlotCultureCostModifier;
	int m_iPlotBuyCostModifier;
#if defined(MOD_BUILDINGS_CITY_WORKING)
	int m_iCityWorkingChange;
	int m_iCitySupplyModifier;
	int m_iCitySupplyFlat;
	int m_iDamageReductionFlat;
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
	int m_iFoodBonusPerCityMajorityFollower;
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
	int m_iStrengthValueRanged;
	int m_iDamage;
	int m_iThreatValue;
	int m_hGarrison;  // unused
	mutable int m_hGarrisonOverride; //only temporary, not serialized
	int m_iResourceDemanded;
	int m_iResourceDemandedCounter;
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
	int m_iSpySecurityModifier;
	int m_iSpySecurityModifierPerXPop;
	int m_iNumPreviousSpyMissions;
	fraction m_fDefensePerWonder;
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
	std::vector<int> m_aiBaseYieldRatePermanentWLTKDTimes100;
	std::vector<int> m_aiBaseYieldRateFromLeague;
	int m_iTotalScienceyAid;
	int m_iTotalArtsyAid;
	std::set<int> m_siPlots;
	std::set<int> m_siAccomplishmentsWithBonuses;
	std::vector<int> m_aiChangeGrowthExtraYield;
	std::vector<int> m_aiYieldFromPassingTR;
#if defined(MOD_BALANCE_CORE)
	int m_iHappinessFromEmpire;
	int m_iUnhappinessFromEmpire;
	int m_iCachedBasicNeedsMedian;
	int m_iCachedGoldMedian;
	int m_iCachedScienceMedian;
	int m_iCachedCultureMedian;
	int m_iCachedTechNeedModifier;
	int m_iCachedEmpireSizeModifier;
	int m_iYieldMediansCachedTurn;
	std::vector<int> m_aiNumProjects;
	std::vector<int> m_aiNumTimesAttackedThisTurn;
	std::vector<int> m_aiSpecialReligionYieldsTimes100;
	std::vector<int> m_aiYieldFromVictory;
	std::vector<int> m_aiYieldFromVictoryGlobal;
	std::vector<int> m_aiYieldFromVictoryGlobalEraScaling;
	std::vector<int> m_aiYieldFromVictoryGlobalInGoldenAge;
	std::vector<int> m_aiYieldFromVictoryGlobalInGoldenAgeEraScaling;
	std::vector<int> m_aiYieldFromPillage;
	std::vector<int> m_aiYieldFromPillageGlobal;
	std::vector<int> m_aiYieldFromGoldenAgeStart;
	std::vector<int> m_aiYieldChangePerGoldenAge;
	std::vector<int> m_aiYieldChangePerGoldenAgeCap;
	std::vector<int> m_aiYieldFromPreviousGoldenAges;
	std::vector<int> m_aiGoldenAgeYieldMod;
	std::vector<int> m_aiYieldChangesPerLocalTheme;
	std::vector<int> m_aiYieldFromUnitGiftGlobal;
	std::vector<int> m_aiYieldFromWLTKD;
	std::vector<int> m_aiInstantYieldFromWLTKDStart;
	std::vector<int> m_aiYieldFromConstruction;
	std::vector<int> m_aiYieldFromTech;
	std::vector<int> m_aiYieldFromBirth;
	std::vector<int> m_aiYieldFromBirthEraScaling;
	std::vector<int> m_aiYieldFromUnitProduction;
	std::vector<int> m_aiYieldFromBorderGrowth;
	std::vector<int> m_aiYieldFromPolicyUnlock;
	std::vector<int> m_aiYieldFromPurchase;
	std::vector<int> m_aiYieldFromPurchaseGlobal;
	std::vector<int> m_aiYieldFromFaithPurchase;
	std::vector<int> m_aiYieldFromUnitLevelUp;
	std::vector<int> m_aiYieldFromUnitLevelUpGlobal;
	std::vector<int> m_aiYieldFromCombatExperienceTimes100;
	std::vector<int> m_aiYieldPerAlly;
	std::vector<int> m_aiYieldPerFriend;
	std::vector<int> m_aiYieldFromInternalTREnd;
	std::vector<int> m_aiYieldFromInternationalTREnd;
	std::vector<int> m_aiYieldFromInternalTR;
	std::vector<int> m_aiYieldFromProcessModifier;
	std::vector<int> m_aiYieldFromLongCount;
	std::vector<int> m_aiYieldFromGPBirthScaledWithWriterBulb;
	std::vector<int> m_aiYieldFromGPBirthScaledWithArtistBulb;
	map<GreatPersonTypes, map<std::pair<YieldTypes, YieldTypes>, int>> m_miYieldFromGPBirthScaledWithPerTurnYield;
	std::vector<int> m_aiSpecialistRateModifierFromBuildings;
	std::vector<int> m_aiThemingYieldBonus;
	std::vector<int> m_aiYieldFromSpyAttack;
	std::vector<int> m_aiYieldFromSpyDefense;
	std::vector<int> m_aiYieldFromSpyIdentify;
	std::vector<int> m_aiYieldFromSpyDefenseOrID;
	std::vector<int> m_aiYieldFromSpyRigElection;
	std::vector<int> m_aiYieldChangesPerCityStrengthTimes100;
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
	int m_iBorderObstacleCity;
	int m_iBorderObstacleWater;
	int m_iDeepWaterTileDamage;
	int m_iNumNearbyMountains;
	int m_iLocalUnhappinessMod;
	int m_iExperiencePerGoldenAge;
	int m_iExperiencePerGoldenAgeCap;
	int m_iExperienceFromPreviousGoldenAges;
	bool m_bNoWarmonger;
	int m_iNoStarvationNonSpecialist;
	int m_iEmpireSizeModifierReduction;
	int m_iDistressFlatReduction;
	int m_iPovertyFlatReduction;
	int m_iIlliteracyFlatReduction;
	int m_iBoredomFlatReduction;
	int m_iReligiousUnrestFlatReduction;
	int m_iBasicNeedsMedianModifier;
	int m_iGoldMedianModifier;
	int m_iScienceMedianModifier;
	int m_iCultureMedianModifier;
	int m_iReligiousUnrestModifier;
	int m_iTradeRouteSeaDistanceModifier;
	int m_iTradeRouteLandDistanceModifier;
	int m_iNukeInterceptionChance;
	std::vector<int> m_aiEconomicValue;
	std::tr1::unordered_map<YieldTypes, int> m_miInstantYieldsTotal;
#endif
	map<UnitClassTypes, int> m_miUnitClassTrainingAllowed;
	map<ProjectTypes, int> m_miWLTKDFromProject;
	std::vector<int> m_aiBaseYieldRateFromReligion;
#if defined(MOD_BALANCE_CORE)
	std::vector<int> m_aiBaseYieldRateFromCSAlliance;
	std::vector<int> m_aiBaseYieldRateFromCSFriendship;
	std::vector<int> m_aiYieldFromMinors;
	std::vector<int> m_aiResourceQuantityPerXFranchises;
	std::vector<int> m_aiYieldChangeFromCorporationFranchises;
	std::vector<int> m_aiResourceQuantityFromPOP;
	int m_iLandTourismBonus;
	int m_iSeaTourismBonus;
	int m_iAlwaysHeal;
	int m_iResourceDiversityModifier;
	int m_iNoUnhappfromXSpecialists;
	std::vector<int> m_aiGreatWorkYieldChange;
	std::vector<int> m_aiDamagePermyriad;
#endif
	std::vector<int> m_aiYieldRateModifier;
	std::vector<int> m_aiLuxuryExtraYield;
	std::vector<int> m_aiYieldPerPop;
	std::vector<int> m_aiYieldRateFromBuildingsEraScalingTimes100;
	std::vector<fraction> m_afYieldPerBuilding;
	std::vector<fraction> m_afYieldPerTile;
	std::vector<fraction> m_afYieldPerCityStateStrategicResource;
#if defined(MOD_BALANCE_CORE)
	std::map<int, int> m_aiYieldPerPopInEmpire;
#endif
	std::map<int, std::map<int, int>> m_miTechEnhancedYields;
	std::map<int, std::map<int, int>> m_miYieldsFromAccomplishments;
	std::map<pair<GreatPersonTypes, EraTypes>, int> m_miGreatPersonPointFromConstruction;
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
	std::vector<int> m_paiNumImprovementWorked;
	std::vector<int> m_paiImprovementCount;
	std::vector<int> m_paiYieldFromUnimprovedFeatures;
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
	std::vector<int> m_paiUnitProduction;
	std::vector<int> m_paiUnitProductionTime;
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
	int m_iEventGPPFromSpecialists;
	std::vector<int> m_vEventGPPFromSpecialistsExpiryTurns;
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
	int m_iBuildingProductionBlockedTurns;
	int m_iNoTourismTurns;
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

	int m_inumHiddenBuildings;
	std::vector<bool> m_abIsBuildingHidden;

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
	std::vector<int> m_aiBuildingCostInvestmentReduction;
	std::vector<bool> m_abUnitInvestment;
	std::vector<int> m_aiUnitCostInvestmentReduction;
	std::vector<bool> m_abBuildingConstructed;
#endif

	int m_iVassalLevyEra;

	//cache for great work yields, they are need often during citizen re-assignment but they don't change
	mutable vector<int> m_GwYieldCache; //not serialized

	IDInfo m_combatUnit;		// The unit the city is in combat with

	void doGrowth();
	void doProduction(bool bAllowNoProduction);
	void doProcess();
	void doDecay();
	void doMeltdown();
	void doUnitCompletionYields(CvUnit* pUnit, UnitCreationReason eReason);
	bool doCheckProduction();
	bool CrosscheckYieldsFromMinors();

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
SYNC_ARCHIVE_VAR(int, m_iBaseGreatPeopleRate)
SYNC_ARCHIVE_VAR(int, m_iGreatPeopleRateModifier)
SYNC_ARCHIVE_VAR(int, m_iGPRateModifierPerMarriage)
SYNC_ARCHIVE_VAR(int, m_iGPRateModifierPerLocalTheme)
SYNC_ARCHIVE_VAR(int, m_iGPPOnCitizenBirth)
SYNC_ARCHIVE_VAR(int, m_iJONSCultureStoredTimes100)
SYNC_ARCHIVE_VAR(int, m_iJONSCultureLevel)
SYNC_ARCHIVE_VAR(std::vector<int>, m_iaAddedYieldPerTurnFromTraits)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiBaseYieldRateFromPolicies)
SYNC_ARCHIVE_VAR(int, m_iAdditionalFood)
SYNC_ARCHIVE_VAR(int, m_iCityBuildingBombardRange)
SYNC_ARCHIVE_VAR(int, m_iCityIndirectFire)
SYNC_ARCHIVE_VAR(int, m_iCityBuildingRangeStrikeModifier)
SYNC_ARCHIVE_VAR(int, m_iGarrisonRangedAttackModifier)
SYNC_ARCHIVE_VAR(int, m_iCultureRateModifier)
SYNC_ARCHIVE_VAR(int, m_iNumWorldWonders)
SYNC_ARCHIVE_VAR(int, m_iNumTeamWonders)
SYNC_ARCHIVE_VAR(int, m_iNumNationalWonders)
SYNC_ARCHIVE_VAR(int, m_iWonderProductionModifier)
SYNC_ARCHIVE_VAR(int, m_iCapturePlunderModifier)
SYNC_ARCHIVE_VAR(int, m_iDiplomatInfluenceBoost)
SYNC_ARCHIVE_VAR(int, m_iBorderGrowthRateIncrease)
SYNC_ARCHIVE_VAR(int, m_iPlotCultureCostModifier)
SYNC_ARCHIVE_VAR(int, m_iPlotBuyCostModifier)
SYNC_ARCHIVE_VAR(int, m_iCityWorkingChange)
SYNC_ARCHIVE_VAR(int, m_iCitySupplyModifier)
SYNC_ARCHIVE_VAR(int, m_iCitySupplyFlat)
SYNC_ARCHIVE_VAR(int, m_iDamageReductionFlat)
SYNC_ARCHIVE_VAR(bool, m_bAllowsProductionTradeRoutes)
SYNC_ARCHIVE_VAR(bool, m_bAllowsFoodTradeRoutes)
SYNC_ARCHIVE_VAR(bool, m_bAllowPuppetPurchase)
SYNC_ARCHIVE_VAR(int, m_iCityAutomatonWorkersChange)
SYNC_ARCHIVE_VAR(int, m_iMaintenance)
SYNC_ARCHIVE_VAR(int, m_iHealRate)
SYNC_ARCHIVE_VAR(int, m_iNoOccupiedUnhappinessCount)
SYNC_ARCHIVE_VAR(int, m_iFoodBonusPerCityMajorityFollower)
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
SYNC_ARCHIVE_VAR(int, m_iStrengthValueRanged)
SYNC_ARCHIVE_VAR(int, m_iDamage)
SYNC_ARCHIVE_VAR(int, m_iThreatValue)
SYNC_ARCHIVE_VAR(int, m_hGarrison)
SYNC_ARCHIVE_VAR(int, m_iResourceDemanded)
SYNC_ARCHIVE_VAR(int, m_iResourceDemandedCounter)
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
SYNC_ARCHIVE_VAR(int, m_iSpySecurityModifier)
SYNC_ARCHIVE_VAR(int, m_iSpySecurityModifierPerXPop)
SYNC_ARCHIVE_VAR(int, m_iNumPreviousSpyMissions)
SYNC_ARCHIVE_VAR(fraction, m_fDefensePerWonder)
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
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiBaseYieldRatePermanentWLTKDTimes100)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiBaseYieldRateFromLeague)
SYNC_ARCHIVE_VAR(std::set<int>, m_siPlots)
SYNC_ARCHIVE_VAR(std::set<int>, m_siAccomplishmentsWithBonuses)
SYNC_ARCHIVE_VAR(int, m_iTotalScienceyAid)
SYNC_ARCHIVE_VAR(int, m_iTotalArtsyAid)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiChangeGrowthExtraYield)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromPassingTR)
SYNC_ARCHIVE_VAR(int, m_iHappinessFromEmpire)
SYNC_ARCHIVE_VAR(int, m_iUnhappinessFromEmpire)
SYNC_ARCHIVE_VAR(int, m_iCachedBasicNeedsMedian)
SYNC_ARCHIVE_VAR(int, m_iCachedGoldMedian)
SYNC_ARCHIVE_VAR(int, m_iCachedScienceMedian)
SYNC_ARCHIVE_VAR(int, m_iCachedCultureMedian)
SYNC_ARCHIVE_VAR(int, m_iCachedTechNeedModifier)
SYNC_ARCHIVE_VAR(int, m_iCachedEmpireSizeModifier)
SYNC_ARCHIVE_VAR(int, m_iYieldMediansCachedTurn)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiNumProjects)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiNumTimesAttackedThisTurn)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiSpecialReligionYieldsTimes100)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromVictory)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromVictoryGlobal)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromVictoryGlobalEraScaling)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromVictoryGlobalInGoldenAge)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromVictoryGlobalInGoldenAgeEraScaling)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromPillage)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromPillageGlobal)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromGoldenAgeStart)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldChangePerGoldenAge)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldChangePerGoldenAgeCap)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromPreviousGoldenAges)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiGoldenAgeYieldMod)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldChangesPerLocalTheme)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromUnitGiftGlobal)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromWLTKD)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiInstantYieldFromWLTKDStart)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromConstruction)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromTech)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromBirth)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromBirthEraScaling)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromUnitProduction)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromBorderGrowth)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromPolicyUnlock)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromPurchase)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromPurchaseGlobal)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromFaithPurchase)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromUnitLevelUp)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromUnitLevelUpGlobal)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromCombatExperienceTimes100)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldPerAlly)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldPerFriend)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromInternalTREnd)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromInternationalTREnd)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromInternalTR)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromProcessModifier)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromLongCount)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromGPBirthScaledWithWriterBulb)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromGPBirthScaledWithArtistBulb)
SYNC_ARCHIVE_VAR(SYNC_ARCHIVE_VAR_TYPE(std::map<GreatPersonTypes, std::map<std::pair<YieldTypes, YieldTypes>, int>>), m_miYieldFromGPBirthScaledWithPerTurnYield)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiSpecialistRateModifierFromBuildings)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiThemingYieldBonus)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromSpyAttack)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromSpyDefense)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromSpyIdentify)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromSpyDefenseOrID)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromSpyRigElection)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldChangesPerCityStrengthTimes100)
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
SYNC_ARCHIVE_VAR(int, m_iBorderObstacleCity)
SYNC_ARCHIVE_VAR(int, m_iBorderObstacleWater)
SYNC_ARCHIVE_VAR(int, m_iDeepWaterTileDamage)
SYNC_ARCHIVE_VAR(int, m_iNumNearbyMountains)
SYNC_ARCHIVE_VAR(int, m_iLocalUnhappinessMod)
SYNC_ARCHIVE_VAR(int, m_iExperiencePerGoldenAge)
SYNC_ARCHIVE_VAR(int, m_iExperiencePerGoldenAgeCap)
SYNC_ARCHIVE_VAR(int, m_iExperienceFromPreviousGoldenAges)
SYNC_ARCHIVE_VAR(bool, m_bNoWarmonger)
SYNC_ARCHIVE_VAR(int, m_iNoStarvationNonSpecialist)
SYNC_ARCHIVE_VAR(int, m_iEmpireSizeModifierReduction)
SYNC_ARCHIVE_VAR(int, m_iDistressFlatReduction)
SYNC_ARCHIVE_VAR(int, m_iPovertyFlatReduction)
SYNC_ARCHIVE_VAR(int, m_iIlliteracyFlatReduction)
SYNC_ARCHIVE_VAR(int, m_iBoredomFlatReduction)
SYNC_ARCHIVE_VAR(int, m_iReligiousUnrestFlatReduction)
SYNC_ARCHIVE_VAR(int, m_iBasicNeedsMedianModifier)
SYNC_ARCHIVE_VAR(int, m_iGoldMedianModifier)
SYNC_ARCHIVE_VAR(int, m_iScienceMedianModifier)
SYNC_ARCHIVE_VAR(int, m_iCultureMedianModifier)
SYNC_ARCHIVE_VAR(int, m_iReligiousUnrestModifier)
SYNC_ARCHIVE_VAR(int, m_iTradeRouteSeaDistanceModifier)
SYNC_ARCHIVE_VAR(int, m_iTradeRouteLandDistanceModifier)
SYNC_ARCHIVE_VAR(int, m_iNukeInterceptionChance)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiEconomicValue)
SYNC_ARCHIVE_VAR(SYNC_ARCHIVE_VAR_TYPE(map<UnitClassTypes, int>), m_miUnitClassTrainingAllowed)
SYNC_ARCHIVE_VAR(SYNC_ARCHIVE_VAR_TYPE(map<ProjectTypes, int>), m_miWLTKDFromProject)
SYNC_ARCHIVE_VAR(SYNC_ARCHIVE_VAR_TYPE(std::tr1::unordered_map<YieldTypes, int>), m_miInstantYieldsTotal)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiBaseYieldRateFromReligion)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiBaseYieldRateFromCSAlliance)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiBaseYieldRateFromCSFriendship)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromMinors)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiResourceQuantityPerXFranchises)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldChangeFromCorporationFranchises)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiResourceQuantityFromPOP)
SYNC_ARCHIVE_VAR(int, m_iLandTourismBonus)
SYNC_ARCHIVE_VAR(int, m_iSeaTourismBonus)
SYNC_ARCHIVE_VAR(int, m_iAlwaysHeal)
SYNC_ARCHIVE_VAR(int, m_iResourceDiversityModifier)
SYNC_ARCHIVE_VAR(int, m_iNoUnhappfromXSpecialists)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiGreatWorkYieldChange)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiDamagePermyriad)
SYNC_ARCHIVE_VAR(SYNC_ARCHIVE_VAR_TYPE(std::map<int, std::map<int, int>>), m_miTechEnhancedYields)
SYNC_ARCHIVE_VAR(SYNC_ARCHIVE_VAR_TYPE(std::map<int, std::map<int, int>>), m_miYieldsFromAccomplishments)
SYNC_ARCHIVE_VAR(SYNC_ARCHIVE_VAR_TYPE(std::map<pair<GreatPersonTypes, EraTypes>, int>), m_miGreatPersonPointFromConstruction)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldRateModifier)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiLuxuryExtraYield)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldPerPop)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldRateFromBuildingsEraScalingTimes100)
SYNC_ARCHIVE_VAR(std::vector<fraction>, m_afYieldPerBuilding)
SYNC_ARCHIVE_VAR(std::vector<fraction>, m_afYieldPerTile)
SYNC_ARCHIVE_VAR(std::vector<fraction>, m_afYieldPerCityStateStrategicResource)
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
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiNumImprovementWorked)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiImprovementCount)
SYNC_ARCHIVE_VAR(CvString, m_strScriptData)
SYNC_ARCHIVE_VAR(int, m_iDamageTakenThisTurn)
SYNC_ARCHIVE_VAR(int, m_iDamageTakenLastTurn)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiNoResource)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiFreeResource)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiNumResourcesLocal)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiNumUnimprovedResourcesLocal)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiProjectProduction)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiUnitProduction)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiUnitProductionTime)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiUnitCombatFreeExperience)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiUnitCombatProductionModifier)
SYNC_ARCHIVE_VAR(SYNC_ARCHIVE_VAR_TYPE(std::map<PromotionTypes, int>), m_paiFreePromotionCount)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiBuildingClassCulture)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiHurryModifier)
SYNC_ARCHIVE_VAR(int, m_iHappinessDelta)
SYNC_ARCHIVE_VAR(int, m_iPillagedPlots)
SYNC_ARCHIVE_VAR(int, m_iGrowthEvent)
SYNC_ARCHIVE_VAR(int, m_iEventGPPFromSpecialists)
SYNC_ARCHIVE_VAR(std::vector<int>, m_vEventGPPFromSpecialistsExpiryTurns)
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
SYNC_ARCHIVE_VAR(int, m_iBuildingProductionBlockedTurns)
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
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiBuildingCostInvestmentReduction)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abUnitInvestment)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiUnitCostInvestmentReduction)
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
			m_pCity->OverrideGarrison(NULL);
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
