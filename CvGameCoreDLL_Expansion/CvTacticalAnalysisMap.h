/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_TACTICAL_ANALYSIS_MAP_H
#define CIV5_TACTICAL_ANALYSIS_MAP_H

#include "FBitFlags.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvTacticalAnalysisPlot
//!  \brief		Per plot data stored inside CvTacticalAnalysisMap for each hex in the game
//
//!  Key Attributes:
//!  - Array of these objects created by CvTacticalAnalysisMap::Init()
//!  - Destroyed in CvTacticalAnalysisMap destructor
//!  - Holds player-specific map data
//!  - Filled in for each plot at the start of AI processing for a player turn
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
enum eTacticalAnalysisFlags
{
    // STATIC FLAGS - don't change as we process different zones and targets
    TACTICAL_FLAG_REVEALED					 = 0x00000001, // Is this plot revealed to this player?
    TACTICAL_FLAG_VISIBLE                    = 0x00000002, // Is this plot visible to this player?
    TACTICAL_FLAG_IMPASSABLE_TERRAIN         = 0x00000004, // Is this terrain impassable to this player?
    TACTICAL_FLAG_IMPASSABLE_TERRITORY       = 0x00000008, // Is this neutral territory impassable to this player?
    TACTICAL_FLAG_NOT_VISIBLE_TO_ENEMY       = 0x00000010, // A tile no enemy unit can see?
    TACTICAL_FLAG_SUBJECT_TO_ENEMY_ATTACK    = 0x00000020, // Enemy can strike at a unit here
    TACTICAL_FLAG_ENEMY_CAN_REACH_WITH_MOVES = 0x00000040, // Enemy can move to this tile and still have movement left this turn
    TACTICAL_FLAG_FRIENDLY_TURN_END_TILE     = 0x00000080, // Is one of our friendly units ending its move here?
    TACTICAL_FLAG_FRIENDLY_CITY              = 0x00000100, // Friendly city here?
    TACTICAL_FLAG_ENEMY_CITY                 = 0x00000200, // Enemy city here?
    TACTICAL_FLAG_NEUTRAL_CITY				 = 0x00000400, // Neutral city here?
    TACTICAL_FLAG_WATER						 = 0x00000800, // Water?
    TACTICAL_FLAG_OCEAN						 = 0x00001000, // Ocean?
    TACTICAL_FLAG_OWN_TERRITORY				 = 0x00002000, // Territory owned by the active player
    TACTICAL_FLAG_FRIENDLY_TERRITORY		 = 0x00004000, // Territory owned by allies
    TACTICAL_FLAG_ENEMY_TERRITORY			 = 0x00008000, // Territory owned by enemies
    TACTICAL_FLAG_UNCLAIMED_TERRITORY	     = 0x00010000, // Territory that is unclaimed

    // DYNAMIC FLAGS - updated for each zone or target
    TACTICAL_FLAG_WITHIN_RANGE_OF_TARGET	 = 0x00020000,	// Is this a plot we can use to bombard the target?
    TACTICAL_FLAG_CAN_USE_TO_FLANK			 = 0x00040000,        // Does this plot help provide a flanking bonus on target?
    TACTICAL_FLAG_SAFE_DEPLOYMENT			 = 0x00080000,         // Should be a safe spot to deploy ranged units
};

