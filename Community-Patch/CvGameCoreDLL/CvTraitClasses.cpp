/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvInfosSerializationHelper.h"

#include "LintFree.h"

//======================================================================================================
//					CvTraitEntry
//======================================================================================================
/// Constructor
CvTraitEntry::CvTraitEntry() :
m_iLevelExperienceModifier(0),
m_iGreatPeopleRateModifier(0),
m_iGreatScientistRateModifier(0),
m_iGreatGeneralRateModifier(0),
m_iGreatGeneralExtraBonus(0),
m_iMaxGlobalBuildingProductionModifier(0),
m_iMaxTeamBuildingProductionModifier(0),
m_iMaxPlayerBuildingProductionModifier(0),
m_iCityUnhappinessModifier(0),
m_iPopulationUnhappinessModifier(0),
m_iCityStateBonusModifier(0),
m_iCityStateFriendshipModifier(0),
m_iCityStateCombatModifier(0),
m_iLandBarbarianConversionPercent(0),
m_iLandBarbarianConversionExtraUnits(0),
m_iSeaBarbarianConversionPercent(0),
m_iCapitalBuildingModifier(0),
m_iPlotBuyCostModifier(0),
m_iPlotCultureCostModifier(0),
m_iCultureFromKills(0),
m_iCityCultureBonus(0),
m_iPolicyCostModifier(0),
m_iTradeRouteChange(0),
m_iWonderProductionModifier(0),
m_iPlunderModifier(0),
m_iImprovementMaintenanceModifier(0),
m_iGoldenAgeDurationModifier(0),
m_iGoldenAgeMoveChange(0),
m_iGoldenAgeCombatModifier(0),
m_iObsoleteTech(NO_TECH),
m_iExtraEmbarkMoves(0),
m_iFreeUnitClassType(NO_UNITCLASS),
m_iNaturalWonderFirstFinderGold(0),
m_iNaturalWonderSubsequentFinderGold(0),
m_iNaturalWonderYieldModifier(0),
m_iNaturalWonderHappinessModifier(0),
m_iNearbyImprovementCombatBonus(0),
m_iNearbyImprovementBonusRange(0),
m_iCultureBuildingYieldChange(0),
m_iLandUnitMaintenanceModifier(0),
m_iNavalUnitMaintenanceModifier(0),

m_eFreeUnitPrereqTech(NO_TECH),

m_bFightWellDamaged(false),
m_bMoveFriendlyWoodsAsRoad(false),
m_bFasterAlongRiver(false),
m_bFasterInHills(false),
m_bEmbarkedNotCivilian(false),
m_bEmbarkedAllWater(false),
m_bEmbarkedToLandFlatCost(false),
m_bNoHillsImprovementMaintenance(false),
m_bTechBoostFromCapitalScienceBuildings(false),
m_bStaysAliveZeroCities(false),
m_paiExtraYieldThreshold(NULL),
m_paiYieldChange(NULL),
m_paiYieldChangeStrategicResources(NULL),
m_paiYieldModifier(NULL),
m_piStrategicResourceQuantityModifier(NULL),
m_piResourceQuantityModifiers(NULL),
m_ppiImprovementYieldChanges(NULL),
m_ppiSpecialistYieldChanges(NULL)
{
}

/// Destructor
CvTraitEntry::~CvTraitEntry()
{
	CvDatabaseUtility::SafeDelete2DArray( m_ppiImprovementYieldChanges );
	CvDatabaseUtility::SafeDelete2DArray( m_ppiSpecialistYieldChanges );
}

/// Accessor:: Modifier to experience needed for new level
int CvTraitEntry::GetLevelExperienceModifier() const
{
	return m_iLevelExperienceModifier;
}

/// Accessor:: Great person generation rate change
int CvTraitEntry::GetGreatPeopleRateModifier() const
{
	return m_iGreatPeopleRateModifier;
}

/// Accessor:: Great scientist generation rate change
int CvTraitEntry::GetGreatScientistRateModifier() const
{
	return m_iGreatScientistRateModifier;
}

/// Accessor:: Great general generation rate change
int CvTraitEntry::GetGreatGeneralRateModifier() const
{
	return m_iGreatGeneralRateModifier;
}

/// Accessor:: Great general additional combat bonus
int CvTraitEntry::GetGreatGeneralExtraBonus() const
{
	return m_iGreatGeneralExtraBonus;
}

/// Accessor:: Overall production boost
int CvTraitEntry::GetMaxGlobalBuildingProductionModifier() const
{
	return m_iMaxGlobalBuildingProductionModifier;
}

/// Accessor:: Team production boost
int CvTraitEntry::GetMaxTeamBuildingProductionModifier() const
{
	return m_iMaxTeamBuildingProductionModifier;
}

/// Accessor:: Player production boost
int CvTraitEntry::GetMaxPlayerBuildingProductionModifier() const
{
	return m_iMaxPlayerBuildingProductionModifier;
}

/// Accessor:: City unhappiness change
int CvTraitEntry::GetCityUnhappinessModifier() const
{
	return m_iCityUnhappinessModifier;
}

/// Accessor:: Unhappiness change based on overall population
int CvTraitEntry::GetPopulationUnhappinessModifier() const
{
	return m_iPopulationUnhappinessModifier;
}

/// Accessor:: percent boost in value of city state bonuses
int CvTraitEntry::GetCityStateBonusModifier() const
{
	return m_iCityStateBonusModifier;
}

/// Accessor:: percent boost in value of city state bonuses
int CvTraitEntry::GetCityStateFriendshipModifier() const
{
	return m_iCityStateFriendshipModifier;
}

/// Accessor:: percent boost in value of city state bonuses
int CvTraitEntry::GetCityStateCombatModifier() const
{
	return m_iCityStateCombatModifier;
}

/// Accessor:: percent chance a barbarian camp joins this civ
int CvTraitEntry::GetLandBarbarianConversionPercent() const
{
	return m_iLandBarbarianConversionPercent;
}

/// Accessor:: extra copies of the camp guard created if it joins this civ
int CvTraitEntry::GetLandBarbarianConversionExtraUnits() const
{
	return m_iLandBarbarianConversionExtraUnits;
}

/// Accessor:: percent chance a barbarian naval unit joins this civ
int CvTraitEntry::GetSeaBarbarianConversionPercent() const
{
	return m_iSeaBarbarianConversionPercent;
}

/// Accessor:: discount when constructing a building that is already present in capital
int CvTraitEntry::GetCapitalBuildingModifier() const
{
	return m_iCapitalBuildingModifier;
}

/// Accessor:: cheaper purchase of tiles for culture border expansion
int CvTraitEntry::GetPlotBuyCostModifier() const
{
	return m_iPlotBuyCostModifier;
}

/// Accessor:: increased rate of culture border expansion
int CvTraitEntry::GetPlotCultureCostModifier() const
{
	return m_iPlotCultureCostModifier;
}

/// Accessor:: increased rate of culture border expansion
int CvTraitEntry::GetCultureFromKills() const
{
	return m_iCultureFromKills;
}

/// Accessor:: extra culture from buildings that provide culture
int CvTraitEntry::GetCityCultureBonus() const
{
	return m_iCityCultureBonus;
}

/// Accessor:: discount on buying policies
int CvTraitEntry::GetPolicyCostModifier() const
{
	return m_iPolicyCostModifier;
}

/// Accessor:: extra money from trade routes
int CvTraitEntry::GetTradeRouteChange() const
{
	return m_iTradeRouteChange;
}

/// Accessor:: boost in wonder building speed
int CvTraitEntry::GetWonderProductionModifier() const
{
	return m_iWonderProductionModifier;
}

/// Accessor:: boost in gold received from cities and barbarian camps
int CvTraitEntry::GetPlunderModifier() const
{
	return m_iPlunderModifier;
}

/// Accessor:: percent change to cost of improvement maintenance
int CvTraitEntry::GetImprovementMaintenanceModifier() const
{
	return m_iImprovementMaintenanceModifier;
}

/// Accessor:: extra length of each golden age
int CvTraitEntry::GetGoldenAgeDurationModifier() const
{
	return m_iGoldenAgeDurationModifier;
}

/// Accessor:: movement bonus during golden ages
int CvTraitEntry::GetGoldenAgeMoveChange() const
{
	return m_iGoldenAgeMoveChange;
}

