/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvInfosSerializationHelper.h"
#include "CvBarbarians.h"

#include "LintFree.h"

//======================================================================================================
//					CvBeliefEntry
//======================================================================================================
/// Constructor
CvBeliefEntry::CvBeliefEntry() :
	m_iMinPopulation(0),
	m_iMinFollowers(0),
	m_iMaxDistance(0),
	m_iCityGrowthModifier(0),
	m_iFaithFromKills(0),
	m_iFaithFromDyingUnits(0),
	m_iRiverHappiness(0),
	m_iHappinessPerCity(0),
	m_iHappinessPerXPeacefulForeignFollowers(0),
	m_iBorderGrowthRateIncreaseGlobal(0),
	m_iPlotCultureCostModifier(0),
	m_iCityRangeStrikeModifier(0),
	m_iCombatModifierEnemyCities(0),
	m_iCombatModifierFriendlyCities(0),
	m_iFriendlyHealChange(0),
	m_iCityStateFriendshipModifier(0),
	m_iLandBarbarianConversionPercent(0),
	m_iWonderProductionModifier(0),
	m_iUnitProductionModifier(0),
	m_iPlayerHappiness(0),
	m_iPlayerCultureModifier(0),
	m_fHappinessPerFollowingCity(0),
	m_iGoldPerFollowingCity(0),
	m_iGoldPerXFollowers(0),
	m_iGoldWhenCityAdopts(0),
	m_iSciencePerOtherReligionFollower(0),
	m_iSpreadDistanceModifier(0),
	m_iSpreadStrengthModifier(0),
	m_iProphetStrengthModifier(0),
	m_iProphetCostModifier(0),
	m_iMissionaryStrengthModifier(0),
	m_iMissionaryCostModifier(0),
	m_iInquisitorCostModifier(0),
	m_iFriendlyCityStateSpreadModifier(0),
	m_iGreatPersonExpendedFaith(0),
	m_iCityStateMinimumInfluence(0),
	m_iCityStateInfluenceModifier(0),
	m_iOtherReligionPressureErosion(0),
	m_iSpyPressure(0),
	m_iInquisitorPressureRetention(0),
	m_iFaithBuildingTourism(0),
	m_iFullyConvertedHappiness(0),

	m_bPantheon(false),
	m_bFounder(false),
	m_bFollower(false),
	m_bEnhancer(false),
	m_bReformer(false),
	m_bRequiresPeace(false),
	m_bConvertsBarbarians(false),
	m_bFaithPurchaseAllGreatPeople(false),

	m_bRequiresImprovement(false),
	m_bRequiresResource(false),
	m_bRequiresNoImprovement(false),
	m_bRequiresNoFeature(false),

	m_iHappinessFromForeignSpies(0),
	m_iGetPressureChangeTradeRoute(0),
	m_piYieldPerActiveTR(NULL),
	m_piYieldPerConstruction(NULL),
	m_piYieldPerWorldWonderConstruction(NULL),
	m_piYieldPerPop(NULL),
	m_piYieldPerGPT(NULL),
	m_piYieldPerLux(NULL),
	m_pbiYieldPerBorderGrowth(),
	m_piYieldPerHeal(NULL),
	m_piYieldPerBirth(NULL),
	m_piYieldPerHolyCityBirth(NULL),
	m_piYieldPerScience(NULL),
	m_piYieldFromGPUse(NULL),
	m_piYieldBonusGoldenAge(NULL),
	m_piYieldFromSpread(NULL),
	m_piYieldFromForeignSpread(NULL),
	m_piYieldFromConquest(NULL),
	m_piYieldFromPolicyUnlock(NULL),
	m_piYieldFromEraUnlock(NULL),
	m_pbiYieldFromTechUnlock(),
	m_piYieldFromConversion(NULL),
	m_piYieldFromConversionExpo(NULL),
	m_piYieldFromWLTKD(NULL),
	m_piYieldFromProposal(NULL),
	m_piYieldFromHost(NULL),
	m_piYieldFromFaithPurchase(NULL),
	m_piYieldFromKnownPantheons(NULL),
	m_iCombatVersusOtherReligionOwnLands(0),
	m_iCombatVersusOtherReligionTheirLands(0),
	m_iMissionaryInfluenceCS(0),
	m_iHappinessPerPantheon(0),
	m_iExtraVotes(0),
	m_iCityScalerLimiter(0),
	m_iFollowerScalerLimiter(0),
	m_iPolicyReductionWonderXFollowerCities(0),
	m_bAIGoodStartingPantheon(false),
	m_piMaxYieldPerFollower(NULL),
	m_piMaxYieldPerFollowerPercent(NULL),
	m_piImprovementVoteChange(NULL),
	m_iReducePolicyRequirements(0),
	m_iCSYieldBonus(0),

	m_eRequiredCivilization(NO_CIVILIZATION),

	m_eObsoleteEra(NO_ERA),
	m_eResourceRevealed(NO_RESOURCE),
	m_eSpreadModifierDoublingTech(NO_TECH),

	m_paiCityYieldChange(NULL),
	m_paiHolyCityYieldChange(NULL),
	m_paiYieldChangePerForeignCity(NULL),
	m_paiYieldChangePerXForeignFollowers(NULL),
	m_paiYieldChangePerXCityStateFollowers(NULL),
	m_piYieldPerFollowingCity(NULL),
	m_piYieldPerXFollowers(NULL),
	m_piYieldPerOtherReligionFollower(NULL),
	m_piResourceQuantityModifiers(NULL),
	m_ppiImprovementYieldChanges(NULL),
	m_ppiBuildingClassYieldChanges(NULL),
	m_paiBuildingClassHappiness(NULL),
	m_ppaiFeatureYieldChange(NULL),
	m_ppiYieldPerXTerrain(NULL),
	m_ppiYieldPerXFeature(NULL),
	m_ppiCityYieldFromUnimprovedFeature(NULL),
	m_ppiUnimprovedFeatureYieldChanges(NULL),
	m_paiLakePlotYieldChange(NULL),
	m_ppaiResourceYieldChange(NULL),
	m_ppaiTerrainYieldChange(NULL),
	m_ppiTradeRouteYieldChange(NULL),
	m_ppiSpecialistYieldChange(NULL),
	m_ppiGreatPersonExpendedYield(NULL),
	m_piGoldenAgeGreatPersonRateModifier(NULL),
	m_piGreatPersonPoints(NULL),
	m_piCapitalYieldChange(NULL),
	m_piCoastalCityYieldChange(NULL),
	m_piGreatWorkYieldChange(NULL),
	m_piYieldFromKills(NULL),
	m_piYieldFromRemoveHeresy(NULL),
	m_piYieldFromBarbarianKills(NULL),
	m_ppiPlotYieldChange(NULL),
	m_pbFaithPurchaseUnitSpecificEnabled(NULL),

	m_aiFreePromotions(),
	m_pbiYieldFromImprovementBuild(),
	m_pbiYieldFromPillageGlobal(),

	m_piResourceHappiness(NULL),
	m_piYieldChangeAnySpecialist(NULL),
	m_piYieldChangeTradeRoute(NULL),
	m_piYieldChangeNaturalWonder(NULL),
	m_piYieldChangeWorldWonder(NULL),
	m_piYieldModifierNaturalWonder(NULL),
	m_piMaxYieldModifierPerFollower(NULL),
	m_piMaxYieldModifierPerFollowerPercent(NULL),
	m_pbFaithPurchaseUnitEraEnabled(NULL),
	m_pbBuildingClassEnabled(NULL)
{
}

/// Destructor
CvBeliefEntry::~CvBeliefEntry()
{
	CvDatabaseUtility::SafeDelete2DArray(m_ppiImprovementYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiBuildingClassYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiFeatureYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiYieldPerXTerrain);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiYieldPerXFeature);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiCityYieldFromUnimprovedFeature);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiUnimprovedFeatureYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiResourceYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiTerrainYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiTradeRouteYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiSpecialistYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiGreatPersonExpendedYield);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiPlotYieldChange);

	if (MOD_RELIGION_EXTENSIONS)
	{
		m_aiFreePromotions.clear();
		m_pbiYieldFromImprovementBuild.clear();
		m_pbiYieldFromPillageGlobal.clear();
	}
}

/// Accessor:: Minimum population in this city for belief to be active (0 = no such requirement)
int CvBeliefEntry::GetMinPopulation() const
{
	return m_iMinPopulation;
}

/// Accessor:: Minimum followers in this city for belief to be active (0 = no such requirement)
int CvBeliefEntry::GetMinFollowers() const
{
	return m_iMinFollowers;
}

/// Accessor:: Maximum distance from a city of this religion for belief to be active (0 = no such requirement)
int CvBeliefEntry::GetMaxDistance() const
{
	return m_iMaxDistance;
}

/// Accessor:: Modifier to city growth rate
int CvBeliefEntry::GetCityGrowthModifier() const
{
	return m_iCityGrowthModifier;
}

/// Accessor:: Percentage of enemy strength received in Faith for killing him
int CvBeliefEntry::GetFaithFromKills() const
{
	return m_iFaithFromKills;
}

/// Accessor:: Faith received when a friendly unit dies
int CvBeliefEntry::GetFaithFromDyingUnits() const
{
	return m_iFaithFromDyingUnits;
}

/// Accessor:: Happiness from each city settled on a river
int CvBeliefEntry::GetRiverHappiness() const
{
	return m_iRiverHappiness;
}

/// Accessor:: Happiness per every X population in a city
int CvBeliefEntry::GetHappinessPerCity() const
{
	return m_iHappinessPerCity;
}

/// Accessor:: Happiness per every X population in a foreign city
int CvBeliefEntry::GetHappinessPerXPeacefulForeignFollowers() const
{
	return m_iHappinessPerXPeacefulForeignFollowers;
}

/// Accessor:: Boost in speed of acquiring tiles through culture (rate increase)
int CvBeliefEntry::GetBorderGrowthRateIncreaseGlobal() const
{
	return m_iBorderGrowthRateIncreaseGlobal;
}

/// Accessor:: Boost in speed of acquiring tiles through culture (cost reduction)
int CvBeliefEntry::GetPlotCultureCostModifier() const
{
	return m_iPlotCultureCostModifier;
}

/// Accessor:: Boost in city strike strength
int CvBeliefEntry::GetCityRangeStrikeModifier() const
{
	return m_iCityRangeStrikeModifier;
}

/// Accessor:: Boost in combat near enemy cities of this religion
int CvBeliefEntry::GetCombatModifierEnemyCities() const
{
	return m_iCombatModifierEnemyCities;
}

/// Accessor:: Boost in combat near friendly cities of this religion
int CvBeliefEntry::GetCombatModifierFriendlyCities() const
{
	return m_iCombatModifierFriendlyCities;
}

/// Accessor:: Additional healing in friendly territory
int CvBeliefEntry::GetFriendlyHealChange() const
{
	return m_iFriendlyHealChange;
}

/// Accessor:: Boost in city state influence effectiveness
int CvBeliefEntry::GetCityStateFriendshipModifier() const
{
	return m_iCityStateFriendshipModifier;
}

/// Accessor:: Chance of converting a barbarian camp guard
int CvBeliefEntry::GetLandBarbarianConversionPercent() const
{
	return m_iLandBarbarianConversionPercent;
}

/// Accessor:: boost in production speed for wonders prior to obsolete era
int CvBeliefEntry::GetWonderProductionModifier() const
{
	return m_iWonderProductionModifier;
}

int CvBeliefEntry::GetUnitProductionModifier() const
{
	return m_iUnitProductionModifier;
}

/// Accessor:: boost in production speed for wonders prior to obsolete era
int CvBeliefEntry::GetPlayerHappiness() const
{
	return m_iPlayerHappiness;
}

/// Accessor:: boost in production speed for wonders prior to obsolete era
int CvBeliefEntry::GetPlayerCultureModifier() const
{
	return m_iPlayerCultureModifier;
}

/// Accessor:: amount of extra happiness from each city following this religion
float CvBeliefEntry::GetHappinessPerFollowingCity() const
{
	return m_fHappinessPerFollowingCity;
}

/// Accessor:: amount of extra gold from each city following this religion
int CvBeliefEntry::GetGoldPerFollowingCity() const
{
	return m_iGoldPerFollowingCity;
}

/// Accessor:: amount of extra gold from each city following this religion
int CvBeliefEntry::GetGoldPerXFollowers() const
{
	return m_iGoldPerXFollowers;
}

/// Accessor:: amount of extra gold from each city following this religion
int CvBeliefEntry::GetGoldWhenCityAdopts() const
{
	return m_iGoldWhenCityAdopts;
}

/// Accessor:: amount of science for each follower of another religion in city spread to
int CvBeliefEntry::GetSciencePerOtherReligionFollower() const
{
	return m_iSciencePerOtherReligionFollower;
}

/// Accessor:: extra distance in city-to-city religion spread
int CvBeliefEntry::GetSpreadDistanceModifier() const
{
	return m_iSpreadDistanceModifier;
}

/// Accessor:: extra strength in city-to-city religion spread
int CvBeliefEntry::GetSpreadStrengthModifier() const
{
	return m_iSpreadStrengthModifier;
}

/// Accessor:: prophet conversion strength modifier
int CvBeliefEntry::GetProphetStrengthModifier() const
{
	return m_iProphetStrengthModifier;
}

/// Accessor:: prophet cost discount
int CvBeliefEntry::GetProphetCostModifier() const
{
	return m_iProphetCostModifier;
}

/// Accessor:: missionary conversion strength modifier
int CvBeliefEntry::GetMissionaryStrengthModifier() const
{
	return m_iMissionaryStrengthModifier;
}