enum AITacticalTargetType
{
    AI_TACTICAL_TARGET_NONE,
    AI_TACTICAL_TARGET_CITY,
    AI_TACTICAL_TARGET_BARBARIAN_CAMP,
    AI_TACTICAL_TARGET_IMPROVEMENT,
    AI_TACTICAL_TARGET_BLOCKADE_RESOURCE_POINT,
    AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT,    // Can't attack one of our cities
    AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, // Can damage one of our cities
    AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT,   // Can contribute to capturing one of our cities
    AI_TACTICAL_TARGET_CITY_TO_DEFEND,
    AI_TACTICAL_TARGET_IMPROVEMENT_TO_DEFEND,
    AI_TACTICAL_TARGET_DEFENSIVE_BASTION,
    AI_TACTICAL_TARGET_ANCIENT_RUINS,
    AI_TACTICAL_TARGET_BOMBARDMENT_ZONE,     // Used for naval bombardment operation
    AI_TACTICAL_TARGET_EMBARKED_MILITARY_UNIT,
    AI_TACTICAL_TARGET_EMBARKED_CIVILIAN,
    AI_TACTICAL_TARGET_VERY_HIGH_PRIORITY_CIVILIAN,
    AI_TACTICAL_TARGET_HIGH_PRIORITY_CIVILIAN,
    AI_TACTICAL_TARGET_MEDIUM_PRIORITY_CIVILIAN,
    AI_TACTICAL_TARGET_LOW_PRIORITY_CIVILIAN,
};

class CvTacticalAnalysisCell
{
	FBITFLAGS(uint);

public:
	CvTacticalAnalysisCell(void);

	void Clear();