/// Accessor:: combat bonus during golden ages
int CvTraitEntry::GetGoldenAgeCombatModifier() const
{
	return m_iGoldenAgeCombatModifier;
}

/// Accessor:: combat bonus during golden ages
int CvTraitEntry::GetExtraEmbarkMoves() const
{
	return m_iExtraEmbarkMoves;
}

/// Accessor: what unit does this trait provide?
int CvTraitEntry::GetFreeUnitClassType() const
{
	return m_iFreeUnitClassType;
}

/// Accessor:: bonus gold for being first to find a natural wonder
int CvTraitEntry::GetNaturalWonderFirstFinderGold() const
{
	return m_iNaturalWonderFirstFinderGold;
}

/// Accessor:: bonus gold for being first to find a natural wonder
int CvTraitEntry::GetNaturalWonderSubsequentFinderGold() const
{
	return m_iNaturalWonderSubsequentFinderGold;
}

/// Accessor:: modifier to bonuses for having natural wonders worked or in territory
int CvTraitEntry::GetNaturalWonderYieldModifier() const
{
	return m_iNaturalWonderYieldModifier;
}

/// Accessor: modifier to happiness received from finding natural wonders
int CvTraitEntry::GetNaturalWonderHappinessModifier() const
{
	return m_iNaturalWonderHappinessModifier;
}

/// Accessor: size of combat bonus for nearby improvements
int CvTraitEntry::GetNearbyImprovementCombatBonus() const
{
	return m_iNearbyImprovementCombatBonus;
}

/// Accessor: range of bonus from nearby improvements
int CvTraitEntry::GetNearbyImprovementBonusRange() const
{
	return m_iNearbyImprovementBonusRange;
}

/// Accessor: extra yield for culture buildings
int CvTraitEntry::GetCultureBuildingYieldChange() const
{
	return m_iCultureBuildingYieldChange;
}

/// Accessor: change to usual cost for maintaining land units
int CvTraitEntry::GetLandUnitMaintenanceModifier() const
{
	return m_iLandUnitMaintenanceModifier;
}

/// Accessor: change to usual cost for maintaining naval units
int CvTraitEntry::GetNavalUnitMaintenanceModifier() const
{
	return m_iNavalUnitMaintenanceModifier;
}

/// Accessor: tech that triggers this free unit
TechTypes CvTraitEntry::GetFreeUnitPrereqTech() const
{
	return m_eFreeUnitPrereqTech;
}

/// Accessor: tech that triggers this free unit
ImprovementTypes CvTraitEntry::GetCombatBonusImprovement() const
{
	return m_eCombatBonusImprovement;
}

/// Accessor:: does this civ get combat bonuses when damaged?
bool CvTraitEntry::IsFightWellDamaged() const
{
	return m_bFightWellDamaged;
}

/// Accessor:: does this civ move units through forest as if it is road?
bool CvTraitEntry::IsMoveFriendlyWoodsAsRoad() const
{
	return m_bMoveFriendlyWoodsAsRoad;
}

/// Accessor:: does this civ move along rivers like a Scout?
bool CvTraitEntry::IsFasterAlongRiver() const
{
	return m_bFasterAlongRiver;
}

/// Accessor:: does this civ move in Hills like a Scout?
bool CvTraitEntry::IsFasterInHills() const
{
	return m_bFasterInHills;
}

/// Accessor:: are our embarked units immune from capture?
bool CvTraitEntry::IsEmbarkedNotCivilian() const
{
	return m_bEmbarkedNotCivilian;
}

/// Accessor:: are our embarked units allowed to enter ocean?
bool CvTraitEntry::IsEmbarkedAllWater() const
{
	return m_bEmbarkedAllWater;
}

/// Accessor:: are our embarked units able to move to land for 1 MP?
bool CvTraitEntry::IsEmbarkedToLandFlatCost() const
{
	return m_bEmbarkedToLandFlatCost;
}

/// Accessor:: free improvement maintenance in hills?
bool CvTraitEntry::IsNoHillsImprovementMaintenance() const
{
	return m_bNoHillsImprovementMaintenance;
}

/// Accessor:: tech boost when build science building in capital?
bool CvTraitEntry::IsTechBoostFromCapitalScienceBuildings() const
{
	return m_bTechBoostFromCapitalScienceBuildings;
}

/// Accessor:: does this civ still exist with zero cities?
bool CvTraitEntry::IsStaysAliveZeroCities() const
{
	return m_bStaysAliveZeroCities;
}

/// Accessor:: Get brief text description
const char* CvTraitEntry::getShortDescription() const
{
	return m_strShortDescription;
}

/// Accessor:: Set brief text description
void CvTraitEntry::setShortDescription(const char* szVal)
{
	m_strShortDescription = szVal;
}

/// Accessor:: 1 extra yield comes all tiles with a base yield of this
int CvTraitEntry::GetExtraYieldThreshold(int i) const
{
	return m_paiExtraYieldThreshold ? m_paiExtraYieldThreshold[i] : -1;
}

/// Accessor:: Additional yield
int CvTraitEntry::GetYieldChange(int i) const
{
	return m_paiYieldChange ? m_paiYieldChange[i] : -1;
}

/// Accessor:: Extra yield from strategic resources
int CvTraitEntry::GetYieldChangeStrategicResources(int i) const
{
	return m_paiYieldChangeStrategicResources ? m_paiYieldChangeStrategicResources[i] : -1;
}

/// Accessor:: Modifier to yield
int CvTraitEntry::GetYieldModifier(int i) const
{
	return m_paiYieldModifier ? m_paiYieldModifier[i] : -1;
}

/// Accessor:: Additional quantity of strategic resources
int CvTraitEntry::GetStrategicResourceQuantityModifier(int i) const
{
	return m_piStrategicResourceQuantityModifier ? m_piStrategicResourceQuantityModifier[i] : -1;
}

/// Accessor:: Additional quantity of a specific resource
int CvTraitEntry::GetResourceQuantityModifier(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piResourceQuantityModifiers ? m_piResourceQuantityModifiers[i] : -1;
}

/// Accessor:: Extra yield from an improvement
int CvTraitEntry::GetImprovementYieldChanges(ImprovementTypes eIndex1, YieldTypes eIndex2) const
{
	CvAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "Index out of bounds");
	CvAssertMsg(eIndex1 > -1, "Index out of bounds");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(eIndex2 > -1, "Index out of bounds");
	return m_ppiImprovementYieldChanges ? m_ppiImprovementYieldChanges[eIndex1][eIndex2] : 0;
}

/// Accessor:: Extra yield from an improvement
int CvTraitEntry::GetSpecialistYieldChanges(SpecialistTypes eIndex1, YieldTypes eIndex2) const
{
	CvAssertMsg(eIndex1 < GC.getNumSpecialistInfos(), "Index out of bounds");
	CvAssertMsg(eIndex1 > -1, "Index out of bounds");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(eIndex2 > -1, "Index out of bounds");
	return m_ppiSpecialistYieldChanges ? m_ppiSpecialistYieldChanges[eIndex1][eIndex2] : 0;
}

/// Accessor:: Additional moves for a class of combat unit
int CvTraitEntry::GetMovesChangeUnitCombat(const int unitCombatID) const
{
	CvAssertMsg((unitCombatID >= 0), "unitCombatID is less than zero");
	CvAssertMsg((unitCombatID < GC.getNumUnitCombatClassInfos()), "unitCombatID exceeds number of combat classes");

	return m_piMovesChangeUnitCombats[unitCombatID];
}

/// Tech that makes this trait obsolete
int CvTraitEntry::GetObsoleteTech() const
{
	return m_iObsoleteTech;
}

/// Accessor:: Does the civ get free promotions?
bool CvTraitEntry::IsFreePromotionUnitCombat(const int promotionID, const int unitCombatID) const
{
	std::multimap<int, int>::const_iterator it = m_FreePromotionUnitCombats.find(promotionID);
	if(it != m_FreePromotionUnitCombats.end())
	{
		// get an iterator to the element that is one past the last element associated with key
		std::multimap<int, int>::const_iterator lastElement = m_FreePromotionUnitCombats.upper_bound(promotionID);

		// for each element in the sequence [itr, lastElement)
		for ( ; it != lastElement; ++it)
		{
			if(it->second == unitCombatID)
			{
				return true;
			}
		}
	}

	return false;
}

