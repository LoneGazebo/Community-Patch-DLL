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

	AdvisorTypes GetAdvisor () const;
	const char* GetAdvisorCounselText() const;
	int GetAdvisorCounselImportance () const;

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
	CvAICityStrategyEntry *GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvAICityStrategyEntry*> m_paAICityStrategyEntries;
};

class CvBuildingProductionAI;
class CvUnitProductionAI;

enum CityBuildableType
{
	NOT_A_CITY_BUILDABLE = 0,
	CITY_BUILDABLE_BUILDING,
	CITY_BUILDABLE_UNIT,
	CITY_BUILDABLE_PROJECT,
	CITY_BUILDABLE_UNIT_FOR_OPERATION,
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
	{
	}

	CityBuildableType m_eBuildableType;
	int m_iIndex;							// index in building or unit array
	int m_iTurnsToConstruct;
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

	void Init(CvAICityStrategies *pAICityStrategies, CvCity *pCity, bool bIsCity);
	void Uninit();
	void Reset();
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

	// Flavor recipient required function
	void FlavorUpdate();

	// Must be called when a new City is founded
	void UpdateFlavorsForNewCity();

	// City specialization functions
	CitySpecializationTypes GetSpecialization() const {return m_eSpecialization;};
	bool SetSpecialization (CitySpecializationTypes eSpecialization);
	CitySpecializationTypes GetDefaultSpecialization() const {return m_eDefaultSpecialization;};
	bool SetDefaultSpecialization (CitySpecializationTypes eDefaultSpecialization);
	void SpecializationFlavorChange (bool bTurnOn, CitySpecializationTypes eSpecialization);

	// City strategy accessor functions
	CvCity* GetCity();
	CvAICityStrategies* GetAICityStrategies();
	bool IsUsingCityStrategy(AICityStrategyTypes eStrategy);
	void SetUsingCityStrategy(AICityStrategyTypes eStrategy, bool bValue);
	int GetTurnCityStrategyAdopted(AICityStrategyTypes eStrategy);
	void SetTurnCityStrategyAdopted(AICityStrategyTypes eStrategy, int iValue);
	CvBuildingProductionAI *GetBuildingProductionAI();
	CvUnitProductionAI *GetUnitProductionAI();
	CvProjectProductionAI *GetProjectProductionAI();
	CvString GetLogFileName(CvString& playerName, CvString& cityName) const;

	bool IsYieldDeficient (YieldTypes yieldType);
	YieldTypes GetDeficientYield (void); // returns if any yield is deficient, starting with food, then production. Returns NO_YIELD if the city is fine
	double GetYieldAverage (YieldTypes eYieldType);
	double GetDeficientYieldValue (YieldTypes eYieldType);

	// City AI methods
	void ChooseProduction(bool bUseAsyncRandom, BuildingTypes eIgnoreBldg = NO_BUILDING, UnitTypes eIgnoreUnit = NO_UNIT);
	void DoTurn();

	// these functions must be called together. Reset clears the internal arrays, update evalutes the city, and GetBestYieldAverage... returns the value that the builder AI uses.
	void ResetBestYields ();
	void UpdateBestYields ();
	unsigned short GetBestYieldAverageTimes100(YieldTypes eYield);
	short GetYieldDeltaTimes100 (YieldTypes eYield);
	YieldTypes GetFocusYield ();

	// Public logging functions
	void LogHurry (HurryTypes iHurryType, int iHurryAmount, int iHurryAmountAvailable, int iTurnsSaved);
	void LogCityProduction(CvCityBuildable Buildable, bool bRush);

private:

	void ReweightByCost();

	// Logging functions
	void LogFlavors(FlavorTypes eFlavor = NO_FLAVOR);
	void LogStrategy(AICityStrategyTypes eStrategy, bool bValue);
	void LogPossibleBuilds();
	void LogSpecializationChange(CitySpecializationTypes eSpecialization);

	CvCity *m_pCity;
	CvAICityStrategies *m_pAICityStrategies;