	bool IsRevealed()
	{
		return GetBit(TACTICAL_FLAG_REVEALED);
	};
	void SetRevealed(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_REVEALED, bNewValue);
	};
	bool IsVisible()
	{
		return GetBit(TACTICAL_FLAG_VISIBLE);
	};
	void SetVisible(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_VISIBLE, bNewValue);
	};
	bool IsImpassableTerrain()
	{
		return GetBit(TACTICAL_FLAG_IMPASSABLE_TERRAIN);
	};
	void SetImpassableTerrain(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_IMPASSABLE_TERRAIN, bNewValue);
	};
	bool IsImpassableTerritory()
	{
		return GetBit(TACTICAL_FLAG_IMPASSABLE_TERRITORY);
	};
	void SetImpassableTerritory(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_IMPASSABLE_TERRITORY, bNewValue);
	};
	bool IsNotVisibleToEnemy()
	{
		return GetBit(TACTICAL_FLAG_NOT_VISIBLE_TO_ENEMY);
	};
	void SetNotVisibleToEnemy(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_NOT_VISIBLE_TO_ENEMY, bNewValue);
	};
	bool IsSubjectToAttack()
	{
		return GetBit(TACTICAL_FLAG_SUBJECT_TO_ENEMY_ATTACK);
	};
	void SetSubjectToAttack(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_SUBJECT_TO_ENEMY_ATTACK, bNewValue);
	};
	bool IsEnemyCanMovePast()
	{
		return GetBit(TACTICAL_FLAG_ENEMY_CAN_REACH_WITH_MOVES);
	};
	void SetEnemyCanMovePast(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_ENEMY_CAN_REACH_WITH_MOVES, bNewValue);
	};
	bool IsFriendlyTurnEndTile()
	{
		return GetBit(TACTICAL_FLAG_FRIENDLY_TURN_END_TILE);
	};
	void SetFriendlyTurnEndTile(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_FRIENDLY_TURN_END_TILE, bNewValue);
	};
	bool IsFriendlyCity()
	{
		return GetBit(TACTICAL_FLAG_FRIENDLY_CITY);
	};
	void SetFriendlyCity(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_FRIENDLY_CITY, bNewValue);
	};
	bool IsEnemyCity()
	{
		return GetBit(TACTICAL_FLAG_ENEMY_CITY);
	};
	void SetEnemyCity(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_ENEMY_CITY, bNewValue);
	};
	bool IsNeutralCity()
	{
		return GetBit(TACTICAL_FLAG_NEUTRAL_CITY);
	};
	void SetNeutralCity(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_NEUTRAL_CITY, bNewValue);
	};
	bool IsCity()
	{
		return IsFriendlyCity() || IsEnemyCity() || IsNeutralCity();
	}
	bool IsWater()
	{
		return GetBit(TACTICAL_FLAG_WATER);
	};
	void SetWater(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_WATER, bNewValue);
	};
	bool IsOcean()
	{
		return GetBit(TACTICAL_FLAG_OCEAN);
	};
	void SetOcean(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_OCEAN, bNewValue);
	};
	bool IsOwnTerritory()
	{
		return GetBit(TACTICAL_FLAG_OWN_TERRITORY);
	};
	void SetOwnTerritory(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_OWN_TERRITORY, bNewValue);
	};
	bool IsFriendlyTerritory()
	{
		return GetBit(TACTICAL_FLAG_FRIENDLY_TERRITORY);
	};
	void SetFriendlyTerritory(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_FRIENDLY_TERRITORY, bNewValue);
	};
	bool IsEnemyTerritory()
	{
		return GetBit(TACTICAL_FLAG_ENEMY_TERRITORY);
	};
	void SetEnemyTerritory(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_ENEMY_TERRITORY, bNewValue);
	};
	bool IsUnclaimedTerritory()
	{
		return GetBit(TACTICAL_FLAG_UNCLAIMED_TERRITORY);
	};
	void SetUnclaimedTerritory(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_UNCLAIMED_TERRITORY, bNewValue);
	};

	bool IsWithinRangeOfTarget()
	{
		return GetBit(TACTICAL_FLAG_WITHIN_RANGE_OF_TARGET);
	};
	void SetWithinRangeOfTarget(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_WITHIN_RANGE_OF_TARGET, bNewValue);
	};
	bool IsHelpsProvidesFlankBonus()
	{
		return GetBit(TACTICAL_FLAG_CAN_USE_TO_FLANK);
	};
	void SetHelpsProvidesFlankBonus(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_CAN_USE_TO_FLANK, bNewValue);
	};
	bool IsSafeForDeployment()
	{
		return GetBit(TACTICAL_FLAG_SAFE_DEPLOYMENT);
	};
	void SetSafeForDeployment(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_SAFE_DEPLOYMENT, bNewValue);
	};

	CvUnit* GetEnemyMilitaryUnit()
	{
		return m_pEnemyMilitary;
	};
	void SetEnemyMilitaryUnit(CvUnit* pUnit)
	{
		m_pEnemyMilitary = pUnit;
	};
	CvUnit* GetNeutralMilitaryUnit()
	{
		return m_pNeutralMilitary;
	};
	void SetNeutralMilitaryUnit(CvUnit* pUnit)
	{
		m_pNeutralMilitary = pUnit;
	};
	CvUnit* GetFriendlyMilitaryUnit()
	{
		return m_pFriendlyMilitary;
	};
	void SetFriendlyMilitaryUnit(CvUnit* pUnit)
	{
		m_pFriendlyMilitary = pUnit;
	};
	CvUnit* GetEnemyCivilianUnit()
	{
		return m_pEnemyCivilian;
	};
	void SetEnemyCivilianUnit(CvUnit* pUnit)
	{
		m_pEnemyCivilian = pUnit;
	};
	CvUnit* GetNeutralCivilianUnit()
	{
		return m_pNeutralCivilian;
	};
	void SetNeutralCivilianUnit(CvUnit* pUnit)
	{
		m_pNeutralCivilian = pUnit;
	};
	CvUnit* GetFriendlyCivilianUnit()
	{
		return m_pFriendlyCivilian;
	};
	void SetFriendlyCivilianUnit(CvUnit* pUnit)
	{
		m_pFriendlyCivilian = pUnit;
	};

	int GetDefenseModifier() const
	{
		return m_iDefenseModifier;
	};
	void SetDefenseModifier(int iModifier)
	{
		m_iDefenseModifier = iModifier;
	};
	int GetDeploymentScore() const
	{
		return m_iDeploymentScore;
	};
	void SetDeploymentScore(int iScore)
	{
		m_iDeploymentScore = iScore;
	};
	int GetDominanceZone() const
	{
		return m_iDominanceZoneID;
	};
	void SetDominanceZone(int iZone)
	{
		m_iDominanceZoneID = iZone;
	};
	AITacticalTargetType GetTargetType() const
	{
		return m_eTargetType;
	};
	void SetTargetType(AITacticalTargetType eTargetType)
	{
		m_eTargetType = eTargetType;
	};

	bool CanUseForOperationGathering();
	bool CanUseForOperationGatheringCheckWater(bool bWater=false);

