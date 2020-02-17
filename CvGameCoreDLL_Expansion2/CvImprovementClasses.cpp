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
#include "CvImprovementClasses.h"
#include "FireWorks/FRemark.h"
#include "CvInfosSerializationHelper.h"

// must be included after all other headers
#include "LintFree.h"

#ifdef _MSC_VER
#pragma warning ( disable : 4505 ) // unreferenced local function has been removed.. needed by REMARK below
#endif//_MSC_VER
REMARK_GROUP("CvImprovementClasses");


//======================================================================================================
//					CvImprovementResourceInfo
//======================================================================================================
CvImprovementResourceInfo::CvImprovementResourceInfo() :
	m_iDiscoverRand(0),
	m_bResourceMakesValid(false),
	m_bResourceTrade(false),
	m_piYieldChange(NULL)
{
}
//------------------------------------------------------------------------------
CvImprovementResourceInfo::~CvImprovementResourceInfo()
{
	SAFE_DELETE_ARRAY(m_piYieldChange);
}
//------------------------------------------------------------------------------
int CvImprovementResourceInfo::getDiscoverRand() const
{
	return m_iDiscoverRand;
}
//------------------------------------------------------------------------------
bool CvImprovementResourceInfo::isResourceMakesValid() const
{
	return m_bResourceMakesValid;
}
//------------------------------------------------------------------------------
bool CvImprovementResourceInfo::isResourceTrade() const
{
	return m_bResourceTrade;
}
//------------------------------------------------------------------------------
int CvImprovementResourceInfo::getYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChange ? m_piYieldChange[i] : -1;
}
//------------------------------------------------------------------------------

//======================================================================================================
//					CvImprovementEntry
//======================================================================================================
CvImprovementEntry::CvImprovementEntry(void):
	m_iGoldMaintenance(0),
	m_iCultureBombRadius(0),
	m_iRequiresXAdjacentLand(-1),
#if defined(MOD_API_EXTENSIONS)
	m_iRequiresXAdjacentWater(-1),
#endif
#if defined(MOD_GLOBAL_STACKING_RULES)
	m_iAdditionalUnits(0),
#endif
	m_iCultureAdjacentSameType(0),
	m_iTilesPerGoody(0),
	m_iGoodyUniqueRange(0),
	m_iFeatureGrowthProbability(0),
	m_iUpgradeTime(0),
	m_iRiverSideUpgradeMod(0),
	m_iCoastalLandUpgradeMod(0),
	m_iHillsUpgradeMod(0),
	m_iFreshWaterUpgradeMod(0),
	m_iDefenseModifier(0),
	m_iNearbyEnemyDamage(0),
	m_iPillageGold(0),
	m_iResourceExtractionMod(0),
	m_iLuxuryCopiesSiphonedFromMinor(0),
#if defined(MOD_DIPLOMACY_CITYSTATES)
	m_iImprovementLeagueVotes(0),
#endif
#if defined(MOD_BALANCE_CORE)
	m_iHappinessOnConstruction(0),
	m_iImprovementResource(NO_RESOURCE),
	m_iImprovementResourceQuantity(0),
	m_iUnitFreePromotionImprovement(NO_PROMOTION),
	m_iWonderProductionModifier(0),
	m_iUnitPlotExperience(0),
	m_iGAUnitPlotExperience(0),
	m_eCreatesFeature(NO_FEATURE),
	m_bNewOwner(false),
	m_bOwnerOnly(true),
#endif
	m_iImprovementPillage(NO_IMPROVEMENT),
	m_iImprovementUpgrade(NO_IMPROVEMENT),
#if defined(MOD_GLOBAL_RELOCATION)
	m_bAllowsRebaseTo(false),
	m_bAllowsAirliftFrom(false),
	m_bAllowsAirliftTo(false),
#endif
	m_bHillsMakesValid(false),
#if defined(MOD_GLOBAL_ALPINE_PASSES)
	m_bMountainsMakesValid(false),
#endif
#if defined(MOD_GLOBAL_PASSABLE_FORTS)
	m_bMakesPassable(false),
#endif
	m_bWaterAdjacencyMakesValid(false),
	m_bFreshWaterMakesValid(false),
	m_bRiverSideMakesValid(false),
	m_bNoFreshWater(false),
#if defined(MOD_API_EXTENSIONS)
	m_bAddsFreshWater(false),
#endif
	m_bRequiresFlatlands(false),
	m_bRequiresFlatlandsOrFreshWater(false),
	m_bRequiresFeature(false),
	m_bRequiresImprovement(false),
	m_bRemovesResource(false),
	m_bPromptWhenComplete(false),
	m_bWater(false),
	m_bCoastal(false),
	m_bDestroyedWhenPillaged(false),
	m_bDisplacePillager(false),
	m_bBuildableOnResources(false),
	m_bBarbarianCamp(false),
	m_bGoody(false),
	m_bPermanent(false),
	m_bOutsideBorders(false),
	m_bInAdjacentFriendly(false),
	m_bIgnoreOwnership(false),
	m_bOnlyCityStateTerritory(false),
#if defined(MOD_DIPLOMACY_CITYSTATES)
	m_bIsEmbassy(false),
#endif
#if defined(MOD_BALANCE_CORE)
	m_iGetObsoleteTech(NO_TECH),
	m_bAdjacentLake(false),
	m_bAdjacentCity(false),
	m_iGrantsVision(0),
	m_iMovesChange(0),
#endif
	m_bNoTwoAdjacent(false),
	m_bAdjacentLuxury(false),
	m_bAllowsWalkWater(false),
	m_bCreatedByGreatPerson(false),
	m_bSpecificCivRequired(false),
	m_eImprovementUsageType(IMPROVEMENTUSAGE_BASIC),
	m_eRequiredCivilization(NO_CIVILIZATION),
	m_iWorldSoundscapeScriptId(0),
	m_piResourceQuantityRequirements(NULL),
	m_piPrereqNatureYield(NULL),
	m_piYieldChange(NULL),
	m_piYieldPerEra(NULL),
	m_piRiverSideYieldChange(NULL),
	m_piCoastalLandYieldChange(NULL),
	m_piHillsYieldChange(NULL),
	m_piFreshWaterChange(NULL),
	m_piAdjacentCityYieldChange(NULL),
	m_piAdjacentMountainYieldChange(NULL),
	m_piFlavorValue(NULL),
	m_pbTerrainMakesValid(NULL),
	m_pbFeatureMakesValid(NULL),
	m_pbImprovementMakesValid(NULL),
#if defined(MOD_API_UNIFIED_YIELDS)
	m_piAdjacentSameTypeYield(NULL),
	m_piAdjacentTwoSameTypeYield(NULL),
	m_ppiAdjacentImprovementYieldChanges(NULL),
	m_ppiAdjacentTerrainYieldChanges(NULL),
	m_ppiAdjacentResourceYieldChanges(NULL),
	m_ppiAdjacentFeatureYieldChanges(NULL),
	m_ppiFeatureYieldChanges(NULL),
#endif
	m_ppiTechYieldChanges(NULL),
	m_ppiTechNoFreshWaterYieldChanges(NULL),
	m_ppiTechFreshWaterYieldChanges(NULL),
	m_ppiRouteYieldChanges(NULL),
	m_paImprovementResource(NULL)
{
}

