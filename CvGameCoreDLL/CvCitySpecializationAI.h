/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_CITY_SPECIALIZATION_AI_H
#define CIV5_CITY_SPECIALIZATION_AI_H

enum ProductionSpecializationSubtypes
{
	PRODUCTION_SPECIALIZATION_MILITARY_TRAINING,
	PRODUCTION_SPECIALIZATION_EMERGENCY_UNITS,
	PRODUCTION_SPECIALIZATION_MILITARY_NAVAL,
	PRODUCTION_SPECIALIZATION_WONDER,
	PRODUCTION_SPECIALIZATION_SPACESHIP,
	NUM_PRODUCTION_SPECIALIZATION_SUBTYPES
};

// Add one more for general economics
#define NUM_SPECIALIZATION_YIELDS (NUM_YIELD_TYPES+1)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvCitySpecializationXMLEntry
//!  \brief		A single entry in the city specialization XML file
//
//!  Key Attributes:
//!  - Populated from XML\AI\CIV5CitySpecializations.xml
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCitySpecializationXMLEntry: public CvBaseInfo
{
public:
	CvCitySpecializationXMLEntry();
	virtual ~CvCitySpecializationXMLEntry();

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	// Accessor Functions
	int GetFlavorValue(int i) const;
	YieldTypes GetYieldType() const;
	int GetYieldTargetTimes100 (YieldTypes eYield) const;
	int GetSubtype() const;
	bool IsWonder() const;
	bool IsDefault() const;
	bool IsMustBeCoastal() const;
	bool IsOperationUnitProvider() const;

private:
	int* m_piFlavorValue;
	YieldTypes m_eYieldType;
	int m_iSubtype;
	bool m_bWonder;
	bool m_bDefault;
	bool m_bMustBeCoastal;
	bool m_bOperationUnitProvider;
	int* m_piYieldTargetTimes100;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvCitySpecializationXMLEntries
//!  \brief		Game-wide information about possible AI strategies
//
//! Key Attributes:
//! - Populated from XML\AI\Civ5CitySpecializationInfos.xml
//! - Contains an array of CvCitySpecializationXMLEntry from the above XML file
//! - One instance for the entire game
//! - Accessed heavily by CvCitySpecializationAI class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCitySpecializationXMLEntries
{
public:
	CvCitySpecializationXMLEntries(void);
	~CvCitySpecializationXMLEntries(void);

	// Accessor functions
	std::vector<CvCitySpecializationXMLEntry*>& GetCitySpecializationEntries();
	int GetNumCitySpecializations();
	CvCitySpecializationXMLEntry *GetEntry(int index);
	CitySpecializationTypes GetFirstSpecializationForYield(YieldTypes eYield);
	CitySpecializationTypes GetNextSpecializationForYield();
	int GetNumSpecializationsForYield(YieldTypes eYield);

	void DeleteArray();

private:
	std::vector<CvCitySpecializationXMLEntry*> m_paCitySpecializationEntries;
	int m_CurrentIndex;
	YieldTypes m_CurrentYield;
};

enum CitySpecializationUpdateType
{
	SPECIALIZATION_UPDATE_NEW_GRAND_STRATEGY,
	SPECIALIZATION_UPDATE_NOW_AT_WAR,
	SPECIALIZATION_UPDATE_CITY_FOUNDED,
	SPECIALIZATION_UPDATE_MY_CITY_CAPTURED,
	SPECIALIZATION_UPDATE_ENEMY_CITY_CAPTURED,
	SPECIALIZATION_UPDATE_STRATEGY_NOW_ON,
	SPECIALIZATION_UPDATE_STRATEGY_NOW_OFF,
	SPECIALIZATION_UPDATE_RESEARCH_COMPLETE,
	SPECIALIZATION_UPDATE_WONDER_BUILT_BY_RIVAL,
};

class CitySpecializationData
{
public:
	int m_eID;
	int m_iWeight[NUM_YIELD_TYPES];
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvCitySpecializationAI
//!  \brief		Decides which cities are assigned special roles
//
//!  Key Attributes:
//!  - Plan is it will be contained in CvPlayerState object within CvPlayer class
//!  - One instance for each civ (player or AI)
//!  - Accessed by any class that needs to ask about or update city specializations
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CvCitySpecializationAI
{
public:
	CvCitySpecializationAI(void);
	~CvCitySpecializationAI(void);
	void Init(CvCitySpecializationXMLEntries *pSpecializations, CvPlayer *pPlayer);
	void Uninit();
	void Reset();
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	CvPlayer* GetPlayer();
	void DoTurn();

	CvCitySpecializationXMLEntries* GetCitySpecializations();
	void SetSpecializationsDirty (CitySpecializationUpdateType eUpdate);
	CitySpecializationTypes GetNextSpecializationDesired() const { return m_eNextSpecializationDesired; };
	BuildingTypes GetNextWonderDesired() const { return m_eNextWonderDesired; };
	CvCity *GetWonderBuildCity() const;

	// Public since WonderProductionAI also writes to this file
	CvString GetLogFileName(CvString& playerName) const;

	CitySpecializationTypes GetWonderSpecialization() const;

private:
	void WeightSpecializations();
	int WeightProductionSubtypes(int iFlavorWonder, int iFlavorSpaceship);
	void AssignSpecializations();
	void SelectSpecializations();
	CitySpecializationTypes SelectProductionSpecialization(int &iReductionAmount);
	CitySpecializationTypes GetEconomicDefaultSpecialization() const;
	int GetWonderSubtype() const;
	CvCity *FindBestWonderCity() const;
	void FindBestSites();
	int PlotValueForSpecificYield(CvPlot *pPlot, YieldTypes eYield);
	int PlotValueForScience(CvPlot *pPlot);
	int AdjustValueBasedOnBuildings(CvCity *pCity, YieldTypes eYield, int iInitialValue);
	bool CanBuildSpaceshipParts();

	// Logging functions
	void LogSpecializationWeights();
	void LogSpecializationAssignment(CvCity *pCity, CitySpecializationTypes eType, bool bWonderCity=false);
	void LogSpecializationUpdate(CitySpecializationUpdateType eUpdate);
	void LogNextSpecialization(CitySpecializationTypes eType);
	void LogBestSites();
	void LogCity(CvCity *pCity, CitySpecializationData data);

	CvPlayer *m_pPlayer;
	CvCitySpecializationXMLEntries *m_pSpecializations;
	bool m_bSpecializationsDirty;
	CitySpecializationTypes m_eNextSpecializationDesired;
	list<CitySpecializationTypes> m_SpecializationsNeeded;
	int m_iBestValue[NUM_YIELD_TYPES];
	CvWeightedVector<YieldTypes, NUM_SPECIALIZATION_YIELDS, true> m_YieldWeights;
	int m_iNumSpecializationsForThisYield[NUM_SPECIALIZATION_YIELDS];  // Array is offset by 1 so NO_YIELD = 0, YIELD_FOOD = 1
	CvWeightedVector<ProductionSpecializationSubtypes, 4, true> m_ProductionSubtypeWeights;
	int m_iNumSpecializationsForThisSubtype[NUM_PRODUCTION_SPECIALIZATION_SUBTYPES];
	int m_iLastTurnEvaluated;

	// Wonder builds
	bool m_bInterruptWonders;
	BuildingTypes m_eNextWonderDesired;
	int m_iWonderCityID;
	int m_iNextWonderWeight;
	bool m_bWonderChosen;
};

#endif //CIV5_CITY_SPECIALIZATION_AI_H