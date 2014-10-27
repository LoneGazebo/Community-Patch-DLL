/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvInternalGameCoreUtils.h"
#include "FStlContainerSerialization.h"
#include "CvEnumSerialization.h"
#include "CvDLLUtilDefines.h"
#include "CvDllCity.h"
#include "CvDllPlot.h"

// include after all other headers
#include "LintFree.h"

/// Constructor
CvBuildingEntry::CvBuildingEntry(void):
	m_iBuildingClassType(NO_BUILDINGCLASS),
	m_pkBuildingClassInfo(NULL),
	m_iNearbyTerrainRequired(NO_VICTORY),
	m_iProhibitedCityTerrain(NO_VICTORY),
	m_iVictoryPrereq(NO_VICTORY),
	m_iFreeStartEra(NO_ERA),
	m_iMaxStartEra(NO_ERA),
	m_iObsoleteTech(NO_TECH),
	m_iEnhancedYieldTech(NO_TECH),
	m_iGoldMaintenance(0),
	m_iMutuallyExclusiveGroup(0),
	m_iReplacementBuildingClass(NO_BUILDINGCLASS),
	m_iPrereqAndTech(NO_TECH),
	m_iSpecialistType(NO_SPECIALIST),
	m_iSpecialistCount(0),
	m_iSpecialistExtraCulture(0),
	m_iGreatPeopleRateChange(0),
	m_iFreeBuildingClass(NO_BUILDINGCLASS),
	m_iFreeBuildingThisCity(NO_BUILDINGCLASS),
	m_iFreePromotion(NO_PROMOTION),
	m_iTrainedFreePromotion(NO_PROMOTION),
	m_iFreePromotionRemoved(NO_PROMOTION),
	m_iProductionCost(0),
	m_iFaithCost(0),
	m_iNumCityCostMod(0),
	m_iHurryCostModifier(0),
	m_iNumCitiesPrereq(0),
	m_iUnitLevelPrereq(0),
	m_iCultureRateModifier(0),
	m_iGlobalCultureRateModifier(0),
	m_iGreatPeopleRateModifier(0),
	m_iGlobalGreatPeopleRateModifier(0),
	m_iGreatGeneralRateModifier(0),
	m_iGreatPersonExpendGold(0),
	m_iUnitUpgradeCostMod(0),
	m_iGoldenAgeModifier(0),
	m_iFreeExperience(0),
	m_iGlobalFreeExperience(0),
	m_iFoodKept(0),
	m_iAirlift(0),
	m_iAirModifier(0),
	m_iNukeModifier(0),
	m_iNukeExplosionRand(0),
	m_iWorkerSpeedModifier(0),
	m_iMilitaryProductionModifier(0),
	m_iSpaceProductionModifier(0),
	m_iGlobalSpaceProductionModifier(0),
	m_iMinAreaSize(0),
	m_iConquestProbability(0),
	m_iHealRateChange(0),
	m_iHappiness(0),
	m_iUnmoddedHappiness(0),
	m_iUnhappinessModifier(0),
	m_iHappinessPerCity(0),
	m_iHappinessPerXPolicies(0),
	m_iCityCountUnhappinessMod(0),
	m_bNoOccupiedUnhappiness(false),
	m_iGlobalPopulationChange(0),
	m_iTechShare(0),
	m_iFreeTechs(0),
	m_iFreePolicies(0),
	m_iFreeGreatPeople(0),
	m_iMedianTechPercentChange(0),
	m_iGold(0),
	m_bNearbyMountainRequired(false),
	m_bAllowsRangeStrike(false),
	m_iDefenseModifier(0),
	m_iGlobalDefenseModifier(0),
	m_iExtraCityHitPoints(0),
	m_iMissionType(NO_MISSION),
	m_iMinorFriendshipChange(0),
	m_iVictoryPoints(0),
	m_iExtraMissionarySpreads(0),
	m_iReligiousPressureModifier(0),
	m_iEspionageModifier(0),
	m_iGlobalEspionageModifier(0),
	m_iExtraSpies(0),
	m_iSpyRankChange(0),
	m_iInstantSpyRankChange(0),
	m_iPreferredDisplayPosition(0),
	m_iPortraitIndex(-1),
	m_bTeamShare(false),
	m_bWater(false),
	m_bRiver(false),
	m_bFreshWater(false),
	m_bMountain(false),
	m_bHill(false),
	m_bFlat(false),
	m_bFoundsReligion(false),
	m_bIsReligious(false),
	m_bBorderObstacle(false),
	m_bPlayerBorderObstacle(false),
	m_bCapital(false),
	m_bGoldenAge(false),
	m_bMapCentering(false),
	m_bNeverCapture(false),
	m_bNukeImmune(false),
	m_bExtraLuxuries(false),
	m_bDiplomaticVoting(false),
	m_bAllowsWaterRoutes(false),
	m_bCityWall(false),
	m_bUnlockedByBelief(false),
	m_bRequiresHolyCity(false),
	m_bAffectSpiesNow(false),
	m_bEspionage(false),
	m_piLockedBuildingClasses(NULL),
	m_piPrereqAndTechs(NULL),
	m_piResourceQuantityRequirements(NULL),
	m_piResourceQuantity(NULL),
	m_piResourceCultureChanges(NULL),
	m_piResourceFaithChanges(NULL),
	m_piProductionTraits(NULL),
	m_piSeaPlotYieldChange(NULL),
	m_piRiverPlotYieldChange(NULL),
	m_piLakePlotYieldChange(NULL),
	m_piSeaResourceYieldChange(NULL),
	m_piYieldChange(NULL),
	m_piYieldChangePerPop(NULL),
	m_piYieldModifier(NULL),
	m_piAreaYieldModifier(NULL),
	m_piGlobalYieldModifier(NULL),
	m_piTechEnhancedYieldChange(NULL),
	m_piUnitCombatFreeExperience(NULL),
	m_piUnitCombatProductionModifiers(NULL),
	m_piDomainFreeExperience(NULL),
	m_piDomainProductionModifier(NULL),
	m_piPrereqNumOfBuildingClass(NULL),
	m_piFlavorValue(NULL),
	m_piLocalResourceAnds(NULL),
	m_piLocalResourceOrs(NULL),
	m_paiHurryModifier(NULL),
	m_pbBuildingClassNeededInCity(NULL),
	m_piNumFreeUnits(NULL),
	m_bArtInfoEraVariation(false),
	m_bArtInfoCulturalVariation(false),
	m_bArtInfoRandomVariation(false),
	m_ppaiResourceYieldChange(NULL),
	m_ppaiFeatureYieldChange(NULL),
	m_ppaiSpecialistYieldChange(NULL),
	m_ppaiResourceYieldModifier(NULL),
	m_ppaiTerrainYieldChange(NULL),
	m_ppiBuildingClassYieldChanges(NULL),
	m_paiBuildingClassHappiness(NULL)
{
}

/// Destructor
CvBuildingEntry::~CvBuildingEntry(void)
{
	SAFE_DELETE_ARRAY(m_piLockedBuildingClasses);
	SAFE_DELETE_ARRAY(m_piPrereqAndTechs);
	SAFE_DELETE_ARRAY(m_piResourceQuantityRequirements);
	SAFE_DELETE_ARRAY(m_piResourceQuantity);
	SAFE_DELETE_ARRAY(m_piResourceCultureChanges);
	SAFE_DELETE_ARRAY(m_piResourceFaithChanges);
	SAFE_DELETE_ARRAY(m_piProductionTraits);
	SAFE_DELETE_ARRAY(m_piSeaPlotYieldChange);
	SAFE_DELETE_ARRAY(m_piRiverPlotYieldChange);
	SAFE_DELETE_ARRAY(m_piLakePlotYieldChange);
	SAFE_DELETE_ARRAY(m_piSeaResourceYieldChange);
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piYieldChangePerPop);
	SAFE_DELETE_ARRAY(m_piYieldModifier);
	SAFE_DELETE_ARRAY(m_piAreaYieldModifier);
	SAFE_DELETE_ARRAY(m_piGlobalYieldModifier);
	SAFE_DELETE_ARRAY(m_piTechEnhancedYieldChange);
	SAFE_DELETE_ARRAY(m_piUnitCombatFreeExperience);
	SAFE_DELETE_ARRAY(m_piUnitCombatProductionModifiers);
	SAFE_DELETE_ARRAY(m_piDomainFreeExperience);
	SAFE_DELETE_ARRAY(m_piDomainProductionModifier);
	SAFE_DELETE_ARRAY(m_piPrereqNumOfBuildingClass);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piLocalResourceAnds);
	SAFE_DELETE_ARRAY(m_piLocalResourceOrs);
	SAFE_DELETE_ARRAY(m_paiHurryModifier);
	SAFE_DELETE_ARRAY(m_pbBuildingClassNeededInCity);
	SAFE_DELETE_ARRAY(m_piNumFreeUnits);
	SAFE_DELETE_ARRAY(m_paiBuildingClassHappiness);

	CvDatabaseUtility::SafeDelete2DArray(m_ppaiResourceYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiFeatureYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiSpecialistYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiResourceYieldModifier);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiTerrainYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiBuildingClassYieldChanges);
}

