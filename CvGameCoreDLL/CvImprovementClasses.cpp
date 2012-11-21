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
m_iCulture(0),
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
m_iImprovementPillage(NO_IMPROVEMENT),
m_iImprovementUpgrade(NO_IMPROVEMENT),
m_bHillsMakesValid(false),
m_bFreshWaterMakesValid(false),
m_bRiverSideMakesValid(false),
m_bNoFreshWater(false),
m_bRequiresFlatlands(false),
m_bRequiresFlatlandsOrFreshWater(false),
m_bRequiresFeature(false),
m_bWater(false),
m_bCoastal(false),
m_bDestroyedWhenPillaged(false),
m_bBuildableOnResources(false),
m_bBarbarianCamp(false),
m_bGoody(false),
m_bPermanent(false),
m_bOutsideBorders(false),
m_bCreatedByGreatPerson(false),
m_bSpecificCivRequired(false),
m_eImprovementUsageType(IMPROVEMENTUSAGE_BASIC),
m_eRequiredCivilization(NO_CIVILIZATION),
m_iWorldSoundscapeScriptId(0),
m_piResourceQuantityRequirements(NULL),
m_piPrereqNatureYield(NULL),
m_piYieldChange(NULL),
m_piRiverSideYieldChange(NULL),
m_piCoastalLandYieldChange(NULL),
m_piHillsYieldChange(NULL),
m_piFreshWaterChange(NULL),
m_piAdjacentCityYieldChange(NULL),
m_piAdjacentMountainYieldChange(NULL),
m_piFlavorValue(NULL),
m_pbTerrainMakesValid(NULL),
m_pbFeatureMakesValid(NULL),
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
	SAFE_DELETE_ARRAY(m_piRiverSideYieldChange);
	SAFE_DELETE_ARRAY(m_piCoastalLandYieldChange);
	SAFE_DELETE_ARRAY(m_piHillsYieldChange);
	SAFE_DELETE_ARRAY(m_piFreshWaterChange);
	SAFE_DELETE_ARRAY(m_piAdjacentCityYieldChange);
	SAFE_DELETE_ARRAY(m_piAdjacentMountainYieldChange);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_pbTerrainMakesValid);
	SAFE_DELETE_ARRAY(m_pbFeatureMakesValid);

	if (m_paImprovementResource != NULL)
	{
		SAFE_DELETE_ARRAY(m_paImprovementResource); // XXX make sure this isn't leaking memory...
	}

	if (m_ppiTechYieldChanges != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray( m_ppiTechYieldChanges );
	}

	if( m_ppiTechNoFreshWaterYieldChanges != NULL )
	{
		CvDatabaseUtility::SafeDelete2DArray( m_ppiTechNoFreshWaterYieldChanges );
	}

	if( m_ppiTechFreshWaterYieldChanges != NULL )
	{
		CvDatabaseUtility::SafeDelete2DArray( m_ppiTechFreshWaterYieldChanges );
	}

	if (m_ppiRouteYieldChanges != NULL)
	{
		CvDatabaseUtility::SafeDelete2DArray( m_ppiRouteYieldChanges );
	}
}