/// Accessor:: missionary cost discount
int CvBeliefEntry::GetMissionaryCostModifier() const
{
	return m_iMissionaryCostModifier;
}

/// Accessor:: missionary cost discount
int CvBeliefEntry::GetInquisitorCostModifier() const
{
	return m_iInquisitorCostModifier;
}


/// Accessor: speed increase of spread to friendly city states
int CvBeliefEntry::GetFriendlyCityStateSpreadModifier() const
{
	return m_iFriendlyCityStateSpreadModifier;
}

/// Accessor: faith earned for each GP expended
int CvBeliefEntry::GetGreatPersonExpendedFaith() const
{
	return m_iGreatPersonExpendedFaith;
}

/// Accessor: minimum influence with city states of a shared religion
int CvBeliefEntry::GetCityStateMinimumInfluence() const
{
	return m_iCityStateMinimumInfluence;
}

/// Accessor: modifier to influence boosts with city states
int CvBeliefEntry::GetCityStateInfluenceModifier() const
{
	return m_iCityStateInfluenceModifier;
}

/// Accessor: percentage of religious pressure gain that becomes a drop in pressure of other religions
int CvBeliefEntry::GetOtherReligionPressureErosion() const
{
	return m_iOtherReligionPressureErosion;
}

/// Accessor: base religious pressure (before speed multiplier) from having a spy in a city
int CvBeliefEntry::GetSpyPressure() const
{
	return m_iSpyPressure;
}

/// Accessor: percentage of religious pressure retained if one of your cities is hit with an Inquisitor
int CvBeliefEntry::GetInquisitorPressureRetention() const
{
	return m_iInquisitorPressureRetention;
}

/// Accessor: how much tourism can I get from Buildings bought with Faith?
int CvBeliefEntry::GetFaithBuildingTourism() const
{
	return m_iFaithBuildingTourism;
}

int CvBeliefEntry::GetFullyConvertedHappiness() const
{
	return m_iFullyConvertedHappiness;
}

/// Accessor: is this a belief a pantheon can adopt
bool CvBeliefEntry::IsPantheonBelief() const
{
	return m_bPantheon;
}

/// Accessor: is this a belief a religion founder can adopt
bool CvBeliefEntry::IsFounderBelief() const
{
	return m_bFounder;
}

/// Accessor: is this a belief a religion follower can adopt
bool CvBeliefEntry::IsFollowerBelief() const
{
	return m_bFollower;
}

/// Accessor: is this a belief that enhances the spread of the religion
bool CvBeliefEntry::IsEnhancerBelief() const
{
	return m_bEnhancer;
}

/// Accessor: is this a belief that is added with the Reformation social policy
bool CvBeliefEntry::IsReformationBelief() const
{
	return m_bReformer;
}

/// Accessor: is this a belief that requires you to be at peace to benefit?
bool CvBeliefEntry::RequiresPeace() const
{
	return m_bRequiresPeace;
}

/// Accessor: is this a belief that allows your missionaries to convert adjacent barbarians?
bool CvBeliefEntry::ConvertsBarbarians() const
{
	return m_bConvertsBarbarians;
}

/// Accessor: is this a belief that allows you to purchase any type of Great Person with Faith?
bool CvBeliefEntry::FaithPurchaseAllGreatPeople() const
{
	return m_bFaithPurchaseAllGreatPeople;
}

/// Accessor: is this a belief that grants faith only from improvements?
bool CvBeliefEntry::RequiresImprovement() const
{
	return m_bRequiresImprovement;
}
/// Accessor: is this a belief that grants faith only from resources?
bool CvBeliefEntry::RequiresResource() const
{
	return m_bRequiresResource;
}
/// Accessor: is this a belief that grants faith only from no improvements?
bool CvBeliefEntry::RequiresNoImprovement() const
{
	return m_bRequiresNoImprovement;
}
/// Accessor: is this a belief that grants faith only from no improvements on a feature?
bool CvBeliefEntry::RequiresNoFeature() const
{
	return m_bRequiresNoFeature;
}

int CvBeliefEntry::GetHappinessFromForeignSpies() const
{
	return m_iHappinessFromForeignSpies;
}
int CvBeliefEntry::GetPressureChangeTradeRoute() const
{
	return m_iGetPressureChangeTradeRoute;
}
/// Accessor:: Yield Per Pop
int CvBeliefEntry::GetYieldPerActiveTR(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldPerActiveTR ? m_piYieldPerActiveTR[i] : -1;
}

int CvBeliefEntry::GetYieldPerConstruction(int i) const
{
	return m_piYieldPerConstruction ? m_piYieldPerConstruction[i] : -1;
}
int CvBeliefEntry::GetYieldPerWorldWonderConstruction(int i) const
{
	return m_piYieldPerWorldWonderConstruction ? m_piYieldPerWorldWonderConstruction[i] : -1;
}

int CvBeliefEntry::GetYieldPerPop(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldPerPop ? m_piYieldPerPop[i] : -1;
}
/// Accessor:: Yield Per GPT
int CvBeliefEntry::GetYieldPerGPT(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldPerGPT ? m_piYieldPerGPT[i] : -1;
}
/// Accessor:: Yield Per Luxury
int CvBeliefEntry::GetYieldPerLux(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldPerLux ? m_piYieldPerLux[i] : -1;
}
/// Accessor:: Yield Per Border Growth
int CvBeliefEntry::GetYieldPerBorderGrowth (YieldTypes eYield, bool bEraScaling) const
{
	PRECONDITION(eYield < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eYield > -1, "Index out of bounds");

	std::map<int, std::map<bool, int>>::const_iterator itYield = m_pbiYieldPerBorderGrowth.find((int)eYield);
	if (itYield != m_pbiYieldPerBorderGrowth.end())
	{
		std::map<bool, int>::const_iterator itBool = itYield->second.find(bEraScaling);
		if (itBool != itYield->second.end())
		{
			return itBool->second;
		}
	}
	return 0;
}
/// Accessor:: Yield Per Heal
int CvBeliefEntry::GetYieldPerHeal (int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldPerHeal ? m_piYieldPerHeal[i] : -1;
}
/// Accessor:: Yield Per Birth
int CvBeliefEntry::GetYieldPerBirth(int i) const
{
	return m_piYieldPerBirth ? m_piYieldPerBirth[i] : -1;
}
/// Accessor:: Yield Per Holy City Birth
int CvBeliefEntry::GetYieldPerHolyCityBirth(int i) const
{
	return m_piYieldPerHolyCityBirth ? m_piYieldPerHolyCityBirth[i] : -1;
}

/// Accessor:: Yield Per Science
int CvBeliefEntry::GetYieldPerScience(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldPerScience ? m_piYieldPerScience[i] : -1;
}
/// Accessor:: Yield Per GP Use
int CvBeliefEntry::GetYieldFromGPUse(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldFromGPUse ? m_piYieldFromGPUse[i] : -1;
}
/// Accessor:: Yield Bonus GA
int CvBeliefEntry::GetYieldBonusGoldenAge(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldBonusGoldenAge ? m_piYieldBonusGoldenAge[i] : -1;
}
/// Accessor:: Yield From Spread
int CvBeliefEntry::GetYieldFromSpread(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldFromSpread ? m_piYieldFromSpread[i] : -1;
}
/// Accessor:: Yield From Foreign Spread
int CvBeliefEntry::GetYieldFromForeignSpread(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldFromForeignSpread ? m_piYieldFromForeignSpread[i] : -1;
}
/// Accessor:: Yield Conquest
int CvBeliefEntry::GetYieldFromConquest(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldFromConquest ? m_piYieldFromConquest[i] : -1;
}
/// Accessor:: Yield Policy Unlock
int CvBeliefEntry::GetYieldFromPolicyUnlock(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldFromPolicyUnlock ? m_piYieldFromPolicyUnlock[i] : -1;
}
/// Accessor:: Yield Era Unlock
int CvBeliefEntry::GetYieldFromEraUnlock(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldFromEraUnlock ? m_piYieldFromEraUnlock[i] : -1;
}
/// Accessor:: Yield Tech Unlock
int CvBeliefEntry::GetYieldFromTechUnlock(YieldTypes eYield, bool bEraScaling) const
{
	PRECONDITION(eYield < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eYield > -1, "Index out of bounds");

	std::map<int, std::map<bool, int>>::const_iterator itYield = m_pbiYieldFromTechUnlock.find((int)eYield);
	if (itYield != m_pbiYieldFromTechUnlock.end())
	{
		std::map<bool, int>::const_iterator itBool = itYield->second.find(bEraScaling);
		if (itBool != itYield->second.end())
		{
			return itBool->second;
		}
	}
	return 0;
}
/// Accessor:: Yield from Conversion
int CvBeliefEntry::GetYieldFromConversion(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldFromConversion ? m_piYieldFromConversion[i] : -1;
}
int CvBeliefEntry::GetYieldFromConversionExpo(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldFromConversionExpo ? m_piYieldFromConversionExpo[i] : -1;
}

/// Accessor:: Yield from WTLKD
int CvBeliefEntry::GetYieldFromWLTKD(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldFromWLTKD ? m_piYieldFromWLTKD[i] : -1;
}
/// Accessor:: Yield from Proposal
int CvBeliefEntry::GetYieldFromProposal(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldFromProposal ? m_piYieldFromProposal[i] : -1;
}
/// Accessor:: Yield from Vote
int CvBeliefEntry::GetYieldFromHost(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldFromHost ? m_piYieldFromHost[i] : -1;
}

int CvBeliefEntry::GetYieldFromFaithPurchase(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldFromFaithPurchase ? m_piYieldFromFaithPurchase[i] : -1;
}
/// Accessor:: Yield from Known Pantheons
int CvBeliefEntry::GetYieldFromKnownPantheons(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldFromKnownPantheons ? m_piYieldFromKnownPantheons[i] : -1;
}
/// Accessor:: Yield from Followers
int CvBeliefEntry::GetMaxYieldPerFollower(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piMaxYieldPerFollower ? m_piMaxYieldPerFollower[i] : -1;
}
/// Accessor:: Yield from Followers Halved
int CvBeliefEntry::GetMaxYieldPerFollowerPercent(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piMaxYieldPerFollowerPercent ? m_piMaxYieldPerFollowerPercent[i] : -1;
}

int CvBeliefEntry::GetIgnorePolicyRequirementsAmount() const
{
	return m_iReducePolicyRequirements;
}
int CvBeliefEntry::GetCSYieldBonus() const
{
	return m_iCSYieldBonus;
}

/// Accessor:: Extra yield from an improvement
int CvBeliefEntry::GetImprovementVoteChange(ImprovementTypes eIndex1) const
{
	PRECONDITION(eIndex1 < GC.getNumImprovementInfos(), "Index out of bounds");
	PRECONDITION(eIndex1 > -1, "Index out of bounds");
	return m_piImprovementVoteChange ? m_piImprovementVoteChange[eIndex1] : 0;
}

/// Accessor: combat bonus v. other in own lands
int CvBeliefEntry::GetCombatVersusOtherReligionOwnLands() const
{
	return m_iCombatVersusOtherReligionOwnLands;
}
/// Accessor: combat bonus v. other in enemy lands
int CvBeliefEntry::GetCombatVersusOtherReligionTheirLands() const
{
	return m_iCombatVersusOtherReligionTheirLands;
}
/// Accessor: missionary CS influence
int CvBeliefEntry::GetMissionaryInfluenceCS() const
{
	return m_iMissionaryInfluenceCS;
}
/// Accessor: Happy per pantheon known
int CvBeliefEntry::GetHappinessPerPantheon() const
{
	return m_iHappinessPerPantheon;
}
/// Accessor: Extra votes from WC
int CvBeliefEntry::GetExtraVotes() const
{
	return m_iExtraVotes;
}

int CvBeliefEntry::GetCityScalerLimiter() const
{
	return m_iCityScalerLimiter;
}

int CvBeliefEntry::GetFollowerScalerLimiter() const
{
	return m_iFollowerScalerLimiter;
}
/// Accessor: Extra Policy Reduction Wonder X Follower Cities
int CvBeliefEntry::GetPolicyReductionWonderXFollowerCities() const
{
	return m_iPolicyReductionWonderXFollowerCities;
}
bool CvBeliefEntry::IsAIGoodStartingPantheon() const
{
	return m_bAIGoodStartingPantheon;
}

//------------------------------------------------------------------------------
CivilizationTypes CvBeliefEntry::GetRequiredCivilization() const
{
	return m_eRequiredCivilization;
}
/// Accessor: era when wonder production modifier goes obsolete
EraTypes CvBeliefEntry::GetObsoleteEra() const
{
	return m_eObsoleteEra;
}

/// Accessor:: resource revealed near this city
ResourceTypes CvBeliefEntry::GetResourceRevealed() const
{
	return m_eResourceRevealed;
}

/// Accessor:: technology that doubles the effect of the SpreadStrengthModifier
TechTypes CvBeliefEntry::GetSpreadModifierDoublingTech() const
{
	return m_eSpreadModifierDoublingTech;
}

/// Accessor:: Get brief text description
const char* CvBeliefEntry::getShortDescription() const
{
	return m_strShortDescription;
}

/// Accessor:: Set brief text description
void CvBeliefEntry::setShortDescription(const char* szVal)
{
	m_strShortDescription = szVal;
}
/// Accessor:: Get brief text description
const char* CvBeliefEntry::getTooltip() const
{
	return m_strTooltip;
}

/// Accessor:: Set brief text description
void CvBeliefEntry::setTooltip(const char* szVal)
{
	m_strTooltip = szVal;
}

