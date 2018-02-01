/*	-------------------------------------------------------------------------------------------------------
	? 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_PLOT_H
#define CIV5_PLOT_H

#include "LinkedList.h"

#ifdef _MSC_VER
#pragma warning ( push )
#pragma warning ( disable : 6385 ) // Possible invalid data access
#endif
#include <bitset>
#ifdef _MSC_VER
#pragma warning ( pop )
#endif

#include <array>
#include "CvUnit.h"
#include "CvCity.h"
#include "CvGlobals.h"
#include "CvGame.h"
#include "CvEnums.h"

#pragma warning( disable: 4251 )		// needs to have dll-interface to be used by clients of class

class CvArea;
class CvMap;
class CvRoute;

typedef bool (*ConstPlotUnitFunc)(const CvUnit* pUnit, int iData1, int iData2);
typedef bool (*PlotUnitFunc)(CvUnit* pUnit, int iData1, int iData2);

// please don't change this
#define NUM_INVISIBLE_TYPES 1

typedef FFastVector<IDInfo, true, c_eCiv5GameplayDLL, 0> IDInfoVector;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  STRUCT: CvArchaeologyData
//!  \brief All the archaeological data stored for a plot
//
//!  Key Attributes:
//!  - Stored in CvPlot class
//!  - Stores creation info (GP, year, era, player)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
struct CvArchaeologyData
{
	GreatWorkArtifactClass m_eArtifactType;
	EraTypes m_eEra;
	PlayerTypes m_ePlayer1;
	PlayerTypes m_ePlayer2;
	GreatWorkType m_eWork;

	CvArchaeologyData():
		m_eArtifactType(NO_GREAT_WORK_ARTIFACT_CLASS),
		m_eEra(NO_ERA),
		m_ePlayer1(NO_PLAYER),
		m_ePlayer2(NO_PLAYER),
		m_eWork(NO_GREAT_WORK)
	{}

	void Reset()
	{
		m_eArtifactType = NO_GREAT_WORK_ARTIFACT_CLASS;
		m_eEra = NO_ERA;
		m_ePlayer1 = NO_PLAYER;
		m_ePlayer2 = NO_PLAYER;
		m_eWork = NO_GREAT_WORK;
	}
};
FDataStream& operator>>(FDataStream&, CvArchaeologyData&);
FDataStream& operator<<(FDataStream&, const CvArchaeologyData&);

class CvPlot
{

public:
	CvPlot();
	~CvPlot();

	void init(int iX, int iY);
	void uninit();
	void reset(int iX = 0, int iY = 0, bool bConstructorCall=false);

	void setupGraphical();

	void erase(bool bEraseUnits);

	void doTurn();

	void doImprovement();

	FogOfWarModeTypes GetActiveFogOfWarMode() const;
	void updateFog(bool bDefer=false);
	void updateVisibility();

	void updateSymbols();

	void updateCenterUnit();

	void verifyUnitValidPlot();

	void nukeExplosion(int iDamageLevel, CvUnit* pNukeUnit = NULL);

	bool isAdjacentToArea(int iAreaID) const;
	bool isAdjacentToArea(const CvArea* pArea) const;
	bool shareAdjacentArea(const CvPlot* pPlot) const;
	bool isAdjacent(const CvPlot* pPlot) const;
	bool isAdjacentToLand() const;
	bool isAdjacentToLand_Cached() const { return m_bIsAdjacentToLand; }
	bool isDeepWater() const;
	bool isShallowWater() const;
	bool isAdjacentToShallowWater() const;
#if defined(MOD_PROMOTIONS_CROSS_ICE)
	bool isAdjacentToIce() const;
#endif
	bool isCoastalLand(int iMinWaterSize = -1) const;
	int GetSizeLargestAdjacentWater() const;

	bool isVisibleWorked() const;
	bool isWithinTeamCityRadius(TeamTypes eTeam, PlayerTypes eIgnorePlayer = NO_PLAYER) const;

	bool isLake() const;
	bool isFreshWater_cached() const;
	bool isFreshWater();
	void updateFreshwater();

	bool isRiverCrossingFlowClockwise(DirectionTypes eDirection) const;
	bool isRiverSide() const;
	bool isRiverConnection(DirectionTypes eDirection) const;

	CvPlot* getNeighboringPlot(DirectionTypes eDirection) const;
	CvPlot* getNearestLandPlotInternal(int iDistance) const;
	int getNearestLandArea() const;
	CvPlot* getNearestLandPlot() const;

	int seeFromLevel(TeamTypes eTeam) const;
	int seeThroughLevel(bool bIncludeShubbery=true) const;
#if defined(MOD_API_EXTENSIONS)
	void changeSeeFromSight(TeamTypes eTeam, DirectionTypes eDirection, int iFromLevel, bool bIncrement, InvisibleTypes eSeeInvisible, CvUnit* pUnit=NULL);
	void changeAdjacentSight(TeamTypes eTeam, int iRange, bool bIncrement, InvisibleTypes eSeeInvisible, DirectionTypes eFacingDirection, CvUnit* pUnit=NULL);
#else
	void changeSeeFromSight(TeamTypes eTeam, DirectionTypes eDirection, int iFromLevel, bool bIncrement, InvisibleTypes eSeeInvisible);
	void changeAdjacentSight(TeamTypes eTeam, int iRange, bool bIncrement, InvisibleTypes eSeeInvisible, DirectionTypes eFacingDirection, bool bBasedOnUnit=true);
#endif
	bool canSeePlot(const CvPlot* plot, TeamTypes eTeam, int iRange, DirectionTypes eFacingDirection) const;
	bool shouldProcessDisplacementPlot(int dx, int dy, int range, DirectionTypes eFacingDirection) const;
	void updateSight(bool bIncrement);
	void updateSeeFromSight(bool bIncrement, bool bRecalculate);

#if defined(MOD_API_EXTENSIONS)
	bool canHaveResource(ResourceTypes eResource, bool bIgnoreLatitude = false, bool bIgnoreCiv = false) const;
#else
	bool canHaveResource(ResourceTypes eResource, bool bIgnoreLatitude = false) const;
#endif
	bool canHaveImprovement(ImprovementTypes eImprovement, PlayerTypes ePlayer = NO_PLAYER, bool bOnlyTestVisible = false) const;

	bool canBuild(BuildTypes eBuild, PlayerTypes ePlayer = NO_PLAYER, bool bTestVisible = false, bool bTestPlotOwner = true) const;
	int getBuildTime(BuildTypes eBuild, PlayerTypes ePlayer) const;
	int getBuildTurnsTotal(BuildTypes eBuild, PlayerTypes ePlayer) const;
	int getBuildTurnsLeft(BuildTypes eBuild, PlayerTypes ePlayer, int iNowExtra, int iThenExtra) const;
	int getFeatureProduction(BuildTypes eBuild, PlayerTypes ePlayer, CvCity** ppCity) const;

	CvUnit* getBestDefender(PlayerTypes eOwner, PlayerTypes eAttackingPlayer = NO_PLAYER, const CvUnit* pAttacker = NULL, bool bTestAtWar = false, bool bIgnoreVisibility = false, bool bTestCanMove = false, bool bNoncombatAllowed = false) const;
	CvUnit* getBestGarrison(PlayerTypes eOwner) const;
	CvUnit* getSelectedUnit() const;
	int getUnitPower(PlayerTypes eOwner = NO_PLAYER) const;

	bool isFortification(TeamTypes eTeam) const;
	int defenseModifier(TeamTypes eDefender, bool bIgnoreImprovement, bool bIgnoreFeature, bool bForHelp = false) const;
	int movementCost(const CvUnit* pUnit, const CvPlot* pFromPlot, int iMovesRemaining) const;
	int MovementCostNoZOC(const CvUnit* pUnit, const CvPlot* pFromPlot, int iMovesRemaining) const;

#if defined(MOD_GLOBAL_STACKING_RULES)
	inline int getUnitLimit() const 
	{
		if(isWater())
		{
			return (GC.getPLOT_UNIT_LIMIT() + getAdditionalUnitsFromImprovement());
		}
		else		
		{
			return isCity() ? (GC.getCITY_UNIT_LIMIT() + getStackingUnits()) : (GC.getPLOT_UNIT_LIMIT() + getAdditionalUnitsFromImprovement() + getStackingUnits());
		}
	}
#endif

	inline DomainTypes getDomain() const { return isWater() ? DOMAIN_SEA : DOMAIN_LAND; }

	int getExtraMovePathCost() const;
	void changeExtraMovePathCost(int iChange);

	bool isAdjacentOwned() const;
	bool isAdjacentPlayer(PlayerTypes ePlayer, bool bLandOnly = false) const;
	bool IsAdjacentOwnedByOtherTeam(TeamTypes eTeam) const;
	bool isAdjacentTeam(TeamTypes eTeam, bool bLandOnly = false) const;
	CvCity* GetAdjacentFriendlyCity(TeamTypes eTeam, bool bLandOnly = false) const;
	CvCity* GetAdjacentCity() const;
	int GetNumAdjacentDifferentTeam(TeamTypes eTeam, bool bIgnoreWater) const;
	int GetNumAdjacentMountains() const;
	int GetNumPassableNeighbors(int iRings, PlayerTypes ePlayer, DomainTypes eDomain) const;
#if defined(MOD_BALANCE_CORE_SETTLER)
	int countPassableNeighbors(DomainTypes eDomain=NO_DOMAIN, CvPlot** aPassableNeighbors=NULL) const;
	bool IsChokePoint() const;
	bool IsLandbridge(int iMinDistanceSaved, int iMinOceanSize) const;
#endif

	void plotAction(PlotUnitFunc func, int iData1 = -1, int iData2 = -1, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM);
	int plotCount(ConstPlotUnitFunc funcA, int iData1A = -1, int iData2A = -1, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, ConstPlotUnitFunc funcB = NULL, int iData1B = -1, int iData2B = -1) const;
	CvUnit* plotCheck(ConstPlotUnitFunc funcA, int iData1A = -1, int iData2A = -1, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, ConstPlotUnitFunc funcB = NULL, int iData1B = -1, int iData2B = -1) const;

	bool isOwned() const;
	bool isBarbarian() const;
	bool isRevealedBarbarian() const;

	bool HasBarbarianCamp();

#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	bool HasDig();
#endif	

	bool isVisible(TeamTypes eTeam, bool bDebug) const;
	bool isVisible(TeamTypes eTeam) const;

	bool isActiveVisible(bool bDebug) const;
	bool isActiveVisible() const;
#if defined(MOD_BALANCE_CORE)
	bool isVisibleToCivTeam(bool bNoObserver = false, bool bNoMinor = false) const;
#else
	bool isVisibleToCivTeam() const;
#endif
	bool isVisibleToEnemy(PlayerTypes eFriendlyPlayer) const;
	bool isVisibleToWatchingHuman() const;
	bool isAdjacentVisible(TeamTypes eTeam, bool bDebug=false) const;
	bool isAdjacentNonvisible(TeamTypes eTeam) const;
	int getNumAdjacentNonvisible(TeamTypes eTeam) const;
	int getNumAdjacentOwnedBy(PlayerTypes ePlayer) const;

	bool isGoody(TeamTypes eTeam = NO_TEAM) const;
	bool isRevealedGoody(TeamTypes eTeam = NO_TEAM) const;
	void removeGoody();

	TeamTypes getTeam() const
	{
		PlayerTypes playerID = getOwner();
		if(playerID != NO_PLAYER)
		{
			return ::getTeam(playerID);
		}
		else
		{
			return NO_TEAM;
		}
	}

	bool isCity() const
	{
		if((m_plotCity.eOwner >= 0) && m_plotCity.eOwner < MAX_PLAYERS && m_plotCity.iID>0)
		{
#if defined(MOD_CORE_DEBUGGING)
			 if (MOD_CORE_DEBUGGING && (GET_PLAYER((PlayerTypes)m_plotCity.eOwner).getCity(m_plotCity.iID)) == NULL)
				 OutputDebugString("warning: inconsistent plot state! bad city ID.");
#endif
			 return true;
		}

		return false;
	}

	bool isEnemyCity(const CvUnit& kUnit) const
	{
		CvCity* pCity = getPlotCity();
		if(pCity != NULL)
			return kUnit.isEnemy(pCity->getTeam(), this);

		return false;
	}

	bool isFriendlyCity(const CvUnit& kUnit, bool bIgnored) const;
	bool isFriendlyCityOrPassableImprovement(PlayerTypes ePlayer, const CvUnit* pUnit = NULL) const;
	bool isCityOrPassableImprovement(PlayerTypes ePlayer, bool bMustBeFriendly, const CvUnit* pUnit = NULL) const;
	bool IsFriendlyTerritory(PlayerTypes ePlayer) const;

	bool isBeingWorked() const;

	bool isUnit() const;
	bool isVisibleEnemyDefender(const CvUnit* pUnit) const;
	CvUnit* getVisibleEnemyDefender(PlayerTypes ePlayer) const;
	int getNumDefenders(PlayerTypes ePlayer) const;
	int getNumNavalDefenders(PlayerTypes ePlayer) const;
	int getNumVisibleEnemyDefenders(const CvUnit* pUnit) const;
	int getNumUnitsOfAIType(UnitAITypes eType, PlayerTypes ePlayer=NO_PLAYER) const;
	CvUnit* getFirstUnitOfAITypeSameTeam(TeamTypes eTeam, UnitAITypes eType) const;
	CvUnit* getFirstUnitOfAITypeOtherTeam(TeamTypes eTeam, UnitAITypes eType) const;
	bool isVisibleEnemyUnit(PlayerTypes ePlayer) const;
	bool isVisibleEnemyUnit(const CvUnit* pUnit) const;
	bool isVisibleOtherUnit(PlayerTypes ePlayer) const;

	bool isEnemyUnit(PlayerTypes ePlayer, bool bCombat, bool bCheckVisibility, bool bIgnoreBarbs = false) const;
	bool isNeutralUnit(PlayerTypes ePlayer, bool bCombat, bool bCheckVisibility) const;

	//units which can cause or lift a blockade
	bool IsBlockadeUnit(PlayerTypes ePlayer, bool bFriendly) const;

	int getMaxFriendlyUnitsOfType(const CvUnit* pUnit, bool bBreakOnUnitLimit = true) const;

	bool isFighting() const;
	bool isUnitFighting() const;
	bool isCityFighting() const;

	bool canHaveFeature(FeatureTypes eFeature) const;

	bool isRoute() const;
	bool isValidRoute(const CvUnit* pUnit) const;

	void SetCityConnection(PlayerTypes ePlayer, bool bActive);
	bool IsCityConnection(PlayerTypes ePlayer = NO_PLAYER) const;

#if defined(MOD_BALANCE_CORE)
	void SetTradeUnitRoute(bool bActive);
	bool IsTradeUnitRoute() const;
#endif

	bool isValidDomainForLocation(const CvUnit& unit) const;
	bool isValidDomainForAction(const CvUnit& unit) const;


	inline int getX() const
	{
		return m_iX;
	}

	inline int getY() const
	{
		return m_iY;
	}

	bool at(int iX, int iY) const;
	int getLatitude() const;

	CvArea* area() const;

	inline int getArea() const
	{
		return m_iArea;
	}

	void setArea(int iNewValue);

	std::vector<int> getAllAdjacentAreas() const;

	inline int getLandmass() const
	{
		return m_iLandmass;
	}
	void setLandmass(int iNewValue);

	int getFeatureVariety() const;

	int getOwnershipDuration() const;
	bool isOwnershipScore() const;
	void setOwnershipDuration(int iNewValue);
	void changeOwnershipDuration(int iChange);

	int getImprovementDuration() const;
	void setImprovementDuration(int iNewValue);
	void changeImprovementDuration(int iChange);

	int getUpgradeProgress() const;
	int getUpgradeTimeLeft(ImprovementTypes eImprovement, PlayerTypes ePlayer) const;
	void setUpgradeProgress(int iNewValue);
	void changeUpgradeProgress(int iChange);

#if defined(MOD_API_UNIFIED_YIELDS)
	int ComputeYieldFromAdjacentImprovement(CvImprovementEntry& kImprovement, ImprovementTypes eValue, YieldTypes eYield) const;
	int ComputeYieldFromTwoAdjacentImprovement(CvImprovementEntry& kImprovement, ImprovementTypes eValue, YieldTypes eYield) const;
	int ComputeYieldFromOtherAdjacentImprovement(CvImprovementEntry& kImprovement, YieldTypes eYield) const;
	int ComputeYieldFromAdjacentTerrain(CvImprovementEntry& kImprovement, YieldTypes eYield) const;
	int ComputeYieldFromAdjacentResource(CvImprovementEntry& kImprovement, YieldTypes eYield) const;
	int ComputeYieldFromAdjacentPlot(CvImprovementEntry& kImprovement, YieldTypes eYield) const;
#else
	int ComputeCultureFromAdjacentImprovement(CvImprovementEntry& kImprovement, ImprovementTypes eValue) const;
#endif

#if defined(MOD_GLOBAL_STACKING_RULES)
	int getAdditionalUnitsFromImprovement() const;
	void calculateAdditionalUnitsFromImprovement();
	int getStackingUnits() const;
#endif

	int getNumMajorCivsRevealed() const;
	void setNumMajorCivsRevealed(int iNewValue);
	void changeNumMajorCivsRevealed(int iChange);

	int getCityRadiusCount() const;
	int isCityRadius() const;
	void changeCityRadiusCount(int iChange);

	bool isStartingPlot() const;
	void setStartingPlot(bool bNewValue);

	bool isNEOfRiver() const;
	void setNEOfRiver(bool bNewValue, FlowDirectionTypes eRiverDir);

	bool isWOfRiver() const;
	void setWOfRiver(bool bNewValue, FlowDirectionTypes eRiverDir);

	bool isNWOfRiver() const;
	void setNWOfRiver(bool bNewValue, FlowDirectionTypes eRiverDir);

	FlowDirectionTypes getRiverEFlowDirection() const;
	FlowDirectionTypes getRiverSEFlowDirection() const;
	FlowDirectionTypes getRiverSWFlowDirection() const;

	CvPlot* getInlandCorner() const;
	bool hasCoastAtSECorner() const;

	bool isPotentialCityWork() const;
	bool isPotentialCityWorkForArea(CvArea* pArea) const;
	void updatePotentialCityWork();


	inline PlayerTypes getOwner() const
	{
		return (PlayerTypes)m_eOwner;
	}

	void setOwner(PlayerTypes eNewValue, int iAcquiringCityID, bool bCheckUnits = true, bool bUpdateResources = true);
	void ClearCityPurchaseInfo(void);
	PlayerTypes GetCityPurchaseOwner(void);
	int GetCityPurchaseID(void);
	void SetCityPurchaseID(int iAcquiringCityID);

	bool IsHomeFrontForPlayer(PlayerTypes ePlayer) const;

	PlotTypes getPlotType() const
	{
		return (PlotTypes)m_ePlotType;
	}
	bool isWater()          const
	{
		return (PlotTypes)m_ePlotType == PLOT_OCEAN;
	};
	bool isHills()          const
	{
		return (PlotTypes)m_ePlotType == PLOT_HILLS;
	};
	bool isIce()            const
	{
		return getFeatureType() == FEATURE_ICE;
	};
	bool isOpenGround()     const
	{
		if( isHills() || isMountain() || m_bRoughFeature) 
			return false;
		return true;
	}
	bool isMountain()       const
	{
		//the two should be synonymous ...
		return (TerrainTypes)m_eTerrainType == TERRAIN_MOUNTAIN || (PlotTypes)m_ePlotType == PLOT_MOUNTAIN;
	};
	bool isRiver()          const
	{
		return m_iRiverCrossingCount > 0;
	}

	//can a generic unit move through this plot (disregarding promotions, combat/civilian etc)
	bool isValidMovePlot(PlayerTypes ePlayer, bool bCheckTerritory=true) const;

	bool isBlockaded(PlayerTypes ePlayer);

	TerrainTypes getTerrainType() const
	{
		return (TerrainTypes)m_eTerrainType;
	}
	FeatureTypes getFeatureType() const
	{
		return (FeatureTypes)m_eFeatureType.get();
	}

#if defined(MOD_API_PLOT_BASED_DAMAGE)
	int getTurnDamage(bool bIgnoreTerrainDamage, bool bIgnoreFeatureDamage, bool bExtraTerrainDamage, bool bExtraFeatureDamage) const
	{
		int damage = 0;

		if (MOD_API_PLOT_BASED_DAMAGE) {
			const TerrainTypes eTerrain = getTerrainType();
			const FeatureTypes eFeature = getFeatureType();
			
			// Make an exception for the volcano
			if (eFeature != NO_FEATURE) {
				CvFeatureInfo* pkFeatureInfo = GC.getFeatureInfo(eFeature);
				if (pkFeatureInfo && pkFeatureInfo->GetType() == "FEATURE_VOLCANO") {
					bIgnoreTerrainDamage = false;
					bIgnoreFeatureDamage = false;
				}
			}

			if (eTerrain != NO_TERRAIN) {
				CvTerrainInfo* pkTerrainInfo = GC.getTerrainInfo(eTerrain);
				if (pkTerrainInfo) {
					// no damage for units on montain cities
					if (!bIgnoreTerrainDamage && !isCity())
					{						
						damage += pkTerrainInfo->getTurnDamage();
					}
					if (bExtraTerrainDamage) {
						damage += pkTerrainInfo->getExtraTurnDamage();
					}
				}
			}

			if (eFeature != NO_FEATURE) {
				CvFeatureInfo* pkFeatureInfo = GC.getFeatureInfo(eFeature);
				if (pkFeatureInfo) {
					if (!bIgnoreFeatureDamage) {
						damage += pkFeatureInfo->getTurnDamage();
					}
					
					if (bExtraFeatureDamage) {
						damage += pkFeatureInfo->getExtraTurnDamage();
					}
				}
			}
		}		
		return damage;
	}
#endif

	bool isImpassable(TeamTypes eTeam = NO_TEAM) const;
	bool IsAllowsWalkWater() const;
	bool needsEmbarkation(const CvUnit* pUnit) const;

	bool isRoughGround() const
	{
		if(isHills() || isMountain())
			return true;

		return m_bRoughFeature;
	}

	bool isFlatlands() const;
	void setPlotType(PlotTypes eNewValue, bool bRecalculate = true, bool bRebuildGraphics = true, bool bEraseUnitsIfWater = true);

	bool IsRoughFeature() const;
	void SetRoughFeature(bool bValue);

	void setTerrainType(TerrainTypes eNewValue, bool bRecalculate = true, bool bRebuildGraphics = true);

	void setFeatureType(FeatureTypes eNewValue, int iVariety = -1);
#if defined(MOD_PSEUDO_NATURAL_WONDER)
	bool IsNaturalWonder(bool orPseudoNatural = false) const;
#else
	bool IsNaturalWonder() const;
#endif

	ResourceTypes getResourceType(TeamTypes eTeam = NO_TEAM) const;
	ResourceTypes getNonObsoleteResourceType(TeamTypes eTeam = NO_TEAM) const;
	void setResourceType(ResourceTypes eNewValue, int iResourceNum, bool bForMinorCivPlot = false);
	int getNumResource() const;
	void setNumResource(int iNum);
	void changeNumResource(int iChange);
	int getNumResourceForPlayer(PlayerTypes ePlayer) const;
#if defined(MOD_GLOBAL_VENICE_KEEPS_RESOURCES)
	void removeMinorResources(bool bVenice = false);
#endif

	ImprovementTypes getImprovementType() const;
	ImprovementTypes getImprovementTypeNeededToImproveResource(PlayerTypes ePlayer = NO_PLAYER, bool bTestPlotOwner = true);
	void setImprovementType(ImprovementTypes eNewValue, PlayerTypes eBuilder = NO_PLAYER);
#if defined(MOD_DIPLOMACY_CITYSTATES)
	bool IsImprovementEmbassy() const;
	void SetImprovementEmbassy(bool bEmbassy);
#endif
	bool IsImprovementPillaged() const;
#if defined(MOD_EVENTS_TILE_IMPROVEMENTS)
	void SetImprovementPillaged(bool bPillaged, bool bEvents = true);
#else
	void SetImprovementPillaged(bool bPillaged);
#endif

	// Someone gifted an improvement in an owned plot? (major civ gift to city-state)
	bool IsImprovedByGiftFromMajor() const;
	void SetImprovedByGiftFromMajor(bool bValue);

	bool HasSpecialImprovement() const;

	// Who built the improvement in this plot?
	PlayerTypes GetPlayerThatBuiltImprovement() const;
	void SetPlayerThatBuiltImprovement(PlayerTypes eBuilder);
	
	// Someone footing the bill for an improvement/route in an unowned plot?
	PlayerTypes GetPlayerResponsibleForImprovement() const;
	void SetPlayerResponsibleForImprovement(PlayerTypes eNewValue);
	PlayerTypes GetPlayerResponsibleForRoute() const;
	void SetPlayerResponsibleForRoute(PlayerTypes eNewValue);

	bool IsBarbarianCampNotConverting() const;
	void SetBarbarianCampNotConverting(bool bNotConverting);

	GenericWorldAnchorTypes GetWorldAnchor() const;
	int GetWorldAnchorData() const;
	void SetWorldAnchor(GenericWorldAnchorTypes eAnchor, int iData1 = -1);

	RouteTypes getRouteType() const;
	void setRouteType(RouteTypes eNewValue);
	void updateCityRoute();

	bool IsRoutePillaged() const;
#if defined(MOD_EVENTS_TILE_IMPROVEMENTS)
	void SetRoutePillaged(bool bPillaged, bool bEvents = true);
#else
	void SetRoutePillaged(bool bPillaged);
#endif

	PlayerTypes GetPlayerThatClearedBarbCampHere() const;
	void SetPlayerThatClearedBarbCampHere(PlayerTypes eNewValue);

#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	PlayerTypes GetPlayerThatClearedDigHere() const;
	void SetPlayerThatClearedDigHere(PlayerTypes eNewValue);
#endif

	CvCity* GetResourceLinkedCity() const;
	void SetResourceLinkedCity(const CvCity* pNewValue);
	bool IsResourceLinkedCityActive() const;
	void SetResourceLinkedCityActive(bool bValue);
	void DoFindCityToLinkResourceTo(CvCity* pCityToExclude = NULL);

	CvCity* getPlotCity() const
	{
		if((m_plotCity.eOwner >= 0) && m_plotCity.eOwner < MAX_PLAYERS)
			return (GET_PLAYER((PlayerTypes)m_plotCity.eOwner).getCity(m_plotCity.iID));

		return NULL;
	}

	void setPlotCity(CvCity* pNewValue);

	int getWorkingCityID() const;
	CvCity* getWorkingCity() const;
	void updateWorkingCity();

	CvCity* getWorkingCityOverride() const;
	void setWorkingCityOverride(const CvCity* pNewValue);

	int getReconCount() const;
	void changeReconCount(int iChange);

	int getRiverCrossingCount() const;
	void changeRiverCrossingCount(int iChange);

	int getYield(YieldTypes eIndex) const;
	int calculateNatureYield(YieldTypes eIndex, PlayerTypes ePlayer, bool bIgnoreFeature = false) const;
	int calculateBestNatureYield(YieldTypes eIndex, PlayerTypes ePlayer) const;
	int calculateTotalBestNatureYield(PlayerTypes ePlayer) const;
	int calculateImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYield, PlayerTypes ePlayer, bool bOptimal = false, RouteTypes eAssumeThisRoute = NUM_ROUTE_TYPES) const;
	int calculateYield(YieldTypes eIndex, bool bDisplay = false);
	bool hasYield() const;
	void updateYield();

	int getYieldWithBuild(BuildTypes eBuild, YieldTypes eYield, bool bWithUpgrade, PlayerTypes ePlayer) const;

	int countNumAirUnits(TeamTypes eTeam) const;
	int countNumAntiAirUnits(TeamTypes eTeam) const;

#if defined(MOD_BALANCE_CORE_SETTLER)
	int GetExplorationBonus(const CvPlayer* pPlayer, const CvUnit* pUnit);
#endif

	int getFoundValue(PlayerTypes eIndex);
	bool isBestAdjacentFound(PlayerTypes eIndex);
	void setFoundValue(PlayerTypes eIndex, int iNewValue);

	int getPlayerCityRadiusCount(PlayerTypes eIndex) const;
	bool isPlayerCityRadius(PlayerTypes eIndex) const;
	void changePlayerCityRadiusCount(PlayerTypes eIndex, int iChange);

	int getVisibilityCount(TeamTypes eTeam) const
	{
		CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
		CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

#if defined(MOD_CORE_DELAYED_VISIBILITY)
		//return the hacked visibility count so plots which were once visible this turn stay that way
		return m_aiVisibilityCountThisTurnMax[eTeam];
#else
		return m_aiVisibilityCount[eTeam];
#endif
	}

	void flipVisibility(TeamTypes eTeam);
#if defined(MOD_API_EXTENSIONS)
	PlotVisibilityChangeResult changeVisibilityCount(TeamTypes eTeam, int iChange, InvisibleTypes eSeeInvisible, bool bInformExplorationTracking, bool bAlwaysSeeInvisible, CvUnit* pUnit = NULL);
#else
	PlotVisibilityChangeResult changeVisibilityCount(TeamTypes eTeam, int iChange, InvisibleTypes eSeeInvisible, bool bInformExplorationTracking, bool bAlwaysSeeInvisible);
#endif

	PlayerTypes getRevealedOwner(TeamTypes eTeam, bool bDebug) const;
	TeamTypes getRevealedTeam(TeamTypes eTeam, bool bDebug) const;
	PlayerTypes getRevealedOwner(TeamTypes eTeam) const;
	TeamTypes getRevealedTeam(TeamTypes eTeam) const;
	bool setRevealedOwner(TeamTypes eTeam, PlayerTypes eNewValue);
	void updateRevealedOwner(TeamTypes eTeam);

	bool isRiverCrossing(DirectionTypes eIndex) const;
	void updateRiverCrossing(DirectionTypes eIndex);
	void updateRiverCrossing();

	bool isRevealed(TeamTypes eTeam, bool bDebug) const
	{
		if(bDebug && GC.getGame().isDebugMode())
			return true;
		CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
		CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
		return m_bfRevealed.GetBit(eTeam);
	}

	bool isRevealed(TeamTypes eTeam) const
	{
		CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
		CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
		return m_bfRevealed.GetBit(eTeam);
	}

#if defined(MOD_API_EXTENSIONS)
	bool setRevealed(TeamTypes eTeam, bool bNewValue, CvUnit* pUnit = NULL, bool bTerrainOnly = false, TeamTypes eFromTeam = NO_TEAM);
#else
	bool setRevealed(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly = false, TeamTypes eFromTeam = NO_TEAM);
#endif
	bool isAdjacentRevealed(TeamTypes eTeam) const;
	bool isAdjacentNonrevealed(TeamTypes eTeam) const;
	int getNumAdjacentNonrevealed(TeamTypes eTeam) const;
	bool IsResourceForceReveal(TeamTypes eTeam) const;
	void SetResourceForceReveal(TeamTypes eTeam, bool bValue);
#if defined(MOD_BALANCE_CORE)
	bool IsTeamImpassable(TeamTypes eTeam) const;
	void SetTeamImpassable(TeamTypes eTeam, bool bValue);
#endif

	ImprovementTypes getRevealedImprovementType(TeamTypes eTeam, bool bDebug) const;
	ImprovementTypes getRevealedImprovementType(TeamTypes eTeam) const;
	bool setRevealedImprovementType(TeamTypes eTeam, ImprovementTypes eNewValue);

	RouteTypes getRevealedRouteType(TeamTypes eTeam, bool bDebug) const;
	RouteTypes getRevealedRouteType(TeamTypes eTeam) const;
	bool setRevealedRouteType(TeamTypes eTeam, RouteTypes eNewValue);

	int getBuildProgress(BuildTypes eBuild) const;
	bool changeBuildProgress(BuildTypes eBuild, int iChange, PlayerTypes ePlayer = NO_PLAYER, bool bNewBuild = false);
	bool getAnyBuildProgress() const;
	void SilentlyResetAllBuildProgress();

	bool isLayoutDirty() const;							// The plot layout contains resources, routes, and improvements
	void setLayoutDirty(bool bDirty);
	bool isLayoutStateDifferent() const;
	void setLayoutStateToCurrent();
	void updateLayout(bool bDebug);

	void getVisibleImprovementState(ImprovementTypes& eType, bool& bWorked);				// determines how the improvement state is shown in the engine
	void getVisibleResourceState(ResourceTypes& eType, bool& bImproved, bool& bWorked);		// determines how the resource state is shown in the engine

	CvUnit* getCenterUnit();
	const CvUnit* getCenterUnit() const;
	const CvUnit* getDebugCenterUnit() const;
	void setCenterUnit(CvUnit* pNewValue);

	int getInvisibleVisibilityCount(TeamTypes eTeam, InvisibleTypes eInvisible) const;
	bool isInvisibleVisible(TeamTypes eTeam, InvisibleTypes eInvisible) const;
	void changeInvisibleVisibilityCount(TeamTypes eTeam, InvisibleTypes eInvisible, int iChange);

	int getNumUnits() const;
	int GetNumCombatUnits();
	CvUnit* getUnitByIndex(int iIndex) const;
	int getUnitIndex(CvUnit* pUnit) const;
	void addUnit(CvUnit* pUnit, bool bUpdate = true);
	void removeUnit(CvUnit* pUnit, bool bUpdate = true);
	const IDInfo* nextUnitNode(const IDInfo* pNode) const;
	IDInfo* nextUnitNode(IDInfo* pNode);
	const IDInfo* prevUnitNode(const IDInfo* pNode) const;
	IDInfo* prevUnitNode(IDInfo* pNode);
	const IDInfo* headUnitNode() const;
	IDInfo* headUnitNode();
	const IDInfo* tailUnitNode() const;
	IDInfo* tailUnitNode();
	uint getUnits(IDInfoVector* pkInfoVector) const;
	int getNumLayerUnits(int iLayerID = -1) const;
	CvUnit* getLayerUnit(int iIndex, int iLayerID = -1) const;

	// Script data needs to be a narrow string for pickling in Python
	CvString getScriptData() const;
	void setScriptData(const char* szNewValue);

#if defined(SHOW_PLOT_POPUP)
	void showPopupText(PlayerTypes ePlayer, const char* szMessage);
#endif

	bool canTrain(UnitTypes eUnit, bool bContinue, bool bTestVisible) const;

	void read(FDataStream& kStream);
	void write(FDataStream& kStream) const;

	inline int getScratchPad() const
	{
		return m_iScratchPad;
	}
	inline void setScratchPad(int iNewValue)
	{
		m_iScratchPad = iNewValue;
	}

	PlayerTypes GetBuilderAIScratchPadPlayer() const;
	void SetBuilderAIScratchPadPlayer(PlayerTypes ePlayer);

	short GetBuilderAIScratchPadTurn() const;
	void SetBuilderAIScratchPadTurn(short sNewValue);

	RouteTypes GetBuilderAIScratchPadRoute() const;
	void SetBuilderAIScratchPadRoute(RouteTypes eRoute);

	short GetBuilderAIScratchPadValue() const;
	void SetBuilderAIScratchPadValue(short sNewValue);

	int GetPlotIndex() const;

	char GetContinentType() const;
	void SetContinentType(const char cContinent);

	const FAutoArchive& getSyncArchive() const;
	FAutoArchive& getSyncArchive();
	std::string debugDump(const FAutoVariableBase&) const;
	std::string stackTraceRemark(const FAutoVariableBase&) const;

	// Validate the contents of the plot.  This will attempt to clean up inconsistencies
	int Validate(CvMap& kParentMap);

	bool MustPayMaintenanceHere(PlayerTypes ePlayer) const;
#if defined(MOD_API_EXTENSIONS)
	void SetArchaeologicalRecord(GreatWorkArtifactClass eType, PlayerTypes ePlayer1, PlayerTypes ePlayer2);
	void SetArchaeologicalRecord(GreatWorkArtifactClass eType, EraTypes eEra, PlayerTypes ePlayer1, PlayerTypes ePlayer2);
#endif
	void AddArchaeologicalRecord(GreatWorkArtifactClass eType, PlayerTypes ePlayer1, PlayerTypes ePlayer2);
	void AddArchaeologicalRecord(GreatWorkArtifactClass eType, EraTypes eEra, PlayerTypes ePlayer1, PlayerTypes ePlayer2);
	void ClearArchaeologicalRecord();
	CvArchaeologyData GetArchaeologicalRecord() const;
	void SetArtifactType(GreatWorkArtifactClass eType);
	void SetArtifactGreatWork(GreatWorkType eWork);
	bool HasWrittenArtifact() const;

	bool IsNearEnemyCitadel(PlayerTypes ePlayer, int* piCitadelDamage=NULL, PromotionTypes ePromotion = NO_PROMOTION) const;

#if defined(MOD_API_EXTENSIONS)
	bool IsCivilization(CivilizationTypes iCivilizationType) const;
	bool HasFeature(FeatureTypes iFeatureType) const;
	bool HasAnyNaturalWonder() const;
	bool HasNaturalWonder(FeatureTypes iFeatureType) const;
	LUAAPIINLINE(IsFeatureIce, HasFeature, FEATURE_ICE)
	LUAAPIINLINE(IsFeatureJungle, HasFeature, FEATURE_JUNGLE)
	LUAAPIINLINE(IsFeatureMarsh, HasFeature, FEATURE_MARSH)
	LUAAPIINLINE(IsFeatureOasis, HasFeature, FEATURE_OASIS)
	LUAAPIINLINE(IsFeatureFloodPlains, HasFeature, FEATURE_FLOOD_PLAINS)
	LUAAPIINLINE(IsFeatureForest, HasFeature, FEATURE_FOREST)
	LUAAPIINLINE(IsFeatureFallout, HasFeature, FEATURE_FALLOUT)
	LUAAPIINLINE(IsFeatureAtoll, HasFeature, ((FeatureTypes)GC.getInfoTypeForString("FEATURE_ATOLL")))
	bool IsFeatureLake() const;
	bool IsFeatureRiver() const;
	bool HasImprovement(ImprovementTypes iImprovementType) const;
	bool HasPlotType(PlotTypes iPlotType) const;
	LUAAPIINLINE(IsPlotMountain, HasPlotType, PLOT_MOUNTAIN)
	LUAAPIINLINE(IsPlotMountains, HasPlotType, PLOT_MOUNTAIN)
	LUAAPIINLINE(IsPlotHill, HasPlotType, PLOT_HILLS)
	LUAAPIINLINE(IsPlotHills, HasPlotType, PLOT_HILLS)
	LUAAPIINLINE(IsPlotLand, HasPlotType, PLOT_LAND)
	LUAAPIINLINE(IsPlotOcean, HasPlotType, PLOT_OCEAN)
	bool HasResource(ResourceTypes iResourceType) const;
	bool HasRoute(RouteTypes iRouteType) const;
	LUAAPIINLINE(IsRouteRoad, HasRoute, ROUTE_ROAD)
	LUAAPIINLINE(IsRouteRailroad, HasRoute, ROUTE_RAILROAD)
	bool HasTerrain(TerrainTypes iTerrainType) const;
	LUAAPIINLINE(IsTerrainGrass, HasTerrain, TERRAIN_GRASS)
	LUAAPIINLINE(IsTerrainPlains, HasTerrain, TERRAIN_PLAINS)
	LUAAPIINLINE(IsTerrainDesert, HasTerrain, TERRAIN_DESERT)
	LUAAPIINLINE(IsTerrainTundra, HasTerrain, TERRAIN_TUNDRA)
	LUAAPIINLINE(IsTerrainSnow, HasTerrain, TERRAIN_SNOW)
	LUAAPIINLINE(IsTerrainCoast, HasTerrain, TERRAIN_COAST)
	LUAAPIINLINE(IsTerrainOcean, HasTerrain, TERRAIN_OCEAN)
	LUAAPIINLINE(IsTerrainMountain, HasTerrain, TERRAIN_MOUNTAIN)
	LUAAPIINLINE(IsTerrainMountains, HasTerrain, TERRAIN_MOUNTAIN)
	LUAAPIINLINE(IsTerrainHill, HasTerrain, TERRAIN_HILL)
	LUAAPIINLINE(IsTerrainHills, HasTerrain, TERRAIN_HILL)
	bool IsAdjacentToFeature(FeatureTypes iFeatureType) const;
	bool IsWithinDistanceOfFeature(FeatureTypes iFeatureType, int iDistance) const;
#if defined(MOD_BALANCE_CORE)
	bool IsWithinDistanceOfUnit(PlayerTypes ePlayer, UnitTypes eOtherUnit, int iDistance, bool bIsFriendly, bool bIsEnemy) const;
	bool IsWithinDistanceOfUnitClass(PlayerTypes ePlayer, UnitClassTypes eUnitClass, int iDistance, bool bIsFriendly, bool bIsEnemy) const;
	bool IsWithinDistanceOfUnitCombatType(PlayerTypes ePlayer, UnitCombatTypes eUnitCombat, int iDistance, bool bIsFriendly, bool bIsEnemy) const;
	bool IsWithinDistanceOfUnitPromotion(PlayerTypes ePlayer, PromotionTypes eUnitPromotion, int iDistance, bool bIsFriendly, bool bIsEnemy) const;
	bool IsWithinDistanceOfCity(const CvUnit* eThisUnit, int iDistance, bool bIsFriendly, bool bIsEnemy) const;
	bool IsAdjacentToUnit(PlayerTypes ePlayer, UnitTypes eOtherUnit, bool bIsFriendly, bool bIsEnemy) const;
	bool IsAdjacentToUnitClass(PlayerTypes ePlayer, UnitClassTypes eUnitClass, bool bIsFriendly, bool bIsEnemy) const;
	bool IsAdjacentToUnitCombatType(PlayerTypes ePlayer, UnitCombatTypes eUnitCombat, bool bIsFriendly, bool bIsEnemy) const;
	bool IsAdjacentToUnitPromotion(PlayerTypes ePlayer, PromotionTypes eUnitPromotion, bool bIsFriendly, bool bIsEnemy) const;
	bool IsAdjacentToTradeRoute() const;
#endif
	bool IsAdjacentToImprovement(ImprovementTypes iImprovementType) const;
	bool IsWithinDistanceOfImprovement(ImprovementTypes iImprovementType, int iDistance) const;
	bool IsAdjacentToPlotType(PlotTypes iPlotType) const;
	bool IsWithinDistanceOfPlotType(PlotTypes iPlotType, int iDistance) const;
	bool IsAdjacentToResource(ResourceTypes iResourceType) const;
	bool IsWithinDistanceOfResource(ResourceTypes iResourceType, int iDistance) const;
	bool IsAdjacentToTerrain(TerrainTypes iTerrainType) const;
	bool IsWithinDistanceOfTerrain(TerrainTypes iTerrainType, int iDistance) const;
#endif

#if defined(MOD_BALANCE_CORE)
	bool IsEnemyCityAdjacent(TeamTypes eMyTeam, const CvCity* pSpecifyCity) const;
	int GetNumEnemyUnitsAdjacent(TeamTypes eMyTeam, DomainTypes eDomain, const CvUnit* pUnitToExclude = NULL, bool bCountRanged = true) const;
	vector<CvUnit*> GetAdjacentEnemyUnits(TeamTypes eMyTeam, DomainTypes eDomain) const;
	int GetAdjacentEnemyPower(PlayerTypes ePlayer) const;

	int GetNumFriendlyUnitsAdjacent(TeamTypes eMyTeam, DomainTypes eDomain, const CvUnit* pUnitToExclude = NULL, bool bCountRanged = true) const;
	bool IsFriendlyUnitAdjacent(TeamTypes eMyTeam, bool bCombatUnit) const;
	int GetNumSpecificPlayerUnitsAdjacent(PlayerTypes ePlayer, const CvUnit* pUnitToExclude = NULL, const CvUnit* pExampleUnitType = NULL, bool bCombatOnly = true) const;

	int GetDefenseBuildValue(PlayerTypes eOwner);
	void UpdatePlotsWithLOS();
	bool GetPlotsAtRangeX(int iRange, bool bFromPlot, bool bWithLOS, std::vector<CvPlot*>& vResult) const;

	void updateImpassable(TeamTypes eTeam = NO_TEAM);

	bool hasSharedAdjacentArea(CvPlot* pOtherPlot) const;
#endif

#if defined(MOD_BALANCE_CORE)
	int GetNumSpecificFriendlyUnitCombatsAdjacent(TeamTypes eMyTeam, UnitCombatTypes eUnitCombat, const CvUnit* pUnitToExclude = NULL) const;
#endif

	bool canPlaceCombatUnit(PlayerTypes ePlayer) const;

protected:
	class PlotBoolField
	{
	public:
		enum config { eCount = 4, eSize = 32 };
		DWORD m_dwBits[eCount];

		bool GetBit(const uint uiEntry) const
		{
			const uint uiOffset = uiEntry/eSize;
			return m_dwBits[uiOffset] & 1<<(uiEntry-(eSize*uiOffset));
		}
		void SetBit(const uint uiEntry)
		{
			const uint uiOffset = uiEntry/eSize;
			m_dwBits[uiOffset] |= 1<<(uiEntry-(eSize*uiOffset));
		}
		void ClearBit(const uint uiEntry)
		{
			const uint uiOffset = uiEntry/eSize;
			m_dwBits[uiOffset] &= ~(1<<(uiEntry-(eSize*uiOffset)));
		}
		void ToggleBit(const uint uiEntry)
		{
			const uint uiOffset = uiEntry/eSize;
			m_dwBits[uiOffset] ^= 1<<(uiEntry-(eSize*uiOffset));
		}
		void ClearAll()
		{
			for(uint i = 0; i <eCount; ++i)
			{
				m_dwBits[i] = 0;
			}
		}

		bool ValidateFromBoolArray(const bool* pBools, uint uiCount) const
		{
			for(uint i = 0; i < uiCount; ++i)
				if(GetBit(i) != pBools[i]) return false;

			return true;
		}

		void InitFromBoolArray(bool* pBools, uint uiCount)
		{
			for(uint i = 0; i < uiCount; ++i)
				if(GetBit(i) != pBools[i]) ToggleBit(i);
		}
	};

	short m_iX;
	short m_iY;
	//save memory, enum is uint32 ...
	char /*PlayerTypes*/  m_eOwner;
	char /*PlotTypes*/    m_ePlotType;
	char /*TerrainTypes*/ m_eTerrainType;

	PlotBoolField m_bfRevealed;

	FFastSmallFixedList<IDInfo, 4, true, c_eCiv5GameplayDLL > m_units;

	IDInfo m_plotCity;
	IDInfo m_workingCity;
	IDInfo m_workingCityOverride;
	IDInfo m_ResourceLinkedCity;
	IDInfo m_purchaseCity;

	//external memory allocated by CvMap
	friend class CvMap;
	uint8* m_aiYield;
	uint8* m_aiPlayerCityRadiusCount;
	uint8* m_aiVisibilityCount;				//serialized
	uint8* m_aiVisibilityCountThisTurnMax;	//not serialized
	char* m_aiRevealedOwner;
	char *m_aeRevealedImprovementType;
	char *m_aeRevealedRouteType;
	bool* m_abResourceForceReveal;