/// Destructor
CvImprovementEntry::~CvImprovementEntry(void)
{
	SAFE_DELETE_ARRAY(m_piResourceQuantityRequirements);
	SAFE_DELETE_ARRAY(m_piPrereqNatureYield);
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piYieldPerEra);
	SAFE_DELETE_ARRAY(m_piRiverSideYieldChange);
	SAFE_DELETE_ARRAY(m_piCoastalLandYieldChange);
	SAFE_DELETE_ARRAY(m_piHillsYieldChange);
	SAFE_DELETE_ARRAY(m_piFreshWaterChange);
	SAFE_DELETE_ARRAY(m_piAdjacentCityYieldChange);
	SAFE_DELETE_ARRAY(m_piAdjacentMountainYieldChange);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_pbTerrainMakesValid);
	SAFE_DELETE_ARRAY(m_pbFeatureMakesValid);
	SAFE_DELETE_ARRAY(m_pbImprovementMakesValid);
#if defined(MOD_API_UNIFIED_YIELDS)
	SAFE_DELETE_ARRAY(m_piAdjacentSameTypeYield);
	SAFE_DELETE_ARRAY(m_piAdjacentTwoSameTypeYield);
	if(m_ppiAdjacentImprovementYieldChanges != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray(m_ppiAdjacentImprovementYieldChanges);
	}
	if(m_ppiAdjacentResourceYieldChanges != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray(m_ppiAdjacentResourceYieldChanges);
	}
	if(m_ppiAdjacentTerrainYieldChanges != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray(m_ppiAdjacentTerrainYieldChanges);
	}
	if(m_ppiAdjacentFeatureYieldChanges != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray(m_ppiAdjacentFeatureYieldChanges);
	}
	if (m_ppiFeatureYieldChanges != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray(m_ppiFeatureYieldChanges);
	}
#endif
	if(m_paImprovementResource != NULL)
	{
		SAFE_DELETE_ARRAY(m_paImprovementResource); // XXX make sure this isn't leaking memory...
	}

	if(m_ppiTechYieldChanges != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray(m_ppiTechYieldChanges);
	}

	if(m_ppiTechNoFreshWaterYieldChanges != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray(m_ppiTechNoFreshWaterYieldChanges);
	}

	if(m_ppiTechFreshWaterYieldChanges != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray(m_ppiTechFreshWaterYieldChanges);
	}

	if(m_ppiRouteYieldChanges != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray(m_ppiRouteYieldChanges);
	}
}

/// Read from XML file
bool CvImprovementEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	//Basic properties
	const char* szArtDefineTag = kResults.GetText("ArtDefineTag");
	SetArtDefineTag(szArtDefineTag);

	m_iGoldMaintenance = kResults.GetInt("GoldMaintenance");
	m_iCultureBombRadius = kResults.GetInt("CultureBombRadius");
	m_iRequiresXAdjacentLand = kResults.GetInt("RequiresXAdjacentLand");
#if defined(MOD_API_EXTENSIONS)
	m_iRequiresXAdjacentWater = kResults.GetInt("RequiresXAdjacentWater");
#endif
#if defined(MOD_GLOBAL_STACKING_RULES)
	m_iAdditionalUnits = kResults.GetInt("AdditionalUnits");
#endif
	m_iCultureAdjacentSameType = kResults.GetInt("CultureAdjacentSameType");
#if defined(MOD_GLOBAL_RELOCATION)
	m_bAllowsRebaseTo = kResults.GetBool("AllowsRebaseTo");
	m_bAllowsAirliftFrom = kResults.GetBool("AllowsAirliftFrom");
	m_bAllowsAirliftTo = kResults.GetBool("AllowsAirliftTo");
#endif
	m_bHillsMakesValid = kResults.GetBool("HillsMakesValid");
#if defined(MOD_GLOBAL_ALPINE_PASSES)
	m_bMountainsMakesValid = kResults.GetBool("MountainsMakesValid");
#endif
#if defined(MOD_GLOBAL_PASSABLE_FORTS)
	m_bMakesPassable = kResults.GetBool("MakesPassable");
#endif
	m_bWaterAdjacencyMakesValid = kResults.GetBool("WaterAdjacencyMakesValid");
	m_bFreshWaterMakesValid = kResults.GetBool("FreshWaterMakesValid");
	m_bRiverSideMakesValid = kResults.GetBool("RiverSideMakesValid");
	m_bNoFreshWater = kResults.GetBool("NoFreshWater");
#if defined(MOD_API_EXTENSIONS)
	if (MOD_API_EXTENSIONS) {
		m_bAddsFreshWater = kResults.GetBool("AddsFreshWater");
	}
#endif
	m_bRequiresFlatlands = kResults.GetBool("RequiresFlatlands");
	m_bRequiresFlatlandsOrFreshWater = kResults.GetBool("RequiresFlatlandsOrFreshWater");
	m_bRequiresFeature = kResults.GetBool("RequiresFeature");
	m_bRequiresImprovement = kResults.GetBool("RequiresImprovement");
	m_bRemovesResource = kResults.GetBool("RemovesResource");
	m_bPromptWhenComplete = kResults.GetBool("PromptWhenComplete");
	m_bWater = kResults.GetBool("Water");
	m_bCoastal = kResults.GetBool("Coastal");
	m_bDestroyedWhenPillaged = kResults.GetBool("DestroyedWhenPillaged");
	m_bDisplacePillager = kResults.GetBool("DisplacePillager");
	m_bBuildableOnResources = kResults.GetBool("BuildableOnResources");
	m_eImprovementUsageType = m_bBuildableOnResources ? IMPROVEMENTUSAGE_LANDMARK : IMPROVEMENTUSAGE_BASIC;
	m_bBarbarianCamp = kResults.GetBool("BarbarianCamp");
	m_bGoody = kResults.GetBool("Goody");
	m_bPermanent = kResults.GetBool("Permanent");
	m_iTilesPerGoody = kResults.GetInt("TilesPerGoody");
	m_iGoodyUniqueRange = kResults.GetInt("GoodyRange");
	m_iFeatureGrowthProbability = kResults.GetInt("FeatureGrowth");
	m_iUpgradeTime = kResults.GetInt("UpgradeTime");
	m_iRiverSideUpgradeMod = kResults.GetInt("RiverSideUpgradeMod");
	m_iCoastalLandUpgradeMod = kResults.GetInt("CoastalLandUpgradeMod");
	m_iHillsUpgradeMod = kResults.GetInt("HillsUpgradeMod");
	m_iFreshWaterUpgradeMod = kResults.GetInt("FreshWaterUpgradeMod");
	m_iDefenseModifier = kResults.GetInt("DefenseModifier");
	m_iNearbyEnemyDamage = kResults.GetInt("NearbyEnemyDamage");
	m_iPillageGold = kResults.GetInt("PillageGold");
	m_bOutsideBorders = kResults.GetBool("OutsideBorders");
	m_bInAdjacentFriendly = kResults.GetBool("InAdjacentFriendly");
	m_bIgnoreOwnership = kResults.GetBool("IgnoreOwnership");
	m_bOnlyCityStateTerritory = kResults.GetBool("OnlyCityStateTerritory");
#if defined(MOD_DIPLOMACY_CITYSTATES)
	if (MOD_DIPLOMACY_CITYSTATES) {
	m_bIsEmbassy = kResults.GetBool("IsEmbassy");
	}
