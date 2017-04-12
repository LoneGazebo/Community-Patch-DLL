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

#include "CvDiplomacyAIEnums.h"

#include "CvDLLUtilDefines.h"
// Stores all possible damage sources on a tile (terrain, improvements, cities, units)

typedef std::vector<std::pair<PlayerTypes,int>> DangerUnitVector;
typedef std::vector<std::pair<PlayerTypes,int>> DangerCityVector;
typedef std::set<std::pair<PlayerTypes,int>> UnitSet;

struct SUnitInfo
{
	SUnitInfo(const CvUnit* pUnit=NULL)
	{
		m_pUnit = pUnit;
		m_x = pUnit ? pUnit->plot()->getX() : 0;
		m_y = pUnit ? pUnit->plot()->getY() : 0;
		m_damage = pUnit ? pUnit->getDamage() : 0;
	}
	const bool operator<(const SUnitInfo& rhs) const
	{
		return m_pUnit<rhs.m_pUnit;
	}
	const bool operator==(const SUnitInfo& rhs) const
	{
		return (m_pUnit==rhs.m_pUnit && m_x==rhs.m_x && m_y==rhs.m_y && m_damage==rhs.m_damage);
	}

	const CvUnit* m_pUnit;
	int m_x;
	int m_y;
	int m_damage;
};

#define DANGER_MAX_CACHE_SIZE 5
struct CvDangerPlotContents
{
	CvDangerPlotContents()
	{
		m_pPlot = NULL;
		m_iX = INVALID_PLOT_COORD;
		m_iY = INVALID_PLOT_COORD;
		clear();
	}

	void init(CvPlot& plot)
	{
		m_pPlot = &plot;
		m_iX = m_pPlot->getX();
		m_iY = m_pPlot->getY();
	}

	void clear()
	{
		m_bFlatPlotDamage = false;
		m_pCitadel = NULL;
		m_apUnits.clear();
		m_apCities.clear();
		m_fogDanger.clear();

		//reset cache
		m_lastResults.clear();
	};

	int GetDanger(const CvUnit* pUnit, const set<int>& unitsToIgnore, AirActionType iAirAction);
	int GetDanger(CvCity* pCity, const CvUnit* pPretendGarrison = NULL);
	std::vector<CvUnit*> GetPossibleAttackers() const;

	// should not normally be used, primarily for compatibility
	int GetDanger(PlayerTypes ePlayer);
	int GetDamageFromFeatures(PlayerTypes ePlayer) const;

	// just for internal use
	int GetAirUnitDamage(const CvUnit* pUnit, AirActionType iAirAction = AIR_ACTION_ATTACK);

	CvPlot* m_pPlot;
	int m_iX;
	int m_iY;
	bool m_bFlatPlotDamage;
	//only one citadel can affect a unit at a time
	CvPlot* m_pCitadel;
	DangerUnitVector m_apUnits;
	DangerCityVector m_apCities;
	std::vector<int> m_fogDanger;

	//caching ...
	std::vector< std::pair<SUnitInfo,int> > m_lastResults;
};

inline FDataStream & operator >> (FDataStream & kStream, CvDangerPlotContents & kStruct)
{
	int iX;
	int iY;

	kStream >> iX;
	kStream >> iY;

	CvPlot* pPlot = GC.getMap().plot(iX,iY);

	if (pPlot)
		kStruct.init(*pPlot);

	return kStream;
}

inline FDataStream & operator << (FDataStream & kStream, const CvDangerPlotContents & kStruct)
{
	kStream << kStruct.m_iX;
	kStream << kStruct.m_iY;
	return kStream;
}

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
	void Reset();

	void UpdateDanger(bool bPretendWarWithAllCivs = false, bool bIgnoreVisibility = false);
	int GetDanger(const CvPlot& pPlot, const CvUnit* pUnit, const set<int>& unitsToIgnore, AirActionType iAirAction = AIR_ACTION_ATTACK);
	int GetDanger(const CvPlot& pPlot, CvCity* pCity, const CvUnit* pPretendGarrison = NULL);
	int GetDanger(const CvPlot& pPlot, PlayerTypes ePlayer);

	std::vector<CvUnit*> GetPossibleAttackers(const CvPlot& Plot) const;
	bool UpdateDangerSingleUnit(CvUnit* pUnit, bool bIgnoreVisibility, bool bRemember);
	bool IsKnownAttacker(PlayerTypes eOwner, int iUnitID) const;
	void AddKnownAttacker(PlayerTypes eOwner, int iUnitID);

	void SetDirty();
	bool IsDirty() const
	{
		return m_bDirty;
	}

	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

protected:

	bool IsDangerByRelationshipZero(PlayerTypes ePlayer, CvPlot* pPlot);

	bool ShouldIgnorePlayer(PlayerTypes ePlayer);
	bool ShouldIgnoreUnit(CvUnit* pUnit, bool bIgnoreVisibility = false);
	bool ShouldIgnoreCity(CvCity* pCity, bool bIgnoreVisibility = false);
	bool ShouldIgnoreCitadel(CvPlot* pCitadelPlot, bool bIgnoreVisibility = false);

	void AssignUnitDangerValue(CvUnit* pUnit, CvPlot* pPlot);
	void AssignCityDangerValue(CvCity* pCity, CvPlot* pPlot);

	PlayerTypes m_ePlayer;
	bool m_bArrayAllocated;
	bool m_bDirty;
	double m_fMajorWarMod;
	double m_fMajorHostileMod;
	double m_fMajorDeceptiveMod;
	double m_fMajorGuardedMod;
	double m_fMajorAfraidMod;
	double m_fMajorFriendlyMod;
	double m_fMajorNeutralMod;
	double m_fMinorNeutralMinorMod;
	double m_fMinorFriendlyMod;
	double m_fMinorBullyMod;
	double m_fMinorConquestMod;

	CvDangerPlotContents* m_DangerPlots;
	UnitSet m_knownUnits;
};

#endif //CIV5_PROJECT_CLASSES_H