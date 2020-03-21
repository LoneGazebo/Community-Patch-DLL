/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

// player.h

#ifndef CIV5_PLAYER_H
#define CIV5_PLAYER_H

#define SAFE_ESTIMATE_NUM_IMPROVEMENTS 50
#define SAFE_ESTIMATE_NUM_CITIES       64
#define MAX_INCOMING_UNITS	20

#include "CvCityAI.h"
#include "CvUnit.h"
#include "CvArmyAI.h"
#include "LinkedList.h"
#include "FFastVector.h"
#include "CvPreGame.h"
#include "CvAchievementUnlocker.h"
#include "CvUnitCycler.h"
#include "TContainer.h"
#if defined(MOD_BALANCE_CORE)
#include "CvMinorCivAI.h"
#endif

class CvPlayerPolicies;
class CvEconomicAI;
class CvMilitaryAI;
class CvCitySpecializationAI;
class CvWonderProductionAI;
class CvGrandStrategyAI;
class CvDiplomacyAI;
class CvPlayerReligions;
class CvReligionAI;
class CvPlayerTechs;
class CvFlavorManager;
class CvTacticalAI;
class CvHomelandAI;
class CvMinorCivAI;
class CvDealAI;
class CvBuilderTaskingAI;
class CvDangerPlots;
#if defined(MOD_BALANCE_CORE_SETTLER)
	class CvDistanceMap;
#endif
#if defined(MOD_BALANCE_CORE)
	class CvPlayerCorporations;
	class CvPlayerContracts;
#endif
class CvCityConnections;
class CvNotifications;
class CvTreasury;
class CvPlayerTraits;
class CvGameInitialItemsOverrides;
class CvDiplomacyRequests;
class CvPlayerEspionage;
class CvEspionageAI;
class CvPlayerTrade;
class CvTradeAI;
class CvLeagueAI;
class CvPlayerCulture;
struct SPath;

typedef std::list<CvPopupInfo*> CvPopupQueue;

typedef std::vector< std::pair<UnitCombatTypes, PromotionTypes> > UnitCombatPromotionArray;
typedef std::vector< std::pair<UnitClassTypes, PromotionTypes> > UnitClassPromotionArray;
typedef std::vector< std::pair<CivilizationTypes, LeaderHeadTypes> > CivLeaderArray;

class CvPlayer
{
	friend class CvPlayerPolicies;
private:
	CvPlayer(const CvPlayer&); //hide copy constructor
	CvPlayer& operator=(const CvPlayer&); //hide assignment operator

public:
	typedef std::map<unsigned int, int> TurnData;

	CvPlayer();
	virtual ~CvPlayer();

	void init(PlayerTypes eID);
	void setupGraphical();
	void reset(PlayerTypes eID = NO_PLAYER, bool bConstructorCall = false);
	void gameStartInit();
	void uninit();

	void initFreeState(CvGameInitialItemsOverrides& kOverrides);
	void initFreeUnits(CvGameInitialItemsOverrides& kOverrides);
	void addFreeUnitAI(UnitAITypes eUnitAI, int iCount);
	CvPlot* addFreeUnit(UnitTypes eUnit, UnitAITypes eUnitAI = NO_UNITAI);

#if defined(MOD_API_EXTENSIONS) && defined(MOD_BALANCE_CORE)
	CvCity* initCity(int iX, int iY, bool bBumpUnits = true, bool bInitialFounding = true, ReligionTypes eInitialReligion = NO_RELIGION, const char* szName = NULL, CvUnitEntry* pkSettlerUnitEntry = NULL);
#elif defined(MOD_API_EXTENSIONS)
	CvCity* initCity(int iX, int iY, bool bBumpUnits = true, bool bInitialFounding = true, ReligionTypes eInitialReligion = NO_RELIGION, const char* szName = NULL);
#elif defined(MOD_BALANCE_CORE)
	CvCity* initCity(int iX, int iY, bool bBumpUnits = true, bool bInitialFounding = true, CvUnitEntry* pkSettlerUnitEntry = NULL);
#else
	CvCity* initCity(int iX, int iY, bool bBumpUnits = true, bool bInitialFounding = true);
#endif
#if defined(MOD_API_EXTENSIONS)
#if defined(MOD_GLOBAL_VENICE_KEEPS_RESOURCES)
	CvCity* acquireCity(CvCity* pCity, bool bConquest, bool bGift, bool bVenice = false);
#else
	CvCity* acquireCity(CvCity* pCity, bool bConquest, bool bGift);
#endif
#else
#if defined(MOD_GLOBAL_VENICE_KEEPS_RESOURCES)
	void acquireCity(CvCity* pCity, bool bConquest, bool bGift, bool bVenice = false);
#else
	void acquireCity(CvCity* pCity, bool bConquest, bool bGift);
#endif
#endif
	bool IsValidBuildingForPlayer(CvCity* pCity, BuildingTypes eBuilding, bool bGift, bool bCapture);
	void killCities();
	CvString getNewCityName() const;
	CvString GetBorrowedCityName(CivilizationTypes eCivToBorrowFrom) const;
	void getCivilizationCityName(CvString& szBuffer, CivilizationTypes eCivilization) const;
	bool isCityNameValid(CvString& szName, bool bTestDestroyed = true, bool bForce = false) const;

	int getBuyPlotDistance() const;
	int getWorkPlotDistance() const;
	int GetNumWorkablePlots() const;

#if defined(MOD_BALANCE_CORE)
	void DoRevolutionPlayer(PlayerTypes ePlayer, int iOldCityID);
	void SetCenterOfMassEmpire();
	CvPlot* GetCenterOfMassEmpire() const;

	void UpdateCityThreatCriteria();
	//0 == highest, 1 = second highest, etc. Not all cities will be assigned!
	CvCity* GetThreatenedCityByRank(int iRank = 0, bool bCoastalOnly = false);

	void UpdateBestMilitaryCities();
	void SetBestMilitaryCityDomain(int iValue, DomainTypes eDomain);
	void SetBestMilitaryCityCombatClass(int iValue, UnitCombatTypes eUnitCombat);
	CvCity* GetBestMilitaryCity(UnitCombatTypes eUnitCombat = NO_UNITCOMBAT, DomainTypes eDomain = NO_DOMAIN);

	// Declared public because CvPlayerCorporations needs to access this. Maybe want to use a friend
	void processCorporations(CorporationTypes eCorporation, int iChange);
#endif
#if defined(MOD_BALANCE_CORE_EVENTS)
	void DoEvents();
	void DoCancelEventChoice(EventChoiceTypes eEventChoice);
	void CheckActivePlayerEvents(CvCity* pCity);
	bool IsEventValid(EventTypes eEvent);
	bool IsEventChoiceValid(EventChoiceTypes eEventChoice, EventTypes eParentEvent);
	void DoStartEvent(EventTypes eEvent);
	void DoEventChoice(EventChoiceTypes eEventChoice, EventTypes eEvent = NO_EVENT, bool bSendMsg = true);
	void DoEventSyncChoices(EventChoiceTypes eEventChoice, CvCity* pCity);
	CvString GetScaledHelpText(EventChoiceTypes eEventChoice, bool bYieldsOnly);
	CvString GetDisabledTooltip(EventChoiceTypes eEventChoice);

	void IncrementEvent(EventTypes eEvent, int iValue);
	int GetEventIncrement(EventTypes eEvent) const;

	void ChangePlayerEventCooldown(int iValue);
	int GetPlayerEventCooldown() const;

	int GetEventChoiceDuration(EventChoiceTypes eEventChoice) const;
	void ChangeEventChoiceDuration(EventChoiceTypes eEventChoice, int iValue);
	void SetEventChoiceDuration(EventChoiceTypes eEventChoice, int iValue);

	int GetEventCooldown(EventTypes eEvent) const;
	void ChangeEventCooldown(EventTypes eEvent, int iValue);
	void SetEventCooldown(EventTypes eEvent, int iValue);

	void SetEventActive(EventTypes eEvent, bool bValue);
	bool IsEventActive(EventTypes eEvent) const;

	void SetEventChoiceActive(EventChoiceTypes eEventChoice, bool bValue);
	bool IsEventChoiceActive(EventChoiceTypes eEventChoice) const;

	bool IsEventChoiceFired(EventChoiceTypes eEventChoice) const;
	void SetEventChoiceFired(EventChoiceTypes eEventChoice, bool bValue);

	bool IsEventFired(EventTypes eEvent) const;
	void SetEventFired(EventTypes eEvent, bool bValue);
#endif
	void DoLiberatePlayer(PlayerTypes ePlayer, int iOldCityID, bool bForced = false);
	bool CanLiberatePlayer(PlayerTypes ePlayer);
	bool CanLiberatePlayerCity(PlayerTypes ePlayer);
#if defined(MOD_BALANCE_CORE)
	CvUnit* initUnit(UnitTypes eUnit, int iX, int iY, UnitAITypes eUnitAI = NO_UNITAI, UnitCreationReason eReason = REASON_DEFAULT, bool bNoMove = false, bool bSetupGraphical = true, int iMapLayer = 0, int iNumGoodyHutsPopped = 0, ContractTypes eContract = NO_CONTRACT, bool bHistoric = true, CvUnit* pPassUnit = NULL);
	CvUnit* initUnitWithNameOffset(UnitTypes eUnit, int nameOffset, int iX, int iY, UnitAITypes eUnitAI = NO_UNITAI, UnitCreationReason eReason = REASON_DEFAULT, bool bNoMove = false, bool bSetupGraphical = true, int iMapLayer = 0, int iNumGoodyHutsPopped = 0, ContractTypes eContract = NO_CONTRACT, bool bHistoric = true, CvUnit* pPassUnit = NULL);
#else
	CvUnit* initUnit(UnitTypes eUnit, int iX, int iY, UnitAITypes eUnitAI = NO_UNITAI, UnitCreationReason eReason = REASON_DEFAULT, bool bNoMove = false, bool bSetupGraphical = true, int iMapLayer = 0, int iNumGoodyHutsPopped = 0, ContractTypes eContract = NO_CONTRACT, bool bHistoric = true);
	CvUnit* initUnitWithNameOffset(UnitTypes eUnit, int nameOffset, int iX, int iY, UnitAITypes eUnitAI = NO_UNITAI, UnitCreationReason eReason = REASON_DEFAULT, bool bNoMove = false, bool bSetupGraphical = true, int iMapLayer = 0, int iNumGoodyHutsPopped = 0, ContractTypes eContract = NO_CONTRACT, bool bHistoric = true);
#endif
	CvUnit* initNamedUnit(UnitTypes eUnit, const char* strKey, int iX, int iY, UnitAITypes eUnitAI = NO_UNITAI, UnitCreationReason eReason = REASON_DEFAULT, bool bNoMove = false, bool bSetupGraphical = true, int iMapLayer = 0, int iNumGoodyHutsPopped = 0);

	void disbandUnit(bool bAnnounce);
	void killUnits();

#if defined(MOD_API_EXTENSIONS) || defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
	UnitTypes GetSpecificUnitType(const char* szUnitClass, bool hideAssert = false);
#endif
#if defined(MOD_API_EXTENSIONS) || defined(MOD_BUGFIX_BUILDINGCLASS_NOT_BUILDING)
	BuildingTypes GetSpecificBuildingType(const char* szBuildingClass, bool hideAssert = false);
#endif

	CvPlot *GetGreatAdmiralSpawnPlot (CvUnit *pUnit);

	int GetNumBuilders() const;
	void SetNumBuilders(int iNum);
	void ChangeNumBuilders(int iChange);
	int GetMaxNumBuilders() const;
	void SetMaxNumBuilders(int iNum);
	void ChangeMaxNumBuilders(int iChange);

	int GetNumUnitsWithUnitAI(UnitAITypes eUnitAIType, bool bIncludeBeingTrained = false, bool bIncludeWater = true);
	int GetNumUnitsWithDomain(DomainTypes eDomain, bool bMilitaryOnly);
	int GetNumUnitsWithUnitCombat(UnitCombatTypes eDomain);
	int GetNumUnitsOfType(UnitTypes eUnit, bool bIncludeBeingTrained = false);
	void UpdateDangerPlots(bool bKeepKnownUnits);
	void SetDangerPlotsDirty();

	bool isHuman() const;
	bool isObserver() const;
	bool isBarbarian() const;
	void doBarbarianRansom(int iOption, int iUnitID);

	const char* getName() const;
	const char* getNameKey() const;
	const char* const getNickName() const;
	const char* getCivilizationDescription() const;
	const char* getCivilizationDescriptionKey() const;
	const char* getCivilizationShortDescription() const;
	const char* getCivilizationShortDescriptionKey() const;
	const char* getCivilizationAdjective() const;
	const char* getCivilizationAdjectiveKey() const;
	const char* getCivilizationTypeKey() const;
	const char* getLeaderTypeKey() const;

	bool isWhiteFlag() const;
	const char* GetStateReligionName() const;
	CvString GetStateReligionKey() const;
	void SetStateReligionKey(const char* strKey);
	const CvString getWorstEnemyName() const;
	ArtStyleTypes getArtStyleType() const;

	void doTurn();
	void doTurnPostDiplomacy();
	void doTurnUnits();
	void SetAllUnitsUnprocessed();
	void DoUnitReset();
	void DoUnitAttrition();
	void RepositionInvalidUnits();
	void ResetReachablePlotsForAllUnits();

	void updateYield();
	void updateExtraSpecialistYield();
	void updateCityPlotYield();
	void updateCitySight(bool bIncrement);
	void UpdateNotifications();
	void UpdateReligion();

	void updateTimers();

	bool hasPromotableUnit() const;

	bool hasReadyUnit() const;
	int GetCountReadyUnits() const;
	const CvUnit* GetFirstReadyUnit() const;
	void EndTurnsForReadyUnits();
	bool hasAutoUnit() const;
	bool hasBusyUnit() const;
	const CvUnit* getBusyUnit() const;
	bool hasBusyCity() const;
	bool hasBusyUnitOrCity() const;
	const CvCity* getBusyCity() const;
	void chooseTech(int iDiscover = 0, const char* strTxt=0, TechTypes iTechJustDiscovered=NO_TECH);

	// Civ 5 Score
	int GetScore(bool bFinal = false, bool bVictory = false) const;

	int GetScoreFromCities() const;
	int GetScoreFromPopulation() const;
	int GetScoreFromLand() const;
	int GetScoreFromWonders() const;
	int GetScoreFromPolicies() const;
	int GetScoreFromGreatWorks() const;
	int GetScoreFromReligion() const;
	int GetScoreFromTechs() const;
	int GetScoreFromFutureTech() const;
	void ChangeScoreFromFutureTech(int iChange);
	int GetScoreFromScenario1() const;
	void ChangeScoreFromScenario1(int iChange);
	int GetScoreFromScenario2() const;
	void ChangeScoreFromScenario2(int iChange);
	int GetScoreFromScenario3() const;
	void ChangeScoreFromScenario3(int iChange);
	int GetScoreFromScenario4() const;
	void ChangeScoreFromScenario4(int iChange);
	// End Civ 5 Score
		
	int countCityFeatures(FeatureTypes eFeature, bool bReset = false) const;
	int countNumBuildings(BuildingTypes eBuilding, bool bReset = false) const;
	int countNumBuildingsInPuppets(BuildingTypes eBuilding, bool bReset = false) const;
	int countCitiesNeedingTerrainImprovements(bool bReset = false) const;

	void setCityFeatures(FeatureTypes eFeature, int iValue);
	int getCityFeatures(FeatureTypes eFeature) const;
	void setNumBuildings(BuildingTypes eBuilding, int iValue);
	int getNumBuildings(BuildingTypes eBuilding) const;

	void setNumBuildingsInPuppets(BuildingTypes eBuilding, int iValue);
	int getNumBuildingsInPuppets(BuildingTypes eBuilding) const;

	void setCitiesNeedingTerrainImprovements(int iValue);
	int getCitiesNeedingTerrainImprovements() const;

	bool IsCityConnectedToCity(CvCity* pCity1, CvCity* pCity2, RouteTypes eRestrictRouteType = ROUTE_ANY, bool bIgnoreHarbors = false, SPath* pPathOut = NULL);
	bool IsCapitalConnectedToPlayer(PlayerTypes ePlayer);

	void findNewCapital();

	bool canRaze(CvCity* pCity, bool bIgnoreCapitals = false) const;
	void raze(CvCity* pCity);
	void unraze(CvCity* pCity);
	void disband(CvCity* pCity);

	bool canReceiveGoody(CvPlot* pPlot, GoodyTypes eGoody, CvUnit* pUnit) const;
	void receiveGoody(CvPlot* pPlot, GoodyTypes eGoody, CvUnit* pUnit);
	void doGoody(CvPlot* pPlot, CvUnit* pUnit);

	void AwardFreeBuildings(CvCity* pCity); // slewis - broken out so that Venice can get free buildings when they purchase something

	void SetNoSettling(int iPlotIndex);
	bool IsNoSettling(int iPlotIndex) const;
	void ClearNoSettling();

	bool canFound(int iX, int iY, bool bIgnoreDistanceToExistingCities, bool bIgnoreHappiness, const CvUnit* pUnit) const;
	bool canFound(int iX, int iY) const;

#if defined(MOD_GLOBAL_RELIGIOUS_SETTLERS) && defined(MOD_BALANCE_CORE)
	void found(int iX, int iY, ReligionTypes eReligion = NO_RELIGION, bool bForce = false, CvUnitEntry* pkSettlerUnitEntry = NULL);
#elif defined(MOD_GLOBAL_RELIGIOUS_SETTLERS)
	void found(int iX, int iY, ReligionTypes eReligion = NO_RELIGION, bool bForce = false);
#elif defined(MOD_BALANCE_CORE)
	void found(int iX, int iY, CvUnitEntry* pkSettlerUnitEntry = NULL);
#else
	void found(int iX, int iY);
#endif
#if defined(MOD_DIPLOMACY_CITYSTATES)
	void cityBoost(int iX, int iY, CvUnitEntry* pkUnitEntry, int iExtraPlots, int iPopChange, int iFoodPercent);
#endif

	bool canTrain(UnitTypes eUnit, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, bool bIgnoreUniqueUnitStatus = false, CvString* toolTipSink = NULL) const;
#if defined(MOD_BALANCE_CORE)
	bool canBarbariansTrain(UnitTypes eUnit, bool bIgnoreUniqueUnitStatus = false, ResourceTypes eResourceNearby = NO_RESOURCE) const;
#endif
	bool canConstruct(BuildingTypes eBuilding, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, CvString* toolTipSink = NULL) const;
	bool canConstruct(BuildingTypes eBuilding, const std::vector<int>& vPreExistingBuildings, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, CvString* toolTipSink = NULL) const;
	bool canCreate(ProjectTypes eProject, bool bContinue = false, bool bTestVisible = false) const;
	bool canPrepare(SpecialistTypes eSpecialist, bool bContinue = false) const;
	bool canMaintain(ProcessTypes eProcess, bool bContinue = false) const;
	bool IsCanPurchaseAnyCity(bool bTestPurchaseCost, bool bTestTrainable, UnitTypes eUnit, BuildingTypes eBuilding, YieldTypes ePurchaseYield);
	bool isProductionMaxedUnitClass(UnitClassTypes eUnitClass) const;
	bool isProductionMaxedBuildingClass(BuildingClassTypes eBuildingClass, bool bAcquireCity = false) const;
	bool isProductionMaxedProject(ProjectTypes eProject) const;
	int getProductionNeeded(UnitTypes eUnit) const;
	int getProductionNeeded(BuildingTypes eBuilding) const;
	int getProductionNeeded(ProjectTypes eProject) const;
	int getProductionNeeded(SpecialistTypes eSpecialist) const;

#if defined(MOD_PROCESS_STOCKPILE)
	int getMaxStockpile() const;
#endif

	int getProductionModifier(CvString* toolTipSink = NULL) const;
	int getProductionModifier(UnitTypes eUnit, CvString* toolTipSink = NULL) const;
	int getProductionModifier(BuildingTypes eBuilding, CvString* toolTipSink = NULL) const;
	int getProductionModifier(ProjectTypes eProject, CvString* toolTipSink = NULL) const;
	int getProductionModifier(SpecialistTypes eSpecialist, CvString* toolTipSink = NULL) const;
	int getProductionModifier(ProcessTypes eProcess, CvString* toolTipSink = NULL) const;

	int getBuildingClassPrereqBuilding(BuildingTypes eBuilding, BuildingClassTypes ePrereqBuildingClass, int iExtra = 0) const;
	void removeBuildingClass(BuildingClassTypes eBuildingClass);
	void processBuilding(BuildingTypes eBuilding, int iChange, bool bFirst, CvArea* pArea);
	int GetBuildingClassYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYieldType);
	int GetBuildingClassYieldModifier(BuildingClassTypes eBuildingClass, YieldTypes eYieldType);

	bool canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestEra = false, bool bTestVisible = false, bool bTestGold = true, bool bTestPlotOwner = true) const;
	bool IsBuildBlockedByFeature(BuildTypes eBuild, FeatureTypes eFeature) const;
	int getBuildCost(const CvPlot* pPlot, BuildTypes eBuild) const;
	int getImprovementUpgradeRate() const;

	RouteTypes getBestRoute(CvPlot* pPlot = NULL) const;

	int GetAllFeatureProduction() const;
	void ChangeAllFeatureProduction(int iChange);

	int calculateTotalYield(YieldTypes eYield) const;

	int GetUnitProductionMaintenanceMod() const;
	void UpdateUnitProductionMaintenanceMod();
	int calculateUnitProductionMaintenanceMod() const;

	int GetUnitGrowthMaintenanceMod() const;
	void UpdateUnitGrowthMaintenanceMod();
	int calculateUnitGrowthMaintenanceMod() const;

	int GetNumUnitsSupplied() const;
	int GetNumUnitsSuppliedByHandicap(bool bIgnoreReduction = false) const;
	int GetNumUnitsSuppliedByCities(bool bIgnoreReduction = false) const;
	int GetNumUnitsSuppliedByPopulation(bool bIgnoreReduction = false) const;

	int GetNumUnitsOutOfSupply() const;
#if defined(MOD_BALANCE_CORE)
	int GetNumUnitsToSupply() const;
	int getNumUnitsSupplyFree() const;
	void changeNumUnitsSupplyFree(int iValue);
#endif

	int calculateUnitCost() const;
	int calculateUnitSupply() const;
	int calculateResearchModifier(TechTypes eTech);
	int calculateGoldRate() const;
	int calculateGoldRateTimes100() const;

	int unitsRequiredForGoldenAge() const;
	int unitsGoldenAgeCapable() const;
	int unitsGoldenAgeReady() const;

	int greatGeneralThreshold() const;
	int greatAdmiralThreshold() const;

	int specialistYield(SpecialistTypes eSpecialist, YieldTypes eYield) const;

#if defined(MOD_BUGFIX_MINOR)
	int GetCityYieldChangeTimes100(YieldTypes eYield) const;
	void ChangeCityYieldChangeTimes100(YieldTypes eYield, int iChange);
#else
	int GetCityYieldChange(YieldTypes eYield) const;
	void ChangeCityYieldChange(YieldTypes eYield, int iChange);
#endif

	int GetCoastalCityYieldChange(YieldTypes eYield) const;
	void ChangeCoastalCityYieldChange(YieldTypes eYield, int iChange);

