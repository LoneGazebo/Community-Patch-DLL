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
#include "CvInternalGameCoreUtils.h"
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
	m_iNationalPopReqModifier(0),
	m_iCityWorkingChange(0),
	m_iCityAutomatonWorkersChange(0),
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
	m_iRouteBuilderCostModifier(0),
	m_iGoldenAgeDurationModifier(0),
	m_iGoldenAgeMoveChange(0),
	m_iGoldenAgeCombatModifier(0),
	m_iGoldenAgeTourismModifier(0),
	m_iGoldenAgeGreatArtistRateModifier(0),
	m_iGoldenAgeGreatMusicianRateModifier(0),
	m_iGoldenAgeGreatWriterRateModifier(0),
	m_iObsoleteTech(NO_TECH),
	m_iPrereqTech(NO_TECH),
	m_iObsoleteBelief(NO_BELIEF),
	m_iPrereqBelief(NO_BELIEF),
	m_iObsoletePolicy(NO_POLICY),
	m_iPrereqPolicy(NO_POLICY),
	m_iExtraEmbarkMoves(0),
	m_iFreeUnitClassType(NO_UNITCLASS),
	m_iNaturalWonderFirstFinderGold(0),
	m_iNaturalWonderSubsequentFinderGold(0),
	m_iNaturalWonderYieldModifier(0),
	m_iNaturalWonderHappinessModifier(0),
	m_iNearbyImprovementCombatBonus(0),
	m_iNearbyImprovementBonusRange(0),
	m_iCultureBuildingYieldChange(0),
	m_iWarWearinessModifier(0),
	m_iEnemyWarWearinessModifier(0),
	m_iCombatBonusVsHigherPop(0),
	m_bBuyOwnedTiles(false),
	m_bNewCitiesStartWithCapitalReligion(false),
	m_bNoSpread(false),
	m_iXPBonusFromGreatPersonBirth(0),
	m_iUnitHealFromGreatPersonBirth(0),
	m_iBullyMilitaryStrengthModifier(0),
	m_iBullyValueModifier(0),
	m_bIgnoreBullyPenalties(false),
	m_bDiplomaticMarriage(false),
	m_bAdoptionFreeTech(false),
	m_bGPWLTKD(false),
	m_bGreatWorkWLTKD(false),
	m_bExpansionWLTKD(false),
	m_bTradeRouteOnly(false),
	m_bKeepConqueredBuildings(false),
	m_iWLTKDGPImprovementModifier(0),
	m_iGrowthBoon(0),
	m_bMountainPass(false),
	m_bUniqueBeliefsOnly(false),
	m_iAllianceCSDefense(0),
	m_iAllianceCSStrength(0),
	m_iTourismGABonus(0),
	m_bNoNaturalReligionSpread(false),
	m_bNoOpenTrade(false),
	m_bGoldenAgeOnWar(false),
	m_bUnableToCancelRazing(false),
	m_iTourismToGAP(0),
	m_iGoldToGAP(0),
	m_iInfluenceMeetCS(0),
	m_iMultipleAttackBonus(0),
	m_iCityConquestGWAM(0),
	m_iEventTourismBoost(0),
	m_iEventGP(0),
	m_iWLTKDCulture(0),
	m_iWLTKDGATimer(0),
	m_iWLTKDUnhappinessNeedsMod(0),
	m_iStartingSpies(0),
	m_iSpyOffensiveStrengthModifier(0),
	m_iSpyMoveRateBonus(0),
	m_iSpySecurityModifier(0),
	m_iSpyExtraRankBonus(0),
	m_iQuestYieldModifier(0),
	m_iWonderProductionModifierToBuilding(0),
	m_iPolicyGEorGM(0),
	m_iGAGarrisonCityRangeStrikeModifier(0),
	m_bBestUnitSpawnOnImpDOW(false),
	m_iBestUnitImprovement(NO_IMPROVEMENT),
	m_iGGGARateFromDenunciationsAndWars(0),
	m_bTradeRouteMinorInfluenceAP(false),
	m_bProdModFromNumSpecialists(false),
	m_iConquestOfTheWorldCityAttack(0),
	m_bConquestOfTheWorld(false),
	m_bFreeUpgrade(false),
	m_bWarsawPact(false),
	m_iEnemyWarSawPactPromotion(NO_PROMOTION),
	m_bFreeZuluPikemanToImpi(false),
	m_bPermanentYieldsDecreaseEveryEra(false),
	m_bImportsCountTowardsMonopolies(false),
	m_bCanPurchaseNavalUnitsFaith(false),
	m_iPuppetPenaltyReduction(0),
	m_iSharedReligionTourismModifier(0),
	m_iExtraMissionaryStrength(0),
	m_bCanGoldInternalTradeRoutes(false),
	m_bAnnexedCityStatesGiveYields(false),
	m_iExtraTradeRoutesPerXOwnedCities(0),
	m_iExtraTradeRoutesPerXOwnedVassals(0),
	m_iMinorInfluencePerGiftedUnit(0),
	m_bIsCapitalOnly(false),
	m_iInvestmentModifier(0),
	m_iCombatBonusVsHigherTech(0),
	m_iCombatBonusVsLargerCiv(0),
	m_iLandUnitMaintenanceModifier(0),
	m_iNavalUnitMaintenanceModifier(0),
	m_iRazeSpeedModifier(0),
	m_iDOFGreatPersonModifier(0),
	m_iLuxuryHappinessRetention(0),
	m_iExtraSupply(0),
	m_iExtraSupplyPerCity(0),
	m_iExtraSupplyPerPopulation(0),
	m_iExtraSpies(0),
	m_iUnresearchedTechBonusFromKills(0),
	m_iExtraFoundedCityTerritoryClaimRange(0),
	m_iExtraConqueredCityTerritoryClaimRange(0),
	m_iExtraTenetsFirstAdoption(0),
	m_iMonopolyModFlat(0),
	m_iMonopolyModPercent(0),
	m_iFreeSocialPoliciesPerEra(0),
	m_iNumTradeRoutesModifier(0),
	m_iTradeRouteResourceModifier(0),
	m_iUniqueLuxuryCities(0),
	m_iUniqueLuxuryQuantity(0),
	m_iWorkerSpeedModifier(0),
	m_iAfraidMinorPerTurnInfluence(0),
	m_iLandTradeRouteRangeBonus(0),
	m_iSeaTradeRouteRangeBonus(0),
	m_iTradeReligionModifier(0),
	m_iTradeBuildingModifier(0),

	m_eFreeUnitPrereqTech(NO_TECH),
	m_eFreeBuilding(NO_BUILDING),
	m_eFreeCapitalBuilding(NO_BUILDING),
	m_eFreeBuildingPrereqTech(NO_TECH),
	m_eCapitalFreeBuildingPrereqTech(NO_TECH),
	m_eFreeUnitOnConquest(NO_UNIT),
	m_eFreeBuildingOnConquest(NO_BUILDING),
	m_bBullyAnnex(false),
	m_iBullyYieldMultiplierAnnex(0),
	m_bFightWellDamaged(false),
	m_bWoodlandMovementBonus(false),
	m_bRiverMovementBonus(false),
	m_bFasterInHills(false),
	m_bEmbarkedAllWater(false),
	m_bEmbarkedToLandFlatCost(false),
	m_bNoHillsImprovementMaintenance(false),
	m_bTechBoostFromCapitalScienceBuildings(false),
	m_bArtistGoldenAgeTechBoost(false),
	m_bStaysAliveZeroCities(false),
	m_bFaithFromUnimprovedForest(false),
	m_bAnyBelief(false),
	m_bAlwaysReligion(false),
	m_bNoTradeRouteProximityPenalty(false),
	m_bCanPlunderWithoutWar(false),
	m_bBonusReligiousBelief(false),
	m_bAbleToAnnexCityStates(false),
	m_bCrossesMountainsAfterGreatGeneral(false),
	m_bCrossesIce(false),
	m_bGGFromBarbarians(false),
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
	m_piNumFreeResourceOnWorldWonderCompletion(NULL),
	m_ppiImprovementYieldChanges(NULL),
	m_ppiPlotYieldChanges(NULL),
	m_paiGAPToYield(NULL),
	m_paiMountainRangeYield(NULL),
	m_piYieldFromLevelUp(NULL),
	m_piYieldFromHistoricEvent(NULL),
	m_piYieldFromOwnPantheon(NULL),
	m_piYieldFromXMilitaryUnits(NULL),
	m_tradeRouteEndYield(),
	m_piYieldFromRouteMovement(NULL),
	m_piYieldFromExport(NULL),
	m_piYieldFromImport(NULL),
	m_piYieldFromTilePurchase(NULL),
	m_piYieldFromTileEarn(NULL),
	m_piYieldFromCSAlly(NULL),
	m_piYieldFromCSFriend(NULL),
	m_piYieldFromSettle(NULL),
	m_piYieldFromConquest(NULL),
	m_piYieldFromCityDamageTimes100(NULL),
	m_iPurchasedUnitsBonusXP(0),
	m_iVotePerXCSAlliance(0),
	m_iVotePerXCSFollowingFollowingYourReligion(0),
	m_iChanceToConvertReligiousUnits(0),
	m_iGoldenAgeFromVictory(0),
	m_iFreePolicyPerXTechs(0),
	m_eGPFaithPurchaseEra(NO_ERA),
	m_iFaithCostModifier(0),
	m_bFreeGreatWorkOnConquest(false),
	m_bPopulationBoostReligion(false),
	m_bStartsWithPantheon(false),
	m_bProphetFervor(false),
	m_bCombatBoostNearNaturalWonder(false),
	m_piNumPledgesDomainProdMod(NULL),
	m_piFreeUnitClassesDOW(NULL),
	m_piDomainFreeExperienceModifier(NULL),
	m_ppiYieldFromTileEarnTerrainType(NULL),
	m_ppiYieldFromTilePurchaseTerrainType(NULL),
	m_ppiYieldFromTileConquest(NULL),
	m_ppiYieldFromTileCultureBomb(NULL),
	m_ppiYieldFromTileStealCultureBomb(NULL),
	m_ppiYieldFromTileSettle(NULL),
	m_ppiYieldChangePerImprovementBuilt(NULL),
	m_pbiYieldFromBarbarianCampClear(),
	m_pbiYieldFromRouteMovementInForeignTerritory(),
	m_ppiBuildingClassYieldChanges(NULL),
	m_piCapitalYieldChanges(NULL),
	m_piCityYieldChanges(NULL),
	m_piPermanentYieldChangeWLTKD(NULL),
	m_piCoastalCityYieldChanges(NULL),
	m_piGreatWorkYieldChanges(NULL),
	m_piArtifactYieldChanges(NULL),
	m_piArtYieldChanges(NULL),
	m_piLitYieldChanges(NULL),
	m_piMusicYieldChanges(NULL),
	m_piSeaPlotYieldChanges(NULL),
	m_ppiFeatureYieldChanges(NULL),
	m_ppiResourceYieldChanges(NULL),
	m_piLuxuryYieldChanges(NULL),
	m_miResourceYieldChangesFromGoldenAge(),
	m_miResourceYieldChangesFromGoldenAgeCap(),
	m_ppiTerrainYieldChanges(NULL),
	m_piYieldFromKills(NULL),
	m_piYieldFromBarbarianKills(NULL),
	m_piYieldFromMinorDemand(NULL),
	m_piYieldFromLuxuryResourceGain(NULL),
	m_piYieldChangeTradeRoute(NULL),
	m_piYieldChangeWorldWonder(NULL),
	m_ppiTradeRouteYieldChange(NULL),
	m_ppiSpecialistYieldChanges(NULL),
	m_ppiGreatPersonExpendedYield(NULL),
	m_ppiGreatPersonBornYield(NULL),
	m_piGoldenAgeGreatPersonRateModifier(NULL),
	m_piGreatPersonCostReduction(NULL),
	m_piPerPuppetGreatPersonRateModifier(NULL),
	m_piGreatPersonGWAM(NULL),
	m_ppiCityYieldFromUnimprovedFeature(NULL),
	m_piGoldenAgeFromGreatPersonBirth(NULL),
	m_piGreatPersonProgressFromPolicyUnlock(NULL),
	m_piGreatPersonProgressFromKills(),
	m_piRandomGreatPersonProgressFromKills(),
	m_ppiUnimprovedFeatureYieldChanges(NULL)
{
}

/// Destructor
CvTraitEntry::~CvTraitEntry()
{
	CvDatabaseUtility::SafeDelete2DArray(m_ppiImprovementYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPlotYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiBuildingClassYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiFeatureYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiResourceYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiTerrainYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiTradeRouteYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiSpecialistYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiGreatPersonExpendedYield);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiGreatPersonBornYield);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiCityYieldFromUnimprovedFeature);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiUnimprovedFeatureYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiYieldFromTileEarnTerrainType);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiYieldFromTilePurchaseTerrainType);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiYieldFromTileConquest);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiYieldFromTileCultureBomb);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiYieldFromTileStealCultureBomb);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiYieldFromTileSettle);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiYieldChangePerImprovementBuilt);
	m_pbiYieldFromBarbarianCampClear.clear();
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
/// Accessor:: discount when constructing a building based on Wonder Production Modifier present
int CvTraitEntry::GetWonderProductionModifierToBuilding() const
{
	return m_iWonderProductionModifierToBuilding;
}
int CvTraitEntry::GetGoldenAgeGarrisonedCityRangeStrikeModifier() const
{
	return m_iGAGarrisonCityRangeStrikeModifier;
}
bool CvTraitEntry::IsBestUnitSpawnOnImprovementDOW() const
{
	return m_bBestUnitSpawnOnImpDOW;
}
ImprovementTypes CvTraitEntry::GetBestSpawnUnitImprovement() const
{
	return m_iBestUnitImprovement;
}
int CvTraitEntry::GetGGGARateFromDenunciationsAndWars() const
{
	return m_iGGGARateFromDenunciationsAndWars;
}
/// Accessor:: cheaper purchase of tiles for culture border expansion
int CvTraitEntry::GetPlotBuyCostModifier() const
{
	return m_iPlotBuyCostModifier;
}
int CvTraitEntry::GetNationalPopReqModifier() const
{
	return m_iNationalPopReqModifier;
}

/// Accessor:: greater border expansion
int CvTraitEntry::GetCityWorkingChange() const
{
	return m_iCityWorkingChange;
}

/// Accessor:: automaton workers
int CvTraitEntry::GetCityAutomatonWorkersChange() const
{
	return m_iCityAutomatonWorkersChange;
}

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

