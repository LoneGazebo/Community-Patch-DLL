/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_TRAIT_CLASSES_H
#define CIV5_TRAIT_CLASSES_H

#include "CustomMods.h"

struct FreeResourceXCities
{
	FreeResourceXCities():
		m_iNumCities(0),
		m_iResourceQuantity(0)
	{};

	int m_iNumCities;
	int m_iResourceQuantity;
};

struct MayaBonusChoice
{
	MayaBonusChoice():
		m_eUnitType(NO_UNIT),
		m_iBaktunJustFinished(0)
	{};

	MayaBonusChoice(const MayaBonusChoice& in)
	{
		m_eUnitType = in.m_eUnitType;
		m_iBaktunJustFinished = in.m_iBaktunJustFinished;
	};

	UnitTypes m_eUnitType;
	int m_iBaktunJustFinished;
};

struct TradeRouteProductionSiphon
{
	TradeRouteProductionSiphon() :
		m_iSiphonPercent(0),
		m_iPercentIncreaseWithOpenBorders(0)
	{};

	bool IsHaveProductionSiphon()
	{
		if (m_iSiphonPercent != 0 || m_iPercentIncreaseWithOpenBorders != 0)
		{
			return true;
		}
		return false;
	}

	int m_iSiphonPercent;
	int m_iPercentIncreaseWithOpenBorders;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvTraitEntry
//!  \brief		A single entry in the trait XML file
//
//!  Key Attributes:
//!  - Used to be called CvTraitInfo
//!  - Populated from XML\Civilizations\CIV5Traits.xml
//!  - Array of these contained in CvTraitXMLEntries class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTraitEntry: public CvBaseInfo
{
public:
	CvTraitEntry(void);
	~CvTraitEntry(void);

	int GetLevelExperienceModifier() const;
	int GetGreatPeopleRateModifier() const;
	int GetGreatScientistRateModifier() const;
	int GetGreatGeneralRateModifier() const;
	int GetGreatGeneralExtraBonus() const;
	int GetGreatPersonGiftInfluence() const;
	int GetMaxGlobalBuildingProductionModifier() const;
	int GetMaxTeamBuildingProductionModifier() const;
	int GetMaxPlayerBuildingProductionModifier() const;
	int GetCityUnhappinessModifier() const;
	int GetPopulationUnhappinessModifier() const;
	int GetCityStateBonusModifier() const;
	int GetCityStateFriendshipModifier() const;
	int GetCityStateCombatModifier() const;
	int GetLandBarbarianConversionPercent() const;
	int GetLandBarbarianConversionExtraUnits() const;
	int GetSeaBarbarianConversionPercent() const;
	int GetCapitalBuildingModifier() const;
	int GetPlotBuyCostModifier() const;
	int GetNationalPopReqModifier() const;
#if defined(MOD_TRAITS_CITY_WORKING)
	int GetCityWorkingChange() const;
#endif
#if defined(MOD_TRAITS_CITY_AUTOMATON_WORKERS)
	int GetCityAutomatonWorkersChange() const;
#endif
	int GetPlotCultureCostModifier() const;
	int GetCultureFromKills() const;
	int GetFaithFromKills() const;
	int GetCityCultureBonus() const;
	int GetCapitalThemingBonusModifier() const;
	int GetPolicyCostModifier() const;
	int GetCityConnectionTradeRouteChange() const;
	int GetWonderProductionModifier() const;
	int GetPlunderModifier() const;
	int GetImprovementMaintenanceModifier() const;
#if defined(MOD_CIV6_WORKER)
	int GetRouteBuilderCostModifier() const;
#endif
	int GetGoldenAgeDurationModifier() const;
	int GetGoldenAgeMoveChange() const;
	int GetGoldenAgeCombatModifier() const;
	int GetGoldenAgeTourismModifier() const;
	int GetGoldenAgeGreatArtistRateModifier() const;
	int GetGoldenAgeGreatMusicianRateModifier() const;
	int GetGoldenAgeGreatWriterRateModifier() const;
	int GetExtraEmbarkMoves() const;
	int GetFreeUnitClassType() const;
	int GetNaturalWonderFirstFinderGold() const;
	int GetNaturalWonderSubsequentFinderGold() const;
	int GetNaturalWonderYieldModifier() const;
	int GetNaturalWonderHappinessModifier() const;
	int GetNearbyImprovementCombatBonus() const;
	int GetNearbyImprovementBonusRange() const;
	int GetCultureBuildingYieldChange() const;
#if defined(MOD_BALANCE_CORE)
	int GetWarWearinessModifier() const;
	int GetEnemyWarWearinessModifier() const;
	int GetCombatBonusVsHigherPop() const;
	bool IsBuyOwnedTiles() const;
	bool IsReconquista() const;
	bool IsForeignReligionSpreadImmune() const;
	int GetInspirationalLeader() const;
	int GetBullyMilitaryStrengthModifier() const;
	int GetBullyValueModifier() const;
	bool IgnoreBullyPenalties() const;
	bool IsDiplomaticMarriage() const;
	bool IsAdoptionFreeTech() const;
	bool IsGPWLTKD() const;
	bool IsGreatWorkWLTKD() const;
	bool IsExpansionWLTKD() const;
	bool IsTradeRouteOnly() const;
	bool IsKeepConqueredBuildings() const;
	bool IsMountainPass() const;
	bool IsUniqueBeliefsOnly() const;
	bool IsNoNaturalReligionSpread() const;
	bool IsNoOpenTrade() const;
	bool IsGoldenAgeOnWar() const;
	bool IsUnableToCancelRazing() const;
	int GetWLTKDGPImprovementModifier() const;
	int GetGrowthBoon() const;
	int GetAllianceCSDefense() const;
	int GetAllianceCSStrength() const;
	int GetTourismGABonus() const;
	int GetTourismToGAP() const;
	int GetGoldToGAP() const;
	int GetInfluenceMeetCS() const;
	int GetMultipleAttackBonus() const;
	int GetCityConquestGWAM() const;
	int GetEventTourismBoost() const;
	int GetEventGP() const;
	int GetWLTKDCulture() const;
	int GetWLTKDGATimer() const;
	int GetGAUnhappinesNeedMod() const;
	int GetStartingSpies() const;
	int GetStartingSpyRank() const;
	int GetSpyMoveRateBonus() const;
	int	GetEspionageModifier() const;
	int GetSpyExtraRankBonus() const;
	int GetQuestYieldModifier() const;
	int GetWonderProductionModifierToBuilding() const;
	int GetPolicyGEorGM() const;
	int GetGoldenAgeGarrisonedCityRangeStrikeModifier() const;
	bool IsBestUnitSpawnOnImprovementDOW() const;
	ImprovementTypes GetBestSpawnUnitImprovement() const;
	int GetGGGARateFromDenunciationsAndWars() const;
	bool IsTradeRouteMinorInfluenceAdmiralPoints() const;
	bool IsProductionModFromNumSpecialists() const;
	int GetConquestOfTheWorldCityAttack() const;
	bool IsConquestOfTheWorld() const;
	bool IsFreeUpgrade() const;
	bool IsWarsawPact() const;
	PromotionTypes GetEnemyWarSawPactPromotion() const;
	bool IsFreeZuluPikemanToImpi() const;
	bool IsPermanentYieldsDecreaseEveryEra() const;
	bool IsImportsCountTowardsMonopolies() const;
	bool IsCanPurchaseNavalUnitsFaith() const;
	int GetPuppetPenaltyReduction() const;
	int GetSharedReligionTourismModifier() const;
	int GetExtraMissionaryStrength() const;
	bool IsCanGoldInternalTradeRoutes() const;
	int GetExtraTradeRoutesPerXOwnedCities() const;
	int GetExtraTradeRoutesPerXOwnedVassals() const;
	bool IsCapitalOnly() const;
#endif
#if defined(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
	int GetInvestmentModifier() const;
#endif
	int GetCombatBonusVsHigherTech() const;
	int GetCombatBonusVsLargerCiv() const;
	int GetLandUnitMaintenanceModifier() const;
	int GetNavalUnitMaintenanceModifier() const;
	int GetRazeSpeedModifier() const;
	int GetDOFGreatPersonModifier() const;
	int GetLuxuryHappinessRetention() const;
#if defined(MOD_TRAITS_EXTRA_SUPPLY)
	int GetExtraSupply() const;
	int GetExtraSupplyPerCity() const;
	int GetExtraSupplyPerPopulation() const;
#endif
	int GetExtraSpies() const;
	int GetUnresearchedTechBonusFromKills() const;
	int GetExtraFoundedCityTerritoryClaimRange() const;
#if defined(MOD_BALANCE_CORE)
	int GetExtraConqueredCityTerritoryClaimRange() const;
	int GetExtraTenetsFirstAdoption() const;
#endif
	int GetFreeSocialPoliciesPerEra() const;
	int GetNumTradeRoutesModifier() const;
	int GetTradeRouteResourceModifier() const;
	int GetUniqueLuxuryCities() const;
	int GetUniqueLuxuryQuantity() const;
	int GetWorkerSpeedModifier() const;
	int GetAfraidMinorPerTurnInfluence() const;
	int GetLandTradeRouteRangeBonus() const;
#if defined(MOD_TRAITS_TRADE_ROUTE_BONUSES)
	int GetSeaTradeRouteRangeBonus() const;
#endif
	int GetTradeReligionModifier() const;
	int GetTradeBuildingModifier() const;
#if defined(MOD_BALANCE_CORE)
	int GetNumFreeBuildings() const;
	TechTypes GetFreeBuildingPrereqTech() const;
	TechTypes GetCapitalFreeBuildingPrereqTech() const;
	int TradeRouteStartYield(int i) const;
	int YieldFromRouteMovement(int i) const;
	int YieldFromOwnPantheon(int i) const;
	int YieldFromHistoricEvent(int i) const;
	int YieldFromLevelUp(int i) const;
#endif

	TechTypes GetFreeUnitPrereqTech() const;
	ImprovementTypes GetCombatBonusImprovement() const;
	BuildingTypes GetFreeBuilding() const;
#if defined(MOD_BALANCE_CORE)
	BuildingTypes GetFreeCapitalBuilding() const;
	UnitTypes GetFreeUnitOnConquest() const;
#endif
	BuildingTypes GetFreeBuildingOnConquest() const;
#if defined(MOD_BALANCE_CORE_AFRAID_ANNEX)
	bool IsBullyAnnex() const;
	int GetBullyYieldMultiplierAnnex() const;
#endif
	bool IsFightWellDamaged() const;
	bool IsWoodlandMovementBonus() const;
	bool IsRiverMovementBonus() const;
	bool IsFasterInHills() const;
	bool IsEmbarkedAllWater() const;
	bool IsEmbarkedToLandFlatCost() const;
	bool IsNoHillsImprovementMaintenance() const;
	bool IsTechBoostFromCapitalScienceBuildings() const;
	bool IsStaysAliveZeroCities() const;
	bool IsFaithFromUnimprovedForest() const;
#if defined(MOD_TRAITS_ANY_BELIEF)
	bool IsAnyBelief() const;
	bool IsAlwaysReligion() const;
	bool IsIgnoreTradeDistanceScaling() const;
	bool IsCanPlunderWithoutWar() const;
#endif
	bool IsBonusReligiousBelief() const;
	bool IsAbleToAnnexCityStates() const;
	bool IsCrossesMountainsAfterGreatGeneral() const;
#if defined(MOD_TRAITS_CROSSES_ICE)
	bool IsCrossesIce() const;
#endif
#if defined(MOD_TRAITS_GG_FROM_BARBARIANS)
	bool IsGGFromBarbarians() const;
#endif
	bool IsMayaCalendarBonuses() const;
	bool IsNoAnnexing() const;
	bool IsTechFromCityConquer() const;
	bool IsUniqueLuxuryRequiresNewArea() const;
	bool IsRiverTradeRoad() const;
	bool IsAngerFreeIntrusionOfCityStates() const;

	const char* getShortDescription() const;
	void setShortDescription(const char* szVal);
#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
	int GetPovertyHappinessChange() const;
	int GetDefenseHappinessChange() const;
	int GetIlliteracyHappinessChange() const;
	int GetUnculturedHappinessChange() const;
	int GetMinorityHappinessChange() const;
	bool IsNoConnectionUnhappiness() const;
	bool IsNoReligiousStrife() const;
	bool IsOddEraScaler() const;
	int GetWonderProductionModGA() const;
	int GetCultureBonusModifierConquest() const;
	int GetProductionBonusModifierConquest() const;
#endif

	// Arrays
	int GetExtraYieldThreshold(int i) const;
	int GetYieldChange(int i) const;
	int GetYieldChangeStrategicResources(int i) const;
	int GetYieldChangeNaturalWonder(int i) const;
	int GetYieldChangePerTradePartner(int i) const;
	int GetYieldChangeIncomingTradeRoute(int i) const;
	int GetYieldModifier(int i) const;
	int GetStrategicResourceQuantityModifier(int i) const;
	int GetObsoleteTech() const;
	int GetPrereqTech() const;
#if defined(MOD_TRAITS_OTHER_PREREQS)
	int GetObsoleteBelief() const;
	int GetPrereqBelief() const;
	int GetObsoletePolicy() const;
	int GetPrereqPolicy() const;
#endif
	int GetResourceQuantityModifier(int i) const;
	int GetMovesChangeUnitCombat(const int unitCombatID) const;
#if defined(MOD_BALANCE_CORE)
	int GetMovesChangeUnitClass(const int unitClassID) const;
	int GetGAPToYield(int i) const;
	int GetMountainRangeYield(int i) const;
	int GetNumPledgeDomainProductionModifier(DomainTypes eDomain) const;
	int GetDomainFreeExperienceModifier(DomainTypes eDomain) const;
	int GetGreatPersonProgressFromPolicyUnlock(GreatPersonTypes eIndex) const;
	int GetFreeUnitClassesDOW(UnitClassTypes eUnitClass) const;
	int GetYieldFromTileEarnTerrainType(TerrainTypes eIndex1, YieldTypes eIndex2) const;
	int GetYieldFromTilePurchaseTerrainType(TerrainTypes eIndex1, YieldTypes eIndex2) const;
	int GetYieldFromTileConquest(TerrainTypes eIndex1, YieldTypes eIndex2) const;
	int GetYieldFromTileCultureBomb(TerrainTypes eIndex1, YieldTypes eIndex2) const;
	int GetYieldFromTileStealCultureBomb(TerrainTypes eIndex1, YieldTypes eIndex2) const;
	int GetYieldFromTileSettle(TerrainTypes eIndex1, YieldTypes eIndex2) const;
	int GetYieldChangePerImprovementBuilt(ImprovementTypes eIndex1, YieldTypes eIndex2) const;
	int GetYieldFromBarbarianCampClear(YieldTypes eIndex1, bool bEraScaling) const;
#endif
	int GetMaintenanceModifierUnitCombat(const int unitCombatID) const;
	int GetImprovementYieldChanges(ImprovementTypes eIndex1, YieldTypes eIndex2) const;
#if defined(MOD_API_UNIFIED_YIELDS) && defined(MOD_API_PLOT_YIELDS)
	int GetPlotYieldChanges(PlotTypes eIndex1, YieldTypes eIndex2) const;
#endif
#if defined(MOD_BALANCE_CORE)
	int GetYieldFromLevelUp(int i) const;
	int GetYieldFromHistoricEvent(int i) const;
	int GetYieldFromOwnPantheon(int i) const;
	int GetTradeRouteStartYield(int i) const;
	int GetYieldFromRouteMovement(int i) const;
	int GetYieldFromExport(int i) const;
	int GetYieldFromImport(int i) const;
	int GetYieldFromTilePurchase(int i) const;
	int GetYieldFromTileEarn(int i) const;
	int GetYieldFromCSAlly(int i) const;
	int GetYieldFromCSFriend(int i) const;
	int GetYieldFromSettle(int i) const;
	int GetYieldFromConquest(int i) const;
	int GetVotePerXCSAlliance() const;
	int GetVotePerXCSFollowingYourReligion() const;
	int GetChanceToConvertReligiousUnits() const;
	int GetGoldenAgeFromVictory() const;
	bool IsFreeGreatWorkOnConquest() const;
	bool IsPopulationBoostReligion() const;
	bool IsCombatBoostNearNaturalWonder() const;
	int GetFreePolicyPerXTechs() const;
	EraTypes GetGPFaithPurchaseEra() const;
	int GetFaithCostModifier() const;
#endif
#if defined(MOD_BALANCE_CORE) && defined(MOD_TRAITS_YIELD_FROM_ROUTE_MOVEMENT_IN_FOREIGN_TERRITORY)
	int GetYieldFromRouteMovementInForeignTerritory(YieldTypes eIndex, bool bTradePartner) const;
#endif
#if defined(MOD_API_UNIFIED_YIELDS)
	int GetBuildingClassYieldChanges(BuildingClassTypes eIndex1, YieldTypes eIndex2) const;
	int GetCapitalYieldChanges(int i) const;
	int GetCityYieldChanges(int i) const;
	int GetPermanentYieldChangeWLTKD(int i) const;
	int GetCoastalCityYieldChanges(int i) const;
	int GetGreatWorkYieldChanges(int i) const;
	int GetArtifactYieldChanges(int i) const;
	int GetArtYieldChanges(int i) const;
	int GetLitYieldChanges(int i) const;
	int GetMusicYieldChanges(int i) const;
	int GetSeaPlotYieldChanges(int i) const;
	int GetFeatureYieldChanges(FeatureTypes eIndex1, YieldTypes eIndex2) const;
	int GetResourceYieldChanges(ResourceTypes eIndex1, YieldTypes eIndex2) const;
	int GetTerrainYieldChanges(TerrainTypes eIndex1, YieldTypes eIndex2) const;
	int GetYieldFromKills(YieldTypes eYield) const;
	int GetYieldFromBarbarianKills(YieldTypes eYield) const;
	int GetYieldChangeTradeRoute(int i) const;
	int GetYieldChangeWorldWonder(int i) const;
	int GetTradeRouteYieldChange(DomainTypes eIndex1, YieldTypes eIndex2) const;
#endif
	int GetSpecialistYieldChanges(SpecialistTypes eIndex1, YieldTypes eIndex2) const;
#if defined(MOD_API_UNIFIED_YIELDS)
	int GetGreatPersonExpendedYield(GreatPersonTypes eIndex1, YieldTypes eIndex2) const;
	int GetGreatPersonBornYield(GreatPersonTypes eIndex1, YieldTypes eIndex2) const;
	int GetGoldenAgeGreatPersonRateModifier(GreatPersonTypes eIndex1) const;
	int GetPerPuppetGreatPersonRateModifier(GreatPersonTypes eIndex1) const;
	int GetGreatPersonCostReduction(GreatPersonTypes eIndex1) const;
	int GetGreatPersonGWAM(GreatPersonTypes eIndex1) const;
#if defined(MOD_BALANCE_CORE)
	int GetGoldenAgeFromGreatPersonBirth(GreatPersonTypes eIndex1) const;
#endif
	int GetCityYieldFromUnimprovedFeature(FeatureTypes eIndex1, YieldTypes eIndex2) const;
#endif
	int GetUnimprovedFeatureYieldChanges(FeatureTypes eIndex1, YieldTypes eIndex2) const;
	FreeResourceXCities GetFreeResourceXCities(ResourceTypes eResource) const;

	bool IsFreePromotionUnitCombat(const int promotionID, const int unitCombatID) const;
#if defined(MOD_BALANCE_CORE)
	bool IsSpecialUpgradeUnitClass(const int unitClassesID, const int unitID) const;
	bool IsFreePromotionUnitClass(const int promotionID, const int unitClassID) const;
	int GetGoldenAgeYieldModifier(const int iYield) const;
	std::pair<int, bool> GetUnitCombatProductionCostModifier(const int unitCombatID) const;
	int GetNonSpecialistFoodChange() const;
	bool IsNoBuild(BuildTypes eBuild) const;
	int GetDomainProductionModifiersPerSpecialist(DomainTypes eDomain) const;
	bool UnitClassCanBuild(const int buildID, const int unitClassID) const;
	bool TerrainClaimBoost(TerrainTypes eTerrain);
#endif
#if defined(MOD_TRAITS_TRADE_ROUTE_PRODUCTION_SIPHON)
	TradeRouteProductionSiphon GetTradeRouteProductionSiphon(const bool bInternationalOnly) const;
#endif
	bool IsObsoleteByTech(TeamTypes eTeam);
	bool IsEnabledByTech(TeamTypes eTeam);
#if defined(MOD_TRAITS_OTHER_PREREQS)
	bool IsObsoleteByBelief(PlayerTypes ePlayer);
	bool IsEnabledByBelief(PlayerTypes ePlayer);
	bool IsObsoleteByPolicy(PlayerTypes ePlayer);
	bool IsEnabledByPolicy(PlayerTypes ePlayer);
#endif

	bool NoTrain(UnitClassTypes eUnitClassType);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:

	int m_iLevelExperienceModifier;
	int m_iGreatPeopleRateModifier;
	int m_iGreatScientistRateModifier;
	int m_iGreatGeneralRateModifier;
	int m_iGreatGeneralExtraBonus;
	int m_iGreatPersonGiftInfluence;
	int m_iMaxGlobalBuildingProductionModifier;
	int m_iMaxTeamBuildingProductionModifier;
	int m_iMaxPlayerBuildingProductionModifier;
	int m_iCityUnhappinessModifier;
	int m_iPopulationUnhappinessModifier;
	int m_iCityStateBonusModifier;
	int m_iCityStateFriendshipModifier;
	int m_iCityStateCombatModifier;
	int m_iLandBarbarianConversionPercent;
	int m_iLandBarbarianConversionExtraUnits;
	int m_iSeaBarbarianConversionPercent;
	int m_iCapitalBuildingModifier;
	int m_iPlotBuyCostModifier;
	int m_iNationalPopReqModifier;
#if defined(MOD_TRAITS_CITY_WORKING)
	int m_iCityWorkingChange;
#endif
#if defined(MOD_TRAITS_CITY_AUTOMATON_WORKERS)
	int m_iCityAutomatonWorkersChange;
#endif
	int m_iPlotCultureCostModifier;
	int m_iCultureFromKills;
	int m_iFaithFromKills;
	int m_iCityCultureBonus;
	int m_iCapitalThemingBonusModifier;
	int m_iPolicyCostModifier;
	int m_iCityConnectionTradeRouteChange;
	int m_iWonderProductionModifier;
	int m_iPlunderModifier;
	int m_iImprovementMaintenanceModifier;
#if defined(MOD_CIV6_WORKER)
	int m_iRouteBuilderCostModifier;
#endif
	int m_iGoldenAgeDurationModifier;
	int m_iGoldenAgeMoveChange;
	int m_iGoldenAgeCombatModifier;
	int m_iGoldenAgeTourismModifier;
	int m_iGoldenAgeGreatArtistRateModifier;
	int m_iGoldenAgeGreatMusicianRateModifier;
	int m_iGoldenAgeGreatWriterRateModifier;
	int m_iObsoleteTech;
	int m_iPrereqTech;
#if defined(MOD_TRAITS_OTHER_PREREQS)
	int m_iObsoleteBelief;
	int m_iPrereqBelief;
	int m_iObsoletePolicy;
	int m_iPrereqPolicy;
#endif
	int m_iExtraEmbarkMoves;
	int m_iFreeUnitClassType;
	int m_iNaturalWonderFirstFinderGold;
	int m_iNaturalWonderSubsequentFinderGold;
	int m_iNaturalWonderYieldModifier;
	int m_iNaturalWonderHappinessModifier;
	int m_iNearbyImprovementCombatBonus;
	int m_iNearbyImprovementBonusRange;
	int m_iCultureBuildingYieldChange;
#if defined(MOD_BALANCE_CORE)
	int m_iWarWearinessModifier;
	int m_iEnemyWarWearinessModifier;
	int m_iCombatBonusVsHigherPop;
	bool m_bBuyOwnedTiles;
	bool m_bReconquista;
	bool m_bNoSpread;
	int m_iInspirationalLeader;
	int m_iBullyMilitaryStrengthModifier;
	int m_iBullyValueModifier;
	bool m_bIgnoreBullyPenalties;
	bool m_bDiplomaticMarriage;
	bool m_bAdoptionFreeTech;
	bool m_bGPWLTKD;
	bool m_bGreatWorkWLTKD;
	bool m_bExpansionWLTKD;
	bool m_bTradeRouteOnly;
	bool m_bKeepConqueredBuildings;
	bool m_bMountainPass;
	bool m_bUniqueBeliefsOnly;
	int m_iGrowthBoon;
	int m_iWLTKDGPImprovementModifier;
	int m_iAllianceCSDefense;
	int m_iAllianceCSStrength;
	int m_iTourismGABonus;
	bool m_bNoNaturalReligionSpread;
	bool m_bNoOpenTrade;
	bool m_bGoldenAgeOnWar;
	bool m_bUnableToCancelRazing;
	int m_iTourismToGAP;
	int m_iGoldToGAP;
	int m_iInfluenceMeetCS;
	int m_iMultipleAttackBonus;
	int m_iCityConquestGWAM;
	int m_iEventTourismBoost;
	int m_iEventGP;
	int m_iWonderProductionModifierToBuilding;
	int m_iPolicyGEorGM;
	int m_iGAGarrisonCityRangeStrikeModifier;
	bool m_bBestUnitSpawnOnImpDOW;
	ImprovementTypes m_iBestUnitImprovement;
	int m_iGGGARateFromDenunciationsAndWars;
	bool m_bTradeRouteMinorInfluenceAP;
	bool m_bProdModFromNumSpecialists;
	int m_iConquestOfTheWorldCityAttack;
	bool m_bConquestOfTheWorld;
	bool m_bFreeUpgrade;
	bool m_bWarsawPact;
	int m_iEnemyWarSawPactPromotion;
	bool m_bFreeZuluPikemanToImpi;
	bool m_bPermanentYieldsDecreaseEveryEra;
	bool m_bImportsCountTowardsMonopolies;
	bool m_bCanPurchaseNavalUnitsFaith;
	int m_iPuppetPenaltyReduction;
	int m_iSharedReligionTourismModifier;
	int m_iExtraMissionaryStrength;
	bool m_bCanGoldInternalTradeRoutes;
	int m_iExtraTradeRoutesPerXOwnedCities;
	int m_iExtraTradeRoutesPerXOwnedVassals;
	bool m_bIsCapitalOnly;
#endif
#if defined(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
	int m_iInvestmentModifier;
#endif
	int m_iCombatBonusVsHigherTech;
	int m_iCombatBonusVsLargerCiv;
	int m_iLandUnitMaintenanceModifier;
	int m_iNavalUnitMaintenanceModifier;
	int m_iRazeSpeedModifier;
	int m_iDOFGreatPersonModifier;
	int m_iLuxuryHappinessRetention;
#if defined(MOD_TRAITS_EXTRA_SUPPLY)
	int m_iExtraSupply;
	int m_iExtraSupplyPerCity;
	int m_iExtraSupplyPerPopulation;
#endif
	int m_iExtraSpies;
	int m_iUnresearchedTechBonusFromKills;
	int m_iExtraFoundedCityTerritoryClaimRange;
#if defined(MOD_BALANCE_CORE)
	int m_iExtraConqueredCityTerritoryClaimRange;
	int m_iExtraTenetsFirstAdoption;
#endif
	int m_iFreeSocialPoliciesPerEra;
	int m_iNumTradeRoutesModifier;
	int m_iTradeRouteResourceModifier;
	int m_iUniqueLuxuryCities;
	int m_iUniqueLuxuryQuantity;
	int m_iWorkerSpeedModifier;
	int m_iAfraidMinorPerTurnInfluence;
	int m_iLandTradeRouteRangeBonus;
#if defined(MOD_TRAITS_TRADE_ROUTE_BONUSES)
	int m_iSeaTradeRouteRangeBonus;
#endif
	int m_iTradeReligionModifier;
	int m_iTradeBuildingModifier;
#if defined(MOD_BALANCE_CORE)
	int m_iNumFreeBuildings;
	int m_iWLTKDCulture;
	int m_iWLTKDGATimer;
	int m_iGAUnhappinesNeedMod;
	int m_iStartingSpies;
	int m_iStartingSpyRank;
	int m_iSpyMoveRateBonus;
	int m_iEspionageModifier;
	int m_iSpyExtraRankBonus;
	int m_iQuestYieldModifier;
#endif

	TechTypes m_eFreeUnitPrereqTech;
	ImprovementTypes m_eCombatBonusImprovement;
	BuildingTypes m_eFreeBuilding;
#if defined(MOD_BALANCE_CORE)
	BuildingTypes m_eFreeCapitalBuilding;
	TechTypes m_eFreeBuildingPrereqTech;
	TechTypes m_eCapitalFreeBuildingPrereqTech;
	UnitTypes m_eFreeUnitOnConquest;
#endif
	BuildingTypes m_eFreeBuildingOnConquest;
#if defined(MOD_BALANCE_CORE_AFRAID_ANNEX)
	bool m_bBullyAnnex;
	int m_iBullyYieldMultiplierAnnex;
#endif
	bool m_bFightWellDamaged;
	bool m_bWoodlandMovementBonus;
	bool m_bRiverMovementBonus;
	bool m_bFasterInHills;
	bool m_bEmbarkedAllWater;
	bool m_bEmbarkedToLandFlatCost;
	bool m_bNoHillsImprovementMaintenance;
	bool m_bTechBoostFromCapitalScienceBuildings;
	bool m_bStaysAliveZeroCities;
	bool m_bFaithFromUnimprovedForest;
#if defined(MOD_TRAITS_ANY_BELIEF)
	bool m_bAnyBelief;
	bool m_bAlwaysReligion;
	bool m_bIgnoreTradeDistanceScaling;
	bool m_bCanPlunderWithoutWar;
#endif
	bool m_bBonusReligiousBelief;
	bool m_bAbleToAnnexCityStates;
	bool m_bCrossesMountainsAfterGreatGeneral;
#if defined(MOD_TRAITS_CROSSES_ICE)
	bool m_bCrossesIce;
#endif
#if defined(MOD_TRAITS_GG_FROM_BARBARIANS)
	bool m_bGGFromBarbarians;
#endif
	bool m_bMayaCalendarBonuses;
	bool m_bNoAnnexing;
	bool m_bTechFromCityConquer;
	bool m_bUniqueLuxuryRequiresNewArea;
	bool m_bRiverTradeRoad;
	bool m_bAngerFreeIntrusionOfCityStates;
#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
	int m_iPovertyHappinessChange;
	int m_iDefenseHappinessChange;
	int m_iUnculturedHappinessChange;
	int m_iIlliteracyHappinessChange;
	int m_iMinorityHappinessChange;
	bool m_bNoConnectionUnhappiness;
	bool m_bIsNoReligiousStrife;
	bool m_bIsOddEraScaler;
	int m_iWonderProductionModGA;
	int m_iCultureBonusModifierConquest;
	int m_iProductionBonusModifierConquest;
#endif

	CvString m_strShortDescription;

	// Arrays
	int* m_paiExtraYieldThreshold;
	int* m_paiYieldChange;
	int* m_paiYieldChangeStrategicResources;
	int* m_paiYieldChangeNaturalWonder;
	int* m_paiYieldChangePerTradePartner;
	int* m_paiYieldChangeIncomingTradeRoute;
	int* m_paiYieldModifier;
	int* m_piStrategicResourceQuantityModifier;
	int* m_piResourceQuantityModifiers;
	int* m_piMovesChangeUnitCombats;
#if defined(MOD_BALANCE_CORE)
	int* m_paiGAPToYield;
	int* m_paiMountainRangeYield;
	int* m_piMovesChangeUnitClasses;
	int** m_ppiYieldFromTileEarnTerrainType;
	int** m_ppiYieldFromTilePurchaseTerrainType;
	int** m_ppiYieldFromTileConquest;
	int** m_ppiYieldFromTileCultureBomb;
	int** m_ppiYieldFromTileStealCultureBomb;
	int** m_ppiYieldFromTileSettle;
	int** m_ppiYieldChangePerImprovementBuilt;
	std::map<int, std::map<bool, int>> m_pbiYieldFromBarbarianCampClear;
#endif
	int* m_piMaintenanceModifierUnitCombats;
	int** m_ppiImprovementYieldChanges;
#if defined(MOD_API_UNIFIED_YIELDS) && defined(MOD_API_PLOT_YIELDS)
	int** m_ppiPlotYieldChanges;
#endif
#if defined(MOD_BALANCE_CORE)
	int* m_piYieldFromLevelUp;
	int* m_piYieldFromHistoricEvent;
	int* m_piYieldFromOwnPantheon;
	int* m_piTradeRouteStartYield;
	int* m_piYieldFromRouteMovement;
	int* m_piYieldFromExport;
	int* m_piYieldFromImport;
	int* m_piYieldFromTilePurchase;
	int* m_piYieldFromTileEarn;
	int* m_piYieldFromCSAlly;
	int* m_piYieldFromCSFriend;
	int* m_piYieldFromSettle;
	int* m_piYieldFromConquest;
	int m_iVotePerXCSAlliance;
	int m_iVotePerXCSFollowingFollowingYourReligion;
	int m_iChanceToConvertReligiousUnits;
	int m_iGoldenAgeFromVictory;
	int m_iFreePolicyPerXTechs;
	EraTypes m_eGPFaithPurchaseEra;
	int m_iFaithCostModifier;
	bool m_bFreeGreatWorkOnConquest;
	bool m_bPopulationBoostReligion;
	bool m_bCombatBoostNearNaturalWonder;
	int* m_piNumPledgesDomainProdMod;
	int* m_piDomainFreeExperienceModifier;
	int* m_piGreatPersonProgressFromPolicyUnlock;
	int* m_piFreeUnitClassesDOW;
#endif
#if defined(MOD_BALANCE_CORE) && defined(MOD_TRAITS_YIELD_FROM_ROUTE_MOVEMENT_IN_FOREIGN_TERRITORY)
	std::map<int, std::map<bool, int>> m_pbiYieldFromRouteMovementInForeignTerritory;
#endif
#if defined(MOD_API_UNIFIED_YIELDS)
	int** m_ppiBuildingClassYieldChanges;
	int* m_piCapitalYieldChanges;
	int* m_piCityYieldChanges;
	int* m_piPermanentYieldChangeWLTKD;
	int* m_piCoastalCityYieldChanges;
	int* m_piGreatWorkYieldChanges;
	int* m_piArtifactYieldChanges;
	int* m_piArtYieldChanges;
	int* m_piLitYieldChanges;
	int* m_piMusicYieldChanges;
	int* m_piSeaPlotYieldChanges;
	int** m_ppiFeatureYieldChanges;
	int** m_ppiResourceYieldChanges;
	int** m_ppiTerrainYieldChanges;
	int* m_piYieldFromKills;
	int* m_piYieldFromBarbarianKills;
	int* m_piYieldChangeTradeRoute;
	int* m_piYieldChangeWorldWonder;
	int** m_ppiTradeRouteYieldChange;
#endif
	int** m_ppiSpecialistYieldChanges;
#if defined(MOD_API_UNIFIED_YIELDS)
	int** m_ppiGreatPersonExpendedYield;
	int** m_ppiGreatPersonBornYield;
	int* m_piGreatPersonCostReduction;
	int* m_piGoldenAgeGreatPersonRateModifier;
	int* m_piPerPuppetGreatPersonRateModifier;
	int* m_piGreatPersonGWAM;
	int** m_ppiCityYieldFromUnimprovedFeature;
#if defined(MOD_BALANCE_CORE)
	int* m_piGoldenAgeFromGreatPersonBirth;
#endif
#endif
	int** m_ppiUnimprovedFeatureYieldChanges;

	std::multimap<int, int> m_FreePromotionUnitCombats;
#if defined(MOD_BALANCE_CORE)
	std::multimap<int, int> m_FreePromotionUnitClass;
	std::multimap<int, int> m_BuildsUnitClasses;
	std::vector<bool> m_abTerrainClaimBoost;
	std::multimap<int, int> m_piUpgradeUnitClass;
	std::map<int, int> m_piGoldenAgeYieldModifier;
	std::map<int, std::pair<int, bool>> m_pibUnitCombatProductionCostModifier;
	int m_iNonSpecialistFoodChange;
	std::vector<int> m_aiNoBuilds;
	std::map<int, int> m_piDomainProductionModifiersPerSpecialist;
#endif
#if defined(MOD_TRAITS_TRADE_ROUTE_PRODUCTION_SIPHON)
	std::map<bool, TradeRouteProductionSiphon> m_biiTradeRouteProductionSiphon;
#endif
	std::vector<FreeResourceXCities> m_aFreeResourceXCities;
	std::vector<bool> m_abNoTrainUnitClass;

private:
	CvTraitEntry(const CvTraitEntry&);
	CvTraitEntry& operator=(const CvTraitEntry&);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvTraitXMLEntries
//!  \brief		Game-wide information about civilization traits
//
//! Key Attributes:
//! - Plan is it will be contained in CvGameRules object within CvGame class
//! - Populated from XML\GameInfo\CIV5Traits.xml
//! - Contains an array of CvTraitEntry from the above XML file
//! - One instance for the entire game
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTraitXMLEntries
{
public:
	CvTraitXMLEntries(void);
	~CvTraitXMLEntries(void);

	// Accessor functions
	std::vector<CvTraitEntry*>& GetTraitEntries();
	int GetNumTraits();
	CvTraitEntry* GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvTraitEntry*> m_paTraitEntries;
};

#define SAFE_ESTIMATE_NUM_FREE_UNITS 5

struct FreeTraitUnit
{
	UnitTypes m_iFreeUnit;
	TechTypes m_ePrereqTech;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvPlayerTraits
//!  \brief		Information about the traits of a player
//
//!  Key Attributes:
//!  - One instance for each player
//!  - Accessed by any class that needs to check trait info
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPlayerTraits
{
public:
	CvPlayerTraits(void);
	~CvPlayerTraits(void);
	void Init(CvTraitXMLEntries* pTraits, CvPlayer* pPlayer);
	void Uninit();
	void Reset();
	void InitPlayerTraits();

	void SetIsWarmonger();
	void SetIsNerd();
	void SetIsTourism();
	void SetIsDiplomat();
	void SetIsExpansionist();
	void SetIsSmaller();
	void SetIsReligious();

	bool IsWarmonger() const
	{
		return m_bIsWarmonger;
	}
	bool IsNerd() const
	{
		return m_bIsNerd;
	}
	bool IsTourism() const
	{
		return m_bIsTourism;
	}
	bool IsDiplomat() const
	{
		return m_bIsDiplomat;
	}
	bool IsExpansionist() const
	{
		return m_bIsExpansionist;
	}
	bool IsSmaller() const
	{
		return m_bIsSmaller;
	}
	bool IsReligious() const
	{
		return m_bIsReligious;
	}

	// Accessor functions
	bool HasTrait(TraitTypes eTrait) const;
	int GetGreatPeopleRateModifier() const
	{
		return m_iGreatPeopleRateModifier;
	};
	int GetGreatScientistRateModifier() const
	{
		return m_iGreatScientistRateModifier;
	};
	int GetGreatGeneralRateModifier() const
	{
		return m_iGreatGeneralRateModifier;
	};
	int GetGreatGeneralExtraBonus() const
	{
		return m_iGreatGeneralExtraBonus;
	};
	int GetGreatPersonGiftInfluence() const
	{
		return m_iGreatPersonGiftInfluence;
	};
	int GetLevelExperienceModifier() const
	{
		return m_iLevelExperienceModifier;
	};
	int GetMaxGlobalBuildingProductionModifier() const
	{
		return m_iMaxGlobalBuildingProductionModifier;
	};
	int GetMaxTeamBuildingProductionModifier() const
	{
		return m_iMaxTeamBuildingProductionModifier;
	};
	int GetMaxPlayerBuildingProductionModifier() const
	{
		return m_iMaxPlayerBuildingProductionModifier;
	};
	int GetCityUnhappinessModifier() const
	{
		return m_iCityUnhappinessModifier;
	};
	int GetPopulationUnhappinessModifier() const
	{
		return m_iPopulationUnhappinessModifier;
	};
	int GetCityStateBonusModifier() const
	{
		return m_iCityStateBonusModifier;
	};
	int GetCityStateFriendshipModifier() const
	{
		return m_iCityStateFriendshipModifier;
	};
	int GetCityStateCombatModifier() const
	{
		return m_iCityStateCombatModifier;
	};
	int GetLandBarbarianConversionPercent() const
	{
		return m_iLandBarbarianConversionPercent;
	};
	int GetLandBarbarianConversionExtraUnits() const
	{
		return m_iLandBarbarianConversionExtraUnits;
	};
	int GetSeaBarbarianConversionPercent() const
	{
		return m_iSeaBarbarianConversionPercent;
	};
	int GetCapitalBuildingModifier() const
	{
		return m_iCapitalBuildingModifier;
	};
	int GetPlotBuyCostModifier() const
	{
		return m_iPlotBuyCostModifier;
	};
	int GetNationalPopReqModifier() const
	{
		return m_iNationalPopReqModifier;
	};
#if defined(MOD_TRAITS_CITY_WORKING)
	int GetCityWorkingChange() const
	{
		return m_iCityWorkingChange;
	};
#endif
#if defined(MOD_TRAITS_CITY_AUTOMATON_WORKERS)
	int GetCityAutomatonWorkersChange() const
	{
		return m_iCityAutomatonWorkersChange;
	};
#endif
	int GetPlotCultureCostModifier() const
	{
		return m_iPlotCultureCostModifier;
	};
	int GetCultureFromKills() const
	{
		return m_iCultureFromKills;
	};
	int GetFaithFromKills() const
	{
		return m_iFaithFromKills;
	};
	int GetCityCultureBonus() const
	{
		return m_iCityCultureBonus;
	};
	int GetCapitalThemingBonusModifier() const
	{
		return m_iCapitalThemingBonusModifier;
	}
	int GetPolicyCostModifier() const
	{
		return m_iPolicyCostModifier;
	};
	int GetCityConnectionTradeRouteChange() const
	{
		return m_iCityConnectionTradeRouteChange;
	};
	int GetWonderProductionModifier() const
	{
		return m_iWonderProductionModifier;
	};
	int GetPlunderModifier() const
	{
		return m_iPlunderModifier;
	};
	int GetImprovementMaintenanceModifier() const
	{
		return m_iImprovementMaintenanceModifier;
	};
#if defined(MOD_CIV6_WORKER)
	int GetRouteBuilderCostModifier() const
	{
		return m_iRouteBuilderCostModifier;
	};
#endif
	int GetGoldenAgeDurationModifier() const
	{
		return m_iGoldenAgeDurationModifier;
	};
	int GetGoldenAgeMoveChange() const
	{
		return m_iGoldenAgeMoveChange;
	};
	int GetGoldenAgeCombatModifier() const
	{
		return m_iGoldenAgeCombatModifier;
	};
	int GetGoldenAgeTourismModifier() const
	{
		return m_iGoldenAgeTourismModifier;
	};
	int GetGoldenAgeGreatArtistRateModifier() const
	{
		return m_iGoldenAgeGreatArtistRateModifier;
	};
	int GetGoldenAgeGreatMusicianRateModifier() const
	{
		return m_iGoldenAgeGreatMusicianRateModifier;
	};
	int GetGoldenAgeGreatWriterRateModifier() const
	{
		return m_iGoldenAgeGreatWriterRateModifier;
	};
	int GetExtraEmbarkMoves() const
	{
		return m_iExtraEmbarkMoves;
	};
	int GetFirstFreeUnit(TechTypes eTech);
	int GetNextFreeUnit();
	int GetNaturalWonderFirstFinderGold() const
	{
		return m_iNaturalWonderFirstFinderGold;
	};
	int GetNaturalWonderSubsequentFinderGold() const
	{
		return m_iNaturalWonderSubsequentFinderGold;
	};
	int GetNaturalWonderYieldModifier() const
	{
		return m_iNaturalWonderYieldModifier;
	};
	int GetNaturalWonderHappinessModifier() const
	{
		return m_iNaturalWonderHappinessModifier;
	};
	int GetNearbyImprovementCombatBonus() const
	{
		return m_iNearbyImprovementCombatBonus;
	};
	int GetNearbyImprovementBonusRange() const
	{
		return m_iNearbyImprovementBonusRange;
	};
	int GetCultureBuildingYieldChange() const
	{
		return m_iCultureBuildingYieldChange;
	};
#if defined(MOD_BALANCE_CORE)
	int GetWarWearinessModifier() const
	{
		return m_iWarWearinessModifier;
	};
	int GetEnemyWarWearinessModifier() const
	{
		return m_iEnemyWarWearinessModifier;
	};
	int GetCombatBonusVsHigherPop() const
	{
		return m_iCombatBonusVsHigherPop;
	};
	bool IsBuyOwnedTiles() const
	{
		return m_bBuyOwnedTiles;
	};
	bool IsReconquista() const
	{
		return m_bReconquista;
	};
	bool IsForeignReligionSpreadImmune() const
	{
		return m_bNoSpread;
	};
	int GetInspirationalLeader() const
	{
		return m_iInspirationalLeader;
	};
	int GetBullyMilitaryStrengthModifier() const
	{
		return m_iBullyMilitaryStrengthModifier;
	};
	int GetBullyValueModifier() const
	{
		return m_iBullyValueModifier;
	};
	bool IgnoreBullyPenalties() const
	{
		return m_bIgnoreBullyPenalties;
	};
	bool IsDiplomaticMarriage() const
	{
		return m_bDiplomaticMarriage;
	};
	bool IsAdoptionFreeTech() const
	{
		return m_bAdoptionFreeTech;
	};
	bool IsGPWLTKD() const
	{
		return m_bGPWLTKD;
	};
	bool IsGreatWorkWLTKD() const
	{
		return m_bGreatWorkWLTKD;
	};
	bool IsExpansionWLTKD() const
	{
		return m_bExpansionWLTKD;
	};
	bool IsTradeRouteOnly() const
	{
		return m_bTradeRouteOnly;
	};
	bool IsKeepConqueredBuildings() const
	{
		return m_bKeepConqueredBuildings;
	};
	bool IsMountainPass() const
	{
		return m_bMountainPass;
	};
	bool IsUniqueBeliefsOnly() const
	{
		return m_bUniqueBeliefsOnly;
	};
	bool IsNoNaturalReligionSpread() const
	{
		return m_bNoNaturalReligionSpread;
	};
	bool IsNoOpenTrade() const
	{
		return m_bNoOpenTrade;
	};
	bool IsGoldenAgeOnWar() const
	{
		return m_bGoldenAgeOnWar;
	};
	bool IsUnableToCancelRazing() const
	{
		return m_bUnableToCancelRazing;
	}
	int GetWLTKDGPImprovementModifier() const
	{
		return m_iWLTKDGPImprovementModifier;
	};
	int GetGrowthBoon() const
	{
		return m_iGrowthBoon;
	};
	int GetAllianceCSDefense() const
	{
		return m_iAllianceCSDefense;
	};
	int GetAllianceCSStrength() const
	{
		return m_iAllianceCSStrength;
	};
	int GetTourismGABonus() const
	{
		return m_iTourismGABonus;
	};
	int GetTourismToGAP() const
	{
		return m_iTourismToGAP;
	};
	int GetGoldToGAP() const
	{
		return m_iGoldToGAP;
	};
	int GetInfluenceMeetCS() const
	{
		return m_iInfluenceMeetCS;
	};
	int GetMultipleAttackBonus() const
	{
		return m_iMultipleAttackBonus;
	};
	int GetCityConquestGWAM() const
	{
		return m_iCityConquestGWAM;
	};
	int GetEventTourismBoost() const
	{
		return m_iEventTourismBoost;
	};
	int GetEventGP() const
	{
		return m_iEventGP;
	};
	int GetWLTKDCulture() const
	{
		return m_iWLTKDCulture;
	};
	int GetWLTKDGATimer() const
	{
		return m_iWLTKDGATimer;
	};
	int GetGAUnhappinesNeedMod() const
	{
		return m_iGAUnhappinesNeedMod;
	};
	int GetStartingSpies() const
	{
		return m_iStartingSpies;
	};
	int GetStartingSpyRank() const
	{
		return m_iStartingSpyRank;
	};
	int GetSpyMoveRateBonus() const
	{
		return m_iSpyMoveRateBonus;
	};
	int GetEspionageModifier() const
	{
		return m_iEspionageModifier;
	};
	int GetSpyExtraRankBonus() const
	{
		return m_iSpyExtraRankBonus;
	};
	int GetQuestYieldModifier() const
	{
		return m_iQuestYieldModifier;
	};
	int GetWonderProductionModifierToBuilding() const
	{
		return m_iWonderProductionModifierToBuilding;
	};
	int GetPolicyGEorGM() const
	{
		return m_iPolicyGEorGM;
	};
	int GetGoldenAgeGarrisonedCityRangeStrikeModifier() const
	{
		return m_iGAGarrisonCityRangeStrikeModifier;
	};
	bool IsBestUnitSpawnOnImprovementDOW() const
	{
		return m_bBestUnitSpawnOnImpDOW;
	};
	ImprovementTypes GetBestSpawnUnitImprovement()
	{
		return m_iBestUnitImprovement;
	};
	void SetBestSpawnUnitImprovement(ImprovementTypes eType)
	{
		m_iBestUnitImprovement = eType;
	};
	int GetGGGARateFromDenunciationsAndWars() const
	{
		return m_iGGGARateFromDenunciationsAndWars;
	};
	bool IsTradeRouteMinorInfluenceAdmiralPoints() const
	{
		return m_bTradeRouteMinorInfluenceAP;
	};
	bool IsProductionModFromNumSpecialists() const
	{
		return m_bProdModFromNumSpecialists;
	};
	int GetConquestOfTheWorldCityAttack() const
	{
		return m_iConquestOfTheWorldCityAttack;
	};
	bool IsConquestOfTheWorld() const
	{
		return m_bConquestOfTheWorld;
	};
	bool IsFreeUpgrade() const
	{
		return m_bFreeUpgrade;
	};
	bool IsFreeZuluPikemanToImpi() const
	{
		return m_bFreeZuluPikemanToImpi;
	};
	bool IsPermanentYieldsDecreaseEveryEra() const
	{
		return m_bPermanentYieldsDecreaseEveryEra;
	};
	bool IsImportsCountTowardsMonopolies() const
	{
		return m_bImportsCountTowardsMonopolies;
	};
	bool IsCanPurchaseNavalUnitsFaith() const
	{
		return m_bCanPurchaseNavalUnitsFaith;
	};
	int GetPuppetPenaltyReduction() const
	{
		return m_iPuppetPenaltyReduction;
	};
	bool IsWarsawPact() const
	{
		return m_bWarsawPact;
	};
	PromotionTypes GetEnemyWarSawPactPromotion() const
	{
		return (PromotionTypes)m_iEnemyWarSawPactPromotion;
	};
	int GetSharedReligionTourismModifier() const
	{
		return m_iSharedReligionTourismModifier;
	};
	int GetExtraMissionaryStrength() const
	{
		return m_iExtraMissionaryStrength;
	};
	bool IsCanGoldInternalTradeRoutes() const
	{
		return m_bCanGoldInternalTradeRoutes;
	};
	int GetExtraTradeRoutesPerXOwnedCities() const
	{
		return m_iExtraTradeRoutesPerXOwnedCities;
	};
	int GetExtraTradeRoutesPerXOwnedVassals() const
	{
		return m_iExtraTradeRoutesPerXOwnedVassals;
	};
	bool IsCapitalOnly() const
	{
		return m_bIsCapitalOnly;
	};
#endif
#if defined(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
	int GetInvestmentModifier() const
	{
		return m_iInvestmentModifier;
	};
#endif
	int GetCombatBonusVsHigherTech() const
	{
		return m_iCombatBonusVsHigherTech;
	};
	int GetCombatBonusVsLargerCiv() const
	{
		return m_iCombatBonusVsLargerCiv;
	};
	int GetLandUnitMaintenanceModifier() const
	{
		return m_iLandUnitMaintenanceModifier;
	};
	int GetNavalUnitMaintenanceModifier() const
	{
		return m_iNavalUnitMaintenanceModifier;
	};
	int GetRazeSpeedModifier() const
	{
		return m_iRazeSpeedModifier;
	};
	int GetDOFGreatPersonModifier() const
	{
		return m_iDOFGreatPersonModifier;
	};
	int GetLuxuryHappinessRetention() const
	{
		return m_iLuxuryHappinessRetention;
	};
#if defined(MOD_TRAITS_EXTRA_SUPPLY)
	int GetExtraSupply() const
	{
		return m_iExtraSupply;
	};
	int GetExtraSupplyPerCity() const
	{
		return m_iExtraSupplyPerCity;
	};
	int GetExtraSupplyPerPopulation() const
	{
		return m_iExtraSupplyPerPopulation;
	};
#endif
	int GetExtraSpies() const
	{
		return m_iExtraSpies;
	};
	int GetUnresearchedTechBonusFromKills() const
	{
		return m_iUnresearchedTechBonusFromKills;
	}
	int GetExtraFoundedCityTerritoryClaimRange () const
	{
		return m_iExtraFoundedCityTerritoryClaimRange;
	}
#if defined(MOD_BALANCE_CORE)
	int GetExtraConqueredCityTerritoryClaimRange() const
	{
		return m_iExtraConqueredCityTerritoryClaimRange;
	}
	int GetExtraTenetsFirstAdoption() const
	{
		return m_iExtraTenetsFirstAdoption;
	}
#endif
	int GetFreeSocialPoliciesPerEra() const
	{
		return m_iFreeSocialPoliciesPerEra;
	}
	int GetNumTradeRoutesModifier() const
	{
		return m_iNumTradeRoutesModifier;
	}
	int GetTradeRouteResourceModifier() const
	{
		return m_iTradeRouteResourceModifier;
	}
	int GetUniqueLuxuryCities() const
	{
		return m_iUniqueLuxuryCities;
	}
	int GetUniqueLuxuryQuantity() const
	{
		return m_iUniqueLuxuryQuantity;
	}
	int GetWorkerSpeedModifier() const
	{
		return m_iWorkerSpeedModifier;
	}
	int GetAfraidMinorPerTurnInfluence() const
	{
		return m_iAfraidMinorPerTurnInfluence;
	}
	int GetLandTradeRouteRangeBonus() const
	{
		return m_iLandTradeRouteRangeBonus;
	}
#if defined(MOD_TRAITS_TRADE_ROUTE_BONUSES)
	int GetSeaTradeRouteRangeBonus() const
	{
		return m_iSeaTradeRouteRangeBonus;
	}
#endif
	int GetTradeReligionModifier() const
	{
		return m_iTradeReligionModifier;
	}
	int GetTradeBuildingModifier() const
	{
		return m_iTradeBuildingModifier;
	}
#if defined(MOD_BALANCE_CORE)
	int GetNumFreeBuildings() const
	{
		return m_iNumFreeBuildings;
	}
#endif
#if defined(MOD_BALANCE_CORE_AFRAID_ANNEX)
	bool IsBullyAnnex() const
	{
		return m_bBullyAnnex;
	};
	int GetBullyYieldMultiplierAnnex() const
	{
		return m_iBullyYieldMultiplierAnnex;
	}
#endif

	bool IsFightWellDamaged() const
	{
		return m_bFightWellDamaged;
	};
	bool IsWoodlandMovementBonus() const
	{
		return m_bWoodlandMovementBonus;
	};
	bool IsRiverMovementBonus() const
	{
		return m_bRiverMovementBonus;
	};
	bool IsFasterInHills() const
	{
		return m_bFasterInHills;
	};
	bool IsEmbarkedAllWater() const
	{
		return m_bEmbarkedAllWater;
	};
	bool IsEmbarkedToLandFlatCost() const
	{
		return m_bEmbarkedToLandFlatCost;
	};
	bool IsNoHillsImprovementMaintenance() const
	{
		return m_bNoHillsImprovementMaintenance;
	};
	bool IsTechBoostFromCapitalScienceBuildings() const
	{
		return m_bTechBoostFromCapitalScienceBuildings;
	};
	bool IsStaysAliveZeroCities() const
	{
		return m_bStaysAliveZeroCities;
	};
	bool IsFaithFromUnimprovedForest() const
	{
		return m_bFaithFromUnimprovedForest;
	};
#if defined(MOD_TRAITS_ANY_BELIEF)
	bool IsAnyBelief() const
	{
		return m_bAnyBelief;
	};
	bool IsAlwaysReligion() const
	{
		return m_bAlwaysReligion;
	};

	bool IsIgnoreTradeDistanceScaling() const
	{
		return m_bIgnoreTradeDistanceScaling;
	};
	bool IsCanPlunderWithoutWar() const
	{
		return m_bCanPlunderWithoutWar;
	};
#endif
	bool IsBonusReligiousBelief() const
	{
		return m_bBonusReligiousBelief;
	};
	bool IsAbleToAnnexCityStates() const
	{
		return m_bAbleToAnnexCityStates;
	};
	bool IsCrossesMountainsAfterGreatGeneral() const
	{
		return m_bCrossesMountainsAfterGreatGeneral;
	};
#if defined(MOD_TRAITS_CROSSES_ICE)
	bool IsCrossesIce() const
	{
		return m_bCrossesIce;
	};
#endif
#if defined(MOD_TRAITS_GG_FROM_BARBARIANS)
	bool IsGGFromBarbarians() const
	{
		return m_bGGFromBarbarians;
	};
#endif
	bool IsMayaCalendarBonuses() const
	{
		return m_bMayaCalendarBonuses;
	};
	bool IsNoAnnexing() const
	{
		return m_bNoAnnexing;
	};
	bool IsTechFromCityConquer() const
	{
		return m_bTechFromCityConquer;
	};
	bool IsUniqueLuxuryRequiresNewArea() const
	{
		return m_bUniqueLuxuryRequiresNewArea;
	}
	bool WillGetUniqueLuxury(CvArea *pArea) const;
	bool IsRiverTradeRoad() const
	{
		return m_bRiverTradeRoad;
	}
	bool IsAngerFreeIntrusionOfCityStates() const
	{
		return m_bAngerFreeIntrusionOfCityStates;
	}
#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
	int GetPovertyHappinessChange() const
	{
		return m_iPovertyHappinessChange;
	}
	int GetDefenseHappinessChange() const
	{
		return m_iDefenseHappinessChange;
	}
	int GetIlliteracyHappinessChange() const
	{
		return m_iIlliteracyHappinessChange;
	}
	int GetUnculturedHappinessChange() const
	{
		return m_iUnculturedHappinessChange;
	}
	int GetMinorityHappinessChange() const
	{
		return m_iMinorityHappinessChange;
	}
	bool IsNoConnectionUnhappiness() const
	{
		return m_bNoConnectionUnhappiness;
	}
	bool IsNoReligiousStrife() const
	{
		return m_bIsNoReligiousStrife;
	}
	bool IsOddEraScaler() const
	{
		return m_bIsOddEraScaler;
	}
	int GetWonderProductionModGA() const
	{
		return m_iWonderProductionModGA;
	}
	int GetCultureBonusModifierConquest() const
	{
		return m_iCultureBonusModifierConquest;
	};
	int GetProductionBonusModifierConquest() const
	{
		return m_iProductionBonusModifierConquest;
	};
#endif

	int GetExtraYieldThreshold(YieldTypes eYield) const
	{
		return m_iExtraYieldThreshold[(int)eYield];
	};
	int GetFreeCityYield(YieldTypes eYield) const
	{
		return m_iFreeCityYield[(int)eYield];
	};
	int GetYieldChangeStrategicResources(YieldTypes eYield) const
	{
		return m_iYieldChangeStrategicResources[(int)eYield];
	};
	int GetYieldChangeNaturalWonder(YieldTypes eYield) const
	{
		return m_iYieldChangeNaturalWonder[(int)eYield];
	};
	int GetYieldChangePerTradePartner(YieldTypes eYield) const
	{
		return m_iYieldChangePerTradePartner[(int)eYield];
	};
	int GetYieldChangeIncomingTradeRoute(YieldTypes eYield) const
	{
		return m_iYieldChangeIncomingTradeRoute[(int)eYield];
	};
	int GetYieldRateModifier(YieldTypes eYield) const
	{
		return m_iYieldRateModifier[(int)eYield];
	};
	int GetStrategicResourceQuantityModifier(TerrainTypes eTerrain) const
	{
		return m_iStrategicResourceQuantityModifier[(int)eTerrain];
	};
	int GetResourceQuantityModifier(ResourceTypes eResource) const
	{
		return ((uint)eResource < m_aiResourceQuantityModifier.size())?m_aiResourceQuantityModifier[(int)eResource]:0;
	};
#if defined(MOD_BALANCE_CORE)
	int GetGreatPersonCostReduction(GreatPersonTypes eGreatPerson) const
	{
		return ((uint)eGreatPerson < m_aiGreatPersonCostReduction.size()) ? m_aiGreatPersonCostReduction[(int)eGreatPerson] : 0;
	};
	int GetPerPuppetGreatPersonRateModifier(GreatPersonTypes eGreatPerson) const
	{
		return ((uint)eGreatPerson < m_aiPerPuppetGreatPersonRateModifier.size()) ? m_aiPerPuppetGreatPersonRateModifier[(int)eGreatPerson] : 0;
	};
	int GetGreatPersonGWAM(GreatPersonTypes eGreatPerson) const
	{
		return ((uint)eGreatPerson < m_aiGreatPersonGWAM.size()) ? m_aiGreatPersonGWAM[(int)eGreatPerson] : 0;
	};
	int GetGoldenAgeGreatPersonRateModifier(GreatPersonTypes eGreatPerson) const
	{
		return ((uint)eGreatPerson < m_aiGoldenAgeGreatPersonRateModifier.size()) ? m_aiGoldenAgeGreatPersonRateModifier[(int)eGreatPerson] : 0;
	};

	int GetGoldenAgeFromGreatPersonBirth(GreatPersonTypes eGreatPerson) const
	{
		return ((uint)eGreatPerson < m_aiGoldenAgeFromGreatPersonBirth.size()) ? m_aiGoldenAgeFromGreatPersonBirth[(int)eGreatPerson] : 0;
	};

	int GetNumPledgeDomainProductionModifier(DomainTypes eDomain) const
	{
		return ((uint)eDomain < m_aiNumPledgesDomainProdMod.size()) ? m_aiNumPledgesDomainProdMod[(int)eDomain] : 0;
	};
	int GetDomainFreeExperienceModifier(DomainTypes eDomain) const
	{
		return ((uint)eDomain < m_aiDomainFreeExperienceModifier.size()) ? m_aiDomainFreeExperienceModifier[(int)eDomain] : 0;
	};
	int GetGreatPersonProgressFromPolicyUnlock(GreatPersonTypes eIndex) const
	{
		return ((uint)eIndex < m_aiGreatPersonProgressFromPolicyUnlock.size()) ? m_aiGreatPersonProgressFromPolicyUnlock[(int)eIndex] : 0;
	};
	int GetFreeUnitClassesDOW(UnitClassTypes eUnitClass) const
	{
		return ((uint)eUnitClass < m_aiFreeUnitClassesDOW.size()) ? m_aiFreeUnitClassesDOW[(int)eUnitClass] : 0;
	}
#endif
	int GetMovesChangeUnitCombat(const int unitCombatID) const;
#if defined(MOD_BALANCE_CORE)
	int GetMovesChangeUnitClass(const int unitClassID) const;
	int GetYieldChangeFromTileEarnTerrainType(TerrainTypes eTerrain, YieldTypes eYield) const;
	int GetYieldChangeFromTilePurchaseTerrainType(TerrainTypes eTerrain, YieldTypes eYield) const;
	int GetYieldChangeFromTileConquest(TerrainTypes eTerrain, YieldTypes eYield) const;
	int GetYieldChangeFromTileCultureBomb(TerrainTypes eTerrain, YieldTypes eYield) const;
	int GetYieldChangeFromTileStealCultureBomb(TerrainTypes eTerrain, YieldTypes eYield) const;
	int GetYieldChangeFromTileSettle(TerrainTypes eTerrain, YieldTypes eYield) const;
	int GetYieldChangePerImprovementBuilt(ImprovementTypes eImprovement, YieldTypes eYield) const;
	vector<ImprovementTypes> GetImprovementTypesWithYieldChange() const;
	void UpdateYieldChangeImprovementTypes();
	int GetYieldFromBarbarianCampClear(YieldTypes eYield, bool bEraScaling) const;
#endif
	int GetMaintenanceModifierUnitCombat(const int unitCombatID) const;
	int GetImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYield) const;
#if defined(MOD_API_UNIFIED_YIELDS) && defined(MOD_API_PLOT_YIELDS)
	int GetPlotYieldChange(PlotTypes ePlot, YieldTypes eYield) const;
#endif
#if defined(MOD_BALANCE_CORE)
	int GetYieldFromLevelUp(YieldTypes eYield) const
	{
		return m_iYieldFromLevelUp[(int)eYield];
	};
	int GetYieldFromHistoricEvent(YieldTypes eYield) const
	{
		return m_iYieldFromHistoricEvent[(int)eYield];
	};
	int GetYieldFromOwnPantheon(YieldTypes eYield) const
	{
		return m_iYieldFromOwnPantheon[(int)eYield];
	};
	int GetTradeRouteStartYield(YieldTypes eYield) const
	{
		return m_iTradeRouteStartYield[(int)eYield];
	};
	int GetYieldFromRouteMovement(YieldTypes eYield) const
	{
		return m_iYieldFromRouteMovement[(int)eYield];
	};
	int GetYieldFromExport(YieldTypes eYield) const
	{
		return m_iYieldFromExport[(int)eYield];
	};
	int GetYieldFromImport(YieldTypes eYield) const
	{
		return m_iYieldFromImport[(int)eYield];
	};
	int GetYieldFromTilePurchase(YieldTypes eYield) const
	{
		return m_iYieldFromTilePurchase[(int)eYield];
	};
	int GetYieldFromTileEarn(YieldTypes eYield) const
	{
		return m_iYieldFromTileEarn[(int)eYield];
	};
	int GetYieldFromCSAlly(YieldTypes eYield) const
	{
		return m_iYieldFromCSAlly[(int)eYield];
	};
	int GetYieldFromCSFriend(YieldTypes eYield) const
	{
		return m_iYieldFromCSFriend[(int)eYield];
	};
	int GetYieldFromSettle(YieldTypes eYield) const
	{
		return m_iYieldFromSettle[(int)eYield];
	};
	int GetYieldFromConquest(YieldTypes eYield) const
	{
		return m_iYieldFromConquest[(int)eYield];
	};
	int GetVotePerXCSAlliance() const
	{
		return m_iVotePerXCSAlliance;
	};
	int GetVotePerXCSFollowingYourReligion() const
	{
		return m_iVotePerXCSFollowingFollowingYourReligion;
	};
	int GetChanceToConvertReligiousUnits() const
	{
		return m_iChanceToConvertReligiousUnits;
	};
	int GetGoldenAgeFromVictory() const
	{
		return m_iGoldenAgeFromVictory;
	};
	int GetFreePolicyPerXTechs() const
	{
		return m_iFreePolicyPerXTechs;
	};
	EraTypes GetGPFaithPurchaseEra() const
	{
		return m_eGPFaithPurchaseEra;
	};
	int GetFaithCostModifier() const
	{
		return m_iFaithCostModifier;
	};
	bool IsFreeGreatWorkOnConquest() const
	{
		return m_bFreeGreatWorkOnConquest;
	};
	bool IsPopulationBoostReligion() const
	{
		return m_bPopulationBoostReligion;
	};
	bool IsCombatBoostNearNaturalWonder() const
	{
		return m_bCombatBoostNearNaturalWonder;
	};
	int GetGAPToYield(YieldTypes eYield) const
	{
		return m_iGAPToYield[(int)eYield];
	};
	int GetMountainRangeYield(YieldTypes eYield) const
	{
		return m_iMountainRangeYield[(int)eYield];
	};
#endif
#if defined(MOD_BALANCE_CORE) && defined(MOD_TRAITS_YIELD_FROM_ROUTE_MOVEMENT_IN_FOREIGN_TERRITORY)
	int GetYieldFromRouteMovementInForeignTerritory(YieldTypes eYield, bool bTradePartner) const;
#endif
#if defined(MOD_API_UNIFIED_YIELDS)
	int GetBuildingClassYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield) const;
	int GetCapitalYieldChanges(YieldTypes eYield) const
	{
		return m_iCapitalYieldChanges[(int)eYield];
	};
	int GetCityYieldChanges(YieldTypes eYield) const
	{
		return m_iCityYieldChanges[(int)eYield];
	};
	int GetPermanentYieldChangeWLTKD(YieldTypes eYield) const
	{
		return m_iPermanentYieldChangeWLTKD[(int)eYield];
	};
	int GetCoastalCityYieldChanges(YieldTypes eYield) const
	{
		return m_iCoastalCityYieldChanges[(int)eYield];
	};
	int GetGreatWorkYieldChanges(YieldTypes eYield) const
	{
		return m_iGreatWorkYieldChanges[(int)eYield];
	};
	int GetArtifactYieldChanges(YieldTypes eYield) const
	{
		return m_iArtifactYieldChanges[(int)eYield];
	};
	int GetArtYieldChanges(YieldTypes eYield) const
	{
		return m_iArtYieldChanges[(int)eYield];
	};
	int GetLitYieldChanges(YieldTypes eYield) const
	{
		return m_iLitYieldChanges[(int)eYield];
	};
	int GetMusicYieldChanges(YieldTypes eYield) const
	{
		return m_iMusicYieldChanges[(int)eYield];
	};
	int GetSeaPlotYieldChanges(YieldTypes eYield) const
	{
		return m_iSeaPlotYieldChanges[(int)eYield];
	};

	int GetFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYield) const;
	int GetResourceYieldChange(ResourceTypes eResource, YieldTypes eYield) const;
	int GetTerrainYieldChange(TerrainTypes eTerrain, YieldTypes eYield) const;
	int GetYieldFromKills(YieldTypes eYield) const;
	int GetYieldFromBarbarianKills(YieldTypes eYield) const;
	int GetYieldChangeTradeRoute(YieldTypes eYield) const
	{
		return m_iYieldChangeTradeRoute[(int)eYield];
	};
	int GetYieldChangeWorldWonder(YieldTypes eYield) const
	{
		return m_iYieldChangeWorldWonder[(int)eYield];
	};
	int GetTradeRouteYieldChange(DomainTypes eDomain, YieldTypes eYield) const;
#endif
	int GetSpecialistYieldChange(SpecialistTypes eSpecialist, YieldTypes eYield) const;
#if defined(MOD_API_UNIFIED_YIELDS)
	int GetGreatPersonExpendedYield(GreatPersonTypes eGreatPerson, YieldTypes eYield) const;
	int GetGreatPersonBornYield(GreatPersonTypes eGreatPerson, YieldTypes eYield) const;
	
