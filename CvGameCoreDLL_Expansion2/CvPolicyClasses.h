/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once
 
#ifndef CIV5_POLICY_CLASSES_H
#define CIV5_POLICY_CLASSES_H

// Forward definitions
class CvPolicyAI;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvPolicyEntry
//!  \brief		A single entry in the policy tree
//
//!  Key Attributes:
//!  - Used to be called CvPolicyInfo
//!  - Populated from XML\GameInfo\CIV5PolicyInfos.xml
//!  - Array of these contained in CvPolicyXMLEntries class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPolicyEntry: public CvBaseInfo
{
public:
	CvPolicyEntry(void);
	~CvPolicyEntry(void);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	// Accessor Functions (Non-Arrays)
	int GetCultureCost() const;
	int GetGridX() const;
	int GetGridY() const;
	int GetLevel() const;
	int GetPolicyCostModifier() const;
	int GetCulturePerCity() const;
	int GetCulturePerWonder() const;
	int GetCultureWonderMultiplier() const;
	int GetCulturePerTechResearched() const;
	int GetCultureImprovementChange() const;
	int GetCultureFromKills() const;
	int GetCultureFromBarbarianKills() const;
	int GetGoldFromKills() const;
	int GetEmbarkedExtraMoves() const;
	int GetAttackBonusTurns() const;
	int GetGoldenAgeTurns() const;
	int GetGoldenAgeMeterMod() const;
	int GetGoldenAgeDurationMod() const;
	int GetNumFreeTechs() const;
	int GetNumFreePolicies() const;
	int GetNumFreeGreatPeople() const;
	int GetMedianTechPercentChange() const;
	int GetStrategicResourceMod() const;
	int GetWonderProductionModifier() const;
	int GetBuildingProductionModifier() const;
	int GetGreatPeopleRateModifier() const;
	int GetGreatGeneralRateModifier() const;
	int GetGreatAdmiralRateModifier() const;
	int GetGreatWriterRateModifier() const;
	int GetGreatArtistRateModifier() const;
	int GetGreatMusicianRateModifier() const;
	int GetGreatMerchantRateModifier() const;
	int GetGreatScientistRateModifier() const;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	int GetGreatDiplomatRateModifier() const;
#endif
	int GetDomesticGreatGeneralRateModifier() const;
	int GetExtraHappiness() const;
	int GetExtraHappinessPerCity() const;
#if defined(HH_MOD_NATURAL_WONDER_MODULARITY)
	int GetExtraNaturalWonderHappiness() const;
#endif
	int GetUnhappinessMod() const;
	int GetCityCountUnhappinessMod() const;
	int GetOccupiedPopulationUnhappinessMod() const;
	int GetCapitalUnhappinessMod() const;
	int GetFreeExperience() const;
	int GetWorkerSpeedModifier() const;
	int GetAllFeatureProduction() const;
	int GetImprovementCostModifier() const;
	int GetImprovementUpgradeRateModifier() const;
	int GetSpecialistProductionModifier() const;
	int GetSpecialistUpgradeModifier() const;
	int GetMilitaryProductionModifier() const;
	int GetBaseFreeUnits() const;
	int GetBaseFreeMilitaryUnits() const;
	int GetFreeUnitsPopulationPercent() const;
	int GetFreeMilitaryUnitsPopulationPercent() const;
	int GetHappinessPerGarrisonedUnit() const;
	int GetCulturePerGarrisonedUnit() const;
	int GetHappinessPerTradeRoute() const;
	int GetHappinessPerXPopulation() const;
#if defined(MOD_BALANCE_CORE_POLICIES)
	int GetHappinessPerXPopulationGlobal() const;
	EraTypes GetPolicyEraUnlock() const;
	int GetIdeologyPoint() const;
	bool IsCorporationOfficesAsFranchises() const;
	bool IsCorporationFreeFranchiseAbovePopular() const;
	bool IsCorporationRandomForeignFranchise() const;
	int GetAdditionalNumFranchisesMod() const;
	bool IsUpgradeCSTerritory() const;
	int GetArchaeologicalDigTourism() const;
	int GetGoldenAgeTourism() const;
	int GetExtraCultureandScienceTradeRoutes() const;
	int GetTradeRouteLandDistanceModifier() const;
	int GetTradeRouteSeaDistanceModifier() const;
	int GetEspionageModifier() const;
	int GetXCSAlliesLowersPolicyNeedWonders() const;
	int GetTRSpeedBoost() const;
	int GetTRVisionBoost() const;
	int GetHappinessPerXPolicies() const;
	int GetHappinessPerXGreatWorks() const;
#endif
	int GetExtraHappinessPerLuxury() const;
	int GetUnhappinessFromUnitsMod() const;
	int GetNumExtraBuilders() const;
	int GetPlotGoldCostMod() const;
#if defined(MOD_POLICIES_CITY_WORKING)
	int GetCityWorkingChange() const;
#endif
	int GetPlotCultureCostModifier() const;
	int GetPlotCultureExponentModifier() const;
	int GetNumCitiesPolicyCostDiscount() const;
	int GetGarrisonedCityRangeStrikeModifier() const;
	int GetUnitPurchaseCostModifier() const;
	int GetBuildingPurchaseCostModifier() const;
	int GetCityConnectionTradeRouteGoldModifier() const;
	int GetTradeMissionGoldModifier() const;
	int GetFaithCostModifier() const;
	int GetCulturalPlunderMultiplier() const;
	int GetStealTechSlowerModifier() const;
	int GetStealTechFasterModifier() const;
	int GetCatchSpiesModifier() const;
#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
	int GetConversionModifier() const;
#endif
	int GetGoldPerUnit() const;
	int GetGoldPerMilitaryUnit() const;
	int GetCityStrengthMod() const;
	int GetCityGrowthMod() const;
	int GetCapitalGrowthMod() const;
	int GetSettlerProductionModifier() const;
	int GetCapitalSettlerProductionModifier() const;
	int GetNewCityExtraPopulation() const;
	int GetFreeFoodBox() const;
	int GetImprovementGoldMaintenanceMod() const;
#if defined(MOD_CIV6_WORKER)
	int GetRouteBuilderCostMod() const;
#endif
	int GetBuildingGoldMaintenanceMod() const;
	int GetUnitGoldMaintenanceMod() const;
	int GetUnitSupplyMod() const;
	int GetHappyPerMilitaryUnit() const;
	int GetFreeSpecialist() const;
	int GetTechPrereq() const;
	int GetMaxConscript() const;
	int GetExpModifier() const;
	int GetExpInBorderModifier() const;
	int GetMinorQuestFriendshipMod() const;
	int GetMinorGoldFriendshipMod() const;
	int GetMinorFriendshipMinimum() const;
	int GetMinorFriendshipDecayMod() const;
	int GetOtherPlayersMinorFriendshipDecayMod() const;
	int GetCityStateUnitFrequencyModifier() const;
	int GetCommonFoeTourismModifier() const;
	int GetLessHappyTourismModifier() const;
	int GetSharedIdeologyTourismModifier() const;
	int GetLandTradeRouteGoldChange() const;
	int GetSeaTradeRouteGoldChange() const;
	int GetSharedIdeologyTradeGoldChange() const;
	int GetRiggingElectionModifier() const;
	int GetMilitaryUnitGiftExtraInfluence() const;
	int GetProtectedMinorPerTurnInfluence() const;
	int GetAfraidMinorPerTurnInfluence() const;
	int GetMinorBullyScoreModifier() const;
	int GetThemingBonusMultiplier() const;
	int GetInternalTradeRouteYieldModifier() const;
#if defined(MOD_BALANCE_CORE)
	int GetPositiveWarScoreTourismMod() const;
	int GetInternalTradeRouteYieldModifierCapital() const;
	int GetTradeRouteYieldModifierCapital() const;
	BuildingClassTypes GetNewCityFreeBuilding() const;
	
	bool IsNoCSDecayAtWar() const;
	bool CanBullyFriendlyCS() const;
	int GetBullyGlobalCSReduction() const;
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	bool IsVassalsNoRebel() const;
	int GetVassalCSBonusModifier() const;
#endif
	int GetSharedReligionTourismModifier() const;
	int GetTradeRouteTourismModifier() const;
	int GetOpenBordersTourismModifier() const;
	int GetCityStateTradeChange() const;
	bool IsMinorGreatPeopleAllies() const;
	bool IsMinorScienceAllies() const;
	bool IsMinorResourceBonus() const;
	int GetPolicyBranchType() const;
	int GetNumExtraBranches() const;
	int GetHappinessToCulture() const;
	int GetHappinessToScience() const;
	int GetNumCitiesFreeCultureBuilding() const;
	int GetNumCitiesFreeFoodBuilding() const;
	bool IsHalfSpecialistUnhappiness() const;
	bool IsHalfSpecialistFood() const;
#if defined(MOD_BALANCE_CORE)
	bool IsHalfSpecialistFoodCapital() const;
	int GetStealGWSlowerModifier() const;
	int GetStealGWFasterModifier() const;
	int GetEventTourism() const;
	int GetEventTourismCS() const;
	int GetMonopolyModFlat() const;
	int GetMonopolyModPercent() const;
	bool IsDummy() const;
	bool IsOpener() const;
	bool IsFinisher() const;
	int GetCityStateCombatModifier() const;
	int GetGreatEngineerRateModifier() const;
	int GetDefenseBoost() const;
#endif
	bool IsMilitaryFoodProduction() const;
	int GetWoundedUnitDamageMod() const;
	int GetUnitUpgradeCostMod() const;
	int GetBarbarianCombatBonus() const;
	bool IsAlwaysSeeBarbCamps() const;
	bool IsRevealAllCapitals() const;
	bool IsGarrisonFreeMaintenance() const;
	bool IsGoldenAgeCultureBonusDisabled() const;
	bool IsSecondReligionPantheon() const;
	bool IsAddReformationBelief() const;
	bool IsEnablesSSPartHurry() const;
	bool IsEnablesSSPartPurchase() const;
	bool IsAbleToAnnexCityStates() const;
	std::string pyGetWeLoveTheKing()
	{
		return GetWeLoveTheKing();
	}
	const char* GetWeLoveTheKing();
	void SetWeLoveTheKingKey(const char* szVal);

	// Accessor Functions (Arrays)
	int GetPrereqOrPolicies(int i) const;
	int GetPrereqAndPolicies(int i) const;
	int GetPolicyDisables(int i) const;
	int GetYieldModifier(int i) const;
	int* GetYieldModifierArray() const;
	int GetCityYieldChange(int i) const;
	int* GetCityYieldChangeArray() const;
	int GetCoastalCityYieldChange(int i) const;
	int* GetCoastalCityYieldChangeArray() const;
	int GetCapitalYieldChange(int i) const;
	int* GetCapitalYieldChangeArray() const;
	int GetCapitalYieldPerPopChange(int i) const;
	int* GetCapitalYieldPerPopChangeArray() const;
	int GetCapitalYieldPerPopChangeEmpire(int i) const;
	int* GetCapitalYieldPerPopChangeEmpireArray() const;
	int GetCapitalYieldModifier(int i) const;
	int* GetCapitalYieldModifierArray() const;
	int GetGreatWorkYieldChange(int i) const;
	int* GetGreatWorkYieldChangeArray() const;
	int GetSpecialistExtraYield(int i) const;
	int* GetSpecialistExtraYieldArray() const;
	int IsFreePromotion(int i) const;
	bool IsFreePromotionUnitCombat(const int promotionID, const int unitCombatID) const;
#if defined(MOD_RELIGION_POLICY_BRANCH_FAITH_GP)
	bool HasFaithPurchaseUnitClasses() const;
	bool IsFaithPurchaseUnitClass(const int eUnitClass, const int eCurrentEra) const;
#endif
	int GetUnitCombatProductionModifiers(int i) const;
	int GetUnitCombatFreeExperiences(int i) const;
	int GetBuildingClassCultureChange(int i) const;
	int GetBuildingClassHappiness(int i) const;
	int GetBuildingClassProductionModifier(int i) const;
	int GetUnitClassProductionModifiers(int i) const;
	int GetBuildingClassTourismModifier(int i) const;
	int GetNumFreeUnitsByClass(int i) const;
	int GetTourismByUnitClassCreated(int i) const;
	int GetImprovementCultureChanges(int i) const;

	int GetHurryModifier(int i) const;
	bool IsSpecialistValid(int i) const;
#if defined(MOD_BALANCE_CORE)
	int GetFreeChosenBuilding(int i) const;
#endif
#if defined(MOD_BALANCE_CORE_POLICIES)
	int GetResourceFromCSAlly(int i) const;
	int GetYieldFromBirth(int i) const;
	int GetYieldFromBirthCapital(int i) const;
	int GetYieldFromBirthRetroactive(int i) const;
	int GetYieldFromBirthCapitalRetroactive(int i) const;
	int GetYieldFromConstruction(int i) const;
	int GetYieldFromWonderConstruction(int i) const;
	int GetYieldFromTech(int i) const;
	bool GetNoUnhappinessExpansion() const;
	bool GetNoUnhappyIsolation() const;
	bool GetDoubleBorderGA() const;
	int GetIncreasedQuestInfluence() const;
	int GetGreatScientistBeakerModifier() const;
	int GetGreatEngineerHurryModifier() const;
	int GetTechCostXCitiesMod() const;
	int GetTourismCostXCitiesMod() const;
	int GetCitadelBoost() const;
	int GetPuppetProdMod() const;
	int GetOccupiedProdMod() const;
	int GetInternalTradeGold() const;
	int GetFreeWCVotes() const;
	int GetInfluenceGPExpend() const;
	int GetFreeTradeRoute() const;
	int GetFreeSpy() const;
	int GetReligionDistance() const;
	int GetPressureMod() const;
	int GetYieldFromBorderGrowth(int i) const;
	int GetYieldGPExpend(int i) const;
	int GetGarrisonsOccupiedUnhapppinessMod() const;
	int GetTradeReligionModifier() const;
	int GetBestRangedUnitSpawnSettle() const;
	int GetBestNumberLandCombatUnitClass() const;
	int GetBestNumberLandRangedUnitClass() const;
	int GetBestNumberSeaCombatUnitClass() const;
	int GetBestNumberSeaRangedUnitClass() const;
	int GetConquerorYield(int i) const;
	int GetFounderYield(int i) const;
	int GetReligionBuildingYieldMod(int i, int j) const;
	int GetReligionYieldMod(int i) const;
	int GetGoldenAgeYieldMod(int i) const;
	int GetFreePopulation() const;
	int GetExtraMoves() const;
	int GetMaxCorps() const;
	int GetRazingSpeedBonus() const;
	bool IsNoPartisans() const;
	int GetExtraSupplyPerPopulation() const;
#endif
#if defined(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
	int GetInvestmentModifier() const;
#endif
	int GetImprovementYieldChanges(int i, int j) const;
#if defined(MOD_API_UNIFIED_YIELDS) && defined(MOD_API_PLOT_YIELDS)
	int GetPlotYieldChanges(int i, int j) const;
#endif
#if defined(MOD_API_UNIFIED_YIELDS)
	int GetFeatureYieldChanges(int i, int j) const;
	int GetCityYieldFromUnimprovedFeature(int i, int j) const;
	int GetUnimprovedFeatureYieldChanges(int i, int j) const;
	int GetResourceYieldChanges(int i, int j) const;
	int GetTerrainYieldChanges(int i, int j) const;
	int GetTradeRouteYieldChange(int i, int j) const;
	int GetSpecialistYieldChanges(int i, int j) const;
	int GetGreatPersonExpendedYield(int i, int j) const;
	int GetGoldenAgeGreatPersonRateModifier(int i) const;
	int GetYieldFromKills(int i) const;
	int* GetYieldFromKillsArray() const;
	int GetYieldFromBarbarianKills(int i) const;
	int* GetYieldFromBarbarianKillsArray() const;
	int GetYieldChangeTradeRoute(int i) const;
	int* GetYieldChangeTradeRouteArray() const;
	int GetYieldChangesNaturalWonder(int i) const;
	int* GetYieldChangesNaturalWonderArray() const;
	int GetYieldChangeWorldWonder(int i) const;
	int* GetYieldChangeWorldWonderArray() const;
	int GetYieldFromMinorDemand(int i) const;
	int* GetYieldFromMinorDemandArray() const;
	int GetYieldFromWLTKD(int i) const;
	int* GetYieldFromWLTKDArray() const;

	int GetArtifactYieldChanges(int i) const;
	int* GetArtifactYieldChangesArray() const;

	int GetArtYieldChanges(int i) const;
	int* GetArtYieldChangesArray() const;

	int GetMusicYieldChanges(int i) const;
	int* GetMusicYieldChangesArray() const;

	int GetLitYieldChanges(int i) const;
	int* GetLitYieldChangesArray() const;

	int GetRelicYieldChanges(int i) const;
	int* GetRelicYieldChangesArray() const;
	int GetFilmYieldChanges(int i) const;
	int* GetFilmYieldChangesArray() const;

	bool IsOnlyTradeSameIdeology() const;

	int GetYieldFromNonSpecialistCitizens(int i) const;
	int* GetYieldFromNonSpecialistCitizensArray() const;

	int GetYieldModifierFromGreatWorks(int i) const;
	int* GetYieldModifierFromGreatWorksArray() const;

	int GetYieldModifierFromActiveSpies(int i) const;
	int* GetYieldModifierFromActiveSpiesArray() const;

	int GetYieldFromDelegateCount(int i) const;
	int* GetYieldFromDelegateCountArray() const;

	int GetMissionInfluenceModifier() const;

	bool IsCSResourcesForMonopolies() const;
	
	int GetHappinessPerActiveTradeRoute() const;
#endif
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
	int GetInternationalRouteYieldModifier(int i) const;
	int* GetInternationalRouteYieldModifiersArray();
#endif
	int GetBuildingClassYieldModifiers(int i, int j) const;
	int GetBuildingClassYieldChanges(int i, int j) const;
	int GetFlavorValue(int i) const;

	bool IsOneShot() const;
	bool IncludesOneShotFreeUnits() const;

	BuildingTypes GetFreeBuildingOnConquest() const;
#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
	int GetPovertyHappinessChangePolicy() const;
	int GetDefenseHappinessChangePolicy() const;
	int GetUnculturedHappinessChangePolicy() const;
	int GetIlliteracyHappinessChangePolicy() const;
	int GetMinorityHappinessChangePolicy() const;

	int GetPovertyHappinessChangePolicyCapital() const;
	int GetDefenseHappinessChangePolicyCapital() const;
	int GetUnculturedHappinessChangePolicyCapital() const;
	int GetIlliteracyHappinessChangePolicyCapital() const;
	int GetMinorityHappinessChangePolicyCapital() const;
	int GetPuppetUnhappinessMod() const;
	int GetNoUnhappfromXSpecialists() const;
	int GetHappfromXSpecialists() const;
	int GetNoUnhappfromXSpecialistsCapital() const;
	int GetSpecialistFoodChange() const;

	int GetWarWearinessModifier() const;
	int GetWarScoreModifier() const;

	int GetGreatGeneralExtraBonus() const;
#endif
private:
	int m_iTechPrereq;
	int m_iCultureCost;
	int m_iGridX;
	int m_iGridY;
	int m_iLevel;
	int m_iPolicyCostModifier;
	int m_iCulturePerCity;
	int m_iCulturePerWonder;
	int m_iCultureWonderMultiplier;
	int m_iCulturePerTechResearched;
	int m_iCultureImprovementChange;
	int m_iCultureFromKills;
	int m_iCultureFromBarbarianKills;
	int m_iGoldFromKills;
	int m_iEmbarkedExtraMoves;
	int m_iAttackBonusTurns;
	int m_iGoldenAgeTurns;
	int m_iGoldenAgeMeterMod;
	int m_iGoldenAgeDurationMod;
	int m_iNumFreeTechs;
	int m_iNumFreePolicies;
	int m_iNumFreeGreatPeople;
	int m_iMedianTechPercentChange;
	int m_iStrategicResourceMod;
	int m_iWonderProductionModifier;
	int m_iBuildingProductionModifier;
	int m_iGreatPeopleRateModifier;
	int m_iGreatGeneralRateModifier;
	int m_iGreatAdmiralRateModifier;
	int m_iGreatWriterRateModifier;
	int m_iGreatArtistRateModifier;
	int m_iGreatMusicianRateModifier;
	int m_iGreatMerchantRateModifier;
	int m_iGreatScientistRateModifier;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	int m_iGreatDiplomatRateModifier;
#endif
	int m_iDomesticGreatGeneralRateModifier;
	int m_iExtraHappiness;
	int m_iExtraHappinessPerCity;
#if defined(HH_MOD_NATURAL_WONDER_MODULARITY)
	int m_iExtraNaturalWonderHappiness;
#endif
	int m_iUnhappinessMod;
	int m_iCityCountUnhappinessMod;
	int m_iOccupiedPopulationUnhappinessMod;
	int m_iCapitalUnhappinessMod;
	int m_iFreeExperience;
	int m_iWorkerSpeedModifier;
	int m_iAllFeatureProduction;
	int m_iImprovementCostModifier;
	int m_iImprovementUpgradeRateModifier;
	int m_iSpecialistProductionModifier;
	int m_iSpecialistUpgradeModifier;
	int m_iMilitaryProductionModifier;
	int m_iBaseFreeUnits;
	int m_iBaseFreeMilitaryUnits;
	int m_iFreeUnitsPopulationPercent;
	int m_iFreeMilitaryUnitsPopulationPercent;
	int m_iHappinessPerGarrisonedUnit;
	int m_iCulturePerGarrisonedUnit;
	int m_iHappinessPerTradeRoute;
	int m_iHappinessPerXPopulation;
#if defined(MOD_BALANCE_CORE_POLICIES)
	int m_iHappinessPerXPopulationGlobal;
	EraTypes m_ePolicyEraUnlock;
	int m_iIdeologyPoint;
#endif
	int m_iExtraHappinessPerLuxury;
	int m_iUnhappinessFromUnitsMod;
	int m_iNumExtraBuilders;
	int m_iPlotGoldCostMod;
#if defined(MOD_POLICIES_CITY_WORKING)
	int m_iCityWorkingChange;
#endif
	int m_iPlotCultureCostModifier;
	int m_iPlotCultureExponentModifier;
	int m_iNumCitiesPolicyCostDiscount;
	int m_iGarrisonedCityRangeStrikeModifier;
	int m_iUnitPurchaseCostModifier;
	int m_iBuildingPurchaseCostModifier;
	int m_iCityConnectionTradeRouteGoldModifier;
	int m_iTradeMissionGoldModifier;
	int m_iFaithCostModifier;
	int m_iCulturalPlunderMultiplier;
	int m_iStealTechSlowerModifier;
	int m_iStealTechFasterModifier;
	int m_iCatchSpiesModifier;
#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
	int m_iConversionModifier;
#endif
	int m_iGoldPerUnit;
	int m_iGoldPerMilitaryUnit;
	int m_iCityStrengthMod;
	int m_iCityGrowthMod;
	int m_iCapitalGrowthMod;
	int m_iSettlerProductionModifier;
	int m_iCapitalSettlerProductionModifier;
	int m_iNewCityExtraPopulation;
	int m_iFreeFoodBox;
	int m_iImprovementGoldMaintenanceMod;
#if defined(MOD_CIV6_WORKER)
	int m_iRouteBuilderCostMod;
#endif
	int m_iBuildingGoldMaintenanceMod;
	int m_iUnitGoldMaintenanceMod;
	int m_iUnitSupplyMod;
	int m_iHappyPerMilitaryUnit;
	int m_iExpModifier;
	int m_iExpInBorderModifier;
	int m_iMinorQuestFriendshipMod;
	int m_iMinorGoldFriendshipMod;
	int m_iMinorFriendshipMinimum;
	int m_iMinorFriendshipDecayMod;
	int m_iOtherPlayersMinorFriendshipDecayMod;
	int m_iCityStateUnitFrequencyModifier;
	int m_iCommonFoeTourismModifier;
	int m_iLessHappyTourismModifier;
	int m_iSharedIdeologyTourismModifier;
	int m_iLandTradeRouteGoldChange;
	int m_iSeaTradeRouteGoldChange;
	int m_iSharedIdeologyTradeGoldChange;
	int m_iRiggingElectionModifier;
	int m_iMilitaryUnitGiftExtraInfluence;
	int m_iProtectedMinorPerTurnInfluence;
	int m_iAfraidMinorPerTurnInfluence;
	int m_iMinorBullyScoreModifier;
	int m_iThemingBonusMultiplier;
	int m_iInternalTradeRouteYieldModifier;

	int m_iPositiveWarScoreTourismMod;
	int m_iInternalTradeRouteYieldModifierCapital;
	int m_iTradeRouteYieldModifierCapital;
	BuildingClassTypes m_eNewCityFreeBuilding;

	bool m_bNoCSDecayAtWar;
	bool m_bBullyFriendlyCS;
	int m_iBullyGlobalCSReduction;
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	bool m_bVassalsNoRebel;
	int m_iVassalCSBonusModifier;
#endif

	int m_iSharedReligionTourismModifier;
	int m_iTradeRouteTourismModifier;
	int m_iOpenBordersTourismModifier;
	int m_iCityStateTradeChange;
	bool m_bMinorGreatPeopleAllies;
	bool m_bMinorScienceAllies;
	bool m_bMinorResourceBonus;
	int m_iFreeSpecialist;
	int m_iMaxConscript;
	int m_iPolicyBranchType;
	int m_iNumExtraBranches;
	int m_iWoundedUnitDamageMod;
	int m_iUnitUpgradeCostMod;
	int m_iBarbarianCombatBonus;
	int m_iHappinessToCulture;
	int m_iHappinessToScience;
	int m_iNumCitiesFreeCultureBuilding;
	int m_iNumCitiesFreeFoodBuilding;

	bool m_bHalfSpecialistUnhappiness;
	bool m_bHalfSpecialistFood;
#if defined(MOD_BALANCE_CORE)
	int m_iStealGWSlowerModifier;
	int m_iStealGWFasterModifier;
	bool m_bHalfSpecialistFoodCapital;
	int m_iEventTourism;
	int m_iEventTourismCS;
	int m_iMonopolyModFlat;
	int m_iMonopolyModPercent;
	bool m_bDummy;
	bool m_bOpener;
	bool m_bFinisher;
	int m_iCityStateCombatModifier;
	int m_iGreatEngineerRateModifier;
	int m_iDefenseBoost;
#endif
	bool m_bMilitaryFoodProduction;
	bool m_bAlwaysSeeBarbCamps;
	bool m_bRevealAllCapitals;
	bool m_bGarrisonFreeMaintenance;
	bool m_bGoldenAgeCultureBonusDisabled;
	bool m_bSecondReligionPantheon;
	bool m_bAddReformationBelief;
	bool m_bEnablesSSPartHurry;
	bool m_bEnablesSSPartPurchase;
	bool m_bAbleToAnnexCityStates;

	bool m_bIsOnlyTradeSameIdeology;
	bool m_bOneShot;
	bool m_bIncludesOneShotFreeUnits;
#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
	int m_iPovertyHappinessChangePolicy;
	int m_iDefenseHappinessChangePolicy;
	int m_iUnculturedHappinessChangePolicy;
	int m_iIlliteracyHappinessChangePolicy;
	int m_iMinorityHappinessChangePolicy;

	int m_iPovertyHappinessChangePolicyCapital;
	int m_iDefenseHappinessChangePolicyCapital;
	int m_iUnculturedHappinessChangePolicyCapital;
	int m_iIlliteracyHappinessChangePolicyCapital;
	int m_iMinorityHappinessChangePolicyCapital;
	int m_iPuppetUnhappinessModPolicy;
	int m_iNoUnhappfromXSpecialists;
	int m_iHappfromXSpecialists;
	int m_iNoUnhappfromXSpecialistsCapital;
	int m_iSpecialistFoodChange;
	int m_iWarWearinessModifier;
	int m_iWarScoreModifier;

	int m_iGreatGeneralExtraBonus;
#endif

	CvString m_strWeLoveTheKingKey;
	CvString m_wstrWeLoveTheKing;

	BuildingTypes m_eFreeBuildingOnConquest;

	// Arrays
	std::multimap<int, int> m_FreePromotionUnitCombats;
#if defined(MOD_RELIGION_POLICY_BRANCH_FAITH_GP)
	std::multimap<int, int> m_FaithPurchaseUnitClasses;
#endif
	int* m_piPrereqOrPolicies;
	int* m_piPrereqAndPolicies;
	int* m_piPolicyDisables;
	int* m_piYieldModifier;
	int* m_piCityYieldChange;
	int* m_piCoastalCityYieldChange;
	int* m_piCapitalYieldChange;
	int* m_piCapitalYieldPerPopChange;
	int* m_piCapitalYieldPerPopChangeEmpire;
	int* m_piCapitalYieldModifier;
	int* m_piGreatWorkYieldChange;
	int* m_piSpecialistExtraYield;
	int* m_piImprovementCultureChange;
	bool* m_pabFreePromotion;
	int* m_paiUnitCombatProductionModifiers;
	int* m_paiUnitCombatFreeExperiences;
	int* m_paiHurryModifier;
	int* m_paiBuildingClassCultureChanges;
	int* m_paiBuildingClassProductionModifiers;
	int* m_paiUnitClassProductionModifiers;
	int* m_paiBuildingClassTourismModifiers;
	int* m_paiBuildingClassHappiness;
	int* m_paiFreeUnitClasses;
	int* m_paiTourismOnUnitCreation;
#if defined(MOD_BALANCE_CORE)
	int* m_paiFreeChosenBuilding;
#endif
#if defined(MOD_BALANCE_CORE_POLICIES)
	int* m_piResourcefromCSAlly;
	int* m_piYieldFromBirth;
	int* m_piYieldFromBirthCapital;
	int* m_piYieldFromBirthRetroactive;
	int* m_piYieldFromBirthCapitalRetroactive;
	int* m_piYieldFromConstruction;
	int* m_piYieldFromWonderConstruction;
	int* m_piYieldFromTech;
	bool m_bNoUnhappinessExpansion;
	bool m_bNoUnhappyIsolation;
	bool m_bDoubleBorderGA;
	int m_iGreatScientistBeakerModifier;
	int m_iGreatEngineerHurryModifier;
	int m_iTechCostXCitiesMod;
	int m_iTourismCostXCitiesMod;
	int m_iIncreasedQuestInfluence;
	int m_iPuppetProdMod;
	int m_iOccupiedProdMod;
	int m_iCitadelBoost;
	int m_iInternalTradeGold;
	int m_iFreeWCVotes;
	int m_iInfluenceGPExpend;
	int m_iFreeTradeRoute;
	int m_iFreeSpy;
	int m_iReligionDistance;
	int m_iPressureMod;
	int* m_piYieldFromBorderGrowth;
	int* m_piYieldGPExpend;
	int m_iGarrisonsOccupiedUnhapppinessMod;
	int m_iTradeReligionModifier;
	int m_iBestRangedUnitSpawnSettle;
	int m_iBestNumberLandCombatUnitClass;
	int m_iBestNumberLandRangedUnitClass;
	int m_iBestNumberSeaCombatUnitClass;
	int m_iBestNumberSeaRangedUnitClass;
	int m_iFreePopulation;
	int m_iExtraMoves;
	int m_iMaxCorporations;
	int m_iRazingSpeedBonus;
	int m_iExtraSupplyPerPopulation;
	bool m_bNoPartisans;
	bool m_bCorporationOfficesAsFranchises;
	bool m_bCorporationFreeFranchiseAbovePopular;
	bool m_bCorporationRandomForeignFranchise;
	int m_iAdditionalNumFranchisesMod;
	bool m_bUpgradeCSTerritory;
	int m_iArchaeologicalDigTourism;
	int m_iGoldenAgeTourism;
	int m_iExtraCultureandScienceTradeRoutes;
	int m_iTradeRouteLandDistanceModifier;
	int m_iTradeRouteSeaDistanceModifier;
	int m_iEspionageModifier;
	int* m_piConquerorYield;
	int* m_piFounderYield;
	int* m_piReligionYieldMod;
	int* m_piGoldenAgeYieldMod;
	int** m_ppiReligionBuildingYieldMod;
	int m_iXCSAlliesLowersPolicyNeedWonders;
	int m_iTRSpeedBoost;
	int m_iTRVisionBoost;
	int m_iHappinessPerXPolicies;
	int m_iHappinessPerXGreatWorks;
#endif
#if defined(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
	int m_iInvestmentModifier;
#endif
//	bool* m_pabHurry;
	bool* m_pabSpecialistValid;
	int** m_ppiImprovementYieldChanges;
#if defined(MOD_API_UNIFIED_YIELDS) && defined(MOD_API_PLOT_YIELDS)
	int** m_ppiPlotYieldChanges;
#endif
#if defined(MOD_API_UNIFIED_YIELDS)
	int** m_ppiFeatureYieldChanges;
	int** m_ppiCityYieldFromUnimprovedFeature;
	int** m_ppiUnimprovedFeatureYieldChanges;
	int** m_ppiResourceYieldChanges;
	int** m_ppiTerrainYieldChanges;
	int** m_ppiTradeRouteYieldChange;
	int** m_ppiSpecialistYieldChanges;
	int** m_ppiGreatPersonExpendedYield;
	int* m_piGoldenAgeGreatPersonRateModifier;
	int* m_piYieldFromKills;
	int* m_piYieldFromBarbarianKills;
	int* m_piYieldChangeTradeRoute;
	int* m_piYieldChangesNaturalWonder;
	int* m_piYieldChangeWorldWonder;
	int* m_piYieldFromMinorDemand;
	int* m_piYieldFromWLTKD;
	int* m_piArtifactYieldChanges;
	int* m_piArtYieldChanges;
	int* m_piLitYieldChanges;
	int* m_piMusicYieldChanges;
	int* m_piRelicYieldChanges;
	int* m_piFilmYieldChanges;
	int* m_piYieldFromNonSpecialistCitizens;
	int* m_piYieldModifierFromGreatWorks;
	int* m_piYieldModifierFromActiveSpies;
	int* m_piYieldFromDelegateCount;
	int m_iMissionInfluenceModifier;
	int m_iHappinessPerActiveTradeRoute;
	bool m_bCSResourcesForMonopolies;
#endif
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
	int* m_piInternationalRouteYieldModifiers;
#endif
	int** m_ppiBuildingClassYieldModifiers;
	int** m_ppiBuildingClassYieldChanges;
	int* m_piFlavorValue;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvPolicyBranchEntry
//!  \brief		A branch that encompasses Policies
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPolicyBranchEntry: public CvBaseInfo
{
public:
	CvPolicyBranchEntry(void);
	~CvPolicyBranchEntry(void);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	// Accessor Functions (Non-Arrays)
	int GetEraPrereq() const;
	int GetFreePolicy() const;
	int GetFreeFinishingPolicy() const;
	int GetFirstAdopterFreePolicies() const;
	int GetSecondAdopterFreePolicies() const;
	bool IsPurchaseByLevel() const;
	bool IsLockedWithoutReligion() const;
	bool IsMutuallyExclusive() const;
	bool IsDelayWhenNoReligion() const;
	bool IsDelayWhenNoCulture() const;
	bool IsDelayWhenNoCityStates() const;
	bool IsDelayWhenNoScience() const;
#if defined(MOD_BALANCE_CORE)
	int GetNumPolicyRequirement() const;
#endif

	// Accessor Functions (Arrays)
	int GetPolicyBranchDisables(int i) const;

private:
	int m_iEraPrereq;
	int m_iFreePolicy;
	int m_iFreeFinishingPolicy;
	int m_iFirstAdopterFreePolicies;
	int m_iSecondAdopterFreePolicies;
	bool m_bPurchaseByLevel;
	bool m_bLockedWithoutReligion;
	bool m_bMutuallyExclusive;
	bool m_bDelayWhenNoReligion;
	bool m_bDelayWhenNoCulture;
	bool m_bDelayWhenNoCityStates;
	bool m_bDelayWhenNoScience;
#if defined(MOD_BALANCE_CORE)
	int m_iNumPolicyRequirement;
#endif

	// Arrays
	int* m_piPolicyBranchDisables;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvPolicyXMLEntries
//!  \brief		Game-wide information about the policy tree
//
//! Key Attributes:
//! - Plan is it will be contained in CvGameRules object within CvGame class
//! - Populated from XML\GameInfo\CIV5PolicyInfos.xml
//! - Contains an array of CvPolicyEntry from the above XML file
//! - One instance for the entire game
//! - Accessed heavily by CvPlayerPolicies class (which stores the policy state for 1 player)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPolicyXMLEntries
{
public:
	CvPolicyXMLEntries(void);
	~CvPolicyXMLEntries(void);

	// Policy functions
	std::vector<CvPolicyEntry*>& GetPolicyEntries();
	int GetNumPolicies();
	CvPolicyEntry* GetPolicyEntry(int index);

	void DeletePoliciesArray();

	// Policy Branch functions
	std::vector<CvPolicyBranchEntry*>& GetPolicyBranchEntries();
	int GetNumPolicyBranches();
	_Ret_maybenull_ CvPolicyBranchEntry* GetPolicyBranchEntry(int index);

	void DeletePolicyBranchesArray();

private:
	std::vector<CvPolicyEntry*> m_paPolicyEntries;
	std::vector<CvPolicyBranchEntry*> m_paPolicyBranchEntries;
};

enum PolicyModifierType
{
    POLICYMOD_EXTRA_HAPPINESS = 0,
    POLICYMOD_EXTRA_HAPPINESS_PER_CITY,
#if defined(HH_MOD_NATURAL_WONDER_MODULARITY)
	POLICYMOD_EXTRA_NATURALWONDER_HAPPINESS,
#endif
    POLICYMOD_GREAT_PERSON_RATE,
    POLICYMOD_GREAT_GENERAL_RATE,
    POLICYMOD_DOMESTIC_GREAT_GENERAL_RATE,
    POLICYMOD_POLICY_COST_MODIFIER,
    POLICYMOD_RELIGION_PRODUCTION_MODIFIER,
    POLICYMOD_WONDER_PRODUCTION_MODIFIER,
    POLICYMOD_BUILDING_PRODUCTION_MODIFIER,
    POLICYMOD_FREE_EXPERIENCE,
    POLICYMOD_EXTRA_CULTURE_FROM_IMPROVEMENTS,
    POLICYMOD_CULTURE_FROM_KILLS,
    POLICYMOD_EMBARKED_EXTRA_MOVES,
    POLICYMOD_CULTURE_FROM_BARBARIAN_KILLS,
    POLICYMOD_GOLD_FROM_KILLS,
    POLICYMOD_CULTURE_FROM_GARRISON,
    POLICYMOD_UNIT_FREQUENCY_MODIFIER,
    POLICYMOD_TRADE_MISSION_GOLD_MODIFIER,
    POLICYMOD_FAITH_COST_MODIFIER,
    POLICYMOD_CULTURAL_PLUNDER_MULTIPLIER,
    POLICYMOD_STEAL_TECH_SLOWER_MODIFIER,
    POLICYMOD_CATCH_SPIES_MODIFIER,
	POLICYMOD_GREAT_ADMIRAL_RATE,
	POLICYMOD_GREAT_WRITER_RATE,
	POLICYMOD_GREAT_ARTIST_RATE,
	POLICYMOD_GREAT_MUSICIAN_RATE,
	POLICYMOD_GREAT_MERCHANT_RATE,
#if defined(MOD_BALANCE_CORE)
	POLICYMOD_STEAL_GW_SLOWER_MODIFIER,
	POLICYMOD_STEAL_GW_FASTER_MODIFIER,
	POLICYMOD_GREAT_ENGINEER_RATE,
	POLICYMOD_CITY_DEFENSE_BOOST,
#endif
#if defined(MOD_DIPLOMACY_CITYSTATES)
	POLICYMOD_GREAT_DIPLOMAT_RATE,
#endif
	POLICYMOD_GREAT_SCIENTIST_RATE,
	POLICYMOD_TOURISM_MOD_COMMON_FOE,
	POLICYMOD_TOURISM_MOD_LESS_HAPPY,
	POLICYMOD_TOURISM_MOD_SHARED_IDEOLOGY,
	POLICYMOD_BUILDING_PURCHASE_COST_MODIFIER,
	POLICYMOD_LAND_TRADE_GOLD_CHANGE,
	POLICYMOD_SEA_TRADE_GOLD_CHANGE,
	POLICYMOD_SHARED_IDEOLOGY_TRADE_CHANGE,
	POLICYMOD_RIGGING_ELECTION_MODIFIER,
	POLICYMOD_MILITARY_UNIT_GIFT_INFLUENCE,
	POLICYMOD_PROTECTED_MINOR_INFLUENCE,
	POLICYMOD_AFRAID_INFLUENCE,
	POLICYMOD_MINOR_BULLY_SCORE_MODIFIER,
	POLICYMOD_STEAL_TECH_FASTER_MODIFIER,
	POLICYMOD_THEMING_BONUS,
	POLICYMOD_CITY_STATE_TRADE_CHANGE,
	POLICYMOD_INTERNAL_TRADE_MODIFIER,
#if defined(MOD_BALANCE_CORE)
	POLICYMOD_INTERNAL_TRADE_CAPITAL_MODIFIER,
	POLICYMOD_TRADE_CAPITAL_MODIFIER,
#endif
    POLICYMOD_SHARED_RELIGION_TOURISM_MODIFIER,
    POLICYMOD_TRADE_ROUTE_TOURISM_MODIFIER,
	POLICYMOD_OPEN_BORDERS_TOURISM_MODIFIER,
#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
    POLICYMOD_CONVERSION_MODIFIER,
#endif
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvPlayerPolicies
//!  \brief		Information about the policies of a single player
//
//!  Key Attributes:
//!  - Plan is it will be contained in CvPlayerState object within CvPlayer class
//!  - One instance for each civ (player or AI)
//!  - Accessed by any class that needs to check policy state
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPlayerPolicies: public CvFlavorRecipient
{
public:
	CvPlayerPolicies(void);
	~CvPlayerPolicies(void);
	void Init(CvPolicyXMLEntries* pPolicies, CvPlayer* pPlayer, bool bIsCity);
	void Uninit();
	void Reset();
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	// Flavor recipient required function
	void FlavorUpdate();

	CvPlayer* GetPlayer();

	// Accessor functions
	bool HasPolicy(PolicyTypes eIndex) const;
#if defined(MOD_API_EXTENSIONS)
	bool IsFreePolicy(PolicyTypes eIndex) const;
	void SetPolicy(PolicyTypes eIndex, bool bNewValue, bool bFree);
#else
	void SetPolicy(PolicyTypes eIndex, bool bNewValue);
#endif
#if defined(MOD_BALANCE_CORE)
	int GetNumPoliciesOwned(bool bSkipFinisher = false, bool bExcludeFree = false) const;
#else
	int GetNumPoliciesOwned() const;
#endif
	int GetNumPoliciesOwnedInBranch(PolicyBranchTypes eBranch) const;
	CvPolicyXMLEntries* GetPolicies() const;
#if defined(MOD_BALANCE_CORE)
	void ClearCache();
#endif
	// Functions to return benefits from policies
	int GetNumericModifier(PolicyModifierType eType);
	int GetYieldModifier(YieldTypes eYieldType);
	int GetBuildingClassYieldModifier(BuildingClassTypes eBuildingClass, YieldTypes eYieldType);
	int GetBuildingClassYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYieldType);
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
	int GetInternationalRouteYieldModifier(YieldTypes eYieldType);
#endif
#if defined(MOD_BALANCE_CORE_POLICIES)
	int GetReligionBuildingClassYieldModifier(BuildingClassTypes eBuildingClass, YieldTypes eYieldType);
#endif
	int GetBuildingClassProductionModifier(BuildingClassTypes eBuildingClass);
	int GetBuildingClassTourismModifier(BuildingClassTypes eBuildingClass);
	int GetBuildingClassHappinessModifier(BuildingClassTypes eBuildingClass);

	int GetImprovementCultureChange(ImprovementTypes eImprovement);

	bool HasPolicyEncouragingGarrisons() const;
	bool HasPolicyGrantingReformationBelief() const;
	CvString GetWeLoveTheKingString();
	std::vector<BuildingTypes> GetFreeBuildingsOnConquest();
	int GetTourismFromUnitCreation(UnitClassTypes eUnitClass) const;

	// Functions to give current player status with respect to policies
	int GetNextPolicyCost();
	bool CanAdoptPolicy(PolicyTypes eIndex, bool bIgnoreCost = false) const;
	int GetNumPoliciesCanBeAdopted();

	// Policy Branch Stuff
	void DoUnlockPolicyBranch(PolicyBranchTypes eBranchType);
	bool CanUnlockPolicyBranch(PolicyBranchTypes eBranchType);

	bool IsPolicyBranchUnlocked(PolicyBranchTypes eBranchType) const;
	void SetPolicyBranchUnlocked(PolicyBranchTypes eBranchType, bool bNewValue, bool bRevolution);
	int GetNumPolicyBranchesUnlocked() const;

	// Blocked branches (because of other branch choices)
	void DoSwitchToPolicyBranch(PolicyBranchTypes eBranchType);
	void SetPolicyBranchBlocked(PolicyBranchTypes eBranchType, bool bValue);
	bool IsPolicyBranchBlocked(PolicyBranchTypes eBranchType) const;
	bool IsPolicyBlocked(PolicyTypes eType) const;

#if defined(MOD_API_EXTENSIONS)
	bool CanAdoptIdeology(PolicyBranchTypes eIdeology) const;
	bool HasAdoptedIdeology(PolicyBranchTypes eIdeology) const;
#endif

	// Ideology change
	void DoSwitchIdeologies(PolicyBranchTypes eBranchType);
	void ClearPolicyBranch(PolicyBranchTypes eBranchType);

	// Finished branches
	int GetNumPolicyBranchesFinished() const;
	void SetPolicyBranchFinished(PolicyBranchTypes eBranchType, bool bValue);
	bool IsPolicyBranchFinished(PolicyBranchTypes eBranchType) const;
	bool WillFinishBranchIfAdopted(PolicyTypes eType) const;

	PolicyBranchTypes GetPolicyBranchChosen(int iID) const;
	void SetPolicyBranchChosen(int iID, PolicyBranchTypes eBranchType);
	int GetNumPolicyBranchesAllowed() const;

	int GetNumExtraBranches() const;
	void ChangeNumExtraBranches(int iChange);

	// Below is used to determine the "title" for the player
	void DoNewPolicyPickedForHistory(PolicyTypes ePolicy);
	PolicyBranchTypes GetDominantPolicyBranchForTitle() const;

	PolicyBranchTypes GetBranchPicked1() const;
	void SetBranchPicked1(PolicyBranchTypes eBranch);
	PolicyBranchTypes GetBranchPicked2() const;
	void SetBranchPicked2(PolicyBranchTypes eBranch);
	PolicyBranchTypes GetBranchPicked3() const;
	void SetBranchPicked3(PolicyBranchTypes eBranch);

	// functions to deal with one-shot effects
	bool HasOneShotPolicyFired(PolicyTypes eIndex) const;
	void SetOneShotPolicyFired(PolicyTypes eIndex, bool bFired);
	bool HaveOneShotFreeUnitsFired(PolicyTypes eIndex) const;
	void SetOneShotFreeUnitsFired(PolicyTypes eIndex, bool bFired);

	// IDEOLOGY
	PolicyBranchTypes GetLateGamePolicyTree() const;
	bool IsTimeToChooseIdeology() const;
	std::vector<PolicyTypes> GetAvailableTenets(PolicyBranchTypes eBranch, int iLevel);
	PolicyTypes GetTenet(PolicyBranchTypes eBranch, int iLevel, int iIndex);
	int GetNumTenetsOfLevel(PolicyBranchTypes eBranch, int iLevel) const;
	bool CanGetAdvancedTenet() const;

	// Functions to process AI each turn
	void DoPolicyAI();
	void DoChooseIdeology();

private:
	void AddFlavorAsStrategies(int iPropagatePercent);

#if defined(MOD_BALANCE_CORE)
	void UpdateModifierCache();
	std::vector<int> m_vBuildingClassTourismModifier;
	std::vector<int> m_vBuildingClassHappinessModifier;
#endif

	// Logging functions
	void LogFlavors(FlavorTypes eFlavor = NO_FLAVOR);

#if defined(MOD_API_EXTENSIONS)
	bool* m_pabFreePolicy;
#endif
	bool* m_pabHasPolicy;
	bool* m_pabHasOneShotPolicyFired;
	bool* m_pabHaveOneShotFreeUnitsFired;
	bool* m_pabPolicyBranchUnlocked;
	bool* m_pabPolicyBranchBlocked;
	bool* m_pabPolicyBranchFinished;
	int* m_paiPolicyBranchBlockedCount;
	int* m_paiPolicyBlockedCount;
	PolicyBranchTypes* m_paePolicyBranchesChosen;
	PolicyBranchTypes* m_paePolicyBlockedBranchCheck;
	CvPolicyXMLEntries* m_pPolicies;
	CvPolicyAI* m_pPolicyAI;
	CvPlayer* m_pPlayer;

	int m_iNumExtraBranches;

	PolicyBranchTypes m_eBranchPicked1;
	PolicyBranchTypes m_eBranchPicked2;
	PolicyBranchTypes m_eBranchPicked3;

	typedef std::map<PolicyModifierType,std::pair<int,int>> ModifierMap;
	ModifierMap mModifierLookup;
};

namespace PolicyHelpers
{
	int GetNumPlayersWithBranchUnlocked(PolicyBranchTypes eBranch);
	int GetNumFreePolicies(PolicyBranchTypes eBranch);
}

#endif //CIV5_POLICY_CLASSES_H