/// Accessor:: percent change to builder cost of roads
int CvTraitEntry::GetRouteBuilderCostModifier() const
{
	return m_iRouteBuilderCostModifier;
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

/// Accessor: combat bonus vs. civ with more citizens
int CvTraitEntry::GetWarWearinessModifier() const
{
	return m_iWarWearinessModifier;
}
int CvTraitEntry::GetEnemyWarWearinessModifier() const
{
	return m_iEnemyWarWearinessModifier;
}

int CvTraitEntry::GetCombatBonusVsHigherPop() const
{
	return m_iCombatBonusVsHigherPop;
}
bool CvTraitEntry::IsBuyOwnedTiles() const
{
	return m_bBuyOwnedTiles;
}

bool CvTraitEntry::IsNewCitiesStartWithCapitalReligion() const
{
	return m_bNewCitiesStartWithCapitalReligion;
}

bool CvTraitEntry::IsForeignReligionSpreadImmune() const
{
	return m_bNoSpread;
}

int CvTraitEntry::GetXPBonusFromGreatPersonBirth() const
{
	return m_iXPBonusFromGreatPersonBirth;
}

int CvTraitEntry::GetUnitHealFromGreatPersonBirth() const
{
	return m_iUnitHealFromGreatPersonBirth;
}

int CvTraitEntry::GetBullyMilitaryStrengthModifier() const
{
	return m_iBullyMilitaryStrengthModifier;
}
int CvTraitEntry::GetBullyValueModifier() const
{
	return m_iBullyValueModifier;
}

bool CvTraitEntry::IgnoreBullyPenalties() const
{
	return m_bIgnoreBullyPenalties;
}

bool CvTraitEntry::IsDiplomaticMarriage() const
{
	return m_bDiplomaticMarriage;
}
bool CvTraitEntry::IsAdoptionFreeTech() const
{
	return m_bAdoptionFreeTech;
}
bool CvTraitEntry::IsGPWLTKD() const
{
	return m_bGPWLTKD;
}
bool CvTraitEntry::IsGreatWorkWLTKD() const
{
	return m_bGreatWorkWLTKD;
}
bool CvTraitEntry::IsExpansionWLTKD() const
{
	return m_bExpansionWLTKD;
}
bool CvTraitEntry::IsTradeRouteOnly() const
{
	return m_bTradeRouteOnly;
}
bool CvTraitEntry::IsKeepConqueredBuildings() const
{
	return m_bKeepConqueredBuildings;
}
bool CvTraitEntry::IsMountainPass() const
{
	return m_bMountainPass;
}
int CvTraitEntry::GetWLTKDGPImprovementModifier() const
{
	return m_iWLTKDGPImprovementModifier;
}
int CvTraitEntry::GetGrowthBoon() const
{
	return m_iGrowthBoon;
}
int CvTraitEntry::GetAllianceCSDefense() const
{
	return m_iAllianceCSDefense;
}
int CvTraitEntry::GetAllianceCSStrength() const
{
	return m_iAllianceCSStrength;
}
bool CvTraitEntry::IsUniqueBeliefsOnly() const
{
	return m_bUniqueBeliefsOnly;
}
bool CvTraitEntry::IsNoNaturalReligionSpread() const
{
	return m_bNoNaturalReligionSpread;
}
bool CvTraitEntry::IsNoOpenTrade() const
{
	return m_bNoOpenTrade;
}
bool CvTraitEntry::IsGoldenAgeOnWar() const
{
	return m_bGoldenAgeOnWar;
}

bool CvTraitEntry::IsUnableToCancelRazing() const
{
	return m_bUnableToCancelRazing;
}

int CvTraitEntry::GetTourismGABonus() const
{
	return m_iTourismGABonus;
}
int CvTraitEntry::GetTourismToGAP() const
{
	return m_iTourismToGAP;
}
int CvTraitEntry::GetGoldToGAP() const
{
	return m_iGoldToGAP;
}
int CvTraitEntry::GetInfluenceMeetCS() const
{
	return m_iInfluenceMeetCS;
}
int CvTraitEntry::GetMultipleAttackBonus() const
{
	return m_iMultipleAttackBonus;
}
int CvTraitEntry::GetCityConquestGWAM() const
{
	return m_iCityConquestGWAM;
}
int CvTraitEntry::GetEventTourismBoost() const
{
	return m_iEventTourismBoost;
}
int CvTraitEntry::GetEventGP() const
{
	return m_iEventGP;
}
int CvTraitEntry::GetWLTKDCulture() const
{
	return m_iWLTKDCulture;
}
int CvTraitEntry::GetWLTKDGATimer() const
{
	return m_iWLTKDGATimer;
}
int CvTraitEntry::GetWLTKDUnhappinessNeedsMod() const
{
	return m_iWLTKDUnhappinessNeedsMod;
}
int CvTraitEntry::GetStartingSpies() const
{
	return m_iStartingSpies;
}
int CvTraitEntry::GetSpyOffensiveStrengthModifier() const
{
	return m_iSpyOffensiveStrengthModifier;
}
int CvTraitEntry::GetSpySecurityModifier() const
{
	return m_iSpySecurityModifier;
}

int CvTraitEntry::GetSpyExtraRankBonus() const
{
	return m_iSpyExtraRankBonus;
}
int CvTraitEntry::GetSpyMoveRateBonus() const
{
	return m_iSpyMoveRateBonus;
}
int CvTraitEntry::GetQuestYieldModifier() const
{
	return m_iQuestYieldModifier;
}
int CvTraitEntry::GetPolicyGEorGM() const
{
	return m_iPolicyGEorGM;
}
bool CvTraitEntry::IsTradeRouteMinorInfluenceAdmiralPoints() const
{
	return m_bTradeRouteMinorInfluenceAP;
}
bool CvTraitEntry::IsProductionModFromNumSpecialists() const
{
	return m_bProdModFromNumSpecialists;
}
int CvTraitEntry::GetConquestOfTheWorldCityAttack() const
{
	return m_iConquestOfTheWorldCityAttack;
}
bool CvTraitEntry::IsConquestOfTheWorld() const
{
	return m_bConquestOfTheWorld;
}
bool CvTraitEntry::IsFreeUpgrade() const
{
	return m_bFreeUpgrade;
}
bool CvTraitEntry::IsWarsawPact() const
{
	return m_bWarsawPact;
}
PromotionTypes CvTraitEntry::GetEnemyWarSawPactPromotion() const
{
	return (PromotionTypes)m_iEnemyWarSawPactPromotion;
}
bool CvTraitEntry::IsFreeZuluPikemanToImpi() const
{
	return m_bFreeZuluPikemanToImpi;
}
bool CvTraitEntry::IsPermanentYieldsDecreaseEveryEra() const
{
	return m_bPermanentYieldsDecreaseEveryEra;
}
bool CvTraitEntry::IsImportsCountTowardsMonopolies() const
{
	return m_bImportsCountTowardsMonopolies;
}
bool CvTraitEntry::IsCanPurchaseNavalUnitsFaith() const
{
	return m_bCanPurchaseNavalUnitsFaith;
}
int CvTraitEntry::GetPuppetPenaltyReduction() const
{
	return m_iPuppetPenaltyReduction;
}
/// Boost to tourism bonus for shared religion (same as the policy one)
int CvTraitEntry::GetSharedReligionTourismModifier() const
{
	return m_iSharedReligionTourismModifier;
}
/// Missionaries gain % more strength
int CvTraitEntry::GetExtraMissionaryStrength() const
{
	return m_iExtraMissionaryStrength;
}
/// Can send internal trade routes which are calculated as if international (yields gold)
bool CvTraitEntry::IsCanGoldInternalTradeRoutes() const
{
	return m_bCanGoldInternalTradeRoutes;
}
/// Conquered City-States continue to give bonus yields
bool CvTraitEntry::IsAnnexedCityStatesGiveYields() const
{
	return m_bAnnexedCityStatesGiveYields;
}
int CvTraitEntry::GetExtraTradeRoutesPerXOwnedCities() const
{
	return m_iExtraTradeRoutesPerXOwnedCities;
}
int CvTraitEntry::GetExtraTradeRoutesPerXOwnedVassals() const
{
	return m_iExtraTradeRoutesPerXOwnedVassals;
}
/// Gifting units to City-States awards Influence per turn while the units remain alive
int CvTraitEntry::GetMinorInfluencePerGiftedUnit() const
{
	return m_iMinorInfluencePerGiftedUnit;
}
bool CvTraitEntry::IsCapitalOnly() const
{
	return m_bIsCapitalOnly;
}

int CvTraitEntry::GetInvestmentModifier() const
{
	return m_iInvestmentModifier;
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

/// Accessor: number of extra base units supplied
int CvTraitEntry::GetExtraSupply() const
{
	return m_iExtraSupply;
}

/// Accessor: number of extra units supplied per city
int CvTraitEntry::GetExtraSupplyPerCity() const
{
	return m_iExtraSupplyPerCity;
}

/// Accessor: number of extra units supplied per population (percentage)
int CvTraitEntry::GetExtraSupplyPerPopulation() const
{
	return m_iExtraSupplyPerPopulation;
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
int CvTraitEntry::GetExtraConqueredCityTerritoryClaimRange() const
{
	return m_iExtraConqueredCityTerritoryClaimRange;
}
int CvTraitEntry::GetExtraTenetsFirstAdoption() const
{
	return m_iExtraTenetsFirstAdoption;
}
int CvTraitEntry::GetMonopolyModFlat() const
{
	return m_iMonopolyModFlat;
}
int CvTraitEntry::GetMonopolyModPercent() const
{
	return m_iMonopolyModPercent;
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

int CvTraitEntry::GetSeaTradeRouteRangeBonus() const
{
	return m_iSeaTradeRouteRangeBonus;
}

int CvTraitEntry::GetTradeReligionModifier() const
{
	return m_iTradeReligionModifier;
}

int CvTraitEntry::GetTradeBuildingModifier() const
{
	return m_iTradeBuildingModifier;
}
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
/// Accessor:: does this civ get a free great work when it conquers a city?
bool CvTraitEntry::IsFreeGreatWorkOnConquest() const
{
	return m_bFreeGreatWorkOnConquest;
}
bool CvTraitEntry::IsPopulationBoostReligion() const
{
	return m_bPopulationBoostReligion;
}
bool CvTraitEntry::StartsWithPantheon() const
{
	return m_bStartsWithPantheon;
}
bool CvTraitEntry::IsProphetFervor() const
{
	return m_bProphetFervor;
}
bool CvTraitEntry::IsCombatBoostNearNaturalWonder() const
{
	return m_bCombatBoostNearNaturalWonder;
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

/// Accessor: free building in each city
BuildingTypes CvTraitEntry::GetFreeBuilding() const
{
	return m_eFreeBuilding;
}
/// Does the capital get a free building?
BuildingTypes CvTraitEntry::GetFreeCapitalBuilding() const
{
	return m_eFreeCapitalBuilding;
}
/// Does this civ get a free unit when it conquers a city?
UnitTypes CvTraitEntry::GetFreeUnitOnConquest() const
{
	return m_eFreeUnitOnConquest;
}
/// Accessor: free building in each city conquered
BuildingTypes CvTraitEntry::GetFreeBuildingOnConquest() const
{
	return m_eFreeBuildingOnConquest;
}
bool CvTraitEntry::IsBullyAnnex() const
{
	return m_bBullyAnnex;
}
int CvTraitEntry::GetBullyYieldMultiplierAnnex() const
{
	return m_iBullyYieldMultiplierAnnex;
}
/// Accessor:: does this civ get combat bonuses when damaged?
bool CvTraitEntry::IsFightWellDamaged() const
{
	return m_bFightWellDamaged;
}

/// Accessor:: does this civ move units through forest as if it is road?
bool CvTraitEntry::IsWoodlandMovementBonus() const
{
	return m_bWoodlandMovementBonus;
}

/// Accessor:: does this civ move along rivers like a Scout?
bool CvTraitEntry::IsRiverMovementBonus() const
{
	return m_bRiverMovementBonus;
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
/// Artists Open Golden Age to Receive Current Median One-Time Research Points in Researchable Technology
bool CvTraitEntry::IsArtistGoldenAgeTechBoost() const
{
	return m_bArtistGoldenAgeTechBoost;
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

/// Accessor: can this civ have any belief?
bool CvTraitEntry::IsAnyBelief() const
{
	return m_bAnyBelief;
}
bool CvTraitEntry::IsAlwaysReligion() const
{
	return m_bAlwaysReligion;
}
bool CvTraitEntry::IsNoTradeRouteProximityPenalty() const
{
	return m_bNoTradeRouteProximityPenalty;
}
bool CvTraitEntry::IsCanPlunderWithoutWar() const
{
	return m_bCanPlunderWithoutWar;
}

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

/// Accessor: do combat units have the ability to cross ice?
bool CvTraitEntry::IsCrossesIce() const
{
	return m_bCrossesIce;
}

/// Accessor: do combat units gain GG/GA points from killing barbarians?
bool CvTraitEntry::IsGGFromBarbarians() const
{
	return m_bGGFromBarbarians;
}

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

//Traits for affecting city happiness
int CvTraitEntry::GetBasicNeedsMedianModifier() const
{
	return m_iBasicNeedsMedianModifier;
}
int CvTraitEntry::GetGoldMedianModifier() const
{
	return m_iGoldMedianModifier;
}
int CvTraitEntry::GetScienceMedianModifier() const
{
	return m_iScienceMedianModifier;
}
int CvTraitEntry::GetCultureMedianModifier() const
{
	return m_iCultureMedianModifier;
}
int CvTraitEntry::GetReligiousUnrestModifier() const
{
	return m_iReligiousUnrestModifier;
}
bool CvTraitEntry::IsNoConnectionUnhappiness() const
{
	return m_bNoConnectionUnhappiness;
}
bool CvTraitEntry::IsNoReligiousStrife() const
{
	return m_bIsNoReligiousStrife;
}
bool CvTraitEntry::IsOddEraScaler() const
{
	return m_bIsOddEraScaler;
}
int CvTraitEntry::GetWonderProductionModGA() const
{
	return m_iWonderProductionModGA;
}
int CvTraitEntry::GetCultureBonusModifierConquest() const
{
	return m_iCultureBonusModifierConquest;
}
int CvTraitEntry::GetProductionBonusModifierConquest() const
{
	return m_iProductionBonusModifierConquest;
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
	PRECONDITION(i < GC.getNumResourceInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piResourceQuantityModifiers ? m_piResourceQuantityModifiers[i] : -1;
}

/// Accessor: Amount of a specific resource awarded on World Wonder completion
int CvTraitEntry::GetNumFreeResourceOnWorldWonderCompletion(int i) const
{
	PRECONDITION(i < GC.getNumResourceInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piNumFreeResourceOnWorldWonderCompletion ? m_piNumFreeResourceOnWorldWonderCompletion[i] : 0;
}

/// Accessor:: Extra yield from an improvement
int CvTraitEntry::GetImprovementYieldChanges(ImprovementTypes eIndex1, YieldTypes eIndex2) const
{
	PRECONDITION(eIndex1 < GC.getNumImprovementInfos(), "Index out of bounds");
	PRECONDITION(eIndex1 > -1, "Index out of bounds");
	PRECONDITION(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex2 > -1, "Index out of bounds");
	return m_ppiImprovementYieldChanges ? m_ppiImprovementYieldChanges[eIndex1][eIndex2] : 0;
}

int CvTraitEntry::GetYieldFromTileEarnTerrainType(TerrainTypes eIndex1, YieldTypes eIndex2) const
{
	PRECONDITION(eIndex1 < GC.getNumTerrainInfos(), "Index out of bounds");
	PRECONDITION(eIndex1 > -1, "Index out of bounds");
	PRECONDITION(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex2 > -1, "Index out of bounds");
	return m_ppiYieldFromTileEarnTerrainType ? m_ppiYieldFromTileEarnTerrainType[eIndex1][eIndex2] : 0;
}

int CvTraitEntry::GetYieldFromTilePurchaseTerrainType(TerrainTypes eIndex1, YieldTypes eIndex2) const
{
	PRECONDITION(eIndex1 < GC.getNumTerrainInfos(), "Index out of bounds");
	PRECONDITION(eIndex1 > -1, "Index out of bounds");
	PRECONDITION(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex2 > -1, "Index out of bounds");
	return m_ppiYieldFromTilePurchaseTerrainType ? m_ppiYieldFromTilePurchaseTerrainType[eIndex1][eIndex2] : 0;
}

int CvTraitEntry::GetYieldFromTileConquest(TerrainTypes eIndex1, YieldTypes eIndex2) const
{
	PRECONDITION(eIndex1 < GC.getNumTerrainInfos(), "Index out of bounds");
	PRECONDITION(eIndex1 > -1, "Index out of bounds");
	PRECONDITION(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex2 > -1, "Index out of bounds");
	return m_ppiYieldFromTileConquest ? m_ppiYieldFromTileConquest[eIndex1][eIndex2] : 0;
}

int CvTraitEntry::GetYieldFromTileCultureBomb(TerrainTypes eIndex1, YieldTypes eIndex2) const
{
	PRECONDITION(eIndex1 < GC.getNumTerrainInfos(), "Index out of bounds");
	PRECONDITION(eIndex1 > -1, "Index out of bounds");
	PRECONDITION(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex2 > -1, "Index out of bounds");
	return m_ppiYieldFromTileCultureBomb ? m_ppiYieldFromTileCultureBomb[eIndex1][eIndex2] : 0;
}

int CvTraitEntry::GetYieldFromTileStealCultureBomb(TerrainTypes eIndex1, YieldTypes eIndex2) const
{
	PRECONDITION(eIndex1 < GC.getNumTerrainInfos(), "Index out of bounds");
	PRECONDITION(eIndex1 > -1, "Index out of bounds");
	PRECONDITION(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex2 > -1, "Index out of bounds");
	return m_ppiYieldFromTileStealCultureBomb ? m_ppiYieldFromTileStealCultureBomb[eIndex1][eIndex2] : 0;
}

int CvTraitEntry::GetYieldFromTileSettle(TerrainTypes eIndex1, YieldTypes eIndex2) const
{
	PRECONDITION(eIndex1 < GC.getNumTerrainInfos(), "Index out of bounds");
	PRECONDITION(eIndex1 > -1, "Index out of bounds");
	PRECONDITION(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex2 > -1, "Index out of bounds");
	return m_ppiYieldFromTileSettle ? m_ppiYieldFromTileSettle[eIndex1][eIndex2] : 0;
}

int CvTraitEntry::GetYieldChangePerImprovementBuilt(ImprovementTypes eIndex1, YieldTypes eIndex2) const
{
	PRECONDITION(eIndex1 < GC.getNumImprovementInfos(), "Index out of bounds");
	PRECONDITION(eIndex1 > -1, "Index out of bounds");
	PRECONDITION(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex2 > -1, "Index out of bounds");
	return m_ppiYieldChangePerImprovementBuilt ? m_ppiYieldChangePerImprovementBuilt[eIndex1][eIndex2] : 0;
}

int CvTraitEntry::GetYieldFromBarbarianCampClear(YieldTypes eIndex1, bool bEraScaling) const
{
	PRECONDITION(eIndex1 < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex1 > -1, "Index out of bounds");
	std::map<int, std::map<bool, int>>::const_iterator itYield = m_pbiYieldFromBarbarianCampClear.find((int)eIndex1);
	if (itYield != m_pbiYieldFromBarbarianCampClear.end()) // find returns the iterator to map::end if the key eYield is not present in the map
	{
		std::map<bool, int>::const_iterator itBool = itYield->second.find(bEraScaling);
		if (itBool != itYield->second.end())
		{
			return itBool->second;
		}

		return 0;
	}

	return 0;
}

/// Accessor:: Extra yield from a plot
int CvTraitEntry::GetPlotYieldChanges(PlotTypes eIndex1, YieldTypes eIndex2) const
{
	PRECONDITION(eIndex1 < GC.getNumPlotInfos(), "Index out of bounds");
	PRECONDITION(eIndex1 > -1, "Index out of bounds");
	PRECONDITION(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex2 > -1, "Index out of bounds");
	return m_ppiPlotYieldChanges ? m_ppiPlotYieldChanges[eIndex1][eIndex2] : 0;
}

int CvTraitEntry::GetYieldFromHistoricEvent(int i) const
{
	return m_piYieldFromHistoricEvent? m_piYieldFromHistoricEvent[i] : -1;
}
int CvTraitEntry::GetYieldFromXMilitaryUnits(int i) const
{
	return m_piYieldFromXMilitaryUnits ? m_piYieldFromXMilitaryUnits[i] : -1;
}
int CvTraitEntry::GetYieldFromLevelUp(int i) const
{
	return m_piYieldFromLevelUp ? m_piYieldFromLevelUp[i] : -1;
}
int CvTraitEntry::GetYieldFromOwnPantheon(int i) const
{
	return m_piYieldFromOwnPantheon? m_piYieldFromOwnPantheon[i] : -1;
}
std::pair<int, int> CvTraitEntry::GetTradeRouteEndYield(YieldTypes eYield) const
{
	const std::map<int, std::pair<int, int>>::const_iterator it = m_tradeRouteEndYield.find(static_cast<int>(eYield));
	if (it != m_tradeRouteEndYield.end())
	{
		return it->second;
	}
	return std::make_pair(0, 0);
}
int CvTraitEntry::GetYieldFromRouteMovement(int i) const
{
	return m_piYieldFromRouteMovement ? m_piYieldFromRouteMovement[i] : -1;
}
int CvTraitEntry::GetYieldFromExport(int i) const
{
	return m_piYieldFromExport ? m_piYieldFromExport[i] : -1;
}
int CvTraitEntry::GetYieldFromImport(int i) const
{
	return m_piYieldFromImport ? m_piYieldFromImport[i] : -1;
}
int CvTraitEntry::GetYieldFromTilePurchase(int i) const
{
	return m_piYieldFromTilePurchase ? m_piYieldFromTilePurchase[i] : -1;
}
int CvTraitEntry::GetYieldFromTileEarn(int i) const
{
	return m_piYieldFromTileEarn ? m_piYieldFromTileEarn[i] : -1;
}
int CvTraitEntry::GetYieldFromCSAlly(int i) const
{
	return m_piYieldFromCSAlly ? m_piYieldFromCSAlly[i] : -1;
}
int CvTraitEntry::GetYieldFromCSFriend(int i) const
{
	return m_piYieldFromCSFriend ? m_piYieldFromCSFriend[i] : -1;
}
int CvTraitEntry::GetYieldFromSettle(int i) const
{
	return m_piYieldFromSettle ? m_piYieldFromSettle[i] : -1;
}
int CvTraitEntry::GetYieldFromConquest(int i) const
{
	return m_piYieldFromConquest ? m_piYieldFromConquest[i] : -1;
}
int CvTraitEntry::GetYieldFromCityDamageTimes100(int i) const
{
	return m_piYieldFromCityDamageTimes100 ? m_piYieldFromCityDamageTimes100[i] : -1;
}
int CvTraitEntry::GetPurchasedUnitsBonusXP() const
{
	return m_iPurchasedUnitsBonusXP;
}
int CvTraitEntry::GetVotePerXCSAlliance() const
{
	return m_iVotePerXCSAlliance;
}
int CvTraitEntry::GetVotePerXCSFollowingYourReligion() const
{
	return m_iVotePerXCSFollowingFollowingYourReligion;
}
int CvTraitEntry::GetChanceToConvertReligiousUnits() const
{
	return m_iChanceToConvertReligiousUnits;
}
int CvTraitEntry::GetGoldenAgeFromVictory() const
{
	return m_iGoldenAgeFromVictory;
}
int CvTraitEntry::GetFreePolicyPerXTechs() const
{
	return m_iFreePolicyPerXTechs;
}
EraTypes CvTraitEntry::GetGPFaithPurchaseEra() const
{
	return m_eGPFaithPurchaseEra;
}
int CvTraitEntry::GetFaithCostModifier() const
{
	return m_iFaithCostModifier;
}


int CvTraitEntry::GetNumPledgeDomainProductionModifier(DomainTypes eDomain) const
{
	PRECONDITION((int)eDomain < NUM_DOMAIN_TYPES, "Index out of bounds");
	PRECONDITION((int)eDomain > -1, "Index out of bounds");
	return m_piNumPledgesDomainProdMod ? m_piNumPledgesDomainProdMod[(int)eDomain] : 0;
}
int CvTraitEntry::GetDomainFreeExperienceModifier(DomainTypes eDomain) const
{
	PRECONDITION((int)eDomain < NUM_DOMAIN_TYPES, "Index out of bounds");
	PRECONDITION((int)eDomain > -1, "Index out of bounds");
	return m_piDomainFreeExperienceModifier ? m_piDomainFreeExperienceModifier[(int)eDomain] : 0;
}
int CvTraitEntry::GetFreeUnitClassesDOW(UnitClassTypes eUnitClass) const
{
	PRECONDITION((int)eUnitClass < GC.getNumUnitClassInfos(), "Index out of bounds");
	PRECONDITION((int)eUnitClass > -1, "Index out of bounds");
	return m_piFreeUnitClassesDOW ? m_piFreeUnitClassesDOW[(int)eUnitClass] : 0;
}

int CvTraitEntry::GetYieldFromRouteMovementInForeignTerritory(YieldTypes eIndex, bool bTradePartner) const
{
	PRECONDITION(eIndex < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex > -1, "Index out of bounds");

	std::map<int, std::map<bool, int>>::const_iterator itYield = m_pbiYieldFromRouteMovementInForeignTerritory.find((int)eIndex);
	if (itYield != m_pbiYieldFromRouteMovementInForeignTerritory.end()) // find returns the iterator to map::end if the key eYield is not present in the map
	{
		std::map<bool, int>::const_iterator itBool = itYield->second.find(bTradePartner);
		if (itBool != itYield->second.end())
		{
			return itBool->second;
		}
	}

	return 0;
}

int CvTraitEntry::GetBuildingClassYieldChanges(BuildingClassTypes eIndex1, YieldTypes eIndex2) const
{
	PRECONDITION(eIndex1 < GC.getNumBuildingClassInfos(), "Index out of bounds");
	PRECONDITION(eIndex1 > -1, "Index out of bounds");
	PRECONDITION(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex2 > -1, "Index out of bounds");
	return m_ppiBuildingClassYieldChanges ? m_ppiBuildingClassYieldChanges[eIndex1][eIndex2] : 0;
}

int CvTraitEntry::GetCapitalYieldChanges(int i) const
{
	return m_piCapitalYieldChanges ? m_piCapitalYieldChanges[i] : -1;
}

int CvTraitEntry::GetCityYieldChanges(int i) const
{
	return m_piCityYieldChanges ? m_piCityYieldChanges[i] : -1;
}
int CvTraitEntry::GetPermanentYieldChangeWLTKD(int i) const
{
	return m_piPermanentYieldChangeWLTKD ? m_piPermanentYieldChangeWLTKD[i] : -1;
}

int CvTraitEntry::GetCoastalCityYieldChanges(int i) const
{
	return m_piCoastalCityYieldChanges ? m_piCoastalCityYieldChanges[i] : -1;
}

int CvTraitEntry::GetGreatWorkYieldChanges(int i) const
{
	return m_piGreatWorkYieldChanges ? m_piGreatWorkYieldChanges[i] : -1;
}
int CvTraitEntry::GetArtifactYieldChanges(int i) const
{
	return m_piArtifactYieldChanges ? m_piArtifactYieldChanges[i] : -1;
}
int CvTraitEntry::GetArtYieldChanges(int i) const
{
	return m_piArtYieldChanges ? m_piArtYieldChanges[i] : -1;
}
int CvTraitEntry::GetLitYieldChanges(int i) const
{
	return m_piLitYieldChanges ? m_piLitYieldChanges[i] : -1;
}
int CvTraitEntry::GetMusicYieldChanges(int i) const
{
	return m_piMusicYieldChanges ? m_piMusicYieldChanges[i] : -1;
}
int CvTraitEntry::GetSeaPlotYieldChanges(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piSeaPlotYieldChanges ? m_piSeaPlotYieldChanges[i] : 0;
}
int CvTraitEntry::GetFeatureYieldChanges(FeatureTypes eIndex1, YieldTypes eIndex2) const
{
	PRECONDITION(eIndex1 < GC.getNumFeatureInfos(), "Index out of bounds");
	PRECONDITION(eIndex1 > -1, "Index out of bounds");
	PRECONDITION(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex2 > -1, "Index out of bounds");
	return m_ppiFeatureYieldChanges ? m_ppiFeatureYieldChanges[eIndex1][eIndex2] : 0;
}

int CvTraitEntry::GetResourceYieldChanges(ResourceTypes eIndex1, YieldTypes eIndex2) const
{
	PRECONDITION(eIndex1 < GC.getNumResourceInfos(), "Index out of bounds");
	PRECONDITION(eIndex1 > -1, "Index out of bounds");
	PRECONDITION(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex2 > -1, "Index out of bounds");
	return m_ppiResourceYieldChanges ? m_ppiResourceYieldChanges[eIndex1][eIndex2] : 0;
}

int CvTraitEntry::GetLuxuryYieldChanges(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piLuxuryYieldChanges ? m_piLuxuryYieldChanges[i] : 0;
}

std::map<int, std::map<int, int>> CvTraitEntry::GetResourceYieldChangesFromGoldenAge()
{
	return m_miResourceYieldChangesFromGoldenAge;
}

std::map<int, std::map<int, int>> CvTraitEntry::GetResourceYieldChangesFromGoldenAgeCap()
{
	return m_miResourceYieldChangesFromGoldenAgeCap;
}

int CvTraitEntry::GetTerrainYieldChanges(TerrainTypes eIndex1, YieldTypes eIndex2) const
{
	PRECONDITION(eIndex1 < GC.getNumTerrainInfos(), "Index out of bounds");
	PRECONDITION(eIndex1 > -1, "Index out of bounds");
	PRECONDITION(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex2 > -1, "Index out of bounds");
	return m_ppiTerrainYieldChanges ? m_ppiTerrainYieldChanges[eIndex1][eIndex2] : 0;
}

int CvTraitEntry::GetYieldFromKills(YieldTypes eYield) const
{
	PRECONDITION((int)eYield < NUM_YIELD_TYPES, "Yield type out of bounds");
	PRECONDITION((int)eYield > -1, "Index out of bounds");
	return m_piYieldFromKills ? m_piYieldFromKills[(int)eYield] : 0;
}

int CvTraitEntry::GetYieldFromBarbarianKills(YieldTypes eYield) const
{
	PRECONDITION((int)eYield < NUM_YIELD_TYPES, "Yield type out of bounds");
	PRECONDITION((int)eYield > -1, "Index out of bounds");
	return m_piYieldFromBarbarianKills ? m_piYieldFromBarbarianKills[(int)eYield] : 0;
}

int CvTraitEntry::GetYieldFromMinorDemand(YieldTypes eYield) const
{
	PRECONDITION((int)eYield < NUM_YIELD_TYPES, "Yield type out of bounds");
	PRECONDITION((int)eYield > -1, "Index out of bounds");
	return m_piYieldFromMinorDemand ? m_piYieldFromMinorDemand[(int)eYield] : 0;
}

int CvTraitEntry::GetYieldFromLuxuryResourceGain(YieldTypes eYield) const
{
	PRECONDITION((int)eYield < NUM_YIELD_TYPES, "Yield type out of bounds");
	PRECONDITION((int)eYield > -1, "Index out of bounds");
	return m_piYieldFromLuxuryResourceGain ? m_piYieldFromLuxuryResourceGain[(int)eYield] : 0;
}

int CvTraitEntry::GetYieldChangeTradeRoute(int i) const
{
	return m_piYieldChangeTradeRoute ? m_piYieldChangeTradeRoute[i] : 0;
}

int CvTraitEntry::GetYieldChangeWorldWonder(int i) const
{
	return m_piYieldChangeWorldWonder ? m_piYieldChangeWorldWonder[i] : 0;
}

int CvTraitEntry::GetTradeRouteYieldChange(DomainTypes eIndex1, YieldTypes eIndex2) const
{
	PRECONDITION(eIndex1 < NUM_DOMAIN_TYPES, "Index out of bounds");
	PRECONDITION(eIndex1 > -1, "Index out of bounds");
	PRECONDITION(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex2 > -1, "Index out of bounds");
	return m_ppiTradeRouteYieldChange ? m_ppiTradeRouteYieldChange[eIndex1][eIndex2] : 0;
}

/// Accessor:: Extra yield from an improvement
int CvTraitEntry::GetSpecialistYieldChanges(SpecialistTypes eIndex1, YieldTypes eIndex2) const
{
	PRECONDITION(eIndex1 < GC.getNumSpecialistInfos(), "Index out of bounds");
	PRECONDITION(eIndex1 > -1, "Index out of bounds");
	PRECONDITION(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex2 > -1, "Index out of bounds");
	return m_ppiSpecialistYieldChanges ? m_ppiSpecialistYieldChanges[eIndex1][eIndex2] : 0;
}

int CvTraitEntry::GetGreatPersonExpendedYield(GreatPersonTypes eIndex1, YieldTypes eIndex2) const
{
	PRECONDITION(eIndex1 < GC.getNumGreatPersonInfos(), "Index out of bounds");
	PRECONDITION(eIndex1 > -1, "Index out of bounds");
	PRECONDITION(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex2 > -1, "Index out of bounds");
	return m_ppiGreatPersonExpendedYield ? m_ppiGreatPersonExpendedYield[eIndex1][eIndex2] : 0;
}
int CvTraitEntry::GetGreatPersonBornYield(GreatPersonTypes eIndex1, YieldTypes eIndex2) const
{
	PRECONDITION(eIndex1 < GC.getNumGreatPersonInfos(), "Index out of bounds");
	PRECONDITION(eIndex1 > -1, "Index out of bounds");
	PRECONDITION(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex2 > -1, "Index out of bounds");
	return m_ppiGreatPersonBornYield ? m_ppiGreatPersonBornYield[eIndex1][eIndex2] : 0;
}
int CvTraitEntry::GetGoldenAgeGreatPersonRateModifier(GreatPersonTypes eGreatPerson) const
{
	PRECONDITION((int)eGreatPerson < GC.getNumGreatPersonInfos(), "Yield type out of bounds");
	PRECONDITION((int)eGreatPerson > -1, "Index out of bounds");
	return m_piGoldenAgeGreatPersonRateModifier ? m_piGoldenAgeGreatPersonRateModifier[(int)eGreatPerson] : 0;
}

int CvTraitEntry::GetGreatPersonCostReduction(GreatPersonTypes eGreatPerson) const
{
	PRECONDITION((int)eGreatPerson < GC.getNumGreatPersonInfos(), "Yield type out of bounds");
	PRECONDITION((int)eGreatPerson > -1, "Index out of bounds");
	return m_piGreatPersonCostReduction ? m_piGreatPersonCostReduction[(int)eGreatPerson] : 0;
}

int CvTraitEntry::GetPerPuppetGreatPersonRateModifier(GreatPersonTypes eGreatPerson) const
{
	PRECONDITION((int)eGreatPerson < GC.getNumGreatPersonInfos(), "Yield type out of bounds");
	PRECONDITION((int)eGreatPerson > -1, "Index out of bounds");
	return m_piPerPuppetGreatPersonRateModifier ? m_piPerPuppetGreatPersonRateModifier[(int)eGreatPerson] : 0;
}

int CvTraitEntry::GetGreatPersonGWAM(GreatPersonTypes eGreatPerson) const
{
	PRECONDITION((int)eGreatPerson < GC.getNumGreatPersonInfos(), "Yield type out of bounds");
	PRECONDITION((int)eGreatPerson > -1, "Index out of bounds");
	return m_piGreatPersonGWAM ? m_piGreatPersonGWAM[(int)eGreatPerson] : 0;
}

int CvTraitEntry::GetGoldenAgeFromGreatPersonBirth(GreatPersonTypes eGreatPerson) const
{
	PRECONDITION((int)eGreatPerson < GC.getNumGreatPersonInfos(), "Yield type out of bounds");
	PRECONDITION((int)eGreatPerson > -1, "Index out of bounds");
	return m_piGoldenAgeFromGreatPersonBirth ? m_piGoldenAgeFromGreatPersonBirth[(int)eGreatPerson] : 0;
}

int CvTraitEntry::GetGreatPersonProgressFromPolicyUnlock(GreatPersonTypes eIndex) const
{
	PRECONDITION((int)eIndex < GC.getNumGreatPersonInfos(), "Yield type out of bounds");
	PRECONDITION((int)eIndex > -1, "Index out of bounds");
	return m_piGreatPersonProgressFromPolicyUnlock ? m_piGreatPersonProgressFromPolicyUnlock[(int)eIndex] : 0;
}

int CvTraitEntry::GetGreatPersonProgressFromKills(GreatPersonTypes eIndex) const
{
	PRECONDITION((int)eIndex < GC.getNumGreatPersonInfos(), "Yield type out of bounds");
	PRECONDITION((int)eIndex > -1, "Index out of bounds");

	std::map<int, int>::const_iterator it = m_piGreatPersonProgressFromKills.find((int)eIndex);
	if (it != m_piGreatPersonProgressFromKills.end()) // find returns the iterator to map::end if the key eIndex is not present in the map
	{
		return it->second;
	}

	return 0;
}

int CvTraitEntry::GetRandomGreatPersonProgressFromKills(GreatPersonTypes eIndex) const
{
	PRECONDITION((int)eIndex < GC.getNumGreatPersonInfos(), "Yield type out of bounds");
	PRECONDITION((int)eIndex > -1, "Index out of bounds");

	std::map<int, int>::const_iterator it = m_piRandomGreatPersonProgressFromKills.find((int)eIndex);
	if (it != m_piRandomGreatPersonProgressFromKills.end()) // find returns the iterator to map::end if the key eIndex is not present in the map
	{
		return it->second;
	}

	return 0;
}

int CvTraitEntry::GetCityYieldFromUnimprovedFeature(FeatureTypes eIndex1, YieldTypes eIndex2) const
{
	PRECONDITION(eIndex1 < GC.getNumFeatureInfos(), "Index out of bounds");
	PRECONDITION(eIndex1 > -1, "Index out of bounds");
	PRECONDITION(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex2 > -1, "Index out of bounds");
	return m_ppiCityYieldFromUnimprovedFeature ? m_ppiCityYieldFromUnimprovedFeature[eIndex1][eIndex2] : 0;
}

/// Accessor:: Extra yield from an unimproved feature
int CvTraitEntry::GetUnimprovedFeatureYieldChanges(FeatureTypes eIndex1, YieldTypes eIndex2) const
{
	PRECONDITION(eIndex1 < GC.getNumFeatureInfos(), "Index out of bounds");
	PRECONDITION(eIndex1 > -1, "Index out of bounds");
	PRECONDITION(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex2 > -1, "Index out of bounds");
	return m_ppiUnimprovedFeatureYieldChanges ? m_ppiUnimprovedFeatureYieldChanges[eIndex1][eIndex2] : 0;
}

/// Accessor:: Additional moves for a class of combat unit
int CvTraitEntry::GetMovesChangeUnitCombat(const int unitCombatID) const
{
	ASSERT((unitCombatID >= 0), "unitCombatID is less than zero");
	PRECONDITION((unitCombatID < GC.getNumUnitCombatClassInfos()), "unitCombatID exceeds number of combat classes");

	return m_piMovesChangeUnitCombats[unitCombatID];
}
/// Accessor:: Additional moves for a class of combat unit
int CvTraitEntry::GetMovesChangeUnitClass(const int unitClassID) const
{
	ASSERT((unitClassID >= 0), "unitCombatID is less than zero");
	PRECONDITION((unitClassID < GC.getNumUnitClassInfos()), "unitCombatID exceeds number of combat classes");

	return m_piMovesChangeUnitClasses[unitClassID];
}
int CvTraitEntry::GetGAPToYield(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	return m_paiGAPToYield[i];
}
int CvTraitEntry::GetMountainRangeYield(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	return m_paiMountainRangeYield[i];
}

/// Accessor:: Maintenance Modifier for a class of combat unit
int CvTraitEntry::GetMaintenanceModifierUnitCombat(const int unitCombatID) const
{
	ASSERT((unitCombatID >= 0), "unitCombatID is less than zero");
	PRECONDITION((unitCombatID < GC.getNumUnitCombatClassInfos()), "unitCombatID exceeds number of combat classes");

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

/// Belief that makes this trait obsolete
int CvTraitEntry::GetObsoleteBelief() const
{
	return m_iObsoleteBelief;
}

/// Belief that enables this trait
int CvTraitEntry::GetPrereqBelief() const
{
	return m_iPrereqBelief;
}

/// Policy that makes this trait obsolete
int CvTraitEntry::GetObsoletePolicy() const
{
	return m_iObsoletePolicy;
}

/// Policy that enables this trait
int CvTraitEntry::GetPrereqPolicy() const
{
	return m_iPrereqPolicy;
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
/// Accessor:: Do certain units have a unique upgrade path?
bool CvTraitEntry::IsSpecialUpgradeUnitClass(const int unitClassesID, const int unitID) const
{
	std::multimap<int, int>::const_iterator it = m_piUpgradeUnitClass.find(unitClassesID);
	if (it != m_piUpgradeUnitClass.end())
	{
		// get an iterator to the element that is one past the last element associated with key
		std::multimap<int, int>::const_iterator lastElement = m_piUpgradeUnitClass.upper_bound(unitClassesID);

		// for each element in the sequence [itr, lastElement)
		for (; it != lastElement; ++it)
		{
			if (it->second == unitID)
			{
				return true;
			}
		}
	}

	return false;
}
/// Accessor:: Does the civ get free promotions for a class?
bool CvTraitEntry::IsFreePromotionUnitClass(const int promotionID, const int unitClassID) const
{
	std::multimap<int, int>::const_iterator it = m_FreePromotionUnitClass.find(promotionID);
	if(it != m_FreePromotionUnitClass.end())
	{
		// get an iterator to the element that is one past the last element associated with key
		std::multimap<int, int>::const_iterator lastElement = m_FreePromotionUnitClass.upper_bound(promotionID);

		// for each element in the sequence [itr, lastElement)
		for(; it != lastElement; ++it)
		{
			if(it->second == unitClassID)
			{
				return true;
			}
		}
	}

	return false;
}
/// Accessor:: Does the civ have a golden age modifier for the yield type?
int CvTraitEntry::GetGoldenAgeYieldModifier(const int iYield) const
{
	PRECONDITION(iYield < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(iYield > -1, "Index out of bounds");
	
	std::map<int, int>::const_iterator it = m_piGoldenAgeYieldModifier.find(iYield);
	if (it != m_piGoldenAgeYieldModifier.end()) // find returns the iterator to map::end if the key iYield is not present in the map
	{
		return it->second;
	}

	return 0;
}
/// Accessor:: Does the civ grant units the ability to build something?
bool CvTraitEntry::UnitClassCanBuild(const int buildID, const int unitClassID) const
{
	std::multimap<int, int>::const_iterator it = m_BuildsUnitClasses.find(buildID);
	if(it != m_BuildsUnitClasses.end())
	{
		// get an iterator to the element that is one past the last element associated with key
		std::multimap<int, int>::const_iterator lastElement = m_BuildsUnitClasses.upper_bound(buildID);

		// for each element in the sequence [itr, lastElement)
		for(; it != lastElement; ++it)
		{
			if(it->second == unitClassID)
			{
				return true;
			}
		}
	}

	return false;
}
/// Accessor:: Does the civ have a production cost modifier for the unit combat type? And is it only granted during golden ages?
std::pair <int, bool> CvTraitEntry::GetUnitCombatProductionCostModifier(const int unitCombatID) const
{
	PRECONDITION(unitCombatID >= 0, "unitCombatID expected to be >= 0");
	PRECONDITION(unitCombatID < GC.getNumUnitCombatClassInfos(), "unitCombatID expected to be < GC.getNumUnitCombatInfos()");

	std::map<int, std::pair<int, bool>>::const_iterator it = m_pibUnitCombatProductionCostModifier.find(unitCombatID);
	if (it != m_pibUnitCombatProductionCostModifier.end()) // find returns the iterator to map::end if the key iYield is not present in the map
	{
		return it->second;
	}

	return std::make_pair(0, false);
}
/// Accessor:: Does the civ require less food for non-specialists?
int CvTraitEntry::GetNonSpecialistFoodChange() const
{
	return m_iNonSpecialistFoodChange;
}

/// Accessor:: Is this build blocked for this civ?
bool CvTraitEntry::IsNoBuild(BuildTypes eBuild) const
{
	std::vector<int>::const_iterator it = find(m_aiNoBuilds.begin(), m_aiNoBuilds.end(), (int)eBuild);

	return it != m_aiNoBuilds.end();
}

/// Accessor:: Does the civ have a production modifier for domain type per worked specialist?
int CvTraitEntry::GetDomainProductionModifiersPerSpecialist(DomainTypes eDomain) const
{
	PRECONDITION(eDomain >= 0, "domainID expected to be >= 0");
	PRECONDITION(eDomain < NUM_DOMAIN_TYPES, "domainID expected to be < NUM_DOMAIN_TYPES");

	std::map<int, int>::const_iterator it = m_piDomainProductionModifiersPerSpecialist.find(eDomain);
	if (it != m_piDomainProductionModifiersPerSpecialist.end()) // find returns the iterator to map::end if the key eDomain is not present
	{
		return it->second;
	}

	return 0;
}

/// Accessor:: If linked with trade routes, does the origin city gain a percent of the target city's production towards that specific thing
TradeRouteProductionSiphon CvTraitEntry::GetTradeRouteProductionSiphon(bool bInternationalOnly) const
{
	std::map<bool, TradeRouteProductionSiphon>::const_iterator it = m_biiTradeRouteProductionSiphon.find(bInternationalOnly);
	if (it != m_biiTradeRouteProductionSiphon.end()) // find returns the iterator to map::end if the key bInternationalOnly is not present
	{
		return it->second;
	}

	TradeRouteProductionSiphon sDefault;

	return sDefault;
}

/// Accessor:: Does this trait change the reveal and/or the city trade techs for resources?
AlternateResourceTechs CvTraitEntry::GetAlternateResourceTechs(ResourceTypes eResource) const
{
	std::map<int, AlternateResourceTechs>::const_iterator it = m_piiAlternateResourceTechs.find((int)eResource);
	if (it != m_piiAlternateResourceTechs.end()) // find returns the iterator to map::end if the key eResource is not present
	{
		return it->second;
	}

	AlternateResourceTechs sDefault;
	sDefault.m_eTechCityTrade = NO_TECH;
	sDefault.m_eTechReveal = NO_TECH;

	return sDefault;
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
		return GET_TEAM(eTeam).GetTeamTechs()->HasTech((TechTypes)m_iPrereqTech);
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

/// Has this trait become obsolete?
bool CvTraitEntry::IsObsoleteByBelief(PlayerTypes ePlayer)
{
	bool bObsolete = false;

	if (m_iObsoleteBelief != NO_BELIEF)
		bObsolete = (GET_PLAYER(ePlayer).HasBelief((BeliefTypes)m_iObsoleteBelief));

	return bObsolete;
}

/// Is this trait enabled by belief?
bool CvTraitEntry::IsEnabledByBelief(PlayerTypes ePlayer)
{
	bool bEnabled = true;

	if (m_iPrereqBelief != NO_BELIEF)
		bEnabled = (GET_PLAYER(ePlayer).HasBelief((BeliefTypes)m_iPrereqBelief));

	return bEnabled;
}

/// Has this trait become obsolete?
bool CvTraitEntry::IsObsoleteByPolicy(PlayerTypes ePlayer)
{
	bool bObsolete = false;

	if (m_iObsoletePolicy != NO_POLICY)
		bObsolete = (GET_PLAYER(ePlayer).HasPolicy((PolicyTypes)m_iObsoletePolicy));

	return bObsolete;
}

/// Is this trait enabled by policy?
bool CvTraitEntry::IsEnabledByPolicy(PlayerTypes ePlayer)
{
	bool bEnabled = true;

	if (m_iPrereqPolicy != NO_POLICY)
		bEnabled = (GET_PLAYER(ePlayer).HasPolicy((PolicyTypes)m_iPrereqPolicy));

	return bEnabled;
}

bool CvTraitEntry::TerrainClaimBoost(TerrainTypes eTerrain)
{
	if (eTerrain != NO_TERRAIN)
	{
		return m_abTerrainClaimBoost[eTerrain];
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
	m_iNationalPopReqModifier				= kResults.GetInt("NationalPopReqModifier");
	m_iCityWorkingChange					= kResults.GetInt("CityWorkingChange");
	m_iCityAutomatonWorkersChange			= kResults.GetInt("CityAutomatonWorkersChange");
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
	m_iWarWearinessModifier					= kResults.GetInt("WarWearinessModifier");
	m_iEnemyWarWearinessModifier			= kResults.GetInt("EnemyWarWearinessModifier");
	m_iCombatBonusVsHigherPop				= kResults.GetInt("CombatBonusVsHigherPop");
	m_bBuyOwnedTiles						= kResults.GetBool("BuyOwnedTiles");
	m_bNewCitiesStartWithCapitalReligion	= kResults.GetBool("NewCitiesStartWithCapitalReligion");
	m_bNoSpread								= kResults.GetBool("NoSpread");
	m_iXPBonusFromGreatPersonBirth			= kResults.GetInt("XPBonusFromGreatPersonBirth");
	m_iUnitHealFromGreatPersonBirth			= kResults.GetInt("UnitHealFromGreatPersonBirth");
	m_bDiplomaticMarriage					= kResults.GetBool("DiplomaticMarriage");
	m_iBullyMilitaryStrengthModifier		= kResults.GetInt("CSBullyMilitaryStrengthModifier");
	m_iBullyValueModifier					= kResults.GetInt("CSBullyValueModifier");
	m_bIgnoreBullyPenalties					= kResults.GetBool("IgnoreBullyPenalties");
	m_bAdoptionFreeTech						= kResults.GetBool("IsAdoptionFreeTech");
	m_iWLTKDGPImprovementModifier			= kResults.GetInt("WLTKDGPImprovementModifier");
	m_iGrowthBoon							= kResults.GetInt("GrowthBoon");
	m_iAllianceCSDefense					= kResults.GetInt("AllianceCSDefense");
	m_iAllianceCSStrength					= kResults.GetInt("AllianceCSStrength");
	m_iTourismGABonus						= kResults.GetInt("TourismGABonus");
	m_bGPWLTKD								= kResults.GetBool("GPWLTKD");
	m_bGreatWorkWLTKD						= kResults.GetBool("GreatWorkWLTKD");
	m_bExpansionWLTKD						= kResults.GetBool("ExpansionWLTKD");
	m_bTradeRouteOnly						= kResults.GetBool("TradeRouteOnly");
	m_bKeepConqueredBuildings				= kResults.GetBool("KeepConqueredBuildings");
	m_bMountainPass							= kResults.GetBool("MountainPass");
	m_bUniqueBeliefsOnly					= kResults.GetBool("UniqueBeliefsOnly");
	m_bNoNaturalReligionSpread				= kResults.GetBool("NoNaturalReligionSpread");
	m_bNoOpenTrade							= kResults.GetBool("NoOpenTrade");
	m_bGoldenAgeOnWar						= kResults.GetBool("GoldenAgeOnWar");
	m_bUnableToCancelRazing                 = kResults.GetBool("UnableToCancelRazing");
	m_iInfluenceMeetCS						= kResults.GetInt("InfluenceMeetCS");
	m_iTourismToGAP							= kResults.GetInt("TourismToGAP");
	m_iGoldToGAP							= kResults.GetInt("GoldToGAP");
	m_iMultipleAttackBonus					= kResults.GetInt("MultipleAttackBonus");
	m_iCityConquestGWAM						= kResults.GetInt("CityConquestGWAM");
	m_iEventTourismBoost					= kResults.GetInt("EventTourismBoost");
	m_iEventGP								= kResults.GetInt("EventGP");
	m_iWLTKDCulture							= kResults.GetInt("WLTKDCultureBoost");
	m_iWLTKDGATimer							= kResults.GetInt("WLTKDFromGATurns");
	m_iWLTKDUnhappinessNeedsMod				= kResults.GetInt("WLTKDUnhappinessNeedsMod");
	m_iStartingSpies						= kResults.GetInt("StartingSpies");
	m_iSpyOffensiveStrengthModifier			= kResults.GetInt("SpyOffensiveStrengthModifier");
	m_iSpyMoveRateBonus						= kResults.GetInt("SpyMoveRateModifier");
	m_iSpySecurityModifier					= kResults.GetInt("SpySecurityModifier");
	m_iSpyExtraRankBonus					= kResults.GetInt("SpyExtraRankBonus");
	m_iQuestYieldModifier					= kResults.GetInt("MinorQuestYieldModifier");
	m_iWonderProductionModifierToBuilding	= kResults.GetInt("WonderProductionModifierToBuilding");
	m_iPolicyGEorGM							= kResults.GetInt("PolicyGEorGM");
	m_iGAGarrisonCityRangeStrikeModifier	= kResults.GetInt("GAGarrisonCityRangeStrikeModifier");
	m_bBestUnitSpawnOnImpDOW				= kResults.GetBool("BestUnitSpawnOnImpDOW");
	m_iGGGARateFromDenunciationsAndWars		= kResults.GetInt("GGGARateFromDenunciationsAndWars");
	m_bTradeRouteMinorInfluenceAP			= kResults.GetBool("TradeRouteMinorInfluenceAP");
	m_bProdModFromNumSpecialists			= kResults.GetBool("ProdModFromNumSpecialists");
	m_iConquestOfTheWorldCityAttack			= kResults.GetInt("ConquestOfTheWorldCityAttack");
	m_bConquestOfTheWorld					= kResults.GetBool("ConquestOfTheWorld");
	m_bFreeUpgrade							= kResults.GetBool("FreeUpgrade");
	m_bWarsawPact							= kResults.GetBool("WarsawPact");
	m_bFreeZuluPikemanToImpi				= kResults.GetBool("FreeZuluPikemanToImpi");
	m_bPermanentYieldsDecreaseEveryEra		= kResults.GetBool("PermanentYieldsDecreaseEveryEra");
	m_bImportsCountTowardsMonopolies		= kResults.GetBool("ImportsCountTowardsMonopolies");
	m_bCanPurchaseNavalUnitsFaith			= kResults.GetBool("CanPurchaseNavalUnitsFaith");
	m_iPuppetPenaltyReduction				= kResults.GetInt("ReducePuppetPenalties");
	m_iSharedReligionTourismModifier		= kResults.GetInt("SharedReligionTourismModifier");
	m_iExtraMissionaryStrength				= kResults.GetInt("ExtraMissionaryStrength");
	m_bCanGoldInternalTradeRoutes			= kResults.GetBool("CanGoldInternalTradeRoutes");
	m_bAnnexedCityStatesGiveYields			= kResults.GetBool("AnnexedCityStatesGiveYields");
	m_iExtraTradeRoutesPerXOwnedCities		= kResults.GetInt("TradeRoutesPerXOwnedCities");
	m_iExtraTradeRoutesPerXOwnedVassals		= kResults.GetInt("TradeRoutesPerXOwnedVassals");
	m_iMinorInfluencePerGiftedUnit			= kResults.GetInt("MinorInfluencePerGiftedUnit");
	m_bIsCapitalOnly						= kResults.GetBool("IsCapitalOnly");
	m_iInvestmentModifier					= kResults.GetInt("InvestmentModifier");
	m_iCombatBonusVsHigherTech				= kResults.GetInt("CombatBonusVsHigherTech");
	m_iCombatBonusVsLargerCiv				= kResults.GetInt("CombatBonusVsLargerCiv");
	m_iLandUnitMaintenanceModifier          = kResults.GetInt("LandUnitMaintenanceModifier");
	m_iNavalUnitMaintenanceModifier         = kResults.GetInt("NavalUnitMaintenanceModifier");
	m_iRazeSpeedModifier					= kResults.GetInt("RazeSpeedModifier");
	m_iDOFGreatPersonModifier				= kResults.GetInt("DOFGreatPersonModifier");
	m_iLuxuryHappinessRetention				= kResults.GetInt("LuxuryHappinessRetention");
	m_iExtraSupply							= kResults.GetInt("ExtraSupply");
	m_iExtraSupplyPerCity					= kResults.GetInt("ExtraSupplyPerCity");
	m_iExtraSupplyPerPopulation				= kResults.GetInt("ExtraSupplyPerPopulation");
	m_iExtraSpies							= kResults.GetInt("ExtraSpies");
	m_iUnresearchedTechBonusFromKills		= kResults.GetInt("UnresearchedTechBonusFromKills");
	m_iExtraFoundedCityTerritoryClaimRange  = kResults.GetInt("ExtraFoundedCityTerritoryClaimRange");
	m_iExtraConqueredCityTerritoryClaimRange = kResults.GetInt("ExtraConqueredCityTerritoryClaimRange");
	m_iExtraTenetsFirstAdoption				= kResults.GetInt("ExtraTenetsFirstAdoption");
	m_iMonopolyModFlat						= kResults.GetInt("MonopolyModFlat");
	m_iMonopolyModPercent					= kResults.GetInt("MonopolyModPercent");
	m_iFreeSocialPoliciesPerEra				= kResults.GetInt("FreeSocialPoliciesPerEra");
	m_iNumTradeRoutesModifier				= kResults.GetInt("NumTradeRoutesModifier");
	m_iTradeRouteResourceModifier			= kResults.GetInt("TradeRouteResourceModifier");
	m_iUniqueLuxuryCities					= kResults.GetInt("UniqueLuxuryCities");
	m_iUniqueLuxuryQuantity					= kResults.GetInt("UniqueLuxuryQuantity");
	m_iWorkerSpeedModifier					= kResults.GetInt("WorkerSpeedModifier");
	m_iAfraidMinorPerTurnInfluence			= kResults.GetInt("AfraidMinorPerTurnInfluence");
	m_iLandTradeRouteRangeBonus				= kResults.GetInt("LandTradeRouteRangeBonus");
	m_iSeaTradeRouteRangeBonus				= kResults.GetInt("SeaTradeRouteRangeBonus");
	m_iTradeReligionModifier				= kResults.GetInt("TradeReligionModifier");
	m_iTradeBuildingModifier				= kResults.GetInt("TradeBuildingModifier");
	m_iNumFreeBuildings						= kResults.GetInt("NumFreeBuildings");
	m_iNonSpecialistFoodChange				= kResults.GetInt("NonSpecialistFoodChange");
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

	szTextVal = kResults.GetText("EnemyWarSawPactPromotion");
	if (szTextVal)
	{
		m_iEnemyWarSawPactPromotion = (PromotionTypes)GC.getInfoTypeForString(szTextVal, true);
	}
	szTextVal = kResults.GetText("BestUnitImprovement");
	if(szTextVal)
	{
		m_iBestUnitImprovement = (ImprovementTypes)GC.getInfoTypeForString(szTextVal, true);
	}
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

	szTextVal = kResults.GetText("ObsoleteBelief");
	if(szTextVal)
	{
		m_iObsoleteBelief = GC.getInfoTypeForString(szTextVal, true);
	}

	szTextVal = kResults.GetText("PrereqBelief");
	if(szTextVal)
	{
		m_iPrereqBelief = GC.getInfoTypeForString(szTextVal, true);
	}

	szTextVal = kResults.GetText("ObsoletePolicy");
	if(szTextVal)
	{
		m_iObsoletePolicy = GC.getInfoTypeForString(szTextVal, true);
	}

	szTextVal = kResults.GetText("PrereqPolicy");
	if(szTextVal)
	{
		m_iPrereqPolicy = GC.getInfoTypeForString(szTextVal, true);
	}

	szTextVal = kResults.GetText("FreeCapitalBuilding");
	if(szTextVal)
	{
		m_eFreeCapitalBuilding = (BuildingTypes)GC.getInfoTypeForString(szTextVal, true);
	}

	szTextVal = kResults.GetText("FreeUnitOnConquest");
	if(szTextVal)
	{
		m_eFreeUnitOnConquest = (UnitTypes)GC.getInfoTypeForString(szTextVal, true);
	}

	szTextVal = kResults.GetText("FreeBuildingOnConquest");
	if(szTextVal)
	{
		m_eFreeBuildingOnConquest = (BuildingTypes)GC.getInfoTypeForString(szTextVal, true);
	}
	m_bBullyAnnex = kResults.GetBool("BullyAnnex");
	m_iBullyYieldMultiplierAnnex = kResults.GetInt("BullyYieldMultiplierAnnex");
	m_bFightWellDamaged = kResults.GetBool("FightWellDamaged");
	m_bWoodlandMovementBonus = kResults.GetBool("MoveFriendlyWoodsAsRoad");
	m_bRiverMovementBonus = kResults.GetBool("FasterAlongRiver");
	m_bFasterInHills = kResults.GetBool("FasterInHills");
	m_bEmbarkedAllWater = kResults.GetBool("EmbarkedAllWater");
	m_bEmbarkedToLandFlatCost = kResults.GetBool("EmbarkedToLandFlatCost");
	m_bNoHillsImprovementMaintenance = kResults.GetBool("NoHillsImprovementMaintenance");
	m_bTechBoostFromCapitalScienceBuildings = kResults.GetBool("TechBoostFromCapitalScienceBuildings");
	m_bArtistGoldenAgeTechBoost = kResults.GetBool("ArtistGoldenAgeTechBoost");
	m_bStaysAliveZeroCities = kResults.GetBool("StaysAliveZeroCities");
	m_bFaithFromUnimprovedForest = kResults.GetBool("FaithFromUnimprovedForest");
	m_bAnyBelief = kResults.GetBool("AnyBelief");
	m_bAlwaysReligion = kResults.GetBool("AlwaysReligion");

	m_bNoTradeRouteProximityPenalty = kResults.GetBool("NoTradeRouteProximityPenalty");
	m_bCanPlunderWithoutWar = kResults.GetBool("CanPlunderWithoutWar");
	m_bBonusReligiousBelief = kResults.GetBool("BonusReligiousBelief");
	m_bAbleToAnnexCityStates = kResults.GetBool("AbleToAnnexCityStates");
	m_bCrossesMountainsAfterGreatGeneral = kResults.GetBool("CrossesMountainsAfterGreatGeneral");
	m_bCrossesIce = kResults.GetBool("CrossesIce");
	m_bGGFromBarbarians = kResults.GetBool("GGFromBarbarians");
	m_bMayaCalendarBonuses = kResults.GetBool("MayaCalendarBonuses");
	m_bNoAnnexing = kResults.GetBool("NoAnnexing");
	m_bTechFromCityConquer = kResults.GetBool("TechFromCityConquer");
	m_bUniqueLuxuryRequiresNewArea = kResults.GetBool("UniqueLuxuryRequiresNewArea");
	m_bRiverTradeRoad = kResults.GetBool("RiverTradeRoad");
	m_bAngerFreeIntrusionOfCityStates = kResults.GetBool("AngerFreeIntrusionOfCityStates");
	m_iBasicNeedsMedianModifier = kResults.GetInt("BasicNeedsMedianModifier");
	m_iGoldMedianModifier = kResults.GetInt("GoldMedianModifier");
	m_iScienceMedianModifier = kResults.GetInt("ScienceMedianModifier");
	m_iCultureMedianModifier = kResults.GetInt("CultureMedianModifier");
	m_iReligiousUnrestModifier = kResults.GetInt("ReligiousUnrestModifier");
	m_bNoConnectionUnhappiness = kResults.GetBool("NoConnectionUnhappiness");
	m_bIsNoReligiousStrife = kResults.GetBool("IsNoReligiousStrife");
	m_bIsOddEraScaler = kResults.GetBool("IsOddEraScaler");
	m_iWonderProductionModGA = kResults.GetInt("WonderProductionModGA");
	m_iCultureBonusModifierConquest = kResults.GetInt("CultureBonusModifierConquest");
	m_iProductionBonusModifierConquest = kResults.GetInt("ProductionBonusModifierConquest");

	//Arrays
	const char* szTraitType = GetType();
	kUtility.SetYields(m_paiExtraYieldThreshold, "Trait_ExtraYieldThresholds", "TraitType", szTraitType);

	kUtility.SetYields(m_paiYieldChange, "Trait_YieldChanges", "TraitType", szTraitType);
	kUtility.SetYields(m_paiYieldChangeStrategicResources, "Trait_YieldChangesStrategicResources", "TraitType", szTraitType);
	kUtility.SetYields(m_paiYieldChangeNaturalWonder, "Trait_YieldChangesNaturalWonder", "TraitType", szTraitType);
	kUtility.SetYields(m_paiYieldChangePerTradePartner, "Trait_YieldChangesPerTradePartner", "TraitType", szTraitType);
	kUtility.SetYields(m_paiYieldChangeIncomingTradeRoute, "Trait_YieldChangesIncomingTradeRoute", "TraitType", szTraitType);
	kUtility.SetYields(m_paiYieldModifier, "Trait_YieldModifiers", "TraitType", szTraitType);
	kUtility.SetYields(m_paiGAPToYield, "Trait_GAPToYield", "TraitType", szTraitType);
	kUtility.SetYields(m_paiMountainRangeYield, "Trait_MountainRangeYield", "TraitType", szTraitType);
	kUtility.PopulateArrayByValue(m_piNumPledgesDomainProdMod, "Domains", "Trait_NumPledgeDomainProdMod", "DomainType", "TraitType", szTraitType, "Modifier");
	kUtility.PopulateArrayByValue(m_piDomainFreeExperienceModifier, "Domains", "Trait_DomainFreeExperienceModifier", "DomainType", "TraitType", szTraitType, "Modifier", 0, NUM_DOMAIN_TYPES);
	kUtility.PopulateArrayByValue(m_piFreeUnitClassesDOW, "UnitClasses", "Trait_FreeUnitClassesDOW", "UnitClassType", "TraitType", szTraitType, "Number");
	kUtility.PopulateArrayByValue(m_piResourceQuantityModifiers, "Resources", "Trait_ResourceQuantityModifiers", "ResourceType", "TraitType", szTraitType, "ResourceQuantityModifier");
	kUtility.PopulateArrayByValue(m_piNumFreeResourceOnWorldWonderCompletion, "Resources", "Trait_FreeResourceOnWorldWonderCompletion", "ResourceType", "TraitType", szTraitType, "ResourceQuantity");

	// Sets
	kUtility.PopulateSetByExistence(m_siFreePromotions, "UnitPromotions", "Trait_FreePromotions", "PromotionType", "TraitType", szTraitType);

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
			PRECONDITION(iTerrainID > -1 && iTerrainID < iNumTerrains);

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
	}
	//Populate m_piUpgradeUnitClass
	{
		std::string sqlKey = "UnitClassUpgrade";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if (pResults == NULL)
		{
			const char* szSQL = "select UnitClasses.ID, Units.ID from Trait_UnitClassUpgrade, UnitClasses, Units where TraitType = ? and UnitClassType = UnitClasses.Type and UnitType = Units.Type";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int unitClassesID = pResults->GetInt(0);
			const int unitID = pResults->GetInt(1);

			m_piUpgradeUnitClass.insert(std::pair<int, int>(unitClassesID, unitID));
		}

		pResults->Reset();

		//Trim extra memory off container since this is mostly read-only.
		std::multimap<int, int>(m_piUpgradeUnitClass).swap(m_piUpgradeUnitClass);
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
			PRECONDITION(iUnitCombatID > -1 && iUnitCombatID < iNumUnitCombatClasses);

			const int iMovesChange = pResults->GetInt(1);
			m_piMovesChangeUnitCombats[iUnitCombatID] = iMovesChange;
		}

		pResults->Reset();
	}
	//Populate m_ppiYieldFromTileEarnTerrainType
	{
		kUtility.Initialize2DArray(m_ppiYieldFromTileEarnTerrainType, "Terrains", "Yields");

		std::string strKey("Trait_YieldFromTileEarnTerrainType");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Terrains.ID as TerrainID, Yields.ID as YieldID, Yield from Trait_YieldFromTileEarnTerrainType inner join Terrains on Terrains.Type = TerrainType inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int TerrainID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiYieldFromTileEarnTerrainType[TerrainID][YieldID] = yield;
		}
	}
	//Populate m_ppiYieldFromTilePurchaseTerrainType
	{
		kUtility.Initialize2DArray(m_ppiYieldFromTilePurchaseTerrainType, "Terrains", "Yields");

		std::string strKey("Trait_YieldFromTilePurchaseTerrainType");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Terrains.ID as TerrainID, Yields.ID as YieldID, Yield from Trait_YieldFromTilePurchaseTerrainType inner join Terrains on Terrains.Type = TerrainType inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int TerrainID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiYieldFromTilePurchaseTerrainType[TerrainID][YieldID] = yield;
		}
	}
	//Populate m_ppiYieldFromTileConquest
	{
		kUtility.Initialize2DArray(m_ppiYieldFromTileConquest, "Terrains", "Yields");

		std::string strKey("Trait_YieldFromTileConquest");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Terrains.ID as TerrainID, Yields.ID as YieldID, Yield from Trait_YieldFromTileConquest inner join Terrains on Terrains.Type = TerrainType inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int TerrainID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiYieldFromTileConquest[TerrainID][YieldID] = yield;
		}
	}
	//Populate m_ppiYieldFromTileCultureBomb
	{
		kUtility.Initialize2DArray(m_ppiYieldFromTileCultureBomb, "Terrains", "Yields");

		std::string strKey("Trait_YieldFromTileCultureBomb");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Terrains.ID as TerrainID, Yields.ID as YieldID, Yield from Trait_YieldFromTileCultureBomb inner join Terrains on Terrains.Type = TerrainType inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int TerrainID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiYieldFromTileCultureBomb[TerrainID][YieldID] = yield;
		}
	}
	//Populate m_ppiYieldFromTileStealCultureBomb
	{
		kUtility.Initialize2DArray(m_ppiYieldFromTileStealCultureBomb, "Terrains", "Yields");

		std::string strKey("Trait_YieldFromTileStealCultureBomb");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Terrains.ID as TerrainID, Yields.ID as YieldID, Yield from Trait_YieldFromTileStealCultureBomb inner join Terrains on Terrains.Type = TerrainType inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int TerrainID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiYieldFromTileStealCultureBomb[TerrainID][YieldID] = yield;
		}
	}
	//Populate m_ppiYieldFromTileSettle
	{
		kUtility.Initialize2DArray(m_ppiYieldFromTileSettle, "Terrains", "Yields");

		std::string strKey("Trait_YieldFromTileSettle");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Terrains.ID as TerrainID, Yields.ID as YieldID, Yield from Trait_YieldFromTileSettle inner join Terrains on Terrains.Type = TerrainType inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int TerrainID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiYieldFromTileSettle[TerrainID][YieldID] = yield;
		}
	}
	//Populate m_ppiYieldChangePerImprovementBuilt
	{
		kUtility.Initialize2DArray(m_ppiYieldChangePerImprovementBuilt, "Improvements", "Yields");

		std::string strKey("Trait_YieldChangesPerImprovementBuilt");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Improvements.ID as ImprovementID, Yields.ID as YieldID, Yield from Trait_YieldChangesPerImprovementBuilt inner join Improvements on Improvements.Type = ImprovementType inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int ImprovementID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiYieldChangePerImprovementBuilt[ImprovementID][YieldID] = yield;
		}
	}
	//Populate m_pbiYieldFromBarbarianCampClear
	{
		std::string strKey("Trait_YieldFromBarbarianCampClears");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Yields.ID as YieldID, Yield, IsEraScaling from Trait_YieldFromBarbarianCampClears inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int iYieldType = pResults->GetInt(0);
			const int iYield = pResults->GetInt(1);
			const bool bEraScaling = pResults->GetBool(2);

			m_pbiYieldFromBarbarianCampClear[iYieldType][bEraScaling] = iYield;
		}

		pResults->Reset();

		//Trim extra memory off container since this is mostly read-only.
		std::map<int, std::map<bool, int>>(m_pbiYieldFromBarbarianCampClear).swap(m_pbiYieldFromBarbarianCampClear);
	}
	//Populate m_MovesChangeUnitClass
	{
		const int iNumUnitClasses = kUtility.MaxRows("UnitClasses");
		kUtility.InitializeArray(m_piMovesChangeUnitClasses, iNumUnitClasses, 0);

		std::string sqlKey = "Trait_MovesChangeUnitClass";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select UnitClasses.ID, MovesChange from Trait_MovesChangeUnitClass inner join UnitClasses on UnitClasses.Type = UnitClassType where TraitType = ?;";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int iUnitClassID = pResults->GetInt(0);
			PRECONDITION(iUnitClassID > -1 && iUnitClassID < iNumUnitClasses);

			const int iMovesChange = pResults->GetInt(1);
			m_piMovesChangeUnitClasses[iUnitClassID] = iMovesChange;
		}

		pResults->Reset();
	}
	//Populate m_FreePromotionUnitClass
	{
		std::string sqlKey = "FreePromotionUnitClass";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select UnitPromotions.ID, UnitClasses.ID from Trait_FreePromotionUnitClass, UnitPromotions, UnitClasses where TraitType = ? and PromotionType = UnitPromotions.Type and UnitClassType = UnitClasses.Type";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int unitPromotionID = pResults->GetInt(0);
			const int unitClassInfoID = pResults->GetInt(1);

			m_FreePromotionUnitClass.insert(std::pair<int, int>(unitPromotionID, unitClassInfoID));
		}

		pResults->Reset();

		//Trim extra memory off container since this is mostly read-only.
		std::multimap<int,int>(m_FreePromotionUnitClass).swap(m_FreePromotionUnitClass);
	}
	//Populate m_piGoldenAgeYieldModifier
	{
		std::string sqlKey = "Trait_GoldenAgeYieldModifiers";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if (pResults == NULL)
		{
			const char* szSQL = "select Yields.ID, Yield from Trait_GoldenAgeYieldModifiers, Yields where TraitType = ? and YieldType = Yields.Type";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int yieldID = pResults->GetInt(0);
			const int yield = pResults->GetInt(1);

			m_piGoldenAgeYieldModifier.insert(std::pair<int, int>(yieldID, yield));
		}

		pResults->Reset();

		//Trim extra memory off container since this is mostly read-only.
		std::map<int, int>(m_piGoldenAgeYieldModifier).swap(m_piGoldenAgeYieldModifier);
	}
	//Populate m_BuildsUnitClasses
	{
		std::string sqlKey = "BuildsUnitClasses";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select Builds.ID, UnitClasses.ID from Trait_BuildsUnitClasses, Builds, UnitClasses where TraitType = ? and BuildType = Builds.Type and UnitClassType = UnitClasses.Type";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int buildID = pResults->GetInt(0);
			const int unitClassInfoID = pResults->GetInt(1);

			m_BuildsUnitClasses.insert(std::pair<int, int>(buildID, unitClassInfoID));
		}

		pResults->Reset();

		//Trim extra memory off container since this is mostly read-only.
		std::multimap<int,int>(m_BuildsUnitClasses).swap(m_BuildsUnitClasses);
	}
	//Populate m_pibUnitCombatProductionCostModifier
	{
		std::string sqlKey = "Trait_UnitCombatProductionCostModifiers";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if (pResults == NULL)
		{
			const char* szSQL = "select UnitCombatInfos.ID as UnitCombatInfosID, CostModifier, IsGoldenAgeOnly from Trait_UnitCombatProductionCostModifiers inner join UnitCombatInfos on UnitCombatInfos.Type = UnitCombatType where TraitType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int iUnitCombat = pResults->GetInt(0);
			const int iCostModifier = pResults->GetInt(1);
			const bool bGoldenAgeOnly = pResults->GetBool(2);

			m_pibUnitCombatProductionCostModifier.insert(std::pair<int, std::pair<int, bool>>(iUnitCombat, std::make_pair(iCostModifier, bGoldenAgeOnly)));
		}

		pResults->Reset();

		//Trim extra memory off container since this is mostly read-only.
		std::map<int, std::pair<int, bool>>(m_pibUnitCombatProductionCostModifier).swap(m_pibUnitCombatProductionCostModifier);
	}

	//Populate m_aiNoBuilds
	{
		std::string sqlKey = "Trait_NoBuilds";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if (pResults == NULL)
		{
			const char* szSQL = "select Builds.ID from Trait_NoBuilds, Builds where TraitType = ? and BuildType = Builds.Type";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int iBuild = pResults->GetInt(0);

			std::vector<int>::const_iterator it = find(m_aiNoBuilds.begin(), m_aiNoBuilds.end(), iBuild); // does this build id already exist in our vector?

			if (it == m_aiNoBuilds.end()) // only add an entry if it does not exist
			{
				m_aiNoBuilds.push_back(iBuild);
			}
		}

		pResults->Reset();

		//Trim extra memory off container since this is mostly read-only.
		std::vector<int>(m_aiNoBuilds).swap(m_aiNoBuilds);
	}

	//Populate m_piDomainProductionModifiersPerSpecialist
	{
		std::string sqlKey = "Trait_DomainProductionModifiersPerSpecialist";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if (pResults == NULL)
		{
			const char* szSQL = "select Domains.ID as DomainID, Modifier from Trait_DomainProductionModifiersPerSpecialist inner join Domains on Domains.Type = DomainType where TraitType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int iDomain = pResults->GetInt(0);
			const int iModifier = pResults->GetInt(1);

			m_piDomainProductionModifiersPerSpecialist.insert(std::pair<int, int>(iDomain, iModifier));
		}

		pResults->Reset();

		//Trim extra memory off container since this is mostly read-only.
		std::map<int, int>(m_piDomainProductionModifiersPerSpecialist).swap(m_piDomainProductionModifiersPerSpecialist);
	}

	//Populate m_pbiYieldFromRouteMovementInForeignTerritory
	{
		std::string sqlKey = "Trait_YieldFromRouteMovementInForeignTerritory";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if (pResults == NULL)
		{
			const char* szSQL = "select Yields.ID as YieldID, Yield, IsGrantedToTradePartner from Trait_YieldFromRouteMovementInForeignTerritory inner join Yields on Yields.Type = YieldType where TraitType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int iYieldType = pResults->GetInt(0);
			const int iYield = pResults->GetInt(1);
			const bool bGrantedToTradePartner = pResults->GetBool(2);

			m_pbiYieldFromRouteMovementInForeignTerritory[iYieldType][bGrantedToTradePartner] += iYield;
		}

		pResults->Reset();

		//Trim extra memory off container since this is mostly read-only.
		std::map<int, std::map<bool, int>>(m_pbiYieldFromRouteMovementInForeignTerritory).swap(m_pbiYieldFromRouteMovementInForeignTerritory);
	}

	//Populate m_biiTradeRouteProductionSiphon
	{
		std::string sqlKey = "Trait_TradeRouteProductionSiphon";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if (pResults == NULL)
		{
			const char* szSQL = "select SiphonPercent, PercentIncreaseWithOpenBorders, IsInternationalOnly from Trait_TradeRouteProductionSiphon where TraitType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int iSiphonPercent = pResults->GetInt(0);
			const int iPercentIncreaseWithOpenBorders = pResults->GetInt(1);
			const bool bInternationalOnly = pResults->GetBool(2);

			m_biiTradeRouteProductionSiphon[bInternationalOnly].m_iSiphonPercent += iSiphonPercent;
			m_biiTradeRouteProductionSiphon[bInternationalOnly].m_iPercentIncreaseWithOpenBorders += iPercentIncreaseWithOpenBorders;
		}

		pResults->Reset();

		//Trim extra memory off container since this is mostly read-only.
		std::map<bool, TradeRouteProductionSiphon>(m_biiTradeRouteProductionSiphon).swap(m_biiTradeRouteProductionSiphon);
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
			PRECONDITION(iUnitCombatID > -1 && iUnitCombatID < iNumUnitCombatClasses);

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

	//PlotYieldChanges
	{
		kUtility.Initialize2DArray(m_ppiPlotYieldChanges, "Plots", "Yields");

		std::string strKey("Trait_PlotYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Plots.ID as PlotID, Yields.ID as YieldID, Yield from Trait_PlotYieldChanges inner join Plots on Plots.Type = PlotType inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int PlotID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiPlotYieldChanges[PlotID][YieldID] = yield;
		}
	}

	kUtility.SetYields(m_piYieldFromLevelUp, "Trait_YieldFromLevelUp", "TraitType", szTraitType);
	kUtility.SetYields(m_piYieldFromHistoricEvent, "Trait_YieldFromHistoricEvent", "TraitType", szTraitType);
	kUtility.SetYields(m_piYieldFromXMilitaryUnits, "Trait_YieldFromXMilitaryUnits", "TraitType", szTraitType);
	kUtility.SetYields(m_piYieldFromOwnPantheon, "Trait_YieldFromOwnPantheon", "TraitType", szTraitType);
	// Trait_TradeRouteEndYield
	{
		const std::string sqlKey = "Trait_TradeRouteEndYield";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if (pResults == NULL)
		{
			const char* szSQL = "select Yields.ID as YieldsID, YieldDomestic, YieldInternational from Trait_TradeRouteEndYield inner join Yields on Yields.Type = YieldType where TraitType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int yieldID = pResults->GetInt(0);
			const int yieldDomestic = pResults->GetInt(1);
			const int yieldInternational = pResults->GetInt(2);

			if (yieldDomestic != 0 || yieldInternational != 0)
			{
				const std::map<int, std::pair<int, int>>::iterator it = m_tradeRouteEndYield.find(yieldID);
				if (it != m_tradeRouteEndYield.end())
				{
					std::pair<int, int>& yieldValues = it->second;
					yieldValues.first += yieldDomestic;
					yieldValues.second += yieldInternational;

					if (yieldValues.first == 0 && yieldValues.second == 0)
					{
						m_tradeRouteEndYield.erase(it);
					}
				}
				else
				{
					const std::pair<int, int> yieldValues = std::make_pair(yieldDomestic, yieldInternational);
					m_tradeRouteEndYield.insert(it, std::make_pair(yieldID, yieldValues));
				}
			}
		}

		pResults->Reset();
	}
	kUtility.SetYields(m_piYieldFromRouteMovement, "Trait_YieldFromRouteMovement", "TraitType", szTraitType);
	kUtility.SetYields(m_piYieldFromExport, "Trait_YieldFromExport", "TraitType", szTraitType);
	kUtility.SetYields(m_piYieldFromImport, "Trait_YieldFromImport", "TraitType", szTraitType);
	kUtility.SetYields(m_piYieldFromTilePurchase, "Trait_YieldFromTilePurchase", "TraitType", szTraitType);
	kUtility.SetYields(m_piYieldFromTileEarn, "Trait_YieldFromTileEarn", "TraitType", szTraitType);
	kUtility.SetYields(m_piYieldFromCSAlly, "Trait_YieldFromCSAlly", "TraitType", szTraitType);
	kUtility.SetYields(m_piYieldFromCSFriend, "Trait_YieldFromCSFriend", "TraitType", szTraitType);
	kUtility.SetYields(m_piYieldFromSettle, "Trait_YieldFromSettle", "TraitType", szTraitType);
	kUtility.SetYields(m_piYieldFromConquest, "Trait_YieldFromConquest", "TraitType", szTraitType);
	kUtility.SetYields(m_piYieldFromCityDamageTimes100, "Trait_YieldFromCityDamageTimes100", "TraitType", szTraitType);
	m_iPurchasedUnitsBonusXP = kResults.GetInt("PurchasedUnitsBonusXP");
	m_iVotePerXCSAlliance = kResults.GetInt("VotePerXCSAlliance");
	m_iVotePerXCSFollowingFollowingYourReligion = kResults.GetInt("VotePerXCSFollowingYourReligion");
	m_iChanceToConvertReligiousUnits = kResults.GetInt("ChanceToConvertReligiousUnits");
	m_iGoldenAgeFromVictory = kResults.GetInt("GoldenAgeFromVictory");
	m_iFreePolicyPerXTechs = kResults.GetInt("FreePolicyPerXTechs");

	szTextVal = kResults.GetText("GPFaithPurchaseEra");
	if (szTextVal)
	{
		m_eGPFaithPurchaseEra = (EraTypes)GC.getInfoTypeForString(szTextVal, true);
	}
	m_iFaithCostModifier = kResults.GetInt("FaithCostModifier");
	m_bFreeGreatWorkOnConquest = kResults.GetBool("FreeGreatWorkOnConquest");
	m_bPopulationBoostReligion = kResults.GetBool("PopulationBoostReligion");
	m_bStartsWithPantheon = kResults.GetBool("StartsWithPantheon");
	m_bProphetFervor = kResults.GetBool("ProphetFervor");
	m_bCombatBoostNearNaturalWonder = kResults.GetBool("CombatBoostNearNaturalWonder");

	//BuildingClassYieldChanges
	{
		kUtility.Initialize2DArray(m_ppiBuildingClassYieldChanges, "BuildingClasses", "Yields");

		std::string strKey("Trait_BuildingClassYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select BuildingClasses.ID as BuildingClassID, Yields.ID as YieldID, YieldChange from Trait_BuildingClassYieldChanges inner join BuildingClasses on BuildingClasses.Type = BuildingClassType inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int BuildingClassID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiBuildingClassYieldChanges[BuildingClassID][YieldID] = yield;
		}
	}

	kUtility.SetYields(m_piCapitalYieldChanges, "Trait_CapitalYieldChanges", "TraitType", szTraitType);
	kUtility.SetYields(m_piCityYieldChanges, "Trait_CityYieldChanges", "TraitType", szTraitType);
	kUtility.SetYields(m_piPermanentYieldChangeWLTKD, "Trait_PermanentYieldChangeWLTKD", "TraitType", szTraitType);
	kUtility.SetYields(m_piCoastalCityYieldChanges, "Trait_CoastalCityYieldChanges", "TraitType", szTraitType);
	kUtility.SetYields(m_piGreatWorkYieldChanges, "Trait_GreatWorkYieldChanges", "TraitType", szTraitType);
	kUtility.SetYields(m_piArtifactYieldChanges, "Trait_ArtifactYieldChanges", "TraitType", szTraitType);
	kUtility.SetYields(m_piArtYieldChanges, "Trait_ArtYieldChanges", "TraitType", szTraitType);
	kUtility.SetYields(m_piLitYieldChanges, "Trait_LitYieldChanges", "TraitType", szTraitType);
	kUtility.SetYields(m_piMusicYieldChanges, "Trait_MusicYieldChanges", "TraitType", szTraitType);
	kUtility.SetYields(m_piSeaPlotYieldChanges, "Trait_SeaPlotYieldChanges", "TraitType", szTraitType);
	kUtility.SetYields(m_piLuxuryYieldChanges, "Trait_LuxuryYieldChanges", "TraitType", szTraitType);

	//FeatureYieldChanges
	{
		kUtility.Initialize2DArray(m_ppiFeatureYieldChanges, "Features", "Yields");

		std::string strKey("Trait_FeatureYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Features.ID as FeatureID, Yields.ID as YieldID, Yield from Trait_FeatureYieldChanges inner join Features on Features.Type = FeatureType inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int FeatureID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiFeatureYieldChanges[FeatureID][YieldID] = yield;
		}
	}
	
	//ResourceYieldChanges
	{
		kUtility.Initialize2DArray(m_ppiResourceYieldChanges, "Resources", "Yields");

		std::string strKey("Trait_ResourceYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Resources.ID as ResourceID, Yields.ID as YieldID, Yield from Trait_ResourceYieldChanges inner join Resources on Resources.Type = ResourceType inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int ResourceID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiResourceYieldChanges[ResourceID][YieldID] = yield;
		}
	}

	// Trait_ResourceYieldChangesFromGoldenAge
	{
		std::string strKey("Trait_ResourceYieldChangesFromGoldenAge");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Resources.ID as ResourceID, Yields.ID as YieldID, Yield, YieldCap from Trait_ResourceYieldChangesFromGoldenAge inner join Resources on Resources.Type = ResourceType inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int iResourceID = pResults->GetInt(0);
			const int iYieldType = pResults->GetInt(1);
			const int iYield = pResults->GetInt(2);
			const int iYieldCap = pResults->GetInt(3);

			m_miResourceYieldChangesFromGoldenAge[iResourceID][iYieldType] += iYield;
			m_miResourceYieldChangesFromGoldenAgeCap[iResourceID][iYieldType] += iYieldCap;
		}

		pResults->Reset();

		//Trim extra memory off container since this is mostly read-only.
		std::map<int, std::map<int, int>>(m_miResourceYieldChangesFromGoldenAge).swap(m_miResourceYieldChangesFromGoldenAge);
		std::map<int, std::map<int, int>>(m_miResourceYieldChangesFromGoldenAgeCap).swap(m_miResourceYieldChangesFromGoldenAgeCap);
	}
	
	//TerrainYieldChanges
	{
		kUtility.Initialize2DArray(m_ppiTerrainYieldChanges, "Terrains", "Yields");

		std::string strKey("Trait_TerrainYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Terrains.ID as TerrainID, Yields.ID as YieldID, Yield from Trait_TerrainYieldChanges inner join Terrains on Terrains.Type = TerrainType inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int TerrainID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiTerrainYieldChanges[TerrainID][YieldID] = yield;
		}
	}
	
	kUtility.SetYields(m_piYieldFromKills, "Trait_YieldFromKills", "TraitType", szTraitType);
	kUtility.SetYields(m_piYieldFromBarbarianKills, "Trait_YieldFromBarbarianKills", "TraitType", szTraitType);
	kUtility.SetYields(m_piYieldFromMinorDemand, "Trait_YieldFromMinorDemand", "TraitType", szTraitType);
	kUtility.SetYields(m_piYieldFromLuxuryResourceGain, "Trait_YieldFromLuxuryResourceGain", "TraitType", szTraitType);
	kUtility.SetYields(m_piYieldChangeTradeRoute, "Trait_YieldChangeTradeRoute", "TraitType", szTraitType);
	kUtility.SetYields(m_piYieldChangeWorldWonder, "Trait_YieldChangeWorldWonder", "TraitType", szTraitType);

	//TradeRouteYieldChange
	{
		kUtility.Initialize2DArray(m_ppiTradeRouteYieldChange, "Domains", "Yields");

		std::string strKey("Building_TradeRouteYieldChange");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Domains.ID as DomainID, Yields.ID as YieldID, Yield from Trait_TradeRouteYieldChange inner join Domains on Domains.Type = DomainType inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int DomainID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiTradeRouteYieldChange[DomainID][YieldID] = yield;
		}
	}

	//SpecialistYieldChanges
	{
		kUtility.Initialize2DArray(m_ppiSpecialistYieldChanges, "Specialists", "Yields");

		std::string strKey("Trait_SpecialistYieldChanges");
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

	//GreatPersonExpendedYield
	{
		kUtility.Initialize2DArray(m_ppiGreatPersonExpendedYield, "GreatPersons", "Yields");

		std::string strKey("Trait_GreatPersonExpendedYield");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select GreatPersons.ID as GreatPersonID, Yields.ID as YieldID, Yield from Trait_GreatPersonExpendedYield inner join GreatPersons on GreatPersons.Type = GreatPersonType inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int GreatPersonID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiGreatPersonExpendedYield[GreatPersonID][YieldID] = yield;
		}
	}
	//GreatPersonBornYield
	{
		kUtility.Initialize2DArray(m_ppiGreatPersonBornYield, "GreatPersons", "Yields");

		std::string strKey("Trait_GreatPersonBornYield");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select GreatPersons.ID as GreatPersonID, Yields.ID as YieldID, Yield from Trait_GreatPersonBornYield inner join GreatPersons on GreatPersons.Type = GreatPersonType inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int GreatPersonID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiGreatPersonBornYield[GreatPersonID][YieldID] = yield;
		}
	}

	kUtility.PopulateArrayByValue(m_piGreatPersonCostReduction, "GreatPersons", "Trait_GreatPersonCostReduction", "GreatPersonType", "TraitType", szTraitType, "Modifier");
	kUtility.PopulateArrayByValue(m_piGoldenAgeGreatPersonRateModifier, "GreatPersons", "Trait_GoldenAgeGreatPersonRateModifier", "GreatPersonType", "TraitType", szTraitType, "Modifier");
	kUtility.PopulateArrayByValue(m_piPerPuppetGreatPersonRateModifier, "GreatPersons", "Trait_PerPuppetGreatPersonRateModifier", "GreatPersonType", "TraitType", szTraitType, "Modifier");
	kUtility.PopulateArrayByValue(m_piGreatPersonGWAM, "GreatPersons", "Trait_GreatPersonBirthGWAM", "GreatPersonType", "TraitType", szTraitType, "Value");
	
	//CityYieldFromUnimprovedFeature
	{
		kUtility.Initialize2DArray(m_ppiCityYieldFromUnimprovedFeature, "Features", "Yields");

		std::string strKey("Trait_CityYieldFromUnimprovedFeature");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Features.ID as FeatureID, Yields.ID as YieldID, Yield from Trait_CityYieldFromUnimprovedFeature inner join Features on Features.Type = FeatureType inner join Yields on Yields.Type = YieldType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while(pResults->Step())
		{
			const int FeatureID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiCityYieldFromUnimprovedFeature[FeatureID][YieldID] = yield;
		}
	}
	// TerrainClaimBoost
	{
		int iTerrainLoop = 0;
		for (iTerrainLoop = 0; iTerrainLoop < GC.getNumTerrainInfos(); iTerrainLoop++)
		{
			m_abTerrainClaimBoost.push_back(false);
		}

		std::string strKey("Trait_TerrainClaimBoost");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "SELECT Traits.ID, Terrains.ID FROM Trait_TerrainClaimBoost inner join Traits on Trait_TerrainClaimBoost.TraitType = Traits.Type inner join Terrains on Trait_TerrainClaimBoost.TerrainType = Terrains.Type where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int iTerrainType = pResults->GetInt(1);
			m_abTerrainClaimBoost[iTerrainType] = true;
		}
	}

	kUtility.PopulateArrayByValue(m_piGoldenAgeFromGreatPersonBirth, "GreatPersons", "Trait_GoldenAgeFromGreatPersonBirth", "GreatPersonType", "TraitType", szTraitType, "GoldenAgeTurns");
	kUtility.PopulateArrayByValue(m_piGreatPersonProgressFromPolicyUnlock, "GreatPersons", "Trait_GreatPersonProgressFromPolicyUnlock", "GreatPersonType", "TraitType", szTraitType, "Value");

	//GreatPersonProgressFromKills
	{
		std::string strKey("Trait_GreatPersonProgressFromKills");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select GreatPersons.ID as GreatPersonID, Value from Trait_GreatPersonProgressFromKills inner join GreatPersons on GreatPersons.Type = GreatPersonType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int iGreatPerson = pResults->GetInt(0);
			const int iValue = pResults->GetInt(1);

			m_piGreatPersonProgressFromKills[iGreatPerson] += iValue;
		}

		pResults->Reset();

		//Trim extra memory off container since this is mostly read-only.
		std::map<int, int>(m_piGreatPersonProgressFromKills).swap(m_piGreatPersonProgressFromKills);
	}

	//RandomGreatPersonProgressFromKills
	{
		std::string strKey("Trait_RandomGreatPersonProgressFromKills");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select GreatPersons.ID as GreatPersonID, Value from Trait_RandomGreatPersonProgressFromKills inner join GreatPersons on GreatPersons.Type = GreatPersonType where TraitType = ?");
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int iGreatPerson = pResults->GetInt(0);
			const int iValue = pResults->GetInt(1);

			m_piRandomGreatPersonProgressFromKills[iGreatPerson] += iValue;
		}

		pResults->Reset();

		//Trim extra memory off container since this is mostly read-only.
		std::map<int, int>(m_piRandomGreatPersonProgressFromKills).swap(m_piRandomGreatPersonProgressFromKills);
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
		int iUnitClassLoop = 0;
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
		int iResourceLoop = 0;
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

	//Populate m_piiAlternateResourceTechs
	{
		std::string sqlKey = "Trait_AlternateResourceTechs";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if (pResults == NULL)
		{
			const char* szSQL = "select Resources.ID as ResourceID, A.TechReveal, A.TechCityTrade from Trait_AlternateResourceTechs A inner join Resources on Resources.Type = ResourceType where TraitType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szTraitType);

		while (pResults->Step())
		{
			const int iResource = pResults->GetInt(0);
			AlternateResourceTechs pTechs;
			pTechs.m_eTechReveal = (TechTypes)GC.getInfoTypeForString(pResults->GetText("TechReveal"), true);
			pTechs.m_eTechCityTrade = (TechTypes)GC.getInfoTypeForString(pResults->GetText("TechCityTrade"), true);

			m_piiAlternateResourceTechs[iResource] = pTechs;
		}

		pResults->Reset();

		//Trim extra memory off container since this is mostly read-only.
		std::map<int, AlternateResourceTechs>(m_piiAlternateResourceTechs).swap(m_piiAlternateResourceTechs);
	}

	m_piGoldenAgeGreatPersonRateModifier[GC.getInfoTypeForString("GREATPERSON_WRITER")] += m_iGoldenAgeGreatWriterRateModifier;
	m_piGoldenAgeGreatPersonRateModifier[GC.getInfoTypeForString("GREATPERSON_ARTIST")] += m_iGoldenAgeGreatArtistRateModifier;
	m_piGoldenAgeGreatPersonRateModifier[GC.getInfoTypeForString("GREATPERSON_MUSICIAN")] += m_iGoldenAgeGreatMusicianRateModifier;

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
	return (index!=NO_TRAIT) ? m_paTraitEntries[index] : NULL;
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

	m_vLeaderHasTrait = std::vector<bool>( GC.getNumTraitInfos(), false );
}