#if defined(MOD_BUGFIX_MINOR)
	int GetCapitalYieldChangeTimes100(YieldTypes eYield) const;
	void ChangeCapitalYieldChangeTimes100(YieldTypes eYield, int iChange);
#else
	int GetCapitalYieldChange(YieldTypes eYield) const;
	void ChangeCapitalYieldChange(YieldTypes eYield, int iChange);
#endif

	int GetCapitalYieldPerPopChange(YieldTypes eYield) const;
	void ChangeCapitalYieldPerPopChange(YieldTypes eYield, int iChange);

	int GetCapitalYieldPerPopChangeEmpire(YieldTypes eYield) const;
	void ChangeCapitalYieldPerPopChangeEmpire(YieldTypes eYield, int iChange);

	int GetGreatWorkYieldChange(YieldTypes eYield) const;
	void ChangeGreatWorkYieldChange(YieldTypes eYield, int iChange);

	CvPlot* getStartingPlot() const;
	void setStartingPlot(CvPlot* pNewValue);

	int getTotalPopulation() const;
	float getAveragePopulation() const;
	void changeTotalPopulation(int iChange);
	long getRealPopulation() const;

	int GetNewCityExtraPopulation() const;
	void ChangeNewCityExtraPopulation(int iChange);

	int GetFreeFoodBox() const;
	void ChangeFreeFoodBox(int iChange);

	int getTotalLand() const;
	void changeTotalLand(int iChange);

	int getTotalLandScored() const;
	void changeTotalLandScored(int iChange);

	int GetHappinessFromTradeRoutes() const;
	void DoUpdateCityConnectionHappiness();

	// Culture
	int GetTotalJONSCulturePerTurn() const;


	int GetJONSCulturePerTurnFromCities() const;
	int GetJONSCultureFromCitiesTimes100(bool bIgnoreTrade) const;

	int GetJONSCulturePerTurnFromExcessHappiness() const;
	int GetJONSCulturePerTurnFromTraits() const;

#if defined(MOD_BALANCE_CORE)
	int GetYieldPerTurnFromResources(YieldTypes eYield, bool bExported, bool bImported) const;
#endif

	int GetJONSCulturePerTurnForFree() const;
	void ChangeJONSCulturePerTurnForFree(int iChange);

	int GetJONSCulturePerTurnFromMinorCivs() const; // DEPRECATED, use GetCulturePerTurnFromMinorCivs() instead
	void ChangeJONSCulturePerTurnFromMinorCivs(int iChange); // DEPRECATED, does nothing
	int GetCulturePerTurnFromMinorCivs() const;
	int GetCulturePerTurnFromMinor(PlayerTypes eMinor) const;

	int GetCulturePerTurnFromReligion() const;

	int GetCulturePerTurnFromBonusTurns() const;

	int GetJONSCultureCityModifier() const;
	void ChangeJONSCultureCityModifier(int iChange);
#if defined(MOD_DIPLOMACY_CITYSTATES)
	int GetLeagueCultureCityModifier() const;
	void ChangeLeagueCultureCityModifier(int iChange);
#endif

	int getJONSCulture() const;
	void setJONSCulture(int iNewValue);
	void changeJONSCulture(int iChange);

	int GetJONSCultureEverGenerated() const;
	void SetJONSCultureEverGenerated(int iNewValue);
	void ChangeJONSCultureEverGenerated(int iChange);

	int GetJONSCulturePerCityPerTurn() const;
	
	int GetWondersConstructed() const;
	void SetWondersConstructed(int iNewValue);
	void ChangeWondersConstructed(int iChange);

	int GetCulturePerWonder() const;
	void ChangeCulturePerWonder(int iChange);

	int GetCultureWonderMultiplier() const;
	void ChangeCultureWonderMultiplier(int iChange);

	int GetCulturePerTechResearched() const;
	void ChangeCulturePerTechResearched(int iChange);

	int GetSpecialistCultureChange() const;
	void ChangeSpecialistCultureChange(int iChange);

	int GetCultureYieldFromPreviousTurns(int iGameTurn, int iNumPreviousTurnsToCount);
#if defined(MOD_BALANCE_CORE)
	int GetTourismYieldFromPreviousTurns(int iGameTurn, int iNumPreviousTurnsToCount);
	int GetGAPYieldFromPreviousTurns(int iGameTurn, int iNumPreviousTurnsToCount);
#endif

	int GetNumCitiesFreeCultureBuilding() const;
	void ChangeNumCitiesFreeCultureBuilding(int iChange);
	int GetNumCitiesFreeFoodBuilding() const;
	void ChangeNumCitiesFreeFoodBuilding(int iChange);
#if defined(MOD_BALANCE_CORE)
	int GetNumCitiesFreeChosenBuilding(BuildingClassTypes eBuildingClass) const;
	void ChangeNumCitiesFreeChosenBuilding(BuildingClassTypes eBuildingClass, int iChange);

	bool IsFreeUnitNewFoundCity(UnitClassTypes eUnitClass) const;
	void ChangeNewFoundCityFreeUnit(UnitClassTypes eUnitClass, bool bValue);

	bool IsFreeBuildingNewFoundCity(BuildingClassTypes eBuildingClass) const;
	void ChangeNewFoundCityFreeBuilding(BuildingClassTypes eBuildingClass, bool bValue);

	bool IsFreeChosenBuildingNewCity(BuildingClassTypes eBuildingClass) const;
	void ChangeFreeChosenBuildingNewCity(BuildingClassTypes eBuildingClass, bool bValue);

	bool IsFreeBuildingAllCity(BuildingClassTypes eBuildingClass) const;
	void ChangeAllCityFreeBuilding(BuildingClassTypes eBuildingClass, bool bValue);
	
	void SetReformation(bool bValue);
	bool IsReformation() const;

	int GetReformationFollowerReduction() const;
	void ChangeReformationFollowerReduction(int iValue);
#endif

	void DoYieldsFromKill(CvUnit* pAttackingUnit, CvUnit* pDefendingUnit);

	void DoTechFromCityConquer(CvCity* pConqueredCity);
#if defined(MOD_BALANCE_CORE)
	void DoFreeGreatWorkOnConquest(PlayerTypes ePlayer, CvCity* pCity);
	void DoWarVictoryBonuses();
	void DoDifficultyBonus(HistoricEventTypes eHistoricEvent = NO_HISTORIC_EVENT_TYPE);
#endif

#if defined(MOD_API_UNIFIED_YIELDS)
	int GetYieldPerTurnFromReligion(YieldTypes eYield) const;
	int GetYieldPerTurnFromTraits(YieldTypes eYield) const;
#endif

	// Faith
	int GetTotalFaithPerTurn() const;

	int GetFaithPerTurnFromCities() const;
	int GetFaithPerTurnFromMinorCivs() const;
#if defined(MOD_BALANCE_CORE)
	int GetGoldPerTurnFromMinorCivs() const;
	int GetGoldPerTurnFromMinor(PlayerTypes eMinor) const;

	int GetSciencePerTurnFromMinorCivs() const;
	int GetSciencePerTurnFromMinor(PlayerTypes eMinor) const;

	int GetYieldPerTurnFromMinors(YieldTypes eYield) const;
	void SetYieldPerTurnFromMinors(YieldTypes eYield, int iValue);
#endif
	int GetFaithPerTurnFromMinor(PlayerTypes eMinor) const;
	int GetFaithPerTurnFromReligion() const;
	int GetFaith() const;
	void SetFaith(int iNewValue);
	void ChangeFaith(int iChange);
	int GetFaithEverGenerated() const;
	void SetFaithEverGenerated(int iNewValue);
	void ChangeFaithEverGenerated(int iChange);

	// Happiness
	int DoUpdateTotalUnhappiness(CvCity* pAssumeCityAnnexed = NULL, CvCity* pAssumeCityPuppeted = NULL);
	void DoUpdateTotalHappiness();
	void DoUpdateLuxuryHappiness();
	int GetEmpireHappinessForCity(CvCity* pCity = NULL) const;
	int GetEmpireUnhappinessForCity(CvCity* pCity = NULL) const;
	int GetHappiness() const;
	void SetHappiness(int iNewValue);
	void SetUnhappiness(int iNewValue);
	int GetUnhappiness() const;
	void CalculateNetHappiness();
	int GetHappinessRatioRawPercent();
	void DistributeHappinessToCities(int iTotal, int iLux);

#if defined(MOD_BALANCE_CORE_HAPPINESS_NATIONAL)
	void ChangeEmpireNeedsModifierGlobal(int iChange);
	int GetEmpireNeedsModifierGlobal() const;

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

	void ChangeLandmarksTourismPercentGlobal(int iChange);
	int GetLandmarksTourismPercentGlobal() const;

	void ChangeGreatWorksTourismModifierGlobal(int iChange);
	int GetGreatWorksTourismModifierGlobal() const;
#endif
	int GetHappinessForGAP() const;
	int GetExcessHappiness() const;
	bool IsEmpireUnhappy() const;
	bool IsEmpireVeryUnhappy() const;
	bool IsEmpireSuperUnhappy() const;

	void DoUpdateUprisings();
	int GetUprisingCounter() const;
	void SetUprisingCounter(int iValue);
	void ChangeUprisingCounter(int iChange);
	void DoResetUprisingCounter(bool bFirstTime);
	void DoUprising();

	void DoUpdateCityRevolts();
	int GetCityRevoltCounter() const;
	void SetCityRevoltCounter(int iValue);
	void ChangeCityRevoltCounter(int iChange);
	void DoResetCityRevoltCounter();
	void DoCityRevolt();
	CvCity *GetMostUnhappyCity();
	PlayerTypes GetMostUnhappyCityRecipient(CvCity* pCity);

	int GetHappinessFromPolicies() const;
	int GetHappinessFromCities() const;
	int GetHappinessFromBuildings() const;

	int GetExtraHappinessPerCity() const;
	void ChangeExtraHappinessPerCity(int iChange);
	int GetExtraHappinessPerXPolicies() const;
	void ChangeExtraHappinessPerXPolicies(int iChange);

	int GetExtraHappinessPerXPoliciesFromPolicies() const;
	void ChangeExtraHappinessPerXPoliciesFromPolicies(int iChange);

	int GetHappinessPerXGreatWorks() const;
	void ChangeHappinessPerXGreatWorks(int iChange);

#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	int GetHappinessFromResourceMonopolies() const;
#endif
	int GetHappinessFromResources() const;
	int GetHappinessFromResourceVariety() const;
	int GetHappinessFromReligion();
	int GetHappinessFromNaturalWonders() const;
#if defined(MOD_BALANCE_CORE)
	void SetNWOwned(FeatureTypes eFeature, bool bValue);
	bool IsNWOwned(FeatureTypes eFeature) const;

	void ChangeUnitClassProductionModifier(UnitClassTypes eUnitClass, int iValue);
	int GetUnitClassProductionModifier(UnitClassTypes eUnitClass) const;
#endif

	int GetHappinessFromLuxury(ResourceTypes eResource, bool bIncludeImport = true) const;
	int GetExtraHappinessPerLuxury() const;
	void ChangeExtraHappinessPerLuxury(int iChange);
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	int getGlobalAverage(YieldTypes eYield) const;
#endif
#if defined(MOD_BALANCE_CORE_HAPPINESS_LUXURY)
	int GetPlayerHappinessLuxuryPopulationFactor1000() const;
	int GetPlayerHappinessLuxuryCountFactor1000() const;
	int GetBonusHappinessFromLuxuries(int iPop = 0) const;
	int GetBonusHappinessFromLuxuriesFlat() const;
	int GetBonusHappinessFromLuxuriesFlatForUI() const;
	int GetBonusHappinessFromLuxuriesGradient() const;
#endif
#if defined(MOD_BALANCE_CORE)
	int GetUnhappinessFromWarWeariness() const;
#endif

	int GetUnhappinessFromCityForUI(CvCity* pCity) const;
	int GetUnhappinessFromCityCount(CvCity* pAssumeCityAnnexed = NULL, CvCity* pAssumeCityPuppeted = NULL) const;
	int GetUnhappinessFromCapturedCityCount(CvCity* pAssumeCityAnnexed = NULL, CvCity* pAssumeCityPuppeted = NULL) const;
	int GetUnhappinessFromCityPopulation(CvCity* pAssumeCityAnnexed = NULL, CvCity* pAssumeCityPuppeted = NULL) const;
	int GetUnhappinessFromCitySpecialists(CvCity* pAssumeCityAnnexed, CvCity* pAssumeCityPuppeted) const;
	int GetUnhappinessFromPuppetCityPopulation() const;
	int GetUnhappinessFromOccupiedCities(CvCity* pAssumeCityAnnexed = NULL, CvCity* pAssumeCityPuppeted = NULL) const;

	int GetUnhappinessFromUnits() const;
	void ChangeUnhappinessFromUnits(int iChange);

	int GetUnhappinessFromUnitsMod() const;
	void ChangeUnhappinessFromUnitsMod(int iChange);

	int GetUnhappinessMod() const;
	void ChangeUnhappinessMod(int iChange);
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	int getHappinessFromCitizenNeeds() const;
	int getUnhappinessFromCitizenNeeds() const;
	int getUnhappinessFromCityCulture() const;
	int getUnhappinessFromCityScience() const;
	int getUnhappinessFromCityDefense() const;
	int getUnhappinessFromCityGold() const;
	int getUnhappinessFromCityConnection() const;
	int getUnhappinessFromCityPillaged() const;
	int getUnhappinessFromCityStarving() const;
	int getUnhappinessFromCityMinority() const;
	int getUnhappinessFromCityJFDSpecial() const;
#endif

	int GetCityCountUnhappinessMod() const;
	void ChangeCityCountUnhappinessMod(int iChange);

	int GetOccupiedPopulationUnhappinessMod() const;
	void ChangeOccupiedPopulationUnhappinessMod(int iChange);

	int GetCapitalUnhappinessMod() const;
	void ChangeCapitalUnhappinessMod(int iChange);

	int GetHappinessPerGarrisonedUnit() const;
	void SetHappinessPerGarrisonedUnit(int iValue);
	void ChangeHappinessPerGarrisonedUnit(int iChange);

	int GetHappinessPerTradeRoute() const;
	void SetHappinessPerTradeRoute(int iValue);
	void ChangeHappinessPerTradeRoute(int iChange);

	int GetHappinessPerXPopulation() const;
	void SetHappinessPerXPopulation(int iValue);
	void ChangeHappinessPerXPopulation(int iChange);

#if defined(MOD_BALANCE_CORE_POLICIES)
	int GetHappinessPerXPopulationGlobal() const;
	void SetHappinessPerXPopulationGlobal(int iValue);
	void ChangeHappinessPerXPopulationGlobal(int iChange);

	int GetIdeologyPoint() const;
	void SetIdeologyPoint(int iValue);
	void ChangeIdeologyPoint(int iChange);

	int GetNoXPLossUnitPurchase() const;
	void SetNoXPLossUnitPurchase(int iValue);
	void ChangeNoXPLossUnitPurchase(int iChange);

	void ChangeCSAlliesLowersPolicyNeedWonders(int iValue);
	int GetCSAlliesLowersPolicyNeedWonders() const;

	void ChangePositiveWarScoreTourismMod(int iValue);
	int GetPositiveWarScoreTourismMod() const;

	void ChangeIsNoCSDecayAtWar(int iValue);
	bool IsNoCSDecayAtWar() const;

	void ChangeCanBullyFriendlyCS(int iValue);
	bool IsCanBullyFriendlyCS() const;

	void ChangeBullyGlobalCSReduction(int iValue);
	int GetBullyGlobalCSReduction() const;
#endif

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	void ChangeIsVassalsNoRebel(int iValue);
	bool IsVassalsNoRebel() const;

	void ChangeVassalCSBonusModifier(int iValue);
	int GetVassalCSBonusModifier() const;
#endif

	void UpdateHappinessFromMinorCivs();
	int GetHappinessFromMinorCivs() const;
	int GetHappinessFromMinor(PlayerTypes eMinor) const;

	int GetHappinessFromLeagues() const;
	void SetHappinessFromLeagues(int iValue);
	void ChangeHappinessFromLeagues(int iChange);

	// END Happiness

	// Espionage
	int GetEspionageModifier() const;
	void ChangeEspionageModifier(int iChange);
	int GetStartingSpyRank() const;
	void ChangeStartingSpyRank(int iChange);
	// END Espionage

#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
	int GetConversionModifier() const;
	void ChangeConversionModifier(int iChange);
#endif

	int GetExtraLeagueVotes() const;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	int GetImprovementLeagueVotes() const;
	void ChangeImprovementLeagueVotes(int iChange);
	int GetFaithToVotes() const;
	void ChangeFaithToVotes(int iChange);
	int TestFaithToVotes(int iChange);
	int GetCapitalsToVotes() const;
	void ChangeCapitalsToVotes(int iChange);
	int TestCapitalsToVotes(int iChange);
	int GetDoFToVotes() const;
	void ChangeDoFToVotes(int iChange);
	int TestDoFToVotes(int iChange);
	int GetRAToVotes() const;
	void ChangeRAToVotes(int iChange);
	int TestRAToVotes(int iChange);

	int GetDefensePactsToVotes() const;
	void ChangeDefensePactsToVotes(int iChange);
	int TestDefensePactsToVotes(int iChange);

	int GetGPExpendInfluence() const;
	void ChangeGPExpendInfluence(int iChange);
	
	void SetLeagueArt(bool bValue);
	bool IsLeagueArt() const;

	void SetLeagueScholar(bool bValue);
	bool IsLeagueScholar() const;

	//Artsy/Sciencey Bonus
	void SetLeagueAid(bool bValue);
	bool IsLeagueAid() const;

	void ProcessLeagueResolutions();
#if defined(MOD_BALANCE_CORE)
	PlayerTypes AidRankGeneric(int eType = 0);
	int ScoreDifferencePercent(int eType = 0);
#endif
	PlayerTypes AidRank();
	int ScoreDifference();

	int GetScienceRateFromMinorAllies() const;
	void ChangeScienceRateFromMinorAllies(int iChange);
	void SetScienceRateFromMinorAllies(int iValue);

	int GetScienceRateFromLeagueAid() const;
	void ChangeScienceRateFromLeagueAid(int iChange);
	void SetScienceRateFromLeagueAid(int iValue);
#endif

	void ChangeExtraLeagueVotes(int iChange);

	int GetWoundedUnitDamageMod() const;
	void SetWoundedUnitDamageMod(int iValue);
	void ChangeWoundedUnitDamageMod(int iChange);

	int GetUnitUpgradeCostMod() const;
	void SetUnitUpgradeCostMod(int iValue);
	void ChangeUnitUpgradeCostMod(int iChange);

	int GetBarbarianCombatBonus() const;
	void SetBarbarianCombatBonus(int iValue);
	void ChangeBarbarianCombatBonus(int iChange);

	bool IsAlwaysSeeBarbCamps() const;
	void SetAlwaysSeeBarbCampsCount(int iValue);
	void ChangeAlwaysSeeBarbCampsCount(int iChange);

#if defined(MOD_API_EXTENSIONS)
	bool grantPolicy(PolicyTypes iPolicy, bool bFree=false);
	bool revokePolicy(PolicyTypes iPolicy);
	bool swapPolicy(PolicyTypes iNewPolicy, PolicyTypes iOldPolicy);
	void setHasPolicy(PolicyTypes eIndex, bool bNewValue, bool bFree=false);
#else
	void setHasPolicy(PolicyTypes eIndex, bool bNewValue);
#endif
	int getNextPolicyCost() const;
	void DoUpdateNextPolicyCost();
	bool canAdoptPolicy(PolicyTypes ePolicy) const;
	void doAdoptPolicy(PolicyTypes ePolicy);
	void DoBuyNewBranch(PolicyBranchTypes eBranch);

	bool IsAnarchy() const;
	int GetAnarchyNumTurns() const;
	void SetAnarchyNumTurns(int iValue);
	void ChangeAnarchyNumTurns(int iChange);

	int getAdvancedStartPoints() const;
	void setAdvancedStartPoints(int iNewValue);
	void changeAdvancedStartPoints(int iChange);

	bool canStealTech(PlayerTypes eTarget, TechTypes eTech) const;
	bool canSpyDestroyUnit(PlayerTypes eTarget, CvUnit& kUnit) const;
	bool canSpyBribeUnit(PlayerTypes eTarget, CvUnit& kUnit) const;
	bool canSpyDestroyBuilding(PlayerTypes eTarget, BuildingTypes eBuilding) const;
	bool canSpyDestroyProject(PlayerTypes eTarget, ProjectTypes eProject) const;

	void doAdvancedStartAction(AdvancedStartActionTypes eAction, int iX, int iY, int iData, bool bAdd);
	int getAdvancedStartUnitCost(UnitTypes eUnit, bool bAdd, CvPlot* pPlot = NULL);
	int getAdvancedStartCityCost(bool bAdd, CvPlot* pPlot = NULL);
	int getAdvancedStartPopCost(bool bAdd, CvCity* pCity = NULL);
	int getAdvancedStartBuildingCost(BuildingTypes eBuilding, bool bAdd, CvCity* pCity = NULL);
	int getAdvancedStartImprovementCost(ImprovementTypes eImprovement, bool bAdd, CvPlot* pPlot = NULL);
	int getAdvancedStartRouteCost(RouteTypes eRoute, bool bAdd, CvPlot* pPlot = NULL);
	int getAdvancedStartTechCost(TechTypes eTech, bool bAdd);
	int getAdvancedStartVisibilityCost(bool bAdd, CvPlot* pPlot = NULL);

	// Temporary Bonuses
	int GetAttackBonusTurns() const;
	void ChangeAttackBonusTurns(int iChange);
	int GetCultureBonusTurns() const;
	void ChangeCultureBonusTurns(int iChange);

#if defined(MOD_BALANCE_CORE)
	int GetCultureBonusTurnsConquest() const;
	void ChangeCultureBonusTurnsConquest(int iChange);

	int GetProductionBonusTurnsConquest() const;
	void ChangeProductionBonusTurnsConquest(int iChange);
#endif
	int GetTourismBonusTurns() const;
	void ChangeTourismBonusTurns(int iChange);

	// Golden Age Stuff

#if defined(MOD_DIPLOMACY_CITYSTATES)
	void DoProcessVotes();
#endif
#if defined(MOD_BALANCE_CORE_YIELDS)
	void DoChangeGreatGeneralRate();
	void DoChangeGreatAdmiralRate();
#endif
#if defined(MOD_BALANCE_CORE)
	int GetGoldenAgePointsFromEmpire();
	int GetGoldenAgePointsFromCities();
#endif
	void DoProcessGoldenAge();

	int GetGoldenAgeProgressThreshold() const;

	int GetGoldenAgeProgressMeter() const;
	void SetGoldenAgeProgressMeter(int iValue);
	void ChangeGoldenAgeProgressMeter(int iChange);

	int GetGoldenAgeMeterMod() const;
	void SetGoldenAgeMeterMod(int iValue);
	void ChangeGoldenAgeMeterMod(int iChange);

	int GetNumGoldenAges() const;
	void SetNumGoldenAges(int iValue);
	void ChangeNumGoldenAges(int iChange);

	int getGoldenAgeTurns() const;
	bool isGoldenAge() const;
#if defined(MOD_BALANCE_CORE)
	void changeGoldenAgeTurns(int iChange, int iValue = 0, bool bFree = false);
