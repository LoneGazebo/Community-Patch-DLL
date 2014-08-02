/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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
class CvRiver;
class CvCity;

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
	bool isFreshWater() const;

	bool isRiverCrossingFlowClockwise(DirectionTypes eDirection) const;
	bool isRiverSide() const;
	bool isRiverConnection(DirectionTypes eDirection) const;

	CvPlot* getNeighboringPlot(DirectionTypes eDirection) const;
	CvPlot* getNearestLandPlotInternal(int iDistance) const;
	int getNearestLandArea() const;
	CvPlot* getNearestLandPlot() const;

	int seeFromLevel(TeamTypes eTeam) const;
	int seeThroughLevel(bool bIncludeShubbery=true) const;
	void changeSeeFromSight(TeamTypes eTeam, DirectionTypes eDirection, int iFromLevel, bool bIncrement, InvisibleTypes eSeeInvisible);
	void changeAdjacentSight(TeamTypes eTeam, int iRange, bool bIncrement, InvisibleTypes eSeeInvisible, DirectionTypes eFacingDirection, bool bBasedOnUnit=true);
	bool canSeePlot(const CvPlot* plot, TeamTypes eTeam, int iRange, DirectionTypes eFacingDirection) const;
	bool shouldProcessDisplacementPlot(int dx, int dy, int range, DirectionTypes eFacingDirection) const;
	void updateSight(bool bIncrement);
	void updateSeeFromSight(bool bIncrement);

#if defined(MOD_API_EXTENSIONS)
	bool canHaveResource(ResourceTypes eResource, bool bIgnoreLatitude = false, bool bIgnoreCiv = false) const;
#else
	bool canHaveResource(ResourceTypes eResource, bool bIgnoreLatitude = false) const;
#endif
	bool canHaveImprovement(ImprovementTypes eImprovement, TeamTypes eTeam = NO_TEAM, bool bOnlyTestVisible = false) const;

	bool canBuild(BuildTypes eBuild, PlayerTypes ePlayer = NO_PLAYER, bool bTestVisible = false, bool bTestPlotOwner = true) const;
	int getBuildTime(BuildTypes eBuild, PlayerTypes ePlayer) const;
	int getBuildTurnsTotal(BuildTypes eBuild, PlayerTypes ePlayer) const;
	int getBuildTurnsLeft(BuildTypes eBuild, PlayerTypes ePlayer, int iNowExtra, int iThenExtra) const;
	int getFeatureProduction(BuildTypes eBuild, PlayerTypes ePlayer, CvCity** ppCity) const;

	UnitHandle getBestDefender(PlayerTypes eOwner, PlayerTypes eAttackingPlayer = NO_PLAYER, const CvUnit* pAttacker = NULL, bool bTestAtWar = false, bool bTestPotentialEnemy = false, bool bTestCanMove = false, bool bNoncombatAllowed = false);
	const UnitHandle getBestDefender(PlayerTypes eOwner, PlayerTypes eAttackingPlayer = NO_PLAYER, const CvUnit* pAttacker = NULL, bool bTestAtWar = false, bool bTestPotentialEnemy = false, bool bTestCanMove = false, bool bNoncombatAllowed = false) const;
	const CvUnit* getSelectedUnit() const;
	CvUnit* getSelectedUnit();
	int getUnitPower(PlayerTypes eOwner = NO_PLAYER) const;

	int defenseModifier(TeamTypes eDefender, bool bIgnoreBuilding, bool bHelp = false) const;
	int movementCost(const CvUnit* pUnit, const CvPlot* pFromPlot, int iMovesRemaining = 0) const;
	int MovementCostNoZOC(const CvUnit* pUnit, const CvPlot* pFromPlot, int iMovesRemaining = 0) const;
#if defined(MOD_GLOBAL_STACKING_RULES)
	inline int getUnitLimit() const 
	{
		return isCity() ? GC.getCITY_UNIT_LIMIT() : (GC.getPLOT_UNIT_LIMIT() + getAdditionalUnitsFromImprovement());
	}
