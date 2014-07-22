/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_TRAIT_CLASSES_H
#define CIV5_TRAIT_CLASSES_H

struct FreeResourceXCities
{
	FreeResourceXCities():
		m_iNumCities(0),
		m_iResourceQuantity(0)
	{};

	int m_iNumCities;
	int m_iResourceQuantity;
};

struct MayaBonusChoice
{
	MayaBonusChoice():
		m_eUnitType(NO_UNIT),
		m_iBaktunJustFinished(0)
	{};

	MayaBonusChoice(const MayaBonusChoice& in)
	{
		m_eUnitType = in.m_eUnitType;
		m_iBaktunJustFinished = in.m_iBaktunJustFinished;
	};

	UnitTypes m_eUnitType;
	int m_iBaktunJustFinished;
};

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
	int GetGreatPersonGiftInfluence() const;
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
#if defined(MOD_TRAITS_CITY_WORKING)
	int GetCityWorkingChange() const;
#endif
	int GetPlotCultureCostModifier() const;
	int GetCultureFromKills() const;
	int GetFaithFromKills() const;
	int GetCityCultureBonus() const;
	int GetCapitalThemingBonusModifier() const;
	int GetPolicyCostModifier() const;
	int GetCityConnectionTradeRouteChange() const;
	int GetWonderProductionModifier() const;
	int GetPlunderModifier() const;
	int GetImprovementMaintenanceModifier() const;
	int GetGoldenAgeDurationModifier() const;
	int GetGoldenAgeMoveChange() const;
	int GetGoldenAgeCombatModifier() const;
	int GetGoldenAgeTourismModifier() const;
	int GetGoldenAgeGreatArtistRateModifier() const;
	int GetGoldenAgeGreatMusicianRateModifier() const;
	int GetGoldenAgeGreatWriterRateModifier() const;
	int GetExtraEmbarkMoves() const;
	int GetFreeUnitClassType() const;
	int GetNaturalWonderFirstFinderGold() const;
	int GetNaturalWonderSubsequentFinderGold() const;
	int GetNaturalWonderYieldModifier() const;
	int GetNaturalWonderHappinessModifier() const;
	int GetNearbyImprovementCombatBonus() const;
	int GetNearbyImprovementBonusRange() const;
	int GetCultureBuildingYieldChange() const;
	int GetCombatBonusVsHigherTech() const;
	int GetCombatBonusVsLargerCiv() const;
	int GetLandUnitMaintenanceModifier() const;
	int GetNavalUnitMaintenanceModifier() const;
	int GetRazeSpeedModifier() const;
	int GetDOFGreatPersonModifier() const;
	int GetLuxuryHappinessRetention() const;
	int GetExtraSpies() const;
	int GetUnresearchedTechBonusFromKills() const;
	int GetExtraFoundedCityTerritoryClaimRange() const;
	int GetFreeSocialPoliciesPerEra() const;
	int GetNumTradeRoutesModifier() const;
	int GetTradeRouteResourceModifier() const;
	int GetUniqueLuxuryCities() const;
	int GetUniqueLuxuryQuantity() const;
	int GetWorkerSpeedModifier() const;
	int GetAfraidMinorPerTurnInfluence() const;
	int GetLandTradeRouteRangeBonus() const;
#if defined(MOD_TRAITS_TRADE_ROUTE_BONUSES)
	int GetSeaTradeRouteRangeBonus() const;
#endif
	int GetTradeReligionModifier() const;
	int GetTradeBuildingModifier() const;
#if defined(MOD_BALANCE_CORE)
	int GetNumFreeBuildings() const;
	TechTypes GetFreeBuildingPrereqTech() const;
	TechTypes GetCapitalFreeBuildingPrereqTech() const;
#endif

	TechTypes GetFreeUnitPrereqTech() const;
	ImprovementTypes GetCombatBonusImprovement() const;
	BuildingTypes GetFreeBuilding() const;
#if defined(MOD_BALANCE_CORE)
	BuildingTypes GetFreeCapitalBuilding() const;
#endif
	BuildingTypes GetFreeBuildingOnConquest() const;

	bool IsFightWellDamaged() const;
	bool IsMoveFriendlyWoodsAsRoad() const;
	bool IsFasterAlongRiver() const;
	bool IsFasterInHills() const;
	bool IsEmbarkedAllWater() const;
	bool IsEmbarkedToLandFlatCost() const;
	bool IsNoHillsImprovementMaintenance() const;
	bool IsTechBoostFromCapitalScienceBuildings() const;
	bool IsStaysAliveZeroCities() const;
	bool IsFaithFromUnimprovedForest() const;