#endif
#if defined(MOD_BALANCE_CORE)
	const char* szObsoleteTech = kResults.GetText("ObsoleteTech");
	m_iGetObsoleteTech = (CivilizationTypes)GC.getInfoTypeForString(szObsoleteTech, true);
	m_bAdjacentLake = kResults.GetBool("Lakeside");
	m_bAdjacentCity = kResults.GetBool("Cityside");
	m_iGrantsVision = kResults.GetInt("GrantsVisionXTiles");
	m_iUnitPlotExperience = kResults.GetInt("UnitPlotExperience");
	m_iGAUnitPlotExperience = kResults.GetInt("GAUnitPlotExperience");
	const char* szFeatureType = kResults.GetText("CreatesFeature");
	m_eCreatesFeature = (FeatureTypes)GC.getInfoTypeForString(szFeatureType, true);
	m_bNewOwner = kResults.GetBool("NewOwner");
	m_bOwnerOnly = kResults.GetBool("OwnerOnly");
	m_iMovesChange = kResults.GetInt("MovesChange");
#endif
	m_bNoTwoAdjacent = kResults.GetBool("NoTwoAdjacent");
	m_bAdjacentLuxury = kResults.GetBool("AdjacentLuxury");
	m_bAllowsWalkWater = kResults.GetBool("AllowsWalkWater");
	m_bCreatedByGreatPerson = kResults.GetBool("CreatedByGreatPerson");
	m_bSpecificCivRequired = kResults.GetBool("SpecificCivRequired");
	m_iResourceExtractionMod = kResults.GetInt("ResourceExtractionMod");
	m_iLuxuryCopiesSiphonedFromMinor = kResults.GetInt("LuxuryCopiesSiphonedFromMinor");
#if defined(MOD_DIPLOMACY_CITYSTATES)
	if (MOD_DIPLOMACY_CITYSTATES) {
		m_iImprovementLeagueVotes = kResults.GetInt("ImprovementLeagueVotes");
	}
#endif

	const char* szCivilizationType = kResults.GetText("CivilizationType");
	m_eRequiredCivilization = (CivilizationTypes)GC.getInfoTypeForString(szCivilizationType, true);
#if defined(MOD_BALANCE_CORE)
	m_iHappinessOnConstruction = kResults.GetInt("HappinessOnConstruction");
	const char* szImprovementResource = kResults.GetText("ImprovementResource");
	m_iImprovementResource = (ResourceTypes)GC.getInfoTypeForString(szImprovementResource, true);
	m_iImprovementResourceQuantity = kResults.GetInt("ImprovementResourceQuantity");
	const char* szTextVal;
	szTextVal = kResults.GetText("UnitFreePromotion");
	if(szTextVal != NULL)
	{
		m_iUnitFreePromotionImprovement = GC.getInfoTypeForString(szTextVal, true);
	}
	m_iWonderProductionModifier = kResults.GetInt("WonderProductionModifier");
#endif
	//References
	const char* szWorldsoundscapeAudioScript = kResults.GetText("WorldSoundscapeAudioScript");
	if(szWorldsoundscapeAudioScript != NULL)
	{
		m_iWorldSoundscapeScriptId = gDLL->GetAudioTagIndex(szWorldsoundscapeAudioScript, AUDIOTAG_SOUNDSCAPE);
	}
	else
	{
		m_iWorldSoundscapeScriptId = -1;
		Remark(1, "Warning: Missing soundscape definition in XML for feature: '%s'", GetType());
	}

	const char* szImprovementPillage = kResults.GetText("ImprovementPillage");
	m_iImprovementPillage = GC.getInfoTypeForString(szImprovementPillage, true);

	const char* szImprovementUpgrade = kResults.GetText("ImprovementUpgrade");
	m_iImprovementUpgrade = GC.getInfoTypeForString(szImprovementUpgrade, true);

	//Arrays
	const char* szImprovementType = GetType();
	const size_t lenImprovementType = strlen(szImprovementType);

	kUtility.PopulateArrayByExistence(m_pbTerrainMakesValid,
	                                  "Terrains",
	                                  "Improvement_ValidTerrains",
	                                  "TerrainType",
	                                  "ImprovementType",
	                                  szImprovementType);

	kUtility.PopulateArrayByExistence(m_pbFeatureMakesValid,
	                                  "Features",
	                                  "Improvement_ValidFeatures",
	                                  "FeatureType",
	                                  "ImprovementType",
	                                  szImprovementType);

	kUtility.PopulateArrayByExistence(m_pbImprovementMakesValid,
									  "Improvements",
									  "Improvement_ValidImprovements",
									  "PrereqImprovement",
									  "ImprovementType",
							          szImprovementType);

#if defined(MOD_API_UNIFIED_YIELDS)
	kUtility.SetYields(m_piAdjacentSameTypeYield, "Improvement_YieldAdjacentSameType", "ImprovementType", szImprovementType);
	kUtility.SetYields(m_piAdjacentTwoSameTypeYield, "Improvement_YieldAdjacentTwoSameType", "ImprovementType", szImprovementType);
#endif

	kUtility.SetYields(m_piYieldChange, "Improvement_Yields", "ImprovementType", szImprovementType);
	kUtility.SetYields(m_piYieldPerEra, "Improvement_YieldPerEra", "ImprovementType", szImprovementType);
	kUtility.SetYields(m_piAdjacentCityYieldChange, "Improvement_AdjacentCityYields", "ImprovementType", szImprovementType);
	kUtility.SetYields(m_piAdjacentMountainYieldChange, "Improvement_AdjacentMountainYieldChanges", "ImprovementType", szImprovementType);
	kUtility.SetYields(m_piCoastalLandYieldChange, "Improvement_CoastalLandYields", "ImprovementType", szImprovementType);
	kUtility.SetYields(m_piFreshWaterChange, "Improvement_FreshWaterYields", "ImprovementType", szImprovementType);
	kUtility.SetYields(m_piHillsYieldChange, "Improvement_HillsYields", "ImprovementType", szImprovementType);
	kUtility.SetYields(m_piRiverSideYieldChange, "Improvement_RiverSideYields", "ImprovementType", szImprovementType);
	kUtility.SetYields(m_piPrereqNatureYield, "Improvement_PrereqNatureYields", "ImprovementType", szImprovementType);

	kUtility.SetFlavors(m_piFlavorValue, "Improvement_Flavors", "ImprovementType", szImprovementType);

	{
		//Initialize Improvement Resource Types to number of Resources
		const int iNumResources = kUtility.MaxRows("Resources");
		m_paImprovementResource = FNEW(CvImprovementResourceInfo[iNumResources], c_eCiv5GameplayDLL, 0);

		kUtility.InitializeArray(m_piResourceQuantityRequirements, iNumResources);


		std::string strResourceTypesKey = "Improvements - ResourceTypes";
		Database::Results* pResourceTypes = kUtility.GetResults(strResourceTypesKey);
		if(pResourceTypes == NULL)
		{
			pResourceTypes = kUtility.PrepareResults(strResourceTypesKey, "select Resources.ID, ResourceType, ResourceMakesValid, ResourceTrade, DiscoveryRand from Improvement_ResourceTypes inner join Resources on ResourceType = Resources.Type where ImprovementType = ?");
		}

		std::string strYieldResultsKey = "Improvements - YieldResults";
		Database::Results* pYieldResults = kUtility.GetResults(strYieldResultsKey);
		if(pYieldResults == NULL)
		{
			pYieldResults = kUtility.PrepareResults(strYieldResultsKey, "select Yields.ID, Yield from Improvement_ResourceType_Yields inner join Yields on YieldType = Yields.Type where ImprovementType = ? and ResourceType = ?");
		}

		pResourceTypes->Bind(1, szImprovementType, lenImprovementType, false);

		while(pResourceTypes->Step())
		{
			const int idx = pResourceTypes->GetInt("ID");
			CvImprovementResourceInfo& pResourceInfo = m_paImprovementResource[idx];

			const char* szResourceType = pResourceTypes->GetText("ResourceType");
			pResourceInfo.m_bResourceMakesValid = pResourceTypes->GetBool("ResourceMakesValid");
			pResourceInfo.m_bResourceTrade = pResourceTypes->GetBool("ResourceTrade");
			pResourceInfo.m_iDiscoverRand = pResourceTypes->GetInt("DiscoveryRand");
			m_piResourceQuantityRequirements[idx] = pResourceTypes->GetInt("QuantityRequirement");

			//Populate Yields for structure
			kUtility.InitializeArray(pResourceInfo.m_piYieldChange, "Yields");

			pYieldResults->Bind(1, szImprovementType, lenImprovementType, false);
			pYieldResults->Bind(2, szResourceType, -1, false);
			while(pYieldResults->Step())
			{
				const int yieldIdx = pYieldResults->GetInt("ID");
				const int yield = pYieldResults->GetInt("Yield");
				pResourceInfo.m_piYieldChange[yieldIdx] = yield;
			}

			pYieldResults->Reset();
		}

		pResourceTypes->Reset();
	}

	const int iNumYields = kUtility.MaxRows("Yields");