/// Has this trait become obsolete?
bool CvTraitEntry::IsObsoleteByTech(TeamTypes eTeam)
{
	if (m_iObsoleteTech != NO_TECH)
	{
		if (GET_TEAM(eTeam).GetTeamTechs()->HasTech((TechTypes)m_iObsoleteTech))
		{
			return true;
		}
	}
	return false;
}

/// Load XML data
bool CvTraitEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	//Basic Properties
	setShortDescription(kResults.GetText("ShortDescription"));

	m_iLevelExperienceModifier				= kResults.GetInt("LevelExperienceModifier");
	m_iGreatPeopleRateModifier				= kResults.GetInt("GreatPeopleRateModifier");
	m_iGreatScientistRateModifier			= kResults.GetInt("GreatScientistRateModifier");
	m_iGreatGeneralRateModifier				= kResults.GetInt("GreatGeneralRateModifier");
	m_iGreatGeneralExtraBonus				= kResults.GetInt("GreatGeneralExtraBonus");
	m_iMaxGlobalBuildingProductionModifier	= kResults.GetInt("MaxGlobalBuildingProductionModifier");
	m_iMaxTeamBuildingProductionModifier	= kResults.GetInt("MaxTeamBuildingProductionModifier");
	m_iMaxPlayerBuildingProductionModifier	= kResults.GetInt("MaxPlayerBuildingProductionModifier");
	m_iCityUnhappinessModifier           	= kResults.GetInt("CityUnhappinessModifier");
	m_iPopulationUnhappinessModifier    	= kResults.GetInt("PopulationUnhappinessModifier");
	m_iCityStateBonusModifier               = kResults.GetInt("CityStateBonusModifier");
	m_iCityStateFriendshipModifier          = kResults.GetInt("CityStateFriendshipModifier");
	m_iCityStateCombatModifier				= kResults.GetInt("CityStateCombatModifier");
	m_iLandBarbarianConversionPercent       = kResults.GetInt("LandBarbarianConversionPercent");
	m_iLandBarbarianConversionExtraUnits    = kResults.GetInt("LandBarbarianConversionExtraUnits");
	m_iSeaBarbarianConversionPercent        = kResults.GetInt("SeaBarbarianConversionPercent");
	m_iCapitalBuildingModifier				= kResults.GetInt("CapitalBuildingModifier");
	m_iPlotBuyCostModifier					= kResults.GetInt("PlotBuyCostModifier");
	m_iPlotCultureCostModifier              = kResults.GetInt("PlotCultureCostModifier");
	m_iCultureFromKills						= kResults.GetInt("CultureFromKills");
	m_iCityCultureBonus						= kResults.GetInt("CityCultureBonus");
	m_iPolicyCostModifier					= kResults.GetInt("PolicyCostModifier");
	m_iTradeRouteChange						= kResults.GetInt("TradeRouteChange");
	m_iWonderProductionModifier				= kResults.GetInt("WonderProductionModifier");
	m_iPlunderModifier						= kResults.GetInt("PlunderModifier");
	m_iImprovementMaintenanceModifier       = kResults.GetInt("ImprovementMaintenanceModifier");
	m_iGoldenAgeDurationModifier			= kResults.GetInt("GoldenAgeDurationModifier");
	m_iGoldenAgeMoveChange				    = kResults.GetInt("GoldenAgeMoveChange");
	m_iGoldenAgeCombatModifier				= kResults.GetInt("GoldenAgeCombatModifier");
	m_iExtraEmbarkMoves						= kResults.GetInt("ExtraEmbarkMoves");
	m_iNaturalWonderFirstFinderGold         = kResults.GetInt("NaturalWonderFirstFinderGold");
	m_iNaturalWonderSubsequentFinderGold    = kResults.GetInt("NaturalWonderSubsequentFinderGold");
	m_iNaturalWonderYieldModifier           = kResults.GetInt("NaturalWonderYieldModifier");
	m_iNaturalWonderHappinessModifier       = kResults.GetInt("NaturalWonderHappinessModifier");
	m_iNearbyImprovementCombatBonus			= kResults.GetInt("NearbyImprovementCombatBonus");
	m_iNearbyImprovementBonusRange			= kResults.GetInt("NearbyImprovementBonusRange");
	m_iCultureBuildingYieldChange			= kResults.GetInt("CultureBuildingYieldChange");
	m_iLandUnitMaintenanceModifier          = kResults.GetInt("LandUnitMaintenanceModifier");
	m_iNavalUnitMaintenanceModifier         = kResults.GetInt("NavalUnitMaintenanceModifier");

	const char* szTextVal = NULL;
	szTextVal = kResults.GetText("FreeUnit");
	if (szTextVal)
	{
		m_iFreeUnitClassType = GC.getInfoTypeForString(szTextVal, true);
	}

	szTextVal = kResults.GetText("FreeUnitPrereqTech");
	if (szTextVal)
	{
		m_eFreeUnitPrereqTech = (TechTypes)GC.getInfoTypeForString(szTextVal, true);
	}

	szTextVal = kResults.GetText("CombatBonusImprovement");
	if (szTextVal)
	{
		m_eCombatBonusImprovement = (ImprovementTypes)GC.getInfoTypeForString(szTextVal, true);
	}

	szTextVal = kResults.GetText("ObsoleteTech");
	if (szTextVal)
	{
		m_iObsoleteTech = GC.getInfoTypeForString(szTextVal, true);
	}

	m_bFightWellDamaged = kResults.GetBool("FightWellDamaged");
	m_bMoveFriendlyWoodsAsRoad = kResults.GetBool("MoveFriendlyWoodsAsRoad");
	m_bFasterAlongRiver = kResults.GetBool("FasterAlongRiver");
	m_bFasterInHills = kResults.GetBool("FasterInHills");
	m_bEmbarkedNotCivilian = kResults.GetBool("EmbarkedNotCivilian");
	m_bEmbarkedAllWater = kResults.GetBool("EmbarkedAllWater");
	m_bEmbarkedToLandFlatCost = kResults.GetBool("EmbarkedToLandFlatCost");
	m_bNoHillsImprovementMaintenance = kResults.GetBool("NoHillsImprovementMaintenance");
	m_bTechBoostFromCapitalScienceBuildings = kResults.GetBool("TechBoostFromCapitalScienceBuildings");
	m_bStaysAliveZeroCities = kResults.GetBool("StaysAliveZeroCities");

	//Arrays
	const char* szTraitType = GetType();
	kUtility.SetYields(m_paiExtraYieldThreshold, "Trait_ExtraYieldThresholds", "TraitType", szTraitType);

	kUtility.SetYields(m_paiYieldChange, "Trait_YieldChanges", "TraitType", szTraitType);
	kUtility.SetYields(m_paiYieldChangeStrategicResources, "Trait_YieldChangesStrategicResources", "TraitType", szTraitType);
	kUtility.SetYields(m_paiYieldModifier, "Trait_YieldModifiers", "TraitType", szTraitType);

	const int iNumTerrains = GC.getNumTerrainInfos();

	//Trait_Terrains
	{
		kUtility.InitializeArray(m_piStrategicResourceQuantityModifier, iNumTerrains, 0);

		std::string sqlKey = "Trait_Terrains";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select TraitType, Terrains.ID as TerrainID, StrategicResourceQuantityModifier from Trait_Terrains join Terrains on Terrains.Type = TerrainType where TraitType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int iTerrainID = pResults->GetInt("TerrainID");
			CvAssert(iTerrainID > -1 && iTerrainID < iNumTerrains);

			const int iStrategicResourceQuantityModifier = pResults->GetInt("StrategicResourceQuantityModifier");
			m_piStrategicResourceQuantityModifier[iTerrainID] = iStrategicResourceQuantityModifier;
		}
	}

	//Populate m_FreePromotionUnitCombats
	{
		std::string sqlKey = "FreePromotionUnitCombats";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select UnitPromotions.ID, UnitCombatInfos.ID from Trait_FreePromotionUnitCombats, UnitPromotions, UnitCombatInfos where TraitType = ? and PromotionType = UnitPromotions.Type and UnitCombatType = UnitCombatInfos.Type";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int unitPromotionID = pResults->GetInt(0);
			const int unitCombatInfoID = pResults->GetInt(1);

			m_FreePromotionUnitCombats.insert(std::pair<int, int>(unitPromotionID, unitCombatInfoID));
		}

		pResults->Reset();

		//Trim extra memory off container since this is mostly read-only.
		std::multimap<int,int>(m_FreePromotionUnitCombats).swap(m_FreePromotionUnitCombats);

		kUtility.PopulateArrayByValue(m_piResourceQuantityModifiers, "Resources", "Trait_ResourceQuantityModifiers", "ResourceType", "TraitType", szTraitType, "ResourceQuantityModifier");
	}

	//Populate m_MovesChangeUnitCombats
	{
		const int iNumUnitCombatClasses = kUtility.MaxRows("UnitCombatInfos");
		kUtility.InitializeArray(m_piMovesChangeUnitCombats, iNumUnitCombatClasses, 0);

		std::string sqlKey = "Trait_MovesChangeUnitCombats";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select UnitCombatInfos.ID, MovesChange from Trait_MovesChangeUnitCombats inner join UnitCombatInfos on UnitCombatInfos.Type = UnitCombatType where TraitType = ?;";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int iUnitCombatID = pResults->GetInt(0);
			CvAssert(iUnitCombatID > -1 && iUnitCombatID < iNumUnitCombatClasses);

			const int iMovesChange = pResults->GetInt(1);
			m_piMovesChangeUnitCombats[iUnitCombatID] = iMovesChange;
		}

		pResults->Reset();
	}

	//ImprovementYieldChanges
	{
		kUtility.Initialize2DArray(m_ppiImprovementYieldChanges, "Improvements", "Yields");

		std::string strKey("Trait_ImprovementYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Improvements.ID as ImprovementID, Yields.ID as YieldID, Yield from Trait_ImprovementYieldChanges inner join Improvements on Improvements.Type = ImprovementType inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int ImprovementID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiImprovementYieldChanges[ImprovementID][YieldID] = yield;
		}
	}

	//SpecialistYieldChanges
	{
		kUtility.Initialize2DArray(m_ppiSpecialistYieldChanges, "Specialists", "Yields");

		std::string strKey("Building_SpecialistYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Specialists.ID as SpecialistID, Yields.ID as YieldID, Yield from Trait_SpecialistYieldChanges inner join Specialists on Specialists.Type = SpecialistType inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int SpecialistID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiSpecialistYieldChanges[SpecialistID][YieldID] = yield;
		}
	}

	return true;
}