void CvPlayerTraits::SetIsWarmonger()
{
	if (GetGreatGeneralRateModifier() > 0 ||
		GetGreatGeneralExtraBonus() > 0 ||
		GetLevelExperienceModifier() < 0 ||
		GetCityStateCombatModifier() > 0 ||
		GetLandBarbarianConversionPercent() > 0 ||
		GetLandBarbarianConversionExtraUnits() > 0 ||
		GetSeaBarbarianConversionPercent() > 0 ||
		GetCultureFromKills() > 0 ||
		GetFaithFromKills() > 0 ||
		GetPlunderModifier() > 0 ||
		GetGoldenAgeMoveChange() > 0 ||
		GetGoldenAgeCombatModifier() > 0 ||
		GetExtraEmbarkMoves() > 0 ||
		GetBullyMilitaryStrengthModifier() > 0 ||
		GetXPBonusFromGreatPersonBirth() > 0 ||
		GetUnitHealFromGreatPersonBirth() > 0 ||
		GetBullyValueModifier() > 0 ||
		GetMultipleAttackBonus() > 0 ||
		GetCityConquestGWAM() > 0 ||
		GetLandUnitMaintenanceModifier() < 0 ||
		GetNavalUnitMaintenanceModifier() < 0 ||
		GetProductionBonusModifierConquest() > 0 ||
		GetGoldenAgeFromVictory() > 0 ||
		GetWarWearinessModifier() > 0 ||
		GetEnemyWarWearinessModifier() > 0 ||
		GetBullyYieldMultiplierAnnex() > 0 ||
		(GetPuppetPenaltyReduction() > 0 && !IsNoAnnexing()) || // puppet & annexing - Warmonger, puppet & no annexing - Smaller
		GetGoldenAgeFromGreatPersonBirth(static_cast<GreatPersonTypes>(GC.getInfoTypeForString("GREATPERSON_GENERAL"))) > 0 ||
		GetGoldenAgeFromGreatPersonBirth(static_cast<GreatPersonTypes>(GC.getInfoTypeForString("GREATPERSON_ADMIRAL"))) > 0 ||
		GetGreatPersonGWAM(static_cast<GreatPersonTypes>(GC.getInfoTypeForString("GREATPERSON_GENERAL"))) > 0 ||
		GetGreatPersonGWAM(static_cast<GreatPersonTypes>(GC.getInfoTypeForString("GREATPERSON_ADMIRAL"))) > 0 ||
		IsTechFromCityConquer())

	{
		m_bIsWarmonger = true;
		return;
	}

	if (IsKeepConqueredBuildings() ||
		IsCanPurchaseNavalUnitsFaith() ||
		IsBullyAnnex() ||
		IgnoreBullyPenalties() ||
		IsAnnexedCityStatesGiveYields() ||
		IsFightWellDamaged() ||
		IsEmbarkedToLandFlatCost() ||
		IsRandomGreatPersonProgressFromKills())
	{
		m_bIsWarmonger = true;
		return;
	}

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		YieldTypes eYield = (YieldTypes)iYield;
		if (GetYieldFromLevelUp(eYield) > 0 ||
			GetYieldFromConquest(eYield) > 0 ||
			GetYieldFromCityDamageTimes100(eYield) > 0)
		{
			m_bIsWarmonger = true;
			return;
		}

		for (int iTerrain = 0; iTerrain < GC.getNumTerrainInfos(); iTerrain++)
		{
			TerrainTypes eTerrain = (TerrainTypes)iTerrain;
			if (GetYieldChangeFromTileConquest(eTerrain, eYield) > 0 ||
				GetYieldChangeFromTileCultureBomb(eTerrain, eYield) > 0 ||
				GetYieldChangeFromTileStealCultureBomb(eTerrain, eYield) > 0)
			{
				m_bIsWarmonger = true;
				return;
			}
		}
	}

	// Not optimal, since this assumes the free promotions are always combat-based.
	// But there isn't a good way to classify promotions.
	for (int iNumPromos = 0; iNumPromos < GC.getNumPromotionInfos(); iNumPromos++)
	{
		for (int iNumUnits = 0; iNumUnits < GC.getNumUnitCombatClassInfos(); iNumUnits++)
		{
			UnitCombatTypes eUClass = (UnitCombatTypes)iNumUnits;
			if (eUClass == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_RECON", true))
				continue;

			if (HasFreePromotionUnitCombat((PromotionTypes)iNumPromos, eUClass))
			{

				m_bIsWarmonger = true;
				return;
			}
		}
		for (int iNumUnits = 0; iNumUnits < GC.getNumUnitClassInfos(); iNumUnits++)
		{
			UnitClassTypes eUClass = (UnitClassTypes)iNumUnits;
			if (HasFreePromotionUnitClass((PromotionTypes)iNumPromos, eUClass))
			{
				m_bIsWarmonger = true;
				return;
			}
		}
	}

	if (!GetFreePromotions().empty())
	{
		m_bIsWarmonger = true;
		return;
	}

	for (int iDomain = 0; iDomain < NUM_DOMAIN_TYPES; iDomain++)
	{
		DomainTypes eDomain = (DomainTypes)iDomain;
		if (GetDomainFreeExperienceModifier(eDomain) > 0 ||
			GetDomainProductionModifiersPerSpecialist(eDomain) > 0)
		{
			m_bIsWarmonger = true;
			return;
		}
	}

	for (int iUnitCombat = 0; iUnitCombat < GC.getNumUnitCombatClassInfos(); iUnitCombat++)
	{
		UnitCombatTypes eUnitCombat = (UnitCombatTypes)iUnitCombat;
		if (GetUnitCombatProductionCostModifier(eUnitCombat).first < 0 && GetUnitCombatProductionCostModifier(eUnitCombat).second == false) // If we get a unit production cost reduction outside of golden ages
		{
			m_bIsWarmonger = true;
			return;
		}
	}

	for (int iGreatPerson = 0; iGreatPerson < GC.getNumGreatPersonInfos(); iGreatPerson++)
	{
		GreatPersonTypes eGreatPerson = (GreatPersonTypes)iGreatPerson;
		if (GetGreatPersonProgressFromKills(eGreatPerson) > 0)
		{
			m_bIsWarmonger = true;
			return;
		}
	}
}