	int GetCityYieldFromUnimprovedFeature(FeatureTypes eFeature, YieldTypes eYield) const;
#endif
	int GetUnimprovedFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYield) const;
	FreeResourceXCities GetFreeResourceXCities(ResourceTypes eResource) const;

	bool HasFreePromotionUnitCombat(const int promotionID, const int unitCombatID) const;
#if defined(MOD_BALANCE_CORE)
	bool HasSpecialUnitUpgrade(const int unitClassID, const int unitID) const;
	bool HasFreePromotionUnitClass(const int promotionID, const int unitClassID) const;
	bool HasUnitClassCanBuild(const int buildID, const int unitClassID) const;	
	int GetGoldenAgeYieldModifier(YieldTypes eYield) const;
	std::pair <int, bool> GetUnitCombatProductionCostModifier(UnitCombatTypes eUnitCombat) const;
	int GetNonSpecialistFoodChange() const
	{
		return m_iNonSpecialistFoodChange;
	}
	bool IsNoBuild(const BuildTypes eBuild) const;
	int GetDomainProductionModifiersPerSpecialist(DomainTypes eDomain) const;
#endif
#if defined(MOD_TRAITS_TRADE_ROUTE_PRODUCTION_SIPHON)
	TradeRouteProductionSiphon GetTradeRouteProductionSiphon(bool bInternationalOnly) const;
	bool IsTradeRouteProductionSiphon() const;