//=====================================
// CvTraitXMLEntries
//=====================================
/// Constructor
CvTraitXMLEntries::CvTraitXMLEntries(void)
{

}

/// Destructor
CvTraitXMLEntries::~CvTraitXMLEntries(void)
{
	DeleteArray();
}

/// Returns vector of trait entries
std::vector<CvTraitEntry*>& CvTraitXMLEntries::GetTraitEntries()
{
	return m_paTraitEntries;
}

/// Number of defined traits
int CvTraitXMLEntries::GetNumTraits()
{
	return m_paTraitEntries.size();
}

/// Clear trait entries
void CvTraitXMLEntries::DeleteArray()
{
	for (std::vector<CvTraitEntry*>::iterator it = m_paTraitEntries.begin(); it != m_paTraitEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paTraitEntries.clear();
}

/// Get a specific entry
CvTraitEntry *CvTraitXMLEntries::GetEntry(int index)
{
	return m_paTraitEntries[index];
}

//=====================================
// CvPlayerTraits
//=====================================
/// Constructor
CvPlayerTraits::CvPlayerTraits ()
{
}

/// Destructor
CvPlayerTraits::~CvPlayerTraits (void)
{
	Uninit();
}

/// Initialize
void CvPlayerTraits::Init(CvTraitXMLEntries *pTraits, CvPlayer *pPlayer)
{
	// Store off the pointers to objects we'll need later
	m_pTraits = pTraits;
	m_pPlayer = pPlayer;

	Reset();
}

/// Store off data on bonuses from traits
void CvPlayerTraits::InitPlayerTraits()
{
	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (HasTrait((TraitTypes)iI))
		{
			CvTraitEntry* trait = GC.getTraitInfo((TraitTypes)iI);
			m_iGreatPeopleRateModifier += trait->GetGreatPeopleRateModifier();
			m_iGreatScientistRateModifier += trait->GetGreatScientistRateModifier();
			m_iGreatGeneralRateModifier += trait->GetGreatGeneralRateModifier();
			m_iGreatGeneralExtraBonus += trait->GetGreatGeneralExtraBonus();
			m_iLevelExperienceModifier += trait->GetLevelExperienceModifier();
			m_iMaxGlobalBuildingProductionModifier += trait->GetMaxGlobalBuildingProductionModifier();
			m_iMaxTeamBuildingProductionModifier += trait->GetMaxTeamBuildingProductionModifier();
			m_iMaxPlayerBuildingProductionModifier += trait->GetMaxPlayerBuildingProductionModifier();
			m_iCityUnhappinessModifier += trait->GetCityUnhappinessModifier();
			m_iPopulationUnhappinessModifier += trait->GetPopulationUnhappinessModifier();
			m_iCityStateBonusModifier += trait->GetCityStateBonusModifier();
			m_iCityStateFriendshipModifier += trait->GetCityStateFriendshipModifier();
			m_iCityStateCombatModifier += trait->GetCityStateCombatModifier();
			m_iLandBarbarianConversionPercent += trait->GetLandBarbarianConversionPercent();
			m_iLandBarbarianConversionExtraUnits += trait->GetLandBarbarianConversionExtraUnits();
			m_iSeaBarbarianConversionPercent += trait->GetSeaBarbarianConversionPercent();
			m_iCapitalBuildingModifier += trait->GetCapitalBuildingModifier();
			m_iPlotBuyCostModifier += trait->GetPlotBuyCostModifier();
			m_iPlotCultureCostModifier += trait->GetPlotCultureCostModifier();
			m_iCultureFromKills += trait->GetCultureFromKills();
			m_iCityCultureBonus += trait->GetCityCultureBonus();
			m_iPolicyCostModifier += trait->GetPolicyCostModifier();
			m_iTradeRouteChange += trait->GetTradeRouteChange();
			m_iWonderProductionModifier += trait->GetWonderProductionModifier();
			m_iPlunderModifier += trait->GetPlunderModifier();
			m_iImprovementMaintenanceModifier += trait->GetImprovementMaintenanceModifier();
			m_iGoldenAgeDurationModifier += trait->GetGoldenAgeDurationModifier();
			m_iGoldenAgeMoveChange += trait->GetGoldenAgeMoveChange();
			m_iGoldenAgeCombatModifier += trait->GetGoldenAgeCombatModifier();
			m_iExtraEmbarkMoves += trait->GetExtraEmbarkMoves();
			m_iNaturalWonderFirstFinderGold += trait->GetNaturalWonderFirstFinderGold();
			m_iNaturalWonderSubsequentFinderGold += trait->GetNaturalWonderSubsequentFinderGold();
			m_iNaturalWonderYieldModifier += trait->GetNaturalWonderYieldModifier();
			m_iNaturalWonderHappinessModifier += trait->GetNaturalWonderHappinessModifier();
			m_iNearbyImprovementCombatBonus += trait->GetNearbyImprovementCombatBonus();
			m_iNearbyImprovementBonusRange += trait->GetNearbyImprovementBonusRange();
			m_iCultureBuildingYieldChange += trait->GetCultureBuildingYieldChange();
			m_iLandUnitMaintenanceModifier += trait->GetLandUnitMaintenanceModifier();
			m_iNavalUnitMaintenanceModifier += trait->GetNavalUnitMaintenanceModifier();

			if (trait->IsFightWellDamaged())
			{
				m_bFightWellDamaged = true;
				// JON: Changing the way this works. Above line can/should probably be removed at some point
				int iWoundedUnitDamageMod = /*-50*/ GC.getTRAIT_WOUNDED_DAMAGE_MOD();
				m_pPlayer->ChangeWoundedUnitDamageMod(iWoundedUnitDamageMod);
			}
			if (trait->IsMoveFriendlyWoodsAsRoad())
			{
				m_bMoveFriendlyWoodsAsRoad = true;
			}
			if (trait->IsFasterAlongRiver())
			{
				m_bFasterAlongRiver = true;
			}
			if (trait->IsFasterInHills())
			{
				m_bFasterInHills = true;
			}
			if (trait->IsEmbarkedNotCivilian())
			{
				m_bEmbarkedNotCivilian = true;
			}
			if (trait->IsEmbarkedAllWater())
			{
				m_bEmbarkedAllWater = true;
			}
			if (trait->IsEmbarkedToLandFlatCost())
			{
				m_bEmbarkedToLandFlatCost = true;
			}
			if (trait->IsNoHillsImprovementMaintenance())
			{
				m_bNoHillsImprovementMaintenance = true;
			}
			if (trait->IsTechBoostFromCapitalScienceBuildings())
			{
				m_bTechBoostFromCapitalScienceBuildings = true;
			}
			if (trait->IsStaysAliveZeroCities())
			{
				m_bStaysAliveZeroCities = true;
			}

			for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
			{
				if (trait->GetExtraYieldThreshold(iYield) > m_iExtraYieldThreshold[iYield])
				{
					m_iExtraYieldThreshold[iYield] = trait->GetExtraYieldThreshold(iYield);
				}
				m_iFreeCityYield[iYield] = trait->GetYieldChange(iYield);
				m_iYieldChangeStrategicResources[iYield] = trait->GetYieldChangeStrategicResources(iYield);
				m_iYieldRateModifier[iYield] = trait->GetYieldModifier(iYield);

				for (int iImprovementLoop = 0; iImprovementLoop < GC.getNumImprovementInfos(); iImprovementLoop++)
				{
					int iChange = trait->GetImprovementYieldChanges((ImprovementTypes)iImprovementLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiImprovementYieldChange[iImprovementLoop];
						yields[iYield] = (m_ppaaiImprovementYieldChange[iImprovementLoop][iYield] + iChange);
						m_ppaaiImprovementYieldChange[iImprovementLoop] = yields;
					}
				}

				for (int iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
				{
					int iChange = trait->GetSpecialistYieldChanges((SpecialistTypes)iSpecialistLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiSpecialistYieldChange[iSpecialistLoop];
						yields[iYield] = (m_ppaaiSpecialistYieldChange[iSpecialistLoop][iYield] + iChange);
						m_ppaaiSpecialistYieldChange[iSpecialistLoop] = yields;
					}
				}
			}
			CvAssert(GC.getNumTerrainInfos() <= NUM_TERRAIN_TYPES);
			for (int iTerrain = 0; iTerrain < GC.getNumTerrainInfos(); iTerrain++)
			{
				m_iStrategicResourceQuantityModifier[iTerrain] = trait->GetStrategicResourceQuantityModifier(iTerrain);
			}

			for (int iResource = 0; iResource < GC.getNumResourceInfos(); iResource++)
			{
				m_aiResourceQuantityModifier[iResource] = trait->GetResourceQuantityModifier(iResource);
			}

			FreeTraitUnit traitUnit;
			traitUnit.m_iFreeUnit = (UnitTypes)trait->GetFreeUnitClassType();
			if (traitUnit.m_iFreeUnit != NO_UNITCLASS)
			{
				traitUnit.m_ePrereqTech = trait->GetFreeUnitPrereqTech();
				m_aFreeTraitUnits.push_back(traitUnit);
			}

			m_eCombatBonusImprovement = trait->GetCombatBonusImprovement();

			int iNumUnitCombatClassInfos = GC.getNumUnitCombatClassInfos();
			for (int jJ= 0; jJ < iNumUnitCombatClassInfos; jJ++)
			{
				m_paiMovesChangeUnitCombat[jJ] += trait->GetMovesChangeUnitCombat(jJ);
			}
		}
	}
}

/// Deallocate memory created in initialize
void CvPlayerTraits::Uninit()
{
	m_aiResourceQuantityModifier.clear();
	m_paiMovesChangeUnitCombat.clear();
	m_ppaaiImprovementYieldChange.clear();
	m_ppaaiSpecialistYieldChange.clear();
}

/// Reset data members
void CvPlayerTraits::Reset()
{
	Uninit();

	m_iGreatPeopleRateModifier = 0;
	m_iGreatScientistRateModifier = 0;
	m_iGreatGeneralRateModifier = 0;
	m_iGreatGeneralExtraBonus = 0;
	m_iLevelExperienceModifier= 0;
	m_iMaxGlobalBuildingProductionModifier = 0;
	m_iMaxTeamBuildingProductionModifier = 0;
	m_iMaxPlayerBuildingProductionModifier = 0;
    m_iCityUnhappinessModifier = 0;
	m_iPopulationUnhappinessModifier = 0;
	m_iCityStateBonusModifier = 0;
	m_iCityStateFriendshipModifier = 0;
	m_iCityStateCombatModifier = 0;
	m_iLandBarbarianConversionPercent = 0;
	m_iLandBarbarianConversionExtraUnits = 0;
	m_iSeaBarbarianConversionPercent = 0;
	m_iCapitalBuildingModifier = 0;
	m_iPlotBuyCostModifier = 0;
    m_iPlotCultureCostModifier = 0;
	m_iCultureFromKills = 0;
	m_iCityCultureBonus = 0;
	m_iPolicyCostModifier = 0;
	m_iTradeRouteChange = 0;
	m_iWonderProductionModifier = 0;
	m_iPlunderModifier = 0;
	m_iImprovementMaintenanceModifier = 0;
	m_iGoldenAgeDurationModifier = 0;
	m_iGoldenAgeMoveChange = 0;
	m_iGoldenAgeCombatModifier = 0;
	m_iExtraEmbarkMoves = 0;
	m_iNaturalWonderFirstFinderGold = 0;
	m_iNaturalWonderSubsequentFinderGold = 0;
	m_iNaturalWonderYieldModifier = 0;
	m_iNaturalWonderHappinessModifier = 0;
	m_iNearbyImprovementCombatBonus = 0;
	m_iNearbyImprovementBonusRange = 0;
	m_iCultureBuildingYieldChange = 0;
	m_iLandUnitMaintenanceModifier = 0;
	m_iNavalUnitMaintenanceModifier = 0;

	m_bFightWellDamaged = false;
	m_bMoveFriendlyWoodsAsRoad = false;
	m_bFasterAlongRiver = false;
	m_bFasterInHills = false;
	m_bEmbarkedNotCivilian = false;
	m_bEmbarkedAllWater = false;
	m_bEmbarkedToLandFlatCost = false;
	m_bNoHillsImprovementMaintenance = false;
	m_bTechBoostFromCapitalScienceBuildings = false;
	m_bStaysAliveZeroCities = false;

	m_eCampGuardType = NO_UNIT;
	m_eCombatBonusImprovement = NO_IMPROVEMENT;

	m_ppaaiImprovementYieldChange.clear();
	m_ppaaiImprovementYieldChange.resize(GC.getNumImprovementInfos());
	m_ppaaiSpecialistYieldChange.clear();
	m_ppaaiSpecialistYieldChange.resize(GC.getNumSpecialistInfos());

	Firaxis::Array< int, NUM_YIELD_TYPES > yield;
	for( unsigned int j = 0; j < NUM_YIELD_TYPES; ++j )
	{
		yield[j] = 0;
	}

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		m_iExtraYieldThreshold[iYield] = 0;
		m_iFreeCityYield[iYield] = 0;
		m_iYieldChangeStrategicResources[iYield] = 0;
		m_iYieldRateModifier[iYield] = 0;

		for (int iImprovement = 0; iImprovement < GC.getNumImprovementInfos(); iImprovement++)
		{
			m_ppaaiImprovementYieldChange[iImprovement] = yield;
		}
		for (int iSpecialist = 0; iSpecialist < GC.getNumSpecialistInfos(); iSpecialist++)
		{
			m_ppaaiSpecialistYieldChange[iSpecialist] = yield;
		}
	}

	for (int iTerrain = 0; iTerrain < GC.getNumTerrainInfos(); iTerrain++)
	{
		m_iStrategicResourceQuantityModifier[iTerrain] = 0;
	}

	m_aiResourceQuantityModifier.clear();
	m_aiResourceQuantityModifier.resize( GC.getNumResourceInfos() );

	for (int iResource = 0; iResource < GC.getNumResourceInfos(); iResource++)
	{
		m_aiResourceQuantityModifier[iResource] = 0;
	}

	m_aFreeTraitUnits.clear();

	int iNumUnitCombatClassInfos = GC.getNumUnitCombatClassInfos();
	CvAssertMsg((0 < iNumUnitCombatClassInfos),  "GC.getNumUnitCombatClassInfos() is not greater than zero but an array is being allocated in CvPlayerTraits::Reset");
	m_paiMovesChangeUnitCombat.clear(); m_paiMovesChangeUnitCombat.resize(iNumUnitCombatClassInfos);
	for (int iI = 0; iI < iNumUnitCombatClassInfos; iI++)
	{
		m_paiMovesChangeUnitCombat[iI] = 0;
	}
}

