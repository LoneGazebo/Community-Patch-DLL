/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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

typedef std::list<CvPopupInfo*> CvPopupQueue;

typedef std::vector< std::pair<UnitCombatTypes, PromotionTypes> > UnitCombatPromotionArray;
typedef std::vector< std::pair<UnitClassTypes, PromotionTypes> > UnitClassPromotionArray;
typedef std::vector< std::pair<CivilizationTypes, LeaderHeadTypes> > CivLeaderArray;
typedef FStaticVector<int, 152* 96, true, c_eCiv5GameplayDLL, 0> CvPlotsVector; // allocate the size of HUGE Terra world just in case (this is max that we ship with)

class CvPlayer
{
	friend class CvPlayerPolicies;

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

#if defined(MOD_API_EXTENSIONS)
	CvCity* initCity(int iX, int iY, bool bBumpUnits = true, bool bInitialFounding = true, ReligionTypes eInitialReligion = NO_RELIGION, const char* szName = NULL);
#else
	CvCity* initCity(int iX, int iY, bool bBumpUnits = true, bool bInitialFounding = true);
#endif
#if defined(MOD_GLOBAL_VENICE_KEEPS_RESOURCES)
	void acquireCity(CvCity* pCity, bool bConquest, bool bGift, bool bVenice = false);
#else
	void acquireCity(CvCity* pCity, bool bConquest, bool bGift);
#endif
	void killCities();
	CvString getNewCityName() const;
	CvString GetBorrowedCityName(CivilizationTypes eCivToBorrowFrom) const;
	void getCivilizationCityName(CvString& szBuffer, CivilizationTypes eCivilization) const;
	bool isCityNameValid(CvString& szName, bool bTestDestroyed = true) const;

#if defined(MOD_GLOBAL_CITY_WORKING)
	int getBuyPlotDistance() const;
	int getWorkPlotDistance() const;
	int GetNumWorkablePlots() const;
#endif

	void DoLiberatePlayer(PlayerTypes ePlayer, int iOldCityID);
	bool CanLiberatePlayer(PlayerTypes ePlayer);
	bool CanLiberatePlayerCity(PlayerTypes ePlayer);

	CvUnit* initUnit(UnitTypes eUnit, int iX, int iY, UnitAITypes eUnitAI = NO_UNITAI, DirectionTypes eFacingDirection = NO_DIRECTION, bool bNoMove=false, bool bSetupGraphical=true, int iMapLayer = 0, int iNumGoodyHutsPopped = 0);
	CvUnit* initUnitWithNameOffset(UnitTypes eUnit, int nameOffset, int iX, int iY, UnitAITypes eUnitAI = NO_UNITAI, DirectionTypes eFacingDirection = NO_DIRECTION, bool bNoMove = false, bool bSetupGraphical = true, int iMapLayer = 0, int iNumGoodyHutsPopped = 0);

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

	void InitDangerPlots();
	void UpdateDangerPlots();
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
	void RespositionInvalidUnits();

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
	const UnitHandle getBusyUnit() const;
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

	int countCityFeatures(FeatureTypes eFeature) const;
	int countNumBuildings(BuildingTypes eBuilding) const;
	//int countNumCitiesConnectedToCapital() const;

	int countCitiesFeatureSurrounded() const;

	bool IsCityConnectedToCity(CvCity* pCity1, CvCity* pCity2, RouteTypes eRestrictRouteType = NO_ROUTE, bool bIgnoreHarbors = false);
	bool IsCapitalConnectedToPlayer(PlayerTypes ePlayer, RouteTypes eRestrictRouteType = NO_ROUTE);
	bool IsCapitalConnectedToCity(CvCity* pCity, RouteTypes eRestrictRouteType = NO_ROUTE);
#if defined(MOD_API_EXTENSIONS)
	bool IsPlotConnectedToPlot(CvPlot* pFromPlot, CvPlot* pToPlot, RouteTypes eRestrictRoute = NO_ROUTE, bool bIgnoreHarbors = false);
#endif

	void findNewCapital();

	bool canRaze(CvCity* pCity, bool bIgnoreCapitals = false) const;
	void raze(CvCity* pCity);
	void unraze(CvCity* pCity);
	void disband(CvCity* pCity);

	bool canReceiveGoody(CvPlot* pPlot, GoodyTypes eGoody, CvUnit* pUnit) const;
	void receiveGoody(CvPlot* pPlot, GoodyTypes eGoody, CvUnit* pUnit);
	void doGoody(CvPlot* pPlot, CvUnit* pUnit);

	void AwardFreeBuildings(CvCity* pCity); // slewis - broken out so that Venice can get free buildings when they purchase something
	bool canFound(int iX, int iY, bool bTestVisible = false) const;

#if defined(MOD_GLOBAL_RELIGIOUS_SETTLERS)
	void found(int iX, int iY, ReligionTypes eReligion = NO_RELIGION);
#else
	void found(int iX, int iY);
#endif
#if defined(MOD_DIPLOMACY_CITYSTATES)
	void cityBoost(int iX, int iY, CvUnitEntry* pkUnitEntry, int iExtraPlots, int iPopChange, int iFoodPercent);
#endif