/// Read from XML file
bool CvBuildingEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	//Basic Properties
	m_iGoldMaintenance = kResults.GetInt("GoldMaintenance");
	m_iMutuallyExclusiveGroup = kResults.GetInt("MutuallyExclusiveGroup");
	m_bTeamShare = kResults.GetBool("TeamShare");
	m_bWater = kResults.GetBool("Water");
	m_bRiver = kResults.GetBool("River");
	m_bFreshWater = kResults.GetBool("FreshWater");
	m_bMountain = kResults.GetBool("Mountain");
	m_bHill = kResults.GetBool("Hill");
	m_bFlat = kResults.GetBool("Flat");
	m_bFoundsReligion = kResults.GetBool("FoundsReligion");
	m_bIsReligious = kResults.GetBool("IsReligious");
	m_bBorderObstacle = kResults.GetBool("BorderObstacle");
	m_bPlayerBorderObstacle = kResults.GetBool("PlayerBorderObstacle");
	m_bCapital = kResults.GetBool("Capital");
	m_bGoldenAge = kResults.GetBool("GoldenAge");
	m_bMapCentering = kResults.GetBool("MapCentering");
	m_bNeverCapture = kResults.GetBool("NeverCapture");
	m_bNukeImmune = kResults.GetBool("NukeImmune");
	m_bCityWall = kResults.GetBool("CityWall");
	m_bExtraLuxuries = kResults.GetBool("ExtraLuxuries");
	m_bDiplomaticVoting = kResults.GetBool("DiplomaticVoting");
	m_bAllowsWaterRoutes = kResults.GetBool("AllowsWaterRoutes");
	m_iProductionCost = kResults.GetInt("Cost");
	m_iFaithCost = kResults.GetInt("FaithCost");
	m_bUnlockedByBelief = kResults.GetBool("UnlockedByBelief");
	m_bRequiresHolyCity = kResults.GetBool("HolyCity");
	m_bAffectSpiesNow = kResults.GetBool("AffectSpiesNow");
	m_bEspionage = kResults.GetBool("Espionage");
	m_iNumCityCostMod = kResults.GetInt("NumCityCostMod");
	m_iHurryCostModifier = kResults.GetInt("HurryCostModifier");
	m_iMinAreaSize = kResults.GetInt("MinAreaSize");
	m_iConquestProbability = kResults.GetInt("ConquestProb");
	m_iNumCitiesPrereq = kResults.GetInt("CitiesPrereq");
	m_iUnitLevelPrereq = kResults.GetInt("LevelPrereq");
	m_iCultureRateModifier = kResults.GetInt("CultureRateModifier");
	m_iGlobalCultureRateModifier = kResults.GetInt("GlobalCultureRateModifier");
	m_iGreatPeopleRateModifier = kResults.GetInt("GreatPeopleRateModifier");
	m_iGlobalGreatPeopleRateModifier = kResults.GetInt("GlobalGreatPeopleRateModifier");
	m_iGreatGeneralRateModifier = kResults.GetInt("GreatGeneralRateModifier");
	m_iGreatPersonExpendGold = kResults.GetInt("GreatPersonExpendGold");
	m_iUnitUpgradeCostMod = kResults.GetInt("UnitUpgradeCostMod");
	m_iGoldenAgeModifier = kResults.GetInt("GoldenAgeModifier");
	m_iFreeExperience = kResults.GetInt("Experience");
	m_iGlobalFreeExperience = kResults.GetInt("GlobalExperience");
	m_iFoodKept = kResults.GetInt("FoodKept");
	m_iAirlift = kResults.GetInt("Airlift");
	m_iAirModifier = kResults.GetInt("AirModifier");
	m_iNukeModifier = kResults.GetInt("NukeModifier");
	m_iNukeExplosionRand = kResults.GetInt("NukeExplosionRand");
	m_iHealRateChange = kResults.GetInt("HealRateChange");
	m_iHappiness = kResults.GetInt("Happiness");
	m_iUnmoddedHappiness = kResults.GetInt("UnmoddedHappiness");
	m_iUnhappinessModifier = kResults.GetInt("UnhappinessModifier");
	m_iHappinessPerCity = kResults.GetInt("HappinessPerCity");
	m_iHappinessPerXPolicies = kResults.GetInt("HappinessPerXPolicies");
	m_iCityCountUnhappinessMod = kResults.GetInt("CityCountUnhappinessMod");
	m_bNoOccupiedUnhappiness = kResults.GetBool("NoOccupiedUnhappiness");
	m_iWorkerSpeedModifier = kResults.GetInt("WorkerSpeedModifier");
	m_iMilitaryProductionModifier = kResults.GetInt("MilitaryProductionModifier");
	m_iSpaceProductionModifier = kResults.GetInt("SpaceProductionModifier");
	m_iGlobalSpaceProductionModifier = kResults.GetInt("GlobalSpaceProductionModifier");
	m_iBuildingProductionModifier = kResults.GetInt("BuildingProductionModifier");
	m_iWonderProductionModifier = kResults.GetInt("WonderProductionModifier");
	m_iTradeRouteModifier = kResults.GetInt("TradeRouteModifier");
	m_iCapturePlunderModifier = kResults.GetInt("CapturePlunderModifier");
	m_iPolicyCostModifier = kResults.GetInt("PolicyCostModifier");
	m_iPlotCultureCostModifier = kResults.GetInt("PlotCultureCostModifier");
	m_iGlobalPlotCultureCostModifier = kResults.GetInt("GlobalPlotCultureCostModifier");
	m_iPlotBuyCostModifier = kResults.GetInt("PlotBuyCostModifier");
	m_iGlobalPlotBuyCostModifier = kResults.GetInt("GlobalPlotBuyCostModifier");
	m_iGlobalPopulationChange = kResults.GetInt("GlobalPopulationChange");
	m_iTechShare = kResults.GetInt("TechShare");
	m_iFreeTechs = kResults.GetInt("FreeTechs");
	m_iFreePolicies = kResults.GetInt("FreePolicies");
	m_iFreeGreatPeople = kResults.GetInt("FreeGreatPeople");
	m_iMedianTechPercentChange = kResults.GetInt("MedianTechPercentChange");
	m_iGold = kResults.GetInt("Gold");
	m_bNearbyMountainRequired = kResults.GetInt("NearbyMountainRequired");
	m_bAllowsRangeStrike = kResults.GetInt("AllowsRangeStrike");
	m_iDefenseModifier = kResults.GetInt("Defense");
	m_iGlobalDefenseModifier = kResults.GetInt("GlobalDefenseMod");
	m_iExtraCityHitPoints = kResults.GetInt("ExtraCityHitPoints");
	m_iMinorFriendshipChange = kResults.GetInt("MinorFriendshipChange");
	m_iVictoryPoints = kResults.GetInt("VictoryPoints");
	m_iExtraMissionarySpreads = kResults.GetInt("ExtraMissionarySpreads");
	m_iReligiousPressureModifier = kResults.GetInt("ReligiousPressureModifier");
	m_iEspionageModifier = kResults.GetInt("EspionageModifier");
	m_iGlobalEspionageModifier = kResults.GetInt("GlobalEspionageModifier");
	m_iExtraSpies = kResults.GetInt("ExtraSpies");
	m_iSpyRankChange = kResults.GetInt("SpyRankChange");
	m_iInstantSpyRankChange = kResults.GetInt("InstantSpyRankChange");
	m_iPreferredDisplayPosition = kResults.GetInt("DisplayPosition");
	m_iPortraitIndex = kResults.GetInt("PortraitIndex");

	m_bArtInfoCulturalVariation = kResults.GetBool("ArtInfoCulturalVariation");
	m_bArtInfoEraVariation = kResults.GetBool("ArtInfoEraVariation");
	m_bArtInfoRandomVariation = kResults.GetBool("ArtInfoRandomVariation");

	//References
	const char* szTextVal;
	szTextVal = kResults.GetText("BuildingClass");
	m_iBuildingClassType = GC.getInfoTypeForString(szTextVal, true);

	//This may need to be deferred to a routine that is called AFTER pre-fetch has been called for all infos.
	m_pkBuildingClassInfo = GC.getBuildingClassInfo(static_cast<BuildingClassTypes>(m_iBuildingClassType));
	CvAssertMsg(m_pkBuildingClassInfo, "Could not find BuildingClassInfo for BuildingType. Have BuildingClasses been prefetched yet?");

	szTextVal = kResults.GetText("ArtDefineTag");
	SetArtDefineTag(szTextVal);

	szTextVal = kResults.GetText("WonderSplashAudio");
	m_strWonderSplashAudio = szTextVal;

	szTextVal = kResults.GetText("NearbyTerrainRequired");
	m_iNearbyTerrainRequired = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("ProhibitedCityTerrain");
	m_iProhibitedCityTerrain = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("VictoryPrereq");
	m_iVictoryPrereq = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("FreeStartEra");
	m_iFreeStartEra = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("MaxStartEra");
	m_iMaxStartEra = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("ObsoleteTech");
	m_iObsoleteTech = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("EnhancedYieldTech");
	m_iEnhancedYieldTech = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("FreeBuilding");
	m_iFreeBuildingClass = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("FreeBuildingThisCity");
	m_iFreeBuildingThisCity = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("FreePromotion");
	m_iFreePromotion = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("TrainedFreePromotion");
	m_iTrainedFreePromotion = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("FreePromotionRemoved");
	m_iFreePromotionRemoved = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("ReplacementBuildingClass");
	m_iReplacementBuildingClass= GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("PrereqTech");
	m_iPrereqAndTech = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("SpecialistType");
	m_iSpecialistType = GC.getInfoTypeForString(szTextVal, true);
	m_iSpecialistCount = kResults.GetInt("SpecialistCount");
	m_iSpecialistExtraCulture = kResults.GetInt("SpecialistExtraCulture");
	m_iGreatPeopleRateChange= kResults.GetInt("GreatPeopleRateChange");

	//Arrays
	const char* szBuildingType = GetType();

	kUtility.SetFlavors(m_piFlavorValue, "Building_Flavors", "BuildingType", szBuildingType);

	kUtility.SetYields(m_piSeaPlotYieldChange, "Building_SeaPlotYieldChanges", "BuildingType", szBuildingType);
	kUtility.SetYields(m_piRiverPlotYieldChange, "Building_RiverPlotYieldChanges", "BuildingType", szBuildingType);
	kUtility.SetYields(m_piLakePlotYieldChange, "Building_LakePlotYieldChanges", "BuildingType", szBuildingType);
	kUtility.SetYields(m_piSeaResourceYieldChange, "Building_SeaResourceYieldChanges", "BuildingType", szBuildingType);
	kUtility.SetYields(m_piYieldChange, "Building_YieldChanges", "BuildingType", szBuildingType);
	kUtility.SetYields(m_piYieldChangePerPop, "Building_YieldChangesPerPop", "BuildingType", szBuildingType);
	kUtility.SetYields(m_piYieldModifier, "Building_YieldModifiers", "BuildingType", szBuildingType);
	kUtility.SetYields(m_piAreaYieldModifier, "Building_AreaYieldModifiers", "BuildingType", szBuildingType);
	kUtility.SetYields(m_piGlobalYieldModifier, "Building_GlobalYieldModifiers", "BuildingType", szBuildingType);
	kUtility.SetYields(m_piTechEnhancedYieldChange, "Building_TechEnhancedYieldChanges", "BuildingType", szBuildingType);

	kUtility.PopulateArrayByValue(m_piResourceQuantityRequirements, "Resources", "Building_ResourceQuantityRequirements", "ResourceType", "BuildingType", szBuildingType, "Cost");
	kUtility.PopulateArrayByValue(m_piResourceQuantity, "Resources", "Building_ResourceQuantity", "ResourceType", "BuildingType", szBuildingType, "Quantity");
	kUtility.PopulateArrayByValue(m_piResourceCultureChanges, "Resources", "Building_ResourceCultureChanges", "ResourceType", "BuildingType", szBuildingType, "CultureChange");
	kUtility.PopulateArrayByValue(m_piResourceFaithChanges, "Resources", "Building_ResourceFaithChanges", "ResourceType", "BuildingType", szBuildingType, "FaithChange");

	kUtility.PopulateArrayByValue(m_paiHurryModifier, "HurryInfos", "Building_HurryModifiers", "HurryType", "BuildingType", szBuildingType, "HurryCostModifier");

	//kUtility.PopulateArrayByValue(m_piProductionTraits, "Traits", "Building_ProductionTraits", "TraitType", "BuildingType", szBuildingType, "Trait");

	kUtility.PopulateArrayByValue(m_piUnitCombatFreeExperience, "UnitCombatInfos", "Building_UnitCombatFreeExperiences", "UnitCombatType", "BuildingType", szBuildingType, "Experience");
	kUtility.PopulateArrayByValue(m_piUnitCombatProductionModifiers, "UnitCombatInfos", "Building_UnitCombatProductionModifiers", "UnitCombatType", "BuildingType", szBuildingType, "Modifier");
	kUtility.PopulateArrayByValue(m_piDomainFreeExperience, "Domains", "Building_DomainFreeExperiences", "DomainType", "BuildingType", szBuildingType, "Experience", 0, NUM_DOMAIN_TYPES);
	kUtility.PopulateArrayByValue(m_piDomainProductionModifier, "Domains", "Building_DomainProductionModifiers", "DomainType", "BuildingType", szBuildingType, "Modifier", 0, NUM_DOMAIN_TYPES);

	kUtility.PopulateArrayByValue(m_piPrereqNumOfBuildingClass, "BuildingClasses", "Building_PrereqBuildingClasses", "BuildingClassType", "BuildingType", szBuildingType, "NumBuildingNeeded");
	kUtility.PopulateArrayByExistence(m_pbBuildingClassNeededInCity, "BuildingClasses", "Building_ClassesNeededInCity", "BuildingClassType", "BuildingType", szBuildingType);
	//kUtility.PopulateArrayByExistence(m_piNumFreeUnits, "Units", "Building_FreeUnits", "UnitType", "BuildingType", szBuildingType);
	kUtility.PopulateArrayByValue(m_piNumFreeUnits, "Units", "Building_FreeUnits", "UnitType", "BuildingType", szBuildingType, "NumUnits");
	kUtility.PopulateArrayByValue(m_paiBuildingClassHappiness, "BuildingClasses", "Building_BuildingClassHappiness", "BuildingClassType", "BuildingType", szBuildingType, "Happiness");

	kUtility.PopulateArrayByExistence(m_piLockedBuildingClasses, "BuildingClasses", "Building_LockedBuildingClasses", "BuildingClassType", "BuildingType", szBuildingType);
	kUtility.PopulateArrayByExistence(m_piPrereqAndTechs, "Technologies", "Building_TechAndPrereqs", "TechType", "BuildingType", szBuildingType);
	kUtility.PopulateArrayByExistence(m_piLocalResourceAnds, "Resources", "Building_LocalResourceAnds", "ResourceType", "BuildingType", szBuildingType);
	kUtility.PopulateArrayByExistence(m_piLocalResourceOrs, "Resources", "Building_LocalResourceOrs", "ResourceType", "BuildingType", szBuildingType);

	//ResourceYieldChanges
	{
		kUtility.Initialize2DArray(m_ppaiResourceYieldChange, "Resources", "Yields");

		std::string strKey("Building_ResourceYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Resources.ID as ResourceID, Yields.ID as YieldID, Yield from Building_ResourceYieldChanges inner join Resources on Resources.Type = ResourceType inner join Yields on Yields.Type = YieldType where BuildingType = ?");
		}

		pResults->Bind(1, szBuildingType);

		while(pResults->Step())
		{
			const int ResourceID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppaiResourceYieldChange[ResourceID][YieldID] = yield;
		}
	}

	//FeatureYieldChanges
	{
		kUtility.Initialize2DArray(m_ppaiFeatureYieldChange, "Features", "Yields");

		std::string strKey("Building_FeatureYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Features.ID as FeatureID, Yields.ID as YieldID, Yield from Building_FeatureYieldChanges inner join Features on Features.Type = FeatureType inner join Yields on Yields.Type = YieldType where BuildingType = ?");
		}

		pResults->Bind(1, szBuildingType);

		while(pResults->Step())
		{
			const int FeatureID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppaiFeatureYieldChange[FeatureID][YieldID] = yield;
		}
	}

	//TerrainYieldChanges
	{
		kUtility.Initialize2DArray(m_ppaiTerrainYieldChange, "Terrains", "Yields");

		std::string strKey("Building_TerrainYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Terrains.ID as TerrainID, Yields.ID as YieldID, Yield from Building_TerrainYieldChanges inner join Terrains on Terrains.Type = TerrainType inner join Yields on Yields.Type = YieldType where BuildingType = ?");
		}

		pResults->Bind(1, szBuildingType);

		while(pResults->Step())
		{
			const int TerrainID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppaiTerrainYieldChange[TerrainID][YieldID] = yield;
		}
	}

	//SpecialistYieldChanges
	{
		kUtility.Initialize2DArray(m_ppaiSpecialistYieldChange, "Specialists", "Yields");

		std::string strKey("Building_SpecialistYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Specialists.ID as SpecialistID, Yields.ID as YieldID, Yield from Building_SpecialistYieldChanges inner join Specialists on Specialists.Type = SpecialistType inner join Yields on Yields.Type = YieldType where BuildingType = ?");
		}

		pResults->Bind(1, szBuildingType);

		while(pResults->Step())
		{
			const int SpecialistID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppaiSpecialistYieldChange[SpecialistID][YieldID] = yield;
		}
	}

	//ResourceYieldModifiers
	{
		kUtility.Initialize2DArray(m_ppaiResourceYieldModifier, "Resources", "Yields");

		std::string strKey("Building_ResourceYieldModifiers");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Resources.ID as ResourceID, Yields.ID as YieldID, Yield from Building_ResourceYieldModifiers inner join Resources on Resources.Type = ResourceType inner join Yields on Yields.Type = YieldType where BuildingType = ?");
		}

		pResults->Bind(1, szBuildingType);

		while(pResults->Step())
		{
			const int ResourceID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppaiResourceYieldModifier[ResourceID][YieldID] = yield;
		}
	}

	//BuildingClassYieldChanges
	{
		kUtility.Initialize2DArray(m_ppiBuildingClassYieldChanges, "BuildingClasses", "Yields");

		std::string strKey("Building_BuildingClassYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select BuildingClasses.ID as BuildingClassID, Yields.ID as YieldID, YieldChange from Building_BuildingClassYieldChanges inner join BuildingClasses on BuildingClasses.Type = BuildingClassType inner join Yields on Yields.Type = YieldType where BuildingType = ?");
		}

		pResults->Bind(1, szBuildingType);

		while(pResults->Step())
		{
			const int BuildingClassID = pResults->GetInt(0);
			const int iYieldID = pResults->GetInt(1);
			const int iYieldChange = pResults->GetInt(2);

			m_ppiBuildingClassYieldChanges[BuildingClassID][iYieldID] = iYieldChange;
		}
	}

	return true;
}

