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

#if defined(MOD_BALANCE_CORE_BELIEFS_RESOURCE)
	bool RequiresImprovement() const;
	bool RequiresResource() const;
	bool RequiresNoImprovement() const;
	bool RequiresNoImprovementFeature() const;
#endif

#if defined(MOD_BALANCE_CORE_BELIEFS)
	bool IsHalvedFollowers() const;
	int GetYieldPerPop(int i) const;
	int GetYieldPerGPT(int i) const;
	int GetYieldPerLux(int i) const;
	int GetYieldPerBorderGrowth(int i) const;
	int GetYieldPerHeal(int i) const;
	int GetYieldPerBirth(int i) const;
	int GetYieldPerScience(int i) const;
	int GetYieldFromGPUse(int i) const;
	int GetYieldBonusGoldenAge(int i) const;
	int GetYieldFromSpread(int i) const;
	int GetYieldFromForeignSpread(int i) const;
	int GetYieldFromConquest(int i) const;
	int GetYieldFromPolicyUnlock(int i) const;
	int GetYieldFromEraUnlock(int i) const;
	int GetYieldFromConversion(int i) const;
	int GetYieldFromWLTKD(int i) const;
	int GetYieldFromProposal(int i) const;
	int GetYieldFromHost(int i) const;
	int GetYieldFromKnownPantheons(int i) const;
	int GetCombatVersusOtherReligionOwnLands() const;
	int GetCombatVersusOtherReligionTheirLands() const;
	int GetMissionaryInfluenceCS()const;
	int GetHappinessPerPantheon() const;
	int GetExtraVotes() const;
	int GetMaxYieldPerFollower(int i) const;
#endif
#if defined(MOD_BALANCE_CORE)
	CivilizationTypes GetRequiredCivilization() const;
#endif

	EraTypes GetObsoleteEra() const;
	ResourceTypes GetResourceRevealed() const;
	TechTypes GetSpreadModifierDoublingTech() const;

	const char* getShortDescription() const;
	void setShortDescription(const char* szVal);

#if defined(MOD_BALANCE_CORE)
	const char* getTooltip() const;
	void setTooltip(const char* szVal);
#endif

	// Arrays
	int GetCityYieldChange(int i) const;
	int GetHolyCityYieldChange(int i) const;
	int GetYieldChangePerForeignCity(int i) const;
	int GetYieldChangePerXForeignFollowers(int i) const;
#if defined(MOD_API_UNIFIED_YIELDS)
	int GetYieldPerFollowingCity(int i) const;
	int GetYieldPerXFollowers(int i) const;
	int GetYieldPerOtherReligionFollower(int i) const;
#endif
	int GetResourceQuantityModifier(int i) const;
	int GetImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const;
	int GetBuildingClassYieldChange(int i, int j) const;
	int GetBuildingClassHappiness(int i) const;
	int GetBuildingClassTourism(int i) const;
	int GetFeatureYieldChange(int i, int j) const;
#if defined(MOD_API_UNIFIED_YIELDS)
	int GetYieldPerXTerrainTimes100(int i, int j) const;
	int GetYieldPerXFeatureTimes100(int i, int j) const;
	int GetCityYieldFromUnimprovedFeature(int i, int j) const;
	int GetUnimprovedFeatureYieldChange(int i, int j) const;
	int GetLakePlotYieldChange(int i) const;
#endif
	int GetResourceYieldChange(int i, int j) const;
	int GetTerrainYieldChange(int i, int j) const;
#if defined(MOD_API_UNIFIED_YIELDS)
	int GetTradeRouteYieldChange(int i, int j) const;
	int GetSpecialistYieldChange(int i, int j) const;
	int GetGreatPersonExpendedYield(int i, int j) const;
	int GetGoldenAgeGreatPersonRateModifier(int i) const;
	int GetCapitalYieldChange(int i) const;
	int GetCoastalCityYieldChange(int i) const;
	int GetGreatWorkYieldChange(int i) const;
	int GetYieldFromKills(YieldTypes eYield) const;
	int GetYieldFromBarbarianKills(YieldTypes eYield) const;