#if defined(MOD_TRAITS_ANY_BELIEF)
	bool IsAnyBelief() const;
#endif
	bool IsBonusReligiousBelief() const;
	bool IsAbleToAnnexCityStates() const;
	bool IsCrossesMountainsAfterGreatGeneral() const;
#if defined(MOD_TRAITS_CROSSES_ICE)
	bool IsCrossesIce() const;
#endif
	bool IsMayaCalendarBonuses() const;
	bool IsNoAnnexing() const;
	bool IsTechFromCityConquer() const;
	bool IsUniqueLuxuryRequiresNewArea() const;
	bool IsRiverTradeRoad() const;
	bool IsAngerFreeIntrusionOfCityStates() const;

	const char* getShortDescription() const;
	void setShortDescription(const char* szVal);
#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
	int GetPovertyHappinessChange() const;
	int GetDefenseHappinessChange() const;
	int GetIlliteracyHappinessChange() const;
	int GetMinorityHappinessChange() const;
	bool IsNoConnectionUnhappiness() const;
	bool IsNoReligiousStrife() const;
#endif

	// Arrays
	int GetExtraYieldThreshold(int i) const;
	int GetYieldChange(int i) const;
	int GetYieldChangeStrategicResources(int i) const;
	int GetYieldChangeNaturalWonder(int i) const;
	int GetYieldChangePerTradePartner(int i) const;
	int GetYieldChangeIncomingTradeRoute(int i) const;
	int GetYieldModifier(int i) const;
	int GetStrategicResourceQuantityModifier(int i) const;
	int GetObsoleteTech() const;
	int GetPrereqTech() const;
	int GetResourceQuantityModifier(int i) const;
	int GetMovesChangeUnitCombat(const int unitCombatID) const;
	int GetMaintenanceModifierUnitCombat(const int unitCombatID) const;
	int GetImprovementYieldChanges(ImprovementTypes eIndex1, YieldTypes eIndex2) const;
	int GetSpecialistYieldChanges(SpecialistTypes eIndex1, YieldTypes eIndex2) const;
	int GetUnimprovedFeatureYieldChanges(FeatureTypes eIndex1, YieldTypes eIndex2) const;
	FreeResourceXCities GetFreeResourceXCities(ResourceTypes eResource) const;

	bool IsFreePromotionUnitCombat(const int promotionID, const int unitCombatID) const;
	bool IsObsoleteByTech(TeamTypes eTeam);
	bool IsEnabledByTech(TeamTypes eTeam);

	bool NoTrain(UnitClassTypes eUnitClassType);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iLevelExperienceModifier;
	int m_iGreatPeopleRateModifier;
	int m_iGreatScientistRateModifier;
	int m_iGreatGeneralRateModifier;
	int m_iGreatGeneralExtraBonus;
	int m_iGreatPersonGiftInfluence;
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
#if defined(MOD_TRAITS_CITY_WORKING)
	int m_iCityWorkingChange;
#endif
	int m_iPlotCultureCostModifier;
	int m_iCultureFromKills;
	int m_iFaithFromKills;
	int m_iCityCultureBonus;
	int m_iCapitalThemingBonusModifier;
	int m_iPolicyCostModifier;
	int m_iCityConnectionTradeRouteChange;
	int m_iWonderProductionModifier;
	int m_iPlunderModifier;
	int m_iImprovementMaintenanceModifier;
	int m_iGoldenAgeDurationModifier;
	int m_iGoldenAgeMoveChange;
	int m_iGoldenAgeCombatModifier;
	int m_iGoldenAgeTourismModifier;
	int m_iGoldenAgeGreatArtistRateModifier;
	int m_iGoldenAgeGreatMusicianRateModifier;
	int m_iGoldenAgeGreatWriterRateModifier;
	int m_iObsoleteTech;
	int m_iPrereqTech;
	int m_iExtraEmbarkMoves;
	int m_iFreeUnitClassType;
	int m_iNaturalWonderFirstFinderGold;
	int m_iNaturalWonderSubsequentFinderGold;
	int m_iNaturalWonderYieldModifier;
	int m_iNaturalWonderHappinessModifier;
	int m_iNearbyImprovementCombatBonus;
	int m_iNearbyImprovementBonusRange;
	int m_iCultureBuildingYieldChange;
	int m_iCombatBonusVsHigherTech;
	int m_iCombatBonusVsLargerCiv;
	int m_iLandUnitMaintenanceModifier;
	int m_iNavalUnitMaintenanceModifier;
	int m_iRazeSpeedModifier;
	int m_iDOFGreatPersonModifier;
	int m_iLuxuryHappinessRetention;
	int m_iExtraSpies;
	int m_iUnresearchedTechBonusFromKills;
	int m_iExtraFoundedCityTerritoryClaimRange;
	int m_iFreeSocialPoliciesPerEra;
	int m_iNumTradeRoutesModifier;
	int m_iTradeRouteResourceModifier;
	int m_iUniqueLuxuryCities;
	int m_iUniqueLuxuryQuantity;
	int m_iWorkerSpeedModifier;
	int m_iAfraidMinorPerTurnInfluence;
	int m_iLandTradeRouteRangeBonus;
