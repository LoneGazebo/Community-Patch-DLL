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

	void UpdateDanger (bool bPretendWarWithAllCivs = false, bool bIgnoreVisibility = false);
	void AddDanger (int iPlotX, int iPlotY, int iValue);
	int GetDanger (const CvPlot & pPlot) const;
	int GetCityDanger (CvCity* pCity); // sums the plots around the city to determine it's danger value

	int ModifyDangerByRelationship (PlayerTypes ePlayer, CvPlot* pPlot, int iDanger);

	bool ShouldIgnorePlayer (PlayerTypes ePlayer);
	bool ShouldIgnoreUnit  (CvUnit* pUnit, bool bIgnoreVisibility = false);
	bool ShouldIgnoreCity  (CvCity* pCity, bool bIgnoreVisibility = false);
	bool ShouldIgnoreCitadel (CvPlot* pCitadelPlot, bool bIgnoreVisibility = false);
	void AssignUnitDangerValue (CvUnit* pUnit, CvPlot* pPlot);
	void AssignCityDangerValue (CvCity* pCity, CvPlot* pPlot);

	void SetDirty();
	bool IsDirty() const { return m_bDirty; }

	void Read  (FDataStream& kStream);
	void Write (FDataStream& kStream) const;

protected:

	bool IsDangerByRelationshipZero(PlayerTypes ePlayer, CvPlot* pPlot);

	int GetDangerValueOfCitadel() const;

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

	FFastVector<int, true, c_eCiv5GameplayDLL, 0> m_DangerPlots;
};


#endif //CIV5_PROJECT_CLASSES_H