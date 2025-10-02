/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvUnitClasses.h"

// include this after all other headers!
#include "LintFree.h"

//====================================
// PlagueInfo
//====================================

/// Equals operator
bool PlagueInfo::operator==(const PlagueInfo& rhs) const
{
	return (ePlague == rhs.ePlague &&
		eDomain == rhs.eDomain &&
		bApplyOnAttack == rhs.bApplyOnAttack &&
		bApplyOnDefense == rhs.bApplyOnDefense &&
		iApplyChance == rhs.iApplyChance);
}

bool PlagueInfo::operator<(const PlagueInfo& rhs) const { return GC.getPromotionInfo(ePlague)->GetPlaguePriority() > GC.getPromotionInfo(rhs.ePlague)->GetPlaguePriority(); }

template<typename PlagueInfoTemplate, typename Visitor>
void PlagueInfo::Serialize(PlagueInfoTemplate& plagueInfo, Visitor& visitor)
{
	visitor(plagueInfo.ePlague);
	visitor(plagueInfo.eDomain);
	visitor(plagueInfo.bApplyOnAttack);
	visitor(plagueInfo.bApplyOnDefense);
	visitor(plagueInfo.iApplyChance);
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, PlagueInfo& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	PlagueInfo::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const PlagueInfo& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	PlagueInfo::Serialize(readFrom, serialVisitor);
	return saveTo;
}

//====================================
// CvPromotionEntry
//====================================

/// Constructor
CvPromotionEntry::CvPromotionEntry():
	m_iLayerAnimationPath(ANIMATIONPATH_NONE),
	m_iPrereqPromotion(NO_PROMOTION),
	m_iPrereqOrPromotion1(NO_PROMOTION),
	m_iPrereqOrPromotion2(NO_PROMOTION),
	m_iPrereqOrPromotion3(NO_PROMOTION),
	m_iPrereqOrPromotion4(NO_PROMOTION),
	m_iPrereqOrPromotion5(NO_PROMOTION),
	m_iPrereqOrPromotion6(NO_PROMOTION),
	m_iPrereqOrPromotion7(NO_PROMOTION),
	m_iPrereqOrPromotion8(NO_PROMOTION),
	m_iPrereqOrPromotion9(NO_PROMOTION),
	m_iTechPrereq(NO_TECH),
	m_iInvisibleType(NO_INVISIBLE),
	m_iSeeInvisibleType(NO_INVISIBLE),
	m_iVisibilityChange(0),
#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
	m_iReconChange(0),
#endif
	m_iMovesChange(0),
	m_iMoveDiscountChange(0),
	m_iRangeChange(0),
	m_iRangedAttackModifier(0),
	m_iInterceptionCombatModifier(0),
	m_iInterceptionDefenseDamageModifier(0),
	m_iAirSweepCombatModifier(0),
	m_iInterceptChanceChange(0),
	m_iNumInterceptionChange(0),
	m_iAirInterceptRangeChange(0), // JJ: This is new
	m_iEvasionChange(0),
	m_iCargoChange(0),
	m_iEnemyHealChange(0),
	m_iNeutralHealChange(0),
	m_iFriendlyHealChange(0),
	m_iSameTileHealChange(0),
	m_iAdjacentTileHealChange(0),
	m_iEnemyDamageChance(0),
	m_iNeutralDamageChance(0),
	m_iEnemyDamage(0),
	m_iNeutralDamage(0),
	m_iCombatPercent(0),
	m_iCityAttackPercent(0),
	m_iCityDefensePercent(0),
	m_iRangedDefenseMod(0),
	m_iHillsAttackPercent(0),
	m_iHillsDefensePercent(0),
	m_iOpenAttackPercent(0),
	m_iOpenRangedAttackMod(0),
	m_iRoughAttackPercent(0),
	m_iRoughRangedAttackMod(0),
	m_iAttackFortifiedMod(0),
	m_iAttackWoundedMod(0),
	m_iAttackFullyHealedMod(0),
	m_iAttackAboveHealthMod(0),
	m_iAttackBelowHealthMod(0),
	m_bRangedFlankAttack(false),
	m_iExtraFlankPower(0),
	m_iFlankAttackModifier(0),
	m_iNearbyEnemyCombatMod(0),
	m_iNearbyEnemyCombatRange(0),
	m_iOpenDefensePercent(0),
	m_iRoughDefensePercent(0),
	m_iOpenFromPercent(0),
	m_iRoughFromPercent(0),
	m_iExtraAttacks(0),
	m_bGreatGeneral(false),
	m_bGreatAdmiral(false),
	m_iAuraRangeChange(0),
	m_iAuraEffectChange(0),
	m_iNumRepairCharges(0),
	m_iMilitaryCapChange(0),
	m_iGreatGeneralModifier(0),
	m_bGreatGeneralReceivesMovement(false),
	m_iGreatGeneralCombatModifier(0),
	m_iFriendlyLandsModifier(0),
	m_iFriendlyLandsAttackModifier(0),
	m_iOutsideFriendlyLandsModifier(0),
	m_iCommandType(NO_COMMAND),
	m_bNoSupply(false),
	m_iMaxHitPointsChange(0),
	m_iMaxHitPointsModifier(0),
	m_iVsUnhappyMod(0),
	m_iUpgradeDiscount(0),
	m_iExperiencePercent(0),
	m_iAdjacentMod(0),
	m_iNoAdjacentUnitMod(0),
	m_iAttackMod(0),
	m_iDefenseMod(0),
	m_iBorderMod(0),
	m_iMarriageMod(0),
	m_iMarriageModCap(0),
	m_iGetGroundAttackDamage(0),
	m_iDropRange(0),
	m_iExtraNavalMoves(0),
	m_iHPHealedIfDefeatEnemy(0),
	m_iGoldenAgeValueFromKills(0),
	m_iExtraWithdrawal(0),
	m_iCombatChange(0),
	m_iTileDamageIfNotMoved(0),
	m_iFortifiedModifier(0),
	m_iMinEffectiveHealth(0),
	m_bRequiresLeadership(false),
	m_bCannotHeal(false),
	m_bPillageFortificationsOnKill(false),
#if defined(MOD_BALANCE_CORE_JFD)
	m_iPlagueID(NO_PROMOTION),
	m_iPlaguePriority(0),
#endif
	m_iEmbarkExtraVisibility(0),
	m_iEmbarkDefenseModifier(0),
	m_iCapitalDefenseModifier(0),
	m_iCapitalDefenseFalloff(0),
	m_iCapitalDefenseLimit(0),
	m_iCityAttackPlunderModifier(0),
	m_iReligiousStrengthLossRivalTerritory(0),
	m_iTradeMissionInfluenceModifier(0),
	m_iTradeMissionGoldModifier(0),
	m_iCombatModPerLevel(0),
	m_iCaptureDefeatedEnemyChance(0),
	m_iBarbarianCombatBonus(0),
	m_iGoodyHutYieldBonus(0),
	m_iDiploMissionInfluence(0),
	m_bGainsXPFromScouting(false),
	m_iXPFromPillaging(0),
	m_iExtraXPOnKill(0),
	m_bGainsXPFromSpotting(false),
	m_bCannotBeCaptured(false),
	m_bIsLostOnMove(false),
	m_bCityStateOnly(false),
	m_bDiplomaticMissionAccomplishment(false),
	m_bBarbarianOnly(false),
	m_bStrongerDamaged(false),
	m_bFightWellDamaged(false),
	m_bFreeAttackMoves(false),
	m_iNegatesPromotion(NO_PROMOTION),
	m_iForcedDamageValue(0),
	m_iChangeDamageValue(0),
	m_iDamageTakenMod(0),
	m_iInfluenceFromCombatXPTimes100(0),
	m_iPromotionDuration(0),
	m_iMoraleBreakChance(0),
	m_iDamageAoEFortified(0),
	m_iWorkRateMod(0),
	m_bCannotBeChosen(false),
	m_bLostWithUpgrade(false),
	m_bNotWithUpgrade(false),
	m_bInstaHeal(false),
	m_bLeader(false),
	m_bBlitz(false),
	m_bAmphib(false),
	m_bRiver(false),
	m_bEnemyRoute(false),
	m_bRivalTerritory(false),
	m_bMustSetUpToRangedAttack(false),
	m_bRangedSupportFire(false),
	m_bAlwaysHeal(false),
	m_bHealOutsideFriendly(false),
	m_bHillsDoubleMove(false),
	m_bRiverDoubleMove(false),
	m_bIgnoreTerrainCost(false),
	m_bIgnoreTerrainDamage(false),
	m_bIgnoreFeatureDamage(false),
	m_bExtraTerrainDamage(false),
	m_bExtraFeatureDamage(false),
#if defined(MOD_PROMOTIONS_IMPROVEMENT_BONUS)
	m_iNearbyImprovementCombatBonus(0),
	m_iNearbyImprovementBonusRange(0),
	m_eCombatBonusImprovement(NO_IMPROVEMENT),
#endif
	m_iNearbyUnitClassBonus(0),
	m_iNearbyUnitClassBonusRange(0),
	m_iCombatBonusFromNearbyUnitClass(NO_UNITCLASS),
	m_iWonderProductionModifier(0),
	m_iAOEDamageOnKill(0),
	m_iAOEDamageOnPillage(0),
	m_iAOEHealOnPillage(0),
	m_iCombatModPerCSAlliance(0),
	m_iAttackModPerSamePromotionAttack(0),
	m_iAttackModPerSamePromotionAttackCap(0),
	m_iAoEDamageOnMove(0),
	m_iPartialHealOnPillage(0),
	m_iSplashDamage(0),
	m_iMinRange(0),
	m_iMaxRange(0),
	m_iMultiAttackBonus(0),
	m_iLandAirDefenseValue(0),
	m_iDamageReductionCityAssault(0),
	m_bMountainsDoubleMove(false),
	m_bEmbarkFlatCost(false),
	m_bDisembarkFlatCost(false),
	m_bMountedOnly(false),
#if defined(MOD_PROMOTIONS_CROSS_MOUNTAINS)
	m_bCanCrossMountains(false),
#endif
#if defined(MOD_PROMOTIONS_CROSS_OCEANS)
	m_bCanCrossOceans(false),
#endif
#if defined(MOD_PROMOTIONS_CROSS_ICE)
	m_bCanCrossIce(false),
#endif
#if defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
	m_bGGFromBarbarians(false),
#endif
	m_bRoughTerrainEndsTurn(false),
	m_bCapturedUnitsConscripted(false),
	m_bHoveringUnit(false),
	m_bFlatMovementCost(false),
	m_bCanMoveImpassable(false),
	m_bNoCapture(false),
	m_bOnlyDefensive(false),
	m_bNoAttackInOcean(false),
	m_bNoDefensiveBonus(false),
	m_bNukeImmune(false),
	m_bHiddenNationality(false),
	m_bAlwaysHostile(false),
	m_bNoRevealMap(false),
	m_bRecon(false),
	m_bCanMoveAllTerrain(false),
	m_bCanMoveAfterAttacking(false),
	m_bAirSweepCapable(false),
	m_bAllowsEmbarkation(false),
	m_bRangeAttackIgnoreLOS(false),
	m_bFreePillageMoves(false),
	m_bHealOnPillage(false),
	m_bHealIfDefeatExcludesBarbarians(false),
	m_bEmbarkedAllWater(false),
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
	m_bEmbarkedDeepWater(false),
#endif
	m_bCityAttackOnly(false),
	m_bCaptureDefeatedEnemy(false),
	m_bIgnoreGreatGeneralBenefit(false),
	m_bIgnoreZOC(false),
	m_bHasPostCombatPromotions(false),
	m_bPostCombatPromotionsExclusive(false),
	m_bSapper(false),
	m_iNearbyCityCombatMod(0),
	m_iNearbyFriendlyCityCombatMod(0),
	m_iNearbyEnemyCityCombatMod(0),
	m_bIsNearbyPromotion(false),
	m_iNearbyRange(0),
	m_iConvertDomainUnit(NO_UNIT),
	m_iConvertDomain(NO_DOMAIN),
	m_bIsConvertUnit(false),
	m_bIsConvertEnemyUnitToBarbarian(false),
	m_bIsConvertOnFullHP(false),
	m_bIsConvertOnDamage(false),
	m_iDamageThreshold(0),
	m_iConvertDamageOrFullHPUnit(NO_UNIT),
	m_iStackedGreatGeneralExperience(0),
	m_iPillageBonusStrength(0),
	m_iReligiousPressureModifier(0),
	m_iAdjacentCityDefenseMod(0),
	m_iNearbyEnemyDamage(0),
	m_iMilitaryProductionModifier(0),
	m_piYieldModifier(NULL),
	m_piYieldChange(NULL),
	m_piYieldFromAncientRuins(NULL),
	m_piYieldFromTRPlunder(NULL),
	m_iGeneralGoldenAgeExpPercent(0),
	m_iGiveCombatMod(0),
	m_iGiveHPHealedIfEnemyKilled(0),
	m_iGiveExperiencePercent(0),
	m_iGiveOutsideFriendlyLandsModifier(0),
	m_iGiveDomain(NO_DOMAIN),
	m_iGiveExtraAttacks(0),
	m_iGiveDefenseMod(0),
	m_bGiveInvisibility(false),
	m_bGiveOnlyOnStartingTurn(false),
	m_iNearbyHealEnemyTerritory(0),
	m_iNearbyHealNeutralTerritory(0),
	m_iNearbyHealFriendlyTerritory(0),
	m_iAdjacentEnemySapMovement(0),
	m_bCanHeavyCharge(false),
	m_piTerrainAttackPercent(NULL),
	m_piTerrainDefensePercent(NULL),
	m_piFeatureAttackPercent(NULL),
	m_piFeatureDefensePercent(NULL),
	m_piTerrainModifierAttack(NULL),
	m_piTerrainModifierDefense(NULL),
	m_piYieldFromScouting(NULL),
	m_piYieldFromKills(NULL),
	m_piYieldFromBarbarianKills(NULL),
	m_piYieldFromCombatExperienceTimes100(NULL),
	m_piGarrisonYield(NULL),
	m_piFortificationYield(NULL),
	m_piUnitCombatModifierPercent(NULL),
	m_piUnitCombatModifierPercentAttack(NULL),
		m_piUnitCombatModifierPercentDefense(NULL),
	m_piUnitClassModifierPercent(NULL),
	m_piUnitClassAttackModifier(NULL),
	m_piUnitClassDefenseModifier(NULL),
	m_piDomainModifierPercent(NULL),
	m_piDomainAttackPercent(NULL),
	m_piDomainDefensePercent(NULL),
	m_piFeaturePassableTech(NULL),
	m_piCombatModPerAdjacentUnitCombatModifierPercent(NULL),
	m_piCombatModPerAdjacentUnitCombatAttackModifier(NULL),
	m_piCombatModPerAdjacentUnitCombatDefenseModifier(NULL),
	m_pbIgnoreTerrainCostIn(NULL),
	m_pbIgnoreTerrainCostFrom(NULL),
	m_pbIgnoreFeatureCostIn(NULL),
	m_pbIgnoreFeatureCostFrom(NULL),
	m_pbTerrainDoubleMove(NULL),
	m_pbFeatureDoubleMove(NULL),
#if defined(MOD_PROMOTIONS_HALF_MOVE)
	m_pbTerrainHalfMove(NULL),
	m_pbTerrainExtraMove(NULL),
	m_pbFeatureHalfMove(NULL),
	m_pbFeatureExtraMove(NULL),
#endif
	m_pbTerrainDoubleHeal(NULL),
	m_pbFeatureDoubleHeal(NULL),
	m_pbTerrainImpassable(NULL),
	m_piTerrainPassableTech(NULL),
	m_pbFeatureImpassable(NULL),
	m_pbUnitCombat(NULL),
	m_pbCivilianUnitType(NULL),
#if defined(MOD_PROMOTIONS_UNIT_NAMING)
	m_pbUnitName(NULL),
#endif
	m_pbPostCombatRandomPromotion(NULL)
{
}

/// Destructor
CvPromotionEntry::~CvPromotionEntry(void)
{
	SAFE_DELETE_ARRAY(m_piTerrainAttackPercent);
	SAFE_DELETE_ARRAY(m_piTerrainDefensePercent);
	SAFE_DELETE_ARRAY(m_piFeatureAttackPercent);
	SAFE_DELETE_ARRAY(m_piFeatureDefensePercent);
	SAFE_DELETE_ARRAY(m_piTerrainModifierAttack);
	SAFE_DELETE_ARRAY(m_piTerrainModifierDefense);
	SAFE_DELETE_ARRAY(m_piYieldFromScouting);
	SAFE_DELETE_ARRAY(m_piYieldModifier);
	SAFE_DELETE_ARRAY(m_piYieldFromAncientRuins);
	SAFE_DELETE_ARRAY(m_piYieldFromTRPlunder);
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piYieldFromKills);
	SAFE_DELETE_ARRAY(m_piYieldFromBarbarianKills);
	SAFE_DELETE_ARRAY(m_piYieldFromCombatExperienceTimes100);
	SAFE_DELETE_ARRAY(m_piGarrisonYield);
	SAFE_DELETE_ARRAY(m_piFortificationYield);
	SAFE_DELETE_ARRAY(m_piUnitCombatModifierPercent);
	SAFE_DELETE_ARRAY(m_piUnitCombatModifierPercentAttack);
	SAFE_DELETE_ARRAY(m_piUnitCombatModifierPercentDefense);
	SAFE_DELETE_ARRAY(m_piUnitClassModifierPercent);
	SAFE_DELETE_ARRAY(m_piUnitClassAttackModifier);
	SAFE_DELETE_ARRAY(m_piUnitClassDefenseModifier);
	SAFE_DELETE_ARRAY(m_piDomainModifierPercent);
	SAFE_DELETE_ARRAY(m_piDomainAttackPercent);
	SAFE_DELETE_ARRAY(m_piDomainDefensePercent);
	SAFE_DELETE_ARRAY(m_piFeaturePassableTech);
	SAFE_DELETE_ARRAY(m_piCombatModPerAdjacentUnitCombatModifierPercent);
	SAFE_DELETE_ARRAY(m_piCombatModPerAdjacentUnitCombatAttackModifier);
	SAFE_DELETE_ARRAY(m_piCombatModPerAdjacentUnitCombatDefenseModifier);
	SAFE_DELETE_ARRAY(m_pbIgnoreTerrainCostIn);
	SAFE_DELETE_ARRAY(m_pbIgnoreTerrainCostFrom);
	SAFE_DELETE_ARRAY(m_pbIgnoreFeatureCostIn);
	SAFE_DELETE_ARRAY(m_pbIgnoreFeatureCostFrom);
	SAFE_DELETE_ARRAY(m_pbTerrainDoubleMove);
	SAFE_DELETE_ARRAY(m_pbFeatureDoubleMove);
