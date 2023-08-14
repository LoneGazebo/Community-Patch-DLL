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
	  m_bConsecutiveEras(false),
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

	int GetBonus() const {return m_iBonus;};
	CvString GetDescription() {return m_strDescription;};
	bool IsSameEra() const {return m_bSameEra;};
	bool IsUniqueEras() {return m_bUniqueEras || IsConsecutiveEras();};
	bool IsConsecutiveEras() const {return m_bConsecutiveEras;};
	bool IsMustBeArt() const {return m_bMustBeArt;};
	bool IsMustBeArtifact() const {return m_bMustBeArtifact;};
	bool IsMustBeEqualArtArtifact() const {return m_bMustBeEqualArtArtifact;};
	bool IsRequiresOwner() const {return m_bRequiresOwner;};
	bool IsRequiresAnyButOwner() const {return m_bRequiresAnyButOwner;};
	bool IsRequiresSamePlayer() const {return m_bRequiresSamePlayer;};
	bool IsRequiresUniquePlayers() const {return m_bRequiresUniquePlayers;};
	int GetAIPriority() const {return m_iAIPriority;};

protected:
	int m_iBonus;
	CvString m_strDescription;
	bool m_bSameEra;
	bool m_bUniqueEras;
	bool m_bConsecutiveEras;
	bool m_bMustBeArt;
	bool m_bMustBeArtifact;
	bool m_bMustBeEqualArtArtifact;
	bool m_bRequiresOwner;
	bool m_bRequiresAnyButOwner;
	bool m_bRequiresSamePlayer;
	bool m_bRequiresUniquePlayers;
	int m_iAIPriority;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvDoubleYieldInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvDoubleYieldInfo
{
	friend class CvBuildingEntry;

public:
	CvDoubleYieldInfo() :
		m_iYieldIn(NO_YIELD),
		m_iYieldOut(NO_YIELD),
		m_iValue(0)
	{

	};

	YieldTypes GetYieldIn() { return m_iYieldIn; };
	YieldTypes GetYieldOut() { return m_iYieldOut; };
	int GetValue() const { return m_iValue; };

protected:
	YieldTypes m_iYieldIn;
	YieldTypes m_iYieldOut;
	int m_iValue;
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
	virtual ~CvBuildingEntry(void);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	// Accessor Functions (Non-Arrays)
	BuildingClassTypes GetBuildingClassType() const;
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
#if defined(MOD_BALANCE_CORE)
	int GetEra() const;
#endif
	int GetPolicyBranchType() const;
#if defined(MOD_BALANCE_CORE_POLICIES)
	PolicyTypes GetPolicyType() const;
	CivilizationTypes GetCivType() const;
#endif
#if defined(MOD_BALANCE_CORE)
	int GetResourceType() const;
	int GetNumPoliciesNeeded() const;
	int GrantsRandomResourceTerritory() const;
	bool IsPuppetPurchaseOverride() const;
	bool IsAllowsPuppetPurchase() const;
	int GetCooldown() const;
	bool IsTradeRouteInvulnerable() const;
	int GetTRSpeedBoost() const;
	int GetTRVisionBoost() const;
	int GetTRTurnModGlobal() const;
	int GetTRTurnModLocal() const;
	int GetVotesPerGPT() const;
	bool IsRequiresRail() const;
	bool IsDummy() const;
	int GetLandmarksTourismPercentGlobal() const;
	int GetGreatWorksTourismModifierGlobal() const;
	int GetEventRequiredActive() const;
	int GetCityEventRequiredActive() const;
#endif
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
	int GetAirModifierGlobal() const;
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
	int GetBorderGrowthRateIncrease() const;
	int GetBorderGrowthRateIncreaseGlobal() const;
	int GetGlobalPlotCultureCostModifier() const;
	int GetPlotCultureCostModifier() const;
	int GetGlobalPlotBuyCostModifier() const;
	int GetPlotBuyCostModifier() const;
#if defined(MOD_BUILDINGS_CITY_WORKING)
	int GetGlobalCityWorkingChange() const;
	int GetCityWorkingChange() const;
#endif
#if defined(MOD_BUILDINGS_CITY_AUTOMATON_WORKERS)
	int GetGlobalCityAutomatonWorkersChange() const;
	int GetCityAutomatonWorkersChange() const;
#endif
#if defined(MOD_BALANCE_CORE)
	int GetNumRequiredTier3Tenets() const;
	bool IsNoWater() const;
	bool IsNoRiver() const;
	bool IsNoCoast() const;
	bool IsCapitalOnly() const;
	bool IsReformation() const;
	int GetReformationFollowerReduction() const;
	bool IsBuildAnywhere() const;
	int GetTradeReligionModifier() const;
	int GetNumFreeArtifacts() const;
	int GetResourceDiversityModifier() const;
	int GetNoUnhappfromXSpecialists() const;
	int GetNoUnhappfromXSpecialistsGlobal() const;

	int GetPurchaseCooldownReduction(bool bCivilian = false) const;
#endif
	bool IsVassalLevyEra() const;
#if defined(MOD_BALANCE_CORE_POP_REQ_BUILDINGS)
	int GetNationalPopulationRequired() const;
	int GetLocalPopulationRequired() const;
#endif
#if defined(MOD_BALANCE_CORE_FOLLOWER_POP_WONDER)
	int GetNationalFollowerPopRequired() const;
	int GetGlobalFollowerPopRequired() const;
#endif
	int GetMinAreaSize() const;
	int GetConquestProbability() const;
	int GetHealRateChange() const;
	int GetHappiness() const;
	int GetUnmoddedHappiness() const;
	int GetUnhappinessModifier() const;
#if defined(MOD_BALANCE_CORE)
	int GetLocalUnhappinessModifier() const;
	int GetGlobalBuildingGoldMaintenanceMod() const;
	int GetBuildingDefenseModifier() const;
	int GetDamageReductionFlat() const;
	int GetCitySupplyModifier() const;
	int GetCitySupplyModifierGlobal() const;
	int GetCitySupplyFlat() const;
	int GetCitySupplyFlatGlobal() const;
	int CityRangedStrikeRange() const;
	int CityIndirectFire() const;
	int CityRangedStrikeModifier() const;
#endif
	int GetHappinessPerCity() const;
	int GetHappinessPerXPolicies() const;
	int GetCityCountUnhappinessMod() const;
	bool IsNoOccupiedUnhappiness() const;
	int GetGlobalPopulationChange() const;
	int GetPopulationChange() const;
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
	int GetExtraMissionaryStrength() const;
	int GetExtraMissionarySpreadsGlobal() const;
	int GetReligiousPressureModifier() const;
	int GetEspionageModifier() const;
	int GetGlobalEspionageModifier() const;
	int GetEspionageTurnsModifierFriendly() const;
	int GetEspionageTurnsModifierEnemyCity() const;
	int GetEspionageTurnsModifierEnemyGlobal() const;
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
	int GetCityConnectionTradeRouteGoldModifier() const;
	int GetCityStateTradeRouteProductionModifier() const;
	int GetGreatScientistBeakerModifier() const;
	int GetExtraLeagueVotes() const;
	int GetSingleVotes() const;
	int GetFaithToVotes() const;
	int GetCapitalsToVotes() const;
	int GetDoFToVotes() const;
	int GetRAToVotes() const;
	int GetDPToVotes() const;
	bool IsIgnoreDefensivePactLimit() const;
	int GetGPExpendInfluence() const;
	int GetEmpireSizeModifierReduction() const;
	int GetEmpireSizeModifierReductionGlobal() const;
	int GetBasicNeedsMedianModifier() const;
	int GetGoldMedianModifier() const;
	int GetScienceMedianModifier() const;
	int GetCultureMedianModifier() const;
	int GetReligiousUnrestModifier() const;
	int GetBasicNeedsMedianModifierGlobal() const;
	int GetGoldMedianModifierGlobal() const;
	int GetScienceMedianModifierGlobal() const;
	int GetCultureMedianModifierGlobal() const;
	int GetReligiousUnrestModifierGlobal() const;
	int GetPreferredDisplayPosition() const;
	int GetPortraitIndex() const;
	bool IsTeamShare() const;
	bool IsWater() const;
	bool IsRiver() const;
	bool IsFreshWater() const;
	bool IsAddsFreshWater() const;
	bool IsPurchaseOnly() const;
	bool IsSecondaryPantheon() const;
	int GetGreatWorkYieldChange(int i) const;
	int* GetGreatWorkYieldChangeArray() const;

	int GetGreatWorkYieldChangeLocal(int i) const;
	int* GetGreatWorkYieldChangeLocalArray() const;
	bool IsMountain() const;
	bool IsHill() const;
	bool IsFlat() const;
	bool IsFoundsReligion() const;
	bool IsReligious() const;
	bool IsBorderObstacle() const;
#if defined(MOD_BALANCE_CORE)
	bool IsAnyBodyOfWater() const;
	int GetCityAirStrikeDefense() const;
	int GetBorderObstacleLand() const;
	int GetBorderObstacleWater() const;
	int GetDeepWaterTileDamage() const;
	int GetWLTKDTurns() const;
	int GetEventTourism() const;
	int GetLandTourismEnd() const;
	int GetSeaTourismEnd() const;
	int GetAlwaysHeal() const;
	bool IsCorp() const;
	int GetNukeInterceptionChance() const;
#endif
#if defined(HH_MOD_BUILDINGS_FRUITLESS_PILLAGE)
	bool IsPlayerBorderGainlessPillage() const;
	bool IsCityGainlessPillage() const;
#endif
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
	bool AllowsFoodTradeRoutesGlobal() const;
	bool AllowsProductionTradeRoutes() const;
	bool AllowsProductionTradeRoutesGlobal() const;
	bool NullifyInfluenceModifier() const;

	const char* GetArtDefineTag() const;
	void SetArtDefineTag(const char* szVal);
	const bool GetArtInfoCulturalVariation() const;
	const bool GetArtInfoEraVariation() const;
	const bool GetArtInfoRandomVariation() const;

	const char* GetWonderSplashAudio() const;
	CvString GetThemingBonusHelp() const;

	int GetDistressFlatReduction() const;
	int GetPovertyFlatReduction() const;
	int GetIlliteracyFlatReduction() const;
	int GetBoredomFlatReduction() const;
	int GetReligiousUnrestFlatReduction() const;

	int GetDistressFlatReductionGlobal() const;
	int GetPovertyFlatReductionGlobal() const;
	int GetIlliteracyFlatReductionGlobal() const;
	int GetBoredomFlatReductionGlobal() const;
	int GetReligiousUnrestFlatReductionGlobal() const;

	// Accessor Functions (Arrays)
	int GetGrowthExtraYield(int i) const;
	int* GetGrowthExtraYieldArray() const;

	int GetNeedBuildingThisCity() const;

#if defined(MOD_BALANCE_CORE_POLICIES)
	int GetYieldFromDeath(int i) const;
	int* GetYieldFromDeathArray() const;
#endif
#if defined(MOD_BALANCE_CORE)
	int GetYieldFromVictory(int i) const;
	int* GetYieldFromVictoryArray() const;

	int GetYieldFromVictoryGlobal(int i) const;
	int* GetYieldFromVictoryGlobalArray() const;

	int GetYieldFromVictoryGlobalEraScaling(int i) const;
	int* GetYieldFromVictoryGlobalArrayEraScaling() const;

	int GetYieldFromVictoryGlobalPlayer(int i) const;
	int* GetYieldFromVictoryGlobalPlayerArray() const;

	int GetYieldFromPillage(int i) const;
	int* GetYieldFromPillageArray() const;

	int GetYieldFromPillageGlobal(int i) const;
	int* GetYieldFromPillageGlobalArray() const;

	int GetYieldFromPillageGlobalPlayer(int i) const;
	int* GetYieldFromPillageGlobalPlayerArray() const;

	int GetGoldenAgeYieldMod(int i) const;
	int* GetGoldenAgeYieldModArray() const;

	int GetYieldFromWLTKD(int i) const;
	int* GetYieldFromWLTKDArray() const;

	int GetYieldFromGPExpend(int i) const;
	int* GetYieldFromGPExpendArray() const;

	int GetYieldFromTech(int i) const;
	int* GetYieldFromTechArray() const;

	int GetYieldFromConstruction(int i) const;
	int* GetYieldFromConstructionArray() const;

	int GetYieldFromBirth(int i) const;
	int* GetYieldFromBirthArray() const;

	int GetYieldFromUnitProduction(int i) const;
	int* GetYieldFromUnitProductionArray() const;

	int GetYieldFromBorderGrowth(int i) const;
	int* GetYieldFromBorderGrowthArray() const;

	int GetYieldFromPolicyUnlock(int i) const;
	int* GetYieldFromPolicyUnlockArray() const;

	int GetYieldFromUnitLevelUp(int i) const;
	int* GetYieldFromUnitLevelUpArray() const;

	int GetYieldFromCombatExperience(int i) const;
	int* GetYieldFromCombatExperienceArray() const;

	int GetYieldFromPurchase(int i) const;
	int* GetYieldFromPurchaseArray() const;

	int GetYieldFromFaithPurchase(int i) const;
	int* GetYieldFromFaithPurchaseArray() const;

	int GetYieldFromInternalTREnd(int i) const;
	int* GetYieldFromInternalTREndArray() const;

	int GetYieldFromInternal(int i) const;
	int* GetYieldFromInternalArray() const;

	int GetYieldFromProcessModifier(int i) const;
	int* GetYieldFromProcessModifierArray() const;

	int GetThemingYieldBonus(int i) const;
	int* GetThemingYieldBonusArray() const;

	int GetYieldFromSpyAttack(int i) const;
	int* GetYieldFromSpyAttackArray() const;

	int GetYieldFromSpyDefense(int i) const;
	int* GetYieldFromSpyDefenseArray() const;

#endif
	int GetYieldChange(int i) const;
	int* GetYieldChangeArray() const;
	int GetYieldChangePerPop(int i) const;
	int* GetYieldChangePerPopArray() const;
#if defined(MOD_BALANCE_CORE)
	int GetYieldChangePerPopInEmpire(int i) const;
#endif
	int GetYieldChangePerReligion(int i) const;
	int* GetYieldChangePerReligionArray() const;
	int GetYieldModifier(int i) const;
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
#if defined(MOD_BALANCE_CORE)
	int GetDomainFreeExperiencePerGreatWorkGlobal(int i) const;
	int GetDomainFreeExperienceGlobal(int i) const;
#endif
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
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	int GetFeatureOr(int i) const;
	int GetFeatureAnd(int i) const;
	int GetResourceMonopolyAnd(int i) const;
	int GetResourceMonopolyOr(int i) const;
	int GetGPRateModifierPerXFranchises() const;
	int GetResourceQuantityPerXFranchises(int i) const;
	int GetYieldPerFranchise(int i) const;
#endif
	int GetResourceQuantityFromPOP(int i) const;
	int GetHurryModifier(int i) const;
#if defined(MOD_BALANCE_CORE)
	int GetHurryModifierLocal(int i) const;
#endif
	bool IsBuildingClassNeededInCity(int i) const;
#if defined(MOD_BALANCE_CORE)
	bool IsBuildingClassNeededAnywhere(int i) const;
	bool IsBuildingClassNeededNowhere(int i) const;
	int GetNumFreeSpecialUnits(int i) const;
	std::map<int, int> GetResourcePlotsToPlace(int i) const;
	bool IsResourcePlotsToPlace() const;
	int GetYieldPerFriend(int i) const;
	int GetYieldPerAlly(int i) const;
	int GetYieldChangeWorldWonder(int i) const;
	int GetYieldChangeWorldWonderGlobal(int i) const;
#endif
	int GetNumFreeUnits(int i) const;
#if defined(MOD_BALANCE_CORE_BUILDING_INSTANT_YIELD)
	int GetInstantYield(int i) const;
	int* GetInstantYieldArray() const;

	int GetYieldFromYield(int i, int j) const;

	int GetYieldFromYieldGlobal(int i, int j) const;

	int GetInstantReligionPressure() const;
#endif

	int GetResourceYieldChange(int i, int j) const;
	int* GetResourceYieldChangeArray(int i) const;
	int GetFeatureYieldChange(int i, int j) const;
	int* GetFeatureYieldChangeArray(int i) const;
#if defined(MOD_BALANCE_CORE)
	int GetResourceYieldChangeGlobal(int iResource, int iYieldType) const;
	int GetImprovementYieldChange(int i, int j) const;
	int* GetImprovementYieldChangeArray(int i) const;

	int GetImprovementYieldChangeGlobal(int i, int j) const;
	int* GetImprovementYieldChangeGlobalArray(int i) const;
	int GetSpecialistYieldChangeLocal(int i, int j) const;
	int* GetSpecialistYieldChangeLocalArray(int i) const;
#endif
	int GetSpecialistYieldChange(int i, int j) const;
	int* GetSpecialistYieldChangeArray(int i) const;
	int GetResourceYieldModifier(int i, int j) const;
	int* GetResourceYieldModifierArray(int i) const;
	int GetTerrainYieldChange(int i, int j) const;
	int* GetTerrainYieldChangeArray(int i) const;

	int GetYieldPerXTerrain(int i, int j) const;
	int* GetYieldPerXTerrainArray(int i) const;

	int GetYieldPerXFeature(int i, int j) const;
	int* GetYieldPerXFeatureArray(int i) const;

	int GetPlotYieldChange(int i, int j) const;
	int* GetPlotYieldChangeArray(int i) const;

	int GetBuildingClassYieldChange(int i, int j) const;
	int GetBuildingClassYieldModifier(int i, int j) const;
#if defined(MOD_BALANCE_CORE)
	int GetBuildingClassLocalYieldChange(int i, int j) const;
	int GetBuildingClassLocalHappiness(int i) const;
	int GetSpecificGreatPersonRateModifier(int) const;
	int GetResourceHappiness(int i) const;
#endif
	int GetBuildingClassHappiness(int i) const;

#if defined(MOD_BALANCE_CORE)
	std::multimap<int, std::pair<int, int>> GetGreatPersonProgressFromConstructionArray() const;
#endif

	CvThemingBonusInfo *GetThemingBonusInfo(int i) const;
	int GetNumThemingBonuses() const { return m_iNumThemingBonuses; }
	const vector<UnitTypes>& GetUnitsUnlocked() const { return m_vUnitTypesUnlocked; }
	void UpdateUnitTypesUnlocked();
	bool IsFaithPurchaseOnly() const;

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
#if defined(MOD_BALANCE_CORE_POLICIES)
	PolicyTypes m_iPolicyType;
	CivilizationTypes m_eCivType;
#endif
#if defined(MOD_BALANCE_CORE)
	int m_iNumPoliciesNeeded;
	int m_iResourceType;
	int m_iGrantsRandomResourceTerritory;
	bool m_bPuppetPurchaseOverride;
	bool m_bAllowsPuppetPurchase;
	int m_iGetCooldown;
	bool m_bTradeRouteInvulnerable;
	int m_iTRSpeedBoost;
	int m_iTRTurnModGlobal;
	int m_iTRTurnModLocal;
	int m_iTRVisionBoost;
	int m_iVotesPerGPT;
	bool m_bRequiresRail;
	bool m_bDummy;
	int m_iLandmarksTourismPercentGlobal;
	int m_iGreatWorksTourismModifierGlobal;
#endif
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
	int m_iAirModifierGlobal;
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
	int m_iBorderGrowthRateIncrease;
	int m_iBorderGrowthRateIncreaseGlobal;
	int m_iGlobalPlotCultureCostModifier;
	int m_iPlotCultureCostModifier;
	int m_iGlobalPlotBuyCostModifier;
	int m_iPlotBuyCostModifier;
#if defined(MOD_BUILDINGS_CITY_WORKING)
	int m_iGlobalCityWorkingChange;
	int m_iCityWorkingChange;
#endif
#if defined(MOD_BUILDINGS_CITY_AUTOMATON_WORKERS)
	int m_iGlobalCityAutomatonWorkersChange;
	int m_iCityAutomatonWorkersChange;
#endif
	int m_iMinAreaSize;
	int m_iConquestProbability;
	int m_iHealRateChange;
	int m_iHappiness;
	int m_iUnmoddedHappiness;
	int m_iUnhappinessModifier;
#if defined(MOD_BALANCE_CORE)
	int m_iLocalUnhappinessModifier;
	int m_iGlobalBuildingGoldMaintenanceMod;
	int m_iBuildingDefenseModifier;
	int m_iDamageReductionFlat;
	int m_iCitySupplyModifier;
	int m_iCitySupplyModifierGlobal;
	int m_iCitySupplyFlat;
	int m_iCitySupplyFlatGlobal;
	int m_iCityRangedStrikeRange;
	int m_iCityIndirectFire;
	int m_iRangedStrikeModifier;
#endif
	int m_iHappinessPerCity;
	int m_iHappinessPerXPolicies;
	int m_iCityCountUnhappinessMod;
	bool m_bNoOccupiedUnhappiness;
	int m_iGlobalPopulationChange;
	int m_iPopulationChange;
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
	int m_iExtraMissionaryStrength;
	int m_iExtraMissionarySpreadsGlobal;
	int m_iReligiousPressureModifier;
	int m_iEspionageModifier;
	int m_iGlobalEspionageModifier;
	int m_iEspionageTurnsModifierFriendly;
	int m_iEspionageTurnsModifierEnemyCity;
	int m_iEspionageTurnsModifierEnemyGlobal;
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
	int m_iCityConnectionTradeRouteGoldModifier;
	int m_iCityStateTradeRouteProductionModifier;
	int m_iGreatScientistBeakerModifier;
	int m_iExtraLeagueVotes;
	int m_iSingleLeagueVotes;
	int m_iFaithToVotesBase;
	int m_iCapitalsToVotesBase;
	int m_iDoFToVotesBase;
	int m_iRAToVotesBase;
	int m_iDPToVotesBase;
	bool m_bIgnoreDefensivePactLimit;
	int m_iGPExpendInfluenceBase;
	int m_iEmpireSizeModifierReduction;
	int m_iEmpireSizeModifierReductionGlobal;
	int m_iBasicNeedsMedianModifier;
	int m_iGoldMedianModifier;
	int m_iScienceMedianModifier;
	int m_iCultureMedianModifier;
	int m_iReligiousUnrestModifier;
	int m_iBasicNeedsMedianModifierGlobal;
	int m_iGoldMedianModifierGlobal;
	int m_iScienceMedianModifierGlobal;
	int m_iCultureMedianModifierGlobal;
	int m_iReligiousUnrestModifierGlobal;
	int m_iDistressFlatReduction;
	int m_iPovertyFlatReduction;
	int m_iIlliteracyFlatReduction;
	int m_iBoredomFlatReduction;
	int m_iReligiousUnrestFlatReduction;
	int m_iDistressFlatReductionGlobal;
	int m_iPovertyFlatReductionGlobal;
	int m_iIlliteracyFlatReductionGlobal;
	int m_iBoredomFlatReductionGlobal;
	int m_iReligiousUnrestFlatReductionGlobal;
	int m_iPreferredDisplayPosition;
	int m_iPortraitIndex;

	bool m_bTeamShare;
	bool m_bWater;
	bool m_bRiver;
	bool m_bFreshWater;
	bool m_bAddsFreshWater;
	bool m_bPurchaseOnly;
	bool m_bSecondaryPantheon;
	int* m_piGreatWorkYieldChange;
	int* m_piGreatWorkYieldChangeLocal;
#if defined(MOD_BALANCE_CORE)
	int m_iNumRequiredTier3Tenets;
	bool m_bIsNoWater;
	bool m_bIsNoRiver;
	bool m_bIsNoCoast;
	bool m_bIsCapitalOnly;
	bool m_bIsReformation;
	int m_iReformationFollowerReduction;
	bool m_bBuildAnywhere;
	int m_iTradeReligionModifier;
	int m_iFreeArtifacts;
	int m_iResourceDiversityModifier;
	bool m_bAnyWater;
	int m_iNoUnhappfromXSpecialists;
	int m_iNoUnhappfromXSpecialistsGlobal;
	int m_iPurchaseCooldownReduction;
	int m_iPurchaseCooldownReductionCivilian;
#endif
#if defined(MOD_BALANCE_CORE_EVENTS)
	int m_iEventRequiredActive;
	int m_iCityEventRequiredActive;
#endif
	bool m_bVassalLevyEra;
#if defined(MOD_BALANCE_CORE_POP_REQ_BUILDINGS)
	int m_iNationalPopRequired;
	int m_iLocalPopRequired;
#endif
#if defined(MOD_BALANCE_CORE_FOLLOWER_POP_WONDER)
	int m_iNationalFollowerPopRequired;
	int m_iGlobalFollowerPopRequired;
#endif
	bool m_bMountain;
	bool m_bHill;
	bool m_bFlat;
	bool m_bFoundsReligion;
	bool m_bIsReligious;
	bool m_bBorderObstacle;
#if defined(MOD_BALANCE_CORE)
	int m_iCityAirStrikeDefense;
	int m_iBorderObstacleCity;
	int m_iBorderObstacleWater;
	int m_iDeepWaterTileDamage;
	int m_iWLTKDTurns;
	int m_iEventTourism;
	int m_iLandTourism;
	int m_iSeaTourism;
	int m_iAlwaysHeal;
	bool m_bIsCorp;
	int m_iNukeInterceptionChance;
#endif
#if defined(HH_MOD_BUILDINGS_FRUITLESS_PILLAGE)
	bool m_bPlayerBorderGainlessPillage;
	bool m_bCityGainlessPillage;
#endif
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
	bool m_bAllowsFoodTradeRoutesGlobal;
	bool m_bAllowsProductionTradeRoutes;
	bool m_bAllowsProductionTradeRoutesGlobal;
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
	int* m_piGrowthExtraYield;
	int m_iNeedBuildingThisCity;
#if defined(MOD_BALANCE_CORE_POLICIES)
	int* m_piYieldFromDeath;
#endif
#if defined(MOD_BALANCE_CORE)
	int* m_piYieldFromVictory;
	int* m_piYieldFromVictoryGlobal;
	int* m_piYieldFromVictoryGlobalEraScaling;
	int* m_piYieldFromVictoryGlobalPlayer;
	int* m_piYieldFromPillage;
	int* m_piYieldFromPillageGlobal;
	int* m_piYieldFromPillageGlobalPlayer;
	int* m_piGoldenAgeYieldMod;
	int* m_piYieldFromWLTKD;
	int* m_piYieldFromGPExpend;
	int* m_piThemingYieldBonus;
	int* m_piYieldFromSpyAttack;
	int* m_piYieldFromSpyDefense;
	int* m_piYieldFromTech;
	int* m_piYieldFromConstruction;
	int* m_piYieldFromBirth;
	int* m_piYieldFromUnitProduction;
	int* m_piYieldFromBorderGrowth;
	int* m_piYieldFromPolicyUnlock;
	int* m_piYieldFromUnitLevelUp;
	int* m_piYieldFromCombatExperience;
	int* m_piYieldFromPurchase;
	int* m_piYieldFromFaithPurchase;
	int* m_piYieldFromInternalTREnd;
	int* m_piYieldFromInternal;
	int* m_piYieldFromProcessModifier;
#endif
	int* m_piYieldChange;
	int* m_piYieldChangePerPop;
#if defined(MOD_BALANCE_CORE)
	std::map<int, int> m_piYieldChangePerPopInEmpire;
#endif
	int* m_piYieldChangePerReligion;
	int* m_piYieldModifier;
	int* m_piAreaYieldModifier;
	int* m_piGlobalYieldModifier;
	int* m_piTechEnhancedYieldChange;
	int* m_piUnitCombatFreeExperience;
	int* m_piUnitCombatProductionModifiers;
	int* m_piDomainFreeExperience;
	int* m_piDomainFreeExperiencePerGreatWork;
#if defined(MOD_BALANCE_CORE)
	int* m_piDomainFreeExperiencePerGreatWorkGlobal;
	std::map<int, int> m_piDomainFreeExperienceGlobal;
#endif
	int* m_piDomainProductionModifier;
	int* m_piPrereqNumOfBuildingClass;
	int* m_piFlavorValue;
	int* m_piLocalResourceAnds;
	int* m_piLocalResourceOrs;
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	int* m_piLocalFeatureOrs;
	int* m_piLocalFeatureAnds;
	int* m_piResourceMonopolyAnds;
	int* m_piResourceMonopolyOrs;
	int* m_piYieldPerFranchise;
	int m_iGPRateModifierPerXFranchises;
	int* m_piResourceQuantityPerXFranchises;
#endif
	int* m_piResourceQuantityFromPOP;
	int* m_paiHurryModifier;

	bool* m_pbBuildingClassNeededInCity;
#if defined(MOD_BALANCE_CORE)
	int* m_paiHurryModifierLocal;
	bool* m_pbBuildingClassNeededAnywhere;
	bool* m_pbBuildingClassNeededNowhere;
	int* m_piNumSpecFreeUnits;
	std::map<int, std::map<int, int>> m_ppiResourcePlotsToPlace;
	int* m_piYieldPerFriend;
	int* m_piYieldPerAlly;
	int* m_piYieldChangeWorldWonder;
	int* m_piYieldChangeWorldWonderGlobal;
#endif
	int* m_piNumFreeUnits;

	int** m_ppaiResourceYieldChange;
	int** m_ppaiFeatureYieldChange;
#if defined(MOD_BALANCE_CORE)
	std::map<int, std::map<int, int>> m_ppiResourceYieldChangeGlobal;
	CvDoubleYieldInfo* m_paYieldFromYield;
	CvDoubleYieldInfo* m_paYieldFromYieldGlobal;
	int** m_ppaiImprovementYieldChange;
	int** m_ppaiImprovementYieldChangeGlobal;
	int** m_ppaiSpecialistYieldChangeLocal;
#endif
	int** m_ppaiSpecialistYieldChange;
	int** m_ppaiResourceYieldModifier;
	int** m_ppaiTerrainYieldChange;
	int** m_ppaiYieldPerXTerrain;
	int** m_ppaiYieldPerXFeature;
	int** m_ppaiPlotYieldChange;
	int** m_ppiBuildingClassYieldChanges;
	int** m_ppiBuildingClassYieldModifiers;
#if defined(MOD_BALANCE_CORE)
	int** m_ppiBuildingClassLocalYieldChanges;
	int* m_paiBuildingClassLocalHappiness;
	int* m_paiSpecificGreatPersonRateModifier;
	int* m_paiResourceHappinessChange;
#endif
	int* m_paiBuildingClassHappiness;
#if defined(MOD_BALANCE_CORE_BUILDING_INSTANT_YIELD)
	int* m_piInstantYield;
	int m_iInstantReligionPressure;
#endif
#if defined(MOD_BALANCE_CORE)
	std::multimap<int, std::pair<int, int>> m_piiGreatPersonProgressFromConstruction;
#endif

	CvThemingBonusInfo* m_paThemingBonusInfo;
	int m_iNumThemingBonuses;
	vector<UnitTypes> m_vUnitTypesUnlocked; //not serialized
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
	_Ret_maybenull_ CvBuildingEntry* GetEntry(int index) const;
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
	void Init(CvBuildingXMLEntries* pPossibleBuildings, CvCity* pCity);
	void Uninit();
	void Reset();
	template<typename CityBuildings, typename Visitor>
	static void Serialize(CityBuildings& cityBuildings, Visitor& visitor);
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	// Accessor functions
	CvBuildingXMLEntries* GetPossibleBuildings() const;

	int GetNumBuildings() const;
	void ChangeNumBuildings(int iChange);
	int GetNumBuilding(BuildingTypes eIndex) const;

	int GetNumBuildingClass(BuildingClassTypes eIndex) const;
	bool HasBuildingClass(BuildingClassTypes eIndex) const;
	BuildingTypes GetBuildingTypeFromClass(BuildingClassTypes eIndex) const;
	void RemoveAllRealBuildingsOfClass(BuildingClassTypes eIndex);

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
#if defined(MOD_BALANCE_CORE)
	void SetNumRealBuilding(BuildingTypes eIndex, int iNewValue, bool bNoBonus = false);
	void SetNumRealBuildingTimed(BuildingTypes eIndex, int iNewValue, bool bFirst, PlayerTypes eOriginalOwner, int iOriginalTime, bool bNoBonus = false);
#else
	void SetNumRealBuilding(BuildingTypes eIndex, int iNewValue);
	void SetNumRealBuildingTimed(BuildingTypes eIndex, int iNewValue, bool bFirst, PlayerTypes eOriginalOwner, int iOriginalTime);
#endif
	int GetNumFreeBuilding(BuildingTypes eIndex) const;
	void SetNumFreeBuilding(BuildingTypes eIndex, int iNewValue);
#if defined(MOD_BALANCE_CORE)
	int IsFirstTimeBuilding(BuildingTypes eBuilding);
	void SetFirstTimeBuilding(BuildingTypes eBuilding, int bValue);
#endif
	int GetBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield) const;
	void SetBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield, int iChange);
	void ChangeBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield, int iChange);

	int GetBuildingGreatWork(BuildingClassTypes eBuildingClass, int iSlot) const;
	void SetBuildingGreatWork(BuildingClassTypes eBuildingClass, int iSlot, int iGreatWorkIndex);
	bool IsHoldingGreatWork(BuildingClassTypes eBuildingClass) const;
	int GetNumGreatWorksInBuilding(BuildingClassTypes eBuildingClass) const;
  
	bool HasAnyAvailableGreatWorkSlot() const;
	bool HasAvailableGreatWorkSlot(GreatWorkSlotType eGreatWorkSlot) const;
	int GetNumAvailableGreatWorkSlots(GreatWorkSlotType eGreatWorkSlot = NO_GREAT_WORK_SLOT) const;
	int GetNumFilledGreatWorkSlots(GreatWorkSlotType eGreatWorkSlot = NO_GREAT_WORK_SLOT) const;
	bool GetNextAvailableGreatWorkSlot(BuildingClassTypes *eBuildingClass, int *iSlot) const;
	bool GetNextAvailableGreatWorkSlot(GreatWorkSlotType eGreatWorkSlot, BuildingClassTypes *eBuildingClass, int *iSlot) const;

	int GetYieldFromGreatWorks(YieldTypes eYield) const;
	int GetCultureFromGreatWorks() const;
