/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "CvPolicyAI.h"
#include "CvFlavorManager.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvEconomicAI.h"
#include "CvGrandStrategyAI.h"
#include "CvInfosSerializationHelper.h"

// Include this after all other headers.
#include "LintFree.h"

/// Constructor
CvPolicyEntry::CvPolicyEntry(void):
	m_iCultureCost(0),
	m_iGridX(0),
	m_iGridY(0),
	m_iLevel(0),
	m_iPolicyCostModifier(0),
	m_iCulturePerCity(0),
	m_iCulturePerWonder(0),
	m_iCultureWonderMultiplier(0),
	m_iCulturePerTechResearched(0),
	m_iCultureImprovementChange(0),
	m_iCultureFromKills(0),
	m_iCultureFromBarbarianKills(0),
	m_iGoldFromKills(0),
	m_iEmbarkedExtraMoves(0),
	m_iAttackBonusTurns(0),
	m_iGoldenAgeTurns(0),
	m_iGoldenAgeMeterMod(0),
	m_iGoldenAgeDurationMod(0),
	m_iNumFreeTechs(0),
	m_iNumFreePolicies(0),
	m_iNumFreeGreatPeople(0),
	m_iMedianTechPercentChange(0),
	m_iStrategicResourceMod(0),
	m_iWonderProductionModifier(0),
	m_iBuildingProductionModifier(0),
	m_iGreatPeopleRateModifier(0),
	m_iGreatGeneralRateModifier(0),
	m_iGreatAdmiralRateModifier(0),
	m_iGreatWriterRateModifier(0),
	m_iGreatArtistRateModifier(0),
	m_iGreatMusicianRateModifier(0),
	m_iGreatMerchantRateModifier(0),
	m_iGreatScientistRateModifier(0),
#if defined(MOD_DIPLOMACY_CITYSTATES)
	m_iGreatDiplomatRateModifier(0),
#endif
	m_iDomesticGreatGeneralRateModifier(0),
	m_iExtraHappiness(0),
	m_iExtraHappinessPerCity(0),
#if defined(HH_MOD_NATURAL_WONDER_MODULARITY)
	m_iExtraNaturalWonderHappiness(0),
#endif
	m_iUnhappinessMod(0),
	m_iCityCountUnhappinessMod(0),
	m_iOccupiedPopulationUnhappinessMod(0),
	m_iCapitalUnhappinessMod(0),
	m_iFreeExperience(0),
	m_iWorkerSpeedModifier(0),
	m_iAllFeatureProduction(0),
	m_iImprovementCostModifier(0),
	m_iImprovementUpgradeRateModifier(0),
	m_iSpecialistProductionModifier(0),
	m_iSpecialistUpgradeModifier(0),
	m_iMilitaryProductionModifier(0),
	m_iBaseFreeUnits(0),
	m_iBaseFreeMilitaryUnits(0),
	m_iFreeUnitsPopulationPercent(0),
	m_iFreeMilitaryUnitsPopulationPercent(0),
	m_iHappinessPerGarrisonedUnit(0),
	m_iCulturePerGarrisonedUnit(0),
	m_iHappinessPerTradeRoute(0),
	m_iHappinessPerXPopulation(0),
#if defined(MOD_BALANCE_CORE_POLICIES)
	m_iHappinessPerXPopulationGlobal(0),
	m_ePolicyEraUnlock(NO_ERA),
	m_iIdeologyPoint(0),
	m_bNoXPLossUnitPurchase(false),
	m_piGoldenAgeYieldMod(NULL),
	m_bNoForeignCorpsInCities(false),
	m_bNoFranchisesInForeignCities(false),
	m_iCorporationOfficesAsFranchises(0),
	m_iCorporationFreeFranchiseAbovePopular(0),
	m_iCorporationRandomForeignFranchise(0),
	m_iAdditionalNumFranchisesMod(0),
	m_iAdditionalNumFranchises(0),
	m_bUpgradeCSVassalTerritory(false),
	m_iArchaeologicalDigTourism(0),
	m_iGoldenAgeTourism(0),
	m_iExtraCultureandScienceTradeRoutes(0),
	m_iTradeRouteLandDistanceModifier(0),
	m_iTradeRouteSeaDistanceModifier(0),
	m_iEspionageModifier(0),
	m_iXCSAlliesLowersPolicyNeedWonders(0),
	m_iTRSpeedBoost(0),
	m_iTRVisionBoost(0),
	m_iHappinessPerXPolicies(0),
	m_iHappinessPerXGreatWorks(0),
	m_iExtraMissionaryStrength(0),
	m_iExtraMissionarySpreads(0),
#endif
	m_iExtraHappinessPerLuxury(0),
	m_iUnhappinessFromUnitsMod(0),
	m_iNumExtraBuilders(0),
	m_iPlotGoldCostMod(0),
#if defined(MOD_POLICIES_CITY_WORKING)
	m_iCityWorkingChange(0),
#endif
#if defined(MOD_POLICIES_CITY_AUTOMATON_WORKERS)
	m_iCityAutomatonWorkersChange(0),
#endif
	m_iPlotCultureCostModifier(0),
	m_iPlotCultureExponentModifier(0),
	m_iNumCitiesPolicyCostDiscount(0),
	m_iGarrisonedCityRangeStrikeModifier(0),
	m_iUnitPurchaseCostModifier(0),
	m_iBuildingPurchaseCostModifier(0),
	m_iCityConnectionTradeRouteGoldModifier(0),
	m_iTradeMissionGoldModifier(0),
	m_iFaithCostModifier(0),
	m_iCulturalPlunderMultiplier(0),
	m_iStealTechSlowerModifier(0),
	m_iStealTechFasterModifier(0),
	m_iCatchSpiesModifier(0),
#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
	m_iConversionModifier(0),
#endif
	m_iGoldPerUnit(0),
	m_iGoldPerMilitaryUnit(0),
	m_iCityStrengthMod(0),
	m_iCityGrowthMod(0),
	m_iCapitalGrowthMod(0),
	m_iSettlerProductionModifier(0),
	m_iCapitalSettlerProductionModifier(0),
	m_iNewCityExtraPopulation(0),
	m_iFreeFoodBox(0),
	m_iImprovementGoldMaintenanceMod(0),
#if defined(MOD_CIV6_WORKER)
	m_iRouteBuilderCostMod(0),
#endif
	m_iBuildingGoldMaintenanceMod(0),
	m_iUnitGoldMaintenanceMod(0),
	m_iUnitSupplyMod(0),
	m_iHappyPerMilitaryUnit(0),
	m_iFreeSpecialist(0),
	m_iTechPrereq(NO_TECH),
	m_iMaxConscript(0),
	m_iExpModifier(0),
	m_iExpInBorderModifier(0),
	m_iMinorQuestFriendshipMod(0),
	m_iMinorGoldFriendshipMod(0),
	m_iMinorFriendshipMinimum(0),
	m_iMinorFriendshipDecayMod(0),
	m_iOtherPlayersMinorFriendshipDecayMod(0),
	m_iCityStateUnitFrequencyModifier(0),
	m_iCommonFoeTourismModifier(0),
	m_iLessHappyTourismModifier(0),
	m_iSharedIdeologyTourismModifier(0),
	m_iLandTradeRouteGoldChange(0),
	m_iSeaTradeRouteGoldChange(0),
	m_iSharedIdeologyTradeGoldChange(0),
	m_iRiggingElectionModifier(0),
	m_iMilitaryUnitGiftExtraInfluence(0),
	m_iProtectedMinorPerTurnInfluence(0),
	m_iAfraidMinorPerTurnInfluence(0),
	m_iMinorBullyScoreModifier(0),
	m_iThemingBonusMultiplier(0),
	m_iInternalTradeRouteYieldModifier(0),
#if defined(MOD_BALANCE_CORE)
	m_iInternalTradeRouteYieldModifierCapital(0),
	m_iTradeRouteYieldModifierCapital(0),
	m_iTradeRouteYieldModifier(0),
	m_iPositiveWarScoreTourismMod(0),
#endif
	m_bNoCSDecayAtWar (false),
	m_bBullyFriendlyCS(false),
	m_iBullyGlobalCSReduction(0),
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	m_bVassalsNoRebel(false),
	m_iVassalCSBonusModifier(0),
#endif
	m_iSharedReligionTourismModifier(0),
	m_iTradeRouteTourismModifier(0),
	m_iOpenBordersTourismModifier(0),
	m_iCityStateTradeChange(0),
	m_bMinorGreatPeopleAllies(false),
	m_bMinorScienceAllies(false),
	m_bMinorResourceBonus(false),
	m_bGoldenAgeCultureBonusDisabled(false),
	m_bSecondReligionPantheon(false),
	m_bAddReformationBelief(false),
	m_bEnablesSSPartHurry(false),
	m_bEnablesSSPartPurchase(false),
	m_iPolicyBranchType(NO_POLICY_BRANCH_TYPE),
	m_iNumExtraBranches(0),
	m_iHappinessToCulture(0),
	m_iHappinessToScience(0),
	m_iNumCitiesFreeCultureBuilding(0),
	m_iNumCitiesFreeFoodBuilding(0),
	m_bHalfSpecialistUnhappiness(false),
	m_bHalfSpecialistFood(false),
#if defined(MOD_BALANCE_CORE)
	m_bHalfSpecialistFoodCapital(false),
	m_iStealGWSlowerModifier(0),
	m_iStealGWFasterModifier(0),
	m_iEventTourism(0),
	m_iEventTourismCS(0),
	m_iMonopolyModFlat(0),
	m_iMonopolyModPercent(0),
	m_bDummy(false),
	m_bOpener(false),
	m_bFinisher(false),
	m_iCityStateCombatModifier(0),
	m_iGreatEngineerRateModifier(0),
	m_iDefenseBoost(0),
	m_eNewCityFreeBuilding(NO_BUILDINGCLASS),
	m_eAllCityFreeBuilding(NO_BUILDINGCLASS),
	m_eNewFoundCityFreeUnit(NO_UNITCLASS),
	m_eNewFoundCityFreeBuilding(NO_BUILDINGCLASS),
#endif
	m_bMilitaryFoodProduction(false),
	m_iWoundedUnitDamageMod(0),
	m_iUnitUpgradeCostMod(0),
	m_iBarbarianCombatBonus(0),
	m_bAlwaysSeeBarbCamps(false),
	m_bRevealAllCapitals(false),
	m_bGarrisonFreeMaintenance(false),
	m_bAbleToAnnexCityStates(false),
	m_bOneShot(false),
	m_bIsOnlyTradeSameIdeology(false),
	m_bIncludesOneShotFreeUnits(false),
#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
	m_iPovertyHappinessChangePolicy(0),
	m_iDefenseHappinessChangePolicy(0),
	m_iUnculturedHappinessChangePolicy(0),
	m_iIlliteracyHappinessChangePolicy(0),
	m_iMinorityHappinessChangePolicy(0),
	m_iPovertyHappinessChangePolicyCapital(0),
	m_iDefenseHappinessChangePolicyCapital(0),
	m_iUnculturedHappinessChangePolicyCapital(0),
	m_iIlliteracyHappinessChangePolicyCapital(0),
	m_iMinorityHappinessChangePolicyCapital(0),
	m_iPuppetUnhappinessModPolicy(0),
	m_iNoUnhappfromXSpecialists(0),
	m_iHappfromXSpecialists(0),
	m_iNoUnhappfromXSpecialistsCapital(0),
	m_iSpecialistFoodChange(0),
	m_iWarWearinessModifier(0),
	m_iWarScoreModifier(0),
	m_iGreatGeneralExtraBonus(0),
#endif
	m_piPrereqOrPolicies(NULL),
	m_piPrereqAndPolicies(NULL),
	m_piPolicyDisables(NULL),
	m_piYieldModifier(NULL),
	m_piCityYieldChange(NULL),
	m_piCoastalCityYieldChange(NULL),
	m_piCapitalYieldChange(NULL),
	m_piCapitalYieldPerPopChange(NULL),
	m_piCapitalYieldPerPopChangeEmpire(NULL),
	m_piCapitalYieldModifier(NULL),
	m_piGreatWorkYieldChange(NULL),
	m_piSpecialistExtraYield(NULL),
	m_pabFreePromotion(NULL),
	m_paiUnitCombatProductionModifiers(NULL),
	m_paiUnitCombatFreeExperiences(NULL),
	m_paiBuildingClassCultureChanges(NULL),
	m_paiBuildingClassProductionModifiers(NULL),
	m_paiUnitClassProductionModifiers(NULL),
	m_paiBuildingClassTourismModifiers(NULL),
	m_paiBuildingClassHappiness(NULL),
	m_paiFreeUnitClasses(NULL),
	m_paiTourismOnUnitCreation(NULL),
	m_paiHurryModifier(NULL),
	m_pabSpecialistValid(NULL),
#if defined(MOD_BALANCE_CORE)
	m_paiFreeChosenBuilding(NULL),
#endif
#if defined(MOD_BALANCE_CORE_POLICIES)
	m_piResourcefromCSAlly(NULL),
	m_piYieldFromBirth(NULL),
	m_piYieldFromBirthCapital(NULL),
	m_piYieldFromBirthRetroactive(NULL),
	m_piYieldFromBirthCapitalRetroactive(NULL),
	m_piYieldFromConstruction(NULL),
	m_piYieldFromWonderConstruction(NULL),
	m_piYieldFromTech(NULL),
	m_bNoUnhappinessExpansion(false),
	m_bNoUnhappyIsolation(false),
	m_bDoubleBorderGA(false),
	m_iIncreasedQuestInfluence(0),
	m_iGreatScientistBeakerModifier(0),
	m_iGreatEngineerHurryModifier(0),
	m_iTechCostXCitiesMod(0),
	m_iTourismCostXCitiesMod(0),
	m_iInternalTradeGold(0),
	m_iCultureBombBoost(0),
	m_iPuppetProdMod(0),
	m_iOccupiedProdMod(0),
	m_iFreeWCVotes(0),
	m_iInfluenceGPExpend(0),
	m_iFreeTradeRoute(0),
	m_iFreeSpy(0),
	m_iReligionDistance(0),
	m_iPressureMod(0),
	m_piYieldFromBorderGrowth(NULL),
	m_piYieldGPExpend(NULL),
	m_iGarrisonsOccupiedUnhapppinessMod(0),
	m_iTradeReligionModifier(0),
	m_iXPopulationConscription(0),
	m_iBestNumberLandCombatUnitClass(0),
	m_iBestNumberLandRangedUnitClass(0),
	m_iBestNumberSeaCombatUnitClass(0),
	m_iBestNumberSeaRangedUnitClass(0),
	m_iFreePopulation(0),
	m_iFreePopulationCapital(0),
	m_iExtraMoves(0),
	m_iMaxCorporations(0),
	m_iRazingSpeedBonus(0),
	m_iExtraSupplyPerPopulation(0),
	m_bNoPartisans(false),
	m_piConquerorYield(NULL),
	m_piFounderYield(NULL),
	m_piReligionYieldMod(NULL),
	m_ppiReligionBuildingYieldMod(NULL),
#endif
#if defined(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
	m_iInvestmentModifier(0),
#endif
	m_ppiImprovementYieldChanges(NULL),
#if defined(MOD_API_UNIFIED_YIELDS) && defined(MOD_API_PLOT_YIELDS)
	m_ppiPlotYieldChanges(NULL),
#endif
#if defined(MOD_API_UNIFIED_YIELDS)
	m_ppiFeatureYieldChanges(NULL),
	m_ppiCityYieldFromUnimprovedFeature(NULL),
	m_ppiUnimprovedFeatureYieldChanges(NULL),
	m_ppiResourceYieldChanges(NULL),
	m_ppiTerrainYieldChanges(NULL),
	m_ppiTradeRouteYieldChange(NULL),
	m_ppiSpecialistYieldChanges(NULL),
	m_ppiGreatPersonExpendedYield(NULL),
	m_piGoldenAgeGreatPersonRateModifier(NULL),
	m_piYieldFromKills(NULL),
	m_piYieldFromBarbarianKills(NULL),
	m_piYieldChangeTradeRoute(NULL),
	m_piYieldChangesNaturalWonder(NULL),
	m_piYieldChangeWorldWonder(NULL),
	m_piYieldFromMinorDemand(NULL),
	m_piYieldFromWLTKD(NULL),
	m_piArtifactYieldChanges(NULL),
	m_piArtYieldChanges(NULL),
	m_piLitYieldChanges(NULL),
	m_piMusicYieldChanges(NULL),
	m_piYieldFromNonSpecialistCitizens(NULL),
	m_piYieldModifierFromGreatWorks(NULL),
	m_piYieldModifierFromActiveSpies(NULL),
	m_piYieldFromDelegateCount(NULL),
	m_piYieldChangesPerReligion(NULL),
	m_iMissionInfluenceModifier(0),
	m_iHappinessPerActiveTradeRoute(0),
	m_bCSResourcesForMonopolies(false),
	m_iNeedsModifierFromAirUnits(0),
	m_iFlatDefenseFromAirUnits(0),
	m_iPuppetYieldPenaltyMod(0),
	m_iConquestPerEraBuildingProductionMod(0),
	m_iAdmiralLuxuryBonus(0),
#endif
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
	m_piInternationalRouteYieldModifiers(NULL),
#endif
	m_ppiBuildingClassYieldModifiers(NULL),
	m_ppiBuildingClassYieldChanges(NULL),
	m_piFlavorValue(NULL),
	m_eFreeBuildingOnConquest(NO_BUILDING)
{
}

/// Destructor
CvPolicyEntry::~CvPolicyEntry(void)
{
	SAFE_DELETE_ARRAY(m_piPrereqOrPolicies);
	SAFE_DELETE_ARRAY(m_piPrereqAndPolicies);
	SAFE_DELETE_ARRAY(m_piPolicyDisables);
	SAFE_DELETE_ARRAY(m_piYieldModifier);
	SAFE_DELETE_ARRAY(m_piCityYieldChange);
	SAFE_DELETE_ARRAY(m_piCoastalCityYieldChange);
	SAFE_DELETE_ARRAY(m_piCapitalYieldChange);
	SAFE_DELETE_ARRAY(m_piCapitalYieldPerPopChange);
	SAFE_DELETE_ARRAY(m_piCapitalYieldPerPopChangeEmpire);
	SAFE_DELETE_ARRAY(m_piCapitalYieldModifier);
	SAFE_DELETE_ARRAY(m_piGreatWorkYieldChange);
	SAFE_DELETE_ARRAY(m_piSpecialistExtraYield);
	SAFE_DELETE_ARRAY(m_pabFreePromotion);
	SAFE_DELETE_ARRAY(m_paiUnitCombatProductionModifiers);
	SAFE_DELETE_ARRAY(m_paiUnitCombatFreeExperiences);
	SAFE_DELETE_ARRAY(m_paiBuildingClassCultureChanges);
	SAFE_DELETE_ARRAY(m_paiBuildingClassProductionModifiers);
	SAFE_DELETE_ARRAY(m_paiUnitClassProductionModifiers);
	SAFE_DELETE_ARRAY(m_paiBuildingClassTourismModifiers);
	SAFE_DELETE_ARRAY(m_paiBuildingClassHappiness);
	SAFE_DELETE_ARRAY(m_paiFreeUnitClasses);
	SAFE_DELETE_ARRAY(m_paiTourismOnUnitCreation);

//	SAFE_DELETE_ARRAY(m_pabHurry);
	SAFE_DELETE_ARRAY(m_paiHurryModifier);
	SAFE_DELETE_ARRAY(m_pabSpecialistValid);
#if defined(MOD_BALANCE_CORE)
	SAFE_DELETE_ARRAY(m_paiFreeChosenBuilding);
#endif
#if defined(MOD_BALANCE_CORE_POLICIES)
	SAFE_DELETE_ARRAY(m_piResourcefromCSAlly);
	SAFE_DELETE_ARRAY(m_piYieldFromBirth);
	SAFE_DELETE_ARRAY(m_piYieldFromBirthRetroactive);
	SAFE_DELETE_ARRAY(m_piYieldFromBirthCapital);
	SAFE_DELETE_ARRAY(m_piYieldFromBirthCapitalRetroactive);
	SAFE_DELETE_ARRAY(m_piYieldFromConstruction);
	SAFE_DELETE_ARRAY(m_piYieldFromWonderConstruction);
	SAFE_DELETE_ARRAY(m_piYieldFromTech);
	SAFE_DELETE_ARRAY(m_piYieldFromBorderGrowth);
	SAFE_DELETE_ARRAY(m_piYieldGPExpend);
	SAFE_DELETE_ARRAY(m_piConquerorYield);
	SAFE_DELETE_ARRAY(m_piFounderYield);
	SAFE_DELETE_ARRAY(m_piReligionYieldMod);
	SAFE_DELETE_ARRAY(m_piGoldenAgeYieldMod);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiReligionBuildingYieldMod);
#endif
	CvDatabaseUtility::SafeDelete2DArray(m_ppiImprovementYieldChanges);
#if defined(MOD_API_UNIFIED_YIELDS) && defined(MOD_API_PLOT_YIELDS)
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPlotYieldChanges);
#endif
#if defined(MOD_API_UNIFIED_YIELDS)
	CvDatabaseUtility::SafeDelete2DArray(m_ppiFeatureYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiCityYieldFromUnimprovedFeature);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiUnimprovedFeatureYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiResourceYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiTerrainYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiTradeRouteYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiSpecialistYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiGreatPersonExpendedYield);
	SAFE_DELETE_ARRAY(m_piGoldenAgeGreatPersonRateModifier);
	SAFE_DELETE_ARRAY(m_piYieldFromKills);
	SAFE_DELETE_ARRAY(m_piYieldFromBarbarianKills);
	SAFE_DELETE_ARRAY(m_piYieldChangeTradeRoute);
	SAFE_DELETE_ARRAY(m_piYieldChangesNaturalWonder);
	SAFE_DELETE_ARRAY(m_piYieldChangeWorldWonder);
	SAFE_DELETE_ARRAY(m_piYieldFromMinorDemand);
	SAFE_DELETE_ARRAY(m_piYieldFromWLTKD);
	SAFE_DELETE_ARRAY(m_piArtifactYieldChanges);
	SAFE_DELETE_ARRAY(m_piArtYieldChanges);
	SAFE_DELETE_ARRAY(m_piLitYieldChanges);
	SAFE_DELETE_ARRAY(m_piMusicYieldChanges);
	SAFE_DELETE_ARRAY(m_piYieldFromNonSpecialistCitizens);
	SAFE_DELETE_ARRAY(m_piYieldModifierFromGreatWorks);
	SAFE_DELETE_ARRAY(m_piYieldModifierFromActiveSpies);
	SAFE_DELETE_ARRAY(m_piYieldFromDelegateCount);
	SAFE_DELETE_ARRAY(m_piYieldChangesPerReligion);
#endif
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
	SAFE_DELETE_ARRAY(m_piInternationalRouteYieldModifiers);
#endif 
	CvDatabaseUtility::SafeDelete2DArray(m_ppiBuildingClassYieldModifiers);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiBuildingClassYieldChanges);
}

/// Read from XML file (pass 1)
bool CvPolicyEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	//Basic Properties
	m_iCultureCost = kResults.GetInt("CultureCost");
	m_iGridX = kResults.GetInt("GridX");
	m_iGridY = kResults.GetInt("GridY");
	m_iLevel = kResults.GetInt("Level");
	m_iPolicyCostModifier = kResults.GetInt("PolicyCostModifier");
	m_iCulturePerCity = kResults.GetInt("CulturePerCity");
	m_iCulturePerWonder = kResults.GetInt("CulturePerWonder");
	m_iCultureWonderMultiplier = kResults.GetInt("CultureWonderMultiplier");
	m_iCulturePerTechResearched = kResults.GetInt("CulturePerTechResearched");
	m_iCultureImprovementChange = kResults.GetInt("CultureImprovementChange");
	m_iCultureFromKills = kResults.GetInt("CultureFromKills");
	m_iCultureFromBarbarianKills = kResults.GetInt("CultureFromBarbarianKills");
	m_iGoldFromKills = kResults.GetInt("GoldFromKills");
	m_iEmbarkedExtraMoves = kResults.GetInt("EmbarkedExtraMoves");
	m_iAttackBonusTurns = kResults.GetInt("AttackBonusTurns");
	m_iGoldenAgeTurns = kResults.GetInt("GoldenAgeTurns");
	m_iGoldenAgeMeterMod = kResults.GetInt("GoldenAgeMeterMod");
	m_iGoldenAgeDurationMod = kResults.GetInt("GoldenAgeDurationMod");
	m_iNumFreeTechs = kResults.GetInt("NumFreeTechs");
	m_iNumFreePolicies = kResults.GetInt("NumFreePolicies");
	m_iNumFreeGreatPeople = kResults.GetInt("NumFreeGreatPeople");
	m_iMedianTechPercentChange = kResults.GetInt("MedianTechPercentChange");
	m_iStrategicResourceMod = kResults.GetInt("StrategicResourceMod");
	m_iWonderProductionModifier = kResults.GetInt("WonderProductionModifier");
	m_iBuildingProductionModifier = kResults.GetInt("BuildingProductionModifier");
	m_iGreatPeopleRateModifier = kResults.GetInt("GreatPeopleRateModifier");
	m_iGreatGeneralRateModifier = kResults.GetInt("GreatGeneralRateModifier");
	m_iGreatAdmiralRateModifier = kResults.GetInt("GreatAdmiralRateModifier");
	m_iGreatWriterRateModifier = kResults.GetInt("GreatWriterRateModifier");
	m_iGreatArtistRateModifier = kResults.GetInt("GreatArtistRateModifier");
	m_iGreatMusicianRateModifier = kResults.GetInt("GreatMusicianRateModifier");
	m_iGreatMerchantRateModifier = kResults.GetInt("GreatMerchantRateModifier");
#if defined(MOD_DIPLOMACY_CITYSTATES)
	m_iGreatDiplomatRateModifier = kResults.GetInt("GreatDiplomatRateModifier");
#endif
	m_iGreatScientistRateModifier = kResults.GetInt("GreatScientistRateModifier");
	m_iDomesticGreatGeneralRateModifier = kResults.GetInt("DomesticGreatGeneralRateModifier");
	m_iExtraHappiness = kResults.GetInt("ExtraHappiness");
	m_iExtraHappinessPerCity = kResults.GetInt("ExtraHappinessPerCity");
#if defined(HH_MOD_NATURAL_WONDER_MODULARITY)
	m_iExtraNaturalWonderHappiness = kResults.GetInt("ExtraNaturalWonderHappiness");
#endif
	m_iUnhappinessMod = kResults.GetInt("UnhappinessMod");
	m_iCityCountUnhappinessMod = kResults.GetInt("CityCountUnhappinessMod");
	m_iOccupiedPopulationUnhappinessMod = kResults.GetInt("OccupiedPopulationUnhappinessMod");
	m_iCapitalUnhappinessMod = kResults.GetInt("CapitalUnhappinessMod");
	m_iFreeExperience = kResults.GetInt("FreeExperience");
	m_iWorkerSpeedModifier = kResults.GetInt("WorkerSpeedModifier");
	m_iAllFeatureProduction = kResults.GetInt("AllFeatureProduction");
	m_iImprovementCostModifier = kResults.GetInt("ImprovementCostModifier");
	m_iImprovementUpgradeRateModifier = kResults.GetInt("ImprovementUpgradeRateModifier");
	m_iSpecialistProductionModifier = kResults.GetInt("SpecialistProductionModifier");
	m_iSpecialistUpgradeModifier = kResults.GetInt("SpecialistUpgradeModifier");
	m_iMilitaryProductionModifier = kResults.GetInt("MilitaryProductionModifier");
	m_iBaseFreeUnits = kResults.GetInt("BaseFreeUnits");
	m_iBaseFreeMilitaryUnits = kResults.GetInt("BaseFreeMilitaryUnits");
	m_iFreeUnitsPopulationPercent = kResults.GetInt("FreeUnitsPopulationPercent");
	m_iFreeMilitaryUnitsPopulationPercent = kResults.GetInt("FreeMilitaryUnitsPopulationPercent");
	m_iHappinessPerGarrisonedUnit = kResults.GetInt("HappinessPerGarrisonedUnit");
	m_iCulturePerGarrisonedUnit = kResults.GetInt("CulturePerGarrisonedUnit");
	m_iHappinessPerTradeRoute = kResults.GetInt("HappinessPerTradeRoute");
	m_iHappinessPerXPopulation = kResults.GetInt("HappinessPerXPopulation");