/// Accessor:: Additional yield
int CvBeliefEntry::GetCityYieldChange(int i) const
{
	return m_paiCityYieldChange ? m_paiCityYieldChange[i] : -1;
}

/// Accessor:: Additional player-level yield for controlling holy city
int CvBeliefEntry::GetHolyCityYieldChange(int i) const
{
	return m_paiHolyCityYieldChange ? m_paiHolyCityYieldChange[i] : -1;
}

/// Accessor:: Additional player-level yield for each foreign city converted
int CvBeliefEntry::GetYieldChangePerForeignCity(int i) const
{
	return m_paiYieldChangePerForeignCity ? m_paiYieldChangePerForeignCity[i] : -1;
}

/// Accessor:: Additional player-level yield for followers in foreign cities
int CvBeliefEntry::GetYieldChangePerXForeignFollowers(int i) const
{
	return m_paiYieldChangePerXForeignFollowers ? m_paiYieldChangePerXForeignFollowers[i] : -1;
}

/// Accessor:: Additional player-level yield for followers in City-States
int CvBeliefEntry::GetYieldChangePerXCityStateFollowers(int i) const
{
	return m_paiYieldChangePerXCityStateFollowers ? m_paiYieldChangePerXCityStateFollowers[i] : -1;
}

int CvBeliefEntry::GetYieldPerFollowingCity(int i) const
{
	return m_piYieldPerFollowingCity ? m_piYieldPerFollowingCity[i] : 0;
}

int CvBeliefEntry::GetYieldPerXFollowers(int i) const
{
	return m_piYieldPerXFollowers ? m_piYieldPerXFollowers[i] : 0;
}

int CvBeliefEntry::GetYieldPerOtherReligionFollower(int i) const
{
	return m_piYieldPerOtherReligionFollower ? m_piYieldPerOtherReligionFollower[i] : 0;
}