#if defined(MOD_BALANCE_CORE)
	bool* m_abIsImpassable;
	bool m_bIsTradeUnitRoute;

	int m_iLastTurnBuildChanged;
#endif

#if defined(MOD_BALANCE_CORE)
	int m_iPlotIndex;
	std::vector<CvPlot*> m_vPlotsWithLineOfSightFromHere2;
	std::vector<CvPlot*> m_vPlotsWithLineOfSightFromHere3;
	std::vector<CvPlot*> m_vPlotsWithLineOfSightToHere2;
	std::vector<CvPlot*> m_vPlotsWithLineOfSightToHere3;
#endif

	char* m_szScriptData;
	short* m_paiBuildProgress;

#if defined(SHOW_PLOT_POPUP)
	float m_fPopupDelay;
#endif

	CvUnit* m_pCenterUnit;

	short m_apaiInvisibleVisibilityCount[MAX_TEAMS][NUM_INVISIBLE_TYPES];

	int m_iArea;
	int m_iLandmass;

	// This is a variable that you can use for whatever nefarious deeds you need to do
	// it will not be saved or loaded - you should assume that it is filled with garbage
	// when you get it
	int m_iScratchPad;
	char m_cBuilderAIScratchPadPlayer;
	short m_sBuilderAIScratchPadTurn;
	short m_sBuilderAIScratchPadValue;
	RouteTypes m_eBuilderAIScratchPadRoute;

	short m_iOwnershipDuration;
	short m_iImprovementDuration;
	short m_iUpgradeProgress;

	short m_iCulture;

	uint m_uiCityConnectionBitFlags;

	FAutoArchiveClassContainer<CvPlot> m_syncArchive; // this must appear before the first auto variable in the class
	FAutoVariable<char, CvPlot> /*FeatureTypes*/ m_eFeatureType;
	char /*ResourceTypes*/ m_eResourceType;
	char /*ImprovementTypes*/ m_eImprovementType;
	char /*ImprovementTypes*/ m_eImprovementTypeUnderConstruction;
	char /*PlayerTypes*/ m_ePlayerBuiltImprovement;
	char /*PlayerTypes*/ m_ePlayerResponsibleForImprovement;
	char /*PlayerTypes*/ m_ePlayerResponsibleForRoute;
	char /*PlayerTypes*/ m_ePlayerThatClearedBarbCampHere;
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	char /*PlayerTypes*/ m_ePlayerThatClearedDigHere;
#endif
	char /*RouteTypes*/ m_eRouteType;