#if defined(MOD_BALANCE_CORE_POLICIES)
	m_iHappinessPerXPopulationGlobal = kResults.GetInt("HappinessPerXPopulationGlobal");
	const char* szUnlockPolicyEra = kResults.GetText("UnlocksPolicyBranchEra");
	if(szUnlockPolicyEra)
	{
		m_ePolicyEraUnlock = (EraTypes)GC.getInfoTypeForString(szUnlockPolicyEra, true);
	}
	m_iIdeologyPoint = kResults.GetInt("IdeologyPoint");
	m_bNoXPLossUnitPurchase = kResults.GetBool("NoXPLossUnitPurchase");
	m_bNoForeignCorpsInCities = kResults.GetBool("NoForeignCorpsInCities");
	m_bNoFranchisesInForeignCities = kResults.GetBool("NoFranchisesInForeignCities");
	m_iCorporationOfficesAsFranchises = kResults.GetInt("CorporationOfficesAsNumFranchises");
	m_iCorporationFreeFranchiseAbovePopular = kResults.GetInt("CorporationNumFreeFranchiseAbovePopular");
	m_iCorporationRandomForeignFranchise = kResults.GetInt("CorporationRandomForeignFranchiseMod");
	m_iAdditionalNumFranchisesMod = kResults.GetInt("AdditionalNumFranchisesMod");
	m_iAdditionalNumFranchises = kResults.GetInt("AdditionalNumFranchises");
	m_bUpgradeCSVassalTerritory = kResults.GetBool("UpgradeCSVassalTerritory");
	m_iArchaeologicalDigTourism = kResults.GetInt("ArchaeologicalDigTourism");
	m_iGoldenAgeTourism = kResults.GetInt("GoldenAgeTourism");
	m_iExtraCultureandScienceTradeRoutes = kResults.GetInt("ExtraCultureandScienceTradeRoutes");
	m_iTradeRouteLandDistanceModifier = kResults.GetInt("TradeRouteLandDistanceModifier");
	m_iTradeRouteSeaDistanceModifier = kResults.GetInt("TradeRouteSeaDistanceModifier");
	m_iEspionageModifier = kResults.GetInt("EspionageModifier");
	m_iXCSAlliesLowersPolicyNeedWonders = kResults.GetInt("XCSAlliesLowersPolicyNeedWonders");
	m_iTRVisionBoost = kResults.GetInt("TRVisionBoost");
	m_iTRSpeedBoost = kResults.GetInt("TRSpeedBoost");
	m_iHappinessPerXPolicies = kResults.GetInt("HappinessPerXPolicies");
	m_iHappinessPerXGreatWorks = kResults.GetInt("HappinessPerXGreatWorks");
	m_iExtraMissionaryStrength = kResults.GetInt("ExtraMissionaryStrength");
	m_iExtraMissionarySpreads = kResults.GetInt("ExtraMissionarySpreads");
#endif
	m_iExtraHappinessPerLuxury = kResults.GetInt("ExtraHappinessPerLuxury");
	m_iUnhappinessFromUnitsMod = kResults.GetInt("UnhappinessFromUnitsMod");
	m_iNumExtraBuilders = kResults.GetInt("NumExtraBuilders");
	m_iPlotGoldCostMod = kResults.GetInt("PlotGoldCostMod");
#if defined(MOD_POLICIES_CITY_WORKING)
	m_iCityWorkingChange = kResults.GetInt("CityWorkingChange");
#endif
#if defined(MOD_POLICIES_CITY_AUTOMATON_WORKERS)
	m_iCityAutomatonWorkersChange = kResults.GetInt("CityAutomatonWorkersChange");
#endif
	m_iPlotCultureCostModifier = kResults.GetInt("PlotCultureCostModifier");
	m_iPlotCultureExponentModifier = kResults.GetInt("PlotCultureExponentModifier");
	m_iNumCitiesPolicyCostDiscount = kResults.GetInt("NumCitiesPolicyCostDiscount");
	m_iGarrisonedCityRangeStrikeModifier = kResults.GetInt("GarrisonedCityRangeStrikeModifier");
	m_iUnitPurchaseCostModifier = kResults.GetInt("UnitPurchaseCostModifier");
	m_iBuildingPurchaseCostModifier = kResults.GetInt("BuildingPurchaseCostModifier");
	m_iCityConnectionTradeRouteGoldModifier = kResults.GetInt("CityConnectionTradeRouteGoldModifier");
	m_iTradeMissionGoldModifier = kResults.GetInt("TradeMissionGoldModifier");
	m_iFaithCostModifier = kResults.GetInt("FaithCostModifier");
	m_iCulturalPlunderMultiplier = kResults.GetInt("CulturalPlunderMultiplier");
	m_iStealTechSlowerModifier = kResults.GetInt("StealTechSlowerModifier");
	m_iStealTechFasterModifier = kResults.GetInt("StealTechFasterModifier");
	m_iCatchSpiesModifier = kResults.GetInt("CatchSpiesModifier");
#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
	m_iConversionModifier = kResults.GetInt("ConversionModifier");
#endif
	m_iGoldPerUnit = kResults.GetInt("GoldPerUnit");
	m_iGoldPerMilitaryUnit = kResults.GetInt("GoldPerMilitaryUnit");
	m_iCityStrengthMod = kResults.GetInt("CityStrengthMod");
	m_iCityGrowthMod = kResults.GetInt("CityGrowthMod");
	m_iCapitalGrowthMod = kResults.GetInt("CapitalGrowthMod");
	m_iSettlerProductionModifier = kResults.GetInt("SettlerProductionModifier");
	m_iCapitalSettlerProductionModifier = kResults.GetInt("CapitalSettlerProductionModifier");
	m_iNewCityExtraPopulation = kResults.GetInt("NewCityExtraPopulation");
	m_iFreeFoodBox = kResults.GetInt("FreeFoodBox");
	m_iImprovementGoldMaintenanceMod = kResults.GetInt("RouteGoldMaintenanceMod");
#if defined(MOD_CIV6_WORKER)
	m_iRouteBuilderCostMod = kResults.GetInt("RouteBuilderCostMod");
#endif
	m_iBuildingGoldMaintenanceMod = kResults.GetInt("BuildingGoldMaintenanceMod");
	m_iUnitGoldMaintenanceMod = kResults.GetInt("UnitGoldMaintenanceMod");
	m_iUnitSupplyMod = kResults.GetInt("UnitSupplyMod");
	m_iHappyPerMilitaryUnit = kResults.GetInt("HappyPerMilitaryUnit");
	m_iHappinessToCulture = kResults.GetInt("HappinessToCulture");
	m_iHappinessToScience = kResults.GetInt("HappinessToScience");
	m_iNumCitiesFreeCultureBuilding = kResults.GetInt("NumCitiesFreeCultureBuilding");
	m_iNumCitiesFreeFoodBuilding = kResults.GetInt("NumCitiesFreeFoodBuilding");
	m_bHalfSpecialistUnhappiness = kResults.GetBool("HalfSpecialistUnhappiness");
	m_bHalfSpecialistFood = kResults.GetBool("HalfSpecialistFood");
#if defined(MOD_BALANCE_CORE)
	m_iStealGWSlowerModifier = kResults.GetInt("StealGWSlowerModifier");
	m_iStealGWFasterModifier = kResults.GetInt("StealGWFasterModifier");
	m_bHalfSpecialistFoodCapital = kResults.GetBool("HalfSpecialistFoodCapital");
	m_iEventTourism = kResults.GetInt("EventTourism");
	m_iEventTourismCS = kResults.GetInt("EventTourismCS");
	m_iMonopolyModFlat = kResults.GetInt("MonopolyModFlat");
	m_iMonopolyModPercent = kResults.GetInt("MonopolyModPercent");
	m_bDummy = kResults.GetBool("IsDummy");
	m_bOpener = kResults.GetBool("IsOpener");
	m_bFinisher = kResults.GetBool("IsFinisher");
	m_iCityStateCombatModifier = kResults.GetInt("CityStateCombatModifier");
	m_iGreatEngineerRateModifier = kResults.GetInt("GreatEngineerRateModifier");
	m_iDefenseBoost = kResults.GetInt("DefenseBoostAllCities");
#endif
	m_bMilitaryFoodProduction = kResults.GetBool("MilitaryFoodProduction");
	m_iMaxConscript = kResults.GetInt("MaxConscript");
	m_iWoundedUnitDamageMod = kResults.GetInt("WoundedUnitDamageMod");
	m_iUnitUpgradeCostMod = kResults.GetInt("UnitUpgradeCostMod");
	m_iBarbarianCombatBonus = kResults.GetInt("BarbarianCombatBonus");
	m_bAlwaysSeeBarbCamps = kResults.GetBool("AlwaysSeeBarbCamps");
	m_bRevealAllCapitals = kResults.GetBool("RevealAllCapitals");
	m_bGarrisonFreeMaintenance = kResults.GetBool("GarrisonFreeMaintenance");
	m_iFreeSpecialist = kResults.GetInt("FreeSpecialist");
	m_iExpModifier = kResults.GetInt("ExpModifier");
	m_iExpInBorderModifier = kResults.GetInt("ExpInBorderModifier");
	m_iMinorQuestFriendshipMod = kResults.GetInt("MinorQuestFriendshipMod");
	m_iMinorGoldFriendshipMod = kResults.GetInt("MinorGoldFriendshipMod");
	m_iMinorFriendshipMinimum = kResults.GetInt("MinorFriendshipMinimum");
	m_iMinorFriendshipDecayMod = kResults.GetInt("MinorFriendshipDecayMod");
	m_iOtherPlayersMinorFriendshipDecayMod = kResults.GetInt("OtherPlayersMinorFriendshipDecayMod");
	m_iCityStateUnitFrequencyModifier = kResults.GetInt("CityStateUnitFrequencyModifier");
	m_iCommonFoeTourismModifier = kResults.GetInt("CommonFoeTourismModifier");
	m_iLessHappyTourismModifier = kResults.GetInt("LessHappyTourismModifier");
	m_iSharedIdeologyTourismModifier = kResults.GetInt("SharedIdeologyTourismModifier");
	m_iLandTradeRouteGoldChange = kResults.GetInt("LandTradeRouteGoldChange");
	m_iSeaTradeRouteGoldChange = kResults.GetInt("SeaTradeRouteGoldChange");
	m_iSharedIdeologyTradeGoldChange = kResults.GetInt("SharedIdeologyTradeGoldChange");

	m_iRiggingElectionModifier = kResults.GetInt("RiggingElectionModifier");
	m_iMilitaryUnitGiftExtraInfluence = kResults.GetInt("MilitaryUnitGiftExtraInfluence");
	m_iProtectedMinorPerTurnInfluence = kResults.GetInt("ProtectedMinorPerTurnInfluence");
	m_iAfraidMinorPerTurnInfluence = kResults.GetInt("AfraidMinorPerTurnInfluence");
	m_iMinorBullyScoreModifier = kResults.GetInt("MinorBullyScoreModifier");
	m_iThemingBonusMultiplier = kResults.GetInt("ThemingBonusMultiplier");
	m_iInternalTradeRouteYieldModifier = kResults.GetInt("InternalTradeRouteYieldModifier");
#if defined(MOD_BALANCE_CORE)
	m_iPositiveWarScoreTourismMod = kResults.GetInt("PositiveWarScoreTourismMod");
	m_iInternalTradeRouteYieldModifierCapital = kResults.GetInt("InternalTradeRouteYieldModifierCapital");
	m_iTradeRouteYieldModifierCapital = kResults.GetInt("TradeRouteYieldModifierCapital");
	m_iTradeRouteYieldModifier = kResults.GetInt("TradeRouteYieldModifier");
#endif
	m_bNoCSDecayAtWar = kResults.GetBool("NoAlliedCSInfluenceDecayAtWar");
	m_bBullyFriendlyCS = kResults.GetBool("CanBullyFriendlyCS");
	m_iBullyGlobalCSReduction = kResults.GetInt("BullyGlobalCSInfluenceShift");
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	m_bVassalsNoRebel = kResults.GetBool("VassalsNoRebel");
	m_iVassalCSBonusModifier = kResults.GetInt("VassalCSBonusModifier");
#endif
	m_iSharedReligionTourismModifier = kResults.GetInt("SharedReligionTourismModifier");
	m_iTradeRouteTourismModifier = kResults.GetInt("TradeRouteTourismModifier");
	m_iOpenBordersTourismModifier = kResults.GetInt("OpenBordersTourismModifier");
	m_iCityStateTradeChange = kResults.GetInt("CityStateTradeChange");
	m_bMinorGreatPeopleAllies = kResults.GetBool("MinorGreatPeopleAllies");
	m_bMinorScienceAllies = kResults.GetBool("MinorScienceAllies");
	m_bMinorResourceBonus = kResults.GetBool("MinorResourceBonus");
	m_bGoldenAgeCultureBonusDisabled = kResults.GetBool("GoldenAgeCultureBonusDisabled");
	m_bSecondReligionPantheon = kResults.GetBool("SecondReligionPantheon");
	m_bAddReformationBelief = kResults.GetBool("AddReformationBelief");
	m_bEnablesSSPartHurry = kResults.GetBool("EnablesSSPartHurry");
	m_bEnablesSSPartPurchase = kResults.GetBool("EnablesSSPartPurchase");
	m_bAbleToAnnexCityStates = kResults.GetBool("AbleToAnnexCityStates");
	m_bOneShot = kResults.GetBool("OneShot");
	m_bIsOnlyTradeSameIdeology = kResults.GetBool("IsOnlyTradeSameIdeology");
	m_bIncludesOneShotFreeUnits = kResults.GetBool("IncludesOneShotFreeUnits");
#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
	m_iPovertyHappinessChangePolicy = kResults.GetInt("PovertyHappinessMod");
	m_iDefenseHappinessChangePolicy = kResults.GetInt("DefenseHappinessMod");
	m_iUnculturedHappinessChangePolicy = kResults.GetInt("UnculturedHappinessMod");
	m_iIlliteracyHappinessChangePolicy = kResults.GetInt("IlliteracyHappinessMod");
	m_iMinorityHappinessChangePolicy = kResults.GetInt("MinorityHappinessMod");
	m_iPovertyHappinessChangePolicyCapital = kResults.GetInt("PovertyHappinessModCapital");
	m_iDefenseHappinessChangePolicyCapital = kResults.GetInt("DefenseHappinessModCapital");
	m_iUnculturedHappinessChangePolicyCapital = kResults.GetInt("UnculturedHappinessModCapital");
	m_iIlliteracyHappinessChangePolicyCapital = kResults.GetInt("IlliteracyHappinessModCapital");
	m_iMinorityHappinessChangePolicyCapital = kResults.GetInt("MinorityHappinessModCapital");
	m_iPuppetUnhappinessModPolicy = kResults.GetInt("PuppetUnhappinessModPolicy");
	m_iNoUnhappfromXSpecialists = kResults.GetInt("NoUnhappfromXSpecialists");
	m_iHappfromXSpecialists = kResults.GetInt("HappfromXSpecialists");
	m_iNoUnhappfromXSpecialistsCapital = kResults.GetInt("NoUnhappfromXSpecialistsCapital");
	m_iSpecialistFoodChange = kResults.GetInt("SpecialistFoodChange");
	m_iWarWearinessModifier = kResults.GetInt("WarWearinessModifier");
	m_iWarScoreModifier = kResults.GetInt("WarScoreModifier");
	m_iGreatGeneralExtraBonus = kResults.GetInt("GreatGeneralExtraBonus");
#endif
#if defined(MOD_BALANCE_CORE_POLICIES)
	m_iGarrisonsOccupiedUnhapppinessMod = kResults.GetInt("GarrisonsOccupiedUnhapppinessMod");
	m_iTradeReligionModifier = kResults.GetInt("TradeReligionModifier");
	m_iXPopulationConscription = kResults.GetInt("XPopulationConscription");
	m_iBestNumberLandCombatUnitClass = kResults.GetInt("BestNumberLandCombatUnitClass");
	m_iBestNumberLandRangedUnitClass = kResults.GetInt("BestNumberLandRangedUnitClass");
	m_iBestNumberSeaCombatUnitClass = kResults.GetInt("BestNumberSeaCombatUnitClass");
	m_iBestNumberSeaRangedUnitClass = kResults.GetInt("BestNumberSeaRangedUnitClass");
	m_iFreePopulation = kResults.GetInt("FreePopulation");
	m_iFreePopulationCapital = kResults.GetInt("FreePopulationCapital");
	m_iExtraMoves = kResults.GetInt("ExtraMoves");
	m_iMaxCorporations = kResults.GetInt("MaxCorporations");
	m_iRazingSpeedBonus = kResults.GetInt("RazingSpeedBonus");
	m_iExtraSupplyPerPopulation = kResults.GetInt("ExtraSupplyPerPopulation");
	m_bNoPartisans = kResults.GetBool("NoPartisans");
	m_bNoUnhappinessExpansion = kResults.GetBool("NoUnhappinessExpansion");
	m_bNoUnhappyIsolation = kResults.GetBool("NoUnhappyIsolation");
	m_bDoubleBorderGA = kResults.GetBool("DoubleBorderGA");
	m_iIncreasedQuestInfluence = kResults.GetInt("IncreasedQuestRewards");
	m_iGreatScientistBeakerModifier = kResults.GetInt("GreatScientistBeakerModifier");
	m_iGreatEngineerHurryModifier = kResults.GetInt("GreatEngineerHurryModifier");
	m_iTechCostXCitiesMod = kResults.GetInt("TechCostXCitiesMod");
	m_iTourismCostXCitiesMod = kResults.GetInt("TourismCostXCitiesMod");
	m_iInternalTradeGold = kResults.GetInt("InternalTradeGold");
	m_iCultureBombBoost = kResults.GetInt("CultureBombBoost");
	m_iPuppetProdMod = kResults.GetInt("PuppetProdMod");
	m_iOccupiedProdMod = kResults.GetInt("OccupiedProdMod");
	m_iFreeWCVotes = kResults.GetInt("FreeWCVotes");
	m_iInfluenceGPExpend = kResults.GetInt("InfluenceGPExpend");
	m_iFreeTradeRoute = kResults.GetInt("FreeTradeRoute");
	m_iFreeSpy = kResults.GetInt("FreeSpy");
	m_iReligionDistance = kResults.GetInt("ReligionDistance");
	m_iPressureMod = kResults.GetInt("PressureMod");
	m_iMissionInfluenceModifier = kResults.GetInt("MissionInfluenceModifier");
	m_iHappinessPerActiveTradeRoute = kResults.GetInt("HappinessPerActiveTradeRoute");
	m_bCSResourcesForMonopolies = kResults.GetBool("CSResourcesCountForMonopolies");
	m_iNeedsModifierFromAirUnits = kResults.GetInt("NeedsModifierFromAirUnits");
	m_iFlatDefenseFromAirUnits = kResults.GetInt("FlatDefenseFromAirUnits");
	m_iPuppetYieldPenaltyMod = kResults.GetInt("PuppetYieldPenaltyMod");
	m_iConquestPerEraBuildingProductionMod = kResults.GetInt("ConquestPerEraBuildingProductionMod");
	m_iAdmiralLuxuryBonus = kResults.GetInt("AdmiralLuxuryBonus");
#endif
#if defined(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
	m_iInvestmentModifier = kResults.GetInt("InvestmentModifier");
#endif
	m_strWeLoveTheKingKey = kResults.GetText("WeLoveTheKing");
	m_wstrWeLoveTheKing = GetLocalizedText(m_strWeLoveTheKingKey);

	//References
	const char* szTechPrereq = kResults.GetText("TechPrereq");
	m_iTechPrereq = GC.getInfoTypeForString(szTechPrereq, true);

	const char* szPolicyBranchType = kResults.GetText("PolicyBranchType");
	m_iPolicyBranchType = GC.getInfoTypeForString(szPolicyBranchType, true);

	m_iNumExtraBranches = kResults.GetInt("NumExtraBranches");

	const char* szFreeBuilding = kResults.GetText("FreeBuildingOnConquest");
	if(szFreeBuilding)
	{
		m_eFreeBuildingOnConquest = (BuildingTypes)GC.getInfoTypeForString(szFreeBuilding, true);
	}
#if defined(MOD_BALANCE_CORE)
	const char* szNewCityFreeBuilding = kResults.GetText("NewCityFreeBuilding");
	if(szNewCityFreeBuilding)
	{
		m_eNewCityFreeBuilding = (BuildingClassTypes)GC.getInfoTypeForString(szNewCityFreeBuilding, true);
	}
	const char* szAllCityFreeBuilding = kResults.GetText("AllCityFreeBuilding");
	if (szAllCityFreeBuilding)
	{
		m_eAllCityFreeBuilding = (BuildingClassTypes)GC.getInfoTypeForString(szAllCityFreeBuilding, true);
	}
	const char* szNewFoundCityFreeUnit = kResults.GetText("NewFoundCityFreeUnit");
	if (szNewFoundCityFreeUnit)
	{
		m_eNewFoundCityFreeUnit = (UnitClassTypes)GC.getInfoTypeForString(szNewFoundCityFreeUnit, true);
	}
	const char* szNewFoundCityFreeBuilding = kResults.GetText("NewFoundCityFreeBuilding");
	if (szNewFoundCityFreeBuilding)
	{
		m_eNewFoundCityFreeBuilding = (BuildingClassTypes)GC.getInfoTypeForString(szNewFoundCityFreeBuilding, true);
	}
#endif
	//Arrays
	const char* szPolicyType = GetType();
	kUtility.SetYields(m_piYieldModifier, "Policy_YieldModifiers", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piCityYieldChange, "Policy_CityYieldChanges", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piCoastalCityYieldChange, "Policy_CoastalCityYieldChanges", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piCapitalYieldChange, "Policy_CapitalYieldChanges", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piCapitalYieldPerPopChange, "Policy_CapitalYieldPerPopChanges", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piCapitalYieldPerPopChangeEmpire, "Policy_CapitalYieldPerPopChangeEmpire", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piCapitalYieldModifier, "Policy_CapitalYieldModifiers", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piGreatWorkYieldChange, "Policy_GreatWorkYieldChanges", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piSpecialistExtraYield, "Policy_SpecialistExtraYields", "PolicyType", szPolicyType);
#if defined(MOD_BALANCE_CORE_POLICIES)
	kUtility.PopulateArrayByValue(m_piResourcefromCSAlly, "Resources", "Policy_ResourcefromCSAlly", "ResourceType", "PolicyType", szPolicyType, "Number");
	kUtility.SetYields(m_piYieldFromBirth, "Policy_YieldFromBirth", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piYieldFromBirthRetroactive, "Policy_YieldFromBirthRetroactive", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piYieldFromBirthCapital, "Policy_YieldFromBirthCapital", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piYieldFromBirthCapitalRetroactive, "Policy_YieldFromBirthCapitalRetroactive", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piYieldFromConstruction, "Policy_YieldFromConstruction", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piYieldFromWonderConstruction, "Policy_YieldFromWonderConstruction", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piYieldFromTech, "Policy_YieldFromTech", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piYieldFromBorderGrowth, "Policy_YieldFromBorderGrowth", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piYieldGPExpend, "Policy_YieldGPExpend", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piConquerorYield, "Policy_ConquerorYield", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piFounderYield, "Policy_FounderYield", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piReligionYieldMod, "Policy_ReligionYieldMod", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piGoldenAgeYieldMod, "Policy_GoldenAgeYieldMod", "PolicyType", szPolicyType);
#endif
	kUtility.SetFlavors(m_piFlavorValue, "Policy_Flavors", "PolicyType", szPolicyType);

	kUtility.PopulateArrayByValue(m_paiHurryModifier, "HurryInfos", "Policy_HurryModifiers", "HurryType", "PolicyType", szPolicyType, "HurryCostModifier");

	kUtility.PopulateArrayByExistence(m_pabSpecialistValid, "Specialists", "Policy_ValidSpecialists", "SpecialistType", "PolicyType", szPolicyType);

	kUtility.PopulateArrayByExistence(m_pabFreePromotion, "UnitPromotions", "Policy_FreePromotions", "PromotionType", "PolicyType", szPolicyType);
	kUtility.PopulateArrayByValue(m_paiUnitCombatFreeExperiences, "UnitCombatInfos", "Policy_UnitCombatFreeExperiences", "UnitCombatType", "PolicyType", szPolicyType, "FreeExperience");
	kUtility.PopulateArrayByValue(m_paiUnitCombatProductionModifiers, "UnitCombatInfos", "Policy_UnitCombatProductionModifiers", "UnitCombatType", "PolicyType", szPolicyType, "ProductionModifier");

#if defined(MOD_BALANCE_CORE)
	kUtility.PopulateArrayByValue(m_paiFreeChosenBuilding, "BuildingClasses", "Policy_FreeBuilding", "BuildingClassType", "PolicyType", szPolicyType, "Count");
#endif

	kUtility.PopulateArrayByValue(m_paiBuildingClassCultureChanges, "BuildingClasses", "Policy_BuildingClassCultureChanges", "BuildingClassType", "PolicyType", szPolicyType, "CultureChange");
	kUtility.PopulateArrayByValue(m_paiBuildingClassProductionModifiers, "BuildingClasses", "Policy_BuildingClassProductionModifiers", "BuildingClassType", "PolicyType", szPolicyType, "ProductionModifier");
	kUtility.PopulateArrayByValue(m_paiUnitClassProductionModifiers, "UnitClasses", "Policy_UnitClassProductionModifiers", "UnitClassType", "PolicyType", szPolicyType, "ProductionModifier");
	kUtility.PopulateArrayByValue(m_paiBuildingClassTourismModifiers, "BuildingClasses", "Policy_BuildingClassTourismModifiers", "BuildingClassType", "PolicyType", szPolicyType, "TourismModifier");
	kUtility.PopulateArrayByValue(m_paiBuildingClassHappiness, "BuildingClasses", "Policy_BuildingClassHappiness", "BuildingClassType", "PolicyType", szPolicyType, "Happiness");

	kUtility.PopulateArrayByValue(m_paiFreeUnitClasses, "UnitClasses", "Policy_FreeUnitClasses", "UnitClassType", "PolicyType", szPolicyType, "Count");
	kUtility.PopulateArrayByValue(m_paiTourismOnUnitCreation, "UnitClasses", "Policy_TourismOnUnitCreation", "UnitClassType", "PolicyType", szPolicyType, "Tourism");

	//BuildingYieldModifiers
	{
		kUtility.Initialize2DArray(m_ppiBuildingClassYieldModifiers, "BuildingClasses", "Yields");

		std::string strKey("Policy_BuildingClassYieldModifiers");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select BuildingClasses.ID as BuildingClassID, Yields.ID as YieldID, YieldMod from Policy_BuildingClassYieldModifiers inner join BuildingClasses on BuildingClasses.Type = BuildingClassType inner join Yields on Yields.Type = YieldType where PolicyType = ?");
		}

		pResults->Bind(1, szPolicyType);

		while(pResults->Step())
		{
			const int BuildingClassID = pResults->GetInt(0);
			const int iYieldID = pResults->GetInt(1);
			const int iYieldMod = pResults->GetInt(2);

			m_ppiBuildingClassYieldModifiers[BuildingClassID][iYieldID] = iYieldMod;
		}
	}

	//BuildingYieldChanges
	{
		kUtility.Initialize2DArray(m_ppiBuildingClassYieldChanges, "BuildingClasses", "Yields");

		std::string strKey("Policy_BuildingClassYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select BuildingClasses.ID as BuildingClassID, Yields.ID as YieldID, YieldChange from Policy_BuildingClassYieldChanges inner join BuildingClasses on BuildingClasses.Type = BuildingClassType inner join Yields on Yields.Type = YieldType where PolicyType = ?");
		}

		pResults->Bind(1, szPolicyType);

		while(pResults->Step())
		{
			const int BuildingClassID = pResults->GetInt(0);
			const int iYieldID = pResults->GetInt(1);
			const int iYieldChange = pResults->GetInt(2);

			m_ppiBuildingClassYieldChanges[BuildingClassID][iYieldID] = iYieldChange;
		}
	}

	//ImprovementYieldChanges
	{
		kUtility.Initialize2DArray(m_ppiImprovementYieldChanges, "Improvements", "Yields");

		std::string strKey("Policy_ImprovementYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Improvements.ID as ImprovementID, Yields.ID as YieldID, Yield from Policy_ImprovementYieldChanges inner join Improvements on Improvements.Type = ImprovementType inner join Yields on Yields.Type = YieldType where PolicyType = ?");
		}

		pResults->Bind(1, szPolicyType);

		while(pResults->Step())
		{
			const int ImprovementID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiImprovementYieldChanges[ImprovementID][YieldID] = yield;
		}
	}
#if defined(MOD_BALANCE_CORE_POLICIES)
	//ReligionBuildingYieldMod
	{
		kUtility.Initialize2DArray(m_ppiReligionBuildingYieldMod, "BuildingClasses", "Yields");

		std::string strKey("Policy_ReligionBuildingYieldMod");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select BuildingClasses.ID as BuildingClassID, Yields.ID as YieldID, YieldMod from Policy_ReligionBuildingYieldMod inner join BuildingClasses on BuildingClasses.Type = BuildingClassType inner join Yields on Yields.Type = YieldType where PolicyType = ?");
		}

		pResults->Bind(1, szPolicyType);

		while(pResults->Step())
		{
			const int BuildingClassID = pResults->GetInt(0);
			const int iYieldID = pResults->GetInt(1);
			const int iYieldMod = pResults->GetInt(2);

			m_ppiReligionBuildingYieldMod[BuildingClassID][iYieldID] = iYieldMod;
		}
	}