#if defined(MOD_PROMOTIONS_HALF_MOVE)
	SAFE_DELETE_ARRAY(m_pbTerrainHalfMove);
	SAFE_DELETE_ARRAY(m_pbTerrainExtraMove);
	SAFE_DELETE_ARRAY(m_pbFeatureHalfMove);
	SAFE_DELETE_ARRAY(m_pbFeatureExtraMove);
#endif
	SAFE_DELETE_ARRAY(m_pbTerrainDoubleHeal);
	SAFE_DELETE_ARRAY(m_pbFeatureDoubleHeal);
	SAFE_DELETE_ARRAY(m_pbTerrainImpassable);
	SAFE_DELETE_ARRAY(m_piTerrainPassableTech);
	SAFE_DELETE_ARRAY(m_pbFeatureImpassable);
	SAFE_DELETE_ARRAY(m_pbUnitCombat);
	SAFE_DELETE_ARRAY(m_pbCivilianUnitType);
#if defined(MOD_PROMOTIONS_UNIT_NAMING)
	SAFE_DELETE_ARRAY(m_pbUnitName);
#endif
	SAFE_DELETE_ARRAY(m_pbPostCombatRandomPromotion);
}
//------------------------------------------------------------------------------
bool CvPromotionEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvHotKeyInfo::CacheResults(kResults, kUtility))
		return false;

	SetSound(kResults.GetText("Sound"));

	//Basic Properties
	m_iCaptureDefeatedEnemyChance = kResults.GetInt("CaptureDefeatedEnemyChance");
	m_iBarbarianCombatBonus = kResults.GetInt("BarbarianCombatBonus");
	m_iGoodyHutYieldBonus = kResults.GetInt("GoodyHutYieldBonus");
	m_iDiploMissionInfluence = kResults.GetInt("DiploMissionInfluence");
	m_bGainsXPFromScouting = kResults.GetBool("GainsXPFromScouting");
	m_iXPFromPillaging = kResults.GetInt("XPFromPillaging");
	m_iExtraXPOnKill = kResults.GetInt("ExtraXPOnKill");
	m_bGainsXPFromSpotting = kResults.GetBool("GainsXPFromSpotting");
	m_bCannotBeCaptured = kResults.GetBool("CannotBeCaptured");
	m_bIsLostOnMove = kResults.GetBool("IsLostOnMove");
	m_bCityStateOnly = kResults.GetBool("CityStateOnly");
	m_bDiplomaticMissionAccomplishment = kResults.GetBool("DiplomaticMissionAccomplishment");
	m_bBarbarianOnly = kResults.GetBool("BarbarianOnly");
	m_bStrongerDamaged = kResults.GetBool("StrongerDamaged");
	m_bFightWellDamaged = kResults.GetBool("FightWellDamaged");
	m_bFreeAttackMoves = kResults.GetBool("FreeAttackMoves");
	const char* szNegatesPromotion = kResults.GetText("NegatesPromotion");
	m_iNegatesPromotion = GC.getInfoTypeForString(szNegatesPromotion, true);
	m_iForcedDamageValue = kResults.GetInt("ForcedDamageValue");
	m_iChangeDamageValue = kResults.GetInt("ChangeDamageValue");
	m_iDamageTakenMod = kResults.GetInt("DamageTakenMod");
	m_iInfluenceFromCombatXPTimes100 = kResults.GetInt("InfluenceFromCombatXpTimes100");
	m_iPromotionDuration = kResults.GetInt("PromotionDuration");
	m_iMoraleBreakChance = kResults.GetInt("MoraleBreakChance");
	m_iDamageAoEFortified = kResults.GetInt("AoEWhileFortified");
	m_iWorkRateMod = kResults.GetInt("WorkRateMod");
	m_bCannotBeChosen = kResults.GetBool("CannotBeChosen");
	m_bLostWithUpgrade = kResults.GetBool("LostWithUpgrade");
	m_bNotWithUpgrade = kResults.GetBool("NotWithUpgrade");
	m_bInstaHeal = kResults.GetBool("InstaHeal");
	m_bLeader = kResults.GetBool("Leader");
	m_bBlitz = kResults.GetBool("Blitz");
	m_bAmphib = kResults.GetBool("Amphib");
	m_bRiver = kResults.GetBool("River");
	m_bEnemyRoute = kResults.GetBool("EnemyRoute");
	m_bRivalTerritory = kResults.GetBool("RivalTerritory");
	m_bMustSetUpToRangedAttack = kResults.GetBool("MustSetUpToRangedAttack");
	m_bRangedSupportFire= kResults.GetBool("RangedSupportFire");
	m_bAlwaysHeal = kResults.GetBool("AlwaysHeal");
	m_bHealOutsideFriendly = kResults.GetBool("HealOutsideFriendly");
	m_bHillsDoubleMove = kResults.GetBool("HillsDoubleMove");
	m_bRiverDoubleMove = kResults.GetBool("RiverDoubleMove");
	m_bIgnoreTerrainCost = kResults.GetBool("IgnoreTerrainCost");
	m_bIgnoreTerrainDamage = kResults.GetBool("IgnoreTerrainDamage");
	m_bIgnoreFeatureDamage = kResults.GetBool("IgnoreFeatureDamage");
	m_bExtraTerrainDamage = kResults.GetBool("ExtraTerrainDamage");
	m_bExtraFeatureDamage = kResults.GetBool("ExtraFeatureDamage");
#if defined(MOD_PROMOTIONS_IMPROVEMENT_BONUS)
	if (MOD_PROMOTIONS_IMPROVEMENT_BONUS) {
		m_iNearbyImprovementCombatBonus = kResults.GetInt("NearbyImprovementCombatBonus");
		m_iNearbyImprovementBonusRange = kResults.GetInt("NearbyImprovementBonusRange");
		const char* szTextVal = kResults.GetText("CombatBonusImprovement");
		if (szTextVal) {
			m_eCombatBonusImprovement = (ImprovementTypes)GC.getInfoTypeForString(szTextVal, true);
		}
	}
#endif
	const char* szCombatBonusFromNearbyUnitClass = kResults.GetText("CombatBonusFromNearbyUnitClass");
	m_iCombatBonusFromNearbyUnitClass = (UnitClassTypes)GC.getInfoTypeForString(szCombatBonusFromNearbyUnitClass, true);
	m_iNearbyUnitClassBonusRange = kResults.GetInt("NearbyUnitClassBonusRange");
	m_iNearbyUnitClassBonus = kResults.GetInt("NearbyUnitClassBonus");
	m_iWonderProductionModifier = kResults.GetInt("WonderProductionModifier");
	m_iAOEDamageOnKill = kResults.GetInt("AOEDamageOnKill");
	m_iAOEDamageOnPillage = kResults.GetInt("AOEDamageOnPillage");
	m_iAOEHealOnPillage = kResults.GetInt("AoEHealOnPillage");
	m_iCombatModPerCSAlliance = kResults.GetInt("CombatModPerCSAlliance");
	m_iAttackModPerSamePromotionAttack = kResults.GetInt("AttackModPerSamePromotionAttack");
	m_iAttackModPerSamePromotionAttackCap = kResults.GetInt("AttackModPerSamePromotionAttackCap");
	m_iAoEDamageOnMove = kResults.GetInt("AoEDamageOnMove");
	m_iPartialHealOnPillage = kResults.GetInt("PartialHealOnPillage");
	m_iSplashDamage = kResults.GetInt("SplashDamage");
	m_iMinRange = kResults.GetInt("MinimumRangeRequired");
	m_iMaxRange = kResults.GetInt("MaximumRangeRequired");
	m_iMultiAttackBonus = kResults.GetInt("MultiAttackBonus");
	m_iLandAirDefenseValue = kResults.GetInt("LandAirDefenseBonus");
	m_iDamageReductionCityAssault = kResults.GetInt("DamageReductionCityAssault");
	m_bMountainsDoubleMove = kResults.GetBool("MountainsDoubleMove");
	m_bEmbarkFlatCost = kResults.GetBool("EmbarkFlatCost");
	m_bDisembarkFlatCost = kResults.GetBool("DisembarkFlatCost");
	m_bMountedOnly = kResults.GetBool("MountedOnly");
#if defined(MOD_PROMOTIONS_CROSS_MOUNTAINS)
	if (MOD_PROMOTIONS_CROSS_MOUNTAINS)
	{
		m_bCanCrossMountains = kResults.GetBool("CanCrossMountains");
	}
#endif
#if defined(MOD_PROMOTIONS_CROSS_OCEANS)
	m_bCanCrossOceans = kResults.GetBool("CanCrossOceans");
#endif
#if defined(MOD_PROMOTIONS_CROSS_ICE)
	if (MOD_PROMOTIONS_CROSS_ICE) {
		m_bCanCrossIce = kResults.GetBool("CanCrossIce");
	}
#endif
#if defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
	if (MOD_PROMOTIONS_GG_FROM_BARBARIANS) {
		m_bGGFromBarbarians = kResults.GetBool("GGFromBarbarians");
	}
#endif
	m_bRoughTerrainEndsTurn = kResults.GetBool("RoughTerrainEndsTurn");
	m_bCapturedUnitsConscripted = kResults.GetBool("CapturedUnitsConscripted");
	m_bHoveringUnit = kResults.GetBool("HoveringUnit");
	m_bFlatMovementCost = kResults.GetBool("FlatMovementCost");
	m_bCanMoveImpassable = kResults.GetBool("CanMoveImpassable");
	m_bNoCapture = kResults.GetBool("NoCapture");
	m_bOnlyDefensive = kResults.GetBool("OnlyDefensive");
	m_bNoAttackInOcean = kResults.GetBool("NoAttackInOcean");
	m_bNoDefensiveBonus = kResults.GetBool("NoDefensiveBonus");
	m_bNukeImmune = kResults.GetBool("NukeImmune");
	m_bHiddenNationality = kResults.GetBool("HiddenNationality");
	m_bAlwaysHostile = kResults.GetBool("AlwaysHostile");
	m_bNoRevealMap = kResults.GetBool("NoRevealMap");
	m_bRecon = kResults.GetBool("Recon");
	m_bCanMoveAllTerrain = kResults.GetBool("CanMoveAllTerrain");
	m_bCanMoveAfterAttacking = kResults.GetBool("CanMoveAfterAttacking");
	m_bAirSweepCapable = kResults.GetBool("AirSweepCapable");
	m_bAllowsEmbarkation = kResults.GetBool("AllowsEmbarkation");
	m_bRangeAttackIgnoreLOS = kResults.GetBool("RangeAttackIgnoreLOS");
	m_bFreePillageMoves = kResults.GetBool("FreePillageMoves");
	m_bHealOnPillage = kResults.GetBool("HealOnPillage");
	m_bHealIfDefeatExcludesBarbarians = kResults.GetBool("HealIfDestroyExcludesBarbarians");
	m_bEmbarkedAllWater = kResults.GetBool("EmbarkedAllWater");
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
	m_bEmbarkedDeepWater = kResults.GetBool("EmbarkedDeepWater");
#endif
	m_bCityAttackOnly = kResults.GetBool("CityAttackOnly");
	m_bCaptureDefeatedEnemy = kResults.GetBool("CaptureDefeatedEnemy");
	m_bIgnoreGreatGeneralBenefit = kResults.GetBool("IgnoreGreatGeneralBenefit");
	m_bIgnoreZOC = kResults.GetBool("IgnoreZOC");
	m_bHasPostCombatPromotions = kResults.GetBool("HasPostCombatPromotions");
	m_bPostCombatPromotionsExclusive = kResults.GetBool("PostCombatPromotionsExclusive");
	m_bSapper = kResults.GetBool("Sapper");
	m_iNearbyCityCombatMod = kResults.GetInt("NearbyCityCombatMod");
	m_iNearbyFriendlyCityCombatMod = kResults.GetInt("NearbyFriendlyCityCombatMod");
	m_iNearbyEnemyCityCombatMod = kResults.GetBool("NearbyEnemyCityCombatMod");
	m_bIsNearbyPromotion = kResults.GetBool("IsNearbyPromotion");
	m_iNearbyRange = kResults.GetInt("NearbyRange");
	const char* szConvertDomainUnit = kResults.GetText("ConvertDomainUnit");
	m_iConvertDomainUnit = (UnitTypes)GC.getInfoTypeForString(szConvertDomainUnit, true);
	const char* szConvertDomain = kResults.GetText("ConvertDomain");
	m_iConvertDomain = (DomainTypes)GC.getInfoTypeForString(szConvertDomain, true);
	m_bIsConvertUnit = kResults.GetBool("IsConvertUnit");
	m_bIsConvertEnemyUnitToBarbarian = kResults.GetBool("IsConvertEnemyUnitToBarbarian");
	m_bIsConvertOnFullHP = kResults.GetBool("IsConvertOnFullHP");
	m_bIsConvertOnDamage = kResults.GetBool("IsConvertOnDamage");
	m_iDamageThreshold = kResults.GetInt("DamageThreshold");
	const char* szConvertDamageOrFullHPUnit = kResults.GetText("ConvertDamageOrFullHPUnit");
	m_iConvertDamageOrFullHPUnit = (UnitTypes)GC.getInfoTypeForString(szConvertDamageOrFullHPUnit, true);
	m_iStackedGreatGeneralExperience = kResults.GetInt("StackedGreatGeneralXP");
	m_iPillageBonusStrength = kResults.GetInt("PillageBonusStrength");
	m_iReligiousPressureModifier = kResults.GetInt("ReligiousPressureModifier");
	m_iAdjacentCityDefenseMod = kResults.GetInt("AdjacentCityDefenseMod");
	m_iNearbyEnemyDamage = kResults.GetInt("NearbyEnemyDamage");
	m_iMilitaryProductionModifier = kResults.GetInt("MilitaryProductionModifier");
	m_iGeneralGoldenAgeExpPercent = kResults.GetInt("GeneralGoldenAgeExpPercent");
	m_iGiveCombatMod = kResults.GetInt("GiveCombatMod");
	m_iGiveHPHealedIfEnemyKilled = kResults.GetInt("GiveHPHealedIfEnemyKilled");
	m_iGiveExperiencePercent = kResults.GetInt("GiveExperiencePercent");
	m_iGiveOutsideFriendlyLandsModifier = kResults.GetInt("GiveOutsideFriendlyLandsModifier");
	const char* szGiveDomain = kResults.GetText("GiveDomain");
	m_iGiveDomain = (DomainTypes)GC.getInfoTypeForString(szGiveDomain, true);
	m_iGiveExtraAttacks = kResults.GetInt("GiveExtraAttacks");
	m_iGiveDefenseMod = kResults.GetInt("GiveDefenseMod");
	m_bGiveInvisibility = kResults.GetBool("GiveInvisibility");
	m_bGiveOnlyOnStartingTurn = kResults.GetBool("GiveOnlyOnStartingTurn");
	m_iNearbyHealEnemyTerritory = kResults.GetInt("NearbyHealEnemyTerritory");
	m_iNearbyHealNeutralTerritory = kResults.GetInt("NearbyHealNeutralTerritory");
	m_iNearbyHealFriendlyTerritory = kResults.GetInt("NearbyHealFriendlyTerritory");

	m_iAdjacentEnemySapMovement = kResults.GetInt("AdjacentEnemySapMovement");
	m_bCanHeavyCharge = kResults.GetBool("HeavyCharge");

	m_iVisibilityChange = kResults.GetInt("VisibilityChange");
#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
	m_iReconChange = kResults.GetInt("ReconChange");