void CvPlayerTraits::SetIsNerd()
{
	if (GetGreatScientistRateModifier() > 0 ||
		GetInvestmentModifier() < 0 ||
		GetFreePolicyPerXTechs() > 0 ||
		GetGoldenAgeFromGreatPersonBirth(static_cast<GreatPersonTypes>(GC.getInfoTypeForString("GREATPERSON_SCIENTIST"))) > 0 ||
		GetGoldenAgeYieldModifier(YIELD_SCIENCE) > 0)
	{
		m_bIsNerd = true;
		return;
	}

	if (IsAdoptionFreeTech() ||
		IsTechBoostFromCapitalScienceBuildings() ||
		IsMayaCalendarBonuses() ||
		IsTechFromCityConquer())
	{
		m_bIsNerd = true;
		return;
	}

	for (int iSpecialist = 0; iSpecialist < GC.getNumSpecialistInfos(); iSpecialist++)
	{
		SpecialistTypes eSpecialist = (SpecialistTypes)iSpecialist;
		if (GetSpecialistYieldChange(eSpecialist, YIELD_SCIENCE) > 0)
		{
			m_bIsNerd = true;
			return;
		}
	}
}

void CvPlayerTraits::SetIsTourism()
{
	if (GetGreatPeopleRateModifier() > 0 ||
		GetGreatPersonGiftInfluence() > 0 ||
		GetCityCultureBonus() > 0 ||
		GetCapitalThemingBonusModifier() > 0 ||
		GetPolicyCostModifier() < 0 ||
		GetWonderProductionModifier() > 0 ||
		GetGoldenAgeTourismModifier() > 0 ||
		GetGoldenAgeGreatPersonRateModifier(static_cast<GreatPersonTypes>(GC.getInfoTypeForString("GREATPERSON_ARTIST"))) > 0 ||
		GetGoldenAgeGreatPersonRateModifier(static_cast<GreatPersonTypes>(GC.getInfoTypeForString("GREATPERSON_MUSICIAN"))) > 0 ||
		GetGoldenAgeGreatPersonRateModifier(static_cast<GreatPersonTypes>(GC.getInfoTypeForString("GREATPERSON_WRITER"))) > 0 ||
		GetTourismGABonus() > 0 ||
		GetTourismToGAP() > 0 ||
		GetGoldToGAP() > 0 ||
		GetEventTourismBoost() > 0 ||
		GetEventGP() > 0 ||
		GetWLTKDCulture() > 0 ||
		GetExtraConqueredCityTerritoryClaimRange() > 0 ||
		GetExtraTenetsFirstAdoption() > 0 ||
		GetFreeSocialPoliciesPerEra() > 0 ||
		GetCultureBonusModifierConquest() > 0 ||
		GetGoldenAgeFromGreatPersonBirth(static_cast<GreatPersonTypes>(GC.getInfoTypeForString("GREATPERSON_ARTIST"))) > 0 ||
		GetGoldenAgeFromGreatPersonBirth(static_cast<GreatPersonTypes>(GC.getInfoTypeForString("GREATPERSON_MUSICIAN"))) > 0 ||
		GetGoldenAgeFromGreatPersonBirth(static_cast<GreatPersonTypes>(GC.getInfoTypeForString("GREATPERSON_WRITER"))) > 0 ||
		GetXPBonusFromGreatPersonBirth() > 0 ||
		GetUnitHealFromGreatPersonBirth() > 0 ||
		GetSharedReligionTourismModifier() > 0 ||
		GetGoldenAgeYieldModifier(YIELD_TOURISM) > 0)
	{
		m_bIsTourism = true;
		return;
	}

	if (IsGPWLTKD() ||
		IsGreatWorkWLTKD())
	{
		m_bIsTourism = true;
		return;
	}

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		YieldTypes eYield = (YieldTypes)iYield;
		if (GetYieldFromHistoricEvent(eYield) > 0)
		{
			m_bIsTourism = true;
			return;
		}
	}
}
void CvPlayerTraits::SetIsDiplomat()
{
	if (GetCityStateBonusModifier() > 0 ||
		GetCityStateFriendshipModifier() > 0 ||
		GetAllianceCSDefense() > 0 ||
		GetAllianceCSStrength() > 0 ||
		GetInfluenceMeetCS() > 0 ||
		GetStartingSpies() > 0 ||
		GetNumTradeRoutesModifier() > 0 ||
		GetLandTradeRouteRangeBonus() > 0 ||
		GetSeaTradeRouteRangeBonus() > 0 ||
		GetQuestYieldModifier() > 0 ||
		GetLuxuryHappinessRetention() > 0 ||
		GetTradeBuildingModifier() > 0 ||
		GetVotePerXCSAlliance() > 0 ||
		GetMinorInfluencePerGiftedUnit() > 0 ||
		(MOD_BALANCE_VP && GetGoldenAgeFromGreatPersonBirth(static_cast<GreatPersonTypes>(GC.getInfoTypeForString("GREATPERSON_DIPLOMAT"))) > 0))
	{
		m_bIsDiplomat = true;
		return;
	}

	if (IsImportsCountTowardsMonopolies() ||
		IsDiplomaticMarriage() ||
		IsAbleToAnnexCityStates() ||
		IsCanPlunderWithoutWar() ||
		IsNoTradeRouteProximityPenalty())
	{
		m_bIsDiplomat = true;
		return;
	}

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		YieldTypes eYield = (YieldTypes)iYield;
		if (GetYieldChangePerTradePartner(eYield) > 0 ||
			GetYieldFromCSAlly(eYield) > 0 ||
			GetYieldFromCSFriend(eYield) > 0)
		{
			m_bIsDiplomat = true;
			return;
		}
	}
}
void CvPlayerTraits::SetIsSmaller()
{
	if (GetGreatScientistRateModifier() > 0 ||
		GetFreePolicyPerXTechs() > 0 ||
		GetGreatPeopleRateModifier() > 0 ||
		GetCapitalThemingBonusModifier() > 0 ||
		GetPolicyCostModifier() < 0 ||
		GetWonderProductionModifier() > 0 ||
		GetEventTourismBoost() > 0 ||
		GetEventGP() > 0)
	{
		m_bIsSmaller = true;
		return;
	}

	if (IsImportsCountTowardsMonopolies() ||
		IsAdoptionFreeTech() ||
		IsPopulationBoostReligion() ||
		IsProphetFervor() ||
		IsNoAnnexing() ||
		IsTechBoostFromCapitalScienceBuildings() ||
		IsRandomGreatPersonProgressFromKills())
	{
		m_bIsSmaller = true;
		return;
	}

	for (int iDomain = 0; iDomain < NUM_DOMAIN_TYPES; iDomain++)
	{
		DomainTypes eDomain = (DomainTypes)iDomain;
		if (GetDomainProductionModifiersPerSpecialist(eDomain) > 0)
		{
			m_bIsSmaller = true;
			return;
		}
	}

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		YieldTypes eYield = (YieldTypes)iYield;
		if (GetYieldChangePerTradePartner(eYield) > 0 ||
			GetYieldFromCSAlly(eYield) > 0 ||
			GetYieldFromCSFriend(eYield) > 0 ||
			GetYieldChangeWorldWonder(eYield) > 0)
		{
			m_bIsSmaller = true;
			return;
		}

		for (int iSpecialist = 0; iSpecialist < GC.getNumSpecialistInfos(); iSpecialist++)
		{
			SpecialistTypes eSpecialist = (SpecialistTypes)iSpecialist;
			if (GetSpecialistYieldChange(eSpecialist, eYield) > 0)
			{
				m_bIsSmaller = true;
				return;
			}
		}
	}

	for (int iGreatPerson = 0; iGreatPerson < GC.getNumGreatPersonInfos(); iGreatPerson++)
	{
		GreatPersonTypes eGreatPerson = (GreatPersonTypes)iGreatPerson;
		if (GetGreatPersonProgressFromKills(eGreatPerson) > 0)
		{
			m_bIsSmaller = true;
			return;
		}
	}

	if (GC.getGame().getGameTurn() / 2 > GC.getGame().getEstimateEndTurn())
	{
		if (m_pPlayer->getNumCities() < 4)
		{
			m_bIsSmaller = true;
			return;
		}
	}
}

void CvPlayerTraits::SetIsExpansionist()
{
	if (GetPlotBuyCostModifier() < 0 ||
		GetCityWorkingChange() > 0 ||
		GetPlotCultureCostModifier() < 0 ||
		GetNaturalWonderFirstFinderGold() > 0 ||
		GetNaturalWonderSubsequentFinderGold() > 0 ||
		GetNaturalWonderYieldModifier() > 0 ||
		GetNaturalWonderHappinessModifier() > 0 ||
		GetGrowthBoon() > 0 ||
		GetWLTKDUnhappinessNeedsMod() < 0 ||
		GetUniqueLuxuryCities() > 0 ||
		GetExtraFoundedCityTerritoryClaimRange() > 0 ||
		GetPolicyGEorGM() > 0 ||
		GetNonSpecialistFoodChange() > 0)
	{
		m_bIsExpansionist = true;
		return;
	}

	if (IsBuyOwnedTiles() ||
		IsExpansionWLTKD() ||
		IsWoodlandMovementBonus() ||
		IsRiverMovementBonus() ||
		IsFasterInHills() ||
		IsEmbarkedAllWater() ||
		IsRiverTradeRoad() ||
		IsNoConnectionUnhappiness())
	{
		m_bIsExpansionist = true;
		return;
	}

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		YieldTypes eYield = (YieldTypes)iYield;
		if (GetYieldFromTilePurchase(eYield) > 0 ||
			GetYieldFromTileEarn(eYield) > 0 ||
			GetYieldFromSettle(eYield) > 0 ||
			GetYieldFromLuxuryResourceGain(eYield) > 0 ||
			GetGoldenAgeYieldModifier(eYield) > 0)
		{
			m_bIsExpansionist = true;
			return;
		}
		
		for (int iImprovementLoop = 0; iImprovementLoop < GC.getNumImprovementInfos(); iImprovementLoop++)
		{
			ImprovementTypes eImprovement = (ImprovementTypes)iImprovementLoop;
			if (GetYieldChangePerImprovementBuilt(eImprovement, eYield) > 0)
			{
				m_bIsExpansionist = true;
				return;
			}
		}

		for (int iTerrain = 0; iTerrain < GC.getNumTerrainInfos(); iTerrain++)
		{
			TerrainTypes eTerrain = (TerrainTypes)iTerrain;
			if (GetYieldChangeFromTileEarnTerrainType(eTerrain, eYield) > 0 ||
				GetYieldChangeFromTilePurchaseTerrainType(eTerrain, eYield) > 0 ||
				GetYieldChangeFromTileSettle(eTerrain, eYield) > 0)
			{
				m_bIsExpansionist = true;
				return;
			}
		}
	}
}

void CvPlayerTraits::SetIsReligious()
{
	if (GetYieldFromHistoricEvent(YIELD_FAITH) > 0 ||
		GetYieldFromLevelUp(YIELD_FAITH) > 0 ||
		GetYieldFromConquest(YIELD_FAITH) > 0 ||
		GetYieldFromCityDamageTimes100(YIELD_FAITH) > 0 ||
		GetYieldChangePerTradePartner(YIELD_FAITH) > 0 ||
		GetYieldFromCSAlly(YIELD_FAITH) > 0 ||
		GetYieldFromCSFriend(YIELD_FAITH) > 0 ||
		GetYieldChangePerTradePartner(YIELD_FAITH) > 0 ||
		GetYieldFromTilePurchase(YIELD_FAITH) > 0 ||
		GetYieldFromTileEarn(YIELD_FAITH) > 0 ||
		GetYieldFromSettle(YIELD_FAITH) > 0 ||
		GetYieldChangeWorldWonder(YIELD_FAITH) > 0 ||
		GetGoldenAgeFromGreatPersonBirth(static_cast<GreatPersonTypes>(GC.getInfoTypeForString("GREATPERSON_PROPHET"))) > 0 ||
		GetSharedReligionTourismModifier() > 0 ||
		GetExtraMissionaryStrength() > 0 ||
		GetGoldenAgeYieldModifier(YIELD_FAITH) > 0 ||
		GetYieldFromBarbarianCampClear(YIELD_FAITH, true) > 0 ||
		GetYieldFromBarbarianCampClear(YIELD_FAITH, false) > 0 ||
		GetYieldFromXMilitaryUnits(YIELD_FAITH) > 0 ||
		GetTradeReligionModifier() > 0 ||
		GetGPFaithPurchaseEra() != NO_ERA ||
		GetFaithCostModifier() > 0 ||
		GetFaithFromKills() > 0)
	{
		m_bIsReligious = true;
		return;
	}

	if (IsForeignReligionSpreadImmune() ||
		IsNoNaturalReligionSpread() ||
		IsFaithFromUnimprovedForest() ||
		IsAlwaysReligion() ||
		IsAnyBelief() ||
		IsBonusReligiousBelief() ||
		IsPopulationBoostReligion() ||
		StartsWithPantheon() ||
		IsProphetFervor())
	{
		m_bIsReligious = true;
		return;
	}

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		YieldTypes eYield = (YieldTypes)iYield;
		if (GetYieldFromOwnPantheon(eYield) > 0)
		{
			m_bIsReligious = true;
			return;
		}
	}

	if (m_pPlayer->GetReligions()->GetStateReligion() != NO_RELIGION)
	{
		m_bIsReligious = true;
		return;
	}
}