#endif

	// Public functions to make trait-based game state changes
	void AddUniqueLuxuries(CvCity *pCity);
#if defined(MOD_BALANCE_CORE)
	bool AddUniqueLuxuriesAround(CvCity *pCity, int iNumResource);
#endif
#if defined(MOD_EVENTS_UNIT_CAPTURE)
	bool CheckForBarbarianConversion(CvUnit* pByUnit, CvPlot* pPlot);
#else
	bool CheckForBarbarianConversion(CvPlot* pPlot);
#endif
	int GetCapitalBuildingDiscount(BuildingTypes eBuilding);
#if defined(MOD_BALANCE_CORE)
	TechTypes GetFreeBuildingPrereqTech() const;
	TechTypes GetCapitalFreeBuildingPrereqTech() const;
	int GetWonderProductionToBuildingDiscount(BuildingTypes eBuilding);
	void SpawnBestUnitsOnImprovementDOW(CvCity *pCity);
#endif
	BuildingTypes GetFreeBuilding() const;
#if defined(MOD_BALANCE_CORE)
	BuildingTypes GetFreeCapitalBuilding() const;
	UnitTypes GetFreeUnitOnConquest() const;
#endif
	BuildingTypes GetFreeBuildingOnConquest() const;
	void SetDefeatedBarbarianCampGuardType(UnitTypes eType)
	{
		m_eCampGuardType = eType;
	};
	UnitTypes GetDefeatedBarbarianCampGuardType() const
	{
		return m_eCampGuardType;
	};
	void SetCombatBonusImprovementType(ImprovementTypes eType)
	{
		m_eCombatBonusImprovement = eType;
	};
	ImprovementTypes GetCombatBonusImprovementType() const
	{
		return m_eCombatBonusImprovement;
	};

	bool IsAbleToCrossMountainsWithGreatGeneral() const;