#endif
#if defined(MOD_API_UNIFIED_YIELDS) && defined(MOD_API_PLOT_YIELDS)
	//PlotYieldChanges
	if (MOD_API_UNIFIED_YIELDS && MOD_API_PLOT_YIELDS)
	{
		kUtility.Initialize2DArray(m_ppiPlotYieldChanges, "Plots", "Yields");

		std::string strKey("Policy_PlotYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Plots.ID as PlotID, Yields.ID as YieldID, Yield from Policy_PlotYieldChanges inner join Plots on Plots.Type = PlotType inner join Yields on Yields.Type = YieldType where PolicyType = ?");
		}

		pResults->Bind(1, szPolicyType);

		while(pResults->Step())
		{
			const int PlotID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiPlotYieldChanges[PlotID][YieldID] = yield;
		}
	}
#endif

#if defined(MOD_API_UNIFIED_YIELDS)
	//FeatureYieldChanges
	if (MOD_API_UNIFIED_YIELDS)
	{
		kUtility.Initialize2DArray(m_ppiFeatureYieldChanges, "Features", "Yields");

		std::string strKey("Policy_FeatureYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Features.ID as FeatureID, Yields.ID as YieldID, Yield from Policy_FeatureYieldChanges inner join Features on Features.Type = FeatureType inner join Yields on Yields.Type = YieldType where PolicyType = ?");
		}

		pResults->Bind(1, szPolicyType);

		while(pResults->Step())
		{
			const int FeatureID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiFeatureYieldChanges[FeatureID][YieldID] = yield;
		}
	}
	
	//CityYieldFromUnimprovedFeature
	if (MOD_API_UNIFIED_YIELDS)
	{
		kUtility.Initialize2DArray(m_ppiCityYieldFromUnimprovedFeature, "Features", "Yields");

		std::string strKey("Policy_CityYieldFromUnimprovedFeature");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Features.ID as FeatureID, Yields.ID as YieldID, Yield from Policy_CityYieldFromUnimprovedFeature inner join Features on Features.Type = FeatureType inner join Yields on Yields.Type = YieldType where PolicyType = ?");
		}

		pResults->Bind(1, szPolicyType);

		while(pResults->Step())
		{
			const int FeatureID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiCityYieldFromUnimprovedFeature[FeatureID][YieldID] = yield;
		}
	}
	
	//UnimprovedFeatureYieldChanges
	if (MOD_API_UNIFIED_YIELDS)
	{
		kUtility.Initialize2DArray(m_ppiUnimprovedFeatureYieldChanges, "Features", "Yields");

		std::string strKey("Policy_UnimprovedFeatureYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Features.ID as FeatureID, Yields.ID as YieldID, Yield from Policy_UnimprovedFeatureYieldChanges inner join Features on Features.Type = FeatureType inner join Yields on Yields.Type = YieldType where PolicyType = ?");
		}

		pResults->Bind(1, szPolicyType);

		while(pResults->Step())
		{
			const int FeatureID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiUnimprovedFeatureYieldChanges[FeatureID][YieldID] = yield;
		}
	}
	
	//ResourceYieldChanges
	if (MOD_API_UNIFIED_YIELDS)
	{
		kUtility.Initialize2DArray(m_ppiResourceYieldChanges, "Resources", "Yields");

		std::string strKey("Policy_ResourceYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Resources.ID as ResourceID, Yields.ID as YieldID, Yield from Policy_ResourceYieldChanges inner join Resources on Resources.Type = ResourceType inner join Yields on Yields.Type = YieldType where PolicyType = ?");
		}

		pResults->Bind(1, szPolicyType);

		while(pResults->Step())
		{
			const int ResourceID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiResourceYieldChanges[ResourceID][YieldID] = yield;
		}
	}
	
	//TerrainYieldChanges
	if (MOD_API_UNIFIED_YIELDS)
	{
		kUtility.Initialize2DArray(m_ppiTerrainYieldChanges, "Terrains", "Yields");

		std::string strKey("Policy_TerrainYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Terrains.ID as TerrainID, Yields.ID as YieldID, Yield from Policy_TerrainYieldChanges inner join Terrains on Terrains.Type = TerrainType inner join Yields on Yields.Type = YieldType where PolicyType = ?");
		}

		pResults->Bind(1, szPolicyType);

		while(pResults->Step())
		{
			const int TerrainID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiTerrainYieldChanges[TerrainID][YieldID] = yield;
		}
	}
	
	//TradeRouteYieldChange
	if (MOD_API_UNIFIED_YIELDS)
	{
		kUtility.Initialize2DArray(m_ppiTradeRouteYieldChange, "Domains", "Yields");

		std::string strKey("Policy_TradeRouteYieldChange");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Domains.ID as DomainID, Yields.ID as YieldID, Yield from Policy_TradeRouteYieldChange inner join Domains on Domains.Type = DomainType inner join Yields on Yields.Type = YieldType where PolicyType = ?");
		}

		pResults->Bind(1, szPolicyType);

		while(pResults->Step())
		{
			const int DomainID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiTradeRouteYieldChange[DomainID][YieldID] = yield;
		}
	}
	
	//SpecialistYieldChanges
	if (MOD_API_UNIFIED_YIELDS)
	{
		kUtility.Initialize2DArray(m_ppiSpecialistYieldChanges, "Specialists", "Yields");

		std::string strKey("Policy_SpecialistYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Specialists.ID as SpecialistID, Yields.ID as YieldID, Yield from Policy_SpecialistYieldChanges inner join Specialists on Specialists.Type = SpecialistType inner join Yields on Yields.Type = YieldType where PolicyType = ?");
		}

		pResults->Bind(1, szPolicyType);

		while(pResults->Step())
		{
			const int SpecialistID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiSpecialistYieldChanges[SpecialistID][YieldID] = yield;
		}
	}
	
	//GreatPersonExpendedYield
	if (MOD_API_UNIFIED_YIELDS)
	{
		kUtility.Initialize2DArray(m_ppiGreatPersonExpendedYield, "GreatPersons", "Yields");

		std::string strKey("Policy_GreatPersonExpendedYield");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select GreatPersons.ID as GreatPersonID, Yields.ID as YieldID, Yield from Policy_GreatPersonExpendedYield inner join GreatPersons on GreatPersons.Type = GreatPersonType inner join Yields on Yields.Type = YieldType where PolicyType = ?");
		}

		pResults->Bind(1, szPolicyType);

		while(pResults->Step())
		{
			const int GreatPersonID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiGreatPersonExpendedYield[GreatPersonID][YieldID] = yield;
		}
	}
	
	kUtility.PopulateArrayByValue(m_piGoldenAgeGreatPersonRateModifier, "GreatPersons", "Policy_GoldenAgeGreatPersonRateModifier", "GreatPersonType", "PolicyType", szPolicyType, "Modifier");
	kUtility.SetYields(m_piYieldFromKills, "Policy_YieldFromKills", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piYieldFromBarbarianKills, "Policy_YieldFromBarbarianKills", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piYieldChangeTradeRoute, "Policy_YieldChangeTradeRoute", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piYieldChangesNaturalWonder, "Policy_YieldChangesNaturalWonder", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piYieldChangeWorldWonder, "Policy_YieldChangeWorldWonder", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piYieldFromMinorDemand, "Policy_YieldFromMinorDemand", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piYieldFromWLTKD, "Policy_WLTKDYieldMod", "PolicyType", szPolicyType);

	kUtility.SetYields(m_piArtifactYieldChanges, "Policy_ArtifactYieldChanges", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piArtYieldChanges, "Policy_ArtYieldChanges", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piLitYieldChanges, "Policy_LitYieldChanges", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piMusicYieldChanges, "Policy_MusicYieldChanges", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piRelicYieldChanges, "Policy_RelicYieldChanges", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piFilmYieldChanges, "Policy_FilmYieldChanges", "PolicyType", szPolicyType);

	kUtility.SetYields(m_piYieldFromNonSpecialistCitizens, "Policy_YieldFromNonSpecialistCitizens", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piYieldModifierFromGreatWorks, "Policy_YieldModifierFromGreatWorks", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piYieldModifierFromActiveSpies, "Policy_YieldModifierFromActiveSpies", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piYieldFromDelegateCount, "Policy_YieldFromDelegateCount", "PolicyType", szPolicyType);

	kUtility.SetYields(m_piYieldChangesPerReligion, "Policy_YieldChangesPerReligion", "PolicyType", szPolicyType);

	
#endif
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
	kUtility.SetYields(m_piInternationalRouteYieldModifiers, "Policy_InternationalRouteYieldModifiers", "PolicyType", szPolicyType);
#endif

	//ImprovementCultureChanges
	kUtility.PopulateArrayByValue(m_piImprovementCultureChange, "Improvements", "Policy_ImprovementCultureChanges", "ImprovementType", "PolicyType", szPolicyType, "CultureChange");

	//OrPreReqs
	{
		kUtility.InitializeArray(m_piPrereqOrPolicies, "Policies", (int)NO_POLICY);

		std::string sqlKey = "m_piPrereqOrPolicies";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select Policies.ID from Policy_PrereqORPolicies inner join Policies on Policies.Type = PrereqPolicy where PolicyType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szPolicyType, false);

		int i = 0;
		while(pResults->Step())
		{
			m_piPrereqOrPolicies[i++] = pResults->GetInt(0);
		}

		pResults->Reset();
	}

	//AndPreReqs
	{
		kUtility.InitializeArray(m_piPrereqAndPolicies, "Policies", (int)NO_POLICY);
		std::string sqlKey = "m_piPrereqAndPolicies";

		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select Policies.ID from Policy_PrereqPolicies inner join Policies on Policies.Type = PrereqPolicy where PolicyType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szPolicyType, false);

		int i = 0;
		while(pResults->Step())
		{
			m_piPrereqAndPolicies[i++] = pResults->GetInt(0);
		}

		pResults->Reset();
	}

	//Policy_Disables
	{
		kUtility.InitializeArray(m_piPolicyDisables, "Policies", (int)NO_POLICY);

		std::string sqlKey = "m_piPolicyDisables";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select Policies.ID from Policy_Disables inner join Policies on Policies.Type = PolicyDisable where PolicyType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szPolicyType, false);

		int i = 0;
		while(pResults->Step())
		{
			m_piPolicyDisables[i++] = pResults->GetInt(0);
		}

		pResults->Reset();
	}

	//UnitCombatFreePromotions
	{
		m_FreePromotionUnitCombats.clear();
		std::string sqlKey = "m_FreePromotionsUnitCombats";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select UnitPromotions.ID, UnitCombatInfos.ID  from Policy_FreePromotionUnitCombats, UnitPromotions, UnitCombatInfos where PolicyType = ? and PromotionType = UnitPromotions.ID and UnitCombatType = UnitCombatInfos.ID";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szPolicyType, false);

		while(pResults->Step())
		{
			const int UnitPromotionID = pResults->GetInt(0);
			const int UnitCombatInfoID = pResults->GetInt(1);

			m_FreePromotionUnitCombats.insert(std::pair<int, int>(UnitPromotionID, UnitCombatInfoID));
		}

		//Trim capacity
		std::multimap<int, int>(m_FreePromotionUnitCombats).swap(m_FreePromotionUnitCombats);

		pResults->Reset();
	}

#if defined(MOD_RELIGION_POLICY_BRANCH_FAITH_GP)
	//FaithPurchaseUnitClasses
	if (MOD_RELIGION_POLICY_BRANCH_FAITH_GP)
	{
		m_FaithPurchaseUnitClasses.clear();

		std::string sqlKey1 = "m_FaithPurchaseUnitClasses1";
		Database::Results* pResults1 = kUtility.GetResults(sqlKey1);
		if(pResults1 == NULL)
		{
			const char* szSQL = "select u.ID, -1 from Policy_FaithPurchaseUnitClasses p, UnitClasses u where p.PolicyType = ? and p.UnitClassType = u.Type and p.EraType is null;";
			pResults1 = kUtility.PrepareResults(sqlKey1, szSQL);
		}

		pResults1->Bind(1, szPolicyType, false);

		while(pResults1->Step())
		{
			const int UnitClassID = pResults1->GetInt(0);
			const int EraID = pResults1->GetInt(1);

			m_FaithPurchaseUnitClasses.insert(std::pair<int, int>(UnitClassID, EraID));
		}

		std::string sqlKey2 = "m_FaithPurchaseUnitClasses2";
		Database::Results* pResults2 = kUtility.GetResults(sqlKey2);
		if(pResults2 == NULL)
		{
			const char* szSQL = "select u.ID, e.ID from Policy_FaithPurchaseUnitClasses p, UnitClasses u, Eras e where p.PolicyType = ? and p.UnitClassType = u.Type and p.EraType = e.Type;";
			pResults2 = kUtility.PrepareResults(sqlKey2, szSQL);
		}

		pResults2->Bind(1, szPolicyType, false);

		while(pResults2->Step())
		{
			const int UnitClassID = pResults2->GetInt(0);
			const int EraID = pResults2->GetInt(1);

			m_FaithPurchaseUnitClasses.insert(std::pair<int, int>(UnitClassID, EraID));
		}

		//Trim capacity
		std::multimap<int, int>(m_FaithPurchaseUnitClasses).swap(m_FaithPurchaseUnitClasses);

		pResults1->Reset();
		pResults2->Reset();
	}
#endif

	return true;
}

/// Cost of next policy
int CvPolicyEntry::GetCultureCost() const
{
	return m_iCultureCost;
}

/// X location on policy page
int CvPolicyEntry::GetGridX() const
{
	return m_iGridX;
}

/// Y location on policy page
int CvPolicyEntry::GetGridY() const
{
	return m_iGridY;
}

/// If this policy's branch unlocks by level, what is the level for this policy?
int CvPolicyEntry::GetLevel() const
{
	return m_iLevel;
}

/// Percentage change in cost of subsequent policy purchases
int CvPolicyEntry::GetPolicyCostModifier() const
{
	return m_iPolicyCostModifier;
}

/// Amount of Culture each City gets for free
int CvPolicyEntry::GetCulturePerCity() const
{
	return m_iCulturePerCity;
}

/// Amount of extra Culture each Wonder gets
int CvPolicyEntry::GetCulturePerWonder() const
{
	return m_iCulturePerWonder;
}

/// Culture multiplier for a city with a wonder
int CvPolicyEntry::GetCultureWonderMultiplier() const
{
	return m_iCultureWonderMultiplier;
}

/// Amount of Culture provided when a Tech is researched
int CvPolicyEntry::GetCulturePerTechResearched() const
{
	return m_iCulturePerTechResearched;
}

/// Extra culture provided from improvements that already provide culture
int CvPolicyEntry::GetCultureImprovementChange() const
{
	return m_iCultureImprovementChange;
}

/// Percentage of killed unit strength that is added as culture
int CvPolicyEntry::GetCultureFromKills() const
{
	return m_iCultureFromKills;
}

/// Percentage of killed barbarian strength that is added as culture
int CvPolicyEntry::GetCultureFromBarbarianKills() const
{
	return m_iCultureFromBarbarianKills;
}

/// Percentage of killed unit strength that is added as gold
int CvPolicyEntry::GetGoldFromKills() const
{
	return m_iGoldFromKills;
}

/// Extra moves for embarked units
int CvPolicyEntry::GetEmbarkedExtraMoves() const
{
	return m_iEmbarkedExtraMoves;
}

/// Number of turns of attack bonus
int CvPolicyEntry::GetAttackBonusTurns() const
{
	return m_iAttackBonusTurns;
}

/// Number of free Golden Age turns
int CvPolicyEntry::GetGoldenAgeTurns() const
{
	return m_iGoldenAgeTurns;
}

/// Modify how big the Golden Age meter is (-50 = 50% of normal)
int CvPolicyEntry::GetGoldenAgeMeterMod() const
{
	return m_iGoldenAgeMeterMod;
}

/// Modify how long Golden Ages last
int CvPolicyEntry::GetGoldenAgeDurationMod() const
{
	return m_iGoldenAgeDurationMod;
}

/// Number of free Techs
int CvPolicyEntry::GetNumFreeTechs() const
{
	return m_iNumFreeTechs;
}

/// Number of free Policies
int CvPolicyEntry::GetNumFreePolicies() const
{
	return m_iNumFreePolicies;
}

/// Number of free Great People
int CvPolicyEntry::GetNumFreeGreatPeople() const
{
	return m_iNumFreeGreatPeople;
}

/// Boost to percentage of median tech awarded for research agreement
int CvPolicyEntry::GetMedianTechPercentChange() const
{
	return m_iMedianTechPercentChange;
}

/// Mod to owned Strategic Resources (200 = 200% of normal output)
int CvPolicyEntry::GetStrategicResourceMod() const
{
	return m_iStrategicResourceMod;
}

/// Production bonus when working on a Wonder
int CvPolicyEntry::GetWonderProductionModifier() const
{
	return m_iWonderProductionModifier;
}

/// Production bonus when working on a Building
int CvPolicyEntry::GetBuildingProductionModifier() const
{
	return m_iBuildingProductionModifier;
}

///  Change in spawn rate for great people
int CvPolicyEntry::GetGreatPeopleRateModifier() const
{
	return m_iGreatPeopleRateModifier;
}

///  Change in spawn rate for great generals
int CvPolicyEntry::GetGreatGeneralRateModifier() const
{
	return m_iGreatGeneralRateModifier;
}

///  Change in spawn rate for great admirals
int CvPolicyEntry::GetGreatAdmiralRateModifier() const
{
	return m_iGreatAdmiralRateModifier;
}

///  Change in spawn rate for great writers
int CvPolicyEntry::GetGreatWriterRateModifier() const
{
	return m_iGreatWriterRateModifier;
}

///  Change in spawn rate for great artists
int CvPolicyEntry::GetGreatArtistRateModifier() const
{
	return m_iGreatArtistRateModifier;
}

///  Change in spawn rate for great musicians
int CvPolicyEntry::GetGreatMusicianRateModifier() const
{
	return m_iGreatMusicianRateModifier;
}

///  Change in spawn rate for great merchants
int CvPolicyEntry::GetGreatMerchantRateModifier() const
{
	return m_iGreatMerchantRateModifier;
}

#if defined(MOD_DIPLOMACY_CITYSTATES)
///  Change in spawn rate for great diplomats
int CvPolicyEntry::GetGreatDiplomatRateModifier() const
{
	return m_iGreatDiplomatRateModifier;
}
#endif

///  Change in spawn rate for great scientists
int CvPolicyEntry::GetGreatScientistRateModifier() const
{
	return m_iGreatScientistRateModifier;
}

///  Change in spawn rate for domestic great generals
int CvPolicyEntry::GetDomesticGreatGeneralRateModifier() const
{
	return m_iDomesticGreatGeneralRateModifier;
}

///  Extra Happiness
int CvPolicyEntry::GetExtraHappiness() const
{
	return m_iExtraHappiness;
}

///  Extra Happiness per city
int CvPolicyEntry::GetExtraHappinessPerCity() const
{
	return m_iExtraHappinessPerCity;
}

#if defined(HH_MOD_NATURAL_WONDER_MODULARITY)
///  Extra happiness per known Natural Wonder
int CvPolicyEntry::GetExtraNaturalWonderHappiness() const
{
	return m_iExtraNaturalWonderHappiness;
}
#endif

///  Unhappiness mod (-50 = 50% of normal Unhappiness)
int CvPolicyEntry::GetUnhappinessMod() const
{
	return m_iUnhappinessMod;
}

///  City Count Unhappiness mod (-50 = 50% of normal Unhappiness)
int CvPolicyEntry::GetCityCountUnhappinessMod() const
{
	return m_iCityCountUnhappinessMod;
}

///  Occupied Population Unhappiness mod (-50 = 50% of normal Unhappiness)
int CvPolicyEntry::GetOccupiedPopulationUnhappinessMod() const
{
	return m_iOccupiedPopulationUnhappinessMod;
}

///  Unhappiness mod for capital (-50 = 50% of normal Unhappiness)
int CvPolicyEntry::GetCapitalUnhappinessMod() const
{
	return m_iCapitalUnhappinessMod;
}

/// Free experience for every new unit recruited
int CvPolicyEntry::GetFreeExperience() const
{
	return m_iFreeExperience;
}

/// Improvement in worker speed
int CvPolicyEntry::GetWorkerSpeedModifier() const
{
	return m_iWorkerSpeedModifier;
}

/// How much Production does removing ALL Features now give us?
int CvPolicyEntry::GetAllFeatureProduction() const
{
	return m_iAllFeatureProduction;
}

/// Reduction in improvement costs
int CvPolicyEntry::GetImprovementCostModifier() const
{
	return m_iImprovementCostModifier;
}

/// Improvement upgrade speed
int CvPolicyEntry::GetImprovementUpgradeRateModifier() const
{
	return m_iImprovementUpgradeRateModifier;
}

/// Specialist production boost
int CvPolicyEntry::GetSpecialistProductionModifier() const
{
	return m_iSpecialistProductionModifier;
}

/// Increase rate of Specialist growth
int CvPolicyEntry::GetSpecialistUpgradeModifier() const
{
	return m_iSpecialistUpgradeModifier;
}

/// Military unit production boost
int CvPolicyEntry::GetMilitaryProductionModifier() const
{
	return m_iMilitaryProductionModifier;
}

/// Number of units with no upkeep cost
int CvPolicyEntry::GetBaseFreeUnits() const
{
	return m_iBaseFreeUnits;
}

/// Number of military units with no upkeep cost
int CvPolicyEntry::GetBaseFreeMilitaryUnits() const
{
	return m_iBaseFreeMilitaryUnits;
}

/// Number of free upkeep units based on population size
int CvPolicyEntry::GetFreeUnitsPopulationPercent() const
{
	return m_iFreeUnitsPopulationPercent;
}

/// Number of free upkeep military units based on population size
int CvPolicyEntry::GetFreeMilitaryUnitsPopulationPercent() const
{
	return m_iFreeMilitaryUnitsPopulationPercent;
}

/// Happiness from each City with a Garrison
int CvPolicyEntry::GetHappinessPerGarrisonedUnit() const
{
	return m_iHappinessPerGarrisonedUnit;
}

/// Culture from each City with a Garrison
int CvPolicyEntry::GetCulturePerGarrisonedUnit() const
{
	return m_iCulturePerGarrisonedUnit;
}

/// Happiness from each City with a Trade Route to the capital
int CvPolicyEntry::GetHappinessPerTradeRoute() const
{
	return m_iHappinessPerTradeRoute;
}

/// Happiness from large cities
int CvPolicyEntry::GetHappinessPerXPopulation() const
{
	return m_iHappinessPerXPopulation;
}
#if defined(MOD_BALANCE_CORE_POLICIES)
/// Happiness from global city pop
int CvPolicyEntry::GetHappinessPerXPopulationGlobal() const
{
	return m_iHappinessPerXPopulationGlobal;
}
/// Era Unlock for Policy Branch
EraTypes CvPolicyEntry::GetPolicyEraUnlock() const
{
	return m_ePolicyEraUnlock;
}
/// Ideology points towards picking an ideology
int CvPolicyEntry::GetIdeologyPoint() const
{
	return m_iIdeologyPoint;
}

bool CvPolicyEntry::IsNoXPLossUnitPurchase() const
{
	return m_bNoXPLossUnitPurchase;
}

/// Does this make Offices count as Franchises?
int CvPolicyEntry::GetCorporationOfficesAsFranchises() const
{
	return m_iCorporationOfficesAsFranchises;
}

bool CvPolicyEntry::IsNoForeignCorpsInCities() const
{
	return m_bNoForeignCorpsInCities;
}
bool CvPolicyEntry::IsNoFranchisesInForeignCities() const
{
	return m_bNoFranchisesInForeignCities;
}

/// Does this grant a modifier to franchise count for each foreign city above Popular?
int CvPolicyEntry::GetCorporationFreeFranchiseAbovePopular() const
{
	return m_iCorporationFreeFranchiseAbovePopular;
}
/// Does this grant a chance for a random foreign franchise each turn?
int CvPolicyEntry::GetCorporationRandomForeignFranchiseMod() const
{
	return m_iCorporationRandomForeignFranchise;
}
/// Does this grant additional franchises that can be established?
int CvPolicyEntry::GetAdditionalNumFranchisesMod() const
{
	return m_iAdditionalNumFranchisesMod;
}
int CvPolicyEntry::GetAdditionalNumFranchises() const
{
	return m_iAdditionalNumFranchises;
}
bool CvPolicyEntry::IsUpgradeCSVassalTerritory() const
{
	return m_bUpgradeCSVassalTerritory;
}
int CvPolicyEntry::GetArchaeologicalDigTourism() const
{
	return m_iArchaeologicalDigTourism;
}
int CvPolicyEntry::GetGoldenAgeTourism() const
{
	return m_iGoldenAgeTourism;
}
int CvPolicyEntry::GetExtraCultureandScienceTradeRoutes() const
{
	return m_iExtraCultureandScienceTradeRoutes;
}
int CvPolicyEntry::GetTradeRouteLandDistanceModifier() const
{
	return m_iTradeRouteLandDistanceModifier;
}
int CvPolicyEntry::GetTradeRouteSeaDistanceModifier() const
{
	return m_iTradeRouteSeaDistanceModifier;
}
int CvPolicyEntry::GetEspionageModifier() const
{
	return m_iEspionageModifier;
}
int CvPolicyEntry::GetXCSAlliesLowersPolicyNeedWonders() const
{
	return m_iXCSAlliesLowersPolicyNeedWonders;
}

int CvPolicyEntry::GetTRVisionBoost() const
{
	return m_iTRVisionBoost;
}
int CvPolicyEntry::GetTRSpeedBoost() const
{
	return m_iTRSpeedBoost;
}
int CvPolicyEntry::GetHappinessPerXPolicies() const
{
	return m_iHappinessPerXPolicies;
}

int CvPolicyEntry::GetHappinessPerXGreatWorks() const
{
	return m_iHappinessPerXGreatWorks;
}

int CvPolicyEntry::GetExtraMissionaryStrength() const
{
	return m_iExtraMissionaryStrength;
}

int CvPolicyEntry::GetExtraMissionarySpreads() const
{
	return m_iExtraMissionarySpreads;
}
#endif
/// Happiness from each connected Luxury Resource
int CvPolicyEntry::GetExtraHappinessPerLuxury() const
{
	return m_iExtraHappinessPerLuxury;
}

/// Unhappiness from Units (Workers) Percent? (50 = 50% of normal)
int CvPolicyEntry::GetUnhappinessFromUnitsMod() const
{
	return m_iUnhappinessFromUnitsMod;
}

/// Number of Builders a Player is allowed to train
int CvPolicyEntry::GetNumExtraBuilders() const
{
	return m_iNumExtraBuilders;
}

/// How much less does Plot buying cost
int CvPolicyEntry::GetPlotGoldCostMod() const
{
	return m_iPlotGoldCostMod;
}

#if defined(MOD_POLICIES_CITY_WORKING)
/// How many more rings can we work
int CvPolicyEntry::GetCityWorkingChange() const
{
	return m_iCityWorkingChange;
}
#endif

#if defined(MOD_POLICIES_CITY_AUTOMATON_WORKERS)
/// How many more tiles can we work
int CvPolicyEntry::GetCityAutomatonWorkersChange() const
{
	return m_iCityAutomatonWorkersChange;
}
#endif

/// How much Culture is needed for a City to acquire a new Plot?
int CvPolicyEntry::GetPlotCultureCostModifier() const
{
	return m_iPlotCultureCostModifier;
}

/// How much do we dampen the exponent used to determine Culture needed for a City to acquire a new Plot?
int CvPolicyEntry::GetPlotCultureExponentModifier() const
{
	return m_iPlotCultureExponentModifier;
}

/// How much do we dampen the growth of policy costs based on number of cities?
int CvPolicyEntry::GetNumCitiesPolicyCostDiscount() const
{
	return m_iNumCitiesPolicyCostDiscount;
}

/// Increase in city range strike due to garrison
int CvPolicyEntry::GetGarrisonedCityRangeStrikeModifier() const
{
	return m_iGarrisonedCityRangeStrikeModifier;
}

/// Cost of purchasing units reduced?
int CvPolicyEntry::GetUnitPurchaseCostModifier() const
{
	return m_iUnitPurchaseCostModifier;
}

/// Cost of purchasing buildings reduced?
int CvPolicyEntry::GetBuildingPurchaseCostModifier() const
{
	return m_iBuildingPurchaseCostModifier;
}

/// How much more Gold do we make from Trade Routes
int CvPolicyEntry::GetCityConnectionTradeRouteGoldModifier() const
{
	return m_iCityConnectionTradeRouteGoldModifier;
}

/// How much more Gold do we make from Trade Missions?
int CvPolicyEntry::GetTradeMissionGoldModifier() const
{
	return m_iTradeMissionGoldModifier;
}