/// Class of this building
int CvBuildingEntry::GetBuildingClassType() const
{
	return m_iBuildingClassType;
}

const CvBuildingClassInfo& CvBuildingEntry::GetBuildingClassInfo() const
{
	if(m_pkBuildingClassInfo == NULL)
	{
		const char* szError = "ERROR: Building does not contain valid BuildingClass type!!";
		GC.LogMessage(szError);
		CvAssertMsg(false, szError);
	}

#pragma warning ( push )
#pragma warning ( disable : 6011 ) // Dereferencing NULL pointer
	return *m_pkBuildingClassInfo;
#pragma warning ( pop )
}

/// Does this building require a city built on or next to a specific terrain type?
int CvBuildingEntry::GetNearbyTerrainRequired() const
{
	return m_iNearbyTerrainRequired;
}

/// Does this building need the absence of a terrain under the city?
int CvBuildingEntry::GetProhibitedCityTerrain() const
{
	return m_iProhibitedCityTerrain;
}

/// Does a Victory need to be active for this building to be buildable?
int CvBuildingEntry::GetVictoryPrereq() const
{
	return m_iVictoryPrereq;
}

/// Do you get this building for free if start in a later era?
int CvBuildingEntry::GetFreeStartEra() const
{
	return m_iFreeStartEra;
}

/// Is this building unbuildable if start in a later era?
int CvBuildingEntry::GetMaxStartEra() const
{
	return m_iMaxStartEra;
}

/// Tech that makes this building obsolete
int CvBuildingEntry::GetObsoleteTech() const
{
	return m_iObsoleteTech;
}

/// Tech that improves the yield from this building
int CvBuildingEntry::GetEnhancedYieldTech() const
{
	return m_iEnhancedYieldTech;
}

/// How much GPT does this Building cost?
int CvBuildingEntry::GetGoldMaintenance() const
{
	return m_iGoldMaintenance;
}

/// Only one Building from each Group may be constructed in a City
int CvBuildingEntry::GetMutuallyExclusiveGroup() const
{
	return m_iMutuallyExclusiveGroup;
}

/// Upgraded version of this building
int CvBuildingEntry::GetReplacementBuildingClass() const
{
	return m_iReplacementBuildingClass;
}

/// Techs required for this building
int CvBuildingEntry::GetPrereqAndTech() const
{
	return m_iPrereqAndTech;
}

/// What SpecialistType is allowed by this Building
int CvBuildingEntry::GetSpecialistType() const
{
	return m_iSpecialistType;
}

/// How many SpecialistTypes are allowed by this Building
int CvBuildingEntry::GetSpecialistCount() const
{
	return m_iSpecialistCount;
}

/// Extra culture from every specialist
int CvBuildingEntry::GetSpecialistExtraCulture() const
{
	return m_iSpecialistExtraCulture;
}

/// How many GPP does this Building provide (linked to the SpecialistType)
int CvBuildingEntry::GetGreatPeopleRateChange() const
{
	return m_iGreatPeopleRateChange;
}

/// Free building in each city from this building/wonder
int CvBuildingEntry::GetFreeBuildingClass() const
{
	return m_iFreeBuildingClass;
}

/// Free building in the city that builds this building/wonder
int CvBuildingEntry::GetFreeBuildingThisCity() const
{
	return m_iFreeBuildingThisCity;
}

/// Does this building give all units a promotion for free instantly?
int CvBuildingEntry::GetFreePromotion() const
{
	return m_iFreePromotion;
}

/// Does this building give units a promotion when trained from this city?
int CvBuildingEntry::GetTrainedFreePromotion() const
{
	return m_iTrainedFreePromotion;
}

/// Does this building get rid of an undesirable promotion?
int CvBuildingEntry::GetFreePromotionRemoved() const
{
	return m_iFreePromotionRemoved;
}

/// Shields to construct the building
int CvBuildingEntry::GetProductionCost() const
{
	return m_iProductionCost;
}

/// Faith to construct the unit (as a percentage of cost of next Great Prophet)
int CvBuildingEntry::GetFaithCost() const
{
	return m_iFaithCost;
}

/// Additional cost based on the number of cities in the empire
int CvBuildingEntry::GetNumCityCostMod() const
{
	return m_iNumCityCostMod;
}

/// Does this Building modify any hurry costs
int CvBuildingEntry::GetHurryCostModifier() const
{
	return m_iHurryCostModifier;
}

/// Number of cities required to build this?
int CvBuildingEntry::GetNumCitiesPrereq() const
{
	return m_iNumCitiesPrereq;
}

/// Do we need a unit at a certain level to build this?
int CvBuildingEntry::GetUnitLevelPrereq() const
{
	return m_iUnitLevelPrereq;
}

/// Multiplier to the rate of accumulating culture for policies
int CvBuildingEntry::GetCultureRateModifier() const
{
	return m_iCultureRateModifier;
}

/// Multiplier to the rate of accumulating culture for policies in all Cities
int CvBuildingEntry::GetGlobalCultureRateModifier() const
{
	return m_iGlobalCultureRateModifier;
}

/// Change in spawn rate for great people
int CvBuildingEntry::GetGreatPeopleRateModifier() const
{
	return m_iGreatPeopleRateModifier;
}

/// Change global spawn rate for great people
int CvBuildingEntry::GetGlobalGreatPeopleRateModifier() const
{
	return m_iGlobalGreatPeopleRateModifier;
}

/// Change in spawn rate for great generals
int CvBuildingEntry::GetGreatGeneralRateModifier() const
{
	return m_iGreatGeneralRateModifier;
}

/// Gold received when great person expended
int CvBuildingEntry::GetGreatPersonExpendGold() const
{
	return m_iGreatPersonExpendGold;
}

/// Reduces cost of unit upgrades?
int CvBuildingEntry::GetUnitUpgradeCostMod() const
{
	return m_iUnitUpgradeCostMod;
}

/// Percentage increase in the length of Golden Ages
int CvBuildingEntry::GetGoldenAgeModifier() const
{
	return m_iGoldenAgeModifier;
}

/// Free experience for units built in this city
int CvBuildingEntry::GetFreeExperience() const
{
	return m_iFreeExperience;
}

/// Free experience for all player units
int CvBuildingEntry::GetGlobalFreeExperience() const
{
	return m_iGlobalFreeExperience;
}

/// Percentage of food retained after city growth
int CvBuildingEntry::GetFoodKept() const
{
	return m_iFoodKept;
}

/// Does this building allow airlifts?
int CvBuildingEntry::GetAirlift() const
{
	return m_iAirlift;
}

/// Modifier to city air defense
int CvBuildingEntry::GetAirModifier() const
{
	return m_iAirModifier;
}

/// Modifier to city nuke defense
int CvBuildingEntry::GetNukeModifier() const
{
	return m_iNukeModifier;
}

/// Will this building cause a big problem (meltdown) if the city is hit with a nuke?
int CvBuildingEntry::GetNukeExplosionRand() const
{
	return m_iNukeExplosionRand;
}

/// Improvement in worker speed
int CvBuildingEntry::GetWorkerSpeedModifier() const
{
	return m_iWorkerSpeedModifier;
}

/// Improvement in military unit production
int CvBuildingEntry::GetMilitaryProductionModifier() const
{
	return m_iMilitaryProductionModifier;
}

/// Improvement in space race component production
int CvBuildingEntry::GetSpaceProductionModifier() const
{
	return m_iSpaceProductionModifier;
}

/// Improvement in space race component production in all cities
int CvBuildingEntry::GetGlobalSpaceProductionModifier() const
{
	return m_iGlobalSpaceProductionModifier;
}

/// Improvement in building production
int CvBuildingEntry::GetBuildingProductionModifier() const
{
	return m_iBuildingProductionModifier;
}

/// Improvement in wonder production
int CvBuildingEntry::GetWonderProductionModifier() const
{
	return m_iWonderProductionModifier;
}

/// Trade route gold modifier
int CvBuildingEntry::GetTradeRouteModifier() const
{
	return m_iTradeRouteModifier;
}

/// Increased plunder if city captured
int CvBuildingEntry::GetCapturePlunderModifier() const
{
	return m_iCapturePlunderModifier;
}

/// Change in culture cost to earn a new policy
int CvBuildingEntry::GetPolicyCostModifier() const
{
	return m_iPolicyCostModifier;
}

/// Change in culture cost to earn a new tile
int CvBuildingEntry::GetPlotCultureCostModifier() const
{
	return m_iPlotCultureCostModifier;
}

/// Change in culture cost to earn a new tile
int CvBuildingEntry::GetGlobalPlotCultureCostModifier() const
{
	return m_iGlobalPlotCultureCostModifier;
}

/// Change in gold cost to earn a new tile
int CvBuildingEntry::GetPlotBuyCostModifier() const
{
	return m_iPlotBuyCostModifier;
}

/// Change in gold cost to earn a new tile across the empire
int CvBuildingEntry::GetGlobalPlotBuyCostModifier() const
{
	return m_iGlobalPlotBuyCostModifier;
}

/// Required Plot count of the CvArea this City belongs to (Usually used for Water Buildings to prevent Harbors in tiny lakes and such)
int CvBuildingEntry::GetMinAreaSize() const
{
	return m_iMinAreaSize;
}

/// Chance of building surviving after conquest
int CvBuildingEntry::GetConquestProbability() const
{
	return m_iConquestProbability;
}

/// Improvement in unit heal rate from this building
int CvBuildingEntry::GetHealRateChange() const
{
	return m_iHealRateChange;
}

/// Happiness provided by this building
int CvBuildingEntry::GetHappiness() const
{
	return m_iHappiness;
}