#else
	void changeGoldenAgeTurns(int iChange);
#endif
	int getGoldenAgeLength() const;

#if defined(MOD_BALANCE_CORE)
	int getGoldenAgeLengthModifier() const;
#endif

	int getNumUnitGoldenAges() const;
	void changeNumUnitGoldenAges(int iChange);

	int getStrikeTurns() const;
	void changeStrikeTurns(int iChange);

	int getGoldenAgeModifier() const;
	void changeGoldenAgeModifier(int iChange);

	// Great People Stuff
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
	void createGreatGeneral(UnitTypes eGreatPersonUnit, int iX, int iY, bool bIsFree);
	void createGreatAdmiral(UnitTypes eGreatPersonUnit, int iX, int iY, bool bIsFree);
#else
	void createGreatGeneral(UnitTypes eGreatPersonUnit, int iX, int iY);
	void createGreatAdmiral(UnitTypes eGreatPersonUnit, int iX, int iY);
#endif

#if defined(MOD_GLOBAL_TRULY_FREE_GP)
	int getGreatPeopleCreated(bool bExcludeFree) const;
	void incrementGreatPeopleCreated(bool bIsFree);

	int getGreatGeneralsCreated(bool bExcludeFree) const;
	void incrementGreatGeneralsCreated(bool bIsFree);
	int getGreatAdmiralsCreated(bool bExcludeFree) const;
	void incrementGreatAdmiralsCreated(bool bIsFree);
#if defined(MOD_GLOBAL_SEPARATE_GP_COUNTERS)
	int getGreatMerchantsCreated(bool bExcludeFree) const;
	void incrementGreatMerchantsCreated(bool bIsFree);
	int getGreatScientistsCreated(bool bExcludeFree) const;
	void incrementGreatScientistsCreated(bool bIsFree);
	int getGreatEngineersCreated(bool bExcludeFree) const;
	void incrementGreatEngineersCreated(bool bIsFree);
#endif
	int getGreatWritersCreated(bool bExcludeFree) const;
	void incrementGreatWritersCreated(bool bIsFree);
	int getGreatArtistsCreated(bool bExcludeFree) const;
	void incrementGreatArtistsCreated(bool bIsFree);
	int getGreatMusiciansCreated(bool bExcludeFree) const;
	void incrementGreatMusiciansCreated(bool bIsFree);
#if defined(MOD_DIPLOMACY_CITYSTATES)
	int getGreatDiplomatsCreated(bool bExcludeFree) const;
	void incrementGreatDiplomatsCreated(bool bIsFree);
#endif
#if defined(MOD_BALANCE_CORE)
	int getGPExtra1Created(bool bExcludeFree) const;
	void incrementGPExtra1Created(bool bIsFree);
	
	int getGPExtra2Created(bool bExcludeFree) const;
	void incrementGPExtra2Created(bool bIsFree);

	int getGPExtra3Created(bool bExcludeFree) const;
	void incrementGPExtra3Created(bool bIsFree);

	int getGPExtra4Created(bool bExcludeFree) const;
	void incrementGPExtra4Created(bool bIsFree);

	int getGPExtra5Created(bool bExcludeFree) const;
	void incrementGPExtra5Created(bool bIsFree);
#endif
#else
	int getGreatPeopleCreated() const;
	void incrementGreatPeopleCreated();

	int getGreatGeneralsCreated() const;
	void incrementGreatGeneralsCreated();
	int getGreatAdmiralsCreated() const;
	void incrementGreatAdmiralsCreated();
#if defined(MOD_GLOBAL_SEPARATE_GP_COUNTERS)
	int getGreatMerchantsCreated() const;
	void incrementGreatMerchantsCreated();
	int getGreatScientistsCreated() const;
	void incrementGreatScientistsCreated();
	int getGreatEngineersCreated() const;
	void incrementGreatEngineersCreated();
#endif
	int getGreatWritersCreated() const;
	void incrementGreatWritersCreated();
	int getGreatArtistsCreated() const;
	void incrementGreatArtistsCreated();
	int getGreatMusiciansCreated() const;
	void incrementGreatMusiciansCreated();
#if defined(MOD_DIPLOMACY_CITYSTATES)
	int getGreatDiplomatsCreated() const;
	void incrementGreatDiplomatsCreated();
#endif
#if defined(MOD_BALANCE_CORE)
	int getGPExtra1Created() const;
	void incrementGPExtra1Created();
	
	int getGPExtra2Created() const;
	void incrementGPExtra2Created();

	int getGPExtra3Created() const;
	void incrementGPExtra3Created();

	int getGPExtra4Created() const;
	void incrementGPExtra4Created();

	int getGPExtra5Created() const;
	void incrementGPExtra5Created();
#endif
#endif

	int getMerchantsFromFaith() const;
	void incrementMerchantsFromFaith();
	int getScientistsFromFaith() const;
	void incrementScientistsFromFaith();
	int getWritersFromFaith() const;
	void incrementWritersFromFaith();
	int getArtistsFromFaith() const;
	void incrementArtistsFromFaith();
	int getMusiciansFromFaith() const;
	void incrementMusiciansFromFaith();
	int getGeneralsFromFaith() const;
	void incrementGeneralsFromFaith();
	int getAdmiralsFromFaith() const;
	void incrementAdmiralsFromFaith();
	int getEngineersFromFaith() const;
	void incrementEngineersFromFaith();
#if defined(MOD_DIPLOMACY_CITYSTATES)
	int getDiplomatsFromFaith() const;
	void incrementDiplomatsFromFaith();
#endif
#if defined(MOD_BALANCE_CORE)
	int getGPExtra1FromFaith() const;
	void incrementGPExtra1FromFaith();
	int getGPExtra2FromFaith() const;
	void incrementGPExtra2FromFaith();
	int getGPExtra3FromFaith() const;
	void incrementGPExtra3FromFaith();
	int getGPExtra4FromFaith() const;
	void incrementGPExtra4FromFaith();
	int getGPExtra5FromFaith() const;
	void incrementGPExtra5FromFaith();
#endif

	int getGreatPeopleThresholdModifier() const;
	void changeGreatPeopleThresholdModifier(int iChange);

	int getGreatGeneralsThresholdModifier() const;
	void changeGreatGeneralsThresholdModifier(int iChange);
	int getGreatAdmiralsThresholdModifier() const;
	void changeGreatAdmiralsThresholdModifier(int iChange);

	int getPolicyCostModifier() const;
	void recomputePolicyCostModifier();

	int getGreatPeopleRateModifier() const;
	int getGreatGeneralRateModifier() const;
	int getGreatAdmiralRateModifier() const;
	int getGreatWriterRateModifier() const;
	int getGreatArtistRateModifier() const;
	int getGreatMusicianRateModifier() const;
	int getGreatMerchantRateModifier() const;
	int getGreatScientistRateModifier() const;
	int getGreatEngineerRateModifier() const;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	int getGreatDiplomatRateModifier() const;
#endif
	int getDomesticGreatGeneralRateModifier() const;
#if defined(MOD_BALANCE_CORE)
	int getArtsyGreatPersonRateModifier();
	int getScienceyGreatPersonRateModifier();
#endif
	void changeGreatPeopleRateModFromBldgs(int ichange);
	void changeGreatGeneralRateModFromBldgs(int ichange);
	void recomputeGreatPeopleModifiers();
	int GetGreatPeopleRateModFromFriendships() const;

	int GetGreatScientistBeakerMod() const;
	void SetGreatScientistBeakerMod(int iValue);
	void ChangeGreatScientistBeakerMod(int iChange);

	int GetGreatEngineerHurryMod() const;
	void SetGreatEngineerHurryMod(int iValue);
	void ChangeGreatEngineerHurryMod(int iChange);	

	int GetTechCostXCitiesModifier() const;
	void SetTechCostXCitiesModifier(int iValue);
	void ChangeTechCostXCitiesModifier(int iChange);

	int GetTourismCostXCitiesMod() const;
	void ChangeTourismCostXCitiesMod(int iChange);

	int GetGreatGeneralCombatBonus() const;
	void SetGreatGeneralCombatBonus(int iValue);
	void ChangeGreatGeneralCombatBonus(int iValue);

	// Unit Killed in Combat
#if defined(MOD_API_EXTENSIONS)
	void DoUnitKilledCombat(CvUnit* pKillingUnit, PlayerTypes eKilledPlayer, UnitTypes eUnitType);
#else
	void DoUnitKilledCombat(PlayerTypes eKilledPlayer, UnitTypes eUnit);
#endif
#if defined(MOD_BALANCE_CORE)
	void doInstantYield(InstantYieldType iType, bool bCityFaith = false, GreatPersonTypes eGreatPerson = NO_GREATPERSON, BuildingTypes eBuilding = NO_BUILDING, int iPassYield = 0, bool bEraScale = true, PlayerTypes ePlayer = NO_PLAYER, CvPlot* pPlot = NULL, bool bSuppress = false, CvCity* pCity = NULL, bool bSeaTrade = false, bool bInternational = true, bool bEvent = false, YieldTypes eYield = NO_YIELD, CvUnit* pUnit = NULL, TerrainTypes ePassTerrain = NO_TERRAIN, CvMinorCivQuest* pQuestData = NULL, CvCity* pOtherCity = NULL, CvUnit* pAttackingUnit = NULL);
	void addInstantYieldText(InstantYieldType iType, CvString strInstantYield);
	void setInstantYieldText(InstantYieldType iType, CvString strInstantYield);
	CvString getInstantYieldText(InstantYieldType iType)  const;
	void doInstantGWAM(GreatPersonTypes eGreatPerson, CvString strUnitName, bool bConquest = false);
	void doPolicyGEorGM(int iPolicyGEorGM);
	void doInstantGreatPersonProgress(InstantYieldType iType, bool bSuppress = false, CvCity* pCity = NULL, BuildingTypes eBuilding = NO_BUILDING);
	void addInstantGreatPersonProgressText(InstantYieldType iType, CvString strInstantYield);
	void setInstantGreatPersonProgressText(InstantYieldType iType, CvString strInstantYield);
	CvString getInstantGreatPersonProgressText(InstantYieldType iType)  const;
#endif
	// Great People Expenditure
#if defined(MOD_EVENTS_GREAT_PEOPLE)
	void DoGreatPersonExpended(UnitTypes eGreatPersonUnit, CvUnit* pGreatPersonUnit);
#else
	void DoGreatPersonExpended(UnitTypes eGreatPersonUnit);
#endif
	int GetGreatPersonExpendGold() const;
	void ChangeGreatPersonExpendGold(int iChange);

	// Great People Spawning
	void DoSeedGreatPeopleSpawnCounter();
	void DoApplyNewAllyGPBonus();
	int GetGreatPeopleSpawnCounter();
	void SetGreatPeopleSpawnCounter(int iValue);
	void ChangeGreatPeopleSpawnCounter(int iChange);

#if defined(MOD_GLOBAL_TRULY_FREE_GP)
	void DoSpawnGreatPerson(PlayerTypes eMinor, bool bIsFree);
#else
	void DoSpawnGreatPerson(PlayerTypes eMinor);
#endif
	void DoGreatPeopleSpawnTurn();
	CvCity* GetGreatPersonSpawnCity(UnitTypes eUnit);

	// End Great People Stuff

	int getMaxGlobalBuildingProductionModifier() const;
	void changeMaxGlobalBuildingProductionModifier(int iChange);

	int getMaxTeamBuildingProductionModifier() const;
	void changeMaxTeamBuildingProductionModifier(int iChange);

	int getMaxPlayerBuildingProductionModifier() const;
	void changeMaxPlayerBuildingProductionModifier(int iChange);

	int getFreeExperience() const;
	void changeFreeExperienceFromBldgs(int ichange);
	void changeFreeExperienceFromMinors(int ichange);
	void recomputeFreeExperience();

	void doUpdateBarbarianCampVisibility();

	int getFeatureProductionModifier() const;
	void changeFeatureProductionModifier(int iChange);

	int getWorkerSpeedModifier() const;
	void changeWorkerSpeedModifier(int iChange);

#if defined(MOD_CIV6_WORKER)
	int GetImprovementBuilderCost(BuildTypes iBuild) const;
#endif

	int getImprovementCostModifier() const;
	void changeImprovementCostModifier(int iChange);

	int getImprovementUpgradeRateModifier() const;
	void changeImprovementUpgradeRateModifier(int iChange);

	int getSpecialistProductionModifier() const;
	void changeSpecialistProductionModifier(int iChange);

	int getMilitaryProductionModifier() const;
	void changeMilitaryProductionModifier(int iChange);

	int getSpaceProductionModifier() const;
	void changeSpaceProductionModifier(int iChange);

	int getCityDefenseModifier() const;
	void changeCityDefenseModifier(int iChange);

	int getUnitFortificationModifier() const;
	void changeUnitFortificationModifier(int iChange);

	int getUnitBaseHealModifier() const;
	void changeUnitBaseHealModifier(int iChange);

	int getWonderProductionModifier() const;
	void changeWonderProductionModifier(int iChange);

	int getSettlerProductionModifier() const;
	void changeSettlerProductionModifier(int iChange);

	int getCapitalSettlerProductionModifier() const;
	void changeCapitalSettlerProductionModifier(int iChange);

	int GetPolicyCostBuildingModifier() const;
	void ChangePolicyCostBuildingModifier(int iChange);

	int GetPolicyCostMinorCivModifier() const;
	void ChangePolicyCostMinorCivModifier(int iChange);

	int GetInfluenceSpreadModifier() const;
	void ChangeInfluenceSpreadModifier(int iChange);

	int GetExtraVotesPerDiplomat() const;
	void ChangeExtraVotesPerDiplomat(int iChange);

	int getNumNukeUnits() const;
	void changeNumNukeUnits(int iChange);

	int getBaseFreeUnits() const;
	void changeBaseFreeUnits(int iChange);

	int getNumOutsideUnits();
	void changeNumOutsideUnits(int iChange);
	int GetVerifiedOutsideUnitCount();

	int getGoldPerUnit() const;
	void changeGoldPerUnit(int iChange);
	int getGoldPerUnitTimes100() const;
	void changeGoldPerUnitTimes100(int iChange);

	int getGoldPerMilitaryUnit() const;
	void changeGoldPerMilitaryUnit(int iChange);

	int GetImprovementGoldMaintenanceMod() const;
	void ChangeImprovementGoldMaintenanceMod(int iChange);

#if defined(MOD_CIV6_WORKER)
	int GetRouteBuilderCostMod() const;
	void ChangeRouteBuilderCostMod(int iChange);
#endif

	int GetBuildingGoldMaintenanceMod() const;
	void ChangeBuildingGoldMaintenanceMod(int iChange);

	int GetUnitGoldMaintenanceMod() const;
	void ChangeUnitGoldMaintenanceMod(int iChange);

	int GetUnitSupplyMod() const;
	void ChangeUnitSupplyMod(int iChange);

	int getExtraUnitCost() const;
	void changeExtraUnitCost(int iChange);

	int GetNumMaintenanceFreeUnits(DomainTypes eDomain = NO_DOMAIN, bool bOnlyCombatUnits = false) const;

	int getNumMilitaryUnits() const;
#if defined(MOD_BATTLE_ROYALE)
	/*Changes to allow finer details of the Military Units that the Player has*/
	int getNumMilitaryLandUnits() const;
	int getNumMilitarySeaUnits() const;
	int getNumMilitaryAirUnits() const;
	int GetMilitarySeaMight() const;
	int GetMilitaryAirMight() const;
	int GetMilitaryLandMight() const;
	void changeNumMilitaryUnits(int iChange, DomainTypes eDomain = NO_DOMAIN);
#else
	void changeNumMilitaryUnits(int iChange);
#endif

	int getHappyPerMilitaryUnit() const;
	void changeHappyPerMilitaryUnit(int iChange);

	int getHappinessToCulture() const;
	void changeHappinessToCulture(int iChange);

	int getHappinessToScience() const;
	void changeHappinessToScience(int iChange);

	int getHalfSpecialistUnhappinessCount() const;
	bool isHalfSpecialistUnhappiness() const;
	void changeHalfSpecialistUnhappinessCount(int iChange);

	int getHalfSpecialistFoodCount() const;
	bool isHalfSpecialistFood() const;
	void changeHalfSpecialistFoodCount(int iChange);

#if defined(MOD_BALANCE_CORE)
	int getHalfSpecialistFoodCapitalCount() const;
	bool isHalfSpecialistFoodCapital() const;
	void changeHalfSpecialistFoodCapitalCount(int iChange);

	int getTradeRouteLandDistanceModifier() const;
	void changeTradeRouteLandDistanceModifier(int iChange);

	int getTradeRouteSeaDistanceModifier() const;
	void changeTradeRouteSeaDistanceModifier(int iChange);

	void ChangeDomainFreeExperiencePerGreatWorkGlobal(DomainTypes eDomain, int iChange);
	int GetDomainFreeExperiencePerGreatWorkGlobal(DomainTypes eDomain) const;

	void ChangeDomainFreeExperience(DomainTypes eDomain, int iChange);
	int GetDomainFreeExperience(DomainTypes) const;

	void SetNullifyInfluenceModifier(bool bValue);
	bool IsNullifyInfluenceModifier() const;
#endif

#if defined(MOD_TRAITS_TRADE_ROUTE_PRODUCTION_SIPHON)
	int GetTradeRouteProductionSiphonPercent(bool bInternationalOnly, CvPlayer* pOtherPlayer) const;
#endif

	int getMilitaryFoodProductionCount() const;
	bool isMilitaryFoodProduction() const;
	void changeMilitaryFoodProductionCount(int iChange);

	int GetGoldenAgeCultureBonusDisabledCount() const;
	bool IsGoldenAgeCultureBonusDisabled() const;
	void ChangeGoldenAgeCultureBonusDisabledCount(int iChange);

	void ChangeNumMissionarySpreads(int iChange);
	int GetNumMissionarySpreads() const;

	int GetSecondReligionPantheonCount() const;
	bool IsSecondReligionPantheon() const;
	void ChangeSecondReligionPantheonCount(int iChange);

	int GetEnablesSSPartHurryCount() const;
	bool IsEnablesSSPartHurry() const;
	void ChangeEnablesSSPartHurryCount(int iChange);

	int GetEnablesSSPartPurchaseCount() const;
	bool IsEnablesSSPartPurchase() const;
	void ChangeEnablesSSPartPurchaseCount(int iChange);

	int getHighestUnitLevel() const;
	void setHighestUnitLevel(int iNewValue);

	int getConscriptCount() const;
	void setConscriptCount(int iNewValue);
	void changeConscriptCount(int iChange);

	int getMaxConscript() const;
	void changeMaxConscript(int iChange);

	int getOverflowResearch() const;
	void setOverflowResearch(int iNewValue);
	void changeOverflowResearch(int iChange);
	int getOverflowResearchTimes100() const;
	void setOverflowResearchTimes100(int iNewValue);
	void changeOverflowResearchTimes100(int iChange);

	int getExpModifier() const;
	void changeExpModifier(int iChange);

	int getExpInBorderModifier() const;
	void changeExpInBorderModifier(int iChange);

	int getLevelExperienceModifier() const;
	void changeLevelExperienceModifier(int iChange);

	int getMinorQuestFriendshipMod() const;
	void changeMinorQuestFriendshipMod(int iChange);

	int getMinorGoldFriendshipMod() const;
	void changeMinorGoldFriendshipMod(int iChange);

	int GetMinorFriendshipAnchorMod() const;
	void SetMinorFriendshipAnchorMod(int iValue);
	void ChangeMinorFriendshipAnchorMod(int iChange);

	int GetMinorFriendshipDecayMod() const;
	void changeGetMinorFriendshipDecayMod(int iChange);

	bool IsMinorScienceAllies() const;
	int GetMinorScienceAlliesCount() const;
	void ChangeMinorScienceAlliesCount(int iChange);

	bool IsMinorResourceBonus() const;
	int GetMinorResourceBonusCount() const;
	void ChangeMinorResourceBonusCount(int iChange);

	bool IsAbleToAnnexCityStates() const;
	int GetAbleToAnnexCityStatesCount() const;
	void ChangeAbleToAnnexCityStatesCount(int iChange);

	bool IsOnlyTradeSameIdeology() const;
	void ChangeOnlyTradeSameIdeology(int iChange);
	
#if defined(MOD_BALANCE_CORE)
	//JFD
	void SetPiety(int iValue);
	void ChangePiety(int iValue);
	int GetPiety() const;
	int GetPietyRate() const;
	void SetPietyRate(int iValue);
	void ChangePietyRate(int iValue);
	int GetTurnsSinceConversion() const;
	void SetTurnsSinceConversion(int iValue);
	void DoPiety();
	bool HasStateReligion();
	bool HasSecularized() const;
	void SetHasSecularized(bool bValue);
	bool IsPagan();

	int GetSovereignty() const;
	void SetSovereignty(int iValue);
	void ChangeSovereignty(int iValue);

	int GetGovernment() const;
	void SetGovernment(int iValue);
	bool HasGovernment();

	int GetReformCooldown() const;
	void SetReformCooldown(int iValue, bool bNoEvent = false);
	void ChangeReformCooldown(int iValue);

	int GetGovernmentCooldown() const;
	void SetGovernmentCooldown(int iValue, bool bNoEvent = false);
	void ChangeGovernmentCooldown(int iValue);

	int GetReformCooldownRate() const;
	void SetReformCooldownRate(int iValue);
	void ChangeReformCooldownRate(int iValue);

	int GetGovernmentCooldownRate() const;
	void SetGovernmentCooldownRate(int iValue);
	void ChangeGovernmentCooldownRate(int iValue);

	int GetPoliticLeader() const;
	void SetPoliticLeader(int iID);

	void SetPoliticLeaderKey(const char* strKey);
	CvString GetPoliticLeaderKey() const;

	void SetLegislatureName(const char* strKey);
	CvString GetLegislatureName() const;

	int GetPoliticPercent(int iID) const;
	void SetPoliticPercent(int iID, int iValue);

	void DoGovernmentCooldown();
	void DoReformCooldown();

	int GetCurrency() const;
	void SetCurrency(int iID);

	bool HasCurrency();

	void SetCurrencyName(const char* strKey);
	CvString GetCurrencyName() const;

	void SetProsperityScore(int iValue);
	int GetProsperityScore() const;

	bool PlayerHasContract(ContractTypes eContract) const;
	void SetActiveContract(ContractTypes eContract, bool bValue);

	//DONE
	void DoDiversity(DomainTypes eDomain);
	int GetDiversity(DomainTypes eDomain) const;

	int GetDominationResistance(PlayerTypes ePlayer);

	void ChangeArchaeologicalDigTourism(int iChange);
	int GetArchaeologicalDigTourism() const;

	void ChangeGoldenAgeTourism(int iChange);
	int GetGoldenAgeTourism() const;

	void ChangeExtraCultureandScienceTradeRoutes(int iChange);
	int GetExtraCultureandScienceTradeRoutes() const;

	bool CanUpgradeCSVassalTerritory() const;
	void ChangeUpgradeCSVassalTerritory(int iChange);
	int GetUpgradeCSVassalTerritory() const;

	void ChangeRazingSpeedBonus(int iChange);
	int GetRazingSpeedBonus() const;

	bool IsNoPartisans() const;
	void ChangeNoPartisans(int iChange);
	int GetNoPartisans() const;

	void ChangeSpawnCooldown(int iChange);
	void SetSpawnCooldown(int iChange);
	int GetSpawnCooldown() const;

	bool IsDiplomaticMarriage() const;
	int GetAbleToMarryCityStatesCount() const;
	void ChangeAbleToMarryCityStatesCount(int iChange);

	void ChangeTRSpeedBoost(int iValue);
	int GetTRSpeedBoost() const;
	void SetTRSpeedBoost(int iValue);

	void ChangeVotesPerGPT(int iValue);
	int GetVotesPerGPT() const;

	void ChangeTRVisionBoost(int iValue);
	int GetTRVisionBoost() const;
	void SetTRVisionBoost(int iValue);

	void ChangeEventTourism(int iValue);
	int GetEventTourism() const;
	void SetEventTourism(int iValue);

	int GlobalTourismAlreadyReceived(MinorCivQuestTypes eQuest) const;
	void SetGlobalTourismAlreadyReceived(MinorCivQuestTypes eQuest, int iValue);

	void ChangeEventTourismCS(int iValue);
	int GetEventTourismCS() const;
	void SetEventTourismCS(int iValue);

	void ChangeNumHistoricEvents(HistoricEventTypes eHistoricEvent, int iValue);
	int GetNumHistoricEvents() const;
	void SetNumHistoricEvents(int iValue);

	int GetHistoricEventTourism(HistoricEventTypes eHistoricEvent, CvCity* pCity = NULL);

	void ChangeSingleVotes(int iValue);
	int GetSingleVotes() const;
	void SetSingleVotes(int iValue);

	void ChangeMonopolyModFlat(int iValue);
	int GetMonopolyModFlat() const;
	void SetMonopolyModFlat(int iValue);

	void ChangeMonopolyModPercent(int iValue);
	int GetMonopolyModPercent() const;
	void SetMonopolyModPercent(int iValue);

	int GetCachedValueOfPeaceWithHuman() const;
	void SetCachedValueOfPeaceWithHuman(int iValue);

	void ChangeFaithPurchaseCooldown(int iValue);
	int GetFaithPurchaseCooldown() const;

	int GetNumCSAllies() const;
	void SetNumCSAllies(int iValue);

	int GetNumCSFriends() const;
	void SetNumCSFriends(int iValue);

	void RefreshCSAlliesFriends();
