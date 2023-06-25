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

#include "CvCityAI.h"
#include "CvUnit.h"
#include "CvArmyAI.h"
#include "LinkedList.h"
#include "CvPreGame.h"
#include "CvAchievementUnlocker.h"
#include "CvUnitCycler.h"
#include "TContainer.h"
#if defined(MOD_BALANCE_CORE)
#include "CvMinorCivAI.h"
#endif
#include "CvSerialize.h"

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
class CvDistanceMap;
#if defined(MOD_BALANCE_CORE)
class CvPlayerCorporations;
class CvPlayerContracts;
#endif
class CvCityConnections;
class CvNotifications;
#if defined(MOD_WH_MILITARY_LOG)
class CvEventLog;
#endif
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

const size_t INSTANT_YIELD_HISTORY_LENGTH = 30u;


struct SPlayerActiveEspionageEvent
{
	PlayerTypes eOtherPlayer;
	bool bIncoming;
	bool bIdentified;
	int iStartTurn;
	int iEndTurn;
	YieldTypes eYield;
	int iAmount;
};

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
	void initFreeUnits();
	void addFreeUnitAI(UnitAITypes eUnitAI, bool bGameStart, int iCount);
	CvPlot* addFreeUnit(UnitTypes eUnit, bool bGameStart, UnitAITypes eUnitAI = NO_UNITAI);

	CvCity* initCity(int iX, int iY, bool bBumpUnits = true, bool bInitialFounding = true, ReligionTypes eInitialReligion = NO_RELIGION, const char* szName = NULL, CvUnitEntry* pkSettlerUnitEntry = NULL);

	CvCity* acquireCity(CvCity* pCity, bool bConquest, bool bGift);
	bool IsValidBuildingForPlayer(CvCity* pCity, BuildingTypes eBuilding, bool bConquest);
	void killCities();
	CvString getNewCityName() const;
	CvString GetBorrowedCityName(CivilizationTypes eCivToBorrowFrom) const;
	void getCivilizationCityName(CvString& szBuffer, CivilizationTypes eCivilization) const;
	bool isCityNameValid(CvString& szName, bool bTestDestroyed = true, bool bForce = false) const;

	int getBuyPlotDistance() const;
	int getWorkPlotDistance() const;
	int GetNumWorkablePlots() const;

	void DoRevolutionPlayer(PlayerTypes ePlayer, int iOldCityID);

	void UpdateCityThreatCriteria();
	//0 == highest, 1 = second highest, etc. Not all cities will be assigned!
	vector<CvCity*> GetThreatenedCities(bool bCoastalOnly);

	void UpdateBestMilitaryCities();
	void SetBestMilitaryCityDomain(int iValue, DomainTypes eDomain);
	void SetBestMilitaryCityCombatClass(int iValue, UnitCombatTypes eUnitCombat);
	CvCity* GetBestMilitaryCity(UnitCombatTypes eUnitCombat = NO_UNITCOMBAT, DomainTypes eDomain = NO_DOMAIN);

	// Declared public because CvPlayerCorporations needs to access this. Maybe want to use a friend
	void processCorporations(CorporationTypes eCorporation, int iChange);

#if defined(MOD_BALANCE_CORE_EVENTS)
	void DoEvents();
	void DoCancelEventChoice(EventChoiceTypes eEventChoice);
	void CheckActivePlayerEvents(CvCity* pCity);
	bool IsEventValid(EventTypes eEvent);
	bool IsEventChoiceValid(EventChoiceTypes eEventChoice, EventTypes eParentEvent);
	void DoStartEvent(EventTypes eEvent, bool bSendMsg);
	void DoEventChoice(EventChoiceTypes eEventChoice, EventTypes eEvent = NO_EVENT, bool bSendMsg = true, bool bEspionage = false);
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
	void DoLiberatePlayer(PlayerTypes ePlayer, int iOldCityID, bool bForced, bool bSphereRemoval);
	bool CanLiberatePlayer(PlayerTypes ePlayer);
	bool CanLiberatePlayerCity(PlayerTypes ePlayer);

	CvUnit* initUnit(UnitTypes eUnit, int iX, int iY, UnitAITypes eUnitAI = NO_UNITAI, UnitCreationReason eReason = REASON_DEFAULT, bool bNoMove = false, bool bSetupGraphical = true, int iMapLayer = 0, int iNumGoodyHutsPopped = 0, ContractTypes eContract = NO_CONTRACT, bool bHistoric = true, CvUnit* pPassUnit = NULL);
	CvUnit* initUnitWithNameOffset(UnitTypes eUnit, int nameOffset, int iX, int iY, UnitAITypes eUnitAI = NO_UNITAI, UnitCreationReason eReason = REASON_DEFAULT, bool bNoMove = false, bool bSetupGraphical = true, int iMapLayer = 0, int iNumGoodyHutsPopped = 0, ContractTypes eContract = NO_CONTRACT, bool bHistoric = true, CvUnit* pPassUnit = NULL);
	CvUnit* initNamedUnit(UnitTypes eUnit, const char* strKey, int iX, int iY, UnitAITypes eUnitAI = NO_UNITAI, UnitCreationReason eReason = REASON_DEFAULT, bool bNoMove = false, bool bSetupGraphical = true, int iMapLayer = 0, int iNumGoodyHutsPopped = 0);

	void disbandUnit(bool bAnnounce);
	void killUnits();

	UnitTypes GetSpecificUnitType(const char* szUnitClass, bool hideAssert = false);
	UnitTypes GetSpecificUnitType(UnitClassTypes eUnitCass) const;
	BuildingTypes GetSpecificBuildingType(const char* szBuildingClass, bool hideAssert = false);

	CvPlot *GetBestCoastalSpawnPlot (CvUnit *pUnit);

	int GetNumBuilders() const;
	void SetNumBuilders(int iNum);
	void ChangeNumBuilders(int iChange);
	int GetMaxNumBuilders() const;
	void SetMaxNumBuilders(int iNum);
	void ChangeMaxNumBuilders(int iChange);

	bool HasActiveSettler();

	int GetNumUnitsWithUnitAI(UnitAITypes eUnitAIType, bool bIncludeBeingTrained);
	int GetNumUnitsWithDomain(DomainTypes eDomain, bool bMilitaryOnly);
	int GetNumUnitsWithUnitCombat(UnitCombatTypes eDomain);
	int GetNumUnitsOfType(UnitTypes eUnit, bool bIncludeBeingTrained = false);
	int GetNumUnitPromotions(PromotionTypes ePromotion);
	void UpdateDangerPlots(bool bKeepKnownUnits);
	void SetDangerPlotsDirty();

	bool isHuman() const;
	bool isObserver() const;
	bool isBarbarian() const;

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
	void EndTurnsForReadyUnits(bool bLinkedUnitsOnly = false);
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

	//building step chains
	void BuildBuildingStepValues();
	std::vector<BuildingTypes> FindInitialBuildings();
	void SetChainLength(BuildingTypes eBuilding);
	int GetChainLength(BuildingTypes eBuilding);

	void AwardFreeBuildings(CvCity* pCity); // slewis - broken out so that Venice can get free buildings when they purchase something

	bool canFoundCityExt(int iX, int iY, bool bIgnoreDistanceToExistingCities, bool bIgnoreHappiness) const;
	bool canFoundCity(int iX, int iY) const;

#if defined(MOD_GLOBAL_RELIGIOUS_SETTLERS) && defined(MOD_BALANCE_CORE)
	void foundCity(int iX, int iY, ReligionTypes eReligion = NO_RELIGION, bool bForce = false, CvUnitEntry* pkSettlerUnitEntry = NULL);
#elif defined(MOD_GLOBAL_RELIGIOUS_SETTLERS)
	void foundCity(int iX, int iY, ReligionTypes eReligion = NO_RELIGION, bool bForce = false);
#elif defined(MOD_BALANCE_CORE)
	void foundCity(int iX, int iY, CvUnitEntry* pkSettlerUnitEntry = NULL);
#else
	void foundCity(int iX, int iY);
#endif
	void cityBoost(int iX, int iY, CvUnitEntry* pkUnitEntry, int iExtraPlots, int iPopChange, int iFoodPercent);

	bool canTrainUnit(UnitTypes eUnit, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, bool bIgnoreUniqueUnitStatus = false, CvString* toolTipSink = NULL) const;
	bool canConstruct(BuildingTypes eBuilding, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, CvString* toolTipSink = NULL) const;
	bool canConstruct(BuildingTypes eBuilding, const std::vector<int>& vPreExistingBuildings, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, CvString* toolTipSink = NULL) const;
	bool canCreate(ProjectTypes eProject, bool bContinue = false, bool bTestVisible = false) const;
	bool canPrepare(SpecialistTypes eSpecialist, bool bContinue = false) const;
	bool canMaintain(ProcessTypes eProcess, bool bContinue = false) const;
	bool IsCanPurchaseAnyCity(bool bTestPurchaseCost, bool bTestTrainable, UnitTypes eUnit, BuildingTypes eBuilding, YieldTypes ePurchaseYield);
	bool isProductionMaxedUnitClass(UnitClassTypes eUnitClass) const;
	bool isProductionMaxedBuildingClass(BuildingClassTypes eBuildingClass, bool bAcquireCity = false) const;
	bool isProductionMaxedProject(ProjectTypes eProject) const;
	int getProductionNeeded(UnitTypes eUnit, bool bIgnoreDifficulty) const;
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
	int GetBuildingClassYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYieldType, const vector<int>& preexistingBuildingsCount);
	int GetBuildingClassYieldModifier(BuildingClassTypes eBuildingClass, YieldTypes eYieldType, const vector<int>& preexistingBuildingsCount);

	int GetWorldWonderYieldChange(int iYield);

	bool canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestEra = false, bool bTestVisible = false, bool bTestGold = true, bool bTestPlotOwner = true, const CvUnit* pUnit = NULL) const;
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

	int GetNumUnitsSupplied(bool bCheckWarWeariness = true) const;
	int GetNumUnitsSuppliedByHandicap(bool bIgnoreReduction = false) const;
	int GetNumUnitsSuppliedByCities(bool bIgnoreReduction = false) const;
	int GetNumUnitsSuppliedByPopulation(bool bIgnoreReduction = false) const;

	int GetNumUnitsOutOfSupply(bool bCheckWarWeariness = true) const;
#if defined(MOD_BALANCE_CORE)
	int GetNumUnitsToSupply() const;
	int getNumUnitsSupplyFree() const;
	void changeNumUnitsSupplyFree(int iValue);
#endif

	int calculateUnitCost() const;
	int calculateResearchModifier(TechTypes eTech);
	int calculateGoldRate() const;
	int calculateGoldRateTimes100() const;
	int getAvgGoldRate() const;
	void cacheAvgGoldRate();
	int getTurnsToBankruptcy(int iAssumedExtraExpense) const;

	int GetAverageProduction() const;
	int GetAverageProductionTimes100() const;
	int GetAverageInstantProduction();
	int GetAverageInstantProductionTimes100();

	int unitsRequiredForGoldenAge() const;
	int unitsGoldenAgeCapable() const;
	int unitsGoldenAgeReady() const;

	int greatGeneralThreshold() const;
	int greatAdmiralThreshold() const;

	int specialistYield(SpecialistTypes eSpecialist, YieldTypes eYield) const;

	int GetCityYieldChangeTimes100(YieldTypes eYield) const;
	void ChangeCityYieldChangeTimes100(YieldTypes eYield, int iChange);

	int GetCoastalCityYieldChange(YieldTypes eYield) const;
	void ChangeCoastalCityYieldChange(YieldTypes eYield, int iChange);

	int GetCapitalYieldChangeTimes100(YieldTypes eYield) const;
	void ChangeCapitalYieldChangeTimes100(YieldTypes eYield, int iChange);

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

	//name is misleading, should be HappinessFromCityConnections
	int GetHappinessFromTradeRoutes() const;
	void DoUpdateCityConnectionHappiness();
	bool UpdateCityConnection(const CvPlot* pPlot, bool bActive);
	bool IsCityConnectionPlot(const CvPlot* pPlot) const;

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

	int GetCulturePerTurnFromMinorCivs() const;
	int GetCulturePerTurnFromMinor(PlayerTypes eMinor) const;

	int GetCulturePerTurnFromReligion() const;

	int GetCulturePerTurnFromBonusTurns() const;

	int GetJONSCultureCityModifier() const;
	void ChangeJONSCultureCityModifier(int iChange);
	int GetLeagueCultureCityModifier() const;
	void ChangeLeagueCultureCityModifier(int iChange);

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

	void DoYieldsFromKill(CvUnit* pAttackingUnit, CvUnit* pDefendingUnit, CvCity* pCity = NULL);

	void DoTechFromCityConquer(CvCity* pConqueredCity);

	void DoHealGlobal(int iValue);
#if defined(MOD_BALANCE_CORE)
	void DoFreeGreatWorkOnConquest(PlayerTypes ePlayer, CvCity* pCity);
	void DoWarVictoryBonuses();
	void DoDifficultyBonus(HistoricEventTypes eHistoricEvent);
#endif

	int GetYieldPerTurnFromReligion(YieldTypes eYield) const;
	int GetYieldPerTurnFromTraits(YieldTypes eYield) const;

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
	int GetEmpireHappinessFromCities() const;
	int GetHappiness() const;
	void SetHappiness(int iNewValue);
	void SetUnhappiness(int iNewValue);
	int GetUnhappiness() const;
	void CalculateNetHappiness();
	int GetHappinessRatioRawPercent();
	void DistributeHappinessToCities();

	int GetEmpireSizeModifierReductionGlobal() const;
	void ChangeEmpireSizeModifierReductionGlobal(int iChange);

	int GetDistressFlatReductionGlobal() const;
	void ChangeDistressFlatReductionGlobal(int iChange);

	int GetPovertyFlatReductionGlobal() const;
	void ChangePovertyFlatReductionGlobal(int iChange);

	int GetIlliteracyFlatReductionGlobal() const;
	void ChangeIlliteracyFlatReductionGlobal(int iChange);

	int GetBoredomFlatReductionGlobal() const;
	void ChangeBoredomFlatReductionGlobal(int iChange);

	int GetReligiousUnrestFlatReductionGlobal() const;
	void ChangeReligiousUnrestFlatReductionGlobal(int iChange);

	int GetBasicNeedsMedianModifierGlobal() const;
	void ChangeBasicNeedsMedianModifierGlobal(int iChange);

	int GetGoldMedianModifierGlobal() const;
	void ChangeGoldMedianModifierGlobal(int iChange);

	int GetScienceMedianModifierGlobal() const;
	void ChangeScienceMedianModifierGlobal(int iChange);

	int GetCultureMedianModifierGlobal() const;
	void ChangeCultureMedianModifierGlobal(int iChange);

	int GetReligiousUnrestModifierGlobal() const;
	void ChangeReligiousUnrestModifierGlobal(int iChange);

	int GetBasicNeedsMedianModifierCapital() const;
	void ChangeBasicNeedsMedianModifierCapital(int iChange);

	int GetGoldMedianModifierCapital() const;
	void ChangeGoldMedianModifierCapital(int iChange);

	int GetScienceMedianModifierCapital() const;
	void ChangeScienceMedianModifierCapital(int iChange);

	int GetCultureMedianModifierCapital() const;
	void ChangeCultureMedianModifierCapital(int iChange);

	int GetReligiousUnrestModifierCapital() const;
	void ChangeReligiousUnrestModifierCapital(int iChange);

	void ChangeLandmarksTourismPercentGlobal(int iChange);
	int GetLandmarksTourismPercentGlobal() const;

	void ChangeGreatWorksTourismModifierGlobal(int iChange);
	int GetGreatWorksTourismModifierGlobal() const;

	void DoTestEmpireInBadShapeForWar();
	bool IsNoNewWars() const;
	void SetNoNewWars(bool bValue);
	bool IsInTerribleShapeForWar() const;
	void SetInTerribleShapeForWar(bool bValue);

	int GetTurnsSinceLastAttackedMinorCiv() const;
	void SetTurnLastAttackedMinorCiv(int iTurn);

	int GetHappinessForGAP() const;
	int GetExcessHappiness() const;
	int GetUnhappinessGrowthPenalty() const;
	int GetUnhappinessSettlerCostPenalty() const;
	int GetUnhappinessCombatStrengthPenalty() const;
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
	void SetNaturalWonderOwned(FeatureTypes eFeature, bool bValue);