#endif
#if defined(MOD_RELIGION_PLOT_YIELDS)
	int GetPlotYieldChange(int i, int j) const;
#endif
	int GetResourceHappiness(int i) const;
	int GetYieldChangeAnySpecialist(int i) const;
	int GetYieldChangeTradeRoute(int i) const;
	int GetYieldChangeNaturalWonder(int i) const;
	int GetYieldChangeWorldWonder(int i) const;
	int GetYieldModifierNaturalWonder(int i) const;
	int GetMaxYieldModifierPerFollower(int i) const;
	bool IsFaithUnitPurchaseEra(int i) const;
#if defined(MOD_BALANCE_CORE)
	bool IsFaithUnitPurchaseSpecific(int i) const;
#endif
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
#if defined(MOD_BALANCE_CORE_BELIEFS_RESOURCE)
	bool m_bRequiresImprovement;
	bool m_bRequiresResource;
	bool m_bRequiresNoImprovement;
	bool m_bRequiresNoImprovementFeature;
#endif

	EraTypes m_eObsoleteEra;
	ResourceTypes m_eResourceRevealed;
	TechTypes m_eSpreadModifierDoublingTech;

	CvString m_strShortDescription;
#if defined(MOD_BALANCE_CORE)
	CvString m_strTooltip;
#endif


	// Arrays
	int* m_paiCityYieldChange;
	int* m_paiHolyCityYieldChange;
	int* m_paiYieldChangePerForeignCity;
	int* m_paiYieldChangePerXForeignFollowers;
#if defined(MOD_API_UNIFIED_YIELDS)
	int* m_piYieldPerFollowingCity;
	int* m_piYieldPerXFollowers;
	int* m_piYieldPerOtherReligionFollower;
#endif
	int* m_piResourceQuantityModifiers;
	int** m_ppiImprovementYieldChanges;
	int** m_ppiBuildingClassYieldChanges;
	int* m_paiBuildingClassHappiness;
	int* m_paiBuildingClassTourism;
	int** m_ppaiFeatureYieldChange;
#if defined(MOD_API_UNIFIED_YIELDS)
	int** m_ppiYieldPerXTerrain;
	int** m_ppiYieldPerXFeature;
	int** m_ppiCityYieldFromUnimprovedFeature;
	int** m_ppiUnimprovedFeatureYieldChanges;
	int* m_paiLakePlotYieldChange;
#endif
	int** m_ppaiResourceYieldChange;
	int** m_ppaiTerrainYieldChange;
#if defined(MOD_API_UNIFIED_YIELDS)
	int** m_ppiTradeRouteYieldChange;
	int** m_ppiSpecialistYieldChange;
	int** m_ppiGreatPersonExpendedYield;
	int* m_piGoldenAgeGreatPersonRateModifier;
	int* m_piCapitalYieldChange;
	int* m_piCoastalCityYieldChange;
	int* m_piGreatWorkYieldChange;
	int* m_piYieldFromKills;
	int* m_piYieldFromBarbarianKills;
#endif
#if defined(MOD_RELIGION_PLOT_YIELDS)
	int** m_ppiPlotYieldChange;
#endif
	int* m_piResourceHappiness;
	int* m_piYieldChangeAnySpecialist;
	int* m_piYieldChangeTradeRoute;
	int* m_piYieldChangeNaturalWonder;
	int* m_piYieldChangeWorldWonder;
	int* m_piYieldModifierNaturalWonder;
	int* m_piMaxYieldModifierPerFollower;
#if defined(MOD_BALANCE_CORE)
	bool* m_pbFaithPurchaseUnitSpecificEnabled;
#endif
	bool* m_pbFaithPurchaseUnitEraEnabled;
    bool* m_pbBuildingClassEnabled;

