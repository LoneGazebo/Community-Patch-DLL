/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_TRAIT_CLASSES_H
#define CIV5_TRAIT_CLASSES_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvTraitEntry
//!  \brief		A single entry in the trait XML file
//
//!  Key Attributes:
//!  - Used to be called CvTraitInfo
//!  - Populated from XML\Civilizations\CIV5Traits.xml
//!  - Array of these contained in CvTraitXMLEntries class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTraitEntry: public CvBaseInfo
{
public:
	CvTraitEntry(void);
	~CvTraitEntry(void);

	int GetLevelExperienceModifier() const;
	int GetGreatPeopleRateModifier() const;
	int GetGreatScientistRateModifier() const;
	int GetGreatGeneralRateModifier() const;
	int GetGreatGeneralExtraBonus() const;
	int GetMaxGlobalBuildingProductionModifier() const;
	int GetMaxTeamBuildingProductionModifier() const;
	int GetMaxPlayerBuildingProductionModifier() const;
	int GetCityUnhappinessModifier() const;
	int GetPopulationUnhappinessModifier() const;
	int GetCityStateBonusModifier() const;
	int GetCityStateFriendshipModifier() const;
	int GetCityStateCombatModifier() const;
	int GetLandBarbarianConversionPercent() const;
	int GetLandBarbarianConversionExtraUnits() const;
	int GetSeaBarbarianConversionPercent() const;
	int GetCapitalBuildingModifier() const;
	int GetPlotBuyCostModifier() const;
	int GetPlotCultureCostModifier() const;
	int GetCultureFromKills() const;
	int GetCityCultureBonus() const;
	int GetPolicyCostModifier() const;
	int GetTradeRouteChange() const;
	int GetWonderProductionModifier() const;
	int GetPlunderModifier() const;
	int GetImprovementMaintenanceModifier() const;
	int GetGoldenAgeDurationModifier() const;
	int GetGoldenAgeMoveChange() const;
	int GetGoldenAgeCombatModifier() const;
	int GetExtraEmbarkMoves() const;
	int GetFreeUnitClassType() const;
	int GetNaturalWonderFirstFinderGold() const;
	int GetNaturalWonderSubsequentFinderGold() const;
	int GetNaturalWonderYieldModifier() const;
	int GetNaturalWonderHappinessModifier() const;
	int GetNearbyImprovementCombatBonus() const;
	int GetNearbyImprovementBonusRange() const;
	int GetCultureBuildingYieldChange() const;
	int GetLandUnitMaintenanceModifier() const;
	int GetNavalUnitMaintenanceModifier() const;

	TechTypes GetFreeUnitPrereqTech() const;
	ImprovementTypes GetCombatBonusImprovement() const;

	bool IsFightWellDamaged() const;
	bool IsMoveFriendlyWoodsAsRoad() const;
	bool IsFasterAlongRiver() const;
	bool IsFasterInHills() const;
	bool IsEmbarkedNotCivilian() const;
	bool IsEmbarkedAllWater() const;
	bool IsEmbarkedToLandFlatCost() const;
	bool IsNoHillsImprovementMaintenance() const;
	bool IsTechBoostFromCapitalScienceBuildings() const;
	bool IsStaysAliveZeroCities() const;

	const char* getShortDescription() const;
	void setShortDescription(const char* szVal);

	// Arrays
	int GetExtraYieldThreshold(int i) const;
	int GetYieldChange(int i) const;
	int GetYieldChangeStrategicResources(int i) const;
	int GetYieldModifier(int i) const;
	int GetStrategicResourceQuantityModifier(int i) const;
	int GetObsoleteTech() const;
	int GetResourceQuantityModifier(int i) const;
	int GetMovesChangeUnitCombat(const int unitCombatID) const;
	int GetImprovementYieldChanges(ImprovementTypes eIndex1, YieldTypes eIndex2) const;
	int GetSpecialistYieldChanges(SpecialistTypes eIndex1, YieldTypes eIndex2) const;

	bool IsFreePromotionUnitCombat(const int promotionID, const int unitCombatID) const;
	bool IsObsoleteByTech(TeamTypes eTeam);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iLevelExperienceModifier;
	int m_iGreatPeopleRateModifier;
	int m_iGreatScientistRateModifier;
	int m_iGreatGeneralRateModifier;
	int m_iGreatGeneralExtraBonus;
	int m_iMaxGlobalBuildingProductionModifier;
	int m_iMaxTeamBuildingProductionModifier;
	int m_iMaxPlayerBuildingProductionModifier;
	int m_iCityUnhappinessModifier;
	int m_iPopulationUnhappinessModifier;
	int m_iCityStateBonusModifier;
	int m_iCityStateFriendshipModifier;
	int m_iCityStateCombatModifier;
	int m_iLandBarbarianConversionPercent;
	int m_iLandBarbarianConversionExtraUnits;
	int m_iSeaBarbarianConversionPercent;
	int m_iCapitalBuildingModifier;
	int m_iPlotBuyCostModifier;
	int m_iPlotCultureCostModifier;
	int m_iCultureFromKills;
	int m_iCityCultureBonus;
	int m_iPolicyCostModifier;
	int m_iTradeRouteChange;
	int m_iWonderProductionModifier;
	int m_iPlunderModifier;
	int m_iImprovementMaintenanceModifier;
	int m_iGoldenAgeDurationModifier;
	int m_iGoldenAgeMoveChange;
	int m_iGoldenAgeCombatModifier;
	int m_iObsoleteTech;
	int m_iExtraEmbarkMoves;
	int m_iFreeUnitClassType;
	int m_iNaturalWonderFirstFinderGold;
	int m_iNaturalWonderSubsequentFinderGold;
	int m_iNaturalWonderYieldModifier;
	int m_iNaturalWonderHappinessModifier;
	int m_iNearbyImprovementCombatBonus;
	int m_iNearbyImprovementBonusRange;
	int m_iCultureBuildingYieldChange;
	int m_iLandUnitMaintenanceModifier;
	int m_iNavalUnitMaintenanceModifier;

	TechTypes m_eFreeUnitPrereqTech;
	ImprovementTypes m_eCombatBonusImprovement;

	bool m_bFightWellDamaged;
	bool m_bMoveFriendlyWoodsAsRoad;
	bool m_bFasterAlongRiver;
	bool m_bFasterInHills;
	bool m_bEmbarkedNotCivilian;
	bool m_bEmbarkedAllWater;
	bool m_bEmbarkedToLandFlatCost;
	bool m_bNoHillsImprovementMaintenance;
	bool m_bTechBoostFromCapitalScienceBuildings;
	bool m_bStaysAliveZeroCities;

	CvString m_strShortDescription;

	// Arrays
	int* m_paiExtraYieldThreshold;
	int* m_paiYieldChange;
	int* m_paiYieldChangeStrategicResources;
	int* m_paiYieldModifier;
	int* m_piStrategicResourceQuantityModifier;
	int* m_piResourceQuantityModifiers;
	int* m_piMovesChangeUnitCombats;
	int** m_ppiImprovementYieldChanges;
	int** m_ppiSpecialistYieldChanges;

	std::multimap<int, int> m_FreePromotionUnitCombats;

private:
	CvTraitEntry( const CvTraitEntry&);
	CvTraitEntry& operator=( const CvTraitEntry& );
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvTraitXMLEntries
//!  \brief		Game-wide information about civilization traits
//
//! Key Attributes:
//! - Plan is it will be contained in CvGameRules object within CvGame class
//! - Populated from XML\GameInfo\CIV5Traits.xml
//! - Contains an array of CvTraitEntry from the above XML file
//! - One instance for the entire game
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTraitXMLEntries
{
public:
	CvTraitXMLEntries(void);
	~CvTraitXMLEntries(void);

	// Accessor functions
	std::vector<CvTraitEntry*>& GetTraitEntries();
	int GetNumTraits();
	CvTraitEntry *GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvTraitEntry*> m_paTraitEntries;
};

#define SAFE_ESTIMATE_NUM_FREE_UNITS 5

struct FreeTraitUnit
{
	UnitTypes m_iFreeUnit;
	TechTypes m_ePrereqTech;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvPlayerTraits
//!  \brief		Information about the traits of a player
//
//!  Key Attributes:
//!  - One instance for each player
//!  - Accessed by any class that needs to check trait info
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPlayerTraits
{
public:
	CvPlayerTraits(void);
	~CvPlayerTraits(void);
	void Init(CvTraitXMLEntries *pTraits, CvPlayer *pPlayer);
	void Uninit();
	void Reset();
	void InitPlayerTraits();

	// Accessor functions
	bool HasTrait(TraitTypes eTrait) const;
	int GetGreatPeopleRateModifier() const {return m_iGreatPeopleRateModifier;};
	int GetGreatScientistRateModifier() const {return m_iGreatScientistRateModifier;};
	int GetGreatGeneralRateModifier() const {return m_iGreatGeneralRateModifier;};
	int GetGreatGeneralExtraBonus() const {return m_iGreatGeneralExtraBonus;};
	int GetLevelExperienceModifier() const {return m_iLevelExperienceModifier;};
	int GetMaxGlobalBuildingProductionModifier() const {return m_iMaxGlobalBuildingProductionModifier;};
	int GetMaxTeamBuildingProductionModifier() const {return m_iMaxTeamBuildingProductionModifier;};
	int GetMaxPlayerBuildingProductionModifier() const {return m_iMaxPlayerBuildingProductionModifier;};
	int GetCityUnhappinessModifier() const {return m_iCityUnhappinessModifier;};
	int GetPopulationUnhappinessModifier() const {return m_iPopulationUnhappinessModifier;};
	int GetCityStateBonusModifier() const {return m_iCityStateBonusModifier;};
	int GetCityStateFriendshipModifier() const {return m_iCityStateFriendshipModifier;};
	int GetCityStateCombatModifier() const {return m_iCityStateCombatModifier;};
	int GetLandBarbarianConversionPercent() const {return m_iLandBarbarianConversionPercent;};
	int GetLandBarbarianConversionExtraUnits() const {return m_iLandBarbarianConversionExtraUnits;};
	int GetSeaBarbarianConversionPercent() const {return m_iSeaBarbarianConversionPercent;};
	int GetCapitalBuildingModifier() const {return m_iCapitalBuildingModifier;};
	int GetPlotBuyCostModifier() const {return m_iPlotBuyCostModifier;};
	int GetPlotCultureCostModifier() const {return m_iPlotCultureCostModifier;};
	int GetCultureFromKills() const {return m_iCultureFromKills;};
	int GetCityCultureBonus() const {return m_iCityCultureBonus;};
	int GetPolicyCostModifier() const {return m_iPolicyCostModifier;};
	int GetTradeRouteChange() const {return m_iTradeRouteChange;};
	int GetWonderProductionModifier() const {return m_iWonderProductionModifier;};
	int GetPlunderModifier() const {return m_iPlunderModifier;};
	int GetImprovementMaintenanceModifier() const {return m_iImprovementMaintenanceModifier;};
	int GetGoldenAgeDurationModifier() const {return m_iGoldenAgeDurationModifier;};
	int GetGoldenAgeMoveChange() const {return m_iGoldenAgeMoveChange;};
	int GetGoldenAgeCombatModifier() const {return m_iGoldenAgeCombatModifier;};
	int GetExtraEmbarkMoves() const {return m_iExtraEmbarkMoves;};
	int GetFirstFreeUnit(TechTypes eTech);
	int GetNextFreeUnit();
	int GetNaturalWonderFirstFinderGold() const {return m_iNaturalWonderFirstFinderGold;};
	int GetNaturalWonderSubsequentFinderGold() const {return m_iNaturalWonderSubsequentFinderGold;};
	int GetNaturalWonderYieldModifier() const {return m_iNaturalWonderYieldModifier;};
	int GetNaturalWonderHappinessModifier() const {return m_iNaturalWonderHappinessModifier;};
	int GetNearbyImprovementCombatBonus() const {return m_iNearbyImprovementCombatBonus;};
	int GetNearbyImprovementBonusRange() const {return m_iNearbyImprovementBonusRange;};
	int GetCultureBuildingYieldChange() const {return m_iCultureBuildingYieldChange;};
	int GetLandUnitMaintenanceModifier() const {return m_iLandUnitMaintenanceModifier;};
	int GetNavalUnitMaintenanceModifier() const {return m_iNavalUnitMaintenanceModifier;};

	bool IsFightWellDamaged() const {return m_bFightWellDamaged;};
	bool IsMoveFriendlyWoodsAsRoad() const {return m_bMoveFriendlyWoodsAsRoad;};
	bool IsFasterAlongRiver() const {return m_bFasterAlongRiver;};
	bool IsFasterInHills() const {return m_bFasterInHills;};
	bool IsEmbarkedNotCivilian() const {return m_bEmbarkedNotCivilian;};
	bool IsEmbarkedAllWater() const {return m_bEmbarkedAllWater;};
	bool IsEmbarkedToLandFlatCost() const {return m_bEmbarkedToLandFlatCost;};
	bool IsNoHillsImprovementMaintenance() const {return m_bNoHillsImprovementMaintenance;};
	bool IsTechBoostFromCapitalScienceBuildings() const {return m_bTechBoostFromCapitalScienceBuildings;};
	bool IsStaysAliveZeroCities() const {return m_bStaysAliveZeroCities;};

	int GetExtraYieldThreshold(YieldTypes eYield) const {return m_iExtraYieldThreshold[(int)eYield];};
	int GetFreeCityYield(YieldTypes eYield) const {return m_iFreeCityYield[(int)eYield];};
	int GetYieldChangeStrategicResources(YieldTypes eYield) const {return m_iYieldChangeStrategicResources[(int)eYield];};
	int GetYieldRateModifier(YieldTypes eYield) const {return m_iYieldRateModifier[(int)eYield];};
	int GetStrategicResourceQuantityModifier(TerrainTypes eTerrain) const {return m_iStrategicResourceQuantityModifier[(int)eTerrain];};
	int GetResourceQuantityModifier(ResourceTypes eResource) const {return ((uint)eResource < m_aiResourceQuantityModifier.size())?m_aiResourceQuantityModifier[(int)eResource]:0;};
	int GetMovesChangeUnitCombat(const int unitCombatID) const;
	int GetImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYield) const;
	int GetSpecialistYieldChange(SpecialistTypes eSpecialist, YieldTypes eYield) const;

	bool HasFreePromotionUnitCombat(const int promotionID, const int unitCombatID) const;

	// Public functions to make trait-based game state changes
	bool CheckForBarbarianConversion(CvPlot *pPlot);
	int GetCapitalBuildingDiscount(BuildingTypes eBuilding);
	void SetDefeatedBarbarianCampGuardType(UnitTypes eType) {m_eCampGuardType = eType;};
	UnitTypes GetDefeatedBarbarianCampGuardType () const {return m_eCampGuardType;};
	void SetCombatBonusImprovementType(ImprovementTypes eType) {m_eCombatBonusImprovement = eType;};
	ImprovementTypes GetCombatBonusImprovementType () const {return m_eCombatBonusImprovement;};

	// Serialization
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

private:
	bool ConvertBarbarianCamp(CvPlot *pPlot);
	bool ConvertBarbarianNavalUnit(UnitHandle pUnit);

	CvTraitXMLEntries *m_pTraits;
	CvPlayer *m_pPlayer;

	// Cached data about this player's traits
	int m_iGreatPeopleRateModifier;
	int m_iGreatScientistRateModifier;
	int m_iGreatGeneralRateModifier;
	int m_iGreatGeneralExtraBonus;
	int m_iLevelExperienceModifier;
	int m_iMaxGlobalBuildingProductionModifier;
	int m_iMaxTeamBuildingProductionModifier;
	int m_iMaxPlayerBuildingProductionModifier;
	int m_iCityUnhappinessModifier;
	int m_iPopulationUnhappinessModifier;
	int m_iCityStateBonusModifier;
	int m_iCityStateFriendshipModifier;
	int m_iCityStateCombatModifier;
	int m_iLandBarbarianConversionPercent;
	int m_iLandBarbarianConversionExtraUnits;
	int m_iSeaBarbarianConversionPercent;
	int m_iCapitalBuildingModifier;
	int m_iPlotBuyCostModifier;
	int m_iPlotCultureCostModifier;
	int m_iCultureFromKills;
	int m_iCityCultureBonus;
	int m_iPolicyCostModifier;
	int m_iTradeRouteChange;
	int m_iWonderProductionModifier;
	int m_iPlunderModifier;
	int m_iImprovementMaintenanceModifier;
	int m_iGoldenAgeDurationModifier;
	int m_iGoldenAgeMoveChange;
	int m_iGoldenAgeCombatModifier;
	int m_iExtraEmbarkMoves;
	int m_iNaturalWonderFirstFinderGold;
	int m_iNaturalWonderSubsequentFinderGold;
	int m_iNaturalWonderYieldModifier;
	int m_iNaturalWonderHappinessModifier;
	int m_iNearbyImprovementCombatBonus;
	int m_iNearbyImprovementBonusRange;
	int m_iCultureBuildingYieldChange;
	int m_iLandUnitMaintenanceModifier;
	int m_iNavalUnitMaintenanceModifier;

	bool m_bFightWellDamaged;
	bool m_bMoveFriendlyWoodsAsRoad;
	bool m_bFasterAlongRiver;
	bool m_bFasterInHills;
	bool m_bEmbarkedNotCivilian;
	bool m_bEmbarkedAllWater;
	bool m_bEmbarkedToLandFlatCost;
	bool m_bNoHillsImprovementMaintenance;
	bool m_bTechBoostFromCapitalScienceBuildings;
	bool m_bStaysAliveZeroCities;

	UnitTypes m_eCampGuardType;
	unsigned int m_uiFreeUnitIndex;
	TechTypes m_eFreeUnitPrereqTech;
	ImprovementTypes m_eCombatBonusImprovement;

	int m_iExtraYieldThreshold[NUM_YIELD_TYPES];
	int m_iFreeCityYield[NUM_YIELD_TYPES];
	int m_iYieldChangeStrategicResources[NUM_YIELD_TYPES];
	int m_iYieldRateModifier[NUM_YIELD_TYPES];
	int m_iStrategicResourceQuantityModifier[NUM_TERRAIN_TYPES];
	std::vector<int> m_aiResourceQuantityModifier;
	FStaticVector<FreeTraitUnit, SAFE_ESTIMATE_NUM_FREE_UNITS, true, c_eCiv5GameplayDLL, 0> m_aFreeTraitUnits;

	std::vector<int> m_paiMovesChangeUnitCombat;

	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppaaiImprovementYieldChange;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppaaiSpecialistYieldChange;
};

#endif //CIV5_TRAIT_CLASSES_H