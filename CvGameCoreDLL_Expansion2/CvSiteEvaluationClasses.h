/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_SITE_EVALUATION_CLASSES_H
#define CIV5_SITE_EVALUATION_CLASSES_H

enum SiteEvaluationFactors
{
    SITE_EVALUATION_HAPPINESS = NUM_YIELD_TYPES,
    SITE_EVALUATION_RESOURCES,
    SITE_EVALUATION_STRATEGIC,
    NUM_SITE_EVALUATION_FACTORS
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvCitySiteEvaluator
//!  \brief		Base class that contains the default routines to pick city sites
//
//!  Key Attributes:
//!  - Derived class CvSiteEvaluatorForSettler overrides defaults for founding new cities
//!  - Derived class CvSiteEvaluatorForStart overrides defaults for start location placement
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCitySiteEvaluator
{
public:
	CvCitySiteEvaluator(void);
	~CvCitySiteEvaluator(void);

	virtual void Init();
	virtual void ComputeFlavorMultipliers(const CvPlayer* pPlayer);
	virtual bool CanFound(const CvPlot* pPlot, const CvPlayer* pPlayer, bool bIgnoreDistanceToExistingCities) const;
	virtual int PlotFoundValue(CvPlot* pPlot, const CvPlayer* pPlayer, const std::vector<int>& ignorePlots, bool bCoastOnly=false, CvString* pDebug=NULL);
	virtual int PlotFertilityValue(CvPlot* pPlot, bool bIncludeCoast=false);

protected:
	// Each of these routines computes a number from 0 (no value) to 100 (best possible value)
	virtual int ComputeFoodValue(CvPlot* pPlot, const CvPlayer* pPlayer);
	virtual int ComputeHappinessValue(CvPlot* pPlot, const CvPlayer* pPlayer);
	virtual int ComputeProductionValue(CvPlot* pPlot, const CvPlayer* pPlayer);
	virtual int ComputeGoldValue(CvPlot* pPlot, const CvPlayer* pPlayer);
	virtual int ComputeScienceValue(CvPlot* pPlot, const CvPlayer* pPlayer);
	virtual int ComputeFaithValue(CvPlot* pPlot, const CvPlayer* pPlayer);
	virtual int ComputeTradeableResourceValue(CvPlot* pPlot, const CvPlayer* pPlayer);
	virtual int ComputeStrategicValue(CvPlot* pPlot, int iPlotsFromCity);

	int m_iFlavorMultiplier[NUM_SITE_EVALUATION_FACTORS];  // Extra for tradeable resources and strategic value

	int m_iRingModifier[MAX_CITY_RADIUS+MAX_CITY_RADIUS+2];
	int m_iExpansionIndex;
	int m_iGrowthIndex;
	int m_iNavalIndex;

	int m_iBrazilMultiplier;
	int m_iSpainMultiplier;
	int m_iMorrocoMultiplier;
#if defined(MOD_BALANCE_CORE)
	int m_iFranceMultiplier;
#endif
	int m_iNetherlandsMultiplier;
	int m_iIncaMultiplier;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvSiteEvaluatorForSettler
//!  \brief		Calculates a plot's value as a place to build a new city
//
//! Key Attributes:
//! - Derived from CvCitySiteEvaluator
//! - Only overrides routines from the base class where city settling is a special case
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSiteEvaluatorForSettler: public CvCitySiteEvaluator
{
public:
	CvSiteEvaluatorForSettler(void);
	~CvSiteEvaluatorForSettler(void);

	virtual int PlotFoundValue(CvPlot* pPlot, const CvPlayer* pPlayer, const std::vector<int>& ignorePlots, bool bCoastOnly=false, CvString* pDebug=NULL);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvSiteEvaluatorForStart
//!  \brief		Calculates a plot's value as a place to start a civilization
//
//! Key Attributes:
//! - Derived from CvCitySiteEvaluator
//! - Only overrides routines from the base class where civ start location is a special case
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSiteEvaluatorForStart: public CvCitySiteEvaluator
{
public:
	CvSiteEvaluatorForStart(void);
	~CvSiteEvaluatorForStart(void);

	virtual void ComputeFlavorMultipliers(const CvPlayer* pPlayer);
	virtual int PlotFoundValue(CvPlot* pPlot, CvPlayer* pPlayer, const std::vector<int>& ignorePlots, bool bCoastOnly=true);
};

#endif //CIV5_SITE_EVALUATION_CLASSES_H