/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_BELIEF_CLASSES_H
#define CIV5_BELIEF_CLASSES_H

class CvReligion;

#define SAFE_ESTIMATE_NUM_BELIEFS 100

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvBeliefEntry
//!  \brief		A single entry in the Belief XML file
//
//!  Key Attributes:
//!  - Used to be called CvBeliefInfo
//!  - Populated from XML\Civilizations\CIV5Beliefs.xml
//!  - Array of these contained in CvBeliefXMLEntries class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBeliefEntry: public CvBaseInfo
{
public:
	CvBeliefEntry(void);
	~CvBeliefEntry(void);

	int GetMinPopulation() const;
	int GetMinFollowers() const;
	int GetMaxDistance() const;
	int GetCityGrowthModifier() const;
	int GetFaithFromKills() const;
	int GetFaithFromDyingUnits() const;
	int GetRiverHappiness() const;
	int GetHappinessPerCity() const;
	int GetHappinessPerXPeacefulForeignFollowers() const;
	int GetPlotCultureCostModifier() const;
	int GetCityRangeStrikeModifier() const;
	int GetCombatModifierEnemyCities() const;
	int GetCombatModifierFriendlyCities() const;
	int GetFriendlyHealChange() const;
	int GetCityStateFriendshipModifier() const;
	int GetLandBarbarianConversionPercent() const;
	int GetWonderProductionModifier() const;
	int GetPlayerHappiness() const;
	int GetPlayerCultureModifier() const;
	float GetHappinessPerFollowingCity() const;
	int GetGoldPerFollowingCity() const;
	int GetGoldPerXFollowers() const;
	int GetGoldWhenCityAdopts() const;
	int GetSciencePerOtherReligionFollower() const;
	int GetSpreadDistanceModifier() const;
	int GetSpreadStrengthModifier() const;
	int GetProphetStrengthModifier() const;
	int GetProphetCostModifier() const;
	int GetMissionaryStrengthModifier() const;
	int GetMissionaryCostModifier() const;
	int GetFriendlyCityStateSpreadModifier() const;
	int GetGreatPersonExpendedFaith() const;
	int GetCityStateMinimumInfluence() const;
	int GetCityStateInfluenceModifier() const;
	int GetOtherReligionPressureErosion() const;
	int GetSpyPressure() const;
	int GetInquisitorPressureRetention() const;
	int GetFaithBuildingTourism() const;

	bool IsPantheonBelief() const;
	bool IsFounderBelief() const;
	bool IsFollowerBelief() const;
	bool IsEnhancerBelief() const;
	bool IsReformationBelief() const;
	bool RequiresPeace() const;
	bool ConvertsBarbarians() const;
	bool FaithPurchaseAllGreatPeople() const;

	EraTypes GetObsoleteEra() const;
	ResourceTypes GetResourceRevealed() const;
	TechTypes GetSpreadModifierDoublingTech() const;

	const char* getShortDescription() const;
	void setShortDescription(const char* szVal);

	// Arrays
	int GetCityYieldChange(int i) const;
	int GetHolyCityYieldChange(int i) const;
	int GetYieldChangePerForeignCity(int i) const;
	int GetYieldChangePerXForeignFollowers(int i) const;
	int GetResourceQuantityModifier(int i) const;
	int GetImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const;
	int GetBuildingClassYieldChange(int i, int j) const;
	int GetBuildingClassHappiness(int i) const;
	int GetBuildingClassTourism(int i) const;
	int GetFeatureYieldChange(int i, int j) const;
	int GetResourceYieldChange(int i, int j) const;
	int GetTerrainYieldChange(int i, int j) const;
	int GetResourceHappiness(int i) const;
	int GetYieldChangeAnySpecialist(int i) const;
	int GetYieldChangeTradeRoute(int i) const;
	int GetYieldChangeNaturalWonder(int i) const;
	int GetYieldChangeWorldWonder(int i) const;
	int GetYieldModifierNaturalWonder(int i) const;
	int GetMaxYieldModifierPerFollower(int i) const;
	bool IsFaithUnitPurchaseEra(int i) const;
	bool IsBuildingClassEnabled(int i) const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iMinPopulation;
	int m_iMinFollowers;
	int m_iMaxDistance;
	int m_iCityGrowthModifier;
	int m_iFaithFromKills;
	int m_iFaithFromDyingUnits;
	int m_iRiverHappiness;
	int m_iHappinessPerCity;
	int m_iHappinessPerXPeacefulForeignFollowers;
	int m_iPlotCultureCostModifier;
	int m_iCityRangeStrikeModifier;
	int m_iCombatModifierEnemyCities;
	int m_iCombatModifierFriendlyCities;
	int m_iFriendlyHealChange;
	int m_iCityStateFriendshipModifier;
	int m_iLandBarbarianConversionPercent;
	int m_iWonderProductionModifier;
	int m_iPlayerHappiness;
	int m_iPlayerCultureModifier;

	float m_fHappinessPerFollowingCity;

	int m_iGoldPerFollowingCity;
	int m_iGoldPerXFollowers;
	int m_iGoldWhenCityAdopts;
	int m_iSciencePerOtherReligionFollower;
	int m_iSpreadDistanceModifier;
	int m_iSpreadStrengthModifier;
	int m_iProphetStrengthModifier;
	int m_iProphetCostModifier;
	int m_iMissionaryStrengthModifier;
	int m_iMissionaryCostModifier;
	int m_iFriendlyCityStateSpreadModifier;
	int m_iGreatPersonExpendedFaith;
	int m_iCityStateMinimumInfluence;
	int m_iCityStateInfluenceModifier;
	int m_iOtherReligionPressureErosion;
	int m_iSpyPressure;
	int m_iInquisitorPressureRetention;
	int m_iFaithBuildingTourism;

	bool m_bPantheon;
	bool m_bFounder;
	bool m_bFollower;
	bool m_bEnhancer;
	bool m_bReformer;
	bool m_bRequiresPeace;
	bool m_bConvertsBarbarians;
	bool m_bFaithPurchaseAllGreatPeople;

	EraTypes m_eObsoleteEra;
	ResourceTypes m_eResourceRevealed;
	TechTypes m_eSpreadModifierDoublingTech;

	CvString m_strShortDescription;

	// Arrays
	int* m_paiCityYieldChange;
	int* m_paiHolyCityYieldChange;
	int* m_paiYieldChangePerForeignCity;
	int* m_paiYieldChangePerXForeignFollowers;
	int* m_piResourceQuantityModifiers;
	int** m_ppiImprovementYieldChanges;
	int** m_ppiBuildingClassYieldChanges;
	int* m_paiBuildingClassHappiness;
	int* m_paiBuildingClassTourism;
	int** m_ppaiFeatureYieldChange;
	int** m_ppaiResourceYieldChange;
	int** m_ppaiTerrainYieldChange;
	int* m_piResourceHappiness;
	int* m_piYieldChangeAnySpecialist;
	int* m_piYieldChangeTradeRoute;
	int* m_piYieldChangeNaturalWonder;
	int* m_piYieldChangeWorldWonder;
	int* m_piYieldModifierNaturalWonder;
	int* m_piMaxYieldModifierPerFollower;
	bool* m_pbFaithPurchaseUnitEraEnabled;
    bool* m_pbBuildingClassEnabled;

private:
	CvBeliefEntry(const CvBeliefEntry&);
	CvBeliefEntry& operator=(const CvBeliefEntry&);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvBeliefXMLEntries
//!  \brief		Game-wide information about religious beliefs
//
//! Key Attributes:
//! - Plan is it will be contained in CvGameRules object within CvGame class
//! - Populated from XML\GameInfo\CIV5Beliefs.xml
//! - Contains an array of CvBeliefEntry from the above XML file
//! - One instance for the entire game
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBeliefXMLEntries
{
public:
	CvBeliefXMLEntries(void);
	~CvBeliefXMLEntries(void);

	// Accessor functions
	std::vector<CvBeliefEntry*>& GetBeliefEntries();
	int GetNumBeliefs();
	CvBeliefEntry* GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvBeliefEntry*> m_paBeliefEntries;
};

typedef FStaticVector<int, 5, false, c_eCiv5GameplayDLL >BeliefList;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvReligionBeliefs
//!  \brief		Information about the beliefs of a religion
//
//!  Key Attributes:
//!  - One instance for each religion (or pantheon)
//!  - Accessed by any class that needs to check belief info
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvReligionBeliefs
{
public:
	CvReligionBeliefs(void);
	~CvReligionBeliefs(void);
	CvReligionBeliefs(const CvReligionBeliefs& source);
	void Uninit();
	void Reset();
	void AddBelief(BeliefTypes eBelief);

	// Accessor functions
	bool HasBelief(BeliefTypes eBelief) const;
	BeliefTypes GetBelief(int iIndex) const;
	int GetNumBeliefs() const;

	int GetFaithFromDyingUnits() const
	{
		return m_iFaithFromDyingUnits;
	};
	int GetRiverHappiness() const
	{
		return m_iRiverHappiness;
	};
	int GetPlotCultureCostModifier() const
	{
		return m_iPlotCultureCostModifier;
	};
	int GetCityRangeStrikeModifier() const
	{
		return m_iCityRangeStrikeModifier;
	};
	int GetCombatModifierEnemyCities() const
	{
		return m_iCombatModifierEnemyCities;
	};
	int GetCombatModifierFriendlyCities() const
	{
		return m_iCombatModifierFriendlyCities;
	};
	int GetFriendlyHealChange() const
	{
		return m_iFriendlyHealChange;
	};
	int GetCityStateFriendshipModifier() const
	{
		return m_iCityStateFriendshipModifier;
	};
	int GetLandBarbarianConversionPercent() const
	{
		return m_iLandBarbarianConversionPercent;
	};
	int GetSpreadDistanceModifier() const
	{
		return m_iSpreadDistanceModifier;
	};
	int GetSpreadStrengthModifier() const
	{
		return m_iSpreadStrengthModifier;
	};
	int GetProphetStrengthModifier() const
	{
		return m_iProphetStrengthModifier;
	};
	int GetProphetCostModifier() const
	{
		return m_iProphetCostModifier;
	};
	int GetMissionaryStrengthModifier() const
	{
		return m_iMissionaryStrengthModifier;
	};
	int GetMissionaryCostModifier() const
	{
		return m_iMissionaryCostModifier;
	};
	int GetFriendlyCityStateSpreadModifier() const
	{
		return m_iFriendlyCityStateSpreadModifier;
	};
	int GetGreatPersonExpendedFaith() const
	{
		return m_iGreatPersonExpendedFaith;
	};
	int GetCityStateMinimumInfluence() const
	{
		return m_iCityStateMinimumInfluence;
	}
	int GetCityStateInfluenceModifier() const
	{
		return m_iCityStateInfluenceModifier;
	}
	int GetOtherReligionPressureErosion() const
	{
		return m_iOtherReligionPressureErosion;
	}
	int GetSpyPressure() const
	{
		return m_iSpyPressure;
	}
	int GetInquisitorPressureRetention() const
	{
		return m_iInquisitorPressureRetention;
	}
	int GetFaithBuildingTourism() const
	{
		return m_iFaithBuildingTourism;
	}

	EraTypes GetObsoleteEra() const
	{
		return m_eObsoleteEra;
	};
	ResourceTypes GetResourceRevealed() const
	{
		return m_eResourceRevealed;
	};
	TechTypes GetSpreadModifierDoublingTech() const
	{
		return m_eSpreadModifierDoublingTech;
	};

	int GetFaithFromKills(int iDistance) const;
	int GetHappinessPerCity(int iPopulation) const;
	int GetHappinessPerXPeacefulForeignFollowers() const;
	int GetWonderProductionModifier(EraTypes eWonderEra) const;
	int GetPlayerHappiness(bool bAtPeace) const;
	int GetPlayerCultureModifier(bool bAtPeace) const;
	float GetHappinessPerFollowingCity() const;
	int GetGoldPerFollowingCity() const;
	int GetGoldPerXFollowers() const;
	int GetGoldWhenCityAdopts() const;
	int GetSciencePerOtherReligionFollower() const;
	int GetCityGrowthModifier(bool bAtPeace) const;

	int GetCityYieldChange(int iPopulation, YieldTypes eYield) const;
	int GetHolyCityYieldChange(YieldTypes eYield) const;
	int GetYieldChangePerForeignCity(YieldTypes eYield) const;
	int GetYieldChangePerXForeignFollowers(YieldTypes eYield) const;
	int GetResourceQuantityModifier(ResourceTypes eResource) const;
	int GetImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYield) const;
	int GetBuildingClassYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYieldType, int iFollowers) const;
	int GetBuildingClassHappiness(BuildingClassTypes eBuildingClass, int iFollowers) const;
	int GetBuildingClassTourism(BuildingClassTypes eBuildingClass) const;
	int GetFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYieldType) const;
	int GetResourceYieldChange(ResourceTypes eResource, YieldTypes eYieldType) const;
	int GetTerrainYieldChange(TerrainTypes eTerrain, YieldTypes eYieldType) const;
	int GetResourceHappiness(ResourceTypes eResource) const;
	int GetYieldChangeAnySpecialist(YieldTypes eYieldType) const;
	int GetYieldChangeTradeRoute(YieldTypes eYieldType) const;
	int GetYieldChangeNaturalWonder(YieldTypes eYieldType) const;
	int GetYieldChangeWorldWonder(YieldTypes eYieldType) const;
	int GetYieldModifierNaturalWonder(YieldTypes eYieldType) const;
	int GetMaxYieldModifierPerFollower(YieldTypes eYieldType) const;

	bool IsBuildingClassEnabled(BuildingClassTypes eType) const;
	bool IsFaithBuyingEnabled(EraTypes eEra) const;
	bool IsConvertsBarbarians() const;
	bool IsFaithPurchaseAllGreatPeople() const;

	// Serialization
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