#if defined(MOD_TRAITS_CROSSES_ICE)
	bool IsAbleToCrossIce() const;
	bool TerrainClaimBoost(TerrainTypes eTerrain);
#endif

	bool NoTrain(UnitClassTypes eUnitClassType);

	// Maya calendar routines
	bool IsUsingMayaCalendar() const;
	bool IsEndOfMayaLongCount();
	CvString GetMayaCalendarString();
	CvString GetMayaCalendarLongString();
	void ChooseMayaBoost();
	void ComputeMayaDate();
	int GetUnitBaktun(UnitTypes eUnit) const;
	void SetUnitBaktun(UnitTypes eUnit);
	bool IsFreeMayaGreatPersonChoice() const;
#if defined(MOD_BALANCE_CORE_MAYA_CHANGE)
	bool IsProphetValid() const;
#endif
	// Serialization
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

	const std::vector<TraitTypes> GetPotentiallyActiveTraits() { return m_vPotentiallyActiveLeaderTraits; }

private:
#if defined(MOD_EVENTS_UNIT_CAPTURE)
	bool ConvertBarbarianCamp(CvUnit* pByUnit, CvPlot* pPlot);
	bool ConvertBarbarianNavalUnit(CvUnit* pByUnit, CvUnit* pUnit);
#else
	bool ConvertBarbarianCamp(CvPlot* pPlot);
	bool ConvertBarbarianNavalUnit(CvUnit* pUnit);