/// How much more of a discount do we get on Faith purchases?
int CvPolicyEntry::GetFaithCostModifier() const
{
	return m_iFaithCostModifier;
}

/// How much culture do we get for each point of culture in the sacked city?
int CvPolicyEntry::GetCulturalPlunderMultiplier() const
{
	return m_iCulturalPlunderMultiplier;
}

/// How much enemy tech stealing is slowed?
int CvPolicyEntry::GetStealTechSlowerModifier() const
{
	return m_iStealTechSlowerModifier;
}

int CvPolicyEntry::GetStealTechFasterModifier() const
{
	return m_iStealTechFasterModifier;
}

/// How much easier is it to catch enemy spies?
int CvPolicyEntry::GetCatchSpiesModifier() const
{
	return m_iCatchSpiesModifier;
}

#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
/// How much unfriendly religion spread is slowed?
int CvPolicyEntry::GetConversionModifier() const
{
	return m_iConversionModifier;
}
#endif

/// Upkeep cost
int CvPolicyEntry::GetGoldPerUnit() const
{
	return m_iGoldPerUnit;
}

/// Military unit upkeep cost
int CvPolicyEntry::GetGoldPerMilitaryUnit() const
{
	return m_iGoldPerMilitaryUnit;
}

/// City strength modifier (e.g. 100 = double strength)
int CvPolicyEntry::GetCityStrengthMod() const
{
	return m_iCityStrengthMod;
}

/// City growth food modifier (e.g. 100 = double growth rate)
int CvPolicyEntry::GetCityGrowthMod() const
{
	return m_iCityGrowthMod;
}

/// Capital growth food modifier (e.g. 50 = +50% growth)
int CvPolicyEntry::GetCapitalGrowthMod() const
{
	return m_iCapitalGrowthMod;
}

/// Settler production modifier (e.g. 50 = +50% production)
int CvPolicyEntry::GetSettlerProductionModifier() const
{
	return m_iSettlerProductionModifier;
}

/// Capital Settler production modifier (e.g. 50 = +50% production)
int CvPolicyEntry::GetCapitalSettlerProductionModifier() const
{
	return m_iCapitalSettlerProductionModifier;
}

/// Amount of extra population newly-founded Cities receive
int CvPolicyEntry::GetNewCityExtraPopulation() const
{
	return m_iNewCityExtraPopulation;
}
#if defined(MOD_BALANCE_CORE)
BuildingClassTypes CvPolicyEntry::GetNewFoundCityFreeBuilding() const
{
	return m_eNewFoundCityFreeBuilding;
}
UnitClassTypes CvPolicyEntry::GetNewFoundCityFreeUnit() const
{
	return m_eNewFoundCityFreeUnit;
}
BuildingClassTypes CvPolicyEntry::GetNewCityFreeBuilding() const
{
	return m_eNewCityFreeBuilding;
}
BuildingClassTypes CvPolicyEntry::GetAllCityFreeBuilding() const
{
	return m_eAllCityFreeBuilding;
}
#endif
/// Amount of free food newly-founded Cities receive
int CvPolicyEntry::GetFreeFoodBox() const
{
	return m_iFreeFoodBox;
}

/// Route upkeep cost Modifier (e.g. 50 = 150% normal cost)
int CvPolicyEntry::GetImprovementGoldMaintenanceMod() const
{
	return m_iImprovementGoldMaintenanceMod;
}

#if defined(MOD_CIV6_WORKER)
/// Route cost Modifier (e.g. 50 = 150% normal cost)
int CvPolicyEntry::GetRouteBuilderCostMod() const
{
	return m_iRouteBuilderCostMod;
}
#endif

/// Building upkeep cost Modifier (e.g. 50 = 150% normal cost)
int CvPolicyEntry::GetBuildingGoldMaintenanceMod() const
{
	return m_iBuildingGoldMaintenanceMod;
}

/// Unit upkeep cost Modifier (e.g. 50 = 150% normal cost)
int CvPolicyEntry::GetUnitGoldMaintenanceMod() const
{
	return m_iUnitGoldMaintenanceMod;
}

/// Military unit supply cost Modifier (e.g. 50 = 150% normal cost)
int CvPolicyEntry::GetUnitSupplyMod() const
{
	return m_iUnitSupplyMod;
}

/// Military unit happiness bonus
int CvPolicyEntry::GetHappyPerMilitaryUnit() const
{
	return m_iHappyPerMilitaryUnit;
}

/// Free specialist in each site
int CvPolicyEntry::GetFreeSpecialist() const
{
	return m_iFreeSpecialist;
}

/// Technology prerequisite
int CvPolicyEntry::GetTechPrereq() const
{
	return m_iTechPrereq;
}

/// Number of units that may be conscripted
int CvPolicyEntry::GetMaxConscript() const
{
	return m_iMaxConscript;
}

/// Modifier to experience
int CvPolicyEntry::GetExpModifier() const
{
	return m_iExpModifier;
}

/// Modifier to experience gained within cultural borders
int CvPolicyEntry::GetExpInBorderModifier() const
{
	return m_iExpInBorderModifier;
}

/// Friendship modifier for completing Minor Civ Quests (50 = 150% of normal friendship amount)
int CvPolicyEntry::GetMinorQuestFriendshipMod() const
{
	return m_iMinorQuestFriendshipMod;
}

/// Friendship modifier for Gold gifts to Minors (50 = 150% of normal friendship amount)
int CvPolicyEntry::GetMinorGoldFriendshipMod() const
{
	return m_iMinorGoldFriendshipMod;
}

/// Minimum Friendship with all Minors
int CvPolicyEntry::GetMinorFriendshipMinimum() const
{
	return m_iMinorFriendshipMinimum;
}

/// Friendship decay mod with Minors (-50 = 50% normal decay)
int CvPolicyEntry::GetMinorFriendshipDecayMod() const
{
	return m_iMinorFriendshipDecayMod;
}

/// OTHER PLAYERS' Friendship decay mod with Minors (50 = 150% normal decay)
int CvPolicyEntry::GetOtherPlayersMinorFriendshipDecayMod() const
{
	return m_iOtherPlayersMinorFriendshipDecayMod;
}

/// Increase in frequency of receiving units from military minors
int CvPolicyEntry::GetCityStateUnitFrequencyModifier() const
{
	return m_iCityStateUnitFrequencyModifier;
}

/// Tourism boost with civs fighting a common foe
int CvPolicyEntry::GetCommonFoeTourismModifier() const
{
	return m_iCommonFoeTourismModifier;
}

/// Tourism boost with civs with less happiness
int CvPolicyEntry::GetLessHappyTourismModifier() const
{
	return m_iLessHappyTourismModifier;
}

/// Tourism boost with civs of the same ideology
int CvPolicyEntry::GetSharedIdeologyTourismModifier() const
{
	return m_iSharedIdeologyTourismModifier;
}

/// Land trade route gold boost
int CvPolicyEntry::GetLandTradeRouteGoldChange() const
{
	return m_iLandTradeRouteGoldChange;
}

/// Sea trade route gold boost
int CvPolicyEntry::GetSeaTradeRouteGoldChange() const
{
	return m_iSeaTradeRouteGoldChange;
}

/// Trade route gold change with civs with whom you share an ideology
int CvPolicyEntry::GetSharedIdeologyTradeGoldChange() const
{
	return m_iSharedIdeologyTradeGoldChange;
}

/// Boost to chance of rigging an election
int CvPolicyEntry::GetRiggingElectionModifier() const
{
	return m_iRiggingElectionModifier;
}

///Influence boost upon gifting a military unit
int CvPolicyEntry::GetMilitaryUnitGiftExtraInfluence() const
{
	return m_iMilitaryUnitGiftExtraInfluence;
}

/// Influence boost with minors you have a trade route with
int CvPolicyEntry::GetProtectedMinorPerTurnInfluence() const
{
	return m_iProtectedMinorPerTurnInfluence;
}

/// Influence boost with minor eligible for bullying
int CvPolicyEntry::GetAfraidMinorPerTurnInfluence() const
{
	return m_iAfraidMinorPerTurnInfluence;
}

/// Score modifier for ability to bully a minor
int CvPolicyEntry::GetMinorBullyScoreModifier() const
{
	return m_iMinorBullyScoreModifier;
}

/// Boost to museum theming
int CvPolicyEntry::GetThemingBonusMultiplier() const
{
	return m_iThemingBonusMultiplier;
}

/// Boost to internal trade routes
int CvPolicyEntry::GetInternalTradeRouteYieldModifier() const
{
	return m_iInternalTradeRouteYieldModifier;
}
#if defined(MOD_BALANCE_CORE)
int CvPolicyEntry::GetTradeRouteYieldModifier() const
{
	return m_iTradeRouteYieldModifier;
}
/// Boost to internal trade routes from Capital
int CvPolicyEntry::GetInternalTradeRouteYieldModifierCapital() const
{
	return m_iInternalTradeRouteYieldModifierCapital;
}
int CvPolicyEntry::GetTradeRouteYieldModifierCapital() const
{
	return m_iTradeRouteYieldModifierCapital;
}
int CvPolicyEntry::GetPositiveWarScoreTourismMod() const
{
	return m_iPositiveWarScoreTourismMod;
}

#endif

bool CvPolicyEntry::IsNoCSDecayAtWar() const
{
	return m_bNoCSDecayAtWar;
}
bool CvPolicyEntry::CanBullyFriendlyCS() const
{
	return m_bBullyFriendlyCS;
}
int CvPolicyEntry::GetBullyGlobalCSReduction() const
{
	return m_iBullyGlobalCSReduction;
}
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
bool CvPolicyEntry::IsVassalsNoRebel() const
{
	return m_bVassalsNoRebel;
}
int CvPolicyEntry::GetVassalCSBonusModifier() const
{
	return m_iVassalCSBonusModifier;
}
#endif

/// Boost to tourism bonus for shared religion
int CvPolicyEntry::GetSharedReligionTourismModifier() const
{
	return m_iSharedReligionTourismModifier;
}

/// Boost to tourism bonus for trade routes
int CvPolicyEntry::GetTradeRouteTourismModifier() const
{
	return m_iTradeRouteTourismModifier;
}

/// Boost to tourism bonus for open borders
int CvPolicyEntry::GetOpenBordersTourismModifier() const
{
	return m_iOpenBordersTourismModifier;
}

/// Boost to museum theming
int CvPolicyEntry::GetCityStateTradeChange() const
{
	return m_iCityStateTradeChange;
}

/// Great People from Allied Minors?
bool CvPolicyEntry::IsMinorGreatPeopleAllies() const
{
	return m_bMinorGreatPeopleAllies;
}

/// Science bonus from Allied Minors?
bool CvPolicyEntry::IsMinorScienceAllies() const
{
	return m_bMinorScienceAllies;
}

/// Resource bonus from Minors?
bool CvPolicyEntry::IsMinorResourceBonus() const
{
	return m_bMinorResourceBonus;
}

/// What Policy Branch does this Policy belong to?
int CvPolicyEntry::GetPolicyBranchType() const
{
	return m_iPolicyBranchType;
}

/// How many extra branches are we allowed to pick from?
int CvPolicyEntry::GetNumExtraBranches() const
{
	return m_iNumExtraBranches;
}

/// Excess Happiness converted into Culture
int CvPolicyEntry::GetHappinessToCulture() const
{
	return m_iHappinessToCulture;
}

/// Excess Happiness converted into Science
int CvPolicyEntry::GetHappinessToScience() const
{
	return m_iHappinessToScience;
}

/// Cities that receive a free culture building
int CvPolicyEntry::GetNumCitiesFreeCultureBuilding() const
{
	return m_iNumCitiesFreeCultureBuilding;
}

/// Cities that receive a free food building
int CvPolicyEntry::GetNumCitiesFreeFoodBuilding() const
{
	return m_iNumCitiesFreeFoodBuilding;
}

/// No Unhappiness from Specialist Population
bool CvPolicyEntry::IsHalfSpecialistUnhappiness() const
{
	return m_bHalfSpecialistUnhappiness;
}

/// Specialists don't eat food
bool CvPolicyEntry::IsHalfSpecialistFood() const
{
	return m_bHalfSpecialistFood;
}
#if defined(MOD_BALANCE_CORE)
/// Specialists don't eat food
bool CvPolicyEntry::IsHalfSpecialistFoodCapital() const
{
	return m_bHalfSpecialistFoodCapital;
}
int CvPolicyEntry::GetStealGWSlowerModifier() const
{
	return m_iStealGWSlowerModifier;
}
int CvPolicyEntry::GetStealGWFasterModifier() const
{
	return m_iStealGWFasterModifier;
}
int CvPolicyEntry::GetEventTourism() const
{
	return m_iEventTourism;
}
int CvPolicyEntry::GetEventTourismCS() const
{
	return m_iEventTourismCS;
}
int CvPolicyEntry::GetMonopolyModFlat() const
{
	return m_iMonopolyModFlat;
}
int CvPolicyEntry::GetMonopolyModPercent() const
{
	return m_iMonopolyModPercent;
}
/// Dummy Policy?
bool CvPolicyEntry::IsDummy() const
{
	return m_bDummy;
}
bool CvPolicyEntry::IsOpener() const
{
	return m_bOpener;
}
bool CvPolicyEntry::IsFinisher() const
{
	return m_bFinisher;
}
int CvPolicyEntry::GetCityStateCombatModifier() const
{
	return m_iCityStateCombatModifier;
}
int CvPolicyEntry::GetGreatEngineerRateModifier() const
{
	return m_iGreatEngineerRateModifier;
}
int CvPolicyEntry::GetDefenseBoost() const
{
	return m_iDefenseBoost;
}
#endif
/// Military units now all produced with food
bool CvPolicyEntry::IsMilitaryFoodProduction() const
{
	return m_bMilitaryFoodProduction;
}

/// Mod to how much damage a Unit does when wounded
int CvPolicyEntry::GetWoundedUnitDamageMod() const
{
	return m_iWoundedUnitDamageMod;
}

/// Mod to unit upgrade cost
int CvPolicyEntry::GetUnitUpgradeCostMod() const
{
	return m_iUnitUpgradeCostMod;
}

/// Combat bonus when fighting Barb Units
int CvPolicyEntry::GetBarbarianCombatBonus() const
{
	return m_iBarbarianCombatBonus;
}

/// Can we now see when and where Barb Camps appear?
bool CvPolicyEntry::IsAlwaysSeeBarbCamps() const
{
	return m_bAlwaysSeeBarbCamps;
}

/// Reveal all Minor Civ capital locations
bool CvPolicyEntry::IsRevealAllCapitals() const
{
	return m_bRevealAllCapitals;
}

/// Save on maintenance on garrisons?
bool CvPolicyEntry::IsGarrisonFreeMaintenance() const
{
	return m_bGarrisonFreeMaintenance;
}

bool CvPolicyEntry::IsGoldenAgeCultureBonusDisabled() const
{
	return m_bGoldenAgeCultureBonusDisabled;
}

bool CvPolicyEntry::IsSecondReligionPantheon() const
{
	return m_bSecondReligionPantheon;
}

bool CvPolicyEntry::IsAddReformationBelief() const
{
	return m_bAddReformationBelief;
}

bool CvPolicyEntry::IsEnablesSSPartHurry() const
{
	return m_bEnablesSSPartHurry;
}

bool CvPolicyEntry::IsEnablesSSPartPurchase() const
{
	return m_bEnablesSSPartPurchase;
}

/// Are we able to buy out City-States?
bool CvPolicyEntry::IsAbleToAnnexCityStates() const
{
	return m_bAbleToAnnexCityStates;
}

/// Only trade with same ideologies
bool CvPolicyEntry::IsOnlyTradeSameIdeology() const
{
	return m_bIsOnlyTradeSameIdeology;
}

/// Is this a one shot policy effect
bool CvPolicyEntry::IsOneShot() const
{
	return m_bOneShot;
}

/// Are there one shot free units that come with this policy?
bool CvPolicyEntry::IncludesOneShotFreeUnits() const
{
	return m_bIncludesOneShotFreeUnits;
}
#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
int CvPolicyEntry::GetPovertyHappinessChangePolicy() const
{
	return m_iPovertyHappinessChangePolicy;
}
int CvPolicyEntry::GetDefenseHappinessChangePolicy() const
{
	return m_iDefenseHappinessChangePolicy;
}
int CvPolicyEntry::GetUnculturedHappinessChangePolicy() const
{
	return m_iUnculturedHappinessChangePolicy;
}
int CvPolicyEntry::GetIlliteracyHappinessChangePolicy() const
{
	return m_iIlliteracyHappinessChangePolicy;
}
int CvPolicyEntry::GetMinorityHappinessChangePolicy() const
{
	return m_iMinorityHappinessChangePolicy;
}
int CvPolicyEntry::GetPovertyHappinessChangePolicyCapital() const
{
	return m_iPovertyHappinessChangePolicyCapital;
}
int CvPolicyEntry::GetDefenseHappinessChangePolicyCapital() const
{
	return m_iDefenseHappinessChangePolicyCapital;
}
int CvPolicyEntry::GetUnculturedHappinessChangePolicyCapital() const
{
	return m_iUnculturedHappinessChangePolicyCapital;
}
int CvPolicyEntry::GetIlliteracyHappinessChangePolicyCapital() const
{
	return m_iIlliteracyHappinessChangePolicyCapital;
}
int CvPolicyEntry::GetMinorityHappinessChangePolicyCapital() const
{
	return m_iMinorityHappinessChangePolicyCapital;
}
int CvPolicyEntry::GetPuppetUnhappinessMod() const
{
	return m_iPuppetUnhappinessModPolicy;
}
int CvPolicyEntry::GetNoUnhappfromXSpecialists() const
{
	return m_iNoUnhappfromXSpecialists;
}
int CvPolicyEntry::GetHappfromXSpecialists() const
{
	return m_iHappfromXSpecialists;
}
int CvPolicyEntry::GetNoUnhappfromXSpecialistsCapital() const
{
	return m_iNoUnhappfromXSpecialistsCapital;
}
int CvPolicyEntry::GetSpecialistFoodChange() const
{
	return m_iSpecialistFoodChange;
}
int CvPolicyEntry::GetWarWearinessModifier() const
{
	return m_iWarWearinessModifier;
}
int CvPolicyEntry::GetWarScoreModifier() const
{
	return m_iWarScoreModifier;
}

int CvPolicyEntry::GetGreatGeneralExtraBonus() const
{
	return m_iGreatGeneralExtraBonus;
}
#endif

/// Return "We Love the King" day text
const char* CvPolicyEntry::GetWeLoveTheKing()
{
	return m_wstrWeLoveTheKing.c_str();
}

/// Set "We Love the King" day text
void CvPolicyEntry::SetWeLoveTheKingKey(const char* szVal)
{
	m_strWeLoveTheKingKey = szVal;
}

// ARRAYS

/// Prerequisite policies with OR
int CvPolicyEntry::GetPrereqOrPolicies(int i) const
{
	return m_piPrereqOrPolicies ? m_piPrereqOrPolicies[i] : -1;
}

/// Prerequisite policies with AND
int CvPolicyEntry::GetPrereqAndPolicies(int i) const
{
	return m_piPrereqAndPolicies ? m_piPrereqAndPolicies[i] : -1;
}

/// Policies disabled when this one achieved
int CvPolicyEntry::GetPolicyDisables(int i) const
{
	return m_piPolicyDisables ? m_piPolicyDisables[i] : -1;
}

/// Change to yield by type
int CvPolicyEntry::GetYieldModifier(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldModifier ? m_piYieldModifier[i] : -1;
}

/// Array of yield changes
int* CvPolicyEntry::GetYieldModifierArray() const
{
	return m_piYieldModifier;
}
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
/// Change to traderoute yield modifier by type
int CvPolicyEntry::GetInternationalRouteYieldModifier(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piInternationalRouteYieldModifiers ? m_piInternationalRouteYieldModifiers[i] : -1;
}

int* CvPolicyEntry::GetInternationalRouteYieldModifiersArray()
{
	return m_piInternationalRouteYieldModifiers;
}

#endif

/// Change to yield in every City by type
int CvPolicyEntry::GetCityYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piCityYieldChange ? m_piCityYieldChange[i] : -1;
}

/// Array of yield changes in cities
int* CvPolicyEntry::GetCityYieldChangeArray() const
{
	return m_piCityYieldChange;
}

/// Change to yield in coastal Cities by type
int CvPolicyEntry::GetCoastalCityYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piCoastalCityYieldChange ? m_piCoastalCityYieldChange[i] : -1;
}

/// Array of yield changes in coastal Cities
int* CvPolicyEntry::GetCoastalCityYieldChangeArray() const
{
	return m_piCoastalCityYieldChange;
}

/// Change to yield in Capital by type
int CvPolicyEntry::GetCapitalYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piCapitalYieldChange ? m_piCapitalYieldChange[i] : -1;
}

/// Array of yield changes in Capital
int* CvPolicyEntry::GetCapitalYieldChangeArray() const
{
	return m_piCapitalYieldChange;
}

/// Change to yield in Capital by type (per pop)
int CvPolicyEntry::GetCapitalYieldPerPopChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piCapitalYieldPerPopChange ? m_piCapitalYieldPerPopChange[i] : -1;
}

/// Array of yield changes in Capital (per pop)
int* CvPolicyEntry::GetCapitalYieldPerPopChangeArray() const
{
	return m_piCapitalYieldPerPopChange;
}


/// Change to yield in Capital by type (per pop)
int CvPolicyEntry::GetCapitalYieldPerPopChangeEmpire(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piCapitalYieldPerPopChangeEmpire ? m_piCapitalYieldPerPopChangeEmpire[i] : -1;
}

/// Array of yield changes in Capital (per pop)
int* CvPolicyEntry::GetCapitalYieldPerPopChangeEmpireArray() const
{
	return m_piCapitalYieldPerPopChangeEmpire;
}
/// Change to yield in capital by type
int CvPolicyEntry::GetCapitalYieldModifier(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piCapitalYieldModifier ? m_piCapitalYieldModifier[i] : -1;
}

/// Array of yield changes in capital
int* CvPolicyEntry::GetCapitalYieldModifierArray() const
{
	return m_piCapitalYieldModifier;
}

/// Change to Great Work yield by type
int CvPolicyEntry::GetGreatWorkYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piGreatWorkYieldChange ? m_piGreatWorkYieldChange[i] : -1;
}

/// Array of yield changes to Great Works
int* CvPolicyEntry::GetGreatWorkYieldChangeArray() const
{
	return m_piGreatWorkYieldChange;
}

/// Extra specialist yield by yield type
int CvPolicyEntry::GetSpecialistExtraYield(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piSpecialistExtraYield ? m_piSpecialistExtraYield[i] : -1;
}

/// Array of extra specialist yield
int* CvPolicyEntry::GetSpecialistExtraYieldArray() const
{
	return m_piSpecialistExtraYield;
}