#endif
#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
	int GetPovertyUnhappinessMod() const;
	void ChangePovertyUnhappinessMod(int iChange);
	int GetDefenseUnhappinessMod() const;
	void ChangeDefenseUnhappinessMod(int iChange);
	int GetUnculturedUnhappinessMod() const;
	void ChangeUnculturedUnhappinessMod(int iChange);
	int	GetIlliteracyUnhappinessMod() const;
	void ChangeIlliteracyUnhappinessMod(int iChange);
	int GetMinorityUnhappinessMod() const;
	void ChangeMinorityUnhappinessMod(int iChange);

	int GetPovertyUnhappinessModCapital() const;
	void ChangePovertyUnhappinessModCapital(int iChange);
	int GetDefenseUnhappinessModCapital() const;
	void ChangeDefenseUnhappinessModCapital(int iChange);
	int GetUnculturedUnhappinessModCapital() const;
	void ChangeUnculturedUnhappinessModCapital(int iChange);
	int	GetIlliteracyUnhappinessModCapital() const;
	void ChangeIlliteracyUnhappinessModCapital(int iChange);
	int GetMinorityUnhappinessModCapital() const;
	void ChangeMinorityUnhappinessModCapital(int iChange);
	int GetPuppetUnhappinessMod() const;
	void ChangePuppetUnhappinessMod(int iChange);
	int GetNoUnhappfromXSpecialists() const;
	void ChangeNoUnhappfromXSpecialists(int iChange);

	int GetTechDeviation() const;

	int GetHappfromXSpecialists() const;
	void ChangeHappfromXSpecialists(int iChange);

	int GetNoUnhappfromXSpecialistsCapital() const;
	void ChangeNoUnhappfromXSpecialistsCapital(int iChange);

	int GetSpecialistFoodChange() const;
	void ChangeSpecialistFoodChange(int iChange);

	int GetWarWearinessModifier() const;
	void ChangeWarWearinessModifier(int iChange);

	int GetWarScoreModifier() const;
	void ChangeWarScoreModifier(int iChange);

	void SetProductionRoutesAllCities(bool bValue);
	bool IsProductionRoutesAllCities() const;

	void SetFoodRoutesAllCities(bool bValue);
	bool IsFoodRoutesAllCities() const;
#endif

	int getCultureBombTimer() const;
	void setCultureBombTimer(int iNewValue);
	void changeCultureBombTimer(int iChange);

	int getConversionTimer() const;
	void setConversionTimer(int iNewValue);
	void changeConversionTimer(int iChange);

	CvCity* getCapitalCity() const;
	void setCapitalCity(CvCity* pNewCapitalCity);

	int GetOriginalCapitalX() const;
	int GetOriginalCapitalY() const;
#if defined(MOD_BALANCE_CORE)
	void setOriginalCapitalXY(CvCity* pCapitalCity);
#endif
	bool IsHasLostCapital() const;
	void SetHasLostCapital(bool bValue, PlayerTypes eConqueror);

	bool IsHasLostHolyCity() const;
	void SetHasLostHolyCity(bool bValue, PlayerTypes eConqueror);
	void SetLostHolyCityXY(int iX, int iY);
	int GetLostHolyCityX();
	int GetLostHolyCityY();
	PlayerTypes GetHolyCityConqueror();
#if defined(MOD_GLOBAL_NO_CONQUERED_SPACESHIPS)
	void disassembleSpaceship(CvPlot* pPlot);
#endif
	PlayerTypes GetCapitalConqueror() const;

	int getCitiesLost() const;
	void changeCitiesLost(int iChange);

	//power is military + economic
	int getPower() const;
	int GetMilitaryMight(bool bForMinor = false) const;
	int GetEconomicMight() const;
	int GetProductionMight() const;

#if defined(MOD_UNITS_XP_TIMES_100)
	int getCombatExperienceTimes100() const;
#else
	int getCombatExperience() const;
#endif
#if defined(MOD_GLOBAL_LOCAL_GENERALS)
#if defined(MOD_UNITS_XP_TIMES_100)
	void setCombatExperienceTimes100(int iExperienceTimes100, CvUnit* pFromUnit = NULL);
	void changeCombatExperienceTimes100(int iChangeTimes100, CvUnit* pFromUnit = NULL);
#else
	void setCombatExperience(int iExperience, CvUnit* pFromUnit = NULL);
	void changeCombatExperience(int iChange, CvUnit* pFromUnit = NULL);
#endif
#else
#if defined(MOD_UNITS_XP_TIMES_100)
	void setCombatExperienceTimes100(int iExperienceTimes100);
	void changeCombatExperienceTimes100(int iChangeTimes100);
#else
	void setCombatExperience(int iExperience);
	void changeCombatExperience(int iChange);
#endif
#endif
#if defined(MOD_UNITS_XP_TIMES_100)
	int getLifetimeCombatExperienceTimes100() const;
	int getNavalCombatExperienceTimes100() const;
#else
	int getLifetimeCombatExperience() const;
	int getNavalCombatExperience() const;
#endif
#if defined(MOD_GLOBAL_LOCAL_GENERALS)
#if defined(MOD_UNITS_XP_TIMES_100)
	void setNavalCombatExperienceTimes100(int iExperienceTimes100, CvUnit* pFromUnit = NULL);
	void changeNavalCombatExperienceTimes100(int iChangeTimes100, CvUnit* pFromUnit = NULL);
#else
	void setNavalCombatExperience(int iExperience, CvUnit* pFromUnit = NULL);
	void changeNavalCombatExperience(int iChange, CvUnit* pFromUnit = NULL);
#endif
#else
#if defined(MOD_UNITS_XP_TIMES_100)
	void setNavalCombatExperienceTimes100(int iExperienceTimes100);
	void changeNavalCombatExperienceTimes100(int iChangeTimes100);
#else
	void setNavalCombatExperience(int iExperience);
	void changeNavalCombatExperience(int iChange);
#endif
#endif

	int getBorderObstacleCount() const;
	bool isBorderObstacle() const;
	void changeBorderObstacleCount(int iChange);

#if defined(HH_MOD_BUILDINGS_FRUITLESS_PILLAGE)
	bool isBorderGainlessPillage() const;
	void changeBorderGainlessPillageCount(int iChange);
#endif
	int getNetID() const;
	void setNetID(int iNetID);
	bool isConnected() const;
	void sendTurnReminder();

	uint getStartTime() const;
	void setStartTime(uint uiStartTime);
	uint getTotalTimePlayed() const;

	bool isMinorCiv() const;
#if defined(MOD_API_EXTENSIONS)
	bool isMajorCiv() const;
#endif
	bool IsHasBetrayedMinorCiv() const;
	void SetHasBetrayedMinorCiv(bool bValue);

	void setAlive(bool bNewValue, bool bNotify = true);
	void verifyAlive();
	bool isAlive() const
	{
		return m_bAlive;
	}

	bool isEverAlive() const
	{
		return m_bEverAlive;
	}

	bool isPotentiallyAlive() const
	{
		return m_bPotentiallyAlive;
	}

	void setBeingResurrected(bool bNewValue);
	bool isBeingResurrected() const
	{
		return m_bBeingResurrected;
	}

	bool isTurnActive() const;
	void setTurnActive(bool bNewValue, bool bDoTurn = true);
	bool isSimultaneousTurns() const;
	void setDynamicTurnsSimultMode(bool simultaneousTurns);

	bool isAutoMoves() const;
	void setAutoMoves(bool bNewValue);
	bool hasProcessedAutoMoves() const;
	void setProcessedAutoMoves(bool bNewValue);

	void setTurnActiveForPbem(bool bActive);

	bool isPbemNewTurn() const;
	void setPbemNewTurn(bool bNew);

	bool isEndTurn() const;
	void setEndTurn(bool bNewValue);

	EndTurnBlockingTypes GetEndTurnBlockingType(void) const;
	int GetEndTurnBlockingNotificationIndex(void) const;
	void SetEndTurnBlocking(EndTurnBlockingTypes eBlockingType, int iNotificationIndex);

	bool isTurnDone() const;
	bool isLocalPlayer() const;
	bool isExtendedGame() const;
	void makeExtendedGame();

	bool isFoundedFirstCity() const;
	void setFoundedFirstCity(bool bNewValue);
	int GetNumCitiesFounded() const;
	void ChangeNumCitiesFounded(int iValue);

	// slewis - centralizing code where a player gets whacked by another player
	void CheckForMurder(PlayerTypes ePossibleVictimPlayer); // check to see if we defeated this other player

	bool isStrike() const;
	void setStrike(bool bNewValue);

	bool IsCramped() const;
	void DoUpdateCramped();

	const CvHandicapInfo& getHandicapInfo() const;
	HandicapTypes getHandicapType() const;

	const CvCivilizationInfo& getCivilizationInfo() const;
	CivilizationTypes getCivilizationType() const;

	const CvLeaderHeadInfo& getLeaderInfo() const;
	LeaderHeadTypes getLeaderType() const;
#if defined(MOD_API_EXTENSIONS)
	void setLeaderType(LeaderHeadTypes eNewLeader);
#endif

	LeaderHeadTypes getPersonalityType() const;
	void setPersonalityType(LeaderHeadTypes eNewValue);

	EraTypes GetCurrentEra() const;

	inline PlayerTypes GetID() const
	{
		return m_eID;
	}

	inline TeamTypes getTeam() const
	{
		return CvPreGame::teamType(m_eID);
	}

	void setTeam(TeamTypes eTeam);
	bool IsAITeammateOfHuman() const;

	PlayerColorTypes getPlayerColor() const;
	const CvColorA& getPlayerTextColor() const;

	int getSeaPlotYield(YieldTypes eIndex) const;
	void changeSeaPlotYield(YieldTypes eIndex, int iChange);

	int getYieldRateModifier(YieldTypes eIndex) const;
	void changeYieldRateModifier(YieldTypes eIndex, int iChange);
#if defined(MOD_BALANCE_CORE_POLICIES)
	int GetApproachScratchValue(PlayerTypes ePlayer, MajorCivApproachTypes eMajorCivApproach) const;
	void SetApproachScratchValue(PlayerTypes ePlayer, MajorCivApproachTypes eMajorCivApproach, int iValue);

	int GetTradeReligionModifier() const;
	void changeTradeReligionModifier(int iChange);

	int getYieldFromBirth(YieldTypes eIndex) const;
	void changeYieldFromBirth(YieldTypes eIndex, int iChange);

	int getYieldFromBirthCapital(YieldTypes eIndex) const;
	void changeYieldFromBirthCapital(YieldTypes eIndex, int iChange);

	int getYieldFromDeath(YieldTypes eIndex) const;
	void changeYieldFromDeath(YieldTypes eIndex, int iChange);

	int GetYieldFromVictory(YieldTypes eIndex) const;
	void ChangeYieldFromVictory(YieldTypes eIndex, int iChange);

	int GetYieldFromPillage(YieldTypes eIndex) const;
	void ChangeYieldFromPillage(YieldTypes eIndex, int iChange);

	int getYieldFromConstruction(YieldTypes eIndex) const;
	void changeYieldFromConstruction(YieldTypes eIndex, int iChange);

	int getYieldFromwonderConstruction(YieldTypes eIndex) const;
	void changeYieldFromwonderConstruction(YieldTypes eIndex, int iChange);

	int getYieldFromTech(YieldTypes eIndex) const;
	void changeYieldFromTech(YieldTypes eIndex, int iChange);

	int getYieldFromBorderGrowth(YieldTypes eIndex) const;
	void changeYieldFromBorderGrowth(YieldTypes eIndex, int iChange);

	int getYieldGPExpend(YieldTypes eIndex) const;
	void changeYieldGPExpend(YieldTypes eIndex, int iChange);

	int getConquerorYield(YieldTypes eIndex) const;
	void changeConquerorYield(YieldTypes eIndex, int iChange);

	int getFounderYield(YieldTypes eIndex) const;
	void changeFounderYield(YieldTypes eIndex, int iChange);

	int getArtifactYieldBonus(YieldTypes eIndex) const;
	void changeArtifactYieldBonus(YieldTypes eYield, int iChange);

	int getArtYieldBonus(YieldTypes eIndex) const;
	void changeArtYieldBonus(YieldTypes eYield, int iChange);

	int getMusicYieldBonus(YieldTypes eIndex) const;
	void changeMusicYieldBonus(YieldTypes eYield, int iChange);

	int getLitYieldBonus(YieldTypes eIndex) const;
	void changeLitYieldBonus(YieldTypes eYield, int iChange);

	int getFilmYieldBonus(YieldTypes eIndex) const;
	void changeFilmYieldBonus(YieldTypes eYield, int iChange);
	int getRelicYieldBonus(YieldTypes eIndex) const;
	void changeRelicYieldBonus(YieldTypes eYield, int iChange);

	

	int getReligionYieldRateModifier(YieldTypes eIndex)	const;
	void changeReligionYieldRateModifier(YieldTypes eIndex, int iChange);

	int getGoldenAgeYieldMod(YieldTypes eIndex)	const;
	void changeGoldenAgeYieldMod(YieldTypes eIndex, int iChange);

	int getYieldFromNonSpecialistCitizens(YieldTypes eIndex)	const;
	void changeYieldFromNonSpecialistCitizens(YieldTypes eIndex, int iChange);

	int getYieldModifierFromGreatWorks(YieldTypes eIndex)	const;
	void changeYieldModifierFromGreatWorks(YieldTypes eIndex, int iChange);

	int getYieldModifierFromActiveSpies(YieldTypes eIndex)	const;
	void changeYieldModifierFromActiveSpies(YieldTypes eIndex, int iChange);

	int getYieldFromDelegateCount(YieldTypes eIndex)	const;
	void changeYieldFromDelegateCount(YieldTypes eIndex, int iChange);

	int GetGarrisonsOccupiedUnhapppinessMod() const;
	void changeGarrisonsOccupiedUnhapppinessMod(int iChange);

	int GetXPopulationConscription() const;
	void changeXPopulationConscription(int iChange);
	void DoXPopulationConscription(CvCity* pCity);

	int GetExtraMoves() const;
	void changeExtraMoves(int iChange);

	bool IsNoUnhappinessExpansion() const;
	int GetNoUnhappinessExpansion() const;
	void ChangeNoUnhappinessExpansion(int iChange);

	bool IsNoUnhappyIsolation() const;
	int GetNoUnhappyIsolation() const;
	void ChangeNoUnhappyIsolation(int iChange);

	bool IsDoubleBorderGA() const;
	int GetDoubleBorderGA() const;
	void ChangeDoubleBorderGA(int iChange);

	bool IsIncreasedQuestInfluence() const;
	int GetIncreasedQuestInfluence() const;
	void ChangeIncreasedQuestInfluence(int iChange);

	bool IsCultureBombBoost() const;
	int GetCultureBombBoost() const;
	void changeCultureBombBoost(int iChange);

	bool IsPuppetProdMod() const;
	int GetPuppetProdMod() const;
	void changePuppetProdMod(int iChange);

	bool IsOccupiedProdMod() const;
	int GetOccupiedProdMod() const;
	void changeOccupiedProdMod(int iChange);

	bool IsGoldInternalTrade() const;
	int GetGoldInternalTrade() const;
	void changeGoldInternalTrade(int iChange);

	int GetFreeWCVotes() const;
	void changeFreeWCVotes(int iChange);

	int GetInfluenceGPExpend() const;
	void changeInfluenceGPExpend(int iChange);

	int GetFreeTradeRoute() const;
	void changeFreeTradeRoute(int iChange);

	int GetFreeSpy() const;
	void changeFreeSpy(int iChange);

	int GetReligionDistance() const;
	void changeReligionDistance(int iChange);

	int GetPressureMod() const;
	void changePressureMod(int iChange);

	int getBuildingClassCultureChange(BuildingClassTypes eIndex) const;
	void changeBuildingClassCultureChange(BuildingClassTypes eIndex, int iChange);

	int GetCityStateCombatModifier() const;
	void changeCityStateCombatModifier(int iChange);

#endif
#if defined(MOD_BALANCE_CORE_SPIES)
	bool IsAdvancedActionsEnabled() const;
	void SetAdvancedActionsEnabled(bool bValue);

	int GetAdvancedActionGold() const;
	void changeAdvancedActionGold(int iChange);
	void setAdvancedActionGold(int iChange);
	int GetAdvancedActionScience() const;
	void changeAdvancedActionScience(int iChange);
	void setAdvancedActionScience(int iChange);
	int GetAdvancedActionUnrest() const;
	void changeAdvancedActionUnrest(int iChange);
	void setAdvancedActionUnrest(int iChange);
	int GetAdvancedActionRebellion() const;
	void changeAdvancedActionRebellion(int iChange);
	void setAdvancedActionRebellion(int iChange);
	int GetAdvancedActionGP() const;
	void changeAdvancedActionGP(int iChange);
	void setAdvancedActionGP(int iChange);
	int GetAdvancedActionUnit() const;
	void changeAdvancedActionUnit(int iChange);
	void setAdvancedActionUnit(int iChange);
	int GetAdvancedActionWonder() const;
	void changeAdvancedActionWonder(int iChange);
	void setAdvancedActionWonder(int iChange);
	int GetAdvancedActionBuilding() const;
	void changeAdvancedActionBuilding(int iChange);
	void setAdvancedActionBuilding(int iChange);
	bool IsCannotFailSpies() const;
	int GetCannotFailSpies() const;
	void changeCannotFailSpies(int iChange);
	
	void changeMaxAirUnits(int iChange);
	int getMaxAirUnits() const;

	int GetImprovementExtraYield(ImprovementTypes eImprovement, YieldTypes eYield) const;
	void ChangeImprovementExtraYield(ImprovementTypes eImprovement, YieldTypes eYield, int iChange);
#endif
#if defined(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
	int GetInvestmentModifier() const;
	void changeInvestmentModifier(int iChange);

	int GetMissionInfluenceModifier() const;
	void changeMissionInfluenceModifier(int iChange);

	int GetHappinessPerActiveTradeRoute() const;
	void changeHappinessPerActiveTradeRoute(int iChange);

	int GetNeedsModifierFromAirUnits() const;
	void changeNeedsModifierFromAirUnits(int iChange);

	int GetFlatDefenseFromAirUnits() const;
	void changeFlatDefenseFromAirUnits(int iChange);

	int GetPuppetYieldPenaltyMod() const;
	void changePuppetYieldPenaltyMod(int iChange);

	int GetConquestPerEraBuildingProductionMod() const;
	void changeConquestPerEraBuildingProductionMod(int iChange);

	int GetAdmiralLuxuryBonus() const;
	void changeAdmiralLuxuryBonus(int iChange);

	bool IsCSResourcesCountMonopolies() const;
	void changeCSResourcesCountMonopolies(int iChange);

	int GetScalingNationalPopulationRequrired(BuildingTypes eBuilding) const;
#endif
	int getCapitalYieldRateModifier(YieldTypes eIndex) const;
	void changeCapitalYieldRateModifier(YieldTypes eIndex, int iChange);

	int getExtraYieldThreshold(YieldTypes eIndex) const;
	void updateExtraYieldThreshold(YieldTypes eIndex);

	// Science

	int GetScience() const;
	int GetScienceTimes100() const;


	int GetScienceFromCitiesTimes100(bool bIgnoreTrade) const;
	int GetScienceFromOtherPlayersTimes100() const;
	int GetScienceFromHappinessTimes100() const;
	int GetScienceFromResearchAgreementsTimes100() const;
	int GetScienceFromBudgetDeficitTimes100() const;

	int GetScienceYieldFromPreviousTurns(int iGameTurn, int iNumPreviousTurnsToCount);

	bool IsGetsScienceFromPlayer(PlayerTypes ePlayer) const;
	void SetGetsScienceFromPlayer(PlayerTypes ePlayer, bool bValue);

	// END Science

	void DoDeficit();

	int getSpecialistExtraYield(YieldTypes eIndex) const;
	void changeSpecialistExtraYield(YieldTypes eIndex, int iChange);

	PlayerProximityTypes GetProximityToPlayer(PlayerTypes ePlayer) const;
	void SetProximityToPlayer(PlayerTypes ePlayer, PlayerProximityTypes eProximity);
	void DoUpdateProximityToPlayer(PlayerTypes ePlayer);

	void UpdateResearchAgreements(int iValue);
	int GetResearchAgreementCounter(PlayerTypes ePlayer) const;
	void SetResearchAgreementCounter(PlayerTypes ePlayer, int iValue);
	void ChangeResearchAgreementCounter(PlayerTypes ePlayer, int iChange);

	void DoCivilianReturnLogic(bool bReturn, PlayerTypes eToPlayer, int iUnitID);

	// Incoming Units
	void DoIncomingUnits();
#if defined(MOD_BALANCE_CORE)
	void DoTradeInfluenceAP();