#endif

	CvTraitXMLEntries* m_pTraits;
	CvPlayer* m_pPlayer;
	std::vector<bool> m_vLeaderHasTrait;
	std::vector<TraitTypes> m_vPotentiallyActiveLeaderTraits;

	bool m_bIsWarmonger;
	bool m_bIsNerd;
	bool m_bIsTourism;
	bool m_bIsDiplomat;
	bool m_bIsExpansionist;
	bool m_bIsSmaller;
	bool m_bIsReligious;

	// Cached data about this player's traits
	int m_iGreatPeopleRateModifier;
	int m_iGreatScientistRateModifier;
	int m_iGreatGeneralRateModifier;
	int m_iGreatGeneralExtraBonus;
	int m_iGreatPersonGiftInfluence;
	int m_iLevelExperienceModifier;
	int m_iMaxGlobalBuildingProductionModifier;
	int m_iMaxTeamBuildingProductionModifier;
	int m_iMaxPlayerBuildingProductionModifier;
	int m_iCityUnhappinessModifier;
	int m_iPopulationUnhappinessModifier;
	int m_iCityStateBonusModifier;
	int m_iCityStateFriendshipModifier;
	int m_iCityStateCombatModifier;
	int m_iLandBarbarianConversionPercent;
	int m_iLandBarbarianConversionExtraUnits;
	int m_iSeaBarbarianConversionPercent;
	int m_iCapitalBuildingModifier;
	int m_iPlotBuyCostModifier;
	int m_iNationalPopReqModifier;
