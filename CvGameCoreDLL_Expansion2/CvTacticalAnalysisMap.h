/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_TACTICAL_ANALYSIS_MAP_H
#define CIV5_TACTICAL_ANALYSIS_MAP_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvTacticalDominanceZone
//!  \brief		A tactical sector of the map (in owned territory it is divided by city)
//
//!  Key Attributes:
//!  - Array of these objects created by CvTacticalAnalysisMap::Init()
//!  - Objects refilled and sorted each turn
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

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
    TACTICAL_TERRITORY_FRIENDLY,
    TACTICAL_TERRITORY_ENEMY,
    TACTICAL_TERRITORY_NEUTRAL,
};

enum eTacticalPosture
{
    TACTICAL_POSTURE_NONE,
    TACTICAL_POSTURE_WITHDRAW,
    TACTICAL_POSTURE_ATTRIT_FROM_RANGE,
    TACTICAL_POSTURE_EXPLOIT_FLANKS,
    TACTICAL_POSTURE_STEAMROLL,
    TACTICAL_POSTURE_SURGICAL_CITY_STRIKE,
    TACTICAL_POSTURE_HEDGEHOG,
    TACTICAL_POSTURE_COUNTERATTACK,
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
	inline int GetZoneID() const
	{
		return m_iZoneID;
	}
	inline void SetZoneID(int iID)
	{
		m_iZoneID = iID;
	}
	inline eDominanceTerritoryTypes GetTerritoryType() const
	{
		return m_eTerritoryType;
	}
	inline void SetTerritoryType(eDominanceTerritoryTypes eTerritoryType)
	{
		m_eTerritoryType = eTerritoryType;
	}
	inline eTacticalDominanceFlags GetOverallDominanceFlag() const
	{
		return m_eOverallDominanceFlag;
	}

	eTacticalDominanceFlags CvTacticalDominanceZone::GetRangedDominanceFlag(int iDominancePercentage) const;
	eTacticalDominanceFlags CvTacticalDominanceZone::GetUnitCountDominanceFlag(int iDominancePercentage) const;
	eTacticalDominanceFlags CvTacticalDominanceZone::GetNavalRangedDominanceFlag(int iDominancePercentage) const;
	eTacticalDominanceFlags CvTacticalDominanceZone::GetNavalUnitCountDominanceFlag(int iDominancePercentage) const;

	inline eTacticalPosture GetPosture() const
	{
		return m_ePosture;
	}

	inline PlayerTypes GetOwner() const
	{
		return m_eOwner;
	}