#if defined(MOD_BALANCE_CORE)
	//AdjacentImprovementYieldChanges
	{
		const int iNumImprovements = kUtility.MaxRows("Improvements");
		CvAssertMsg(iNumImprovements > 0, "Num Improvement Infos <= 0");
		kUtility.Initialize2DArray(m_ppiAdjacentImprovementYieldChanges, iNumImprovements, iNumYields);

		std::string strKey = "Improvements - AdjacentImprovementYieldChanges";
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Yields.ID as YieldID, Improvements.ID as ImprovementID, Yield from Improvement_AdjacentImprovementYieldChanges inner join Yields on YieldType = Yields.Type inner join Improvements on OtherImprovementType = Improvements.Type where ImprovementType = ?");
		}

		pResults->Bind(1, szImprovementType, lenImprovementType, false);

		while(pResults->Step())
		{
			const int yield_idx = pResults->GetInt(0);
			CvAssert(yield_idx > -1);

			const int improvement_idx = pResults->GetInt(1);
			CvAssert(improvement_idx > -1);

			const int yield = pResults->GetInt(2);

			m_ppiAdjacentImprovementYieldChanges[improvement_idx][yield_idx] = yield;
		}

		pResults->Reset();
	}
	//m_ppiAdjacentResourceYieldChanges
	{
		const int iNumResources = kUtility.MaxRows("Resources");
		CvAssertMsg(iNumResources > 0, "Num Resource Infos <= 0");
		kUtility.Initialize2DArray(m_ppiAdjacentResourceYieldChanges, iNumResources, iNumYields);

		std::string strKey = "Improvements - AdjacentResourceYieldChanges";
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Yields.ID as YieldID, Resources.ID as ResourceID, Yield from Improvement_AdjacentResourceYieldChanges inner join Yields on YieldType = Yields.Type inner join Resources on ResourceType = Resources.Type where ImprovementType = ?");
		}

		pResults->Bind(1, szImprovementType, lenImprovementType, false);

		while(pResults->Step())
		{
			const int yield_idx = pResults->GetInt(0);
			CvAssert(yield_idx > -1);

			const int resource_idx = pResults->GetInt(1);
			CvAssert(resource_idx > -1);

			const int yield = pResults->GetInt(2);

			m_ppiAdjacentResourceYieldChanges[resource_idx][yield_idx] = yield;
		}

		pResults->Reset();
	}
	//m_ppiAdjacentTerrainYieldChanges
	{
		const int iNumTerrains = kUtility.MaxRows("Terrains");
		CvAssertMsg(iNumTerrains > 0, "Num Terrain Infos <= 0");
		kUtility.Initialize2DArray(m_ppiAdjacentTerrainYieldChanges, iNumTerrains, iNumYields);

		std::string strKey = "Terrains - AdjacentTerrainYieldChanges";
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Yields.ID as YieldID, Terrains.ID as TerrainID, Yield from Improvement_AdjacentTerrainYieldChanges inner join Yields on YieldType = Yields.Type inner join Terrains on TerrainType = Terrains.Type where ImprovementType = ?");
		}

		pResults->Bind(1, szImprovementType, lenImprovementType, false);

		while(pResults->Step())
		{
			const int yield_idx = pResults->GetInt(0);
			CvAssert(yield_idx > -1);

			const int terrain_idx = pResults->GetInt(1);
			CvAssert(terrain_idx > -1);

			const int yield = pResults->GetInt(2);

			m_ppiAdjacentTerrainYieldChanges[terrain_idx][yield_idx] = yield;
		}

		pResults->Reset();
	}

	//m_ppiAdjacentFeatureYieldChanges
	{
		const int iNumFeatures = kUtility.MaxRows("Features");
		CvAssertMsg(iNumFeatures > 0, "Num Feature Infos <= 0");
		kUtility.Initialize2DArray(m_ppiAdjacentFeatureYieldChanges, iNumFeatures, iNumYields);

		std::string strKey = "Features - FeatureYieldChanges";
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Yields.ID as YieldID, Features.ID as FeatureID, Yield from Improvement_AdjacentFeatureYieldChanges inner join Yields on YieldType = Yields.Type inner join Features on FeatureType = Features.Type where ImprovementType = ?");
		}

		pResults->Bind(1, szImprovementType, lenImprovementType, false);

		while (pResults->Step())
		{
			const int yield_idx = pResults->GetInt(0);
			CvAssert(yield_idx > -1);

			const int feature_idx = pResults->GetInt(1);
			CvAssert(feature_idx > -1);

			const int yield = pResults->GetInt(2);

			m_ppiAdjacentFeatureYieldChanges[feature_idx][yield_idx] = yield;
		}

		pResults->Reset();
	}

	//m_ppiFeatureYieldChanges
	{
		const int iNumFeatures = kUtility.MaxRows("Features");
		CvAssertMsg(iNumFeatures > 0, "Num Feature Infos <= 0");
		kUtility.Initialize2DArray(m_ppiFeatureYieldChanges, iNumFeatures, iNumYields);

		std::string strKey = "Features - FeatureYieldChanges";
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Yields.ID as YieldID, Features.ID as FeatureID, Yield from Improvement_FeatureYieldChanges inner join Yields on YieldType = Yields.Type inner join Features on FeatureType = Features.Type where ImprovementType = ?");
		}

		pResults->Bind(1, szImprovementType, lenImprovementType, false);

		while (pResults->Step())
		{
			const int yield_idx = pResults->GetInt(0);
			CvAssert(yield_idx > -1);

			const int feature_idx = pResults->GetInt(1);
			CvAssert(feature_idx > -1);

			const int yield = pResults->GetInt(2);

			m_ppiFeatureYieldChanges[feature_idx][yield_idx] = yield;
		}

		pResults->Reset();
	}
	