	bool canTrain(UnitTypes eUnit, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, bool bIgnoreUniqueUnitStatus = false, CvString* toolTipSink = NULL) const;
	bool canConstruct(BuildingTypes eBuilding, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, CvString* toolTipSink = NULL) const;
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

	bool canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestEra = false, bool bTestVisible = false, bool bTestGold = true, bool bTestPlotOwner = true) const;
	bool IsBuildBlockedByFeature(BuildTypes eBuild, FeatureTypes eFeature) const;
	int getBuildCost(const CvPlot* pPlot, BuildTypes eBuild) const;
	RouteTypes getBestRoute(CvPlot* pPlot = NULL) const;
	int getImprovementUpgradeRate() const;

	int GetAllFeatureProduction() const;
	void ChangeAllFeatureProduction(int iChange);

	int calculateTotalYield(YieldTypes eYield) const;

	int GetUnitProductionMaintenanceMod() const;
	void UpdateUnitProductionMaintenanceMod();
	int calculateUnitProductionMaintenanceMod() const;

	int GetNumUnitsSupplied() const;
	int GetNumUnitsSuppliedByHandicap() const;
	int GetNumUnitsSuppliedByCities() const;
	int GetNumUnitsSuppliedByPopulation() const;

	int GetNumUnitsOutOfSupply() const;

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

	int GetCityYieldChange(YieldTypes eYield) const;
	void ChangeCityYieldChange(YieldTypes eYield, int iChange);

	int GetCoastalCityYieldChange(YieldTypes eYield) const;
	void ChangeCoastalCityYieldChange(YieldTypes eYield, int iChange);

	int GetCapitalYieldChange(YieldTypes eYield) const;
	void ChangeCapitalYieldChange(YieldTypes eYield, int iChange);

	int GetCapitalYieldPerPopChange(YieldTypes eYield) const;
	void ChangeCapitalYieldPerPopChange(YieldTypes eYield, int iChange);

	int GetGreatWorkYieldChange(YieldTypes eYield) const;
	void ChangeGreatWorkYieldChange(YieldTypes eYield, int iChange);

	CvPlot* getStartingPlot() const;
	void setStartingPlot(CvPlot* pNewValue);

	int getTotalPopulation() const;
	int getAveragePopulation() const;
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
#if defined(MOD_BALANCE_CORE_HAPPINESS_NATIONAL)
	int GetTotalJONSCulturePerTurnforUI() const;
#endif

	int GetJONSCulturePerTurnFromCities() const;

	int GetJONSCulturePerTurnFromExcessHappiness() const;
	int GetJONSCulturePerTurnFromTraits() const;

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

	int GetCulturePerWonder() const;
	void ChangeCulturePerWonder(int iChange);

	int GetCultureWonderMultiplier() const;
	void ChangeCultureWonderMultiplier(int iChange);

	int GetCulturePerTechResearched() const;
	void ChangeCulturePerTechResearched(int iChange);

	int GetSpecialistCultureChange() const;
	void ChangeSpecialistCultureChange(int iChange);

	int GetCultureYieldFromPreviousTurns(int iGameTurn, int iNumPreviousTurnsToCount);

	int GetNumCitiesFreeCultureBuilding() const;
	void ChangeNumCitiesFreeCultureBuilding(int iChange);
	int GetNumCitiesFreeFoodBuilding() const;
	void ChangeNumCitiesFreeFoodBuilding(int iChange);
#if defined(MOD_BALANCE_CORE)
	int GetNumCitiesFreeChosenBuilding(BuildingClassTypes eBuildingClass) const;
	void ChangeNumCitiesFreeChosenBuilding(BuildingClassTypes eBuildingClass, int iChange);
#endif

	void DoYieldsFromKill(UnitTypes eAttackingUnitType, UnitTypes eKilledUnitType, int iX, int iY, bool bWasBarbarian, int iExistingDelay);
	void DoYieldBonusFromKill(YieldTypes eYield, UnitTypes eAttackingUnitType, UnitTypes eKilledUnitType, int iX, int iY, bool bWasBarbarian, int &iNumBonuses);
	void DoUnresearchedTechBonusFromKill(UnitTypes eKilledUnitType, int iX, int iY, int &iNumBonuses);
	void ReportYieldFromKill(YieldTypes eYield, int iValue, int iX, int iY, int iDelay);

	void DoTechFromCityConquer(CvCity* pConqueredCity);

	// Faith
	int GetTotalFaithPerTurn() const;
#if defined(MOD_BALANCE_CORE_HAPPINESS_NATIONAL)
	int GetTotalFaithPerTurnForUI() const;
#endif
	int GetFaithPerTurnFromCities() const;
	int GetFaithPerTurnFromMinorCivs() const;
	int GetFaithPerTurnFromMinor(PlayerTypes eMinor) const;
	int GetFaithPerTurnFromReligion() const;
	int GetFaith() const;
	void SetFaith(int iNewValue);
	void ChangeFaith(int iChange);
	int GetFaithEverGenerated() const;
	void SetFaithEverGenerated(int iNewValue);
	void ChangeFaithEverGenerated(int iChange);

	// Happiness

	void DoUpdateHappiness();
	int GetHappiness() const;
	void SetHappiness(int iNewValue);
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	void SetUnhappiness(int iNewValue);
	int GetSetUnhappiness() const;
	void CalculateHappiness();