#if defined(MOD_BALANCE_CORE)
	void ChangeUnitClassProductionModifier(UnitClassTypes eUnitClass, int iValue);
	int GetUnitClassProductionModifier(UnitClassTypes eUnitClass) const;
#endif

	int GetHappinessFromLuxury(ResourceTypes eResource, bool bIncludeImport = true) const;
	int GetExtraHappinessPerLuxury() const;
	void ChangeExtraHappinessPerLuxury(int iChange);

	int GetBonusHappinessFromLuxuriesFlat() const;
	int GetBonusHappinessFromLuxuriesFlatForUI() const;

	int GetUnhappinessFromWarWeariness() const;

	int GetUnhappinessFromCityForUI(CvCity* pCity) const;
	int GetUnhappinessFromCityCount(CvCity* pAssumeCityAnnexed = NULL, CvCity* pAssumeCityPuppeted = NULL) const;
	int GetUnhappinessFromCapturedCityCount(CvCity* pAssumeCityAnnexed = NULL, CvCity* pAssumeCityPuppeted = NULL) const;
	int GetUnhappinessFromCityPopulation(CvCity* pAssumeCityAnnexed = NULL, CvCity* pAssumeCityPuppeted = NULL) const;
	int GetUnhappinessFromCitySpecialists(CvCity* pAssumeCityAnnexed, CvCity* pAssumeCityPuppeted) const;
	int GetUnhappinessFromPuppetCitySpecialists() const;
	int GetUnhappinessFromPuppetCityPopulation() const;
	int GetUnhappinessFromOccupiedCities(CvCity* pAssumeCityAnnexed = NULL, CvCity* pAssumeCityPuppeted = NULL) const;

	int GetUnhappinessFromUnits() const;
	void ChangeUnhappinessFromUnits(int iChange);

	int GetUnhappinessFromUnitsMod() const;
	void ChangeUnhappinessFromUnitsMod(int iChange);

	int GetUnhappinessMod() const;
	void ChangeUnhappinessMod(int iChange);
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	int GetHappinessFromCitizenNeeds() const;
	int GetUnhappinessFromCitizenNeeds() const;
	int GetUnhappinessFromBoredom() const;
	int GetUnhappinessFromIlliteracy() const;
	int GetUnhappinessFromDistress() const;
	int GetUnhappinessFromPoverty() const;
	int GetUnhappinessFromIsolation() const;
	int GetUnhappinessFromPillagedTiles() const;
	int GetUnhappinessFromFamine() const;
	int GetUnhappinessFromReligiousUnrest() const;
	int GetUnhappinessFromCityJFDSpecial() const;
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

	void ChangeIsVassalsNoRebel(int iValue);
	bool IsVassalsNoRebel() const;

	void ChangeVassalYieldBonusModifier(int iValue);
	int GetVassalYieldBonusModifier() const;

	void ChangeCSYieldBonusModifier(int iValue);
	int GetCSYieldBonusModifier() const;

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
	int GetEspionageTurnsModifierFriendly() const;
	void ChangeEspionageTurnsModifierFriendly(int iChange);
	int GetEspionageTurnsModifierEnemy() const;
	void ChangeEspionageTurnsModifierEnemy(int iChange);
	int GetStartingSpyRank() const;
	void ChangeStartingSpyRank(int iChange);
	// END Espionage

#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
	int GetConversionModifier() const;
	void ChangeConversionModifier(int iChange);
#endif

	// Extra Yields from Annexed Minors
	int GetFoodInCapitalPerTurnFromAnnexedMinors() const;
	void UpdateFoodInCapitalPerTurnFromAnnexedMinors();
	int GetFoodInOtherCitiesPerTurnFromAnnexedMinors() const;
	void UpdateFoodInOtherCitiesPerTurnFromAnnexedMinors();
	int GetGoldPerTurnFromAnnexedMinors() const;
	void UpdateGoldPerTurnFromAnnexedMinors();
	int GetCulturePerTurnFromAnnexedMinors() const;
	void UpdateCulturePerTurnFromAnnexedMinors();
	int GetSciencePerTurnFromAnnexedMinors() const;
	void UpdateSciencePerTurnFromAnnexedMinors();
	int GetFaithPerTurnFromAnnexedMinors() const;
	void UpdateFaithPerTurnFromAnnexedMinors();
	int GetHappinessFromAnnexedMinors() const;
	void UpdateHappinessFromAnnexedMinors();

	int GetExtraLeagueVotes() const;
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

	int GetReligionVotes() const;
	int CalculateReligionExtraVotes(const CvReligion *pReligion) const;
	int CalculateReligionVotesFromImprovements(const CvReligion *pReligion) const;

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

	void ChangeExtraLeagueVotes(int iChange);

	int GetWoundedUnitDamageMod() const;
	void SetWoundedUnitDamageMod(int iValue);
	void ChangeWoundedUnitDamageMod(int iChange);

	int GetUnitUpgradeCostMod() const;
	void SetUnitUpgradeCostMod(int iValue);
	void ChangeUnitUpgradeCostMod(int iChange);

	int GetBarbarianCombatBonus(bool bIgnoreHandicap) const;
	void SetBarbarianCombatBonus(int iValue);
	void ChangeBarbarianCombatBonus(int iChange);

	bool IsAlwaysSeeBarbCamps() const;
	void SetAlwaysSeeBarbCampsCount(int iValue);
	void ChangeAlwaysSeeBarbCampsCount(int iChange);

	bool grantPolicy(PolicyTypes iPolicy, bool bFree=false);
	bool revokePolicy(PolicyTypes iPolicy);
	bool swapPolicy(PolicyTypes iNewPolicy, PolicyTypes iOldPolicy);
	void setHasPolicy(PolicyTypes eIndex, bool bNewValue, bool bFree=false);
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

	int getTourismBonusTurnsPlayer(PlayerTypes eWithPlayer) const;
	void changeTourismBonusTurnsPlayer(PlayerTypes eWithPlayer, int iChange);

	// Golden Age Stuff

	void DoProcessVotes();
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
	void changeGoldenAgeTurns(int iChange, bool bFree = false);
	int getGoldenAgeLength(int iManualLength = -1) const;

	int getNumUnitGoldenAges() const;
	void changeNumUnitGoldenAges(int iChange);

	int getStrikeTurns() const;
	void changeStrikeTurns(int iChange);

	int getGoldenAgeModifier(bool bCheckMonopolies = true) const;
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
	int getGreatDiplomatsCreated(bool bExcludeFree) const;
	void incrementGreatDiplomatsCreated(bool bIsFree);
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
	int getGreatDiplomatsCreated() const;
	void incrementGreatDiplomatsCreated();
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
	int getDiplomatsFromFaith() const;
	void incrementDiplomatsFromFaith();
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

	byte getResourceFromGP(ResourceTypes eResource) const;
	void changeResourceFromGP(ResourceTypes eResource, byte iChange);

	int getResourceModFromReligion(ResourceTypes eIndex) const;
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
	int getGreatDiplomatRateModifier() const;
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
	void DoUnitKilledCombat(CvUnit* pKillingUnit, PlayerTypes eKilledPlayer, UnitTypes eUnitType);
#if defined(MOD_BALANCE_CORE)
	void doInstantYield(InstantYieldType iType, bool bCityFaith = false, GreatPersonTypes eGreatPerson = NO_GREATPERSON, BuildingTypes eBuilding = NO_BUILDING, int iPassYield = 0, bool bEraScale = true, PlayerTypes ePlayer = NO_PLAYER, CvPlot* pPlot = NULL, bool bSuppress = false, CvCity* pCity = NULL, bool bSeaTrade = false, bool bInternational = true, bool bEvent = false, YieldTypes eYield = NO_YIELD, CvUnit* pUnit = NULL, TerrainTypes ePassTerrain = NO_TERRAIN, CvMinorCivQuest* pQuestData = NULL, CvCity* pOtherCity = NULL, CvUnit* pAttackingUnit = NULL);
	void addInstantYieldText(InstantYieldType iType, const CvString& strInstantYield);
	void setInstantYieldText(InstantYieldType iType, const CvString& strInstantYield);
	CvString getInstantYieldText(InstantYieldType iType)  const;
	void doInstantGWAM(GreatPersonTypes eGreatPerson, const CvString& strUnitName, bool bConquest = false);
	void doPolicyGEorGM(int iPolicyGEorGM);
	void doInstantGreatPersonProgress(InstantYieldType iType, bool bSuppress = false, CvCity* pCity = NULL, BuildingTypes eBuilding = NO_BUILDING, int iPassValue = 0, GreatPersonTypes ePassGreatPerson = NO_GREATPERSON);
	void addInstantGreatPersonProgressText(InstantYieldType iType, const CvString& strInstantYield);
	void setInstantGreatPersonProgressText(InstantYieldType iType, const CvString& strInstantYield);
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
	int GetHappinessFromMilitaryUnits() const;

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

	void DoUnitDiversity();
	bool IsUnderrepresentedUnitType(UnitAITypes eType) const;

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

	int GetNoUnhappfromXSpecialists() const;
	void ChangeNoUnhappfromXSpecialists(int iChange);

	int GetTechNeedModifier() const;

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

	int getCultureBombTimer() const;
	void setCultureBombTimer(int iNewValue);
	void changeCultureBombTimer(int iChange);

	int getConversionTimer() const;
	void setConversionTimer(int iNewValue);
	void changeConversionTimer(int iChange);

	CvCity* getCapitalCity() const;
	int getCapitalCityID() const;
	void setCapitalCity(CvCity* pNewCapitalCity);

	int GetOriginalCapitalX() const;
	int GetOriginalCapitalY() const;
	void setOriginalCapitalXY(CvCity* pCapitalCity);
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
	void ResetMightCalcTurn();

	int getCombatExperienceTimes100() const;
#if defined(MOD_GLOBAL_LOCAL_GENERALS)
	void setCombatExperienceTimes100(int iExperienceTimes100, CvUnit* pFromUnit = NULL);
	void changeCombatExperienceTimes100(int iChangeTimes100, CvUnit* pFromUnit = NULL);
#else
	void setCombatExperienceTimes100(int iExperienceTimes100);
	void changeCombatExperienceTimes100(int iChangeTimes100);
#endif
	int getLifetimeCombatExperienceTimes100() const;
	int getNavalCombatExperienceTimes100() const;
#if defined(MOD_GLOBAL_LOCAL_GENERALS)
	void setNavalCombatExperienceTimes100(int iExperienceTimes100, CvUnit* pFromUnit = NULL);
	void changeNavalCombatExperienceTimes100(int iChangeTimes100, CvUnit* pFromUnit = NULL);
#else
	void setNavalCombatExperienceTimes100(int iExperienceTimes100);
	void changeNavalCombatExperienceTimes100(int iChangeTimes100);
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
	bool isMajorCiv() const;

	bool IsVassalOfSomeone() const;
	int GetNumVassals() const;

	int GetNumValidMajorsMet(bool bJustMetBuffer) const;
	bool HasMetValidMinorCiv() const;
	bool IsHasBetrayedMinorCiv() const;
	void SetHasBetrayedMinorCiv(bool bValue);

	void setAlive(bool bNewValue, bool bNotify = true);
	void verifyAlive(PlayerTypes eKiller = NO_PLAYER);
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
	void setLeaderType(LeaderHeadTypes eNewLeader);

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

	int GetYieldFromWorldWonderConstruction(YieldTypes eIndex) const;
	void ChangeYieldFromWorldWonderConstruction(YieldTypes eIndex, int iChange);

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

	std::vector<SPlayerActiveEspionageEvent> CvPlayer::GetActiveEspionageEventsList() const;

	int GetNumAnnexedCityStates(MinorCivTraitTypes eIndex)	const;
	void ChangeNumAnnexedCityStates(MinorCivTraitTypes eIndex, int iChange);

	int getYieldFromNonSpecialistCitizens(YieldTypes eIndex)	const;
	void changeYieldFromNonSpecialistCitizens(YieldTypes eIndex, int iChange);

	int getYieldModifierFromGreatWorks(YieldTypes eIndex)	const;
	void changeYieldModifierFromGreatWorks(YieldTypes eIndex, int iChange);

	int getYieldModifierFromActiveSpies(YieldTypes eIndex)	const;
	void changeYieldModifierFromActiveSpies(YieldTypes eIndex, int iChange);

	int getYieldFromDelegateCount(YieldTypes eIndex)	const;
	void changeYieldFromDelegateCount(YieldTypes eIndex, int iChange);

	int getYieldForLiberation(YieldTypes eIndex)	const;
	void changeYieldForLiberation(YieldTypes eIndex, int iChange);

	int getInfluenceForLiberation()	const;
	void changeInfluenceForLiberation(int iChange);

	int getExperienceForLiberation()	const;
	void changeExperienceForLiberation(int iChange);

	int getCityCaptureHealGlobal() const;
	void changeCityCaptureHealGlobal(int iChange);

	int getNumBuildingClassInLiberatedCities(BuildingClassTypes eIndex)	const;
	void changeNumBuildingClassInLiberatedCities(BuildingClassTypes eIndex, int iChange);

	int getUnitsInLiberatedCities()	const;
	void changeUnitsInLiberatedCities(int iChange);

	int GetGarrisonsOccupiedUnhappinessMod() const;
	void changeGarrisonsOccupiedUnhappinessMod(int iChange);

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

	bool IsDoubleBorderGrowthGA() const;
	int GetDoubleBorderGrowthGA() const;
	void ChangeDoubleBorderGrowthGA(int iChange);

	bool IsDoubleBorderGrowthWLTKD() const;
	int GetDoubleBorderGrowthWLTKD() const;
	void ChangeDoubleBorderGrowthWLTKD(int iChange);

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

	void ChangeMinimumAllyInfluenceIncreaseAtWar(int iValue);

	int GetMinimumAllyInfluenceIncreaseAtWar() const;

	void ChangeCanBullyFriendlyCS(int iValue);
	bool IsCanBullyFriendlyCS() const;

	void ChangeKeepConqueredBuildings(int iValue);
	bool IsKeepConqueredBuildings() const;

	void ChangeBullyGlobalCSReduction(int iValue);
	int GetBullyGlobalCSReduction() const;
#endif
#if defined(MOD_BALANCE_CORE_SPIES)
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

#if defined(MOD_POLICIES_UNIT_CLASS_REPLACEMENTS)
	UnitClassTypes GetUnitClassReplacement(UnitClassTypes eUnitClass) const;
	void SetUnitClassReplacement(UnitClassTypes eReplacedUnitClass, UnitClassTypes eReplacementUnitClass);