/// Production modifier by unit type
int CvPolicyEntry::GetUnitCombatProductionModifiers(int i) const
{
	CvAssertMsg(i < GC.getNumUnitCombatClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiUnitCombatProductionModifiers ? m_paiUnitCombatProductionModifiers[i] : -1;
}
/// Do all Units get Promotion ID i?
int CvPolicyEntry::IsFreePromotion(int i) const
{
	return m_pabFreePromotion ? m_pabFreePromotion[i] : -1;
}

/// Does the specific unit combat get a specific free promotion?
bool CvPolicyEntry::IsFreePromotionUnitCombat(const int promotionID, const int unitCombatID) const
{
	std::multimap<int, int>::const_iterator it = m_FreePromotionUnitCombats.find(promotionID);
	if(it != m_FreePromotionUnitCombats.end())
	{
		// get an iterator to the element that is one past the last element associated with key
		std::multimap<int, int>::const_iterator lastElement = m_FreePromotionUnitCombats.upper_bound(promotionID);

		// for each element in the sequence [itr, lastElement)
		for(; it != lastElement; ++it)
		{
			if(it->second == unitCombatID)
			{
				return true;
			}
		}
	}

	return false;
}

#if defined(MOD_RELIGION_POLICY_BRANCH_FAITH_GP)
bool CvPolicyEntry::HasFaithPurchaseUnitClasses() const
{
	return (m_FaithPurchaseUnitClasses.size() != 0);
}

bool CvPolicyEntry::IsFaithPurchaseUnitClass(const int eUnitClass, const int eCurrentEra) const
{
	std::multimap<int, int>::const_iterator it = m_FaithPurchaseUnitClasses.find(eUnitClass);
	if (it != m_FaithPurchaseUnitClasses.end())
	{
		const int eRequiredEra = it->second;
		
		if (eRequiredEra == NO_ERA || eCurrentEra >= eRequiredEra)
		{
			return true;
		}
	}

	return false;
}
#endif

#if defined(MOD_BALANCE_CORE_POLICIES) && defined(MOD_API_UNIFIED_YIELDS)
/// What is the golden age modifier for the specific yield type?
int CvPolicyEntry::GetYieldChangesPerReligionTimes100(int iYield) const
{
	CvAssertMsg((YieldTypes)iYield < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg((YieldTypes)iYield > -1, "Index out of bounds");

	return m_piYieldChangesPerReligion ? m_piYieldChangesPerReligion[iYield] : 0;
}
/// What is the golden age modifier for the specific yield type?
int* CvPolicyEntry::GetYieldChangesPerReligionTimes100Array() const
{
	return m_piYieldChangesPerReligion;
}
#endif

/// Free experience by unit type
int CvPolicyEntry::GetUnitCombatFreeExperiences(int i) const
{
	CvAssertMsg(i < GC.getNumUnitCombatClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiUnitCombatFreeExperiences ? m_paiUnitCombatFreeExperiences[i] : -1;
}

/// Amount of extra Culture per turn a BuildingClass provides
int CvPolicyEntry::GetBuildingClassCultureChange(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiBuildingClassCultureChanges ? m_paiBuildingClassCultureChanges[i] : -1;
}

/// Amount of extra Culture per turn a BuildingClass provides
int CvPolicyEntry::GetBuildingClassHappiness(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiBuildingClassHappiness ? m_paiBuildingClassHappiness[i] : -1;
}

/// Number of free Units provided by this Policy
int CvPolicyEntry::GetNumFreeUnitsByClass(int i) const
{
	CvAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiFreeUnitClasses ? m_paiFreeUnitClasses[i] : -1;
}

/// Instant tourism bump when a unit of a particular class is created
int CvPolicyEntry::GetTourismByUnitClassCreated(int i) const
{
	CvAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiTourismOnUnitCreation ? m_paiTourismOnUnitCreation[i] : -1;
}

/// Is this hurry type now enabled?
//bool CvPolicyEntry::IsHurry(int i) const
//{
//	FAssertMsg(i < GC.getNumHurryInfos(), "Index out of bounds");
//	FAssertMsg(i > -1, "Index out of bounds");
//	return m_pabHurry ? m_pabHurry[i] : false;
//}

/// Modifier to Hurry cost
int CvPolicyEntry::GetHurryModifier(int i) const
{
	CvAssertMsg(i < GC.getNumHurryInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiHurryModifier ? m_paiHurryModifier[i] : -1;
}

/// Is this type of specialist now valid?
bool CvPolicyEntry::IsSpecialistValid(int i) const
{
	CvAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pabSpecialistValid ? m_pabSpecialistValid[i] : false;
}
#if defined(MOD_BALANCE_CORE)
/// Does this Policy grant a free building?
int CvPolicyEntry::GetFreeChosenBuilding(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiFreeChosenBuilding ? m_paiFreeChosenBuilding[i] : -1;
}
#endif
#if defined(MOD_BALANCE_CORE_POLICIES)
int CvPolicyEntry::GetResourceFromCSAlly(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piResourcefromCSAlly[i];
}
/// Does this Policy grant yields from citizen birth?
int CvPolicyEntry::GetYieldFromBirth(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldFromBirth[i];
}
/// Does this Policy grant yields from citizen birth in the Capital?
int CvPolicyEntry::GetYieldFromBirthCapital(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldFromBirthCapital[i];
}

int CvPolicyEntry::GetYieldFromBirthRetroactive(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldFromBirthRetroactive[i];
}
/// Does this Policy grant yields from citizen birth in the Capital?
int CvPolicyEntry::GetYieldFromBirthCapitalRetroactive(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldFromBirthCapitalRetroactive[i];
}
/// Does this Policy grant yields from constructing buildings?
int CvPolicyEntry::GetYieldFromConstruction(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldFromConstruction[i];
}
/// Does this Policy grant yields from constructing buildings?
int CvPolicyEntry::GetYieldFromWonderConstruction(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldFromWonderConstruction[i];
}
/// Does this Policy grant yields from researching techs?
int CvPolicyEntry::GetYieldFromTech(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldFromTech[i];
}
/// Does this Policy negate expansion unhappiness?
bool CvPolicyEntry::GetNoUnhappinessExpansion() const
{
	return m_bNoUnhappinessExpansion;
}
/// Does this Policy negate isolation unhappiness?
bool CvPolicyEntry::GetNoUnhappyIsolation() const
{
	return m_bNoUnhappyIsolation;
}
/// Does this Policy generate double border growth during GAs?
bool CvPolicyEntry::GetDoubleBorderGA() const
{
	return m_bDoubleBorderGA;
}
/// Increased influence from quests?
int CvPolicyEntry::GetIncreasedQuestInfluence() const
{
	return m_iIncreasedQuestInfluence;
}
/// Better beakers?
int CvPolicyEntry::GetGreatScientistBeakerModifier() const
{
	return m_iGreatScientistBeakerModifier;
}
int CvPolicyEntry::GetGreatEngineerHurryModifier() const
{
	return m_iGreatEngineerHurryModifier;
}
int CvPolicyEntry::GetTechCostXCitiesMod() const
{
	return m_iTechCostXCitiesMod;
}

int CvPolicyEntry::GetTourismCostXCitiesMod() const
{
	return m_iTourismCostXCitiesMod;
}
/// Citadel Boost?
int CvPolicyEntry::GetCultureBombBoost() const
{
	return m_iCultureBombBoost;
}
/// Puppet Production Boost?
int CvPolicyEntry::GetPuppetProdMod() const
{
	return m_iPuppetProdMod;
}
/// Occupied Production Boost?
int CvPolicyEntry::GetOccupiedProdMod() const
{
	return m_iOccupiedProdMod;
}
/// Gold from internal trade routes?
int CvPolicyEntry::GetInternalTradeGold() const
{
	return m_iInternalTradeGold;
}
//Free WC Votes?
int CvPolicyEntry::GetFreeWCVotes() const
{
	return m_iFreeWCVotes;
}
//Influence from GP expenditure?
int CvPolicyEntry::GetInfluenceGPExpend() const
{
	return m_iInfluenceGPExpend;
}
//Free Trade Route?
int CvPolicyEntry::GetFreeTradeRoute() const
{
	return m_iFreeTradeRoute;
}
//Free Spy?
int CvPolicyEntry::GetFreeSpy() const
{
	return m_iFreeSpy;
}

int CvPolicyEntry::GetReligionDistance() const
{
	return m_iReligionDistance;
}
int CvPolicyEntry::GetPressureMod() const
{
	return m_iPressureMod;
}

/// Does this Policy grant yields from border growth?
int CvPolicyEntry::GetYieldFromBorderGrowth(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldFromBorderGrowth[i];
}
/// Does this Policy grant yields from expending GPs?
int CvPolicyEntry::GetYieldGPExpend(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldGPExpend[i];
}
/// Does this Policy grant unhappiness reduction from garrisons?
int CvPolicyEntry::GetGarrisonsOccupiedUnhapppinessMod() const
{
	return m_iGarrisonsOccupiedUnhapppinessMod;
}
/// Does this Policy grant religios pressure from trade?
int CvPolicyEntry::GetTradeReligionModifier() const
{
	return m_iTradeReligionModifier;
}
/// Does this Policy grant a free ranged unit upon settling?
int CvPolicyEntry::GetXPopulationConscription() const
{
	return m_iXPopulationConscription;
}
/// Policy Grants best number of land combat units
int CvPolicyEntry::GetBestNumberLandCombatUnitClass() const
{
	return m_iBestNumberLandCombatUnitClass;
}
/// Policy Grants best number of land ranged units
int CvPolicyEntry::GetBestNumberLandRangedUnitClass() const
{
	return m_iBestNumberLandRangedUnitClass;
}
/// Policy Grants best number of sea comat units
int CvPolicyEntry::GetBestNumberSeaCombatUnitClass() const
{
	return m_iBestNumberSeaCombatUnitClass;
}
/// Policy Grants best number of sea ranged units
int CvPolicyEntry::GetBestNumberSeaRangedUnitClass() const
{
	return m_iBestNumberSeaRangedUnitClass;
}
/// Does this Policy grant free population?
int CvPolicyEntry::GetFreePopulation() const
{
	return m_iFreePopulation;
}
int CvPolicyEntry::GetFreePopulationCapital() const
{
	return m_iFreePopulationCapital;
}
/// Does this Policy grant free moves?
int CvPolicyEntry::GetExtraMoves() const
{
	return m_iExtraMoves;
}
/// Does this Policy grant more corps?
int CvPolicyEntry::GetMaxCorps() const
{
	return m_iMaxCorporations;
}
/// Does this Policy grant faster razing?
int CvPolicyEntry::GetRazingSpeedBonus() const
{
	return m_iRazingSpeedBonus;
}
/// Does this Policy grant more military units?
int CvPolicyEntry::GetExtraSupplyPerPopulation() const
{
	return m_iExtraSupplyPerPopulation;
}
/// Does this Policy grant faster razing?
bool CvPolicyEntry::IsNoPartisans() const
{
	return m_bNoPartisans;
}
/// Does this Policy grant yields from conquering cities?
int CvPolicyEntry::GetConquerorYield(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piConquerorYield[i];
}

/// Does this Policy grant yields from conquering cities?
int CvPolicyEntry::GetFounderYield(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piFounderYield[i];
}
/// Does this Policy boost yields from having your religion in the city?
int CvPolicyEntry::GetReligionYieldMod(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piReligionYieldMod[i];
}
/// Does this Policy boost building yields from having your religion in the city?
int CvPolicyEntry::GetReligionBuildingYieldMod(int i, int j) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiReligionBuildingYieldMod[i][j];
}
/// Does this Policy boost yields from golden ages?
int CvPolicyEntry::GetGoldenAgeYieldMod(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piGoldenAgeYieldMod[i];
}

#endif
#if defined(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
int CvPolicyEntry::GetInvestmentModifier () const
{
	return m_iInvestmentModifier ;
}
#endif
/// Yield modifier for a specific improvement by yield type
int CvPolicyEntry::GetImprovementYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiImprovementYieldChanges[i][j];
}

#if defined(MOD_API_UNIFIED_YIELDS) && defined(MOD_API_PLOT_YIELDS)
/// Yield modifier for a specific plot by yield type
int CvPolicyEntry::GetPlotYieldChanges(int i, int j) const
{
	if (MOD_API_PLOT_YIELDS) {
		CvAssertMsg(i < GC.getNumPlotInfos(), "Index out of bounds");
		CvAssertMsg(i > -1, "Index out of bounds");
		CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
		CvAssertMsg(j > -1, "Index out of bounds");
		return m_ppiPlotYieldChanges[i][j];
	} else {
		return 0;
	}
}
#endif

#if defined(MOD_API_UNIFIED_YIELDS)
int CvPolicyEntry::GetFeatureYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiFeatureYieldChanges[i][j];
}

int CvPolicyEntry::GetCityYieldFromUnimprovedFeature(int i, int j) const
{
	CvAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiCityYieldFromUnimprovedFeature[i][j];
}

int CvPolicyEntry::GetUnimprovedFeatureYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiUnimprovedFeatureYieldChanges[i][j];
}

int CvPolicyEntry::GetResourceYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiResourceYieldChanges[i][j];
}

int CvPolicyEntry::GetTerrainYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiTerrainYieldChanges[i][j];
}

int CvPolicyEntry::GetTradeRouteYieldChange(int i, int j) const
{
	CvAssertMsg(i < NUM_DOMAIN_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiTradeRouteYieldChange[i][j];
}

int CvPolicyEntry::GetSpecialistYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiSpecialistYieldChanges[i][j];
}

int CvPolicyEntry::GetGreatPersonExpendedYield(int i, int j) const
{
	CvAssertMsg(i < GC.getNumGreatPersonInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiGreatPersonExpendedYield[i][j];
}

int CvPolicyEntry::GetGoldenAgeGreatPersonRateModifier(int i) const
{
	CvAssertMsg(i < GC.getNumGreatPersonInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piGoldenAgeGreatPersonRateModifier ? m_piGoldenAgeGreatPersonRateModifier[i] : 0;
}

int CvPolicyEntry::GetYieldFromKills(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldFromKills ? m_piYieldFromKills[i] : 0;
}

int* CvPolicyEntry::GetYieldFromKillsArray() const
{
	return m_piYieldFromKills;
}

int CvPolicyEntry::GetYieldFromBarbarianKills(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldFromBarbarianKills ? m_piYieldFromBarbarianKills[i] : 0;
}

int* CvPolicyEntry::GetYieldFromBarbarianKillsArray() const
{
	return m_piYieldFromBarbarianKills;
}

int CvPolicyEntry::GetYieldChangeTradeRoute(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChangeTradeRoute ? m_piYieldChangeTradeRoute[i] : 0;
}

int* CvPolicyEntry::GetYieldChangeTradeRouteArray() const
{
	return m_piYieldChangeTradeRoute;
}

int CvPolicyEntry::GetYieldChangesNaturalWonder(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChangesNaturalWonder ? m_piYieldChangesNaturalWonder[i] : 0;
}

int* CvPolicyEntry::GetYieldChangesNaturalWonderArray() const
{
	return m_piYieldChangesNaturalWonder;
}

int CvPolicyEntry::GetYieldChangeWorldWonder(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChangeWorldWonder ? m_piYieldChangeWorldWonder[i] : 0;
}

int* CvPolicyEntry::GetYieldChangeWorldWonderArray() const
{
	return m_piYieldChangeWorldWonder;
}

int CvPolicyEntry::GetYieldFromMinorDemand(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldFromMinorDemand ? m_piYieldFromMinorDemand[i] : 0;
}

int* CvPolicyEntry::GetYieldFromMinorDemandArray() const
{
	return m_piYieldFromMinorDemand;
}

int CvPolicyEntry::GetYieldFromWLTKD(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldFromWLTKD ? m_piYieldFromWLTKD[i] : 0;
}

int* CvPolicyEntry::GetYieldFromWLTKDArray() const
{
	return m_piYieldFromWLTKD;
}

int CvPolicyEntry::GetArtifactYieldChanges(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piArtifactYieldChanges ? m_piArtifactYieldChanges[i] : 0;
}

int* CvPolicyEntry::GetArtifactYieldChangesArray() const
{
	return m_piArtifactYieldChanges;
}

int CvPolicyEntry::GetArtYieldChanges(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piArtYieldChanges ? m_piArtYieldChanges[i] : 0;
}

int* CvPolicyEntry::GetArtYieldChangesArray() const
{
	return m_piArtYieldChanges;
}

int CvPolicyEntry::GetLitYieldChanges(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piLitYieldChanges ? m_piLitYieldChanges[i] : 0;
}

int* CvPolicyEntry::GetLitYieldChangesArray() const
{
	return m_piLitYieldChanges;
}

int CvPolicyEntry::GetMusicYieldChanges(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piMusicYieldChanges ? m_piMusicYieldChanges[i] : 0;
}

int* CvPolicyEntry::GetMusicYieldChangesArray() const
{
	return m_piMusicYieldChanges;
}

int CvPolicyEntry::GetRelicYieldChanges(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piRelicYieldChanges ? m_piRelicYieldChanges[i] : 0;
}

int* CvPolicyEntry::GetRelicYieldChangesArray() const
{
	return m_piRelicYieldChanges;
}

int CvPolicyEntry::GetFilmYieldChanges(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piFilmYieldChanges ? m_piFilmYieldChanges[i] : 0;
}

int* CvPolicyEntry::GetFilmYieldChangesArray() const
{
	return m_piFilmYieldChanges;
}

int CvPolicyEntry::GetYieldFromNonSpecialistCitizens(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldFromNonSpecialistCitizens ? m_piYieldFromNonSpecialistCitizens[i] : 0;
}

int* CvPolicyEntry::GetYieldFromNonSpecialistCitizensArray() const
{
	return m_piYieldFromNonSpecialistCitizens;
}
int CvPolicyEntry::GetYieldModifierFromGreatWorks(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldModifierFromGreatWorks ? m_piYieldModifierFromGreatWorks[i] : 0;
}

int* CvPolicyEntry::GetYieldModifierFromGreatWorksArray() const
{
	return m_piYieldModifierFromGreatWorks;
}

int CvPolicyEntry::GetYieldModifierFromActiveSpies(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldModifierFromActiveSpies ? m_piYieldModifierFromActiveSpies[i] : 0;
}

int* CvPolicyEntry::GetYieldModifierFromActiveSpiesArray() const
{
	return m_piYieldModifierFromActiveSpies;
}

int CvPolicyEntry::GetYieldFromDelegateCount(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldFromDelegateCount ? m_piYieldFromDelegateCount[i] : 0;
}

int* CvPolicyEntry::GetYieldFromDelegateCountArray() const
{
	return m_piYieldFromDelegateCount;
}

int CvPolicyEntry::GetMissionInfluenceModifier() const
{
	return m_iMissionInfluenceModifier;
}

int CvPolicyEntry::GetHappinessPerActiveTradeRoute() const
{
	return m_iHappinessPerActiveTradeRoute;
}

int CvPolicyEntry::GetNeedsModifierFromAirUnits() const
{
	return m_iNeedsModifierFromAirUnits;
}
int CvPolicyEntry::GetFlatDefenseFromAirUnits() const
{
	return m_iFlatDefenseFromAirUnits;
}
int CvPolicyEntry::GetPuppetYieldPenaltyMod() const
{
	return m_iPuppetYieldPenaltyMod;
}
int CvPolicyEntry::GetConquestPerEraBuildingProductionMod() const
{
	return m_iConquestPerEraBuildingProductionMod;
}
int CvPolicyEntry::GetAdmiralLuxuryBonus() const
{
	return m_iAdmiralLuxuryBonus;
}

bool CvPolicyEntry::IsCSResourcesForMonopolies() const
{
	return m_bCSResourcesForMonopolies;
}
#endif

/// Yield modifier for a specific BuildingClass by yield type
int CvPolicyEntry::GetBuildingClassYieldModifiers(int i, int j) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiBuildingClassYieldModifiers[i][j];
}

/// Yield change for a specific BuildingClass by yield type
int CvPolicyEntry::GetBuildingClassYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiBuildingClassYieldChanges[i][j];
}

/// Production modifier for a specific BuildingClass
int CvPolicyEntry::GetBuildingClassProductionModifier(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiBuildingClassProductionModifiers[i];
}
/// Production modifier for a specific UnitClass
int CvPolicyEntry::GetUnitClassProductionModifiers(int i) const
{
	CvAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiUnitClassProductionModifiers[i];
}

/// Tourism modifier for a specific BuildingClass
int CvPolicyEntry::GetBuildingClassTourismModifier(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiBuildingClassTourismModifiers[i];
}

/// Find value of flavors associated with this policy
int CvPolicyEntry::GetFlavorValue(int i) const
{
	CvAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}

/// Culture modifier for a specific improvement
int CvPolicyEntry::GetImprovementCultureChanges(int i) const
{
	CvAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piImprovementCultureChange[i];
}

/// Free building in each city conquered
BuildingTypes CvPolicyEntry::GetFreeBuildingOnConquest() const
{
	return m_eFreeBuildingOnConquest;
}

//=====================================
// CvPolicyBranchEntry
//=====================================

/// Constructor
CvPolicyBranchEntry::CvPolicyBranchEntry(void):
	m_iEraPrereq(NO_ERA),
	m_iFreePolicy(NO_POLICY),
	m_iFreeFinishingPolicy(NO_POLICY),
	m_iFirstAdopterFreePolicies(0),
	m_iSecondAdopterFreePolicies(0),
#if defined(MOD_BALANCE_CORE)
	m_iNumPolicyRequirement(0),
#endif
	m_piPolicyBranchDisables(NULL)
{
}

/// Destructor
CvPolicyBranchEntry::~CvPolicyBranchEntry(void)
{
	SAFE_DELETE_ARRAY(m_piPolicyBranchDisables);
}

/// Read from XML file (pass 1)
bool CvPolicyBranchEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	//References
	const char* szEraPrereq = kResults.GetText("EraPrereq");
	m_iEraPrereq = GC.getInfoTypeForString(szEraPrereq, true);

	const char* szFreePolicy = kResults.GetText("FreePolicy");
	m_iFreePolicy = GC.getInfoTypeForString(szFreePolicy, true);

	const char* szFreeFinishingPolicy = kResults.GetText("FreeFinishingPolicy");
	m_iFreeFinishingPolicy = GC.getInfoTypeForString(szFreeFinishingPolicy, true);

	m_iFirstAdopterFreePolicies = kResults.GetInt("FirstAdopterFreePolicies");
	m_iSecondAdopterFreePolicies = kResults.GetInt("SecondAdopterFreePolicies");

	const char* szPolicyBranchType = GetType();

	m_bPurchaseByLevel = kResults.GetBool("PurchaseByLevel");
	m_bLockedWithoutReligion = kResults.GetBool("LockedWithoutReligion");
	m_bMutuallyExclusive = kResults.GetBool("AIMutuallyExclusive");

	m_bDelayWhenNoReligion = kResults.GetBool("AIDelayNoReligion");
	m_bDelayWhenNoCulture = kResults.GetBool("AIDelayNoCulture");
	m_bDelayWhenNoCityStates = kResults.GetBool("AIDelayNoCityStates");
	m_bDelayWhenNoScience = kResults.GetBool("AIDelayNoScience");
#if defined(MOD_BALANCE_CORE)
	m_iNumPolicyRequirement = kResults.GetInt("NumPolicyRequirement");
#endif

	//PolicyBranch_Disables
	{
		kUtility.InitializeArray(m_piPolicyBranchDisables, "PolicyBranchTypes", (int)NO_POLICY_BRANCH_TYPE);

		std::string sqlKey = "m_piPolicyBranchDisables";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select PolicyBranchTypes.ID from PolicyBranch_Disables inner join PolicyBranchTypes on PolicyBranchTypes.Type = PolicyBranchDisable where PolicyBranchType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szPolicyBranchType, false);

		int iID;
		while(pResults->Step())
		{
			iID = pResults->GetInt(0);
			m_piPolicyBranchDisables[iID] = 1;
		}

		pResults->Reset();
	}

	return true;
}

/// Era prerequisite
int CvPolicyBranchEntry::GetEraPrereq() const
{
	return m_iEraPrereq;
}

/// Do we get a Policy for free with this (they'd be invisible - a way to give branches bonuses)
int CvPolicyBranchEntry::GetFreePolicy() const
{
	return m_iFreePolicy;
}

/// Do we get a Policy for free when we finish this branch (they'd be invisible - a way to make finishing branches meaningful)
int CvPolicyBranchEntry::GetFreeFinishingPolicy() const
{
	return m_iFreeFinishingPolicy;
}

/// How many Policies do we get for free when we open this branch before anyone else?
int CvPolicyBranchEntry::GetFirstAdopterFreePolicies() const
{
	return m_iFirstAdopterFreePolicies;
}

/// How many Policies do we get for free when we open this branch before anyone else?
int CvPolicyBranchEntry::GetSecondAdopterFreePolicies() const
{
	return m_iSecondAdopterFreePolicies;
}

/// Policy Branches disabled when this one chosen
int CvPolicyBranchEntry::GetPolicyBranchDisables(int i) const
{
	return m_piPolicyBranchDisables ? m_piPolicyBranchDisables[i] : -1;
}

/// Are policies in this branch unlocked by buying lower-level prereq policies?
bool CvPolicyBranchEntry::IsPurchaseByLevel() const
{
	return m_bPurchaseByLevel;
}

/// Are policies in this branch locked if religion is off?
bool CvPolicyBranchEntry::IsLockedWithoutReligion() const
{
	return m_bLockedWithoutReligion;
}

/// If AI chooses this branch, is it prohibited from choosing the other mutually exclusive ones?
bool CvPolicyBranchEntry::IsMutuallyExclusive() const
{
	return m_bMutuallyExclusive;
}

/// Should the AI delay selecting this branch when game has disabled religion?
bool CvPolicyBranchEntry::IsDelayWhenNoReligion() const
{
	return m_bDelayWhenNoReligion;
}

/// Should the AI delay selecting this branch when game has disabled culture?
bool CvPolicyBranchEntry::IsDelayWhenNoCulture() const
{
	return m_bDelayWhenNoCulture;
}

/// Should the AI delay selecting this branch when game has no city-states?
bool CvPolicyBranchEntry::IsDelayWhenNoCityStates() const
{
	return m_bDelayWhenNoCityStates;
}

/// Should the AI delay selecting this branch when game has disabled science?
bool CvPolicyBranchEntry::IsDelayWhenNoScience() const
{
	return m_bDelayWhenNoScience;
}
#if defined(MOD_BALANCE_CORE)
/// Policies needed to unlock branch.
int CvPolicyBranchEntry::GetNumPolicyRequirement() const
{
	return m_iNumPolicyRequirement;
}
#endif
//=====================================
// CvPolicyXMLEntries
//=====================================
/// Constructor
CvPolicyXMLEntries::CvPolicyXMLEntries(void)
{

}

/// Destructor
CvPolicyXMLEntries::~CvPolicyXMLEntries(void)
{
	DeletePoliciesArray();
	DeletePolicyBranchesArray();
}

/// Returns vector of policy entries
std::vector<CvPolicyEntry*>& CvPolicyXMLEntries::GetPolicyEntries()
{
	return m_paPolicyEntries;
}

/// Number of defined policies
int CvPolicyXMLEntries::GetNumPolicies()
{
	return m_paPolicyEntries.size();
}

/// Clear policy entries
void CvPolicyXMLEntries::DeletePoliciesArray()
{
	for(std::vector<CvPolicyEntry*>::iterator it = m_paPolicyEntries.begin(); it != m_paPolicyEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paPolicyEntries.clear();
}

/// Get a specific entry
CvPolicyEntry* CvPolicyXMLEntries::GetPolicyEntry(int index)
{
	return m_paPolicyEntries[index];
}

/// Returns vector of PolicyBranch entries
std::vector<CvPolicyBranchEntry*>& CvPolicyXMLEntries::GetPolicyBranchEntries()
{
	return m_paPolicyBranchEntries;
}

/// Number of defined PolicyBranches
int CvPolicyXMLEntries::GetNumPolicyBranches()
{
	return m_paPolicyBranchEntries.size();
}

/// Clear PolicyBranch entries
void CvPolicyXMLEntries::DeletePolicyBranchesArray()
{
	for(std::vector<CvPolicyBranchEntry*>::iterator it = m_paPolicyBranchEntries.begin(); it != m_paPolicyBranchEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paPolicyBranchEntries.clear();
}

/// Get a specific entry
CvPolicyBranchEntry* CvPolicyXMLEntries::GetPolicyBranchEntry(int index)
{
	return m_paPolicyBranchEntries[index];
}

//=====================================
// CvPlayerPolicies
//=====================================
/// Constructor
CvPlayerPolicies::CvPlayerPolicies():
#if defined(MOD_API_EXTENSIONS)
	m_pabFreePolicy(NULL),
#endif
	m_pabHasPolicy(NULL),
	m_pabHasOneShotPolicyFired(NULL),
	m_pabHaveOneShotFreeUnitsFired(NULL),
	m_pabPolicyBranchUnlocked(NULL),
	m_pabPolicyBranchBlocked(NULL),
	m_pabPolicyBranchFinished(NULL),
	m_paePolicyBranchesChosen(NULL),
	m_paePolicyBlockedBranchCheck(NULL),
	m_pPolicyAI(NULL),
	m_pPolicies(NULL),
	m_pPlayer(NULL)
{
#if defined(MOD_BALANCE_CORE)
	m_vBuildingClassTourismModifier.resize(GC.getNumBuildingClassInfos(), 0);
	m_vBuildingClassHappinessModifier.resize(GC.getNumBuildingClassInfos(), 0);
#endif
}

/// Destructor
CvPlayerPolicies::~CvPlayerPolicies(void)
{
}

/// Initialize
void CvPlayerPolicies::Init(CvPolicyXMLEntries* pPolicies, CvPlayer* pPlayer, bool bIsCity)
{
	// Init base class
	CvFlavorRecipient::Init();

	// Store off the pointer to the policies active for this game
	m_bIsCity = bIsCity;
	m_pPolicies = pPolicies;
	m_pPlayer = pPlayer;

	// Initialize policy status array
#if defined(MOD_API_EXTENSIONS)
	CvAssertMsg(m_pabFreePolicy==NULL, "about to leak memory, CvPlayerPolicies::m_pabFreePolicy");
	m_pabFreePolicy = FNEW(bool[m_pPolicies->GetNumPolicies()], c_eCiv5GameplayDLL, 0);
#endif
	CvAssertMsg(m_pabHasPolicy==NULL, "about to leak memory, CvPlayerPolicies::m_pabHasPolicy");
	m_pabHasPolicy = FNEW(bool[m_pPolicies->GetNumPolicies()], c_eCiv5GameplayDLL, 0);
	CvAssertMsg(m_pabHasOneShotPolicyFired==NULL, "about to leak memory, CvPlayerPolicies::m_pabHasOneShotPolicyFired");
	m_pabHasOneShotPolicyFired = FNEW(bool[m_pPolicies->GetNumPolicies()], c_eCiv5GameplayDLL, 0);
	CvAssertMsg(m_pabHaveOneShotFreeUnitsFired==NULL, "about to leak memory, CvPlayerPolicies::m_pabHaveOneShotFreeUnitsFired");
	m_pabHaveOneShotFreeUnitsFired = FNEW(bool[m_pPolicies->GetNumPolicies()], c_eCiv5GameplayDLL, 0);

	// Policy Branches Chosen
	CvAssertMsg(m_pabPolicyBranchUnlocked==NULL, "about to leak memory, CvPlayerPolicies::m_pabPolicyBranchUnlocked");
	m_pabPolicyBranchUnlocked = FNEW(bool[m_pPolicies->GetNumPolicyBranches()], c_eCiv5GameplayDLL, 0);

	// Policy Branches Blocked by choices
	CvAssertMsg(m_pabPolicyBranchBlocked==NULL, "about to leak memory, CvPlayerPolicies::m_pabPolicyBranchBlocked");
	m_pabPolicyBranchBlocked = FNEW(bool[m_pPolicies->GetNumPolicyBranches()], c_eCiv5GameplayDLL, 0);

	// Policy Branches finished
	CvAssertMsg(m_pabPolicyBranchFinished==NULL, "about to leak memory, CvPlayerPolicies::m_pabPolicyBranchFinished");
	m_pabPolicyBranchFinished = FNEW(bool[m_pPolicies->GetNumPolicyBranches()], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paePolicyBranchesChosen==NULL, "about to leak memory, CvPlayerPolicies::m_paePolicyBranchesChosen");
	m_paePolicyBranchesChosen = FNEW(PolicyBranchTypes[m_pPolicies->GetNumPolicyBranches()], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paePolicyBlockedBranchCheck==NULL, "about to leak memory, CvPlayerPolicies::m_paePolicyBlockedBranchCheck");
	m_paePolicyBlockedBranchCheck = FNEW(PolicyBranchTypes[m_pPolicies->GetNumPolicies()], c_eCiv5GameplayDLL, 0);
	
	// Create AI object
	m_pPolicyAI = FNEW(CvPolicyAI(this), c_eCiv5GameplayDLL, 0);

	Reset();
}

/// Deallocate memory created in initialize
void CvPlayerPolicies::Uninit()
{
	// Uninit base class
	CvFlavorRecipient::Uninit();

#if defined(MOD_API_EXTENSIONS)
	SAFE_DELETE_ARRAY(m_pabFreePolicy);
#endif
	SAFE_DELETE_ARRAY(m_pabHasPolicy);
	SAFE_DELETE_ARRAY(m_pabHasOneShotPolicyFired);
	SAFE_DELETE_ARRAY(m_pabHaveOneShotFreeUnitsFired);
	SAFE_DELETE_ARRAY(m_pabPolicyBranchUnlocked);
	SAFE_DELETE_ARRAY(m_pabPolicyBranchBlocked);
	SAFE_DELETE_ARRAY(m_pabPolicyBranchFinished);
	SAFE_DELETE_ARRAY(m_paePolicyBranchesChosen);
	SAFE_DELETE(m_pPolicyAI);
	SAFE_DELETE_ARRAY(m_paePolicyBlockedBranchCheck);
}

/// Reset policy status array to all false
void CvPlayerPolicies::Reset()
{
	int iI;

	for(iI = 0; iI < m_pPolicies->GetNumPolicies(); iI++)
	{
#if defined(MOD_API_EXTENSIONS)
		m_pabFreePolicy[iI] = false;
#endif
		m_pabHasPolicy[iI] = false;
		m_pabHasOneShotPolicyFired[iI] = false;
		m_pabHaveOneShotFreeUnitsFired[iI] = false;
		m_paePolicyBlockedBranchCheck[iI] = (PolicyBranchTypes)-2;
	}

	for(iI = 0; iI < m_pPolicies->GetNumPolicyBranches(); iI++)
	{
		m_pabPolicyBranchUnlocked[iI] = false;
		m_pabPolicyBranchBlocked[iI] = false;
		m_pabPolicyBranchFinished[iI] = false;
		m_paePolicyBranchesChosen[iI] = NO_POLICY_BRANCH_TYPE;
	}

	m_iNumExtraBranches = 0;

	m_eBranchPicked1 = NO_POLICY_BRANCH_TYPE;
	m_eBranchPicked2 = NO_POLICY_BRANCH_TYPE;
	m_eBranchPicked3 = NO_POLICY_BRANCH_TYPE;

	// Reset AI too
	m_pPolicyAI->Reset();


	CvAssert( m_pPolicies->GetNumPolicies() == m_pPolicies->GetNumPolicies());
	//  Pre-calculate a policy to branch table so we don't do this over and over again.
	for (iI = 0; iI < m_pPolicies->GetNumPolicies(); ++iI)
	{
		PolicyTypes eType = (PolicyTypes) iI;
		CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(eType);
		if(pkPolicyInfo == NULL)
		{
			m_paePolicyBlockedBranchCheck[eType] = NO_POLICY_BRANCH_TYPE;
		}
		else
		{
			// What is our branch?
			PolicyBranchTypes eBranch = (PolicyBranchTypes) pkPolicyInfo->GetPolicyBranchType();

			// Are we a free branch policy?
			if(eBranch == NO_POLICY_BRANCH_TYPE)
			{
				int iNumPolicyBranches = m_pPolicies->GetNumPolicyBranches();
				for(int iBranchLoop = 0; iBranchLoop < iNumPolicyBranches; iBranchLoop++)
				{
					const PolicyBranchTypes eLoopBranch = static_cast<PolicyBranchTypes>(iBranchLoop);
					CvPolicyBranchEntry* pkLoopPolicyBranch = GC.getPolicyBranchInfo(eLoopBranch);
					if(pkLoopPolicyBranch)
					{
						// Yes, it's a freebie
						if(pkLoopPolicyBranch->GetFreePolicy() == eType)
						{
							eBranch = eLoopBranch;
							break;
						}
					}
				}
			}

			m_paePolicyBlockedBranchCheck[eType] = eBranch;
		}
	}
}

/// Serialization read
void CvPlayerPolicies::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	CvAssertMsg(m_pPolicies != NULL && m_pPolicies->GetNumPolicies() > 0, "Number of policies to serialize is expected to greater than 0");

	uint uiPolicyCount = 0;
	uint uiPolicyBranchCount = 0;
	if(m_pPolicies)
	{
		uiPolicyCount = m_pPolicies->GetNumPolicies();
		uiPolicyBranchCount = m_pPolicies->GetNumPolicyBranches();
	}

#if defined(MOD_API_EXTENSIONS)
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabFreePolicy, uiPolicyCount);
#endif
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabHasPolicy, uiPolicyCount);
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabHasOneShotPolicyFired, uiPolicyCount);
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabHaveOneShotFreeUnitsFired, uiPolicyCount);

	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabPolicyBranchUnlocked, uiPolicyBranchCount);
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabPolicyBranchBlocked, uiPolicyBranchCount);
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabPolicyBranchFinished, uiPolicyBranchCount);
	CvInfosSerializationHelper::ReadHashedTypeArray(kStream, m_paePolicyBranchesChosen, uiPolicyBranchCount);

	kStream >> m_iNumExtraBranches;

	m_eBranchPicked1 = (PolicyBranchTypes)CvInfosSerializationHelper::ReadHashed(kStream);
	m_eBranchPicked2 = (PolicyBranchTypes)CvInfosSerializationHelper::ReadHashed(kStream);
	m_eBranchPicked3 = (PolicyBranchTypes)CvInfosSerializationHelper::ReadHashed(kStream);

	if (uiVersion < 2)
	{
		int temp;
		kStream >> temp;  // m_iMaxEffectiveCities moved to player class
	}

	// Now for AI
	m_pPolicyAI->Read(kStream);

	CvAssertMsg(m_piLatestFlavorValues != NULL && GC.getNumFlavorTypes() > 0, "Number of flavor values to serialize is expected to greater than 0");

	int iNumFlavors;
	kStream >> iNumFlavors;

	ArrayWrapper<int> wrapm_piLatestFlavorValues(iNumFlavors, m_piLatestFlavorValues);
	kStream >> wrapm_piLatestFlavorValues;