/// Accessor:: Additional quantity of a specific resource
int CvBeliefEntry::GetResourceQuantityModifier(int i) const
{
	PRECONDITION(i < GC.getNumResourceInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piResourceQuantityModifiers ? m_piResourceQuantityModifiers[i] : -1;
}

/// Accessor:: Extra yield from an improvement
int CvBeliefEntry::GetImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const
{
	PRECONDITION(eIndex1 < GC.getNumImprovementInfos(), "Index out of bounds");
	PRECONDITION(eIndex1 > -1, "Index out of bounds");
	PRECONDITION(eIndex2 < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eIndex2 > -1, "Index out of bounds");
	return m_ppiImprovementYieldChanges ? m_ppiImprovementYieldChanges[eIndex1][eIndex2] : 0;
}

/// Yield change for a specific BuildingClass by yield type
int CvBeliefEntry::GetBuildingClassYieldChange(int i, int j) const
{
	PRECONDITION(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	PRECONDITION(j < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(j > -1, "Index out of bounds");
	return m_ppiBuildingClassYieldChanges[i][j];
}

/// Amount of extra Happiness per turn a BuildingClass provides
int CvBeliefEntry::GetBuildingClassHappiness(int i) const
{
	PRECONDITION(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_paiBuildingClassHappiness ? m_paiBuildingClassHappiness[i] : -1;
}

/// Change to Feature yield by type
int CvBeliefEntry::GetFeatureYieldChange(int i, int j) const
{
	PRECONDITION(i < GC.getNumFeatureInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	PRECONDITION(j < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(j > -1, "Index out of bounds");
	return m_ppaiFeatureYieldChange ? m_ppaiFeatureYieldChange[i][j] : -1;
}

int CvBeliefEntry::GetYieldPerXTerrainTimes100(int i, int j) const
{
	PRECONDITION(i < GC.getNumTerrainInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	PRECONDITION(j < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(j > -1, "Index out of bounds");
	return m_ppiYieldPerXTerrain ? m_ppiYieldPerXTerrain[i][j] : 0;
}
int CvBeliefEntry::GetYieldPerXFeatureTimes100(int i, int j) const
{
	PRECONDITION(i < GC.getNumFeatureInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	PRECONDITION(j < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(j > -1, "Index out of bounds");
	return m_ppiYieldPerXFeature ? m_ppiYieldPerXFeature[i][j] : 0;
}
int CvBeliefEntry::GetCityYieldFromUnimprovedFeature(int i, int j) const
{
	PRECONDITION(i < GC.getNumFeatureInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	PRECONDITION(j < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(j > -1, "Index out of bounds");
	return m_ppiCityYieldFromUnimprovedFeature ? m_ppiCityYieldFromUnimprovedFeature[i][j] : 0;
}

int CvBeliefEntry::GetUnimprovedFeatureYieldChange(int i, int j) const
{
	PRECONDITION(i < GC.getNumFeatureInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	PRECONDITION(j < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(j > -1, "Index out of bounds");
	return m_ppiUnimprovedFeatureYieldChanges ? m_ppiUnimprovedFeatureYieldChanges[i][j] : 0;
}
int CvBeliefEntry::GetLakePlotYieldChange(int i) const
{
	return m_paiLakePlotYieldChange ? m_paiLakePlotYieldChange[i] : 0;
}

/// Change to Resource yield by type
int CvBeliefEntry::GetResourceYieldChange(int i, int j) const
{
	PRECONDITION(i < GC.getNumResourceInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	PRECONDITION(j < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(j > -1, "Index out of bounds");
	return m_ppaiResourceYieldChange ? m_ppaiResourceYieldChange[i][j] : -1;
}

/// Change to yield by terrain
int CvBeliefEntry::GetTerrainYieldChange(int i, int j) const
{
	PRECONDITION(i < GC.getNumTerrainInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	PRECONDITION(j < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(j > -1, "Index out of bounds");
	return m_ppaiTerrainYieldChange ? m_ppaiTerrainYieldChange[i][j] : -1;
}

int CvBeliefEntry::GetTradeRouteYieldChange(int i, int j) const
{
	PRECONDITION(i < NUM_DOMAIN_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	PRECONDITION(j < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(j > -1, "Index out of bounds");
	return m_ppiTradeRouteYieldChange ? m_ppiTradeRouteYieldChange[i][j] : 0;
}

int CvBeliefEntry::GetSpecialistYieldChange(int i, int j) const
{
	PRECONDITION(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	PRECONDITION(j < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(j > -1, "Index out of bounds");
	return m_ppiSpecialistYieldChange ? m_ppiSpecialistYieldChange[i][j] : 0;
}

int CvBeliefEntry::GetGreatPersonExpendedYield(int i, int j) const
{
	PRECONDITION(i < GC.getNumGreatPersonInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	PRECONDITION(j < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(j > -1, "Index out of bounds");
	return m_ppiGreatPersonExpendedYield ? m_ppiGreatPersonExpendedYield[i][j] : 0;
}

int CvBeliefEntry::GetGoldenAgeGreatPersonRateModifier(int i) const
{
	PRECONDITION(i < GC.getNumGreatPersonInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piGoldenAgeGreatPersonRateModifier ? m_piGoldenAgeGreatPersonRateModifier[i] : 0;
}

int CvBeliefEntry::GetCapitalYieldChange(int i) const
{
	return m_piCapitalYieldChange ? m_piCapitalYieldChange[i] : 0;
}

int CvBeliefEntry::GetCoastalCityYieldChange(int i) const
{
	return m_piCoastalCityYieldChange ? m_piCoastalCityYieldChange[i] : 0;
}

int CvBeliefEntry::GetGreatWorkYieldChange(int i) const
{
	return m_piGreatWorkYieldChange ? m_piGreatWorkYieldChange[i] : 0;
}

/// Do we get one of our yields from defeating an enemy?
int CvBeliefEntry::GetYieldFromKills(YieldTypes eYield) const
{
	PRECONDITION((int)eYield < NUM_YIELD_TYPES, "Yield type out of bounds");
	PRECONDITION((int)eYield > -1, "Index out of bounds");
	return m_piYieldFromKills[(int)eYield];
}

int CvBeliefEntry::GetYieldFromRemoveHeresy(YieldTypes eYield) const
{
	PRECONDITION((int)eYield < NUM_YIELD_TYPES, "Yield type out of bounds");
	PRECONDITION((int)eYield > -1, "Index out of bounds");
	return m_piYieldFromRemoveHeresy[(int)eYield];
}

/// Do we get one of our yields from defeating a barbarian?
int CvBeliefEntry::GetYieldFromBarbarianKills(YieldTypes eYield) const
{
	PRECONDITION((int)eYield < NUM_YIELD_TYPES, "Yield type out of bounds");
	PRECONDITION((int)eYield > -1, "Index out of bounds");
	return m_piYieldFromBarbarianKills[(int)eYield];
}

int CvBeliefEntry::GetGreatPersonPoints(GreatPersonTypes i) const
{
	PRECONDITION(i < GC.getNumGreatPersonInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piGreatPersonPoints ? m_piGreatPersonPoints[i] : 0;
}

/// Change to yield by plot
int CvBeliefEntry::GetPlotYieldChange(int i, int j) const
{
	PRECONDITION(i < GC.getNumPlotInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	PRECONDITION(j < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(j > -1, "Index out of bounds");
	return m_ppiPlotYieldChange ? m_ppiPlotYieldChange[i][j] : -1;
}

/// Free promotions 
std::vector<int> CvBeliefEntry::GetFreePromotions() const
{
	return m_aiFreePromotions;
}

/// Instant yield when finishing an improvement build (roads are not improvements)
int CvBeliefEntry::GetYieldFromImprovementBuild(YieldTypes eYield, bool bEraScaling) const
{
	PRECONDITION(eYield < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eYield > -1, "Index out of bounds");

	std::map<int, std::map<bool, int>>::const_iterator itYield = m_pbiYieldFromImprovementBuild.find((int)eYield);
	if (itYield != m_pbiYieldFromImprovementBuild.end())
	{
		std::map<bool, int>::const_iterator itBool = itYield->second.find(bEraScaling);
		if (itBool != itYield->second.end())
		{
			return itBool->second;
		}
	}
	return 0;
}

/// Instant yield when pillaging
int CvBeliefEntry::GetYieldFromPillageGlobal(YieldTypes eYield, bool bEraScaling) const
{
	PRECONDITION(eYield < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(eYield > -1, "Index out of bounds");

	std::map<int, std::map<bool, int>>::const_iterator itYield = m_pbiYieldFromPillageGlobal.find((int)eYield);
	if (itYield != m_pbiYieldFromPillageGlobal.end())
	{
		std::map<bool, int>::const_iterator itBool = itYield->second.find(bEraScaling);
		if (itBool != itYield->second.end())
		{
			return itBool->second;
		}
	}
	return 0;
}

/// Happiness from a resource
int CvBeliefEntry::GetResourceHappiness(int i) const
{
	PRECONDITION(i < GC.getNumResourceInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piResourceHappiness ? m_piResourceHappiness[i] : -1;
}

/// Yield boost from having a specialist of any type in city
int CvBeliefEntry::GetYieldChangeAnySpecialist(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldChangeAnySpecialist ? m_piYieldChangeAnySpecialist[i] : -1;
}

/// Yield boost from a trade route
int CvBeliefEntry::GetYieldChangeTradeRoute(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldChangeTradeRoute ? m_piYieldChangeTradeRoute[i] : -1;
}

/// Yield boost from a natural wonder
int CvBeliefEntry::GetYieldChangeNaturalWonder(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldChangeNaturalWonder ? m_piYieldChangeNaturalWonder[i] : -1;
}

/// Yield boost from a world wonder
int CvBeliefEntry::GetYieldChangeWorldWonder(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldChangeWorldWonder ? m_piYieldChangeWorldWonder[i] : -1;
}

/// Yield percentage boost from a natural wonder
int CvBeliefEntry::GetYieldModifierNaturalWonder(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piYieldModifierNaturalWonder ? m_piYieldModifierNaturalWonder[i] : -1;
}

/// Do we get a yield modifier 
int CvBeliefEntry::GetMaxYieldModifierPerFollower(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piMaxYieldModifierPerFollower ? m_piMaxYieldModifierPerFollower[i] : -1;
}
int CvBeliefEntry::GetMaxYieldModifierPerFollowerPercent(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_piMaxYieldModifierPerFollowerPercent ? m_piMaxYieldModifierPerFollowerPercent[i] : -1;
}

/// Can we buy units of this era with faith?
bool CvBeliefEntry::IsFaithUnitPurchaseEra(int i) const
{
	PRECONDITION(i > -1, "Index out of bounds");
	PRECONDITION(i < GC.getNumEraInfos(), "Index out of bounds");
	return m_pbFaithPurchaseUnitEraEnabled ? m_pbFaithPurchaseUnitEraEnabled[i] : false;
}
/// Can we buy units of this type with faith?
bool CvBeliefEntry::IsFaithUnitPurchaseSpecific(int i) const
{
	PRECONDITION(i > -1, "Index out of bounds");
	PRECONDITION(i < GC.getNumUnitInfos(), "Index out of bounds");
	return m_pbFaithPurchaseUnitSpecificEnabled ? m_pbFaithPurchaseUnitSpecificEnabled[i] : false;
}
/// Can we buy units of this era with faith?
bool CvBeliefEntry::IsBuildingClassEnabled(int i) const
{
	PRECONDITION(i > -1, "Index out of bounds");
	PRECONDITION(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	return m_pbBuildingClassEnabled ? m_pbBuildingClassEnabled[i] : false;
}

/// Load XML data
bool CvBeliefEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	//Basic Properties
	setShortDescription(kResults.GetText("ShortDescription"));
	setTooltip(kResults.GetText("Tooltip"));

	m_iMinPopulation                  = kResults.GetInt("MinPopulation");
	m_iMinFollowers                   = kResults.GetInt("MinFollowers");
	m_iMaxDistance					  = kResults.GetInt("MaxDistance");
	m_iCityGrowthModifier		      = kResults.GetInt("CityGrowthModifier");
	m_iFaithFromKills				  = kResults.GetInt("FaithFromKills");
	m_iFaithFromDyingUnits			  = kResults.GetInt("FaithFromDyingUnits");
	m_iRiverHappiness				  = kResults.GetInt("RiverHappiness");
	m_iHappinessPerCity				  = kResults.GetInt("HappinessPerCity");
	m_iHappinessPerXPeacefulForeignFollowers  = kResults.GetInt("HappinessPerXPeacefulForeignFollowers");
	m_iBorderGrowthRateIncreaseGlobal = kResults.GetInt("BorderGrowthRateIncreaseGlobal");
	m_iPlotCultureCostModifier	      = kResults.GetInt("PlotCultureCostModifier");
	m_iCityRangeStrikeModifier	      = kResults.GetInt("CityRangeStrikeModifier");
	m_iCombatModifierEnemyCities      = kResults.GetInt("CombatModifierEnemyCities");
	m_iCombatModifierFriendlyCities   = kResults.GetInt("CombatModifierFriendlyCities");
	m_iFriendlyHealChange	          = kResults.GetInt("FriendlyHealChange");
	m_iCityStateFriendshipModifier    = kResults.GetInt("CityStateFriendshipModifier");
	m_iLandBarbarianConversionPercent = kResults.GetInt("LandBarbarianConversionPercent");
	m_iWonderProductionModifier       = kResults.GetInt("WonderProductionModifier");
	m_iUnitProductionModifier		  = kResults.GetInt("UnitProductionModifier");
	m_iPlayerHappiness			      = kResults.GetInt("PlayerHappiness");
	m_iPlayerCultureModifier          = kResults.GetInt("PlayerCultureModifier");
	m_fHappinessPerFollowingCity      = kResults.GetFloat("HappinessPerFollowingCity");
	m_iGoldPerFollowingCity           = kResults.GetInt("GoldPerFollowingCity");
	m_iGoldPerXFollowers              = kResults.GetInt("GoldPerXFollowers");
	m_iGoldWhenCityAdopts             = kResults.GetInt("GoldPerFirstCityConversion");
	m_iSciencePerOtherReligionFollower= kResults.GetInt("SciencePerOtherReligionFollower");
	m_iSpreadDistanceModifier         = kResults.GetInt("SpreadDistanceModifier");
	m_iSpreadStrengthModifier		  = kResults.GetInt("SpreadStrengthModifier");
	m_iProphetStrengthModifier        = kResults.GetInt("ProphetStrengthModifier");
	m_iProphetCostModifier            = kResults.GetInt("ProphetCostModifier");
	m_iMissionaryStrengthModifier     = kResults.GetInt("MissionaryStrengthModifier");
	m_iMissionaryCostModifier         = kResults.GetInt("MissionaryCostModifier");
	m_iInquisitorCostModifier		  = kResults.GetInt("InquisitorCostModifier");
	m_iFriendlyCityStateSpreadModifier= kResults.GetInt("FriendlyCityStateSpreadModifier");
	m_iGreatPersonExpendedFaith       = kResults.GetInt("GreatPersonExpendedFaith");
	m_iCityStateMinimumInfluence      = kResults.GetInt("CityStateMinimumInfluence");
	m_iCityStateInfluenceModifier     = kResults.GetInt("CityStateInfluenceModifier");
	m_iOtherReligionPressureErosion   = kResults.GetInt("OtherReligionPressureErosion");
	m_iSpyPressure					  = kResults.GetInt("SpyPressure");
	m_iInquisitorPressureRetention    = kResults.GetInt("InquisitorPressureRetention");
	m_iFaithBuildingTourism           = kResults.GetInt("FaithBuildingTourism");
	m_iFullyConvertedHappiness		  = kResults.GetInt("FullyConvertedHappiness");

	m_bPantheon						  = kResults.GetBool("Pantheon");
	m_bFounder						  = kResults.GetBool("Founder");
	m_bFollower						  = kResults.GetBool("Follower");
	m_bEnhancer						  = kResults.GetBool("Enhancer");
	m_bReformer						  = kResults.GetBool("Reformation");
	m_bRequiresPeace				  = kResults.GetBool("RequiresPeace");
	m_bConvertsBarbarians			  = kResults.GetBool("ConvertsBarbarians");
	m_bFaithPurchaseAllGreatPeople	  = kResults.GetBool("FaithPurchaseAllGreatPeople");

	m_bRequiresImprovement			  = kResults.GetBool("RequiresImprovement");
	m_bRequiresResource				  = kResults.GetBool("RequiresResource");
	m_bRequiresNoImprovement		  = kResults.GetBool("RequiresNoImprovement");
	m_bRequiresNoFeature			  = kResults.GetBool("RequiresNoImprovementFeature");

	m_iHappinessFromForeignSpies = kResults.GetInt("HappinessFromForeignSpies");
	m_iGetPressureChangeTradeRoute = kResults.GetInt("PressureChangeTradeRoute");
	m_iCombatVersusOtherReligionOwnLands = kResults.GetInt("CombatVersusOtherReligionOwnLands");
	m_iCombatVersusOtherReligionTheirLands = kResults.GetInt("CombatVersusOtherReligionTheirLands");
	m_iMissionaryInfluenceCS = kResults.GetInt("MissionaryInfluenceCS");
	m_iHappinessPerPantheon = kResults.GetInt("HappinessPerPantheon");
	m_iExtraVotes = kResults.GetInt("ExtraVotes");
	m_iCityScalerLimiter = kResults.GetInt("CityScalerLimiter");
	m_iFollowerScalerLimiter = kResults.GetInt("FollowerScalerLimiter");
	m_iPolicyReductionWonderXFollowerCities = kResults.GetInt("PolicyReductionWonderXFollowerCities");

	m_bAIGoodStartingPantheon = kResults.GetBool("AI_GoodStartingPantheon");
	const char* szCivilizationType = kResults.GetText("CivilizationType");
	m_eRequiredCivilization = (CivilizationTypes)GC.getInfoTypeForString(szCivilizationType, true);
	//References
	const char* szTextVal = NULL;
	szTextVal						  = kResults.GetText("ObsoleteEra");
	m_eObsoleteEra					  = (EraTypes)GC.getInfoTypeForString(szTextVal, true);
	szTextVal						  = kResults.GetText("ResourceRevealed");
	m_eResourceRevealed				  = (ResourceTypes)GC.getInfoTypeForString(szTextVal, true);
	szTextVal						  = kResults.GetText("SpreadModifierDoublingTech");
	m_eSpreadModifierDoublingTech     = (TechTypes)GC.getInfoTypeForString(szTextVal, true);

	//Arrays
	const char* szBeliefType = GetType();
	kUtility.SetYields(m_paiCityYieldChange, "Belief_CityYieldChanges", "BeliefType", szBeliefType);
	kUtility.SetYields(m_paiHolyCityYieldChange, "Belief_HolyCityYieldChanges", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldChangeAnySpecialist, "Belief_YieldChangeAnySpecialist", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldChangeTradeRoute, "Belief_YieldChangeTradeRoute", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldChangeNaturalWonder, "Belief_YieldChangeNaturalWonder", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldChangeWorldWonder, "Belief_YieldChangeWorldWonder", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldModifierNaturalWonder, "Belief_YieldModifierNaturalWonder", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldPerActiveTR, "Belief_YieldPerActiveTR", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldPerConstruction, "Belief_YieldPerConstruction", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldPerWorldWonderConstruction, "Belief_YieldPerWorldWonderConstruction", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldPerPop, "Belief_YieldPerPop", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldPerGPT, "Belief_YieldPerGPT", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldPerLux, "Belief_YieldPerLux", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldPerHeal, "Belief_YieldPerHeal", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldPerBirth, "Belief_YieldPerBirth", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldPerHolyCityBirth, "Belief_YieldPerHolyCityBirth", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldPerScience, "Belief_YieldPerScience", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldFromGPUse, "Belief_YieldFromGPUse", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldBonusGoldenAge, "Belief_YieldBonusGoldenAge", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldFromSpread, "Belief_YieldFromSpread", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldFromForeignSpread, "Belief_YieldFromForeignSpread", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldFromConquest, "Belief_YieldFromConquest", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldFromPolicyUnlock, "Belief_YieldFromPolicyUnlock", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldFromEraUnlock, "Belief_YieldFromEraUnlock", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldFromConversion, "Belief_YieldFromConversion", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldFromConversionExpo, "Belief_YieldFromConversionExpo", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldFromWLTKD, "Belief_YieldFromWLTKD", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldFromProposal, "Belief_YieldFromProposal", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldFromHost, "Belief_YieldFromHost", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldFromFaithPurchase, "Belief_YieldFromFaithPurchase", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldFromKnownPantheons, "Belief_YieldFromKnownPantheons", "BeliefType", szBeliefType);
	kUtility.PopulateArrayByValue(m_piMaxYieldPerFollower, "Yields", "Belief_MaxYieldPerFollower", "YieldType", "BeliefType", szBeliefType, "Max");
	kUtility.PopulateArrayByValue(m_piMaxYieldPerFollowerPercent, "Yields", "Belief_MaxYieldPerFollowerPercent", "YieldType", "BeliefType", szBeliefType, "Max");

	kUtility.PopulateArrayByValue(m_piImprovementVoteChange, "Improvements", "Belief_VotePerXImprovementOwned", "ImprovementType", "BeliefType", szBeliefType, "Amount");

	m_iReducePolicyRequirements = kResults.GetInt("ReducePolicyRequirements");
	m_iCSYieldBonus = kResults.GetInt("CSYieldBonusFromSharedReligion");

	kUtility.PopulateArrayByValue(m_piMaxYieldModifierPerFollower, "Yields", "Belief_MaxYieldModifierPerFollower", "YieldType", "BeliefType", szBeliefType, "Max");
	kUtility.PopulateArrayByValue(m_piMaxYieldModifierPerFollowerPercent, "Yields", "Belief_MaxYieldModifierPerFollowerPercent", "YieldType", "BeliefType", szBeliefType, "Max");
	kUtility.PopulateArrayByValue(m_piResourceHappiness, "Resources", "Belief_ResourceHappiness", "ResourceType", "BeliefType", szBeliefType, "HappinessChange");
	kUtility.PopulateArrayByValue(m_piResourceQuantityModifiers, "Resources", "Belief_ResourceQuantityModifiers", "ResourceType", "BeliefType", szBeliefType, "ResourceQuantityModifier");
	kUtility.PopulateArrayByValue(m_paiBuildingClassHappiness, "BuildingClasses", "Belief_BuildingClassHappiness", "BuildingClassType", "BeliefType", szBeliefType, "Happiness");
	kUtility.PopulateArrayByValue(m_paiYieldChangePerForeignCity, "Yields", "Belief_YieldChangePerForeignCity", "YieldType", "BeliefType", szBeliefType, "Yield");
	kUtility.PopulateArrayByValue(m_paiYieldChangePerXForeignFollowers, "Yields", "Belief_YieldChangePerXForeignFollowers", "YieldType", "BeliefType", szBeliefType, "ForeignFollowers");
	kUtility.PopulateArrayByValue(m_paiYieldChangePerXCityStateFollowers, "Yields", "Belief_YieldChangePerXCityStateFollowers", "YieldType", "BeliefType", szBeliefType, "PerXFollowers");
	kUtility.PopulateArrayByValue(m_piYieldPerFollowingCity, "Yields", "Belief_YieldPerFollowingCity", "YieldType", "BeliefType", szBeliefType, "Yield");
	kUtility.PopulateArrayByValue(m_piYieldPerXFollowers, "Yields", "Belief_YieldPerXFollowers", "YieldType", "BeliefType", szBeliefType, "PerXFollowers");
	kUtility.PopulateArrayByValue(m_piYieldPerOtherReligionFollower, "Yields", "Belief_YieldPerOtherReligionFollower", "YieldType", "BeliefType", szBeliefType, "Yield");
	kUtility.PopulateArrayByValue(m_paiLakePlotYieldChange, "Yields", "Belief_LakePlotYield", "YieldType", "BeliefType", szBeliefType, "Yield");	
	kUtility.PopulateArrayByExistence(m_pbFaithPurchaseUnitSpecificEnabled, "Units", "Belief_SpecificFaithUnitPurchase", "UnitType", "BeliefType", szBeliefType);
	kUtility.PopulateArrayByExistence(m_pbFaithPurchaseUnitEraEnabled, "Eras", "Belief_EraFaithUnitPurchase", "EraType", "BeliefType", szBeliefType);
	kUtility.PopulateArrayByExistence(m_pbBuildingClassEnabled, "BuildingClasses", "Belief_BuildingClassFaithPurchase", "BuildingClassType", "BeliefType", szBeliefType);

	//YieldPerBorderGrowth
	{
		std::string strKey("Belief_YieldPerBorderGrowth");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Yields.ID as YieldID, Yield, IsEraScaling from Belief_YieldPerBorderGrowth inner join Yields on Yields.Type = YieldType where BeliefType = ?");
		}

		pResults->Bind(1, szBeliefType);

		while (pResults->Step())
		{
			const int YieldID = pResults->GetInt(0);
			const int yield = pResults->GetInt(1);
			const bool eraScaling = pResults->GetBool(2);

			m_pbiYieldPerBorderGrowth[YieldID][eraScaling] = yield;
		}

		//Trim extra memory off container since this is mostly read-only.
		std::map<int, std::map<bool, int>>(m_pbiYieldPerBorderGrowth).swap(m_pbiYieldPerBorderGrowth);
	}

	//YieldFromTechUnlock
	{
		std::string strKey("Belief_YieldFromTechUnlock");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Yields.ID as YieldID, Yield, IsEraScaling from Belief_YieldFromTechUnlock inner join Yields on Yields.Type = YieldType where BeliefType = ?");
		}

		pResults->Bind(1, szBeliefType);

		while (pResults->Step())
		{
			const int YieldID = pResults->GetInt(0);
			const int yield = pResults->GetInt(1);
			const bool eraScaling = pResults->GetBool(2);

			m_pbiYieldFromTechUnlock[YieldID][eraScaling] = yield;
		}

		//Trim extra memory off container since this is mostly read-only.
		std::map<int, std::map<bool, int>>(m_pbiYieldFromTechUnlock).swap(m_pbiYieldFromTechUnlock);
	}

	//ImprovementYieldChanges
	{
		kUtility.Initialize2DArray(m_ppiImprovementYieldChanges, "Improvements", "Yields");

		std::string strKey("Belief_ImprovementYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Improvements.ID as ImprovementID, Yields.ID as YieldID, Yield from Belief_ImprovementYieldChanges inner join Improvements on Improvements.Type = ImprovementType inner join Yields on Yields.Type = YieldType where BeliefType = ?");
		}

		pResults->Bind(1, szBeliefType);

		while(pResults->Step())
		{
			const int ImprovementID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiImprovementYieldChanges[ImprovementID][YieldID] = yield;
		}
	}

	//BuildingClassYieldChanges
	{
		kUtility.Initialize2DArray(m_ppiBuildingClassYieldChanges, "BuildingClasses", "Yields");

		std::string strKey("Belief_BuildingClassYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select BuildingClasses.ID as BuildingClassID, Yields.ID as YieldID, YieldChange from Belief_BuildingClassYieldChanges inner join BuildingClasses on BuildingClasses.Type = BuildingClassType inner join Yields on Yields.Type = YieldType where BeliefType = ?");
		}

		pResults->Bind(1, szBeliefType);

		while(pResults->Step())
		{
			const int BuildingClassID = pResults->GetInt(0);
			const int iYieldID = pResults->GetInt(1);
			const int iYieldChange = pResults->GetInt(2);

			m_ppiBuildingClassYieldChanges[BuildingClassID][iYieldID] = iYieldChange;
		}

		std::string strKey2("Belief_BuildingClassTourism");
		pResults = kUtility.GetResults(strKey2);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey2, "select BuildingClasses.ID as BuildingClassID, Tourism from Belief_BuildingClassTourism inner join BuildingClasses on BuildingClasses.Type = BuildingClassType where BeliefType = ?");
		}

		pResults->Bind(1, szBeliefType);

		while (pResults->Step())
		{
			const int BuildingClassID = pResults->GetInt(0);
			const int iYieldID = (int)YIELD_TOURISM;
			const int iYieldChange = pResults->GetInt(1);

			m_ppiBuildingClassYieldChanges[BuildingClassID][iYieldID] += iYieldChange;
		}
	}

	//FeatureYieldChanges
	{
		kUtility.Initialize2DArray(m_ppaiFeatureYieldChange, "Features", "Yields");

		std::string strKey("Belief_FeatureYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Features.ID as FeatureID, Yields.ID as YieldID, Yield from Belief_FeatureYieldChanges inner join Features on Features.Type = FeatureType inner join Yields on Yields.Type = YieldType where BeliefType = ?");
		}

		pResults->Bind(1, szBeliefType);

		while(pResults->Step())
		{
			const int FeatureID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppaiFeatureYieldChange[FeatureID][YieldID] = yield;
		}
	}

	//CityYieldPerXTerrain
	{
		kUtility.Initialize2DArray(m_ppiYieldPerXTerrain, "Terrains", "Yields");

		std::string strKey("Belief_CityYieldPerXTerrainTimes100");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Terrains.ID as TerrainID, Yields.ID as YieldID, Yield from Belief_CityYieldPerXTerrainTimes100 inner join Terrains on Terrains.Type = TerrainType inner join Yields on Yields.Type = YieldType where BeliefType = ?");
		}

		pResults->Bind(1, szBeliefType);

		while(pResults->Step())
		{
			const int TerrainID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiYieldPerXTerrain[TerrainID][YieldID] = yield;
		}
	}
	//CityYieldPerXFeature
	{
		kUtility.Initialize2DArray(m_ppiYieldPerXFeature, "Features", "Yields");

		std::string strKey("Belief_CityYieldPerXFeatureTimes100");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Features.ID as FeatureID, Yields.ID as YieldID, Yield from Belief_CityYieldPerXFeatureTimes100 inner join Features on Features.Type = FeatureType inner join Yields on Yields.Type = YieldType where BeliefType = ?");
		}

		pResults->Bind(1, szBeliefType);

		while(pResults->Step())
		{
			const int FeatureID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiYieldPerXFeature[FeatureID][YieldID] = yield;
		}
	}
	//CityYieldFromUnimprovedFeature
	{
		kUtility.Initialize2DArray(m_ppiCityYieldFromUnimprovedFeature, "Features", "Yields");

		std::string strKey("Belief_CityYieldFromUnimprovedFeature");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Features.ID as FeatureID, Yields.ID as YieldID, Yield from Belief_CityYieldFromUnimprovedFeature inner join Features on Features.Type = FeatureType inner join Yields on Yields.Type = YieldType where BeliefType = ?");
		}

		pResults->Bind(1, szBeliefType);

		while(pResults->Step())
		{
			const int FeatureID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiCityYieldFromUnimprovedFeature[FeatureID][YieldID] = yield;
		}
	}

	//UnimprovedFeatureYieldChanges
	{
		kUtility.Initialize2DArray(m_ppiUnimprovedFeatureYieldChanges, "Features", "Yields");

		std::string strKey("Belief_UnimprovedFeatureYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Features.ID as FeatureID, Yields.ID as YieldID, Yield from Belief_UnimprovedFeatureYieldChanges inner join Features on Features.Type = FeatureType inner join Yields on Yields.Type = YieldType where BeliefType = ?");
		}

		pResults->Bind(1, szBeliefType);

		while(pResults->Step())
		{
			const int FeatureID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiUnimprovedFeatureYieldChanges[FeatureID][YieldID] = yield;
		}
	}

	//ResourceYieldChanges
	{
		kUtility.Initialize2DArray(m_ppaiResourceYieldChange, "Resources", "Yields");

		std::string strKey("Belief_ResourceYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Resources.ID as ResourceID, Yields.ID as YieldID, Yield from Belief_ResourceYieldChanges inner join Resources on Resources.Type = ResourceType inner join Yields on Yields.Type = YieldType where BeliefType = ?");
		}

		pResults->Bind(1, szBeliefType);

		while(pResults->Step())
		{
			const int ResourceID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppaiResourceYieldChange[ResourceID][YieldID] = yield;
		}
	}

	//TerrainYieldChanges
	{
		kUtility.Initialize2DArray(m_ppaiTerrainYieldChange, "Terrains", "Yields");

		std::string strKey("Belief_TerrainYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Terrains.ID as TerrainID, Yields.ID as YieldID, Yield from Belief_TerrainYieldChanges inner join Terrains on Terrains.Type = TerrainType inner join Yields on Yields.Type = YieldType where BeliefType = ?");
		}

		pResults->Bind(1, szBeliefType);

		while(pResults->Step())
		{
			const int TerrainID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppaiTerrainYieldChange[TerrainID][YieldID] = yield;
		}
	}

	//TradeRouteYieldChange
	{
		kUtility.Initialize2DArray(m_ppiTradeRouteYieldChange, "Domains", "Yields");

		std::string strKey("Belief_TradeRouteYieldChange");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Domains.ID as DomainID, Yields.ID as YieldID, Yield from Belief_TradeRouteYieldChange inner join Domains on Domains.Type = DomainType inner join Yields on Yields.Type = YieldType where BeliefType = ?");
		}

		pResults->Bind(1, szBeliefType);

		while(pResults->Step())
		{
			const int DomainID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiTradeRouteYieldChange[DomainID][YieldID] = yield;
		}
	}
	
	//SpecialistYieldChanges
	{
		kUtility.Initialize2DArray(m_ppiSpecialistYieldChange, "Specialists", "Yields");

		std::string strKey("Belief_SpecialistYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Specialists.ID as SpecialistID, Yields.ID as YieldID, Yield from Belief_SpecialistYieldChanges inner join Specialists on Specialists.Type = SpecialistType inner join Yields on Yields.Type = YieldType where BeliefType = ?");
		}

		pResults->Bind(1, szBeliefType);

		while(pResults->Step())
		{
			const int SpecialistID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiSpecialistYieldChange[SpecialistID][YieldID] = yield;
		}
	}
	
	//GreatPersonExpendedYield
	{
		kUtility.Initialize2DArray(m_ppiGreatPersonExpendedYield, "GreatPersons", "Yields");

		std::string strKey("Belief_GreatPersonExpendedYield");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select GreatPersons.ID as GreatPersonID, Yields.ID as YieldID, Yield from Belief_GreatPersonExpendedYield inner join GreatPersons on GreatPersons.Type = GreatPersonType inner join Yields on Yields.Type = YieldType where BeliefType = ?");
		}

		pResults->Bind(1, szBeliefType);

		while(pResults->Step())
		{
			const int GreatPersonID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiGreatPersonExpendedYield[GreatPersonID][YieldID] = yield;
		}
	}
	
	kUtility.PopulateArrayByValue(m_piGoldenAgeGreatPersonRateModifier, "GreatPersons", "Belief_GoldenAgeGreatPersonRateModifier", "GreatPersonType", "BeliefType", szBeliefType, "Modifier");
	kUtility.PopulateArrayByValue(m_piGreatPersonPoints, "GreatPersons", "Belief_GreatPersonPoints", "GreatPersonType", "BeliefType", szBeliefType, "Value");
	kUtility.SetYields(m_piCapitalYieldChange, "Belief_CapitalYieldChanges", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piCoastalCityYieldChange, "Belief_CoastalCityYieldChanges", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piGreatWorkYieldChange, "Belief_GreatWorkYieldChanges", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldFromKills, "Belief_YieldFromKills", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldFromRemoveHeresy, "Belief_YieldFromRemoveHeresy", "BeliefType", szBeliefType);
	kUtility.SetYields(m_piYieldFromBarbarianKills, "Belief_YieldFromBarbarianKills", "BeliefType", szBeliefType);

	//PlotYieldChanges
	kUtility.Initialize2DArray(m_ppiPlotYieldChange, "Plots", "Yields");

	std::string strKey("Belief_PlotYieldChanges");
	Database::Results* pResults = kUtility.GetResults(strKey);
	if(pResults == NULL)
	{
		pResults = kUtility.PrepareResults(strKey, "select Plots.ID as PlotID, Yields.ID as YieldID, Yield from Belief_PlotYieldChanges inner join Plots on Plots.Type = PlotType inner join Yields on Yields.Type = YieldType where BeliefType = ?");
	}

	pResults->Bind(1, szBeliefType);

	while(pResults->Step())
	{
		const int PlotID = pResults->GetInt(0);
		const int YieldID = pResults->GetInt(1);
		const int yield = pResults->GetInt(2);

		m_ppiPlotYieldChange[PlotID][YieldID] = yield;
	}

	if (MOD_RELIGION_EXTENSIONS)
	{
		//FreePromotions
		{
			std::string strKey("Belief_FreePromotions");
			Database::Results* pResults = kUtility.GetResults(strKey);
			if (pResults == NULL)
			{
				pResults = kUtility.PrepareResults(strKey, "select UnitPromotions.ID as UnitPromotionsID from Belief_FreePromotions inner join UnitPromotions on UnitPromotions.Type = PromotionType where BeliefType = ?");
			}

			pResults->Bind(1, szBeliefType);

			while (pResults->Step())
			{
				const int PromotionID = pResults->GetInt(0);

				m_aiFreePromotions.push_back(PromotionID);
			}

			//Trim extra memory off container since this is mostly read-only.
			std::vector<int>(m_aiFreePromotions).swap(m_aiFreePromotions);
		}

		//YieldFromImprovementBuild
		{
			std::string strKey("Belief_YieldFromImprovementBuild");
			Database::Results* pResults = kUtility.GetResults(strKey);
			if (pResults == NULL)
			{
				pResults = kUtility.PrepareResults(strKey, "select Yields.ID as YieldID, Yield, IsEraScaling from Belief_YieldFromImprovementBuild inner join Yields on Yields.Type = YieldType where BeliefType = ?");
			}

			pResults->Bind(1, szBeliefType);

			while (pResults->Step())
			{
				const int YieldID = pResults->GetInt(0);
				const int yield = pResults->GetInt(1);
				const bool eraScaling = pResults->GetBool(2);

				m_pbiYieldFromImprovementBuild[YieldID][eraScaling] = yield;
			}

			//Trim extra memory off container since this is mostly read-only.
			std::map<int, std::map<bool, int>>(m_pbiYieldFromImprovementBuild).swap(m_pbiYieldFromImprovementBuild);
		}

		//YieldFromPillageGlobal
		{
			std::string strKey("Belief_YieldFromPillageGlobal");
			Database::Results* pResults = kUtility.GetResults(strKey);
			if (pResults == NULL)
			{
				pResults = kUtility.PrepareResults(strKey, "select Yields.ID as YieldID, Yield, IsEraScaling from Belief_YieldFromPillageGlobal inner join Yields on Yields.Type = YieldType where BeliefType = ?");
			}

			pResults->Bind(1, szBeliefType);

			while (pResults->Step())
			{
				const int YieldID = pResults->GetInt(0);
				const int yield = pResults->GetInt(1);
				const bool eraScaling = pResults->GetBool(2);

				m_pbiYieldFromPillageGlobal[YieldID][eraScaling] = yield;
			}

			//Trim extra memory off container since this is mostly read-only.
			std::map<int, std::map<bool, int>>(m_pbiYieldFromPillageGlobal).swap(m_pbiYieldFromPillageGlobal);
		}
	}

	return true;
}

//=====================================
// CvBeliefXMLEntries
//=====================================
/// Constructor
CvBeliefXMLEntries::CvBeliefXMLEntries(void)
{

}

/// Destructor
CvBeliefXMLEntries::~CvBeliefXMLEntries(void)
{
	DeleteArray();
}

/// Returns vector of belief entries
std::vector<CvBeliefEntry*>& CvBeliefXMLEntries::GetBeliefEntries()
{
	return m_paBeliefEntries;
}

/// Number of defined beliefs
int CvBeliefXMLEntries::GetNumBeliefs()
{
	return m_paBeliefEntries.size();
}

/// Clear belief entries
void CvBeliefXMLEntries::DeleteArray()
{
	for(std::vector<CvBeliefEntry*>::iterator it = m_paBeliefEntries.begin(); it != m_paBeliefEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paBeliefEntries.clear();
}

/// Get a specific entry
CvBeliefEntry* CvBeliefXMLEntries::GetEntry(int index)
{
	return (index != NO_BELIEF) ? m_paBeliefEntries[index] : NULL;
}

//=====================================
// CvReligionBeliefs
//=====================================
/// Constructor
CvReligionBeliefs::CvReligionBeliefs() :
	m_eReligion(NO_RELIGION),
	m_BeliefLookup(GC.GetGameBeliefs()->GetNumBeliefs(), 0),
	m_ReligionBeliefs()
{
	Reset();
}

/// Copy Constructor with typical parameters
CvReligionBeliefs::CvReligionBeliefs(const CvReligionBeliefs& source) :
	m_eReligion(source.m_eReligion),
	m_BeliefLookup(source.m_BeliefLookup),
	m_ReligionBeliefs(source.m_ReligionBeliefs)
{
	// No need to call Reset here since we're copying the state from source
}

/// Reset data members
void CvReligionBeliefs::Reset()
{
	m_ReligionBeliefs.clear();
	m_eReligion = NO_RELIGION;
	m_BeliefLookup = std::vector<int>(GC.GetGameBeliefs()->GetNumBeliefs(),0);
}

void CvReligionBeliefs::SetReligion(ReligionTypes eReligion)
{
	m_eReligion = eReligion;
}
ReligionTypes CvReligionBeliefs::GetReligion() const
{
	return m_eReligion;
}

/// Store off data on bonuses from beliefs
void CvReligionBeliefs::AddBelief(BeliefTypes eBelief, PlayerTypes ePlayer, bool bTriggerAccomplishment)
{
	PRECONDITION(eBelief != NO_BELIEF);
	CvBeliefEntry* belief = GC.GetGameBeliefs()->GetEntry(eBelief);
	ASSERT(belief);

	m_ReligionBeliefs.push_back(eBelief);
	m_BeliefLookup[eBelief] = 1;

	if (bTriggerAccomplishment)
	{
		if (belief->IsPantheonBelief())
			GET_PLAYER(ePlayer).CompleteAccomplishment(ACCOMPLISHMENT_BELIEF_PANTHEON);
		else if (belief->IsFounderBelief())
			GET_PLAYER(ePlayer).CompleteAccomplishment(ACCOMPLISHMENT_BELIEF_FOUNDER);
		else if (belief->IsFollowerBelief())
			GET_PLAYER(ePlayer).CompleteAccomplishment(ACCOMPLISHMENT_BELIEF_FOLLOWER);
		else if (belief->IsEnhancerBelief())
			GET_PLAYER(ePlayer).CompleteAccomplishment(ACCOMPLISHMENT_BELIEF_ENHANCER);
		else if (belief->IsReformationBelief())
			GET_PLAYER(ePlayer).CompleteAccomplishment(ACCOMPLISHMENT_BELIEF_REFORMATION);
	}
}

/// Does this religion possess a specific belief?
bool CvReligionBeliefs::HasBelief(BeliefTypes eBelief) const
{
	if (eBelief == NO_BELIEF)
		return m_ReligionBeliefs.empty();

	return (m_BeliefLookup[eBelief] == 1);
}

/// Does this religion possess a specific belief?
BeliefTypes CvReligionBeliefs::GetBelief(int iIndex) const
{
	if (iIndex>=0 && iIndex<(int)m_ReligionBeliefs.size())
		return (BeliefTypes)m_ReligionBeliefs[iIndex];

	return NO_BELIEF;
}

/// How many beliefs does this religion have?
int CvReligionBeliefs::GetNumBeliefs() const
{
	return m_ReligionBeliefs.size();
}

// Does the requested pantheon belief exist in the religion of the requested player?
bool CvReligionBeliefs::IsPantheonBeliefInReligion(BeliefTypes eBelief, ReligionTypes eReligion, PlayerTypes ePlayer) const
{
	ReligionTypes ePantheon = GC.getGame().GetGameReligions()->GetPantheonCreatedByPlayer(ePlayer);
	if (ePantheon == NO_RELIGION || eReligion == NO_RELIGION)
		return false;

	CvReligionBeliefs ePanthBeliefs = GC.getGame().GetGameReligions()->GetReligion(ePantheon, ePlayer)->m_Beliefs;
	CvReligionBeliefs eFounderBeliefs = GC.getGame().GetGameReligions()->GetReligion(eReligion, ePlayer)->m_Beliefs;

	return (ePanthBeliefs.HasBelief(eBelief) && eFounderBeliefs.HasBelief(eBelief));
}

bool CvReligionBeliefs::IsBeliefValid(BeliefTypes eBelief, ReligionTypes eReligion, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	if (ePlayer == NO_PLAYER)
		return true;

	bool bEligibleForFounderBenefits = false;
	const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);
	if (pReligion)
	{
		//do not look up the holy city itself, it's too expensive and we only need the plot
		CvPlot* pHolyCityPlot = GC.getMap().plot(pReligion->m_iHolyCityX, pReligion->m_iHolyCityY);
		if (pReligion->m_bPantheon && pHolyCityPlot == NULL)
		{
			if (pReligion->m_eFounder == ePlayer)
			{		
				bEligibleForFounderBenefits = true;
			}
		}
		else
		{
			//don't care about founder, ownership counts!
			if (pHolyCityPlot && pHolyCityPlot->getOwner() == ePlayer)
			{
				bEligibleForFounderBenefits = true;
			}
		}
	}

	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();

	if (eReligion != NO_RELIGION && pBeliefs->GetEntry(eBelief)->IsFounderBelief())
	{
		if (!bEligibleForFounderBenefits)
		{	
			return false;
		}
		if (pCity != NULL && pCity->getOwner() != ePlayer)
		{
			return false;
		}
		if (bHolyCityOnly)
		{
			if (pCity != NULL && !pCity->GetCityReligions()->IsHolyCityForReligion(eReligion))
			{
				return false;
			}
		}
	}
	if (eReligion != NO_RELIGION && pBeliefs->GetEntry(eBelief)->IsEnhancerBelief())
	{
		if (bHolyCityOnly)
		{
			if (pCity != NULL && !pCity->GetCityReligions()->IsHolyCityForReligion(eReligion))
			{
				return false;
			}
		}
		if (pCity != NULL && pCity->getOwner() != ePlayer)
		{
			return false;
		}
		if (!bEligibleForFounderBenefits)
		{
			return false;
		}
	}
	if (pBeliefs->GetEntry(eBelief)->IsFollowerBelief())
	{
		//If calling on a city for city-based beliefs, must be capital or the majority.
		if (pCity != NULL && pCity->GetCityReligions()->GetReligiousMajority() != eReligion)
		{
			return false;
		}
		if (pCity != NULL && pCity->getOwner() != ePlayer)
		{
			return false;
		}
	}
	if (pBeliefs->GetEntry(eBelief)->IsReformationBelief())
	{
		//If calling on a city for city-based beliefs, must be capital or the majority.
		if (pCity != NULL && pCity->GetCityReligions()->GetReligiousMajority() != eReligion)
		{
			return false;
		}
		if (!bEligibleForFounderBenefits && GET_PLAYER(ePlayer).GetReligions()->GetStateReligion() != eReligion)
		{
			return false;
		}
		if (bHolyCityOnly && pCity != NULL)
		{
			//For founder/controller
			if (bEligibleForFounderBenefits && !pCity->GetCityReligions()->IsHolyCityForReligion(eReligion))
			{
				return false;
			}
			//Everyone else gets it in their capital.
			if (!bEligibleForFounderBenefits && GET_PLAYER(ePlayer).getCapitalCity() != pCity)
			{
				return false;
			}
		}
	}
	
	if (pBeliefs->GetEntry(eBelief)->IsPantheonBelief())
	{
		if (eReligion == NO_RELIGION)
		{
			eReligion = RELIGION_PANTHEON;
		}
		if (!bEligibleForFounderBenefits)
		{
			//Capital only? Only if the majority for you.
			if (bHolyCityOnly && eReligion > RELIGION_PANTHEON && GET_PLAYER(ePlayer).GetReligions()->GetStateReligion() != eReligion)
			{
				return false;
			}
		}
		//If calling on a city for city-based beliefs, must be capital or the majority.
		if (pCity != NULL)
		{
			if (pCity->GetCityReligions()->GetReligiousMajority() != eReligion && !IsPantheonBeliefInReligion(eBelief, eReligion, ePlayer))
			{
				return false;
			}
			if (pCity->getOwner() != ePlayer)
			{
				return false;
			}
			if (bHolyCityOnly)
			{
				if (eReligion == RELIGION_PANTHEON)
				{
					if (GET_PLAYER(ePlayer).getCapitalCity() != pCity)
					{
						return false;
					}
					ReligionTypes eParentReligion = GET_PLAYER(ePlayer).GetReligions()->GetStateReligion(false);
					if (eParentReligion != NO_RELIGION && eReligion != eParentReligion)
					{
						eReligion = eParentReligion;
					}
				}
				if (eReligion > RELIGION_PANTHEON)
				{
					//For founder/controller
					if (bEligibleForFounderBenefits && !pCity->GetCityReligions()->IsHolyCityForReligion(eReligion))
					{
						return false;
					}
					//Everyone else gets it in their capital.
					if (!bEligibleForFounderBenefits && GET_PLAYER(ePlayer).getCapitalCity() != pCity)
					{
						return false;
					}
				}
			}
		}
	}
	if (MOD_BALANCE_UNIQUE_BELIEFS_ONLY_FOR_CIV && pBeliefs->GetEntry(eBelief)->GetRequiredCivilization() != NO_CIVILIZATION)
	{
		if (pBeliefs->GetEntry(eBelief)->GetRequiredCivilization() != GET_PLAYER(ePlayer).getCivilizationType())
			return false;
	}

	return true;
}

int CvReligionBeliefs::GetFaithFromDyingUnits(PlayerTypes ePlayer, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetFaithFromDyingUnits();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, NULL, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetRiverHappiness(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetRiverHappiness();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetBorderGrowthRateIncreaseGlobal(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetBorderGrowthRateIncreaseGlobal();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetPlotCultureCostModifier(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetPlotCultureCostModifier();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetCityRangeStrikeModifier(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetCityRangeStrikeModifier();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetCombatModifierEnemyCities(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetCombatModifierEnemyCities();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetCombatModifierFriendlyCities(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetCombatModifierFriendlyCities();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetFriendlyHealChange(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetFriendlyHealChange();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetCityStateFriendshipModifier(PlayerTypes ePlayer, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetCityStateFriendshipModifier();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, NULL, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetLandBarbarianConversionPercent(PlayerTypes ePlayer, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetLandBarbarianConversionPercent();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, NULL, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetSpreadDistanceModifier(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetSpreadDistanceModifier();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetSpreadStrengthModifier(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetSpreadStrengthModifier();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetProphetStrengthModifier(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetProphetStrengthModifier();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetProphetCostModifier(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetProphetCostModifier();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetMissionaryStrengthModifier(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetMissionaryStrengthModifier();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetMissionaryCostModifier(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetMissionaryCostModifier();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetInquisitorCostModifier(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetInquisitorCostModifier();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetFriendlyCityStateSpreadModifier(PlayerTypes ePlayer, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetFriendlyCityStateSpreadModifier();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, NULL, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetGreatPersonExpendedFaith(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetGreatPersonExpendedFaith();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetCityStateMinimumInfluence(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetCityStateMinimumInfluence();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetCityStateInfluenceModifier(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetCityStateInfluenceModifier();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetOtherReligionPressureErosion(PlayerTypes ePlayer, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetOtherReligionPressureErosion();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, NULL, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetSpyPressure(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetSpyPressure();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetInquisitorPressureRetention(PlayerTypes ePlayer, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetInquisitorPressureRetention();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, NULL, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetFaithBuildingTourism(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetFaithBuildingTourism();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetFullyConvertedHappiness(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetFullyConvertedHappiness();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetCombatVersusOtherReligionOwnLands(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetCombatVersusOtherReligionOwnLands();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetCombatVersusOtherReligionTheirLands(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetCombatVersusOtherReligionTheirLands();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetMissionaryInfluenceCS(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetMissionaryInfluenceCS();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetHappinessPerPantheon(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;


	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetHappinessPerPantheon();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetExtraVotes(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetExtraVotes();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetPolicyReductionWonderXFollowerCities(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetPolicyReductionWonderXFollowerCities();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetCityScalerLimiter(int iCap) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();

	int iBiggestValue = 0;
	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetCityScalerLimiter();
		if (iValue != 0)
		{
			if (iValue > iBiggestValue)
			{
				iBiggestValue = iValue;
			}
		}
	}
	if (iBiggestValue == 0)
		return iCap;

	return min(iCap, iBiggestValue);
}

int CvReligionBeliefs::GetFollowerScalerLimiter(int iCap) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();

	int iBiggestValue = 0;
	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetFollowerScalerLimiter();
		if (iValue > 0)
		{
			if (iValue > iBiggestValue)
			{
				iBiggestValue = iValue;
			}
		}
	}

	if (iBiggestValue == 0)
		return iCap;

	return min(iCap, iBiggestValue);
}

EraTypes CvReligionBeliefs::GetObsoleteEra(PlayerTypes ePlayer, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();

	EraTypes eEra = NO_ERA;
	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		EraTypes eTestEra = pBeliefs->GetEntry(*it)->GetObsoleteEra();
		if (eTestEra != NO_ERA && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, NULL, bHolyCityOnly))
		{
			eEra = eTestEra;
			break;
		}
	}

	return eEra;
}
ResourceTypes CvReligionBeliefs::GetResourceRevealed(PlayerTypes ePlayer, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	ResourceTypes eResource = NO_RESOURCE;
	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		ResourceTypes eTestResource = pBeliefs->GetEntry(*it)->GetResourceRevealed();
		if (eTestResource != NO_RESOURCE && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, NULL, bHolyCityOnly))
		{
			eResource = eTestResource;
			break;
		}
	}

	return eResource;
}
TechTypes CvReligionBeliefs::GetSpreadModifierDoublingTech(PlayerTypes ePlayer, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();

	TechTypes eTech = NO_TECH;
	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		TechTypes eTestTech = pBeliefs->GetEntry(*it)->GetSpreadModifierDoublingTech();
		if (eTestTech != NO_TECH && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, NULL, bHolyCityOnly))
		{
			eTech = eTestTech;
			break;
		}
	}

	return eTech;
}
/// Faith from kills
int CvReligionBeliefs::GetFaithFromKills(int iDistance, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;
	int iRequiredDistance = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = 0;
		iRequiredDistance = pBeliefs->GetEntry(*it)->GetMaxDistance();
		if (iRequiredDistance == 0 || iDistance <= iRequiredDistance)
		{
			iValue = pBeliefs->GetEntry(*it)->GetFaithFromKills();
		}
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Happiness per city
int CvReligionBeliefs::GetHappinessPerCity(int iPopulation, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = 0;
		if (iPopulation >= pBeliefs->GetEntry(*it)->GetMinPopulation())
		{
			iValue = pBeliefs->GetEntry(*it)->GetHappinessPerCity();
		}
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Happiness per X followers in foreign cities of powers you are not at war with
int CvReligionBeliefs::GetHappinessPerXPeacefulForeignFollowers(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetHappinessPerXPeacefulForeignFollowers();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			return iValue;
		}
	}

	return 0;
}

/// Wonder production boost
int CvReligionBeliefs::GetWonderProductionModifier(EraTypes eWonderEra, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = 0;
		EraTypes eObsoleteEra = pBeliefs->GetEntry(*it)->GetObsoleteEra();
		if (eObsoleteEra == NO_ERA || eWonderEra < eObsoleteEra)
		{
			iValue = pBeliefs->GetEntry(*it)->GetWonderProductionModifier();
		}
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetUnitProductionModifier(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetUnitProductionModifier();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Player happiness boost
int CvReligionBeliefs:: GetPlayerHappiness(bool bAtPeace, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = 0;
		if (bAtPeace || !pBeliefs->GetEntry(*it)->RequiresPeace())
		{
			iValue = pBeliefs->GetEntry(*it)->GetPlayerHappiness();
		}
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Player culture modifier
int CvReligionBeliefs:: GetPlayerCultureModifier(bool bAtPeace, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = 0;
		if (bAtPeace || !pBeliefs->GetEntry(*it)->RequiresPeace())
		{
			iValue = pBeliefs->GetEntry(*it)->GetPlayerCultureModifier();
		}
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Happiness per following city
float CvReligionBeliefs::GetHappinessPerFollowingCity(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	float rtnValue = 0.0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		float fValue = pBeliefs->GetEntry(*it)->GetHappinessPerFollowingCity();
		if (fValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += fValue;
		}
	}

	return rtnValue;
}

/// Gold per following city
int CvReligionBeliefs:: GetGoldPerFollowingCity(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetGoldPerFollowingCity();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Gold per following city
int CvReligionBeliefs:: GetGoldPerXFollowers(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetGoldPerXFollowers();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Gold per following city
int CvReligionBeliefs:: GetGoldWhenCityAdopts(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetGoldWhenCityAdopts();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Science per other religion follower
int CvReligionBeliefs:: GetSciencePerOtherReligionFollower(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetSciencePerOtherReligionFollower();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// City growth modifier
int CvReligionBeliefs::GetCityGrowthModifier(bool bAtPeace, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = 0;
		if (bAtPeace || !pBeliefs->GetEntry(*it)->RequiresPeace())
		{
			iValue = pBeliefs->GetEntry(*it)->GetCityGrowthModifier();
		}
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Extra yield
int CvReligionBeliefs::GetCityYieldChange(int iPopulation, YieldTypes eYield, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = 0;
		if (iPopulation >= pBeliefs->GetEntry(*it)->GetMinPopulation())
		{
			iValue = pBeliefs->GetEntry(*it)->GetCityYieldChange(eYield);
		}
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Extra holy city yield
int CvReligionBeliefs::GetHolyCityYieldChange (YieldTypes eYield, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetHolyCityYieldChange(eYield);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Extra yield for foreign cities following religion
int CvReligionBeliefs::GetYieldChangePerForeignCity(YieldTypes eYield, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldChangePerForeignCity(eYield);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Extra yield for foreign followers
int CvReligionBeliefs::GetYieldChangePerXForeignFollowers(YieldTypes eYield, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldChangePerXForeignFollowers(eYield);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetYieldChangePerXCityStateFollowers(YieldTypes eYield, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldChangePerXCityStateFollowers(eYield);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetYieldPerFollowingCity(YieldTypes eYield, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldPerFollowingCity(eYield);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetLakePlotYieldChange(YieldTypes eYield, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetLakePlotYieldChange(eYield);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetYieldPerXFollowers(YieldTypes eYield, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldPerXFollowers(eYield);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetYieldPerOtherReligionFollower(YieldTypes eYield, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldPerOtherReligionFollower(eYield);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Extra yield from this improvement
int CvReligionBeliefs::GetResourceQuantityModifier(ResourceTypes eResource, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetResourceQuantityModifier(eResource);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Extra yield from this improvement
int CvReligionBeliefs::GetImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYield, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetImprovementYieldChange(eImprovement, eYield);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Get yield change from beliefs for a specific building class
int CvReligionBeliefs::GetBuildingClassYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYieldType, int iFollowers, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int temprtnValue = pBeliefs->GetEntry(*it)->GetBuildingClassYieldChange(eBuildingClass, eYieldType);
		if (temprtnValue != 0 && iFollowers >= pBeliefs->GetEntry(*it)->GetMinFollowers())
		{
			if (IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
			{
				rtnValue += temprtnValue;
			}
		}
	}

	return rtnValue;
}

/// Get Happiness from beliefs for a specific building class
int CvReligionBeliefs::GetBuildingClassHappiness(BuildingClassTypes eBuildingClass, int iFollowers, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = 0;
		if (iFollowers >= pBeliefs->GetEntry(*it)->GetMinFollowers())
		{
			iValue = pBeliefs->GetEntry(*it)->GetBuildingClassHappiness(eBuildingClass);
		}
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Get yield change from beliefs for a specific feature
int CvReligionBeliefs::GetFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetFeatureYieldChange(eFeature, eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetYieldPerXTerrainTimes100(TerrainTypes eTerrain, YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldPerXTerrainTimes100(eTerrain, eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetYieldPerXFeatureTimes100(FeatureTypes eFeature, YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldPerXFeatureTimes100(eFeature, eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetCityYieldFromUnimprovedFeature(FeatureTypes eFeature, YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetCityYieldFromUnimprovedFeature(eFeature, eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetUnimprovedFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetUnimprovedFeatureYieldChange(eFeature, eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Get yield change from beliefs for a specific resource
int CvReligionBeliefs::GetResourceYieldChange(ResourceTypes eResource, YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetResourceYieldChange(eResource, eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Get yield change from beliefs for a specific terrain
int CvReligionBeliefs::GetTerrainYieldChange(TerrainTypes eTerrain, YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetTerrainYieldChange(eTerrain, eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetTradeRouteYieldChange(DomainTypes eDomain, YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetTradeRouteYieldChange(eDomain, eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetSpecialistYieldChange(SpecialistTypes eSpecialist, YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetSpecialistYieldChange(eSpecialist, eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetGreatPersonExpendedYield(GreatPersonTypes eGreatPerson, YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetGreatPersonExpendedYield(eGreatPerson, eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetGoldenAgeGreatPersonRateModifier(GreatPersonTypes eGreatPerson, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetGoldenAgeGreatPersonRateModifier(eGreatPerson);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetCapitalYieldChange(int iPopulation, YieldTypes eYield, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = 0;
		if (iPopulation >= pBeliefs->GetEntry(*it)->GetMinPopulation())
		{
			iValue = pBeliefs->GetEntry(*it)->GetCapitalYieldChange(eYield);
		}
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{	
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetCoastalCityYieldChange(int iPopulation, YieldTypes eYield, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = 0;
		if (iPopulation >= pBeliefs->GetEntry(*it)->GetMinPopulation())
		{
			iValue = pBeliefs->GetEntry(*it)->GetCoastalCityYieldChange(eYield);
		}
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetGreatWorkYieldChange(int iPopulation, YieldTypes eYield, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = 0;
		if (iPopulation >= pBeliefs->GetEntry(*it)->GetMinPopulation())
		{
			iValue = pBeliefs->GetEntry(*it)->GetGreatWorkYieldChange(eYield);
		}
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Do we get one of our yields from defeating an enemy?
int CvReligionBeliefs::GetYieldFromKills(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldFromKills(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetYieldFromRemoveHeresy(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldFromRemoveHeresy(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetGreatPersonPoints(GreatPersonTypes eGreatPerson, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetGreatPersonPoints(eGreatPerson);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Do we get one of our yields from defeating a barbarian?
int CvReligionBeliefs::GetYieldFromBarbarianKills(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldFromBarbarianKills(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Get yield change from beliefs for a specific plot
int CvReligionBeliefs::GetPlotYieldChange(PlotTypes ePlot, YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetPlotYieldChange(ePlot, eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Get free promotions from beliefs
std::vector<int> CvReligionBeliefs::GetFreePromotions(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	std::vector<int> rtnVector;

	if (MOD_RELIGION_EXTENSIONS)
	{
		// combine all the free promotions from the beliefs
		for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
		{
			std::vector<int> aPromotions = pBeliefs->GetEntry(*it)->GetFreePromotions();
			if (!aPromotions.empty() && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
			{
				for (std::vector<int>::iterator itPromotions = aPromotions.begin(); itPromotions != aPromotions.end(); ++itPromotions)
				{
					rtnVector.push_back(*itPromotions);
				}
			}
		}

		// sort and remove duplicates
		if (rtnVector.size() > 1)
		{
			std::stable_sort(rtnVector.begin(), rtnVector.end());
			rtnVector.erase( std::unique( rtnVector.begin(), rtnVector.end() ), rtnVector.end() );
		}

	}

	return rtnVector;
}

///  Gain instant yields when building improvements
int CvReligionBeliefs::GetYieldFromImprovementBuild(YieldTypes eYield, bool bEraScaling, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldFromImprovementBuild(eYield, bEraScaling);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Gain instant yields when pillaging
int CvReligionBeliefs::GetYieldFromPillageGlobal(YieldTypes eYield, bool bEraScaling, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldFromPillageGlobal(eYield, bEraScaling);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

// Get happiness boost from a resource
int CvReligionBeliefs::GetResourceHappiness(ResourceTypes eResource, PlayerTypes ePlayer, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetResourceHappiness(eResource);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, NULL, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Get yield change from beliefs for a specialist being present in city
int CvReligionBeliefs::GetYieldChangeAnySpecialist(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldChangeAnySpecialist(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Get yield change from beliefs for a trade route
int CvReligionBeliefs::GetYieldChangeTradeRoute(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldChangeTradeRoute(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Get yield change from beliefs for a natural wonder
int CvReligionBeliefs::GetYieldChangeNaturalWonder(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldChangeNaturalWonder(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Get yield change from beliefs for a world wonder
int CvReligionBeliefs::GetYieldChangeWorldWonder(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldChangeWorldWonder(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Get yield modifier from beliefs for a natural wonder
int CvReligionBeliefs::GetYieldModifierNaturalWonder(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldModifierNaturalWonder(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Get yield modifier from beliefs for a natural wonder
int CvReligionBeliefs::GetMaxYieldModifierPerFollower(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetMaxYieldModifierPerFollower(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
int CvReligionBeliefs::GetMaxYieldModifierPerFollowerPercent(int& iMaxVal, YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	iMaxVal = 1;
	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetMaxYieldModifierPerFollowerPercent(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
			iMaxVal = max(iMaxVal, pBeliefs->GetEntry(*it)->GetMaxYieldModifierPerFollower(eYieldType));
		}
	}

	return rtnValue;
}



/// Does this belief allow a building to be constructed?
bool CvReligionBeliefs::IsBuildingClassEnabled(BuildingClassTypes eType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();

	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		if (pBeliefs->GetEntry(*it)->IsBuildingClassEnabled((int)eType))
		{
			if (IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
			{
				return true;
			}
		}
	}

	return false;
}

/// Is there a belief that allows faith buying of units
bool CvReligionBeliefs::IsFaithBuyingEnabled(EraTypes eEra, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		if (pBeliefs->GetEntry(*it)->IsFaithUnitPurchaseEra((int)eEra))
		{
			if (IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
			{
				return true;
			}
		}
	}

	return false;
}
/// Is there a belief that allows faith buying of specific units?
bool CvReligionBeliefs::IsSpecificFaithBuyingEnabled(UnitTypes eUnit, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		if (pBeliefs->GetEntry(*it)->IsFaithUnitPurchaseSpecific((int)eUnit))
		{
			if (IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
			{
				return true;
			}
		}
	}

	return false;
}

/// Is there a belief that allows faith buying of specific units?
BeliefTypes CvReligionBeliefs::GetSpecificFaithBuyingEnabledBelief(UnitTypes eUnit) const
{
	CvBeliefXMLEntries* pAllBeliefs = GC.GetGameBeliefs();

	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		CvBeliefEntry* pBelief = pAllBeliefs->GetEntry(*it);

		if (pBelief == NULL)
			continue;

		if (pBelief->IsFaithUnitPurchaseSpecific((int)eUnit))
			return (BeliefTypes)(*it);
	}

	return NO_BELIEF;
}
/// Is there a belief that allows us to convert adjacent barbarians?
bool CvReligionBeliefs::IsConvertsBarbarians(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		if (pBeliefs->GetEntry(*it)->ConvertsBarbarians() && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			return true;
		}
	}

	return false;
}

/// Is there a belief that allows faith buying of all great people
bool CvReligionBeliefs::IsFaithPurchaseAllGreatPeople(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		if (pBeliefs->GetEntry(*it)->FaithPurchaseAllGreatPeople() && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			return true;
		}
	}

	return false;
}

/// Is there a belief that requires improvements?
bool CvReligionBeliefs::RequiresImprovement(PlayerTypes ePlayer, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		if (pBeliefs->GetEntry(*it)->RequiresImprovement())
		{
			if (IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, NULL, bHolyCityOnly))
			{
				return true;
			}
		}
	}
	return false;
}
/// Is there a belief that requires improvements?
bool CvReligionBeliefs::RequiresNoImprovement(PlayerTypes ePlayer, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		if (pBeliefs->GetEntry(*it)->RequiresNoImprovement())
		{
			if (IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, NULL, bHolyCityOnly))
			{
				return true;
			}
		}
	}

	return false;
}
/// Is there a belief that requires improvements?
bool CvReligionBeliefs::RequiresNoFeature(PlayerTypes ePlayer, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		if (pBeliefs->GetEntry(*it)->RequiresNoFeature())
		{
			if (IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, NULL, bHolyCityOnly))
			{
			
				return true;
			}
		}
	}

	return false;
}
/// Is there a belief that requires a resource?
bool CvReligionBeliefs::RequiresResource(PlayerTypes ePlayer, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		if (pBeliefs->GetEntry(*it)->RequiresResource())
		{
			if (IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, NULL, bHolyCityOnly))
			{
				return true;
			}
		}
	}

	return false;
}

int CvReligionBeliefs::GetYieldPerActiveTR(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	int iNumTRs = GET_PLAYER(ePlayer).GetTrade()->GetNumberOfTradeRoutesCity(pCity);
	if (iNumTRs <= 0)
		return 0;

	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldPerActiveTR(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			iValue *= iNumTRs;
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetYieldPerConstruction(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldPerConstruction(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetYieldPerWorldWonderConstruction(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldPerWorldWonderConstruction(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Get yield modifier from beliefs for pop
int CvReligionBeliefs::GetYieldPerPop(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldPerPop(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
/// Get bonus pressure from trade routes
int CvReligionBeliefs::GetPressureChangeTradeRoute(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetPressureChangeTradeRoute();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
/// Get bonus pressure from trade routes
int CvReligionBeliefs::GetHappinessFromForeignSpies(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetHappinessFromForeignSpies();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Get yield modifier from beliefs for gpt
int CvReligionBeliefs::GetYieldPerGPT(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldPerGPT(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
/// Get yield modifier from beliefs for luxury
int CvReligionBeliefs::GetYieldPerLux(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldPerLux(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
/// Get yield modifier from beliefs for border growth
int CvReligionBeliefs::GetYieldPerBorderGrowth(YieldTypes eYieldType, bool bEraScaling, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldPerBorderGrowth(eYieldType, bEraScaling);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
/// Get yield modifier from beliefs for border growth
int CvReligionBeliefs::GetYieldPerHeal(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldPerHeal(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
/// Get yield modifier from beliefs from birth
int CvReligionBeliefs::GetYieldPerBirth(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldPerBirth(eYieldType);
		if (iValue != 0)
		{
			//Does double duty for pantheons and followers, so needs two checks
			if (pBeliefs->GetEntry(*it)->IsFollowerBelief())
			{
				if (IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
				{
					rtnValue += iValue;
				}
			}
			else
			{
				//No city here, because it is universal.
				if (IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, NULL, bHolyCityOnly))
				{
					rtnValue += iValue;
				}
			}
		}
	}

	return rtnValue;
}

int CvReligionBeliefs::GetYieldPerHolyCityBirth(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldPerHolyCityBirth(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Get yield modifier from beliefs from science
int CvReligionBeliefs::GetYieldPerScience(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldPerScience(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
/// Get yield modifier from beliefs from GP Use
int CvReligionBeliefs::GetYieldFromGPUse(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldFromGPUse(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
/// Get yield modifier from beliefs from GA bonus
int CvReligionBeliefs::GetYieldBonusGoldenAge(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldBonusGoldenAge(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
/// Get yield modifier from beliefs from spread
int CvReligionBeliefs::GetYieldFromSpread(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldFromSpread(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
/// Get yield modifier from beliefs from foreign spread
int CvReligionBeliefs::GetYieldFromForeignSpread(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldFromForeignSpread(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
/// Get yield modifier from beliefs from conquest
int CvReligionBeliefs::GetYieldFromConquest(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldFromConquest(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
/// Get yield modifier from beliefs from policy unlock
int CvReligionBeliefs::GetYieldFromPolicyUnlock(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldFromPolicyUnlock(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
/// Get yield modifier from beliefs from era unlock
int CvReligionBeliefs::GetYieldFromEraUnlock(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldFromEraUnlock(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
/// Get yield modifier from beliefs from tech unlock
int CvReligionBeliefs::GetYieldFromTechUnlock(YieldTypes eYieldType, bool bEraScaling, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldFromTechUnlock(eYieldType, bEraScaling);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
/// Get yield modifier from beliefs from city conversion
int CvReligionBeliefs::GetYieldFromConversion(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldFromConversion(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Get yield modifier from beliefs from city conversion
int CvReligionBeliefs::GetYieldFromConversionExpo(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldFromConversionExpo(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
/// Get yield modifier from beliefs from wtlkd
int CvReligionBeliefs::GetYieldFromWLTKD(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldFromWLTKD(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
/// Get yield modifier from beliefs from passing a WC proposal
int CvReligionBeliefs::GetYieldFromProposal(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldFromProposal(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
/// Get yield modifier from beliefs from Hosting WC
int CvReligionBeliefs::GetYieldFromHost(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldFromHost(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Get yield modifier from beliefs from Hosting WC
int CvReligionBeliefs::GetYieldFromFaithPurchase(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldFromFaithPurchase(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}


/// Get yield per known pantheon (times100)
int CvReligionBeliefs::GetYieldFromKnownPantheons(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetYieldFromKnownPantheons(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Get yield from beliefs from # of followers
int CvReligionBeliefs::GetMaxYieldPerFollower(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetMaxYieldPerFollower(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}
/// Get yield from beliefs from # of followers halved
int CvReligionBeliefs::GetMaxYieldPerFollowerPercent(YieldTypes eYieldType, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetMaxYieldPerFollowerPercent(eYieldType);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Get yield from beliefs from # of followers halved
int CvReligionBeliefs::GetIgnorePolicyRequirementsAmount(EraTypes eEra, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();

	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		EraTypes eObsoleteEra = pBeliefs->GetEntry(*it)->GetObsoleteEra();
		if (pBeliefs->GetEntry(*it)->GetIgnorePolicyRequirementsAmount() && (eObsoleteEra == NO_ERA || eEra < eObsoleteEra) && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			return true;
		}
	}

	return false;
}

/// Get yield from beliefs from # of followers halved
int CvReligionBeliefs::GetCSYieldBonus(PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	int rtnValue = 0;

	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		int iValue = pBeliefs->GetEntry(*it)->GetCSYieldBonus();
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			rtnValue += iValue;
		}
	}

	return rtnValue;
}

/// Get votes per improvement (fractional) from belief
fraction CvReligionBeliefs::GetVoteFromOwnedImprovement(ImprovementTypes eImprovement, PlayerTypes ePlayer, const CvCity* pCity, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	
	fraction fVotes = 0;
	
	// if two beliefs give fractional votes for the same improvement, then the fractional vote gets larger (1/2 + 1/3 = 5/6)
	for (BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		/// GetImprovementVoteChange() returns the number of improvements required for each vote
		int iValue = pBeliefs->GetEntry(*it)->GetImprovementVoteChange(eImprovement);
		if (iValue != 0 && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, pCity, bHolyCityOnly))
		{
			fVotes += fraction(1, iValue);
   		}
	}

	return fVotes;
}


/// Get unique civ
CivilizationTypes CvReligionBeliefs::GetUniqueCiv(PlayerTypes ePlayer, bool bHolyCityOnly) const
{
	CvBeliefXMLEntries* pBeliefs = GC.GetGameBeliefs();
	CivilizationTypes eCivilization = NO_CIVILIZATION;

	for(BeliefList::const_iterator it = m_ReligionBeliefs.begin(); it != m_ReligionBeliefs.end(); ++it)
	{
		if ((pBeliefs->GetEntry(*it)->GetRequiredCivilization() != NO_CIVILIZATION) && IsBeliefValid((BeliefTypes)*it, GetReligion(), ePlayer, NULL, bHolyCityOnly))
		{
			return pBeliefs->GetEntry(*it)->GetRequiredCivilization();
		}
	}

	return eCivilization;
}

template<typename ReligionBeliefs, typename Visitor>
void CvReligionBeliefs::Serialize(ReligionBeliefs& religionBeliefs, Visitor& visitor)
{
	visitor(religionBeliefs.m_eReligion);
	visitor(religionBeliefs.m_ReligionBeliefs);
	visitor(religionBeliefs.m_BeliefLookup);
}

/// Serialization read
void CvReligionBeliefs::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

/// Serialization write
void CvReligionBeliefs::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator<<(FDataStream& saveTo, const CvReligionBeliefs& readFrom)
{
	readFrom.Write(saveTo);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, CvReligionBeliefs& writeTo)
{
	writeTo.Read(loadFrom);
	return loadFrom;
}

/// BELIEF HELPER CLASSES

/// Is there an adjacent barbarian naval unit that could be converted?
bool CvBeliefHelpers::ConvertBarbarianUnit(const CvUnit *pByUnit, CvUnit* pUnit)

{
	CvUnit* pNewUnit = NULL;
	CvPlot *pPlot = pUnit->plot();

	CvPlayer* pPlayer = &GET_PLAYER(pByUnit->getOwner());

	if (MOD_EVENTS_UNIT_CAPTURE) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_UnitCaptured, pPlayer->GetID(), pByUnit->GetID(), pUnit->getOwner(), pUnit->GetID(), false, 4);
	}

	// Convert the barbarian into our unit
	pNewUnit = pPlayer->initUnit(pUnit->getUnitType(), pUnit->getX(), pUnit->getY(), pUnit->AI_getUnitAIType(), REASON_CONVERT, true /*bNoMove*/, false);
	ASSERT(pNewUnit, "pNewUnit is not assigned a valid value");
	pNewUnit->convert(pUnit, false);
	pNewUnit->setupGraphical();
	pNewUnit->finishMoves(); // No move first turn

	if(GC.getLogging() && GC.getAILogging())
	{
		CvString logMsg;
		logMsg.Format("Converted barbarian (with belief), X: %d, Y: %d", pUnit->getX(), pUnit->getY());
		pPlayer->GetHomelandAI()->LogHomelandMessage(logMsg);
	}

	CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_HEATHEN_CONVERTS");
	CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_HEATHEN_CONVERTS");
	pPlayer->GetNotifications()->Add(NOTIFICATION_GENERIC, strBuffer, strSummary, pUnit->getX(), pUnit->getY(), -1);

	if (pPlot->getImprovementType() == GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT))
	{
		pPlot->setImprovementType(NO_IMPROVEMENT);

		CvBarbarians::DoBarbCampCleared(pPlot, pPlayer->GetID());
		pPlot->SetPlayerThatClearedBarbCampHere(pPlayer->GetID());

		// Don't give gold for Camps cleared by heathen conversion
	}

	return true;
}