/// UnmoddedHappiness provided by this building - NOT affected by a city's pop
int CvBuildingEntry::GetUnmoddedHappiness() const
{
	return m_iUnmoddedHappiness;
}

/// Get percentage modifier to overall player happiness
int CvBuildingEntry::GetUnhappinessModifier() const
{
	return m_iUnhappinessModifier;
}

/// HappinessPerCity provided by this building
int CvBuildingEntry::GetHappinessPerCity() const
{
	return m_iHappinessPerCity;
}

/// Happiness per X number of Policies provided by this building
int CvBuildingEntry::GetHappinessPerXPolicies() const
{
	return m_iHappinessPerXPolicies;
}

/// CityCountUnhappinessMod provided by this building
int CvBuildingEntry::GetCityCountUnhappinessMod() const
{
	return m_iCityCountUnhappinessMod;
}

/// NoOccupiedUnhappiness
bool CvBuildingEntry::IsNoOccupiedUnhappiness() const
{
	return m_bNoOccupiedUnhappiness;
}

/// Population added to every City in the player's empire
int CvBuildingEntry::GetGlobalPopulationChange() const
{
	return m_iGlobalPopulationChange;
}

/// If this # of players have a Tech then the owner of this Building gets that Tech as well
int CvBuildingEntry::GetTechShare() const
{
	return m_iTechShare;
}

/// Number of free techs granted by this building
int CvBuildingEntry::GetFreeTechs() const
{
	return m_iFreeTechs;
}

/// Number of free Policies granted by this building
int CvBuildingEntry::GetFreePolicies() const
{
	return m_iFreePolicies;
}

/// Number of free Great People granted by this building
int CvBuildingEntry::GetFreeGreatPeople() const
{
	return m_iFreeGreatPeople;
}

/// Boost to median tech received from research agreements
int CvBuildingEntry::GetMedianTechPercentChange() const
{
	return m_iMedianTechPercentChange;
}

/// Gold generated by this building
int CvBuildingEntry::GetGold() const
{
	return m_iGold;
}

/// Does a city need to be near a mountain to build this?
bool CvBuildingEntry::IsNearbyMountainRequired() const
{
	return m_bNearbyMountainRequired;
}

/// Does this Building allow us to Range Strike?
bool CvBuildingEntry::IsAllowsRangeStrike() const
{
	return m_bAllowsRangeStrike;
}

/// Modifier to city defense
int CvBuildingEntry::GetDefenseModifier() const
{
	return m_iDefenseModifier;
}

/// Modifier to every City's Building defense
int CvBuildingEntry::GetGlobalDefenseModifier() const
{
	return m_iGlobalDefenseModifier;
}

/// Modifier to city's hit points
int CvBuildingEntry::GetExtraCityHitPoints() const
{
	return m_iExtraCityHitPoints;
}

/// Instant Friendship mod change with City States
int CvBuildingEntry::GetMinorFriendshipChange() const
{
	return m_iMinorFriendshipChange;
}

/// VPs added to overall Team score
int CvBuildingEntry::GetVictoryPoints() const
{
	return m_iVictoryPoints;
}

/// Extra religion spreads from missionaries built in this city
int CvBuildingEntry::GetExtraMissionarySpreads() const
{
	return m_iExtraMissionarySpreads;
}

/// Extra religion pressure emanating from this city
int CvBuildingEntry::GetReligiousPressureModifier() const
{
	return m_iReligiousPressureModifier;
}

/// Modifier to chance of espionage against this city
int CvBuildingEntry::GetEspionageModifier() const
{
	return m_iEspionageModifier;
}

/// Modifier to chance of espionage against all cities
int CvBuildingEntry::GetGlobalEspionageModifier() const
{
	return m_iGlobalEspionageModifier;
}

/// Extra spies after this is built
int CvBuildingEntry::GetExtraSpies() const
{
	return m_iExtraSpies;
}

/// Increase in rank of all starting spies
int CvBuildingEntry::GetSpyRankChange() const
{
	return m_iSpyRankChange;
}

/// One-time boost for all existing spies
int CvBuildingEntry::GetInstantSpyRankChange() const
{
	return m_iInstantSpyRankChange;
}

/// wWhat ring the engine will try to display this building
int CvBuildingEntry::GetPreferredDisplayPosition() const
{
	return m_iPreferredDisplayPosition;
}

/// index of portrait in the texture sheet
int CvBuildingEntry::GetPortraitIndex() const
{
	return m_iPortraitIndex;
}

/// Is the presence of this building shared with team allies?
bool CvBuildingEntry::IsTeamShare() const
{
	return m_bTeamShare;
}

/// Must this be built in a coastal city?
bool CvBuildingEntry::IsWater() const
{
	return m_bWater;
}

/// Must this be built in a river city?
bool CvBuildingEntry::IsRiver() const
{
	return m_bRiver;
}

/// Must this be built in a city next to FreshWater?
bool CvBuildingEntry::IsFreshWater() const
{
	return m_bFreshWater;
}

/// Must this be built in a city next to Mountain?
bool CvBuildingEntry::IsMountain() const
{
	return m_bMountain;
}

/// Must this be built in a city on a hill?
bool CvBuildingEntry::IsHill() const
{
	return m_bHill;
}

/// Must this be built in a city on Flat ground?
bool CvBuildingEntry::IsFlat() const
{
	return m_bFlat;
}

/// Does this Building Found a Religion?
bool CvBuildingEntry::IsFoundsReligion() const
{
	return m_bFoundsReligion;
}

/// Is this a "Religous" Building? (qualifies it for Production bonuses for Policies, etc.)
bool CvBuildingEntry::IsReligious() const
{
	return m_bIsReligious;
}

/// Is this an obstacle at the edge of your empire (e.g. Great Wall) -- for you AND your teammates
bool CvBuildingEntry::IsBorderObstacle() const
{
	return m_bBorderObstacle;
}

/// Is this an obstacle at the edge of your empire (e.g. Great Wall) -- for just the owning player
bool CvBuildingEntry::IsPlayerBorderObstacle() const
{
	return m_bPlayerBorderObstacle;
}

/// Does this trigger drawing a wall around the city
bool CvBuildingEntry::IsCityWall() const
{
	return m_bCityWall;
}

/// Is this building unlocked through religion?
bool CvBuildingEntry::IsUnlockedByBelief() const
{
	return m_bUnlockedByBelief;
}

/// Does it have to be built in the Holy City?
bool CvBuildingEntry::IsRequiresHolyCity() const
{
	return m_bRequiresHolyCity;
}

/// Does this building affect spy rates when it is built?
bool CvBuildingEntry::AffectSpiesNow() const
{
	return m_bAffectSpiesNow;
}

// Is this an espionage building that should be disabled when espionage is disabled?
bool CvBuildingEntry::IsEspionage() const
{
	return m_bEspionage;
}

/// Does this building define the capital?
bool CvBuildingEntry::IsCapital() const
{
	return m_bCapital;
}

/// Does this building spawn a golden age?
bool CvBuildingEntry::IsGoldenAge() const
{
	return m_bGoldenAge;
}

/// Is the map centered after this building is constructed?
bool CvBuildingEntry::IsMapCentering() const
{
	return m_bMapCentering;
}

/// Can this building never be captured?
bool CvBuildingEntry::IsNeverCapture() const
{
	return m_bNeverCapture;
}

/// Is the building immune to nukes?
bool CvBuildingEntry::IsNukeImmune() const
{
	return m_bNukeImmune;
}

/// Does the building add an additional of each luxury in city radius
bool CvBuildingEntry::IsExtraLuxuries() const
{
	return m_bExtraLuxuries;
}

/// Begins voting for the diplo victory?
bool CvBuildingEntry::IsDiplomaticVoting() const
{
	return m_bDiplomaticVoting;
}

/// Does the building allow routes over the water
bool CvBuildingEntry::AllowsWaterRoutes() const
{
	return m_bAllowsWaterRoutes;
}

/// Derive property: is this considered a science building?
bool CvBuildingEntry::IsScienceBuilding() const
{
	bool bRtnValue = false;

	if(IsCapital())
	{
		bRtnValue = false;
	}
	else if(GetYieldChange(YIELD_SCIENCE) > 0)
	{
		bRtnValue = true;
	}
	else if(GetYieldChangePerPop(YIELD_SCIENCE) > 0)
	{
		bRtnValue = true;
	}
	else if(GetTechEnhancedYieldChange(YIELD_SCIENCE) > 0)
	{
		bRtnValue = true;
	}
	else if(GetYieldModifier(YIELD_SCIENCE) > 0)
	{
		bRtnValue = true;
	}

	return bRtnValue;
}

/// Retrieve art tag
const char* CvBuildingEntry::GetArtDefineTag() const
{
	return m_strArtDefineTag.c_str();
}

/// Set art tag
void CvBuildingEntry::SetArtDefineTag(const char* szVal)
{
	m_strArtDefineTag = szVal;
}

/// Return whether we should try to find a culture specific variant art tag
const bool CvBuildingEntry::GetArtInfoCulturalVariation() const
{
	return m_bArtInfoCulturalVariation;
}

/// Return whether we should try to find an era specific variant art tag
const bool CvBuildingEntry::GetArtInfoEraVariation() const
{
	return m_bArtInfoEraVariation;
}

/// Return whether we should try to find an era specific variant art tag
const bool CvBuildingEntry::GetArtInfoRandomVariation() const
{
	return m_bArtInfoRandomVariation;
}



const char* CvBuildingEntry::GetWonderSplashAudio() const
{
	return m_strWonderSplashAudio.c_str();
}



// ARRAYS

/// Change to yield by type
int CvBuildingEntry::GetYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChange ? m_piYieldChange[i] : -1;
}

/// Array of yield changes
int* CvBuildingEntry::GetYieldChangeArray() const
{
	return m_piYieldChange;
}

/// Change to yield by type
int CvBuildingEntry::GetYieldChangePerPop(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChangePerPop ? m_piYieldChangePerPop[i] : -1;
}

/// Array of yield changes
int* CvBuildingEntry::GetYieldChangePerPopArray() const
{
	return m_piYieldChangePerPop;
}

/// Modifier to yield by type
int CvBuildingEntry::GetYieldModifier(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldModifier ? m_piYieldModifier[i] : -1;
}

/// Array of yield modifiers
int* CvBuildingEntry::GetYieldModifierArray() const
{
	return m_piYieldModifier;
}

/// Modifier to yield by type in area
int CvBuildingEntry::GetAreaYieldModifier(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piAreaYieldModifier ? m_piAreaYieldModifier[i] : -1;
}

/// Array of yield modifiers in area
int* CvBuildingEntry::GetAreaYieldModifierArray() const
{
	return m_piAreaYieldModifier;
}

/// Global modifier to yield by type
int CvBuildingEntry::GetGlobalYieldModifier(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piGlobalYieldModifier ? m_piGlobalYieldModifier[i] : -1;
}

/// Array of global yield modifiers
int* CvBuildingEntry::GetGlobalYieldModifierArray() const
{
	return m_piGlobalYieldModifier;
}

/// Change to yield based on earning a tech
int CvBuildingEntry::GetTechEnhancedYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piTechEnhancedYieldChange ? m_piTechEnhancedYieldChange[i] : -1;
}

/// Array of yield changes based on earning a tech
int* CvBuildingEntry::GetTechEnhancedYieldChangeArray() const
{
	return m_piTechEnhancedYieldChange;
}

/// Sea plot yield changes by type
int CvBuildingEntry::GetSeaPlotYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piSeaPlotYieldChange ? m_piSeaPlotYieldChange[i] : -1;
}

/// Array of sea plot yield changes
int* CvBuildingEntry::GetSeaPlotYieldChangeArray() const
{
	return m_piSeaPlotYieldChange;
}

/// River plot yield changes by type
int CvBuildingEntry::GetRiverPlotYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piRiverPlotYieldChange ? m_piRiverPlotYieldChange[i] : -1;
}

/// Array of river plot yield changes
int* CvBuildingEntry::GetRiverPlotYieldChangeArray() const
{
	return m_piRiverPlotYieldChange;
}

/// Lake plot yield changes by type
int CvBuildingEntry::GetLakePlotYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piLakePlotYieldChange ? m_piLakePlotYieldChange[i] : -1;
}

/// Array of lake plot yield changes
int* CvBuildingEntry::GetLakePlotYieldChangeArray() const
{
	return m_piLakePlotYieldChange;
}

/// Sea resource yield changes by type
int CvBuildingEntry::GetSeaResourceYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piSeaResourceYieldChange ? m_piSeaResourceYieldChange[i] : -1;
}

