/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreUtils.h"
#include "CvInfosSerializationHelper.h"

#include "LintFree.h"

/// Constructor
CvUnitEntry::CvUnitEntry(void) :
	m_iProductionCost(0),
	m_iFaithCost(0),
	m_bRequiresFaithPurchaseEnabled(false),
#if defined(MOD_GLOBAL_EXCLUDE_FROM_GIFTS)
	m_bNoMinorGifts(false),
#endif
	m_bPurchaseOnly(false),
	m_bMoveAfterPurchase(false),
#if defined(MOD_GLOBAL_MOVE_AFTER_UPGRADE)
	m_bMoveAfterUpgrade(false),
#endif
#if defined(MOD_GLOBAL_CANNOT_EMBARK)
	m_bCannotEmbark(false),
#endif
	m_iHurryCostModifier(0),
	m_iAdvancedStartCost(0),
	m_iMinAreaSize(0),
	m_iMoves(0),
	m_bMoves(false),
	m_iBaseSightRange(0),
	m_iRange(0),
	m_iAirInterceptRange(0),
	m_iBaseLandAirDefense(0),
	m_iAirUnitCap(0),
	m_iNukeDamageLevel(0),
	m_iWorkRate(0),
	m_iNumFreeTechs(0),
	m_iBaseBeakersTurnsToCount(0),
	m_iBaseCultureTurnsToCount(0),
	m_iBaseTurnsForGAPToCount(0),
	m_iBaseHurry(0),
	m_iHurryMultiplier(0),
	m_bRushBuilding(false),
	m_iBaseGold(0),
	m_iScaleFromNumGWs(0),
	m_iScaleFromNumThemes(0),
	m_iNumGoldPerEra(0),
#if defined(MOD_DIPLOMACY_CITYSTATES)
	m_iNumInfPerEra(0),
	m_iProductionCostPerEra(0),
#endif
#if defined(MOD_BALANCE_CORE)
	m_iNumFreeLux(0),
	m_iBeliefUnlock(NO_BELIEF),
	m_bCultureFromExperienceOnDisband(false),
	m_bFreeUpgrade(false),
	m_bUnitEraUpgrade(false),
	m_bWarOnly(0),
	m_bWLTKD(false),
	m_bGoldenAge(false),
	m_bCultureBoost(0),
	m_bExtraAttackHealthOnKill(false),
	m_bHighSeaRaider(false),
#endif
	m_bSpreadReligion(false),
	m_bRemoveHeresy(false),
	m_iReligionSpreads(0),
	m_iReligiousStrength(0),
	m_bFoundReligion(false),
	m_bRequiresEnhancedReligion(false),
	m_bProhibitsSpread(false),
#if defined(MOD_CIV6_WORKER)
	m_iBuilderStrength(0),
#endif
	m_bCanBuyCityState(false),
#if defined(MOD_GLOBAL_SEPARATE_GREAT_ADMIRAL)
	m_bCanRepairFleet(false),
	m_bCanChangePort(false),
#endif
	m_iCombat(0),
#if defined(MOD_GLOBAL_STACKING_RULES)
	m_iStackCombat(0),
#endif
#if defined(MOD_CARGO_SHIPS)
	m_iCargoCombat(0),
	m_iSpecialUnitCargoLoad(NO_SPECIALUNIT),
#endif
	m_iCombatLimit(0),
	m_iRangedCombat(0),
	m_iRangedCombatLimit(0),
	m_bCoastalFire(false),
#if defined(MOD_UNITS_NO_SUPPLY)
	m_bNoSupply(false),
#endif
#if defined(MOD_UNITS_MAX_HP)
	m_iMaxHitPoints(100),
#endif
	m_iXPValueAttack(0),
	m_iXPValueDefense(0),
	m_iSpecialCargo(0),
	m_iDomainCargo(0),
	m_iConscriptionValue(0),
	m_iExtraMaintenanceCost(0),
	m_bNoMaintenance(false),
	m_iUnhappiness(0),
	m_iUnitClassType(NO_UNITCLASS),
	m_iSpecialUnitType(NO_SPECIALUNIT),
	m_iUnitCaptureClassType(NO_UNITCLASS),
	m_iUnitCombatType(NO_UNITCOMBAT),
#if defined(MOD_GLOBAL_PROMOTION_CLASSES)
	m_iUnitPromotionType(NO_UNITCOMBAT),
#endif
#if defined(MOD_EVENTS_CAN_MOVE_INTO)
	m_bSendCanMoveIntoEvent(false),
#endif
	m_iDomainType(NO_DOMAIN),
	m_iCivilianAttackPriority(NO_CIVILIAN_ATTACK_PRIORITY),
	m_iDefaultUnitAIType(NO_UNITAI),
	m_iPrereqPillageTech(NO_TECH),
	m_iPrereqAndTech(NO_TECH),
	m_iObsoleteTech(NO_TECH),
	m_iPolicyType(NO_POLICY),
	m_iGoodyHutUpgradeUnitClass(NO_UNITCLASS),
	m_iGroupSize(0),
	m_iGroupDefinitions(0),
	m_iUnitMeleeWaveSize(0),
	m_iUnitRangedWaveSize(0),
	m_iNumUnitNames(0),
	m_iCommandType(NO_COMMAND),
	m_bFoodProduction(false),
	m_bNoBadGoodies(false),
	m_bRivalTerritory(false),
	m_bMilitarySupport(false),
	m_bMilitaryProduction(false),
	m_bPillage(false),
	m_bFound(false),
	m_bFoundAbroad(false),
#if defined(MOD_BALANCE_CORE)
	m_bFoundMid(false),
	m_bFoundLate(false),
	m_iFoundColony(false),
	m_bIsCityAttackSupport(false),
	m_iGPExtra(0),
	m_iGoodyModifier(0),
	m_iSupplyCapBoost(0),
#endif
	m_iCultureBombRadius(0),
	m_iNumberOfCultureBombs(0),
	m_iGoldenAgeTurns(0),
	m_iFreePolicies(0),
	m_iOneShotTourism(0),
	m_iOneShotTourismPercentOthers(0),
	m_bIgnoreBuildingDefense(false),
	m_bPrereqResources(false),
	m_bMechanized(false),
	m_bSuicide(false),
	m_bCaptureWhileEmbarked(false),
	m_bRangeAttackOnlyInDomain(false),
	m_bTrade(false),
	m_iNumExoticGoods(0),
	m_pbUpgradeUnitClass(NULL),
	m_pbUnitAIType(NULL),
	m_pbNotUnitAIType(NULL),
	m_pbBuilds(NULL),
	m_pbGreatPeoples(NULL),
	m_pbBuildings(NULL),
	m_pbBuildingClassRequireds(NULL),
#if defined(MOD_BALANCE_CORE)
	m_piScalingFromOwnedImprovements(NULL),
	m_pbBuildOnFound(NULL),
	m_pbBuildingClassPurchaseRequireds(NULL),
	m_iResourceType(NO_RESOURCE),
	m_bPuppetPurchaseOverride(false),
	m_bMinorCivGift(false),
	m_bNoMinorCivGift(false),
	m_bIsMounted(false),
	m_iCooldown(0),
	m_iGlobalFaithCooldown(0),
	m_iLocalFaithCooldown(0),
	m_iFriendlyLandsPromotion(NO_PROMOTION),
#endif
	m_piPrereqAndTechs(NULL),
	m_piResourceQuantityRequirements(NULL),
	m_piResourceQuantityExpended(NULL),
	m_piProductionTraits(NULL),
	m_piFlavorValue(NULL),
	m_piUnitGroupRequired(NULL),
	m_pbFreePromotions(NULL),
	m_paszEarlyArtDefineTags(NULL),
	m_paszLateArtDefineTags(NULL),
	m_paszMiddleArtDefineTags(NULL),
	m_paszUnitNames(NULL),
	m_paeGreatWorks(NULL),
#if defined(MOD_BALANCE_CORE)
	m_paeGreatPersonEra(NULL),
	m_piEraCombatStrength(NULL),
	m_ppiEraUnitCombatType(NULL),
	m_ppiEraUnitPromotions(NULL),
#endif
#if defined(MOD_UNITS_RESOURCE_QUANTITY_TOTALS)
	m_piResourceQuantityTotals(),
#endif
#if defined(MOD_GLOBAL_STACKING_RULES)
	m_iNumberStackingUnits(0),
#endif
	m_bUnitArtInfoEraVariation(false),
	m_bUnitArtInfoCulturalVariation(false),
	m_iUnitFlagIconOffset(0),
	m_iUnitPortraitOffset(0)
{
}

/// Destructor
CvUnitEntry::~CvUnitEntry(void)
{
	SAFE_DELETE_ARRAY(m_pbUpgradeUnitClass);
	SAFE_DELETE_ARRAY(m_pbUnitAIType);
	SAFE_DELETE_ARRAY(m_pbNotUnitAIType);
	SAFE_DELETE_ARRAY(m_pbBuilds);
	SAFE_DELETE_ARRAY(m_pbGreatPeoples);
	SAFE_DELETE_ARRAY(m_pbBuildings);
	SAFE_DELETE_ARRAY(m_pbBuildingClassRequireds);
#if defined(MOD_BALANCE_CORE)
	SAFE_DELETE_ARRAY(m_piScalingFromOwnedImprovements);
	SAFE_DELETE_ARRAY(m_pbBuildOnFound);
	SAFE_DELETE_ARRAY(m_pbBuildingClassPurchaseRequireds);
#endif
	SAFE_DELETE_ARRAY(m_piPrereqAndTechs);
	SAFE_DELETE_ARRAY(m_piResourceQuantityRequirements);
	SAFE_DELETE_ARRAY(m_piResourceQuantityExpended);
	SAFE_DELETE_ARRAY(m_piProductionTraits);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piUnitGroupRequired);
	SAFE_DELETE_ARRAY(m_pbFreePromotions);
	SAFE_DELETE_ARRAY(m_paszEarlyArtDefineTags);
	SAFE_DELETE_ARRAY(m_paszLateArtDefineTags);
	SAFE_DELETE_ARRAY(m_paszMiddleArtDefineTags);
	SAFE_DELETE_ARRAY(m_paszUnitNames);
	SAFE_DELETE_ARRAY(m_paeGreatWorks);