#if defined(MOD_GLOBAL_GREATWORK_YIELDTYPES)
	int GetNumGreatWorks(bool bIgnoreYield = true) const;
#else
	int GetNumGreatWorks() const;
#endif
	int GetNumGreatWorks(GreatWorkSlotType eGreatWorkSlot, bool bArtifact = false, bool bArt = false) const;
#if defined(MOD_BALANCE_CORE)
	int GetThemingBonusIndex(BuildingTypes eBuilding) const;
	void SetThemingBonusIndex(BuildingTypes eBuilding, int iIndex);
#endif

	int GetLandmarksTourismPercent() const;
	void ChangeLandmarksTourismPercent(int iChange);
	int GetGreatWorksTourismModifier() const;
	void ChangeGreatWorksTourismModifier(int iChange);

#if defined(MOD_GLOBAL_GREATWORK_YIELDTYPES)
	int GetCurrentThemingBonuses(YieldTypes eYield) const;
#else
	int GetCurrentThemingBonuses() const;
#endif
	int GetTotalNumThemedBuildings() const;
	int GetNumBuildingsFromFaith() const;

	int GetCityStateTradeRouteProductionModifier() const;

	int GetBuildingProductionModifier() const;
	void ChangeBuildingProductionModifier(int iChange);

	int GetBuildingProductionModifierPotentialFromMinorTrade() const;
	void ChangeBuildingProductionModifierPotentialFromMinorTrade(int iChange);

	int GetBuildingDefense() const;
	void ChangeBuildingDefense(int iChange);
	int GetBuildingDefenseMod() const;
	void ChangeBuildingDefenseMod(int iChange);

	int GetMissionaryExtraSpreads() const;
	void ChangeMissionaryExtraSpreads(int iChange);

	void IncrementWonderStats(BuildingClassTypes eIndex);
	bool CheckForAllWondersBuilt();
	bool CheckForSevenAncientWondersBuilt();