/// Array of sea resource yield changes
int* CvBuildingEntry::GetSeaResourceYieldChangeArray() const
{
	return m_piSeaResourceYieldChange;
}

/// Free combat experience by unit combat type
int CvBuildingEntry::GetUnitCombatFreeExperience(int i) const
{
	CvAssertMsg(i < GC.getNumUnitCombatClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piUnitCombatFreeExperience ? m_piUnitCombatFreeExperience[i] : -1;
}

/// Free combat experience by unit combat type
int CvBuildingEntry::GetUnitCombatProductionModifier(int i) const
{
	CvAssertMsg(i < GC.getNumUnitCombatClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piUnitCombatProductionModifiers ? m_piUnitCombatProductionModifiers[i] : -1;
}

/// Free experience gained for units in this domain
int CvBuildingEntry::GetDomainFreeExperience(int i) const
{
	CvAssertMsg(i < NUM_DOMAIN_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piDomainFreeExperience ? m_piDomainFreeExperience[i] : -1;
}

/// Production modifier in this domain
int CvBuildingEntry::GetDomainProductionModifier(int i) const
{
	CvAssertMsg(i < NUM_DOMAIN_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piDomainProductionModifier ? m_piDomainProductionModifier[i] : -1;
}

/// BuildingClasses that may no longer be constructed after this Building is built in a City
int CvBuildingEntry::GetLockedBuildingClasses(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piLockedBuildingClasses ? m_piLockedBuildingClasses[i] : -1;
}

/// Prerequisite techs with AND
int CvBuildingEntry::GetPrereqAndTechs(int i) const
{
	CvAssertMsg(i < GC.getNUM_BUILDING_AND_TECH_PREREQS(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piPrereqAndTechs ? m_piPrereqAndTechs[i] : -1;
}

/// Resources consumed to construct
int CvBuildingEntry::GetResourceQuantityRequirement(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piResourceQuantityRequirements ? m_piResourceQuantityRequirements[i] : -1;
}

/// Resources provided once constructed
int CvBuildingEntry::GetResourceQuantity(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piResourceQuantity ? m_piResourceQuantity[i] : -1;
}

/// Boost in Culture for each of these Resources
int CvBuildingEntry::GetResourceCultureChange(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piResourceCultureChanges ? m_piResourceCultureChanges[i] : -1;
}

/// Boost in Faith for each of these Resources
int CvBuildingEntry::GetResourceFaithChange(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piResourceFaithChanges ? m_piResourceFaithChanges[i] : -1;
}

/// Boost in production for leader with this trait
int CvBuildingEntry::GetProductionTraits(int i) const
{
	CvAssertMsg(i < GC.getNumTraitInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piProductionTraits ? m_piProductionTraits[i] : 0;
}

/// Number of prerequisite buildings of a particular class
int CvBuildingEntry::GetPrereqNumOfBuildingClass(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piPrereqNumOfBuildingClass ? m_piPrereqNumOfBuildingClass[i] : -1;
}

/// Find value of flavors associated with this building
int CvBuildingEntry::GetFlavorValue(int i) const
{
	CvAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}

/// Prerequisite resources with AND
int CvBuildingEntry::GetLocalResourceAnd(int i) const
{
	CvAssertMsg(i < GC.getNUM_BUILDING_RESOURCE_PREREQS(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piLocalResourceAnds ? m_piLocalResourceAnds[i] : -1;
}

/// Prerequisite resources with OR
int CvBuildingEntry::GetLocalResourceOr(int i) const
{
	CvAssertMsg(i < GC.getNUM_BUILDING_RESOURCE_PREREQS(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piLocalResourceOrs ? m_piLocalResourceOrs[i] : -1;
}

/// Modifier to Hurry cost
int CvBuildingEntry::GetHurryModifier(int i) const
{
	CvAssertMsg(i < GC.getNumHurryInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiHurryModifier ? m_paiHurryModifier[i] : -1;
}

/// Can it only built if there is a building of this class in the city?
bool CvBuildingEntry::IsBuildingClassNeededInCity(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbBuildingClassNeededInCity ? m_pbBuildingClassNeededInCity[i] : false;
}

/// Free units which appear near the capital
int CvBuildingEntry::GetNumFreeUnits(int i) const
{
	CvAssertMsg(i < GC.getNumUnitInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piNumFreeUnits ? m_piNumFreeUnits[i] : -1;
}

/// Change to Resource yield by type
int CvBuildingEntry::GetResourceYieldChange(int i, int j) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppaiResourceYieldChange ? m_ppaiResourceYieldChange[i][j] : -1;
}

/// Array of changes to Resource yield
int* CvBuildingEntry::GetResourceYieldChangeArray(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_ppaiResourceYieldChange[i];
}

/// Change to Feature yield by type
int CvBuildingEntry::GetFeatureYieldChange(int i, int j) const
{
	CvAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppaiFeatureYieldChange ? m_ppaiFeatureYieldChange[i][j] : -1;
}

/// Array of changes to Feature yield
int* CvBuildingEntry::GetFeatureYieldChangeArray(int i) const
{
	CvAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_ppaiFeatureYieldChange[i];
}

/// Change to specialist yield by type
int CvBuildingEntry::GetSpecialistYieldChange(int i, int j) const
{
	CvAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppaiSpecialistYieldChange ? m_ppaiSpecialistYieldChange[i][j] : -1;
}

/// Array of changes to specialist yield
int* CvBuildingEntry::GetSpecialistYieldChangeArray(int i) const
{
	CvAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_ppaiSpecialistYieldChange[i];
}

/// Modifier to resource yield
int CvBuildingEntry::GetResourceYieldModifier(int i, int j) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppaiResourceYieldModifier ? m_ppaiResourceYieldModifier[i][j] : -1;
}

/// Array of modifiers to resource yield
int* CvBuildingEntry::GetResourceYieldModifierArray(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_ppaiResourceYieldModifier[i];
}

/// Change to Terrain yield by type
int CvBuildingEntry::GetTerrainYieldChange(int i, int j) const
{
	CvAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppaiTerrainYieldChange ? m_ppaiTerrainYieldChange[i][j] : -1;
}

/// Array of changes to Feature yield
int* CvBuildingEntry::GetTerrainYieldChangeArray(int i) const
{
	CvAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_ppaiTerrainYieldChange[i];
}

/// Yield change for a specific BuildingClass by yield type
int CvBuildingEntry::GetBuildingClassYieldChange(int i, int j) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiBuildingClassYieldChanges[i][j];
}

/// Amount of extra Happiness per turn a BuildingClass provides
int CvBuildingEntry::GetBuildingClassHappiness(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paiBuildingClassHappiness ? m_paiBuildingClassHappiness[i] : -1;
}

//=====================================
// CvBuildingXMLEntries
//=====================================
/// Constructor
CvBuildingXMLEntries::CvBuildingXMLEntries(void)
{

}

/// Destructor
CvBuildingXMLEntries::~CvBuildingXMLEntries(void)
{
	DeleteArray();
}

/// Returns vector of policy entries
std::vector<CvBuildingEntry*>& CvBuildingXMLEntries::GetBuildingEntries()
{
	return m_paBuildingEntries;
}

/// Number of defined policies
int CvBuildingXMLEntries::GetNumBuildings()
{
	return m_paBuildingEntries.size();
}

/// Clear policy entries
void CvBuildingXMLEntries::DeleteArray()
{
	for(std::vector<CvBuildingEntry*>::iterator it = m_paBuildingEntries.begin(); it != m_paBuildingEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paBuildingEntries.clear();
}

/// Get a specific entry
CvBuildingEntry* CvBuildingXMLEntries::GetEntry(int index)
{
	return m_paBuildingEntries[index];
}

//=====================================
// CvCityBuildings
//=====================================
/// Constructor
CvCityBuildings::CvCityBuildings():
	m_paiBuildingProduction(NULL),
	m_paiBuildingProductionTime(NULL),
	m_paiBuildingOriginalOwner(NULL),
	m_paiBuildingOriginalTime(NULL),
	m_paiNumRealBuilding(NULL),
	m_paiNumFreeBuilding(NULL),
	m_iNumBuildings(0),
	m_iBuildingProductionModifier(0),
	m_iBuildingDefense(0),
	m_iBuildingDefenseMod(0),
	m_iMissionaryExtraSpreads(0),
	m_bSoldBuildingThisTurn(false),
	m_pBuildings(NULL),
	m_pCity(NULL)
{
}

/// Destructor
CvCityBuildings::~CvCityBuildings(void)
{
}

/// Initialize
void CvCityBuildings::Init(CvBuildingXMLEntries* pBuildings, CvCity* pCity)
{
	// Store off the pointers to objects we'll need later
	m_pBuildings = pBuildings;
	m_pCity = pCity;

	// Initialize status arrays

	int iNumBuildings = m_pBuildings->GetNumBuildings();

	CvAssertMsg((0 < iNumBuildings),  "m_pBuildings->GetNumBuildings() is not greater than zero but an array is being allocated in CvCityBuildings::Init");

	CvAssertMsg(m_paiBuildingProduction==NULL, "about to leak memory, CvCityBuildings::m_paiBuildingProduction");
	m_paiBuildingProduction = FNEW(int[iNumBuildings], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paiBuildingProductionTime==NULL, "about to leak memory, CvCityBuildings::m_paiBuildingProductionTime");
	m_paiBuildingProductionTime = FNEW(int[iNumBuildings], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paiBuildingOriginalOwner==NULL, "about to leak memory, CvCityBuildings::m_paiBuildingOriginalOwner");
	m_paiBuildingOriginalOwner = FNEW(int[iNumBuildings], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paiBuildingOriginalTime==NULL, "about to leak memory, CvCityBuildings::m_paiBuildingOriginalTime");
	m_paiBuildingOriginalTime = FNEW(int[iNumBuildings], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paiNumRealBuilding==NULL, "about to leak memory, CvCityBuildings::m_paiNumRealBuilding");
	m_paiNumRealBuilding = FNEW(int[iNumBuildings], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paiNumFreeBuilding==NULL, "about to leak memory, CvCityBuildings::m_paiNumFreeBuilding");
	m_paiNumFreeBuilding = FNEW(int[iNumBuildings], c_eCiv5GameplayDLL, 0);

	m_aBuildingYieldChange.clear();

	Reset();
}

/// Deallocate memory created in initialize
void CvCityBuildings::Uninit()
{
	SAFE_DELETE_ARRAY(m_paiBuildingProduction);
	SAFE_DELETE_ARRAY(m_paiBuildingProductionTime);
	SAFE_DELETE_ARRAY(m_paiBuildingOriginalOwner);
	SAFE_DELETE_ARRAY(m_paiBuildingOriginalTime);
	SAFE_DELETE_ARRAY(m_paiNumRealBuilding);
	SAFE_DELETE_ARRAY(m_paiNumFreeBuilding);
}

/// Reset status arrays to all false
void CvCityBuildings::Reset()
{
	int iI;

	// Initialize non-arrays
	m_iNumBuildings = 0;
	m_iBuildingProductionModifier = 0;
	m_iBuildingDefense = 0;
	m_iBuildingDefenseMod = 0;
	m_iMissionaryExtraSpreads = 0;

	m_bSoldBuildingThisTurn = false;

	for(iI = 0; iI < m_pBuildings->GetNumBuildings(); iI++)
	{
		m_paiBuildingProduction[iI] = 0;
		m_paiBuildingProductionTime[iI] = 0;
		m_paiBuildingOriginalOwner[iI] = NO_PLAYER;
		m_paiBuildingOriginalTime[iI] = MIN_INT;
		m_paiNumRealBuilding[iI] = 0;
		m_paiNumFreeBuilding[iI] = 0;
	}
}

/// Serialization read
void CvCityBuildings::Read(FDataStream& kStream)
{
	CvAssertMsg(m_pBuildings != NULL && m_pBuildings->GetNumBuildings() > 0, "Number of buildings to serialize is expected to greater than 0");

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_iNumBuildings;
	kStream >> m_iBuildingProductionModifier;
	kStream >> m_iBuildingDefense;
	kStream >> m_iBuildingDefenseMod;

	if(uiVersion >= 4)
		kStream >> m_iMissionaryExtraSpreads;
	else
		m_iMissionaryExtraSpreads = 0;

	if(uiVersion >= 3)
		kStream >> m_bSoldBuildingThisTurn;
	else
		m_bSoldBuildingThisTurn = false;

	if(uiVersion >= 2)
	{
		BuildingArrayHelpers::Read(kStream, m_paiBuildingProduction);
		BuildingArrayHelpers::Read(kStream, m_paiBuildingProductionTime);
		BuildingArrayHelpers::Read(kStream, m_paiBuildingOriginalOwner);
		BuildingArrayHelpers::Read(kStream, m_paiBuildingOriginalTime);
		BuildingArrayHelpers::Read(kStream, m_paiNumRealBuilding);
		BuildingArrayHelpers::Read(kStream, m_paiNumFreeBuilding);
	}
	else
	{
		ArrayWrapper<int>wrapm_paiBuildingProduction(89, m_paiBuildingProduction);
		kStream >> wrapm_paiBuildingProduction;
		ArrayWrapper<int>wrapm_paiBuildingProductionTime(89, m_paiBuildingProductionTime);
		kStream >> wrapm_paiBuildingProductionTime;
		ArrayWrapper<int>wrapm_paiBuildingOriginalOwner(89, m_paiBuildingOriginalOwner);
		kStream >> wrapm_paiBuildingOriginalOwner;
		ArrayWrapper<int>wrapm_paiBuildingOriginalTime(89, m_paiBuildingOriginalTime);
		kStream >> wrapm_paiBuildingOriginalTime;
		ArrayWrapper<int>wrapm_paiNumRealBuilding(89, m_paiNumRealBuilding);
		kStream >> wrapm_paiNumRealBuilding;
		ArrayWrapper<int>wrapm_paiNumFreeBuilding(89, m_paiNumFreeBuilding);
		kStream >> wrapm_paiNumFreeBuilding;
	}

	kStream >> m_aBuildingYieldChange;
}

/// Serialization write
void CvCityBuildings::Write(FDataStream& kStream)
{
	CvAssertMsg(m_pBuildings != NULL && m_pBuildings->GetNumBuildings() > 0, "Number of buildings to serialize is expected to greater than 0");

	// Current version number
	uint uiVersion = 4;
	kStream << uiVersion;

	kStream << m_iNumBuildings;
	kStream << m_iBuildingProductionModifier;
	kStream << m_iBuildingDefense;
	kStream << m_iBuildingDefenseMod;
	kStream << m_iMissionaryExtraSpreads;

	if(uiVersion >= 3)
		kStream << m_bSoldBuildingThisTurn;

#ifdef _MSC_VER
#pragma warning ( push )
#pragma warning ( disable : 6011 ) // if m_pBuildings is NULL during load, we're screwed. Redesign the class or the loader code.
#endif//_MSC_VER
	int iNumBuildings = m_pBuildings->GetNumBuildings();
#ifdef _MSC_VER
#pragma warning ( pop )
#endif//_MSC_VER

	BuildingArrayHelpers::Write(kStream, m_paiBuildingProduction, iNumBuildings);
	BuildingArrayHelpers::Write(kStream, m_paiBuildingProductionTime, iNumBuildings);
	BuildingArrayHelpers::Write(kStream, m_paiBuildingOriginalOwner, iNumBuildings);
	BuildingArrayHelpers::Write(kStream, m_paiBuildingOriginalTime, iNumBuildings);
	BuildingArrayHelpers::Write(kStream, m_paiNumRealBuilding, iNumBuildings);
	BuildingArrayHelpers::Write(kStream, m_paiNumFreeBuilding, iNumBuildings);

	kStream << m_aBuildingYieldChange;
}

/// Accessor: Get full array of all building XML data
CvBuildingXMLEntries* CvCityBuildings::GetBuildings() const
{
	return m_pBuildings;
}

/// Accessor: Total number of buildings in the city
int CvCityBuildings::GetNumBuildings() const
{
	return m_iNumBuildings;
}

/// Accessor: Update total number of buildings in the city
void CvCityBuildings::ChangeNumBuildings(int iChange)
{
	m_iNumBuildings = (m_iNumBuildings + iChange);
	CvAssert(GetNumBuildings() >= 0);

//	GET_PLAYER(m_pCity->getOwner()).updateNumResourceUsed();
}

/// Accessor: How many of these buildings in the city?
int CvCityBuildings::GetNumBuilding(BuildingTypes eIndex) const
{
	CvAssertMsg(eIndex != NO_BUILDING, "BuildingType eIndex is expected to not be NO_BUILDING");

	if(GC.getCITY_MAX_NUM_BUILDINGS() <= 1)
	{
		return std::max(GetNumRealBuilding(eIndex), GetNumFreeBuilding(eIndex));
	}
	else
	{
		return (GetNumRealBuilding(eIndex) + GetNumFreeBuilding(eIndex));
	}
}

/// Accessor: How many of these buildings are not obsolete?
int CvCityBuildings::GetNumActiveBuilding(BuildingTypes eIndex) const
{
	CvAssertMsg(eIndex != NO_BUILDING, "BuildingType eIndex is expected to not be NO_BUILDING");

	if(GET_TEAM(m_pCity->getTeam()).isObsoleteBuilding(eIndex))
	{
		return 0;
	}

	return (GetNumBuilding(eIndex));
}

/// Is the player allowed to sell building eIndex in this city?
bool CvCityBuildings::IsBuildingSellable(const CvBuildingEntry& kBuilding) const
{
	// Can't sell more than one building per turn
	if(IsSoldBuildingThisTurn())
		return false;

	// Can't sell a building if it doesn't cost us anything (no exploits)
	if(kBuilding.GetGoldMaintenance() <= 0)
		return false;

	// Is this a free building?
	if(GetNumFreeBuilding((BuildingTypes)kBuilding.GetID()) > 0)
		return false;

	// Science building in capital that has given us a tech boost?
	if(m_pCity->isCapital() && kBuilding.IsScienceBuilding())
	{
		return !(GET_PLAYER(m_pCity->getOwner()).GetPlayerTraits()->IsTechBoostFromCapitalScienceBuildings());
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pCity->getOwner());
		args->Push(kBuilding.GetID());

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "CityBuildingsIsBuildingSellable", args.get(), bResult))
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

/// Sell eIndex~!
void CvCityBuildings::DoSellBuilding(BuildingTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");

	CvBuildingEntry* pkBuildingEntry = GC.getBuildingInfo(eIndex);
	if(!pkBuildingEntry)
		return;

	// Can we actually do this?
	if(!IsBuildingSellable(*pkBuildingEntry))
		return;

	// Gold refund
	int iRefund = GetSellBuildingRefund(eIndex);
	GET_PLAYER(m_pCity->getOwner()).GetTreasury()->ChangeGold(iRefund);

	// Kick everyone out
	m_pCity->GetCityCitizens()->DoRemoveAllSpecialistsFromBuilding(eIndex);

	SetNumRealBuilding(eIndex, 0);

	SetSoldBuildingThisTurn(true);
}

/// How much of a refund will the player get from selling eIndex?
int CvCityBuildings::GetSellBuildingRefund(BuildingTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");

	int iRefund = GET_PLAYER(m_pCity->getOwner()).getProductionNeeded(eIndex);
	iRefund /= /*10*/ GC.getBUILDING_SALE_DIVISOR();

	return iRefund;
}

/// Has a building already been sold this turn?
bool CvCityBuildings::IsSoldBuildingThisTurn() const
{
	return m_bSoldBuildingThisTurn;
}

/// Has a building already been sold this turn?
void CvCityBuildings::SetSoldBuildingThisTurn(bool bValue)
{
	if(IsSoldBuildingThisTurn() != bValue)
		m_bSoldBuildingThisTurn = bValue;
}

/// What is the total maintenance? (no modifiers)
int CvCityBuildings::GetTotalBaseBuildingMaintenance() const
{
	int iTotalCost = 0;

	for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

		if(pkBuildingInfo)
		{
			if(GetNumBuilding(eBuilding))
				iTotalCost += (pkBuildingInfo->GetGoldMaintenance() * GetNumBuilding(eBuilding));
		}
	}

	return iTotalCost;
}

/// Accessor: How far is construction of this building?
int CvCityBuildings::GetBuildingProduction(BuildingTypes eIndex)	const
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");
	return m_paiBuildingProduction[eIndex] / 100;
}

/// Accessor: How far is construction of this building? (in hundredths)
int CvCityBuildings::GetBuildingProductionTimes100(BuildingTypes eIndex)	const
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");
	return m_paiBuildingProduction[eIndex];
}

/// Accessor: Set how much construction is complete for this building
void CvCityBuildings::SetBuildingProduction(BuildingTypes eIndex, int iNewValue)
{
	SetBuildingProductionTimes100(eIndex, iNewValue*100);
}

/// Accessor: Set how much construction is complete for this building (in hundredths)
void CvCityBuildings::SetBuildingProductionTimes100(BuildingTypes eIndex, int iNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings())");

	if(GetBuildingProductionTimes100(eIndex) != iNewValue)
	{
		if(GetBuildingProductionTimes100(eIndex) == 0)
		{
			NotifyNewBuildingStarted(eIndex);
		}

		m_paiBuildingProduction[eIndex] = iNewValue;
		CvAssert(GetBuildingProductionTimes100(eIndex) >= 0);

		if((m_pCity->getOwner() == GC.getGame().getActivePlayer()) && m_pCity->isCitySelected())
		{
			GC.GetEngineUserInterface()->setDirty(CityScreen_DIRTY_BIT, true);
		}

		auto_ptr<ICvCity1> pCity = GC.WrapCityPointer(m_pCity);

		GC.GetEngineUserInterface()->SetSpecificCityInfoDirty(pCity.get(), CITY_UPDATE_TYPE_BANNER);
	}
}

/// Accessor: Update construction progress for this building
void CvCityBuildings::ChangeBuildingProduction(BuildingTypes eIndex, int iChange)
{
	ChangeBuildingProductionTimes100(eIndex, iChange*100);
}

/// Accessor: Update construction progress for this building (in hundredths)
void CvCityBuildings::ChangeBuildingProductionTimes100(BuildingTypes eIndex, int iChange)
{
	SetBuildingProductionTimes100(eIndex, (GetBuildingProductionTimes100(eIndex) + iChange));
}

/// Accessor: How many turns has this building been under production?
int CvCityBuildings::GetBuildingProductionTime(BuildingTypes eIndex)	const
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");
	return m_paiBuildingProductionTime[eIndex];
}

/// Accessor: Set number of turns this building been under production
void CvCityBuildings::SetBuildingProductionTime(BuildingTypes eIndex, int iNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");
	m_paiBuildingProductionTime[eIndex] = iNewValue;
	CvAssert(GetBuildingProductionTime(eIndex) >= 0);
}

/// Accessor: Change number of turns this building been under production
void CvCityBuildings::ChangeBuildingProductionTime(BuildingTypes eIndex, int iChange)
{
	SetBuildingProductionTime(eIndex, (GetBuildingProductionTime(eIndex) + iChange));
}

/// Accessor: Who owned the city when this building was built?
int CvCityBuildings::GetBuildingOriginalOwner(BuildingTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");
	return m_paiBuildingOriginalOwner[eIndex];
}

/// Accessor: Set who owned the city when this building was built
void CvCityBuildings::SetBuildingOriginalOwner(BuildingTypes eIndex, int iNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");
	m_paiBuildingOriginalOwner[eIndex] = iNewValue;
}

/// Accessor: What year was this building built?
int CvCityBuildings::GetBuildingOriginalTime(BuildingTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");
	return m_paiBuildingOriginalTime[eIndex];
}

/// Accessor: Set year building was built
void CvCityBuildings::SetBuildingOriginalTime(BuildingTypes eIndex, int iNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");
	m_paiBuildingOriginalTime[eIndex] = iNewValue;
}

/// Accessor: How many of these buildings have been constructed in the city?
int CvCityBuildings::GetNumRealBuilding(BuildingTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");
	return m_paiNumRealBuilding[eIndex];
}

/// Accessor: Set number of these buildings that have been constructed in the city
void CvCityBuildings::SetNumRealBuilding(BuildingTypes eIndex, int iNewValue)
{
	SetNumRealBuildingTimed(eIndex, iNewValue, true, m_pCity->getOwner(), GC.getGame().getGameTurnYear());

}

/// Accessor: Set number of these buildings that have been constructed in the city (with date)
void CvCityBuildings::SetNumRealBuildingTimed(BuildingTypes eIndex, int iNewValue, bool bFirst, PlayerTypes eOriginalOwner, int iOriginalTime)
{
	CvPlayer* pPlayer = &GET_PLAYER(m_pCity->getOwner());

	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");

	int iChangeNumRealBuilding = iNewValue - GetNumRealBuilding(eIndex);

	CvBuildingEntry* buildingEntry = GC.getBuildingInfo(eIndex);
	const BuildingClassTypes buildingClassType = (BuildingClassTypes) buildingEntry->GetBuildingClassType();
	const CvBuildingClassInfo& kBuildingClassInfo = buildingEntry->GetBuildingClassInfo();

	if(iChangeNumRealBuilding != 0)
	{
		int iOldNumBuilding = GetNumBuilding(eIndex);

		m_paiNumRealBuilding[eIndex] = iNewValue;

		if(GetNumRealBuilding(eIndex) > 0)
		{
			SetBuildingOriginalOwner(eIndex, eOriginalOwner);
			SetBuildingOriginalTime(eIndex, iOriginalTime);
		}
		else
		{
			SetBuildingOriginalOwner(eIndex, NO_PLAYER);
			SetBuildingOriginalTime(eIndex, MIN_INT);
		}

		// Process building effects
		if(iOldNumBuilding != GetNumBuilding(eIndex))
		{
			m_pCity->processBuilding(eIndex, iChangeNumRealBuilding, bFirst);
		}

		// Maintenance cost
		if(buildingEntry->GetGoldMaintenance() != 0)
		{
			pPlayer->GetTreasury()->ChangeBaseBuildingGoldMaintenance(buildingEntry->GetGoldMaintenance() * iChangeNumRealBuilding);
		}

		//Achievement for Temples
		const char* szBuildingTypeC = buildingEntry->GetType();
		CvString szBuildingType = szBuildingTypeC;
		if(szBuildingType == "BUILDING_TEMPLE")
		{
			if(m_pCity->getOwner() == GC.getGame().getActivePlayer())
			{
				gDLL->IncrementSteamStatAndUnlock(ESTEAMSTAT_TEMPLES, 1000, ACHIEVEMENT_1000TEMPLES);
			}
		}

		if(buildingEntry->GetPreferredDisplayPosition() > 0)
		{
			auto_ptr<ICvCity1> pDllCity(new CvDllCity(m_pCity));

			if(iNewValue > 0)
			{
				// if this is a WW that (likely has a half-built state)
				if(isWorldWonderClass(kBuildingClassInfo))
				{
					if(GetBuildingProduction(eIndex))
					{
						GC.GetEngineUserInterface()->AddDeferredWonderCommand(WONDER_EDITED, pDllCity.get(), eIndex, 1);
					}
					else
					{
						GC.GetEngineUserInterface()->AddDeferredWonderCommand(WONDER_CREATED, pDllCity.get(), eIndex, 1);
					}
				}
				else
				{
					GC.GetEngineUserInterface()->AddDeferredWonderCommand(WONDER_CREATED, pDllCity.get(), eIndex, 1);
				}
			}
			else
			{
				GC.GetEngineUserInterface()->AddDeferredWonderCommand(WONDER_REMOVED, pDllCity.get(), eIndex, 0);
			}
		}

		if(!(kBuildingClassInfo.isNoLimit()))
		{
			if(isWorldWonderClass(kBuildingClassInfo))
			{
				m_pCity->changeNumWorldWonders(iChangeNumRealBuilding);
				pPlayer->ChangeNumWonders(iChangeNumRealBuilding);
			}
			else if(isTeamWonderClass(kBuildingClassInfo))
			{
				m_pCity->changeNumTeamWonders(iChangeNumRealBuilding);
			}
			else if(isNationalWonderClass(kBuildingClassInfo))
			{
				m_pCity->changeNumNationalWonders(iChangeNumRealBuilding);
				if(m_pCity->isHuman() && !GC.getGame().isGameMultiPlayer())
				{
					IncrementWonderStats(buildingClassType);
				}
			}
			else
			{
				ChangeNumBuildings(iChangeNumRealBuilding);
			}
		}

		if(buildingEntry->IsCityWall())
		{
			auto_ptr<ICvPlot1> pDllPlot(new CvDllPlot(m_pCity->plot()));
			gDLL->GameplayWallCreated(pDllPlot.get());
		}

		// Update the amount of a Resource used up by this Building
		int iNumResources = GC.getNumResourceInfos();
		for(int iResourceLoop = 0; iResourceLoop < iNumResources; iResourceLoop++)
		{
			if(buildingEntry->GetResourceQuantityRequirement(iResourceLoop) > 0)
			{
				pPlayer->changeNumResourceUsed((ResourceTypes) iResourceLoop, iChangeNumRealBuilding * buildingEntry->GetResourceQuantityRequirement(iResourceLoop));
			}
		}

		if(iChangeNumRealBuilding > 0)
		{
			if(bFirst)
			{
				if(GC.getGame().isFinalInitialized()/* && !(gDLL->GetWorldBuilderMode() )*/)
				{
					// World Wonder Notification
					if(isWorldWonderClass(kBuildingClassInfo))
					{
						Localization::String localizedText = Localization::Lookup("TXT_KEY_MISC_COMPLETES_WONDER");
						localizedText << pPlayer->getNameKey() << buildingEntry->GetTextKey();
						GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, m_pCity->getOwner(), localizedText.toUTF8(), m_pCity->getX(), m_pCity->getY());

						bool bDontShowRewardPopup = GC.GetEngineUserInterface()->IsOptionNoRewardPopups();

						// Notification in MP games
						if(bDontShowRewardPopup || GC.getGame().isNetworkMultiPlayer())	// KWG: Candidate for !GC.getGame().IsOption(GAMEOPTION_SIMULTANEOUS_TURNS)
						{
							CvNotifications* pNotifications = GET_PLAYER(m_pCity->getOwner()).GetNotifications();
							if(pNotifications)
							{
								localizedText = Localization::Lookup("TXT_KEY_MISC_WONDER_COMPLETED");
								localizedText << pPlayer->getNameKey() << buildingEntry->GetTextKey();
								pNotifications->Add(NOTIFICATION_WONDER_COMPLETED_ACTIVE_PLAYER, localizedText.toUTF8(), localizedText.toUTF8(), m_pCity->getX(), m_pCity->getY(), eIndex, pPlayer->GetID());
							}
						}
						// Popup in SP games
						else
						{
							if(m_pCity->getOwner() == GC.getGame().getActivePlayer())
							{
								CvPopupInfo kPopup(BUTTONPOPUP_WONDER_COMPLETED_ACTIVE_PLAYER, eIndex);
								GC.GetEngineUserInterface()->AddPopup(kPopup);

								if(GET_PLAYER(GC.getGame().getActivePlayer()).isHuman())
								{
									gDLL->UnlockAchievement(ACHIEVEMENT_BUILD_WONDER);

									//look to see if all wonders have been built to unlock the other one
									IncrementWonderStats(buildingClassType);

								}
							}
						}

						// Wonder notification for all other players
						for(int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
						{
							CvPlayerAI& thisPlayer = GET_PLAYER((PlayerTypes)iI);
							if(thisPlayer.isAlive())
							{
								// Owner already got his messaging
								if(iI != m_pCity->getOwner())
								{
									// If the builder is met, and the city is revealed
									// Special case for DLC_06 Scenario: Always show the more informative notification
									if((m_pCity->plot()->isRevealed(thisPlayer.getTeam()) && GET_TEAM(thisPlayer.getTeam()).isHasMet(m_pCity->getTeam())) || gDLL->IsModActivated(CIV5_DLC_06_SCENARIO_MODID))
									{
										CvNotifications* pNotifications = thisPlayer.GetNotifications();
										if(pNotifications)
										{
											localizedText = Localization::Lookup("TXT_KEY_MISC_WONDER_COMPLETED");
											localizedText << pPlayer->getNameKey() << buildingEntry->GetTextKey();
											pNotifications->Add(NOTIFICATION_WONDER_COMPLETED, localizedText.toUTF8(), localizedText.toUTF8(), m_pCity->getX(), m_pCity->getY(), eIndex, pPlayer->GetID());
										}
									}
									else
									{
										CvNotifications* pNotifications = thisPlayer.GetNotifications();
										if(pNotifications)
										{
											localizedText = Localization::Lookup("TXT_KEY_MISC_WONDER_COMPLETED_UNKNOWN");
											localizedText <<  buildingEntry->GetTextKey();
											pNotifications->Add(NOTIFICATION_WONDER_COMPLETED, localizedText.toUTF8(), localizedText.toUTF8(), -1, -1, eIndex, -1);
										}
									}
								}
							}

							//Achievements!
							if(pPlayer->GetID() == GC.getGame().getActivePlayer() && strcmp(buildingEntry->GetType(), "BUILDING_GREAT_FIREWALL") == 0)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP1_16);
							}
						}
					}
				}

				GC.getGame().incrementBuildingClassCreatedCount(buildingClassType);
			}
		}

		m_pCity->updateStrengthValue();

		// Building might affect City Banner stats
		auto_ptr<ICvCity1> pCity = GC.WrapCityPointer(m_pCity);
		GC.GetEngineUserInterface()->SetSpecificCityInfoDirty(pCity.get(), CITY_UPDATE_TYPE_BANNER);
	}
}

/// Accessor: Get number of free buildings of this type in city
int CvCityBuildings::GetNumFreeBuilding(BuildingTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");
	return m_paiNumFreeBuilding[eIndex];
}

/// Accessor: Set number of free buildings of this type in city
void CvCityBuildings::SetNumFreeBuilding(BuildingTypes eIndex, int iNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < m_pBuildings->GetNumBuildings(), "eIndex expected to be < m_pBuildings->GetNumBuildings()");

	if(GetNumFreeBuilding(eIndex) != iNewValue)
	{
		int iOldNumBuilding = GetNumBuilding(eIndex);

		m_paiNumFreeBuilding[eIndex] = iNewValue;

		if(iOldNumBuilding != GetNumBuilding(eIndex))
		{
			m_pCity->processBuilding(eIndex, iNewValue - iOldNumBuilding, true);
		}
	}
}
/// Accessor: Get yield boost for a specific building by yield type
int CvCityBuildings::GetBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield) const
{
	for(std::vector<BuildingYieldChange>::const_iterator it = m_aBuildingYieldChange.begin(); it != m_aBuildingYieldChange.end(); ++it)
	{
		if((*it).eBuildingClass == eBuildingClass && (*it).eYield == eYield)
		{
			return (*it).iChange;
		}
	}

	return 0;
}

/// Accessor: Set yield boost for a specific building by yield type
void CvCityBuildings::SetBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield, int iChange)
{
	for(std::vector<BuildingYieldChange>::iterator it = m_aBuildingYieldChange.begin(); it != m_aBuildingYieldChange.end(); ++it)
	{
		if((*it).eBuildingClass == eBuildingClass && (*it).eYield == eYield)
		{
			int iOldChange = (*it).iChange;
			if(iOldChange != iChange)
			{

				if(iChange == 0)
				{
					m_aBuildingYieldChange.erase(it);
				}
				else
				{
					(*it).iChange = iChange;
				}

				BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(m_pCity->getCivilizationType())->getCivilizationBuildings(eBuildingClass);
				if(NO_BUILDING != eBuilding)
				{
					if(GetNumActiveBuilding(eBuilding) > 0)
					{
						m_pCity->ChangeBaseYieldRateFromBuildings(eYield, (iChange - iOldChange) * GetNumActiveBuilding(eBuilding));
					}
				}
			}

			return;
		}
	}

	if(0 != iChange)
	{
		BuildingYieldChange kChange;
		kChange.eBuildingClass = eBuildingClass;
		kChange.eYield = eYield;
		kChange.iChange = iChange;
		m_aBuildingYieldChange.push_back(kChange);

		BuildingTypes eBuilding = (BuildingTypes)m_pCity->getCivilizationInfo().getCivilizationBuildings(eBuildingClass);
		if(NO_BUILDING != eBuilding)
		{
			if(GetNumActiveBuilding(eBuilding) > 0)
			{
				m_pCity->ChangeBaseYieldRateFromBuildings(eYield, iChange * GetNumActiveBuilding(eBuilding));
			}
		}
	}
}

