/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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
#include "FAutoVector.h"

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

	void updateYield();

	bool IsIndustrialRouteToCapital() const;
	void SetIndustrialRouteToCapital(bool bValue);
	void DoUpdateIndustrialRouteToCapital();

	void SetRouteToCapitalConnected(bool bValue);
#if defined(MOD_API_EXTENSIONS)
	bool IsRouteToCapitalConnected(void) const;
#else
	bool IsRouteToCapitalConnected(void);
#endif

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
	int GetNumResourceLocal(ResourceTypes eResource, bool bImproved = false);
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
	void addProductionExperience(CvUnit* pUnit, bool bConscript = false);

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
	void processSpecialist(SpecialistTypes eSpecialist, int iChange);

#if defined(MOD_BALANCE_CORE)
	void UpdateReligion(ReligionTypes eNewMajority, bool bRecalcPlotYields=true);
#else
	void UpdateReligion(ReligionTypes eNewMajority);
#endif

	int GetCultureFromSpecialist(SpecialistTypes eSpecialist) const;

	CvHandicapInfo& getHandicapInfo() const;
	HandicapTypes getHandicapType() const;

	CvCivilizationInfo& getCivilizationInfo() const;
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
	bool IsEverCapital() const;
	void SetEverCapital(bool bValue);

	bool isCoastal(int iMinWaterSize = -1) const;
#if defined(MOD_API_EXTENSIONS)
	bool isAddsFreshWater() const;
#endif
#if defined(MOD_BALANCE_CORE)
	int GetUnitPurchaseCooldown() const;
	void SetUnitPurchaseCooldown(int iValue);
	void ChangeUnitPurchaseCooldown(int iValue);
	int GetBuildingPurchaseCooldown() const;
	void SetBuildingPurchaseCooldown(int iValue);
	void ChangeBuildingPurchaseCooldown(int iValue);
	void CheckForOperationUnits();
	void DoSellBuilding();
	void SetTraded(PlayerTypes ePlayer, bool bValue);
	bool IsTraded(PlayerTypes ePlayer);
#endif
	int foodConsumption(bool bNoAngry = false, int iExtra = 0) const;
	int foodDifference(bool bBottom = true) const;
	int foodDifferenceTimes100(bool bBottom = true, CvString* toolTipSink = NULL) const;
	int growthThreshold() const;

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
	CvArea* area() const;
	CvArea* waterArea() const;

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

	int getJONSCulturePerTurn() const;

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

	int GetFaithPerTurn() const;
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

	int getNukeModifier() const;
	void changeNukeModifier(int iChange);

	int GetTradeRouteTargetBonus() const;
	void ChangeTradeRouteTargetBonus(int iChange);

	int GetTradeRouteRecipientBonus() const;
	void ChangeTradeRouteRecipientBonus(int iChange);

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
	int getHappinessDelta() const;
	int getThresholdSubtractions(YieldTypes eYield, int iMod = 0) const;
	int getThresholdAdditions() const;
	int getUnhappyCitizenCount() const;
	void setUnhappyCitizenCount(int iNewValue);
	void changeUnhappyCitizenCount(int iChange);

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
	void ChangeWeLoveTheKingDayCounter(int iChange);

	int GetLastTurnGarrisonAssigned() const;
	void SetLastTurnGarrisonAssigned(int iValue);

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
	int GetYieldPerTurnFromReligion(ReligionTypes eReligion, YieldTypes eYield) const;
#endif

	int getBaseYieldRateModifier(YieldTypes eIndex, int iExtra = 0, CvString* toolTipSink = NULL) const;
	int getYieldRate(YieldTypes eIndex, bool bIgnoreTrade) const;
	int getYieldRateTimes100(YieldTypes eIndex, bool bIgnoreTrade) const;
#if defined(MOD_PROCESS_STOCKPILE)
	int getBasicYieldRateTimes100(YieldTypes eIndex, bool bIgnoreTrade) const;
#endif