#if defined(MOD_BALANCE_CORE)
	UpdateModifierCache();
#endif
}

/// Serialization write
void CvPlayerPolicies::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 2;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	CvAssertMsg(m_pPolicies != NULL && GC.getNumPolicyInfos() > 0, "Number of policies to serialize is expected to greater than 0");

	uint uiPolicyCount = 0;
	uint uiPolicyBranchCount = 0;
	if(m_pPolicies)
	{
		uiPolicyCount = m_pPolicies->GetNumPolicies();
		uiPolicyBranchCount = m_pPolicies->GetNumPolicyBranches();
	}

#if defined(MOD_API_EXTENSIONS)
	CvInfosSerializationHelper::WriteHashedDataArray<PolicyTypes>(kStream, m_pabFreePolicy, uiPolicyCount);
#endif
	CvInfosSerializationHelper::WriteHashedDataArray<PolicyTypes>(kStream, m_pabHasPolicy, uiPolicyCount);
	CvInfosSerializationHelper::WriteHashedDataArray<PolicyTypes>(kStream, m_pabHasOneShotPolicyFired, uiPolicyCount);
	CvInfosSerializationHelper::WriteHashedDataArray<PolicyTypes>(kStream, m_pabHaveOneShotFreeUnitsFired, uiPolicyCount);
	CvInfosSerializationHelper::WriteHashedDataArray<PolicyBranchTypes>(kStream, m_pabPolicyBranchUnlocked, uiPolicyBranchCount);
	CvInfosSerializationHelper::WriteHashedDataArray<PolicyBranchTypes>(kStream, m_pabPolicyBranchBlocked, uiPolicyBranchCount);
	CvInfosSerializationHelper::WriteHashedDataArray<PolicyBranchTypes>(kStream, m_pabPolicyBranchFinished, uiPolicyBranchCount);
	CvInfosSerializationHelper::WriteHashedTypeArray<PolicyBranchTypes>(kStream, m_paePolicyBranchesChosen, uiPolicyBranchCount);

	kStream << m_iNumExtraBranches;

	CvInfosSerializationHelper::WriteHashed(kStream, m_eBranchPicked1);
	CvInfosSerializationHelper::WriteHashed(kStream, m_eBranchPicked2);
	CvInfosSerializationHelper::WriteHashed(kStream, m_eBranchPicked3);

	// Now for AI
	m_pPolicyAI->Write(kStream);

	CvAssertMsg(m_piLatestFlavorValues != NULL && GC.getNumFlavorTypes() > 0, "Number of flavor values to serialize is expected to greater than 0");
	kStream << GC.getNumFlavorTypes();
	kStream << ArrayWrapper<int>(GC.getNumFlavorTypes(), m_piLatestFlavorValues);
}

/// Respond to a new set of flavor values
void CvPlayerPolicies::FlavorUpdate()
{
	AddFlavorAsStrategies(GC.getPOLICY_WEIGHT_PROPAGATION_PERCENT());
}

/// Accessor: Player object
CvPlayer* CvPlayerPolicies::GetPlayer()
{
	return m_pPlayer;
}

#if defined(MOD_BALANCE_CORE)
//could be extended for other modifiers as well ...
void CvPlayerPolicies::UpdateModifierCache()
{
	m_vBuildingClassTourismModifier.clear();
	m_vBuildingClassTourismModifier.resize(GC.getNumBuildingClassInfos(), 0);
	m_vBuildingClassHappinessModifier.clear();
	m_vBuildingClassHappinessModifier.resize(GC.getNumBuildingClassInfos(), 0);

	//reset this as well - new values will be set directly in GetNumericModifier()
	mModifierLookup.clear();

	for (int j = 0; j < GC.getNumBuildingClassInfos(); j++)
	{
		int iTourism = 0;
		int iHappiness = 0;
		for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
		{
			// Do we have this policy?
			if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
			{
				iTourism += m_pPolicies->GetPolicyEntry(i)->GetBuildingClassTourismModifier((BuildingClassTypes)j);
				iHappiness += m_pPolicies->GetPolicyEntry(i)->GetBuildingClassHappiness((BuildingClassTypes)j);
			}
		}

		m_vBuildingClassTourismModifier[j] = iTourism;
		m_vBuildingClassHappinessModifier[j] = iHappiness;
	}
}
#endif


/// Accessor: does a player have a policy
bool CvPlayerPolicies::HasPolicy(PolicyTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumPolicyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabHasPolicy[eIndex];
}
#if defined(MOD_API_EXTENSIONS)
/// Accessor: was this policy given for free
bool CvPlayerPolicies::IsFreePolicy(PolicyTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumPolicyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabFreePolicy[eIndex];
}
#endif
/// Accessor: set whether player has a policy
#if defined(MOD_API_EXTENSIONS)
void CvPlayerPolicies::SetPolicy(PolicyTypes eIndex, bool bNewValue, bool bFree)
#else
void CvPlayerPolicies::SetPolicy(PolicyTypes eIndex, bool bNewValue)
#endif
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < m_pPolicies->GetNumPolicies(), "eIndex is expected to be within maximum bounds (invalid Index)");

	CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(eIndex);
	if(pkPolicyInfo == NULL)
		return;

	if(HasPolicy(eIndex) != bNewValue)
	{
		m_pabHasPolicy[eIndex] = bNewValue;

		int iChange = bNewValue ? 1 : -1;
#if defined(MOD_API_EXTENSIONS)
		m_pabFreePolicy[eIndex] = bFree;
		if (bFree) iChange = 0;
#endif
		GetPlayer()->ChangeNumPolicies(iChange);

#if defined(MOD_BALANCE_CORE)
		UpdateModifierCache();
#endif

		if(bNewValue)
		{
			DoNewPolicyPickedForHistory(eIndex);

			if(m_pPlayer->GetID() == GC.getGame().getActivePlayer())
				GC.GetEngineUserInterface()->SetPolicyNotificationSeen(false);
		}

		PolicyBranchTypes eThisBranch = (PolicyBranchTypes) pkPolicyInfo->GetPolicyBranchType();

		if(eThisBranch != NO_POLICY_BRANCH_TYPE)
		{
			bool bBranchFinished;

			// We don't have this Policy, so this branch is definitely not finished
			if(!bNewValue)
			{
				bBranchFinished = false;
			}
			// We now have this Policy, so we MAY have this branch finished
			else
			{
				bBranchFinished = true;

				// Is the branch this policy is in finished?
				for(int iPolicyLoop = 0; iPolicyLoop < GetPolicies()->GetNumPolicies(); iPolicyLoop++)
				{
					const PolicyTypes eLoopPolicy = static_cast<PolicyTypes>(iPolicyLoop);

					CvPolicyEntry* pkLoopPolicyInfo = GC.getPolicyInfo(eLoopPolicy);
					if(pkLoopPolicyInfo)
					{
						// This policy belongs to our branch
						if(pkLoopPolicyInfo->GetPolicyBranchType() == eThisBranch)
						{
							// We don't have this policy!
							if(!HasPolicy(eLoopPolicy))
							{
								bBranchFinished = false;

								// No need to continue, we already know we don't have the branch
								break;
							}
						}
					}
				}
			}

			SetPolicyBranchFinished(eThisBranch, bBranchFinished);

			if(bBranchFinished)
			{
				CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(eThisBranch);
				if(pkPolicyBranchInfo)
				{
					PolicyTypes eFinisher = (PolicyTypes)pkPolicyBranchInfo->GetFreeFinishingPolicy();
					if(eFinisher != NO_POLICY)
					{
						GetPlayer()->setHasPolicy(eFinisher, true);
						GetPlayer()->ChangeNumFreePoliciesEver(1);
#if defined(MOD_BALANCE_CORE)
						if(GetPlayer()->GetPlayerTraits()->IsAdoptionFreeTech())
						{
							if (!GetPlayer()->isHuman())
							{
								GetPlayer()->AI_chooseFreeTech();
							}
							else
							{
								CvString strBuffer = GetLocalizedText("TXT_KEY_MISC_COMPLETED_POLICY_UA_CHOOSE_TECH");
								GetPlayer()->chooseTech(1, strBuffer.GetCString());
							}
						}
#endif
					}
				}
			}
		}
	}
}

/// Accessor: is a one-shot policy spent?
bool CvPlayerPolicies::HasOneShotPolicyFired(PolicyTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumPolicyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabHasOneShotPolicyFired[eIndex];
}

/// mark a one shot policy as spent
void CvPlayerPolicies::SetOneShotPolicyFired(PolicyTypes eIndex, bool bFired)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumPolicyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_pabHasOneShotPolicyFired[eIndex] = bFired;
}

bool CvPlayerPolicies::HaveOneShotFreeUnitsFired(PolicyTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumPolicyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabHaveOneShotFreeUnitsFired[eIndex];
}

/// mark a one shot policy as spent
void CvPlayerPolicies::SetOneShotFreeUnitsFired(PolicyTypes eIndex, bool bFired)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumPolicyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_pabHaveOneShotFreeUnitsFired[eIndex] = bFired;
}

/// Returns number of policies purchased by this player
#if defined(MOD_BALANCE_CORE)
int CvPlayerPolicies::GetNumPoliciesOwned(bool bSkipFinisher, bool bExcludeFree) const
#else
int CvPlayerPolicies::GetNumPoliciesOwned() const
#endif
{
	int rtnValue = 0;

	for(int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{
		// Do we have this policy?
		if(m_pabHasPolicy[i])
		{
#if defined(MOD_API_EXTENSIONS)
			if (bExcludeFree && m_pabFreePolicy[i]) continue;
#endif
#if defined(MOD_BALANCE_CORE)
			if(m_pPolicies->GetPolicyEntry(i)->IsDummy())
				continue;

			//Skipping finishers?
			if(bSkipFinisher && m_pPolicies->GetPolicyEntry(i)->IsFinisher())
			{
				continue;
			}
#endif
			rtnValue++;
		}
	}

	return rtnValue;
}
/// Number of policies purchased in this branch
int CvPlayerPolicies::GetNumPoliciesOwnedInBranch(PolicyBranchTypes eBranch) const
{
	int rtnValue = 0;

	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{
		// Do we have this policy?
		if (m_pabHasPolicy[i] && m_pPolicies->GetPolicyEntry(i)->GetPolicyBranchType() == eBranch)
		{
			rtnValue++;
		}
	}

	return rtnValue;
}

/// Number of policies purchased in this branch - used for LUA display
int CvPlayerPolicies::GetNumPoliciesOwnedInBranchForDisplay(PolicyBranchTypes eBranch) const
{
	int rtnValue = 0;

	CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(eBranch);
	if (pkPolicyBranchInfo == NULL)
		return 0;

	// If not an ideology, count the opener.
	if (!pkPolicyBranchInfo->IsPurchaseByLevel())
	{
		rtnValue = (m_pabPolicyBranchUnlocked[eBranch]) ? 1 : 0;
	}

	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{
		// Do we have this policy? Exclude finishers.
		if (m_pabHasPolicy[i] && !m_pPolicies->GetPolicyEntry(i)->IsFinisher() && m_pPolicies->GetPolicyEntry(i)->GetPolicyBranchType() == eBranch)
		{
			rtnValue++;
		}
	}

	return rtnValue;
}

/// Return the policy data (from XML)
CvPolicyXMLEntries* CvPlayerPolicies::GetPolicies() const
{
	return m_pPolicies;
}

/// Get numeric modifier by adding up its value from all purchased policies
int CvPlayerPolicies::GetNumericModifier(PolicyModifierType eType)
{
	int rtnValue = 0;

	//simply memoization for repeated calls
	ModifierMap::iterator it = mModifierLookup.find(eType);
	if (it!=mModifierLookup.end() && it->second.first==GC.getGame().getGameTurn())
		return it->second.second;

	int iNumPolicies = m_pPolicies->GetNumPolicies();
	for(int i = 0; i < iNumPolicies; i++)
	{
		// Do we have this policy?
		if(m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			// Yes, so add it to our counts
			switch(eType)
			{
			case POLICYMOD_EXTRA_HAPPINESS:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetExtraHappiness();
				break;
			case POLICYMOD_EXTRA_HAPPINESS_PER_CITY:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetExtraHappinessPerCity();
				break;
#if defined(HH_MOD_NATURAL_WONDER_MODULARITY)
			case POLICYMOD_EXTRA_NATURALWONDER_HAPPINESS:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetExtraNaturalWonderHappiness();
				break;
#endif
			case POLICYMOD_GREAT_PERSON_RATE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGreatPeopleRateModifier();
				break;
			case POLICYMOD_GREAT_GENERAL_RATE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGreatGeneralRateModifier();
				break;
			case POLICYMOD_GREAT_ADMIRAL_RATE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGreatAdmiralRateModifier();
				break;
			case POLICYMOD_GREAT_WRITER_RATE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGreatWriterRateModifier();
				break;
			case POLICYMOD_GREAT_ARTIST_RATE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGreatArtistRateModifier();
				break;
			case POLICYMOD_GREAT_MUSICIAN_RATE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGreatMusicianRateModifier();
				break;
			case POLICYMOD_GREAT_MERCHANT_RATE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGreatMerchantRateModifier();
				break;
#if defined(MOD_BALANCE_CORE)
			case POLICYMOD_GREAT_ENGINEER_RATE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGreatEngineerRateModifier();
				break;
			case POLICYMOD_STEAL_GW_SLOWER_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetStealGWSlowerModifier();
				break;
			case POLICYMOD_STEAL_GW_FASTER_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetStealGWFasterModifier();
				break;
			case POLICYMOD_CITY_DEFENSE_BOOST:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetDefenseBoost();
				break;
#endif
#if defined(MOD_DIPLOMACY_CITYSTATES)
			case POLICYMOD_GREAT_DIPLOMAT_RATE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGreatDiplomatRateModifier();
				break;
#endif
			case POLICYMOD_GREAT_SCIENTIST_RATE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGreatScientistRateModifier();
				break;
			case POLICYMOD_DOMESTIC_GREAT_GENERAL_RATE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetDomesticGreatGeneralRateModifier();
				break;
			case POLICYMOD_POLICY_COST_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetPolicyCostModifier();
				break;
			case POLICYMOD_WONDER_PRODUCTION_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetWonderProductionModifier();
				break;
			case POLICYMOD_BUILDING_PRODUCTION_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetBuildingProductionModifier();
				break;
			case POLICYMOD_FREE_EXPERIENCE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetFreeExperience();
				break;
			case POLICYMOD_EXTRA_CULTURE_FROM_IMPROVEMENTS:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetCultureImprovementChange();
				break;
			case POLICYMOD_CULTURE_FROM_KILLS:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetCultureFromKills();
				break;
			case POLICYMOD_EMBARKED_EXTRA_MOVES:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetEmbarkedExtraMoves();
				break;
			case POLICYMOD_CULTURE_FROM_BARBARIAN_KILLS:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetCultureFromBarbarianKills();
				break;
			case POLICYMOD_GOLD_FROM_KILLS:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGoldFromKills();
				break;
			case POLICYMOD_CULTURE_FROM_GARRISON:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetCulturePerGarrisonedUnit();
				break;
			case POLICYMOD_UNIT_FREQUENCY_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetCityStateUnitFrequencyModifier();
				break;
			case POLICYMOD_TOURISM_MOD_COMMON_FOE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetCommonFoeTourismModifier();
				break;
			case POLICYMOD_TOURISM_MOD_LESS_HAPPY:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetLessHappyTourismModifier();
				break;
			case POLICYMOD_TOURISM_MOD_SHARED_IDEOLOGY:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetSharedIdeologyTourismModifier();
				break;
			case POLICYMOD_TRADE_MISSION_GOLD_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetTradeMissionGoldModifier();
				break;
			case POLICYMOD_FAITH_COST_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetFaithCostModifier();
				break;
			case POLICYMOD_CULTURAL_PLUNDER_MULTIPLIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetCulturalPlunderMultiplier();
				break;
			case POLICYMOD_STEAL_TECH_SLOWER_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetStealTechSlowerModifier();
				break;
			case POLICYMOD_STEAL_TECH_FASTER_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetStealTechFasterModifier();
				break;
			case POLICYMOD_CATCH_SPIES_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetCatchSpiesModifier();
				break;
			case POLICYMOD_BUILDING_PURCHASE_COST_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetBuildingPurchaseCostModifier();
				break;
			case POLICYMOD_LAND_TRADE_GOLD_CHANGE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetLandTradeRouteGoldChange();
				break;
			case POLICYMOD_SEA_TRADE_GOLD_CHANGE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetSeaTradeRouteGoldChange();
				break;
			case POLICYMOD_SHARED_IDEOLOGY_TRADE_CHANGE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetSharedIdeologyTradeGoldChange();
				break;
			case POLICYMOD_RIGGING_ELECTION_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetRiggingElectionModifier();
				break;
			case POLICYMOD_MILITARY_UNIT_GIFT_INFLUENCE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetMilitaryUnitGiftExtraInfluence();
				break;
			case POLICYMOD_PROTECTED_MINOR_INFLUENCE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetProtectedMinorPerTurnInfluence();
				break;
			case POLICYMOD_AFRAID_INFLUENCE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetAfraidMinorPerTurnInfluence();
				break;
			case POLICYMOD_MINOR_BULLY_SCORE_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetMinorBullyScoreModifier();
				break;
			case POLICYMOD_THEMING_BONUS:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetThemingBonusMultiplier();
				break;
			case POLICYMOD_CITY_STATE_TRADE_CHANGE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetCityStateTradeChange();
				break;
			case POLICYMOD_INTERNAL_TRADE_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetInternalTradeRouteYieldModifier();
				break;
#if defined(MOD_BALANCE_CORE)
			case POLICYMOD_INTERNAL_TRADE_CAPITAL_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetInternalTradeRouteYieldModifierCapital();
				break;
			case POLICYMOD_TRADE_CAPITAL_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetTradeRouteYieldModifierCapital();
				break;

			case POLICYMOD_TRADE_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetTradeRouteYieldModifier();
				break;
#endif
			case POLICYMOD_SHARED_RELIGION_TOURISM_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetSharedReligionTourismModifier();
				break;
			case POLICYMOD_TRADE_ROUTE_TOURISM_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetTradeRouteTourismModifier();
				break;
			case POLICYMOD_OPEN_BORDERS_TOURISM_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetOpenBordersTourismModifier();
#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
			case POLICYMOD_CONVERSION_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetConversionModifier();
				break;
#endif
			}
		}
	}

	//remember the value for next time
	mModifierLookup[eType] = std::make_pair(GC.getGame().getGameTurn(),rtnValue);

	return rtnValue;
}
#if defined(MOD_BALANCE_CORE)
void CvPlayerPolicies::ClearCache()
{
	mModifierLookup.clear();
}
#endif

/// Get overall modifier from policies for a type of yield
int CvPlayerPolicies::GetYieldModifier(YieldTypes eYieldType)
{
	int rtnValue = 0;

	for(int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{
		// Do we have this policy?
		if(m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			rtnValue += m_pPolicies->GetPolicyEntry(i)->GetYieldModifier(eYieldType);
		}
	}

	return rtnValue;
}
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
int CvPlayerPolicies::GetInternationalRouteYieldModifier(YieldTypes eYield)
{
	int totalModifiersFromPolicies = 0;
	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{
		if(m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			totalModifiersFromPolicies += m_pPolicies->GetPolicyEntry(i)->GetInternationalRouteYieldModifier(eYield);
		}
	}
	return totalModifiersFromPolicies;
}
#endif

/// Get yield modifier from policies for a specific building class
int CvPlayerPolicies::GetBuildingClassYieldModifier(BuildingClassTypes eBuildingClass, YieldTypes eYieldType)
{
	int rtnValue = 0;

	for(int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{
		// Do we have this policy?
		if(m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			rtnValue += m_pPolicies->GetPolicyEntry(i)->GetBuildingClassYieldModifiers(eBuildingClass, eYieldType);
		}
	}

	return rtnValue;
}

/// Get yield change from policies for a specific building class
int CvPlayerPolicies::GetBuildingClassYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYieldType)
{
	int rtnValue = 0;

	for(int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{
		// Do we have this policy?
		if(m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			rtnValue += m_pPolicies->GetPolicyEntry(i)->GetBuildingClassYieldChanges(eBuildingClass, eYieldType);
		}
	}

	return rtnValue;
}
#if defined(MOD_BALANCE_CORE_POLICIES)
/// Get yield modifier from policies for a specific building class if your religion is present
int CvPlayerPolicies::GetReligionBuildingClassYieldModifier(BuildingClassTypes eBuildingClass, YieldTypes eYieldType)
{
	int rtnValue = 0;

	for(int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{
		// Do we have this policy?
		if(m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			rtnValue += m_pPolicies->GetPolicyEntry(i)->GetReligionBuildingYieldMod(eBuildingClass, eYieldType);
		}
	}

	return rtnValue;
}
#endif
/// Get culture change from policies for a specific improvement
int CvPlayerPolicies::GetImprovementCultureChange(ImprovementTypes eImprovement)
{
	int rtnValue = 0;

	for(int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{
		// Do we have this policy?
		if(m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			rtnValue += m_pPolicies->GetPolicyEntry(i)->GetImprovementCultureChanges(eImprovement);
		}
	}

	return rtnValue;
}

/// Get production modifier from policies for a specific building class
int CvPlayerPolicies::GetBuildingClassProductionModifier(BuildingClassTypes eBuildingClass)
{
	int rtnValue = 0;

	for(int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{
		// Do we have this policy?
		if(m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			rtnValue += m_pPolicies->GetPolicyEntry(i)->GetBuildingClassProductionModifier(eBuildingClass);
		}
	}

	return rtnValue;
}

/// Get tourism modifier from policies for a specific building class
int CvPlayerPolicies::GetBuildingClassTourismModifier(BuildingClassTypes eBuildingClass)
{

	if (eBuildingClass > NO_BUILDINGCLASS && eBuildingClass < (int)m_vBuildingClassTourismModifier.size())
		return m_vBuildingClassTourismModifier[eBuildingClass];
	else
		return 0;
}

/// Get happiness modifier from policies for a specific building class
int CvPlayerPolicies::GetBuildingClassHappinessModifier(BuildingClassTypes eBuildingClass)
{

	if (eBuildingClass > NO_BUILDINGCLASS && eBuildingClass < (int)m_vBuildingClassHappinessModifier.size())
		return m_vBuildingClassHappinessModifier[eBuildingClass];
	else
		return 0;
}


/// Does any policy owned give benefit for garrisons?
bool CvPlayerPolicies::HasPolicyEncouragingGarrisons() const
{
	for(int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{
		// Do we have this policy?
		if(m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			CvPolicyEntry* pPolicy = m_pPolicies->GetPolicyEntry(i);
			if(pPolicy->GetGarrisonedCityRangeStrikeModifier() > 0)
			{
				return true;
			}
			else if(pPolicy->GetCulturePerGarrisonedUnit() > 0)
			{
				return true;
			}
			else if(pPolicy->GetHappinessPerGarrisonedUnit() > 0)
			{
				return true;
			}
			else if(pPolicy->IsGarrisonFreeMaintenance())
			{
				return true;
			}
		}
	}

	return false;
}

/// Does any policy owned give a Reformation belief?
bool CvPlayerPolicies::HasPolicyGrantingReformationBelief() const
{
	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{
		// Do we have this policy?
		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			CvPolicyEntry *pPolicy = m_pPolicies->GetPolicyEntry(i);
			if (pPolicy->IsAddReformationBelief())
			{
				return true;
			}
		}
	}

	return false;
}

/// Returns the proper ruler name for "We love the XXX day"
CvString CvPlayerPolicies::GetWeLoveTheKingString()
{
	CvString rtnValue;

	// Policies are arranged from least to most advanced in XML
	//   So loop from back to front until we find a string
	for(int i = m_pPolicies->GetNumPolicies() - 1; i >= 0; i--)
	{
		// Do we have this policy?
		if(m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			// Does it have a string for us?
			CvString str = m_pPolicies->GetPolicyEntry(i)->GetWeLoveTheKing();
			if(str.length() > 0)
			{
				rtnValue = str;
				break;  // All done when find the first one
			}
		}
	}

	return rtnValue;
}

/// List of buildings we get in conquered cities
std::vector<BuildingTypes> CvPlayerPolicies::GetFreeBuildingsOnConquest()
{
	std::vector<BuildingTypes> freeBuildings;
	freeBuildings.reserve(m_pPolicies->GetNumPolicies());

	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{
		// Do we have this policy?
		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			CvPolicyEntry *pPolicy = m_pPolicies->GetPolicyEntry(i);
			BuildingTypes eFreeBuilding = pPolicy->GetFreeBuildingOnConquest();
			if (eFreeBuilding)
			{
				freeBuildings.push_back(eFreeBuilding);
			}
		}
	}
	return freeBuildings;
}

/// How much free tourism is created when we create a unit?
int CvPlayerPolicies::GetTourismFromUnitCreation(UnitClassTypes eUnitClass) const
{
	int iTourism = 0;

	for (int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{
		// Do we have this policy?
		if (m_pabHasPolicy[i] && !IsPolicyBlocked((PolicyTypes)i))
		{
			CvPolicyEntry *pPolicy = m_pPolicies->GetPolicyEntry(i);
			if (pPolicy->GetTourismByUnitClassCreated(eUnitClass) > 0)
			{
				iTourism += pPolicy->GetTourismByUnitClassCreated(eUnitClass);
			}
		}
	}

	return iTourism;
}

/// How much will the next policy cost?
int CvPlayerPolicies::GetNextPolicyCost()
{
#if defined(MOD_BUGFIX_DUMMY_POLICIES)
#if defined(MOD_API_EXTENSIONS)
	int iNumPolicies = GetNumPoliciesOwned(MOD_BUGFIX_DUMMY_POLICIES, true);
#else
	int iNumPolicies = GetNumPoliciesOwned(MOD_BUGFIX_DUMMY_POLICIES);
#endif
#else
#if defined(MOD_API_EXTENSIONS)
	int iNumPolicies = GetNumPoliciesOwned(false, true);
#else
	int iNumPolicies = GetNumPoliciesOwned();
#endif
#endif

	// Reduce count by however many free Policies we've had in this game
	iNumPolicies -= (m_pPlayer->GetNumFreePoliciesEver() - m_pPlayer->GetNumFreePolicies() - m_pPlayer->GetNumFreeTenets());

	// Each branch we unlock (after the first) costs us a buy, so add that in; JON: not any more
	//if (GetNumPolicyBranchesUnlocked() > 0)
	//{
	//	iNumPolicies += (GetNumPolicyBranchesUnlocked() - 1);
	//}

	int iCost = 0;
	iCost += (int)(iNumPolicies* (/*7*/ GC.getPOLICY_COST_INCREASE_TO_BE_EXPONENTED() + GC.getPOLICY_COST_EXTRA_VALUE()));

	// Exponential cost scaling
	iCost = (int)pow((double)iCost, (double) /*1.70*/ GC.getPOLICY_COST_EXPONENT());

	// Base cost that doesn't get exponent-ed
	iCost += /*25*/ GC.getBASE_POLICY_COST();

	// Mod for City Count
	int iMod = GC.getMap().getWorldInfo().GetNumCitiesPolicyCostMod();	// Default is 40, gets smaller on larger maps
	int iPolicyModDiscount = m_pPlayer->GetNumCitiesPolicyCostDiscount();
	if(iPolicyModDiscount != 0)
	{
		iMod = iMod * (100 + iPolicyModDiscount);
		iMod /= 100;
	}

	int iNumCities = m_pPlayer->GetMaxEffectiveCities();

	iMod = (iCost * (iNumCities - 1) * iMod);
	iMod /= 100;
	iCost += iMod;

	// Policy Cost Mod
	iCost *= (100 + m_pPlayer->getPolicyCostModifier());
	iCost /= 100;

	// Game Speed Mod
	iCost *= GC.getGame().getGameSpeedInfo().getCulturePercent();
	iCost /= 100;

	// Handicap Mod
	iCost *= m_pPlayer->getHandicapInfo().getPolicyPercent();
	iCost /= 100;

#if defined(MOD_BALANCE_CORE_PURCHASE_COST_INCREASE)
	if (MOD_BALANCE_CORE_PURCHASE_COST_INCREASE)
	{
		int iTier1 = 0;
		int iTier2 = 0;
		int iTier3 = 0;
		PolicyBranchTypes eLoopBranch;
		for (int iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
		{
			eLoopBranch = (PolicyBranchTypes)iBranchLoop;

			if (eLoopBranch != NO_POLICY_BRANCH_TYPE)
			{
				CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(eLoopBranch);
				if (pkPolicyBranchInfo && pkPolicyBranchInfo->IsPurchaseByLevel())
				{
					iTier1 += m_pPlayer->GetPlayerPolicies()->GetNumTenetsOfLevel(eLoopBranch, 1);
					iTier2 += m_pPlayer->GetPlayerPolicies()->GetNumTenetsOfLevel(eLoopBranch, 2);
					iTier3 += m_pPlayer->GetPlayerPolicies()->GetNumTenetsOfLevel(eLoopBranch, 3);
				}
			}
		}

		//% cost increases.
		iTier1 *= (int)(GC.getPOLICY_COST_EXPONENT());
		iTier2 *= (int)(GC.getPOLICY_COST_EXPONENT() * 2);
		iTier3 *= (int)(GC.getPOLICY_COST_EXPONENT() * 3);
			
		iCost *= (100 + iTier1 + iTier2 + iTier3);
		iCost /= 100;
	}
#endif

	// Make the number nice and even
	int iDivisor = /*5*/ GC.getPOLICY_COST_VISIBLE_DIVISOR();
	iCost /= iDivisor;
	iCost *= iDivisor;

	return iCost;
}

/// Can we adopt this policy?
bool CvPlayerPolicies::CanAdoptPolicy(PolicyTypes eIndex, bool bIgnoreCost) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumPolicyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	CvPolicyEntry* pkPolicyEntry = GC.getPolicyInfo(eIndex);
	if(pkPolicyEntry == NULL)
		return false;

	// Already has Policy?
	if(HasPolicy(eIndex))
	{
		return false;
	}

	// Has enough culture to spend?
	if((!bIgnoreCost) && m_pPlayer->getNextPolicyCost() > 0)
	{
		if(m_pPlayer->getJONSCulture() < m_pPlayer->getNextPolicyCost())
		{
			bool bTenet = pkPolicyEntry->GetLevel() > 0;
			if (m_pPlayer->GetNumFreePolicies() == 0)
			{
				if (!bTenet || m_pPlayer->GetNumFreeTenets() == 0)
				{
					return false;
				}
			}
		}
	}

	PolicyBranchTypes eBranch = (PolicyBranchTypes) pkPolicyEntry->GetPolicyBranchType();

	// If it doesn't have a branch, it's a freebie that comes WITH the branch, so we can't pick it manually
	if(eBranch == NO_POLICY_BRANCH_TYPE)
	{
		return false;
	}

	if(!IsPolicyBranchUnlocked(eBranch))
	{
		return false;
	}

	// Is it from a branch with Levels?
	CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(eBranch);
	if(pkPolicyBranchInfo == NULL)
	{
		return false;
	}
	else
	{
		if (pkPolicyBranchInfo->IsPurchaseByLevel())
		{
			// If below level 1, can't have as many of this level as of the previous one
			int iLevel = pkPolicyEntry->GetLevel();
			if (iLevel > 1)
			{
				int iPoliciesOfThisLevel = GetNumTenetsOfLevel(eBranch, iLevel) + 1 /* For the policy we're adding here */;
				int iPoliciesOfPreviousLevel = GetNumTenetsOfLevel(eBranch, iLevel - 1);
				if (iPoliciesOfThisLevel >= iPoliciesOfPreviousLevel)
				{
					return false;
				}
			}
		}
	}

	// Other Policies as Prereqs

	bool bFoundPossible = false;
	bool bFoundValid = false;

	for(int iI = 0; iI < GC.getNUM_OR_TECH_PREREQS(); iI++)
	{
		PolicyTypes ePrereq = (PolicyTypes)pkPolicyEntry->GetPrereqOrPolicies(iI);
		if(ePrereq != NO_POLICY)
		{
			bFoundPossible = true;

			if(HasPolicy(ePrereq))
			{
				bFoundValid = true;
				break;
			}
		}
	}

	if(bFoundPossible && !bFoundValid)
	{
		return false;
	}

	for(int iI = 0; iI < GC.getNUM_AND_TECH_PREREQS(); iI++)
	{
		const PolicyTypes ePrereq = static_cast<PolicyTypes>(pkPolicyEntry->GetPrereqAndPolicies(iI));

		if(ePrereq == NO_POLICY)
			continue;

		CvPolicyEntry* pkPrereqPolicyInfo = GC.getPolicyInfo(ePrereq);
		if(pkPrereqPolicyInfo)
		{
			if(!HasPolicy(ePrereq))
			{
				return false;
			}
		}
	}

	// Disabled by another Policy?
	for(int iPolicyLoop = 0; iPolicyLoop < GetPolicies()->GetNumPolicies(); iPolicyLoop++)
	{
		const PolicyTypes eDisablePolicy =static_cast<PolicyTypes>(iPolicyLoop);

		CvPolicyEntry* pkDisablePolicyInfo = GC.getPolicyInfo(eDisablePolicy);
		if(pkDisablePolicyInfo)
		{
			if(HasPolicy(eDisablePolicy))
			{
				for(int iI = 0; iI < GC.getNUM_AND_TECH_PREREQS(); iI++)
				{
					if(pkDisablePolicyInfo->GetPolicyDisables(iI) == eIndex)
					{
						return false;
					}
				}
			}
		}
	}

	// Has tech prereq? (no policies have one by default)
	if(pkPolicyEntry->GetTechPrereq() != NO_TECH)
	{
		if(!GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech((TechTypes) pkPolicyEntry->GetTechPrereq()))
		{
			return false;
		}
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pPlayer->GetID());
		args->Push(eIndex);

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "PlayerCanAdoptPolicy", args.get(), bResult))
		{
			// Check the result.
			if(bResult == false)
			{
				return false;
			}
		}
	}
	
#if defined(MOD_EVENTS_IDEOLOGIES)
	if (MOD_EVENTS_IDEOLOGIES && pkPolicyEntry->GetLevel() > 0) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanAdoptTenet, m_pPlayer->GetID(), eIndex) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
#endif

	return true;
}

/// Player gets a new Policy Branch!
void CvPlayerPolicies::DoUnlockPolicyBranch(PolicyBranchTypes eBranchType)
{
	CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(eBranchType);
	if(pkPolicyBranchInfo == NULL)
	{
		return;
	}

	// Can we actually do this?
	if(!CanUnlockPolicyBranch(eBranchType))
	{
		return;
	}

	// Set that we now have it
	SetPolicyBranchUnlocked(eBranchType, true, false);

	// Are we blocked? If so, unblock us
	DoSwitchToPolicyBranch(eBranchType);

	// Free Policy with this Branch?
	PolicyTypes eFreePolicy = (PolicyTypes) pkPolicyBranchInfo->GetFreePolicy();
	if(eFreePolicy != NO_POLICY)
	{
		GetPlayer()->setHasPolicy(eFreePolicy, true);
	}

	// Pay Culture cost - if applicable
	if(GetPlayer()->GetNumFreePolicies() == 0)
	{
		GetPlayer()->changeJONSCulture(-GetPlayer()->getNextPolicyCost());
	}
	else
	{
		GetPlayer()->ChangeNumFreePolicies(-1);
	}

	// Update cost if trying to buy another policy this turn
	GetPlayer()->DoUpdateNextPolicyCost();

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);

	// This Dirty bit must only be set when changing something for the active player
	if(GC.getGame().getActivePlayer() == GetPlayer()->GetID())
	{
		GC.GetEngineUserInterface()->setDirty(Policies_DIRTY_BIT, true);
	}

#if defined(MOD_BALANCE_CORE)
	CvCity* pCapital = m_pPlayer->getCapitalCity();
	int iPolicyGEorGM = m_pPlayer->GetPlayerTraits()->GetPolicyGEorGM();
	if (iPolicyGEorGM > 0 && pCapital != NULL)
	{
		m_pPlayer->doPolicyGEorGM(iPolicyGEorGM);
	}
	m_pPlayer->doInstantYield(INSTANT_YIELD_TYPE_POLICY_UNLOCK, false, NO_GREATPERSON, NO_BUILDING, 0, false);
	m_pPlayer->doInstantGreatPersonProgress(INSTANT_YIELD_TYPE_POLICY_UNLOCK);

	int iLoop;
	for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop)) 
	{
		pLoopCity->GetCityCitizens()->SetDirty(true);
	}