/// Does this player possess a specific trait?
bool CvPlayerTraits::HasTrait(TraitTypes eTrait) const
{
	CvAssert(m_pPlayer);

	if (m_pPlayer != NULL)
	{
		CvAssertMsg((m_pPlayer->getLeaderType() >= 0), "getLeaderType() is less than zero");
		CvAssertMsg((eTrait >= 0), "eTrait is less than zero");
		return m_pPlayer->getLeaderInfo().hasTrait(eTrait) && !m_pTraits->GetEntry(eTrait)->IsObsoleteByTech(m_pPlayer->getTeam());
	}
	else
	{
		return false;
	}
}

/// Bonus movement for this combat class
int CvPlayerTraits::GetMovesChangeUnitCombat(const int unitCombatID) const
{
	CvAssertMsg(unitCombatID < GC.getNumUnitCombatClassInfos(),  "Invalid unitCombatID parameter in call to CvPlayerTraits::GetMovesChangeUnitCombat()");

	if (unitCombatID == NO_UNITCLASS)
	{
		return 0;
	}

	return m_paiMovesChangeUnitCombat[unitCombatID];
}

/// Extra yield from this improvement
int CvPlayerTraits::GetImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYield) const
{
	CvAssertMsg(eImprovement < GC.getNumImprovementInfos(),  "Invalid eImprovement parameter in call to CvPlayerTraits::GetImprovementYieldChange()");
	CvAssertMsg(eYield < NUM_YIELD_TYPES,  "Invalid eYield parameter in call to CvPlayerTraits::GetImprovementYieldChange()");

	if (eImprovement == NO_IMPROVEMENT)
	{
		return 0;
	}

	return m_ppaaiImprovementYieldChange[(int)eImprovement][(int)eYield];
}