#if defined(MOD_UNITS_RESOURCE_QUANTITY_TOTALS)
	m_piResourceQuantityTotals.clear();
#endif
#if defined(MOD_BALANCE_CORE)
	SAFE_DELETE_ARRAY(m_paeGreatPersonEra);
	SAFE_DELETE_ARRAY(m_piEraCombatStrength);
	if(m_ppiEraUnitCombatType != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray(m_ppiEraUnitCombatType);
	}
	if(m_ppiEraUnitPromotions != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray(m_ppiEraUnitPromotions);
	}
#endif

}

bool CvUnitEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	//Basic Properties
	m_iProductionCost = kResults.GetInt("Cost");
	m_iFaithCost = kResults.GetInt("FaithCost");
	m_bRequiresFaithPurchaseEnabled = kResults.GetBool("RequiresFaithPurchaseEnabled");
#if defined(MOD_GLOBAL_EXCLUDE_FROM_GIFTS)
	m_bNoMinorGifts = kResults.GetBool("NoMinorGifts");
#endif
	m_bPurchaseOnly = kResults.GetBool("PurchaseOnly");
	m_bMoveAfterPurchase = kResults.GetBool("MoveAfterPurchase");
#if defined(MOD_GLOBAL_MOVE_AFTER_UPGRADE)
	m_bMoveAfterUpgrade = kResults.GetBool("MoveAfterUpgrade");
#endif
#if defined(MOD_GLOBAL_CANNOT_EMBARK)
	m_bCannotEmbark = kResults.GetBool("CannotEmbark");
#endif
	m_iHurryCostModifier = kResults.GetInt("HurryCostModifier");
	m_iAdvancedStartCost = kResults.GetInt("AdvancedStartCost");
	m_iMinAreaSize = kResults.GetInt("MinAreaSize");
	m_iMoves = kResults.GetInt("Moves");
	m_bMoves = kResults.GetInt("Immobile");
	m_iBaseSightRange = kResults.GetInt("BaseSightRange");
	m_iRange = kResults.GetInt("Range");
	m_iAirInterceptRange = kResults.GetInt("AirInterceptRange");
	m_iBaseLandAirDefense = kResults.GetInt("BaseLandAirDefense");
	m_iAirUnitCap = kResults.GetInt("AirUnitCap");
	m_iNukeDamageLevel = kResults.GetInt("NukeDamageLevel");
	m_iWorkRate = kResults.GetInt("WorkRate");
	m_iNumFreeTechs = kResults.GetInt("NumFreeTechs");
	m_iBaseBeakersTurnsToCount = kResults.GetInt("BaseBeakersTurnsToCount");
	m_iBaseCultureTurnsToCount = kResults.GetInt("BaseCultureTurnsToCount");
	m_iBaseTurnsForGAPToCount = kResults.GetInt("BaseTurnsForGAPToCount");
	m_iBaseHurry = kResults.GetInt("BaseHurry");
	m_iHurryMultiplier = kResults.GetInt("HurryMultiplier");
	m_bRushBuilding= kResults.GetInt("RushBuilding");
	m_iBaseGold = kResults.GetInt("BaseGold");
	m_iScaleFromNumGWs = kResults.GetInt("ScaleFromNumGWs");
	m_iScaleFromNumThemes = kResults.GetInt("ScaleFromNumThemes");
	m_iNumGoldPerEra = kResults.GetInt("NumGoldPerEra");
#if defined(MOD_DIPLOMACY_CITYSTATES)
	m_iNumInfPerEra = kResults.GetInt("NumInfPerEra");
	m_iProductionCostPerEra = kResults.GetInt("ProductionCostAddedPerEra");
#endif
#if defined(MOD_BALANCE_CORE)
	m_iNumFreeLux = kResults.GetInt("NumFreeLux");
	m_bFreeUpgrade = kResults.GetBool("FreeUpgrade");
#endif
	m_bSpreadReligion = kResults.GetBool("SpreadReligion");
	m_bRemoveHeresy = kResults.GetBool("RemoveHeresy");
	m_iReligionSpreads = kResults.GetInt("ReligionSpreads");
	m_iReligiousStrength = kResults.GetInt("ReligiousStrength");
	m_bFoundReligion = kResults.GetBool("FoundReligion");
	m_bRequiresEnhancedReligion = kResults.GetBool("RequiresEnhancedReligion");
	m_bProhibitsSpread = kResults.GetBool("ProhibitsSpread");
#if defined(MOD_CIV6_WORKER)
	m_iBuilderStrength = kResults.GetInt("BuilderStrength");
#endif
	m_bCanBuyCityState = kResults.GetBool("CanBuyCityState");
#if defined(MOD_GLOBAL_SEPARATE_GREAT_ADMIRAL)
	m_bCanRepairFleet = kResults.GetBool("CanRepairFleet");
	m_bCanChangePort = kResults.GetBool("CanChangePort");
#endif
	m_iCombat = kResults.GetInt("Combat");
	m_iCombatLimit = kResults.GetInt("CombatLimit");
	m_iRangedCombat = kResults.GetInt("RangedCombat");
	m_iRangedCombatLimit = kResults.GetInt("RangedCombatLimit");
	m_bCoastalFire = kResults.GetBool("CoastalFireOnly");
#if defined(MOD_UNITS_NO_SUPPLY)
	m_bNoSupply = (kResults.GetInt("NoSupply") != 0);
#endif
#if defined(MOD_UNITS_MAX_HP)
	if (MOD_UNITS_MAX_HP) {
		m_iMaxHitPoints = kResults.GetInt("MaxHitPoints");
	} else {
		m_iMaxHitPoints = GC.getMAX_HIT_POINTS();
	}
#endif
	m_iXPValueAttack = kResults.GetInt("XPValueAttack");
	m_iXPValueDefense = kResults.GetInt("XPValueDefense");
	m_iConscriptionValue = kResults.GetInt("Conscription");
	m_iExtraMaintenanceCost = kResults.GetInt("ExtraMaintenanceCost");
	m_bNoMaintenance = kResults.GetBool("NoMaintenance");
	m_iUnhappiness = kResults.GetInt("Unhappiness");
	m_iUnitFlagIconOffset = kResults.GetInt("UnitFlagIconOffset");
	m_iUnitPortraitOffset = kResults.GetInt("PortraitIndex");
	m_iLeaderExperience = kResults.GetInt("LeaderExperience");
	m_bFoodProduction = kResults.GetBool("Food");
	m_bNoBadGoodies = kResults.GetBool("NoBadGoodies");
	m_bRivalTerritory = kResults.GetBool("RivalTerritory");
	m_bMilitarySupport = kResults.GetBool("MilitarySupport");
	m_bMilitaryProduction = kResults.GetBool("MilitaryProduction");
	m_bPillage = kResults.GetBool("Pillage");
	m_bFound = kResults.GetBool("Found");
	m_bFoundAbroad = kResults.GetBool("FoundAbroad");
#if defined(MOD_BALANCE_CORE)
	m_bFoundMid = kResults.GetBool("FoundMid");
	m_bFoundLate = kResults.GetBool("FoundLate");
	m_iFoundColony = kResults.GetInt("FoundColony");
	m_bIsCityAttackSupport = kResults.GetBool("CityAttackOnly");
	m_iGPExtra = kResults.GetInt("GPExtra");
	m_iGoodyModifier = kResults.GetInt("GoodyModifier");
	m_iSupplyCapBoost = kResults.GetInt("SupplyCapBoost");
#endif
	m_iCultureBombRadius = kResults.GetInt("CultureBombRadius");
	m_iNumberOfCultureBombs = kResults.GetInt("NumberOfCultureBombs");
	m_iGoldenAgeTurns = kResults.GetInt("GoldenAgeTurns");
	m_iFreePolicies = kResults.GetInt("FreePolicies");
	m_iOneShotTourism = kResults.GetInt("OneShotTourism");
	m_iOneShotTourismPercentOthers = kResults.GetInt("OneShotTourismPercentOthers");
	m_bIgnoreBuildingDefense = kResults.GetBool("IgnoreBuildingDefense");
	m_bPrereqResources = kResults.GetBool("PrereqResources");
	m_bMechanized = kResults.GetBool("Mechanized");
	m_bSuicide = kResults.GetBool("Suicide");
	m_bCaptureWhileEmbarked = kResults.GetBool("CaptureWhileEmbarked");
	m_bRangeAttackOnlyInDomain = kResults.GetBool("RangeAttackOnlyInDomain");
	m_bTrade = kResults.GetBool("Trade");
	m_iNumExoticGoods = kResults.GetInt("NumExoticGoods");

	m_strUnitArtInfoTag = kResults.GetText("UnitArtInfo");
	m_bUnitArtInfoCulturalVariation = kResults.GetBool("UnitArtInfoCulturalVariation");
	m_bUnitArtInfoEraVariation = kResults.GetBool("UnitArtInfoEraVariation");

#if defined(MOD_GLOBAL_STACKING_RULES)
	m_iNumberStackingUnits = kResults.GetInt("NumberStackingUnits");
	m_iStackCombat = kResults.GetInt("StackCombat");
#endif
#if defined(MOD_CARGO_SHIPS)
	m_iCargoCombat = kResults.GetInt("CargoCombat");
#endif

	//References
	const char* szTextVal = NULL;
	szTextVal = kResults.GetText("Class");
	m_iUnitClassType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("Special");
	m_iSpecialUnitType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("Capture");
	m_iUnitCaptureClassType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("CombatClass");
	m_iUnitCombatType = GC.getInfoTypeForString(szTextVal, true);

#if defined(MOD_GLOBAL_PROMOTION_CLASSES)
	szTextVal = kResults.GetText("PromotionClass");
	m_iUnitPromotionType = GC.getInfoTypeForString(szTextVal, true);

	if (m_iUnitPromotionType == NO_UNITCOMBAT || !MOD_GLOBAL_PROMOTION_CLASSES) 
	{
		m_iUnitPromotionType = m_iUnitCombatType;
	}
#endif
#if defined(MOD_CARGO_SHIPS)
	szTextVal = kResults.GetText("SpecialUnitCargoLoad");
	m_iSpecialUnitCargoLoad = GC.getInfoTypeForString(szTextVal, true);