/// Read from XML file
bool CvImprovementEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if (!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	//Basic properties
	const char* szArtDefineTag = kResults.GetText("ArtDefineTag");
	SetArtDefineTag(szArtDefineTag);

	m_iGoldMaintenance = kResults.GetInt("GoldMaintenance");
	m_iCulture = kResults.GetInt("Culture");
	m_iCultureAdjacentSameType = kResults.GetInt("CultureAdjacentSameType");
	m_bHillsMakesValid = kResults.GetBool("HillsMakesValid");
	m_bFreshWaterMakesValid = kResults.GetBool("FreshWaterMakesValid");
	m_bRiverSideMakesValid = kResults.GetBool("RiverSideMakesValid");
	m_bNoFreshWater = kResults.GetBool("NoFreshWater");
	m_bRequiresFlatlands = kResults.GetBool("RequiresFlatlands");
	m_bRequiresFlatlandsOrFreshWater = kResults.GetBool("RequiresFlatlandsOrFreshWater");
	m_bRequiresFeature = kResults.GetBool("RequiresFeature");
	m_bWater = kResults.GetBool("Water");
	m_bCoastal = kResults.GetBool("Coastal");
	m_bDestroyedWhenPillaged = kResults.GetBool("DestroyedWhenPillaged");
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
	m_bCreatedByGreatPerson = kResults.GetBool("CreatedByGreatPerson");
	m_bSpecificCivRequired = kResults.GetBool("SpecificCivRequired");
	m_iResourceExtractionMod = kResults.GetInt("ResourceExtractionMod");

	const char* szCivilizationType = kResults.GetText("CivilizationType");
	m_eRequiredCivilization = (CivilizationTypes)GC.getInfoTypeForString(szCivilizationType, true);

	//References
	const char* szWorldsoundscapeAudioScript = kResults.GetText("WorldSoundscapeAudioScript");
	if (szWorldsoundscapeAudioScript != NULL)
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

	kUtility.PopulateArrayByExistence(	m_pbTerrainMakesValid,
												"Terrains",
												"Improvement_ValidTerrains",
												"TerrainType",
												"ImprovementType",
												szImprovementType);

	kUtility.PopulateArrayByExistence(	m_pbFeatureMakesValid,
												"Features",
												"Improvement_ValidFeatures",
												"FeatureType",
												"ImprovementType",
												szImprovementType);

	kUtility.SetYields(m_piYieldChange, "Improvement_Yields", "ImprovementType", szImprovementType);
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
	const int iNumTechs = GC.getNumTechInfos();
	CvAssertMsg(iNumTechs > 0, "Num Tech Infos <= 0");


	//TechYieldChanges
	{
		kUtility.Initialize2DArray( m_ppiTechYieldChanges, iNumTechs, iNumYields );

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
		kUtility.Initialize2DArray( m_ppiTechNoFreshWaterYieldChanges, iNumTechs, iNumYields );

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
		kUtility.Initialize2DArray( m_ppiTechFreshWaterYieldChanges, iNumTechs, iNumYields );

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
		kUtility.Initialize2DArray( m_ppiRouteYieldChanges, iNumRoutes, iNumYields );

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

/// Culture from this Improvement
int CvImprovementEntry::GetCulture() const
{
	return m_iCulture;
}

/// Bonus culture if another Improvement of same type is adjacent
int CvImprovementEntry::GetCultureAdjacentSameType() const
{
	return m_iCultureAdjacentSameType;
}

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

/// Requires hills to be constructed
bool CvImprovementEntry::IsHillsMakesValid() const
{
	return m_bHillsMakesValid;
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

/// the chance of the specified Resource appearing randomly when the Improvement is present with no current Resource
int CvImprovementEntry::GetImprovementResourceDiscoverRand(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_paImprovementResource[i].m_iDiscoverRand;
}

/// Gets the flavor value of the improvement
int CvImprovementEntry::GetFlavorValue (int i) const
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
CvImprovementEntry *CvImprovementXMLEntries::GetEntry(int index)
{
	return m_paImprovementEntries[index];
}

/// Tell which improvement unlocks a resource
CvImprovementEntry *CvImprovementXMLEntries::GetImprovementForResource(int eResource)
{
	for (unsigned int iImprovement = 0; iImprovement < m_paImprovementEntries.size(); ++iImprovement)
	{
		CvImprovementEntry *pImprovement = GetEntry((ImprovementTypes)iImprovement);
		if (pImprovement && pImprovement->IsImprovementResourceMakesValid(eResource))
		{
			return pImprovement;
		}
	}

	return NULL;
}

/// Clear improvement entries
void CvImprovementXMLEntries::DeleteArray()
{
	for (std::vector<CvImprovementEntry*>::iterator it = m_paImprovementEntries.begin(); it != m_paImprovementEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paImprovementEntries.clear();
}

/// Helper function to read in an integer array of data sized according to number of building types
void ImprovementArrayHelpers::Read(FDataStream &kStream, int *paiImprovementArray)
{
	int iNumEntries;
	FStringFixedBuffer(sTemp, 64);
	int iType;

	kStream >> iNumEntries;

	for (int iI = 0; iI < iNumEntries; iI++)
	{
		kStream >> sTemp;
		if (sTemp != "NO_IMPROVEMENT")
		{
			iType = GC.getInfoTypeForString(sTemp);
			if (iType != -1)
			{
				kStream >> paiImprovementArray[iType];
			}
			else
			{
				CvString szError;
				szError.Format("LOAD ERROR: Improvement Type not found: %s", sTemp);
				GC.LogMessage(szError.GetCString());
				CvAssertMsg(false, szError);
				int iDummy;
				kStream >> iDummy;
			}
		}
	}
}

/// Helper function to write out an integer array of data sized according to number of improvement types
void ImprovementArrayHelpers::Write(FDataStream &kStream, int *paiImprovementArray, int iArraySize)
{
	FStringFixedBuffer(sTemp, 64);

	kStream << iArraySize;

	for (int iI = 0; iI < iArraySize; iI++)
	{
		const ImprovementTypes eImprovement = static_cast<ImprovementTypes>(iI);
		CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
		if(pkImprovementInfo)
		{
			sTemp = pkImprovementInfo->GetType();
			kStream << sTemp;
			kStream << paiImprovementArray[iI];
		}
		else
		{
			sTemp = "NO_IMPROVEMENT";
			kStream << sTemp;
		}
	}
}

/// Helper function to read in an integer array of data sized according to number of building types
void ImprovementArrayHelpers::ReadYieldArray(FDataStream &kStream, int **ppaaiImprovementYieldArray)
{
	int iNumEntries;
	FStringFixedBuffer(sTemp, 64);
	int iType;

	kStream >> iNumEntries;

	for (int iI = 0; iI < iNumEntries; iI++)
	{
		kStream >> sTemp;
		if (sTemp != "NO_IMPROVEMENT")
		{
			iType = GC.getInfoTypeForString(sTemp);
			if (iType != -1)
			{
				for (int jJ = 0; jJ < NUM_YIELD_TYPES; jJ++)
				{
					kStream >> ppaaiImprovementYieldArray[iType][jJ];
				}
			}
			else
			{
				CvString szError;
				szError.Format("LOAD ERROR: Improvement Type not found: %s", sTemp);
				GC.LogMessage(szError.GetCString());
				CvAssertMsg(false, szError);
				for (int jJ = 0; jJ < NUM_YIELD_TYPES; jJ++)
				{
					int iDummy;
					kStream >> iDummy;
				}
			}
		}
	}
}

/// Helper function to write out an integer array of data sized according to number of improvement types
void ImprovementArrayHelpers::WriteYieldArray(FDataStream &kStream, int **ppaaiImprovementYieldArray, int iArraySize)
{
	FStringFixedBuffer(sTemp, 64);

	kStream << iArraySize;

	for (int iI = 0; iI < iArraySize; iI++)
	{
		const ImprovementTypes eImprovement = static_cast<ImprovementTypes>(iI);
		CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
		if(pkImprovementInfo)
		{
			sTemp = pkImprovementInfo->GetType();
			kStream << sTemp;
			for (int jJ = 0; jJ < NUM_YIELD_TYPES; jJ++)
			{
				kStream << ppaaiImprovementYieldArray[iI][jJ];
			}
		}
		else
		{
			sTemp = "NO_IMPROVEMENT";
			kStream << sTemp;
		}
	}
}