/// Store off data on bonuses from traits
void CvPlayerTraits::InitPlayerTraits()
{
	// Reset all values to prevent accumulation on multiple calls
	Reset();

	// precompute the traits our leader has
	m_vPotentiallyActiveLeaderTraits.clear();
	for(int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		m_vLeaderHasTrait[iI] = false;
		if (m_pPlayer && m_pPlayer->isMajorCiv() && m_pPlayer->isAlive() && m_pPlayer->getLeaderInfo().hasTrait( (TraitTypes)iI ))
		{
			m_vLeaderHasTrait[iI] = true;
			m_vPotentiallyActiveLeaderTraits.push_back( (TraitTypes)iI );
		}
	}

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
			m_iNationalPopReqModifier += trait->GetNationalPopReqModifier();
			m_iCityWorkingChange += trait->GetCityWorkingChange();
			m_iCityAutomatonWorkersChange += trait->GetCityAutomatonWorkersChange();
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
			m_iExtraEmbarkMoves += trait->GetExtraEmbarkMoves();
			m_iNaturalWonderFirstFinderGold += trait->GetNaturalWonderFirstFinderGold();
			m_iNaturalWonderSubsequentFinderGold += trait->GetNaturalWonderSubsequentFinderGold();
			m_iNaturalWonderYieldModifier += trait->GetNaturalWonderYieldModifier();
			m_iNaturalWonderHappinessModifier += trait->GetNaturalWonderHappinessModifier();
			m_iNearbyImprovementCombatBonus += trait->GetNearbyImprovementCombatBonus();
			m_iNearbyImprovementBonusRange += trait->GetNearbyImprovementBonusRange();
			m_iCultureBuildingYieldChange += trait->GetCultureBuildingYieldChange();
			m_iEnemyWarSawPactPromotion = trait->GetEnemyWarSawPactPromotion();
			m_iCombatBonusVsHigherPop += trait->GetCombatBonusVsHigherPop();
			m_iWarWearinessModifier += trait->GetWarWearinessModifier();
			m_iEnemyWarWearinessModifier += trait->GetEnemyWarWearinessModifier();
			if(trait->IsBuyOwnedTiles())
			{
				m_bBuyOwnedTiles = true;
			}
			if(trait->IsNewCitiesStartWithCapitalReligion())
			{
				m_bNewCitiesStartWithCapitalReligion = true;
			}
			if(trait->IsForeignReligionSpreadImmune())
			{
				m_bNoSpread = true;
			}
			if(trait->GetXPBonusFromGreatPersonBirth() != 0)
			{
				m_iXPBonusFromGreatPersonBirth += trait->GetXPBonusFromGreatPersonBirth();
			}
			if(trait->GetUnitHealFromGreatPersonBirth() != 0)
			{
				m_iUnitHealFromGreatPersonBirth += trait->GetUnitHealFromGreatPersonBirth();
			}
			if (trait->GetBullyMilitaryStrengthModifier() != 0)
			{
				m_iBullyMilitaryStrengthModifier += trait->GetBullyMilitaryStrengthModifier();
			}
			if (trait->GetBullyValueModifier() != 0)
			{
				m_iBullyValueModifier += trait->GetBullyValueModifier();
			}
			if (trait->IgnoreBullyPenalties())
			{
				m_bIgnoreBullyPenalties = true;
			}
			if(trait->IsDiplomaticMarriage())
			{
				m_bDiplomaticMarriage = true;
			}
			if(trait->IsAdoptionFreeTech())
			{
				m_bAdoptionFreeTech = true;
			}
			if(trait->IsGPWLTKD())
			{
				m_bGPWLTKD = true;
			}
			if (trait->IsGreatWorkWLTKD())
			{
				m_bGreatWorkWLTKD = true;
			}
			if (trait->IsExpansionWLTKD())
			{
				m_bExpansionWLTKD = true;
			}
			if(trait->IsTradeRouteOnly())
			{
				m_bTradeRouteOnly = true;
			}
			if(trait->IsKeepConqueredBuildings())
			{
				m_bKeepConqueredBuildings = true;
			}
			if(trait->IsMountainPass())
			{
				m_bMountainPass = true;
			}
			if(trait->IsUniqueBeliefsOnly())
			{
				m_bUniqueBeliefsOnly = true;
			}
			if(trait->IsNoNaturalReligionSpread())
			{
				m_bNoNaturalReligionSpread = true;
			}
			if (trait->IsNoOpenTrade())
			{
				m_bNoOpenTrade  = true;
			}
			if (trait->IsGoldenAgeOnWar())
			{
				m_bGoldenAgeOnWar = true;
			}
			if (trait->IsUnableToCancelRazing()) 
			{
				m_bUnableToCancelRazing = true;
			}
			if (trait->IsBestUnitSpawnOnImprovementDOW())
			{
				m_bBestUnitSpawnOnImpDOW = true;
			}
			if (trait->IsTradeRouteMinorInfluenceAdmiralPoints())
			{
				m_bTradeRouteMinorInfluenceAP = true;
			}
			if (trait->IsProductionModFromNumSpecialists())
			{
				m_bProdModFromNumSpecialists = true;
			}
			if (trait->IsConquestOfTheWorld())
			{
				m_bConquestOfTheWorld = true;
			}
			if (trait->IsFreeUpgrade())
			{
				m_bFreeUpgrade = true;
			}
			if (trait->IsWarsawPact())
			{
				m_bWarsawPact = true;
			}
			if (trait->IsFreeZuluPikemanToImpi())
			{
				m_bFreeZuluPikemanToImpi = true;
			}
			if (trait->IsPermanentYieldsDecreaseEveryEra())
			{
				m_bPermanentYieldsDecreaseEveryEra = true;
			}
			if (trait->IsImportsCountTowardsMonopolies())
			{
				m_bImportsCountTowardsMonopolies = true;
			}
			if (trait->IsCanPurchaseNavalUnitsFaith())
			{
				m_bCanPurchaseNavalUnitsFaith = true;
			}

			m_iPuppetPenaltyReduction += trait->GetPuppetPenaltyReduction();
			m_iTourismToGAP += trait->GetTourismToGAP();
			m_iGoldToGAP += trait->GetGoldToGAP();
			m_iInfluenceMeetCS += trait->GetInfluenceMeetCS();
			m_iMultipleAttackBonus += trait->GetMultipleAttackBonus();
			m_iCityConquestGWAM += trait->GetCityConquestGWAM();
			m_iEventTourismBoost += trait->GetEventTourismBoost();
			m_iGrowthBoon += trait->GetGrowthBoon();		
			m_iWLTKDGPImprovementModifier += trait->GetWLTKDGPImprovementModifier();
			m_iAllianceCSDefense += trait->GetAllianceCSDefense();
			m_iAllianceCSStrength += trait->GetAllianceCSStrength();
			m_iTourismGABonus += trait->GetTourismGABonus();
			m_iEventGP += trait->GetEventGP();
			m_iWLTKDCulture += trait->GetWLTKDCulture();
			m_iWLTKDGATimer += trait->GetWLTKDGATimer();
			m_iWLTKDUnhappinessNeedsMod += trait->GetWLTKDUnhappinessNeedsMod();
			m_iStartingSpies += trait->GetStartingSpies();
			m_iSpyOffensiveStrengthModifier += trait->GetSpyOffensiveStrengthModifier();
			m_iSpyMoveRateBonus += trait->GetSpyMoveRateBonus();
			m_iSpySecurityModifier += trait->GetSpySecurityModifier();
			m_iSpyExtraRankBonus += trait->GetSpyExtraRankBonus();
			m_iQuestYieldModifier += trait->GetQuestYieldModifier();
			m_iWonderProductionModifierToBuilding += trait->GetWonderProductionModifierToBuilding();
			m_iPolicyGEorGM += trait->GetPolicyGEorGM();
			m_iGAGarrisonCityRangeStrikeModifier += trait->GetGoldenAgeGarrisonedCityRangeStrikeModifier();
			m_iBestUnitImprovement = trait->GetBestSpawnUnitImprovement();
			m_iGGGARateFromDenunciationsAndWars += trait->GetGGGARateFromDenunciationsAndWars();
			m_iConquestOfTheWorldCityAttack += trait->GetConquestOfTheWorldCityAttack();
			m_iSharedReligionTourismModifier += trait->GetSharedReligionTourismModifier();
			m_iExtraMissionaryStrength += trait->GetExtraMissionaryStrength();
			if (trait->IsCanGoldInternalTradeRoutes())
			{
				m_bCanGoldInternalTradeRoutes = true;
			}
			if (trait->IsAnnexedCityStatesGiveYields())
			{
				m_bAnnexedCityStatesGiveYields = true;
			}
			m_iExtraTradeRoutesPerXOwnedCities += trait->GetExtraTradeRoutesPerXOwnedCities();
			m_iExtraTradeRoutesPerXOwnedVassals += trait->GetExtraTradeRoutesPerXOwnedVassals();
			m_iMinorInfluencePerGiftedUnit += trait->GetMinorInfluencePerGiftedUnit();
			if (trait->IsCapitalOnly())
			{
				m_bIsCapitalOnly = true;
			}

			m_iInvestmentModifier += trait->GetInvestmentModifier();
			m_iCombatBonusVsHigherTech += trait->GetCombatBonusVsHigherTech();
			m_iCombatBonusVsLargerCiv += trait->GetCombatBonusVsLargerCiv();
			m_iLandUnitMaintenanceModifier += trait->GetLandUnitMaintenanceModifier();
			m_iNavalUnitMaintenanceModifier += trait->GetNavalUnitMaintenanceModifier();
			m_iRazeSpeedModifier += trait->GetRazeSpeedModifier();
			m_iDOFGreatPersonModifier += trait->GetDOFGreatPersonModifier();
			m_iLuxuryHappinessRetention += trait->GetLuxuryHappinessRetention();
			m_iExtraSupply += trait->GetExtraSupply();
			m_iExtraSupplyPerCity += trait->GetExtraSupplyPerCity();
			m_iExtraSupplyPerPopulation += trait->GetExtraSupplyPerPopulation();
			m_iExtraSpies += trait->GetExtraSpies();
			m_iUnresearchedTechBonusFromKills += trait->GetUnresearchedTechBonusFromKills();
			m_iExtraFoundedCityTerritoryClaimRange += trait->GetExtraFoundedCityTerritoryClaimRange();
			m_iExtraConqueredCityTerritoryClaimRange += trait->GetExtraConqueredCityTerritoryClaimRange();
			m_iExtraTenetsFirstAdoption += trait->GetExtraTenetsFirstAdoption();
			m_iMonopolyModFlat += trait->GetMonopolyModFlat();
			m_iMonopolyModPercent += trait->GetMonopolyModPercent();
			m_iFreeSocialPoliciesPerEra += trait->GetFreeSocialPoliciesPerEra();
			m_iNumTradeRoutesModifier += trait->GetNumTradeRoutesModifier();
			m_iTradeRouteResourceModifier += trait->GetTradeRouteResourceModifier();
			m_iUniqueLuxuryCities += trait->GetUniqueLuxuryCities();
			m_iUniqueLuxuryQuantity	+= trait->GetUniqueLuxuryQuantity();
			m_iWorkerSpeedModifier += trait->GetWorkerSpeedModifier();
			m_iAfraidMinorPerTurnInfluence += trait->GetAfraidMinorPerTurnInfluence();
			m_iLandTradeRouteRangeBonus += trait->GetLandTradeRouteRangeBonus();
			m_iSeaTradeRouteRangeBonus += trait->GetSeaTradeRouteRangeBonus();
			m_iTradeReligionModifier += trait->GetTradeReligionModifier();
			m_iTradeBuildingModifier += trait->GetTradeBuildingModifier();
			m_iNumFreeBuildings	+= trait->GetNumFreeBuildings();
			m_iNonSpecialistFoodChange += trait->GetNonSpecialistFoodChange();

			if (trait->GetTradeRouteProductionSiphon(true).IsHaveProductionSiphon())
			{
				m_aiiTradeRouteProductionSiphon[true].m_iSiphonPercent += trait->GetTradeRouteProductionSiphon(true).m_iSiphonPercent;
				m_aiiTradeRouteProductionSiphon[true].m_iPercentIncreaseWithOpenBorders += trait->GetTradeRouteProductionSiphon(true).m_iPercentIncreaseWithOpenBorders;
			}
			if (trait->GetTradeRouteProductionSiphon(false).IsHaveProductionSiphon())
			{
				m_aiiTradeRouteProductionSiphon[false].m_iSiphonPercent += trait->GetTradeRouteProductionSiphon(false).m_iSiphonPercent;
				m_aiiTradeRouteProductionSiphon[false].m_iPercentIncreaseWithOpenBorders += trait->GetTradeRouteProductionSiphon(false).m_iPercentIncreaseWithOpenBorders;
			}

			if(trait->IsBullyAnnex())
			{
				m_bBullyAnnex = true;
			}
			m_iBullyYieldMultiplierAnnex += trait->GetBullyYieldMultiplierAnnex();
			if(trait->IsFightWellDamaged())
			{
				m_bFightWellDamaged = true;
			}
			if(trait->IsWoodlandMovementBonus())
			{
				m_bWoodlandMovementBonus = true;
			}
			if(trait->IsRiverMovementBonus())
			{
				m_bRiverMovementBonus = true;
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
			if(trait->IsArtistGoldenAgeTechBoost())
			{
				m_bArtistGoldenAgeTechBoost = true;
			}
			if(trait->IsStaysAliveZeroCities())
			{
				m_bStaysAliveZeroCities = true;
			}
			if(trait->IsFaithFromUnimprovedForest())
			{
				m_bFaithFromUnimprovedForest = true;
			}
			if(trait->IsAnyBelief())
			{
				m_bAnyBelief = true;
			}
			if(trait->IsAlwaysReligion())
			{
				m_bAlwaysReligion = true;
			}
			if (trait->IsNoTradeRouteProximityPenalty())
			{
				m_bNoTradeRouteProximityPenalty = true;
			}
			if (trait->IsCanPlunderWithoutWar())
			{
				m_bCanPlunderWithoutWar = true;
			}
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
			if(trait->IsCrossesIce())
			{
				m_bCrossesIce = true;
			}
			if(trait->IsGGFromBarbarians())
			{
				m_bGGFromBarbarians = true;
			}
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

			m_iBasicNeedsMedianModifier += trait->GetBasicNeedsMedianModifier();
			m_iGoldMedianModifier += trait->GetGoldMedianModifier();
			m_iScienceMedianModifier += trait->GetScienceMedianModifier();
			m_iCultureMedianModifier += trait->GetCultureMedianModifier();
			m_iReligiousUnrestModifier += trait->GetReligiousUnrestModifier();

			if( trait->IsNoConnectionUnhappiness())
			{
				m_bNoConnectionUnhappiness = true;
			}
			if( trait->IsNoReligiousStrife())
			{
				m_bIsNoReligiousStrife = true;
			}
			if( trait->IsOddEraScaler())
			{
				m_bIsOddEraScaler= true;
			}
			m_iWonderProductionModGA += trait->GetWonderProductionModGA();
			m_iCultureBonusModifierConquest += trait->GetCultureBonusModifierConquest();
			m_iProductionBonusModifierConquest += trait->GetProductionBonusModifierConquest();

			m_miResourceYieldChangesFromGoldenAge = trait->GetResourceYieldChangesFromGoldenAge();
			m_miResourceYieldChangesFromGoldenAgeCap = trait->GetResourceYieldChangesFromGoldenAgeCap();

			m_iPurchasedUnitsBonusXP = trait->GetPurchasedUnitsBonusXP();
			m_iVotePerXCSAlliance = trait->GetVotePerXCSAlliance();
			m_iVotePerXCSFollowingFollowingYourReligion = trait->GetVotePerXCSFollowingYourReligion();
			m_iChanceToConvertReligiousUnits = trait->GetChanceToConvertReligiousUnits();
			m_iGoldenAgeFromVictory = trait->GetGoldenAgeFromVictory();
			m_iFreePolicyPerXTechs = trait->GetFreePolicyPerXTechs();
			m_eGPFaithPurchaseEra = trait->GetGPFaithPurchaseEra();
			m_iFaithCostModifier = trait->GetFaithCostModifier();
			if(trait->IsFreeGreatWorkOnConquest())
			{
				m_bFreeGreatWorkOnConquest = true;
			}
			if(trait->IsPopulationBoostReligion())
			{
				m_bPopulationBoostReligion = true;
			}
			if(trait->StartsWithPantheon())
			{
				m_bStartsWithPantheon = true;
			}
			if(trait->IsProphetFervor())
			{
				m_bProphetFervor = true;
			}
			if(trait->IsCombatBoostNearNaturalWonder())
			{
				m_bCombatBoostNearNaturalWonder= true;
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

					iChange = trait->GetCityYieldFromUnimprovedFeature((FeatureTypes)iFeatureLoop, (YieldTypes)iYield);
					if(iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppiCityYieldFromUnimprovedFeature[iFeatureLoop];
						yields[iYield] = (m_ppiCityYieldFromUnimprovedFeature[iFeatureLoop][iYield] + iChange);
						m_ppiCityYieldFromUnimprovedFeature[iFeatureLoop] = yields;
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
					iChange = trait->GetYieldChangePerImprovementBuilt((ImprovementTypes)iImprovementLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiYieldChangePerImprovementBuilt[iImprovementLoop];
						yields[iYield] = (m_ppaaiYieldChangePerImprovementBuilt[iImprovementLoop][iYield] + iChange);
						m_ppaaiYieldChangePerImprovementBuilt[iImprovementLoop] = yields;
						UpdateYieldChangeImprovementTypes();
					}
				}
				for(int iTerrainLoop = 0; iTerrainLoop < GC.getNumTerrainInfos(); iTerrainLoop++)
				{
					int iChange = trait->GetYieldFromTileEarnTerrainType((TerrainTypes)iTerrainLoop, (YieldTypes)iYield);
					if(iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppiYieldFromTileEarnTerrainType[iTerrainLoop];
						yields[iYield] = (m_ppiYieldFromTileEarnTerrainType[iTerrainLoop][iYield] + iChange);
						m_ppiYieldFromTileEarnTerrainType[iTerrainLoop] = yields;
						m_bHasYieldFromTileEarn = true;
					}
					iChange = trait->GetYieldFromTilePurchaseTerrainType((TerrainTypes)iTerrainLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppiYieldFromTilePurchaseTerrainType[iTerrainLoop];
						yields[iYield] = (m_ppiYieldFromTilePurchaseTerrainType[iTerrainLoop][iYield] + iChange);
						m_ppiYieldFromTilePurchaseTerrainType[iTerrainLoop] = yields;
						m_bHasYieldFromTilePurchase = true;
					}
					iChange = trait->GetYieldFromTileConquest((TerrainTypes)iTerrainLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppiYieldFromTileConquest[iTerrainLoop];
						yields[iYield] = (m_ppiYieldFromTileConquest[iTerrainLoop][iYield] + iChange);
						m_ppiYieldFromTileConquest[iTerrainLoop] = yields;
					}
					iChange = trait->GetYieldFromTileCultureBomb((TerrainTypes)iTerrainLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppiYieldFromTileCultureBomb[iTerrainLoop];
						yields[iYield] = (m_ppiYieldFromTileCultureBomb[iTerrainLoop][iYield] + iChange);
						m_ppiYieldFromTileCultureBomb[iTerrainLoop] = yields;
						m_bHasYieldFromTileCultureBomb = true;
					}
					iChange = trait->GetYieldFromTileStealCultureBomb((TerrainTypes)iTerrainLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppiYieldFromTileStealCultureBomb[iTerrainLoop];
						yields[iYield] = (m_ppiYieldFromTileStealCultureBomb[iTerrainLoop][iYield] + iChange);
						m_ppiYieldFromTileStealCultureBomb[iTerrainLoop] = yields;
					}
					iChange = trait->GetYieldFromTileSettle((TerrainTypes)iTerrainLoop, (YieldTypes)iYield);
					if (iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppiYieldFromTileSettle[iTerrainLoop];
						yields[iYield] = (m_ppiYieldFromTileSettle[iTerrainLoop][iYield] + iChange);
						m_ppiYieldFromTileSettle[iTerrainLoop] = yields;
					}
				}
				for(int iPlotLoop = 0; iPlotLoop < GC.getNumPlotInfos(); iPlotLoop++)
				{
					int iChange = trait->GetPlotYieldChanges((PlotTypes)iPlotLoop, (YieldTypes)iYield);
					if(iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppiPlotYieldChange[iPlotLoop];
						yields[iYield] = (m_ppiPlotYieldChange[iPlotLoop][iYield] + iChange);
						m_ppiPlotYieldChange[iPlotLoop] = yields;
					}
				}
				m_iYieldFromLevelUp[iYield] = trait->GetYieldFromLevelUp(iYield);
				m_iYieldFromHistoricEvent[iYield] = trait->GetYieldFromHistoricEvent(iYield);
				m_iYieldFromXMilitaryUnits[iYield] = trait->GetYieldFromXMilitaryUnits(iYield);
				m_iYieldFromOwnPantheon[iYield] = trait->GetYieldFromOwnPantheon(iYield);
				// TradeRouteEndYield
				{
					const std::pair<int, int> tradeEndChange = trait->GetTradeRouteEndYield(static_cast<YieldTypes>(iYield));
					const int tradeEndDomesticChange = tradeEndChange.first;
					const int tradeEndInternationalChange = tradeEndChange.second;
					if (tradeEndDomesticChange != 0)
					{
						std::map<int, int>::iterator tradeEndDomesticIt = m_tradeRouteEndYieldDomestic.find(iYield);
						if (tradeEndDomesticIt != m_tradeRouteEndYieldDomestic.end())
						{
							tradeEndDomesticIt->second += tradeEndDomesticChange;
							if (tradeEndDomesticIt->second == 0)
							{
								m_tradeRouteEndYieldDomestic.erase(tradeEndDomesticIt);
							}
						}
						else
						{
							m_tradeRouteEndYieldDomestic.insert(tradeEndDomesticIt, std::make_pair(iYield, tradeEndDomesticChange));
						}
					}
					if (tradeEndInternationalChange != 0)
					{
						std::map<int, int>::iterator tradeEndInternationalIt = m_tradeRouteEndYieldInternational.find(iYield);
						if (tradeEndInternationalIt != m_tradeRouteEndYieldInternational.end())
						{
							tradeEndInternationalIt->second += tradeEndInternationalChange;
							if (tradeEndInternationalIt->second == 0)
							{
								m_tradeRouteEndYieldInternational.erase(tradeEndInternationalIt);
							}
						}
						else
						{
							m_tradeRouteEndYieldInternational.insert(tradeEndInternationalIt, std::make_pair(iYield, tradeEndInternationalChange));
						}
					}
				}
				m_iYieldFromRouteMovement[iYield] = trait->GetYieldFromRouteMovement(iYield);
				m_iYieldFromExport[iYield] = trait->GetYieldFromExport(iYield);
				m_iYieldFromImport[iYield] = trait->GetYieldFromImport(iYield);
				m_iYieldFromTilePurchase[iYield] = trait->GetYieldFromTilePurchase(iYield);
				if (m_iYieldFromTilePurchase[iYield] > 0)
				{
					m_bHasYieldFromTilePurchase = true;
				}
				m_iYieldFromTileEarn[iYield] = trait->GetYieldFromTileEarn(iYield);
				if (m_iYieldFromTileEarn[iYield] > 0)
				{
					m_bHasYieldFromTileEarn = true;
				}
				m_iYieldFromCSAlly[iYield] = trait->GetYieldFromCSAlly(iYield);
				m_iYieldFromCSFriend[iYield] = trait->GetYieldFromCSFriend(iYield);
				m_iYieldFromSettle[iYield] = trait->GetYieldFromSettle(iYield);
				m_iYieldFromConquest[iYield] = trait->GetYieldFromConquest(iYield);
				m_iYieldFromCityDamageTimes100[iYield] = trait->GetYieldFromCityDamageTimes100(iYield);
				m_pbiYieldFromBarbarianCampClear[iYield][true] = trait->GetYieldFromBarbarianCampClear((YieldTypes)iYield, true);
				m_pbiYieldFromBarbarianCampClear[iYield][false] = trait->GetYieldFromBarbarianCampClear((YieldTypes)iYield, false);
				if (trait->GetGoldenAgeYieldModifier(iYield) != 0)
				{
					m_aiGoldenAgeYieldModifier.insert(std::pair<int, int>(iYield, trait->GetGoldenAgeYieldModifier(iYield)));
				}

				m_pbiYieldFromRouteMovementInForeignTerritory[iYield][true] = trait->GetYieldFromRouteMovementInForeignTerritory((YieldTypes)iYield, true);
				m_pbiYieldFromRouteMovementInForeignTerritory[iYield][false] = trait->GetYieldFromRouteMovementInForeignTerritory((YieldTypes)iYield, false);

				for(int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
				{
					int iChange = trait->GetBuildingClassYieldChanges((BuildingClassTypes)iBuildingClassLoop, (YieldTypes)iYield);
					if(iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppiBuildingClassYieldChange[iBuildingClassLoop];
						yields[iYield] = (m_ppiBuildingClassYieldChange[iBuildingClassLoop][iYield] + iChange);
						m_ppiBuildingClassYieldChange[iBuildingClassLoop] = yields;
					}
				}

				m_iCapitalYieldChanges[iYield] = trait->GetCapitalYieldChanges(iYield);
				m_iCityYieldChanges[iYield] = trait->GetCityYieldChanges(iYield);
				m_iPermanentYieldChangeWLTKD[iYield] = trait->GetPermanentYieldChangeWLTKD(iYield);
				m_iCoastalCityYieldChanges[iYield] = trait->GetCoastalCityYieldChanges(iYield);
				m_iGreatWorkYieldChanges[iYield] = trait->GetGreatWorkYieldChanges(iYield);
				m_iArtifactYieldChanges[iYield] = trait->GetArtifactYieldChanges(iYield);
				m_iArtYieldChanges[iYield] = trait->GetArtYieldChanges(iYield);
				m_iLitYieldChanges[iYield] = trait->GetLitYieldChanges(iYield);
				m_iMusicYieldChanges[iYield] = trait->GetMusicYieldChanges(iYield);
				m_iSeaPlotYieldChanges[iYield] = trait->GetSeaPlotYieldChanges(iYield);
				m_iGAPToYield[iYield] = trait->GetGAPToYield(iYield);
				m_iMountainRangeYield[iYield] = trait->GetMountainRangeYield(iYield);
				m_iLuxuryYieldChanges[iYield] = trait->GetLuxuryYieldChanges(iYield);

				for(int iFeatureLoop = 0; iFeatureLoop < GC.getNumFeatureInfos(); iFeatureLoop++)
				{
					int iChange = trait->GetFeatureYieldChanges((FeatureTypes)iFeatureLoop, (YieldTypes)iYield);
					if(iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppiFeatureYieldChange[iFeatureLoop];
						yields[iYield] = (m_ppiFeatureYieldChange[iFeatureLoop][iYield] + iChange);
						m_ppiFeatureYieldChange[iFeatureLoop] = yields;
					}
				}

				for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
				{
					int iChange = trait->GetResourceYieldChanges((ResourceTypes)iResourceLoop, (YieldTypes)iYield);
					if(iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppiResourceYieldChange[iResourceLoop];
						yields[iYield] = (m_ppiResourceYieldChange[iResourceLoop][iYield] + iChange);
						m_ppiResourceYieldChange[iResourceLoop] = yields;
					}
				}

				for(int iTerrainLoop = 0; iTerrainLoop < GC.getNumTerrainInfos(); iTerrainLoop++)
				{
					int iChange = trait->GetTerrainYieldChanges((TerrainTypes)iTerrainLoop, (YieldTypes)iYield);
					if(iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppiTerrainYieldChange[iTerrainLoop];
						yields[iYield] = (m_ppiTerrainYieldChange[iTerrainLoop][iYield] + iChange);
						m_ppiTerrainYieldChange[iTerrainLoop] = yields;
					}
				}

				m_iYieldFromKills[iYield] = trait->GetYieldFromKills((YieldTypes) iYield);
				m_iYieldFromBarbarianKills[iYield] = trait->GetYieldFromBarbarianKills((YieldTypes) iYield);
				m_iYieldFromMinorDemand[iYield] = trait->GetYieldFromMinorDemand((YieldTypes)iYield);
				m_iYieldFromLuxuryResourceGain[iYield] = trait->GetYieldFromLuxuryResourceGain((YieldTypes)iYield);
				m_iYieldChangeTradeRoute[iYield] = trait->GetYieldChangeTradeRoute(iYield);
				m_iYieldChangeWorldWonder[iYield] = trait->GetYieldChangeWorldWonder(iYield);

				for(int iDomainLoop = 0; iDomainLoop < NUM_DOMAIN_TYPES; iDomainLoop++)
				{
					int iChange = trait->GetTradeRouteYieldChange((DomainTypes)iDomainLoop, (YieldTypes)iYield);
					if(iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppiTradeRouteYieldChange[iDomainLoop];
						yields[iYield] = (m_ppiTradeRouteYieldChange[iDomainLoop][iYield] + iChange);
						m_ppiTradeRouteYieldChange[iDomainLoop] = yields;
					}
				}

				for(int iGreatPersonLoop = 0; iGreatPersonLoop < GC.getNumGreatPersonInfos(); iGreatPersonLoop++)
				{
					int iChange = trait->GetGreatPersonExpendedYield((GreatPersonTypes)iGreatPersonLoop, (YieldTypes)iYield);
					if(iChange > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppiGreatPersonExpendedYield[iGreatPersonLoop];
						yields[iYield] = (m_ppiGreatPersonExpendedYield[iGreatPersonLoop][iYield] + iChange);
						m_ppiGreatPersonExpendedYield[iGreatPersonLoop] = yields;
					}
					int iChange2 = trait->GetGreatPersonBornYield((GreatPersonTypes)iGreatPersonLoop, (YieldTypes)iYield);
					if(iChange2 > 0)
					{
						Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppiGreatPersonBornYield[iGreatPersonLoop];
						yields[iYield] = (m_ppiGreatPersonBornYield[iGreatPersonLoop][iYield] + iChange2);
						m_ppiGreatPersonBornYield[iGreatPersonLoop] = yields;
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

			ASSERT(GC.getNumTerrainInfos() <= NUM_TERRAIN_TYPES);
			for(int iTerrain = 0; iTerrain < GC.getNumTerrainInfos(); iTerrain++)
			{
				m_iStrategicResourceQuantityModifier[iTerrain] = trait->GetStrategicResourceQuantityModifier(iTerrain);
				m_abTerrainClaimBoost[iTerrain] = trait->TerrainClaimBoost((TerrainTypes)iTerrain);			
			}

			for(int iResource = 0; iResource < GC.getNumResourceInfos(); iResource++)
			{
				m_aiResourceQuantityModifier[iResource] = trait->GetResourceQuantityModifier(iResource);
				m_aiNumFreeResourceOnWorldWonderCompletion[iResource] = trait->GetNumFreeResourceOnWorldWonderCompletion(iResource);
				if (trait->GetAlternateResourceTechs((ResourceTypes)iResource).IsAlternateResourceTechs())
				{
					m_aiiAlternateResourceTechs[iResource] = trait->GetAlternateResourceTechs((ResourceTypes)iResource);
				}
			}

			for (int iGreatPersonTypes = 0; iGreatPersonTypes < GC.getNumGreatPersonInfos(); iGreatPersonTypes++)
			{
				m_aiGreatPersonCostReduction[iGreatPersonTypes] = trait->GetGreatPersonCostReduction((GreatPersonTypes)iGreatPersonTypes);
				m_aiPerPuppetGreatPersonRateModifier[iGreatPersonTypes] = trait->GetPerPuppetGreatPersonRateModifier((GreatPersonTypes)iGreatPersonTypes);
				m_aiGreatPersonGWAM[iGreatPersonTypes] = trait->GetGreatPersonGWAM((GreatPersonTypes)iGreatPersonTypes);
				m_aiGoldenAgeGreatPersonRateModifier[iGreatPersonTypes] = trait->GetGoldenAgeGreatPersonRateModifier((GreatPersonTypes)iGreatPersonTypes);
				m_aiGoldenAgeFromGreatPersonBirth[iGreatPersonTypes] = trait->GetGoldenAgeFromGreatPersonBirth((GreatPersonTypes)iGreatPersonTypes);
				m_aiGreatPersonProgressFromPolicyUnlock[iGreatPersonTypes] = trait->GetGreatPersonProgressFromPolicyUnlock((GreatPersonTypes)iGreatPersonTypes);
				if (trait->GetGreatPersonProgressFromKills((GreatPersonTypes)iGreatPersonTypes) > 0)
				{
					m_aiGreatPersonProgressFromKills[iGreatPersonTypes] = trait->GetGreatPersonProgressFromKills((GreatPersonTypes)iGreatPersonTypes);
				}
				if (trait->GetRandomGreatPersonProgressFromKills((GreatPersonTypes)iGreatPersonTypes) > 0)
				{
					m_aiRandomGreatPersonProgressFromKills[iGreatPersonTypes] = trait->GetRandomGreatPersonProgressFromKills((GreatPersonTypes)iGreatPersonTypes);
				}
			}

			for (int iDomain = 0; iDomain < NUM_DOMAIN_TYPES; iDomain++)
			{
				m_aiNumPledgesDomainProdMod[iDomain] = trait->GetNumPledgeDomainProductionModifier((DomainTypes)iDomain);
				m_aiDomainFreeExperienceModifier[iDomain] = trait->GetDomainFreeExperienceModifier((DomainTypes)iDomain);
				if (trait->GetDomainProductionModifiersPerSpecialist((DomainTypes)iDomain) > 0)
				{
					m_aiDomainProductionModifiersPerSpecialist.insert(std::make_pair(iDomain, trait->GetDomainProductionModifiersPerSpecialist((DomainTypes)iDomain)));
				}
			}

			for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); iUnitClass++)
			{
				m_abNoTrain[iUnitClass] = trait->NoTrain((UnitClassTypes)iUnitClass);
				m_aiFreeUnitClassesDOW[iUnitClass] = trait->GetFreeUnitClassesDOW((UnitClassTypes)iUnitClass);
			}
			FreeTraitUnit traitUnit;
			traitUnit.m_iFreeUnit = (UnitTypes)trait->GetFreeUnitClassType();
			if(traitUnit.m_iFreeUnit != NO_UNIT)
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
				if (trait->GetUnitCombatProductionCostModifier(jJ).first != 0)
				{
					m_aibUnitCombatProductionCostModifier.insert(std::make_pair(jJ, trait->GetUnitCombatProductionCostModifier(jJ)));
				}
			}
			int iNumUnitClasses = GC.getNumUnitClassInfos();
			for(int jJ= 0; jJ < iNumUnitClasses; jJ++)
			{
				m_paiMovesChangeUnitClass[jJ] += trait->GetMovesChangeUnitClass(jJ);
			}

			for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				FreeResourceXCities temp = trait->GetFreeResourceXCities((ResourceTypes)iResourceLoop);
				if(temp.m_iResourceQuantity > 0)
				{
					m_aFreeResourceXCities[iResourceLoop] = temp;
				}
			}

			for (int iBuildLoop = 0; iBuildLoop < GC.getNumBuildInfos(); iBuildLoop++)
			{
				BuildTypes eBuild = (BuildTypes)iBuildLoop;

				if (trait->IsNoBuild(eBuild))
				{
					std::vector<int>::const_iterator it = find(m_aiNoBuilds.begin(), m_aiNoBuilds.end(), iBuildLoop); // try to find if the entry already exists

					if (it == m_aiNoBuilds.end())
					{
						m_aiNoBuilds.push_back(iBuildLoop); // only add into container if it does not already exist
					}
				}
			}

			// Free promotions
			set<int> siFreePromotions = trait->GetFreePromotions();
			for (set<int>::iterator it = siFreePromotions.begin(); it != siFreePromotions.end(); ++it)
			{
				PromotionTypes ePromotion = static_cast<PromotionTypes>(*it);
				m_seFreePromotions.insert(ePromotion);
			}
		}
	}

	SetIsWarmonger();
	SetIsNerd();
	SetIsTourism();
	SetIsDiplomat();
	SetIsExpansionist();
	SetIsSmaller();
	SetIsReligious();
}

/// Deallocate memory created in initialize
void CvPlayerTraits::Uninit()
{
	m_aiResourceQuantityModifier.clear();
	m_aiNumFreeResourceOnWorldWonderCompletion.clear();
	m_abNoTrain.clear();
	m_paiMovesChangeUnitCombat.clear();
	m_abTerrainClaimBoost.clear();
	m_paiMovesChangeUnitClass.clear();
	m_ppiYieldFromTileEarnTerrainType.clear();
	m_ppiYieldFromTilePurchaseTerrainType.clear();
	m_ppiYieldFromTileConquest.clear();
	m_ppiYieldFromTileCultureBomb.clear();
	m_ppiYieldFromTileStealCultureBomb.clear();
	m_ppiYieldFromTileSettle.clear();
	m_ppaaiYieldChangePerImprovementBuilt.clear();
	UpdateYieldChangeImprovementTypes();
	m_pbiYieldFromBarbarianCampClear.clear();
	m_paiMaintenanceModifierUnitCombat.clear();
	m_ppaaiImprovementYieldChange.clear();
	m_ppiPlotYieldChange.clear();
	m_ppiBuildingClassYieldChange.clear();
	m_ppiFeatureYieldChange.clear();
	m_ppiResourceYieldChange.clear();
	m_ppiTerrainYieldChange.clear();
	m_ppiTradeRouteYieldChange.clear();

	m_aiGreatPersonCostReduction.clear();
	m_aiPerPuppetGreatPersonRateModifier.clear();
	m_aiGreatPersonGWAM.clear();
	m_aiGoldenAgeGreatPersonRateModifier.clear();
	m_aiGoldenAgeFromGreatPersonBirth.clear();
	m_aiGreatPersonProgressFromPolicyUnlock.clear();
	m_aiGreatPersonProgressFromKills.clear();
	m_aiRandomGreatPersonProgressFromKills.clear();
	m_aiNumPledgesDomainProdMod.clear();
	m_aiFreeUnitClassesDOW.clear();
	m_ppaaiSpecialistYieldChange.clear();
	m_aiDomainFreeExperienceModifier.clear();
	m_aiNoBuilds.clear();
	m_ppiGreatPersonExpendedYield.clear();
	m_ppiGreatPersonBornYield.clear();
	m_ppiCityYieldFromUnimprovedFeature.clear();
	m_ppaaiUnimprovedFeatureYieldChange.clear();
	m_aFreeResourceXCities.clear();
}