#endif

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pPlayer->GetID());
		args->Push(eBranchType);

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		LuaSupport::CallHook(pkScriptSystem, "PlayerAdoptPolicyBranch", args.get(), bResult);
	}
}

/// can the player unlock eBranchType right now?
bool CvPlayerPolicies::CanUnlockPolicyBranch(PolicyBranchTypes eBranchType)
{
	// Must have enough culture to spend a buy opening a new branch
	if(GetPlayer()->getJONSCulture() < GetPlayer()->getNextPolicyCost())
	{
		if(GetPlayer()->GetNumFreePolicies() == 0)
			return false;
	}

	CvPolicyBranchEntry* pkBranchEntry = m_pPolicies->GetPolicyBranchEntry(eBranchType);
	if(pkBranchEntry)
	{
		// Ideology branches unlocked through a direct call to SetPolicyBranchUnlocked()
		if (pkBranchEntry->IsPurchaseByLevel())
		{
			return false;
		}

		if (pkBranchEntry->IsLockedWithoutReligion())
		{
			if (GC.getGame().isOption(GAMEOPTION_NO_RELIGION))
			{
				return false;
			}
		}

		EraTypes ePrereqEra = (EraTypes) pkBranchEntry->GetEraPrereq();

		// Must be in the proper Era
		if(ePrereqEra != NO_ERA)
		{
#if defined(MOD_BALANCE_CORE_POLICIES)
			//If we've finished a policy branch, unlock the next set.
			// Set Policies in this branch as blocked
			bool bCanUnlockEarly = false;
			for(int iPolicyLoop = 0; iPolicyLoop < GetPolicies()->GetNumPolicies(); iPolicyLoop++)
			{
				const PolicyTypes eLoopPolicy = static_cast<PolicyTypes>(iPolicyLoop);

				CvPolicyEntry* pkLoopPolicyInfo = GC.getPolicyInfo(eLoopPolicy);
				if(pkLoopPolicyInfo)
				{
					if(HasPolicy(eLoopPolicy) && (pkLoopPolicyInfo->GetPolicyEraUnlock() != NO_ERA) && (pkLoopPolicyInfo->GetPolicyEraUnlock() >= ePrereqEra))
					{
						bCanUnlockEarly = true;
					}
				}
			}
			//Using a system of numbers instead? Okay.
			int iNumPolicies = GetPlayer()->GetPlayerPolicies()->GetNumPoliciesOwned(true, true);
			if(iNumPolicies >= pkBranchEntry->GetNumPolicyRequirement())
			{
				bCanUnlockEarly = true;
			}

			if(!bCanUnlockEarly && GET_TEAM(GetPlayer()->getTeam()).GetCurrentEra() < ePrereqEra)
			{
				return false;
			}
#else
			if(GET_TEAM(GetPlayer()->getTeam()).GetCurrentEra() < ePrereqEra)
			{
				return false;
			}
#endif
		}
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pPlayer->GetID());
		args->Push(eBranchType);

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "PlayerCanAdoptPolicyBranch", args.get(), bResult))
		{
			// Check the result.
			if(bResult == false)
			{
				return false;
			}
		}
	}

	return true;
}

/// Accessor: has a player unlocked eBranchType to pick Policies from?
bool CvPlayerPolicies::IsPolicyBranchUnlocked(PolicyBranchTypes eBranchType) const
{
	CvAssertMsg(eBranchType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBranchType < m_pPolicies->GetNumPolicyBranches(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabPolicyBranchUnlocked[eBranchType];
}

/// Accessor: sets that a player has (or hasn't) unlocked eBranchType to pick Policies from
void CvPlayerPolicies::SetPolicyBranchUnlocked(PolicyBranchTypes eBranchType, bool bNewValue, bool bRevolution)
{
	CvAssertMsg(eBranchType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBranchType < m_pPolicies->GetNumPolicyBranches(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(IsPolicyBranchUnlocked(eBranchType) != bNewValue)
	{
		// Unlocked?
		if (bNewValue)
		{
			int iFreePolicies = PolicyHelpers::GetNumFreePolicies(eBranchType);

			// Late-game tree so want to issue notification?
			CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(eBranchType);
			if(pkPolicyBranchInfo != NULL)
			{
				if (pkPolicyBranchInfo->IsPurchaseByLevel())
				{
					if (iFreePolicies > 0 && m_pPlayer->GetCulture()->GetTurnIdeologyAdopted() == -1)
					{
						iFreePolicies += m_pPlayer->GetPlayerTraits()->GetExtraTenetsFirstAdoption();
					}
					m_pPlayer->ChangeNumFreeTenets(iFreePolicies, !bRevolution);
#if defined(MOD_BALANCE_CORE)
					if(!bRevolution)
					{
						GAMEEVENTINVOKE_HOOK(GAMEEVENT_IdeologyAdopted, m_pPlayer->GetID(), eBranchType);
					}
#endif
					for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
						PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
						CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);

						// Issue notification if OTHER than target player
						if (m_pPlayer->GetID() != eNotifyPlayer)
						{
							CvTeam& kNotifyTeam = GET_TEAM(kCurNotifyPlayer.getTeam());
							const bool bHasMet = kNotifyTeam.isHasMet(m_pPlayer->getTeam());

							CvNotifications* pNotifications = kCurNotifyPlayer.GetNotifications();
							if(pNotifications)
							{
								CvString strBuffer;
								if(bHasMet)
								{
									if (bRevolution)
										strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_IDEOLOGY_CHANGE", m_pPlayer->getCivilizationShortDescriptionKey(), pkPolicyBranchInfo->GetDescriptionKey());
									else
										strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_IDEOLOGY_CHOSEN", m_pPlayer->getCivilizationShortDescriptionKey(), pkPolicyBranchInfo->GetDescriptionKey());
								}
								else
								{
									if (bRevolution)
										strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_IDEOLOGY_CHANGE_UNMET", pkPolicyBranchInfo->GetDescriptionKey());
									else
										strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_IDEOLOGY_CHOSEN_UNMET", pkPolicyBranchInfo->GetDescriptionKey());
								}

								CvString strSummary;
								if (bRevolution)
									strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_IDEOLOGY_CHANGE");
								else
									strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_IDEOLOGY_CHOSEN");

								pNotifications->Add(NOTIFICATION_IDEOLOGY_CHOSEN, strBuffer, strSummary, -1, -1, m_pPlayer->GetID());
							}
						}
					}
				}
				else
				{
					m_pPlayer->ChangeNumFreePolicies(iFreePolicies);
				}
			}
		}

		m_pabPolicyBranchUnlocked[eBranchType] = bNewValue;
	}

	//if it's an ideology, remember the turn we first chose one
	if (bNewValue && m_pPlayer->GetCulture()->GetTurnIdeologyAdopted()==-1)
	{
		PolicyBranchTypes eFreedomBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_FREEDOM();
		PolicyBranchTypes eAutocracyBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_AUTOCRACY();
		PolicyBranchTypes eOrderBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_ORDER();
		if (eFreedomBranch == eBranchType || eAutocracyBranch == eBranchType || eOrderBranch == eBranchType)
			m_pPlayer->GetCulture()->SetTurnIdeologyAdopted(GC.getGame().getGameTurn());
	}
}

/// Accessor: how many branches has this player unlocked?
int CvPlayerPolicies::GetNumPolicyBranchesUnlocked() const
{
	int iCount = 0;

	for(int iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
	{
		if(IsPolicyBranchUnlocked((PolicyBranchTypes) iBranchLoop))
		{
			iCount++;
		}
	}

	return iCount;
}
 
/// We're going to be using eBranchType now
void CvPlayerPolicies::DoSwitchToPolicyBranch(PolicyBranchTypes eBranchType)
{
	// Must be unlocked
	if(!IsPolicyBranchUnlocked(eBranchType))
	{
		return;
	}

	// Is this branch blocked?
	if(IsPolicyBranchBlocked(eBranchType))
	{
		// Anarchy time!
		int iNumTurnsAnarchy = /*1*/ GC.getSWITCH_POLICY_BRANCHES_ANARCHY_TURNS();
		GetPlayer()->ChangeAnarchyNumTurns(iNumTurnsAnarchy);
#if defined(MOD_BALANCE_CORE)
		Localization::String strSummary = Localization::Lookup("TXT_KEY_ANARCHY_BEGINS_SUMMARY");
		Localization::String strMessage = Localization::Lookup("TXT_KEY_ANARCHY_BEGINS");
		GetPlayer()->GetNotifications()->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), GetPlayer()->GetID(), GC.getSWITCH_POLICY_BRANCHES_ANARCHY_TURNS(), -1);
#endif

		// Turn off blocking
		SetPolicyBranchBlocked(eBranchType, false);
	}

	// Does THIS Branch block any other branch?
	int iBranchLoop;
	for(iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
	{
		const PolicyBranchTypes eDisableBranch = static_cast<PolicyBranchTypes>(iBranchLoop);
		CvPolicyBranchEntry* pkDisablePolicyBranchInfo = GC.getPolicyBranchInfo(eBranchType);
		if(pkDisablePolicyBranchInfo)
		{
			if(pkDisablePolicyBranchInfo->GetPolicyBranchDisables(eDisableBranch) > 0)
			{
				SetPolicyBranchBlocked(eDisableBranch, true);
			}
		}

	}

//	std::vector<PolicyBranchTypes> veOtherPoliciesToUnblock;
	bool bUnlockBranch;

	// Do a pass over the Policies to see if there are any we can safely unblock
	for(iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
	{
		const PolicyBranchTypes eActivateBranch = static_cast<PolicyBranchTypes>(iBranchLoop);
		CvPolicyBranchEntry* pkActivateBranchInfo = GC.getPolicyBranchInfo(eActivateBranch);
		if(pkActivateBranchInfo)
		{
			// Must be activatable
			if(!IsPolicyBranchUnlocked(eActivateBranch))
			{
				continue;
			}

			// Must be blocked now
			if(!IsPolicyBranchBlocked(eActivateBranch))
			{
				continue;
			}

			// Let's try to unblock this
			bUnlockBranch = true;

			// Loop through all Policies we have and make sure they don't interfere with us
			for(int iOtherBranchLoop = 0; iOtherBranchLoop < m_pPolicies->GetNumPolicyBranches(); iOtherBranchLoop++)
			{
				const PolicyBranchTypes eOtherBranch = static_cast<PolicyBranchTypes>(iOtherBranchLoop);
				CvPolicyBranchEntry* pkOtherPolicyBranchInfo = GC.getPolicyBranchInfo(eOtherBranch);
				if(pkOtherPolicyBranchInfo)
				{
					// Don't test branch against itself
					if(eActivateBranch != eOtherBranch)
					{
						// Is this other branch unlocked and unblocked?
						if(IsPolicyBranchUnlocked(eOtherBranch))
						{
							if(!IsPolicyBranchBlocked(eOtherBranch))
							{
								// Do we block them?
								if(pkActivateBranchInfo->GetPolicyBranchDisables(eOtherBranch) > 0)
								{
									bUnlockBranch = false;
								}
								// Do they block us?
								if(pkOtherPolicyBranchInfo->GetPolicyBranchDisables(eActivateBranch) > 0)
								{
									bUnlockBranch = false;
								}
							}
						}

						// We've identified that eActivateBranch conflicts with something, so leave it be
						if(!bUnlockBranch)
						{
							break;
						}
					}
				}
			}

			// We know that eActivateBranch doesn't affect anything, so unblock it!
			if(bUnlockBranch)
			{
				SetPolicyBranchBlocked(eActivateBranch, false);
			}
		}
	}

	// This Dirty bit must only be set when changing something for the active player
	if(GC.getGame().getActivePlayer() == GetPlayer()->GetID())
	{
		GC.GetEngineUserInterface()->setDirty(Policies_DIRTY_BIT, true);
	}
}

/// Accessor: is eBranchType blocked because of branch choices?
void CvPlayerPolicies::SetPolicyBranchBlocked(PolicyBranchTypes eBranchType, bool bValue)
{
	CvAssertMsg(eBranchType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBranchType < m_pPolicies->GetNumPolicyBranches(), "eIndex is expected to be within maximum bounds (invalid Index)");

	CvPolicyBranchEntry* pkPolicyBranchEntry = GC.getPolicyBranchInfo(eBranchType);
	if(pkPolicyBranchEntry)
	{
		if(bValue != IsPolicyBranchBlocked(eBranchType))
		{
			m_pabPolicyBranchBlocked[eBranchType] = bValue;

#if defined(MOD_BALANCE_CORE)
			UpdateModifierCache();
#endif

			int iPolicyEffectChange = bValue ? -1 : 1;

			if(iPolicyEffectChange != 0)
			{
				// Set Policies in this branch as blocked
				for(int iPolicyLoop = 0; iPolicyLoop < GetPolicies()->GetNumPolicies(); iPolicyLoop++)
				{
					const PolicyTypes ePolicy = static_cast<PolicyTypes>(iPolicyLoop);
					CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(ePolicy);
					if(pkPolicyInfo)
					{
						if(eBranchType == (PolicyBranchTypes) pkPolicyInfo->GetPolicyBranchType() ||	// Branch type matches
						        pkPolicyBranchEntry->GetFreePolicy() == ePolicy ||		// Free Policy with this branch
						        pkPolicyBranchEntry->GetFreeFinishingPolicy() == ePolicy)
						{
							//ChangePolicyBlockedCount(ePolicy, iPolicyEffectChange);

							// Activate/Deactivate Policies
							if(HasPolicy(ePolicy))
							{
								GetPlayer()->processPolicies(ePolicy, iPolicyEffectChange);
							}
						}
					}
				}
			}
		}
	}
}

/// Accessor: is eBranchType blocked because of branch choices?
bool CvPlayerPolicies::IsPolicyBranchBlocked(PolicyBranchTypes eBranchType) const
{
	CvAssertMsg(eBranchType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBranchType < m_pPolicies->GetNumPolicyBranches(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabPolicyBranchBlocked[eBranchType];
}

/// Accessor: is eType blocked because of  choices?
bool CvPlayerPolicies::IsPolicyBlocked(PolicyTypes eType) const
{
	CvAssertMsg(eType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eType < m_pPolicies->GetNumPolicies(), "eIndex is expected to be within maximum bounds (invalid Index)");

	// Get the policy branch we have to check.
	PolicyBranchTypes eBranch = m_paePolicyBlockedBranchCheck[eType];
	if (eBranch == NO_POLICY_BRANCH_TYPE)
		return false;	// Policy has no branch

	return IsPolicyBranchBlocked(eBranch);
}

#if defined(MOD_API_EXTENSIONS)
bool CvPlayerPolicies::CanAdoptIdeology(PolicyBranchTypes eIdeology) const
{
#if defined(MOD_EVENTS_IDEOLOGIES)
	if (MOD_EVENTS_IDEOLOGIES) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanAdoptTenet, m_pPlayer->GetID(), eIdeology) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
#endif

	return true;
}

bool CvPlayerPolicies::HasAdoptedIdeology(PolicyBranchTypes eIdeology) const
{
	return IsPolicyBranchUnlocked(eIdeology);
}
#endif

/// Implement a switch of ideologies
void CvPlayerPolicies::DoSwitchIdeologies(PolicyBranchTypes eNewBranchType)
{
	PolicyBranchTypes eOldBranchType = GetLateGamePolicyTree();
	CvAssertMsg (eOldBranchType != eNewBranchType && eNewBranchType != NO_POLICY_BRANCH_TYPE && eOldBranchType != NO_POLICY_BRANCH_TYPE, "Illegal time for Ideology change");

#if defined(MOD_BALANCE_CORE)
	GAMEEVENTINVOKE_HOOK(GAMEEVENT_IdeologySwitched, m_pPlayer->GetID(), eOldBranchType, eNewBranchType);
#endif

	int iOldBranchTenets = GetNumPoliciesOwnedInBranch(eOldBranchType);
	int iNewBranchTenets = max(0, iOldBranchTenets - GC.getSWITCH_POLICY_BRANCHES_TENETS_LOST());

	ClearPolicyBranch(eOldBranchType);
	SetPolicyBranchUnlocked(eOldBranchType, false, false);

	SetPolicyBranchUnlocked(eNewBranchType, true, true /*bRevolution*/);
	m_pPlayer->GetCulture()->DoPublicOpinion();
	m_pPlayer->GetCulture()->SetTurnIdeologySwitch(GC.getGame().getGameTurn());
	m_pPlayer->setJONSCulture(0);
	m_pPlayer->ChangeNumFreeTenets(iNewBranchTenets, false /*bCountAsFreePolicies*/);
#if defined(MOD_BUGFIX_MISSING_POLICY_EVENTS)
	if (MOD_BUGFIX_MISSING_POLICY_EVENTS)
	{
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if(pkScriptSystem)
		{
			CvLuaArgsHandle args;
			args->Push(m_pPlayer->GetID());
			args->Push(eNewBranchType);

			bool bResult = false;
			LuaSupport::CallHook(pkScriptSystem, "PlayerAdoptPolicyBranch", args.get(), bResult);
		}
	}
#endif

	//Buildings enabled by the old policy branch should be destroyed.
	int iLoop;
	for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if (pLoopCity != NULL)
		{
			CvPlayer &kCityPlayer = GET_PLAYER(pLoopCity->getOwner());
			for (int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
			{
				const CvCivilizationInfo& playerCivilizationInfo = kCityPlayer.getCivilizationInfo();
				BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings((BuildingClassTypes)iBuildingClassLoop);
				if (eBuilding != NO_BUILDING)
				{
					CvBuildingEntry *pkBuilding = GC.getBuildingInfo(eBuilding);
					if (pkBuilding)
					{
						bool bApplies = false;
						if (pkBuilding->GetPolicyBranchType() == eOldBranchType)
						{
							bApplies = true;
						}
						else if (pkBuilding->GetPolicyType() != NO_POLICY)
						{
							CvPolicyEntry* pkLoopPolicyInfo = GC.getPolicyInfo((PolicyTypes)pkBuilding->GetPolicyType());
							if (pkLoopPolicyInfo)
							{
								// This policy belongs to our branch
								if (pkLoopPolicyInfo->GetPolicyBranchType() == eOldBranchType)
								{
									bApplies = true;
								}
							}
						}
						if (bApplies && pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
						{
							pLoopCity->GetCityBuildings()->SetNumRealBuilding(eBuilding, 0);

							//release the WW back into the wild.
							if (pkBuilding->GetBuildingClassInfo().getMaxGlobalInstances() != -1)
							{
								GC.getGame().decrementBuildingClassCreatedCount((BuildingClassTypes)iBuildingClassLoop);
							}
						}
					}
				}
			}
		}
	}


	if (GC.getGame().getActivePlayer() == m_pPlayer->GetID())
	{
		DLLUI->setDirty(Policies_DIRTY_BIT, true);
	}
}

/// Delete all policies from a branch
void CvPlayerPolicies::ClearPolicyBranch(PolicyBranchTypes eBranchType)
{
	// count the policies within the branch
	for(int iPolicyLoop = 0; iPolicyLoop < GetPolicies()->GetNumPolicies(); iPolicyLoop++)
	{
		const PolicyTypes eLoopPolicy = static_cast<PolicyTypes>(iPolicyLoop);
		CvPolicyEntry* pkLoopPolicyInfo = GC.getPolicyInfo(eLoopPolicy);
		if(pkLoopPolicyInfo)
		{
			PolicyBranchTypes eLoopBranch = (PolicyBranchTypes)pkLoopPolicyInfo->GetPolicyBranchType();
			if (eLoopBranch == eBranchType)
			{
				m_pPlayer->setHasPolicy(eLoopPolicy, false);
			}
		}
	}
}

/// Accessor: how many Policy branches have we finished?
int CvPlayerPolicies::GetNumPolicyBranchesFinished() const
{
	int iNumBranchesFinished = 0;

	PolicyBranchTypes eLoopBranch;
	for(int iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
	{
		eLoopBranch = (PolicyBranchTypes) iBranchLoop;

		if(IsPolicyBranchFinished(eLoopBranch))
		{
			iNumBranchesFinished++;
		}
	}

	return iNumBranchesFinished;
}

/// Accessor: is eBranchType finished?
void CvPlayerPolicies::SetPolicyBranchFinished(PolicyBranchTypes eBranchType, bool bValue)
{
	CvAssertMsg(eBranchType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBranchType < m_pPolicies->GetNumPolicyBranches(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(bValue != IsPolicyBranchFinished(eBranchType))
	{
		m_pabPolicyBranchFinished[eBranchType] = bValue;


#if !defined(NO_ACHIEVEMENTS)
		bool bUsingXP1Scenario3 = gDLL->IsModActivated(CIV5_XP1_SCENARIO3_MODID);

		//Achievements for fulfilling branches
		if(!GC.getGame().isGameMultiPlayer() && GET_PLAYER(GC.getGame().getActivePlayer()).isHuman())
		{
			//Must not be playing smokey skies scenario.
			if(m_pPlayer->GetID() == GC.getGame().getActivePlayer() && !bUsingXP1Scenario3)
			{
				switch(eBranchType)
				{
				case 0:
					gDLL->UnlockAchievement(ACHIEVEMENT_POLICY_TRADITION);
					break;
				case 1:
					gDLL->UnlockAchievement(ACHIEVEMENT_POLICY_LIBERTY);
					break;
				case 2:
					gDLL->UnlockAchievement(ACHIEVEMENT_POLICY_HONOR);
					break;
				case 3:
					gDLL->UnlockAchievement(ACHIEVEMENT_POLICY_PIETY);
					break;
				case 4:
					gDLL->UnlockAchievement(ACHIEVEMENT_POLICY_PATRONAGE);
					break;
				
				case 5:	//Aesthetics
					gDLL->UnlockAchievement(ACHIEVEMENT_XP2_48);
					break;
				case 6:
					gDLL->UnlockAchievement(ACHIEVEMENT_POLICY_COMMERCE);
					break;
				case 7: //Exploration
					gDLL->UnlockAchievement(ACHIEVEMENT_XP2_47);
					break;
				case 8:
					gDLL->UnlockAchievement(ACHIEVEMENT_POLICY_RATIONALISM);
					break;
				}
			}
			if(gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_TRADITION) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_HONOR) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_LIBERTY) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_PIETY) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_PATRONAGE) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_ORDER) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_AUTOCRACY)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_FREEDOM)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_COMMERCE)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_RATIONALISM))
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_ALL_SOCIAL_POLICIES);
			}
		}