#endif
#if defined(MOD_BALANCE_CORE_HAPPINESS_NATIONAL)
	//LUA Functions
	int CalculateUnhappinessTooltip(YieldTypes eYield) const;
#endif
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
	PlayerTypes GetMostUnhappyCityRecipient();

	int GetHappinessFromPolicies() const;
	int GetHappinessFromCities() const;
	int GetHappinessFromBuildings() const;

	int GetExtraHappinessPerCity() const;
	void ChangeExtraHappinessPerCity(int iChange);
	int GetExtraHappinessPerXPolicies() const;
	void ChangeExtraHappinessPerXPolicies(int iChange);

	int GetHappinessFromResources() const;
	int GetHappinessFromResourceVariety() const;
	int GetHappinessFromReligion();
	int GetHappinessFromNaturalWonders() const;

	int GetExtraHappinessPerLuxury() const;
	void ChangeExtraHappinessPerLuxury(int iChange);
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	int getGlobalAverage(YieldTypes eYield) const;
#endif
#if defined(MOD_BALANCE_CORE_HAPPINESS_LUXURY)
	int getPopNeededForLux() const;
	int GetBaseLuxuryHappiness() const;
	void SetBaseLuxuryHappiness(int iValue);
#endif
#if defined(MOD_BALANCE_CORE)
	int getCurrentTotalPop() const;
#endif

	int GetHappinessFromLuxury(ResourceTypes eResource) const;

	int GetUnhappiness(CvCity* pAssumeCityAnnexed = NULL, CvCity* pAssumeCityPuppeted = NULL) const;

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
	int getUnhappinessFromCityCulture() const;
	int getUnhappinessFromCityDefense() const;
	int getUnhappinessFromCityGold() const;
	int getUnhappinessFromCityConnection() const;
	int getUnhappinessFromCityPillaged() const;
	int getUnhappinessFromCityStarving() const;
	int getUnhappinessFromCityMinority() const;
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

	void setHasPolicy(PolicyTypes eIndex, bool bNewValue);
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
	void changeGoldenAgeTurns(int iChange);
	int getGoldenAgeLength() const;

	int getNumUnitGoldenAges() const;
	void changeNumUnitGoldenAges(int iChange);

	int getStrikeTurns() const;
	void changeStrikeTurns(int iChange);

	int getGoldenAgeModifier() const;
	void changeGoldenAgeModifier(int iChange);

	// Great People Stuff
	void createGreatGeneral(UnitTypes eGreatPersonUnit, int iX, int iY);
	void createGreatAdmiral(UnitTypes eGreatPersonUnit, int iX, int iY);

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
	int getDiplomatsFromFaith() const;
	void incrementDiplomatsFromFaith();
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
	void changeGreatPeopleRateModFromBldgs(int ichange);
	void changeGreatGeneralRateModFromBldgs(int ichange);
	void recomputeGreatPeopleModifiers();
	int GetGreatPeopleRateModFromFriendships() const;

	int GetGreatScientistBeakerMod() const;
	void SetGreatScientistBeakerMod(int iValue);
	void ChangeGreatScientistBeakerMod(int iChange);

	int GetGreatGeneralCombatBonus() const;
	void SetGreatGeneralCombatBonus(int iValue);

	// Unit Killed in Combat
	void DoUnitKilledCombat(PlayerTypes eKilledPlayer, UnitTypes eUnit);

	// Great People Expenditure
#if defined (MOD_EVENTS_GREAT_PEOPLE)
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

	void DoSpawnGreatPerson(PlayerTypes eMinor);
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

	int GetRouteGoldMaintenanceMod() const;
	void ChangeRouteGoldMaintenanceMod(int iChange);

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
	void changeNumMilitaryUnits(int iChange);

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

	int getMilitaryFoodProductionCount() const;
	bool isMilitaryFoodProduction() const;
	void changeMilitaryFoodProductionCount(int iChange);

	int GetGoldenAgeCultureBonusDisabledCount() const;
	bool IsGoldenAgeCultureBonusDisabled() const;
	void ChangeGoldenAgeCultureBonusDisabledCount(int iChange);

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
#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
	int GetPovertyUnhappinessMod() const;
	void ChangePovertyUnhappinessMod(int iChange);
	int GetDefenseUnhappinessMod() const;
	void ChangeDefenseUnhappinessMod(int iChange);
	int	GetIlliteracyUnhappinessMod() const;
	void ChangeIlliteracyUnhappinessMod(int iChange);
	int GetMinorityUnhappinessMod() const;
	void ChangeMinorityUnhappinessMod(int iChange);

	int GetPovertyUnhappinessModCapital() const;
	void ChangePovertyUnhappinessModCapital(int iChange);
	int GetDefenseUnhappinessModCapital() const;
	void ChangeDefenseUnhappinessModCapital(int iChange);
	int	GetIlliteracyUnhappinessModCapital() const;
	void ChangeIlliteracyUnhappinessModCapital(int iChange);
	int GetMinorityUnhappinessModCapital() const;
	void ChangeMinorityUnhappinessModCapital(int iChange);
	int GetPuppetUnhappinessMod() const;
	void ChangePuppetUnhappinessMod(int iChange);
	int GetNoUnhappfromXSpecialists() const;
	void ChangeNoUnhappfromXSpecialists(int iChange);
	int GetNoUnhappfromXSpecialistsCapital() const;
	void ChangeNoUnhappfromXSpecialistsCapital(int iChange);