#endif

	bool IsCSResourcesCountMonopolies() const;
	void changeCSResourcesCountMonopolies(int iChange);

	int GetScalingNationalPopulationRequired(BuildingTypes eBuilding) const;
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

	bool IsGetsScienceFromPlayer(PlayerTypes ePlayer) const;
	void SetGetsScienceFromPlayer(PlayerTypes ePlayer, bool bValue);

	// END Science

	void DoBankruptcy();

	int getSpecialistExtraYield(YieldTypes eIndex) const;
	void changeSpecialistExtraYield(YieldTypes eIndex, int iChange);

	int GetPlayerNumTurnsAtPeace(PlayerTypes ePlayer) const;
	void SetPlayerNumTurnsAtPeace(PlayerTypes ePlayer, int iValue);
	void ChangePlayerNumTurnsAtPeace(PlayerTypes ePlayer, int iChange);

	int GetPlayerNumTurnsAtWar(PlayerTypes ePlayer) const;
	void SetPlayerNumTurnsAtWar(PlayerTypes ePlayer, int iValue);
	void ChangePlayerNumTurnsAtWar(PlayerTypes ePlayer, int iChange);
	int GetTeamNumTurnsAtWar(TeamTypes eTeam) const;

	int GetPlayerNumTurnsSinceCityCapture(PlayerTypes ePlayer) const;
	void SetPlayerNumTurnsSinceCityCapture(PlayerTypes ePlayer, int iValue);
	void ChangePlayerNumTurnsSinceCityCapture(PlayerTypes ePlayer, int iChange);

	int GetWarValueLost(PlayerTypes ePlayer) const;
	void SetWarValueLost(PlayerTypes ePlayer, int iValue);
	void ChangeWarValueLost(PlayerTypes ePlayer, int iChange);
	void DoWarValueLostDecay();
	void DoUpdateWarDamage();

	int GetWarDamageValue(PlayerTypes ePlayer) const;
	void SetWarDamageValue(PlayerTypes ePlayer, int iValue);

	void changeUnitsBuiltCount(UnitTypes eUnitType, int iValue);
	int getUnitsBuiltCount(UnitTypes eUnitType) const;

	void DoUpdateWarPeaceTurnCounters();
	void ResetWarPeaceTurnCounters();

	PlayerProximityTypes GetProximityToPlayer(PlayerTypes ePlayer) const;
	void SetProximityToPlayer(PlayerTypes ePlayer, PlayerProximityTypes eProximity);

	pair<int,int> GetClosestCityPair(PlayerTypes ePlayer);
	void DoUpdateProximityToPlayers();

	void UpdateResearchAgreements(int iValue);
	int GetResearchAgreementCounter(PlayerTypes ePlayer) const;
	void SetResearchAgreementCounter(PlayerTypes ePlayer, int iValue);
	void ChangeResearchAgreementCounter(PlayerTypes ePlayer, int iChange);

	void DoCivilianReturnLogic(bool bReturn, PlayerTypes eToPlayer, int iUnitID);

#if defined(MOD_BALANCE_CORE)
	void DoTradeInfluenceAP();
#endif
	void DoDistanceGift(PlayerTypes eFromPlayer, CvUnit* pUnit);
	void AddIncomingUnit(PlayerTypes eFromPlayer, CvUnit* pUnit);
	PlayerTypes GetBestGiftTarget(DomainTypes eUnitDomain);

	bool isOption(PlayerOptionTypes eIndex) const;
	void setOption(PlayerOptionTypes eIndex, bool bNewValue);

	bool isPlayable() const;
	void setPlayable(bool bNewValue);

	int getNumResourceUsed(ResourceTypes eIndex) const;
	void changeNumResourceUsed(ResourceTypes eIndex, int iChange);
	int getNumResourceTotal(ResourceTypes eIndex, bool bIncludeImport = true) const;
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
	bool WouldGainMonopoly(ResourceTypes eResource, int iExtraResource) const;

	//cache these because we need them a lot
	int GetCombatAttackBonusFromMonopolies() const;
	int GetCombatDefenseBonusFromMonopolies() const;
	void UpdateMonopolyCache();
	void UpdatePlotBlockades();

	int getCityYieldModFromMonopoly(YieldTypes eYield) const;
	void changeCityYieldModFromMonopoly(YieldTypes eYield, int iValue);

	int getResourceShortageValue(ResourceTypes eIndex) const;
	void changeResourceShortageValue(ResourceTypes eIndex, int iChange);
	void setResourceShortageValue(ResourceTypes eIndex, int iChange);

	int getResourceFromCSAlliances(ResourceTypes eIndex) const;
	void changeResourceFromCSAlliances(ResourceTypes eIndex, int iChange);
	void setResourceFromCSAlliances(ResourceTypes eIndex, int iChange);

#endif

	bool IsResourceCityTradeable(ResourceTypes eResource, bool bCheckTeam = true) const;
	bool IsResourceRevealed(ResourceTypes eResource, bool bCheckTeam = true) const;
	CvImprovementEntry* GetResourceImprovement(ResourceTypes eResource, bool bCivSpecific = false) const;

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

	int getResourceImportFromMajor(ResourceTypes eIndex) const;
	void changeResourceImportFromMajor(ResourceTypes eIndex, int iChange);

	int getResourceFromMinors(ResourceTypes eIndex) const;
	void changeResourceFromMinors(ResourceTypes eIndex, int iChange);

	int getResourceSiphoned(ResourceTypes eIndex) const;
	void changeResourceSiphoned(ResourceTypes eIndex, int iChange);

	int getResourceInOwnedPlots(ResourceTypes eIndex);

	int getTotalImprovementsBuilt() const;
	void changeTotalImprovementsBuilt(int iChange);
	int getImprovementCount(ImprovementTypes eIndex, bool bBuiltOnly = false) const;
	void changeImprovementCount(ImprovementTypes eIndex, int iChange, bool bBuilt = false);

#if defined(MOD_BALANCE_CORE)
	int getTotalImprovementsBuilt(ImprovementTypes eIndex) const;
	void changeTotalImprovementsBuilt(ImprovementTypes eIndex, int iChange);
#endif

#if defined(MOD_IMPROVEMENTS_EXTENSIONS)
	int getResponsibleForRouteCount(RouteTypes eIndex) const;
	void changeResponsibleForRouteCount(RouteTypes eIndex, int iChange);

	int getResponsibleForImprovementCount(ImprovementTypes eIndex) const;
	void changeResponsibleForImprovementCount(ImprovementTypes eIndex, int iChange);
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

	int getYieldFromYieldGlobal(YieldTypes eIndex1, YieldTypes eIndex2) const;
	void changeYieldFromYieldGlobal(YieldTypes eIndex1, YieldTypes eIndex2, int iChange);

	int getPlotYieldChange(PlotTypes eIndex1, YieldTypes eIndex2) const;
	void changePlotYieldChange(PlotTypes eIndex1, YieldTypes eIndex2, int iChange);

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

#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	int getSpecificGreatPersonRateModifierFromMonopoly(GreatPersonTypes eIndex1, MonopolyTypes eIndex2) const;
	int getSpecificGreatPersonRateModifierFromMonopoly(GreatPersonTypes eIndex1) const;
	void changeSpecificGreatPersonRateModifierFromMonopoly(GreatPersonTypes eIndex1, MonopolyTypes eIndex2, int iChange);

	int getSpecificGreatPersonRateChangeFromMonopoly(GreatPersonTypes eIndex1, MonopolyTypes eIndex2) const;
	int getSpecificGreatPersonRateChangeFromMonopoly(GreatPersonTypes eIndex1) const;
	void changeSpecificGreatPersonRateChangeFromMonopoly(GreatPersonTypes eIndex1, MonopolyTypes eIndex2, int iChange);
#endif

	int getImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const;
	void changeImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange);

	CvUnitCycler& GetUnitCycler() { return m_UnitCycle; };

	bool removeFromArmy(int iArmyID, int iID);

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

	CvUnit* firstUnitInSquad(int* pIterIdx, int iSquadNum);
	CvUnit* nextUnitInSquad(int* pIterIdx, int iSquadNum);
#if defined(MOD_BALANCE_CORE)
	CvUnit* nextUnit(const CvUnit* pCurrent, bool bRev);
	const CvUnit* nextUnit(const CvUnit* pCurrent, bool bRev) const;
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
	size_t getNumAIOperations() const;
	CvAIOperation* getAIOperationByIndex(size_t iIndex) const;
	CvAIOperation* getAIOperation(int iID);
	const CvAIOperation* getAIOperation(int iID) const;
	CvAIOperation* addAIOperation(AIOperationTypes eOperationType, size_t iMaxMissingUnits, PlayerTypes eEnemy = NO_PLAYER, CvCity* pTarget = NULL, CvCity* pMuster = NULL);
	void deleteAIOperation(int iID);
	CvAIOperation* getFirstAIOperationOfType(AIOperationTypes eOperationType, PlayerTypes eTargetPlayer = NO_PLAYER, CvPlot* pTargetPlot=NULL);
	CvAIOperation* getFirstOffensiveAIOperation(PlayerTypes eTargetPlayer);

	bool IsTargetCityForOperation(CvCity* pCity, bool bNaval) const;
	bool IsMusterCityForOperation(CvCity* pCity, bool bNavalOp) const;

	int GetNumOffensiveOperations(DomainTypes eDomain);
	bool HasAnyOffensiveOperationsAgainstPlayer(PlayerTypes ePlayer);

	bool StopAllLandDefensiveOperationsAgainstPlayer(PlayerTypes ePlayer, AIOperationAbortReason eReason);
	bool StopAllLandOffensiveOperationsAgainstPlayer(PlayerTypes ePlayer, AIOperationAbortReason eReason);

	bool StopAllSeaDefensiveOperationsAgainstPlayer(PlayerTypes ePlayer, AIOperationAbortReason eReason);
	bool StopAllSeaOffensiveOperationsAgainstPlayer(PlayerTypes ePlayer, AIOperationAbortReason eReason);

	vector<int> GetPlotsTargetedByExplorers(const CvUnit* pIgnoreUnit=NULL) const;
	bool IsPlotTargetedForCity(CvPlot *pPlot, CvAIOperation* pOpToIgnore) const;

	void GatherPerTurnReplayStats(int iGameTurn);
	const std::map<CvString,TurnData>& getReplayData() const;
	int getReplayDataValue(const CvString& strDataset, unsigned int uiTurn) const;
	void setReplayDataValue(const CvString& strDataset, unsigned int uiTurn, int iValue);

	int getYieldPerTurnHistory(YieldTypes eYield, int iNumTurns, bool bIgnoreInstant = false);
	void updateYieldPerTurnHistory();

	int getInstantYieldAvg(YieldTypes eYield, int iTurnA, int iTurnB) const;
	int getInstantYieldValue(YieldTypes eYield, int iTurn) const;
	void changeInstantYieldValue(YieldTypes eYield, int iValue);

	void LogInstantYield(YieldTypes eYield, int iValue, InstantYieldType eInstantYield, CvCity* pCity);

	CvString getInstantYieldHistoryTooltip(int iGameTurn, int iNumPreviousTurnsToCount);

	int getInstantTourismPerPlayerAvg(PlayerTypes ePlayer, int iTurnA, int iTurnB) const;
	int getInstantTourismPerPlayerValue(PlayerTypes ePlayer, int iTurn) const;
	void changeInstantTourismPerPlayerValue(PlayerTypes ePlayer , int iValue);

	// Arbitrary Script Data
	std::string getScriptData() const;
	void setScriptData(const std::string& szNewValue);

	const CvString& getPbemEmailAddress() const;
	void setPbemEmailAddress(const char* szAddress);

	int getUnitExtraCost(UnitClassTypes eUnitClass) const;
	void setUnitExtraCost(UnitClassTypes eUnitClass, int iCost);

	void addAnnexedMilitaryCityStates(PlayerTypes eMinor);
	void removeAnnexedMilitaryCityStates(PlayerTypes eMinor);
	void updateTimerAnnexedMilitaryCityStates();

	void UpdateEspionageYields(bool bIncoming);
	void AddEspionageEvent(PlayerTypes eOtherPlayer, bool bIncoming, bool bIdentified, int iStartTurn, int iEndTurn, YieldTypes eYield, int iAmount);
	void RemoveEspionageEventsForPlayer(PlayerTypes ePlayer);
	void ProcessEspionageEvents();

	int GetYieldPerTurnFromEspionageEvents(YieldTypes eYield, bool bIncoming) const;

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

	int GetBorderGrowthRateIncreaseGlobal() const;
	void ChangeBorderGrowthRateIncreaseGlobal(int iChange);

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

	int GetPlotDanger(const CvPlot& Plot, const CvUnit* pUnit, const UnitIdContainer& unitsToIgnore, int iExtraDamage, AirActionType iAirAction = AIR_ACTION_ATTACK);
	int GetPlotDanger(const CvCity* pCity, const CvUnit* pPretendGarrison = NULL);
	int GetPlotDanger(const CvPlot& Plot, bool bFixedDamageOnly);
	void ResetDangerCache(const CvPlot& Plot, int iRange);
	int GetDangerPlotAge() const;
	std::vector<CvUnit*> GetPossibleAttackers(const CvPlot& Plot, TeamTypes eTeamForVisibilityCheck);

	bool IsKnownAttacker(const CvUnit* pAttacker);
	bool AddKnownAttacker(const CvUnit* pAttacker);

	CvCity* GetClosestCity(const CvPlot* pPlot, int iSearchRadius, bool bSameLandmass);

	int GetNumPuppetCities() const;
	int GetNumRealCities() const;
	int GetNumCapitalCities() const;
	int GetNumMinorsControlled() const;
	int GetNumEffectiveCities(bool bIncludePuppets = false);
	int GetNumEffectiveCoastalCities() const;

#if defined(MOD_BALANCE_CORE_MILITARY)
	int GetFractionOriginalCapitalsUnderControl() const;
	bool OwnsOurCity(PlayerTypes ePlayer);
	int GetNumOurCitiesOwnedBy(PlayerTypes ePlayer);
	int CalculateDefensivePactLimit(bool bIsAITradeWithHumanPossible = false) const;
	bool IsIgnoreDefensivePactLimit() const;
	int GetMilitaryRating() const;
	void SetMilitaryRating(int iValue);
	void ChangeMilitaryRating(int iChange);
	void DoMilitaryRatingDecay();
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

	int GetUnitSupplyFromExpendedGreatPeople() const;
	void ChangeUnitSupplyFromExpendedGreatPeople(int iChange);

	int GetAvgUnitExp100() const;
#endif

	int GetNumNaturalWondersDiscoveredInArea() const;
	void SetNumNaturalWondersDiscoveredInArea(int iValue);
	void ChangeNumNaturalWondersDiscoveredInArea(int iChange);

	int GetNumNaturalWondersInOwnedPlots();

	int GetTurnsSinceSettledLastCity() const;
	void SetTurnsSinceSettledLastCity(int iValue);
	void ChangeTurnsSinceSettledLastCity(int iChange);

	bool HaveGoodSettlePlot(int iAreaID);
	CvPlot* GetBestSettlePlot(const CvUnit* pUnit, CvAIOperation* pOpToIgnore=NULL, bool bForceLogging=false) const;
	PlayerTypes GetPlayerWhoStoleMyFavoriteCitySite();
	int GetSettlePlotQualityMeasure(CvPlot* pPlot);

	// New Victory Stuff
	int GetNumWonders() const;
	void ChangeNumWonders(int iValue);
	int GetNumPolicies() const;
	void ChangeNumPolicies(int iValue);
	int GetNumGreatPeople() const;
	void ChangeNumGreatPeople(int iValue);
	// End New Victory Stuff

#if defined(MOD_BALANCE_CORE)
	std::vector<int> GetTotalBuildingCount(bool bIncludePuppets=false) const;
	void SetBestWonderCities();
	bool isCapitalCompetitive();
	CvCity* GetBestProductionCity( BuildingTypes eBuilding = NO_BUILDING, ProjectTypes eProject = NO_PROJECT);
	bool IsCityCompetitive(CvCity* pCity, BuildingTypes eBuilding = NO_BUILDING, ProjectTypes eProject = NO_PROJECT);
#endif
	void DoAdoptedGreatPersonCityStatePolicy();

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

	bool CanEmbark() const;
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
	int AddNotification(NotificationTypes eNotificationType, const char* sMessage, const char* sSummary, CvPlot* pPlot = NULL, int iGameDataIndex = -1, int iExtraGameData = -1);
	int AddNotification(NotificationTypes eNotificationType, const char* sMessage, const char* sSummary, int iGameDataIndex, int iExtraGameData = -1);
#if defined(MOD_WH_MILITARY_LOG)
	CvEventLog* GetMilitaryLog() const;
	bool AddMilitaryEvent(const char* sMessage, CvPlot* pPlot, PlayerTypes eOtherPlayer, int iData1 = -1, int iData2 = -1, int iData3 = -1, int iData4 = -1);
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
	vector<PlayerTypes> GetUnfriendlyMajors() const;
	int CountNumDangerousMajorsAtWarWith(bool bExcludePhonyWars, bool bExcludeIfNoTarget) const;
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

	// for serialization
	template<typename Player, typename Visitor>
	static void Serialize(Player& player , Visitor& visitor);
	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

	virtual void AI_init() = 0;
	virtual void AI_reset() = 0;
	virtual void AI_doTurnPre() = 0;
	virtual void AI_doTurnPost() = 0;
	virtual void AI_doTurnUnitsPre() = 0;
	virtual void AI_doTurnUnitsPost() = 0;
	virtual void AI_unitUpdate() = 0;
	virtual void AI_conquerCity(CvCity* pCity, PlayerTypes ePlayerToLiberate, bool bGift, bool bAllowSphereRemoval) = 0;
	bool HasSameIdeology(PlayerTypes ePlayer) const;