/// Reset data members
void CvPlayerTraits::Reset()
{
	Uninit();

	m_vLeaderHasTrait = std::vector<bool>( GC.getNumTraitInfos(), false );
	m_vPotentiallyActiveLeaderTraits.clear();

	m_bIsWarmonger = false;
	m_bIsNerd = false;
	m_bIsTourism = false;
	m_bIsDiplomat = false;
	m_bIsExpansionist = false;
	m_bIsSmaller = false;
	m_bIsReligious = false;

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
	m_iNationalPopReqModifier = 0;
	m_iCityWorkingChange = 0;
	m_iCityAutomatonWorkersChange = 0;
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
	m_iExtraEmbarkMoves = 0;
	m_iNaturalWonderFirstFinderGold = 0;
	m_iNaturalWonderSubsequentFinderGold = 0;
	m_iNaturalWonderYieldModifier = 0;
	m_iNaturalWonderHappinessModifier = 0;
	m_iNearbyImprovementCombatBonus = 0;
	m_iNearbyImprovementBonusRange = 0;
	m_iCultureBuildingYieldChange = 0;
	m_iWarWearinessModifier = 0;
	m_iEnemyWarWearinessModifier = 0;
	m_iCombatBonusVsHigherPop = 0;
	m_bBuyOwnedTiles = false;
	m_bNewCitiesStartWithCapitalReligion = false;
	m_bNoSpread = false;
	m_iXPBonusFromGreatPersonBirth = 0;
	m_iUnitHealFromGreatPersonBirth = 0;
	m_iBullyMilitaryStrengthModifier = 0;
	m_iBullyValueModifier = 0;
	m_bIgnoreBullyPenalties = false;
	m_bDiplomaticMarriage = false;
	m_bAdoptionFreeTech = false;
	m_iGrowthBoon = 0;
	m_iWLTKDGPImprovementModifier = 0;
	m_iAllianceCSDefense = 0;
	m_iAllianceCSStrength = 0;
	m_iTourismGABonus = 0;
	m_iEventGP = 0;
	m_iWLTKDCulture = 0;
	m_iWLTKDGATimer = 0;
	m_iWLTKDUnhappinessNeedsMod = 0;
	m_iStartingSpies = 0;
	m_iSpyOffensiveStrengthModifier = 0;
	m_iSpyMoveRateBonus = 0;
	m_iSpySecurityModifier = 0;
	m_iSpyExtraRankBonus = 0;
	m_iQuestYieldModifier = 0;
	m_bGPWLTKD = false;
	m_bGreatWorkWLTKD = false;
	m_bExpansionWLTKD = false;
	m_bTradeRouteOnly = false;
	m_bKeepConqueredBuildings = false;
	m_bMountainPass = false;
	m_bUniqueBeliefsOnly = false;
	m_bNoNaturalReligionSpread = false;
	m_bNoOpenTrade = false;
	m_bGoldenAgeOnWar = false;
	m_bUnableToCancelRazing = false;
	m_iTourismToGAP = 0;
	m_iGoldToGAP = 0;
	m_iInfluenceMeetCS = 0;
	m_iMultipleAttackBonus = 0;
	m_iCityConquestGWAM = 0;
	m_iEventTourismBoost = 0;
	m_iWonderProductionModifierToBuilding = 0;
	m_iPolicyGEorGM = 0;
	m_iGAGarrisonCityRangeStrikeModifier = 0;
	m_bBestUnitSpawnOnImpDOW = false;
	m_iBestUnitImprovement = NO_IMPROVEMENT;
	m_iGGGARateFromDenunciationsAndWars = 0;
	m_bTradeRouteMinorInfluenceAP = false;
	m_bProdModFromNumSpecialists = false;
	m_iConquestOfTheWorldCityAttack = 0;
	m_bConquestOfTheWorld = false;
	m_bFreeUpgrade = false;
	m_bWarsawPact = false;
	m_iEnemyWarSawPactPromotion = NO_PROMOTION;
	m_bFreeZuluPikemanToImpi = false;
	m_bPermanentYieldsDecreaseEveryEra = false;
	m_bImportsCountTowardsMonopolies = false;
	m_bCanPurchaseNavalUnitsFaith = false;
	m_iPuppetPenaltyReduction = 0;
	m_iSharedReligionTourismModifier = 0;
	m_iExtraMissionaryStrength = 0;
	m_bCanGoldInternalTradeRoutes = false;
	m_bAnnexedCityStatesGiveYields = false;
	m_iExtraTradeRoutesPerXOwnedCities = 0;
	m_iExtraTradeRoutesPerXOwnedVassals = 0;
	m_iMinorInfluencePerGiftedUnit = 0;
	m_bIsCapitalOnly = false;
	m_iInvestmentModifier = 0;
	m_iCombatBonusVsHigherTech = 0;
	m_iCombatBonusVsLargerCiv = 0;
	m_iLandUnitMaintenanceModifier = 0;
	m_iNavalUnitMaintenanceModifier = 0;
	m_iRazeSpeedModifier = 0;
	m_iDOFGreatPersonModifier = 0;
	m_iLuxuryHappinessRetention = 0;
	m_iExtraSupply = 0;
	m_iExtraSupplyPerCity = 0;
	m_iExtraSupplyPerPopulation = 0;
	m_iExtraSpies = 0;
	m_iUnresearchedTechBonusFromKills = 0;
	m_iExtraFoundedCityTerritoryClaimRange = 0;
	m_iExtraConqueredCityTerritoryClaimRange = 0;
	m_iExtraTenetsFirstAdoption = 0;
	m_iMonopolyModFlat = 0;
	m_iMonopolyModPercent = 0;
	m_iFreeSocialPoliciesPerEra = 0;
	m_iNumTradeRoutesModifier = 0;
	m_iTradeRouteResourceModifier = 0;
	m_iUniqueLuxuryCities = 0;
	m_iUniqueLuxuryQuantity = 0;
	m_iWorkerSpeedModifier = 0;
	m_iAfraidMinorPerTurnInfluence = 0;
	m_iLandTradeRouteRangeBonus = 0;
	m_iSeaTradeRouteRangeBonus = 0;
	m_iTradeReligionModifier = 0;
	m_iTradeBuildingModifier = 0;
	m_iNumFreeBuildings = 0;
	m_eFreeUnitOnConquest = NO_UNIT;
	m_iNonSpecialistFoodChange = 0;
	m_bBullyAnnex = false;
	m_iBullyYieldMultiplierAnnex = 0;
	m_bFightWellDamaged = false;
	m_bWoodlandMovementBonus = false;
	m_bRiverMovementBonus = false;
	m_bFasterInHills = false;
	m_bEmbarkedAllWater = false;
	m_bEmbarkedToLandFlatCost = false;
	m_bNoHillsImprovementMaintenance = false;
	m_bTechBoostFromCapitalScienceBuildings = false;
	m_bArtistGoldenAgeTechBoost = false;
	m_bStaysAliveZeroCities = false;
	m_bFaithFromUnimprovedForest = false;
	m_bAnyBelief = false;
	m_bAlwaysReligion = false;
	m_bNoTradeRouteProximityPenalty = false;
	m_bCanPlunderWithoutWar = false;
	m_bBonusReligiousBelief = false;
	m_bAbleToAnnexCityStates = false;
	m_bCrossesMountainsAfterGreatGeneral = false;
	m_bCrossesIce = false;
	m_bGGFromBarbarians = false;
	m_bMayaCalendarBonuses = false;
	m_bNoAnnexing = false;
	m_bTechFromCityConquer = false;
	m_bUniqueLuxuryRequiresNewArea = false;
	m_bRiverTradeRoad = false;
	m_bAngerFreeIntrusionOfCityStates = false;
	m_iBasicNeedsMedianModifier = 0;
	m_iGoldMedianModifier = 0;
	m_iScienceMedianModifier = 0;
	m_iCultureMedianModifier = 0;
	m_iReligiousUnrestModifier = 0;
	m_bNoConnectionUnhappiness = false;
	m_bIsNoReligiousStrife = false;
	m_bIsOddEraScaler = false;
	m_iWonderProductionModGA = 0;
	m_iCultureBonusModifierConquest = 0;
	m_iProductionBonusModifierConquest = 0;


	m_eCampGuardType = NO_UNIT;
	m_eCombatBonusImprovement = NO_IMPROVEMENT;

	m_ppaaiImprovementYieldChange.clear();
	m_ppaaiImprovementYieldChange.resize(GC.getNumImprovementInfos());
	m_ppiYieldFromTileEarnTerrainType.clear();
	m_ppiYieldFromTileEarnTerrainType.resize(GC.getNumTerrainInfos());
	m_ppiYieldFromTilePurchaseTerrainType.clear();
	m_ppiYieldFromTilePurchaseTerrainType.resize(GC.getNumTerrainInfos());
	m_ppiYieldFromTileConquest.clear();
	m_ppiYieldFromTileConquest.resize(GC.getNumTerrainInfos());
	m_ppiYieldFromTileCultureBomb.clear();
	m_ppiYieldFromTileCultureBomb.resize(GC.getNumTerrainInfos());
	m_ppiYieldFromTileStealCultureBomb.clear();
	m_ppiYieldFromTileStealCultureBomb.resize(GC.getNumTerrainInfos());
	m_ppiYieldFromTileSettle.clear();
	m_ppiYieldFromTileSettle.resize(GC.getNumTerrainInfos());
	m_ppaaiYieldChangePerImprovementBuilt.clear();
	m_ppaaiYieldChangePerImprovementBuilt.resize(GC.getNumImprovementInfos());
	UpdateYieldChangeImprovementTypes();
	m_pbiYieldFromBarbarianCampClear.clear();
	m_pbiYieldFromRouteMovementInForeignTerritory.clear();
	m_ppiPlotYieldChange.clear();
	m_ppiPlotYieldChange.resize(GC.getNumPlotInfos());
	m_ppiBuildingClassYieldChange.clear();
	m_ppiBuildingClassYieldChange.resize(GC.getNumBuildingClassInfos());
	m_ppiFeatureYieldChange.clear();
	m_ppiFeatureYieldChange.resize(GC.getNumFeatureInfos());
	m_ppiResourceYieldChange.clear();
	m_ppiResourceYieldChange.resize(GC.getNumResourceInfos());
	m_miResourceYieldChangesFromGoldenAge.clear();
	m_miResourceYieldChangesFromGoldenAgeCap.clear();
	m_ppiTerrainYieldChange.clear();
	m_ppiTerrainYieldChange.resize(GC.getNumTerrainInfos());
	m_ppiTradeRouteYieldChange.clear();
	m_ppiTradeRouteYieldChange.resize(NUM_DOMAIN_TYPES);
	m_ppaaiSpecialistYieldChange.clear();
	m_ppaaiSpecialistYieldChange.resize(GC.getNumSpecialistInfos());
	m_ppiGreatPersonExpendedYield.clear();
	m_ppiGreatPersonExpendedYield.resize(GC.getNumGreatPersonInfos());
	m_ppiGreatPersonBornYield.clear();
	m_ppiGreatPersonBornYield.resize(GC.getNumGreatPersonInfos());
	m_ppiCityYieldFromUnimprovedFeature.clear();
	m_ppiCityYieldFromUnimprovedFeature.resize(GC.getNumFeatureInfos());
	m_ppaaiUnimprovedFeatureYieldChange.clear();
	m_ppaaiUnimprovedFeatureYieldChange.resize(GC.getNumFeatureInfos());

	Firaxis::Array< int, NUM_YIELD_TYPES > yield;
	for(unsigned int j = 0; j < NUM_YIELD_TYPES; ++j)
	{
		yield[j] = 0;
	}

	m_tradeRouteEndYieldDomestic.clear();
	m_tradeRouteEndYieldInternational.clear();

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
			m_ppaaiYieldChangePerImprovementBuilt[iImprovement] = yield;
		}
		UpdateYieldChangeImprovementTypes();

		for(int iPlot = 0; iPlot < GC.getNumPlotInfos(); iPlot++)
		{
			m_ppiPlotYieldChange[iPlot] = yield;
		}

		m_iYieldFromLevelUp[iYield] = 0;
		m_iYieldFromHistoricEvent[iYield] = 0;
		m_iYieldFromXMilitaryUnits[iYield] = 0;
		m_iYieldFromOwnPantheon[iYield] = 0;
		m_iYieldFromRouteMovement[iYield] = 0;
		m_iYieldFromExport[iYield] = 0;
		m_iYieldFromImport[iYield] = 0;
		m_iYieldFromTilePurchase[iYield] = 0;
		m_iYieldFromTileEarn[iYield] = 0;
		m_iYieldFromSettle[iYield] = 0;
		m_iYieldFromConquest[iYield] = 0;
		m_iYieldFromCityDamageTimes100[iYield] = 0;
		m_iYieldFromCSAlly[iYield] = 0;
		m_iYieldFromCSFriend[iYield] = 0;
		m_aiGoldenAgeYieldModifier.erase(iYield);
		m_iPurchasedUnitsBonusXP = 0;
		m_iVotePerXCSAlliance = 0;
		m_iVotePerXCSFollowingFollowingYourReligion = 0;
		m_iChanceToConvertReligiousUnits = 0;
		m_iGoldenAgeFromVictory = 0;
		m_iFreePolicyPerXTechs = 0;
		m_eGPFaithPurchaseEra = NO_ERA;
		m_iFaithCostModifier = 0;
		m_bFreeGreatWorkOnConquest = false;
		m_bPopulationBoostReligion = false;
		m_bStartsWithPantheon = false;
		m_bProphetFervor = false;
		m_bCombatBoostNearNaturalWonder = false;
		for(int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); iBuildingClass++)
		{
			m_ppiBuildingClassYieldChange[iBuildingClass] = yield;
		}
		m_iCapitalYieldChanges[iYield] = 0;
		m_iCityYieldChanges[iYield] = 0;
		m_iPermanentYieldChangeWLTKD[iYield] = 0;
		m_iCoastalCityYieldChanges[iYield] = 0;
		m_iGreatWorkYieldChanges[iYield] = 0;
		m_iArtifactYieldChanges[iYield] = 0;
		m_iArtYieldChanges[iYield] = 0;
		m_iLitYieldChanges[iYield] = 0;
		m_iMusicYieldChanges[iYield] = 0;
		m_iSeaPlotYieldChanges[iYield] = 0;
		m_iGAPToYield[iYield] = 0;
		m_iMountainRangeYield[iYield] = 0;
		m_iLuxuryYieldChanges[iYield] = 0;
		for(int iFeature = 0; iFeature < GC.getNumFeatureInfos(); iFeature++)
		{
			m_ppiFeatureYieldChange[iFeature] = yield;
		}
		for(int iResource = 0; iResource < GC.getNumResourceInfos(); iResource++)
		{
			m_ppiResourceYieldChange[iResource] = yield;
		}
		for(int iTerrain = 0; iTerrain < GC.getNumTerrainInfos(); iTerrain++)
		{
			m_ppiTerrainYieldChange[iTerrain] = yield;
			m_ppiYieldFromTileEarnTerrainType[iTerrain] = yield;
			m_ppiYieldFromTilePurchaseTerrainType[iTerrain] = yield;
			m_ppiYieldFromTileConquest[iTerrain] = yield;
			m_ppiYieldFromTileCultureBomb[iTerrain] = yield;
			m_ppiYieldFromTileStealCultureBomb[iTerrain] = yield;
			m_ppiYieldFromTileSettle[iTerrain] = yield;
		}
		m_iYieldFromKills[iYield] = 0;
		m_iYieldFromBarbarianKills[iYield] = 0;
		m_iYieldFromMinorDemand[iYield] = 0;
		m_iYieldFromLuxuryResourceGain[iYield] = 0;
		m_iYieldChangeTradeRoute[iYield] = 0;
		m_iYieldChangeWorldWonder[iYield] = 0;
		for(int iDomain = 0; iDomain < NUM_DOMAIN_TYPES; iDomain++)
		{
			m_ppiTradeRouteYieldChange[iDomain] = yield;
		}
		for(int iGreatPerson = 0; iGreatPerson < GC.getNumGreatPersonInfos(); iGreatPerson++)
		{
			m_ppiGreatPersonExpendedYield[iGreatPerson] = yield;
			m_ppiGreatPersonBornYield[iGreatPerson] = yield;
		}

		for(int iSpecialist = 0; iSpecialist < GC.getNumSpecialistInfos(); iSpecialist++)
		{
			m_ppaaiSpecialistYieldChange[iSpecialist] = yield;
		}
		for(int iFeature = 0; iFeature < GC.getNumFeatureInfos(); iFeature++)
		{
			m_ppiCityYieldFromUnimprovedFeature[iFeature] = yield;
			m_ppaaiUnimprovedFeatureYieldChange[iFeature] = yield;
		}
	}

	m_abTerrainClaimBoost.clear();
	m_abTerrainClaimBoost.resize(GC.getNumTerrainInfos());

	for(int iTerrain = 0; iTerrain < GC.getNumTerrainInfos(); iTerrain++)
	{
		m_iStrategicResourceQuantityModifier[iTerrain] = 0;
		m_abTerrainClaimBoost[iTerrain] = false;
	}
	m_aiResourceQuantityModifier.clear();
	m_aiResourceQuantityModifier.resize(GC.getNumResourceInfos());
	m_aiNumFreeResourceOnWorldWonderCompletion.clear();
	m_aiNumFreeResourceOnWorldWonderCompletion.resize(GC.getNumResourceInfos());

	for(int iResource = 0; iResource < GC.getNumResourceInfos(); iResource++)
	{
		m_aiResourceQuantityModifier[iResource] = 0;
		m_aiNumFreeResourceOnWorldWonderCompletion[iResource] = 0;
	}

	m_abNoTrain.clear();
	m_abNoTrain.resize(GC.getNumUnitClassInfos());
	for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); iUnitClass++)
	{
		m_abNoTrain[iUnitClass] = false;
	}

	m_aFreeTraitUnits.clear();

	int iNumUnitCombatClassInfos = GC.getNumUnitCombatClassInfos();
	ASSERT((0 < iNumUnitCombatClassInfos),  "GC.getNumUnitCombatClassInfos() is not greater than zero but an array is being allocated in CvPlayerTraits::Reset");
	m_paiMovesChangeUnitCombat.clear();
	m_paiMaintenanceModifierUnitCombat.clear();
	m_paiMovesChangeUnitCombat.resize(iNumUnitCombatClassInfos);
	m_paiMaintenanceModifierUnitCombat.resize(iNumUnitCombatClassInfos);
	for(int iI = 0; iI < iNumUnitCombatClassInfos; iI++)
	{
		m_paiMovesChangeUnitCombat[iI] = 0;
		m_paiMaintenanceModifierUnitCombat[iI] = 0;
		m_aibUnitCombatProductionCostModifier.erase(iI);
	}
	m_aiGreatPersonCostReduction.clear();
	m_aiPerPuppetGreatPersonRateModifier.clear();
	m_aiGreatPersonGWAM.clear();
	m_aiGoldenAgeGreatPersonRateModifier.clear();
	m_aiGoldenAgeFromGreatPersonBirth.clear();
	m_aiGreatPersonProgressFromPolicyUnlock.clear();
	m_aiGreatPersonProgressFromKills.clear();
	m_aiRandomGreatPersonProgressFromKills.clear();

	m_aiGreatPersonCostReduction.resize(GC.getNumGreatPersonInfos());
	m_aiPerPuppetGreatPersonRateModifier.resize(GC.getNumGreatPersonInfos());
	m_aiGreatPersonGWAM.resize(GC.getNumGreatPersonInfos());
	m_aiGoldenAgeGreatPersonRateModifier.resize(GC.getNumGreatPersonInfos());
	m_aiGoldenAgeFromGreatPersonBirth.resize(GC.getNumGreatPersonInfos());
	m_aiGreatPersonProgressFromPolicyUnlock.resize(GC.getNumGreatPersonInfos());
	for (int iI = 0; iI < GC.getNumGreatPersonInfos(); iI++)
	{
		m_aiGreatPersonCostReduction[iI] = 0;
		m_aiPerPuppetGreatPersonRateModifier[iI] = 0;
		m_aiGreatPersonGWAM[iI] = 0;
		m_aiGoldenAgeGreatPersonRateModifier[iI] = 0;
		m_aiGoldenAgeFromGreatPersonBirth[iI] = 0;
		m_aiGreatPersonProgressFromPolicyUnlock[iI] = 0;
	}

	m_aiNumPledgesDomainProdMod.clear();
	m_aiFreeUnitClassesDOW.clear();
	m_aiDomainFreeExperienceModifier.clear();

	m_aiNumPledgesDomainProdMod.resize(NUM_DOMAIN_TYPES);
	m_aiDomainFreeExperienceModifier.resize(NUM_DOMAIN_TYPES);
	m_aiFreeUnitClassesDOW.resize(GC.getNumUnitClassInfos());

	for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		m_aiNumPledgesDomainProdMod[iI] = 0;
		m_aiDomainFreeExperienceModifier[iI] = 0;
	}

	m_paiMovesChangeUnitClass.clear();
	m_paiMovesChangeUnitClass.resize(GC.getNumUnitClassInfos());
	for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); iUnitClass++)
	{
		m_paiMovesChangeUnitClass[iUnitClass] = 0;
		m_aiFreeUnitClassesDOW[iUnitClass] = 0;
	}
	m_aiNoBuilds.clear();
	m_aiDomainProductionModifiersPerSpecialist.clear();
	m_aiiTradeRouteProductionSiphon.clear();
	m_aiiAlternateResourceTechs.clear();
	int iResourceLoop = 0;
	for(iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		FreeResourceXCities temp;
		m_aFreeResourceXCities.push_back(temp);
	}
}

/// Does this player possess a specific trait?
bool CvPlayerTraits::HasTrait(TraitTypes eTrait) const
{
	ASSERT(m_pPlayer);

	if(m_pPlayer != NULL)
	{
		ASSERT((m_pPlayer->getLeaderType() >= 0), "getLeaderType() is less than zero");
		ASSERT((eTrait >= 0), "eTrait is less than zero");

		TeamTypes eTeam = m_pPlayer->getTeam();
		PlayerTypes ePlayer = m_pPlayer->GetID();
		CvTraitEntry* pTrait = m_pTraits->GetEntry(eTrait);
		return m_vLeaderHasTrait[eTrait] && 
			   ((!pTrait->IsObsoleteByBelief(ePlayer) && pTrait->IsEnabledByBelief(ePlayer)) &&
			    (!pTrait->IsObsoleteByPolicy(ePlayer) && pTrait->IsEnabledByPolicy(ePlayer)) &&
			    (!pTrait->IsObsoleteByTech(eTeam)     && pTrait->IsEnabledByTech(eTeam)));
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

/// Instant great person progress when killing enemy units
int CvPlayerTraits::GetGreatPersonProgressFromKills(GreatPersonTypes eIndex) const
{
	PRECONDITION((int)eIndex < GC.getNumGreatPersonInfos(), "Yield type out of bounds");
	PRECONDITION((int)eIndex > -1, "Index out of bounds");

	std::map<int, int>::const_iterator it = m_aiGreatPersonProgressFromKills.find((int)eIndex);
	if (it != m_aiGreatPersonProgressFromKills.end()) // find returns the iterator to map::end if the key eIndex is not present in the map
	{
		return it->second;
	}

	return 0;
}

/// Does this civilization get random great person progress when killing enemy units?
bool CvPlayerTraits::IsRandomGreatPersonProgressFromKills() const
{
	return !m_aiRandomGreatPersonProgressFromKills.empty();
}

/// Instant random great person progress when killing enemy units
std::pair<GreatPersonTypes, int> CvPlayerTraits::GetRandomGreatPersonProgressFromKills(const CvSeeder& additionalSeed) const
{
	// how many options we have
	int iSize = m_aiRandomGreatPersonProgressFromKills.size();
	uint uChoice = 0;

	if (iSize > 0)
	{
		// get our pseudo RNG seed
		uChoice = GC.getGame().urandLimitExclusive(m_aiRandomGreatPersonProgressFromKills.size(), additionalSeed.mix(m_pPlayer->GetPseudoRandomSeed()).mix(GC.getGame().getNumCities()));

		// access the element at the position of our RNG seed
		for (std::map<int, int>::const_iterator it = m_aiRandomGreatPersonProgressFromKills.begin(); it != m_aiRandomGreatPersonProgressFromKills.end(); it++) // find returns the iterator to map::end if the key eIndex is not present in the map
		{
			if (uChoice == 0)
			{
				return std::make_pair((GreatPersonTypes)it->first, it->second);
			}
			uChoice--;
		}
	}

	return std::make_pair(NO_GREATPERSON, 0);
}

/// Bonus movement for this combat class
int CvPlayerTraits::GetMovesChangeUnitCombat(const int unitCombatID) const
{
	PRECONDITION(unitCombatID < GC.getNumUnitCombatClassInfos(),  "Invalid unitCombatID parameter in call to CvPlayerTraits::GetMovesChangeUnitCombat()");

	if(unitCombatID == NO_UNITCLASS)
	{
		return 0;
	}

	return m_paiMovesChangeUnitCombat[unitCombatID];
}
/// Bonus movement for this unit class
int CvPlayerTraits::GetMovesChangeUnitClass(const int unitClassID) const
{
	PRECONDITION(unitClassID < GC.getNumUnitClassInfos(),  "Invalid unitClassID parameter in call to CvPlayerTraits::GetMovesChangeUnitClass()");

	if(unitClassID == NO_UNITCLASS)
	{
		return 0;
	}

	return m_paiMovesChangeUnitClass[unitClassID];
}

/// Maintenance modifier for this combat class
int CvPlayerTraits::GetMaintenanceModifierUnitCombat(const int unitCombatID) const
{
	PRECONDITION(unitCombatID < GC.getNumUnitCombatClassInfos(),  "Invalid unitCombatID parameter in call to CvPlayerTraits::GetMaintenanceModifierUnitCombat()");
	ASSERT(unitCombatID > -1, "Index out of bounds");

	if(unitCombatID == NO_UNITCLASS)
	{
		return 0;
	}

	return m_paiMaintenanceModifierUnitCombat[unitCombatID];
}

/// Extra yield from this improvement
int CvPlayerTraits::GetImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYield) const
{
	PRECONDITION(eImprovement < GC.getNumImprovementInfos(),  "Invalid eImprovement parameter in call to CvPlayerTraits::GetImprovementYieldChange()");
	PRECONDITION(eYield < NUM_YIELD_TYPES,  "Invalid eYield parameter in call to CvPlayerTraits::GetImprovementYieldChange()");
	PRECONDITION(eYield > -1, "Index out of bounds");

	if(eImprovement == NO_IMPROVEMENT)
	{
		return 0;
	}

	return m_ppaaiImprovementYieldChange[(int)eImprovement][(int)eYield];
}
int CvPlayerTraits::GetYieldChangeFromTileEarnTerrainType(TerrainTypes eTerrain, YieldTypes eYield) const
{
	PRECONDITION(eTerrain < GC.getNumTerrainInfos(),  "Invalid eImprovement parameter in call to CvPlayerTraits::GetYieldChangeFromTileEarnTerrainType()");
	PRECONDITION(eYield < NUM_YIELD_TYPES,  "Invalid eYield parameter in call to CvPlayerTraits::GetYieldChangeFromTileEarnTerrainType()");
	PRECONDITION(eYield > -1, "Index out of bounds");

	if(eTerrain == NO_TERRAIN)
	{
		return 0;
	}
	return m_ppiYieldFromTileEarnTerrainType[(int)eTerrain][(int)eYield];
}
int CvPlayerTraits::GetYieldChangeFromTilePurchaseTerrainType(TerrainTypes eTerrain, YieldTypes eYield) const
{
	PRECONDITION(eTerrain < GC.getNumTerrainInfos(), "Invalid eTerrain parameter in call to CvPlayerTraits::GetYieldChangeFromTilePurchaseTerrainType()");
	PRECONDITION(eYield < NUM_YIELD_TYPES, "Invalid eYield parameter in call to CvPlayerTraits::GetYieldChangeFromTilePurchaseTerrainType()");
	PRECONDITION(eYield > -1, "Index out of bounds");

	if (eTerrain == NO_TERRAIN)
	{
		return 0;
	}
	return m_ppiYieldFromTilePurchaseTerrainType[(int)eTerrain][(int)eYield];
}
int CvPlayerTraits::GetYieldChangeFromTileConquest(TerrainTypes eTerrain, YieldTypes eYield) const
{
	PRECONDITION(eTerrain < GC.getNumTerrainInfos(), "Invalid eTerrain parameter in call to CvPlayerTraits::GetYieldChangeFromTileConquest()");
	PRECONDITION(eYield < NUM_YIELD_TYPES, "Invalid eYield parameter in call to CvPlayerTraits::GetYieldChangeFromTileConquest()");
	PRECONDITION(eYield > -1, "Index out of bounds");

	if (eTerrain == NO_TERRAIN)
	{
		return 0;
	}
	return m_ppiYieldFromTileConquest[(int)eTerrain][(int)eYield];
}
int CvPlayerTraits::GetYieldChangeFromTileCultureBomb(TerrainTypes eTerrain, YieldTypes eYield) const
{
	PRECONDITION(eTerrain < GC.getNumTerrainInfos(), "Invalid eTerrain parameter in call to CvPlayerTraits::GetYieldChangeFromTileCultureBomb()");
	PRECONDITION(eYield < NUM_YIELD_TYPES, "Invalid eYield parameter in call to CvPlayerTraits::GetYieldChangeFromTileCultureBomb()");
	PRECONDITION(eYield > -1, "Index out of bounds");

	if (eTerrain == NO_TERRAIN)
	{
		return 0;
	}
	return m_ppiYieldFromTileCultureBomb[(int)eTerrain][(int)eYield];
}
int CvPlayerTraits::GetYieldChangeFromTileStealCultureBomb(TerrainTypes eTerrain, YieldTypes eYield) const
{
	PRECONDITION(eTerrain < GC.getNumTerrainInfos(), "Invalid eTerrain parameter in call to CvPlayerTraits::GetYieldChangeFromTileStealCultureBomb()");
	PRECONDITION(eYield < NUM_YIELD_TYPES, "Invalid eYield parameter in call to CvPlayerTraits::GetYieldChangeFromTileStealCultureBomb()");
	PRECONDITION(eYield > -1, "Index out of bounds");

	if (eTerrain == NO_TERRAIN)
	{
		return 0;
	}
	return m_ppiYieldFromTileStealCultureBomb[(int)eTerrain][(int)eYield];
}
int CvPlayerTraits::GetYieldChangeFromTileSettle(TerrainTypes eTerrain, YieldTypes eYield) const
{
	PRECONDITION(eTerrain < GC.getNumTerrainInfos(), "Invalid eTerrain parameter in call to CvPlayerTraits::GetYieldChangeFromTileSettle()");
	PRECONDITION(eYield < NUM_YIELD_TYPES, "Invalid eYield parameter in call to CvPlayerTraits::GetYieldChangeFromTileSettle()");
	PRECONDITION(eYield > -1, "Index out of bounds");

	if (eTerrain == NO_TERRAIN)
	{
		return 0;
	}
	return m_ppiYieldFromTileSettle[(int)eTerrain][(int)eYield];
}
int CvPlayerTraits::GetYieldChangePerImprovementBuilt(ImprovementTypes eImprovement, YieldTypes eYield) const
{
	PRECONDITION(eImprovement < GC.getNumImprovementInfos(), "Invalid eImprovement parameter in call to CvPlayerTraits::GetYieldChangePerImprovementBuilt()");
	PRECONDITION(eYield < NUM_YIELD_TYPES, "Invalid eYield parameter in call to CvPlayerTraits::GetYieldChangePerImprovementBuilt()");
	PRECONDITION(eYield > -1, "Index out of bounds");

	if (eImprovement == NO_IMPROVEMENT)
	{
		return 0;
	}
	return m_ppaaiYieldChangePerImprovementBuilt[(int)eImprovement][(int)eYield];
}

vector<ImprovementTypes> CvPlayerTraits::GetImprovementTypesWithYieldChange() const
{
	return m_vYieldChangeImprovementTypes;
}

void CvPlayerTraits::UpdateYieldChangeImprovementTypes()
{
	m_vYieldChangeImprovementTypes.clear();

	for (size_t iImprovement = 0; iImprovement < m_ppaaiYieldChangePerImprovementBuilt.size(); iImprovement++)
	{
		for (size_t iYield = 0; iYield < m_ppaaiYieldChangePerImprovementBuilt[iImprovement].size(); iYield++)
		{
			//remember the improvement type if there's one or more yields it affects
			if (m_ppaaiYieldChangePerImprovementBuilt[iImprovement][iYield] != 0)
			{
				m_vYieldChangeImprovementTypes.push_back((ImprovementTypes)iImprovement);
				break;
			}
		}
	}
}

int CvPlayerTraits::GetYieldFromBarbarianCampClear(YieldTypes eYield, bool bEraScaling) const
{
	PRECONDITION(eYield < NUM_YIELD_TYPES, "Invalid eYield parameter in call to CvPlayerTraits::GetYieldFromBarbarianCampClear()");
	PRECONDITION(eYield > -1, "Index out of bounds");

	std::map<int, std::map<bool, int>>::const_iterator itYield = m_pbiYieldFromBarbarianCampClear.find((int)eYield);
	if (itYield != m_pbiYieldFromBarbarianCampClear.end()) // find returns the iterator to map::end if the key eYield is not present in the map
	{
		std::map<bool, int>::const_iterator itBool = itYield->second.find(bEraScaling);
		if (itBool != itYield->second.end())
		{
			return itBool->second;
		}

		return 0;
	}

	return 0;
}

int CvPlayerTraits::GetYieldFromRouteMovementInForeignTerritory(YieldTypes eIndex, bool bTradePartner) const
{
	PRECONDITION(eIndex < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex > -1, "Index out of bounds");

	std::map<int, std::map<bool, int>>::const_iterator itYield = m_pbiYieldFromRouteMovementInForeignTerritory.find((int)eIndex);
	if (itYield != m_pbiYieldFromRouteMovementInForeignTerritory.end()) // find returns the iterator to map::end if the key eYield is not present in the map
	{
		std::map<bool, int>::const_iterator itBool = itYield->second.find(bTradePartner);
		if (itBool != itYield->second.end())
		{
			return itBool->second;
		}
	}

	return 0;
}

/// Extra yield from this plot
int CvPlayerTraits::GetPlotYieldChange(PlotTypes ePlot, YieldTypes eYield) const
{
	PRECONDITION(ePlot < GC.getNumPlotInfos(),  "Invalid ePlot parameter in call to CvPlayerTraits::GetPlotYieldChange()");
	PRECONDITION(eYield < NUM_YIELD_TYPES,  "Invalid eYield parameter in call to CvPlayerTraits::GetPlotYieldChange()");
	PRECONDITION(eYield > -1, "Index out of bounds");

	return m_ppiPlotYieldChange[(int)ePlot][(int)eYield];
}

int CvPlayerTraits::GetBuildingClassYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield) const
{
	PRECONDITION(eBuildingClass < GC.getNumBuildingClassInfos(),  "Invalid eBuildingClass parameter in call to CvPlayerTraits::GetBuildingClassYieldChange()");
	PRECONDITION(eYield < NUM_YIELD_TYPES,  "Invalid eYield parameter in call to CvPlayerTraits::GetBuildingClassYieldChange()");
	PRECONDITION(eYield > -1, "Index out of bounds");

	if(eBuildingClass == NO_BUILDINGCLASS)
	{
		return 0;
	}

	return m_ppiBuildingClassYieldChange[(int)eBuildingClass][(int)eYield];
}

