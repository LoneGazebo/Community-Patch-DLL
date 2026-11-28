/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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

// The unit and city lists of a danger plot entry. Every player keeps one entry per plot of the map, so the size of the
// list object matters more than its speed: this one is a pointer and two 16-bit counts (8 bytes; std::vector is 16 with
// VS2008). It keeps the elements in insertion order, as std::vector does, and offers only what the danger code uses.
template<typename T>
class CvDangerList
{
public:
	typedef T value_type;
	typedef T* iterator;
	typedef const T* const_iterator;

	CvDangerList() : m_pData(NULL), m_uSize(0), m_uCapacity(0) {}
	CvDangerList(const CvDangerList& other) : m_pData(NULL), m_uSize(0), m_uCapacity(0) { assign(other); }
	~CvDangerList() { delete[] m_pData; }
	CvDangerList& operator=(const CvDangerList& other)
	{
		if (this != &other)
			assign(other);
		return *this;
	}

	iterator begin() { return m_pData; }
	iterator end() { return m_pData + m_uSize; }
	const_iterator begin() const { return m_pData; }
	const_iterator end() const { return m_pData + m_uSize; }
	size_t size() const { return m_uSize; }
	bool empty() const { return m_uSize == 0; }
	size_t capacity() const { return m_uCapacity; }

	void clear() { m_uSize = 0; }
	// frees the storage too
	void release()
	{
		delete[] m_pData;
		m_pData = NULL;
		m_uSize = m_uCapacity = 0;
	}
	void reserve(size_t uCapacity)
	{
		if (uCapacity > m_uCapacity)
			grow(uCapacity);
	}
	void push_back(const T& value)
	{
		if (m_uSize == m_uCapacity)
		{
			// a list holds each unit or city at most once per update, so this needs 65535 live units (or cities) of other
			// players that can reach one plot
			if (m_uCapacity == USHRT_MAX)
			{
				ASSERT(false, "danger list full: entry dropped");
				return;
			}
			grow(m_uCapacity < 4 ? 4 : std::min<size_t>(2 * m_uCapacity, USHRT_MAX));
		}
		m_pData[m_uSize++] = value;
	}

private:
	void grow(size_t uCapacity)
	{
		T* pData = new T[uCapacity];
		for (unsigned short i = 0; i < m_uSize; ++i)
			pData[i] = m_pData[i];
		delete[] m_pData;
		m_pData = pData;
		m_uCapacity = static_cast<unsigned short>(uCapacity);
	}
	void assign(const CvDangerList& other)
	{
		if (other.m_uSize > m_uCapacity)
		{
			release();
			grow(other.m_uSize);
		}
		for (unsigned short i = 0; i < other.m_uSize; ++i)
			m_pData[i] = other.m_pData[i];
		m_uSize = other.m_uSize;
	}

	T* m_pData;
	unsigned short m_uSize;
	unsigned short m_uCapacity;
};

// Stores all possible damage sources on a tile (terrain, improvements, cities, units)
typedef CvDangerList<std::pair<PlayerTypes,int>> DangerUnitVector;
typedef CvDangerList<std::pair<PlayerTypes,int>> DangerCityVector;
typedef std::set<std::pair<PlayerTypes,int>> UnitSet;