#endif

	int getCultureBombTimer() const;
	void setCultureBombTimer(int iNewValue);
	void changeCultureBombTimer(int iChange);

	int getConversionTimer() const;
	void setConversionTimer(int iNewValue);
	void changeConversionTimer(int iChange);

	CvCity* getCapitalCity();
	const CvCity* getCapitalCity() const;
	void setCapitalCity(CvCity* pNewCapitalCity);

	int GetOriginalCapitalX() const;
	int GetOriginalCapitalY() const;

	bool IsHasLostCapital() const;
	void SetHasLostCapital(bool bValue, PlayerTypes eConqueror);
#if defined(MOD_GLOBAL_NO_CONQUERED_SPACESHIPS)
	void disassembleSpaceship();
#endif
	PlayerTypes GetCapitalConqueror() const;

	int getCitiesLost() const;
	void changeCitiesLost(int iChange);

	int getPower() const;
	int GetMilitaryMight() const;
	int GetEconomicMight() const;
	int calculateMilitaryMight() const;
	int calculateEconomicMight() const;
	int calculateProductionMight() const;

	int getCombatExperience() const;
#if defined(MOD_GLOBAL_LOCAL_GENERALS)
	void setCombatExperience(int iExperience, CvUnit* pFromUnit = NULL);
	void changeCombatExperience(int iChange, CvUnit* pFromUnit = NULL);
#else
	void setCombatExperience(int iExperience);
	void changeCombatExperience(int iChange);
#endif
	int getLifetimeCombatExperience() const;
	int getNavalCombatExperience() const;
#if defined(MOD_GLOBAL_LOCAL_GENERALS)
	void setNavalCombatExperience(int iExperience, CvUnit* pFromUnit = NULL);
	void changeNavalCombatExperience(int iChange, CvUnit* pFromUnit = NULL);
#else
	void setNavalCombatExperience(int iExperience);
	void changeNavalCombatExperience(int iChange);
#endif

	int getBorderObstacleCount() const;
	bool isBorderObstacle() const;
	void changeBorderObstacleCount(int iChange);

	int getNetID() const;
	void setNetID(int iNetID);
	bool isConnected() const;
	void sendTurnReminder();

	uint getStartTime() const;
	void setStartTime(uint uiStartTime);
	uint getTotalTimePlayed() const;

	bool isMinorCiv() const;
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

	CvHandicapInfo& getHandicapInfo() const;
	HandicapTypes getHandicapType() const;

	CvCivilizationInfo& getCivilizationInfo() const;
	CivilizationTypes getCivilizationType() const;

	CvLeaderHeadInfo& getLeaderInfo() const;
	LeaderHeadTypes getLeaderType() const;

	LeaderHeadTypes getPersonalityType() const;
	void setPersonalityType(LeaderHeadTypes eNewValue);

	EraTypes GetCurrentEra() const;

	PlayerTypes GetID() const
	{
		return m_eID;
	}

	static TeamTypes getTeam(PlayerTypes ePlayerID)
	{
		return CvPreGame::teamType(ePlayerID);
	}

	TeamTypes getTeam() const
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

	int getCapitalYieldRateModifier(YieldTypes eIndex) const;
	void changeCapitalYieldRateModifier(YieldTypes eIndex, int iChange);

	int getExtraYieldThreshold(YieldTypes eIndex) const;
	void updateExtraYieldThreshold(YieldTypes eIndex);

	// Science

	int GetScience() const;
	int GetScienceTimes100() const;
#if defined(MOD_BALANCE_CORE_HAPPINESS_NATIONAL)
	int GetScienceTimes100ForUI() const;