#if defined(MOD_TRAITS_CITY_WORKING)
	int m_iCityWorkingChange;
#endif
#if defined(MOD_TRAITS_CITY_AUTOMATON_WORKERS)
	int m_iCityAutomatonWorkersChange;
#endif
	int m_iPlotCultureCostModifier;
	int m_iCultureFromKills;
	int m_iFaithFromKills;
	int m_iCityCultureBonus;
	int m_iCapitalThemingBonusModifier;
	int m_iPolicyCostModifier;
	int m_iCityConnectionTradeRouteChange;
	int m_iWonderProductionModifier;
	int m_iPlunderModifier;
	int m_iImprovementMaintenanceModifier;
#if defined(MOD_CIV6_WORKER)
	int m_iRouteBuilderCostModifier;
#endif
	int m_iGoldenAgeDurationModifier;
	int m_iGoldenAgeMoveChange;
	int m_iGoldenAgeCombatModifier;
	int m_iGoldenAgeTourismModifier;
	int m_iGoldenAgeGreatArtistRateModifier;
	int m_iGoldenAgeGreatMusicianRateModifier;
	int m_iGoldenAgeGreatWriterRateModifier;
	int m_iExtraEmbarkMoves;
	int m_iNaturalWonderFirstFinderGold;
	int m_iNaturalWonderSubsequentFinderGold;
	int m_iNaturalWonderYieldModifier;
	int m_iNaturalWonderHappinessModifier;
	int m_iNearbyImprovementCombatBonus;
	int m_iNearbyImprovementBonusRange;
	int m_iCultureBuildingYieldChange;