/// Accessor: Change yield boost for a specific building by yield type
void CvCityBuildings::ChangeBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield, int iChange)
{
	SetBuildingYieldChange(eBuildingClass, eYield, GetBuildingYieldChange(eBuildingClass, eYield) + iChange);
}

/// Accessor: Get current production modifier from buildings
int CvCityBuildings::GetBuildingProductionModifier() const
{
	return m_iBuildingProductionModifier;
}

/// Accessor: Change current production modifier from buildings
void CvCityBuildings::ChangeBuildingProductionModifier(int iChange)
{
	m_iBuildingProductionModifier = (m_iBuildingProductionModifier + iChange);
	CvAssert(GetBuildingProductionModifier() >= 0);
}

/// Accessor: Get current defense boost from buildings
int CvCityBuildings::GetBuildingDefense() const
{
	return m_iBuildingDefense;
}

/// Accessor: Change current defense boost from buildings
void CvCityBuildings::ChangeBuildingDefense(int iChange)
{
	if(iChange != 0)
	{
		m_iBuildingDefense = (m_iBuildingDefense + iChange);
		CvAssert(GetBuildingDefense() >= 0);

		m_pCity->plot()->plotAction(PUF_makeInfoBarDirty);
	}
}

/// Accessor: Get current defense boost Mod from buildings
int CvCityBuildings::GetBuildingDefenseMod() const
{
	return m_iBuildingDefenseMod;
}