#endif

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

	void DoDistanceGift(PlayerTypes eFromPlayer, CvUnit* pUnit);
	void AddIncomingUnit(PlayerTypes eFromPlayer, CvUnit* pUnit);

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
	int getNumResourceTotal(ResourceTypes eIndex, bool bIncludeImport = true) const;
	void changeNumResourceTotal(ResourceTypes eIndex, int iChange, bool bIgnoreResourceWarning = false);

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
	void changeProjectMaking(ProjectTypes eIndex, int iChange);

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

	int getImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const;
	void changeImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange);

	CvUnitCycler& GetUnitCycler() { return m_UnitCycle; };

	bool removeFromArmy(int iArmyID, int iID);
	bool removeFromArmy(int iID);

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
	int getNumCities() const;
	CvCity* getCity(int iID);
	const CvCity* getCity(int iID) const;
	CvCity* addCity();
	void deleteCity(int iID);
	CvCity* GetFirstCityWithBuildingClass(BuildingClassTypes eBuildingClass);

	// unit iteration
	int getNumUnits() const;
	const CvUnit* firstUnit(int* pIterIdx, bool bRev=false) const;
	const CvUnit* nextUnit(int* pIterIdx, bool bRev=false) const;
	const CvUnit* getUnit(int iID) const;
	CvUnit* firstUnit(int* pIterIdx, bool bRev=false);
	CvUnit* nextUnit(int* pIterIdx, bool bRev=false);
	CvUnit* getUnit(int iID);
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
	CvAIOperation* addAIOperation(int iOperationType, PlayerTypes eEnemy=NO_PLAYER, int iArea=-1, CvCity* pTarget=NULL, CvCity* pMuster=NULL);
	void deleteAIOperation(int iID);
	bool haveAIOperationOfType(int iOperationType, int* piID=NULL, PlayerTypes eTargetPlayer = NO_PLAYER, CvPlot* pTargetPlot=NULL);
	int numOperationsOfType(int iOperationType);
	bool IsCityAlreadyTargeted(CvCity* pCity, DomainTypes eDomain=NO_DOMAIN, int iPercentToTarget=100, int iIgnoreOperationID=-1) const;
	bool IsPlotTargetedForCity(CvPlot *pPlot) const;
	void GatherPerTurnReplayStats(int iGameTurn);
	unsigned int getNumReplayDataSets() const;
	const char* getReplayDataSetName(unsigned int uiDataSet) const;
	unsigned int getReplayDataSetIndex(const char* szDataSetName);
	int getReplayDataValue(unsigned int uiDataSet, unsigned int uiTurn) const;
	void setReplayDataValue(unsigned int uiDataSet, unsigned int uiTurn, int iValue);
	TurnData getReplayDataHistory(unsigned int uiDataSet) const;

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

	void InitPlots();  // this needs to be called after the map is inited. It makes the list of how many plots the player controls
	void UpdatePlots();  // Modifies the list of plots and sets which ones the player owns
	void AddAPlot(CvPlot* pPlot); // adds a plot at the end of the list
	CvPlotsVector& GetPlots();  // gets the list of plots the player owns
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

	int GetPlotDanger(CvPlot& Plot) const;
	bool IsPlotUnderImmediateThreat(CvPlot& Plot) const;
	CvCity* GetClosestFriendlyCity(CvPlot& plot, int iSearchRadius);

	int GetNumPuppetCities() const;
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	int GetNumCapitalCities() const;
#endif
	int GetMaxEffectiveCities(bool bIncludePuppets = false);

	int GetNumNaturalWondersDiscoveredInArea() const;
	void SetNumNaturalWondersDiscoveredInArea(int iValue);
	void ChangeNumNaturalWondersDiscoveredInArea(int iChange);

	int GetNumNaturalWondersInOwnedPlots();

	int GetTurnsSinceSettledLastCity() const;
	void SetTurnsSinceSettledLastCity(int iValue);
	void ChangeTurnsSinceSettledLastCity(int iChange);

	int GetBestSettleAreas(int iMinScore, int& iFirstArea, int& iSecondArea);
	CvPlot* GetBestSettlePlot(CvUnit* pUnit, bool bEscorted, int iArea = -1) const;

	// New Victory Stuff
	int GetNumWonders() const;
	void ChangeNumWonders(int iValue);
	int GetNumPolicies() const;
	void ChangeNumPolicies(int iValue);
	int GetNumGreatPeople() const;
	void ChangeNumGreatPeople(int iValue);
	// End New Victory Stuff

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
#if defined(MOD_API_EXTENSIONS)
	int AddNotification(NotificationTypes eNotificationType, const char* sMessage, const char* sSummary, CvPlot* pPlot = NULL, int iGameDataIndex = -1, int iExtraGameData = -1);
	int AddNotification(NotificationTypes eNotificationType, const char* sMessage, const char* sSummary, int iGameDataIndex, int iExtraGameData = -1);
#endif
	CvDiplomacyRequests* GetDiplomacyRequests() const;
	bool HasActiveDiplomacyRequests() const;

	CvTreasury* GetTreasury() const;

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
	bool IsAtPeaceWith(PlayerTypes iPlayer) const;
	bool IsAtWar() const;
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
	bool HasAnyTradeRouteWith(PlayerTypes iPlayer) const;
	bool HasUnit(UnitTypes iUnitType);
	bool HasUnitClass(UnitClassTypes iUnitClassType);

	bool HasTrait(TraitTypes eTrait) const;
	bool HasAnyHolyCity();
	bool HasHolyCity(ReligionTypes eReligion);
	bool HasCapturedHolyCity(ReligionTypes eReligion);
	bool HasEmbassyWith(PlayerTypes eOtherPlayer) const;
	void DoForceDefPact(PlayerTypes eOtherPlayer);
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
	virtual void AI_updateFoundValues(bool bStartingLoc = false) = 0;
	virtual void AI_unitUpdate() = 0;
	virtual void AI_conquerCity(CvCity* pCity, PlayerTypes eOldOwner) = 0;
	virtual int AI_foundValue(int iX, int iY, int iMinUnitRange = -1, bool bStartingLoc = false) = 0;
	virtual void AI_chooseFreeGreatPerson() = 0;
	virtual void AI_chooseFreeTech() = 0;
	virtual void AI_chooseResearch() = 0;
	virtual int AI_plotTargetMissionAIs(CvPlot* pPlot, MissionAITypes eMissionAI, int iRange = 0) = 0;
	virtual void AI_launch(VictoryTypes eVictory) = 0;

	virtual OperationSlot PeekAtNextUnitToBuildForOperationSlot(int iAreaID) = 0;
	virtual OperationSlot CityCommitToBuildUnitForOperationSlot(int iAreaID, int iTurns, CvCity* pCity) = 0;
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

	CvPlayerAchievements& GetPlayerAchievements(){return m_kPlayerAchievements;}

	bool hasTurnTimerExpired();

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	int GetScoreFromVassals() const;
	int GetScoreFromVassal(PlayerTypes ePlayer) const;

	int GetJONSCulturePerTurnFromVassals() const;

	int GetHappinessFromVassals() const;
	int GetHappinessFromVassal(PlayerTypes ePlayer) const;

	int GetScienceFromVassalTimes100() const;

	int GetVassalGoldMaintenanceMod() const;
	void SetVassalGoldMaintenanceMod(int iValue);
	void ChangeVassalGoldMaintenanceMod(int iChange);