#endif
	const int iNumTechs = GC.getNumTechInfos();
	CvAssertMsg(iNumTechs > 0, "Num Tech Infos <= 0");

	//TechYieldChanges
	{
		kUtility.Initialize2DArray(m_ppiTechYieldChanges, iNumTechs, iNumYields);

		std::string strKey = "Improvements - TechYieldChanges";
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Yields.ID as YieldID, Technologies.ID as TechID, Yield from Improvement_TechYieldChanges inner join Yields on YieldType = Yields.Type inner join Technologies on TechType = Technologies.Type where ImprovementType = ?");
		}

		pResults->Bind(1, szImprovementType, lenImprovementType, false);

		while(pResults->Step())
		{
			const int yield_idx = pResults->GetInt(0);
			CvAssert(yield_idx > -1);

			const int tech_idx = pResults->GetInt(1);
			CvAssert(tech_idx > -1);

			const int yield = pResults->GetInt(2);

			m_ppiTechYieldChanges[tech_idx][yield_idx] = yield;
		}
	}

	//TechNoFreshWaterYieldChanges
	{
		kUtility.Initialize2DArray(m_ppiTechNoFreshWaterYieldChanges, iNumTechs, iNumYields);

		std::string strKey = "Improvements - TechNoFreshWaterYieldChanges";
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Yields.ID as YieldID, Technologies.ID as TechID, Yield from Improvement_TechNoFreshWaterYieldChanges inner join Yields on YieldType = Yields.Type inner join Technologies on TechType = Technologies.Type where ImprovementType = ?");
		}

		pResults->Bind(1, szImprovementType, lenImprovementType, false);

		while(pResults->Step())
		{
			const int yield_idx = pResults->GetInt(0);
			CvAssert(yield_idx > -1);

			const int tech_idx = pResults->GetInt(1);
			CvAssert(tech_idx > -1);

			const int yield = pResults->GetInt(2);

			m_ppiTechNoFreshWaterYieldChanges[tech_idx][yield_idx] = yield;
		}

		pResults->Reset();

	}

	//TechFreshWaterYieldChanges
	{
		kUtility.Initialize2DArray(m_ppiTechFreshWaterYieldChanges, iNumTechs, iNumYields);

		std::string strKey = "Improvements - TechFreshWaterYieldChanges";
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Yields.ID as YieldID, Technologies.ID as TechID, Yield from Improvement_TechFreshWaterYieldChanges inner join Yields on YieldType = Yields.Type inner join Technologies on TechType = Technologies.Type where ImprovementType = ?");
		}

		pResults->Bind(1, szImprovementType, lenImprovementType, false);

		while(pResults->Step())
		{
			const int yield_idx = pResults->GetInt(0);
			CvAssert(yield_idx > -1);

			const int tech_idx = pResults->GetInt(1);
			CvAssert(tech_idx > -1);

			const int yield = pResults->GetInt(2);

			m_ppiTechFreshWaterYieldChanges[tech_idx][yield_idx] = yield;
		}

		pResults->Reset();

	}

	//RouteYieldChanges
	{
		const int iNumRoutes = kUtility.MaxRows("Routes");
		kUtility.Initialize2DArray(m_ppiRouteYieldChanges, iNumRoutes, iNumYields);

		std::string strKey = "Improvements - RouteYieldChanges";
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Yields.ID as YieldID, Routes.ID as RouteID, Yield from Improvement_RouteYieldChanges inner join Yields on YieldType = Yields.Type inner join Routes on RouteType = Routes.Type where ImprovementType = ?;");
		}

		pResults->Bind(1, szImprovementType, lenImprovementType, false);

		while(pResults->Step())
		{
			const int yield_idx = pResults->GetInt(0);
			CvAssert(yield_idx > -1);

			const int route_idx = pResults->GetInt(1);
			CvAssert(route_idx > -1);

			const int yield = pResults->GetInt(2);

			m_ppiRouteYieldChanges[route_idx][yield_idx] = yield;
		}

		pResults->Reset();

	}

	return true;
}

/// The gold maintenance cost
int CvImprovementEntry::GetGoldMaintenance() const
{
	return m_iGoldMaintenance;
}

/// Convert nearby tiles?
int CvImprovementEntry::GetCultureBombRadius() const
{
	return m_iCultureBombRadius;
}

/// How many adjacent tiles must be land?
int CvImprovementEntry::GetRequiresXAdjacentLand() const
{
	return m_iRequiresXAdjacentLand;
}

#if defined(MOD_API_EXTENSIONS)
/// How many adjacent tiles must be water?
int CvImprovementEntry::GetRequiresXAdjacentWater() const
{
	return m_iRequiresXAdjacentWater;
}
#endif

#if defined(MOD_GLOBAL_STACKING_RULES)
/// Additional units that can stack in this improvement
int CvImprovementEntry::GetAdditionalUnits() const
{
	return m_iAdditionalUnits;
}
#endif

#if defined(MOD_API_UNIFIED_YIELDS)
/// Bonus yield if another Improvement of same type is adjacent
int CvImprovementEntry::GetYieldAdjacentSameType(YieldTypes eYield) const
{
	int iYield = GetAdjacentSameTypeYield(eYield);
	
	// Special case for culture
	if (eYield == YIELD_CULTURE) {
		iYield += m_iCultureAdjacentSameType;
	}
	
	return iYield;
}
/// Bonus yield if another Improvement of same type is adjacent
int CvImprovementEntry::GetYieldAdjacentTwoSameType(YieldTypes eYield) const
{
	int iYield = GetAdjacentTwoSameTypeYield(eYield);

	return iYield;
}
#else
/// Bonus culture if another Improvement of same type is adjacent
int CvImprovementEntry::GetCultureAdjacentSameType() const
{
	return m_iCultureAdjacentSameType;
}
#endif

/// The number of tiles in an area needed for a goody hut to be placed by the map generator
int CvImprovementEntry::GetTilesPerGoody() const
{
	return m_iTilesPerGoody;
}

/// How far goody huts need to be away from each other
int CvImprovementEntry::GetGoodyUniqueRange() const
{
	return m_iGoodyUniqueRange;
}

/// How likely this improvement is to expand into an adjacent tile
int CvImprovementEntry::GetFeatureGrowthProbability() const
{
	return m_iFeatureGrowthProbability;
}

/// Amount of time needed before an improvement upgrades to its next state
int CvImprovementEntry::GetUpgradeTime() const
{
	return m_iUpgradeTime;
}

/// Modifier for how much improvements upgrade if they are adjacent to a river
int CvImprovementEntry::GetRiverSideUpgradeMod() const
{
	return m_iRiverSideUpgradeMod;
}

/// Modifier for how much improvements upgrade if they are on the coast
int CvImprovementEntry::GetCoastalLandUpgradeMod() const
{
	return m_iCoastalLandUpgradeMod;
}

/// Modifier for how much improvements upgrade if they are on a hill
int CvImprovementEntry::GetHillsUpgradeMod() const
{
	return m_iHillsUpgradeMod;
}

/// Modifier for how much improvements upgrade if they are exposed to fresh water
int CvImprovementEntry::GetFreshWaterUpgradeMod() const
{
	return m_iFreshWaterUpgradeMod;
}

/// Modifier for the defensive improvement this improvement provides
int CvImprovementEntry::GetDefenseModifier() const
{
	return m_iDefenseModifier;
}

/// Damage done to nearby enemy units
int CvImprovementEntry::GetNearbyEnemyDamage() const
{
	return m_iNearbyEnemyDamage;
}

/// How much gold this improvement provides the pillager
int CvImprovementEntry::GetPillageGold() const
{
	return m_iPillageGold;
}

/// Modifier on the amount of resources generated from the tile with this improvement. (100 doubles output)
int CvImprovementEntry::GetResourceExtractionMod() const
{
	return m_iResourceExtractionMod;
}

/// Do we get any copies of the luxury types that the plot's owner has?
int CvImprovementEntry::GetLuxuryCopiesSiphonedFromMinor() const
{
	return m_iLuxuryCopiesSiphonedFromMinor;
}