#endif

	}
}

/// Accessor: is eBranchType finished?
bool CvPlayerPolicies::IsPolicyBranchFinished(PolicyBranchTypes eBranchType) const
{
	CvAssertMsg(eBranchType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBranchType < m_pPolicies->GetNumPolicyBranches(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabPolicyBranchFinished[eBranchType];
}

/// Is this the last policy needed to finish this branch?
bool CvPlayerPolicies::WillFinishBranchIfAdopted(PolicyTypes eType) const
{
	PolicyBranchTypes eBranchType = (PolicyBranchTypes)m_pPolicies->GetPolicyEntry(eType)->GetPolicyBranchType();

	if(eBranchType != NO_POLICY_BRANCH_TYPE)
	{
		// Is the branch this policy is in finished?
		for(int iPolicyLoop = 0; iPolicyLoop < GetPolicies()->GetNumPolicies(); iPolicyLoop++)
		{
			const PolicyTypes eLoopPolicy = static_cast<PolicyTypes>(iPolicyLoop);

			CvPolicyEntry* pkLoopPolicyInfo = GC.getPolicyInfo(eLoopPolicy);
			if(pkLoopPolicyInfo)
			{
				// This policy belongs to our branch
				if(pkLoopPolicyInfo->GetPolicyBranchType() == eBranchType)
				{
					// We don't have this policy!
					if(!HasPolicy(eLoopPolicy))
					{
						// Is it this policy passed in?
						if(eLoopPolicy != eType)
						{
							// No, so this one won't finish branch
							return false;
						}
					}
				}
			}
		}

		// Didn't find any policy in this branch that we didn't have covered.  This will finish it
		return true;
	}

	return false;
}

/// What Policy Branches has the player chosen to adopt?
PolicyBranchTypes CvPlayerPolicies::GetPolicyBranchChosen(int iID) const
{
	if(iID < GetNumPolicyBranchesAllowed())
	{
		return m_paePolicyBranchesChosen[iID];
	}

	FAssert(false);

	return NO_POLICY_BRANCH_TYPE;
}

/// Assign Policy Branch adopted
void CvPlayerPolicies::SetPolicyBranchChosen(int iID, PolicyBranchTypes eBranchType)
{
	FAssert(eBranchType > -1);
	FAssert(eBranchType < m_pPolicies->GetNumPolicyBranches());

	if(iID < GetNumPolicyBranchesAllowed())
	{
		m_paePolicyBranchesChosen[iID] = eBranchType;
	}
	else
	{
		FAssert(false);
	}
}

/// How many Branches is the player allowed to pick from right now?
int CvPlayerPolicies::GetNumPolicyBranchesAllowed() const
{
	return /*2*/ GC.getNUM_POLICY_BRANCHES_ALLOWED() + GetNumExtraBranches();
}

/// Number of extra branches we're allowed to pick from
int CvPlayerPolicies::GetNumExtraBranches() const
{
	return m_iNumExtraBranches;
}

/// Changes number of extra branches we're allowed to pick from
void CvPlayerPolicies::ChangeNumExtraBranches(int iChange)
{
	m_iNumExtraBranches += iChange;
}

/// How many policies can we purchase at present?
int CvPlayerPolicies::GetNumPoliciesCanBeAdopted()
{
	int iNumPoliciesToAcquire = 0;

	// count the branch openers
	for(int iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
	{
		PolicyBranchTypes eBranchType = (PolicyBranchTypes)iBranchLoop;
		if (IsPolicyBranchUnlocked(eBranchType) || CanUnlockPolicyBranch(eBranchType))
		{
			CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(eBranchType);
			if(pkPolicyBranchInfo)
			{
				// Yes, it's a freebie
				if(pkPolicyBranchInfo->GetFreePolicy() != NO_POLICY)
				{
					iNumPoliciesToAcquire++;
				}
			}
		}
	}

	// count the policies within the branch
	for(int iPolicyLoop = 0; iPolicyLoop < GetPolicies()->GetNumPolicies(); iPolicyLoop++)
	{
		const PolicyTypes eLoopPolicy = static_cast<PolicyTypes>(iPolicyLoop);
		CvPolicyEntry* pkLoopPolicyInfo = GC.getPolicyInfo(eLoopPolicy);
		if(pkLoopPolicyInfo)
		{
			PolicyBranchTypes eBranchType = (PolicyBranchTypes)pkLoopPolicyInfo->GetPolicyBranchType();
			if (eBranchType != -1)
			{
				if (IsPolicyBranchUnlocked(eBranchType) || CanUnlockPolicyBranch(eBranchType))
				{
					iNumPoliciesToAcquire++;
				}
			}
		}
	}

#if defined(MOD_BUGFIX_DUMMY_POLICIES)
	return iNumPoliciesToAcquire - GetNumPoliciesOwned(MOD_BUGFIX_DUMMY_POLICIES);
#else
	return iNumPoliciesToAcquire - GetNumPoliciesOwned();
#endif
}

/// New Policy picked... figure how what that means for history. This isn't the greatest example of programming ever, but oh well, it'll do
void CvPlayerPolicies::DoNewPolicyPickedForHistory(PolicyTypes ePolicy)
{
	CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(ePolicy);
	if(pkPolicyInfo == NULL)
		return;

	PolicyBranchTypes eNewBranch = (PolicyBranchTypes) pkPolicyInfo->GetPolicyBranchType();

	// Are we a free branch policy?
	if(eNewBranch == NO_POLICY_BRANCH_TYPE)
	{
		for(int iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
		{
			const PolicyBranchTypes eLoopBranch = static_cast<PolicyBranchTypes>(iBranchLoop);
			CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(eLoopBranch);
			if(pkPolicyBranchInfo)
			{
				// Yes, it's a freebie
				if(pkPolicyBranchInfo->GetFreePolicy() == ePolicy)
				{
					eNewBranch = eLoopBranch;
					break;
				}
			}
		}
	}

	// Have we filled up the slots yet?
	if(GetBranchPicked1() == NO_POLICY_BRANCH_TYPE)
	{
		SetBranchPicked1(eNewBranch);
	}
	else if(GetBranchPicked2() == NO_POLICY_BRANCH_TYPE)
	{
		SetBranchPicked2(eNewBranch);
	}
	else if(GetBranchPicked3() == NO_POLICY_BRANCH_TYPE)
	{
		SetBranchPicked3(eNewBranch);
	}

	// if we've gotten here it means that all the slots are filled already, so we gotta bump some stuff
	else
	{
		SetBranchPicked3(GetBranchPicked2());
		SetBranchPicked2(GetBranchPicked1());
		SetBranchPicked1(eNewBranch);
	}
}

/// Everything we picked recently the same?
PolicyBranchTypes CvPlayerPolicies::GetDominantPolicyBranchForTitle() const
{
	// Everything we've picked recently matches
	if(GetBranchPicked1() == GetBranchPicked2() &&
	        GetBranchPicked1() == GetBranchPicked3() &&
	        GetBranchPicked2() == GetBranchPicked3())
	{
		return GetBranchPicked1();
	}

	// Haven't picked stuff from a branch three times in a row, so we have to see which branch we have the most of

	PolicyBranchTypes eTempBranch;

	std::vector<int> viPolicyBranchCounts;

	// Init vector
	int iBranchLoop;
	for(iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
	{
		viPolicyBranchCounts.push_back(0);
	}

	// Get count of each branch
	for(int iPolicyLoop = 0; iPolicyLoop < GC.getNumPolicyInfos(); iPolicyLoop++)
	{
		const PolicyTypes eLoopPolicy = static_cast<PolicyTypes>(iPolicyLoop);
		CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(eLoopPolicy);
		if(pkPolicyInfo)
		{
			if(HasPolicy(eLoopPolicy))
			{
				eTempBranch = (PolicyBranchTypes) pkPolicyInfo->GetPolicyBranchType();

				// Are we a free branch policy?
				if(eTempBranch == NO_POLICY_BRANCH_TYPE)
				{
					for(iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
					{
						const PolicyBranchTypes eLoopBranch = static_cast<PolicyBranchTypes>(iBranchLoop);
						CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(eLoopBranch);
						if(pkPolicyBranchInfo)
						{
							// Yes, it's a freebie
							if(pkPolicyBranchInfo->GetFreePolicy() == eLoopPolicy)
							{
								eTempBranch = eLoopBranch;
								break;
							}
						}
					}
				}

				if(eTempBranch != NO_POLICY_BRANCH_TYPE)
				{
					viPolicyBranchCounts[eTempBranch]++;
				}
			}
		}
	}

	// Now that we have our vector, see which has the most
	PolicyBranchTypes eBestBranch = NO_POLICY_BRANCH_TYPE;
	int iBestValue = 0;

	for(iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
	{
		if(viPolicyBranchCounts[iBranchLoop] > iBestValue)
		{
			eBestBranch = (PolicyBranchTypes) iBranchLoop;
			iBestValue = viPolicyBranchCounts[iBranchLoop];
		}
	}

	return eBestBranch;
}

/// What branches have we picked from lately?
PolicyBranchTypes CvPlayerPolicies::GetBranchPicked1() const
{
	return m_eBranchPicked1;
}

/// What branches have we picked from lately?
void CvPlayerPolicies::SetBranchPicked1(PolicyBranchTypes eBranch)
{
	m_eBranchPicked1 = eBranch;
}

/// What branches have we picked from lately?
PolicyBranchTypes CvPlayerPolicies::GetBranchPicked2() const
{
	return m_eBranchPicked2;
}

/// What branches have we picked from lately?
void CvPlayerPolicies::SetBranchPicked2(PolicyBranchTypes eBranch)
{
	m_eBranchPicked2 = eBranch;
}

/// What branches have we picked from lately?
PolicyBranchTypes CvPlayerPolicies::GetBranchPicked3() const
{
	return m_eBranchPicked3;
}

/// What branches have we picked from lately?
void CvPlayerPolicies::SetBranchPicked3(PolicyBranchTypes eBranch)
{
	m_eBranchPicked3 = eBranch;
}

// IDEOLOGIES

/// What is their Freedom/Autocracy/Order choice?
PolicyBranchTypes CvPlayerPolicies::GetLateGamePolicyTree() const
{
	PolicyBranchTypes eOurChoice = NO_POLICY_BRANCH_TYPE;

	PolicyBranchTypes eLoopBranch;
	for(int iBranchLoop = 0; iBranchLoop < m_pPolicies->GetNumPolicyBranches(); iBranchLoop++)
	{
		eLoopBranch = (PolicyBranchTypes) iBranchLoop;

		CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(eLoopBranch);
		if(pkPolicyBranchInfo)
		{
			if (pkPolicyBranchInfo->IsPurchaseByLevel() && IsPolicyBranchUnlocked(eLoopBranch))
			{
				eOurChoice = eLoopBranch;
				break;
			}
		}
	}

	return eOurChoice;
}

/// Is the player far enough into Industrialization that they need to choose an Ideology?
bool CvPlayerPolicies::IsTimeToChooseIdeology() const
{
	PolicyBranchTypes eFreedomBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_FREEDOM();
	PolicyBranchTypes eAutocracyBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_AUTOCRACY();
	PolicyBranchTypes eOrderBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_ORDER();
#if defined(MOD_BALANCE_CORE)
	if(m_pPlayer->isMinorCiv() || m_pPlayer->isBarbarian())
	{
		return false;
	}
#endif
	if (eFreedomBranch == NO_POLICY_BRANCH_TYPE || eAutocracyBranch == NO_POLICY_BRANCH_TYPE || eOrderBranch == NO_POLICY_BRANCH_TYPE)
	{
		return false;
	}
#if defined(MOD_BALANCE_CORE_IDEOLOGY_START)
	if(MOD_BALANCE_CORE_IDEOLOGY_START && m_pPlayer->GetIdeologyPoint() >= GC.getBALANCE_MOD_POLICY_BRANCHES_NEEDED_IDEOLOGY())
	{
		if (m_pPlayer->GetCurrentEra() >= GD_INT_GET(IDEOLOGY_PREREQ_ERA))
		{
			return true;
		}
	}
	if(MOD_BALANCE_CORE_IDEOLOGY_START && m_pPlayer->GetPlayerPolicies()->GetNumPoliciesOwned(true, true) >= GC.getBALANCE_MOD_POLICIES_NEEDED_IDEOLOGY())
	{
		if (m_pPlayer->GetCurrentEra() >= GD_INT_GET(IDEOLOGY_PREREQ_ERA))
		{
			return true;
		}
	}
#endif
#if defined(MOD_CONFIG_GAME_IN_XML)
	if (m_pPlayer->GetCurrentEra() > GD_INT_GET(IDEOLOGY_START_ERA))
#else
	if (m_pPlayer->GetCurrentEra() > GC.getInfoTypeForString("ERA_INDUSTRIAL"))
#endif
	{
		return true;
	}

	// Check for the right number of buildings of a certain type (3 factories)
	else
	{
		CvBuildingXMLEntries* pkGameBuildings = GC.GetGameBuildings();
		CvCivilizationInfo* pkInfo = GC.getCivilizationInfo(m_pPlayer->getCivilizationType());
		if(pkInfo)
		{
			// Find a building that triggers an ideology
			// Loop through all building classes
			for(int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
			{
				const BuildingTypes eBuilding = static_cast<BuildingTypes>(pkInfo->getCivilizationBuildings(iI));
				CvBuildingEntry* pkBuildingInfo = NULL;
				if(eBuilding != -1)
				{
					pkBuildingInfo = pkGameBuildings->GetEntry(eBuilding);
					if (pkBuildingInfo)
					{
						int iIdeologyTriggerCount = pkBuildingInfo->GetXBuiltTriggersIdeologyChoice();
						if (iIdeologyTriggerCount > 0)
						{
							if (m_pPlayer->getBuildingClassCount((BuildingClassTypes)iI) >= iIdeologyTriggerCount)
							{
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

/// List of tenets that can be adopted for an Ideology
std::vector<PolicyTypes> CvPlayerPolicies::GetAvailableTenets(PolicyBranchTypes eBranch, int iLevel)
{
	std::vector<PolicyTypes> availableTenets;

	CvPolicyXMLEntries* pkPolicies = GC.GetGamePolicies();
	const int iNumPolicies = pkPolicies->GetNumPolicies();

	availableTenets.reserve(iNumPolicies);
	for(int iI = 0; iI < iNumPolicies; iI++)
	{
		const PolicyTypes eTenet(static_cast<PolicyTypes>(iI));
		CvPolicyEntry* pEntry = pkPolicies->GetPolicyEntry(eTenet);
		if (pEntry && pEntry->GetPolicyBranchType() == eBranch && pEntry->GetLevel() == iLevel && !HasPolicy(eTenet))
		{
#if defined(MOD_EVENTS_IDEOLOGIES)
			if (MOD_EVENTS_IDEOLOGIES && !CanAdoptPolicy(eTenet)) {
				continue;
			}
#endif

			availableTenets.push_back(eTenet);
		}
	}

	return availableTenets;
}

/// Get the nth tenets owned of an Ideology at a certain level
PolicyTypes CvPlayerPolicies::GetTenet(PolicyBranchTypes eBranch, int iLevel, int iIndex)
{
	int iNumFound = 0;

	CvPolicyXMLEntries* pkPolicies = GC.GetGamePolicies();
	const int iNumPolicies = pkPolicies->GetNumPolicies();

	for(int iI = 0; iI < iNumPolicies; iI++)
	{
		const PolicyTypes eTenet(static_cast<PolicyTypes>(iI));
		CvPolicyEntry* pEntry = pkPolicies->GetPolicyEntry(eTenet);
		if (pEntry && pEntry->GetPolicyBranchType() == eBranch && pEntry->GetLevel() == iLevel && HasPolicy(eTenet))
		{
			iNumFound++;
			if (iNumFound == iIndex)
			{
				return eTenet;
			}
		}
	}

	return NO_POLICY;
}

// How many tenets do we have of this level?
int CvPlayerPolicies::GetNumTenetsOfLevel(PolicyBranchTypes eBranch, int iLevel) const
{
	int iNumFound = 0;

	CvPolicyXMLEntries* pkPolicies = GC.GetGamePolicies();
	const int iNumPolicies = pkPolicies->GetNumPolicies();

	for(int iI = 0; iI < iNumPolicies; iI++)
	{
		const PolicyTypes eTenet(static_cast<PolicyTypes>(iI));
		CvPolicyEntry* pEntry = pkPolicies->GetPolicyEntry(eTenet);
		if (pEntry && pEntry->GetPolicyBranchType() == eBranch && pEntry->GetLevel() == iLevel && HasPolicy(eTenet))
		{
			iNumFound++;
		}
	}

	return iNumFound;
}

// Can I purchase a Level 2 or 3 tenet?
bool CvPlayerPolicies::CanGetAdvancedTenet() const
{
	PolicyBranchTypes eIdeology = GetLateGamePolicyTree();
	if (eIdeology == NO_POLICY_BRANCH_TYPE)
	{
		return false;
	}
		
	CvPolicyXMLEntries* pkPolicies = GC.GetGamePolicies();
	for(int iI = 0; iI < GC.getNumPolicyInfos(); iI++)
	{
		const PolicyTypes eTenet(static_cast<PolicyTypes>(iI));
		CvPolicyEntry* pEntry = pkPolicies->GetPolicyEntry(eTenet);
		if (pEntry && pEntry->GetPolicyBranchType() == eIdeology && pEntry->GetLevel() > 1 && !HasPolicy(eTenet) && CanAdoptPolicy(eTenet))
		{
			return true;
		}
	}

	return false;
}

// AI

/// Update the policy AI for the turn
void CvPlayerPolicies::DoPolicyAI()
{
	CvString strBuffer;

	m_pPolicyAI->DoConsiderIdeologySwitch(m_pPlayer);

	// Do we have enough points to buy a new policy?
	if (m_pPlayer->getNextPolicyCost() > 0 || m_pPlayer->GetNumFreePolicies() > 0 || m_pPlayer->GetNumFreeTenets() > 0)
	{
		// Adopt new policies until we run out of freebies and culture (usually only one per turn)
		while(m_pPlayer->getJONSCulture() >= m_pPlayer->getNextPolicyCost() || m_pPlayer->GetNumFreePolicies() > 0 || m_pPlayer->GetNumFreeTenets() > 0)
		{
			// Choose the policy we want next (or a branch)
			int iNextPolicy = m_pPolicyAI->ChooseNextPolicy(m_pPlayer);
			if (iNextPolicy == NO_POLICY)
				break;

			// These actions should spend our number of free policies or our culture, otherwise we'll loop forever
			if(iNextPolicy < m_pPolicies->GetNumPolicyBranches()) // Low return values indicate a branch has been chosen
			{
				m_pPlayer->GetPlayerPolicies()->DoUnlockPolicyBranch((PolicyBranchTypes)iNextPolicy);
			}
			else
			{
				m_pPlayer->doAdoptPolicy((PolicyTypes)(iNextPolicy - m_pPolicies->GetNumPolicyBranches()));
			}
		}
	}
}

/// Get the policy AI to pick an ideology
void CvPlayerPolicies::DoChooseIdeology()
{
	m_pPolicyAI->DoChooseIdeology(m_pPlayer);
}

// PRIVATE METHODS

// Internal method to add all of this leaderheads' flavors as strategies for policy AI
void CvPlayerPolicies::AddFlavorAsStrategies(int iPropagatePercent)
{
	int iFlavorValue;

	// Start by resetting the AI
	m_pPolicyAI->Reset();

#if defined(MOD_BALANCE_CORE)
	int iCurrentUnhappiness = m_pPlayer->GetUnhappiness(); 
#endif

	// Now populate the AI with the current flavor information
	for(int iFlavor = 0; iFlavor < GC.getNumFlavorTypes(); iFlavor++)
	{
//		OLD WAY: use CURRENT player flavors
//		iFlavorValue = GetLatestFlavorValue((FlavorTypes) iFlavor);

//		NEW WAY: use PERSONALITY flavors (since policy choices are LONG-TERM)
//		EVEN NEWER WAY: add in a modifier for the Grand Strategy we are running (since these are also long term)
#if defined(MOD_AI_SMART_GRAND_STRATEGY)
		// NEWER NEWER WAY: don't add grand strategy factor before medieval era, the AI still doesn't know if the Grand Strategy is solid.
		EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);
		if (MOD_AI_SMART_GRAND_STRATEGY && m_pPlayer->GetCurrentEra() < eMedieval)
		{
			iFlavorValue = m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes) iFlavor);
		}
		else
#endif
		iFlavorValue = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes) iFlavor);

//		Boost flavor even further based on in-game conditions
		EconomicAIStrategyTypes eStrategyLosingMoney = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY", true);
		if (eStrategyLosingMoney == NO_ECONOMICAISTRATEGY)
		{
			continue;
		}
		CvTeam& kTeam = GET_TEAM(m_pPlayer->getTeam());
		bool bIsAtWarWithSomeone = (kTeam.getAtWarCount(false) > 0);
		bool bInDeficit = m_pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyLosingMoney);

		if(bInDeficit && iFlavor == GC.getInfoTypeForString("FLAVOR_GOLD"))
		{
			iFlavorValue += 5;
		}
#if defined(MOD_BALANCE_CORE)
		else if(m_pPlayer->GetHappiness() < iCurrentUnhappiness && iFlavor == GC.getInfoTypeForString("FLAVOR_HAPPINESS"))
#else
		else if(m_pPlayer->GetHappiness() < m_pPlayer->GetUnhappiness() && iFlavor == GC.getInfoTypeForString("FLAVOR_HAPPINESS"))
#endif
		{
			iFlavorValue += 5;
		}
		else if(bIsAtWarWithSomeone && iFlavor == GC.getInfoTypeForString("FLAVOR_DEFENSE"))
		{
			iFlavorValue += 3;
		}
		else if(bIsAtWarWithSomeone && iFlavor == GC.getInfoTypeForString("FLAVOR_CITY_DEFENSE"))
		{
			iFlavorValue += 3;
		}

		if(iFlavorValue > 0)
		{
			m_pPolicyAI->AddFlavorWeights((FlavorTypes)iFlavor, iFlavorValue, iPropagatePercent);
		}
	}
}

void CvPlayerPolicies::LogFlavors(FlavorTypes)
{
	return; // Now using personality flavors, so this is unnecessary (or is it?)
}

// HELPER CLASSES

int PolicyHelpers::GetNumPlayersWithBranchUnlocked(PolicyBranchTypes eBranch)
{
	int iRtnValue = 0;

	for (int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		CvPlayer &kPlayer = GET_PLAYER((PlayerTypes)iI);

		if (kPlayer.isAlive() && !kPlayer.isMinorCiv() && !kPlayer.isBarbarian())
		{
			if (kPlayer.GetPlayerPolicies()->IsPolicyBranchUnlocked(eBranch))
			{
				iRtnValue++;
			}
		}
	}

	return iRtnValue;
}

int PolicyHelpers::GetNumFreePolicies(PolicyBranchTypes eBranch)
{
	int iFreePolicies = 0;

	CvPolicyBranchEntry *pkEntry = GC.getPolicyBranchInfo(eBranch);
	if (pkEntry)
	{
		if (pkEntry->GetEraPrereq() >= GC.getGame().getStartEra())
		{
			int iNumPreviousUnlockers = PolicyHelpers::GetNumPlayersWithBranchUnlocked(eBranch);
			if (iNumPreviousUnlockers == 0)
			{
				iFreePolicies = pkEntry->GetFirstAdopterFreePolicies();
			}
			else if (iNumPreviousUnlockers == 1)
			{
				iFreePolicies = pkEntry->GetSecondAdopterFreePolicies();
			}
#if defined(MOD_BALANCE_CORE)
			if (MOD_BALANCE_CORE_VICTORY_GAME_CHANGES)
			{ 
				if (iNumPreviousUnlockers >= 1)
				iFreePolicies = pkEntry->GetSecondAdopterFreePolicies();
			}
#endif
		}
	}

	return iFreePolicies;
}