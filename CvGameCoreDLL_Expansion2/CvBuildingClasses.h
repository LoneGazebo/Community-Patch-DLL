/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_BUILDING_CLASSES_H
#define CIV5_BUILDING_CLASSES_H

#include "CvDatabaseUtility.h"

#define MAX_THEMING_BONUSES 12

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvThemingBonusInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvThemingBonusInfo
{
	friend class CvBuildingEntry;

public:
	CvThemingBonusInfo() :
	  m_iBonus(0),
	  m_bSameEra(false),
	  m_bUniqueEras(false),
#if defined(MOD_API_EXTENSIONS)
	  m_bConsecutiveEras(false),
#endif
	  m_bMustBeArt(false),
	  m_bMustBeArtifact(false),
	  m_bMustBeEqualArtArtifact(false),
	  m_bRequiresOwner(false),
	  m_bRequiresAnyButOwner(false),
	  m_bRequiresSamePlayer(false),
	  m_bRequiresUniquePlayers(false),
	  m_iAIPriority(0)
	  {

	  };

	int GetBonus() {return m_iBonus;};
	CvString GetDescription() {return m_strDescription;};
	bool IsSameEra() {return m_bSameEra;};
#if defined(MOD_API_EXTENSIONS)
	bool IsUniqueEras() {return m_bUniqueEras || IsConsecutiveEras();};
	bool IsConsecutiveEras() {return m_bConsecutiveEras;};
#else
	bool IsUniqueEras() {return m_bUniqueEras;};
#endif
	bool IsMustBeArt() {return m_bMustBeArt;};
	bool IsMustBeArtifact() {return m_bMustBeArtifact;};
	bool IsMustBeEqualArtArtifact() {return m_bMustBeEqualArtArtifact;};
	bool IsRequiresOwner() {return m_bRequiresOwner;};
	bool IsRequiresAnyButOwner() {return m_bRequiresAnyButOwner;};
	bool IsRequiresSamePlayer() {return m_bRequiresSamePlayer;};
	bool IsRequiresUniquePlayers() {return m_bRequiresUniquePlayers;};
	int GetAIPriority() {return m_iAIPriority;};

protected:
	int m_iBonus;
	CvString m_strDescription;
	bool m_bSameEra;
	bool m_bUniqueEras;
#if defined(MOD_API_EXTENSIONS)
	bool m_bConsecutiveEras;
#endif
	bool m_bMustBeArt;
	bool m_bMustBeArtifact;
	bool m_bMustBeEqualArtArtifact;
	bool m_bRequiresOwner;
	bool m_bRequiresAnyButOwner;
	bool m_bRequiresSamePlayer;
	bool m_bRequiresUniquePlayers;
	int m_iAIPriority;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvBuildingEntry
//!  \brief		A single building available in the game
//
//!  Key Attributes:
//!  - Used to be called CvBuildingInfo
//!  - Populated from XML\Buildings\CIV5BuildingInfos.xml
//!  - Array of these contained in CvBuildingXMLEntries class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBuildingEntry: public CvBaseInfo
{
public:
	CvBuildingEntry(void);
	~CvBuildingEntry(void);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	// Accessor Functions (Non-Arrays)
	int GetBuildingClassType() const;
	const CvBuildingClassInfo& GetBuildingClassInfo() const;

	int GetNearbyTerrainRequired() const;
	int GetProhibitedCityTerrain() const;
	int GetVictoryPrereq() const;
	int GetFreeStartEra() const;
	int GetMaxStartEra() const;
	int GetObsoleteTech() const;
	int GetEnhancedYieldTech() const;
	int GetTechEnhancedTourism() const;
	int GetGoldMaintenance() const;
	int GetMutuallyExclusiveGroup() const;
	int GetReplacementBuildingClass() const;
	int GetPrereqAndTech() const;
	int GetPolicyBranchType() const;
	int GetSpecialistType() const;
	int GetSpecialistCount() const;
	int GetSpecialistExtraCulture() const;
	int GetGreatPeopleRateChange() const;
	GreatWorkSlotType GetGreatWorkSlotType() const;
#if defined(MOD_GLOBAL_GREATWORK_YIELDTYPES)
	YieldTypes GetGreatWorkYieldType() const;
#endif
	int GetGreatWorkCount() const;
	GreatWorkType GetFreeGreatWork() const;
	int GetFreeBuildingClass() const;
	int GetFreeBuildingThisCity() const;
	int GetFreePromotion() const;
	int GetTrainedFreePromotion() const;
	int GetFreePromotionRemoved() const;
	int GetProductionCost() const;
	int GetFaithCost() const;
	int GetLeagueCost() const;
	int GetNumCityCostMod() const;
	int GetHurryCostModifier() const;
	int GetNumCitiesPrereq() const;
	int GetUnitLevelPrereq() const;
	int GetCultureRateModifier() const;
	int GetGlobalCultureRateModifier() const;
	int GetGreatPeopleRateModifier() const;
	int GetGlobalGreatPeopleRateModifier() const;
	int GetGreatGeneralRateModifier() const;
	int GetGreatPersonExpendGold() const;
	int GetUnitUpgradeCostMod() const;
	int GetGoldenAgeModifier() const;
	int GetFreeExperience() const;
	int GetGlobalFreeExperience() const;
	int GetFoodKept() const;
	bool IsAirlift() const;
	int GetAirModifier() const;
	int GetNukeModifier() const;
	int GetNukeExplosionRand() const;
	int GetWorkerSpeedModifier() const;
	int GetMilitaryProductionModifier() const;
	int GetSpaceProductionModifier() const;
	int GetGlobalSpaceProductionModifier() const;
	int GetBuildingProductionModifier() const;
	int GetWonderProductionModifier() const;
	int GetCityConnectionTradeRouteModifier() const;
	int GetCapturePlunderModifier() const;
	int GetPolicyCostModifier() const;
	int GetGlobalPlotCultureCostModifier() const;
	int GetPlotCultureCostModifier() const;
	int GetGlobalPlotBuyCostModifier() const;
	int GetPlotBuyCostModifier() const;
#if defined(MOD_BUILDINGS_CITY_WORKING)
	int GetGlobalCityWorkingChange() const;
	int GetCityWorkingChange() const;
#endif
#if defined(MOD_BALANCE_CORE)
	bool IsNoWater() const;
#endif
#if defined(MOD_BALANCE_CORE_POP_REQ_BUILDINGS)
	int GetNationalPopulationRequired() const;
	int GetLocalPopulationRequired() const;
#endif
	int GetMinAreaSize() const;
	int GetConquestProbability() const;
	int GetHealRateChange() const;
	int GetHappiness() const;
	int GetUnmoddedHappiness() const;
	int GetUnhappinessModifier() const;
	int GetHappinessPerCity() const;
	int GetHappinessPerXPolicies() const;
	int GetCityCountUnhappinessMod() const;
	bool IsNoOccupiedUnhappiness() const;
	int GetGlobalPopulationChange() const;
	int GetTechShare() const;
	int GetFreeTechs() const;
	int GetFreePolicies() const;
	int GetFreeGreatPeople() const;
	int GetMedianTechPercentChange() const;
	int GetGold() const;
	bool IsNearbyMountainRequired() const;
	bool IsAllowsRangeStrike() const;
	int GetDefenseModifier() const;
	int GetGlobalDefenseModifier() const;
	int GetExtraCityHitPoints() const;
	int GetMinorFriendshipChange() const;
	int GetVictoryPoints() const;
	int GetExtraMissionarySpreads() const;
	int GetReligiousPressureModifier() const;
	int GetEspionageModifier() const;
	int GetGlobalEspionageModifier() const;
	int GetExtraSpies() const;
	int GetSpyRankChange() const;
	int GetTradeRouteRecipientBonus() const;
	int GetTradeRouteTargetBonus() const;
	int GetNumTradeRouteBonus() const;
	int GetInstantSpyRankChange() const;
#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
	int GetConversionModifier() const;
	int GetGlobalConversionModifier() const;
#endif
	int GetLandmarksTourismPercent() const;
	int GetInstantMilitaryIncrease() const;
	int GetGreatWorksTourismModifier() const;
	int GetXBuiltTriggersIdeologyChoice() const;
	int GetTradeRouteSeaDistanceModifier() const;
	int GetTradeRouteSeaGoldBonus() const;
	int GetTradeRouteLandDistanceModifier() const;
	int GetTradeRouteLandGoldBonus() const;
	int GetCityStateTradeRouteProductionModifier() const;
	int GetGreatScientistBeakerModifier() const;
	int GetExtraLeagueVotes() const;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	int GetFaithToVotes() const;
	int GetCapitalsToVotes() const;
	int GetDoFToVotes() const;
	int GetRAToVotes() const;
	int GetGPExpendInfluence() const;
	int GetGrowthExtraYield() const;
#endif
#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
	int GetPovertyHappinessChangeBuilding() const;
	int GetDefenseHappinessChangeBuilding() const;
	int GetIlliteracyHappinessChangeBuilding() const;
	int GetMinorityHappinessChangeBuilding() const;
	int GetPovertyHappinessChangeBuildingGlobal() const;
	int GetDefenseHappinessChangeBuildingGlobal() const;
	int GetIlliteracyHappinessChangeBuildingGlobal() const;
	int GetMinorityHappinessChangeBuildingGlobal() const;
#endif
	int GetPreferredDisplayPosition() const;
	int GetPortraitIndex() const;
	bool IsTeamShare() const;
	bool IsWater() const;
	bool IsRiver() const;
	bool IsFreshWater() const;
#if defined(MOD_API_EXTENSIONS)
	bool IsAddsFreshWater() const;
	bool IsPurchaseOnly() const;
#endif
	bool IsMountain() const;
	bool IsHill() const;
	bool IsFlat() const;
	bool IsFoundsReligion() const;
	bool IsReligious() const;
	bool IsBorderObstacle() const;
	bool IsPlayerBorderObstacle() const;
	bool IsCityWall() const;
	bool IsCapital() const;
	bool IsGoldenAge() const;
	bool IsMapCentering() const;
	bool IsNeverCapture() const;
	bool IsNukeImmune() const;
	bool IsExtraLuxuries() const;
	bool IsDiplomaticVoting() const;
	bool AllowsWaterRoutes() const;
	bool IsScienceBuilding() const;
	bool IsUnlockedByBelief() const;
	bool IsUnlockedByLeague() const;
	bool IsRequiresHolyCity() const;
	bool AffectSpiesNow() const;
	bool IsEspionage() const;
	bool AllowsFoodTradeRoutes() const;
	bool AllowsProductionTradeRoutes() const;
	bool NullifyInfluenceModifier() const;

	const char* GetArtDefineTag() const;
	void SetArtDefineTag(const char* szVal);
	const bool GetArtInfoCulturalVariation() const;
	const bool GetArtInfoEraVariation() const;
	const bool GetArtInfoRandomVariation() const;

	const char* GetWonderSplashAudio() const;
	CvString GetThemingBonusHelp() const;

	// Accessor Functions (Arrays)
	int GetYieldChange(int i) const;
	int* GetYieldChangeArray() const;
	int GetYieldChangePerPop(int i) const;
	int* GetYieldChangePerPopArray() const;
	int GetYieldChangePerReligion(int i) const;
	int* GetYieldChangePerReligionArray() const;
	int GetYieldModifier(int i) const;;
	int* GetYieldModifierArray() const;
	int GetAreaYieldModifier(int i) const;
	int* GetAreaYieldModifierArray() const;
	int GetGlobalYieldModifier(int i) const;
	int* GetGlobalYieldModifierArray() const;
	int GetTechEnhancedYieldChange(int i) const;
	int* GetTechEnhancedYieldChangeArray() const;
	int GetSeaPlotYieldChange(int i) const;
	int* GetSeaPlotYieldChangeArray() const;
	int GetRiverPlotYieldChange(int i) const;
	int* GetRiverPlotYieldChangeArray() const;
	int GetLakePlotYieldChange(int i) const;
	int* GetLakePlotYieldChangeArray() const;
	int GetSeaResourceYieldChange(int i) const;
	int* GetSeaResourceYieldChangeArray() const;
	int GetSpecialistCount(int i) const;
	int GetFreeSpecialistCount(int i) const;
	int GetUnitCombatFreeExperience(int i) const;
	int GetUnitCombatProductionModifier(int i) const;
	int GetDomainFreeExperience(int i) const;
	int GetDomainFreeExperiencePerGreatWork(int i) const;
	int GetDomainProductionModifier(int i) const;
	int GetLockedBuildingClasses(int i) const;
	int GetPrereqAndTechs(int i) const;
	int GetResourceQuantityRequirement(int i) const;
	int GetResourceQuantity(int i) const;
	int GetResourceCultureChange(int i) const;
	int GetResourceFaithChange(int i) const;
	int GetProductionTraits(int i) const;
	int GetPrereqNumOfBuildingClass(int i) const;
	int GetFlavorValue(int i) const;
	int GetLocalResourceAnd(int i) const;
	int GetLocalResourceOr(int i) const;
	int GetHurryModifier(int i) const;
	bool IsBuildingClassNeededInCity(int i) const;
	int GetNumFreeUnits(int i) const;

#if defined(MOD_BALANCE_CORE_YIELDS)
	int GetPlotYieldChange(int i, int j) const;
	int* GetPlotYieldChangeArray(int i) const;
#endif

	int GetResourceYieldChange(int i, int j) const;
	int* GetResourceYieldChangeArray(int i) const;
	int GetFeatureYieldChange(int i, int j) const;
	int* GetFeatureYieldChangeArray(int i) const;
	int GetSpecialistYieldChange(int i, int j) const;
	int* GetSpecialistYieldChangeArray(int i) const;
	int GetResourceYieldModifier(int i, int j) const;
	int* GetResourceYieldModifierArray(int i) const;
	int GetTerrainYieldChange(int i, int j) const;
	int* GetTerrainYieldChangeArray(int i) const;
	int GetBuildingClassYieldChange(int i, int j) const;
	int GetBuildingClassHappiness(int i) const;

	CvThemingBonusInfo *GetThemingBonusInfo(int i) const;
	int GetNumThemingBonuses() const {return m_iNumThemingBonuses;};

private:
	int m_iBuildingClassType;
	const CvBuildingClassInfo* m_pkBuildingClassInfo;

	int m_iNearbyTerrainRequired;
	int m_iProhibitedCityTerrain;
	int m_iVictoryPrereq;
	int m_iFreeStartEra;
	int m_iMaxStartEra;
	int m_iObsoleteTech;
	int m_iEnhancedYieldTech;
	int m_iTechEnhancedTourism;
	int m_iGoldMaintenance;
	int m_iMutuallyExclusiveGroup;
	int m_iReplacementBuildingClass;
	int m_iPrereqAndTech;
	int m_iPolicyBranchType;
	int m_iSpecialistType;
	int m_iSpecialistCount;
	int m_iSpecialistExtraCulture;
	int m_iGreatPeopleRateChange;
	GreatWorkSlotType m_eGreatWorkSlotType;
#if defined(MOD_GLOBAL_GREATWORK_YIELDTYPES)
	YieldTypes m_eGreatWorkYieldType;
#endif
	int m_iGreatWorkCount;
	GreatWorkType m_eFreeGreatWork;
	int m_iFreeBuildingClass;
	int m_iFreeBuildingThisCity;
	int m_iFreePromotion;
	int m_iTrainedFreePromotion;
	int m_iFreePromotionRemoved;
	int m_iProductionCost;
	int m_iFaithCost;
	int m_iLeagueCost;
	int m_iNumCityCostMod;
	int m_iHurryCostModifier;
	int m_iNumCitiesPrereq;
	int m_iUnitLevelPrereq;
	int m_iCultureRateModifier;
	int m_iGlobalCultureRateModifier;
	int m_iGreatPeopleRateModifier;
	int m_iGlobalGreatPeopleRateModifier;
	int m_iGreatGeneralRateModifier;
	int m_iGreatPersonExpendGold;
	int m_iUnitUpgradeCostMod;
	int m_iGoldenAgeModifier;
	int m_iFreeExperience;
	int m_iGlobalFreeExperience;
	int m_iFoodKept;
	bool m_bAirlift;
	int m_iAirModifier;
	int m_iNukeModifier;
	int m_iNukeExplosionRand;
	int m_iWorkerSpeedModifier;
	int m_iMilitaryProductionModifier;
	int m_iSpaceProductionModifier;
	int m_iGlobalSpaceProductionModifier;
	int m_iBuildingProductionModifier;
	int m_iWonderProductionModifier;
	int m_iCityConnectionTradeRouteModifier;
	int m_iCapturePlunderModifier;
	int m_iPolicyCostModifier;
	int m_iGlobalPlotCultureCostModifier;
	int m_iPlotCultureCostModifier;
	int m_iGlobalPlotBuyCostModifier;
	int m_iPlotBuyCostModifier;
#if defined(MOD_BUILDINGS_CITY_WORKING)
	int m_iGlobalCityWorkingChange;
	int m_iCityWorkingChange;
#endif
	int m_iMinAreaSize;
	int m_iConquestProbability;
	int m_iHealRateChange;
	int m_iHappiness;
	int m_iUnmoddedHappiness;
	int m_iUnhappinessModifier;
	int m_iHappinessPerCity;
	int m_iHappinessPerXPolicies;
	int m_iCityCountUnhappinessMod;
	bool m_bNoOccupiedUnhappiness;
	int m_iGlobalPopulationChange;
	int m_iTechShare;
	int m_iFreeTechs;
	int m_iFreePolicies;
	int m_iFreeGreatPeople;
	int m_iMedianTechPercentChange;
	int m_iGold;
	bool m_bNearbyMountainRequired;
	bool m_bAllowsRangeStrike;
	int m_iDefenseModifier;
	int m_iGlobalDefenseModifier;
	int m_iExtraCityHitPoints;
	int m_iMissionType;
	int m_iMinorFriendshipChange;
	int m_iVictoryPoints;
	int m_iExtraMissionarySpreads;
	int m_iReligiousPressureModifier;
	int m_iEspionageModifier;
	int m_iGlobalEspionageModifier;
	int m_iExtraSpies;
	int m_iSpyRankChange;
	int m_iInstantSpyRankChange;

#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
	int m_iConversionModifier;
	int m_iGlobalConversionModifier;
#endif

	int m_iLandmarksTourismPercent;
	int m_iInstantMilitaryIncrease;
	int m_iGreatWorksTourismModifier;
	int m_iXBuiltTriggersIdeologyChoice;
	int m_iTradeRouteRecipientBonus;
	int m_iTradeRouteTargetBonus;
	int m_iNumTradeRouteBonus;
	int m_iTradeRouteSeaDistanceModifier;
	int m_iTradeRouteSeaGoldBonus;
	int m_iTradeRouteLandDistanceModifier;
	int m_iTradeRouteLandGoldBonus;
	int m_iCityStateTradeRouteProductionModifier;
	int m_iGreatScientistBeakerModifier;
	int m_iExtraLeagueVotes;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	int m_iFaithToVotesBase;
	int m_iCapitalsToVotesBase;
	int m_iDoFToVotesBase;
	int m_iRAToVotesBase;
	int m_iGPExpendInfluenceBase;
	int m_iGrowthExtraYieldBase;
#endif
#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
	int m_iPovertyHappinessChangeBuilding;
	int m_iDefenseHappinessChangeBuilding;
	int m_iIlliteracyHappinessChangeBuilding;
	int m_iMinorityHappinessChangeBuilding;
	int m_iPovertyHappinessChangeBuildingGlobal;
	int m_iDefenseHappinessChangeBuildingGlobal;
	int m_iIlliteracyHappinessChangeBuildingGlobal;
	int m_iMinorityHappinessChangeBuildingGlobal;
#endif
	int m_iPreferredDisplayPosition;
	int m_iPortraitIndex;

	bool m_bTeamShare;
	bool m_bWater;
	bool m_bRiver;
	bool m_bFreshWater;
#if defined(MOD_API_EXTENSIONS)
	bool m_bAddsFreshWater;
	bool m_bPurchaseOnly;
#endif
#if defined(MOD_BALANCE_CORE)
	bool m_bIsNoWater;
#endif
#if defined(MOD_BALANCE_CORE_POP_REQ_BUILDINGS)
	int m_iNationalPopRequired;
	int m_iLocalPopRequired;
#endif
	bool m_bMountain;
	bool m_bHill;
	bool m_bFlat;
	bool m_bFoundsReligion;
	bool m_bIsReligious;
	bool m_bBorderObstacle;
	bool m_bPlayerBorderObstacle;
	bool m_bCapital;
	bool m_bGoldenAge;
	bool m_bMapCentering;
	bool m_bNeverCapture;
	bool m_bNukeImmune;
	bool m_bExtraLuxuries;
	bool m_bDiplomaticVoting;
	bool m_bAllowsWaterRoutes;
	bool m_bCityWall;
	bool m_bUnlockedByBelief;
	bool m_bUnlockedByLeague;
	bool m_bRequiresHolyCity;
	bool m_bAffectSpiesNow;
	bool m_bEspionage;
	bool m_bAllowsFoodTradeRoutes;
	bool m_bAllowsProductionTradeRoutes;
	bool m_bNullifyInfluenceModifier;

	bool m_bArtInfoCulturalVariation;
	bool m_bArtInfoEraVariation;
	bool m_bArtInfoRandomVariation;

	CvString m_strArtDefineTag;
	CvString m_strWonderSplashAudio;
	CvString m_strThemingBonusHelp;

	// Arrays

	int* m_piLockedBuildingClasses;
	int* m_piPrereqAndTechs;
	int* m_piResourceQuantityRequirements;
	int* m_piResourceQuantity;
	int* m_piResourceCultureChanges;
	int* m_piResourceFaithChanges;
	int* m_piProductionTraits;
	int* m_piSeaPlotYieldChange;
	int* m_piRiverPlotYieldChange;
	int* m_piLakePlotYieldChange;
	int* m_piSeaResourceYieldChange;
	int* m_piYieldChange;
	int* m_piYieldChangePerPop;
	int* m_piYieldChangePerReligion;
	int* m_piYieldModifier;
	int* m_piAreaYieldModifier;
	int* m_piGlobalYieldModifier;
	int* m_piTechEnhancedYieldChange;
	int* m_piUnitCombatFreeExperience;
	int* m_piUnitCombatProductionModifiers;
	int* m_piDomainFreeExperience;
	int* m_piDomainFreeExperiencePerGreatWork;
	int* m_piDomainProductionModifier;
	int* m_piPrereqNumOfBuildingClass;
	int* m_piFlavorValue;
	int* m_piLocalResourceAnds;
	int* m_piLocalResourceOrs;
	int* m_paiHurryModifier;

	bool* m_pbBuildingClassNeededInCity;
	int* m_piNumFreeUnits;

	int** m_ppaiResourceYieldChange;
	int** m_ppaiFeatureYieldChange;
	int** m_ppaiSpecialistYieldChange;
	int** m_ppaiResourceYieldModifier;
	int** m_ppaiTerrainYieldChange;
	int** m_ppiBuildingClassYieldChanges;
	int* m_paiBuildingClassHappiness;
#if defined(MOD_BALANCE_CORE_YIELDS)
	int** m_ppaiBuildingPlotYieldChange;
#endif

	CvThemingBonusInfo* m_paThemingBonusInfo;
	int m_iNumThemingBonuses;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvBuildingXMLEntries
//!  \brief		Game-wide information about buildings
//
//! Key Attributes:
//! - Plan is it will be contained in CvGameRules object within CvGame class
//! - Populated from XML\Buildings\CIV5BuildingInfos.xml
//! - Contains an array of CvBuildingEntry from the above XML file
//! - One instance for the entire game
//! - Accessed heavily by CvCityBuildings class (which stores the building state for 1 city)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBuildingXMLEntries
{
public:
	CvBuildingXMLEntries(void);
	~CvBuildingXMLEntries(void);

	// Accessor functions
	std::vector<CvBuildingEntry*>& GetBuildingEntries();
	int GetNumBuildings();
	_Ret_maybenull_ CvBuildingEntry* GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvBuildingEntry*> m_paBuildingEntries;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvCityBuildings
//!  \brief		Information about the buildings constructed in a single city
//
//!  Key Attributes:
//!  - Plan is it will be contained in CvCityState object within CvCity class
//!  - One instance for each city
//!  - Accessed by any class that needs to check building construction status
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCityBuildings
{
public:
	CvCityBuildings(void);
	~CvCityBuildings(void);
	void Init(CvBuildingXMLEntries* pBuildings, CvCity* pCity);
	void Uninit();
	void Reset();
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

	// Accessor functions
	CvBuildingXMLEntries* GetBuildings() const;

	int GetNumBuildings() const;
	void ChangeNumBuildings(int iChange);

	int GetNumBuilding(BuildingTypes eIndex) const;
	int GetNumActiveBuilding(BuildingTypes eIndex) const;

	bool IsBuildingSellable(const CvBuildingEntry& kBuilding) const;
	void DoSellBuilding(BuildingTypes eIndex);
	int GetSellBuildingRefund(BuildingTypes eIndex) const;

	bool IsSoldBuildingThisTurn() const;
	void SetSoldBuildingThisTurn(bool bValue);

	int GetTotalBaseBuildingMaintenance() const;

	int GetBuildingProduction(BuildingTypes eIndex) const;
	int GetBuildingProductionTimes100(BuildingTypes eIndex) const;
	void SetBuildingProduction(BuildingTypes eIndex, int iNewValue);
	void SetBuildingProductionTimes100(BuildingTypes eIndex, int iNewValue);
	void ChangeBuildingProduction(BuildingTypes eIndex, int iChange);
	void ChangeBuildingProductionTimes100(BuildingTypes eIndex, int iChange);

	int GetBuildingProductionTime(BuildingTypes eIndex) const;
	void SetBuildingProductionTime(BuildingTypes eIndex, int iNewValue);
	void ChangeBuildingProductionTime(BuildingTypes eIndex, int iChange);

	int GetBuildingOriginalOwner(BuildingTypes eIndex) const;
	void SetBuildingOriginalOwner(BuildingTypes eIndex, int iNewValue);
	int GetBuildingOriginalTime(BuildingTypes eIndex) const;
	void SetBuildingOriginalTime(BuildingTypes eIndex, int iNewValue);

	int GetNumRealBuilding(BuildingTypes eIndex) const;
	void SetNumRealBuilding(BuildingTypes eIndex, int iNewValue);
	void SetNumRealBuildingTimed(BuildingTypes eIndex, int iNewValue, bool bFirst, PlayerTypes eOriginalOwner, int iOriginalTime);

	int GetNumFreeBuilding(BuildingTypes eIndex) const;
	void SetNumFreeBuilding(BuildingTypes eIndex, int iNewValue);

	int GetBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield) const;
	void SetBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield, int iChange);
	void ChangeBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield, int iChange);

	int GetBuildingGreatWork(BuildingClassTypes eBuildingClass, int iSlot) const;
	void SetBuildingGreatWork(BuildingClassTypes eBuildingClass, int iSlot, int iGreatWorkIndex);
	bool IsHoldingGreatWork(BuildingClassTypes eBuildingClass) const;
	int GetNumGreatWorksInBuilding(BuildingClassTypes eBuildingClass) const;
  
	bool HasAnyAvailableGreatWorkSlot() const;
	bool HasAvailableGreatWorkSlot(GreatWorkSlotType eGreatWorkSlot) const;
	int GetNumAvailableGreatWorkSlots() const;
	int GetNumAvailableGreatWorkSlots(GreatWorkSlotType eGreatWorkSlot) const;
	bool GetNextAvailableGreatWorkSlot(BuildingClassTypes *eBuildingClass, int *iSlot) const;
	bool GetNextAvailableGreatWorkSlot(GreatWorkSlotType eGreatWorkSlot, BuildingClassTypes *eBuildingClass, int *iSlot) const;

#if defined(MOD_GLOBAL_GREATWORK_YIELDTYPES)
	int GetYieldFromGreatWorks(YieldTypes eYield) const;
#endif
	int GetCultureFromGreatWorks() const;
#if defined(MOD_GLOBAL_GREATWORK_YIELDTYPES)
	int GetNumGreatWorks(bool bIgnoreYield = true) const;
#else
	int GetNumGreatWorks() const;
#endif
	int GetNumGreatWorks(GreatWorkSlotType eGreatWorkSlot) const;
	int GetLandmarksTourismPercent() const;
	void ChangeLandmarksTourismPercent(int iChange);
	int GetGreatWorksTourismModifier() const;
	void ChangeGreatWorksTourismModifier(int iChange);

#if defined(MOD_GLOBAL_GREATWORK_YIELDTYPES)
	int GetThemingBonuses(YieldTypes eYield) const;
#else
	int GetThemingBonuses() const;
#endif
	int GetNumBuildingsFromFaith() const;

	int GetCityStateTradeRouteProductionModifier() const;

	int GetBuildingProductionModifier() const;
	void ChangeBuildingProductionModifier(int iChange);

	int GetBuildingDefense() const;
	void ChangeBuildingDefense(int iChange);
	int GetBuildingDefenseMod() const;
	void ChangeBuildingDefenseMod(int iChange);

	int GetMissionaryExtraSpreads() const;
	void ChangeMissionaryExtraSpreads(int iChange);

	void IncrementWonderStats(BuildingClassTypes eIndex);
	bool CheckForAllWondersBuilt();
	bool CheckForSevenAncientWondersBuilt();

private:
	void NotifyNewBuildingStarted(BuildingTypes eIndex);

	int m_iNumBuildings;
	int m_iBuildingProductionModifier;
	int m_iBuildingDefense;
	int m_iBuildingDefenseMod;
	int m_iMissionaryExtraSpreads;
	int m_iLandmarksTourismPercent;
	int m_iGreatWorksTourismModifier;

	bool m_bSoldBuildingThisTurn;

	int* m_paiBuildingProduction;
	int* m_paiBuildingProductionTime;
	int* m_paiBuildingOriginalOwner;
	int* m_paiBuildingOriginalTime;
	int* m_paiNumRealBuilding;
	int* m_paiNumFreeBuilding;

	std::vector<BuildingYieldChange> m_aBuildingYieldChange;
	std::vector<BuildingGreatWork> m_aBuildingGreatWork;

	CvBuildingXMLEntries* m_pBuildings;
	CvCity* m_pCity;
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Helper Functions to serialize arrays of variable length (based on number of buildings defined in game)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
namespace BuildingArrayHelpers
{
void Read(FDataStream& kStream, int* paiBuildingArray);
void Write(FDataStream& kStream, int* paiBuildingArray, int iArraySize);
}

#endif //CIV5_BUILDING_CLASSES_H