#if defined(MOD_DIPLOMACY_CITYSTATES)
/// Does this improvement grant an extra World Congress vote?
int CvImprovementEntry::GetCityStateExtraVote() const
{
	return m_iImprovementLeagueVotes;
}
#endif
int CvImprovementEntry::GetHappinessOnConstruction() const
{
	return m_iHappinessOnConstruction;
}
#if defined(MOD_BALANCE_CORE)
// Does this improvement create a resource when construced?
int CvImprovementEntry::GetResourceFromImprovement() const
{
	return m_iImprovementResource;
}
// Does this improvement create a resource when construced?
int CvImprovementEntry::GetResourceQuantityFromImprovement() const
{
	return m_iImprovementResourceQuantity;
}
int CvImprovementEntry::GetUnitFreePromotion() const
{
	return m_iUnitFreePromotionImprovement;
}
int CvImprovementEntry::GetWonderProductionModifier() const
{
	return m_iWonderProductionModifier;
}
int CvImprovementEntry::GetUnitPlotExperience() const
{
	return m_iUnitPlotExperience;
}
int CvImprovementEntry::GetMovesChange() const
{
	return m_iMovesChange;
}
int CvImprovementEntry::GetGAUnitPlotExperience() const
{
	return m_iGAUnitPlotExperience;
}
FeatureTypes CvImprovementEntry::GetCreatedFeature() const
{
	return m_eCreatesFeature;
}
bool CvImprovementEntry::IsNewOwner() const
{
	return m_bNewOwner;
}
bool CvImprovementEntry::IsOwnerOnly() const
{
	return m_bOwnerOnly;
}
#endif
/// Returns the type of improvement that results from this improvement being pillaged
int CvImprovementEntry::GetImprovementPillage() const
{
	return m_iImprovementPillage;
}

/// Set the type of improvement that results from this improvement being pillaged
void CvImprovementEntry::SetImprovementPillage(int i)
{
	m_iImprovementPillage = i;
}

/// Returns the type of improvement that results from this improvement growing
int CvImprovementEntry::GetImprovementUpgrade() const
{
	return m_iImprovementUpgrade;
}

/// Set the type of improvement that results from this improvement growing
void CvImprovementEntry::SetImprovementUpgrade(int i)
{
	m_iImprovementUpgrade = i;
}

#if defined(MOD_GLOBAL_RELOCATION)
bool CvImprovementEntry::IsAllowsRebaseTo() const
{
	return m_bAllowsRebaseTo;
}

bool CvImprovementEntry::IsAllowsAirliftFrom() const
{
	return m_bAllowsAirliftFrom;
}

bool CvImprovementEntry::IsAllowsAirliftTo() const
{
	return m_bAllowsAirliftTo;
}
#endif

/// Requires hills to be constructed
bool CvImprovementEntry::IsHillsMakesValid() const
{
	return m_bHillsMakesValid;
}

#if defined(MOD_GLOBAL_ALPINE_PASSES)
/// Requires mountains to be constructed
bool CvImprovementEntry::IsMountainsMakesValid() const
{
	return m_bMountainsMakesValid;
}
#endif

#if defined(MOD_GLOBAL_PASSABLE_FORTS)
/// Permits the tile to be passed by ships
bool CvImprovementEntry::IsMakesPassable() const
{
	return m_bMakesPassable;
}
#endif

// Requires any body of water to build
bool CvImprovementEntry::IsWaterAdjacencyMakesValid() const
{
	return m_bWaterAdjacencyMakesValid;
}

/// Requires fresh water to build
bool CvImprovementEntry::IsFreshWaterMakesValid() const
{
	return m_bFreshWaterMakesValid;
}

/// Requires being adjacent to a river to build
bool CvImprovementEntry::IsRiverSideMakesValid() const
{
	return m_bRiverSideMakesValid;
}

/// Can't be built next on a tile with fresh water
bool CvImprovementEntry::IsNoFreshWater() const
{
	return m_bNoFreshWater;
}

#if defined(MOD_API_EXTENSIONS)
/// Adds fresh water to the plot
bool CvImprovementEntry::IsAddsFreshWater() const
{
	return m_bAddsFreshWater;
}
#endif

/// Requires that it must be built on something other than a hill
bool CvImprovementEntry::IsRequiresFlatlands() const
{
	return m_bRequiresFlatlands;
}

/// Requires that it must be built on something other than a hill or next to fresh water
bool CvImprovementEntry::IsRequiresFlatlandsOrFreshWater() const
{
	return m_bRequiresFlatlandsOrFreshWater;
}

/// Is this only built on top of a feature?
bool CvImprovementEntry::IsRequiresFeature() const
{
	return m_bRequiresFeature;
}

/// Is this only built on top of an improvement?
bool CvImprovementEntry::IsRequiresImprovement() const
{
	return m_bRequiresImprovement;
}

/// Does this remove the resource underneath when built?
bool CvImprovementEntry::IsRemovesResource() const
{
	return m_bRemovesResource;
}

/// Do we prompt for a user choice after this is constructed?
bool CvImprovementEntry::IsPromptWhenComplete() const
{
	return m_bPromptWhenComplete;
}

/// Is this only placed out in the water?
bool CvImprovementEntry::IsWater() const
{
	return m_bWater;
}

/// Is this only placed on the coast?
bool CvImprovementEntry::IsCoastal() const
{
	return m_bCoastal;
}


/// Is this a destroyed rather than pillaged?
bool CvImprovementEntry::IsDestroyedWhenPillaged() const
{
	return m_bDestroyedWhenPillaged;
}

/// Is the unit that pillaged this perhaps in an illegal spot?
bool CvImprovementEntry::IsDisplacePillager() const
{
	return m_bDisplacePillager;
}

/// Can this be built on top of Resources?
bool CvImprovementEntry::IsBuildableOnResources() const
{
	return m_bBuildableOnResources;
}

/// Is this a barbarian camp?
bool CvImprovementEntry::IsBarbarianCamp() const
{
	return m_bBarbarianCamp;
}

/// Is this a goody hut?
bool CvImprovementEntry::IsGoody() const
{
	return m_bGoody;
}

/// Is this permanent?
bool CvImprovementEntry::IsPermanent() const
{
	return m_bPermanent;
}

/// Does this improvement need to be built outside of a civ's borders?
bool CvImprovementEntry::IsOutsideBorders() const
{
	return m_bOutsideBorders;
}

/// Can this improvement be built in anyone's lands?
bool CvImprovementEntry::IsIgnoreOwnership() const
{
	return m_bIgnoreOwnership;
}

/// Can this improvement only be built in City-State lands?
bool CvImprovementEntry::IsOnlyCityStateTerritory() const
{
	return m_bOnlyCityStateTerritory;
}
#if defined(MOD_DIPLOMACY_CITYSTATES)
/// Can only of this improvement be built in City-State lands?
bool CvImprovementEntry::IsEmbassy() const
{
	return m_bIsEmbassy;
}
#endif
#if defined(MOD_BALANCE_CORE)
int CvImprovementEntry::GetObsoleteTech() const
{
	return m_iGetObsoleteTech;
}
bool CvImprovementEntry::IsAdjacentLake() const
{
	return m_bAdjacentLake;
}
bool CvImprovementEntry::IsAdjacentCity() const
{
	return m_bAdjacentCity;
}
int CvImprovementEntry::GetGrantsVision() const
{
	return m_iGrantsVision;
}
#endif
/// Can this improvement not be built adjacent to another one of the same type?
bool CvImprovementEntry::IsNoTwoAdjacent() const
{
	return m_bNoTwoAdjacent;
}

/// Does this improvement need to be built next to a luxury resource?
bool CvImprovementEntry::IsAdjacentLuxury() const
{
	return m_bAdjacentLuxury;
}

