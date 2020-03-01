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
	int GetUnitProductionModifier() const;
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
	int GetInquisitorCostModifier() const;
	int GetFriendlyCityStateSpreadModifier() const;
	int GetGreatPersonExpendedFaith() const;
	int GetCityStateMinimumInfluence() const;
	int GetCityStateInfluenceModifier() const;
	int GetOtherReligionPressureErosion() const;
	int GetSpyPressure() const;
	int GetInquisitorPressureRetention() const;
	int GetFullyConvertedHappiness() const;
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
	bool RequiresNoFeature() const;
#endif

#if defined(MOD_BALANCE_CORE_BELIEFS)
	int GetHappinessFromForeignSpies() const;
	int GetPressureChangeTradeRoute() const;
	int GetYieldPerActiveTR(int i) const;
	int GetYieldPerConstruction(int i) const;
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
	int GetYieldFromConversionExpo(int i) const;
	int GetYieldFromWLTKD(int i) const;
	int GetYieldFromProposal(int i) const;
	int GetYieldFromHost(int i) const;
	int GetYieldFromFaithPurchase(int i) const;
	int GetYieldFromKnownPantheons(int i) const;
	int GetCombatVersusOtherReligionOwnLands() const;
	int GetCombatVersusOtherReligionTheirLands() const;
	int GetMissionaryInfluenceCS()const;
	int GetHappinessPerPantheon() const;
	int GetExtraVotes() const;
	int GetCityScalerLimiter() const;
	int GetFollowerScalerLimiter() const;
	int GetPolicyReductionWonderXFollowerCities() const;
	int GetMaxYieldPerFollower(int i) const;
	int GetMaxYieldPerFollowerPercent(int i) const;
	int GetIgnorePolicyRequirementsAmount() const;
	int GetCSYieldBonus() const;
	int GetImprovementVoteChange(ImprovementTypes eImprovement) const;
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
	int GetYieldFromRemoveHeresy(YieldTypes eYield) const;
	int GetYieldFromBarbarianKills(YieldTypes eYield) const;
	int GetGreatPersonPoints(GreatPersonTypes eGreatPerson) const;
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
	int GetMaxYieldModifierPerFollowerPercent(int i) const;
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
	int m_iUnitProductionModifier;
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
	int m_iInquisitorCostModifier;
	int m_iFriendlyCityStateSpreadModifier;
	int m_iGreatPersonExpendedFaith;
	int m_iCityStateMinimumInfluence;
	int m_iCityStateInfluenceModifier;
	int m_iOtherReligionPressureErosion;
	int m_iSpyPressure;
	int m_iInquisitorPressureRetention;
	int m_iFaithBuildingTourism;
	int m_iFullyConvertedHappiness;

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
	bool m_bRequiresNoFeature;
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
	int* m_piGreatPersonPoints;
	int* m_piCapitalYieldChange;
	int* m_piCoastalCityYieldChange;
	int* m_piGreatWorkYieldChange;
	int* m_piYieldFromKills;
	int* m_piYieldFromRemoveHeresy;
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
	int* m_piMaxYieldModifierPerFollowerPercent;
#if defined(MOD_BALANCE_CORE)
	bool* m_pbFaithPurchaseUnitSpecificEnabled;
#endif
	bool* m_pbFaithPurchaseUnitEraEnabled;
    bool* m_pbBuildingClassEnabled;

#if defined(MOD_BALANCE_CORE_BELIEFS)
	int m_iHappinessFromForeignSpies;
	int m_iGetPressureChangeTradeRoute;
	int* m_piYieldPerActiveTR;
	int* m_piYieldPerConstruction;
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
	int* m_piYieldFromConversionExpo;
	int* m_piYieldFromWLTKD;
	int* m_piYieldFromProposal;
	int* m_piYieldFromHost;
	int* m_piYieldFromFaithPurchase;
	int* m_piYieldFromKnownPantheons;
	int* m_piMaxYieldPerFollower;
	int* m_piMaxYieldPerFollowerPercent;
	int* m_piImprovementVoteChange;
	int m_iReducePolicyRequirements;
	int m_iCSYieldBonus;
	int m_iCombatVersusOtherReligionOwnLands;
	int m_iCombatVersusOtherReligionTheirLands;
	int m_iMissionaryInfluenceCS;
	int m_iHappinessPerPantheon;
	int m_iExtraVotes;
	int m_iCityScalerLimiter;
	int m_iFollowerScalerLimiter;
	int m_iPolicyReductionWonderXFollowerCities;
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
#if defined(MOD_BALANCE_CORE)
	void SetReligion(ReligionTypes eReligion);
	ReligionTypes GetReligion() const;
#endif

	// Accessor functions
	bool HasBelief(BeliefTypes eBelief) const;
	BeliefTypes GetBelief(int iIndex) const;
	int GetNumBeliefs() const;