#endif
	m_iMovesChange = kResults.GetInt("MovesChange");
	m_iMoveDiscountChange = kResults.GetInt("MoveDiscountChange");
	m_iRangeChange = kResults.GetInt("RangeChange");
	m_iRangedAttackModifier = kResults.GetInt("RangedAttackModifier");
	m_iInterceptionCombatModifier = kResults.GetInt("InterceptionCombatModifier");
	m_iInterceptionDefenseDamageModifier = kResults.GetInt("InterceptionDefenseDamageModifier");
	m_iAirSweepCombatModifier = kResults.GetInt("AirSweepCombatModifier");
	m_iInterceptChanceChange = kResults.GetInt("InterceptChanceChange");
	m_iNumInterceptionChange = kResults.GetInt("NumInterceptionChange");
	m_iAirInterceptRangeChange = kResults.GetInt("AirInterceptRangeChange"); // JJ: This is new
	m_iEvasionChange = kResults.GetInt("EvasionChange");
	m_iCargoChange = kResults.GetInt("CargoChange");
	m_iEnemyHealChange = kResults.GetInt("EnemyHealChange");
	m_iNeutralHealChange = kResults.GetInt("NeutralHealChange");
	m_iFriendlyHealChange = kResults.GetInt("FriendlyHealChange");
	m_iSameTileHealChange = kResults.GetInt("SameTileHealChange");
	m_iAdjacentTileHealChange = kResults.GetInt("AdjacentTileHealChange");
	m_iEnemyDamageChance = kResults.GetInt("EnemyDamageChance");
	m_iNeutralDamageChance = kResults.GetInt("NeutralDamageChance");
	m_iEnemyDamage = kResults.GetInt("EnemyDamage");
	m_iNeutralDamage = kResults.GetInt("NeutralDamage");
	m_iCombatPercent = kResults.GetInt("CombatPercent");
	m_iCityAttackPercent = kResults.GetInt("CityAttack");
	m_iCityDefensePercent = kResults.GetInt("CityDefense");
	m_iRangedDefenseMod = kResults.GetInt("RangedDefenseMod");
	m_iHillsAttackPercent = kResults.GetInt("HillsAttack");
	m_iHillsDefensePercent = kResults.GetInt("HillsDefense");
	m_iOpenAttackPercent = kResults.GetInt("OpenAttack");
	m_iOpenFromPercent = kResults.GetInt("OpenFromMod");
	m_iOpenRangedAttackMod = kResults.GetInt("OpenRangedAttackMod");
	m_iRoughAttackPercent = kResults.GetInt("RoughAttack");
	m_iRoughFromPercent = kResults.GetInt("RoughFromMod");
	m_iRoughRangedAttackMod = kResults.GetInt("RoughRangedAttackMod");
	m_iAttackFortifiedMod = kResults.GetInt("AttackFortifiedMod");
	m_iAttackWoundedMod = kResults.GetInt("AttackWoundedMod");
	m_iAttackFullyHealedMod = kResults.GetInt("AttackFullyHealedMod");
	m_iAttackAboveHealthMod = kResults.GetInt("AttackAbove50HealthMod");
	m_iAttackBelowHealthMod = kResults.GetInt("AttackBelowEqual50HealthMod");
	m_bRangedFlankAttack = kResults.GetBool("RangedFlankAttack");
	m_iExtraFlankPower = kResults.GetInt("ExtraFlankPower");
	m_iFlankAttackModifier = kResults.GetInt("FlankAttackModifier");
	m_iNearbyEnemyCombatMod = kResults.GetInt("NearbyEnemyCombatMod");
	m_iNearbyEnemyCombatRange = kResults.GetInt("NearbyEnemyCombatRange");
	m_iOpenDefensePercent = kResults.GetInt("OpenDefense");
	m_iRoughDefensePercent = kResults.GetInt("RoughDefense");
	m_iExtraAttacks = kResults.GetInt("ExtraAttacks");
	m_bGreatGeneral = kResults.GetBool("GreatGeneral");
	m_bGreatAdmiral = kResults.GetBool("GreatAdmiral");
	m_iAuraRangeChange = kResults.GetInt("AuraRangeChange");
	m_iAuraEffectChange = kResults.GetInt("AuraEffectChange");
	m_iNumRepairCharges = kResults.GetInt("NumRepairCharges");
	m_iMilitaryCapChange = kResults.GetInt("MilitaryCapChange");
	m_iGreatGeneralModifier = kResults.GetInt("GreatGeneralModifier");
	m_bGreatGeneralReceivesMovement = kResults.GetBool("GreatGeneralReceivesMovement");
	m_iGreatGeneralCombatModifier = kResults.GetInt("GreatGeneralCombatModifier");
	m_iFriendlyLandsModifier = kResults.GetInt("FriendlyLandsModifier");
	m_iFriendlyLandsAttackModifier = kResults.GetInt("FriendlyLandsAttackModifier");
	m_iOutsideFriendlyLandsModifier = kResults.GetInt("OutsideFriendlyLandsModifier");
	m_bNoSupply = kResults.GetBool("NoSupply");
	m_iMaxHitPointsChange = kResults.GetInt("MaxHitPointsChange");
	m_iMaxHitPointsModifier = kResults.GetInt("MaxHitPointsModifier");
	m_iVsUnhappyMod = kResults.GetInt("VsUnhappyMod");
	m_iUpgradeDiscount = kResults.GetInt("UpgradeDiscount");
	m_iExperiencePercent = kResults.GetInt("ExperiencePercent");
	m_iAdjacentMod = kResults.GetInt("AdjacentMod");
	m_iNoAdjacentUnitMod = kResults.GetInt("NoAdjacentUnitMod");
	m_iAttackMod = kResults.GetInt("AttackMod");
	m_iDefenseMod = kResults.GetInt("DefenseMod");
	m_iBorderMod = kResults.GetInt("BorderMod");
	m_iMarriageMod = kResults.GetInt("MarriageMod");
	m_iMarriageModCap = kResults.GetInt("MarriageModCap");
	m_iGetGroundAttackDamage = kResults.GetInt("GetGroundAttackDamage");
	m_iDropRange = kResults.GetInt("DropRange");
	m_iExtraNavalMoves = kResults.GetInt("ExtraNavalMovement");
	m_iHPHealedIfDefeatEnemy = kResults.GetInt("HPHealedIfDestroyEnemy");
	m_iGoldenAgeValueFromKills = kResults.GetInt("GoldenAgeValueFromKills");
	m_iExtraWithdrawal = kResults.GetInt("ExtraWithdrawal");
	m_iCombatChange = kResults.GetInt("CombatChange");
	m_iTileDamageIfNotMoved = kResults.GetInt("TileDamageIfNotMoved");
	m_iFortifiedModifier = kResults.GetInt("FortifiedModifier");
	m_iMinEffectiveHealth = kResults.GetInt("MinEffectiveHealth");
	m_bRequiresLeadership = kResults.GetBool("RequiresLeadership");
	m_bCannotHeal = kResults.GetBool("CannotHeal");
	m_bPillageFortificationsOnKill = kResults.GetBool("PillageFortificationsOnKill");
#if defined(MOD_BALANCE_CORE_JFD)


	m_iPlagueID = kResults.GetInt("PlagueID");
	m_iPlaguePriority = kResults.GetInt("PlaguePriority");