	CitySpecializationTypes m_eSpecialization;
	CitySpecializationTypes m_eDefaultSpecialization;

	bool* m_pabUsingCityStrategy;
	int* m_paiTurnCityStrategyAdopted;
	int* m_aiTempFlavors;

	// AI sub objects
	CvBuildingProductionAI *m_pBuildingProductionAI;
	CvUnitProductionAI *m_pUnitProductionAI;
	CvProjectProductionAI *m_pProjectProductionAI;

	CvWeightedVector<CvCityBuildable, (SAFE_ESTIMATE_NUM_BUILDINGS + SAFE_ESTIMATE_NUM_UNITS), true> m_Buildables;

	static unsigned char  m_acBestYields[NUM_YIELD_TYPES][NUM_CITY_PLOTS - 1];
	unsigned short m_asBestYieldAverageTimes100[NUM_YIELD_TYPES];
	short m_asYieldDeltaTimes100[NUM_YIELD_TYPES];
	YieldTypes m_eFocusYield;
};

namespace CityStrategyAIHelpers
{
	// CvCityStrategy namespace - non-member functions
	int ReweightByTurnsLeft(int iOriginalWeight, int iTurnsLeft);
	int GetWeightThresholdModifier(AICityStrategyTypes eStrategy, CvCity *pCity);

	// The following functions check triggers to see if a particular CityStrategy should be adopted/continued
	bool IsTestCityStrategy_TinyCity(CvCity *pCity);
	bool IsTestCityStrategy_SmallCity(CvCity *pCity);
	bool IsTestCityStrategy_MediumCity(CvCity *pCity);
	bool IsTestCityStrategy_LargeCity(CvCity *pCity);
	bool IsTestCityStrategy_Landlocked(CvCity *pCity);
	bool IsTestCityStrategy_NeedTileImprovers(AICityStrategyTypes eStrategy, CvCity *pCity);
	bool IsTestCityStrategy_WantTileImprovers(AICityStrategyTypes eStrategy, CvCity *pCity);
	bool IsTestCityStrategy_EnoughTileImprovers(AICityStrategyTypes eStrategy, CvCity *pCity);
	bool IsTestCityStrategy_NeedNavalGrowth(AICityStrategyTypes eStrategy, CvCity *pCity);
	bool IsTestCityStrategy_NeedNavalTileImprovement(CvCity *pCity);
	bool IsTestCityStrategy_EnoughNavalTileImprovement(CvCity *pCity);
	bool IsTestCityStrategy_NeedImprovement (CvCity* pCity, YieldTypes yield);
	bool IsTestCityStrategy_HaveTrainingFacility(CvCity* pCity);
	bool IsTestCityStrategy_CapitalNeedSettler(AICityStrategyTypes eStrategy, CvCity* pCity);
	bool IsTestCityStrategy_CapitalUnderThreat(CvCity* pCity);
	bool IsTestCityStrategy_FirstCultureBuilding(CvCity* pCity);
	bool IsTestCityStrategy_FirstCultureBuildingEmergency(CvCity* pCity);
	bool IsTestCityStrategy_FirstScienceBuilding(CvCity* pCity);
	bool IsTestCityStrategy_FirstGoldBuilding(CvCity* pCity);
	bool IsTestCityStrategy_UnderBlockade(CvCity* pCity);
	bool IsTestCityStrategy_IsPuppet(CvCity* pCity);
	bool IsTestCityStrategy_MediumCityHighDifficulty(CvCity *pCity);
	bool IsTestCityStrategy_OriginalCapital(CvCity *pCity);
	bool IsTestCityStrategy_RiverCity(CvCity *pCity);
	bool IsTestCityStrategy_HillCity(CvCity *pCity);
	bool IsTestCityStrategy_MountainCity(CvCity *pCity);
	bool IsTestCityStrategy_ForestCity(CvCity *pCity);
	bool IsTestCityStrategy_JungleCity(CvCity *pCity);
	bool IsTestCityStrategy_CoastCity(CvCity *pCity);

}

#endif // CIV5_CITY_STRATEGY_AI_H