/// Extra yield from this specialist
int CvPlayerTraits::GetSpecialistYieldChange(SpecialistTypes eSpecialist, YieldTypes eYield) const
{
	CvAssertMsg(eSpecialist < GC.getNumSpecialistInfos(),  "Invalid eSpecialist parameter in call to CvPlayerTraits::GetSpecialistYieldChange()");
	CvAssertMsg(eYield < NUM_YIELD_TYPES,  "Invalid eYield parameter in call to CvPlayerTraits::GetSpecialistYieldChange()");

	if (eSpecialist == NO_SPECIALIST)
	{
		return 0;
	}

	return m_ppaaiSpecialistYieldChange[(int)eSpecialist][(int)eYield];
}

/// Do all new units get a specific promotion?
bool CvPlayerTraits::HasFreePromotionUnitCombat(const int promotionID, const int unitCombatID) const
{
	CvAssertMsg((promotionID >= 0), "promotionID is less than zero");
	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		const TraitTypes eTrait = static_cast<TraitTypes>(iI);
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(eTrait);
		if(pkTraitInfo)
		{
			if (HasTrait(eTrait))
			{
				if (pkTraitInfo->IsFreePromotionUnitCombat(promotionID, unitCombatID))
				{
					return true;
				}
			}
		}
	}

	return false;
}

/// Does a unit entering this tile cause a barbarian to convert to the player?
bool CvPlayerTraits::CheckForBarbarianConversion(CvPlot *pPlot)
{
	// Loop through all adjacent plots
	CvPlot* pAdjacentPlot;
	int iI;
	bool bRtnValue = false;

	if (pPlot->isWater() && GetSeaBarbarianConversionPercent() > 0)
	{
		for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

			if (pAdjacentPlot != NULL)
			{
				if (pAdjacentPlot->isWater())
				{
					UnitHandle pNavalUnit = pAdjacentPlot->getBestDefender(BARBARIAN_PLAYER);
					if (pNavalUnit)
					{
						if (ConvertBarbarianNavalUnit(pNavalUnit))
						{
							bRtnValue = true;
						}
					}
				}
			}
		}
	}

	else if (GetLandBarbarianConversionPercent() > 0 && pPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT() &&
		m_eCampGuardType != NO_UNIT)
	{
		bRtnValue = ConvertBarbarianCamp(pPlot);
	}

	return bRtnValue;
}

/// Discounted cost if building already present in capital
int CvPlayerTraits::GetCapitalBuildingDiscount(BuildingTypes eBuilding)
{
	if (m_iCapitalBuildingModifier > 0)
	{
		// Find this player's capital
		CvCity *pCapital = m_pPlayer->getCapitalCity();
		if (pCapital)
		{
			if (pCapital->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				return m_iCapitalBuildingModifier;
			}
		}
	}
	return 0;
}

/// First free unit received through traits
int CvPlayerTraits::GetFirstFreeUnit(TechTypes eTech)
{
	m_uiFreeUnitIndex = 0;
	m_eFreeUnitPrereqTech = eTech;

	while (m_uiFreeUnitIndex < m_aFreeTraitUnits.size())
	{
		if (eTech == m_aFreeTraitUnits[m_uiFreeUnitIndex].m_ePrereqTech)
		{
			return m_aFreeTraitUnits[m_uiFreeUnitIndex].m_iFreeUnit;
		}

		m_uiFreeUnitIndex++;
	}

	return NO_UNITCLASS;
}

/// Next free unit received through traits
int CvPlayerTraits::GetNextFreeUnit()
{
	m_uiFreeUnitIndex++;

	while (m_uiFreeUnitIndex < m_aFreeTraitUnits.size())
	{
		if (m_eFreeUnitPrereqTech == m_aFreeTraitUnits[m_uiFreeUnitIndex].m_ePrereqTech)
		{
			return m_aFreeTraitUnits[m_uiFreeUnitIndex].m_iFreeUnit;
		}

		m_uiFreeUnitIndex++;
	}

	return NO_UNITCLASS;
}