/// Accessor: Change current defense boost mod from buildings
void CvCityBuildings::ChangeBuildingDefenseMod(int iChange)
{
	if(iChange != 0)
	{
		m_iBuildingDefenseMod = (m_iBuildingDefenseMod + iChange);
		CvAssert(m_iBuildingDefenseMod >= 0);

		m_pCity->plot()->plotAction(PUF_makeInfoBarDirty);
	}
}

/// Accessor: Get extra times to spread religion for missionaries from this city
int CvCityBuildings::GetMissionaryExtraSpreads() const
{
	return m_iMissionaryExtraSpreads;
}

/// Accessor: Change extra times to spread religion for missionaries from this city
void CvCityBuildings::ChangeMissionaryExtraSpreads(int iChange)
{
	if(iChange != 0)
	{
		m_iMissionaryExtraSpreads = (m_iMissionaryExtraSpreads + iChange);
		CvAssert(m_iMissionaryExtraSpreads >= 0);
	}
}

void CvCityBuildings::IncrementWonderStats(BuildingClassTypes eIndex)
{
	CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eIndex);
	if(pkBuildingClassInfo == NULL)
		return;

	const char* szWonderTypeChar = pkBuildingClassInfo->GetType();
	CvString szWonderType = szWonderTypeChar;

	if(szWonderType == "BUILDINGCLASS_HEROIC_EPIC")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_HEROICEPIC);
	}
	else if(szWonderType == "BUILDINGCLASS_NATIONAL_COLLEGE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_NATIONALCOLLEGE);
	}
	else if(szWonderType == "BUILDINGCLASS_NATIONAL_EPIC")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_NATIONALEPIC);
	}
	else if(szWonderType == "BUILDINGCLASS_IRONWORKS")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_IRONWORKS);
	}
	else if(szWonderType == "BUILDINGCLASS_OXFORD_UNIVERSITY")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_OXFORDUNIVERSITY);
	}
	else if(szWonderType == "BUILDINGCLASS_HERMITAGE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_HERMITAGE);
	}
	else if(szWonderType == "BUILDINGCLASS_GREAT_LIGHTHOUSE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_GREATLIGHTHOUSE);
	}
	else if(szWonderType == "BUILDINGCLASS_STONEHENGE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_STONEHENGE);
	}
	else if(szWonderType == "BUILDINGCLASS_GREAT_LIBRARY")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_GREATLIBRARY);
	}
	else if(szWonderType == "BUILDINGCLASS_PYRAMID")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_PYRAMIDS);
	}
	else if(szWonderType == "BUILDINGCLASS_COLOSSUS")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_COLOSSUS);
	}
	else if(szWonderType == "BUILDINGCLASS_ORACLE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_ORACLE);
	}
	else if(szWonderType == "BUILDINGCLASS_HANGING_GARDEN")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_HANGINGGARDENS);
	}
	else if(szWonderType == "BUILDINGCLASS_GREAT_WALL")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_GREATWALL);
	}
	else if(szWonderType == "BUILDINGCLASS_ANGKOR_WAT")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_ANGKORWAT);
	}
	else if(szWonderType == "BUILDINGCLASS_HAGIA_SOPHIA")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_HAGIASOPHIA);
	}
	else if(szWonderType == "BUILDINGCLASS_CHICHEN_ITZA")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_CHICHENITZA);
	}
	else if(szWonderType == "BUILDINGCLASS_MACHU_PICHU")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_MACHUPICCHU);
	}
	else if(szWonderType == "BUILDINGCLASS_NOTRE_DAME")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_NOTREDAME);
	}
	else if(szWonderType == "BUILDINGCLASS_PORCELAIN_TOWER")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_PORCELAINTOWER);
	}
	else if(szWonderType == "BUILDINGCLASS_HIMEJI_CASTLE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_HIMEJICASTLE);
	}
	else if(szWonderType == "BUILDINGCLASS_SISTINE_CHAPEL")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_SISTINECHAPEL);
	}
	else if(szWonderType == "BUILDINGCLASS_KREMLIN")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_KREMLIN);
	}
	else if(szWonderType == "BUILDINGCLASS_FORBIDDEN_PALACE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_FORBIDDENPALACE);
	}
	else if(szWonderType == "BUILDINGCLASS_TAJ_MAHAL")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_TAJMAHAL);
	}
	else if(szWonderType == "BUILDINGCLASS_BIG_BEN")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_BIGBEN);
	}
	else if(szWonderType == "BUILDINGCLASS_LOUVRE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_LOUVRE);
	}
	else if(szWonderType == "BUILDINGCLASS_BRANDENBURG_GATE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_BRANDENBURGGATE);
	}
	else if(szWonderType == "BUILDINGCLASS_STATUE_OF_LIBERTY")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_STATUEOFLIBERTY);
	}
	else if(szWonderType == "BUILDINGCLASS_CRISTO_REDENTOR")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_CRISTOREDENTOR);
	}
	else if(szWonderType == "BUILDINGCLASS_EIFFEL_TOWER")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_EIFFELTOWER);
	}
	else if(szWonderType == "BUILDINGCLASS_PENTAGON")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_PENTAGON);
	}
	else if(szWonderType == "BUILDINGCLASS_UNITED_NATIONS")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_UNITEDNATION);
	}
	else if(szWonderType == "BUILDINGCLASS_SYDNEY_OPERA_HOUSE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_SYDNEYOPERAHOUSE);
	}
	else if(szWonderType == "BUILDINGCLASS_STATUE_ZEUS")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_STATUEOFZEUS);
	}
	else if(szWonderType == "BUILDINGCLASS_TEMPLE_ARTEMIS")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_TEMPLEOFARTEMIS);
	}
	else if(szWonderType == "BUILDINGCLASS_MAUSOLEUM_HALICARNASSUS")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_MAUSOLEUMOFHALICARNASSUS);
	}
	else
	{
		OutputDebugString("\nNo Stat for selected Wonder: ");
		OutputDebugString(szWonderType);
		OutputDebugString("\n");
	}

	bool bCheckForWonders = false;
	bCheckForWonders = CheckForAllWondersBuilt();
	if(bCheckForWonders)
	{
		gDLL->UnlockAchievement(ACHIEVEMENT_ALL_WONDERS);
	}

	//DLC_06
	bool bCheckForAncientWonders = false;
	bCheckForAncientWonders = CheckForSevenAncientWondersBuilt();
	if(bCheckForAncientWonders)
	{
		gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_ANCIENT_WONDERS);
	}

}
bool CvCityBuildings::CheckForAllWondersBuilt()
{
	int iI;
	int iStartStatWonder = ESTEAMSTAT_ANGKORWAT;
	int iEndStatWonder = ESTEAMSTAT_PYRAMIDS;		//Don't include the united nations because it was removed in BNW.
	int32 nStat;

	for(iI = iStartStatWonder; iI < iEndStatWonder; iI++)
	{
		if(gDLL->GetSteamStat((ESteamStat)iI, &nStat))
		{
			if(nStat <= 0)
			{
				return false;
			}
		}
	}
	return true;
}