#endif
	m_iEmbarkExtraVisibility = kResults.GetInt("EmbarkExtraVisibility");
	m_iEmbarkDefenseModifier = kResults.GetInt("EmbarkDefenseModifier");
	m_iCapitalDefenseModifier = kResults.GetInt("CapitalDefenseModifier");
	m_iCapitalDefenseFalloff = kResults.GetInt("CapitalDefenseFalloff");
	m_iCapitalDefenseLimit = kResults.GetInt("CapitalDefenseLimit");
	m_iCityAttackPlunderModifier = kResults.GetInt("CityAttackPlunderModifier");
	m_iReligiousStrengthLossRivalTerritory = kResults.GetInt("ReligiousStrengthLossRivalTerritory");
	m_iTradeMissionInfluenceModifier = kResults.GetInt("TradeMissionInfluenceModifier");
	m_iTradeMissionGoldModifier = kResults.GetInt("TradeMissionGoldModifier");
	m_iCombatModPerLevel = kResults.GetInt("CombatModPerLevel");

	//References
	const char* szLayerAnimationPath = kResults.GetText("LayerAnimationPath");
	m_iLayerAnimationPath = GC.getInfoTypeForString(szLayerAnimationPath, true);

	const char* szTechPrereq = kResults.GetText("TechPrereq");
	m_iTechPrereq = GC.getInfoTypeForString(szTechPrereq, true);

	const char* szInvisible = kResults.GetText("Invisible");
	m_iInvisibleType = GC.getInfoTypeForString(szInvisible, true);

	const char* szSeeInvisible = kResults.GetText("SeeInvisible");
	m_iSeeInvisibleType = GC.getInfoTypeForString(szSeeInvisible, true);

	const char* szPromotionPrereq = kResults.GetText("PromotionPrereq");
	m_iPrereqPromotion = GC.getInfoTypeForString(szPromotionPrereq, true);

	const char* szPromotionPrereqOr1 = kResults.GetText("PromotionPrereqOr1");
	m_iPrereqOrPromotion1 = GC.getInfoTypeForString(szPromotionPrereqOr1, true);

	const char* szPromotionPrereqOr2 = kResults.GetText("PromotionPrereqOr2");
	m_iPrereqOrPromotion2 = GC.getInfoTypeForString(szPromotionPrereqOr2, true);

	const char* szPromotionPrereqOr3 = kResults.GetText("PromotionPrereqOr3");
	m_iPrereqOrPromotion3 = GC.getInfoTypeForString(szPromotionPrereqOr3, true);

	const char* szPromotionPrereqOr4 = kResults.GetText("PromotionPrereqOr4");
	m_iPrereqOrPromotion4 = GC.getInfoTypeForString(szPromotionPrereqOr4, true);

	const char* szPromotionPrereqOr5 = kResults.GetText("PromotionPrereqOr5");
	m_iPrereqOrPromotion5 = GC.getInfoTypeForString(szPromotionPrereqOr5, true);

	const char* szPromotionPrereqOr6 = kResults.GetText("PromotionPrereqOr6");
	m_iPrereqOrPromotion6 = GC.getInfoTypeForString(szPromotionPrereqOr6, true);

	const char* szPromotionPrereqOr7 = kResults.GetText("PromotionPrereqOr7");
	m_iPrereqOrPromotion7 = GC.getInfoTypeForString(szPromotionPrereqOr7, true);

	const char* szPromotionPrereqOr8 = kResults.GetText("PromotionPrereqOr8");
	m_iPrereqOrPromotion8 = GC.getInfoTypeForString(szPromotionPrereqOr8, true);

	const char* szPromotionPrereqOr9 = kResults.GetText("PromotionPrereqOr9");
	m_iPrereqOrPromotion9 = GC.getInfoTypeForString(szPromotionPrereqOr9, true);

	//Arrays
	const int iNumUnitClasses = kUtility.MaxRows("UnitClasses");
	const int iNumTerrains = GC.getNumTerrainInfos();
	const int iNumFeatures = GC.getNumFeatureInfos();
	const int iNumDomains = kUtility.MaxRows("Domains");
	DEBUG_VARIABLE(iNumDomains);
	const int iNumUnitCombatClasses = kUtility.MaxRows("UnitCombatInfos");
	const int iNumUnitTypes = kUtility.MaxRows("Units");

	const char* szPromotionType = GetType();

	//UnitPromotions_Terrains
	{
		kUtility.InitializeArray(m_piTerrainAttackPercent, iNumTerrains, 0);
		kUtility.InitializeArray(m_piTerrainDefensePercent, iNumTerrains, 0);
		kUtility.InitializeArray(m_piTerrainModifierAttack, iNumTerrains, 0);
		kUtility.InitializeArray(m_piTerrainModifierDefense, iNumTerrains, 0);
		kUtility.InitializeArray(m_piTerrainPassableTech, iNumTerrains, NO_TECH);
		kUtility.InitializeArray(m_pbIgnoreTerrainCostIn, iNumTerrains, false);
		kUtility.InitializeArray(m_pbIgnoreTerrainCostFrom, iNumTerrains, false);
		kUtility.InitializeArray(m_pbTerrainDoubleMove, iNumTerrains, false);
#if defined(MOD_PROMOTIONS_HALF_MOVE)
		kUtility.InitializeArray(m_pbTerrainHalfMove, iNumTerrains, false);
		kUtility.InitializeArray(m_pbTerrainExtraMove, iNumTerrains, false);
#endif
		kUtility.InitializeArray(m_pbTerrainDoubleHeal, iNumTerrains, false);
		kUtility.InitializeArray(m_pbTerrainImpassable, iNumTerrains, false);

		std::string sqlKey = "UnitPromotions_Terrains";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select Terrains.ID as TerrainID, coalesce(Technologies.ID, -1) as PassableTechID, UnitPromotions_Terrains.* from UnitPromotions_Terrains inner join Terrains on TerrainType = Terrains.Type left join Technologies on PassableTech = Technologies.Type where PromotionType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		ASSERT(pResults);
		if(!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while(pResults->Step())
		{
			const int iTerrainID = pResults->GetInt("TerrainID");
			PRECONDITION(iTerrainID > -1 && iTerrainID < iNumTerrains);

			const int iTerrainAttack = pResults->GetInt("Attack");
			m_piTerrainAttackPercent[iTerrainID] = iTerrainAttack;

			const int iTerrainDefense = pResults->GetInt("Defense");
			m_piTerrainDefensePercent[iTerrainID] = iTerrainDefense;

			const bool bIgnoreTerrainCostIn = pResults->GetBool("IgnoreTerrainCostIn");
			m_pbIgnoreTerrainCostIn[iTerrainID] = bIgnoreTerrainCostIn;

			const bool bIgnoreTerrainCostFrom = pResults->GetBool("IgnoreTerrainCostFrom");
			m_pbIgnoreTerrainCostFrom[iTerrainID] = bIgnoreTerrainCostFrom;

			const bool bDoubleMove = pResults->GetBool("DoubleMove");
			m_pbTerrainDoubleMove[iTerrainID] = bDoubleMove;

#if defined(MOD_PROMOTIONS_HALF_MOVE)
			const bool bHalfMove = pResults->GetBool("HalfMove");
			m_pbTerrainHalfMove[iTerrainID] = bHalfMove;

			const bool bExtraMove = pResults->GetBool("ExtraMove");
			m_pbTerrainExtraMove[iTerrainID] = bExtraMove;
#endif
			const bool bDoubleHeal = pResults->GetBool("DoubleHeal");
			m_pbTerrainDoubleHeal[iTerrainID] = bDoubleHeal;

			const bool bImpassable = pResults->GetBool("Impassable");
			m_pbTerrainImpassable[iTerrainID] = bImpassable;

			const int iPassableTechID = pResults->GetInt("PassableTechID");
			m_piTerrainPassableTech[iTerrainID] = iPassableTechID;
		}
	}

	//UnitPromotions_TerrainModifiers
	{
		kUtility.InitializeArray(m_piTerrainModifierAttack, iNumTerrains, 0);
		kUtility.InitializeArray(m_piTerrainModifierDefense, iNumTerrains, 0);

		std::string sqlKey = "UnitPromotions_TerrainModifiers";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if (pResults == NULL)
		{
			const char* szSQL = "select Terrains.ID as TerrainID, UnitPromotions_TerrainModifiers.Attack, UnitPromotions_TerrainModifiers.Defense from UnitPromotions_TerrainModifiers inner join Terrains on TerrainType = Terrains.Type where PromotionType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		ASSERT(pResults);
		if (!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while (pResults->Step())
		{
			const int iTerrainID = pResults->GetInt("TerrainID");
			PRECONDITION(iTerrainID > -1 && iTerrainID < iNumTerrains);

			const int iTerrainAttack = pResults->GetInt("Attack");
			m_piTerrainModifierAttack[iTerrainID] = iTerrainAttack;

			const int iTerrainDefense = pResults->GetInt("Defense");
			m_piTerrainModifierDefense[iTerrainID] = iTerrainDefense;
		}
	}

	//UnitPromotions_Features
	{
		kUtility.InitializeArray(m_piFeatureAttackPercent, iNumFeatures, 0);
		kUtility.InitializeArray(m_piFeatureDefensePercent, iNumFeatures, 0);
		kUtility.InitializeArray(m_piFeaturePassableTech, iNumFeatures, NO_TECH);
		kUtility.InitializeArray(m_pbIgnoreFeatureCostIn, iNumFeatures, false);
		kUtility.InitializeArray(m_pbIgnoreFeatureCostFrom, iNumFeatures, false);
		kUtility.InitializeArray(m_pbFeatureDoubleMove, iNumFeatures, false);
#if defined(MOD_PROMOTIONS_HALF_MOVE)
		kUtility.InitializeArray(m_pbFeatureHalfMove, iNumFeatures, false);
		kUtility.InitializeArray(m_pbFeatureExtraMove, iNumFeatures, false);
#endif
		kUtility.InitializeArray(m_pbFeatureDoubleHeal, iNumFeatures, false);
		kUtility.InitializeArray(m_pbFeatureImpassable, iNumFeatures, false);

		std::string sqlKey = "UnitPromotions_Features";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select Features.ID as FeatureID, coalesce(Technologies.ID, -1) as PassableTechID, UnitPromotions_Features.* from UnitPromotions_Features inner join Features on FeatureType = Features.Type left join Technologies on PassableTech = Technologies.Type where PromotionType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		ASSERT(pResults);
		if(!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while(pResults->Step())
		{
			const int iFeatureID = pResults->GetInt("FeatureID");
			PRECONDITION(iFeatureID > -1 && iFeatureID < iNumFeatures);

			const int iFeatureAttack = pResults->GetInt("Attack");
			m_piFeatureAttackPercent[iFeatureID] = iFeatureAttack;

			const int iFeatureDefense = pResults->GetInt("Defense");
			m_piFeatureDefensePercent[iFeatureID] = iFeatureDefense;

			const bool bIgnoreTerrainCostIn = pResults->GetBool("IgnoreTerrainCostIn");
			m_pbIgnoreFeatureCostIn[iFeatureID] = bIgnoreTerrainCostIn;

			const bool bIgnoreTerrainCostFrom = pResults->GetBool("IgnoreTerrainCostFrom");
			m_pbIgnoreFeatureCostFrom[iFeatureID] = bIgnoreTerrainCostFrom;

			const bool bDoubleMove = pResults->GetBool("DoubleMove");
			m_pbFeatureDoubleMove[iFeatureID] = bDoubleMove;

#if defined(MOD_PROMOTIONS_HALF_MOVE)
			const bool bHalfMove = pResults->GetBool("HalfMove");
			m_pbFeatureHalfMove[iFeatureID] = bHalfMove;

			const bool bExtraMove = pResults->GetBool("ExtraMove");
			m_pbFeatureExtraMove[iFeatureID] = bExtraMove;
#endif
			const bool bDoubleHeal = pResults->GetBool("DoubleHeal");
			m_pbFeatureDoubleHeal[iFeatureID] = bDoubleHeal;

			const bool bImpassable = pResults->GetBool("Impassable");
			m_pbFeatureImpassable[iFeatureID] = bImpassable;

			const int iPassableTech = pResults->GetInt("PassableTechID");
			m_piFeaturePassableTech[iFeatureID] = iPassableTech;
		}
	}
	kUtility.SetYields(m_piYieldModifier, "UnitPromotions_YieldModifiers", "PromotionType", szPromotionType);
	kUtility.SetYields(m_piYieldChange, "UnitPromotions_YieldChanges", "PromotionType", szPromotionType);
	kUtility.SetYields(m_piYieldFromAncientRuins, "UnitPromotions_YieldFromAncientRuins", "PromotionType", szPromotionType);
	kUtility.SetYields(m_piYieldFromTRPlunder, "UnitPromotions_YieldFromTRPlunder", "PromotionType", szPromotionType);

	//UnitPromotions_YieldFromScouting
	{
		kUtility.InitializeArray(m_piYieldFromScouting, NUM_YIELD_TYPES, 0);

		std::string sqlKey = "UnitPromotions_YieldFromScouting";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select Yields.ID as YieldID, UnitPromotions_YieldFromScouting.* from UnitPromotions_YieldFromScouting inner join Yields on YieldType = Yields.Type where PromotionType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		ASSERT(pResults);
		if(!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while(pResults->Step())
		{
			const int iYieldID = pResults->GetInt("YieldID");
			PRECONDITION(iYieldID > -1 && iYieldID < NUM_YIELD_TYPES);

			const int iYield = pResults->GetInt("Yield");
			m_piYieldFromScouting[iYieldID] = iYield;
		}
	}
	//UnitPromotions_YieldFromKills
	{
		kUtility.InitializeArray(m_piYieldFromKills, NUM_YIELD_TYPES, 0);

		std::string sqlKey = "UnitPromotions_YieldFromKills";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select Yields.ID as YieldID, UnitPromotions_YieldFromKills.* from UnitPromotions_YieldFromKills inner join Yields on YieldType = Yields.Type where PromotionType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		ASSERT(pResults);
		if(!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while(pResults->Step())
		{
			const int iYieldID = pResults->GetInt("YieldID");
			PRECONDITION(iYieldID > -1 && iYieldID < NUM_YIELD_TYPES);

			const int iYield = pResults->GetInt("Yield");
			m_piYieldFromKills[iYieldID] = iYield;
		}
	}
	//UnitPromotions_YieldFromCombatExperienceTimes100
	{
		kUtility.InitializeArray(m_piYieldFromCombatExperienceTimes100, NUM_YIELD_TYPES, 0);

		std::string sqlKey = "UnitPromotions_YieldFromCombatExperienceTimes100";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select Yields.ID as YieldID, UnitPromotions_YieldFromCombatExperienceTimes100.* from UnitPromotions_YieldFromCombatExperienceTimes100 inner join Yields on YieldType = Yields.Type where PromotionType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		ASSERT(pResults);
		if(!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while(pResults->Step())
		{
			const int iYieldID = pResults->GetInt("YieldID");
			PRECONDITION(iYieldID > -1 && iYieldID < NUM_YIELD_TYPES);

			const int iYield = pResults->GetInt("Yield");
			m_piYieldFromCombatExperienceTimes100[iYieldID] = iYield;
		}
	}
	//UnitPromotions_GarrisonYield
	{
		kUtility.InitializeArray(m_piGarrisonYield, NUM_YIELD_TYPES, 0);

		std::string sqlKey = "UnitPromotions_GarrisonYield";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select Yields.ID as YieldID, UnitPromotions_GarrisonYield.* from UnitPromotions_GarrisonYield inner join Yields on YieldType = Yields.Type where PromotionType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		ASSERT(pResults);
		if(!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while(pResults->Step())
		{
			const int iYieldID = pResults->GetInt("YieldID");
			PRECONDITION(iYieldID > -1 && iYieldID < NUM_YIELD_TYPES);

			const int iYield = pResults->GetInt("Yield");
			m_piGarrisonYield[iYieldID] = iYield;
		}
	}

	//UnitPromotions_FortificationYield
	{
		kUtility.InitializeArray(m_piFortificationYield, NUM_YIELD_TYPES, 0);

		std::string sqlKey = "UnitPromotions_FortificationYield";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if (pResults == NULL)
		{
			const char* szSQL = "select Yields.ID as YieldID, UnitPromotions_FortificationYield.* from UnitPromotions_FortificationYield inner join Yields on YieldType = Yields.Type where PromotionType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		ASSERT(pResults);
		if (!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while (pResults->Step())
		{
			const int iYieldID = pResults->GetInt("YieldID");
			PRECONDITION(iYieldID > -1 && iYieldID < NUM_YIELD_TYPES);

			const int iYield = pResults->GetInt("Yield");
			m_piFortificationYield[iYieldID] = iYield;
		}
	}

	//UnitPromotions_YieldFromBarbarianKills
	{
		kUtility.InitializeArray(m_piYieldFromBarbarianKills, NUM_YIELD_TYPES, 0);

		std::string sqlKey = "UnitPromotions_YieldFromBarbarianKills";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select Yields.ID as YieldID, UnitPromotions_YieldFromBarbarianKills.* from UnitPromotions_YieldFromBarbarianKills inner join Yields on YieldType = Yields.Type where PromotionType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		ASSERT(pResults);
		if(!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while(pResults->Step())
		{
			const int iYieldID = pResults->GetInt("YieldID");
			PRECONDITION(iYieldID > -1 && iYieldID < NUM_YIELD_TYPES);

			const int iYield = pResults->GetInt("Yield");
			m_piYieldFromBarbarianKills[iYieldID] = iYield;
		}
	}

	//UnitPromotions_UnitClasses
	{
		kUtility.InitializeArray(m_piUnitClassModifierPercent, iNumUnitClasses, 0);
		kUtility.InitializeArray(m_piUnitClassAttackModifier, iNumUnitClasses, 0);
		kUtility.InitializeArray(m_piUnitClassDefenseModifier, iNumUnitClasses, 0);

		std::string sqlKey = "UnitPromotions_UnitClasses";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select UnitClasses.ID, Modifier, Attack, Defense from UnitPromotions_UnitClasses inner join UnitClasses on UnitClassType = UnitClasses.Type where PromotionType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		ASSERT(pResults);
		if(!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while(pResults->Step())
		{
			const int iUnitClassID = pResults->GetInt(0);
			PRECONDITION(iUnitClassID > -1 && iUnitClassID  < iNumUnitClasses);

			const int iModifier = pResults->GetInt("Modifier");
			m_piUnitClassModifierPercent[iUnitClassID] = iModifier;

			const int iAttack = pResults->GetInt("Attack");
			m_piUnitClassAttackModifier[iUnitClassID] = iAttack;

			const int iDefense = pResults->GetInt("Defense");
			m_piUnitClassDefenseModifier[iUnitClassID] = iDefense;
		}

		pResults->Reset();
	}

	//UnitPromotions_CombatModPerAdjacentUnitCombat
	{
		kUtility.InitializeArray(m_piCombatModPerAdjacentUnitCombatModifierPercent, iNumUnitCombatClasses, 0);
		kUtility.InitializeArray(m_piCombatModPerAdjacentUnitCombatAttackModifier, iNumUnitCombatClasses, 0);
		kUtility.InitializeArray(m_piCombatModPerAdjacentUnitCombatDefenseModifier, iNumUnitCombatClasses, 0);

		std::string sqlKey = "UnitPromotions_CombatModPerAdjacentUnitCombat";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select UnitCombatInfos.ID, Modifier, Attack, Defense from UnitPromotions_CombatModPerAdjacentUnitCombat inner join UnitCombatInfos on UnitCombatType = UnitCombatInfos.Type where PromotionType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		ASSERT(pResults);
		if(!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while(pResults->Step())
		{
			const int iUnitCombatID = pResults->GetInt(0);
			PRECONDITION(iUnitCombatID > -1 && iUnitCombatID  < iNumUnitCombatClasses);

			const int iModifier = pResults->GetInt("Modifier");
			m_piCombatModPerAdjacentUnitCombatModifierPercent[iUnitCombatID] = iModifier;

			const int iAttack = pResults->GetInt("Attack");
			m_piCombatModPerAdjacentUnitCombatAttackModifier[iUnitCombatID] = iAttack;

			const int iDefense = pResults->GetInt("Defense");
			m_piCombatModPerAdjacentUnitCombatDefenseModifier[iUnitCombatID] = iDefense;
		}

		pResults->Reset();
	}

	//UnitPromotions_InstantYield
	{
		std::string sqlKey = "UnitPromotions_InstantYields";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if (pResults == NULL)
		{
			const char* szSQL = "select Yields.ID as YieldsID, Yield, IsEraScaling from UnitPromotions_InstantYields inner join Yields on Yields.Type = YieldType where PromotionType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szPromotionType);

		while (pResults->Step())
		{
			const int yieldID = pResults->GetInt(0);
			const int yield = pResults->GetInt(1);
			const bool eraScaling = pResults->GetBool(2);
			
			m_piInstantYields.insert(std::make_pair(yieldID, std::make_pair(yield, eraScaling)));
		}

		pResults->Reset();

		//Trim extra memory off container since this is mostly read-only.
		std::map<int, std::pair<int, bool>>(m_piInstantYields).swap(m_piInstantYields);
	}

	//UnitPromotions_Domains
	{
		kUtility.InitializeArray(m_piDomainModifierPercent, NUM_DOMAIN_TYPES, 0);
		kUtility.InitializeArray(m_piDomainAttackPercent, NUM_DOMAIN_TYPES, 0);
		kUtility.InitializeArray(m_piDomainDefensePercent, NUM_DOMAIN_TYPES, 0);

		std::string sqlKey = "UnitPromotions_Domains";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select Domains.ID, Modifier, Attack, Defense from UnitPromotions_Domains inner join Domains on DomainType = Domains.Type where PromotionType = ?;";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		ASSERT(pResults);
		if(!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while(pResults->Step())
		{
			const int iDomainID = pResults->GetInt(0);
			PRECONDITION(iDomainID > -1 && iDomainID < iNumDomains);

			const int iModifier = pResults->GetInt("Modifier");
			if (iDomainID > -1 && iDomainID < NUM_DOMAIN_TYPES)
				m_piDomainModifierPercent[iDomainID] = iModifier;

			const int iAttack = pResults->GetInt("Attack");
			if (iDomainID > -1 && iDomainID < NUM_DOMAIN_TYPES)
				m_piDomainAttackPercent[iDomainID] = iAttack;

			const int iDefense = pResults->GetInt("Defense");
			if (iDomainID > -1 && iDomainID < NUM_DOMAIN_TYPES)
				m_piDomainDefensePercent[iDomainID] = iDefense;
		}

		pResults->Reset();
	}

	//UnitPromotions_UnitCombatMods
	{
		kUtility.InitializeArray(m_piUnitCombatModifierPercent, iNumUnitCombatClasses, 0);
		kUtility.InitializeArray(m_piUnitCombatModifierPercentAttack, iNumUnitCombatClasses, 0);
		kUtility.InitializeArray(m_piUnitCombatModifierPercentDefense, iNumUnitCombatClasses, 0);

		std::string sqlKey = "m_piUnitCombatModifierPercent";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select UnitCombatInfos.ID, Modifier, Attack, Defense from UnitPromotions_UnitCombatMods inner join UnitCombatInfos on UnitCombatInfos.Type = UnitCombatType where PromotionType = ?;";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		ASSERT(pResults);
		if(!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while(pResults->Step())
		{
			const int iUnitCombatID = pResults->GetInt(0);
			PRECONDITION(iUnitCombatID > -1 && iUnitCombatID < iNumUnitCombatClasses);

			const int iUnitCombatMod = pResults->GetInt(1);
			m_piUnitCombatModifierPercent[iUnitCombatID] = iUnitCombatMod;

			const int iUnitCombatModAttack = pResults->GetInt(2);
			m_piUnitCombatModifierPercentAttack[iUnitCombatID] = iUnitCombatModAttack;

			const int iUnitCombatModDefense = pResults->GetInt(3);
			m_piUnitCombatModifierPercentDefense[iUnitCombatID] = iUnitCombatModDefense;
		}

		pResults->Reset();
	}

	//UnitPromotions_UnitCombats
	{
		kUtility.InitializeArray(m_pbUnitCombat, iNumUnitCombatClasses, false);

		std::string sqlKey = "m_pbUnitCombat";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select UnitCombatInfos.ID from UnitPromotions_UnitCombats inner join UnitCombatInfos On UnitCombatInfos.Type = UnitCombatType where PromotionType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		ASSERT(pResults);
		if(!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while(pResults->Step())
		{
			const int iUnitCombatInfoID = pResults->GetInt(0);
			ASSERT(iUnitCombatInfoID < iNumUnitCombatClasses);

			m_pbUnitCombat[iUnitCombatInfoID] = true;
		}

		pResults->Reset();
	}

	//UnitPromotions_CivilianUnitType
	{
		kUtility.InitializeArray(m_pbCivilianUnitType, iNumUnitTypes, false);

		std::string sqlKey = "m_pbCivilianUnitType";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select Units.ID from UnitPromotions_CivilianUnitType inner join Units On Units.Type = UnitType where PromotionType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		ASSERT(pResults);
		if(!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while(pResults->Step())
		{
			const int iUnit = (UnitTypes)pResults->GetInt(0);
			ASSERT(iUnit < iNumUnitTypes);

			m_pbCivilianUnitType[iUnit] = true;
		}

		pResults->Reset();
	}

	//UnitPromotions_BlockedPromotions
	{
		m_siBlockedPromotions.clear();

		std::string sqlKey = "UnitPromotions_BlockedPromotions";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if (pResults == NULL)
		{
			const char* szSQL = "select UnitPromotions.ID from UnitPromotions_BlockedPromotions inner join UnitPromotions On UnitPromotions.Type = BlockedPromotionType where UnitPromotions_BlockedPromotions.PromotionType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		ASSERT(pResults);
		if (!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while (pResults->Step())
		{
			const int iBlockedPromotion = pResults->GetInt(0);

			m_siBlockedPromotions.insert(iBlockedPromotion);
		}

		pResults->Reset();
	}

	//UnitPromotions_Plagues
	{
		m_vsPlagues.clear();

		std::string sqlKey = "UnitPromotions_Plagues";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if (pResults == NULL)
		{
			const char* szSQL = "select UnitPromotions.ID as PromotionID, Domains.ID as DomainID, ApplyOnAttack, ApplyOnDefense, ApplyChance from UnitPromotions_Plagues inner join UnitPromotions On UnitPromotions.Type = PlaguePromotionType inner join Domains on DomainType = Domains.Type where UnitPromotions_Plagues.PromotionType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		ASSERT(pResults);
		if (!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while (pResults->Step())
		{
			PlagueInfo plagueInfo;

			plagueInfo.ePlague = (PromotionTypes)pResults->GetInt(0);
			plagueInfo.eDomain = (DomainTypes)pResults->GetInt(1);
			plagueInfo.bApplyOnAttack = pResults->GetBool(2);
			plagueInfo.bApplyOnDefense = pResults->GetBool(3);
			plagueInfo.iApplyChance = pResults->GetInt(4);

			m_vsPlagues.push_back(plagueInfo);
		}

		pResults->Reset();
	}



	//UnitPromotions_YieldFromPillage
	{
		std::string sqlKey = "UnitPromotions_YieldFromPillage";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if (pResults == NULL)
		{
			const char* szSQL = "select Yields.ID as YieldsID, Yield, YieldNoScale from UnitPromotions_YieldFromPillage inner join Yields on Yields.Type = YieldType where PromotionType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		pResults->Bind(1, szPromotionType);

		while (pResults->Step())
		{
			// Note:
			// * yieldFlat: row[2], pair.first
			// * yieldEraScaled: row[1], pair.second
			// 
			// The columns should still be loaded in the order that they appear as it _may_ have an impact on cache performance.
			// Once the values are on the stack the access order is likely non-impactful since the compiler will re-order things
			// to the best of its ability and often try to keep the values loaded into registers.
			// 
			// This is a nano-optimization but it's still the correct thing to do :P
			const int yieldID = pResults->GetInt(0);
			const int yieldEraScaled = pResults->GetInt(1);
			const int yieldFlat = pResults->GetInt(2);

			if (yieldFlat != 0 || yieldEraScaled != 0)
			{
				const std::map<int, std::pair<int, int>>::iterator it = m_yieldFromPillage.find(yieldID);
				if (it != m_yieldFromPillage.end())
				{
					std::pair<int, int>& yieldValues = it->second;
					yieldValues.first += yieldFlat;
					yieldValues.second += yieldEraScaled;

					if (yieldValues.first == 0 && yieldValues.second == 0)
					{
						m_yieldFromPillage.erase(it);
					}
				}
				else
				{
					const std::pair<int, int> yieldValues = std::make_pair(yieldFlat, yieldEraScaled);
					m_yieldFromPillage.insert(it, std::make_pair(yieldID, yieldValues));
				}
			}
		}

		pResults->Reset();

		//Trim extra memory off container since this is mostly read-only.
		std::map<int, std::pair<int, int>>(m_yieldFromPillage).swap(m_yieldFromPillage);
	}

#if defined(MOD_PROMOTIONS_UNIT_NAMING)
	if (MOD_PROMOTIONS_UNIT_NAMING)
	{
		// We don't store the unit names, only if the promotion gives a name, we will look-up the actual name as it's needed
		kUtility.PopulateArrayByExistence(m_pbUnitName,
			"Units",
			"UnitPromotions_UnitName",
			"UnitType",
			"PromotionType",
			szPromotionType);
	}
#endif

	kUtility.PopulateArrayByExistence(m_pbPostCombatRandomPromotion,
		"UnitPromotions",
		"UnitPromotions_PostCombatRandomPromotion",
		"NewPromotion",
		"PromotionType",
		szPromotionType);

	return true;
}

/// Accessor: The granny animation index used to indicate that a unit has leveled up
int CvPromotionEntry::GetLayerAnimationPath() const
{
	return m_iLayerAnimationPath;
}

/// Accessor: The promotion required before this promotion is available
int CvPromotionEntry::GetPrereqPromotion() const
{
	return m_iPrereqPromotion;
}

/// Set the promotion required before this promotion is available
void CvPromotionEntry::SetPrereqPromotion(int i)
{
	m_iPrereqPromotion = i;
}

/// Accessor: Gets promotion 1 of an either/or promotion prerequisite.
int CvPromotionEntry::GetPrereqOrPromotion1() const
{
	return m_iPrereqOrPromotion1;
}

/// Accessor: Sets promotion 1 of an either/or promotion prerequisite.
void CvPromotionEntry::SetPrereqOrPromotion1(int i)
{
	m_iPrereqOrPromotion1 = i;
}

/// Accessor: Gets promotion 2 of an either/or promotion prerequisite.
int CvPromotionEntry::GetPrereqOrPromotion2() const
{
	return m_iPrereqOrPromotion2;
}

/// Accessor: Sets promotion 2 of an either/or promotion prerequisite.
void CvPromotionEntry::SetPrereqOrPromotion2(int i)
{
	m_iPrereqOrPromotion2 = i;
}

/// Accessor: Gets promotion 3 of an either/or promotion prerequisite.
int CvPromotionEntry::GetPrereqOrPromotion3() const
{
	return m_iPrereqOrPromotion3;
}

/// Accessor: Sets promotion 3 of an either/or promotion prerequisite.
void CvPromotionEntry::SetPrereqOrPromotion3(int i)
{
	m_iPrereqOrPromotion3 = i;
}

/// Accessor: Gets promotion 4 of an either/or promotion prerequisite.
int CvPromotionEntry::GetPrereqOrPromotion4() const
{
	return m_iPrereqOrPromotion4;
}

/// Accessor: Sets promotion 4 of an either/or promotion prerequisite.
void CvPromotionEntry::SetPrereqOrPromotion4(int i)
{
	m_iPrereqOrPromotion4 = i;
}

/// Accessor: Gets promotion 5 of an either/or promotion prerequisite.
int CvPromotionEntry::GetPrereqOrPromotion5() const
{
	return m_iPrereqOrPromotion5;
}

/// Accessor: Sets promotion 5 of an either/or promotion prerequisite.
void CvPromotionEntry::SetPrereqOrPromotion5(int i)
{
	m_iPrereqOrPromotion5 = i;
}

/// Accessor: Gets promotion 6 of an either/or promotion prerequisite.
int CvPromotionEntry::GetPrereqOrPromotion6() const
{
	return m_iPrereqOrPromotion6;
}

/// Accessor: Sets promotion 6 of an either/or promotion prerequisite.
void CvPromotionEntry::SetPrereqOrPromotion6(int i)
{
	m_iPrereqOrPromotion6 = i;
}

/// Accessor: Gets promotion 7 of an either/or promotion prerequisite.
int CvPromotionEntry::GetPrereqOrPromotion7() const
{
	return m_iPrereqOrPromotion7;
}

/// Accessor: Sets promotion 7 of an either/or promotion prerequisite.
void CvPromotionEntry::SetPrereqOrPromotion7(int i)
{
	m_iPrereqOrPromotion7 = i;
}

/// Accessor: Gets promotion 8 of an either/or promotion prerequisite.
int CvPromotionEntry::GetPrereqOrPromotion8() const
{
	return m_iPrereqOrPromotion8;
}

/// Accessor: Sets promotion 8 of an either/or promotion prerequisite.
void CvPromotionEntry::SetPrereqOrPromotion8(int i)
{
	m_iPrereqOrPromotion8 = i;
}

/// Accessor: Gets promotion 9 of an either/or promotion prerequisite.
int CvPromotionEntry::GetPrereqOrPromotion9() const
{
	return m_iPrereqOrPromotion9;
}

/// Accessor: Sets promotion 9 of an either/or promotion prerequisite.
void CvPromotionEntry::SetPrereqOrPromotion9(int i)
{
	m_iPrereqOrPromotion9 = i;
}

/// Accessor: Gets the tech prerequisite for this promotion
int CvPromotionEntry::GetTechPrereq() const
{
	return m_iTechPrereq;
}

/// Accessor: What type of invisible this unit is
int CvPromotionEntry::GetInvisibleType() const
{
	return m_iInvisibleType;
}

/// Accessor: What type of invisible this unit can see
int CvPromotionEntry::GetSeeInvisibleType() const
{
	return m_iSeeInvisibleType;
}

/// Accessor: How many additional tiles this promotion allows a unit to see (can be negative)
int CvPromotionEntry::GetVisibilityChange() const
{
	return m_iVisibilityChange;
}

#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
/// Accessor: How many additional tiles this promotion allows a unit to recon (can be negative)
int CvPromotionEntry::GetReconChange() const
{
	return m_iReconChange;
}
#endif

/// Accessor: How many additional tiles a unit may move
int CvPromotionEntry::GetMovesChange() const
{
	return m_iMovesChange;
}

/// Accessor: How much movement is discounted
int CvPromotionEntry::GetMoveDiscountChange() const
{
	return m_iMoveDiscountChange;
}

/// Accessor: How much the air range of the unit is modified
int CvPromotionEntry::GetRangeChange() const
{
	return m_iRangeChange;
}

/// Accessor: How much the ranged strength of the unit is modified
int CvPromotionEntry::GetRangedAttackModifier() const
{
	return m_iRangedAttackModifier;
}

/// Accessor: How much the strength of the unit is modified when intercepting
int CvPromotionEntry::GetInterceptionCombatModifier() const
{
	return m_iInterceptionCombatModifier;
}

/// Accessor: Mod to Damage taken if intercepted
int CvPromotionEntry::GetInterceptionDefenseDamageModifier() const
{
	return m_iInterceptionDefenseDamageModifier;
}

/// Accessor: How much the strength of the unit is modified when air sweeping
int CvPromotionEntry::GetAirSweepCombatModifier() const
{
	return m_iAirSweepCombatModifier;
}

/// Accessor: How much the intercept chance changes for an air unit
int CvPromotionEntry::GetInterceptChanceChange() const
{
	return m_iInterceptChanceChange;
}

/// Accessor: Numbe rof extra interceptions that may be made on one turn
int CvPromotionEntry::GetNumInterceptionChange() const
{
	return m_iNumInterceptionChange;
}

/// Accessor: How much additional range this promotion allows an unit to perform interception (can be negative)
int CvPromotionEntry::GetAirInterceptRangeChange() const
{
	return m_iAirInterceptRangeChange;
}

/// Accessor: How well an air unit can evade interception
int CvPromotionEntry::GetEvasionChange() const
{
	return m_iEvasionChange;
}

/// Accessor: The amount of cargo a unit can carry
int CvPromotionEntry::GetCargoChange() const
{
	return m_iCargoChange;
}

/// Accessor: Damage/health bonus per turn in enemy lands
int CvPromotionEntry::GetEnemyHealChange() const
{
	return m_iEnemyHealChange;
}

/// Accessor: Damage/health bonus per turn in neutral lands
int CvPromotionEntry::GetNeutralHealChange() const
{
	return m_iNeutralHealChange;
}

/// Accessor: Damage/health bonus per turn in friendly lands
int CvPromotionEntry::GetFriendlyHealChange() const
{
	return m_iFriendlyHealChange;
}

/// Accessor: Heals units in the same tile
int CvPromotionEntry::GetSameTileHealChange() const
{
	return m_iSameTileHealChange;
}

/// Accessor: Heals units in adjacent tiles
int CvPromotionEntry::GetAdjacentTileHealChange() const
{
	return m_iAdjacentTileHealChange;
}

/// Accessor: Chance per turn to take damage in enemy lands
int CvPromotionEntry::GetEnemyDamageChance() const
{
	return m_iEnemyDamageChance;
}

/// Accessor: Chance per turn to take damage in neutral lands
int CvPromotionEntry::GetNeutralDamageChance() const
{
	return m_iNeutralDamageChance;
}

/// Accessor: Amount of damage taken by being in enemy lands
int CvPromotionEntry::GetEnemyDamage() const
{
	return m_iEnemyDamage;
}

/// Accessor: Amount of damage taken by being in neutral lands
int CvPromotionEntry::GetNeutralDamage() const
{
	return m_iNeutralDamage;
}

/// Accessor: Bonus strength percent
int CvPromotionEntry::GetCombatPercent() const
{
	return m_iCombatPercent;
}

/// Accessor: Bonus city attack percent
int CvPromotionEntry::GetCityAttackPercent() const
{
	return m_iCityAttackPercent;
}

/// Accessor: Bonus city defense percent
int CvPromotionEntry::GetCityDefensePercent() const
{
	return m_iCityDefensePercent;
}

/// Accessor: Bonus ranged defense percent
int CvPromotionEntry::GetRangedDefenseMod() const
{
	return m_iRangedDefenseMod;
}

/// Accessor: Bonus hills attack percent
int CvPromotionEntry::GetHillsAttackPercent() const
{
	return m_iHillsAttackPercent;
}

/// Accessor: Bonus hills defense percent
int CvPromotionEntry::GetHillsDefensePercent() const
{
	return m_iHillsDefensePercent;
}

/// Accessor: Bonus open terrain attack percent
int CvPromotionEntry::GetOpenAttackPercent() const
{
	return m_iOpenAttackPercent;
}
/// Accessor: Bonus open terrain combat strength mod
int CvPromotionEntry::GetOpenFromPercent() const
{
	return m_iOpenFromPercent;
}

/// Accessor: Bonus open terrain ranged attack mod
int CvPromotionEntry::GetOpenRangedAttackMod() const
{
	return m_iOpenRangedAttackMod;
}

/// Accessor: Bonus open terrain attack percent
int CvPromotionEntry::GetRoughAttackPercent() const
{
	return m_iRoughAttackPercent;
}

/// Accessor: Bonus rough terrain combat strength mod
int CvPromotionEntry::GetRoughFromPercent() const
{
	return m_iRoughFromPercent;
}

/// Accessor: Bonus rough terrain ranged attack mod
int CvPromotionEntry::GetRoughRangedAttackMod() const
{
	return m_iRoughRangedAttackMod;
}

/// Accessor: Bonus when attacking fortified units
int CvPromotionEntry::GetAttackFortifiedMod() const
{
	return m_iAttackFortifiedMod;
}

/// Accessor: Bonus when attacking wounded units
int CvPromotionEntry::GetAttackWoundedMod() const
{
	return m_iAttackWoundedMod;
}

int CvPromotionEntry::GetAttackFullyHealedMod() const
{
	return m_iAttackFullyHealedMod;
}

int CvPromotionEntry::GetAttackAboveHealthMod() const
{
	return m_iAttackAboveHealthMod;
}
int CvPromotionEntry::GetAttackBelowHealthMod() const
{
	return m_iAttackBelowHealthMod;
}

/// Accessor: Ranged unit attacks benefit from units flanking target
bool CvPromotionEntry::IsRangedFlankAttack() const
{
	return m_bRangedFlankAttack;
}

/// Accessor: Counts as additional units when supporting a flank
int CvPromotionEntry::GetExtraFlankPower() const
{
	return m_iExtraFlankPower;
}

/// Accessor: Bonus when making a flank attack
int CvPromotionEntry::GetFlankAttackModifier() const
{
	return m_iFlankAttackModifier;
}

/// Accessor: Modifier on nearby enemy combat units
int CvPromotionEntry::GetNearbyEnemyCombatMod() const
{
	return m_iNearbyEnemyCombatMod;
}

/// Accessor: Range of modifier on nearby enemy combat units
int CvPromotionEntry::GetNearbyEnemyCombatRange() const
{
	return m_iNearbyEnemyCombatRange;
}

/// Accessor: Bonus open terrain defense percent
int CvPromotionEntry::GetOpenDefensePercent() const
{
	return m_iOpenDefensePercent;
}

/// Accessor: Bonus open terrain defense percent
int CvPromotionEntry::GetRoughDefensePercent() const
{
	return m_iRoughDefensePercent;
}

/// Accessor: Does the unit have more than 1 attack?
int CvPromotionEntry::GetExtraAttacks() const
{
	return m_iExtraAttacks;
}

/// Accessor: Does this Promotion make a Unit a Great General?
bool CvPromotionEntry::IsGreatGeneral() const
{
	return m_bGreatGeneral;
}

/// Accessor: Does this Promotion make a Unit a Great Admiral?
bool CvPromotionEntry::IsGreatAdmiral() const
{
	return m_bGreatAdmiral;
}

/// Accessor: Does this Promotion change the range of the aura of a Great General or Great Admiral?
int CvPromotionEntry::GetAuraRangeChange() const
{
	return m_iAuraRangeChange;
}

/// Accessor: Does this Promotion change the effect of the aura of a Great General or Great Admiral?
int CvPromotionEntry::GetAuraEffectChange() const
{
	return m_iAuraEffectChange;
}

int CvPromotionEntry::GetNumRepairCharges() const
{
	return m_iNumRepairCharges;
}

int CvPromotionEntry::GetMilitaryCapChange() const
{
	return m_iMilitaryCapChange;
}

/// Accessor: Increase in rate of great general creation
int CvPromotionEntry::GetGreatGeneralModifier() const
{
	return m_iGreatGeneralModifier;
}

/// Accessor: Does this Promotion make a Great General move at this unit's rate?
bool CvPromotionEntry::IsGreatGeneralReceivesMovement() const
{
	return m_bGreatGeneralReceivesMovement;
}

/// Accessor: Combat bonus when stacked with Great General
int CvPromotionEntry::GetGreatGeneralCombatModifier() const
{
	return m_iGreatGeneralCombatModifier;
}

/// Accessor: Combat mod for fighting in friendly territory
int CvPromotionEntry::GetFriendlyLandsModifier() const
{
	return m_iFriendlyLandsModifier;
}

/// Accessor: Attack mod for fighting in friendly territory
int CvPromotionEntry::GetFriendlyLandsAttackModifier() const
{
	return m_iFriendlyLandsAttackModifier;
}

/// Accessor: Combat mod for fighting outside friendly territory
int CvPromotionEntry::GetOutsideFriendlyLandsModifier() const
{
	return m_iOutsideFriendlyLandsModifier;
}

/// Accessor: Returns the command type for this HotKeyInfo class (which would be COMMAND_PROMOTION)
int CvPromotionEntry::GetCommandType() const
{
	return m_iCommandType;
}

/// Sets the command type for this class.
void CvPromotionEntry::SetCommandType(int iNewType)
{
	m_iCommandType = iNewType;
}

/// Accessor: Unit has no supply cost
bool CvPromotionEntry::IsNoSupply() const
{
	return m_bNoSupply;
}

/// Accessor: Absolute change of max hit points
int CvPromotionEntry::GetMaxHitPointsChange() const
{
	return m_iMaxHitPointsChange;
}

/// Accessor: Percentage modifier of base max hit points
int CvPromotionEntry::GetMaxHitPointsModifier() const
{
	return m_iMaxHitPointsModifier;
}

/// Accessor: Modifier against units of unhappy empires
int CvPromotionEntry::GetVsUnhappyMod() const
{
	return m_iVsUnhappyMod;
}

/// Accessor: How much upgrading this unit is discounted
int CvPromotionEntry::GetUpgradeDiscount() const
{
	return m_iUpgradeDiscount;
}

/// Accessor: Increased rate of gaining experience
int CvPromotionEntry::GetExperiencePercent() const
{
	return m_iExperiencePercent;
}

/// Accessor: Bonus when adjacent to a friendly Unit
int CvPromotionEntry::GetAdjacentMod() const
{
	return m_iAdjacentMod;
}

/// Accessor: Bonus when not adjacent to friendly Units
int CvPromotionEntry::GetNoAdjacentUnitMod() const
{
	return m_iNoAdjacentUnitMod;
}

/// Accessor: Percent attack change
int CvPromotionEntry::GetAttackMod() const
{
	return m_iAttackMod;
}

/// Accessor: Percent defense change
int CvPromotionEntry::GetDefenseMod() const
{
	return m_iDefenseMod;
}

/// Accessor: Border combat strength change
int CvPromotionEntry::GetBorderMod() const
{
	return m_iBorderMod;
}

/// Accessor: Per-Marriage combat strength change
int CvPromotionEntry::GetMarriageMod() const
{
	return m_iMarriageMod;
}

/// Accessor: Per-Marriage combat strength change cap
int CvPromotionEntry::GetMarriageModCap() const
{
	return m_iMarriageModCap;
}

/// Accessor: Percent defense change
int CvPromotionEntry::GetGroundAttackDamage() const
{
	return m_iGetGroundAttackDamage;
}

/// Accessor: Number of tiles away a unit may paradrop
int CvPromotionEntry::GetDropRange() const
{
	return m_iDropRange;
}

/// Accessor: Number of extra tiles of movement for naval vessels
int CvPromotionEntry::GetExtraNavalMoves() const
{
	return m_iExtraNavalMoves;
}

/// Accessor: HP healed after defeating enemy (up to 10 for full heal)
int CvPromotionEntry::GetHPHealedIfDefeatEnemy() const
{
	return m_iHPHealedIfDefeatEnemy;
}

/// Accessor: percent of defeated enemy strenth awarded as points toward next Golden Age
int CvPromotionEntry::GetGoldenAgeValueFromKills() const
{
	return m_iGoldenAgeValueFromKills;
}

/// Accessor: maximum chance to withdraw before melee
int CvPromotionEntry::GetExtraWithdrawal() const
{
	return m_iExtraWithdrawal;
}

/// Accessor: added base combat strength
int CvPromotionEntry::GetCombatChange() const
{
	return m_iCombatChange;
}

/// Accessor: additional damage to all units on the target tile if the unit didn't move before the attack
int CvPromotionEntry::GetTileDamageIfNotMoved() const
{
	return m_iTileDamageIfNotMoved;
}

/// Accessor: Percentage multiplier on fortify combat strength bonus
int CvPromotionEntry::GetFortifiedModifier() const
{
	return m_iFortifiedModifier;
}

/// Accessor: promotion is only active if the unit starts its turn with a minimum percentage of their maximum health
int CvPromotionEntry::GetMinEffectiveHealth() const
{
	return m_iMinEffectiveHealth;
}

/// Accessor: promotion is only active if the unit starts its turn affected by the leadership aura of a great general/admiral
bool CvPromotionEntry::IsRequiresLeadership() const
{
	return m_bRequiresLeadership;
}

/// Accessor: units with this promotion cannot heal
bool CvPromotionEntry::IsCannotHeal() const
{
	return m_bCannotHeal;
}

bool CvPromotionEntry::IsPillageFortificationsOnKill() const
{
	return m_bPillageFortificationsOnKill;
}

#if defined(MOD_BALANCE_CORE_JFD)



int CvPromotionEntry::GetPlagueID() const
{
	return m_iPlagueID;
}

int CvPromotionEntry::GetPlaguePriority() const
{
	return m_iPlaguePriority;
}
#endif

/// Accessor: extra sight range when embarked
int CvPromotionEntry::GetEmbarkExtraVisibility() const
{
	return m_iEmbarkExtraVisibility;
}

/// Accessor: extra defense when embarked
int CvPromotionEntry::GetEmbarkDefenseModifier() const
{
	return m_iEmbarkDefenseModifier;
}

/// Accessor: bonus defending near capital
int CvPromotionEntry::GetCapitalDefenseModifier() const
{
	return m_iCapitalDefenseModifier;
}

/// Accessor: drop per hex away from capital of CapitalDefenseModifier
int CvPromotionEntry::GetCapitalDefenseFalloff() const
{
	return m_iCapitalDefenseFalloff;
}

int CvPromotionEntry::GetCapitalDefenseLimit() const
{
	return m_iCapitalDefenseLimit;
}

/// Accessor: gold earned from damage on an attacked city
int CvPromotionEntry::GetCityAttackPlunderModifier() const
{
	return m_iCityAttackPlunderModifier;
}

/// Accessor: loss of strength at end of each turn in rival territory without open borders
int CvPromotionEntry::GetReligiousStrengthLossRivalTerritory() const
{
	return m_iReligiousStrengthLossRivalTerritory;
}

/// Accessor: This is for great merchants for when they complete their mission in a city state.
int CvPromotionEntry::GetTradeMissionInfluenceModifier() const
{
	return m_iTradeMissionInfluenceModifier;
}

/// Accessor: This is for great merchants for when they complete their mission in a city state.
int CvPromotionEntry::GetTradeMissionGoldModifier() const
{
	return m_iTradeMissionGoldModifier;
}

/// Accessor: Combat mod for each unit level above the first one
int CvPromotionEntry::GetCombatModPerLevel() const
{
	return m_iCombatModPerLevel;
}
int CvPromotionEntry::GetDiploMissionInfluence() const
{
	return m_iDiploMissionInfluence;
}

int CvPromotionEntry::GetGoodyHutYieldBonus() const
{
	return m_iGoodyHutYieldBonus;
}
/// Accessor: Can this Promotion grant XP from scouting?
bool CvPromotionEntry::IsGainsXPFromScouting() const
{
	return m_bGainsXPFromScouting;
}

/// Accessor: Can this Promotion grant XP from pillaging?
int CvPromotionEntry::GetXPFromPillaging() const
{
	return m_iXPFromPillaging;
}

/// Accessor: Extra XP when killing a unit
int CvPromotionEntry::GetExtraXPOnKill() const
{
	return m_iExtraXPOnKill;
}

/// Accessor: Can this Promotion grant XP from spotting?
bool CvPromotionEntry::IsGainsXPFromSpotting() const
{
	return m_bGainsXPFromSpotting;
}

int CvPromotionEntry::GetCaptureDefeatedEnemyChance() const
{
	return m_iCaptureDefeatedEnemyChance;
}
/// Accessor: Can this Promotion grant bonuses v. barbarians?
int CvPromotionEntry::GetBarbarianCombatBonus() const
{
	return m_iBarbarianCombatBonus;
}
//Cannot be captured
bool CvPromotionEntry::CannotBeCaptured() const
{
	return m_bCannotBeCaptured;
}
//Promotion lost if you leave the terrain/feature that gives it.
bool CvPromotionEntry::IsLostOnMove() const
{
	return m_bIsLostOnMove;
}
//Promotion for CSs only
bool CvPromotionEntry::IsCityStateOnly() const
{
	return m_bCityStateOnly;
}
//Units with this promotion trigger ACCOMPLISHMENT_DIPLOMATIC_MISSION_BOOST when finishing a diplomatic mission
bool CvPromotionEntry::IsDiplomaticMissionAccomplishment() const
{
	return m_bDiplomaticMissionAccomplishment;
}
//Promotion for barbs only
bool CvPromotionEntry::IsBarbarianOnly() const
{
	return m_bBarbarianOnly;
}
bool CvPromotionEntry::IsStrongerDamaged() const
{
	return m_bStrongerDamaged;
}
bool CvPromotionEntry::IsFightWellDamaged() const
{
	return m_bFightWellDamaged;
}
bool CvPromotionEntry::IsFreeAttackMoves() const
{
	return m_bFreeAttackMoves;
}
int CvPromotionEntry::NegatesPromotion() const
{
	return m_iNegatesPromotion;
}
int CvPromotionEntry::GetForcedDamageValue() const
{
	return m_iForcedDamageValue;
}
int CvPromotionEntry::GetChangeDamageValue() const
{
	return m_iChangeDamageValue;
}
int CvPromotionEntry::GetDamageTakenMod() const
{
	return m_iDamageTakenMod;
}
int CvPromotionEntry::GetInfluenceFromCombatXPTimes100() const
{
	return m_iInfluenceFromCombatXPTimes100;
}
int CvPromotionEntry::PromotionDuration() const
{
	return m_iPromotionDuration;
}
int CvPromotionEntry::GetMoraleBreakChance() const
{
	return m_iMoraleBreakChance;
}
int CvPromotionEntry::GetDamageAoEFortified() const
{
	return m_iDamageAoEFortified;
}
int CvPromotionEntry::GetWorkRateMod() const
{
	return m_iWorkRateMod;
}
/// Accessor: Can this Promotion be earned through normal leveling?
bool CvPromotionEntry::IsCannotBeChosen() const
{
	return m_bCannotBeChosen;
}

/// Accessor: Is this lost with upgrading?
bool CvPromotionEntry::IsLostWithUpgrade() const
{
	return m_bLostWithUpgrade;
}

/// Accessor: Is this a promotion that is not given out when a unit is upgrading?
bool CvPromotionEntry::IsNotWithUpgrade() const
{
	return m_bNotWithUpgrade;
}

/// Accessor: Does this promotion instantly heal a Unit?
bool CvPromotionEntry::IsInstaHeal() const
{
	return m_bInstaHeal;
}

/// Accessor: Does this unit have a warlord attached to it?
bool CvPromotionEntry::IsLeader() const
{
	return m_bLeader;
}

/// Accessor: Can this unit attack multiple times a turn?
bool CvPromotionEntry::IsBlitz() const
{
	return m_bBlitz;
}

/// Accessor: Can this unit attack from the sea without penalty?
bool CvPromotionEntry::IsAmphib() const
{
	return m_bAmphib;
}

/// Accessor: Can this unit attack across rivers without penalty?
bool CvPromotionEntry::IsRiver() const
{
	return m_bRiver;
}

/// Accessor: Can this unit use enemy roads?
bool CvPromotionEntry::IsEnemyRoute() const
{
	return m_bEnemyRoute;
}

/// Accessor: Can enter rival territory
bool CvPromotionEntry::IsRivalTerritory() const
{
	return m_bRivalTerritory;
}

/// Accessor: Must this unit set up multiple turns for a ranged attack
bool CvPromotionEntry::IsMustSetUpToRangedAttack() const
{
	return m_bMustSetUpToRangedAttack;
}

/// Accessor: Can this Unit provide Ranged Support Fire?
bool CvPromotionEntry::IsRangedSupportFire() const
{
	return m_bRangedSupportFire;
}

/// Accessor: Unit can heal while moving
bool CvPromotionEntry::IsAlwaysHeal() const
{
	return m_bAlwaysHeal;
}

/// Accessor: Unit can heal outside friendly territory (naval units)
bool CvPromotionEntry::IsHealOutsideFriendly() const
{
	return m_bHealOutsideFriendly;
}

/// Accessor: Double movement in hills
bool CvPromotionEntry::IsHillsDoubleMove() const
{
	return m_bHillsDoubleMove;
}

/// Accessor: Double movement when next to rivers
bool CvPromotionEntry::IsRiverDoubleMove() const
{
	return m_bRiverDoubleMove;
}

/// Accessor: Ignores terrain movement penalties
bool CvPromotionEntry::IsIgnoreTerrainCost() const
{
	return m_bIgnoreTerrainCost;
}

/// Accessor: Ignores terrain damage
bool CvPromotionEntry::IsIgnoreTerrainDamage() const
{
	return m_bIgnoreTerrainDamage;
}

/// Accessor: Ignores feature damage
bool CvPromotionEntry::IsIgnoreFeatureDamage() const
{
	return m_bIgnoreFeatureDamage;
}

/// Accessor: Takes extra terrain damage
bool CvPromotionEntry::IsExtraTerrainDamage() const
{
	return m_bExtraTerrainDamage;
}

/// Accessor: Takes extra feature damage
bool CvPromotionEntry::IsExtraFeatureDamage() const
{
	return m_bExtraFeatureDamage;
}

#if defined(MOD_PROMOTIONS_IMPROVEMENT_BONUS)
/// Accessor: Can cross mountains (but we'd rather they left them nice and straight!)
int CvPromotionEntry::GetNearbyImprovementCombatBonus() const
{
	return m_iNearbyImprovementCombatBonus;
}
int CvPromotionEntry::GetNearbyImprovementBonusRange() const
{
	return m_iNearbyImprovementBonusRange;
}
ImprovementTypes CvPromotionEntry::GetCombatBonusImprovement() const
{
	return m_eCombatBonusImprovement;
}
#endif
/// Get the UnitClass we want to receive the bonus from.
UnitClassTypes CvPromotionEntry::GetCombatBonusFromNearbyUnitClass() const
{
	return m_iCombatBonusFromNearbyUnitClass;
}
/// Distance from this UnitClass
int CvPromotionEntry::GetNearbyUnitClassBonusRange() const
{
	return m_iNearbyUnitClassBonusRange;
}
/// Bonus from this UnitClass
int CvPromotionEntry::GetNearbyUnitClassBonus() const
{
	return m_iNearbyUnitClassBonus;
}
int CvPromotionEntry::GetWonderProductionModifier() const
{
	return m_iWonderProductionModifier;
}
int CvPromotionEntry::GetAOEDamageOnKill() const
{
	return m_iAOEDamageOnKill;
}
int CvPromotionEntry::GetAOEDamageOnPillage() const
{
	return m_iAOEDamageOnPillage;
}
int CvPromotionEntry::GetAOEHealOnPillage() const
{
	return m_iAOEHealOnPillage;
}
int CvPromotionEntry::GetCombatModPerCSAlliance() const
{
	return m_iCombatModPerCSAlliance;
}
int CvPromotionEntry::GetAttackModPerSamePromotionAttack() const
{
	return m_iAttackModPerSamePromotionAttack;
}
int CvPromotionEntry::GetAttackModPerSamePromotionAttackCap() const
{
	return m_iAttackModPerSamePromotionAttackCap;
}
int CvPromotionEntry::GetAoEDamageOnMove() const
{
	return m_iAoEDamageOnMove;
}
int CvPromotionEntry::GetPartialHealOnPillage() const
{
	return m_iPartialHealOnPillage;
}
int CvPromotionEntry::GetSplashDamage() const
{
	return m_iSplashDamage;
}
int CvPromotionEntry::GetMinRange() const
{
	return m_iMinRange;
}
int CvPromotionEntry::GetMaxRange() const
{
	return m_iMaxRange;
}
int CvPromotionEntry::GetMultiAttackBonus() const
{
	return m_iMultiAttackBonus;
}
int CvPromotionEntry::GetLandAirDefenseValue() const
{
	return m_iLandAirDefenseValue;
}
int CvPromotionEntry::GetDamageReductionCityAssault() const
{
	return m_iDamageReductionCityAssault;
}
/// Accessor: Double movement in hills
bool CvPromotionEntry::IsMountainsDoubleMove() const
{
	return m_bMountainsDoubleMove;
}

bool CvPromotionEntry::IsEmbarkFlatCost() const
{
	return m_bEmbarkFlatCost;
}

bool CvPromotionEntry::IsDisembarkFlatCost() const
{
	return m_bDisembarkFlatCost;
}

bool CvPromotionEntry::IsMountedOnly() const
{
	return m_bMountedOnly;
}



#if defined(MOD_PROMOTIONS_CROSS_MOUNTAINS)
/// Accessor: Can cross mountains (but we'd rather they left them nice and straight!)
bool CvPromotionEntry::CanCrossMountains() const
{
	return m_bCanCrossMountains;
}
#endif

#if defined(MOD_PROMOTIONS_CROSS_OCEANS)
/// Accessor: Can cross oceans (but that may make them angry!)
bool CvPromotionEntry::CanCrossOceans() const
{
	return m_bCanCrossOceans;
}
#endif

#if defined(MOD_PROMOTIONS_CROSS_ICE)
/// Accessor: Can cross ice (with what? Fire?)
bool CvPromotionEntry::CanCrossIce() const
{
	return m_bCanCrossIce;
}
#endif

#if defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
/// Accessor: Gets GG/GA points from barbarians
bool CvPromotionEntry::IsGGFromBarbarians() const
{
	return m_bGGFromBarbarians;
}
#endif

/// Accessor: Entering rough terrain uses up a unit's movement? (regardless of how many moves he has)
bool CvPromotionEntry::IsRoughTerrainEndsTurn() const
{
	return m_bRoughTerrainEndsTurn;
}

/// Accessor: Units captured by a unit with this promotion get the conscript promotion and don't count for military supply
bool CvPromotionEntry::IsCapturedUnitsConscripted() const
{
	return m_bCapturedUnitsConscripted;
}

/// Accessor: Unit may pass over coast and Mountains
bool CvPromotionEntry::IsHoveringUnit() const
{
	return m_bHoveringUnit;
}

/// Accessor: All terrain costs 1 to enter
bool CvPromotionEntry::IsFlatMovementCost() const
{
	return m_bFlatMovementCost;
}

/// Accessor: May enter ice tiles
bool CvPromotionEntry::IsCanMoveImpassable() const
{
	return m_bCanMoveImpassable;
}

/// Accessor: Unable to capture cities
bool CvPromotionEntry::IsNoCapture() const
{
	return m_bNoCapture;
}
/// Accessor: Unable to attack enemy units
bool CvPromotionEntry::IsOnlyDefensive() const
{
	return m_bOnlyDefensive;
}

/// Accessor: Unable to attack enemy units while in deep water
bool CvPromotionEntry::IsNoAttackInOcean() const
{
	return m_bNoAttackInOcean;
}

/// Accessor: No defensive bonuses
bool CvPromotionEntry::IsNoDefensiveBonus() const
{
	return m_bNoDefensiveBonus;
}

/// Accessor: Cannot be hurt by nukes?
bool CvPromotionEntry::IsNukeImmune() const
{
	return m_bNukeImmune;
}

/// Accessor: Nationality is hidden from other players
bool CvPromotionEntry::IsHiddenNationality() const
{
	return m_bHiddenNationality;
}

/// Accessor: Can attack without declaring war
bool CvPromotionEntry::IsAlwaysHostile() const
{
	return m_bAlwaysHostile;
}

/// Accessor: Cannot reveal terrain unless it is inside a player's territory
bool CvPromotionEntry::IsNoRevealMap() const
{
	return m_bNoRevealMap;
}

/// Accessor: Is this a Recon unit? (free sight all around it)
bool CvPromotionEntry::IsRecon() const
{
	return m_bRecon;
}

/// Accessor: Can move through all terrain?
bool CvPromotionEntry::CanMoveAllTerrain() const
{
	return m_bCanMoveAllTerrain;
}

/// Accessor: Can move after attacking another Unit?
bool CvPromotionEntry::IsCanMoveAfterAttacking() const
{
	return m_bCanMoveAfterAttacking;
}

/// Accessor: Can this unit air sweep?
bool CvPromotionEntry::IsAirSweepCapable() const
{
	return m_bAirSweepCapable;
}

/// Accessor: Can this unit embark?
bool CvPromotionEntry::IsAllowsEmbarkation() const
{
	return m_bAllowsEmbarkation;
}

/// Accessor: Can this unit shoot over obstacles?
bool CvPromotionEntry::IsRangeAttackIgnoreLOS() const
{
	return m_bRangeAttackIgnoreLOS;
}

/// Accessor: Lose no movement when pillaging?
bool CvPromotionEntry::IsFreePillageMoves() const
{
	return m_bFreePillageMoves;
}

/// Accessor: When pillaging, does the unit heal?
bool CvPromotionEntry::IsHealOnPillage() const
{
	return m_bHealOnPillage;
}

/// Accessor: Do we only get healed after a combat win if fighting a real civ or minor?
bool CvPromotionEntry::IsHealIfDefeatExcludeBarbarians() const
{
	return m_bHealIfDefeatExcludesBarbarians;
}

/// Accessor: Do we not get run over by naval units when embarked at sea?
bool CvPromotionEntry::IsEmbarkedAllWater() const
{
	return m_bEmbarkedAllWater;
}

#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
/// Accessor: Do we not get run over by naval units when embarked at sea?
bool CvPromotionEntry::IsEmbarkedDeepWater() const
{
	return m_bEmbarkedDeepWater;
}
#endif

/// Accessor: Does this unit only attack cities
bool CvPromotionEntry::IsCityAttackSupport() const
{
	return m_bCityAttackOnly;
}

/// Accessor: Is a defeated enemy captured?
bool CvPromotionEntry::IsCaptureDefeatedEnemy() const
{
	return m_bCaptureDefeatedEnemy;
}

/// Accessor: Should this infantry ignore benefits from great generals
bool CvPromotionEntry::IsIgnoreGreatGeneralBenefit() const
{
	return m_bIgnoreGreatGeneralBenefit;
}

/// Accessor: Can this unit ignore ZOC when moving?
bool CvPromotionEntry::IsIgnoreZOC() const
{
	return m_bIgnoreZOC;
}

/// Accessor: Is this a Sapper unit (when next to a city, provides city attack bonus to nearby units)?
bool CvPromotionEntry::IsSapper() const
{
	return m_bSapper;
}

int CvPromotionEntry::GetNearbyCityCombatMod() const
{
	return m_iNearbyCityCombatMod;
}
int CvPromotionEntry::GetNearbyFriendlyCityCombatMod() const
{
	return m_iNearbyFriendlyCityCombatMod;
}
int CvPromotionEntry::GetNearbyEnemyCityCombatMod() const
{
	return m_iNearbyEnemyCityCombatMod;
}
bool CvPromotionEntry::IsNearbyPromotion() const
{
	return m_bIsNearbyPromotion;
}
int CvPromotionEntry::GetNearbyRange() const
{
	return m_iNearbyRange;
}
bool CvPromotionEntry::IsConvertEnemyUnitToBarbarian() const
{
	return m_bIsConvertEnemyUnitToBarbarian;
}
bool CvPromotionEntry::IsConvertOnFullHP() const
{
	return m_bIsConvertOnFullHP;
}
bool CvPromotionEntry::IsConvertOnDamage() const
{
	return m_bIsConvertOnDamage;
}
int CvPromotionEntry::GetDamageThreshold() const
{
	return m_iDamageThreshold;
}
int CvPromotionEntry::GetConvertDamageOrFullHPUnit() const
{
	return m_iConvertDamageOrFullHPUnit;
}
bool CvPromotionEntry::IsConvertUnit() const
{
	return m_bIsConvertUnit;
}
int CvPromotionEntry::GetConvertDomainUnit() const
{
	return m_iConvertDomainUnit;
}
int CvPromotionEntry::GetConvertDomain() const
{
	return m_iConvertDomain;
}
int CvPromotionEntry::GetStackedGreatGeneralExperience() const
{
	return m_iStackedGreatGeneralExperience;
}
int CvPromotionEntry::GetPillageBonusStrengthPercent() const
{
	return m_iPillageBonusStrength;
}
int CvPromotionEntry::GetReligiousPressureModifier() const
{
	return m_iReligiousPressureModifier;
}
int CvPromotionEntry::GetAdjacentCityDefenseMod() const
{
	return m_iAdjacentCityDefenseMod;
}
int CvPromotionEntry::GetNearbyEnemyDamage() const
{
	return m_iNearbyEnemyDamage;
}
int CvPromotionEntry::GetMilitaryProductionModifier() const
{
	return m_iMilitaryProductionModifier;
}
int CvPromotionEntry::GetGeneralGoldenAgeExpPercent() const
{
	return m_iGeneralGoldenAgeExpPercent;
}
int CvPromotionEntry::GetGiveCombatMod() const
{
	return m_iGiveCombatMod;
}
int CvPromotionEntry::GetGiveHPIfEnemyKilled() const
{
	return m_iGiveHPHealedIfEnemyKilled;
}
int CvPromotionEntry::GetGiveExperiencePercent() const
{
	return m_iGiveExperiencePercent;
}
int CvPromotionEntry::GetGiveOutsideFriendlyLandsModifier() const
{
	return m_iGiveOutsideFriendlyLandsModifier;
}
int CvPromotionEntry::GetGiveDomain() const
{
	return m_iGiveDomain;
}
int CvPromotionEntry::GetGiveExtraAttacks() const
{
	return m_iGiveExtraAttacks;
}
int CvPromotionEntry::GetGiveDefenseMod() const
{
	return m_iGiveDefenseMod;
}
bool CvPromotionEntry::IsGiveInvisibility() const
{
	return m_bGiveInvisibility;
}
bool CvPromotionEntry::IsGiveOnlyOnStartingTurn() const
{
	return m_bGiveOnlyOnStartingTurn;
}
int CvPromotionEntry::GetNearbyHealEnemyTerritory() const
{
	return m_iNearbyHealEnemyTerritory;
}
int CvPromotionEntry::GetNearbyHealNeutralTerritory() const
{
	return m_iNearbyHealNeutralTerritory;
}
int CvPromotionEntry::GetNearbyHealFriendlyTerritory() const
{
	return m_iNearbyHealFriendlyTerritory;
}

int CvPromotionEntry::GetAdjacentEnemySapMovement() const
{
	return m_iAdjacentEnemySapMovement;
}

/// Accessor: Can this unit doa heavy charge (which either force an enemy to retreat or take extra damage)
bool CvPromotionEntry::IsCanHeavyCharge() const
{
	return m_bCanHeavyCharge;
}

/// Accessor: Does this promotion change into another after combat?
bool CvPromotionEntry::HasPostCombatPromotions() const
{
	return m_bHasPostCombatPromotions;
}

/// Accessor: Do we have to pick a post-combat promotion that hasn't been used yet?
bool CvPromotionEntry::ArePostCombatPromotionsExclusive() const
{
	return m_bPostCombatPromotionsExclusive;
}

/// Is the promotion only active if certain conditions are fulfilled?
bool CvPromotionEntry::IsConditionalPromotion() const
{
	if (GetMinEffectiveHealth() > 0)
		return true;

	if (IsRequiresLeadership())
		return true;

	return false;
}

/// Accessor: Sound to play when the promotion is gained
const char* CvPromotionEntry::GetSound() const
{
	return m_strSound;
}

/// Set the sound to be played when the promotion is gained
void CvPromotionEntry::SetSound(const char* szVal)
{
	m_strSound = szVal;
}

// ARRAYS

/// Percentage bonus when attacking a tile of a given terrain
int CvPromotionEntry::GetTerrainAttackPercent(int i) const
{
	PRECONDITION(i < GC.getNumTerrainInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumTerrainInfos() && m_piTerrainAttackPercent)
	{
		return m_piTerrainAttackPercent[i];
	}

	return 0;
}

/// Percentage bonus when when defending a tile of a given terrain
int CvPromotionEntry::GetTerrainDefensePercent(int i) const
{
	PRECONDITION(i < GC.getNumTerrainInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumTerrainInfos() && m_piTerrainDefensePercent)
	{
		return m_piTerrainDefensePercent[i];
	}

	return 0;
}

/// Percentage bonus when attacking a tile of a given terrain
int CvPromotionEntry::GetTerrainModifierAttack(int i) const
{
	PRECONDITION(i < GC.getNumTerrainInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumTerrainInfos() && m_piTerrainModifierAttack)
	{
		return m_piTerrainModifierAttack[i];
	}

	return 0;
}

/// Percentage bonus when when defending a tile of a given terrain
int CvPromotionEntry::GetTerrainModifierDefense(int i) const
{
	PRECONDITION(i < GC.getNumTerrainInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumTerrainInfos() && m_piTerrainModifierDefense)
	{
		return m_piTerrainModifierDefense[i];
	}

	return 0;
}

/// Percentage bonus when when attacking a tile with a given feature
int CvPromotionEntry::GetFeatureAttackPercent(int i) const
{
	PRECONDITION(i < GC.getNumFeatureInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumFeatureInfos() && m_piFeatureAttackPercent)
	{
		return m_piFeatureAttackPercent[i];
	}

	return 0;
}

/// Percentage bonus when when defending a tile with a given feature
int CvPromotionEntry::GetFeatureDefensePercent(int i) const
{
	PRECONDITION(i < GC.getNumFeatureInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumFeatureInfos() && m_piFeatureDefensePercent)
	{
		return m_piFeatureDefensePercent[i];
	}

	return 0;
}
/// Modifier to yield by type
int CvPromotionEntry::GetYieldModifier(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	if(i > -1 && i < NUM_YIELD_TYPES && m_piYieldModifier)
	{
		return m_piYieldModifier[i];
	}

	return 0;
}

/// Modifier to yield by type
int CvPromotionEntry::GetYieldChange(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	if (i > -1 && i < NUM_YIELD_TYPES && m_piYieldChange)
	{
		return m_piYieldChange[i];
	}

	return 0;
}

/// Instant yields when picking up ancient ruins
int CvPromotionEntry::GetYieldFromAncientRuins(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	if (i > -1 && i < NUM_YIELD_TYPES && m_piYieldFromAncientRuins)
	{
		return m_piYieldFromAncientRuins[i];
	}

	return 0;
}

/// Instant yields when plundering a trade route
int CvPromotionEntry::GetYieldFromTRPlunder(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	if (i > -1 && i < NUM_YIELD_TYPES && m_piYieldFromTRPlunder)
	{
		return m_piYieldFromTRPlunder[i];
	}

	return 0;
}

int CvPromotionEntry::GetYieldFromScouting(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < NUM_YIELD_TYPES && m_piYieldFromScouting)
	{
		return m_piYieldFromScouting[i];
	}

	return 0;
}
int CvPromotionEntry::GetYieldFromKills(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < NUM_YIELD_TYPES && m_piYieldFromKills)
	{
		return m_piYieldFromKills[i];
	}

	return 0;
}

int CvPromotionEntry::GetYieldFromCombatExperienceTimes100(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if (i > -1 && i < NUM_YIELD_TYPES && m_piYieldFromCombatExperienceTimes100)
	{
		return m_piYieldFromCombatExperienceTimes100[i];
	}

	return 0;
}

int CvPromotionEntry::GetGarrisonYield(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < NUM_YIELD_TYPES && m_piGarrisonYield)
	{
		return m_piGarrisonYield[i];
	}

	return 0;
}

int CvPromotionEntry::GetFortificationYield(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if (i > -1 && i < NUM_YIELD_TYPES && m_piFortificationYield)
	{
		return m_piFortificationYield[i];
	}

	return 0;
}

int CvPromotionEntry::GetYieldFromBarbarianKills(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < NUM_YIELD_TYPES && m_piYieldFromBarbarianKills)
	{
		return m_piYieldFromBarbarianKills[i];
	}

	return 0;
}

/// Percentage bonus when fighting against a specific unit *combat* class
int CvPromotionEntry::GetUnitCombatModifierPercent(int i) const
{
	PRECONDITION(i < GC.getNumUnitCombatClassInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumUnitCombatClassInfos() && m_piUnitCombatModifierPercent)
	{
		return m_piUnitCombatModifierPercent[i];
	}

	return 0;
}

/// Percentage bonus when attacking a specific unit *combat* class
int CvPromotionEntry::GetUnitCombatModifierPercentAttack(int i) const
{
	PRECONDITION(i < GC.getNumUnitCombatClassInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumUnitCombatClassInfos() && m_piUnitCombatModifierPercentAttack)
	{
		return m_piUnitCombatModifierPercentAttack[i];
	}

	return 0;
}

/// Percentage bonus when defending against a specific unit *combat* class
int CvPromotionEntry::GetUnitCombatModifierPercentDefense(int i) const
{
	PRECONDITION(i < GC.getNumUnitCombatClassInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumUnitCombatClassInfos() && m_piUnitCombatModifierPercentDefense)
	{
		return m_piUnitCombatModifierPercentDefense[i];
	}

	return 0;
}

/// Percentage bonus when fighting against a specific unit class
int CvPromotionEntry::GetUnitClassModifierPercent(int i) const
{
	PRECONDITION(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumUnitClassInfos() && m_piUnitClassModifierPercent)
	{
		return m_piUnitClassModifierPercent[i];
	}

	return 0;
}

/// Percentage bonus when attacking a specific unit class
int CvPromotionEntry::GetUnitClassAttackModifier(int i) const
{
	PRECONDITION(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumUnitClassInfos() && m_piUnitClassAttackModifier)
	{
		return m_piUnitClassAttackModifier[i];
	}

	return 0;
}

/// Percentage bonus when defending against a specific unit class
int CvPromotionEntry::GetUnitClassDefenseModifier(int i) const
{
	PRECONDITION(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumUnitClassInfos() && m_piUnitClassDefenseModifier)
	{
		return m_piUnitClassDefenseModifier[i];
	}

	return 0;
}

/// Percentage bonus when fighting against a unit with a specific domain (LAND/SEA/AIR)
int CvPromotionEntry::GetDomainModifierPercent(int i) const
{
	PRECONDITION(i < NUM_DOMAIN_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < NUM_DOMAIN_TYPES && m_piDomainModifierPercent)
	{
		return m_piDomainModifierPercent[i];
	}

	return 0;
}

/// Percentage bonus when attacking a unit with a specific domain (LAND/SEA/AIR)
int CvPromotionEntry::GetDomainAttackPercent(int i) const
{
	PRECONDITION(i < NUM_DOMAIN_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < NUM_DOMAIN_TYPES && m_piDomainAttackPercent)
	{
		return m_piDomainAttackPercent[i];
	}

	return 0;
}

/// Percentage bonus when defending against a unit with a specific domain (LAND/SEA/AIR)
int CvPromotionEntry::GetDomainDefensePercent(int i) const
{
	PRECONDITION(i < NUM_DOMAIN_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < NUM_DOMAIN_TYPES && m_piDomainDefensePercent)
	{
		return m_piDomainDefensePercent[i];
	}

	return 0;
}

/// Percentage bonus when fighting next to friendly unit *combat* classes (increases with more adjacent units)
int CvPromotionEntry::GetCombatModPerAdjacentUnitCombatModifierPercent(int i) const
{
	PRECONDITION(i < GC.getNumUnitCombatClassInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumUnitCombatClassInfos() && m_piCombatModPerAdjacentUnitCombatModifierPercent)
	{
		return m_piCombatModPerAdjacentUnitCombatModifierPercent[i];
	}

	return 0;
}

/// Percentage bonus when attacking next to friendly unit *combat* classes (increases with more adjacent units)
int CvPromotionEntry::GetCombatModPerAdjacentUnitCombatAttackModifier(int i) const
{
	PRECONDITION(i < GC.getNumUnitCombatClassInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumUnitCombatClassInfos() && m_piCombatModPerAdjacentUnitCombatAttackModifier)
	{
		return m_piCombatModPerAdjacentUnitCombatAttackModifier[i];
	}

	return 0;
}

/// Percentage bonus when defending next to friendly unit *combat* classes (increases with more adjacent units) [not enemy as intended]
int CvPromotionEntry::GetCombatModPerAdjacentUnitCombatDefenseModifier(int i) const
{
	PRECONDITION(i < GC.getNumUnitCombatClassInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumUnitCombatClassInfos() && m_piCombatModPerAdjacentUnitCombatDefenseModifier)
	{
		return m_piCombatModPerAdjacentUnitCombatDefenseModifier[i];
	}

	return 0;
}

/// Immediately gain these Yields when the promotion is gained
std::pair<int, bool> CvPromotionEntry::GetInstantYields(int i) const
{
	PRECONDITION(i < NUM_YIELD_TYPES, "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if (i > -1 && i < NUM_YIELD_TYPES && !m_piInstantYields.empty())
	{
		std::map<int, std::pair<int, bool>>::const_iterator it = m_piInstantYields.find(i);
		if (it != m_piInstantYields.end()) // find returns the iterator to map::end if the key iYield is not present in the map
		{
			return it->second;
		}
	}

	return std::make_pair(0, false);
}

/// Indicates if a feature type is traversable by the unit
int CvPromotionEntry::GetFeaturePassableTech(int i) const
{
	PRECONDITION(i < GC.getNumFeatureInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumFeatureInfos() && m_piFeaturePassableTech)
	{
		return m_piFeaturePassableTech[i];
	}

	return -1;
}

/// Indicates if a unit ignores terrain cost in a type of terrain
bool CvPromotionEntry::GetIgnoreTerrainCostIn(int i) const
{
	PRECONDITION(i < GC.getNumTerrainInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumTerrainInfos() && m_pbIgnoreTerrainCostIn)
	{
		return m_pbIgnoreTerrainCostIn[i];
	}

	return false;
}

/// Indicates if a unit ignores terrain cost from a type of terrain
bool CvPromotionEntry::GetIgnoreTerrainCostFrom(int i) const
{
	PRECONDITION(i < GC.getNumTerrainInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumTerrainInfos() && m_pbIgnoreTerrainCostFrom)
	{
		return m_pbIgnoreTerrainCostFrom[i];
	}

	return false;
}

/// Indicates if a unit ignores terrain cost in a type of terrain feature
bool CvPromotionEntry::GetIgnoreFeatureCostIn(int i) const
{
	PRECONDITION(i < GC.getNumFeatureInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumFeatureInfos() && m_pbIgnoreFeatureCostIn)
	{
		return m_pbIgnoreFeatureCostIn[i];
	}

	return false;
}

/// Indicates if a unit ignores terrain cost in a type of terrain feature
bool CvPromotionEntry::GetIgnoreFeatureCostFrom(int i) const
{
	PRECONDITION(i < GC.getNumFeatureInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumFeatureInfos() && m_pbIgnoreFeatureCostFrom)
	{
		return m_pbIgnoreFeatureCostFrom[i];
	}

	return false;
}

/// Indicates if a unit can move twice as fast in a type of terrain
bool CvPromotionEntry::GetTerrainDoubleMove(int i) const
{
	PRECONDITION(i < GC.getNumTerrainInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumTerrainInfos() && m_pbTerrainDoubleMove)
	{
		return m_pbTerrainDoubleMove[i];
	}

	return false;
}

/// Indicates if a unit can move twice as fast in a type of terrain feature
bool CvPromotionEntry::GetFeatureDoubleMove(int i) const
{
	PRECONDITION(i < GC.getNumFeatureInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumFeatureInfos() && m_pbFeatureDoubleMove)
	{
		return m_pbFeatureDoubleMove[i];
	}

	return false;
}

#if defined(MOD_PROMOTIONS_HALF_MOVE)
/// Indicates if a unit can move half as fast in a type of terrain
bool CvPromotionEntry::GetTerrainHalfMove(int i) const
{
	PRECONDITION(i < GC.getNumTerrainInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumTerrainInfos() && m_pbTerrainHalfMove)
	{
		return m_pbTerrainHalfMove[i];
	}

	return false;
}

/// Indicates if a unit loses one movement point when entering a given terrain
bool CvPromotionEntry::GetTerrainExtraMove(int i) const
{
	PRECONDITION(i < GC.getNumTerrainInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if (i > -1 && i < GC.getNumTerrainInfos() && m_pbTerrainExtraMove)
	{
		return m_pbTerrainExtraMove[i];
	}

	return false;
}

/// Indicates if a unit can move half as fast in a type of feature
bool CvPromotionEntry::GetFeatureHalfMove(int i) const
{
	PRECONDITION(i < GC.getNumFeatureInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumFeatureInfos() && m_pbFeatureHalfMove)
	{
		return m_pbFeatureHalfMove[i];
	}

	return false;
}

/// Indicates if a unit loses one movement point when entering a given feature
bool CvPromotionEntry::GetFeatureExtraMove(int i) const
{
	PRECONDITION(i < GC.getNumFeatureInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if (i > -1 && i < GC.getNumFeatureInfos() && m_pbFeatureExtraMove)
	{
		return m_pbFeatureExtraMove[i];
	}

	return false;
}
#endif
/// Indicates if a unit can heal twice as fast in a type of terrain
bool CvPromotionEntry::GetTerrainDoubleHeal(int i) const
{
	PRECONDITION(i < GC.getNumTerrainInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumTerrainInfos() && m_pbTerrainDoubleHeal)
	{
		return m_pbTerrainDoubleHeal[i];
	}

	return false;
}

/// Indicates if a unit can heal twice as fast in a type of feature
bool CvPromotionEntry::GetFeatureDoubleHeal(int i) const
{
	PRECONDITION(i < GC.getNumFeatureInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumFeatureInfos() && m_pbFeatureDoubleHeal)
	{
		return m_pbFeatureDoubleHeal[i];
	}

	return false;
}
/// Indicates if a terrain type is impassable
bool CvPromotionEntry::GetTerrainImpassable(int i) const
{
	PRECONDITION(i < GC.getNumTerrainInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumTerrainInfos() && m_pbTerrainImpassable)
	{
		return m_pbTerrainImpassable[i];
	}

	return false;
}

/// Indicates what tech is needed to pass through a terrain type
int CvPromotionEntry::GetTerrainPassableTech(int i) const
{
	PRECONDITION(i < GC.getNumTerrainInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumTerrainInfos() && m_piTerrainPassableTech)
	{
		return m_piTerrainPassableTech[i];
	}

	return -1;
}

/// Indicates if a feature type is impassable
bool CvPromotionEntry::GetFeatureImpassable(int i) const
{
	PRECONDITION(i < GC.getNumFeatureInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumFeatureInfos() && m_pbFeatureImpassable)
	{
		return m_pbFeatureImpassable[i];
	}

	return false;
}

/// Returns the combat classes that this promotion is available for
bool CvPromotionEntry::GetUnitCombatClass(int i) const
{
	PRECONDITION(i < GC.getNumUnitCombatClassInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumUnitCombatClassInfos() && m_pbUnitCombat)
	{
		return m_pbUnitCombat[i];
	}

	return false;
}

/// Returns the civilian unit type that this promotion is available for
bool CvPromotionEntry::GetCivilianUnitType(int i) const
{
	PRECONDITION(i < GC.getNumUnitInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumUnitInfos() && m_pbCivilianUnitType)
	{
		return m_pbCivilianUnitType[i];
	}

	return false;
}

/// Returns the amount of a given yield to receive when a unit that has this promotion pillages a tile.
/// 
/// The first element of the pair is the flat amount, the second element is the era scaling amount.
std::pair<int, int> CvPromotionEntry::GetYieldFromPillage(YieldTypes eYield) const
{
	PRECONDITION(eYield < NUM_YIELD_TYPES, "Yield index out of bounds");
	PRECONDITION(eYield > NO_YIELD, "Yield index out of bounds");

	if (eYield < NUM_YIELD_TYPES && eYield > NO_YIELD)
	{
		std::map<int, std::pair<int, int>>::const_iterator it = m_yieldFromPillage.find(static_cast<int>(eYield));
		if (it != m_yieldFromPillage.end())
		{
			return it->second;
		}
	}

	return std::make_pair(0, 0);
}

std::set<int> CvPromotionEntry::GetBlockedPromotions() const
{
	return m_siBlockedPromotions;
}

std::vector<PlagueInfo> CvPromotionEntry::GetPlagues() const
{
	return m_vsPlagues;
}

#if defined(MOD_PROMOTIONS_UNIT_NAMING)
/// If this a promotion that names a unit
bool CvPromotionEntry::IsUnitNaming(int i) const
{
	PRECONDITION(i < GC.getNumPromotionInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_pbUnitName ? m_pbUnitName[i] : false;
}

void CvPromotionEntry::GetUnitName(UnitTypes eUnit, CvString& sUnitName) const
{
	Database::Results kDBResults;
	char szQuery[512] = {0};
	sprintf_s(szQuery, "select Name from UnitPromotions_UnitName n, UnitPromotions p, Units u where n.PromotionType = p.Type and n.UnitType = u.Type and p.ID = %i and u.ID = %i;", GetID(), eUnit);
	if(DB.Execute(kDBResults, szQuery))
	{
		while(kDBResults.Step())
		{
			sUnitName = kDBResults.GetText("Name");
		}
	}
}
#endif

/// If this a promotion that can randomly turn into other c
bool CvPromotionEntry::IsPostCombatRandomPromotion(int i) const
{
	PRECONDITION(i < GC.getNumPromotionInfos(), "Index out of bounds");
	PRECONDITION(i > -1, "Index out of bounds");
	return m_pbPostCombatRandomPromotion ? m_pbPostCombatRandomPromotion[i] : false;
}

//=====================================
// CvPromotionEntryXMLEntries
//=====================================
/// Constructor
CvPromotionXMLEntries::CvPromotionXMLEntries(void)
{

}

/// Destructor
CvPromotionXMLEntries::~CvPromotionXMLEntries(void)
{
	DeleteArray();
}

/// Returns vector of promotion entries
std::vector<CvPromotionEntry*>& CvPromotionXMLEntries::GetPromotionEntries()
{
	return m_paPromotionEntries;
}

/// Number of defined projects
int CvPromotionXMLEntries::GetNumPromotions()
{
	return m_paPromotionEntries.size();
}

/// Get a specific entry
CvPromotionEntry* CvPromotionXMLEntries::GetEntry(int index)
{
	return (index!=NO_PROMOTION) ? m_paPromotionEntries[index] : NULL;
}

/// Clear promotion entries
void CvPromotionXMLEntries::DeleteArray()
{
	for(std::vector<CvPromotionEntry*>::iterator it = m_paPromotionEntries.begin(); it != m_paPromotionEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paPromotionEntries.clear();
}

//=====================================
// CvUnitPromotions
//=====================================
/// Constructor
CvUnitPromotions::CvUnitPromotions()
{
}

/// Destructor
CvUnitPromotions::~CvUnitPromotions(void)
{
}

/// Initialize
void CvUnitPromotions::Init()
{
	Reset();
}

/// Deallocate memory created in initialize
void CvUnitPromotions::Uninit()
{
	Reset();
}

/// Reset unit promotion array to all false
void CvUnitPromotions::Reset()
{
	m_kHasPromotion.SetSize(0);
	m_kPromotionActive.SetSize(0);

	m_unitClassDefenseMod.clear();
	m_unitClassAttackMod.clear();
	m_terrainPassableCache.clear();
	m_bTerrainPassable = false;
	m_featurePassableCache.clear();
	m_bFeaturePassable = false;
}

///
template<typename UnitPromotions, typename Visitor>
void CvUnitPromotions::Serialize(UnitPromotions& unitPromotions, Visitor& visitor)
{
	visitor(unitPromotions.m_kHasPromotion);
	visitor(unitPromotions.m_kPromotionActive);
}

/// Serialization read
void CvUnitPromotions::Read(FDataStream& kStream)
{
	Reset();

	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);

	UpdateCache();
}

/// Serialization write
void CvUnitPromotions::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& stream, CvUnitPromotions& unitPromotions)
{
	unitPromotions.Read(stream);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvUnitPromotions& unitPromotions)
{
	unitPromotions.Write(stream);
	return stream;
}

/// Accessor: Does the unit have a certain promotion
bool CvUnitPromotions::HasPromotion(PromotionTypes eIndex) const
{
	PRECONDITION(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	PRECONDITION(eIndex < GC.getNumPromotionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(eIndex >= 0 && eIndex < GC.getNumPromotionInfos())
	{
		return m_kHasPromotion.GetBit(eIndex);
	}

	return false;
}

/// Sets the promotion to a certain value
void CvUnitPromotions::SetPromotion(PromotionTypes eIndex, bool bValue)
{
	PRECONDITION(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	PRECONDITION(eIndex < GC.getNumPromotionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(eIndex >= 0 && eIndex < GC.getNumPromotionInfos())
	{
		m_kHasPromotion.SetBit(eIndex, bValue);
	}

	UpdateCache();
}

/// Accessor: Does the unit have a certain promotion
bool CvUnitPromotions::IsPromotionActive(PromotionTypes eIndex) const
{
	PRECONDITION(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	PRECONDITION(eIndex < GC.getNumPromotionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (eIndex >= 0 && eIndex < GC.getNumPromotionInfos())
	{
		return m_kPromotionActive.GetBit(eIndex);
	}

	return false;
}

/// Sets the promotion to a certain value
void CvUnitPromotions::SetPromotionActive(PromotionTypes eIndex, bool bValue)
{
	PRECONDITION(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	PRECONDITION(eIndex < GC.getNumPromotionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (eIndex >= 0 && eIndex < GC.getNumPromotionInfos())
	{
		m_kPromotionActive.SetBit(eIndex, bValue);
	}
	
	UpdateCache();
}

/// determines if the terrain type is passable given the unit's current promotions
void CvUnitPromotions::UpdateCache()
{
	m_terrainPassableCache.clear();
	m_bTerrainPassable = false;
	m_featurePassableCache.clear();
	m_bFeaturePassable = false;

	for(int iTerrain = 0; iTerrain < GC.getNumTerrainInfos(); iTerrain++)
	{
		std::vector<TechTypes> reqTechs;
		for(int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); iPromotion++)
		{
			PromotionTypes ePromotion = (PromotionTypes)iPromotion;
			if (HasPromotion(ePromotion) && IsPromotionActive(ePromotion))
			{
				CvPromotionEntry* promotion = GC.getPromotionInfo((PromotionTypes)iPromotion);
				if(promotion)
				{
					TechTypes eTech = (TechTypes) promotion->GetTerrainPassableTech(iTerrain);
					if(eTech != NO_TECH)
					{
						reqTechs.push_back(eTech);
						m_bTerrainPassable = true;
					}
				}
			}
		}
		
		m_terrainPassableCache.push_back( reqTechs );
	}

	for(int iFeature = 0; iFeature < GC.getNumFeatureInfos(); iFeature++)
	{
		std::vector<TechTypes> reqTechs;
		for(int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); iPromotion++)
		{
			PromotionTypes ePromotion = (PromotionTypes)iPromotion;
			if (HasPromotion(ePromotion) && IsPromotionActive(ePromotion))
			{
				CvPromotionEntry* promotion = GC.getPromotionInfo(ePromotion);
				if(promotion)
				{
					TechTypes eTech = (TechTypes) promotion->GetFeaturePassableTech(iFeature);
					if (eTech != NO_TECH)
					{
						reqTechs.push_back(eTech);
						m_bFeaturePassable = true;
					}
				}
			}
		}

		m_featurePassableCache.push_back( reqTechs );
	}
}

/// determines if the unit has a promotion that makes a feature passable
bool CvUnitPromotions::HasAllowFeaturePassable() const
{
	return m_bFeaturePassable;
}

/// determines if the feature is passable given the unit's current promotions and tech level
bool CvUnitPromotions::GetAllowFeaturePassable(FeatureTypes eFeatureType, TeamTypes eTeam) const
{
	CvTeamTechs* teamTechs = eTeam != NO_TEAM ? GET_TEAM(eTeam).GetTeamTechs() : NULL;
	if (!teamTechs)
		return false;

	std::vector<TechTypes> reqTechs = m_featurePassableCache[eFeatureType];
	for (std::vector<TechTypes>::iterator it_techs = reqTechs.begin(); it_techs != reqTechs.end(); ++it_techs)
		if (teamTechs->HasTech(*it_techs))
			return true;

	//have none of the techs?
	return false;
}

/// determines if the unit has a promotion that makes a terrain passable
bool CvUnitPromotions::HasAllowTerrainPassable() const
{
	return m_bTerrainPassable;
}
/// determines if the terrain is passable given the unit's current promotions and tech level
bool CvUnitPromotions::GetAllowTerrainPassable(TerrainTypes eTerrainType, TeamTypes eTeam) const
{
	CvTeamTechs* teamTechs = eTeam != NO_TEAM ? GET_TEAM(eTeam).GetTeamTechs() : NULL;
	if(!teamTechs) 
		return false;

	std::vector<TechTypes> reqTechs = m_terrainPassableCache[eTerrainType];
	for ( std::vector<TechTypes>::iterator it_techs = reqTechs.begin(); it_techs != reqTechs.end(); ++it_techs )
		if (teamTechs->HasTech(*it_techs))
			return true;

	//have none of the techs?
	return false;
}

/// returns the advantage percent when attacking the specified unit class
int CvUnitPromotions::GetUnitClassAttackMod(UnitClassTypes eUnitClass) const
{
	if ((size_t)eUnitClass<m_unitClassAttackMod.size())
		return m_unitClassAttackMod[eUnitClass];

	int iSum = 0;
	for(int iLoop = 0; iLoop < GC.getNumPromotionInfos(); iLoop++)
	{
		PromotionTypes ePromotion = (PromotionTypes)iLoop;
		CvPromotionEntry* promotion = GC.getPromotionInfo(ePromotion);
		if(promotion && HasPromotion(ePromotion))
		{
			iSum += promotion->GetUnitClassAttackModifier(eUnitClass);
		}
	}

	return iSum;
}

/// returns the advantage percent when defending against the specified unit class
int CvUnitPromotions::GetUnitClassDefenseMod(UnitClassTypes eUnitClass) const
{
	if ((size_t)eUnitClass<m_unitClassDefenseMod.size())
		return m_unitClassDefenseMod[eUnitClass];

	int iSum = 0;
	for(int iLoop = 0; iLoop < GC.getNumPromotionInfos(); iLoop++)
	{
		PromotionTypes ePromotion = (PromotionTypes)iLoop;
		CvPromotionEntry* promotion = GC.getPromotionInfo(ePromotion);
		if(promotion && HasPromotion(ePromotion))
		{
			iSum += promotion->GetUnitClassDefenseModifier(eUnitClass);
		}
	}

	return iSum;
}

// Swap to a new promotion after a combat - returns new promotion we switched to
PromotionTypes CvUnitPromotions::ChangePromotionAfterCombat(PromotionTypes eIndex, CvUnit* pThisUnit)
{
	CvPromotionEntry *pkEntry = GC.GetGamePromotions()->GetEntry(eIndex);
	if (!pkEntry || !pThisUnit)
		return NO_PROMOTION;

	std::vector<int> aPossiblePromotions;
	for (int iI = 0; iI < GC.GetGamePromotions()->GetNumPromotions(); iI++)
	{
		if (pkEntry->IsPostCombatRandomPromotion(iI))
		{
			if (!pkEntry->ArePostCombatPromotionsExclusive() || !IsInUseByPlayer((PromotionTypes)iI, pThisUnit->getOwner()))
			{
#if defined(MOD_EVENTS_UNIT_UPGRADES)
				if (MOD_EVENTS_UNIT_UPGRADES) {
					if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_CanHavePromotion, pThisUnit->getOwner(), pThisUnit->GetID(), iI) == GAMEEVENTRETURN_FALSE) {
						continue;
					}

					if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_UnitCanHavePromotion, pThisUnit->getOwner(), pThisUnit->GetID(), iI) == GAMEEVENTRETURN_FALSE) {
						continue;
					}
				}
#endif
				aPossiblePromotions.push_back(iI);
			}
		}
	}

	uint uNumChoices = aPossiblePromotions.size();
	if (uNumChoices > 0)
	{
		uint uChoice = GC.getGame().urandLimitExclusive(uNumChoices, pThisUnit->plot()->GetPseudoRandomSeed().mix(pThisUnit->GetID()).mix(pThisUnit->getDamage()));
		return static_cast<PromotionTypes>(aPossiblePromotions[uChoice]);
	}

	return NO_PROMOTION;
}

// PRIVATE METHODS

// Is this (post-combat) promotion already present for some unit of this player?
bool CvUnitPromotions::IsInUseByPlayer(PromotionTypes eIndex, PlayerTypes ePlayer)
{
	CvPlayer &kPlayer = GET_PLAYER(ePlayer);
	int iValueLoop = 0;
	for(CvUnit* pLoopUnit = kPlayer.firstUnit(&iValueLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iValueLoop))
	{
		if (pLoopUnit->isHasPromotion(eIndex))
		{
			return true;
		}
	}

	return false;
}

// Read the saved promotions.  Entries are saved as string values, all entries are saved.
void PromotionArrayHelpers::ReadV3(FDataStream& kStream, CvBitfield& kPromotions)
{
	int iNumEntries = 0;
	CvString sTemp;
	int iType = 0;

	kStream >> iNumEntries;

	kPromotions.SetSize( iNumEntries );

	for(int iI = 0; iI < iNumEntries; iI++)
	{
		kStream >> sTemp;
		if(sTemp == "PROMOTION_OLIGARCHY")
		{
			bool bTemp = false;
			kStream >> bTemp;
		}
		else
		{
			iType = GC.getInfoTypeForString(sTemp);
			if(iType != -1)
			{
				bool bValue = false;
				kStream >> bValue;
				kPromotions.SetBit(iType, bValue);
			}
			else
			{
				CvString szError;
				szError.Format("LOAD ERROR: Promotion Type not found: %s", sTemp.c_str());
				GC.LogMessage(szError.GetCString());
				ASSERT(false, szError);
				bool bDummy = false;
				kStream >> bDummy;
			}
		}
	}
}

// Read the saved promotions.  Entries are saved as hash values
void PromotionArrayHelpers::Read(FDataStream& kStream, CvBitfield& kPromotions)
{
	int iNumEntries = 0;
	int iType = 0;

	kStream >> iNumEntries;

	kPromotions.SetSize( iNumEntries );

	uint uiHashTemp = 0;
	uint uiOligarchyHash = FStringHash("PROMOTION_OLIGARCHY");
	for(int iI = 0; iI < iNumEntries; iI++)
	{
		kStream >> uiHashTemp;
		if(uiHashTemp == uiOligarchyHash)
		{
			bool bTemp = false;
			kStream >> bTemp;
		}
		else
		{
			iType = GC.getInfoTypeForHash(uiHashTemp);
			if(iType != -1)
			{
				bool bValue = false;
				kStream >> bValue;
				kPromotions.SetBit(iType, bValue);
			}
			else
			{
				CvString szError;
				szError.Format("LOAD ERROR: Promotion Type not found for hash: %u", uiHashTemp);
				GC.LogMessage(szError.GetCString());
				ASSERT(false, szError);
				bool bDummy = false;
				kStream >> bDummy;
			}
		}
	}
}

// Save the promotions.  Entries are saved as hash values and only the entries that are 'on' are saved
void PromotionArrayHelpers::Write(FDataStream& kStream, const CvBitfield& kPromotions, int iArraySize)
{
	// We are only going to save the 'on' bit, so we have to count them
	int iCount = 0;

	for(int iI = 0; iI < iArraySize; iI++)
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(iI);
		if (kPromotions.GetBit(ePromotion) && GC.getPromotionInfo(ePromotion) != NULL)
			++iCount;
	}

	kStream << iCount;

	for(int iI = 0; iI < iArraySize; iI++)
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(iI);
		bool bValue = kPromotions.GetBit(ePromotion);
		if (bValue)
		{
			CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
			if(pkPromotionInfo)
			{
				uint uiHash = FStringHash( pkPromotionInfo->GetType() );		// Save just the hash
				kStream << uiHash;
				kStream << bValue;
			}
		}
	}
}
