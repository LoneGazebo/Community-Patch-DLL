/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

// unit.h

#ifndef CIV5_UNIT_H
#define CIV5_UNIT_H

#include "CvEnumSerialization.h"
#include "FStlContainerSerialization.h"
#include "FAutoVariable.h"
#include "FAutoVector.h"
#include "FObjectHandle.h"
#include "CvInfos.h"
#include "CvPromotionClasses.h"
#include "CvAStarNode.h"
#include "CvSerialize.h"

#define DEFAULT_UNIT_MAP_LAYER 0
#define TRADE_UNIT_MAP_LAYER 1

#pragma warning( disable: 4251 )		// needs to have dll-interface to be used by clients of class

class CvPlot;
class CvUnitEntry;
class CvUnitReligion;

#if defined(MOD_BALANCE_CORE_MILITARY)
class CvTacticalMove;
#endif

typedef std::vector<int> UnitIdContainer; //use a vector as most of the time this will be empty
typedef std::vector<std::pair<TerrainTypes, int>> TerrainTypeCounter;
typedef std::vector<std::pair<FeatureTypes, int>> FeatureTypeCounter;
typedef std::vector<std::pair<UnitClassTypes, int>> UnitClassCounter;
typedef FFastSmallFixedList< MissionData, 12, true, c_eCiv5GameplayDLL > MissionQueue;

struct CvUnitCaptureDefinition
{
	PlayerTypes eOriginalOwner;		// Who first created the unit
	PlayerTypes eOldPlayer;			// The previous owner of the unit, no necessarily the original owner
	UnitTypes	eOldType;			// Previous type of the unit, the type can change when capturing
	PlayerTypes eCapturingPlayer;
	UnitTypes	eCaptureUnitType;
	int iUnitID;
	int iX;
	int iY;
	bool bEmbarked;
	bool bAsIs;
#if defined(MOD_API_EXTENSIONS)
	int iScenarioData;
#endif
	ReligionTypes eReligion;
	int iReligiousStrength;
	int iSpreadsUsed;

	CvUnitCaptureDefinition()
		: eOriginalOwner(NO_PLAYER)
		, eOldPlayer(NO_PLAYER)
		, eOldType(NO_UNIT)
		, eCapturingPlayer(NO_PLAYER)
		, eCaptureUnitType(NO_UNIT)
		, iUnitID(-1)
		, iX(-1)
		, iY(-1)
		, bEmbarked(false)
		, bAsIs(false)
#if defined(MOD_API_EXTENSIONS)
		, iScenarioData(0)
#endif
		, eReligion(NO_RELIGION)
		, iReligiousStrength(0)
		, iSpreadsUsed(0) { }

	inline bool IsValid() const
	{
		return eCapturingPlayer != NO_PLAYER && eCaptureUnitType != NO_UNIT;
	}
};

//we calculate a unit's strength modifier very often, so we cache the most recent results
struct SStrengthModifierInput
{
	SStrengthModifierInput(const CvUnit* pOtherUnit, const CvPlot* pBattlePlot, bool bIgnoreUnitAdjacencyBoni, const CvPlot* pFromPlot, bool bQuickAndDirty, int iOurDamage);
	const bool operator==(const SStrengthModifierInput& rhs) const;

	int m_iOtherUnitID;
	int m_iBattlePlot;
	int m_iFromPlot;
	bool m_bIgnoreUnitAdjacencyBoni;
	bool m_bQuickAndDirty;
	int m_iOurDamage;
	int m_iTheirDamage;
};

enum AreaEffectType
{
	AE_GREAT_GENERAL,
	AE_SAPPER,
	AE_SIEGETOWER
};

SYNC_OBJECT_BEGIN(CvUnit)
SYNC_OBJECT_VAR(PlayerTypes, m_eOwner, true)
SYNC_OBJECT_VAR(PlayerTypes, m_eOriginalOwner, true)
SYNC_OBJECT_VAR(UnitTypes, m_eUnitType, true)
SYNC_OBJECT_VAR(int, m_iX, true)
SYNC_OBJECT_VAR(int, m_iY, true)
SYNC_OBJECT_VAR(int, m_iID, true)
SYNC_OBJECT_VAR(int, m_iDamage, true)
SYNC_OBJECT_VAR(int, m_iMoves, true)
SYNC_OBJECT_VAR(int, m_iArmyId, true)
SYNC_OBJECT_VAR(int, m_iBaseCombat, true)
SYNC_OBJECT_VAR(int, m_iBaseRangedCombat, true)
SYNC_OBJECT_VAR(int, m_iHotKeyNumber, true)
SYNC_OBJECT_VAR(int, m_iDeployFromOperationTurn, true)
SYNC_OBJECT_VAR(int, m_iLastMoveTurn, true)
SYNC_OBJECT_VAR(int, m_iReconX, true)
SYNC_OBJECT_VAR(int, m_iReconY, true)
SYNC_OBJECT_VAR(int, m_iReconCount, true)
SYNC_OBJECT_VAR(int, m_iGameTurnCreated, true)
SYNC_OBJECT_VAR(bool, m_bImmobile, true)
SYNC_OBJECT_VAR(int, m_iExperienceTimes100, true)
SYNC_OBJECT_VAR(int, m_iLevel, true)
SYNC_OBJECT_VAR(int, m_iCargo, true)
SYNC_OBJECT_VAR(int, m_iCargoCapacity, true)
SYNC_OBJECT_VAR(int, m_iAttackPlotX, true)
SYNC_OBJECT_VAR(int, m_iAttackPlotY, true)
SYNC_OBJECT_VAR(int, m_iCombatTimer, true)
SYNC_OBJECT_VAR(int, m_iCombatFirstStrikes, true)
SYNC_OBJECT_VAR(int, m_iCombatDamage, true)
SYNC_OBJECT_VAR(bool, m_bMovedThisTurn, true)
SYNC_OBJECT_VAR(bool, m_bFortified, true)
SYNC_OBJECT_VAR(int, m_iBlitzCount, true)
SYNC_OBJECT_VAR(int, m_iAmphibCount, true)
SYNC_OBJECT_VAR(int, m_iRiverCrossingNoPenaltyCount, true)
SYNC_OBJECT_VAR(int, m_iEnemyRouteCount, true)
SYNC_OBJECT_VAR(int, m_iRivalTerritoryCount, true)
SYNC_OBJECT_VAR(int, m_iIsSlowInEnemyLandCount, true)
SYNC_OBJECT_VAR(int, m_iRangeAttackIgnoreLOSCount, true)
SYNC_OBJECT_VAR(int, m_iCityAttackOnlyCount, true)
SYNC_OBJECT_VAR(int, m_iCaptureDefeatedEnemyCount, true)
SYNC_OBJECT_VAR(int, m_iOriginCity, true)
SYNC_OBJECT_VAR(int, m_iCannotBeCapturedCount, true)
SYNC_OBJECT_VAR(int, m_iForcedDamage, true)
SYNC_OBJECT_VAR(int, m_iChangeDamage, true)
SYNC_OBJECT_VAR(SYNC_OBJECT_VAR_TYPE(std::map<PromotionTypes, int>), m_PromotionDuration, true)
SYNC_OBJECT_VAR(SYNC_OBJECT_VAR_TYPE(std::map<PromotionTypes, int>), m_TurnPromotionGained, true)
SYNC_OBJECT_VAR(int, m_iRangedSupportFireCount, true)
SYNC_OBJECT_VAR(int, m_iAlwaysHealCount, true)
SYNC_OBJECT_VAR(int, m_iHealOutsideFriendlyCount, true)
SYNC_OBJECT_VAR(int, m_iHillsDoubleMoveCount, true)
SYNC_OBJECT_VAR(int, m_iMountainsDoubleMoveCount, true)
SYNC_OBJECT_VAR(int, m_iEmbarkFlatCostCount, true)
SYNC_OBJECT_VAR(int, m_iDisembarkFlatCostCount, true)
SYNC_OBJECT_VAR(int, m_iAOEDamageOnKill, true)
SYNC_OBJECT_VAR(int, m_iAoEDamageOnMove, true)
SYNC_OBJECT_VAR(int, m_iSplashDamage, true)
SYNC_OBJECT_VAR(int, m_iMultiAttackBonus, true)
SYNC_OBJECT_VAR(int, m_iLandAirDefenseValue, true)
SYNC_OBJECT_VAR(int, m_iImmuneToFirstStrikesCount, true)
SYNC_OBJECT_VAR(int, m_iExtraVisibilityRange, true)
SYNC_OBJECT_VAR(int, m_iExtraReconRange, true)
SYNC_OBJECT_VAR(int, m_iExtraMoves, true)
SYNC_OBJECT_VAR(int, m_iExtraMoveDiscount, true)
SYNC_OBJECT_VAR(int, m_iExtraRange, true)
SYNC_OBJECT_VAR(int, m_iInterceptChance, true)
SYNC_OBJECT_VAR(int, m_iExtraEvasion, true)
SYNC_OBJECT_VAR(int, m_iExtraFirstStrikes, true)
SYNC_OBJECT_VAR(int, m_iExtraChanceFirstStrikes, true)
SYNC_OBJECT_VAR(int, m_iExtraWithdrawal, true)
SYNC_OBJECT_VAR(int, m_iPlagueChance, true)
SYNC_OBJECT_VAR(int, m_iIsPlagued, true)
SYNC_OBJECT_VAR(int, m_iPlagueID, true)
SYNC_OBJECT_VAR(int, m_iPlaguePriority, true)
SYNC_OBJECT_VAR(int, m_iPlagueIDImmunity, true)
SYNC_OBJECT_VAR(int, m_iPlaguePromotion, true)
SYNC_OBJECT_VAR(ContractTypes, m_eUnitContract, true)
SYNC_OBJECT_VAR(int, m_iNegatorPromotion, true)
SYNC_OBJECT_VAR(bool, m_bIsNoMaintenance, true)
SYNC_OBJECT_VAR(int, m_iExtraEnemyHeal, true)
SYNC_OBJECT_VAR(int, m_iExtraNeutralHeal, true)
SYNC_OBJECT_VAR(int, m_iExtraFriendlyHeal, true)
SYNC_OBJECT_VAR(int, m_iEnemyDamageChance, true)
SYNC_OBJECT_VAR(int, m_iNeutralDamageChance, true)
SYNC_OBJECT_VAR(int, m_iEnemyDamage, true)
SYNC_OBJECT_VAR(int, m_iNeutralDamage, true)
SYNC_OBJECT_VAR(int, m_iNearbyEnemyCombatMod, true)
SYNC_OBJECT_VAR(int, m_iNearbyEnemyCombatRange, true)
SYNC_OBJECT_VAR(int, m_iSameTileHeal, true)
SYNC_OBJECT_VAR(int, m_iAdjacentTileHeal, true)
SYNC_OBJECT_VAR(int, m_iAdjacentModifier, true)
SYNC_OBJECT_VAR(int, m_iRangedAttackModifier, true)
SYNC_OBJECT_VAR(int, m_iInterceptionCombatModifier, true)
SYNC_OBJECT_VAR(int, m_iInterceptionDefenseDamageModifier, true)
SYNC_OBJECT_VAR(int, m_iAirSweepCombatModifier, true)
SYNC_OBJECT_VAR(int, m_iAttackModifier, true)
SYNC_OBJECT_VAR(int, m_iDefenseModifier, true)
SYNC_OBJECT_VAR(int, m_iGroundAttackDamage, true)
SYNC_OBJECT_VAR(int, m_iExtraCombatPercent, true)
SYNC_OBJECT_VAR(int, m_iExtraCityAttackPercent, true)
SYNC_OBJECT_VAR(int, m_iExtraCityDefensePercent, true)
SYNC_OBJECT_VAR(int, m_iExtraRangedDefenseModifier, true)
SYNC_OBJECT_VAR(int, m_iExtraHillsAttackPercent, true)
SYNC_OBJECT_VAR(int, m_iExtraHillsDefensePercent, true)
SYNC_OBJECT_VAR(int, m_iExtraOpenAttackPercent, true)
SYNC_OBJECT_VAR(int, m_iExtraOpenRangedAttackMod, true)
SYNC_OBJECT_VAR(int, m_iExtraRoughAttackPercent, true)
SYNC_OBJECT_VAR(int, m_iExtraRoughRangedAttackMod, true)
SYNC_OBJECT_VAR(int, m_iExtraAttackFortifiedMod, true)
SYNC_OBJECT_VAR(int, m_iExtraAttackWoundedMod, true)
SYNC_OBJECT_VAR(int, m_iExtraFullyHealedMod, true)
SYNC_OBJECT_VAR(int, m_iExtraAttackAboveHealthMod, true)
SYNC_OBJECT_VAR(int, m_iExtraAttackBelowHealthMod, true)
SYNC_OBJECT_VAR(int, m_iFlankAttackModifier, true)
SYNC_OBJECT_VAR(int, m_iExtraOpenDefensePercent, true)
SYNC_OBJECT_VAR(int, m_iExtraRoughDefensePercent, true)
SYNC_OBJECT_VAR(int, m_iExtraOpenFromPercent, true)
SYNC_OBJECT_VAR(int, m_iExtraRoughFromPercent, true)
SYNC_OBJECT_VAR(int, m_iPillageChange, true)
SYNC_OBJECT_VAR(int, m_iUpgradeDiscount, true)
SYNC_OBJECT_VAR(int, m_iExperiencePercent, true)
SYNC_OBJECT_VAR(int, m_iDropRange, true)
SYNC_OBJECT_VAR(int, m_iAirSweepCapableCount, true)
SYNC_OBJECT_VAR(int, m_iExtraNavalMoves, true)
SYNC_OBJECT_VAR(int, m_iKamikazePercent, true)
SYNC_OBJECT_VAR(DirectionTypes, m_eFacingDirection, true)
SYNC_OBJECT_VAR(int, m_iIgnoreTerrainCostCount, true)
SYNC_OBJECT_VAR(int, m_iIgnoreTerrainDamageCount, true)
SYNC_OBJECT_VAR(int, m_iIgnoreFeatureDamageCount, true)
SYNC_OBJECT_VAR(int, m_iExtraTerrainDamageCount, true)
SYNC_OBJECT_VAR(int, m_iExtraFeatureDamageCount, true)
SYNC_OBJECT_VAR(int, m_iNearbyImprovementCombatBonus, true)
SYNC_OBJECT_VAR(int, m_iNearbyImprovementBonusRange, true)
SYNC_OBJECT_VAR(ImprovementTypes, m_eCombatBonusImprovement, true)
SYNC_OBJECT_VAR(int, m_iNearbyUnitClassBonus, true)
SYNC_OBJECT_VAR(int, m_iNearbyUnitClassBonusRange, true)
SYNC_OBJECT_VAR(UnitClassTypes, m_iCombatBonusFromNearbyUnitClass, true)
SYNC_OBJECT_VAR(int, m_bNearbyPromotion, true)
SYNC_OBJECT_VAR(int, m_iNearbyUnitPromotionRange, true)
SYNC_OBJECT_VAR(int, m_iNearbyCityCombatMod, true)
SYNC_OBJECT_VAR(int, m_iNearbyFriendlyCityCombatMod, true)
SYNC_OBJECT_VAR(int, m_iNearbyEnemyCityCombatMod, true)
SYNC_OBJECT_VAR(int, m_iPillageBonusStrengthPercent, true)
SYNC_OBJECT_VAR(int, m_iStackedGreatGeneralExperience, true)
SYNC_OBJECT_VAR(int, m_bIsHighSeaRaider, true)
SYNC_OBJECT_VAR(int, m_iWonderProductionModifier, true)
SYNC_OBJECT_VAR(int, m_iUnitProductionModifier, true)
SYNC_OBJECT_VAR(int, m_iNearbyEnemyDamage, true)
SYNC_OBJECT_VAR(int, m_iGGGAXPPercent, true)
SYNC_OBJECT_VAR(int, m_iGiveCombatMod, true)
SYNC_OBJECT_VAR(int, m_iGiveHPIfEnemyKilled, true)
SYNC_OBJECT_VAR(int, m_iGiveExperiencePercent, true)
SYNC_OBJECT_VAR(int, m_iGiveOutsideFriendlyLandsModifier, true)
SYNC_OBJECT_VAR(int, m_eGiveDomain, true)
SYNC_OBJECT_VAR(int, m_iGiveExtraAttacks, true)
SYNC_OBJECT_VAR(int, m_iGiveDefenseMod, true)
SYNC_OBJECT_VAR(int, m_bGiveInvisibility, true)
SYNC_OBJECT_VAR(int, m_bGiveOnlyOnStartingTurn, true)
SYNC_OBJECT_VAR(int, m_bConvertUnit, true)
SYNC_OBJECT_VAR(int, m_eConvertDomain, true)
SYNC_OBJECT_VAR(UnitTypes, m_eConvertDomainUnit, true)
SYNC_OBJECT_VAR(int, m_bConvertEnemyUnitToBarbarian, true)
SYNC_OBJECT_VAR(int, m_bConvertOnFullHP, true)
SYNC_OBJECT_VAR(int, m_bConvertOnDamage, true)
SYNC_OBJECT_VAR(int, m_iDamageThreshold, true)
SYNC_OBJECT_VAR(UnitTypes, m_eConvertDamageOrFullHPUnit, true)
SYNC_OBJECT_VAR(int, m_iNumberOfCultureBombs, true)
SYNC_OBJECT_VAR(int, m_iNearbyHealEnemyTerritory, true)
SYNC_OBJECT_VAR(int, m_iNearbyHealNeutralTerritory, true)
SYNC_OBJECT_VAR(int, m_iNearbyHealFriendlyTerritory, true)
SYNC_OBJECT_VAR(int, m_iCanCrossMountainsCount, true)
SYNC_OBJECT_VAR(int, m_iCanCrossOceansCount, true)
SYNC_OBJECT_VAR(int, m_iCanCrossIceCount, true)
SYNC_OBJECT_VAR(int, m_iNumTilesRevealedThisTurn, true)
SYNC_OBJECT_VAR(int, m_bSpottedEnemy, true)
SYNC_OBJECT_VAR(int, m_iGainsXPFromScouting, true)
SYNC_OBJECT_VAR(int, m_iGainsXPFromPillaging, true)
SYNC_OBJECT_VAR(int, m_iGainsXPFromSpotting, true)
SYNC_OBJECT_VAR(int, m_iCaptureDefeatedEnemyChance, true)
SYNC_OBJECT_VAR(int, m_iBarbCombatBonus, true)
SYNC_OBJECT_VAR(int, m_iAdjacentEnemySapMovement, true)
SYNC_OBJECT_VAR(int, m_iGGFromBarbariansCount, true)
SYNC_OBJECT_VAR(int, m_iAuraRangeChange, true)
SYNC_OBJECT_VAR(int, m_iAuraEffectChange, true)
SYNC_OBJECT_VAR(int, m_iNumRepairCharges, true)
SYNC_OBJECT_VAR(int, m_iMilitaryCapChange, true)
SYNC_OBJECT_VAR(int, m_iRoughTerrainEndsTurnCount, true)
SYNC_OBJECT_VAR(int, m_iEmbarkAbilityCount, true)
SYNC_OBJECT_VAR(int, m_iHoveringUnitCount, true)
SYNC_OBJECT_VAR(int, m_iFlatMovementCostCount, true)
SYNC_OBJECT_VAR(int, m_iCanMoveImpassableCount, true)
SYNC_OBJECT_VAR(int, m_iOnlyDefensiveCount, true)
SYNC_OBJECT_VAR(int, m_iNoDefensiveBonusCount, true)
SYNC_OBJECT_VAR(int, m_iNoCaptureCount, true)
SYNC_OBJECT_VAR(int, m_iNukeImmuneCount, true)
SYNC_OBJECT_VAR(int, m_iHiddenNationalityCount, true)
SYNC_OBJECT_VAR(int, m_iAlwaysHostileCount, true)
SYNC_OBJECT_VAR(int, m_iNoRevealMapCount, true)
SYNC_OBJECT_VAR(int, m_iCanMoveAllTerrainCount, true)
SYNC_OBJECT_VAR(int, m_iCanMoveAfterAttackingCount, true)
SYNC_OBJECT_VAR(int, m_iFreePillageMoveCount, true)
SYNC_OBJECT_VAR(int, m_iHealOnPillageCount, true)
SYNC_OBJECT_VAR(int, m_iHPHealedIfDefeatEnemy, true)
SYNC_OBJECT_VAR(int, m_iGoldenAgeValueFromKills, true)
SYNC_OBJECT_VAR(int, m_iTacticalAIPlotX, true)
SYNC_OBJECT_VAR(int, m_iTacticalAIPlotY, true)
SYNC_OBJECT_VAR(int, m_iGarrisonCityID, true)
SYNC_OBJECT_VAR(int, m_iNumAttacks, true)
SYNC_OBJECT_VAR(int, m_iAttacksMade, true)
SYNC_OBJECT_VAR(int, m_iGreatGeneralCount, true)
SYNC_OBJECT_VAR(int, m_iGreatAdmiralCount, true)
SYNC_OBJECT_VAR(int, m_iGreatGeneralModifier, true)
SYNC_OBJECT_VAR(int, m_iGreatGeneralReceivesMovementCount, true)
SYNC_OBJECT_VAR(int, m_iGreatGeneralCombatModifier, true)
SYNC_OBJECT_VAR(int, m_iIgnoreGreatGeneralBenefit, true)
SYNC_OBJECT_VAR(int, m_iIgnoreZOC, true)
SYNC_OBJECT_VAR(int, m_iNoSupply, true)
SYNC_OBJECT_VAR(int, m_iMaxHitPointsChange, true)
SYNC_OBJECT_VAR(int, m_iMaxHitPointsModifier, true)
SYNC_OBJECT_VAR(int, m_iFriendlyLandsModifier, true)
SYNC_OBJECT_VAR(int, m_iFriendlyLandsAttackModifier, true)
SYNC_OBJECT_VAR(int, m_iOutsideFriendlyLandsModifier, true)
SYNC_OBJECT_VAR(int, m_iHealIfDefeatExcludeBarbariansCount, true)
SYNC_OBJECT_VAR(int, m_iNumInterceptions, true)
SYNC_OBJECT_VAR(int, m_iExtraAirInterceptRange, true)
SYNC_OBJECT_VAR(int, m_iMadeInterceptionCount, true)
SYNC_OBJECT_VAR(int, m_iEverSelectedCount, true)
SYNC_OBJECT_VAR(int, m_iSapperCount, true)
SYNC_OBJECT_VAR(int, m_iCanHeavyCharge, true)
SYNC_OBJECT_VAR(int, m_iStrongerDamaged, true)
SYNC_OBJECT_VAR(int, m_iFightWellDamaged, true)
SYNC_OBJECT_VAR(int, m_iCanMoraleBreak, true)
SYNC_OBJECT_VAR(int, m_iDamageAoEFortified, true)
SYNC_OBJECT_VAR(int, m_iWorkRateMod, true)
SYNC_OBJECT_VAR(int, m_iDamageReductionCityAssault, true)
SYNC_OBJECT_VAR(int, m_iGoodyHutYieldBonus, true)
SYNC_OBJECT_VAR(int, m_iReligiousPressureModifier, true)
SYNC_OBJECT_VAR(int, m_iNumExoticGoods, true)
SYNC_OBJECT_VAR(bool, m_bPromotionReady, true)
SYNC_OBJECT_VAR(bool, m_bDeathDelay, true)
SYNC_OBJECT_VAR(bool, m_bCombatFocus, true)
SYNC_OBJECT_VAR(bool, m_bInfoBarDirty, true)
SYNC_OBJECT_VAR(bool, m_bNotConverting, true)
SYNC_OBJECT_VAR(bool, m_bAirCombat, true)
SYNC_OBJECT_VAR(bool, m_bSetUpForRangedAttack, true)
SYNC_OBJECT_VAR(bool, m_bEmbarked, true)
SYNC_OBJECT_VAR(bool, m_bPromotedFromGoody, true)
SYNC_OBJECT_VAR(bool, m_bAITurnProcessed, true)
SYNC_OBJECT_VAR(int, m_iDamageTakenThisTurn, true)
SYNC_OBJECT_VAR(int, m_iDamageTakenLastTurn, true)
SYNC_OBJECT_VAR(PlayerTypes, m_eCapturingPlayer, true)
SYNC_OBJECT_VAR(bool, m_bCapturedAsIs, true)
SYNC_OBJECT_VAR(UnitTypes, m_eLeaderUnitType, true)
SYNC_OBJECT_VAR(InvisibleTypes, m_eInvisibleType, true)
SYNC_OBJECT_VAR(InvisibleTypes, m_eSeeInvisibleType, true)
SYNC_OBJECT_VAR(GreatPeopleDirectiveTypes, m_eGreatPeopleDirectiveType, true)
SYNC_OBJECT_VAR(CvString, m_strScriptData, true)
SYNC_OBJECT_VAR(int, m_iScenarioData, true)
SYNC_OBJECT_VAR(int, m_iBuilderStrength, true)
SYNC_OBJECT_VAR(TerrainTypeCounter, m_terrainDoubleMoveCount, true)
SYNC_OBJECT_VAR(FeatureTypeCounter, m_featureDoubleMoveCount, true)
SYNC_OBJECT_VAR(TerrainTypeCounter, m_terrainHalfMoveCount, true)
SYNC_OBJECT_VAR(FeatureTypeCounter, m_featureHalfMoveCount, true)
SYNC_OBJECT_VAR(TerrainTypeCounter, m_terrainExtraMoveCount, true)
SYNC_OBJECT_VAR(FeatureTypeCounter, m_featureExtraMoveCount, true)
SYNC_OBJECT_VAR(TerrainTypeCounter, m_terrainDoubleHeal, true)
SYNC_OBJECT_VAR(FeatureTypeCounter, m_featureDoubleHeal, true)
SYNC_OBJECT_VAR(TerrainTypeCounter, m_terrainImpassableCount, true)
SYNC_OBJECT_VAR(FeatureTypeCounter, m_featureImpassableCount, true)
SYNC_OBJECT_VAR(TerrainTypeCounter, m_extraTerrainAttackPercent, true)
SYNC_OBJECT_VAR(TerrainTypeCounter, m_extraTerrainDefensePercent, true)
SYNC_OBJECT_VAR(FeatureTypeCounter, m_extraFeatureAttackPercent, true)
SYNC_OBJECT_VAR(FeatureTypeCounter, m_extraFeatureDefensePercent, true)
SYNC_OBJECT_VAR(UnitClassCounter, m_extraUnitClassAttackMod, true)
SYNC_OBJECT_VAR(UnitClassCounter, m_extraUnitClassDefenseMod, true)
SYNC_OBJECT_VAR(std::vector<int>, m_aiNumTimesAttackedThisTurn, true)
SYNC_OBJECT_VAR(std::vector<int>, m_yieldFromScouting, true)
SYNC_OBJECT_VAR(std::vector<int>, m_yieldFromKills, true)
SYNC_OBJECT_VAR(std::vector<int>, m_yieldFromBarbarianKills, true)
SYNC_OBJECT_VAR(std::vector<int>, m_extraUnitCombatModifier, true)
SYNC_OBJECT_VAR(std::vector<int>, m_unitClassModifier, true)
SYNC_OBJECT_VAR(std::vector<int>, m_iCombatModPerAdjacentUnitCombatModifier, true)
SYNC_OBJECT_VAR(std::vector<int>, m_iCombatModPerAdjacentUnitCombatAttackMod, true)
SYNC_OBJECT_VAR(std::vector<int>, m_iCombatModPerAdjacentUnitCombatDefenseMod, true)
SYNC_OBJECT_VAR(int, m_iMissionTimer, true)
SYNC_OBJECT_VAR(int, m_iMissionAIX, true)
SYNC_OBJECT_VAR(int, m_iMissionAIY, true)
SYNC_OBJECT_VAR(MissionAITypes, m_eMissionAIType, true)
SYNC_OBJECT_VAR(ActivityTypes, m_eActivityType, true)
SYNC_OBJECT_VAR(AutomateTypes, m_eAutomateType, true)
SYNC_OBJECT_VAR(UnitAITypes, m_eUnitAIType, true)
SYNC_OBJECT_VAR(int, m_eCombatType, true)
SYNC_OBJECT_VAR(int, m_iEmbarkedAllWaterCount, true)
SYNC_OBJECT_VAR(int, m_iEmbarkedDeepWaterCount, true)
SYNC_OBJECT_VAR(int, m_iEmbarkExtraVisibility, true)
SYNC_OBJECT_VAR(int, m_iEmbarkDefensiveModifier, true)
SYNC_OBJECT_VAR(int, m_iCapitalDefenseModifier, true)
SYNC_OBJECT_VAR(int, m_iCapitalDefenseFalloff, true)
SYNC_OBJECT_VAR(int, m_iCityAttackPlunderModifier, true)
SYNC_OBJECT_VAR(int, m_iReligiousStrengthLossRivalTerritory, true)
SYNC_OBJECT_VAR(int, m_iTradeMissionInfluenceModifier, true)
SYNC_OBJECT_VAR(int, m_iTradeMissionGoldModifier, true)
SYNC_OBJECT_VAR(int, m_iDiploMissionInfluence, true)
SYNC_OBJECT_VAR(int, m_iMapLayer, true)
SYNC_OBJECT_VAR(int, m_iNumGoodyHutsPopped, true)
SYNC_OBJECT_VAR(int, m_iTourismBlastStrength, true)
SYNC_OBJECT_VAR(int, m_iTourismBlastLength, true)
SYNC_OBJECT_VAR(int, m_iHurryStrength, true)
SYNC_OBJECT_VAR(int, m_iScienceBlastStrength, true)
SYNC_OBJECT_VAR(int, m_iCultureBlastStrength, true)
SYNC_OBJECT_VAR(int, m_iGAPBlastStrength, true)
SYNC_OBJECT_VAR(std::vector<bool>, m_abPromotionEverObtained, true)
SYNC_OBJECT_VAR(AITacticalMove, m_eTacticalMove, true)
SYNC_OBJECT_VAR(int, m_iTacticalMoveSetTurn, true)
SYNC_OBJECT_VAR(AIHomelandMove, m_eHomelandMove, true)
SYNC_OBJECT_VAR(int, m_iHomelandMoveSetTurn, true)
SYNC_OBJECT_END()

