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

#define NUM_CITY_RINGS 3

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
	virtual void ComputeFlavorMultipliers(CvPlayer* pPlayer);
	virtual bool CanFound(CvPlot* pPlot, const CvPlayer* pPlayer, bool bTestVisible) const;
	virtual int PlotFoundValue(CvPlot* pPlot, CvPlayer* pPlayer, YieldTypes eYield = NO_YIELD, bool bCoastOnly=true);
	virtual int PlotFertilityValue(CvPlot* pPlot);
	virtual int BestFoundValueForSpecificYield(CvPlayer* pPlayer, YieldTypes eYield);

protected:
	// Each of these routines computes a number from 0 (no value) to 100 (best possible value)
	virtual int ComputeFoodValue(CvPlot* pPlot, CvPlayer* pPlayer);
	virtual int ComputeHappinessValue(CvPlot* pPlot, CvPlayer* pPlayer);
	virtual int ComputeProductionValue(CvPlot* pPlot, CvPlayer* pPlayer);
	virtual int ComputeGoldValue(CvPlot* pPlot, CvPlayer* pPlayer);
	virtual int ComputeScienceValue(CvPlot* pPlot, CvPlayer* pPlayer);
	virtual int ComputeFaithValue(CvPlot* pPlot, CvPlayer* pPlayer);
	virtual int ComputeTradeableResourceValue(CvPlot* pPlot, CvPlayer* pPlayer);
	virtual int ComputeStrategicValue(CvPlot* pPlot, CvPlayer* pPlayer, int iPlotsFromCity);

	int m_iFlavorMultiplier[NUM_SITE_EVALUATION_FACTORS];  // Extra for tradeable resources and strategic value
	int m_iRingModifier[NUM_CITY_RINGS+NUM_CITY_RINGS+2];

	int m_iExpansionIndex;
	int m_iGrowthIndex;
	int m_iNavalIndex;

	int m_iBrazilMultiplier;
	int m_iSpainMultiplier;
	int m_iMorrocoMultiplier;
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

	int PlotFoundValue(CvPlot* pPlot, CvPlayer* pPlayer, YieldTypes eYield = NO_YIELD, bool bCoastOnly=true);
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

	virtual void ComputeFlavorMultipliers(CvPlayer* pPlayer);
	int PlotFoundValue(CvPlot* pPlot, CvPlayer* pPlayer, YieldTypes eYield = NO_YIELD, bool bCoastOnly=true);
};

#endif //CIV5_SITE_EVALUATION_CLASSES_H