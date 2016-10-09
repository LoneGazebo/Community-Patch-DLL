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
    TACTICAL_FLAG_ENEMY_COMBAT_UNIT          = 0x00020000, // Enemy combat unit here?
    TACTICAL_FLAG_NEUTRAL_COMBAT_UNIT		 = 0x00040000, // Neutral combat unit here?

    // DYNAMIC FLAGS - updated for each zone or target
    TACTICAL_FLAG_WITHIN_RANGE_OF_TARGET	 = 0x02000000,	// Is this a plot we can use to bombard the target?
    TACTICAL_FLAG_CAN_USE_TO_FLANK			 = 0x04000000,  // Does this plot help provide a flanking bonus on target?
    TACTICAL_FLAG_SAFE_DEPLOYMENT			 = 0x08000000,  // Should be a safe spot to deploy ranged units

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

	bool IsSafeForDeployment()
	{
		return GetBit(TACTICAL_FLAG_SAFE_DEPLOYMENT);
	};
	void SetSafeForDeployment(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_SAFE_DEPLOYMENT, bNewValue);
	};

	bool IsEnemyCombatUnit()
	{
		return GetBit(TACTICAL_FLAG_ENEMY_COMBAT_UNIT);
	};
	void SetEnemyCombatUnit(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_ENEMY_COMBAT_UNIT, bNewValue);
	};
	bool IsNeutralCombatUnit()
	{
		return GetBit(TACTICAL_FLAG_NEUTRAL_COMBAT_UNIT);
	};
	void SetNeutralCombatUnit(bool bNewValue)
	{
		SetBit(TACTICAL_FLAG_NEUTRAL_COMBAT_UNIT, bNewValue);
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

	int GetTargetDistance() const
	{
		return m_iTargetDistance;
	};
	void SetTargetDistance(int iDistance)
	{
		m_iTargetDistance = iDistance;
	};
	bool GetHasLOS() const
	{
		return m_bHasLOSToTarget;
	};
	void SetHasLOS(bool bValue)
	{
		m_bHasLOSToTarget = bValue;
	};


	friend FDataStream& operator<<(FDataStream& saveTo, const CvTacticalAnalysisCell& readFrom);
	friend FDataStream& operator>>(FDataStream& loadFrom, CvTacticalAnalysisCell& writeTo);

private:
	int m_iDeploymentScore;
	AITacticalTargetType m_eTargetType;
	int m_iDominanceZoneID;
	int m_iTargetDistance;
	bool m_bHasLOSToTarget;
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
	CvCity* GetZoneCity() const;
	void SetZoneCity(CvCity* pCity);
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

	inline int GetFriendlyNavalStrength() const
	{
		return m_iFriendlyNavalStrength;
	};
	inline void AddFriendlyNavalStrength(int iStrength)
	{
		m_iFriendlyNavalStrength += iStrength;
	};
	inline int GetEnemyNavalStrength() const
	{
		return m_iEnemyNavalStrength;
	};
	inline void AddEnemyNavalStrength(int iStrength)
	{
		m_iEnemyNavalStrength += iStrength;
	};
	inline int GetFriendlyNavalRangedStrength() const
	{
		return m_iFriendlyNavalRangedStrength;
	};
	inline void AddFriendlyNavalRangedStrength(int iRangedStrength)
	{
		m_iFriendlyNavalRangedStrength += iRangedStrength;
	};
	inline int GetEnemyNavalRangedStrength() const
	{
		return m_iEnemyNavalRangedStrength;
	};
	inline void AddEnemyNavalRangedStrength(int iRangedStrength)
	{
		m_iEnemyNavalRangedStrength += iRangedStrength;
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
	inline void AddFriendlyMeleeUnitCount(int iUnitCount)
	{
		m_iFriendlyMeleeUnitCount += iUnitCount;
	};
	inline void AddEnemyMeleeUnitCount(int iUnitCount)
	{
		m_iEnemyMeleeUnitCount += iUnitCount;
	};
	inline int GetFriendlyMeleeUnitCount() const
	{
		return m_iFriendlyMeleeUnitCount;
	};
	inline int GetEnemyMeleeUnitCount() const
	{
		return m_iEnemyMeleeUnitCount;
	};
	inline void AddNeutralUnitCount(int iUnitCount)
	{
		m_iNeutralUnitCount += iUnitCount;
	};
	inline void AddNeutralStrength(int iUnitStrength)
	{
		m_iNeutralUnitStrength += iUnitStrength;
	};
	inline int GetNeutralUnitCount() const
	{
		return m_iNeutralUnitCount;
	};
	inline int GetNeutralStrength() const
	{
		return m_iNeutralUnitStrength;
	};
	inline int GetEnemyRangedUnitCount() const
	{
		return m_iEnemyRangedUnitCount;
	};
	inline void AddEnemyRangedUnitCount(int iUnitCount)
	{
		m_iEnemyRangedUnitCount += iUnitCount;
	};
	inline int GetEnemyNavalUnitCount() const
	{
		return m_iEnemyNavalUnitCount;
	};
	inline void AddEnemyNavalUnitCount(int iUnitCount)
	{
		m_iEnemyNavalUnitCount += iUnitCount;
	};
	inline int GetFriendlyNavalUnitCount() const
	{
		return m_iFriendlyNavalUnitCount;
	};
	inline void AddFriendlyNavalUnitCount(int iUnitCount)
	{
		m_iFriendlyNavalUnitCount += iUnitCount;
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
	TacticalMoveZoneType GetZoneType() const;

#if defined(MOD_BALANCE_CORE_MILITARY)
	void Extend(CvPlot* pPlot);
	int GetCenterX() const { return (m_iAvgX+500)/1000; }
	int GetCenterY() const { return (m_iAvgY+500)/1000; }
	int GetNumPlots() const { return m_iPlotCount; }
	const std::vector<int>& GetNeighboringZones() const { return m_vNeighboringZones; }
	void AddNeighboringZone(int iZoneID);
	void ClearNeighboringZones() { m_vNeighboringZones.clear(); }
#endif

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
	int m_iFriendlyNavalStrength;
	int m_iEnemyNavalStrength;
	int m_iFriendlyNavalRangedStrength;
	int m_iEnemyNavalRangedStrength;
	int m_iFriendlyUnitCount;
	int m_iEnemyUnitCount;
	int m_iFriendlyRangedUnitCount;
	int m_iFriendlyMeleeUnitCount;
	int m_iEnemyMeleeUnitCount;
	int m_iNeutralUnitCount;
	int m_iNeutralUnitStrength;
	int m_iEnemyRangedUnitCount;
	int m_iEnemyNavalUnitCount;
	int m_iFriendlyNavalUnitCount;
	int m_iZoneValue;
	int m_iRangeClosestEnemyUnit;
	bool m_bIsWater;
	bool m_bIsNavalInvasion;

	int m_iAvgX, m_iAvgY;
	int m_iPlotCount;
	std::vector<int> m_vNeighboringZones;

	friend FDataStream& operator<<(FDataStream& saveTo, const CvTacticalDominanceZone& readFrom);
	friend FDataStream& operator>>(FDataStream& loadFrom, CvTacticalDominanceZone& writeTo);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvTacticalAnalysisMap
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTacticalAnalysisMap
{
public:
	CvTacticalAnalysisMap(void);
	~CvTacticalAnalysisMap(void);

	void Init(PlayerTypes ePlayer);
	void Refresh();
	bool IsUpToDate();

	void EstablishZoneNeighborhood();
	int GetNumZones() const
	{
		return m_DominanceZones.size();
	};

	CvTacticalDominanceZone* GetZoneByIndex(int iIndex);
	CvTacticalDominanceZone* GetZoneByCity(CvCity* pCity, bool bWater);
	CvTacticalDominanceZone* GetZoneByID(int iID);
	CvTacticalDominanceZone* GetZoneByPlot(CvPlot* pPlot);

	CvTacticalAnalysisCell* GetCell(int iPlotIndex)
	{
		return (iPlotIndex>=0 && iPlotIndex<(int)m_pCells.size()) ? &m_pCells[iPlotIndex] : NULL;
	}
	int GetDominancePercentage() const
	{
		return m_iDominancePercentage;
	}
	bool IsInEnemyDominatedZone(CvPlot* pPlot);

	// Routines to update the map
	void ClearDynamicFlags();

	// Range variable to keep dominance zones and tactical AI in sync
	int GetTacticalRange() const {return m_iTacticalRange;}

#if defined(MOD_BALANCE_CORE_MILITARY_LOGGING)
	void Dump();
#endif

protected:
	void AddTemporaryZones();
	bool PopulateCell(int iIndex, CvPlot* pPlot);
	void AddToDominanceZones(int iIndex, CvTacticalAnalysisCell* pCell);
	void CalculateMilitaryStrengths();
	void PrioritizeZones();
	void LogZones();
	void BuildEnemyUnitList();
	void MarkCellsNearEnemy();
	CvTacticalDominanceZone* MergeWithExistingZone(CvTacticalDominanceZone* pNewZone);
	eTacticalDominanceFlags ComputeDominance(CvTacticalDominanceZone* pZone);

	// Cached global define values
	int m_iDominancePercentage;
	int m_iUnitStrengthMultiplier;
	int m_iTacticalRange;

	PlayerTypes m_ePlayer;
	std::vector<CvTacticalAnalysisCell> m_pCells;
	int m_iTurnBuilt;

	std::vector<CvTacticalDominanceZone> m_DominanceZones;
	std::vector<IDInfo> m_EnemyUnits;
	std::vector<IDInfo> m_EnemyCities;

	friend FDataStream& operator<<(FDataStream& saveTo, const CvTacticalAnalysisMap& readFrom);
	friend FDataStream& operator>>(FDataStream& loadFrom, CvTacticalAnalysisMap& writeTo);
};


#endif //CIV5_TACTICAL_ANALYSIS_MAP_H