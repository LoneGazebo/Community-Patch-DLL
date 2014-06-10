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
	m_iDomesticGreatGeneralRateModifier(0),
	m_iExtraHappiness(0),
	m_iExtraHappinessPerCity(0),
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
	m_iExtraHappinessPerLuxury(0),
	m_iUnhappinessFromUnitsMod(0),
	m_iNumExtraBuilders(0),
	m_iPlotGoldCostMod(0),
	m_iPlotCultureCostModifier(0),
	m_iPlotCultureExponentModifier(0),
	m_iNumCitiesPolicyCostDiscount(0),
	m_iGarrisonedCityRangeStrikeModifier(0),
	m_iUnitPurchaseCostModifier(0),
	m_iTradeRouteGoldModifier(0),
	m_iTradeMissionGoldModifier(0),
	m_iFaithCostModifier(0),
	m_iCulturalPlunderMultiplier(0),
	m_iStealTechSlowerModifier(0),
	m_iCatchSpiesModifier(0),
	m_iGoldPerUnit(0),
	m_iGoldPerMilitaryUnit(0),
	m_iCityStrengthMod(0),
	m_iCityGrowthMod(0),
	m_iCapitalGrowthMod(0),
	m_iSettlerProductionModifier(0),
	m_iCapitalSettlerProductionModifier(0),
	m_iNewCityExtraPopulation(0),
	m_iFreeFoodBox(0),
	m_iRouteGoldMaintenanceMod(0),
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
	m_bMinorGreatPeopleAllies(false),
	m_bMinorScienceAllies(false),
	m_bMinorResourceBonus(false),
	m_bGoldenAgeCultureBonusDisabled(false),
	m_iPolicyBranchType(NO_POLICY_BRANCH_TYPE),
	m_iNumExtraBranches(0),
	m_iHappinessToCulture(0),
	m_iHappinessToScience(0),
	m_iNumCitiesFreeCultureBuilding(0),
	m_iNumCitiesFreeFoodBuilding(0),
	m_bHalfSpecialistUnhappiness(false),
	m_bHalfSpecialistFood(false),
	m_bMilitaryFoodProduction(false),
	m_iWoundedUnitDamageMod(0),
	m_iUnitUpgradeCostMod(0),
	m_iBarbarianCombatBonus(0),
	m_bAlwaysSeeBarbCamps(false),
	m_bRevealAllCapitals(false),
	m_bGarrisonFreeMaintenance(false),
	m_bOneShot(false),
	m_bIncludesOneShotFreeUnits(false),
	m_piPrereqOrPolicies(NULL),
	m_piPrereqAndPolicies(NULL),
	m_piPolicyDisables(NULL),
	m_piYieldModifier(NULL),
	m_piCityYieldChange(NULL),
	m_piCoastalCityYieldChange(NULL),
	m_piCapitalYieldChange(NULL),
	m_piCapitalYieldPerPopChange(NULL),
	m_piCapitalYieldModifier(NULL),
	m_piSpecialistExtraYield(NULL),
	m_pabFreePromotion(NULL),
	m_paiUnitCombatProductionModifiers(NULL),
	m_paiUnitCombatFreeExperiences(NULL),
	m_paiBuildingClassCultureChanges(NULL),
	m_paiBuildingClassProductionModifiers(NULL),
	m_paiBuildingClassHappiness(NULL),
	m_paiFreeUnitClasses(NULL),
	m_paiHurryModifier(NULL),
	m_pabSpecialistValid(NULL),
	m_ppiImprovementYieldChanges(NULL),
	m_ppiBuildingClassYieldModifiers(NULL),
	m_ppiBuildingClassYieldChanges(NULL),
	m_piFlavorValue(NULL)
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
	SAFE_DELETE_ARRAY(m_piCapitalYieldModifier);
	SAFE_DELETE_ARRAY(m_piSpecialistExtraYield);
	SAFE_DELETE_ARRAY(m_pabFreePromotion);
	SAFE_DELETE_ARRAY(m_paiUnitCombatProductionModifiers);
	SAFE_DELETE_ARRAY(m_paiUnitCombatFreeExperiences);
	SAFE_DELETE_ARRAY(m_paiBuildingClassCultureChanges);
	SAFE_DELETE_ARRAY(m_paiBuildingClassProductionModifiers);
	SAFE_DELETE_ARRAY(m_paiBuildingClassHappiness);
	SAFE_DELETE_ARRAY(m_paiFreeUnitClasses);