#endif
	void DoDistanceGift(PlayerTypes eFromPlayer, CvUnit* pUnit);
	bool CanGiftUnit(PlayerTypes eToPlayer);
	void AddIncomingUnit(PlayerTypes eFromPlayer, CvUnit* pUnit);
	PlayerTypes GetBestGiftTarget();

	UnitTypes GetIncomingUnitType(PlayerTypes eFromPlayer) const;
	void SetIncomingUnitType(PlayerTypes eFromPlayer, UnitTypes eUnitType);

	int GetIncomingUnitCountdown(PlayerTypes eFromPlayer) const;
	void SetIncomingUnitCountdown(PlayerTypes eFromPlayer, int iNumTurns);
	void ChangeIncomingUnitCountdown(PlayerTypes eFromPlayer, int iChange);

	bool isOption(PlayerOptionTypes eIndex) const;
	void setOption(PlayerOptionTypes eIndex, bool bNewValue);

	bool isPlayable() const;
	void setPlayable(bool bNewValue);

	int getNumResourceUsed(ResourceTypes eIndex) const;
	void changeNumResourceUsed(ResourceTypes eIndex, int iChange);
	int getNumResourceTotal(ResourceTypes eIndex, bool bIncludeImport = true, bool bIncludeMinors = false) const;
	int getNumResourcesFromOther(ResourceTypes eIndex) const;

	void changeNumResourceTotal(ResourceTypes eIndex, int iChange, bool bIgnoreResourceWarning = false);
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	bool HasGlobalMonopoly(ResourceTypes eResource) const;
	void SetHasGlobalMonopoly(ResourceTypes eResource, bool bNewValue);
	bool HasStrategicMonopoly(ResourceTypes eResource) const;
	void SetHasStrategicMonopoly(ResourceTypes eResource, bool bNewValue);
	void CheckForMonopoly(ResourceTypes eResource);
	const std::vector<ResourceTypes>& GetStrategicMonopolies() const { return m_vResourcesWStrategicMonopoly; }
	const std::vector<ResourceTypes>& GetGlobalMonopolies() const { return m_vResourcesWGlobalMonopoly; }
	int GetMonopolyPercent(ResourceTypes eResource) const;

	int getCityYieldModFromMonopoly(YieldTypes eYield) const;
	void changeCityYieldModFromMonopoly(YieldTypes eYield, int iValue);

	int getResourceOverValue(ResourceTypes eIndex) const;
	void changeResourceOverValue(ResourceTypes eIndex, int iChange);
	void setResourceOverValue(ResourceTypes eIndex, int iChange);

	int getResourceFromCSAlliances(ResourceTypes eIndex) const;
	void changeResourceFromCSAlliances(ResourceTypes eIndex, int iChange);
	void setResourceFromCSAlliances(ResourceTypes eIndex, int iChange);

#endif

	int getSiphonLuxuryCount(PlayerTypes eFromPlayer) const;
	void changeSiphonLuxuryCount(PlayerTypes eFromPlayer, int iChange);
	
	void UpdateResourcesSiphoned();

	void DoTestOverResourceNotification(ResourceTypes eIndex);

	int GetStrategicResourceMod() const;
	void ChangeStrategicResourceMod(int iChange);

	int getNumResourceAvailable(ResourceTypes eIndex, bool bIncludeImport = true) const;

	int getResourceGiftedToMinors(ResourceTypes eIndex) const;
	void changeResourceGiftedToMinors(ResourceTypes eIndex, int iChange);

	int getResourceExport(ResourceTypes eIndex) const;
	void changeResourceExport(ResourceTypes eIndex, int iChange);

	int getResourceImport(ResourceTypes eIndex) const;
	void changeResourceImport(ResourceTypes eIndex, int iChange);

	int getResourceFromMinors(ResourceTypes eIndex) const;
	void changeResourceFromMinors(ResourceTypes eIndex, int iChange);

	int getResourceSiphoned(ResourceTypes eIndex) const;
	void changeResourceSiphoned(ResourceTypes eIndex, int iChange);

	int getResourceInOwnedPlots(ResourceTypes eIndex);

	int getTotalImprovementsBuilt() const;
	void changeTotalImprovementsBuilt(int iChange);
	int getImprovementCount(ImprovementTypes eIndex) const;
	void changeImprovementCount(ImprovementTypes eIndex, int iChange);

#if defined(MOD_BALANCE_CORE)
	int getTotalImprovementsBuilt(ImprovementTypes eIndex) const;
	void changeTotalImprovementsBuilt(ImprovementTypes eIndex, int iChange);
#endif

	int getGreatPersonImprovementCount();

	int getFreeBuildingCount(BuildingTypes eIndex) const;
	bool isBuildingFree(BuildingTypes eIndex) const;
	void changeFreeBuildingCount(BuildingTypes eIndex, int iChange);

	int GetFreePromotionCount(PromotionTypes eIndex) const;
	bool IsFreePromotion(PromotionTypes eIndex) const;
	void ChangeFreePromotionCount(PromotionTypes eIndex, int iChange);

	int getUnitCombatProductionModifiers(UnitCombatTypes eIndex) const;
	void changeUnitCombatProductionModifiers(UnitCombatTypes eIndex, int iChange);
	int getUnitCombatFreeExperiences(UnitCombatTypes eIndex) const;
	void changeUnitCombatFreeExperiences(UnitCombatTypes eIndex, int iChange);

	int getUnitClassCount(UnitClassTypes eIndex) const;
	bool isUnitClassMaxedOut(UnitClassTypes eIndex, int iExtra = 0) const;
	void changeUnitClassCount(UnitClassTypes eIndex, int iChange);
	int getUnitClassMaking(UnitClassTypes eIndex) const;
	void changeUnitClassMaking(UnitClassTypes eIndex, int iChange);
	int getUnitClassCountPlusMaking(UnitClassTypes eIndex) const;

	int getBuildingClassCount(BuildingClassTypes eIndex) const;
	bool isBuildingClassMaxedOut(BuildingClassTypes eIndex, int iExtra = 0) const;
	void changeBuildingClassCount(BuildingClassTypes eIndex, int iChange);
	int getBuildingClassMaking(BuildingClassTypes eIndex) const;
	void changeBuildingClassMaking(BuildingClassTypes eIndex, int iChange);
	int getBuildingClassCountPlusMaking(BuildingClassTypes eIndex) const;

	int getProjectMaking(ProjectTypes eIndex) const;
	void changeProjectMaking(ProjectTypes eIndex, int iChange, CvCity* pCity = NULL);

	int getHurryCount(HurryTypes eIndex) const;
	bool IsHasAccessToHurry(HurryTypes eIndex) const;

	bool IsCanHurry(HurryTypes eIndex) const;
	int GetHurryGoldCost(HurryTypes eHurry) const;
	void DoHurry(HurryTypes eIndex);

	bool canPopRush();
	void changeHurryCount(HurryTypes eIndex, int iChange);
	int getHurryModifier(HurryTypes eIndex) const;
	void changeHurryModifier(HurryTypes eIndex, int iChange);

	void setResearchingTech(TechTypes eIndex, bool bNewValue);

	int getSpecialistExtraYield(SpecialistTypes eIndex1, YieldTypes eIndex2) const;
	void changeSpecialistExtraYield(SpecialistTypes eIndex1, YieldTypes eIndex2, int iChange);

#if defined(MOD_API_UNIFIED_YIELDS) && defined(MOD_API_PLOT_YIELDS)
	int getPlotYieldChange(PlotTypes eIndex1, YieldTypes eIndex2) const;
	void changePlotYieldChange(PlotTypes eIndex1, YieldTypes eIndex2, int iChange);
#endif

#if defined(MOD_API_UNIFIED_YIELDS)
	int getFeatureYieldChange(FeatureTypes eIndex1, YieldTypes eIndex2) const;
	void changeFeatureYieldChange(FeatureTypes eIndex1, YieldTypes eIndex2, int iChange);

	int getCityYieldFromUnimprovedFeature(FeatureTypes eIndex1, YieldTypes eIndex2) const;
	void changeCityYieldFromUnimprovedFeature(FeatureTypes eIndex1, YieldTypes eIndex2, int iChange);

	int getUnimprovedFeatureYieldChange(FeatureTypes eIndex1, YieldTypes eIndex2) const;
	void changeUnimprovedFeatureYieldChange(FeatureTypes eIndex1, YieldTypes eIndex2, int iChange);

	int getResourceYieldChange(ResourceTypes eIndex1, YieldTypes eIndex2) const;
	void changeResourceYieldChange(ResourceTypes eIndex1, YieldTypes eIndex2, int iChange);

	int getTerrainYieldChange(TerrainTypes eIndex1, YieldTypes eIndex2) const;
	void changeTerrainYieldChange(TerrainTypes eIndex1, YieldTypes eIndex2, int iChange);

	int getTradeRouteYieldChange(DomainTypes eIndex1, YieldTypes eIndex2) const;
	void changeTradeRouteYieldChange(DomainTypes eIndex1, YieldTypes eIndex2, int iChange);

	int getSpecialistYieldChange(SpecialistTypes eIndex1, YieldTypes eIndex2) const;
	void changeSpecialistYieldChange(SpecialistTypes eIndex1, YieldTypes eIndex2, int iChange);

	int getGreatPersonExpendedYield(GreatPersonTypes eIndex1, YieldTypes eIndex2) const;
	void changeGreatPersonExpendedYield(GreatPersonTypes eIndex1, YieldTypes eIndex2, int iChange);

	int getGoldenAgeGreatPersonRateModifier(GreatPersonTypes eIndex1) const;
	void changeGoldenAgeGreatPersonRateModifier(GreatPersonTypes eIndex1, int iChange);

	int GetYieldFromKills(YieldTypes eYield) const;
	void changeYieldFromKills(YieldTypes eYield, int iChange);

	int GetYieldFromBarbarianKills(YieldTypes eYield) const;
	void changeYieldFromBarbarianKills(YieldTypes eYield, int iChange);

	int GetYieldChangeTradeRoute(YieldTypes eYield) const;
	void ChangeYieldChangeTradeRoute(YieldTypes eYield, int iChange);

	int GetYieldChangesNaturalWonder(YieldTypes eYield) const;
	void ChangeYieldChangesNaturalWonder(YieldTypes eYield, int iChange);

	int GetYieldChangesPerReligionTimes100(YieldTypes eYield) const;
	void ChangeYieldChangesPerReligionTimes100(YieldTypes eYield, int iChange);

	int GetYieldChangeWorldWonder(YieldTypes eYield) const;
	void ChangeYieldChangeWorldWonder(YieldTypes eYield, int iChange);

	int GetYieldFromMinorDemand(YieldTypes eYield) const;
	void ChangeYieldFromMinorDemand(YieldTypes eYield, int iChange);

	int GetYieldFromWLTKD(YieldTypes eYield) const;
	void ChangeYieldFromWLTKD(YieldTypes eYield, int iChange);

	int getBuildingClassYieldChange(BuildingClassTypes eIndex1, YieldTypes eIndex2) const;
	void changeBuildingClassYieldChange(BuildingClassTypes eIndex1, YieldTypes eIndex2, int iChange);
#endif

#if defined(MOD_API_UNIFIED_YIELDS) && defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	int getSpecificGreatPersonRateModifierFromMonopoly(GreatPersonTypes eIndex1, MonopolyTypes eIndex2) const;
	int getSpecificGreatPersonRateModifierFromMonopoly(GreatPersonTypes eIndex1) const;
	void changeSpecificGreatPersonRateModifierFromMonopoly(GreatPersonTypes eIndex1, MonopolyTypes eIndex2, int iChange);
#endif

	int getImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const;
	void changeImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange);

	CvUnitCycler& GetUnitCycler() { return m_UnitCycle; };

	bool removeFromArmy(int iArmyID, int iID);
	bool removeFromArmy(int iID);

	int findPathLengthNew(TechTypes eTech, int pTechs[] = NULL) const;
	int findPathLength(TechTypes eTech, bool bCost = true) const;
	int getQueuePosition(TechTypes eTech) const;
	void clearResearchQueue();
	bool pushResearch(TechTypes eTech, bool bClear = false);
	void popResearch(TechTypes eTech);
	int getLengthResearchQueue() const;
	CLLNode<TechTypes>* nextResearchQueueNode(CLLNode<TechTypes>* pNode);
	const CLLNode<TechTypes>* nextResearchQueueNode(const CLLNode<TechTypes>* pNode) const;
	CLLNode<TechTypes>* headResearchQueueNode();
	const CLLNode<TechTypes>* headResearchQueueNode() const;
	CLLNode<TechTypes>* tailResearchQueueNode();

	void addCityName(const CvString& szName);
	int getNumCityNames() const;
	CvString getCityName(int iIndex) const;
	CLLNode<CvString>* nextCityNameNode(CLLNode<CvString>* pNode);
	const CLLNode<CvString>* nextCityNameNode(const CLLNode<CvString>* pNode) const;
	CLLNode<CvString>* headCityNameNode();
	const CLLNode<CvString>* headCityNameNode() const;

	// city iteration
	const CvCity* firstCity(int* pIterIdx, bool bRev=false) const;
	CvCity* firstCity(int* pIterIdx, bool bRev=false);
	const CvCity* nextCity(int* pIterIdx, bool bRev=false) const;
	CvCity* nextCity(int* pIterIdx, bool bRev=false);
#if defined(MOD_BALANCE_CORE)
	const CvCity* nextCity(const CvCity* pCurrent, bool bRev=false) const;
	CvCity* nextCity(const CvCity* pCurrent, bool bRev=false);
#endif
	int getNumCities() const;
	CvCity* getCity(int iID) const;
	CvCity* addCity();
	void deleteCity(int iID);
	CvCity* GetFirstCityWithBuildingClass(BuildingClassTypes eBuildingClass);

	// unit iteration
	int getNumUnits() const;
	const CvUnit* firstUnit(int* pIterIdx, bool bRev=false) const;
	const CvUnit* nextUnit(int* pIterIdx, bool bRev=false) const;
	CvUnit* firstUnit(int* pIterIdx, bool bRev=false);
	CvUnit* nextUnit(int* pIterIdx, bool bRev=false);
#if defined(MOD_BALANCE_CORE)
	CvUnit* nextUnit(const CvUnit* pCurrent, bool bRev);
	const CvUnit* nextUnit(const CvCity* pCurrent, bool bRev) const;
#endif
	CvUnit* getUnit(int iID) const;
	CvUnit* addUnit();
	void deleteUnit(int iID);

	// army iteration
	const CvArmyAI* firstArmyAI(int* pIterIdx, bool bRev=false) const;
	const CvArmyAI* nextArmyAI(int* pIterIdx, bool bRev=false) const;
	CvArmyAI* firstArmyAI(int* pIterIdx, bool bRev=false);
	CvArmyAI* nextArmyAI(int* pIterIdx, bool bRev=false);
	int getNumArmyAIs() const;
	const CvArmyAI* getArmyAI(int iID) const;
	CvArmyAI* getArmyAI(int iID);
	CvArmyAI* addArmyAI();
	void deleteArmyAI(int iID);

	// operations
	CvAIOperation* getFirstAIOperation();
	CvAIOperation* getNextAIOperation();
	CvAIOperation* getAIOperation(int iID);
	const CvAIOperation* getAIOperation(int iID) const;
	CvAIOperation* addAIOperation(int iOperationType, PlayerTypes eEnemy = NO_PLAYER, int iArea = -1, CvCity* pTarget = NULL, CvCity* pMuster = NULL, bool bNeedOceanMoves = false);
	void deleteAIOperation(int iID);
	bool haveAIOperationOfType(int iOperationType, int* piID=NULL, PlayerTypes eTargetPlayer = NO_PLAYER, CvPlot* pTargetPlot=NULL);
	int numOperationsOfType(int iOperationType);
	bool IsCityAlreadyTargeted(CvCity* pCity, DomainTypes eDomain = NO_DOMAIN, int iPercentToTarget = 100, int iIgnoreOperationID = -1, AIOperationTypes eAlreadyActiveOperation = AI_OPERATION_TYPE_INVALID) const;

	int GetNumOffensiveOperations(DomainTypes eDomain);

	bool StopAllLandDefensiveOperationsAgainstPlayer(PlayerTypes ePlayer, AIOperationAbortReason eReason);
	bool StopAllLandOffensiveOperationsAgainstPlayer(PlayerTypes ePlayer, bool bIncludeSneakOps, AIOperationAbortReason eReason);

	bool StopAllSeaDefensiveOperationsAgainstPlayer(PlayerTypes ePlayer, AIOperationAbortReason eReason);
	bool StopAllSeaOffensiveOperationsAgainstPlayer(PlayerTypes ePlayer, bool bIncludeSneakOps, AIOperationAbortReason eReason);

#if defined(MOD_BALANCE_CORE)
	bool IsMusterCityAlreadyTargeted(CvCity* pCity, DomainTypes eDomain = NO_DOMAIN, int iPercentToTarget = 100, int iIgnoreOperationID = -1, AIOperationTypes eAlreadyActiveOperation = AI_OPERATION_TYPE_INVALID) const;
	bool IsPlotTargetedForExplorer(const CvPlot* pPlot, const CvUnit* pIgnoreUnit=NULL) const;
#endif

	bool IsPlotTargetedForCity(CvPlot *pPlot, CvAIOperation* pOpToIgnore) const;
	void GatherPerTurnReplayStats(int iGameTurn);
	unsigned int getNumReplayDataSets() const;
	const char* getReplayDataSetName(unsigned int uiDataSet) const;
	unsigned int getReplayDataSetIndex(const char* szDataSetName);
	int getReplayDataValue(unsigned int uiDataSet, unsigned int uiTurn) const;
	void setReplayDataValue(unsigned int uiDataSet, unsigned int uiTurn, int iValue);
	TurnData getReplayDataHistory(unsigned int uiDataSet) const;

	int getInstantYieldValue(YieldTypes eYield, int iTurn) const;
	void changeInstantYieldValue(YieldTypes eYield, int iValue);
	CvString getInstantYieldHistoryTooltip(int iGameTurn, int iNumPreviousTurnsToCount);

	int getInstantTourismValue(PlayerTypes ePlayer, int iTurn) const;
	void changeInstantTourismValue(PlayerTypes ePlayer , int iValue);

	// Arbitrary Script Data
	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);

	const CvString& getPbemEmailAddress() const;
	void setPbemEmailAddress(const char* szAddress);

	int getUnitExtraCost(UnitClassTypes eUnitClass) const;
	void setUnitExtraCost(UnitClassTypes eUnitClass, int iCost);

	void launch(VictoryTypes victoryType);

	void invalidatePopulationRankCache();
	void invalidateYieldRankCache(YieldTypes eYield = NO_YIELD);

	PlayerTypes pickConqueredCityOwner(const CvCity& kCity) const;

	int getNewCityProductionValue() const;

	int getGrowthThreshold(int iPopulation) const;

	int GetCityStrengthMod() const;
	void SetCityStrengthMod(int iValue);
	void ChangeCityStrengthMod(int iChange);

	int GetCityGrowthMod() const;
	void SetCityGrowthMod(int iValue);
	void ChangeCityGrowthMod(int iChange);

	int GetCapitalGrowthMod() const;
	void SetCapitalGrowthMod(int iValue);
	void ChangeCapitalGrowthMod(int iChange);

	void UpdatePlots();  // Refreshes the list of plots and sets which ones the player owns
	void AddAPlot(CvPlot* pPlot); // adds an owned plot
	const PlotIndexContainer& GetPlots() const;  // gets the list of plots the player owns
	int GetNumPlots() const;

	int GetNumPlotsBought() const;
	void SetNumPlotsBought(int iValue);
	void ChangeNumPlotsBought(int iChange);

	int GetBuyPlotCost() const;

	int GetPlotGoldCostMod() const;
	void ChangePlotGoldCostMod(int iChange);
#if defined(MOD_TRAITS_CITY_WORKING) || defined(MOD_BUILDINGS_CITY_WORKING) || defined(MOD_POLICIES_CITY_WORKING) || defined(MOD_TECHS_CITY_WORKING)
	int GetCityWorkingChange() const;
	void ChangeCityWorkingChange(int iChange);
#endif

#if defined(MOD_TRAITS_CITY_AUTOMATON_WORKERS) || defined(MOD_BUILDINGS_CITY_AUTOMATON_WORKERS) || defined(MOD_POLICIES_CITY_AUTOMATON_WORKERS) || defined(MOD_TECHS_CITY_AUTOMATON_WORKERS)
	int GetCityAutomatonWorkersChange() const;
	void ChangeCityAutomatonWorkersChange(int iChange);
#endif

	int GetPlotCultureCostModifier() const;
	void ChangePlotCultureCostModifier(int iChange);
	int GetPlotCultureExponentModifier() const;
	void ChangePlotCultureExponentModifier(int iChange);

	int GetNumCitiesPolicyCostDiscount() const;
	void ChangeNumCitiesPolicyCostDiscount(int iChange);

	int GetGarrisonedCityRangeStrikeModifier() const;
	void ChangeGarrisonedCityRangeStrikeModifier(int iChange);
	bool IsGarrisonFreeMaintenance() const;
	void ChangeGarrisonFreeMaintenanceCount(int iChange);

	int GetUnitPurchaseCostModifier() const;
	void ChangeUnitPurchaseCostModifier(int iChange);

	int GetPlotDanger(const CvPlot& Plot, const CvUnit* pUnit, const UnitIdContainer& unitsToIgnore, AirActionType iAirAction = AIR_ACTION_ATTACK);
	int GetPlotDanger(const CvCity* pCity, const CvUnit* pPretendGarrison = NULL);
	int GetPlotDanger(const CvPlot& Plot, bool bFixedDamageOnly);
	void ResetDangerCache(const CvPlot& Plot, int iRange);
	std::vector<CvUnit*> GetPossibleAttackers(const CvPlot& Plot, TeamTypes eTeamForVisibilityCheck);

	bool IsKnownAttacker(const CvUnit* pAttacker);
	bool AddKnownAttacker(const CvUnit* pAttacker);

	CvCity* GetClosestCity(const CvPlot* pPlot, int iSearchRadius, bool bSameArea);

	int GetNumPuppetCities() const;
	int GetNumRealCities() const;
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) || defined(MOD_BALANCE_CORE)
	int GetNumCapitalCities() const;
	int GetNumMinorsControlled() const;
#endif
	int GetMaxEffectiveCities(bool bIncludePuppets = false);

#if defined(MOD_BALANCE_CORE_MILITARY)
	int GetFractionOriginalCapitalsUnderControl() const;
	int GetMilitaryRating() const;
	void SetMilitaryRating(int iValue);
	void ChangeMilitaryRating(int iChange);
	void UpdateMilitaryStats();
	void UpdateAreaEffectUnits();
	void UpdateAreaEffectUnit(CvUnit* pUnit);
	void UpdateAreaEffectPlots();
	int GetAreaEffectModifier(AreaEffectType eType, DomainTypes eDomain, const CvPlot* pTestPlot, const CvUnit* pIgnoreThisUnit=NULL) const;
	const std::vector< std::pair<int,int> >& GetAreaEffectPromotionUnits() const;
	const std::vector< std::pair<int,int> >& GetAreaEffectPositiveUnits() const;
	const std::vector< std::pair<int,int> >& GetAreaEffectNegativeUnits() const;
	const std::vector< std::pair<int,int> >& GetPossibleInterceptors() const;
	const std::vector<int>& GetAreaEffectPositiveFromTraitsPlots() const;
	//this ignores the barbarians
	const std::vector<PlayerTypes>& GetPlayersAtWarWith() const { return m_playersWeAreAtWarWith; }
	const std::vector<PlayerTypes>& GetPlayersAtWarWithInFuture() const { return m_playersAtWarWithInFuture; }
	void UpdateCityStrength();
	void UpdateCurrentAndFutureWars();
	//to check whether peace is a good idea
	bool HasCityInDanger(bool bAboutToFall, int iMinDanger) const;

	int GetExtraSupplyPerPopulation() const;
	void ChangeExtraSupplyPerPopulation(int iValue);

	int getCitySupplyFlatGlobal() const;
	void changeCitySupplyFlatGlobal(int iChange);

	int GetAvgUnitExp100() const;
