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

#ifdef AUI_DANGER_PLOTS_REMADE

#include "CvDLLUtilDefines.h"
// Stores all possible damage sources on a tile (terrain, improvements, cities, units)

typedef std::vector<std::pair<PlayerTypes,int>> DangerUnitVector;
typedef std::vector<std::pair<PlayerTypes,int>> DangerCityVector;
typedef std::set<std::pair<PlayerTypes,int>> UnitSet;

struct SUnitStats
{
	SUnitStats(const CvUnit* pUnit=NULL)
	{
		m_pUnit = pUnit;
		m_x = pUnit ? pUnit->plot()->getX() : 0;
		m_y = pUnit ? pUnit->plot()->getY() : 0;
		m_damage = pUnit ? pUnit->getDamage() : 0;
	}
	const bool operator<(const SUnitStats& rhs) const
	{
		return m_pUnit<rhs.m_pUnit;
	}
	const bool operator==(const SUnitStats& rhs) const
	{
		return (m_pUnit==rhs.m_pUnit && m_x==rhs.m_x && m_y==rhs.m_y && m_damage==rhs.m_damage);
	}

	const CvUnit* m_pUnit;
	int m_x;
	int m_y;
	int m_damage;
};

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
		m_iFlatPlotDamage = 0;
		m_pCitadel = NULL;
		m_apUnits.clear();
		m_apCities.clear();

		//reset cache
		m_lastUnit = SUnitStats(NULL);
		m_lastResult = 0;
	};

	int GetDanger(CvUnit* pUnit, int iAirAction = AIR_ACTION_ATTACK, int iAfterNIntercepts = 0);
	int GetDanger(CvCity* pCity, CvUnit* pPretendGarrison = NULL, int iAfterNIntercepts = 0);
	// should not normally be used, primarily for compatibility
	int GetDanger(PlayerTypes ePlayer);
	bool IsUnderImmediateThreat(CvUnit* pUnit);
	bool IsUnderImmediateThreat(PlayerTypes ePlayer);
	bool CouldAttackHere(CvUnit* pAttacker);
	bool CouldAttackHere(CvCity* pAttacker);
	int GetDamageFromFeatures(PlayerTypes ePlayer) const;

	CvPlot* m_pPlot;
	int m_iX;
	int m_iY;
	int m_iFlatPlotDamage;
	//only one citadel can affect a unit at a time
	CvPlot* m_pCitadel;
	DangerUnitVector m_apUnits;
	DangerCityVector m_apCities;

	//caching ...
	SUnitStats m_lastUnit;
	int m_lastResult;
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
#endif // AUI_DANGER_PLOTS_REMADE

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
#ifdef AUI_DANGER_PLOTS_REMADE
	int GetDanger(const CvPlot& pPlot, CvUnit* pUnit, int iAirAction = AIR_ACTION_ATTACK, int iAfterNIntercepts = 0);
	int GetDanger(const CvPlot& pPlot, CvCity* pCity, CvUnit* pPretendGarrison = NULL, int iAfterNIntercepts = 0);
	int GetDanger(const CvPlot& pPlot, PlayerTypes ePlayer);
	bool IsUnderImmediateThreat(const CvPlot& pPlot, CvUnit* pUnit);
	bool IsUnderImmediateThreat(const CvPlot& pPlot, PlayerTypes ePlayer);
	bool CouldAttackHere(const CvPlot& pPlot, CvUnit* pAttacker);
	bool CouldAttackHere(const CvPlot& pPlot, CvCity* pAttacker);
	bool UpdateDangerSingleUnit(CvUnit* pUnit, bool bIgnoreVisibility);
#else
	void AddDanger(int iPlotX, int iPlotY, int iValue, bool bWithinOneMove);
	int GetDanger(const CvPlot& pPlot) const;
	bool IsUnderImmediateThreat(const CvPlot& pPlot) const;

	int GetCityDanger(CvCity* pCity);  // sums the plots around the city to determine it's danger value
	int ModifyDangerByRelationship(PlayerTypes ePlayer, CvPlot* pPlot, int iDanger);
#endif // AUI_DANGER_PLOTS_REMADE

	bool ShouldIgnorePlayer(PlayerTypes ePlayer);
	bool ShouldIgnoreUnit(CvUnit* pUnit, bool bIgnoreVisibility = false);
	bool ShouldIgnoreCity(CvCity* pCity, bool bIgnoreVisibility = false);
	bool ShouldIgnoreCitadel(CvPlot* pCitadelPlot, bool bIgnoreVisibility = false);

	void AssignUnitDangerValue(CvUnit* pUnit, CvPlot* pPlot);
	void AssignCityDangerValue(CvCity* pCity, CvPlot* pPlot);

	void SetDirty();
	bool IsDirty() const
	{
		return m_bDirty;
	}

	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

protected:

	bool IsDangerByRelationshipZero(PlayerTypes ePlayer, CvPlot* pPlot);

#ifndef AUI_DANGER_PLOTS_REMADE
	int GetDangerValueOfCitadel() const;
#endif // AUI_DANGER_PLOTS_REMADE

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
	double m_fMinorNeutralrMod;
	double m_fMinorFriendlyMod;
	double m_fMinorBullyMod;
	double m_fMinorConquestMod;

#ifdef AUI_DANGER_PLOTS_REMADE
	CvDangerPlotContents* m_DangerPlots;
	UnitSet m_knownUnits;
#else
	FFastVector<uint, true, c_eCiv5GameplayDLL, 0> m_DangerPlots;
#endif // AUI_DANGER_PLOTS_REMADE
};

#endif //CIV5_PROJECT_CLASSES_H