private:
	// Cached data about this religion's beliefs
	int m_iFaithFromDyingUnits;
	int m_iRiverHappiness;
	int m_iPlotCultureCostModifier;
	int m_iCityRangeStrikeModifier;
	int m_iCombatModifierEnemyCities;
	int m_iCombatModifierFriendlyCities;
	int m_iFriendlyHealChange;
	int m_iCityStateFriendshipModifier;
	int m_iLandBarbarianConversionPercent;
	int m_iSpreadDistanceModifier;
	int m_iSpreadStrengthModifier;
	int m_iProphetStrengthModifier;
	int m_iProphetCostModifier;
	int m_iMissionaryStrengthModifier;
	int m_iMissionaryCostModifier;
	int m_iFriendlyCityStateSpreadModifier;
	int m_iGreatPersonExpendedFaith;
	int m_iCityStateMinimumInfluence;
	int m_iCityStateInfluenceModifier;
	int m_iOtherReligionPressureErosion;
	int m_iSpyPressure;
	int m_iInquisitorPressureRetention;
	int m_iFaithBuildingTourism;

	EraTypes m_eObsoleteEra;
	ResourceTypes m_eResourceRevealed;
	TechTypes m_eSpreadModifierDoublingTech;

	BeliefList m_ReligionBeliefs;

	// Arrays
	int* m_paiBuildingClassEnabled;
};

namespace CvBeliefHelpers
{
	bool ConvertBarbarianUnit(CvPlayer *pPlayer, UnitHandle pUnit);
}

#endif //CIV5_BELIEF_CLASSES_H