#if defined(MOD_BALANCE_CORE_BELIEFS)
	bool m_bIsHalvedFollowers;
	int* m_piYieldPerPop;
	int* m_piYieldPerGPT;
	int* m_piYieldPerLux;
	int* m_piYieldPerBorderGrowth;
	int* m_piYieldPerHeal;
	int* m_piYieldPerBirth;
	int* m_piYieldPerScience;
	int* m_piYieldFromGPUse;
	int* m_piYieldBonusGoldenAge;
	int* m_piYieldFromSpread;
	int* m_piYieldFromForeignSpread;
	int* m_piYieldFromConquest;
	int* m_piYieldFromPolicyUnlock;
	int* m_piYieldFromEraUnlock;
	int* m_piYieldFromConversion;
	int* m_piYieldFromWLTKD;
	int* m_piYieldFromProposal;
	int* m_piYieldFromHost;
	int* m_piYieldFromKnownPantheons;
	int* m_piMaxYieldPerFollower;
	int m_iCombatVersusOtherReligionOwnLands;
	int m_iCombatVersusOtherReligionTheirLands;
	int m_iMissionaryInfluenceCS;
	int m_iHappinessPerPantheon;
	int m_iExtraVotes;
#endif
#if defined(MOD_BALANCE_CORE)
	CivilizationTypes m_eRequiredCivilization;
#endif

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
#if defined(MOD_BALANCE_CORE_BELIEFS)
	int GetCombatVersusOtherReligionOwnLands() const
	{
		return m_iCombatVersusOtherReligionOwnLands;
	}
	int GetCombatVersusOtherReligionTheirLands() const
	{
		return m_iCombatVersusOtherReligionTheirLands;
	}
	int GetMissionaryInfluenceCS() const
	{
		return m_iMissionaryInfluenceCS;
	}
	int GetHappinessPerPantheon() const
	{
		return m_iHappinessPerPantheon;
	}
	int GetExtraVotes() const
	{
		return m_iExtraVotes;
	}
#endif
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
#if defined(MOD_API_UNIFIED_YIELDS)
	int GetYieldPerFollowingCity(YieldTypes eYield) const;
	int GetYieldPerXFollowers(YieldTypes eYield) const;
	int GetYieldPerOtherReligionFollower(YieldTypes eYield) const;
#endif
	int GetResourceQuantityModifier(ResourceTypes eResource) const;
	int GetImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYield) const;
	int GetBuildingClassYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYieldType, int iFollowers) const;
	int GetBuildingClassHappiness(BuildingClassTypes eBuildingClass, int iFollowers) const;
	int GetBuildingClassTourism(BuildingClassTypes eBuildingClass) const;
	int GetFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYieldType) const;
#if defined(MOD_API_UNIFIED_YIELDS)
	int GetYieldPerXTerrainTimes100(TerrainTypes eTerrain, YieldTypes eYieldType) const;
	int GetYieldPerXFeatureTimes100(FeatureTypes eFeature, YieldTypes eYieldType) const;
	int GetCityYieldFromUnimprovedFeature(FeatureTypes eFeature, YieldTypes eYieldType) const;
	int GetUnimprovedFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYieldType) const;
	int GetLakePlotYieldChange(YieldTypes eYieldType) const;
#endif
	int GetResourceYieldChange(ResourceTypes eResource, YieldTypes eYieldType) const;
	int GetTerrainYieldChange(TerrainTypes eTerrain, YieldTypes eYieldType) const;
#if defined(MOD_API_UNIFIED_YIELDS)
	int GetTradeRouteYieldChange(DomainTypes eDomain, YieldTypes eYieldType) const;
	int GetSpecialistYieldChange(SpecialistTypes eSpecialist, YieldTypes eYieldType) const;
	int GetGreatPersonExpendedYield(GreatPersonTypes eGreatPerson, YieldTypes eYieldType) const;
	int GetGoldenAgeGreatPersonRateModifier(GreatPersonTypes eGreatPerson) const;
	int GetCapitalYieldChange(int iPopulation, YieldTypes eYield) const;
	int GetCoastalCityYieldChange(int iPopulation, YieldTypes eYield) const;
	int GetGreatWorkYieldChange(int iPopulation, YieldTypes eYield) const;
	int GetYieldFromBarbarianKills(YieldTypes eYield) const;
	int GetYieldFromKills(YieldTypes eYield) const;
#endif
#if defined(MOD_RELIGION_PLOT_YIELDS)
	int GetPlotYieldChange(PlotTypes ePlot, YieldTypes eYieldType) const;