#if defined(MOD_GLOBAL_STACKING_RULES)
	short m_eUnitIncrement;
#endif
	char /*GenericWorldAnchorTypes*/ m_eWorldAnchor;
	char /*int*/ m_cWorldAnchorData;
	char /*FlowDirectionTypes*/ m_eRiverEFlowDirection; // flow direction on the E edge (isWofRiver)
	char /*FlowDirectionTypes*/ m_eRiverSEFlowDirection; // flow direction on the SE edge (isNWofRiver)
	char /*FlowDirectionTypes*/ m_eRiverSWFlowDirection; // flow direction on the SW edge (isNEofRiver)
	char m_iFeatureVariety;
	char m_iNumMajorCivsRevealed;
	char m_iCityRadiusCount;
	char m_iReconCount;
	char m_iRiverCrossingCount;
	char m_iResourceNum;
	char m_cContinentType;
	char m_cRiverCrossing;	// bit field

#if defined(MOD_DIPLOMACY_CITYSTATES)
	bool m_bImprovementEmbassy:1;
#endif
	bool m_bImprovementPillaged:1;
	bool m_bRoutePillaged:1;
	bool m_bStartingPlot:1;
	bool m_bHills:1;
	bool m_bNEOfRiver:1;
	bool m_bWOfRiver:1;
	bool m_bNWOfRiver:1;
	bool m_bPotentialCityWork:1;
	bool m_bPlotLayoutDirty:1;
	bool m_bLayoutStateWorked:1;
	bool m_bBarbCampNotConverting:1;
	bool m_bRoughFeature:1;
	bool m_bResourceLinkedCityActive:1;
	bool m_bImprovedByGiftFromMajor:1;
	bool m_bIsAdjacentToLand:1;				// Cached value, do not serialize
	bool m_bIsImpassable:1;
	bool m_bIsFreshwater:1;

	CvArchaeologyData m_kArchaeologyData;

	void processArea(CvArea* pArea, int iChange);
	void doImprovementUpgrade();

	// added so under cheat mode we can access protected stuff
	friend class CvGameTextMgr;
};