private:
	CvUnit* m_pEnemyMilitary;
	CvUnit* m_pEnemyCivilian;
	CvUnit* m_pNeutralMilitary;
	CvUnit* m_pNeutralCivilian;
	CvUnit* m_pFriendlyMilitary;
	CvUnit* m_pFriendlyCivilian;

	int m_iDefenseModifier;
	int m_iDeploymentScore;
	AITacticalTargetType m_eTargetType;
	int m_iDominanceZoneID;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvTacticalDominanceZone
//!  \brief		A tactical sector of the map (in owned territory it is divided by city)
//
//!  Key Attributes:
//!  - Array of these objects created by CvTacticalAnalysisMap::Init()
//!  - Objects refilled and sorted each turn
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define SAFE_ESTIMATE_NUM_DOMINANCE_ZONES (5 * MAX_MAJOR_CIVS)

enum eTacticalDominanceFlags
{
    TACTICAL_DOMINANCE_NO_UNITS_VISIBLE,
    TACTICAL_DOMINANCE_FRIENDLY,
    TACTICAL_DOMINANCE_ENEMY,
    TACTICAL_DOMINANCE_EVEN,
};

enum eDominanceTerritoryTypes
{
    TACTICAL_TERRITORY_NONE,
    TACTICAL_TERRITORY_NO_OWNER,
    TACTICAL_TERRITORY_FRIENDLY,
    TACTICAL_TERRITORY_ENEMY,
    TACTICAL_TERRITORY_NEUTRAL,
    TACTICAL_TERRITORY_TEMP_ZONE,
};

class CvTacticalDominanceZone
{
public:
	CvTacticalDominanceZone(void);

	bool operator<(const CvTacticalDominanceZone& zone) const
	{
		return (m_iZoneValue > zone.m_iZoneValue);
	}