#endif
	int GetResourceHappiness(ResourceTypes eResource) const;
	int GetYieldChangeAnySpecialist(YieldTypes eYieldType) const;
	int GetYieldChangeTradeRoute(YieldTypes eYieldType) const;
	int GetYieldChangeNaturalWonder(YieldTypes eYieldType) const;
	int GetYieldChangeWorldWonder(YieldTypes eYieldType) const;
	int GetYieldModifierNaturalWonder(YieldTypes eYieldType) const;
	int GetMaxYieldModifierPerFollower(YieldTypes eYieldType) const;

	bool IsBuildingClassEnabled(BuildingClassTypes eType) const;
	bool IsFaithBuyingEnabled(EraTypes eEra) const;
#if defined(MOD_BALANCE_CORE)
	bool IsSpecificFaithBuyingEnabled(UnitTypes eUnit) const;
#endif
	bool IsConvertsBarbarians() const;
	bool IsFaithPurchaseAllGreatPeople() const;

#if defined(MOD_BALANCE_CORE_BELIEFS_RESOURCE)
	bool RequiresImprovement() const;
	bool RequiresResource() const;
	bool RequiresNoImprovement() const;
	bool RequiresNoImprovementFeature() const;
#endif

#if defined(MOD_BALANCE_CORE_BELIEFS)
	bool IsHalvedFollowers() const;
	int GetYieldPerPop(YieldTypes eYieldType) const;
	int GetYieldPerGPT(YieldTypes eYieldType) const;
	int GetYieldPerLux(YieldTypes eYieldType) const;
	int GetYieldPerBorderGrowth(YieldTypes eYieldType) const;
	int GetYieldPerHeal(YieldTypes eYieldType) const;
	int GetYieldPerBirth(YieldTypes eYieldType) const;
	int GetYieldPerScience(YieldTypes eYieldType) const;
	int GetYieldFromGPUse(YieldTypes eYieldType) const;
	int GetYieldBonusGoldenAge(YieldTypes eYieldType) const;
	int GetYieldFromSpread(YieldTypes eYieldType) const;
	int GetYieldFromForeignSpread(YieldTypes eYieldType) const;
	int GetYieldFromConquest(YieldTypes eYieldType) const;
	int GetYieldFromPolicyUnlock(YieldTypes eYieldType) const;
	int GetYieldFromEraUnlock(YieldTypes eYieldType) const;
	int GetYieldFromConversion(YieldTypes eYieldType) const;
	int GetYieldFromWLTKD(YieldTypes eYieldType) const;
	int GetYieldFromProposal(YieldTypes eYieldType) const;
	int GetYieldFromHost(YieldTypes eYieldType) const;
	int GetMaxYieldPerFollower(YieldTypes eYieldType) const;
	int GetYieldFromKnownPantheons(YieldTypes eYieldType) const;
	CivilizationTypes GetUniqueCiv() const;
#endif

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
#if defined(MOD_BALANCE_CORE_BELIEFS)
	int m_iCombatVersusOtherReligionOwnLands;
	int m_iCombatVersusOtherReligionTheirLands;
	int m_iMissionaryInfluenceCS;
	int m_iHappinessPerPantheon;
	int m_iExtraVotes;
#endif
#if defined(MOD_BALANCE_CORE)
	CivilizationTypes m_eRequiredCivilization;
#endif
	EraTypes m_eObsoleteEra;
	ResourceTypes m_eResourceRevealed;
	TechTypes m_eSpreadModifierDoublingTech;

	BeliefList m_ReligionBeliefs;
#if defined(MOD_BALANCE_CORE)
	std::vector<int> m_BeliefLookup;
#endif

	// Arrays
	int* m_paiBuildingClassEnabled;
};

namespace CvBeliefHelpers
{
#if defined(MOD_EVENTS_UNIT_CAPTURE)
	bool ConvertBarbarianUnit(const CvUnit *pByUnit, UnitHandle pUnit);
#else
	bool ConvertBarbarianUnit(CvPlayer *pPlayer, UnitHandle pUnit);
#endif
}

#endif //CIV5_BELIEF_CLASSES_H