#endif

	int getExtraMovePathCost() const;
	void changeExtraMovePathCost(int iChange);

	bool isAdjacentOwned() const;
	bool isAdjacentPlayer(PlayerTypes ePlayer, bool bLandOnly = false) const;
	bool IsAdjacentOwnedByOtherTeam(TeamTypes eTeam) const;
	bool isAdjacentTeam(TeamTypes eTeam, bool bLandOnly = false) const;
	CvCity* GetAdjacentFriendlyCity(TeamTypes eTeam, bool bLandOnly = false) const;
	CvCity* GetAdjacentCity(bool bLandOnly = false) const;
	int GetNumAdjacentDifferentTeam(TeamTypes eTeam, bool bIgnoreWater) const;
	int GetNumAdjacentMountains() const;
#if defined(MOD_BALANCE_CORE_SETTLER)
	int GetNumAdjacentWater() const;
	int GetNumAdjacentPlotType(PlotTypes iPlotType) const;
	bool NoTwoPlotTypeTouch(PlotTypes iPlotType, bool bChokePoint = false);
	bool IsChokePoint(bool bWater = false, bool bMountain = false, int iDistance = 0);
#endif

	void plotAction(PlotUnitFunc func, int iData1 = -1, int iData2 = -1, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM);
	int plotCount(ConstPlotUnitFunc funcA, int iData1A = -1, int iData2A = -1, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, ConstPlotUnitFunc funcB = NULL, int iData1B = -1, int iData2B = -1) const;
	CvUnit* plotCheck(ConstPlotUnitFunc funcA, int iData1A = -1, int iData2A = -1, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, ConstPlotUnitFunc funcB = NULL, int iData1B = -1, int iData2B = -1);
	const CvUnit* plotCheck(ConstPlotUnitFunc funcA, int iData1A = -1, int iData2A = -1, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, ConstPlotUnitFunc funcB = NULL, int iData1B = -1, int iData2B = -1) const;

	bool isOwned() const;
	bool isBarbarian() const;
	bool isRevealedBarbarian() const;

	bool HasBarbarianCamp();

#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	bool HasDig();
#endif	

	bool isVisible(TeamTypes eTeam, bool bDebug) const
	{
		if(bDebug && GC.getGame().isDebugMode())
			return true;
		else
		{
			if(eTeam == NO_TEAM)
				return false;

			return ((getVisibilityCount(eTeam) > 0));
		}
	}

	bool isVisible(TeamTypes eTeam) const
	{
		if(eTeam == NO_TEAM)
			return false;

		return ((getVisibilityCount(eTeam) > 0));
	}

	bool isActiveVisible(bool bDebug) const;
	bool isActiveVisible() const;
	bool isVisibleToCivTeam() const;
	bool isVisibleToEnemyTeam(TeamTypes eFriendlyTeam) const;
	bool isVisibleToWatchingHuman() const;
	bool isAdjacentVisible(TeamTypes eTeam, bool bDebug) const;
	bool isAdjacentVisible(TeamTypes eTeam) const;
	bool isAdjacentNonvisible(TeamTypes eTeam) const;
	int  getNumAdjacentNonvisible(TeamTypes eTeam) const;

	bool isGoody(TeamTypes eTeam = NO_TEAM) const;
	bool isRevealedGoody(TeamTypes eTeam = NO_TEAM) const;
	void removeGoody();

	TeamTypes getTeam() const
	{
		PlayerTypes playerID = getOwner();
		if(playerID != NO_PLAYER)
		{
			return CvPlayer::getTeam(playerID);
		}
		else
		{
			return NO_TEAM;
		}
	}

	bool isCity() const
	{
		if((m_plotCity.eOwner >= 0) && m_plotCity.eOwner < MAX_PLAYERS)
			return (GET_PLAYER((PlayerTypes)m_plotCity.eOwner).getCity(m_plotCity.iID)) != NULL;

		return false;
	}

	bool isEnemyCity(const CvUnit& kUnit) const
	{
		CvCity* pCity = getPlotCity();
		if(pCity != NULL)
			return kUnit.isEnemy(pCity->getTeam(), this);

		return false;
	}

	bool isFriendlyCity(const CvUnit& kUnit, bool bCheckImprovement) const;
#if defined(MOD_GLOBAL_PASSABLE_FORTS)
	bool isFriendlyCityOrPassableImprovement(const CvUnit& kUnit, bool bCheckImprovement) const;