/// Does this improvement allows land units to cross water?
bool CvImprovementEntry::IsAllowsWalkWater() const
{
	return m_bAllowsWalkWater;
}

/// Does this improvement need to be built inside or adjacent to a civ's borders?
bool CvImprovementEntry::IsInAdjacentFriendly() const
{
	return m_bInAdjacentFriendly;
}

bool CvImprovementEntry::IsCreatedByGreatPerson() const
{
	return m_bCreatedByGreatPerson;
}

bool CvImprovementEntry::IsSpecificCivRequired() const
{
	return m_bSpecificCivRequired;
}

CivilizationTypes CvImprovementEntry::GetRequiredCivilization() const
{
	return m_eRequiredCivilization;
}

/// DEPRECATED
const char* CvImprovementEntry::GetArtDefineTag() const
{
	return m_strArtDefineTag;
}

/// DEPRECATED
void CvImprovementEntry::SetArtDefineTag(const char* szVal)
{
	m_strArtDefineTag = szVal;
}

ImprovementUsageTypes CvImprovementEntry::GetImprovementUsage() const
{
	return m_eImprovementUsageType;
}

void CvImprovementEntry::SetImprovementUsage(const ImprovementUsageTypes usageType)
{
	m_eImprovementUsageType = usageType;
}

/// DEPRECATED
int CvImprovementEntry::GetWorldSoundscapeScriptId() const
{
	return m_iWorldSoundscapeScriptId;
}

/// What resource is required to build this improvement?
int CvImprovementEntry::GetResourceQuantityRequirement(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piResourceQuantityRequirements ? m_piResourceQuantityRequirements[i] : -1;
}

/// How much of a resource yield is required before this improvement can be built
int CvImprovementEntry::GetPrereqNatureYield(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piPrereqNatureYield ? m_piPrereqNatureYield[i] : -1;
}

int* CvImprovementEntry::GetPrereqNatureYieldArray()
{
	return m_piPrereqNatureYield;
}

/// How much this improvement improves a certain yield
int CvImprovementEntry::GetYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}

int* CvImprovementEntry::GetYieldChangeArray()
{
	return m_piYieldChange;
}

/// How much this improvement improves a certain yield for each era of age
int CvImprovementEntry::GetYieldChangePerEra(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldPerEra ? m_piYieldPerEra[i] : 0;
}

/// How much being next to a river improves the yield of this improvement
int CvImprovementEntry::GetRiverSideYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piRiverSideYieldChange ? m_piRiverSideYieldChange[i] : 0;
}

int* CvImprovementEntry::GetRiverSideYieldChangeArray()
{
	return m_piRiverSideYieldChange;
}

/// How much being on a coastal tile improves the yield of this improvement
int CvImprovementEntry::GetCoastalLandYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piCoastalLandYieldChange ? m_piCoastalLandYieldChange[i] : 0;
}

int* CvImprovementEntry::GetCoastalLandYieldChangeArray()
{
	return m_piCoastalLandYieldChange;
}

/// How much being on a hill tile improves the yield of this improvement
int CvImprovementEntry::GetHillsYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piHillsYieldChange ? m_piHillsYieldChange[i] : 0;
}

int* CvImprovementEntry::GetHillsYieldChangeArray()
{
	return m_piHillsYieldChange;
}

/// How much having access to fresh water improves the yield of this improvement
int CvImprovementEntry::GetFreshWaterYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piFreshWaterChange ? m_piFreshWaterChange[i] : 0;
}

int* CvImprovementEntry::GetFreshWaterYieldChangeArray() // For Moose - CvWidgetData XXX
{
	return m_piFreshWaterChange;
}

/// How much being adjacent to a city improves the yield of this improvement
int CvImprovementEntry::GetAdjacentCityYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piAdjacentCityYieldChange ? m_piAdjacentCityYieldChange[i] : 0;
}

int* CvImprovementEntry::GetAdjacentCityYieldChangeArray()
{
	return m_piAdjacentCityYieldChange;
}

/// How much being adjacent to a mountain improves the yield of this improvement
int CvImprovementEntry::GetAdjacentMountainYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piAdjacentMountainYieldChange ? m_piAdjacentMountainYieldChange[i] : 0;
}

int* CvImprovementEntry::GetAdjacentMountainYieldChangeArray()
{
	return m_piAdjacentMountainYieldChange;
}

/// If this improvement requires a terrain type to be valid
bool CvImprovementEntry::GetTerrainMakesValid(int i) const
{
	CvAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbTerrainMakesValid ? m_pbTerrainMakesValid[i] : false;
}

/// If this improvement requires a feature to be valid
bool CvImprovementEntry::GetFeatureMakesValid(int i) const
{
	CvAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbFeatureMakesValid ? m_pbFeatureMakesValid[i] : false;
}

/// If this improvement requires a different improvement to be valid
bool CvImprovementEntry::GetImprovementMakesValid(int i) const
{
	CvAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbImprovementMakesValid ? m_pbImprovementMakesValid[i] : false;
}

#if defined(MOD_API_UNIFIED_YIELDS)
/// If this improvement requires a terrain type to be valid
int CvImprovementEntry::GetAdjacentSameTypeYield(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piAdjacentSameTypeYield ? m_piAdjacentSameTypeYield[i] : 0;
}
int* CvImprovementEntry::GetAdjacentSameTypeYieldArray()
{
	return m_piAdjacentSameTypeYield;
}
/// If this improvement requires a terrain type to be valid
int CvImprovementEntry::GetAdjacentTwoSameTypeYield(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piAdjacentTwoSameTypeYield ? m_piAdjacentTwoSameTypeYield[i] : 0;
}
int* CvImprovementEntry::GetAdjacentTwoSameTypeYieldArray()
{
	return m_piAdjacentTwoSameTypeYield;
}
/// How much a tech improves the yield of this improvement if it has fresh water
int CvImprovementEntry::GetAdjacentImprovementYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiAdjacentImprovementYieldChanges[i][j];
}

int* CvImprovementEntry::GetAdjacentImprovementYieldChangesArray(int i)
{
	return m_ppiAdjacentImprovementYieldChanges[i];
}

/// How much an improvement yields if built next to a resource
int CvImprovementEntry::GetAdjacentResourceYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiAdjacentResourceYieldChanges[i][j];
}

int* CvImprovementEntry::GetAdjacentResourceYieldChangesArray(int i)
{
	return m_ppiAdjacentResourceYieldChanges[i];
}

int CvImprovementEntry::GetAdjacentTerrainYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiAdjacentTerrainYieldChanges[i][j];
}

int* CvImprovementEntry::GetAdjacentTerrainYieldChangesArray(int i)
{
	return m_ppiAdjacentTerrainYieldChanges[i];
}

int CvImprovementEntry::GetAdjacentFeatureYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumPlotInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiAdjacentFeatureYieldChanges[i][j];
}

int* CvImprovementEntry::GetAdjacentFeatureYieldChangesArray(int i)
{
	return m_ppiAdjacentFeatureYieldChanges[i];
}

int CvImprovementEntry::GetFeatureYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiFeatureYieldChanges[i][j];
}
int* CvImprovementEntry::GetFeatureYieldChangesArray(int i)
{
	return m_ppiFeatureYieldChanges[i];
}
#endif

/// How much a tech improves the yield of this improvement
int CvImprovementEntry::GetTechYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumTechInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiTechYieldChanges[i][j];
}

int* CvImprovementEntry::GetTechYieldChangesArray(int i)
{
	return m_ppiTechYieldChanges[i];
}