class CvUnit
{
	friend class CvUnitMission;
	friend class CvUnitCombat;

public:

	CvUnit();
	~CvUnit();

	enum
	{
		//these values can be called via the dll external interface, don't modify them
	    MOVEFLAG_ATTACK							= 0x0001, // flag for CvUnit missions to allow attacks (melee combat or ranged capturing civilian). pathfinder handles this automatically
	    MOVEFLAG_UNUSED1						= 0x0002, // 
	    MOVEFLAG_DESTINATION					= 0x0004, // we want to end the turn in the given plot. only relevant for canMoveInto(), pathfinder handles it automatically
	    MOVEFLAG_UNUSED2						= 0x0008, // 
	    MOVEFLAG_IGNORE_STACKING				= 0x0010, // stacking rules (with owned units) don't apply (on turn end plots)
	    MOVEFLAG_UNUSED3						= 0x0020, //
	    MOVEFLAG_UNUSED4						= 0x0040, // 
	    MOVEFLAG_UNUSED5						= 0x0080, // 
		//these values are used internally only
		MOVEFLAG_IGNORE_DANGER					= 0x0100, //do not apply a penalty for dangerous plots
		MOVEFLAG_NO_EMBARK						= 0x0200, //do not ever embark (but move along if already embarked)
		MOVEFLAG_NO_ENEMY_TERRITORY				= 0x0400, //don't enter enemy territory, even if we could
		MOVEFLAG_MAXIMIZE_EXPLORE				= 0x0800, //try to reveal as many plots as possible
		MOVEFLAG_NO_DEFENSIVE_SUPPORT			= 0x1000, //without this set in a melee attack, the defender can receive support from adjacent ranged units (unless disabled globally)
		MOVEFLAG_NO_OCEAN						= 0x2000, //don't use deep water even if we could
		MOVEFLAG_DONT_STACK_WITH_NEUTRAL		= 0x4000, //for civilian with escort
		MOVEFLAG_APPROX_TARGET_RING1			= 0x8000, //don't need to reach the target exactly, a ring1 tile is good enough
		MOVEFLAG_APPROX_TARGET_RING2			= 0x10000, //don't need to reach the target exactly, a ring2 tile is good enough
		MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN	= 0x20000, //no embarkation on approximate target tile
		MOVEFLAG_IGNORE_ZOC						= 0x40000, //ignore zones of control
		MOVEFLAG_IGNORE_RIGHT_OF_PASSAGE		= 0x80000, //pretend we can enter everybody's territory
		MOVEFLAG_SELECTIVE_ZOC					= 0x100000, //ignore ZOC from enemy units on given plots
		MOVEFLAG_PRETEND_ALL_REVEALED			= 0x200000, //pretend all plots are revealed, ie territory is known. leaks information, only for AI to recognize dead ends
		MOVEFLAG_AI_ABORT_IN_DANGER				= 0x400000, //abort movement if about to end turn on a dangerous plot (should always check if move was executed afterwards)
		MOVEFLAG_NO_STOPNODES					= 0x800000, //if we already know we can reach the target plot, don't bother with stop nodes
		MOVEFLAG_ABORT_IF_NEW_ENEMY_REVEALED	= 0x1000000, //abort if additional enemies become visible, irrespective of danger level
		MOVEFLAG_IGNORE_ENEMIES					= 0x2000000, //similar to IGNORE_STACKING but pretend we can pass through enemies
		MOVEFLAG_TURN_END_IS_NEXT_TURN			= 0x4000000, //consider when a unit may take action again, ie if the target plot has zero moves left, add one to the turn count
		MOVEFLAG_APPROX_TARGET_SAME_OWNER		= 0x8000000, //same owner of approximate target tile
		MOVEFLAG_PRETEND_CANALS					= 0x10000000, //pretend ships can move one tile inland to see if a canal would make sense

		//some flags are relevant during pathfinding, some only during execution
		PATHFINDER_FLAG_MASK					= ~(MOVEFLAG_ABORT_IF_NEW_ENEMY_REVEALED|MOVEFLAG_TURN_END_IS_NEXT_TURN),
	};