#endif

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
	FAutoVariable<int, CvPlayer> m_iCulturePerWonder;
	FAutoVariable<int, CvPlayer> m_iCultureWonderMultiplier;
	FAutoVariable<int, CvPlayer> m_iCulturePerTechResearched;
	int m_iFaith;
	int m_iFaithEverGenerated;
	FAutoVariable<int, CvPlayer> m_iHappiness;
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	FAutoVariable<int, CvPlayer> m_iUnhappiness;
	FAutoVariable<int, CvPlayer> m_iHappinessTotal;
#endif
#if defined(MOD_BALANCE_CORE_HAPPINESS_LUXURY)
	FAutoVariable<int, CvPlayer> m_iBaseLuxuryHappiness;
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
	int m_iHappinessPerXPopulation;
	int m_iHappinessFromLeagues;
	FAutoVariable<int, CvPlayer> m_iSpecialPolicyBuildingHappiness;  //unused
	FAutoVariable<int, CvPlayer> m_iWoundedUnitDamageMod;
	FAutoVariable<int, CvPlayer> m_iUnitUpgradeCostMod;
	FAutoVariable<int, CvPlayer> m_iBarbarianCombatBonus;
	FAutoVariable<int, CvPlayer> m_iAlwaysSeeBarbCampsCount;
	FAutoVariable<int, CvPlayer> m_iHappinessFromBuildings;
	FAutoVariable<int, CvPlayer> m_iHappinessPerCity;
	int m_iHappinessPerXPolicies;
	int m_iEspionageModifier;
	int m_iSpyStartingRank;
#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
	int m_iConversionModifier;
#endif
	int m_iExtraLeagueVotes;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	int m_iImprovementLeagueVotes;
	int m_iFaithToVotes;
	int m_iCapitalsToVotes;
	int m_iDoFToVotes;
	int m_iRAToVotes;
	int m_iGPExpendInfluence;
	bool m_bIsLeagueAid;
	bool m_bIsLeagueScholar;
	bool m_bIsLeagueArt;
	int m_iScienceRateFromLeague;
	int m_iScienceRateFromLeagueAid;
	FAutoVariable<int, CvPlayer> m_iLeagueCultureCityModifier;
#endif
	FAutoVariable<int, CvPlayer> m_iAdvancedStartPoints;
	FAutoVariable<int, CvPlayer> m_iAttackBonusTurns;
	int m_iCultureBonusTurns;
	int m_iTourismBonusTurns;
	FAutoVariable<int, CvPlayer> m_iGoldenAgeProgressMeter;
	FAutoVariable<int, CvPlayer> m_iGoldenAgeMeterMod;
	FAutoVariable<int, CvPlayer> m_iNumGoldenAges;
	FAutoVariable<int, CvPlayer> m_iGoldenAgeTurns;
	FAutoVariable<int, CvPlayer> m_iNumUnitGoldenAges;
	FAutoVariable<int, CvPlayer> m_iStrikeTurns;
	FAutoVariable<int, CvPlayer> m_iGoldenAgeModifier;
	FAutoVariable<int, CvPlayer> m_iGreatPeopleCreated;
	FAutoVariable<int, CvPlayer> m_iGreatGeneralsCreated;
	int m_iGreatAdmiralsCreated;
#if defined(MOD_GLOBAL_SEPARATE_GP_COUNTERS)
	int m_iGreatMerchantsCreated;
	int m_iGreatScientistsCreated;
	int m_iGreatEngineersCreated;
#endif
	int m_iGreatWritersCreated;
	int m_iGreatArtistsCreated;
	int m_iGreatMusiciansCreated;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	int m_iGreatDiplomatsCreated;
	int m_iDiplomatsFromFaith;
