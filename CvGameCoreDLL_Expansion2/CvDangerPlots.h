/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_DANGER_PLOTS_H
#define CIV5_DANGER_PLOTS_H

#include <vector>
#include <set>
#include "CvEnums.h"

// Stores all possible damage sources on a tile (terrain, improvements, cities, units)
typedef std::vector<std::pair<PlayerTypes,int>> DangerUnitVector;
typedef std::vector<std::pair<PlayerTypes,int>> DangerCityVector;
typedef std::set<std::pair<PlayerTypes,int>> UnitSet;

struct SCachedUnitDanger
{
	static const size_t maxIgnoredUnits = 5;

	SCachedUnitDanger(const CvUnit* pUnit = NULL, int iSelfDamage = 0)
	{
		m_iUnitID = pUnit ? pUnit->GetID() : 0;
		m_iPlotIndex = pUnit ? pUnit->plot()->GetPlotIndex() : -1;
		m_damage = pUnit ? pUnit->getDamage() + iSelfDamage : iSelfDamage;
		memset(m_enemyUnitsToIgnore, 0, sizeof(m_enemyUnitsToIgnore));
	}

	bool addIgnoredUnits(const UnitIdContainer& enemyUnitsToIgnore)
	{
		//don't copy the vector that seems to be expensive
		if (enemyUnitsToIgnore.size() > maxIgnoredUnits)
			return false;

		for (size_t i = 0; i < enemyUnitsToIgnore.size(); i++)
			m_enemyUnitsToIgnore[i] = enemyUnitsToIgnore[i];

		return true;
	}

	const bool operator==(const SCachedUnitDanger& rhs) const
	{
		return (m_iUnitID==rhs.m_iUnitID && m_iPlotIndex==rhs.m_iPlotIndex && m_damage==rhs.m_damage && memcmp(m_enemyUnitsToIgnore, rhs.m_enemyUnitsToIgnore, sizeof(m_enemyUnitsToIgnore))==0);
	}

	int m_iUnitID;
	int m_iPlotIndex;
	int m_damage;
	int m_enemyUnitsToIgnore[maxIgnoredUnits]; //zero if unused
};

struct CvDangerPlotContents
{
	CvDangerPlotContents()
	{
		m_pPlot = NULL;
		reset();
	}

	void reset()
	{
		m_bFlatPlotDamage = false;
		m_iImprovementDamage = 0;
		m_iFogCount = 0;

		//make sure the allocated size doesn't grow too much over time
		m_apUnits.clear(); if (m_apUnits.capacity() > 5) { m_apUnits = DangerUnitVector(); m_apUnits.reserve(5); }
		m_apCities.clear(); //cities are fairly static so don't care to prune
		m_apCaptureUnits.clear(); if (m_apCaptureUnits.capacity() > 5) { DangerUnitVector().swap(m_apCaptureUnits); }

		//reset cache
		m_lastUnitDangerResults.clear();
	};

	void resetCache()
	{
		m_lastUnitDangerResults.clear();
	}

	int GetDanger(const CvUnit* pUnit, const UnitIdContainer& unitsToIgnore, int iExtraDamage, AirActionType iAirAction);
	int GetDanger(const CvCity* pCity, const CvUnit* pPretendGarrison = NULL);
	std::vector<CvUnit*> GetPossibleAttackers(TeamTypes eTeamForVisibilityCheck) const;

	// should not normally be used if a city or unit is known, primarily for compatibility
	int GetDanger(bool bFixedDamageOnly);

	// just for internal use
	int GetAirUnitDamage(const CvUnit* pUnit, AirActionType iAirAction = AIR_ACTION_ATTACK);

	CvPlot* m_pPlot;
	bool m_bFlatPlotDamage;
	int m_iImprovementDamage; //only one citadel can affect a unit at a time
	DangerUnitVector m_apUnits;
	DangerCityVector m_apCities;
	DangerUnitVector m_apCaptureUnits; //for civilians
	int m_iFogCount;

	//caching ...
	std::deque< std::pair<SCachedUnitDanger,int> > m_lastUnitDangerResults;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvDangerPlots
//!  \brief		Used to calculate the relative danger of a given plot for a player
//
//!  Key Attributes:
//!  - Replaces the AI_getPlotDanger function in CvPlayerAI
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvDangerPlots
{
public:
	CvDangerPlots(void);
	~CvDangerPlots(void);

	void Init(PlayerTypes ePlayer, bool bAllocate);
	void Uninit();

	void UpdateDanger(bool bKeepKnownUnits=true);
	int GetDanger(const CvPlot& pPlot, const CvUnit* pUnit, const UnitIdContainer& unitsToIgnore, int iExtraDamage=0, AirActionType iAirAction=AIR_ACTION_ATTACK);
	int GetDanger(const CvCity* pCity, const CvUnit* pPretendGarrison = NULL);
	int GetDanger(const CvPlot& pPlot, bool bFixedDamageOnly);

	std::vector<CvUnit*> GetPossibleAttackers(const CvPlot& Plot, TeamTypes eTeamForVisibilityCheck) const;
	void ResetDangerCache(const CvPlot* pCenterPlot, int iRange);
	bool IsKnownAttacker(const CvUnit* pUnit) const;
	bool AddKnownAttacker(const CvUnit* pUnit);

	void SetDirty();
	bool IsDirty() const { return m_bDirty; }
	int GetTurnSliceBuilt() const { return m_iTurnSliceBuilt; }

	template<typename DangerPlots, typename Visitor>
	static void Serialize(DangerPlots& dangerPlots, Visitor& visitor);
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

protected:

	void AddFogDanger(CvPlot* pOrigin, TeamTypes eEnemyTeam, int iRange, bool bCheckOwnership);
	void UpdateDangerInternal(bool bKeepKnownUnits, const PlotIndexContainer& plotsToIgnoreForZOC);
	bool UpdateDangerSingleUnit(const CvUnit* pUnit, bool bIgnoreVisibility, const PlotIndexContainer& plotsToIgnoreForZOC);

	bool ShouldIgnorePlayer(PlayerTypes ePlayer);
	bool ShouldIgnoreUnit(const CvUnit* pUnit, bool bIgnoreVisibility = false);
	bool ShouldIgnoreCity(const CvCity* pCity, bool bIgnoreVisibility = false);
	bool ShouldIgnoreCitadel(CvPlot* pCitadelPlot, bool bIgnoreVisibility = false);

	void AssignUnitDangerValue(const CvUnit* pUnit, CvPlot* pPlot);
	void AssignCityDangerValue(const CvCity* pCity, CvPlot* pPlot);

	PlayerTypes m_ePlayer;
	bool m_bDirty;
	int m_iTurnSliceBuilt;
	vector<CvDangerPlotContents> m_DangerPlots; //not serialized!
	UnitSet m_knownUnits;
};

FDataStream& operator>>(FDataStream&, CvDangerPlots&);
FDataStream& operator<<(FDataStream&, const CvDangerPlots&);

#endif //CIV5_PROJECT_CLASSES_H