#if defined(MOD_BALANCE_CORE)
	int getEconomicValue(PlayerTypes ePossibleNewOwner = NO_PLAYER, int iNumTurnsForDepreciation = 100) const;
#endif
#if defined(MOD_BALANCE_CORE_SPIES)
	void SetRank(int iRank);
	int GetRank() const;

	void SetTurnsSinceLastRankMessage(int iTurns);
	void ChangeTurnsSinceLastRankMessage(int iTurns);
	int GetTurnsSinceLastRankMessage() const;

	void DoRankIncreaseWarning(int iRank);
	void SetEspionageRanking(int iPotential);
#endif
	// Base Yield
	int getBaseYieldRate(YieldTypes eIndex) const;

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

	int GetGoldenAgeYieldMod(YieldTypes eIndex) const;
	void ChangeGoldenAgeYieldMod(YieldTypes eIndex, int iChange);

	int GetYieldFromWLTKD(YieldTypes eIndex) const;
	void ChangeYieldFromWLTKD(YieldTypes eIndex, int iChange);

	int GetThemingYieldBonus(YieldTypes eIndex) const;
	void ChangeThemingYieldBonus(YieldTypes eIndex, int iChange);

	int GetYieldFromConstruction(YieldTypes eIndex) const;
	void ChangeYieldFromConstruction(YieldTypes eIndex, int iChange);

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

	int GetPovertyUnhappinessGlobal() const;
	void ChangePovertyUnhappinessGlobal(int iChange);

	int GetDefenseUnhappinessGlobal() const;
	void ChangeDefenseUnhappinessGlobal(int iChange);

	int GetUnculturedUnhappinessGlobal() const;
	void ChangeUnculturedUnhappinessGlobal(int iChange);

	int GetIlliteracyUnhappinessGlobal() const;
	void ChangeIlliteracyUnhappinessGlobal(int iChange);

	int GetMinorityUnhappinessGlobal() const;
	void ChangeMinorityUnhappinessGlobal(int iChange);
#endif

	int GetBaseYieldRateFromReligion(YieldTypes eIndex) const;
	void ChangeBaseYieldRateFromReligion(YieldTypes eIndex, int iChange);

#if defined(MOD_BALANCE_CORE)
	int GetBaseYieldRateFromCSAlliance(YieldTypes eIndex) const;
	void ChangeBaseYieldRateFromCSAlliance(YieldTypes eIndex, int iChange);
	void SetBaseYieldRateFromCSAlliance(YieldTypes eIndex, int iValue);

	int GetReligiousTradeModifier() const;
	void ChangeReligiousTradeModifier(int iChange);
	void SetReligiousTradeModifier(int iValue);

	int GetFreeBuildingTradeTargetCity() const;
	void ChangeFreeBuildingTradeTargetCity(int iChange);
	void SetFreeBuildingTradeTargetCity(int iValue);

	int GetCorporationYieldChange(YieldTypes eIndex) const;
	void ChangeCorporationYieldChange(YieldTypes eIndex, int iChange);
	void SetCorporationYieldChange(YieldTypes eIndex, int iValue);

	int GetCorporationYieldModChange(YieldTypes eIndex) const;
	void ChangeCorporationYieldModChange(YieldTypes eIndex, int iChange);
	void SetCorporationYieldModChange(YieldTypes eIndex, int iValue);

	int GetCorporationGPChange() const;
	void ChangeCorporationGPChange(int iChange);
	void SetCorporationGPChange(int iValue);

	bool IsFranchised(PlayerTypes ePlayer);
	void SetFranchised(PlayerTypes ePlayer, bool bValue);

	bool HasOffice();
	void SetHasOffice(bool bValue);

	int GetCorporationResourceQuantity(ResourceTypes eResource) const;
	void ChangeCorporationResourceQuantity(ResourceTypes eResource, int iChange);
	void SetCorporationResourceQuantity(ResourceTypes eResource, int iValue);

	int GetLandTourismBonus() const;
	void ChangeLandTourismBonus(int iChange);
	void SetLandTourismBonus(int iValue);

	int GetSeaTourismBonus() const;
	void ChangeSeaTourismBonus(int iChange);
	void SetSeaTourismBonus(int iValue);

	int GetAlwaysHeal() const;
	void ChangeAlwaysHeal(int iChange);
	void SetAlwaysHeal(int iValue);

	bool IsBastion() const;
	void SetBastion(bool bValue);
	void TestBastion();
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
#if defined(MOD_BALANCE_CORE)
	CvPlot* GetNextBuyablePlot(bool bForPurchase);