#if defined(MOD_BALANCE_CORE_EVENTS)
	virtual void AI_DoEventChoice(EventTypes eEvent) = 0;
	virtual bool AI_DoEspionageEventChoice(CityEventTypes eEvent, int uiSpyIndex, CvCity* pCity) = 0;
#endif

	virtual void computeFoundValueThreshold();
	virtual void updatePlotFoundValues();
	virtual void invalidatePlotFoundValues();
	virtual int getPlotFoundValue(int iX, int iY);
	virtual void setPlotFoundValue(int iX, int iY, int iValue);

	virtual void AI_chooseFreeGreatPerson() = 0;
	virtual void AI_chooseFreeTech() = 0;
	virtual void AI_chooseResearch() = 0;
	virtual void AI_launch(VictoryTypes eVictory) = 0;
	virtual OperationSlot PeekAtNextUnitToBuildForOperationSlot(CvCity* pCity, bool& bCitySameAsMuster) = 0;
	virtual void CityCommitToBuildUnitForOperationSlot(OperationSlot slot) = 0;
	virtual void CityUncommitToBuildUnitForOperationSlot(OperationSlot thisSlot) = 0;
	virtual void CityFinishedBuildingUnitForOperationSlot(OperationSlot thisSlot, CvUnit* pThisUnit) = 0;
	virtual int GetNumUnitsNeededToBeBuilt() = 0;
	const CvSyncArchive<CvPlayer>& getSyncArchive() const;
	CvSyncArchive<CvPlayer>& getSyncArchive();
	void disconnected();
	void reconnected();
	bool hasBusyUnitUpdatesRemaining() const;
	void setBusyUnitUpdatesRemaining(int iUpdateCount);
	bool hasUnitsThatNeedAIUpdate() const;

	void checkInitialTurnAIProcessed();
	void checkRunAutoMovesForEveryone();
	std::string debugDump(const FAutoVariableBase&) const;
	std::string stackTraceRemark(const FAutoVariableBase&) const;

	CvPlayerAchievements& GetPlayerAchievements(){return m_kPlayerAchievements;}

	bool hasTurnTimerExpired();
#if defined(MOD_BALANCE_CORE)
	int GetScoreFromMinorAllies() const;
	int GetScoreFromMilitarySize() const;
#endif

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

	int GetCityDistancePathLength( const CvPlot* pPlot ) const;
	CvCity* GetClosestCityByPathLength( const CvPlot* pPlot) const;
	int GetCityDistanceInPlots(const CvPlot* pPlot) const;
	CvCity* GetClosestCityByPlots(const CvPlot* pPlot) const;
	CvCity* GetClosestCityToUsByPlots(PlayerTypes eOtherPlayer) const;
	CvCity* GetClosestCityToCity(const CvCity* pRefCity);

	void setUnlockedGrowthAnywhereThisTurn(bool bValue);
	bool unlockedGrowthAnywhereThisTurn() const;

	bool IsEarlyExpansionPhase() const;