#if defined(MOD_TRAITS_TRADE_ROUTE_BONUSES)
	int m_iSeaTradeRouteRangeBonus;
#endif
	int m_iTradeReligionModifier;
	int m_iTradeBuildingModifier;
#if defined(MOD_BALANCE_CORE)
	int m_iNumFreeBuildings;
#endif

	TechTypes m_eFreeUnitPrereqTech;
	ImprovementTypes m_eCombatBonusImprovement;
	BuildingTypes m_eFreeBuilding;
#if defined(MOD_BALANCE_CORE)
	BuildingTypes m_eFreeCapitalBuilding;
	TechTypes m_eFreeBuildingPrereqTech;
	TechTypes m_eCapitalFreeBuildingPrereqTech;
#endif
	BuildingTypes m_eFreeBuildingOnConquest;

	bool m_bFightWellDamaged;
	bool m_bMoveFriendlyWoodsAsRoad;
	bool m_bFasterAlongRiver;
	bool m_bFasterInHills;
	bool m_bEmbarkedAllWater;
	bool m_bEmbarkedToLandFlatCost;
	bool m_bNoHillsImprovementMaintenance;
	bool m_bTechBoostFromCapitalScienceBuildings;
	bool m_bStaysAliveZeroCities;
	bool m_bFaithFromUnimprovedForest;
#if defined(MOD_TRAITS_ANY_BELIEF)
	bool m_bAnyBelief;
#endif
	bool m_bBonusReligiousBelief;
	bool m_bAbleToAnnexCityStates;
	bool m_bCrossesMountainsAfterGreatGeneral;
#if defined(MOD_TRAITS_CROSSES_ICE)
	bool m_bCrossesIce;
#endif
	bool m_bMayaCalendarBonuses;
	bool m_bNoAnnexing;
	bool m_bTechFromCityConquer;
	bool m_bUniqueLuxuryRequiresNewArea;
	bool m_bRiverTradeRoad;
	bool m_bAngerFreeIntrusionOfCityStates;
#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
	int m_iPovertyHappinessChange;
	int m_iDefenseHappinessChange;
	int m_iIlliteracyHappinessChange;
	int m_iMinorityHappinessChange;
	bool m_bNoConnectionUnhappiness;
	bool m_bIsNoReligiousStrife;
#endif

	CvString m_strShortDescription;

	// Arrays
	int* m_paiExtraYieldThreshold;
	int* m_paiYieldChange;
	int* m_paiYieldChangeStrategicResources;
	int* m_paiYieldChangeNaturalWonder;
	int* m_paiYieldChangePerTradePartner;
	int* m_paiYieldChangeIncomingTradeRoute;
	int* m_paiYieldModifier;
	int* m_piStrategicResourceQuantityModifier;
	int* m_piResourceQuantityModifiers;
	int* m_piMovesChangeUnitCombats;
	int* m_piMaintenanceModifierUnitCombats;
	int** m_ppiImprovementYieldChanges;
	int** m_ppiSpecialistYieldChanges;
	int** m_ppiUnimprovedFeatureYieldChanges;

	std::multimap<int, int> m_FreePromotionUnitCombats;
	std::vector<FreeResourceXCities> m_aFreeResourceXCities;
	std::vector<bool> m_abNoTrainUnitClass;