#else
	CvPlot* GetNextBuyablePlot();
#endif
#if defined(MOD_BALANCE_CORE)
	void GetBuyablePlotList(std::vector<int>& aiPlotList, bool bForPurchase);
#else
	void GetBuyablePlotList(std::vector<int>& aiPlotList);
#endif
	int GetBuyPlotCost(int iPlotX, int iPlotY) const;
	void BuyPlot(int iPlotX, int iPlotY);
	void DoAcquirePlot(int iPlotX, int iPlotY);
	int GetBuyPlotScore(int& iBestX, int& iBestY);
	int GetIndividualPlotScore(const CvPlot* pPlot) const;

	int GetCheapestPlotInfluence() const;
	void SetCheapestPlotInfluence(int iValue);
	void DoUpdateCheapestPlotInfluence();

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

	int CreateUnit(UnitTypes eUnitType, UnitAITypes eAIType = NO_UNITAI, bool bUseToSatisfyOperation=true);
	bool CreateBuilding(BuildingTypes eBuildType);
	bool CreateProject(ProjectTypes eProjectType);

	bool CanPlaceUnitHere(UnitTypes eUnitType);
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
	virtual void AI_chooseProduction(bool bInterruptWonders) = 0;
	virtual bool AI_isChooseProductionDirty() = 0;
	virtual void AI_setChooseProductionDirty(bool bNewValue) = 0;

	virtual int AI_GetNumPlotsAcquiredByOtherPlayer(PlayerTypes ePlayer) const = 0;
	virtual void AI_ChangeNumPlotsAcquiredByOtherPlayer(PlayerTypes ePlayer, int iChange) = 0;

	void invalidatePopulationRankCache();
	void invalidateYieldRankCache(YieldTypes eYield = NO_YIELD);

	bool CommitToBuildingUnitForOperation();
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
	bool IsConnectedToCapital() const;
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
	int CountNumWorkedImprovement(ImprovementTypes eImprovement);
	int CountNumWorkedResource(ResourceTypes eResource);
	int CountNumImprovement(ImprovementTypes eImprovement);
	int CountNumWorkedRiverTiles(TerrainTypes eTerrain = NO_TERRAIN);
#endif

#if defined(MOD_CORE_PER_TURN_DAMAGE)
	int addDamageReceivedThisTurn(int iDamage);
	void flipDamageReceivedPerTurn();
	bool isInDangerOfFalling() const;
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
#endif
	FAutoVariable<int, CvCity> m_iMaintenance;
	FAutoVariable<int, CvCity> m_iHealRate;
	FAutoVariable<int, CvCity> m_iNoOccupiedUnhappinessCount;
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
	FAutoVariable<int, CvCity> m_iCheapestPlotInfluence;
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
	FAutoVariable<bool, CvCity> m_bEverCapital;
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
	FAutoVariable<std::vector<int>, CvCity> m_aiChangeYieldFromVictory;
	FAutoVariable<std::vector<int>, CvCity> m_aiGoldenAgeYieldMod;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldFromWLTKD;
	FAutoVariable<std::vector<int>, CvCity> m_aiYieldFromConstruction;
	FAutoVariable<std::vector<int>, CvCity> m_aiSpecialistRateModifier;
	FAutoVariable<std::vector<int>, CvCity> m_aiThemingYieldBonus;
	FAutoVariable<std::vector<int>, CvCity> m_aiNumTimesOwned;
	FAutoVariable<int, CvCity> m_iUnhappyCitizen;
	FAutoVariable<int, CvCity> m_iUnitPurchaseCooldown;
	FAutoVariable<int, CvCity> m_iBuildingPurchaseCooldown;
	FAutoVariable<int, CvCity> m_iReligiousTradeModifier;
	FAutoVariable<int, CvCity> m_iFreeBuildingTradeTargetCity;
	FAutoVariable<int, CvCity> m_iBaseTourism;
	FAutoVariable<int, CvCity> m_iBaseTourismBeforeModifiers;
	FAutoVariable<int, CvCity> m_iBorderObstacleCity;
	FAutoVariable<int, CvCity> m_iBorderObstacleWater;
	FAutoVariable<int, CvCity> m_iNumNearbyMountains;
	FAutoVariable<int, CvCity> m_iLocalUnhappinessMod;
	FAutoVariable<bool, CvCity> m_bNoWarmonger;