#if defined(MOD_BALANCE_CORE)
	bool IsBeliefValid(BeliefTypes eBelief, ReligionTypes eReligion, PlayerTypes ePlayer, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
#endif

	int GetFaithFromDyingUnits(PlayerTypes ePlayer = NO_PLAYER, bool bHolyCityOnly = false) const;
	int GetRiverHappiness(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetPlotCultureCostModifier(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetCityRangeStrikeModifier(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetCombatModifierEnemyCities(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetCombatModifierFriendlyCities(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetFriendlyHealChange(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetCityStateFriendshipModifier(PlayerTypes ePlayer = NO_PLAYER, bool bHolyCityOnly = false) const;
	int GetLandBarbarianConversionPercent(PlayerTypes ePlayer = NO_PLAYER, bool bHolyCityOnly = false) const;
	int GetSpreadDistanceModifier(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetSpreadStrengthModifier(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetProphetStrengthModifier(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetProphetCostModifier(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetMissionaryStrengthModifier(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetMissionaryCostModifier(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetInquisitorCostModifier(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetFriendlyCityStateSpreadModifier(PlayerTypes ePlayer = NO_PLAYER, bool bHolyCityOnly = false) const;
	int GetGreatPersonExpendedFaith(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetCityStateMinimumInfluence(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetCityStateInfluenceModifier(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetOtherReligionPressureErosion(PlayerTypes ePlayer = NO_PLAYER, bool bHolyCityOnly = false) const;
	int GetSpyPressure(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetInquisitorPressureRetention(PlayerTypes ePlayer = NO_PLAYER, bool bHolyCityOnly = false) const;
	int GetFaithBuildingTourism(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetFullyConvertedHappiness(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
#if defined(MOD_BALANCE_CORE_BELIEFS)
	int GetCombatVersusOtherReligionOwnLands(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetCombatVersusOtherReligionTheirLands(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetMissionaryInfluenceCS(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetHappinessPerPantheon(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetExtraVotes(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetPolicyReductionWonderXFollowerCities(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetCityScalerLimiter(int iCap = 0) const;
	int GetFollowerScalerLimiter(int iCap = 0) const;
#endif
	EraTypes GetObsoleteEra(PlayerTypes ePlayer = NO_PLAYER, bool bHolyCityOnly = false) const;
	ResourceTypes GetResourceRevealed(PlayerTypes ePlayer = NO_PLAYER, bool bHolyCityOnly = false) const;
	TechTypes GetSpreadModifierDoublingTech(PlayerTypes ePlayer = NO_PLAYER, bool bHolyCityOnly = false) const;

	int GetFaithFromKills(int iDistance, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetHappinessPerCity(int iPopulation, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetHappinessPerXPeacefulForeignFollowers(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetWonderProductionModifier(EraTypes eWonderEra, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetUnitProductionModifier(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetPlayerHappiness(bool bAtPeace, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetPlayerCultureModifier(bool bAtPeace , PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	float GetHappinessPerFollowingCity(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetGoldPerFollowingCity(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetGoldPerXFollowers(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetGoldWhenCityAdopts(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetSciencePerOtherReligionFollower(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetCityGrowthModifier(bool bAtPeace, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;

	int GetCityYieldChange(int iPopulation, YieldTypes eYield, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetHolyCityYieldChange(YieldTypes eYield, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldChangePerForeignCity(YieldTypes eYield, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldChangePerXForeignFollowers(YieldTypes eYield, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
#if defined(MOD_API_UNIFIED_YIELDS)
	int GetYieldPerFollowingCity(YieldTypes eYield, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldPerXFollowers(YieldTypes eYield, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldPerOtherReligionFollower(YieldTypes eYield, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
#endif
	int GetResourceQuantityModifier(ResourceTypes eResource , PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYield, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetBuildingClassYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYieldType, int iFollowers, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetBuildingClassHappiness(BuildingClassTypes eBuildingClass, int iFollowers, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetBuildingClassTourism(BuildingClassTypes eBuildingClass, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
#if defined(MOD_API_UNIFIED_YIELDS)
	int GetYieldPerXTerrainTimes100(TerrainTypes eTerrain, YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldPerXFeatureTimes100(FeatureTypes eFeature, YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetCityYieldFromUnimprovedFeature(FeatureTypes eFeature, YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetUnimprovedFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetLakePlotYieldChange(YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
#endif
	int GetResourceYieldChange(ResourceTypes eResource, YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetTerrainYieldChange(TerrainTypes eTerrain, YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
#if defined(MOD_API_UNIFIED_YIELDS)
	int GetTradeRouteYieldChange(DomainTypes eDomain, YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetSpecialistYieldChange(SpecialistTypes eSpecialist, YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetGreatPersonExpendedYield(GreatPersonTypes eGreatPerson, YieldTypes eYieldType , PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetGoldenAgeGreatPersonRateModifier(GreatPersonTypes eGreatPerson, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetCapitalYieldChange(int iPopulation, YieldTypes eYield, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetCoastalCityYieldChange(int iPopulation, YieldTypes eYield, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetGreatWorkYieldChange(int iPopulation, YieldTypes eYield, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldFromBarbarianKills(YieldTypes eYield, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldFromKills(YieldTypes eYield, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldFromRemoveHeresy(YieldTypes eYield, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetGreatPersonPoints(GreatPersonTypes eGreatPerson, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
#endif
#if defined(MOD_RELIGION_PLOT_YIELDS)
	int GetPlotYieldChange(PlotTypes ePlot, YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
#endif
	int GetResourceHappiness(ResourceTypes eResource , PlayerTypes ePlayer = NO_PLAYER, bool bHolyCityOnly = false) const;
	int GetYieldChangeAnySpecialist(YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldChangeTradeRoute(YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldChangeNaturalWonder(YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldChangeWorldWonder(YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldModifierNaturalWonder(YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetMaxYieldModifierPerFollower(YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetMaxYieldModifierPerFollowerPercent(int& iMaxVal, YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;

	bool IsBuildingClassEnabled(BuildingClassTypes eType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	bool IsFaithBuyingEnabled(EraTypes eEra, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
#if defined(MOD_BALANCE_CORE)
	bool IsSpecificFaithBuyingEnabled(UnitTypes eUnit, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	BeliefTypes GetSpecificFaithBuyingEnabledBelief(UnitTypes eUnit) const;
#endif
	bool IsConvertsBarbarians(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	bool IsFaithPurchaseAllGreatPeople(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;

#if defined(MOD_BALANCE_CORE_BELIEFS_RESOURCE)
	bool RequiresImprovement(PlayerTypes ePlayer = NO_PLAYER, bool bHolyCityOnly = false) const;
	bool RequiresResource(PlayerTypes ePlayer = NO_PLAYER, bool bHolyCityOnly = false) const;
	bool RequiresNoImprovement(PlayerTypes ePlayer = NO_PLAYER, bool bHolyCityOnly = false) const;
	bool RequiresNoFeature(PlayerTypes ePlayer = NO_PLAYER, bool bHolyCityOnly = false) const;
#endif

#if defined(MOD_BALANCE_CORE_BELIEFS)
	int GetHappinessFromForeignSpies(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetPressureChangeTradeRoute(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldPerActiveTR(YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldPerConstruction(YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldPerPop(YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldPerGPT(YieldTypes eYieldType , PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldPerLux(YieldTypes eYieldType , PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldPerBorderGrowth(YieldTypes eYieldType , PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldPerHeal(YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldPerBirth(YieldTypes eYieldType , PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldPerScience(YieldTypes eYieldType , PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldFromGPUse(YieldTypes eYieldType , PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldBonusGoldenAge(YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldFromSpread(YieldTypes eYieldType , PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldFromForeignSpread(YieldTypes eYieldType , PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldFromConquest(YieldTypes eYieldType , PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldFromPolicyUnlock(YieldTypes eYieldType , PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldFromEraUnlock(YieldTypes eYieldType , PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldFromConversion(YieldTypes eYieldType , PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldFromConversionExpo(YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldFromWLTKD(YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldFromProposal(YieldTypes eYieldType , PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldFromHost(YieldTypes eYieldType , PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldFromFaithPurchase(YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetMaxYieldPerFollower(YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetMaxYieldPerFollowerPercent(YieldTypes eYieldType, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetYieldFromKnownPantheons(YieldTypes eYieldType , PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	CivilizationTypes GetUniqueCiv(PlayerTypes ePlayer = NO_PLAYER, bool bHolyCityOnly = false) const;
	int GetIgnorePolicyRequirementsAmount(EraTypes eEra, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetCSYieldBonus(PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
	int GetVoteFromOwnedImprovement(ImprovementTypes eImprovement, PlayerTypes ePlayer = NO_PLAYER, const CvCity* pCity = NULL, bool bHolyCityOnly = false) const;
#endif

	// Serialization
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

private:
	// Cached data about this religion's beliefs
#if !defined(MOD_BALANCE_CORE)
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
#endif

	BeliefList m_ReligionBeliefs;
#if defined(MOD_BALANCE_CORE)
	ReligionTypes m_eReligion;
	std::vector<int> m_BeliefLookup;
#endif

	// Arrays
	// int* m_paiBuildingClassEnabled;
	//int m_iNeedThisToCompile;
};

namespace CvBeliefHelpers
{
#if defined(MOD_EVENTS_UNIT_CAPTURE)
	bool ConvertBarbarianUnit(const CvUnit *pByUnit, CvUnit* pUnit);
#else
	bool ConvertBarbarianUnit(CvPlayer *pPlayer, CvUnit* pUnit);
#endif
}

#endif //CIV5_BELIEF_CLASSES_H