namespace FSerialization
{
void SyncPlots();
void ClearPlotDeltas();
}

#if defined(MOD_BALANCE_CORE_MILITARY)
struct SPlotWithScore
{
	SPlotWithScore(CvPlot* pPlot_, int score_) : pPlot(pPlot_), score(score_) {}
    bool operator<(const SPlotWithScore& other) const //for sorting
    {
        return score < other.score;
    }
    bool operator<(const int ref) const
    {
        return score < ref;
    }
    bool operator==(const SPlotWithScore& other) const //for std::find
    {
        return pPlot == other.pPlot;
    }

	CvPlot* pPlot;
	int score;
};
struct SPlotWithTwoScoresL2
{
	SPlotWithTwoScoresL2(CvPlot* pPlot_, int score1_, int score2_) : pPlot(pPlot_), score1(score1_), score2(score2_) {}

	bool operator<(const SPlotWithTwoScoresL2& other) const
    {
        return score1*score1+score2*score2 < other.score1*other.score1+other.score2*other.score2;
    }

	CvPlot* pPlot;
	int score1,score2;
};
struct SPlotWithTwoScoresTiebreak
{
	SPlotWithTwoScoresTiebreak(CvPlot* pPlot_, int score1_, int score2_) : pPlot(pPlot_), score1(score1_), score2(score2_) {}

	bool operator<(const SPlotWithTwoScoresTiebreak& other) const
	{
		return (score1<other.score1) || (score1==other.score1 && score2<other.score2);
	}

	CvPlot* pPlot;
	int score1, score2;
};

#endif

#endif