#endif

	int GetNumNaturalWondersDiscoveredInArea() const;
	void SetNumNaturalWondersDiscoveredInArea(int iValue);
	void ChangeNumNaturalWondersDiscoveredInArea(int iChange);

	int GetNumNaturalWondersInOwnedPlots();

	int GetTurnsSinceSettledLastCity() const;
	void SetTurnsSinceSettledLastCity(int iValue);
	void ChangeTurnsSinceSettledLastCity(int iChange);

	int GetBestSettleAreas(int& iFirstArea, int& iSecondArea);
	CvPlot* GetBestSettlePlot(const CvUnit* pUnit, int iTargetArea, bool bNeedSafe, bool& bIsSafe, CvAIOperation* pOpToIgnore=NULL, bool bForceLogging=false) const;
	bool HaveGoodSettlePlot(int iAreaID);

	// New Victory Stuff
	int GetNumWonders() const;
	void ChangeNumWonders(int iValue);
	int GetNumPolicies() const;
	void ChangeNumPolicies(int iValue);
	int GetNumGreatPeople() const;
	void ChangeNumGreatPeople(int iValue);
	// End New Victory Stuff

#if defined(MOD_BALANCE_CORE)
	void SetBestWonderCities();
	bool isCapitalCompetitive();
#endif
	void DoAdoptedGreatPersonCityStatePolicy();
	bool IsAlliesGreatPersonBiasApplied() const;
	void SetAlliesGreatPersonBiasApplied(bool bValue);

	// New Religion Stuff
	bool IsHasAdoptedStateReligion() const;
	void SetHasAdoptedStateReligion(bool bValue);

	int GetNumCitiesWithStateReligion();

	CvCity* GetHolyCity();
	void SetHolyCity(int iCityID);

	PromotionTypes GetEmbarkationPromotion() const;
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
	PromotionTypes GetDeepWaterEmbarkationPromotion() const;
#endif

	void DoAnnounceReligionAdoption();
	// End New Religion Stuff

	int GetNumFreeTechs() const;
	void SetNumFreeTechs(int iValue);

	int GetMedianTechPercentage() const;
	void ChangeMedianTechPercentage(int iValue);

	int GetNumFreeGreatPeople() const;
	void SetNumFreeGreatPeople(int iValue);
	void ChangeNumFreeGreatPeople(int iChange);

	int GetMissionaryExtraStrength() const;
	void ChangeMissionaryExtraStrength(int iChange);

	int GetNumMayaBoosts() const;
	void SetNumMayaBoosts(int iValue);
	void ChangeNumMayaBoosts(int iChange);

	int GetNumFaithGreatPeople() const;
	void SetNumFaithGreatPeople(int iValue);
	void ChangeNumFaithGreatPeople(int iChange);

	int GetNumArchaeologyChoices() const;
	void SetNumArchaeologyChoices(int iValue);

	FaithPurchaseTypes GetFaithPurchaseType() const;
	void SetFaithPurchaseType(FaithPurchaseTypes eType);
	int GetFaithPurchaseIndex() const;
	void SetFaithPurchaseIndex(int iIndex);

	int GetNumFreePolicies() const;
	void SetNumFreePolicies(int iValue);
	void ChangeNumFreePolicies(int iChange);

	int GetNumFreePoliciesEver() const;
	void SetNumFreePoliciesEver(int iValue);
	void ChangeNumFreePoliciesEver(int iChange);

	int GetNumFreeTenets() const;
	void SetNumFreeTenets(int iValue, bool bCountAsFreePolicies);
	void ChangeNumFreeTenets(int iChange, bool bCountAsFreePolicies);

	int GetLastSliceMoved() const;
	void SetLastSliceMoved(int iValue);

	void SetEverConqueredBy(PlayerTypes ePlayer, bool bValue);
	bool IsEverConqueredBy(PlayerTypes ePlayer);

	// slewis Tutorial functions
	bool GetEverPoppedGoody(void);  // has this player ever popped a goody hut
	CvPlot* GetClosestGoodyPlot(bool bStopAfterFindingFirst = false);  // find the goody plot that has the closest unit that can reach it, null means none could be found
	bool GetPlotHasOrder(CvPlot* Plot);  // are any of the player's units directed to this plot?
	bool GetAnyUnitHasOrderToGoody(void);
	bool GetEverTrainedBuilder(void);
	// end Tutorial functions

	bool CanCrossOcean() const;
	bool CanCrossMountain() const;
	bool CanCrossIce() const;

	// International Trade
	bool IsAllowedToTradeWith(PlayerTypes eOtherPlayer);
	// end International Trade

	CvPlayerPolicies* GetPlayerPolicies() const;
	CvPlayerTraits* GetPlayerTraits() const;
	CvEconomicAI* GetEconomicAI() const;
	CvMilitaryAI* GetMilitaryAI() const;
	CvCitySpecializationAI* GetCitySpecializationAI() const;
	CvWonderProductionAI* GetWonderProductionAI() const;
	CvGrandStrategyAI* GetGrandStrategyAI() const;
	CvDiplomacyAI* GetDiplomacyAI() const;
	CvPlayerReligions* GetReligions() const;
	CvPlayerCulture* GetCulture() const;
	CvReligionAI* GetReligionAI() const;
	CvPlayerTechs* GetPlayerTechs() const;
	CvFlavorManager* GetFlavorManager() const;
	CvTacticalAI* GetTacticalAI() const;
	CvHomelandAI* GetHomelandAI() const;
	CvMinorCivAI* GetMinorCivAI() const;
	CvDealAI* GetDealAI() const;
	CvBuilderTaskingAI* GetBuilderTaskingAI() const;
	CvCityConnections* GetCityConnections() const;
	CvPlayerEspionage* GetEspionage() const;
	CvEspionageAI* GetEspionageAI() const;
	CvPlayerTrade* GetTrade() const;
	CvTradeAI* GetTradeAI() const;
	CvLeagueAI* GetLeagueAI() const;
	CvNotifications* GetNotifications() const;
#if defined(MOD_BALANCE_CORE)
	CvPlayerCorporations* GetCorporations() const;
	CvPlayerContracts* GetContracts() const;
#endif
#if defined(MOD_API_EXTENSIONS)
	int AddNotification(NotificationTypes eNotificationType, const char* sMessage, const char* sSummary, CvPlot* pPlot = NULL, int iGameDataIndex = -1, int iExtraGameData = -1);
	int AddNotification(NotificationTypes eNotificationType, const char* sMessage, const char* sSummary, int iGameDataIndex, int iExtraGameData = -1);
#endif
	CvDiplomacyRequests* GetDiplomacyRequests() const;
	bool HasActiveDiplomacyRequests() const;

	CvTreasury* GetTreasury() const;
	int GetPseudoRandomSeed() const;

	int GetCityDistanceHighwaterMark() const;
	void SetCityDistanceHighwaterMark(int iNewValue);

	void SetEmbarkedGraphicOverride(CvString szGraphicName)
	{
		m_strEmbarkedGraphicOverride = szGraphicName;
	};
	const CvString& GetEmbarkedGraphicOverride() const
	{
		return m_strEmbarkedGraphicOverride;
	};

#if defined(MOD_API_EXTENSIONS)
	bool HasBelief(BeliefTypes iBeliefType) const;
	bool HasBuilding(BuildingTypes iBuildingType);
	bool HasBuildingClass(BuildingClassTypes iBuildingClassType);
	bool HasAnyWonder();
	bool HasWonder(BuildingTypes iBuildingType);
	bool IsCivilization(CivilizationTypes iCivilizationType) const;
	bool IsInEra(EraTypes iEraType) const;
	bool HasReachedEra(EraTypes iEraType) const;
	bool HasAnyNaturalWonder();
	bool HasNaturalWonder(FeatureTypes iFeatureType);
	bool HasPolicy(PolicyTypes iPolicyType) const;
	bool HasTenet(PolicyTypes iPolicyType) const;
	bool HasPolicyBranch(PolicyBranchTypes iPolicyBranchType) const;
	bool HasIdeology(PolicyBranchTypes iPolicyBranchType) const;
	bool HasProject(ProjectTypes iProjectType) const;
	bool IsAtPeace() const;
	bool IsAtPeaceAllMajors() const;
	bool IsAtPeaceAllMinors() const;
	bool IsAtPeaceWith(PlayerTypes iPlayer) const;
	bool IsAtWar() const;
	bool IsAtWarAnyMajor() const;
	bool IsAtWarAnyMinor() const;
	bool IsAtWarWith(PlayerTypes iPlayer) const;
	bool HasPantheon() const;
	bool HasAnyReligion() const;
	bool HasReligion(ReligionTypes iReligionType) const;
	bool HasEnhancedReligion() const;
	bool IsConnectedTo(PlayerTypes iPlayer);
	bool HasSpecialistSlot(SpecialistTypes iSpecialistType);
	bool HasSpecialist(SpecialistTypes iSpecialistType);
	bool HasTech(TechTypes iTechType) const;
	bool HasAnyDomesticTradeRoute() const;
	bool HasAnyInternationalTradeRoute() const;
	bool HasAnyTradeRoute() const;
	int GetNumInternationalRoutes() const;
	int GetNumInternalRoutes() const;
	bool HasAnyTradeRouteWith(PlayerTypes iPlayer) const;
	bool HasUnit(UnitTypes iUnitType);
	bool HasUnitClass(UnitClassTypes iUnitClassType);
	bool HasUUActive();
	bool HasUUPeriod() const;
	void SetHasUUPeriod();

	bool HasTrait(TraitTypes eTrait) const;
	bool HasAnyHolyCity();
	bool HasHolyCity(ReligionTypes eReligion);
	bool HasCapturedHolyCity(ReligionTypes eReligion);
	bool HasEmbassyWith(PlayerTypes eOtherPlayer) const;
	void DoForceDefPact(PlayerTypes eOtherPlayer);
	int CountAllFeature(FeatureTypes iFeatureType);
	int CountAllWorkedFeature(FeatureTypes iFeatureType);
	int CountAllImprovement(ImprovementTypes iImprovementType, bool bOnlyCreated = false);
	int CountAllWorkedImprovement(ImprovementTypes iImprovementType);
	int CountAllPlotType(PlotTypes iPlotType);
	int CountAllWorkedPlotType(PlotTypes iPlotType);
	int CountAllResource(ResourceTypes iResourceType);
	int CountAllWorkedResource(ResourceTypes iResourceType);
	int CountAllTerrain(TerrainTypes iTerrainType);
	int CountAllWorkedTerrain(TerrainTypes iTerrainType);
#endif

	// for serialization
	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

	virtual void AI_init() = 0;
	virtual void AI_reset() = 0;
	virtual void AI_doTurnPre() = 0;
	virtual void AI_doTurnPost() = 0;
	virtual void AI_doTurnUnitsPre() = 0;
	virtual void AI_doTurnUnitsPost() = 0;
	virtual void AI_unitUpdate() = 0;
#if defined(MOD_BALANCE_CORE)
	virtual void AI_conquerCity(CvCity* pCity, PlayerTypes eOldOwner, bool bGift, bool bAllowRaze) = 0;
	bool HasSameIdeology(PlayerTypes ePlayer) const;
#else
	virtual void AI_conquerCity(CvCity* pCity, PlayerTypes eOldOwner) = 0;
#endif
#if defined(MOD_BALANCE_CORE_EVENTS)
	virtual void AI_DoEventChoice(EventTypes eEvent) = 0;
#endif

	virtual void computeAveragePlotFoundValue();
	virtual void updatePlotFoundValues();
	virtual void invalidatePlotFoundValues();
	virtual int getPlotFoundValue(int iX, int iY);
	virtual void setPlotFoundValue(int iX, int iY, int iValue);

	virtual void AI_chooseFreeGreatPerson() = 0;
	virtual void AI_chooseFreeTech() = 0;
	virtual void AI_chooseResearch() = 0;
	virtual void AI_launch(VictoryTypes eVictory) = 0;
	virtual OperationSlot PeekAtNextUnitToBuildForOperationSlot(CvCity* pCity, bool& bCitySameAsMuster) = 0;
	virtual OperationSlot CityCommitToBuildUnitForOperationSlot(CvCity* pCity) = 0;
	virtual void CityUncommitToBuildUnitForOperationSlot(OperationSlot thisSlot) = 0;
	virtual void CityFinishedBuildingUnitForOperationSlot(OperationSlot thisSlot, CvUnit* pThisUnit) = 0;
	virtual int GetNumUnitsNeededToBeBuilt() = 0;
	const FAutoArchive& getSyncArchive() const;
	FAutoArchive& getSyncArchive();
	void disconnected();
	void reconnected();
	bool hasBusyUnitUpdatesRemaining() const;
	void setBusyUnitUpdatesRemaining(int iUpdateCount);
	bool hasUnitsThatNeedAIUpdate() const;

	void checkInitialTurnAIProcessed();
	void checkRunAutoMovesForEveryone();
	std::string debugDump(const FAutoVariableBase&) const;
	std::string stackTraceRemark(const FAutoVariableBase&) const;

#if !defined(NO_ACHIEVEMENTS)
	CvPlayerAchievements& GetPlayerAchievements(){return m_kPlayerAchievements;}
#endif

	bool hasTurnTimerExpired();
#if defined(MOD_BALANCE_CORE)
	int GetScoreFromMinorAllies() const;
	int GetScoreFromMilitarySize() const;
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	CvString GetVassalIndependenceTooltipAsMaster(PlayerTypes ePlayer) const;
	CvString GetVassalIndependenceTooltipAsVassal() const;

	int GetScoreFromVassals() const;
	int GetScoreFromVassal(PlayerTypes ePlayer) const;

	int GetYieldPerTurnFromVassals(YieldTypes eYield) const;

	int GetHappinessFromVassals() const;
	int GetHappinessFromVassal(PlayerTypes ePlayer) const;

	int GetVassalGoldMaintenanceMod() const;
	void SetVassalGoldMaintenanceMod(int iValue);
	void ChangeVassalGoldMaintenanceMod(int iChange);

	void DoVassalLevy();
	void SetVassalLevy(bool bValue);
#endif

	//assuming a typical unit with baseMoves==2
	int GetCityDistanceInEstimatedTurns( const CvPlot* pPlot ) const;
	CvCity* GetClosestCityByEstimatedTurns( const CvPlot* pPlot) const;
	int GetCityDistanceInPlots(const CvPlot* pPlot) const;
	CvCity* GetClosestCityByPlots(const CvPlot* pPlot) const;

protected:
	class ConqueredByBoolField
	{
	public:
		enum { eCount = 4, eSize = 32 };
		DWORD m_dwBits[eCount];

		bool GetBit(const uint uiEntry) const
		{
			const uint uiOffset = uiEntry/eSize;
			return m_dwBits[uiOffset] & 1<<(uiEntry-(eSize*uiOffset));
		}
		void SetBit(const uint uiEntry)
		{
			const uint uiOffset = uiEntry/eSize;
			m_dwBits[uiOffset] |= 1<<(uiEntry-(eSize*uiOffset));
		}
		void ClearBit(const uint uiEntry)
		{
			const uint uiOffset = uiEntry/eSize;
			m_dwBits[uiOffset] &= ~(1<<(uiEntry-(eSize*uiOffset)));
		}
		void ToggleBit(const uint uiEntry)
		{
			const uint uiOffset = uiEntry/eSize;
			m_dwBits[uiOffset] ^= 1<<(uiEntry-(eSize*uiOffset));
		}
		void ClearAll()
		{
			for(uint i = 0; i <eCount; ++i)
			{
				m_dwBits[i] = 0;
			}
		}

		bool ValidateFromBoolArray(const bool* pBools, uint uiCount) const
		{
			for(uint i = 0; i < uiCount; ++i)
				if(GetBit(i) != pBools[i]) return false;

			return true;
		}

		void InitFromBoolArray(bool* pBools, uint uiCount)
		{
			for(uint i = 0; i < uiCount; ++i)
				if(GetBit(i) != pBools[i]) ToggleBit(i);
		}
	};

	void updateMightStatistics();
	int calculateMilitaryMight(DomainTypes eDomain = NO_DOMAIN) const;
	int calculateEconomicMight() const;
	int calculateProductionMight() const;

	FAutoArchiveClassContainer<CvPlayer> m_syncArchive;

	FAutoVariable<PlayerTypes, CvPlayer> m_eID;
	FAutoVariable<LeaderHeadTypes, CvPlayer> m_ePersonalityType;

	FAutoVariable<int, CvPlayer> m_iStartingX;
	FAutoVariable<int, CvPlayer> m_iStartingY;
	FAutoVariable<int, CvPlayer> m_iTotalPopulation;
	FAutoVariable<int, CvPlayer> m_iTotalLand;
	FAutoVariable<int, CvPlayer> m_iTotalLandScored;
	FAutoVariable<int, CvPlayer> m_iJONSCulturePerTurnForFree;
	FAutoVariable<int, CvPlayer> m_iJONSCulturePerTurnFromMinorCivs;
	FAutoVariable<int, CvPlayer> m_iJONSCultureCityModifier;
	FAutoVariable<int, CvPlayer> m_iJONSCulture;
	FAutoVariable<int, CvPlayer> m_iJONSCultureEverGenerated;
	FAutoVariable<int, CvPlayer> m_iWondersConstructed;
	FAutoVariable<int, CvPlayer> m_iCulturePerWonder;
	FAutoVariable<int, CvPlayer> m_iCultureWonderMultiplier;
	FAutoVariable<int, CvPlayer> m_iCulturePerTechResearched;
	FAutoVariable<int, CvPlayer> m_iFaith;
	FAutoVariable<int, CvPlayer> m_iFaithEverGenerated;
	FAutoVariable<int, CvPlayer> m_iHappiness;
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	FAutoVariable<int, CvPlayer> m_iUnhappiness;
	FAutoVariable<int, CvPlayer> m_iHappinessTotal;
	FAutoVariable<int, CvPlayer> m_iEmpireNeedsModifierGlobal;
	FAutoVariable<int, CvPlayer> m_iChangePovertyUnhappinessGlobal;
	FAutoVariable<int, CvPlayer> m_iChangeDefenseUnhappinessGlobal;
	FAutoVariable<int, CvPlayer> m_iChangeUnculturedUnhappinessGlobal;
	FAutoVariable<int, CvPlayer> m_iChangeIlliteracyUnhappinessGlobal;
	FAutoVariable<int, CvPlayer> m_iChangeMinorityUnhappinessGlobal;
#endif
#if defined(MOD_BALANCE_CORE)
	FAutoVariable<int, CvPlayer> m_iLandmarksTourismPercentGlobal;
	FAutoVariable<int, CvPlayer> m_iGreatWorksTourismModifierGlobal;
	FAutoVariable<int, CvPlayer> m_iCenterOfMassX;
	FAutoVariable<int, CvPlayer> m_iCenterOfMassY;
	FAutoVariable<int, CvPlayer> m_iReferenceFoundValue;
	FAutoVariable<int, CvPlayer> m_iReformationFollowerReduction;
	FAutoVariable<bool, CvPlayer> m_bIsReformation;
	FAutoVariable<std::vector<int>, CvPlayer> m_viInstantYieldsTotal;

	//FAutoVariable<int, CvPlayer> m_iCultureAverage;
//	FAutoVariable<int, CvPlayer> m_iScienceAverage;
//	FAutoVariable<int, CvPlayer> m_iDefenseAverage;
//	FAutoVariable<int, CvPlayer> m_iGoldAverage;
#endif
	FAutoVariable<int, CvPlayer> m_iUprisingCounter;
	FAutoVariable<int, CvPlayer> m_iExtraHappinessPerLuxury;
	FAutoVariable<int, CvPlayer> m_iUnhappinessFromUnits;
	FAutoVariable<int, CvPlayer> m_iUnhappinessFromUnitsMod;
	FAutoVariable<int, CvPlayer> m_iUnhappinessMod;
	FAutoVariable<int, CvPlayer> m_iCityCountUnhappinessMod;
	FAutoVariable<int, CvPlayer> m_iOccupiedPopulationUnhappinessMod;
	FAutoVariable<int, CvPlayer> m_iCapitalUnhappinessMod;
	FAutoVariable<int, CvPlayer> m_iCityRevoltCounter;
	FAutoVariable<int, CvPlayer> m_iHappinessPerGarrisonedUnitCount;
	FAutoVariable<int, CvPlayer> m_iHappinessPerTradeRouteCount;
	FAutoVariable<int, CvPlayer> m_iHappinessPerXPopulation;
#if defined(MOD_BALANCE_CORE_POLICIES)
	FAutoVariable<int, CvPlayer> m_iHappinessPerXPopulationGlobal;
	FAutoVariable<int, CvPlayer> m_iIdeologyPoint;
	FAutoVariable<int, CvPlayer> m_iNoXPLossUnitPurchase;
	FAutoVariable<int, CvPlayer> m_iXCSAlliesLowersPolicyNeedWonders;
	FAutoVariable<int, CvPlayer> m_iHappinessFromMinorCivs;
	FAutoVariable<int, CvPlayer> m_iPositiveWarScoreTourismMod;
	FAutoVariable<int, CvPlayer> m_iIsNoCSDecayAtWar;
	FAutoVariable<int, CvPlayer> m_iCanBullyFriendlyCS;
	FAutoVariable<int, CvPlayer> m_iBullyGlobalCSReduction;	
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	FAutoVariable<int, CvPlayer> m_iIsVassalsNoRebel;
	FAutoVariable<int, CvPlayer> m_iVassalCSBonusModifier;		
#endif
	FAutoVariable<int, CvPlayer> m_iHappinessFromLeagues;
	FAutoVariable<int, CvPlayer> m_iSpecialPolicyBuildingHappiness;  //unused
	FAutoVariable<int, CvPlayer> m_iWoundedUnitDamageMod;
	FAutoVariable<int, CvPlayer> m_iUnitUpgradeCostMod;
	FAutoVariable<int, CvPlayer> m_iBarbarianCombatBonus;
	FAutoVariable<int, CvPlayer> m_iAlwaysSeeBarbCampsCount;
	FAutoVariable<int, CvPlayer> m_iHappinessPerCity;
	FAutoVariable<int, CvPlayer> m_iHappinessPerXPolicies;
	FAutoVariable<int, CvPlayer> m_iExtraHappinessPerXPoliciesFromPolicies;
	FAutoVariable<int, CvPlayer> m_iHappinessPerXGreatWorks;
	FAutoVariable<int, CvPlayer> m_iEspionageModifier;
	FAutoVariable<int, CvPlayer> m_iSpyStartingRank;
#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
	FAutoVariable<int, CvPlayer> m_iConversionModifier;