//	SAFE_DELETE_ARRAY(m_pabHurry);
	SAFE_DELETE_ARRAY(m_paiHurryModifier);
	SAFE_DELETE_ARRAY(m_pabSpecialistValid);

	CvDatabaseUtility::SafeDelete2DArray(m_ppiImprovementYieldChanges);
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
	m_iDomesticGreatGeneralRateModifier = kResults.GetInt("DomesticGreatGeneralRateModifier");
	m_iExtraHappiness = kResults.GetInt("ExtraHappiness");
	m_iExtraHappinessPerCity = kResults.GetInt("ExtraHappinessPerCity");
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
	m_iExtraHappinessPerLuxury = kResults.GetInt("ExtraHappinessPerLuxury");
	m_iUnhappinessFromUnitsMod = kResults.GetInt("UnhappinessFromUnitsMod");
	m_iNumExtraBuilders = kResults.GetInt("NumExtraBuilders");
	m_iPlotGoldCostMod = kResults.GetInt("PlotGoldCostMod");
	m_iPlotCultureCostModifier = kResults.GetInt("PlotCultureCostModifier");
	m_iPlotCultureExponentModifier = kResults.GetInt("PlotCultureExponentModifier");
	m_iNumCitiesPolicyCostDiscount = kResults.GetInt("NumCitiesPolicyCostDiscount");
	m_iGarrisonedCityRangeStrikeModifier = kResults.GetInt("GarrisonedCityRangeStrikeModifier");
	m_iUnitPurchaseCostModifier = kResults.GetInt("UnitPurchaseCostModifier");
	m_iTradeRouteGoldModifier = kResults.GetInt("TradeRouteGoldModifier");
	m_iTradeMissionGoldModifier = kResults.GetInt("TradeMissionGoldModifier");
	m_iFaithCostModifier = kResults.GetInt("FaithCostModifier");
	m_iCulturalPlunderMultiplier = kResults.GetInt("CulturalPlunderMultiplier");
	m_iStealTechSlowerModifier = kResults.GetInt("StealTechSlowerModifier");
	m_iCatchSpiesModifier = kResults.GetInt("CatchSpiesModifier");
	m_iGoldPerUnit = kResults.GetInt("GoldPerUnit");
	m_iGoldPerMilitaryUnit = kResults.GetInt("GoldPerMilitaryUnit");
	m_iCityStrengthMod = kResults.GetInt("CityStrengthMod");
	m_iCityGrowthMod = kResults.GetInt("CityGrowthMod");
	m_iCapitalGrowthMod = kResults.GetInt("CapitalGrowthMod");
	m_iSettlerProductionModifier = kResults.GetInt("SettlerProductionModifier");
	m_iCapitalSettlerProductionModifier = kResults.GetInt("CapitalSettlerProductionModifier");
	m_iNewCityExtraPopulation = kResults.GetInt("NewCityExtraPopulation");
	m_iFreeFoodBox = kResults.GetInt("FreeFoodBox");
	m_iRouteGoldMaintenanceMod = kResults.GetInt("RouteGoldMaintenanceMod");
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
	m_bMinorGreatPeopleAllies = kResults.GetBool("MinorGreatPeopleAllies");
	m_bMinorScienceAllies = kResults.GetBool("MinorScienceAllies");
	m_bMinorResourceBonus = kResults.GetBool("MinorResourceBonus");
	m_bGoldenAgeCultureBonusDisabled = kResults.GetBool("GoldenAgeCultureBonusDisabled");
	m_bOneShot = kResults.GetBool("OneShot");
	m_bIncludesOneShotFreeUnits = kResults.GetBool("IncludesOneShotFreeUnits");

	m_strWeLoveTheKingKey = kResults.GetText("WeLoveTheKing");
	m_wstrWeLoveTheKing = GetLocalizedText(m_strWeLoveTheKingKey);

	//References
	const char* szTechPrereq = kResults.GetText("TechPrereq");
	m_iTechPrereq = GC.getInfoTypeForString(szTechPrereq, true);

	const char* szPolicyBranchType = kResults.GetText("PolicyBranchType");
	m_iPolicyBranchType = GC.getInfoTypeForString(szPolicyBranchType, true);

	m_iNumExtraBranches = kResults.GetInt("NumExtraBranches");

	//Arrays
	const char* szPolicyType = GetType();
	kUtility.SetYields(m_piYieldModifier, "Policy_YieldModifiers", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piCityYieldChange, "Policy_CityYieldChanges", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piCoastalCityYieldChange, "Policy_CoastalCityYieldChanges", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piCapitalYieldChange, "Policy_CapitalYieldChanges", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piCapitalYieldPerPopChange, "Policy_CapitalYieldPerPopChanges", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piCapitalYieldModifier, "Policy_CapitalYieldModifiers", "PolicyType", szPolicyType);
	kUtility.SetYields(m_piSpecialistExtraYield, "Policy_SpecialistExtraYields", "PolicyType", szPolicyType);

	kUtility.SetFlavors(m_piFlavorValue, "Policy_Flavors", "PolicyType", szPolicyType);

	kUtility.PopulateArrayByValue(m_paiHurryModifier, "HurryInfos", "Policy_HurryModifiers", "HurryType", "PolicyType", szPolicyType, "HurryCostModifier");

	kUtility.PopulateArrayByExistence(m_pabSpecialistValid, "Specialists", "Policy_ValidSpecialists", "SpecialistType", "PolicyType", szPolicyType);

	kUtility.PopulateArrayByExistence(m_pabFreePromotion, "UnitPromotions", "Policy_FreePromotions", "PromotionType", "PolicyType", szPolicyType);
	kUtility.PopulateArrayByValue(m_paiUnitCombatFreeExperiences, "UnitCombatInfos", "Policy_UnitCombatFreeExperiences", "UnitCombatType", "PolicyType", szPolicyType, "FreeExperience");
	kUtility.PopulateArrayByValue(m_paiUnitCombatProductionModifiers, "UnitCombatInfos", "Policy_UnitCombatProductionModifiers", "UnitCombatType", "PolicyType", szPolicyType, "ProductionModifier");

	kUtility.PopulateArrayByValue(m_paiBuildingClassCultureChanges, "BuildingClasses", "Policy_BuildingClassCultureChanges", "BuildingClassType", "PolicyType", szPolicyType, "CultureChange");
	kUtility.PopulateArrayByValue(m_paiBuildingClassProductionModifiers, "BuildingClasses", "Policy_BuildingClassProductionModifiers", "BuildingClassType", "PolicyType", szPolicyType, "ProductionModifier");
	kUtility.PopulateArrayByValue(m_paiBuildingClassHappiness, "BuildingClasses", "Policy_BuildingClassHappiness", "BuildingClassType", "PolicyType", szPolicyType, "Happiness");

	kUtility.PopulateArrayByValue(m_paiFreeUnitClasses, "UnitClasses", "Policy_FreeUnitClasses", "UnitClassType", "PolicyType", szPolicyType, "Count");

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