/// How much a tech improves the yield of this improvement if it DOES NOT have fresh water
int CvImprovementEntry::GetTechNoFreshWaterYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumTechInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiTechNoFreshWaterYieldChanges[i][j];
}

int* CvImprovementEntry::GetTechNoFreshWaterYieldChangesArray(int i)
{
	return m_ppiTechNoFreshWaterYieldChanges[i];
}

/// How much a tech improves the yield of this improvement if it has fresh water
int CvImprovementEntry::GetTechFreshWaterYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumTechInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiTechFreshWaterYieldChanges[i][j];
}

int* CvImprovementEntry::GetTechFreshWaterYieldChangesArray(int i)
{
	return m_ppiTechFreshWaterYieldChanges[i];
}

/// How much a type of route improves the yield of this improvement
int CvImprovementEntry::GetRouteYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumRouteInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiRouteYieldChanges[i][j];
}

int* CvImprovementEntry::GetRouteYieldChangesArray(int i)				// For Moose - CvWidgetData XXX
{
	return m_ppiRouteYieldChanges[i];
}

/// How much a yield improves when a resource is present with the improvement
int CvImprovementEntry::GetImprovementResourceYield(int i, int j) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_paImprovementResource[i].m_piYieldChange ? m_paImprovementResource[i].getYieldChange(j) : 0;
}

/// What resources does this improvement require to be built
bool CvImprovementEntry::IsImprovementResourceMakesValid(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paImprovementResource[i].m_bResourceMakesValid;
}

/// Does this improvement enable a tradeable resource
bool CvImprovementEntry::IsImprovementResourceTrade(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paImprovementResource[i].m_bResourceTrade;
}

bool CvImprovementEntry::IsExpandedImprovementResourceTrade(int i, bool bIgnorePrimary) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	if (!bIgnorePrimary)
	{
		if (m_paImprovementResource[i].m_bResourceTrade)
			return true;
	}

	if (MOD_BALANCE_CORE_DIPLOMACY_ADVANCED)
	{
		if (IsCreatedByGreatPerson() || IsAdjacentCity())
			return true;
	}

	return false;

}

/// the chance of the specified Resource appearing randomly when the Improvement is present with no current Resource
int CvImprovementEntry::GetImprovementResourceDiscoverRand(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paImprovementResource[i].m_iDiscoverRand;
}

/// Gets the flavor value of the improvement
int CvImprovementEntry::GetFlavorValue(int i) const
{
	CvAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(i > -1, "Indes out of bounds");
	return m_piFlavorValue[i];
}


//=====================================
// CvPromotionEntryXMLEntries
//=====================================
/// Constructor
CvImprovementXMLEntries::CvImprovementXMLEntries(void)
{

}

/// Destructor
CvImprovementXMLEntries::~CvImprovementXMLEntries(void)
{
	DeleteArray();
}

/// Returns vector of improvement entries
std::vector<CvImprovementEntry*>& CvImprovementXMLEntries::GetImprovementEntries()
{
	return m_paImprovementEntries;
}

/// Number of defined projects
int CvImprovementXMLEntries::GetNumImprovements()
{
	return m_paImprovementEntries.size();
}

/// Get a specific entry
CvImprovementEntry* CvImprovementXMLEntries::GetEntry(int index)
{
#if defined(MOD_BALANCE_CORE)
	return (index!=NO_IMPROVEMENT) ? m_paImprovementEntries[index] : NULL;
#else
	return m_paImprovementEntries[index];
#endif
}

/// Tell which improvement unlocks a resource
CvImprovementEntry* CvImprovementXMLEntries::GetImprovementForResource(int eResource)
{
	for(unsigned int iImprovement = 0; iImprovement < m_paImprovementEntries.size(); ++iImprovement)
	{
		CvImprovementEntry* pImprovement = GetEntry((ImprovementTypes)iImprovement);
		if(pImprovement && pImprovement->IsImprovementResourceMakesValid(eResource))
		{
			return pImprovement;
		}
	}

	return NULL;
}

/// Clear improvement entries
void CvImprovementXMLEntries::DeleteArray()
{
	for(std::vector<CvImprovementEntry*>::iterator it = m_paImprovementEntries.begin(); it != m_paImprovementEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paImprovementEntries.clear();
}

/// Helper function to read in an integer array of data sized according to number of building types
void ImprovementArrayHelpers::Read(FDataStream& kStream, int* paiImprovementArray)
{
	int iNumEntries;

	kStream >> iNumEntries;

	int iArraySize = GC.getNumImprovementInfos();
	for(int iI = 0; iI < iNumEntries; iI++)
	{
		uint uiHash;
		kStream >> uiHash;
		if (uiHash != 0 && uiHash != (uint)NO_IMPROVEMENT)
		{
			int iType = GC.getInfoTypeForHash(uiHash);
			if(iType != -1 && iType < iArraySize)
			{
				kStream >> paiImprovementArray[iType];
			}
			else
			{
				CvString szError;
				szError.Format("LOAD ERROR: Improvement Type not found");
				GC.LogMessage(szError.GetCString());
				CvAssertMsg(false, szError);

				int iDummy;
				kStream >> iDummy;
			}
		}
	}
}

/// Helper function to write out an integer array of data sized according to number of improvement types
void ImprovementArrayHelpers::Write(FDataStream& kStream, int* paiImprovementArray, int iArraySize)
{
	kStream << iArraySize;

	for(int iI = 0; iI < iArraySize; iI++)
	{
		const ImprovementTypes eImprovement = static_cast<ImprovementTypes>(iI);
		CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
		if(pkImprovementInfo)
		{
			CvInfosSerializationHelper::WriteHashed(kStream, pkImprovementInfo);
			kStream << paiImprovementArray[iI];
		}
		else
		{
			kStream << (int)NO_IMPROVEMENT;
		}
	}
}

/// Helper function to read in an integer array of data sized according to number of building types
void ImprovementArrayHelpers::ReadYieldArray(FDataStream& kStream, int** ppaaiImprovementYieldArray, int iNumYields)
{
	int iNumEntries;

	kStream >> iNumEntries;

	for(int iI = 0; iI < iNumEntries; iI++)
	{
		int iHash;
		kStream >> iHash;
		if(iHash != (int)0)
		{
			int iType = GC.getInfoTypeForHash(iHash);
			if(iType != -1)
			{
				for(int jJ = 0; jJ < iNumYields; jJ++)
				{
					kStream >> ppaaiImprovementYieldArray[iType][jJ];
				}
			}
			else
			{
				CvString szError;
				szError.Format("LOAD ERROR: Improvement Type not found: %08x", iHash);
				GC.LogMessage(szError.GetCString());
				CvAssertMsg(false, szError);

				for(int jJ = 0; jJ < iNumYields; jJ++)
				{
					int iDummy;
					kStream >> iDummy;
				}
			}
		}
	}
}

/// Helper function to write out an integer array of data sized according to number of improvement types
void ImprovementArrayHelpers::WriteYieldArray(FDataStream& kStream, int** ppaaiImprovementYieldArray, int iArraySize)
{
	kStream << iArraySize;

	for(int iI = 0; iI < iArraySize; iI++)
	{
		const ImprovementTypes eImprovement = static_cast<ImprovementTypes>(iI);
		CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
		if(pkImprovementInfo)
		{
			CvInfosSerializationHelper::WriteHashed(kStream, pkImprovementInfo);
			for(int jJ = 0; jJ < NUM_YIELD_TYPES; jJ++)
			{
				kStream << ppaaiImprovementYieldArray[iI][jJ];
			}
		}
		else
		{
			kStream << (int)0;
		}
	}
}