#endif
#if defined(MOD_BALANCE_CORE)
	szTextVal = kResults.GetText("ResourceType");
	m_iResourceType = GC.getInfoTypeForString(szTextVal, true);

	m_bPuppetPurchaseOverride = kResults.GetBool("PuppetPurchaseOverride");
	m_bMinorCivGift = kResults.GetBool("MinorCivGift");
	m_bNoMinorCivGift = kResults.GetBool("NoMinorCivGift");

	m_iCooldown = kResults.GetInt("PurchaseCooldown");
	m_iGlobalFaithCooldown = kResults.GetInt("GlobalFaithPurchaseCooldown");
	m_iLocalFaithCooldown = kResults.GetInt("LocalFaithPurchaseCooldown");

	szTextVal = kResults.GetText("FriendlyLandsPromotion");
	m_iFriendlyLandsPromotion = (PromotionTypes)GC.getInfoTypeForString(szTextVal, true);

	m_bIsMounted = kResults.GetBool("IsMounted");

	szTextVal = kResults.GetText("BeliefRequired");
	m_iBeliefUnlock = GC.getInfoTypeForString(szTextVal, true);
	m_bCultureFromExperienceOnDisband = kResults.GetBool("CulExpOnDisbandUpgrade");
	m_bUnitEraUpgrade = kResults.GetBool("UnitEraUpgrade");
	m_bWarOnly = kResults.GetBool("WarOnly");
	m_bWLTKD = kResults.GetBool("WLTKDFromBirth");
	m_bGoldenAge = kResults.GetBool("GoldenAgeFromBirth");
	m_bCultureBoost = kResults.GetBool("CultureBoost");
	m_bExtraAttackHealthOnKill = kResults.GetBool("ExtraAttackHealthOnKill");
	m_bHighSeaRaider = kResults.GetBool("HighSeaRaider");
#endif

#if defined(MOD_EVENTS_CAN_MOVE_INTO)
	m_bSendCanMoveIntoEvent = kResults.GetBool("SendCanMoveIntoEvent");
#endif

	szTextVal = kResults.GetText("Domain");
	m_iDomainType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("CivilianAttackPriority");
	m_iCivilianAttackPriority = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("DefaultUnitAI");
	m_iDefaultUnitAIType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("PillagePrereqTech");
	m_iPrereqPillageTech = GC.getInfoTypeForString(szTextVal, true);
	
	szTextVal = kResults.GetText("PrereqTech");
	m_iPrereqAndTech = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("ObsoleteTech");
	m_iObsoleteTech = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("PolicyType");
	m_iPolicyType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("GoodyHutUpgradeUnitClass");
	m_iGoodyHutUpgradeUnitClass = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("SpecialCargo");
	m_iSpecialCargo = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("DomainCargo");
	m_iDomainCargo = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("ProjectPrereq");
	m_iProjectPrereq = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("SpaceshipProject");
	m_iSpaceshipProject = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("LeaderPromotion");
	m_iLeaderPromotion = GC.getInfoTypeForString(szTextVal, true);

	//Arrays
	const char* szUnitType = GetType();
	const size_t lenUnitType = strlen(szUnitType);

	kUtility.SetFlavors(m_piFlavorValue, "Unit_Flavors", "UnitType", szUnitType);

	kUtility.PopulateArrayByValue(m_piProductionTraits, "Traits", "Unit_ProductionTraits", "TraitType", "UnitType", szUnitType, "Trait");

	kUtility.PopulateArrayByValue(m_piResourceQuantityRequirements, "Resources", "Unit_ResourceQuantityRequirements", "ResourceType", "UnitType", szUnitType, "Cost");
	kUtility.PopulateArrayByValue(m_piResourceQuantityExpended, "Resources", "Unit_ResourceQuantityExpended", "ResourceType", "UnitType", szUnitType, "Amount");
	kUtility.PopulateArrayByValue(m_piProductionModifierBuildings, "Buildings", "Unit_ProductionModifierBuildings", "BuildingType", "UnitType", szUnitType, "ProductionModifier");
	kUtility.PopulateArrayByValue(m_piYieldFromKills, "Yields", "Unit_YieldFromKills", "YieldType", "UnitType", szUnitType, "Yield");
#if defined(MOD_API_UNIFIED_YIELDS)
	kUtility.PopulateArrayByValue(m_piYieldFromBarbarianKills, "Yields", "Unit_YieldFromBarbarianKills", "YieldType", "UnitType", szUnitType, "Yield");
#endif
	kUtility.PopulateArrayByExistence(m_pbFreePromotions, "UnitPromotions", "Unit_FreePromotions", "PromotionType", "UnitType", szUnitType);

	kUtility.PopulateArrayByExistence(m_pbUpgradeUnitClass, "UnitClasses", "Unit_ClassUpgrades", "UnitClassType", "UnitType", szUnitType);

	kUtility.PopulateArrayByExistence(m_pbUnitAIType, "UnitAIInfos", "Unit_AITypes", "UnitAIType", "UnitType", szUnitType);
	kUtility.PopulateArrayByExistence(m_pbNotUnitAIType, "UnitAIInfos", "Unit_NotAITypes", "UnitAIType", "UnitType", szUnitType);

	kUtility.PopulateArrayByExistence(m_pbBuilds, "Builds", "Unit_Builds", "BuildType", "UnitType", szUnitType);
	kUtility.PopulateArrayByExistence(m_pbGreatPeoples, "Specialists", "Unit_GreatPersons", "GreatPersonType", "UnitType", szUnitType);
	kUtility.PopulateArrayByExistence(m_pbBuildings, "Buildings", "Unit_Buildings", "BuildingType", "UnitType", szUnitType);
	kUtility.PopulateArrayByExistence(m_pbBuildingClassRequireds, "BuildingClasses", "Unit_BuildingClassRequireds", "BuildingClassType", "UnitType", szUnitType);
#if defined(MOD_BALANCE_CORE)
	kUtility.PopulateArrayByValue(m_piScalingFromOwnedImprovements, "Improvements", "Unit_ScalingFromOwnedImprovements", "ImprovementType", "UnitType", szUnitType, "Amount");
	kUtility.PopulateArrayByExistence(m_pbBuildOnFound, "BuildingClasses", "Unit_BuildOnFound", "BuildingClassType", "UnitType", szUnitType);
	kUtility.PopulateArrayByExistence(m_pbBuildingClassPurchaseRequireds, "BuildingClasses", "Unit_BuildingClassPurchaseRequireds", "BuildingClassType", "UnitType", szUnitType);
	kUtility.PopulateArrayByValue(m_piEraCombatStrength, "Eras", "Unit_EraCombatStrength", "EraType", "UnitType", szUnitType, "CombatStrength");
#endif
	//TechTypes
	{
		//Initialize array to NO_TECH
		kUtility.InitializeArray(m_piPrereqAndTechs, "Technologies", (int)NO_TECH);

		std::string strKey = "Units - TechTypes";
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Technologies.ID from Unit_TechTypes inner join Technologies on TechType = Technologies.Type where UnitType = ?");
		}

		pResults->Bind(1, szUnitType, -1, false);

		int i = 0;
		while(pResults->Step())
		{
			m_piPrereqAndTechs[i++] = pResults->GetInt(0);
		}

		pResults->Reset();
	}

	//Unit Unique Names Count
	{
		std::string strKey = "Units - UniqueNameCount";
		Database::Results* pUnitNameCount = kUtility.GetResults(strKey);
		if(pUnitNameCount == NULL)
		{
			pUnitNameCount = kUtility.PrepareResults(strKey, "select count(*) from Unit_UniqueNames where UnitType = ?");
		}

		pUnitNameCount->Bind(1, szUnitType, -1, false);

		pUnitNameCount->Step();
		m_iNumUnitNames = pUnitNameCount->GetInt(0);

		pUnitNameCount->Reset();
	}

	//Unit Unique Names
	{
		if(m_iNumUnitNames > 0)
		{
			m_paszUnitNames = FNEW(CvString[m_iNumUnitNames], c_eCiv5GameplayDLL, 0);
			m_paeGreatWorks = FNEW(GreatWorkType[m_iNumUnitNames], c_eCiv5GameplayDLL, 0);
#if defined(MOD_BALANCE_CORE)
			m_paeGreatPersonEra = FNEW(EraTypes[m_iNumUnitNames], c_eCiv5GameplayDLL, 0);
#endif

			std::string strKey = "Units - UniqueNames";
			Database::Results* pResults = kUtility.GetResults(strKey);
			if(pResults == NULL)
			{
#if defined(MOD_BALANCE_CORE)
				pResults = kUtility.PrepareResults(strKey, "select UniqueName, GreatWorkType, EraType from Unit_UniqueNames where UnitType = ? ORDER BY rowid");
#else
				pResults = kUtility.PrepareResults(strKey, "select UniqueName, GreatWorkType from Unit_UniqueNames where UnitType = ? ORDER BY rowid");
#endif
			}

			pResults->Bind(1, szUnitType, -1, false);

			int i = 0;
			while(pResults->Step())
			{
				m_paszUnitNames[i] = pResults->GetText(0);
			
				const char* szGreatWorkType = pResults->GetText(1);
				if(szGreatWorkType == NULL)
				{
					m_paeGreatWorks[i] = NO_GREAT_WORK;
				}
				else
				{
					m_paeGreatWorks[i] = static_cast<GreatWorkType>(GC.getInfoTypeForString(szGreatWorkType, true));
				}
#if defined(MOD_BALANCE_CORE)
				const char* szEraType = pResults->GetText(2);
				if(szEraType == NULL)
				{
					m_paeGreatPersonEra[i] = NO_ERA;
				}
				else
				{
					m_paeGreatPersonEra[i] = static_cast<EraTypes>(GC.getInfoTypeForString(szEraType, true));
				}
#endif
				i++;
			}

			pResults->Reset();
		}
	}

	//MovementRates
	{
		std::string strKey = "Units - MovementRates";
		Database::Results* pMovementRates = kUtility.GetResults(strKey);
		if(pMovementRates == NULL)
		{
			pMovementRates = kUtility.PrepareResults(strKey, "SELECT * FROM MovementRates where Type = ? ORDER BY NumHexes");
		}

		const char* szMovementRate = kResults.GetText("MoveRate");
		pMovementRates->Bind(1, szMovementRate, -1, false);
		while(pMovementRates->Step())
		{
			int f0 = pMovementRates->GetInt("NumHexes") - 1;
			float f1 = pMovementRates->GetFloat("TotalTime");
			float f2 = pMovementRates->GetFloat("EaseIn");
			float f3 = pMovementRates->GetFloat("EaseOut");
			float f4 = pMovementRates->GetFloat("IndividualOffset");
			float f5 = pMovementRates->GetFloat("RowOffset");
			float f6 = pMovementRates->GetFloat("CurveRoll");
			int   iPathSubdivision = pMovementRates->GetInt("PathSubdivision");
			if(f0 >= 0 && f0 <= 11)
			{
				m_unitMoveRate[f0].m_fTotalMoveRate    = f1;
				m_unitMoveRate[f0].m_fEaseIn           = f2;
				m_unitMoveRate[f0].m_fEaseOut          = f3;
				m_unitMoveRate[f0].m_fIndividualOffset = f4;
				m_unitMoveRate[f0].m_fRowOffset        = f5;
				m_unitMoveRate[f0].m_fCurveRoll        = f6;
				m_unitMoveRate[f0].m_iPathSubdivision  = iPathSubdivision;
			}
		}
		pMovementRates->Reset();

	}