#endif
	int m_iMerchantsFromFaith;
	int m_iScientistsFromFaith;
	int m_iWritersFromFaith;
	int m_iArtistsFromFaith;
	int m_iMusiciansFromFaith;
	int m_iGeneralsFromFaith;
	int m_iAdmiralsFromFaith;
	int m_iEngineersFromFaith;
	FAutoVariable<int, CvPlayer> m_iGreatPeopleThresholdModifier;
	FAutoVariable<int, CvPlayer> m_iGreatGeneralsThresholdModifier;
	int m_iGreatAdmiralsThresholdModifier;
	int m_iGreatGeneralCombatBonus;
	FAutoVariable<int, CvPlayer> m_iAnarchyNumTurns;
	FAutoVariable<int, CvPlayer> m_iPolicyCostModifier;
	FAutoVariable<int, CvPlayer> m_iGreatPeopleRateModifier;
	FAutoVariable<int, CvPlayer> m_iGreatPeopleRateModFromBldgs;
	FAutoVariable<int, CvPlayer> m_iGreatGeneralRateModifier;
	FAutoVariable<int, CvPlayer> m_iGreatGeneralRateModFromBldgs;
	FAutoVariable<int, CvPlayer> m_iDomesticGreatGeneralRateModifier;
	FAutoVariable<int, CvPlayer> m_iDomesticGreatGeneralRateModFromBldgs;
	int m_iGreatAdmiralRateModifier;
	int m_iGreatWriterRateModifier;
	int m_iGreatArtistRateModifier;
	int m_iGreatMusicianRateModifier;
	int m_iGreatMerchantRateModifier;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	int m_iGreatDiplomatRateModifier;
#endif
	int m_iGreatScientistRateModifier;
	int m_iGreatScientistBeakerModifier;
	int m_iGreatEngineerRateModifier;
	int m_iGreatPersonExpendGold;
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	int m_iPovertyUnhappinessMod;
	int m_iDefenseUnhappinessMod;
	int m_iIlliteracyUnhappinessMod;
	int m_iMinorityUnhappinessMod;
	int m_iPovertyUnhappinessModCapital;
	int m_iDefenseUnhappinessModCapital;
	int m_iIlliteracyUnhappinessModCapital;
	int m_iMinorityUnhappinessModCapital;
	int m_iPuppetUnhappinessMod;
	int m_iNoUnhappfromXSpecialists;
	int m_iNoUnhappfromXSpecialistsCapital;
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
	FAutoVariable<int, CvPlayer> m_iPolicyCostBuildingModifier;
	FAutoVariable<int, CvPlayer> m_iPolicyCostMinorCivModifier;
	int m_iInfluenceSpreadModifier;
	int m_iExtraVotesPerDiplomat;
	FAutoVariable<int, CvPlayer> m_iNumNukeUnits;
	FAutoVariable<int, CvPlayer> m_iNumOutsideUnits;
	FAutoVariable<int, CvPlayer> m_iBaseFreeUnits;
	FAutoVariable<int, CvPlayer> m_iBaseFreeMilitaryUnits;
	FAutoVariable<int, CvPlayer> m_iFreeUnitsPopulationPercent;
	FAutoVariable<int, CvPlayer> m_iFreeMilitaryUnitsPopulationPercent;
	FAutoVariable<int, CvPlayer> m_iGoldPerUnit;
	FAutoVariable<int, CvPlayer> m_iGoldPerMilitaryUnit;
	FAutoVariable<int, CvPlayer> m_iRouteGoldMaintenanceMod;
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
	FAutoVariable<int, CvPlayer> m_iMilitaryFoodProductionCount;
	int m_iGoldenAgeCultureBonusDisabledCount;
	int m_iSecondReligionPantheonCount;
	int m_iEnablesSSPartHurryCount;
	int m_iEnablesSSPartPurchaseCount;
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
	int m_iAbleToAnnexCityStatesCount;
	FAutoVariable<int, CvPlayer> m_iFreeSpecialist;
	FAutoVariable<int, CvPlayer> m_iCultureBombTimer;
	FAutoVariable<int, CvPlayer> m_iConversionTimer;
	FAutoVariable<int, CvPlayer> m_iCapitalCityID;
	FAutoVariable<int, CvPlayer> m_iCitiesLost;
	FAutoVariable<int, CvPlayer> m_iMilitaryMight;
	FAutoVariable<int, CvPlayer> m_iEconomicMight;
	FAutoVariable<int, CvPlayer> m_iTurnMightRecomputed;
	FAutoVariable<int, CvPlayer> m_iNewCityExtraPopulation;
	FAutoVariable<int, CvPlayer> m_iFreeFoodBox;
	FAutoVariable<int, CvPlayer> m_iScenarioScore1;
	FAutoVariable<int, CvPlayer> m_iScenarioScore2;
	FAutoVariable<int, CvPlayer> m_iScenarioScore3;
	FAutoVariable<int, CvPlayer> m_iScenarioScore4;
	FAutoVariable<int, CvPlayer> m_iScoreFromFutureTech;
	FAutoVariable<int, CvPlayer> m_iCombatExperience;
	int m_iLifetimeCombatExperience;
	int m_iNavalCombatExperience;
	int m_iBorderObstacleCount;
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
	int m_iCityWorkingChange;