#if defined(MOD_BALANCE_CORE)
	int m_iWarWearinessModifier;
	int m_iEnemyWarWearinessModifier;
	int m_iCombatBonusVsHigherPop;
	bool m_bBuyOwnedTiles;
	bool m_bReconquista;
	bool m_bNoSpread;
	int m_iInspirationalLeader;
	int m_iBullyMilitaryStrengthModifier;
	int m_iBullyValueModifier;
	bool m_bIgnoreBullyPenalties;
	bool m_bDiplomaticMarriage;
	bool m_bAdoptionFreeTech;
	bool m_bGPWLTKD;
	bool m_bGreatWorkWLTKD;
	bool m_bExpansionWLTKD;
	bool m_bTradeRouteOnly;
	bool m_bKeepConqueredBuildings;
	bool m_bMountainPass;
	bool m_bUniqueBeliefsOnly;
	bool m_bNoNaturalReligionSpread;
	bool m_bNoOpenTrade;
	bool m_bGoldenAgeOnWar;
	bool m_bUnableToCancelRazing;
	int m_iTourismToGAP;
	int m_iGoldToGAP;
	int m_iInfluenceMeetCS;
	int m_iMultipleAttackBonus;
	int m_iCityConquestGWAM;
	int m_iEventTourismBoost;
	int m_iWLTKDGPImprovementModifier;
	int m_iGrowthBoon;
	int m_iAllianceCSDefense;
	int m_iAllianceCSStrength;
	int m_iTourismGABonus;
	int m_iEventGP;
	int m_iWLTKDCulture;
	int m_iWLTKDGATimer;
	int m_iGAUnhappinesNeedMod;
	int m_iStartingSpies;
	int m_iStartingSpyRank;
	int m_iSpyMoveRateBonus;
	int m_iEspionageModifier;
	int m_iSpyExtraRankBonus;
	int m_iQuestYieldModifier;
	int m_iWonderProductionModifierToBuilding;
	int m_iPolicyGEorGM;
	int m_iGAGarrisonCityRangeStrikeModifier;
	bool m_bBestUnitSpawnOnImpDOW;
	ImprovementTypes m_iBestUnitImprovement;
	int m_iGGGARateFromDenunciationsAndWars;
	bool m_bTradeRouteMinorInfluenceAP;
	bool m_bProdModFromNumSpecialists;
	int m_iConquestOfTheWorldCityAttack;
	bool m_bConquestOfTheWorld;
	bool m_bFreeUpgrade;
	bool m_bWarsawPact;
	int m_iEnemyWarSawPactPromotion;
	bool m_bFreeZuluPikemanToImpi;
	bool m_bPermanentYieldsDecreaseEveryEra;
	bool m_bImportsCountTowardsMonopolies;
	bool m_bCanPurchaseNavalUnitsFaith;
	int m_iPuppetPenaltyReduction;
	int m_iSharedReligionTourismModifier;
	int m_iExtraMissionaryStrength;
	bool m_bCanGoldInternalTradeRoutes;
	int m_iExtraTradeRoutesPerXOwnedCities;
	int m_iExtraTradeRoutesPerXOwnedVassals;
	bool m_bIsCapitalOnly;
#endif
#if defined(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
	int m_iInvestmentModifier;