#if defined(MOD_BALANCE_CORE)
	//Populate m_ppiEraUnitCombatType
	{
		const int iNumUnitCombats = kUtility.MaxRows("UnitCombatInfos");
		const int iNumEras = kUtility.MaxRows("Eras");
		kUtility.Initialize2DArray(m_ppiEraUnitCombatType, iNumUnitCombats, iNumEras);

		std::string sqlKey = "Units - EraCombatType";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(sqlKey, "select UnitCombatInfos.ID as UnitCombatInfosID, Eras.ID as ErasID, Value from Unit_EraCombatType inner join UnitCombatInfos on UnitCombatType = UnitCombatInfos.Type inner join Eras on EraType = Eras.Type where UnitType = ?");
		}

		pResults->Bind(1, szUnitType, lenUnitType, false);

		while(pResults->Step())
		{
			const int UnitCombatInfosID = pResults->GetInt(0);
			const int ErasID = pResults->GetInt(1);
			const int Value = pResults->GetInt(2);

			m_ppiEraUnitCombatType[UnitCombatInfosID][ErasID] = Value;
		}

		pResults->Reset();
	}

	//Populate m_ppiEraUnitPromotions
	{
		const int iNumPromotions = kUtility.MaxRows("UnitPromotions");
		const int iNumEras = kUtility.MaxRows("Eras");
		kUtility.Initialize2DArray(m_ppiEraUnitPromotions, iNumPromotions, iNumEras);

		std::string sqlKey = "Units - EraUnitPromotions";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(sqlKey, "select UnitPromotions.ID as UnitPromotionsID, Eras.ID as ErasID, Value from Unit_EraUnitPromotions inner join UnitPromotions on PromotionType = UnitPromotions.Type inner join Eras on EraType = Eras.Type where UnitType = ?");
		}

		pResults->Bind(1, szUnitType, lenUnitType, false);

		while(pResults->Step())
		{
			const int UnitPromotionsID = pResults->GetInt(0);
			const int ErasID = pResults->GetInt(1);
			const int Value = pResults->GetInt(2);

			m_ppiEraUnitPromotions[UnitPromotionsID][ErasID] = Value;
		}

		pResults->Reset();
	}

	//Populate m_piResourceQuantityTotals
	{
		std::string strKey("Unit_ResourceQuantityTotals");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Resources.ID as ResourceID, Amount from Unit_ResourceQuantityTotals inner join Resources on Resources.Type = ResourceType where UnitType = ?");
		}

		pResults->Bind(1, szUnitType, lenUnitType, false);

		while (pResults->Step())
		{
			const int iResource = pResults->GetInt(0);
			const int iAmount = pResults->GetInt(1);

			m_piResourceQuantityTotals[iResource] = iAmount;
		}

		pResults->Reset();

		//Trim extra memory off container since this is mostly read-only.
		std::map<int, int>(m_piResourceQuantityTotals).swap(m_piResourceQuantityTotals);
	}
#endif
	// Calculate military Power and cache it
	DoUpdatePower();

	return true;
}

/// Shields to construct the unit
int CvUnitEntry::GetProductionCost() const
{
	return m_iProductionCost;
}

/// Faith to construct the unit (as a percentage of cost of next Great Prophet)
int CvUnitEntry::GetFaithCost() const
{
	return m_iFaithCost;
}

/// Do we need a belief to unlock faith purchasing of this unit?
bool CvUnitEntry::IsRequiresFaithPurchaseEnabled() const
{
	return m_bRequiresFaithPurchaseEnabled;
}

#if defined(MOD_GLOBAL_EXCLUDE_FROM_GIFTS)
/// Can City States gift this unit?
bool CvUnitEntry::IsNoMinorGifts() const
{
	return m_bNoMinorGifts;
}
#endif

/// Do we need to purchase this unit (i.e. can't be built)?
bool CvUnitEntry::IsPurchaseOnly() const
{
	return m_bPurchaseOnly;
}

/// Do we need to purchase this unit (i.e. can't be built)?
bool CvUnitEntry::CanMoveAfterPurchase() const
{
	return m_bMoveAfterPurchase;
}

#if defined(MOD_GLOBAL_MOVE_AFTER_UPGRADE)
/// Can this unit move after being upgraded?
bool CvUnitEntry::CanMoveAfterUpgrade() const
{
	return m_bMoveAfterUpgrade;
}
#endif

#if defined(MOD_GLOBAL_CANNOT_EMBARK)
/// Can this unit embark?
bool CvUnitEntry::CannotEmbark() const
{
	return m_bCannotEmbark;
}
#endif

/// Does it cost extra to hurry this init?
int CvUnitEntry::GetHurryCostModifier() const
{
	return m_iHurryCostModifier;
}

/// Cost if starting midway through game
int CvUnitEntry::GetAdvancedStartCost() const
{
	return m_iAdvancedStartCost;
}

/// Required Plot count of the CvArea this City belongs to (Usually used for Water Units to prevent building them on tiny lakes and such)
int CvUnitEntry::GetMinAreaSize() const
{
	return m_iMinAreaSize;
}

/// Movement points
int CvUnitEntry::GetMoves() const
{
	return m_iMoves;
}

/// Can this Unit move on its own?
bool CvUnitEntry::IsImmobile() const
{
	return m_bMoves;
}

/// Sight range on flat terrain
int CvUnitEntry::GetBaseSightRange() const
{
	return m_iBaseSightRange;
}

/// Air mission range
int CvUnitEntry::GetRange() const
{
	return m_iRange;
}

/// Air interception range
int CvUnitEntry::GetAirInterceptRange() const
{
	return m_iAirInterceptRange;
}
/// Air interception range
int CvUnitEntry::GetBaseLandAirDefense() const
{
	return m_iBaseLandAirDefense;
}

/// How many air units does this count as?
int CvUnitEntry::GetAirUnitCap() const
{
	return m_iAirUnitCap;
}

/// If non-zero this is a nuclear weapon with the specified attack range
int CvUnitEntry::GetNukeDamageLevel() const
{
	return m_iNukeDamageLevel;
}

/// Amount of work performed in a turn
int CvUnitEntry::GetWorkRate() const
{
	return m_iWorkRate;
}

/// How many free Techs does this Unit give us?
int CvUnitEntry::GetNumFreeTechs() const
{
	return m_iNumFreeTechs;
}

/// How many previous turns worth of beakers does this Unit give us?
int CvUnitEntry::GetBaseBeakersTurnsToCount() const
{
	return m_iBaseBeakersTurnsToCount;
}

/// How many previous turns worth of culture does this Unit give us?
int CvUnitEntry::GetBaseCultureTurnsToCount() const
{
	return m_iBaseCultureTurnsToCount;
}

/// How many previous turns worth of culture does this Unit give us?
int CvUnitEntry::GetBaseTurnsForGAPToCount() const
{
	return m_iBaseTurnsForGAPToCount;
}

/// What is the base amount of production provided by this unit?
int CvUnitEntry::GetBaseHurry() const
{
	return m_iBaseHurry;
}

/// Additional production * city population size
int CvUnitEntry::GetHurryMultiplier() const
{
	return m_iHurryMultiplier;
}

/// Ability to complete Building immediately (should replace the next 2 entries eventually)
bool CvUnitEntry::IsRushBuilding() const
{
	return m_bRushBuilding;
}

/// Base boost to gold (for great people)
int CvUnitEntry::GetBaseGold() const
{
	return m_iBaseGold;
}

/// Era boost to gold (for great people)
int CvUnitEntry::GetScaleFromNumGWs() const
{
	return m_iScaleFromNumGWs;
}
/// Era boost to gold (for great people)
int CvUnitEntry::GetScaleFromNumThemes() const
{
	return m_iScaleFromNumThemes;
}

/// Era boost to gold (for great people)
int CvUnitEntry::GetNumGoldPerEra() const
{
	return m_iNumGoldPerEra;
}

#if defined(MOD_DIPLOMACY_CITYSTATES)
/// Era boost to influence (for great people)
int CvUnitEntry::GetNumInfPerEra() const
{
	return m_iNumInfPerEra;
}
int CvUnitEntry::GetProductionCostPerEra() const
{
	return m_iProductionCostPerEra;
}
#endif
#if defined(MOD_BALANCE_CORE)
/// Free Luxuries in Capital (for great people)
int CvUnitEntry::GetNumFreeLux() const
{
	return m_iNumFreeLux;
}
bool CvUnitEntry::IsFreeUpgrade() const
{
	return m_bFreeUpgrade;
}
/// Belief Unlock only (if faith purchasing enabled)
int CvUnitEntry::GetBeliefUnlock() const
{
	return m_iBeliefUnlock;
}
#endif
/// Can this Unit Spread Religion to a City?
bool CvUnitEntry::IsSpreadReligion() const
{
	return m_bSpreadReligion;
}

/// Can this Unit Remove Heresy in a City?
bool CvUnitEntry::IsRemoveHeresy() const
{
	return m_bRemoveHeresy;
}

/// How many times can this Unit Spread Religion?
int CvUnitEntry::GetReligionSpreads() const
{
	return m_iReligionSpreads;
}