	// Accessor functions
	inline int GetDominanceZoneID() const
	{
		return m_iDominanceZoneID;
	};
	inline void SetDominanceZoneID(int iID)
	{
		m_iDominanceZoneID = iID;
	};
	inline eDominanceTerritoryTypes GetTerritoryType() const
	{
		return m_eTerritoryType;
	};
	inline void SetTerritoryType(eDominanceTerritoryTypes eTerritoryType)
	{
		m_eTerritoryType = eTerritoryType;
	};
	inline eTacticalDominanceFlags GetDominanceFlag() const
	{
		return m_eDominanceFlag;
	};
	inline void SetDominanceFlag(eTacticalDominanceFlags eDominanceFlag)
	{
		m_eDominanceFlag = eDominanceFlag;
	};
	inline PlayerTypes GetOwner() const
	{
		return m_eOwner;
	};
	inline void SetOwner(PlayerTypes eOwner)
	{
		m_eOwner = eOwner;
	};
	CvCity* GetClosestCity() const;
	void SetClosestCity(CvCity* pCity);
	int GetRangeClosestEnemyUnit() const;
	void SetRangeClosestEnemyUnit(int iRange);
	inline int GetAreaID() const
	{
		return m_iAreaID;
	};
	inline void SetAreaID(int iID)
	{
		m_iAreaID = iID;
	};
	inline int GetFriendlyStrength() const
	{
		return m_iFriendlyStrength;
	};
	inline void AddFriendlyStrength(int iStrength)
	{
		m_iFriendlyStrength += iStrength;
	};
	inline int GetEnemyStrength() const
	{
		return m_iEnemyStrength;
	};
	inline void AddEnemyStrength(int iStrength)
	{
		m_iEnemyStrength += iStrength;
	};
	inline int GetFriendlyRangedStrength() const
	{
		return m_iFriendlyRangedStrength;
	};
	inline void AddFriendlyRangedStrength(int iRangedStrength)
	{
		m_iFriendlyRangedStrength += iRangedStrength;
	};
	inline int GetEnemyRangedStrength() const
	{
		return m_iEnemyRangedStrength;
	};
	inline void AddEnemyRangedStrength(int iRangedStrength)
	{
		m_iEnemyRangedStrength += iRangedStrength;
	};
	inline int GetFriendlyUnitCount() const
	{
		return m_iFriendlyUnitCount;
	};
	inline void AddFriendlyUnitCount(int iUnitCount)
	{
		m_iFriendlyUnitCount += iUnitCount;
	};
	inline int GetEnemyUnitCount() const
	{
		return m_iEnemyUnitCount;
	};
	inline void AddEnemyUnitCount(int iUnitCount)
	{
		m_iEnemyUnitCount += iUnitCount;
	};
	inline int GetFriendlyRangedUnitCount() const
	{
		return m_iFriendlyRangedUnitCount;
	};
	inline void AddFriendlyRangedUnitCount(int iUnitCount)
	{
		m_iFriendlyRangedUnitCount += iUnitCount;
	};
	inline int GetEnemyRangedUnitCount() const
	{
		return m_iEnemyRangedUnitCount;
	};
	inline void AddEnemyRangedUnitCount(int iUnitCount)
	{
		m_iEnemyRangedUnitCount += iUnitCount;
	};
	inline int GetDominanceZoneValue() const
	{
		return m_iZoneValue;
	};
	inline void SetDominanceZoneValue(int iValue)
	{
		m_iZoneValue = iValue;
	};
	inline bool IsWater() const
	{
		return m_bIsWater;
	};
	inline void SetWater(bool bWater)
	{
		m_bIsWater = bWater;
	};
	inline bool IsNavalInvasion() const
	{
		return m_bIsNavalInvasion;
	};
	inline void SetNavalInvasion(bool bIsNavalInvasion)
	{
		m_bIsNavalInvasion = bIsNavalInvasion;
	};
	inline CvPlot* GetTempZoneCenter() const
	{
		return m_pTempZoneCenter;
	};
	inline void SetTempZoneCenter(CvPlot* pPlot)
	{
		m_pTempZoneCenter = pPlot;
	};
	TacticalMoveZoneType GetZoneType() const;

private:
	int m_iDominanceZoneID;
	eDominanceTerritoryTypes m_eTerritoryType;
	eTacticalDominanceFlags m_eDominanceFlag;
	PlayerTypes m_eOwner;
	int m_iCityID;
	int m_iAreaID;
	int m_iFriendlyStrength;
	int m_iEnemyStrength;
	int m_iFriendlyRangedStrength;
	int m_iEnemyRangedStrength;
	int m_iFriendlyUnitCount;
	int m_iEnemyUnitCount;
	int m_iFriendlyRangedUnitCount;
	int m_iEnemyRangedUnitCount;
	int m_iZoneValue;
	int m_iRangeClosestEnemyUnit;
	bool m_bIsWater;
	bool m_bIsNavalInvasion;
	CvPlot* m_pTempZoneCenter;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvTacticalAnalysisEnemy
//!  \brief		A single enemy unit for use in tactical analysis processing
//
//!  Key Attributes:
//!  - Static vector of these created by BuildEnemyUnitList()
//!  - Referenced by MarkCellsNearEnemy() and other routines that need to check where
//!    enemy units can reach this turn
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define SAFE_ESTIMATE_NUM_ENEMIES 250

class CvTacticalAnalysisEnemy
{
public:
	CvTacticalAnalysisEnemy(void)
	{
		m_pUnit = NULL;
	}
	CvTacticalAnalysisEnemy(CvUnit* pUnit)
	{
		m_pUnit = pUnit;
	}
	CvUnit* GetUnit() const
	{
		return m_pUnit;
	};
	void SetID(CvUnit* pUnit)
	{
		m_pUnit = pUnit;
	};

private:
	CvUnit* m_pUnit;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvTacticalAnalysisMap
//!  \brief		Shared spatial map used by all AI players to analyze moves when at war
//
//!  Key Attributes:
//!  - Created by CvGame class
//!  - Shared by all players; data is refreshed at start of each AI turn if player at war
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTacticalAnalysisMap
{
public:
	CvTacticalAnalysisMap(void);
	~CvTacticalAnalysisMap(void);

	void Init(int iNumPlots);
	void RefreshDataForNextPlayer(CvPlayer* pPlayer);
	bool IsBuilt() const
	{
		return m_bIsBuilt;
	};
	int GetNumZones() const
	{
		return m_DominanceZones.size();
	};
	CvTacticalDominanceZone* GetZone(int iIndex);
	CvTacticalDominanceZone* GetZoneByCity(CvCity* pCity, bool bWater);
	CvTacticalAnalysisCell* GetCell(int iPlotIndex)
	{
		return &m_pPlots[iPlotIndex];
	};
	int GetDominancePercentage() const
	{
		return m_iDominancePercentage;
	};
	bool IsInEnemyDominatedZone(CvPlot* pPlot);

	// Properties of current player's military
	int GetBestFriendlyRange() const
	{
		return m_iBestFriendlyRange;
	};
	void SetBestFriendlyRange(int iNewRange)
	{
		m_iBestFriendlyRange = iNewRange;
	};
	bool CanIgnoreLOS() const
	{
		return m_bIgnoreLOS;
	};
	void SetIgnoreLOS(bool bNewValue)
	{
		m_bIgnoreLOS = bNewValue;
	};

	// Routines to update the map
	void ClearDynamicFlags();
	void SetTargetBombardCells(CvPlot* pTarget, int iRange, bool bIgnoreLOS);
	void SetTargetFlankBonusCells(CvPlot* pTarget);

	// Get the player the map was built for
	CvPlayer* GetPlayer()
	{
		return m_pPlayer;
	}

	// Range variable to keep dominance zones and tactical AI in sync
	int GetTacticalRange() const {return m_iTacticalRange;};

protected:
	void AddTemporaryZones();
	bool PopulateCell(int iIndex, CvPlot* pPlot);
	void AddToDominanceZones(int iIndex, CvTacticalAnalysisCell* pCell);
	void CalculateMilitaryStrengths();
	void PrioritizeZones();
	void LogZones();
	void BuildEnemyUnitList();
	void MarkCellsNearEnemy();
	CvTacticalDominanceZone* FindExistingZone(CvPlot* pPlot);
	eTacticalDominanceFlags ComputeDominance(CvTacticalDominanceZone* pZone);

	// Cached global define values
	int m_iDominancePercentage;
	int m_iUnitStrengthMultiplier;
	int m_iTacticalRange;

	CvTacticalAnalysisCell* m_pPlots;
	int m_iNumPlots;
	CvPlayer* m_pPlayer;
	int m_iTurnBuilt;
	int m_iBestFriendlyRange;
	bool m_bIgnoreLOS;
	bool m_bAtWar;
	bool m_bIsBuilt;
	CvTacticalDominanceZone m_TempZone;
	FStaticVector<CvTacticalDominanceZone, SAFE_ESTIMATE_NUM_DOMINANCE_ZONES, true, c_eCiv5GameplayDLL, 0> m_DominanceZones;
	FStaticVector<CvUnit*, SAFE_ESTIMATE_NUM_ENEMIES, true, c_eCiv5GameplayDLL, 0> m_EnemyUnits;
};


#endif //CIV5_TACTICAL_ANALYSIS_MAP_H