#endif
	FAutoVariable<int, CvPlayer> m_iExtraLeagueVotes;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	FAutoVariable<int, CvPlayer> m_iImprovementLeagueVotes;
	FAutoVariable<int, CvPlayer> m_iFaithToVotes;
	FAutoVariable<int, CvPlayer> m_iCapitalsToVotes;
	FAutoVariable<int, CvPlayer> m_iDoFToVotes;
	FAutoVariable<int, CvPlayer> m_iRAToVotes;
	FAutoVariable<int, CvPlayer> m_iDefensePactsToVotes;
	FAutoVariable<int, CvPlayer> m_iGPExpendInfluence;
	FAutoVariable<bool, CvPlayer> m_bIsLeagueAid;
	FAutoVariable<bool, CvPlayer> m_bIsLeagueScholar;
	FAutoVariable<bool, CvPlayer> m_bIsLeagueArt;
	FAutoVariable<int, CvPlayer> m_iScienceRateFromLeague;
	FAutoVariable<int, CvPlayer> m_iScienceRateFromLeagueAid;
	FAutoVariable<int, CvPlayer> m_iLeagueCultureCityModifier;
#endif
	FAutoVariable<int, CvPlayer> m_iAdvancedStartPoints;
	FAutoVariable<int, CvPlayer> m_iAttackBonusTurns;
	FAutoVariable<int, CvPlayer> m_iCultureBonusTurns;
	FAutoVariable<int, CvPlayer> m_iTourismBonusTurns;
	FAutoVariable<int, CvPlayer> m_iGoldenAgeProgressMeter;
	FAutoVariable<int, CvPlayer> m_iGoldenAgeMeterMod;
	FAutoVariable<int, CvPlayer> m_iNumGoldenAges;
	FAutoVariable<int, CvPlayer> m_iGoldenAgeTurns;
	FAutoVariable<int, CvPlayer> m_iNumUnitGoldenAges;
	FAutoVariable<int, CvPlayer> m_iStrikeTurns;
	FAutoVariable<int, CvPlayer> m_iGoldenAgeModifier;
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
	FAutoVariable<int, CvPlayer> m_iProductionBonusTurnsConquest;
	FAutoVariable<int, CvPlayer> m_iCultureBonusTurnsConquest;
	FAutoVariable<int, CvPlayer> m_iFreeGreatPeopleCreated;
	FAutoVariable<int, CvPlayer> m_iFreeGreatGeneralsCreated;
	FAutoVariable<int, CvPlayer> m_iFreeGreatAdmiralsCreated;
#if defined(MOD_GLOBAL_SEPARATE_GP_COUNTERS)
	FAutoVariable<int, CvPlayer> m_iFreeGreatMerchantsCreated;
	FAutoVariable<int, CvPlayer> m_iFreeGreatScientistsCreated;
	FAutoVariable<int, CvPlayer> m_iFreeGreatEngineersCreated;
#endif
	FAutoVariable<int, CvPlayer> m_iFreeGreatWritersCreated;
	FAutoVariable<int, CvPlayer> m_iFreeGreatArtistsCreated;
	FAutoVariable<int, CvPlayer> m_iFreeGreatMusiciansCreated;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	FAutoVariable<int, CvPlayer> m_iFreeGreatDiplomatsCreated;
#endif
#if defined(MOD_BALANCE_CORE)
	FAutoVariable<int, CvPlayer> m_iGPExtra1Created;
	FAutoVariable<int, CvPlayer> m_iGPExtra2Created;
	FAutoVariable<int, CvPlayer> m_iGPExtra3Created;
	FAutoVariable<int, CvPlayer> m_iGPExtra4Created;
	FAutoVariable<int, CvPlayer> m_iGPExtra5Created;
	FAutoVariable<int, CvPlayer> m_iFreeGPExtra1Created;
	FAutoVariable<int, CvPlayer> m_iFreeGPExtra2Created;
	FAutoVariable<int, CvPlayer> m_iFreeGPExtra3Created;
	FAutoVariable<int, CvPlayer> m_iFreeGPExtra4Created;
	FAutoVariable<int, CvPlayer> m_iFreeGPExtra5Created;
#endif
#endif
	FAutoVariable<int, CvPlayer> m_iGreatPeopleCreated;
	FAutoVariable<int, CvPlayer> m_iGreatGeneralsCreated;
	FAutoVariable<int, CvPlayer> m_iGreatAdmiralsCreated;
#if defined(MOD_GLOBAL_SEPARATE_GP_COUNTERS)
	FAutoVariable<int, CvPlayer> m_iGreatMerchantsCreated;
	FAutoVariable<int, CvPlayer> m_iGreatScientistsCreated;
	FAutoVariable<int, CvPlayer> m_iGreatEngineersCreated;
#endif
	FAutoVariable<int, CvPlayer> m_iGreatWritersCreated;
	FAutoVariable<int, CvPlayer> m_iGreatArtistsCreated;
	FAutoVariable<int, CvPlayer> m_iGreatMusiciansCreated;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	FAutoVariable<int, CvPlayer> m_iGreatDiplomatsCreated;
	FAutoVariable<int, CvPlayer> m_iDiplomatsFromFaith;
#endif
#if defined(MOD_BALANCE_CORE)
	FAutoVariable<int, CvPlayer> m_iGPExtra1FromFaith;
	FAutoVariable<int, CvPlayer> m_iGPExtra2FromFaith;
	FAutoVariable<int, CvPlayer> m_iGPExtra3FromFaith;
	FAutoVariable<int, CvPlayer> m_iGPExtra4FromFaith;
	FAutoVariable<int, CvPlayer> m_iGPExtra5FromFaith;
#endif
	FAutoVariable<int, CvPlayer> m_iMerchantsFromFaith;
	FAutoVariable<int, CvPlayer> m_iScientistsFromFaith;
	FAutoVariable<int, CvPlayer> m_iWritersFromFaith;
	FAutoVariable<int, CvPlayer> m_iArtistsFromFaith;
	FAutoVariable<int, CvPlayer> m_iMusiciansFromFaith;
	FAutoVariable<int, CvPlayer> m_iGeneralsFromFaith;
	FAutoVariable<int, CvPlayer> m_iAdmiralsFromFaith;
	FAutoVariable<int, CvPlayer> m_iEngineersFromFaith;
	FAutoVariable<int, CvPlayer> m_iGreatPeopleThresholdModifier;
	FAutoVariable<int, CvPlayer> m_iGreatGeneralsThresholdModifier;
	FAutoVariable<int, CvPlayer> m_iGreatAdmiralsThresholdModifier;
	FAutoVariable<int, CvPlayer> m_iGreatGeneralCombatBonus;
	FAutoVariable<int, CvPlayer> m_iAnarchyNumTurns;
	FAutoVariable<int, CvPlayer> m_iPolicyCostModifier;
	FAutoVariable<int, CvPlayer> m_iGreatPeopleRateModifier;
	FAutoVariable<int, CvPlayer> m_iGreatPeopleRateModFromBldgs;
	FAutoVariable<int, CvPlayer> m_iGreatGeneralRateModifier;
	FAutoVariable<int, CvPlayer> m_iGreatGeneralRateModFromBldgs;
	FAutoVariable<int, CvPlayer> m_iDomesticGreatGeneralRateModifier;
	FAutoVariable<int, CvPlayer> m_iDomesticGreatGeneralRateModFromBldgs;
	FAutoVariable<int, CvPlayer> m_iGreatAdmiralRateModifier;
	FAutoVariable<int, CvPlayer> m_iGreatWriterRateModifier;
	FAutoVariable<int, CvPlayer> m_iGreatArtistRateModifier;
	FAutoVariable<int, CvPlayer> m_iGreatMusicianRateModifier;
	FAutoVariable<int, CvPlayer> m_iGreatMerchantRateModifier;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	FAutoVariable<int, CvPlayer> m_iGreatDiplomatRateModifier;
#endif
	FAutoVariable<int, CvPlayer> m_iGreatScientistRateModifier;
	FAutoVariable<int, CvPlayer> m_iGreatScientistBeakerModifier;
	FAutoVariable<int, CvPlayer> m_iGreatEngineerHurryMod;
	FAutoVariable<int, CvPlayer> m_iTechCostXCitiesModifier;
	FAutoVariable<int, CvPlayer> m_iTourismCostXCitiesMod;
	FAutoVariable<int, CvPlayer> m_iGreatEngineerRateModifier;
	FAutoVariable<int, CvPlayer> m_iGreatPersonExpendGold;
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	FAutoVariable<int, CvPlayer> m_iPovertyUnhappinessMod;
	FAutoVariable<int, CvPlayer> m_iDefenseUnhappinessMod;
	FAutoVariable<int, CvPlayer> m_iUnculturedUnhappinessMod;
	FAutoVariable<int, CvPlayer> m_iIlliteracyUnhappinessMod;
	FAutoVariable<int, CvPlayer> m_iMinorityUnhappinessMod;
	FAutoVariable<int, CvPlayer> m_iPovertyUnhappinessModCapital;
	FAutoVariable<int, CvPlayer> m_iDefenseUnhappinessModCapital;
	FAutoVariable<int, CvPlayer> m_iUnculturedUnhappinessModCapital;
	FAutoVariable<int, CvPlayer> m_iIlliteracyUnhappinessModCapital;
	FAutoVariable<int, CvPlayer> m_iMinorityUnhappinessModCapital;
	FAutoVariable<int, CvPlayer> m_iPuppetUnhappinessMod;
	FAutoVariable<int, CvPlayer> m_iNoUnhappfromXSpecialists;
	FAutoVariable<int, CvPlayer> m_iHappfromXSpecialists;
	FAutoVariable<int, CvPlayer> m_iNoUnhappfromXSpecialistsCapital;
	FAutoVariable<int, CvPlayer> m_iSpecialistFoodChange;
	FAutoVariable<int, CvPlayer> m_iWarWearinessModifier;
	FAutoVariable<int, CvPlayer> m_iWarScoreModifier;
#endif
#if defined(MOD_BALANCE_CORE_POLICIES)
	FAutoVariable<int, CvPlayer> m_iGarrisonsOccupiedUnhapppinessMod;
	FAutoVariable<int, CvPlayer> m_iXPopulationConscription;
	FAutoVariable<int, CvPlayer> m_iExtraMoves;
	FAutoVariable<int, CvPlayer> m_iNoUnhappinessExpansion;
	FAutoVariable<int, CvPlayer> m_iNoUnhappyIsolation;
	FAutoVariable<int, CvPlayer> m_iDoubleBorderGA;
	FAutoVariable<int, CvPlayer> m_iIncreasedQuestInfluence;
	FAutoVariable<int, CvPlayer> m_iCultureBombBoost;
	FAutoVariable<int, CvPlayer> m_iPuppetProdMod;
	FAutoVariable<int, CvPlayer> m_iOccupiedProdMod;
	FAutoVariable<int, CvPlayer> m_iGoldInternalTrade;
	FAutoVariable<int, CvPlayer> m_iFreeWCVotes;
	FAutoVariable<int, CvPlayer> m_iInfluenceGPExpend;
	FAutoVariable<int, CvPlayer> m_iFreeTradeRoute;
	FAutoVariable<int, CvPlayer> m_iFreeSpy;
	FAutoVariable<int, CvPlayer> m_iReligionDistance;
	FAutoVariable<int, CvPlayer> m_iPressureMod;
	FAutoVariable<int, CvPlayer> m_iTradeReligionModifier;
	FAutoVariable<int, CvPlayer> m_iCityStateCombatModifier;
#endif
#if defined(MOD_BALANCE_CORE_SPIES)
	FAutoVariable<bool, CvPlayer> m_bAdvancedActionsEnabled;
	FAutoVariable<int, CvPlayer> m_iAdvancedActionGold;
	FAutoVariable<int, CvPlayer> m_iAdvancedActionScience;
	FAutoVariable<int, CvPlayer> m_iAdvancedActionUnrest;
	FAutoVariable<int, CvPlayer> m_iAdvancedActionRebellion;
	FAutoVariable<int, CvPlayer> m_iAdvancedActionGP;
	FAutoVariable<int, CvPlayer> m_iAdvancedActionUnit;
	FAutoVariable<int, CvPlayer> m_iAdvancedActionWonder;
	FAutoVariable<int, CvPlayer> m_iAdvancedActionBuilding;
	FAutoVariable<int, CvPlayer> m_iCannotFailSpies;
	FAutoVariable<int, CvPlayer> m_iMaxAirUnits;
#endif
#if defined(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
	FAutoVariable<int, CvPlayer> m_iInvestmentModifier;
	FAutoVariable<int, CvPlayer> m_iMissionInfluenceModifier;
	FAutoVariable<int, CvPlayer> m_iHappinessPerActiveTradeRoute;
	FAutoVariable<int, CvPlayer> m_iCSResourcesCountMonopolies;
	FAutoVariable<int, CvPlayer> m_iConquestPerEraBuildingProductionMod;
	FAutoVariable<int, CvPlayer> m_iAdmiralLuxuryBonus;
	FAutoVariable<int, CvPlayer> m_iPuppetYieldPenaltyMod;
	FAutoVariable<int, CvPlayer> m_iNeedsModifierFromAirUnits;
	FAutoVariable<int, CvPlayer> m_iFlatDefenseFromAirUnits;
#endif
	FAutoVariable<int, CvPlayer> m_iMaxGlobalBuildingProductionModifier;
	FAutoVariable<int, CvPlayer> m_iMaxTeamBuildingProductionModifier;
	FAutoVariable<int, CvPlayer> m_iMaxPlayerBuildingProductionModifier;
	FAutoVariable<int, CvPlayer> m_iFreeExperience;
	FAutoVariable<int, CvPlayer> m_iFreeExperienceFromBldgs;
	FAutoVariable<int, CvPlayer> m_iFreeExperienceFromMinors;
	FAutoVariable<int, CvPlayer> m_iFeatureProductionModifier;
	FAutoVariable<int, CvPlayer> m_iWorkerSpeedModifier;
	FAutoVariable<int, CvPlayer> m_iImprovementCostModifier;
	FAutoVariable<int, CvPlayer> m_iImprovementUpgradeRateModifier;
	FAutoVariable<int, CvPlayer> m_iSpecialistProductionModifier;
	FAutoVariable<int, CvPlayer> m_iMilitaryProductionModifier;
	FAutoVariable<int, CvPlayer> m_iSpaceProductionModifier;
	FAutoVariable<int, CvPlayer> m_iCityDefenseModifier;
	FAutoVariable<int, CvPlayer> m_iUnitFortificationModifier;
	FAutoVariable<int, CvPlayer> m_iUnitBaseHealModifier;
	FAutoVariable<int, CvPlayer> m_iWonderProductionModifier;
	FAutoVariable<int, CvPlayer> m_iSettlerProductionModifier;
	FAutoVariable<int, CvPlayer> m_iCapitalSettlerProductionModifier;
	FAutoVariable<int, CvPlayer> m_iUnitProductionMaintenanceMod;
	FAutoVariable<int, CvPlayer> m_iUnitGrowthMaintenanceMod;
	FAutoVariable<int, CvPlayer> m_iPolicyCostBuildingModifier;
	FAutoVariable<int, CvPlayer> m_iPolicyCostMinorCivModifier;
	FAutoVariable<int, CvPlayer> m_iInfluenceSpreadModifier;
	FAutoVariable<int, CvPlayer> m_iExtraVotesPerDiplomat;
	FAutoVariable<int, CvPlayer> m_iNumNukeUnits;
	FAutoVariable<int, CvPlayer> m_iNumOutsideUnits;
	FAutoVariable<int, CvPlayer> m_iBaseFreeUnits;
	FAutoVariable<int, CvPlayer> m_iBaseFreeMilitaryUnits;
	FAutoVariable<int, CvPlayer> m_iFreeUnitsPopulationPercent;
	FAutoVariable<int, CvPlayer> m_iFreeMilitaryUnitsPopulationPercent;
	FAutoVariable<int, CvPlayer> m_iGoldPerUnit;
	FAutoVariable<int, CvPlayer> m_iGoldPerMilitaryUnit;
	FAutoVariable<int, CvPlayer> m_iImprovementGoldMaintenanceMod;
#if defined(MOD_CIV6_WORKER)
	FAutoVariable<int, CvPlayer> m_iRouteBuilderCostMod;
#endif
	FAutoVariable<int, CvPlayer> m_iBuildingGoldMaintenanceMod;
	FAutoVariable<int, CvPlayer> m_iUnitGoldMaintenanceMod;
	FAutoVariable<int, CvPlayer> m_iUnitSupplyMod;
	FAutoVariable<int, CvPlayer> m_iExtraUnitCost;
	FAutoVariable<int, CvPlayer> m_iNumMilitaryUnits;
	FAutoVariable<int, CvPlayer> m_iHappyPerMilitaryUnit;
	FAutoVariable<int, CvPlayer> m_iHappinessToCulture;
	FAutoVariable<int, CvPlayer> m_iHappinessToScience;
	FAutoVariable<int, CvPlayer> m_iHalfSpecialistUnhappinessCount;
	FAutoVariable<int, CvPlayer> m_iHalfSpecialistFoodCount;
#if defined(MOD_BALANCE_CORE)
	FAutoVariable<int, CvPlayer> m_iHalfSpecialistFoodCapitalCount;
	FAutoVariable<int, CvPlayer> m_iTradeRouteLandDistanceModifier;
	FAutoVariable<int, CvPlayer> m_iTradeRouteSeaDistanceModifier;
	FAutoVariable<bool, CvPlayer> m_bNullifyInfluenceModifier;
#endif
	FAutoVariable<int, CvPlayer> m_iMilitaryFoodProductionCount;
	FAutoVariable<int, CvPlayer> m_iGoldenAgeCultureBonusDisabledCount;
	FAutoVariable<int, CvPlayer> m_iNumMissionarySpreads;
	FAutoVariable<int, CvPlayer> m_iSecondReligionPantheonCount;
	FAutoVariable<int, CvPlayer> m_iEnablesSSPartHurryCount;
	FAutoVariable<int, CvPlayer> m_iEnablesSSPartPurchaseCount;
	FAutoVariable<int, CvPlayer> m_iConscriptCount;
	FAutoVariable<int, CvPlayer> m_iMaxConscript;
	FAutoVariable<int, CvPlayer> m_iHighestUnitLevel;
	FAutoVariable<int, CvPlayer> m_iOverflowResearch;
	FAutoVariable<int, CvPlayer> m_iExpModifier;
	FAutoVariable<int, CvPlayer> m_iExpInBorderModifier;
	FAutoVariable<int, CvPlayer> m_iLevelExperienceModifier;
	FAutoVariable<int, CvPlayer> m_iMinorQuestFriendshipMod;
	FAutoVariable<int, CvPlayer> m_iMinorGoldFriendshipMod;
	FAutoVariable<int, CvPlayer> m_iMinorFriendshipMinimum;
	FAutoVariable<int, CvPlayer> m_iMinorFriendshipDecayMod;
	FAutoVariable<int, CvPlayer> m_iMinorScienceAlliesCount;
	FAutoVariable<int, CvPlayer> m_iMinorResourceBonusCount;
	FAutoVariable<int, CvPlayer> m_iAbleToAnnexCityStatesCount;
	FAutoVariable<int, CvPlayer> m_iOnlyTradeSameIdeology;
#if defined(MOD_BALANCE_CORE)
	FAutoVariable<int, CvPlayer> m_iSupplyFreeUnits; //military units which don't count against the supply limit
	FAutoVariable<std::vector<CvString>, CvPlayer> m_aistrInstantYield;
	std::map<int, CvString> m_aistrInstantGreatPersonProgress;
	FAutoVariable<std::vector<bool>, CvPlayer> m_abActiveContract;
	FAutoVariable<int, CvPlayer> m_iJFDReformCooldownRate;
	FAutoVariable<int, CvPlayer> m_iJFDGovernmentCooldownRate;
	FAutoVariable<CvString, CvPlayer> m_strJFDPoliticKey;
	FAutoVariable<CvString, CvPlayer> m_strJFDLegislatureName;
	FAutoVariable<int, CvPlayer> m_iJFDPoliticLeader;
	FAutoVariable<int, CvPlayer> m_iJFDSovereignty;
	FAutoVariable<int, CvPlayer> m_iJFDGovernment;
	FAutoVariable<int, CvPlayer> m_iJFDReformCooldown;
	FAutoVariable<int, CvPlayer> m_iJFDGovernmentCooldown;
	FAutoVariable<int, CvPlayer> m_iJFDPiety;
	FAutoVariable<int, CvPlayer> m_iJFDPietyRate;
	FAutoVariable<int, CvPlayer> m_iJFDConversionTurn;
	FAutoVariable<bool, CvPlayer> m_bJFDSecularized;
	FAutoVariable<CvString, CvPlayer> m_strJFDCurrencyName;
	FAutoVariable<int, CvPlayer> m_iJFDProsperity;
	FAutoVariable<int, CvPlayer> m_iJFDCurrency;
	FAutoVariable<int, CvPlayer> m_iGoldenAgeTourism;
	FAutoVariable<int, CvPlayer> m_iExtraCultureandScienceTradeRoutes;
	FAutoVariable<int, CvPlayer> m_iArchaeologicalDigTourism;
	FAutoVariable<int, CvPlayer> m_iUpgradeCSVassalTerritory;
	FAutoVariable<int, CvPlayer> m_iRazingSpeedBonus;
	FAutoVariable<int, CvPlayer> m_iNoPartisans;
	FAutoVariable<int, CvPlayer> m_iSpawnCooldown;
	FAutoVariable<int, CvPlayer> m_iAbleToMarryCityStatesCount;
	FAutoVariable<bool, CvPlayer> m_bTradeRoutesInvulnerable;
	FAutoVariable<int, CvPlayer> m_iTRSpeedBoost;
	FAutoVariable<int, CvPlayer> m_iVotesPerGPT;
	FAutoVariable<int, CvPlayer> m_iTRVisionBoost;
	FAutoVariable<int, CvPlayer> m_iEventTourism;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiGlobalTourismAlreadyReceived;
	FAutoVariable<int, CvPlayer> m_iEventTourismCS;
	FAutoVariable<int, CvPlayer> m_iNumHistoricEvent;
	FAutoVariable<int, CvPlayer> m_iSingleVotes;
	FAutoVariable<int, CvPlayer> m_iMonopolyModFlat;
	FAutoVariable<int, CvPlayer> m_iMonopolyModPercent;
	FAutoVariable<int, CvPlayer> m_iCachedValueOfPeaceWithHuman;
	FAutoVariable<int, CvPlayer> m_iFaithPurchaseCooldown;
	FAutoVariable<int, CvPlayer> m_iCSAllies;
	FAutoVariable<int, CvPlayer> m_iCSFriends;
	std::vector<int> m_piCityFeatures;
	std::vector<int> m_piNumBuildings;
	std::vector<int> m_piNumBuildingsInPuppets;
	FAutoVariable<int, CvPlayer> m_iCitiesNeedingTerrainImprovements;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiBestMilitaryCombatClassCity;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiBestMilitaryDomainCity;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiEventChoiceDuration;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiEventIncrement;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiEventCooldown;
	FAutoVariable<std::vector<bool>, CvPlayer> m_abEventActive;
	FAutoVariable<std::vector<bool>, CvPlayer> m_abEventChoiceActive;
	FAutoVariable<std::vector<bool>, CvPlayer> m_abEventChoiceFired;
	FAutoVariable<std::vector<bool>, CvPlayer> m_abEventFired;
	FAutoVariable<int, CvPlayer> m_iPlayerEventCooldown;
	FAutoVariable<std::vector<bool>, CvPlayer> m_abNWOwned;
	FAutoVariable<std::vector<int>, CvPlayer> m_paiUnitClassProductionModifiers;
	FAutoVariable<int, CvPlayer> m_iExtraSupplyPerPopulation;
	FAutoVariable<int, CvPlayer> m_iCitySupplyFlatGlobal;
	FAutoVariable<int, CvPlayer> m_iMissionaryExtraStrength;