bool CvCityBuildings::CheckForSevenAncientWondersBuilt()
{
	GUID guid;
	ExtractGUID(CIV5_DLC_06_PACKAGEID, guid);

	if(gDLL->IsDLCValid(guid))
	{
		ESteamStat arrWonderStats[7] =
		{
			ESTEAMSTAT_COLOSSUS,
			ESTEAMSTAT_GREATLIGHTHOUSE,
			ESTEAMSTAT_HANGINGGARDENS,
			ESTEAMSTAT_PYRAMIDS,
			ESTEAMSTAT_STATUEOFZEUS,
			ESTEAMSTAT_TEMPLEOFARTEMIS,
			ESTEAMSTAT_MAUSOLEUMOFHALICARNASSUS
		};
		int32 nStat;
		for(int iI = 0; iI < 7; iI++)
		{
			if(gDLL->GetSteamStat(arrWonderStats[iI], &nStat))
			{
				if(nStat <= 0)
				{
					return false;
				}
			}
			else
			{
				// Couldn't get one of the SteamStats for some reason
				return false;
			}
		}
		return true;
	}
	return false;
}

/// Uses the notification system to send information out when other players need to know a building has been started
void CvCityBuildings::NotifyNewBuildingStarted(BuildingTypes /*eIndex*/)
{
	// JON: Disabling this notification
	return;

	// is this city starting a wonder? If so, send a notification
	//CvBuildingEntry* buildingEntry = GC.getBuildingInfo(eIndex);
	//if (isLimitedWonderClass((BuildingClassTypes)(buildingEntry->GetBuildingClassType())) && GetBuildingProductionTimes100(eIndex) == 0)
	//{
	//	Localization::String locString;
	//	Localization::String locSummaryString;

	//	for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	//	{
	//		PlayerTypes ePlayer = (PlayerTypes)ui;

	//		if (ePlayer == m_pCity->getOwner() || !GET_PLAYER(ePlayer).isAlive())
	//		{
	//			continue;
	//		}

	//		int iX = -1;
	//		int iY = -1;
	//		int iPlayerID = -1;

	//		if (GET_TEAM(m_pCity->getTeam()).isHasMet(GET_PLAYER(ePlayer).getTeam()))
	//		{
	//			if (m_pCity->isRevealed(GET_PLAYER(ePlayer).getTeam(), false))
	//			{
	//				locString = Localization::Lookup("TXT_KEY_NOTIFICATION_WONDER_STARTED");
	//				locString << GET_PLAYER(m_pCity->getOwner()).getNameKey() << buildingEntry->GetTextKey() << m_pCity->getNameKey();
	//			}
	//			else
	//			{
	//				locString = Localization::Lookup("TXT_KEY_NOTIFICATION_WONDER_STARTED_UNKNOWN_LOCATION");
	//				locString << GET_PLAYER(m_pCity->getOwner()).getNameKey() << buildingEntry->GetTextKey();
	//			}


	//			locSummaryString = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_WONDER_STARTED");
	//			locSummaryString << GET_PLAYER(m_pCity->getOwner()).getNameKey() << buildingEntry->GetTextKey();

	//		}
	//		else
	//		{
	//			locString = Localization::Lookup("TXT_KEY_NOTIFICATION_WONDER_STARTED_UNMET");
	//			locString << buildingEntry->GetTextKey();
	//			locSummaryString = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_WONDER_STARTED_UNKNOWN");
	//			locSummaryString << buildingEntry->GetTextKey();
	//		}

	//		CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
	//		if (pNotifications)
	//		{
	//			pNotifications->Add(NOTIFICATION_WONDER_STARTED, locString.toUTF8(), locSummaryString.toUTF8(), iX, iY, eIndex);
	//		}
	//	}
	//}
}

/// Helper function to read in an integer array of data sized according to number of building types
void BuildingArrayHelpers::Read(FDataStream& kStream, int* paiBuildingArray)
{
	int iNumEntries;
	FStringFixedBuffer(sTemp, 64);
	int iType;

	kStream >> iNumEntries;

	for(int iI = 0; iI < iNumEntries; iI++)
	{
		kStream >> sTemp;
		if(sTemp != "NO_BUILDING")
		{
			iType = GC.getInfoTypeForString(sTemp);
			if(iType != -1)
			{
				kStream >> paiBuildingArray[iType];
			}
			else
			{
				CvString szError;
				szError.Format("LOAD ERROR: Building Type not found: %s", sTemp);
				GC.LogMessage(szError.GetCString());
				CvAssertMsg(false, szError);
				int iDummy;
				kStream >> iDummy; // Skip it.
			}
		}
	}
}

/// Helper function to write out an integer array of data sized according to number of building types
void BuildingArrayHelpers::Write(FDataStream& kStream, int* paiBuildingArray, int iArraySize)
{
	FStringFixedBuffer(sTemp, 64);

	kStream << iArraySize;

	for(int iI = 0; iI < iArraySize; iI++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iI);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if(pkBuildingInfo)
		{
			sTemp = pkBuildingInfo->GetType();
			kStream << sTemp;
			kStream << paiBuildingArray[iI];
		}
		else
		{
			sTemp = "NO_BUILDING";
			kStream << sTemp;
		}
	}
}