/// How much more Gold do we make from Trade Routes
int CvPolicyEntry::GetTradeRouteGoldModifier() const
{
	return m_iTradeRouteGoldModifier;
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

/// How much easier is it to catch enemy spies?
int CvPolicyEntry::GetCatchSpiesModifier() const
{
	return m_iCatchSpiesModifier;
}

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

/// Amount of free food newly-founded Cities receive
int CvPolicyEntry::GetFreeFoodBox() const
{
	return m_iFreeFoodBox;
}

/// Route upkeep cost Modifier (e.g. 50 = 150% normal cost)
int CvPolicyEntry::GetRouteGoldMaintenanceMod() const
{
	return m_iRouteGoldMaintenanceMod;
}

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

/// Yield modifier for a specific improvement by yield type
int CvPolicyEntry::GetImprovementYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiImprovementYieldChanges[i][j];
}

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

//=====================================
// CvPolicyBranchEntry
//=====================================

/// Constructor
CvPolicyBranchEntry::CvPolicyBranchEntry(void):
	m_iEraPrereq(NO_ERA),
	m_iFreePolicy(NO_POLICY),
	m_iFreeFinishingPolicy(NO_POLICY),
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

	const char* szPolicyBranchType = GetType();

	m_bMutuallyExclusive = kResults.GetBool("AIMutuallyExclusive");

	m_bDelayWhenNoReligion = kResults.GetBool("AIDelayNoReligion");
	m_bDelayWhenNoCulture = kResults.GetBool("AIDelayNoCulture");
	m_bDelayWhenNoCityStates = kResults.GetBool("AIDelayNoCityStates");
	m_bDelayWhenNoScience = kResults.GetBool("AIDelayNoScience");

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