#endif
	FAutoVariable<int, CvPlayer> m_iFreeSpecialist;
	FAutoVariable<int, CvPlayer> m_iCultureBombTimer;
	FAutoVariable<int, CvPlayer> m_iConversionTimer;
	FAutoVariable<int, CvPlayer> m_iCapitalCityID;
	FAutoVariable<int, CvPlayer> m_iCitiesLost;
	FAutoVariable<int, CvPlayer> m_iMilitaryRating;
	FAutoVariable<int, CvPlayer> m_iMilitaryMight;
	FAutoVariable<int, CvPlayer> m_iEconomicMight;
	FAutoVariable<int, CvPlayer> m_iProductionMight;
	FAutoVariable<int, CvPlayer> m_iTurnSliceMightRecomputed;
	FAutoVariable<int, CvPlayer> m_iNewCityExtraPopulation;
	FAutoVariable<int, CvPlayer> m_iFreeFoodBox;
	FAutoVariable<int, CvPlayer> m_iScenarioScore1;
	FAutoVariable<int, CvPlayer> m_iScenarioScore2;
	FAutoVariable<int, CvPlayer> m_iScenarioScore3;
	FAutoVariable<int, CvPlayer> m_iScenarioScore4;
	FAutoVariable<int, CvPlayer> m_iScoreFromFutureTech;
	FAutoVariable<int, CvPlayer> m_iCombatExperience;
	FAutoVariable<int, CvPlayer> m_iLifetimeCombatExperience;
	FAutoVariable<int, CvPlayer> m_iNavalCombatExperience;
#if defined(MOD_UNITS_XP_TIMES_100)
	FAutoVariable<int, CvPlayer> m_iCombatExperienceTimes100;
	FAutoVariable<int, CvPlayer> m_iLifetimeCombatExperienceTimes100;
	FAutoVariable<int, CvPlayer> m_iNavalCombatExperienceTimes100;
#endif
	FAutoVariable<int, CvPlayer> m_iBorderObstacleCount;
#if defined(HH_MOD_BUILDINGS_FRUITLESS_PILLAGE)
	FAutoVariable<int, CvPlayer> m_iBorderGainlessPillageCount;
#endif
	FAutoVariable<int, CvPlayer> m_iPopRushHurryCount;
	FAutoVariable<int, CvPlayer> m_iTotalImprovementsBuilt;
	FAutoVariable<int, CvPlayer> m_iNextOperationID;
	FAutoVariable<int, CvPlayer> m_iCostNextPolicy;
	FAutoVariable<int, CvPlayer> m_iNumBuilders;
	FAutoVariable<int, CvPlayer> m_iMaxNumBuilders;
	FAutoVariable<int, CvPlayer> m_iCityStrengthMod;
	FAutoVariable<int, CvPlayer> m_iCityGrowthMod;
	FAutoVariable<int, CvPlayer> m_iCapitalGrowthMod;
	FAutoVariable<int, CvPlayer> m_iNumPlotsBought;
	FAutoVariable<int, CvPlayer> m_iPlotGoldCostMod;
#if defined(MOD_TRAITS_CITY_WORKING) || defined(MOD_BUILDINGS_CITY_WORKING) || defined(MOD_POLICIES_CITY_WORKING) || defined(MOD_TECHS_CITY_WORKING)
	FAutoVariable<int, CvPlayer> m_iCityWorkingChange;
#endif
#if defined(MOD_TRAITS_CITY_AUTOMATON_WORKERS) || defined(MOD_BUILDINGS_CITY_AUTOMATON_WORKERS) || defined(MOD_POLICIES_CITY_AUTOMATON_WORKERS) || defined(MOD_TECHS_CITY_AUTOMATON_WORKERS)
	FAutoVariable<int, CvPlayer> m_iCityAutomatonWorkersChange;
#endif
	FAutoVariable<int, CvPlayer> m_iPlotCultureCostModifier;
	FAutoVariable<int, CvPlayer> m_iPlotCultureExponentModifier;
	FAutoVariable<int, CvPlayer> m_iNumCitiesPolicyCostDiscount;
	FAutoVariable<int, CvPlayer> m_iGarrisonedCityRangeStrikeModifier;
	FAutoVariable<int, CvPlayer> m_iGarrisonFreeMaintenanceCount;
	FAutoVariable<int, CvPlayer> m_iNumCitiesFreeCultureBuilding;
	FAutoVariable<int, CvPlayer> m_iNumCitiesFreeFoodBuilding;
	FAutoVariable<int, CvPlayer> m_iUnitPurchaseCostModifier;
	FAutoVariable<int, CvPlayer> m_iAllFeatureProduction;
	FAutoVariable<int, CvPlayer> m_iCityDistanceHighwaterMark; // this is used to determine camera zoom
	FAutoVariable<int, CvPlayer> m_iOriginalCapitalX;
	FAutoVariable<int, CvPlayer> m_iOriginalCapitalY;
	FAutoVariable<int, CvPlayer> m_iHolyCityX;
	FAutoVariable<int, CvPlayer> m_iHolyCityY;
	FAutoVariable<int, CvPlayer> m_iNumWonders;
	FAutoVariable<int, CvPlayer> m_iNumPolicies;
	FAutoVariable<int, CvPlayer> m_iNumGreatPeople;
	FAutoVariable<int, CvPlayer> m_iCityConnectionHappiness;
	FAutoVariable<int, CvPlayer> m_iHolyCityID;
	FAutoVariable<int, CvPlayer> m_iTurnsSinceSettledLastCity;
	FAutoVariable<int, CvPlayer> m_iNumNaturalWondersDiscoveredInArea;
	FAutoVariable<int, CvPlayer> m_iStrategicResourceMod;
	FAutoVariable<int, CvPlayer> m_iSpecialistCultureChange;
	FAutoVariable<int, CvPlayer> m_iGreatPeopleSpawnCounter;

	FAutoVariable<int, CvPlayer> m_iFreeTechCount;
	FAutoVariable<int, CvPlayer> m_iMedianTechPercentage;
	FAutoVariable<int, CvPlayer> m_iNumFreePolicies;
	FAutoVariable<int, CvPlayer> m_iNumFreePoliciesEver; 
	FAutoVariable<int, CvPlayer> m_iNumFreeTenets;
    FAutoVariable<int, CvPlayer> m_iMaxEffectiveCities;

	FAutoVariable<int, CvPlayer> m_iLastSliceMoved;

	FAutoVariable<uint, CvPlayer> m_uiStartTime;  // XXX save these?

	FAutoVariable<bool, CvPlayer> m_bHasUUPeriod;
	FAutoVariable<bool, CvPlayer> m_bHasBetrayedMinorCiv;
	FAutoVariable<bool, CvPlayer> m_bAlive;
	FAutoVariable<bool, CvPlayer> m_bEverAlive;
	FAutoVariable<bool, CvPlayer> m_bPotentiallyAlive;
	FAutoVariable<bool, CvPlayer> m_bBeingResurrected;
	FAutoVariable<bool, CvPlayer> m_bTurnActive;
	FAutoVariable<bool, CvPlayer> m_bAutoMoves;					// Signal that we can process the auto moves when ready.
	bool						  m_bProcessedAutoMoves;		// Signal that we have processed the auto moves
	FAutoVariable<bool, CvPlayer> m_bEndTurn;					// Signal that the player has completed their turn.  The turn will still be active until the auto-moves have been processed.
	FAutoVariable<bool, CvPlayer> m_bDynamicTurnsSimultMode;
	FAutoVariable<bool, CvPlayer> m_bPbemNewTurn;
	FAutoVariable<bool, CvPlayer> m_bExtendedGame;
	FAutoVariable<bool, CvPlayer> m_bFoundedFirstCity;
	FAutoVariable<int, CvPlayer> m_iNumCitiesFounded;
	FAutoVariable<bool, CvPlayer> m_bStrike;
	FAutoVariable<bool, CvPlayer> m_bCramped;
	FAutoVariable<bool, CvPlayer> m_bLostCapital;
	FAutoVariable<PlayerTypes, CvPlayer> m_eConqueror;
	FAutoVariable<bool, CvPlayer> m_bLostHolyCity;
	FAutoVariable<PlayerTypes, CvPlayer> m_eHolyCityConqueror;
	FAutoVariable<bool, CvPlayer> m_bHasAdoptedStateReligion;
	FAutoVariable<bool, CvPlayer> m_bAlliesGreatPersonBiasApplied;

	FAutoVariable<std::vector<int>, CvPlayer> m_aiCityYieldChange;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiCoastalCityYieldChange;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiCapitalYieldChange;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiCapitalYieldPerPopChange;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiCapitalYieldPerPopChangeEmpire;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiSeaPlotYield;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiYieldRateModifier;
#if defined(MOD_BALANCE_CORE_POLICIES)
	FAutoVariable<std::vector<int>, CvPlayer> m_paiJFDPoliticPercent;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiYieldFromMinors;
	FAutoVariable<std::vector<int>, CvPlayer> m_paiResourceFromCSAlliances;
	FAutoVariable<std::vector<int>, CvPlayer> m_paiResourceOverValue;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiYieldFromBirth;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiYieldFromBirthCapital;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiYieldFromDeath;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiYieldFromPillage;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiYieldFromVictory;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiYieldFromConstruction;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiYieldFromwonderConstruction;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiYieldFromTech;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiYieldFromBorderGrowth;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiYieldGPExpend;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiConquerorYield;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiFounderYield;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiArtifactYieldBonus;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiArtYieldBonus;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiMusicYieldBonus;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiLitYieldBonus;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiFilmYieldBonus;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiRelicYieldBonus;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiReligionYieldRateModifier;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiGoldenAgeYieldMod;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiYieldFromNonSpecialistCitizens;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiYieldModifierFromGreatWorks;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiYieldModifierFromActiveSpies;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiYieldFromDelegateCount;
	FAutoVariable<std::vector<int>, CvPlayer> m_paiBuildingClassCulture;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiDomainFreeExperiencePerGreatWorkGlobal;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiCityYieldModFromMonopoly;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiDomainDiversity;
	FAutoVariable<bool, CvPlayer> m_bAllowsProductionTradeRoutesGlobal;
	FAutoVariable<bool, CvPlayer> m_bAllowsFoodTradeRoutesGlobal;
#endif
#if defined(MOD_BALANCE_CORE)
	std::map<int, int> m_piDomainFreeExperience;
#endif

	FAutoVariable<std::vector<int>, CvPlayer> m_aiCapitalYieldRateModifier;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiExtraYieldThreshold;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiSpecialistExtraYield;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiProximityToPlayer;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiResearchAgreementCounter;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiIncomingUnitTypes;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiIncomingUnitCountdowns;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiMinorFriendshipAnchors; // DEPRECATED
	FAutoVariable<std::vector<int>, CvPlayer> m_aiSiphonLuxuryCount;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiGreatWorkYieldChange;

	typedef std::pair<uint, int> PlayerOptionEntry;
	typedef std::vector< PlayerOptionEntry > PlayerOptionsVector;
	FAutoVariable<PlayerOptionsVector, CvPlayer> m_aOptions;

	FAutoVariable<CvString, CvPlayer> m_strReligionKey;
	FAutoVariable<CvString, CvPlayer> m_strScriptData;

	CvString m_strEmbarkedGraphicOverride;

	FAutoVariable<std::vector<int>, CvPlayer> m_paiNumResourceUsed;
	FAutoVariable<std::vector<int>, CvPlayer> m_paiNumResourceTotal;
	FAutoVariable<std::vector<int>, CvPlayer> m_paiResourceGiftedToMinors;
	FAutoVariable<std::vector<int>, CvPlayer> m_paiResourceExport;
	FAutoVariable<std::vector<int>, CvPlayer> m_paiResourceImport;
	FAutoVariable<std::vector<int>, CvPlayer> m_paiResourceFromMinors;
	FAutoVariable<std::vector<int>, CvPlayer> m_paiResourcesSiphoned;
	FAutoVariable<std::vector<int>, CvPlayer> m_paiImprovementCount;
#if defined(MOD_BALANCE_CORE)
	FAutoVariable<std::vector<int>, CvPlayer> m_paiTotalImprovementsBuilt;
#endif
	FAutoVariable<std::vector<int>, CvPlayer> m_paiFreeBuildingCount;
	FAutoVariable<std::vector<int>, CvPlayer> m_paiFreePromotionCount;
	FAutoVariable<std::vector<int>, CvPlayer> m_paiUnitCombatProductionModifiers;
	FAutoVariable<std::vector<int>, CvPlayer> m_paiUnitCombatFreeExperiences;
	FAutoVariable<std::vector<int>, CvPlayer> m_paiUnitClassCount;
	FAutoVariable<std::vector<int>, CvPlayer> m_paiUnitClassMaking;
	FAutoVariable<std::vector<int>, CvPlayer> m_paiBuildingClassCount;
	FAutoVariable<std::vector<int>, CvPlayer> m_paiBuildingClassMaking;
	FAutoVariable<std::vector<int>, CvPlayer> m_paiProjectMaking;
	FAutoVariable<std::vector<int>, CvPlayer> m_paiHurryCount;
	FAutoVariable<std::vector<int>, CvPlayer> m_paiHurryModifier;

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	FAutoVariable<bool, CvPlayer> m_bVassalLevy;
	FAutoVariable<int, CvPlayer> m_iVassalGoldMaintenanceMod;
#endif
#if defined(MOD_BALANCE_CORE)
	FAutoVariable<std::vector<int>, CvPlayer> m_paiNumCitiesFreeChosenBuilding;
	FAutoVariable<std::vector<int>, CvPlayer> m_pabFreeChosenBuildingNewCity;
	FAutoVariable<std::vector<int>, CvPlayer> m_pabAllCityFreeBuilding;
	FAutoVariable<std::vector<int>, CvPlayer> m_pabNewFoundCityFreeUnit;
	FAutoVariable<std::vector<int>, CvPlayer> m_pabNewFoundCityFreeBuilding;
#endif

	FAutoVariable<std::vector<bool>, CvPlayer> m_pabLoyalMember;

#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	FAutoVariable<std::vector<bool>, CvPlayer> m_pabHasGlobalMonopoly;
	FAutoVariable<std::vector<bool>, CvPlayer> m_pabHasStrategicMonopoly;
	std::vector<ResourceTypes> m_vResourcesWGlobalMonopoly;
	std::vector<ResourceTypes> m_vResourcesWStrategicMonopoly;
#endif

	FAutoVariable<std::vector<bool>, CvPlayer> m_pabGetsScienceFromPlayer;

	FAutoVariable< std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > >, CvPlayer> m_ppaaiSpecialistExtraYield;
#if defined(MOD_API_UNIFIED_YIELDS) && defined(MOD_API_PLOT_YIELDS)
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiPlotYieldChange;
#endif
#if defined(MOD_API_UNIFIED_YIELDS)
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiImprovementYieldChange;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiFeatureYieldChange;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiResourceYieldChange;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiTerrainYieldChange;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiTradeRouteYieldChange;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiSpecialistYieldChange;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiGreatPersonExpendedYield;
	std::vector<int> m_piGoldenAgeGreatPersonRateModifier;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiUnimprovedFeatureYieldChange;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiCityYieldFromUnimprovedFeature;
	std::vector< Firaxis::Array<int, NUM_MAJOR_CIV_APPROACHES > > m_ppiApproachScratchValue;
	std::vector<int> m_piYieldFromKills;
	std::vector<int> m_piYieldFromBarbarianKills;
	std::vector<int> m_piYieldChangeTradeRoute;
	std::vector<int> m_piYieldChangesNaturalWonder;
	std::vector<int> m_piYieldChangesPerReligion;
	std::vector<int> m_piYieldChangeWorldWonder;
	std::vector<int> m_piYieldFromMinorDemand;
	std::vector<int> m_piYieldFromWLTKD;
	FAutoVariable< std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > >, CvPlayer> m_ppiBuildingClassYieldChange;
#endif
#if defined(MOD_API_UNIFIED_YIELDS) && defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	std::map<GreatPersonTypes, std::map<MonopolyTypes, int>> m_ppiSpecificGreatPersonRateModifierFromMonopoly; // Note that m_ppiSpecificGreatPersonRateModifierFromMonopoly does not have to be saved in kStream because setHasGlobalMonopoly and setHasStrategicMonopoly are called on game load
#endif
	FAutoVariable< std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > >, CvPlayer> m_ppaaiImprovementYieldChange;

	// Obsolete: only used to read old saves
	FAutoVariable< std::vector< Firaxis::Array< int, NUM_YIELD_TYPES > >, CvPlayer> m_ppaaiBuildingClassYieldMod;

	CvUnitCycler	m_UnitCycle;	

	// slewis's tutorial variables!
	FAutoVariable<bool, CvPlayer> m_bEverPoppedGoody;
	FAutoVariable<bool, CvPlayer> m_bEverTrainedBuilder;
	// end slewis's tutorial variables

	EndTurnBlockingTypes  m_eEndTurnBlockingType;
	int  m_iEndTurnBlockingNotificationIndex;

	CLinkList<TechTypes> m_researchQueue;
	CLinkList<CvString> m_cityNames;

	TContainer<CvCityAI> m_cities;
	TContainer<CvUnit> m_units;
	TContainer<CvArmyAI> m_armyAIs;

	std::map<int, CvAIOperation*> m_AIOperations;
	std::map<int, CvAIOperation*>::iterator m_CurrentOperation;

	std::vector< std::pair<int, PlayerVoteTypes> > m_aVote;
	std::vector< std::pair<UnitClassTypes, int> > m_aUnitExtraCosts;

	std::vector<CvString> m_ReplayDataSets;
	std::vector<TurnData> m_ReplayDataSetValues;

	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiInstantYieldHistoryValues;
	std::vector< Firaxis::Array<int, MAX_MAJOR_CIVS> > m_ppiInstantTourismHistoryValues;

	void doResearch();
	void doWarnings();

	// Danger plots!
	CvDangerPlots* m_pDangerPlots;

#if defined(MOD_BALANCE_CORE_SETTLER)
	FAutoVariable<int, CvPlayer> m_iFoundValueOfCapital;
	std::vector<int> m_viPlotFoundValues;
	int	m_iPlotFoundValuesUpdateTurn;
#endif

	//plots we have pledged no to settle
	std::set<int> m_noSettlingPlots;

	// Policies
	CvPlayerPolicies* m_pPlayerPolicies;
	void processPolicies(PolicyTypes ePolicy, int iChange);

	// AI Strategies
	CvEconomicAI* m_pEconomicAI;
	CvMilitaryAI* m_pMilitaryAI;
	CvCitySpecializationAI* m_pCitySpecializationAI;
	CvWonderProductionAI* m_pWonderProductionAI;

	// AI Grand Strategies
	CvGrandStrategyAI* m_pGrandStrategyAI;

	// Diplomacy AI
	CvDiplomacyAI* m_pDiplomacyAI;

	// Religion AI
	CvPlayerReligions* m_pReligions;
	CvReligionAI* m_pReligionAI;

#if defined(MOD_BALANCE_CORE)
	CvPlayerCorporations* m_pCorporations;
	CvPlayerContracts* m_pContracts;
#endif

	// AI Tactics
	CvTacticalAI* m_pTacticalAI;
	CvHomelandAI* m_pHomelandAI;

	// Techs
	CvPlayerTechs* m_pPlayerTechs;

	// Flavor Manager
	CvFlavorManager* m_pFlavorManager;

	// Minor Civ AI
	CvMinorCivAI* m_pMinorCivAI;

	// Deal AI
	CvDealAI* m_pDealAI;

	// Builder Tasking AI
	CvBuilderTaskingAI* m_pBuilderTaskingAI;

	// City Connections
	CvCityConnections* m_pCityConnections;

	// Espionage
	CvPlayerEspionage* m_pEspionage;
	CvEspionageAI* m_pEspionageAI;

	// Trade
	CvPlayerTrade* m_pTrade;
	CvTradeAI* m_pTradeAI;

	// League AI
	CvLeagueAI* m_pLeagueAI;

	// Culture
	CvPlayerCulture* m_pCulture;

	CvNotifications* m_pNotifications;
	CvDiplomacyRequests* m_pDiplomacyRequests;

	PlotIndexContainer m_aiPlots;

	// Treasury
	CvTreasury* m_pTreasury;

	CvPlayerTraits* m_pTraits;

	// human player wanted to end turn processing but hasn't received
	// the net turn complete message
	bool m_activeWaitingForEndTurnMessage;
	int  m_endTurnBusyUnitUpdatesLeft;

	int m_lastGameTurnInitialAIProcessed;

	ConqueredByBoolField m_bfEverConqueredBy;

	FAutoVariable<int, CvPlayer> m_iNumFreeGreatPeople;
	FAutoVariable<int, CvPlayer> m_iNumMayaBoosts;
	FAutoVariable<int, CvPlayer> m_iNumFaithGreatPeople;
	FAutoVariable<int, CvPlayer> m_iNumArchaeologyChoices;

	FaithPurchaseTypes m_eFaithPurchaseType;
	FAutoVariable<int, CvPlayer> m_iFaithPurchaseIndex;

	void checkArmySizeAchievement();

	friend class CvPlayerManager;

	CvPlayerAchievements m_kPlayerAchievements;

#if defined(MOD_BALANCE_CORE_MILITARY)
	//percent
	FAutoVariable<int, CvPlayer> m_iFractionOriginalCapitalsUnderControl;
	FAutoVariable<int, CvPlayer> m_iAvgUnitExp100;
	std::vector< std::pair<int,int> > m_unitsAreaEffectPositive; //unit / plot
	std::vector< std::pair<int,int> > m_unitsAreaEffectNegative; //unit / plot
	std::vector< std::pair<int,int> > m_unitsAreaEffectPromotion; //unit / plot
	std::vector< std::pair<int,int> > m_unitsWhichCanIntercept; //unit / plot
	std::vector<int> m_plotsAreaEffectPositiveFromTraits;
	std::vector<PlayerTypes> m_playersWeAreAtWarWith;
	std::vector<PlayerTypes> m_playersAtWarWithInFuture;
#endif

	mutable int m_iNumUnitsSuppliedCached; //not serialized

#if defined(MOD_BATTLE_ROYALE)
	FAutoVariable<int, CvPlayer> m_iNumMilitarySeaUnits;
	FAutoVariable<int, CvPlayer> m_iNumMilitaryAirUnits;
	FAutoVariable<int, CvPlayer> m_iNumMilitaryLandUnits;
	FAutoVariable<int, CvPlayer> m_iMilitarySeaMight;
	FAutoVariable<int, CvPlayer> m_iMilitaryAirMight;
	FAutoVariable<int, CvPlayer> m_iMilitaryLandMight;
#endif

};

bool CancelActivePlayerEndTurn();

namespace FSerialization
{
void SyncPlayer();
void ClearPlayerDeltas();
}

#endif