#endif
	int m_iCombatBonusVsHigherTech;
	int m_iCombatBonusVsLargerCiv;
	int m_iLandUnitMaintenanceModifier;
	int m_iNavalUnitMaintenanceModifier;
	int m_iRazeSpeedModifier;
	int m_iDOFGreatPersonModifier;
	int m_iLuxuryHappinessRetention;
#if defined(MOD_TRAITS_EXTRA_SUPPLY)
	int m_iExtraSupply;
	int m_iExtraSupplyPerCity;
	int m_iExtraSupplyPerPopulation;
#endif
	int m_iExtraSpies;
	int m_iUnresearchedTechBonusFromKills;
	int m_iExtraFoundedCityTerritoryClaimRange;
#if defined(MOD_BALANCE_CORE)
	int m_iExtraConqueredCityTerritoryClaimRange;
	int m_iExtraTenetsFirstAdoption;
#endif
	int m_iFreeSocialPoliciesPerEra;
	int m_iNumTradeRoutesModifier;
	int m_iTradeRouteResourceModifier;
	int m_iUniqueLuxuryCities;
	int m_iUniqueLuxuryQuantity;
	int m_iUniqueLuxuryCitiesPlaced;  
	int m_iWorkerSpeedModifier;
	int m_iAfraidMinorPerTurnInfluence; 
	int m_iLandTradeRouteRangeBonus;
#if defined(MOD_TRAITS_TRADE_ROUTE_BONUSES)
	int m_iSeaTradeRouteRangeBonus;
#endif
	int m_iTradeReligionModifier;
	int m_iTradeBuildingModifier;
#if defined(MOD_BALANCE_CORE)
	int m_iNumFreeBuildings;
#endif
	// Saved

	bool m_bFightWellDamaged;
	bool m_bWoodlandMovementBonus;
	bool m_bRiverMovementBonus;
	bool m_bFasterInHills;
	bool m_bEmbarkedAllWater;
	bool m_bEmbarkedToLandFlatCost;
	bool m_bNoHillsImprovementMaintenance;
	bool m_bTechBoostFromCapitalScienceBuildings;
	bool m_bStaysAliveZeroCities;
	bool m_bFaithFromUnimprovedForest;
#if defined(MOD_TRAITS_ANY_BELIEF)
	bool m_bAnyBelief;
	bool m_bAlwaysReligion;
	bool m_bIgnoreTradeDistanceScaling;
	bool m_bCanPlunderWithoutWar;
#endif
	bool m_bBonusReligiousBelief;
	bool m_bAbleToAnnexCityStates;
	bool m_bCrossesMountainsAfterGreatGeneral;
#if defined(MOD_TRAITS_CROSSES_ICE)
	bool m_bCrossesIce;
#endif
#if defined(MOD_TRAITS_GG_FROM_BARBARIANS)
	bool m_bGGFromBarbarians;
#endif
	bool m_bMayaCalendarBonuses;
	bool m_bNoAnnexing;
	bool m_bTechFromCityConquer;
	bool m_bUniqueLuxuryRequiresNewArea;
	bool m_bRiverTradeRoad;
	bool m_bAngerFreeIntrusionOfCityStates;
#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
	int m_iPovertyHappinessChange;
	int m_iDefenseHappinessChange;
	int m_iUnculturedHappinessChange;
	int m_iIlliteracyHappinessChange;
	int m_iMinorityHappinessChange;
	bool m_bNoConnectionUnhappiness;
	bool m_bIsNoReligiousStrife;
	bool m_bIsOddEraScaler;
	int m_iWonderProductionModGA;
#endif

	UnitTypes m_eCampGuardType;
	unsigned int m_uiFreeUnitIndex;
	TechTypes m_eFreeUnitPrereqTech;
	ImprovementTypes m_eCombatBonusImprovement;
	BuildingTypes m_eFreeBuilding;
#if defined(MOD_BALANCE_CORE)
	BuildingTypes m_eFreeCapitalBuilding;
	UnitTypes m_eFreeUnitOnConquest;
#endif
	BuildingTypes m_eFreeBuildingOnConquest;
#if defined(MOD_BALANCE_CORE_AFRAID_ANNEX)
	bool m_bBullyAnnex;
	int m_iBullyYieldMultiplierAnnex;
	std::vector<bool> m_abTerrainClaimBoost;
#endif
	int m_iExtraYieldThreshold[NUM_YIELD_TYPES];
	int m_iFreeCityYield[NUM_YIELD_TYPES];
	int m_iYieldChangeStrategicResources[NUM_YIELD_TYPES];
	int m_iYieldChangeNaturalWonder[NUM_YIELD_TYPES];
	int m_iYieldChangePerTradePartner[NUM_YIELD_TYPES];
	int m_iYieldChangeIncomingTradeRoute[NUM_YIELD_TYPES];
	int m_iYieldRateModifier[NUM_YIELD_TYPES];
	int m_iStrategicResourceQuantityModifier[NUM_TERRAIN_TYPES];
	std::vector<int> m_aiResourceQuantityModifier;
	std::vector<bool> m_abNoTrain;
	FStaticVector<FreeTraitUnit, SAFE_ESTIMATE_NUM_FREE_UNITS, true, c_eCiv5GameplayDLL, 0> m_aFreeTraitUnits;
	std::vector<int> m_aUniqueLuxuryAreas;

	// Maya calendar bonus data
	int m_iBaktunPreviousTurn;  // Saved
	int m_iBaktun;	 // Not saved
	int m_iKatun;    // Not saved
	int m_iTun;      // Not saved
	int m_iWinal;    // Not saved
	int m_iKin;      // Not saved
	std::vector<MayaBonusChoice> m_aMayaBonusChoices;  // Saved

	std::vector<int> m_paiMovesChangeUnitCombat;
	std::vector<int> m_paiMaintenanceModifierUnitCombat;
#if defined(MOD_BALANCE_CORE)
	std::vector<int> m_paiMovesChangeUnitClass;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiYieldFromTileEarnTerrainType;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiYieldFromTilePurchaseTerrainType;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiYieldFromTileConquest;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiYieldFromTileCultureBomb;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiYieldFromTileStealCultureBomb;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiYieldFromTileSettle;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppaaiYieldChangePerImprovementBuilt;
	vector<ImprovementTypes> m_vYieldChangeImprovementTypes; //not serialized, built on the fly
	std::map<int, std::map<bool, int>> m_pbiYieldFromBarbarianCampClear;
#endif

	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppaaiImprovementYieldChange;
#if defined(MOD_API_UNIFIED_YIELDS) && defined(MOD_API_PLOT_YIELDS)
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiPlotYieldChange;
#endif
#if defined(MOD_BALANCE_CORE)
	int m_iYieldFromLevelUp[NUM_YIELD_TYPES];
	int m_iYieldFromHistoricEvent[NUM_YIELD_TYPES];
	int m_iYieldFromOwnPantheon[NUM_YIELD_TYPES];
	int m_iTradeRouteStartYield[NUM_YIELD_TYPES];
	int m_iYieldFromRouteMovement[NUM_YIELD_TYPES];
	int m_iYieldFromExport[NUM_YIELD_TYPES];
	int m_iYieldFromImport[NUM_YIELD_TYPES];
	int m_iYieldFromTilePurchase[NUM_YIELD_TYPES];
	int m_iYieldFromTileEarn[NUM_YIELD_TYPES];
	int m_iYieldFromCSAlly[NUM_YIELD_TYPES];
	int m_iYieldFromCSFriend[NUM_YIELD_TYPES];
	int m_iYieldFromSettle[NUM_YIELD_TYPES];
	int m_iYieldFromConquest[NUM_YIELD_TYPES];
	std::map<int, int> m_aiGoldenAgeYieldModifier;
	std::map<int, std::pair<int, bool>> m_aibUnitCombatProductionCostModifier;
	int m_iNonSpecialistFoodChange;
	std::vector<int> m_aiNoBuilds;
	std::map<int, int> m_aiDomainProductionModifiersPerSpecialist;
	int m_iVotePerXCSAlliance;
	int m_iVotePerXCSFollowingFollowingYourReligion;
	int m_iChanceToConvertReligiousUnits;
	int m_iGoldenAgeFromVictory;
	int m_iFreePolicyPerXTechs;
	EraTypes m_eGPFaithPurchaseEra;
	int m_iFaithCostModifier;
	bool m_bFreeGreatWorkOnConquest;
	bool m_bPopulationBoostReligion;
	bool m_bCombatBoostNearNaturalWonder;
	int m_iCultureBonusModifierConquest;
	int m_iProductionBonusModifierConquest;
#endif
#if defined(MOD_BALANCE_CORE) && defined(MOD_TRAITS_YIELD_FROM_ROUTE_MOVEMENT_IN_FOREIGN_TERRITORY)
	std::map<int, std::map<bool, int>> m_pbiYieldFromRouteMovementInForeignTerritory;
#endif
#if defined(MOD_TRAITS_TRADE_ROUTE_PRODUCTION_SIPHON)
	std::map<bool, TradeRouteProductionSiphon> m_aiiTradeRouteProductionSiphon;
#endif
#if defined(MOD_API_UNIFIED_YIELDS)
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiBuildingClassYieldChange;
	int m_iCityYieldChanges[NUM_YIELD_TYPES];
	int m_iPermanentYieldChangeWLTKD[NUM_YIELD_TYPES];
	int m_iCapitalYieldChanges[NUM_YIELD_TYPES];
	int m_iCoastalCityYieldChanges[NUM_YIELD_TYPES];
	int m_iGreatWorkYieldChanges[NUM_YIELD_TYPES];
	int m_iArtifactYieldChanges[NUM_YIELD_TYPES];
	int m_iArtYieldChanges[NUM_YIELD_TYPES];
	int m_iLitYieldChanges[NUM_YIELD_TYPES];
	int m_iMusicYieldChanges[NUM_YIELD_TYPES];
	int m_iSeaPlotYieldChanges[NUM_YIELD_TYPES];
#if defined(MOD_BALANCE_CORE)
	int m_iGAPToYield[NUM_YIELD_TYPES];
	int m_iMountainRangeYield[NUM_YIELD_TYPES];
#endif
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiFeatureYieldChange;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiResourceYieldChange;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiTerrainYieldChange;
	int m_iYieldFromKills[NUM_YIELD_TYPES];
	int m_iYieldFromBarbarianKills[NUM_YIELD_TYPES];
	int m_iYieldChangeTradeRoute[NUM_YIELD_TYPES];
	int m_iYieldChangeWorldWonder[NUM_YIELD_TYPES];
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiTradeRouteYieldChange;
#endif
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppaaiSpecialistYieldChange;
#if defined(MOD_BALANCE_CORE)
	std::vector<int> m_aiDomainFreeExperienceModifier;
	std::vector<int> m_aiGreatPersonProgressFromPolicyUnlock;
#endif
#if defined(MOD_API_UNIFIED_YIELDS)
	std::vector<int> m_aiGreatPersonCostReduction;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiGreatPersonExpendedYield;
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiGreatPersonBornYield;
	std::vector<int> m_aiNumPledgesDomainProdMod;
	std::vector<int> m_aiFreeUnitClassesDOW;
	std::vector<int> m_aiGoldenAgeGreatPersonRateModifier;
	std::vector<int> m_aiPerPuppetGreatPersonRateModifier;
	std::vector<int> m_aiGreatPersonGWAM;
#if defined(MOD_BALANCE_CORE)
	std::vector<int> m_aiGoldenAgeFromGreatPersonBirth;
#endif
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppiCityYieldFromUnimprovedFeature;
#endif
	std::vector< Firaxis::Array<int, NUM_YIELD_TYPES > > m_ppaaiUnimprovedFeatureYieldChange;

	std::vector<FreeResourceXCities> m_aFreeResourceXCities;
};

#endif //CIV5_TRAIT_CLASSES_H