private:
	CvTraitEntry(const CvTraitEntry&);
	CvTraitEntry& operator=(const CvTraitEntry&);
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
	CvTraitEntry* GetEntry(int index);

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
	void Init(CvTraitXMLEntries* pTraits, CvPlayer* pPlayer);
	void Uninit();
	void Reset();
	void InitPlayerTraits();

	// Accessor functions
	bool HasTrait(TraitTypes eTrait) const;
	int GetGreatPeopleRateModifier() const
	{
		return m_iGreatPeopleRateModifier;
	};
	int GetGreatScientistRateModifier() const
	{
		return m_iGreatScientistRateModifier;
	};
	int GetGreatGeneralRateModifier() const
	{
		return m_iGreatGeneralRateModifier;
	};
	int GetGreatGeneralExtraBonus() const
	{
		return m_iGreatGeneralExtraBonus;
	};
	int GetGreatPersonGiftInfluence() const
	{
		return m_iGreatPersonGiftInfluence;
	};
	int GetLevelExperienceModifier() const
	{
		return m_iLevelExperienceModifier;
	};
	int GetMaxGlobalBuildingProductionModifier() const
	{
		return m_iMaxGlobalBuildingProductionModifier;
	};
	int GetMaxTeamBuildingProductionModifier() const
	{
		return m_iMaxTeamBuildingProductionModifier;
	};
	int GetMaxPlayerBuildingProductionModifier() const
	{
		return m_iMaxPlayerBuildingProductionModifier;
	};
	int GetCityUnhappinessModifier() const
	{
		return m_iCityUnhappinessModifier;
	};
	int GetPopulationUnhappinessModifier() const
	{
		return m_iPopulationUnhappinessModifier;
	};
	int GetCityStateBonusModifier() const
	{
		return m_iCityStateBonusModifier;
	};
	int GetCityStateFriendshipModifier() const
	{
		return m_iCityStateFriendshipModifier;
	};
	int GetCityStateCombatModifier() const
	{
		return m_iCityStateCombatModifier;
	};
	int GetLandBarbarianConversionPercent() const
	{
		return m_iLandBarbarianConversionPercent;
	};
	int GetLandBarbarianConversionExtraUnits() const
	{
		return m_iLandBarbarianConversionExtraUnits;
	};
	int GetSeaBarbarianConversionPercent() const
	{
		return m_iSeaBarbarianConversionPercent;
	};
	int GetCapitalBuildingModifier() const
	{
		return m_iCapitalBuildingModifier;
	};
	int GetPlotBuyCostModifier() const
	{
		return m_iPlotBuyCostModifier;
	};
#if defined(MOD_TRAITS_CITY_WORKING)
	int GetCityWorkingChange() const
	{
		return m_iCityWorkingChange;
	};