/// Policy Branches disabled when this one chosen
int CvPolicyBranchEntry::GetPolicyBranchDisables(int i) const
{
	return m_piPolicyBranchDisables ? m_piPolicyBranchDisables[i] : -1;
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
	m_pPlayer(NULL),
	m_iMaxEffectiveCities(0)
{
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

	m_iMaxEffectiveCities = 0;

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

	CvAssertMsg(m_pPolicies != NULL && m_pPolicies->GetNumPolicies() > 0, "Number of policies to serialize is expected to greater than 0");

	uint uiPolicyCount = 0;
	uint uiPolicyBranchCount = 0;
	if(m_pPolicies)
	{
		uiPolicyCount = m_pPolicies->GetNumPolicies();
		uiPolicyBranchCount = m_pPolicies->GetNumPolicyBranches();
	}

	if(uiVersion >= 5)
	{
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabHasPolicy, uiPolicyCount);
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabHasOneShotPolicyFired, uiPolicyCount);
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabHaveOneShotFreeUnitsFired, uiPolicyCount);

		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabPolicyBranchUnlocked, uiPolicyBranchCount);
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabPolicyBranchBlocked, uiPolicyBranchCount);
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabPolicyBranchFinished, uiPolicyBranchCount);
		CvInfosSerializationHelper::ReadHashedTypeArray(kStream, m_paePolicyBranchesChosen, uiPolicyBranchCount);

	}
	else
	{
		CvInfosSerializationHelper::ReadV0DataArray(kStream, m_pabHasPolicy, uiPolicyCount, CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0PolicyTags));
		CvInfosSerializationHelper::ReadV0DataArray(kStream, m_pabHasOneShotPolicyFired, uiPolicyCount, CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0PolicyTags));

		if(uiVersion >= 4)
		{
			CvInfosSerializationHelper::ReadV0DataArray(kStream, m_pabHaveOneShotFreeUnitsFired,uiPolicyCount, CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0PolicyTags));
		}
		else
		{
			for(uint iI = 0; iI < uiPolicyCount; iI++)
			{
				m_pabHaveOneShotFreeUnitsFired[iI] = false;
			}
		}

		CvInfosSerializationHelper::ReadV0DataArray(kStream, m_pabPolicyBranchUnlocked, uiPolicyBranchCount, CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0PolicyBranchTags));
		CvInfosSerializationHelper::ReadV0DataArray(kStream, m_pabPolicyBranchBlocked, uiPolicyBranchCount, CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0PolicyBranchTags));
		CvInfosSerializationHelper::ReadV0DataArray(kStream, m_pabPolicyBranchFinished, uiPolicyBranchCount, CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0PolicyBranchTags));
		CvInfosSerializationHelper::ReadV0TypeArray(kStream, m_paePolicyBranchesChosen, uiPolicyBranchCount, CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0PolicyBranchTags));
	}

	kStream >> m_iNumExtraBranches;

	if(uiVersion >= 5)
	{
		m_eBranchPicked1 = (PolicyBranchTypes)CvInfosSerializationHelper::ReadHashed(kStream);
		m_eBranchPicked2 = (PolicyBranchTypes)CvInfosSerializationHelper::ReadHashed(kStream);
		m_eBranchPicked3 = (PolicyBranchTypes)CvInfosSerializationHelper::ReadHashed(kStream);
	}
	else
	{
		kStream >> m_eBranchPicked1;
		m_eBranchPicked1 = CvInfosSerializationHelper::ConvertV0(m_eBranchPicked1, CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0PolicyBranchTags));
		kStream >> m_eBranchPicked2;
		m_eBranchPicked2 = CvInfosSerializationHelper::ConvertV0(m_eBranchPicked2, CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0PolicyBranchTags));
		kStream >> m_eBranchPicked3;
		m_eBranchPicked3 = CvInfosSerializationHelper::ConvertV0(m_eBranchPicked3, CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0PolicyBranchTags));
	}

	if(uiVersion < 2)
	{
		int iNumCities = m_pPlayer->getNumCities();
		iNumCities -= m_pPlayer->GetNumPuppetCities();
		if(iNumCities == 0)
			iNumCities = 1;
		m_iMaxEffectiveCities = iNumCities;
	}
	else
	{
		kStream >> m_iMaxEffectiveCities;
	}

	// Now for AI
	m_pPolicyAI->Read(kStream);

	CvAssertMsg(m_piLatestFlavorValues != NULL && GC.getNumFlavorTypes() > 0, "Number of flavor values to serialize is expected to greater than 0");

	int iNumFlavors = 27;
	if(uiVersion >= 3)
	{
		kStream >> iNumFlavors;
	}

	ArrayWrapper<int> wrapm_piLatestFlavorValues(iNumFlavors, m_piLatestFlavorValues);
	kStream >> wrapm_piLatestFlavorValues;
}