	enum MoveResult
	{
		//values >= 0 are valid
		MOVE_RESULT_CANCEL		= 0xFFFFFFFE,		//cannot continue mission
		MOVE_RESULT_NEXT_TURN	= 0xFFFFFFFD,		//continue next turn
		MOVE_RESULT_ATTACK		= 0xFFFFFFFC,		//move resulted in an attack
		MOVE_RESULT_NO_TARGET	= 0xFFFFFFFB,		//attack not required
	};

#if defined(MOD_BALANCE_CORE)
	void init(int iID, UnitTypes eUnit, UnitAITypes eUnitAI, PlayerTypes eOwner, int iX, int iY, UnitCreationReason eReason, bool bNoMove, bool bSetupGraphical = true, int iMapLayer = DEFAULT_UNIT_MAP_LAYER, int iNumGoodyHutsPopped = 0, ContractTypes eContract = NO_CONTRACT, bool bHistoric = true, CvUnit* pPassUnit = NULL);
	void initWithNameOffset(int iID, UnitTypes eUnit, int iNameOffset, UnitAITypes eUnitAI, PlayerTypes eOwner, int iX, int iY, UnitCreationReason eReason, bool bNoMove, bool bSetupGraphical = true, int iMapLayer = DEFAULT_UNIT_MAP_LAYER, int iNumGoodyHutsPopped = 0, ContractTypes eContract = NO_CONTRACT, bool bHistoric = true, bool bSkipNaming = false, CvUnit* pPassUnit = NULL);
#else
	void init(int iID, UnitTypes eUnit, UnitAITypes eUnitAI, PlayerTypes eOwner, int iX, int iY, UnitCreationReason eReason, bool bNoMove, bool bSetupGraphical = true, int iMapLayer = DEFAULT_UNIT_MAP_LAYER, int iNumGoodyHutsPopped = 0, ContractTypes eContract = NO_CONTRACT, bool bHistoric = true);
	void initWithNameOffset(int iID, UnitTypes eUnit, int iNameOffset, UnitAITypes eUnitAI, PlayerTypes eOwner, int iX, int iY, UnitCreationReason eReason, bool bNoMove, bool bSetupGraphical = true, int iMapLayer = DEFAULT_UNIT_MAP_LAYER, int iNumGoodyHutsPopped = 0, ContractTypes eContract = NO_CONTRACT, bool bHistoric = true, bool bSkipNaming = false);
#endif

	void uninit();

	void reset(int iID = 0, UnitTypes eUnit = NO_UNIT, PlayerTypes eOwner = NO_PLAYER, bool bConstructorCall = false);
	void setupGraphical();

	void initPromotions();
	void uninitInfos();  // used to uninit arrays that may be reset due to mod changes

#if defined(MOD_BALANCE_CORE)
	void convert(CvUnit* pUnit, bool bIsUpgrade);
#else
	void convert(CvUnit* pUnit, bool bIsUpgrade);
#endif
	void kill(bool bDelay, PlayerTypes ePlayer = NO_PLAYER);

	void doTurn();
	bool isActionRecommended(int iAction);

#if defined(MOD_BALANCE_CORE)
	void DoLocationPromotions(bool bSpawn, CvPlot* pOldPlot = NULL, CvPlot* pNewPlot = NULL);
#endif

	bool isBetterDefenderThan(const CvUnit* pDefender, const CvUnit* pAttacker) const;

	bool canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible = false, bool bTestBusy = true) const;
	void doCommand(CommandTypes eCommand, int iData1, int iData2);

	bool IsDoingPartialMove() const;

	ActivityTypes GetActivityType() const;
	void SetActivityType(ActivityTypes eNewValue);

	AutomateTypes GetAutomateType() const;
	bool IsAutomated() const;
	void SetAutomateType(AutomateTypes eNewValue);
	bool CanAutomate(AutomateTypes eAutomate, bool bTestVisible = false) const;
	void Automate(AutomateTypes eAutomate);

	bool ReadyToSelect() const;
	bool ReadyToMove() const;
	bool ReadyToSwap() const;
	bool ReadyToAuto() const;
	bool IsBusy() const;
	bool SentryAlert(bool bAllowAttacks) const;

	bool CanDoInterfaceMode(InterfaceModeTypes eInterfaceMode, bool bTestVisibility = false);

	RouteTypes GetBestBuildRoute(CvPlot* pPlot, BuildTypes* peBestBuild = NULL) const;
	void PlayActionSound();

	TeamTypes GetDeclareWarMove(const CvPlot& pPlot) const;
	TeamTypes GetDeclareWarRangeStrike(const CvPlot& pPlot) const;

	bool canEnterTerritory(TeamTypes eTeam, bool bEndTurn = true) const;
	bool canEnterTerrain(const CvPlot& pPlot, int iMoveFlags = 0) const;
	bool canMoveInto(const CvPlot& pPlot, int iMoveFlags = 0) const;
	bool canMoveOrAttackInto(const CvPlot& pPlot, int iMoveFlags = 0) const;

	bool IsAngerFreeUnit() const;

	int getCombatDamage(int iStrength, int iOpponentStrength, bool bIncludeRand, bool bAttackerIsCity, bool bDefenderIsCity) const;
	void move(CvPlot& pPlot, bool bShow);
	bool jumpToNearestValidPlot();
	bool jumpToNearestValidPlotWithinRange(int iRange, CvPlot* pStartPlot=NULL);

	bool canScrap(bool bTestVisible = false) const;
	void scrap();
	int GetScrapGold() const;

	bool canGift(bool bTestVisible = false, bool bTestTransport = true) const;
	void gift(bool bTestTransport = true);

	bool CanDistanceGift(PlayerTypes eToPlayer) const;

	// Cargo/transport methods (units inside other units)
	bool canLoadUnit(const CvUnit& pUnit, const CvPlot& pPlot) const;
	void loadUnit(CvUnit& pUnit);
	bool canLoad(const CvPlot& pPlot) const;
	void load();
	bool shouldLoadOnMove(const CvPlot* pPlot) const;
	bool canUnload() const;
	void unload();
	bool canUnloadAll() const;
	void unloadAll();
	const CvUnit* getTransportUnit() const;
	CvUnit* getTransportUnit();

	bool isCargo() const;
	void setTransportUnit(CvUnit* pTransportUnit);
#if defined(MOD_CARGO_SHIPS)
	SpecialUnitTypes specialUnitCargoLoad() const;
#endif
	SpecialUnitTypes specialCargo() const;
	DomainTypes domainCargo() const;
	int cargoSpace() const;
	void changeCargoSpace(int iChange);
	bool isFull() const;
	int cargoSpaceAvailable(SpecialUnitTypes eSpecialCargo = NO_SPECIALUNIT, DomainTypes eDomainCargo = NO_DOMAIN) const;
	bool hasCargo() const;
	bool canCargoAllMove() const;
	int getUnitAICargo(UnitAITypes eUnitAI) const;

	bool canHold(const CvPlot* pPlot) const; // skip turn
	bool canSleep(const CvPlot* pPlot) const;
	bool canFortify(const CvPlot* pPlot) const;
	bool canAirPatrol(const CvPlot* pPlot) const;

	bool IsRangeAttackIgnoreLOS() const;
	int GetRangeAttackIgnoreLOSCount() const;
	void ChangeRangeAttackIgnoreLOSCount(int iChange);

	bool canSetUpForRangedAttack(const CvPlot* pPlot) const; //no longer used
	bool isSetUpForRangedAttack() const; //no longer used
	void setSetUpForRangedAttack(bool bValue); //no longer used

	bool IsCityAttackSupport() const;
	void ChangeCityAttackOnlyCount(int iChange);

	bool IsCaptureDefeatedEnemy() const;
	void ChangeCaptureDefeatedEnemyCount(int iChange);
	int GetCaptureChance(CvUnit *pEnemy);

#if defined(MOD_BALANCE_CORE)
	void ChangeCannotBeCapturedCount(int iChange);
	bool GetCannotBeCaptured();
	int getForcedDamageValue();
	void ChangeForcedDamageValue(int iChange);

	int getChangeDamageValue();
	void ChangeChangeDamageValue(int iChange);

	int getPromotionDuration(PromotionTypes ePromotion);
	void ChangePromotionDuration(PromotionTypes ePromotion, int iChange);

	int getTurnPromotionGained(PromotionTypes ePromotion);
	void SetTurnPromotionGained(PromotionTypes ePromotion, int iValue);

	int getNegatorPromotion();
	void SetNegatorPromotion(int iValue);
#endif

	bool canEmbarkAtPlot(const CvPlot* pPlot) const;
	bool canDisembarkAtPlot(const CvPlot* pPlot) const;
	bool canEmbarkOnto(const CvPlot& pOriginPlot, const CvPlot& pTargetPlot, bool bOverrideEmbarkedCheck = false, int iMoveFlags = 0) const;
	bool canDisembarkOnto(const CvPlot& pOriginPlot, const CvPlot& pTargetPlot, bool bOverrideEmbarkedCheck = false, int iMoveFlags = 0) const;
	bool CanEverEmbark() const;  // can this unit ever change into an embarked unit
	void embark(CvPlot* pPlot);
	void disembark(CvPlot* pPlot);
	inline bool isEmbarked() const
	{
		return m_bEmbarked;
	}

	void setEmbarked(bool bValue);

	bool IsHasEmbarkAbility() const;
	int GetEmbarkAbilityCount() const;
	void ChangeEmbarkAbilityCount(int iChange);

	bool canHeal(const CvPlot* pPlot, bool bCheckMovement = true) const;
	bool canSentry(const CvPlot* pPlot) const;

	int healRate(const CvPlot* pPlot) const;
	int healTurns(const CvPlot* pPlot) const;
	void doHeal();
	void DoAttrition();
	int GetDanger(const CvPlot* pAtPlot=NULL) const;
	int GetDanger(const CvPlot* pAtPlot, const UnitIdContainer& unitsToIgnore, int iExtraDamage) const;

#if defined(MOD_GLOBAL_RELOCATION)
	const CvPlot* getAirliftFromPlot(const CvPlot* pPlot) const;
	const CvPlot* getAirliftToPlot(const CvPlot* pPlot, bool bIncludeCities) const;
#endif
	bool canAirlift(const CvPlot* pPlot) const;
	bool canAirliftAt(const CvPlot* pPlot, int iX, int iY) const;
	bool airlift(int iX, int iY);

	bool isNukeVictim(const CvPlot* pPlot, TeamTypes eTeam) const;
	bool canNuke() const;
	bool canNukeAt(const CvPlot* pPlot, int iX, int iY) const;

	bool canParadrop(const CvPlot* pPlot, bool bOnlyTestVisibility) const;
	bool canParadropAt(const CvPlot* pPlot, int iX, int iY) const;
	bool paradrop(int iX, int iY);

	bool canMakeTradeRoute(const CvPlot* pPlot) const;
	bool canMakeTradeRouteAt(const CvPlot* pPlot, int iX, int iY, TradeConnectionType eConnectionType) const;
	bool makeTradeRoute(int iX, int iY, TradeConnectionType eConnectionType);

	bool canChangeTradeUnitHomeCity(const CvPlot* pPlot) const;
	bool canChangeTradeUnitHomeCityAt(const CvPlot* pPlot, int iX, int iY) const;
	bool changeTradeUnitHomeCity(int iX, int iY);

	bool canChangeAdmiralPort(const CvPlot* pPlot) const;
	bool canChangeAdmiralPortAt(const CvPlot* pPlot, int iX, int iY) const;
	bool changeAdmiralPort(int iX, int iY);

	bool canPlunderTradeRoute(const CvPlot* pPlot, bool bOnlyTestVisibility = false) const;
	bool plunderTradeRoute();

	bool canCreateGreatWork(const CvPlot* pPlot, bool bOnlyTestVisibility = false) const;
	bool createGreatWork();

#if defined(MOD_BALANCE_CORE)
	bool canGetFreeLuxury() const;
	bool createFreeLuxury();
#endif

	int getNumExoticGoods() const;
	void setNumExoticGoods(int iValue);
	void changeNumExoticGoods(int iChange);
	float calculateExoticGoodsDistanceFactor(const CvPlot* pPlot);
	bool canSellExoticGoods(const CvPlot* pPlot, bool bOnlyTestVisibility = false) const;
	int getExoticGoodsGoldAmount();
	int getExoticGoodsXPAmount();
	bool sellExoticGoods();

	int getRebaseRange() const;
	bool canRebase(bool bForced = false) const;
	bool canRebaseAt(int iXDest, int iYDest, bool bForced = false) const;
	bool rebase(int iX, int iY, bool bForced = false);

	bool canPillage(const CvPlot* pPlot) const;
	bool shouldPillage(const CvPlot* pPlot, bool bConservative = false) const;
	bool pillage();

	bool canFoundCity(const CvPlot* pPlot, bool bIgnoreDistanceToExistingCities = false, bool bIgnoreHappiness = false) const;
	bool foundCity();

	bool canJoinCity(const CvPlot* pPlot, SpecialistTypes eSpecialist) const;
	bool joinCity(SpecialistTypes eSpecialist);

	bool canConstruct(const CvPlot* pPlot, BuildingTypes eBuilding) const;
	bool construct(BuildingTypes eBuilding);

	bool CanFoundReligion(const CvPlot* pPlot) const;
	bool DoFoundReligion();

	bool CanEnhanceReligion(const CvPlot* pPlot) const;
	bool DoEnhanceReligion();

	bool CanSpreadReligion(const CvPlot* pPlot) const;
	bool DoSpreadReligion();

	bool CanRemoveHeresy(const CvPlot* pPlot) const;
	bool DoRemoveHeresy();

	int GetNumFollowersAfterSpread() const;
	ReligionTypes GetMajorityReligionAfterSpread() const;
	CvCity *GetSpreadReligionTargetCity() const;
#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
	int GetConversionStrength(const CvCity* pCity) const;
#else
	int GetConversionStrength() const;
#endif
#if defined(MOD_BALANCE_CORE)
	bool greatperson();