/// How strong is this religious unit?
int CvUnitEntry::GetReligiousStrength() const
{
	return m_iReligiousStrength;
}

#if defined(MOD_CIV6_WORKER)
/// How strong is this builder unit?
int CvUnitEntry::GetBuilderStrength() const
{
	return m_iBuilderStrength;
}
#endif

/// Can this Unit Found a Religion?
bool CvUnitEntry::IsFoundReligion() const
{
	return m_bFoundReligion;
}

/// Can this Unit only be bought for a religion that has been enhanced?
bool CvUnitEntry::IsRequiresEnhancedReligion() const
{
	return m_bRequiresEnhancedReligion;
}

/// Does this unit spot religion from spreading into an adjacent city?
bool CvUnitEntry::IsProhibitsSpread() const
{
	return m_bProhibitsSpread;
}

/// Can this unit buy a city state?
bool CvUnitEntry::IsCanBuyCityState() const
{
	return m_bCanBuyCityState;
}

#if defined(MOD_GLOBAL_SEPARATE_GREAT_ADMIRAL)
bool CvUnitEntry::IsCanRepairFleet() const
{
	return m_bCanRepairFleet;
}

bool CvUnitEntry::IsCanChangePort() const
{
	return m_bCanChangePort;
}
#endif

/// Returns combat value
int CvUnitEntry::GetCombat() const
{
	return m_iCombat;
}

/// Sets combat value
void CvUnitEntry::SetCombat(int iNum)
{
	m_iCombat = iNum;
}

/// Maximum damage to enemy
int CvUnitEntry::GetCombatLimit() const
{
	return m_iCombatLimit;
}

/// Bombard combat value
int CvUnitEntry::GetRangedCombat() const
{
	return m_iRangedCombat;
}

/// Maximum damage to enemy in bombard
int CvUnitEntry::GetRangedCombatLimit() const
{
	return m_iRangedCombatLimit;
}

bool CvUnitEntry::IsCoastalFireOnly() const
{
	return m_bCoastalFire;
}

#if defined(MOD_UNITS_NO_SUPPLY)
/// Unit has no supply cost
bool CvUnitEntry::IsNoSupply() const
{
	return m_bNoSupply;
}
#endif

#if defined(MOD_UNITS_MAX_HP)
/// Maximum hit points, usually 100
int CvUnitEntry::GetMaxHitPoints() const
{
	return m_iMaxHitPoints;
}
#endif

/// Experience point value when attacking
int CvUnitEntry::GetXPValueAttack() const
{
	return m_iXPValueAttack;
}

/// Experience point value when defending
int CvUnitEntry::GetXPValueDefense() const
{
	return m_iXPValueDefense;
}

/// Is there a special unit this unit carries (e.g. Nuclear Sub carries Nuclear missile)
int CvUnitEntry::GetSpecialCargo() const
{
	return m_iSpecialCargo;
}

/// Allows this Special unit type to be loaded onto a cargo ship (e.g. Great People)
#if defined(MOD_CARGO_SHIPS)
int CvUnitEntry::GetSpecialUnitCargoLoad() const
{
	return m_iSpecialUnitCargoLoad;
}
#endif

/// Is there a class of units (domain) that this unit carries
int CvUnitEntry::GetDomainCargo() const
{
	return m_iDomainCargo;
}

/// Cost to conscript this unit
int CvUnitEntry::GetConscriptionValue() const
{
	return m_iConscriptionValue;
}

/// Extra cost for unit maintenance in Gold (deducted every turn)
int CvUnitEntry::GetExtraMaintenanceCost() const
{
	return m_iExtraMaintenanceCost;
}

/// No unit maintenance in Gold (deducted every turn)
bool CvUnitEntry::IsNoMaintenance() const
{
	return m_bNoMaintenance;
}

/// Amount of Happiness used up supporting this Unit
int CvUnitEntry::GetUnhappiness() const
{
	return m_iUnhappiness;
}

/// Class of unit
int CvUnitEntry::GetUnitClassType() const
{
	return m_iUnitClassType;
}

/// Special class of this unit (if any)
int CvUnitEntry::GetSpecialUnitType() const
{
	return m_iSpecialUnitType;
}

/// If captured, what unit does it become?
int CvUnitEntry::GetUnitCaptureClassType() const
{
	return m_iUnitCaptureClassType;
}

/// Combat type (melee, mounted, siege, air, etc.)
int CvUnitEntry::GetUnitCombatType() const
{
	return m_iUnitCombatType;
}

#if defined(MOD_GLOBAL_PROMOTION_CLASSES)
/// Combat type for promotions - this permits subs and anti-air units to receive more meaningful promotion trees
int CvUnitEntry::GetUnitPromotionType() const
{
	return m_iUnitPromotionType;
}
#endif

#if defined(MOD_EVENTS_CAN_MOVE_INTO)
/// Send CanMoveInto events for this unit type?
bool CvUnitEntry::IsSendCanMoveIntoEvent() const
{
	return m_bSendCanMoveIntoEvent;
}
#endif

/// If this is a civilian, what is our priority to attack it?
int CvUnitEntry::GetCivilianAttackPriority() const
{
	return m_iCivilianAttackPriority;
}

/// Default AI behavior
UnitAITypes CvUnitEntry::GetDefaultUnitAIType() const
{
	return (UnitAITypes)m_iDefaultUnitAIType;
}

/// Tech needed to pillage (if we can pillage)
int CvUnitEntry::GetPrereqPillageTech() const
{
	return m_iPrereqPillageTech;
}

/// Prerequisite techs with AND
int CvUnitEntry::GetPrereqAndTech() const
{
	return m_iPrereqAndTech;
}

/// Tech that makes this unit obsolete
int CvUnitEntry::GetObsoleteTech() const
{
	return m_iObsoleteTech;
}

#if defined(MOD_BALANCE_CORE)
/// Era this unit belongs to
int CvUnitEntry::GetEra() const
{
	TechTypes eTech = (TechTypes)GetPrereqAndTech();
	if (eTech != NO_TECH)
	{
		CvTechEntry* pTech = GC.getTechInfo((TechTypes)GetPrereqAndTech());
		return pTech->GetEra();
	}

	return -1;
}
#endif

/// Policy required for this unit
int CvUnitEntry::GetPolicyType() const
{
	return m_iPolicyType;
}

/// Unitclass that replaces this Unit if the appropriate Goody is received from a Hut
int CvUnitEntry::GetGoodyHutUpgradeUnitClass() const
{
	return m_iGoodyHutUpgradeUnitClass;
}

/// Initial number of individuals in the unit group
int CvUnitEntry::GetGroupSize() const
{
	return m_iGroupSize;
}

/// Number of UnitMeshGroups for this unit
int CvUnitEntry::GetGroupDefinitions() const
{
	return m_iGroupDefinitions;
}

/// Maximum number of attackers in one round (melee combat)
int CvUnitEntry::GetMeleeWaveSize() const
{
	return m_iUnitMeleeWaveSize;
}

/// Maximum number of attackers in one round (ranged combat)
int CvUnitEntry::GetRangedWaveSize() const
{
	return m_iUnitRangedWaveSize;
}

/// Individual names for this unit (for great people)
int CvUnitEntry::GetNumUnitNames() const
{
	return m_iNumUnitNames;
}

/// Is food used to produce this unit?
bool CvUnitEntry::IsFoodProduction() const
{
	return m_bFoodProduction;
}

/// Does this unit never encounter a bad goodie hut?
bool CvUnitEntry::IsNoBadGoodies() const
{
	return m_bNoBadGoodies;
}

/// Prohibited from enemy territory
bool CvUnitEntry::IsRivalTerritory() const
{
	return m_bRivalTerritory;
}

/// Does it require military supplies?
bool CvUnitEntry::IsMilitarySupport() const
{
	return m_bMilitarySupport;
}

/// Is this considered a military unit?
bool CvUnitEntry::IsMilitaryProduction() const
{
	return m_bMilitaryProduction;
}

/// Can this unit pillage?
bool CvUnitEntry::IsPillage() const
{
	return m_bPillage;
}

/// Can it start a city?
bool CvUnitEntry::IsFound() const
{
	return m_bFound;
}

/// Can it start a city on a continent other than one with capital?
bool CvUnitEntry::IsFoundAbroad() const
{
	return m_bFoundAbroad;
}

#if defined(MOD_BALANCE_CORE_SETTLER_ADVANCED)
/// Can it start a city in the mid game?
bool CvUnitEntry::IsFoundMid() const
{
	return m_bFoundMid;
}
/// Can it start a city in the late game?
bool CvUnitEntry::IsFoundLate() const
{
	return m_bFoundLate;
}

int CvUnitEntry::GetNumColonyFound() const
{
	return m_iFoundColony;
}

/// City Attack Only?
bool CvUnitEntry::IsCityAttackSupport() const
{
	return m_bIsCityAttackSupport;
}
int CvUnitEntry::IsGPExtra() const
{
	return m_iGPExtra;
}

int CvUnitEntry::GetGoodyModifier() const
{
	return m_iGoodyModifier;
}
int CvUnitEntry::GetSupplyCapBoost() const
{
	return m_iSupplyCapBoost;
}

#endif

/// Distance this unit steals
int CvUnitEntry::GetCultureBombRadius() const
{
	return m_iCultureBombRadius;
}
int CvUnitEntry::GetNumberOfCultureBombs() const
{
	return m_iNumberOfCultureBombs;
}

/// Number of GA turns this Unit can give us
int CvUnitEntry::GetGoldenAgeTurns() const
{
	return m_iGoldenAgeTurns;
}

/// Number of free policies this Unit can give us
int CvUnitEntry::GetFreePolicies() const
{
	return m_iFreePolicies;
}

/// Size of one-shot tourism blast when unit expended
int CvUnitEntry::GetOneShotTourism() const
{
	return m_iOneShotTourism;
}

/// Trickle over to civs other than primary target from one-shot tourism blast
int CvUnitEntry::GetOneShotTourismPercentOthers() const
{
	return m_iOneShotTourismPercentOthers;
}

/// Not affected by walls?
bool CvUnitEntry::IsIgnoreBuildingDefense() const
{
	return m_bIgnoreBuildingDefense;
}

/// Does it open up resources (e.g. workboat)
bool CvUnitEntry::IsPrereqResources() const
{
	return m_bPrereqResources;
}