#if defined(MOD_BALANCE_CORE)
	const std::vector<BuildingTypes>& GetAllBuildingsHere() const { return m_buildingsThatExistAtLeastOnce; }
#endif
private:
	void NotifyNewBuildingStarted(BuildingTypes eIndex);

	int m_iNumBuildings;
	int m_iBuildingProductionModifier;
	int m_iBuildingProductionModifierPotentialFromMinorTrade;
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
#if defined(MOD_BALANCE_CORE)
	int* m_paiFirstTimeBuilding;
	int* m_paiThemingBonusIndex;
#endif

#if defined(MOD_BALANCE_CORE)
	std::vector<BuildingTypes> m_buildingsThatExistAtLeastOnce;
#endif

	std::vector<BuildingYieldChange> m_aBuildingYieldChange;
	std::vector<BuildingGreatWork> m_aBuildingGreatWork;

	CvBuildingXMLEntries* m_pPossibleBuildings;
	CvCity* m_pCity;
};

FDataStream& operator>>(FDataStream&, CvCityBuildings&);
FDataStream& operator<<(FDataStream&, const CvCityBuildings&);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Helper Functions to serialize arrays of variable length (based on number of buildings defined in game)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
namespace BuildingArrayHelpers
{
void Read(FDataStream& kStream, int* paiBuildingArray);
void Write(FDataStream& kStream, int* paiBuildingArray, int iArraySize);
}

#endif //CIV5_BUILDING_CLASSES_H