//todo: return more information on top of the total danger
//might help to make better decisions in tactical AI
//alternatively: try to check whether attacks are mutually exclusive b/c of units blocking each other
//unfortunately MOD_COMBATAI_TWO_PASS_DANGER doesn't really work well if blocking units move ...
//wild idea: run a reverse tactsim for the enemy first and use that to find our most endangered units
struct UnitDangerEstimate
{
	int iTotalDamage;
	int iMeleeAttacks;
	int iRangedAttacks;
	int iWorstSingleAttack;
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
		m_apUnits.clear(); if (m_apUnits.capacity() > 5) { m_apUnits.release(); m_apUnits.reserve(5); }
		m_apCities.clear(); //cities are fairly static so don't care to prune
		m_apCaptureUnits.clear(); if (m_apCaptureUnits.capacity() > 5) { m_apCaptureUnits.release(); }
	};

	int GetDanger(const CvUnit* pUnit, const SUnitIDValueContainer& extraUnitDamage, int iExtraDamage, AirActionType iAirAction);
	int GetDanger(const CvCity* pCity, const CvUnit* pPretendGarrison, const SUnitIDValueContainer& extraUnitDamage, int iExtraSelfDamage = 0);
	std::vector<CvUnit*> GetPossibleAttackers(TeamTypes eTeamForVisibilityCheck) const;

	// should not normally be used if a city or unit is known, primarily for compatibility
	int GetDanger(bool bFixedDamageOnly);

	// just for internal use
	int GetAirUnitDamage(const CvUnit* pUnit, AirActionType iAirAction = AIR_ACTION_ATTACK);

	// ordered so that the entry has no padding inside it (40 bytes): one entry per plot for every player
	CvPlot* m_pPlot;
	int m_iImprovementDamage; //only one citadel can affect a unit at a time
	int m_iFogCount;
	DangerUnitVector m_apUnits;
	DangerCityVector m_apCities;
	DangerUnitVector m_apCaptureUnits; //for civilians
	bool m_bFlatPlotDamage;
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

	void Init(PlayerTypes ePlayer);
	void Uninit();

	void UpdateDanger();
	int GetDanger(const CvPlot& pPlot, const CvUnit* pUnit, const SUnitIDValueContainer& unitDamageDealt, int iExtraDamage = 0, AirActionType iAirAction = AIR_ACTION_ATTACK);
	int GetDanger(const CvCity* pCity, const CvUnit* pPretendGarrison = NULL, const SUnitIDValueContainer& unitDamageDealt = SUnitIDValueContainer());
	int GetDanger(const CvPlot& pPlot, bool bFixedDamageOnly);

	std::vector<CvUnit*> GetPossibleAttackers(const CvPlot& Plot, TeamTypes eTeamForVisibilityCheck) const;
	bool IsKnownAttacker(const CvUnit* pUnit) const;
	bool AddKnownAttacker(const CvUnit* pUnit);
	const UnitSet& GetVanishedUnits() const { return m_vanishedUnits; }

	void SetDirty();
	bool IsDirty() const { return m_bDirty; }

	template<typename DangerPlots, typename Visitor>
	static void Serialize(DangerPlots& dangerPlots, Visitor& visitor);
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

protected:

	void AddFogDanger(CvPlot* pOrigin, TeamTypes eEnemyTeam, int iRange, bool bCheckOwnership);
	void UpdateDangerInternal(const PlotIndexContainer& plotsToIgnoreForZOC, bool bTurnChange, bool bWarChange);
	bool UpdateDangerSingleUnit(const CvUnit* pUnit, bool bIgnoreVisibility, const PlotIndexContainer& plotsToIgnoreForZOC);

	bool ShouldIgnorePlayer(PlayerTypes ePlayer);
	bool ShouldIgnoreUnit(const CvUnit* pUnit, bool bIgnoreVisibility = false);
	bool ShouldIgnoreCity(const CvCity* pCity, bool bIgnoreVisibility = false);
	bool ShouldIgnoreCitadel(CvPlot* pCitadelPlot, bool bIgnoreVisibility = false);

	void AssignUnitDangerValue(const CvUnit* pUnit, CvPlot* pPlot);
	void AssignCityDangerValue(const CvCity* pCity, CvPlot* pPlot);

	PlayerTypes m_ePlayer;
	bool m_bDirty;
	int m_iTurnBuilt;
	vector<CvDangerPlotContents> m_DangerPlots; //not serialized!
	UnitSet m_knownUnits;
	UnitSet m_vanishedUnits;
};

FDataStream& operator>>(FDataStream&, CvDangerPlots&);
FDataStream& operator<<(FDataStream&, const CvDangerPlots&);

#endif //CIV5_PROJECT_CLASSES_H