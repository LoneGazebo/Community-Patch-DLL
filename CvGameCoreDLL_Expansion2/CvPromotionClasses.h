/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_PROMOTION_CLASSES_H
#define CIV5_PROMOTION_CLASSES_H

#include "CvBitfield.h"

struct PlagueInfo
{
	bool operator==(const PlagueInfo& rhs) const;
	bool operator<(const PlagueInfo& rhs) const;
	template<typename PlagueInfoTemplate, typename Visitor>
	static void Serialize(PlagueInfoTemplate& plagueInfo, Visitor& visitor);

	PromotionTypes ePlague;
	DomainTypes eDomain;
	bool bApplyOnAttack;
	bool bApplyOnDefense;
	int iApplyChance;
};
FDataStream& operator>>(FDataStream&, PlagueInfo&);
FDataStream& operator<<(FDataStream&, const PlagueInfo&);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvPromotionEntry
//!  \brief		A single promotion available in the game
//
//!  Key Attributes:
//!  - Used to be called CvPromotionInfo
//!  - Populated from XML\Units\CIV5UnitPromotions.xml
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPromotionEntry: public CvHotKeyInfo
{
public:
	CvPromotionEntry(void);
	virtual ~CvPromotionEntry(void);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	int	GetLayerAnimationPath() const;
	int	GetPrereqPromotion() const;
	void	SetPrereqPromotion(int i);
	int	GetPrereqOrPromotion1() const;
	void	SetPrereqOrPromotion1(int i);
	int	GetPrereqOrPromotion2() const;
	void	SetPrereqOrPromotion2(int i);
	int	GetPrereqOrPromotion3() const;
	void	SetPrereqOrPromotion3(int i);
	int	GetPrereqOrPromotion4() const;
	void	SetPrereqOrPromotion4(int i);
	int	GetPrereqOrPromotion5() const;
	void	SetPrereqOrPromotion5(int i);
	int	GetPrereqOrPromotion6() const;
	void	SetPrereqOrPromotion6(int i);
	int	GetPrereqOrPromotion7() const;
	void	SetPrereqOrPromotion7(int i);
	int	GetPrereqOrPromotion8() const;
	void	SetPrereqOrPromotion8(int i);
	int	GetPrereqOrPromotion9() const;
	void	SetPrereqOrPromotion9(int i);

	int  GetTechPrereq() const;
	int  GetInvisibleType() const;
	int  GetSeeInvisibleType() const;
	int  GetVisibilityChange() const;
#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
	int  GetReconChange() const;
#endif
	int  GetMovesChange() const;
	int  GetMoveDiscountChange() const;
	int  GetRangeChange() const;
	int  GetRangedAttackModifier() const;
	int  GetInterceptionCombatModifier() const;
	int  GetInterceptionDefenseDamageModifier() const;
	int  GetAirSweepCombatModifier() const;
	int  GetInterceptChanceChange() const;
	int  GetNumInterceptionChange() const;
#if defined(MOD_BALANCE_CORE)
	int  GetAirInterceptRangeChange() const; // JJ: This is new
#endif
	int  GetEvasionChange() const;
	int  GetCargoChange() const;
	int  GetEnemyHealChange() const;
	int  GetNeutralHealChange() const;
	int  GetFriendlyHealChange() const;
	int  GetSameTileHealChange() const;
	int  GetAdjacentTileHealChange() const;
	int  GetEnemyDamageChance() const;
	int  GetNeutralDamageChance() const;
	int  GetEnemyDamage() const;
	int  GetNeutralDamage() const;
	int  GetCombatPercent() const;
	int  GetCityAttackPercent() const;
	int  GetCityDefensePercent() const;
	int  GetRangedDefenseMod() const;
	int  GetHillsAttackPercent() const;
	int  GetHillsDefensePercent() const;
	int  GetOpenAttackPercent() const;
	int  GetOpenFromPercent() const;
	int  GetOpenRangedAttackMod() const;
	int  GetRoughAttackPercent() const;
	int  GetRoughFromPercent() const;
	int  GetRoughRangedAttackMod() const;
	int  GetAttackFortifiedMod() const;
	int  GetAttackWoundedMod() const;
	int  GetAttackFullyHealedMod() const;
	int  GetAttackAboveHealthMod() const;
	int  GetAttackBelowHealthMod() const;
	bool IsRangedFlankAttack() const;
	int  GetExtraFlankPower() const;
	int  GetFlankAttackModifier() const;
	int  GetNearbyEnemyCombatMod() const;
	int  GetNearbyEnemyCombatRange() const;
	int  GetOpenDefensePercent() const;
	int  GetRoughDefensePercent() const;
	int  GetExtraAttacks() const;
	bool IsGreatGeneral() const;
	bool IsGreatAdmiral() const;
	int  GetAuraRangeChange() const;
	int  GetAuraEffectChange() const;
	int  GetNumRepairCharges() const;
	int  GetMilitaryCapChange() const;
	int  GetGreatGeneralModifier() const;
	bool IsGreatGeneralReceivesMovement() const;
	int  GetGreatGeneralCombatModifier() const;
	int  GetFriendlyLandsModifier() const;
	int  GetFriendlyLandsAttackModifier() const;
	int  GetOutsideFriendlyLandsModifier() const;
	int  GetCommandType() const;
	void SetCommandType(int iNewType);

	bool IsNoSupply() const;

	int GetMaxHitPointsChange() const;
	int GetMaxHitPointsModifier() const;
	int GetVsUnhappyMod() const;
	int GetUpgradeDiscount() const;
	int GetExperiencePercent() const;
	int GetAdjacentMod() const;
	int GetNoAdjacentUnitMod() const;
	int GetAttackMod() const;
	int GetDefenseMod() const;
	int GetBorderMod() const;
	int GetMarriageMod() const;
	int GetMarriageModCap() const;
	int GetGroundAttackDamage() const;

	int GetDropRange() const;
	int GetExtraNavalMoves() const;
	int GetHPHealedIfDefeatEnemy() const;
	int GetGoldenAgeValueFromKills() const;
	int GetExtraWithdrawal() const;
	int GetCombatChange() const;
	int GetTileDamageIfNotMoved() const;
	int GetFortifiedModifier() const;
	int GetMinEffectiveHealth() const;
	bool IsRequiresLeadership() const;
	bool IsCannotHeal() const;
	bool IsPillageFortificationsOnKill() const;
#if defined(MOD_BALANCE_CORE_JFD)
	int GetPlagueID() const;
	int GetPlaguePriority() const;
#endif
	int GetEmbarkExtraVisibility() const;
	int GetEmbarkDefenseModifier() const;
	int GetCapitalDefenseModifier() const;
	int GetCapitalDefenseFalloff() const;
	int GetCityAttackPlunderModifier() const;
	int GetReligiousStrengthLossRivalTerritory() const;
	
	int GetTradeMissionInfluenceModifier() const;
	int GetTradeMissionGoldModifier() const;
#if defined(MOD_BALANCE_CORE)
	int GetDiploMissionInfluence() const;
	int GetCaptureDefeatedEnemyChance() const;
	int GetBarbarianCombatBonus() const;
	int GetGoodyHutYieldBonus() const;
	bool IsGainsXPFromScouting() const;
	int GetXPFromPillaging() const;
	int GetExtraXPOnKill() const;
	bool IsGainsXPFromSpotting() const;
	int NegatesPromotion() const;
	bool CannotBeCaptured() const;
	bool IsLostOnMove() const;
	int ForcedDamageValue() const;
	int ChangeDamageValue() const;
	int PromotionDuration() const;
	bool IsCityStateOnly() const;
	bool IsBarbarianOnly() const;
	int GetMoraleBreakChance() const;
	int GetDamageAoEFortified() const;
	int GetWorkRateMod() const;
	UnitClassTypes GetCombatBonusFromNearbyUnitClass() const;
	int GetNearbyUnitClassBonusRange() const;
	int GetNearbyUnitClassBonus() const;
	int GetWonderProductionModifier() const;
	bool IsStrongerDamaged() const;
	bool IsFightWellDamaged() const;
	bool IsMountainsDoubleMove() const;
	bool IsEmbarkFlatCost() const;
	bool IsDisembarkFlatCost() const;
	bool IsMountedOnly() const;
	int GetAOEDamageOnKill() const;
	int GetAOEDamageOnPillage() const;
	int GetAOEHealOnPillage() const;
	int GetCombatModPerCSAlliance() const;
	int GetAttackModPerSamePromotionAttack() const;
	int GetAttackModPerSamePromotionAttackCap() const;
	int GetAoEDamageOnMove() const;
	int GetPartialHealOnPillage() const;
	int GetSplashDamage() const;
	int GetMinRange() const;
	int GetMaxRange() const;
	int GetMultiAttackBonus() const;
	int GetLandAirDefenseValue() const;
	int GetDamageReductionCityAssault() const;
#endif
	bool IsCannotBeChosen() const;
	bool IsLostWithUpgrade() const;
	bool IsNotWithUpgrade() const;
	bool IsInstaHeal() const;
	bool IsLeader() const;
	bool IsBlitz() const;
	bool IsAmphib() const;
	bool IsRiver() const;
	bool IsEnemyRoute() const;
	bool IsRivalTerritory() const;
	bool IsMustSetUpToRangedAttack() const;
	bool IsRangedSupportFire() const;
	bool IsAlwaysHeal() const;
	bool IsHealOutsideFriendly() const;
	bool IsHillsDoubleMove() const;
	bool IsRiverDoubleMove() const;

	bool IsIgnoreTerrainCost() const;
	bool IsIgnoreTerrainDamage() const;
	bool IsIgnoreFeatureDamage() const;
	bool IsExtraTerrainDamage() const;
	bool IsExtraFeatureDamage() const;
#if defined(MOD_PROMOTIONS_IMPROVEMENT_BONUS)
	int GetNearbyImprovementCombatBonus() const;
	int GetNearbyImprovementBonusRange() const;
	ImprovementTypes GetCombatBonusImprovement() const;
#endif
#if defined(MOD_PROMOTIONS_CROSS_MOUNTAINS)
	bool CanCrossMountains() const;
#endif
#if defined(MOD_PROMOTIONS_CROSS_OCEANS)
	bool CanCrossOceans() const;
#endif
#if defined(MOD_PROMOTIONS_CROSS_ICE)
	bool CanCrossIce() const;
#endif
#if defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
	bool IsGGFromBarbarians() const;
#endif
	bool IsRoughTerrainEndsTurn() const;
	bool IsCapturedUnitsConscripted() const;
	bool IsHoveringUnit() const;
	bool IsFlatMovementCost() const;
	bool IsCanMoveImpassable() const;
	bool IsNoCapture() const;
	bool IsOnlyDefensive() const;
	bool IsNoAttackInOcean() const;
	bool IsNoDefensiveBonus() const;
	bool IsNukeImmune() const;
	bool IsHiddenNationality() const;
	bool IsAlwaysHostile() const;
	bool IsNoRevealMap() const;
	bool IsRecon() const;
	bool CanMoveAllTerrain() const;
	bool IsCanMoveAfterAttacking() const;
	bool IsAirSweepCapable() const;
	bool IsAllowsEmbarkation() const;
	bool IsRangeAttackIgnoreLOS() const;
	bool IsFreePillageMoves() const;
	bool IsHealOnPillage() const;
	bool IsHealIfDefeatExcludeBarbarians() const;
	bool IsEmbarkedAllWater() const;
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
	bool IsEmbarkedDeepWater() const;
#endif
	bool IsCityAttackSupport() const;
	bool IsCaptureDefeatedEnemy() const;
	bool IsIgnoreGreatGeneralBenefit() const;
	bool IsIgnoreZOC() const;
	bool IsSapper() const;
#if defined(MOD_BALANCE_CORE)
	int GetNearbyCityCombatMod() const;
	int GetNearbyFriendlyCityCombatMod() const;
	int GetNearbyEnemyCityCombatMod() const;
	bool IsNearbyPromotion() const;
	int GetNearbyRange() const;
	bool IsConvertEnemyUnitToBarbarian() const;
	bool IsConvertOnFullHP() const;
	bool IsConvertOnDamage() const;
	int GetDamageThreshold() const;
	int GetConvertDamageOrFullHPUnit() const;
	bool IsConvertUnit() const;
	int GetConvertDomainUnit() const;
	int GetConvertDomain() const;
	int GetStackedGreatGeneralExperience() const;
	int GetPillageBonusStrengthPercent() const;
	int GetReligiousPressureModifier() const;
	int GetAdjacentCityDefenseMod() const;
	int GetNearbyEnemyDamage() const;
	int GetMilitaryProductionModifier() const;
	int GetGeneralGoldenAgeExpPercent() const;
	int GetGiveCombatMod() const;
	int GetGiveHPIfEnemyKilled() const;
	int GetGiveExperiencePercent() const;
	int GetGiveOutsideFriendlyLandsModifier() const;
	int GetGiveDomain() const;
	int GetGiveExtraAttacks() const;
	int GetGiveDefenseMod() const;
	bool IsGiveInvisibility() const;
	bool IsGiveOnlyOnStartingTurn() const;
	int GetNearbyHealEnemyTerritory() const;
	int GetNearbyHealNeutralTerritory() const;
	int GetNearbyHealFriendlyTerritory() const;
	int GetAdjacentEnemySapMovement() const;
#endif
	bool IsCanHeavyCharge() const;
	bool HasPostCombatPromotions() const;
	bool ArePostCombatPromotionsExclusive() const;
	bool IsConditionalPromotion() const;

	const char* GetSound() const;
	void SetSound(const char* szVal);

	// Arrays
	int GetTerrainAttackPercent(int i) const;
	int GetTerrainDefensePercent(int i) const;
	int GetTerrainModifierAttack(int i) const;
	int GetTerrainModifierDefense(int i) const;
	int GetFeatureAttackPercent(int i) const;
	int GetFeatureDefensePercent(int i) const;
#if defined(MOD_BALANCE_CORE)
	int GetYieldFromAncientRuins(int i) const;
	int GetYieldFromTRPlunder(int i) const;
	int GetYieldFromScouting(int i) const;
	int GetYieldModifier(int i) const;
	int GetYieldChange(int i) const;
#endif
	int GetYieldFromKills(int i) const;
	int GetYieldFromBarbarianKills(int i) const;
	int GetYieldFromCombatExperienceTimes100(int i) const;
	int GetGarrisonYield(int i) const;
	int GetFortificationYield(int i) const;
	int GetUnitCombatModifierPercent(int i) const;
	int GetUnitCombatModifierPercentAttack(int i) const;
	int GetUnitCombatModifierPercentDefense(int i) const;
	int GetUnitClassModifierPercent(int i) const;
	int GetUnitClassAttackModifier(int i) const;
	int GetUnitClassDefenseModifier(int i) const;
	int GetDomainModifierPercent(int i) const;
	int GetDomainAttackPercent(int i) const;
	int GetDomainDefensePercent(int i) const;
	int GetFeaturePassableTech(int i) const;

#if defined(MOD_BALANCE_CORE)
	int GetCombatModPerAdjacentUnitCombatModifierPercent(int i) const;
	int GetCombatModPerAdjacentUnitCombatAttackModifier(int i) const;
	int GetCombatModPerAdjacentUnitCombatDefenseModifier(int i) const;
	std::pair<int, bool> GetInstantYields(int i) const;
#endif

	bool GetIgnoreTerrainCostIn(int i) const;
	bool GetIgnoreTerrainCostFrom(int i) const;
	bool GetIgnoreFeatureCostIn(int i) const;
	bool GetIgnoreFeatureCostFrom(int i) const;
	bool GetTerrainDoubleMove(int i) const;
	bool GetFeatureDoubleMove(int i) const;
#if defined(MOD_PROMOTIONS_HALF_MOVE)
	bool GetTerrainHalfMove(int i) const;
	bool GetTerrainExtraMove(int i) const;
	bool GetFeatureHalfMove(int i) const;
	bool GetFeatureExtraMove(int i) const;
#endif
#if defined(MOD_BALANCE_CORE)
	bool GetTerrainDoubleHeal(int i) const;
	bool GetFeatureDoubleHeal(int i) const;
#endif
	bool GetTerrainImpassable(int i) const;
	int  GetTerrainPassableTech(int i) const;
	bool GetFeatureImpassable(int i) const;
	bool GetUnitCombatClass(int i) const;
	bool GetCivilianUnitType(int i) const;
	std::pair<int, int> GetYieldFromPillage(YieldTypes eYield) const;
	std::set<int> GetBlockedPromotions() const;
	std::vector<PlagueInfo> GetPlagues() const;
#if defined(MOD_PROMOTIONS_UNIT_NAMING)
	bool IsUnitNaming(int i) const;
	void GetUnitName(UnitTypes eUnit, CvString& sUnitName) const;
#endif
	bool IsPostCombatRandomPromotion(int i) const;

protected:
	int m_iLayerAnimationPath;
	int m_iPrereqPromotion;
	int m_iPrereqOrPromotion1;
	int m_iPrereqOrPromotion2;
	int m_iPrereqOrPromotion3;
	int m_iPrereqOrPromotion4;
	int m_iPrereqOrPromotion5;
	int m_iPrereqOrPromotion6;
	int m_iPrereqOrPromotion7;
	int m_iPrereqOrPromotion8;
	int m_iPrereqOrPromotion9;

	int m_iTechPrereq;
	int m_iInvisibleType;
	int m_iSeeInvisibleType;
	int m_iVisibilityChange;
#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
	int m_iReconChange;
#endif
	int m_iMovesChange;
	int m_iMoveDiscountChange;
	int m_iRangeChange;
	int m_iRangedAttackModifier;
	int m_iInterceptionCombatModifier;
	int m_iInterceptionDefenseDamageModifier;
	int m_iAirSweepCombatModifier;
	int m_iInterceptChanceChange;
	int m_iNumInterceptionChange;
#if defined(MOD_BALANCE_CORE)
	int m_iAirInterceptRangeChange; // JJ: This is new
#endif
	int m_iEvasionChange;
	int m_iCargoChange;
	int m_iEnemyHealChange;
	int m_iNeutralHealChange;
	int m_iFriendlyHealChange;
	int m_iSameTileHealChange;
	int m_iAdjacentTileHealChange;
	int m_iEnemyDamageChance;
	int m_iNeutralDamageChance;
	int m_iEnemyDamage;
	int m_iNeutralDamage;
	int m_iCombatPercent;
	int m_iCityAttackPercent;
	int m_iCityDefensePercent;
	int m_iRangedDefenseMod;
	int m_iHillsAttackPercent;
	int m_iHillsDefensePercent;
	int m_iOpenAttackPercent;
	int m_iOpenFromPercent;
	int m_iOpenRangedAttackMod;
	int m_iRoughAttackPercent;
	int m_iRoughFromPercent;
	int m_iRoughRangedAttackMod;
	int m_iAttackFortifiedMod;
	int m_iAttackWoundedMod;
	int m_iAttackFullyHealedMod;
	int m_iAttackAboveHealthMod;
	int m_iAttackBelowHealthMod;
	bool m_bRangedFlankAttack;
	int m_iExtraFlankPower;
	int m_iFlankAttackModifier;
	int m_iNearbyEnemyCombatMod;
	int m_iNearbyEnemyCombatRange;
	int m_iOpenDefensePercent;
	int m_iRoughDefensePercent;
	int m_iExtraAttacks;
	bool m_bGreatGeneral;
	bool m_bGreatAdmiral;
	int m_iAuraRangeChange;
	int m_iAuraEffectChange;
	int m_iNumRepairCharges;
	int m_iMilitaryCapChange;
	int m_iGreatGeneralModifier;
	bool m_bGreatGeneralReceivesMovement;
	int m_iGreatGeneralCombatModifier;
	int m_iFriendlyLandsModifier;
	int m_iFriendlyLandsAttackModifier;
	int m_iOutsideFriendlyLandsModifier;
	int m_iCommandType;
	bool m_bNoSupply;
	int m_iMaxHitPointsChange;
	int m_iMaxHitPointsModifier;
	int m_iVsUnhappyMod;
	int m_iUpgradeDiscount;
	int m_iExperiencePercent;
	int m_iAdjacentMod;
	int m_iNoAdjacentUnitMod;
	int m_iAttackMod;
	int m_iDefenseMod;
	int m_iBorderMod;
	int m_iMarriageMod;
	int m_iMarriageModCap;
	int m_iGetGroundAttackDamage;
	int m_iDropRange;
	int m_iExtraNavalMoves;
	int m_iHPHealedIfDefeatEnemy;
	int m_iGoldenAgeValueFromKills;
	int m_iExtraWithdrawal;
	int m_iCombatChange;
	int m_iTileDamageIfNotMoved;
	int m_iFortifiedModifier;
	int m_iMinEffectiveHealth;
	bool m_bRequiresLeadership;
	bool m_bCannotHeal;
	bool m_bPillageFortificationsOnKill;
#if defined(MOD_BALANCE_CORE_JFD)
	int m_iPlagueID;
	int m_iPlaguePriority;
#endif
	int m_iEmbarkExtraVisibility;
	int m_iEmbarkDefenseModifier;
	int m_iCapitalDefenseModifier;
	int m_iCapitalDefenseFalloff;
	int m_iCityAttackPlunderModifier;
	int m_iReligiousStrengthLossRivalTerritory;
	int m_iTradeMissionInfluenceModifier;
	int m_iTradeMissionGoldModifier;

#if defined(MOD_BALANCE_CORE)
	int m_iCaptureDefeatedEnemyChance;
	int m_iBarbarianCombatBonus;
	int m_iGoodyHutYieldBonus;
	int m_iDiploMissionInfluence;
	bool m_bGainsXPFromScouting;
	int m_iXPFromPillaging;
	int m_iExtraXPOnKill;
	bool m_bGainsXPFromSpotting;
	bool m_bCannotBeCaptured;
	int m_iNegatesPromotion;
	int m_iForcedDamageValue;
	int m_iChangeDamageValue;
	int m_iPromotionDuration;
	int m_iMoraleBreakChance;
	int m_iDamageAoEFortified;
	int m_iWorkRateMod;
	bool m_bIsLostOnMove;
	bool m_bCityStateOnly;
	bool m_bBarbarianOnly;
	bool m_bStrongerDamaged;
	bool m_bFightWellDamaged;
#endif
	bool m_bCannotBeChosen;
	bool m_bLostWithUpgrade;
	bool m_bNotWithUpgrade;
	bool m_bInstaHeal;
	bool m_bLeader;
	bool m_bBlitz;
	bool m_bAmphib;
	bool m_bRiver;
	bool m_bEnemyRoute;
	bool m_bRivalTerritory;
	bool m_bMustSetUpToRangedAttack;
	bool m_bRangedSupportFire;
	bool m_bAlwaysHeal;
	bool m_bHealOutsideFriendly;
	bool m_bHillsDoubleMove;
	bool m_bRiverDoubleMove;
	bool m_bIgnoreTerrainCost;
	bool m_bIgnoreTerrainDamage;
	bool m_bIgnoreFeatureDamage;
	bool m_bExtraTerrainDamage;
	bool m_bExtraFeatureDamage;
#if defined(MOD_PROMOTIONS_IMPROVEMENT_BONUS)
	int m_iNearbyImprovementCombatBonus;
	int m_iNearbyImprovementBonusRange;
	ImprovementTypes m_eCombatBonusImprovement;
#endif
#if defined(MOD_BALANCE_CORE)	
	int m_iNearbyUnitClassBonus;
	int m_iNearbyUnitClassBonusRange;
	UnitClassTypes m_iCombatBonusFromNearbyUnitClass;
	int m_iWonderProductionModifier;
	int m_iAOEDamageOnKill;
	int m_iAOEDamageOnPillage;
	int m_iAOEHealOnPillage;
	int m_iCombatModPerCSAlliance;
	int m_iAttackModPerSamePromotionAttack;
	int m_iAttackModPerSamePromotionAttackCap;
	int m_iAoEDamageOnMove;
	int m_iPartialHealOnPillage;
	int m_iSplashDamage;
	int m_iMinRange;
	int m_iMaxRange;
	int m_iMultiAttackBonus;
	int m_iLandAirDefenseValue;
	int m_iDamageReductionCityAssault;
	bool m_bMountainsDoubleMove;
	bool m_bEmbarkFlatCost;
	bool m_bDisembarkFlatCost;
	bool m_bMountedOnly;
#endif
#if defined(MOD_PROMOTIONS_CROSS_MOUNTAINS)
	bool m_bCanCrossMountains;
#endif
#if defined(MOD_PROMOTIONS_CROSS_OCEANS)
	bool m_bCanCrossOceans;
#endif
#if defined(MOD_PROMOTIONS_CROSS_ICE)
	bool m_bCanCrossIce;
#endif
#if defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
	bool m_bGGFromBarbarians;
#endif
	bool m_bRoughTerrainEndsTurn;
	bool m_bCapturedUnitsConscripted;
	bool m_bHoveringUnit;
	bool m_bFlatMovementCost;
	bool m_bCanMoveImpassable;
	bool m_bNoCapture;
	bool m_bOnlyDefensive;
	bool m_bNoAttackInOcean;
	bool m_bNoDefensiveBonus;
	bool m_bNukeImmune;
	bool m_bHiddenNationality;
	bool m_bAlwaysHostile;
	bool m_bNoRevealMap;
	bool m_bRecon;
	bool m_bCanMoveAllTerrain;
	bool m_bCanMoveAfterAttacking;
	bool m_bAirSweepCapable;
	bool m_bAllowsEmbarkation;
	bool m_bRangeAttackIgnoreLOS;
	bool m_bFreePillageMoves;
	bool m_bHealOnPillage;
	bool m_bHealIfDefeatExcludesBarbarians;
	bool m_bEmbarkedAllWater;
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
	bool m_bEmbarkedDeepWater;
#endif
	bool m_bCityAttackOnly;
	bool m_bCaptureDefeatedEnemy;
	bool m_bIgnoreGreatGeneralBenefit;
	bool m_bIgnoreZOC;
	bool m_bHasPostCombatPromotions;
	bool m_bPostCombatPromotionsExclusive;
	bool m_bSapper;
#if defined(MOD_BALANCE_CORE)
	int m_iNearbyCityCombatMod;
	int m_iNearbyFriendlyCityCombatMod;
	int m_iNearbyEnemyCityCombatMod;
	bool m_bIsNearbyPromotion;
	int m_iNearbyRange;
	UnitTypes m_eRequiredUnit;
	int m_iConvertDomainUnit;
	int m_iConvertDomain;
	bool m_bIsConvertUnit;
	bool m_bIsConvertEnemyUnitToBarbarian;
	bool m_bIsConvertOnFullHP;
	bool m_bIsConvertOnDamage;
	int m_iDamageThreshold;
	int m_iConvertDamageOrFullHPUnit;
	int m_iStackedGreatGeneralExperience;
	int m_iPillageBonusStrength;
	int m_iReligiousPressureModifier;
	int m_iAdjacentCityDefenseMod;
	int m_iNearbyEnemyDamage;
	int m_iMilitaryProductionModifier;
	int m_iGeneralGoldenAgeExpPercent;
	int m_iGiveCombatMod;
	int m_iGiveHPHealedIfEnemyKilled;
	int m_iGiveExperiencePercent;
	int m_iGiveOutsideFriendlyLandsModifier;
	int m_iGiveDomain;
	int m_iGiveExtraAttacks;
	int m_iGiveDefenseMod;
	bool m_bGiveInvisibility;
	bool m_bGiveOnlyOnStartingTurn;
	int m_iNearbyHealEnemyTerritory;
	int m_iNearbyHealNeutralTerritory;
	int m_iNearbyHealFriendlyTerritory;
	int m_iAdjacentEnemySapMovement;
#endif
	bool m_bCanHeavyCharge;

	CvString m_strSound;

	// Arrays
	int* m_piTerrainAttackPercent;
	int* m_piTerrainDefensePercent;
	int* m_piFeatureAttackPercent;
	int* m_piFeatureDefensePercent;
	int* m_piTerrainModifierAttack;
	int* m_piTerrainModifierDefense;
#if defined(MOD_BALANCE_CORE)
	int* m_piYieldFromScouting;
	int* m_piYieldModifier;
	int* m_piYieldChange;
	int* m_piYieldFromAncientRuins;
	int* m_piYieldFromTRPlunder;
#endif
	int* m_piYieldFromKills;
	int* m_piYieldFromBarbarianKills;
	int* m_piYieldFromCombatExperienceTimes100;
	int* m_piGarrisonYield;
	int* m_piFortificationYield;
	int* m_piUnitCombatModifierPercent;
	int* m_piUnitCombatModifierPercentAttack;
	int* m_piUnitCombatModifierPercentDefense;
	int* m_piUnitClassModifierPercent;
	int* m_piUnitClassAttackModifier;
	int* m_piUnitClassDefenseModifier;
	int* m_piDomainModifierPercent;
	int* m_piDomainAttackPercent;
	int* m_piDomainDefensePercent;

#if defined(MOD_BALANCE_CORE)
	int* m_piCombatModPerAdjacentUnitCombatModifierPercent;
	int* m_piCombatModPerAdjacentUnitCombatAttackModifier;
	int* m_piCombatModPerAdjacentUnitCombatDefenseModifier;
	std::map<int, std::pair<int, bool>> m_piInstantYields;
#endif

	int* m_piTerrainPassableTech;
	int* m_piFeaturePassableTech;

	bool* m_pbIgnoreTerrainCostIn;
	bool* m_pbIgnoreTerrainCostFrom;
	bool* m_pbIgnoreFeatureCostIn;
	bool* m_pbIgnoreFeatureCostFrom;
	bool* m_pbTerrainDoubleMove;
	bool* m_pbFeatureDoubleMove;
#if defined(MOD_PROMOTIONS_HALF_MOVE)
	bool* m_pbTerrainHalfMove;
	bool* m_pbTerrainExtraMove;
	bool* m_pbFeatureHalfMove;
	bool* m_pbFeatureExtraMove;
#endif
#if defined(MOD_BALANCE_CORE)
	bool* m_pbTerrainDoubleHeal;
	bool* m_pbFeatureDoubleHeal;
#endif
	bool* m_pbTerrainImpassable;
	bool* m_pbFeatureImpassable;
	bool* m_pbUnitCombat;
	bool* m_pbCivilianUnitType;
#if defined(MOD_PROMOTIONS_UNIT_NAMING)
	bool* m_pbUnitName;
#endif
	bool* m_pbPostCombatRandomPromotion;
	std::map<int, std::pair<int, int>> m_yieldFromPillage;
	std::set<int> m_siBlockedPromotions;
	std::vector<PlagueInfo> m_vsPlagues;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvPromotionXMLEntries
//!  \brief		Game-wide information about promotions
//
//! Key Attributes:
//! - Plan is it will be contained in CvGameRules object within CvGame class
//! - Populated from XML\GameInfo\CIV5PromotionInfo.xml
//! - Contains an array of CvPromotionEntry from the above XML file
//! - One instance for the entire game
//! - Accessed heavily by the [what stores info on projects built?] class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPromotionXMLEntries
{
public:
	CvPromotionXMLEntries(void);
	~CvPromotionXMLEntries(void);

	// Accessor functions
	std::vector<CvPromotionEntry*>& GetPromotionEntries();
	int GetNumPromotions();
	_Ret_maybenull_ CvPromotionEntry* GetEntry(int index);

	// Binary cache functions
	void DeleteArray();

private:
	std::vector<CvPromotionEntry*> m_paPromotionEntries;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvUnitPromotions
//!  \brief		Information about the promotions of a single unit
//
//!  Key Attributes:
//!  - Plan is it will be contained in CvPlayerState object within CvUnit class
//!  - One instance for each unit
//!  - Accessed by any class that needs to check promotions
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUnitPromotions
{
public:
	CvUnitPromotions(void);
	~CvUnitPromotions(void);
	void Init();
	void Uninit();
	void Reset();
	template<typename UnitPromotions, typename Visitor>
	static void Serialize(UnitPromotions& unitPromotions, Visitor& visitor);
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	bool HasPromotion(PromotionTypes eIndex) const;
	void SetPromotion(PromotionTypes eIndex, bool bValue);

	bool IsPromotionActive(PromotionTypes eIndex) const;
	void SetPromotionActive(PromotionTypes eIndex, bool bValue);

	bool HasAllowFeaturePassable() const;
	bool GetAllowFeaturePassable(FeatureTypes eFeatureType, TeamTypes eTeam) const;
	bool HasAllowTerrainPassable() const;
	bool GetAllowTerrainPassable(TerrainTypes eTerrainType, TeamTypes eTeam) const;

	int GetUnitClassAttackMod(UnitClassTypes eUnitClass) const;
	int GetUnitClassDefenseMod(UnitClassTypes eUnitClass) const;

	PromotionTypes ChangePromotionAfterCombat(PromotionTypes eIndex, CvUnit* pThisUnit);

private:
	bool IsInUseByPlayer(PromotionTypes eIndex, PlayerTypes ePlayer); 

	void UpdateCache();
	bool m_bTerrainPassable;
	bool m_bFeaturePassable;
	std::vector<std::vector<TechTypes>> m_terrainPassableCache;
	std::vector<std::vector<TechTypes>> m_featurePassableCache;
	std::vector<int> m_unitClassDefenseMod;
	std::vector<int> m_unitClassAttackMod;

	CvBitfield m_kHasPromotion;
	CvBitfield m_kPromotionActive;
};

FDataStream& operator>>(FDataStream&, CvUnitPromotions&);
FDataStream& operator<<(FDataStream&, const CvUnitPromotions&);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Helper Functions to serialize arrays of variable length (based on number of promotions defined in game)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
namespace PromotionArrayHelpers
{
void ReadV3(FDataStream& kStream, CvBitfield& kPromotions);
void Read(FDataStream& kStream, CvBitfield& kPromotions);
void Write(FDataStream& kStream, const CvBitfield& kPromotions, int iArraySize);
}

#endif //CIV5_PROMOTION_CLASSES_H