#endif
#if defined(MOD_BALANCE_CORE)
	FAutoVariable<int, CvCity> m_iBlockBuildingDestruction;
	FAutoVariable<int, CvCity> m_iBlockWWDestruction;
	FAutoVariable<int, CvCity> m_iBlockUDestruction;
	FAutoVariable<int, CvCity> m_iBlockGPDestruction;
	FAutoVariable<int, CvCity> m_iBlockRebellion;
	FAutoVariable<int, CvCity> m_iBlockUnrest;
	FAutoVariable<int, CvCity> m_iBlockScience;
	FAutoVariable<int, CvCity> m_iBlockGold;
#endif
#if defined(MOD_BALANCE_CORE_SPIES)
	FAutoVariable<int, CvCity> m_iCityRank;
	FAutoVariable<int, CvCity> m_iTurnsSinceRankAnnouncement;
#endif
#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
	FAutoVariable<int, CvCity> m_iChangePovertyUnhappiness;
	FAutoVariable<int, CvCity> m_iChangeDefenseUnhappiness;
	FAutoVariable<int, CvCity> m_iChangeUnculturedUnhappiness;
	FAutoVariable<int, CvCity> m_iChangeIlliteracyUnhappiness;
	FAutoVariable<int, CvCity> m_iChangeMinorityUnhappiness;
	FAutoVariable<int, CvCity> m_iChangePovertyUnhappinessGlobal;
	FAutoVariable<int, CvCity> m_iChangeDefenseUnhappinessGlobal;
	FAutoVariable<int, CvCity> m_iChangeUnculturedUnhappinessGlobal;
	FAutoVariable<int, CvCity> m_iChangeIlliteracyUnhappinessGlobal;
	FAutoVariable<int, CvCity> m_iChangeMinorityUnhappinessGlobal;
#endif
	FAutoVariable<std::vector<int>, CvCity> m_aiBaseYieldRateFromReligion;
#if defined(MOD_BALANCE_CORE)
	FAutoVariable<std::vector<int>, CvCity> m_aiBaseYieldRateFromCSAlliance;
	FAutoVariable<std::vector<int>, CvCity> m_aiCorporationYieldChange;
	FAutoVariable<std::vector<int>, CvCity> m_aiCorporationYieldModChange;
	FAutoVariable<int, CvCity> m_iCorporationGPChange;
	FAutoVariable<std::vector<int>, CvCity> m_aiCorporationResourceQuantity;
	FAutoVariable<int, CvCity> m_iLandTourismBonus;
	FAutoVariable<int, CvCity> m_iSeaTourismBonus;
	FAutoVariable<int, CvCity> m_iAlwaysHeal;
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
	FAutoVariable<std::vector<bool>, CvCity> m_abIsPurchased;
	FAutoVariable<std::vector<bool>, CvCity> m_abFranchised;
	FAutoVariable<std::vector<bool>, CvCity> m_abTraded;
	FAutoVariable<bool, CvCity> m_bHasOffice;
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
#endif

#if defined(MOD_BALANCE_CORE)
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
	int ** m_ppaiLocalBuildingClassYield;
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