int CvPlayerTraits::GetFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYield) const
{
	PRECONDITION(eFeature < GC.getNumFeatureInfos(),  "Invalid eFeature parameter in call to CvPlayerTraits::GetFeatureYieldChange()");
	PRECONDITION(eYield < NUM_YIELD_TYPES,  "Invalid eYield parameter in call to CvPlayerTraits::GetFeatureYieldChange()");
	PRECONDITION(eYield > -1, "Index out of bounds");

	return m_ppiFeatureYieldChange[(int)eFeature][(int)eYield];
}

int CvPlayerTraits::GetResourceYieldChange(ResourceTypes eResource, YieldTypes eYield) const
{
	PRECONDITION(eResource < GC.getNumResourceInfos(),  "Invalid eResource parameter in call to CvPlayerTraits::GetResourceYieldChange()");
	PRECONDITION(eYield < NUM_YIELD_TYPES,  "Invalid eYield parameter in call to CvPlayerTraits::GetResourceYieldChange()");
	PRECONDITION(eYield > -1, "Index out of bounds");

	return m_ppiResourceYieldChange[(int)eResource][(int)eYield];
}


int CvPlayerTraits::GetResourceYieldChangesFromGoldenAge(ResourceTypes eResource, YieldTypes eYield) const
{
	PRECONDITION(eYield < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eYield > -1, "Index out of bounds");

	std::map<int, std::map<int, int>>::const_iterator itResource = m_miResourceYieldChangesFromGoldenAge.find((int)eResource);
	if (itResource != m_miResourceYieldChangesFromGoldenAge.end())
	{
		std::map<int, int>::const_iterator itYield = itResource->second.find(eYield);
		if (itYield != itResource->second.end())
		{
			return itYield->second;
		}
	}

	return 0;
}

int CvPlayerTraits::GetResourceYieldChangesFromGoldenAgeCap(ResourceTypes eResource, YieldTypes eYield) const
{
	PRECONDITION(eYield < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eYield > -1, "Index out of bounds");

	std::map<int, std::map<int, int>>::const_iterator itResource = m_miResourceYieldChangesFromGoldenAgeCap.find((int)eResource);
	if (itResource != m_miResourceYieldChangesFromGoldenAgeCap.end())
	{
		std::map<int, int>::const_iterator itYield = itResource->second.find(eYield);
		if (itYield != itResource->second.end())
		{
			return itYield->second;
		}
	}

	return 0;
}


int CvPlayerTraits::GetTerrainYieldChange(TerrainTypes eTerrain, YieldTypes eYield) const
{
	PRECONDITION(eTerrain < GC.getNumTerrainInfos(),  "Invalid eTerrain parameter in call to CvPlayerTraits::GetTerrainYieldChange()");
	PRECONDITION(eYield < NUM_YIELD_TYPES,  "Invalid eYield parameter in call to CvPlayerTraits::GetTerrainYieldChange()");

	return m_ppiTerrainYieldChange[(int)eTerrain][(int)eYield];
}

int CvPlayerTraits::GetYieldFromKills(YieldTypes eYield) const
{
	PRECONDITION((int)eYield < NUM_YIELD_TYPES, "Yield type out of bounds");
	PRECONDITION((int)eYield > -1, "Index out of bounds");
	return m_iYieldFromKills[(int)eYield];
}

int CvPlayerTraits::GetYieldFromBarbarianKills(YieldTypes eYield) const
{
	PRECONDITION((int)eYield < NUM_YIELD_TYPES, "Yield type out of bounds");
	PRECONDITION((int)eYield > -1, "Index out of bounds");
	return m_iYieldFromBarbarianKills[(int)eYield];
}

int CvPlayerTraits::GetYieldFromMinorDemand(YieldTypes eYield) const
{
	PRECONDITION((int)eYield < NUM_YIELD_TYPES, "Yield type out of bounds");
	PRECONDITION((int)eYield > -1, "Index out of bounds");
	return m_iYieldFromMinorDemand[(int)eYield];
}

int CvPlayerTraits::GetYieldFromLuxuryResourceGain(YieldTypes eYield) const
{
	PRECONDITION((int)eYield < NUM_YIELD_TYPES, "Yield type out of bounds");
	PRECONDITION((int)eYield > -1, "Index out of bounds");
	return m_iYieldFromLuxuryResourceGain[(int)eYield];
}

/// Extra yield from this specialist
int CvPlayerTraits::GetTradeRouteYieldChange(DomainTypes eDomain, YieldTypes eYield) const
{
	PRECONDITION(eDomain < NUM_DOMAIN_TYPES,  "Invalid eDomain parameter in call to CvPlayerTraits::GetDomainYieldChange()");
	PRECONDITION(eYield < NUM_YIELD_TYPES,  "Invalid eYield parameter in call to CvPlayerTraits::GetDomainYieldChange()");

	if(eDomain == NO_DOMAIN)
	{
		return 0;
	}

	return m_ppiTradeRouteYieldChange[(int)eDomain][(int)eYield];
}

/// Extra yield from this specialist
int CvPlayerTraits::GetSpecialistYieldChange(SpecialistTypes eSpecialist, YieldTypes eYield) const
{
	PRECONDITION(eSpecialist < GC.getNumSpecialistInfos(),  "Invalid eSpecialist parameter in call to CvPlayerTraits::GetSpecialistYieldChange()");
	PRECONDITION(eYield < NUM_YIELD_TYPES,  "Invalid eYield parameter in call to CvPlayerTraits::GetSpecialistYieldChange()");

	if(eSpecialist == NO_SPECIALIST)
	{
		return 0;
	}
	if(IsOddEraScaler())
	{
		int iYield = m_ppaaiSpecialistYieldChange[(int)eSpecialist][(int)eYield];
		if(iYield > 0)
		{
			if(m_pPlayer->GetCurrentEra() >= (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true))
			{
				iYield += m_ppaaiSpecialistYieldChange[(int)eSpecialist][(int)eYield];
			}
			if(m_pPlayer->GetCurrentEra() >= (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true))
			{
				iYield += m_ppaaiSpecialistYieldChange[(int)eSpecialist][(int)eYield];
			}
			if(m_pPlayer->GetCurrentEra() >= (EraTypes) GC.getInfoTypeForString("ERA_POSTMODERN", true))
			{
				iYield += m_ppaaiSpecialistYieldChange[(int)eSpecialist][(int)eYield];
			}
			return iYield;
		}
	}

	return m_ppaaiSpecialistYieldChange[(int)eSpecialist][(int)eYield];
}

int CvPlayerTraits::GetGreatPersonExpendedYield(GreatPersonTypes eGreatPerson, YieldTypes eYield) const
{
	PRECONDITION(eGreatPerson < GC.getNumGreatPersonInfos(), "Invalid eGreatPerson parameter in call to CvPlayerTraits::GetGreatPersonExpendedYield()");
	PRECONDITION(eYield < NUM_YIELD_TYPES,  "Invalid eYield parameter in call to CvPlayerTraits::GetGreatPersonExpendedYield()");

	if(eGreatPerson == NO_GREATPERSON)
	{
		return 0;
	}

	return m_ppiGreatPersonExpendedYield[(int)eGreatPerson][(int)eYield];
}
int CvPlayerTraits::GetGreatPersonBornYield(GreatPersonTypes eGreatPerson, YieldTypes eYield) const
{
	PRECONDITION(eGreatPerson < GC.getNumGreatPersonInfos(),  "Invalid eGreatPerson parameter in call to CvPlayerTraits::GetGreatPersonBornYield()");
	PRECONDITION(eYield < NUM_YIELD_TYPES,  "Invalid eYield parameter in call to CvPlayerTraits::GetGreatPersonBornYield()");

	if(eGreatPerson == NO_GREATPERSON)
	{
		return 0;
	}

	return m_ppiGreatPersonBornYield[(int)eGreatPerson][(int)eYield];
}

int CvPlayerTraits::GetCityYieldFromUnimprovedFeature(FeatureTypes eFeature, YieldTypes eYield) const
{
	PRECONDITION(eFeature < GC.getNumFeatureInfos(),  "Invalid eImprovement parameter in call to CvPlayerTraits::GetCityYieldFromUnimprovedFeature()");
	PRECONDITION(eYield < NUM_YIELD_TYPES,  "Invalid eYield parameter in call to CvPlayerTraits::GetCityYieldFromUnimprovedFeature()");

	if(eFeature == NO_FEATURE)
	{
		return 0;
	}

	return m_ppiCityYieldFromUnimprovedFeature[(int)eFeature][(int)eYield];
}

/// Extra yield from a feature without improvement
int CvPlayerTraits::GetUnimprovedFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYield) const
{
	PRECONDITION(eFeature < GC.getNumFeatureInfos(),  "Invalid eImprovement parameter in call to CvPlayerTraits::GetUnimprovedFeatureYieldChange()");
	PRECONDITION(eYield < NUM_YIELD_TYPES,  "Invalid eYield parameter in call to CvPlayerTraits::GetUnimprovedFeatureYieldChange()");

	if(eFeature == NO_FEATURE)
	{
		return 0;
	}

	return m_ppaaiUnimprovedFeatureYieldChange[(int)eFeature][(int)eYield];
}

/// Do all new units get a specific promotion?
bool CvPlayerTraits::HasFreePromotionUnitCombat(const int promotionID, const int unitCombatID) const
{
	ASSERT((promotionID >= 0), "promotionID is less than zero");
	for(size_t iI = 0; iI < m_vPotentiallyActiveLeaderTraits.size(); iI++)
	{
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(m_vPotentiallyActiveLeaderTraits[iI]);
		if(pkTraitInfo && HasTrait(m_vPotentiallyActiveLeaderTraits[iI]))
		{
			if(pkTraitInfo->IsFreePromotionUnitCombat(promotionID, unitCombatID))
			{
				return true;
			}
		}
	}

	return false;
}
/// Does this player have units that have a special upgrade path?
bool CvPlayerTraits::HasSpecialUnitUpgrade(const int unitClassID, const int unitID) const
{
	ASSERT((unitClassID >= 0), "unitClassID is less than zero");
	for (size_t iI = 0; iI < m_vPotentiallyActiveLeaderTraits.size(); iI++)
	{
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(m_vPotentiallyActiveLeaderTraits[iI]);
		if (pkTraitInfo && HasTrait(m_vPotentiallyActiveLeaderTraits[iI]))
		{
			if (pkTraitInfo->IsSpecialUpgradeUnitClass(unitClassID, unitID))
			{
				return true;
			}
		}
	}

	return false;
}
/// Do all new units of a certain class get a specific promotion?
bool CvPlayerTraits::HasFreePromotionUnitClass(const int promotionID, const int unitClassID) const
{
	ASSERT((promotionID >= 0), "promotionID is less than zero");
	for(size_t iI = 0; iI < m_vPotentiallyActiveLeaderTraits.size(); iI++)
	{
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(m_vPotentiallyActiveLeaderTraits[iI]);
		if(pkTraitInfo && HasTrait(m_vPotentiallyActiveLeaderTraits[iI]))
		{
			if(pkTraitInfo->IsFreePromotionUnitClass(promotionID, unitClassID))
			{
				return true;
			}
		}
	}

	return false;
}
/// Do all new units of a certain class get a specific promotion?
bool CvPlayerTraits::HasUnitClassCanBuild(const int buildID, const int unitClassID) const
{
	ASSERT((buildID >= 0), "buildID is less than zero");
	for(size_t iI = 0; iI < m_vPotentiallyActiveLeaderTraits.size(); iI++)
	{
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(m_vPotentiallyActiveLeaderTraits[iI]);
		if(pkTraitInfo && HasTrait(m_vPotentiallyActiveLeaderTraits[iI]))
		{
			if(pkTraitInfo->UnitClassCanBuild(buildID, unitClassID))
			{
				return true;
			}
		}
	}

	return false;
}
/// What is the golden age modifier for the specific yield type?
int CvPlayerTraits::GetGoldenAgeYieldModifier(YieldTypes eYield) const
{
	PRECONDITION(eYield < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eYield > -1, "Index out of bounds");

	std::map<int, int>::const_iterator it = m_aiGoldenAgeYieldModifier.find((int)eYield);
	if (it != m_aiGoldenAgeYieldModifier.end()) // find returns the iterator to map::end if the key is not present
	{
		return it->second;
	}

	return 0;
}
/// What is the production cost modifier for the unit combat type? And does it only work during golden ages?
std::pair<int, bool> CvPlayerTraits::GetUnitCombatProductionCostModifier(UnitCombatTypes eUnitCombat) const
{
	PRECONDITION(eUnitCombat >= 0, "unitCombatID expected to be >= 0");
	PRECONDITION(eUnitCombat < GC.getNumUnitCombatClassInfos(), "unitCombatID expected to be < GC.getNumUnitCombatInfos()");

	std::map<int, std::pair<int, bool>>::const_iterator it = m_aibUnitCombatProductionCostModifier.find((int)eUnitCombat);
	if (it != m_aibUnitCombatProductionCostModifier.end()) // find returns the iterator to map::end if the key is not present
	{
		return it->second;
	}

	return std::make_pair(0, false);
}
/// Is this build blocked for this civ?
bool CvPlayerTraits::IsNoBuild(BuildTypes eBuild) const
{
	PRECONDITION(eBuild >= 0, "buildID expected to be >= 0");
	PRECONDITION(eBuild < GC.getNumBuildInfos(), "buildID expected to be < GC.getNumBuildInfos()");

	std::vector<int>::const_iterator it = find(m_aiNoBuilds.begin(), m_aiNoBuilds.end(), (int)eBuild);

	return it != m_aiNoBuilds.end();
}
/// What is the production modifier for the domain type for each worked specialist?
int CvPlayerTraits::GetDomainProductionModifiersPerSpecialist(DomainTypes eDomain) const
{
	PRECONDITION(eDomain >= 0, "domainID expected to be >= 0");
	PRECONDITION(eDomain < NUM_DOMAIN_TYPES, "domainID expected to be < NUM_DOMAIN_TYPES");

	std::map<int, int>::const_iterator it = m_aiDomainProductionModifiersPerSpecialist.find((int)eDomain);
	if (it != m_aiDomainProductionModifiersPerSpecialist.end()) // find returns the iterator to map::end if the key is not present
	{
		return it->second;
	}

	return 0;
}

/// What is the percent if the origin city gains a percent of the target city's production towards that specific thing
TradeRouteProductionSiphon CvPlayerTraits::GetTradeRouteProductionSiphon(bool bInternationalOnly) const
{
	std::map<bool, TradeRouteProductionSiphon>::const_iterator it = m_aiiTradeRouteProductionSiphon.find(bInternationalOnly);
	if (it != m_aiiTradeRouteProductionSiphon.end()) // find returns the iterator to map::end if the key is not present
	{
		return it->second;
	}


	TradeRouteProductionSiphon sBlank;
	return sBlank;
}

bool CvPlayerTraits::IsTradeRouteProductionSiphon() const
{
	return !m_aiiTradeRouteProductionSiphon.empty();
}

/// What are the alternate technologies for revealing resources
AlternateResourceTechs CvPlayerTraits::GetAlternateResourceTechs(ResourceTypes eResource) const
{
	std::map<int, AlternateResourceTechs>::const_iterator it = m_aiiAlternateResourceTechs.find((int)eResource);
	if (it != m_aiiAlternateResourceTechs.end()) // find returns the iterator to map::end if the key is not present
	{
		return it->second;
	}

	AlternateResourceTechs sBlank;
	sBlank.m_eTechCityTrade = NO_TECH;
	sBlank.m_eTechReveal = NO_TECH;

	return sBlank;
}

/// Does the player's traits cause any resource to have a different reveal or city trade tech?
bool CvPlayerTraits::IsAlternateResourceTechs() const
{
	return !m_aiiAlternateResourceTechs.empty();
}

/// Does each city get a free building?
BuildingTypes CvPlayerTraits::GetFreeBuilding() const
{
	for(size_t iI = 0; iI < m_vPotentiallyActiveLeaderTraits.size(); iI++)
	{
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(m_vPotentiallyActiveLeaderTraits[iI]);
		if(pkTraitInfo && HasTrait(m_vPotentiallyActiveLeaderTraits[iI]))
		{
			if(pkTraitInfo->GetFreeBuilding()!=NO_BUILDING)
			{
				return pkTraitInfo->GetFreeBuilding();
			}
		}
	}

	return NO_BUILDING;
}
/// Does the capital get a free building?
BuildingTypes CvPlayerTraits::GetFreeCapitalBuilding() const
{
	for(size_t iI = 0; iI < m_vPotentiallyActiveLeaderTraits.size(); iI++)
	{
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(m_vPotentiallyActiveLeaderTraits[iI]);
		if(pkTraitInfo && HasTrait(m_vPotentiallyActiveLeaderTraits[iI]))
		{
			if(pkTraitInfo->GetFreeCapitalBuilding()!=NO_BUILDING)
			{
				return pkTraitInfo->GetFreeCapitalBuilding();
			}
		}
	}

	return NO_BUILDING;
}
UnitTypes CvPlayerTraits::GetFreeUnitOnConquest() const
{
	for(size_t iI = 0; iI < m_vPotentiallyActiveLeaderTraits.size(); iI++)
	{
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(m_vPotentiallyActiveLeaderTraits[iI]);
		if(pkTraitInfo && HasTrait(m_vPotentiallyActiveLeaderTraits[iI]))
		{
			if(pkTraitInfo->GetFreeUnitOnConquest() != NO_UNIT)
			{
				return pkTraitInfo->GetFreeUnitOnConquest();
			}
		}
	}

	return NO_UNIT;
}

/// Does each conquered city get a free building?
BuildingTypes CvPlayerTraits::GetFreeBuildingOnConquest() const
{
	for(size_t iI = 0; iI < m_vPotentiallyActiveLeaderTraits.size(); iI++)
	{
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(m_vPotentiallyActiveLeaderTraits[iI]);
		if(pkTraitInfo && HasTrait(m_vPotentiallyActiveLeaderTraits[iI]))
		{
			if(pkTraitInfo->GetFreeBuildingOnConquest()!=NO_BUILDING)
			{
				return pkTraitInfo->GetFreeBuildingOnConquest();
			}
		}
	}

	return NO_BUILDING;
}

/// Should unique luxuries appear around this tile?
bool CvPlayerTraits::AddUniqueLuxuriesAround(CvCity *pCity, int iNumResourceToGive)
{
	// Still have more of these cities to award?
	bool bResult = false;
	if (m_iUniqueLuxuryCities <= m_iUniqueLuxuryCitiesPlaced)
		return bResult;

	// Find our unique resources
	vector<ResourceTypes> vPossibleResources;
	for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		ResourceTypes eResource = (ResourceTypes)iResourceLoop;
		CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
		if (pkResource != NULL && pkResource->GetRequiredCivilization() == m_pPlayer->getCivilizationType())
		{
			vPossibleResources.push_back(eResource);

			//if this is one we haven't got so far, boost the chance
			if (m_pPlayer->getNumResourceTotal(eResource, false) == 0)
				vPossibleResources.push_back(eResource);
		}
	}

	if (vPossibleResources.empty())
		return false;

	//choose one
	uint uChoice = GC.getGame().urandLimitExclusive(vPossibleResources.size(), CvSeeder(pCity->plot()->GetPseudoRandomSeed()).mix(GET_PLAYER(pCity->getOwner()).GetPseudoRandomSeed()).mix(GC.getGame().GetCultureMedian()));
	ResourceTypes eResourceToGive = vPossibleResources[uChoice];
		
	//first round. place on owned non-city, non-resource plots without improvement
	int iNumResourceGiven = 0;
	CvPlot* pLoopPlot = NULL;
	for(int iCityPlotLoop = 0; iCityPlotLoop < pCity->GetNumWorkablePlots(); iCityPlotLoop++)
	{
		pLoopPlot = iterateRingPlots(pCity->getX(), pCity->getY(), iCityPlotLoop);
		if (pLoopPlot != NULL && pLoopPlot->getOwner() == m_pPlayer->GetID() && pLoopPlot->CanSpawnResource(pCity->getOwner())
			&& pLoopPlot->getFeatureType() == NO_FEATURE && pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
		{
			pLoopPlot->setResourceType(eResourceToGive, 1, false);
			iNumResourceGiven++;

			if (iNumResourceGiven >= iNumResourceToGive)
			{
				bResult = true;
				break;
			}
		}
	}

	//second round. non-owned plots, remove improvement if necessary
	if(iNumResourceGiven < iNumResourceToGive)
	{
		for(int iCityPlotLoop = 0; iCityPlotLoop < pCity->GetNumWorkablePlots(); iCityPlotLoop++)
		{
			pLoopPlot = iterateRingPlots(pCity->getX(), pCity->getY(), iCityPlotLoop);
			if (pLoopPlot != NULL && pLoopPlot->getOwner() == NO_PLAYER && pLoopPlot->CanSpawnResource(pCity->getOwner()))
			{
				if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
					pLoopPlot->setImprovementType(NO_IMPROVEMENT);

				pLoopPlot->setResourceType(eResourceToGive, 1, false);
				iNumResourceGiven++;

				if(iNumResourceGiven >= iNumResourceToGive)
				{
					bResult = true;
					break;
				}
			}
		}
	}

	//third round, city center plot
	if(iNumResourceGiven < iNumResourceToGive)
	{
		ResourceTypes eCurrentResource = pCity->plot()->getResourceType();
		if(eCurrentResource == NO_RESOURCE)
		{
			pCity->plot()->setResourceType(eResourceToGive, 1, false);
			bResult = true;
		}
	}

	if (bResult)
		m_iUniqueLuxuryCitiesPlaced++;   // One less to give out

	return bResult;
}

void CvPlayerTraits::SpawnBestUnitsOnImprovementDOW(CvCity *pCity)
{
	// No supply? Forget about it
	if (m_pPlayer->GetNumUnitsToSupply() >= m_pPlayer->GetNumUnitsSupplied())
		return;

	UnitTypes eUnit = m_pPlayer->GetCompetitiveSpawnUnitType(false, false, true, true, pCity);
	if (eUnit == NO_UNIT)
		return;

	std::set<int> siPlots = pCity->GetPlotList();
	for (std::set<int>::const_iterator it = siPlots.begin(); it != siPlots.end(); ++it)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(*it);
		// It's possible that the player runs out of supply mid loop
		if (m_pPlayer->GetNumUnitsToSupply() >= m_pPlayer->GetNumUnitsSupplied())
			return;

		ImprovementTypes eImprovement = pLoopPlot->getImprovementType();
		if (eImprovement == NO_IMPROVEMENT)
			continue;

		if (eImprovement == GetBestSpawnUnitImprovement())
		{
			CvUnit* pkUnit = m_pPlayer->initUnit(eUnit, pLoopPlot->getX(), pLoopPlot->getY());
			pCity->addProductionExperience(pkUnit);
			if (!pkUnit->jumpToNearestValidPlot())
				pkUnit->kill(false);
		}
	}
}

/// Should unique luxuries appear beneath this tile?
void CvPlayerTraits::AddUniqueLuxuries(CvCity *pCity)
{
	// Still have more of these cities to award?
	if (m_iUniqueLuxuryCities > m_iUniqueLuxuryCitiesPlaced)
	{
		int iArea = pCity->plot()->getArea();

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

			if (MOD_EVENTS_AREA_RESOURCES)
			{
				if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_AreaCanHaveAnyResource, m_pPlayer->GetID(), iArea) == GAMEEVENTRETURN_FALSE)
					return;
			}
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
			if (MOD_EVENTS_AREA_RESOURCES)
			{
				GAMEEVENTINVOKE_HOOK(GAMEEVENT_PlaceResource, m_pPlayer->GetID(), eResourceToGive, m_iUniqueLuxuryQuantity, pCity->getX(), pCity->getY());
			}
			else 
			{
				pCity->plot()->setResourceType(NO_RESOURCE, 0, true);
				pCity->plot()->setResourceType(eResourceToGive, m_iUniqueLuxuryQuantity, true);
			}
		}
	}
}