#endif
	int GetPlotCultureCostModifier() const
	{
		return m_iPlotCultureCostModifier;
	};
	int GetCultureFromKills() const
	{
		return m_iCultureFromKills;
	};
	int GetFaithFromKills() const
	{
		return m_iFaithFromKills;
	};
	int GetCityCultureBonus() const
	{
		return m_iCityCultureBonus;
	};
	int GetCapitalThemingBonusModifier() const
	{
		return m_iCapitalThemingBonusModifier;
	}
	int GetPolicyCostModifier() const
	{
		return m_iPolicyCostModifier;
	};
	int GetCityConnectionTradeRouteChange() const
	{
		return m_iCityConnectionTradeRouteChange;
	};
	int GetWonderProductionModifier() const
	{
		return m_iWonderProductionModifier;
	};
	int GetPlunderModifier() const
	{
		return m_iPlunderModifier;
	};
	int GetImprovementMaintenanceModifier() const
	{
		return m_iImprovementMaintenanceModifier;
	};
	int GetGoldenAgeDurationModifier() const
	{
		return m_iGoldenAgeDurationModifier;
	};
	int GetGoldenAgeMoveChange() const
	{
		return m_iGoldenAgeMoveChange;
	};
	int GetGoldenAgeCombatModifier() const
	{
		return m_iGoldenAgeCombatModifier;
	};
	int GetGoldenAgeTourismModifier() const
	{
		return m_iGoldenAgeTourismModifier;
	};
	int GetGoldenAgeGreatArtistRateModifier() const
	{
		return m_iGoldenAgeGreatArtistRateModifier;
	};
	int GetGoldenAgeGreatMusicianRateModifier() const
	{
		return m_iGoldenAgeGreatMusicianRateModifier;
	};
	int GetGoldenAgeGreatWriterRateModifier() const
	{
		return m_iGoldenAgeGreatWriterRateModifier;
	};
	int GetExtraEmbarkMoves() const
	{
		return m_iExtraEmbarkMoves;
	};
	int GetFirstFreeUnit(TechTypes eTech);
	int GetNextFreeUnit();
	int GetNaturalWonderFirstFinderGold() const
	{
		return m_iNaturalWonderFirstFinderGold;
	};
	int GetNaturalWonderSubsequentFinderGold() const
	{
		return m_iNaturalWonderSubsequentFinderGold;
	};
	int GetNaturalWonderYieldModifier() const
	{
		return m_iNaturalWonderYieldModifier;
	};
	int GetNaturalWonderHappinessModifier() const
	{
		return m_iNaturalWonderHappinessModifier;
	};
	int GetNearbyImprovementCombatBonus() const
	{
		return m_iNearbyImprovementCombatBonus;
	};
	int GetNearbyImprovementBonusRange() const
	{
		return m_iNearbyImprovementBonusRange;
	};
	int GetCultureBuildingYieldChange() const
	{
		return m_iCultureBuildingYieldChange;
	};
	int GetCombatBonusVsHigherTech() const
	{
		return m_iCombatBonusVsHigherTech;
	};
	int GetCombatBonusVsLargerCiv() const
	{
		return m_iCombatBonusVsLargerCiv;
	};
	int GetLandUnitMaintenanceModifier() const
	{
		return m_iLandUnitMaintenanceModifier;
	};
	int GetNavalUnitMaintenanceModifier() const
	{
		return m_iNavalUnitMaintenanceModifier;
	};
	int GetRazeSpeedModifier() const
	{
		return m_iRazeSpeedModifier;
	};
	int GetDOFGreatPersonModifier() const
	{
		return m_iDOFGreatPersonModifier;
	};
	int GetLuxuryHappinessRetention() const
	{
		return m_iLuxuryHappinessRetention;
	};
	int GetExtraSpies() const
	{
		return m_iExtraSpies;
	};
	int GetUnresearchedTechBonusFromKills() const
	{
		return m_iUnresearchedTechBonusFromKills;
	}
	int GetExtraFoundedCityTerritoryClaimRange () const
	{
		return m_iExtraFoundedCityTerritoryClaimRange;
	}
	int GetFreeSocialPoliciesPerEra() const
	{
		return m_iFreeSocialPoliciesPerEra;
	}
	int GetNumTradeRoutesModifier() const
	{
		return m_iNumTradeRoutesModifier;
	}
	int GetTradeRouteResourceModifier() const
	{
		return m_iTradeRouteResourceModifier;
	}
	int GetUniqueLuxuryCities() const
	{
		return m_iUniqueLuxuryCities;
	}
	int GetUniqueLuxuryQuantity() const
	{
		return m_iUniqueLuxuryQuantity;
	}
	int GetWorkerSpeedModifier() const
	{
		return m_iWorkerSpeedModifier;
	}
	int GetAfraidMinorPerTurnInfluence() const
	{
		return m_iAfraidMinorPerTurnInfluence;
	}
	int GetLandTradeRouteRangeBonus() const
	{
		return m_iLandTradeRouteRangeBonus;
	}
#if defined(MOD_TRAITS_TRADE_ROUTE_BONUSES)
	int GetSeaTradeRouteRangeBonus() const
	{
		return m_iSeaTradeRouteRangeBonus;
	}
#endif
	int GetTradeReligionModifier() const
	{
		return m_iTradeReligionModifier;
	}
	int GetTradeBuildingModifier() const
	{
		return m_iTradeBuildingModifier;
	}
#if defined(MOD_BALANCE_CORE)
	int GetNumFreeBuildings() const
	{
		return m_iNumFreeBuildings;
	}
#endif

	bool IsFightWellDamaged() const
	{
		return m_bFightWellDamaged;
	};
	bool IsMoveFriendlyWoodsAsRoad() const
	{
		return m_bMoveFriendlyWoodsAsRoad;
	};
	bool IsFasterAlongRiver() const
	{
		return m_bFasterAlongRiver;
	};
	bool IsFasterInHills() const
	{
		return m_bFasterInHills;
	};
	bool IsEmbarkedAllWater() const
	{
		return m_bEmbarkedAllWater;
	};
	bool IsEmbarkedToLandFlatCost() const
	{
		return m_bEmbarkedToLandFlatCost;
	};
	bool IsNoHillsImprovementMaintenance() const
	{
		return m_bNoHillsImprovementMaintenance;
	};
	bool IsTechBoostFromCapitalScienceBuildings() const
	{
		return m_bTechBoostFromCapitalScienceBuildings;
	};
	bool IsStaysAliveZeroCities() const
	{
		return m_bStaysAliveZeroCities;
	};
	bool IsFaithFromUnimprovedForest() const
	{
		return m_bFaithFromUnimprovedForest;
	};