/// Serialization read
void CvPlayerTraits::Read(FDataStream& kStream)
{
	int iNumEntries;

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_iGreatPeopleRateModifier;
	if (uiVersion >= 5)
	{
		kStream >> m_iGreatScientistRateModifier;
	}
	else
	{
		m_iGreatScientistRateModifier = 0;
	}
	kStream >> m_iGreatGeneralRateModifier;
	kStream >> m_iGreatGeneralExtraBonus;
	kStream >> m_iLevelExperienceModifier;
	kStream >> m_iMaxGlobalBuildingProductionModifier;
	kStream >> m_iMaxTeamBuildingProductionModifier;
	kStream >> m_iMaxPlayerBuildingProductionModifier;
	kStream >> m_iCityUnhappinessModifier;
	kStream >> m_iPopulationUnhappinessModifier;
	kStream >> m_iCityStateBonusModifier;
	kStream >> m_iCityStateFriendshipModifier;
	if (uiVersion >= 7)
	{
		kStream >> m_iCityStateCombatModifier;
	}
	else
	{
		m_iCityStateCombatModifier = 0;
	}
	kStream >> m_iLandBarbarianConversionPercent;
	if (uiVersion >= 21)
	{
		kStream >> m_iLandBarbarianConversionExtraUnits;
	}
	else
	{
		m_iLandBarbarianConversionExtraUnits = 0;
	}
	kStream >> m_iSeaBarbarianConversionPercent;
	kStream >> m_iCapitalBuildingModifier;
	if (uiVersion >= 3)
	{
		kStream >> m_iPlotBuyCostModifier;
	}
	else
	{
		m_iPlotBuyCostModifier = 0;
	}
	kStream >> m_iPlotCultureCostModifier;
	kStream >> m_iCultureFromKills;
	kStream >> m_iCityCultureBonus;
	kStream >> m_iPolicyCostModifier;
	kStream >> m_iTradeRouteChange;
	kStream >> m_iWonderProductionModifier;
	kStream >> m_iPlunderModifier;

	if (uiVersion >= 12)
	{
		kStream >> m_iImprovementMaintenanceModifier;
	}
	else
	{
		m_iImprovementMaintenanceModifier = 0;
	}

	kStream >> m_iGoldenAgeDurationModifier;
	kStream >> m_iGoldenAgeMoveChange;
	kStream >> m_iGoldenAgeCombatModifier;

	if (uiVersion >= 2)
	{
		kStream >> m_iExtraEmbarkMoves;
	}
	else
	{
		m_iExtraEmbarkMoves = 0;
	}

	if (uiVersion >= 10)
	{
		kStream >> m_iNaturalWonderFirstFinderGold;

		if (uiVersion >= 11)
		{
			kStream >> m_iNaturalWonderSubsequentFinderGold;
		}
		else
		{
			m_iNaturalWonderSubsequentFinderGold = 0;
		}

		kStream >> m_iNaturalWonderYieldModifier;
		kStream >> m_iNaturalWonderHappinessModifier;
	}
	else
	{
		m_iNaturalWonderFirstFinderGold = 0;
		m_iNaturalWonderYieldModifier = 0;
		m_iNaturalWonderHappinessModifier = 0;
	}

	if (uiVersion >= 14)
	{
		kStream >> m_iNearbyImprovementCombatBonus;
		kStream >> m_iNearbyImprovementBonusRange;
	}
	else
	{
		m_iNearbyImprovementCombatBonus = 0;
		m_iNearbyImprovementBonusRange = 0;
	}

	if (uiVersion >= 16)
	{
		kStream >> m_iCultureBuildingYieldChange;
	}
	else
	{
		m_iCultureBuildingYieldChange = 0;
	}

	if (uiVersion >= 19)
	{
		kStream >> m_iLandUnitMaintenanceModifier;
	}
	else
	{
		m_iLandUnitMaintenanceModifier = 0;
	}

	if (uiVersion >= 17)
	{
		kStream >> m_iNavalUnitMaintenanceModifier;
	}
	else
	{
		m_iNavalUnitMaintenanceModifier = 0;
	}

	kStream >> m_bFightWellDamaged;
    kStream >> m_bMoveFriendlyWoodsAsRoad;
	kStream >> m_bFasterAlongRiver;

	if (uiVersion >= 12)
	{
		kStream >> m_bFasterInHills;
	}
	else
	{
		m_bFasterInHills = false;
	}

	kStream >> m_bEmbarkedNotCivilian;

	if (uiVersion >= 13)
	{
		kStream >> m_bEmbarkedAllWater;
	}
	else
	{
		m_bEmbarkedAllWater = false;
	}

	if (uiVersion >= 15)
	{
		kStream >> m_bEmbarkedToLandFlatCost;
	}
	else
	{
		m_bEmbarkedToLandFlatCost = false;
	}

	if (uiVersion >= 12)
	{
		kStream >> m_bNoHillsImprovementMaintenance;
	}
	else
	{
		m_bNoHillsImprovementMaintenance = false;
	}

	if (uiVersion >= 22)
	{
		kStream >> m_bTechBoostFromCapitalScienceBuildings;
		kStream >> m_bStaysAliveZeroCities;
	}
	else
	{
		m_bTechBoostFromCapitalScienceBuildings = false;
		m_bStaysAliveZeroCities = false;
	}

	kStream >> m_eCampGuardType;

	if (uiVersion >= 14)
	{
		kStream >> m_eCombatBonusImprovement;
	}
	else
	{
		m_eCombatBonusImprovement = NO_IMPROVEMENT;
	}

	ArrayWrapper<int> kExtraYieldThreshold(NUM_YIELD_TYPES, m_iExtraYieldThreshold);
	kStream >> kExtraYieldThreshold;

	ArrayWrapper<int> kFreeCityYield(NUM_YIELD_TYPES, m_iFreeCityYield);
	kStream >> kFreeCityYield;

	ArrayWrapper<int> kYieldChangeResourcesWrapper(NUM_YIELD_TYPES, m_iYieldChangeStrategicResources);
	kStream >> kYieldChangeResourcesWrapper;

	ArrayWrapper<int> kYieldRateModifierWrapper(NUM_YIELD_TYPES, m_iYieldRateModifier);
	kStream >> kYieldRateModifierWrapper;

	ArrayWrapper<int> kStrategicResourceQuantityModifierWrapper(GC.getNumTerrainInfos(), m_iStrategicResourceQuantityModifier);
	kStream >> kStrategicResourceQuantityModifierWrapper;

	if (uiVersion >= 18)
	{
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_aiResourceQuantityModifier);
	}
	else
	{
		if (uiVersion >= 4)
		{
			// Added version 4
			CvInfosSerializationHelper::ReadV0DataArray(kStream, &m_aiResourceQuantityModifier[0], m_aiResourceQuantityModifier.size(), CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0ResourceTags));
		}
		else
		{
			for (int iResource = 0; iResource < GC.getNumResourceInfos(); iResource++)
			{
				m_aiResourceQuantityModifier[iResource] = 0;
			}
		}
	}

	if (uiVersion >= 8)
	{
		kStream >> iNumEntries;
		m_aFreeTraitUnits.clear();
		for (int iI = 0; iI < iNumEntries; iI++)
		{
			FreeTraitUnit trait;
			kStream >> trait.m_iFreeUnit;
			kStream >> trait.m_ePrereqTech;
			m_aFreeTraitUnits.push_back(trait);
		}
	}
	else if (uiVersion >= 6)
	{
		kStream >> iNumEntries;
		m_aFreeTraitUnits.clear();
		for (int iI = 0; iI < iNumEntries; iI++)
		{
			FreeTraitUnit trait;
			kStream >> trait.m_iFreeUnit;
			trait.m_ePrereqTech = NO_TECH;
			m_aFreeTraitUnits.push_back(trait);
		}
	}
	else
	{
		m_aFreeTraitUnits.clear();
	}

	if (uiVersion >= 9)
	{
		kStream >> iNumEntries;
		for (int iI = 0; iI < iNumEntries; iI++)
		{
			kStream >> m_paiMovesChangeUnitCombat[iI];
		}
	}

	if (uiVersion >= 20)
	{
		kStream >> m_ppaaiImprovementYieldChange;
		kStream >> m_ppaaiSpecialistYieldChange;
	}
	else
	{
		m_ppaaiImprovementYieldChange.clear();
		m_ppaaiImprovementYieldChange.resize(GC.getNumImprovementInfos());
		m_ppaaiSpecialistYieldChange.clear();
		m_ppaaiSpecialistYieldChange.resize(GC.getNumSpecialistInfos());

		Firaxis::Array< int, NUM_YIELD_TYPES > yield;
		for( unsigned int j = 0; j < NUM_YIELD_TYPES; ++j )
		{
			yield[j] = 0;
		}

		for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
		{
			for (int iImprovement = 0; iImprovement < GC.getNumImprovementInfos(); iImprovement++)
			{
				m_ppaaiImprovementYieldChange[iImprovement] = yield;
			}
			for (int iSpecialist = 0; iSpecialist < GC.getNumSpecialistInfos(); iSpecialist++)
			{
				m_ppaaiSpecialistYieldChange[iSpecialist] = yield;
			}
		}
	}
}

