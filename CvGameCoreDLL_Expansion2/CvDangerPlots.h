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

// Stores all possible damage sources on a tile (terrain, improvements, cities, units)
typedef std::vector<std::pair<PlayerTypes,int>> DangerUnitVector;
typedef std::vector<std::pair<PlayerTypes,int>> DangerCityVector;
typedef std::set<std::pair<PlayerTypes,int>> UnitSet;

struct SUnitInfo
{
	SUnitInfo(const CvUnit* pUnit=NULL, const UnitIdContainer& enemyUnitsToIgnore=UnitIdContainer()) : m_enemyUnitsToIgnore(enemyUnitsToIgnore)
	{
		m_iUnitID = pUnit ? pUnit->GetID() : 0;
		m_iPlotIndex = pUnit ? pUnit->plot()->GetPlotIndex() : -1;
		m_damage = pUnit ? pUnit->getDamage() : 0;
	}
	const bool operator==(const SUnitInfo& rhs) const
	{
		return (m_iUnitID==rhs.m_iUnitID && m_iPlotIndex==rhs.m_iPlotIndex && m_damage==rhs.m_damage && m_enemyUnitsToIgnore==rhs.m_enemyUnitsToIgnore);
	}

	int m_iUnitID;
	int m_iPlotIndex;
	int m_damage;
	UnitIdContainer m_enemyUnitsToIgnore;
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
		m_bEnemyAdjacent = false;
		m_bEnemyCanCapture = false;
		m_iImprovementDamage = 0;
		m_apUnits.clear();
		m_apCities.clear();
		m_fogDanger.clear();

		//reset cache
		m_lastResults.clear();
	};

	void resetCache()
	{
		m_lastResults.clear();
	}

	int GetDanger(const CvUnit* pUnit, const UnitIdContainer& unitsToIgnore, AirActionType iAirAction);
	int GetDanger(const CvCity* pCity, const CvUnit* pPretendGarrison = NULL);
	std::vector<CvUnit*> GetPossibleAttackers(TeamTypes eTeamForVisibilityCheck) const;

	// should not normally be used if a city or unit is known, primarily for compatibility
	int GetDanger(bool bFixedDamageOnly);

	// just for internal use
	int GetAirUnitDamage(const CvUnit* pUnit, AirActionType iAirAction = AIR_ACTION_ATTACK);

	CvPlot* m_pPlot;
	bool m_bFlatPlotDamage;
	bool m_bEnemyAdjacent; //use this as a tiebreaker, disengage when in doubt
	bool m_bEnemyCanCapture; //for civilians
	int m_iImprovementDamage; //only one citadel can affect a unit at a time
	DangerUnitVector m_apUnits;
	DangerCityVector m_apCities;
	std::vector<int> m_fogDanger;

	//caching ...
	std::vector< std::pair<SUnitInfo,int> > m_lastResults;
};

/*
inline FDataStream & operator >> (FDataStream & kStream, CvDangerPlotContents & kStruct)
{
	int iX;
	int iY;

	kStream >> iX;
	kStream >> iY;

	CvPlot* pPlot = GC.getMap().plot(iX,iY);

	if (pPlot)
	{
		kStruct.m_pPlot = pPlot;
		kStream << kStruct.m_bFlatPlotDamage;
		kStream << kStruct.m_bEnemyAdjacent;
		kStream << kStruct.m_bEnemyCanCapture;
		kStream << kStruct.m_iImprovementDamage;
		kStream << kStruct.m_apUnits;
		kStream << kStruct.m_apCities;
		kStream << kStruct.m_fogDanger;
	}
	else
		kStruct.clear();

	return kStream;
}

inline FDataStream & operator << (FDataStream & kStream, const CvDangerPlotContents & kStruct)
{
	if (kStruct.m_pPlot)
	{
		kStream << kStruct.m_pPlot->getX();
		kStream << kStruct.m_pPlot->getY();
		kStream << kStruct.m_bFlatPlotDamage;
		kStream << kStruct.m_bEnemyAdjacent;
		kStream << kStruct.m_bEnemyCanCapture;
		kStream << kStruct.m_iImprovementDamage;
		kStream << kStruct.m_apUnits;
		kStream << kStruct.m_apCities;
		kStream << kStruct.m_fogDanger;
	}
	else
		kStream << -1 << -1;

	return kStream;
}
*/

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
	int GetDanger(const CvPlot& pPlot, const CvUnit* pUnit, const UnitIdContainer& unitsToIgnore, AirActionType iAirAction = AIR_ACTION_ATTACK);
	int GetDanger(const CvCity* pCity, const CvUnit* pPretendGarrison = NULL);
	int GetDanger(const CvPlot& pPlot, bool bFixedDamageOnly);

	std::vector<CvUnit*> GetPossibleAttackers(const CvPlot& Plot, TeamTypes eTeamForVisibilityCheck) const;
	void ResetDangerCache(const CvPlot* pCenterPlot, int iRange);
	bool IsKnownAttacker(const CvUnit* pUnit) const;
	bool AddKnownAttacker(const CvUnit* pUnit);

	void SetDirty();
	bool IsDirty() const { return m_bDirty; }
	int GetTurnSliceBuilt() const { return m_iTurnSliceBuilt; }

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

#endif //CIV5_PROJECT_CLASSES_H