#if defined(MOD_TRAITS_ANY_BELIEF)
	bool IsAnyBelief() const
	{
		return m_bAnyBelief;
	};
#endif
	bool IsBonusReligiousBelief() const
	{
		return m_bBonusReligiousBelief;
	};
	bool IsAbleToAnnexCityStates() const
	{
		return m_bAbleToAnnexCityStates;
	};
	bool IsCrossesMountainsAfterGreatGeneral() const
	{
		return m_bCrossesMountainsAfterGreatGeneral;
	};
#if defined(MOD_TRAITS_CROSSES_ICE)
	bool IsCrossesIce() const
	{
		return m_bCrossesIce;
	};
#endif
	bool IsMayaCalendarBonuses() const
	{
		return m_bMayaCalendarBonuses;
	};
	bool IsNoAnnexing() const
	{
		return m_bNoAnnexing;
	};
	bool IsTechFromCityConquer() const
	{
		return m_bTechFromCityConquer;
	};
	bool IsUniqueLuxuryRequiresNewArea() const
	{
		return m_bUniqueLuxuryRequiresNewArea;
	}
	bool WillGetUniqueLuxury(CvArea *pArea) const;
	bool IsRiverTradeRoad() const
	{
		return m_bRiverTradeRoad;
	}
	bool IsAngerFreeIntrusionOfCityStates() const
	{
		return m_bAngerFreeIntrusionOfCityStates;
	}
#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
	int GetPovertyHappinessChange() const
	{
		return m_iPovertyHappinessChange;
	}
	int GetDefenseHappinessChange() const
	{
		return m_iDefenseHappinessChange;
	}
	int GetIlliteracyHappinessChange() const
	{
		return m_iIlliteracyHappinessChange;
	}
	int GetMinorityHappinessChange() const
	{
		return m_iMinorityHappinessChange;
	}
	bool IsNoConnectionUnhappiness() const
	{
		return m_bNoConnectionUnhappiness;
	}
	bool IsNoReligiousStrife() const
	{
		return m_bIsNoReligiousStrife;
	}
#endif

	int GetExtraYieldThreshold(YieldTypes eYield) const
	{
		return m_iExtraYieldThreshold[(int)eYield];
	};
	int GetFreeCityYield(YieldTypes eYield) const
	{
		return m_iFreeCityYield[(int)eYield];
	};
	int GetYieldChangeStrategicResources(YieldTypes eYield) const
	{
		return m_iYieldChangeStrategicResources[(int)eYield];
	};
	int GetYieldChangeNaturalWonder(YieldTypes eYield) const
	{
		return m_iYieldChangeNaturalWonder[(int)eYield];
	};
	int GetYieldChangePerTradePartner(YieldTypes eYield) const
	{
		return m_iYieldChangePerTradePartner[(int)eYield];
	};
	int GetYieldChangeIncomingTradeRoute(YieldTypes eYield) const
	{
		return m_iYieldChangeIncomingTradeRoute[(int)eYield];
	};
	int GetYieldRateModifier(YieldTypes eYield) const
	{
		return m_iYieldRateModifier[(int)eYield];
	};
	int GetStrategicResourceQuantityModifier(TerrainTypes eTerrain) const
	{
		return m_iStrategicResourceQuantityModifier[(int)eTerrain];
	};
	int GetResourceQuantityModifier(ResourceTypes eResource) const
	{
		return ((uint)eResource < m_aiResourceQuantityModifier.size())?m_aiResourceQuantityModifier[(int)eResource]:0;
	};
	int GetMovesChangeUnitCombat(const int unitCombatID) const;
	int GetMaintenanceModifierUnitCombat(const int unitCombatID) const;
	int GetImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYield) const;
	int GetSpecialistYieldChange(SpecialistTypes eSpecialist, YieldTypes eYield) const;
	int GetUnimprovedFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYield) const;
	FreeResourceXCities GetFreeResourceXCities(ResourceTypes eResource) const;

	bool HasFreePromotionUnitCombat(const int promotionID, const int unitCombatID) const;

	// Public functions to make trait-based game state changes
	void AddUniqueLuxuries(CvCity *pCity);
	bool CheckForBarbarianConversion(CvPlot* pPlot);
	int GetCapitalBuildingDiscount(BuildingTypes eBuilding);