#endif
	int GetScaleAmount(int iAmountToScale) const;
	bool canDiscover(const CvPlot* pPlot, bool bTestVisible = false) const;
	int getDiscoverAmount();
	bool discover();

	bool IsCanRushBuilding(CvCity* pCity, bool bTestVisible) const;
	bool DoRushBuilding();

	int getMaxHurryProduction(CvCity* pCity) const;
	int getHurryProduction(const CvPlot* pPlot) const;
	bool canHurry(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool hurry();

	int getTradeGold(const CvPlot* pPlot) const;
	int getTradeInfluence(const CvPlot* pPlot) const;
	bool canTrade(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool trade();

	bool canBuyCityState(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool buyCityState();

	bool canRepairFleet(const CvPlot *pPlot, bool bTestVisible = false) const;
	bool repairFleet();

	bool CanBuildSpaceship(const CvPlot* pPlot, bool bTestVisible) const;
	bool DoBuildSpaceship();

	bool CanCultureBomb(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool isCultureBomb() const;
	bool DoCultureBomb();
	void PerformCultureBomb(int iRadius);

	bool canGoldenAge(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool goldenAge();
	int GetGoldenAgeTurns() const;

	bool canGivePolicies(const CvPlot* pPlot, bool bTestVisible = false) const;
	int getGivePoliciesCulture();
	bool givePolicies();

	bool canBlastTourism(const CvPlot* pPlot, bool bTestVisible = false) const;
	int getBlastTourism();
	bool blastTourism();

	bool canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestVisible = false, bool bTestGold = true) const;
	bool build(BuildTypes eBuild);

#if defined(MOD_CIV6_WORKER)
	int getBuilderStrength() const;
	void setBuilderStrength(const int newPower);
#endif
#if defined(MOD_BALANCE_CORE)
	int getNumberOfCultureBombs() const;
	void setNumberOfCultureBombs(const int iBombs);
#endif
	bool canPromote(PromotionTypes ePromotion, int iLeaderUnitId) const;
	void promote(PromotionTypes ePromotion, int iLeaderUnitId);

	int canLead(const CvPlot* pPlot, int iUnitId) const;
	bool lead(int iUnitId);

	int canGiveExperience(const CvPlot* pPlot) const;
	bool giveExperience();
	int getStackExperienceToGive(int iNumUnits) const;

	bool isReadyForUpgrade() const;
	bool CanUpgradeRightNow(bool bOnlyTestVisible) const;
#if defined(MOD_API_EXTENSIONS)
	bool CanUpgradeTo(UnitTypes eUpgradeUnitType, bool bOnlyTestVisible) const;
#endif
#if defined(MOD_GLOBAL_CS_UPGRADES)
	bool CanUpgradeInTerritory(bool bOnlyTestVisible) const;
#endif
	UnitTypes GetUpgradeUnitType() const;
	int upgradePrice(UnitTypes eUnit) const;
#if defined(MOD_API_EXTENSIONS)
	CvUnit* DoUpgrade(bool bFree = false);
#else
	CvUnit* DoUpgrade();
#endif
#if defined(MOD_API_EXTENSIONS)
	CvUnit* DoUpgradeTo(UnitTypes eUpgradeUnitType, bool bFree = false);
#endif

	HandicapTypes getHandicapType() const;
	const CvCivilizationInfo& getCivilizationInfo() const;
	CivilizationTypes getCivilizationType() const;
	const char* getVisualCivAdjective(TeamTypes eForTeam) const;
	SpecialUnitTypes getSpecialUnitType() const;
	bool IsGreatPerson() const;
	UnitTypes getCaptureUnitType(CivilizationTypes eCivilization) const;
	int getUnitCombatType() const;
	void setUnitCombatType(UnitCombatTypes eCombat);
#if defined(MOD_GLOBAL_PROMOTION_CLASSES)
	UnitCombatTypes getUnitPromotionType() const;
#endif
	DomainTypes getDomainType() const;
	//check if plot type matches the (primary) domain type
	bool isNativeDomain(const CvPlot* pPlot) const;
	//check territory, terrain, stacking, etc
	bool canEndTurnAtPlot(const CvPlot* pPlot) const;

	int flavorValue(FlavorTypes eFlavor) const;

	bool isBarbarian() const;
	bool isHuman() const;

	int visibilityRange() const;
#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
	int reconRange() const;
#endif
	bool canChangeVisibility() const;

	int baseMoves(bool bPretendEmbarked) const;
	int maxMoves() const;
	int movesLeft() const;
	bool canMove() const;
	bool hasMoved() const;

	int GetRange() const;
	int GetNukeDamageLevel() const;

	bool canBuildRoute() const;
	BuildTypes getBuildType() const;
	int workRate(bool bMax, BuildTypes eBuild = NO_BUILD) const;

	bool isNoBadGoodies() const;
	bool isRivalTerritory() const;
	int getRivalTerritoryCount() const;
	void changeRivalTerritoryCount(int iChange);
	bool isFound() const;
	bool IsFoundAbroad() const;
#if defined(MOD_BALANCE_CORE_SETTLER_ADVANCED)
	bool IsFoundMid() const;
	bool IsFoundLate() const;
	bool CanFoundColony() const;
#endif
	bool IsWork() const;
	bool isGoldenAge() const;
	bool isGivesPolicies() const;
	bool isBlastTourism() const;
	bool canCoexistWithEnemyUnit(TeamTypes eTeam) const;

	bool isSlowInEnemyLand() const; //used to be setup for ranged attack
	int getIsSlowInEnemyLandCount() const;
	void changeIsSlowInEnemyLandCount(int iChange);

	bool isRangedSupportFire() const;
	int getRangedSupportFireCount() const;
	void changeRangedSupportFireCount(int iChange);

	bool isFighting() const;
	bool isAttacking() const;
	bool isDefending() const;
	bool isInCombat() const;

	int GetMaxHitPoints() const;
	int GetCurrHitPoints() const;
	bool IsHurt() const;
	bool IsDead() const;

	int GetStrategicResourceCombatPenalty() const;
	int GetUnhappinessCombatPenalty() const;

	void SetBaseCombatStrength(int iCombat);
	int GetBaseCombatStrength() const;
	int GetBestAttackStrength() const; //ranged or melee, whichever is greater
	int GetDamageCombatModifier(bool bForDefenseAgainstRanged = false, int iAssumedDamage = 0) const;

	int GetGenericMeleeStrengthModifier(const CvUnit* pOtherUnit, const CvPlot* pBattlePlot, 
									bool bIgnoreUnitAdjacencyBoni, const CvPlot* pFromPlot = NULL, bool bQuickAndDirty = false) const;
	int GetMaxAttackStrength(const CvPlot* pFromPlot, const CvPlot* pToPlot, const CvUnit* pDefender, 
									bool bIgnoreUnitAdjacencyBoni = false, bool bQuickAndDirty = false) const;
	int GetMaxDefenseStrength(const CvPlot* pInPlot, const CvUnit* pAttacker, const CvPlot* pFromPlot, 
									bool bFromRangedAttack = false, bool bQuickAndDirty = false) const;

	int GetEmbarkedUnitDefense() const;

	int GetBaseRangedCombatStrength() const;
#if defined(MOD_API_EXTENSIONS)
	void SetBaseRangedCombatStrength(int iStrength);
#endif

	int GetMaxRangedCombatStrength(const CvUnit* pOtherUnit, const CvCity* pCity, bool bAttacking, 
									const CvPlot* pMyPlot = NULL, const CvPlot* pOtherPlot = NULL, 
									bool bIgnoreUnitAdjacencyBoni = false, bool bQuickAndDirty = false) const;
	int GetAirCombatDamage(const CvUnit* pDefender, const CvCity* pCity, bool bIncludeRand, int iAssumeExtraDamage = 0, 
									const CvPlot* pTargetPlot = NULL, const CvPlot* pFromPlot = NULL, bool bQuickAndDirty = false) const;
	int GetRangeCombatDamage(const CvUnit* pDefender, const CvCity* pCity, bool bIncludeRand, int iAssumeExtraDamage = 0, 
									const CvPlot* pTargetPlot = NULL, const CvPlot* pFromPlot = NULL, 
									bool bIgnoreUnitAdjacencyBoni = false, bool bQuickAndDirty = false) const;
	int GetRangeCombatSplashDamage(const CvPlot* pTargetPlot) const;

	bool canSiege(TeamTypes eTeam) const;
	bool canAirDefend(const CvPlot* pPlot = NULL) const;

	int GetAirStrikeDefenseDamage(const CvUnit* pAttacker, bool bIncludeRand = true, const CvPlot* pTargetPlot = NULL) const;
	int GetInterceptionDamage(const CvUnit* pInterceptedAttacker, bool bIncludeRand = true, const CvPlot* pTargetPlot = NULL) const;

#if defined(MOD_BALANCE_CORE_MILITARY)
	int GetResistancePower(const CvUnit* pOtherUnit) const;
#endif

	int GetCombatLimit() const;
	int GetRangedCombatLimit() const;

	bool isWaiting() const;
	bool IsEverFortifyable() const;
	int fortifyModifier() const;

	int experienceNeeded() const;
	int attackXPValue() const;
	int defenseXPValue() const;
	int maxXPValue() const;

	int firstStrikes() const;
	int chanceFirstStrikes() const;
	int maxFirstStrikes() const;

	bool immuneToFirstStrikes() const;
	bool ignoreBuildingDefense() const;

	bool ignoreTerrainCost() const;
	int getIgnoreTerrainCostCount() const;
	void changeIgnoreTerrainCostCount(int iValue);

	bool ignoreTerrainDamage() const;
	int getIgnoreTerrainDamageCount() const;
	void changeIgnoreTerrainDamageCount(int iValue);

	bool ignoreFeatureDamage() const;
	int getIgnoreFeatureDamageCount() const;
	void changeIgnoreFeatureDamageCount(int iValue);

	bool extraTerrainDamage() const;
	int getExtraTerrainDamageCount() const;
	void changeExtraTerrainDamageCount(int iValue);

	bool extraFeatureDamage() const;
	int getExtraFeatureDamageCount() const;
	void changeExtraFeatureDamageCount(int iValue);

#if defined(MOD_PROMOTIONS_IMPROVEMENT_BONUS)
	int GetNearbyImprovementCombatBonus() const;
	void SetNearbyImprovementCombatBonus(int iCombatBonus);
	int GetNearbyImprovementBonusRange() const;
	void SetNearbyImprovementBonusRange(int iBonusRange);
	ImprovementTypes GetCombatBonusImprovement() const;
	void SetCombatBonusImprovement(ImprovementTypes eImprovement);
#endif
#if defined(MOD_BALANCE_CORE)
	int getNearbyUnitClassBonus() const;
	void SetNearbyUnitClassBonus(int iCombatBonus);
	int getNearbyUnitClassBonusRange() const;
	void SetNearbyUnitClassBonusRange(int iBonusRange);
	UnitClassTypes getCombatBonusFromNearbyUnitClass() const;
	void SetCombatBonusFromNearbyUnitClass(UnitClassTypes eUnitClass);
	void ChangeNearbyPromotion(int iValue);
	int GetNearbyPromotion() const;
	bool isNearbyPromotion() const;
	int GetNearbyUnitPromotionsRange() const;
	void ChangeNearbyUnitPromotionRange(int iBonusRange);
	void ChangeNearbyCityCombatMod(int iValue);
	int getNearbyCityCombatMod() const;
	void ChangeNearbyFriendlyCityCombatMod(int iValue);
	int getNearbyFriendlyCityCombatMod() const;
	void ChangeNearbyEnemyCityCombatMod(int iValue);
	int getNearbyEnemyCityCombatMod() const;
	int getPillageBonusStrengthPercent() const;
	void ChangePillageBonusStrengthPercent(int iBonus);
	int getStackedGreatGeneralExperience() const;
	void ChangeStackedGreatGeneralExperience(int iExperience);
	void ChangeIsHighSeaRaider(int iValue);
	int GetIsHighSeaRaider() const;
	bool isHighSeaRaider() const;
	int getWonderProductionModifier() const;
	void ChangeWonderProductionModifier(int iValue);
	int getMilitaryProductionModifier() const;
	void ChangeMilitaryProductionModifier(int iValue);
	int getNearbyEnemyDamage() const;
	void ChangeNearbyEnemyDamage(int iValue);
	int GetGGGAXPPercent() const;
	void ChangeGGGAXPPercent(int iValue);
	int getGiveCombatMod() const;
	void ChangeGiveCombatMod(int iValue);
	int getGiveHPIfEnemyKilled() const;
	void ChangeGiveHPIfEnemyKilled(int iValue);
	int getGiveExperiencePercent() const;
	void ChangeGiveExperiencePercent(int iValue);
	int getGiveOutsideFriendlyLandsModifier() const;
	void ChangeGiveOutsideFriendlyLandsModifier(int iValue);
	const DomainTypes getGiveDomain() const;
	void ChangeGiveDomain(DomainTypes eDomain);
	int getGiveExtraAttacks() const;
	void ChangeGiveExtraAttacks(int iValue);
	int getGiveDefenseMod() const;
	void ChangeGiveDefenseMod(int iValue);
	int getNearbyHealEnemyTerritory() const;
	void ChangeNearbyHealEnemyTerritory(int iValue);
	int getNearbyHealNeutralTerritory() const;
	void ChangeNearbyHealNeutralTerritory(int iValue);
	int getNearbyHealFriendlyTerritory() const;
	void ChangeNearbyHealFriendlyTerritory(int iValue);
	void ChangeIsGiveInvisibility(int iValue);
	int GetIsGiveInvisibility() const;
	bool isGiveInvisibility() const;
	bool isGiveOnlyOnStartingTurn() const;
	void SetIsGiveOnlyOnStartingTurn(bool bNewValue);
	void ChangeIsConvertUnit(int iValue);
	int getIsConvertUnit() const;
	bool isConvertUnit() const;
	const DomainTypes getConvertDomain() const;
	void ChangeConvertDomain(DomainTypes eDomain);
	const UnitTypes getConvertDomainUnitType() const;
	void ChangeConvertDomainUnit(UnitTypes eUnit);
	void ChangeIsConvertEnemyUnitToBarbarian(int iValue);
	int getIsConvertEnemyUnitToBarbarian() const;
	bool isConvertEnemyUnitToBarbarian() const;
	void ChangeIsConvertOnFullHP(int iValue);
	int getIsConvertOnFullHP() const;
	bool isConvertOnFullHP() const;
	void ChangeIsConvertOnDamage(int iValue);
	int getIsConvertOnDamage() const;
	bool isConvertOnDamage() const;
	int getDamageThreshold() const;
	void ChangeDamageThreshold(int iValue);
	const UnitTypes getConvertDamageOrFullHPUnit() const;
	void ChangeConvertDamageOrFullHPUnit(UnitTypes eUnit);
	bool canIntercept() const;
	int GetAirInterceptRange() const;
#endif
#if defined(MOD_PROMOTIONS_CROSS_MOUNTAINS)
	bool canCrossMountains() const;
	int getCanCrossMountainsCount() const;
	void changeCanCrossMountainsCount(int iValue);
#endif

#if defined(MOD_PROMOTIONS_CROSS_OCEANS)
	bool canCrossOceans() const;
	int getCanCrossOceansCount() const;
	void changeCanCrossOceansCount(int iValue);
#endif

#if defined(MOD_PROMOTIONS_CROSS_ICE)
	bool canCrossIce() const;
	int getCanCrossIceCount() const;
	void changeCanCrossIceCount(int iValue);
#endif

#if defined(MOD_BALANCE_CORE)
	void ChangeNumTilesRevealedThisTurn(int iValue);
	void SetNumTilesRevealedThisTurn(int iValue);
	int GetNumTilesRevealedThisTurn();

	bool HasSpottedEnemy() const;
	void SetSpottedEnemy(bool bValue);

	bool IsGainsXPFromScouting() const;
	int GetGainsXPFromScouting() const;
	void ChangeGainsXPFromScouting(int iValue);

	bool IsGainsXPFromSpotting() const;
	int GetGainsXPFromSpotting() const;
	void ChangeGainsXPFromSpotting(int iValue);

	bool IsGainsXPFromPillaging() const;
	int GetGainsXPFromPillaging() const;
	void ChangeGainsXPFromPillaging(int iValue);

	bool IsGainsYieldFromScouting() const;

	int GetCaptureDefeatedEnemyChance() const;
	void ChangeCaptureDefeatedEnemyChance(int iValue);
	
	int GetBarbarianCombatBonus() const;
	void ChangeBarbarianCombatBonus(int iValue);

	int GetAdjacentEnemySapMovement() const;
	void ChangeAdjacentEnemySapMovement(int iValue);

#endif

#if defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
	bool isGGFromBarbarians() const;
	int getGGFromBarbariansCount() const;
	void changeGGFromBarbariansCount(int iValue);
#endif

	bool IsRoughTerrainEndsTurn() const;
	int GetRoughTerrainEndsTurnCount() const;
	void ChangeRoughTerrainEndsTurnCount(int iValue);

	bool IsHoveringUnit() const;
	int GetHoveringUnitCount() const;
	void ChangeHoveringUnitCount(int iValue);

	bool flatMovementCost() const;
	int getFlatMovementCostCount() const;
	void changeFlatMovementCostCount(int iValue);

	bool canMoveImpassable() const;
	int getCanMoveImpassableCount() const;
	void changeCanMoveImpassableCount(int iValue);

	bool canMoveAllTerrain() const;
	void changeCanMoveAllTerrainCount(int iValue);
	int getCanMoveAllTerrainCount() const;

	bool canMoveAfterAttacking() const;
	void changeCanMoveAfterAttackingCount(int iValue);
	int getCanMoveAfterAttackingCount() const;

	bool hasFreePillageMove() const;
	void changeFreePillageMoveCount(int iValue);
	int getFreePillageMoveCount() const;

	bool hasHealOnPillage() const;
	void changeHealOnPillageCount(int iValue);
	int getHealOnPillageCount() const;

	bool isHiddenNationality() const;
	void changeHiddenNationalityCount(int iValue);
	int getHiddenNationalityCount() const;

	bool isNoRevealMap() const;
	void changeNoRevealMapCount(int iValue);
	int getNoRevealMapCount() const;

	bool isOnlyDefensive() const;
	int getOnlyDefensiveCount() const;
	void changeOnlyDefensiveCount(int iValue);

	bool noDefensiveBonus() const;
	int getNoDefensiveBonusCount() const;
	void changeNoDefensiveBonusCount(int iValue);

	bool isNoCapture() const;
	int getNoCaptureCount() const;
	void changeNoCaptureCount(int iValue);

	bool isNeverInvisible() const;
	bool isInvisible(TeamTypes eTeam, bool bDebug, bool bCheckCargo = true) const;

	bool isNukeImmune() const;
	void changeNukeImmuneCount(int iValue);
	int getNukeImmuneCount() const;

	int interceptionProbability() const;
	int evasionProbability() const;
	int withdrawalProbability() const;

	int GetNumFriendlyUnitsAdjacent(const CvUnit* pUnitToExclude = NULL) const;
	int GetNumEnemyUnitsAdjacent(const CvUnit* pUnitToExclude = NULL) const;
	bool IsEnemyCityAdjacent(const CvCity* pSpecifyCity = NULL) const;
	int GetNumOwningPlayerUnitsAdjacent(const CvUnit* pUnitToExclude = NULL, const CvUnit* pUnitCompare = NULL, bool bCombatOnly = true) const;
	bool IsFriendlyUnitAdjacent(bool bCombatUnit) const;

	int GetAdjacentModifier() const;
	void ChangeAdjacentModifier(int iValue);
	int GetRangedAttackModifier() const;
	void ChangeRangedAttackModifier(int iValue);
	int GetInterceptionCombatModifier() const;
	void ChangeInterceptionCombatModifier(int iValue);
	int GetInterceptionDefenseDamageModifier() const;
	void ChangeInterceptionDefenseDamageModifier(int iValue);
	int GetAirSweepCombatModifier() const;
	void ChangeAirSweepCombatModifier(int iValue);
	int getAttackModifier() const;
	void changeAttackModifier(int iValue);
	int getDefenseModifier(bool bQuick = false) const;
	void changeDefenseModifier(int iValue);

	int getGroundAttackDamage() const;
	void changeGroundAttackDamage(int iValue);

	int cityAttackModifier() const;
	int cityDefenseModifier() const;
	int rangedDefenseModifier() const;
	int hillsAttackModifier() const;
	int hillsDefenseModifier() const;
	int openAttackModifier() const;
	int openRangedAttackModifier() const;
	int roughAttackModifier() const;
	int roughRangedAttackModifier() const;
	int attackFortifiedModifier() const;
	int attackWoundedModifier() const;
	int attackFullyHealedModifier() const;
	int attackAbove50HealthModifier() const;
	int attackBelow50HealthModifier() const;
	int openDefenseModifier() const;
	int roughDefenseModifier() const;
	int terrainAttackModifier(TerrainTypes eTerrain) const;
	int terrainDefenseModifier(TerrainTypes eTerrain) const;
	int featureAttackModifier(FeatureTypes eFeature) const;
	int featureDefenseModifier(FeatureTypes eFeature) const;
	int unitClassAttackModifier(UnitClassTypes eUnitClass) const;
	int unitClassDefenseModifier(UnitClassTypes eUnitClass) const;
	int unitCombatModifier(UnitCombatTypes eUnitCombat) const;
	int domainModifier(DomainTypes eDomain) const;

#if defined(MOD_BALANCE_CORE)
	//int combatModPerAdjacentUnitCombatAttackMod(UnitCombatTypes eIndex) const;
	int combatModPerAdjacentUnitCombatDefenseMod(UnitCombatTypes eIndex) const;
#endif

	int GetYieldModifier(YieldTypes eYield) const;
	void SetYieldModifier(YieldTypes eYield, int iValue);

	int GetYieldChange(YieldTypes eYield) const;
	void SetYieldChange(YieldTypes eYield, int iValue);

	int GetGarrisonYieldChange(YieldTypes eYield) const;
	void SetGarrisonYieldChange(YieldTypes eYield, int iValue);

	int GetFortificationYieldChange(YieldTypes eYield) const;
	void SetFortificationYieldChange(YieldTypes eYield, int iValue);

	inline int GetID() const
	{
		return m_iID;
	}
	IDInfo GetIDInfo() const;
	void SetID(int iID);

	int getHotKeyNumber();
	void setHotKeyNumber(int iNewValue);

	inline int getX() const
	{
		return m_iX.get();
	}

	inline int getY() const
	{
		return m_iY.get();
	}

	void setXY(int iX, int iY, bool bGroup = false, bool bUpdate = true, bool bShow = false, bool bCheckPlotVisible = false, bool bNoMove = false);
	bool at(int iX, int iY) const;
	bool atPlot(const CvPlot& plot) const;
	inline CvPlot* plot() const;
	int getArea() const;
	bool onMap() const;

#if defined(MOD_BALANCE_CORE)
	void setOriginCity(int ID);
	CvCity* getOriginCity() const;
#endif

	int getLastMoveTurn() const;
	void setLastMoveTurn(int iNewValue);

	bool IsRecentlyDeployedFromOperation() const;
	void SetDeployFromOperationTurn(int iTurn)
	{
		m_iDeployFromOperationTurn = iTurn;
	};
#if defined(MOD_BALANCE_CORE)
	int GetDeployFromOperationTurn()
	{
		return m_iDeployFromOperationTurn;
	};
#endif

	bool IsRecon() const;
	int GetReconCount() const;
	void ChangeReconCount(int iChange);

	CvPlot* getReconPlot() const;
	void setReconPlot(CvPlot* pNewValue);

	int getGameTurnCreated() const;
	void setGameTurnCreated(int iNewValue);

	int getDamage() const;
	int setDamage(int iNewValue, PlayerTypes ePlayer = NO_PLAYER, float fAdditionalTextDelay = 0.0f, const CvString* pAppendText = NULL);
	int changeDamage(int iChange, PlayerTypes ePlayer = NO_PLAYER, float fAdditionalTextDelay = 0.0f, const CvString* pAppendText = NULL);

#if defined(MOD_CORE_PER_TURN_DAMAGE)
	int addDamageReceivedThisTurn(int iDamage, CvUnit* pAttacker=NULL);
	void flipDamageReceivedPerTurn();
	bool isProjectedToDieNextTurn() const;
#endif

	int getMoves() const;
	void changeMoves(int iChange);
	void restoreFullMoves();
	void finishMoves();

	bool IsImmobile() const;
	void SetImmobile(bool bValue);

	bool IsInForeignOwnedTerritory() const;

	int getExperienceTimes100() const;
	void setExperienceTimes100(int iNewValueTimes100, int iMax = -1);
	void changeExperienceTimes100(int iChangeTimes100, int iMax = -1, bool bFromCombat = false, bool bInBorders = false, bool bUpdateGlobal = false);

	int getLevel() const;
	void setLevel(int iNewValue);
	void changeLevel(int iChange);

	int getCargo() const;
	void changeCargo(int iChange);

	CvPlot* getAttackPlot() const;
	void setAttackPlot(const CvPlot* pNewValue, bool bAirCombat);
	bool isAirCombat() const;

	int getCombatTimer() const;
	void setCombatTimer(int iNewValue);
	void changeCombatTimer(int iChange);

	int getCombatFirstStrikes() const;
	void setCombatFirstStrikes(int iNewValue);
	void changeCombatFirstStrikes(int iChange);

	int GetMapLayer() const;
	bool CanGarrison() const;
	bool IsGarrisoned(void) const;
	void SetGarrisonedCity(int iCityID);
	CvCity* GetGarrisonedCity() const;
	void triggerFortifyAnimation(bool bState);
	bool IsFortified() const;
	void SetFortified(bool bValue);
	
	int DoBlockade(CvPlot* pWhere, bool bValue);
	int DoAdjacentPlotDamage(CvPlot* pWhere, int iValue, const char* chTextKey = NULL);

	int getBlitzCount() const;
	bool isBlitz() const;
	void changeBlitzCount(int iChange);

	int getAmphibCount() const;
	bool isAmphibious() const;
	void changeAmphibCount(int iChange);

	int getRiverCrossingNoPenaltyCount() const;
	bool isRiverCrossingNoPenalty() const;
	void changeRiverCrossingNoPenaltyCount(int iChange);

	int getEnemyRouteCount() const;
	bool isEnemyRoute() const;
	void changeEnemyRouteCount(int iChange);

	int getAlwaysHealCount() const;
	bool isAlwaysHeal() const;
	void changeAlwaysHealCount(int iChange);

	int getHealOutsideFriendlyCount() const;
	bool isHealOutsideFriendly() const;
	void changeHealOutsideFriendlyCount(int iChange);

	int getHillsDoubleMoveCount() const;
	bool isHillsDoubleMove() const;
	void changeHillsDoubleMoveCount(int iChange);
#if defined(MOD_BALANCE_CORE)
	int getMountainsDoubleMoveCount() const;
	bool isMountainsDoubleMove() const;
	void changeMountainsDoubleMoveCount(int iChange);

	int getEmbarkFlatCostCount() const;
	bool isEmbarkFlatCost() const;
	void changeEmbarkFlatCostCount(int iChange);

	int getDisembarkFlatCostCount() const;
	bool isDisembarkFlatCost() const;
	void changeDisembarkFlatCostCount(int iChange);

	int getAOEDamageOnKill() const;
	void changeAOEDamageOnKill(int iChange);
	
	int getAoEDamageOnMove() const;
	void changeAoEDamageOnMove(int iChange);

	int getSplashDamage() const;
	void changeSplashDamage(int iChange);

	int getMultiAttackBonus() const;
	void changeMultiAttackBonus(int iChange);

	int getLandAirDefenseValue() const;
	void changeLandAirDefenseValue(int iChange);
#endif
	int getImmuneToFirstStrikesCount() const;
	void changeImmuneToFirstStrikesCount(int iChange);

	int getExtraVisibilityRange() const;
	void changeExtraVisibilityRange(int iChange);

#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
	int getExtraReconRange() const;
	void changeExtraReconRange(int iChange);
#endif

	int getExtraMoves() const;
	void changeExtraMoves(int iChange);

	int getExtraMoveDiscount() const;
	void changeExtraMoveDiscount(int iChange);

	int getExtraNavalMoves() const;
	void changeExtraNavalMoves(int iChange);

	int getHPHealedIfDefeatEnemy() const;
	void changeHPHealedIfDefeatEnemy(int iValue);
	bool IsHealIfDefeatExcludeBarbarians() const;
	int GetHealIfDefeatExcludeBarbariansCount() const;
	void ChangeHealIfDefeatExcludeBarbariansCount(int iValue);

	int GetGoldenAgeValueFromKills() const;
	void ChangeGoldenAgeValueFromKills(int iValue);

	int getExtraRange() const;
	void changeExtraRange(int iChange);

	int getInterceptChance() const;
	void changeInterceptChance(int iChange);

	int getExtraEvasion() const;
	void changeExtraEvasion(int iChange);

	int getExtraFirstStrikes() const;
	void changeExtraFirstStrikes(int iChange);

	int getExtraChanceFirstStrikes() const;
	void changeExtraChanceFirstStrikes(int iChange);

	int getExtraWithdrawal() const;
	void changeExtraWithdrawal(int iChange);

#if defined(MOD_BALANCE_CORE_JFD)
	int getPlagueChance() const;
	void changePlagueChance(int iChange);

	int getPlaguePromotionID() const;
	void setPlagued(int iChange);
	bool isPlagued() const;

	int getPlagueID() const;
	void setPlagueID(int iValue);

	int getPlaguePriority() const;
	void setPlaguePriority(int iValue);

	int getPlagueIDImmunity() const;
	void setPlagueIDImmunity(int iValue);

	int getPlaguePromotion() const;
	void setPlaguePromotion(int iValue);

	bool CanPlague(CvUnit* pOtherUnit) const;

	void setContractUnit(ContractTypes eContract);
	bool isContractUnit() const;
	ContractTypes getContract() const;
#endif

	bool IsNoMaintenance() const;
	void SetNoMaintenance(bool bValue);

	int getExtraEnemyHeal() const;
	void changeExtraEnemyHeal(int iChange);

	int getExtraNeutralHeal() const;
	void changeExtraNeutralHeal(int iChange);

	int getExtraFriendlyHeal() const;
	void changeExtraFriendlyHeal(int iChange);

	int getSameTileHeal() const;
	void changeSameTileHeal(int iChange);

	int getAdjacentTileHeal() const;
	void changeAdjacentTileHeal(int iChange);

	int getEnemyDamageChance() const;
	void changeEnemyDamageChance(int iChange);

	int getNeutralDamageChance() const;
	void changeNeutralDamageChance(int iChange);

	int getEnemyDamage() const;
	void changeEnemyDamage(int iChange);

	int getNeutralDamage() const;
	void changeNeutralDamage(int iChange);

	int getNearbyEnemyCombatMod() const;
	void changeNearbyEnemyCombatMod(int iChange);

	int getNearbyEnemyCombatRange() const;
	void changeNearbyEnemyCombatRange(int iChange);

	int getExtraCombatPercent() const;
	void changeExtraCombatPercent(int iChange);

	int getExtraCityAttackPercent() const;
	void changeExtraCityAttackPercent(int iChange);

	int getExtraCityDefensePercent() const;
	void changeExtraCityDefensePercent(int iChange);

	int getExtraRangedDefenseModifier() const;
	void changeExtraRangedDefenseModifier(int iChange);

	int getExtraHillsAttackPercent() const;
	void changeExtraHillsAttackPercent(int iChange);

	int getExtraHillsDefensePercent() const;
	void changeExtraHillsDefensePercent(int iChange);

	int getExtraOpenAttackPercent() const;
	void changeExtraOpenAttackPercent(int iChange);

	int getExtraOpenRangedAttackMod() const;
	void changeExtraOpenRangedAttackMod(int iChange);

	int getExtraRoughAttackPercent() const;
	void changeExtraRoughAttackPercent(int iChange);

	int getExtraRoughRangedAttackMod() const;
	void changeExtraRoughRangedAttackMod(int iChange);

	int getExtraAttackFortifiedMod() const;
	void changeExtraAttackFortifiedMod(int iChange);

	int getExtraAttackWoundedMod() const;
	void changeExtraAttackWoundedMod(int iChange);

	int getExtraAttackFullyHealedMod() const;
	void changeExtraAttackFullyHealedMod(int iChange);


	int getExtraAttackAboveHealthMod() const;
	void changeExtraAttackAboveHealthMod(int iChange);

	int getExtraAttackBelowHealthMod() const;
	void changeExtraAttackBelowHealthMod(int iChange);

	int GetFlankAttackModifier() const;
	void ChangeFlankAttackModifier(int iChange);

	int getExtraOpenDefensePercent() const;
	void changeExtraOpenDefensePercent(int iChange);

	int getExtraOpenFromPercent() const;
	void changeExtraOpenFromPercent(int iChange);

	int getExtraRoughDefensePercent() const;
	void changeExtraRoughDefensePercent(int iChange);

	int getExtraRoughFromPercent() const;
	void changeExtraRoughFromPercent(int iChange);

	int getNumAttacks() const;
	int getNumAttacksMadeThisTurn() const;
	void changeExtraAttacks(int iChange);

	int GetNearbyCityBonusCombatMod(const CvPlot * pAtPlot = NULL) const;

	int GetGoldenAgeGeneralExpPercent() const;
	int GetGiveExperiencePercentToUnit() const;
	int GetGiveCombatModToUnit(const CvPlot * pAtPlot = NULL) const;
	int GetGiveDefenseModToUnit() const;
	int GetHealEnemyTerritoryFromNearbyUnit() const;
	int GetHealNeutralTerritoryFromNearbyUnit() const;
	int GetHealFriendlyTerritoryFromNearbyUnit() const;
	int GetGiveOutsideFriendlyLandsModifierToUnit() const;
	int GetGiveExtraAttacksToUnit() const;
	int GetGiveHPIfEnemyKilledToUnit() const;
	bool IsHiddenByNearbyUnit(const CvPlot * pAtPlot = NULL) const;

	// Great General Stuff
	bool IsNearCityAttackSupport(const CvPlot* pAtPlot = NULL, const CvUnit* pIgnoreThisGeneral = NULL) const;
	bool IsNearGreatGeneral(const CvPlot* pAtPlot = NULL, const CvUnit* pIgnoreThisGeneral = NULL) const;

	bool IsStackedGreatGeneral(const CvPlot* pAtPlot = NULL, const CvUnit* pIgnoreThisGeneral = NULL) const;
	int GetGreatGeneralStackMovement(const CvPlot* pAtPlot = NULL) const;
	int GetReverseGreatGeneralModifier(const CvPlot* pAtPlot = NULL) const;
	int GetNearbyImprovementModifier(const CvPlot* pAtPlot = NULL) const;
#if defined(MOD_PROMOTIONS_IMPROVEMENT_BONUS)
	int GetNearbyImprovementModifierFromTraits(const CvPlot* pAtPlot = NULL) const;
	int GetNearbyImprovementModifierFromPromotions(const CvPlot* pAtPlot = NULL) const;
	int GetNearbyImprovementModifier(ImprovementTypes eBonusImprovement, int iImprovementRange, int iImprovementModifier, const CvPlot* pAtPlot = NULL) const;
#endif

	bool IsGreatGeneral() const;
	int GetGreatGeneralCount() const;
	void ChangeGreatGeneralCount(int iChange);
	bool IsGreatAdmiral() const;
	int GetGreatAdmiralCount() const;
	void ChangeGreatAdmiralCount(int iChange);

#if defined(MOD_PROMOTIONS_AURA_CHANGE)
	int GetAuraRangeChange() const;
	void ChangeAuraRangeChange(int iChange);
	int GetAuraEffectChange() const;
	void ChangeAuraEffectChange(int iChange);

	int GetNumRepairCharges() const;
	void ChangeNumRepairCharges(int iChange);

	int GetMilitaryCapChange() const;
	void ChangeMilitaryCapChange(int iChange);

#endif

	int getGreatGeneralModifier() const;
	void changeGreatGeneralModifier(int iChange);

	bool IsGreatGeneralReceivesMovement() const;
	void ChangeGreatGeneralReceivesMovementCount(int iChange);
	int GetGreatGeneralCombatModifier() const;
	void ChangeGreatGeneralCombatModifier(int iChange);

	bool IsIgnoreGreatGeneralBenefit() const;
	void ChangeIgnoreGreatGeneralBenefitCount(int iChange);
	// END Great General Stuff

#if defined(MOD_UNITS_NO_SUPPLY)
	bool isNoSupply() const;
	void changeNoSupply(int iChange);
#endif

	int getMaxHitPointsBase() const;
	void setMaxHitPointsBase(int iMaxHitPoints);
	void changeMaxHitPointsBase(int iChange);
	
	int getMaxHitPointsChange() const;
	void changeMaxHitPointsChange(int iChange);
	int getMaxHitPointsModifier() const;
	void changeMaxHitPointsModifier(int iChange);

	bool IsIgnoreZOC() const;
	void ChangeIgnoreZOCCount(int iChange);

	bool IsSapper() const;
	void ChangeSapperCount(int iChange);
#if defined(MOD_BALANCE_CORE)
	int GetNearbyUnitClassModifierFromUnitClass(const CvPlot* pAtPlot = NULL) const;
	int GetNearbyUnitClassModifier(UnitClassTypes eUnitClass, int iUnitClassRange, int iUnitClassModifier, const CvPlot* pAtPlot = NULL) const;
	void DoNearbyUnitPromotion(const CvPlot* pPlot = NULL);
	void DoImprovementExperience(const CvPlot* pPlot = NULL);
	bool IsStrongerDamaged() const;
	void ChangeIsStrongerDamaged(int iChange);
	bool IsFightWellDamaged() const;
	void ChangeIsFightWellDamaged(int iChange);

	int GetGoodyHutYieldBonus() const;
	void ChangeGoodyHutYieldBonus(int iChange);

	int GetReligiousPressureModifier() const;
	void ChangeReligiousPressureModifier(int iChange);

	void DoStackedGreatGeneralExperience(const CvPlot* pPlot = NULL);
	void DoConvertOnDamageThreshold(const CvPlot* pPlot = NULL);
	void DoConvertEnemyUnitToBarbarian(const CvPlot* pPlot = NULL);
	void DoConvertReligiousUnitsToMilitary(const CvPlot* pPlot = NULL);
	void DoFinishBuildIfSafe();
#endif

	bool IsCanHeavyCharge() const;
	void ChangeCanHeavyChargeCount(int iChange);

#if defined(MOD_BALANCE_CORE)
	int GetMoraleBreakChance() const;
	void ChangeMoraleBreakChance(int iChange);

	int GetDamageAoEFortified() const;
	void ChangeDamageAoEFortified(int iChange);

	int GetWorkRateMod() const;
	void ChangeWorkRateMod(int iChange);

	int GetDamageReductionCityAssault() const;
	void ChangeDamageReductionCityAssault(int iChange);
#endif

	int getFriendlyLandsModifier() const;
	void changeFriendlyLandsModifier(int iChange);

	int getFriendlyLandsAttackModifier() const;
	void changeFriendlyLandsAttackModifier(int iChange);

	int getOutsideFriendlyLandsModifier() const;
	void changeOutsideFriendlyLandsModifier(int iChange);

	int getPillageChange() const;
	void changePillageChange(int iChange);

	int getUpgradeDiscount() const;
	void changeUpgradeDiscount(int iChange);

	int getExperiencePercent() const;
	void changeExperiencePercent(int iChange);

	int getKamikazePercent() const;
	void changeKamikazePercent(int iChange);

	DirectionTypes getFacingDirection(bool checkLineOfSightProperty) const;
	void setFacingDirection(DirectionTypes facingDirection);
	void rotateFacingDirectionClockwise();
	void rotateFacingDirectionCounterClockwise();

	bool isSuicide() const;
	bool isTrade() const;

	int getDropRange() const;
	void changeDropRange(int iChange);

	bool isOutOfAttacks() const;
	void setMadeAttack(bool bNewValue);

	int GetNumInterceptions() const;
	void ChangeNumInterceptions(int iChange);

	int GetExtraAirInterceptRange() const; // JJ: New
	void ChangeExtraAirInterceptRange(int iChange);

	bool canInterceptNow() const;
	int getMadeInterceptionCount() const;
	void increaseInterceptionCount();
	void resetInterceptionCount();

	bool TurnProcessed() const;
	void SetTurnProcessed(bool bValue);

	bool isPromotionReady() const;
	void setPromotionReady(bool bNewValue);
	void testPromotionReady();

	bool isDelayedDeath() const;
	bool isDelayedDeathExported() const;
	void startDelayedDeath();
	bool doDelayedDeath();

	bool isCombatFocus() const;

	bool isInfoBarDirty() const;
	void setInfoBarDirty(bool bNewValue);

	bool IsNotConverting() const;
	void SetNotConverting(bool bNewValue);

	inline PlayerTypes getOwner() const
	{
		return m_eOwner.get();
	}
	PlayerTypes getVisualOwner(TeamTypes eForTeam = NO_TEAM) const;
	PlayerTypes getCombatOwner(TeamTypes eForTeam, const CvPlot& pPlot) const;
	TeamTypes getTeam() const;

	PlayerTypes GetOriginalOwner() const;
	void SetOriginalOwner(PlayerTypes ePlayer);

	PlayerTypes getCapturingPlayer() const;
	void setCapturingPlayer(PlayerTypes eNewValue);
	bool IsCapturedAsIs() const;
	void SetCapturedAsIs(bool bSetValue);

	const UnitTypes getUnitType() const;
	CvUnitEntry& getUnitInfo() const;
	UnitClassTypes getUnitClassType() const;
	bool isUnitAI(UnitAITypes eType) const;

	const UnitTypes getLeaderUnitType() const;
	void setLeaderUnitType(UnitTypes leaderUnitType);

	const InvisibleTypes getInvisibleType() const;
	void setInvisibleType(InvisibleTypes InvisibleType);

	const InvisibleTypes getSeeInvisibleType() const;
	void setSeeInvisibleType(InvisibleTypes InvisibleType);

	const CvUnit* getCombatUnit() const;
	CvUnit* getCombatUnit();
	void setCombatUnit(CvUnit* pUnit, bool bAttacking = false);

	const CvCity* getCombatCity() const;
	CvCity* getCombatCity();
	void setCombatCity(CvCity* pCity);

	void clearCombat();

	int getExtraDomainModifier(DomainTypes eIndex) const;
	void changeExtraDomainModifier(DomainTypes eIndex, int iChange);

	const CvString getName() const;
	const char* getNameKey() const;
#if defined(MOD_PROMOTIONS_UNIT_NAMING)
	const CvString getUnitName() const;
	void setUnitName(const CvString strNewValue);
#endif
	const CvString getNameNoDesc() const;
	void setName(const CvString strNewValue);
#if defined(MOD_GLOBAL_NO_LOST_GREATWORKS)
	const CvString getGreatName() const;
	void setGreatName(CvString strName);
#endif
	GreatWorkType GetGreatWork() const;
	void SetGreatWork(GreatWorkType eGreatWork);
#if defined(MOD_API_EXTENSIONS)
	bool HasGreatWork() const;
	bool HasUnusedGreatWork() const;
#endif
	int GetTourismBlastStrength() const;
	void SetTourismBlastStrength(int iValue);

	int GetTourismBlastLength() const;
	void SetTourismBlastLength(int iValue);

#if defined(MOD_BALANCE_CORE)
	int GetScienceBlastStrength() const;
	void SetScienceBlastStrength(int iValue);

	int GetHurryStrength() const;
	void SetHurryStrength(int iValue);

	int GetCultureBlastStrength() const;
	void SetCultureBlastStrength(int iValue);

	int GetGAPBlastStrength() const;
	void SetGAPBlastStrength(int iValue);

	int getGAPBlast();

	bool IsPromotionEverObtained(PromotionTypes eIndex) const;
	void SetPromotionEverObtained(PromotionTypes eIndex, bool bValue);
#endif

	// Arbitrary Script Data
	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);
	int getScenarioData() const;
	void setScenarioData(int iNewValue);

	int getTerrainDoubleMoveCount(TerrainTypes eIndex) const;
	inline bool isTerrainDoubleMove(TerrainTypes eIndex) const { return getTerrainDoubleMoveCount(eIndex) > 0; }
	void changeTerrainDoubleMoveCount(TerrainTypes eIndex, int iChange);

	int getFeatureDoubleMoveCount(FeatureTypes eIndex) const;
	inline bool isFeatureDoubleMove(FeatureTypes eIndex) const { return getFeatureDoubleMoveCount(eIndex) > 0; }
	void changeFeatureDoubleMoveCount(FeatureTypes eIndex, int iChange);

#if defined(MOD_PROMOTIONS_HALF_MOVE)
	int getTerrainHalfMoveCount(TerrainTypes eIndex) const;
	inline bool isTerrainHalfMove(TerrainTypes eIndex) const { return getTerrainHalfMoveCount(eIndex) > 0; }
	void changeTerrainHalfMoveCount(TerrainTypes eIndex, int iChange);

	int getTerrainExtraMoveCount(TerrainTypes eIndex) const;
	inline bool isTerrainExtraMove(TerrainTypes eIndex) const { return getTerrainExtraMoveCount(eIndex) > 0; }
	void changeTerrainExtraMoveCount(TerrainTypes eIndex, int iChange);

	int getFeatureHalfMoveCount(FeatureTypes eIndex) const;
	inline bool isFeatureHalfMove(FeatureTypes eIndex) const { return getFeatureHalfMoveCount(eIndex) > 0; }
	void changeFeatureHalfMoveCount(FeatureTypes eIndex, int iChange);

	int getFeatureExtraMoveCount(FeatureTypes eIndex) const;
	inline bool isFeatureExtraMove(FeatureTypes eIndex) const { return getFeatureExtraMoveCount(eIndex) > 0; }
	void changeFeatureExtraMoveCount(FeatureTypes eIndex, int iChange);
#endif
#if defined(MOD_BALANCE_CORE)
	int getTerrainDoubleHeal(TerrainTypes eIndex) const;
	bool isTerrainDoubleHeal(TerrainTypes eIndex) const;
	void changeTerrainDoubleHeal(TerrainTypes eIndex, int iChange);

	int getFeatureDoubleHeal(FeatureTypes eIndex) const;
	bool isFeatureDoubleHeal(FeatureTypes eIndex) const;
	void changeFeatureDoubleHeal(FeatureTypes eIndex, int iChange);

	void ChangeNumTimesAttackedThisTurn(PlayerTypes ePlayer, int iValue);
	int GetNumTimesAttackedThisTurn(PlayerTypes ePlayer) const;

	int getCombatModPerAdjacentUnitCombatModifier(UnitCombatTypes eIndex) const;
	void changeCombatModPerAdjacentUnitCombatModifier(UnitCombatTypes eIndex, int iChange);

	int getCombatModPerAdjacentUnitCombatAttackMod(UnitCombatTypes eIndex) const;
	void changeCombatModPerAdjacentUnitCombatAttackMod(UnitCombatTypes eIndex, int iChange);

	int getCombatModPerAdjacentUnitCombatDefenseMod(UnitCombatTypes eIndex) const;
	void changeCombatModPerAdjacentUnitCombatDefenseMod(UnitCombatTypes eIndex, int iChange);
#endif

	int getTerrainImpassableCount(TerrainTypes eIndex) const;
	bool isTerrainImpassable(TerrainTypes eIndex) const;
	void changeTerrainImpassableCount(TerrainTypes eIndex, int iChange);

	int getFeatureImpassableCount(FeatureTypes eIndex) const;
	bool isFeatureImpassable(FeatureTypes eIndex) const;
	void changeFeatureImpassableCount(FeatureTypes eIndex, int iChange);

	int getExtraTerrainAttackPercent(TerrainTypes eIndex) const;
	void changeExtraTerrainAttackPercent(TerrainTypes eIndex, int iChange);
	int getExtraTerrainDefensePercent(TerrainTypes eIndex) const;
	void changeExtraTerrainDefensePercent(TerrainTypes eIndex, int iChange);
	int getExtraFeatureAttackPercent(FeatureTypes eIndex) const;
	void changeExtraFeatureAttackPercent(FeatureTypes eIndex, int iChange);
	int getExtraFeatureDefensePercent(FeatureTypes eIndex) const;
	void changeExtraFeatureDefensePercent(FeatureTypes eIndex, int iChange);

	int getUnitClassAttackMod(UnitClassTypes eUnitClass) const;
	void changeUnitClassAttackMod(UnitClassTypes eUnitClass, int iChange);

	int getUnitClassDefenseMod(UnitClassTypes eUnitClass) const;
	void changeUnitClassDefenseMod(UnitClassTypes eUnitClass, int iChange);

#if defined(MOD_API_UNIFIED_YIELDS)
	int getYieldFromKills(YieldTypes eIndex) const;
	void changeYieldFromKills(YieldTypes eIndex, int iChange);
	int getYieldFromBarbarianKills(YieldTypes eIndex) const;
	void changeYieldFromBarbarianKills(YieldTypes eIndex, int iChange);
#endif
#if defined(MOD_BALANCE_CORE)
	int getYieldFromScouting(YieldTypes eIndex) const;
	void changeYieldFromScouting(YieldTypes eIndex, int iChange);
	bool isCultureFromExperienceDisbandUpgrade() const;
	bool isFreeUpgrade() const;
	bool isUnitEraUpgrade() const;
	bool isReligiousUnit() const;
	bool isWLKTKDOnBirth() const;
	bool isGoldenAgeOnBirth() const;
	bool isCultureBoost() const;
	bool isExtraAttackHealthOnKill() const;
	bool isHighSeaRaiderUnit() const;
#endif

	int getExtraUnitCombatModifier(UnitCombatTypes eIndex) const;
	void changeExtraUnitCombatModifier(UnitCombatTypes eIndex, int iChange);

	int getUnitClassModifier(UnitClassTypes eIndex) const;
	void changeUnitClassModifier(UnitClassTypes eIndex, int iChange);

	bool canAcquirePromotion(PromotionTypes ePromotion) const;
	bool canAcquirePromotionAny() const;
	bool isPromotionValid(PromotionTypes ePromotion) const;
	bool isHasPromotion(PromotionTypes eIndex) const;
	void setHasPromotion(PromotionTypes eIndex, bool bNewValue);

	int getSubUnitCount() const;
	int getSubUnitsAlive() const;
	int getSubUnitsAlive(int iDamage) const;

	bool isEnemy(TeamTypes eTeam, const CvPlot* pPlot = NULL) const;

	bool canRangeStrike() const;
	bool canEverRangeStrikeAt(int iX, int iY, const CvPlot* pSourcePlot, bool bIgnoreVisibility) const;
	bool canEverRangeStrikeAt(int iX, int iY) const;
	bool canRangeStrikeAt(int iX, int iY, bool bNeedWar = true, bool bNoncombatAllowed = true) const;

#if defined(MOD_GLOBAL_STACKING_RULES)
	int getNumberStackingUnits() const;
	bool IsStackingUnit() const;
	bool IsCargoCombatUnit() const;
#endif

	bool IsAirSweepCapable() const;
	int GetAirSweepCapableCount() const;
	void ChangeAirSweepCapableCount(int iChange);

	bool canAirSweep() const;
	bool canAirSweepAt(int iX, int iY) const;
	bool airSweep(int iX, int iY);

	bool attemptGroundAttacks(const CvPlot& pPlot);

	bool isAlwaysHostile(const CvPlot& pPlot) const;
	void changeAlwaysHostileCount(int iValue);
	int getAlwaysHostileCount() const;

	int getArmyID() const;
	void setArmyID(int iNewArmyID);
	CvString getTacticalZoneInfo() const;
	bool shouldHeal(bool bBeforeAttacks = false) const;

	void setTacticalMove(AITacticalMove eMove);
	AITacticalMove getTacticalMove(int* pTurnSet=NULL) const;
	bool canUseForAIOperation() const;
	bool canUseForTacticalAI() const;

	void SetTacticalAIPlot(CvPlot* pPlot);
	CvPlot* GetTacticalAIPlot() const;

	void LogWorkerEvent(BuildTypes eBuildType, bool bStartingConstruction);

	int GetPower() const;

	bool CanSwapWithUnitHere(CvPlot& atPlot) const;
	CvUnit* GetPotentialUnitToSwapWith(CvPlot& atPlot) const;

	bool CanPushOutUnitHere(CvPlot& atPlot) const;
	CvUnit* GetPotentialUnitToPushOut(CvPlot& atPlot) const;
	bool PushBlockingUnitOutOfPlot(CvPlot& atPlot);

	bool CanStackUnitAtPlot(const CvPlot* pPlot) const;
	int CountStackingUnitsAtPlot(const CvPlot* pPlot) const;

	void read(FDataStream& kStream);
	void write(FDataStream& kStream) const;

	void AI_promote();
	UnitAITypes AI_getUnitAIType() const;
	void AI_setUnitAIType(UnitAITypes eNewValue);
	int AI_promotionValue(PromotionTypes ePromotion);

	GreatPeopleDirectiveTypes GetGreatPeopleDirective() const;
	void SetGreatPeopleDirective(GreatPeopleDirectiveTypes eDirective);

	bool IsSelected() const;
	bool IsFirstTimeSelected() const;
	void IncrementFirstTimeSelected();

	void SetPosition(CvPlot* pkPlot);

	const FAutoArchive& getSyncArchive() const;
	FAutoArchive& getSyncArchive();

	const CvSyncObject<CvUnit>& getSyncObject() const;
	CvSyncObject<CvUnit>& getSyncObject();

	// Mission routines
	void PushMission(MissionTypes eMission, int iData1 = -1, int iData2 = -1, int iFlags = 0, bool bAppend = false, bool bManual = false, MissionAITypes eMissionAI = NO_MISSIONAI, CvPlot* pMissionAIPlot = NULL, CvUnit* pMissionAIUnit = NULL);
	void PopMission();
	void AutoMission();
	void UpdateMission();
	CvPlot* LastMissionPlot();
	bool CanStartMission(int iMission, int iData1, int iData2, CvPlot* pPlot = NULL, bool bTestVisible = false);
	int GetMissionTimer() const;
	void SetMissionTimer(int iNewValue);
	void ChangeMissionTimer(int iChange);
	void ClearMissionQueue(bool bKeepPathCache = false, int iUnitCycleTimer = 1);
	int GetLengthMissionQueue() const;
	const MissionData* GetHeadMissionData() const;
	const MissionData* GetMissionData(int iIndex) const;
	CvPlot* GetMissionAIPlot() const;
	MissionAITypes GetMissionAIType() const;
	void SetMissionAI(MissionAITypes eNewMissionAI, CvPlot* pNewPlot, CvUnit* pNewUnit);
	CvUnit* GetMissionAIUnit();

	CvUnit* rangeStrikeTarget(const CvPlot& pPlot, bool bNoncombatAllowed) const;

	bool IsCanAttackWithMove() const;
	bool IsCanAttackRanged() const;
	bool IsCanAttack() const;
	bool IsCanAttackWithMoveNow() const;
	bool IsCanDefend() const;
	bool IsCivilianUnit() const;
	bool IsCombatUnit() const;

	ReachablePlots GetAllPlotsInReachThisTurn(bool bCheckTerritory=true, bool bCheckZOC=true, bool bAllowEmbark=true, int iMinMovesLeft=0) const;
	bool IsEnemyInMovementRange(bool bOnlyFortified = false, bool bOnlyCities = false);

	// Path-finding routines
	bool GeneratePath(const CvPlot* pToPlot, int iFlags = 0, int iMaxTurns = INT_MAX, int* piPathTurns = NULL);

	// you must call GeneratePath with caching before using these methods!
	CvPlot* GetPathFirstPlot() const;
	CvPlot* GetPathLastPlot() const;
	CvPlot* GetPathEndFirstTurnPlot() const;
	int GetMovementPointsAtCachedTarget() const;
	CvPlot* GetLastValidDestinationPlotInCachedPath() const;
	const CvPathNodeArray& GetLastPath() const;
	bool IsCurrentPathUnsafe() const;

	bool IsEmbarkAllWater() const;
	void ChangeEmbarkAllWaterCount(int iValue);
	int GetEmbarkAllWaterCount() const;

#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
	bool IsEmbarkDeepWater() const;
	void ChangeEmbarkDeepWaterCount(int iValue);
	int GetEmbarkDeepWaterCount() const;
#endif

	void ChangeEmbarkExtraVisibility(int iValue);
	int GetEmbarkExtraVisibility() const;

	void ChangeEmbarkDefensiveModifier(int iValue);
	int GetEmbarkDefensiveModifier() const;

	void ChangeCapitalDefenseModifier(int iValue);
	int GetCapitalDefenseModifier() const;
	void ChangeCapitalDefenseFalloff(int iValue);
	int GetCapitalDefenseFalloff() const;

	void ChangeCityAttackPlunderModifier(int iValue);
	int GetCityAttackPlunderModifier() const;

	void ChangeReligiousStrengthLossRivalTerritory(int iValue);
	int GetReligiousStrengthLossRivalTerritory() const;

	void ChangeTradeMissionInfluenceModifier(int iValue);
	int GetTradeMissionInfluenceModifier() const;

	void ChangeTradeMissionGoldModifier(int iValue);
	int GetTradeMissionGoldModifier() const;

	void ChangeDiploMissionInfluence(int iValue);
	int GetDiploMissionInfluence() const;

	bool IsHasBeenPromotedFromGoody() const;
	void SetBeenPromotedFromGoody(bool bBeenPromoted);

#if defined(MOD_BALANCE_CORE)
	bool IsHigherPopThan(const CvUnit* pOtherUnit) const;
#endif
	bool IsHigherTechThan(UnitTypes otherUnit) const;
	bool IsLargerCivThan(const CvUnit* pOtherUnit) const;

	int GetNumGoodyHutsPopped() const;
	void ChangeNumGoodyHutsPopped(int iValue);

	CvUnitReligion* GetReligionData() const
	{
		return m_pReligion;
	};

	static void dispatchingNetMessage(bool dispatching);
	static bool dispatchingNetMessage();

	std::string debugDump(const FAutoVariableBase&) const;
	std::string stackTraceRemark(const FAutoVariableBase&) const;

#if defined(MOD_BALANCE_CORE_MILITARY)
	const char* GetMissionInfo();
	void DumpDangerInNeighborhood();
#endif

#if defined(MOD_BALANCE_CORE_MILITARY)
	void setHomelandMove(AIHomelandMove eMove);
	AIHomelandMove getHomelandMove(int* pTurnSet=NULL) const;
	bool hasCurrentTacticalMove() const;
#endif

#if defined(MOD_API_EXTENSIONS)
	bool IsCivilization(CivilizationTypes iCivilizationType) const;
	bool HasPromotion(PromotionTypes iPromotionType) const;
	bool IsUnit(UnitTypes iUnitType) const;
	bool IsUnitClass(UnitClassTypes iUnitClassType) const;
	bool IsOnFeature(FeatureTypes iFeatureType) const;
	bool IsAdjacentToFeature(FeatureTypes iFeatureType) const;
	bool IsWithinDistanceOfFeature(FeatureTypes iFeatureType, int iDistance) const;
#if defined(MOD_BALANCE_CORE)
	bool IsWithinDistanceOfUnit(UnitTypes eOtherUnit, int iDistance, bool bIsFriendly, bool bIsEnemy) const;
	bool IsWithinDistanceOfUnitClass(UnitClassTypes eUnitClass, int iDistance, bool bIsFriendly, bool bIsEnemy) const;
	bool IsWithinDistanceOfUnitCombatType(UnitCombatTypes eUnitCombat, int iDistance, bool bIsFriendly, bool bIsEnemy) const;
	bool IsWithinDistanceOfUnitPromotion(PromotionTypes eUnitPromotion, int iDistance, bool bIsFriendly, bool bIsEnemy) const;
	bool IsWithinDistanceOfCity(int iDistance, bool bIsFriendly, bool bIsEnemy) const;
	bool IsAdjacentToUnit(UnitTypes eOtherUnit, bool bIsFriendly, bool bIsEnemy) const;
	bool IsAdjacentToUnitClass(UnitClassTypes eUnitClass, bool bIsFriendly, bool bIsEnemy) const;
	bool IsAdjacentToUnitCombatType(UnitCombatTypes eUnitCombat, bool bIsFriendly, bool bIsEnemy) const;
	bool IsAdjacentToUnitPromotion(PromotionTypes eUnitPromotion, bool bIsFriendly, bool bIsEnemy) const;
#endif
	bool IsOnImprovement(ImprovementTypes iImprovementType) const;
	bool IsAdjacentToImprovement(ImprovementTypes iImprovementType) const;
	bool IsWithinDistanceOfImprovement(ImprovementTypes iImprovementType, int iDistance) const;
	bool IsOnPlotType(PlotTypes iPlotType) const;
	bool IsAdjacentToPlotType(PlotTypes iPlotType) const;
	bool IsWithinDistanceOfPlotType(PlotTypes iPlotType, int iDistance) const;
	bool IsOnResource(ResourceTypes iResourceType) const;
	bool IsAdjacentToResource(ResourceTypes iResourceType) const;
	bool IsWithinDistanceOfResource(ResourceTypes iResourceType, int iDistance) const;
	bool IsOnTerrain(TerrainTypes iTerrainType) const;
	bool IsAdjacentToTerrain(TerrainTypes iTerrainType) const;
	bool IsWithinDistanceOfTerrain(TerrainTypes iTerrainType, int iDistance) const;
#endif

	int TurnsToReachTarget(const CvPlot* pTarget,int iFlags, int iMaxTurns);
	int TurnsToReachTarget(const CvPlot* pTarget, bool bIgnoreUnits = false, bool bIgnoreStacking = false, int iMaxTurns = MAX_INT);
	bool CanSafelyReachInXTurns(const CvPlot* pTarget, int iTurns);
	void ClearPathCache();

	bool	getCaptureDefinition(CvUnitCaptureDefinition* pkCaptureDef, PlayerTypes eCapturingPlayer = NO_PLAYER);
	static CvUnit* createCaptureUnit(const CvUnitCaptureDefinition& kCaptureDef, bool ForcedCapture = false);

protected:
	const MissionData* HeadMissionData() const;
	MissionData* HeadMissionData();

	void setMoves(int iNewValue);

	bool HaveCachedPathTo(const CvPlot* pToPlot, int iFlags) const;
	bool IsCachedPathValid() const;
	bool VerifyCachedPath(const CvPlot* pDestPlot, int iFlags, int iMaxTurns);
	//return -1 if impossible, turns to target otherwise (zero is valid!)
	int ComputePath(const CvPlot* pToPlot, int iFlags, int iMaxTurns, bool bCacheResult);

	bool HasQueuedVisualizationMoves() const;
	void QueueMoveForVisualization(CvPlot* pkPlot);
	void PublishQueuedVisualizationMoves();

	bool CheckDOWNeededForMove(int iX, int iY);
	MoveResult UnitAttackWithMove(int iX, int iY, int iFlags);
	int UnitPathTo(int iX, int iY, int iFlags);
	bool UnitMove(CvPlot* pPlot, bool bCombat, CvUnit* pCombatUnit, bool bEndMove = false);
	bool UnitRoadTo(int iX, int iY, int iFlags);
	bool UnitBuild(BuildTypes eBuild);

	FAutoArchiveClassContainer<CvUnit> m_syncArchive;
	SYNC_OBJECT_MEMBER(CvUnit)

	SYNC_VAR_MEMBER(m_eOwner)
	SYNC_VAR_MEMBER(m_eOriginalOwner)
	SYNC_VAR_MEMBER(m_eUnitType)
	SYNC_VAR_MEMBER(m_iX)
	SYNC_VAR_MEMBER(m_iY)
	SYNC_VAR_MEMBER(m_iID)

	SYNC_VAR_MEMBER(m_iDamage)
	SYNC_VAR_MEMBER(m_iMoves)
	SYNC_VAR_MEMBER(m_iArmyId)
	SYNC_VAR_MEMBER(m_iBaseCombat)
#if defined(MOD_API_EXTENSIONS)
	SYNC_VAR_MEMBER(m_iBaseRangedCombat)
#endif

	SYNC_VAR_MEMBER(m_iHotKeyNumber)
	SYNC_VAR_MEMBER(m_iDeployFromOperationTurn)
	SYNC_VAR_MEMBER(m_iLastMoveTurn)
	SYNC_VAR_MEMBER(m_iReconX)
	SYNC_VAR_MEMBER(m_iReconY)
	SYNC_VAR_MEMBER(m_iReconCount)
	SYNC_VAR_MEMBER(m_iGameTurnCreated)
	SYNC_VAR_MEMBER(m_bImmobile)
	SYNC_VAR_MEMBER(m_iExperienceTimes100)
	SYNC_VAR_MEMBER(m_iLevel)
	SYNC_VAR_MEMBER(m_iCargo)
	SYNC_VAR_MEMBER(m_iCargoCapacity)
	SYNC_VAR_MEMBER(m_iAttackPlotX)
	SYNC_VAR_MEMBER(m_iAttackPlotY)
	SYNC_VAR_MEMBER(m_iCombatTimer)
	SYNC_VAR_MEMBER(m_iCombatFirstStrikes)
	SYNC_VAR_MEMBER(m_iCombatDamage)
	SYNC_VAR_MEMBER(m_bMovedThisTurn)
	SYNC_VAR_MEMBER(m_bFortified)
	SYNC_VAR_MEMBER(m_iBlitzCount)
	SYNC_VAR_MEMBER(m_iAmphibCount)
	SYNC_VAR_MEMBER(m_iRiverCrossingNoPenaltyCount)
	SYNC_VAR_MEMBER(m_iEnemyRouteCount)
	SYNC_VAR_MEMBER(m_iRivalTerritoryCount)
	SYNC_VAR_MEMBER(m_iIsSlowInEnemyLandCount)
	SYNC_VAR_MEMBER(m_iRangeAttackIgnoreLOSCount)
	SYNC_VAR_MEMBER(m_iCityAttackOnlyCount)
	SYNC_VAR_MEMBER(m_iCaptureDefeatedEnemyCount)
#if defined(MOD_BALANCE_CORE)
	SYNC_VAR_MEMBER(m_iOriginCity)
	SYNC_VAR_MEMBER(m_iCannotBeCapturedCount)
	SYNC_VAR_MEMBER(m_iForcedDamage)
	SYNC_VAR_MEMBER(m_iChangeDamage)
	SYNC_VAR_MEMBER(m_PromotionDuration)
	SYNC_VAR_MEMBER(m_TurnPromotionGained)
#endif
	SYNC_VAR_MEMBER(m_iRangedSupportFireCount)
	SYNC_VAR_MEMBER(m_iAlwaysHealCount)
	SYNC_VAR_MEMBER(m_iHealOutsideFriendlyCount)
	SYNC_VAR_MEMBER(m_iHillsDoubleMoveCount)
#if defined(MOD_BALANCE_CORE)
	SYNC_VAR_MEMBER(m_iMountainsDoubleMoveCount)
	SYNC_VAR_MEMBER(m_iEmbarkFlatCostCount)
	SYNC_VAR_MEMBER(m_iDisembarkFlatCostCount)
	SYNC_VAR_MEMBER(m_iAOEDamageOnKill)
	SYNC_VAR_MEMBER(m_iAoEDamageOnMove)
	SYNC_VAR_MEMBER(m_iSplashDamage)
	SYNC_VAR_MEMBER(m_iMultiAttackBonus)
	SYNC_VAR_MEMBER(m_iLandAirDefenseValue)
#endif
	SYNC_VAR_MEMBER(m_iImmuneToFirstStrikesCount)
	SYNC_VAR_MEMBER(m_iExtraVisibilityRange)
#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
	SYNC_VAR_MEMBER(m_iExtraReconRange)
#endif
	SYNC_VAR_MEMBER(m_iExtraMoves)
	SYNC_VAR_MEMBER(m_iExtraMoveDiscount)
	SYNC_VAR_MEMBER(m_iExtraRange)
	SYNC_VAR_MEMBER(m_iInterceptChance)
	SYNC_VAR_MEMBER(m_iExtraEvasion)
	SYNC_VAR_MEMBER(m_iExtraFirstStrikes)
	SYNC_VAR_MEMBER(m_iExtraChanceFirstStrikes)
	SYNC_VAR_MEMBER(m_iExtraWithdrawal)
#if defined(MOD_BALANCE_CORE_JFD)
	SYNC_VAR_MEMBER(m_iPlagueChance)
	SYNC_VAR_MEMBER(m_iIsPlagued)
	SYNC_VAR_MEMBER(m_iPlagueID)
	SYNC_VAR_MEMBER(m_iPlaguePriority)
	SYNC_VAR_MEMBER(m_iPlagueIDImmunity)
	SYNC_VAR_MEMBER(m_iPlaguePromotion)
	SYNC_VAR_MEMBER(m_eUnitContract)
	SYNC_VAR_MEMBER(m_iNegatorPromotion)
#endif
	SYNC_VAR_MEMBER(m_bIsNoMaintenance)
	SYNC_VAR_MEMBER(m_iExtraEnemyHeal)
	SYNC_VAR_MEMBER(m_iExtraNeutralHeal)
	SYNC_VAR_MEMBER(m_iExtraFriendlyHeal)
	SYNC_VAR_MEMBER(m_iEnemyDamageChance)
	SYNC_VAR_MEMBER(m_iNeutralDamageChance)
	SYNC_VAR_MEMBER(m_iEnemyDamage)
	SYNC_VAR_MEMBER(m_iNeutralDamage)
	SYNC_VAR_MEMBER(m_iNearbyEnemyCombatMod)
	SYNC_VAR_MEMBER(m_iNearbyEnemyCombatRange)
	SYNC_VAR_MEMBER(m_iSameTileHeal)
	SYNC_VAR_MEMBER(m_iAdjacentTileHeal)
	SYNC_VAR_MEMBER(m_iAdjacentModifier)
	SYNC_VAR_MEMBER(m_iRangedAttackModifier)
	SYNC_VAR_MEMBER(m_iInterceptionCombatModifier)
	SYNC_VAR_MEMBER(m_iInterceptionDefenseDamageModifier)
	SYNC_VAR_MEMBER(m_iAirSweepCombatModifier)
	SYNC_VAR_MEMBER(m_iAttackModifier)
	SYNC_VAR_MEMBER(m_iDefenseModifier)
	SYNC_VAR_MEMBER(m_iGroundAttackDamage)
	SYNC_VAR_MEMBER(m_iExtraCombatPercent)
	SYNC_VAR_MEMBER(m_iExtraCityAttackPercent)
	SYNC_VAR_MEMBER(m_iExtraCityDefensePercent)
	SYNC_VAR_MEMBER(m_iExtraRangedDefenseModifier)
	SYNC_VAR_MEMBER(m_iExtraHillsAttackPercent)
	SYNC_VAR_MEMBER(m_iExtraHillsDefensePercent)
	SYNC_VAR_MEMBER(m_iExtraOpenAttackPercent)
	SYNC_VAR_MEMBER(m_iExtraOpenRangedAttackMod)
	SYNC_VAR_MEMBER(m_iExtraRoughAttackPercent)
	SYNC_VAR_MEMBER(m_iExtraRoughRangedAttackMod)
	SYNC_VAR_MEMBER(m_iExtraAttackFortifiedMod)
	SYNC_VAR_MEMBER(m_iExtraAttackWoundedMod)
	SYNC_VAR_MEMBER(m_iExtraFullyHealedMod)
	SYNC_VAR_MEMBER(m_iExtraAttackAboveHealthMod)
	SYNC_VAR_MEMBER(m_iExtraAttackBelowHealthMod)
	SYNC_VAR_MEMBER(m_iFlankAttackModifier)
	SYNC_VAR_MEMBER(m_iExtraOpenDefensePercent)
	SYNC_VAR_MEMBER(m_iExtraRoughDefensePercent)
	SYNC_VAR_MEMBER(m_iExtraOpenFromPercent)
	SYNC_VAR_MEMBER(m_iExtraRoughFromPercent)
	SYNC_VAR_MEMBER(m_iPillageChange)
	SYNC_VAR_MEMBER(m_iUpgradeDiscount)
	SYNC_VAR_MEMBER(m_iExperiencePercent)
	SYNC_VAR_MEMBER(m_iDropRange)
	SYNC_VAR_MEMBER(m_iAirSweepCapableCount)
	SYNC_VAR_MEMBER(m_iExtraNavalMoves)
	SYNC_VAR_MEMBER(m_iKamikazePercent)
	SYNC_VAR_MEMBER(m_eFacingDirection)
	SYNC_VAR_MEMBER(m_iIgnoreTerrainCostCount)
	SYNC_VAR_MEMBER(m_iIgnoreTerrainDamageCount)
	SYNC_VAR_MEMBER(m_iIgnoreFeatureDamageCount)
	SYNC_VAR_MEMBER(m_iExtraTerrainDamageCount)
	SYNC_VAR_MEMBER(m_iExtraFeatureDamageCount)
#if defined(MOD_PROMOTIONS_IMPROVEMENT_BONUS)
	SYNC_VAR_MEMBER(m_iNearbyImprovementCombatBonus)
	SYNC_VAR_MEMBER(m_iNearbyImprovementBonusRange)
	SYNC_VAR_MEMBER(m_eCombatBonusImprovement)
#endif
#if defined(MOD_BALANCE_CORE)
	SYNC_VAR_MEMBER(m_iNearbyUnitClassBonus)
	SYNC_VAR_MEMBER(m_iNearbyUnitClassBonusRange)
	SYNC_VAR_MEMBER(m_iCombatBonusFromNearbyUnitClass)
	SYNC_VAR_MEMBER(m_bNearbyPromotion)
	SYNC_VAR_MEMBER(m_iNearbyUnitPromotionRange)
	SYNC_VAR_MEMBER(m_iNearbyCityCombatMod)
	SYNC_VAR_MEMBER(m_iNearbyFriendlyCityCombatMod)
	SYNC_VAR_MEMBER(m_iNearbyEnemyCityCombatMod)
	SYNC_VAR_MEMBER(m_iPillageBonusStrengthPercent)
	SYNC_VAR_MEMBER(m_iStackedGreatGeneralExperience)
	SYNC_VAR_MEMBER(m_bIsHighSeaRaider)
	SYNC_VAR_MEMBER(m_iWonderProductionModifier)
	SYNC_VAR_MEMBER(m_iUnitProductionModifier)
	SYNC_VAR_MEMBER(m_iNearbyEnemyDamage)
	SYNC_VAR_MEMBER(m_iGGGAXPPercent)
	SYNC_VAR_MEMBER(m_iGiveCombatMod)
	SYNC_VAR_MEMBER(m_iGiveHPIfEnemyKilled)
	SYNC_VAR_MEMBER(m_iGiveExperiencePercent)
	SYNC_VAR_MEMBER(m_iGiveOutsideFriendlyLandsModifier)
	SYNC_VAR_MEMBER(m_eGiveDomain)
	SYNC_VAR_MEMBER(m_iGiveExtraAttacks)
	SYNC_VAR_MEMBER(m_iGiveDefenseMod)
	SYNC_VAR_MEMBER(m_bGiveInvisibility)
	SYNC_VAR_MEMBER(m_bGiveOnlyOnStartingTurn)
	SYNC_VAR_MEMBER(m_bConvertUnit)
	SYNC_VAR_MEMBER(m_eConvertDomain)
	SYNC_VAR_MEMBER(m_eConvertDomainUnit)
	SYNC_VAR_MEMBER(m_bConvertEnemyUnitToBarbarian)
	SYNC_VAR_MEMBER(m_bConvertOnFullHP)
	SYNC_VAR_MEMBER(m_bConvertOnDamage)
	SYNC_VAR_MEMBER(m_iDamageThreshold)
	SYNC_VAR_MEMBER(m_eConvertDamageOrFullHPUnit)
	SYNC_VAR_MEMBER(m_iNumberOfCultureBombs)
	SYNC_VAR_MEMBER(m_iNearbyHealEnemyTerritory)
	SYNC_VAR_MEMBER(m_iNearbyHealNeutralTerritory)
	SYNC_VAR_MEMBER(m_iNearbyHealFriendlyTerritory)
#endif
#if defined(MOD_PROMOTIONS_CROSS_MOUNTAINS)
	SYNC_VAR_MEMBER(m_iCanCrossMountainsCount)
#endif
#if defined(MOD_PROMOTIONS_CROSS_OCEANS)
	SYNC_VAR_MEMBER(m_iCanCrossOceansCount)
#endif
#if defined(MOD_PROMOTIONS_CROSS_ICE)
	SYNC_VAR_MEMBER(m_iCanCrossIceCount)
#endif
#if defined(MOD_BALANCE_CORE)
	SYNC_VAR_MEMBER(m_iNumTilesRevealedThisTurn)
	SYNC_VAR_MEMBER(m_bSpottedEnemy)
	SYNC_VAR_MEMBER(m_iGainsXPFromScouting)
	SYNC_VAR_MEMBER(m_iGainsXPFromPillaging)
	SYNC_VAR_MEMBER(m_iGainsXPFromSpotting)
	SYNC_VAR_MEMBER(m_iCaptureDefeatedEnemyChance)
	SYNC_VAR_MEMBER(m_iBarbCombatBonus)
	SYNC_VAR_MEMBER(m_iAdjacentEnemySapMovement)
#endif
#if defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
	SYNC_VAR_MEMBER(m_iGGFromBarbariansCount)
#endif
#if defined(MOD_PROMOTIONS_AURA_CHANGE)
	SYNC_VAR_MEMBER(m_iAuraRangeChange)
	SYNC_VAR_MEMBER(m_iAuraEffectChange)
	SYNC_VAR_MEMBER(m_iNumRepairCharges)
	SYNC_VAR_MEMBER(m_iMilitaryCapChange)
#endif
	SYNC_VAR_MEMBER(m_iRoughTerrainEndsTurnCount)
	SYNC_VAR_MEMBER(m_iEmbarkAbilityCount)
	SYNC_VAR_MEMBER(m_iHoveringUnitCount)
	SYNC_VAR_MEMBER(m_iFlatMovementCostCount)
	SYNC_VAR_MEMBER(m_iCanMoveImpassableCount)
	SYNC_VAR_MEMBER(m_iOnlyDefensiveCount)
	SYNC_VAR_MEMBER(m_iNoDefensiveBonusCount)
	SYNC_VAR_MEMBER(m_iNoCaptureCount)
	SYNC_VAR_MEMBER(m_iNukeImmuneCount)
	SYNC_VAR_MEMBER(m_iHiddenNationalityCount)
	SYNC_VAR_MEMBER(m_iAlwaysHostileCount)
	SYNC_VAR_MEMBER(m_iNoRevealMapCount)
	SYNC_VAR_MEMBER(m_iCanMoveAllTerrainCount)
	SYNC_VAR_MEMBER(m_iCanMoveAfterAttackingCount)
	SYNC_VAR_MEMBER(m_iFreePillageMoveCount)
	SYNC_VAR_MEMBER(m_iHealOnPillageCount)
	SYNC_VAR_MEMBER(m_iHPHealedIfDefeatEnemy)
	SYNC_VAR_MEMBER(m_iGoldenAgeValueFromKills)
	SYNC_VAR_MEMBER(m_iTacticalAIPlotX)
	SYNC_VAR_MEMBER(m_iTacticalAIPlotY)
	SYNC_VAR_MEMBER(m_iGarrisonCityID)
	SYNC_VAR_MEMBER(m_iNumAttacks)
	SYNC_VAR_MEMBER(m_iAttacksMade)
	SYNC_VAR_MEMBER(m_iGreatGeneralCount)
	SYNC_VAR_MEMBER(m_iGreatAdmiralCount)
	SYNC_VAR_MEMBER(m_iGreatGeneralModifier)
	SYNC_VAR_MEMBER(m_iGreatGeneralReceivesMovementCount)
	SYNC_VAR_MEMBER(m_iGreatGeneralCombatModifier)
	SYNC_VAR_MEMBER(m_iIgnoreGreatGeneralBenefit)
	SYNC_VAR_MEMBER(m_iIgnoreZOC)
#if defined(MOD_UNITS_NO_SUPPLY)
	SYNC_VAR_MEMBER(m_iNoSupply)
#endif
	int m_iMaxHitPointsBase;
	SYNC_VAR_MEMBER(m_iMaxHitPointsChange)
	SYNC_VAR_MEMBER(m_iMaxHitPointsModifier)
	SYNC_VAR_MEMBER(m_iFriendlyLandsModifier)
	SYNC_VAR_MEMBER(m_iFriendlyLandsAttackModifier)
	SYNC_VAR_MEMBER(m_iOutsideFriendlyLandsModifier)
	SYNC_VAR_MEMBER(m_iHealIfDefeatExcludeBarbariansCount)
	SYNC_VAR_MEMBER(m_iNumInterceptions)
#if defined(MOD_BALANCE_CORE)
	SYNC_VAR_MEMBER(m_iExtraAirInterceptRange)
#endif
	SYNC_VAR_MEMBER(m_iMadeInterceptionCount)
	SYNC_VAR_MEMBER(m_iEverSelectedCount)
	SYNC_VAR_MEMBER(m_iSapperCount)
	SYNC_VAR_MEMBER(m_iCanHeavyCharge)
#if defined(MOD_BALANCE_CORE)
	SYNC_VAR_MEMBER(m_iStrongerDamaged)
	SYNC_VAR_MEMBER(m_iFightWellDamaged)
	SYNC_VAR_MEMBER(m_iCanMoraleBreak)
	SYNC_VAR_MEMBER(m_iDamageAoEFortified)
	SYNC_VAR_MEMBER(m_iWorkRateMod)
	SYNC_VAR_MEMBER(m_iDamageReductionCityAssault)
	SYNC_VAR_MEMBER(m_iGoodyHutYieldBonus)
	SYNC_VAR_MEMBER(m_iReligiousPressureModifier)
#endif
	SYNC_VAR_MEMBER(m_iNumExoticGoods)
	SYNC_VAR_MEMBER(m_bPromotionReady)
	SYNC_VAR_MEMBER(m_bDeathDelay)
	SYNC_VAR_MEMBER(m_bCombatFocus)
	SYNC_VAR_MEMBER(m_bInfoBarDirty)
	SYNC_VAR_MEMBER(m_bNotConverting)
	SYNC_VAR_MEMBER(m_bAirCombat)
	//to be removed
		SYNC_VAR_MEMBER(m_bSetUpForRangedAttack)
	SYNC_VAR_MEMBER(m_bEmbarked)
	SYNC_VAR_MEMBER(m_bPromotedFromGoody)
	SYNC_VAR_MEMBER(m_bAITurnProcessed)
#if defined(MOD_CORE_PER_TURN_DAMAGE)
	SYNC_VAR_MEMBER(m_iDamageTakenThisTurn)
	SYNC_VAR_MEMBER(m_iDamageTakenLastTurn)
#endif

	SYNC_VAR_MEMBER(m_eCapturingPlayer)
	SYNC_VAR_MEMBER(m_bCapturedAsIs)
	SYNC_VAR_MEMBER(m_eLeaderUnitType)
	SYNC_VAR_MEMBER(m_eInvisibleType)
	SYNC_VAR_MEMBER(m_eSeeInvisibleType)
	SYNC_VAR_MEMBER(m_eGreatPeopleDirectiveType)
	CvUnitEntry* m_pUnitInfo;

	bool m_bWaitingForMove;			///< If true, the unit is busy visualizing its move.

	IDInfo m_combatUnit;
	IDInfo m_combatCity;
	IDInfo m_transportUnit;

	std::vector<int> m_extraDomainModifiers;
	std::vector<int> m_YieldModifier;
	std::vector<int> m_YieldChange;
	std::vector<int> m_iGarrisonYieldChange;
	std::vector<int> m_iFortificationYieldChange;

	SYNC_VAR_MEMBER(m_strScriptData)
	SYNC_VAR_MEMBER(m_iScenarioData)

	CvUnitPromotions  m_Promotions;
	CvUnitReligion* m_pReligion;

#if defined(MOD_CIV6_WORKER)
	SYNC_VAR_MEMBER(m_iBuilderStrength)
#endif

	SYNC_VAR_MEMBER(m_terrainDoubleMoveCount)
	SYNC_VAR_MEMBER(m_featureDoubleMoveCount)
#if defined(MOD_PROMOTIONS_HALF_MOVE)
	SYNC_VAR_MEMBER(m_terrainHalfMoveCount)
	SYNC_VAR_MEMBER(m_featureHalfMoveCount)

	SYNC_VAR_MEMBER(m_terrainExtraMoveCount)
	SYNC_VAR_MEMBER(m_featureExtraMoveCount)
#endif
#if defined(MOD_BALANCE_CORE)
	SYNC_VAR_MEMBER(m_terrainDoubleHeal)
	SYNC_VAR_MEMBER(m_featureDoubleHeal)
#endif
	SYNC_VAR_MEMBER(m_terrainImpassableCount)
	SYNC_VAR_MEMBER(m_featureImpassableCount)
	SYNC_VAR_MEMBER(m_extraTerrainAttackPercent)
	SYNC_VAR_MEMBER(m_extraTerrainDefensePercent)
	SYNC_VAR_MEMBER(m_extraFeatureAttackPercent)
	SYNC_VAR_MEMBER(m_extraFeatureDefensePercent)

	SYNC_VAR_MEMBER(m_extraUnitClassAttackMod)
	SYNC_VAR_MEMBER(m_extraUnitClassDefenseMod)
#if defined(MOD_BALANCE_CORE)
	SYNC_VAR_MEMBER(m_aiNumTimesAttackedThisTurn)
	SYNC_VAR_MEMBER(m_yieldFromScouting)
#endif
#if defined(MOD_API_UNIFIED_YIELDS)
	SYNC_VAR_MEMBER(m_yieldFromKills)
	SYNC_VAR_MEMBER(m_yieldFromBarbarianKills)
#endif
	SYNC_VAR_MEMBER(m_extraUnitCombatModifier)
	SYNC_VAR_MEMBER(m_unitClassModifier)
#if defined(MOD_BALANCE_CORE)
	SYNC_VAR_MEMBER(m_iCombatModPerAdjacentUnitCombatModifier)
	SYNC_VAR_MEMBER(m_iCombatModPerAdjacentUnitCombatAttackMod)
	SYNC_VAR_MEMBER(m_iCombatModPerAdjacentUnitCombatDefenseMod)
#endif
	SYNC_VAR_MEMBER(m_iMissionTimer)
	SYNC_VAR_MEMBER(m_iMissionAIX)
	SYNC_VAR_MEMBER(m_iMissionAIY)
	SYNC_VAR_MEMBER(m_eMissionAIType)
	IDInfo m_missionAIUnit;
	SYNC_VAR_MEMBER(m_eActivityType)
	SYNC_VAR_MEMBER(m_eAutomateType)
	SYNC_VAR_MEMBER(m_eUnitAIType) //current AI type, might be different from default
	SYNC_VAR_MEMBER(m_eCombatType)

	//not serialized
	std::vector<CvPlot*> m_unitMoveLocs;

	SYNC_VAR_MEMBER(m_iEmbarkedAllWaterCount)
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
	SYNC_VAR_MEMBER(m_iEmbarkedDeepWaterCount)
#endif
	SYNC_VAR_MEMBER(m_iEmbarkExtraVisibility)
	SYNC_VAR_MEMBER(m_iEmbarkDefensiveModifier)
	SYNC_VAR_MEMBER(m_iCapitalDefenseModifier)
	SYNC_VAR_MEMBER(m_iCapitalDefenseFalloff)
	SYNC_VAR_MEMBER(m_iCityAttackPlunderModifier)
	SYNC_VAR_MEMBER(m_iReligiousStrengthLossRivalTerritory)
	SYNC_VAR_MEMBER(m_iTradeMissionInfluenceModifier)
	SYNC_VAR_MEMBER(m_iTradeMissionGoldModifier)
	SYNC_VAR_MEMBER(m_iDiploMissionInfluence)
	SYNC_VAR_MEMBER(m_iMapLayer)		// Which layer does the unit reside on for pathing/stacking/etc.
	SYNC_VAR_MEMBER(m_iNumGoodyHutsPopped)
	SYNC_VAR_MEMBER(m_iTourismBlastStrength)
	SYNC_VAR_MEMBER(m_iTourismBlastLength)
#if defined(MOD_BALANCE_CORE)
	SYNC_VAR_MEMBER(m_iHurryStrength)
	SYNC_VAR_MEMBER(m_iScienceBlastStrength)
	SYNC_VAR_MEMBER(m_iCultureBlastStrength)
	SYNC_VAR_MEMBER(m_iGAPBlastStrength)
	SYNC_VAR_MEMBER(m_abPromotionEverObtained)
#endif
		
#if defined(MOD_PROMOTIONS_UNIT_NAMING)
	CvString m_strUnitName;
#endif
	CvString m_strName;
#if defined(MOD_GLOBAL_NO_LOST_GREATWORKS)
	CvString m_strGreatName;
#endif
	GreatWorkType m_eGreatWork;

	//this is always stored with the zero-counting convention
	//ie every plot we can reach this turn has turn count 0, even if there are no moves left
	mutable CvPathNodeArray m_kLastPath;
	mutable uint m_uiLastPathCacheOrigin;
	mutable uint m_uiLastPathCacheDestination;
	mutable uint m_uiLastPathFlags;
	mutable uint m_uiLastPathTurnSlice;

	bool canAdvance(const CvPlot& pPlot, int iThreshold) const;

#if defined(MOD_BALANCE_CORE)
	void DoPlagueTransfer(CvUnit& defender);
#endif
#if defined(MOD_CARGO_SHIPS)
	void DoCargoPromotions(CvUnit& cargounit);
	void RemoveCargoPromotions(CvUnit& cargounit);
#endif

	bool CanFallBack(const CvUnit& pAttacker, bool bCheckChances) const;
	bool DoFallBack(const CvUnit& pAttacker);

private:

	mutable MissionQueue m_missionQueue;

#if defined(MOD_BALANCE_CORE_MILITARY)
	// for debugging
	CvString m_strMissionInfoString;
	SYNC_VAR_MEMBER(m_eTacticalMove)
	SYNC_VAR_MEMBER(m_iTacticalMoveSetTurn)
	SYNC_VAR_MEMBER(m_eHomelandMove)
	SYNC_VAR_MEMBER(m_iHomelandMoveSetTurn)
#endif

	friend class CvLuaUnit;
};

FDataStream& operator<<(FDataStream&, const CvUnit&);
FDataStream& operator>>(FDataStream&, CvUnit&);

namespace FSerialization
{
void SyncUnits();
void ClearUnitDeltas();
}

#endif