/// Serialization write
void CvPlayerPolicies::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 5;
	kStream << uiVersion;

	CvAssertMsg(m_pPolicies != NULL && GC.getNumPolicyInfos() > 0, "Number of policies to serialize is expected to greater than 0");

	uint uiPolicyCount = 0;
	uint uiPolicyBranchCount = 0;
	if(m_pPolicies)
	{
		uiPolicyCount = m_pPolicies->GetNumPolicies();
		uiPolicyBranchCount = m_pPolicies->GetNumPolicyBranches();
	}

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

	kStream << m_iMaxEffectiveCities;

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

/// Accessor: does a player have a policy
bool CvPlayerPolicies::HasPolicy(PolicyTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumPolicyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabHasPolicy[eIndex];
}

/// Accessor: set whether player has a policy
void CvPlayerPolicies::SetPolicy(PolicyTypes eIndex, bool bNewValue)
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
		GetPlayer()->ChangeNumPolicies(iChange);

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
int CvPlayerPolicies::GetNumPoliciesOwned() const
{
	int rtnValue = 0;

	for(int i = 0; i < m_pPolicies->GetNumPolicies(); i++)
	{
		// Do we have this policy?
		if(m_pabHasPolicy[i])
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
			case POLICYMOD_GREAT_PERSON_RATE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGreatPeopleRateModifier();
				break;
			case POLICYMOD_GREAT_GENERAL_RATE:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetGreatGeneralRateModifier();
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
			case POLICYMOD_CATCH_SPIES_MODIFIER:
				rtnValue += m_pPolicies->GetPolicyEntry(i)->GetCatchSpiesModifier();
				break;
			}
		}
	}

	return rtnValue;
}

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

/// How much will the next policy cost?
int CvPlayerPolicies::GetNextPolicyCost()
{
	int iNumPolicies = GetNumPoliciesOwned();

	// Reduce count by however many free Policies we've had in this game
	iNumPolicies -= (m_pPlayer->GetNumFreePoliciesEver() - m_pPlayer->GetNumFreePolicies());

	// Each branch we unlock (after the first) costs us a buy, so add that in; JON: not any more
	//if (GetNumPolicyBranchesUnlocked() > 0)
	//{
	//	iNumPolicies += (GetNumPolicyBranchesUnlocked() - 1);
	//}

	int iCost = 0;
	iCost += (iNumPolicies* /*7*/ GC.getPOLICY_COST_INCREASE_TO_BE_EXPONENTED());

	// Exponential cost scaling
	iCost = (int) pow((double) iCost, (double) /*1.70*/ GC.getPOLICY_COST_EXPONENT());

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

	int iNumCities = m_pPlayer->getNumCities();

	iNumCities -= m_pPlayer->GetNumPuppetCities();	// Don't count puppet Cities

	int iNumLimboCities = 0;
	const CvCity* pLoopCity;
	int iLoop;
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity->IsIgnoreCityForHappiness() || pLoopCity->IsRazing())
		{
			iNumLimboCities++;
		}
	}

	iNumCities -= iNumLimboCities;	// Don't count cities where the player hasn't decided yet what to do with them or ones that are currently being razed

	if(iNumCities == 0)	// If we don't pretend the player has at least one city it screws up the math
		iNumCities = 1;

	m_iMaxEffectiveCities = m_iMaxEffectiveCities > iNumCities ? m_iMaxEffectiveCities : iNumCities;

	iMod = (iCost * (m_iMaxEffectiveCities - 1) * iMod);
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

	if(m_pPlayer->IsAnarchy())
	{
		return false;
	}

	// Has enough culture to spend?
	if((!bIgnoreCost) && m_pPlayer->getNextPolicyCost() > 0)
	{
		if(m_pPlayer->getJONSCulture() < m_pPlayer->getNextPolicyCost())
		{
			if(m_pPlayer->GetNumFreePolicies() == 0)
			{
				return false;
			}
		}
	}

	PolicyBranchTypes eBranch = (PolicyBranchTypes) pkPolicyEntry->GetPolicyBranchType();

	// If it doesn't have a branch, it's a freebie that comes WITH the branch, so we can't pick it manually
	if(eBranch == NO_POLICY_BRANCH_TYPE)
	{
		return false;
	}

	// If we have 0 branches unlocked already the first Policy is free!  After that we have to have unlocked the branch