/// Mechanized unit?
bool CvUnitEntry::IsMechUnit() const
{
	return m_bMechanized;
}

/// Suicide attack unit?
bool CvUnitEntry::IsSuicide() const
{
	return m_bSuicide;
}

/// Capture this unit even if he's embarked?
bool CvUnitEntry::IsCaptureWhileEmbarked() const
{
	return m_bCaptureWhileEmbarked;
}

/// Only have a ranged attack in their own domain?
bool CvUnitEntry::IsRangeAttackOnlyInDomain() const
{
	return m_bRangeAttackOnlyInDomain;
}

/// Is a international trade unit
bool CvUnitEntry::IsTrade() const
{
	return m_bTrade;
}

/// Number of exotic goods this unit starts with
int CvUnitEntry::GetNumExoticGoods() const
{
	return m_iNumExoticGoods;
}

/// Return unit's current command
int CvUnitEntry::GetCommandType() const
{
	return m_iCommandType;
}

/// Set unit's current command
void CvUnitEntry::SetCommandType(int iNewType)
{
	m_iCommandType = iNewType;
}

/// Set unit's ability to stack with another combat unit
#if defined(MOD_GLOBAL_STACKING_RULES)
int CvUnitEntry::GetNumberStackingUnits() const
{
	return m_iNumberStackingUnits;
}

int CvUnitEntry::StackCombat() const
{
	return m_iStackCombat;
}
bool CvUnitEntry::IsCultureFromExperienceDisbandUpgrade() const
{
	return m_bCultureFromExperienceOnDisband;
}
bool CvUnitEntry::IsUnitEraUpgrade() const
{
	return m_bUnitEraUpgrade;
}
bool CvUnitEntry::IsWarOnly() const
{
	return m_bWarOnly;
}
bool CvUnitEntry::IsWLTKDFromBirth() const
{
	return m_bWLTKD;
}
bool CvUnitEntry::IsGoldenAgeFromBirth() const
{
	return m_bGoldenAge;
}
bool CvUnitEntry::IsCultureBoost() const
{
	return m_bCultureBoost;
}
bool CvUnitEntry::IsExtraAttackHealthOnKill() const
{
	return m_bExtraAttackHealthOnKill;
}
bool CvUnitEntry::IsHighSeaRaider() const
{
	return m_bHighSeaRaider;
}
#endif
#if defined(MOD_CARGO_SHIPS)
int CvUnitEntry::CargoCombat() const
{
	return m_iCargoCombat;
}
#endif

// ARRAYS