protected:
	class ConqueredByBoolField
	{
	public:
		void read(FDataStream& stream);
		void write(FDataStream& stream) const;

		enum
		{
			eSize = 32,
			eCount = (MAX_PLAYERS / eSize) + (MAX_PLAYERS % eSize == 0 ? 0 : 1),
			eTotalBits = eCount * eSize
		};
		uint32 m_bits[eCount];

		bool GetBit(const uint uiEntry) const
		{
			const uint uiOffset = uiEntry/eSize;
			return (m_bits[uiOffset] & 1<<(uiEntry-(eSize*uiOffset))) != 0;
		}
		void SetBit(const uint uiEntry)
		{
			const uint uiOffset = uiEntry/eSize;
			m_bits[uiOffset] |= 1<<(uiEntry-(eSize*uiOffset));
		}
		void ClearBit(const uint uiEntry)
		{
			const uint uiOffset = uiEntry/eSize;
			m_bits[uiOffset] &= ~(1<<(uiEntry-(eSize*uiOffset)));
		}
		void ToggleBit(const uint uiEntry)
		{
			const uint uiOffset = uiEntry/eSize;
			m_bits[uiOffset] ^= 1<<(uiEntry-(eSize*uiOffset));
		}
		void ClearAll()
		{
			for(uint i = 0; i <eCount; ++i)
			{
				m_bits[i] = 0;
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

	SYNC_ARCHIVE_MEMBER(CvPlayer)

	PlayerTypes m_eID;
	LeaderHeadTypes m_ePersonalityType;

	int m_iStartingX;
	int m_iStartingY;
	int m_iTotalPopulation;
	int m_iTotalLand;
	int m_iTotalLandScored;
	int m_iJONSCulturePerTurnForFree;
	int m_iJONSCultureCityModifier;
	int m_iJONSCulture;
	int m_iJONSCultureEverGenerated;
	int m_iWondersConstructed;
	int m_iCulturePerWonder;
	int m_iCultureWonderMultiplier;
	int m_iCulturePerTechResearched;
	int m_iFaith;
	int m_iFaithEverGenerated;
	int m_iHappiness;
	int m_iUnhappiness;
	int m_iHappinessTotal;
	int m_iEmpireSizeModifierReductionGlobal;
	int m_iDistressFlatReductionGlobal;
	int m_iPovertyFlatReductionGlobal;
	int m_iIlliteracyFlatReductionGlobal;
	int m_iBoredomFlatReductionGlobal;
	int m_iReligiousUnrestFlatReductionGlobal;
	int m_iBasicNeedsMedianModifierGlobal;
	int m_iGoldMedianModifierGlobal;
	int m_iScienceMedianModifierGlobal;
	int m_iCultureMedianModifierGlobal;
	int m_iReligiousUnrestModifierGlobal;
	int m_iBasicNeedsMedianModifierCapital;
	int m_iGoldMedianModifierCapital;
	int m_iScienceMedianModifierCapital;
	int m_iCultureMedianModifierCapital;
	int m_iReligiousUnrestModifierCapital;
	int m_iLandmarksTourismPercentGlobal;
	int m_iGreatWorksTourismModifierGlobal;
	int m_iCenterOfMassX;
	int m_iCenterOfMassY;
	int m_iReferenceFoundValue;
	int m_iReformationFollowerReduction;
	bool m_bIsReformation;
	std::vector<int> m_viInstantYieldsTotal;
	std::tr1::unordered_map<YieldTypes, int> m_miLocalInstantYieldsTotal;
	std::tr1::unordered_map<YieldTypes, std::vector<int>> m_aiYieldHistory;
	int m_iUprisingCounter;
	int m_iExtraHappinessPerLuxury;
	int m_iUnhappinessFromUnits;
	int m_iUnhappinessFromUnitsMod;
	int m_iUnhappinessMod;
	int m_iCityCountUnhappinessMod;
	int m_iOccupiedPopulationUnhappinessMod;
	int m_iCapitalUnhappinessMod;
	int m_iCityRevoltCounter;
	int m_iHappinessPerGarrisonedUnitCount;
	int m_iHappinessPerTradeRouteCount;
	int m_iHappinessPerXPopulation;
#if defined(MOD_BALANCE_CORE_POLICIES)
	int m_iHappinessPerXPopulationGlobal;
	int m_iIdeologyPoint;
	int m_iNoXPLossUnitPurchase;
	int m_iXCSAlliesLowersPolicyNeedWonders;
	int m_iHappinessFromMinorCivs;
	int m_iPositiveWarScoreTourismMod;
	int m_iIsNoCSDecayAtWar;
	int m_iMinimumAllyInfluenceIncreaseAtWar;
	int m_iCanBullyFriendlyCS;
	int m_iKeepConqueredBuildings;
	int m_iBullyGlobalCSReduction;	
#endif
	int m_iIsVassalsNoRebel;
	int m_iVassalYieldBonusModifier;
	int m_iCSYieldBonusModifier;
	int m_iHappinessFromLeagues;
	int m_iWoundedUnitDamageMod;
	int m_iUnitUpgradeCostMod;
	int m_iBarbarianCombatBonus;
	int m_iAlwaysSeeBarbCampsCount;
	int m_iHappinessPerCity;
	int m_iHappinessPerXPolicies;
	int m_iExtraHappinessPerXPoliciesFromPolicies;
	int m_iHappinessPerXGreatWorks;
	int m_iEspionageModifier;
	int m_iEspionageTurnsModifierFriendly;
	int m_iEspionageTurnsModifierEnemy;
	int m_iSpyStartingRank;
#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
	int m_iConversionModifier;
#endif
	int m_iFoodInCapitalFromAnnexedMinors;
	int m_iFoodInOtherCitiesFromAnnexedMinors;
	int m_iGoldPerTurnFromAnnexedMinors;
	int m_iCulturePerTurnFromAnnexedMinors;
	int m_iSciencePerTurnFromAnnexedMinors;
	int m_iFaithPerTurnFromAnnexedMinors;
	int	m_iHappinessFromAnnexedMinors;
	int m_iExtraLeagueVotes;
	int m_iImprovementLeagueVotes;
	int m_iFaithToVotes;
	int m_iCapitalsToVotes;
	int m_iDoFToVotes;
	int m_iRAToVotes;
	int m_iDefensePactsToVotes;
	int m_iGPExpendInfluence;
	bool m_bIsLeagueAid;
	bool m_bIsLeagueScholar;
	bool m_bIsLeagueArt;
	int m_iScienceRateFromLeague;
	int m_iScienceRateFromLeagueAid;
	int m_iLeagueCultureCityModifier;
	int m_iAdvancedStartPoints;
	int m_iAttackBonusTurns;
	int m_iCultureBonusTurns;
	int m_iTourismBonusTurns;
	int m_iGoldenAgeProgressMeter;
	int m_iGoldenAgeMeterMod;
	int m_iNumGoldenAges;
	int m_iGoldenAgeTurns;
	int m_iNumUnitGoldenAges;
	int m_iStrikeTurns;
	int m_iGoldenAgeModifier;
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
	int m_iProductionBonusTurnsConquest;
	int m_iCultureBonusTurnsConquest;
	int m_iFreeGreatPeopleCreated;
	int m_iFreeGreatGeneralsCreated;
	int m_iFreeGreatAdmiralsCreated;
#if defined(MOD_GLOBAL_SEPARATE_GP_COUNTERS)
	int m_iFreeGreatMerchantsCreated;
	int m_iFreeGreatScientistsCreated;
	int m_iFreeGreatEngineersCreated;
#endif
	int m_iFreeGreatWritersCreated;
	int m_iFreeGreatArtistsCreated;
	int m_iFreeGreatMusiciansCreated;
	int m_iFreeGreatDiplomatsCreated;
#if defined(MOD_BALANCE_CORE)
	int m_iGPExtra1Created;
	int m_iGPExtra2Created;
	int m_iGPExtra3Created;
	int m_iGPExtra4Created;
	int m_iGPExtra5Created;
	int m_iFreeGPExtra1Created;
	int m_iFreeGPExtra2Created;
	int m_iFreeGPExtra3Created;
	int m_iFreeGPExtra4Created;
	int m_iFreeGPExtra5Created;
#endif
#endif
	int m_iGreatPeopleCreated;
	int m_iGreatGeneralsCreated;
	int m_iGreatAdmiralsCreated;
#if defined(MOD_GLOBAL_SEPARATE_GP_COUNTERS)
	int m_iGreatMerchantsCreated;
	int m_iGreatScientistsCreated;
	int m_iGreatEngineersCreated;
#endif
	int m_iGreatWritersCreated;
	int m_iGreatArtistsCreated;
	int m_iGreatMusiciansCreated;
	int m_iGreatDiplomatsCreated;
	int m_iDiplomatsFromFaith;
#if defined(MOD_BALANCE_CORE)
	int m_iGPExtra1FromFaith;
	int m_iGPExtra2FromFaith;
	int m_iGPExtra3FromFaith;
	int m_iGPExtra4FromFaith;
	int m_iGPExtra5FromFaith;
#endif
	int m_iMerchantsFromFaith;
	int m_iScientistsFromFaith;
	int m_iWritersFromFaith;
	int m_iArtistsFromFaith;
	int m_iMusiciansFromFaith;
	int m_iGeneralsFromFaith;
	int m_iAdmiralsFromFaith;
	int m_iEngineersFromFaith;
	int m_iGreatPeopleThresholdModifier;
	int m_iGreatGeneralsThresholdModifier;
	int m_iGreatAdmiralsThresholdModifier;
	int m_iGreatGeneralCombatBonus;
	int m_iAnarchyNumTurns;
	int m_iPolicyCostModifier;
	int m_iGreatPeopleRateModifier;
	int m_iGreatPeopleRateModFromBldgs;
	int m_iGreatGeneralRateModifier;
	int m_iGreatGeneralRateModFromBldgs;
	int m_iDomesticGreatGeneralRateModifier;
	int m_iGreatAdmiralRateModifier;
	int m_iGreatWriterRateModifier;
	int m_iGreatArtistRateModifier;
	int m_iGreatMusicianRateModifier;
	int m_iGreatMerchantRateModifier;
	int m_iGreatDiplomatRateModifier;
	int m_iGreatScientistRateModifier;
	int m_iGreatScientistBeakerModifier;
	int m_iGreatEngineerHurryMod;
	int m_iTechCostXCitiesModifier;
	int m_iTourismCostXCitiesMod;
	int m_iGreatEngineerRateModifier;
	int m_iGreatPersonExpendGold;
	int m_iNoUnhappfromXSpecialists;
	int m_iHappfromXSpecialists;
	int m_iNoUnhappfromXSpecialistsCapital;
	int m_iSpecialistFoodChange;
	int m_iWarWearinessModifier;
	int m_iWarScoreModifier;
#if defined(MOD_BALANCE_CORE_POLICIES)
	int m_iGarrisonsOccupiedUnhappinessMod;
	int m_iXPopulationConscription;
	int m_iExtraMoves;
	int m_iNoUnhappinessExpansion;
	int m_iNoUnhappyIsolation;
	int m_iDoubleBorderGrowthGA;
	int m_iDoubleBorderGrowthWLTKD;
	int m_iIncreasedQuestInfluence;
	int m_iCultureBombBoost;
	int m_iPuppetProdMod;
	int m_iOccupiedProdMod;
	int m_iGoldInternalTrade;
	int m_iFreeWCVotes;
	int m_iInfluenceGPExpend;
	int m_iFreeTradeRoute;
	int m_iFreeSpy;
	int m_iReligionDistance;
	int m_iPressureMod;
	int m_iTradeReligionModifier;
	int m_iCityStateCombatModifier;
	int m_iInfluenceForLiberation;
	int m_iExperienceForLiberation;
	int m_iUnitsInLiberatedCities;
	int m_iCityCaptureHealGlobal;
#endif
#if defined(MOD_BALANCE_CORE_SPIES)
	int m_iMaxAirUnits;
#endif
#if defined(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
	int m_iInvestmentModifier;
	int m_iMissionInfluenceModifier;
	int m_iHappinessPerActiveTradeRoute;
	int m_iCSResourcesCountMonopolies;
	int m_iConquestPerEraBuildingProductionMod;
	int m_iAdmiralLuxuryBonus;
	int m_iPuppetYieldPenaltyMod;
	int m_iNeedsModifierFromAirUnits;
	int m_iFlatDefenseFromAirUnits;
#endif
#if defined(MOD_POLICIES_UNIT_CLASS_REPLACEMENTS)
	std::map<UnitClassTypes, UnitClassTypes> m_piUnitClassReplacements;
#endif
	int m_iMaxGlobalBuildingProductionModifier;
	int m_iMaxTeamBuildingProductionModifier;
	int m_iMaxPlayerBuildingProductionModifier;
	int m_iFreeExperience;
	int m_iFreeExperienceFromBldgs;
	int m_iFreeExperienceFromMinors;
	int m_iFeatureProductionModifier;
	int m_iWorkerSpeedModifier;
	int m_iImprovementCostModifier;
	int m_iImprovementUpgradeRateModifier;
	int m_iSpecialistProductionModifier;
	int m_iMilitaryProductionModifier;
	int m_iSpaceProductionModifier;
	int m_iCityDefenseModifier;
	int m_iUnitFortificationModifier;
	int m_iUnitBaseHealModifier;
	int m_iWonderProductionModifier;
	int m_iSettlerProductionModifier;
	int m_iCapitalSettlerProductionModifier;
	int m_iUnitProductionMaintenanceMod;
	int m_iUnitGrowthMaintenanceMod;
	int m_iPolicyCostBuildingModifier;
	int m_iPolicyCostMinorCivModifier;
	int m_iInfluenceSpreadModifier;
	int m_iExtraVotesPerDiplomat;
	int m_iNumNukeUnits;
	int m_iNumOutsideUnits;
	int m_iBaseFreeUnits;
	int m_iBaseFreeMilitaryUnits;
	int m_iFreeUnitsPopulationPercent;
	int m_iFreeMilitaryUnitsPopulationPercent;
	int m_iGoldPerUnit;
	int m_iGoldPerMilitaryUnit;
	int m_iImprovementGoldMaintenanceMod;
#if defined(MOD_CIV6_WORKER)
	int m_iRouteBuilderCostMod;
#endif
	int m_iBuildingGoldMaintenanceMod;
	int m_iUnitGoldMaintenanceMod;
	int m_iUnitSupplyMod;
	int m_iExtraUnitCost;
	int m_iNumMilitaryUnits;
	int m_iHappyPerMilitaryUnit;
	int m_iHappinessToCulture;
	int m_iHappinessToScience;
	int m_iHalfSpecialistUnhappinessCount;
	int m_iHalfSpecialistFoodCount;
#if defined(MOD_BALANCE_CORE)
	int m_iHalfSpecialistFoodCapitalCount;
	int m_iTradeRouteLandDistanceModifier;
	int m_iTradeRouteSeaDistanceModifier;
	bool m_bNullifyInfluenceModifier;
#endif
	int m_iMilitaryFoodProductionCount;
	int m_iGoldenAgeCultureBonusDisabledCount;
	int m_iNumMissionarySpreads;
	int m_iSecondReligionPantheonCount;
	int m_iEnablesSSPartHurryCount;
	int m_iEnablesSSPartPurchaseCount;
	int m_iConscriptCount;
	int m_iMaxConscript;
	int m_iHighestUnitLevel;
	int m_iOverflowResearch;
	int m_iExpModifier;
	int m_iExpInBorderModifier;
	int m_iLevelExperienceModifier;
	int m_iMinorQuestFriendshipMod;
	int m_iMinorGoldFriendshipMod;
	int m_iMinorFriendshipMinimum;
	int m_iMinorFriendshipDecayMod;
	int m_iMinorScienceAlliesCount;
	int m_iMinorResourceBonusCount;
	int m_iAbleToAnnexCityStatesCount;
	int m_iOnlyTradeSameIdeology;
#if defined(MOD_BALANCE_CORE)
	int m_iSupplyFreeUnits; //military units which don't count against the supply limit
	std::vector<CvString> m_aistrInstantYield; // not serialized
	std::map<int, CvString> m_aistrInstantGreatPersonProgress;
	std::vector<bool> m_abActiveContract;
	int m_iJFDReformCooldownRate;
	int m_iJFDGovernmentCooldownRate;
	CvString m_strJFDPoliticKey;
	CvString m_strJFDLegislatureName;
	int m_iJFDPoliticLeader;
	int m_iJFDSovereignty;
	int m_iJFDGovernment;
	int m_iJFDReformCooldown;
	int m_iJFDGovernmentCooldown;
	int m_iJFDPiety;
	int m_iJFDPietyRate;
	int m_iJFDConversionTurn;
	bool m_bJFDSecularized;
	CvString m_strJFDCurrencyName;
	int m_iJFDProsperity;
	int m_iJFDCurrency;
	int m_iGoldenAgeTourism;
	int m_iExtraCultureandScienceTradeRoutes;
	int m_iArchaeologicalDigTourism;
	int m_iUpgradeCSVassalTerritory;
	int m_iRazingSpeedBonus;
	int m_iNoPartisans;
	int m_iSpawnCooldown;
	bool m_bTradeRoutesInvulnerable;
	int m_iTRSpeedBoost;
	int m_iVotesPerGPT;
	int m_iTRVisionBoost;
	int m_iEventTourism;
	std::vector<int> m_aiGlobalTourismAlreadyReceived;
	int m_iEventTourismCS;
	int m_iNumHistoricEvent;
	int m_iSingleVotes;
	int m_iMonopolyModFlat;
	int m_iMonopolyModPercent;
	int m_iCachedValueOfPeaceWithHuman;
	int m_iFaithPurchaseCooldown;
	int m_iCSAllies;
	int m_iCSFriends;
	std::vector<int> m_piCityFeatures;
	std::vector<int> m_piNumBuildings;
	std::vector<int> m_piNumBuildingsInPuppets;
	int m_iCitiesNeedingTerrainImprovements;
	std::vector<int> m_aiBestMilitaryCombatClassCity;
	std::vector<int> m_aiBestMilitaryDomainCity;
	std::vector<int> m_aiEventChoiceDuration;
	std::vector<int> m_aiEventIncrement;
	std::vector<int> m_aiEventCooldown;
	std::vector<bool> m_abEventActive;
	std::vector<bool> m_abEventChoiceActive;
	std::vector<bool> m_abEventChoiceFired;
	std::vector<bool> m_abEventFired;
	int m_iPlayerEventCooldown;
	std::vector<FeatureTypes> m_ownedNaturalWonders;
	std::vector<int> m_paiUnitClassProductionModifiers;
	int m_iExtraSupplyPerPopulation;
	int m_iCitySupplyFlatGlobal;
	int m_iUnitSupplyFromExpendedGP;
	int m_iMissionaryExtraStrength;
#endif
	int m_iFreeSpecialist;
	int m_iCultureBombTimer;
	int m_iConversionTimer;
	int m_iCapitalCityID;
	int m_iCitiesLost;
	int m_iMilitaryRating;
	int m_iMilitaryMight;
	int m_iEconomicMight;
	int m_iProductionMight;
	int m_iTurnSliceMightRecomputed;
	int m_iNewCityExtraPopulation;
	int m_iFreeFoodBox;
	int m_iScenarioScore1;
	int m_iScenarioScore2;
	int m_iScenarioScore3;
	int m_iScenarioScore4;
	int m_iScoreFromFutureTech;
	int m_iTurnLastAttackedMinorCiv;
	int m_iCombatExperienceTimes100;
	int m_iLifetimeCombatExperienceTimes100;
	int m_iNavalCombatExperienceTimes100;
	int m_iBorderObstacleCount;
#if defined(HH_MOD_BUILDINGS_FRUITLESS_PILLAGE)
	int m_iBorderGainlessPillageCount;
#endif
	int m_iPopRushHurryCount;
	int m_iTotalImprovementsBuilt;
	int m_iCostNextPolicy;
	int m_iNumBuilders;
	int m_iMaxNumBuilders;
	int m_iCityStrengthMod;
	int m_iCityGrowthMod;
	int m_iCapitalGrowthMod;
	int m_iNumPlotsBought;
	int m_iPlotGoldCostMod;
#if defined(MOD_TRAITS_CITY_WORKING) || defined(MOD_BUILDINGS_CITY_WORKING) || defined(MOD_POLICIES_CITY_WORKING) || defined(MOD_TECHS_CITY_WORKING)
	int m_iCityWorkingChange;
#endif
#if defined(MOD_TRAITS_CITY_AUTOMATON_WORKERS) || defined(MOD_BUILDINGS_CITY_AUTOMATON_WORKERS) || defined(MOD_POLICIES_CITY_AUTOMATON_WORKERS) || defined(MOD_TECHS_CITY_AUTOMATON_WORKERS)
	int m_iCityAutomatonWorkersChange;
#endif
	int m_iCachedGoldRate;
	int m_iBorderGrowthRateIncreaseGlobal;
	int m_iPlotCultureCostModifier;
	int m_iPlotCultureExponentModifier;
	int m_iNumCitiesPolicyCostDiscount;
	int m_iGarrisonedCityRangeStrikeModifier;
	int m_iGarrisonFreeMaintenanceCount;
	int m_iNumCitiesFreeCultureBuilding;
	int m_iNumCitiesFreeFoodBuilding;
	int m_iUnitPurchaseCostModifier;
	int m_iAllFeatureProduction;
	int m_iCityDistanceHighwaterMark; // this is used to determine camera zoom
	int m_iOriginalCapitalX;
	int m_iOriginalCapitalY;
	int m_iHolyCityX;
	int m_iHolyCityY;
	int m_iNumWonders;
	int m_iNumPolicies;
	int m_iNumGreatPeople;
	int m_iCityConnectionHappiness;
	int m_iHolyCityID;
	int m_iTurnsSinceSettledLastCity;
	int m_iNumNaturalWondersDiscoveredInArea;
	int m_iStrategicResourceMod;
	int m_iSpecialistCultureChange;
	int m_iGreatPeopleSpawnCounter;

	int m_iFreeTechCount;
	int m_iMedianTechPercentage;
	int m_iNumFreePolicies;
	int m_iNumFreePoliciesEver; 
	int m_iNumFreeTenets;

	int m_iLastSliceMoved; // not serialized


	uint m_uiStartTime;  // XXX save these?

	bool m_bHasUUPeriod;
	bool m_bNoNewWars;
	bool m_bTerribleShapeForWar;
	bool m_bHasBetrayedMinorCiv;
	bool m_bAlive;
	bool m_bEverAlive;
	bool m_bPotentiallyAlive;
	bool m_bTurnActive;
	bool m_bAutoMoves;					// Signal that we can process the auto moves when ready.
	bool						  m_bProcessedAutoMoves;		// Signal that we have processed the auto moves
	bool m_bEndTurn;					// Signal that the player has completed their turn.  The turn will still be active until the auto-moves have been processed.
	bool m_bDynamicTurnsSimultMode;
	bool m_bPbemNewTurn;
	bool m_bExtendedGame;
	bool m_bFoundedFirstCity;
	int m_iNumCitiesFounded;
	bool m_bStrike;
	bool m_bCramped;
	bool m_bLostCapital;
	PlayerTypes m_eConqueror;
	bool m_bLostHolyCity;
	PlayerTypes m_eHolyCityConqueror;
	bool m_bHasAdoptedStateReligion;

	std::vector<int> m_aiCityYieldChange;
	std::vector<int> m_aiCoastalCityYieldChange;
	std::vector<int> m_aiCapitalYieldChange;
	std::vector<int> m_aiCapitalYieldPerPopChange;
	std::vector<int> m_aiCapitalYieldPerPopChangeEmpire;
	std::vector<int> m_aiSeaPlotYield;
	std::vector<int> m_aiYieldRateModifier;
#if defined(MOD_BALANCE_CORE_POLICIES)
	std::vector<int> m_paiJFDPoliticPercent;
	std::vector<int> m_aiYieldFromMinors;
	std::vector<int> m_paiResourceFromCSAlliances;
	std::vector<int> m_paiResourceShortageValue;
	std::vector<int> m_aiYieldFromBirth;
	std::vector<int> m_aiYieldFromBirthCapital;
	std::vector<int> m_aiYieldFromDeath;
	std::vector<int> m_aiYieldFromPillage;
	std::vector<int> m_aiYieldFromVictory;
	std::vector<int> m_aiYieldFromConstruction;
	std::vector<int> m_aiYieldFromWorldWonderConstruction;
	std::vector<int> m_aiYieldFromTech;
	std::vector<int> m_aiYieldFromBorderGrowth;
	std::vector<int> m_aiYieldGPExpend;
	std::vector<int> m_aiConquerorYield;
	std::vector<int> m_aiFounderYield;
	std::vector<int> m_aiArtifactYieldBonus;
	std::vector<int> m_aiArtYieldBonus;
	std::vector<int> m_aiMusicYieldBonus;
	std::vector<int> m_aiLitYieldBonus;
	std::vector<int> m_aiFilmYieldBonus;
	std::vector<int> m_aiRelicYieldBonus;
	std::vector<int> m_aiReligionYieldRateModifier;
	std::vector<int> m_aiGoldenAgeYieldMod;
	std::vector<int> m_aiNumAnnexedCityStates;
	std::vector< std::pair<PlayerTypes, int> > m_AnnexedMilitaryCityStatesUnitSpawnTurns;
	std::vector<SPlayerActiveEspionageEvent> m_vActiveEspionageEventsList;
	std::vector<int> m_aiIncomingEspionageYields;
	std::vector<int> m_aiOutgoingEspionageYields;
	std::vector<int> m_aiYieldFromNonSpecialistCitizens;
	std::vector<int> m_aiYieldModifierFromGreatWorks;
	std::vector<int> m_aiYieldModifierFromActiveSpies;
	std::vector<int> m_aiYieldFromDelegateCount;
	std::vector<int> m_aiYieldForLiberation;
	std::vector<int> m_aiBuildingClassInLiberatedCities;
	std::vector<int> m_paiBuildingClassCulture;
	std::vector<int> m_aiDomainFreeExperiencePerGreatWorkGlobal;
	std::vector<int> m_aiCityYieldModFromMonopoly;
	std::vector<UnitAITypes> m_neededUnitAITypes;
	bool m_bAllowsProductionTradeRoutesGlobal;
	bool m_bAllowsFoodTradeRoutesGlobal;
	
#endif
#if defined(MOD_BALANCE_CORE)
	std::map<int, int> m_piDomainFreeExperience;
#endif

	std::vector<int> m_aiCapitalYieldRateModifier;
	std::vector<int> m_aiExtraYieldThreshold;
	std::vector<int> m_aiSpecialistExtraYield;
	std::vector<int> m_aiPlayerNumTurnsAtPeace;
	std::vector<int> m_aiPlayerNumTurnsAtWar;
	std::vector<int> m_aiPlayerNumTurnsSinceCityCapture;
	std::vector<int> m_aiWarValueLost;
	std::vector<int> m_aiWarDamageValue;
	std::vector<int> m_aiNumUnitsBuilt;
	std::vector<int> m_aiProximityToPlayer;
	std::vector<int> m_aiResearchAgreementCounter;
	std::vector<int> m_aiSiphonLuxuryCount;
	std::vector<int> m_aiGreatWorkYieldChange;
	std::vector<int> m_aiTourismBonusTurnsPlayer;

	typedef std::pair<uint, int> PlayerOptionEntry;
	typedef std::vector< PlayerOptionEntry > PlayerOptionsVector;
	PlayerOptionsVector m_aOptions;

	CvString m_strReligionKey;
	CvString m_strScriptData;

	CvString m_strEmbarkedGraphicOverride;

	std::vector<int> m_paiNumResourceUsed;
	std::vector<int> m_paiNumResourceTotal;
	std::vector<int> m_paiResourceGiftedToMinors;
	std::vector<int> m_paiResourceExport; //always to majors
	std::vector<int> m_paiResourceImportFromMajor;
	std::vector<int> m_paiResourceFromMinors;
	std::vector<int> m_paiResourcesSiphoned;
	std::vector<byte> m_aiNumResourceFromGP;
	std::vector<int> m_paiImprovementCount;
	std::vector<int> m_paiImprovementBuiltCount;
#if defined(MOD_BALANCE_CORE)
	std::vector<int> m_paiTotalImprovementsBuilt;
#endif
#if defined(MOD_IMPROVEMENTS_EXTENSIONS)
	std::map<RouteTypes, int> m_piResponsibleForRouteCount;
	std::map<ImprovementTypes, int> m_piResponsibleForImprovementCount;

	std::vector<int> m_paiBuildingChainSteps;
#endif
	std::vector<int> m_paiFreeBuildingCount;
	std::vector<int> m_paiFreePromotionCount;
	std::vector<int> m_paiUnitCombatProductionModifiers;
	std::vector<int> m_paiUnitCombatFreeExperiences;
	std::vector<int> m_paiUnitClassCount;
	std::vector<int> m_paiUnitClassMaking;
	std::vector<int> m_paiBuildingClassCount;
	std::vector<int> m_paiBuildingClassMaking;
	std::vector<int> m_paiProjectMaking;
	std::vector<int> m_paiHurryCount;
	std::vector<int> m_paiHurryModifier;

	bool m_bVassalLevy;
	int m_iVassalGoldMaintenanceMod;

#if defined(MOD_BALANCE_CORE)
	std::vector<int> m_paiNumCitiesFreeChosenBuilding;
	std::vector<bool> m_pabFreeChosenBuildingNewCity;
	std::vector<bool> m_pabAllCityFreeBuilding;
	std::vector<bool> m_pabNewFoundCityFreeUnit;
	std::vector<bool> m_pabNewFoundCityFreeBuilding;
#endif

	std::vector<bool> m_pabLoyalMember;

#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	std::vector<bool> m_pabHasGlobalMonopoly;
	std::vector<bool> m_pabHasStrategicMonopoly;
	std::vector<ResourceTypes> m_vResourcesWGlobalMonopoly;
	std::vector<ResourceTypes> m_vResourcesWStrategicMonopoly;
	int m_iCombatAttackBonusFromMonopolies;
	int m_iCombatDefenseBonusFromMonopolies;
#endif

	std::vector<bool> m_pabGetsScienceFromPlayer;

	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppaaiSpecialistExtraYield;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiYieldFromYieldGlobal;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiPlotYieldChange;
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
	std::vector<int> m_piYieldFromKills;
	std::vector<int> m_piYieldFromBarbarianKills;
	std::vector<int> m_piYieldChangeTradeRoute;
	std::vector<int> m_piYieldChangesNaturalWonder;
	std::vector<int> m_piYieldChangesPerReligion;
	std::vector<int> m_piYieldChangeWorldWonder;
	std::vector<int> m_piYieldFromMinorDemand;
	std::vector<int> m_piYieldFromWLTKD;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiBuildingClassYieldChange;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppaaiImprovementYieldChange;
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	std::map<GreatPersonTypes, std::map<MonopolyTypes, int>> m_ppiSpecificGreatPersonRateModifierFromMonopoly;
	std::map<GreatPersonTypes, std::map<MonopolyTypes, int>> m_ppiSpecificGreatPersonRateChangeFromMonopoly;
#endif

	CvUnitCycler	m_UnitCycle;	

	// slewis's tutorial variables!
	bool m_bEverPoppedGoody;
	bool m_bEverTrainedBuilder;
	// end slewis's tutorial variables

	EndTurnBlockingTypes  m_eEndTurnBlockingType;
	int  m_iEndTurnBlockingNotificationIndex;

	CLinkList<TechTypes> m_researchQueue;
	CLinkList<CvString> m_cityNames;

	TContainer<CvCityAI> m_cities;
	TContainer<CvUnit> m_units;
	TContainer<CvArmyAI> m_armyAIs;

	std::vector< std::pair<int, CvAIOperation*> > m_AIOperations;
	std::vector< std::pair<UnitClassTypes, int> > m_aUnitExtraCosts;

	std::map<CvString,TurnData> m_ReplayData;

	std::deque< pair< int, vector<int> > > m_ppiInstantYieldHistoryValues;
	std::deque< pair< int, vector<int> > > m_ppiInstantTourismPerPlayerHistoryValues;

	void doResearch();
	void doWarnings();

	// Danger plots!
	CvDangerPlots* m_pDangerPlots;

	int m_iPreviousBestSettlePlot;
	int m_iFoundValueOfCapital;

	// not serialized
	std::vector<int> m_viPlotFoundValues;
	int	m_iPlotFoundValuesUpdateTurn;
	bool m_bUnlockedGrowthAnywhereThisTurn;

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
#if defined(MOD_WH_MILITARY_LOG)
	CvEventLog* m_pMilitaryLog;
#endif
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

	int m_iNumFreeGreatPeople;
	int m_iNumMayaBoosts;
	int m_iNumFaithGreatPeople;
	int m_iNumArchaeologyChoices;

	FaithPurchaseTypes m_eFaithPurchaseType;
	int m_iFaithPurchaseIndex;

	void checkArmySizeAchievement();

	friend class CvPlayerManager;

	CvPlayerAchievements m_kPlayerAchievements;

#if defined(MOD_BALANCE_CORE_MILITARY)
	//percent
	int m_iFractionOriginalCapitalsUnderControl;
	int m_iAvgUnitExp100;
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
	int m_iNumMilitarySeaUnits;
	int m_iNumMilitaryAirUnits;
	int m_iNumMilitaryLandUnits;
	int m_iMilitarySeaMight;
	int m_iMilitaryAirMight;
	int m_iMilitaryLandMight;
#endif

	std::vector<int> m_vCityConnectionPlots; //serialized

	friend FDataStream& operator>>(FDataStream&, CvPlayer::ConqueredByBoolField&);
	friend FDataStream& operator<<(FDataStream&, const CvPlayer::ConqueredByBoolField&);
};

inline FDataStream& operator>>(FDataStream& stream, CvPlayer::ConqueredByBoolField& bfEverConqueredBy)
{
	bfEverConqueredBy.read(stream);
	return stream;
}
inline FDataStream& operator<<(FDataStream& stream, const CvPlayer::ConqueredByBoolField& bfEverConqueredBy)
{
	bfEverConqueredBy.write(stream);
	return stream;
}

bool CancelActivePlayerEndTurn();

namespace FSerialization
{
void SyncPlayer();
void ClearPlayerDeltas();
}

SYNC_ARCHIVE_BEGIN(CvPlayer)
SYNC_ARCHIVE_VAR(PlayerTypes, m_eID)
SYNC_ARCHIVE_VAR(LeaderHeadTypes, m_ePersonalityType)
SYNC_ARCHIVE_VAR(int, m_iStartingX)
SYNC_ARCHIVE_VAR(int, m_iStartingY)
SYNC_ARCHIVE_VAR(int, m_iTotalPopulation)
SYNC_ARCHIVE_VAR(int, m_iTotalLand)
SYNC_ARCHIVE_VAR(int, m_iTotalLandScored)
SYNC_ARCHIVE_VAR(int, m_iJONSCulturePerTurnForFree)
SYNC_ARCHIVE_VAR(int, m_iJONSCultureCityModifier)
SYNC_ARCHIVE_VAR(int, m_iJONSCulture)
SYNC_ARCHIVE_VAR(int, m_iJONSCultureEverGenerated)
SYNC_ARCHIVE_VAR(int, m_iWondersConstructed)
SYNC_ARCHIVE_VAR(int, m_iCulturePerWonder)
SYNC_ARCHIVE_VAR(int, m_iCultureWonderMultiplier)
SYNC_ARCHIVE_VAR(int, m_iCulturePerTechResearched)
SYNC_ARCHIVE_VAR(int, m_iFaith)
SYNC_ARCHIVE_VAR(int, m_iFaithEverGenerated)
SYNC_ARCHIVE_VAR(int, m_iHappiness)
SYNC_ARCHIVE_VAR(int, m_iUnhappiness)
SYNC_ARCHIVE_VAR(int, m_iHappinessTotal)
SYNC_ARCHIVE_VAR(int, m_iEmpireSizeModifierReductionGlobal)
SYNC_ARCHIVE_VAR(int, m_iDistressFlatReductionGlobal)
SYNC_ARCHIVE_VAR(int, m_iPovertyFlatReductionGlobal)
SYNC_ARCHIVE_VAR(int, m_iIlliteracyFlatReductionGlobal)
SYNC_ARCHIVE_VAR(int, m_iBoredomFlatReductionGlobal)
SYNC_ARCHIVE_VAR(int, m_iReligiousUnrestFlatReductionGlobal)
SYNC_ARCHIVE_VAR(int, m_iBasicNeedsMedianModifierGlobal)
SYNC_ARCHIVE_VAR(int, m_iGoldMedianModifierGlobal)
SYNC_ARCHIVE_VAR(int, m_iCultureMedianModifierGlobal)
SYNC_ARCHIVE_VAR(int, m_iScienceMedianModifierGlobal)
SYNC_ARCHIVE_VAR(int, m_iReligiousUnrestModifierGlobal)
SYNC_ARCHIVE_VAR(int, m_iBasicNeedsMedianModifierCapital)
SYNC_ARCHIVE_VAR(int, m_iGoldMedianModifierCapital)
SYNC_ARCHIVE_VAR(int, m_iScienceMedianModifierCapital)
SYNC_ARCHIVE_VAR(int, m_iCultureMedianModifierCapital)
SYNC_ARCHIVE_VAR(int, m_iReligiousUnrestModifierCapital)
SYNC_ARCHIVE_VAR(int, m_iLandmarksTourismPercentGlobal)
SYNC_ARCHIVE_VAR(int, m_iGreatWorksTourismModifierGlobal)
SYNC_ARCHIVE_VAR(int, m_iCenterOfMassX)
SYNC_ARCHIVE_VAR(int, m_iCenterOfMassY)
SYNC_ARCHIVE_VAR(int, m_iReferenceFoundValue)
SYNC_ARCHIVE_VAR(int, m_iReformationFollowerReduction)
SYNC_ARCHIVE_VAR(bool, m_bIsReformation)
SYNC_ARCHIVE_VAR(std::vector<int>, m_viInstantYieldsTotal)
SYNC_ARCHIVE_VAR(SYNC_ARCHIVE_VAR_TYPE(std::tr1::unordered_map<YieldTypes, int>), m_miLocalInstantYieldsTotal)
SYNC_ARCHIVE_VAR(SYNC_ARCHIVE_VAR_TYPE(std::tr1::unordered_map<YieldTypes, std::vector<int>>), m_aiYieldHistory)
SYNC_ARCHIVE_VAR(int, m_iUprisingCounter)
SYNC_ARCHIVE_VAR(int, m_iExtraHappinessPerLuxury)
SYNC_ARCHIVE_VAR(int, m_iUnhappinessFromUnits)
SYNC_ARCHIVE_VAR(int, m_iUnhappinessFromUnitsMod)
SYNC_ARCHIVE_VAR(int, m_iUnhappinessMod)
SYNC_ARCHIVE_VAR(int, m_iCityCountUnhappinessMod)
SYNC_ARCHIVE_VAR(int, m_iOccupiedPopulationUnhappinessMod)
SYNC_ARCHIVE_VAR(int, m_iCapitalUnhappinessMod)
SYNC_ARCHIVE_VAR(int, m_iCityRevoltCounter)
SYNC_ARCHIVE_VAR(int, m_iHappinessPerGarrisonedUnitCount)
SYNC_ARCHIVE_VAR(int, m_iHappinessPerTradeRouteCount)
SYNC_ARCHIVE_VAR(int, m_iHappinessPerXPopulation)
SYNC_ARCHIVE_VAR(int, m_iHappinessPerXPopulationGlobal)
SYNC_ARCHIVE_VAR(int, m_iIdeologyPoint)
SYNC_ARCHIVE_VAR(int, m_iNoXPLossUnitPurchase)
SYNC_ARCHIVE_VAR(int, m_iXCSAlliesLowersPolicyNeedWonders)
SYNC_ARCHIVE_VAR(int, m_iHappinessFromMinorCivs)
SYNC_ARCHIVE_VAR(int, m_iPositiveWarScoreTourismMod)
SYNC_ARCHIVE_VAR(int, m_iIsNoCSDecayAtWar)
SYNC_ARCHIVE_VAR(int, m_iMinimumAllyInfluenceIncreaseAtWar)
SYNC_ARCHIVE_VAR(int, m_iCanBullyFriendlyCS)
SYNC_ARCHIVE_VAR(int, m_iKeepConqueredBuildings)
SYNC_ARCHIVE_VAR(int, m_iBullyGlobalCSReduction)
SYNC_ARCHIVE_VAR(int, m_iIsVassalsNoRebel)
SYNC_ARCHIVE_VAR(int, m_iVassalYieldBonusModifier)
SYNC_ARCHIVE_VAR(int, m_iCSYieldBonusModifier)
SYNC_ARCHIVE_VAR(int, m_iHappinessFromLeagues)
SYNC_ARCHIVE_VAR(int, m_iWoundedUnitDamageMod)
SYNC_ARCHIVE_VAR(int, m_iUnitUpgradeCostMod)
SYNC_ARCHIVE_VAR(int, m_iBarbarianCombatBonus)
SYNC_ARCHIVE_VAR(int, m_iAlwaysSeeBarbCampsCount)
SYNC_ARCHIVE_VAR(int, m_iHappinessPerCity)
SYNC_ARCHIVE_VAR(int, m_iHappinessPerXPolicies)
SYNC_ARCHIVE_VAR(int, m_iExtraHappinessPerXPoliciesFromPolicies)
SYNC_ARCHIVE_VAR(int, m_iHappinessPerXGreatWorks)
SYNC_ARCHIVE_VAR(int, m_iEspionageModifier)
SYNC_ARCHIVE_VAR(int, m_iEspionageTurnsModifierFriendly)
SYNC_ARCHIVE_VAR(int, m_iEspionageTurnsModifierEnemy)
SYNC_ARCHIVE_VAR(int, m_iSpyStartingRank)
SYNC_ARCHIVE_VAR(int, m_iConversionModifier)
SYNC_ARCHIVE_VAR(int, m_iFoodInCapitalFromAnnexedMinors)
SYNC_ARCHIVE_VAR(int, m_iFoodInOtherCitiesFromAnnexedMinors)
SYNC_ARCHIVE_VAR(int, m_iGoldPerTurnFromAnnexedMinors)
SYNC_ARCHIVE_VAR(int, m_iCulturePerTurnFromAnnexedMinors)
SYNC_ARCHIVE_VAR(int, m_iSciencePerTurnFromAnnexedMinors)
SYNC_ARCHIVE_VAR(int, m_iFaithPerTurnFromAnnexedMinors)
SYNC_ARCHIVE_VAR(int, m_iHappinessFromAnnexedMinors)
SYNC_ARCHIVE_VAR(int, m_iExtraLeagueVotes)
SYNC_ARCHIVE_VAR(int, m_iImprovementLeagueVotes)
SYNC_ARCHIVE_VAR(int, m_iFaithToVotes)
SYNC_ARCHIVE_VAR(int, m_iCapitalsToVotes)
SYNC_ARCHIVE_VAR(int, m_iDoFToVotes)
SYNC_ARCHIVE_VAR(int, m_iRAToVotes)
SYNC_ARCHIVE_VAR(int, m_iDefensePactsToVotes)
SYNC_ARCHIVE_VAR(int, m_iGPExpendInfluence)
SYNC_ARCHIVE_VAR(bool, m_bIsLeagueAid)
SYNC_ARCHIVE_VAR(bool, m_bIsLeagueScholar)
SYNC_ARCHIVE_VAR(bool, m_bIsLeagueArt)
SYNC_ARCHIVE_VAR(int, m_iScienceRateFromLeague)
SYNC_ARCHIVE_VAR(int, m_iScienceRateFromLeagueAid)
SYNC_ARCHIVE_VAR(int, m_iLeagueCultureCityModifier)
SYNC_ARCHIVE_VAR(int, m_iAdvancedStartPoints)
SYNC_ARCHIVE_VAR(int, m_iAttackBonusTurns)
SYNC_ARCHIVE_VAR(int, m_iCultureBonusTurns)
SYNC_ARCHIVE_VAR(int, m_iTourismBonusTurns)
SYNC_ARCHIVE_VAR(int, m_iGoldenAgeProgressMeter)
SYNC_ARCHIVE_VAR(int, m_iGoldenAgeMeterMod)
SYNC_ARCHIVE_VAR(int, m_iNumGoldenAges)
SYNC_ARCHIVE_VAR(int, m_iGoldenAgeTurns)
SYNC_ARCHIVE_VAR(int, m_iNumUnitGoldenAges)
SYNC_ARCHIVE_VAR(int, m_iStrikeTurns)
SYNC_ARCHIVE_VAR(int, m_iGoldenAgeModifier)
SYNC_ARCHIVE_VAR(int, m_iProductionBonusTurnsConquest)
SYNC_ARCHIVE_VAR(int, m_iCultureBonusTurnsConquest)
SYNC_ARCHIVE_VAR(int, m_iFreeGreatPeopleCreated)
SYNC_ARCHIVE_VAR(int, m_iFreeGreatGeneralsCreated)
SYNC_ARCHIVE_VAR(int, m_iFreeGreatAdmiralsCreated)
SYNC_ARCHIVE_VAR(int, m_iFreeGreatMerchantsCreated)
SYNC_ARCHIVE_VAR(int, m_iFreeGreatScientistsCreated)
SYNC_ARCHIVE_VAR(int, m_iFreeGreatEngineersCreated)
SYNC_ARCHIVE_VAR(int, m_iFreeGreatWritersCreated)
SYNC_ARCHIVE_VAR(int, m_iFreeGreatArtistsCreated)
SYNC_ARCHIVE_VAR(int, m_iFreeGreatMusiciansCreated)
SYNC_ARCHIVE_VAR(int, m_iFreeGreatDiplomatsCreated)
SYNC_ARCHIVE_VAR(int, m_iGPExtra1Created)
SYNC_ARCHIVE_VAR(int, m_iGPExtra2Created)
SYNC_ARCHIVE_VAR(int, m_iGPExtra3Created)
SYNC_ARCHIVE_VAR(int, m_iGPExtra4Created)
SYNC_ARCHIVE_VAR(int, m_iGPExtra5Created)
SYNC_ARCHIVE_VAR(int, m_iFreeGPExtra1Created)
SYNC_ARCHIVE_VAR(int, m_iFreeGPExtra2Created)
SYNC_ARCHIVE_VAR(int, m_iFreeGPExtra3Created)
SYNC_ARCHIVE_VAR(int, m_iFreeGPExtra4Created)
SYNC_ARCHIVE_VAR(int, m_iFreeGPExtra5Created)
SYNC_ARCHIVE_VAR(int, m_iGreatPeopleCreated)
SYNC_ARCHIVE_VAR(int, m_iGreatGeneralsCreated)
SYNC_ARCHIVE_VAR(int, m_iGreatAdmiralsCreated)
SYNC_ARCHIVE_VAR(int, m_iGreatMerchantsCreated)
SYNC_ARCHIVE_VAR(int, m_iGreatScientistsCreated)
SYNC_ARCHIVE_VAR(int, m_iGreatEngineersCreated)
SYNC_ARCHIVE_VAR(int, m_iGreatWritersCreated)
SYNC_ARCHIVE_VAR(int, m_iGreatArtistsCreated)
SYNC_ARCHIVE_VAR(int, m_iGreatMusiciansCreated)
SYNC_ARCHIVE_VAR(int, m_iGreatDiplomatsCreated)
SYNC_ARCHIVE_VAR(int, m_iDiplomatsFromFaith)
SYNC_ARCHIVE_VAR(int, m_iGPExtra1FromFaith)
SYNC_ARCHIVE_VAR(int, m_iGPExtra2FromFaith)
SYNC_ARCHIVE_VAR(int, m_iGPExtra3FromFaith)
SYNC_ARCHIVE_VAR(int, m_iGPExtra4FromFaith)
SYNC_ARCHIVE_VAR(int, m_iGPExtra5FromFaith)
SYNC_ARCHIVE_VAR(int, m_iMerchantsFromFaith)
SYNC_ARCHIVE_VAR(int, m_iScientistsFromFaith)
SYNC_ARCHIVE_VAR(int, m_iWritersFromFaith)
SYNC_ARCHIVE_VAR(int, m_iArtistsFromFaith)
SYNC_ARCHIVE_VAR(int, m_iMusiciansFromFaith)
SYNC_ARCHIVE_VAR(int, m_iGeneralsFromFaith)
SYNC_ARCHIVE_VAR(int, m_iAdmiralsFromFaith)
SYNC_ARCHIVE_VAR(int, m_iEngineersFromFaith)
SYNC_ARCHIVE_VAR(int, m_iGreatPeopleThresholdModifier)
SYNC_ARCHIVE_VAR(int, m_iGreatGeneralsThresholdModifier)
SYNC_ARCHIVE_VAR(int, m_iGreatAdmiralsThresholdModifier)
SYNC_ARCHIVE_VAR(int, m_iGreatGeneralCombatBonus)
SYNC_ARCHIVE_VAR(int, m_iAnarchyNumTurns)
SYNC_ARCHIVE_VAR(int, m_iPolicyCostModifier)
SYNC_ARCHIVE_VAR(int, m_iGreatPeopleRateModifier)
SYNC_ARCHIVE_VAR(int, m_iGreatPeopleRateModFromBldgs)
SYNC_ARCHIVE_VAR(int, m_iGreatGeneralRateModifier)
SYNC_ARCHIVE_VAR(int, m_iGreatGeneralRateModFromBldgs)
SYNC_ARCHIVE_VAR(int, m_iDomesticGreatGeneralRateModifier)
SYNC_ARCHIVE_VAR(int, m_iGreatAdmiralRateModifier)
SYNC_ARCHIVE_VAR(int, m_iGreatWriterRateModifier)
SYNC_ARCHIVE_VAR(int, m_iGreatArtistRateModifier)
SYNC_ARCHIVE_VAR(int, m_iGreatMusicianRateModifier)
SYNC_ARCHIVE_VAR(int, m_iGreatMerchantRateModifier)
SYNC_ARCHIVE_VAR(int, m_iGreatDiplomatRateModifier)
SYNC_ARCHIVE_VAR(int, m_iGreatScientistRateModifier)
SYNC_ARCHIVE_VAR(int, m_iGreatScientistBeakerModifier)
SYNC_ARCHIVE_VAR(int, m_iGreatEngineerHurryMod)
SYNC_ARCHIVE_VAR(int, m_iTechCostXCitiesModifier)
SYNC_ARCHIVE_VAR(int, m_iTourismCostXCitiesMod)
SYNC_ARCHIVE_VAR(int, m_iGreatEngineerRateModifier)
SYNC_ARCHIVE_VAR(int, m_iGreatPersonExpendGold)
SYNC_ARCHIVE_VAR(int, m_iNoUnhappfromXSpecialists)
SYNC_ARCHIVE_VAR(int, m_iHappfromXSpecialists)
SYNC_ARCHIVE_VAR(int, m_iNoUnhappfromXSpecialistsCapital)
SYNC_ARCHIVE_VAR(int, m_iSpecialistFoodChange)
SYNC_ARCHIVE_VAR(int, m_iWarWearinessModifier)
SYNC_ARCHIVE_VAR(int, m_iWarScoreModifier)
SYNC_ARCHIVE_VAR(int, m_iGarrisonsOccupiedUnhappinessMod)
SYNC_ARCHIVE_VAR(int, m_iXPopulationConscription)
SYNC_ARCHIVE_VAR(int, m_iExtraMoves)
SYNC_ARCHIVE_VAR(int, m_iNoUnhappinessExpansion)
SYNC_ARCHIVE_VAR(int, m_iNoUnhappyIsolation)
SYNC_ARCHIVE_VAR(int, m_iDoubleBorderGrowthGA)
SYNC_ARCHIVE_VAR(int, m_iDoubleBorderGrowthWLTKD)
SYNC_ARCHIVE_VAR(int, m_iIncreasedQuestInfluence)
SYNC_ARCHIVE_VAR(int, m_iCultureBombBoost)
SYNC_ARCHIVE_VAR(int, m_iPuppetProdMod)
SYNC_ARCHIVE_VAR(int, m_iOccupiedProdMod)
SYNC_ARCHIVE_VAR(int, m_iGoldInternalTrade)
SYNC_ARCHIVE_VAR(int, m_iFreeWCVotes)
SYNC_ARCHIVE_VAR(int, m_iInfluenceGPExpend)
SYNC_ARCHIVE_VAR(int, m_iFreeTradeRoute)
SYNC_ARCHIVE_VAR(int, m_iFreeSpy)
SYNC_ARCHIVE_VAR(int, m_iReligionDistance)
SYNC_ARCHIVE_VAR(int, m_iPressureMod)
SYNC_ARCHIVE_VAR(int, m_iTradeReligionModifier)
SYNC_ARCHIVE_VAR(int, m_iCityStateCombatModifier)
SYNC_ARCHIVE_VAR(int, m_iMaxAirUnits)
SYNC_ARCHIVE_VAR(int, m_iInvestmentModifier)
SYNC_ARCHIVE_VAR(int, m_iMissionInfluenceModifier)
SYNC_ARCHIVE_VAR(int, m_iHappinessPerActiveTradeRoute)
SYNC_ARCHIVE_VAR(int, m_iCSResourcesCountMonopolies)
SYNC_ARCHIVE_VAR(int, m_iConquestPerEraBuildingProductionMod)
SYNC_ARCHIVE_VAR(int, m_iAdmiralLuxuryBonus)
SYNC_ARCHIVE_VAR(int, m_iPuppetYieldPenaltyMod)
SYNC_ARCHIVE_VAR(int, m_iNeedsModifierFromAirUnits)
SYNC_ARCHIVE_VAR(int, m_iFlatDefenseFromAirUnits)
SYNC_ARCHIVE_VAR(int, m_iMaxGlobalBuildingProductionModifier)
SYNC_ARCHIVE_VAR(int, m_iMaxTeamBuildingProductionModifier)
SYNC_ARCHIVE_VAR(int, m_iMaxPlayerBuildingProductionModifier)
SYNC_ARCHIVE_VAR(int, m_iFreeExperience)
SYNC_ARCHIVE_VAR(int, m_iFreeExperienceFromBldgs)
SYNC_ARCHIVE_VAR(int, m_iFreeExperienceFromMinors)
SYNC_ARCHIVE_VAR(int, m_iFeatureProductionModifier)
SYNC_ARCHIVE_VAR(int, m_iWorkerSpeedModifier)
SYNC_ARCHIVE_VAR(int, m_iImprovementCostModifier)
SYNC_ARCHIVE_VAR(int, m_iImprovementUpgradeRateModifier)
SYNC_ARCHIVE_VAR(int, m_iSpecialistProductionModifier)
SYNC_ARCHIVE_VAR(int, m_iMilitaryProductionModifier)
SYNC_ARCHIVE_VAR(int, m_iSpaceProductionModifier)
SYNC_ARCHIVE_VAR(int, m_iCityDefenseModifier)
SYNC_ARCHIVE_VAR(int, m_iUnitFortificationModifier)
SYNC_ARCHIVE_VAR(int, m_iUnitBaseHealModifier)
SYNC_ARCHIVE_VAR(int, m_iWonderProductionModifier)
SYNC_ARCHIVE_VAR(int, m_iSettlerProductionModifier)
SYNC_ARCHIVE_VAR(int, m_iCapitalSettlerProductionModifier)
SYNC_ARCHIVE_VAR(int, m_iUnitProductionMaintenanceMod)
SYNC_ARCHIVE_VAR(int, m_iUnitGrowthMaintenanceMod)
SYNC_ARCHIVE_VAR(int, m_iPolicyCostBuildingModifier)
SYNC_ARCHIVE_VAR(int, m_iPolicyCostMinorCivModifier)
SYNC_ARCHIVE_VAR(int, m_iInfluenceSpreadModifier)
SYNC_ARCHIVE_VAR(int, m_iExtraVotesPerDiplomat)
SYNC_ARCHIVE_VAR(int, m_iNumNukeUnits)
SYNC_ARCHIVE_VAR(int, m_iNumOutsideUnits)
SYNC_ARCHIVE_VAR(int, m_iBaseFreeUnits)
SYNC_ARCHIVE_VAR(int, m_iBaseFreeMilitaryUnits)
SYNC_ARCHIVE_VAR(int, m_iFreeUnitsPopulationPercent)
SYNC_ARCHIVE_VAR(int, m_iFreeMilitaryUnitsPopulationPercent)
SYNC_ARCHIVE_VAR(int, m_iGoldPerUnit)
SYNC_ARCHIVE_VAR(int, m_iGoldPerMilitaryUnit)
SYNC_ARCHIVE_VAR(int, m_iImprovementGoldMaintenanceMod)
SYNC_ARCHIVE_VAR(int, m_iRouteBuilderCostMod)
SYNC_ARCHIVE_VAR(int, m_iBuildingGoldMaintenanceMod)
SYNC_ARCHIVE_VAR(int, m_iUnitGoldMaintenanceMod)
SYNC_ARCHIVE_VAR(int, m_iUnitSupplyMod)
SYNC_ARCHIVE_VAR(int, m_iExtraUnitCost)
SYNC_ARCHIVE_VAR(int, m_iNumMilitaryUnits)
SYNC_ARCHIVE_VAR(int, m_iHappyPerMilitaryUnit)
SYNC_ARCHIVE_VAR(int, m_iHappinessToCulture)
SYNC_ARCHIVE_VAR(int, m_iHappinessToScience)
SYNC_ARCHIVE_VAR(int, m_iHalfSpecialistUnhappinessCount)
SYNC_ARCHIVE_VAR(int, m_iHalfSpecialistFoodCount)
SYNC_ARCHIVE_VAR(int, m_iHalfSpecialistFoodCapitalCount)
SYNC_ARCHIVE_VAR(int, m_iTradeRouteLandDistanceModifier)
SYNC_ARCHIVE_VAR(int, m_iTradeRouteSeaDistanceModifier)
SYNC_ARCHIVE_VAR(bool, m_bNullifyInfluenceModifier)
SYNC_ARCHIVE_VAR(int, m_iMilitaryFoodProductionCount)
SYNC_ARCHIVE_VAR(int, m_iGoldenAgeCultureBonusDisabledCount)
SYNC_ARCHIVE_VAR(int, m_iNumMissionarySpreads)
SYNC_ARCHIVE_VAR(int, m_iSecondReligionPantheonCount)
SYNC_ARCHIVE_VAR(int, m_iEnablesSSPartHurryCount)
SYNC_ARCHIVE_VAR(int, m_iEnablesSSPartPurchaseCount)
SYNC_ARCHIVE_VAR(int, m_iConscriptCount)
SYNC_ARCHIVE_VAR(int, m_iMaxConscript)
SYNC_ARCHIVE_VAR(int, m_iHighestUnitLevel)
SYNC_ARCHIVE_VAR(int, m_iOverflowResearch)
SYNC_ARCHIVE_VAR(int, m_iExpModifier)
SYNC_ARCHIVE_VAR(int, m_iExpInBorderModifier)
SYNC_ARCHIVE_VAR(int, m_iLevelExperienceModifier)
SYNC_ARCHIVE_VAR(int, m_iMinorQuestFriendshipMod)
SYNC_ARCHIVE_VAR(int, m_iMinorGoldFriendshipMod)
SYNC_ARCHIVE_VAR(int, m_iMinorFriendshipMinimum)
SYNC_ARCHIVE_VAR(int, m_iMinorFriendshipDecayMod)
SYNC_ARCHIVE_VAR(int, m_iMinorScienceAlliesCount)
SYNC_ARCHIVE_VAR(int, m_iMinorResourceBonusCount)
SYNC_ARCHIVE_VAR(int, m_iAbleToAnnexCityStatesCount)
SYNC_ARCHIVE_VAR(int, m_iOnlyTradeSameIdeology)
SYNC_ARCHIVE_VAR(int, m_iSupplyFreeUnits)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abActiveContract)
SYNC_ARCHIVE_VAR(int, m_iJFDReformCooldownRate)
SYNC_ARCHIVE_VAR(int, m_iJFDGovernmentCooldownRate)
SYNC_ARCHIVE_VAR(CvString, m_strJFDPoliticKey)
SYNC_ARCHIVE_VAR(CvString, m_strJFDLegislatureName)
SYNC_ARCHIVE_VAR(int, m_iJFDPoliticLeader)
SYNC_ARCHIVE_VAR(int, m_iJFDSovereignty)
SYNC_ARCHIVE_VAR(int, m_iJFDGovernment)
SYNC_ARCHIVE_VAR(int, m_iJFDReformCooldown)
SYNC_ARCHIVE_VAR(int, m_iJFDGovernmentCooldown)
SYNC_ARCHIVE_VAR(int, m_iJFDPiety)
SYNC_ARCHIVE_VAR(int, m_iJFDPietyRate)
SYNC_ARCHIVE_VAR(int, m_iJFDConversionTurn)
SYNC_ARCHIVE_VAR(bool, m_bJFDSecularized)
SYNC_ARCHIVE_VAR(CvString, m_strJFDCurrencyName)
SYNC_ARCHIVE_VAR(int, m_iJFDProsperity)
SYNC_ARCHIVE_VAR(int, m_iJFDCurrency)
SYNC_ARCHIVE_VAR(int, m_iGoldenAgeTourism)
SYNC_ARCHIVE_VAR(int, m_iExtraCultureandScienceTradeRoutes)
SYNC_ARCHIVE_VAR(int, m_iArchaeologicalDigTourism)
SYNC_ARCHIVE_VAR(int, m_iUpgradeCSVassalTerritory)
SYNC_ARCHIVE_VAR(int, m_iRazingSpeedBonus)
SYNC_ARCHIVE_VAR(int, m_iNoPartisans)
SYNC_ARCHIVE_VAR(int, m_iSpawnCooldown)
SYNC_ARCHIVE_VAR(bool, m_bTradeRoutesInvulnerable)
SYNC_ARCHIVE_VAR(int, m_iTRSpeedBoost)
SYNC_ARCHIVE_VAR(int, m_iVotesPerGPT)
SYNC_ARCHIVE_VAR(int, m_iTRVisionBoost)
SYNC_ARCHIVE_VAR(int, m_iEventTourism)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiGlobalTourismAlreadyReceived)
SYNC_ARCHIVE_VAR(int, m_iEventTourismCS)
SYNC_ARCHIVE_VAR(int, m_iNumHistoricEvent)
SYNC_ARCHIVE_VAR(int, m_iSingleVotes)
SYNC_ARCHIVE_VAR(int, m_iMonopolyModFlat)
SYNC_ARCHIVE_VAR(int, m_iMonopolyModPercent)
SYNC_ARCHIVE_VAR(int, m_iCachedValueOfPeaceWithHuman)
SYNC_ARCHIVE_VAR(int, m_iFaithPurchaseCooldown)
SYNC_ARCHIVE_VAR(int, m_iCSAllies)
SYNC_ARCHIVE_VAR(int, m_iCSFriends)
SYNC_ARCHIVE_VAR(int, m_iCitiesNeedingTerrainImprovements)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiBestMilitaryCombatClassCity)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiBestMilitaryDomainCity)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiEventChoiceDuration)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiEventIncrement)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiEventCooldown)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abEventActive)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abEventChoiceActive)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abEventChoiceFired)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abEventFired)
SYNC_ARCHIVE_VAR(int, m_iPlayerEventCooldown)
SYNC_ARCHIVE_VAR(std::vector<FeatureTypes>, m_ownedNaturalWonders)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiUnitClassProductionModifiers)
SYNC_ARCHIVE_VAR(int, m_iExtraSupplyPerPopulation)
SYNC_ARCHIVE_VAR(int, m_iCitySupplyFlatGlobal)
SYNC_ARCHIVE_VAR(int, m_iUnitSupplyFromExpendedGP)
SYNC_ARCHIVE_VAR(int, m_iMissionaryExtraStrength)
SYNC_ARCHIVE_VAR(int, m_iFreeSpecialist)
SYNC_ARCHIVE_VAR(int, m_iCultureBombTimer)
SYNC_ARCHIVE_VAR(int, m_iConversionTimer)
SYNC_ARCHIVE_VAR(int, m_iCapitalCityID)
SYNC_ARCHIVE_VAR(int, m_iCitiesLost)
SYNC_ARCHIVE_VAR(int, m_iMilitaryRating)
SYNC_ARCHIVE_VAR(int, m_iMilitaryMight)
SYNC_ARCHIVE_VAR(int, m_iEconomicMight)
SYNC_ARCHIVE_VAR(int, m_iProductionMight)
SYNC_ARCHIVE_VAR(int, m_iTurnSliceMightRecomputed)
SYNC_ARCHIVE_VAR(int, m_iNewCityExtraPopulation)
SYNC_ARCHIVE_VAR(int, m_iFreeFoodBox)
SYNC_ARCHIVE_VAR(int, m_iScenarioScore1)
SYNC_ARCHIVE_VAR(int, m_iScenarioScore2)
SYNC_ARCHIVE_VAR(int, m_iScenarioScore3)
SYNC_ARCHIVE_VAR(int, m_iScenarioScore4)
SYNC_ARCHIVE_VAR(int, m_iScoreFromFutureTech)
SYNC_ARCHIVE_VAR(int, m_iTurnLastAttackedMinorCiv)
SYNC_ARCHIVE_VAR(int, m_iCombatExperienceTimes100)
SYNC_ARCHIVE_VAR(int, m_iLifetimeCombatExperienceTimes100)
SYNC_ARCHIVE_VAR(int, m_iNavalCombatExperienceTimes100)
SYNC_ARCHIVE_VAR(int, m_iBorderObstacleCount)
SYNC_ARCHIVE_VAR(int, m_iBorderGainlessPillageCount)
SYNC_ARCHIVE_VAR(int, m_iPopRushHurryCount)
SYNC_ARCHIVE_VAR(int, m_iTotalImprovementsBuilt)
SYNC_ARCHIVE_VAR(int, m_iCostNextPolicy)
SYNC_ARCHIVE_VAR(int, m_iNumBuilders)
SYNC_ARCHIVE_VAR(int, m_iMaxNumBuilders)
SYNC_ARCHIVE_VAR(int, m_iCityStrengthMod)
SYNC_ARCHIVE_VAR(int, m_iCityGrowthMod)
SYNC_ARCHIVE_VAR(int, m_iCapitalGrowthMod)
SYNC_ARCHIVE_VAR(int, m_iNumPlotsBought)
SYNC_ARCHIVE_VAR(int, m_iPlotGoldCostMod)
SYNC_ARCHIVE_VAR(int, m_iCityWorkingChange)
SYNC_ARCHIVE_VAR(int, m_iCityAutomatonWorkersChange)
SYNC_ARCHIVE_VAR(int, m_iCachedGoldRate)
SYNC_ARCHIVE_VAR(int, m_iBorderGrowthRateIncreaseGlobal)
SYNC_ARCHIVE_VAR(int, m_iPlotCultureCostModifier)
SYNC_ARCHIVE_VAR(int, m_iPlotCultureExponentModifier)
SYNC_ARCHIVE_VAR(int, m_iNumCitiesPolicyCostDiscount)
SYNC_ARCHIVE_VAR(int, m_iGarrisonedCityRangeStrikeModifier)
SYNC_ARCHIVE_VAR(int, m_iGarrisonFreeMaintenanceCount)
SYNC_ARCHIVE_VAR(int, m_iNumCitiesFreeCultureBuilding)
SYNC_ARCHIVE_VAR(int, m_iNumCitiesFreeFoodBuilding)
SYNC_ARCHIVE_VAR(int, m_iUnitPurchaseCostModifier)
SYNC_ARCHIVE_VAR(int, m_iAllFeatureProduction)
SYNC_ARCHIVE_VAR(int, m_iCityDistanceHighwaterMark)
SYNC_ARCHIVE_VAR(int, m_iOriginalCapitalX)
SYNC_ARCHIVE_VAR(int, m_iOriginalCapitalY)
SYNC_ARCHIVE_VAR(int, m_iHolyCityX)
SYNC_ARCHIVE_VAR(int, m_iHolyCityY)
SYNC_ARCHIVE_VAR(int, m_iNumWonders)
SYNC_ARCHIVE_VAR(int, m_iNumPolicies)
SYNC_ARCHIVE_VAR(int, m_iNumGreatPeople)
SYNC_ARCHIVE_VAR(int, m_iCityConnectionHappiness)
SYNC_ARCHIVE_VAR(int, m_iHolyCityID)
SYNC_ARCHIVE_VAR(int, m_iTurnsSinceSettledLastCity)
SYNC_ARCHIVE_VAR(int, m_iNumNaturalWondersDiscoveredInArea)
SYNC_ARCHIVE_VAR(int, m_iStrategicResourceMod)
SYNC_ARCHIVE_VAR(int, m_iSpecialistCultureChange)
SYNC_ARCHIVE_VAR(int, m_iGreatPeopleSpawnCounter)
SYNC_ARCHIVE_VAR(int, m_iFreeTechCount)
SYNC_ARCHIVE_VAR(int, m_iMedianTechPercentage)
SYNC_ARCHIVE_VAR(int, m_iNumFreePolicies)
SYNC_ARCHIVE_VAR(int, m_iNumFreePoliciesEver)
SYNC_ARCHIVE_VAR(int, m_iNumFreeTenets)
SYNC_ARCHIVE_VAR(uint, m_uiStartTime)
SYNC_ARCHIVE_VAR(bool, m_bHasUUPeriod)
SYNC_ARCHIVE_VAR(bool, m_bNoNewWars)
SYNC_ARCHIVE_VAR(bool, m_bTerribleShapeForWar)
SYNC_ARCHIVE_VAR(bool, m_bHasBetrayedMinorCiv)
SYNC_ARCHIVE_VAR(bool, m_bAlive)
SYNC_ARCHIVE_VAR(bool, m_bEverAlive)
SYNC_ARCHIVE_VAR(bool, m_bPotentiallyAlive)
SYNC_ARCHIVE_VAR(bool, m_bTurnActive)
SYNC_ARCHIVE_VAR(bool, m_bAutoMoves)
SYNC_ARCHIVE_VAR(bool, m_bEndTurn)
SYNC_ARCHIVE_VAR(bool, m_bDynamicTurnsSimultMode)
SYNC_ARCHIVE_VAR(bool, m_bPbemNewTurn)
SYNC_ARCHIVE_VAR(bool, m_bExtendedGame)
SYNC_ARCHIVE_VAR(bool, m_bFoundedFirstCity)
SYNC_ARCHIVE_VAR(int, m_iNumCitiesFounded)
SYNC_ARCHIVE_VAR(bool, m_bStrike)
SYNC_ARCHIVE_VAR(bool, m_bCramped)
SYNC_ARCHIVE_VAR(bool, m_bLostCapital)
SYNC_ARCHIVE_VAR(PlayerTypes, m_eConqueror)
SYNC_ARCHIVE_VAR(bool, m_bLostHolyCity)
SYNC_ARCHIVE_VAR(PlayerTypes, m_eHolyCityConqueror)
SYNC_ARCHIVE_VAR(bool, m_bHasAdoptedStateReligion)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiCityYieldChange)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiCoastalCityYieldChange)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiCapitalYieldChange)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiCapitalYieldPerPopChange)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiCapitalYieldPerPopChangeEmpire)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiSeaPlotYield)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldRateModifier)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiJFDPoliticPercent)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromMinors)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiResourceFromCSAlliances)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiResourceShortageValue)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromBirth)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromBirthCapital)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromDeath)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromPillage)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromVictory)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromConstruction)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromWorldWonderConstruction)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromTech)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromBorderGrowth)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldGPExpend)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiConquerorYield)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiFounderYield)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiArtifactYieldBonus)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiArtYieldBonus)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiMusicYieldBonus)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiLitYieldBonus)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiFilmYieldBonus)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiRelicYieldBonus)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiReligionYieldRateModifier)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiGoldenAgeYieldMod)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiNumAnnexedCityStates)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiIncomingEspionageYields)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiOutgoingEspionageYields)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromNonSpecialistCitizens)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldModifierFromGreatWorks)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldModifierFromActiveSpies)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiYieldFromDelegateCount)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiBuildingClassCulture)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiDomainFreeExperiencePerGreatWorkGlobal)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiCityYieldModFromMonopoly)
SYNC_ARCHIVE_VAR(std::vector<UnitAITypes>, m_neededUnitAITypes)
SYNC_ARCHIVE_VAR(bool, m_bAllowsProductionTradeRoutesGlobal)
SYNC_ARCHIVE_VAR(bool, m_bAllowsFoodTradeRoutesGlobal)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiCapitalYieldRateModifier)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiExtraYieldThreshold)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiSpecialistExtraYield)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiPlayerNumTurnsAtPeace)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiPlayerNumTurnsAtWar)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiPlayerNumTurnsSinceCityCapture)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiWarValueLost)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiWarDamageValue)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiNumUnitsBuilt)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiProximityToPlayer)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiResearchAgreementCounter)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiSiphonLuxuryCount)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiGreatWorkYieldChange)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiTourismBonusTurnsPlayer)
SYNC_ARCHIVE_VAR(SYNC_ARCHIVE_VAR_TYPE(std::vector< std::pair<uint, int> >), m_aOptions)
SYNC_ARCHIVE_VAR(CvString, m_strReligionKey)
SYNC_ARCHIVE_VAR(CvString, m_strScriptData)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiNumResourceUsed)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiNumResourceTotal)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiResourceGiftedToMinors)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiResourceExport)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiResourceImportFromMajor)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiResourceFromMinors)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiResourcesSiphoned)
SYNC_ARCHIVE_VAR(std::vector<byte>, m_aiNumResourceFromGP)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiImprovementCount)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiImprovementBuiltCount)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiTotalImprovementsBuilt)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiBuildingChainSteps)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiFreeBuildingCount)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiFreePromotionCount)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiUnitCombatProductionModifiers)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiUnitCombatFreeExperiences)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiUnitClassCount)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiUnitClassMaking)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiBuildingClassCount)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiBuildingClassMaking)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiProjectMaking)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiHurryCount)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiHurryModifier)
SYNC_ARCHIVE_VAR(bool, m_bVassalLevy)
SYNC_ARCHIVE_VAR(int, m_iVassalGoldMaintenanceMod)
SYNC_ARCHIVE_VAR(std::vector<int>, m_paiNumCitiesFreeChosenBuilding)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_pabFreeChosenBuildingNewCity)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_pabAllCityFreeBuilding)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_pabNewFoundCityFreeUnit)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_pabNewFoundCityFreeBuilding)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_pabLoyalMember)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_pabHasGlobalMonopoly)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_pabHasStrategicMonopoly)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_pabGetsScienceFromPlayer)
SYNC_ARCHIVE_VAR(SYNC_ARCHIVE_VAR_TYPE(std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > >), m_ppaaiSpecialistExtraYield)
SYNC_ARCHIVE_VAR(SYNC_ARCHIVE_VAR_TYPE(std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > >), m_ppiBuildingClassYieldChange)
SYNC_ARCHIVE_VAR(SYNC_ARCHIVE_VAR_TYPE(std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > >), m_ppaaiImprovementYieldChange)
SYNC_ARCHIVE_VAR(bool, m_bEverPoppedGoody)
SYNC_ARCHIVE_VAR(bool, m_bEverTrainedBuilder)
SYNC_ARCHIVE_VAR(int, m_iPreviousBestSettlePlot)
SYNC_ARCHIVE_VAR(int, m_iFoundValueOfCapital)
SYNC_ARCHIVE_VAR(int, m_iNumFreeGreatPeople)
SYNC_ARCHIVE_VAR(int, m_iNumMayaBoosts)
SYNC_ARCHIVE_VAR(int, m_iNumFaithGreatPeople)
SYNC_ARCHIVE_VAR(int, m_iNumArchaeologyChoices)
SYNC_ARCHIVE_VAR(int, m_iFaithPurchaseIndex)
SYNC_ARCHIVE_VAR(int, m_iFractionOriginalCapitalsUnderControl)
SYNC_ARCHIVE_VAR(int, m_iAvgUnitExp100)
SYNC_ARCHIVE_VAR(int, m_iNumMilitarySeaUnits)
SYNC_ARCHIVE_VAR(int, m_iNumMilitaryAirUnits)
SYNC_ARCHIVE_VAR(int, m_iNumMilitaryLandUnits)
SYNC_ARCHIVE_VAR(int, m_iMilitarySeaMight)
SYNC_ARCHIVE_VAR(int, m_iMilitaryAirMight)
SYNC_ARCHIVE_VAR(int, m_iMilitaryLandMight)
SYNC_ARCHIVE_END()


FDataStream& operator>>(FDataStream& loadFrom, SPlayerActiveEspionageEvent& writeTo);
FDataStream& operator<<(FDataStream& saveTo, const SPlayerActiveEspionageEvent& readFrom);

#endif