#if defined(MOD_BALANCE_CORE)
	TechTypes GetFreeBuildingPrereqTech() const;
	TechTypes GetCapitalFreeBuildingPrereqTech() const;
#endif
	BuildingTypes GetFreeBuilding() const;
#if defined(MOD_BALANCE_CORE)
	BuildingTypes GetFreeCapitalBuilding() const;
#endif
	BuildingTypes GetFreeBuildingOnConquest() const;
	void SetDefeatedBarbarianCampGuardType(UnitTypes eType)
	{
		m_eCampGuardType = eType;
	};
	UnitTypes GetDefeatedBarbarianCampGuardType() const
	{
		return m_eCampGuardType;
	};
	void SetCombatBonusImprovementType(ImprovementTypes eType)
	{
		m_eCombatBonusImprovement = eType;
	};
	ImprovementTypes GetCombatBonusImprovementType() const
	{
		return m_eCombatBonusImprovement;
	};
	bool IsAbleToCrossMountains() const;
#if defined(MOD_TRAITS_CROSSES_ICE)
	bool IsAbleToCrossIce() const;
#endif

	bool NoTrain(UnitClassTypes eUnitClassType);

	// Maya calendar routines
	bool IsUsingMayaCalendar() const;
	bool IsEndOfMayaLongCount();
	CvString GetMayaCalendarString();
	CvString GetMayaCalendarLongString();
	void ChooseMayaBoost();
	void ComputeMayaDate();
	int GetUnitBaktun(UnitTypes eUnit) const;
	void SetUnitBaktun(UnitTypes eUnit);
	bool IsFreeMayaGreatPersonChoice() const;

	// Serialization
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

private:
	bool ConvertBarbarianCamp(CvPlot* pPlot);
	bool ConvertBarbarianNavalUnit(UnitHandle pUnit);

	CvTraitXMLEntries* m_pTraits;
	CvPlayer* m_pPlayer;

	// Cached data about this player's traits
	int m_iGreatPeopleRateModifier;
	int m_iGreatScientistRateModifier;
	int m_iGreatGeneralRateModifier;
	int m_iGreatGeneralExtraBonus;
	int m_iGreatPersonGiftInfluence;
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
#if defined(MOD_TRAITS_CITY_WORKING)
	int m_iCityWorkingChange;
#endif
	int m_iPlotCultureCostModifier;
	int m_iCultureFromKills;
	int m_iFaithFromKills;
	int m_iCityCultureBonus;
	int m_iCapitalThemingBonusModifier;
	int m_iPolicyCostModifier;
	int m_iCityConnectionTradeRouteChange;
	int m_iWonderProductionModifier;
	int m_iPlunderModifier;
	int m_iImprovementMaintenanceModifier;
	int m_iGoldenAgeDurationModifier;
	int m_iGoldenAgeMoveChange;
	int m_iGoldenAgeCombatModifier;
	int m_iGoldenAgeTourismModifier;
	int m_iGoldenAgeGreatArtistRateModifier;
	int m_iGoldenAgeGreatMusicianRateModifier;
	int m_iGoldenAgeGreatWriterRateModifier;
	int m_iExtraEmbarkMoves;
	int m_iNaturalWonderFirstFinderGold;
	int m_iNaturalWonderSubsequentFinderGold;
	int m_iNaturalWonderYieldModifier;
	int m_iNaturalWonderHappinessModifier;
	int m_iNearbyImprovementCombatBonus;
	int m_iNearbyImprovementBonusRange;
	int m_iCultureBuildingYieldChange;
	int m_iCombatBonusVsHigherTech;
	int m_iCombatBonusVsLargerCiv;
	int m_iLandUnitMaintenanceModifier;
	int m_iNavalUnitMaintenanceModifier;
	int m_iRazeSpeedModifier;
	int m_iDOFGreatPersonModifier;
	int m_iLuxuryHappinessRetention;
	int m_iExtraSpies;
	int m_iUnresearchedTechBonusFromKills;
	int m_iExtraFoundedCityTerritoryClaimRange;
	int m_iFreeSocialPoliciesPerEra;
	int m_iNumTradeRoutesModifier;
	int m_iTradeRouteResourceModifier;
	int m_iUniqueLuxuryCities;
	int m_iUniqueLuxuryQuantity;
	int m_iUniqueLuxuryCitiesPlaced;  
	int m_iWorkerSpeedModifier;
	int m_iAfraidMinorPerTurnInfluence; 
	int m_iLandTradeRouteRangeBonus;