/// Serialization write
void CvPlayerTraits::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 22;
	kStream << uiVersion;

	kStream << m_iGreatPeopleRateModifier;
	kStream << m_iGreatScientistRateModifier;
	kStream << m_iGreatGeneralRateModifier;
	kStream << m_iGreatGeneralExtraBonus;
	kStream << m_iLevelExperienceModifier;
	kStream << m_iMaxGlobalBuildingProductionModifier;
	kStream << m_iMaxTeamBuildingProductionModifier;
	kStream << m_iMaxPlayerBuildingProductionModifier;
	kStream << m_iCityUnhappinessModifier;
	kStream << m_iPopulationUnhappinessModifier;
	kStream << m_iCityStateBonusModifier;
	kStream << m_iCityStateFriendshipModifier;
	kStream << m_iCityStateCombatModifier;
	kStream << m_iLandBarbarianConversionPercent;
	kStream << m_iLandBarbarianConversionExtraUnits;
	kStream << m_iSeaBarbarianConversionPercent;
	kStream << m_iCapitalBuildingModifier;
	kStream << m_iPlotBuyCostModifier;
	kStream << m_iPlotCultureCostModifier;
	kStream << m_iCultureFromKills;
	kStream << m_iCityCultureBonus;
	kStream << m_iPolicyCostModifier;
	kStream << m_iTradeRouteChange;
	kStream << m_iWonderProductionModifier;
	kStream << m_iPlunderModifier;
	kStream << m_iImprovementMaintenanceModifier;
	kStream << m_iGoldenAgeDurationModifier;
	kStream << m_iGoldenAgeMoveChange;
	kStream << m_iGoldenAgeCombatModifier;
	kStream << m_iExtraEmbarkMoves;
	kStream << m_iNaturalWonderFirstFinderGold;
	kStream << m_iNaturalWonderSubsequentFinderGold;
	kStream << m_iNaturalWonderYieldModifier;
	kStream << m_iNaturalWonderHappinessModifier;
	kStream << m_iNearbyImprovementCombatBonus;
	kStream << m_iNearbyImprovementBonusRange;
	kStream << m_iCultureBuildingYieldChange;
	kStream << m_iLandUnitMaintenanceModifier;
	kStream << m_iNavalUnitMaintenanceModifier;

	kStream << m_bFightWellDamaged;
	kStream << m_bMoveFriendlyWoodsAsRoad;
	kStream << m_bFasterAlongRiver;
	kStream << m_bFasterInHills;
	kStream << m_bEmbarkedNotCivilian;
	kStream << m_bEmbarkedAllWater;
	kStream << m_bEmbarkedToLandFlatCost;
	kStream << m_bNoHillsImprovementMaintenance;
	kStream << m_bTechBoostFromCapitalScienceBuildings;
	kStream << m_bStaysAliveZeroCities;

	kStream << m_eCampGuardType;
	kStream << m_eCombatBonusImprovement;

	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iExtraYieldThreshold);
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iFreeCityYield);
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iYieldChangeStrategicResources);
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iYieldRateModifier);
	kStream << ArrayWrapper<int>(GC.getNumTerrainInfos(), m_iStrategicResourceQuantityModifier);
	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes>(kStream, m_aiResourceQuantityModifier);  // Added version 4

	kStream << m_aFreeTraitUnits.size();
	for (uint ui = 0; ui < m_aFreeTraitUnits.size(); ui++)
	{
		kStream << m_aFreeTraitUnits[ui].m_iFreeUnit;
		kStream << m_aFreeTraitUnits[ui].m_ePrereqTech;
	}

	int iNumUnitCombatClassInfos = GC.getNumUnitCombatClassInfos();
	kStream << 	iNumUnitCombatClassInfos;
	for (int iI = 0; iI < iNumUnitCombatClassInfos; iI++)
	{
		kStream << m_paiMovesChangeUnitCombat[iI];
	}

	kStream << m_ppaaiImprovementYieldChange;
	kStream << m_ppaaiSpecialistYieldChange;
}

// PRIVATE METHODS

/// Is there an adjacent barbarian camp that could be converted?
bool CvPlayerTraits::ConvertBarbarianCamp(CvPlot *pPlot)
{
	UnitHandle pGiftUnit;

	// Has this camp already decided not to convert?
	if (pPlot->IsBarbarianCampNotConverting())
	{
		return false;
	}

	// Roll die to see if it converts
	if (GC.getGame().getJonRandNum(100, "Barbarian Camp Conversion") < m_iLandBarbarianConversionPercent)
	{
		pPlot->setImprovementType(NO_IMPROVEMENT);

		int iNumGold = /*25*/ GC.getGOLD_FROM_BARBARIAN_CONVERSION();
		m_pPlayer->GetTreasury()->ChangeGold(iNumGold);

		// Set who last cleared the camp here
		pPlot->SetPlayerThatClearedBarbCampHere(m_pPlayer->GetID());

		// Convert the barbarian into our unit
		FAssertMsg(m_eCampGuardType < GC.getNumUnitInfos(), "Illegal camp guard unit type");
		pGiftUnit = m_pPlayer->initUnit(m_eCampGuardType, pPlot->getX(), pPlot->getY(), NO_UNITAI, NO_DIRECTION, true /*bNoMove*/);
		if (!pGiftUnit->jumpToNearestValidPlot())
			pGiftUnit->kill(false);
		else
			pGiftUnit->finishMoves();

		// Convert any extra units
		for (int iI = 0; iI < m_iLandBarbarianConversionExtraUnits; iI++)
		{
			pGiftUnit = m_pPlayer->initUnit(m_eCampGuardType, pPlot->getX(), pPlot->getY(), NO_UNITAI, NO_DIRECTION, true /*bNoMove*/);
			if (!pGiftUnit->jumpToNearestValidPlot())
				pGiftUnit->kill(false);
			else
				pGiftUnit->finishMoves();
		}

		if (GC.getLogging() && GC.getAILogging())
		{
			CvString logMsg;
			logMsg.Format("Converted barbarian camp, X: %d, Y: %d", pPlot->getX(), pPlot->getY());
			m_pPlayer->GetHomelandAI()->LogHomelandMessage(logMsg);
		}

		CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BARB_CAMP_CONVERTS");
		CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BARB_CAMP_CONVERTS");
		m_pPlayer->GetNotifications()->Add(NOTIFICATION_GENERIC, strBuffer, strSummary, pPlot->getX(), pPlot->getY(), -1);
		//Increase Stat
		if(m_pPlayer->isHuman() &&!GC.getGame().isGameMultiPlayer())
		{
			gDLL->IncrementSteamStatAndUnlock(ESTEAMSTAT_BARBSCONVERTED, 10, ACHIEVEMENT_SPECIAL_BARBARIANWARLORD );
		}
	}

	// Decided not to
	else
	{
		pPlot->SetBarbarianCampNotConverting(true);
		return false;
	}

	return true;
}

/// Is there an adjacent barbarian naval unit that could be converted?
bool CvPlayerTraits::ConvertBarbarianNavalUnit(UnitHandle pUnit)
{
	UnitHandle pGiftUnit;

	// Has this unit already decided not to convert?
	if (pUnit->IsNotConverting())
	{
		return false;
	}

	// Roll die to see if it converts
	if (GC.getGame().getJonRandNum(100, "Barbarian Naval Unit Conversion") < m_iSeaBarbarianConversionPercent)
	{
		int iNumGold = /*25*/ GC.getGOLD_FROM_BARBARIAN_CONVERSION();
		m_pPlayer->GetTreasury()->ChangeGold(iNumGold);

		// Convert the barbarian into our unit
		pGiftUnit = m_pPlayer->initUnit(pUnit->getUnitType(), pUnit->getX(), pUnit->getY(), pUnit->AI_getUnitAIType(), NO_DIRECTION, true /*bNoMove*/, false);
		CvAssertMsg(pGiftUnit, "GiftUnit is not assigned a valid value");
		pGiftUnit->convert(pUnit.pointer());
		pGiftUnit->setupGraphical();
		pGiftUnit->finishMoves(); // No move first turn

        // Validate that the achievement is reached by a live human and active player at the same time
        if (m_pPlayer->isHuman() && !GC.getGame().isGameMultiPlayer() && m_pPlayer->getLeaderInfo().GetType() && _stricmp(m_pPlayer->getLeaderInfo().GetType(), "LEADER_SULEIMAN") == 0)
        {
		    gDLL->IncrementSteamStatAndUnlock( ESTEAMSTAT_BARBSNAVALCONVERTED, 10, ACHIEVEMENT_SPECIAL_BARBARYPIRATE );
        }

		if (GC.getLogging() && GC.getAILogging())
		{
			CvString logMsg;
			logMsg.Format("Converted barbarian naval unit, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
			m_pPlayer->GetHomelandAI()->LogHomelandMessage(logMsg);
		}

		CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BARB_NAVAL_UNIT_CONVERTS");
		CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BARB_NAVAL_UNIT_CONVERTS");
		m_pPlayer->GetNotifications()->Add(NOTIFICATION_GENERIC, strBuffer, strSummary, pUnit->getX(), pUnit->getY(), -1);
		return true;
	}

	// Decided not to
	else
	{
		pUnit->SetNotConverting(true);
		return false;
	}
}