#endif
	bool IsFriendlyTerritory(PlayerTypes ePlayer) const;

	bool isBeingWorked() const;

	bool isUnit() const;
	bool isVisibleEnemyDefender(const CvUnit* pUnit) const;
	CvUnit* getVisibleEnemyDefender(PlayerTypes ePlayer);
	int getNumDefenders(PlayerTypes ePlayer) const;
	int getNumVisibleEnemyDefenders(const CvUnit* pUnit) const;
	int getNumVisiblePotentialEnemyDefenders(const CvUnit* pUnit) const;
	bool isVisibleEnemyUnit(PlayerTypes ePlayer) const;
	bool isVisibleEnemyUnit(const CvUnit* pUnit) const;
	bool isVisibleOtherUnit(PlayerTypes ePlayer) const;

#if defined(MOD_GLOBAL_SHORT_EMBARKED_BLOCKADES)
	bool IsActualEnemyUnit(PlayerTypes ePlayer, bool bCombatUnitsOnly = true, bool bNavalUnitsOnly=false) const;
#else
	bool IsActualEnemyUnit(PlayerTypes ePlayer, bool bCombatUnitsOnly = true) const;
#endif
#if defined(MOD_GLOBAL_ALLIES_BLOCK_BLOCKADES)
	bool IsActualAlliedUnit(PlayerTypes ePlayer, bool bCombatUnitsOnly = true) const;
#endif

	int getNumFriendlyUnitsOfType(const CvUnit* pUnit, bool bBreakOnUnitLimit = true) const;

	bool isFighting() const;
	bool isUnitFighting() const;
	bool isCityFighting() const;

	bool canHaveFeature(FeatureTypes eFeature) const;

	bool isRoute() const;
	bool isValidRoute(const CvUnit* pUnit) const;

	void SetTradeRoute(PlayerTypes ePlayer, bool bActive);
	bool IsTradeRoute(PlayerTypes ePlayer = NO_PLAYER) const;

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
	CvArea* waterArea() const;
	CvArea* secondWaterArea() const;

	inline int getArea() const
	{
		return m_iArea;
	}

	void setArea(int iNewValue);

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

	int ComputeCultureFromAdjacentImprovement(CvImprovementEntry& kImprovement, ImprovementTypes eValue) const;

#if defined(MOD_GLOBAL_STACKING_RULES)
	int getAdditionalUnitsFromImprovement() const;
	void calculateAdditionalUnitsFromImprovement();
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
#if defined(MOD_PROMOTIONS_CROSS_ICE)
	bool isIce()            const
	{
		return getFeatureType() == FEATURE_ICE;
	};
#endif
#if defined(MOD_PATHFINDER_TERRAFIRMA)
	bool isTerraFirma(const CvUnit* pUnit)     const
	{
		bool bTerraFirma = !isWater();
		
		if (pUnit->getDomainType() == DOMAIN_LAND) {
#if defined(MOD_PROMOTIONS_CROSS_ICE)
			bTerraFirma = bTerraFirma || (pUnit->canCrossIce() && isIce());
#endif
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
			bTerraFirma = bTerraFirma || (pUnit->IsHoveringUnit() && isShallowWater());
#endif
#if defined(MOD_BUGFIX_HOVERING_PATHFINDER)
			bTerraFirma = bTerraFirma || (pUnit->IsHoveringUnit() && (isShallowWater() || getFeatureType() == FEATURE_ICE));
#endif
		}
		
		return bTerraFirma;
	};
#endif
	bool isOpenGround()     const
	{
		if((PlotTypes)m_ePlotType == PLOT_HILLS || (PlotTypes)m_ePlotType == PLOT_MOUNTAIN || m_bRoughFeature) return false;
		return true;
	}
	bool isMountain()       const
	{
		return (PlotTypes)m_ePlotType == PLOT_MOUNTAIN;
	};
	bool isRiver()          const
	{
		return m_iRiverCrossingCount > 0;
	}

#if defined(MOD_GLOBAL_ADJACENT_BLOCKADES)
	bool isBlockaded(PlayerTypes ePlayer);
#endif

	TerrainTypes getTerrainType() const
	{
		return (TerrainTypes)m_eTerrainType;
	}
	FeatureTypes getFeatureType() const
	{
		char f = m_eFeatureType;
		return (FeatureTypes)f;
	}