//	if (GetNumPolicyBranchesUnlocked() > 0)
	{
		if(!IsPolicyBranchUnlocked(eBranch))
		{
			return false;
		}
	}

	//PolicyBranchTypes ePolicyBranch;
	//bool bMatchFound = false;

	// Belongs to a branch that's allowed?
	//for (int iPolicyBranchChosenLoop = 0; iPolicyBranchChosenLoop < GetNumPolicyBranchesAllowed(); iPolicyBranchChosenLoop++)
	//{
	//	ePolicyBranch = GetPolicyBranchChosen(iPolicyBranchChosenLoop);

	//	// Picked a Branch, does this one match the Policy?
	//	if (ePolicyBranch != NO_POLICY_BRANCH_TYPE)
	//	{
	//		if (GC.getPolicyInfo(eIndex).GetPolicyBranchType() == ePolicyBranch)
	//		{
	//			bMatchFound = true;
	//			break;
	//		}
	//	}
	//	// Hasn't picked a Branch yet
	//	else
	//	{
	//		bMatchFound = true;
	//		break;
	//	}
	//}

	//// We don't have any open Policy Branch slots and the ones we've picked don't match this Policy
	//if (!bMatchFound)
	//{
	//	return false;
	//}

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
	SetPolicyBranchUnlocked(eBranchType, true);

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
	if(GetPlayer()->IsAnarchy())
	{
		return false;
	}

	// Must have enough culture to spend a buy opening a new branch
	if(GetPlayer()->getJONSCulture() < GetPlayer()->getNextPolicyCost())
	{
		if(GetPlayer()->GetNumFreePolicies() == 0)
			return false;
	}

	CvPolicyBranchEntry* pkBranchEntry = m_pPolicies->GetPolicyBranchEntry(eBranchType);
	if(pkBranchEntry)
	{
		EraTypes ePrereqEra = (EraTypes) pkBranchEntry->GetEraPrereq();

		// Must be in the proper Era
		if(ePrereqEra != NO_ERA)
		{
			if(GET_TEAM(GetPlayer()->getTeam()).GetCurrentEra() < ePrereqEra)
			{
				return false;
			}
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
void CvPlayerPolicies::SetPolicyBranchUnlocked(PolicyBranchTypes eBranchType, bool bNewValue)
{
	CvAssertMsg(eBranchType >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBranchType < m_pPolicies->GetNumPolicyBranches(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(IsPolicyBranchUnlocked(eBranchType) != bNewValue)
	{
		m_pabPolicyBranchUnlocked[eBranchType] = bNewValue;

		// JON: This notification disabled
		//if (bNewValue)
		//{
		//	const char* strBranchText = GC.getPolicyBranchInfo(eBranchType)->GetTextKey();

		//	// Tell everyone we picked something new
		//	Localization::String strMessage = Localization::Lookup("TXT_KEY_MISC_PLAYER_ADOPTED_POLICY");
		//	strMessage << GetPlayer()->getCivilizationShortDescriptionKey() << strBranchText;
		//	Localization::String strSummary = Localization::Lookup("TXT_KEY_MISC_PLAYER_ADOPTED_POLICY_SUMMARY");
		//	strSummary << GetPlayer()->getCivilizationShortDescriptionKey() << strBranchText;

		//	// Send out a message to the other players
		//	PlayerTypes ePlayer;
		//	for (int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		//	{
		//		ePlayer = (PlayerTypes) iI;

		//		if (GET_PLAYER(ePlayer).isAlive())
		//		{
		//			// Don't tell us about ourselves
		//			if (ePlayer != GetPlayer()->GetID())
		//			{
		//				// Must have met other player
		//				if (GET_TEAM(GetPlayer()->getTeam()).isHasMet(GET_PLAYER(ePlayer).getTeam()))
		//				{
		//					CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
		//					if (pNotifications)
		//					{
		//						pNotifications->Add(NOTIFICATION_POLICY_ADOPTION, strMessage.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
		//					}
		//				}
		//			}
		//		}
		//	}
		//}
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
				case 5:
					gDLL->UnlockAchievement(ACHIEVEMENT_POLICY_COMMERCE);
					break;
				case 6:
					gDLL->UnlockAchievement(ACHIEVEMENT_POLICY_RATIONALISM);
					break;
				case 7:
					gDLL->UnlockAchievement(ACHIEVEMENT_POLICY_FREEDOM);
					break;
				case 8:
					gDLL->UnlockAchievement(ACHIEVEMENT_POLICY_ORDER);
					break;
				case 9:
					gDLL->UnlockAchievement(ACHIEVEMENT_POLICY_AUTOCRACY);
					break;
				}
			}
			if(gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_TRADITION) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_HONOR) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_LIBERTY) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_PIETY) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_PATRONAGE) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_ORDER) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_AUTOCRACY)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_FREEDOM)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_COMMERCE)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_POLICY_RATIONALISM))
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_ALL_SOCIAL_POLICIES);
			}
		}

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

