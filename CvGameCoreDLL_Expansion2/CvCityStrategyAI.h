/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_CITY_STRATEGY_AI_H
#define CIV5_CITY_STRATEGY_AI_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAICityStrategyEntry
//!  \brief		A single entry in the AI strategy XML file
//
//!  Key Attributes:
//!  - Populated from XML\???? (not sure what path/name you want)
//!  - Array of these contained in CvAICityStrategies class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAICityStrategyEntry: public CvBaseInfo
{
public:
	CvAICityStrategyEntry();
	virtual ~CvAICityStrategyEntry();

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	// Accessor functions
	int GetFlavorValue(int i) const;

	int GetWeightThreshold() const;
	int GetPersonalityFlavorThresholdMod(int i) const;

	int GetTechPrereq() const;
	int GetTechObsolete() const;
	int GetMinimumNumTurnsExecuted() const;
	int GetCheckTriggerTurnCount() const;

	bool IsNoMinorCivs() const;
	bool IsPermanent() const;

	AdvisorTypes GetAdvisor() const;
	const char* GetAdvisorCounselText() const;
	int GetAdvisorCounselImportance() const;

private:
	int* m_piFlavorValue;

	int m_iWeightThreshold;
	int* m_piPersonalityFlavorThresholdMod;

	int m_iTechPrereq;
	int m_iTechObsolete;
	int m_iMinimumNumTurnsExecuted;
	int m_iCheckTriggerTurnCount;

	bool m_bNoMinorCivs;
	bool m_bPermanent;

	AdvisorTypes m_eAdvisor;
	CvString m_strAdvisorCounselText;
	int m_iAdvisorCounselImportance;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAICityStrategies
//!  \brief		Game-wide information about possible AI strategies
//
//! Key Attributes:
//! - Plan is it will be contained in CvGameRules object within CvGame class
//! - Populated from XML\???? (not sure what path/name you want)
//! - Contains an array of CvAICityStrategyEntry from the above XML file
//! - One instance for the entire game
//! - Accessed heavily by CvCityStrategyAI class (which stores the AI strategy state for 1 player)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAICityStrategies
{
public:
	CvAICityStrategies(void);
	~CvAICityStrategies(void);

	// Accessor functions
	std::vector<CvAICityStrategyEntry*>& GetAICityStrategyEntries();
	int GetNumAICityStrategies();
	CvAICityStrategyEntry* GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvAICityStrategyEntry*> m_paAICityStrategyEntries;
};

class CvBuildingProductionAI;
class CvUnitProductionAI;
class CvProcessProductionAI;

enum CityBuildableType
{
    NOT_A_CITY_BUILDABLE = 0,
    CITY_BUILDABLE_BUILDING,
    CITY_BUILDABLE_UNIT,
    CITY_BUILDABLE_PROJECT,
	CITY_BUILDABLE_PROCESS,
    CITY_BUILDABLE_UNIT_FOR_OPERATION,
    CITY_BUILDABLE_UNIT_FOR_ARMY,
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  STRUCT:     CvCityBuildable
//!  \brief		One item a city can build
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
struct CvCityBuildable
{
	CvCityBuildable() :
		m_eBuildableType(NOT_A_CITY_BUILDABLE)
		, m_iIndex(0)
		, m_iTurnsToConstruct(0)
		, m_iValue(0)
		, m_iCityID(-1)
	{
	}

	CityBuildableType m_eBuildableType;
	int m_iIndex;							// index in building or unit array
	int m_iTurnsToConstruct;
	int m_iValue;
	int m_iCityID;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvCityStrategyAI
//!  \brief		Manages operations for a single city in the game world
//
//!  Key Attributes:
//!  - One instance for each city
//!  - Receives instructions from other AI components (usually as flavor changes) to
//!    specialize, switch production, etc.
//!  - Oversees both the city governor AI and the AI managing what the city is building
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCityStrategyAI: public CvFlavorRecipient
{
public:
	CvCityStrategyAI(void);
	~CvCityStrategyAI(void);

	void Init(CvAICityStrategies* pAICityStrategies, CvCity* pCity, bool bIsCity);
	void Uninit();
	void Reset();
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

	// Flavor recipient required function
	void FlavorUpdate();

	// Must be called when a new City is founded
	void UpdateFlavorsForNewCity();

	// City specialization functions
	CitySpecializationTypes GetSpecialization() const
	{
		return m_eSpecialization;
	};
	bool SetSpecialization(CitySpecializationTypes eSpecialization);
	CitySpecializationTypes GetDefaultSpecialization() const
	{
		return m_eDefaultSpecialization;
	};
	bool SetDefaultSpecialization(CitySpecializationTypes eDefaultSpecialization);
	void SpecializationFlavorChange(bool bTurnOn, CitySpecializationTypes eSpecialization);

	// City strategy accessor functions
	CvCity* GetCity();
	CvAICityStrategies* GetAICityStrategies();
	bool IsUsingCityStrategy(AICityStrategyTypes eStrategy);
	void SetUsingCityStrategy(AICityStrategyTypes eStrategy, bool bValue);
	int GetTurnCityStrategyAdopted(AICityStrategyTypes eStrategy);
	void SetTurnCityStrategyAdopted(AICityStrategyTypes eStrategy, int iValue);
	CvBuildingProductionAI* GetBuildingProductionAI();
	CvUnitProductionAI* GetUnitProductionAI();
	CvProjectProductionAI* GetProjectProductionAI();
	CvProcessProductionAI* GetProcessProductionAI();
	CvString GetLogFileName(CvString& playerName, CvString& cityName) const;
#if defined(MOD_BALANCE_CORE)
	CvString GetProductionLogFileName(CvString& playerName, CvString& cityName) const;
	CvString GetHurryLogFileName(CvString& playerName, CvString& cityName) const;
#endif

	bool IsYieldDeficient(YieldTypes yieldType);
#if defined(MOD_BALANCE_CORE)
	YieldTypes GetMostDeficientYield();
	YieldTypes GetHighestYield();
#endif

	YieldTypes GetDeficientYield(void);  // returns if any yield is deficient, starting with food, then production. Returns NO_YIELD if the city is fine
	double GetYieldAverage(YieldTypes eYieldType);
	double GetDeficientYieldValue(YieldTypes eYieldType);

#if defined(MOD_BALANCE_CORE)
	void PrecalcYieldAverages();
#endif

	// City AI methods
#if defined(MOD_BALANCE_CORE)
	void ChooseProduction(BuildingTypes eIgnoreBldg = NO_BUILDING, UnitTypes eIgnoreUnit = NO_UNIT, bool bInterruptBuildings = false, bool bInterruptWonders = false);
#else
	void ChooseProduction(BuildingTypes eIgnoreBldg = NO_BUILDING, UnitTypes eIgnoreUnit = NO_UNIT);
#endif
#if defined(MOD_BALANCE_CORE)
	CvCityBuildable ChooseHurry(bool bUnitOnly = false, bool bFaithPurchase = false);
	void LogHurryMessage(CvString& strMsg);
#endif
	void DoTurn();

	// these functions must be called together. Reset clears the internal arrays, update evalutes the city, and GetBestYieldAverage... returns the value that the builder AI uses.
	void ResetBestYields();
	void UpdateBestYields();
	unsigned short GetBestYieldAverageTimes100(YieldTypes eYield);
	short GetYieldDeltaTimes100(YieldTypes eYield);
	YieldTypes GetFocusYield();

	// Public logging functions
	void LogHurry(HurryTypes iHurryType, int iHurryAmount, int iHurryAmountAvailable, int iTurnsSaved);
	void LogCityProduction(CvCityBuildable Buildable, bool bRush);

	void LogInvalidItem(CvCityBuildable Buildable, int iVal);

private:

	void ReweightByCost();
#if defined(MOD_BALANCE_CORE)
	void ReweightPreCheckByCost();
#endif

	// Logging functions
	void LogFlavors(FlavorTypes eFlavor = NO_FLAVOR);
	void LogStrategy(AICityStrategyTypes eStrategy, bool bValue);
	void LogPossibleBuilds();
#if defined(MOD_BALANCE_CORE)
	void LogPossibleHurries();
	void LogPossibleHurriesPostCheck();
	void LogPossibleBuildsPostCheck();
#endif
	void LogSpecializationChange(CitySpecializationTypes eSpecialization);

	CvCity* m_pCity;
	CvAICityStrategies* m_pAICityStrategies;

	CitySpecializationTypes m_eSpecialization;
	CitySpecializationTypes m_eDefaultSpecialization;

	bool* m_pabUsingCityStrategy;
	int* m_paiTurnCityStrategyAdopted;
	int* m_aiTempFlavors;

	// AI sub objects
	CvBuildingProductionAI* m_pBuildingProductionAI;
	CvUnitProductionAI* m_pUnitProductionAI;
	CvProjectProductionAI* m_pProjectProductionAI;
	CvProcessProductionAI* m_pProcessProductionAI;

	CvWeightedVector<CvCityBuildable, (SAFE_ESTIMATE_NUM_BUILDINGS + SAFE_ESTIMATE_NUM_UNITS), true> m_Buildables;
#if defined(MOD_BALANCE_CORE)
	CvWeightedVector<CvCityBuildable, (SAFE_ESTIMATE_NUM_BUILDINGS + SAFE_ESTIMATE_NUM_UNITS), true> m_BuildablesPrecheck;
#endif


	static unsigned char  m_acBestYields[NUM_YIELD_TYPES][MAX_CITY_PLOTS];
	unsigned short m_asBestYieldAverageTimes100[NUM_YIELD_TYPES];
	short m_asYieldDeltaTimes100[NUM_YIELD_TYPES];
	YieldTypes m_eFocusYield;

#if defined(MOD_BALANCE_CORE)
	double m_adYieldAvg[NUM_YIELD_TYPES];
#endif

};

namespace CityStrategyAIHelpers
{
// CvCityStrategy namespace - non-member functions
int ReweightByTurnsLeft(int iOriginalWeight, int iTurnsLeft);
int GetWeightThresholdModifier(AICityStrategyTypes eStrategy, CvCity* pCity);

// The following functions check triggers to see if a particular CityStrategy should be adopted/continued
bool IsTestCityStrategy_TinyCity(CvCity* pCity);
bool IsTestCityStrategy_SmallCity(CvCity* pCity);
bool IsTestCityStrategy_MediumCity(CvCity* pCity);
bool IsTestCityStrategy_LargeCity(CvCity* pCity);
bool IsTestCityStrategy_Landlocked(CvCity* pCity);
#if defined(MOD_BALANCE_CORE)
bool IsTestCityStrategy_Lakebound(CvCity* pCity);
#endif
bool IsTestCityStrategy_NeedTileImprovers(AICityStrategyTypes eStrategy, CvCity* pCity);
bool IsTestCityStrategy_WantTileImprovers(AICityStrategyTypes eStrategy, CvCity* pCity);
bool IsTestCityStrategy_EnoughTileImprovers(AICityStrategyTypes eStrategy, CvCity* pCity);
bool IsTestCityStrategy_NeedNavalGrowth(AICityStrategyTypes eStrategy, CvCity* pCity);
bool IsTestCityStrategy_NeedNavalTileImprovement(CvCity* pCity);
bool IsTestCityStrategy_EnoughNavalTileImprovement(CvCity* pCity);
#if defined(MOD_BALANCE_CORE)
bool IsTestCityStrategy_EnoughSettlers(CvCity* pCity);
#if defined(MOD_BUGFIX_MINOR_CIV_STRATEGIES)
bool IsTestCityStrategy_NewContinentFeeder(AICityStrategyTypes eStrategy, CvCity* pCity);
#else
bool IsTestCityStrategy_NewContinentFeeder(CvCity* pCity);
#endif
bool IsTestCityStrategy_PocketCity(CvCity* pCity);
#endif
bool IsTestCityStrategy_NeedImprovement(CvCity* pCity, YieldTypes yield);
bool IsTestCityStrategy_HaveTrainingFacility(CvCity* pCity);
bool IsTestCityStrategy_CapitalNeedSettler(AICityStrategyTypes eStrategy, CvCity* pCity);
bool IsTestCityStrategy_CapitalUnderThreat(CvCity* pCity);
bool IsTestCityStrategy_FirstCultureBuilding(CvCity* pCity);
bool IsTestCityStrategy_FirstCultureBuildingEmergency(CvCity* pCity);
bool IsTestCityStrategy_FirstScienceBuilding(CvCity* pCity);
bool IsTestCityStrategy_FirstGoldBuilding(CvCity* pCity);
bool IsTestCityStrategy_FirstFaithBuilding(CvCity* pCity);
bool IsTestCityStrategy_FirstProductionBuilding(CvCity* pCity);
bool IsTestCityStrategy_UnderBlockade(CvCity* pCity);
bool IsTestCityStrategy_IsPuppet(CvCity* pCity);
bool IsTestCityStrategy_MediumCityHighDifficulty(CvCity* pCity);
bool IsTestCityStrategy_OriginalCapital(CvCity* pCity);
bool IsTestCityStrategy_RiverCity(CvCity* pCity);
bool IsTestCityStrategy_HillCity(CvCity* pCity);
bool IsTestCityStrategy_MountainCity(CvCity* pCity);
bool IsTestCityStrategy_ForestCity(CvCity* pCity);
bool IsTestCityStrategy_JungleCity(CvCity* pCity);
bool IsTestCityStrategy_CoastCity(CvCity* pCity);
bool IsTestCityStrategy_ManyTechsStolen(CvCity* pCity);
bool IsTestCityStrategy_KeyScienceCity(CvCity* pCity);
bool IsTestCityStrategy_GoodGPCity(CvCity* pCity);
bool IsTestCityStrategy_NeedInternationalTradeRoute(CvCity* pCity, DomainTypes eDomain);
bool IsTestCityStrategy_NoNeedInternationalTradeRoute(CvCity* pCity, DomainTypes eDomain);
bool IsTestCityStrategy_IsInternationalTradeDestination(CvCity* pCity);
bool IsTestCityStrategy_IsInternationalTradeOrigin(CvCity* pCity);
bool IsTestCityStrategy_NeedCultureBuilding(CvCity* pCity);
bool IsTestCityStrategy_NeedTourismBuilding(CvCity *pCity);
bool IsTestCityStrategy_GoodAirliftCity(CvCity *pCity);
#if defined(MOD_DIPLOMACY_CITYSTATES)
bool IsTestCityStrategy_NeedDiplomats(CvCity *pCity);
bool IsTestCityStrategy_NeedDiplomatsCritical(CvCity *pCity);
#endif
#if defined(MOD_BALANCE_CORE_HAPPINESS)
bool IsTestCityStrategy_NeedHappinessCulture(CvCity *pCity);
bool IsTestCityStrategy_NeedHappinessScience(CvCity *pCity);
bool IsTestCityStrategy_NeedHappinessDefense(CvCity *pCity);
bool IsTestCityStrategy_NeedHappinessGold(CvCity *pCity);
bool IsTestCityStrategy_NeedHappinessConnection(CvCity *pCity);
bool IsTestCityStrategy_NeedHappinessPillage(CvCity *pCity);
bool IsTestCityStrategy_NeedHappinessReligion(CvCity *pCity);
bool IsTestCityStrategy_NeedHappinessStarve(CvCity *pCity);


int GetBuildingYieldValue(CvCity *pCity, BuildingTypes eBuilding, YieldTypes eYield);
int GetBuildingGrandStrategyValue(CvCity *pCity, BuildingTypes eBuilding, PlayerTypes ePlayer);
int GetBuildingPolicyValue(CvCity *pCity, BuildingTypes eBuilding);
int GetBuildingBasicValue(CvCity *pCity, BuildingTypes eBuilding);
int GetBuildingTraitValue(CvCity *pCity, YieldTypes eYield, BuildingTypes eBuilding, int iValue);
#endif
}

#endif // CIV5_CITY_STRATEGY_AI_H