	CvCity* GetZoneCity() const;
	void SetZoneCity(CvCity* pCity);
	inline int GetAreaID() const
	{
		return m_iAreaID;
	};
	inline void SetAreaID(int iID)
	{
		m_iAreaID = iID;
	}
	inline unsigned int GetOverallFriendlyStrength() const
	{
		//siege units are very vulnerable to melee units, so give a bonus to melee
		return (m_iFriendlyMeleeStrength*4)/3 + m_iFriendlyNavalStrength + m_iFriendlyRangedStrength + m_iFriendlyNavalRangedStrength;
	}
	inline unsigned int GetOverallEnemyStrength() const
	{
		//siege units are very vulnerable to melee units, so give a bonus to melee
		return (m_iEnemyMeleeStrength*4)/3 + m_iEnemyNavalStrength + m_iEnemyRangedStrength + m_iEnemyNavalRangedStrength;
	}
	inline unsigned int GetFriendlyMeleeStrength() const
	{
		return m_iFriendlyMeleeStrength;
	}
	inline void AddFriendlyMeleeStrength(int iStrength)
	{
		m_iFriendlyMeleeStrength += iStrength;
	}
	inline unsigned int GetEnemyMeleeStrength() const
	{
		return m_iEnemyMeleeStrength;
	}
	inline void AddEnemyMeleeStrength(int iStrength)
	{
		m_iEnemyMeleeStrength += iStrength;
	}
	inline unsigned int GetFriendlyRangedStrength() const
	{
		return m_iFriendlyRangedStrength;
	}
	inline void AddFriendlyRangedStrength(int iRangedStrength)
	{
		m_iFriendlyRangedStrength += iRangedStrength;
	}
	inline unsigned int GetEnemyRangedStrength() const
	{
		return m_iEnemyRangedStrength;
	}
	inline void AddEnemyRangedStrength(int iRangedStrength)
	{
		m_iEnemyRangedStrength += iRangedStrength;
	}
	inline unsigned int GetFriendlyNavalStrength() const
	{
		return m_iFriendlyNavalStrength;
	}
	inline void AddFriendlyNavalStrength(int iStrength)
	{
		m_iFriendlyNavalStrength += iStrength;
	}
	inline unsigned int GetEnemyNavalStrength() const
	{
		return m_iEnemyNavalStrength;
	}
	inline void AddEnemyNavalStrength(int iStrength)
	{
		m_iEnemyNavalStrength += iStrength;
	}
	inline unsigned int GetFriendlyNavalRangedStrength() const
	{
		return m_iFriendlyNavalRangedStrength;
	}
	inline void AddFriendlyNavalRangedStrength(int iRangedStrength)
	{
		m_iFriendlyNavalRangedStrength += iRangedStrength;
	}
	inline unsigned int GetEnemyNavalRangedStrength() const
	{
		return m_iEnemyNavalRangedStrength;
	}
	inline void AddEnemyNavalRangedStrength(int iRangedStrength)
	{
		m_iEnemyNavalRangedStrength += iRangedStrength;
	}
	inline int GetTotalFriendlyUnitCount() const
	{
		return m_iFriendlyUnitCount + m_iFriendlyNavalUnitCount;
	}
	inline void AddFriendlyUnitCount(int iUnitCount)
	{
		m_iFriendlyUnitCount += iUnitCount;
	}
	inline int GetTotalEnemyUnitCount() const
	{
		return m_iEnemyUnitCount + m_iEnemyNavalUnitCount;
	}
	inline void AddEnemyUnitCount(int iUnitCount)
	{
		m_iEnemyUnitCount += iUnitCount;
	}
	inline void AddNeutralUnitCount(int iUnitCount)
	{
		m_iNeutralUnitCount += iUnitCount;
	}
	inline void AddNeutralStrength(int iUnitStrength)
	{
		m_iNeutralUnitStrength += iUnitStrength;
	}
	inline int GetNeutralUnitCount() const
	{
		return m_iNeutralUnitCount;
	}
	inline unsigned int GetNeutralStrength() const
	{
		return m_iNeutralUnitStrength;
	}
	inline int GetEnemyNavalUnitCount() const
	{
		return m_iEnemyNavalUnitCount;
	}
	inline void AddEnemyNavalUnitCount(int iUnitCount)
	{
		m_iEnemyNavalUnitCount += iUnitCount;
	}
	inline int GetFriendlyNavalUnitCount() const
	{
		return m_iFriendlyNavalUnitCount;
	}
	inline void AddFriendlyNavalUnitCount(int iUnitCount)
	{
		m_iFriendlyNavalUnitCount += iUnitCount;
	}
	inline int GetDominanceZoneValue() const
	{
		return m_iZoneValue;
	}
	inline void SetDominanceZoneValue(int iValue)
	{
		m_iZoneValue = iValue;
	}
	inline DomainTypes GetDomain() const
	{
		//encoded into the sign of the ID
		return m_iZoneID < 0 ? DOMAIN_SEA : DOMAIN_LAND;
	}
	inline bool IsWater() const
	{
		return m_iZoneID < 0;
	}

#if defined(MOD_BALANCE_CORE_MILITARY)
	void Extend(CvPlot* pPlot);
	int GetCenterX() const { return (m_iAvgX+500)/1000; }
	int GetCenterY() const { return (m_iAvgY+500)/1000; }
	int GetNumPlots() const { return m_iPlotCount; }
	const std::vector<int>& GetNeighboringZones() const { return m_vNeighboringZones; }
	void AddNeighboringZone(int iZoneID);
	void ClearNeighboringZones() { m_vNeighboringZones.clear(); }
	int GetBorderScore(DomainTypes eDomain, CvCity** ppWorstNeighborCity=NULL) const;
	bool HasNeighborZone(PlayerTypes eOwner) const;
	eTacticalDominanceFlags SetOverallDominance(int iDominancePercentage);
	eTacticalPosture SelectPostureSingleZone(int iDominancePercent);
	eTacticalPosture SelectPostureMultiZone(vector<CvTacticalDominanceZone*> vNeighbors);
#endif

private:
	int m_iZoneID;
	eDominanceTerritoryTypes m_eTerritoryType;
	eTacticalDominanceFlags m_eOverallDominanceFlag;
	eTacticalPosture m_ePosture;
	PlayerTypes m_eOwner;
	int m_iCityID;
	int m_iAreaID;
	unsigned int m_iFriendlyMeleeStrength;
	unsigned int m_iEnemyMeleeStrength;
	unsigned int m_iFriendlyRangedStrength;
	unsigned int m_iEnemyRangedStrength;
	unsigned int m_iFriendlyNavalStrength;
	unsigned int m_iEnemyNavalStrength;
	unsigned int m_iFriendlyNavalRangedStrength;
	unsigned int m_iEnemyNavalRangedStrength;
	int m_iFriendlyUnitCount; //all land units
	int m_iEnemyUnitCount; //all land units
	int m_iNeutralUnitCount;
	unsigned int m_iNeutralUnitStrength;
	int m_iEnemyNavalUnitCount; //all naval units
	int m_iFriendlyNavalUnitCount; //all naval units
	int m_iZoneValue;

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

	void Reset(PlayerTypes ePlayer);
	void Refresh(bool force = false);
	bool IsUpToDate();
	void Invalidate();

	CvTacticalDominanceZone* GetZoneByIndex(int iIndex);
	CvTacticalDominanceZone* GetZoneByCity(const CvCity* pCity, bool bWater);
	CvTacticalDominanceZone* GetZoneByID(int iID);
	CvTacticalDominanceZone* GetZoneByPlot(const CvPlot* pPlot);

	int GetDominanceZoneID(int iPlotIndex);
	bool IsInEnemyDominatedZone(const CvPlot* pPlot);
	int GetNumZones();

protected:
	void CreateDominanceZones();
	void CalculateMilitaryStrengths();
	void PrioritizeZones();
	void LogZones();
	void UpdatePostures();
	void EstablishZoneNeighborhood();

	PlayerTypes m_ePlayer;
	int m_iTurnSliceBuilt;

	std::vector<int> m_vPlotZoneID; //zone id for each plot
	std::map<int, int> m_IdLookup; //id to index
	std::vector<CvTacticalDominanceZone> m_vDominanceZones;

	friend FDataStream& operator<<(FDataStream& saveTo, const CvTacticalAnalysisMap& readFrom);
	friend FDataStream& operator>>(FDataStream& loadFrom, CvTacticalAnalysisMap& writeTo);
};


#endif //CIV5_TACTICAL_ANALYSIS_MAP_H