#endif
	FAutoVariable<int, CvPlayer> m_iPlotCultureCostModifier;
	int m_iPlotCultureExponentModifier;
	int m_iNumCitiesPolicyCostDiscount;
	int m_iGarrisonedCityRangeStrikeModifier;
	int m_iGarrisonFreeMaintenanceCount;
	int m_iNumCitiesFreeCultureBuilding;
	int m_iNumCitiesFreeFoodBuilding;
	FAutoVariable<int, CvPlayer> m_iUnitPurchaseCostModifier;
	FAutoVariable<int, CvPlayer> m_iAllFeatureProduction;
	FAutoVariable<int, CvPlayer> m_iCityDistanceHighwaterMark; // this is used to determine camera zoom
	FAutoVariable<int, CvPlayer> m_iOriginalCapitalX;
	FAutoVariable<int, CvPlayer> m_iOriginalCapitalY;
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
	int m_iMedianTechPercentage;
	FAutoVariable<int, CvPlayer> m_iNumFreePolicies;
	FAutoVariable<int, CvPlayer> m_iNumFreePoliciesEver; 
	int m_iNumFreeTenets;
    int m_iMaxEffectiveCities;

	int m_iLastSliceMoved;

	FAutoVariable<uint, CvPlayer> m_uiStartTime;  // XXX save these?

	FAutoVariable<bool, CvPlayer> m_bHasBetrayedMinorCiv;
	FAutoVariable<bool, CvPlayer> m_bAlive;
	FAutoVariable<bool, CvPlayer> m_bEverAlive;
	bool m_bBeingResurrected;
	FAutoVariable<bool, CvPlayer> m_bTurnActive;
	FAutoVariable<bool, CvPlayer> m_bAutoMoves;					// Signal that we can process the auto moves when ready.
	bool						  m_bProcessedAutoMoves;		// Signal that we have processed the auto moves
	FAutoVariable<bool, CvPlayer> m_bEndTurn;					// Signal that the player has completed their turn.  The turn will still be active until the auto-moves have been processed.
	bool						  m_bDynamicTurnsSimultMode;
	FAutoVariable<bool, CvPlayer> m_bPbemNewTurn;
	FAutoVariable<bool, CvPlayer> m_bExtendedGame;
	FAutoVariable<bool, CvPlayer> m_bFoundedFirstCity;
	int m_iNumCitiesFounded;
	FAutoVariable<bool, CvPlayer> m_bStrike;
	FAutoVariable<bool, CvPlayer> m_bCramped;
	FAutoVariable<bool, CvPlayer> m_bLostCapital;
	PlayerTypes m_eConqueror;
	FAutoVariable<bool, CvPlayer> m_bHasAdoptedStateReligion;
	FAutoVariable<bool, CvPlayer> m_bAlliesGreatPersonBiasApplied;

	FAutoVariable<std::vector<int>, CvPlayer> m_aiCityYieldChange;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiCoastalCityYieldChange;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiCapitalYieldChange;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiCapitalYieldPerPopChange;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiSeaPlotYield;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiYieldRateModifier;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiCapitalYieldRateModifier;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiExtraYieldThreshold;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiSpecialistExtraYield;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiProximityToPlayer;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiResearchAgreementCounter;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiIncomingUnitTypes;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiIncomingUnitCountdowns;
	FAutoVariable<std::vector<int>, CvPlayer> m_aiMinorFriendshipAnchors; // DEPRECATED
	std::vector<int> m_aiSiphonLuxuryCount;
	std::vector<int> m_aiGreatWorkYieldChange;

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
	FAutoVariable<int, CvPlayer> m_iVassalGoldMaintenanceMod;
#endif
#if defined(MOD_BALANCE_CORE)
	FAutoVariable<std::vector<int>, CvPlayer> m_paiNumCitiesFreeChosenBuilding;
#endif

	FAutoVariable<std::vector<bool>, CvPlayer> m_pabLoyalMember;

	FAutoVariable<std::vector<bool>, CvPlayer> m_pabGetsScienceFromPlayer;

	FAutoVariable< std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > >, CvPlayer> m_ppaaiSpecialistExtraYield;
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

	FFreeListTrashArray<CvCityAI> m_cities;

	FFreeListTrashArray<CvUnit> m_units;

	FFreeListTrashArray<CvArmyAI> m_armyAIs;

	std::map<int, CvAIOperation*> m_AIOperations;
	std::map<int, CvAIOperation*>::iterator m_CurrentOperation;

	std::vector< std::pair<int, PlayerVoteTypes> > m_aVote;
	std::vector< std::pair<UnitClassTypes, int> > m_aUnitExtraCosts;

	std::vector<CvString> m_ReplayDataSets;
	std::vector< TurnData > m_ReplayDataSetValues;

	void doResearch();
	void doWarnings();

	// Danger plots!
	CvDangerPlots* m_pDangerPlots;

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

	CvPlotsVector m_aiPlots;

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

	void doUpdateCacheOnTurn();

	void doArmySize();

	friend class CvPlayerManager;
	friend CvUnit* GetPlayerUnit(IDInfo& unit);
	friend const CvUnit* GetPlayerUnit(const IDInfo& unit);

	CvPlayerAchievements m_kPlayerAchievements;
};

extern bool CancelActivePlayerEndTurn();

namespace FSerialization
{
void SyncPlayer();
void ClearPlayerDeltas();
}

#endif