#if defined(MOD_API_PLOT_BASED_DAMAGE)
	int getTurnDamage(bool bIgnoreTerrainDamage, bool bIgnoreFeatureDamage, bool bExtraTerrainDamage, bool bExtraFeatureDamage) const
	{
		int damage = 0;

		if (MOD_API_PLOT_BASED_DAMAGE) {
			const TerrainTypes eTerrain = isMountain() ? TERRAIN_MOUNTAIN : getTerrainType();
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
					if (!bIgnoreTerrainDamage) {
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
	bool isImpassable()     const
	{
		return m_bIsImpassable;
	}

	bool IsAllowsWalkWater() const;

	bool isRoughGround() const
	{
		if(isHills())
		{
			return true;
		}
		if(isMountain())
		{
			return true;
		}

		return m_bRoughFeature;
	}

	bool isFlatlands() const;
	void setPlotType(PlotTypes eNewValue, bool bRecalculate = true, bool bRebuildGraphics = true, bool bEraseUnitsIfWater = true);

	bool IsRoughFeature() const;
	void SetRoughFeature(bool bValue);

	void setTerrainType(TerrainTypes eNewValue, bool bRecalculate = true, bool bRebuildGraphics = true);

	void setFeatureType(FeatureTypes eNewValue, int iVariety = -1);

	bool IsNaturalWonder() const;

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

	CvCity* getWorkingCity() const;
	void updateWorkingCity();

	CvCity* getWorkingCityOverride() const;
	void setWorkingCityOverride(const CvCity* pNewValue);

	int getReconCount() const;
	void changeReconCount(int iChange);

	int getRiverCrossingCount() const;
	void changeRiverCrossingCount(int iChange);

	short* getYield();
	int getYield(YieldTypes eIndex) const;
	int calculateNatureYield(YieldTypes eIndex, TeamTypes eTeam, bool bIgnoreFeature = false) const;
	int calculateBestNatureYield(YieldTypes eIndex, TeamTypes eTeam) const;
	int calculateTotalBestNatureYield(TeamTypes eTeam) const;
	int calculateImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYield, PlayerTypes ePlayer, bool bOptimal = false, RouteTypes eAssumeThisRoute = NUM_ROUTE_TYPES) const;
	int calculateYield(YieldTypes eIndex, bool bDisplay = false);
	bool hasYield() const;
	void updateYield();

	int getYieldWithBuild(BuildTypes eBuild, YieldTypes eYield, bool bWithUpgrade, PlayerTypes ePlayer) const;

	int countNumAirUnits(TeamTypes eTeam) const;

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

		return m_aiVisibilityCount[eTeam];
	}

	PlotVisibilityChangeResult changeVisibilityCount(TeamTypes eTeam, int iChange, InvisibleTypes eSeeInvisible, bool bInformExplorationTracking, bool bAlwaysSeeInvisible);

	PlayerTypes getRevealedOwner(TeamTypes eTeam, bool bDebug) const;
	TeamTypes getRevealedTeam(TeamTypes eTeam, bool bDebug) const;
	PlayerTypes getRevealedOwner(TeamTypes eTeam) const;
	TeamTypes getRevealedTeam(TeamTypes eTeam) const;
	bool setRevealedOwner(TeamTypes eTeam, PlayerTypes eNewValue);
	void updateRevealedOwner(TeamTypes eTeam);

	bool isRiverCrossing(DirectionTypes eIndex) const;
	void updateRiverCrossing(DirectionTypes eIndex);
	void updateRiverCrossing();

	bool IsNoSettling(PlayerTypes eMajor) const;
	void SetNoSettling(PlayerTypes eMajor, bool bValue);

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

	bool setRevealed(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly = false, TeamTypes eFromTeam = NO_TEAM);
	bool isAdjacentRevealed(TeamTypes eTeam) const;
	bool isAdjacentNonrevealed(TeamTypes eTeam) const;
	int getNumAdjacentNonrevealed(TeamTypes eTeam) const;
	bool IsResourceForceReveal(TeamTypes eTeam) const;
	void SetResourceForceReveal(TeamTypes eTeam, bool bValue);

	ImprovementTypes getRevealedImprovementType(TeamTypes eTeam, bool bDebug) const;
	ImprovementTypes getRevealedImprovementType(TeamTypes eTeam) const;
	bool setRevealedImprovementType(TeamTypes eTeam, ImprovementTypes eNewValue);

	RouteTypes getRevealedRouteType(TeamTypes eTeam, bool bDebug) const;
	RouteTypes getRevealedRouteType(TeamTypes eTeam) const;
	bool setRevealedRouteType(TeamTypes eTeam, RouteTypes eNewValue);

	int getBuildProgress(BuildTypes eBuild) const;
	bool changeBuildProgress(BuildTypes eBuild, int iChange, PlayerTypes ePlayer = NO_PLAYER);
	bool getAnyBuildProgress() const;
	void SilentlyResetAllBuildProgress();

	bool isLayoutDirty() const;							// The plot layout contains resources, routes, and improvements
	void setLayoutDirty(bool bDirty);
	bool isLayoutStateDifferent() const;
	void setLayoutStateToCurrent();
	void updateLayout(bool bDebug);

	void getVisibleImprovementState(ImprovementTypes& eType, bool& bWorked);				// determines how the improvement state is shown in the engine
	void getVisibleResourceState(ResourceTypes& eType, bool& bImproved, bool& bWorked);		// determines how the resource state is shown in the engine

	UnitHandle getCenterUnit();
	const UnitHandle getCenterUnit() const;
	const CvUnit* getDebugCenterUnit() const;
	void setCenterUnit(UnitHandle pNewValue);

	int getInvisibleVisibilityCount(TeamTypes eTeam, InvisibleTypes eInvisible) const;
	bool isInvisibleVisible(TeamTypes eTeam, InvisibleTypes eInvisible) const;
	void changeInvisibleVisibilityCount(TeamTypes eTeam, InvisibleTypes eInvisible, int iChange);

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	bool IsAvoidMovement(PlayerTypes ePlayer) const;
	void SetAvoidMovement(PlayerTypes ePlayer, bool bNewValue);
#endif

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
	bool IsAdjacentToImprovement(ImprovementTypes iImprovementType) const;
	bool IsWithinDistanceOfImprovement(ImprovementTypes iImprovementType, int iDistance) const;
	bool IsAdjacentToPlotType(PlotTypes iPlotType) const;
	bool IsWithinDistanceOfPlotType(PlotTypes iPlotType, int iDistance) const;
	bool IsAdjacentToResource(ResourceTypes iResourceType) const;
	bool IsWithinDistanceOfResource(ResourceTypes iResourceType, int iDistance) const;
	bool IsAdjacentToTerrain(TerrainTypes iTerrainType) const;
	bool IsWithinDistanceOfTerrain(TerrainTypes iTerrainType, int iDistance) const;
#endif

protected:
	class PlotBoolField
	{
	public:
		enum { eCount = 4, eSize = 32 };
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
	char /*PlayerTypes*/  m_eOwner;
	char /*PlotTypes*/    m_ePlotType;
	char /*TerrainTypes*/ m_eTerrainType;

	PlotBoolField m_bfRevealed;

	FFastSmallFixedList<IDInfo, 8, true, c_eCiv5GameplayDLL > m_units;

	IDInfo m_plotCity;
	IDInfo m_workingCity;
	IDInfo m_workingCityOverride;
	IDInfo m_ResourceLinkedCity;
	IDInfo m_purchaseCity;

	friend class CvMap;
	short* m_aiYield;
	int* m_aiFoundValue;
	char* m_aiPlayerCityRadiusCount;
	short* m_aiVisibilityCount;
	char* m_aiRevealedOwner;
	//bool *m_abRevealed;

	short /*ImprovementTypes*/ *m_aeRevealedImprovementType;
	short /*RouteTypes*/ *m_aeRevealedRouteType;
	bool* m_abNoSettling;

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	bool* m_abAvoidMovement;
#endif

	bool* m_abResourceForceReveal;


	char* m_szScriptData;
	short* m_paiBuildProgress;

	UnitHandle m_pCenterUnit;

	short m_apaiInvisibleVisibilityCount[REALLY_MAX_TEAMS][NUM_INVISIBLE_TYPES];

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

	uint m_uiTradeRouteBitFlags;


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
	bool m_bIsImpassable:1;					// Cached value, do not serialize

	CvArchaeologyData m_kArchaeologyData;

	void processArea(CvArea* pArea, int iChange);
	void doImprovementUpgrade();
	void updateImpassable();


	// added so under cheat mode we can access protected stuff
	friend class CvGameTextMgr;
};

namespace FSerialization
{
void SyncPlots();
void ClearPlotDeltas();
}

#endif
