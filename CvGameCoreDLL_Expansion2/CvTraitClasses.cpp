/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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
#include "CvDiplomacyAI.h"
#include "CvGrandStrategyAI.h"

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
	m_iGreatPersonGiftInfluence(0),
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
#if defined(MOD_TRAITS_CITY_WORKING)
	m_iCityWorkingChange(0),
#endif
	m_iPlotCultureCostModifier(0),
	m_iCultureFromKills(0),
	m_iFaithFromKills(0),
	m_iCityCultureBonus(0),
	m_iCapitalThemingBonusModifier(0),
	m_iPolicyCostModifier(0),
	m_iCityConnectionTradeRouteChange(0),
	m_iWonderProductionModifier(0),
	m_iPlunderModifier(0),
	m_iImprovementMaintenanceModifier(0),
	m_iGoldenAgeDurationModifier(0),
	m_iGoldenAgeMoveChange(0),
	m_iGoldenAgeCombatModifier(0),
	m_iGoldenAgeTourismModifier(0),
	m_iGoldenAgeGreatArtistRateModifier(0),
	m_iGoldenAgeGreatMusicianRateModifier(0),
	m_iGoldenAgeGreatWriterRateModifier(0),
	m_iObsoleteTech(NO_TECH),
	m_iPrereqTech(NO_TECH),
	m_iExtraEmbarkMoves(0),
	m_iFreeUnitClassType(NO_UNITCLASS),
	m_iNaturalWonderFirstFinderGold(0),
	m_iNaturalWonderSubsequentFinderGold(0),
	m_iNaturalWonderYieldModifier(0),
	m_iNaturalWonderHappinessModifier(0),
	m_iNearbyImprovementCombatBonus(0),
	m_iNearbyImprovementBonusRange(0),
	m_iCultureBuildingYieldChange(0),
	m_iCombatBonusVsHigherTech(0),
	m_iCombatBonusVsLargerCiv(0),
	m_iLandUnitMaintenanceModifier(0),
	m_iNavalUnitMaintenanceModifier(0),
	m_iRazeSpeedModifier(0),
	m_iDOFGreatPersonModifier(0),
	m_iLuxuryHappinessRetention(0),
	m_iExtraSpies(0),
	m_iUnresearchedTechBonusFromKills(0),
	m_iExtraFoundedCityTerritoryClaimRange(0),
	m_iFreeSocialPoliciesPerEra(0),
	m_iNumTradeRoutesModifier(0),
	m_iTradeRouteResourceModifier(0),
	m_iUniqueLuxuryCities(0),
	m_iUniqueLuxuryQuantity(0),
#if defined(MOD_BUGFIX_MINOR)
	m_iWorkerSpeedModifier(0),
	m_iAfraidMinorPerTurnInfluence(0),
	m_iLandTradeRouteRangeBonus(0),
#endif
#if defined(MOD_TRAITS_TRADE_ROUTE_BONUSES)
	m_iSeaTradeRouteRangeBonus(0),
#endif
#if defined(MOD_BUGFIX_MINOR)
	m_iTradeReligionModifier(0),
	m_iTradeBuildingModifier(0),
#endif

	m_eFreeUnitPrereqTech(NO_TECH),
	m_eFreeBuilding(NO_BUILDING),
#if defined(MOD_BALANCE_CORE)
	m_eFreeCapitalBuilding(NO_BUILDING),
	m_eFreeBuildingPrereqTech(NO_TECH),
	m_eCapitalFreeBuildingPrereqTech(NO_TECH),
#endif
	m_eFreeBuildingOnConquest(NO_BUILDING),

	m_bFightWellDamaged(false),
	m_bMoveFriendlyWoodsAsRoad(false),
	m_bFasterAlongRiver(false),
	m_bFasterInHills(false),
	m_bEmbarkedAllWater(false),
	m_bEmbarkedToLandFlatCost(false),
	m_bNoHillsImprovementMaintenance(false),
	m_bTechBoostFromCapitalScienceBuildings(false),
	m_bStaysAliveZeroCities(false),
	m_bFaithFromUnimprovedForest(false),
#if defined(MOD_TRAITS_ANY_BELIEF)
	m_bAnyBelief(false),
#endif
	m_bBonusReligiousBelief(false),
	m_bAbleToAnnexCityStates(false),
	m_bCrossesMountainsAfterGreatGeneral(false),
#if defined(MOD_TRAITS_CROSSES_ICE)
	m_bCrossesIce(false),
#endif
	m_bMayaCalendarBonuses(false),
	m_bNoAnnexing(false),
	m_bTechFromCityConquer(false),
	m_bUniqueLuxuryRequiresNewArea(false),

	m_paiExtraYieldThreshold(NULL),
	m_paiYieldChange(NULL),
	m_paiYieldChangeStrategicResources(NULL),
	m_paiYieldChangeNaturalWonder(NULL),
	m_paiYieldChangePerTradePartner(NULL),
	m_paiYieldChangeIncomingTradeRoute(NULL),
	m_paiYieldModifier(NULL),
	m_piStrategicResourceQuantityModifier(NULL),
	m_piResourceQuantityModifiers(NULL),
	m_ppiImprovementYieldChanges(NULL),
	m_ppiSpecialistYieldChanges(NULL),
	m_ppiUnimprovedFeatureYieldChanges(NULL)
{
}