/// Does a unit entering this tile cause a barbarian to convert to the player?
bool CvPlayerTraits::CheckForBarbarianConversion(CvUnit* pByUnit, CvPlot* pPlot)
{
	// Loop through all adjacent plots
	CvPlot* pAdjacentPlot = NULL;
	int iI = 0;
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
					CvUnit* pNavalUnit = pAdjacentPlot->getBestDefender(BARBARIAN_PLAYER);
					if(pNavalUnit)
					{
						if (ConvertBarbarianNavalUnit(pByUnit, pNavalUnit))
						{
							bRtnValue = true;
						}
					}
				}
			}
		}
	}

	else if(GetLandBarbarianConversionPercent() > 0 && pPlot->getImprovementType() == GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT) &&
	        m_eCampGuardType != NO_UNIT)
	{
		bRtnValue = ConvertBarbarianCamp(pByUnit, pPlot);
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

int CvPlayerTraits::GetWonderProductionToBuildingDiscount(BuildingTypes eBuilding)
{
	CvBuildingEntry* thisBuildingEntry = GC.getBuildingInfo(eBuilding);
	if (thisBuildingEntry)
	{
		const CvBuildingClassInfo& kBuildingClassInfo = thisBuildingEntry->GetBuildingClassInfo();
		if (GetWonderProductionModifierToBuilding() > 0)
		{
			if (!(::isWorldWonderClass(kBuildingClassInfo) || ::isTeamWonderClass(kBuildingClassInfo) || ::isNationalWonderClass(kBuildingClassInfo)))
			{
				return (GetWonderProductionModifierToBuilding());
			}
		}
	}
	return 0;
}
TechTypes CvPlayerTraits::GetFreeBuildingPrereqTech() const
{
	for(size_t iI = 0; iI < m_vPotentiallyActiveLeaderTraits.size(); iI++)
	{
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(m_vPotentiallyActiveLeaderTraits[iI]);
		if(pkTraitInfo && HasTrait(m_vPotentiallyActiveLeaderTraits[iI]) && pkTraitInfo->GetFreeBuildingPrereqTech())
			return pkTraitInfo->GetFreeBuildingPrereqTech();
	}

	return NO_TECH;
}

TechTypes CvPlayerTraits::GetCapitalFreeBuildingPrereqTech() const
{
	for(size_t iI = 0; iI < m_vPotentiallyActiveLeaderTraits.size(); iI++)
	{
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(m_vPotentiallyActiveLeaderTraits[iI]);
		if(pkTraitInfo && HasTrait(m_vPotentiallyActiveLeaderTraits[iI]) && pkTraitInfo->GetCapitalFreeBuildingPrereqTech())
			return pkTraitInfo->GetCapitalFreeBuildingPrereqTech();
	}

	return NO_TECH;
}

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
bool CvPlayerTraits::IsAbleToCrossMountainsWithGreatGeneral() const
{
	return (m_bCrossesMountainsAfterGreatGeneral && m_pPlayer->getGreatGeneralsCreated(false) > 0);
}

/// Is this civ currently able to cross ice with combat units?
bool CvPlayerTraits::IsAbleToCrossIce() const
{
	return m_bCrossesIce;
}

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
bool CvPlayerTraits::TerrainClaimBoost(TerrainTypes eTerrain)
{
	if (eTerrain != NO_TERRAIN)
	{
		return m_abTerrainClaimBoost[eTerrain];
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
	for(size_t iI = 0; iI < m_vPotentiallyActiveLeaderTraits.size(); iI++)
	{
		CvTraitEntry* pkTraitInfo = GC.getTraitInfo(m_vPotentiallyActiveLeaderTraits[iI]);
		if(pkTraitInfo && HasTrait(m_vPotentiallyActiveLeaderTraits[iI]) && pkTraitInfo->IsMayaCalendarBonuses())
			return true;
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
		m_pPlayer->doInstantYield(INSTANT_YIELD_TYPE_BAKTUN_END);

		// Since m_iBaktunPreviousTurn will be overwritten in a moment, this is the only place to properly send an event for end of Maya long count
		if (MOD_EVENTS_GOLDEN_AGE && m_pPlayer != NULL)
			// GameEvents.PlayerEndOfMayaLongCount.Add(function(iPlayer, iBaktun, iBaktunPreviousTurn) end)
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_PlayerEndOfMayaLongCount, m_pPlayer->GetID(), m_iBaktun, m_iBaktunPreviousTurn);
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
	bool bHasReligion = false;
	if (MOD_BALANCE_ALTERNATE_MAYA_TRAIT && m_pPlayer->GetReligions()->GetOwnedReligion() != NO_RELIGION)
	{
		bHasReligion = true;
	}

	ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_PROPHET", true);

	if (GetUnitBaktun(ePossibleGreatPerson) == 0)
	{
		CvGameReligions* pReligions = GC.getGame().GetGameReligions();
		ReligionTypes eReligion = GET_PLAYER(m_pPlayer->GetID()).GetReligions()->GetOwnedReligion();

		// Have a religion that isn't enhanced yet?
		if (eReligion != NO_RELIGION)
		{
			const CvReligion* pMyReligion = pReligions->GetReligion(eReligion, m_pPlayer->GetID());
			if (!pMyReligion->m_bEnhanced)
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
		}

		// Don't have a religion and they can still be founded?
		else
		{
			if (pReligions->GetNumReligionsStillToFound() > 0 || IsAlwaysReligion())
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
		}

		if (MOD_BALANCE_ALTERNATE_MAYA_TRAIT && eDesiredGreatPerson == ePossibleGreatPerson && !bHasReligion)
		{
			eDesiredGreatPerson = NO_UNIT;
		}
	}

	// Highly wonder competitive and still early in game?
	if(eDesiredGreatPerson == NO_UNIT)
	{
		ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_ENGINEER");

		if(GetUnitBaktun(ePossibleGreatPerson) == 0)
		{
			if(m_pPlayer->GetDiplomacyAI()->GetWonderCompetitiveness() >= 8 && GC.getGame().getGameTurn() <= (GC.getGame().getEstimateEndTurn() / 2))
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
		}
	}

	// Does our grand strategy match one that is available?
	if (eDesiredGreatPerson == NO_UNIT)
	{
		ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_SCIENTIST");

		if (GetUnitBaktun(ePossibleGreatPerson) == 0)
		{
			if (m_pPlayer->GetDiplomacyAI()->IsGoingForSpaceshipVictory())
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
		}
	}
	if(eDesiredGreatPerson == NO_UNIT)
	{
		ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_ARTIST");

		if(GetUnitBaktun(ePossibleGreatPerson) == 0)
		{
			if(m_pPlayer->GetDiplomacyAI()->IsGoingForCultureVictory())
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
		}
	}
	if(eDesiredGreatPerson == NO_UNIT)
	{
		if (MOD_BALANCE_VP)
		{
			ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_GREAT_DIPLOMAT");
		}
		else
		{
			ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_MERCHANT");
		}

		if (GetUnitBaktun(ePossibleGreatPerson) == 0 && m_pPlayer->GetDiplomacyAI()->IsGoingForDiploVictory())
		{
			eDesiredGreatPerson = ePossibleGreatPerson;
		}
	}
	if(eDesiredGreatPerson == NO_UNIT)
	{
		ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_GREAT_GENERAL");

		if(GetUnitBaktun(ePossibleGreatPerson) == 0)
		{
			if(m_pPlayer->GetDiplomacyAI()->IsGoingForWorldConquest())
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
		}
	}
	if (MOD_BALANCE_VP)
	{
		ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_GREAT_DIPLOMAT");

		if (GetUnitBaktun(ePossibleGreatPerson) == 0 && m_pPlayer->GetDiplomacyAI()->IsGoingForDiploVictory())
		{
			eDesiredGreatPerson = ePossibleGreatPerson;
		}
	}

	// No obvious strategic choice, just go for first one available in a reasonable order
	if (eDesiredGreatPerson == NO_UNIT)
	{
		ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_PROPHET", true);

		if (GetUnitBaktun(ePossibleGreatPerson) == 0)
		{
			eDesiredGreatPerson = ePossibleGreatPerson;
			if (MOD_BALANCE_ALTERNATE_MAYA_TRAIT && eDesiredGreatPerson == ePossibleGreatPerson && !bHasReligion)
			{
				eDesiredGreatPerson = NO_UNIT;
			}
		}

		if(eDesiredGreatPerson == NO_UNIT)
		{
			ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_ENGINEER");

			if(GetUnitBaktun(ePossibleGreatPerson) == 0)
			{
				eDesiredGreatPerson = ePossibleGreatPerson;
			}
			else
			{
				ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_WRITER");

				if(GetUnitBaktun(ePossibleGreatPerson) == 0)
				{
					eDesiredGreatPerson = ePossibleGreatPerson;
				}
				else
				{
					ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_MERCHANT");

					if(GetUnitBaktun(ePossibleGreatPerson) == 0)
					{
						eDesiredGreatPerson = ePossibleGreatPerson;
					}
					else
					{
						ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_SCIENTIST");

						if(GetUnitBaktun(ePossibleGreatPerson) == 0)
						{
							eDesiredGreatPerson = ePossibleGreatPerson;
						}
						else
						{
							ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_ARTIST");

							if(GetUnitBaktun(ePossibleGreatPerson) == 0)
							{
								eDesiredGreatPerson = ePossibleGreatPerson;
							}
							else
							{
								ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_MUSICIAN");

								if(GetUnitBaktun(ePossibleGreatPerson) == 0)
								{
									eDesiredGreatPerson = ePossibleGreatPerson;
								}
								else
								{
									ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_GREAT_GENERAL");

									if(GetUnitBaktun(ePossibleGreatPerson) == 0)
									{
										eDesiredGreatPerson = ePossibleGreatPerson;
									}
									else
									{
										ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_GREAT_ADMIRAL");

										if (GetUnitBaktun(ePossibleGreatPerson) == 0)
										{
											eDesiredGreatPerson = ePossibleGreatPerson;
										}
										else if (MOD_BALANCE_VP)
										{
											ePossibleGreatPerson = m_pPlayer->GetSpecificUnitType("UNITCLASS_GREAT_DIPLOMAT");

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
	}

	// Actually get the great person
	if(eDesiredGreatPerson != NO_UNIT)
	{
		CvCity* pCity = m_pPlayer->GetGreatPersonSpawnCity(eDesiredGreatPerson);
		if(pCity)
		{
			pCity->GetCityCitizens()->DoSpawnGreatPerson(eDesiredGreatPerson, true, false, MOD_GLOBAL_TRULY_FREE_GP);
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

int CvPlayerTraits::GetCurrentBaktun() const
{
	ASSERT(IsUsingMayaCalendar());
	return m_iBaktun;
}

/// At the end of which calendar cycle was this unit chosen as a special bonus (0 if none)?
int CvPlayerTraits::GetUnitBaktun(UnitTypes eUnit) const
{
	std::vector<MayaBonusChoice>::const_iterator it;

	// Loop through all units available to tactical AI this turn
	for (it = m_aMayaBonusChoices.begin(); it != m_aMayaBonusChoices.end(); it++)
	{
		if(it->m_eUnitType == eUnit)
		{
			CvUnitEntry* pUnitEntry = GC.getUnitInfo(eUnit);
			if (MOD_BALANCE_ALTERNATE_MAYA_TRAIT && m_aMayaBonusChoices.size() > 1 && pUnitEntry->IsFoundReligion() && !IsProphetValid())
			{
				return 1;
			}
			else
			{
				return it->m_iBaktunJustFinished;
			}
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

	// Remember each choice as an accomplishment.
	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	UnitAITypes eUnitAI = pkUnitInfo->GetDefaultUnitAIType();
	switch (eUnitAI)
	{
	case UNITAI_GENERAL:
		m_pPlayer->CompleteAccomplishment(ACCOMPLISHMENT_LONGCOUNT_GENERAL);
		break;
	case UNITAI_ADMIRAL:
		m_pPlayer->CompleteAccomplishment(ACCOMPLISHMENT_LONGCOUNT_ADMIRAL);
		break;
	case UNITAI_PROPHET:
		m_pPlayer->CompleteAccomplishment(ACCOMPLISHMENT_LONGCOUNT_PROPHET);
		break;
	case UNITAI_WRITER:
		m_pPlayer->CompleteAccomplishment(ACCOMPLISHMENT_LONGCOUNT_WRITER);
		break;
	case UNITAI_ARTIST:
		m_pPlayer->CompleteAccomplishment(ACCOMPLISHMENT_LONGCOUNT_ARTIST);
		break;
	case UNITAI_MUSICIAN:
		m_pPlayer->CompleteAccomplishment(ACCOMPLISHMENT_LONGCOUNT_MUSICIAN);
		break;
	case UNITAI_ENGINEER:
		m_pPlayer->CompleteAccomplishment(ACCOMPLISHMENT_LONGCOUNT_ENGINEER);
		break;
	case UNITAI_MERCHANT:
		m_pPlayer->CompleteAccomplishment(ACCOMPLISHMENT_LONGCOUNT_MERCHANT);
		break;
	case UNITAI_SCIENTIST:
		m_pPlayer->CompleteAccomplishment(ACCOMPLISHMENT_LONGCOUNT_SCIENTIST);
		break;
	case UNITAI_DIPLOMAT:
		m_pPlayer->CompleteAccomplishment(ACCOMPLISHMENT_LONGCOUNT_DIPLOMAT);
		break;
	default:
		break;
	}
}

/// Have Maya unlocked free choice of Great People?
bool CvPlayerTraits::IsProphetValid() const
{
	// Has state religion? Valid.
	if (m_pPlayer->GetReligions()->GetStateReligion(false) != NO_RELIGION)
	{
		return true;
	}
	// Getting into the upper baktuns? Let's let it happen.
	if (m_iBaktun > 8)
	{
		return true;
	}
	// Post-medieval? True.
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);
	return m_pPlayer->GetCurrentEra() > eMedieval;
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
		const UnitTypes eUnit = m_pPlayer->GetSpecificUnitType(eUnitClass);
		if (eUnit != NO_UNIT)
		{
			CvUnitEntry* pUnitEntry = GC.getUnitInfo(eUnit);
			if (pUnitEntry)
			{
				if (pUnitEntry->GetSpecialUnitType() == eSpecialUnitGreatPerson)
				{
					if (MOD_BALANCE_ALTERNATE_MAYA_TRAIT && pUnitEntry->IsFoundReligion() && !IsProphetValid())
						continue;

					iNumGreatPeopleTypes++;
				}
			}
		}
	}

	return ((int)m_aMayaBonusChoices.size() >= iNumGreatPeopleTypes);
}

// SERIALIZATION METHODS

// FreeResourceXCities
template<typename FreeResourceXCitiesT, typename Visitor>
void FreeResourceXCities::Serialize(FreeResourceXCitiesT& freeResourceXCities, Visitor& visitor)
{
	visitor(freeResourceXCities.m_iNumCities);
	visitor(freeResourceXCities.m_iResourceQuantity);
}
FDataStream& operator<<(FDataStream& saveTo, const FreeResourceXCities& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	FreeResourceXCities::Serialize(readFrom, serialVisitor);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, FreeResourceXCities& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	FreeResourceXCities::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

// MayaBonusChoice
template<typename MayaBonusChoiceT, typename Visitor>
void MayaBonusChoice::Serialize(MayaBonusChoiceT& mayaBonusChoice, Visitor& visitor)
{
	visitor(mayaBonusChoice.m_eUnitType);
	visitor(mayaBonusChoice.m_iBaktunJustFinished);
}
FDataStream& operator<<(FDataStream& saveTo, const MayaBonusChoice& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	MayaBonusChoice::Serialize(readFrom, serialVisitor);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, MayaBonusChoice& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	MayaBonusChoice::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

// FreeTraitUnit
template<typename FreeTraitUnitT, typename Visitor>
void FreeTraitUnit::Serialize(FreeTraitUnitT& freeTraitUnit, Visitor& visitor)
{
	visitor(freeTraitUnit.m_iFreeUnit);
	visitor(freeTraitUnit.m_ePrereqTech);
}
FDataStream& operator<<(FDataStream& saveTo, const FreeTraitUnit& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	FreeTraitUnit::Serialize(readFrom, serialVisitor);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, FreeTraitUnit& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	FreeTraitUnit::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

// TradeRouteProductionSiphon
template<typename TradeRouteProductionSiphonT, typename Visitor>
void TradeRouteProductionSiphon::Serialize(TradeRouteProductionSiphonT& tradeRouteProductionSiphon, Visitor& visitor)
{
	visitor(tradeRouteProductionSiphon.m_iSiphonPercent);
	visitor(tradeRouteProductionSiphon.m_iPercentIncreaseWithOpenBorders);
}
FDataStream& operator<<(FDataStream& saveTo, const TradeRouteProductionSiphon& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	TradeRouteProductionSiphon::Serialize(readFrom, serialVisitor);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, TradeRouteProductionSiphon& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	TradeRouteProductionSiphon::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

// AlternateResourceTechs
template<typename AlternateResourceTechsT, typename Visitor>
void AlternateResourceTechs::Serialize(AlternateResourceTechsT& alternateResourceTechs, Visitor& visitor)
{
	visitor(alternateResourceTechs.m_eTechReveal);
	visitor(alternateResourceTechs.m_eTechCityTrade);
}
FDataStream& operator<<(FDataStream& saveTo, const AlternateResourceTechs& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	AlternateResourceTechs::Serialize(readFrom, serialVisitor);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, AlternateResourceTechs& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	AlternateResourceTechs::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

// --

template<typename PlayerTraits, typename Visitor>
void CvPlayerTraits::Serialize(PlayerTraits& playerTraits, Visitor& visitor)
{
	visitor(playerTraits.m_bIsWarmonger);
	visitor(playerTraits.m_bIsNerd);
	visitor(playerTraits.m_bIsTourism);
	visitor(playerTraits.m_bIsDiplomat);
	visitor(playerTraits.m_bIsExpansionist);
	visitor(playerTraits.m_bIsSmaller);
	visitor(playerTraits.m_bIsReligious);
	visitor(playerTraits.m_iGreatPeopleRateModifier);
	visitor(playerTraits.m_iGreatScientistRateModifier);
	visitor(playerTraits.m_iGreatGeneralRateModifier);
	visitor(playerTraits.m_iGreatGeneralExtraBonus);
	visitor(playerTraits.m_iGreatPersonGiftInfluence);
	visitor(playerTraits.m_iLevelExperienceModifier);
	visitor(playerTraits.m_iMaxGlobalBuildingProductionModifier);
	visitor(playerTraits.m_iMaxTeamBuildingProductionModifier);
	visitor(playerTraits.m_iMaxPlayerBuildingProductionModifier);
	visitor(playerTraits.m_iCityUnhappinessModifier);
	visitor(playerTraits.m_iPopulationUnhappinessModifier);
	visitor(playerTraits.m_iCityStateBonusModifier);
	visitor(playerTraits.m_iCityStateFriendshipModifier);
	visitor(playerTraits.m_iCityStateCombatModifier);
	visitor(playerTraits.m_iLandBarbarianConversionPercent);
	visitor(playerTraits.m_iLandBarbarianConversionExtraUnits);
	visitor(playerTraits.m_iSeaBarbarianConversionPercent);
	visitor(playerTraits.m_iCapitalBuildingModifier);
	visitor(playerTraits.m_iPlotBuyCostModifier);
	visitor(playerTraits.m_iNationalPopReqModifier);
	visitor(playerTraits.m_iCityWorkingChange);
	visitor(playerTraits.m_iCityAutomatonWorkersChange);
	visitor(playerTraits.m_iPlotCultureCostModifier);
	visitor(playerTraits.m_iCultureFromKills);
	visitor(playerTraits.m_iFaithFromKills);
	visitor(playerTraits.m_iCityCultureBonus);
	visitor(playerTraits.m_iCapitalThemingBonusModifier);
	visitor(playerTraits.m_iPolicyCostModifier);
	visitor(playerTraits.m_iCityConnectionTradeRouteChange);
	visitor(playerTraits.m_iWonderProductionModifier);
	visitor(playerTraits.m_iPlunderModifier);
	visitor(playerTraits.m_iImprovementMaintenanceModifier);
	visitor(playerTraits.m_iGoldenAgeDurationModifier);
	visitor(playerTraits.m_iGoldenAgeMoveChange);
	visitor(playerTraits.m_iGoldenAgeCombatModifier);
	visitor(playerTraits.m_iGoldenAgeTourismModifier);
	visitor(playerTraits.m_iExtraEmbarkMoves);
	visitor(playerTraits.m_iNaturalWonderFirstFinderGold);
	visitor(playerTraits.m_iNaturalWonderSubsequentFinderGold);
	visitor(playerTraits.m_iNaturalWonderYieldModifier);
	visitor(playerTraits.m_iNaturalWonderHappinessModifier);
	visitor(playerTraits.m_iNearbyImprovementCombatBonus);
	visitor(playerTraits.m_iNearbyImprovementBonusRange);
	visitor(playerTraits.m_iCultureBuildingYieldChange);
	visitor(playerTraits.m_iWarWearinessModifier);
	visitor(playerTraits.m_iEnemyWarWearinessModifier);
	visitor(playerTraits.m_iCombatBonusVsHigherPop);
	visitor(playerTraits.m_bBuyOwnedTiles);
	visitor(playerTraits.m_bNewCitiesStartWithCapitalReligion);
	visitor(playerTraits.m_bNoSpread);
	visitor(playerTraits.m_iXPBonusFromGreatPersonBirth);
	visitor(playerTraits.m_iUnitHealFromGreatPersonBirth);
	visitor(playerTraits.m_iBullyMilitaryStrengthModifier);
	visitor(playerTraits.m_iBullyValueModifier);
	visitor(playerTraits.m_bIgnoreBullyPenalties);
	visitor(playerTraits.m_bDiplomaticMarriage);
	visitor(playerTraits.m_bAdoptionFreeTech);
	visitor(playerTraits.m_bGPWLTKD);
	visitor(playerTraits.m_bGreatWorkWLTKD);
	visitor(playerTraits.m_bExpansionWLTKD);
	visitor(playerTraits.m_bTradeRouteOnly);
	visitor(playerTraits.m_bKeepConqueredBuildings);
	visitor(playerTraits.m_bMountainPass);
	visitor(playerTraits.m_bUniqueBeliefsOnly);
	visitor(playerTraits.m_bNoNaturalReligionSpread);
	visitor(playerTraits.m_bNoOpenTrade);
	visitor(playerTraits.m_bGoldenAgeOnWar);
	visitor(playerTraits.m_bUnableToCancelRazing);
	visitor(playerTraits.m_iWLTKDGPImprovementModifier);
	visitor(playerTraits.m_iGrowthBoon);
	visitor(playerTraits.m_iAllianceCSDefense);
	visitor(playerTraits.m_iAllianceCSStrength);
	visitor(playerTraits.m_iTourismGABonus);
	visitor(playerTraits.m_iTourismToGAP);
	visitor(playerTraits.m_iGoldToGAP);
	visitor(playerTraits.m_iInfluenceMeetCS);
	visitor(playerTraits.m_iMultipleAttackBonus);
	visitor(playerTraits.m_iCityConquestGWAM);
	visitor(playerTraits.m_iEventTourismBoost);
	visitor(playerTraits.m_iEventGP);
	visitor(playerTraits.m_iWLTKDCulture);
	visitor(playerTraits.m_iWLTKDGATimer);
	visitor(playerTraits.m_iWLTKDUnhappinessNeedsMod);
	visitor(playerTraits.m_iStartingSpies);
	visitor(playerTraits.m_iSpyOffensiveStrengthModifier);
	visitor(playerTraits.m_iSpyMoveRateBonus);
	visitor(playerTraits.m_iSpySecurityModifier);
	visitor(playerTraits.m_iSpyExtraRankBonus);
	visitor(playerTraits.m_iQuestYieldModifier);
	visitor(playerTraits.m_iWonderProductionModifierToBuilding);
	visitor(playerTraits.m_iPolicyGEorGM);
	visitor(playerTraits.m_iGAGarrisonCityRangeStrikeModifier);
	visitor(playerTraits.m_bBestUnitSpawnOnImpDOW);
	visitor(playerTraits.m_iBestUnitImprovement);
	visitor(playerTraits.m_iGGGARateFromDenunciationsAndWars);
	visitor(playerTraits.m_bTradeRouteMinorInfluenceAP);
	visitor(playerTraits.m_bProdModFromNumSpecialists);
	visitor(playerTraits.m_iConquestOfTheWorldCityAttack);
	visitor(playerTraits.m_bConquestOfTheWorld);
	visitor(playerTraits.m_bFreeUpgrade);
	visitor(playerTraits.m_bWarsawPact);
	visitor(playerTraits.m_iEnemyWarSawPactPromotion);
	visitor(playerTraits.m_bFreeZuluPikemanToImpi);
	visitor(playerTraits.m_bPermanentYieldsDecreaseEveryEra);
	visitor(playerTraits.m_bImportsCountTowardsMonopolies);
	visitor(playerTraits.m_bCanPurchaseNavalUnitsFaith);
	visitor(playerTraits.m_iPuppetPenaltyReduction);
	visitor(playerTraits.m_iSharedReligionTourismModifier);
	visitor(playerTraits.m_iExtraMissionaryStrength);
	visitor(playerTraits.m_bCanGoldInternalTradeRoutes);
	visitor(playerTraits.m_bAnnexedCityStatesGiveYields);
	visitor(playerTraits.m_iExtraTradeRoutesPerXOwnedCities);
	visitor(playerTraits.m_iExtraTradeRoutesPerXOwnedVassals);
	visitor(playerTraits.m_iMinorInfluencePerGiftedUnit);
	visitor(playerTraits.m_bIsCapitalOnly);
	visitor(playerTraits.m_iInvestmentModifier);
	visitor(playerTraits.m_iCombatBonusVsHigherTech);
	visitor(playerTraits.m_iCombatBonusVsLargerCiv);
	visitor(playerTraits.m_iLandUnitMaintenanceModifier);
	visitor(playerTraits.m_iNavalUnitMaintenanceModifier);
	visitor(playerTraits.m_iRazeSpeedModifier);
	visitor(playerTraits.m_iDOFGreatPersonModifier);
	visitor(playerTraits.m_iLuxuryHappinessRetention);
	visitor(playerTraits.m_iExtraSupply);
	visitor(playerTraits.m_iExtraSupplyPerCity);
	visitor(playerTraits.m_iExtraSupplyPerPopulation);
	visitor(playerTraits.m_iExtraSpies);
	visitor(playerTraits.m_iUnresearchedTechBonusFromKills);
	visitor(playerTraits.m_iExtraFoundedCityTerritoryClaimRange);
	visitor(playerTraits.m_iExtraConqueredCityTerritoryClaimRange);
	visitor(playerTraits.m_iExtraTenetsFirstAdoption);
	visitor(playerTraits.m_iMonopolyModFlat);
	visitor(playerTraits.m_iMonopolyModPercent);
	visitor(playerTraits.m_iFreeSocialPoliciesPerEra);
	visitor(playerTraits.m_iNumTradeRoutesModifier);
	visitor(playerTraits.m_iTradeRouteResourceModifier);
	visitor(playerTraits.m_iUniqueLuxuryCities);
	visitor(playerTraits.m_iUniqueLuxuryQuantity);
	visitor(playerTraits.m_iUniqueLuxuryCitiesPlaced);
	visitor(playerTraits.m_iWorkerSpeedModifier);
	visitor(playerTraits.m_iAfraidMinorPerTurnInfluence);
	visitor(playerTraits.m_iLandTradeRouteRangeBonus);
	visitor(playerTraits.m_iTradeReligionModifier);
	visitor(playerTraits.m_iTradeBuildingModifier);
	visitor(playerTraits.m_iSeaTradeRouteRangeBonus);
	visitor(playerTraits.m_iNumFreeBuildings);
	visitor(playerTraits.m_bBullyAnnex);
	visitor(playerTraits.m_iBullyYieldMultiplierAnnex);
	visitor(playerTraits.m_bFightWellDamaged);
	visitor(playerTraits.m_bWoodlandMovementBonus);
	visitor(playerTraits.m_bRiverMovementBonus);
	visitor(playerTraits.m_bFasterInHills);
	visitor(playerTraits.m_bEmbarkedAllWater);
	visitor(playerTraits.m_bEmbarkedToLandFlatCost);
	visitor(playerTraits.m_bNoHillsImprovementMaintenance);
	visitor(playerTraits.m_bTechBoostFromCapitalScienceBuildings);
	visitor(playerTraits.m_bArtistGoldenAgeTechBoost);
	visitor(playerTraits.m_bStaysAliveZeroCities);
	visitor(playerTraits.m_bFaithFromUnimprovedForest);
	visitor(playerTraits.m_bAnyBelief);
	visitor(playerTraits.m_bAlwaysReligion);
	visitor(playerTraits.m_bNoTradeRouteProximityPenalty);
	visitor(playerTraits.m_bCanPlunderWithoutWar);
	visitor(playerTraits.m_bBonusReligiousBelief);
	visitor(playerTraits.m_bAbleToAnnexCityStates);
	visitor(playerTraits.m_bCrossesMountainsAfterGreatGeneral);
	visitor(playerTraits.m_bCrossesIce);
	visitor(playerTraits.m_bGGFromBarbarians);
	visitor(playerTraits.m_bMayaCalendarBonuses);
	visitor(playerTraits.m_iBaktunPreviousTurn);
	visitor(playerTraits.m_aMayaBonusChoices);
	visitor(playerTraits.m_bNoAnnexing);
	visitor(playerTraits.m_bTechFromCityConquer);
	visitor(playerTraits.m_bUniqueLuxuryRequiresNewArea);
	visitor(playerTraits.m_bRiverTradeRoad);
	visitor(playerTraits.m_bAngerFreeIntrusionOfCityStates);
	visitor(playerTraits.m_iBasicNeedsMedianModifier);
	visitor(playerTraits.m_iGoldMedianModifier);
	visitor(playerTraits.m_iScienceMedianModifier);
	visitor(playerTraits.m_iCultureMedianModifier);
	visitor(playerTraits.m_iReligiousUnrestModifier);
	visitor(playerTraits.m_bNoConnectionUnhappiness);
	visitor(playerTraits.m_bIsNoReligiousStrife);
	visitor(playerTraits.m_bIsOddEraScaler);
	visitor(playerTraits.m_iWonderProductionModGA);
	visitor(playerTraits.m_iCultureBonusModifierConquest);
	visitor(playerTraits.m_iProductionBonusModifierConquest);
	visitor(playerTraits.m_eCampGuardType);
	visitor(playerTraits.m_eCombatBonusImprovement);
	visitor(playerTraits.m_iExtraYieldThreshold);
	visitor(playerTraits.m_iFreeCityYield);
	visitor(playerTraits.m_iYieldChangeStrategicResources);
	visitor(playerTraits.m_iYieldRateModifier);
	visitor(playerTraits.m_iYieldChangeNaturalWonder);
	visitor(playerTraits.m_iYieldChangePerTradePartner);
	visitor(playerTraits.m_iYieldChangeIncomingTradeRoute);
	visitor(playerTraits.m_iStrategicResourceQuantityModifier);
	visitor(playerTraits.m_aiResourceQuantityModifier);
	visitor(playerTraits.m_aiNumFreeResourceOnWorldWonderCompletion);
	visitor(playerTraits.m_abNoTrain);
	visitor(playerTraits.m_aFreeTraitUnits);
	visitor(playerTraits.m_aiGreatPersonCostReduction);
	visitor(playerTraits.m_aiPerPuppetGreatPersonRateModifier);
	visitor(playerTraits.m_aiGreatPersonGWAM);
	visitor(playerTraits.m_aiGoldenAgeGreatPersonRateModifier);
	visitor(playerTraits.m_aiGoldenAgeFromGreatPersonBirth);
	visitor(playerTraits.m_aiGreatPersonProgressFromPolicyUnlock);
	visitor(playerTraits.m_aiGreatPersonProgressFromKills);
	visitor(playerTraits.m_aiRandomGreatPersonProgressFromKills);
	visitor(playerTraits.m_aiNumPledgesDomainProdMod);
	visitor(playerTraits.m_aFreeResourceXCities);
	visitor(playerTraits.m_aiFreeUnitClassesDOW);
	visitor(playerTraits.m_aiDomainFreeExperienceModifier);
	visitor(playerTraits.m_ppiYieldFromTileEarnTerrainType);
	visitor(playerTraits.m_ppiYieldFromTilePurchaseTerrainType);
	visitor(playerTraits.m_ppiYieldFromTileConquest);
	visitor(playerTraits.m_ppiYieldFromTileCultureBomb);
	visitor(playerTraits.m_ppiYieldFromTileStealCultureBomb);
	visitor(playerTraits.m_ppiYieldFromTileSettle);
	visitor(playerTraits.m_ppaaiYieldChangePerImprovementBuilt);
	visitor(playerTraits.m_pbiYieldFromBarbarianCampClear);
	visitor(playerTraits.m_bHasYieldFromTileCultureBomb);
	visitor(playerTraits.m_bHasYieldFromTileEarn);
	visitor(playerTraits.m_bHasYieldFromTilePurchase);
	visitor(playerTraits.m_aiGoldenAgeYieldModifier);
	visitor(playerTraits.m_aibUnitCombatProductionCostModifier);
	visitor(playerTraits.m_iNonSpecialistFoodChange);
	visitor(playerTraits.m_aiNoBuilds);
	visitor(playerTraits.m_aiDomainProductionModifiersPerSpecialist);
	visitor(playerTraits.m_paiMovesChangeUnitClass);
	visitor(playerTraits.m_abTerrainClaimBoost);
	visitor(playerTraits.m_pbiYieldFromRouteMovementInForeignTerritory);
	visitor(playerTraits.m_aiiTradeRouteProductionSiphon);
	visitor(playerTraits.m_aiiAlternateResourceTechs);
	visitor(playerTraits.m_paiMovesChangeUnitCombat);
	visitor(playerTraits.m_paiMaintenanceModifierUnitCombat);
	visitor(playerTraits.m_ppaaiImprovementYieldChange);
	visitor(playerTraits.m_ppiPlotYieldChange);
	visitor(playerTraits.m_iYieldFromLevelUp);
	visitor(playerTraits.m_iYieldFromHistoricEvent);
	visitor(playerTraits.m_iYieldFromXMilitaryUnits);
	visitor(playerTraits.m_iYieldFromOwnPantheon);
	visitor(playerTraits.m_tradeRouteEndYieldDomestic);
	visitor(playerTraits.m_tradeRouteEndYieldInternational);
	visitor(playerTraits.m_iYieldFromRouteMovement);
	visitor(playerTraits.m_iYieldFromExport);
	visitor(playerTraits.m_iYieldFromImport);
	visitor(playerTraits.m_iYieldFromTilePurchase);
	visitor(playerTraits.m_iYieldFromTileEarn);
	visitor(playerTraits.m_iYieldFromSettle);
	visitor(playerTraits.m_iYieldFromConquest);
	visitor(playerTraits.m_iYieldFromCityDamageTimes100);
	visitor(playerTraits.m_iYieldFromCSAlly);
	visitor(playerTraits.m_iYieldFromCSFriend);
	visitor(playerTraits.m_iGAPToYield);
	visitor(playerTraits.m_iMountainRangeYield);
	visitor(playerTraits.m_iLuxuryYieldChanges);
	visitor(playerTraits.m_bFreeGreatWorkOnConquest);
	visitor(playerTraits.m_bPopulationBoostReligion);
	visitor(playerTraits.m_bStartsWithPantheon);
	visitor(playerTraits.m_bProphetFervor);
	visitor(playerTraits.m_bCombatBoostNearNaturalWonder);
	visitor(playerTraits.m_iPurchasedUnitsBonusXP);
	visitor(playerTraits.m_iVotePerXCSAlliance);
	visitor(playerTraits.m_iGoldenAgeFromVictory);
	visitor(playerTraits.m_iFreePolicyPerXTechs);
	visitor(playerTraits.m_eGPFaithPurchaseEra);
	visitor(playerTraits.m_iFaithCostModifier);
	visitor(playerTraits.m_iVotePerXCSFollowingFollowingYourReligion);
	visitor(playerTraits.m_iChanceToConvertReligiousUnits);
	visitor(playerTraits.m_ppiBuildingClassYieldChange);
	visitor(playerTraits.m_iCapitalYieldChanges);
	visitor(playerTraits.m_iCityYieldChanges);
	visitor(playerTraits.m_iPermanentYieldChangeWLTKD);
	visitor(playerTraits.m_iCoastalCityYieldChanges);
	visitor(playerTraits.m_iGreatWorkYieldChanges);
	visitor(playerTraits.m_iArtifactYieldChanges);
	visitor(playerTraits.m_iArtYieldChanges);
	visitor(playerTraits.m_iLitYieldChanges);
	visitor(playerTraits.m_iMusicYieldChanges);
	visitor(playerTraits.m_iSeaPlotYieldChanges);
	visitor(playerTraits.m_ppiFeatureYieldChange);
	visitor(playerTraits.m_ppiResourceYieldChange);
	visitor(playerTraits.m_miResourceYieldChangesFromGoldenAge);
	visitor(playerTraits.m_miResourceYieldChangesFromGoldenAgeCap);
	visitor(playerTraits.m_ppiTerrainYieldChange);
	visitor(playerTraits.m_iYieldFromKills);
	visitor(playerTraits.m_iYieldFromBarbarianKills);
	visitor(playerTraits.m_iYieldFromMinorDemand);
	visitor(playerTraits.m_iYieldFromLuxuryResourceGain);
	visitor(playerTraits.m_iYieldChangeTradeRoute);
	visitor(playerTraits.m_iYieldChangeWorldWonder);
	visitor(playerTraits.m_ppiTradeRouteYieldChange);
	visitor(playerTraits.m_ppaaiSpecialistYieldChange);
	visitor(playerTraits.m_ppiGreatPersonExpendedYield);
	visitor(playerTraits.m_ppiGreatPersonBornYield);
	visitor(playerTraits.m_ppiCityYieldFromUnimprovedFeature);
	visitor(playerTraits.m_ppaaiUnimprovedFeatureYieldChange);
	visitor(playerTraits.m_aUniqueLuxuryAreas);
}

/// Serialization read
void CvPlayerTraits::Read(FDataStream& kStream)
{
	// precompute the traits our leader has
	m_vPotentiallyActiveLeaderTraits.clear();
	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		m_vLeaderHasTrait[iI] = false;
		if (m_pPlayer && m_pPlayer->isMajorCiv() && m_pPlayer->isAlive() && m_pPlayer->getLeaderInfo().hasTrait((TraitTypes)iI))
		{
			m_vLeaderHasTrait[iI] = true;
			m_vPotentiallyActiveLeaderTraits.push_back((TraitTypes)iI);
		}
	}

	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);

	UpdateYieldChangeImprovementTypes();
}

/// Serialization write
void CvPlayerTraits::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& stream, CvPlayerTraits& playerTraits)
{
	playerTraits.Read(stream);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvPlayerTraits& playerTraits)
{
	playerTraits.Write(stream);
	return stream;
}

// PRIVATE METHODS

/// Is there an adjacent barbarian camp that could be converted?
bool CvPlayerTraits::ConvertBarbarianCamp(CvUnit* pByUnit, CvPlot* pPlot)
{
	CvUnit* pGiftUnit = NULL;

	// Has this camp already decided not to convert?
	if(pPlot->IsBarbarianCampNotConverting())
	{
		return false;
	}

	// Roll die to see if it converts
	if (GC.getGame().randRangeInclusive(1, 100, CvSeeder::fromRaw(0x960ab208).mix(pByUnit->GetID()).mix(pPlot->GetPseudoRandomSeed())) <= m_iLandBarbarianConversionPercent)
	{
		pPlot->setImprovementType(NO_IMPROVEMENT);

		int iNumGold = /*25*/ GD_INT_GET(GOLD_FROM_BARBARIAN_CONVERSION);
		m_pPlayer->GetTreasury()->ChangeGold(iNumGold);

		// Set who last cleared the camp here
		pPlot->SetPlayerThatClearedBarbCampHere(m_pPlayer->GetID());

		// Convert the barbarian into our unit
		PRECONDITION(m_eCampGuardType < GC.getNumUnitInfos(), "Illegal camp guard unit type");
		pGiftUnit = m_pPlayer->initUnit(m_eCampGuardType, pPlot->getX(), pPlot->getY(), NO_UNITAI, REASON_CONVERT, true /*bNoMove*/);
		if (!pGiftUnit->jumpToNearestValidPlot())
			pGiftUnit->kill(false);
		else
		{
			if (MOD_EVENTS_UNIT_CAPTURE)
			{
				GAMEEVENTINVOKE_HOOK(GAMEEVENT_UnitCaptured, m_pPlayer->GetID(), pByUnit->GetID(), m_pPlayer->GetID(), pGiftUnit->GetID(), false, 2);
			}
			pGiftUnit->finishMoves();
		}

		// Convert any extra units
		for(int iI = 0; iI < m_iLandBarbarianConversionExtraUnits; iI++)
		{
			pGiftUnit = m_pPlayer->initUnit(m_eCampGuardType, pPlot->getX(), pPlot->getY(), NO_UNITAI, REASON_CONVERT, true /*bNoMove*/);
			if (!pGiftUnit->jumpToNearestValidPlot())
				pGiftUnit->kill(false);
			else
			{
				if (MOD_EVENTS_UNIT_CAPTURE) {
					GAMEEVENTINVOKE_HOOK(GAMEEVENT_UnitCaptured, m_pPlayer->GetID(), pByUnit->GetID(), m_pPlayer->GetID(), pGiftUnit->GetID(), false, 2);
				}
				pGiftUnit->finishMoves();
			}
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

		//Increase Stat
		if (MOD_ENABLE_ACHIEVEMENTS && m_pPlayer->isHuman(ISHUMAN_ACHIEVEMENTS) &&!GC.getGame().isGameMultiPlayer())
		{
			gDLL->IncrementSteamStatAndUnlock(ESTEAMSTAT_BARBSCONVERTED, 10, ACHIEVEMENT_SPECIAL_BARBARIANWARLORD);
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
bool CvPlayerTraits::ConvertBarbarianNavalUnit(CvUnit* pByUnit, CvUnit* pUnit)
{
	CvUnit* pGiftUnit = NULL;

	// Has this unit already decided not to convert?
	if(pUnit->IsNotConverting())
	{
		return false;
	}

	// Roll die to see if it converts
	if (GC.getGame().randRangeInclusive(1, 100, CvSeeder::fromRaw(0x960ab208).mix(pByUnit->GetID()).mix(pUnit->GetID())) <= m_iSeaBarbarianConversionPercent)
	{
		int iNumGold = /*25*/ GD_INT_GET(GOLD_FROM_BARBARIAN_CONVERSION);
		m_pPlayer->GetTreasury()->ChangeGold(iNumGold);

		if (MOD_EVENTS_UNIT_CAPTURE) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_UnitCaptured, m_pPlayer->GetID(), pByUnit->GetID(), pUnit->getOwner(), pUnit->GetID(), false, 3);
		}

		// Convert the barbarian into our unit
		pGiftUnit = m_pPlayer->initUnit(pUnit->getUnitType(), pUnit->getX(), pUnit->getY(), pUnit->AI_getUnitAIType(), REASON_CONVERT, true /*bNoMove*/, false);
		ASSERT(pGiftUnit, "GiftUnit is not assigned a valid value");
		pGiftUnit->convert(pUnit, false);
		pGiftUnit->setupGraphical();
		pGiftUnit->finishMoves(); // No move first turn

		// Validate that the achievement is reached by a live human and active player at the same time
		if (MOD_ENABLE_ACHIEVEMENTS && m_pPlayer->isHuman(ISHUMAN_ACHIEVEMENTS) && !GC.getGame().isGameMultiPlayer() && m_pPlayer->getLeaderInfo().GetType() && _stricmp(m_pPlayer->getLeaderInfo().GetType(), "LEADER_SULEIMAN") == 0)
		{
			gDLL->IncrementSteamStatAndUnlock(ESTEAMSTAT_BARBSNAVALCONVERTED, 10, ACHIEVEMENT_SPECIAL_BARBARYPIRATE);
		}

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