/// Prerequisite techs with AND
int CvUnitEntry::GetPrereqAndTechs(int i) const
{
	CvAssertMsg(i < GC.getNUM_UNIT_AND_TECH_PREREQS(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piPrereqAndTechs ? m_piPrereqAndTechs[i] : -1;
}

/// Resources consumed to construct
int CvUnitEntry::GetResourceQuantityRequirement(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piResourceQuantityRequirements ? m_piResourceQuantityRequirements[i] : -1;
}

/// Resources consumed to construct
int CvUnitEntry::GetResourceQuantityExpended(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piResourceQuantityExpended ? m_piResourceQuantityExpended[i] : -1;
}

/// Production boost for having a specific building in city
int CvUnitEntry::GetBuildingProductionModifier(BuildingTypes eBuilding) const
{
	CvAssertMsg((int)eBuilding < GC.getNumBuildingInfos(), "Building type out of bounds");
	CvAssertMsg((int)eBuilding > -1, "Index out of bounds");
	return m_piProductionModifierBuildings[(int)eBuilding];
}

/// Do we get one of our yields from defeating an enemy?
int CvUnitEntry::GetYieldFromKills(YieldTypes eYield) const
{
	CvAssertMsg((int)eYield < NUM_YIELD_TYPES, "Yield type out of bounds");
	CvAssertMsg((int)eYield > -1, "Index out of bounds");
	return m_piYieldFromKills[(int)eYield];
}

#if defined(MOD_API_UNIFIED_YIELDS)
/// Do we get one of our yields from defeating a barbarian?
int CvUnitEntry::GetYieldFromBarbarianKills(YieldTypes eYield) const
{
	CvAssertMsg((int)eYield < NUM_YIELD_TYPES, "Yield type out of bounds");
	CvAssertMsg((int)eYield > -1, "Index out of bounds");
	return m_piYieldFromBarbarianKills[(int)eYield];
}
#endif

/// Boost in production for leader with this trait
int CvUnitEntry::GetProductionTraits(int i) const
{
	CvAssertMsg(i < GC.getNumTraitInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piProductionTraits ? m_piProductionTraits[i] : -1;
}

/// Find value of flavors associated with this building
int CvUnitEntry::GetFlavorValue(int i) const
{
	CvAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}

int CvUnitEntry::GetUnitGroupRequired(int i) const
{
	CvAssertMsg(i < GetGroupDefinitions(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piUnitGroupRequired ? m_piUnitGroupRequired[i] : NULL;
}

/// What can this unit upgrade into?
bool CvUnitEntry::GetUpgradeUnitClass(int i) const
{
	CvAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbUpgradeUnitClass ? m_pbUpgradeUnitClass[i] : false;
}

/// What AIs strategies can this unit adopt
bool CvUnitEntry::GetUnitAIType(int i) const
{
	CvAssertMsg(i < NUM_UNITAI_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbUnitAIType ? m_pbUnitAIType[i] : false;
}

/// AI strategies this unit can NOT adopt
bool CvUnitEntry::GetNotUnitAIType(int i) const
{
	CvAssertMsg(i < NUM_UNITAI_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbNotUnitAIType ? m_pbNotUnitAIType[i] : false;
}

/// What improvements can this unit build?
bool CvUnitEntry::GetBuilds(int i) const
{
	CvAssertMsg(i < GC.getNumBuildInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbBuilds ? m_pbBuilds[i] : false;
}

/// Type(s) of great people represented by this unit
bool CvUnitEntry::GetGreatPeoples(int i) const
{
	CvAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbGreatPeoples ? m_pbGreatPeoples[i] : false;
}

/// Is this unit required to construct a certain building?
bool CvUnitEntry::GetBuildings(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbBuildings ? m_pbBuildings[i] : false;
}

/// Does this Unit need a certain BuildingClass in this City to train?
bool CvUnitEntry::GetBuildingClassRequireds(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbBuildingClassRequireds ? m_pbBuildingClassRequireds[i] : false;
}


int CvUnitEntry::GetScalingFromOwnedImprovements(int i) const
{
	CvAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piScalingFromOwnedImprovements ? m_piScalingFromOwnedImprovements[i] : -1;
}

#if defined(MOD_BALANCE_CORE)
/// Does this Unit create something when it founds a city?
bool CvUnitEntry::GetBuildOnFound(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbBuildOnFound? m_pbBuildOnFound[i] : false;
}
/// Does this Unit need a certain BuildingClass in this City to purchase?
bool CvUnitEntry::GetBuildingClassPurchaseRequireds(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbBuildingClassPurchaseRequireds ? m_pbBuildingClassPurchaseRequireds[i] : false;
}
/// Does this Unit get a new combat strength when reaching a new Era?
int CvUnitEntry::GetEraCombatStrength(int i) const
{
	CvAssertMsg(i < GC.getNumEraInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piEraCombatStrength ? m_piEraCombatStrength[i] : -1;
}

/// Accessor:: Does this Unit have a different CombatType in a new Era?
int CvUnitEntry::GetUnitNewEraCombatType(int i, int j) const
{
	CvAssertMsg(i < GC.getNumUnitCombatClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < GC.getNumEraInfos(), "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiEraUnitCombatType ? m_ppiEraUnitCombatType[i][j] : 0;
}
int* CvUnitEntry::GetUnitNewEraCombatTypeChangesArray(int i)
{
	return m_ppiEraUnitCombatType[i];
}
/// Accessor:: Does this Unit get promotions in a new Era?
int CvUnitEntry::GetUnitNewEraPromotions(int i, int j) const
{
	CvAssertMsg(i < GC.getNumPromotionInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < GC.getNumEraInfos(), "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiEraUnitPromotions ? m_ppiEraUnitPromotions[i][j] : 0;
}

int* CvUnitEntry::GetUnitNewEraPromotionsChangesArray(int i)
{
	return m_ppiEraUnitPromotions[i];
}
#endif

#if defined(MOD_UNITS_RESOURCE_QUANTITY_TOTALS)
/// Player must have gross number of resources to build (does not consume)
int CvUnitEntry::GetResourceQuantityTotal(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	std::map<int, int>::const_iterator itResource = m_piResourceQuantityTotals.find(i);
	if (itResource != m_piResourceQuantityTotals.end()) // find returns the iterator to map::end if the key iResource is not present in the map
	{
		return itResource->second;
	}

	return 0;
}
#endif

/// Initial set of promotions for this unit
bool CvUnitEntry::GetFreePromotions(int i) const
{
	CvAssertMsg(i < GC.getNumPromotionInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbFreePromotions ? m_pbFreePromotions[i] : false;
}

/// Project required to train this unit?
int CvUnitEntry::GetProjectPrereq() const
{
	return m_iProjectPrereq;
}

/// Does this unit build a spaceship project?
int CvUnitEntry::GetSpaceshipProject() const
{
	return m_iSpaceshipProject;
}

/// Is this a great leader (general) promotion unit?
int CvUnitEntry::GetLeaderPromotion() const
{
	return m_iLeaderPromotion;
}

/// What is the experience bonus provided by this great leader?
int CvUnitEntry::GetLeaderExperience() const
{
	return m_iLeaderExperience;
}

/// Return art tag
const char* CvUnitEntry::GetUnitArtInfoTag() const
{
	return m_strUnitArtInfoTag;
}

/// Return whether we should try to find a culture specific variant art tag
const bool CvUnitEntry::GetUnitArtInfoCulturalVariation() const
{
	return m_bUnitArtInfoCulturalVariation;
}


/// Return whether we should try to find an era specific variant art tag
const bool CvUnitEntry::GetUnitArtInfoEraVariation() const
{
	return m_bUnitArtInfoEraVariation;
}


/// Unique names for individual units (for great people)
const char* CvUnitEntry::GetUnitNames(int i) const
{
	CvAssertMsg(i < GetNumUnitNames(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return (m_paszUnitNames) ? m_paszUnitNames[i] : NULL;
}

/// Unique great works created by individual units.
GreatWorkType CvUnitEntry::GetGreatWorks(int i) const
{
	CvAssertMsg(i < GetNumUnitNames(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return (m_paeGreatWorks) ? m_paeGreatWorks[i] : NO_GREAT_WORK;
}

bool CvUnitEntry::IsGreatWorkUnit() const
{
	for (int i = 0; i < GetNumUnitNames(); i++)
	{
		if (GetGreatWorks(i) != NO_GREAT_WORK)
			return true;
	}
	return false;
}
#if defined(MOD_BALANCE_CORE)
/// Unique era for individual units.
EraTypes CvUnitEntry::GetGreatPersonEra(int i) const
{
	CvAssertMsg(i < GetNumUnitNames(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return (m_paeGreatPersonEra) ? m_paeGreatPersonEra[i] : NO_ERA;
}
/// Resource required for this unit
int CvUnitEntry::GetResourceType() const
{
	return m_iResourceType;
}
/// Can be bough in a puppet city.
bool CvUnitEntry::IsPuppetPurchaseOverride() const
{
	return m_bPuppetPurchaseOverride;
}
/// Is a Minor Civ Gift Unit
bool CvUnitEntry::IsMinorCivGift() const
{
	return m_bMinorCivGift;
}
bool CvUnitEntry::IsInvalidMinorCivGift() const
{
	return m_bNoMinorCivGift;
}
/// Is mounted
bool CvUnitEntry::IsMounted() const
{
	return m_bIsMounted;
}
/// Purchase cooldown for this unit.
int CvUnitEntry::GetCooldown() const
{
	return m_iCooldown;
}
/// Purchase cooldown for this unit.
int CvUnitEntry::GetGlobalFaithCooldown() const
{
	return m_iGlobalFaithCooldown;
}
/// Local faith purchase cooldown for this unit.
int CvUnitEntry::GetLocalFaithCooldown() const
{
	return m_iLocalFaithCooldown;
}
PromotionTypes CvUnitEntry::GetFriendlyLandsPromotion() const
{
	return (PromotionTypes)m_iFriendlyLandsPromotion;
}
#endif
/// What flag icon to use
int CvUnitEntry::GetUnitFlagIconOffset() const
{
	return m_iUnitFlagIconOffset;
}


/// What portrait to use
int CvUnitEntry::GetUnitPortraitOffset() const
{
	return m_iUnitPortraitOffset;
}


// DERIVED FIELDS

/// Total cargo space from all free promotions
int CvUnitEntry::GetCargoSpace() const
{
	int rtnValue = 0;

	for(int iLoop = 0; iLoop < GC.getNumPromotionInfos(); iLoop++)
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(iLoop);
		CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
		if(pkPromotionInfo)
		{
			if(GetFreePromotions(iLoop))
			{
				const int iVal = pkPromotionInfo->GetCargoChange();
				rtnValue += iVal;
			}
		}
	}

	return rtnValue;
}

/// Military might or "power" - returns cache
int CvUnitEntry::GetPower() const
{
	return m_iCachedPower;
}

/// Update military Power
void CvUnitEntry::DoUpdatePower()
{
	int iPower;

// ***************
// Main Factors - Strength & Moves
// ***************

	// We want a Unit that has twice the strength to be roughly worth 3x as much with regards to Power
	iPower = int(pow((double) GetCombat(), 1.5));

	// Ranged Strength
	int iRangedStrength = int(pow((double) GetRangedCombat(), 1.45));

	// Naval ranged attacks are less useful
	if(GetDomainType() == DOMAIN_SEA)
	{
#if defined(MOD_BUGFIX_UNIT_POWER_CALC)
		if (!MOD_BUGFIX_UNIT_POWER_CALC)
		{
#if defined(MOD_BUGFIX_UNIT_POWER_NAVAL_CONSISTENCY)
			if (!MOD_BUGFIX_UNIT_POWER_NAVAL_CONSISTENCY)
			{
				// We can either ignore this or divide naval melee attacks by two, but if we leave this alone Destroyers are more than twice as powerful as Battleships!!!
#endif
#endif
				iRangedStrength *= 3;
				iRangedStrength /= 4;
#if defined(MOD_BUGFIX_UNIT_POWER_CALC)
#if defined(MOD_BUGFIX_UNIT_POWER_NAVAL_CONSISTENCY)
			}
#endif
		}
#endif
	}

	if(iRangedStrength > iPower)
	{
		iPower = iRangedStrength;
	}

	// We want Movement rate to be important, but not a dominating factor; a Unit with double the moves of a similarly-strengthed Unit should be ~1.5x as Powerful
	iPower = int((float) iPower * pow(min(1.0,(double) GetMoves()), 0.3));
#if defined(MOD_BUGFIX_UNIT_POWER_CALC)
	if (IsImmobile()) {
		// A unit that can't move should be worth half of one that can
		iPower /= 2;
	}
#endif

// ***************
// Other modifiers
// ***************

	// Suicide Units are obviously less useful
	if(IsSuicide())
	{
		iPower /= 2;
	}

	// Nukes are cool
	if(GetNukeDamageLevel() > 0)
	{
		iPower += 4000;
	}

// ***************
// Promotion modifiers
// ***************

	int iTemp;
	int iLoop;

#if defined(MOD_BUGFIX_UNIT_POWER_CALC)
	if (MOD_BUGFIX_UNIT_POWER_CALC) {
		int iBasePower = iPower;
		int iBonusPower = 0;

	for(int iPromotionLoop = 0; iPromotionLoop < GC.getNumPromotionInfos(); iPromotionLoop++)
	{
		CvPromotionEntry* kPromotion = GC.getPromotionInfo((PromotionTypes)iPromotionLoop);
		if(kPromotion == NULL)
			continue;

		if(GetFreePromotions(iPromotionLoop))
		{
			// City Attack - add half of the bonus
			if(kPromotion->GetCityAttackPercent() > 0)
			{
				iTemp = (iBasePower * kPromotion->GetCityAttackPercent() / 2);
				iTemp /= 100;
				iBonusPower += iTemp;
			}

			// Attack - add half of the bonus
			if(kPromotion->GetAttackMod() > 0)
			{
				iTemp = (iBasePower * kPromotion->GetAttackMod() / 2);
				iTemp /= 100;
				iBonusPower += iTemp;
			}

			// Defense - add half of the bonus
			if(kPromotion->GetDefenseMod() > 0)
			{
				iTemp = (iBasePower * kPromotion->GetDefenseMod() / 2);
				iTemp /= 100;
				iBonusPower += iTemp;
			}

			// Paradrop - add 25%
			if(kPromotion->GetDropRange() > 0)
			{
				iTemp = iBasePower;
				iTemp /= 4;
				iBonusPower += iTemp;
			}

			// Blitz - add 20%
			if(kPromotion->IsBlitz())
			{
				iTemp = iBasePower;
				iTemp /= 5;
				iBonusPower += iTemp;
			}

			// Set Up For Ranged Attack - reduce by 20%
			if(kPromotion->IsMustSetUpToRangedAttack())
			{
				iTemp = iBasePower;
				iTemp /= 5;
				iBonusPower -= iTemp;
			}

			// Only Defensive - reduce  by 25%, but only if the Unit has no ranged capability
			if(kPromotion->IsOnlyDefensive() && GetRangedCombat() == 0)
			{
				iTemp = iBasePower;
				iTemp /= 4;
				iBonusPower -= iTemp;
			}

			for(iLoop = 0; iLoop < GC.getNumTerrainInfos(); iLoop++)
			{
				// Terrain Attack - add one quarter of the bonus
				if(kPromotion->GetTerrainAttackPercent(iLoop) > 0)
				{
					iTemp = (iBasePower * kPromotion->GetTerrainAttackPercent(iLoop) / 4);
					iTemp /= 100;
					iBonusPower += iTemp;
				}
				// Terrain Defense - add one quarter of the bonus
				if(kPromotion->GetTerrainDefensePercent(iLoop) > 0)
				{
					iTemp = (iBasePower * kPromotion->GetTerrainDefensePercent(iLoop) / 4);
					iTemp /= 100;
					iBonusPower += iTemp;
				}
			}

			for(iLoop = 0; iLoop < GC.getNumFeatureInfos(); iLoop++)
			{
				// Feature Attack - add one quarter of the bonus
				if(kPromotion->GetFeatureAttackPercent(iLoop) > 0)
				{
					iTemp = (iBasePower * kPromotion->GetFeatureAttackPercent(iLoop) / 4);
					iTemp /= 100;
					iBonusPower += iTemp;
				}
				// Feature Defense - add one quarter of the bonus
				if(kPromotion->GetFeatureDefensePercent(iLoop) > 0)
				{
					iTemp = (iBasePower * kPromotion->GetFeatureDefensePercent(iLoop) / 4);
					iTemp /= 100;
					iBonusPower += iTemp;
				}
			}

#if defined(MOD_BUGFIX_UNITCOMBAT_BONUS_VS_DOMAIN_ONLY)
			if (MOD_BUGFIX_UNITCOMBAT_BONUS_VS_DOMAIN_ONLY) {
				// Only add UNITCOMBAT bonuses against the same domain, so we need to know the domain of combat classes
				//   DOMAIN_SEA is UNITCOMBAT_NAVALRANGED and UNITCOMBAT_NAVALMELEE
				//   DOMAIN_AIR is UNITCOMBAT_HELICOPTER, UNITCOMBAT_FIGHTER and UNITCOMBAT_BOMBER
				//   DOMAIN_LAND is everything else
				int iMyDomain = GetDomainType();
				for(iLoop = 0; iLoop < GC.getNumUnitCombatClassInfos(); iLoop++)
				{
					int iTheirDomain = NO_DOMAIN;
					CvBaseInfo* pkUnitCombatInfo = GC.getUnitCombatClassInfo((UnitCombatTypes)iLoop);
					if (pkUnitCombatInfo) {
						int iTheirUnitCombat = pkUnitCombatInfo->GetID();
					
						if (iTheirUnitCombat == GC.getInfoTypeForString("UNITCOMBAT_NAVALRANGED", true) || iTheirUnitCombat == GC.getInfoTypeForString("UNITCOMBAT_NAVALMELEE", true)) {
							iTheirDomain = DOMAIN_SEA;
						} else if (iTheirUnitCombat == GC.getInfoTypeForString("UNITCOMBAT_HELICOPTER", true) || iTheirUnitCombat == GC.getInfoTypeForString("UNITCOMBAT_FIGHTER", true) || iTheirUnitCombat == GC.getInfoTypeForString("UNITCOMBAT_BOMBER", true)) {
							iTheirDomain = DOMAIN_AIR;
						} else {
							iTheirDomain = DOMAIN_LAND;
						}
					}

					if (iMyDomain == iTheirDomain) {
						// Unit Combat Class (e.g. Pikemen) - add one quarter of the bonus
						if(kPromotion->GetUnitCombatModifierPercent(iLoop) > 0)
						{
							iTemp = (iBasePower * kPromotion->GetUnitCombatModifierPercent(iLoop) / 4);
							iTemp /= 100;
							iBonusPower += iTemp;
						}
					}
				}
			} else {
#endif
				for(iLoop = 0; iLoop < GC.getNumUnitCombatClassInfos(); iLoop++)
				{
					// Unit Combat Class (e.g. Pikemen) - add one quarter of the bonus
					if(kPromotion->GetUnitCombatModifierPercent(iLoop) > 0)
					{
						iTemp = (iBasePower * kPromotion->GetUnitCombatModifierPercent(iLoop) / 4);
						iTemp /= 100;
						iBonusPower += iTemp;
					}
				}
#if defined(MOD_BUGFIX_UNITCOMBAT_BONUS_VS_DOMAIN_ONLY)
			}
#endif

			for(iLoop = 0; iLoop < GC.getNumUnitClassInfos(); iLoop++)
			{
				// Unit Class (e.g. bonus ONLY against Galleys) - add one eighth of the bonus
				// We're assuming here that the bonus against the other Unit is at least going to be somewhat useful - trust the XML! :o
				if(kPromotion->GetUnitClassModifierPercent(iLoop) > 0)
				{
					iTemp = (iBasePower * kPromotion->GetUnitClassModifierPercent(iLoop) / 8);
					iTemp /= 100;
					iBonusPower += iTemp;
				}
				// Unit Class Attack - one tenth of the bonus
				if(kPromotion->GetUnitClassAttackModifier(iLoop) > 0)
				{
					iTemp = (iBasePower * kPromotion->GetUnitClassAttackModifier(iLoop) / 10);
					iTemp /= 100;
					iBonusPower += iTemp;
				}
				// Unit Class Defense - one tenth of the bonus
				if(kPromotion->GetUnitClassDefenseModifier(iLoop) > 0)
				{
					iTemp = (iBasePower * kPromotion->GetUnitClassDefenseModifier(iLoop) / 10);
					iTemp /= 100;
					iBonusPower += iTemp;
				}
			}

			for(iLoop = 0; iLoop < NUM_DOMAIN_TYPES; iLoop++)
			{
				// Domain - add one quarter of the bonus
				if(kPromotion->GetDomainModifierPercent(iLoop) > 0)
				{
					iTemp = (iBasePower * kPromotion->GetDomainModifierPercent(iLoop) / 4);
					iTemp /= 100;
					iBonusPower += iTemp;
				}
			}
		}
	}

	iPower = iBasePower + iBonusPower;

	} else {
#endif

	for(int iPromotionLoop = 0; iPromotionLoop < GC.getNumPromotionInfos(); iPromotionLoop++)
	{
		CvPromotionEntry* kPromotion = GC.getPromotionInfo((PromotionTypes)iPromotionLoop);
		if(kPromotion == NULL)
			continue;

		if(GetFreePromotions(iPromotionLoop))
		{
			// City Attack - add half of the bonus
			if(kPromotion->GetCityAttackPercent() > 0)
			{
				iTemp = (iPower * kPromotion->GetCityAttackPercent() / 2);
				iTemp /= 100;
				iPower += iTemp;
			}

			// Attack - add half of the bonus
			if(kPromotion->GetAttackMod() > 0)
			{
				iTemp = (iPower * kPromotion->GetAttackMod() / 2);
				iTemp /= 100;
				iPower += iTemp;
			}

			// Defense - add half of the bonus
			if(kPromotion->GetDefenseMod() > 0)
			{
				iTemp = (iPower * kPromotion->GetDefenseMod() / 2);
				iTemp /= 100;
				iPower += iTemp;
			}

			// Paradrop - add 25%
			if(kPromotion->GetDropRange() > 0)
			{
				iTemp = iPower;
				iTemp /= 4;
				iPower += iTemp;
			}

			// Blitz - add 20%
			if(kPromotion->IsBlitz())
			{
				iTemp = iPower;
				iTemp /= 5;
				iPower += iTemp;
			}

			// Set Up For Ranged Attack - reduce by 20%
			if(kPromotion->IsMustSetUpToRangedAttack())
			{
				iTemp = iPower;
				iTemp /= 5;
				iPower -= iTemp;
			}

			// Only Defensive - reduce  by 25%, but only if the Unit has no ranged capability
			if(kPromotion->IsOnlyDefensive() && GetRangedCombat() == 0)
			{
				iTemp = iPower;
				iTemp /= 4;
				iPower -= iTemp;
			}

			for(iLoop = 0; iLoop < GC.getNumTerrainInfos(); iLoop++)
			{
				// Terrain Attack - add one quarter of the bonus
				if(kPromotion->GetTerrainAttackPercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetTerrainAttackPercent(iLoop) / 4);
					iTemp /= 100;
					iPower += iTemp;
				}
				// Terrain Defense - add one quarter of the bonus
				if(kPromotion->GetTerrainDefensePercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetTerrainDefensePercent(iLoop) / 4);
					iTemp /= 100;
					iPower += iTemp;
				}
			}

			for(iLoop = 0; iLoop < GC.getNumFeatureInfos(); iLoop++)
			{
				// Feature Attack - add one quarter of the bonus
				if(kPromotion->GetFeatureAttackPercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetFeatureAttackPercent(iLoop) / 4);
					iTemp /= 100;
					iPower += iTemp;
				}
				// Feature Defense - add one quarter of the bonus
				if(kPromotion->GetFeatureDefensePercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetFeatureDefensePercent(iLoop) / 4);
					iTemp /= 100;
					iPower += iTemp;
				}
			}

			for(iLoop = 0; iLoop < GC.getNumUnitCombatClassInfos(); iLoop++)
			{
				// Unit Combat Class (e.g. Pikemen) - add one quarter of the bonus
				if(kPromotion->GetUnitCombatModifierPercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetUnitCombatModifierPercent(iLoop) / 4);
					iTemp /= 100;
					iPower += iTemp;
				}
			}

			for(iLoop = 0; iLoop < GC.getNumUnitClassInfos(); iLoop++)
			{
				// Unit Class (e.g. bonus ONLY against Galleys) - add one eighth of the bonus
				// We're assuming here that the bonus against the other Unit is at least going to be somewhat useful - trust the XML! :o
				if(kPromotion->GetUnitClassModifierPercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetUnitClassModifierPercent(iLoop) / 8);
					iTemp /= 100;
					iPower += iTemp;
				}
				// Unit Class Attack - one tenth of the bonus
				if(kPromotion->GetUnitClassAttackModifier(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetUnitClassAttackModifier(iLoop) / 10);
					iTemp /= 100;
					iPower += iTemp;
				}
				// Unit Class Defense - one tenth of the bonus
				if(kPromotion->GetUnitClassDefenseModifier(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetUnitClassDefenseModifier(iLoop) / 10);
					iTemp /= 100;
					iPower += iTemp;
				}
			}

			for(iLoop = 0; iLoop < NUM_DOMAIN_TYPES; iLoop++)
			{
				// Domain - add one quarter of the bonus
				if(kPromotion->GetDomainModifierPercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetDomainModifierPercent(iLoop) / 4);
					iTemp /= 100;
					iPower += iTemp;
				}
			}
		}
	}

#if defined(MOD_BUGFIX_UNIT_POWER_CALC)
	}

	// CUSTOMLOG("UnitClass::Power\t%s\t%i", GetDescription(), iPower);
#endif

	// Debug output
	//char temp[256];
	//sprintf(temp, "%s: %i\n", GetDescription(), iPower);
	//OutputDebugString(temp);

	m_iCachedPower = iPower;
}

UnitMoveRate CvUnitEntry::GetMoveRate(int numHexes) const
{
	if(numHexes < 0)
		numHexes = 0;
	if(numHexes > 11)
		numHexes = 11;
	return m_unitMoveRate[numHexes];
}

//=====================================
// CvUnitXMLEntries
//=====================================
/// Constructor
CvUnitXMLEntries::CvUnitXMLEntries(void)
{

}

/// Destructor
CvUnitXMLEntries::~CvUnitXMLEntries(void)
{
	DeleteArray();
}

/// Returns vector of policy entries
std::vector<CvUnitEntry*>& CvUnitXMLEntries::GetUnitEntries()
{
	return m_paUnitEntries;
}

/// Number of defined policies
int CvUnitXMLEntries::GetNumUnits()
{
	return m_paUnitEntries.size();
}


/// Clear policy entries
void CvUnitXMLEntries::DeleteArray()
{
	for(std::vector<CvUnitEntry*>::iterator it = m_paUnitEntries.begin(); it != m_paUnitEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paUnitEntries.clear();
}

/// Get a specific entry
CvUnitEntry* CvUnitXMLEntries::GetEntry(int index)
{
#if defined(MOD_BALANCE_CORE)
	return (index!=NO_UNIT) ? m_paUnitEntries[index] : NULL;
#else
	return m_paUnitEntries[index];
#endif
}

/// Helper function to read in an integer array of data sized according to number of unit types
void UnitArrayHelpers::Read(FDataStream& kStream, int* paiUnitArray)
{
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, paiUnitArray, GC.getNumUnitInfos());
}

/// Helper function to write out an integer array of data sized according to number of unit types
void UnitArrayHelpers::Write(FDataStream& kStream, int* paiUnitArray, int iArraySize)
{
	CvInfosSerializationHelper::WriteHashedDataArray<UnitTypes, int>(kStream, paiUnitArray, iArraySize);
}