/// Destructor
CvTraitEntry::~CvTraitEntry()
{
	CvDatabaseUtility::SafeDelete2DArray(m_ppiImprovementYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiSpecialistYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiUnimprovedFeatureYieldChanges);
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

/// Accessor:: Influence gained for gifting GP to city state
int CvTraitEntry::GetGreatPersonGiftInfluence() const
{
	return m_iGreatPersonGiftInfluence;
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

#if defined(MOD_TRAITS_CITY_WORKING)
/// Accessor:: greater border expansion
int CvTraitEntry::GetCityWorkingChange() const
{
	return m_iCityWorkingChange;
}
#endif

/// Accessor:: increased rate of culture border expansion
int CvTraitEntry::GetPlotCultureCostModifier() const
{
	return m_iPlotCultureCostModifier;
}

/// Accessor:: culture for kills
int CvTraitEntry::GetCultureFromKills() const
{
	return m_iCultureFromKills;
}

/// Accessor:: faith for kills
int CvTraitEntry::GetFaithFromKills() const
{
	return m_iFaithFromKills;
}

/// Accessor:: extra culture from buildings that provide culture
int CvTraitEntry::GetCityCultureBonus() const
{
	return m_iCityCultureBonus;
}

/// Accessor:: boost to theming bonuses in capital
int CvTraitEntry::GetCapitalThemingBonusModifier() const
{
	return m_iCapitalThemingBonusModifier;
}

/// Accessor:: discount on buying policies
int CvTraitEntry::GetPolicyCostModifier() const
{
	return m_iPolicyCostModifier;
}

/// Accessor:: extra money from trade routes
int CvTraitEntry::GetCityConnectionTradeRouteChange() const
{
	return m_iCityConnectionTradeRouteChange;
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

/// Accessor:: tourism bonus during golden ages
int CvTraitEntry::GetGoldenAgeTourismModifier() const
{
	return m_iGoldenAgeTourismModifier;
}

/// Accessor:: artist bonus during golden ages
int CvTraitEntry::GetGoldenAgeGreatArtistRateModifier() const
{
	return m_iGoldenAgeGreatArtistRateModifier;
}

/// Accessor:: musician bonus during golden ages
int CvTraitEntry::GetGoldenAgeGreatMusicianRateModifier() const
{
	return m_iGoldenAgeGreatMusicianRateModifier;
}

/// Accessor:: writer bonus during golden ages
int CvTraitEntry::GetGoldenAgeGreatWriterRateModifier() const
{
	return m_iGoldenAgeGreatWriterRateModifier;
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

/// Accessor: combat bonus in own territory vs. higher tech units
int CvTraitEntry::GetCombatBonusVsHigherTech() const
{
	return m_iCombatBonusVsHigherTech;
}

/// Accessor: combat bonus vs. civ with more cities
int CvTraitEntry::GetCombatBonusVsLargerCiv() const
{
	return m_iCombatBonusVsLargerCiv;
}

/// Accessor: change to speed razing cities
int CvTraitEntry::GetRazeSpeedModifier() const
{
	return m_iRazeSpeedModifier;
}

/// Accessor: great person boost from friendship
int CvTraitEntry::GetDOFGreatPersonModifier() const
{
	return m_iDOFGreatPersonModifier;
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

/// Accessor: percentage of happiness retained from luxuries traded away
int CvTraitEntry::GetLuxuryHappinessRetention() const
{
	return m_iLuxuryHappinessRetention;
}

/// Accessor: number of extra spies
int CvTraitEntry::GetExtraSpies() const
{
	return m_iExtraSpies;
}

/// Accessor: modifier for beaker bonus towards unresearched tech upon killing unit from that tech
int CvTraitEntry::GetUnresearchedTechBonusFromKills() const
{
	return m_iUnresearchedTechBonusFromKills;
}

int CvTraitEntry::GetExtraFoundedCityTerritoryClaimRange() const
{
	return m_iExtraFoundedCityTerritoryClaimRange;
}

/// Accessor: extra social policy from advancing to the next age
int CvTraitEntry::GetFreeSocialPoliciesPerEra() const
{
	return m_iFreeSocialPoliciesPerEra;
}

int CvTraitEntry::GetNumTradeRoutesModifier() const
{
	return m_iNumTradeRoutesModifier;
}

int CvTraitEntry::GetTradeRouteResourceModifier() const
{
	return m_iTradeRouteResourceModifier;
}

int CvTraitEntry::GetUniqueLuxuryCities() const
{
	return m_iUniqueLuxuryCities;
}

int CvTraitEntry::GetUniqueLuxuryQuantity() const
{
	return m_iUniqueLuxuryQuantity;
}

int CvTraitEntry::GetWorkerSpeedModifier() const
{
	return m_iWorkerSpeedModifier;
}

int CvTraitEntry::GetAfraidMinorPerTurnInfluence() const
{
	return m_iAfraidMinorPerTurnInfluence;
}

int CvTraitEntry::GetLandTradeRouteRangeBonus() const
{
	return m_iLandTradeRouteRangeBonus;
}

#if defined(MOD_TRAITS_TRADE_ROUTE_BONUSES)
int CvTraitEntry::GetSeaTradeRouteRangeBonus() const
{
	return m_iSeaTradeRouteRangeBonus;
}
#endif

int CvTraitEntry::GetTradeReligionModifier() const
{
	return m_iTradeReligionModifier;
}

int CvTraitEntry::GetTradeBuildingModifier() const
{
	return m_iTradeBuildingModifier;
}
#if defined(MOD_BALANCE_CORE)
int CvTraitEntry::GetNumFreeBuildings() const
{
	return m_iNumFreeBuildings;
}
TechTypes CvTraitEntry::GetFreeBuildingPrereqTech() const
{
	return m_eFreeBuildingPrereqTech;
}
TechTypes CvTraitEntry::GetCapitalFreeBuildingPrereqTech() const
{
	return m_eCapitalFreeBuildingPrereqTech;
}
#endif

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

/// Accessor: free building in each city
BuildingTypes CvTraitEntry::GetFreeBuilding() const
{
	return m_eFreeBuilding;
}
#if defined(MOD_BALANCE_CORE)
/// Does the capital get a free building?
BuildingTypes CvTraitEntry::GetFreeCapitalBuilding() const
{
	return m_eFreeCapitalBuilding;
}
#endif
/// Accessor: free building in each city conquered
BuildingTypes CvTraitEntry::GetFreeBuildingOnConquest() const
{
	return m_eFreeBuildingOnConquest;
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

/// Accessor: does this civ get Faith from settling cities near Forest?
bool CvTraitEntry::IsFaithFromUnimprovedForest() const
{
	return m_bFaithFromUnimprovedForest;
}

#if defined(MOD_TRAITS_ANY_BELIEF)
/// Accessor: can this civ have any belief?
bool CvTraitEntry::IsAnyBelief() const
{
	return m_bAnyBelief;
}
#endif

/// Accessor: does this civ get a bonus religious belief?
bool CvTraitEntry::IsBonusReligiousBelief() const
{
	return m_bBonusReligiousBelief;
}

/// Accessor: does this civ have the ability to use gold to annex city states?
bool CvTraitEntry::IsAbleToAnnexCityStates() const
{
	return m_bAbleToAnnexCityStates;
}

/// Accessor: do combat units have the ability to cross mountains after a great general is born?
bool CvTraitEntry::IsCrossesMountainsAfterGreatGeneral() const
{
	return m_bCrossesMountainsAfterGreatGeneral;
}

#if defined(MOD_TRAITS_CROSSES_ICE)
/// Accessor: do combat units have the ability to cross ice?
bool CvTraitEntry::IsCrossesIce() const
{
	return m_bCrossesIce;
}
#endif

/// Accessor: is this civ receiving bonuses based on the Maya calendar?
bool CvTraitEntry::IsMayaCalendarBonuses() const
{
	return m_bMayaCalendarBonuses;
}

/// Accessor: is this civ prevented from annexing cities?
bool CvTraitEntry::IsNoAnnexing() const
{
	return m_bNoAnnexing;
}

/// Accessor: does this civ get a tech from conquering a city?
bool CvTraitEntry::IsTechFromCityConquer() const
{
	return m_bTechFromCityConquer;
}

/// Accessor: do unique luxuries only get added into cities founded on new continents?
bool CvTraitEntry::IsUniqueLuxuryRequiresNewArea() const
{
	return m_bUniqueLuxuryRequiresNewArea;
}

/// Accessor: Do rivers count as trade roads
bool CvTraitEntry::IsRiverTradeRoad() const
{
	return m_bRiverTradeRoad;
}

/// Accessor: Do our units not anger City-States when they intrude in their lands?
bool CvTraitEntry::IsAngerFreeIntrusionOfCityStates() const
{
	return m_bAngerFreeIntrusionOfCityStates;
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

/// Accessor:: Extra yield from natural wonders
int CvTraitEntry::GetYieldChangeNaturalWonder(int i) const
{
	return m_paiYieldChangeNaturalWonder ? m_paiYieldChangeNaturalWonder[i] : -1;
}

/// Accessor:: Extra yield from trade partners
int CvTraitEntry::GetYieldChangePerTradePartner(int i) const
{
	return m_paiYieldChangePerTradePartner ? m_paiYieldChangePerTradePartner[i] : -1;
}

/// Accessor:: Extra yield from incoming trade routes
int CvTraitEntry::GetYieldChangeIncomingTradeRoute(int i) const
{
	return m_paiYieldChangeIncomingTradeRoute ? m_paiYieldChangeIncomingTradeRoute[i] : -1;
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

/// Accessor:: Extra yield from an unimproved feature
int CvTraitEntry::GetUnimprovedFeatureYieldChanges(FeatureTypes eIndex1, YieldTypes eIndex2) const
{
	CvAssertMsg(eIndex1 < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(eIndex1 > -1, "Index out of bounds");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(eIndex2 > -1, "Index out of bounds");
	return m_ppiUnimprovedFeatureYieldChanges ? m_ppiUnimprovedFeatureYieldChanges[eIndex1][eIndex2] : 0;
}

/// Accessor:: Additional moves for a class of combat unit
int CvTraitEntry::GetMovesChangeUnitCombat(const int unitCombatID) const
{
	CvAssertMsg((unitCombatID >= 0), "unitCombatID is less than zero");
	CvAssertMsg((unitCombatID < GC.getNumUnitCombatClassInfos()), "unitCombatID exceeds number of combat classes");

	return m_piMovesChangeUnitCombats[unitCombatID];
}

/// Accessor:: Maintenance Modifier for a class of combat unit
int CvTraitEntry::GetMaintenanceModifierUnitCombat(const int unitCombatID) const
{
	CvAssertMsg((unitCombatID >= 0), "unitCombatID is less than zero");
	CvAssertMsg((unitCombatID < GC.getNumUnitCombatClassInfos()), "unitCombatID exceeds number of combat classes");

	return m_piMaintenanceModifierUnitCombats[unitCombatID];
}

/// Does this trait provide free resources in the first X cities?
FreeResourceXCities CvTraitEntry::GetFreeResourceXCities(ResourceTypes eResource) const
{
	return m_aFreeResourceXCities[(int)eResource];
}

/// Tech that makes this trait obsolete
int CvTraitEntry::GetObsoleteTech() const
{
	return m_iObsoleteTech;
}

/// Tech that enables Maya calendar bonuses
int CvTraitEntry::GetPrereqTech() const
{
	return m_iPrereqTech;
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

/// Has this trait become obsolete?
bool CvTraitEntry::IsObsoleteByTech(TeamTypes eTeam)
{
	if(m_iObsoleteTech != NO_TECH)
	{
		if(GET_TEAM(eTeam).GetTeamTechs()->HasTech((TechTypes)m_iObsoleteTech))
		{
			return true;
		}
	}
	return false;
}

/// Is this trait enabled by tech?
bool CvTraitEntry::IsEnabledByTech(TeamTypes eTeam)
{
	if(m_iPrereqTech != NO_TECH)
	{
		if(GET_TEAM(eTeam).GetTeamTechs()->HasTech((TechTypes)m_iPrereqTech))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool CvTraitEntry::NoTrain(UnitClassTypes eUnitClass)
{
	if (eUnitClass != NO_UNITCLASS)
	{
		return m_abNoTrainUnitClass[eUnitClass];
	}
	else
	{
		return false;
	}
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
	m_iGreatPersonGiftInfluence				= kResults.GetInt("GreatPersonGiftInfluence");
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
#if defined(MOD_TRAITS_CITY_WORKING)
	m_iCityWorkingChange					= kResults.GetInt("CityWorkingChange");
#endif
	m_iPlotCultureCostModifier              = kResults.GetInt("PlotCultureCostModifier");
	m_iCultureFromKills						= kResults.GetInt("CultureFromKills");
	m_iFaithFromKills						= kResults.GetInt("FaithFromKills");
	m_iCityCultureBonus						= kResults.GetInt("CityCultureBonus");
	m_iCapitalThemingBonusModifier          = kResults.GetInt("CapitalThemingBonusModifier");
	m_iPolicyCostModifier					= kResults.GetInt("PolicyCostModifier");
	m_iCityConnectionTradeRouteChange		= kResults.GetInt("CityConnectionTradeRouteChange");
	m_iWonderProductionModifier				= kResults.GetInt("WonderProductionModifier");
	m_iPlunderModifier						= kResults.GetInt("PlunderModifier");
	m_iImprovementMaintenanceModifier       = kResults.GetInt("ImprovementMaintenanceModifier");
	m_iGoldenAgeDurationModifier			= kResults.GetInt("GoldenAgeDurationModifier");
	m_iGoldenAgeMoveChange				    = kResults.GetInt("GoldenAgeMoveChange");
	m_iGoldenAgeCombatModifier				= kResults.GetInt("GoldenAgeCombatModifier");
	m_iGoldenAgeTourismModifier				= kResults.GetInt("GoldenAgeTourismModifier");
	m_iGoldenAgeGreatArtistRateModifier		= kResults.GetInt("GoldenAgeGreatArtistRateModifier");
	m_iGoldenAgeGreatMusicianRateModifier	= kResults.GetInt("GoldenAgeGreatMusicianRateModifier");
	m_iGoldenAgeGreatWriterRateModifier		= kResults.GetInt("GoldenAgeGreatWriterRateModifier");
	m_iExtraEmbarkMoves						= kResults.GetInt("ExtraEmbarkMoves");
	m_iNaturalWonderFirstFinderGold         = kResults.GetInt("NaturalWonderFirstFinderGold");
	m_iNaturalWonderSubsequentFinderGold    = kResults.GetInt("NaturalWonderSubsequentFinderGold");
	m_iNaturalWonderYieldModifier           = kResults.GetInt("NaturalWonderYieldModifier");
	m_iNaturalWonderHappinessModifier       = kResults.GetInt("NaturalWonderHappinessModifier");
	m_iNearbyImprovementCombatBonus			= kResults.GetInt("NearbyImprovementCombatBonus");
	m_iNearbyImprovementBonusRange			= kResults.GetInt("NearbyImprovementBonusRange");
	m_iCultureBuildingYieldChange			= kResults.GetInt("CultureBuildingYieldChange");
	m_iCombatBonusVsHigherTech				= kResults.GetInt("CombatBonusVsHigherTech");
	m_iCombatBonusVsLargerCiv				= kResults.GetInt("CombatBonusVsLargerCiv");
	m_iLandUnitMaintenanceModifier          = kResults.GetInt("LandUnitMaintenanceModifier");
	m_iNavalUnitMaintenanceModifier         = kResults.GetInt("NavalUnitMaintenanceModifier");
	m_iRazeSpeedModifier					= kResults.GetInt("RazeSpeedModifier");
	m_iDOFGreatPersonModifier				= kResults.GetInt("DOFGreatPersonModifier");
	m_iLuxuryHappinessRetention				= kResults.GetInt("LuxuryHappinessRetention");
	m_iExtraSpies							= kResults.GetInt("ExtraSpies");
	m_iUnresearchedTechBonusFromKills		= kResults.GetInt("UnresearchedTechBonusFromKills");
	m_iExtraFoundedCityTerritoryClaimRange  = kResults.GetInt("ExtraFoundedCityTerritoryClaimRange");
	m_iFreeSocialPoliciesPerEra				= kResults.GetInt("FreeSocialPoliciesPerEra");
	m_iNumTradeRoutesModifier				= kResults.GetInt("NumTradeRoutesModifier");
	m_iTradeRouteResourceModifier			= kResults.GetInt("TradeRouteResourceModifier");
	m_iUniqueLuxuryCities					= kResults.GetInt("UniqueLuxuryCities");
	m_iUniqueLuxuryQuantity					= kResults.GetInt("UniqueLuxuryQuantity");
	m_iWorkerSpeedModifier					= kResults.GetInt("WorkerSpeedModifier");
	m_iAfraidMinorPerTurnInfluence			= kResults.GetInt("AfraidMinorPerTurnInfluence");
	m_iLandTradeRouteRangeBonus				= kResults.GetInt("LandTradeRouteRangeBonus");
#if defined(MOD_TRAITS_TRADE_ROUTE_BONUSES)
	if (MOD_TRAITS_TRADE_ROUTE_BONUSES) {
		m_iSeaTradeRouteRangeBonus			= kResults.GetInt("SeaTradeRouteRangeBonus");
	}
#endif
	m_iTradeReligionModifier				= kResults.GetInt("TradeReligionModifier");
	m_iTradeBuildingModifier				= kResults.GetInt("TradeBuildingModifier");

#if defined(MOD_BALANCE_CORE)
	m_iNumFreeBuildings						= kResults.GetInt("NumFreeBuildings");
#endif
	const char* szTextVal = NULL;
	szTextVal = kResults.GetText("FreeUnit");
	if(szTextVal)
	{
		m_iFreeUnitClassType = GC.getInfoTypeForString(szTextVal, true);
	}

	szTextVal = kResults.GetText("FreeUnitPrereqTech");
	if(szTextVal)
	{
		m_eFreeUnitPrereqTech = (TechTypes)GC.getInfoTypeForString(szTextVal, true);
	}

#if defined(MOD_BALANCE_CORE)
	szTextVal = kResults.GetText("FreeBuildingPrereqTech");
	if(szTextVal)
	{
		m_eFreeBuildingPrereqTech = (TechTypes)GC.getInfoTypeForString(szTextVal, true);
	}
	szTextVal = kResults.GetText("CapitalFreeBuildingPrereqTech");
	if(szTextVal)
	{
		m_eCapitalFreeBuildingPrereqTech = (TechTypes)GC.getInfoTypeForString(szTextVal, true);
	}
#endif

	szTextVal = kResults.GetText("CombatBonusImprovement");
	if(szTextVal)
	{
		m_eCombatBonusImprovement = (ImprovementTypes)GC.getInfoTypeForString(szTextVal, true);
	}

	szTextVal = kResults.GetText("ObsoleteTech");
	if(szTextVal)
	{
		m_iObsoleteTech = GC.getInfoTypeForString(szTextVal, true);
	}

	szTextVal = kResults.GetText("PrereqTech");
	if(szTextVal)
	{
		m_iPrereqTech = GC.getInfoTypeForString(szTextVal, true);
	}

	szTextVal = kResults.GetText("FreeBuilding");
	if(szTextVal)
	{
		m_eFreeBuilding = (BuildingTypes)GC.getInfoTypeForString(szTextVal, true);
	}
#if defined(MOD_BALANCE_CORE)
	szTextVal = kResults.GetText("FreeCapitalBuilding");
	if(szTextVal)
	{
		m_eFreeCapitalBuilding = (BuildingTypes)GC.getInfoTypeForString(szTextVal, true);
	}
#endif

	szTextVal = kResults.GetText("FreeBuildingOnConquest");
	if(szTextVal)
	{
		m_eFreeBuildingOnConquest = (BuildingTypes)GC.getInfoTypeForString(szTextVal, true);
	}
	m_bFightWellDamaged = kResults.GetBool("FightWellDamaged");
	m_bMoveFriendlyWoodsAsRoad = kResults.GetBool("MoveFriendlyWoodsAsRoad");
	m_bFasterAlongRiver = kResults.GetBool("FasterAlongRiver");
	m_bFasterInHills = kResults.GetBool("FasterInHills");
	m_bEmbarkedAllWater = kResults.GetBool("EmbarkedAllWater");
	m_bEmbarkedToLandFlatCost = kResults.GetBool("EmbarkedToLandFlatCost");
	m_bNoHillsImprovementMaintenance = kResults.GetBool("NoHillsImprovementMaintenance");
	m_bTechBoostFromCapitalScienceBuildings = kResults.GetBool("TechBoostFromCapitalScienceBuildings");
	m_bStaysAliveZeroCities = kResults.GetBool("StaysAliveZeroCities");
	m_bFaithFromUnimprovedForest = kResults.GetBool("FaithFromUnimprovedForest");
#if defined(MOD_TRAITS_ANY_BELIEF)
	if (MOD_TRAITS_ANY_BELIEF) {
		m_bAnyBelief = kResults.GetBool("AnyBelief");
	}
#endif
	m_bBonusReligiousBelief = kResults.GetBool("BonusReligiousBelief");
	m_bAbleToAnnexCityStates = kResults.GetBool("AbleToAnnexCityStates");
	m_bCrossesMountainsAfterGreatGeneral = kResults.GetBool("CrossesMountainsAfterGreatGeneral");
#if defined(MOD_TRAITS_CROSSES_ICE)
	m_bCrossesIce = kResults.GetBool("CrossesIce");
#endif
	m_bMayaCalendarBonuses = kResults.GetBool("MayaCalendarBonuses");
	m_bNoAnnexing = kResults.GetBool("NoAnnexing");
	m_bTechFromCityConquer = kResults.GetBool("TechFromCityConquer");
	m_bUniqueLuxuryRequiresNewArea = kResults.GetBool("UniqueLuxuryRequiresNewArea");
	m_bRiverTradeRoad = kResults.GetBool("RiverTradeRoad");
	m_bAngerFreeIntrusionOfCityStates = kResults.GetBool("AngerFreeIntrusionOfCityStates");

	//Arrays
	const char* szTraitType = GetType();
	kUtility.SetYields(m_paiExtraYieldThreshold, "Trait_ExtraYieldThresholds", "TraitType", szTraitType);

	kUtility.SetYields(m_paiYieldChange, "Trait_YieldChanges", "TraitType", szTraitType);
	kUtility.SetYields(m_paiYieldChangeStrategicResources, "Trait_YieldChangesStrategicResources", "TraitType", szTraitType);
	kUtility.SetYields(m_paiYieldChangeNaturalWonder, "Trait_YieldChangesNaturalWonder", "TraitType", szTraitType);
	kUtility.SetYields(m_paiYieldChangePerTradePartner, "Trait_YieldChangesPerTradePartner", "TraitType", szTraitType);
	kUtility.SetYields(m_paiYieldChangeIncomingTradeRoute, "Trait_YieldChangesIncomingTradeRoute", "TraitType", szTraitType);
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

	//Populate m_MaintenanceModifierUnitCombats
	{
		const int iNumUnitCombatClasses = kUtility.MaxRows("UnitCombatInfos");
		kUtility.InitializeArray(m_piMaintenanceModifierUnitCombats, iNumUnitCombatClasses, 0);

		std::string sqlKey = "Trait_MaintenanceModifierUnitCombats";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select UnitCombatInfos.ID, MaintenanceModifier from Trait_MaintenanceModifierUnitCombats inner join UnitCombatInfos on UnitCombatInfos.Type = UnitCombatType where TraitType = ?;";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int iUnitCombatID = pResults->GetInt(0);
			CvAssert(iUnitCombatID > -1 && iUnitCombatID < iNumUnitCombatClasses);

			const int iMaintenanceModifier = pResults->GetInt(1);
			m_piMaintenanceModifierUnitCombats[iUnitCombatID] = iMaintenanceModifier;
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

	//UnimprovedFeatureYieldChanges
	{
		kUtility.Initialize2DArray(m_ppiUnimprovedFeatureYieldChanges, "Features", "Yields");

		std::string strKey("Trait_UnimprovedFeatureYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Features.ID as FeatureID, Yields.ID as YieldID, Yield from Trait_UnimprovedFeatureYieldChanges inner join Features on Features.Type = FeatureType inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int FeatureID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiUnimprovedFeatureYieldChanges[FeatureID][YieldID] = yield;
		}
	}

	// NoTrain
	{
		int iUnitClassLoop;
		for (iUnitClassLoop = 0; iUnitClassLoop < GC.getNumUnitClassInfos(); iUnitClassLoop++)
		{
			m_abNoTrainUnitClass.push_back(false);
		}

		std::string strKey("Trait_NoTrain");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "SELECT Traits.ID, UnitClasses.ID FROM Trait_NoTrain inner join Traits on Trait_NoTrain.TraitType = Traits.Type inner join UnitClasses on Trait_NoTrain.UnitClassType = UnitClasses.Type where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int iUnitClass = pResults->GetInt(1);
			m_abNoTrainUnitClass[iUnitClass] = true;
		}
	}

	// FreeResourceXCities
	{
		// Init vector
		int iResourceLoop;
		for(iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			FreeResourceXCities temp;
			m_aFreeResourceXCities.push_back(temp);
		}

		std::string strKey("Trait_FreeResourceFirstXCities");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Resources.ID as ResourceID, ResourceQuantity, NumCities from Trait_FreeResourceFirstXCities inner join Resources on Resources.Type = ResourceType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			FreeResourceXCities temp;

			const int iResource = pResults->GetInt(0);
			temp.m_iResourceQuantity = pResults->GetInt(1);
			temp.m_iNumCities = pResults->GetInt(2);

			m_aFreeResourceXCities[iResource] = temp;
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
	for(std::vector<CvTraitEntry*>::iterator it = m_paTraitEntries.begin(); it != m_paTraitEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paTraitEntries.clear();
}

/// Get a specific entry
CvTraitEntry* CvTraitXMLEntries::GetEntry(int index)
{
	return m_paTraitEntries[index];
}

//=====================================
// CvPlayerTraits
//=====================================
/// Constructor
CvPlayerTraits::CvPlayerTraits()
{
}

/// Destructor
CvPlayerTraits::~CvPlayerTraits(void)
{
	Uninit();
}

/// Initialize
void CvPlayerTraits::Init(CvTraitXMLEntries* pTraits, CvPlayer* pPlayer)
{
	// Store off the pointers to objects we'll need later
	m_pTraits = pTraits;
	m_pPlayer = pPlayer;

	Reset();

	// Initialize items that can't be reset everytime we earn a new tech (which resets all the other
	// trait data that can rebuilt from the trait entries)
	m_iBaktunPreviousTurn = 0;
	m_aMayaBonusChoices.clear();
	m_iBaktun = 0;
	m_iKatun = 0;
	m_iTun = 0;
	m_iWinal = 0;
	m_iKin = 0;

	m_aUniqueLuxuryAreas.clear();
	m_iUniqueLuxuryCitiesPlaced = 0;
}

/// Store off data on bonuses from traits
void CvPlayerTraits::InitPlayerTraits()
{
	for(int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if(HasTrait((TraitTypes)iI))
		{
			CvTraitEntry* trait = GC.getTraitInfo((TraitTypes)iI);
			m_iGreatPeopleRateModifier += trait->GetGreatPeopleRateModifier();
			m_iGreatScientistRateModifier += trait->GetGreatScientistRateModifier();
			m_iGreatGeneralRateModifier += trait->GetGreatGeneralRateModifier();
			m_iGreatGeneralExtraBonus += trait->GetGreatGeneralExtraBonus();
			m_iGreatPersonGiftInfluence += trait->GetGreatPersonGiftInfluence();
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
#if defined(MOD_TRAITS_CITY_WORKING)
			m_iCityWorkingChange += trait->GetCityWorkingChange();
#endif
			m_iPlotCultureCostModifier += trait->GetPlotCultureCostModifier();
			m_iCultureFromKills += trait->GetCultureFromKills();
			m_iFaithFromKills += trait->GetFaithFromKills();
			m_iCityCultureBonus += trait->GetCityCultureBonus();
			m_iCapitalThemingBonusModifier += trait->GetCapitalThemingBonusModifier();
			m_iPolicyCostModifier += trait->GetPolicyCostModifier();
			m_iCityConnectionTradeRouteChange += trait->GetCityConnectionTradeRouteChange();
			m_iWonderProductionModifier += trait->GetWonderProductionModifier();
			m_iPlunderModifier += trait->GetPlunderModifier();
			m_iImprovementMaintenanceModifier += trait->GetImprovementMaintenanceModifier();
			m_iGoldenAgeDurationModifier += trait->GetGoldenAgeDurationModifier();
			m_iGoldenAgeMoveChange += trait->GetGoldenAgeMoveChange();
			m_iGoldenAgeCombatModifier += trait->GetGoldenAgeCombatModifier();
			m_iGoldenAgeTourismModifier += trait->GetGoldenAgeTourismModifier();
			m_iGoldenAgeGreatArtistRateModifier += trait->GetGoldenAgeGreatArtistRateModifier();
			m_iGoldenAgeGreatMusicianRateModifier += trait->GetGoldenAgeGreatMusicianRateModifier();
			m_iGoldenAgeGreatWriterRateModifier += trait->GetGoldenAgeGreatWriterRateModifier();
			m_iExtraEmbarkMoves += trait->GetExtraEmbarkMoves();
			m_iNaturalWonderFirstFinderGold += trait->GetNaturalWonderFirstFinderGold();
			m_iNaturalWonderSubsequentFinderGold += trait->GetNaturalWonderSubsequentFinderGold();
			m_iNaturalWonderYieldModifier += trait->GetNaturalWonderYieldModifier();
			m_iNaturalWonderHappinessModifier += trait->GetNaturalWonderHappinessModifier();
			m_iNearbyImprovementCombatBonus += trait->GetNearbyImprovementCombatBonus();
			m_iNearbyImprovementBonusRange += trait->GetNearbyImprovementBonusRange();
			m_iCultureBuildingYieldChange += trait->GetCultureBuildingYieldChange();
			m_iCombatBonusVsHigherTech += trait->GetCombatBonusVsHigherTech();
			m_iCombatBonusVsLargerCiv += trait->GetCombatBonusVsLargerCiv();
			m_iLandUnitMaintenanceModifier += trait->GetLandUnitMaintenanceModifier();
			m_iNavalUnitMaintenanceModifier += trait->GetNavalUnitMaintenanceModifier();
			m_iRazeSpeedModifier += trait->GetRazeSpeedModifier();
			m_iDOFGreatPersonModifier += trait->GetDOFGreatPersonModifier();
			m_iLuxuryHappinessRetention += trait->GetLuxuryHappinessRetention();
			m_iExtraSpies += trait->GetExtraSpies();
			m_iUnresearchedTechBonusFromKills += trait->GetUnresearchedTechBonusFromKills();
			m_iExtraFoundedCityTerritoryClaimRange += trait->GetExtraFoundedCityTerritoryClaimRange();
			m_iFreeSocialPoliciesPerEra += trait->GetFreeSocialPoliciesPerEra();
			m_iNumTradeRoutesModifier += trait->GetNumTradeRoutesModifier();
			m_iTradeRouteResourceModifier += trait->GetTradeRouteResourceModifier();
			m_iUniqueLuxuryCities += trait->GetUniqueLuxuryCities();
			m_iUniqueLuxuryQuantity	+= trait->GetUniqueLuxuryQuantity();
			m_iWorkerSpeedModifier += trait->GetWorkerSpeedModifier();
			m_iAfraidMinorPerTurnInfluence += trait->GetAfraidMinorPerTurnInfluence();
			m_iLandTradeRouteRangeBonus += trait->GetLandTradeRouteRangeBonus();
#if defined(MOD_TRAITS_TRADE_ROUTE_BONUSES)
			m_iSeaTradeRouteRangeBonus += trait->GetSeaTradeRouteRangeBonus();
#endif
			m_iTradeReligionModifier += trait->GetTradeReligionModifier();
			m_iTradeBuildingModifier += trait->GetTradeBuildingModifier();
#if defined(MOD_BALANCE_CORE)
			m_iNumFreeBuildings	+= trait->GetNumFreeBuildings();
#endif

			if(trait->IsFightWellDamaged())
			{
				m_bFightWellDamaged = true;
				// JON: Changing the way this works. Above line can/should probably be removed at some point
				int iWoundedUnitDamageMod = /*-50*/ GC.getTRAIT_WOUNDED_DAMAGE_MOD();
				m_pPlayer->ChangeWoundedUnitDamageMod(iWoundedUnitDamageMod);
			}
			if(trait->IsMoveFriendlyWoodsAsRoad())
			{
				m_bMoveFriendlyWoodsAsRoad = true;
			}
			if(trait->IsFasterAlongRiver())
			{
				m_bFasterAlongRiver = true;
			}
			if(trait->IsFasterInHills())
			{
				m_bFasterInHills = true;
			}
			if(trait->IsEmbarkedAllWater())
			{
				m_bEmbarkedAllWater = true;
			}
			if(trait->IsEmbarkedToLandFlatCost())
			{
				m_bEmbarkedToLandFlatCost = true;
			}
			if(trait->IsNoHillsImprovementMaintenance())
			{
				m_bNoHillsImprovementMaintenance = true;
			}
			if(trait->IsTechBoostFromCapitalScienceBuildings())
			{
				m_bTechBoostFromCapitalScienceBuildings = true;
			}
			if(trait->IsStaysAliveZeroCities())
			{
				m_bStaysAliveZeroCities = true;
			}
			if(trait->IsFaithFromUnimprovedForest())
			{
				m_bFaithFromUnimprovedForest = true;
			}
#if defined(MOD_TRAITS_ANY_BELIEF)
			if(trait->IsAnyBelief())
			{
				m_bAnyBelief = true;
			}
#endif
			if(trait->IsBonusReligiousBelief())
			{
				m_bBonusReligiousBelief = true;
			}
			if(trait->IsAbleToAnnexCityStates())
			{
				m_bAbleToAnnexCityStates = true;
			}
			if(trait->IsCrossesMountainsAfterGreatGeneral())
			{
				m_bCrossesMountainsAfterGreatGeneral = true;
			}
#if defined(MOD_TRAITS_CROSSES_ICE)
			if(trait->IsCrossesIce())
			{
				m_bCrossesIce = true;
			}
#endif
			if(trait->IsMayaCalendarBonuses())
			{
				m_bMayaCalendarBonuses = true;
			}
			if (trait->IsNoAnnexing())
			{
				m_bNoAnnexing = true;
			}
			if (trait->IsTechFromCityConquer())
			{
				m_bTechFromCityConquer = true;
			}
			if (trait->IsUniqueLuxuryRequiresNewArea())
			{
				m_bUniqueLuxuryRequiresNewArea = true;
			}
			if (trait->IsRiverTradeRoad())
			{
				m_bRiverTradeRoad = true;
			}
			if (trait->IsAngerFreeIntrusionOfCityStates())
			{
				m_bAngerFreeIntrusionOfCityStates = true;
			}

			for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
			{
				if(trait->GetExtraYieldThreshold(iYield) > m_iExtraYieldThreshold[iYield])
				{
					m_iExtraYieldThreshold[iYield] = trait->GetExtraYieldThreshold(iYield);
				}
				m_iFreeCityYield[iYield] = trait->GetYieldChange(iYield);
				m_iYieldChangeStrategicResources[iYield] = trait->GetYieldChangeStrategicResources(iYield);
				m_iYieldChangeNaturalWonder[iYield] = trait->GetYieldChangeNaturalWonder(iYield);
				m_iYieldChangePerTradePartner[iYield] = trait->GetYieldChangePerTradePartner(iYield);
				m_iYieldChangeIncomingTradeRoute[iYield] = trait->GetYieldChangeIncomingTradeRoute(iYield);
				m_iYieldRateModifier[iYield] = trait->GetYieldModifier(iYield);

				for(int iFeatureLoop = 0; iFeatureLoop < GC.getNumFeatureInfos(); iFeatureLoop++)
				{
					int iChange = trait->GetUnimprovedFeatureYieldChanges((FeatureTypes)iFeatureLoop, (YieldTypes)iYield);
					if(iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiUnimprovedFeatureYieldChange[iFeatureLoop];
						yields[iYield] = (m_ppaaiUnimprovedFeatureYieldChange[iFeatureLoop][iYield] + iChange);
						m_ppaaiUnimprovedFeatureYieldChange[iFeatureLoop] = yields;
					}
				}

				for(int iImprovementLoop = 0; iImprovementLoop < GC.getNumImprovementInfos(); iImprovementLoop++)
				{
					int iChange = trait->GetImprovementYieldChanges((ImprovementTypes)iImprovementLoop, (YieldTypes)iYield);
					if(iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiImprovementYieldChange[iImprovementLoop];
						yields[iYield] = (m_ppaaiImprovementYieldChange[iImprovementLoop][iYield] + iChange);
						m_ppaaiImprovementYieldChange[iImprovementLoop] = yields;
					}
				}

				for(int iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
				{
					int iChange = trait->GetSpecialistYieldChanges((SpecialistTypes)iSpecialistLoop, (YieldTypes)iYield);
					if(iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiSpecialistYieldChange[iSpecialistLoop];
						yields[iYield] = (m_ppaaiSpecialistYieldChange[iSpecialistLoop][iYield] + iChange);
						m_ppaaiSpecialistYieldChange[iSpecialistLoop] = yields;
					}
				}
			}
			CvAssert(GC.getNumTerrainInfos() <= NUM_TERRAIN_TYPES);
			for(int iTerrain = 0; iTerrain < GC.getNumTerrainInfos(); iTerrain++)
			{
				m_iStrategicResourceQuantityModifier[iTerrain] = trait->GetStrategicResourceQuantityModifier(iTerrain);
			}

			for(int iResource = 0; iResource < GC.getNumResourceInfos(); iResource++)
			{
				m_aiResourceQuantityModifier[iResource] = trait->GetResourceQuantityModifier(iResource);
			}

			for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); iUnitClass++)
			{
				m_abNoTrain[iUnitClass] = trait->NoTrain((UnitClassTypes)iUnitClass);
			}

			FreeTraitUnit traitUnit;
			traitUnit.m_iFreeUnit = (UnitTypes)trait->GetFreeUnitClassType();
			if(traitUnit.m_iFreeUnit != NO_UNITCLASS)
			{
				traitUnit.m_ePrereqTech = trait->GetFreeUnitPrereqTech();
				m_aFreeTraitUnits.push_back(traitUnit);
			}

			m_eCombatBonusImprovement = trait->GetCombatBonusImprovement();

			int iNumUnitCombatClassInfos = GC.getNumUnitCombatClassInfos();
			for(int jJ= 0; jJ < iNumUnitCombatClassInfos; jJ++)
			{
				m_paiMovesChangeUnitCombat[jJ] += trait->GetMovesChangeUnitCombat(jJ);
				m_paiMaintenanceModifierUnitCombat[jJ] += trait->GetMaintenanceModifierUnitCombat(jJ);
			}

			for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				FreeResourceXCities temp = trait->GetFreeResourceXCities((ResourceTypes)iResourceLoop);
				if(temp.m_iResourceQuantity > 0)
				{
					m_aFreeResourceXCities[iResourceLoop] = temp;
				}
			}
		}
	}
}

/// Deallocate memory created in initialize
void CvPlayerTraits::Uninit()
{
	m_aiResourceQuantityModifier.clear();
	m_abNoTrain.clear();
	m_paiMovesChangeUnitCombat.clear();
	m_paiMaintenanceModifierUnitCombat.clear();
	m_ppaaiImprovementYieldChange.clear();
	m_ppaaiSpecialistYieldChange.clear();
	m_ppaaiUnimprovedFeatureYieldChange.clear();
	m_aFreeResourceXCities.clear();
}

/// Reset data members
void CvPlayerTraits::Reset()
{
	Uninit();

	m_iGreatPeopleRateModifier = 0;
	m_iGreatScientistRateModifier = 0;
	m_iGreatGeneralRateModifier = 0;
	m_iGreatGeneralExtraBonus = 0;
	m_iGreatPersonGiftInfluence = 0;
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
#if defined(MOD_TRAITS_CITY_WORKING)
	m_iCityWorkingChange = 0;
#endif
	m_iPlotCultureCostModifier = 0;
	m_iCultureFromKills = 0;
	m_iFaithFromKills = 0;
	m_iCityCultureBonus = 0;
	m_iCapitalThemingBonusModifier = 0;
	m_iPolicyCostModifier = 0;
	m_iCityConnectionTradeRouteChange = 0;
	m_iWonderProductionModifier = 0;
	m_iPlunderModifier = 0;
	m_iImprovementMaintenanceModifier = 0;
	m_iGoldenAgeDurationModifier = 0;
	m_iGoldenAgeMoveChange = 0;
	m_iGoldenAgeCombatModifier = 0;
	m_iGoldenAgeTourismModifier = 0;
	m_iGoldenAgeGreatArtistRateModifier = 0;
	m_iGoldenAgeGreatMusicianRateModifier = 0;
	m_iGoldenAgeGreatWriterRateModifier = 0;
	m_iExtraEmbarkMoves = 0;
	m_iNaturalWonderFirstFinderGold = 0;
	m_iNaturalWonderSubsequentFinderGold = 0;
	m_iNaturalWonderYieldModifier = 0;
	m_iNaturalWonderHappinessModifier = 0;
	m_iNearbyImprovementCombatBonus = 0;
	m_iNearbyImprovementBonusRange = 0;
	m_iCultureBuildingYieldChange = 0;
	m_iCombatBonusVsHigherTech = 0;
	m_iCombatBonusVsLargerCiv = 0;
	m_iLandUnitMaintenanceModifier = 0;
	m_iNavalUnitMaintenanceModifier = 0;
	m_iRazeSpeedModifier = 0;
	m_iDOFGreatPersonModifier = 0;
	m_iLuxuryHappinessRetention = 0;
	m_iExtraSpies = 0;
	m_iUnresearchedTechBonusFromKills = 0;
	m_iExtraFoundedCityTerritoryClaimRange = 0;
	m_iFreeSocialPoliciesPerEra = 0;
	m_iNumTradeRoutesModifier = 0;
	m_iTradeRouteResourceModifier = 0;
	m_iUniqueLuxuryCities = 0;
	m_iUniqueLuxuryQuantity = 0;
	m_iWorkerSpeedModifier = 0;
	m_iAfraidMinorPerTurnInfluence = 0;
	m_iLandTradeRouteRangeBonus = 0;
#if defined(MOD_TRAITS_TRADE_ROUTE_BONUSES)
	m_iSeaTradeRouteRangeBonus = 0;
#endif
	m_iTradeReligionModifier = 0;
	m_iTradeBuildingModifier = 0;
#if defined(MOD_BALANCE_CORE)
	m_iNumFreeBuildings = 0;
#endif

	m_bFightWellDamaged = false;
	m_bMoveFriendlyWoodsAsRoad = false;
	m_bFasterAlongRiver = false;
	m_bFasterInHills = false;
	m_bEmbarkedAllWater = false;
	m_bEmbarkedToLandFlatCost = false;
	m_bNoHillsImprovementMaintenance = false;
	m_bTechBoostFromCapitalScienceBuildings = false;
	m_bStaysAliveZeroCities = false;
	m_bFaithFromUnimprovedForest = false;
#if defined(MOD_TRAITS_ANY_BELIEF)
	m_bAnyBelief = false;
#endif
	m_bBonusReligiousBelief = false;
	m_bAbleToAnnexCityStates = false;
	m_bCrossesMountainsAfterGreatGeneral = false;
#if defined(MOD_TRAITS_CROSSES_ICE)
	m_bCrossesIce = false;
#endif
	m_bMayaCalendarBonuses = false;
	m_bNoAnnexing = false;
	m_bTechFromCityConquer = false;
	m_bUniqueLuxuryRequiresNewArea = false;
	m_bRiverTradeRoad = false;
	m_bAngerFreeIntrusionOfCityStates = false;

	m_eCampGuardType = NO_UNIT;
	m_eCombatBonusImprovement = NO_IMPROVEMENT;

	m_ppaaiImprovementYieldChange.clear();
	m_ppaaiImprovementYieldChange.resize(GC.getNumImprovementInfos());
	m_ppaaiSpecialistYieldChange.clear();
	m_ppaaiSpecialistYieldChange.resize(GC.getNumSpecialistInfos());
	m_ppaaiUnimprovedFeatureYieldChange.clear();
	m_ppaaiUnimprovedFeatureYieldChange.resize(GC.getNumFeatureInfos());

	Firaxis::Array< int, NUM_YIELD_TYPES > yield;
	for(unsigned int j = 0; j < NUM_YIELD_TYPES; ++j)
	{
		yield[j] = 0;
	}

	for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		m_iExtraYieldThreshold[iYield] = 0;
		m_iFreeCityYield[iYield] = 0;
		m_iYieldChangeStrategicResources[iYield] = 0;
		m_iYieldChangeNaturalWonder[iYield] = 0;
		m_iYieldChangePerTradePartner[iYield] = 0;
		m_iYieldChangeIncomingTradeRoute[iYield] = 0;
		m_iYieldRateModifier[iYield] = 0;

		for(int iImprovement = 0; iImprovement < GC.getNumImprovementInfos(); iImprovement++)
		{
			m_ppaaiImprovementYieldChange[iImprovement] = yield;
		}
		for(int iSpecialist = 0; iSpecialist < GC.getNumSpecialistInfos(); iSpecialist++)
		{
			m_ppaaiSpecialistYieldChange[iSpecialist] = yield;
		}
		for(int iFeature = 0; iFeature < GC.getNumFeatureInfos(); iFeature++)
		{
			m_ppaaiUnimprovedFeatureYieldChange[iFeature] = yield;
		}
	}

	for(int iTerrain = 0; iTerrain < GC.getNumTerrainInfos(); iTerrain++)
	{
		m_iStrategicResourceQuantityModifier[iTerrain] = 0;
	}

	m_aiResourceQuantityModifier.clear();
	m_aiResourceQuantityModifier.resize(GC.getNumResourceInfos());

	for(int iResource = 0; iResource < GC.getNumResourceInfos(); iResource++)
	{
		m_aiResourceQuantityModifier[iResource] = 0;
	}

	m_abNoTrain.clear();
	m_abNoTrain.resize(GC.getNumUnitClassInfos());
	for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); iUnitClass++)
	{
		m_abNoTrain[iUnitClass] = false;
	}

	m_aFreeTraitUnits.clear();

	int iNumUnitCombatClassInfos = GC.getNumUnitCombatClassInfos();
	CvAssertMsg((0 < iNumUnitCombatClassInfos),  "GC.getNumUnitCombatClassInfos() is not greater than zero but an array is being allocated in CvPlayerTraits::Reset");
	m_paiMovesChangeUnitCombat.clear();
	m_paiMaintenanceModifierUnitCombat.clear();
	m_paiMovesChangeUnitCombat.resize(iNumUnitCombatClassInfos);
	m_paiMaintenanceModifierUnitCombat.resize(iNumUnitCombatClassInfos);
	for(int iI = 0; iI < iNumUnitCombatClassInfos; iI++)
	{
		m_paiMovesChangeUnitCombat[iI] = 0;
		m_paiMaintenanceModifierUnitCombat[iI] = 0;
	}

	int iResourceLoop;
	for(iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		FreeResourceXCities temp;
		m_aFreeResourceXCities.push_back(temp);
	}
}

/// Does this player possess a specific trait?
bool CvPlayerTraits::HasTrait(TraitTypes eTrait) const
{
	CvAssert(m_pPlayer);

	if(m_pPlayer != NULL)
	{
		CvAssertMsg((m_pPlayer->getLeaderType() >= 0), "getLeaderType() is less than zero");
		CvAssertMsg((eTrait >= 0), "eTrait is less than zero");
		return m_pPlayer->getLeaderInfo().hasTrait(eTrait) && !m_pTraits->GetEntry(eTrait)->IsObsoleteByTech(m_pPlayer->getTeam()) && m_pTraits->GetEntry(eTrait)->IsEnabledByTech(m_pPlayer->getTeam());
	}
	else
	{
		return false;
	}
}

/// Will settling a city in this new area unlock a unique luxury?
bool CvPlayerTraits::WillGetUniqueLuxury(CvArea *pArea) const
{
	// Still have more of these cities to award?
	if (m_iUniqueLuxuryCities > m_iUniqueLuxuryCitiesPlaced)
	{
		int iArea = pArea->GetID();

		// If we have to be in a new area, check to see if this area is okay
		if (m_bUniqueLuxuryRequiresNewArea)
		{
			// Can't be the capital itself
			if (m_pPlayer->GetNumCitiesFounded() == 0)
			{
				return false;
			}

			CvPlot *pOriginalCapitalPlot = GC.getMap().plot(m_pPlayer->GetOriginalCapitalX(), m_pPlayer->GetOriginalCapitalY());
			if (pOriginalCapitalPlot)
			{
				if (pOriginalCapitalPlot->getArea() == iArea)
				{
					return false;
				}
			}

			// Already in the list?
			if (std::find (m_aUniqueLuxuryAreas.begin(), m_aUniqueLuxuryAreas.end(), iArea) != m_aUniqueLuxuryAreas.end())
			{
				return false;
			}
		}

		int iNumUniqueResourcesGiven = m_aUniqueLuxuryAreas.size();

		// Loop through all resources and see if we can find one more
		int iNumUniquesFound = 0;
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			ResourceTypes eResource = (ResourceTypes) iResourceLoop;
			CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
			if (pkResource != NULL && pkResource->GetRequiredCivilization() == m_pPlayer->getCivilizationType())
			{
				iNumUniquesFound++;
				if (iNumUniquesFound > iNumUniqueResourcesGiven)
				{
					return true;
				}
			}
		}
	}

	return false;
}

/// Bonus movement for this combat class
int CvPlayerTraits::GetMovesChangeUnitCombat(const int unitCombatID) const
{
	CvAssertMsg(unitCombatID < GC.getNumUnitCombatClassInfos(),  "Invalid unitCombatID parameter in call to CvPlayerTraits::GetMovesChangeUnitCombat()");

	if(unitCombatID == NO_UNITCLASS)
	{
		return 0;
	}

	return m_paiMovesChangeUnitCombat[unitCombatID];
}

/// Maintenance modifier for this combat class
int CvPlayerTraits::GetMaintenanceModifierUnitCombat(const int unitCombatID) const
{
	CvAssertMsg(unitCombatID < GC.getNumUnitCombatClassInfos(),  "Invalid unitCombatID parameter in call to CvPlayerTraits::GetMaintenanceModifierUnitCombat()");

	if(unitCombatID == NO_UNITCLASS)
	{
		return 0;
	}

	return m_paiMaintenanceModifierUnitCombat[unitCombatID];
}

/// Extra yield from this improvement
int CvPlayerTraits::GetImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYield) const
{
	CvAssertMsg(eImprovement < GC.getNumImprovementInfos(),  "Invalid eImprovement parameter in call to CvPlayerTraits::GetImprovementYieldChange()");
	CvAssertMsg(eYield < NUM_YIELD_TYPES,  "Invalid eYield parameter in call to CvPlayerTraits::GetImprovementYieldChange()");

	if(eImprovement == NO_IMPROVEMENT)
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

	if(eSpecialist == NO_SPECIALIST)
	{
		return 0;
	}

	return m_ppaaiSpecialistYieldChange[(int)eSpecialist][(int)eYield];
}

/// Extra yield from a feature without improvement
int CvPlayerTraits::GetUnimprovedFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYield) const
{
	CvAssertMsg(eFeature < GC.getNumFeatureInfos(),  "Invalid eImprovement parameter in call to CvPlayerTraits::GetUnimprovedFeatureYieldChange()");
	CvAssertMsg(eYield < NUM_YIELD_TYPES,  "Invalid eYield parameter in call to CvPlayerTraits::GetUnimprovedFeatureYieldChange()");

	if(eFeature == NO_FEATURE)
	{
		return 0;
	}

	return m_ppaaiUnimprovedFeatureYieldChange[(int)eFeature][(int)eYield];
}

/// Do all new units get a specific promotion?
bool CvPlayerTraits::HasFreePromotionUnitCombat(const int promotionID, const int unitCombatID) const
{
	CvAssertMsg((promotionID >= 0), "promotionID is less than zero");
	for(int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		const TraitTypes eTrait = static_cast<TraitTypes>(iI);
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(eTrait);
		if(pkTraitInfo)
		{
			if(HasTrait(eTrait))
			{
				if(pkTraitInfo->IsFreePromotionUnitCombat(promotionID, unitCombatID))
				{
					return true;
				}
			}
		}
	}

	return false;
}

/// Does each city get a free building?
BuildingTypes CvPlayerTraits::GetFreeBuilding() const
{
	for(int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		const TraitTypes eTrait = static_cast<TraitTypes>(iI);
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(eTrait);
		if(pkTraitInfo)
		{
			if(HasTrait(eTrait))
			{
				if(pkTraitInfo->GetFreeBuilding())
				{
					return pkTraitInfo->GetFreeBuilding();
				}
			}
		}
	}

	return NO_BUILDING;
}
#if defined(MOD_BALANCE_CORE)
/// Does the capital get a free building?
BuildingTypes CvPlayerTraits::GetFreeCapitalBuilding() const
{
	for(int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		const TraitTypes eTrait = static_cast<TraitTypes>(iI);
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(eTrait);
		if(pkTraitInfo)
		{
			if(HasTrait(eTrait))
			{
				if(pkTraitInfo->GetFreeCapitalBuilding())
				{
					return pkTraitInfo->GetFreeCapitalBuilding();
				}
			}
		}
	}

	return NO_BUILDING;
}
#endif

/// Does each conquered city get a free building?
BuildingTypes CvPlayerTraits::GetFreeBuildingOnConquest() const
{
	for(int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		const TraitTypes eTrait = static_cast<TraitTypes>(iI);
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(eTrait);
		if(pkTraitInfo)
		{
			if(HasTrait(eTrait))
			{
				if(pkTraitInfo->GetFreeBuildingOnConquest())
				{
					return pkTraitInfo->GetFreeBuildingOnConquest();
				}
			}
		}
	}

	return NO_BUILDING;
}

/// Should unique luxuries appear beneath this tile?
void CvPlayerTraits::AddUniqueLuxuries(CvCity *pCity)
{
	// Still have more of these cities to award?
	if (m_iUniqueLuxuryCities > m_iUniqueLuxuryCitiesPlaced)
	{
		int iArea = pCity->getArea();

		// If we have to be in a new area, check to see if this area is okay
		if (m_bUniqueLuxuryRequiresNewArea)
		{
			// Can't be the capital itself of the area where the capital was founded
			if (m_pPlayer->GetNumCitiesFounded() == 1)
			{
				return;
			}

			CvPlot *pOriginalCapitalPlot = GC.getMap().plot(m_pPlayer->GetOriginalCapitalX(), m_pPlayer->GetOriginalCapitalY());
			if (pOriginalCapitalPlot)
			{
				if (pOriginalCapitalPlot->getArea() == iArea)
				{
					return;
				}
			}

			// Already in the list?
			if (std::find (m_aUniqueLuxuryAreas.begin(), m_aUniqueLuxuryAreas.end(), iArea) != m_aUniqueLuxuryAreas.end())
			{
				return;
			}
			
#if defined(MOD_EVENTS_AREA_RESOURCES)
			if (MOD_EVENTS_AREA_RESOURCES) {
				if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_AreaCanHaveAnyResource, m_pPlayer->GetID(), iArea) == GAMEEVENTRETURN_FALSE) {
					return;
				}
			}
#endif
		}

		m_aUniqueLuxuryAreas.push_back(iArea);  		// Store area
		int iNumUniqueResourcesGiven = m_aUniqueLuxuryAreas.size();
		m_iUniqueLuxuryCitiesPlaced++;   // One less to give out

		// Loop through all resources and see if we can find this many unique ones
		ResourceTypes eResourceToGive = NO_RESOURCE;
		int iNumUniquesFound = 0;
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			ResourceTypes eResource = (ResourceTypes) iResourceLoop;
			CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
			if (pkResource != NULL && pkResource->GetRequiredCivilization() == m_pPlayer->getCivilizationType())
			{
				iNumUniquesFound++;
				if (iNumUniquesFound == iNumUniqueResourcesGiven)
				{
					eResourceToGive = eResource;
					break;
				}
			}
		}

		if (eResourceToGive != NO_RESOURCE)
		{
#if defined(MOD_EVENTS_AREA_RESOURCES)
			if (MOD_EVENTS_AREA_RESOURCES) {
				GAMEEVENTINVOKE_HOOK(GAMEEVENT_PlaceResource, m_pPlayer->GetID(), eResourceToGive, m_iUniqueLuxuryQuantity, pCity->getX(), pCity->getY());
			} else {
#endif
				pCity->plot()->setResourceType(NO_RESOURCE, 0, true);
				pCity->plot()->setResourceType(eResourceToGive, m_iUniqueLuxuryQuantity, true);
#if defined(MOD_EVENTS_AREA_RESOURCES)
			}
#endif
		}
	}
}

/// Does a unit entering this tile cause a barbarian to convert to the player?
bool CvPlayerTraits::CheckForBarbarianConversion(CvPlot* pPlot)
{
	// Loop through all adjacent plots
	CvPlot* pAdjacentPlot;
	int iI;
	bool bRtnValue = false;

	if(pPlot->isWater() && GetSeaBarbarianConversionPercent() > 0)
	{
		for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

			if(pAdjacentPlot != NULL)
			{
				if(pAdjacentPlot->isWater())
				{
					UnitHandle pNavalUnit = pAdjacentPlot->getBestDefender(BARBARIAN_PLAYER);
					if(pNavalUnit)
					{
						if(ConvertBarbarianNavalUnit(pNavalUnit))
						{
							bRtnValue = true;
						}
					}
				}
			}
		}
	}

	else if(GetLandBarbarianConversionPercent() > 0 && pPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT() &&
	        m_eCampGuardType != NO_UNIT)
	{
		bRtnValue = ConvertBarbarianCamp(pPlot);
	}

	return bRtnValue;
}

/// Discounted cost if building already present in capital
int CvPlayerTraits::GetCapitalBuildingDiscount(BuildingTypes eBuilding)
{
	if(m_iCapitalBuildingModifier > 0)
	{
		// Find this player's capital
		CvCity* pCapital = m_pPlayer->getCapitalCity();
		if(pCapital)
		{
			if(pCapital->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				return m_iCapitalBuildingModifier;
			}
		}
	}
	return 0;
}

#if defined(MOD_BALANCE_CORE)
TechTypes CvPlayerTraits::GetFreeBuildingPrereqTech() const
{
	for(int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		const TraitTypes eTrait = static_cast<TraitTypes>(iI);
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(eTrait);
		if(pkTraitInfo)
		{
			if(HasTrait(eTrait))
			{
				if(pkTraitInfo->GetFreeBuildingPrereqTech())
				{
					return pkTraitInfo->GetFreeBuildingPrereqTech();
				}
			}
		}
	}

	return NO_TECH;
}
TechTypes CvPlayerTraits::GetCapitalFreeBuildingPrereqTech() const
{
	for(int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		const TraitTypes eTrait = static_cast<TraitTypes>(iI);
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(eTrait);
		if(pkTraitInfo)
		{
			if(HasTrait(eTrait))
			{
				if(pkTraitInfo->GetCapitalFreeBuildingPrereqTech())
				{
					return pkTraitInfo->GetCapitalFreeBuildingPrereqTech();
				}
			}
		}
	}

	return NO_TECH;
}
#endif

/// First free unit received through traits
int CvPlayerTraits::GetFirstFreeUnit(TechTypes eTech)
{
	m_uiFreeUnitIndex = 0;
	m_eFreeUnitPrereqTech = eTech;

	while(m_uiFreeUnitIndex < m_aFreeTraitUnits.size())
	{
		if(eTech == m_aFreeTraitUnits[m_uiFreeUnitIndex].m_ePrereqTech)
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

	while(m_uiFreeUnitIndex < m_aFreeTraitUnits.size())
	{
		if(m_eFreeUnitPrereqTech == m_aFreeTraitUnits[m_uiFreeUnitIndex].m_ePrereqTech)
		{
			return m_aFreeTraitUnits[m_uiFreeUnitIndex].m_iFreeUnit;
		}

		m_uiFreeUnitIndex++;
	}

	return NO_UNITCLASS;
}

/// Does this trait provide free resources in the first X cities?
FreeResourceXCities CvPlayerTraits::GetFreeResourceXCities(ResourceTypes eResource) const
{
	return m_aFreeResourceXCities[(int)eResource];
}

/// Is this civ currently able to cross mountains with combat units?
bool CvPlayerTraits::IsAbleToCrossMountains() const
{
	return (m_bCrossesMountainsAfterGreatGeneral && m_pPlayer->getGreatGeneralsCreated() > 0);
}

#if defined(MOD_TRAITS_CROSSES_ICE)
/// Is this civ currently able to cross ice with combat units?
bool CvPlayerTraits::IsAbleToCrossIce() const
{
	return (m_bCrossesIce);
}
#endif

bool CvPlayerTraits::NoTrain(UnitClassTypes eUnitClassType)
{
	if (eUnitClassType != NO_UNITCLASS)
	{
		return m_abNoTrain[eUnitClassType];
	}
	else
	{
		return false;
	}
}


// MAYA TRAIT SPECIAL METHODS

const float CALENDAR_START = -3112.3973f;  // Actual date is August 11, 3114 BCE.  This float is reversed engineered to come out to Dec 21, 2012 as 13.0.0.0.0
const float DAYS_IN_BAKTUN = 144000.0f;
const float DAYS_IN_KATUN = 7200.0f;
const float DAYS_IN_TUN = 360.0f;
const float DAYS_IN_WINAL = 20.0f;
const float DAYS_IN_YEAR = 365.242199f;

/// Is the Maya calendar active for this player?
bool CvPlayerTraits::IsUsingMayaCalendar() const
{
	int iNumTraits = GC.getNumTraitInfos();
	for(int iI = 0; iI < iNumTraits; iI++)
	{
		const TraitTypes eTrait = static_cast<TraitTypes>(iI);
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(eTrait);
		if(pkTraitInfo)
		{
			if(pkTraitInfo->IsMayaCalendarBonuses())
			{
				if(HasTrait(eTrait))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}
	return false;
}

/// Is this the first turn at the end of a baktun (cycle) of the Maya Long Count calendar?
bool CvPlayerTraits::IsEndOfMayaLongCount()
{
	bool bRtnValue = false;

	if (!IsUsingMayaCalendar())
	{
		return bRtnValue;
	}

	ComputeMayaDate();

	if(m_iBaktunPreviousTurn + 1 == m_iBaktun)
	{
		bRtnValue = true;
	}

	m_iBaktunPreviousTurn = m_iBaktun;

	return bRtnValue;
}

/// Return a string with the Maya calendar date
CvString CvPlayerTraits::GetMayaCalendarString()
{
	CvString szRtnValue;
	ComputeMayaDate();
	szRtnValue.Format("%d.%d.%d.%d.%d", m_iBaktun, m_iKatun, m_iTun, m_iWinal, m_iKin);
	return szRtnValue;
}

/// Return a string with the Maya calendar date
CvString CvPlayerTraits::GetMayaCalendarLongString()
{
	CvString szRtnValue;
	ComputeMayaDate();
	szRtnValue = GetLocalizedText("TXT_KEY_MAYA_CALENDAR_LONG_STRING", m_iBaktun, m_iKatun, m_iTun, m_iWinal, m_iKin);
	return szRtnValue;
}

/// AI routine to pick a Maya bonus for an AI civ
void CvPlayerTraits::ChooseMayaBoost()
{
	UnitTypes eDesiredGreatPerson = NO_UNIT;
	UnitTypes ePossibleGreatPerson;

	// Go for a prophet?
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
	ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_PROPHET", true);
#else
	ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_PROPHET", true);
#endif
	if(GetUnitBaktun(ePossibleGreatPerson) == 0)
	{
		CvGameReligions* pReligions = GC.getGame().GetGameReligions();
		ReligionTypes eReligion = pReligions->GetReligionCreatedByPlayer(m_pPlayer->GetID());

		// Have a religion that isn't enhanced yet?
		if(eReligion != NO_RELIGION)
		{
			const CvReligion* pMyReligion = pReligions->GetReligion(eReligion, m_pPlayer->GetID());
			if(!pMyReligion->m_bEnhanced)
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
		}

		// Don't have a religion and they can still be founded?
		else
		{
			if(pReligions->GetNumReligionsStillToFound() > 0)
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
		}
	}

	// Highly wonder competitive and still early in game?
	if(eDesiredGreatPerson == NO_UNIT)
	{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
		ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_ENGINEER");
#else
		ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_ENGINEER");
#endif
		if(GetUnitBaktun(ePossibleGreatPerson) == 0)
		{
			if(m_pPlayer->GetDiplomacyAI()->GetWonderCompetitiveness() >= 8 && GC.getGame().getGameTurn() <= (GC.getGame().getEstimateEndTurn() / 2))
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
		}
	}

	// Does our grand strategy match one that is available?
	AIGrandStrategyTypes eVictoryStrategy = m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy();
	if(eDesiredGreatPerson == NO_UNIT)
	{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
		ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_SCIENTIST");
#else
		ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_SCIENTIST");
#endif
		if(GetUnitBaktun(ePossibleGreatPerson) == 0)
		{
			if(eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_SPACESHIP"))
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
		}
	}
	if(eDesiredGreatPerson == NO_UNIT)
	{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
		ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_ARTIST");
#else
		ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_ARTIST");
#endif
		if(GetUnitBaktun(ePossibleGreatPerson) == 0)
		{
			if(eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE"))
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
		}
	}
	if(eDesiredGreatPerson == NO_UNIT)
	{
#if defined(MOD_DIPLOMACY_CITYSTATES)
		if (MOD_DIPLOMACY_CITYSTATES)
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
			ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_GREAT_DIPLOMAT");
#else
			ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_GREAT_DIPLOMAT");
#endif
		else
#endif
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
			ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_MERCHANT");
#else
			ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_MERCHANT");
#endif
		if(GetUnitBaktun(ePossibleGreatPerson) == 0)
		{
			if(eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_UNITED_NATIONS"))
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
		}
	}
	if(eDesiredGreatPerson == NO_UNIT)
	{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
		ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_GREAT_GENERAL");
#else
		ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_GREAT_GENERAL");
#endif
		if(GetUnitBaktun(ePossibleGreatPerson) == 0)
		{
			if(eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST"))
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
		}
	}

	// No obvious strategic choice, just go for first one available in a reasonable order
	if(eDesiredGreatPerson == NO_UNIT)
	{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
		ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_PROPHET", true);
#else
		ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_PROPHET", true);
#endif
		if(GetUnitBaktun(ePossibleGreatPerson) == 0)
		{
			eDesiredGreatPerson = ePossibleGreatPerson;
		}
		else
		{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
			ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_ENGINEER");
#else
			ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_ENGINEER");
#endif
			if(GetUnitBaktun(ePossibleGreatPerson) == 0)
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
			else
			{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
				ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_WRITER");
#else
				ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_WRITER");
#endif
				if(GetUnitBaktun(ePossibleGreatPerson) == 0)
				{
					eDesiredGreatPerson = ePossibleGreatPerson;
				}
				else
				{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
					ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_MERCHANT");
#else
					ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_MERCHANT");
#endif
					if(GetUnitBaktun(ePossibleGreatPerson) == 0)
					{
						eDesiredGreatPerson = ePossibleGreatPerson;
					}
					else
					{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
						ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_SCIENTIST");
#else
						ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_SCIENTIST");
#endif
						if(GetUnitBaktun(ePossibleGreatPerson) == 0)
						{
							eDesiredGreatPerson = ePossibleGreatPerson;
						}
						else
						{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
							ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_ARTIST");
#else
							ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_ARTIST");
#endif
							if(GetUnitBaktun(ePossibleGreatPerson) == 0)
							{
								eDesiredGreatPerson = ePossibleGreatPerson;
							}
							else
							{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
								ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_MUSICIAN");
#else
								ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_MUSICIAN");
#endif
								if(GetUnitBaktun(ePossibleGreatPerson) == 0)
								{
									eDesiredGreatPerson = ePossibleGreatPerson;
								}
								else
								{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
									ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_GREAT_GENERAL");
#else
									ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_GREAT_GENERAL");
#endif
									if(GetUnitBaktun(ePossibleGreatPerson) == 0)
									{
										eDesiredGreatPerson = ePossibleGreatPerson;
									}
									else
									{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
										ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_GREAT_ADMIRAL");
#else
										ePossibleGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_GREAT_ADMIRAL");
#endif
										if(GetUnitBaktun(ePossibleGreatPerson) == 0)
										{
											eDesiredGreatPerson = ePossibleGreatPerson;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// Actually get the great person
	if(eDesiredGreatPerson != NO_UNIT)
	{
		CvCity* pCity = m_pPlayer->GetGreatPersonSpawnCity(eDesiredGreatPerson);
		if(pCity)
		{
			pCity->GetCityCitizens()->DoSpawnGreatPerson(eDesiredGreatPerson, true, false);
			SetUnitBaktun(eDesiredGreatPerson);
		}
		m_pPlayer->ChangeNumMayaBoosts(-1);
	}
}

/// Converts current game year to Maya date information for use by other functions
void CvPlayerTraits::ComputeMayaDate()
{
	float fCalendarStart = CALENDAR_START;
	int iYear = GC.getGame().getGameTurnYear();
	float fYear = (float)iYear;

	if(fYear >= fCalendarStart)
	{
		// Days since calendar start
		float fDays = (fYear - fCalendarStart) * DAYS_IN_YEAR;
		m_iBaktun = (int)(fDays / DAYS_IN_BAKTUN);
		fDays = fDays - ((float)m_iBaktun * DAYS_IN_BAKTUN);
		m_iKatun = (int)(fDays / DAYS_IN_KATUN);
		fDays = fDays - ((float)m_iKatun * DAYS_IN_KATUN);
		m_iTun = (int)(fDays / DAYS_IN_TUN);
		fDays = fDays - ((float)m_iTun * DAYS_IN_TUN);
		m_iWinal = (int)(fDays / DAYS_IN_WINAL);
		fDays = fDays - ((float)m_iWinal * DAYS_IN_WINAL);
		m_iKin = (int)fDays;
	}
}

/// At the end of which calendar cycle was this unit chosen as a special bonus (0 if none)?
int CvPlayerTraits::GetUnitBaktun(UnitTypes eUnit) const
{
	std::vector<MayaBonusChoice>::const_iterator it;

	// Loop through all units available to tactical AI this turn
	for(it = m_aMayaBonusChoices.begin(); it != m_aMayaBonusChoices.end(); it++)
	{
		if(it->m_eUnitType == eUnit)
		{
			return it->m_iBaktunJustFinished;
		}
	}

	return 0;
}

/// Set calendar cycle when this unit was chosen as a special bonus
void CvPlayerTraits::SetUnitBaktun(UnitTypes eUnit)
{
	MayaBonusChoice choice;
	choice.m_eUnitType = eUnit;
	choice.m_iBaktunJustFinished = m_iBaktun;
	m_aMayaBonusChoices.push_back(choice);
}

/// Have Maya unlocked free choice of Great People?
bool CvPlayerTraits::IsFreeMayaGreatPersonChoice() const
{
	// True if have already selected each GP type once
	int iNumGreatPeopleTypes = 0;
	SpecialUnitTypes eSpecialUnitGreatPerson = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_PEOPLE");

	// Loop through adding the available units
	for(int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(iI);
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
		if(pkUnitClassInfo)
		{
			const UnitTypes eUnit = (UnitTypes)m_pPlayer->getCivilizationInfo().getCivilizationUnits(eUnitClass);
			if (eUnit != NO_UNIT)
			{
				CvUnitEntry* pUnitEntry = GC.getUnitInfo(eUnit);
				if (pUnitEntry)
				{
					if (pUnitEntry->GetSpecialUnitType() == eSpecialUnitGreatPerson)
					{
						iNumGreatPeopleTypes++;
					}
				}
			}	
		}
	}

	return ((int)m_aMayaBonusChoices.size() >= iNumGreatPeopleTypes);
}

// SERIALIZATION METHODS

/// Serialization read
void CvPlayerTraits::Read(FDataStream& kStream)
{
	int iNumEntries;

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	kStream >> m_iGreatPeopleRateModifier;
	kStream >> m_iGreatScientistRateModifier;
	kStream >> m_iGreatGeneralRateModifier;
	kStream >> m_iGreatGeneralExtraBonus;

	kStream >> m_iGreatPersonGiftInfluence;

	kStream >> m_iLevelExperienceModifier;
	kStream >> m_iMaxGlobalBuildingProductionModifier;
	kStream >> m_iMaxTeamBuildingProductionModifier;
	kStream >> m_iMaxPlayerBuildingProductionModifier;
	kStream >> m_iCityUnhappinessModifier;
	kStream >> m_iPopulationUnhappinessModifier;
	kStream >> m_iCityStateBonusModifier;
	kStream >> m_iCityStateFriendshipModifier;
	kStream >> m_iCityStateCombatModifier;
	kStream >> m_iLandBarbarianConversionPercent;
	kStream >> m_iLandBarbarianConversionExtraUnits;
	kStream >> m_iSeaBarbarianConversionPercent;
	kStream >> m_iCapitalBuildingModifier;
	kStream >> m_iPlotBuyCostModifier;
#if defined(MOD_TRAITS_CITY_WORKING)
    MOD_SERIALIZE_READ(23, kStream, m_iCityWorkingChange, 0);
#endif
	kStream >> m_iPlotCultureCostModifier;
	kStream >> m_iCultureFromKills;
	if (uiVersion >= 19)
	{
		kStream >> m_iFaithFromKills;
	}
	else
	{
		m_iFaithFromKills = 0;
	}
	kStream >> m_iCityCultureBonus;

	if (uiVersion >= 17)
	{
		kStream >> m_iCapitalThemingBonusModifier;
	}
	else
	{
		m_iCapitalThemingBonusModifier = 0;
	}

	kStream >> m_iPolicyCostModifier;
	kStream >> m_iCityConnectionTradeRouteChange;
	kStream >> m_iWonderProductionModifier;
	kStream >> m_iPlunderModifier;

	kStream >> m_iImprovementMaintenanceModifier;

	kStream >> m_iGoldenAgeDurationModifier;
	kStream >> m_iGoldenAgeMoveChange;
	kStream >> m_iGoldenAgeCombatModifier;

	if (uiVersion >= 2)
	{
		kStream >> m_iGoldenAgeTourismModifier;
		kStream >> m_iGoldenAgeGreatArtistRateModifier;
		kStream >> m_iGoldenAgeGreatMusicianRateModifier;
		kStream >> m_iGoldenAgeGreatWriterRateModifier;
	}
	else
	{
		m_iGoldenAgeTourismModifier = 0;
		m_iGoldenAgeGreatArtistRateModifier = 0;
		m_iGoldenAgeGreatMusicianRateModifier = 0;
		m_iGoldenAgeGreatWriterRateModifier = 0;
	}

	kStream >> m_iExtraEmbarkMoves;

	kStream >> m_iNaturalWonderFirstFinderGold;

	kStream >> m_iNaturalWonderSubsequentFinderGold;

	kStream >> m_iNaturalWonderYieldModifier;
	kStream >> m_iNaturalWonderHappinessModifier;

	kStream >> m_iNearbyImprovementCombatBonus;
	kStream >> m_iNearbyImprovementBonusRange;

	kStream >> m_iCultureBuildingYieldChange;

	kStream >> m_iCombatBonusVsHigherTech;

	kStream >> m_iCombatBonusVsLargerCiv;

	kStream >> m_iLandUnitMaintenanceModifier;

	kStream >> m_iNavalUnitMaintenanceModifier;

	kStream >> m_iRazeSpeedModifier;

	kStream >> m_iDOFGreatPersonModifier;

	kStream >> m_iLuxuryHappinessRetention;

	kStream >> m_iExtraSpies;

	kStream >> m_iUnresearchedTechBonusFromKills;

	if (uiVersion >= 4)
	{
		kStream >> m_iExtraFoundedCityTerritoryClaimRange;
	}
	else
	{
		m_iExtraFoundedCityTerritoryClaimRange = 0;
	}

	if (uiVersion >= 5)
	{
		kStream >> m_iFreeSocialPoliciesPerEra;
	}
	else
	{
		m_iFreeSocialPoliciesPerEra = 0;
	}

	if (uiVersion >= 6)
	{
		kStream >> m_iNumTradeRoutesModifier;
	}
	else
	{
		m_iNumTradeRoutesModifier = 0;
	}

	if (uiVersion >= 8)
	{
		kStream >> m_iTradeRouteResourceModifier;
	}
	else
	{
		m_iTradeRouteResourceModifier = 0;
	}

	if (uiVersion >= 9)
	{
		kStream >> m_iUniqueLuxuryCities;
		kStream >> m_iUniqueLuxuryQuantity;
	}
	else
	{
		m_iUniqueLuxuryCities = 0;
		m_iUniqueLuxuryQuantity = 0;
	}

	if (uiVersion >= 11)
	{
		kStream >> m_iUniqueLuxuryCitiesPlaced;
	}
	else
	{
		m_iUniqueLuxuryCitiesPlaced = 0;
	}

	if (uiVersion >= 13)
	{
		kStream >> m_iWorkerSpeedModifier;
	}
	else
	{
		m_iWorkerSpeedModifier = 0;
	}

	if (uiVersion >= 14)
	{
		kStream >> m_iAfraidMinorPerTurnInfluence;
	}
	else
	{
		m_iAfraidMinorPerTurnInfluence = 0;
	}
	
	if (uiVersion >= 15)
	{
		kStream >> m_iLandTradeRouteRangeBonus;
		kStream >> m_iTradeReligionModifier;
	}
	else
	{
		m_iLandTradeRouteRangeBonus = 0;
		m_iTradeReligionModifier = 0;
	}

	if (uiVersion >= 16)
	{
		kStream >> m_iTradeBuildingModifier;
	}
	else
	{
		m_iTradeBuildingModifier = 0;
	}

#if defined(MOD_TRAITS_TRADE_ROUTE_BONUSES)
	MOD_SERIALIZE_READ(52, kStream, m_iSeaTradeRouteRangeBonus, 0);
#endif

#if defined(MOD_BALANCE_CORE)
	MOD_SERIALIZE_READ(51, kStream, m_iNumFreeBuildings, 0);
#endif
	kStream >> m_bFightWellDamaged;
	kStream >> m_bMoveFriendlyWoodsAsRoad;
	kStream >> m_bFasterAlongRiver;

	kStream >> m_bFasterInHills;

	kStream >> m_bEmbarkedAllWater;

	kStream >> m_bEmbarkedToLandFlatCost;

	kStream >> m_bNoHillsImprovementMaintenance;

	kStream >> m_bTechBoostFromCapitalScienceBuildings;
	kStream >> m_bStaysAliveZeroCities;

	kStream >> m_bFaithFromUnimprovedForest;

#if defined(MOD_TRAITS_ANY_BELIEF)
	MOD_SERIALIZE_READ(46, kStream, m_bAnyBelief, false);
#endif
	kStream >> m_bBonusReligiousBelief;

	kStream >> m_bAbleToAnnexCityStates;

	kStream >> m_bCrossesMountainsAfterGreatGeneral;
#if defined(MOD_TRAITS_CROSSES_ICE)
	MOD_SERIALIZE_READ(23, kStream, m_bCrossesIce, false);
#endif

	kStream >> m_bMayaCalendarBonuses;
	kStream >> m_iBaktunPreviousTurn;

	kStream >> iNumEntries;
	m_aMayaBonusChoices.clear();
	MayaBonusChoice choice;
	for(int iI = 0; iI < iNumEntries; iI++)
	{
		kStream >> choice.m_eUnitType;
		kStream >> choice.m_iBaktunJustFinished;
		m_aMayaBonusChoices.push_back(choice);
	}

	kStream >> m_bNoAnnexing;
	if (uiVersion >= 3)
	{
		kStream >> m_bTechFromCityConquer;
	}
	else
	{
		m_bTechFromCityConquer = false;
	}

	if (uiVersion >= 9)
	{
		kStream >> m_bUniqueLuxuryRequiresNewArea;
	}
	else
	{
		m_bUniqueLuxuryRequiresNewArea = false;
	}

	if (uiVersion >= 12)
	{
		kStream >> m_bRiverTradeRoad;
	}
	else
	{
		m_bRiverTradeRoad = false;
	}

	if (uiVersion >= 18)
	{
		kStream >> m_bAngerFreeIntrusionOfCityStates;
	}
	else
	{
		m_bAngerFreeIntrusionOfCityStates = false;
	}

	kStream >> m_eCampGuardType;

	kStream >> m_eCombatBonusImprovement;

	ArrayWrapper<int> kExtraYieldThreshold(NUM_YIELD_TYPES, m_iExtraYieldThreshold);
	kStream >> kExtraYieldThreshold;

	ArrayWrapper<int> kFreeCityYield(NUM_YIELD_TYPES, m_iFreeCityYield);
	kStream >> kFreeCityYield;

	ArrayWrapper<int> kYieldChangeResourcesWrapper(NUM_YIELD_TYPES, m_iYieldChangeStrategicResources);
	kStream >> kYieldChangeResourcesWrapper;

	ArrayWrapper<int> kYieldRateModifierWrapper(NUM_YIELD_TYPES, m_iYieldRateModifier);
	kStream >> kYieldRateModifierWrapper;

	ArrayWrapper<int> kYieldChangeNaturalWonderWrapper(NUM_YIELD_TYPES, m_iYieldChangeNaturalWonder);
	kStream >> kYieldChangeNaturalWonderWrapper;

	if (uiVersion >= 7)
	{
		ArrayWrapper<int> kYieldChangePerTradePartnerWrapper(NUM_YIELD_TYPES, m_iYieldChangePerTradePartner);
		kStream >> kYieldChangePerTradePartnerWrapper;

		ArrayWrapper<int> kYieldChangeIncomingTradeRouteWrapper(NUM_YIELD_TYPES, m_iYieldChangeIncomingTradeRoute);
		kStream >> kYieldChangeIncomingTradeRouteWrapper;
	}
	else
	{
		for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
		{
			m_iYieldChangePerTradePartner[iYield] = 0;
			m_iYieldChangeIncomingTradeRoute[iYield] = 0;
		}
	}

	CvAssert(GC.getNumTerrainInfos() == NUM_TERRAIN_TYPES);	// If this is not true, m_iStrategicResourceQuantityModifier must be resized dynamically
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, &m_iStrategicResourceQuantityModifier[0], GC.getNumTerrainInfos());

	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_aiResourceQuantityModifier);

	kStream >> iNumEntries;
	m_abNoTrain.clear();
	for (int i = 0; i < iNumEntries; i++)
	{
		bool bValue;
		kStream >> bValue;
		m_abNoTrain.push_back(bValue);
	}

	kStream >> iNumEntries;
	m_aFreeTraitUnits.clear();
	for(int iI = 0; iI < iNumEntries; iI++)
	{
		FreeTraitUnit trait;
		kStream >> trait.m_iFreeUnit;
		kStream >> trait.m_ePrereqTech;
		m_aFreeTraitUnits.push_back(trait);
	}

	kStream >> iNumEntries;
	for(int iI = 0; iI < iNumEntries; iI++)
	{
		kStream >> m_paiMovesChangeUnitCombat[iI];
	}
	if (uiVersion >= 10)
	{
		for(int iI = 0; iI < iNumEntries; iI++)
		{
			kStream >> m_paiMaintenanceModifierUnitCombat[iI];
		}
	}
	else
	{
		int iNumUnitCombatClassInfos = GC.getNumUnitCombatClassInfos();
		m_paiMaintenanceModifierUnitCombat.resize(iNumUnitCombatClassInfos);
		for(int iI = 0; iI < iNumUnitCombatClassInfos; iI++)
		{
			m_paiMaintenanceModifierUnitCombat[iI] = 0;
		}
	}

	kStream >> m_ppaaiImprovementYieldChange;
	kStream >> m_ppaaiSpecialistYieldChange;

	kStream >> m_ppaaiUnimprovedFeatureYieldChange;

	if (uiVersion >= 11)
	{
		kStream >> iNumEntries;
		m_aUniqueLuxuryAreas.clear();
		for (int iI = 0; iI < iNumEntries; iI++)
		{
			int iAreaID;
			kStream >> iAreaID;
			m_aUniqueLuxuryAreas.push_back(iAreaID);
		}
	}
	else if (uiVersion >= 9)
	{
		kStream >> m_aUniqueLuxuryAreas;
	}
	else
	{
		m_aUniqueLuxuryAreas.clear();
	}
}

/// Serialization write
void CvPlayerTraits::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 19;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	kStream << m_iGreatPeopleRateModifier;
	kStream << m_iGreatScientistRateModifier;
	kStream << m_iGreatGeneralRateModifier;
	kStream << m_iGreatGeneralExtraBonus;
	kStream << m_iGreatPersonGiftInfluence;
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
#if defined(MOD_TRAITS_CITY_WORKING)
    MOD_SERIALIZE_WRITE(kStream, m_iCityWorkingChange);
#endif
	kStream << m_iPlotCultureCostModifier;
	kStream << m_iCultureFromKills;
	kStream << m_iFaithFromKills;
	kStream << m_iCityCultureBonus;
	kStream << m_iCapitalThemingBonusModifier;
	kStream << m_iPolicyCostModifier;
	kStream << m_iCityConnectionTradeRouteChange;
	kStream << m_iWonderProductionModifier;
	kStream << m_iPlunderModifier;
	kStream << m_iImprovementMaintenanceModifier;
	kStream << m_iGoldenAgeDurationModifier;
	kStream << m_iGoldenAgeMoveChange;
	kStream << m_iGoldenAgeCombatModifier;
	kStream << m_iGoldenAgeTourismModifier;
	kStream << m_iGoldenAgeGreatArtistRateModifier;
	kStream << m_iGoldenAgeGreatMusicianRateModifier;
	kStream << m_iGoldenAgeGreatWriterRateModifier;
	kStream << m_iExtraEmbarkMoves;
	kStream << m_iNaturalWonderFirstFinderGold;
	kStream << m_iNaturalWonderSubsequentFinderGold;
	kStream << m_iNaturalWonderYieldModifier;
	kStream << m_iNaturalWonderHappinessModifier;
	kStream << m_iNearbyImprovementCombatBonus;
	kStream << m_iNearbyImprovementBonusRange;
	kStream << m_iCultureBuildingYieldChange;
	kStream << m_iCombatBonusVsHigherTech;
	kStream << m_iCombatBonusVsLargerCiv;
	kStream << m_iLandUnitMaintenanceModifier;
	kStream << m_iNavalUnitMaintenanceModifier;
	kStream << m_iRazeSpeedModifier;
	kStream << m_iDOFGreatPersonModifier;
	kStream << m_iLuxuryHappinessRetention;
	kStream << m_iExtraSpies;
	kStream << m_iUnresearchedTechBonusFromKills;
	kStream << m_iExtraFoundedCityTerritoryClaimRange;
	kStream << m_iFreeSocialPoliciesPerEra;
	kStream << m_iNumTradeRoutesModifier;
	kStream << m_iTradeRouteResourceModifier;
	kStream << m_iUniqueLuxuryCities;
	kStream << m_iUniqueLuxuryQuantity;
	kStream << m_iUniqueLuxuryCitiesPlaced;
	kStream << m_iWorkerSpeedModifier;
	kStream << m_iAfraidMinorPerTurnInfluence;
	kStream << m_iLandTradeRouteRangeBonus;
	kStream << m_iTradeReligionModifier;
	kStream << m_iTradeBuildingModifier;
#if defined(MOD_TRAITS_TRADE_ROUTE_BONUSES)
	MOD_SERIALIZE_WRITE(kStream, m_iSeaTradeRouteRangeBonus);
#endif
#if defined(MOD_BALANCE_CORE)
	MOD_SERIALIZE_WRITE(kStream, m_iNumFreeBuildings);
#endif

	kStream << m_bFightWellDamaged;
	kStream << m_bMoveFriendlyWoodsAsRoad;
	kStream << m_bFasterAlongRiver;
	kStream << m_bFasterInHills;
	kStream << m_bEmbarkedAllWater;
	kStream << m_bEmbarkedToLandFlatCost;
	kStream << m_bNoHillsImprovementMaintenance;
	kStream << m_bTechBoostFromCapitalScienceBuildings;
	kStream << m_bStaysAliveZeroCities;
	kStream << m_bFaithFromUnimprovedForest;
#if defined(MOD_TRAITS_ANY_BELIEF)
	MOD_SERIALIZE_WRITE(kStream, m_bAnyBelief);
#endif
	kStream << m_bBonusReligiousBelief;
	kStream << m_bAbleToAnnexCityStates;
	kStream << m_bCrossesMountainsAfterGreatGeneral;
#if defined(MOD_TRAITS_CROSSES_ICE)
	MOD_SERIALIZE_WRITE(kStream, m_bCrossesIce);
#endif
	kStream << m_bMayaCalendarBonuses;

	kStream << m_iBaktunPreviousTurn;

	std::vector<MayaBonusChoice>::const_iterator it;
	kStream << m_aMayaBonusChoices.size();
	for(it = m_aMayaBonusChoices.begin(); it != m_aMayaBonusChoices.end(); it++)
	{
		kStream << it->m_eUnitType;
		kStream << it->m_iBaktunJustFinished;
	}

	kStream << m_bNoAnnexing;
	kStream << m_bTechFromCityConquer;
	kStream << m_bUniqueLuxuryRequiresNewArea;
	kStream << m_bRiverTradeRoad;
	kStream << m_bAngerFreeIntrusionOfCityStates;

	kStream << m_eCampGuardType;
	kStream << m_eCombatBonusImprovement;

	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iExtraYieldThreshold);
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iFreeCityYield);
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iYieldChangeStrategicResources);
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iYieldRateModifier);
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iYieldChangeNaturalWonder);
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iYieldChangePerTradePartner);
	kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_iYieldChangeIncomingTradeRoute);
	
	CvInfosSerializationHelper::WriteHashedDataArray<TerrainTypes>(kStream, &m_iStrategicResourceQuantityModifier[0], GC.getNumTerrainInfos());
	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes>(kStream, m_aiResourceQuantityModifier);

	kStream << m_abNoTrain.size();
	for (uint ui = 0; ui < m_abNoTrain.size(); ui++)
	{
		kStream << m_abNoTrain[ui];
	}

	kStream << m_aFreeTraitUnits.size();
	for(uint ui = 0; ui < m_aFreeTraitUnits.size(); ui++)
	{
		kStream << m_aFreeTraitUnits[ui].m_iFreeUnit;
		kStream << m_aFreeTraitUnits[ui].m_ePrereqTech;
	}

	int iNumUnitCombatClassInfos = GC.getNumUnitCombatClassInfos();
	kStream << 	iNumUnitCombatClassInfos;
	for(int iI = 0; iI < iNumUnitCombatClassInfos; iI++)
	{
		kStream << m_paiMovesChangeUnitCombat[iI];
	}
	for(int iI = 0; iI < iNumUnitCombatClassInfos; iI++)
	{
		kStream << m_paiMaintenanceModifierUnitCombat[iI];
	}

	kStream << m_ppaaiImprovementYieldChange;
	kStream << m_ppaaiSpecialistYieldChange;
	kStream << m_ppaaiUnimprovedFeatureYieldChange;

	kStream << (int)m_aUniqueLuxuryAreas.size();
	for (unsigned int iI = 0; iI < m_aUniqueLuxuryAreas.size(); iI++)
	{
		kStream << m_aUniqueLuxuryAreas[iI];
	}
}

// PRIVATE METHODS

/// Is there an adjacent barbarian camp that could be converted?
bool CvPlayerTraits::ConvertBarbarianCamp(CvPlot* pPlot)
{
	UnitHandle pGiftUnit;

	// Has this camp already decided not to convert?
	if(pPlot->IsBarbarianCampNotConverting())
	{
		return false;
	}

	// Roll die to see if it converts
	if(GC.getGame().getJonRandNum(100, "Barbarian Camp Conversion") < m_iLandBarbarianConversionPercent)
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
		for(int iI = 0; iI < m_iLandBarbarianConversionExtraUnits; iI++)
		{
			pGiftUnit = m_pPlayer->initUnit(m_eCampGuardType, pPlot->getX(), pPlot->getY(), NO_UNITAI, NO_DIRECTION, true /*bNoMove*/);
			if (!pGiftUnit->jumpToNearestValidPlot())
				pGiftUnit->kill(false);
			else
				pGiftUnit->finishMoves();
		}

		if(GC.getLogging() && GC.getAILogging())
		{
			CvString logMsg;
			logMsg.Format("Converted barbarian camp, X: %d, Y: %d", pPlot->getX(), pPlot->getY());
			m_pPlayer->GetHomelandAI()->LogHomelandMessage(logMsg);
		}

		CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BARB_CAMP_CONVERTS");
		CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BARB_CAMP_CONVERTS");
		m_pPlayer->GetNotifications()->Add(NOTIFICATION_GENERIC, strBuffer, strSummary, pPlot->getX(), pPlot->getY(), -1);
#if !defined(NO_ACHIEVEMENTS)
		//Increase Stat
		if(m_pPlayer->isHuman() &&!GC.getGame().isGameMultiPlayer())
		{
			gDLL->IncrementSteamStatAndUnlock(ESTEAMSTAT_BARBSCONVERTED, 10, ACHIEVEMENT_SPECIAL_BARBARIANWARLORD);
		}
#endif
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
	if(pUnit->IsNotConverting())
	{
		return false;
	}

	// Roll die to see if it converts
	if(GC.getGame().getJonRandNum(100, "Barbarian Naval Unit Conversion") < m_iSeaBarbarianConversionPercent)
	{
		int iNumGold = /*25*/ GC.getGOLD_FROM_BARBARIAN_CONVERSION();
		m_pPlayer->GetTreasury()->ChangeGold(iNumGold);

		// Convert the barbarian into our unit
		pGiftUnit = m_pPlayer->initUnit(pUnit->getUnitType(), pUnit->getX(), pUnit->getY(), pUnit->AI_getUnitAIType(), NO_DIRECTION, true /*bNoMove*/, false);
		CvAssertMsg(pGiftUnit, "GiftUnit is not assigned a valid value");
		pGiftUnit->convert(pUnit.pointer(), false);
		pGiftUnit->setupGraphical();
		pGiftUnit->finishMoves(); // No move first turn

#if !defined(NO_ACHIEVEMENTS)
		// Validate that the achievement is reached by a live human and active player at the same time
		if(m_pPlayer->isHuman() && !GC.getGame().isGameMultiPlayer() && m_pPlayer->getLeaderInfo().GetType() && _stricmp(m_pPlayer->getLeaderInfo().GetType(), "LEADER_SULEIMAN") == 0)
		{
			gDLL->IncrementSteamStatAndUnlock(ESTEAMSTAT_BARBSNAVALCONVERTED, 10, ACHIEVEMENT_SPECIAL_BARBARYPIRATE);
		}
#endif

		if(GC.getLogging() && GC.getAILogging())
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