#if defined(MOD_TRAITS_TRADE_ROUTE_BONUSES)
	int m_iSeaTradeRouteRangeBonus;
#endif
	int m_iTradeReligionModifier;
	int m_iTradeBuildingModifier;
#if defined(MOD_BALANCE_CORE)
	int m_iNumFreeBuildings;
#endif
	// Saved

	bool m_bFightWellDamaged;
	bool m_bMoveFriendlyWoodsAsRoad;
	bool m_bFasterAlongRiver;
	bool m_bFasterInHills;
	bool m_bEmbarkedAllWater;
	bool m_bEmbarkedToLandFlatCost;
	bool m_bNoHillsImprovementMaintenance;
	bool m_bTechBoostFromCapitalScienceBuildings;
	bool m_bStaysAliveZeroCities;
	bool m_bFaithFromUnimprovedForest;
#if defined(MOD_TRAITS_ANY_BELIEF)
	bool m_bAnyBelief;
#endif
	bool m_bBonusReligiousBelief;
	bool m_bAbleToAnnexCityStates;
	bool m_bCrossesMountainsAfterGreatGeneral;
#if defined(MOD_TRAITS_CROSSES_ICE)
	bool m_bCrossesIce;
#endif
	bool m_bMayaCalendarBonuses;
	bool m_bNoAnnexing;
	bool m_bTechFromCityConquer;
	bool m_bUniqueLuxuryRequiresNewArea;
	bool m_bRiverTradeRoad;
	bool m_bAngerFreeIntrusionOfCityStates;
#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
	int m_iPovertyHappinessChange;
	int m_iDefenseHappinessChange;
	int m_iIlliteracyHappinessChange;
	int m_iMinorityHappinessChange;
	bool m_bNoConnectionUnhappiness;
	bool m_bIsNoReligiousStrife;
#endif

	UnitTypes m_eCampGuardType;
	unsigned int m_uiFreeUnitIndex;
	TechTypes m_eFreeUnitPrereqTech;
	ImprovementTypes m_eCombatBonusImprovement;
	BuildingTypes m_eFreeBuilding;
#if defined(MOD_BALANCE_CORE)
	BuildingTypes m_eFreeCapitalBuilding;
#endif
	BuildingTypes m_eFreeBuildingOnConquest;

	int m_iExtraYieldThreshold[NUM_YIELD_TYPES];
	int m_iFreeCityYield[NUM_YIELD_TYPES];
	int m_iYieldChangeStrategicResources[NUM_YIELD_TYPES];
	int m_iYieldChangeNaturalWonder[NUM_YIELD_TYPES];
	int m_iYieldChangePerTradePartner[NUM_YIELD_TYPES];
	int m_iYieldChangeIncomingTradeRoute[NUM_YIELD_TYPES];
	int m_iYieldRateModifier[NUM_YIELD_TYPES];
	int m_iStrategicResourceQuantityModifier[NUM_TERRAIN_TYPES];
	std::vector<int> m_aiResourceQuantityModifier;
	std::vector<bool> m_abNoTrain;
	FStaticVector<FreeTraitUnit, SAFE_ESTIMATE_NUM_FREE_UNITS, true, c_eCiv5GameplayDLL, 0> m_aFreeTraitUnits;
	std::vector<int> m_aUniqueLuxuryAreas;

	// Maya calendar bonus data
	int m_iBaktunPreviousTurn;  // Saved
	int m_iBaktun;	 // Not saved
	int m_iKatun;    // Not saved
	int m_iTun;      // Not saved
	int m_iWinal;    // Not saved
	int m_iKin;      // Not saved
	std::vector<MayaBonusChoice> m_aMayaBonusChoices;  // Saved

	std::vector<int> m_paiMovesChangeUnitCombat;
	std::vector<int> m_paiMaintenanceModifierUnitCombat;

	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppaaiImprovementYieldChange;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppaaiSpecialistYieldChange;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppaaiUnimprovedFeatureYieldChange;

	std::vector<FreeResourceXCities> m_aFreeResourceXCities;
};

#endif //CIV5_TRAIT_CLASSES_H