/// What is their Freedom/Autocracy/Order choice?
PolicyBranchTypes CvPlayerPolicies::GetLateGamePolicyTree() const
{
	PolicyBranchTypes eFreedom = (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_FREEDOM", true /*bHideAsserts*/);
	PolicyBranchTypes eAutocracy = (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_AUTOCRACY", true /*bHideAsserts*/);
	PolicyBranchTypes eOrder = (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_ORDER", true /*bHideAsserts*/);

	PolicyBranchTypes eOurChoice = NO_POLICY_BRANCH_TYPE;

	if(eFreedom != NO_POLICY_BRANCH_TYPE && m_pPlayer->GetPlayerPolicies()->IsPolicyBranchUnlocked(eFreedom))
	{
		eOurChoice = eFreedom;
	}
	else if(eAutocracy != NO_POLICY_BRANCH_TYPE && m_pPlayer->GetPlayerPolicies()->IsPolicyBranchUnlocked(eAutocracy))
	{
		eOurChoice = eAutocracy;
	}
	else if(eOrder != NO_POLICY_BRANCH_TYPE && m_pPlayer->GetPlayerPolicies()->IsPolicyBranchUnlocked(eOrder))
	{
		eOurChoice = eOrder;
	}

	return eOurChoice;
}

/// How many policies can we purchase at present?
int CvPlayerPolicies::GetNumPoliciesCanBeAdopted() const
{
	return m_pPolicies->GetNumPolicies() - GetNumPoliciesOwned();
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

/// Update the policy AI for the turn
void CvPlayerPolicies::DoPolicyAI()
{
	CvString strBuffer;

	// Do we have enough points to buy a new policy?
	if(m_pPlayer->getNextPolicyCost() > 0)
	{
		// Adopt new policies until we run out of freebies and culture (usually only one per turn)
		while(m_pPlayer->getJONSCulture() >= m_pPlayer->getNextPolicyCost() || m_pPlayer->GetNumFreePolicies() > 0)
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

// PRIVATE METHODS

// Internal method to add all of this leaderheads' flavors as strategies for policy AI
void CvPlayerPolicies::AddFlavorAsStrategies(int iPropagatePercent)
{
	int iFlavorValue;

	// Start by resetting the AI
	m_pPolicyAI->Reset();

	// Now populate the AI with the current flavor information
	for(int iFlavor = 0; iFlavor < GC.getNumFlavorTypes(); iFlavor++)
	{
//		OLD WAY: use CURRENT player flavors
//		iFlavorValue = GetLatestFlavorValue((FlavorTypes) iFlavor);

//		NEW WAY: use PERSONALITY flavors (since policy choices are LONG-TERM)
//		EVEN NEWER WAY: add in a modifier for the Grand Strategy we are running (since these are also long term)
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
		else if(m_pPlayer->GetHappiness() < m_pPlayer->GetUnhappiness() && iFlavor == GC.getInfoTypeForString("FLAVOR_HAPPINESS"))
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
