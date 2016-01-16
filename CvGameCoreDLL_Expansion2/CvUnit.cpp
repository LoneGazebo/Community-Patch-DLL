/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvUnit.h"
#include "CvArea.h"
#include "CvPlot.h"
#include "CvCity.h"
#include "CvGlobals.h"
#include "CvGameCoreUtils.h"
#include "CvInternalGameCoreUtils.h"
#include "CvMap.h"
#include "CvPlayerAI.h"
#include "CvRandom.h"
#include "CvTeam.h"
#include "CvGameCoreUtils.h"
#include "ICvDLLUserInterface.h"
#include "CvInfos.h"
#include "CvImprovementClasses.h"
#include "CvAStar.h"
#include "CvMinorCivAI.h"
#include "CvDiplomacyAI.h"
#include "CvEnumSerialization.h"
#include "CvNotifications.h"
#include "FMemoryStream.h"
#include <set>
#include "FStlContainerSerialization.h"
#include "CvUnitMission.h"
#include "CvUnitCombat.h"
#include "CvEconomicAI.h"
#include "CvAchievementUnlocker.h"
#include "CvMilitaryAI.h"
#include "CvTypes.h"

#include "CvDllPlot.h"
#include "CvDllUnit.h"
#include "CvDllCity.h"
#include "CvGameQueries.h"
#include "CvBarbarians.h"

#if !defined(FINAL_RELEASE)
#include <sstream>

// If defined, various operations related to the movement of units will be logged.
//#define LOG_UNIT_MOVES

#endif

#if defined(LOG_UNIT_MOVES)
#define LOG_UNIT_MOVES_MESSAGE(x)		gDLL->netMessageDebugLog(x)
#define LOG_UNIT_MOVES_MESSAGE_OSTR(x)	{ std::ostringstream str; str << x; gDLL->netMessageDebugLog(str.str()); }
#else
#define LOG_UNIT_MOVES_MESSAGE(x)		((void)0)
#define LOG_UNIT_MOVES_MESSAGE_OSTR(x)	((void)0)
#endif

// Come back to this
#include "LintFree.h"

// for statistics
int saiTaskWhenKilled[100] = {0};

namespace FSerialization
{
std::set<CvUnit*> unitsToCheck;
void SyncUnits()
{
	if(GC.getGame().isNetworkMultiPlayer())
	{
		PlayerTypes authoritativePlayer = GC.getGame().getActivePlayer();

		std::set<CvUnit*>::const_iterator i;
		for(i = unitsToCheck.begin(); i != unitsToCheck.end(); ++i)
		{
			const CvUnit* unit = *i;

			if(unit)
			{
				const CvPlayer& player = GET_PLAYER(unit->getOwner());
				if(unit->getOwner() == authoritativePlayer || (gDLL->IsHost() && !player.isHuman() && player.isAlive()))
				{
					const FAutoArchive& archive = unit->getSyncArchive();
					if(archive.hasDeltas())
					{
						FMemoryStream memoryStream;
						std::vector<std::pair<std::string, std::string> > callStacks;
						archive.saveDelta(memoryStream, callStacks);
						gDLL->sendUnitSyncCheck(unit->getOwner(), unit->GetID(), memoryStream, callStacks);
					}
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
// clears ALL deltas for ALL units
void ClearUnitDeltas()
{
	std::set<CvUnit*>::iterator i;
	for(i = unitsToCheck.begin(); i != unitsToCheck.end(); ++i)
	{
		CvUnit* unit = *i;

		if(unit)
		{
			FAutoArchive& archive = unit->getSyncArchive();
			archive.clearDelta();
		}
	}
}
}

bool s_dispatchingNetMessage = false;

OBJECT_VALIDATE_DEFINITION(CvUnit)

//	--------------------------------------------------------------------------------
// Public Functions...
CvUnit::CvUnit() :
	m_syncArchive(*this)
	, m_iID("CvUnit::m_iID", m_syncArchive)
	, m_iHotKeyNumber("CvUnit::m_iHotKeyNumber", m_syncArchive)
	, m_iX("CvUnit::m_iX", m_syncArchive, true)
	, m_iY("CvUnit::m_iY", m_syncArchive, true)
	, m_iLastMoveTurn("CvUnit::m_iLastMoveTurn", m_syncArchive)
	, m_iCycleOrder(-1)
	, m_iDeployFromOperationTurn("CvUnit::DeployFromOperationTurn", m_syncArchive)
	, m_iReconX("CvUnit::m_iReconX", m_syncArchive)
	, m_iReconY("CvUnit::m_iReconY", m_syncArchive)
	, m_iReconCount("CvUnit::m_iReconCount", m_syncArchive)
	, m_iGameTurnCreated("CvUnit::m_iGameTurnCreated", m_syncArchive)
	, m_iDamage("CvUnit::m_iDamage", m_syncArchive, true)
	, m_iMoves("CvUnit::m_iMoves", m_syncArchive, true)
	, m_bImmobile("CvUnit::m_bImmobile", m_syncArchive)
	, m_iExperience("CvUnit::m_iExperience", m_syncArchive)
	, m_iLevel("CvUnit::m_iLevel", m_syncArchive)
	, m_iCargo("CvUnit::m_iCargo", m_syncArchive)
	, m_iCargoCapacity("CvUnit::m_iCargoCapacity", m_syncArchive)
	, m_iAttackPlotX("CvUnit::m_iAttackPlotX", m_syncArchive)
	, m_iAttackPlotY("CvUnit::m_iAttackPlotY", m_syncArchive)
	, m_iCombatTimer("CvUnit::m_iCombatTimer", m_syncArchive)
	, m_iCombatFirstStrikes("CvUnit::m_iCombatFirstStrikes", m_syncArchive)
	, m_iCombatDamage("CvUnit::m_iCombatDamage", m_syncArchive)
	, m_iFortifyTurns("CvUnit::m_iFortifyTurns", m_syncArchive, true)
	, m_bFortifiedThisTurn("CvUnit::m_bFortifiedThisTurn", m_syncArchive)
	, m_iBlitzCount("CvUnit::m_iBlitzCount", m_syncArchive)
	, m_iAmphibCount("CvUnit::m_iAmphibCount", m_syncArchive)
	, m_iRiverCrossingNoPenaltyCount("CvUnit::m_iRiverCrossingNoPenaltyCount", m_syncArchive)
	, m_iEnemyRouteCount("CvUnit::m_iEnemyRouteCount", m_syncArchive)
	, m_iRivalTerritoryCount("CvUnit::m_iRivalTerritoryCount", m_syncArchive)
	, m_iMustSetUpToRangedAttackCount("CvUnit::m_iMustSetUpToRangedAttackCount", m_syncArchive)
	, m_iRangeAttackIgnoreLOSCount("CvUnit::m_iRangeAttackIgnoreLOSCount", m_syncArchive)
	, m_iCityAttackOnlyCount("CvUnit::m_iCityAttackOnlyCount", m_syncArchive)
	, m_iCaptureDefeatedEnemyCount("CvUnit::m_iCaptureDefeatedEnemyCount", m_syncArchive)
#if defined(MOD_BALANCE_CORE)
	, m_iCannotBeCapturedCount("CvUnit::m_iCannotBeCapturedCount", m_syncArchive)
	, m_iForcedDamage("CvUnit::m_iForcedDamage", m_syncArchive)
	, m_iChangeDamage("CvUnit::m_iForcedDamage", m_syncArchive)
	, m_PromotionDuration("CvUnit::m_PromotionDuration", m_syncArchive)
	, m_TurnPromotionGained("CvUnit::m_TurnPromotionGained", m_syncArchive)
#endif

	, m_iRangedSupportFireCount("CvUnit::m_iRangedSupportFireCount", m_syncArchive)
	, m_iAlwaysHealCount("CvUnit::m_iAlwaysHealCount", m_syncArchive)
	, m_iHealOutsideFriendlyCount("CvUnit::m_iHealOutsideFriendlyCount", m_syncArchive)
	, m_iHillsDoubleMoveCount("CvUnit::m_iHillsDoubleMoveCount", m_syncArchive)
	, m_iImmuneToFirstStrikesCount("CvUnit::m_iImmuneToFirstStrikesCount", m_syncArchive)
	, m_iExtraVisibilityRange("CvUnit::m_iExtraVisibilityRange", m_syncArchive)
#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
	, m_iExtraReconRange("CvUnit::m_iExtraReconRange", m_syncArchive)
#endif
	, m_iExtraMoves("CvUnit::m_iExtraMoves", m_syncArchive)
	, m_iExtraMoveDiscount("CvUnit::m_iExtraMoveDiscount", m_syncArchive)
	, m_iExtraRange("CvUnit::m_iExtraRange", m_syncArchive)
	, m_iExtraIntercept("CvUnit::m_iExtraIntercept", m_syncArchive)
	, m_iExtraEvasion("CvUnit::m_iExtraEvasion", m_syncArchive)
	, m_iExtraFirstStrikes("CvUnit::m_iExtraFirstStrikes", m_syncArchive)
	, m_iExtraChanceFirstStrikes("CvUnit::m_iExtraChanceFirstStrikes", m_syncArchive)
	, m_iExtraWithdrawal("CvUnit::m_iExtraWithdrawal", m_syncArchive)
	, m_iExtraEnemyHeal("CvUnit::m_iExtraEnemyHeal", m_syncArchive)
	, m_iExtraNeutralHeal("CvUnit::m_iExtraNeutralHeal", m_syncArchive)
	, m_iExtraFriendlyHeal("CvUnit::m_iExtraFriendlyHeal", m_syncArchive)
	, m_iSameTileHeal("CvUnit::m_iSameTileHeal", m_syncArchive)
	, m_iAdjacentTileHeal("CvUnit::m_iAdjacentTileHeal", m_syncArchive)
	, m_iEnemyDamageChance("CvUnit::m_iEnemyDamageChance", m_syncArchive)
	, m_iNeutralDamageChance("CvUnit::m_iNeutralDamageChance", m_syncArchive)
	, m_iEnemyDamage("CvUnit::m_iEnemyDamage", m_syncArchive)
	, m_iNeutralDamage("CvUnit::m_iNeutralDamage", m_syncArchive)
	, m_iNearbyEnemyCombatMod("CvUnit::m_iNearbyEnemyCombatMod", m_syncArchive)
	, m_iNearbyEnemyCombatRange("CvUnit::m_iNearbyEnemyCombatRange", m_syncArchive)
	, m_iSapperCount("CvUnit::m_iSapperCount", m_syncArchive)
	, m_iCanHeavyCharge("CvUnit::m_iCanHeavyCharge", m_syncArchive)
	, m_iNumExoticGoods("CvUnit::m_iNumExoticGoods", m_syncArchive)
	, m_iAdjacentModifier("CvUnit::m_iAdjacentModifier", m_syncArchive)
	, m_iRangedAttackModifier("CvUnit::m_iRangedAttackModifier", m_syncArchive)
	, m_iInterceptionCombatModifier("CvUnit::m_iInterceptionCombatModifier", m_syncArchive)
	, m_iInterceptionDefenseDamageModifier("CvUnit::m_iInterceptionDefenseDamageModifier", m_syncArchive)
	, m_iAirSweepCombatModifier("CvUnit::m_iAirSweepCombatModifier", m_syncArchive)
	, m_iAttackModifier("CvUnit::m_iAttackModifier", m_syncArchive)
	, m_iDefenseModifier("CvUnit::m_iDefenseModifier", m_syncArchive)
	, m_iExtraCombatPercent("CvUnit::m_iExtraCombatPercent", m_syncArchive)
	, m_iExtraCityAttackPercent("CvUnit::m_iExtraCityAttackPercent", m_syncArchive)
	, m_iExtraCityDefensePercent("CvUnit::m_iExtraCityDefensePercent", m_syncArchive)
	, m_iExtraRangedDefenseModifier("CvUnit::m_iExtraRangedDefenseModifier", m_syncArchive)
	, m_iExtraHillsAttackPercent("CvUnit::m_iExtraHillsAttackPercent", m_syncArchive)
	, m_iExtraHillsDefensePercent("CvUnit::m_iExtraHillsDefensePercent", m_syncArchive)
	, m_iExtraOpenAttackPercent("CvUnit::m_iExtraOpenAttackPercent", m_syncArchive)
	, m_iExtraOpenRangedAttackMod("CvUnit::m_iExtraOpenRangedAttackMod", m_syncArchive)
	, m_iExtraRoughAttackPercent("CvUnit::m_iExtraRoughAttackPercent", m_syncArchive)
	, m_iExtraRoughRangedAttackMod("CvUnit::m_iExtraRoughRangedAttackMod", m_syncArchive)
	, m_iExtraAttackFortifiedMod("CvUnit::m_iExtraAttackFortifiedMod", m_syncArchive)
	, m_iExtraAttackWoundedMod("CvUnit::m_iExtraAttackWoundedMod", m_syncArchive)
	, m_iFlankAttackModifier("CvUnit::m_iFlankAttackModifier", m_syncArchive)
	, m_iExtraOpenDefensePercent("CvUnit::m_iExtraOpenDefensePercent", m_syncArchive)
	, m_iExtraRoughDefensePercent("CvUnit::m_iExtraRoughDefensePercent", m_syncArchive)
	, m_iPillageChange("CvUnit::m_iPillageChange", m_syncArchive)
	, m_iUpgradeDiscount("CvUnit::m_iUpgradeDiscount", m_syncArchive)
	, m_iExperiencePercent("CvUnit::m_iExperiencePercent", m_syncArchive)
	, m_iDropRange("CvUnit::m_iDropRange", m_syncArchive)
	, m_iAirSweepCapableCount("CvUnit::m_iAirSweepCapableCount", m_syncArchive)
	, m_iExtraNavalMoves("CvUnit::m_iExtraNavalMoves", m_syncArchive)
	, m_iKamikazePercent("CvUnit::m_iKamikazePercent", m_syncArchive)
	, m_iBaseCombat("CvUnit::m_iBaseCombat", m_syncArchive)
#if defined(MOD_API_EXTENSIONS)
	, m_iBaseRangedCombat("CvUnit::m_iBaseRangedCombat", m_syncArchive)
#endif
	, m_eFacingDirection("CvUnit::m_eFacingDirection", m_syncArchive, true)
	, m_iArmyId("CvUnit::m_iArmyId", m_syncArchive)
	, m_iIgnoreTerrainCostCount("CvUnit::m_iIgnoreTerrainCostCount", m_syncArchive)
#if defined(MOD_API_PLOT_BASED_DAMAGE)
	, m_iIgnoreTerrainDamageCount("CvUnit::m_iIgnoreTerrainDamageCount", m_syncArchive)
	, m_iIgnoreFeatureDamageCount("CvUnit::m_iIgnoreFeatureDamageCount", m_syncArchive)
	, m_iExtraTerrainDamageCount("CvUnit::m_iExtraTerrainDamageCount", m_syncArchive)
	, m_iExtraFeatureDamageCount("CvUnit::m_iExtraFeatureDamageCount", m_syncArchive)
#endif
#if defined(MOD_PROMOTIONS_IMPROVEMENT_BONUS)
	, m_iNearbyImprovementCombatBonus("CvUnit::m_iNearbyImprovementCombatBonus", m_syncArchive)
	, m_iNearbyImprovementBonusRange("CvUnit::m_iNearbyImprovementBonusRange", m_syncArchive)
	, m_eCombatBonusImprovement("CvUnit::m_eCombatBonusImprovement", m_syncArchive)
#endif
#if defined(MOD_PROMOTIONS_CROSS_MOUNTAINS)
	, m_iCanCrossMountainsCount("CvUnit::m_iCanCrossMountainsCount", m_syncArchive)
#endif
#if defined(MOD_PROMOTIONS_CROSS_OCEANS)
	, m_iCanCrossOceansCount("CvUnit::m_iCanCrossOceansCount", m_syncArchive)
#endif
#if defined(MOD_PROMOTIONS_CROSS_ICE)
	, m_iCanCrossIceCount("CvUnit::m_iCanCrossIceCount", m_syncArchive)
#endif
#if defined(MOD_BALANCE_CORE)
	, m_iNumTilesRevealedThisTurn("CvUnit::m_iNumTilesRevealedThisTurn", m_syncArchive)
#endif
	, m_iRoughTerrainEndsTurnCount("CvUnit::m_iRoughTerrainEndsTurnCount", m_syncArchive)
	, m_iEmbarkAbilityCount("CvUnit::m_iEmbarkAbilityCount", m_syncArchive)
	, m_iHoveringUnitCount("CvUnit::m_iHoveringUnitCount", m_syncArchive)
	, m_iFlatMovementCostCount("CvUnit::m_iFlatMovementCostCount", m_syncArchive)
	, m_iCanMoveImpassableCount("CvUnit::m_iCanMoveImpassableCount", m_syncArchive)
	, m_iOnlyDefensiveCount("CvUnit::m_iOnlyDefensiveCount", m_syncArchive)
	, m_iNoDefensiveBonusCount("CvUnit::m_iNoDefensiveBonusCount", m_syncArchive)
	, m_iNoCaptureCount("CvUnit::m_iNoCaptureCount", m_syncArchive)
	, m_iNukeImmuneCount("CvUnit::m_iNukeImmuneCount", m_syncArchive)
	, m_iHiddenNationalityCount("CvUnit::m_iHiddenNationalityCount", m_syncArchive)
	, m_iAlwaysHostileCount("CvUnit::m_iAlwaysHostileCount", m_syncArchive)
	, m_iNoRevealMapCount("CvUnit::m_iNoRevealMapCount", m_syncArchive)
	, m_iCanMoveAllTerrainCount("CvUnit::m_iCanMoveAllTerrainCount", m_syncArchive)
	, m_iCanMoveAfterAttackingCount("CvUnit::m_iCanMoveAfterAttackingCount", m_syncArchive)
	, m_iFreePillageMoveCount("CvUnit::m_iFreePillageMoveCount", m_syncArchive)
	, m_iHealOnPillageCount("CvUnit::m_iHealOnPillageCount", m_syncArchive)
	, m_iHPHealedIfDefeatEnemy("CvUnit::m_iHPHealedIfDefeatEnemy", m_syncArchive)
	, m_iGoldenAgeValueFromKills("CvUnit::m_iGoldenAgeValueFromKills", m_syncArchive)
	, m_iHealIfDefeatExcludeBarbariansCount("CvUnit::m_iHealIfDefeatExcludeBarbariansCount", m_syncArchive)
	, m_iTacticalAIPlotX("CvUnit::m_iTacticalAIPlotX", m_syncArchive)
	, m_iTacticalAIPlotY("CvUnit::m_iTacticalAIPlotY", m_syncArchive)
	, m_iGarrisonCityID("CvUnit::m_iGarrisonCityID", m_syncArchive)
	, m_iFlags("CvUnit::m_iFlags", m_syncArchive)
	, m_iNumAttacks("CvUnit::m_iNumAttacks", m_syncArchive)
	, m_iAttacksMade("CvUnit::m_iAttacksMade", m_syncArchive)
	, m_iGreatGeneralCount("CvUnit::m_iGreatGeneralCount", m_syncArchive)
	, m_iGreatAdmiralCount("CvUnit::m_iGreatAdmiralCount", m_syncArchive)
	, m_iGreatGeneralModifier("CvUnit::m_iGreatGeneralModifier", m_syncArchive)
	, m_iGreatGeneralReceivesMovementCount("CvUnit::m_iGreatGeneralReceivesMovementCount", m_syncArchive)
	, m_iGreatGeneralCombatModifier("CvUnit::m_iGreatGeneralCombatModifier", m_syncArchive)
	, m_iIgnoreGreatGeneralBenefit("CvUnit::m_iIgnoreGreatGeneralBenefit", m_syncArchive)
	, m_iIgnoreZOC("CvUnit::m_iIgnoreZOC", m_syncArchive)
	, m_iFriendlyLandsModifier("CvUnit::m_iFriendlyLandsModifier", m_syncArchive)
	, m_iFriendlyLandsAttackModifier("CvUnit::m_iFriendlyLandsAttackModifier", m_syncArchive)
	, m_iOutsideFriendlyLandsModifier("CvUnit::m_iOutsideFriendlyLandsModifier", m_syncArchive)
	, m_iNumInterceptions("CvUnit::m_iNumInterceptions", m_syncArchive)
	, m_iMadeInterceptionCount("CvUnit::m_iMadeInterceptionCount", m_syncArchive)
	, m_iEverSelectedCount("CvUnit::m_iEverSelectedCount", m_syncArchive)
	, m_bIgnoreDangerWakeup("CvUnit::m_bIgnoreDangerWakeup", m_syncArchive, false, false)
	, m_iEmbarkedAllWaterCount("CvUnit::m_iEmbarkedAllWaterCount", m_syncArchive)
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
	, m_iEmbarkedDeepWaterCount("CvUnit::m_iEmbarkedDeepWaterCount", m_syncArchive)
#endif
	, m_iEmbarkExtraVisibility("CvUnit::m_iEmbarkExtraVisibility", m_syncArchive)
	, m_iEmbarkDefensiveModifier("CvUnit::m_iEmbarkDefensiveModifier", m_syncArchive)
	, m_iCapitalDefenseModifier("CvUnit::m_iCapitalDefenseModifier", m_syncArchive)
	, m_iCapitalDefenseFalloff("CvUnit::m_iCapitalDefenseFalloff", m_syncArchive)
	, m_iCityAttackPlunderModifier("CvUnit::m_iCityAttackPlunderModifier", m_syncArchive)
	, m_iReligiousStrengthLossRivalTerritory("CvUnit::m_iReligiousStrengthLossRivalTerritory", m_syncArchive)
	, m_iTradeMissionInfluenceModifier("CvUnit::m_iTradeMissionInfluenceModifier", m_syncArchive)
	, m_iTradeMissionGoldModifier("CvUnit::m_iTradeMissionGoldModifier", m_syncArchive)
#if defined(MOD_PROMOTIONS_UNIT_NAMING)
	, m_strUnitName("")
#endif
	, m_strName("")
	, m_eGreatWork(NO_GREAT_WORK)
	, m_iTourismBlastStrength("CvUnit::m_iTourismBlastStrength", m_syncArchive)
	, m_bPromotionReady("CvUnit::m_bPromotionReady", m_syncArchive)
	, m_bDeathDelay("CvUnit::m_bDeathDelay", m_syncArchive)
	, m_bCombatFocus("CvUnit::m_bCombatFocus", m_syncArchive)
	, m_bInfoBarDirty("CvUnit::m_bInfoBarDirty", m_syncArchive)
	, m_bNotConverting("CvUnit::m_bNotConverting", m_syncArchive)
	, m_bAirCombat("CvUnit::m_bAirCombat", m_syncArchive)
	, m_bSetUpForRangedAttack("CvUnit::m_bSetUpForRangedAttack", m_syncArchive)
	, m_bEmbarked("CvUnit::m_bEmbarked", m_syncArchive)
	, m_bAITurnProcessed("CvUnit::m_bAITurnProcessed", m_syncArchive, false, true)
	, m_eTacticalMove("CvUnit::m_eTacticalMove", m_syncArchive)
	, m_eOwner("CvUnit::m_eOwner", m_syncArchive)
	, m_eOriginalOwner("CvUnit::m_eOriginalOwner", m_syncArchive)
	, m_eCapturingPlayer("CvUnit::m_eCapturingPlayer", m_syncArchive)
	, m_bCapturedAsIs("CvUnit::m_eCapturingPlayer", m_syncArchive, false, false)
	, m_eUnitType("CvUnit::m_eUnitType", m_syncArchive)
	, m_eLeaderUnitType("CvUnit::m_eLeaderUnitType", m_syncArchive)
	, m_eInvisibleType("CvUnit::m_eInvisibleType", m_syncArchive)
	, m_eSeeInvisibleType("CvUnit::m_eSeeInvisibleType", m_syncArchive)
	, m_eGreatPeopleDirectiveType("CvUnit::m_eGreatPeopleDirectiveType", m_syncArchive)
#if defined(MOD_CORE_PER_TURN_DAMAGE)
	, m_iDamageTakenThisTurn("CvUnit::m_iDamageTakenThisTurn", m_syncArchive)
	, m_iDamageTakenLastTurn("CvUnit::m_iDamageTakenLastTurn", m_syncArchive)
#endif
	, m_combatUnit()
	, m_transportUnit()
	, m_extraDomainModifiers()
	, m_strNameIAmNotSupposedToBeUsedAnyMoreBecauseThisShouldNotBeCheckedAndWeNeedToPreserveSaveGameCompatibility("CvUnit::m_strNameIAmNotSupposedToBeUsedAnyMoreBecauseThisShouldNotBeCheckedAndWeNeedToPreserveSaveGameCompatibility", m_syncArchive, "")
	, m_strScriptData("CvUnit::m_szScriptData", m_syncArchive)
	, m_iScenarioData("CvUnit::m_iScenarioData", m_syncArchive)
	, m_terrainDoubleMoveCount("CvUnit::m_terrainDoubleMoveCount", m_syncArchive)
	, m_featureDoubleMoveCount("CvUnit::m_featureDoubleMoveCount", m_syncArchive)
#if defined(MOD_PROMOTIONS_HALF_MOVE)
	, m_terrainHalfMoveCount("CvUnit::m_terrainHalfMoveCount", m_syncArchive)
	, m_featureHalfMoveCount("CvUnit::m_featureHalfMoveCount", m_syncArchive)
#endif
#if defined(MOD_BALANCE_CORE)
	, m_iScienceBlastStrength("CvUnit::m_iScienceBlastStrength", m_syncArchive)
	, m_iCultureBlastStrength("CvUnit::m_iCultureBlastStrength", m_syncArchive)
	, m_terrainDoubleHeal("CvUnit::m_terrainDoubleHeal", m_syncArchive)
	, m_featureDoubleHeal("CvUnit::m_featureDoubleHeal", m_syncArchive)
#endif
	, m_terrainImpassableCount("CvUnit::m_terrainImpassableCount", m_syncArchive)
	, m_featureImpassableCount("CvUnit::m_featureImpassableCount", m_syncArchive)
	, m_extraTerrainAttackPercent("CvUnit::m_extraTerrainAttackPercent", m_syncArchive/*, true*/)
	, m_extraTerrainDefensePercent("CvUnit::m_extraTerrainDefensePercent", m_syncArchive/*, true*/)
	, m_extraFeatureAttackPercent("CvUnit::m_extraFeatureAttackPercent", m_syncArchive/*, true*/)
	, m_extraFeatureDefensePercent("CvUnit::m_extraFeatureDefensePercent", m_syncArchive/*, true*/)
#if defined(MOD_API_UNIFIED_YIELDS)
	, m_yieldFromKills("CvUnit::m_yieldFromKills", m_syncArchive/*, true*/)
	, m_yieldFromBarbarianKills("CvUnit::m_yieldFromBarbarianKills", m_syncArchive/*, true*/)
#endif
	, m_extraUnitCombatModifier("CvUnit::m_extraUnitCombatModifier", m_syncArchive/*, true*/)
	, m_unitClassModifier("CvUnit::m_unitClassModifier", m_syncArchive/*, true*/)
	, m_iMissionTimer("CvUnit::m_iMissionTimer", m_syncArchive)
	, m_iMissionAIX("CvUnit::m_iMissionAIX", m_syncArchive)
	, m_iMissionAIY("CvUnit::m_iMissionAIY", m_syncArchive)
	, m_eMissionAIType("CvUnit::m_eMissionAIType", m_syncArchive)
	, m_missionAIUnit()
	, m_eActivityType("CvUnit::m_eActivityType", m_syncArchive, true)
	, m_eAutomateType("CvUnit::m_eAutomateType", m_syncArchive)
	, m_eUnitAIType("CvUnit::m_eUnitAIType", m_syncArchive)
	, m_thisHandle(this)
	, m_bWaitingForMove(false)
	, m_pReligion(FNEW(CvUnitReligion, c_eCiv5GameplayDLL, 0))
	, m_iMapLayer("CvUnit::m_iMapLayer", m_syncArchive, DEFAULT_UNIT_MAP_LAYER)
	, m_iNumGoodyHutsPopped("CvUnit::m_iNumGoodyHutsPopped", m_syncArchive)
	, m_iLastGameTurnAtFullHealth("CvUnit::m_iLastGameTurnAtFullHealth", m_syncArchive, -1)
{
	initPromotions();
	OBJECT_ALLOCATED
	FSerialization::unitsToCheck.insert(this);
	reset(0, NO_UNIT, NO_PLAYER, true);
}


//	--------------------------------------------------------------------------------
CvUnit::~CvUnit()
{
	m_thisHandle.ignoreDestruction(true);
	FSerialization::unitsToCheck.erase(this);
	if(!gDLL->GetDone() && GC.IsGraphicsInitialized())  // don't need to remove entity when the app is shutting down, or crash can occur
	{
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitDestroyed(pDllUnit.get());
	}

	if (m_iMapLayer != DEFAULT_UNIT_MAP_LAYER)
		GC.getMap().plotManager().RemoveUnit(GetIDInfo(), m_iX, m_iY, m_iMapLayer);

	uninit();

	OBJECT_DESTROYED
}


//	--------------------------------------------------------------------------------
void CvUnit::init(int iID, UnitTypes eUnit, UnitAITypes eUnitAI, PlayerTypes eOwner, int iX, int iY, DirectionTypes eFacingDirection, bool bNoMove, bool bSetupGraphical, int iMapLayer /*= DEFAULT_UNIT_MAP_LAYER*/, int iNumGoodyHutsPopped)
{
	initWithNameOffset(iID, eUnit, -1, eUnitAI, eOwner, iX, iY, eFacingDirection, bNoMove, bSetupGraphical, iMapLayer, iNumGoodyHutsPopped);
#if defined(MOD_BALANCE_CORE)
	if(GC.getGame().getGameTurn() > 0)
	{
		GET_PLAYER(getOwner()).UpdateAreaEffectUnits(false);
	}
#endif
}

// ---------------------------------------------------------------------------------
#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
void CvUnit::initWithSpecificName(int iID, UnitTypes eUnit, const char* strKey, UnitAITypes eUnitAI, PlayerTypes eOwner, int iX, int iY, DirectionTypes eFacingDirection, bool bNoMove, bool bSetupGraphical, int iMapLayer, int iNumGoodyHutsPopped)
{
	VALIDATE_OBJECT
	CvString strBuffer;

	CvAssert(NO_UNIT != eUnit);

	initPromotions();
	m_pReligion->Init();

	//--------------------------------
	// Init saved data
	reset(iID, eUnit, eOwner);

	if(eFacingDirection == NO_DIRECTION)
		m_eFacingDirection = DIRECTION_SOUTHEAST;
	else
		m_eFacingDirection = eFacingDirection;

	// If this is a hovering unit, we must add that promotion before setting XY, or else it'll get the embark promotion (which we don't want)
	PromotionTypes ePromotion;
	for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		if(getUnitInfo().GetFreePromotions(iI))
		{
			ePromotion = (PromotionTypes) iI;

			if(GC.getPromotionInfo(ePromotion)->IsHoveringUnit())
				setHasPromotion(ePromotion, true);
		}
	}

	// Set the layer of the map the unit resides
	m_iMapLayer = iMapLayer;
	
	// Set num goody huts popped
	m_iNumGoodyHutsPopped = iNumGoodyHutsPopped;

	//--------------------------------
	// Init containers

	//--------------------------------
	// Init pre-setup() data
	setXY(iX, iY, false, false, false, false, bNoMove);

	//--------------------------------
	// Init non-saved data

	//--------------------------------
	// Init other game data
	plot()->updateCenterUnit();

	SetGreatWork(NO_GREAT_WORK);


	if(strKey != NULL)
	{
		CvString strName = strKey;
		int iNumNames = getUnitInfo().GetNumUnitNames();
		//Look for units from previous and current eras.
		for (int iI = 0; iI < iNumNames; iI++)
		{
			CvString strOtherName = getUnitInfo().GetUnitNames(iI);
			if(strOtherName == strName)
			{
				setName(strName);
				SetGreatWork(getUnitInfo().GetGreatWorks(iI));
				GC.getGame().addGreatPersonBornName(strName);
				break;
			}
		}
	}
	else
	{
		return;
	}
	setGameTurnCreated(GC.getGame().getGameTurn());

	GC.getGame().incrementUnitCreatedCount(getUnitType());

	CvPlayer& kPlayer = GET_PLAYER(getOwner());

	GC.getGame().incrementUnitClassCreatedCount((UnitClassTypes)(getUnitInfo().GetUnitClassType()));
	GET_TEAM(getTeam()).changeUnitClassCount(((UnitClassTypes)(getUnitInfo().GetUnitClassType())), 1);
	kPlayer.changeUnitClassCount(((UnitClassTypes)(getUnitInfo().GetUnitClassType())), 1);

	// Builder Limit
	if(getUnitInfo().GetWorkRate() > 0 && getUnitInfo().GetDomainType() == DOMAIN_LAND)
	{
		kPlayer.ChangeNumBuilders(1);
	}

	// Units can add Unhappiness
	if(GC.getUnitInfo(getUnitType())->GetUnhappiness() != 0)
	{
		kPlayer.ChangeUnhappinessFromUnits(GC.getUnitInfo(getUnitType())->GetUnhappiness());
	}

	kPlayer.changeExtraUnitCost(getUnitInfo().GetExtraMaintenanceCost());

	// Add Resource Quantity to Used
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		if(getUnitInfo().GetResourceQuantityRequirement(iResourceLoop) > 0)
		{
			kPlayer.changeNumResourceUsed((ResourceTypes) iResourceLoop, GC.getUnitInfo(getUnitType())->GetResourceQuantityRequirement(iResourceLoop));
		}
	}

	if(getUnitInfo().GetNukeDamageLevel() != -1)
	{
		kPlayer.changeNumNukeUnits(1);
	}

	if(getUnitInfo().IsMilitarySupport())
	{
		kPlayer.changeNumMilitaryUnits(1);
	}

	// Free Promotions from Unit XML
	for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		if(getUnitInfo().GetFreePromotions(iI))
		{
			ePromotion = (PromotionTypes) iI;

			if(!GC.getPromotionInfo(ePromotion)->IsHoveringUnit())	// Hovering units handled above
				setHasPromotion(ePromotion, true);
		}
	}

	const UnitCombatTypes unitCombatType = getUnitCombatType();
	if(unitCombatType != NO_UNITCOMBAT)
	{
		// Any free Promotions to apply?
		for(int iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
		{
			const PromotionTypes promotionID = (PromotionTypes)iJ;
			if(kPlayer.GetPlayerTraits()->HasFreePromotionUnitCombat(promotionID, unitCombatType))
			{
				setHasPromotion(promotionID, true);
			}
		}
	}
	const UnitClassTypes unitClassType = getUnitClassType();
	if(unitClassType != NO_UNITCLASS)
	{
		// Any free Promotions to apply?
		for(int iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
		{
			const PromotionTypes promotionID = (PromotionTypes)iJ;
			if(kPlayer.GetPlayerTraits()->HasFreePromotionUnitClass(promotionID, unitClassType))
			{
				setHasPromotion(promotionID, true);
			}
		}
	}

	// Free Promotions from Policies, Techs, etc.
	for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		ePromotion = (PromotionTypes) iI;

		if(kPlayer.IsFreePromotion(ePromotion))
		{
			// Valid Promotion for this Unit?
			if(::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()))
			{
				setHasPromotion(ePromotion, true);
			}

			else if(::IsPromotionValidForCivilianUnitType(ePromotion, getUnitType()))
			{
				setHasPromotion(ePromotion, true);
			}

		}
	}
	
	// Adjacent terrain/feature that provides free promotions?
	CvPlot* pAdjacentPlot;
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(plot()->getX(), plot()->getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			FeatureTypes eFeature = pAdjacentPlot->getFeatureType();
			if(eFeature != NO_FEATURE)
			{
				PromotionTypes ePromotion = (PromotionTypes)GC.getFeatureInfo(eFeature)->getAdjacentSpawnLocationUnitFreePromotion();
				if(ePromotion != NO_PROMOTION)
				{
					CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
					if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
					{
						bool bNoPromotion = false;
						// Check for negating promotions
						if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
						{
							bNoPromotion = true;
						}
						if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
						{
							bNoPromotion = true;
						}
						if(!bNoPromotion)
						{
							for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
							{
								const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
								CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
								if(pkPromotionInfo)
								{
									PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
									// Unit has negation promotion
									if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
									{
										bNoPromotion = true;
										break;
									}
								}
							}
						}
						if(!bNoPromotion)
						{
							setHasPromotion(ePromotion, true);
						}
					}
				}
			}

			// Starting terrain that provides free promotions?
			TerrainTypes eAdjacentTerrain = pAdjacentPlot->getTerrainType();
			if(eAdjacentTerrain != NO_TERRAIN && (eAdjacentTerrain <= TERRAIN_SNOW))
			{
				PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(eAdjacentTerrain)->getAdjacentSpawnLocationUnitFreePromotion();
				if(ePromotion != NO_PROMOTION)
				{
					CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
					if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
					{
						bool bNoPromotion = false;
						// Check for negating promotions
						if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
						{
							bNoPromotion = true;
						}
						if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
						{
							bNoPromotion = true;
						}
						if(!bNoPromotion)
						{
							for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
							{
								const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
								CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
								if(pkPromotionInfo)
								{
									PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
									// Unit has negation promotion
									if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
									{
										bNoPromotion = true;
										break;
									}
								}
							}
						}
						if(!bNoPromotion)
						{
							setHasPromotion(ePromotion, true);
						}
					}
				}
			}
			// Starting terrain that provides free promotions?
			if(pAdjacentPlot->isHills())
			{
				PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_HILL)->getAdjacentSpawnLocationUnitFreePromotion();
				if(ePromotion != NO_PROMOTION)
				{
					CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
					if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
					{
						bool bNoPromotion = false;
						// Check for negating promotions
						if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
						{
							bNoPromotion = true;
						}
						if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
						{
							bNoPromotion = true;
						}
						if(!bNoPromotion)
						{
							for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
							{
								const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
								CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
								if(pkPromotionInfo)
								{
									PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
									// Unit has negation promotion
									if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
									{
										bNoPromotion = true;
										break;
									}
								}
							}
						}
						if(!bNoPromotion)
						{
							setHasPromotion(ePromotion, true);
						}
					}
				}
			}
			if(pAdjacentPlot->isMountain())
			{
				PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_MOUNTAIN)->getAdjacentSpawnLocationUnitFreePromotion();
				if(ePromotion != NO_PROMOTION)
				{
					CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
					if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
					{
						bool bNoPromotion = false;
						// Check for negating promotions
						if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
						{
							bNoPromotion = true;
						}
						if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
						{
							bNoPromotion = true;
						}
						if(!bNoPromotion)
						{
							for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
							{
								const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
								CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
								if(pkPromotionInfo)
								{
									PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
									// Unit has negation promotion
									if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
									{
										bNoPromotion = true;
										break;
									}
								}
							}
						}
						if(!bNoPromotion)
						{
							setHasPromotion(ePromotion, true);
						}
					}
				}
			}
			if(pAdjacentPlot->isShallowWater() && !pAdjacentPlot->isLake())
			{
				PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_COAST)->getAdjacentSpawnLocationUnitFreePromotion();
				if(ePromotion != NO_PROMOTION)
				{
					CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
					if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
					{
						bool bNoPromotion = false;
						// Check for negating promotions
						if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
						{
							bNoPromotion = true;
						}
						if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
						{
							bNoPromotion = true;
						}
						if(!bNoPromotion)
						{
							for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
							{
								const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
								CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
								if(pkPromotionInfo)
								{
									PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
									// Unit has negation promotion
									if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
									{
										bNoPromotion = true;
										break;
									}
								}
							}
						}
						if(!bNoPromotion)
						{
							setHasPromotion(ePromotion, true);
						}
					}
				}
			}
			if(!pAdjacentPlot->isShallowWater() && !pAdjacentPlot->isLake() && pAdjacentPlot->isWater())
			{
				PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_OCEAN)->getAdjacentSpawnLocationUnitFreePromotion();
				if(ePromotion != NO_PROMOTION)
				{
					CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
					if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
					{
						bool bNoPromotion = false;
						// Check for negating promotions
						if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
						{
							bNoPromotion = true;
						}
						if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
						{
							bNoPromotion = true;
						}
						if(!bNoPromotion)
						{
							for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
							{
								const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
								CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
								if(pkPromotionInfo)
								{
									PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
									// Unit has negation promotion
									if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
									{
										bNoPromotion = true;
										break;
									}
								}
							}
						}
						if(!bNoPromotion)
						{
							setHasPromotion(ePromotion, true);
						}
					}
				}
			}
		}
	}
	// Feature that provides free promotions?
	FeatureTypes eFeature = plot()->getFeatureType();
	if(eFeature != NO_FEATURE)
	{
		PromotionTypes ePromotion = (PromotionTypes)GC.getFeatureInfo(eFeature)->getSpawnLocationUnitFreePromotion();
		if(ePromotion != NO_PROMOTION)
		{
			CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
			if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
			{
				bool bNoPromotion = false;
				// Check for negating promotions
				if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
				{
					bNoPromotion = true;
				}
				if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
				{
					bNoPromotion = true;
				}
				if(!bNoPromotion)
				{
					for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
					{
						const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
						CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
						if(pkPromotionInfo)
						{
							PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
							// Unit has negation promotion
							if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
							{
								bNoPromotion = true;
								break;
							}
						}
					}
				}
				if(!bNoPromotion)
				{
					setHasPromotion(ePromotion, true);
				}
			}
		}
	}
	// Terrain that provides free promotions?
	TerrainTypes eTerrain = plot()->getTerrainType();
	if(eTerrain != NO_TERRAIN && (eTerrain <= TERRAIN_SNOW))
	{
		PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(eTerrain)->getSpawnLocationUnitFreePromotion();
		if(ePromotion != NO_PROMOTION)
		{
			CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
			if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
			{
				bool bNoPromotion = false;
				// Check for negating promotions
				if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
				{
					bNoPromotion = true;
				}
				if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
				{
					bNoPromotion = true;
				}
				if(!bNoPromotion)
				{
					for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
					{
						const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
						CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
						if(pkPromotionInfo)
						{
							PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
							// Unit has negation promotion
							if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
							{
								bNoPromotion = true;
								break;
							}
						}
					}
				}
				if(!bNoPromotion)
				{
					setHasPromotion(ePromotion, true);
				}
			}
		}
	}
	if(plot()->isHills())
	{
		PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_HILL)->getSpawnLocationUnitFreePromotion();
		if(ePromotion != NO_PROMOTION)
		{
			CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
			if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
			{
				bool bNoPromotion = false;
				// Check for negating promotions
				if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
				{
					bNoPromotion = true;
				}
				if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
				{
					bNoPromotion = true;
				}
				if(!bNoPromotion)
				{
					for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
					{
						const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
						CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
						if(pkPromotionInfo)
						{
							PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
							// Unit has negation promotion
							if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
							{
								bNoPromotion = true;
								break;
							}
						}
					}
				}
				if(!bNoPromotion)
				{
					setHasPromotion(ePromotion, true);
				}
			}
		}
	}
	if(plot()->isMountain())
	{
		PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_MOUNTAIN)->getSpawnLocationUnitFreePromotion();
		if(ePromotion != NO_PROMOTION)
		{
			CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
			if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
			{
				bool bNoPromotion = false;
				// Check for negating promotions
				if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
				{
					bNoPromotion = true;
				}
				if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
				{
					bNoPromotion = true;
				}
				if(!bNoPromotion)
				{
					for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
					{
						const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
						CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
						if(pkPromotionInfo)
						{
							PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
							// Unit has negation promotion
							if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
							{
								bNoPromotion = true;
								break;
							}
						}
					}
				}
				if(!bNoPromotion)
				{
					setHasPromotion(ePromotion, true);
				}
			}
		}
	}
	if(plot()->isShallowWater() && !plot()->isLake())
	{
		PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_COAST)->getSpawnLocationUnitFreePromotion();
		if(ePromotion != NO_PROMOTION)
		{
			CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
			if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
			{
				bool bNoPromotion = false;
				// Check for negating promotions
				if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
				{
					bNoPromotion = true;
				}
				if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
				{
					bNoPromotion = true;
				}
				if(!bNoPromotion)
				{
					for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
					{
						const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
						CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
						if(pkPromotionInfo)
						{
							PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
							// Unit has negation promotion
							if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
							{
								bNoPromotion = true;
								break;
							}
						}
					}
				}
				if(!bNoPromotion)
				{
					setHasPromotion(ePromotion, true);
				}
			}
		}
	}
	if(!plot()->isShallowWater() && !plot()->isLake() && plot()->isWater())
	{
		PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_OCEAN)->getSpawnLocationUnitFreePromotion();
		if(ePromotion != NO_PROMOTION)
		{
			CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
			if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
			{
				bool bNoPromotion = false;
				// Check for negating promotions
				if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
				{
					bNoPromotion = true;
				}
				if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
				{
					bNoPromotion = true;
				}
				if(!bNoPromotion)
				{
					for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
					{
						const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
						CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
						if(pkPromotionInfo)
						{
							PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
							// Unit has negation promotion
							if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
							{
								bNoPromotion = true;
								break;
							}
						}
					}
				}
				if(!bNoPromotion)
				{
					setHasPromotion(ePromotion, true);
				}
			}
		}
	}
	// Give embark promotion for free?
	if(GET_TEAM(getTeam()).canEmbark() || kPlayer.GetPlayerTraits()->IsEmbarkedAllWater())
	{
		PromotionTypes ePromotionEmbarkation = kPlayer.GetEmbarkationPromotion();

		bool bGivePromotion = false;

		// Civilians get it for free
		if(getDomainType() == DOMAIN_LAND)
		{
			if(!IsCombatUnit())
				bGivePromotion = true;
		}

		// Can the unit get this? (handles water units and such)
		if(!bGivePromotion && ::IsPromotionValidForUnitCombatType(ePromotionEmbarkation, getUnitType()))
			bGivePromotion = true;

		// Some case that gives us the promotion?
		if(bGivePromotion)
			setHasPromotion(ePromotionEmbarkation, true);
	}
	
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
	// Flip Deep Water Embarkation to Defensive Deep Water Embarkation if the player has the required trait
	if (IsEmbarkDeepWater()) {
		setHasPromotion((PromotionTypes)GC.getPROMOTION_DEEPWATER_EMBARKATION(), false);
		setHasPromotion(kPlayer.GetDeepWaterEmbarkationPromotion(), true);
	}
#endif

	// Strip off Ocean Impassable promotion because of trait?
	if(kPlayer.GetPlayerTraits()->IsEmbarkedAllWater())
	{
		PromotionTypes ePromotionOceanImpassable = (PromotionTypes)GC.getPROMOTION_OCEAN_IMPASSABLE();
		if(isHasPromotion(ePromotionOceanImpassable))
		{
			setHasPromotion(ePromotionOceanImpassable, false);
		}
		PromotionTypes ePromotionOceanImpassableUntilAstronomy = (PromotionTypes)GC.getPROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY();
		if(isHasPromotion(ePromotionOceanImpassableUntilAstronomy))
		{
			setHasPromotion(ePromotionOceanImpassableUntilAstronomy, false);
		}
	}

	// Any exotic goods that can be sold? (Portuguese unique unit mission)
	if (getUnitInfo().GetNumExoticGoods() > 0)
	{
		changeNumExoticGoods(getUnitInfo().GetNumExoticGoods());
	}

	// free XP from handicap?
	int iXP = GC.getGame().getHandicapInfo().getAIFreeXP();
	if (iXP && !kPlayer.isHuman() && /*kPlayer.GetID() < MAX_MAJOR_CIVS &&*/ canAcquirePromotionAny())
	{
		changeExperience(iXP);
	}

	// bonus xp in combat from handicap?
	int iXPPercent = GC.getGame().getHandicapInfo().getAIFreeXPPercent();
	if (iXPPercent && !kPlayer.isHuman() && /*kPlayer.GetID() < MAX_MAJOR_CIVS &&*/ canAcquirePromotionAny())
	{
		changeExperiencePercent(iXPPercent);
	}


	// Is this Unit immobile?
	if(getUnitInfo().IsImmobile())
	{
		SetImmobile(true);
	}

	setMoves(maxMoves());

	// Religious unit? If so takes religion from city
	if (getUnitInfo().IsSpreadReligion() || getUnitInfo().IsRemoveHeresy())
	{
		CvCity *pPlotCity = plot()->getPlotCity();
		if (pPlotCity)
		{
			ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
			if (eReligion > RELIGION_PANTHEON)
			{
				GetReligionData()->SetReligion(eReligion);
				GetReligionData()->SetSpreadsLeft(getUnitInfo().GetReligionSpreads() + pPlotCity->GetCityBuildings()->GetMissionaryExtraSpreads());
				GetReligionData()->SetReligiousStrength(getUnitInfo().GetReligiousStrength());
			}
		}
#if defined(MOD_GLOBAL_RELIGIOUS_SETTLERS)
	}
	else if (MOD_GLOBAL_RELIGIOUS_SETTLERS && (getUnitInfo().IsFound() || getUnitInfo().IsFoundAbroad() || getUnitInfo().IsFoundMid() || getUnitInfo().IsFoundLate()))
	{
		ReligionTypes eReligion = RELIGION_PANTHEON;

		CvCity *pPlotCity = plot()->getPlotCity();
		if (pPlotCity)
		{
			CvCityReligions *pCityReligions = pPlotCity->GetCityReligions();

			int totalFollowers = pPlotCity->getPopulation();
			int randFollower = GC.getGame().getJonRandNum(totalFollowers, "Religious Settlers: Picking a random religion for the settler") + 1;

			for (int i = RELIGION_PANTHEON; i < GC.getNumReligionInfos(); i++)
			{
				int theseFollowers = pCityReligions->GetNumFollowers((ReligionTypes) i);

				if (randFollower <= theseFollowers)
				{
					eReligion = (ReligionTypes) i;
					break;
				}

				randFollower = randFollower - theseFollowers;
				if (randFollower < 0) break;
			}
		}

		GetReligionData()->SetReligion(eReligion);
#endif
	}
	if (getUnitInfo().GetOneShotTourism() > 0)
	{
		SetTourismBlastStrength(kPlayer.GetCulture()->GetTourismBlastStrength(getUnitInfo().GetOneShotTourism()));
	}
#if defined(MOD_BALANCE_CORE)
	if (getUnitInfo().GetBaseBeakersTurnsToCount() > 0)
	{
		SetScienceBlastStrength(getDiscoverAmount());
	}
	if (getUnitInfo().GetBaseCultureTurnsToCount() > 0)
	{
		SetCultureBlastStrength(getGivePoliciesCulture());
	}
#endif

	int iTourism = kPlayer.GetPlayerPolicies()->GetTourismFromUnitCreation((UnitClassTypes)(getUnitInfo().GetUnitClassType()));
	if (iTourism > 0)
	{
#if defined(MOD_BALANCE_CORE)
		iTourism *= GC.getGame().getGameSpeedInfo().getTrainPercent();
		iTourism /= 100;
#endif
		kPlayer.GetCulture()->AddTourismAllKnownCivs(iTourism);
	}

	// Recon unit? If so, he sees what's around him
	if(IsRecon())
	{
		setReconPlot(plot());
	}

	if(getDomainType() == DOMAIN_LAND)
	{
		if(IsCombatUnit())
		{
			if((GC.getGame().getBestLandUnit() == NO_UNIT) || (GetBaseCombatStrength() > GC.getGame().getBestLandUnitCombat()))
			{
				GC.getGame().setBestLandUnit(getUnitType());
			}
		}
	}

	// Update UI
	if(getOwner() == GC.getGame().getActivePlayer())
	{
		DLLUI->setDirty(GameData_DIRTY_BIT, true);

		kPlayer.GetUnitCycler().AddUnit( GetID() );
	}

	// Message for World Unit being born
	if(isWorldUnitClass((UnitClassTypes)(getUnitInfo().GetUnitClassType())))
	{
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if(GET_PLAYER((PlayerTypes)iI).isAlive() && GC.getGame().getActivePlayer())
			{
				if(GET_TEAM(getTeam()).isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_SOMEONE_CREATED_UNIT", kPlayer.getNameKey(), getNameKey());
					DLLUI->AddMessage(0, ((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_WONDER_UNIT_BUILD", MESSAGE_TYPE_MAJOR_EVENT, getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"), getX(), getY(), true, true*/);
				}
				else
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_UNKNOWN_CREATED_UNIT", getNameKey());
					DLLUI->AddMessage(0, ((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_WONDER_UNIT_BUILD", MESSAGE_TYPE_MAJOR_EVENT, getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT")*/);
				}
			}
		}

		strBuffer = GetLocalizedText("TXT_KEY_MISC_SOMEONE_CREATED_UNIT", kPlayer.getNameKey(), getNameKey());
		GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getOwner(), strBuffer, getX(), getY());
	}

	// Update City Strength
	CvPlot* pUnitPlot = plot();
	if(pUnitPlot && pUnitPlot->isCity())
	{
		pUnitPlot->getPlotCity()->SetGarrison( pUnitPlot->getBestGarrison( getOwner() ).pointer() );
	}

	m_iArmyId = -1;

	m_eUnitAIType = eUnitAI;

	// Update Unit Production Maintenance
	kPlayer.UpdateUnitProductionMaintenanceMod();

	// Minor Civ quest
	if(!kPlayer.isMinorCiv() && !isBarbarian())
	{
		PlayerTypes eMinor;
		for(int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
		{
			eMinor = (PlayerTypes) iMinorCivLoop;
			if(GET_PLAYER(eMinor).isAlive())
			{
				// Does this Minor want us to spawn a Unit?
				GET_PLAYER(eMinor).GetMinorCivAI()->DoTestActiveQuestsForPlayer(getOwner(), /*bTestComplete*/ true, /*bTestObsolete*/ false, MINOR_CIV_QUEST_GREAT_PERSON);
			}
		}
	}

	if(bSetupGraphical)
		setupGraphical();
		
#if defined(MOD_EVENTS_UNIT_CREATED)
	if (MOD_EVENTS_UNIT_CREATED) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_UnitCreated, getOwner(), GetID(), getUnitType(), getX(), getY());
	}
#endif
}
#endif
//	--------------------------------------------------------------------------------
void CvUnit::initWithNameOffset(int iID, UnitTypes eUnit, int iNameOffset, UnitAITypes eUnitAI, PlayerTypes eOwner, int iX, int iY, DirectionTypes eFacingDirection, bool bNoMove, bool bSetupGraphical, int iMapLayer, int iNumGoodyHutsPopped)
{
	VALIDATE_OBJECT
	CvString strBuffer;
	int iUnitName;
	int iI;

	CvAssert(NO_UNIT != eUnit);

	initPromotions();
	m_pReligion->Init();

	//--------------------------------
	// Init saved data
	reset(iID, eUnit, eOwner);

	if(eFacingDirection == NO_DIRECTION)
		m_eFacingDirection = DIRECTION_SOUTHEAST;
	else
		m_eFacingDirection = eFacingDirection;

	// If this is a hovering unit, we must add that promotion before setting XY, or else it'll get the embark promotion (which we don't want)
	PromotionTypes ePromotion;
	for(iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		if(getUnitInfo().GetFreePromotions(iI))
		{
			ePromotion = (PromotionTypes) iI;

			if(GC.getPromotionInfo(ePromotion)->IsHoveringUnit())
				setHasPromotion(ePromotion, true);
		}
	}

	// Set the layer of the map the unit resides
	m_iMapLayer = iMapLayer;
	
	// Set num goody huts popped
	m_iNumGoodyHutsPopped = iNumGoodyHutsPopped;

	//--------------------------------
	// Init containers

	//--------------------------------
	// Init pre-setup() data
	setXY(iX, iY, false, false, false, false, bNoMove);

	//--------------------------------
	// Init non-saved data

	//--------------------------------
	// Init other game data
	plot()->updateCenterUnit();

	SetGreatWork(NO_GREAT_WORK);
	iUnitName = GC.getGame().getUnitCreatedCount(getUnitType());
	int iNumNames = getUnitInfo().GetNumUnitNames();
#if defined(MOD_BALANCE_CORE)
	std::vector<int> vfPossibleUnits;
#endif
	if(iUnitName < iNumNames)
	{
		if(iNameOffset == -1)
		{
			iNameOffset = GC.getGame().getJonRandNum(iNumNames, "Unit name selection");
		}
#if defined(MOD_BALANCE_CORE)
		CvString strName = NULL;
		//Look for units from previous and current eras.
		for(iI = 0; iI < iNumNames; iI++)
		{
			EraTypes eUnitEra = getUnitInfo().GetGreatPersonEra(iI);
			if((eUnitEra != NO_ERA) && eUnitEra <= GET_PLAYER(getOwner()).GetCurrentEra())
			{
				strName = getUnitInfo().GetUnitNames(iI);
				if(!GC.getGame().isGreatPersonBorn(strName))
				{
					vfPossibleUnits.push_back(iI);
				}
			}
		}
		if(vfPossibleUnits.size() > 0)
		{
			int iRoll = GC.getGame().getJonRandNum(vfPossibleUnits.size(), "Rolling for era-specific unit");
			strName = getUnitInfo().GetUnitNames(vfPossibleUnits[iRoll]);
			setName(strName);
			SetGreatWork(getUnitInfo().GetGreatWorks(vfPossibleUnits[iRoll]));
			GC.getGame().addGreatPersonBornName(strName);
		}
		//None? Let's look to the next era.
		else
		{
			for(iI = 0; iI < iNumNames; iI++)
			{
				EraTypes eUnitEra = getUnitInfo().GetGreatPersonEra(iI);
				if((eUnitEra != NO_ERA) && ((eUnitEra - 1 ) == GET_PLAYER(getOwner()).GetCurrentEra()))
				{
					strName = getUnitInfo().GetUnitNames(iI);
					if(!GC.getGame().isGreatPersonBorn(strName))
					{
						vfPossibleUnits.push_back(iI);
					}
				}
			}
			if(vfPossibleUnits.size() > 0)
			{
				int iRoll = GC.getGame().getJonRandNum(vfPossibleUnits.size(), "Rolling for era-specific unit");
				strName = getUnitInfo().GetUnitNames(vfPossibleUnits[iRoll]);
				setName(strName);
				SetGreatWork(getUnitInfo().GetGreatWorks(vfPossibleUnits[iRoll]));
				GC.getGame().addGreatPersonBornName(strName);
			}
		}
		//If still no valid GPs, do the old random method.
		if(vfPossibleUnits.size() <= 0)
		{
#endif
		for(iI = 0; iI < iNumNames; iI++)
		{
			int iIndex = (iNameOffset + iI) % iNumNames;
			CvString strName = getUnitInfo().GetUnitNames(iIndex);
			
#if defined(MOD_EVENTS_UNIT_DATA)
			if (MOD_EVENTS_UNIT_DATA) {
				if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_UnitCanHaveName, getOwner(), GetID(), iIndex) == GAMEEVENTRETURN_FALSE) {
					continue;
				}
			}
#endif

			if(!GC.getGame().isGreatPersonBorn(strName))
			{
#if defined(MOD_EVENTS_UNIT_DATA)
				if (MOD_EVENTS_UNIT_DATA) {
					if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_UnitCanHaveGreatWork, getOwner(), GetID(), getUnitInfo().GetGreatWorks(iIndex)) == GAMEEVENTRETURN_FALSE) {
						continue;
					}
				}
#endif			
				setName(strName);
				SetGreatWork(getUnitInfo().GetGreatWorks(iIndex));
				GC.getGame().addGreatPersonBornName(strName);
				break;
			}
		}
#if defined(MOD_BALANCE_CORE)
		}
#endif
	}
	setGameTurnCreated(GC.getGame().getGameTurn());

	GC.getGame().incrementUnitCreatedCount(getUnitType());

	CvPlayer& kPlayer = GET_PLAYER(getOwner());

	GC.getGame().incrementUnitClassCreatedCount((UnitClassTypes)(getUnitInfo().GetUnitClassType()));
	GET_TEAM(getTeam()).changeUnitClassCount(((UnitClassTypes)(getUnitInfo().GetUnitClassType())), 1);
	kPlayer.changeUnitClassCount(((UnitClassTypes)(getUnitInfo().GetUnitClassType())), 1);

	// Builder Limit
	if(getUnitInfo().GetWorkRate() > 0 && getUnitInfo().GetDomainType() == DOMAIN_LAND)
	{
		kPlayer.ChangeNumBuilders(1);
	}
#if defined(MOD_BALANCE_CORE_SETTLER_RESET_FOOD)
	if(MOD_BALANCE_CORE_SETTLER_RESET_FOOD && getUnitInfo().IsFound())
	{
		CvCity* pCity = plot()->getPlotCity();
		if(pCity != NULL)
		{
			pCity->setFood(0);
		}
	}
#endif


	// Units can add Unhappiness
	if(GC.getUnitInfo(getUnitType())->GetUnhappiness() != 0)
	{
		kPlayer.ChangeUnhappinessFromUnits(GC.getUnitInfo(getUnitType())->GetUnhappiness());
	}

	kPlayer.changeExtraUnitCost(getUnitInfo().GetExtraMaintenanceCost());

	// Add Resource Quantity to Used
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		if(getUnitInfo().GetResourceQuantityRequirement(iResourceLoop) > 0)
		{
			kPlayer.changeNumResourceUsed((ResourceTypes) iResourceLoop, GC.getUnitInfo(getUnitType())->GetResourceQuantityRequirement(iResourceLoop));
		}
	}

	if(getUnitInfo().GetNukeDamageLevel() != -1)
	{
		kPlayer.changeNumNukeUnits(1);
	}

	if(getUnitInfo().IsMilitarySupport())
	{
		kPlayer.changeNumMilitaryUnits(1);
	}

	// Free Promotions from Unit XML
	for(iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		if(getUnitInfo().GetFreePromotions(iI))
		{
			ePromotion = (PromotionTypes) iI;

			if(!GC.getPromotionInfo(ePromotion)->IsHoveringUnit())	// Hovering units handled above
				setHasPromotion(ePromotion, true);
		}
	}

	const UnitCombatTypes unitCombatType = getUnitCombatType();
	if(unitCombatType != NO_UNITCOMBAT)
	{
		// Any free Promotions to apply?
		for(int iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
		{
			const PromotionTypes promotionID = (PromotionTypes)iJ;
			if(kPlayer.GetPlayerTraits()->HasFreePromotionUnitCombat(promotionID, unitCombatType))
			{
				setHasPromotion(promotionID, true);
			}
		}
	}
#if defined(MOD_BALANCE_CORE)
	const UnitClassTypes unitClassType = getUnitClassType();
	if(unitClassType != NO_UNITCLASS)
	{
		// Any free Promotions to apply?
		for(int iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
		{
			const PromotionTypes promotionID = (PromotionTypes)iJ;
			if(kPlayer.GetPlayerTraits()->HasFreePromotionUnitClass(promotionID, unitClassType))
			{
				setHasPromotion(promotionID, true);
			}
		}
	}
#endif

	// Free Promotions from Policies, Techs, etc.
	for(iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		ePromotion = (PromotionTypes) iI;

		if(kPlayer.IsFreePromotion(ePromotion))
		{
			// Valid Promotion for this Unit?
			if(::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()))
			{
				setHasPromotion(ePromotion, true);
			}

			else if(::IsPromotionValidForCivilianUnitType(ePromotion, getUnitType()))
			{
				setHasPromotion(ePromotion, true);
			}

		}
	}

	// Give embark promotion for free?
	if(GET_TEAM(getTeam()).canEmbark() || kPlayer.GetPlayerTraits()->IsEmbarkedAllWater())
	{
		PromotionTypes ePromotionEmbarkation = kPlayer.GetEmbarkationPromotion();

		bool bGivePromotion = false;

		// Civilians get it for free
		if(getDomainType() == DOMAIN_LAND)
		{
			if(!IsCombatUnit())
				bGivePromotion = true;
		}

		// Can the unit get this? (handles water units and such)
		if(!bGivePromotion && ::IsPromotionValidForUnitCombatType(ePromotionEmbarkation, getUnitType()))
			bGivePromotion = true;

		// Some case that gives us the promotion?
		if(bGivePromotion)
			setHasPromotion(ePromotionEmbarkation, true);
	}
	
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
	// Flip Deep Water Embarkation to Defensive Deep Water Embarkation if the player has the required trait
	if (IsEmbarkDeepWater()) {
		setHasPromotion((PromotionTypes)GC.getPROMOTION_DEEPWATER_EMBARKATION(), false);
		setHasPromotion(kPlayer.GetDeepWaterEmbarkationPromotion(), true);
	}
#endif

	// Strip off Ocean Impassable promotion because of trait?
	if(kPlayer.GetPlayerTraits()->IsEmbarkedAllWater())
	{
		PromotionTypes ePromotionOceanImpassable = (PromotionTypes)GC.getPROMOTION_OCEAN_IMPASSABLE();
		if(isHasPromotion(ePromotionOceanImpassable))
		{
			setHasPromotion(ePromotionOceanImpassable, false);
		}
		PromotionTypes ePromotionOceanImpassableUntilAstronomy = (PromotionTypes)GC.getPROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY();
		if(isHasPromotion(ePromotionOceanImpassableUntilAstronomy))
		{
			setHasPromotion(ePromotionOceanImpassableUntilAstronomy, false);
		}
	}

	// Any exotic goods that can be sold? (Portuguese unique unit mission)
	if (getUnitInfo().GetNumExoticGoods() > 0)
	{
		changeNumExoticGoods(getUnitInfo().GetNumExoticGoods());
	}

	// free XP from handicap?
	int iXP = GC.getGame().getHandicapInfo().getAIFreeXP();
	if (iXP && !kPlayer.isHuman() && /*kPlayer.GetID() < MAX_MAJOR_CIVS &&*/ canAcquirePromotionAny())
	{
		changeExperience(iXP);
	}

	// bonus xp in combat from handicap?
	int iXPPercent = GC.getGame().getHandicapInfo().getAIFreeXPPercent();
	if (iXPPercent && !kPlayer.isHuman() && /*kPlayer.GetID() < MAX_MAJOR_CIVS &&*/ canAcquirePromotionAny())
	{
		changeExperiencePercent(iXPPercent);
	}


	// Is this Unit immobile?
	if(getUnitInfo().IsImmobile())
	{
		SetImmobile(true);
	}

	setMoves(maxMoves());

	// Religious unit? If so takes religion from city
	if (getUnitInfo().IsSpreadReligion() || getUnitInfo().IsRemoveHeresy())
	{
		CvCity *pPlotCity = plot()->getPlotCity();
		if (pPlotCity)
		{
#if defined(MOD_BALANCE_CORE)
			if(pPlotCity->GetNationalMissionaries() > 0)
			{
				//Only fires once.
				ReligionTypes eNationalReligion = GET_PLAYER(pPlotCity->getOwner()).GetReligions()->GetReligionCreatedByPlayer(false);
				if(eNationalReligion == NO_RELIGION)
				{
					eNationalReligion = GET_PLAYER(pPlotCity->getOwner()).GetReligions()->GetReligionInMostCities();
				}
				if (eNationalReligion != NO_RELIGION)
				{
					GetReligionData()->SetReligion(eNationalReligion);
					GetReligionData()->SetSpreadsLeft(getUnitInfo().GetReligionSpreads() + pPlotCity->GetCityBuildings()->GetMissionaryExtraSpreads());
					GetReligionData()->SetReligiousStrength(getUnitInfo().GetReligiousStrength());
				}
				else
				{
					kill(true);
					pPlotCity->SetNationalMissionaries(0);
					return;
				}
				pPlotCity->SetNationalMissionaries(0);
			}
			else
			{
#endif
			ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
			if (eReligion > RELIGION_PANTHEON)
			{
				GetReligionData()->SetReligion(eReligion);
				GetReligionData()->SetSpreadsLeft(getUnitInfo().GetReligionSpreads() + pPlotCity->GetCityBuildings()->GetMissionaryExtraSpreads());
				GetReligionData()->SetReligiousStrength(getUnitInfo().GetReligiousStrength());
			}
#if defined(MOD_BALANCE_CORE)
			}
#endif
		}
#if defined(MOD_GLOBAL_RELIGIOUS_SETTLERS) && defined(MOD_BALANCE_CORE_SETTLER_ADVANCED)
	}
	else if (MOD_GLOBAL_RELIGIOUS_SETTLERS && MOD_BALANCE_CORE_SETTLER_ADVANCED && (getUnitInfo().IsFound() || getUnitInfo().IsFoundAbroad() || getUnitInfo().IsFoundMid() || getUnitInfo().IsFoundLate()))
	{
		ReligionTypes eReligion = RELIGION_PANTHEON;

		CvCity *pPlotCity = plot()->getPlotCity();
		if (pPlotCity)
		{
			CvCityReligions *pCityReligions = pPlotCity->GetCityReligions();

			int totalFollowers = pPlotCity->getPopulation();
			int randFollower = GC.getGame().getJonRandNum(totalFollowers, "Religious Settlers: Picking a random religion for the settler") + 1;

			for (int i = RELIGION_PANTHEON; i < GC.getNumReligionInfos(); i++)
			{
				int theseFollowers = pCityReligions->GetNumFollowers((ReligionTypes) i);

				if (randFollower <= theseFollowers)
				{
					eReligion = (ReligionTypes) i;
					break;
				}

				randFollower = randFollower - theseFollowers;
				if (randFollower < 0) break;
			}
		}

		GetReligionData()->SetReligion(eReligion);
#endif
#if defined(MOD_GLOBAL_RELIGIOUS_SETTLERS)
	}
	else if (MOD_GLOBAL_RELIGIOUS_SETTLERS && (getUnitInfo().IsFound() || getUnitInfo().IsFoundAbroad()))
	{
		ReligionTypes eReligion = RELIGION_PANTHEON;

		CvCity *pPlotCity = plot()->getPlotCity();
		if (pPlotCity)
		{
			CvCityReligions *pCityReligions = pPlotCity->GetCityReligions();

			int totalFollowers = pPlotCity->getPopulation();
			int randFollower = GC.getGame().getJonRandNum(totalFollowers, "Religious Settlers: Picking a random religion for the settler") + 1;

			for (int i = RELIGION_PANTHEON; i < GC.getNumReligionInfos(); i++)
			{
				int theseFollowers = pCityReligions->GetNumFollowers((ReligionTypes) i);

				if (randFollower <= theseFollowers)
				{
					eReligion = (ReligionTypes) i;
					break;
				}

				randFollower = randFollower - theseFollowers;
				if (randFollower < 0) break;
			}
		}

		GetReligionData()->SetReligion(eReligion);
#endif
	}
	if (getUnitInfo().GetOneShotTourism() > 0)
	{
		SetTourismBlastStrength(kPlayer.GetCulture()->GetTourismBlastStrength(getUnitInfo().GetOneShotTourism()));
	}
#if defined(MOD_BALANCE_CORE)
	if (getUnitInfo().GetBaseBeakersTurnsToCount() > 0)
	{
		SetScienceBlastStrength(getDiscoverAmount());
	}
	if (getUnitInfo().GetBaseCultureTurnsToCount() > 0)
	{
		SetCultureBlastStrength(getGivePoliciesCulture());
	}
#endif
	int iTourism = kPlayer.GetPlayerPolicies()->GetTourismFromUnitCreation((UnitClassTypes)(getUnitInfo().GetUnitClassType()));
	if (iTourism > 0)
	{
		kPlayer.GetCulture()->AddTourismAllKnownCivs(iTourism);
	}

	// Recon unit? If so, he sees what's around him
	if(IsRecon())
	{
		setReconPlot(plot());
	}

	if(getDomainType() == DOMAIN_LAND)
	{
		if(IsCombatUnit())
		{
			if((GC.getGame().getBestLandUnit() == NO_UNIT) || (GetBaseCombatStrength() > GC.getGame().getBestLandUnitCombat()))
			{
				GC.getGame().setBestLandUnit(getUnitType());
			}
		}
	}

	// Update UI
	if(getOwner() == GC.getGame().getActivePlayer())
	{
		DLLUI->setDirty(GameData_DIRTY_BIT, true);

		kPlayer.GetUnitCycler().AddUnit( GetID() );
	}

	// Message for World Unit being born
	if(isWorldUnitClass((UnitClassTypes)(getUnitInfo().GetUnitClassType())))
	{
		for(iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if(GET_PLAYER((PlayerTypes)iI).isAlive() && GC.getGame().getActivePlayer())
			{
				if(GET_TEAM(getTeam()).isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_SOMEONE_CREATED_UNIT", kPlayer.getNameKey(), getNameKey());
					DLLUI->AddMessage(0, ((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_WONDER_UNIT_BUILD", MESSAGE_TYPE_MAJOR_EVENT, getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"), getX(), getY(), true, true*/);
				}
				else
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_UNKNOWN_CREATED_UNIT", getNameKey());
					DLLUI->AddMessage(0, ((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_WONDER_UNIT_BUILD", MESSAGE_TYPE_MAJOR_EVENT, getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT")*/);
				}
			}
		}

		strBuffer = GetLocalizedText("TXT_KEY_MISC_SOMEONE_CREATED_UNIT", kPlayer.getNameKey(), getNameKey());
		GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getOwner(), strBuffer, getX(), getY());
	}

	// Update City Strength
	CvPlot* pUnitPlot = plot();
	if(pUnitPlot && pUnitPlot->isCity())
	{
		pUnitPlot->getPlotCity()->SetGarrison( pUnitPlot->getBestGarrison( getOwner() ).pointer() );
	}

#if defined(MOD_BALANCE_CORE)
	// Adjacent terrain/feature that provides free promotions?
	CvPlot* pAdjacentPlot;
	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(plot()->getX(), plot()->getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			FeatureTypes eFeature = pAdjacentPlot->getFeatureType();
			if(eFeature != NO_FEATURE)
			{
				PromotionTypes ePromotion = (PromotionTypes)GC.getFeatureInfo(eFeature)->getAdjacentSpawnLocationUnitFreePromotion();
				if(ePromotion != NO_PROMOTION)
				{
					CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
					if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
					{
						bool bNoPromotion = false;
						// Check for negating promotions
						if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
						{
							bNoPromotion = true;
						}
						if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
						{
							bNoPromotion = true;
						}
						if(!bNoPromotion)
						{
							for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
							{
								const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
								CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
								if(pkPromotionInfo)
								{
									PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
									// Unit has negation promotion
									if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
									{
										bNoPromotion = true;
										break;
									}
								}
							}
						}
						if(!bNoPromotion)
						{
							setHasPromotion(ePromotion, true);
						}
					}
				}
			}

			// Starting terrain that provides free promotions?
			TerrainTypes eAdjacentTerrain = pAdjacentPlot->getTerrainType();
			if(eAdjacentTerrain != NO_TERRAIN && (eAdjacentTerrain <= TERRAIN_SNOW))
			{
				PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(eAdjacentTerrain)->getAdjacentSpawnLocationUnitFreePromotion();
				if(ePromotion != NO_PROMOTION)
				{
					CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
					if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
					{
						bool bNoPromotion = false;
						// Check for negating promotions
						if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
						{
							bNoPromotion = true;
						}
						if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
						{
							bNoPromotion = true;
						}
						if(!bNoPromotion)
						{
							for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
							{
								const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
								CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
								if(pkPromotionInfo)
								{
									PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
									// Unit has negation promotion
									if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
									{
										bNoPromotion = true;
										break;
									}
								}
							}
						}
						if(!bNoPromotion)
						{
							setHasPromotion(ePromotion, true);
						}
					}
				}
			}
			// Starting terrain that provides free promotions?
			if(pAdjacentPlot->isHills())
			{
				PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_HILL)->getAdjacentSpawnLocationUnitFreePromotion();
				if(ePromotion != NO_PROMOTION)
				{
					CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
					if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
					{
						bool bNoPromotion = false;
						// Check for negating promotions
						if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
						{
							bNoPromotion = true;
						}
						if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
						{
							bNoPromotion = true;
						}
						if(!bNoPromotion)
						{
							for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
							{
								const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
								CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
								if(pkPromotionInfo)
								{
									PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
									// Unit has negation promotion
									if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
									{
										bNoPromotion = true;
										break;
									}
								}
							}
						}
						if(!bNoPromotion)
						{
							setHasPromotion(ePromotion, true);
						}
					}
				}
			}
			if(pAdjacentPlot->isMountain())
			{
				PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_MOUNTAIN)->getAdjacentSpawnLocationUnitFreePromotion();
				if(ePromotion != NO_PROMOTION)
				{
					CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
					if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
					{
						bool bNoPromotion = false;
						// Check for negating promotions
						if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
						{
							bNoPromotion = true;
						}
						if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
						{
							bNoPromotion = true;
						}
						if(!bNoPromotion)
						{
							for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
							{
								const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
								CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
								if(pkPromotionInfo)
								{
									PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
									// Unit has negation promotion
									if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
									{
										bNoPromotion = true;
										break;
									}
								}
							}
						}
						if(!bNoPromotion)
						{
							setHasPromotion(ePromotion, true);
						}
					}
				}
			}
			if(pAdjacentPlot->isShallowWater() && !pAdjacentPlot->isLake())
			{
				PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_COAST)->getAdjacentSpawnLocationUnitFreePromotion();
				if(ePromotion != NO_PROMOTION)
				{
					CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
					if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
					{
						bool bNoPromotion = false;
						// Check for negating promotions
						if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
						{
							bNoPromotion = true;
						}
						if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
						{
							bNoPromotion = true;
						}
						if(!bNoPromotion)
						{
							for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
							{
								const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
								CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
								if(pkPromotionInfo)
								{
									PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
									// Unit has negation promotion
									if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
									{
										bNoPromotion = true;
										break;
									}
								}
							}
						}
						if(!bNoPromotion)
						{
							setHasPromotion(ePromotion, true);
						}
					}
				}
			}
			if(!pAdjacentPlot->isShallowWater() && !pAdjacentPlot->isLake() && pAdjacentPlot->isWater())
			{
				PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_OCEAN)->getAdjacentSpawnLocationUnitFreePromotion();
				if(ePromotion != NO_PROMOTION)
				{
					CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
					if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
					{
						bool bNoPromotion = false;
						// Check for negating promotions
						if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
						{
							bNoPromotion = true;
						}
						if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
						{
							bNoPromotion = true;
						}
						if(!bNoPromotion)
						{
							for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
							{
								const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
								CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
								if(pkPromotionInfo)
								{
									PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
									// Unit has negation promotion
									if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
									{
										bNoPromotion = true;
										break;
									}
								}
							}
						}
						if(!bNoPromotion)
						{
							setHasPromotion(ePromotion, true);
						}
					}
				}
			}
		}
	}
	// Feature that provides free promotions?
	FeatureTypes eFeature = plot()->getFeatureType();
	if(eFeature != NO_FEATURE)
	{
		PromotionTypes ePromotion = (PromotionTypes)GC.getFeatureInfo(eFeature)->getSpawnLocationUnitFreePromotion();
		if(ePromotion != NO_PROMOTION)
		{
			CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
			if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
			{
				bool bNoPromotion = false;
				// Check for negating promotions
				if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
				{
					bNoPromotion = true;
				}
				if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
				{
					bNoPromotion = true;
				}
				if(!bNoPromotion)
				{
					for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
					{
						const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
						CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
						if(pkPromotionInfo)
						{
							PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
							// Unit has negation promotion
							if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
							{
								bNoPromotion = true;
								break;
							}
						}
					}
				}
				if(!bNoPromotion)
				{
					setHasPromotion(ePromotion, true);
				}
			}
		}
	}
	// Terrain that provides free promotions?
	TerrainTypes eTerrain = plot()->getTerrainType();
	if(eTerrain != NO_TERRAIN && (eTerrain <= TERRAIN_SNOW))
	{
		PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(eTerrain)->getSpawnLocationUnitFreePromotion();
		if(ePromotion != NO_PROMOTION)
		{
			CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
			if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
			{
				bool bNoPromotion = false;
				// Check for negating promotions
				if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
				{
					bNoPromotion = true;
				}
				if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
				{
					bNoPromotion = true;
				}
				if(!bNoPromotion)
				{
					for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
					{
						const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
						CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
						if(pkPromotionInfo)
						{
							PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
							// Unit has negation promotion
							if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
							{
								bNoPromotion = true;
								break;
							}
						}
					}
				}
				if(!bNoPromotion)
				{
					setHasPromotion(ePromotion, true);
				}
			}
		}
	}
	if(plot()->isHills())
	{
		PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_HILL)->getSpawnLocationUnitFreePromotion();
		if(ePromotion != NO_PROMOTION)
		{
			CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
			if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
			{
				bool bNoPromotion = false;
				// Check for negating promotions
				if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
				{
					bNoPromotion = true;
				}
				if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
				{
					bNoPromotion = true;
				}
				if(!bNoPromotion)
				{
					for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
					{
						const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
						CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
						if(pkPromotionInfo)
						{
							PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
							// Unit has negation promotion
							if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
							{
								bNoPromotion = true;
								break;
							}
						}
					}
				}
				if(!bNoPromotion)
				{
					setHasPromotion(ePromotion, true);
				}
			}
		}
	}
	if(plot()->isMountain())
	{
		PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_MOUNTAIN)->getSpawnLocationUnitFreePromotion();
		if(ePromotion != NO_PROMOTION)
		{
			CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
			if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
			{
				bool bNoPromotion = false;
				// Check for negating promotions
				if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
				{
					bNoPromotion = true;
				}
				if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
				{
					bNoPromotion = true;
				}
				if(!bNoPromotion)
				{
					for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
					{
						const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
						CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
						if(pkPromotionInfo)
						{
							PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
							// Unit has negation promotion
							if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
							{
								bNoPromotion = true;
								break;
							}
						}
					}
				}
				if(!bNoPromotion)
				{
					setHasPromotion(ePromotion, true);
				}
			}
		}
	}
	if(plot()->isShallowWater() && !plot()->isLake())
	{
		PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_COAST)->getSpawnLocationUnitFreePromotion();
		if(ePromotion != NO_PROMOTION)
		{
			CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
			if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
			{
				bool bNoPromotion = false;
				// Check for negating promotions
				if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
				{
					bNoPromotion = true;
				}
				if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
				{
					bNoPromotion = true;
				}
				if(!bNoPromotion)
				{
					for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
					{
						const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
						CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
						if(pkPromotionInfo)
						{
							PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
							// Unit has negation promotion
							if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
							{
								bNoPromotion = true;
								break;
							}
						}
					}
				}
				if(!bNoPromotion)
				{
					setHasPromotion(ePromotion, true);
				}
			}
		}
	}
	if(!plot()->isShallowWater() && !plot()->isLake() && plot()->isWater())
	{
		PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_OCEAN)->getSpawnLocationUnitFreePromotion();
		if(ePromotion != NO_PROMOTION)
		{
			CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
			if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
			{
				bool bNoPromotion = false;
				// Check for negating promotions
				if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
				{
					bNoPromotion = true;
				}
				if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
				{
					bNoPromotion = true;
				}
				if(!bNoPromotion)
				{
					for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
					{
						const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
						CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
						if(pkPromotionInfo)
						{
							PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
							// Unit has negation promotion
							if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
							{
								bNoPromotion = true;
								break;
							}
						}
					}
				}
				if(!bNoPromotion)
				{
					setHasPromotion(ePromotion, true);
				}
			}
		}
	}
#endif
	m_iArmyId = -1;

	m_eUnitAIType = eUnitAI;
#if defined(MOD_BALANCE_CORE)
	if(IsGreatPerson())
	{
		int iTourism = kPlayer.GetEventTourism();
		kPlayer.ChangeNumHistoricEvents(1);
		// Culture boost based on previous turns
		int iPreviousTurnsToCount = 10;
		// Calculate boost
		iTourism *= kPlayer.GetCultureYieldFromPreviousTurns(GC.getGame().getGameTurn(), iPreviousTurnsToCount);
		iTourism /= 100;
		if(iTourism > 0)
		{
			kPlayer.GetCulture()->AddTourismAllKnownCivs(iTourism);
			if(kPlayer.GetID() == GC.getGame().getActivePlayer())
			{
				char text[256] = {0};
				float fDelay = 0.5f;
				sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_TOURISM]", iTourism);
				DLLUI->AddPopupText(getX(),getY(), text, fDelay);
				CvNotifications* pNotification = kPlayer.GetNotifications();
				if(pNotification)
				{
					CvString strMessage;
					CvString strSummary;
					strMessage = GetLocalizedText("TXT_KEY_TOURISM_EVENT_GP", iTourism);
					strSummary = GetLocalizedText("TXT_KEY_TOURISM_EVENT_SUMMARY");
					pNotification->Add(NOTIFICATION_CULTURE_VICTORY_SOMEONE_INFLUENTIAL, strMessage, strSummary, getX(), getY(), kPlayer.GetID());
				}
			}
		}
	}
#endif
	// Update Unit Production Maintenance
	kPlayer.UpdateUnitProductionMaintenanceMod();

	// Minor Civ quest
	if(!kPlayer.isMinorCiv() && !isBarbarian())
	{
		PlayerTypes eMinor;
		for(int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
		{
			eMinor = (PlayerTypes) iMinorCivLoop;
			if(GET_PLAYER(eMinor).isAlive())
			{
				// Does this Minor want us to spawn a Unit?
				GET_PLAYER(eMinor).GetMinorCivAI()->DoTestActiveQuestsForPlayer(getOwner(), /*bTestComplete*/ true, /*bTestObsolete*/ false, MINOR_CIV_QUEST_GREAT_PERSON);
			}
		}
	}

	//safety first
	if(GC.getMap().plot(iX,iY)->needsEmbarkation(this))
	{
		if (CanEverEmbark())
			setEmbarked(true);
		else
			OutputDebugString("warning: putting un-embarkable unit on water plot!\n");
	}

	if(bSetupGraphical)
		setupGraphical();
		
#if defined(MOD_EVENTS_UNIT_CREATED)
	if (MOD_EVENTS_UNIT_CREATED) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_UnitCreated, getOwner(), GetID(), getUnitType(), getX(), getY());
	}
#endif
}


//	--------------------------------------------------------------------------------
void CvUnit::uninit()
{
	VALIDATE_OBJECT
	uninitInfos();

	m_missionQueue.clear();

	m_Promotions.Uninit();

	m_kLastPath.clear();

	delete m_pReligion;
	m_pReligion = NULL;
}


//	--------------------------------------------------------------------------------
// FUNCTION: reset()
// Initializes data members that are serialized.
void CvUnit::reset(int iID, UnitTypes eUnit, PlayerTypes eOwner, bool bConstructorCall)
{
	VALIDATE_OBJECT
	int iI;

	FAutoArchive& archive = getSyncArchive();
	archive.clearDelta();
	archive.reset();

	m_iID = iID;
	m_iHotKeyNumber = -1;
	m_iX = INVALID_PLOT_COORD;
	m_iY = INVALID_PLOT_COORD;
	m_iLastMoveTurn = 0;
	m_iCycleOrder = -1;
	m_iDeployFromOperationTurn = -100;
	m_iReconX = INVALID_PLOT_COORD;
	m_iReconY = INVALID_PLOT_COORD;
	m_iReconCount = 0;
	m_iGameTurnCreated = 0;
	m_iDamage = 0;
	m_iMoves = 0;
	m_bImmobile = false;
	m_iExperience = 0;
	m_iLevel = 1;
	m_iCargo = 0;
	m_iAttackPlotX = INVALID_PLOT_COORD;
	m_iAttackPlotY = INVALID_PLOT_COORD;
	m_iCombatTimer = 0;
	m_iCombatFirstStrikes = 0;
	m_iCombatDamage = 0;
	m_iFortifyTurns = 0;
	m_bFortifiedThisTurn = false;
	m_iBlitzCount = 0;
	m_iAmphibCount = 0;
	m_iRiverCrossingNoPenaltyCount = 0;
	m_iEnemyRouteCount = 0;
	m_iRivalTerritoryCount = 0;
	m_iMustSetUpToRangedAttackCount = 0;
	m_iRangeAttackIgnoreLOSCount = 0;
	m_iCityAttackOnlyCount = 0;
	m_iCaptureDefeatedEnemyCount = 0;
#if defined(MOD_BALANCE_CORE)
	m_iCannotBeCapturedCount = 0;
	m_iForcedDamage = 0;
	m_iChangeDamage = 0;
#endif
	m_iRangedSupportFireCount = 0;
	m_iAlwaysHealCount = 0;
	m_iHealOutsideFriendlyCount = 0;
	m_iHillsDoubleMoveCount = 0;
	m_iImmuneToFirstStrikesCount = 0;
	m_iExtraVisibilityRange = 0;
#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
	m_iExtraReconRange = 0;
#endif
	m_iExtraMoves = 0;
	m_iExtraMoveDiscount = 0;
	m_iExtraRange = 0;
	m_iExtraIntercept = 0;
	m_iExtraEvasion = 0;
	m_iExtraFirstStrikes = 0;
	m_iExtraChanceFirstStrikes = 0;
	m_iExtraWithdrawal = 0;
	m_iExtraEnemyHeal = 0;
	m_iExtraNeutralHeal = 0;
	m_iExtraFriendlyHeal = 0;
	m_iSameTileHeal = 0;
	m_iAdjacentTileHeal = 0;
	m_iEnemyDamageChance = 0;
	m_iNeutralDamageChance = 0;
	m_iEnemyDamage = 0;
	m_iNeutralDamage = 0;
	m_iNearbyEnemyCombatMod = 0;
	m_iNearbyEnemyCombatRange = 0;
	m_iExtraCombatPercent = 0;
	m_iAdjacentModifier = 0;
	m_iRangedAttackModifier = 0;
	m_iInterceptionCombatModifier = 0;
	m_iInterceptionDefenseDamageModifier = 0;
	m_iAirSweepCombatModifier = 0;
	m_iAttackModifier = 0;
	m_iDefenseModifier = 0;
	m_iExtraCityAttackPercent = 0;
	m_iExtraCityDefensePercent = 0;
	m_iExtraRangedDefenseModifier = 0;
	m_iExtraHillsAttackPercent = 0;
	m_iExtraHillsDefensePercent = 0;
	m_iExtraOpenAttackPercent = 0;
	m_iExtraOpenRangedAttackMod= 0;
	m_iExtraRoughAttackPercent = 0;
	m_iExtraRoughRangedAttackMod= 0;
	m_iExtraAttackFortifiedMod= 0;
	m_iExtraAttackWoundedMod= 0;
	m_iFlankAttackModifier=0;
	m_iExtraOpenDefensePercent = 0;
	m_iExtraRoughDefensePercent = 0;
	m_iPillageChange = 0;
	m_iUpgradeDiscount = 0;
	m_iExperiencePercent = 0;
	m_iDropRange = 0;
	m_iAirSweepCapableCount = 0;
	m_iExtraNavalMoves = 0;
	m_iKamikazePercent = 0;
	m_eFacingDirection = DIRECTION_SOUTHEAST;
	m_iIgnoreTerrainCostCount = 0;
#if defined(MOD_API_PLOT_BASED_DAMAGE)
	m_iIgnoreTerrainDamageCount = 0;
	m_iIgnoreFeatureDamageCount = 0;
	m_iExtraTerrainDamageCount = 0;
	m_iExtraFeatureDamageCount = 0;
#endif
#if defined(MOD_PROMOTIONS_IMPROVEMENT_BONUS)
	m_iNearbyImprovementCombatBonus = 0;
	m_iNearbyImprovementBonusRange = 0;
	m_eCombatBonusImprovement = NO_IMPROVEMENT;
#endif
#if defined(MOD_PROMOTIONS_CROSS_MOUNTAINS)
	m_iCanCrossMountainsCount = 0;
#endif
#if defined(MOD_PROMOTIONS_CROSS_OCEANS)
	m_iCanCrossOceansCount = 0;
#endif
#if defined(MOD_PROMOTIONS_CROSS_ICE)
	m_iCanCrossIceCount = 0;
#endif
#if defined(MOD_BALANCE_CORE)
	m_iNumTilesRevealedThisTurn = 0;
#endif
	m_iRoughTerrainEndsTurnCount = 0;
	m_iEmbarkAbilityCount = 0;
	m_iHoveringUnitCount = 0;
	m_iFlatMovementCostCount = 0;
	m_iCanMoveImpassableCount = 0;
	m_iOnlyDefensiveCount = 0;
	m_iNoDefensiveBonusCount = 0;
	m_iNoCaptureCount = 0;
	m_iNukeImmuneCount = 0;
	m_iAlwaysHealCount = 0;
	m_iHiddenNationalityCount = 0;
	m_iAlwaysHostileCount = 0;
	m_iNoRevealMapCount = 0;
	m_iCanMoveAllTerrainCount = 0;
	m_iCanMoveAfterAttackingCount = 0;
	m_iFreePillageMoveCount = 0;
	m_iHPHealedIfDefeatEnemy = 0;
	m_iGoldenAgeValueFromKills = 0;
	m_iSapperCount = 0;
	m_iCanHeavyCharge = 0;
	m_iNumExoticGoods = 0;
	m_iTacticalAIPlotX = INVALID_PLOT_COORD;
	m_iTacticalAIPlotY = INVALID_PLOT_COORD;
	m_iGarrisonCityID = -1;
	m_iFlags = 0;
	m_iNumAttacks = 1;
	m_iAttacksMade = 0;
	m_iGreatGeneralCount = 0;
	m_iGreatAdmiralCount = 0;
	m_iGreatGeneralModifier = 0;
	m_iGreatGeneralReceivesMovementCount = 0;
	m_iGreatGeneralCombatModifier = 0;
	m_iIgnoreGreatGeneralBenefit = 0;
	m_iIgnoreZOC = 0;
	m_iHealIfDefeatExcludeBarbariansCount = 0;
	m_iNumInterceptions = 1;
	m_iMadeInterceptionCount = 0;
	m_iEverSelectedCount = 0;

	m_bIgnoreDangerWakeup = false;
	m_iEmbarkedAllWaterCount = 0;
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
	m_iEmbarkedDeepWaterCount = 0;
#endif
	m_iEmbarkExtraVisibility = 0;
	m_iEmbarkDefensiveModifier = 0;
	m_iCapitalDefenseModifier = 0;
	m_iCapitalDefenseFalloff = 0;
	m_iCityAttackPlunderModifier = 0;
	m_iReligiousStrengthLossRivalTerritory = 0;
	m_iTradeMissionInfluenceModifier = 0;
	m_iTradeMissionGoldModifier = 0;

	m_bPromotionReady = false;
	m_bDeathDelay = false;
	m_bCombatFocus = false;
	m_bInfoBarDirty = false;
	m_bNotConverting = false;
	m_bAirCombat = false;
	m_bSetUpForRangedAttack = false;
	m_bEmbarked = false;
	m_bAITurnProcessed = false;
	m_bWaitingForMove = false;
	m_eTacticalMove = NO_TACTICAL_MOVE;

#if defined(MOD_BALANCE_CORE_MILITARY)
	m_eHomelandMove = AI_HOMELAND_MOVE_NONE;
	m_iHomelandMoveSetTurn = 0;
#endif

	m_eOwner = eOwner;
	m_eOriginalOwner = eOwner;
	m_eCapturingPlayer = NO_PLAYER;
	m_bCapturedAsIs = false;
	m_eUnitType = eUnit;
	m_pUnitInfo = (NO_UNIT != m_eUnitType) ? GC.getUnitInfo(m_eUnitType) : NULL;
	m_iBaseCombat = (NO_UNIT != m_eUnitType) ? m_pUnitInfo->GetCombat() : 0;
#if defined(MOD_API_EXTENSIONS)
	m_iBaseRangedCombat = (NO_UNIT != m_eUnitType) ? m_pUnitInfo->GetRangedCombat() : 0;
#endif
	m_eLeaderUnitType = NO_UNIT;
	m_eInvisibleType = NO_INVISIBLE;
	m_eSeeInvisibleType = NO_INVISIBLE;
	m_eGreatPeopleDirectiveType = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;
	m_iCargoCapacity = 0;

	m_combatUnit.reset();
	m_transportUnit.reset();

	m_extraDomainModifiers.clear();

	for(iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		m_extraDomainModifiers.push_back(0);
	}

#if defined(MOD_PROMOTIONS_UNIT_NAMING)
	m_strUnitName = "";
#endif
	m_strName = "";
	m_eGreatWork = NO_GREAT_WORK;
	m_iTourismBlastStrength = 0;
	m_strNameIAmNotSupposedToBeUsedAnyMoreBecauseThisShouldNotBeCheckedAndWeNeedToPreserveSaveGameCompatibility = "";
	m_strScriptData ="";
	m_iScenarioData = 0;

	m_unitMoveLocs.clear();

	uninitInfos();

	// Migrated in from CvSelectionGroup
	m_iMissionTimer = 0;
	m_eActivityType = ACTIVITY_AWAKE;
	m_eAutomateType = NO_AUTOMATE;

	m_kLastPath.clear();
	m_uiLastPathCacheDest = (uint)-1;
#if defined(MOD_BALANCE_CORE)
	m_iScienceBlastStrength = 0;
	m_iCultureBlastStrength = 0;
	m_uiLastPathFlags = 0;
#endif

	m_iMapLayer = DEFAULT_UNIT_MAP_LAYER;
	m_iNumGoodyHutsPopped = 0;
	m_iLastGameTurnAtFullHealth = -1;

#if defined(MOD_BALANCE_CORE_MILITARY)
	m_strMissionInfoString = "";
	m_iTactMoveSetTurn = 0;
#endif

	if(!bConstructorCall)
	{
		m_Promotions.Reset();

		m_terrainDoubleMoveCount.dirtyGet().clear();
#if defined(MOD_PROMOTIONS_HALF_MOVE)
		m_terrainHalfMoveCount.dirtyGet().clear();
#endif
#if defined(MOD_BALANCE_CORE)
		m_terrainDoubleHeal.dirtyGet().clear();
#endif
#if defined(MOD_BALANCE_CORE)
		m_PromotionDuration.dirtyGet().clear();
		m_TurnPromotionGained.dirtyGet().clear();
#endif
		m_terrainImpassableCount.dirtyGet().clear();
		m_extraTerrainAttackPercent.dirtyGet().clear();
		m_extraTerrainDefensePercent.dirtyGet().clear();

		m_featureDoubleMoveCount.dirtyGet().clear();
#if defined(MOD_PROMOTIONS_HALF_MOVE)
		m_featureHalfMoveCount.dirtyGet().clear();
#endif
#if defined(MOD_BALANCE_CORE)
		m_featureDoubleHeal.dirtyGet().clear();
#endif
		m_featureImpassableCount.dirtyGet().clear();
		m_extraFeatureDefensePercent.dirtyGet().clear();
		m_extraFeatureAttackPercent.dirtyGet().clear();

#if defined(MOD_API_UNIFIED_YIELDS)
		m_yieldFromKills.clear();
		m_yieldFromBarbarianKills.clear();
		
		m_yieldFromKills.resize(NUM_YIELD_TYPES);
		m_yieldFromBarbarianKills.resize(NUM_YIELD_TYPES);

		for(int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			m_yieldFromKills.setAt(i,0);
			m_yieldFromBarbarianKills.setAt(i,0);
		}
#endif

		CvAssertMsg((0 < GC.getNumUnitCombatClassInfos()), "GC.getNumUnitCombatClassInfos() is not greater than zero but an array is being allocated in CvUnit::reset");
		m_extraUnitCombatModifier.clear();
		m_extraUnitCombatModifier.resize(GC.getNumUnitCombatClassInfos());
		for(int i = 0; i < GC.getNumUnitCombatClassInfos(); i++)
		{
			m_extraUnitCombatModifier.setAt(i,0);
		}

		m_unitClassModifier.clear();
		m_unitClassModifier.resize(GC.getNumUnitClassInfos());
		for(int i = 0; i < GC.getNumUnitClassInfos(); i++)
		{
			CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo((UnitClassTypes)i);
			if(!pkUnitClassInfo)
			{
				continue;
			}

			m_unitClassModifier.setAt(i,0);
		}

		// Migrated in from CvSelectionGroup
		m_iMissionAIX = INVALID_PLOT_COORD;
		m_iMissionAIY = INVALID_PLOT_COORD;
		m_eMissionAIType = NO_MISSIONAI;
		m_missionAIUnit.reset();

		m_eUnitAIType = NO_UNITAI;
	}
}


//////////////////////////////////////
// graphical only setup
//////////////////////////////////////
void CvUnit::setupGraphical()
{
	VALIDATE_OBJECT
	if(!GC.IsGraphicsInitialized())
	{
		return;
	}

	ICvEngineUtility1* pDLL = GC.getDLLIFace();
	auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));

	if(isEmbarked())
	{
		pDLL->GameplayUnitEmbark(pDllUnit.get(), true);
	}
	else
	{
		pDLL->GameplayUnitCreated(pDllUnit.get());

		pDLL->GameplayUnitShouldDimFlag(pDllUnit.get(), /*bDim*/ getMoves() <= 0);

		if(getFortifyTurns() > 0)
		{
			pDLL->GameplayUnitFortify(pDllUnit.get(), true);
		}

		int iNewValue = getDamage();
		if(iNewValue > 0)
		{
			pDLL->GameplayUnitSetDamage(pDllUnit.get(), iNewValue, 0);
		}

		if(IsWork())
		{
			BuildTypes currentBuild = getBuildType();
			if(currentBuild != NO_BUILD)
			{
				pDLL->GameplayUnitWork(pDllUnit.get(), currentBuild);
			}
		}

		if(IsGarrisoned())
		{
			pDLL->GameplayUnitVisibility(pDllUnit.get(), false);
			pDLL->GameplayUnitGarrison(pDllUnit.get(), true);
		}
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::initPromotions()
{
	VALIDATE_OBJECT

	m_Promotions.Init(GC.GetGamePromotions(), this);
}

//	--------------------------------------------------------------------------------
void CvUnit::uninitInfos()
{
	VALIDATE_OBJECT
#if defined(MOD_PROMOTIONS_HALF_MOVE)
	m_terrainDoubleMoveCount.dirtyGet().clear(); // BUG FIX
	m_featureDoubleMoveCount.dirtyGet().clear();
	m_terrainHalfMoveCount.dirtyGet().clear();
	m_featureHalfMoveCount.dirtyGet().clear();
#else
	m_featureDoubleMoveCount.clear();
#endif
#if defined(MOD_BALANCE_CORE)
	m_PromotionDuration.dirtyGet().clear();
	m_TurnPromotionGained.dirtyGet().clear();
#endif
#if defined(MOD_BALANCE_CORE)
	m_terrainDoubleHeal.dirtyGet().clear();
	m_featureDoubleHeal.dirtyGet().clear();
#endif
	m_terrainImpassableCount.dirtyGet().clear();
	m_featureImpassableCount.dirtyGet().clear();
	m_extraTerrainAttackPercent.dirtyGet().clear();
	m_extraTerrainDefensePercent.dirtyGet().clear();
	m_extraFeatureAttackPercent.dirtyGet().clear();
	m_extraFeatureDefensePercent.dirtyGet().clear();
#if defined(MOD_API_UNIFIED_YIELDS)
	m_yieldFromKills.clear();
	m_yieldFromBarbarianKills.clear();
#endif
	m_extraUnitCombatModifier.clear();
	m_unitClassModifier.clear();
}


//	--------------------------------------------------------------------------------
void CvUnit::convert(CvUnit* pUnit, bool bIsUpgrade)
{
	VALIDATE_OBJECT
	IDInfo* pUnitNode;
	CvUnit* pTransportUnit;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;

	pPlot = plot();
#if defined(MOD_BALANCE_CORE)
	int iLostPromotions = 0;
	bool bFree = false;
#endif
	// Transfer Promotions over
	for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(iI);
		CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
		if(pkPromotionInfo)
		{
			bool bGivePromotion = false;

			// Old unit has the promotion
			if(pUnit->isHasPromotion(ePromotion) && !pkPromotionInfo->IsLostWithUpgrade())
			{
				bGivePromotion = true;
			}

			// New unit gets promotion for free (as per XML)
			else if(getUnitInfo().GetFreePromotions(ePromotion) && (!bIsUpgrade || !pkPromotionInfo->IsNotWithUpgrade()))
			{
				bGivePromotion = true;
#if defined(MOD_BALANCE_CORE)
				bFree = true;
				if((ePromotion == (PromotionTypes)GC.getPROMOTION_OCEAN_IMPASSABLE() || ePromotion == (PromotionTypes)GC.getPROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY()) && (GET_PLAYER(getOwner()).GetPlayerTraits()->IsEmbarkedAllWater() || GET_TEAM(GET_PLAYER(getOwner()).getTeam()).canEmbarkAllWaterPassage()))
				{
					bGivePromotion = false;
					bFree = false;
				}
#endif
			}

			// if we get this due to a policy or wonder
			else if(GET_PLAYER(getOwner()).IsFreePromotion(ePromotion) && (
						::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForCivilianUnitType(ePromotion, getUnitType())))
			{
				bGivePromotion = true;
#if defined(MOD_BALANCE_CORE)
				bFree = true;
				if((ePromotion == (PromotionTypes)GC.getPROMOTION_OCEAN_IMPASSABLE() || ePromotion == (PromotionTypes)GC.getPROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY()) && (GET_PLAYER(getOwner()).GetPlayerTraits()->IsEmbarkedAllWater() || GET_TEAM(GET_PLAYER(getOwner()).getTeam()).canEmbarkAllWaterPassage()))
				{
					bGivePromotion = false;
					bFree = false;
				}
#endif
			}
#if defined(MOD_BALANCE_CORE)
			if(pUnit->getUnitCombatType() != getUnitCombatType())
			{
				bool bRanged = false;
				if((pkPromotionInfo->GetRangedAttackModifier() > 0) || (pkPromotionInfo->GetRangeChange() > 0) || (pkPromotionInfo->GetRangedAttackModifier() > 0) || (pkPromotionInfo->IsRangeAttackIgnoreLOS()) || (pkPromotionInfo->GetOpenRangedAttackMod() > 0) || (pkPromotionInfo->GetRoughRangedAttackMod() > 0) || (pkPromotionInfo->GetExtraAttacks() > 0))
				{
					bRanged = true;
				}
				//If we're losing standard promotions because of a combatclass change, let's replace with some experience.
				if(!isRanged() && pUnit->HasPromotion(ePromotion) && pUnit->isRanged() && bRanged && !bFree)
				{
					iLostPromotions++;
					bGivePromotion = false;
				}
				//Naval Misfire Promotion Catch (sorry for hardcode)
				else if(!isRanged() && pUnit->HasPromotion(ePromotion) && pUnit->isRanged() && (pkPromotionInfo->GetDomainModifierPercent(DOMAIN_SEA) < 0) && (getDomainType() == DOMAIN_LAND))
				{
					iLostPromotions++;
					bGivePromotion = false;
				}
			}
#endif

			setHasPromotion(ePromotion, bGivePromotion);
		}
	}
#if defined(MOD_BALANCE_CORE)
	//20 xp per lost 'good' promotion (plus level).
	if(iLostPromotions > 0)
	{
		int iLevel = pUnit->getLevel();
		if(iLevel <= 0)
		{
			iLevel = 1;
		}
		changeExperience(20 * iLostPromotions * iLevel);
	}
#endif
	setGameTurnCreated(pUnit->getGameTurnCreated());
	setLastMoveTurn(pUnit->getLastMoveTurn());
	setDamage(pUnit->getDamage());
	setMoves(pUnit->getMoves());
	setEmbarked(pUnit->isEmbarked());
	setFacingDirection(pUnit->getFacingDirection(false));
	SetBeenPromotedFromGoody(pUnit->IsHasBeenPromotedFromGoody());
	SetTourismBlastStrength(pUnit->GetTourismBlastStrength());
#if defined(MOD_BALANCE_CORE)
	SetScienceBlastStrength(pUnit->getDiscoverAmount());
	SetCultureBlastStrength(pUnit->getGivePoliciesCulture());
#endif
	if (pUnit->getUnitInfo().GetNumExoticGoods() > 0)
	{
		setNumExoticGoods(pUnit->getNumExoticGoods());
	}
#if defined(MOD_BALANCE_CORE)
	if(pUnit->getUnitCombatType() != getUnitCombatType() && (iLostPromotions > 0))
	{
		setLevel(1);
	}
	else
	{
#endif
	setLevel(pUnit->getLevel());
#if defined(MOD_BALANCE_CORE)
	}
#endif
	int iOldModifier = std::max(1, 100 + GET_PLAYER(pUnit->getOwner()).getLevelExperienceModifier());
	int iOurModifier = std::max(1, 100 + GET_PLAYER(getOwner()).getLevelExperienceModifier());
	setExperience(std::max(0, (pUnit->getExperience() * iOurModifier) / iOldModifier));

	setName(pUnit->getNameNoDesc());
	setLeaderUnitType(pUnit->getLeaderUnitType());
	ChangeNumGoodyHutsPopped(pUnit->GetNumGoodyHutsPopped());

	pTransportUnit = pUnit->getTransportUnit();

	if(pTransportUnit != NULL)
	{
		pUnit->setTransportUnit(NULL);
		setTransportUnit(pTransportUnit);
	}

	pUnitNode = pPlot->headUnitNode();

	while(pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if(pLoopUnit && pLoopUnit->getTransportUnit() == pUnit)
		{
			pLoopUnit->setTransportUnit(this);
		}
	}

	pUnit->kill(true);
}

//	----------------------------------------------------------------------------
//	Kill a unit.
//	Parameters:
//		bDelay			- If true, the unit will be partially cleaned up, but its final removal will happen at the end of the frame.
//		ePlayer			- Optional player ID who is doing the killing.
void CvUnit::kill(bool bDelay, PlayerTypes ePlayer /*= NO_PLAYER*/)
{
	VALIDATE_OBJECT
	CvUnit* pTransportUnit;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;
	CvString strBuffer;

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(getOwner());
		args->Push(GetID());  // unit being killed

		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "CanSaveUnit", args.get(), bResult))
		{
			// Check the result.
			if(bResult)
			{
				return;   // Unit was saved
			}
		}
	}

//---------------------------------
	if (ePlayer!=NO_PLAYER)
	{
		TacticalAIMoveTypes move = getTacticalMove();
		saiTaskWhenKilled[ (int)move+1 ]++;
	}
//---------------------------------

	auto_ptr<ICvUnit1> pDllThisUnit = GC.WrapUnitPointer(this);

	if(IsSelected() && !bDelay)
	{
		DLLUI->VisuallyDeselectUnit(pDllThisUnit.get());
	}

	GET_PLAYER(getOwner()).removeFromArmy(m_iArmyId, GetID());

	pPlot = plot();
	CvAssertMsg(pPlot != NULL, "Plot is not assigned a valid value");

	if(pPlot)
	{
		IDInfo* pkOldUnits = NULL;
		uint uiOldUnitCount = pPlot->getNumUnits();
		const IDInfo* pUnitNode = pPlot->headUnitNode();

		if(pUnitNode)
		{
			pkOldUnits = (IDInfo*)_malloca(pPlot->getNumUnits() * sizeof(IDInfo));	// Allocate an array on the stack, it shouldn't be too large
			IDInfo* pkEntry = pkOldUnits;
			while(pUnitNode != NULL)
			{
				*pkEntry++ = *pUnitNode;
				pUnitNode = pPlot->nextUnitNode(pUnitNode);
			}
		}

		for(uint i = 0; i < uiOldUnitCount; i++)
		{
			pLoopUnit = ::getUnit(pkOldUnits[i]);

			if(pLoopUnit != NULL)
			{
				if(pLoopUnit->getTransportUnit() == this)	// Is this the transport for the search unit?
				{
					if(pLoopUnit->isSuicide() && pLoopUnit->isInCombat())
					{
						// Let suicide units (missiles & other single use items) kill themselves.
						pLoopUnit->setTransportUnit(NULL);
					}
					else
					{
						if(pPlot->isValidDomainForLocation(*pLoopUnit))
						{
							pLoopUnit->setCapturingPlayer(getCapturingPlayer());	// KWG: Creating a new captured cargo, but how does its transport (this) then get attached to the new cargo?
						}

						pLoopUnit->kill(false, ePlayer);
					}
				}
			}
		}

		if(pkOldUnits)
			_freea(pkOldUnits);
	}
	// If a player killed this Unit...
	if(ePlayer != NO_PLAYER)
	{
#if defined(MOD_BALANCE_CORE)
		//Faith bonus from defeated units.
		int iEra = GET_PLAYER(getOwner()).GetCurrentEra();
		if(iEra < 1)
		{
			iEra = 1;
		}
		if(GET_PLAYER(getOwner()).getYieldFromDeath(YIELD_FAITH) > 0)
		{
			if(IsCombatUnit())
			{
				int iFaith = GET_PLAYER(getOwner()).getYieldFromDeath(YIELD_FAITH) * iEra;
				GET_PLAYER(getOwner()).ChangeFaith(iFaith);
				if(getOwner() == GC.getGame().getActivePlayer())
				{
					char text[256] = {0};
					float fDelay = GC.getPOST_COMBAT_TEXT_DELAY() * 2;
					sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_PEACE]", iFaith);
					DLLUI->AddPopupText(pPlot->getX(), pPlot->getY(), text, fDelay);
				}
			}
		}
#endif
		if(!isBarbarian() && !GET_PLAYER(ePlayer).isBarbarian())
		{
			// Notify Diplo AI that damage has been done
			// Best unit that can be built now is given value of 100
			int iValue = getUnitInfo().GetPower();

			int iTypicalPower = GET_PLAYER(ePlayer).GetMilitaryAI()->GetPowerOfStrongestBuildableUnit(DOMAIN_LAND);

			if(iTypicalPower > 0)
			{
				iValue = iValue* /*100*/ GC.getDEFAULT_WAR_VALUE_FOR_UNIT() / iTypicalPower;
			}
			else
			{
				iValue = /*100*/ GC.getDEFAULT_WAR_VALUE_FOR_UNIT();
			}
#if defined(MOD_BALANCE_CORE)
			if(IsCivilianUnit() && pPlot && !pPlot->isCity())
			{
				iValue = GC.getDEFAULT_WAR_VALUE_FOR_UNIT();
			}
			else if(IsCivilianUnit() && pPlot && pPlot->isCity())
			{
				iValue = GC.getDEFAULT_WAR_VALUE_FOR_UNIT() / 4;
			}
#endif

			// My viewpoint
			GET_PLAYER(getOwner()).GetDiplomacyAI()->ChangeWarValueLost(ePlayer, iValue);
			// Bad guy's viewpoint
			GET_PLAYER(ePlayer).GetDiplomacyAI()->ChangeOtherPlayerWarValueLost(getOwner(), ePlayer, iValue);
		
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
			if (MOD_DIPLOMACY_CIV4_FEATURES) {
				CvCity* pLoopCity;
				int iCityLoop;
				bool bNearLoserCity = false;
				PlayerTypes eLoopPlayer;

				// Loop through loser's cities.
				for(pLoopCity = GET_PLAYER(getOwner()).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwner()).nextCity(&iCityLoop))
				{
					if(plotDistance(plot()->getX(), plot()->getY(), pLoopCity->getX(), pLoopCity->getY()) <= GC.getVASSALAGE_FAILED_PROTECT_CITY_DISTANCE())
					{
						bNearLoserCity = true;
						break;
					}
				}

				for(int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
				{
					eLoopPlayer = (PlayerTypes) iPlayerLoop;

					// Is loser's team the vassal of ePlayer?
					if(GET_TEAM(GET_PLAYER(getOwner()).getTeam()).IsVassal(GET_PLAYER(eLoopPlayer).getTeam()) && bNearLoserCity)
					{
						// Master's failed protect score goes up for Vassal
						GET_PLAYER(getOwner()).GetDiplomacyAI()->ChangeVassalFailedProtectValue(ePlayer, iValue);
					}
				}
			}
#endif
		}

		if(NO_UNIT != getLeaderUnitType())
		{
			for(int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if(GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_GENERAL_KILLED", getNameKey());
					DLLUI->AddUnitMessage(0, GetIDInfo(), ((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), strBuffer, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_MAJOR_EVENT);
				}
			}
		}
	}
	
#if defined(MOD_EVENTS_UNIT_PREKILL)
	if (MOD_EVENTS_UNIT_PREKILL) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_UnitPrekill, getOwner(), GetID(), getUnitType(), getX(), getY(), bDelay, ePlayer);
	} else {
#endif
	if (pkScriptSystem) 
	{
		CvLuaArgsHandle args;
		args->Push(((int)getOwner()));
		args->Push(GetID());
		args->Push(getUnitType());
		args->Push(getX());
		args->Push(getY());
		args->Push(bDelay);
		args->Push(ePlayer);

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "UnitPrekill", args.get(), bResult);
	}
#if defined(MOD_EVENTS_UNIT_PREKILL)
	}
#endif

	if(bDelay)
	{
		startDelayedDeath();
		return;
	}


	///////////////////////////////////////////////////////////////////////////////////////////////
	// EVERYTHING AFTER THIS LINE OCCURS UPON THE ACTUAL DELETION OF THE UNIT AND NOT WITH A DELAYED DEATH
	///////////////////////////////////////////////////////////////////////////////////////////////

	if(IsSelected())
	{
		DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
		if(DLLUI->GetLengthSelectionList() == 1)
		{
			if(!(DLLUI->isFocused()) && !(DLLUI->isCitySelection()) && !(DLLUI->isDiploOrPopupWaiting()))
			{
				GC.getGame().updateSelectionList();
			}

			if(IsSelected())
			{
				DLLUI->setCycleSelectionCounter(1);
			}
			else
			{
				DLLUI->setDirty(SelectionCamera_DIRTY_BIT, true);
			}
		}
	}

	DLLUI->RemoveFromSelectionList(pDllThisUnit.get());
	GET_PLAYER(getOwner()).GetUnitCycler().RemoveUnit(GetID());

	// Killing a unit while in combat is not something we really expect to happen.
	// It is *mostly* safe for it to happen, but combat systems must be able to gracefully handle the disapperance of a unit.
	CvAssertMsg_Debug(!isInCombat(), "isCombat did not return false as expected");

	clearCombat();	// Disconnect from any combat

	pTransportUnit = getTransportUnit();

	if(pTransportUnit != NULL)
	{
		setTransportUnit(NULL);
	}

	setReconPlot(NULL);

	CvAssertMsg(getAttackPlot() == NULL, "The current unit instance's attack plot is expected to be NULL");
	CvAssertMsg(getCombatUnit() == NULL, "The current unit instance's combat unit is expected to be NULL");

	GET_TEAM(getTeam()).changeUnitClassCount((UnitClassTypes)getUnitInfo().GetUnitClassType(), -1);
	GET_PLAYER(getOwner()).changeUnitClassCount((UnitClassTypes)getUnitInfo().GetUnitClassType(), -1);

	// Builder Limit
	if(getUnitInfo().GetWorkRate() > 0 && getUnitInfo().GetDomainType() == DOMAIN_LAND)
	{
		GET_PLAYER(getOwner()).ChangeNumBuilders(-1);
	}

	// Some Units count against Happiness
	if(getUnitInfo().GetUnhappiness() != 0)
	{
		GET_PLAYER(getOwner()).ChangeUnhappinessFromUnits(-getUnitInfo().GetUnhappiness());
	}

	GET_PLAYER(getOwner()).changeExtraUnitCost(-(getUnitInfo().GetExtraMaintenanceCost()));

	if(getUnitInfo().GetNukeDamageLevel() != -1)
	{
		GET_PLAYER(getOwner()).changeNumNukeUnits(-1);
	}

	if(getUnitInfo().IsMilitarySupport())
	{
		GET_PLAYER(getOwner()).changeNumMilitaryUnits(-1);
	}

	// A unit dying reduces the Great General meter
	if(getExperience() > 0 && ePlayer != NO_PLAYER)
	{
		int iGreatGeneralMeterLoss = getExperience() * /*50*/ GC.getUNIT_DEATH_XP_GREAT_GENERAL_LOSS();
		iGreatGeneralMeterLoss /= 100;

		if(getDomainType() == DOMAIN_SEA)
		{
			GET_PLAYER(getOwner()).changeNavalCombatExperience(-iGreatGeneralMeterLoss);
		}
		else
		{
			GET_PLAYER(getOwner()).changeCombatExperience(-iGreatGeneralMeterLoss);
		}
	}

	CvUnitCaptureDefinition kCaptureDef;
	getCaptureDefinition(&kCaptureDef);
	
#if defined(MOD_EVENTS_UNIT_CAPTURE)
	if (MOD_EVENTS_UNIT_CAPTURE && (kCaptureDef.eCapturingPlayer != NO_PLAYER && kCaptureDef.eCaptureUnitType != NO_UNIT)) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_UnitCaptured, kCaptureDef.eCapturingPlayer, kCaptureDef.eCaptureUnitType, getOwner(), GetID(), false, 1);
	}
#endif

	setXY(INVALID_PLOT_COORD, INVALID_PLOT_COORD, true);
	if(pPlot)
		pPlot->removeUnit(this, false);

	// Remove Resource Quantity from Used
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		if(getUnitInfo().GetResourceQuantityRequirement(iResourceLoop) > 0)
		{
			GET_PLAYER(getOwner()).changeNumResourceUsed((ResourceTypes) iResourceLoop, -getUnitInfo().GetResourceQuantityRequirement(iResourceLoop));
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// WARNING: This next statement will delete 'this'
	// ANYTHING BELOW HERE MUST NOT REFERENCE THE UNIT!
	GET_PLAYER(getOwner()).deleteUnit(GetID());

	// Update Unit Production Maintenance
	GET_PLAYER(kCaptureDef.eOldPlayer).UpdateUnitProductionMaintenanceMod();

	// Create the captured unit that will replace this unit (if the capture definition is valid)
	CvUnit::createCaptureUnit(kCaptureDef);

	if(GC.getGame().getActivePlayer() == kCaptureDef.eOldPlayer)
	{
		CvMap& theMap = GC.getMap();
		theMap.updateDeferredFog();
	}

	//////////////////////////////////////////////////////////////////////////
	// Do not add anything below here in this method
}

//	---------------------------------------------------------------------------
//	Get a definition that can be used to create a captured version of the unit.
bool CvUnit::getCaptureDefinition(CvUnitCaptureDefinition* pkCaptureDef, PlayerTypes eCapturingPlayer /* = NO_PLAYER */)
{
	CvUnitCaptureDefinition kCaptureDef;
	kCaptureDef.eOldPlayer = getOwner();
	kCaptureDef.eOriginalOwner = GetOriginalOwner();
	kCaptureDef.eOldType = getUnitType();
	kCaptureDef.eCapturingPlayer = (eCapturingPlayer != NO_PLAYER) ? eCapturingPlayer : getCapturingPlayer();
	kCaptureDef.bEmbarked = m_bEmbarked;
	kCaptureDef.eCaptureUnitType = NO_UNIT;
#if defined(MOD_API_EXTENSIONS)
	kCaptureDef.iScenarioData = m_iScenarioData;
#endif
	if (GetReligionData())
	{
		kCaptureDef.eReligion = GetReligionData()->GetReligion();
		kCaptureDef.iReligiousStrength = GetReligionData()->GetReligiousStrength();
		kCaptureDef.iSpreadsLeft = GetReligionData()->GetSpreadsLeft();
	}

	// Captured as is?
	if(IsCapturedAsIs())
	{
		kCaptureDef.bAsIs = true;
		kCaptureDef.eCaptureUnitType = getUnitType();
	}

	// Barbs captured this unit, or a player capturing this unit from the barbs
	else if(isBarbarian() || (kCaptureDef.eCapturingPlayer != NO_PLAYER && GET_PLAYER(kCaptureDef.eCapturingPlayer).isBarbarian()))
	{
		// Must be able to capture this unit normally... don't want the barbs picking up Workboats, Generals, etc.
		if(kCaptureDef.eCapturingPlayer != NO_PLAYER && getCaptureUnitType(GET_PLAYER(kCaptureDef.eCapturingPlayer).getCivilizationType()) != NO_UNIT)
			// Unit type is the same as what it was
			kCaptureDef.eCaptureUnitType = getUnitType();
	}

	// Barbs not involved
	else
	{
		if(kCaptureDef.eCapturingPlayer != NO_PLAYER)
			kCaptureDef.eCaptureUnitType = getCaptureUnitType(GET_PLAYER(kCaptureDef.eCapturingPlayer).getCivilizationType());
	}

	CvPlot* pkPlot = plot();
	if(pkPlot)
	{
		kCaptureDef.iX = pkPlot->getX();
		kCaptureDef.iY = pkPlot->getY();
	}
	else
	{
		kCaptureDef.iX = INVALID_PLOT_COORD;
		kCaptureDef.iY = INVALID_PLOT_COORD;
	}

	if(pkCaptureDef)
		*pkCaptureDef = kCaptureDef;

	return kCaptureDef.eCaptureUnitType != NO_UNIT && kCaptureDef.eCapturingPlayer != NO_PLAYER;
}

//	---------------------------------------------------------------------------
//	Create a new unit using a capture definition.
//	Returns the unit if create or NULL is the definition is not valid, or something else
//	goes awry.
//
//	Please note this method is static because it is often called AFTER the original unit
//	has been deleted.

/* static */ CvUnit* CvUnit::createCaptureUnit(const CvUnitCaptureDefinition& kCaptureDef)
{
	CvUnit* pkCapturedUnit = NULL;

	if(kCaptureDef.eCapturingPlayer != NO_PLAYER && kCaptureDef.eCaptureUnitType != NO_UNIT)
	{
		CvPlot* pkPlot = GC.getMap().plot(kCaptureDef.iX , kCaptureDef.iY);
		if(pkPlot)
		{
			CvPlayerAI& kCapturingPlayer = GET_PLAYER(kCaptureDef.eCapturingPlayer);
			if(kCapturingPlayer.isHuman() || kCapturingPlayer.AI_captureUnit(kCaptureDef.eCaptureUnitType, pkPlot) || 0 == GC.getAI_CAN_DISBAND_UNITS())
			{
				pkCapturedUnit = kCapturingPlayer.initUnit(kCaptureDef.eCaptureUnitType, kCaptureDef.iX, kCaptureDef.iY);

				if(pkCapturedUnit != NULL)
				{
					pkCapturedUnit->GetReligionData()->SetReligion(kCaptureDef.eReligion);
					pkCapturedUnit->GetReligionData()->SetReligiousStrength(kCaptureDef.iReligiousStrength);
					pkCapturedUnit->GetReligionData()->SetSpreadsLeft(kCaptureDef.iSpreadsLeft);

					pkCapturedUnit->SetOriginalOwner(kCaptureDef.eOriginalOwner);

#if defined(MOD_API_EXTENSIONS)
					pkCapturedUnit->setScenarioData(kCaptureDef.iScenarioData);
#endif

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString szMsg;
						szMsg.Format("Captured: %s, Enemy was: %s", GC.getUnitInfo(kCaptureDef.eOldType)->GetDescription(), kCapturingPlayer.getCivilizationShortDescription());
						GET_PLAYER(kCaptureDef.eOldPlayer).GetTacticalAI()->LogTacticalMessage(szMsg, true /*bSkipLogDominanceZone*/);
					}

					if(kCaptureDef.bEmbarked)
					{
						auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(pkCapturedUnit));
						gDLL->GameplayUnitEmbark(pDllUnit.get(), true);
						pkCapturedUnit->setEmbarked(true);
						if (!pkCapturedUnit->jumpToNearestValidPlot())
						{
							pkCapturedUnit->kill(true);
							pkCapturedUnit = NULL;
						}
					}

					bool bDisbanded = false;
					if (pkCapturedUnit != NULL)
					{
						pkCapturedUnit->finishMoves();

						// Minor civs can't capture settlers, ever!
						if(!bDisbanded && GET_PLAYER(pkCapturedUnit->getOwner()).isMinorCiv() && (pkCapturedUnit->isFound() || pkCapturedUnit->IsFoundAbroad()))
						{
							bDisbanded = true;
							pkCapturedUnit->kill(false);
							pkCapturedUnit = NULL;
						}
					}

					// Captured civilian who could be returned?
					if(!kCaptureDef.bAsIs)
					{
						bool bShowingHumanPopup = true;
						bool bShowingActivePlayerPopup = true;

						// Only active player gets the choice
						if(GC.getGame().getActivePlayer() != kCaptureDef.eCapturingPlayer)
						{
							bShowingActivePlayerPopup = false;
						}

						// Original owner is dead!
						if(!GET_PLAYER(kCaptureDef.eOriginalOwner).isAlive())
							bShowingHumanPopup = false;

						// If original owner
						else if(kCaptureDef.eOriginalOwner == kCaptureDef.eCapturingPlayer)
							bShowingHumanPopup = false;

						// Players at war
						else if(GET_TEAM(GET_PLAYER(kCaptureDef.eOriginalOwner).getTeam()).isAtWar(kCapturingPlayer.getTeam()))
							bShowingHumanPopup = false;

						// Players haven't met
						else if(!GET_TEAM(GET_PLAYER(kCaptureDef.eOriginalOwner).getTeam()).isHasMet(kCapturingPlayer.getTeam()))
							bShowingHumanPopup = false;

						// Show the popup
						if(bShowingHumanPopup && bShowingActivePlayerPopup && pkCapturedUnit)
						{
							CvPopupInfo kPopupInfo(BUTTONPOPUP_RETURN_CIVILIAN, kCaptureDef.eCapturingPlayer, kCaptureDef.eOriginalOwner, pkCapturedUnit->GetID());
							DLLUI->AddPopup(kPopupInfo);
							// We are adding a popup that the player must make a choice in, make sure they are not in the end-turn phase.
							if (kCapturingPlayer.isLocalPlayer())
								CancelActivePlayerEndTurn();
						}

						// Take it automatically!
						else if(!bShowingHumanPopup && !bDisbanded && pkCapturedUnit != NULL && !pkCapturedUnit->isBarbarian())	// Don't process if the AI decided to disband the unit, or the barbs captured something
						{
							// If the unit originally belonged to us, we've already done what we needed to do
							if(kCaptureDef.eCapturingPlayer != kCaptureDef.eOriginalOwner)
								kCapturingPlayer.DoCivilianReturnLogic(false, kCaptureDef.eOriginalOwner, pkCapturedUnit->GetID());
						}
						// if Venice
						else if (kCapturingPlayer.GetPlayerTraits()->IsNoAnnexing())
						{
							// If the unit originally belonged to us, we've already done what we needed to do
							if(kCaptureDef.eCapturingPlayer != kCaptureDef.eOriginalOwner)
								kCapturingPlayer.DoCivilianReturnLogic(false, kCaptureDef.eOriginalOwner, pkCapturedUnit->GetID());
						}
					}
					else
					{
						// restore combat units at some percentage of their original health
						if (pkCapturedUnit != NULL)
						{
							int iCapturedHealth = (pkCapturedUnit->GetMaxHitPoints() * GC.getCOMBAT_CAPTURE_HEALTH()) / 100;
							pkCapturedUnit->setDamage(iCapturedHealth);
						}
					}

					if(kCaptureDef.eCapturingPlayer == GC.getGame().getActivePlayer())
					{
						CvString strBuffer;
						CvUnitEntry* pkUnitInfo = GC.getUnitInfo(kCaptureDef.eCaptureUnitType);
						if (pkUnitInfo)
						{
							if (kCaptureDef.eOriginalOwner == kCaptureDef.eCapturingPlayer){
								//player recaptured a friendly unit
								strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_RECAPTURED_UNIT", pkUnitInfo->GetTextKey());
							}
							else{
								strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_CAPTURED_UNIT", pkUnitInfo->GetTextKey());
							}
							DLLUI->AddUnitMessage(0, IDInfo(kCaptureDef.eCapturingPlayer, pkCapturedUnit->GetID()), kCaptureDef.eCapturingPlayer, true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, GC.getUnitInfo(eCaptureUnitType)->GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX(), pPlot->getY()*/);
						}
					}
				}
			}
		}
	}

	return pkCapturedUnit;
}

//	----------------------------------------------------------------------------
void CvUnit::doTurn()
{
	VALIDATE_OBJECT
	CvAssertMsg(!IsDead(), "isDead did not return false as expected");

	// Wake unit if skipped last turn
	ActivityTypes eActivityType = GetActivityType();
	bool bHoldCheck = (eActivityType == ACTIVITY_HOLD) && (isHuman() || !getFortifyTurns());
	bool bHealCheck = (eActivityType == ACTIVITY_HEAL) && (!isHuman() || IsAutomated() || !IsHurt());
#if defined(MOD_BUGFIX_UNITS_AWAKE_IN_DANGER)
	if (MOD_BUGFIX_UNITS_AWAKE_IN_DANGER) {
		// Healing units will awaken if they can see an enemy unit
		bHealCheck = bHealCheck || ((eActivityType == ACTIVITY_HEAL) && SentryAlert(true));
	}
#endif
	bool bSentryCheck = (eActivityType == ACTIVITY_SENTRY) && SentryAlert();
	bool bInterceptCheck = eActivityType == ACTIVITY_INTERCEPT && !isHuman();

	if (bHoldCheck || bHealCheck || bSentryCheck || bInterceptCheck)	
	{
		SetActivityType(ACTIVITY_AWAKE);
	}
#if defined(MOD_BALANCE_CORE)
	SetNumTilesRevealedThisTurn(0);
	if((getDomainType() == DOMAIN_AIR) && (eActivityType != ACTIVITY_HEAL) && (eActivityType != ACTIVITY_INTERCEPT) && isHuman() && !IsHurt() && SentryAlert())
	{
		SetActivityType(ACTIVITY_AWAKE);
	}
#endif

	testPromotionReady();

#if defined(MOD_API_PLOT_BASED_DAMAGE)
	if (MOD_API_PLOT_BASED_DAMAGE) {
		// Do nothing, feature damage is included in with the terrain (mountains) damage taken at the end of the turn
	} else { 
#endif
		FeatureTypes eFeature = plot()->getFeatureType();
		if(NO_FEATURE != eFeature)
		{
			if(0 != GC.getFeatureInfo(eFeature)->getTurnDamage())
			{
#if defined(MOD_API_PLOT_BASED_DAMAGE)
				if(GC.getFeatureInfo(eFeature)->getTurnDamage() > 0)
				{
#endif
				changeDamage(GC.getFeatureInfo(eFeature)->getTurnDamage(), NO_PLAYER);
#if defined(MOD_API_PLOT_BASED_DAMAGE)
				}
#endif
			}
		}
#if defined(MOD_API_PLOT_BASED_DAMAGE)
	}
#endif

	// Only increase our Fortification level if we've actually been told to Fortify
	if(IsFortifiedThisTurn())
	{
		changeFortifyTurns(1);
	}

	// Recon unit? If so, he sees what's around him
	if(IsRecon())
	{
		setReconPlot(plot());
	}

	// If we're not busy doing anything with the turn cycle, make the Unit's Flag bright again
	if(GetActivityType() == ACTIVITY_AWAKE)
	{
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitShouldDimFlag(pDllUnit.get(), /*bDim*/ false);
	}

	// If we told our Unit to sleep last turn and it can now Fortify switch states
	if(GetActivityType() == ACTIVITY_SLEEP)
	{
		if(isFortifyable())
		{
			CvUnit::dispatchingNetMessage(true);
			PushMission(CvTypes::getMISSION_FORTIFY());
			CvUnit::dispatchingNetMessage(false);
			SetFortifiedThisTurn(true);
		}
	}

#if defined(MOD_BALANCE_CORE)
	for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(iI);
		CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
		if(pkPromotionInfo)
		{
			if(isHasPromotion(ePromotion) && pkPromotionInfo->PromotionDuration() > 0)
			{
				int iTurnsElapsed = (GC.getGame().getGameTurn() - getTurnPromotionGained(ePromotion));
				if(iTurnsElapsed > getPromotionDuration(ePromotion))
				{
					setHasPromotion(ePromotion, false);
				}
			}
		}
	}				
#endif

	doDelayedDeath();
}

//	--------------------------------------------------------------------------------
bool CvUnit::isActionRecommended(int iAction)
{
	VALIDATE_OBJECT

	BuildTypes eBuild;

	if(getOwner() != GC.getGame().getActivePlayer())
	{
		return false;
	}

	CvPlot* pPlot = NULL;

	if(pPlot == NULL)
	{
		if(gDLL->shiftKey())
		{
			pPlot = LastMissionPlot();
		}
	}

	if(pPlot == NULL)
	{
		pPlot = plot();
	}

	if(GC.getActionInfo(iAction)->getMissionType() == CvTypes::getMISSION_HEAL())
	{
		if(IsHurt())
		{
			if(!hasMoved())
			{
				if((pPlot->getTeam() == getTeam()) || (healTurns(pPlot) < 4))
				{
					return true;
				}
			}
		}
	}

	if(GC.getActionInfo(iAction)->getMissionType() == CvTypes::getMISSION_FOUND())
	{
		if(canFound(pPlot))
		{
			if(pPlot->isBestAdjacentFound(getOwner()))
			{
				return true;
			}
		}
	}

	if(GC.getActionInfo(iAction)->getMissionType() == CvTypes::getMISSION_BUILD())
	{
		eBuild = ((BuildTypes)(GC.getActionInfo(iAction)->getMissionData()));
		CvAssert(eBuild != NO_BUILD);
		CvAssertMsg(eBuild < GC.getNumBuildInfos(), "Invalid Build");

		const int ciDirectiveSize = 1;
		BuilderDirective aDirective[ ciDirectiveSize ];

		GET_PLAYER(getOwner()).GetBuilderTaskingAI()->EvaluateBuilder(this, aDirective, ciDirectiveSize, false, true);

		if(aDirective[0].m_eDirective != BuilderDirective::NUM_DIRECTIVES && aDirective[0].m_eBuild == eBuild)
		{
			return true;
		}
	}

	if(GC.getActionInfo(iAction)->getCommandType() == COMMAND_PROMOTION)
	{
		return true;
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isBetterDefenderThan(const CvUnit* pDefender, const CvUnit* pAttacker) const
{
	VALIDATE_OBJECT
	int iOurDefense;
	int iTheirDefense;

	if(pDefender == NULL)
	{
		return true;
	}

	TeamTypes eAttackerTeam = NO_TEAM;
	if(NULL != pAttacker)
	{
		eAttackerTeam = pAttacker->getTeam();
	}

	if(canCoexistWithEnemyUnit(eAttackerTeam))
	{
		return false;
	}

	if(!IsCanDefend())
	{
		return false;
	}

	if(IsCanDefend() && !(pDefender->IsCanDefend()))
	{
		return true;
	}

	// Don't let embarked units be best
	if(isEmbarked() && !pDefender->isEmbarked())
	{
		return false;
	}

	if(pAttacker)
	{
		if(getDamage() >= pAttacker->GetCombatLimit() && pDefender->getDamage() < pAttacker->GetCombatLimit())
		{
			return false;
		}

		if(pDefender->getDamage() >= pAttacker->GetCombatLimit() && getDamage() < pAttacker->GetCombatLimit())
		{
			return true;
		}
	}

	iOurDefense = GetMaxDefenseStrength(plot(), pAttacker);
	if(::isWorldUnitClass(getUnitClassType()))
	{
		iOurDefense /= 2;
	}

	if(NULL == pAttacker)
	{
		if(pDefender->currInterceptionProbability() > 0)
		{
			iOurDefense *= (100 + pDefender->currInterceptionProbability());
			iOurDefense /= 100;
		}
	}
	else
	{
		if(!(pAttacker->immuneToFirstStrikes()))
		{
			iOurDefense *= ((((firstStrikes() * 2) + chanceFirstStrikes()) * ((GC.getCOMBAT_DAMAGE() * 2) / 5)) + 100);
			iOurDefense /= 100;
		}

		if(immuneToFirstStrikes())
		{
			iOurDefense *= ((((pAttacker->firstStrikes() * 2) + pAttacker->chanceFirstStrikes()) * ((GC.getCOMBAT_DAMAGE() * 2) / 5)) + 100);
			iOurDefense /= 100;
		}
	}

	iOurDefense /= (getCargo() + 1);

	iTheirDefense = pDefender->GetMaxDefenseStrength(plot(), pAttacker);
	if(::isWorldUnitClass(pDefender->getUnitClassType()))
	{
		iTheirDefense /= 2;
	}

	if(NULL == pAttacker)
	{
		if(currInterceptionProbability() > 0)
		{
			iTheirDefense *= (100 + currInterceptionProbability());
			iTheirDefense /= 100;
		}
	}
	else
	{
		if(!(pAttacker->immuneToFirstStrikes()))
		{
			iTheirDefense *= ((((pDefender->firstStrikes() * 2) + pDefender->chanceFirstStrikes()) * ((GC.getCOMBAT_DAMAGE() * 2) / 5)) + 100);
			iTheirDefense /= 100;
		}

		if(pDefender->immuneToFirstStrikes())
		{
			iTheirDefense *= ((((pAttacker->firstStrikes() * 2) + pAttacker->chanceFirstStrikes()) * ((GC.getCOMBAT_DAMAGE() * 2) / 5)) + 100);
			iTheirDefense /= 100;
		}
	}

	iTheirDefense /= (pDefender->getCargo() + 1);

	if(iOurDefense == iTheirDefense)
	{
		if(NO_UNIT == getLeaderUnitType() && NO_UNIT != pDefender->getLeaderUnitType())
		{
			++iOurDefense;
		}
		else if(NO_UNIT != getLeaderUnitType() && NO_UNIT == pDefender->getLeaderUnitType())
		{
			++iTheirDefense;
		}
		else if(isBeforeUnitCycle(this, pDefender))
		{
			++iOurDefense;
		}
	}

	return (iOurDefense > iTheirDefense);
}


//	--------------------------------------------------------------------------------
bool CvUnit::canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible, bool bTestBusy) const
{
	VALIDATE_OBJECT

	if(bTestBusy && IsBusy())
	{
		return false;
	}

	if (GetAutomateType() == AUTOMATE_TRADE)
		return false;	// No commands for this type.
		
#if defined(MOD_EVENTS_COMMAND)
	if (MOD_EVENTS_COMMAND) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_CanDoCommand, getOwner(), GetID(), eCommand, iData1, iData2, getX(), getY(), bTestVisible) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
#endif

	switch(eCommand)
	{
	case COMMAND_PROMOTION:
		if(canPromote((PromotionTypes)iData1, iData2))
		{
			return true;
		}
		break;

	case COMMAND_UPGRADE:
		if(CanUpgradeRightNow(bTestVisible))
		{
			return true;
		}
		break;

	case COMMAND_AUTOMATE:
		if(CanAutomate((AutomateTypes)iData1, bTestVisible))
		{
			return true;
		}
		break;

	case COMMAND_WAKE:
		if(!IsAutomated() && isWaiting())
		{
			return true;
		}
		break;

	case COMMAND_CANCEL:
	case COMMAND_CANCEL_ALL:
		if(!IsAutomated() && (GetLengthMissionQueue() > 0))
		{
			return true;
		}
		break;

	case COMMAND_STOP_AUTOMATION:
		if(IsAutomated())
		{
			return true;
		}
		break;

	case COMMAND_DELETE:
		if(canScrap(bTestVisible))
		{
			return true;
		}
		break;

	case COMMAND_GIFT:
		if(canGift(bTestVisible))
		{
			return true;
		}
		break;

	case COMMAND_HOTKEY:
		return true;
		break;

	default:
		CvAssert(false);
		break;
	}

	return false;
}


//	--------------------------------------------------------------------------------
void CvUnit::doCommand(CommandTypes eCommand, int iData1, int iData2)
{
	VALIDATE_OBJECT
	bool bCycle;

	bCycle = false;

	CvAssert(getOwner() != NO_PLAYER);

	if(canDoCommand(eCommand, iData1, iData2))
	{
		switch(eCommand)
		{
		case COMMAND_PROMOTION:
			promote((PromotionTypes)iData1, iData2);
			break;

		case COMMAND_UPGRADE:
			DoUpgrade();
			bCycle = true;
			break;

		case COMMAND_AUTOMATE:
			Automate((AutomateTypes)iData1);
			bCycle = true;
			break;

		case COMMAND_WAKE:
			SetActivityType(ACTIVITY_AWAKE);
			if((getOwner() == GC.getGame().getActivePlayer()) && IsSelected())
			{
				// Force the UI to update so they see any new commands
				DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
			}
			break;

		case COMMAND_CANCEL:
			PopMission();
			break;

		case COMMAND_CANCEL_ALL:
			ClearMissionQueue();
			break;

		case COMMAND_STOP_AUTOMATION:
			SetAutomateType(NO_AUTOMATE);
			break;

		case COMMAND_DELETE:
			scrap();
			bCycle = true;
			break;

		case COMMAND_GIFT:
			gift();
			bCycle = true;
			break;

		case COMMAND_HOTKEY:
			setHotKeyNumber(iData1);
			break;

		default:
			CvAssert(false);
			break;
		}
	}

	if(bCycle)
	{
		if(IsSelected())
		{
			DLLUI->setCycleSelectionCounter(1);
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::canEnterTerritory(TeamTypes eTeam, bool bIgnoreRightOfPassage, bool bIsCity, bool bIsDeclareWarMove) const
{
	VALIDATE_OBJECT

	if(eTeam == NO_TEAM)
	{
		return true;
	}

	TeamTypes eMyTeam = GET_PLAYER(getOwner()).getTeam();

	CvTeam& kMyTeam = GET_TEAM(eMyTeam);
	CvTeam& kTheirTeam = GET_TEAM(eTeam);

#if defined(MOD_GLOBAL_CS_OVERSEAS_TERRITORY)
	CvTeam* pTheirTeam = &kTheirTeam;
#endif

	if(kMyTeam.isFriendlyTerritory(eTeam))
	{
		return true;
	}

	if(isEnemy(eTeam))
	{
		return true;
	}

	if(isRivalTerritory())
	{
		return true;
	}

	if(kTheirTeam.isMinorCiv())
	{
		// Minors can't intrude into one another's territory
		if(!kMyTeam.isMinorCiv())
		{
			// If we haven't yet met the Minor we can move in
			// Do we need to do anything special here for human VS AI civs?  AIs might get confused
			if(!kMyTeam.isHasMet(eTeam))
			{
				return true;
			}

			if(bIsCity && bIsDeclareWarMove)
			{
				return false;
			}

#if defined(MOD_GLOBAL_CS_OVERSEAS_TERRITORY)
			CvMinorCivAI* pMinorAI = GET_PLAYER(kTheirTeam.getLeaderID()).GetMinorCivAI();
			PlayerTypes eMinorAlly = pMinorAI->GetAlly();

			// If the minor is allied, treat the plot as being owned by their ally
			if (MOD_GLOBAL_CS_OVERSEAS_TERRITORY && eMinorAlly != NO_PLAYER) {
				if (eMinorAlly == getOwner()) {
					return true;
				}

				pTheirTeam = &GET_TEAM(GET_PLAYER(eMinorAlly).getTeam());
			} else {
#endif
				// The remaining checks are only for AI major vs. AI Minor, humans can always enter a minor's territory.
				if (isHuman())
					return true;

#if !defined(MOD_GLOBAL_CS_OVERSEAS_TERRITORY)
				CvMinorCivAI* pMinorAI = GET_PLAYER(kTheirTeam.getLeaderID()).GetMinorCivAI();
#endif

				// Is this an excluded unit that doesn't cause anger?
				bool bAngerFreeUnit = IsAngerFreeUnit();
				// Player can earn Open Borders with enough Friendship
				bool bHasOpenBorders = pMinorAI->IsPlayerHasOpenBorders(getOwner());
				// If already intruding on this minor, okay to do it some more
				bool bIntruding = pMinorAI->IsMajorIntruding(getOwner());
#if defined(MOD_BALANCE_CORE)
				//Let's let scouts in.
				if(AI_getUnitAIType() == UNITAI_EXPLORE || AI_getUnitAIType() == UNITAI_EXPLORE_SEA)
				{
					return true;
				}
#endif

				if(bAngerFreeUnit || bHasOpenBorders || bIntruding)
				{
					return true;
				}
#if defined(MOD_GLOBAL_CS_OVERSEAS_TERRITORY)
			}
#endif
		}
	}

	if(!bIgnoreRightOfPassage)
	{
#if defined(MOD_GLOBAL_CS_OVERSEAS_TERRITORY)
		if(pTheirTeam->IsAllowsOpenBordersToTeam(eMyTeam))
#else
		if(kTheirTeam.IsAllowsOpenBordersToTeam(eMyTeam))
#endif
		{
			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canEnterTerrain(const CvPlot& enterPlot, int iMoveFlags) const
{
	VALIDATE_OBJECT
	DomainTypes eDomain = getDomainType();

	// Part 1 : Domain specific exclusions -----------------------------------------------

	// Air can go anywhere - provided it's a city or a carrier
	if (eDomain==DOMAIN_AIR)
		return (enterPlot.isCity() && enterPlot.getPlotCity()->getOwner()==getOwner()) || canLoad(enterPlot);

	// Immobile can go nowhere
	if (eDomain==DOMAIN_IMMOBILE || m_bImmobile)
		return false;

	// Sea units - we can exclude non-water plots right away
	if (eDomain==DOMAIN_SEA)
		if (!enterPlot.isWater() && !enterPlot.isCityOrPassableImprovement(getOwner(), false))
			return false;

	// Land units and hover units may go anywhere in principle (with embarkation)
	if (enterPlot.needsEmbarkation(this))
	{
		if (!CanEverEmbark() || (iMoveFlags & MOVEFLAG_NO_EMBARK))
			return false;
	}

	// Part 2 : player traits and unit traits ---------------------------------------------------

	CvPlayer& kPlayer = GET_PLAYER(getOwner());

	// If the plot is impassable, we need to check for positive promotions / traits and their exceptions
	if(enterPlot.isImpassable(getTeam()))
	{
		// Some special units also have it easy
		if(canMoveImpassable() || canMoveAllTerrain())
			return true;

		// if there's a route, anyone can use it
		if(enterPlot.isValidRoute(this))
			return true;

		// Check high seas
		if (enterPlot.isDeepWater() && enterPlot.getFeatureType()==NO_FEATURE)
		{
			if(eDomain == DOMAIN_SEA)
			{
				return canCrossOceans() || kPlayer.CanCrossOcean();
			}
			else if(eDomain == DOMAIN_LAND)
			{
				return IsEmbarkAllWater() || IsEmbarkDeepWater() || kPlayer.CanCrossOcean();
			}
		}

		// Check ice with specialty: is passable if owned
		if (enterPlot.isIce()) 
		{
			bool bCanCross = (canCrossIce() || kPlayer.CanCrossIce() || (eDomain==DOMAIN_SEA && enterPlot.getTeam()==getTeam()));
			return bCanCross;
		}

		// Check mountain.
		if (enterPlot.isMountain())
		{
			bool bCanCross = canCrossMountains() || kPlayer.CanCrossMountain();
			return bCanCross;
		}

		// general promotions ---------------------------------------------------

		if(enterPlot.getFeatureType() != NO_FEATURE && m_Promotions.GetAllowFeaturePassable(enterPlot.getFeatureType()))
		{
			return true;
		}	
		else if(enterPlot.getTerrainType() != NO_TERRAIN && m_Promotions.GetAllowTerrainPassable(enterPlot.getTerrainType()))
		{
			return true;
		}	

		//ok, seems we ran out of jokers. no pasaran!
		return false;
	}
	else //passable. need to check for negative promotions / traits and their exceptions
	{
		if (enterPlot.isDeepWater() && enterPlot.getFeatureType()==NO_FEATURE)
		{
			PromotionTypes ePromotionOceanImpassable = (PromotionTypes)GC.getPROMOTION_OCEAN_IMPASSABLE();
			bool bOceanImpassable =  isHasPromotion(ePromotionOceanImpassable);
			if(bOceanImpassable)
			{
				return false;
			}

			PromotionTypes ePromotionOceanImpassableUntilAstronomy = (PromotionTypes)GC.getPROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY();
			bool bOceanImpassableUntilAstronomy = isHasPromotion(ePromotionOceanImpassableUntilAstronomy) || 
				(eDomain==DOMAIN_LAND && !IsEmbarkDeepWater() && !IsEmbarkAllWater() && !kPlayer.CanCrossOcean());

			if(bOceanImpassableUntilAstronomy )
			{
				CvTeam& kTeam = GET_TEAM(getTeam());
				return kTeam.GetTeamTechs()->HasTech( GC.getGame().getOceanPassableTech() );
			}	
		}

		if(enterPlot.getFeatureType() != NO_FEATURE && isFeatureImpassable(enterPlot.getFeatureType()))
		{
			return false;
		}
		else if(enterPlot.getTerrainType() != NO_TERRAIN && isTerrainImpassable(enterPlot.getTerrainType()))
		{
			return false;
		}

		//ok, seems there are no objections. let's go!
		return true;
	}
}

//	--------------------------------------------------------------------------------
/// Returns the ID of the team to declare war against
TeamTypes CvUnit::GetDeclareWarMove(const CvPlot& plot) const
{
	VALIDATE_OBJECT
	const CvUnit* pUnit;
	TeamTypes eRevealedTeam;

	CvAssert(isHuman());
#if defined(MOD_BALANCE_CORE)
	if(IsAutomated() && isHuman())
	{
		return NO_TEAM;
	}
#endif

	if(getDomainType() != DOMAIN_AIR)
	{
		eRevealedTeam = plot.getRevealedTeam(getTeam(), false);

		if(eRevealedTeam != NO_TEAM)
		{
			if(!GET_TEAM(eRevealedTeam).isMinorCiv() || plot.isCity())
			{
				if(!canEnterTerritory(eRevealedTeam, false /*bIgnoreRightOfPassage*/, plot.isCity(), true))
				{
#if defined(MOD_EVENTS_WAR_AND_PEACE)
					if(GET_TEAM(getTeam()).canDeclareWar(plot.getTeam(), getOwner()))
#else
					if(GET_TEAM(getTeam()).canDeclareWar(plot.getTeam()))
#endif
					{
						return eRevealedTeam;
					}
				}
			}
		}
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
		if(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS && plot.getNumUnits() > 0)
		{
			int iPeaceUnits = 0;
			pUnit = NULL;
			bool bWarCity = false;
			if(plot.isCity() && GET_TEAM(GET_PLAYER(plot.getOwner()).getTeam()).isAtWar(getTeam()))
			{
				bWarCity = true;
			}
			for(int iUnitLoop = 0; iUnitLoop < plot.getNumUnits(); iUnitLoop++)
			{
				CvUnit* loopUnit = plot.getUnitByIndex(iUnitLoop);

				//If we're at war with a civ, and they've got a unit here, let's return that unit instead of the civilian unit on this space.
				if(loopUnit != NULL)
				{
					if(loopUnit->IsCombatUnit())
					{
						if(GET_TEAM(getTeam()).isAtWar(loopUnit->getTeam()))
						{
							//There can be only one military unit on a tile, so one check is good enough.
							pUnit = plot.getUnitByIndex(iUnitLoop);
						}
					}
					//Exception for embarked units in water, civilian or not, as boats can move onto them as a stack (military units are pseudo-units)
					else if(!bWarCity && loopUnit->isEmbarked() && getDomainType() == DOMAIN_SEA)
					{
						pUnit = plot.getUnitByIndex(iUnitLoop);
						if(!GET_TEAM(getTeam()).isAtWar(loopUnit->getTeam()))
						{
							iPeaceUnits++;
						}
					}
					else if(loopUnit->IsCivilianUnit())
					{
						//Only need one to trigger.
						if(!GET_TEAM(getTeam()).isAtWar(loopUnit->getTeam()))
						{
							iPeaceUnits++;
						}
					}
				}
			}
			//If there is a civlian and an enemy unit here (or this is an enemy city), but we're at peace with that civilian, return NO_TEAM.
			if((iPeaceUnits > 0) && (bWarCity || (pUnit != NULL)))
			{
				return NO_TEAM;
			}
		}
#endif

		if(plot.isActiveVisible(false))
		{
			if(canMoveInto(plot, MOVEFLAG_ATTACK | MOVEFLAG_DECLARE_WAR))
			{
				pUnit = plot.plotCheck(PUF_canDeclareWar, getOwner(), isAlwaysHostile(plot), NO_PLAYER, NO_TEAM, PUF_isVisible, getOwner());

				if(pUnit != NULL)
				{
					return pUnit->getTeam();
				}
			}
		}
	}

	return NO_TEAM;
}

//	--------------------------------------------------------------------------------
/// Returns the ID of the minor we'd be bullying with this move
PlayerTypes CvUnit::GetBullyMinorMove(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	PlayerTypes eMinor;

	CvAssert(isHuman());

	if(getDomainType() != DOMAIN_AIR)
	{
		eMinor = pPlot->getRevealedOwner(getTeam());

		if(eMinor != NO_PLAYER)
		{
			if(GET_PLAYER(eMinor).isMinorCiv())
			{
				if(!canEnterTerritory(GET_PLAYER(eMinor).getTeam(), false /*bIgnoreRightOfPassage*/, pPlot->isCity()))
				{
					return eMinor;
				}
			}
		}
	}

	return NO_PLAYER;
}

//	--------------------------------------------------------------------------------
/// Returns the ID of the team to declare war against
TeamTypes CvUnit::GetDeclareWarRangeStrike(const CvPlot& plot) const
{
	VALIDATE_OBJECT
	const CvUnit* pUnit;

	CvAssert(isHuman());

	if(plot.isActiveVisible(false))
	{
		if(canRangeStrikeAt(plot.getX(), plot.getY(), false))
		{
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
			if(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS && plot.getNumUnits() > 0)
			{
				pUnit = NULL;
				int iPeaceUnits = 0;
				bool bWarCity = false;
				if(plot.isCity() && GET_TEAM(GET_PLAYER(plot.getOwner()).getTeam()).isAtWar(getTeam()))
				{
					bWarCity = true;
				}
				for(int iUnitLoop = 0; iUnitLoop < plot.getNumUnits(); iUnitLoop++)
				{
					CvUnit* loopUnit = plot.getUnitByIndex(iUnitLoop);

					//If we're at war with a civ, and they've got a unit here, let's return that unit instead of the civilian unit on this space.
					if(loopUnit != NULL)
					{
						if(loopUnit->IsCombatUnit())
						{
							if(GET_TEAM(getTeam()).isAtWar(loopUnit->getTeam()))
							{
								//There can be only one military unit on a tile, so one check is good enough.
								pUnit = plot.getUnitByIndex(iUnitLoop);
							}
						}
						else if(loopUnit->IsCivilianUnit())
						{
							//Only need one to trigger.
							if(!GET_TEAM(getTeam()).isAtWar(loopUnit->getTeam()))
							{
								iPeaceUnits++;
							}
						}
					}
				}
				//If there is a civlian and an enemy unit here (or this is an enemy city), but we're at peace with that civilian, return NO_TEAM.
				if((iPeaceUnits > 0) && (bWarCity || (pUnit != NULL)))
				{
					return NO_TEAM;
				}
			}
#endif
			pUnit = plot.plotCheck(PUF_canDeclareWar, getOwner(), isAlwaysHostile(plot), NO_PLAYER, NO_TEAM, PUF_isVisible, getOwner());

			if(pUnit != NULL)
			{
				return pUnit->getTeam();
			}
			// Check for City as well
			else
			{
				if(plot.isCity())
				{
#if defined(MOD_EVENTS_WAR_AND_PEACE)
					if(GET_TEAM(getTeam()).canDeclareWar(plot.getTeam(), getOwner()))
#else
					if(GET_TEAM(getTeam()).canDeclareWar(plot.getTeam()))
#endif
					{
						return plot.getTeam();
					}
				}
			}
		}
	}

	return NO_TEAM;
}

//	--------------------------------------------------------------------------------
bool CvUnit::willRevealByMove(const CvPlot& plot) const
{
	VALIDATE_OBJECT
	int iVisRange = visibilityRange();
	TeamTypes eTeam = getTeam();
	int iRange = iVisRange + 1;
	for(int i = -iRange; i <= iRange; ++i)
	{
		for(int j = -iRange; j <= iRange; ++j)
		{
			CvPlot* pLoopPlot = ::plotXYWithRangeCheck(plot.getX(), plot.getY(), i, j, iRange);
			if(NULL != pLoopPlot)
			{
				if(!pLoopPlot->isRevealed(eTeam) && plot.canSeePlot(pLoopPlot, eTeam, iVisRange, NO_DIRECTION))
				{
					return true;
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canMoveInto(const CvPlot& plot, int iMoveFlags) const
{
	VALIDATE_OBJECT
	TeamTypes ePlotTeam;

	//nothing to do
	if(atPlot(plot))
	{
		return true;
	}

	// Cannot move around in unrevealed land freely
	if(!(iMoveFlags & CvUnit::MOVEFLAG_PRETEND_UNEMBARKED) && isNoRevealMap() && willRevealByMove(plot))
	{
		return false;
	}

	// Barbarians have special restrictions early in the game
	if(isBarbarian() && (GC.getGame().getGameTurn() < GC.getGame().GetBarbarianReleaseTurn()) && (plot.isOwned()))
	{
		return false;
	}

	// Added in Civ 5: Destination plots can't allow stacked Units of the same type
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
	if((iMoveFlags & CvUnit::MOVEFLAG_DESTINATION) && (!MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS || !IsCivilianUnit()))
#else
	if(iMoveFlags & CvUnit::MOVEFLAG_DESTINATION)
#endif
	{
		// Don't let another player's unit inside someone's city
		if(!(iMoveFlags & CvUnit::MOVEFLAG_ATTACK) && !(iMoveFlags & CvUnit::MOVEFLAG_DECLARE_WAR))
		{
			if(plot.isCity() && plot.getPlotCity()->getOwner() != getOwner())
				return false;
		}

		// Check to see if any units are present at this full-turn move plot (borrowed from CvGameCoreUtils::pathDestValid())
#if defined(MOD_GLOBAL_STACKING_RULES)
		if(!(iMoveFlags & CvUnit::MOVEFLAG_IGNORE_STACKING) && plot.getUnitLimit() > 0)
#else
		if(!(iMoveFlags & CvUnit::MOVEFLAG_IGNORE_STACKING) && GC.getPLOT_UNIT_LIMIT() > 0)
#endif
		{
			// pSelectionGroup has no Team but the HeadUnit does... ???
#if defined(MOD_GLOBAL_STACKING_RULES)
			if(plot.isVisible(getTeam()) && plot.getNumFriendlyUnitsOfType(this) >= plot.getUnitLimit())
#else
			if(plot.isVisible(getTeam()) && plot.getNumFriendlyUnitsOfType(this) >= GC.getPLOT_UNIT_LIMIT())
#endif
			{
				return FALSE;
			}
		}
	}

	if(isNoCapture())
	{
		if(!(iMoveFlags & CvUnit::MOVEFLAG_ATTACK))
		{
			if(plot.isEnemyCity(*this))
			{
				return false;
			}
		}
	}

	if(iMoveFlags & CvUnit::MOVEFLAG_ATTACK)
	{
		if(isOutOfAttacks())
		{
#if defined(MOD_GLOBAL_CAPTURE_AFTER_ATTACKING)
			// If there are only enemy civilians in the plot, then we don't need remaining attacks to capture them
			// eg a worker in a barbarian camp where we just killed the defender
			if (plot.getBestDefender(NO_PLAYER) != ((UnitHandle) NULL))
			{
				return false;
			}
#else
			return false;
#endif
		}

		// Does unit only attack cities?
		if(IsCityAttackOnly() && !plot.isEnemyCity(*this) && plot.getBestDefender(NO_PLAYER))
		{
			return false;
		}
	}

	// Can't enter an enemy city until it's "defeated"
	if(plot.isEnemyCity(*this))
	{
		if(plot.getPlotCity()->getDamage() < plot.getPlotCity()->GetMaxHitPoints() && !(iMoveFlags & CvUnit::MOVEFLAG_ATTACK))
		{
			return false;
		}
		if(iMoveFlags & CvUnit::MOVEFLAG_ATTACK)
		{
			if(getDomainType() == DOMAIN_AIR)
				return false;
			if(isHasPromotion((PromotionTypes)GC.getPROMOTION_ONLY_DEFENSIVE()))
				return false;	// Can't advance into an enemy city
		}
	}

	if(getDomainType() == DOMAIN_AIR)
	{
		if(iMoveFlags & CvUnit::MOVEFLAG_ATTACK)
		{
			if(!canRangeStrikeAt(plot.getX(), plot.getY()))
			{
				return false;
			}
		}
	}
	else
	{
		if(iMoveFlags & CvUnit::MOVEFLAG_ATTACK)
		{
			if(!IsCanAttack())  // trying to give an attack order to a unit that can't fight. That doesn't work!
			{
				return false;
			}

			// can't embark and attack in one go. however, disembark to attack is allowed
			if(CanEverEmbark() && plot.needsEmbarkation(this))
			{
				return false;
			}

			if(!isHuman() || (plot.isVisible(getTeam())))
			{
				// This stuff to the next if statement is to get units to advance into a tile with an enemy if that enemy is dying...
				bool bCanAdvanceOnDeadUnit = false;

				const IDInfo* pUnitNode = plot.headUnitNode();
				const CvUnit* pLoopUnit;

				bool bPlotContainsCombat = false;
				while(pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(*pUnitNode);
					CvAssertMsg(pLoopUnit, "pUnitNode data should lead to a unit");

					pUnitNode = plot.nextUnitNode(pUnitNode);

					// NULL check is always a good thing to do.
					if(pLoopUnit)
					{
						if(GET_TEAM(getTeam()).isAtWar(GET_PLAYER(pLoopUnit->getOwner()).getTeam()))
						{
							if(!pLoopUnit->IsDead() && pLoopUnit->isInCombat())
							{
								if(pLoopUnit->getCombatUnit() != this)
									bPlotContainsCombat = true;
							}

							if(pLoopUnit->IsDead() || !pLoopUnit->IsCombatUnit())
							{
								bCanAdvanceOnDeadUnit = true;
							}
							else
							{
								bCanAdvanceOnDeadUnit = false;
								break;
							}
						}
					}
				}

				if(bPlotContainsCombat)
				{
					return false;		// Can't enter a plot that contains combat that doesn't involve us.
				}

				if(!bCanAdvanceOnDeadUnit)
				{
					if(plot.isVisibleEnemyUnit(this) != (iMoveFlags & CvUnit::MOVEFLAG_ATTACK))
					{
						// Prevent an attack from failing if a city is empty but still an "enemy" capable of being attacked (this wouldn't happen before in Civ 4)
						if(!(iMoveFlags & CvUnit::MOVEFLAG_ATTACK) || !plot.isEnemyCity(*this))
						{
							if(!(iMoveFlags & CvUnit::MOVEFLAG_DECLARE_WAR) || (plot.isVisibleOtherUnit(getOwner()) != (iMoveFlags & CvUnit::MOVEFLAG_ATTACK) && !((iMoveFlags & CvUnit::MOVEFLAG_ATTACK) && plot.getPlotCity() && !isNoCapture())))
							{
								return false;
							}
						}
					}
				}
			}
		}
		else //if !(iMoveFlags & CvUnit::MOVEFLAG_ATTACK)
		{
			bool bEmbarkedAndAdjacent = false;
			bool bEnemyUnitPresent = false;

			// Without this code, Embarked Units can move on top of enemies because they have no visibility
			if(isEmbarked() || (iMoveFlags & CvUnit::MOVEFLAG_PRETEND_EMBARKED))
			{
				if(plotDistance(getX(), getY(), plot.getX(), plot.getY()) == 1)
				{
					bEmbarkedAndAdjacent = true;
				}
			}

			bool bPlotContainsCombat = false;
			if(plot.getNumUnits())
			{
				for(int iUnitLoop = 0; iUnitLoop < plot.getNumUnits(); iUnitLoop++)
				{
					CvUnit* loopUnit = plot.getUnitByIndex(iUnitLoop);

					if(loopUnit && GET_TEAM(getTeam()).isAtWar(plot.getUnitByIndex(iUnitLoop)->getTeam()))
					{
						bEnemyUnitPresent = true;
						if(!loopUnit->IsDead() && loopUnit->isInCombat())
						{
							if(loopUnit->getCombatUnit() != this)
								bPlotContainsCombat = true;
						}
						break;
					}
				}
			}

			if(bPlotContainsCombat)
			{
				return false;		// Can't enter a plot that contains combat that doesn't involve us.
			}

			if(!isHuman() || plot.isVisible(getTeam()) || bEmbarkedAndAdjacent)
			{
				if(plot.isEnemyCity(*this))
				{
					return false;
				}

				if(plot.isVisibleEnemyUnit(this) || (bEmbarkedAndAdjacent && bEnemyUnitPresent))
				{
					return false;
				}
			}
		}

		ePlotTeam = ((isHuman()) ? plot.getRevealedTeam(getTeam()) : plot.getTeam());

		if(!canEnterTerritory(ePlotTeam, false /*bIgnoreRightOfPassage*/, plot.isCity(), iMoveFlags & CvUnit::MOVEFLAG_DECLARE_WAR))
		{
			CvAssert(ePlotTeam != NO_TEAM);

#if defined(MOD_EVENTS_WAR_AND_PEACE)
			if(!(GET_TEAM(getTeam()).canDeclareWar(ePlotTeam, getOwner())))
#else
			if(!(GET_TEAM(getTeam()).canDeclareWar(ePlotTeam)))
#endif
			{
				return false;
			}

			if(isHuman())
			{
				if(!(iMoveFlags & CvUnit::MOVEFLAG_DECLARE_WAR))
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	}

#if defined(MOD_EVENTS_CAN_MOVE_INTO)
	if (MOD_EVENTS_CAN_MOVE_INTO && m_pUnitInfo->IsSendCanMoveIntoEvent()) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_CanMoveInto, getOwner(), GetID(), plot.getX(), plot.getY(), ((iMoveFlags & CvUnit::MOVEFLAG_ATTACK) != 0), ((iMoveFlags & CvUnit::MOVEFLAG_DECLARE_WAR) != 0)) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
#endif

	// Make sure we can enter the terrain.  Somewhat expensive call, so we do this last.
	if(!canEnterTerrain(plot, iMoveFlags))
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canMoveOrAttackInto(const CvPlot& plot, int iMoveFlags) const
{
	VALIDATE_OBJECT
	return (canMoveInto(plot, iMoveFlags & ~CvUnit::MOVEFLAG_ATTACK) || canMoveInto(plot, iMoveFlags | CvUnit::MOVEFLAG_ATTACK));
}

//	--------------------------------------------------------------------------------
/// Does this unit not
bool CvUnit::IsAngerFreeUnit() const
{
	// If it's a "rival territory" Unit (e.g. Caravel) don't worry about it
	if(isRivalTerritory())
		return true;

	// Don't care about noncombat units
	if(!IsCombatUnit())
		return true;

	// We don't care about other Minors or the Barbs
	if(GET_PLAYER(getOwner()).isMinorCiv())
		return true;

	// We don't care about other Minors or the Barbs
	if(GET_PLAYER(getOwner()).isBarbarian())
		return true;

	return false;
}

//	---------------------------------------------------------------------------
int CvUnit::getCombatDamage(int iStrength, int iOpponentStrength, int iCurrentDamage, bool bIncludeRand, bool bAttackerIsCity, bool bDefenderIsCity) const
{
	VALIDATE_OBJECT
	// The roll will vary damage between 40 and 60 (out of 100) for two units of identical strength

	int iDamageRatio;

	int iWoundedDamageMultiplier = /*50*/ GC.getWOUNDED_DAMAGE_MULTIPLIER();

	if(bAttackerIsCity)
	{
		iDamageRatio = GC.getMAX_HIT_POINTS();		// JON: Cities don't do less damage when wounded
	}
	else
	{
		// Mod (Policies, etc.)
		iWoundedDamageMultiplier += GET_PLAYER(getOwner()).GetWoundedUnitDamageMod();

		iDamageRatio = GC.getMAX_HIT_POINTS() - (iCurrentDamage * iWoundedDamageMultiplier / 100);
	}

	int iDamage = 0;

	iDamage = /*400*/ GC.getATTACK_SAME_STRENGTH_MIN_DAMAGE() * iDamageRatio / GC.getMAX_HIT_POINTS();

	// Don't use rand when calculating projected combat results
	int iRoll = 0;
	if(bIncludeRand)
	{
		iRoll = /*400*/ GC.getGame().getJonRandNum(GC.getATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE(), "Unit Combat Damage");
		iRoll *= iDamageRatio;
		iRoll /= GC.getMAX_HIT_POINTS();
	}
	else
	{
		iRoll = /*400*/ GC.getATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE();
		iRoll -= 1;	// Subtract 1 here, because this is the amount normally "lost" when doing a rand roll
		iRoll *= iDamageRatio;
		iRoll /= GC.getMAX_HIT_POINTS();
		iRoll /= 2;	// The divide by 2 is to provide the average damage
	}
	iDamage += iRoll;

	// Calculations performed to dampen amount of damage by units that are close in strength
	// RATIO = (((((ME / OPP) + 3) / 4) ^ 4) + 1) / 2
	// Examples:
	// 1.301 = (((((9 / 6) + 3) / 4) ^ 4) + 1 / 2
	// 17.5 = (((((40 / 6) + 3) / 4) ^ 4) + 1 / 2

	double fStrengthRatio = (double(iStrength) / iOpponentStrength);

	// In case our strength is less than the other guy's, we'll do things in reverse then make the ratio 1 over the result (we need a # above 1.0)
	if(iOpponentStrength > iStrength)
	{
		fStrengthRatio = (double(iOpponentStrength) / iStrength);
	}

	fStrengthRatio = (fStrengthRatio + 3) / 4;
	fStrengthRatio = pow(fStrengthRatio, 4.0);
#if defined(MOD_BALANCE_CORE)
	//avoid overflows later ...
	fStrengthRatio = MIN(1e3, (fStrengthRatio + 1) / 2);
#else
	fStrengthRatio = (fStrengthRatio + 1) / 2;
#endif

	if(iOpponentStrength > iStrength)
	{
		fStrengthRatio = 1 / fStrengthRatio;
	}

	iDamage = int(iDamage * fStrengthRatio);

	// Modify damage for when a city "attacks" a unit
	if(bAttackerIsCity)
	{
		iDamage *= /*50*/ GC.getCITY_ATTACKING_DAMAGE_MOD();
		iDamage /= 100;
	}

	// Modify damage for when unit is attacking a city
	if(bDefenderIsCity)
	{
		iDamage *= /*100*/ GC.getATTACKING_CITY_MELEE_DAMAGE_MOD();
		iDamage /= 100;
	}

	// Bring it back out of hundreds
	iDamage /= 100;

	iDamage = iDamage > 0 ? iDamage : 1;

	return iDamage;
}

//	--------------------------------------------------------------------------------
void CvUnit::fightInterceptor(const CvPlot& pPlot)
{
	VALIDATE_OBJECT
	CvAssert(getCombatTimer() == 0);

	setAttackPlot(&pPlot, true);
}

//	--------------------------------------------------------------------------------
void CvUnit::move(CvPlot& targetPlot, bool bShow)
{
	VALIDATE_OBJECT
	CvAssert(canMoveOrAttackInto(targetPlot) || isOutOfAttacks());

	CvPlot* pOldPlot = plot();
	CvAssertMsg(pOldPlot, "pOldPlot needs to have a value");

	bool bShouldDeductCost = true;
	int iMoveCost = targetPlot.movementCost(this, plot());

	// we need to get our dis/embarking on
	bool bChangeEmbarkedState = CanEverEmbark() && (targetPlot.needsEmbarkation(this) != pOldPlot->needsEmbarkation(this));
	if (bChangeEmbarkedState)
	{
		if(isEmbarked() && !targetPlot.needsEmbarkation(this)) // moving from water to the land
		{
			if (m_unitMoveLocs.size())	// If we have some queued moves, execute them now, so that the disembark is done at the proper location visually
				PublishQueuedVisualizationMoves();

			disembark(pOldPlot);

#if defined(MOD_BALANCE_CORE_EMBARK_CITY_NO_COST)
			TeamTypes eUnitTeam = getTeam();
			CvTeam& kUnitTeam = GET_TEAM(eUnitTeam);
			UnitHandle pUnit = GET_PLAYER(getOwner()).getUnit(GetID());
			//If city, and player has disembark to city at reduced cost...
			if(MOD_BALANCE_CORE_EMBARK_CITY_NO_COST && targetPlot.isCity() && (targetPlot.getOwner() == getOwner()) && kUnitTeam.isCityNoEmbarkCost())
			{
				if(movesLeft() > (baseMoves(DOMAIN_LAND) * GC.getMOVE_DENOMINATOR()))
				{
					setMoves(baseMoves(DOMAIN_LAND) * GC.getMOVE_DENOMINATOR());
				}
			}
			else if(MOD_BALANCE_CORE_EMBARK_CITY_NO_COST && targetPlot.isCity() && (targetPlot.getOwner() == getOwner()) && kUnitTeam.isCityLessEmbarkCost())
			{
				changeMoves(-iMoveCost);
			}
			else if(!GET_PLAYER(getOwner()).GetPlayerTraits()->IsEmbarkedToLandFlatCost())
			{
				finishMoves();
			}
#else
			finishMoves();
#endif
		}
		else if(!isEmbarked() && targetPlot.needsEmbarkation(this))  // moving from land to the water
		{
			if (m_unitMoveLocs.size())	// If we have some queued moves, execute them now, so that the embark is done at the proper location visually
				PublishQueuedVisualizationMoves();

			embark(pOldPlot);
#if defined(MOD_BALANCE_CORE_EMBARK_CITY_NO_COST)
			TeamTypes eUnitTeam = getTeam();
			CvTeam& kUnitTeam = GET_TEAM(eUnitTeam);
			UnitHandle pUnit = GET_PLAYER(getOwner()).getUnit(GetID());
			//If city, and player has disembark to city at reduced cost...
			if(MOD_BALANCE_CORE_EMBARK_CITY_NO_COST && pOldPlot->isCity() && (pOldPlot->getOwner() == getOwner()) && kUnitTeam.isCityNoEmbarkCost())
			{
				if(movesLeft() > (baseMoves(DOMAIN_SEA) * GC.getMOVE_DENOMINATOR()))
				{
					setMoves(baseMoves(DOMAIN_SEA) * GC.getMOVE_DENOMINATOR());
				}
			}
			else if(MOD_BALANCE_CORE_EMBARK_CITY_NO_COST && pOldPlot->isCity() && (pOldPlot->getOwner() == getOwner()) && kUnitTeam.isCityLessEmbarkCost())
			{
				changeMoves(-iMoveCost);
			}
			else
			{
				finishMoves();
			}
#else
			finishMoves();
#endif
			bShouldDeductCost = false;
		}
	}

	if(bShouldDeductCost)
		changeMoves(-iMoveCost);
	setXY(targetPlot.getX(), targetPlot.getY(), true, true, bShow && targetPlot.isVisibleToWatchingHuman(), bShow);
}


//	--------------------------------------------------------------------------------
bool CvUnit::jumpToNearestValidPlot()
{
	VALIDATE_OBJECT
	CvAssertMsg(!isAttacking(), "isAttacking did not return false as expected");
	CvAssertMsg(!isFighting(), "isFighting did not return false as expected");

	CvCity* pNearestCity = GC.getMap().findCity(getX(), getY(), getOwner());

	int iBestValue = INT_MAX;
	CvPlot* pBestPlot = NULL;

	for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);

		if(pLoopPlot && pLoopPlot->isValidDomainForLocation(*this) && isMatchingDomain(pLoopPlot))
		{
			if(canMoveInto(*pLoopPlot))
			{
#if defined(MOD_GLOBAL_STACKING_RULES)
				if(pLoopPlot->getNumFriendlyUnitsOfType(this) < pLoopPlot->getUnitLimit())
#else
				if(pLoopPlot->getNumFriendlyUnitsOfType(this) < GC.getPLOT_UNIT_LIMIT())
#endif
				{
					// Can only jump to a plot if we can enter the territory, and it's NOT enemy territory OR we're a barb
					if(canEnterTerritory(pLoopPlot->getTeam()) && (isBarbarian() || !isEnemy(pLoopPlot->getTeam(), pLoopPlot)) && !pLoopPlot->isMountain())
					{
						CvAssertMsg(!atPlot(*pLoopPlot), "atPlot(pLoopPlot) did not return false as expected");

						if((getDomainType() != DOMAIN_AIR) || pLoopPlot->isFriendlyCity(*this, true))
						{
							if(getDomainType() != DOMAIN_SEA || (pLoopPlot->isFriendlyCity(*this, true) && pLoopPlot->isCoastalLand()) || (pLoopPlot->isWater() && !pLoopPlot->isLake()))
							{
								if(pLoopPlot->isRevealed(getTeam()))
								{
									int iValue = (plotDistance(getX(), getY(), pLoopPlot->getX(), pLoopPlot->getY()) * 2);

									if(pNearestCity != NULL)
									{
										iValue += plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pNearestCity->getX(), pNearestCity->getY());
									}

									if(iValue < iBestValue || (iValue == iBestValue && GC.getGame().getJonRandNum(3,"jump to plot")<2) )
									{
										iBestValue = iValue;
										pBestPlot = pLoopPlot;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strLogString;
		if(pBestPlot != NULL)
		{
			strLogString.Format("Jump to nearest valid plot by %s , X: %d, Y: %d, From X: %d, From Y: %d", getName().GetCString(),
								pBestPlot->getX(), pBestPlot->getY(), getX(), getY());
			GET_PLAYER(m_eOwner).GetHomelandAI()->LogHomelandMessage(strLogString);
		}
		else
		{
			strLogString.Format("Can't find a valid plot within range. %s deleted, X: %d, Y: %d", getName().GetCString(), getX(), getY());
			GET_PLAYER(m_eOwner).GetHomelandAI()->LogHomelandMessage(strLogString);
		}
	}

	if(pBestPlot != NULL)
	{
		setXY(pBestPlot->getX(), pBestPlot->getY());
		ClearMissionQueue();
		SetActivityType(ACTIVITY_AWAKE);
	}
	else
	{
		CUSTOMLOG("jumpToNearestValidPlot failed for unit %s at plot (%i, %i)", getName().GetCString(), getX(), getY());
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::jumpToNearestValidPlotWithinRange(int iRange)
{
	VALIDATE_OBJECT
	CvPlot* pLoopPlot;
	CvPlot* pBestPlot;
	int iValue;
	int iBestValue;

	CvAssertMsg(!isAttacking(), "isAttacking did not return false as expected");
	CvAssertMsg(!isFighting(), "isFighting did not return false as expected");

	iBestValue = INT_MAX;
	pBestPlot = NULL;

	for(int iDX = -iRange; iDX <= iRange; iDX++)
	{
		for(int iDY = -iRange; iDY <= iRange; iDY++)
		{
			pLoopPlot	= plotXYWithRangeCheck(getX(), getY(), iDX, iDY, iRange);

			if(pLoopPlot != NULL)
			{
				if(pLoopPlot->isValidDomainForLocation(*this) && isMatchingDomain(pLoopPlot))
				{
					if(canMoveInto(*pLoopPlot))
					{
#if defined(MOD_GLOBAL_STACKING_RULES)
						if(pLoopPlot->getNumFriendlyUnitsOfType(this) < pLoopPlot->getUnitLimit())
#else
						if(pLoopPlot->getNumFriendlyUnitsOfType(this) < GC.getPLOT_UNIT_LIMIT())
#endif
						{
							// Can only jump to a plot if we can enter the territory, and it's NOT enemy territory OR we're a barb
							if(canEnterTerritory(pLoopPlot->getTeam()) && (isBarbarian() || !isEnemy(pLoopPlot->getTeam(), pLoopPlot)))
							{
								CvAssertMsg(!atPlot(*pLoopPlot), "atPlot(pLoopPlot) did not return false as expected");

								if((getDomainType() != DOMAIN_AIR) || pLoopPlot->isFriendlyCity(*this, true))
								{
									if(pLoopPlot->isRevealed(getTeam()))
									{
										iValue = (plotDistance(getX(), getY(), pLoopPlot->getX(), pLoopPlot->getY()) * 2);

										if(pLoopPlot->area() != area())
										{
											iValue *= 3;
										}

										if(iValue < iBestValue || (iValue == iBestValue && GC.getGame().getJonRandNum(3,"jump to plot")<2) )
										{
											iBestValue = iValue;
											pBestPlot = pLoopPlot;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if(pBestPlot != NULL)
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Jump to nearest valid plot within range by %s , X: %d, Y: %d, From X: %d, From Y: %d", getName().GetCString(),
								pBestPlot->getX(), pBestPlot->getY(), getX(), getY());
			GET_PLAYER(m_eOwner).GetHomelandAI()->LogHomelandMessage(strLogString);
		}
		setXY(pBestPlot->getX(), pBestPlot->getY(), false, true, pBestPlot->isVisibleToWatchingHuman(), false);
	}
	else
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Can't find a valid plot within range for %s, X: %d, Y: %d", getName().GetCString(), getX(), getY());
			GET_PLAYER(m_eOwner).GetHomelandAI()->LogHomelandMessage(strLogString);
		}
		CUSTOMLOG("jumpToNearestValidPlotWithinRange(%i) failed for unit %s at plot (%i, %i)", iRange, getName().GetCString(), getX(), getY());
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::CanAutomate(AutomateTypes eAutomate, bool bTestVisibility) const
{
	VALIDATE_OBJECT
	if(eAutomate == NO_AUTOMATE)
	{
		return false;
	}
#if defined(MOD_BALANCE_CORE)
	if(eAutomate == 2)
	{
		if ((getDomainType() == DOMAIN_AIR) || (getDomainType() == DOMAIN_IMMOBILE))
		{
			return false;
		}
		if(AI_getUnitAIType() != UNITAI_MISSIONARY)
		{
			return false;
		}

		if (!bTestVisibility)
		{
			int iTargetTurns;
			UnitHandle pUnit = GET_PLAYER(m_eOwner).getUnit(GetID());
			if(pUnit)
			{
				CvPlot* pTarget = GET_PLAYER(m_eOwner).GetReligionAI()->ChooseMissionaryTargetPlot(pUnit, &iTargetTurns);
				if(pTarget == NULL)
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		if (!bTestVisibility)
		{
			if (GC.getUNIT_AUTO_EXTRA_AUTOMATIONS_DISABLED() == 1)
			{
				return false;
			}
		}
	}
	if(eAutomate == 3)
	{
		if ((getDomainType() == DOMAIN_AIR) || (getDomainType() == DOMAIN_IMMOBILE))
		{
			return false;
		}
		if(AI_getUnitAIType() != UNITAI_ARCHAEOLOGIST)
		{
			return false;
		}

		if (!bTestVisibility)
		{
			CvUnit* pUnit = GET_PLAYER(getOwner()).getUnit(GetID());
			if(pUnit)
			{
				if(!GET_PLAYER(m_eOwner).GetHomelandAI()->FindTestArchaeologistPlotPrimer(pUnit))
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		if (!bTestVisibility)
		{
			if (GC.getUNIT_AUTO_EXTRA_AUTOMATIONS_DISABLED() == 1)
			{
				return false;
			}
		}
	}
#endif
#if defined(MOD_DIPLOMACY_CITYSTATES)
	if(eAutomate == 4)
	{
		if ((getDomainType() == DOMAIN_AIR) || (getDomainType() == DOMAIN_IMMOBILE))
		{
			return false;
		}
		if(AI_getUnitAIType() != UNITAI_MESSENGER)
		{
			return false;
		}

		if (!bTestVisibility)
		{
			UnitHandle pUnit = GET_PLAYER(m_eOwner).getUnit(GetID());
			if(pUnit)
			{
				CvPlot* pTarget = GET_PLAYER(m_eOwner).ChooseMessengerTargetPlot(pUnit);
				if(pTarget == NULL)
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		if (!bTestVisibility)
		{
			if (GC.getUNIT_AUTO_EXTRA_AUTOMATIONS_DISABLED() == 1)
			{
				return false;
			}
		}
	}
#endif
	switch(eAutomate)
	{
	case AUTOMATE_BUILD:
		if((AI_getUnitAIType() != UNITAI_WORKER) && (AI_getUnitAIType() != UNITAI_WORKER_SEA))
		{
			return false;
		}

		if(!bTestVisibility)
		{
			if(GC.getUNIT_WORKER_AUTOMATION_DISABLED() == 1)
			{
				return false;
			}
		}

		break;

	case AUTOMATE_EXPLORE:
		if((GetBaseCombatStrength(true) == 0) || (getDomainType() == DOMAIN_AIR) || (getDomainType() == DOMAIN_IMMOBILE))
		{
			return false;
		}

		if(!bTestVisibility)
		{
#if defined(MOD_CORE_ALTERNATIVE_EXPLORE_SCORE)
			if(GET_PLAYER(m_eOwner).GetEconomicAI()->GetExplorationPlots( getDomainType() ).empty())
#else
			if(!GET_PLAYER(m_eOwner).GetHomelandAI()->IsAnyValidExploreMoves(this))
#endif
			{
				return false;
			}
		}

		if (GC.getUNIT_AUTO_EXPLORE_FULL_DISABLED() == 1)
		{
			return false;
		}

		if(!bTestVisibility)
		{
			if(GC.getUNIT_AUTO_EXPLORE_DISABLED() == 1)
			{
				return false;
			}
		}
		break;
	default:
		CvAssert(false);
		break;
	}

	return true;
}


//	--------------------------------------------------------------------------------
void CvUnit::Automate(AutomateTypes eAutomate)
{
	VALIDATE_OBJECT
	if(!CanAutomate(eAutomate))
	{
		return;
	}

	SetAutomateType(eAutomate);
}


//	--------------------------------------------------------------------------------
bool CvUnit::canScrap(bool bTestVisible) const
{
	VALIDATE_OBJECT
	if(plot()->isUnitFighting())
	{
		return false;
	}

	if(!canMove())
	{
		return false;
	}

	if(!bTestVisible)
	{
		if(GC.getUNIT_DELETE_DISABLED() == 1)
		{
			return false;
		}
	}


	return true;
}


//	--------------------------------------------------------------------------------
void CvUnit::scrap()
{
	VALIDATE_OBJECT
	if(!canScrap())
	{
		return;
	}

	if(plot()->getOwner() == getOwner())
	{
		int iGold = GetScrapGold();
		GET_PLAYER(getOwner()).GetTreasury()->ChangeGold(iGold);
	}

	kill(true);
}

//	--------------------------------------------------------------------------------
/// How much gold do we get?
int CvUnit::GetScrapGold() const
{
	int iNumGold = getUnitInfo().GetProductionCost();

	if(iNumGold <= 0)
		return 0;

	iNumGold *= /*10*/ GC.getDISBAND_UNIT_REFUND_PERCENT();
	iNumGold /= 100;

	// Modify amount based on current health
	iNumGold *= 100 * (GC.getMAX_HIT_POINTS() - getDamage()) / GC.getMAX_HIT_POINTS();
	iNumGold /= 100;


	// slewis - moved this out of the plot check because the game speed should effect all scrap gold calculations, not just the ones that are in the owner's plot
	// Modify for game speed
	iNumGold *= GC.getGame().getGameSpeedInfo().getTrainPercent();
	iNumGold /= 100;


	// Check to see if we are transporting other units and add in their scrap value as well.
	CvPlot* pPlot = plot();
	if(pPlot)
	{
		const IDInfo* pUnitNode = pPlot->headUnitNode();
		while(pUnitNode)
		{
			const CvUnit* pLoopUnit = ::getUnit(*pUnitNode);

			if(pLoopUnit != NULL)
			{
				if(pLoopUnit->getTransportUnit() == this)	// Is this the transport for the search unit?
				{
					iNumGold += pLoopUnit->GetScrapGold();
				}
			}

			pUnitNode = pPlot->nextUnitNode(pUnitNode);
		}
	}

	return iNumGold;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canGift(bool bTestVisible, bool bTestTransport) const
{
	VALIDATE_OBJECT
	const CvPlot* pPlot = plot();
	const CvUnit* pTransport = getTransportUnit();

	if(isDelayedDeath())
		return false;

	if(!(pPlot->isOwned()))
	{
		return false;
	}

	if(pPlot->getOwner() == getOwner())
	{
		return false;
	}

	if(pPlot->isVisibleEnemyUnit(this))
	{
		return false;
	}

	if(pPlot->isVisibleEnemyUnit(pPlot->getOwner()))
	{
		return false;
	}

	if(!pPlot->isValidDomainForLocation(*this) && NULL == pTransport)
	{
		return false;
	}

	// Minors
	if(GET_PLAYER(pPlot->getOwner()).isMinorCiv())
	{
		// No settlers
		if(isFound() || IsFoundAbroad())
			return false;

		// No scouts
		UnitClassTypes eScoutClass = (UnitClassTypes) GC.getInfoTypeForString("UNITCLASS_SCOUT", true);
		if (eScoutClass != NO_UNITCLASS && eScoutClass == getUnitClassType())
			return false;

		// No non-combat units
		if(!IsCombatUnit())
		{
			CvPlayer& kPlayer = GET_PLAYER(m_eOwner);

			// Unless okay by trait
			if(kPlayer.GetPlayerTraits()->GetGreatPersonGiftInfluence() == 0 || !IsGreatPerson())
			{
				return false;
			}
		}
	}

	// No for religious units
	if (getUnitInfo().IsSpreadReligion() || getUnitInfo().IsRemoveHeresy())
	{
			return false;
	}

	if(bTestTransport)
	{
		if(pTransport && pTransport->getTeam() != pPlot->getTeam())
		{
			return false;
		}
	}

	if(!bTestVisible)
	{
		if(GET_TEAM(pPlot->getTeam()).isUnitClassMaxedOut(getUnitClassType(), GET_TEAM(pPlot->getTeam()).getUnitClassMaking(getUnitClassType())))
		{
			return false;
		}

		if(GET_PLAYER(pPlot->getOwner()).isUnitClassMaxedOut(getUnitClassType(), GET_PLAYER(pPlot->getOwner()).getUnitClassMaking(getUnitClassType())))
		{
			return false;
		}
	}

	return !atWar(pPlot->getTeam(), getTeam());
}


//	--------------------------------------------------------------------------------
void CvUnit::gift(bool bTestTransport)
{
	VALIDATE_OBJECT
	IDInfo* pUnitNode;
	CvUnit* pGiftUnit;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;
	CvString strBuffer;
	PlayerTypes eOwner;

	if(!canGift(false, bTestTransport))
	{
		return;
	}

	pPlot = plot();

	pUnitNode = pPlot->headUnitNode();

	while(pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(*pUnitNode);

		if(pLoopUnit && pLoopUnit->getTransportUnit() == this)
		{
			pLoopUnit->kill(false);
			pUnitNode = pPlot->headUnitNode();
		}
		else
		{
			pUnitNode = pPlot->nextUnitNode(pUnitNode);
		}
	}

	CvAssertMsg(plot()->getOwner() != NO_PLAYER, "plot()->getOwner() is not expected to be equal with NO_PLAYER");
	pGiftUnit = GET_PLAYER(plot()->getOwner()).initUnit(getUnitType(), getX(), getY(), AI_getUnitAIType(), NO_DIRECTION, false, false);

	CvAssertMsg(pGiftUnit != NULL, "GiftUnit is not assigned a valid value");

	if(pGiftUnit != NULL)
	{
		eOwner = getOwner();

		pGiftUnit->convert(this, false);
		pGiftUnit->setupGraphical();

		pGiftUnit->GetReligionData()->SetReligion(GetReligionData()->GetReligion());
		pGiftUnit->GetReligionData()->SetReligiousStrength(GetReligionData()->GetReligiousStrength());
		pGiftUnit->GetReligionData()->SetSpreadsLeft(GetReligionData()->GetSpreadsLeft());

		if(pGiftUnit->getOwner() == GC.getGame().getActivePlayer())
		{
			strBuffer = GetLocalizedText("TXT_KEY_MISC_GIFTED_UNIT_TO_YOU", GET_PLAYER(eOwner).getNameKey(), pGiftUnit->getNameKey());
			DLLUI->AddUnitMessage(0, pGiftUnit->GetIDInfo(), pGiftUnit->getOwner(), false, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_UNITGIFTED", MESSAGE_TYPE_INFO, pGiftUnit->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pGiftUnit->getX(), pGiftUnit->getY(), true, true*/);
		}

		CvPlayer* pMinorCiv = &GET_PLAYER(pGiftUnit->getOwner());

		// Gift to Minor Civ
		CvPlayer& kPlayer = GET_PLAYER(getOwner());
		if(!kPlayer.isMinorCiv() && pMinorCiv->isMinorCiv())
		{
			pMinorCiv->GetMinorCivAI()->DoUnitGiftFromMajor(eOwner, pGiftUnit, /*bDistanceGift*/ false);
		}
	}
}

//	--------------------------------------------------------------------------------
// Long-distance gift to a city state
bool CvUnit::CanDistanceGift(PlayerTypes eToPlayer) const
{
	VALIDATE_OBJECT

	if (eToPlayer == NO_PLAYER)
		return false;

	// Minors
	if(GET_PLAYER(eToPlayer).isMinorCiv())
	{
		// No settlers
		if(isFound() || IsFoundAbroad())
			return false;

		// No scouts
		UnitClassTypes eScoutClass = (UnitClassTypes) GC.getInfoTypeForString("UNITCLASS_SCOUT", true);
		if (eScoutClass != NO_UNITCLASS && eScoutClass == getUnitClassType())
			return false;

		// No non-combat units
		if(!IsCombatUnit() && !canAirAttack())
		{
			CvPlayer& kPlayer = GET_PLAYER(m_eOwner);

			// Unless okay by trait
			if(kPlayer.GetPlayerTraits()->GetGreatPersonGiftInfluence() == 0 || !IsGreatPerson())
			{
				return false;
			}
		}

		// Is there a distance gift from us waiting to be delivered?
		if (GET_PLAYER(eToPlayer).GetIncomingUnitType(getOwner()) != NO_UNIT)
		{
			return false;
		}
	}

	// No Majors
	else
		return false;

	TeamTypes eToTeam = GET_PLAYER(eToPlayer).getTeam();

	// Maxed out unit class for team
	if(GET_TEAM(eToTeam).isUnitClassMaxedOut(getUnitClassType(), GET_TEAM(eToTeam).getUnitClassMaking(getUnitClassType())))
		return false;

	// Maxed out unit class for Player
	if(GET_PLAYER(eToPlayer).isUnitClassMaxedOut(getUnitClassType(), GET_PLAYER(eToPlayer).getUnitClassMaking(getUnitClassType())))
		return false;
	
#if defined(MOD_EVENTS_MINORS_INTERACTION)
	if(atWar(eToTeam, getTeam()))
		return false;
	
	if (MOD_EVENTS_MINORS_INTERACTION) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanGiftUnit, getOwner(), eToPlayer, GetID()) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
	
	return true;
#else
	return !atWar(eToTeam, getTeam());
#endif				
}

//	--------------------------------------------------------------------------------
#if defined(MOD_CORE_PER_TURN_DAMAGE)
int CvUnit::addDamageReceivedThisTurn(int iDamage, CvUnit* pAttacker)
{
	m_iDamageTakenThisTurn+=iDamage;

	if (pAttacker)
		//remember the attacker for our danger calculation - in case he moves and becomes invisible
		GET_PLAYER(getOwner()).AddKnownAttacker(pAttacker);

	return m_iDamageTakenThisTurn;
}

void CvUnit::flipDamageReceivedPerTurn()
{
	m_iDamageTakenLastTurn = m_iDamageTakenThisTurn;
	m_iDamageTakenThisTurn = 0;
}

bool CvUnit::isProjectedToDieNextTurn() const
{
	return (m_iDamageTakenLastTurn>GetCurrHitPoints());
}
#endif

//	--------------------------------------------------------------------------------
bool CvUnit::canLoadUnit(const CvUnit& unit, const CvPlot& targetPlot) const
{
	VALIDATE_OBJECT
	if(&unit == this)
	{
		return false;
	}

	if(unit.getTeam() != getTeam())
	{
		return false;
	}

	if(getCargo() > 0)
	{
		return false;
	}

	if(unit.isCargo())
	{
		return false;
	}

	if(!(unit.cargoSpaceAvailable(getSpecialUnitType(), getDomainType())))
	{
		return false;
	}

	if(!(unit.atPlot(targetPlot)))
	{
		return false;
	}

	if(!isHiddenNationality() && unit.isHiddenNationality())
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
void CvUnit::loadUnit(CvUnit& unit)
{
	VALIDATE_OBJECT
	CvPlot* p = plot();
	if(!p || !canLoadUnit(unit, *p))
	{
		return;
	}

	setTransportUnit(&unit);
}

//	--------------------------------------------------------------------------------
bool CvUnit::shouldLoadOnMove(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(isCargo())
	{
		return false;
	}

	switch(getDomainType())
	{
	case DOMAIN_LAND:
		if(pPlot->isWater())
		{
			return true;
		}
		break;
	case DOMAIN_AIR:
		if(!pPlot->isFriendlyCity(*this, true))
		{
			return true;
		}
		break;
	default:
		break;
	}

	if(isTerrainImpassable(pPlot->getTerrainType()))
	{
		bool bCanPass = false;
		bCanPass = m_Promotions.GetAllowTerrainPassable(pPlot->getTerrainType());

		if(!bCanPass)
		{
			return true;
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canLoad(const CvPlot& targetPlot) const
{
	VALIDATE_OBJECT
	if(NO_SPECIALUNIT != getSpecialUnitType())
	{
		CvSpecialUnitInfo* pkSpecialUnitInfo = GC.getSpecialUnitInfo(getSpecialUnitType());
		if(pkSpecialUnitInfo)
		{
			if(pkSpecialUnitInfo->isCityLoad())
			{
				if(targetPlot.isCity())
				{
					return true;
				}

#if defined(MOD_EVENTS_REBASE)
				if (MOD_EVENTS_REBASE) {
					if (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_CanLoadAt, getOwner(), GetID(), targetPlot.getX(), targetPlot.getY()) == GAMEEVENTRETURN_TRUE) {
						return true;
					}
				} else {
#endif
				ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
				if (pkScriptSystem)
				{
					CvLuaArgsHandle args;
					args->Push(getOwner());
					args->Push(GetID());
					args->Push(targetPlot.getX());
					args->Push(targetPlot.getY());

					// Attempt to execute the game events.
					// Will return false if there are no registered listeners.
					bool bResult = false;
					if (LuaSupport::CallTestAny(pkScriptSystem, "CanLoadAt", args.get(), bResult))
					{
						// Check the result.
						if (bResult == true) {
							return true;
						}
					}
				}
#if defined(MOD_EVENTS_REBASE)
				}
#endif
			}
		}
	}

	const IDInfo* pUnitNode = targetPlot.headUnitNode();
	while(pUnitNode != NULL)
	{
		const CvUnit* pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = targetPlot.nextUnitNode(pUnitNode);

		if(pLoopUnit && canLoadUnit(*pLoopUnit, targetPlot))
		{
			return true;
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
void CvUnit::load()
{
	VALIDATE_OBJECT
	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;
	int iPass;

	pPlot = plot();

	if(!pPlot || !canLoad(*pPlot))
	{
		return;
	}


	for(iPass = 0; iPass < 2; iPass++)
	{
		pUnitNode = pPlot->headUnitNode();

		while(pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(*pUnitNode);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);

			if(pLoopUnit && canLoadUnit(*pLoopUnit, *pPlot))
			{
				if((iPass == 0) ? (pLoopUnit->getOwner() == getOwner()) : (pLoopUnit->getTeam() == getTeam()))
				{
					setTransportUnit(pLoopUnit);
					break;
				}
			}
		}

		if(isCargo())
		{
			break;
		}
	}
}


//	--------------------------------------------------------------------------------
bool CvUnit::canUnload() const
{
	VALIDATE_OBJECT
	CvPlot& kPlot = *(plot());

	if(getTransportUnit() == NULL)
	{
		return false;
	}

	if(!kPlot.isValidDomainForLocation(*this))
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
void CvUnit::unload()
{
	VALIDATE_OBJECT
	if(!canUnload())
	{
		return;
	}

	setTransportUnit(NULL);
}


//	--------------------------------------------------------------------------------
bool CvUnit::canUnloadAll() const
{
	VALIDATE_OBJECT
	if(getCargo() == 0)
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
void CvUnit::unloadAll()
{
	VALIDATE_OBJECT
	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;

	if(!canUnloadAll())
	{
		return;
	}

	pPlot = plot();

	pUnitNode = pPlot->headUnitNode();

	while(pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if(pLoopUnit && pLoopUnit->getTransportUnit() == this)
		{
			if(pLoopUnit->canUnload())
			{
				pLoopUnit->setTransportUnit(NULL);
			}
			else
			{
				CvAssert(isHuman());
				pLoopUnit->SetActivityType(ACTIVITY_AWAKE);
			}
		}
	}
}


//	--------------------------------------------------------------------------------
bool CvUnit::canHold(const CvPlot* pPlot) const // skip turn
{
	VALIDATE_OBJECT
	if(isHuman() && !getFortifyTurns())  // we aren't fortified
	{
#if defined(MOD_GLOBAL_STACKING_RULES)
		if(pPlot->getNumFriendlyUnitsOfType(this) > pPlot->getUnitLimit())
#else
		if(pPlot->getNumFriendlyUnitsOfType(this) > GC.getPLOT_UNIT_LIMIT())
#endif
		{
			return false;
		}
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canSleep(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	// Can't sleep if we're a Unit that can normally fortify
	if(!noDefensiveBonus() && IsCombatUnit())
	{
		return false;
	}

	if(isHuman() && !getFortifyTurns())  // we aren't fortified
	{
#if defined(MOD_GLOBAL_STACKING_RULES)
		if(pPlot->getNumFriendlyUnitsOfType(this) > pPlot->getUnitLimit())
#else
		if(pPlot->getNumFriendlyUnitsOfType(this) > GC.getPLOT_UNIT_LIMIT())
#endif
		{
			return false;
		}
	}

	if(isFortifyable())
	{
		return false;
	}

	if(isWaiting())
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canFortify(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(isHuman() && !getFortifyTurns())  // we aren't fortified
	{
#if defined(MOD_GLOBAL_STACKING_RULES)
		if(pPlot->getNumFriendlyUnitsOfType(this) > pPlot->getUnitLimit())
#else
		if(pPlot->getNumFriendlyUnitsOfType(this) > GC.getPLOT_UNIT_LIMIT())
#endif
		{
			return false;
		}
	}

	if(!isFortifyable(true))
	{
		return false;
	}

	if(isWaiting())
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canAirPatrol(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(getDomainType() != DOMAIN_AIR)
	{
		return false;
	}

	if(!canAirDefend(pPlot))
	{
		return false;
	}

	if(isWaiting())
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsRangeAttackIgnoreLOS() const
{
	VALIDATE_OBJECT
	return GetRangeAttackIgnoreLOSCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetRangeAttackIgnoreLOSCount() const
{
	VALIDATE_OBJECT
	return m_iRangeAttackIgnoreLOSCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeRangeAttackIgnoreLOSCount(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iRangeAttackIgnoreLOSCount += iChange;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsCityAttackOnly() const
{
	VALIDATE_OBJECT
	return m_iCityAttackOnlyCount > 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeCityAttackOnlyCount(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iCityAttackOnlyCount += iChange;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsCaptureDefeatedEnemy() const
{
	VALIDATE_OBJECT
	return m_iCaptureDefeatedEnemyCount > 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeCaptureDefeatedEnemyCount(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iCaptureDefeatedEnemyCount += iChange;
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::GetCaptureChance(CvUnit *pEnemy)
{
	int iRtnValue = 0;

	if (m_iCaptureDefeatedEnemyCount > 0 && AreUnitsOfSameType(*pEnemy))
	{
		// Look at ratio of intrinsic combat strengths
		CvUnitEntry *pkEnemyInfo = GC.getUnitInfo(pEnemy->getUnitType());
		if (pkEnemyInfo)
		{
			int iTheirCombat = pkEnemyInfo->GetCombat();
#if defined(MOD_BALANCE_CORE)
			if(pEnemy->GetCannotBeCaptured())
			{
				return 0;
			}
#endif

			if (iTheirCombat > 0)
			{
				int iMyCombat = m_pUnitInfo->GetCombat();
				int iComputedChance = GC.getCOMBAT_CAPTURE_MIN_CHANCE() + (int)(((float)iMyCombat / (float)iTheirCombat) * GC.getCOMBAT_CAPTURE_RATIO_MULTIPLIER());
				iRtnValue = min(GC.getCOMBAT_CAPTURE_MAX_CHANCE(), iComputedChance);
			}
		}
	}

	return iRtnValue;
}
#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
void CvUnit::ChangeCannotBeCapturedCount(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iCannotBeCapturedCount += iChange;
	}
}
//	--------------------------------------------------------------------------------
bool CvUnit::GetCannotBeCaptured()
{
	VALIDATE_OBJECT
	return m_iCannotBeCapturedCount > 0;
}
//	--------------------------------------------------------------------------------
void CvUnit::ChangeForcedDamageValue(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iForcedDamage += iChange;
	}
}
//	--------------------------------------------------------------------------------
int CvUnit::getForcedDamageValue()
{
	VALIDATE_OBJECT
	return m_iForcedDamage;
}
//	--------------------------------------------------------------------------------
void CvUnit::ChangeChangeDamageValue(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iChangeDamage += iChange;
	}
}
//	--------------------------------------------------------------------------------
int CvUnit::getChangeDamageValue()
{
	VALIDATE_OBJECT
	return m_iChangeDamage;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangePromotionDuration(PromotionTypes eIndex, int iChange)
{
	std::map<PromotionTypes, int>& m_map = m_PromotionDuration.dirtyGet();
	if (m_map.find(eIndex) != m_map.end())
	{
		m_map[eIndex] += iChange;
		if (m_map[eIndex] == 0)
			m_map.erase(eIndex);
	}
	else
		m_map[eIndex] = iChange;
}
//	--------------------------------------------------------------------------------
int CvUnit::getPromotionDuration(PromotionTypes eIndex)
{
	const std::map<PromotionTypes, int>& m_map = m_PromotionDuration.get();
	std::map<PromotionTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second;
	else
		return 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetTurnPromotionGained(PromotionTypes eIndex, int iValue)
{
	std::map<PromotionTypes, int>& m_map = m_TurnPromotionGained.dirtyGet();
	if (iValue>0)
		m_map[eIndex] = iValue;
	else
		m_map.erase(eIndex);
}
//	--------------------------------------------------------------------------------
int CvUnit::getTurnPromotionGained(PromotionTypes eIndex)
{
	const std::map<PromotionTypes, int>& m_map = m_TurnPromotionGained.get();
	std::map<PromotionTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second;
	else
		return 0;
}
#endif
//	--------------------------------------------------------------------------------
bool CvUnit::canSetUpForRangedAttack(const CvPlot* /*pPlot*/) const
{
	VALIDATE_OBJECT
	if(!isMustSetUpToRangedAttack())
	{
		return false;
	}

	if(isSetUpForRangedAttack())
	{
		return false;
	}

	if(isEmbarked())
		return false;

	if(movesLeft() <= 0)
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isSetUpForRangedAttack() const
{
	VALIDATE_OBJECT
	return m_bSetUpForRangedAttack;
}

//	--------------------------------------------------------------------------------
void CvUnit::setSetUpForRangedAttack(bool bValue)
{
	VALIDATE_OBJECT
	if(isSetUpForRangedAttack() != bValue)
	{
		m_bSetUpForRangedAttack = bValue;

		if(bValue)
		{
			changeMoves(-GC.getMOVE_DENOMINATOR());
		}
	}
}


//	--------------------------------------------------------------------------------
bool CvUnit::canEmbarkAtPlot(const CvPlot* pPlot) const
{
	if(!CanEverEmbark())
	{
		return false;
	}

	if(isEmbarked())
	{
		return false;
	}

	if(movesLeft() <= 0)
	{
		return false;
	}

	// search the water plots around this plot to see if any are vacant
	CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(pPlot);
	for(size_t i=0; i<NUM_DIRECTION_TYPES; i++)
	{
		CvPlot* pEvalPlot = aNeighbors[i];
		if(pEvalPlot && canEmbarkOnto(*plot(), *pEvalPlot))
			return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canDisembarkAtPlot(const CvPlot* pPlot) const
{
	if(!CanEverEmbark())
	{
		return false;
	}

	if(!isEmbarked())
	{
		return false;
	}

	if(movesLeft() <= 0)
	{
		return false;
	}

	// search the water plots around this plot to see if any are vacant
	CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(pPlot);
	for(size_t i=0; i<NUM_DIRECTION_TYPES; i++)
	{
		CvPlot* pEvalPlot = aNeighbors[i];
		if(pEvalPlot && canDisembarkOnto(*plot(), *pEvalPlot))
			return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canEmbarkOnto(const CvPlot& originPlot, const CvPlot& targetPlot, bool bOverrideEmbarkedCheck /* = false */, int iMoveFlags) const
{
	VALIDATE_OBJECT

	if(!CanEverEmbark())
	{
		return false;
	}
		
	if(isEmbarked() && !bOverrideEmbarkedCheck)
	{
		return false;
	}

	if(!targetPlot.needsEmbarkation(this))
	{
		return false;
	}

	if(!targetPlot.isRevealed(getTeam()))
	{
		return false;
	}

	if(originPlot.needsEmbarkation(this))
	{
		return false;
	}

	return canMoveInto(targetPlot, iMoveFlags);
}

//	--------------------------------------------------------------------------------
bool CvUnit::canDisembarkOnto(const CvPlot& originPlot, const CvPlot& targetPlot, bool bOverrideEmbarkedCheck /* = false */, int iMoveFlags) const
{
	VALIDATE_OBJECT

	if(!CanEverEmbark())
	{
		return false;
	}
		
	if(!isEmbarked() && !bOverrideEmbarkedCheck)
	{
		return false;
	}

	if(targetPlot.needsEmbarkation(this))
	{
		return false;
	}

	if(!targetPlot.isRevealed(getTeam()))
	{
		return false;
	}

	if(!originPlot.needsEmbarkation(this))
	{
		return false;
	}

	return canMoveInto(targetPlot, iMoveFlags);
}

//	--------------------------------------------------------------------------------
bool CvUnit::CanEverEmbark() const
{
	VALIDATE_OBJECT

	return (getDomainType() == DOMAIN_LAND && IsHasEmbarkAbility() && !IsHoveringUnit() && !canMoveAllTerrain() && !isCargo() );
}

//	--------------------------------------------------------------------------------
void CvUnit::embark(CvPlot* pPlot)
{
	VALIDATE_OBJECT
	if (canChangeVisibility())
		pPlot->changeAdjacentSight(getTeam(), visibilityRange(), false, getSeeInvisibleType(), getFacingDirection(true));

	setEmbarked(true);

	if (canChangeVisibility())
		pPlot->changeAdjacentSight(getTeam(), visibilityRange(), true, getSeeInvisibleType(), getFacingDirection(true));

	auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
	gDLL->GameplayUnitEmbark(pDllUnit.get(), true);

#if !defined(NO_ACHIEVEMENTS)
	if(isHuman() && !GC.getGame().isGameMultiPlayer() && GET_PLAYER(GC.getGame().getActivePlayer()).isLocalPlayer())
	{
		gDLL->UnlockAchievement(ACHIEVEMENT_UNIT_EMBARK);
	}
#endif
}

//	--------------------------------------------------------------------------------
void CvUnit::disembark(CvPlot* pPlot)
{
	VALIDATE_OBJECT
	if (canChangeVisibility())
		pPlot->changeAdjacentSight(getTeam(), visibilityRange(), false, getSeeInvisibleType(), getFacingDirection(true));

	setEmbarked(false);

	if (canChangeVisibility())
		pPlot->changeAdjacentSight(getTeam(), visibilityRange(), true, getSeeInvisibleType(), getFacingDirection(true));

	auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
	gDLL->GameplayUnitEmbark(pDllUnit.get(), false);
}

//	--------------------------------------------------------------------------------
void CvUnit::setEmbarked(bool bValue)
{
	VALIDATE_OBJECT
	if(m_bEmbarked != bValue)
	{
		m_bEmbarked = bValue;
	}
}

//	--------------------------------------------------------------------------------
/// Does this Unit have the ability to embark?
bool CvUnit::IsHasEmbarkAbility() const
{
#if defined(MOD_GLOBAL_CANNOT_EMBARK)
	if (MOD_GLOBAL_CANNOT_EMBARK && getUnitInfo().CannotEmbark()) {
		return false;
	}
#endif

	return GetEmbarkAbilityCount() > 0;
}

//	--------------------------------------------------------------------------------
/// Does this Unit have the ability to embark?
int CvUnit::GetEmbarkAbilityCount() const
{
	return m_iEmbarkAbilityCount;
}

//	--------------------------------------------------------------------------------
/// Does this Unit have the ability to embark?
void CvUnit::ChangeEmbarkAbilityCount(int iChange)
{
	if(iChange != 0)
	{
		m_iEmbarkAbilityCount += iChange;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsIgnoringDangerWakeup() const
{
	return m_bIgnoreDangerWakeup;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetIgnoreDangerWakeup(bool bState)
{
	m_bIgnoreDangerWakeup = bState;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsEmbarkAllWater() const
{
	VALIDATE_OBJECT
	return (GetEmbarkAllWaterCount() > 0);
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeEmbarkAllWaterCount(int iValue)
{
	m_iEmbarkedAllWaterCount += iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetEmbarkAllWaterCount() const
{
	return m_iEmbarkedAllWaterCount;
}

#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
//	--------------------------------------------------------------------------------
bool CvUnit::IsEmbarkDeepWater() const
{
	VALIDATE_OBJECT
	return MOD_PROMOTIONS_DEEP_WATER_EMBARKATION && (GetEmbarkDeepWaterCount() > 0);
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeEmbarkDeepWaterCount(int iValue)
{
	m_iEmbarkedDeepWaterCount += iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetEmbarkDeepWaterCount() const
{
	return m_iEmbarkedDeepWaterCount;
}
#endif

//	--------------------------------------------------------------------------------
void CvUnit::ChangeEmbarkExtraVisibility(int iValue)
{
	m_iEmbarkExtraVisibility += iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetEmbarkExtraVisibility() const
{
	return m_iEmbarkExtraVisibility;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeEmbarkDefensiveModifier(int iValue)
{
	m_iEmbarkDefensiveModifier += iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetEmbarkDefensiveModifier() const
{
	return m_iEmbarkDefensiveModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeCapitalDefenseModifier(int iValue)
{
	m_iCapitalDefenseModifier += iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetCapitalDefenseModifier() const
{
	return m_iCapitalDefenseModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeCapitalDefenseFalloff(int iValue)
{
	m_iCapitalDefenseFalloff += iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetCapitalDefenseFalloff() const
{
	return m_iCapitalDefenseFalloff;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeCityAttackPlunderModifier(int iValue)
{
	m_iCityAttackPlunderModifier += iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetCityAttackPlunderModifier() const
{
	return m_iCityAttackPlunderModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeReligiousStrengthLossRivalTerritory(int iValue)
{
	m_iReligiousStrengthLossRivalTerritory += iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetReligiousStrengthLossRivalTerritory() const
{
	return m_iReligiousStrengthLossRivalTerritory;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeTradeMissionInfluenceModifier(int iValue)
{
	m_iTradeMissionInfluenceModifier += iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetTradeMissionInfluenceModifier() const
{
	return m_iTradeMissionInfluenceModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeTradeMissionGoldModifier(int iValue)
{
	m_iTradeMissionGoldModifier += iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetTradeMissionGoldModifier() const
{
	return m_iTradeMissionGoldModifier;
}
//	--------------------------------------------------------------------------------
int CvUnit::GetNumGoodyHutsPopped() const
{
	return m_iNumGoodyHutsPopped;
}
//	--------------------------------------------------------------------------------
void CvUnit::ChangeNumGoodyHutsPopped(int iValue)
{
	m_iNumGoodyHutsPopped = iValue;
}
//	--------------------------------------------------------------------------------
bool CvUnit::TurnProcessed() const
{
	VALIDATE_OBJECT
	return m_bAITurnProcessed;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetTurnProcessed(bool bValue)
{
	VALIDATE_OBJECT
	if(TurnProcessed() != bValue)
	{
		m_bAITurnProcessed = bValue;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::isUnderTacticalControl() const
{
	VALIDATE_OBJECT
	return (m_eTacticalMove != NO_TACTICAL_MOVE);
}

//	--------------------------------------------------------------------------------
void CvUnit::setTacticalMove(TacticalAIMoveTypes eMove)
{
	VALIDATE_OBJECT

#if defined(MOD_BALANCE_CORE_MILITARY)

	//clear homeland move, can't have both ...
	m_eHomelandMove = AI_HOMELAND_MOVE_NONE;
	
	if (m_eTacticalMove != eMove)
	{
		m_iTactMoveSetTurn = GC.getGame().getGameTurn();
		m_eTacticalMove = eMove;
	}
#else
	m_eTacticalMove = eMove;
#endif
}

//	--------------------------------------------------------------------------------
TacticalAIMoveTypes CvUnit::getTacticalMove(int* pTurnSet) const
{
	VALIDATE_OBJECT
	if (pTurnSet)
		*pTurnSet = m_iTactMoveSetTurn;

	return m_eTacticalMove;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canRecruitFromTacticalAI() const
{
	VALIDATE_OBJECT
	if(m_eTacticalMove != NO_TACTICAL_MOVE)
	{
		return GC.GetGameTacticalMoves()->GetEntry(m_eTacticalMove)->CanRecruitForOperations();
	}
	return true;
}

//	--------------------------------------------------------------------------------
/// Set a destination plot for multi-turn tactical moves
void CvUnit::SetTacticalAIPlot(CvPlot* pPlot)
{
	VALIDATE_OBJECT
	if(pPlot != NULL)
	{
		m_iTacticalAIPlotX = pPlot->getX();
		m_iTacticalAIPlotY = pPlot->getY();
	}
	else
	{
		m_iTacticalAIPlotX = INVALID_PLOT_COORD;
		m_iTacticalAIPlotY = INVALID_PLOT_COORD;
	}
}

//	--------------------------------------------------------------------------------
/// Get the destination plot for multi-turn tactical moves
CvPlot* CvUnit::GetTacticalAIPlot() const
{
	VALIDATE_OBJECT
	CvPlot* pPlot;

	pPlot = GC.getMap().plot(m_iTacticalAIPlotX, m_iTacticalAIPlotY);

	return pPlot;
}

#if defined(MOD_BALANCE_CORE_MILITARY)
//	--------------------------------------------------------------------------------
bool CvUnit::hasCurrentTacticalMove() const
{ 
	return ( m_eTacticalMove!=NO_TACTICAL_MOVE && m_eTacticalMove!=GC.getInfoTypeForString("TACTICAL_UNASSIGNED") && m_iTactMoveSetTurn==GC.getGame().getGameTurn() );
}

void CvUnit::setHomelandMove(AIHomelandMove eMove)
{
	VALIDATE_OBJECT

	if (hasCurrentTacticalMove())
	{
		if (eMove==AI_HOMELAND_MOVE_NONE)
			return;

		CvTacticalMoveXMLEntry* pkMoveInfo = GC.getTacticalMoveInfo(m_eTacticalMove);
		if (pkMoveInfo)
		{
			CvString msg = CvString::format("Warning: Unit %d with current tactical move %s used for homeland move %s\n",
				GetID(), pkMoveInfo->GetType(), eMove == AI_HOMELAND_MOVE_NONE ? "NONE" : homelandMoveNames[eMove]);
			GET_PLAYER(m_eOwner).GetHomelandAI()->LogHomelandMessage(msg);
		}
	}

	//clear tactical move, can't have both ...
	m_eTacticalMove = NO_TACTICAL_MOVE;

	if (eMove >= NUM_AI_HOMELAND_MOVES)
	{
		m_eHomelandMove = AI_HOMELAND_MOVE_NONE;
		return;
	}

	if (m_eHomelandMove != eMove)
	{
		m_iHomelandMoveSetTurn = GC.getGame().getGameTurn();
		m_eHomelandMove = eMove;
	}
}

//	--------------------------------------------------------------------------------
AIHomelandMove CvUnit::getHomelandMove(int* pTurnSet) const
{
	VALIDATE_OBJECT
	if (pTurnSet)
		*pTurnSet = m_iTactMoveSetTurn;

	return m_eHomelandMove;
}
#endif

//	--------------------------------------------------------------------------------
/// Logs information about when a worker begins and finishes construction of an improvement
void CvUnit::LogWorkerEvent(BuildTypes eBuildType, bool bStartingConstruction)
{
	VALIDATE_OBJECT
	// if not logging, don't log!
	if(!GC.getLogging() || !GC.GetBuilderAILogging())
	{
		return;
	}

	CvString strLog;
	CvString strTemp;

	strTemp.Format("%d,", GC.getGame().getGameTurn()); // turn
	strLog += strTemp;

	CvString strPlayerName;
	strPlayerName = GET_PLAYER(getOwner()).getCivilizationShortDescription();
	strLog += strPlayerName;
	strLog += ",";

	strTemp.Format("%d,", GetID()); // unit id
	strLog += strTemp;

	CvString strLoc; // location
	strLoc.Format("%d,%d,", getX(), getY());
	strLog += strLoc;

	strLog += GC.getBuildInfo(eBuildType)->GetType(); // what is being constructed
	strLog += ",";

	// beginning or completing construction
	if(bStartingConstruction)
	{
		strLog += "begin,";
	}
	else
	{
		strLog += "complete,";
	}

	// drawing improvement information
	CvString strResource = ",";
	CvString strCanSee = ",";

	ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(eBuildType)->getImprovement();
	if(eImprovement != NO_IMPROVEMENT)
	{
		ResourceTypes eResource = plot()->getResourceType(getTeam());
		if(eResource != NO_RESOURCE)
		{
			strResource = GC.getResourceInfo(eResource)->GetType();
			strResource += ",";
		}
		else if(plot()->getResourceType(NO_TEAM) != NO_RESOURCE)
		{
			eResource = plot()->getResourceType(NO_TEAM);
			strResource = GC.getResourceInfo(eResource)->GetType();
			strResource += ",";

			strCanSee = "Can't see!,";
		}
	}

	strLog += strResource;
	strLog += strCanSee;

	// gold cost
	CvString strCost = ",";
	if(bStartingConstruction)
	{
		strCost.Format("%d,", GET_PLAYER(getOwner()).getBuildCost(plot(), eBuildType));
	}
	strLog += strCost;

	// gold amount
	CvString strGoldAmount = ",";
	if(bStartingConstruction)
	{
		strGoldAmount.Format("%d,", GET_PLAYER(getOwner()).GetTreasury()->GetGold());
	}
	strLog += strGoldAmount;

	// Open the log file
	CvString strFileName;
	if(GC.getPlayerAndCityAILogSplit())
	{
		strFileName = "BuilderEventLog_" + strPlayerName + ".csv";
	}
	else
	{
		strFileName = "BuilderEventLog.csv";
	}


	// Open the log file
	FILogFile* pLog;
	pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);
	pLog->Msg(strLog);
}

//	--------------------------------------------------------------------------------
/// Current power of unit (raw unit type power adjusted for health)
int CvUnit::GetPower() const
{
	VALIDATE_OBJECT
	int iPower = getUnitInfo().GetPower();

#if defined(MOD_BUGFIX_UNIT_POWER_CALC)
	if (getUnitInfo().GetCombat() > 0) {
		iPower = iPower * GetBaseCombatStrength() / getUnitInfo().GetCombat();
	}
#endif
#if defined(MOD_API_EXTENSIONS) && defined(MOD_BUGFIX_UNIT_POWER_CALC)
	if (getUnitInfo().GetRangedCombat() > 0) {
		iPower = iPower * GetBaseRangedCombatStrength() / getUnitInfo().GetRangedCombat();
	}
#endif
	
	//Take promotions into account: unit with 4 promotions worth ~50% more
	int iPowerMod = getLevel() * 125;
	iPower = (iPower * (1000 + iPowerMod)) / 1000;
	iPower *= GetCurrHitPoints();
	iPower /= GetMaxHitPoints();
	return iPower;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canHeal(const CvPlot* pPlot, bool bTestVisible) const
{
	VALIDATE_OBJECT

	// No barb healing
	if(isBarbarian())
	{
		return false;
	}

	if(!IsHurt())
	{
		return false;
	}
#if defined(MOD_BALANCE_CORE_MILITARY_RESISTANCE)
	if(MOD_BALANCE_CORE_MILITARY_RESISTANCE && !GET_PLAYER(getOwner()).isMinorCiv())
	{
		CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

		// Loop through all resources
		ResourceTypes eResource;
		int iNumResourceInfos = GC.getNumResourceInfos();
		for(int iResourceLoop = 0; iResourceLoop < iNumResourceInfos; iResourceLoop++)
		{
			eResource = (ResourceTypes) iResourceLoop;

			if(m_pUnitInfo->GetResourceQuantityRequirement(eResource) > 0)
			{
				int iAvailable = kPlayer.getNumResourceAvailable(eResource);
				if(iAvailable < 0)
				{
					return false;
				}
			}
		}
	}
#endif
	if(isWaiting())
	{
		return false;
	}

	if(healRate(pPlot) <= 0)
	{
		return false;
	}
	
	// JON - This should change when one-unit-per-plot city stuff is handled better
	// Unit Healing in cities

	if(isHuman())
	{
		if(plot()->isCity() && getDomainType() != DOMAIN_AIR)
		{
#if defined(MOD_BUGFIX_MINOR)
			// Civilians can heal in cities
			if (!IsCivilianUnit())
			{
#endif

				CvUnit* pUnit;
				int iBestDefenderValue = 0;
				int iBestDefenderID = 0;

				for(int iUnitLoop = 0; iUnitLoop < plot()->getNumUnits(); iUnitLoop++)
				{
					pUnit = plot()->getUnitByIndex(iUnitLoop);

					// Only check land Units vs one another, Naval Units vs one another, etc.
					if(pUnit->getDomainType() == getDomainType())
					{
						if(pUnit->GetBaseCombatStrength() > iBestDefenderValue)
						{
							iBestDefenderValue = pUnit->GetBaseCombatStrength();
							iBestDefenderID = pUnit->GetID();
						}
					}
				}

				// This is NOT the defending unit, it's in storage, so it can't heal
				if(iBestDefenderID != GetID())
				{
					return false;
				}
#if defined(MOD_BUGFIX_MINOR)
			}
#endif
		}
	}

	// Unit now has to be able to Fortify to Heal (since they're very similar states, and Heal gives a defense bonus)
	if(!bTestVisible)
	{
		// Embarked Units can't heal
		if(isEmbarked())
		{
			return false;
		}

		// Boats can only heal in friendly territory (without promotion)
		if(getDomainType() == DOMAIN_SEA)
		{
			if(!IsInFriendlyTerritory() && !isHealOutsideFriendly())
			{
				return false;
			}
		}
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canSentry(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(isHuman() && !getFortifyTurns())  // we aren't fortified
	{
#if defined(MOD_GLOBAL_STACKING_RULES)
		if(pPlot->getNumFriendlyUnitsOfType(this) > pPlot->getUnitLimit())
#else
		if(pPlot->getNumFriendlyUnitsOfType(this) > GC.getPLOT_UNIT_LIMIT())
#endif
		{
			return false;
		}
	}

	if(!IsCanDefend(pPlot))
	{
		return false;
	}

	if(isWaiting())
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
int CvUnit::healRate(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	// Boats can only heal in friendly territory
	if(getDomainType() == DOMAIN_SEA)
	{
		if(!IsInFriendlyTerritory() && !isHealOutsideFriendly())
		{
			return 0;
		}
	}
	
#if defined(MOD_UNITS_HOVERING_LAND_ONLY_HEAL)
	if (MOD_UNITS_HOVERING_LAND_ONLY_HEAL) {
		// Hovering units can only heal over land
		if (IsHoveringUnit() && pPlot->isWater()) {
			return 0;
		}
	}
#endif

	const IDInfo* pUnitNode;
	CvCity* pCity = pPlot->getPlotCity();

	CvCity* pClosestCity = NULL;
	const CvUnit* pLoopUnit;
	CvPlot* pLoopPlot;

	int iExtraHeal = 0;
	int iExtraFriendlyHeal = getExtraFriendlyHeal();
	int iExtraNeutralHeal = getExtraNeutralHeal();
	int iExtraEnemyHeal = getExtraEnemyHeal();
#if defined(MOD_BALANCE_CORE_BELIEFS)
	int iReligionMod = 0;
	if(MOD_BALANCE_CORE_BELIEFS)
	{
		if(GET_PLAYER(getOwner()).getCapitalCity() != NULL && (plot()->getOwner() == getOwner()))
		{
			ReligionTypes eMajority = GET_PLAYER(getOwner()).getCapitalCity()->GetCityReligions()->GetReligiousMajority();
			if(eMajority != NO_RELIGION)
			{
				const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, getOwner());
				if(pReligion)
				{
					iReligionMod = pReligion->m_Beliefs.GetFriendlyHealChange();
					BeliefTypes eSecondaryPantheon = GET_PLAYER(getOwner()).getCapitalCity()->GetCityReligions()->GetSecondaryReligionPantheonBelief();
					if (eSecondaryPantheon != NO_BELIEF)
					{
						iReligionMod += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetFriendlyHealChange();
					}
					iExtraFriendlyHeal += iReligionMod;
				}
			}
		}
	}
	else
	{
#endif
	// Heal from religion
	int iReligionMod = 0;
	if(!pCity)
	{
		pClosestCity = pPlot->GetAdjacentFriendlyCity(getTeam());
	}
	else
	{
		pClosestCity = pCity;
	}
	if(pClosestCity && pClosestCity->getOwner() == getOwner())
	{
		ReligionTypes eMajority = pClosestCity->GetCityReligions()->GetReligiousMajority();
		if(eMajority != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, getOwner());
			if(pReligion)
			{
				iReligionMod = pReligion->m_Beliefs.GetFriendlyHealChange();
				BeliefTypes eSecondaryPantheon = pClosestCity->GetCityReligions()->GetSecondaryReligionPantheonBelief();
				if (eSecondaryPantheon != NO_BELIEF)
				{
					iReligionMod += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetFriendlyHealChange();
				}
				iExtraFriendlyHeal += iReligionMod;
			}
		}
	}
#if defined(MOD_BALANCE_CORE)
	}
#endif
	// Heal from units
	int iBestHealFromUnits = 0;
	pUnitNode = pPlot->headUnitNode();
	while(pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if(pLoopUnit && pLoopUnit->getTeam() == getTeam())
		{
			int iHeal = pLoopUnit->getSameTileHeal();

			if(iHeal > iBestHealFromUnits)
			{
				iBestHealFromUnits = iHeal;
			}
		}
	}
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

		if(pLoopPlot != NULL)
		{
			if(pLoopPlot->area() == pPlot->area())
			{
				pUnitNode = pLoopPlot->headUnitNode();

				while(pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(*pUnitNode);
					pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

					if(pLoopUnit && pLoopUnit->getTeam() == getTeam())
					{
						int iHeal = pLoopUnit->getAdjacentTileHeal();

						if(iHeal > iBestHealFromUnits)
						{
							iBestHealFromUnits = iHeal;
						}
					}
				}
			}
		}
	}
	iExtraHeal += iBestHealFromUnits;

	// Heal from territory ownership (friendly, enemy, etc.)
	int iBaseHeal = 0;
	if(pPlot->isCity())
	{
		iBaseHeal = GC.getCITY_HEAL_RATE();
		iExtraHeal += (GET_TEAM(getTeam()).isFriendlyTerritory(pPlot->getTeam()) ? iExtraFriendlyHeal : iExtraNeutralHeal);
		if(pCity)
		{
			iExtraHeal += pCity->getHealRate();
		}
	}
	else
	{
		if(!IsInFriendlyTerritory())
		{
			if(isEnemy(pPlot->getTeam(), pPlot))
			{
				iBaseHeal = GC.getENEMY_HEAL_RATE();
				iExtraHeal += iExtraEnemyHeal;
			}
			else
			{
				iBaseHeal = GC.getNEUTRAL_HEAL_RATE();
				iExtraHeal += iExtraNeutralHeal;
			}
		}
		else
		{
			iBaseHeal = GC.getFRIENDLY_HEAL_RATE();
			iExtraHeal += iExtraFriendlyHeal;
		}
	}
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	if(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	{
		const std::vector<ResourceTypes>& vStrategicMonopolies = GET_PLAYER(getOwner()).GetStrategicMonopolies();
		for (size_t iResourceLoop = 0; iResourceLoop < vStrategicMonopolies.size(); iResourceLoop++)
		{
			ResourceTypes eResourceLoop = vStrategicMonopolies[iResourceLoop];
			CvResourceInfo* pInfo = GC.getResourceInfo(eResourceLoop);
			if (pInfo && pInfo->getMonopolyHealBonus() > 0)
			{
				iExtraHeal += pInfo->getMonopolyHealBonus();
			}
		}
	}
#endif

	// Base healing rate mod
	int iBaseHealMod = GET_PLAYER(getOwner()).getUnitBaseHealModifier();
	if(iBaseHealMod != 0)
	{
		iBaseHeal = ((100 + iBaseHealMod) / 100) * iBaseHeal;
	}
	CvAssertMsg(iBaseHeal >= 0, "Base healing rate not expected to be negative!");

	int iTotalHeal = iBaseHeal + iExtraHeal;
#if defined(MOD_BALANCE_CORE)
	TerrainTypes eTerrain = plot()->getTerrainType();
	if(eTerrain != NO_TERRAIN)
	{
		if(isTerrainDoubleHeal(eTerrain))
		{
			iTotalHeal *= 2;
		}
	}
	FeatureTypes eFeature = plot()->getFeatureType();
	if(eFeature != NO_FEATURE)
	{
		if(isFeatureDoubleHeal(eFeature))
		{
			iTotalHeal *= 2;
		}
	}
#endif
	return iTotalHeal;
}


//	--------------------------------------------------------------------------------
int CvUnit::healTurns(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	int iHeal;
	int iTurns;

	if(!IsHurt())
	{
		return 0;
	}

	iHeal = healRate(pPlot);

	if(iHeal > 0)
	{
		iTurns = (getDamage() / iHeal);

		if((getDamage() % iHeal) != 0)
		{
			iTurns++;
		}

		return iTurns;
	}
	else
	{
		return INT_MAX;
	}
}


//	--------------------------------------------------------------------------------
void CvUnit::doHeal()
{
	VALIDATE_OBJECT
	if(!isBarbarian())
	{
#if defined(MOD_BALANCE_CORE_MILITARY_RESISTANCE)
		if(MOD_BALANCE_CORE_MILITARY_RESISTANCE && !GET_PLAYER(getOwner()).isMinorCiv())
		{
			CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

			// Loop through all resources
			ResourceTypes eResource;
			int iNumResourceInfos = GC.getNumResourceInfos();
			for(int iResourceLoop = 0; iResourceLoop < iNumResourceInfos; iResourceLoop++)
			{
				eResource = (ResourceTypes) iResourceLoop;

				if(m_pUnitInfo->GetResourceQuantityRequirement(eResource) > 0)
				{
					int iAvailable = kPlayer.getNumResourceAvailable(eResource);
					if(iAvailable < 0)
					{
						return;
					}
				}
			}
		}
#endif
		changeDamage( -healRate(plot()) );
#if defined(MOD_BALANCE_CORE_BELIEFS)
		if(GET_PLAYER(getOwner()).getCapitalCity() != NULL && (plot()->getOwner() == getOwner()) && (plot()->getTurnDamage(false, false, true, true) == 0))
		{
			ReligionTypes eMajority = GET_PLAYER(getOwner()).getCapitalCity()->GetCityReligions()->GetReligiousMajority();
			if(eMajority != NO_RELIGION)
			{
				const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, getOwner());
				if(pReligion)
				{
					int iEra = GET_PLAYER(getOwner()).GetCurrentEra();
					if(iEra < 1)
					{
						iEra = 1;
					}
					if(pReligion->m_Beliefs.GetYieldPerHeal(YIELD_FAITH) > 0)
					{
						GET_PLAYER(getOwner()).ChangeFaith(pReligion->m_Beliefs.GetYieldPerHeal(YIELD_FAITH) * iEra);
						if(getOwner() == GC.getGame().getActivePlayer())
						{
							char text[256] = {0};
							float fDelay = 0.0f;
							sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_PEACE]", pReligion->m_Beliefs.GetYieldPerHeal(YIELD_FAITH)* iEra);
							DLLUI->AddPopupText(getX(),getY(), text, fDelay);
						}
					}
					BeliefTypes eSecondaryPantheon = GET_PLAYER(getOwner()).getCapitalCity()->GetCityReligions()->GetSecondaryReligionPantheonBelief();
					if (eSecondaryPantheon != NO_BELIEF)
					{
						if(GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetYieldPerHeal(YIELD_FAITH) > 0)
						{
							GET_PLAYER(getOwner()).ChangeFaith(GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetYieldPerHeal(YIELD_FAITH) * iEra);
							if(getOwner() == GC.getGame().getActivePlayer())
							{
								char text[256] = {0};
								float fDelay = 0.0f;
								sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_PEACE]", pReligion->m_Beliefs.GetYieldPerHeal(YIELD_FAITH) * iEra);
								DLLUI->AddPopupText(getX(),getY(), text, fDelay);
							}
						}
					}
				}
			}
		}
#endif
	}
	else //barbarian!
	{
		if(IsCombatUnit())
		{
			ImprovementTypes eCamp = (ImprovementTypes)GC.getBARBARIAN_CAMP_IMPROVEMENT();
			if( !hasMoved() && IsHurt() && plot()->getImprovementType()==eCamp )
			{
				changeDamage( -GC.getBALANCE_BARBARIAN_HEAL_RATE() );
			}
		}
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::DoAttrition()
{
	CvPlot* pPlot = plot();
	CvString strAppendText;
#if defined(MOD_BALANCE_CORE)
	if(isTrade())
	{
		return;
	}
#endif
	if(!IsInFriendlyTerritory())
	{
		if(isEnemy(pPlot->getTeam(), pPlot) && getEnemyDamageChance() > 0 && getEnemyDamage() > 0)
		{
			if(GC.getGame().getJonRandNum(100, "Enemy Territory Damage Chance") < getEnemyDamageChance())
			{
				strAppendText =  GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_ATTRITION");
				changeDamage(getEnemyDamage(), NO_PLAYER, 0.0, &strAppendText);
			}
		}
#if defined(MOD_BALANCE_CORE)
		else if(isEnemy(pPlot->getTeam(), pPlot) && getEnemyDamageChance() > 0 && getEnemyDamage() < 0)
		{
			if(GC.getGame().getJonRandNum(100, "Enemy Territory Heal Chance") <= getEnemyDamageChance())
			{
				strAppendText =  GetLocalizedText("TXT_KEY_MISC_YOU_ENEMY_UNITS_DEFECT");
				changeDamage(getEnemyDamage(), NO_PLAYER, 0.0, &strAppendText);
			}
		}
#endif
		else if(getNeutralDamageChance() > 0 && getNeutralDamage() > 0)
		{
			if(GC.getGame().getJonRandNum(100, "Neutral Territory Damage Chance") < getNeutralDamageChance())
			{
				strAppendText =  GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_ATTRITION");
				changeDamage(getNeutralDamage(), NO_PLAYER, 0.0, &strAppendText);
			}
		}
	}

#if defined(MOD_API_PLOT_BASED_DAMAGE)
	if (MOD_API_PLOT_BASED_DAMAGE) {
		int iDamage = plot()->getTurnDamage(ignoreTerrainDamage(), ignoreFeatureDamage(), extraTerrainDamage(), extraFeatureDamage());

		if (0 != iDamage) {
			if (iDamage > 0) {
				// CUSTOMLOG("Applying terrain/feature damage (of %i) for player/unit %i/%i at (%i, %i)", iDamage, getOwner(), GetID(), plot()->getX(), plot()->getY());
				CvString strAppendText =  GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_ATTRITION");
				changeDamage(iDamage, NO_PLAYER, 0.0, &strAppendText);
			} else {
				// CUSTOMLOG("Applying terrain/feature healing (of %i) for player/unit %i/%i at (%i, %i)", iDamage, getOwner(), GetID(), plot()->getX(), plot()->getY());
				changeDamage(iDamage, NO_PLAYER);
			}
		}
	} else {
#endif
		// slewis - helicopters take attrition when ending their turn over mountains.
		if(getDomainType() == DOMAIN_LAND && pPlot->isMountain() && !canMoveAllTerrain())
		{
			strAppendText =  GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_ATTRITION");
#if defined(MOD_BALANCE_CORE)
			if(MOD_BALANCE_CORE && GET_PLAYER(getOwner()).GetPlayerTraits()->IsCrossesMountainsAfterGreatGeneral())
			{
				changeDamage(25, NO_PLAYER, 0.0, &strAppendText);
			}
			else if(MOD_BALANCE_CORE && GET_PLAYER(getOwner()).GetPlayerTraits()->IsMountainPass())
			{
				changeDamage(15, NO_PLAYER, 0.0, &strAppendText);
			}
			else
			{
#endif
			changeDamage(50, NO_PLAYER, 0.0, &strAppendText);
#if defined(MOD_BALANCE_CORE)
			}
#endif
		}
#if defined(MOD_API_PLOT_BASED_DAMAGE)
	}
#endif

	if(getDamage() >= GC.getMAX_HIT_POINTS())
	{
		CvString strBuffer;
		CvNotifications* pNotification = GET_PLAYER(getOwner()).GetNotifications();
		if(pNotification)
		{
			strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_ATTRITION", getNameKey());
			Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_LOST");
			pNotification->Add(NOTIFICATION_UNIT_DIED, strBuffer, strSummary.toUTF8(), getX(), getY(), (int)getUnitType(), getOwner());
		}
	}

	TeamTypes eOwnerTeam = pPlot->getTeam();

	if (eOwnerTeam != NO_TEAM)
	{
		CvTeam &kTeam = GET_TEAM(eOwnerTeam);
		if (!kTeam.isMinorCiv() && eOwnerTeam != getTeam() && !kTeam.IsAllowsOpenBordersToTeam(getTeam()))
		{
			int iReligiousStrengthLoss = GetReligiousStrengthLossRivalTerritory();
			if (iReligiousStrengthLoss > 0)
			{
				int iStrength = GetReligionData()->GetReligiousStrength();
				int iStrengthLoss = (getUnitInfo().GetReligiousStrength() * iReligiousStrengthLoss) / 100;

				if ((iStrength - iStrengthLoss) <= 0)
				{
					CvString strBuffer;
					CvNotifications* pNotification = GET_PLAYER(getOwner()).GetNotifications();
					if(pNotification)
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_ATTRITION", getNameKey());
						Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_LOST");
						pNotification->Add(NOTIFICATION_UNIT_DIED, strBuffer, strSummary.toUTF8(), getX(), getY(), (int)getUnitType(), getOwner());
					}

					kill(false);
				}
				else
				{
					GetReligionData()->SetReligiousStrength(iStrength - iStrengthLoss);
					if (pPlot && pPlot->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF)
					{
						Localization::String string = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_ATTRITION").c_str();

						char text[256];
						sprintf_s (text, "%s [COLOR_WHITE]-%d [ICON_PEACE][ENDCOLOR]", string.toUTF8(), iStrengthLoss);
#if defined(SHOW_PLOT_POPUP)
						SHOW_PLOT_POPUP(plot(), getOwner(), text, 0.0f);
#else
						float fDelay = 0.0f;
						DLLUI->AddPopupText(getX(), getY(), text, fDelay);
#endif
					}
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::canAirlift(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	CvCity* pCity;

	if(getDomainType() != DOMAIN_LAND)
	{
		return false;
	}

	if(hasMoved())
	{
		return false;
	}

	if (pPlot->isWater())
	{
		return false;
	}

	pCity = pPlot->getPlotCity();
	if(pCity == NULL)
	{
		pCity = pPlot->GetAdjacentCity();
		if (pCity == NULL)
		{
			return false;
		}
	}

	if (isTrade())
	{
		return false;
	}

	if (!pCity->CanAirlift())
	{
		return false;
	}

	if(pCity->getTeam() != getTeam())
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canAirliftAt(const CvPlot* pPlot, int iX, int iY) const
{
	VALIDATE_OBJECT
	CvPlot* pTargetPlot;
	CvCity* pStartCity;
	CvCity* pTargetCity;

	if(!canAirlift(pPlot))
	{
		return false;
	}

	pStartCity = pPlot->getPlotCity();
	if(pStartCity == NULL)
	{
		pStartCity = pPlot->GetAdjacentCity();
		if (pStartCity == NULL)
		{
			return false;
		}
	}

	pTargetPlot = GC.getMap().plot(iX, iY);
	int iMoveFlags = CvUnit::MOVEFLAG_DESTINATION;
	if(!pTargetPlot || !canMoveInto(*pTargetPlot, iMoveFlags) || pTargetPlot->isWater())
	{
		return false;
	}

	pTargetCity = pTargetPlot->getPlotCity();
	if(pTargetCity == NULL)
	{
		pTargetCity = pTargetPlot->GetAdjacentFriendlyCity(getTeam());
		if (pTargetCity == NULL)
		{
			return false;
		}
	}

	if (pTargetCity == pStartCity)
	{
		return false;
	}

	if (!pTargetCity->CanAirlift())
	{
		return false;
	}

	// No enemy units adjacent
	CvPlot* pAdjacentPlot;
	int iI;
	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(iX, iY, ((DirectionTypes)iI));
		if(pAdjacentPlot != NULL)
		{
			UnitHandle pDefender = pAdjacentPlot->getBestDefender(NO_PLAYER, getOwner(), NULL, true);
			if (pDefender)
			{
				return false;
			}
		}
	}

	if(pTargetCity->getTeam() != getTeam())
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::airlift(int iX, int iY)
{
	VALIDATE_OBJECT
	CvPlot* pTargetPlot;

	if(!canAirliftAt(plot(), iX, iY))
	{
		return false;
	}

	pTargetPlot = GC.getMap().plot(iX, iY);
	CvAssert(pTargetPlot != NULL);


#if !defined(NO_ACHIEVEMENTS)
	//Here's Looking at You, Kid
	CvPlayerAI& kActivePlayer = GET_PLAYER(GC.getGame().getActivePlayer());
	if(pTargetPlot != NULL && getOwner() == kActivePlayer.GetID() && kActivePlayer.isHuman())
	{
		//Easy checks out of the way, now for the ugly ones.

		//Current City Casablanca?
		CvPlot* pCurrentPlot = plot();
		if(pCurrentPlot != NULL)
		{
			CvCity* pCurrentCity = pCurrentPlot->getPlotCity();
			CvCity* pTargetCity = pTargetPlot->getPlotCity();
			if(	pCurrentCity != NULL && pTargetCity != NULL &&
				strcmp(pCurrentCity->getNameKey(), "TXT_KEY_CITY_NAME_CASABLANCA") == 0 &&
				pTargetCity->IsOriginalCapital())
			{
				const PlayerTypes eOriginalOwner = pTargetCity->getOriginalOwner();
				CvPlayerAI& kOriginalPlayer = GET_PLAYER(eOriginalOwner);
				if(strcmp(kOriginalPlayer.getCivilizationTypeKey(), "CIVILIZATION_PORTUGAL") == 0)
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_XP2_22);
				}
			}
		}
	}
#endif


	if(pTargetPlot != NULL)
	{
		finishMoves();
		setXY(pTargetPlot->getX(), pTargetPlot->getY());
		return true;
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isNukeVictim(const CvPlot* pPlot, TeamTypes eTeam) const
{
	VALIDATE_OBJECT
	CvPlot* pLoopPlot;
	int iDX, iDY;

	if(!(GET_TEAM(eTeam).isAlive()))
	{
		return false;
	}

	if(eTeam == getTeam())
	{
		return false;
	}

	int iBlastRadius = /*2*/ GC.getNUKE_BLAST_RADIUS();

	for(iDX = -(iBlastRadius); iDX <= iBlastRadius; iDX++)
	{
		for(iDY = -(iBlastRadius); iDY <= iBlastRadius; iDY++)
		{
			pLoopPlot	= plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iDX, iDY, iBlastRadius);

			if(pLoopPlot != NULL)
			{
				if(pLoopPlot->getTeam() == eTeam)
				{
					return true;
				}

				if(pLoopPlot->plotCheck(PUF_isCombatTeam, eTeam, getTeam()) != NULL)
				{
					return true;
				}
			}
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canNuke(const CvPlot* /*pPlot*/) const
{
	VALIDATE_OBJECT
	if(GetNukeDamageLevel() == -1)
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canNukeAt(const CvPlot* pPlot, int iX, int iY) const
{
	VALIDATE_OBJECT

	if(!canNuke(pPlot))
	{
		return false;
	}

	int iDistance = plotDistance(pPlot->getX(), pPlot->getY(), iX, iY);

	if(iDistance <= 0)
		return false;	// Don't let the user nuke the same plot they are on.

	int iRange  = GetRange();
	if(iRange > 0 && iDistance > iRange)  // Range 0 can hit anywhere
	{
		return false;
	}

	for(int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if(GET_TEAM((TeamTypes)iI).isAlive())
		{
			CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);
			if(pTargetPlot)
			{
				TeamTypes eTheirTeam = (TeamTypes)iI;
				if(isNukeVictim(pTargetPlot, eTheirTeam))
				{
					CvTeam& myTeam = GET_TEAM(getTeam());
#if defined(MOD_EVENTS_WAR_AND_PEACE)
					if(!myTeam.isAtWar(eTheirTeam) && !myTeam.canDeclareWar(eTheirTeam, getOwner()))
#else
					if(!myTeam.isAtWar(eTheirTeam) && !myTeam.canDeclareWar(eTheirTeam))
#endif
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canParadrop(const CvPlot* pPlot, bool bOnlyTestVisibility) const
{
	VALIDATE_OBJECT
	if(getDropRange() <= 0)
	{
		return false;
	}

	// Things we check when we want to know if the unit can actually drop RIGHT NOW
	if(!bOnlyTestVisibility)
	{
		if(hasMoved())
		{
			return false;
		}

		if(isEmbarked())
		{
			return false;
		}

		if(pPlot->IsFriendlyTerritory(getOwner()))
		{
			// We're in friendly territory, call the event to see if we CAN'T start from here anyway
#if defined(MOD_EVENTS_PARADROPS)
			if (MOD_EVENTS_PARADROPS) {
				if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_CannotParadropFrom, getOwner(), GetID(), pPlot->getX(), pPlot->getY()) == GAMEEVENTRETURN_TRUE) {
					return false;
				}
			} else {
#endif
			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if (pkScriptSystem) 
			{
				CvLuaArgsHandle args;
				args->Push(((int)getOwner()));
				args->Push(GetID());
				args->Push(pPlot->getX());
				args->Push(pPlot->getY());

				// Attempt to execute the game events.
				// Will return false if there are no registered listeners.
				bool bResult = false;
				if (LuaSupport::CallTestAll(pkScriptSystem, "CannotParadropFrom", args.get(), bResult))
				{
					if (bResult == true)
					{
						return false;
					}
				}
			}
#if defined(MOD_EVENTS_PARADROPS)
			}
#endif
		}
		else
		{
			// We're not in friendly territory, call the event to see if we CAN start from here anyway
#if defined(MOD_EVENTS_PARADROPS)
			if (MOD_EVENTS_PARADROPS) {
				if (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_CanParadropFrom, getOwner(), GetID(), pPlot->getX(), pPlot->getY()) == GAMEEVENTRETURN_TRUE) {
					return true;
				}
			} else {
#endif
			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if (pkScriptSystem) {
				CvLuaArgsHandle args;
				args->Push(((int)getOwner()));
				args->Push(GetID());
				args->Push(pPlot->getX());
				args->Push(pPlot->getY());

				// Attempt to execute the game events.
				// Will return false if there are no registered listeners.
				bool bResult = false;
				if (LuaSupport::CallTestAny(pkScriptSystem, "CanParadropFrom", args.get(), bResult))
				{
					if (bResult == true)
					{
						return true;
					}
				}
			}
#if defined(MOD_EVENTS_PARADROPS)
			}
#endif

			return false;
		}
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canParadropAt(const CvPlot* pPlot, int iX, int iY) const
{
	VALIDATE_OBJECT
	if(!canParadrop(pPlot, false))
	{
		return false;
	}

	CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);
	if(NULL == pTargetPlot || pTargetPlot == pPlot)
	{
		return false;
	}

	if(!pTargetPlot->isVisible(getTeam()))
	{
		return false;
	}

	if(!canMoveInto(*pTargetPlot, MOVEFLAG_DESTINATION))
	{
		return false;
	}

	if(plotDistance(pPlot->getX(), pPlot->getY(), iX, iY) > getDropRange())
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::paradrop(int iX, int iY)
{
	VALIDATE_OBJECT
	if(!canParadropAt(plot(), iX, iY))
	{
		return false;
	}

	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	if(! pPlot)
	{
		return false;
	}

#if defined(MOD_GLOBAL_PARATROOPS_MOVEMENT)
	// If dropping less than max range, let the unit make an attack
	int iRange = getDropRange();
	int iDist = plotDistance(plot()->getX(), plot()->getY(), iX, iY);
	bool bCanAttack = MOD_GLOBAL_PARATROOPS_MOVEMENT && (iDist < iRange);
	if (bCanAttack) {
		// CUSTOMLOG("Paradrop - %i of %i is less than range --> can attack", iDist, iRange);
		changeMoves(-GC.getMOVE_DENOMINATOR()); // This equates to a "set-up to range attack", which seems fair
		// setMadeAttack(false); // Don't set to false, just in case someone modded in "attack then paralift out"
	} else {
		// CUSTOMLOG("Paradrop - %i of %i is max range --> cannot attack", iDist, iRange);
#endif
		changeMoves(-(GC.getMOVE_DENOMINATOR() / 2));
		setMadeAttack(true);
#if defined(MOD_GLOBAL_PARATROOPS_MOVEMENT)
	}
#endif

	CvPlot* fromPlot = plot();
	//JON: CHECK FOR INTERCEPTION HERE
#if defined(MOD_GLOBAL_PARATROOPS_AA_DAMAGE)
	if (MOD_GLOBAL_PARATROOPS_AA_DAMAGE) {
		if (CvUnitCombat::ParadropIntercept(*this, *pPlot)) {
			// Unit died during the drop
			return false;
		}
	}
#endif

	//play paradrop animation
	if(pPlot->isActiveVisible(false))
	{
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitParadrop(pDllUnit.get());
	}
	setXY(pPlot->getX(), pPlot->getY(), true, true, false);

#if defined(MOD_EVENTS_PARADROPS)
	if (MOD_EVENTS_PARADROPS) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_ParadropAt, getOwner(), GetID(), fromPlot->getX(), fromPlot->getY(), pPlot->getX(), pPlot->getY());
	} else {
#endif
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(((int)getOwner()));
		args->Push(GetID());
		args->Push(fromPlot->getX());
		args->Push(fromPlot->getY());
		args->Push(pPlot->getX());
		args->Push(pPlot->getY());

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "ParadropAt", args.get(), bResult);
	}
#if defined(MOD_EVENTS_PARADROPS)
	}
#endif

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canMakeTradeRoute(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if (!isTrade())
	{
		return false;
	}

	if (pPlot == NULL)
	{
		return false;
	}

	CvCity* pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		return false;
	}

	if (pCity->getOwner() != getOwner())
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canMakeTradeRouteAt(const CvPlot* pPlot, int iX, int iY, TradeConnectionType eConnectionType) const
{
	if (!canMakeTradeRoute(pPlot))
	{
		return false;
	}

	if (pPlot == NULL)
	{
		return false;
	}

	CvCity* pFromCity = pPlot->getPlotCity();
	if (pFromCity == NULL)
	{
		return false;
	}

	// you must launch a trade mission from one of your own cities
	if (pFromCity->getOwner() != getOwner())
	{
		return false;
	}

	CvPlot* pToPlot = GC.getMap().plot(iX, iY);

	// Null plot...
	if(pToPlot == NULL)
	{
		return false;
	}

	// Same plot...
	if(pToPlot == pPlot)
	{
		return false;
	}

	CvCity* pToCity = pToPlot->getPlotCity();
	if (pToCity == NULL)
	{
		return false;
	}

	if (!GET_PLAYER(getOwner()).GetTrade()->CanCreateTradeRoute(pFromCity, pToCity, getDomainType(), eConnectionType, false))
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::makeTradeRoute(int iX, int iY, TradeConnectionType eConnectionType)
{
	if (!canMakeTradeRouteAt(plot(), iX, iY, eConnectionType))
	{
		return false;
	}

	CvCity* pFromCity = NULL;
	CvCity* pToCity = NULL;

	if (plot())
	{
		pFromCity = plot()->getPlotCity();
	}

	CvPlot* pToPlot = GC.getMap().plot(iX, iY);
	if (pToPlot)
	{
		pToCity = pToPlot->getPlotCity();
	}

	bool bResult = GET_PLAYER(getOwner()).GetTrade()->CreateTradeRoute(pFromCity, pToCity, getDomainType(), eConnectionType);

	if (bResult)
		kill(true);

	return bResult;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canChangeTradeUnitHomeCity(const CvPlot* pPlot) const
{
	if (!isTrade())
	{
		return false;
	}

	if (pPlot == NULL)
	{
		return false;
	}

	CvCity* pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		return false;
	}

	if (pCity->getOwner() != getOwner())
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canChangeTradeUnitHomeCityAt(const CvPlot* pPlot, int iX, int iY) const
{
	if (!canChangeTradeUnitHomeCity(pPlot))
	{
		return false;
	}

	if (pPlot == NULL)
	{
		return false;
	}

	CvCity* pFromCity = pPlot->getPlotCity();
	if (pFromCity == NULL)
	{
		return false;
	}

	// you must launch a trade mission from one of your own cities
	if (pFromCity->getOwner() != getOwner())
	{
		return false;
	}

	CvPlot* pToPlot = GC.getMap().plot(iX, iY);

	// Null plot...
	if(pToPlot == NULL)
	{
		return false;
	}

	// Same plot...
	if(pToPlot == pPlot)
	{
		return false;
	}

	CvCity* pToCity = pToPlot->getPlotCity();
	if (pToCity == NULL)
	{
		return false;
	}

	// can't change cities with a city you don't own
	if (pToCity->getOwner() != getOwner())
	{
		return false;
	}

	if (getDomainType() == DOMAIN_SEA)
	{
#if defined(MOD_BUGFIX_MINOR)
		// The path finder permits routes between cities on lakes,
		// so we'd better allow cargo ships to be relocated there!
		if (!pToCity->isCoastal(0))
#else
		if (!pToCity->isCoastal())
#endif
		{
			return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::changeTradeUnitHomeCity(int iX, int iY)
{
	if (!canChangeTradeUnitHomeCityAt(plot(), iX, iY))
	{
		return false;
	}

	setXY(iX, iY);
	setMoves(0);
	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canChangeAdmiralPort(const CvPlot* pPlot) const
{
#if defined(MOD_GLOBAL_SEPARATE_GREAT_ADMIRAL)
	bool bHasSkill = !MOD_GLOBAL_SEPARATE_GREAT_ADMIRAL && IsGreatAdmiral();
	bHasSkill = bHasSkill || (MOD_GLOBAL_SEPARATE_GREAT_ADMIRAL && m_pUnitInfo->IsCanChangePort());
	if (!bHasSkill)
#else
	if (!IsGreatAdmiral())
#endif
	{
		return false;
	}

	if (pPlot == NULL)
	{
		return false;
	}

	CvCity* pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		return false;
	}

	if (pCity->getOwner() != getOwner())
	{
		return false;
	}

	if (!pCity->isCoastal())
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canChangeAdmiralPortAt(const CvPlot* pPlot, int iX, int iY) const
{
	if (!canChangeAdmiralPort(pPlot))
	{
		return false;
	}

	if (pPlot == NULL)
	{
		return false;
	}

	CvCity* pFromCity = pPlot->getPlotCity();
	if (pFromCity == NULL)
	{
		return false;
	}

	if (pFromCity->getOwner() != getOwner())
	{
		return false;
	}

	CvPlot* pToPlot = GC.getMap().plot(iX, iY);

	// Null plot...
	if(pToPlot == NULL)
	{
		return false;
	}

	// Same plot...
	if(pToPlot == pPlot)
	{
		return false;
	}

	CvCity* pToCity = pToPlot->getPlotCity();
	if (pToCity == NULL)
	{
		return false;
	}

	// can't change cities with a city you don't own
	if (pToCity->getOwner() != getOwner())
	{
		return false;
	}

	if (!pToCity->isCoastal())
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::changeAdmiralPort(int iX, int iY)
{
	if (!canChangeAdmiralPortAt(plot(), iX, iY))
	{
		return false;
	}

	setXY(iX, iY);
	setMoves(0);
	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canPlunderTradeRoute(const CvPlot* pPlot, bool bOnlyTestVisibility) const
{
	if (!IsCombatUnit())
	{
		return false;
	}

	if (pPlot == NULL)
	{
		return false;
	}

	if (isEmbarked())
	{
		return false;
	}
	
#if defined(MOD_GLOBAL_NO_OCEAN_PLUNDERING)
	if (MOD_GLOBAL_NO_OCEAN_PLUNDERING && pPlot->isWater() && !pPlot->isShallowWater())
	{
		return false;
	}
#endif

	if (GET_PLAYER(m_eOwner).GetTrade()->ContainsOpposingPlayerTradeUnit(pPlot))
	{
		if (!bOnlyTestVisibility)
		{
			std::vector<int> aiTradeUnitsAtPlot;
			aiTradeUnitsAtPlot = GET_PLAYER(m_eOwner).GetTrade()->GetOpposingTradeUnitsAtPlot(pPlot, true);
			if (aiTradeUnitsAtPlot.size() <= 0)
			{
				return false;
			}

			PlayerTypes eTradeUnitOwner = GC.getGame().GetGameTrade()->GetOwnerFromID(aiTradeUnitsAtPlot[0]);
			if (eTradeUnitOwner == NO_PLAYER)
			{
				// invalid TradeUnit
				return false;
			}

			TeamTypes eTeam = GET_PLAYER(eTradeUnitOwner).getTeam();
			if (!GET_TEAM(GET_PLAYER(m_eOwner).getTeam()).isAtWar(eTeam))
			{
				return false;
			}
#if defined(MOD_BALANCE_CORE)
			if(GET_PLAYER(m_eOwner).AreTradeRoutesInvulnerable())
			{
				return false;
			}
#endif
		}

		return true;
	}
	else
	{
		return false;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::plunderTradeRoute()
{
	CvPlot* pPlot = plot();
	if (!canPlunderTradeRoute(pPlot, false))
	{
		return false;
	}

	std::vector<int> aiTradeUnitsAtPlot;
	CvPlayerTrade* pTrade = GET_PLAYER(m_eOwner).GetTrade();
	aiTradeUnitsAtPlot = pTrade->GetOpposingTradeUnitsAtPlot(pPlot, false);
	CvAssertMsg(aiTradeUnitsAtPlot.size() > 0, "aiTradeUnitsAtPlot is empty. Earlier check should have verified that this can't be the case");
	if (aiTradeUnitsAtPlot.size() <= 0)
	{
		return false;
	}

	// right now, plunder the first unit
#if defined(MOD_API_EXTENSIONS)
	pTrade->PlunderTradeRoute(aiTradeUnitsAtPlot[0], this);
#else
	pTrade->PlunderTradeRoute(aiTradeUnitsAtPlot[0]);
#endif
	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canCreateGreatWork(const CvPlot* pPlot, bool bOnlyTestVisibility) const
{
	// Unit class has this flag?
	if(GetGreatWork() == NO_GREAT_WORK)
	{
		return false;
	}

	const TeamTypes eTeam = getTeam();
	if(GET_TEAM(eTeam).isMinorCiv())
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

	//If we're only testing whether this unit has the capabilty to ever perform the action, return true.
	if (bOnlyTestVisibility)
	{
		return true;
	}

	// In or adjacent to a city?
	CvCity* pCity;
	if (pPlot == NULL)
	{
		return false;
	}
	pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		pCity = pPlot->GetAdjacentCity();
		if (pCity == NULL)
		{
			return false;
		}
	}

	// ... is this a friendly city?
	if (eTeam != pCity->getTeam())
	{
		return false;
	}

	// Is there a slot of the right type somewhere in the empire?
	GreatWorkSlotType eGreatWorkSlot = CultureHelpers::GetGreatWorkSlot(GetGreatWork());
	CvPlayer &kPlayer = GET_PLAYER(getOwner());
	if (kPlayer.GetCulture()->GetNumAvailableGreatWorkSlots(eGreatWorkSlot) > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::createGreatWork()
{
	CvPlot* pPlot = plot();
	if (!canCreateGreatWork(pPlot, false))
	{
		return false;
	}

	CvGameCulture *pCulture = GC.getGame().GetGameCulture();
	if(pCulture == NULL)
	{
		CvAssertMsg(pCulture != NULL, "This should never happen.");
		return false;
	}
	
	CvPlayer &kPlayer = GET_PLAYER(m_eOwner);
	BuildingClassTypes eBuildingClass = NO_BUILDINGCLASS; // Passed by reference below
	int iSlot = -1; // Passed by reference below
	GreatWorkType eGreatWorkType = GetGreatWork();
	GreatWorkClass eClass = CultureHelpers::GetGreatWorkClass(eGreatWorkType);
	GreatWorkSlotType eGreatWorkSlot = CultureHelpers::GetGreatWorkSlot(eGreatWorkType);
	
	CvCity *pCity = kPlayer.GetCulture()->GetClosestAvailableGreatWorkSlot(getX(), getY(), eGreatWorkSlot, &eBuildingClass, &iSlot);
	if (pCity)
	{
		int iGWindex = pCulture->CreateGreatWork(eGreatWorkType, eClass, m_eOwner, kPlayer.GetCurrentEra(), getName());
		pCity->GetCityBuildings()->SetBuildingGreatWork(eBuildingClass, iSlot, iGWindex);

		if(pPlot->isActiveVisible(false))
		{
			auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
			gDLL->GameplayUnitActivate(pDllUnit.get());
		}
#if defined(MOD_BALANCE_CORE)
		pCity->GetCityCulture()->CalculateBaseTourismBeforeModifiers();
		pCity->GetCityCulture()->CalculateBaseTourism();
#endif
		if(IsGreatPerson())
		{
#if defined(MOD_EVENTS_GREAT_PEOPLE)
			kPlayer.DoGreatPersonExpended(getUnitType(), this);
#else
			kPlayer.DoGreatPersonExpended(getUnitType());
#endif
		}

		kill(true);

		bool bDontShowRewardPopup = GC.GetEngineUserInterface()->IsOptionNoRewardPopups();
		Localization::String localizedText;

		// Notification in MP games
#if defined(MOD_API_EXTENSIONS)
		if(bDontShowRewardPopup || GC.getGame().isReallyNetworkMultiPlayer())
#else
		if(bDontShowRewardPopup || GC.getGame().isNetworkMultiPlayer())
#endif
		{
			CvNotifications* pNotifications = kPlayer.GetNotifications();
			if(pNotifications)
			{
				localizedText = Localization::Lookup("TXT_KEY_MISC_WONDER_COMPLETED");
				localizedText << kPlayer.getNameKey() << pCulture->GetGreatWorkName(iGWindex);
				pNotifications->Add(NOTIFICATION_GREAT_WORK_COMPLETED_ACTIVE_PLAYER, localizedText.toUTF8(), localizedText.toUTF8(), getX(), getY(), iGWindex, kPlayer.GetID());
			}
		}
		// Popup in SP games
		else
		{
			if(kPlayer.GetID() == GC.getGame().getActivePlayer())
			{
				CvPopupInfo kPopup(BUTTONPOPUP_GREAT_WORK_COMPLETED_ACTIVE_PLAYER, iGWindex);
				GC.GetEngineUserInterface()->AddPopup(kPopup);
			}
		}
#if defined(MOD_BALANCE_CORE)
		CvGameCulture *pCulture = GC.getGame().GetGameCulture();
		if(pCulture != NULL)
		{
			int iValue = (int)eGreatWorkType;
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_GreatWorkCreated, getOwner(), GetID(), iValue);
		}
#endif

		return true;
	}

	return false;
}
#if defined(MOD_BALANCE_CORE)
bool CvUnit::canGetFreeLuxury() const
{
	if(plot()->getOwner() == getOwner())
	{
		if(m_pUnitInfo->GetNumFreeLux() > 0)
		{
			return true;
		}
	}
	return false;
}
bool CvUnit::createFreeLuxury()
{
	int iNumLuxuries = m_pUnitInfo->GetNumFreeLux();
	bool bResult = false;
	if(iNumLuxuries > 0)
	{
		// Loop through all resources and see if we can find this many unique ones
		ResourceTypes eResourceToGive = NO_RESOURCE;
		int iBestFlavor = 0;
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			ResourceTypes eResource = (ResourceTypes) iResourceLoop;
			CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
			if (pkResource != NULL && pkResource->getResourceUsage() == RESOURCEUSAGE_LUXURY)
			{
				if(GC.getMap().getNumResources(eResource) <= 0)
				{
					int iRandomFlavor = GC.getGame().getJonRandNum(100, "Resource Flavor");
					//If we've already got this resource, divide the value by the amount.
					if(GET_PLAYER(getOwner()).getNumResourceTotal(eResource, false) > 0)
					{
						iRandomFlavor = 0;
					}
					if(iRandomFlavor > iBestFlavor)
					{
						eResourceToGive = eResource;
						iBestFlavor = iRandomFlavor;
					}
				}
			}
		}
		if (eResourceToGive == NO_RESOURCE)
		{
			for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				ResourceTypes eResource = (ResourceTypes) iResourceLoop;
				CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
				if (pkResource != NULL && pkResource->getResourceUsage() == RESOURCEUSAGE_LUXURY)
				{
					int iRandomFlavor = GC.getGame().getJonRandNum(100, "Resource Flavor");
					//If we've already got this resource, divide the value by the amount.
					if(GET_PLAYER(getOwner()).getNumResourceTotal(eResource, false) > 0)
					{
						iRandomFlavor = 0;
					}
					if(iRandomFlavor > iBestFlavor)
					{
						eResourceToGive = eResource;
						iBestFlavor = iRandomFlavor;
					}
				}
			}
		}
		if (eResourceToGive == NO_RESOURCE)
		{
			for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				ResourceTypes eResource = (ResourceTypes) iResourceLoop;
				CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
				if (pkResource != NULL && pkResource->getResourceUsage() == RESOURCEUSAGE_LUXURY)
				{
					int iRandomFlavor = GC.getGame().getJonRandNum(100, "Resource Flavor");
					if(iRandomFlavor > iBestFlavor)
					{
						eResourceToGive = eResource;
						iBestFlavor = iRandomFlavor;
					}
				}
			}
		}
		if (eResourceToGive != NO_RESOURCE)
		{
			GET_PLAYER(getOwner()).changeNumResourceTotal(eResourceToGive, iNumLuxuries);
			CvPlayerAI& kOwner = GET_PLAYER(getOwner());
			if(kOwner.isHuman())
			{
				CvNotifications* pNotifications = kOwner.GetNotifications();
				if(pNotifications)
				{
					CvResourceInfo* pInfo = GC.getResourceInfo(eResourceToGive);
					if (pInfo)
					{
						Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_FREE_RESOURCE");
						strText <<  getNameKey() << pInfo->GetTextKey();
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_FREE_RESOURCE_SUMMARY");
						strSummary << getNameKey();
						pNotifications->Add(NOTIFICATION_DISCOVERED_LUXURY_RESOURCE, strText.toUTF8(), strSummary.toUTF8(), getX(), getY(), eResourceToGive);
					}
				}
			}
#if defined(MOD_EVENTS_GREAT_PEOPLE)
			kOwner.DoGreatPersonExpended(getUnitType(), this);
#else
			kOwner.DoGreatPersonExpended(getUnitType());
#endif
			kill(true);
			bResult = true;
		}
	}

	return bResult;
}
#endif
//	--------------------------------------------------------------------------------
int CvUnit::getNumExoticGoods() const
{
	return m_iNumExoticGoods;
}

//	--------------------------------------------------------------------------------
void CvUnit::setNumExoticGoods(int iValue)
{
	CvAssert(iValue >= 0);
	m_iNumExoticGoods = iValue;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeNumExoticGoods(int iChange)
{
	setNumExoticGoods(getNumExoticGoods() + iChange);
}

//	--------------------------------------------------------------------------------
float CvUnit::calculateExoticGoodsDistanceFactor(const CvPlot* pPlot)
{
	float fDistanceFactor = 0.0f;

	CvCity* pCapital = GET_PLAYER(getOwner()).getCapitalCity();
	if (pPlot && pCapital)
	{
		int iDistanceThreshold = (GC.getMap().getGridWidth() + GC.getMap().getGridHeight()) / 2;
		fDistanceFactor = plotDistance(pPlot->getX(), pPlot->getY(), pCapital->getX(), pCapital->getY()) / (float) iDistanceThreshold;
	}

	return fDistanceFactor;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canSellExoticGoods(const CvPlot* pPlot, bool bOnlyTestVisibility) const
{
	if (pPlot == NULL)
	{
		return false;
	}

	if (getNumExoticGoods() <= 0)
	{
		return false;
	}

	if (!bOnlyTestVisibility)
	{
		int iNumValidPlots = 0;
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			CvPlot* pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));
			if (pLoopPlot != NULL)
			{
				PlayerTypes eLoopPlotOwner = pLoopPlot->getOwner();
				if (eLoopPlotOwner != getOwner() && eLoopPlotOwner != NO_PLAYER)
				{
					if (!GET_TEAM(getTeam()).isAtWar(GET_PLAYER(eLoopPlotOwner).getTeam()))
					{
#if defined(MOD_BALANCE_CORE_PORTUGAL_CHANGE)
						if(MOD_BALANCE_CORE_PORTUGAL_CHANGE && GET_PLAYER(eLoopPlotOwner).isMinorCiv())
						{
#endif
						iNumValidPlots++;
						break;
#if defined(MOD_BALANCE_CORE_PORTUGAL_CHANGE)
						}
						else
						{
							iNumValidPlots++;
							break;
						}
#endif
					}
				}
			}
		}

		if (iNumValidPlots <= 0)
		{
			return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
int CvUnit::getExoticGoodsGoldAmount()
{
	int iValue = 0;
	if (canSellExoticGoods(plot()))
	{
		float fDistanceFactor = calculateExoticGoodsDistanceFactor(plot());

		int iExtraGold = GC.getEXOTIC_GOODS_GOLD_MAX() - GC.getEXOTIC_GOODS_GOLD_MIN();
		iValue = GC.getEXOTIC_GOODS_GOLD_MIN() + (int)(iExtraGold * fDistanceFactor);
		iValue = MIN(iValue, GC.getEXOTIC_GOODS_GOLD_MAX());
	}
	return iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::getExoticGoodsXPAmount()
{
	int iValue = 0;
	if (canSellExoticGoods(plot()))
	{
		float fDistanceFactor = calculateExoticGoodsDistanceFactor(plot());

		int iExtraXP = GC.getEXOTIC_GOODS_XP_MAX() - GC.getEXOTIC_GOODS_XP_MIN();
		iValue = GC.getEXOTIC_GOODS_XP_MIN() + (int)(iExtraXP * fDistanceFactor);
		iValue = MIN(iValue, GC.getEXOTIC_GOODS_XP_MAX());
	}
	return iValue;
}

//	--------------------------------------------------------------------------------
bool CvUnit::sellExoticGoods()
{
	if (canSellExoticGoods(plot()))
	{
		int iXP = getExoticGoodsXPAmount();
		int iGold = getExoticGoodsGoldAmount();
		changeExperience(iXP);
		GET_PLAYER(getOwner()).GetTreasury()->ChangeGold(iGold);
		char text[256] = {0};
		sprintf_s(text, "[COLOR_YELLOW]+%d[ENDCOLOR][ICON_GOLD]", iGold);
#if defined(SHOW_PLOT_POPUP)
		SHOW_PLOT_POPUP(plot(), getOwner(), text, 0.0f);
#else
		float fDelay = 0.0f;
		DLLUI->AddPopupText(getX(), getY(), text, fDelay);
#endif

		changeNumExoticGoods(-1);
#if defined(MOD_BALANCE_CORE)
		PlayerTypes ePlotOwner = NO_PLAYER;
		ImprovementTypes eFeitoria = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_FEITORIA");
		if (eFeitoria != NO_IMPROVEMENT)
		{
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				CvPlot* pLoopPlotSearch = plotDirection(plot()->getX(), plot()->getY(), ((DirectionTypes)iI));
				if (pLoopPlotSearch != NULL)
				{
					PlayerTypes eLoopPlotOwner = pLoopPlotSearch->getOwner();
					if (eLoopPlotOwner != NO_PLAYER)
					{
						if (!GET_TEAM(getTeam()).isAtWar(GET_PLAYER(eLoopPlotOwner).getTeam()))
						{
							if(GET_PLAYER(eLoopPlotOwner).isMinorCiv())
							{
								ePlotOwner = eLoopPlotOwner;
								break;
							}
						}
					}
				}
			}
			if(ePlotOwner != NO_PLAYER)
			{
				bool bAlreadyHere = false;
				CvPlot* pBestPlot = NULL;
				CvCity* pCity = GET_PLAYER(ePlotOwner).getCapitalCity();
				if(pCity != NULL)
				{

					for (int iCityPlotLoop = 0; iCityPlotLoop < pCity->GetNumWorkablePlots(); iCityPlotLoop++)
					{
						
						CvPlot* pLoopPlot = pCity->GetCityCitizens()->GetCityPlotFromIndex(iCityPlotLoop);
						if(pLoopPlot != NULL && (pLoopPlot->getOwner() == ePlotOwner) && !pLoopPlot->isCity() && !pLoopPlot->isWater() && !pLoopPlot->isImpassable(getTeam()) && !pLoopPlot->IsNaturalWonder() && pLoopPlot->isCoastalLand() && (pLoopPlot->getResourceType() == NO_RESOURCE))
						{
							if(pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
							{
								if(pLoopPlot->getImprovementType() == eFeitoria)
								{
									bAlreadyHere = true;
									break;
								}

							}
						}
					}
					if(!bAlreadyHere)
					{
						for (int iCityPlotLoop = 0; iCityPlotLoop < pCity->GetNumWorkablePlots(); iCityPlotLoop++)
						{
							CvPlot* pLoopPlot = pCity->GetCityCitizens()->GetCityPlotFromIndex(iCityPlotLoop);
							if(pLoopPlot != NULL && (pLoopPlot->getOwner() == ePlotOwner) && !pLoopPlot->isCity() && !pLoopPlot->isWater() && !pLoopPlot->isImpassable(getTeam()) && !pLoopPlot->IsNaturalWonder() && pLoopPlot->isCoastalLand() && (pLoopPlot->getResourceType() == NO_RESOURCE))
							{
								//If we can build on an empty spot, do so.
								if(pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
								{
									pBestPlot = pLoopPlot;
									break;
								}
								//If not, let's clear a basic improvement off.
								else
								{
									CvImprovementEntry* pImprovementInfo = GC.getImprovementInfo(pLoopPlot->getImprovementType());
									if(pImprovementInfo && !pImprovementInfo->IsPermanent() && !pImprovementInfo->IsCreatedByGreatPerson())
									{
										pBestPlot = pLoopPlot;
									}
								}
							}
						}
					}
					if(pBestPlot != NULL && !bAlreadyHere)
					{
						pBestPlot->setImprovementType(NO_IMPROVEMENT);
						pBestPlot->setImprovementType(eFeitoria, getOwner());
					}
				}
			}
		}
		return true;
#endif
	}
	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canRebase(const CvPlot* /*pPlot*/) const
{
	// Must be an air unit
	if(getDomainType() != DOMAIN_AIR)
	{
		return false;
	}

	// Must be an "immobile" air unit (e.g. no helicopters)
	if(!IsImmobile())
	{
		return false;
	}

	// Must have movement points left this turn
	if(getMoves() <= 0)
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canRebaseAt(const CvPlot* pPlot, int iX, int iY) const
{
	// If we can't rebase ANYWHERE then we definitely can't rebase at this X,Y
	if(!canRebase(pPlot))
	{
		return false;
	}

	CvPlot* pToPlot = GC.getMap().plot(iX, iY);

	// Null plot...
	if(pToPlot == NULL)
	{
		return false;
	}

	// Same plot...
	if(pToPlot == pPlot)
	{
		return false;
	}

	// too far
	int iRange = GetRange();
	iRange *= /*200*/ GC.getAIR_UNIT_REBASE_RANGE_MULTIPLIER();
	iRange /= 100;

	if(plotDistance(getX(), getY(), iX, iY) > iRange)
	{
		return false;
	}

	// Can't load to the target plot
	if(!canLoad(*pToPlot))
	{
		return false;
	}

	// Rebase to a City?
	bool bCityToRebase = false;
	if(pToPlot->isCity())
	{
		// City must be owned by us
		if(pToPlot->getPlotCity()->getOwner() == getOwner())
		{
			bCityToRebase = true;
		}

		if (!bCityToRebase)
		{
#if defined(MOD_EVENTS_REBASE)
			if (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_CanRebaseInCity, getOwner(), GetID(), iX, iY) == GAMEEVENTRETURN_TRUE) {
				bCityToRebase = true;
			} else {
#endif
			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if (pkScriptSystem)
			{
				CvLuaArgsHandle args;
				args->Push(getOwner());
				args->Push(GetID());
				args->Push(iX);
				args->Push(iY);

				// Attempt to execute the game events.
				// Will return false if there are no registered listeners.
				bool bResult = false;
				if (LuaSupport::CallTestAny(pkScriptSystem, "CanRebaseInCity", args.get(), bResult))
				{
					// Check the result.
					if (bResult == true)
					{
						bCityToRebase = true;
					}
				}
			}
#if defined(MOD_EVENTS_REBASE)
			}
#endif
		}

		int iUnitsThere = pToPlot->countNumAirUnits(getTeam());
		if (iUnitsThere >= pToPlot->getPlotCity()->GetMaxAirUnits())
		{
			return false;
		}
	}

	// Rebase onto Unit which can hold cargo
	bool bUnitToRebase = false;

	if(!bCityToRebase)
	{
		IDInfo* pUnitNode;
		CvUnit* pLoopUnit;
		pUnitNode = pToPlot->headUnitNode();
		while(pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(*pUnitNode);
			pUnitNode = pToPlot->nextUnitNode(pUnitNode);

			// Can this Unit hold us?
			if(canLoadUnit(*pLoopUnit, *pToPlot))
			{
				bUnitToRebase = true;

				break;
			}
		}
	}

	// No City or Unit at the target to rebase to
	if(!bCityToRebase && !bUnitToRebase)
	{
#if defined(MOD_EVENTS_REBASE)
		if (MOD_EVENTS_REBASE) {
			if (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_CanRebaseTo, getOwner(), GetID(), iX, iY, bCityToRebase) == GAMEEVENTRETURN_TRUE) {
				return true;
			}
		} else {
#endif
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem) 
		{
			CvLuaArgsHandle args;
			args->Push(getOwner());
			args->Push(GetID());
			args->Push(iX);
			args->Push(iY);

			// Attempt to execute the game events.
			// Will return false if there are no registered listeners.
			bool bResult = false;
			if (LuaSupport::CallTestAny(pkScriptSystem, "CanRebaseTo", args.get(), bResult))
			{
				// Check the result.
				if (bResult == true)
				{
					return true;
				}
			}
		}
#if defined(MOD_EVENTS_REBASE)
		}
#endif

		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::rebase(int iX, int iY)
{
	if(!canRebaseAt(plot(), iX, iY))
	{
		return false;
	}

	CvPlot* oldPlot = plot();

	CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);
	CvAssert(pTargetPlot != NULL);
	if(pTargetPlot == NULL)
		return false;

	finishMoves();

	// Loses sight bonus for this turn
	setReconPlot(NULL);

	bool bShow = true;
	// Do the rebase first to keep the visualization in sequence
	if ((plot()->isVisibleToWatchingHuman() || pTargetPlot->isVisibleToWatchingHuman()) && !CvPreGame::quickMovement())
	{
		SpecialUnitTypes eSpecialUnitPlane = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_FIGHTER");
		if(getSpecialUnitType() == eSpecialUnitPlane)
		{
			auto_ptr<ICvPlot1> pDllOldPlot(new CvDllPlot(oldPlot));
			auto_ptr<ICvPlot1> pDllTargetPlot(new CvDllPlot(pTargetPlot));

			auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
			gDLL->GameplayUnitRebased(pDllUnit.get(), pDllOldPlot.get(), pDllTargetPlot.get());
			bShow = false;		// Tell the setXY to not bother showing the move
		}
	}

#if defined(MOD_EVENTS_REBASE)
	if (MOD_EVENTS_REBASE) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_RebaseTo, getOwner(), GetID(), iX, iY);
	} else {
#endif
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(getOwner());
		args->Push(GetID());
		args->Push(iX);
		args->Push(iY);

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "RebaseTo", args.get(), bResult);
	}
#if defined(MOD_EVENTS_REBASE)
	}
#endif

	setXY(pTargetPlot->getX(), pTargetPlot->getY(), false, bShow, false);

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canPillage(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(isEmbarked())
	{
		return false;
	}

	if(!(getUnitInfo().IsPillage()))
	{
		return false;
	}
#if defined(MOD_BALANCE_CORE)
	if(pPlot->getOwner() == getOwner())
	{
		return false;
	}
	if(pPlot->getOwner() == NO_PLAYER && pPlot->isRoute())
	{
		PlayerTypes eRouteOwner = pPlot->GetPlayerResponsibleForRoute();
		if(eRouteOwner != NO_PLAYER && GET_PLAYER(eRouteOwner).isAlive())
		{
			if (!atWar(getTeam(), GET_PLAYER(eRouteOwner).getTeam()))
			{
				return false;
			}
		}
	}
#endif
	else
	{
		TechTypes ePillagePrereq = (TechTypes) getUnitInfo().GetPrereqPillageTech();
		if(ePillagePrereq != NO_TECH)
		{
			if(!GET_TEAM(GET_PLAYER(getOwner()).getTeam()).GetTeamTechs()->HasTech(ePillagePrereq))
			{
				return false;
			}
		}
	}

	// Barbarian boats not allowed to pillage, as they're too annoying :)
	if(isBarbarian() && getDomainType() == DOMAIN_SEA)
	{
		return false;
	}

	if(pPlot->isCity())
	{
		return false;
	}

	ImprovementTypes eImprovementType = pPlot->getImprovementType();
	if(eImprovementType == NO_IMPROVEMENT)
	{
		if(!(pPlot->isRoute()))
		{
			return false;
		}
	}
	else if(eImprovementType == (ImprovementTypes)GC.getRUINS_IMPROVEMENT())
	{
		return false;
	}
	else
	{
		CvImprovementEntry* pImprovementInfo = GC.getImprovementInfo(pPlot->getImprovementType());
		if(pImprovementInfo->IsPermanent())
		{
			return false;
		}
		else if(pImprovementInfo->IsGoody())
		{
			return false;
		}

		// Special case: Feitoria can be in a city-state's lands, don't allow pillaging unless at war with its owner
		if(pImprovementInfo->GetLuxuryCopiesSiphonedFromMinor() > 0)
		{
			PlayerTypes eOwner = pPlot->getOwner();
			PlayerTypes eSiphoner = pPlot->GetPlayerThatBuiltImprovement();
			if (eOwner != NO_PLAYER && GET_PLAYER(eOwner).isMinorCiv())
			{
				if (eSiphoner != NO_PLAYER && GET_PLAYER(eSiphoner).isAlive())
				{
					if (!atWar(getTeam(), GET_PLAYER(eSiphoner).getTeam()))
					{
						return false;
					}
				}
			}
		}
	}

	// Either nothing to pillage or everything is pillaged to its max
	if((eImprovementType == NO_IMPROVEMENT || pPlot->IsImprovementPillaged()) &&
			(pPlot->getRouteType() == NO_ROUTE || pPlot->IsRoutePillaged() /* == GC.getPILLAGE_NUM_TURNS_DISABLED()*/))
	{
		return false;
	}

	if(pPlot->isOwned())
	{
		if(!potentialWarAction(pPlot))
		{
			if((eImprovementType == NO_IMPROVEMENT && !pPlot->isRoute()) || (pPlot->getOwner() != getOwner()))
			{
				return false;
			}
		}
	}

	// can no longer pillage our tiles
	if(pPlot->getOwner() == getOwner())
	{
		return false;
	}

	if(!(pPlot->isValidDomainForAction(*this)))
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::pillage()
{
	VALIDATE_OBJECT
	CvString strBuffer;

	ImprovementTypes eTempImprovement = NO_IMPROVEMENT;

	CvPlot* pPlot = plot();

	if(!canPillage(pPlot))
	{
		return false;
	}

	if(pPlot->isOwned())
	{
		// we should not be calling this without declaring war first, so do not declare war here
		if(!isEnemy(pPlot->getTeam(), pPlot))
		{
			if((pPlot->getImprovementType() == NO_IMPROVEMENT && !pPlot->isRoute()) || (pPlot->getOwner() != getOwner()))
			{
				return false;
			}
		}
	}

	bool bImprovement = false;

	// Has an Improvement
	if(pPlot->getImprovementType() != NO_IMPROVEMENT)
	{
		if(!pPlot->IsImprovementPillaged())
		{
			bImprovement = true;
		}
	}

	// Is an Improvement here that hasn't already been pillaged?
	bool bSuccessfulNonRoadPillage = false;
	if(bImprovement)
	{
		eTempImprovement = pPlot->getImprovementType();
		CvImprovementEntry* pkImprovement = GC.getImprovementInfo(eTempImprovement);
		if(pkImprovement)
		{
			if(pPlot->getTeam() != getTeam())
			{
#if defined(MOD_BALANCE_CORE)
				if((pPlot->getOwner() != NO_PLAYER && !isBarbarian() && !GET_PLAYER(pPlot->getOwner()).isBarbarian()) && GET_TEAM(getTeam()).isAtWar(GET_PLAYER(pPlot->getOwner()).getTeam()))
				{
					// Notify Diplo AI that damage has been done
					int iValue = (GC.getDEFAULT_WAR_VALUE_FOR_UNIT() / 3);
					if(pPlot->getResourceType(getTeam()) != NO_RESOURCE)
					{
						CvResourceInfo* pInfo = GC.getResourceInfo(pPlot->getResourceType(getTeam()));
						if (pInfo && pInfo->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
						{
							iValue *= 2;
						}
					}
					if(pkImprovement->IsCreatedByGreatPerson())
					{
						iValue *= 2;
					}

					// My viewpoint
					GET_PLAYER(getOwner()).GetDiplomacyAI()->ChangeOtherPlayerWarValueLost(pPlot->getOwner(), getOwner(), iValue);
					// Bad guy's viewpoint
					GET_PLAYER(pPlot->getOwner()).GetDiplomacyAI()->ChangeWarValueLost(getOwner(), iValue);
				}
#endif
				int iPillageGold = 0;

				// TODO: add scripting support for "doPillageGold"
				iPillageGold = GC.getGame().getJonRandNum(pkImprovement->GetPillageGold(), "Pillage Gold 1");
				iPillageGold += GC.getGame().getJonRandNum(pkImprovement->GetPillageGold(), "Pillage Gold 2");
				iPillageGold += (getPillageChange() * iPillageGold) / 100;

				if(iPillageGold > 0)
				{
					GET_PLAYER(getOwner()).GetTreasury()->ChangeGold(iPillageGold);

					if(getOwner() == GC.getGame().getActivePlayer())
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_PLUNDERED_GOLD_FROM_IMP", iPillageGold, pkImprovement->GetTextKey());
						DLLUI->AddUnitMessage(0, GetIDInfo(), getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_PILLAGE", MESSAGE_TYPE_INFO, m_pUnitInfo->GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX(), pPlot->getY()*/);
					}

					if(pPlot->isOwned() && pPlot->getOwner() == GC.getGame().getActivePlayer())
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_IMP_DESTROYED", pkImprovement->GetTextKey(), getNameKey(), getVisualCivAdjective(pPlot->getTeam()));
						DLLUI->AddPlotMessage(0, pPlot->GetPlotIndex(), pPlot->getOwner(), false, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_PILLAGED", MESSAGE_TYPE_INFO, m_pUnitInfo->GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX(), pPlot->getY(), true, true*/);
					}
				}
			}

#if !defined(NO_ACHIEVEMENTS)
			//Unlock any possible achievements.
			if(getOwner() == GC.getGame().getActivePlayer() && strcmp(pkImprovement->GetType(), "IMPROVEMENT_FARM") == 0)
				CvAchievementUnlocker::FarmImprovementPillaged();
#endif

			// Improvement that's destroyed?
			bSuccessfulNonRoadPillage = true;
			if(pkImprovement->IsDestroyedWhenPillaged())
			{
#if defined(MOD_GLOBAL_ALPINE_PASSES)
				// If this improvement auto-added a route, we also need to remove the route
				ImprovementTypes eOldImprovement = pPlot->getImprovementType();
				
				// Find the build for this improvement
				for (int i = 0; i < GC.getNumBuildInfos(); i++) {
					CvBuildInfo* pkBuild = GC.getBuildInfo((BuildTypes)i);
					
					if (pkBuild && ((ImprovementTypes)pkBuild->getImprovement()) == eOldImprovement) {
						// Found it, but did it auto-add a route?
						if (pkBuild->getRoute() != NO_ROUTE) {
							// Yes, so remove the route as well
							pPlot->setRouteType(NO_ROUTE);
						}
						
						// Our work here is done
						break;
					}
				}
#endif

				pPlot->setImprovementType(NO_IMPROVEMENT);
			}
			// Improvement that's pillaged?
			else
			{
				pPlot->SetImprovementPillaged(true);
			}

			if (pkImprovement->IsDisplacePillager())
			{
				jumpToNearestValidPlot();
			}
		}
	}
	else if(pPlot->isRoute())
	{
		pPlot->SetRoutePillaged(true);
	}

	if(!hasFreePillageMove())
	{
		changeMoves(-GC.getMOVE_DENOMINATOR());
	}

	if(bSuccessfulNonRoadPillage)
	{
		if (hasHealOnPillage())
		{
			// completely heal unit
			changeDamage(-getDamage());
		}
		else
		{
			int iHealAmount = min(getDamage(), GC.getPILLAGE_HEAL_AMOUNT());
			changeDamage(-iHealAmount);
		}
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canFound(const CvPlot* pPlot, bool bIgnoreDistanceToExistingCities, bool bIgnoreHappiness) const
{
	VALIDATE_OBJECT
	if(!m_pUnitInfo->IsFound())
	{
		if(!m_pUnitInfo->IsFoundAbroad())
		{
			return false;
		}
		else
		{
			CvCity* pCapital = GET_PLAYER(m_eOwner).getCapitalCity();
			if(!pCapital || pCapital->getArea() == pPlot->getArea())
			{
				return false;
			}
		}
	}

	if (pPlot)
		return GET_PLAYER(getOwner()).canFound(pPlot->getX(), pPlot->getY(), bIgnoreDistanceToExistingCities, bIgnoreHappiness, this);
	else
		return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::found()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();
	
#if defined(MOD_AI_SECONDARY_SETTLERS)
	if (IsCombatUnit()) {
		CUSTOMLOG("Trying to found a city with combat unit %s at (%i, %i)", getName().c_str(), getX(), getY());
	}
#endif	

	if(!canFound(pPlot))
	{
		return false;
	}

	PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();

	if(eActivePlayer == getOwner())
	{
		auto_ptr<ICvPlot1> pDllPlot = GC.WrapPlotPointer(pPlot);
		DLLUI->lookAt(pDllPlot.get(), CAMERALOOKAT_NORMAL);
	}

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
#if !defined(NO_ACHIEVEMENTS)
	CvPlayerAI& kActivePlayer = GET_PLAYER(eActivePlayer);
#endif

#if defined(MOD_GLOBAL_RELIGIOUS_SETTLERS)
	if (MOD_GLOBAL_RELIGIOUS_SETTLERS && GetReligionData()->GetReligion() > RELIGION_PANTHEON)
	{
		kPlayer.found(getX(), getY(), GetReligionData()->GetReligion());
	} 
	else 
	{
		kPlayer.found(getX(), getY(), NO_RELIGION, true);
#else
		kPlayer.found(getX(), getY());
#endif
#if defined(MOD_GLOBAL_RELIGIOUS_SETTLERS)
	}
#endif

#if defined(MOD_AI_SECONDARY_SETTLERS)
	if (IsCombatUnit() && plot()->getPlotCity() != NULL) {
		CUSTOMLOG("  ... success!  They founded %s", plot()->getPlotCity()->getName().c_str());
	}
#endif	

	if(pPlot->isActiveVisible(false))
	{
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());

#if !defined(NO_ACHIEVEMENTS)
		//Achievement
		if(eActivePlayer == getOwner() && kActivePlayer.getNumCities() >= 2 && kActivePlayer.isHuman() && !GC.getGame().isGameMultiPlayer())
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_SECOND_CITY);
		}
#endif
	}

#if defined(MOD_BALANCE_CORE_SETTLER_ADVANCED)
	if(MOD_BALANCE_CORE_SETTLER_ADVANCED && m_pUnitInfo->IsFoundMid())
	{
		kPlayer.cityBoost(getX(), getY(), m_pUnitInfo, GC.getPIONEER_EXTRA_PLOTS(), GC.getPIONEER_POPULATION_CHANGE(), GC.getPIONEER_FOOD_PERCENT());
	}
	if(MOD_BALANCE_CORE_SETTLER_ADVANCED && m_pUnitInfo->IsFoundLate())
	{
		kPlayer.cityBoost(getX(), getY(), m_pUnitInfo, GC.getCOLONIST_EXTRA_PLOTS(), GC.getCOLONIST_POPULATION_CHANGE(), GC.getCOLONIST_FOOD_PERCENT());
	}
#endif
#if defined(MOD_BALANCE_CORE)
	int iMaxRange = 3;
	for(int iDX = -iMaxRange; iDX <= iMaxRange; iDX++)
	{
		for(int iDY = -iMaxRange; iDY <= iMaxRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iDX, iDY, iMaxRange);
			if(pLoopPlot && pLoopPlot->getOwner() == getOwner())
			{
				pLoopPlot->verifyUnitValidPlot();
			}
		}
	}
#endif
		
#if defined(MOD_EVENTS_UNIT_FOUNDED)
	if (MOD_EVENTS_UNIT_FOUNDED) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_UnitCityFounded, getOwner(), GetID(), getUnitType(), getX(), getY());
	}
#endif
	auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
	gDLL->GameplayUnitVisibility(pDllUnit.get(), false);
	kill(true);

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canJoin(const CvPlot* /*pPlot*/, SpecialistTypes /*eSpecialist*/) const
{
	VALIDATE_OBJECT

	// JON: Great People can no longer join Cities as Specialists 08/18/09
	return false;
}


//	--------------------------------------------------------------------------------
bool CvUnit::join(SpecialistTypes eSpecialist)
{
	VALIDATE_OBJECT

	if(!canJoin(plot(), eSpecialist))
	{
		return false;
	}

	kill(true);

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canConstruct(const CvPlot* pPlot, BuildingTypes eBuilding) const
{
	VALIDATE_OBJECT
	CvCity* pCity;

	if(eBuilding == NO_BUILDING)
	{
		return false;
	}

	pCity = pPlot->getPlotCity();

	if(pCity == NULL)
	{
		return false;
	}

	if(getTeam() != pCity->getTeam())
	{
		return false;
	}

	if(pCity->GetCityBuildings()->GetNumRealBuilding(eBuilding) > 0)
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::construct(BuildingTypes eBuilding)
{
	VALIDATE_OBJECT
	CvCity* pCity;

	if(!canConstruct(plot(), eBuilding))
	{
		return false;
	}

	pCity = plot()->getPlotCity();

	if(pCity != NULL)
	{
		pCity->GetCityBuildings()->SetNumRealBuilding(eBuilding, pCity->GetCityBuildings()->GetNumRealBuilding(eBuilding) + 1);
	}

	if(plot()->isActiveVisible(false))
	{
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
	}

	kill(true);

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::CanFoundReligion(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	CvCity* pCity;
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();

	pCity = pPlot->getPlotCity();

	if(!m_pUnitInfo->IsFoundReligion())
	{
		return false;
	}

	if(pCity == NULL)
	{
		return false;
	}

	if(GET_PLAYER(getOwner()).GetReligions()->IsFoundingReligion())
	{
		return false;
	}

	if(getTeam() != pCity->getTeam())
	{
		return false;
	}

	if(GET_TEAM(getTeam()).isMinorCiv())
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}
#if defined(MOD_BALANCE_CORE)
	if(pReligions->GetNumReligionsStillToFound() <= 0 && !GET_PLAYER(getOwner()).GetPlayerTraits()->IsAlwaysReligion())
#else
	if(pReligions->GetNumReligionsStillToFound() <= 0)
#endif
	{
		return false;
	}

	if(pReligions->HasCreatedReligion(getOwner()))
	{
		return false;
	}

#if defined(MOD_EVENTS_FOUND_RELIGION)
	if (MOD_EVENTS_FOUND_RELIGION ) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanFoundReligion, getOwner(), pCity->GetID()) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	} else {
#endif
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem) 
	{
		CvLuaArgsHandle args;
		args->Push(getOwner());
		args->Push(pCity->GetID());

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		if (LuaSupport::CallTestAll(pkScriptSystem, "PlayerCanFoundReligion", args.get(), bResult))
		{
			if (bResult == false) 
			{
				return false;
			}
		}
	}
#if defined(MOD_EVENTS_FOUND_RELIGION)
	}
#endif

#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
	if (pReligions->GetAvailableFounderBeliefs(getOwner(), NO_RELIGION).size() < 1 || pReligions->GetAvailableFollowerBeliefs(getOwner(), NO_RELIGION).size() < 1)
#else
	if (pReligions->GetAvailableFounderBeliefs().size() < 1 || pReligions->GetAvailableFollowerBeliefs().size() < 1)
#endif
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::DoFoundReligion()
{
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();

	CvPlot* pkPlot = plot();
	CvCity* pkCity = (NULL != pkPlot)? pkPlot->getPlotCity() : NULL;

	if(pkPlot != NULL && pkCity != NULL)
	{
		if(CanFoundReligion(pkPlot))
		{
			CvPlayerAI& kOwner = GET_PLAYER(getOwner());
			if(kOwner.isHuman())
			{
				CvAssertMsg(pkCity != NULL, "No City??");

				CvNotifications* pNotifications = kOwner.GetNotifications();
				if(pNotifications)
				{
					CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_FOUND_RELIGION");
					CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_FOUND_RELIGION");
					pNotifications->Add(NOTIFICATION_FOUND_RELIGION, strBuffer, strSummary, pkPlot->getX(), pkPlot->getY(), -1, pkCity->GetID());
				}
				kOwner.GetReligions()->SetFoundingReligion(true);
#if defined(MOD_EVENTS_GREAT_PEOPLE)
				kOwner.DoGreatPersonExpended(getUnitType(), this);
#else
				kOwner.DoGreatPersonExpended(getUnitType());
#endif
				kill(true);
			}
			else
			{
				ReligionTypes eReligion = pReligions->GetReligionToFound(getOwner());
				if(eReligion != NO_RELIGION)
				{
					// Pick beliefs for this religion
					BeliefTypes eBeliefs[4];
					eBeliefs[2] = NO_BELIEF;
					eBeliefs[3] = NO_BELIEF;
					int iIndex = 0;
					if(!kOwner.GetReligions()->HasCreatedPantheon())
					{
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
						eBeliefs[iIndex] = kOwner.GetReligionAI()->ChoosePantheonBelief(kOwner.GetID());
#else
						eBeliefs[iIndex] = kOwner.GetReligionAI()->ChoosePantheonBelief();
#endif
						iIndex++;
					}
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
					eBeliefs[iIndex] = kOwner.GetReligionAI()->ChooseFollowerBelief(kOwner.GetID(), eReligion);
#else
					eBeliefs[iIndex] = kOwner.GetReligionAI()->ChooseFollowerBelief();
#endif
					iIndex++;
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
					eBeliefs[iIndex] = kOwner.GetReligionAI()->ChooseFounderBelief(kOwner.GetID(), eReligion);
#else
					eBeliefs[iIndex] = kOwner.GetReligionAI()->ChooseFounderBelief();
#endif
					iIndex++;

					if(kOwner.GetPlayerTraits()->IsBonusReligiousBelief())
					{
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
						eBeliefs[iIndex] = kOwner.GetReligionAI()->ChooseBonusBelief(kOwner.GetID(), eReligion, eBeliefs[0], eBeliefs[1], eBeliefs[2]);
#else
						eBeliefs[iIndex] = kOwner.GetReligionAI()->ChooseBonusBelief(eBeliefs[0], eBeliefs[1], eBeliefs[2]);
#endif
					}

					pReligions->FoundReligion(getOwner(), eReligion, NULL, eBeliefs[0], eBeliefs[1], eBeliefs[2], eBeliefs[3], pkCity);
#if defined(MOD_EVENTS_GREAT_PEOPLE)
					kOwner.DoGreatPersonExpended(getUnitType(), this);
#else
					kOwner.DoGreatPersonExpended(getUnitType());
#endif
					kill(true);
				}
				else
				{
					CvAssertMsg(false, "No religions available to found.");
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnit::CanEnhanceReligion(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	CvCity* pCity;
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();

	pCity = pPlot->getPlotCity();

	if(!m_pUnitInfo->IsFoundReligion())
	{
		return false;
	}

	// If prophet has started spreading religion, can't do other functions
	if(m_pUnitInfo->IsSpreadReligion())
	{
		if(GetReligionData()->GetSpreadsLeft() < m_pUnitInfo->GetReligionSpreads())
		{
			return false;
		}
	}

	if(pCity == NULL)
	{
		return false;
	}

	if(getTeam() != pCity->getTeam())
	{
		return false;
	}

	if(GET_TEAM(getTeam()).isMinorCiv())
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

	if(!pReligions->HasCreatedReligion(getOwner()))
	{
		return false;
	}

	ReligionTypes eReligion = pReligions->GetReligionCreatedByPlayer(getOwner());
	const CvReligion* pReligion = pReligions->GetReligion(eReligion, getOwner());
	if(pReligion->m_bEnhanced)
	{
		return false;
	}

	if(getX() != pReligion->m_iHolyCityX || getY() != pReligion->m_iHolyCityY)
	{
		return false;
	}

#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
	if (pReligions->GetAvailableEnhancerBeliefs(getOwner(), eReligion).size() < 1 || pReligions->GetAvailableFollowerBeliefs(getOwner(), eReligion).size() < 1)
#else
	if (pReligions->GetAvailableEnhancerBeliefs().size() < 1 || pReligions->GetAvailableFollowerBeliefs().size() < 1)
#endif
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::DoEnhanceReligion()
{
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();

	CvPlot* pkPlot = plot();
	CvCity* pkCity = (NULL != pkPlot)? pkPlot->getPlotCity() : NULL;

	if(pkPlot != NULL && pkCity != NULL)
	{
		if(CanEnhanceReligion(pkPlot))
		{
			CvPlayerAI& kOwner = GET_PLAYER(getOwner());
			if(kOwner.isHuman())
			{
				CvAssertMsg(pkCity != NULL, "No City??");

				CvNotifications* pNotifications = kOwner.GetNotifications();
				if(pNotifications)
				{
					CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ENHANCE_RELIGION");
					CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ENHANCE_RELIGION");
					pNotifications->Add(NOTIFICATION_ENHANCE_RELIGION, strBuffer, strSummary, pkPlot->getX(), pkPlot->getY(), -1, pkCity->GetID());
				}
#if defined(MOD_EVENTS_GREAT_PEOPLE)
				kOwner.DoGreatPersonExpended(getUnitType(), this);
#else
				kOwner.DoGreatPersonExpended(getUnitType());
#endif
				kill(true);
			}
			else
			{
				ReligionTypes eReligion = pReligions->GetReligionCreatedByPlayer(getOwner());
				if(eReligion != NO_RELIGION)
				{
					// Pick a belief for this religion
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
					BeliefTypes eBelief1 = kOwner.GetReligionAI()->ChooseFollowerBelief(kOwner.GetID(), eReligion);  // temporary
					BeliefTypes eBelief2 = kOwner.GetReligionAI()->ChooseEnhancerBelief(kOwner.GetID(), eReligion);  // temporary
#else
					BeliefTypes eBelief1 = kOwner.GetReligionAI()->ChooseFollowerBelief();  // temporary
					BeliefTypes eBelief2 = kOwner.GetReligionAI()->ChooseEnhancerBelief();  // temporary
#endif

					pReligions->EnhanceReligion(getOwner(), eReligion, eBelief1, eBelief2);

#if defined(MOD_EVENTS_GREAT_PEOPLE)
					kOwner.DoGreatPersonExpended(getUnitType(), this);
#else
					kOwner.DoGreatPersonExpended(getUnitType());
#endif
					kill(true);
				}
				else
				{
					CvAssertMsg(false, "No religions available to found.");
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnit::CanSpreadReligion(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	CvCity* pCity;

	if(!m_pUnitInfo->IsSpreadReligion())
	{
		return false;
	}

	if(GetReligionData()->GetReligion() == NO_RELIGION)
	{
		return false;
	}

	pCity = pPlot->getPlotCity();
	if(pCity == NULL)
	{
		pCity = pPlot->GetAdjacentCity();
		if(pCity == NULL)
		{
			return false;
		}
#if defined(MOD_BALANCE_CORE)
		else if(GET_PLAYER(pCity->getOwner()).GetPlayerTraits()->IsNoSpread() && (getOwner() != pCity->getOwner()))
		{
			if(GetReligionData()->GetReligion() != GET_PLAYER(pCity->getOwner()).GetReligions()->GetReligionInMostCities())
			{
				return false;
			}
		}
		else if(GET_PLAYER(pCity->getOwner()).isMinorCiv())
		{
			PlayerTypes eAlly = (GET_PLAYER(pCity->getOwner()).GetMinorCivAI()->GetAlly());
			if(eAlly != NO_PLAYER)
			{
				if(GET_PLAYER(eAlly).GetPlayerTraits()->IsNoSpread() && (getOwner() != eAlly))
				{
					if(GetReligionData()->GetReligion() != GET_PLAYER(pCity->getOwner()).GetReligions()->GetReligionInMostCities())
					{
						return false;
					}
				}
			}
		}
#endif
	}

	// Blocked by Inquisitor?
	if(pCity->GetCityReligions()->IsDefendedAgainstSpread(GetReligionData()->GetReligion()))
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}
	
#if defined(MOD_EVENTS_RELIGION)
	if (MOD_EVENTS_RELIGION) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanSpreadReligion, getOwner(), GetID(), plot()->getX(), plot()->getY()) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
#endif

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::DoSpreadReligion()
{
#if defined(MOD_BALANCE_CORE_BELIEFS)
	int iGoldenAgeBonus = 0;
	int iTourism = 0;
	int iCSInfluence = 0;
	int iEra = GET_PLAYER(getOwner()).GetCurrentEra();
	if(iEra < 1)
	{
		iEra = 1;
	}
#endif
#if !defined(MOD_API_UNIFIED_YIELDS)
	int iScienceBonus = 0;
#endif

	CvCity* pCity = GetSpreadReligionTargetCity();

	if (pCity != NULL)
	{
		if(CanSpreadReligion(plot()))
		{
#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
			int iConversionStrength = GetConversionStrength(pCity);
#else
			int iConversionStrength = GetConversionStrength();
#endif
			CvGameReligions* pReligions = GC.getGame().GetGameReligions();
			ReligionTypes eReligion = GetReligionData()->GetReligion();
			if(eReligion > RELIGION_PANTHEON)
			{
				const CvReligion* pReligion = pReligions->GetReligion(eReligion, getOwner());
				if(pReligion)
				{
#if defined(MOD_API_UNIFIED_YIELDS)
					// Requires majority for this city to be another religion
					ReligionTypes eCurrentReligion = pCity->GetCityReligions()->GetReligiousMajority();
					int iOtherFollowers = pCity->GetCityReligions()->GetFollowersOtherReligions(eReligion);
					if (eCurrentReligion != NO_RELIGION && eCurrentReligion != eReligion && iOtherFollowers > 0)
					{
						CvPlayer &kPlayer = GET_PLAYER(m_eOwner);
#if !defined(SHOW_PLOT_POPUP)
						int iDelay = 0;
#endif
						bool bFloatText = pCity->plot() && pCity->plot()->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF;
						for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
						{
							YieldTypes eYield = (YieldTypes) iI;
							
							if (eYield == YIELD_FOOD || eYield == YIELD_PRODUCTION)
							{
								continue;
							}
							
							int iYieldBonus = pReligion->m_Beliefs.GetYieldPerOtherReligionFollower(eYield);

							if (eYield == YIELD_SCIENCE)
							{
								iYieldBonus += pReligion->m_Beliefs.GetSciencePerOtherReligionFollower();
							}

							if (iYieldBonus > 0)
							{
								iYieldBonus *= iOtherFollowers;
									
								switch(eYield)
								{
								case YIELD_GOLD:
									kPlayer.GetTreasury()->ChangeGold(iYieldBonus);
									break;
								case YIELD_CULTURE:
									kPlayer.changeJONSCulture(iYieldBonus);
									break;
								case YIELD_FAITH:
									kPlayer.ChangeFaith(iYieldBonus);
									break;
								case YIELD_GOLDEN_AGE_POINTS:
									kPlayer.ChangeGoldenAgeProgressMeter(iYieldBonus);
									break;
								case YIELD_SCIENCE:
									{
									TechTypes eCurrentTech = kPlayer.GetPlayerTechs()->GetCurrentResearch();
									if(eCurrentTech == NO_TECH)
									{
										kPlayer.changeOverflowResearch(iYieldBonus);
									}
									else
									{
										GET_TEAM(kPlayer.getTeam()).GetTeamTechs()->ChangeResearchProgress(eCurrentTech, iYieldBonus, kPlayer.GetID());
									}
									}
									break;
								case YIELD_TOURISM:
									kPlayer.GetCulture()->AddTourismAllKnownCivs(iYieldBonus);
									break;
								}

								if (bFloatText)
								{
									char text[256] = {0};
									sprintf_s(text, "%s+%d[ENDCOLOR]%s", GC.getYieldInfo(eYield)->getColorString(), iYieldBonus, GC.getYieldInfo(eYield)->getIconString());
#if defined(SHOW_PLOT_POPUP)
									SHOW_PLOT_POPUP(pCity->plot(), getOwner(), text, 0.0f);
#else
									float fDelay = GC.getPOST_COMBAT_TEXT_DELAY() * (2 + ((float)(iDelay++) * 0.5f));
									DLLUI->AddPopupText(pCity->getX(), pCity->getY(), text, fDelay);
#endif
								}
							}
						}
					}
#else
					iScienceBonus = pReligion->m_Beliefs.GetSciencePerOtherReligionFollower();
					if(iScienceBonus > 0)
					{
						// Requires majority for this city to be another religion
						ReligionTypes eCurrentReligion = pCity->GetCityReligions()->GetReligiousMajority();
						if (eCurrentReligion != NO_RELIGION && eCurrentReligion != eReligion)
						{
							iScienceBonus *= pCity->GetCityReligions()->GetFollowersOtherReligions(eReligion);
						}
						else
						{
							iScienceBonus = 0;
						}
					}
#endif
#if defined(MOD_BALANCE_CORE_BELIEFS)
#if defined(MOD_API_UNIFIED_YIELDS_GOLDEN_AGE)
					iGoldenAgeBonus = pReligion->m_Beliefs.GetYieldFromSpread(YIELD_GOLDEN_AGE_POINTS) * iEra;
#endif
					iCSInfluence = (pReligion->m_Beliefs.GetMissionaryInfluenceCS() * iEra);
#if defined(MOD_API_UNIFIED_YIELDS_TOURISM)
					iTourism = pReligion->m_Beliefs.GetYieldFromForeignSpread(YIELD_TOURISM) * iEra;
					iTourism *= GC.getGame().getGameSpeedInfo().getTrainPercent();
					iTourism /= 100;
#endif
#endif

				}
			}

			if(IsGreatPerson())
			{
				pCity->GetCityReligions()->AddProphetSpread(eReligion, iConversionStrength, getOwner());
			}
			else
			{
				pCity->GetCityReligions()->AddReligiousPressure(FOLLOWER_CHANGE_MISSIONARY, eReligion, iConversionStrength, getOwner());
			}
			GetReligionData()->SetSpreadsLeft(GetReligionData()->GetSpreadsLeft() - 1);

			if (pCity->plot() && pCity->plot()->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF)
			{
				const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(GetReligionData()->GetReligion(), m_eOwner);
				Localization::String strReligionName = Localization::Lookup(pReligion->GetName());

				char text[256] = {0};
				sprintf_s(text, "[COLOR_WHITE]%s: %d [ICON_PEACE][ENDCOLOR]",
					strReligionName.toUTF8(),
					iConversionStrength / GC.getRELIGION_MISSIONARY_PRESSURE_MULTIPLIER());
#if defined(SHOW_PLOT_POPUP)
				SHOW_PLOT_POPUP(pCity->plot(), getOwner(), text, 0.0f);
#else
				float fDelay = 0.0f;
				DLLUI->AddPopupText(pCity->getX(), pCity->getY(), text, fDelay);
#endif
			}

#if !defined(MOD_API_UNIFIED_YIELDS)
			if (iScienceBonus > 0)
			{
				CvPlayer &kPlayer = GET_PLAYER(m_eOwner);

				TechTypes eCurrentTech = kPlayer.GetPlayerTechs()->GetCurrentResearch();
				if(eCurrentTech == NO_TECH)
				{
					kPlayer.changeOverflowResearch(iScienceBonus);
				}
				else
				{
					CvTeam &kTeam = GET_TEAM(kPlayer.getTeam());
					kTeam.GetTeamTechs()->ChangeResearchProgress(eCurrentTech, iScienceBonus, kPlayer.GetID());
				}

				if (pCity->plot() && pCity->plot()->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF)
				{
					char text[256] = {0};
					sprintf_s(text, "[COLOR_BLUE]+%d[ENDCOLOR][ICON_RESEARCH]", iScienceBonus);
					float fDelay = GC.getPOST_COMBAT_TEXT_DELAY() * 2;
#if defined(SHOW_PLOT_POPUP)
					SHOW_PLOT_POPUP(pCity->plot(), getOwner(), text, fDelay);
#else
					DLLUI->AddPopupText(pCity->getX(), pCity->getY(), text, fDelay);
#endif
				}
			}
#endif
#if defined(MOD_BALANCE_CORE_BELIEFS)
#if defined(MOD_API_UNIFIED_YIELDS_GOLDEN_AGE)
			float fDelay = GC.getPOST_COMBAT_TEXT_DELAY() * 2;
			if(iGoldenAgeBonus > 0)
			{
				CvPlayer &kPlayer = GET_PLAYER(m_eOwner);
				kPlayer.ChangeGoldenAgeProgressMeter(iGoldenAgeBonus);
				if (pCity->plot() && pCity->plot()->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF)
				{
					char text[256] = {0};
					sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_GOLDEN_AGE]", iGoldenAgeBonus);
					fDelay += 0.5f;
					DLLUI->AddPopupText(pCity->getX(), pCity->getY(), text, fDelay);
				}
			}
#endif
			if(iCSInfluence > 0)
			{
				CvPlayer &kPlayer = GET_PLAYER(m_eOwner);
				if(GET_PLAYER(pCity->getOwner()).isMinorCiv())
				{
					GET_PLAYER(pCity->getOwner()).GetMinorCivAI()->ChangeFriendshipWithMajor(kPlayer.GetID(), iCSInfluence, false);
					if (pCity->plot() && pCity->plot()->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF)
					{
						char text[256] = {0};
						sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_INFLUENCE]", iCSInfluence);
						fDelay += 0.5f;
						DLLUI->AddPopupText(pCity->getX(), pCity->getY(), text, fDelay);
					}
				}
			}
#if defined(MOD_API_UNIFIED_YIELDS_TOURISM)
			if(iTourism > 0)
			{
				if((pCity->getOwner() != m_eOwner) && !GET_PLAYER(pCity->getOwner()).isMinorCiv())
				{
					CvPlayer &kPlayer = GET_PLAYER(m_eOwner);
					kPlayer.GetCulture()->ChangeInfluenceOn(pCity->getOwner(), iTourism);
					// Show tourism spread
					if (pCity->plot() && pCity->plot()->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF)
					{
						CvString strInfluenceText;
						InfluenceLevelTypes eLevel = kPlayer.GetCulture()->GetInfluenceLevel(pCity->getOwner());

						if (eLevel == INFLUENCE_LEVEL_UNKNOWN)
							strInfluenceText = GetLocalizedText( "TXT_KEY_CO_UNKNOWN" );
						else if (eLevel == INFLUENCE_LEVEL_EXOTIC)
							strInfluenceText = GetLocalizedText( "TXT_KEY_CO_EXOTIC");
						else if (eLevel == INFLUENCE_LEVEL_FAMILIAR)
							strInfluenceText = GetLocalizedText( "TXT_KEY_CO_FAMILIAR");
						else if (eLevel == INFLUENCE_LEVEL_POPULAR)
							strInfluenceText = GetLocalizedText( "TXT_KEY_CO_POPULAR");
						else if (eLevel == INFLUENCE_LEVEL_INFLUENTIAL)
							strInfluenceText = GetLocalizedText( "TXT_KEY_CO_INFLUENTIAL");
						else if (eLevel == INFLUENCE_LEVEL_DOMINANT)
							strInfluenceText = GetLocalizedText( "TXT_KEY_CO_DOMINANT");

						char text[256] = {0};
						sprintf_s(text, "[COLOR_WHITE]+%d [ICON_TOURISM][ENDCOLOR]   %s", iTourism, strInfluenceText.c_str());
						fDelay += 0.5f;
						DLLUI->AddPopupText(pCity->getX(), pCity->getY(), text, fDelay);
					}
				}
			}
#endif
#endif

			bool bShow = plot()->isActiveVisible(false);
			if(bShow)
			{
				auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
				gDLL->GameplayUnitActivate(pDllUnit.get());
			}

			if(GetReligionData()->GetSpreadsLeft() <= 0)
			{
				if(IsGreatPerson())
				{
					CvPlayer& kPlayer = GET_PLAYER(getOwner());
#if defined(MOD_EVENTS_GREAT_PEOPLE)
					kPlayer.DoGreatPersonExpended(getUnitType(), this);
#else
					kPlayer.DoGreatPersonExpended(getUnitType());
#endif
				}

				kill(true);
			}
			else
			{
				if(bShow)
				{
					// Because the "Activate" animation will possibly put the animation state into a end-state, we will force a reset, since the unit will still be alive
					auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
					gDLL->GameplayUnitResetAnimationState(pDllUnit.get());
				}

				finishMoves();
			}

#if !defined(NO_ACHIEVEMENTS)
			//Achievements
			const PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
			if(getOwner() == eActivePlayer && pCity->getOwner() != eActivePlayer)
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_XP1_17);
			}
#endif
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnit::CanRemoveHeresy(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	CvCity* pCity;

	if(!m_pUnitInfo->IsRemoveHeresy())
	{
		return false;
	}

	if(GetReligionData()->GetReligion() == NO_RELIGION)
	{
		return false;
	}

	pCity = pPlot->getPlotCity();
	if(pCity == NULL)
	{
		pCity = pPlot->GetAdjacentCity();
		if(pCity == NULL)
		{
			return false;
		}
		else if(pCity->getOwner() != getOwner())
		{
			return false;
		}
	}

	if(!pCity->GetCityReligions()->IsReligionHereOtherThan(GetReligionData()->GetReligion()))
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

#if defined(MOD_EVENTS_RELIGION)
	if (MOD_EVENTS_RELIGION) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanRemoveHeresy, getOwner(), GetID(), plot()->getX(), plot()->getY()) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
#endif

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::DoRemoveHeresy()
{
	CvCity* pCity = GetSpreadReligionTargetCity();

	if (pCity != NULL)
	{
		if(CanRemoveHeresy(plot()))
		{
			bool bShow = plot()->isActiveVisible(false);
			if(bShow)
			{
				auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
				gDLL->GameplayUnitActivate(pDllUnit.get());
			}

			pCity->GetCityReligions()->RemoveOtherReligions(GetReligionData()->GetReligion(), getOwner());
#if !defined(NO_ACHIEVEMENTS)
			//Achievements
			if(getOwner() == GC.getGame().getActivePlayer())
			{
				CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
				if(strcmp(kPlayer.getCivilizationTypeKey(), "CIVILIZATION_SPAIN") == 0)
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_XP1_24);
				}
			}
#endif
			
			kill(true);
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetNumFollowersAfterSpread() const
{
	int iRtnValue = 0;
	CvCity* pCity = GetSpreadReligionTargetCity();

	if (pCity != NULL)
	{
		if(IsGreatPerson())
		{
#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
			iRtnValue = pCity->GetCityReligions()->GetNumFollowersAfterProphetSpread (GetReligionData()->GetReligion(), GetConversionStrength(pCity));
#else
			iRtnValue = pCity->GetCityReligions()->GetNumFollowersAfterProphetSpread (GetReligionData()->GetReligion(), GetConversionStrength());
#endif
		}
		else
		{
#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
			iRtnValue = pCity->GetCityReligions()->GetNumFollowersAfterSpread (GetReligionData()->GetReligion(), GetConversionStrength(pCity));
#else
			iRtnValue = pCity->GetCityReligions()->GetNumFollowersAfterSpread (GetReligionData()->GetReligion(), GetConversionStrength());
#endif
		}
	}

	return iRtnValue;
}

//	--------------------------------------------------------------------------------
ReligionTypes CvUnit::GetMajorityReligionAfterSpread() const
{
	ReligionTypes eRtnValue = NO_RELIGION;
	CvCity* pCity = GetSpreadReligionTargetCity();

	if (pCity != NULL)
	{
		if(IsGreatPerson())
		{
#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
			eRtnValue = pCity->GetCityReligions()->GetMajorityReligionAfterProphetSpread (GetReligionData()->GetReligion(), GetConversionStrength(pCity));
#else
			eRtnValue = pCity->GetCityReligions()->GetMajorityReligionAfterProphetSpread (GetReligionData()->GetReligion(), GetConversionStrength());
#endif
		}
		else
		{
#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
			eRtnValue = pCity->GetCityReligions()->GetMajorityReligionAfterSpread (GetReligionData()->GetReligion(), GetConversionStrength(pCity));
#else
			eRtnValue = pCity->GetCityReligions()->GetMajorityReligionAfterSpread (GetReligionData()->GetReligion(), GetConversionStrength());
#endif
		}
	}

	return eRtnValue;
}

//	--------------------------------------------------------------------------------
CvCity *CvUnit::GetSpreadReligionTargetCity() const
{
	CvCity* pCity = NULL;
	CvPlot* pPlot = plot();

	if(pPlot)
	{
		pCity = pPlot->getPlotCity();
		if(pCity == NULL)
		{
			pCity = pPlot->GetAdjacentCity();
			if(pCity == NULL)
			{
				return NULL;
			}
		}
	}
	return pCity;
}

//	--------------------------------------------------------------------------------
#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
int CvUnit::GetConversionStrength(const CvCity* pCity) const
#else
int CvUnit::GetConversionStrength() const
#endif
{
	int iReligiousStrength = GC.getRELIGION_MISSIONARY_PRESSURE_MULTIPLIER() * GetReligionData()->GetReligiousStrength();
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	ReligionTypes eReligion = GetReligionData()->GetReligion();
	if(eReligion > RELIGION_PANTHEON)
	{
		const CvReligion* pReligion = pReligions->GetReligion(eReligion, getOwner());
		if(pReligion)
		{
			int iStrengthMod;
			if (IsGreatPerson())
			{
				iStrengthMod = pReligion->m_Beliefs.GetProphetStrengthModifier();
			}
			else
			{
				iStrengthMod = pReligion->m_Beliefs.GetMissionaryStrengthModifier();
			}

			if(iStrengthMod != 0)
			{
				iReligiousStrength *= (100 + iStrengthMod);
				iReligiousStrength /= 100;
			}
		}
	}

#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
	if (pCity != NULL && MOD_RELIGION_CONVERSION_MODIFIERS) {
		// Modify iReligiousStrength based on city defenses, but only against hostile units (ie any not the same team as the city)
		PlayerTypes eFromPlayer = getOwner();
		PlayerTypes eToPlayer = pCity->getOwner();

		if (getTeam() != pCity->getTeam()) {
			CvPlayer& pToPlayer = GET_PLAYER(eToPlayer);
			int iCityModifier = pCity->GetConversionModifier();
			
			if (pToPlayer.isMinorCiv() && pToPlayer.GetMinorCivAI()->IsActiveQuestForPlayer(eFromPlayer, MINOR_CIV_QUEST_SPREAD_RELIGION)) {
				// The City State actively wants this religion
				iCityModifier *= -1;
			}

			iReligiousStrength *= (100 + (iCityModifier + pToPlayer.GetConversionModifier() + pToPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CONVERSION_MODIFIER))); 
			iReligiousStrength /= 100;
		}
	}
	// CUSTOMLOG("Unit conversion str: %i", iReligiousStrength);

#endif
	return iReligiousStrength;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canDiscover(const CvPlot* /*pPlot*/, bool bTestVisible) const
{
	VALIDATE_OBJECT
	if(getUnitInfo().GetNumFreeTechs() == 0 && getUnitInfo().GetBaseBeakersTurnsToCount() == 0)
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

	if(!bTestVisible)
	{
		if(GET_PLAYER(getOwner()).getNumCities() == 0)
		{
			return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
int CvUnit::getDiscoverAmount()
{
	int iValue = 0;
#if !defined(MOD_BALANCE_CORE)
	CvPlot* pPlot = plot();
	if(canDiscover(pPlot))
#endif
	{
		CvPlayer* pPlayer = &GET_PLAYER(getOwner());
		CvAssertMsg(pPlayer, "Owner of unit not expected to be NULL. Please send Anton your save file and version.");
		if (pPlayer)
		{
			// Beakers boost based on previous turns
			int iPreviousTurnsToCount = m_pUnitInfo->GetBaseBeakersTurnsToCount();
			iValue = pPlayer->GetScienceYieldFromPreviousTurns(GC.getGame().getGameTurn(), iPreviousTurnsToCount);
			if (pPlayer->GetGreatScientistBeakerMod() != 0)
			{
				iValue += (iValue * pPlayer->GetGreatScientistBeakerMod()) / 100;
				iValue = MAX(iValue, 0); // Cannot be negative
			}

			// Modify based on game speed
			iValue *= GC.getGame().getGameSpeedInfo().getResearchPercent();
			iValue /= 100;
		}
	}
	return iValue;
}
#if defined(MOD_BALANCE_CORE)
bool CvUnit::greatperson()
{
	VALIDATE_OBJECT
	CvPlot* pPlot = plot();
	CvPlayer* pPlayer = &GET_PLAYER(getOwner());
	CvAssertMsg(pPlayer, "Owner of unit not expected to be NULL. Please send Anton your save file and version.");
	if (!pPlayer) return false;
	CvTeam* pTeam = &GET_TEAM(pPlayer->getTeam());
	CvAssertMsg(pTeam, "Owner team of unit not expected to be NULL. Please send Anton your save file and version.");
	if (!pTeam) return false;

	if(pPlot->isActiveVisible(false))
	{
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
	}

	if(IsGreatPerson())
	{
#if defined(MOD_EVENTS_GREAT_PEOPLE)
		pPlayer->DoGreatPersonExpended(getUnitType(), this);
#else
		pPlayer->DoGreatPersonExpended(getUnitType());
#endif
	}

	kill(true);

	return true;
}
#endif
//	--------------------------------------------------------------------------------
bool CvUnit::discover()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();

	if(!canDiscover(pPlot))
	{
		return false;
	}

	CvPlayer* pPlayer = &GET_PLAYER(getOwner());
	CvAssertMsg(pPlayer, "Owner of unit not expected to be NULL. Please send Anton your save file and version.");
	if (!pPlayer) return false;
	CvTeam* pTeam = &GET_TEAM(pPlayer->getTeam());
	CvAssertMsg(pTeam, "Owner team of unit not expected to be NULL. Please send Anton your save file and version.");
	if (!pTeam) return false;

	// Beakers boost based on previous turns
#if defined(MOD_BALANCE_CORE)
	int iBeakersBonus = GetScienceBlastStrength();
#else
	int iBeakersBonus = getDiscoverAmount();
#endif
	TechTypes eCurrentTech = pPlayer->GetPlayerTechs()->GetCurrentResearch();
	if(eCurrentTech == NO_TECH)
	{
		pPlayer->changeOverflowResearch(iBeakersBonus);
	}
	else
	{
		pTeam->GetTeamTechs()->ChangeResearchProgress(eCurrentTech, iBeakersBonus, getOwner());
	}

	// Free techs
	int iNumFreeTechs = m_pUnitInfo->GetNumFreeTechs();
	if(!isHuman())
	{
		for(int iI = 0; iI < iNumFreeTechs; iI++)
		{
			pPlayer->AI_chooseFreeTech();
		}
	}
	else
	{
		CvString strBuffer = GetLocalizedText("TXT_KEY_MISC_GREAT_PERSON_CHOOSE_TECH");
		pPlayer->chooseTech(iNumFreeTechs, strBuffer.GetCString());
	}

	if(pPlot->isActiveVisible(false))
	{
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
	}

	if(IsGreatPerson())
	{
#if defined(MOD_EVENTS_GREAT_PEOPLE)
		pPlayer->DoGreatPersonExpended(getUnitType(), this);
#else
		pPlayer->DoGreatPersonExpended(getUnitType());
#endif
	}

	kill(true);

	return true;
}

//	--------------------------------------------------------------------------------
/// Can this Unit rush a Building in pCity?
bool CvUnit::IsCanRushBuilding(CvCity* pCity, bool bTestVisible) const
{
	VALIDATE_OBJECT
	if(pCity == NULL)
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

	if(!m_pUnitInfo->IsRushBuilding())
	{
		return false;
	}

	if(!bTestVisible)
	{
		if(pCity->getProductionTurnsLeft() == 1)
		{
			return false;
		}

		if(!pCity->isProductionBuilding())
		{
			return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
/// Tells this Unit to rush a Building here (in a City)
bool CvUnit::DoRushBuilding()
{
	VALIDATE_OBJECT
	CvCity* pCity = plot()->getPlotCity();

	if(pCity == NULL)
	{
		return false;
	}

	if(!IsCanRushBuilding(pCity, false))
	{
		return false;
	}

	pCity->setProduction(pCity->getProductionNeeded());

	if(plot()->isActiveVisible(false))
	{
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
	}

	kill(true);

	return true;
}


//	--------------------------------------------------------------------------------
int CvUnit::getMaxHurryProduction(CvCity* pCity) const
{
	int iProduction;

	iProduction = (m_pUnitInfo->GetBaseHurry() + (m_pUnitInfo->GetHurryMultiplier() * pCity->getPopulation()));

	iProduction *= GC.getGame().getGameSpeedInfo().getUnitHurryPercent();
	iProduction /= 100;

	return std::max(0, iProduction);
}


//	--------------------------------------------------------------------------------
int CvUnit::getHurryProduction(const CvPlot* pPlot) const
{
	CvCity* pCity;
	int iProduction;

	pCity = pPlot->getPlotCity();

	if(pCity == NULL)
	{
		return 0;
	}

	iProduction = getMaxHurryProduction(pCity);

	iProduction = std::min(pCity->productionLeft(), iProduction);

	return std::max(0, iProduction);
}


//	--------------------------------------------------------------------------------
bool CvUnit::canHurry(const CvPlot* pPlot, bool bTestVisible) const
{
	if(isDelayedDeath())
	{
		return false;
	}

	CvCity* pCity;

	if(getHurryProduction(pPlot) == 0)
	{
		return false;
	}

	pCity = pPlot->getPlotCity();

	if(pCity == NULL)
	{
		return false;
	}

	if(pCity->getProductionTurnsLeft() == 1)
	{
		return false;
	}

	if(!bTestVisible)
	{
		if(!(pCity->isProductionBuilding()))
		{
			if (!pCity->isProductionUnit())
			{
				return false;
			}
			else
			{
				UnitTypes eUnit = pCity->getProductionUnit();
				CvUnitEntry *pkUnit = GC.GetGameUnits()->GetEntry(eUnit);
				if (pkUnit && GET_PLAYER(pCity->getOwner()).IsEnablesSSPartHurry())
				{
					if (pkUnit->GetSpaceshipProject() != NO_PROJECT)
					{
						return true;
					}
				}
				return false;
			}
		}
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::hurry()
{
	CvCity* pCity;

	CvPlot* pPlot = plot();

	if(!canHurry(pPlot))
	{
		return false;
	}

	pCity = pPlot->getPlotCity();

	if(pCity != NULL)
	{
		pCity->changeProduction(getHurryProduction(pPlot));
#if defined(MOD_BALANCE_CORE_ENGINEER_HURRY)
		if(MOD_BALANCE_CORE_ENGINEER_HURRY && pCity != NULL && pCity->getProductionBuilding() != NO_BUILDING)
		{
			if (pCity->getProduction() >= pCity->getProductionNeeded())
			{
				pCity->popOrder(0, true, true);
			}
		}
#endif
	}

	if(pPlot->isActiveVisible(false))
	{
#if !defined(NO_ACHIEVEMENTS)
		//Achievement check
		if(pCity != NULL && pCity->getProductionBuilding() != NO_BUILDING)
		{
			CvBuildingEntry* pkProductionBuildinInfo = GC.getBuildingInfo(pCity->getProductionBuilding());
			if(pkProductionBuildinInfo)
			{
				if(isLimitedWonderClass(pkProductionBuildinInfo->GetBuildingClassInfo()))
				{
					if(pCity->isHuman() && !GC.getGame().isGameMultiPlayer())
					{
						const char* pLeaderChar = GET_PLAYER(pCity->getOwner()).getLeaderTypeKey();
						CvString szLeader = pLeaderChar;
						if(szLeader == "LEADER_RAMESSES")
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_SPHINX);
						}
					}
				}
			}
		}
#endif
	}

	if(IsGreatPerson())
	{
		CvPlayer& kPlayer = GET_PLAYER(getOwner());
#if defined(MOD_EVENTS_GREAT_PEOPLE)
		kPlayer.DoGreatPersonExpended(getUnitType(), this);
#else
		kPlayer.DoGreatPersonExpended(getUnitType());
#endif
	}

	kill(true);

	return true;
}


//	--------------------------------------------------------------------------------
int CvUnit::getTradeGold(const CvPlot* /*pPlot*/) const
{
	VALIDATE_OBJECT
	int iGold;

	// Seed the gold Value with some cash
	iGold = m_pUnitInfo->GetBaseGold();

	// Amount of Gold also increases with how far into the game we are
	iGold += (m_pUnitInfo->GetNumGoldPerEra() * GET_TEAM(getTeam()).GetCurrentEra());

	iGold *= GC.getGame().getGameSpeedInfo().getUnitTradePercent();
	iGold /= 100;

	iGold *= (100 + GET_PLAYER(getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_MISSION_GOLD_MODIFIER));
	iGold /= 100;

	iGold *= (100 + GetTradeMissionGoldModifier());
	iGold /= 100;

	return std::max(0, iGold);
}

//	--------------------------------------------------------------------------------
int CvUnit::getTradeInfluence(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	int iInf = 0;
	if (pPlot && canTrade(pPlot))
	{
		PlayerTypes eMinor = pPlot->getOwner();
		CvAssertMsg(eMinor != NO_PLAYER, "Performing a trade mission and not in city state territory. This is bad. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
		if (eMinor != NO_PLAYER)
		{
			iInf = /*30*/ GC.getMINOR_FRIENDSHIP_FROM_TRADE_MISSION();
#if defined(MOD_DIPLOMACY_CITYSTATES)
			if (MOD_DIPLOMACY_CITYSTATES) {
				iInf += (m_pUnitInfo->GetNumInfPerEra() * GET_TEAM(getTeam()).GetCurrentEra());
			}
#endif
			int iInfTimes100 = iInf * (100 + GetTradeMissionInfluenceModifier());
			iInf = iInfTimes100 / 100;
		}
	}
	return iInf;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canTrade(const CvPlot* pPlot, bool bTestVisible) const
{
	VALIDATE_OBJECT
	if(isDelayedDeath())
	{
		return false;
	}

	if(m_pUnitInfo->GetBaseGold() == 0)
	{
		return false;
	}

	// Things that block usage but not visibility
	if(!bTestVisible)
	{
		// Not owned
		if(pPlot->getOwner() == NO_PLAYER)
			return false;

		// Owned by a non-minor civ
		if(!GET_PLAYER(pPlot->getOwner()).isMinorCiv())
			return false;

		if(GET_TEAM(pPlot->getTeam()).isAtWar(getTeam()))
		{
			return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::trade()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();

	if(!canTrade(pPlot))
		return false;

	int iTradeGold = getTradeGold(pPlot);
	
	GET_PLAYER(getOwner()).GetTreasury()->ChangeGold(iTradeGold);

	// Improve relations with the Minor
	PlayerTypes eMinor = pPlot->getOwner();
	CvAssertMsg(eMinor != NO_PLAYER, "Performing a trade mission and not in city state territory. This is bad. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	int iFriendship = getTradeInfluence(pPlot);

#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	if (MOD_DIPLOMACY_CITYSTATES_QUESTS) {
		//Added Influence Quest Bonus
		if(eMinor != NO_PLAYER && GET_PLAYER(eMinor).GetMinorCivAI()->IsActiveQuestForPlayer(getOwner(), MINOR_CIV_QUEST_INFLUENCE))
		{	
			int iBoostPercentage = GC.getINFLUENCE_MINOR_QUEST_BOOST();
			iFriendship *= 100 + iBoostPercentage;
			iFriendship /= 100;
		}
	}
	if(MOD_DIPLOMACY_CITYSTATES)
	{
		if (eMinor != NO_PLAYER && (m_pUnitInfo->GetNumInfPerEra() > 0))
		{
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
			{
				PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
				if(eLoopPlayer != NO_PLAYER && (GET_PLAYER(eLoopPlayer).getTeam() != getTeam()) && !GET_PLAYER(eLoopPlayer).isMinorCiv() && eLoopPlayer != getOwner() && GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).isHasMet(GET_PLAYER(eMinor).getTeam()))
				{
					GET_PLAYER(eMinor).GetMinorCivAI()->ChangeFriendshipWithMajor(eLoopPlayer, -iFriendship);
					CvNotifications* pNotifications = GET_PLAYER(eLoopPlayer).GetNotifications();
					if(pNotifications)
					{
						Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_DIPLOMAT_OTHER");
						strText <<  getNameKey() << iFriendship << GET_PLAYER(eMinor).getNameKey() << GET_PLAYER(getOwner()).getCivilizationAdjectiveKey();
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_DIPLOMAT_OTHER_SUMMARY");
						strSummary << getNameKey() << GET_PLAYER(eMinor).getNameKey();
						pNotifications->Add(NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER, strText.toUTF8(), strSummary.toUTF8(), getX(), getY(), getUnitType());
					}
				}
			}
			CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
			if(pNotifications)
			{
				Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_DIPLOMAT");
				strText <<  getNameKey() << iFriendship << GET_PLAYER(eMinor).getNameKey();
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_DIPLOMAT_SUMMARY");
				strSummary << getNameKey() << GET_PLAYER(eMinor).getNameKey();
				pNotifications->Add(NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER, strText.toUTF8(), strSummary.toUTF8(), getX(), getY(), getUnitType());
			}
		}
	}
#endif

#if defined(MOD_BALANCE_CORE_NEW_GP_ATTRIBUTES)
	if(MOD_BALANCE_CORE_NEW_GP_ATTRIBUTES)
	{
		if(m_pUnitInfo->GetNumGoldPerEra() > 0)
		{
			int iCap = 20;
			iCap *= GC.getGame().getGameSpeedInfo().getTrainPercent();
			iCap /= 100;
			CvCity* pLoopCity;
			int iCityLoop;

			// Loop through owner's cities.
			for(pLoopCity = GET_PLAYER(getOwner()).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwner()).nextCity(&iCityLoop))
			{
				if(pLoopCity != NULL)
				{
					pLoopCity->ChangeWeLoveTheKingDayCounter(iCap);
				}
			}
			if(GET_PLAYER(getOwner()).isHuman())
			{
				CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
				if(pNotifications)
				{
					Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_WLTKD_GREAT_MERCHANT");
					strText <<  getNameKey() << iCap;
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_WLTKD_GREAT_MERCHANT_SUMMARY");
					strSummary << getNameKey();
					pNotifications->Add(NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER, strText.toUTF8(), strSummary.toUTF8(), getX(), getY(), getUnitType());
				}
			}
		}
	}
	//This keeps minor civs in the black, financially.
	GET_PLAYER(eMinor).GetTreasury()->ChangeGold((iFriendship / 2));
#endif

	GET_PLAYER(eMinor).GetMinorCivAI()->ChangeFriendshipWithMajor(getOwner(), iFriendship);

	if(getOwner() == GC.getGame().getActivePlayer())
	{
		DLLUI->AddUnitMessage(0, GetIDInfo(), getOwner(), true, GC.getEVENT_MESSAGE_TIME(), GetLocalizedText("TXT_KEY_MERCHANT_RESULT", iTradeGold, iFriendship));
	}

	if(pPlot->isActiveVisible(false))
	{
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
	}

	if(IsGreatPerson())
	{
		CvPlayer& kPlayer = GET_PLAYER(getOwner());
#if defined(MOD_EVENTS_GREAT_PEOPLE)
		kPlayer.DoGreatPersonExpended(getUnitType(), this);
#else
		kPlayer.DoGreatPersonExpended(getUnitType());
#endif
	}

	kill(true);

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canBuyCityState(const CvPlot* pPlot, bool bTestVisible) const
{
	VALIDATE_OBJECT
	if(isDelayedDeath())
	{
		return false;
	}

	if (!m_pUnitInfo->IsCanBuyCityState())
	{
		return false;
	}

	if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && GET_PLAYER(getOwner()).isHuman())
	{
		return false;
	}

	// Things that block usage but not visibility
	if(!bTestVisible)
	{
		// Not owned
		if(pPlot->getOwner() == NO_PLAYER)
			return false;

		// if we don't have a starting city, we can't purchase a city-state.
		if (GET_PLAYER(m_eOwner).getNumCities() <= 0)
		{
			return false;
		}

		// Owned by a non-minor civ
		if(!GET_PLAYER(pPlot->getOwner()).isMinorCiv())
			return false;

		if(GET_TEAM(pPlot->getTeam()).isAtWar(getTeam()))
		{
			return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::buyCityState()
{
	VALIDATE_OBJECT
	CvPlot* pPlot = plot();

	if (!canBuyCityState(pPlot))
	{
		return false;
	}

	// Improve relations with the Minor
	PlayerTypes eMinor = pPlot->getOwner();
	CvAssertMsg(eMinor != NO_PLAYER, "Trying to buy a city state and not in city state territory. This is bad. Please send slewis this with your last 5 autosaves and what changelist # you're playing.");

	if (eMinor != NO_PLAYER)
	{
		CvCity* pMinorCapital = GET_PLAYER(eMinor).getCapitalCity();
		if (pMinorCapital)
		{
			pMinorCapital = NULL; // we shouldn't use this pointer because DoAcquire invalidates it
			int iNumUnits, iCapitalX, iCapitalY;
#if defined(MOD_GLOBAL_VENICE_KEEPS_RESOURCES)
			// CvUnit::buyCityState() is only ever called via CvTypes::getMISSION_BUY_CITY_STATE(), so this MUST be a "Merchant of Venice" type unit
			GET_PLAYER(eMinor).GetMinorCivAI()->DoAcquire(getOwner(), iNumUnits, iCapitalX, iCapitalY, true);
#else
			GET_PLAYER(eMinor).GetMinorCivAI()->DoAcquire(getOwner(), iNumUnits, iCapitalX, iCapitalY);
#endif
			pMinorCapital = GC.getMap().plot(iCapitalX, iCapitalY)->getPlotCity();
			if (pMinorCapital)
			{
				// reduce the resistence to 0 turns because we bought it fairly
				pMinorCapital->ChangeResistanceTurns(-pMinorCapital->GetResistanceTurns());
			}

		}
	}

	if (getOwner() == GC.getGame().getActivePlayer())
	{
		DLLUI->AddUnitMessage(0, GetIDInfo(), getOwner(), true, GC.getEVENT_MESSAGE_TIME(), GetLocalizedText("TXT_KEY_VENETIAN_MERCHANT_BOUGHT_CITY_STATE"));
	}

	if (pPlot->isActiveVisible(false))
	{
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
	}

	if (IsGreatPerson())
	{
		CvPlayer& kPlayer = GET_PLAYER(getOwner());
#if defined(MOD_EVENTS_GREAT_PEOPLE)
		kPlayer.DoGreatPersonExpended(getUnitType(), this);
#else
		kPlayer.DoGreatPersonExpended(getUnitType());
#endif
	}

	kill(true);

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canRepairFleet(const CvPlot* /*pPlot*/, bool /*bTestVisible*/) const
{
	VALIDATE_OBJECT
	if(isDelayedDeath())
	{
		return false;
	}

#if defined(MOD_GLOBAL_SEPARATE_GREAT_ADMIRAL)
	bool bHasSkill = !MOD_GLOBAL_SEPARATE_GREAT_ADMIRAL && IsGreatAdmiral();
	bHasSkill = bHasSkill || (MOD_GLOBAL_SEPARATE_GREAT_ADMIRAL && m_pUnitInfo->IsCanRepairFleet());
	if (!bHasSkill)
#else
	if (!IsGreatAdmiral())
#endif
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::repairFleet()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();

	if(!canRepairFleet(pPlot))
		return false;

	// Do the repairing - first this hex
	int iUnitLoop;
	for (iUnitLoop = 0; iUnitLoop < pPlot->getNumUnits(); iUnitLoop++)
	{
		CvUnit *pUnit = pPlot->getUnitByIndex(iUnitLoop);
		if (pUnit->getOwner() == getOwner() && (pUnit->isEmbarked() || pUnit->getDomainType() == DOMAIN_SEA))
		{
			pUnit->changeDamage(-pUnit->getDamage());
		}
	}

	// And then all adjacent
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot *pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			for (iUnitLoop = 0; iUnitLoop < pAdjacentPlot->getNumUnits(); iUnitLoop++)
			{	
				CvUnit *pUnit = pAdjacentPlot->getUnitByIndex(iUnitLoop);
				if (pUnit->getOwner() == getOwner() && (pUnit->isEmbarked() || pUnit->getDomainType() == DOMAIN_SEA))
				{
					pUnit->changeDamage(-pUnit->getDamage());
				}
			}
		}
	}

	if(pPlot->isActiveVisible(false))
	{
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
	}

	if(IsGreatPerson())
	{
		CvPlayer& kPlayer = GET_PLAYER(getOwner());
#if defined(MOD_EVENTS_GREAT_PEOPLE)
		kPlayer.DoGreatPersonExpended(getUnitType(), this);
#else
		kPlayer.DoGreatPersonExpended(getUnitType());
#endif
	}

	kill(true);

	return true;
}

//	--------------------------------------------------------------------------------
/// Can this Unit build a spaceship part at pPlot??
bool CvUnit::CanBuildSpaceship(const CvPlot* pPlot, bool bVisible) const
{
	VALIDATE_OBJECT

	// Delayed death
	if(isDelayedDeath())
		return false;

	if(GAMESTATE_ON != GC.getGame().getGameState())
		return false;

	// Can this unit actually build a part?
	ProjectTypes eSpaceshipProject = (ProjectTypes) getUnitInfo().GetSpaceshipProject();
	if(eSpaceshipProject == NO_PROJECT)
		return false;

	// Things that block usage but not visibility
	if(!bVisible)
	{
		CvCity* pCity = pPlot->getPlotCity();

		// Not in a city
		if(pCity == NULL)
			return false;

		// Must be our city
		if(pCity->getOwner() != getOwner())
			return false;

		// Can only build SS part in our capital
		if(!pCity->isCapital())
			return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::DoBuildSpaceship()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();

	if(!CanBuildSpaceship(pPlot, false))
		return false;

	CvCity* pCity = pPlot->getPlotCity();

	// If we're not in a city, bail out
	if(pCity == NULL)
		return false;

	// Can this unit actually build a part?
	ProjectTypes eSpaceshipProject = (ProjectTypes)m_pUnitInfo->GetSpaceshipProject();
	if(eSpaceshipProject == NO_PROJECT)
		return false;

	pCity->CreateProject(eSpaceshipProject);

	kill(true);

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::CanCultureBomb(const CvPlot* pPlot, bool bTestVisible) const
{
	VALIDATE_OBJECT

	if(m_pUnitInfo->GetCultureBombRadius() <= 0)
		return false;

	if(isDelayedDeath())
		return false;

	// Things we test for if we're going to perform this action RIGHT NOW
	if(!bTestVisible)
	{
		// Too soon?
		if(GET_PLAYER(getOwner()).getCultureBombTimer() > 0)
			return false;

		if(pPlot != NULL)
		{
			// Can't be inside someone else's territory
			if(pPlot->getOwner() != NO_PLAYER && pPlot->getOwner() != getOwner())
				return false;

			// We have to be in or next to friendly territory
			bool bFoundAdjacent = false;

			CvPlot* pLoopPlot;
			for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

				if(pLoopPlot != NULL)
				{
					if(pLoopPlot->getOwner() == getOwner())
					{
						bFoundAdjacent = true;
						break;
					}
				}
			}

			if(!bFoundAdjacent)
				return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::DoCultureBomb()
{
	// can we actually do this?
	CvPlot* pThisPlot = plot();
	if(!CanCultureBomb(pThisPlot))
		return false;

	CvUnitEntry *pkUnitEntry = GC.getUnitInfo(getUnitType());
	if (pkUnitEntry)
	{
		// Cooldown
		int iCooldown = /*10*/ GC.getCULTURE_BOMB_COOLDOWN();

		CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
		kPlayer.changeCultureBombTimer(iCooldown);

		PerformCultureBomb(pkUnitEntry->GetCultureBombRadius());

		if(pThisPlot->isActiveVisible(false))
		{
			auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
			gDLL->GameplayUnitActivate(pDllUnit.get());
		}

		if(IsGreatPerson())
		{
#if defined(MOD_EVENTS_GREAT_PEOPLE)
			kPlayer.DoGreatPersonExpended(getUnitType(), this);
#else
			kPlayer.DoGreatPersonExpended(getUnitType());
#endif
		}

		kill(true);

		return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
void CvUnit::PerformCultureBomb(int iRadius)
{
	CvPlot* pThisPlot = plot();

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	// Figure out which City gets ownership of these plots
	int iBestCityID = -1;

	// Plot we're standing on belongs to a city already
	if(pThisPlot->getOwner() == getOwner() && pThisPlot->GetCityPurchaseID() != -1)
	{
		iBestCityID = pThisPlot->GetCityPurchaseID();
	}
	// Find closest city
	else
	{
		int iBestCityDistance = -1;

		int iDistance;

		CvCity* pLoopCity = NULL;
		int iLoop = 0;
		for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
		{
			CvPlot* pPlot = pLoopCity->plot();
			if(pPlot)
			{
				iDistance = plotDistance(getX(), getY(), pLoopCity->getX(), pLoopCity->getY());

				if(iBestCityDistance == -1 || iDistance < iBestCityDistance)
				{
					iBestCityID = pLoopCity->GetID();
					iBestCityDistance = iDistance;
				}
			}
		}
	}

	// Keep track of got hit by this so we can figure the diplo ramifications later
	FStaticVector<bool, MAX_CIV_PLAYERS, true, c_eCiv5GameplayDLL, 0> vePlayersBombed;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		vePlayersBombed.push_back(false);
	}

	// Change ownership of nearby plots
	int iBombRange = iRadius;
	CvPlot* pLoopPlot;
	for(int i = -iBombRange; i <= iBombRange; ++i)
	{
		for(int j = -iBombRange; j <= iBombRange; ++j)
		{
			pLoopPlot = ::plotXYWithRangeCheck(getX(), getY(), i, j, iBombRange);

			if(pLoopPlot == NULL)
				continue;

			// Can't be our plot
			if(pLoopPlot->getOwner() == getOwner())
				continue;

			// Can't flip Cities, sorry
			if(pLoopPlot->isCity())
				continue;

			if(pLoopPlot->getOwner() != NO_PLAYER){
				// Notify plot owner
				if(pLoopPlot->getOwner() != getOwner() && !vePlayersBombed[pLoopPlot->getOwner()]){
					CvNotifications* pNotifications = GET_PLAYER(pLoopPlot->getOwner()).GetNotifications();
					if(pNotifications){
						CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_GREAT_ARTIST_STOLE_PLOT", GET_PLAYER(getOwner()).getNameKey());
						CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_GREAT_ARTIST_STOLE_PLOT", GET_PLAYER(getOwner()).getNameKey());
						pNotifications->Add(NOTIFICATION_GENERIC, strBuffer, strSummary, pLoopPlot->getX(), pLoopPlot->getY(), -1);
					}
				}
				vePlayersBombed[pLoopPlot->getOwner()] = true;
			}

			// Have to set owner after we do the above stuff
			pLoopPlot->setOwner(getOwner(), iBestCityID);
#if defined(MOD_BALANCE_CORE_POLICIES)
			if(GET_PLAYER(getOwner()).IsCitadelBoost())
			{
				int iEra = GET_PLAYER(getOwner()).GetCurrentEra();
				float fDelay = 0.0f;
				if(iEra < 1)
				{
					iEra = 1;
				}
				int iGold = GET_PLAYER(getOwner()).getYieldFromBorderGrowth(YIELD_GOLD) * iEra;
				iGold *= GC.getGame().getGameSpeedInfo().getTrainPercent();
				iGold /= 100;
				if(iGold > 0)
				{
					GET_PLAYER(getOwner()).GetTreasury()->ChangeGold(iGold);
					if(getOwner() == GC.getGame().getActivePlayer())
					{
						char text[256] = {0};
						fDelay += 0.5f;
						sprintf_s(text, "[COLOR_YELLOW]+%d[ENDCOLOR][ICON_GOLD]", iGold);
						DLLUI->AddPopupText(pLoopPlot->getX(),pLoopPlot->getY(), text, fDelay);
					}
				}
				int iFaith = GET_PLAYER(getOwner()).getYieldFromBorderGrowth(YIELD_FAITH) * iEra;
				iFaith *= GC.getGame().getGameSpeedInfo().getTrainPercent();
				iFaith /= 100;
				if(iFaith > 0)
				{
					GET_PLAYER(getOwner()).ChangeFaith(iFaith);
					if(getOwner() == GC.getGame().getActivePlayer())
					{
						char text[256] = {0};
						fDelay += 0.5f;
						sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_PEACE]", iFaith);
						DLLUI->AddPopupText(pLoopPlot->getX(),pLoopPlot->getY(), text, fDelay);
					}
				}
				int iCulture = GET_PLAYER(getOwner()).getYieldFromBorderGrowth(YIELD_CULTURE) * iEra;
				iCulture *= GC.getGame().getGameSpeedInfo().getTrainPercent();
				iCulture /= 100;
				if(iCulture > 0)
				{
					GET_PLAYER(getOwner()).changeJONSCulture(iCulture);
					if(getOwner() == GC.getGame().getActivePlayer())
					{
						char text[256] = {0};
						fDelay += 0.5f;
						sprintf_s(text, "[COLOR_MAGENTA]+%d[ENDCOLOR][ICON_CULTURE]", iCulture);
						DLLUI->AddPopupText(pLoopPlot->getX(),pLoopPlot->getY(), text, fDelay);
					}
				}
				int iGAP = GET_PLAYER(getOwner()).getYieldFromBorderGrowth(YIELD_GOLDEN_AGE_POINTS) * iEra;
				iGAP *= GC.getGame().getGameSpeedInfo().getTrainPercent();
				iGAP /= 100;
				if(iGAP > 0)
				{
					GET_PLAYER(getOwner()).ChangeGoldenAgeProgressMeter(iGAP);
					if(getOwner() == GC.getGame().getActivePlayer())
					{
						char text[256] = {0};
						fDelay += 0.5f;
						sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_GOLDEN_AGE]", iGAP);
						DLLUI->AddPopupText(pLoopPlot->getX(),pLoopPlot->getY(), text, fDelay);
					}
				}
				int iScience = GET_PLAYER(getOwner()).getYieldFromBorderGrowth(YIELD_SCIENCE) * iEra;
				iScience *= GC.getGame().getGameSpeedInfo().getTrainPercent();
				iScience /= 100;
				if(iScience > 0)
				{
					TechTypes eCurrentTech = GET_PLAYER(getOwner()).GetPlayerTechs()->GetCurrentResearch();
					if(eCurrentTech == NO_TECH)
					{
						GET_PLAYER(getOwner()).changeOverflowResearch(iScience);
					}
					else
					{
						GET_TEAM(GET_PLAYER(getOwner()).getTeam()).GetTeamTechs()->ChangeResearchProgress(eCurrentTech, iScience, getOwner());
					}
					if(getOwner() == GC.getGame().getActivePlayer())
					{
						char text[256] = {0};
						fDelay += 0.5f;
						sprintf_s(text, "[COLOR_BLUE]+%d[ENDCOLOR][ICON_RESEARCH]", iScience);
						DLLUI->AddPopupText(pLoopPlot->getX(),pLoopPlot->getY(), text, fDelay);
					}
				}
				int iGGP = GET_PLAYER(getOwner()).getYieldFromBorderGrowth(YIELD_GREAT_GENERAL_POINTS) * iEra;
				iGGP *= GC.getGame().getGameSpeedInfo().getTrainPercent();
				iGGP /= 100;
				if(iGGP > 0)
				{
					GET_PLAYER(getOwner()).changeCombatExperience(iGGP);
					if(getOwner() == GC.getGame().getActivePlayer())
					{
						char text[256] = {0};
						fDelay += 0.5f;
						sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_GREAT_GENERAL]", iGGP);
						DLLUI->AddPopupText(pLoopPlot->getX(),pLoopPlot->getY(), text, fDelay);
					}
				}
			}
#endif
		}
	}

	bool bAlreadyShownLeader = false;

	// Now that we know who was hit, figure the diplo ramifications
	CvPlayer* pPlayer;
	for(int iSlotLoop = 0; iSlotLoop < MAX_CIV_PLAYERS; iSlotLoop++)
	{
		if(vePlayersBombed[iSlotLoop])
		{
			pPlayer = &GET_PLAYER((PlayerTypes) iSlotLoop);
			TeamTypes eOtherTeam = pPlayer->getTeam();

			// Humans can handle their own diplo
			if(pPlayer->isHuman())
				continue;

			// Minor civ response
			if(pPlayer->isMinorCiv())
			{
				int iFriendship = /*-50*/ GC.getCULTURE_BOMB_MINOR_FRIENDSHIP_CHANGE();
				pPlayer->GetMinorCivAI()->ChangeFriendshipWithMajor(getOwner(), iFriendship);
			}
			// Major civ response
			else
			{
				pPlayer->GetDiplomacyAI()->ChangeNumTimesCultureBombed(getOwner(), 1);

				// Message for human
				if(getTeam() != eOtherTeam && !GET_TEAM(eOtherTeam).isAtWar(getTeam()) && !CvPreGame::isNetworkMultiplayerGame() && GC.getGame().getActivePlayer() == getOwner() && !bAlreadyShownLeader)
				{
					bAlreadyShownLeader = true;

					DLLUI->SetForceDiscussionModeQuitOnBack(true);		// Set force quit so that when discuss mode pops up the Back button won't go to leader root
					const char* strText = pPlayer->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_CULTURE_BOMBED);
					gDLL->GameplayDiplomacyAILeaderMessage(pPlayer->GetID(), DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::canGoldenAge(const CvPlot* pPlot, bool bTestVisible) const
{
	VALIDATE_OBJECT
	if(!isGoldenAge())
	{
		return false;
	}

	// If prophet has  started spreading religion, can't do other functions
	if(m_pUnitInfo->IsSpreadReligion())
	{
		if(GetReligionData()->GetSpreadsLeft() < m_pUnitInfo->GetReligionSpreads())
		{
			return false;
		}
	}

	// Things we test ONLY when we're going to try to perform this action RIGHT NOW
	if(!bTestVisible)
	{
		// Must be in player's territory
		if(pPlot->getOwner() != getOwner())
			return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::goldenAge()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();

	if(!canGoldenAge(pPlot))
	{
		return false;
	}

	int iGoldenAgeTurns = GetGoldenAgeTurns();

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	kPlayer.changeGoldenAgeTurns(iGoldenAgeTurns);
	kPlayer.changeNumUnitGoldenAges(1);

	if(pPlot->isActiveVisible(false))
	{
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
	}

	if(IsGreatPerson())
	{
#if defined(MOD_EVENTS_GREAT_PEOPLE)
		kPlayer.DoGreatPersonExpended(getUnitType(), this);
#else
		kPlayer.DoGreatPersonExpended(getUnitType());
#endif
	}

	kill(true);

	return true;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetGoldenAgeTurns() const
{
	int iGoldenAgeTurns = m_pUnitInfo->GetGoldenAgeTurns();

	// Unit cannot perform a GA
	if(iGoldenAgeTurns == 0)
		return 0;

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	int iMinTurns = /*3*/ GC.getMIN_UNIT_GOLDEN_AGE_TURNS();
	if(iGoldenAgeTurns < iMinTurns)
		iGoldenAgeTurns = iMinTurns;

	// Player mod
	int iLengthModifier = kPlayer.getGoldenAgeModifier();

#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	// Do we get increased Golden Ages from a resource monopoly?
	if(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	{
		for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			ResourceTypes eResourceLoop = (ResourceTypes) iResourceLoop;
			if(eResourceLoop != NO_RESOURCE)
			{
				CvResourceInfo* pInfo = GC.getResourceInfo(eResourceLoop);
				if (pInfo && pInfo->isMonopoly())
				{
					if(kPlayer.HasGlobalMonopoly(eResourceLoop) && pInfo->getMonopolyGALength() > 0)
					{
						int iTemp = pInfo->getMonopolyGALength();
						iTemp += kPlayer.GetMonopolyModPercent();
						iLengthModifier += iTemp;
					}
				}
			}
		}
	}
#endif
	// Trait mod
	iLengthModifier += kPlayer.GetPlayerTraits()->GetGoldenAgeDurationModifier();
	if(iLengthModifier > 0)
		iGoldenAgeTurns = iGoldenAgeTurns * (100 + iLengthModifier) / 100;

	// Game Speed mod

	iGoldenAgeTurns *= GC.getGame().getGameSpeedInfo().getGoldenAgePercent();
	iGoldenAgeTurns /= 100;

	if(iGoldenAgeTurns < 1)
		iGoldenAgeTurns = 1;

	return iGoldenAgeTurns;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canGivePolicies(const CvPlot* /*pPlot*/, bool /*bTestVisible*/) const
{
	VALIDATE_OBJECT
	if(!isGivesPolicies() && getUnitInfo().GetBaseCultureTurnsToCount() == 0)
	{
		return false;
	}

	// If prophet has  started spreading religion, can't do other functions
	if(m_pUnitInfo->IsSpreadReligion())
	{
		if(GetReligionData()->GetSpreadsLeft() < m_pUnitInfo->GetReligionSpreads())
		{
			return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
int CvUnit::getGivePoliciesCulture()
{
	int iValue = 0;
#if !defined(MOD_BALANCE_CORE)
	CvPlot* pPlot = plot();
	if(canGivePolicies(pPlot))
#endif
	{
		CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

		// Culture boost based on previous turns
		int iPreviousTurnsToCount = getUnitInfo().GetBaseCultureTurnsToCount();
		if (iPreviousTurnsToCount != 0)
		{
			// Calculate boost
			iValue = kPlayer.GetCultureYieldFromPreviousTurns(GC.getGame().getGameTurn(), iPreviousTurnsToCount);
		}

		// Modify based on game speed
		iValue *= GC.getGame().getGameSpeedInfo().getCulturePercent();
		iValue /= 100;
	}
	return iValue;
}

//	--------------------------------------------------------------------------------
bool CvUnit::givePolicies()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();
	if(!canGivePolicies(pPlot))
	{
		return false;
	}
	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	// Culture boost based on previous turns
#if defined(MOD_BALANCE_CORE)
	int iCultureBonus = GetCultureBlastStrength();
#else
	int iCultureBonus = getGivePoliciesCulture();
#endif
	if (iCultureBonus != 0)
	{
		kPlayer.changeJONSCulture(iCultureBonus);
		// Refresh - we might get to pick a policy this turn
	}

	// Free policies
	int iFreePolicies = m_pUnitInfo->GetFreePolicies();
	if (iFreePolicies != 0)
	{
		kPlayer.ChangeNumFreePolicies(iFreePolicies);
	}

	if(pPlot->isActiveVisible(false))
	{
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
	}

	if(IsGreatPerson())
	{
#if defined(MOD_EVENTS_GREAT_PEOPLE)
		kPlayer.DoGreatPersonExpended(getUnitType(), this);
#else
		kPlayer.DoGreatPersonExpended(getUnitType());
#endif
	}

	kill(true);

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canBlastTourism(const CvPlot* pPlot, bool bTestVisible) const
{
	VALIDATE_OBJECT
	if(!isBlastTourism())
	{
		return false;
	}

	//If we're just testing capability, then yes.
	if(bTestVisible)
	{
		return true;
	}

	// Must be a tile owned by another major civilization
	PlayerTypes eOwner = pPlot->getOwner();
	if (eOwner == NO_PLAYER)
	{
		return false;
	}

	CvPlayer &kTileOwner = GET_PLAYER(eOwner);
	if (kTileOwner.isAlive() && !kTileOwner.isMinorCiv() && eOwner != getOwner())
	{
		return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
int CvUnit::getBlastTourism()
{
	if (!canBlastTourism(plot()))
	{
		return 0;
	}

	int iTourismBlast = GetTourismBlastStrength();
	iTourismBlast = iTourismBlast * GC.getGame().getGameSpeedInfo().getCulturePercent() / 100;

	return iTourismBlast;
}

//	--------------------------------------------------------------------------------
bool CvUnit::blastTourism()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();

	if(!canBlastTourism(pPlot))
	{
		return false;
	}

	int iTourismBlast = getBlastTourism();

	int iTourismBlastPercentOthers = m_pUnitInfo->GetOneShotTourismPercentOthers();
	PlayerTypes eOwner = pPlot->getOwner();
	CvPlayer &kUnitOwner = GET_PLAYER(getOwner());

	// Apply to target
	kUnitOwner.GetCulture()->ChangeInfluenceOn(eOwner, iTourismBlast);

	// Apply lesser amount to other civs
	int iTourismBlastOthers = iTourismBlast * iTourismBlastPercentOthers / 100;
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if (eLoopPlayer != eOwner && eLoopPlayer != getOwner() && kUnitOwner.GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			kUnitOwner.GetCulture()->ChangeInfluenceOn(eLoopPlayer, iTourismBlastOthers);
		}
	}

	if(pPlot->isActiveVisible(false))
	{
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
	}

	if(IsGreatPerson())
	{
#if defined(MOD_EVENTS_GREAT_PEOPLE)
		kUnitOwner.DoGreatPersonExpended(getUnitType(), this);
#else
		kUnitOwner.DoGreatPersonExpended(getUnitType());
#endif
	}

	kill(true);
 
	// Show tourism spread
	if (pPlot->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF)
	{
		CvString strInfluenceText;
		InfluenceLevelTypes eLevel = kUnitOwner.GetCulture()->GetInfluenceLevel(eOwner);

		if (eLevel == INFLUENCE_LEVEL_UNKNOWN)
			strInfluenceText = GetLocalizedText( "TXT_KEY_CO_UNKNOWN" );
		else if (eLevel == INFLUENCE_LEVEL_EXOTIC)
			strInfluenceText = GetLocalizedText( "TXT_KEY_CO_EXOTIC");
		else if (eLevel == INFLUENCE_LEVEL_FAMILIAR)
			strInfluenceText = GetLocalizedText( "TXT_KEY_CO_FAMILIAR");
		else if (eLevel == INFLUENCE_LEVEL_POPULAR)
			strInfluenceText = GetLocalizedText( "TXT_KEY_CO_POPULAR");
		else if (eLevel == INFLUENCE_LEVEL_INFLUENTIAL)
			strInfluenceText = GetLocalizedText( "TXT_KEY_CO_INFLUENTIAL");
		else if (eLevel == INFLUENCE_LEVEL_DOMINANT)
			strInfluenceText = GetLocalizedText( "TXT_KEY_CO_DOMINANT");

		char text[256] = {0};
		sprintf_s(text, "[COLOR_WHITE]+%d [ICON_TOURISM][ENDCOLOR]   %s", iTourismBlast, strInfluenceText.c_str());
#if defined(SHOW_PLOT_POPUP)
		SHOW_PLOT_POPUP(pPlot, getOwner(), text, 0.0f);
#else
		float fDelay = 0.0f;
		DLLUI->AddPopupText(pPlot->getX(), pPlot->getY(), text, fDelay);
#endif
	}

#if !defined(NO_ACHIEVEMENTS)
	// Achievements
	if (GET_PLAYER(m_eOwner).isHuman() && !GC.getGame().isGameMultiPlayer())
	{
		if (strcmp(GET_PLAYER(eOwner).getCivilizationTypeKey(),   "CIVILIZATION_AMERICA") == 0 &&
			strcmp(GET_PLAYER(m_eOwner).getCivilizationTypeKey(), "CIVILIZATION_ENGLAND") == 0)
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_XP2_36);
		}

		ImprovementTypes eTempImprovement = pPlot->getImprovementType();
		if (eTempImprovement != NO_IMPROVEMENT)
		{
			CvImprovementEntry* pkImprovement = GC.getImprovementInfo(eTempImprovement);
			if (strcmp(pkImprovement->GetType(), "IMPROVEMENT_KASBAH") == 0)
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_XP2_37);
			}
		}
	}
#endif

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestVisible, bool bTestGold) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eBuild < GC.getNumBuildInfos() && eBuild >= 0, "Index out of bounds");

#if defined(MOD_BALANCE_CORE)
	if(!GET_PLAYER(getOwner()).GetPlayerTraits()->HasUnitClassCanBuild(eBuild, getUnitClassType()) && !m_pUnitInfo->GetBuilds(eBuild))
#else
	if(!(m_pUnitInfo->GetBuilds(eBuild)))
#endif
	{
		return false;
	}

	CvBuildInfo *pkBuildInfo = GC.getBuildInfo(eBuild);
	if (!pkBuildInfo)
	{
		return false;
	}

	// If prophet has  started spreading religion, can't do other functions
	if(m_pUnitInfo->IsSpreadReligion())
	{
		if (GetReligionData()->GetReligion() != NO_RELIGION && GetReligionData()->GetSpreadsLeft() < m_pUnitInfo->GetReligionSpreads())
		{
			return false;
		}
	}

	if(!(GET_PLAYER(getOwner()).canBuild(pPlot, eBuild, false, bTestVisible, bTestGold)))
	{
		return false;
	}

#if defined(MOD_AI_SECONDARY_WORKERS)
	bool bValidBuildPlot = pPlot->isValidDomainForAction(*this) ||
						  (pkBuildInfo->IsWater() && getDomainType() == DOMAIN_LAND && pPlot->isWater() && IsHasEmbarkAbility());
	if(!bValidBuildPlot)
#else
	if(!pPlot->isValidDomainForAction(*this))
#endif
	{
		return false;
	}

	if (pPlot->isWater())
	{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
		// Needs the associated SQL executing - UPDATE Builds SET CanBeEmbarked=1 WHERE Type='BUILD_FISHING_BOATS_NO_KILL';
		if ((isEmbarked() && !pkBuildInfo->IsCanBeEmbarked()))
#else
		if ((isEmbarked() && !pkBuildInfo->IsCanBeEmbarked())  && (strcmp("UNIT_JAPANESE_SAMURAI", getUnitInfo().GetType()) != 0))
#endif
		{
			return false;
		}
	}

	if(!bTestVisible)
	{
		// check for any other units working in this plot
		pPlot = plot();
		const IDInfo* pUnitNode = pPlot->headUnitNode();
		const CvUnit* pLoopUnit = NULL;

		while(pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(*pUnitNode);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);

			if(pLoopUnit && pLoopUnit != this)
			{
				if(pLoopUnit->IsWork() && pLoopUnit->getBuildType() != NO_BUILD)
				{
					return false;
				}
			}
		}
	}

#if defined(MOD_EVENTS_PLOT)
	if (MOD_EVENTS_PLOT) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanBuild, getOwner(), GetID(), getX(), getY(), eBuild) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
#endif

	return true;
}

//	--------------------------------------------------------------------------------
// Returns true if build finished...
bool CvUnit::build(BuildTypes eBuild)
{
	VALIDATE_OBJECT
	bool bFinished;

	CvAssertMsg(eBuild < GC.getNumBuildInfos(), "Invalid Build");
	CvPlayer& kPlayer = GET_PLAYER(getOwner());

	CvBuildInfo* pkBuildInfo = NULL;
	if(eBuild != NO_BUILD)
	{
		pkBuildInfo = GC.getBuildInfo(eBuild);
	}
	CvPlot* pPlot = plot();

	// Don't test Gold
	if(!canBuild(pPlot, eBuild, false, false))
	{
		return false;
	}

	if(GC.getLogging() && GC.getAILogging())
	{
		if(pPlot->getBuildProgress(eBuild) == 0)
		{
			LogWorkerEvent(eBuild, true);
		}
	}

	int iStartedYet = pPlot->getBuildProgress(eBuild);

	// if we are starting something new wipe out the old thing immediately
	if(iStartedYet == 0)
	{
		ImprovementTypes eImprovement = NO_IMPROVEMENT;

		if(pkBuildInfo)
		{
			eImprovement = (ImprovementTypes) pkBuildInfo->getImprovement();
		}

		if(eImprovement != NO_IMPROVEMENT)
		{
			if(pPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				CvImprovementEntry *pkImprovementEntry = GC.getImprovementInfo(eImprovement);
				if (!pkImprovementEntry || !pkImprovementEntry->IsRequiresImprovement())
				{
					pPlot->setImprovementType(NO_IMPROVEMENT);
				}
			}
		}

		// wipe out all build progress also

		bFinished = pPlot->changeBuildProgress(eBuild, workRate(false), getOwner());

	}

	bFinished = pPlot->changeBuildProgress(eBuild, workRate(false), getOwner());

#if defined(MOD_EVENTS_PLOT)
	if (MOD_EVENTS_PLOT) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_PlayerBuilding, getOwner(), GetID(), getX(), getY(), eBuild, (iStartedYet == 0));
	}
#endif

	finishMoves(); // needs to be at bottom because movesLeft() can affect workRate()...

	if(bFinished)
	{
		// Update Resource info
		if(pkBuildInfo)
		{
			ImprovementTypes eImprovement = NO_IMPROVEMENT;
			RouteTypes eRoute = NO_ROUTE;

			if(pkBuildInfo->getImprovement() != NO_IMPROVEMENT)
			{
				eImprovement = (ImprovementTypes) pkBuildInfo->getImprovement();

				CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
				if(pkImprovementInfo)
				{
					if (pkImprovementInfo->GetCultureBombRadius() > 0)
					{
#if defined(MOD_BALANCE_CORE_POLICIES)
						int iCultureBomb = pkImprovementInfo->GetCultureBombRadius();
						iCultureBomb += GET_PLAYER(getOwner()).GetCitadelBoost();

						PerformCultureBomb(iCultureBomb);
#else
						PerformCultureBomb(pkImprovementInfo->GetCultureBombRadius());
#endif
					}
				}
			}
#if defined(GLOBAL_ALPINE_PASSES)
			// Can be both an improvement and a route
			if(pkBuildInfo->getRoute() != NO_ROUTE)
#else
			else if(pkBuildInfo->getRoute() != NO_ROUTE)
#endif
			{
				eRoute = (RouteTypes) pkBuildInfo->getRoute();
			}

			int iNumResource = 0;

			// Update the amount of a Resource used up by popped Build
			for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				if(eImprovement != NO_IMPROVEMENT)
				{
					CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
					if(pkImprovementInfo)
					{
						iNumResource = pkImprovementInfo->GetResourceQuantityRequirement(iResourceLoop);
					}
				}
#if defined(GLOBAL_ALPINE_PASSES)
				// Can be both an improvement and a route
				if(eRoute != NO_ROUTE)
#else
				else if(eRoute != NO_ROUTE)
#endif
				{
					CvRouteInfo* pkRouteInfo = GC.getRouteInfo(eRoute);
					if(pkRouteInfo)
					{
#if defined(GLOBAL_ALPINE_PASSES)
						iNumResource += pkRouteInfo->getResourceQuantityRequirement(iResourceLoop);
#else
						iNumResource = pkRouteInfo->getResourceQuantityRequirement(iResourceLoop);
#endif
					}
				}

				if(iNumResource > 0)
				{
					kPlayer.changeNumResourceUsed((ResourceTypes) iResourceLoop, -iNumResource);
				}
			}

			if(pkBuildInfo->isKill())
			{
				if (pPlot->isActiveVisible(false))
				{
					auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
					gDLL->GameplayUnitActivate(pDllUnit.get());
				}

				if(IsGreatPerson())
				{
#if defined(MOD_EVENTS_GREAT_PEOPLE)
					kPlayer.DoGreatPersonExpended(getUnitType(), this);
#else
					kPlayer.DoGreatPersonExpended(getUnitType());
#endif
				}

				kill(true);
			}

			// Add to player's Improvement count, which will increase cost of future Improvements
			if(pkBuildInfo->getImprovement() != NO_IMPROVEMENT || pkBuildInfo->getRoute() != NO_ROUTE)	// Prevents chopping Forest or Jungle from counting
			{
				kPlayer.changeTotalImprovementsBuilt(1);
			}

			if(GC.getLogging() && GC.getAILogging())
			{
				LogWorkerEvent(eBuild, false);
			}
		}

#if defined(MOD_EVENTS_PLOT)
		if (MOD_EVENTS_PLOT) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_PlayerBuilt, getOwner(), GetID(), getX(), getY(), eBuild);
		}
#endif
	}
	else // we are not done doing this
	{
		if(pkBuildInfo)
		{
			ImprovementTypes eImprovement = NO_IMPROVEMENT;
			RouteTypes eRoute = NO_ROUTE;

			if(pkBuildInfo->getImprovement() != NO_IMPROVEMENT)
			{
				eImprovement = (ImprovementTypes) pkBuildInfo->getImprovement();
			}
			else if(pkBuildInfo->getRoute() != NO_ROUTE)
			{
				eRoute = (RouteTypes) pkBuildInfo->getRoute();
			}

			if(iStartedYet == 0)
			{
				if(pPlot->isVisible(GC.getGame().getActiveTeam(), true))
				{
					if(eImprovement != NO_IMPROVEMENT)
					{
						pPlot->setLayoutDirty(true);

					}
					else if(eRoute != NO_ROUTE)
					{
						pPlot->setLayoutDirty(true);
					}
				}
			}
		}
	}

	return bFinished;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canPromote(PromotionTypes ePromotion, int iLeaderUnitId) const
{
	VALIDATE_OBJECT
	if(iLeaderUnitId >= 0)
	{
		if(iLeaderUnitId == GetID())
		{
			return false;
		}

		// The command is always possible if it's coming from a Warlord unit that gives just experience points
		UnitHandle pWarlord = GET_PLAYER(getOwner()).getUnit(iLeaderUnitId);
		if(pWarlord &&
				NO_UNIT != pWarlord->getUnitType() &&
				pWarlord->getUnitInfo().GetLeaderExperience() > 0 &&
				NO_PROMOTION == pWarlord->getUnitInfo().GetLeaderPromotion() &&
				canAcquirePromotionAny())
		{
			return true;
		}
	}

	if(ePromotion == NO_PROMOTION)
	{
		return false;
	}

	CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
	if(pkPromotionInfo == NULL)
	{
		return false;
	}

	if(!canAcquirePromotion(ePromotion))
	{
		return false;
	}

	if(pkPromotionInfo->IsLeader())
	{
		if(iLeaderUnitId >= 0)
		{
			UnitHandle pWarlord = GET_PLAYER(getOwner()).getUnit(iLeaderUnitId);
			if(pWarlord && NO_UNIT != pWarlord->getUnitType())
			{
				return (pWarlord->getUnitInfo().GetLeaderPromotion() == ePromotion);
			}
		}
		return false;
	}
	else
	{
		if(!isPromotionReady())
		{
			return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
void CvUnit::promote(PromotionTypes ePromotion, int iLeaderUnitId)
{
	VALIDATE_OBJECT

	CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
	if(pkPromotionInfo == NULL)
	{
		return;
	}

	if(!canPromote(ePromotion, iLeaderUnitId))
	{
		return;
	}

	if(iLeaderUnitId >= 0)
	{
		UnitHandle pWarlord = GET_PLAYER(getOwner()).getUnit(iLeaderUnitId);
		if(pWarlord)
		{
			pWarlord->giveExperience();
			if(!pWarlord->getNameNoDesc().empty())
			{
				setName(pWarlord->getName());
			}

			//update graphics models
			m_eLeaderUnitType = pWarlord->getUnitType();
		}
	}

	if(!pkPromotionInfo->IsLeader())
	{
		changeLevel(1);
	}

	// Insta-Heal: never earned
	if(pkPromotionInfo->IsInstaHeal())
	{
		changeDamage(-GC.getINSTA_HEAL_RATE());
	}
	// Set that we have this Promotion
	else
	{
		setHasPromotion(ePromotion, true);

#if defined(MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED)
		if(MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED)
		{
			//Insta-heal removed, gain health with each promotion instead.
			changeDamage((-GC.getINSTA_HEAL_RATE() / 5));
		}
#endif

#if defined(MOD_EVENTS_UNIT_UPGRADES)
		if (MOD_EVENTS_UNIT_UPGRADES) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_UnitPromoted, getOwner(), GetID(), ePromotion);
		} else {
#endif
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem)
		{
			CvLuaArgsHandle args;
			args->Push(((int)getOwner()));
			args->Push(GetID());
			args->Push(ePromotion);

			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "UnitPromoted", args.get(), bResult);
		}
#if defined(MOD_EVENTS_UNIT_UPGRADES)
		}
#endif
	}

	testPromotionReady();

	if(IsSelected())
	{
		DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
	}
	else
	{
		setInfoBarDirty(true);
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::lead(int iUnitId)
{
	VALIDATE_OBJECT
	if(!canLead(plot(), iUnitId))
	{
		return false;
	}

	PromotionTypes eLeaderPromotion = (PromotionTypes)m_pUnitInfo->GetLeaderPromotion();

	if(-1 == iUnitId)
	{
		return false;
	}
	else
	{
		UnitHandle pUnit = GET_PLAYER(getOwner()).getUnit(iUnitId);

		if(!pUnit || !pUnit->canPromote(eLeaderPromotion, GetID()))
		{
			return false;
		}

		pUnit->promote(eLeaderPromotion, GetID());

		kill(true);

		return true;
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::canLead(const CvPlot* pPlot, int iUnitId) const
{
	VALIDATE_OBJECT
	if(isDelayedDeath())
	{
		return 0;
	}

	if(NO_UNIT == getUnitType())
	{
		return 0;
	}

	int iNumUnits = 0;
	CvUnitEntry& kUnitInfo = getUnitInfo();

	if(-1 == iUnitId)
	{
		const IDInfo* pUnitNode = pPlot->headUnitNode();
		while(pUnitNode != NULL)
		{
			const CvUnit* pUnit = ::getUnit(*pUnitNode);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);

			if(pUnit && pUnit != this && pUnit->getOwner() == getOwner() && pUnit->canPromote((PromotionTypes)kUnitInfo.GetLeaderPromotion(), GetID()))
			{
				++iNumUnits;
			}
		}
	}
	else
	{
		const UnitHandle pUnit = GET_PLAYER(getOwner()).getUnit(iUnitId);
		if(pUnit && pUnit != this && pUnit->canPromote((PromotionTypes)kUnitInfo.GetLeaderPromotion(), GetID()))
		{
			iNumUnits = 1;
		}
	}
	return iNumUnits;
}


//	--------------------------------------------------------------------------------
int CvUnit::canGiveExperience(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	int iNumUnits = 0;

	if(NO_UNIT != getUnitType() && getUnitInfo().GetLeaderExperience() > 0)
	{
		const IDInfo* pUnitNode = pPlot->headUnitNode();
		while(pUnitNode != NULL)
		{
			const CvUnit* pUnit = ::getUnit(*pUnitNode);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);

			if(pUnit && pUnit != this && pUnit->getOwner() == getOwner() && pUnit->canAcquirePromotionAny())
			{
				++iNumUnits;
			}
		}
	}

	return iNumUnits;
}

//	--------------------------------------------------------------------------------
bool CvUnit::giveExperience()
{
	VALIDATE_OBJECT
	CvPlot* pPlot = plot();

	if(pPlot)
	{
		int iNumUnits = canGiveExperience(pPlot);
		if(iNumUnits > 0)
		{
			int iTotalExperience = getStackExperienceToGive(iNumUnits);

			int iMinExperiencePerUnit = iTotalExperience / iNumUnits;
			int iRemainder = iTotalExperience % iNumUnits;

			IDInfo* pUnitNode = pPlot->headUnitNode();
			int i = 0;
			while(pUnitNode != NULL)
			{
				CvUnit* pUnit = ::getUnit(*pUnitNode);
				pUnitNode = pPlot->nextUnitNode(pUnitNode);

				if(pUnit && pUnit != this && pUnit->getOwner() == getOwner() && pUnit->canAcquirePromotionAny())
				{
					pUnit->changeExperience(i < iRemainder ? iMinExperiencePerUnit+1 : iMinExperiencePerUnit);
					pUnit->testPromotionReady();
				}

				i++;
			}

			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
int CvUnit::getStackExperienceToGive(int iNumUnits) const
{
	VALIDATE_OBJECT
	return (getUnitInfo().GetLeaderExperience() * (100 + std::min(50, (iNumUnits - 1) * GC.getWARLORD_EXTRA_EXPERIENCE_PER_UNIT_PERCENT()))) / 100;
}

//	--------------------------------------------------------------------------------
/// Is the Unit currently in a state that allows it to upgrade?
bool CvUnit::isReadyForUpgrade() const
{
	VALIDATE_OBJECT

	if(m_iMoves <= 0)
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
/// Can this Unit upgrade with anything right now?
bool CvUnit::CanUpgradeRightNow(bool bOnlyTestVisible) const
{
	VALIDATE_OBJECT
	// Is Unit in a state where it can upgrade?
	if(!isReadyForUpgrade())
		return false;

	UnitTypes eUpgradeUnitType = GetUpgradeUnitType();
	
#if defined(MOD_API_EXTENSIONS)
	return CanUpgradeTo(eUpgradeUnitType, bOnlyTestVisible);
}

bool CvUnit::CanUpgradeTo(UnitTypes eUpgradeUnitType, bool bOnlyTestVisible) const
{
#endif
	// Does the Unit actually upgrade into anything?
	if(eUpgradeUnitType == NO_UNIT)
		return false;

	CvUnitEntry* pUpgradeUnitInfo = GC.getUnitInfo(eUpgradeUnitType);
	if(pUpgradeUnitInfo == NULL)
		return false;

	// Tech requirement
	TechTypes ePrereqTech = (TechTypes) pUpgradeUnitInfo->GetPrereqAndTech();

	if(ePrereqTech != NO_TECH && !GET_TEAM(getTeam()).GetTeamTechs()->HasTech(ePrereqTech))
		return false;

#if defined(MOD_BUGFIX_UNIT_PREREQ_PROJECT)
	// Project requirement
	ProjectTypes ePrereqProject = (ProjectTypes) pUpgradeUnitInfo->GetProjectPrereq();
	if (ePrereqProject != NO_PROJECT) {
		CvProjectEntry* pkProjectInfo = GC.getProjectInfo(ePrereqProject);
		if (pkProjectInfo && GET_TEAM(getTeam()).getProjectCount(ePrereqProject) == 0)
			return false;
	}
#endif
		
	CvPlot* pPlot = plot();

	// Show the upgrade, but don't actually allow it
	if(!bOnlyTestVisible)
	{
#if defined(MOD_GLOBAL_STACKING_RULES)
		if(pPlot->getNumFriendlyUnitsOfType(this) > pPlot->getUnitLimit())
#else
		if(pPlot->getNumFriendlyUnitsOfType(this) > GC.getPLOT_UNIT_LIMIT())
#endif
		{
			return false;
		}

#if defined(MOD_GLOBAL_CS_UPGRADES)
		if (!CanUpgradeInTerritory(bOnlyTestVisible))
			return false;
#else
		// Must be in territory owned by the player
		if(pPlot->getOwner() != getOwner())
			return false;
#endif

		CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

		// Player must have enough Gold
		if(kPlayer.GetTreasury()->GetGold() < upgradePrice(eUpgradeUnitType))
			return false;

		// Resource Requirements
		int iNumOfThisResourceAvailable;
		ResourceTypes eResource;
		int iNumResourceNeeded;
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			eResource = (ResourceTypes) iResourceLoop;
			iNumResourceNeeded = pUpgradeUnitInfo->GetResourceQuantityRequirement(eResource);

			if(iNumResourceNeeded > 0)
			{
				// Amount we have lying around
				iNumOfThisResourceAvailable = kPlayer.getNumResourceAvailable(eResource);
				// Amount this old unit is using
				iNumOfThisResourceAvailable += m_pUnitInfo->GetResourceQuantityRequirement(eResource);

				if(iNumOfThisResourceAvailable <= 0 || iNumOfThisResourceAvailable < iNumResourceNeeded)
					return false;
			}
		}

		if(getDomainType() == DOMAIN_AIR)
		{
			if(!pPlot->isCity() || isCargo())	// Air units must be in a city, and no sneaky "I'm on a carrier in the city" either.
			{
				return false;
			}
		}

		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem)
		{
			CvLuaArgsHandle args;
			args->Push(((int)getOwner()));
			args->Push(GetID());

			bool bResult = false;
			if (LuaSupport::CallTestAll(pkScriptSystem, "CanHaveAnyUpgrade", args.get(), bResult)) {
				if (bResult == false) {
					return false;
	}
			}
		}
	}

#if defined(MOD_EVENTS_UNIT_UPGRADES)
	if (MOD_EVENTS_UNIT_UPGRADES) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_CanHaveAnyUpgrade, getOwner(), GetID()) == GAMEEVENTRETURN_FALSE) {
			return false;
		}

		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_UnitCanHaveAnyUpgrade, getOwner(), GetID()) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	} else {
#endif
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem)
		{
			CvLuaArgsHandle args;
			args->Push(((int)getOwner()));
			args->Push(GetID());

			bool bResult = false;
			if (LuaSupport::CallTestAll(pkScriptSystem, "CanHaveAnyUpgrade", args.get(), bResult)) {
				if (bResult == false) {
					return false;
				}
			}
		}
#if defined(MOD_EVENTS_UNIT_UPGRADES)
	}
#endif

	return true;
}

#if defined(MOD_GLOBAL_CS_UPGRADES)
/// Can this Unit upgrade with anything right now?
bool CvUnit::CanUpgradeInTerritory(bool bOnlyTestVisible) const
{
	VALIDATE_OBJECT

	// Show the upgrade, but don't actually allow it
	if(!bOnlyTestVisible)
	{
		CvPlot* pPlot = plot();
		const PlayerTypes kPlotOwner = pPlot->getOwner();

		// Must be in territory owned by the player or by an allied militaristic City State
		if (kPlotOwner != getOwner()) 
		{
			if(MOD_GLOBAL_CS_UPGRADES && kPlotOwner != NO_PLAYER && GET_PLAYER(getOwner()).CanUpgradeCSTerritory()) 
			{
				const CvPlayer& pPlotOwner = GET_PLAYER(kPlotOwner);
				if (!pPlotOwner.isMinorCiv())
				{
					return false;
				}
				else
				{
					if(pPlotOwner.GetMinorCivAI()->GetAlly() != getOwner())
					{
						return false;
					}
				}
			} 
			else 
			{
				return false;
			}
		}
	}

	return true;
}
#endif

//	--------------------------------------------------------------------------------
/// What UnitType does this Unit upgrade into?
UnitTypes CvUnit::GetUpgradeUnitType() const
{
	VALIDATE_OBJECT
	UnitTypes eUpgradeUnitType = NO_UNIT;

	CvCivilizationInfo& kCiv = GET_PLAYER(getOwner()).getCivilizationInfo();

	// Determine what we're going to upgrade into
	for(int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(iI);
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
		if(pkUnitClassInfo)
		{
			if(m_pUnitInfo->GetUpgradeUnitClass(iI))
			{
				eUpgradeUnitType = (UnitTypes) kCiv.getCivilizationUnits(iI);

#if defined(MOD_EVENTS_UNIT_UPGRADES)
				if (MOD_EVENTS_UNIT_UPGRADES) {
					if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_CanHaveUpgrade, getOwner(), GetID(), iI, eUpgradeUnitType) == GAMEEVENTRETURN_FALSE) {
						continue;
					}

					if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_UnitCanHaveUpgrade, getOwner(), GetID(), iI, eUpgradeUnitType) == GAMEEVENTRETURN_FALSE) {
						continue;
					}
				} else {
#endif
				ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
				if (pkScriptSystem) 
				{
					CvLuaArgsHandle args;
					args->Push(((int)getOwner()));
					args->Push(GetID());
					args->Push(iI);
					args->Push(eUpgradeUnitType);

					bool bResult = false;
					if (LuaSupport::CallTestAll(pkScriptSystem, "CanHaveUpgrade", args.get(), bResult)) 
					{
						if (bResult == false) 
						{
							continue;
						}
					}
				}
#if defined(MOD_EVENTS_UNIT_UPGRADES)
				}
#endif

				break;
			}
		}
	}

	return eUpgradeUnitType;
}

//	--------------------------------------------------------------------------------
/// How much does it cost to upgrade this Unit to a shiny new eUnit?
int CvUnit::upgradePrice(UnitTypes eUnit) const
{
	VALIDATE_OBJECT
	int iPrice = 0;
	
	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	if(pkUnitInfo == NULL)
	{
		return 0;
	}

	if(isBarbarian())
	{
		return 0;
	}

	iPrice = /*10*/ GC.getBASE_UNIT_UPGRADE_COST();

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	iPrice += (std::max(0, (kPlayer.getProductionNeeded(eUnit) - kPlayer.getProductionNeeded(getUnitType()))) * /*2*/ GC.getUNIT_UPGRADE_COST_PER_PRODUCTION());

	// Upgrades for later units are more expensive
	const TechTypes eTech = (TechTypes) pkUnitInfo->GetPrereqAndTech();
	CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
	if(pkTechInfo)
	{
		const EraTypes eUpgradeEra = (EraTypes) pkTechInfo->GetEra();

		double fMultiplier = 1.0f;
		fMultiplier += (eUpgradeEra* /*0.3*/ GC.getUNIT_UPGRADE_COST_MULTIPLIER_PER_ERA());

		iPrice = int(iPrice * fMultiplier);
	}

	if(!isHuman() && !kPlayer.IsAITeammateOfHuman() && !isBarbarian())
	{
		iPrice *= GC.getGame().getHandicapInfo().getAIUnitUpgradePercent();
		iPrice /= 100;

		iPrice *= std::max(0, ((GC.getGame().getHandicapInfo().getAIPerEraModifier() * GET_TEAM(getTeam()).GetCurrentEra()) + 100));
		iPrice /= 100;
	}

	// Discount
	iPrice -= (iPrice * getUpgradeDiscount()) / 100;

	// Mod (Policies, etc.)
	int iMod = kPlayer.GetUnitUpgradeCostMod();
	iPrice *= (100 + iMod);
	iPrice /= 100;

	// Apply exponent
	iPrice = (int) pow((double) iPrice, (double) /*1.0f*/ GC.getUNIT_UPGRADE_COST_EXPONENT());

	// Make the number not be funky
	int iDivisor = /*5*/ GC.getUNIT_UPGRADE_COST_VISIBLE_DIVISOR();
	iPrice /= iDivisor;
	iPrice *= iDivisor;
#if defined(MOD_BALANCE_CORE)
	CvCity* pCity = GET_PLAYER(getOwner()).getCapitalCity();
	if(pCity != NULL)
	{
		int iMaxPrice = pCity->GetPurchaseCost(eUnit);
		if(iMaxPrice > 0)
		{
			if(iPrice > iMaxPrice)
			{
				iPrice = iMaxPrice;
			}
		}
	}
#endif

	return iPrice;
}

//	--------------------------------------------------------------------------------
/// Upgrades this Unit - returns a pointer to the newly created unit
#if defined(MOD_API_EXTENSIONS)
CvUnit* CvUnit::DoUpgrade(bool bFree)
#else
CvUnit* CvUnit::DoUpgrade()
#endif
{
	VALIDATE_OBJECT

	UnitTypes eUnitType = GetUpgradeUnitType();

#if defined(MOD_API_EXTENSIONS)
	return DoUpgradeTo(eUnitType, bFree);
}

CvUnit* CvUnit::DoUpgradeTo(UnitTypes eUnitType, bool bFree)
{
#endif
	// Gold Cost
	int iUpgradeCost = upgradePrice(eUnitType);
	CvPlayerAI& thisPlayer = GET_PLAYER(getOwner());
#if defined(MOD_API_EXTENSIONS)
	if (!bFree) {
#endif
	thisPlayer.GetTreasury()->LogExpenditure(getUnitInfo().GetText(), iUpgradeCost, 3);
	thisPlayer.GetTreasury()->ChangeGold(-iUpgradeCost);
#if defined(MOD_API_EXTENSIONS)
	}
#endif

	// Add newly upgraded Unit & kill the old one
	CvUnit* pNewUnit = thisPlayer.initUnit(eUnitType, getX(), getY(), NO_UNITAI, NO_DIRECTION, false, false);

	if(NULL != pNewUnit)
	{
		if(GC.getGame().getActivePlayer() == getOwner())
		{
			auto_ptr<ICvUnit1> pDllNewUnit = GC.WrapUnitPointer(pNewUnit);
			DLLUI->selectUnit(pDllNewUnit.get(), true, false, false);
		}

#if defined(MOD_EVENTS_UNIT_UPGRADES)
		// MUST call the event before convert() as that kills the old unit
		if (MOD_EVENTS_UNIT_UPGRADES) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_UnitUpgraded, getOwner(), GetID(), pNewUnit->GetID(), false);
		} else {
#endif
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem) 
		{
			CvLuaArgsHandle args;
			args->Push(((int)getOwner()));
			args->Push(GetID());
			args->Push(pNewUnit->GetID());
			args->Push(false); // bGoodyHut

			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "UnitUpgraded", args.get(), bResult);
		}
#if defined(MOD_EVENTS_UNIT_UPGRADES)
		}
#endif
		pNewUnit->convert(this, true);
		pNewUnit->setupGraphical();
		
		// Can't move after upgrading
#if defined(MOD_GLOBAL_MOVE_AFTER_UPGRADE)
		if (MOD_GLOBAL_MOVE_AFTER_UPGRADE && pNewUnit->getUnitInfo().CanMoveAfterUpgrade()) {
			pNewUnit->setSetUpForRangedAttack(isSetUpForRangedAttack());
			pNewUnit->m_iAttacksMade = m_iAttacksMade;
			pNewUnit->m_iMadeInterceptionCount = m_iMadeInterceptionCount;
			pNewUnit->m_eActivityType = m_eActivityType;
			if (IsFortifiedThisTurn()) {
				pNewUnit->SetFortifiedThisTurn(true);
			} else {
				pNewUnit->setFortifyTurns(getFortifyTurns());
			}
		} else 
#endif
		pNewUnit->finishMoves();

		kill(true);
	}

#if !defined(NO_ACHIEVEMENTS)
	if(isHuman() && !GC.getGame().isGameMultiPlayer() && GET_PLAYER(GC.getGame().getActivePlayer()).isLocalPlayer())
	{
		gDLL->UnlockAchievement(ACHIEVEMENT_UNIT_UPGRADE);
	}
#endif

	return pNewUnit;
}


//	--------------------------------------------------------------------------------
HandicapTypes CvUnit::getHandicapType() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).getHandicapType();
}


//	--------------------------------------------------------------------------------
CvCivilizationInfo& CvUnit::getCivilizationInfo() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).getCivilizationInfo();
}

//	--------------------------------------------------------------------------------
CivilizationTypes CvUnit::getCivilizationType() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).getCivilizationType();
}

//	--------------------------------------------------------------------------------
const char* CvUnit::getVisualCivAdjective(TeamTypes eForTeam) const
{
	VALIDATE_OBJECT
	if(getVisualOwner(eForTeam) == getOwner())
	{
		return GET_PLAYER(getOwner()).getCivilizationInfo().getAdjectiveKey();
	}

	return "";
}

//	--------------------------------------------------------------------------------
SpecialUnitTypes CvUnit::getSpecialUnitType() const
{
	VALIDATE_OBJECT
	return ((SpecialUnitTypes)(m_pUnitInfo->GetSpecialUnitType()));
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsGreatPerson() const
{
	SpecialUnitTypes eSpecialUnitGreatPerson = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_PEOPLE");

	return (getSpecialUnitType() == eSpecialUnitGreatPerson);
}

//	--------------------------------------------------------------------------------
UnitTypes CvUnit::getCaptureUnitType(CivilizationTypes eCivilization) const
{
	VALIDATE_OBJECT
	CvAssert(eCivilization != NO_CIVILIZATION);
	CvCivilizationInfo* pkCivilizationInfo = GC.getCivilizationInfo(eCivilization);
	if(pkCivilizationInfo == NULL)
	{
		return NO_UNIT;
	}
	
#if defined(MOD_EVENTS_UNIT_CAPTURE)
	if (MOD_EVENTS_UNIT_CAPTURE) {
		int iValue = 0;
		if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_UnitCaptureType, getOwner(), GetID(), getUnitType(), eCivilization) == GAMEEVENTRETURN_VALUE) {
			// Defend against modder stupidity!
			if (iValue >= NO_UNIT && (iValue == NO_UNIT || GC.getUnitInfo((UnitTypes) iValue) != NULL)) {
				return (UnitTypes) iValue;
			}
		}
	}
#endif

	return ((m_pUnitInfo->GetUnitCaptureClassType() == NO_UNITCLASS) ? NO_UNIT : (UnitTypes)pkCivilizationInfo->getCivilizationUnits(getUnitInfo().GetUnitCaptureClassType()));
}


//	--------------------------------------------------------------------------------
UnitCombatTypes CvUnit::getUnitCombatType() const
{
	VALIDATE_OBJECT
	return ((UnitCombatTypes)(m_pUnitInfo->GetUnitCombatType()));
}

#if defined(MOD_GLOBAL_PROMOTION_CLASSES)
//	--------------------------------------------------------------------------------
UnitCombatTypes CvUnit::getUnitPromotionType() const
{
	VALIDATE_OBJECT
	return ((UnitCombatTypes)(m_pUnitInfo->GetUnitPromotionType()));
}
#endif

bool CvUnit::isNativeDomain(const CvPlot* pPlot) const
{
	switch (getDomainType())
	{
	case DOMAIN_LAND:
		return !pPlot->isWater();
		break;
	case DOMAIN_AIR:
		return true;
		break;
	case DOMAIN_SEA:
		return pPlot->isWater();
		break;
	case DOMAIN_HOVER:
		return true;
		break;
	case DOMAIN_IMMOBILE:
		return false;
		break;
	}

	return true;
}

bool CvUnit::isMatchingDomain(const CvPlot* pPlot) const
{
	switch (getDomainType())
	{
	case DOMAIN_LAND:
		return pPlot->needsEmbarkation(this)==isEmbarked();
		break;
	case DOMAIN_AIR:
		return true;
		break;
	case DOMAIN_SEA:
		return pPlot->isWater();
		break;
	case DOMAIN_HOVER:
		return true;
		break;
	case DOMAIN_IMMOBILE:
		return false;
		break;
	}

	return true;
}

//	---------------------------------------------------------------------------
DomainTypes CvUnit::getDomainType() const
{
	VALIDATE_OBJECT
	return (DomainTypes) m_pUnitInfo->GetDomainType();
}

//	---------------------------------------------------------------------------
int CvUnit::flavorValue(FlavorTypes eFlavor) const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->GetFlavorValue(eFlavor);
}


//	--------------------------------------------------------------------------------
bool CvUnit::isBarbarian() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).isBarbarian();
}


//	--------------------------------------------------------------------------------
bool CvUnit::isHuman() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).isHuman();
}


//	--------------------------------------------------------------------------------
/// Is this a Barbarian Unit threatening a nearby Minor?
void CvUnit::DoTestBarbarianThreatToMinorsWithThisUnitsDeath(PlayerTypes eKillingPlayer)
{
	VALIDATE_OBJECT

	// Need valid player
	if(eKillingPlayer == NO_PLAYER)
		return;

	// No minors
	if(GET_PLAYER(eKillingPlayer).isMinorCiv())
		return;

	PlayerTypes eMinor;
	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		eMinor = (PlayerTypes) iMinorLoop;

		if(GET_PLAYER(eMinor).isAlive())
		{
			if(IsBarbarianUnitThreateningMinor(eMinor))
				GET_PLAYER(eMinor).GetMinorCivAI()->DoThreateningBarbKilled(eKillingPlayer, getX(), getY());
		}
	}
}

//	--------------------------------------------------------------------------------
/// Is this a Barbarian Unit threatening a nearby Minor?
bool CvUnit::IsBarbarianUnitThreateningMinor(PlayerTypes eMinor)
{
	VALIDATE_OBJECT

	// Must be a barb unit
	if(!isBarbarian())
		return false;

	// Plot owned by this minor?
	if(plot()->getOwner() == eMinor)
		return true;

	// Look at adjacent plots
	CvPlot* pLoopPlot;
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(getX(), getY(), (DirectionTypes) iI);

		if(pLoopPlot != NULL)
		{
			if(pLoopPlot->getOwner() == eMinor)
				return true;
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
int CvUnit::visibilityRange() const
{
	VALIDATE_OBJECT

	int iRtnValue;

	if(isEmbarked())
	{
		iRtnValue = GC.getEMBARKED_VISIBILITY_RANGE() + m_iEmbarkExtraVisibility;
	}

	else
	{
		iRtnValue = m_pUnitInfo->GetBaseSightRange() + m_iExtraVisibilityRange;
	}

	return iRtnValue;
}

#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
//	--------------------------------------------------------------------------------
int CvUnit::reconRange() const
{
	VALIDATE_OBJECT

	int iRtnValue = GC.getRECON_VISIBILITY_RANGE();

	if(MOD_PROMOTIONS_VARIABLE_RECON)
	{
		iRtnValue += m_iExtraReconRange;
	}

	return iRtnValue;
}
#endif

//	---------------------------------------------------------------------------
// Get the base movement points for the unit.
// Parameters:
//		eIntoDomain	- If NO_DOMAIN, this will use the units current domain.
//					  This can give different results based on whether the unit is currently embarked or not.
//					  Passing in DOMAIN_SEA will return the units baseMoves as if it were already embarked.
//					  Passing in DOMAIN_LAND will return the units baseMoves as if it were on land, even if it is currently embarked.
int CvUnit::baseMoves(DomainTypes eIntoDomain /* = NO_DOMAIN */) const
{
	VALIDATE_OBJECT
	CvTeam& thisTeam = GET_TEAM(getTeam());
	CvPlayer& thisPlayer = GET_PLAYER(getOwner());
	CvPlayerTraits* pTraits = thisPlayer.GetPlayerTraits();
	DomainTypes eDomain = getDomainType();

	//hover units don't embark but movement speed may change!
	bool bWantEmbarkedMovement = (eIntoDomain == DOMAIN_SEA && (CanEverEmbark() || IsHoveringUnit()) ) || (eIntoDomain == NO_DOMAIN && isEmbarked());
	if(bWantEmbarkedMovement)
	{
		CvPlayerPolicies* pPolicies = thisPlayer.GetPlayerPolicies();
		return GC.getEMBARKED_UNIT_MOVEMENT() + getExtraNavalMoves() + thisTeam.getEmbarkedExtraMoves() + thisTeam.getExtraMoves(eDomain) + pTraits->GetExtraEmbarkMoves() + pPolicies->GetNumericModifier(POLICYMOD_EMBARKED_EXTRA_MOVES);
	}

	int m_iExtraNavalMoves = 0;
	if(eDomain == DOMAIN_SEA)
	{
		m_iExtraNavalMoves += getExtraNavalMoves();
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
		if(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
		{
			const std::vector<ResourceTypes>& vStrategicMonopolies = GET_PLAYER(getOwner()).GetStrategicMonopolies();
			for (size_t iResourceLoop = 0; iResourceLoop < vStrategicMonopolies.size(); iResourceLoop++)
			{
				ResourceTypes eResourceLoop = vStrategicMonopolies[iResourceLoop];
				CvResourceInfo* pInfo = GC.getResourceInfo(eResourceLoop);
				if (pInfo && pInfo->getMonopolyMovementBonus() > 0)
				{
					m_iExtraNavalMoves += pInfo->getMonopolyMovementBonus();
				}
			}
		}
#endif
		// Work boats also get extra moves, and they don't have a combat class to receive a promotion from
		if(m_iBaseCombat == 0)
		{
			m_iExtraNavalMoves += pTraits->GetExtraEmbarkMoves();
		}
	}

	int iExtraGoldenAgeMoves = 0;
	if(thisPlayer.isGoldenAge())
	{
		iExtraGoldenAgeMoves = pTraits->GetGoldenAgeMoveChange();
	}

#if defined(MOD_BALANCE_CORE_POLICIES)
	if(!IsCombatUnit() && !IsGreatGeneral() && !IsGreatAdmiral())
	{
		iExtraGoldenAgeMoves += GET_PLAYER(getOwner()).GetExtraMoves();
	}
#endif

	int iExtraUnitCombatTypeMoves = pTraits->GetMovesChangeUnitCombat((UnitCombatTypes)(m_pUnitInfo->GetUnitCombatType()));

#if defined(MOD_BALANCE_CORE)
	iExtraUnitCombatTypeMoves += pTraits->GetMovesChangeUnitClass((UnitClassTypes)(m_pUnitInfo->GetUnitClassType()));
#endif

	return (m_pUnitInfo->GetMoves() + getExtraMoves() + thisTeam.getExtraMoves(eDomain) + m_iExtraNavalMoves + iExtraGoldenAgeMoves + iExtraUnitCombatTypeMoves);
}


//	---------------------------------------------------------------------------
int CvUnit::maxMoves() const
{
	VALIDATE_OBJECT
#if defined(MOD_PROMOTIONS_FLAGSHIP)
	if(IsGreatGeneral() || (MOD_PROMOTIONS_FLAGSHIP && IsGreatAdmiral()))
#else
	if(IsGreatGeneral())
#endif
	{
		return GetGreatGeneralStackMovement();
	}
	else
	{
		return (baseMoves() * GC.getMOVE_DENOMINATOR());	// WARNING: Uses the current embark state of the unit!
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::movesLeft() const
{
	VALIDATE_OBJECT
	return std::max(0, getMoves());
}


//	--------------------------------------------------------------------------------
bool CvUnit::canMove() const
{
	VALIDATE_OBJECT
	return (getMoves() > 0);
}


//	--------------------------------------------------------------------------------
bool CvUnit::hasMoved()	const
{
	VALIDATE_OBJECT
	return (getMoves() < maxMoves());
}


//	--------------------------------------------------------------------------------
int CvUnit::GetRange() const
{
	VALIDATE_OBJECT

	return (m_pUnitInfo->GetRange() + m_iExtraRange);
}

//	--------------------------------------------------------------------------------
int CvUnit::GetNukeDamageLevel() const
{
	VALIDATE_OBJECT
	return getUnitInfo().GetNukeDamageLevel();
}

//	--------------------------------------------------------------------------------
bool CvUnit::canBuildRoute() const
{
	VALIDATE_OBJECT

	if(m_bEmbarked)
	{
		return false;
	}

	CvTeam& thisTeam = GET_TEAM(getTeam());

	CvTeamTechs* pTeamTechs = thisTeam.GetTeamTechs();

	int iNumBuildInfos = GC.getNumBuildInfos();
	for(int iI = 0; iI < iNumBuildInfos; iI++)
	{
		CvBuildInfo* thisBuildInfo = GC.getBuildInfo((BuildTypes)iI);
#if defined(MOD_GLOBAL_ALPINE_PASSES)
		// Don't count routes that come as part of an improvement
		if(NULL != thisBuildInfo && thisBuildInfo->getImprovement() == NO_IMPROVEMENT && thisBuildInfo->getRoute() != NO_ROUTE)
#else
		if(NULL != thisBuildInfo && thisBuildInfo->getRoute() != NO_ROUTE)
#endif
		{
			if(m_pUnitInfo->GetBuilds(iI))
			{
				if(pTeamTechs->HasTech((TechTypes)(thisBuildInfo->getTechPrereq())))
				{
					return true;
				}
#if defined(MOD_BALANCE_CORE)
				if(thisBuildInfo->getTechObsolete() != NO_TECH && !pTeamTechs->HasTech((TechTypes)(thisBuildInfo->getTechObsolete())))
				{
					return true;
				}
#endif
			}
		}
	}

	return false;
}

//	----------------------------------------------------------------------------
BuildTypes CvUnit::getBuildType() const
{
	VALIDATE_OBJECT
	const MissionQueueNode* pkMissionNode = HeadMissionQueueNode();
	if(pkMissionNode != NULL)
	{
		if(pkMissionNode->eMissionType == CvTypes::getMISSION_ROUTE_TO())
		{
			RouteTypes eBestRoute = GET_PLAYER(m_eOwner).getBestRoute(plot());
			if(eBestRoute != NO_ROUTE)
			{
				for(int iI = 0; iI < GC.getNumBuildInfos(); iI++)
				{
					BuildTypes eBuild = (BuildTypes)iI;
					CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);

					if(pkBuildInfo)
					{
						RouteTypes eRoute = ((RouteTypes)(pkBuildInfo->getRoute()));
						if(eRoute == eBestRoute)
						{
							return eBuild;
						}
					}
				}
			}
		}
		else if(pkMissionNode->eMissionType == CvTypes::getMISSION_BUILD())
		{
			return (BuildTypes)pkMissionNode->iData1;
		}
	}

	return NO_BUILD;
}


//	--------------------------------------------------------------------------------
int CvUnit::workRate(bool bMax, BuildTypes /*eBuild*/) const
{
	VALIDATE_OBJECT
	int iRate;

	if(!bMax)
	{
		if(!canMove())
		{
			return 0;
		}
	}

	iRate = m_pUnitInfo->GetWorkRate();

#if defined(MOD_BALANCE_CORE)
	bool bCanWork = false;
	if(iRate <= 0)
	{
		for(int iI = 0; iI < GC.getNumBuildInfos(); iI++)
		{
			const BuildTypes eBuild = static_cast<BuildTypes>(iI);
			CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
			if(pkBuildInfo)
			{
				if(GET_PLAYER(getOwner()).GetPlayerTraits()->HasUnitClassCanBuild(eBuild, getUnitClassType()))
				{
					bCanWork = true;
					break;
				}
			}
		}
	}
	if(bCanWork)
	{
		iRate = 100;
	}
#endif

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	iRate *= std::max(0, (kPlayer.getWorkerSpeedModifier() + kPlayer.GetPlayerTraits()->GetWorkerSpeedModifier() + 100));
	iRate /= 100;

	if(!kPlayer.isHuman() && !kPlayer.IsAITeammateOfHuman() && !kPlayer.isBarbarian())
	{
		iRate *= std::max(0, (GC.getGame().getHandicapInfo().getAIWorkRateModifier() + 100));
		iRate /= 100;
	}

	return iRate;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isNoBadGoodies() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->IsNoBadGoodies();
}

//	--------------------------------------------------------------------------------
int CvUnit::getRivalTerritoryCount() const
{
	VALIDATE_OBJECT
	return m_iRivalTerritoryCount;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isRivalTerritory() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->IsRivalTerritory() || getRivalTerritoryCount() > 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeRivalTerritoryCount(int iChange)
{
	VALIDATE_OBJECT
	m_iRivalTerritoryCount = (m_iRivalTerritoryCount + iChange);
	CvAssert(getRivalTerritoryCount() >= 0);
}

//	--------------------------------------------------------------------------------
bool CvUnit::isFound() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->IsFound();
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsFoundAbroad() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->IsFoundAbroad();
}

#if defined(MOD_BALANCE_CORE_SETTLER_ADVANCED)
//	--------------------------------------------------------------------------------
bool CvUnit::IsFoundMid() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->IsFoundMid();
}
//	--------------------------------------------------------------------------------
bool CvUnit::IsFoundLate() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->IsFoundLate();
}
#endif

//	--------------------------------------------------------------------------------
bool CvUnit::IsWork() const
{
	VALIDATE_OBJECT
	return (m_pUnitInfo->GetWorkRate() > 0);
}

//	--------------------------------------------------------------------------------
bool CvUnit::isGoldenAge() const
{
	VALIDATE_OBJECT
	if(isDelayedDeath())
	{
		return false;
	}

	return m_pUnitInfo->GetGoldenAgeTurns() > 0;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isGivesPolicies() const
{
	VALIDATE_OBJECT
		if(isDelayedDeath())
		{
			return false;
		}

		return m_pUnitInfo->GetFreePolicies() > 0;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isBlastTourism() const
{
	VALIDATE_OBJECT
		if(isDelayedDeath())
		{
			return false;
		}

		return m_pUnitInfo->GetOneShotTourism() > 0;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canCoexistWithEnemyUnit(TeamTypes eTeam) const
{
	VALIDATE_OBJECT
	if(NO_TEAM == eTeam)
	{
		return false;
	}

	if(isInvisible(eTeam, false))
	{
		return true;
	}

	return false;
}


//	--------------------------------------------------------------------------------
int CvUnit::getMustSetUpToRangedAttackCount() const
{
	VALIDATE_OBJECT
	return m_iMustSetUpToRangedAttackCount;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isMustSetUpToRangedAttack() const
{
	VALIDATE_OBJECT
	return getMustSetUpToRangedAttackCount() > 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeMustSetUpToRangedAttackCount(int iChange)
{
	VALIDATE_OBJECT
	m_iMustSetUpToRangedAttackCount = (m_iMustSetUpToRangedAttackCount + iChange);
	CvAssert(getMustSetUpToRangedAttackCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getRangedSupportFireCount() const
{
	VALIDATE_OBJECT
	return m_iRangedSupportFireCount;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isRangedSupportFire() const
{
	VALIDATE_OBJECT
	return getRangedSupportFireCount() > 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeRangedSupportFireCount(int iChange)
{
	VALIDATE_OBJECT
	m_iRangedSupportFireCount = (m_iRangedSupportFireCount + iChange);
	CvAssert(getRangedSupportFireCount() >= 0);
}


//	--------------------------------------------------------------------------------
bool CvUnit::isFighting() const
{
	VALIDATE_OBJECT
	return (getCombatUnit() != NULL || getCombatCity() != NULL);
}


//	--------------------------------------------------------------------------------
bool CvUnit::isAttacking() const
{
	VALIDATE_OBJECT
	return (getAttackPlot() != NULL && !isDelayedDeath());
}


//	--------------------------------------------------------------------------------
bool CvUnit::isDefending() const
{
	VALIDATE_OBJECT
	return (isFighting() && !isAttacking());
}


//	--------------------------------------------------------------------------------
bool CvUnit::isInCombat() const
{
	VALIDATE_OBJECT
	return (isFighting() || isAttacking());
}


//	--------------------------------------------------------------------------------
int CvUnit::GetMaxHitPoints() const
{
	VALIDATE_OBJECT
	return GC.getMAX_HIT_POINTS();
}


//	--------------------------------------------------------------------------------
int CvUnit::GetCurrHitPoints()	const
{
	VALIDATE_OBJECT
	return (GetMaxHitPoints() - getDamage());
}


//	--------------------------------------------------------------------------------
bool CvUnit::IsHurt() const
{
	VALIDATE_OBJECT
	return (getDamage() > 0);
}


//	--------------------------------------------------------------------------------
bool CvUnit::IsDead() const
{
	VALIDATE_OBJECT
	return (getDamage() >= GetMaxHitPoints());
}

//	--------------------------------------------------------------------------------
/// Over strategic resource limit?
int CvUnit::GetStrategicResourceCombatPenalty() const
{
	int iPenalty = 0;

	// barbs don't have resources
	if(isBarbarian())
		return iPenalty;

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	// Loop through all resources
	ResourceTypes eResource;
	int iNumResourceInfos = GC.getNumResourceInfos();
	for(int iResourceLoop = 0; iResourceLoop < iNumResourceInfos; iResourceLoop++)
	{
		eResource = (ResourceTypes) iResourceLoop;

		int iAvailable = kPlayer.getNumResourceAvailable(eResource);
		if (kPlayer.isMinorCiv())
		{
			iAvailable += kPlayer.getResourceExport(eResource);
		}

		// Over resource limit?
		if(iAvailable < 0)
		{
			if(m_pUnitInfo->GetResourceQuantityRequirement(eResource) > 0)
			{
				int iUsed = kPlayer.getNumResourceUsed(eResource);
				int iMissing = iUsed - kPlayer.getNumResourceTotal(eResource);

				CvAssertMsg(iUsed > 0, "Number of used resources is zero or negative, this is unexpected. Please send Anton your save file and version.");
				if (iUsed <= 0)
					continue;
				CvAssertMsg(iMissing > 0, "Number of missing resources is zero or negative, this is unexpected. Please send Anton your save file and version.");
				if (iMissing <= 0)
					continue;

				double dDeficit = (double) iMissing / (double) iUsed;
				iPenalty += (int) floor((dDeficit) * GC.getSTRATEGIC_RESOURCE_EXHAUSTED_PENALTY()); // round down (for larger negative penalty)
			}
		}
	}

	iPenalty = max(iPenalty, GC.getSTRATEGIC_RESOURCE_EXHAUSTED_PENALTY());
	return iPenalty;
}

//	--------------------------------------------------------------------------------
/// Unhappy?
int CvUnit::GetUnhappinessCombatPenalty() const
{
	CvPlayer &kPlayer = GET_PLAYER(getOwner());
	int iPenalty = 0;

#if defined(MOD_GLOBAL_CS_RAZE_RARELY)
	if((!kPlayer.isMinorCiv() && kPlayer.IsEmpireUnhappy()) || (kPlayer.isMinorCiv() && kPlayer.IsEmpireVeryUnhappy()))
#else
	if(kPlayer.IsEmpireUnhappy())
#endif
	{
		iPenalty = (-1 * kPlayer.GetExcessHappiness()) * GC.getVERY_UNHAPPY_COMBAT_PENALTY_PER_UNHAPPY();
		iPenalty = max(iPenalty, GC.getVERY_UNHAPPY_MAX_COMBAT_PENALTY());
	}

	return iPenalty;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetBaseCombatStrength(int iCombat)
{
	VALIDATE_OBJECT
	m_iBaseCombat = iCombat;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetBaseCombatStrength(bool bIgnoreEmbarked) const
{
	VALIDATE_OBJECT
	if(m_bEmbarked && !bIgnoreEmbarked)
	{
		return GetEmbarkedUnitDefense() / 100;
	}

	return m_iBaseCombat;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetBaseCombatStrengthConsideringDamage() const
{
	int iWoundedDamageMultiplier = /*50*/ GC.getWOUNDED_DAMAGE_MULTIPLIER();

	// Mod (Policies, etc.) - lower means Units are less bothered by damage
	iWoundedDamageMultiplier += GET_PLAYER(getOwner()).GetWoundedUnitDamageMod();

	int iStrength = GetMaxAttackStrength(NULL,NULL,NULL) / 100;

	// How much does damage weaken the effectiveness of the Unit?
	int iDamageMod = m_iDamage * iWoundedDamageMultiplier / 100;

	// Reduce strength points based on damage mod calculated above. Example: 4 damage will normally reduce a 20 strength Unit by 2/10ths, or 1/5, or 4, making it effectively a 16
	iStrength -= (iStrength * iDamageMod / GC.getMAX_HIT_POINTS());
	return iStrength;
}

//	--------------------------------------------------------------------------------
/// What are the generic strength modifiers for this Unit?
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
int CvUnit::GetGenericMaxStrengthModifier(const CvUnit* pOtherUnit, const CvPlot* pBattlePlot, bool bIgnoreUnitAdjacency, const CvPlot* pFromPlot) const
#else
int CvUnit::GetGenericMaxStrengthModifier(const CvUnit* pOtherUnit, const CvPlot* pBattlePlot, bool bIgnoreUnitAdjacency) const
#endif
{
	VALIDATE_OBJECT

	int iModifier = 0;
	int iTempModifier;

	// Generic combat bonus
	iTempModifier = getExtraCombatPercent();
	iModifier += iTempModifier;

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	ReligionTypes eFoundedReligion = pReligions->GetFounderBenefitsReligion(kPlayer.GetID());

	// If the empire is unhappy, then Units get a combat penalty
#if defined(MOD_GLOBAL_CS_RAZE_RARELY)
	if((!kPlayer.isMinorCiv() && kPlayer.IsEmpireUnhappy()) || (kPlayer.isMinorCiv() && kPlayer.IsEmpireVeryUnhappy()))
#else
	if(kPlayer.IsEmpireUnhappy())
#endif
	{
		iModifier += GetUnhappinessCombatPenalty();
	}

#if defined(MOD_BALANCE_CORE_DIPLOMACY_ADVANCED)
	if(MOD_BALANCE_CORE_DIPLOMACY_ADVANCED)
	{
		// units cannot heal anymore, but strength is unaffected
	}
	else
	{
#endif
	// Over our strategic resource limit?
	iTempModifier = GetStrategicResourceCombatPenalty();
	if(iTempModifier != 0)
		iModifier += iTempModifier;
#if defined(MOD_BALANCE_CORE_DIPLOMACY_ADVANCED)
	}
#endif

#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	if (pFromPlot == NULL)
	{
		pFromPlot = plot();
	}

	// Great General nearby
	if (IsNearGreatGeneral(pFromPlot) && !IsIgnoreGreatGeneralBenefit())
#else
	// Great General nearby
	if(IsNearGreatGeneral() && !IsIgnoreGreatGeneralBenefit())
#endif
	{
		iModifier += kPlayer.GetGreatGeneralCombatBonus();
		iModifier += kPlayer.GetPlayerTraits()->GetGreatGeneralExtraBonus();

#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
		if (IsStackedGreatGeneral(pFromPlot))
#else
		if(IsStackedGreatGeneral())
#endif
		{
			iModifier += GetGreatGeneralCombatModifier();
		}
	}
#if defined(MOD_BALANCE_CORE)
	int iCSStrengthMod = 0;
	if(GET_PLAYER(getOwner()).isMinorCiv())
	{
		PlayerTypes eAlly = GET_PLAYER(getOwner()).GetMinorCivAI()->GetAlly();
		if(eAlly != NO_PLAYER)
		{
			int iCSBonus = GET_PLAYER(eAlly).GetPlayerTraits()->GetAllianceCSStrength();
			if(iCSBonus > 0)
			{
				int iNumAllies = 0;
				// Loop through all minors and get the total number we've met.
				for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
				{
					PlayerTypes eMinor = (PlayerTypes) iPlayerLoop;

					if (GET_PLAYER(eMinor).isAlive() && GET_PLAYER(eMinor).isMinorCiv())
					{
						if (GET_PLAYER(eMinor).GetMinorCivAI()->IsAllies(eAlly))
						{
							iNumAllies++;
							if(iNumAllies >= GC.getBALANCE_MAX_CS_ALLY_STRENGTH())
							{
								break;
							}
						}
					}
				}
				if(iNumAllies > GC.getBALANCE_MAX_CS_ALLY_STRENGTH())
				{
					iNumAllies = GC.getBALANCE_MAX_CS_ALLY_STRENGTH();
				}
				iCSStrengthMod = (iCSBonus * iNumAllies);
			}
		}
	}
	else
	{
		int iCSBonus = GET_PLAYER(getOwner()).GetPlayerTraits()->GetAllianceCSStrength();
		if(iCSBonus > 0)
		{
			int iNumAllies = 0;
			// Loop through all minors and get the total number we've met.
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
			{
				PlayerTypes eMinor = (PlayerTypes) iPlayerLoop;

				if (GET_PLAYER(eMinor).isAlive() && GET_PLAYER(eMinor).isMinorCiv())
				{
					if (GET_PLAYER(eMinor).GetMinorCivAI()->IsAllies(getOwner()))
					{
						iNumAllies++;
						if(iNumAllies >= GC.getBALANCE_MAX_CS_ALLY_STRENGTH())
						{
							break;
						}
					}
				}
			}
			if(iNumAllies > GC.getBALANCE_MAX_CS_ALLY_STRENGTH())
			{
				iNumAllies = GC.getBALANCE_MAX_CS_ALLY_STRENGTH();
			}
			iCSStrengthMod = (iCSBonus * iNumAllies);
		}
	}
	iModifier += iCSStrengthMod;
#endif

	// Reverse Great General nearby
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	int iReverseGGModifier = GetReverseGreatGeneralModifier(pFromPlot);
#else
	int iReverseGGModifier = GetReverseGreatGeneralModifier();
#endif
	if(iReverseGGModifier != 0)
	{
		iModifier += iReverseGGModifier;
	}

	// Improvement with combat bonus (from trait) nearby
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	int iNearbyImprovementModifier = GetNearbyImprovementModifier(pFromPlot);
#else
	int iNearbyImprovementModifier = GetNearbyImprovementModifier();
#endif
	if(iNearbyImprovementModifier != 0)
	{
		iModifier += iNearbyImprovementModifier;
	}

	// Adjacent Friendly military Unit?
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	if (pFromPlot->IsFriendlyUnitAdjacent(getTeam(), /*bCombatUnit*/ true))
#else
	if(IsFriendlyUnitAdjacent(/*bCombatUnit*/ true))
#endif
		iModifier += GetAdjacentModifier();

	// Our empire fights well in Golden Ages?
	if(kPlayer.isGoldenAge())
		iModifier += kPlayer.GetPlayerTraits()->GetGoldenAgeCombatModifier();

#if defined(MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED)
	//Domination Victory -- If a player owns more than one capital, your troops fight a % better as a result (% = % of global capitals owned).
	if(MOD_BALANCE_CORE_MILITARY_RESISTANCE && pOtherUnit != NULL)
	{
		iModifier += GetResistancePower(pOtherUnit);
	}			
#endif
	////////////////////////
	// KNOWN BATTLE PLOT
	////////////////////////

	if(pBattlePlot != NULL)
	{
		// Bonuses for fighting in one's lands
		if(pBattlePlot->IsFriendlyTerritory(getOwner()))
		{
			iTempModifier = getFriendlyLandsModifier();
			iModifier += iTempModifier;

			// Founder Belief bonus
			CvCity* pPlotCity = pBattlePlot->getWorkingCity();
			if(pPlotCity)
			{
				ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
				if(eReligion != NO_RELIGION && eReligion == eFoundedReligion)
				{
					const CvReligion* pCityReligion = pReligions->GetReligion(eReligion, pPlotCity->getOwner());
					if(pCityReligion)
					{
						iTempModifier = pCityReligion->m_Beliefs.GetCombatModifierFriendlyCities();
						iModifier += iTempModifier;
					}
				}
			}
		}

#if defined(MOD_BALANCE_CORE_BELIEFS)
		if(MOD_BALANCE_CORE_BELIEFS && pOtherUnit != NULL)
		{
			if(!pOtherUnit->isBarbarian() && !GET_PLAYER(pOtherUnit->getOwner()).isMinorCiv() && pOtherUnit->getOwner() != NO_PLAYER)
			{
				if(GET_PLAYER(pOtherUnit->getOwner()).GetReligions()->GetReligionInMostCities() != NO_RELIGION)
				{
					if(eFoundedReligion != NO_RELIGION)
					{
						const CvReligion* pReligion = pReligions->GetReligion(eFoundedReligion, getOwner());
						if(pReligion)
						{
							//Other religion! Full bonus.
							if((GET_PLAYER(pOtherUnit->getOwner()).GetReligions()->GetReligionInMostCities() != eFoundedReligion) && GET_PLAYER(pOtherUnit->getOwner()).GetReligions()->GetReligionInMostCities() != NO_RELIGION)
							{			
								// Bonus in own land
								if((pReligion->m_Beliefs.GetCombatVersusOtherReligionOwnLands() > 0) && pBattlePlot->IsFriendlyTerritory(getOwner()))
								{
										iTempModifier = pReligion->m_Beliefs.GetCombatVersusOtherReligionOwnLands();
										iModifier += iTempModifier;
								}
								//Bonus in their land
								else if((pReligion->m_Beliefs.GetCombatVersusOtherReligionTheirLands() > 0) && pBattlePlot->IsFriendlyTerritory(pOtherUnit->getOwner()))
								{
									iTempModifier = pReligion->m_Beliefs.GetCombatVersusOtherReligionTheirLands();
									iModifier += iTempModifier;
								}
							}
							//Same religion (or no religion) - half bonus.
							else
							{
								// Bonus in own land
								if((pReligion->m_Beliefs.GetCombatVersusOtherReligionOwnLands() > 0) && pBattlePlot->IsFriendlyTerritory(getOwner()))
								{
										iTempModifier = (pReligion->m_Beliefs.GetCombatVersusOtherReligionOwnLands() / 2);
										iModifier += iTempModifier;
								}
								//Bonus in their land
								else if((pReligion->m_Beliefs.GetCombatVersusOtherReligionTheirLands() > 0) && pBattlePlot->IsFriendlyTerritory(pOtherUnit->getOwner()))
								{
									iTempModifier = (pReligion->m_Beliefs.GetCombatVersusOtherReligionTheirLands() / 2);
									iModifier += iTempModifier;
								}
							}
						}
					}
				}
			}
		}
	
#endif

		// Bonuses outside one's lands
		else
		{
			iTempModifier = getOutsideFriendlyLandsModifier();
			iModifier += iTempModifier;

			// Bonus against city states?
			if(pBattlePlot->isCity() && GET_PLAYER(pBattlePlot->getOwner()).isMinorCiv())
			{
				iModifier += kPlayer.GetPlayerTraits()->GetCityStateCombatModifier();
			}

			// Founder Belief bonus (this must be a city controlled by an enemy)
			CvCity* pPlotCity = pBattlePlot->getWorkingCity();
			if(pPlotCity)
			{
				if(atWar(getTeam(), pPlotCity->getTeam()))
				{
					ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
					if(eReligion != NO_RELIGION && eReligion == eFoundedReligion)
					{
						const CvReligion* pCityReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, pPlotCity->getOwner());
						if(pCityReligion)
						{
							iTempModifier = pCityReligion->m_Beliefs.GetCombatModifierEnemyCities();
							iModifier += iTempModifier;
						}
					}
				}
			}
		}

		// Capital Defense
		iTempModifier = GetCapitalDefenseModifier();
		if(iTempModifier > 0)
		{
			CvCity* pCapital = GET_PLAYER(getOwner()).getCapitalCity();
			if(pCapital)
			{
				int iDistanceToCapital = plotDistance(pBattlePlot->getX(), pBattlePlot->getY(), pCapital->getX(), pCapital->getY());
				iTempModifier += (iDistanceToCapital * GetCapitalDefenseFalloff());
				if (iTempModifier > 0)
				{
					iModifier += iTempModifier;
				}
			}
		}

		// Trait (player level) bonus against higher tech units
		iTempModifier = GET_PLAYER(getOwner()).GetPlayerTraits()->GetCombatBonusVsHigherTech();
		if(iTempModifier > 0)
		{
			// Only applies defending friendly territory
			if(pBattlePlot->getOwner() == getOwner())
			{
				// Check tech levels too
				UnitTypes eMyUnitType = getUnitType();
				if(pOtherUnit && pOtherUnit->IsHigherTechThan(eMyUnitType))
				{
					iModifier += iTempModifier;
				}
			}
		}

#ifndef AUI_UNIT_FIX_BAD_BONUS_STACKS // Relies on pOtherUnit instead of pBattlePlot, so moved to (Other Unit Known) conditional
		// Trait (player level) bonus against larger civs
		iTempModifier = GET_PLAYER(getOwner()).GetPlayerTraits()->GetCombatBonusVsLargerCiv();
		if(iTempModifier > 0)
		{
			if(pOtherUnit && pOtherUnit->IsLargerCivThan(this))
			{
				iModifier += iTempModifier;
			}
		}
#endif
	}

	////////////////////////
	// OTHER UNIT IS KNOWN
	////////////////////////

	if(pOtherUnit != NULL)
	{
		CvAssertMsg(pOtherUnit != this, "Compared combat strength against one's own pointer. This is weird and probably wrong.");

		if(!bIgnoreUnitAdjacency)
		{
			// Flanking
			int iNumAdjacentFriends = pOtherUnit->GetNumEnemyUnitsAdjacent(this);
			if(iNumAdjacentFriends > 0)
			{
				iTempModifier = /*15*/ GC.getBONUS_PER_ADJACENT_FRIEND() * iNumAdjacentFriends;

				int iFlankModifier = GetFlankAttackModifier();
				if(iFlankModifier > 0)
				{
					iTempModifier = iTempModifier * (100 + iFlankModifier) / 100;
				}

				iModifier += iTempModifier;
			}
		}

		// Generic Unit Class Modifier
		iTempModifier = getUnitClassModifier(pOtherUnit->getUnitClassType());
		iModifier += iTempModifier;

		// Unit Combat type Modifier
		if(pOtherUnit->getUnitCombatType() != NO_UNITCOMBAT)
		{
			iTempModifier = unitCombatModifier(pOtherUnit->getUnitCombatType());
			iModifier += iTempModifier;
#if defined(MOD_BALANCE_CORE)
			CvUnitEntry* pUnitInfo = GC.getUnitInfo(pOtherUnit->getUnitType());
			if(pUnitInfo != NULL && pUnitInfo->IsMounted())
			{
				iTempModifier = unitCombatModifier((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_MOUNTED", true));
				iModifier += iTempModifier;
			}
#endif
		}

		// Domain Modifier
		iTempModifier = domainModifier(pOtherUnit->getDomainType());
		iModifier += iTempModifier;

		// Bonus against city states?
		if(GET_PLAYER(pOtherUnit->getOwner()).isMinorCiv())
		{
			iModifier += kPlayer.GetPlayerTraits()->GetCityStateCombatModifier();
		}

		// OTHER UNIT is a Barbarian
		if(pOtherUnit->isBarbarian())
		{
			// Generic Barb Combat Bonus
			iTempModifier = kPlayer.GetBarbarianCombatBonus();
			iModifier += iTempModifier;

			CvHandicapInfo& thisGameHandicap = GC.getGame().getHandicapInfo();

			// Human bonus
			if(isHuman())
			{
				iTempModifier = thisGameHandicap.getBarbarianCombatModifier();
				iModifier += iTempModifier;
			}
			// AI bonus
			else
			{
				iTempModifier = thisGameHandicap.getAIBarbarianCombatModifier();
				iModifier += iTempModifier;
			}

			if(GC.getGame().isOption(GAMEOPTION_RAGING_BARBARIANS))
			{
				iModifier += 25;
			}
		}

#ifdef AUI_UNIT_FIX_BAD_BONUS_STACKS
		// Trait (player level) bonus against larger civs
		iTempModifier = GET_PLAYER(getOwner()).GetPlayerTraits()->GetCombatBonusVsLargerCiv();
		if (iTempModifier > 0)
		{
			if (pOtherUnit->IsLargerCivThan(this))
			{
				iModifier += iTempModifier;
	}
		}
#endif
	}

	return iModifier;
}

//	--------------------------------------------------------------------------------
/// What is the max strength of this Unit when attacking?
int CvUnit::GetMaxAttackStrength(const CvPlot* pFromPlot, const CvPlot* pToPlot, const CvUnit* pDefender) const
{
	VALIDATE_OBJECT

#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	bool bWouldNeedEmbark = (isEmbarked() || (pFromPlot && pFromPlot->needsEmbarkation(this) && CanEverEmbark()));
	bool bIsEmbarkedAttackingLand = pToPlot && !pToPlot->needsEmbarkation(this) && bWouldNeedEmbark;
#else
	bool bIsEmbarkedAttackingLand = isEmbarked() && (pToPlot && !pToPlot->isWater());

	if(isEmbarked() && !bIsEmbarkedAttackingLand)
		return GetEmbarkedUnitDefense();
#endif

	if(GetBaseCombatStrength(bIsEmbarkedAttackingLand) == 0)
		return 0;

	int iCombat;

	int iTempModifier;
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	int iModifier = GetGenericMaxStrengthModifier(pDefender, pToPlot, /*bIgnoreFlanking*/ false, pFromPlot);
#else
	int iModifier = GetGenericMaxStrengthModifier(pDefender, pToPlot, /*bIgnoreFlanking*/ false);
#endif

	// Generic Attack bonus
	iTempModifier = getAttackModifier();
	iModifier += iTempModifier;

	// Kamikaze attack
	if(getKamikazePercent() != 0)
	{
		iTempModifier = getKamikazePercent();
		iModifier += iTempModifier;
	}

	// Temporary attack bonus (Policies, etc.)
	if(GET_PLAYER(getOwner()).GetAttackBonusTurns() > 0)
	{
		iTempModifier = /*20*/ GC.getPOLICY_ATTACK_BONUS_MOD();
		iModifier += iTempModifier;
	}

#if defined(MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED)
	//If Japan, you should get stronger as you lose health.
	if(MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED && GET_PLAYER(getOwner()).GetPlayerTraits()->IsFightWellDamaged())
	{
		iModifier += (getDamage() / 5);
	}
#endif
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	if(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	{
		const std::vector<ResourceTypes>& vStrategicMonopolies = GET_PLAYER(getOwner()).GetStrategicMonopolies();
		for (size_t iResourceLoop = 0; iResourceLoop < vStrategicMonopolies.size(); iResourceLoop++)
		{
			ResourceTypes eResourceLoop = vStrategicMonopolies[iResourceLoop];
			CvResourceInfo* pInfo = GC.getResourceInfo(eResourceLoop);
			if (pInfo && pInfo->getMonopolyAttackBonus() > 0)
			{
				iModifier += pInfo->getMonopolyAttackBonus();
			}
		}
	}
#endif

	////////////////////////
	// KNOWN DESTINATION PLOT
	////////////////////////

	if(pToPlot != NULL)
	{
		// Attacking a City
		if(pToPlot->isCity())
		{
			iTempModifier = cityAttackModifier();
			iModifier += iTempModifier;

			// Nearby unit sapping this city
			if(IsNearSapper(pToPlot->getPlotCity()))
			{
				iTempModifier = GC.getSAPPED_CITY_ATTACK_MODIFIER();
				iModifier += iTempModifier;
			}

			// City Defending against a Barbarian
			if(isBarbarian())
			{
				iTempModifier = GC.getBARBARIAN_CITY_ATTACK_MODIFIER();
				iModifier += iTempModifier;
			}
		}
		// Some bonuses only apply when NOT attacking a city
		else
		{
			// Attacking into Hills
			if(pToPlot->isHills())
			{
				iTempModifier = hillsAttackModifier();
				iModifier += iTempModifier;
			}

			// Attacking into Open Ground
			if(pToPlot->isOpenGround())
			{
				iTempModifier = openAttackModifier();
				iModifier += iTempModifier;
			}

			// Attacking into Rough Ground
			if(pToPlot->isRoughGround())
			{
				iTempModifier = roughAttackModifier();
				iModifier += iTempModifier;
			}

			// Attacking into a Feature
			if(pToPlot->getFeatureType() != NO_FEATURE)
			{
				iTempModifier = featureAttackModifier(pToPlot->getFeatureType());
				iModifier += iTempModifier;
			}
			// No Feature - Use Terrain Attack Mod
			else
			{
				iTempModifier = terrainAttackModifier(pToPlot->getTerrainType());
				iModifier += iTempModifier;

				// Tack on Hills Attack Mod
				if(pToPlot->isHills())
				{
					iTempModifier = terrainAttackModifier(TERRAIN_HILL);
					iModifier += iTempModifier;
				}
			}
		}

#ifndef AUI_UNIT_FIX_BAD_BONUS_STACKS // Already calculated in generic modifiers
		// Bonus for attacking in one's lands
		if(pToPlot->IsFriendlyTerritory(getOwner()))
		{
			iTempModifier = getFriendlyLandsAttackModifier();
			iModifier += iTempModifier;
		}
#endif

		////////////////////////
		// KNOWN ORIGIN PLOT
		////////////////////////

		if(pFromPlot != NULL)
		{
			// Attacking across a river
			if(!isRiverCrossingNoPenalty())
			{
				if(pFromPlot->isRiverCrossing(directionXY(pFromPlot, pToPlot)))
				{
					iTempModifier = GC.getRIVER_ATTACK_MODIFIER();
					iModifier += iTempModifier;
				}
			}

			// Amphibious attack
			if(!isAmphib())
			{
				if(pFromPlot->needsEmbarkation(this))
				{
					iTempModifier = GC.getAMPHIB_ATTACK_MODIFIER();
					iModifier += iTempModifier;
				}
			}
		}
	}

	////////////////////////
	// KNOWN DEFENDER
	////////////////////////

	if(pDefender != NULL)
	{
		CvAssertMsg(pDefender != this, "Compared attack strength against one's own pointer. This is weird and probably wrong.");

		// Unit Class Attack Modifier
		iTempModifier = unitClassAttackModifier(pDefender->getUnitClassType());
		iModifier += iTempModifier;

		// Bonus VS fortified
		if(pDefender->getFortifyTurns() > 0)
			iModifier += attackFortifiedModifier();

		// Bonus VS wounded
		if(pDefender->getDamage() > 0)
			iModifier += attackWoundedModifier();
	}

	// Unit can't drop below 10% strength
	if(iModifier < -90)
		iModifier = -90;

	iCombat = GetBaseCombatStrength(bIsEmbarkedAttackingLand) * (iModifier + 100);

	return std::max(1, iCombat);
}

//	--------------------------------------------------------------------------------
/// What is the max strength of this Unit when defending?
int CvUnit::GetMaxDefenseStrength(const CvPlot* pInPlot, const CvUnit* pAttacker, bool bFromRangedAttack) const
{
	VALIDATE_OBJECT

#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	if (isEmbarked() || (pInPlot && pInPlot->needsEmbarkation(this) && CanEverEmbark()))
#else
	if(m_bEmbarked)
#endif
	{
		return GetEmbarkedUnitDefense();;
	}

	if(GetBaseCombatStrength() == 0)
		return 0;

	int iCombat;

	int iTempModifier;
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	int iModifier = GetGenericMaxStrengthModifier(pAttacker, pInPlot, /*bIgnoreUnitAdjacency*/ bFromRangedAttack, pInPlot);
#else
	int iModifier = GetGenericMaxStrengthModifier(pAttacker, pInPlot, /*bIgnoreUnitAdjacency*/ bFromRangedAttack);
#endif

	// Generic Defense Bonus
	iTempModifier = getDefenseModifier();
	iModifier += iTempModifier;

	// Defense against Ranged
	if(bFromRangedAttack)
		iModifier += rangedDefenseModifier();

#if defined(MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED)
	//If Japan, you should get stronger as you lose health.
	if(MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED && GET_PLAYER(getOwner()).GetPlayerTraits()->IsFightWellDamaged())
	{
		iModifier += (getDamage() / 5);
	}
#endif
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	if(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	{
		const std::vector<ResourceTypes>& vStrategicMonopolies = GET_PLAYER(getOwner()).GetStrategicMonopolies();
		for (size_t iResourceLoop = 0; iResourceLoop < vStrategicMonopolies.size(); iResourceLoop++)
		{
			ResourceTypes eResourceLoop = vStrategicMonopolies[iResourceLoop];
			CvResourceInfo* pInfo = GC.getResourceInfo(eResourceLoop);
			if (pInfo && pInfo->getMonopolyDefenseBonus() > 0)
			{
				iModifier += pInfo->getMonopolyDefenseBonus();
			}
		}
	}
#endif

	////////////////////////
	// KNOWN DEFENSE PLOT
	////////////////////////

	if(pInPlot != NULL)
	{
		// No TERRAIN bonuses for this Unit?
		iTempModifier = pInPlot->defenseModifier(getTeam(), (pAttacker != NULL) ? pAttacker->ignoreBuildingDefense() : true);

		// If we receive normal defensive bonuses OR iTempModifier is actually a PENALTY, then add in the mod
		if(!noDefensiveBonus() || iTempModifier < 0)
			iModifier += iTempModifier;

		// Fortification
		iTempModifier = fortifyModifier();
		iModifier += iTempModifier;

		// City Defense
		if(pInPlot->isCity())
		{
			iTempModifier = cityDefenseModifier();
			iModifier += iTempModifier;
		}

		// Hill Defense
		if(pInPlot->isHills())
		{
			iTempModifier = hillsDefenseModifier();
			iModifier += iTempModifier;
		}

		// Open Ground Defense
		if(pInPlot->isOpenGround())
		{
			iTempModifier = openDefenseModifier();
			iModifier += iTempModifier;
		}

		// Rough Ground Defense
		if(pInPlot->isRoughGround())
		{
			iTempModifier = roughDefenseModifier();
			iModifier += iTempModifier;
		}

		// Feature Defense
		if(pInPlot->getFeatureType() != NO_FEATURE)
		{
			iTempModifier = featureDefenseModifier(pInPlot->getFeatureType());
			iModifier += iTempModifier;
		}
		// No Feature - use Terrain Defense Mod
		else
		{
			iTempModifier = terrainDefenseModifier(pInPlot->getTerrainType());
			iModifier += iTempModifier;

			// Tack on Hills Defense Mod
			if(pInPlot->isHills())
			{
				iTempModifier = terrainDefenseModifier(TERRAIN_HILL);
				iModifier += iTempModifier;
			}
		}
	}

	////////////////////////
	// KNOWN ATTACKER
	////////////////////////

	if(pAttacker != NULL)
	{
		CvAssertMsg(pAttacker != this, "Compared defense strength against one's own pointer. This is weird and probably wrong.");

		// Unit Class Defense Modifier
		iTempModifier = unitClassDefenseModifier(pAttacker->getUnitClassType());
		iModifier += iTempModifier;
	}

	// Unit can't drop below 10% strength
	if(iModifier < -90)
		iModifier = -90;

	iCombat = GetBaseCombatStrength() * (iModifier + 100);

	// Boats do more damage VS one another
	if(pAttacker != NULL)
	{
		if(pAttacker->getDomainType() == DOMAIN_SEA && getDomainType() == DOMAIN_SEA)
		{
			iCombat *= /*40*/ GC.getNAVAL_COMBAT_DEFENDER_STRENGTH_MULTIPLIER();
			iCombat /= 100;
		}
	}

	return std::max(1, iCombat);
}

//	--------------------------------------------------------------------------------
int CvUnit::GetEmbarkedUnitDefense() const
{
	int iRtnValue;
	int iModifier;
	CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
	EraTypes eEra = kPlayer.GetCurrentEra();

	iRtnValue = GC.getEraInfo(eEra)->getEmbarkedUnitDefense() * 100;

	iModifier = GetEmbarkDefensiveModifier();
	if(iModifier > 0)
	{
		iRtnValue = iRtnValue * (100 + iModifier);
		iRtnValue /= 100;
	}
#if defined(MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED)
	if(MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED && !kPlayer.isHuman())
	{
		//25% of defense added in. This is largely to help the AI.
		iRtnValue += (GetBaseCombatStrength(true) / max(1,GC.getBALANCE_EMBARK_DEFENSE_DIVISOR() ) );
	}
#endif

	return iRtnValue;
}
#if defined(MOD_BALANCE_CORE_MILITARY)
//	--------------------------------------------------------------------------------
int CvUnit::GetResistancePower(const CvUnit* pOtherUnit) const
{
	if(MOD_BALANCE_CORE_MILITARY_RESISTANCE && !pOtherUnit->isBarbarian() && !GET_PLAYER(pOtherUnit->getOwner()).isMinorCiv() && pOtherUnit->getOwner() != NO_PLAYER)
	{
		return GET_PLAYER(pOtherUnit->getOwner()).GetFractionOriginalCapitalsUnderControl() / 2;
	}

	return 0;
}
#endif
//	--------------------------------------------------------------------------------
bool CvUnit::canSiege(TeamTypes eTeam) const
{
	VALIDATE_OBJECT
	if(!IsCanDefend())
	{
		return false;
	}

	if(!isEnemy(eTeam))
	{
		return false;
	}

	if(!isNeverInvisible())
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
int CvUnit::GetBaseRangedCombatStrength() const
{
	VALIDATE_OBJECT

#if defined(MOD_API_EXTENSIONS)
	return m_iBaseRangedCombat;
#else
	return m_pUnitInfo->GetRangedCombat();
#endif
}


#if defined(MOD_API_EXTENSIONS)
//	--------------------------------------------------------------------------------
void CvUnit::SetBaseRangedCombatStrength(int iStrength)
{
	VALIDATE_OBJECT

	m_iBaseRangedCombat = iStrength;
}
#endif


//	--------------------------------------------------------------------------------
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
int CvUnit::GetMaxRangedCombatStrength(const CvUnit* pOtherUnit, const CvCity* pCity, bool bAttacking, bool bForRangedAttack, const CvPlot* pTargetPlot, const CvPlot* pFromPlot) const
{
	VALIDATE_OBJECT

	if (pFromPlot == NULL)
	{
		pFromPlot = plot();
	}
	if (pTargetPlot == NULL)
	{
		if (pOtherUnit != NULL)
		{
			pTargetPlot = pOtherUnit->plot();
		}
		else if (pCity != NULL)
		{
			pTargetPlot = pCity->plot();
		}
	}
	const CvPlot* pMyPlot = pFromPlot;

	if (!bAttacking)
	{
		pFromPlot = pTargetPlot;
		pTargetPlot = pMyPlot;
		pMyPlot = pFromPlot;
	}
	
#else
int CvUnit::GetMaxRangedCombatStrength(const CvUnit* pOtherUnit, const CvCity* pCity, bool bAttacking, bool bForRangedAttack) const
{
	VALIDATE_OBJECT
#endif
	int iModifier;
	int iCombat;

	int iTempModifier;

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
	CvPlayerTraits* pTraits = kPlayer.GetPlayerTraits();
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	ReligionTypes eFoundedReligion = pReligions->GetFounderBenefitsReligion(kPlayer.GetID());

	int iStr = isRangedSupportFire() ? GetBaseCombatStrength() / 2 : GetBaseRangedCombatStrength();

	if(iStr == 0)
	{
		return 0;
	}

	// Extra combat percent
	iModifier = getExtraCombatPercent();

	// Kamikaze attack
	if(getKamikazePercent() != 0)
		iModifier += getKamikazePercent();

	// If the empire is unhappy, then Units get a combat penalty
#if defined(MOD_GLOBAL_CS_RAZE_RARELY)
	if((!kPlayer.isMinorCiv() && kPlayer.IsEmpireUnhappy()) || (kPlayer.isMinorCiv() && kPlayer.IsEmpireVeryUnhappy()))
#else
	if(kPlayer.IsEmpireUnhappy())
#endif
	{
		iModifier += GetUnhappinessCombatPenalty();
	}

#if defined(MOD_BALANCE_CORE_MILITARY)
	// units cannot heal anymore, but strength is unaffected
#else
	// Over our strategic resource limit?
	iTempModifier = GetStrategicResourceCombatPenalty();
	if(iTempModifier != 0)
		iModifier += iTempModifier;
#endif

	// Great General nearby
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	if (IsNearGreatGeneral(pMyPlot) && !IsIgnoreGreatGeneralBenefit())
#else
	if(IsNearGreatGeneral() && !IsIgnoreGreatGeneralBenefit())
#endif
	{
		iModifier += /*25*/ GC.getGREAT_GENERAL_STRENGTH_MOD();
		iModifier += pTraits->GetGreatGeneralExtraBonus();

#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
		if (IsStackedGreatGeneral(pMyPlot))
#else
		if(IsStackedGreatGeneral())
#endif
		{
			iModifier += GetGreatGeneralCombatModifier();

		}
	}

	// Reverse Great General nearby
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	int iReverseGGModifier = GetReverseGreatGeneralModifier(pMyPlot);
#else
	int iReverseGGModifier = GetReverseGreatGeneralModifier();
#endif
	if(iReverseGGModifier != 0)
	{
		iModifier += iReverseGGModifier;
	}

	// Improvement with combat bonus (from trait) nearby
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	int iNearbyImprovementModifier = GetNearbyImprovementModifier(pMyPlot);
#else
	int iNearbyImprovementModifier = GetNearbyImprovementModifier();
#endif
	if(iNearbyImprovementModifier != 0)
	{
		iModifier += iNearbyImprovementModifier;
	}

	// Our empire fights well in Golden Ages?
	if(kPlayer.isGoldenAge())
		iModifier += pTraits->GetGoldenAgeCombatModifier();

#if defined(MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED)
	//If Japan, you should get stronger as you lose health.
	if(MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED && GET_PLAYER(getOwner()).GetPlayerTraits()->IsFightWellDamaged())
	{
		iModifier += (getDamage() / 5);
	}
#endif
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	if(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	{
		const std::vector<ResourceTypes>& vStrategicMonopolies = GET_PLAYER(getOwner()).GetStrategicMonopolies();
		for (size_t iResourceLoop = 0; iResourceLoop < vStrategicMonopolies.size(); iResourceLoop++)
		{
			ResourceTypes eResourceLoop = vStrategicMonopolies[iResourceLoop];
			CvResourceInfo* pInfo = GC.getResourceInfo(eResourceLoop);
			if (pInfo && pInfo->getMonopolyAttackBonus() > 0)
			{
				iModifier += pInfo->getMonopolyAttackBonus();
			}
		}
	}
#endif
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	if (pTargetPlot)
	{
	////////////////////////
		// KNOWN BATTLE PLOT
		////////////////////////

		// ATTACKING
		if (bForRangedAttack)
		{
			// Open Ground
			if (pTargetPlot->isOpenGround())
				iModifier += openRangedAttackModifier();

			// Rough Ground
			if (pTargetPlot->isRoughGround())
				iModifier += roughRangedAttackModifier();
#ifdef AUI_UNIT_FIX_BAD_BONUS_STACKS // Previously, ranged defend bonuses from being in certain territory were only applied when attacking and when targetting a plot of certain terrain (instead of being in it)
		}

		// Bonus for fighting in one's lands
		if (pMyPlot->IsFriendlyTerritory(getOwner()))
		{
			iTempModifier = getFriendlyLandsModifier();
			iModifier += iTempModifier;

			// Founder Belief bonus
			CvCity* pPlotCity = pTargetPlot->getWorkingCity();
			if (pPlotCity)
			{
				ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
				if (eReligion != NO_RELIGION && eReligion == eFoundedReligion)
				{
					const CvReligion* pCityReligion = pReligions->GetReligion(eReligion, pPlotCity->getOwner());
					if (pCityReligion)
					{
						iTempModifier = pCityReligion->m_Beliefs.GetCombatModifierFriendlyCities();
						iModifier += iTempModifier;
					}
				}
			}
		}

		// Bonus for fighting outside one's lands
		else
		{
			iTempModifier = getOutsideFriendlyLandsModifier();
			iModifier += iTempModifier;

			// Founder Belief bonus (this must be a city controlled by an enemy)
			CvCity* pPlotCity = pTargetPlot->getWorkingCity();
			if (pPlotCity)
			{
				if (atWar(getTeam(), pPlotCity->getTeam()))
				{
					ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
					if (eReligion != NO_RELIGION && eReligion == eFoundedReligion)
					{
						const CvReligion* pCityReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, pPlotCity->getOwner());
						if (pCityReligion)
						{
							iTempModifier = pCityReligion->m_Beliefs.GetCombatModifierEnemyCities();
							iModifier += iTempModifier;
						}
					}
				}
			}
		}
#else

			// Bonus for fighting in one's lands
			if (pTargetPlot->IsFriendlyTerritory(getOwner()))
			{
				iTempModifier = getFriendlyLandsModifier();
				iModifier += iTempModifier;

				// Founder Belief bonus
				CvCity* pPlotCity = pTargetPlot->getWorkingCity();
				if (pPlotCity)
				{
					ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
					if (eReligion != NO_RELIGION && eReligion == eFoundedReligion)
					{
						const CvReligion* pCityReligion = pReligions->GetReligion(eReligion, pPlotCity->getOwner());
						if (pCityReligion)
						{
							iTempModifier = pCityReligion->m_Beliefs.GetCombatModifierFriendlyCities();
							iModifier += iTempModifier;
						}
					}
				}
			}

			// Bonus for fighting outside one's lands
			else
			{
				iTempModifier = getOutsideFriendlyLandsModifier();
				iModifier += iTempModifier;

				// Founder Belief bonus (this must be a city controlled by an enemy)
				CvCity* pPlotCity = pTargetPlot->getWorkingCity();
				if (pPlotCity)
				{
					if (atWar(getTeam(), pPlotCity->getTeam()))
					{
						ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
						if (eReligion != NO_RELIGION && eReligion == eFoundedReligion)
						{
							const CvReligion* pCityReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, pPlotCity->getOwner());
							if (pCityReligion)
							{
								iTempModifier = pCityReligion->m_Beliefs.GetCombatModifierEnemyCities();
								iModifier += iTempModifier;
							}
						}
					}
				}
			}
		}
#endif
	}
#endif

	////////////////////////
	// OTHER UNIT IS KNOWN
	////////////////////////

	if(NULL != pOtherUnit)
	{
		// Unit Class Mod
		iModifier += getUnitClassModifier(pOtherUnit->getUnitClassType());

		// Unit combat modifier VS other unit
		if(pOtherUnit->getUnitCombatType() != NO_UNITCOMBAT)
			iModifier += unitCombatModifier(pOtherUnit->getUnitCombatType());

		// Domain modifier VS other unit
		iModifier += domainModifier(pOtherUnit->getDomainType());

		// Bonus VS fortified
		if(pOtherUnit->getFortifyTurns() > 0)
			iModifier += attackFortifiedModifier();

		// Bonus VS wounded
		if(pOtherUnit->getDamage() > 0)
			iModifier += attackWoundedModifier();

		// Bonus against city states?
		if(GET_PLAYER(pOtherUnit->getOwner()).isMinorCiv())
		{
			iModifier += pTraits->GetCityStateCombatModifier();
		}

		// OTHER UNIT is a Barbarian
		if(pOtherUnit->isBarbarian())
		{
			// Generic Barb Combat Bonus
			iTempModifier = kPlayer.GetBarbarianCombatBonus();
			iModifier += iTempModifier;

			CvHandicapInfo& thisGameHandicap = GC.getGame().getHandicapInfo();

			// Human bonus
			if(isHuman())
			{
				iTempModifier = thisGameHandicap.getBarbarianCombatModifier();
				iModifier += iTempModifier;
			}
			// AI bonus
			else
			{
				iTempModifier = thisGameHandicap.getAIBarbarianCombatModifier();
				iModifier += iTempModifier;
			}

			if(GC.getGame().isOption(GAMEOPTION_RAGING_BARBARIANS))
			{
				iModifier += 25;
			}
		}

		// ATTACKING
		if(bForRangedAttack)
		{
			// Unit Class Attack Mod
			iModifier += unitClassAttackModifier(pOtherUnit->getUnitClassType());
#ifndef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS

			////////////////////////
			// KNOWN BATTLE PLOT
			////////////////////////

			CvPlot* pTargetPlot = pOtherUnit->plot();

			// Open Ground
			if(pTargetPlot->isOpenGround())
				iModifier += openRangedAttackModifier();

			// Rough Ground
			if(pTargetPlot->isRoughGround())
				iModifier += roughRangedAttackModifier();

			// Bonus for fighting in one's lands
			if(pTargetPlot->IsFriendlyTerritory(getOwner()))
			{
				iTempModifier = getFriendlyLandsModifier();
				iModifier += iTempModifier;

				// Founder Belief bonus
				CvCity* pPlotCity = pTargetPlot->getWorkingCity();
				if(pPlotCity)
				{
					ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
					if(eReligion != NO_RELIGION && eReligion == eFoundedReligion)
					{
						const CvReligion* pCityReligion = pReligions->GetReligion(eReligion, pPlotCity->getOwner());
						if(pCityReligion)
						{
							iTempModifier = pCityReligion->m_Beliefs.GetCombatModifierFriendlyCities();
							iModifier += iTempModifier;
						}
					}
				}
			}

			// Bonus for fighting outside one's lands
			else
			{
				iTempModifier = getOutsideFriendlyLandsModifier();
				iModifier += iTempModifier;

				// Founder Belief bonus (this must be a city controlled by an enemy)
				CvCity* pPlotCity = pTargetPlot->getWorkingCity();
				if(pPlotCity)
				{
					if(atWar(getTeam(), pPlotCity->getTeam()))
					{
						ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
						if(eReligion != NO_RELIGION && eReligion == eFoundedReligion)
						{
							const CvReligion* pCityReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, pPlotCity->getOwner());
							if(pCityReligion)
							{
								iTempModifier = pCityReligion->m_Beliefs.GetCombatModifierEnemyCities();
								iModifier += iTempModifier;
							}
						}
					}
				}
			}
#endif
		}

		// Ranged DEFENSE
		else
		{
#ifndef AUI_UNIT_FIX_BAD_BONUS_STACKS // Moved out of Known Unit block because this doesn't rely on the other unit
			// Ranged Defense Mod
			iModifier += rangedDefenseModifier();
#endif

			// Unit Class Defense Mod
			iModifier += unitClassDefenseModifier(pOtherUnit->getUnitClassType());
		}
	}

	////////////////////////
	// ATTACKING A CITY
	////////////////////////

	if(pCity != NULL)
	{
		// Attacking a City
		iTempModifier = cityAttackModifier();
		iModifier += iTempModifier;

		// Nearby unit sapping this city
		if(IsNearSapper(pCity))
		{
			iTempModifier = GC.getSAPPED_CITY_ATTACK_MODIFIER();
			iModifier += iTempModifier;
		}

		// Bonus against city states?
		if(GET_PLAYER(pCity->getOwner()).isMinorCiv())
		{
			iModifier += pTraits->GetCityStateCombatModifier();
		}
	}

	// Ranged attack mod
	if(bForRangedAttack)
	{
		iModifier += GetRangedAttackModifier();
	}
#ifdef AUI_UNIT_FIX_BAD_BONUS_STACKS
	else
	{
		// Ranged Defense Mod (this was under the known unit bit, which is stupid)
		iModifier += rangedDefenseModifier();
	}
#endif

	// This unit on offense
	if(bAttacking)
	{
		iModifier += getAttackModifier();
	}
	// This Unit on defense
	else
	{
		// No TERRAIN bonuses for this Unit?
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
		iTempModifier = pMyPlot->defenseModifier(getTeam(), false);
#else
		iTempModifier = plot()->defenseModifier(getTeam(), false);
#endif

		// If we receive normal defensive bonuses OR iTempModifier is actually a PENALTY, then add in the mod
		if(!noDefensiveBonus() || iTempModifier < 0)
			iModifier += iTempModifier;

		iModifier += getDefenseModifier();
	}

	// Unit can't drop below 10% strength
	if(iModifier < -90)
		iModifier = -90;

	iCombat = (iStr * (iModifier + 100));

	return std::max(1, iCombat);
}

//	--------------------------------------------------------------------------------
bool CvUnit::canAirAttack() const
{
	VALIDATE_OBJECT
	return (GetBaseRangedCombatStrength() > 0);
}


//	--------------------------------------------------------------------------------
bool CvUnit::canAirDefend(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(pPlot == NULL)
	{
		pPlot = plot();
	}

	if(maxInterceptionProbability() == 0)
	{
		return false;
	}

	if(getDomainType() != DOMAIN_AIR)
	{
		if(!pPlot->isValidDomainForLocation(*this) && isMatchingDomain(pPlot))
		{
			return false;
		}
	}

	return true;
}


//	--------------------------------------------------------------------------------
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
int CvUnit::GetAirCombatDamage(const CvUnit* pDefender, CvCity* pCity, bool bIncludeRand, int iAssumeExtraDamage, const CvPlot* pTargetPlot, const CvPlot* pFromPlot) const
{
	VALIDATE_OBJECT

	if (pFromPlot == NULL)
		pFromPlot = plot();
	if (pTargetPlot == NULL)
	{
		if (pDefender != NULL)
		{
			pTargetPlot = pDefender->plot();
		}
		else if (pCity != NULL)
		{
			pTargetPlot = pCity->plot();
		}
	}

	int iAttackerStrength = GetMaxRangedCombatStrength(pDefender, pCity, /*bAttacking*/ true, /*bForRangedAttack*/ true, pTargetPlot, pFromPlot);
#else
int CvUnit::GetAirCombatDamage(const CvUnit* pDefender, CvCity* pCity, bool bIncludeRand, int iAssumeExtraDamage) const
{
	VALIDATE_OBJECT

	int iAttackerStrength = GetMaxRangedCombatStrength(pDefender, pCity, /*bAttacking*/ true, /*bForRangedAttack*/ true);
#endif
	int iDefenderStrength;

#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	// City is Defender
	if (pCity != NULL)
	{
		iDefenderStrength = pCity->getStrengthValue();
	}
#endif
	// Unit is Defender
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	else if (pDefender != NULL)
#else
	if(pCity == NULL)
#endif
	{
		// Use Ranged combat value for defender, UNLESS it's a boat
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
		if (pDefender->getDomainType() == DOMAIN_SEA || (iDefenderStrength = pDefender->GetMaxRangedCombatStrength(this, /*pCity*/ NULL, false, /*bForRangedAttack*/ false, pTargetPlot, pFromPlot)) <= 0)
		{
			iDefenderStrength = pDefender->GetMaxDefenseStrength(pTargetPlot, this, /*bFromRangedAttack*/ true);
#else
		if(pDefender->GetMaxRangedCombatStrength(this, /*pCity*/ NULL, false, /*bForRangedAttack*/ false) > 0 && !pDefender->getDomainType() == DOMAIN_SEA)
		{
			iDefenderStrength = pDefender->GetMaxRangedCombatStrength(this, /*pCity*/ NULL, false, /*bForRangedAttack*/ false);
		}
		else
		{
			iDefenderStrength = pDefender->GetMaxDefenseStrength(pDefender->plot(), this, /*bFromRangedAttack*/ true);
#endif
		}
	}
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	else
	{
		iDefenderStrength = 1;
	}
#else
	// City is Defender
	else
	{
		iDefenderStrength = pCity->getStrengthValue();
	}
#endif // AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS

	// The roll will vary damage between 30 and 40 (out of 100) for two units of identical strength

	// Note, 0 is valid - means we don't do anything
	int iAttackerDamageRatio = GC.getMAX_HIT_POINTS() - getDamage() - iAssumeExtraDamage;
	if(iAttackerDamageRatio < 0)
		iAttackerDamageRatio = 0;

	int iAttackerDamage = /*250*/ GC.getRANGE_ATTACK_SAME_STRENGTH_MIN_DAMAGE();
	iAttackerDamage *= iAttackerDamageRatio;
	iAttackerDamage /= GC.getMAX_HIT_POINTS();

	int iAttackerRoll = 0;
	if(bIncludeRand)
	{
		iAttackerRoll = /*300*/ GC.getGame().getJonRandNum(GC.getRANGE_ATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE(), "Unit Ranged Combat Damage");
		iAttackerRoll *= iAttackerDamageRatio;
		iAttackerRoll /= GC.getMAX_HIT_POINTS();
	}
	else
	{
		iAttackerRoll = /*300*/ GC.getRANGE_ATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE();
		iAttackerRoll -= 1;	// Subtract 1 here, because this is the amount normally "lost" when doing a rand roll
		iAttackerRoll *= iAttackerDamageRatio;
		iAttackerRoll /= GC.getMAX_HIT_POINTS();
		iAttackerRoll /= 2;	// The divide by 2 is to provide the average damage
	}
	iAttackerDamage += iAttackerRoll;

	double fStrengthRatio = ((iDefenderStrength > 0)?(double(iAttackerStrength) / iDefenderStrength):double(iAttackerStrength));

	// In case our strength is less than the other guy's, we'll do things in reverse then make the ratio 1 over the result
	if(iDefenderStrength > iAttackerStrength)
	{
		fStrengthRatio = (double(iDefenderStrength) / iAttackerStrength);
	}

	fStrengthRatio = (fStrengthRatio + 3) / 4;
	fStrengthRatio = pow(fStrengthRatio, 4.0);
	fStrengthRatio = (fStrengthRatio + 1) / 2;

	if(iDefenderStrength > iAttackerStrength)
	{
		fStrengthRatio = 1 / fStrengthRatio;
	}

	double fAttackerDamage = (double)iAttackerDamage * fStrengthRatio;
	// Protect against it overflowing an int
	if(fAttackerDamage > INT_MAX)
		iAttackerDamage = INT_MAX;
	else
		iAttackerDamage = int(fAttackerDamage);

	// Bring it back out of hundreds
	iAttackerDamage /= 100;

	iAttackerDamage = max(1,iAttackerDamage);

	return iAttackerDamage;
}


//	--------------------------------------------------------------------------------
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
int CvUnit::GetRangeCombatDamage(const CvUnit* pDefender, CvCity* pCity, bool bIncludeRand, int iAssumeExtraDamage, const CvPlot* pTargetPlot, const CvPlot* pFromPlot) const
{
	VALIDATE_OBJECT
	if (pFromPlot == NULL)
	{
		pFromPlot = plot();
	}
	if (pTargetPlot == NULL)
	{
		if (pDefender != NULL)
		{
			pTargetPlot = pDefender->plot();
		}
		else if (pCity != NULL)
		{
			pTargetPlot = pCity->plot();
		}
	}

	int iAttackerStrength = GetMaxRangedCombatStrength(pDefender, pCity, true, /*bForRangedAttack*/ true, pTargetPlot, pFromPlot);

#else
int CvUnit::GetRangeCombatDamage(const CvUnit* pDefender, CvCity* pCity, bool bIncludeRand, int iAssumeExtraDamage) const
{
	VALIDATE_OBJECT

	int iAttackerStrength = GetMaxRangedCombatStrength(pDefender, pCity, true, /*bForRangedAttack*/ true);
#endif

#if defined(MOD_BALANCE_CORE)
	if (iAttackerStrength==0)
		return 0;
#endif

	int iDefenderStrength = 0;

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	// City is Defender
	if (pCity != NULL)
	{
		iDefenderStrength = pCity->getStrengthValue();
	}
	// Unit is Defender
	else if (pDefender != NULL)
	{
		// If this is a defenseless unit, do a fixed amount of damage
		if (!pDefender->IsCanDefend(pTargetPlot))
			return /*4*/ GC.getNONCOMBAT_UNIT_RANGED_DAMAGE();

		if (pDefender->CanEverEmbark() && pTargetPlot && pTargetPlot->needsEmbarkation(this))
		{
			iDefenderStrength = pDefender->GetEmbarkedUnitDefense();
		}
		// Use Ranged combat value for defender, UNLESS it's a boat or an Impi (ranged support)
		else if (!pDefender->isRangedSupportFire() && pDefender->getDomainType() != DOMAIN_SEA && (iDefenderStrength = pDefender->GetMaxRangedCombatStrength(this, /*pCity*/ NULL, false, false, pTargetPlot, pFromPlot)) > 0)
		{
			// Ranged units take less damage from one another
			iDefenderStrength *= /*125*/ GC.getRANGE_ATTACK_RANGED_DEFENDER_MOD();
			iDefenderStrength /= 100;
		}
		else
		{
			iDefenderStrength = pDefender->GetMaxDefenseStrength(pTargetPlot, this, /*bFromRangedAttack*/ true);
		}
	}
	else
	{
		//defender unknown - assume equal strength unit ...
		iDefenderStrength = GetBaseCombatStrength()*100;
	}
#else
	// Unit is Defender
	if(pCity == NULL)
	{
		// If this is a defenseless unit, do a fixed amount of damage
		if(!pDefender->IsCanDefend())
			return /*4*/ GC.getNONCOMBAT_UNIT_RANGED_DAMAGE();

		if (pDefender->isEmbarked())
		{
			iDefenderStrength = pDefender->GetEmbarkedUnitDefense();;
		}
		// Use Ranged combat value for defender, UNLESS it's a boat or an Impi (ranged support)
		else if(!pDefender->isRangedSupportFire() && pDefender->getDomainType() != DOMAIN_SEA && pDefender->GetMaxRangedCombatStrength(this, /*pCity*/ NULL, false, false) > 0)
		{
			iDefenderStrength = pDefender->GetMaxRangedCombatStrength(this, /*pCity*/ NULL, false, /*bForRangedAttack*/ false);

			// Ranged units take less damage from one another
			iDefenderStrength *= /*125*/ GC.getRANGE_ATTACK_RANGED_DEFENDER_MOD();
			iDefenderStrength /= 100;
		}
		else
		{
			iDefenderStrength = pDefender->GetMaxDefenseStrength(pDefender->plot(), this, /*bFromRangedAttack*/ true);
		}
	}
	// City is Defender
	else
	{
		iDefenderStrength = pCity->getStrengthValue();
	}
#endif

	// The roll will vary damage between 30 and 40 (out of 100) for two units of identical strength

	// Note, 0 is valid - means we don't do anything
	int iWoundedDamageMultiplier = /*50*/ GC.getWOUNDED_DAMAGE_MULTIPLIER();
	iWoundedDamageMultiplier += kPlayer.GetWoundedUnitDamageMod();


	int iAttackerDamageRatio = GC.getMAX_HIT_POINTS() - ((getDamage() - iAssumeExtraDamage) * iWoundedDamageMultiplier / 100);
	if(iAttackerDamageRatio < 0)
		iAttackerDamageRatio = 0;

	int iAttackerDamage = /*250*/ GC.getRANGE_ATTACK_SAME_STRENGTH_MIN_DAMAGE();
	iAttackerDamage *= iAttackerDamageRatio;
	iAttackerDamage /= GC.getMAX_HIT_POINTS();

	int iAttackerRoll = 0;
	if(bIncludeRand)
	{
		iAttackerRoll = /*300*/ GC.getGame().getJonRandNum(GC.getRANGE_ATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE(), "Unit Ranged Combat Damage");
		iAttackerRoll *= iAttackerDamageRatio;
		iAttackerRoll /= GC.getMAX_HIT_POINTS();
	}
	else
	{
		iAttackerRoll = /*300*/ GC.getRANGE_ATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE();
		iAttackerRoll -= 1;	// Subtract 1 here, because this is the amount normally "lost" when doing a rand roll
		iAttackerRoll *= iAttackerDamageRatio;
		iAttackerRoll /= GC.getMAX_HIT_POINTS();
		iAttackerRoll /= 2;	// The divide by 2 is to provide the average damage
	}
	iAttackerDamage += iAttackerRoll;

	double fStrengthRatio = (iDefenderStrength > 0)?(double(iAttackerStrength) / iDefenderStrength):double(iAttackerStrength);

	// In case our strength is less than the other guy's, we'll do things in reverse then make the ratio 1 over the result
	if(iDefenderStrength > iAttackerStrength)
	{
		fStrengthRatio = (double(iDefenderStrength) / iAttackerStrength);
	}

	fStrengthRatio = (fStrengthRatio + 3) / 4;
	fStrengthRatio = pow(fStrengthRatio, 4.0);
#if defined(MOD_BALANCE_CORE)
	//avoid overflow later
	fStrengthRatio = MIN(1e3,(fStrengthRatio + 1) / 2);
#else
	fStrengthRatio = (fStrengthRatio + 1) / 2;
#endif

	if(iDefenderStrength > iAttackerStrength)
	{
		fStrengthRatio = 1 / fStrengthRatio;
	}

	double fAttackerDamage = (double)iAttackerDamage * fStrengthRatio;
	// Protect against it overflowing an int
	if(fAttackerDamage > INT_MAX)
		iAttackerDamage = INT_MAX;
	else
		iAttackerDamage = int(fAttackerDamage);

	// Bring it back out of hundreds
	iAttackerDamage /= 100;

	iAttackerDamage = max(1,iAttackerDamage);

	return iAttackerDamage;
}

//	--------------------------------------------------------------------------------
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
int CvUnit::GetAirStrikeDefenseDamage(const CvUnit* pAttacker, bool bIncludeRand, const CvPlot* pTargetPlot, const CvPlot* pFromPlot) const
{
	if (!pAttacker)
		return 0;
	if (pFromPlot == NULL && pAttacker)
		pFromPlot = pAttacker->plot();
	if (pTargetPlot == NULL)
		pTargetPlot = plot();
	int iAttackerStrength = pAttacker->GetMaxRangedCombatStrength(this, /*pCity*/ NULL, true, /*bForRangedAttack*/ false, pTargetPlot, pFromPlot);
#else
int CvUnit::GetAirStrikeDefenseDamage(const CvUnit* pAttacker, bool bIncludeRand) const
{
	int iAttackerStrength = pAttacker->GetMaxRangedCombatStrength(this, /*pCity*/ NULL, true, /*bForRangedAttack*/ false);
#endif
	int iDefenderStrength = 0;

	// Use Ranged combat value for defender, UNLESS it's a boat
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	if (getDomainType() == DOMAIN_SEA || (iDefenderStrength = GetMaxRangedCombatStrength(pAttacker, /*pCity*/ NULL, false, false, pTargetPlot, pFromPlot)) <= 0)
		iDefenderStrength = GetMaxDefenseStrength(pTargetPlot, pAttacker);
#else
	if(GetMaxRangedCombatStrength(this, /*pCity*/ NULL, false, false) > 0 && !getDomainType() == DOMAIN_SEA)
		iDefenderStrength = GetMaxRangedCombatStrength(pAttacker, /*pCity*/ NULL, false, false);
	else
		iDefenderStrength = GetMaxDefenseStrength(plot(), pAttacker);
#endif

	if(iDefenderStrength == 0)
		return 0;

	int iDefenderDamageRatio = GC.getMAX_HIT_POINTS() - getDamage();
	int iDefenderDamage = /*200*/ GC.getAIR_STRIKE_SAME_STRENGTH_MIN_DEFENSE_DAMAGE() * iDefenderDamageRatio / GC.getMAX_HIT_POINTS();

	int iDefenderRoll = 0;
	if(bIncludeRand)
	{
		iDefenderRoll = /*200*/ GC.getGame().getJonRandNum(GC.getAIR_STRIKE_SAME_STRENGTH_POSSIBLE_EXTRA_DEFENSE_DAMAGE(), "Unit Air Strike Combat Damage");
		iDefenderRoll *= iDefenderDamageRatio;
		iDefenderRoll /= GC.getMAX_HIT_POINTS();
	}
	else
	{
		iDefenderRoll = /*200*/ GC.getAIR_STRIKE_SAME_STRENGTH_POSSIBLE_EXTRA_DEFENSE_DAMAGE();
		iDefenderRoll -= 1;	// Subtract 1 here, because this is the amount normally "lost" when doing a rand roll
		iDefenderRoll *= iDefenderDamageRatio;
		iDefenderRoll /= GC.getMAX_HIT_POINTS();
		iDefenderRoll /= 2;	// The divide by 2 is to provide the average damage
	}
	iDefenderDamage += iDefenderRoll;

	double fStrengthRatio = (double(iDefenderStrength) / iAttackerStrength);

	// In case our strength is less than the other guy's, we'll do things in reverse then make the ratio 1 over the result
	if(iAttackerStrength > iDefenderStrength)
	{
		fStrengthRatio = (double(iAttackerStrength) / iDefenderStrength);
	}

	fStrengthRatio = (fStrengthRatio + 3) / 4;
	fStrengthRatio = pow(fStrengthRatio, 4.0);
	fStrengthRatio = (fStrengthRatio + 1) / 2;

	if(iAttackerStrength > iDefenderStrength)
	{
		fStrengthRatio = 1 / fStrengthRatio;
	}

	iDefenderDamage = int(iDefenderDamage * fStrengthRatio);

	// Bring it back out of hundreds
	iDefenderDamage /= 100;

	iDefenderDamage = max(1,iDefenderDamage);

	return iDefenderDamage;
}

//	--------------------------------------------------------------------------------
CvUnit* CvUnit::GetBestInterceptor(const CvPlot& interceptPlot, const CvUnit* pkDefender /* = NULL */, bool bLandInterceptorsOnly /*false*/, bool bVisibleInterceptorsOnly /*false*/, int* piNumPossibleInterceptors) const
{
	VALIDATE_OBJECT
	CvUnit* pBestUnit = 0;
	int iBestValue = 0;
	int iBestDistance = INT_MAX;

	// Loop through all players' Units (that we're at war with) to see if they can intercept
	const std::vector<PlayerTypes>& vEnemies = GET_PLAYER(getOwner()).GetPlayersAtWarWith();

	for(size_t iI = 0; iI < vEnemies.size(); iI++)
	{
		CvPlayerAI& kLoopPlayer = GET_PLAYER(vEnemies[iI]);
		TeamTypes eLoopTeam = kLoopPlayer.getTeam();

		//stealth unit? no intercept
		if(isInvisible(eLoopTeam, false, false))
			continue;

		int iLoop = 0;
		for(CvUnit* pLoopUnit = kLoopPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kLoopPlayer.nextUnit(&iLoop))
		{
			// Must be able to intercept
			if(pLoopUnit != pkDefender && !pLoopUnit->isDelayedDeath() && pLoopUnit->canAirDefend() && !pLoopUnit->isInCombat())
			{
				// Must not have already intercepted this turn
				if(!pLoopUnit->isOutOfInterceptions())
				{
					// Must either be a non-air Unit, or an air Unit that hasn't moved this turn
					if((pLoopUnit->getDomainType() != DOMAIN_AIR) || !(pLoopUnit->hasMoved()))
					{
						// Must either be a non-air Unit or an air Unit on intercept
						if((pLoopUnit->getDomainType() != DOMAIN_AIR) || (pLoopUnit->GetActivityType() == ACTIVITY_INTERCEPT))
						{
							// Check input booleans
							if (!bLandInterceptorsOnly || pLoopUnit->getDomainType() == DOMAIN_LAND)
							{
								if (!bVisibleInterceptorsOnly || pLoopUnit->plot()->isVisible(getTeam()))
								{
									// Test range
									int iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), interceptPlot.getX(), interceptPlot.getY());
									if( iDistance <= pLoopUnit->getUnitInfo().GetAirInterceptRange())
									{
										int iValue = pLoopUnit->currInterceptionProbability();

										if (iValue>0 && piNumPossibleInterceptors)
											(*piNumPossibleInterceptors)++;

										if( iValue>iBestValue || (iValue==iBestValue && iDistance<iBestDistance) )
										{
											iBestDistance = iDistance;
											iBestValue = iValue;
											pBestUnit = pLoopUnit;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return pBestUnit;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetInterceptorCount(const CvPlot& interceptPlot, CvUnit* pkDefender /* = NULL */, bool bLandInterceptorsOnly /*false*/, bool bVisibleInterceptorsOnly /*false*/) const
{
	int iCount = 0;
	GetBestInterceptor(interceptPlot,pkDefender,bLandInterceptorsOnly,bVisibleInterceptorsOnly,&iCount);
	return iCount;
}

//	--------------------------------------------------------------------------------
/// Amount of damage done by this unit when intercepting pAttacker
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
int CvUnit::GetInterceptionDamage(const CvUnit* pAttacker, bool bIncludeRand, const CvPlot* pTargetPlot, const CvPlot* pFromPlot) const
{
	if (pFromPlot == NULL && pAttacker)
		pFromPlot = pAttacker->plot();
	if (pTargetPlot == NULL)
		pTargetPlot = plot();

	int iAttackerStrength = pAttacker->GetMaxRangedCombatStrength(this, /*pCity*/ NULL, true, /*bForRangedAttack*/ false, pTargetPlot, pFromPlot);
#else
int CvUnit::GetInterceptionDamage(const CvUnit* pAttacker, bool bIncludeRand) const
{
	int iAttackerStrength = pAttacker->GetMaxRangedCombatStrength(this, /*pCity*/ NULL, true, /*bForRangedAttack*/ false);
#endif

	int iInterceptorStrength = 0;
#if defined(MOD_BUGFIX_INTERCEPTOR_STRENGTH)
	if (MOD_BUGFIX_INTERCEPTOR_STRENGTH) {
		switch (getDomainType()) {
			case DOMAIN_SEA:
				iInterceptorStrength = GetMaxDefenseStrength(plot(), pAttacker);
				break;

			case DOMAIN_AIR:
				iInterceptorStrength = GetMaxRangedCombatStrength(pAttacker, /*pCity*/ NULL, false, /*bForRangedAttack*/ false);
				break;

			case DOMAIN_LAND:
				iInterceptorStrength = GetMaxAttackStrength(NULL, NULL, pAttacker);
				break;

			case DOMAIN_IMMOBILE:
				iInterceptorStrength = GetMaxAttackStrength(NULL, NULL, pAttacker);
				break;

			case DOMAIN_HOVER:
				iInterceptorStrength = GetMaxAttackStrength(NULL, NULL, pAttacker);
				break;

			default:
				CvAssert(false);
				break;
		}
	} else {
#endif
		// Use Ranged combat value for Interceptor, UNLESS it's a boat
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	if (getDomainType() == DOMAIN_SEA || (iInterceptorStrength = GetMaxRangedCombatStrength(pAttacker, /*pCity*/ NULL, false, /*bForRangedAttack*/ false, pTargetPlot, pFromPlot)) <= 0)
	{
		iInterceptorStrength = GetMaxDefenseStrength(pTargetPlot, pAttacker);
#else
		if(GetMaxRangedCombatStrength(this, /*pCity*/ NULL, false, false) > 0 && !getDomainType() == DOMAIN_SEA)
		{
			iInterceptorStrength = GetMaxRangedCombatStrength(pAttacker, /*pCity*/ NULL, false, /*bForRangedAttack*/ false);
		}
		else
		{
			iInterceptorStrength = GetMaxDefenseStrength(plot(), pAttacker);
#endif
		}
#if defined(MOD_BUGFIX_INTERCEPTOR_STRENGTH)
	}
	CUSTOMLOG("Interceptor base str for player/unit %i/%i is %i", getOwner(), GetID(), iInterceptorStrength);
#endif

	// Mod to interceptor strength
	iInterceptorStrength *= (100 + GetInterceptionCombatModifier());
	iInterceptorStrength /= 100;

	// The roll will vary damage between 2 and 3 (out of 10) for two units of identical strength

	int iInterceptorDamageRatio = GC.getMAX_HIT_POINTS() - getDamage();
	int iInterceptorDamage = /*400*/ GC.getINTERCEPTION_SAME_STRENGTH_MIN_DAMAGE() * iInterceptorDamageRatio / GC.getMAX_HIT_POINTS();

	int iInterceptorRoll = 0;
	if(bIncludeRand)
	{
		iInterceptorRoll = /*300*/ GC.getGame().getJonRandNum(GC.getINTERCEPTION_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE(), "Interception Combat Damage");
		iInterceptorRoll *= iInterceptorDamageRatio;
		iInterceptorRoll /= GC.getMAX_HIT_POINTS();
	}
	else
	{
		iInterceptorRoll = /*300*/ GC.getINTERCEPTION_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE();
		iInterceptorRoll -= 1;	// Subtract 1 here, because this is the amount normally "lost" when doing a rand roll
		iInterceptorRoll *= iInterceptorDamageRatio;
		iInterceptorRoll /= GC.getMAX_HIT_POINTS();
		iInterceptorRoll /= 2;	// The divide by 2 is to provide the average damage
	}
	iInterceptorDamage += iInterceptorRoll;

	double fStrengthRatio = (double(iInterceptorStrength) / iAttackerStrength);

	// In case our strength is less than the other guy's, we'll do things in reverse then make the ratio 1 over the result
	if(iAttackerStrength > iInterceptorStrength)
	{
		fStrengthRatio = (double(iAttackerStrength) / iInterceptorStrength);
	}

	fStrengthRatio = (fStrengthRatio + 3) / 4;
	fStrengthRatio = pow(fStrengthRatio, 4.0);
	fStrengthRatio = (fStrengthRatio + 1) / 2;

	if(iAttackerStrength > iInterceptorStrength)
	{
		fStrengthRatio = 1 / fStrengthRatio;
	}

	iInterceptorDamage = int(iInterceptorDamage * fStrengthRatio);

	// Mod to interception damage
	iInterceptorDamage *= (100 + pAttacker->GetInterceptionDefenseDamageModifier());
	iInterceptorDamage /= 100;

	// Bring it back out of hundreds
	iInterceptorDamage /= 100;

	iInterceptorDamage = max(1,iInterceptorDamage);

	CUSTOMLOG("Interceptor damage by player/unit %i/%i is %i", getOwner(), GetID(), iInterceptorDamage);
	return iInterceptorDamage;
}

#if defined(MOD_GLOBAL_PARATROOPS_AA_DAMAGE)
/// Amount of damage done by this unit when intercepting a paratrooper
// (This code is basically the same as GetInterceptionDamage() except it uses the attacker's defensive strength)
int CvUnit::GetParadropInterceptionDamage(const CvUnit* pAttacker, bool bIncludeRand) const
{
	int iAttackerStrength = pAttacker->GetMaxDefenseStrength(plot(), this);

	int iInterceptorStrength = 0;

#if defined(MOD_BUGFIX_INTERCEPTOR_STRENGTH)
	if (MOD_BUGFIX_INTERCEPTOR_STRENGTH) {
		switch (getDomainType()) {
			case DOMAIN_SEA:
				iInterceptorStrength = GetMaxDefenseStrength(plot(), pAttacker);
				break;

			case DOMAIN_AIR:
				iInterceptorStrength = GetMaxRangedCombatStrength(pAttacker, /*pCity*/ NULL, false, /*bForRangedAttack*/ false);
				break;

			case DOMAIN_LAND:
				iInterceptorStrength = GetMaxAttackStrength(NULL, NULL, pAttacker);
				break;

			case DOMAIN_IMMOBILE:
				iInterceptorStrength = GetMaxAttackStrength(NULL, NULL, pAttacker);
				break;

			default:
				CvAssert(false);
				break;
		}
	} else {
#else
		// Use Ranged combat value for Interceptor, UNLESS it's a boat
		if(GetMaxRangedCombatStrength(this, /*pCity*/ NULL, false, false) > 0 && !getDomainType() == DOMAIN_SEA)
		{
			iInterceptorStrength = GetMaxRangedCombatStrength(pAttacker, /*pCity*/ NULL, false, /*bForRangedAttack*/ false);
		}
		else
		{
			iInterceptorStrength = GetMaxDefenseStrength(plot(), pAttacker);
		}
#endif
#if defined(MOD_BUGFIX_INTERCEPTOR_STRENGTH)
	}
	// CUSTOMLOG("Interceptor base str against paradrop for player/unit %i/%i is %i", getOwner(), GetID(), iInterceptorStrength);
#endif

	// Mod to interceptor strength
	iInterceptorStrength *= (100 + GetInterceptionCombatModifier());
	iInterceptorStrength /= 100;
	// CUSTOMLOG("ParadropIntercept: Paratrooper str=%i, Interceptor str=%i", iAttackerStrength, iInterceptorStrength)

	// The roll will vary damage between 2 and 3 (out of 10) for two units of identical strength

	int iInterceptorDamageRatio = GC.getMAX_HIT_POINTS() - getDamage();
	int iInterceptorDamage = /*400*/ GC.getINTERCEPTION_SAME_STRENGTH_MIN_DAMAGE() * iInterceptorDamageRatio / GC.getMAX_HIT_POINTS();

	int iInterceptorRoll = 0;
	if(bIncludeRand)
	{
		iInterceptorRoll = /*300*/ GC.getGame().getJonRandNum(GC.getINTERCEPTION_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE(), "Interception Combat Damage");
		iInterceptorRoll *= iInterceptorDamageRatio;
		iInterceptorRoll /= GC.getMAX_HIT_POINTS();
	}
	else
	{
		iInterceptorRoll = /*300*/ GC.getINTERCEPTION_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE();
		iInterceptorRoll -= 1;	// Subtract 1 here, because this is the amount normally "lost" when doing a rand roll
		iInterceptorRoll *= iInterceptorDamageRatio;
		iInterceptorRoll /= GC.getMAX_HIT_POINTS();
		iInterceptorRoll /= 2;	// The divide by 2 is to provide the average damage
	}
	iInterceptorDamage += iInterceptorRoll;

	double fStrengthRatio = (double(iInterceptorStrength) / iAttackerStrength);

	// In case our strength is less than the other guy's, we'll do things in reverse then make the ratio 1 over the result
	if(iAttackerStrength > iInterceptorStrength)
	{
		fStrengthRatio = (double(iAttackerStrength) / iInterceptorStrength);
	}

	fStrengthRatio = (fStrengthRatio + 3) / 4;
	fStrengthRatio = pow(fStrengthRatio, 4.0);
	fStrengthRatio = (fStrengthRatio + 1) / 2;

	if(iAttackerStrength > iInterceptorStrength)
	{
		fStrengthRatio = 1 / fStrengthRatio;
	}

	iInterceptorDamage = int(iInterceptorDamage * fStrengthRatio);

	// Mod to interception damage
	iInterceptorDamage *= (100 + pAttacker->GetInterceptionDefenseDamageModifier());
	iInterceptorDamage /= 100;

	// Bring it back out of hundreds
	iInterceptorDamage /= 100;

	iInterceptorDamage = max(1,iInterceptorDamage);

	return iInterceptorDamage;
}
#endif

//	--------------------------------------------------------------------------------
int CvUnit::GetCombatLimit() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->GetCombatLimit();
}


//	--------------------------------------------------------------------------------
int CvUnit::GetRangedCombatLimit() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->GetRangedCombatLimit();
}


//	--------------------------------------------------------------------------------
bool CvUnit::isWaiting() const
{
	VALIDATE_OBJECT

	ActivityTypes eActivityType = GetActivityType();

	return ((eActivityType == ACTIVITY_HOLD)||
			(eActivityType == ACTIVITY_SLEEP)   ||
			(eActivityType == ACTIVITY_HEAL)    ||
			(eActivityType == ACTIVITY_SENTRY)  ||
			(eActivityType == ACTIVITY_INTERCEPT));
}


//	--------------------------------------------------------------------------------
bool CvUnit::isFortifyable(bool bCanWaitForNextTurn) const
{
	VALIDATE_OBJECT
	// Can't fortify if you've already used any moves this turn
	if(!bCanWaitForNextTurn)
	{
		if(hasMoved())
		{
			return false;
		}
	}

	if(!IsEverFortifyable())
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
/// Can this Unit EVER fortify? (may be redundant with some other stuff)
bool CvUnit::IsEverFortifyable() const
{
	VALIDATE_OBJECT

	if(!IsCombatUnit() || noDefensiveBonus() || ((getDomainType() != DOMAIN_LAND) && (getDomainType() != DOMAIN_IMMOBILE)))
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
int CvUnit::fortifyModifier() const
{
	VALIDATE_OBJECT
	int iValue = 0;
	int iTurnsFortified = getFortifyTurns();

	if(isRangedSupportFire())
	{
		return iValue;
	}

	if(iTurnsFortified > 0)
	{
		iValue = iTurnsFortified * GC.getFORTIFY_MODIFIER_PER_TURN();
	
		// Apply modifiers
		int iMod = GET_PLAYER(getOwner()).getUnitFortificationModifier();
		if(iMod != 0)
		{
			iValue = ((100 + iMod) / 100) * iValue;
		}

		if(iValue < 0)
		{
			CvAssertMsg(false, "Calculated a negative combat mod for a fortified unit. Resetting to 0 instead.");
			iValue = 0;
		}
	}

	return iValue;
}


//	--------------------------------------------------------------------------------
int CvUnit::experienceNeeded() const
{
	VALIDATE_OBJECT

	const int iLevel = getLevel();

	// Sum up the levels to get our multiplier (1:1, 2:3, 3:6, 4:10, etc.)
	int iExperienceMultiplier = 0;
	for(int iLevelLoop = 1; iLevelLoop <= iLevel; iLevelLoop++)
	{
		iExperienceMultiplier += iLevelLoop;
	}

	int iExperienceNeeded = /*10*/ GC.getEXPERIENCE_PER_LEVEL() * iExperienceMultiplier;

	const int iModifier = GET_PLAYER(getOwner()).getLevelExperienceModifier();
	if(0 != iModifier)
	{
		float fTemp = (float) iExperienceNeeded;
		fTemp *= (100 + iModifier);
		fTemp /= 100;
		iExperienceNeeded = (int) ceil(fTemp); // Round up
	}

	return iExperienceNeeded;
}


//	--------------------------------------------------------------------------------
int CvUnit::attackXPValue() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->GetXPValueAttack();
}


//	--------------------------------------------------------------------------------
int CvUnit::defenseXPValue() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->GetXPValueDefense();
}


//	--------------------------------------------------------------------------------
int CvUnit::maxXPValue() const
{
	VALIDATE_OBJECT
	int iMaxValue;

	iMaxValue = INT_MAX;

	if(isBarbarian())
	{
		iMaxValue = std::min(iMaxValue, GC.getBARBARIAN_MAX_XP_VALUE());
	}

	return iMaxValue;
}


//	--------------------------------------------------------------------------------
int CvUnit::firstStrikes() const
{
	VALIDATE_OBJECT
	return std::max(0, getExtraFirstStrikes());
}


//	--------------------------------------------------------------------------------
int CvUnit::chanceFirstStrikes() const
{
	VALIDATE_OBJECT
	return std::max(0, getExtraChanceFirstStrikes());
}


//	--------------------------------------------------------------------------------
int CvUnit::maxFirstStrikes() const
{
	VALIDATE_OBJECT
	return (firstStrikes() + chanceFirstStrikes());
}


//	--------------------------------------------------------------------------------
bool CvUnit::isRanged() const
{
	VALIDATE_OBJECT
	if(GetBaseRangedCombatStrength() > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


//	--------------------------------------------------------------------------------
bool CvUnit::immuneToFirstStrikes() const
{
	VALIDATE_OBJECT
	return (getImmuneToFirstStrikesCount() > 0);
}


//	--------------------------------------------------------------------------------
bool CvUnit::ignoreBuildingDefense() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->IsIgnoreBuildingDefense();
}


//	--------------------------------------------------------------------------------
bool CvUnit::ignoreTerrainCost() const
{
	VALIDATE_OBJECT
	return getIgnoreTerrainCostCount() > 0 ? true : false;
}

//	--------------------------------------------------------------------------------
int CvUnit::getIgnoreTerrainCostCount() const
{
	VALIDATE_OBJECT
	return m_iIgnoreTerrainCostCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeIgnoreTerrainCostCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iIgnoreTerrainCostCount += iValue;
	}
}


#if defined(MOD_API_PLOT_BASED_DAMAGE)
//	--------------------------------------------------------------------------------
bool CvUnit::ignoreTerrainDamage() const
{
	VALIDATE_OBJECT
	return getIgnoreTerrainDamageCount() > 0 ? true : false;
}

//	--------------------------------------------------------------------------------
int CvUnit::getIgnoreTerrainDamageCount() const
{
	VALIDATE_OBJECT
	return m_iIgnoreTerrainDamageCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeIgnoreTerrainDamageCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iIgnoreTerrainDamageCount += iValue;
	}
}


//	--------------------------------------------------------------------------------
bool CvUnit::ignoreFeatureDamage() const
{
	VALIDATE_OBJECT
	return getIgnoreFeatureDamageCount() > 0 ? true : false;
}

//	--------------------------------------------------------------------------------
int CvUnit::getIgnoreFeatureDamageCount() const
{
	VALIDATE_OBJECT
	return m_iIgnoreFeatureDamageCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeIgnoreFeatureDamageCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iIgnoreFeatureDamageCount += iValue;
	}
}


//	--------------------------------------------------------------------------------
bool CvUnit::extraTerrainDamage() const
{
	VALIDATE_OBJECT
	return getExtraTerrainDamageCount() > 0 ? true : false;
}

//	--------------------------------------------------------------------------------
int CvUnit::getExtraTerrainDamageCount() const
{
	VALIDATE_OBJECT
	return m_iExtraTerrainDamageCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeExtraTerrainDamageCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iExtraTerrainDamageCount += iValue;
	}
}


//	--------------------------------------------------------------------------------
bool CvUnit::extraFeatureDamage() const
{
	VALIDATE_OBJECT
	return getExtraFeatureDamageCount() > 0 ? true : false;
}

//	--------------------------------------------------------------------------------
int CvUnit::getExtraFeatureDamageCount() const
{
	VALIDATE_OBJECT
	return m_iExtraFeatureDamageCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeExtraFeatureDamageCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iExtraFeatureDamageCount += iValue;
	}
}
#endif


#if defined(MOD_PROMOTIONS_IMPROVEMENT_BONUS)
int CvUnit::GetNearbyImprovementCombatBonus() const
{
	return m_iNearbyImprovementCombatBonus;
}

void CvUnit::SetNearbyImprovementCombatBonus(int iCombatBonus)
{
	m_iNearbyImprovementCombatBonus = iCombatBonus;
}

int CvUnit::GetNearbyImprovementBonusRange() const
{
	return m_iNearbyImprovementBonusRange;
}

void CvUnit::SetNearbyImprovementBonusRange(int iBonusRange)
{
	m_iNearbyImprovementBonusRange = iBonusRange;
}

ImprovementTypes CvUnit::GetCombatBonusImprovement() const
{
	return m_eCombatBonusImprovement;
}

void CvUnit::SetCombatBonusImprovement(ImprovementTypes eImprovement)
{
	m_eCombatBonusImprovement = eImprovement;
}
#endif

#if defined(MOD_PROMOTIONS_CROSS_MOUNTAINS)
//	--------------------------------------------------------------------------------
bool CvUnit::canCrossMountains() const
{
	VALIDATE_OBJECT
	return (MOD_PROMOTIONS_CROSS_MOUNTAINS && getCanCrossMountainsCount() > 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getCanCrossMountainsCount() const
{
	VALIDATE_OBJECT
	return m_iCanCrossMountainsCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeCanCrossMountainsCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iCanCrossMountainsCount += iValue;
	}
}
#endif


#if defined(MOD_PROMOTIONS_CROSS_OCEANS)
//	--------------------------------------------------------------------------------
bool CvUnit::canCrossOceans() const
{
	VALIDATE_OBJECT
	return MOD_PROMOTIONS_CROSS_OCEANS && (getCanCrossOceansCount() > 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getCanCrossOceansCount() const
{
	VALIDATE_OBJECT
	return m_iCanCrossOceansCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeCanCrossOceansCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iCanCrossOceansCount += iValue;
	}
}
#endif


#if defined(MOD_PROMOTIONS_CROSS_ICE)
//	--------------------------------------------------------------------------------
bool CvUnit::canCrossIce() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).GetPlayerTraits()->IsAbleToCrossIce() || (MOD_PROMOTIONS_CROSS_ICE && getCanCrossIceCount() > 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getCanCrossIceCount() const
{
	VALIDATE_OBJECT
	return m_iCanCrossIceCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeCanCrossIceCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iCanCrossIceCount += iValue;
	}
}
#endif
#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
void CvUnit::ChangeNumTilesRevealedThisTurn(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iNumTilesRevealedThisTurn += iValue;
	}
}
//	--------------------------------------------------------------------------------
void CvUnit::SetNumTilesRevealedThisTurn(int iValue)
{
	VALIDATE_OBJECT
	m_iNumTilesRevealedThisTurn = iValue;
}
//	--------------------------------------------------------------------------------
int CvUnit::GetNumTilesRevealedThisTurn()
{
	VALIDATE_OBJECT
	return m_iNumTilesRevealedThisTurn;
}
#endif

//	--------------------------------------------------------------------------------
bool CvUnit::IsRoughTerrainEndsTurn() const
{
	VALIDATE_OBJECT
	return GetRoughTerrainEndsTurnCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetRoughTerrainEndsTurnCount() const
{
	VALIDATE_OBJECT
	return m_iRoughTerrainEndsTurnCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeRoughTerrainEndsTurnCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iRoughTerrainEndsTurnCount += iValue;
	}
}


//	--------------------------------------------------------------------------------
bool CvUnit::IsHoveringUnit() const
{
	VALIDATE_OBJECT
	return GetHoveringUnitCount() > 0 ? true : false;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetHoveringUnitCount() const
{
	VALIDATE_OBJECT
	return m_iHoveringUnitCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeHoveringUnitCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iHoveringUnitCount += iValue;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::flatMovementCost() const
{
	VALIDATE_OBJECT
	return getFlatMovementCostCount() > 0 ? true : false;
}

//	--------------------------------------------------------------------------------
int CvUnit::getFlatMovementCostCount() const
{
	VALIDATE_OBJECT
	return m_iFlatMovementCostCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeFlatMovementCostCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iFlatMovementCostCount += iValue;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::canMoveImpassable() const
{
	VALIDATE_OBJECT
	return getCanMoveImpassableCount() > 0 ? true : false;
}

//	--------------------------------------------------------------------------------
int CvUnit::getCanMoveImpassableCount() const
{
	VALIDATE_OBJECT
	return m_iCanMoveImpassableCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeCanMoveImpassableCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iCanMoveImpassableCount += iValue;
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::changeCanMoveAllTerrainCount(int iValue)
{
	VALIDATE_OBJECT
	m_iCanMoveAllTerrainCount += iValue;
	CvAssert(getCanMoveAllTerrainCount() >= 0);
}

//	--------------------------------------------------------------------------------
bool CvUnit::canMoveAllTerrain() const
{
	VALIDATE_OBJECT
	return (getCanMoveAllTerrainCount() > 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getCanMoveAllTerrainCount() const
{
	VALIDATE_OBJECT
	return m_iCanMoveAllTerrainCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeCanMoveAfterAttackingCount(int iValue)
{
	VALIDATE_OBJECT
	m_iCanMoveAfterAttackingCount += iValue;
	CvAssert(getCanMoveAfterAttackingCount() >= 0);
}

//	--------------------------------------------------------------------------------
bool CvUnit::canMoveAfterAttacking() const
{
	VALIDATE_OBJECT

	// Units with blitz can move after attacking (otherwise there isn't much point!)
	if(isBlitz())
	{
		return true;
	}

	return (getCanMoveAfterAttackingCount() > 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getCanMoveAfterAttackingCount() const
{
	VALIDATE_OBJECT
	return m_iCanMoveAfterAttackingCount;
}

//	--------------------------------------------------------------------------------
bool CvUnit::hasFreePillageMove() const
{
	VALIDATE_OBJECT
	return getFreePillageMoveCount() > 0 ? true : false;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeFreePillageMoveCount(int iValue)
{
	VALIDATE_OBJECT
	m_iFreePillageMoveCount += iValue;
	CvAssert(getFreePillageMoveCount() >= 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getFreePillageMoveCount() const
{
	VALIDATE_OBJECT
	return m_iFreePillageMoveCount;
}

//	--------------------------------------------------------------------------------
bool CvUnit::hasHealOnPillage() const
{
	VALIDATE_OBJECT
	return getHealOnPillageCount() > 0 ? true : false;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeHealOnPillageCount(int iValue)
{
	VALIDATE_OBJECT
	m_iHealOnPillageCount += iValue;
	CvAssert(getHealOnPillageCount() >= 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getHealOnPillageCount() const
{
	VALIDATE_OBJECT
	return m_iHealOnPillageCount;
}

//	--------------------------------------------------------------------------------
int CvUnit::getHPHealedIfDefeatEnemy() const
{
	VALIDATE_OBJECT
	return m_iHPHealedIfDefeatEnemy;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeHPHealedIfDefeatEnemy(int iValue)
{
	VALIDATE_OBJECT
	m_iHPHealedIfDefeatEnemy += iValue;
	CvAssert(getHPHealedIfDefeatEnemy() >= 0);
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsHealIfDefeatExcludeBarbarians() const
{
	VALIDATE_OBJECT
	return GetHealIfDefeatExcludeBarbariansCount() > 0 ? true : false;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetHealIfDefeatExcludeBarbariansCount() const
{
	VALIDATE_OBJECT
	return m_iHealIfDefeatExcludeBarbariansCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeHealIfDefeatExcludeBarbariansCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iHealIfDefeatExcludeBarbariansCount += iValue;
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::GetGoldenAgeValueFromKills() const
{
	VALIDATE_OBJECT
		return m_iGoldenAgeValueFromKills;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeGoldenAgeValueFromKills(int iValue)
{
	VALIDATE_OBJECT
		m_iGoldenAgeValueFromKills += iValue;
	CvAssert(GetGoldenAgeValueFromKills() >= 0);
}

//	--------------------------------------------------------------------------------
bool CvUnit::isOnlyDefensive() const
{
	VALIDATE_OBJECT

	return getOnlyDefensiveCount() > 0 ? true : false;
}

//	--------------------------------------------------------------------------------
int CvUnit::getOnlyDefensiveCount() const
{
	VALIDATE_OBJECT
	return m_iOnlyDefensiveCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeOnlyDefensiveCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iOnlyDefensiveCount += iValue;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::noDefensiveBonus() const
{
	VALIDATE_OBJECT
	// Boats get no bonus for fortifying
	if(getDomainType() == DOMAIN_SEA)
	{
		return true;
	}

	return getNoDefensiveBonusCount() > 0 ? true : false;
}

//	--------------------------------------------------------------------------------
int CvUnit::getNoDefensiveBonusCount() const
{
	VALIDATE_OBJECT
	return m_iNoDefensiveBonusCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeNoDefensiveBonusCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iNoDefensiveBonusCount += iValue;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::isNoCapture() const
{
	VALIDATE_OBJECT
	return getNoCaptureCount() > 0 ? true : false;
}

//	--------------------------------------------------------------------------------
int CvUnit::getNoCaptureCount() const
{
	VALIDATE_OBJECT
	return m_iNoCaptureCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeNoCaptureCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iNoCaptureCount += iValue;
	}
}


//	--------------------------------------------------------------------------------
bool CvUnit::isNeverInvisible() const
{
	VALIDATE_OBJECT
	return (getInvisibleType() == NO_INVISIBLE);
}


//	--------------------------------------------------------------------------------
bool CvUnit::isInvisible(TeamTypes eTeam, bool bDebug, bool bCheckCargo) const
{
	VALIDATE_OBJECT
	if(bDebug && GC.getGame().isDebugMode())
	{
		return false;
	}

	if(GET_PLAYER(getOwner()).getTeam() == eTeam)
	{
		return false;
	}

	if(bCheckCargo)
	{
		if((m_transportUnit.eOwner >= 0) && m_transportUnit.eOwner < MAX_PLAYERS)
		{
			if(GET_PLAYER((PlayerTypes)m_transportUnit.eOwner).getUnit(m_transportUnit.iID))
			{
				return true;
			}
		}
	}

	if(m_eInvisibleType == NO_INVISIBLE)
	{
		return false;
	}

	return !(plot()->isInvisibleVisible(eTeam, getInvisibleType()));
}

//	--------------------------------------------------------------------------------
bool CvUnit::isNukeImmune() const
{
	VALIDATE_OBJECT
	return (getNukeImmuneCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvUnit::changeNukeImmuneCount(int iValue)
{
	VALIDATE_OBJECT
	m_iNukeImmuneCount += iValue;
	CvAssert(getNukeImmuneCount() >= 0);

}

//	--------------------------------------------------------------------------------
int CvUnit::getNukeImmuneCount() const
{
	VALIDATE_OBJECT
	return m_iNukeImmuneCount;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isHiddenNationality() const
{
	VALIDATE_OBJECT
	return (getHiddenNationalityCount() > 0);
}

//	--------------------------------------------------------------------------------
void CvUnit::changeHiddenNationalityCount(int iValue)
{
	VALIDATE_OBJECT
	m_iHiddenNationalityCount += iValue;
	CvAssert(getHiddenNationalityCount() >= 0);

}

//	--------------------------------------------------------------------------------
int CvUnit::getHiddenNationalityCount() const
{
	VALIDATE_OBJECT
	return m_iHiddenNationalityCount;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isNoRevealMap() const
{
	VALIDATE_OBJECT
	return (getNoRevealMapCount() > 0);
}

//	--------------------------------------------------------------------------------
void CvUnit::changeNoRevealMapCount(int iValue)
{
	VALIDATE_OBJECT
	m_iNoRevealMapCount += iValue;
	CvAssert(getNoRevealMapCount() >= 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getNoRevealMapCount() const
{
	VALIDATE_OBJECT
	return m_iNoRevealMapCount;
}

//	--------------------------------------------------------------------------------
int CvUnit::maxInterceptionProbability() const
{
	VALIDATE_OBJECT
	return std::max(0, getExtraIntercept());
}


//	--------------------------------------------------------------------------------
int CvUnit::currInterceptionProbability() const
{
	VALIDATE_OBJECT
	if(getDomainType() != DOMAIN_AIR)
	{
		return maxInterceptionProbability();
	}
	else
	{
		return ((maxInterceptionProbability() * GetCurrHitPoints()) / GetMaxHitPoints());
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::evasionProbability() const
{
	VALIDATE_OBJECT
	return std::max(0, getExtraEvasion());
}


//	--------------------------------------------------------------------------------
int CvUnit::withdrawalProbability() const
{
	VALIDATE_OBJECT
	return std::max(0, getExtraWithdrawal());
}

//	--------------------------------------------------------------------------------
/// How many enemy Units are adjacent to this guy?
int CvUnit::GetNumEnemyUnitsAdjacent(const CvUnit* pUnitToExclude) const
{
	return plot()->GetNumEnemyUnitsAdjacent( getTeam(), getDomainType(), pUnitToExclude);
}

//	--------------------------------------------------------------------------------
/// Is a particular enemy city next to us?
bool CvUnit::IsEnemyCityAdjacent(const CvCity* pSpecifyCity) const
{
	CvAssertMsg(pSpecifyCity, "City is NULL when checking if it is adjacent to a unit");
	return plot()->IsEnemyCityAdjacent(getTeam(), pSpecifyCity);
}

//	--------------------------------------------------------------------------------
int CvUnit::GetNumSpecificPlayerUnitsAdjacent(const CvUnit* pUnitToExclude, const CvUnit* pExampleUnitType, bool bCombatOnly) const
{
	return plot()->GetNumSpecificPlayerUnitsAdjacent(getOwner(), pUnitToExclude, pExampleUnitType, bCombatOnly);
}

//	--------------------------------------------------------------------------------
/// Is there a friendly Unit adjacent to us?
bool CvUnit::IsFriendlyUnitAdjacent(bool bCombatUnit) const
{
	return plot()->IsFriendlyUnitAdjacent(getTeam(), bCombatUnit);
}

//	--------------------------------------------------------------------------------
int CvUnit::GetAdjacentModifier() const
{
	VALIDATE_OBJECT
	return m_iAdjacentModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeAdjacentModifier(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iAdjacentModifier += iValue;
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::GetRangedAttackModifier() const
{
	VALIDATE_OBJECT
	return m_iRangedAttackModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeRangedAttackModifier(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iRangedAttackModifier += iValue;
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::GetInterceptionCombatModifier() const
{
	VALIDATE_OBJECT
	return m_iInterceptionCombatModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeInterceptionCombatModifier(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iInterceptionCombatModifier += iValue;
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::GetInterceptionDefenseDamageModifier() const
{
	VALIDATE_OBJECT
	return m_iInterceptionDefenseDamageModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeInterceptionDefenseDamageModifier(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iInterceptionDefenseDamageModifier += iValue;
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::GetAirSweepCombatModifier() const
{
	VALIDATE_OBJECT
	return m_iAirSweepCombatModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeAirSweepCombatModifier(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iAirSweepCombatModifier += iValue;
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::getAttackModifier() const
{
	VALIDATE_OBJECT
	return m_iAttackModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeAttackModifier(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iAttackModifier += iValue;
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getDefenseModifier() const
{
	VALIDATE_OBJECT
	return m_iDefenseModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeDefenseModifier(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iDefenseModifier += iValue;
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::cityAttackModifier() const
{
	VALIDATE_OBJECT
	return (getExtraCityAttackPercent());
}


//	--------------------------------------------------------------------------------
int CvUnit::cityDefenseModifier() const
{
	VALIDATE_OBJECT
	return (getExtraCityDefensePercent());
}


//	--------------------------------------------------------------------------------
int CvUnit::rangedDefenseModifier() const
{
	VALIDATE_OBJECT
	return (getExtraRangedDefenseModifier());
}


//	--------------------------------------------------------------------------------
int CvUnit::hillsAttackModifier() const
{
	VALIDATE_OBJECT
	return (getExtraHillsAttackPercent());
}


//	--------------------------------------------------------------------------------
int CvUnit::hillsDefenseModifier() const
{
	VALIDATE_OBJECT
	return (getExtraHillsDefensePercent());
}

//	--------------------------------------------------------------------------------
int CvUnit::openAttackModifier() const
{
	VALIDATE_OBJECT
	return (getExtraOpenAttackPercent());
}

//	--------------------------------------------------------------------------------
int CvUnit::openRangedAttackModifier() const
{
	VALIDATE_OBJECT
	return (getExtraOpenRangedAttackMod());
}

//	--------------------------------------------------------------------------------
int CvUnit::roughAttackModifier() const
{
	VALIDATE_OBJECT
	return (getExtraRoughAttackPercent());
}

//	--------------------------------------------------------------------------------
int CvUnit::roughRangedAttackModifier() const
{
	VALIDATE_OBJECT
	return (getExtraRoughRangedAttackMod());
}

//	--------------------------------------------------------------------------------
int CvUnit::attackFortifiedModifier() const
{
	VALIDATE_OBJECT
	return (getExtraAttackFortifiedMod());
}

//	--------------------------------------------------------------------------------
int CvUnit::attackWoundedModifier() const
{
	VALIDATE_OBJECT
	return (getExtraAttackWoundedMod());
}

//	--------------------------------------------------------------------------------
int CvUnit::openDefenseModifier() const
{
	VALIDATE_OBJECT
	return (getExtraOpenDefensePercent());
}

//	--------------------------------------------------------------------------------
int CvUnit::roughDefenseModifier() const
{
	VALIDATE_OBJECT
	return (getExtraRoughDefensePercent());
}

//	--------------------------------------------------------------------------------
int CvUnit::terrainAttackModifier(TerrainTypes eTerrain) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eTerrain >= 0, "eTerrain is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTerrain < GC.getNumTerrainInfos(), "eTerrain is expected to be within maximum bounds (invalid Index)");
	return (getExtraTerrainAttackPercent(eTerrain));
}


//	--------------------------------------------------------------------------------
int CvUnit::terrainDefenseModifier(TerrainTypes eTerrain) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eTerrain >= 0, "eTerrain is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTerrain < GC.getNumTerrainInfos(), "eTerrain is expected to be within maximum bounds (invalid Index)");
	return (getExtraTerrainDefensePercent(eTerrain));
}


//	--------------------------------------------------------------------------------
int CvUnit::featureAttackModifier(FeatureTypes eFeature) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eFeature >= 0, "eFeature is expected to be non-negative (invalid Index)");
	CvAssertMsg(eFeature < GC.getNumFeatureInfos(), "eFeature is expected to be within maximum bounds (invalid Index)");
	return (getExtraFeatureAttackPercent(eFeature));
}

//	--------------------------------------------------------------------------------
int CvUnit::featureDefenseModifier(FeatureTypes eFeature) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eFeature >= 0, "eFeature is expected to be non-negative (invalid Index)");
	CvAssertMsg(eFeature < GC.getNumFeatureInfos(), "eFeature is expected to be within maximum bounds (invalid Index)");
	return (getExtraFeatureDefensePercent(eFeature));
}

//	--------------------------------------------------------------------------------
int CvUnit::unitClassAttackModifier(UnitClassTypes eUnitClass) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eUnitClass >= 0, "eUnitClass is expected to be non-negative (invalid Index)");
	CvAssertMsg(eUnitClass < GC.getNumUnitClassInfos(), "eUnitClass is expected to be within maximum bounds (invalid Index)");
	return m_Promotions.GetUnitClassAttackMod(eUnitClass);
}


//	--------------------------------------------------------------------------------
int CvUnit::unitClassDefenseModifier(UnitClassTypes eUnitClass) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eUnitClass >= 0, "eUnitClass is expected to be non-negative (invalid Index)");
	CvAssertMsg(eUnitClass < GC.getNumUnitClassInfos(), "eUnitClass is expected to be within maximum bounds (invalid Index)");
	return m_Promotions.GetUnitClassDefenseMod(eUnitClass);
}


//	--------------------------------------------------------------------------------
int CvUnit::unitCombatModifier(UnitCombatTypes eUnitCombat) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eUnitCombat >= 0, "eUnitCombat is expected to be non-negative (invalid Index)");
	CvAssertMsg(eUnitCombat < GC.getNumUnitCombatClassInfos(), "eUnitCombat is expected to be within maximum bounds (invalid Index)");
	return (getExtraUnitCombatModifier(eUnitCombat));
}


//	--------------------------------------------------------------------------------
int CvUnit::domainModifier(DomainTypes eDomain) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eDomain >= 0, "eDomain is expected to be non-negative (invalid Index)");
	CvAssertMsg(eDomain < NUM_DOMAIN_TYPES, "eDomain is expected to be within maximum bounds (invalid Index)");
	return (getExtraDomainModifier(eDomain));
}


//	--------------------------------------------------------------------------------
SpecialUnitTypes CvUnit::specialCargo() const
{
	VALIDATE_OBJECT
	return ((SpecialUnitTypes)(m_pUnitInfo->GetSpecialCargo()));
}


//	--------------------------------------------------------------------------------
DomainTypes CvUnit::domainCargo() const
{
	VALIDATE_OBJECT
	return ((DomainTypes)(m_pUnitInfo->GetDomainCargo()));
}


//	--------------------------------------------------------------------------------
int CvUnit::cargoSpace() const
{
	VALIDATE_OBJECT
	return m_iCargoCapacity;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeCargoSpace(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iCargoCapacity += iChange;
		CvAssert(m_iCargoCapacity >= 0);
		setInfoBarDirty(true);
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::isFull() const
{
	VALIDATE_OBJECT
	return (getCargo() >= cargoSpace());
}


//	--------------------------------------------------------------------------------
int CvUnit::cargoSpaceAvailable(SpecialUnitTypes eSpecialCargo, DomainTypes eDomainCargo) const
{
	VALIDATE_OBJECT
	if(specialCargo() != NO_SPECIALUNIT)
	{
		if(specialCargo() != eSpecialCargo)
		{
			return 0;
		}
	}

	if(domainCargo() != NO_DOMAIN)
	{
		if(domainCargo() != eDomainCargo)
		{
			return 0;
		}
	}

	return std::max(0, (cargoSpace() - getCargo()));
}


//	--------------------------------------------------------------------------------
bool CvUnit::hasCargo() const
{
	VALIDATE_OBJECT
	return (getCargo() > 0);
}


//	--------------------------------------------------------------------------------
bool CvUnit::canCargoAllMove() const
{
	VALIDATE_OBJECT
	const IDInfo* pUnitNode;
	const CvUnit* pLoopUnit;
	CvPlot* pPlot;

	pPlot = plot();

	pUnitNode = pPlot->headUnitNode();

	while(pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if(pLoopUnit && pLoopUnit->getTransportUnit() == this)
		{
			if(pLoopUnit->getDomainType() == DOMAIN_LAND)
			{
				if(!(pLoopUnit->canMove()))
				{
					return false;
				}
			}
		}
	}

	return true;
}


//	--------------------------------------------------------------------------------
int CvUnit::getUnitAICargo(UnitAITypes eUnitAI) const
{
	VALIDATE_OBJECT
	const IDInfo* pUnitNode;
	const CvUnit* pLoopUnit;
	CvPlot* pPlot;
	int iCount;

	iCount = 0;

	pPlot = plot();

	pUnitNode = pPlot->headUnitNode();

	while(pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if(pLoopUnit && pLoopUnit->getTransportUnit() == this)
		{
			if(pLoopUnit->AI_getUnitAIType() == eUnitAI)
			{
				iCount++;
			}
		}
	}

	return iCount;
}

#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
bool CvUnit::isAircraftCarrier() const
{
	if(cargoSpace() > 0 && getDomainType()==DOMAIN_SEA && domainCargo() == DOMAIN_AIR)
	{
		SpecialUnitTypes eSpecialUnitPlane = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_FIGHTER");
		SpecialUnitTypes eSpecialUnitMissile = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_MISSILE");
		if(specialCargo() == eSpecialUnitPlane || specialCargo() == eSpecialUnitMissile)
		{
			return true;
		}
	}

	return false;
}
#endif
//	--------------------------------------------------------------------------------
bool CvUnit::IsHasNoValidMove() const
{
#if defined(MOD_GLOBAL_STACKING_RULES)
	if(plot()->getNumFriendlyUnitsOfType(this) <= plot()->getUnitLimit())
#else
	if(plot()->getNumFriendlyUnitsOfType(this) <= GC.getPLOT_UNIT_LIMIT())
#endif
	{
		return false;
	}

	TacticalAIHelpers::ReachablePlotSet plots;
	TacticalAIHelpers::GetAllPlotsInReach(this,plot(),plots,true,true,false);

	for (TacticalAIHelpers::ReachablePlotSet::const_iterator it=plots.begin(); it!=plots.end(); ++it)
	{
		CvPlot* pToPlot = GC.getMap().plotByIndexUnchecked(it->first);

	#if defined(MOD_GLOBAL_STACKING_RULES)
		if(pToPlot->getNumFriendlyUnitsOfType(this) >= pToPlot->getUnitLimit())
	#else
		if(pToPlot->getNumFriendlyUnitsOfType(pUnit) >= GC.getPLOT_UNIT_LIMIT())
	#endif
		{
			continue;
		}

		if(pToPlot->getNumVisibleEnemyDefenders(this) > 0)
		{
			continue;
		}

		// can't capture the unit with a non-combat unit
		if(!IsCombatUnit() && pToPlot->isVisibleEnemyUnit(this))
		{
			continue;
		}

		//if we get here the plot is valid
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
int CvUnit::getIndex() const
{
	VALIDATE_OBJECT
	return GetID();
}


//	--------------------------------------------------------------------------------
IDInfo CvUnit::GetIDInfo() const
{
	VALIDATE_OBJECT
	IDInfo unit(getOwner(), GetID());
	return unit;
}


//	--------------------------------------------------------------------------------
void CvUnit::SetID(int iID)
{
	VALIDATE_OBJECT
	m_iID = iID;
}


//	--------------------------------------------------------------------------------
int CvUnit::getHotKeyNumber()
{
	VALIDATE_OBJECT
	return m_iHotKeyNumber;
}


//	--------------------------------------------------------------------------------
void CvUnit::setHotKeyNumber(int iNewValue)
{
	VALIDATE_OBJECT
	CvUnit* pLoopUnit;
	int iLoop;

	CvAssert(getOwner() != NO_PLAYER);

	if(getHotKeyNumber() != iNewValue)
	{
		if(iNewValue != -1)
		{
			for(pLoopUnit = GET_PLAYER(getOwner()).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(getOwner()).nextUnit(&iLoop))
			{
				if(pLoopUnit->getHotKeyNumber() == iNewValue)
				{
					pLoopUnit->setHotKeyNumber(-1);
				}
			}
		}

		m_iHotKeyNumber = iNewValue;
	}
}

//	-----------------------------------------------------------------------------------------------------------
void CvUnit::setXY(int iX, int iY, bool bGroup, bool bUpdate, bool bShow, bool bCheckPlotVisible, bool bNoMove)
{
	VALIDATE_OBJECT
	IDInfo* pUnitNode = 0;
	CvCity* pOldCity = 0;
	CvCity* pNewCity = 0;
	CvUnit* pTransportUnit = 0;
	CvUnit* pLoopUnit = 0;
	CvPlot* pOldPlot = 0;
	CvPlot* pNewPlot = 0;
	FStaticVector<IDInfo, 10, true, c_eCiv5GameplayDLL, 0> oldUnitList;
	FStaticVector<CvUnitCaptureDefinition, 8, true, c_eCiv5GameplayDLL, 0> kCaptureUnitList;
	ActivityTypes eOldActivityType = NO_ACTIVITY;
	int iI;
	TeamTypes activeTeam = GC.getGame().getActiveTeam();
	TeamTypes eOurTeam = getTeam();

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	bool bOwnerIsActivePlayer = GC.getGame().getActivePlayer() == getOwner();
	// Delay any popups that might be caused by our movement (goody huts, natural wonders, etc.) so the unit movement event gets sent before the popup event.
	if (bOwnerIsActivePlayer)
		DLLUI->SetDontShowPopups(true);

	CvAssert(!at(iX, iY));
	CvAssert(!isFighting());
	CvAssert((iX == INVALID_PLOT_COORD) || (GC.getMap().plot(iX, iY)->getX() == iX));
	CvAssert((iY == INVALID_PLOT_COORD) || (GC.getMap().plot(iX, iY)->getY() == iY));

	eOldActivityType = GetActivityType();

	if(isSetUpForRangedAttack())
	{
		setSetUpForRangedAttack(false);
	}

	if(!bGroup || isCargo())
	{
		bShow = false;
	}

	int iMapLayer = m_iMapLayer;

	pNewPlot = GC.getMap().plot(iX, iY);

	//sanity check - may interfere with carriers for land units, so don't fix it
	/*
	if (pNewPlot)
	{
		if ( !pNewPlot->needsEmbarkation(this) && isEmbarked() )
			setEmbarked(false);
		if ( pNewPlot->needsEmbarkation(this) && CanEverEmbark() && !isEmbarked() )
			setEmbarked(true);
	}
	*/

	if(pNewPlot != NULL && !bNoMove)
	{
		pTransportUnit = getTransportUnit();

		if(pTransportUnit != NULL)
		{
			if(!(pTransportUnit->atPlot(*pNewPlot)))
			{
				setTransportUnit(NULL);
			}
		}

		if(IsCombatUnit())
		{
			oldUnitList.clear();

			pUnitNode = pNewPlot->headUnitNode();

			while(pUnitNode != NULL)
			{
				oldUnitList.push_back(*pUnitNode);
				pUnitNode = pNewPlot->nextUnitNode(pUnitNode);
			}

			int iUnitListSize = (int) oldUnitList.size();
			for(int iVectorLoop = 0; iVectorLoop < (int) iUnitListSize; ++iVectorLoop)
			{
				pLoopUnit = ::getUnit(oldUnitList[iVectorLoop]);
				{
					if(pLoopUnit && !pLoopUnit->isDelayedDeath())
					{
						if(isEnemy(pLoopUnit->getTeam(), pNewPlot) || pLoopUnit->isEnemy(eOurTeam))
						{
							if(!pLoopUnit->canCoexistWithEnemyUnit(eOurTeam))
							{
								// Unit somehow ended up on top of an enemy combat unit
								if(NO_UNITCLASS == pLoopUnit->getUnitInfo().GetUnitCaptureClassType() && pLoopUnit->IsCanDefend(pNewPlot))
								{
									if(!pNewPlot->isCity())
									{
										if (!pLoopUnit->jumpToNearestValidPlot())
											pLoopUnit->kill(false, getOwner());
									}
									else
									{
#if defined(MOD_API_UNIFIED_YIELDS)
										kPlayer.DoYieldsFromKill(this, pLoopUnit, iX, iY, 0);
#else
										kPlayer.DoYieldsFromKill(getUnitType(), pLoopUnit->getUnitType(), iX, iY, pLoopUnit->isBarbarian(), 0);
#endif
										pLoopUnit->kill(false, getOwner());
									}
								}
								// Ran into a noncombat unit
								else
								{
									bool bDisplaced = false;
									ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
									if(pkScriptSystem)
									{
										CvLuaArgsHandle args;
										args->Push(pLoopUnit->getOwner());
										args->Push(pLoopUnit->GetID());  // captured unit

										bool bResult = false;
										if (LuaSupport::CallTestAny(pkScriptSystem, "CanDisplaceCivilian", args.get(), bResult))
										{
											// Check the result.
											if(bResult)
											{
												bDisplaced = true;
											}
										}
									}

									if (!bDisplaced)
									{
										Localization::String strMessage;
										Localization::String strSummary;

										bool bDoCapture = false;
#if defined(MOD_BALANCE_CORE)
										bool bDoEvade = false;
										if(pLoopUnit->IsCivilianUnit() && pLoopUnit->getExtraWithdrawal() > 0 && pLoopUnit->CanWithdrawFromMelee(*this))
										{
											bDoEvade = true;
											pLoopUnit->DoWithdrawFromMelee(*this);
											strMessage = Localization::Lookup("TXT_KEY_UNIT_WITHDREW_DETAILED_PIONEER");
											strMessage << pLoopUnit->getUnitInfo().GetTextKey();
											strSummary = Localization::Lookup("TXT_KEY_UNIT_WITHDREW_PIONEER");

											CvNotifications* pNotification = GET_PLAYER(pLoopUnit->getOwner()).GetNotifications();
											if(pNotification)
											pNotification->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pLoopUnit->getX(), pLoopUnit->getY(), (int) pLoopUnit->getUnitType(), pLoopUnit->getOwner());

										}
#endif
										// Some units can't capture civilians. Embarked units are also not captured, they're simply killed. And some aren't a type that gets captured.
										// slewis - removed the capture clause so that helicopter gunships could capture workers. The promotion says that No Capture only effects cities.
										//if(!isNoCapture() && (!pLoopUnit->isEmbarked() || pLoopUnit->getUnitInfo().IsCaptureWhileEmbarked()) && pLoopUnit->getCaptureUnitType(GET_PLAYER(pLoopUnit->getOwner()).getCivilizationType()) != NO_UNIT)
										if((!pLoopUnit->isEmbarked() || pLoopUnit->getUnitInfo().IsCaptureWhileEmbarked()) && pLoopUnit->getCaptureUnitType(GET_PLAYER(pLoopUnit->getOwner()).getCivilizationType()) != NO_UNIT
#if defined(MOD_BALANCE_CORE)
&& !bDoEvade
#endif
											)
										{
											bDoCapture = true;

											if(isBarbarian())
											{
												strMessage = Localization::Lookup("TXT_KEY_UNIT_CAPTURED_BARBS_DETAILED");
												strMessage << pLoopUnit->getUnitInfo().GetTextKey();
												strSummary = Localization::Lookup("TXT_KEY_UNIT_CAPTURED_BARBS");
											}
											else
											{
												strMessage = Localization::Lookup("TXT_KEY_UNIT_CAPTURED_DETAILED");
												strMessage << pLoopUnit->getUnitInfo().GetTextKey() << GET_PLAYER(getOwner()).getNameKey();
												strSummary = Localization::Lookup("TXT_KEY_UNIT_CAPTURED");
											}
										}
										// Unit was killed instead
#if defined(MOD_EVENTS_UNIT_CAPTURE)

										if (MOD_EVENTS_UNIT_CAPTURE) {
											GAMEEVENTINVOKE_HOOK(GAMEEVENT_UnitCaptured, getOwner(), GetID(), pLoopUnit->getOwner(), pLoopUnit->GetID(), !bDoCapture, 0);
										}
#endif
										else
#if defined(MOD_BALANCE_CORE)
if (!bDoEvade)
#endif
										{
											if(pLoopUnit->isEmbarked())
												changeExperience(1);

											CvString strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_DESTROYED_ENEMY", getNameKey(), 0, pLoopUnit->getNameKey());
											DLLUI->AddUnitMessage(0, GetIDInfo(), getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);

											strMessage = Localization::Lookup("TXT_KEY_UNIT_LOST");
											strSummary = strMessage;

#if defined(MOD_API_UNIFIED_YIELDS)
											kPlayer.DoYieldsFromKill(this, pLoopUnit, iX, iY, 0);
#else
											kPlayer.DoYieldsFromKill(getUnitType(), pLoopUnit->getUnitType(), iX, iY, pLoopUnit->isBarbarian(), 0);
#endif
#if defined(MOD_API_EXTENSIONS)
											kPlayer.DoUnitKilledCombat(this, pLoopUnit->getOwner(), pLoopUnit->getUnitType());
#else
											kPlayer.DoUnitKilledCombat(pLoopUnit->getOwner(), pLoopUnit->getUnitType());
#endif
										}
#if defined(MOD_BALANCE_CORE)
										if (!bDoEvade)
										{
#endif
										CvNotifications* pNotification = GET_PLAYER(pLoopUnit->getOwner()).GetNotifications();
										if(pNotification)
											pNotification->Add(NOTIFICATION_UNIT_DIED, strMessage.toUTF8(), strSummary.toUTF8(), pLoopUnit->getX(), pLoopUnit->getY(), (int) pLoopUnit->getUnitType(), pLoopUnit->getOwner());

										if(pLoopUnit->isEmbarked())
											setMadeAttack(true);

										// If we're capturing the unit, we want to delay the capture, else as the unit is converted to our side, it will be the first unit on our
										// side in the plot and can end up taking over a city, rather than the advancing unit
										CvUnitCaptureDefinition kCaptureDef;
										if(bDoCapture)
										{
											if(pLoopUnit->getCaptureDefinition(&kCaptureDef, getOwner()))
												kCaptureUnitList.push_back(kCaptureDef);
											pLoopUnit->setCapturingPlayer(NO_PLAYER);	// Make absolutely sure this is not valid so the kill does not do the capture.
										}

										pLoopUnit->kill(false, getOwner());
#if defined(MOD_BALANCE_CORE)
										}
#endif
									}
								}
							}
						}
					}
				}
			}
		}
	}

	pOldPlot = plot();

	if(pOldPlot != NULL)
	{
		pOldPlot->removeUnit(this, bUpdate);

		if (iMapLayer == DEFAULT_UNIT_MAP_LAYER)
		{
			// if leaving a city, reveal the unit
			if(pOldPlot->isCity())
			{
				// if pNewPlot is a valid pointer, we are leaving the city and need to visible
				// if pNewPlot is NULL than we are "dead" (e.g. a settler) and need to blend out
				auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
				gDLL->GameplayUnitVisibility(pDllUnit.get(), pNewPlot != NULL && !this->isInvisible(activeTeam, false));
			}

			if (canChangeVisibility())
				pOldPlot->changeAdjacentSight(eOurTeam, visibilityRange(), false, getSeeInvisibleType(), getFacingDirection(true));

			pOldPlot->area()->changeUnitsPerPlayer(getOwner(), -1);

			setLastMoveTurn(GC.getGame().getTurnSlice());

			pOldCity = pOldPlot->getPlotCity();
		}
	}

	if(pNewPlot != NULL)
	{
		m_iX = pNewPlot->getX();
		m_iY = pNewPlot->getY();
	}
	else
	{
		m_iX = INVALID_PLOT_COORD;
		m_iY = INVALID_PLOT_COORD;
	}

	CvAssertMsg(plot() == pNewPlot, "plot is expected to equal pNewPlot");

	if(pNewPlot != NULL)
	{
		//update facing direction
		if(pOldPlot != NULL)
		{
			DirectionTypes newDirection = directionXY(pOldPlot, pNewPlot);
			if(newDirection != NO_DIRECTION)
				m_eFacingDirection = newDirection;
		}

		//update cargo mission animations
		if(isCargo())
		{
			if(eOldActivityType != ACTIVITY_MISSION)
			{
				SetActivityType(eOldActivityType);
			}
		}

		// if entering a city, hide the unit
		if(pNewPlot->isCity() && iMapLayer == DEFAULT_UNIT_MAP_LAYER)
		{
			auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
			gDLL->GameplayUnitVisibility(pDllUnit.get(), false /*bVisible*/);
		}

		setFortifyTurns(0);

		if (iMapLayer == DEFAULT_UNIT_MAP_LAYER)
		{
			if (canChangeVisibility())
#if defined(MOD_BALANCE_CORE)
				pNewPlot->changeAdjacentSight(eOurTeam, visibilityRange(), true, getSeeInvisibleType(), getFacingDirection(true), true, this); // needs to be here so that the square is considered visible when we move into it...
#else
				pNewPlot->changeAdjacentSight(eOurTeam, visibilityRange(), true, getSeeInvisibleType(), getFacingDirection(true)); // needs to be here so that the square is considered visible when we move into it...
#endif

			pNewPlot->addUnit(this, bUpdate);

			pNewPlot->area()->changeUnitsPerPlayer(getOwner(), 1);
			pNewCity = pNewPlot->getPlotCity();
		}
		else
		{
			GC.getMap().plotManager().AddUnit(GetIDInfo(), iX, iY, iMapLayer);
		}
#if defined(MOD_BALANCE_CORE)
		bool bScout = false;
		for(iI = 0; iI < GC.getNumPromotionInfos(); iI++)
		{
			const PromotionTypes ePromotion = static_cast<PromotionTypes>(iI);
			CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
			if(pkPromotionInfo)
			{
				if(!bScout && isHasPromotion(ePromotion) && pkPromotionInfo->IsGainsXPFromScouting())
				{
					bScout = true;
					break;
				}
			}
		}
		if(bScout && GC.getGame().getActivePlayer() == getOwner())
		{
			if(getExperience() <= GC.getBALANCE_SCOUT_XP_MAXIMUM())
			{
				int iExperience = GC.getBALANCE_SCOUT_XP_BASE();
				iExperience += GetNumTilesRevealedThisTurn();
				iExperience /= 6;
				if(iExperience > 0)
				{
					//Up to max barb value - rest has to come through combat!
					changeExperience(iExperience);
				}
			}
		}
#endif
		// Moving into a City (friend or foe)
		if(pNewCity != NULL)
		{
			if(isEnemy(pNewCity->getTeam()) && !canCoexistWithEnemyUnit(pNewCity->getTeam()))
			{
				PlayerTypes eNewOwner = GET_PLAYER(getOwner()).pickConqueredCityOwner(*pNewCity);

				if(NO_PLAYER != eNewOwner)
				{
					
#if !defined(NO_ACHIEVEMENTS)
					//Test for City Achievements here since we need to know about the capturing unit.
					if(getOwner() == GC.getGame().getActivePlayer())
					{
						//Expansion 1 - Capture the city that built Petra using a Landship
						if(strcmp(getUnitInfo().GetType(), "UNIT_WWI_TANK") == 0)
						{
							const int numBuildings = GC.getNumBuildingInfos();
							for(int i = 0; i < numBuildings; ++i)
							{
								const BuildingTypes eBuilding = static_cast<BuildingTypes>(i);
								const CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

								if(pkBuildingInfo != NULL && strcmp(pkBuildingInfo->GetType(), "BUILDING_PETRA") == 0)
								{
									if(pNewCity->GetCityBuildings()->GetNumRealBuilding(eBuilding) > 0)
									{
										gDLL->UnlockAchievement(ACHIEVEMENT_XP1_26);
									}
									break;
								}
							}
						}

						//Expansion 1 - Capture a Spanish Coastal City with a Dutch Sea Begger
						if(strcmp(getUnitInfo().GetType(), "UNIT_DUTCH_SEA_BEGGAR") == 0)
						{
							if(pNewCity->isCoastal())
							{
								if(strcmp(pNewCity->getCivilizationInfo().GetType(), "CIVILIZATION_SPAIN") == 0)
								{
									gDLL->UnlockAchievement(ACHIEVEMENT_XP1_28);
								}
							}
						}

						// Expansion 2 - Capture Venice's holy city as Venice
						// if venice
						//   find venice religion
						//   compare religion to if city is holy city of conquered
						if (!GC.getGame().isGameMultiPlayer())
						{
							if(strcmp(getCivilizationInfo().GetType(), "CIVILIZATION_VENICE") == 0)
							{
								CvCity* pCity = GET_PLAYER(m_eOwner).getCapitalCity();
								ReligionTypes eReligion = pCity->GetCityReligions()->GetReligiousMajority();
								if (eReligion != NO_RELIGION)
								{
									if (pNewCity->GetCityReligions()->IsHolyCityForReligion(eReligion))
									{
										gDLL->UnlockAchievement(ACHIEVEMENT_XP2_26);
									}
								}
							}
						}
					}

					bool bUsingXP1Scenario2 = gDLL->IsModActivated(CIV5_XP1_SCENARIO2_MODID);
					if(bUsingXP1Scenario2)
					{
						if(strcmp(getCivilizationInfo().GetType(), "CIVILIZATION_SONGHAI") == 0)
						{
							const int iRomeX = 33;
							const int iRomeY = 31;

							//Did we, as vandals, just capture Rome with a boat?
							if(pNewCity->getX() == iRomeX && pNewCity->getY() == iRomeY && getDomainType() == DOMAIN_SEA)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP1_52);
							}
						}
					}
#endif

					GET_PLAYER(eNewOwner).acquireCity(pNewCity, true, false); // will delete the pointer
					pNewCity = NULL;
				}
			}
		}

		if(shouldLoadOnMove(pNewPlot))
		{
			load();
		}

		// Can someone can see the plot we moved our Unit into?
		TeamTypes eTeamLoop;
		for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			eTeamLoop = (TeamTypes) iI;

			CvTeam& kLoopTeam = GET_TEAM(eTeamLoop);

			if(kLoopTeam.isAlive())
			{
				// Human can't be met by an AI spotting him.  If both players are human then they don't need to see one another
				if(!isHuman() || kLoopTeam.isHuman())
				{
					if(!isInvisible(eTeamLoop, false))
					{
						if(pNewPlot->isVisible(eTeamLoop))
						{
							kLoopTeam.meet(eOurTeam, false);
						}
					}
				}
			}
		}

		// if I was invisible to the active team but won't be any more or vice versa
		InvisibleTypes eInvisoType = getInvisibleType();
		if(eOurTeam != activeTeam && eInvisoType != NO_INVISIBLE)
		{
			bool bOldInvisibleVisible = false;
			if(pOldPlot)
				bOldInvisibleVisible = pOldPlot->isInvisibleVisible(activeTeam, eInvisoType);
			bool bNewInvisibleVisible = pNewPlot->isInvisibleVisible(activeTeam, eInvisoType);
			if(bOldInvisibleVisible != bNewInvisibleVisible)
			{
				auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
				gDLL->GameplayUnitVisibility(pDllUnit.get(), bNewInvisibleVisible, true);
			}
		}

		CvTeam& kOurTeam = GET_TEAM(eOurTeam);

		// If a Unit is adjacent to someone's borders, meet them
		CvPlot* pAdjacentPlot;
		for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(pNewPlot->getX(), pNewPlot->getY(), ((DirectionTypes)iI));

			if(pAdjacentPlot != NULL)
			{
				// Owned by someone
				if(pAdjacentPlot->getTeam() != NO_TEAM)
				{
					kOurTeam.meet(pAdjacentPlot->getTeam(), false);
				}

				// Have a naval unit here?
				if(isBarbarian() && getDomainType() == DOMAIN_SEA && pAdjacentPlot->isWater())
				{
					UnitHandle pAdjacentUnit = pAdjacentPlot->getBestDefender(NO_PLAYER, BARBARIAN_PLAYER, NULL, true);
					if(pAdjacentUnit)
					{
						GET_PLAYER(pAdjacentUnit->getOwner()).GetPlayerTraits()->CheckForBarbarianConversion(this, pAdjacentPlot);
					}
				}

				// Is this a missionary and do we have a belief that converts barbarians here?
				UnitClassTypes eMissionary = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_MISSIONARY", true);
				if (eMissionary != NO_UNITCLASS)
				{
					if (getUnitClassType() == eMissionary)
					{
						ReligionTypes eFoundedReligion = kPlayer.GetReligions()->GetReligionCreatedByPlayer();
						const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eFoundedReligion, kPlayer.GetID());
						if (pReligion && pReligion->m_Beliefs.IsConvertsBarbarians())
						{
							UnitHandle pAdjacentUnit = pAdjacentPlot->getBestDefender(BARBARIAN_PLAYER);
							if(pAdjacentUnit)
							{
#if defined(MOD_EVENTS_UNIT_CAPTURE)
								CvBeliefHelpers::ConvertBarbarianUnit(this, pAdjacentUnit);
#else
								CvBeliefHelpers::ConvertBarbarianUnit(&kPlayer, pAdjacentUnit);
#endif
							}
						}
					}
					// Or other way around, barbarian moving up to a missionary
					else if (isBarbarian())
					{
						if(pAdjacentPlot->getNumUnits() > 0)
						{
							for(int iNearbyUnitLoop = 0; iNearbyUnitLoop < pAdjacentPlot->getNumUnits(); iNearbyUnitLoop++)
							{
								const CvUnit* const adjUnit = pAdjacentPlot->getUnitByIndex(iNearbyUnitLoop);
								if (adjUnit && adjUnit->getUnitClassType() == eMissionary)
								{
									CvPlayer &adjUnitPlayer = GET_PLAYER(adjUnit->getOwner());
									ReligionTypes eFoundedReligion = adjUnitPlayer.GetReligions()->GetReligionCreatedByPlayer();
									const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eFoundedReligion, adjUnitPlayer.GetID());
									if (pReligion && pReligion->m_Beliefs.IsConvertsBarbarians())
									{
#if defined(MOD_EVENTS_UNIT_CAPTURE)
										if (CvBeliefHelpers::ConvertBarbarianUnit(adjUnit, this))
#else
										if(CvBeliefHelpers::ConvertBarbarianUnit(&adjUnitPlayer, this))
#endif
										{
											ClearMissionQueue();
										}
									}
								}
							}
						}
					}
				}

				// Natural wonder that provides free promotions?
				FeatureTypes eFeature = pAdjacentPlot->getFeatureType();
#if defined(MOD_BALANCE_CORE)
				if(eFeature != NO_FEATURE)
#else
				if(eFeature != NO_FEATURE && GC.getFeatureInfo(eFeature)->IsNaturalWonder())
#endif
				{
					PromotionTypes ePromotion = (PromotionTypes)GC.getFeatureInfo(eFeature)->getAdjacentUnitFreePromotion();
					if(ePromotion != NO_PROMOTION)
					{
#if defined(MOD_BALANCE_CORE)
						CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
						if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
						{
							bool bNoPromotion = false;
							// Check for negating promotions
							if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
							{
								bNoPromotion = true;
							}
							if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
							{
								bNoPromotion = true;
							}
							if(!bNoPromotion)
							{
								for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
								{
									const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
									CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
									if(pkPromotionInfo)
									{
										PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
										// Unit has negation promotion
										if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
										{
											bNoPromotion = true;
											break;
										}
									}
								}
							}
							if(!bNoPromotion)
							{
								setHasPromotion(ePromotion, true);
							}
						}
					}
				}
				// Adjacent terrain that provides free promotions?
				TerrainTypes eTerrain = pAdjacentPlot->getTerrainType();
				if(eTerrain != NO_TERRAIN && (eTerrain <= TERRAIN_SNOW))
				{
					PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(eTerrain)->getAdjacentUnitFreePromotion();
					if(ePromotion != NO_PROMOTION)
					{
						CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
						if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
						{
							bool bNoPromotion = false;
							// Check for negating promotions
							if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
							{
								bNoPromotion = true;
							}
							if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
							{
								bNoPromotion = true;
							}
							if(!bNoPromotion)
							{
								for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
								{
									const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
									CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
									if(pkPromotionInfo)
									{
										PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
										// Unit has negation promotion
										if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
										{
											bNoPromotion = true;
											break;
										}
									}
								}
							}
							if(!bNoPromotion)
							{
								setHasPromotion(ePromotion, true);
							}
						}
					}
				}
				// Adjacent terrain that provides free promotions?
				if(pAdjacentPlot->isHills())
				{
					PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_HILL)->getAdjacentUnitFreePromotion();
					if(ePromotion != NO_PROMOTION)
					{
						CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
						if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
						{
							bool bNoPromotion = false;
							// Check for negating promotions
							if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
							{
								bNoPromotion = true;
							}
							if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
							{
								bNoPromotion = true;
							}
							if(!bNoPromotion)
							{
								for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
								{
									const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
									CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
									if(pkPromotionInfo)
									{
										PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
										// Unit has negation promotion
										if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
										{
											bNoPromotion = true;
											break;
										}
									}
								}
							}
							if(!bNoPromotion)
							{
								setHasPromotion(ePromotion, true);
							}
						}
					}
				}
				// Adjacent terrain that provides free promotions?
				if(pAdjacentPlot->isMountain())
				{
					PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_MOUNTAIN)->getAdjacentUnitFreePromotion();
					if(ePromotion != NO_PROMOTION)
					{
						CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
						if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
						{
							bool bNoPromotion = false;
							// Check for negating promotions
							if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
							{
								bNoPromotion = true;
							}
							if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
							{
								bNoPromotion = true;
							}
							if(!bNoPromotion)
							{
								for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
								{
									const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
									CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
									if(pkPromotionInfo)
									{
										PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
										// Unit has negation promotion
										if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
										{
											bNoPromotion = true;
											break;
										}
									}
								}
							}
							if(!bNoPromotion)
							{
								setHasPromotion(ePromotion, true);
							}
						}
					}
				}
				if(pAdjacentPlot->isShallowWater() && !pAdjacentPlot->isLake())
				{
					PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_COAST)->getAdjacentUnitFreePromotion();
					if(ePromotion != NO_PROMOTION)
					{
						CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
						if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
						{
							bool bNoPromotion = false;
							// Check for negating promotions
							if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
							{
								bNoPromotion = true;
							}
							if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
							{
								bNoPromotion = true;
							}
							if(!bNoPromotion)
							{
								for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
								{
									const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
									CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
									if(pkPromotionInfo)
									{
										PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
										// Unit has negation promotion
										if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
										{
											bNoPromotion = true;
											break;
										}
									}
								}
							}
							if(!bNoPromotion)
							{
								setHasPromotion(ePromotion, true);
							}
						}
					}
				}
				if(!pAdjacentPlot->isShallowWater() && !pAdjacentPlot->isLake() && pAdjacentPlot->isWater())
				{
					PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_OCEAN)->getAdjacentUnitFreePromotion();
					if(ePromotion != NO_PROMOTION)
					{
						CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
						if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
						{
							bool bNoPromotion = false;
							// Check for negating promotions
							if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
							{
								bNoPromotion = true;
							}
							if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
							{
								bNoPromotion = true;
							}
							if(!bNoPromotion)
							{
								for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
								{
									const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
									CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
									if(pkPromotionInfo)
									{
										PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
										// Unit has negation promotion
										if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
										{
											bNoPromotion = true;
											break;
										}
									}
								}
							}
							if(!bNoPromotion)
							{
								setHasPromotion(ePromotion, true);
							}
						}
					}
				}
#else
						// Is this a valid Promotion for the UnitCombatType?
						if(m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT &&
								(::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
						{
							setHasPromotion(ePromotion, true);
						}
					}
				}
#endif
			}
		}
#if defined(MOD_BALANCE_CORE)
		// Feature that provides free promotions?
		FeatureTypes eFeature = pNewPlot->getFeatureType();
		if(eFeature != NO_FEATURE)
		{
			PromotionTypes ePromotion = (PromotionTypes)GC.getFeatureInfo(eFeature)->getLocationUnitFreePromotion();
			if(ePromotion != NO_PROMOTION)
			{
				CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
				{
					bool bNoPromotion = false;
					// Check for negating promotions
					if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
					{
						bNoPromotion = true;
					}
					if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
					{
						bNoPromotion = true;
					}
					if(!bNoPromotion)
					{
						for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
						{
							const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
							CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
							if(pkPromotionInfo)
							{
								PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
								// Unit has negation promotion
								if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
								{
									bNoPromotion = true;
									break;
								}
							}
						}
					}
					if(!bNoPromotion)
					{
						setHasPromotion(ePromotion, true);
					}
				}
			}
		}
		//Improvement that provides free promotion?
		ImprovementTypes eNeededImprovement = pNewPlot->getImprovementType();
		if(eNeededImprovement != NO_IMPROVEMENT)
		{
			PromotionTypes ePromotion = (PromotionTypes)GC.getImprovementInfo(eNeededImprovement)->GetUnitFreePromotion();
			if(ePromotion != NO_PROMOTION)
			{
				CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
				{
					bool bNoPromotion = false;
					// Check for negating promotions
					if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
					{
						bNoPromotion = true;
					}
					if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
					{
						bNoPromotion = true;
					}
					if(!bNoPromotion)
					{
						for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
						{
							const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
							CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
							if(pkPromotionInfo)
							{
								PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
								// Unit has negation promotion
								if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
								{
									bNoPromotion = true;
									break;
								}
							}
						}
					}
					if(!bNoPromotion)
					{
						setHasPromotion(ePromotion, true);
					}
				}
			}
		}
		// Terrain that provides free promotions?
		TerrainTypes eTerrain = pNewPlot->getTerrainType();
		if(eTerrain != NO_TERRAIN && (eTerrain <= TERRAIN_SNOW))
		{
			PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(eTerrain)->getLocationUnitFreePromotion();
			if(ePromotion != NO_PROMOTION)
			{
				CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
				{
					bool bNoPromotion = false;
					// Check for negating promotions
					if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
					{
						bNoPromotion = true;
					}
					if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
					{
						bNoPromotion = true;
					}
					if(!bNoPromotion)
					{
						for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
						{
							const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
							CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
							if(pkPromotionInfo)
							{
								PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
								// Unit has negation promotion
								if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
								{
									bNoPromotion = true;
									break;
								}
							}
						}
					}
					if(!bNoPromotion)
					{
						setHasPromotion(ePromotion, true);
					}
				}
			}
		}
		if(pNewPlot->isHills())
		{
			PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_HILL)->getLocationUnitFreePromotion();
			if(ePromotion != NO_PROMOTION)
			{
				CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
				{
					bool bNoPromotion = false;
					// Check for negating promotions
					if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
					{
						bNoPromotion = true;
					}
					if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
					{
						bNoPromotion = true;
					}
					if(!bNoPromotion)
					{
						for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
						{
							const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
							CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
							if(pkPromotionInfo)
							{
								PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
								// Unit has negation promotion
								if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
								{
									bNoPromotion = true;
									break;
								}
							}
						}
					}
					if(!bNoPromotion)
					{
						setHasPromotion(ePromotion, true);
					}
				}
			}
		}
		if(pNewPlot->isMountain())
		{
			PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_MOUNTAIN)->getLocationUnitFreePromotion();
			if(ePromotion != NO_PROMOTION)
			{
				CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
				{
					bool bNoPromotion = false;
					// Check for negating promotions
					if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
					{
						bNoPromotion = true;
					}
					if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
					{
						bNoPromotion = true;
					}
					if(!bNoPromotion)
					{
						for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
						{
							const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
							CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
							if(pkPromotionInfo)
							{
								PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
								// Unit has negation promotion
								if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
								{
									bNoPromotion = true;
									break;
								}
							}
						}
					}
					if(!bNoPromotion)
					{
						setHasPromotion(ePromotion, true);
					}
				}
			}
		}
		if(pNewPlot->isShallowWater() && !pNewPlot->isLake())
		{
			PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_COAST)->getLocationUnitFreePromotion();
			if(ePromotion != NO_PROMOTION)
			{
				CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
				{
					bool bNoPromotion = false;
					// Check for negating promotions
					if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
					{
						bNoPromotion = true;
					}
					if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
					{
						bNoPromotion = true;
					}
					if(!bNoPromotion)
					{
						for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
						{
							const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
							CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
							if(pkPromotionInfo)
							{
								PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
								// Unit has negation promotion
								if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
								{
									bNoPromotion = true;
									break;
								}
							}
						}
					}
					if(!bNoPromotion)
					{
						setHasPromotion(ePromotion, true);
					}
				}
			}
		}
		if(!pNewPlot->isShallowWater() && !pNewPlot->isLake() && pNewPlot->isWater())
		{
			PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_OCEAN)->getLocationUnitFreePromotion();
			if(ePromotion != NO_PROMOTION)
			{
				CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
				{
					bool bNoPromotion = false;
					// Check for negating promotions
					if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
					{
						bNoPromotion = true;
					}
					if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
					{
						bNoPromotion = true;
					}
					if(!bNoPromotion)
					{
						for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
						{
							const PromotionTypes eNegatingPromotion = static_cast<PromotionTypes>(iI);
							CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eNegatingPromotion);
							if(pkPromotionInfo)
							{
								PromotionTypes eNegatorPromotion = (PromotionTypes)pkPromotionInfo->NegatesPromotion();
								// Unit has negation promotion
								if(isHasPromotion(eNegatingPromotion) && ePromotion == eNegatorPromotion)
								{
									bNoPromotion = true;
									break;
								}
							}
						}
					}
					if(!bNoPromotion)
					{
						setHasPromotion(ePromotion, true);
					}
				}
			}
		}
		if(pOldPlot != NULL)
		{
			// No longer on terrain that provides free promotions?
			if(pOldPlot->getTerrainType() != pNewPlot->getTerrainType())
			{
				for(iI = 0; iI < GC.getNumTerrainInfos(); iI++)
				{
					const TerrainTypes eNeededTerrain = static_cast<TerrainTypes>(iI);
					CvTerrainInfo* pkTerrainInfo = GC.getTerrainInfo(eNeededTerrain);
					if(pkTerrainInfo)
					{
						if(pOldPlot->isHills() && eNeededTerrain == TERRAIN_HILL)
						{
							PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_HILL)->getLocationUnitFreePromotion();
							if(ePromotion != NO_PROMOTION && isHasPromotion(ePromotion))
							{
								CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
								if(pkPromotionInfo && pkPromotionInfo->IsLostOnMove())
								{
									setHasPromotion(ePromotion, false);
								}
							}
						}
						if(pOldPlot->isMountain() && eNeededTerrain == TERRAIN_MOUNTAIN)
						{
							PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_MOUNTAIN)->getLocationUnitFreePromotion();
							if(ePromotion != NO_PROMOTION && isHasPromotion(ePromotion))
							{
								CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
								if(pkPromotionInfo && pkPromotionInfo->IsLostOnMove())
								{
									setHasPromotion(ePromotion, false);
								}
							}
						}
						if((pOldPlot->isWater() && pOldPlot->isShallowWater() && !pOldPlot->isLake()) && eNeededTerrain == TERRAIN_COAST)
						{
							PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_COAST)->getLocationUnitFreePromotion();
							if(ePromotion != NO_PROMOTION && isHasPromotion(ePromotion))
							{
								CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
								if(pkPromotionInfo && pkPromotionInfo->IsLostOnMove())
								{
									setHasPromotion(ePromotion, false);
								}
							}
						}
						if((pOldPlot->isWater() && !pOldPlot->isShallowWater() && !pOldPlot->isLake()) && eNeededTerrain == TERRAIN_OCEAN)
						{
							PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_OCEAN)->getLocationUnitFreePromotion();
							if(ePromotion != NO_PROMOTION && isHasPromotion(ePromotion))
							{
								CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
								if(pkPromotionInfo && pkPromotionInfo->IsLostOnMove())
								{
									setHasPromotion(ePromotion, false);
								}
							}
						}
						if(eNeededTerrain == pOldPlot->getTerrainType() && (eNeededTerrain <= TERRAIN_SNOW))
						{
							PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(eNeededTerrain)->getLocationUnitFreePromotion();
							if(ePromotion != NO_PROMOTION && isHasPromotion(ePromotion))
							{
								CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
								if(pkPromotionInfo && pkPromotionInfo->IsLostOnMove())
								{
									setHasPromotion(ePromotion, false);
								}
							}
						}
					}
				}
			}
			if(pOldPlot->getFeatureType() != pNewPlot->getFeatureType())
			{
				//No longer on feature that provides free promotions?
				for(iI = 0; iI < GC.getNumFeatureInfos(); iI++)
				{
					const FeatureTypes eNeededFeature = static_cast<FeatureTypes>(iI);
					CvFeatureInfo* pkFeatureInfo = GC.getFeatureInfo(eNeededFeature);
					if(pkFeatureInfo)
					{
						if(eNeededFeature == pOldPlot->getFeatureType())
						{
							PromotionTypes ePromotion = (PromotionTypes)GC.getFeatureInfo(eNeededFeature)->getLocationUnitFreePromotion();
							if(ePromotion != NO_PROMOTION && isHasPromotion(ePromotion))
							{
								CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
								if(pkPromotionInfo && pkPromotionInfo->IsLostOnMove())
								{
									setHasPromotion(ePromotion, false);
								}
							}
						}
					}
				}
			}
			if(pOldPlot->getImprovementType() != pNewPlot->getImprovementType())
			{
				//No longer on improvement that provides free promotion?
				for(iI = 0; iI < GC.getNumImprovementInfos(); iI++)
				{
					const ImprovementTypes eNeededImprovement = static_cast<ImprovementTypes>(iI);
					CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eNeededImprovement);
					if(pkImprovementInfo)
					{
						if(eNeededImprovement == pOldPlot->getImprovementType())
						{
							PromotionTypes ePromotion = (PromotionTypes)GC.getImprovementInfo(eNeededImprovement)->GetUnitFreePromotion();
							if(ePromotion != NO_PROMOTION && isHasPromotion(ePromotion))
							{
								CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
								if(pkPromotionInfo && pkPromotionInfo->IsLostOnMove())
								{
										setHasPromotion(ePromotion, false);
								}
							}
						}
					}
				}
			}
		}
#endif

		if(pOldPlot != NULL && getDomainType() == DOMAIN_SEA)
		{
#if defined(MOD_EVENTS_UNIT_CAPTURE)
			kPlayer.GetPlayerTraits()->CheckForBarbarianConversion(this, pNewPlot);
#else
			kPlayer.GetPlayerTraits()->CheckForBarbarianConversion(pNewPlot);
#endif
		}

		if(GC.IsGraphicsInitialized())
		{
			//override bShow if check plot visible
			if(bCheckPlotVisible && (pNewPlot->isVisibleToWatchingHuman() || pOldPlot->isVisibleToWatchingHuman()))
				bShow = true;

			if(CvPreGame::quickMovement())
			{
				bShow = false;
			}

			if(bShow)
			{
				QueueMoveForVisualization(pNewPlot);
			}
			else
			{
				SetPosition(pNewPlot);
			}
		}

	}

	if(pOldPlot != NULL)
	{
		if(hasCargo())
		{
			pUnitNode = pOldPlot->headUnitNode();

			while(pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(*pUnitNode);
				pUnitNode = pOldPlot->nextUnitNode(pUnitNode);

				if(pLoopUnit && pLoopUnit->getTransportUnit() == this)
				{
					pLoopUnit->setXY(iX, iY, bGroup, bUpdate);

					// Reset to head node since we just moved some cargo around, and the unit storage in the plot is going to be different now
					pUnitNode = pOldPlot->headUnitNode();
				}
			}
		}
	}

	if(pNewPlot != NULL && iMapLayer == DEFAULT_UNIT_MAP_LAYER)
	{
		if(!bNoMove)
		{
			if(pNewPlot->isGoody(getTeam()))
			{
				GET_PLAYER(getOwner()).doGoody(pNewPlot, this);
			}
			if(!isBarbarian())
			{
				if(pNewPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
				{
					// See if we need to remove a temporary dominance zone
					kPlayer.GetTacticalAI()->DeleteTemporaryZone(pNewPlot);

					// Is there a camp guard that will convert to our side here, if so have special routine to handle it
#if defined(MOD_EVENTS_UNIT_CAPTURE)
					if (!kPlayer.GetPlayerTraits()->CheckForBarbarianConversion(this, pNewPlot))
#else
					if (!kPlayer.GetPlayerTraits()->CheckForBarbarianConversion(pNewPlot))
#endif
					{
						int iNumGold = GC.getGame().getHandicapInfo().getBarbCampGold();
						iNumGold *= (100 + kPlayer.GetPlayerTraits()->GetPlunderModifier()) / 100;

						// Game Speed Mod
						iNumGold *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType())->getGoldPercent();
						iNumGold /= 100;

						// Normal way to handle it
						if(getOwner() == GC.getGame().getActivePlayer())
						{
							DLLUI->AddUnitMessage(0, GetIDInfo(), getOwner(), true, GC.getEVENT_MESSAGE_TIME(), GetLocalizedText("TXT_KEY_MISC_DESTROYED_BARBARIAN_CAMP", iNumGold));
						}

						pNewPlot->setImprovementType(NO_IMPROVEMENT);

						CvBarbarians::DoBarbCampCleared(pNewPlot, getOwner());

						kPlayer.GetTreasury()->ChangeGold(iNumGold);

#if defined(MOD_BALANCE_CORE_POLICIES)
						if(MOD_BALANCE_CORE_POLICIES)
						{
							float fDelay = 0.5f;
							if(GET_PLAYER(getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURE_FROM_BARBARIAN_KILLS) > 0 || GET_PLAYER(getOwner()).GetBarbarianCombatBonus() > 0)
							{	
								int iCulturePoints = (GET_PLAYER(getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURE_FROM_BARBARIAN_KILLS) / 5);
								if(iCulturePoints <= 0)
								{
									iCulturePoints = GET_PLAYER(getOwner()).GetBarbarianCombatBonus();
								}
								GET_PLAYER(getOwner()).changeJONSCulture(iCulturePoints);
								if(GET_PLAYER(getOwner()).GetID() == GC.getGame().getActivePlayer())
								{
									char text[256] = {0};
									fDelay += 0.5f;
									sprintf_s(text, "[COLOR_MAGENTA]+%d[ENDCOLOR][ICON_CULTURE]", iCulturePoints);
									DLLUI->AddPopupText(pNewPlot->getX(),pNewPlot->getY(), text, fDelay);
								}
							}
						}
#endif

						// Set who last cleared the camp here
						pNewPlot->SetPlayerThatClearedBarbCampHere(getOwner());

						if(getOwner() < MAX_MAJOR_CIVS)
						{
							// Completes a quest for anyone?
							PlayerTypes eMinor;
							for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
							{
								eMinor = (PlayerTypes) iMinorLoop;
								CvPlayer& minorPlayer = GET_PLAYER(eMinor);

								if(!minorPlayer.isAlive())
									continue;

								CvMinorCivAI* pMinorCivAI = minorPlayer.GetMinorCivAI();
								pMinorCivAI->DoTestActiveQuestsForPlayer(getOwner(), /*bTestComplete*/ true, /*bTestObsolete*/ false, MINOR_CIV_QUEST_KILL_CAMP);
							}
						}

						// If it's the active player then show the popup
						if(getOwner() == GC.getGame().getActivePlayer())
						{
							// Don't show in MP
#if defined(MOD_API_EXTENSIONS)
							if(!GC.getGame().isReallyNetworkMultiPlayer())
#else
							if(!GC.getGame().isNetworkMultiPlayer())	// KWG: Candidate for !GC.getGame().isOption(GAMEOPTION_SIMULTANEOUS_TURNS)
#endif
							{
								CvPopupInfo kPopupInfo(BUTTONPOPUP_BARBARIAN_CAMP_REWARD, iNumGold);
								DLLUI->AddPopup(kPopupInfo);
								// We are adding a popup that the player must make a choice in, make sure they are not in the end-turn phase.
								CancelActivePlayerEndTurn();

#if !defined(NO_ACHIEVEMENTS)
								//Increment Stat
								if(kPlayer.isHuman() && !GC.getGame().isGameMultiPlayer())
								{
									gDLL->IncrementSteamStatAndUnlock(ESTEAMSTAT_BARBARIANCAMPS, 100, ACHIEVEMENT_100CAMPS);
								}
#endif

							}
						}
					}
#if defined(MOD_BALANCE_CORE_POLICIES)
					else if(kPlayer.GetPlayerTraits()->GetLandBarbarianConversionPercent() > 0)
					{
						if(MOD_BALANCE_CORE_POLICIES)
						{
							float fDelay = 0.5f;
							if(GET_PLAYER(getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURE_FROM_BARBARIAN_KILLS) > 0 || GET_PLAYER(getOwner()).GetBarbarianCombatBonus() > 0)
							{	
								int iCulturePoints = (GET_PLAYER(getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURE_FROM_BARBARIAN_KILLS) / 5);
								if(iCulturePoints <= 0)
								{
									iCulturePoints = GET_PLAYER(getOwner()).GetBarbarianCombatBonus();
								}
								GET_PLAYER(getOwner()).changeJONSCulture(iCulturePoints);
								if(GET_PLAYER(getOwner()).GetID() == GC.getGame().getActivePlayer())
								{
									char text[256] = {0};
									fDelay += 0.5f;
									sprintf_s(text, "[COLOR_MAGENTA]+%d[ENDCOLOR][ICON_CULTURE]", iCulturePoints);
									DLLUI->AddPopupText(pNewPlot->getX(),pNewPlot->getY(), text, fDelay);
								}
							}
						}
					}
#endif
				}
			}
		}

		// New plot location is owned by someone
		PlayerTypes ePlotOwner = pNewPlot->getOwner();
		if(ePlotOwner != NO_PLAYER)
		{
			// If we're in friendly territory and we can embark, give the promotion for free
			if(pNewPlot->IsFriendlyTerritory(getOwner()))
			{
				if(GET_TEAM(eOurTeam).canEmbark())
				{
					PromotionTypes ePromotionEmbarkation = kPlayer.GetEmbarkationPromotion();

					bool bGivePromotion = false;

					// Civilians get it for free
					if(getDomainType() == DOMAIN_LAND)
					{
						if(!IsCombatUnit())
							bGivePromotion = true;
					}

					// Can the unit get this? (handles water units and such)
					if(!bGivePromotion && ::IsPromotionValidForUnitCombatType(ePromotionEmbarkation, getUnitType()))
						bGivePromotion = true;

					// Some case that gives us the promotion?
					if(bGivePromotion)
						setHasPromotion(ePromotionEmbarkation, true);
						
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
					if (MOD_PROMOTIONS_DEEP_WATER_EMBARKATION) {
						// Incredibly unlikely to ever happen as it implies that hovering units come before embarkation in the tech tree!
						PromotionTypes ePromotionDeepWaterEmbarkation = kPlayer.GetDeepWaterEmbarkationPromotion();
						if(::IsPromotionValidForUnitCombatType(ePromotionDeepWaterEmbarkation, getUnitType())) {
							setHasPromotion(ePromotionDeepWaterEmbarkation, true);
						}
					}
#endif
				}
			}

			// Are we in enemy territory? If so, give notification to owner
			if(GET_TEAM(getTeam()).isAtWar(GET_PLAYER(ePlotOwner).getTeam()) && !isInvisible(GET_PLAYER(ePlotOwner).getTeam(),false,false))
			{
				CvNotifications* pNotifications = GET_PLAYER(ePlotOwner).GetNotifications();
				if(pNotifications)
				{
					Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_ENEMY");
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_ENEMY_SUMMARY");
					pNotifications->Add(NOTIFICATION_ENEMY_IN_TERRITORY, strMessage.toUTF8(), strSummary.toUTF8(), iX, iY, getUnitType(), getOwner());
				}
			}
		}
	}

	// Create any units we captured, now that we own the destination
	for(uint uiCaptureIndex = 0; uiCaptureIndex < kCaptureUnitList.size(); ++uiCaptureIndex)
	{
		createCaptureUnit(kCaptureUnitList[uiCaptureIndex]);
	}

	CvAssert(pOldPlot != pNewPlot);

	if(IsSelected())
		gDLL->GameplayMinimapUnitSelect(iX, iY);

	setInfoBarDirty(true);

	// if there is an enemy city nearby, alert any scripts to this
#if defined(MOD_EVENTS_CITY_BOMBARD)
	int iAttackRange = (MOD_EVENTS_CITY_BOMBARD ? GC.getMAX_CITY_ATTACK_RANGE() : GC.getCITY_ATTACK_RANGE());
#else
	int iAttackRange = GC.getCITY_ATTACK_RANGE();
#endif
	for(int iDX = -iAttackRange; iDX <= iAttackRange; iDX++)
	{
		for(int iDY = -iAttackRange; iDY <= iAttackRange; iDY++)
		{
			CvPlot* pTargetPlot = plotXYWithRangeCheck(getX(), getY(), iDX, iDY, iAttackRange);
			if(pTargetPlot && pTargetPlot->isCity())
			{
				if(isEnemy(pTargetPlot->getTeam()))
				{
					// do it
					CvCity* pkPlotCity = pTargetPlot->getPlotCity();
#if defined(MOD_EVENTS_CITY_BOMBARD)
					if (!MOD_EVENTS_CITY_BOMBARD || plotXYWithRangeCheck(getX(), getY(), iDX, iDY, pkPlotCity->getBombardRange()))
					{
#endif
						auto_ptr<ICvCity1> pPlotCity = GC.WrapCityPointer(pkPlotCity);
						DLLUI->SetSpecificCityInfoDirty(pPlotCity.get(), CITY_UPDATE_TYPE_ENEMY_IN_RANGE);
#if defined(MOD_EVENTS_CITY_BOMBARD)
					}
#endif
				}
			}
		}
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(getOwner());
		args->Push(GetID());
		args->Push(getX());
		args->Push(getY());

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "UnitSetXY", args.get(), bResult);
	}

	if (bOwnerIsActivePlayer)
		DLLUI->SetDontShowPopups(false);

	// update garrison status for old and new city, if applicable
	CvCity* pkPrevGarrisonedCity = GetGarrisonedCity();
	if (pkPrevGarrisonedCity)
	{
		//when moving out, another unit might be present to take over garrison duty
		pkPrevGarrisonedCity->SetGarrison( pkPrevGarrisonedCity->plot()->getBestGarrison(getOwner()).pointer() );
		auto_ptr<ICvCity1> pkDllCity(new CvDllCity(pkPrevGarrisonedCity));
		DLLUI->SetSpecificCityInfoDirty(pkDllCity.get(), CITY_UPDATE_TYPE_GARRISON);
	}

	if (pNewPlot && pNewPlot->isCity())
	{
		//when moving in, see if we're better than the previous garrison
		CvUnit* pBestGarrison = pNewPlot->getBestGarrison(getOwner()).pointer();
		if (pBestGarrison==this)
		{
			CvCity* pkNewGarrisonedCity = pNewPlot->getPlotCity();
			pkNewGarrisonedCity->SetGarrison(this);
			auto_ptr<ICvCity1> pkDllCity(new CvDllCity(pkNewGarrisonedCity));
			DLLUI->SetSpecificCityInfoDirty(pkDllCity.get(), CITY_UPDATE_TYPE_GARRISON);
		}
	}
#if defined(MOD_BALANCE_CORE)
	else if(pNewPlot && !pNewPlot->isCity())
	{
		SetGarrisonedCity(-1);
	}
#endif
	
#if !defined(NO_ACHIEVEMENTS)
	//Dr. Livingstone I presume?
	if (isHuman() && !isDelayedDeath())
	{
		if(strcmp(getCivilizationInfo().GetType(), "CIVILIZATION_BRAZIL") == 0){
			UnitTypes eExplorer = (UnitTypes) GC.getInfoTypeForString("UNIT_EXPLORER", true /*bHideAssert*/); 
			if(getUnitType() == eExplorer && strcmp(getNameNoDesc(), "TXT_KEY_EXPLORER_STANLEY") == 0 ){
				CvPlot* pAdjacentPlot;
				for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
				{
					pAdjacentPlot = plotDirection(pNewPlot->getX(), pNewPlot->getY(), ((DirectionTypes)iI));

					if(pAdjacentPlot != NULL && pAdjacentPlot->getNumUnits() != NULL){
						for(int iJ = 0; iJ < pAdjacentPlot->getNumUnits(); iJ++){
							if(pAdjacentPlot->getUnitByIndex(iJ)->getUnitType() ==  eExplorer && strcmp(pAdjacentPlot->getUnitByIndex(iJ)->getNameNoDesc(), "TXT_KEY_EXPLORER_LIVINGSTON") == 0){
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_52);
							}
						}
					}
				}
			}
		}
	}
#endif
}

//	---------------------------------------------------------------------------
bool CvUnit::at(int iX, int iY) const
{
	VALIDATE_OBJECT
	return((getX() == iX) && (getY() == iY));
}


//	--------------------------------------------------------------------------------
bool CvUnit::atPlot(const CvPlot& checkPlot) const
{
	VALIDATE_OBJECT
	return((getX() == checkPlot.getX()) && (getY() == checkPlot.getY()));
}


//	--------------------------------------------------------------------------------
CvPlot* CvUnit::plot() const
{
	VALIDATE_OBJECT
	return GC.getMap().plotCheckInvalid(getX(), getY());
}


//	--------------------------------------------------------------------------------
int CvUnit::getArea() const
{
	VALIDATE_OBJECT
	return GC.getMap().plotCheckInvalid(getX(), getY())->getArea();
}


//	--------------------------------------------------------------------------------
CvArea* CvUnit::area() const
{
	VALIDATE_OBJECT
	return GC.getMap().plotCheckInvalid(getX(), getY())->area();
}


//	--------------------------------------------------------------------------------
bool CvUnit::onMap() const
{
	VALIDATE_OBJECT
	return (plot() != NULL);
}


//	--------------------------------------------------------------------------------
int CvUnit::getLastMoveTurn() const
{
	VALIDATE_OBJECT
	return m_iLastMoveTurn;
}


//	--------------------------------------------------------------------------------
void CvUnit::setLastMoveTurn(int iNewValue)
{
	VALIDATE_OBJECT
	m_iLastMoveTurn = iNewValue;
	CvAssert(getLastMoveTurn() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::GetCycleOrder() const
{
	VALIDATE_OBJECT
	return m_iCycleOrder;
}


//	--------------------------------------------------------------------------------
void CvUnit::SetCycleOrder(int iNewValue)
{
	VALIDATE_OBJECT
	m_iCycleOrder = iNewValue;
}


//	--------------------------------------------------------------------------------
bool CvUnit::IsRecon() const
{
	return GetReconCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetReconCount() const
{
	return m_iReconCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeReconCount(int iChange)
{
	if(iChange != 0)
	{
		m_iReconCount += iChange;
	}
}

//	--------------------------------------------------------------------------------
CvPlot* CvUnit::getReconPlot() const
{
	VALIDATE_OBJECT
	return GC.getMap().plotCheckInvalid(m_iReconX, m_iReconY);
}


//	--------------------------------------------------------------------------------
void CvUnit::setReconPlot(CvPlot* pNewValue)
{
	VALIDATE_OBJECT
	CvPlot* pOldPlot;

	pOldPlot = getReconPlot();

	if(pOldPlot != pNewValue)
	{
		if(pOldPlot != NULL)
		{
			if (canChangeVisibility())
#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
				pOldPlot->changeAdjacentSight(getTeam(), reconRange(), false, getSeeInvisibleType(), getFacingDirection(true));
#else
				pOldPlot->changeAdjacentSight(getTeam(), GC.getRECON_VISIBILITY_RANGE(), false, getSeeInvisibleType(), getFacingDirection(true));
#endif
			pOldPlot->changeReconCount(-1); // changeAdjacentSight() tests for getReconCount()
		}

		if(pNewValue == NULL)
		{
			m_iReconX = INVALID_PLOT_COORD;
			m_iReconY = INVALID_PLOT_COORD;
		}
		else
		{
			m_iReconX = pNewValue->getX();
			m_iReconY = pNewValue->getY();

			pNewValue->changeReconCount(1); // changeAdjacentSight() tests for getReconCount()
			if (canChangeVisibility())
#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
				pNewValue->changeAdjacentSight(getTeam(), reconRange(), true, getSeeInvisibleType(), getFacingDirection(true));
#else
				pNewValue->changeAdjacentSight(getTeam(), GC.getRECON_VISIBILITY_RANGE(), true, getSeeInvisibleType(), getFacingDirection(true));
#endif
		}
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getGameTurnCreated() const
{
	VALIDATE_OBJECT
	return m_iGameTurnCreated;
}


//	--------------------------------------------------------------------------------
void CvUnit::setGameTurnCreated(int iNewValue)
{
	VALIDATE_OBJECT
	m_iGameTurnCreated = iNewValue;
	CvAssert(getGameTurnCreated() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getDamage() const
{
	VALIDATE_OBJECT
	return m_iDamage;
}


//	--------------------------------------------------------------------------------
/**
	Sets the damage.

	@param	iNewValue				New damage value
	@param	ePlayer					The player doing the damage, can be NO_PLAYER.
	@param	fAdditionalTextDelay	The additional text delay.  If < 0, then no popup text is shown
	@param [in]	pAppendText 	If non-null, the text to append to the popup text.

	@return	The difference in the damage.
 */
int CvUnit::setDamage(int iNewValue, PlayerTypes ePlayer, float fAdditionalTextDelay, const CvString* pAppendText)
{
	VALIDATE_OBJECT
	int iOldValue;
	float fDelay = 0.0f + fAdditionalTextDelay;

	iOldValue = getDamage();

	if(GetCurrHitPoints() == GetMaxHitPoints() && iNewValue < GetMaxHitPoints()) 
	{
		m_iLastGameTurnAtFullHealth = GC.getGame().getGameTurn();
	}

	m_iDamage = range(iNewValue, 0, GetMaxHitPoints());
	int iDiff = m_iDamage - iOldValue;

	CvAssertMsg(GetCurrHitPoints() >= 0, "currHitPoints() is expected to be non-negative (invalid Index)");

	if(iOldValue != getDamage())
	{
		if(IsGarrisoned())
		{
			CvCity* pCity = GetGarrisonedCity();
			if(pCity != NULL)
			{
				pCity->updateStrengthValue();
			}
		}

		setInfoBarDirty(true);

		if(IsSelected())
		{
			DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
		}

		auto_ptr<ICvPlot1> pDllSelectionPlot(DLLUI->getSelectionPlot());
		const int iSelectionPlotIndex = (pDllSelectionPlot.get() != NULL)? pDllSelectionPlot->GetPlotIndex() : -1;

		if(plot()->GetPlotIndex() == iSelectionPlotIndex)
		{
			DLLUI->setDirty(PlotListButtons_DIRTY_BIT, true);
		}

		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitSetDamage(pDllUnit.get(), m_iDamage, iOldValue);


		// send the popup text if the player can see this plot
		if(plot()->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF)
		{
			if(!IsDead())
			{
				CvString text;
				iNewValue = MIN(GC.getMAX_HIT_POINTS(),iNewValue);
				int iDiff = iOldValue - iNewValue;
				if(iNewValue < iOldValue)
				{
					text.Format("[COLOR_GREEN]+%d", iDiff);
					fDelay = GC.getPOST_COMBAT_TEXT_DELAY() * 2;
				}
				else
				{
					text.Format("[COLOR_RED]%d", iDiff);
				}
				if(pAppendText != NULL)
				{
					text += " ";
					text += *pAppendText;
				}
				text += "[ENDCOLOR]";

				int iX = m_iX;
				int iY = m_iY;

				// Where do we display this text? - put it where the unit is moving TO if it's this unit's turn
				if(GET_PLAYER(getOwner()).isTurnActive())
				{
					const MissionData* pkMissionData = GetHeadMissionData();
					if(pkMissionData != NULL)
					{
						if(pkMissionData->eMissionType == CvTypes::getMISSION_MOVE_TO())
						{
							int iTempX = pkMissionData->iData1;
							int iTempY = pkMissionData->iData2;

							CvPlot* pPlot = GC.getMap().plot(iTempX, iTempY);

							// No enemies in our destination, and it's not a city OR it's a city we own
							if(pPlot->getNumVisibleEnemyDefenders(this) == 0 && (!pPlot->isCity() || pPlot->getPlotCity()->getOwner() == getOwner()))
							{
								iX = iTempX;
								iY = iTempY;
							}
						}
					}
				}

				if (!isSuicide())	// Show the HP lost, expect if it is a suicide unit (missisle, etc.)
#if defined(SHOW_PLOT_POPUP)
					SHOW_PLOT_POPUP(GC.getMap().plot(iX, iY), getOwner(), text.c_str(), fDelay);
#else
					DLLUI->AddPopupText(iX, iY, text.c_str(), fDelay);
#endif
			}
		}
	}

	if(IsDead())
	{
#if !defined(NO_ACHIEVEMENTS)
		CvGame& game = GC.getGame();
		const PlayerTypes eActivePlayer = game.getActivePlayer();
		CvPlayerAI& activePlayer = GET_PLAYER(eActivePlayer);
		if(m_iLastGameTurnAtFullHealth != -1 && m_iLastGameTurnAtFullHealth == game.getGameTurn() && getOwner() == eActivePlayer && activePlayer.isHuman())
		{
			CvUnitEntry* pUnitInfo = GC.getUnitInfo(getUnitType());
			if(pUnitInfo != NULL && strcmp(pUnitInfo->GetType(),"UNIT_XCOM_SQUAD") == 0)
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_XP2_46);
			}
		}
#endif

		m_iLastGameTurnAtFullHealth = -1;

		kill(true, ePlayer);

		CvString szMsg;
		CvString szUnitAIString;
		getUnitAIString(szUnitAIString, AI_getUnitAIType());
		szMsg.Format("Killed in combat: %s, AI was: ", getName().GetCString());
		szMsg += szUnitAIString;
		GET_PLAYER(m_eOwner).GetTacticalAI()->LogTacticalMessage(szMsg, true /*bSkipLogDominanceZone*/);

		if(ePlayer != NO_PLAYER)
		{
			if(m_eOwner == BARBARIAN_PLAYER && plot()->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
			{
				GET_PLAYER(ePlayer).GetPlayerTraits()->SetDefeatedBarbarianCampGuardType(getUnitType());
			}

#if defined(MOD_API_EXTENSIONS)
			GET_PLAYER(ePlayer).DoUnitKilledCombat(NULL, getOwner(), getUnitType());
#else
			GET_PLAYER(ePlayer).DoUnitKilledCombat(getOwner(), getUnitType());
#endif
		}
	}

	return iDiff;
}

//    -------------------------------------------------------------------------------- 
/**
	Change the damage by a delta. 
	Damage is measured as an integer percent, from 100 (full health) to 0 (dead)

	@param	iChange					Delta added to the current damage.
	@param	ePlayer					The player doing the damage, can be NO_PLAYER
	@param	fAdditionalTextDelay	The additional text delay.  If < 0, no popup text is shown.
	@param [in]	pAppendText 		If non-null, the text to append to the popup.

	@return	the final delta change to the units damage.
 */
int CvUnit::changeDamage(int iChange, PlayerTypes ePlayer, float fAdditionalTextDelay, const CvString* pAppendText)
{
	VALIDATE_OBJECT;
	return setDamage((getDamage() + iChange), ePlayer, fAdditionalTextDelay, pAppendText);
}


//	--------------------------------------------------------------------------------
int CvUnit::getMoves() const
{
	VALIDATE_OBJECT
	return m_iMoves;
}


//	--------------------------------------------------------------------------------
void CvUnit::setMoves(int iNewValue)
{
	VALIDATE_OBJECT
	if(getMoves() != iNewValue)
	{
		CvPlot* pPlot = plot();

		m_iMoves = iNewValue;

		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitShouldDimFlag(pDllUnit.get(), /*bDim*/ getMoves() <= 0);

		if(IsSelected())
		{
			DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
		}

		auto_ptr<ICvPlot1> pDllSelectionPlot(DLLUI->getSelectionPlot());
		int iSelectionPlotIndex = (pDllSelectionPlot.get() != NULL)? pDllSelectionPlot->GetPlotIndex() : -1;

		if(pPlot->GetPlotIndex() == iSelectionPlotIndex)
		{
			DLLUI->setDirty(PlotListButtons_DIRTY_BIT, true);
		}
	}
}


//	--------------------------------------------------------------------------------
void CvUnit::changeMoves(int iChange)
{
	VALIDATE_OBJECT
	setMoves(getMoves() + iChange);
}


//	--------------------------------------------------------------------------------
void CvUnit::finishMoves()
{
	VALIDATE_OBJECT
	setMoves(0);
}

//	--------------------------------------------------------------------------------
/// Is this unit capable of moving on its own?
bool CvUnit::IsImmobile() const
{
	if(getDomainType() == DOMAIN_IMMOBILE)
	{
		return true;
	}

	return m_bImmobile;
}

/// Is this unit capable of moving on its own?
void CvUnit::SetImmobile(bool bValue)
{
	if(IsImmobile() != bValue)
	{
		m_bImmobile = bValue;
	}
}

//	--------------------------------------------------------------------------------
/// Is this Unit in friendly territory? (ours or someone we have Open Borders with)
bool CvUnit::IsInFriendlyTerritory() const
{
	VALIDATE_OBJECT
	return plot()->IsFriendlyTerritory(getOwner());
}

//	--------------------------------------------------------------------------------
/// Is this Unit under ranged fire from an enemy unit that can do it significant damage?
bool CvUnit::IsUnderEnemyRangedAttack() const
{
	PlayerTypes eLoopPlayer;
	int iTotalDamage = 0;

	for(int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		CvPlayer &kPlayer = GET_PLAYER(eLoopPlayer);
		if(kPlayer.isAlive())
		{
			if (atWar(getTeam(), kPlayer.getTeam()))
			{
				int iLoop;
				CvCity *pLoopCity;
				for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
				{
					if(pLoopCity->canRangeStrikeAt(getX(), getY()))
					{
						return true;
					}
				}

				CvUnit *pLoopUnit;
				for(pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
				{
					if(pLoopUnit)
					{
						if(!pLoopUnit->IsCanAttack())
						{
							continue;
						}

						if(pLoopUnit->IsCanAttackRanged())
						{
							// Are we in range?
							if(plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), getX(), getY()) <= pLoopUnit->GetRange())
							{
								// Do we have LOS to the target?
								if(pLoopUnit->canEverRangeStrikeAt(getX(), getY()))
								{
									// Will we do any damage
									int iExpectedDamage = pLoopUnit->GetRangeCombatDamage(this, NULL, false);
									iTotalDamage += iExpectedDamage;
									if (iTotalDamage > healRate(plot()))
									{
										return true;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
int CvUnit::getExperience() const
{
	VALIDATE_OBJECT
	return m_iExperience;
}


//	--------------------------------------------------------------------------------
void CvUnit::setExperience(int iNewValue, int iMax)
{
	VALIDATE_OBJECT
	if((getExperience() != iNewValue) && (getExperience() < ((iMax == -1) ? INT_MAX : iMax)))
	{
		int iExperienceChange = iNewValue - getExperience();

		m_iExperience = std::min(((iMax == -1) ? INT_MAX : iMax), iNewValue);
		CvAssert(getExperience() >= 0);

		if(getOwner() == GC.getGame().getActivePlayer())
		{
			// Don't show XP for unit that's about to bite the dust
			if(!IsDead())
			{
				Localization::String localizedText = Localization::Lookup("TXT_KEY_EXPERIENCE_POPUP");
				localizedText << iExperienceChange;
#if !defined(SHOW_PLOT_POPUP)
				float fDelay = GC.getPOST_COMBAT_TEXT_DELAY();
#endif

				int iX = m_iX;
				int iY = m_iY;

				// Where do we display this text?
				const MissionData* pkMissionData = GetHeadMissionData();
				if(pkMissionData != NULL)
				{
					if(pkMissionData->eMissionType == CvTypes::getMISSION_MOVE_TO())
					{
						int iTempX = pkMissionData->iData1;
						int iTempY = pkMissionData->iData2;

						if(GC.getMap().plot(iTempX, iTempY)->getNumVisibleEnemyDefenders(this) == 0)
						{
							iX = iTempX;
							iY = iTempY;
						}
					}
				}

#if defined(SHOW_PLOT_POPUP)
				SHOW_PLOT_POPUP(GC.getMap().plot(iX, iY), getOwner(), localizedText.toUTF8(), 0.0f);
#else
				DLLUI->AddPopupText(iX, iY, localizedText.toUTF8(), fDelay);
#endif

				if(IsSelected())
				{
					DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
				}
			}
		}
	}
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExperience(int iChange, int iMax, bool bFromCombat, bool bInBorders, bool bUpdateGlobal)
{
	VALIDATE_OBJECT
	// Barbs don't get XP or Promotions
	if(isBarbarian() && iChange > 0)
	{
		return;
	}

	int iUnitExperience = iChange;
	PromotionTypes eNewPromotion = NO_PROMOTION;

	if(bFromCombat)
	{
		CvPlayer& kPlayer = GET_PLAYER(getOwner());

		// Promotion that changes after combat?
		for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
		{
			const PromotionTypes eLoopPromotion = static_cast<PromotionTypes>(iI);
			CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eLoopPromotion);
			if(pkPromotionInfo)
			{
				if (pkPromotionInfo->HasPostCombatPromotions() && m_Promotions.HasPromotion(eLoopPromotion))
				{
					eNewPromotion = m_Promotions.ChangePromotionAfterCombat(eLoopPromotion);
					setHasPromotion(eLoopPromotion, false);

					if (eNewPromotion != NO_PROMOTION)
					{
						setHasPromotion(eNewPromotion, true);

#if defined(MOD_EVENTS_UNIT_UPGRADES)
						if (MOD_EVENTS_UNIT_UPGRADES) {
							GAMEEVENTINVOKE_HOOK(GAMEEVENT_UnitPromoted, getOwner(), GetID(), eNewPromotion);
						}
#endif

						CvPromotionEntry* pkNewPromotionInfo = GC.getPromotionInfo(eNewPromotion);
						Localization::String localizedText = Localization::Lookup(pkNewPromotionInfo->GetDescriptionKey());
#if defined(SHOW_PLOT_POPUP)
						SHOW_PLOT_POPUP(plot(), getOwner(), localizedText.toUTF8(), 0.0f);
#else
						float fDelay = GC.getPOST_COMBAT_TEXT_DELAY() * 2;
						DLLUI->AddPopupText(getX(), getY(), localizedText.toUTF8(), fDelay);
#endif
					}
				}
			}
		}

		// Player Great General/Admiral mod
		int iCombatExperienceMod;
		if (getDomainType() == DOMAIN_SEA)
		{
			iCombatExperienceMod = 100 + kPlayer.getGreatAdmiralRateModifier();
		}
		else
		{
			iCombatExperienceMod = 100 + kPlayer.getGreatGeneralRateModifier();
		}

		// Unit XP mod
		iUnitExperience += (iChange * kPlayer.getExpModifier()) / 100;
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
		if(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
		{
			const std::vector<ResourceTypes>& vStrategicMonopolies = GET_PLAYER(getOwner()).GetStrategicMonopolies();
			for (size_t iResourceLoop = 0; iResourceLoop < vStrategicMonopolies.size(); iResourceLoop++)
			{
				ResourceTypes eResourceLoop = vStrategicMonopolies[iResourceLoop];
				CvResourceInfo* pInfo = GC.getResourceInfo(eResourceLoop);
				if (pInfo && pInfo->getMonopolyXPBonus() > 0)
				{
					iUnitExperience += pInfo->getMonopolyXPBonus();
				}
			}
		}
#endif
		// Great General & Unit XP mod in borders
		if (bInBorders && getDomainType() == DOMAIN_LAND)
		{
			// In-borders GG mod
			iCombatExperienceMod += kPlayer.getDomesticGreatGeneralRateModifier() + kPlayer.getExpInBorderModifier();
			iUnitExperience += (iChange * kPlayer.getExpInBorderModifier()) / 100;
		}

		if(bUpdateGlobal)
		{
			// Add Unit GG mod
			if (getDomainType() == DOMAIN_LAND) 
			{
				iCombatExperienceMod += getGreatGeneralModifier();
			}

			if(iMax == -1)
			{
				if(getDomainType() == DOMAIN_SEA)
				{
#if defined(MOD_GLOBAL_LOCAL_GENERALS)
					kPlayer.changeNavalCombatExperience((iChange * iCombatExperienceMod) / 100, (MOD_GLOBAL_LOCAL_GENERALS ? this : NULL));
#else
					kPlayer.changeNavalCombatExperience((iChange * iCombatExperienceMod) / 100);
#endif
				}
				else
				{
#if defined(MOD_GLOBAL_LOCAL_GENERALS)
					kPlayer.changeCombatExperience((iChange * iCombatExperienceMod) / 100, (MOD_GLOBAL_LOCAL_GENERALS ? this : NULL));
#else
					kPlayer.changeCombatExperience((iChange * iCombatExperienceMod) / 100);
#endif
				}
			}
			else
			{
				int iModdedChange = min(iMax - m_iExperience, iChange);
				if(iModdedChange > 0)
				{
					if(getDomainType() == DOMAIN_SEA)
					{
#if defined(MOD_GLOBAL_LOCAL_GENERALS)
						kPlayer.changeNavalCombatExperience((iModdedChange * iCombatExperienceMod) / 100, (MOD_GLOBAL_LOCAL_GENERALS ? this : NULL));
#else
						kPlayer.changeNavalCombatExperience((iModdedChange * iCombatExperienceMod) / 100);
#endif
					}
					else
					{
#if defined(MOD_GLOBAL_LOCAL_GENERALS)
						kPlayer.changeCombatExperience((iModdedChange * iCombatExperienceMod) / 100, (MOD_GLOBAL_LOCAL_GENERALS ? this : NULL));
#else
						kPlayer.changeCombatExperience((iModdedChange * iCombatExperienceMod) / 100);
#endif
					}
				}
			}
		}

		if(getExperiencePercent() != 0)
		{
			iUnitExperience *= std::max(0, 100 + getExperiencePercent());
			iUnitExperience /= 100;
		}
	}

	setExperience((getExperience() + iUnitExperience), iMax);
}


//	--------------------------------------------------------------------------------
int CvUnit::getLevel() const
{
	VALIDATE_OBJECT
	return m_iLevel;
}


//	--------------------------------------------------------------------------------
void CvUnit::setLevel(int iNewValue)
{
	VALIDATE_OBJECT
	if(getLevel() != iNewValue)
	{
		m_iLevel = iNewValue;
		CvAssert(getLevel() >= 0);

		if(getLevel() > GET_PLAYER(getOwner()).getHighestUnitLevel())
		{
			GET_PLAYER(getOwner()).setHighestUnitLevel(getLevel());
		}

		if(IsSelected())
		{
			DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
		}
	}
}


//	--------------------------------------------------------------------------------
void CvUnit::changeLevel(int iChange)
{
	VALIDATE_OBJECT
	setLevel(getLevel() + iChange);
}


//	--------------------------------------------------------------------------------
int CvUnit::getCargo() const
{
	VALIDATE_OBJECT
	return m_iCargo;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeCargo(int iChange)
{
	VALIDATE_OBJECT
	m_iCargo = (m_iCargo + iChange);
	CvAssert(getCargo() >= 0);
}


//	--------------------------------------------------------------------------------
CvPlot* CvUnit::getAttackPlot() const
{
	VALIDATE_OBJECT
	return GC.getMap().plotCheckInvalid(m_iAttackPlotX, m_iAttackPlotY);
}


//	--------------------------------------------------------------------------------
void CvUnit::setAttackPlot(const CvPlot* pNewValue, bool bAirCombat)
{
	VALIDATE_OBJECT
	if(getAttackPlot() != pNewValue)
	{
		if(pNewValue != NULL)
		{
			m_iAttackPlotX = pNewValue->getX();
			m_iAttackPlotY = pNewValue->getY();
		}
		else
		{
			m_iAttackPlotX = INVALID_PLOT_COORD;
			m_iAttackPlotY = INVALID_PLOT_COORD;
		}
	}

	m_bAirCombat = bAirCombat;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isAirCombat() const
{
	VALIDATE_OBJECT
	return m_bAirCombat;
}


//	--------------------------------------------------------------------------------
int CvUnit::getCombatTimer() const
{
	VALIDATE_OBJECT
	return m_iCombatTimer;
}


//	--------------------------------------------------------------------------------
void CvUnit::setCombatTimer(int iNewValue)
{
	VALIDATE_OBJECT
	m_iCombatTimer = iNewValue;
	CvAssert(getCombatTimer() >= 0);
}


//	--------------------------------------------------------------------------------
void CvUnit::changeCombatTimer(int iChange)
{
	VALIDATE_OBJECT
	setCombatTimer(getCombatTimer() + iChange);
}


//	--------------------------------------------------------------------------------
int CvUnit::getCombatFirstStrikes() const
{
	VALIDATE_OBJECT
	return m_iCombatFirstStrikes;
}


//	--------------------------------------------------------------------------------
void CvUnit::setCombatFirstStrikes(int iNewValue)
{
	VALIDATE_OBJECT
	m_iCombatFirstStrikes = iNewValue;
	CvAssert(getCombatFirstStrikes() >= 0);
}


//	--------------------------------------------------------------------------------
void CvUnit::changeCombatFirstStrikes(int iChange)
{
	VALIDATE_OBJECT
	setCombatFirstStrikes(getCombatFirstStrikes() + iChange);
}

bool CvUnit::CanGarrison() const
{
	return GetMapLayer()==DEFAULT_UNIT_MAP_LAYER && getDomainType()==DOMAIN_LAND && IsCanDefend();
}

int CvUnit::GetMapLayer() const
{
	return m_iMapLayer;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsGarrisoned(void) const
{
	VALIDATE_OBJECT

	return (m_iGarrisonCityID != -1);
}

void CvUnit::SetGarrisonedCity(int iCityID)
{
	m_iGarrisonCityID = iCityID;
}

//	--------------------------------------------------------------------------------
CvCity* CvUnit::GetGarrisonedCity()
{
	if(IsGarrisoned())
		return GET_PLAYER(getOwner()).getCity(m_iGarrisonCityID);

	return NULL;
}

//	--------------------------------------------------------------------------------
int CvUnit::getFortifyTurns() const
{
	VALIDATE_OBJECT
	return m_iFortifyTurns;
}

//	--------------------------------------------------------------------------------
void CvUnit::setFortifyTurns(int iNewValue)
{
	VALIDATE_OBJECT
	iNewValue = range(iNewValue, 0, GC.getMAX_FORTIFY_TURNS());

	if(iNewValue != getFortifyTurns())
	{
		// Unit subtly slipped into Fortification state by remaining stationary for a turn
		if(getFortifyTurns() == 0 && iNewValue > 0)
		{
			auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
			gDLL->GameplayUnitFortify(pDllUnit.get(), true);
		}

		m_iFortifyTurns = iNewValue;
		setInfoBarDirty(true);

		// Fortification turned off, send an event noting this
		if(iNewValue == 0)
		{
			SetFortifiedThisTurn(false);

			auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
			gDLL->GameplayUnitFortify(pDllUnit.get(), false);
		}
	}
}


//	--------------------------------------------------------------------------------
void CvUnit::changeFortifyTurns(int iChange)
{
	VALIDATE_OBJECT
	setFortifyTurns(getFortifyTurns() + iChange);
}


//	--------------------------------------------------------------------------------
bool CvUnit::IsFortifiedThisTurn() const
{
	VALIDATE_OBJECT
	return m_bFortifiedThisTurn;
}


//	--------------------------------------------------------------------------------
void CvUnit::SetFortifiedThisTurn(bool bValue)
{
	VALIDATE_OBJECT
	if(!IsEverFortifyable() && bValue)
	{
		return;
	}

	if(IsFortifiedThisTurn() != bValue)
	{
		m_bFortifiedThisTurn = bValue;

		if(bValue)
		{
			int iTurnsToFortify = 1;
			if(!isFortifyable())
			{
				iTurnsToFortify = 0;
			}

			// Manually set us to being fortified for the first turn (so we get the Fort bonus immediately)
			setFortifyTurns(iTurnsToFortify);

			if(iTurnsToFortify > 0)
			{
				auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
				gDLL->GameplayUnitFortify(pDllUnit.get(), true);
			}
		}
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getBlitzCount() const
{
	VALIDATE_OBJECT
	return m_iBlitzCount;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isBlitz() const
{
	VALIDATE_OBJECT
	if(GetRange() > 0)
	{
		return false;
	}
	return (getBlitzCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvUnit::changeBlitzCount(int iChange)
{
	VALIDATE_OBJECT
	m_iBlitzCount = (m_iBlitzCount + iChange);
	CvAssert(getBlitzCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getAmphibCount() const
{
	VALIDATE_OBJECT
	return m_iAmphibCount;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isAmphib() const
{
	VALIDATE_OBJECT
	return (getAmphibCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvUnit::changeAmphibCount(int iChange)
{
	VALIDATE_OBJECT
	m_iAmphibCount = (m_iAmphibCount + iChange);
	CvAssert(getAmphibCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getRiverCrossingNoPenaltyCount() const
{
	VALIDATE_OBJECT
	return m_iRiverCrossingNoPenaltyCount;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isRiverCrossingNoPenalty() const
{
	VALIDATE_OBJECT
	return (getRiverCrossingNoPenaltyCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvUnit::changeRiverCrossingNoPenaltyCount(int iChange)
{
	VALIDATE_OBJECT
	m_iRiverCrossingNoPenaltyCount = (m_iRiverCrossingNoPenaltyCount + iChange);
	CvAssert(getRiverCrossingNoPenaltyCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getEnemyRouteCount() const
{
	VALIDATE_OBJECT
	return m_iEnemyRouteCount;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isEnemyRoute() const
{
	VALIDATE_OBJECT
	return (getEnemyRouteCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvUnit::changeEnemyRouteCount(int iChange)
{
	VALIDATE_OBJECT
	m_iEnemyRouteCount = (m_iEnemyRouteCount + iChange);
	CvAssert(getEnemyRouteCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getAlwaysHealCount() const
{
	VALIDATE_OBJECT
	return m_iAlwaysHealCount;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isAlwaysHeal() const
{
	VALIDATE_OBJECT
	return (getAlwaysHealCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvUnit::changeAlwaysHealCount(int iChange)
{
	VALIDATE_OBJECT
	m_iAlwaysHealCount = (m_iAlwaysHealCount + iChange);
	CvAssert(getAlwaysHealCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getHealOutsideFriendlyCount() const
{
	VALIDATE_OBJECT
	return m_iHealOutsideFriendlyCount;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isHealOutsideFriendly() const
{
	VALIDATE_OBJECT
	return (getHealOutsideFriendlyCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvUnit::changeHealOutsideFriendlyCount(int iChange)
{
	VALIDATE_OBJECT
	m_iHealOutsideFriendlyCount = (m_iHealOutsideFriendlyCount + iChange);
	CvAssert(getHealOutsideFriendlyCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getHillsDoubleMoveCount() const
{
	VALIDATE_OBJECT
	return m_iHillsDoubleMoveCount;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isHillsDoubleMove() const
{
	VALIDATE_OBJECT
	return (getHillsDoubleMoveCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvUnit::changeHillsDoubleMoveCount(int iChange)
{
	VALIDATE_OBJECT
	m_iHillsDoubleMoveCount = (m_iHillsDoubleMoveCount + iChange);
	CvAssert(getHillsDoubleMoveCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getImmuneToFirstStrikesCount() const
{
	VALIDATE_OBJECT
	return m_iImmuneToFirstStrikesCount;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeImmuneToFirstStrikesCount(int iChange)
{
	VALIDATE_OBJECT
	m_iImmuneToFirstStrikesCount = (m_iImmuneToFirstStrikesCount + iChange);
	CvAssert(getImmuneToFirstStrikesCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraVisibilityRange() const
{
	VALIDATE_OBJECT
	return m_iExtraVisibilityRange;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraVisibilityRange(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		CvPlot* pkPlot = plot();
		if (canChangeVisibility())
			pkPlot->changeAdjacentSight(getTeam(), visibilityRange(), false, getSeeInvisibleType(), getFacingDirection(true));
		m_iExtraVisibilityRange = (m_iExtraVisibilityRange + iChange);
		if (canChangeVisibility())
			pkPlot->changeAdjacentSight(getTeam(), visibilityRange(), true, getSeeInvisibleType(), getFacingDirection(true));
	}
}


#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
//	--------------------------------------------------------------------------------
int CvUnit::getExtraReconRange() const
{
	VALIDATE_OBJECT
	return m_iExtraReconRange;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraReconRange(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		CvPlot* pkPlot = plot();
		pkPlot->changeAdjacentSight(getTeam(), reconRange(), false, getSeeInvisibleType(), getFacingDirection(true));
		m_iExtraReconRange = (m_iExtraReconRange + iChange);
		pkPlot->changeAdjacentSight(getTeam(), reconRange(), true, getSeeInvisibleType(), getFacingDirection(true));
	}
}
#endif


//	--------------------------------------------------------------------------------
int CvUnit::getExtraMoves() const
{
	VALIDATE_OBJECT
	return m_iExtraMoves;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraMoves(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraMoves += iChange;
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraNavalMoves() const
{
	VALIDATE_OBJECT
	return m_iExtraNavalMoves;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraNavalMoves(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraNavalMoves += iChange;
	CvAssert(getExtraNavalMoves() >= 0);
}



//	--------------------------------------------------------------------------------
int CvUnit::getExtraMoveDiscount() const
{
	VALIDATE_OBJECT
	return m_iExtraMoveDiscount;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraMoveDiscount(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraMoveDiscount = (m_iExtraMoveDiscount + iChange);
	CvAssert(getExtraMoveDiscount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraRange() const
{
	VALIDATE_OBJECT
	return m_iExtraRange;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraRange(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraRange += iChange;
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraIntercept() const
{
	VALIDATE_OBJECT
	return m_iExtraIntercept;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraIntercept(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraIntercept += iChange;
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraEvasion() const
{
	VALIDATE_OBJECT
	return m_iExtraEvasion;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraEvasion(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraEvasion += iChange;
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraFirstStrikes() const
{
	VALIDATE_OBJECT
	return m_iExtraFirstStrikes;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraFirstStrikes(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraFirstStrikes = (m_iExtraFirstStrikes + iChange);
	CvAssert(getExtraFirstStrikes() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraChanceFirstStrikes() const
{
	VALIDATE_OBJECT
	return m_iExtraChanceFirstStrikes;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraChanceFirstStrikes(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraChanceFirstStrikes = (m_iExtraChanceFirstStrikes + iChange);
	CvAssert(getExtraChanceFirstStrikes() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraWithdrawal() const
{
	VALIDATE_OBJECT
	return m_iExtraWithdrawal;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraWithdrawal(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraWithdrawal = (m_iExtraWithdrawal + iChange);
	CvAssert(getExtraWithdrawal() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraEnemyHeal() const
{
	VALIDATE_OBJECT
	return m_iExtraEnemyHeal;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraEnemyHeal(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraEnemyHeal = (m_iExtraEnemyHeal + iChange);
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraNeutralHeal() const
{
	VALIDATE_OBJECT
	return m_iExtraNeutralHeal;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraNeutralHeal(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraNeutralHeal = (m_iExtraNeutralHeal + iChange);
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraFriendlyHeal() const
{
	VALIDATE_OBJECT
	return m_iExtraFriendlyHeal;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraFriendlyHeal(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraFriendlyHeal = (m_iExtraFriendlyHeal + iChange);
	CvAssert(getExtraFriendlyHeal() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getSameTileHeal() const
{
	VALIDATE_OBJECT
	return m_iSameTileHeal;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeSameTileHeal(int iChange)
{
	VALIDATE_OBJECT
	m_iSameTileHeal = (m_iSameTileHeal + iChange);
	CvAssert(getSameTileHeal() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getAdjacentTileHeal() const
{
	VALIDATE_OBJECT
	return m_iAdjacentTileHeal;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeAdjacentTileHeal(int iChange)
{
	VALIDATE_OBJECT
	m_iAdjacentTileHeal = (m_iAdjacentTileHeal + iChange);
	CvAssert(getAdjacentTileHeal() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getEnemyDamageChance() const
{
	VALIDATE_OBJECT
	return m_iEnemyDamageChance;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeEnemyDamageChance(int iChange)
{
	VALIDATE_OBJECT
	m_iEnemyDamageChance = (m_iEnemyDamageChance + iChange);
	CvAssert(getEnemyDamageChance() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getNeutralDamageChance() const
{
	VALIDATE_OBJECT
	return m_iNeutralDamageChance;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeNeutralDamageChance(int iChange)
{
	VALIDATE_OBJECT
	m_iNeutralDamageChance = (m_iNeutralDamageChance + iChange);
	CvAssert(getNeutralDamageChance() >= 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getEnemyDamage() const
{
	VALIDATE_OBJECT
	return m_iEnemyDamage;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeEnemyDamage(int iChange)
{
	VALIDATE_OBJECT
	m_iEnemyDamage = (m_iEnemyDamage + iChange);
#if defined(MOD_BALANCE_CORE)
#else
	CvAssert(getEnemyDamage() >= 0);
#endif
}

//	--------------------------------------------------------------------------------
int CvUnit::getNeutralDamage() const
{
	VALIDATE_OBJECT
	return m_iNeutralDamage;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeNeutralDamage(int iChange)
{
	VALIDATE_OBJECT
	m_iNeutralDamage = (m_iNeutralDamage + iChange);
	CvAssert(getNeutralDamage() >= 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getNearbyEnemyCombatMod() const
{
	VALIDATE_OBJECT
	return m_iNearbyEnemyCombatMod;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeNearbyEnemyCombatMod(int iChange)
{
	VALIDATE_OBJECT
	m_iNearbyEnemyCombatMod = (m_iNearbyEnemyCombatMod + iChange);
}


//	--------------------------------------------------------------------------------
int CvUnit::getNearbyEnemyCombatRange() const
{
	VALIDATE_OBJECT
	return m_iNearbyEnemyCombatRange;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeNearbyEnemyCombatRange(int iChange)
{
	VALIDATE_OBJECT
	m_iNearbyEnemyCombatRange = (m_iNearbyEnemyCombatRange + iChange);
	CvAssert(getNearbyEnemyCombatRange() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraCombatPercent() const
{
	VALIDATE_OBJECT
	return m_iExtraCombatPercent;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraCombatPercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraCombatPercent = (m_iExtraCombatPercent + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraCityAttackPercent() const
{
	VALIDATE_OBJECT
	return m_iExtraCityAttackPercent;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraCityAttackPercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraCityAttackPercent = (m_iExtraCityAttackPercent + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraCityDefensePercent() const
{
	VALIDATE_OBJECT
	return m_iExtraCityDefensePercent;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraCityDefensePercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraCityDefensePercent = (m_iExtraCityDefensePercent + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraRangedDefenseModifier() const
{
	VALIDATE_OBJECT
	return m_iExtraRangedDefenseModifier;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraRangedDefenseModifier(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraRangedDefenseModifier = (m_iExtraRangedDefenseModifier + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraHillsAttackPercent() const
{
	VALIDATE_OBJECT
	return m_iExtraHillsAttackPercent;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraHillsAttackPercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraHillsAttackPercent = (m_iExtraHillsAttackPercent + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraHillsDefensePercent() const
{
	VALIDATE_OBJECT
	return m_iExtraHillsDefensePercent;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraHillsDefensePercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraHillsDefensePercent = (m_iExtraHillsDefensePercent + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraOpenAttackPercent() const
{
	VALIDATE_OBJECT
	return m_iExtraOpenAttackPercent;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraOpenAttackPercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraOpenAttackPercent = (m_iExtraOpenAttackPercent + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraOpenRangedAttackMod() const
{
	VALIDATE_OBJECT
	return m_iExtraOpenRangedAttackMod;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraOpenRangedAttackMod(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraOpenRangedAttackMod = (m_iExtraOpenRangedAttackMod + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraRoughAttackPercent() const
{
	VALIDATE_OBJECT
	return m_iExtraRoughAttackPercent;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraRoughAttackPercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraRoughAttackPercent = (m_iExtraRoughAttackPercent + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraRoughRangedAttackMod() const
{
	VALIDATE_OBJECT
	return m_iExtraRoughRangedAttackMod;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraRoughRangedAttackMod(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraRoughRangedAttackMod = (m_iExtraRoughRangedAttackMod + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraAttackFortifiedMod() const
{
	VALIDATE_OBJECT
	return m_iExtraAttackFortifiedMod;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraAttackFortifiedMod(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraAttackFortifiedMod = (m_iExtraAttackFortifiedMod + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraAttackWoundedMod() const
{
	VALIDATE_OBJECT
	return m_iExtraAttackWoundedMod;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraAttackWoundedMod(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraAttackWoundedMod = (m_iExtraAttackWoundedMod + iChange);

		setInfoBarDirty(true);
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::GetFlankAttackModifier() const
{
	VALIDATE_OBJECT
	return m_iFlankAttackModifier;
}


//	--------------------------------------------------------------------------------
void CvUnit::ChangeFlankAttackModifier(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iFlankAttackModifier = (m_iFlankAttackModifier + iChange);

		setInfoBarDirty(true);
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::getExtraOpenDefensePercent() const
{
	VALIDATE_OBJECT
	return m_iExtraOpenDefensePercent;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraOpenDefensePercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraOpenDefensePercent = (m_iExtraOpenDefensePercent + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraRoughDefensePercent() const
{
	VALIDATE_OBJECT
	return m_iExtraRoughDefensePercent;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraRoughDefensePercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraRoughDefensePercent = (m_iExtraRoughDefensePercent + iChange);

		setInfoBarDirty(true);
	}
}

#ifdef AUI_UNIT_EXTRA_ATTACKS_GETTER
//	--------------------------------------------------------------------------------
int CvUnit::getNumAttacks() const
{
	VALIDATE_OBJECT
	return m_iNumAttacks;
}
int CvUnit::getNumAttacksMadeThisTurn() const
{
	VALIDATE_OBJECT
	return m_iAttacksMade;
}
#endif // AUI_UNIT_EXTRA_ATTACKS_GETTER

//	--------------------------------------------------------------------------------
void CvUnit::changeExtraAttacks(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iNumAttacks += iChange;

		setInfoBarDirty(true);
	}
}

//	--------------------------------------------------------------------------------
// Citadel
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
bool CvUnit::IsNearEnemyCitadel(int& iCitadelDamage, const CvPlot* pInPlot) const
#else
bool CvUnit::IsNearEnemyCitadel(int& iCitadelDamage)
#endif
{
	VALIDATE_OBJECT

#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	if (pInPlot == NULL)
		pInPlot = plot();
#endif

	return pInPlot->IsNearEnemyCitadel( getOwner(), &iCitadelDamage);
}

//	--------------------------------------------------------------------------------
/// Great General close enough to give us a bonus?
#if defined(MOD_BALANCE_CORE_MILITARY)

bool CvUnit::IsNearGreatGeneral(const CvPlot* pAtPlot, const CvUnit* pIgnoreThisGeneral) const
{
	VALIDATE_OBJECT

	int iGreatGeneralRange = /*2*/ GC.getGREAT_GENERAL_RANGE();

	if (pAtPlot == NULL)
	{
		pAtPlot = plot();
		if (pAtPlot == NULL)
			return false;
	}

	const std::vector<int>& possibleUnits = GET_PLAYER(getOwner()).GetAreaEffectPositiveUnits();
	for(std::vector<int>::const_iterator it = possibleUnits.begin(); it!=possibleUnits.end(); ++it)
	{
		UnitHandle pUnit = GET_PLAYER(getOwner()).getUnit(*it);

		if (pUnit && pUnit != pIgnoreThisGeneral)
		{
			// Same domain
			if(pUnit->getDomainType() == getDomainType())
			{
				if (plotDistance( pAtPlot->getX(),pAtPlot->getY(),pUnit->getX(),pUnit->getY() ) <= iGreatGeneralRange)
					return true;
			}
		}

	}

	return false;
}

#else

#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
#ifdef AUI_TACTICAL_FIX_SCORE_GREAT_GENERAL_PLOT_NO_OVERLAP
bool CvUnit::IsNearGreatGeneral(const CvPlot* pAtPlot, const CvUnit* pIgnoreThisGeneral) const
#else
bool CvUnit::IsNearGreatGeneral(const CvPlot* pAtPlot) const
#endif
#else
bool CvUnit::IsNearGreatGeneral() const
#endif
{
	VALIDATE_OBJECT

	int iGreatGeneralRange = /*2*/ GC.getGREAT_GENERAL_RANGE();

	CvPlot* pLoopPlot;
	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	if (pAtPlot == NULL)
	{
		pAtPlot = plot();
		if (pAtPlot == NULL)
			return false;
	}
#endif

	// Look around this Unit to see if there's a Great General nearby
	for(int iX = -iGreatGeneralRange; iX <= iGreatGeneralRange; iX++)
	{
		for(int iY = -iGreatGeneralRange; iY <= iGreatGeneralRange; iY++)
		{
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
			pLoopPlot = plotXYWithRangeCheck(pAtPlot->getX(), pAtPlot->getY(), iX, iY, iGreatGeneralRange);
#else
			pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iX, iY, iGreatGeneralRange);
#endif

			if(pLoopPlot != NULL)
			{
				// If there are Units here, loop through them
				if(pLoopPlot->getNumUnits() > 0)
				{
					pUnitNode = pLoopPlot->headUnitNode();

					while(pUnitNode != NULL)
					{
						pLoopUnit = ::getUnit(*pUnitNode);
						pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

						// Owned by us
#ifdef AUI_TACTICAL_FIX_SCORE_GREAT_GENERAL_PLOT_NO_OVERLAP
						if (pLoopUnit && pLoopUnit->getOwner() == getOwner() && pLoopUnit != pIgnoreThisGeneral)
#else
						if(pLoopUnit && pLoopUnit->getOwner() == getOwner())
#endif
						{
							// Great General unit
							if(pLoopUnit->IsGreatGeneral() || pLoopUnit->IsGreatAdmiral())
							{
								// Same domain
								if(pLoopUnit->getDomainType() == getDomainType())
								{
									return true;
								}
							}
						}
					}
				}
			}
		}
	}

	return false;
}

#endif

//	--------------------------------------------------------------------------------
/// Great General in our hex?
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
#ifdef AUI_TACTICAL_FIX_SCORE_GREAT_GENERAL_PLOT_NO_OVERLAP
bool CvUnit::IsStackedGreatGeneral(const CvPlot* pLoopPlot, const CvUnit* pIgnoreThisGeneral) const
#else
bool CvUnit::IsStackedGreatGeneral(const CvPlot* pLoopPlot) const
#endif
{
	if (pLoopPlot == NULL)
	{
		pLoopPlot = plot();
		if (pLoopPlot == NULL)
			return false;
	}
	const IDInfo* pUnitNode;
	CvUnit* pLoopUnit;
#else
bool CvUnit::IsStackedGreatGeneral() const
{
	CvPlot* pLoopPlot = plot();
	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;

	if(pLoopPlot != NULL)
#endif
	{
		// If there are Units here, loop through them
		if(pLoopPlot->getNumUnits() > 0)
		{
			pUnitNode = pLoopPlot->headUnitNode();

			while(pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(*pUnitNode);
				pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

#ifdef AUI_TACTICAL_FIX_SCORE_GREAT_GENERAL_PLOT_NO_OVERLAP
				if (pLoopUnit && pLoopUnit != pIgnoreThisGeneral)
#else
				if(pLoopUnit)
#endif
				{
					// Great General unit
#if defined(MOD_BUGFIX_MINOR)
					// IsNearGreatGeneral() includes GAs, so we'll add them here as well
					if(pLoopUnit->IsGreatGeneral() || pLoopUnit->IsGreatAdmiral())
#else
					if(pLoopUnit->IsGreatGeneral())
#endif
					{
						// Same domain
						if(pLoopUnit->getDomainType() == getDomainType())
						{
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
int CvUnit::GetGreatGeneralStackMovement(const CvPlot* pLoopPlot) const
{
	int iRtnValue = baseMoves() * GC.getMOVE_DENOMINATOR();

	if (pLoopPlot == NULL)
		pLoopPlot = plot();
	const IDInfo* pUnitNode;
	CvUnit* pLoopUnit;
#else
int CvUnit::GetGreatGeneralStackMovement() const
{
	int iRtnValue = baseMoves() * GC.getMOVE_DENOMINATOR();

	CvPlot* pLoopPlot = plot();
	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;

	if(pLoopPlot != NULL)
#endif
	{
		// If there are Units here, loop through them
		if(pLoopPlot->getNumUnits() > 0)
		{
			pUnitNode = pLoopPlot->headUnitNode();

			while(pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(*pUnitNode);
				pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

				if(pLoopUnit)
				{
					// Give movement to great general?
					if(pLoopUnit->IsGreatGeneralReceivesMovement())
					{
						// Same domain
						if(pLoopUnit->getDomainType() == getDomainType())
						{
#if defined(MOD_PROMOTIONS_FLAGSHIP)
							iRtnValue = std::max(iRtnValue, pLoopUnit->maxMoves());
#else
							iRtnValue = pLoopUnit->maxMoves();
#endif
							break;
						}
					}
				}
			}
		}
	}

	return iRtnValue;
}

#if defined(MOD_BALANCE_CORE_MILITARY)

int CvUnit::GetReverseGreatGeneralModifier(const CvPlot* pAtPlot) const
{
	VALIDATE_OBJECT

	if (pAtPlot == NULL)
	{
		pAtPlot = plot();
		if (pAtPlot == NULL)
			return 0;
	}
	
	int iMaxMod = 0;

	const std::vector<PlayerTypes>& vEnemies = GET_PLAYER(getOwner()).GetPlayersAtWarWith();

	for(std::vector<PlayerTypes>::const_iterator it=vEnemies.begin(); it!=vEnemies.end(); ++it)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER(*it);
		const std::vector<int>& possibleUnits = kLoopPlayer.GetAreaEffectNegativeUnits();

		for(std::vector<int>::const_iterator it = possibleUnits.begin(); it!=possibleUnits.end(); ++it)
		{
			UnitHandle pUnit = kLoopPlayer.getUnit(*it);

			if (pUnit)
			{
				// Unit with a combat modifier against the enemy
				int iMod = pUnit->getNearbyEnemyCombatMod();
				if(iMod != 0)
				{
					// Same domain
					if(((pUnit->getDomainType() == getDomainType()) && (pUnit->getDomainType() == DOMAIN_LAND) && !pUnit->isEmbarked()) || ((pUnit->getDomainType() == getDomainType()) && (pUnit->getDomainType() == DOMAIN_SEA)))
					{
						// Within range?
						int iRange = pUnit->getNearbyEnemyCombatRange();
						if(plotDistance(pAtPlot->getX(), pAtPlot->getY(), pUnit->getX(), pUnit->getY()) <= iRange)
						{
							// Don't assume the first one found is the worst!
							// Combat modifiers are negative, as applied against the defender (and not for the attacker)
							if (iMod < iMaxMod)
							{
								iMaxMod = iMod;
							}
						}
					}
				}
			}
		}
	}

	return iMaxMod;
}

#else

//	--------------------------------------------------------------------------------
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
int CvUnit::GetReverseGreatGeneralModifier(const CvPlot* pAtPlot) const
#else
int CvUnit::GetReverseGreatGeneralModifier()const
#endif
{
	VALIDATE_OBJECT

	int iGreatGeneralRange = /*2*/ GC.getGREAT_GENERAL_RANGE();

	CvPlot* pLoopPlot;
	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	if (pAtPlot == NULL)
	{
		pAtPlot = plot();
		if (pAtPlot == NULL)
			return 0;
	}
#endif
	
#if defined(MOD_BUGFIX_MINOR)
	int iMaxMod = 0;
#endif

	// Look around this Unit to see if there's a Great General nearby
	for(int iX = -iGreatGeneralRange; iX <= iGreatGeneralRange; iX++)
	{
		for(int iY = -iGreatGeneralRange; iY <= iGreatGeneralRange; iY++)
		{
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
			pLoopPlot = plotXYWithRangeCheck(pAtPlot->getX(), pAtPlot->getY(), iX, iY, iGreatGeneralRange);
#else
			pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iX, iY, iGreatGeneralRange);
#endif

			if(pLoopPlot != NULL)
			{
				// If there are Units here, loop through them
				if(pLoopPlot->getNumUnits() > 0)
				{
					pUnitNode = pLoopPlot->headUnitNode();

					while(pUnitNode != NULL)
					{
						pLoopUnit = ::getUnit(*pUnitNode);
						pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

						// Owned by an enemy
						if(pLoopUnit && GET_TEAM(getTeam()).isAtWar(pLoopUnit->getTeam()))
						{
							// Unit with a combat modifier against the enemy
							int iMod = pLoopUnit->getNearbyEnemyCombatMod();
							if(iMod != 0)
							{
								// Same domain
#if defined(MOD_BALANCE_CORE)
								if(((pLoopUnit->getDomainType() == getDomainType()) && (pLoopUnit->getDomainType() == DOMAIN_LAND) && !pLoopUnit->isEmbarked()) || ((pLoopUnit->getDomainType() == getDomainType()) && (pLoopUnit->getDomainType() == DOMAIN_SEA)))
#else
								if(pLoopUnit->getDomainType() == getDomainType())
#endif
								{
									// Within range?
									int iRange = pLoopUnit->getNearbyEnemyCombatRange();
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
									if(plotDistance(pAtPlot->getX(), pAtPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY()) <= iRange)
#else
									if(plotDistance(getX(), getY(), pLoopPlot->getX(), pLoopPlot->getY()) <= iRange)
#endif
									{
#if defined(MOD_BUGFIX_MINOR)
										// Don't assume the first one found is the worst!
										// Combat modifiers are negative, as applied against the defender (and not for the attacker)
										if (iMod < iMaxMod) {
											iMaxMod = iMod;
										}
#else
										return iMod;
#endif
									}
								}
							}
						}
					}
				}
			}
		}
	}

#if defined(MOD_BUGFIX_MINOR)
	return iMaxMod;
#else
	return 0;
#endif
}

#endif

//	--------------------------------------------------------------------------------
#if defined(MOD_PROMOTIONS_IMPROVEMENT_BONUS)
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
int CvUnit::GetNearbyImprovementModifier(const CvPlot* pAtPlot) const
{
	if(pAtPlot == NULL)
	{
		return 0;
	}
	return std::max(GetNearbyImprovementModifierFromTraits(pAtPlot), GetNearbyImprovementModifierFromPromotions(pAtPlot));
}

int CvUnit::GetNearbyImprovementModifierFromTraits(const CvPlot* pAtPlot)const
{
	CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
	CvPlayerTraits* playerTraits = kPlayer.GetPlayerTraits();

#if defined(MOD_BALANCE_CORE_MILITARY)

	int iImprovementRange = playerTraits->GetNearbyImprovementBonusRange();
	int iImprovementModifier = playerTraits->GetNearbyImprovementCombatBonus();

	if (pAtPlot == NULL)
	{
		pAtPlot = plot();
		if (pAtPlot == NULL)
			return 0;
	}

	const std::vector<int>& possiblePlots = GET_PLAYER(getOwner()).GetAreaEffectPositiveFromTraitsPlots();
	for(std::vector<int>::const_iterator it = possiblePlots.begin(); it!=possiblePlots.end(); ++it)
	{
		CvPlot* pCandidatePlot = GC.getMap().plotByIndexUnchecked(*it);

		if (pCandidatePlot)
		{
			if (plotDistance( pAtPlot->getX(),pAtPlot->getY(),pCandidatePlot->getX(),pCandidatePlot->getY() ) <= iImprovementRange)
				return iImprovementModifier;
		}

	}

	return 0;

#else

	return GetNearbyImprovementModifier(playerTraits->GetCombatBonusImprovementType(), playerTraits->GetNearbyImprovementBonusRange(), playerTraits->GetNearbyImprovementCombatBonus(), pAtPlot);

#endif
}

int CvUnit::GetNearbyImprovementModifierFromPromotions(const CvPlot* pAtPlot)const
{
	return GetNearbyImprovementModifier(GetCombatBonusImprovement(), GetNearbyImprovementBonusRange(), GetNearbyImprovementCombatBonus(), pAtPlot);
}

int CvUnit::GetNearbyImprovementModifier(ImprovementTypes eBonusImprovement, int iImprovementRange, int iImprovementModifier, const CvPlot* pAtPlot)const

#else
int CvUnit::GetNearbyImprovementModifier()const
{
	return std::max(GetNearbyImprovementModifierFromTraits(), GetNearbyImprovementModifierFromPromotions());
}

int CvUnit::GetNearbyImprovementModifierFromTraits()const
{
	CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
	CvPlayerTraits* playerTraits = kPlayer.GetPlayerTraits();

	return GetNearbyImprovementModifier(playerTraits->GetCombatBonusImprovementType(), playerTraits->GetNearbyImprovementBonusRange(), playerTraits->GetNearbyImprovementCombatBonus());
}

int CvUnit::GetNearbyImprovementModifierFromPromotions()const
{
	return GetNearbyImprovementModifier(GetCombatBonusImprovement(), GetNearbyImprovementBonusRange(), GetNearbyImprovementCombatBonus());
}

int CvUnit::GetNearbyImprovementModifier(ImprovementTypes eBonusImprovement, int iImprovementRange, int iImprovementModifier)const
#endif

#else

int CvUnit::GetNearbyImprovementModifier()const
#endif
{
	VALIDATE_OBJECT
#if !defined(MOD_PROMOTIONS_IMPROVEMENT_BONUS)
	CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
	int iImprovementRange = kPlayer.GetPlayerTraits()->GetNearbyImprovementBonusRange();
	int iImprovementModifier = kPlayer.GetPlayerTraits()->GetNearbyImprovementCombatBonus();
#endif

	if(iImprovementModifier != 0)
	{
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
		if (pAtPlot == NULL)
		{
			pAtPlot = plot();
			if (pAtPlot == NULL)
				return false;
		}
#endif
		CvPlot* pLoopPlot;

		// Look around this Unit to see if there's an improvement nearby
		for(int iX = -iImprovementRange; iX <= iImprovementRange; iX++)
		{
			for(int iY = -iImprovementRange; iY <= iImprovementRange; iY++)
			{
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
				pLoopPlot = plotXYWithRangeCheck(pAtPlot->getX(), pAtPlot->getY(), iX, iY, iImprovementRange);
#else
				pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iX, iY, iImprovementRange);
#endif

				if(pLoopPlot != NULL)
				{
					// Is the right improvement here?
#if defined(MOD_PROMOTIONS_IMPROVEMENT_BONUS)
					if(pLoopPlot->getImprovementType() == eBonusImprovement)
#else
					if(pLoopPlot->getImprovementType() == kPlayer.GetPlayerTraits()->GetCombatBonusImprovementType())
#endif
					{
						return iImprovementModifier;
					}
				}
			}
		}
	}

	return 0;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsGreatGeneral() const
{
	VALIDATE_OBJECT

#if defined(MOD_BALANCE_CORE_MILITARY)
	if( AI_getUnitAIType() == UNITAI_GENERAL )
		return true;
#endif

	return GetGreatGeneralCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetGreatGeneralCount() const
{
	VALIDATE_OBJECT
	return m_iGreatGeneralCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeGreatGeneralCount(int iChange)
{
	VALIDATE_OBJECT
	m_iGreatGeneralCount += iChange;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsGreatAdmiral() const
{
	VALIDATE_OBJECT

#if defined(MOD_BALANCE_CORE_MILITARY)
	if( AI_getUnitAIType() == UNITAI_ADMIRAL )
		return true;
#endif

	return GetGreatAdmiralCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetGreatAdmiralCount() const
{
	VALIDATE_OBJECT
	return m_iGreatAdmiralCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeGreatAdmiralCount(int iChange)
{
	VALIDATE_OBJECT
	m_iGreatAdmiralCount += iChange;
}

//	--------------------------------------------------------------------------------
int CvUnit::getGreatGeneralModifier() const
{
	VALIDATE_OBJECT
	return m_iGreatGeneralModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeGreatGeneralModifier(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iGreatGeneralModifier += iChange;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsGreatGeneralReceivesMovement() const
{
	return m_iGreatGeneralReceivesMovementCount > 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeGreatGeneralReceivesMovementCount(int iChange)
{
	m_iGreatGeneralReceivesMovementCount += iChange;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetGreatGeneralCombatModifier() const
{
	return m_iGreatGeneralCombatModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeGreatGeneralCombatModifier(int iChange)
{
	m_iGreatGeneralCombatModifier += iChange;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsIgnoreGreatGeneralBenefit() const
{
	return m_iIgnoreGreatGeneralBenefit > 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeIgnoreGreatGeneralBenefitCount(int iChange)
{
	m_iIgnoreGreatGeneralBenefit += iChange;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsIgnoreZOC() const
{
	return m_iIgnoreZOC > 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeIgnoreZOCCount(int iChange)
{
	m_iIgnoreZOC += iChange;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsSapper() const
{
	return (m_iSapperCount > 0);
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeSapperCount(int iChange)
{
	m_iSapperCount += iChange;
}

//	--------------------------------------------------------------------------------
/// Is a particular city being sapped by us?
bool CvUnit::IsSappingCity(const CvCity* pTargetCity) const
{
	CvAssertMsg(pTargetCity, "Target city is NULL when checking sapping combat bonus");
	if (!pTargetCity)
	{
		return false;
	}

	if (IsSapper())
	{
		if (IsEnemyCityAdjacent(pTargetCity))
		{
			return true;
		}
	}
	return false;
}

//	--------------------------------------------------------------------------------
/// Are we near a friendly unit that is sapping this city?
bool CvUnit::IsNearSapper(const CvCity* pTargetCity) const
{
	VALIDATE_OBJECT

	CvAssertMsg(pTargetCity, "Target city is NULL when checking sapping combat bonus");
	if (!pTargetCity)
	{
		return false;
	}

	int iSapperRange = GC.getSAPPER_BONUS_RANGE();

	CvPlot* pLoopPlot;
	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;

	// Look around this Unit to see if there's a Sapper nearby
	for(int iX = -iSapperRange; iX <= iSapperRange; iX++)
	{
		for(int iY = -iSapperRange; iY <= iSapperRange; iY++)
		{
			pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iX, iY, iSapperRange);

			if(pLoopPlot != NULL)
			{
				// Units in our plot do not count
				if (pLoopPlot->getX() == getX() && pLoopPlot->getY() == getY())
					continue;

				// If there are Units here, loop through them
				if(pLoopPlot->getNumUnits() > 0)
				{
					pUnitNode = pLoopPlot->headUnitNode();

					while(pUnitNode != NULL)
					{
						pLoopUnit = ::getUnit(*pUnitNode);
						pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

						// Owned by us
						if(pLoopUnit && pLoopUnit->getOwner() == getOwner())
						{
							// Sapper unit
							if(pLoopUnit->IsSappingCity(pTargetCity))
							{
								// Must be same domain
								if(pLoopUnit->getDomainType() == getDomainType())
								{
									return true;
								}
							}
						}
					}
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsCanHeavyCharge() const
{
	return m_iCanHeavyCharge > 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeCanHeavyChargeCount(int iChange)
{
	m_iCanHeavyCharge += iChange;
}

//	--------------------------------------------------------------------------------
int CvUnit::getFriendlyLandsModifier() const
{
	VALIDATE_OBJECT
	return m_iFriendlyLandsModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeFriendlyLandsModifier(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iFriendlyLandsModifier += iChange;
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::getFriendlyLandsAttackModifier() const
{
	VALIDATE_OBJECT
	return m_iFriendlyLandsAttackModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeFriendlyLandsAttackModifier(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iFriendlyLandsAttackModifier += iChange;
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::getOutsideFriendlyLandsModifier() const
{
	VALIDATE_OBJECT
	return m_iOutsideFriendlyLandsModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeOutsideFriendlyLandsModifier(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iOutsideFriendlyLandsModifier += iChange;
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::getPillageChange() const
{
	VALIDATE_OBJECT
	return m_iPillageChange;
}

//	--------------------------------------------------------------------------------
void CvUnit::changePillageChange(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iPillageChange += iChange;

		setInfoBarDirty(true);
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::getUpgradeDiscount() const
{
	VALIDATE_OBJECT
	return m_iUpgradeDiscount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeUpgradeDiscount(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iUpgradeDiscount += iChange;

		setInfoBarDirty(true);
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::getExperiencePercent() const
{
	VALIDATE_OBJECT
	return m_iExperiencePercent;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeExperiencePercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExperiencePercent += iChange;

		setInfoBarDirty(true);
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::getKamikazePercent() const
{
	VALIDATE_OBJECT
	return m_iKamikazePercent;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeKamikazePercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iKamikazePercent += iChange;

		setInfoBarDirty(true);
	}
}

//	--------------------------------------------------------------------------------
DirectionTypes CvUnit::getFacingDirection(bool checkLineOfSightProperty) const
{
	VALIDATE_OBJECT
	if(checkLineOfSightProperty)
	{
		return NO_DIRECTION; //look in all directions - LOS property has been removed from units (wasn't being used)
	}
	else
	{
		return m_eFacingDirection;
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::setFacingDirection(DirectionTypes facingDirection)
{
	VALIDATE_OBJECT
	if(facingDirection != m_eFacingDirection)
	{
		//remove old fog
		if (canChangeVisibility())
			plot()->changeAdjacentSight(getTeam(), visibilityRange(), false, getSeeInvisibleType(), getFacingDirection(true));

		//change direction
		m_eFacingDirection = facingDirection;

		//clear new fog
		if (canChangeVisibility())
			plot()->changeAdjacentSight(getTeam(), visibilityRange(), true, getSeeInvisibleType(), getFacingDirection(true));

		DLLUI->setDirty(ColoredPlots_DIRTY_BIT, true);
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::rotateFacingDirectionClockwise()
{
	VALIDATE_OBJECT
	//change direction
	DirectionTypes newDirection = (DirectionTypes)((m_eFacingDirection + 1) % NUM_DIRECTION_TYPES);
	setFacingDirection(newDirection);
}

//	--------------------------------------------------------------------------------
void CvUnit::rotateFacingDirectionCounterClockwise()
{
	VALIDATE_OBJECT
	//change direction
	DirectionTypes newDirection = (DirectionTypes)((m_eFacingDirection + NUM_DIRECTION_TYPES - 1) % NUM_DIRECTION_TYPES);
	setFacingDirection(newDirection);
}

//	--------------------------------------------------------------------------------
bool CvUnit::isOutOfAttacks() const
{
	VALIDATE_OBJECT

	// Units with blitz don't run out of attacks!
	if(isBlitz())
	{
		return false;
	}

	return m_iAttacksMade >= m_iNumAttacks;
}


//	--------------------------------------------------------------------------------
void CvUnit::setMadeAttack(bool bNewValue)
{
	VALIDATE_OBJECT
	if(bNewValue)
	{
		m_iAttacksMade++;
	}
	else
	{
		m_iAttacksMade = 0;
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::GetNumInterceptions() const
{
	VALIDATE_OBJECT
	return m_iNumInterceptions;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeNumInterceptions(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
		m_iNumInterceptions += iChange;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isOutOfInterceptions() const
{
	VALIDATE_OBJECT
	return getMadeInterceptionCount() >= GetNumInterceptions();
}

//	--------------------------------------------------------------------------------
int CvUnit::getMadeInterceptionCount() const
{
	return m_iMadeInterceptionCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::setMadeInterception(bool bNewValue)
{
	VALIDATE_OBJECT
	if(bNewValue)
	{
		m_iMadeInterceptionCount++;
	}
	else
	{
		m_iMadeInterceptionCount = 0;
	}
}


//	--------------------------------------------------------------------------------
bool CvUnit::isPromotionReady() const
{
	VALIDATE_OBJECT
	return m_bPromotionReady;
}


//	--------------------------------------------------------------------------------
void CvUnit::setPromotionReady(bool bNewValue)
{
	VALIDATE_OBJECT
	if(isPromotionReady() != bNewValue)
	{
		m_bPromotionReady = bNewValue;

		if(m_bPromotionReady)
		{
			SetAutomateType(NO_AUTOMATE);
			ClearMissionQueue();
			SetActivityType(ACTIVITY_AWAKE);
		}

		if(bNewValue)
		{
			CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
			if(pNotifications)
			{
				CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_UNIT_CAN_GET_PROMOTION");
				CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_UNIT_CAN_GET_PROMOTION");
				pNotifications->Add(NOTIFICATION_UNIT_PROMOTION, strBuffer, strSummary, -1, -1, getUnitType(), GetID());
#if !defined(NO_ACHIEVEMENTS)
				if(isHuman() && !GC.getGame().isGameMultiPlayer() && GET_PLAYER(GC.getGame().getActivePlayer()).isLocalPlayer())
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_UNIT_PROMOTE);
				}
#endif
			}
		}

		if(IsSelected())
		{
			DLLUI->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}


//	--------------------------------------------------------------------------------
void CvUnit::testPromotionReady()
{
	VALIDATE_OBJECT
	setPromotionReady((getExperience() >= experienceNeeded()) && canAcquirePromotionAny());
}


//	--------------------------------------------------------------------------------
bool CvUnit::isDelayedDeath() const
{
	VALIDATE_OBJECT
	return m_bDeathDelay;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isDelayedDeathExported() const
{
	VALIDATE_OBJECT
	return m_bDeathDelay;
}


//	--------------------------------------------------------------------------------
void CvUnit::startDelayedDeath()
{
	VALIDATE_OBJECT
	m_bDeathDelay = true;
}


//	--------------------------------------------------------------------------------
// Returns true if killed...
bool CvUnit::doDelayedDeath()
{
	VALIDATE_OBJECT
	if(m_bDeathDelay && !isFighting() && !IsBusy())
	{
		kill(false);
		return true;
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isCombatFocus() const
{
	VALIDATE_OBJECT
	return m_bCombatFocus;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isInfoBarDirty() const
{
	VALIDATE_OBJECT
	return m_bInfoBarDirty;
}


//	--------------------------------------------------------------------------------
void CvUnit::setInfoBarDirty(bool bNewValue)
{
	VALIDATE_OBJECT

	DLLUI->setDirty(UnitInfo_DIRTY_BIT, bNewValue);
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsNotConverting() const
{
	VALIDATE_OBJECT
	return m_bNotConverting;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetNotConverting(bool bNewValue)
{
	VALIDATE_OBJECT
	if(bNewValue != IsNotConverting())
	{
		m_bNotConverting = bNewValue;
	}
}

//	--------------------------------------------------------------------------------
PlayerTypes CvUnit::getVisualOwner(TeamTypes eForTeam) const
{
	VALIDATE_OBJECT
	if(NO_TEAM == eForTeam)
	{
		eForTeam = GC.getGame().getActiveTeam();
	}

	if(getTeam() != eForTeam && eForTeam != BARBARIAN_TEAM)
	{
		if(isHiddenNationality())
		{
			if(!plot()->isCity())
			{
				return BARBARIAN_PLAYER;
			}
		}
	}

	return getOwner();
}


//	--------------------------------------------------------------------------------
PlayerTypes CvUnit::getCombatOwner(TeamTypes eForTeam, const CvPlot& whosePlot) const
{
	VALIDATE_OBJECT
	if(eForTeam != NO_TEAM && getTeam() != eForTeam && eForTeam != BARBARIAN_TEAM)
	{
		if(isAlwaysHostile(whosePlot))
		{
			return BARBARIAN_PLAYER;
		}
	}

	return getOwner();
}

//	--------------------------------------------------------------------------------
TeamTypes CvUnit::getTeam() const
{
	VALIDATE_OBJECT
	return ::getTeam( getOwner() );
}

//	--------------------------------------------------------------------------------
PlayerTypes CvUnit::GetOriginalOwner() const
{
	return m_eOriginalOwner;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetOriginalOwner(PlayerTypes ePlayer)
{
	m_eOriginalOwner = ePlayer;
}

//	--------------------------------------------------------------------------------
PlayerTypes CvUnit::getCapturingPlayer() const
{
	VALIDATE_OBJECT
	return m_eCapturingPlayer;
}

//	--------------------------------------------------------------------------------
void CvUnit::setCapturingPlayer(PlayerTypes eNewValue)
{
	VALIDATE_OBJECT
	m_eCapturingPlayer = eNewValue;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsCapturedAsIs() const
{
	VALIDATE_OBJECT
	return m_bCapturedAsIs;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetCapturedAsIs(bool bSetValue)
{
	VALIDATE_OBJECT
	m_bCapturedAsIs = bSetValue;
}

//	--------------------------------------------------------------------------------
const UnitTypes CvUnit::getUnitType() const
{
	VALIDATE_OBJECT
	return m_eUnitType;
}

//	--------------------------------------------------------------------------------
CvUnitEntry& CvUnit::getUnitInfo() const
{
	VALIDATE_OBJECT
	return *m_pUnitInfo;
}


//	--------------------------------------------------------------------------------
UnitClassTypes CvUnit::getUnitClassType() const
{
	VALIDATE_OBJECT
	return (UnitClassTypes)getUnitInfo().GetUnitClassType();
}

//	--------------------------------------------------------------------------------
const UnitTypes CvUnit::getLeaderUnitType() const
{
	VALIDATE_OBJECT
	return m_eLeaderUnitType;
}

//	--------------------------------------------------------------------------------
void CvUnit::setLeaderUnitType(UnitTypes leaderUnitType)
{
	VALIDATE_OBJECT
	if(m_eLeaderUnitType != leaderUnitType)
	{
		m_eLeaderUnitType = leaderUnitType;
	}
}

//	--------------------------------------------------------------------------------
const InvisibleTypes CvUnit::getInvisibleType() const
{
	VALIDATE_OBJECT
	return m_eInvisibleType;
}

//	--------------------------------------------------------------------------------
void CvUnit::setInvisibleType(InvisibleTypes InvisibleType)
{
	VALIDATE_OBJECT
	if(m_eInvisibleType != InvisibleType)
	{
		m_eInvisibleType = InvisibleType;
	}
}

//	--------------------------------------------------------------------------------
const InvisibleTypes CvUnit::getSeeInvisibleType() const
{
	VALIDATE_OBJECT
	return m_eSeeInvisibleType;
}

//	--------------------------------------------------------------------------------
void CvUnit::setSeeInvisibleType(InvisibleTypes InvisibleType)
{
	VALIDATE_OBJECT
	if(m_eSeeInvisibleType != InvisibleType)
	{
		CvPlot* pPlot = GC.getMap().plotCheckInvalid(getX(), getY());
		if(pPlot && canChangeVisibility())
		{
			pPlot->changeAdjacentSight(getTeam(), visibilityRange(), false, getSeeInvisibleType(), getFacingDirection(true));
		}
		m_eSeeInvisibleType = InvisibleType;
		if(pPlot && canChangeVisibility())
		{
			pPlot->changeAdjacentSight(getTeam(), visibilityRange(), true, getSeeInvisibleType(), getFacingDirection(true));
		}
	}
}

//	--------------------------------------------------------------------------------
const CvUnit* CvUnit::getCombatUnit() const
{
	VALIDATE_OBJECT
	return getUnit(m_combatUnit);
}

//	--------------------------------------------------------------------------------
CvUnit* CvUnit::getCombatUnit()
{
	VALIDATE_OBJECT
	return getUnit(m_combatUnit);
}


//	--------------------------------------------------------------------------------
void CvUnit::setCombatUnit(CvUnit* pCombatUnit, bool bAttacking)
{
	VALIDATE_OBJECT
	if(isCombatFocus())
	{
		DLLUI->setCombatFocus(false);
	}

	if(pCombatUnit != NULL)
	{
		CvAssertMsg(getCombatUnit() == NULL && getCombatCity() == NULL, "Combat Unit or City is not expected to be assigned");
		m_bCombatFocus = (
			bAttacking && 
			!(DLLUI->isFocusedWidget()) && 
			(
				(getOwner() == GC.getGame().getActivePlayer()) || 
				(
					(pCombatUnit->getOwner() == GC.getGame().getActivePlayer()) && 
					!(GET_PLAYER(GC.getGame().getActivePlayer()).isSimultaneousTurns())
				)
			)
		);
		m_combatUnit = pCombatUnit->GetIDInfo();
	}
	else
	{
		clearCombat();
	}

	setCombatTimer(0);
	setInfoBarDirty(true);

	if(isCombatFocus())
	{
		DLLUI->setCombatFocus(true);
	}
}

//	---------------------------------------------------------------------------
const CvCity* CvUnit::getCombatCity() const
{
	VALIDATE_OBJECT
	return getCity(m_combatCity);
}

//	---------------------------------------------------------------------------
CvCity* CvUnit::getCombatCity()
{
	VALIDATE_OBJECT
	return getCity(m_combatCity);
}

//	---------------------------------------------------------------------------
void CvUnit::setCombatCity(CvCity* pCombatCity)
{
	VALIDATE_OBJECT
	if(isCombatFocus())
	{
		DLLUI->setCombatFocus(false);
	}

	if(pCombatCity != NULL)
	{
		CvAssertMsg(getCombatUnit() == NULL && getCombatCity() == NULL, "Combat Unit or City is not expected to be assigned");
		m_bCombatFocus = (!(DLLUI->isFocusedWidget()) && ((getOwner() == GC.getGame().getActivePlayer()) || ((pCombatCity->getOwner() == GC.getGame().getActivePlayer()) && !(GET_PLAYER(GC.getGame().getActivePlayer()).isSimultaneousTurns()))));
		m_combatCity = pCombatCity->GetIDInfo();
	}
	else
	{
		clearCombat();
	}

	setCombatTimer(0);
	setInfoBarDirty(true);

	if(isCombatFocus())
	{
		DLLUI->setCombatFocus(true);
	}
}

//	----------------------------------------------------------------------------
void CvUnit::clearCombat()
{
	VALIDATE_OBJECT
	if(isCombatFocus())
	{
		DLLUI->setCombatFocus(false);
	}

	m_bCombatFocus = false;
	if(getCombatUnit() != NULL || getCombatCity() != NULL)
	{
		CvAssertMsg(plot()->isUnitFighting(), "plot()->isUnitFighting is expected to be true");
		m_combatCity.reset();
		m_combatUnit.reset();
		setCombatFirstStrikes(0);

		if(IsSelected())
		{
			DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
		}

		auto_ptr<ICvPlot1> pDllSelectionPlot(DLLUI->getSelectionPlot());
		int iSelectionPlotIndex = (pDllSelectionPlot.get() != NULL)? pDllSelectionPlot->GetPlotIndex() : -1;
		if(plot()->GetPlotIndex() == iSelectionPlotIndex)
		{
			DLLUI->setDirty(PlotListButtons_DIRTY_BIT, true);
		}
	}
}

//	----------------------------------------------------------------------------
const CvUnit* CvUnit::getTransportUnit() const
{
	VALIDATE_OBJECT
	return getUnit(m_transportUnit);
}

//	--------------------------------------------------------------------------------
CvUnit* CvUnit::getTransportUnit()
{
	VALIDATE_OBJECT
	return getUnit(m_transportUnit);
}


//	--------------------------------------------------------------------------------
bool CvUnit::isCargo() const
{
	VALIDATE_OBJECT
	return !m_transportUnit.isInvalid();
}


//	--------------------------------------------------------------------------------
void CvUnit::setTransportUnit(CvUnit* pTransportUnit)
{
	VALIDATE_OBJECT
	CvUnit* pOldTransportUnit;

	pOldTransportUnit = getTransportUnit();

	if(pOldTransportUnit != pTransportUnit)
	{
		if(pOldTransportUnit != NULL)
		{
			pOldTransportUnit->changeCargo(-1);
		}

		if(pTransportUnit != NULL)
		{
			CvAssertMsg(pTransportUnit->cargoSpaceAvailable(getSpecialUnitType(), getDomainType()) > 0, "Cargo space is expected to be available");

			m_transportUnit = pTransportUnit->GetIDInfo();

			if(getDomainType() != DOMAIN_AIR)
			{
				SetActivityType(ACTIVITY_SLEEP);
			}

			if(GC.getGame().isFinalInitialized())
			{
				finishMoves();
			}

			pTransportUnit->changeCargo(1);
			pTransportUnit->SetActivityType(ACTIVITY_AWAKE);
		}
		else
		{
			m_transportUnit.reset();

			SetActivityType(ACTIVITY_AWAKE);
		}
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraDomainModifier(DomainTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_extraDomainModifiers[eIndex];
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraDomainModifier(DomainTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_extraDomainModifiers[eIndex] = (m_extraDomainModifiers[eIndex] + iChange);
}


//	--------------------------------------------------------------------------------
const CvString CvUnit::getName() const
{
	VALIDATE_OBJECT
	CvString strBuffer;

	if(m_strName.IsEmpty())
	{
#if defined(MOD_PROMOTIONS_UNIT_NAMING)
		// For units, getUnitInfo().GetTextKey() (ie getNameKey()) and getUnitInfo().GetDescription() are the same
		return GetLocalizedText(getNameKey());
#else
		return getUnitInfo().GetDescription();
#endif
	}

	Localization::String name = Localization::Lookup(m_strName);
#if defined(MOD_PROMOTIONS_UNIT_NAMING)
		// For units, getUnitInfo().GetTextKey() (ie getNameKey()) and getUnitInfo().GetDescription() are the same
	strBuffer.Format("%s (%s)", name.toUTF8(), GetLocalizedText(getNameKey()).c_str());
#else
	strBuffer.Format("%s (%s)", name.toUTF8(), getUnitInfo().GetDescription());
#endif

	return strBuffer;
}


//	--------------------------------------------------------------------------------
const char* CvUnit::getNameKey() const
{
	VALIDATE_OBJECT
#if defined(MOD_PROMOTIONS_UNIT_NAMING)
	if (MOD_PROMOTIONS_UNIT_NAMING) {
		if (!m_strUnitName.IsEmpty()) {
			return m_strUnitName.c_str();
		}
	}
#endif

	return getUnitInfo().GetTextKey();
}


#if defined(MOD_PROMOTIONS_UNIT_NAMING)
//	--------------------------------------------------------------------------------
const CvString CvUnit::getUnitName() const
{
	VALIDATE_OBJECT
	return m_strUnitName.GetCString();
}
//	--------------------------------------------------------------------------------
void CvUnit::setUnitName(const CvString strNewValue)
{
	VALIDATE_OBJECT

	m_strUnitName = strNewValue;
	DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
}
#endif

//	--------------------------------------------------------------------------------
const CvString CvUnit::getNameNoDesc() const
{
	VALIDATE_OBJECT
	return m_strName.GetCString();
}


//	--------------------------------------------------------------------------------
void CvUnit::setName(CvString strNewValue)
{
	VALIDATE_OBJECT
	gDLL->stripSpecialCharacters(strNewValue);

	m_strName = strNewValue;
	DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
}

//	--------------------------------------------------------------------------------
GreatWorkType CvUnit::GetGreatWork() const
{
	return m_eGreatWork;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetGreatWork(GreatWorkType eGreatWork)
{
	VALIDATE_OBJECT
	m_eGreatWork = eGreatWork;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetTourismBlastStrength() const
{
	return m_iTourismBlastStrength;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetTourismBlastStrength(int iValue)
{
	m_iTourismBlastStrength = iValue;
}
#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
int CvUnit::GetScienceBlastStrength() const
{
	return m_iScienceBlastStrength;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetScienceBlastStrength(int iValue)
{
	m_iScienceBlastStrength = iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetCultureBlastStrength() const
{
	return m_iCultureBlastStrength;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetCultureBlastStrength(int iValue)
{
	m_iCultureBlastStrength = iValue;
}
#endif

//	--------------------------------------------------------------------------------
std::string CvUnit::getScriptData() const
{
	VALIDATE_OBJECT
	return m_strScriptData;
}

//	--------------------------------------------------------------------------------
void CvUnit::setScriptData(std::string strNewValue)
{
	VALIDATE_OBJECT
	m_strScriptData = strNewValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::getScenarioData() const
{
	VALIDATE_OBJECT
	return m_iScenarioData;
}

//	--------------------------------------------------------------------------------
void CvUnit::setScenarioData(int iNewValue)
{
	VALIDATE_OBJECT
	m_iScenarioData = iNewValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::getTerrainDoubleMoveCount(TerrainTypes eIndex) const
{
	const std::map<TerrainTypes, int>& m_map = m_terrainDoubleMoveCount.get();
	std::map<TerrainTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second;
	else
		return 0;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isTerrainDoubleMove(TerrainTypes eIndex) const
{
	const std::map<TerrainTypes, int>& m_map = m_terrainDoubleMoveCount.get();
	std::map<TerrainTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second > 0;
	else
		return false;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeTerrainDoubleMoveCount(TerrainTypes eIndex, int iChange)
{
	std::map<TerrainTypes, int>& m_map = m_terrainDoubleMoveCount.dirtyGet();
	if (m_map.find(eIndex) != m_map.end())
	{
		m_map[eIndex] += iChange;
		if (m_map[eIndex]==0)
			m_map.erase(eIndex);
	}
	else
		m_map[eIndex] = iChange;
}


//	--------------------------------------------------------------------------------
int CvUnit::getFeatureDoubleMoveCount(FeatureTypes eIndex) const
{
	const std::map<FeatureTypes, int>& m_map = m_featureDoubleMoveCount.get();
	std::map<FeatureTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second;
	else
		return 0;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isFeatureDoubleMove(FeatureTypes eIndex) const
{
	const std::map<FeatureTypes, int>& m_map = m_featureDoubleMoveCount.get();
	std::map<FeatureTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second > 0;
	else
		return false;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeFeatureDoubleMoveCount(FeatureTypes eIndex, int iChange)
{
	std::map<FeatureTypes, int>& m_map = m_featureDoubleMoveCount.dirtyGet();
	if (m_map.find(eIndex) != m_map.end())
	{
		m_map[eIndex] += iChange;
		if (m_map[eIndex] == 0)
			m_map.erase(eIndex);
	}
	else
		m_map[eIndex] = iChange;
}


#if defined(MOD_PROMOTIONS_HALF_MOVE)
//	--------------------------------------------------------------------------------
int CvUnit::getTerrainHalfMoveCount(TerrainTypes eIndex) const
{
	const std::map<TerrainTypes, int>& m_map = m_terrainHalfMoveCount.get();
	std::map<TerrainTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second;
	else
		return 0;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isTerrainHalfMove(TerrainTypes eIndex) const
{
	const std::map<TerrainTypes, int>& m_map = m_terrainHalfMoveCount.get();
	std::map<TerrainTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second > 0;
	else
		return false;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeTerrainHalfMoveCount(TerrainTypes eIndex, int iChange)
{
	std::map<TerrainTypes, int>& m_map = m_terrainHalfMoveCount.dirtyGet();
	if (m_map.find(eIndex) != m_map.end())
	{
		m_map[eIndex] += iChange;
		if (m_map[eIndex]==0)
			m_map.erase(eIndex);
	}
	else
		m_map[eIndex] = iChange;
}


//	--------------------------------------------------------------------------------
int CvUnit::getFeatureHalfMoveCount(FeatureTypes eIndex) const
{
	const std::map<FeatureTypes, int>& m_map = m_featureHalfMoveCount.get();
	std::map<FeatureTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second;
	else
		return 0;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isFeatureHalfMove(FeatureTypes eIndex) const
{
	const std::map<FeatureTypes, int>& m_map = m_featureHalfMoveCount.get();
	std::map<FeatureTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second > 0;
	else
		return false;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeFeatureHalfMoveCount(FeatureTypes eIndex, int iChange)
{
	std::map<FeatureTypes, int>& m_map = m_featureHalfMoveCount.dirtyGet();
	if (m_map.find(eIndex) != m_map.end())
	{
		m_map[eIndex] += iChange;
		if (m_map[eIndex]==0)
			m_map.erase(eIndex);
	}
	else
		m_map[eIndex] = iChange;
}
#endif
#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
int CvUnit::getTerrainDoubleHeal(TerrainTypes eIndex) const
{
	const std::map<TerrainTypes, int>& m_map = m_terrainDoubleHeal.get();
	std::map<TerrainTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second;
	else
		return 0;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isTerrainDoubleHeal(TerrainTypes eIndex) const
{
	const std::map<TerrainTypes, int>& m_map = m_terrainDoubleHeal.get();
	std::map<TerrainTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second > 0;
	else
		return false;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeTerrainDoubleHeal(TerrainTypes eIndex, int iChange)
{
	std::map<TerrainTypes, int>& m_map = m_terrainDoubleHeal.dirtyGet();
	if (m_map.find(eIndex) != m_map.end())
	{
		m_map[eIndex] += iChange;
		if (m_map[eIndex] == 0)
			m_map.erase(eIndex);
	}
	else
		m_map[eIndex] = iChange;
}


//	--------------------------------------------------------------------------------
int CvUnit::getFeatureDoubleHeal(FeatureTypes eIndex) const
{
	const std::map<FeatureTypes, int>& m_map = m_featureDoubleHeal.get();
	std::map<FeatureTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second;
	else
		return 0;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isFeatureDoubleHeal(FeatureTypes eIndex) const
{
	const std::map<FeatureTypes, int>& m_map = m_featureDoubleHeal.get();
	std::map<FeatureTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second > 0;
	else
		return false;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeFeatureDoubleHeal(FeatureTypes eIndex, int iChange)
{
	std::map<FeatureTypes, int>& m_map = m_featureDoubleHeal.dirtyGet();
	if (m_map.find(eIndex) != m_map.end())
	{
		m_map[eIndex] += iChange;
		if (m_map[eIndex] == 0)
			m_map.erase(eIndex);
	}
	else
		m_map[eIndex] = iChange;
}
#endif

//	--------------------------------------------------------------------------------
int CvUnit::getImpassableCount() const
{
	VALIDATE_OBJECT
	int iCount = 0;

	int iNumTerrains = GC.getNumTerrainInfos();
	for(int iI = 0; iI < iNumTerrains; iI++)
	{
		if(isTerrainImpassable((TerrainTypes) iI))
		{
			iCount++;
		}
	}

	int iNumFeatures = GC.getNumFeatureInfos();
	for(int iI = 0; iI < iNumFeatures; iI++)
	{
		if(isFeatureImpassable((FeatureTypes) iI))
		{
			iCount++;
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int CvUnit::getTerrainImpassableCount(TerrainTypes eIndex) const
{
	const std::map<TerrainTypes, int>& m_map = m_terrainImpassableCount.get();
	std::map<TerrainTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second;
	else
		return 0;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isTerrainImpassable(TerrainTypes eIndex) const
{
	const std::map<TerrainTypes, int>& m_map = m_terrainImpassableCount.get();
	std::map<TerrainTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second > 0;
	else
		return false;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeTerrainImpassableCount(TerrainTypes eIndex, int iChange)
{
	std::map<TerrainTypes, int>& m_map = m_terrainImpassableCount.dirtyGet();
	if (m_map.find(eIndex) != m_map.end())
	{
		m_map[eIndex] += iChange;
		if (m_map[eIndex] == 0)
			m_map.erase(eIndex);
	}
	else
		m_map[eIndex] = iChange;
}


//	--------------------------------------------------------------------------------
int CvUnit::getFeatureImpassableCount(FeatureTypes eIndex) const
{
	const std::map<FeatureTypes, int>& m_map = m_featureImpassableCount.get();
	std::map<FeatureTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second;
	else
		return 0;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isFeatureImpassable(FeatureTypes eIndex) const
{
	const std::map<FeatureTypes, int>& m_map = m_featureImpassableCount.get();
	std::map<FeatureTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second > 0;
	else
		return false;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeFeatureImpassableCount(FeatureTypes eIndex, int iChange)
{
	std::map<FeatureTypes, int>& m_map = m_featureImpassableCount.dirtyGet();
	if (m_map.find(eIndex) != m_map.end())
	{
		m_map[eIndex] += iChange;
		if (m_map[eIndex] == 0)
			m_map.erase(eIndex);
	}
	else
		m_map[eIndex] = iChange;
}

//	--------------------------------------------------------------------------------
int CvUnit::getExtraTerrainAttackPercent(TerrainTypes eIndex) const
{
	const std::map<TerrainTypes, int>& m_map = m_extraTerrainAttackPercent.get();
	std::map<TerrainTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second;
	else
		return 0;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraTerrainAttackPercent(TerrainTypes eIndex, int iChange)
{
	if(iChange != 0)
	{
		std::map<TerrainTypes, int>& m_map = m_extraTerrainAttackPercent.dirtyGet();
		if (m_map.find(eIndex) != m_map.end())
		{
			m_map[eIndex] += iChange;
			if (m_map[eIndex] == 0)
				m_map.erase(eIndex);
		}
		else
			m_map[eIndex] = iChange;

		setInfoBarDirty(true);
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::getExtraTerrainDefensePercent(TerrainTypes eIndex) const
{
	const std::map<TerrainTypes, int>& m_map = m_extraTerrainDefensePercent.get();
	std::map<TerrainTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second;
	else
		return 0;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraTerrainDefensePercent(TerrainTypes eIndex, int iChange)
{
	if(iChange != 0)
	{
		std::map<TerrainTypes, int>& m_map = m_extraTerrainDefensePercent.dirtyGet();
		if (m_map.find(eIndex) != m_map.end())
		{
			m_map[eIndex] += iChange;
			if (m_map[eIndex] == 0)
				m_map.erase(eIndex);
		}
		else
			m_map[eIndex] = iChange;

		setInfoBarDirty(true);
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::getExtraFeatureAttackPercent(FeatureTypes eIndex) const
{
	const std::map<FeatureTypes, int>& m_map = m_extraFeatureAttackPercent.get();
	std::map<FeatureTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second;
	else
		return 0;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraFeatureAttackPercent(FeatureTypes eIndex, int iChange)
{
	if(iChange != 0)
	{
		std::map<FeatureTypes, int>& m_map = m_extraFeatureAttackPercent.dirtyGet();
		if (m_map.find(eIndex) != m_map.end())
		{
			m_map[eIndex] += iChange;
			if (m_map[eIndex] == 0)
				m_map.erase(eIndex);
		}
		else
			m_map[eIndex] = iChange;

		setInfoBarDirty(true);
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::getExtraFeatureDefensePercent(FeatureTypes eIndex) const
{
	const std::map<FeatureTypes, int>& m_map = m_extraFeatureDefensePercent.get();
	std::map<FeatureTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second;
	else
		return 0;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraFeatureDefensePercent(FeatureTypes eIndex, int iChange)
{
	if(iChange != 0)
	{
		std::map<FeatureTypes, int>& m_map = m_extraFeatureDefensePercent.dirtyGet();
		if (m_map.find(eIndex) != m_map.end())
		{
			m_map[eIndex] += iChange;
			if (m_map[eIndex] == 0)
				m_map.erase(eIndex);
		}
		else
			m_map[eIndex] = iChange;

		setInfoBarDirty(true);
	}
}

#if defined(MOD_API_UNIFIED_YIELDS)
//	--------------------------------------------------------------------------------
int CvUnit::getYieldFromKills(YieldTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_yieldFromKills[eIndex];
}


//	--------------------------------------------------------------------------------
void CvUnit::changeYieldFromKills(YieldTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_yieldFromKills.setAt(eIndex, m_yieldFromKills[eIndex] + iChange);
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::getYieldFromBarbarianKills(YieldTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_yieldFromBarbarianKills[eIndex];
}


//	--------------------------------------------------------------------------------
void CvUnit::changeYieldFromBarbarianKills(YieldTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_yieldFromBarbarianKills.setAt(eIndex, m_yieldFromBarbarianKills[eIndex] + iChange);
	}
}
#endif

//	--------------------------------------------------------------------------------
int CvUnit::getExtraUnitCombatModifier(UnitCombatTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_extraUnitCombatModifier[eIndex];
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraUnitCombatModifier(UnitCombatTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_extraUnitCombatModifier.setAt(eIndex, m_extraUnitCombatModifier[eIndex] + iChange);
}


//	--------------------------------------------------------------------------------
int CvUnit::getUnitClassModifier(UnitClassTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_unitClassModifier[eIndex];
}


//	--------------------------------------------------------------------------------
void CvUnit::changeUnitClassModifier(UnitClassTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_unitClassModifier.setAt(eIndex, m_unitClassModifier[eIndex] + iChange);
}


//	--------------------------------------------------------------------------------
bool CvUnit::canAcquirePromotion(PromotionTypes ePromotion) const
{
	VALIDATE_OBJECT
	CvAssertMsg(ePromotion >= 0, "ePromotion is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePromotion < GC.getNumPromotionInfos(), "ePromotion is expected to be within maximum bounds (invalid Index)");

	CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
	if(pkPromotionInfo == NULL)
	{
		return false;
	}

	if(isHasPromotion(ePromotion))
	{
		return false;
	}

	// AND prereq
	if(pkPromotionInfo->GetPrereqPromotion() != NO_PROMOTION)
	{
		if(!isHasPromotion((PromotionTypes)(pkPromotionInfo->GetPrereqPromotion())))
		{
			return false;
		}
	}

	// OR prereqs
	bool bLacksOrPrereq = false;

	PromotionTypes ePromotion1 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion1();
	if(ePromotion1 != NO_PROMOTION)
	{
		if(!isHasPromotion(ePromotion1))
			bLacksOrPrereq = true;
	}

	// OR Promotion 2
	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion2 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion2();
		if(ePromotion2 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion2))
				bLacksOrPrereq = false;
		}
	}

	// OR Promotion 3
	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion3 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion3();
		if(ePromotion3 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion3))
				bLacksOrPrereq = false;
		}
	}

	// OR Promotion 4
	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion4 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion4();
		if(ePromotion4 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion4))
				bLacksOrPrereq = false;
		}
	}

	// OR Promotion 5
	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion5 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion5();
		if(ePromotion5 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion5))
				bLacksOrPrereq = false;
		}
	}

	// OR Promotion 6
	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion6 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion6();
		if(ePromotion6 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion6))
				bLacksOrPrereq = false;
		}
	}

	// OR Promotion 7
	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion7 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion7();
		if(ePromotion7 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion7))
				bLacksOrPrereq = false;
		}
	}

	// OR Promotion 8
	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion8 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion8();
		if(ePromotion8 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion8))
				bLacksOrPrereq = false;
		}
	}

	// OR Promotion 9
	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion9 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion9();
		if(ePromotion9 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion9))
				bLacksOrPrereq = false;
		}
	}

	if(bLacksOrPrereq)
	{
		return false;
	}

	if(pkPromotionInfo->GetTechPrereq() != NO_TECH)
	{
		if(!(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)(pkPromotionInfo->GetTechPrereq()))))
		{
			return false;
		}
	}

	if(!isPromotionValid(ePromotion))
	{
		return false;
	}
	
#if defined(MOD_EVENTS_UNIT_UPGRADES)
	if (MOD_EVENTS_UNIT_UPGRADES) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_CanHavePromotion, getOwner(), GetID(), ePromotion) == GAMEEVENTRETURN_FALSE) {
			return false;
		}

		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_UnitCanHavePromotion, getOwner(), GetID(), ePromotion) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	} else {
#endif
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem) 
	{
		CvLuaArgsHandle args;
		args->Push(((int)getOwner()));
		args->Push(GetID());
		args->Push(ePromotion);

		bool bResult = false;
		if (LuaSupport::CallTestAll(pkScriptSystem, "CanHavePromotion", args.get(), bResult))
		{
			if (bResult == false) 
			{
				return false;
			}
		}
	}
#if defined(MOD_EVENTS_UNIT_UPGRADES)
	}
#endif

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isPromotionValid(PromotionTypes ePromotion) const
{
	VALIDATE_OBJECT

	CvPromotionEntry* promotionInfo = GC.getPromotionInfo(ePromotion);
	if(promotionInfo == NULL)
	{
		return false;
	}

	if(!::isPromotionValid(ePromotion, getUnitType(), true))
		return false;

	// Insta-heal - must be damaged
	if(promotionInfo->IsInstaHeal())
	{
		if(getDamage() == 0)
			return false;
	}
#if defined(MOD_BALANCE_CORE)
	if(promotionInfo->IsBarbarianOnly())
	{
		if(!isBarbarian())
		{
			return false;
		}
	}
	if(promotionInfo->IsCityStateOnly())
	{
		if(!GET_PLAYER(getOwner()).isMinorCiv())
		{
			return false;
		}
	}
#endif

	// Can't acquire interception promotion if unit can't intercept!
	if(promotionInfo->GetInterceptionCombatModifier() != 0)
	{
		if(!canAirDefend())
			return false;
	}

	// Can't acquire Air Sweep promotion if unit can't air sweep!
	if(promotionInfo->GetAirSweepCombatModifier() != 0)
	{
		if(!IsAirSweepCapable())
			return false;
	}

	// Max Interception
	if(promotionInfo->GetInterceptChanceChange() > 0)
	{
		if(promotionInfo->GetInterceptChanceChange() + maxInterceptionProbability() > GC.getMAX_INTERCEPTION_PROBABILITY())
			return false;
	}

	// Max evasion
	if(promotionInfo->GetEvasionChange() > 0)
	{
		if(promotionInfo->GetEvasionChange() + evasionProbability() > GC.getMAX_EVASION_PROBABILITY())
			return false;
	}

	// Hovering units (e.g. Helis) cannot embark
	if(IsHoveringUnit() && promotionInfo->IsAllowsEmbarkation())
		return false;
		
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
	// Only hovering units can deep water embark
	if(!IsHoveringUnit() && promotionInfo->IsEmbarkedDeepWater())
		return false;
#endif

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canAcquirePromotionAny() const
{
	VALIDATE_OBJECT

	// Can't promote a unit that has attacked
	if(isOutOfAttacks())
		return false;

	int iI;

	for(iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		if(canAcquirePromotion((PromotionTypes)iI))
		{
			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isHasPromotion(PromotionTypes eIndex) const
{
	VALIDATE_OBJECT
	return m_Promotions.HasPromotion(eIndex);
}


//	--------------------------------------------------------------------------------
void CvUnit::setHasPromotion(PromotionTypes eIndex, bool bNewValue)
{
	VALIDATE_OBJECT
	int iChange;
	int iI;

	if(isHasPromotion(eIndex) != bNewValue)
	{
		CvPromotionEntry& thisPromotion = *GC.getPromotionInfo(eIndex);

#if defined(MOD_GLOBAL_CANNOT_EMBARK)
		if (bNewValue && MOD_GLOBAL_CANNOT_EMBARK && getUnitInfo().CannotEmbark()) {
			if (thisPromotion.IsAllowsEmbarkation() || thisPromotion.IsEmbarkedAllWater()) {
				return;
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
			} else if (MOD_PROMOTIONS_DEEP_WATER_EMBARKATION && thisPromotion.IsEmbarkedDeepWater()) {
				return;
#endif
			}
		}
#endif

#if defined(MOD_BUGFIX_MINOR)
		// Firaxis only use the CanMoveAllTerrain promotion in the Smokey Skies scenario,
		// which doesn't have the situation where the player discovers Optics ...
		if (bNewValue && canMoveAllTerrain()) {
			if (thisPromotion.IsAllowsEmbarkation() || thisPromotion.IsEmbarkedAllWater()) {
				return;
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
			} else if (MOD_PROMOTIONS_DEEP_WATER_EMBARKATION && thisPromotion.IsEmbarkedDeepWater()) {
				return;
#endif
			}
		}
#endif

		m_Promotions.SetPromotion(eIndex, bNewValue);
		iChange = ((isHasPromotion(eIndex)) ? 1 : -1);

		// Promotions will set Invisibility once but not change it later
		if(getInvisibleType() == NO_INVISIBLE && thisPromotion.GetInvisibleType() != NO_INVISIBLE)
		{
			setInvisibleType((InvisibleTypes) thisPromotion.GetInvisibleType());
		}
		if(getSeeInvisibleType() == NO_INVISIBLE && thisPromotion.GetSeeInvisibleType() != NO_INVISIBLE)
		{
			setSeeInvisibleType((InvisibleTypes) thisPromotion.GetSeeInvisibleType());
		}

		changeBlitzCount((thisPromotion.IsBlitz()) ? iChange : 0);
		changeAmphibCount((thisPromotion.IsAmphib()) ? iChange : 0);
		changeRiverCrossingNoPenaltyCount((thisPromotion.IsRiver()) ? iChange : 0);
		changeEnemyRouteCount((thisPromotion.IsEnemyRoute()) ? iChange : 0);
		changeRivalTerritoryCount((thisPromotion.IsRivalTerritory()) ? iChange : 0);
		changeMustSetUpToRangedAttackCount((thisPromotion.IsMustSetUpToRangedAttack()) ? iChange : 0);
		changeRangedSupportFireCount((thisPromotion.IsRangedSupportFire()) ? iChange : 0);
		changeAlwaysHealCount((thisPromotion.IsAlwaysHeal()) ? iChange : 0);
		changeHealOutsideFriendlyCount((thisPromotion.IsHealOutsideFriendly()) ? iChange : 0);
		changeHillsDoubleMoveCount((thisPromotion.IsHillsDoubleMove()) ? iChange : 0);
		changeIgnoreTerrainCostCount((thisPromotion.IsIgnoreTerrainCost()) ? iChange : 0);
#if defined(MOD_API_PLOT_BASED_DAMAGE)
		changeIgnoreTerrainDamageCount((thisPromotion.IsIgnoreTerrainDamage()) ? iChange : 0);
		changeIgnoreFeatureDamageCount((thisPromotion.IsIgnoreFeatureDamage()) ? iChange : 0);
		changeExtraTerrainDamageCount((thisPromotion.IsExtraTerrainDamage()) ? iChange : 0);
		changeExtraFeatureDamageCount((thisPromotion.IsExtraFeatureDamage()) ? iChange : 0);
#endif
#if defined(MOD_PROMOTIONS_IMPROVEMENT_BONUS)
		if (MOD_PROMOTIONS_IMPROVEMENT_BONUS) {
			if (thisPromotion.GetNearbyImprovementCombatBonus() > 0) {
				SetNearbyImprovementCombatBonus(thisPromotion.GetNearbyImprovementCombatBonus());
				SetNearbyImprovementBonusRange(thisPromotion.GetNearbyImprovementBonusRange());
				SetCombatBonusImprovement(thisPromotion.GetCombatBonusImprovement());
			}
		}
#endif
#if defined(MOD_PROMOTIONS_CROSS_MOUNTAINS)
		if (MOD_PROMOTIONS_CROSS_MOUNTAINS) {
			changeCanCrossMountainsCount((thisPromotion.CanCrossMountains()) ? iChange : 0);
		}
#endif
#if defined(MOD_PROMOTIONS_CROSS_OCEANS)
		if (MOD_PROMOTIONS_CROSS_OCEANS) {
			changeCanCrossOceansCount((thisPromotion.CanCrossOceans()) ? iChange : 0);
		}
#endif
#if defined(MOD_PROMOTIONS_CROSS_ICE)
		if (MOD_PROMOTIONS_CROSS_ICE) {
			changeCanCrossIceCount((thisPromotion.CanCrossIce()) ? iChange : 0);
		}
#endif
		ChangeRoughTerrainEndsTurnCount((thisPromotion.IsRoughTerrainEndsTurn()) ? iChange : 0);
		ChangeHoveringUnitCount((thisPromotion.IsHoveringUnit()) ? iChange : 0);
		changeFlatMovementCostCount((thisPromotion.IsFlatMovementCost()) ? iChange : 0);
		changeCanMoveImpassableCount((thisPromotion.IsCanMoveImpassable()) ? iChange : 0);
		changeOnlyDefensiveCount((thisPromotion.IsOnlyDefensive()) ? iChange : 0);
		changeNoDefensiveBonusCount((thisPromotion.IsNoDefensiveBonus()) ? iChange : 0);
		changeNoCaptureCount((thisPromotion.IsNoCapture()) ? iChange : 0);
		changeNukeImmuneCount((thisPromotion.IsNukeImmune()) ? iChange: 0);
		changeHiddenNationalityCount((thisPromotion.IsHiddenNationality()) ? iChange: 0);
		changeAlwaysHostileCount((thisPromotion.IsAlwaysHostile()) ? iChange: 0);
		changeNoRevealMapCount((thisPromotion.IsNoRevealMap()) ? iChange: 0);
		ChangeReconCount((thisPromotion.IsRecon()) ? iChange: 0);
		changeCanMoveAllTerrainCount((thisPromotion.CanMoveAllTerrain()) ? iChange: 0);
		changeCanMoveAfterAttackingCount((thisPromotion.IsCanMoveAfterAttacking()) ? iChange: 0);
		ChangeAirSweepCapableCount((thisPromotion.IsAirSweepCapable()) ? iChange: 0);
		ChangeEmbarkAbilityCount((thisPromotion.IsAllowsEmbarkation()) ? iChange: 0);
		ChangeRangeAttackIgnoreLOSCount((thisPromotion.IsRangeAttackIgnoreLOS()) ? iChange: 0);
		ChangeHealIfDefeatExcludeBarbariansCount((thisPromotion.IsHealIfDefeatExcludeBarbarians()) ? iChange: 0);
		changeHealOnPillageCount((thisPromotion.IsHealOnPillage()) ? iChange : 0);
		changeFreePillageMoveCount((thisPromotion.IsFreePillageMoves()) ? iChange: 0);
		ChangeEmbarkAllWaterCount((thisPromotion.IsEmbarkedAllWater()) ? iChange: 0);
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
		ChangeEmbarkDeepWaterCount((thisPromotion.IsEmbarkedDeepWater()) ? iChange: 0);
#endif
		ChangeCityAttackOnlyCount((thisPromotion.IsCityAttackOnly()) ? iChange: 0);
		ChangeCaptureDefeatedEnemyCount((thisPromotion.IsCaptureDefeatedEnemy()) ? iChange: 0);
#if defined(MOD_BALANCE_CORE)
		ChangeCannotBeCapturedCount((thisPromotion.CannotBeCaptured()) ? iChange: 0);
		ChangeForcedDamageValue((thisPromotion.ForcedDamageValue()) * iChange);
		ChangeChangeDamageValue((thisPromotion.ChangeDamageValue()) * iChange);


		ChangePromotionDuration(eIndex, (thisPromotion.PromotionDuration()) * iChange);
		if(getPromotionDuration(eIndex) > 0)
		{
			SetTurnPromotionGained(eIndex, GC.getGame().getGameTurn());
		}
#endif
		ChangeCanHeavyChargeCount((thisPromotion.IsCanHeavyCharge()) ? iChange : 0);

		ChangeEmbarkExtraVisibility((thisPromotion.GetEmbarkExtraVisibility()) * iChange);
		ChangeEmbarkDefensiveModifier((thisPromotion.GetEmbarkDefenseModifier()) * iChange);
		ChangeCapitalDefenseModifier((thisPromotion.GetCapitalDefenseModifier()) * iChange);
		ChangeCapitalDefenseFalloff((thisPromotion.GetCapitalDefenseFalloff()) * iChange);
		ChangeCityAttackPlunderModifier((thisPromotion.GetCityAttackPlunderModifier()) *  iChange);
		ChangeReligiousStrengthLossRivalTerritory((thisPromotion.GetReligiousStrengthLossRivalTerritory()) *  iChange);
		ChangeTradeMissionInfluenceModifier((thisPromotion.GetTradeMissionInfluenceModifier()) * iChange);
		ChangeTradeMissionGoldModifier((thisPromotion.GetTradeMissionGoldModifier()) * iChange);
		changeDropRange(thisPromotion.GetDropRange() * iChange);
		changeExtraVisibilityRange(thisPromotion.GetVisibilityChange() * iChange);
#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
		changeExtraReconRange(thisPromotion.GetReconChange() * iChange);
#endif
		changeExtraMoves(thisPromotion.GetMovesChange() * iChange);
		changeExtraMoveDiscount(thisPromotion.GetMoveDiscountChange() * iChange);
		changeExtraNavalMoves(thisPromotion.GetExtraNavalMoves() * iChange);
		changeHPHealedIfDefeatEnemy(thisPromotion.GetHPHealedIfDefeatEnemy() * iChange);
		ChangeGoldenAgeValueFromKills(thisPromotion.GetGoldenAgeValueFromKills() * iChange);
		changeExtraWithdrawal(thisPromotion.GetExtraWithdrawal() * iChange);
		changeExtraRange(thisPromotion.GetRangeChange() * iChange);
		ChangeRangedAttackModifier(thisPromotion.GetRangedAttackModifier() * iChange);
		ChangeInterceptionCombatModifier(thisPromotion.GetInterceptionCombatModifier() * iChange);
		ChangeInterceptionDefenseDamageModifier(thisPromotion.GetInterceptionDefenseDamageModifier() * iChange);
		ChangeAirSweepCombatModifier(thisPromotion.GetAirSweepCombatModifier() * iChange);
		changeExtraIntercept(thisPromotion.GetInterceptChanceChange() * iChange);
		changeExtraEvasion(thisPromotion.GetEvasionChange() * iChange);
		changeExtraEnemyHeal(thisPromotion.GetEnemyHealChange() * iChange);
		changeExtraNeutralHeal(thisPromotion.GetNeutralHealChange() * iChange);
		changeExtraFriendlyHeal(thisPromotion.GetFriendlyHealChange() * iChange);
		changeSameTileHeal(thisPromotion.GetSameTileHealChange() * iChange);
		changeAdjacentTileHeal(thisPromotion.GetAdjacentTileHealChange() * iChange);
		changeEnemyDamageChance(thisPromotion.GetEnemyDamageChance() * iChange);
		changeNeutralDamageChance(thisPromotion.GetNeutralDamageChance() * iChange);
		changeEnemyDamage(thisPromotion.GetEnemyDamage() * iChange);
		changeNeutralDamage(thisPromotion.GetNeutralDamage() * iChange);
		changeNearbyEnemyCombatMod(thisPromotion.GetNearbyEnemyCombatMod() * iChange);
		changeNearbyEnemyCombatRange(thisPromotion.GetNearbyEnemyCombatRange() * iChange);
		ChangeAdjacentModifier(thisPromotion.GetAdjacentMod() * iChange);
		changeAttackModifier(thisPromotion.GetAttackMod() * iChange);
		changeDefenseModifier(thisPromotion.GetDefenseMod() * iChange);
		changeExtraCombatPercent(thisPromotion.GetCombatPercent() * iChange);
		changeExtraCityAttackPercent(thisPromotion.GetCityAttackPercent() * iChange);
		changeExtraCityDefensePercent(thisPromotion.GetCityDefensePercent() * iChange);
		changeExtraRangedDefenseModifier(thisPromotion.GetRangedDefenseMod() * iChange);
		changeExtraHillsAttackPercent(thisPromotion.GetHillsAttackPercent() * iChange);
		changeExtraHillsDefensePercent(thisPromotion.GetHillsDefensePercent() * iChange);
		changeExtraOpenAttackPercent(thisPromotion.GetOpenAttackPercent() * iChange);
		changeExtraOpenRangedAttackMod(thisPromotion.GetOpenRangedAttackMod() * iChange);
		changeExtraRoughAttackPercent(thisPromotion.GetRoughAttackPercent() * iChange);
		changeExtraRoughRangedAttackMod(thisPromotion.GetRoughRangedAttackMod() * iChange);
		changeExtraAttackFortifiedMod(thisPromotion.GetAttackFortifiedMod() * iChange);
		changeExtraAttackWoundedMod(thisPromotion.GetAttackWoundedMod() * iChange);
		ChangeFlankAttackModifier(thisPromotion.GetFlankAttackModifier() * iChange);
		changeExtraOpenDefensePercent(thisPromotion.GetOpenDefensePercent() * iChange);
		changeExtraRoughDefensePercent(thisPromotion.GetRoughDefensePercent() * iChange);
		changeExtraAttacks(thisPromotion.GetExtraAttacks() * iChange);
		ChangeNumInterceptions(thisPromotion.GetNumInterceptionChange() * iChange);

		ChangeGreatGeneralCount(thisPromotion.IsGreatGeneral() ? iChange: 0);
		ChangeGreatAdmiralCount(thisPromotion.IsGreatAdmiral() ? iChange: 0);

		changeGreatGeneralModifier(thisPromotion.GetGreatGeneralModifier() * iChange);
		ChangeGreatGeneralReceivesMovementCount(thisPromotion.IsGreatGeneralReceivesMovement() ? iChange: 0);
		ChangeGreatGeneralCombatModifier(thisPromotion.GetGreatGeneralCombatModifier() * iChange);

		ChangeIgnoreGreatGeneralBenefitCount(thisPromotion.IsIgnoreGreatGeneralBenefit() ? iChange: 0);
		ChangeIgnoreZOCCount(thisPromotion.IsIgnoreZOC() ? iChange: 0);

		ChangeSapperCount((thisPromotion.IsSapper() ? iChange: 0));

		changeFriendlyLandsModifier(thisPromotion.GetFriendlyLandsModifier() * iChange);
		changeFriendlyLandsAttackModifier(thisPromotion.GetFriendlyLandsAttackModifier() * iChange);
		changeOutsideFriendlyLandsModifier(thisPromotion.GetOutsideFriendlyLandsModifier() * iChange);
		changeUpgradeDiscount(thisPromotion.GetUpgradeDiscount() * iChange);
		changeExperiencePercent(thisPromotion.GetExperiencePercent() * iChange);
		changeCargoSpace(thisPromotion.GetCargoChange() * iChange);

#if defined(MOD_PROMOTIONS_UNIT_NAMING)
		if (MOD_PROMOTIONS_UNIT_NAMING) {
			if (thisPromotion.IsUnitNaming(getUnitType())) {
				thisPromotion.GetUnitName(getUnitType(), m_strUnitName);
			}
		}
#endif

		for(iI = 0; iI < GC.getNumTerrainInfos(); iI++)
		{
			changeExtraTerrainAttackPercent(((TerrainTypes)iI), (thisPromotion.GetTerrainAttackPercent(iI) * iChange));
			changeExtraTerrainDefensePercent(((TerrainTypes)iI), (thisPromotion.GetTerrainDefensePercent(iI) * iChange));
			changeTerrainDoubleMoveCount(((TerrainTypes)iI), ((thisPromotion.GetTerrainDoubleMove(iI)) ? iChange : 0));
#if defined(MOD_PROMOTIONS_HALF_MOVE)
			changeTerrainHalfMoveCount(((TerrainTypes)iI), ((thisPromotion.GetTerrainHalfMove(iI)) ? iChange : 0));
#endif
#if defined(MOD_BALANCE_CORE)
			changeTerrainDoubleHeal(((TerrainTypes)iI), ((thisPromotion.GetTerrainDoubleHeal(iI)) ? iChange : 0));		
#endif
			changeTerrainImpassableCount(((TerrainTypes)iI), ((thisPromotion.GetTerrainImpassable(iI)) ? iChange : 0));
		}

		for(iI = 0; iI < GC.getNumFeatureInfos(); iI++)
		{
			changeExtraFeatureAttackPercent(((FeatureTypes)iI), (thisPromotion.GetFeatureAttackPercent(iI) * iChange));
			changeExtraFeatureDefensePercent(((FeatureTypes)iI), (thisPromotion.GetFeatureDefensePercent(iI) * iChange));
			changeFeatureDoubleMoveCount(((FeatureTypes)iI), ((thisPromotion.GetFeatureDoubleMove(iI)) ? iChange : 0));
#if defined(MOD_PROMOTIONS_HALF_MOVE)
			changeFeatureHalfMoveCount(((FeatureTypes)iI), ((thisPromotion.GetFeatureHalfMove(iI)) ? iChange : 0));
#endif
#if defined(MOD_BALANCE_CORE)
			changeFeatureDoubleHeal(((FeatureTypes)iI), ((thisPromotion.GetFeatureDoubleHeal(iI)) ? iChange : 0));		
#endif
			changeFeatureImpassableCount(((FeatureTypes)iI), ((thisPromotion.GetFeatureImpassable(iI)) ? iChange : 0));
		}

#if defined(MOD_API_UNIFIED_YIELDS)
		for(iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			changeYieldFromKills(((YieldTypes)iI), (thisPromotion.GetYieldFromKills(iI) * iChange));
			changeYieldFromBarbarianKills(((YieldTypes)iI), (thisPromotion.GetYieldFromBarbarianKills(iI) * iChange));
		}
#endif

		for(iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
		{
			changeExtraUnitCombatModifier(((UnitCombatTypes)iI), (thisPromotion.GetUnitCombatModifierPercent(iI) * iChange));
		}

		for(iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo((UnitClassTypes)iI);
			if(!pkUnitClassInfo)
			{
				continue;
			}

			changeUnitClassModifier(((UnitClassTypes)iI), (thisPromotion.GetUnitClassModifierPercent(iI) * iChange));
		}

		for(iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
		{
			changeExtraDomainModifier(((DomainTypes)iI), (thisPromotion.GetDomainModifierPercent(iI) * iChange));
		}

		if(IsSelected())
		{
			DLLUI->setDirty(SelectionButtons_DIRTY_BIT, true);
			DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
			if (thisPromotion.GetEmbarkExtraVisibility() || thisPromotion.IsNoRevealMap() || thisPromotion.GetVisibilityChange())
				GC.getMap().updateDeferredFog();
		}

#if !defined(NO_ACHIEVEMENTS)
		PromotionTypes eBuffaloChest =(PromotionTypes) GC.getInfoTypeForString("PROMOTION_BUFFALO_CHEST", true /*bHideAssert*/);
		PromotionTypes eBuffaloLoins =(PromotionTypes) GC.getInfoTypeForString("PROMOTION_BUFFALO_LOINS", true /*bHideAssert*/);

		const PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
		if(getOwner() == eActivePlayer && ((eIndex == eBuffaloChest && isHasPromotion(eBuffaloLoins)) || (eIndex == eBuffaloLoins && isHasPromotion(eBuffaloChest))))
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_XP2_27);
		}
#endif
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getSubUnitCount() const
{
	VALIDATE_OBJECT
	return getUnitInfo().GetGroupSize();
}


//	--------------------------------------------------------------------------------
int CvUnit::getSubUnitsAlive() const
{
	VALIDATE_OBJECT
	return getSubUnitsAlive(getDamage());
}


//	--------------------------------------------------------------------------------
int CvUnit::getSubUnitsAlive(int iDamage) const
{
	VALIDATE_OBJECT
	if(iDamage >= GetMaxHitPoints())
	{
		return 0;
	}
	else
	{
		return std::max(1, (((getUnitInfo().GetGroupSize() * (GetMaxHitPoints() - iDamage)) + (GetMaxHitPoints() / ((getUnitInfo().GetGroupSize() * 2) + 1))) / GetMaxHitPoints()));
	}
}

//	--------------------------------------------------------------------------------
// returns true if unit can initiate a war action with plot (possibly by declaring war)
bool CvUnit::potentialWarAction(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	TeamTypes ePlotTeam = pPlot->getTeam();

	if(ePlotTeam == NO_TEAM)
	{
		return false;
	}

	if(isEnemy(ePlotTeam, pPlot))
	{
		return true;
	}

	if(IsDeclareWar())
	{
		return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnit::AreUnitsOfSameType(const CvUnit& pUnit2, const bool bPretendEmbarked) const
{
	VALIDATE_OBJECT

	bool bUnit1isEmbarked = isEmbarked();
	bool bUnit2isEmbarked = pUnit2.isEmbarked() || bPretendEmbarked;
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
	if(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
	{
		if(IsCivilianUnit())
		{
			bUnit1isEmbarked = false;
		}
		if(pUnit2.IsCivilianUnit())
		{
			bUnit2isEmbarked = false;
		}
	}
#endif

	// 2 embarked units are considered the same type, regardless of circumstances
	if(bUnit1isEmbarked && bUnit2isEmbarked)
	{
		return true;
	}

	return CvGameQueries::AreUnitsSameType(getUnitType(), pUnit2.getUnitType());
}

//	--------------------------------------------------------------------------------
bool CvUnit::CanSwapWithUnitHere(CvPlot& swapPlot) const
{
	VALIDATE_OBJECT
	bool bSwapPossible = false;

	if(getDomainType() == DOMAIN_LAND || getDomainType() == DOMAIN_SEA)
	{
		if(canEnterTerrain(swapPlot))
		{
			// Can I get there this turn?
			CvUnit* pUnit = (CvUnit*)this;

			SPathFinderUserData data(pUnit,CvUnit::MOVEFLAG_IGNORE_DANGER | CvUnit::MOVEFLAG_IGNORE_STACKING,1);
			if(GC.GetPathFinder().DoesPathExist(plot(), &swapPlot, data))
			{
				CvPlot* pEndTurnPlot = GC.GetPathFinder().GetPathEndTurnPlot();
				if(pEndTurnPlot == &swapPlot)
				{
#if defined(MOD_GLOBAL_STACKING_RULES)
					if(swapPlot.getNumFriendlyUnitsOfType(this) >= swapPlot.getUnitLimit())
#else
					if(swapPlot.getNumFriendlyUnitsOfType(this) >= GC.getPLOT_UNIT_LIMIT())
#endif
					{
						const IDInfo* pUnitNode;
						CvUnit* pLoopUnit;
						pUnitNode = swapPlot.headUnitNode();
						while(pUnitNode != NULL)
						{
							pLoopUnit = (CvUnit*)::getUnit(*pUnitNode);
							pUnitNode = swapPlot.nextUnitNode(pUnitNode);

							// A unit can't swap with itself (slewis)
							if (!pLoopUnit || pLoopUnit == this)
							{
								continue;
							}

							// Make sure units belong to the same player
							if(pLoopUnit && pLoopUnit->getOwner() == getOwner())
							{
								if(AreUnitsOfSameType(*pLoopUnit))
								{
									CvPlot* here = plot();
									if(here && pLoopUnit->canEnterTerrain(*here))
									{
										// Can the unit I am swapping with get to me this turn?
										SPathFinderUserData data(pLoopUnit,CvUnit::MOVEFLAG_IGNORE_DANGER | CvUnit::MOVEFLAG_IGNORE_STACKING,1);

										if(pLoopUnit->canMove() && GC.GetPathFinder().DoesPathExist(pLoopUnit->plot(), pUnit->plot(), data))
										{
											CvPlot* pPathEndTurnPlot = GC.GetPathFinder().GetPathEndTurnPlot();
											if(pPathEndTurnPlot == plot())
											{
												bSwapPossible = true;
												break;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return bSwapPossible;
}

//	--------------------------------------------------------------------------------
void CvUnit::read(FDataStream& kStream)
{
	VALIDATE_OBJECT
	// Init data before load
	reset();

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	// all FAutoVariables in the m_syncArchive will be read
	// automagically, no need to explicitly load them here
	kStream >> m_syncArchive;

	// anything not in m_syncArchive needs to be explicitly
	// read

	// The 'automagic' sync archive saves out the unit type index, which is bad since that can change.
	// Read in the hash and update the value.
	{
		uint idUnitType;
		kStream >> idUnitType;
		if (idUnitType != 0)
		{
			UnitTypes eUnitIndex = (UnitTypes)GC.getInfoTypeForHash(idUnitType);
			if (NO_UNIT != eUnitIndex)
				m_eUnitType = eUnitIndex;
		}
	}

	kStream >> m_iLastMoveTurn;
	m_Promotions.Read(kStream);
	m_pUnitInfo = (NO_UNIT != m_eUnitType) ? GC.getUnitInfo(m_eUnitType) : NULL;
	kStream >> m_combatUnit.eOwner;
	kStream >> m_combatUnit.iID;
	kStream >> m_transportUnit.eOwner;
	kStream >> m_transportUnit.iID;
	kStream >> m_missionAIUnit.eOwner;
	kStream >> m_missionAIUnit.iID;
	kStream >> m_extraDomainModifiers;
	kStream >> m_iEverSelectedCount;
	kStream >> m_iMapLayer;
	if (uiVersion >= 9)
	{
		kStream >> m_iNumGoodyHutsPopped;
	}
	else
	{
		m_iNumGoodyHutsPopped = 0;
	}

	kStream >> m_bIgnoreDangerWakeup;

	kStream >> m_iEmbarkedAllWaterCount;
	kStream >> m_iEmbarkExtraVisibility;

#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
	MOD_SERIALIZE_READ(23, kStream, m_iEmbarkedDeepWaterCount, 0);
#endif
	kStream >> m_iEmbarkDefensiveModifier;

#if defined(MOD_API_EXTENSIONS)
	MOD_SERIALIZE_READ(58, kStream, m_iBaseRangedCombat, ((NO_UNIT != m_eUnitType) ? (m_pUnitInfo ? m_pUnitInfo->GetRangedCombat() : 0) : 0));
#endif

	kStream >> m_iCapitalDefenseModifier;
	kStream >> m_iCapitalDefenseFalloff;

	kStream >> m_iCityAttackPlunderModifier;

	kStream >> m_iReligiousStrengthLossRivalTerritory;

	kStream >> m_iTradeMissionInfluenceModifier;
	kStream >> m_iTradeMissionGoldModifier;

	kStream >> m_iEnemyDamageChance;
	kStream >> m_iNeutralDamageChance;

	kStream >> m_iEnemyDamage;
	kStream >> m_iNeutralDamage;

	kStream >> m_iNearbyEnemyCombatMod;
	kStream >> m_iNearbyEnemyCombatRange;

	kStream >> m_iHealOnPillageCount;
	kStream >> m_iFlankAttackModifier;

	if (uiVersion >= 3)
	{
		kStream >> m_iGoldenAgeValueFromKills;
	}
	else
	{
		m_iGoldenAgeValueFromKills = 0;
	}

	kStream >> m_iGreatGeneralReceivesMovementCount;
	kStream >> m_iGreatGeneralCombatModifier;
	kStream >> m_iIgnoreGreatGeneralBenefit;

	if (uiVersion >= 8)
	{
		kStream >> m_iIgnoreZOC;
	}
	else
	{
		m_iIgnoreZOC = 0;
	}

	if (uiVersion >= 2)
	{
		kStream >> m_iSapperCount;
	}
	else
	{
		m_iSapperCount = 0;
	}

	kStream >> m_iCanHeavyCharge;

	if (uiVersion >= 5)
	{
		kStream >> m_iNumExoticGoods;
	}
	else
	{
		m_iNumExoticGoods = 0;
	}

	kStream >> m_iCityAttackOnlyCount;

	kStream >> m_iCaptureDefeatedEnemyCount;

#if defined(MOD_BALANCE_CORE)
	MOD_SERIALIZE_READ(66, kStream, m_iCannotBeCapturedCount, 0);
	MOD_SERIALIZE_READ(66, kStream, m_iForcedDamage, 0);
	MOD_SERIALIZE_READ(66, kStream, m_iChangeDamage, 0);
	MOD_SERIALIZE_READ(66, kStream, m_iScienceBlastStrength, 0);
	MOD_SERIALIZE_READ(66, kStream, m_iCultureBlastStrength, 0);
#endif

	kStream >> m_iGreatAdmiralCount;

#if defined(MOD_PROMOTIONS_UNIT_NAMING)
	MOD_SERIALIZE_READ(46, kStream, m_strUnitName, getNameKey());
#endif
	kStream >> m_strName;

	kStream >> m_iScenarioData;

	kStream >> *m_pReligion;

	m_eGreatWork = NO_GREAT_WORK;
	if(uiVersion > 3)
	{
		if(uiVersion > 5)
		{
			uint uiGreatWorkType = 0;
			kStream >> uiGreatWorkType;
			
			if (uiGreatWorkType != 0)
			{
				m_eGreatWork = (GreatWorkType)GC.getInfoTypeForHash(uiGreatWorkType);
			}
		}
		else
		{
			CvString strGreatWorkType;
			kStream >> strGreatWorkType;
			m_eGreatWork = static_cast<GreatWorkType>(GC.getInfoTypeForString(strGreatWorkType.c_str()));
		}
	}

	if (uiVersion >= 7)
	{
		kStream >> m_iTourismBlastStrength;
	}
	else
	{
		m_iTourismBlastStrength = 0;
	}

	//  Read mission queue
	UINT uSize;
	kStream >> uSize;
	for(UINT uIdx = 0; uIdx < uSize; ++uIdx)
	{
		MissionQueueNode Node;

		kStream >> Node.eMissionType;
		kStream >> Node.iData1;
		kStream >> Node.iData2;
		kStream >> Node.iFlags;
		kStream >> Node.iPushTurn;

		m_missionQueue.insertAtEnd(&Node);
	}

	// Re-attach to the map layer.
	if (m_iMapLayer != DEFAULT_UNIT_MAP_LAYER)
	{
		GC.getMap().plotManager().AddUnit(GetIDInfo(), m_iX, m_iY, m_iMapLayer);
	}
}


//	--------------------------------------------------------------------------------
void CvUnit::write(FDataStream& kStream) const
{
	VALIDATE_OBJECT

	// Current version number
	uint uiVersion = 9;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	kStream << m_syncArchive;

	// Write out a hash for the unit type, the sync archive saved the index, which is not a good thing to do.
	if (m_eUnitType != NO_UNIT && m_pUnitInfo)
		kStream << FString::Hash(m_pUnitInfo->GetType());
	else
		kStream << (uint)0;

	kStream << m_iLastMoveTurn;
	m_Promotions.Write(kStream);
	kStream << m_combatUnit.eOwner;
	kStream << m_combatUnit.iID;
	kStream << m_transportUnit.eOwner;
	kStream << m_transportUnit.iID;
	kStream << m_missionAIUnit.eOwner;
	kStream << m_missionAIUnit.iID;
	kStream << m_extraDomainModifiers;
	kStream << m_iEverSelectedCount;
	kStream << m_iMapLayer;
	kStream << m_iNumGoodyHutsPopped;

	// slewis - move to autovariable when saves are broken
	kStream << m_bIgnoreDangerWakeup;
	kStream << m_iEmbarkedAllWaterCount;
	kStream << m_iEmbarkExtraVisibility;
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
	MOD_SERIALIZE_WRITE(kStream, m_iEmbarkedDeepWaterCount);
#endif
	kStream << m_iEmbarkDefensiveModifier;
#if defined(MOD_API_EXTENSIONS)
	MOD_SERIALIZE_WRITE(kStream, m_iBaseRangedCombat);
#endif
	kStream << m_iCapitalDefenseModifier;
	kStream << m_iCapitalDefenseFalloff;
	kStream << m_iCityAttackPlunderModifier;
	kStream << m_iReligiousStrengthLossRivalTerritory;
	kStream << m_iTradeMissionInfluenceModifier;
	kStream << m_iTradeMissionGoldModifier;
	kStream << m_iEnemyDamageChance;
	kStream << m_iNeutralDamageChance;
	kStream << m_iEnemyDamage;
	kStream << m_iNeutralDamage;
	kStream << m_iNearbyEnemyCombatMod;
	kStream << m_iNearbyEnemyCombatRange;

	kStream << m_iHealOnPillageCount;
	kStream << m_iFlankAttackModifier;
	kStream << m_iGoldenAgeValueFromKills;

	kStream << m_iGreatGeneralReceivesMovementCount;
	kStream << m_iGreatGeneralCombatModifier;
	kStream << m_iIgnoreGreatGeneralBenefit;
	kStream << m_iIgnoreZOC;
	kStream << m_iSapperCount;
	kStream << m_iCanHeavyCharge;
	kStream << m_iNumExoticGoods;
	kStream << m_iCityAttackOnlyCount;
	kStream << m_iCaptureDefeatedEnemyCount;
#if defined(MOD_BALANCE_CORE)
	MOD_SERIALIZE_WRITE(kStream, m_iCannotBeCapturedCount);
	MOD_SERIALIZE_WRITE(kStream, m_iForcedDamage);
	MOD_SERIALIZE_WRITE(kStream, m_iChangeDamage);
	MOD_SERIALIZE_WRITE(kStream, m_iScienceBlastStrength);
	MOD_SERIALIZE_WRITE(kStream, m_iCultureBlastStrength);
#endif

	kStream << m_iGreatAdmiralCount;

#if defined(MOD_PROMOTIONS_UNIT_NAMING)
	MOD_SERIALIZE_WRITE(kStream, m_strUnitName);
#endif
	kStream << m_strName;

	kStream << m_iScenarioData;
	kStream << *m_pReligion;

	if (m_eGreatWork != NO_GREAT_WORK)
	{
		Database::Connection* db = GC.GetGameDatabase();
		Database::Results kQuery;
		if(db && db->Execute(kQuery, "SELECT Type from GreatWorks where ID = ? LIMIT 1"))
		{
			kQuery.Bind(1, m_eGreatWork);

			if(kQuery.Step())
			{
				kStream << FString::Hash(kQuery.GetText(0));
			}
		}
	}
	else
	{
		kStream << (uint)0;
	}

	kStream << m_iTourismBlastStrength;

	//  Write mission list
	kStream << m_missionQueue.getLength();
	for(int uIdx = 0; uIdx < m_missionQueue.getLength(); ++uIdx)
	{
		MissionQueueNode* pNode = m_missionQueue.getAt(uIdx);

		kStream << pNode->eMissionType;
		kStream << pNode->iData1;
		kStream << pNode->iData2;
		kStream << pNode->iFlags;
		kStream << pNode->iPushTurn;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::canRangeStrike() const
{
	VALIDATE_OBJECT
	if(isEmbarked())
	{
		return false;
	}

	if(GetRange() <= 0)
	{
		return false;
	}

	if(GetBaseRangedCombatStrength() <= 0)
	{
		return false;
	}

	if(isMustSetUpToRangedAttack())
	{
		if(!isSetUpForRangedAttack())
		{
			return false;
		}
	}

	if(isOutOfAttacks())
	{
		return false;
	}

#if defined(MOD_BUGFIX_CITY_STACKING)
	// Can't attack out of Cities if there are more units of the same domain type than the stacking limit permits
#if defined(MOD_GLOBAL_STACKING_RULES)
	if(MOD_BUGFIX_CITY_STACKING && plot()->isCity() && plot()->getNumFriendlyUnitsOfType(this, true) > plot()->getUnitLimit())
#else
	if(MOD_BUGFIX_CITY_STACKING && plot()->isCity() && plot()->getNumFriendlyUnitsOfType(this, true) > GC.getPLOT_UNIT_LIMIT())
#endif
	{
		return false;
	}
#endif

	return true;
}

#if defined(MOD_AI_SMART_RANGED_UNITS)

bool CvUnit::canEverRangeStrikeAt(int iX, int iY, const CvPlot* pSourcePlot) const
{
	if (!pSourcePlot)
		pSourcePlot = plot();
#else

bool CvUnit::canEverRangeStrikeAt(int iX, int iY) const
{
	VALIDATE_OBJECT
	const CvPlot* pSourcePlot = plot();
#endif
	const CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);

	// Plot null?
	if(NULL == pTargetPlot)
	{
		return false;
	}

	// Plot not visible?
	if(!pTargetPlot->isVisible(getTeam()))
	{
		return false;
	}

#if defined(MOD_BALANCE_CORE)
	bool bWouldNeedEmbark = pSourcePlot && pSourcePlot->needsEmbarkation(this) && CanEverEmbark();
	bool bIsEmbarkedAttackingLand = pTargetPlot && !pTargetPlot->needsEmbarkation(this) && bWouldNeedEmbark;
	if (bIsEmbarkedAttackingLand)
		return false;
#endif


	// In Range?
	if(plotDistance(pSourcePlot->getX(), pSourcePlot->getY(), pTargetPlot->getX(), pTargetPlot->getY()) > GetRange())
	{
		return false;
	}

	// Can only bombard in domain? (used for Subs' torpedo attack)
	if(getUnitInfo().IsRangeAttackOnlyInDomain())
	{
		if( pTargetPlot->isWater() )
		{
			// preventing submarines from shooting into other water bodies (lakes)
			if (pSourcePlot->getArea() != pTargetPlot->getArea())
			{
				return false;
			}
		}
		else //land target
		{

#if defined(MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED)
			//Subs should be able to attack cities (they're on the coast, they've got ports, etc.). This will help the AI.
			if(MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED)
			{
				if(!pTargetPlot->isCoastalLand() || !pTargetPlot->isCity() || pTargetPlot->getPlotCity()->waterArea()!=pSourcePlot->area() )
				{
					return false;
				}
			}
			else
			{
#endif
			return false;
#if defined(MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED)
			}
#endif
		}
	}

	// Ignores LoS or can see the plot directly?
	if(!IsRangeAttackIgnoreLOS() && getDomainType() != DOMAIN_AIR)
	{
		if(!pSourcePlot->canSeePlot(pTargetPlot, getTeam(), GetRange(), getFacingDirection(true)))
		{
			return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canRangeStrikeAt(int iX, int iY, bool bNeedWar, bool bNoncombatAllowed) const
{
	VALIDATE_OBJECT

	if(!canRangeStrike())
	{
		return false;
	}

	if(!canEverRangeStrikeAt(iX, iY))
	{
		return false;
	}

	CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);

	// If it's NOT a city, see if there are any units to aim for
	if(!pTargetPlot->isCity())
	{
		if(bNeedWar)
		{
			const CvUnit* pDefender = airStrikeTarget(*pTargetPlot, bNoncombatAllowed);
			if(NULL == pDefender)
			{
				return false;
			}

#if defined(MOD_GLOBAL_SUBS_UNDER_ICE_IMMUNITY)
				// if the defender is a sub
				if (MOD_GLOBAL_SUBS_UNDER_ICE_IMMUNITY && pDefender->getInvisibleType() == 0) {
					// if the plot is ice, unless the attacker is also a sub, return false
					if (pTargetPlot->getFeatureType() == FEATURE_ICE && this->getInvisibleType() != 0) {
						return false;
					}

					// if the attacker is an archer type unit, return false
					if (this->getUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_ARCHER", true)) {
						return false;
					}
				}
#endif
		}
		// We don't need to be at war (yet) with a Unit here, so let's try to find one
		else
		{
			const IDInfo* pUnitNode = pTargetPlot->headUnitNode();
			const CvUnit* pLoopUnit;
			bool bFoundUnit = false;

			CvTeam& myTeam = GET_TEAM(getTeam());

			while(pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(*pUnitNode);
				pUnitNode = pTargetPlot->nextUnitNode(pUnitNode);

				if (!pLoopUnit)
					continue;

#if defined(MOD_GLOBAL_SUBS_UNDER_ICE_IMMUNITY)
				// if the defender is a sub
				if (MOD_GLOBAL_SUBS_UNDER_ICE_IMMUNITY && pLoopUnit->getInvisibleType() == 0) {
					// if the plot is ice, unless the attacker is also a sub, ignore them
					if (pTargetPlot->getFeatureType() == FEATURE_ICE && this->getInvisibleType() != 0) {
						continue;
					}

					// if the attacker is an archer type unit, ignore them
					if (this->getUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_ARCHER", true)) {
						continue;
					}
				}
#endif

				if(!pLoopUnit) continue;

				TeamTypes loopTeam = pLoopUnit->getTeam();

				// Make sure it's a valid Team
#if defined(MOD_EVENTS_WAR_AND_PEACE)
				if(myTeam.isAtWar(loopTeam) || myTeam.canDeclareWar(loopTeam, getOwner()))
#else
				if(myTeam.isAtWar(loopTeam) || myTeam.canDeclareWar(loopTeam))
#endif
				{
					bFoundUnit = true;
					break;
				}
			}

			if(!bFoundUnit)
			{
				return false;
			}
		}
	}
	// If it is a City, only consider those we're at war with
	else
	{
		CvAssert(pTargetPlot->getPlotCity() != NULL);

		// If you're already at war don't need to check
		if(!atWar(getTeam(), pTargetPlot->getPlotCity()->getTeam()))
		{
			if(bNeedWar)
			{
				return false;
			}
			// Don't need to be at war with this City's owner (yet)
			else
			{
#if defined(MOD_EVENTS_WAR_AND_PEACE)
				if(!GET_TEAM(getTeam()).canDeclareWar(pTargetPlot->getPlotCity()->getTeam(), getOwner()))
#else
				if(!GET_TEAM(getTeam()).canDeclareWar(pTargetPlot->getPlotCity()->getTeam()))
#endif
				{
					return false;
				}
			}
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
/// Can this Unit air sweep to eliminate interceptors?
bool CvUnit::IsAirSweepCapable() const
{
	return GetAirSweepCapableCount() > 0;
}

//	--------------------------------------------------------------------------------
/// Can this Unit air sweep to eliminate interceptors?
int CvUnit::GetAirSweepCapableCount() const
{
	return m_iAirSweepCapableCount;
}

//	--------------------------------------------------------------------------------
/// Can this Unit air sweep to eliminate interceptors?
void CvUnit::ChangeAirSweepCapableCount(int iChange)
{
	if(iChange != 0)
	{
		m_iAirSweepCapableCount += iChange;
	}
}

//	--------------------------------------------------------------------------------
/// Sweep the skies and clear out interceptors
bool CvUnit::canAirSweep() const
{
	VALIDATE_OBJECT

	if(!IsAirSweepCapable())
	{
		return false;
	}

	if(isEmbarked())
	{
		return false;
	}

	if(GetRange() <= 0)
	{
		return false;
	}

	if(GetBaseRangedCombatStrength() <= 0)
	{
		return false;
	}

	if(isOutOfAttacks())
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
/// Sweep the skies and clear out interceptors
bool CvUnit::canAirSweepAt(int iX, int iY) const
{
	VALIDATE_OBJECT
	if(!canAirSweep())
	{
		return false;
	}

	CvPlot* pSourcePlot = plot();

	CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);

	if(NULL == pTargetPlot)
	{
		return false;
	}

	// Have to be able to see the target plot
	if(!pTargetPlot->isVisible(getTeam()))
	{
		return false;
	}

	// Target plot must be within this unit's air range
	if(plotDistance(pSourcePlot->getX(), pSourcePlot->getY(), pTargetPlot->getX(), pTargetPlot->getY()) > GetRange())
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
/// Sweep the skies and clear out interceptors
bool CvUnit::airSweep(int iX, int iY)
{
	if(!canAirSweepAt(iX, iY))
	{
		return false;
	}

	CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);

	CvUnitCombat::AttackAirSweep(*this, *pTargetPlot, CvUnitCombat::ATTACK_OPTION_NONE);

	return true;
}

//	---------------------------------------------------------------------------
bool CvUnit::isEnemy(TeamTypes eTeam, const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(NULL == pPlot)
	{
		pPlot = plot();
	}

	if(! pPlot)
	{
		return false;
	}
	return (atWar(GET_PLAYER(getCombatOwner(eTeam, *pPlot)).getTeam(), eTeam));
}

//	--------------------------------------------------------------------------------
bool CvUnit::isPotentialEnemy(TeamTypes eTeam, const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(NULL == pPlot)
	{
		pPlot = plot();
	}

	if(! pPlot)
	{
		return false;
	}

	return (::isPotentialEnemy(GET_PLAYER(getCombatOwner(eTeam, *pPlot)).getTeam(), eTeam));
}

//	--------------------------------------------------------------------------------
bool CvUnit::isSuicide() const
{
	VALIDATE_OBJECT
	return (getUnitInfo().IsSuicide() || getKamikazePercent() != 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getDropRange() const
{
	VALIDATE_OBJECT
	return m_iDropRange;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeDropRange(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iDropRange += iChange;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::isTrade() const
{
	VALIDATE_OBJECT
	return getUnitInfo().IsTrade();
}

//	--------------------------------------------------------------------------------
int CvUnit::getAlwaysHostileCount() const
{
	VALIDATE_OBJECT
	return m_iAlwaysHostileCount;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeAlwaysHostileCount(int iValue)
{
	VALIDATE_OBJECT
	m_iAlwaysHostileCount += iValue;
	CvAssert(getAlwaysHostileCount() >= 0);
}


//	--------------------------------------------------------------------------------
bool CvUnit::isAlwaysHostile(const CvPlot& plot) const
{
	VALIDATE_OBJECT
	if(plot.isCity())
	{
		return false;
	}

	return (getAlwaysHostileCount() > 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getArmyID() const
{
	VALIDATE_OBJECT
	return m_iArmyId;
}


//	--------------------------------------------------------------------------------
void CvUnit::setArmyID(int iNewArmyID)
{
	VALIDATE_OBJECT
	m_iArmyId = iNewArmyID;

	if (m_iArmyId!=-1 && GET_PLAYER(getOwner()).GetTacticalAI()->IsUnitHealing(GetID()))
	{
		//shouldn't happen
		OutputDebugString("warning: damaged unit recruited into army!\n");
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsSelected() const
{
	VALIDATE_OBJECT
	return DLLUI->IsUnitSelected(getOwner(), GetID());
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsFirstTimeSelected() const
{
	VALIDATE_OBJECT
	if(IsSelected() && m_iEverSelectedCount == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::IncrementFirstTimeSelected()
{
	VALIDATE_OBJECT
	if(m_iEverSelectedCount < 2)
	{
		m_iEverSelectedCount++;
	}
}


//	--------------------------------------------------------------------------------
void CvUnit::QueueMoveForVisualization(CvPlot* pkPlot)
{
	VALIDATE_OBJECT
	if(pkPlot)
	{
		m_unitMoveLocs.push_back(pkPlot);
		if(m_unitMoveLocs.size() == 20)
		{
			PublishQueuedVisualizationMoves();
		}
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::PublishQueuedVisualizationMoves()
{
	VALIDATE_OBJECT
	auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
	CvPlotIndexVector kPlotArray;
	if(m_unitMoveLocs.size())
	{
		kPlotArray.reserve(m_unitMoveLocs.size());
		for(UnitMovementQueue::const_iterator itr = m_unitMoveLocs.begin(); itr != m_unitMoveLocs.end(); ++itr)
		{
			kPlotArray.push_back((*itr)->GetPlotIndex());
		}
	}
	gDLL->GameplayUnitMoved(pDllUnit.get(), kPlotArray);
	m_unitMoveLocs.clear();
}

//	--------------------------------------------------------------------------------
void CvUnit::SetPosition(CvPlot* pkPlot)
{
	VALIDATE_OBJECT

	auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
	auto_ptr<ICvPlot1> pDllPlot(new CvDllPlot(pkPlot));
	gDLL->GameplayUnitTeleported(pDllUnit.get(), pDllPlot.get());

	m_unitMoveLocs.clear();
}

//	--------------------------------------------------------------------------------
const FAutoArchive& CvUnit::getSyncArchive() const
{
	VALIDATE_OBJECT
	return m_syncArchive;
}

//	--------------------------------------------------------------------------------
FAutoArchive& CvUnit::getSyncArchive()
{
	VALIDATE_OBJECT
	return m_syncArchive;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsDoingPartialMove() const
{
	VALIDATE_OBJECT
	const MissionQueueNode* pkMissionNode = HeadMissionQueueNode();
	if(!pkMissionNode)
	{
		return false;
	}

	if(m_kLastPath.size() == 0)
	{
		return false;
	}

	CvPlot* pEndTurnPlot = GetPathEndTurnPlot();
	CvPlot* pEndPathPlot = GetPathLastPlot();

	if(plot() == pEndTurnPlot && plot() != pEndPathPlot && (pkMissionNode->iData1 == pEndPathPlot->getX() && pkMissionNode->iData2 == pEndPathPlot->getY()))
	{
		if(getMoves() < maxMoves() && getMoves() > 0)
		{
			return true;
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
ActivityTypes CvUnit::GetActivityType() const
{
	VALIDATE_OBJECT
	return m_eActivityType;
}


//	--------------------------------------------------------------------------------
#if defined(MOD_BUGFIX_UNITS_AWAKE_IN_DANGER)
void CvUnit::SetActivityType(ActivityTypes eNewValue, bool bClearFortify)
#else
void CvUnit::SetActivityType(ActivityTypes eNewValue)
#endif
{
	VALIDATE_OBJECT
	CvPlot* pPlot;

	CvAssert(getOwner() != NO_PLAYER);

	ActivityTypes eOldActivity = GetActivityType();

	if(eOldActivity != eNewValue)
	{
		pPlot = plot();

		m_eActivityType = eNewValue;

		// If we're waking up a Unit then remove it's fortification bonus
#if defined(MOD_BUGFIX_UNITS_AWAKE_IN_DANGER)
		if(eNewValue == ACTIVITY_AWAKE && bClearFortify)
#else
		if(eNewValue == ACTIVITY_AWAKE)
#endif
		{
			setFortifyTurns(0);
		}

		auto_ptr<ICvPlot1> pDllSelectionPlot(DLLUI->getSelectionPlot());
		int iSelectionPlotIndex = (pDllSelectionPlot.get() != NULL)? pDllSelectionPlot->GetPlotIndex() : -1;
		if(pPlot->GetPlotIndex() == iSelectionPlotIndex)
		{
			DLLUI->setDirty(PlotListButtons_DIRTY_BIT, true);
			DLLUI->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}

//	--------------------------------------------------------------------------------
AutomateTypes CvUnit::GetAutomateType() const
{
	VALIDATE_OBJECT
	return m_eAutomateType;
}


//	--------------------------------------------------------------------------------
bool CvUnit::IsAutomated() const
{
	VALIDATE_OBJECT
	return (GetAutomateType() != NO_AUTOMATE);
}


//	--------------------------------------------------------------------------------
void CvUnit::SetAutomateType(AutomateTypes eNewValue)
{
	VALIDATE_OBJECT
	CvAssert(getOwner() != NO_PLAYER);

	if(GetAutomateType() != eNewValue)
	{
		AutomateTypes eOldAutomateType = GetAutomateType();
		m_eAutomateType = eNewValue;

		ClearMissionQueue();
		SetActivityType(ACTIVITY_AWAKE);
		if(eOldAutomateType == AUTOMATE_EXPLORE)
		{
			GET_PLAYER(getOwner()).GetEconomicAI()->m_bExplorationPlotsDirty = true; // these need to be rebuilt
		}

		// if canceling automation, cancel on cargo as well
		if(eNewValue == NO_AUTOMATE)
		{
			CvPlot* pPlot = plot();
			if(pPlot != NULL)
			{
				IDInfo* pUnitNode = pPlot->headUnitNode();
				while(pUnitNode != NULL)
				{
					CvUnit* pCargoUnit = ::getUnit(*pUnitNode);
					pUnitNode = pPlot->nextUnitNode(pUnitNode);

					CvUnit* pTransportUnit = pCargoUnit->getTransportUnit();
					if(pTransportUnit != NULL && pTransportUnit == this)
					{
						pCargoUnit->SetAutomateType(NO_AUTOMATE);
						pCargoUnit->SetActivityType(ACTIVITY_AWAKE);
					}
				}
			}
		}
		else if(m_eAutomateType == AUTOMATE_EXPLORE)
		{
			GET_PLAYER(getOwner()).GetEconomicAI()->m_bExplorationPlotsDirty = true; // these need to be rebuilt
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::ReadyToSelect() const
{
	VALIDATE_OBJECT
	return (ReadyToMove() && !IsAutomated());
}


//	--------------------------------------------------------------------------------
bool CvUnit::ReadyToMove() const
{
	VALIDATE_OBJECT
	if(!canMove())
	{
		return false;
	}

	if(GetLengthMissionQueue() != 0)
	{
		return false;
	}

	if(GetActivityType() != ACTIVITY_AWAKE)
	{
		return false;
	}

	if (GetAutomateType() != NO_AUTOMATE)
	{
		return false;
	}

	if(IsBusy())
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::ReadyToAuto() const
{
	VALIDATE_OBJECT
	return (canMove() && (GetLengthMissionQueue() != 0));
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsBusy() const
{
	VALIDATE_OBJECT
	if(GetMissionTimer() > 0)
	{
		return true;
	}

	if(isInCombat())
	{
		return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
#if defined(MOD_BUGFIX_WORKERS_VISIBLE_DANGER) || defined(MOD_BUGFIX_UNITS_AWAKE_IN_DANGER)
bool CvUnit::SentryAlert(bool bSameDomainOrRanged) const
#else
bool CvUnit::SentryAlert() const
#endif
{
	VALIDATE_OBJECT
	int iRange = visibilityRange();
#if defined(MOD_BALANCE_CORE)
	if(getDomainType() == DOMAIN_AIR)
	{
		iRange = GetRange();
		if(iRange > 0)
		{
			for(int iX = -iRange; iX <= iRange; ++iX)
			{
				for(int iY = -iRange; iY <= iRange; ++iY)
				{
					CvPlot* pPlot = ::plotXYWithRangeCheck(getX(), getY(), iX, iY, iRange);
					if(NULL != pPlot)
					{
						if(pPlot->isRevealed(getTeam()))
						{
							if(canRangeStrikeAt(pPlot->getX(), pPlot->getY(), true, false))
							{
								return true;
							}
						}
					}
				}
			}
		}
	}
	else
#endif

	if(iRange > 0)
	{
		CvUnit* pEnemyUnit;

		for(int iX = -iRange; iX <= iRange; ++iX)
		{
			for(int iY = -iRange; iY <= iRange; ++iY)
			{
				CvPlot* pPlot = ::plotXYWithRangeCheck(getX(), getY(), iX, iY, iRange);
				if(NULL != pPlot)
				{
					// because canSeePlot() adds one to the range internally
					if(plot()->canSeePlot(pPlot, getTeam(), (iRange - 1), NO_DIRECTION))
					{
						if(pPlot->isVisibleEnemyUnit(this))
						{
							// Blocker for enemies not in our domain
							pEnemyUnit = pPlot->getVisibleEnemyDefender(getOwner());
							if(pEnemyUnit)
							{
#if defined(MOD_BUGFIX_WORKERS_VISIBLE_DANGER) || defined(MOD_BUGFIX_UNITS_AWAKE_IN_DANGER)
								if (bSameDomainOrRanged)
								{
									if (pEnemyUnit->isRanged() || pEnemyUnit->getDomainType() == getDomainType()) {
										return true;
									}
								}
								else
#endif
									return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsDeclareWar() const
{
	VALIDATE_OBJECT
	if(isHuman())
	{
		return false;
	}
	else
	{
		switch(AI_getUnitAIType())
		{
		case UNITAI_UNKNOWN:
		case UNITAI_SETTLE:
		case UNITAI_WORKER:
			break;
		case UNITAI_CITY_BOMBARD:
			return true;
			break;

		case UNITAI_ATTACK:
		case UNITAI_FAST_ATTACK:
		case UNITAI_PARADROP:
		case UNITAI_DEFENSE:
		case UNITAI_COUNTER:
		case UNITAI_RANGED:
		case UNITAI_CITY_SPECIAL:
		case UNITAI_EXPLORE:
		case UNITAI_ARTIST:
		case UNITAI_SCIENTIST:
		case UNITAI_GENERAL:
		case UNITAI_MERCHANT:
		case UNITAI_ENGINEER:
		case UNITAI_ICBM:
		case UNITAI_WORKER_SEA:
		case UNITAI_SPACESHIP_PART:
		case UNITAI_TREASURE:
		case UNITAI_PROPHET:
		case UNITAI_MISSIONARY:
		case UNITAI_INQUISITOR:
		case UNITAI_ADMIRAL:
		case UNITAI_TRADE_UNIT:
		case UNITAI_ARCHAEOLOGIST:
		case UNITAI_WRITER:
		case UNITAI_MUSICIAN:
#if defined(MOD_DIPLOMACY_CITYSTATES)
		case UNITAI_DIPLOMAT:
		case UNITAI_MESSENGER:
#endif
			break;

		case UNITAI_ATTACK_SEA:
		case UNITAI_RESERVE_SEA:
		case UNITAI_ESCORT_SEA:
		case UNITAI_EXPLORE_SEA:
		case UNITAI_ASSAULT_SEA:
			break;

		case UNITAI_SETTLER_SEA:
		case UNITAI_CARRIER_SEA:
		case UNITAI_MISSILE_CARRIER_SEA:
		case UNITAI_PIRATE_SEA:
		case UNITAI_ATTACK_AIR:
		case UNITAI_DEFENSE_AIR:
		case UNITAI_CARRIER_AIR:
		case UNITAI_MISSILE_AIR:
			break;

		default:
			CvAssert(false);
			break;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
RouteTypes CvUnit::GetBestBuildRoute(CvPlot* pPlot, BuildTypes* peBestBuild) const
{
	VALIDATE_OBJECT

	if(peBestBuild != NULL)
	{
		*peBestBuild = NO_BUILD;
	}

	int iBestValue = 0;
	RouteTypes eBestRoute = NO_ROUTE;
	
#if defined(MOD_GLOBAL_QUICK_ROUTES)
	CvPlayer& kPlayer = GET_PLAYER(getOwner());
	const MissionQueueNode* pkMissionNode = HeadMissionQueueNode();
	bool bOnlyRoad = MOD_GLOBAL_QUICK_ROUTES && kPlayer.isHuman() && (pkMissionNode != NULL && pkMissionNode->eMissionType == CvTypes::getMISSION_ROUTE_TO());
	
	if (bOnlyRoad) {
		// If there is no road to the mission end plot, bOnlyRoad is true
		CvUnit* me = kPlayer.getUnit(m_iID); // God I truely hate "const" - we're in a const method, but LastMissionPlot() isn't so we can't call it!!!
		bOnlyRoad = !IsPlotConnectedToPlot(getOwner(), pPlot, me->LastMissionPlot(), ROUTE_ROAD);
	}
#endif

	for(int iI = 0; iI < GC.getNumBuildInfos(); iI++)
	{
		const BuildTypes eBuild = static_cast<BuildTypes>(iI);
		CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
		if(pkBuildInfo)
		{
			const RouteTypes eRoute = (RouteTypes)pkBuildInfo->getRoute();
			if(eRoute != NO_ROUTE)
			{
				CvRouteInfo* pkRouteInfo = GC.getRouteInfo(eRoute);
				if(pkRouteInfo)
				{
					if(canBuild(pPlot, eBuild))
					{
						int iValue = pkRouteInfo->getValue();
						
#if defined(MOD_GLOBAL_QUICK_ROUTES)
						if (bOnlyRoad && eRoute != ROUTE_ROAD) {
							iValue = 0;
						}
#endif

						if(iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestRoute = eRoute;
							if(peBestBuild != NULL)
							{
								*peBestBuild = eBuild;
							}
						}
					}
				}
			}
		}
	}

	return eBestRoute;
}

//	--------------------------------------------------------------------------------
void CvUnit::PlayActionSound()
{
	VALIDATE_OBJECT
}

//	---------------------------------------------------------------------------
bool CvUnit::UpdatePathCache(CvPlot* pDestPlot, int iFlags)
{
	bool bGenerated = false;
	CvMap& kMap = GC.getMap();
	// If we are in UNITFLAG_EVALUATING_MISSION mode, we can assume that other than the unit that is moving, nothing on the map will change so we can re-use the cached path data more efficiently.
	if ( (m_iFlags & UNITFLAG_EVALUATING_MISSION) != 0 )
	{
		// Some pathing data left?  (the last node is always the current units location)
		if (m_kLastPath.size() >= 2)
		{
			// Was the path destination invisible at the time of generation? See if it is visible now.
			CvPlot* pkPathDest;
			if (m_kLastPath[0].GetFlag((int)CvPathNode::PLOT_INVISIBLE) && (pkPathDest = kMap.plot(m_kLastPath[0].m_iX, m_kLastPath[0].m_iY)) != NULL && pkPathDest->isVisible(getTeam()))
			{
				// The path destination is now visible, recalculate now in case it can't be reached (occupied)
				if ((bGenerated = GeneratePath(pkPathDest, iFlags)) == false && pDestPlot != pkPathDest)
				{
					// Hmm, failed for some reason, re-do the entire path
					bGenerated = GeneratePath(pDestPlot, iFlags);
				}
			}
			else
			{
				// Was the next plot we are stepping into invisible at the time of generation?
				if (m_kLastPath[m_kLastPath.size() - 2].GetFlag((int)CvPathNode::PLOT_INVISIBLE))
				{
					// We are trying to move into a plot that is invisible and we want to continue out into the darkness.  We have to recalculate our path.
					// Since we have already done a path find to the destination once, we can now just path find to how far out we can get in this turn.
					const CvPathNode* pkPathNode = m_kLastPath.GetTurnDest(1);
					if (pkPathNode)
					{
						CvPlot* pkTurnDest = kMap.plot(pkPathNode->m_iX, pkPathNode->m_iY);
						if (pkTurnDest && (bGenerated = GeneratePath(pkTurnDest, iFlags)) == false && pDestPlot != pkTurnDest)
						{
							// Hmm, failed for some reason, re-do the entire path
							bGenerated = GeneratePath(pDestPlot, iFlags);
						}
					}
					else
					{
						bGenerated = GeneratePath(pDestPlot, iFlags);	// Can't find the dest in the path, regenerate
					}
				}
				else
				{
					// The path is still good, just use the next node
					bGenerated = true;
				}
			}
		}
		else
		{
			// If we have already tried this, don't waste time and do it again.
#if defined(MOD_BALANCE_CORE)
			if (m_uiLastPathCacheDest != pDestPlot->GetPlotIndex() || m_uiLastPathFlags != iFlags )	
#else
			if (m_uiLastPathCacheDest != pDestPlot->GetPlotIndex())	
#endif
			{
				bGenerated = GeneratePath(pDestPlot, iFlags);	// Need to regenerate
			}
		}
	}
	else
	{
		// In this case, we can't be sure about the state of the map, so do a full recalculate.
		bGenerated = GeneratePath(pDestPlot, iFlags);
	}

	return bGenerated;
}
//	---------------------------------------------------------------------------
// Returns true if attack was made...
bool CvUnit::UnitAttack(int iX, int iY, int iFlags, int iSteps)
{
	VALIDATE_OBJECT
	CvMap& kMap = GC.getMap();
	CvPlot* pDestPlot = kMap.plot(iX, iY);

	CvAssertMsg(pDestPlot != NULL, "DestPlot is not assigned a valid value");
	if(!pDestPlot)
	{
		return false;
	}

#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
	if (MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS && IsCivilianUnit())
		return false;
#endif

	// Test if this attack requires war to be declared first
	if(isHuman() && getOwner() == GC.getGame().getActivePlayer() && pDestPlot->isVisible(getTeam()))
	{
		TeamTypes eRivalTeam = GetDeclareWarMove(*pDestPlot);

		if(eRivalTeam != NO_TEAM)
		{
			CvPopupInfo kPopup(BUTTONPOPUP_DECLAREWARMOVE);
			kPopup.iData1 = eRivalTeam;
			kPopup.iData2 = pDestPlot->getX();
			kPopup.iData3 = pDestPlot->getY();
			kPopup.bOption1 = false;
			kPopup.bOption2 = pDestPlot->getTeam() != eRivalTeam;
			if(pDestPlot->isCity())
			{
				kPopup.iFlags = DOW_MOVE_ONTO_CITY;
			}
			// If a unit was present, put up the standard DOW message
			else if(pDestPlot->isVisibleOtherUnit(m_eOwner))
			{
				kPopup.iFlags = DOW_MOVE_ONTO_UNIT;
			}
			// Otherwise put out one about entering enemy territory
			else
			{
				kPopup.iFlags = DOW_MOVE_INTO_TERRITORY;
			}
			DLLUI->AddPopup(kPopup);

			return false;
		}
	}

	const CvPathNodeArray& kPathNodeArray = GetPathNodeArray();

	if(iFlags & CvUnit::MOVEFLAG_IGNORE_STACKING)
	{
		if(GeneratePath(pDestPlot, iFlags))
		{
			pDestPlot = GetPathFirstPlot();
		}
	}
	else
	{
		if(getDomainType() != DOMAIN_AIR)
		{
			iSteps += 0;

			UpdatePathCache(pDestPlot, iFlags);
		}
	}

	CvAssertMsg(pDestPlot != NULL, "DestPlot is not assigned a valid value");

	if(!pDestPlot)
	{
		return false;
	}

	bool bAttack = false;
	bool bAdjacent = false;

	if(kPathNodeArray.size() > 1)
	{
		// Previous node is the same plot as the one before the destination (attack point)?
		// Prevents unit from stopping while adjacent to the enemy, but on top of another unit so it has to move to another tile before it can attack
		const CvPathNode& kNode = kPathNodeArray[1];
		if(kNode.m_iX == getX() && kNode.m_iY == getY())
		{
			// The path may not be all the way to the destination, it may be only to the unit's turn limit, so check against the final destination
			const CvPathNode& kDestNode = kPathNodeArray[0];
			if (kDestNode.m_iX == iX && kDestNode.m_iY == iY)
			{
				bAdjacent = true;
			}
		}
	}

	if(bAdjacent || (getDomainType() == DOMAIN_AIR))
	{
		if(!isOutOfAttacks() && (!IsCityAttackOnly() || pDestPlot->isEnemyCity(*this) || !pDestPlot->getBestDefender(NO_PLAYER)))
		{
			// don't allow an attack if we already have one
			if(isFighting() || pDestPlot->isFighting())
			{
				return true;
			}

			// Air mission
			if(getDomainType() == DOMAIN_AIR && GetBaseCombatStrength() == 0)
			{
				if(canRangeStrikeAt(iX, iY))
				{
					CvUnitCombat::AttackAir(*this, *pDestPlot, (iFlags &  MOVEFLAG_NO_DEFENSIVE_SUPPORT)?CvUnitCombat::ATTACK_OPTION_NO_DEFENSIVE_SUPPORT:CvUnitCombat::ATTACK_OPTION_NONE);
					bAttack = true;
				}
			}

			// City combat
			else if(pDestPlot->isCity())
			{
				if(GET_TEAM(getTeam()).isAtWar(pDestPlot->getPlotCity()->getTeam()))
				{
					if(getDomainType() != DOMAIN_AIR)
					{
						// Ranged units that are embarked can't do a move-attack
						if(isRanged() && isEmbarked())
						{
							return false;
						}

						CvUnitCombat::AttackCity(*this, *pDestPlot, (iFlags &  MOVEFLAG_NO_DEFENSIVE_SUPPORT)?CvUnitCombat::ATTACK_OPTION_NO_DEFENSIVE_SUPPORT:CvUnitCombat::ATTACK_OPTION_NONE);
						bAttack = true;
					}
				}
			}

			// Normal unit combat
			else
			{
				// if there are no defenders, do not attack
				UnitHandle pBestDefender = pDestPlot->getBestDefender(NO_PLAYER, getOwner(), this, true);
				if(!pBestDefender)
				{
					return false;
				}

				// Ranged units that are embarked can't do a move-attack
				if(isRanged() && isEmbarked())
				{
					return false;
				}

				bAttack = true;
				CvUnitCombat::Attack(*this, *pDestPlot, (iFlags &  MOVEFLAG_NO_DEFENSIVE_SUPPORT)?CvUnitCombat::ATTACK_OPTION_NO_DEFENSIVE_SUPPORT:CvUnitCombat::ATTACK_OPTION_NONE);
			}

			// Barb camp here that was attacked?
			if(pDestPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
			{
				CvBarbarians::DoCampAttacked(pDestPlot);
			}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
			if(MOD_DIPLOMACY_CITYSTATES_QUESTS && pDestPlot->isCity() && pDestPlot->isBarbarian())
			{
				CvBarbarians::DoCityAttacked(pDestPlot);
			}
#endif
		}
	}

	return bAttack;
}

//	---------------------------------------------------------------------------
bool CvUnit::UnitMove(CvPlot* pPlot, bool bCombat, CvUnit* pCombatUnit, bool bEndMove)
{
	VALIDATE_OBJECT
	bool bCanMoveIntoPlot = false;
	if(canMove() && pPlot)
	{
		// if it's a combat and the attacking unit can capture things or the city is an enemy city.
		if(bCombat && (!(isNoCapture()) || !(pPlot->isEnemyCity(*this))))
		{
			bCanMoveIntoPlot = canMoveOrAttackInto(*pPlot);
		}
		else	VALIDATE_OBJECT

		{
			bCanMoveIntoPlot = canMoveInto(*pPlot);
		}
	}
	VALIDATE_OBJECT


	bool bIsNoCapture = isNoCapture();
	bool bEnemyCity = pPlot && pPlot->isEnemyCity(*this);

	bool bIsCombatUnit = (this == pCombatUnit);
	bool bExecuteMove = false;

	if(pPlot && (bCanMoveIntoPlot || (bIsCombatUnit && !(bIsNoCapture && bEnemyCity))))
	{
		// execute move
		LOG_UNIT_MOVES_MESSAGE_OSTR(std::string("UnitMove() : player ") << GET_PLAYER(getOwner()).getName(); << std::string(" ") << getName() << std::string(" id=") << GetID() << std::string(" moving to ") << pPlot->getX() << std::string(", ") << pPlot->getY());
		move(*pPlot, true);
	}
	else
	{
		bExecuteMove = true;
	}

	//execute move
	if(bEndMove || !canMove() || IsDoingPartialMove())
	{
		bExecuteMove = true;
	}

	if(CvPreGame::quickMovement())
	{
		bExecuteMove = false;
	}

	if(bExecuteMove)
	{
		PublishQueuedVisualizationMoves();
	}

	return bCanMoveIntoPlot;
}

//	---------------------------------------------------------------------------
// Returns the number of turns it will take to reach the target.  
// If no move was made it will return 0. 
// If it can reach the target in one turn or less than one turn (i.e. not use up all its movement points) it will return 1
int CvUnit::UnitPathTo(int iX, int iY, int iFlags, int iPrevETA, bool bBuildingRoute)
{
	VALIDATE_OBJECT
	CvPlot* pDestPlot;
	CvPlot* pPathPlot = NULL;

	LOG_UNIT_MOVES_MESSAGE_OSTR( std::string("UnitPathTo() : player ") << GET_PLAYER(getOwner()).getName() << std::string(" ") << getName() << std::string(" id=") << GetID() << std::string(" moving to ") << iX << std::string(", ") << iY);

	if(at(iX, iY))
	{
		LOG_UNIT_MOVES_MESSAGE("Already at location");
		return 0;
	}

	CvAssert(!IsBusy());
	CvAssert(getOwner() != NO_PLAYER);

	CvMap& kMap = GC.getMap();
	pDestPlot = kMap.plot(iX, iY);
	CvAssertMsg(pDestPlot != NULL, "DestPlot is not assigned a valid value");
	if(!pDestPlot)
	{
		LOG_UNIT_MOVES_MESSAGE("Destination is not a valid plot location");
		return 0;
	}

	CvAssertMsg(canMove(), "canAllMove is expected to be true");

	if(getDomainType() == DOMAIN_AIR)
	{
		if(!canMoveInto(*pDestPlot))
		{
			return 0;
		}

		pPathPlot = pDestPlot;
		m_kLastPath.clear();		// Not used by air units, keep it clear.
	}
	else
	{
		if(bBuildingRoute)
		{
			SPathFinderUserData data(getOwner(),PT_BUILD_ROUTE);
			if(!GC.GetStepFinder().GeneratePath(getX(), getY(), iX, iY, data))
			{
				LOG_UNIT_MOVES_MESSAGE("Unable to generate path with BuildRouteFinder");
				return 0;
			}

			CvAStarNode* pNode = GC.GetStepFinder().GetLastNode();
			if(pNode)
			{
				// walk the nodes until the next node
				while(pNode->m_pParent && pNode->m_pParent->m_pParent)
				{
					pNode = pNode->m_pParent;
				}

				pPathPlot = GC.getMap().plotCheckInvalid(pNode->m_iX, pNode->m_iY);
			}

			if(!pPathPlot || !canMoveInto(*pPathPlot, iFlags | MOVEFLAG_DESTINATION))
			{
				// add route interrupted
				CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
				if(pNotifications)
				{
					CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ROUTE_TO_CANCELLED");
					CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ROUTE_TO_CANCELLED");
					pNotifications->Add(NOTIFICATION_GENERIC, strBuffer, strSummary, getX(), getY(), -1);
				}

#ifdef LOG_UNIT_MOVES
				if(!pPathPlot)
					LOG_UNIT_MOVES_MESSAGE("pPathPlot is NULL");
				else
					LOG_UNIT_MOVES_MESSAGE_OSTR(std::string("Cannot move into pPathPlot ") << pPathPlot->getX() << std::string(" ") << pPathPlot->getY());
#endif
				return 0;
			}
		}
		else
		{
			bool bPathGenerated = UpdatePathCache(pDestPlot, iFlags);

			if (!bPathGenerated)
			{
				LOG_UNIT_MOVES_MESSAGE("Unable to Generate path");
				return 0;
			}

			pPathPlot = GetPathFirstPlot();
		}
	}

	bool bRejectMove = false;

	// slewis'd
	if(m_kLastPath.size() != 0)
	{
		const CvPathNode& kNode = m_kLastPath.front();
		// If we were provided with a previous ETA, check against the new one and if it is two turns greater, cancel the move.
		// We probably revealed something that is causing the unit to take longer to reach the target.
		if(iPrevETA >= 0 && kNode.m_iTurns > iPrevETA + 2)
		{
			LOG_UNIT_MOVES_MESSAGE_OSTR(std::string("Rejecting move iPrevETA=") << iPrevETA << std::string(", m_iData2=") << kNode.m_iData2);
			bRejectMove = true;
		}

		if(kNode.m_iTurns == 1 && !canMoveInto(*pDestPlot))  // if we should end our turn there this turn, but can't move into that tile
		{
			// this is a bit tricky
			// we want to see if this move would be a capture move
			// Since we can't move into the tile, there may be an enemy unit there
			// We can't move into tiles with enemy combat units, so getBestDefender should return null on the tile
			// If there is no defender but we can attack move into the tile, then we know that it is a civilian unit and we should be able to move into it
			const UnitHandle pDefender = pDestPlot->getBestDefender(NO_PLAYER, getOwner(), this, true);
			if(!pDefender && !pDestPlot->isEnemyCity(*this) && canMoveInto(*pDestPlot, MOVEFLAG_ATTACK))
			{
				// Turn on ability to move into enemy units in this case so we can capture civilians
				iFlags |= MOVEFLAG_IGNORE_STACKING;
			}

			const MissionData* pkMissionData = GetHeadMissionData();
			CvAssertMsg(pkMissionData, "Unit mission is null. Need to rethink this code!");
			if(pkMissionData != NULL && pkMissionData->iPushTurn != GC.getGame().getGameTurn())
			{
				LOG_UNIT_MOVES_MESSAGE_OSTR(std::string("Rejecting move pkMissionData->iPushTurn=") << pkMissionData->iPushTurn << std::string(", GC.getGame().getGameTurn()=") << GC.getGame().getGameTurn());
				bRejectMove = true;
			}
		}

		if(bRejectMove)
		{
			m_kLastPath.clear();
			// slewis - perform its queued moves?
			PublishQueuedVisualizationMoves();
			return 0;
		}
	}
	// end slewis'd

	bool bEndMove = (pPathPlot == pDestPlot);
	bool bMoved = UnitMove(pPathPlot, iFlags & MOVEFLAG_IGNORE_STACKING, NULL, bEndMove);

	int iETA = 1;
	uint uiCachedPathSize = m_kLastPath.size();
	if (uiCachedPathSize)
	{
		iETA = m_kLastPath[0].m_iTurns;
		// Only do the shift if we actually moved
		if (bMoved)
		{
			if(uiCachedPathSize > 1)
			{
				// Go back to front, adjusting the turns if needed
				if (m_kLastPath[ uiCachedPathSize - 1 ].m_iTurns != m_kLastPath[ uiCachedPathSize - 2 ].m_iTurns)
				{
					// We have exhausted a turns worth of nodes.  Adjust all the others.
					// KWG: It is unlikely that we will use this path again, because the unit is out of moves anyhow.  Should we just clear the path and save time?
					for (uint uiIndex = uiCachedPathSize - 1; uiIndex--; )
					{
						CvPathNode* pNode = &m_kLastPath[uiIndex];
						if (pNode->m_iTurns > 0)
							pNode->m_iTurns -= 1;
					}
				}

				m_kLastPath.pop_back();
			}
			else
				m_kLastPath.clear();
		}
		else
			m_kLastPath.clear();	// Failed to move, recalculate.
	}

	return iETA;
}

//	---------------------------------------------------------------------------
// Returns true if move was made...
bool CvUnit::UnitRoadTo(int iX, int iY, int iFlags)
{
	VALIDATE_OBJECT
	CvPlot* pPlot;
	RouteTypes eBestRoute;
	BuildTypes eBestBuild;

	if(!IsAutomated() || !at(iX, iY) || (GetLengthMissionQueue() == 1))
	{
		pPlot = plot();

		eBestRoute = GetBestBuildRoute(pPlot, &eBestBuild);

		if(eBestBuild != NO_BUILD)
		{
			UnitBuild(eBestBuild);
			return true;
		}
	}

	bool bWasMoveMade = ( UnitPathTo(iX, iY, iFlags, -1, true) > 0 );
	if(bWasMoveMade)
	{
		PublishQueuedVisualizationMoves();
	}

	return bWasMoveMade;
}


//	--------------------------------------------------------------------------------
// Returns true if build should continue...
bool CvUnit::UnitBuild(BuildTypes eBuild)
{
	VALIDATE_OBJECT
	CvPlot* pPlot;
	bool bContinue;

	CvAssert(getOwner() != NO_PLAYER);
	CvAssertMsg(eBuild < GC.getNumBuildInfos(), "Invalid Build");

	bContinue = false;

	pPlot = plot();
	if(! pPlot)
	{
		return false;
	}

	CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
	if (pkBuildInfo)
	{
		ImprovementTypes eImprovement = (ImprovementTypes)pkBuildInfo->getImprovement();
		if(eImprovement != NO_IMPROVEMENT)
		{
			if(IsAutomated())
			{
				if((pPlot->getImprovementType() != NO_IMPROVEMENT) && (pPlot->getImprovementType() != (ImprovementTypes)(GC.getRUINS_IMPROVEMENT())))
				{
					ResourceTypes eResource = (ResourceTypes)pPlot->getNonObsoleteResourceType(GET_PLAYER(getOwner()).getTeam());
					if((eResource == NO_RESOURCE) || !GC.getImprovementInfo(eImprovement)->IsImprovementResourceTrade(eResource))
					{
						if(GC.getImprovementInfo(eImprovement)->GetImprovementPillage() != NO_IMPROVEMENT)
						{
							return false;
						}
					}
				}
			}
		}
	}

	CvAssertMsg(atPlot(*pPlot), "Unit is expected to be at pPlot");

	// Don't check for Gold cost here (2nd false) because this function is called from continueMission... we spend the Gold then check to see if we can Build
	if(canBuild(pPlot, eBuild, false, false))
	{
		bContinue = true;

		if(build(eBuild))
		{
			bContinue = false;
		}
	}

	return bContinue;
}

//	--------------------------------------------------------------------------------
bool CvUnit::CanDoInterfaceMode(InterfaceModeTypes eInterfaceMode, bool bTestVisibility)
{
	VALIDATE_OBJECT
	CvAssertMsg(eInterfaceMode != NO_INTERFACEMODE, "InterfaceMode is not assigned a valid value");

	if(IsBusy())
	{
		return false;
	}

	switch(eInterfaceMode)
	{
	case INTERFACEMODE_MOVE_TO:
		if((getDomainType() != DOMAIN_AIR) && (getDomainType() != DOMAIN_IMMOBILE) && (!IsImmobile()))
		{
			return true;
		}
		break;

	case INTERFACEMODE_MOVE_TO_TYPE:
		break;

	case INTERFACEMODE_MOVE_TO_ALL:
		break;

	case INTERFACEMODE_ROUTE_TO:
		if(canBuildRoute())
		{
			return true;
		}
		break;

	case INTERFACEMODE_AIRLIFT:
		if(canAirlift(plot()))
		{
			return true;
		}
		break;

	case INTERFACEMODE_NUKE:
		if(canNuke(plot()))
		{
			return true;
		}
		break;

	case INTERFACEMODE_PARADROP:
		if(canParadrop(plot(), bTestVisibility))
		{
			return true;
		}
		break;

	case INTERFACEMODE_RANGE_ATTACK:
		if(canRangeStrike() && getDomainType() != DOMAIN_AIR)
		{
			return true;
		}
		break;

	case INTERFACEMODE_CITY_RANGE_ATTACK:
		break;

	case INTERFACEMODE_ATTACK:
		if(IsCanAttackWithMove() && !isOutOfAttacks())
		{
			if(IsEnemyInMovementRange(false, IsCityAttackOnly()) || bTestVisibility)
			{
				return true;
			}
		}

		break;

	case INTERFACEMODE_AIRSTRIKE:
		if(getDomainType() == DOMAIN_AIR)
		{
			if(canRangeStrike())
			{
				return true;
			}
		}
		break;

	case INTERFACEMODE_AIR_SWEEP:
		if(getDomainType() == DOMAIN_AIR)
		{
			if(canAirSweep())
			{
				return true;
			}
		}
		break;

	case INTERFACEMODE_REBASE:
		if(getDomainType() == DOMAIN_AIR)
		{
			return true;
		}
		break;

	case INTERFACEMODE_EMBARK:
		if(canEmbarkAtPlot(plot()))
		{
			return true;
		}
		break;

	case INTERFACEMODE_DISEMBARK:
		if(canDisembarkAtPlot(plot()))
		{
			return true;
		}
		break;

	case INTERFACEMODE_GIFT_UNIT:
		if(CanDistanceGift((PlayerTypes) DLLUI->GetInterfaceModeValue()))
		{
			return true;
		}
		break;

	case INTERFACEMODE_ESTABLISH_TRADE_ROUTE:
		if (canMakeTradeRoute(plot()))
		{
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
// MISSION ROUTINES
//////////////////////////////////////////////////////////////////////////

#if defined(MOD_BALANCE_CORE_MILITARY)

const char* CvUnit::GetMissionInfo()
{
	if ( (m_eTacticalMove==NO_TACTICAL_MOVE) && (m_eHomelandMove==AI_HOMELAND_MOVE_NONE) )
		m_strMissionInfoString = "no tactical move / no homeland move";
	else
	{
		if (m_eHomelandMove==AI_HOMELAND_MOVE_NONE)
		{
			CvTacticalMoveXMLEntry* pkMoveInfo = GC.getTacticalMoveInfo(m_eTacticalMove);
			if (pkMoveInfo)
			{
				m_strMissionInfoString = (isBarbarian() ? barbarianMoveNames[m_eTacticalMove] : pkMoveInfo->GetType());
				CvString strTemp0 = CvString::format(" (since %d)", GC.getGame().getGameTurn() - m_iTactMoveSetTurn);
				m_strMissionInfoString += strTemp0;
			}
		}

		if (m_eTacticalMove==NO_TACTICAL_MOVE)
		{
			m_strMissionInfoString =  homelandMoveNames[m_eHomelandMove];
			CvString strTemp0 = CvString::format(" (since %d)", GC.getGame().getGameTurn() - m_iHomelandMoveSetTurn);
			m_strMissionInfoString += strTemp0;
		}
	}

	if (m_eGreatPeopleDirectiveType!=NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
	{
		CvString strTemp0 = CvString::format(" // %s", directiveNames[m_eGreatPeopleDirectiveType.get()]);
		m_strMissionInfoString += strTemp0;
	}

	if (m_iMissionAIX!=INVALID_PLOT_COORD && m_iMissionAIY!=INVALID_PLOT_COORD)
	{
		CvString strTemp1;
		getMissionAIString(strTemp1, GetMissionAIType());
		m_strMissionInfoString += " // ";
		m_strMissionInfoString += strTemp1;
		strTemp1.Format(" target: %d,%d", m_iMissionAIX.get(), m_iMissionAIY.get());
		m_strMissionInfoString += strTemp1;
	}

	m_strMissionInfoString += " -----------------------------";

	//this works because it's a member variable!
	return m_strMissionInfoString.c_str();
}

//for debugging: calculate unit danger in all the plots around us
void CvUnit::DumpDangerInNeighborhood()
{
	CvString fname = CvString::format( "Unit%08d_Turn%03d_Danger.txt", GetID(), GC.getGame().getGameTurn() );
	FILogFile* pLog=LOGFILEMGR.GetLog( fname.c_str(), FILogFile::kDontTimeStamp );
	if (!pLog)
		return;

	pLog->Msg( "#x,y,revealed,terrain,state,danger\n" );

	int iRange = 9;
	for (int x = -iRange; x<=iRange; x++)
	for (int y = -iRange; y<=iRange; y++)
	{
		CvPlot* pPlot = GC.getMap().plot( m_iX+x, m_iY+y );
		if (!pPlot)
			continue;

		int iDanger = GET_PLAYER(m_eOwner).GetPlotDanger(*pPlot,this);
		bool bVisible = pPlot->isVisible( GET_PLAYER(m_eOwner).getTeam() );
		bool bHasVisibleEnemyDefender = pPlot->isVisibleEnemyDefender(this);
		bool bHasEnemyUnit = pPlot->getBestDefender(NO_PLAYER,m_eOwner,this);
		bool bHasEnemyCity = pPlot->isEnemyCity(*this);

		int iState = (((int)bHasVisibleEnemyDefender)<<3) + (((int)bHasEnemyCity)<<2) + (((int)bHasEnemyUnit)<<1);

		pLog->Msg( CvString::format( "%03d,%03d,%d,%d,%d,%d\n", 
			pPlot->getX(), pPlot->getY(), bVisible, pPlot->getTerrainType(), iState, iDanger ).c_str() );
	}

	pLog->Close();
}

#endif

//	--------------------------------------------------------------------------------
/// Queue up a new mission
void CvUnit::PushMission(MissionTypes eMission, int iData1, int iData2, int iFlags, bool bAppend, bool bManual, MissionAITypes eMissionAI, CvPlot* pMissionAIPlot, CvUnit* pMissionAIUnit)
{
	VALIDATE_OBJECT

#if defined(MOD_BALANCE_CORE_MILITARY_LOGGING)
	if (MOD_BALANCE_CORE_MILITARY_LOGGING && eMission==CvTypes::getMISSION_MOVE_TO())
	{
		CvPlot* pFromPlot = plot();
		CvPlot* pToPlot = GC.getMap().plot(iData1, iData2);

		if (!pFromPlot || !pToPlot)
			return;

		if ( (!IsCombatUnit() && !pToPlot->getBestDefender(getOwner())) || (IsCombatUnit() && pToPlot->isWater() && getDomainType()==DOMAIN_LAND) )
		{
			int iFromDanger = pFromPlot ? GET_PLAYER(getOwner()).GetPlotDanger(*pFromPlot, this) : 0;
			int iToDanger = pToPlot ? GET_PLAYER(getOwner()).GetPlotDanger(*pToPlot, this) : 0;
			if (iFromDanger < iToDanger || iToDanger==INT_MAX)
				OutputDebugString(CvString::format("%s %s moving into danger at %d,%d!\n", 
					GET_PLAYER(getOwner()).getCivilizationAdjective(), getName().c_str(), iData1, iData2).c_str());
		}
	}
#endif

	//safety check - air units should not use ranged attack missions (for unknown reasons)
	if ( getDomainType()==DOMAIN_AIR && eMission==CvTypes::getMISSION_RANGE_ATTACK() )
		eMission = CvTypes::getMISSION_MOVE_TO();

	CvUnitMission::PushMission(this, eMission, iData1, iData2, iFlags, bAppend, bManual, eMissionAI, pMissionAIPlot, pMissionAIUnit);
}

//	--------------------------------------------------------------------------------
/// Retrieve next mission
void CvUnit::PopMission()
{
	VALIDATE_OBJECT
	CvUnitMission::PopMission(this);
}

//	--------------------------------------------------------------------------------
/// Perform automated mission
void CvUnit::AutoMission()
{
	VALIDATE_OBJECT
	CvUnitMission::AutoMission(this);
}

//	--------------------------------------------------------------------------------
/// Periodic update routine to advance the state of missions
void CvUnit::UpdateMission()
{
	VALIDATE_OBJECT
	CvUnitMission::UpdateMission(m_thisHandle);
}

//	--------------------------------------------------------------------------------
/// Where does this mission end?
CvPlot* CvUnit::LastMissionPlot()
{
	VALIDATE_OBJECT
	return CvUnitMission::LastMissionPlot(this);
}

//	--------------------------------------------------------------------------------
/// Eligible to start a new mission?
bool CvUnit::CanStartMission(int iMission, int iData1, int iData2, CvPlot* pPlot, bool bTestVisible)
{
	VALIDATE_OBJECT
	return CvUnitMission::CanStartMission(this, iMission, iData1, iData2, pPlot, bTestVisible);
}

//	--------------------------------------------------------------------------------
/// Retrieve new value for mission timer
int CvUnit::GetMissionTimer() const
{
	VALIDATE_OBJECT
	return m_iMissionTimer;
}

//	--------------------------------------------------------------------------------
/// Set value from mission timer
void CvUnit::SetMissionTimer(int iNewValue)
{
	VALIDATE_OBJECT
	CvAssert(getOwner() != NO_PLAYER);

	int iOldTimer = m_iMissionTimer;

	m_iMissionTimer = iNewValue;
	CvAssert(GetMissionTimer() >= 0);

	int iNewTimer = m_iMissionTimer;

	if((iOldTimer == 0) != (iNewTimer == 0))
	{
		if((getOwner() == GC.getGame().getActivePlayer()) && IsSelected())
		{
			DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
		}

		if(iNewTimer == 0)
		{
			auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(this));
			gDLL->GameplayUnitMissionEnd(pDllUnit.get());
		}
	}
}

//	--------------------------------------------------------------------------------
/// Change mission timer by a delta value
void CvUnit::ChangeMissionTimer(int iChange)
{
	VALIDATE_OBJECT
	SetMissionTimer(GetMissionTimer() + iChange);
}

//	--------------------------------------------------------------------------------
/// Clear all queued missions
void CvUnit::ClearMissionQueue(int iUnitCycleTimer)
{
	VALIDATE_OBJECT
	CvAssert(getOwner() != NO_PLAYER);

	CvUnitMission::ClearMissionQueue(this, iUnitCycleTimer);
}

//	--------------------------------------------------------------------------------
/// How may missions are in the queue
int CvUnit::GetLengthMissionQueue()	const
{
	VALIDATE_OBJECT
	return m_missionQueue.getLength();
}

//	---------------------------------------------------------------------------
/// Retrieve the data for the first mission in the queue (const correct version)
const MissionData* CvUnit::GetHeadMissionData()
{
	VALIDATE_OBJECT
	if(m_missionQueue.getLength())
		return (m_missionQueue.head());
	return NULL;
}

//	---------------------------------------------------------------------------
const MissionData* CvUnit::GetMissionData(int iIndex)
{
	VALIDATE_OBJECT
	if(iIndex >= 0 && iIndex < m_missionQueue.getLength())
		return m_missionQueue.getAt(iIndex);

	return NULL;
}

//	--------------------------------------------------------------------------------
/// Retrieve the first mission in the queue (const correct version)
const MissionQueueNode* CvUnit::HeadMissionQueueNode() const
{
	VALIDATE_OBJECT
	if(m_missionQueue.getLength() > 0)
	{
		return m_missionQueue.head();
	}
	else
	{
		return NULL;
	}
}

//	--------------------------------------------------------------------------------
/// Retrieve the first mission in the queue
MissionQueueNode* CvUnit::HeadMissionQueueNode()
{
	VALIDATE_OBJECT
	if(m_missionQueue.getLength() > 0)
	{
		return m_missionQueue.head();
	}
	else
	{
		return NULL;
	}
}

//	--------------------------------------------------------------------------------
/// The plot saved with the mission AI data
CvPlot* CvUnit::GetMissionAIPlot() const
{
	VALIDATE_OBJECT
	return GC.getMap().plotCheckInvalid(m_iMissionAIX, m_iMissionAIY);
}

//	--------------------------------------------------------------------------------
/// What is the AI type of the mission?
MissionAITypes CvUnit::GetMissionAIType()
{
	VALIDATE_OBJECT
	return m_eMissionAIType;
}

//	--------------------------------------------------------------------------------
/// Set AI type of the mission
void CvUnit::SetMissionAI(MissionAITypes eNewMissionAI, CvPlot* pNewPlot, CvUnit* pNewUnit)
{
	VALIDATE_OBJECT
	m_eMissionAIType = eNewMissionAI;

	if(pNewPlot != NULL)
	{
		m_iMissionAIX = pNewPlot->getX();
		m_iMissionAIY = pNewPlot->getY();
	}
	else
	{
		m_iMissionAIX = INVALID_PLOT_COORD;
		m_iMissionAIY = INVALID_PLOT_COORD;
	}

	if(pNewUnit != NULL)
	{
		m_missionAIUnit = pNewUnit->GetIDInfo();
	}
	else
	{
		m_missionAIUnit.reset();
	}
}

//	--------------------------------------------------------------------------------
/// Which unit is saved with the mission AI data
CvUnit* CvUnit::GetMissionAIUnit()
{
	VALIDATE_OBJECT
	return getUnit(m_missionAIUnit);
}

// COMBAT ELIGIBILITY ROUTINES

//	--------------------------------------------------------------------------------
/// Is this unit able to ground attack?
bool CvUnit::IsCanAttackWithMove() const
{
	VALIDATE_OBJECT
	if(!IsCombatUnit())
	{
		return false;
	}

	return !isOnlyDefensive();
}

//	--------------------------------------------------------------------------------
/// Does this unit have a ranged attack?
bool CvUnit::IsCanAttackRanged() const
{
	VALIDATE_OBJECT
	return (GetRange() > 0 && GetBaseRangedCombatStrength() > 0);
}

//	--------------------------------------------------------------------------------
/// Can the unit attack either by range or ground attack?
bool CvUnit::IsCanAttack() const
{
	VALIDATE_OBJECT
	return IsCanAttackWithMove() || IsCanAttackRanged();
}

//	--------------------------------------------------------------------------------
/// Any special conditions on the unit right now that prevents an attack?
bool CvUnit::IsCanAttackWithMoveNow() const
{
	VALIDATE_OBJECT
	if(!IsCanAttackWithMove())
	{
		return false;
	}

	// Can't attack out of cities if there is more than 1 combat unit of the same domain in it.
	// This does not apply to air units, which strangely don't show up as combat unit anyhow.
	DomainTypes eSourceDomain = getDomainType();
	CvPlot* pkPlot = plot();
	if (pkPlot->isCity() && eSourceDomain != DOMAIN_AIR)
	{
		IDInfo* pUnitNode = pkPlot->headUnitNode();
		int iCount = 0;
		while(pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
			if(pLoopUnit && pLoopUnit->IsCombatUnit() && pLoopUnit->getDomainType() == eSourceDomain)
			{
				iCount++;
			}

			pUnitNode = pkPlot->nextUnitNode(pUnitNode);
		}

#if defined(MOD_GLOBAL_STACKING_RULES)
		return iCount <= pkPlot->getUnitLimit();
#else
#if defined(MOD_BUGFIX_CITY_STACKING)
		return iCount <= GC.getPLOT_UNIT_LIMIT();
#else
		return iCount <= 1;	// Just us?  Then it is ok.
#endif
#endif
	}

	return true;
}

//	--------------------------------------------------------------------------------
/// Unit able to fight back when attacked?
bool CvUnit::IsCanDefend(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(pPlot == NULL)
	{
		pPlot = plot();
	}

	// This will catch noncombatants
	if(GetBaseCombatStrength() == 0)
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

	if(!pPlot->isValidDomainForAction(*this))
	{
		return false;
	}

	if (isCargo())
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsEnemyInMovementRange(bool bOnlyFortified, bool bOnlyCities)
{
	VALIDATE_OBJECT
	TacticalAIHelpers::ReachablePlotSet plots;
	TacticalAIHelpers::GetAllPlotsInReach(this,plot(),plots,true,true,false);

	for (TacticalAIHelpers::ReachablePlotSet::const_iterator it=plots.begin(); it!=plots.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(it->first);
		if(pPlot->isVisible(getTeam()) && (pPlot->isVisibleEnemyUnit(this) || pPlot->isEnemyCity(*this)))
		{
			if (canMoveInto(*pPlot, CvUnit::MOVEFLAG_ATTACK))
			{
				if(bOnlyFortified)
				{
					CvUnit* pEnemyUnit = pPlot->getVisibleEnemyDefender(getOwner());
					if(pEnemyUnit && pEnemyUnit->IsFortifiedThisTurn())
					{
						return true;
					}
				}
				else if(bOnlyCities)
				{
					if(pPlot->isEnemyCity(*this))
					{
						return true;
					}
				}
				else
				{
					return true;
				}
			}
		}
	}

	return false;
}

// PATH-FINDING ROUTINES

//	--------------------------------------------------------------------------------
/// Use pathfinder to create a path
bool CvUnit::GeneratePath(const CvPlot* pToPlot, int iFlags, int iMaxTurns, int* piPathTurns) const
{
	VALIDATE_OBJECT
	// slewis - a bit of baffling logic
	// we want to catch if the unit is doing a partial move while it is evaluating a mission, because that hoses things
	CvAssertMsg(!(IsDoingPartialMove() && m_iFlags & UNITFLAG_EVALUATING_MISSION), "Repathing during a partial move will cause serious issues!");

	CvAssertMsg(pToPlot != NULL, "Passed in a NULL destination to GeneratePath");
	if(pToPlot == NULL)
		return false;

	bool bSuccess;

	CvTwoLayerPathFinder& kPathFinder = GC.GetPathFinder();
	SPathFinderUserData data(this,iFlags,iMaxTurns);

	bSuccess = kPathFinder.GeneratePath(getX(), getY(), pToPlot->getX(), pToPlot->getY(), data);

	// Regardless of whether or not we made it there, keep track of the plot we tried to path to.  This helps in preventing us from trying to re-path to the same unreachable location.
	m_uiLastPathCacheDest = pToPlot->GetPlotIndex();
#if defined(MOD_BALANCE_CORE)
	m_uiLastPathFlags = iFlags;
#endif

	CopyPath(kPathFinder.GetLastNode(), m_kLastPath);

	if(m_kLastPath.size() != 0)
	{
		CvMap& kMap = GC.getMap();
		TeamTypes eTeam = getTeam();

		// Get the state of the plots in the path, they determine how much of the path is re-usable.
		// KWG: Have the path finder do this for us?
		for (uint uiIndex = m_kLastPath.size(); uiIndex--; )
		{
			CvPathNode& kNode = m_kLastPath[uiIndex];
			CvPlot* pkPlot = kMap.plotCheckInvalid(kNode.m_iX, kNode.m_iY);
			if (pkPlot)
			{
				if (!pkPlot->isVisible(eTeam))
				{
					kNode.SetFlag(CvPathNode::PLOT_INVISIBLE);
					if (uiIndex < (m_kLastPath.size() - 2))
						m_kLastPath[uiIndex + 1].SetFlag(CvPathNode::PLOT_ADJACENT_INVISIBLE);

					// Also determine the destination visibility.  This will be checked in UnitPathTo to see if the destination's visibility has changed and do a re-evaluate again if it has.
					// This will help a unit to stop early in its pathing if the destination is blocked.
					CvPlot* pkPathDest = kMap.plot(m_kLastPath[0].m_iX, m_kLastPath[0].m_iY);
					if (pkPathDest != NULL && !pkPathDest->isVisible(eTeam))
						m_kLastPath[0].SetFlag(CvPathNode::PLOT_INVISIBLE);

					break;	// Anything after is 'in the dark' and should be re-evaluated if trying to move a unit into it.
				}
			}
		}
	}

	if(piPathTurns != NULL)
	{
		*piPathTurns = INT_MAX;

		if(bSuccess)
		{
			if(m_kLastPath.size() != 0)
			{
				*piPathTurns = m_kLastPath.front().m_iTurns;

				//special: if we can reach the target in one turn with movement left, define this as zero turns
				if(*piPathTurns == 1)
				{
					if(m_kLastPath.front().m_iMoves > 0)
					{
						*piPathTurns = 0;
					}
				}
			}
		}
	}

	return bSuccess;
}

//	--------------------------------------------------------------------------------
/// Reset internal pathing data
void CvUnit::ResetPath()
{
	VALIDATE_OBJECT
	m_uiLastPathCacheDest = 0xFFFFFFFF;
	m_uiLastPathFlags = 0xFFFFFFFF;
	m_kLastPath.clear();
}

//	--------------------------------------------------------------------------------
/// What is the first plot along this path?
CvPlot* CvUnit::GetPathFirstPlot() const
{
	VALIDATE_OBJECT

	uint uiPathSize = m_kLastPath.size();
	if(uiPathSize > 0)
	{
		if(uiPathSize == 1)
		{
			const CvPathNode& kNode = m_kLastPath.front();
			return GC.getMap().plotCheckInvalid(kNode.m_iX, kNode.m_iY);
		}
		else
		{
			// The 'first' plot we want is the next to last one.  The last one is always where the unit is currently.
			// Should we even bother caching that one?
			const CvPathNode& kNode = m_kLastPath[ uiPathSize - 2];
			return GC.getMap().plotCheckInvalid(kNode.m_iX, kNode.m_iY);
		}
	}

	CvAssert(false);

	return NULL;
}

//	--------------------------------------------------------------------------------
/// Where does this path end (returns CvPlot*)?
CvPlot* CvUnit::GetPathLastPlot() const
{
	VALIDATE_OBJECT
	if(m_kLastPath.size() > 0)
	{
		const CvPathNode& kNode = m_kLastPath.front();
		return GC.getMap().plotCheckInvalid(kNode.m_iX, kNode.m_iY);
	}

	return NULL;
}

//	--------------------------------------------------------------------------------
/// Returns the last path generated by the unit
const CvPathNodeArray& CvUnit::GetPathNodeArray() const
{
	VALIDATE_OBJECT
	return m_kLastPath;
}

//	--------------------------------------------------------------------------------
/// Clear the pathing cache.  Please use with caution.
void CvUnit::ClearPathCache()
{
	m_kLastPath.setsize(0);
	m_uiLastPathCacheDest = (uint)-1;
#if defined(MOD_BALANCE_CORE)
	m_uiLastPathFlags = 0;
#endif
}


//	--------------------------------------------------------------------------------
/// Where do we end this next move?
CvPlot* CvUnit::GetPathEndTurnPlot() const
{
	VALIDATE_OBJECT

	if(m_kLastPath.size())
	{
		const CvPathNode* pNode = &m_kLastPath[0];

		if(m_kLastPath.size() == 1 || (pNode->m_iTurns == 1))
		{
			return GC.getMap().plotCheckInvalid(pNode->m_iX, pNode->m_iY);
		}

		for(uint uiIndex = 1; uiIndex < m_kLastPath.size(); ++uiIndex)
		{
			pNode = &m_kLastPath[uiIndex];
			if(pNode->m_iTurns == 1)
			{
				return GC.getMap().plotCheckInvalid(pNode->m_iX, pNode->m_iY);
			}
		}
	}

	CvAssert(false);

	return NULL;
}

// PRIVATE METHODS

//	--------------------------------------------------------------------------------
bool CvUnit::canAdvance(const CvPlot& plot, int iThreshold) const
{
	VALIDATE_OBJECT
	CvAssert(IsCombatUnit());
	CvAssert(getDomainType() != DOMAIN_AIR);
	CvAssert(getDomainType() != DOMAIN_IMMOBILE);

	if(plot.getNumVisibleEnemyDefenders(this) > iThreshold)
	{
		return false;
	}

	if(isNoCapture())
	{
		if(plot.isEnemyCity(*this))
		{
			return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
CvUnit* CvUnit::airStrikeTarget(CvPlot& targetPlot, bool bNoncombatAllowed) const
{
	VALIDATE_OBJECT
	UnitHandle pDefender;

	pDefender = targetPlot.getBestDefender(NO_PLAYER, getOwner(), this, true, false, false, bNoncombatAllowed);	// All defaults, except test for war, and allow noncombat units

	if(pDefender)
	{
		if(!pDefender->IsDead())
		{
			return pDefender.pointer();
		}
	}

	return NULL;
}

//	--------------------------------------------------------------------------------
bool CvUnit::CanWithdrawFromMelee(CvUnit& attacker)
{
	VALIDATE_OBJECT
	int iWithdrawChance = getExtraWithdrawal();

	// Does attacker have a speed greater than 1?
	int iAttackerMovementRange = attacker.maxMoves() / GC.getMOVE_DENOMINATOR();
	if(iAttackerMovementRange > 0)
	{
		iWithdrawChance += (GC.getWITHDRAW_MOD_ENEMY_MOVES() * (iAttackerMovementRange - 2));
	}

	// Are some of the retreat hexes away from the attacker blocked?
	int iBlockedHexes = 0;
	CvPlot* pAttackerFromPlot = attacker.plot();
	DirectionTypes eAttackDirection = directionXY(pAttackerFromPlot, plot());
	int iBiases[3] = {0,-1,1};
	int x = plot()->getX();
	int y = plot()->getY();

	for(int i = 0; i < 3; i++)
	{
		int iMovementDirection = (NUM_DIRECTION_TYPES + eAttackDirection + iBiases[i]) % NUM_DIRECTION_TYPES;
		CvPlot* pDestPlot = plotDirection(x, y, (DirectionTypes) iMovementDirection);

		if(pDestPlot && !canMoveInto(*pDestPlot, MOVEFLAG_DESTINATION))
		{
			iBlockedHexes++;
		}
	}

	// If all three hexes away from attacker blocked, we can't withdraw
	if(iBlockedHexes >= 3)
	{
		return false;
	}

	iWithdrawChance += (GC.getWITHDRAW_MOD_BLOCKED_TILE() * iBlockedHexes);

	int iRoll = GC.getGame().getJonRandNum(100, "Withdraw from Melee attempt");

	return iRoll < iWithdrawChance;
}

//	--------------------------------------------------------------------------------
bool CvUnit::DoWithdrawFromMelee(CvUnit& attacker)
{
	VALIDATE_OBJECT
	CvPlot* pAttackerFromPlot = attacker.plot();
	DirectionTypes eAttackDirection = directionXY(pAttackerFromPlot, plot());

	int iRightOrLeftBias = (GC.getGame().getJonRandNum(100, "right or left bias") < 50) ? 1 : -1;
	int iBiases[5] = {0,-1,1,-2,2};
	int x = plot()->getX();
	int y = plot()->getY();

	// try to retreat as close to away from the attacker as possible
	for(int i = 0; i < 5; i++)
	{
		int iMovementDirection = (NUM_DIRECTION_TYPES + eAttackDirection + (iBiases[i] * iRightOrLeftBias)) % NUM_DIRECTION_TYPES;
		CvPlot* pDestPlot = plotDirection(x, y, (DirectionTypes) iMovementDirection);

		if(pDestPlot && canMoveInto(*pDestPlot, MOVEFLAG_DESTINATION | MOVEFLAG_NO_EMBARK))
		{
			setXY(pDestPlot->getX(), pDestPlot->getY(), false, false, true, false);
			return true;
		}
	}
	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnit::CanFallBackFromMelee(CvUnit& attacker)
{
	VALIDATE_OBJECT
	// Are some of the retreat hexes away from the attacker blocked?
	int iBlockedHexes = 0;
	CvPlot* pAttackerFromPlot = attacker.plot();
	DirectionTypes eAttackDirection = directionXY(pAttackerFromPlot, plot());
	int iBiases[3] = {0,-1,1};
	int x = plot()->getX();
	int y = plot()->getY();

	for(int i = 0; i < 3; i++)
	{
		int iMovementDirection = (NUM_DIRECTION_TYPES + eAttackDirection + iBiases[i]) % NUM_DIRECTION_TYPES;
		CvPlot* pDestPlot = plotDirection(x, y, (DirectionTypes) iMovementDirection);

		if(pDestPlot && !canMoveInto(*pDestPlot, MOVEFLAG_DESTINATION))
		{
			iBlockedHexes++;
		}
	}

	// If all three hexes away from attacker blocked, we can't withdraw
	if(iBlockedHexes >= 3)
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::DoFallBackFromMelee(CvUnit& attacker)
{
	VALIDATE_OBJECT

	CvPlot* pAttackerFromPlot = attacker.plot();
	DirectionTypes eAttackDirection = directionXY(pAttackerFromPlot, plot());

	int iRightOrLeftBias = (GC.getGame().getJonRandNum(100, "right or left bias") < 50) ? 1 : -1;
	int iBiases[5] = {0,-1,1,-2,2};
	int x = plot()->getX();
	int y = plot()->getY();

	// try to retreat as close to away from the attacker as possible
	for(int i = 0; i < 5; i++)
	{
		int iMovementDirection = (NUM_DIRECTION_TYPES + eAttackDirection + (iBiases[i] * iRightOrLeftBias)) % NUM_DIRECTION_TYPES;
		CvPlot* pDestPlot = plotDirection(x, y, (DirectionTypes) iMovementDirection);

		if(pDestPlot && canMoveInto(*pDestPlot, MOVEFLAG_DESTINATION) && isMatchingDomain(pDestPlot))
		{
			setXY(pDestPlot->getX(), pDestPlot->getY(), false, false, true, false);
			return true;
		}
	}
	return false;
}

//	--------------------------------------------------------------------------------
UnitAITypes CvUnit::AI_getUnitAIType() const
{
	VALIDATE_OBJECT
	return m_eUnitAIType;
}

//	--------------------------------------------------------------------------------
void CvUnit::AI_setUnitAIType(UnitAITypes eNewValue)
{
	VALIDATE_OBJECT
	CvAssertMsg(eNewValue != NO_UNITAI, "NewValue is not assigned a valid value");

	if(AI_getUnitAIType() != eNewValue)
	{
		m_eUnitAIType = eNewValue;
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::AI_promote()
{
	VALIDATE_OBJECT
	PromotionTypes eBestPromotion;
	int iValue;
	int iBestValue;
	int iI;

	iBestValue = 0;
	eBestPromotion = NO_PROMOTION;
#if defined(MOD_BALANCE_CORE_MILITARY)
	int iNumValidPromotions = 0;
#endif
	for(iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		const PromotionTypes ePromotion(static_cast<PromotionTypes>(iI));

		if(canPromote(ePromotion, -1))
		{
#if defined(MOD_BALANCE_CORE_MILITARY)
			iNumValidPromotions++;
#endif
			iValue = AI_promotionValue(ePromotion);
			if(GC.getLogging() && GC.getAILogging())
			{
				CvPromotionEntry* pkPromotionEntry = GC.getPromotionInfo(ePromotion);
				const char* szPromotionDesc = (pkPromotionEntry != NULL)? pkPromotionEntry->GetDescription() : "Unknown Promotion";

				CvString szMsg;
				szMsg.Format("Promotion, %s, For %s, Value: %d, Damage: %d",
							 szPromotionDesc, getName().GetCString(), iValue, getDamage());
				GET_PLAYER(m_eOwner).GetTacticalAI()->LogTacticalMessage(szMsg, true /*bSkipLogDominanceZone*/);
			}

			if(iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestPromotion = ePromotion;
			}
		}
	}
#if defined(MOD_BALANCE_CORE_MILITARY)
	if(iNumValidPromotions == 1 && eBestPromotion != NO_PROMOTION)
	{
		promote(eBestPromotion, -1);
		//AI_promote();
		CvPromotionEntry* pkPromoInfo = GC.getPromotionInfo(eBestPromotion);

		if (pkPromoInfo && GC.getLogging() && GC.getAILogging())
		{
			CvString szMsg;
			szMsg.Format("Took Only Available Promotion, %s, Received by %s, X: %d, Y: %d, Damage: %d",
						 pkPromoInfo->GetDescription(), getName().GetCString(), getX(), getY(), getDamage());
			GET_PLAYER(m_eOwner).GetTacticalAI()->LogTacticalMessage(szMsg, true /*bSkipLogDominanceZone*/);
		}
	}
	else
#endif
	if(eBestPromotion != NO_PROMOTION)
	{
		promote(eBestPromotion, -1);
		AI_promote();
		CvPromotionEntry* pkPromoInfo = GC.getPromotionInfo(eBestPromotion);

		if (pkPromoInfo && GC.getLogging() && GC.getAILogging())
		{
			CvString szMsg;
			szMsg.Format("Promotion, %s, Received by %s, X: %d, Y: %d, Damage: %d",
						 pkPromoInfo->GetDescription(), getName().GetCString(), getX(), getY(), getDamage());
			GET_PLAYER(m_eOwner).GetTacticalAI()->LogTacticalMessage(szMsg, true /*bSkipLogDominanceZone*/);
		}
	}
}

//	--------------------------------------------------------------------------------
// XXX make sure we include any new UnitAITypes...
int CvUnit::AI_promotionValue(PromotionTypes ePromotion)
{
	VALIDATE_OBJECT

	CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
	if(pkPromotionInfo == NULL)
	{
		//This function really really really should not be called with an invalid promotion type.
		CvAssert(pkPromotionInfo);
		return 0;
	}

	int iValue = 0;

	int iTemp;
	int iExtra;
	int iI;

	// Get flavor info we can use
	CvFlavorManager* pFlavorMgr = GET_PLAYER(m_eOwner).GetFlavorManager();
	int iFlavorOffense = pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE"));
	int iFlavorDefense = pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DEFENSE"));
	int iFlavorRanged = pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RANGED"));
	int iFlavorRecon = pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RECON"));
	int iFlavorMobile = pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_MOBILE"));
	int iFlavorNaval = pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL"));
	int iFlavorAir = pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_AIR"));

	// If we are damaged, insta heal is the way to go
	if(pkPromotionInfo->IsInstaHeal())
	{
		// Half health or less?
		if(getDamage() >= (GetMaxHitPoints() / 2))
		{
			iValue += 1000;   // Enough to lock this one up
		}
	}

#if defined(MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED)
	iTemp = pkPromotionInfo->GetCombatPercent();
	if(iTemp != 0)
	{
		iExtra = getExtraCombatPercent();
		//Let's encourage getting all three tiers asap.
		iTemp *= (100 + iExtra * 11);
		iTemp /= 100;

		iValue += iTemp + iFlavorOffense * 11;

		//Raw power is better for mounted/armor units
		const UnitCombatTypes unitCombatType = getUnitCombatType();
		if(unitCombatType == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_MOUNTED", true))
		{
			iValue *= 3;
			iValue /= 2;
		}
		else if(unitCombatType == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_ARMOR", true))
		{
			iValue *= 3;
			iValue /= 2;
		}
		else
		{
			iValue *= 2;
			iValue /= 3;
		}
	}
	iTemp = pkPromotionInfo->GetRangedAttackModifier();
	if(iTemp != 0)
	{
		iExtra = GetRangedAttackModifier();
		iTemp *= (100 + iExtra * 11);
		iTemp /= 100;
		iValue += iTemp + iFlavorOffense * 11;

		//Raw power is better for archer units than all others
		const UnitCombatTypes unitCombatType = getUnitCombatType();
		if(unitCombatType == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_ARCHER", true))
		{
			iValue *= 3;
			iValue /= 2;
		}
		else
		{
			iValue *= 2;
			iValue /= 3;
		}
	}
#endif

	iTemp = pkPromotionInfo->GetOpenAttackPercent();
	if(iTemp != 0)
	{
		iExtra = getExtraOpenAttackPercent();
		iTemp *= (100 + iExtra * 2);
		iTemp /= 100;
		if(noDefensiveBonus())
		{
			iTemp *= 2;
		}
		iValue += iTemp + iFlavorOffense * 2;
	}

	iTemp = pkPromotionInfo->GetOpenDefensePercent();
	if(iTemp != 0)
	{
		iExtra = getExtraOpenDefensePercent() * 2;
		iTemp *= (100 + iExtra);
		iTemp /= 100;
		if(noDefensiveBonus())
		{
			iTemp *= 2;
		}
		iValue += iTemp + iFlavorDefense * 2;
	}

	iTemp = pkPromotionInfo->GetRoughAttackPercent();
	if(iTemp != 0)
	{
		iExtra = getExtraRoughAttackPercent();
		iTemp *= (100 + iExtra * 2);
		iTemp /= 100;
		if(!noDefensiveBonus())
		{
			iTemp *= 2;
		}
		iValue += iTemp + iFlavorOffense * 2;
	}

	iTemp = pkPromotionInfo->GetRoughDefensePercent();
	if(iTemp != 0)
	{
		iExtra = getExtraRoughDefensePercent() * 2;
		iTemp *= (100 + iExtra);
		iTemp /= 100;
		if(!noDefensiveBonus())
		{
			iTemp *= 2;
		}
		iValue += iTemp + iFlavorDefense * 2;
	}

	iTemp = pkPromotionInfo->GetOpenRangedAttackMod();
	if(iTemp != 0 && isRanged())
	{
		iExtra = getExtraOpenRangedAttackMod();
		iTemp *= (100 + iExtra * 2);
		iTemp /= 100;
		if(noDefensiveBonus())
		{
			iTemp *= 2;
		}
		iValue += iTemp + iFlavorRanged * 2;
	}

	iTemp = pkPromotionInfo->GetRoughRangedAttackMod();
	if(iTemp != 0 && isRanged())
	{
		iExtra = getExtraRoughRangedAttackMod() * 2;
		iTemp *= (100 + iExtra);
		iTemp /= 100;
		if(!noDefensiveBonus())
		{
			iTemp *= 2;
		}
		iValue += iTemp + iFlavorRanged * 2;
	}

	iTemp = pkPromotionInfo->GetVisibilityChange();
	if((AI_getUnitAIType() == UNITAI_EXPLORE_SEA) ||
			(AI_getUnitAIType() == UNITAI_EXPLORE))
	{
		iValue += iTemp * (50 + iFlavorRecon * 2);
	}
	else
	{
		iValue += iTemp * (10 + iFlavorRecon * 2);
	}

	iTemp = pkPromotionInfo->GetCityAttackPercent();
	if(iTemp != 0)
	{
		iExtra = getExtraCityAttackPercent();
		iTemp *= (100 + iExtra * 2);
		iTemp /= 100;
		iValue += iTemp + iFlavorOffense * 2;
		if(isRanged())
		{
			iValue += iFlavorRanged * 2;
		}
	}

	iTemp = pkPromotionInfo->GetCityDefensePercent();
	if(iTemp != 0)
	{
		iExtra = getExtraCityDefensePercent();
		iTemp *= (100 + iExtra * 2);
		iTemp /= 100;
		iValue += iTemp + iFlavorDefense * 2;
		if((AI_getUnitAIType() == UNITAI_DEFENSE) ||
				(AI_getUnitAIType() == UNITAI_COUNTER))
		{
			iValue *= 2;
		}
	}

	iTemp = pkPromotionInfo->GetAttackFortifiedMod();
	if(iTemp != 0 && isRanged())
	{
		iExtra = getExtraAttackFortifiedMod() * 2;
		iTemp *= (100 + iExtra);
		iTemp /= 100;
		iValue += iTemp + (iFlavorOffense * iFlavorRanged) * 2;
	}
#if defined(MOD_BALANCE_CORE)
	iTemp = pkPromotionInfo->GetAttackFortifiedMod();
	if(iTemp != 0 && !isRanged())
	{
		iExtra = getExtraAttackFortifiedMod() * 2;
		iTemp *= (100 + iExtra);
		iTemp /= 100;
		iValue += iTemp + (iFlavorOffense * iFlavorDefense) * 2;
	}
#endif

	iTemp = pkPromotionInfo->GetEnemyHealChange();
	if((AI_getUnitAIType() == UNITAI_PARADROP) ||
			(AI_getUnitAIType() == UNITAI_PIRATE_SEA))
	{
		iValue += iTemp * (50 + iFlavorOffense * 2);
	}
	else
	{
		iValue += iTemp * (10 + iFlavorOffense * 2);
	}

	iTemp = pkPromotionInfo->GetNeutralHealChange();
	if((AI_getUnitAIType() == UNITAI_EXPLORE) ||
			(AI_getUnitAIType() == UNITAI_EXPLORE_SEA))
	{
		iValue += iTemp * (40 + iFlavorRecon * 2);
	}
	else
	{
		iValue += iTemp * (10 + iFlavorRecon * 2);
	}

	iTemp = pkPromotionInfo->GetFriendlyHealChange();
	if((AI_getUnitAIType() == UNITAI_DEFENSE) ||
			(AI_getUnitAIType() == UNITAI_COUNTER))
	{
		iValue += iTemp * (20 + iFlavorDefense * 2);
	}
	else
	{
		iValue += iTemp * (10 + iFlavorDefense * 2);
	}

	iTemp = pkPromotionInfo->GetAdjacentTileHealChange();
	if((AI_getUnitAIType() == UNITAI_DEFENSE) ||
			(AI_getUnitAIType() == UNITAI_COUNTER))
	{
		iValue += iTemp * (40 + iFlavorDefense * 2);
	}
	else
	{
		iValue += iTemp * (10 + iFlavorDefense * 2);
	}

	if(pkPromotionInfo->IsAmphib())
	{
		if((AI_getUnitAIType() == UNITAI_FAST_ATTACK) ||
				(AI_getUnitAIType() == UNITAI_ATTACK))
		{
#if defined(MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED)
			iValue += 50 + iFlavorOffense * 2;
#else
			iValue += 40 + iFlavorOffense * 2;
#endif
		}
		else
		{
#if defined(MOD_BALANCE_CORE_MILITARY)
			iValue += 30 + iFlavorOffense * 2;
#else
			iValue += 10 + iFlavorOffense * 2;
#endif
		}
	}

	if(pkPromotionInfo->IsRiver())
	{
		if((AI_getUnitAIType() == UNITAI_FAST_ATTACK) ||
				(AI_getUnitAIType() == UNITAI_ATTACK))
		{
#if defined(MOD_BALANCE_CORE_MILITARY)
			iValue += 50 + iFlavorOffense * 2;
#else
			iValue += 40 + iFlavorOffense * 2;
#endif
		}
		else
		{
#if defined(MOD_BALANCE_CORE_MILITARY)
			iValue += 30 + iFlavorOffense * 2;
#else
			iValue += 10 + iFlavorOffense * 2;
#endif
		}
	}

	iTemp = pkPromotionInfo->GetRangedDefenseMod();
	if(iTemp != 0)
	{
		iExtra = getExtraRangedDefenseModifier() * 2;
		iTemp *= (100 + iExtra);
		iTemp /= 100;
		// likely not a ranged unit
		if((AI_getUnitAIType() == UNITAI_DEFENSE) || (AI_getUnitAIType() == UNITAI_COUNTER) || (AI_getUnitAIType() == UNITAI_ATTACK))
		{
			iTemp *= 2;
		}
		// a slow unit
		if (maxMoves() / GC.getMOVE_DENOMINATOR() <= 2)
		{
			iTemp *= 2;
		}
		iValue += iTemp + iFlavorDefense * 2;
	}

	if(pkPromotionInfo->IsRangeAttackIgnoreLOS() && isRanged())
	{
		iValue += 75 + iFlavorRanged * 2;
	}

	iTemp = pkPromotionInfo->GetAttackWoundedMod();
	if(iTemp != 0 && isRanged())
	{
		iExtra = getExtraAttackWoundedMod() * 2;
		iTemp *= (100 + iExtra);
		iTemp /= 100;
		iValue += iTemp + (iFlavorOffense * iFlavorRanged) * 2;
	}

	iTemp = pkPromotionInfo->GetFlankAttackModifier();
	if(iTemp > 0)
	{
		iExtra = (2 * iFlavorOffense + iFlavorMobile) * maxMoves() / GC.getMOVE_DENOMINATOR();
		iExtra *= iTemp;
		iExtra /= 100;
		iValue += iExtra;
	}

	if(GC.getPromotionInfo(ePromotion)->IsHealOutsideFriendly() && getDomainType() == DOMAIN_SEA)
	{
		iValue += 50 + iFlavorNaval * 2;
	}

	iTemp = pkPromotionInfo->GetMovesChange();
	if((AI_getUnitAIType() == UNITAI_ATTACK_SEA) ||
			(AI_getUnitAIType() == UNITAI_PIRATE_SEA) ||
			(AI_getUnitAIType() == UNITAI_RESERVE_SEA) ||
			(AI_getUnitAIType() == UNITAI_ESCORT_SEA) ||
			(AI_getUnitAIType() == UNITAI_EXPLORE_SEA) ||
			(AI_getUnitAIType() == UNITAI_ASSAULT_SEA) ||
			(AI_getUnitAIType() == UNITAI_SETTLER_SEA) ||
			(AI_getUnitAIType() == UNITAI_FAST_ATTACK) ||
			(AI_getUnitAIType() == UNITAI_ATTACK) ||
			(AI_getUnitAIType() == UNITAI_PARADROP))
	{
		iValue += iTemp * (50 + iFlavorMobile * 2);
	}
	else
	{
		iValue += iTemp * (30 + iFlavorMobile * 2);
	}

	if(pkPromotionInfo->IsAlwaysHeal())
	{
		if((AI_getUnitAIType() == UNITAI_ATTACK) ||
				(AI_getUnitAIType() == UNITAI_CITY_BOMBARD) ||
				(AI_getUnitAIType() == UNITAI_FAST_ATTACK) ||
				(AI_getUnitAIType() == UNITAI_COUNTER) ||
				(AI_getUnitAIType() == UNITAI_ATTACK_SEA) ||
				(AI_getUnitAIType() == UNITAI_PIRATE_SEA) ||
				(AI_getUnitAIType() == UNITAI_ESCORT_SEA) ||
				(AI_getUnitAIType() == UNITAI_PARADROP))
		{
			iValue += 50 + iFlavorMobile * 2;
		}
		else
		{
			iValue += 30 + iFlavorMobile * 2;
		}
	}

	if(pkPromotionInfo->IsBlitz())
	{
		if((AI_getUnitAIType() == UNITAI_ATTACK) ||
				(AI_getUnitAIType() == UNITAI_CITY_BOMBARD) ||
				(AI_getUnitAIType() == UNITAI_FAST_ATTACK) ||
				(AI_getUnitAIType() == UNITAI_COUNTER) ||
				(AI_getUnitAIType() == UNITAI_ATTACK_SEA) ||
				(AI_getUnitAIType() == UNITAI_PIRATE_SEA) ||
				(AI_getUnitAIType() == UNITAI_ESCORT_SEA) ||
				(AI_getUnitAIType() == UNITAI_PARADROP))
		{
			iValue += 50 + (iFlavorMobile + iFlavorOffense) * 2;
		}
		else
		{
			iValue += 20 + (iFlavorMobile + iFlavorOffense) * 2;
		}
	}

	if(pkPromotionInfo->IsCanMoveAfterAttacking())
	{
		if((AI_getUnitAIType() == UNITAI_ATTACK) ||
				(AI_getUnitAIType() == UNITAI_CITY_BOMBARD) ||
				(AI_getUnitAIType() == UNITAI_FAST_ATTACK) ||
				(AI_getUnitAIType() == UNITAI_COUNTER) ||
				(AI_getUnitAIType() == UNITAI_ATTACK_SEA) ||
				(AI_getUnitAIType() == UNITAI_PIRATE_SEA) ||
				(AI_getUnitAIType() == UNITAI_ESCORT_SEA) ||
				(AI_getUnitAIType() == UNITAI_PARADROP))
		{
			iValue += 30 + (iFlavorMobile + iFlavorOffense) * 2;
		}
		else
		{
			iValue += 10 + (iFlavorMobile + iFlavorOffense) * 2;
		}
	}

	iTemp = pkPromotionInfo->GetExtraAttacks();
	if(iTemp != 0)
	{
		iValue += (iTemp * 200);
	}

	iTemp = pkPromotionInfo->GetRangeChange();
	if(isRanged())
	{
		iValue += (iTemp * 200);
	}

	iTemp = pkPromotionInfo->GetInterceptionCombatModifier();
	if(iTemp != 0 && canAirPatrol(NULL))
	{
		iExtra = GetInterceptionCombatModifier() * 2;
		iTemp *= (100 + iExtra);
		iTemp /= 100;
		iValue += iTemp + iFlavorAir * 2;
	}

	iTemp = pkPromotionInfo->GetAirSweepCombatModifier();
	if(iTemp != 0 && canAirSweep())
	{
		iExtra = GetAirSweepCombatModifier() * 2;
		iTemp *= (100 + iExtra);
		iTemp /= 100;
		iValue += iTemp + iFlavorAir * 2;
	}

	iTemp = pkPromotionInfo->GetNumInterceptionChange();
	if(iTemp != 0)
	{
		iValue += (iTemp * 200);
	}

	iTemp = pkPromotionInfo->GetInterceptionDefenseDamageModifier();
	if(iTemp != 0 && getDomainType() == DOMAIN_AIR)
	{
		iExtra = GetInterceptionDefenseDamageModifier() * 2;
		iTemp *= (100 + iExtra);
		iTemp /= 100;
		iValue += iTemp + iFlavorAir * 2;
	}

	iTemp = pkPromotionInfo->GetDefenseMod();
	if(iTemp != 0)
	{
		iExtra = getDefenseModifier() * 2;
		iTemp *= (100 + iExtra);
		iTemp /= 100;
		iValue += iTemp + iFlavorDefense * 2;
	}

	for(iI = 0; iI < GC.getNumTerrainInfos(); iI++)
	{
		const TerrainTypes eTerrain = static_cast<TerrainTypes>(iI);
		CvTerrainInfo* pkTerrainInfo = GC.getTerrainInfo(eTerrain);
		if(pkTerrainInfo)
		{
			iTemp = pkPromotionInfo->GetTerrainAttackPercent(iI);
			if(iTemp != 0)
			{
				iExtra = getExtraTerrainAttackPercent(eTerrain);
				iTemp *= (100 + iExtra * 2);
				iTemp /= 100;
				iValue += iTemp + iFlavorOffense * 2;
				if((AI_getUnitAIType() == UNITAI_ATTACK) ||
						(AI_getUnitAIType() == UNITAI_FAST_ATTACK))
				{
					iValue *= 2;
				}
			}

			iTemp = pkPromotionInfo->GetTerrainDefensePercent(iI);
			if(iTemp != 0)
			{
				iExtra =  getExtraTerrainDefensePercent(eTerrain);
				iTemp *= (100 + iExtra);
				iTemp /= 100;
				iValue += iTemp + iFlavorDefense * 2;
				if((AI_getUnitAIType() == UNITAI_DEFENSE) ||
						(AI_getUnitAIType() == UNITAI_COUNTER))
				{
					iValue *= 2;
				}
			}

			if(pkPromotionInfo->GetTerrainDoubleMove(iI))
			{
				if(AI_getUnitAIType() == UNITAI_EXPLORE)
				{
					iValue += 5 * (iFlavorRecon + iFlavorMobile);
				}
				else if((AI_getUnitAIType() == UNITAI_ATTACK) || (AI_getUnitAIType() == UNITAI_FAST_ATTACK))
				{
					iValue += 3 * (iFlavorOffense + iFlavorMobile);
				}
				else
				{
					iValue += 3 * iFlavorMobile;
				}
			}

#if defined(MOD_PROMOTIONS_HALF_MOVE)
			if(pkPromotionInfo->GetTerrainHalfMove(iI))
			{
				if(AI_getUnitAIType() == UNITAI_EXPLORE)
				{
					iValue -= 5 * (iFlavorRecon + iFlavorMobile);
				}
				else if((AI_getUnitAIType() == UNITAI_ATTACK) || (AI_getUnitAIType() == UNITAI_FAST_ATTACK))
				{
					iValue -= 3 * (iFlavorOffense + iFlavorMobile);
				}
				else
				{
					iValue -= 3 * iFlavorMobile;
				}
			}
#endif
		}
	}

	for(iI = 0; iI < GC.getNumFeatureInfos(); iI++)
	{
		const FeatureTypes eFeature = static_cast<FeatureTypes>(iI);
		CvFeatureInfo* pkFeatureInfo = GC.getFeatureInfo(eFeature);
		if(pkFeatureInfo)
		{
			iTemp = pkPromotionInfo->GetFeatureAttackPercent(iI);
			if(iTemp != 0)
			{
				iExtra = getExtraFeatureAttackPercent(eFeature);
				iTemp *= (100 + iExtra * 2);
				iTemp /= 100;
				iValue += iTemp + iFlavorOffense * 2;
				if((AI_getUnitAIType() == UNITAI_ATTACK) ||
						(AI_getUnitAIType() == UNITAI_FAST_ATTACK))
				{
					iValue *= 2;
				}
			}

			iTemp = pkPromotionInfo->GetFeatureDefensePercent(iI);;
			if(iTemp != 0)
			{
				iExtra = getExtraFeatureDefensePercent(eFeature);
				iTemp *= (100 + iExtra * 2);
				iTemp /= 100;
				iValue += iTemp + iFlavorDefense * 2;
				if((AI_getUnitAIType() == UNITAI_DEFENSE) ||
						(AI_getUnitAIType() == UNITAI_COUNTER))
				{
					iValue *= 2;
				}
			}

			if(pkPromotionInfo->GetFeatureDoubleMove(iI))
			{
				if(AI_getUnitAIType() == UNITAI_EXPLORE)
				{
					iValue += 5 * (iFlavorRecon + iFlavorMobile);
				}
				else if((AI_getUnitAIType() == UNITAI_ATTACK) || (AI_getUnitAIType() == UNITAI_FAST_ATTACK))
				{
					iValue += 3 * (iFlavorOffense + iFlavorMobile);
				}
				else
				{
					iValue += 3 * iFlavorMobile;
				}
			}

#if defined(MOD_PROMOTIONS_HALF_MOVE)
			if(pkPromotionInfo->GetFeatureHalfMove(iI))
			{
				if(AI_getUnitAIType() == UNITAI_EXPLORE)
				{
					iValue -= 5 * (iFlavorRecon + iFlavorMobile);
				}
				else if((AI_getUnitAIType() == UNITAI_ATTACK) || (AI_getUnitAIType() == UNITAI_FAST_ATTACK))
				{
					iValue -= 3 * (iFlavorOffense + iFlavorMobile);
				}
				else
				{
					iValue -= 3 * iFlavorMobile;
				}
			}
#endif
		}
	}

	int iOtherCombat = 0;
	int iSameCombat = 0;

	for(iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
	{
		const UnitCombatTypes eUnitCombat = static_cast<UnitCombatTypes>(iI);
		CvBaseInfo* pkUnitCombatInfo = GC.getUnitCombatClassInfo(eUnitCombat);
		if(pkUnitCombatInfo)
		{
			if(eUnitCombat == getUnitCombatType())
			{
				iSameCombat += unitCombatModifier(eUnitCombat);
			}
			else
			{
				iOtherCombat += unitCombatModifier(eUnitCombat);
			}
		}
	}

	for(iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
	{
		const UnitCombatTypes eUnitCombat = static_cast<UnitCombatTypes>(iI);
		CvBaseInfo* pkUnitCombatInfo = GC.getUnitCombatClassInfo(eUnitCombat);
		if(pkUnitCombatInfo)
		{
			iTemp = pkPromotionInfo->GetUnitCombatModifierPercent(iI);
			int iCombatWeight = 0;
			//Fighting their own kind
			if((UnitCombatTypes)iI == getUnitCombatType())
			{
				if(iSameCombat >= iOtherCombat)
				{
					iCombatWeight = 70;//"axeman takes formation"
				}
				else
				{
					iCombatWeight = 30;
				}
			}
			else
			{
				//fighting other kinds
				if(unitCombatModifier(eUnitCombat) > 10)
				{
					iCombatWeight = 70;//"spearman takes formation"
				}
				else
				{
					iCombatWeight = 30;
				}
			}

			if((AI_getUnitAIType() == UNITAI_COUNTER) || (AI_getUnitAIType() == UNITAI_RANGED))
			{
				iValue += (iTemp * iCombatWeight) / 25;
			}
			else if((AI_getUnitAIType() == UNITAI_ATTACK) ||
					(AI_getUnitAIType() == UNITAI_DEFENSE))
			{
				iValue += (iTemp * iCombatWeight) / 50;
			}
			else
			{
				iValue += (iTemp * iCombatWeight) / 100;
			}
		}
	}

	for(iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		iTemp = pkPromotionInfo->GetDomainModifierPercent(iI);
		if(AI_getUnitAIType() == UNITAI_COUNTER)
		{
			iValue += (iTemp * 2);
		}
		else if((AI_getUnitAIType() == UNITAI_ATTACK) ||
				(AI_getUnitAIType() == UNITAI_DEFENSE))
		{
			iValue += iTemp;
		}
		else
		{
			iValue += (iTemp / 2);
		}
	}

	if(iValue > 0)
	{
#ifdef AUI_BINOM_RNG
		iValue += GC.getGame().getJonRandNumBinom(iValue, "AI Promote");
#else
		iValue += GC.getGame().getJonRandNum(15, "AI Promote");
#endif // AUI_BINOM_RNG
	}

	return iValue;
}

//	--------------------------------------------------------------------------------
GreatPeopleDirectiveTypes CvUnit::GetGreatPeopleDirective() const
{
	return m_eGreatPeopleDirectiveType;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetGreatPeopleDirective(GreatPeopleDirectiveTypes eDirective)
{
	m_eGreatPeopleDirectiveType = eDirective;
}


//	--------------------------------------------------------------------------------
bool CvUnit::IsHasBeenPromotedFromGoody() const
{
	return m_iFlags & UNITFLAG_ALREADY_GOT_GOODY_UPGRADE;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetBeenPromotedFromGoody(bool bBeenPromoted)
{
	if(bBeenPromoted)
	{
		m_iFlags = m_iFlags | UNITFLAG_ALREADY_GOT_GOODY_UPGRADE;
	}
	else
	{
		m_iFlags = m_iFlags & ~UNITFLAG_ALREADY_GOT_GOODY_UPGRADE;
	}
}

#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
bool CvUnit::IsHigherPopThan(const CvUnit* pOtherUnit) const
{
	int iMyPop = 0;
	int iOtherPop = 0;

	CvPlayer& kPlayer = GET_PLAYER(getOwner());
	iMyPop = kPlayer.getTotalPopulation();

	CvPlayer& kOtherPlayer = GET_PLAYER(pOtherUnit->getOwner());
	iOtherPop = kOtherPlayer.getTotalPopulation();

	return iMyPop > iOtherPop;
}
#endif

//	--------------------------------------------------------------------------------
bool CvUnit::IsHigherTechThan(UnitTypes otherUnit) const
{
	int iMyTechCost = 0;
	int iOtherTechCost = 0;

	TechTypes eMyTech = (TechTypes)getUnitInfo().GetPrereqAndTech();
	if(eMyTech != NO_TECH)
	{
		CvTechEntry* pEntry = GC.GetGameTechs()->GetEntry(eMyTech);
		if(pEntry)
		{
			iMyTechCost = pEntry->GetResearchCost();
		}
	}

	CvUnitEntry* pUnitEntry = GC.GetGameUnits()->GetEntry(otherUnit);
	if(pUnitEntry)
	{
		TechTypes eOtherTech = (TechTypes)pUnitEntry->GetPrereqAndTech();
		if(eOtherTech != NO_TECH)
		{
			CvTechEntry* pEntry = GC.GetGameTechs()->GetEntry(eOtherTech);
			if(pEntry)
			{
				iOtherTechCost = pEntry->GetResearchCost();
			}
		}
	}

	return iMyTechCost > iOtherTechCost;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsLargerCivThan(const CvUnit* pOtherUnit) const
{
	int iMyCities = 0;
	int iOtherCities = 0;

	CvPlayer& kPlayer = GET_PLAYER(getOwner());
	iMyCities = kPlayer.getNumCities();

	CvPlayer& kOtherPlayer = GET_PLAYER(pOtherUnit->getOwner());
	iOtherCities = kOtherPlayer.getNumCities();

	return iMyCities > iOtherCities;
}

//	--------------------------------------------------------------------------------
void CvUnit::dispatchingNetMessage(bool dispatchingMessage)
{
	s_dispatchingNetMessage = dispatchingMessage;
}

//	--------------------------------------------------------------------------------
bool CvUnit::dispatchingNetMessage()
{
	return s_dispatchingNetMessage;
}

//	---------------------------------------------------------------------------------
//	Return true if the unit should change/update the map visibility.
bool CvUnit::canChangeVisibility() const
{
	return m_iMapLayer == DEFAULT_UNIT_MAP_LAYER;
}

//	--------------------------------------------------------------------------------
std::string CvUnit::debugDump(const FAutoVariableBase& /*var*/) const
{
	// TODO: This... just can't be correct.  Surely, there must be something useful
	// this function is supposed to do.  Unfortunately, I don't know what that is.
	static const std::string empty("");
	return empty;
}

//	--------------------------------------------------------------------------------
std::string CvUnit::stackTraceRemark(const FAutoVariableBase& var) const
{
	std::string result = "Game Turn : ";
	char gameTurnBuffer[8] = {0};
	int gameTurn = GC.getGame().getGameTurn();
	sprintf_s(gameTurnBuffer, "%d\0", gameTurn);
	result += gameTurnBuffer;
	result += "\nValue Before Change=" + FSerialization::toString(var);
	if(&var == &m_eActivityType)
	{
		result += "\nm_eActivityType changes based on canMove().\n";
		bool moves = getMoves() > 0;
		if(canMove())
		{
			result += "canMove() returned true because ";
			if(moves)
			{
				result += "getMoves() > 0\n";
				result += "---- Call Stack for m_iMoves last change: ----\n";
				result += m_iMoves.getStackTrace();
				result += "---- END STACK TRACE FOR m_iMoves ----\n";
			}
			else
			{
				result += "some unknown check was added there but not here in the remark code\n";
			}
		}
		else
		{
			result += "\ncanMove() return false because ";
			if(!moves)
			{
				result += "getMoves() == 0\n";
				result += "---- Call Stack for m_iMoves last change: ----\n";
				result += m_iMoves.getStackTrace();
				result += "---- END STACK TRACE FOR m_iMoves ----\n";

			}
			else
			{
				result += "some unknown check was added there but not here in the remark code\n";
			}
		}
	}
	return result;
}

#if defined(MOD_API_EXTENSIONS)
bool CvUnit::IsCivilization(CivilizationTypes iCivilizationType) const
{
	return (GET_PLAYER(getOwner()).getCivilizationType() == iCivilizationType);
}

bool CvUnit::HasPromotion(PromotionTypes iPromotionType) const
{
	return isHasPromotion(iPromotionType);
}

bool CvUnit::IsUnit(UnitTypes iUnitType) const
{
	return (getUnitType() == iUnitType);
}

bool CvUnit::IsUnitClass(UnitClassTypes iUnitClassType) const
{
	return (getUnitClassType() == iUnitClassType);
}

bool CvUnit::IsOnFeature(FeatureTypes iFeatureType) const
{
	return plot()->HasFeature(iFeatureType);
}

bool CvUnit::IsAdjacentToFeature(FeatureTypes iFeatureType) const
{
	return plot()->IsAdjacentToFeature(iFeatureType);
}

bool CvUnit::IsWithinDistanceOfFeature(FeatureTypes iFeatureType, int iDistance) const
{
	return plot()->IsWithinDistanceOfFeature(iFeatureType, iDistance);
}
#if defined(MOD_BALANCE_CORE)
bool CvUnit::IsWithinDistanceOfUnit(UnitTypes eOtherUnit, int iDistance, bool bIsFriendly, bool bIsEnemy) const
{
	return plot()->IsWithinDistanceOfUnit(getOwner(), eOtherUnit, iDistance, bIsFriendly, bIsEnemy);
}
bool CvUnit::IsWithinDistanceOfUnitClass(UnitClassTypes eUnitClass, int iDistance, bool bIsFriendly, bool bIsEnemy) const
{
	return plot()->IsWithinDistanceOfUnitClass(getOwner(), eUnitClass, iDistance, bIsFriendly, bIsEnemy);
}
bool CvUnit::IsWithinDistanceOfUnitCombatType(UnitCombatTypes eUnitCombat, int iDistance, bool bIsFriendly, bool bIsEnemy) const
{
	return plot()->IsWithinDistanceOfUnitCombatType(getOwner(), eUnitCombat, iDistance, bIsFriendly, bIsEnemy);
}
bool CvUnit::IsWithinDistanceOfUnitPromotion(PromotionTypes eUnitPromotion, int iDistance, bool bIsFriendly, bool bIsEnemy) const
{
	return plot()->IsWithinDistanceOfUnitPromotion(getOwner(), eUnitPromotion, iDistance, bIsFriendly, bIsEnemy);
}
bool CvUnit::IsAdjacentToUnit(UnitTypes eOtherUnit, bool bIsFriendly, bool bIsEnemy) const
{
	return plot()->IsAdjacentToUnit(getOwner(), eOtherUnit, bIsFriendly, bIsEnemy);
}
bool CvUnit::IsAdjacentToUnitClass(UnitClassTypes eUnitClass, bool bIsFriendly, bool bIsEnemy) const
{
	return plot()->IsAdjacentToUnitClass(getOwner(), eUnitClass, bIsFriendly, bIsEnemy);
}
bool CvUnit::IsAdjacentToUnitCombatType(UnitCombatTypes eUnitCombat, bool bIsFriendly, bool bIsEnemy) const
{
	return plot()->IsAdjacentToUnitCombatType(getOwner(), eUnitCombat, bIsFriendly, bIsEnemy);
}
bool CvUnit::IsAdjacentToUnitPromotion(PromotionTypes eUnitPromotion, bool bIsFriendly, bool bIsEnemy) const
{
	return plot()->IsAdjacentToUnitPromotion(getOwner(), eUnitPromotion, bIsFriendly, bIsEnemy);
}
#endif
bool CvUnit::IsOnImprovement(ImprovementTypes iImprovementType) const
{
	return plot()->HasImprovement(iImprovementType);
}

bool CvUnit::IsAdjacentToImprovement(ImprovementTypes iImprovementType) const
{
	return plot()->IsAdjacentToImprovement(iImprovementType);
}

bool CvUnit::IsWithinDistanceOfImprovement(ImprovementTypes iImprovementType, int iDistance) const
{
	return plot()->IsWithinDistanceOfImprovement(iImprovementType, iDistance);
}

bool CvUnit::IsOnPlotType(PlotTypes iPlotType) const
{
	return plot()->HasPlotType(iPlotType);
}

bool CvUnit::IsAdjacentToPlotType(PlotTypes iPlotType) const
{
	return plot()->IsAdjacentToPlotType(iPlotType);
}

bool CvUnit::IsWithinDistanceOfPlotType(PlotTypes iPlotType, int iDistance) const
{
	return plot()->IsWithinDistanceOfPlotType(iPlotType, iDistance);
}

bool CvUnit::IsOnResource(ResourceTypes iResourceType) const
{
	return plot()->HasResource(iResourceType);
}

bool CvUnit::IsAdjacentToResource(ResourceTypes iResourceType) const
{
	return plot()->IsAdjacentToResource(iResourceType);
}

bool CvUnit::IsWithinDistanceOfResource(ResourceTypes iResourceType, int iDistance) const
{
	return plot()->IsWithinDistanceOfResource(iResourceType, iDistance);
}

bool CvUnit::IsOnTerrain(TerrainTypes iTerrainType) const
{
	return plot()->HasTerrain(iTerrainType);
}

bool CvUnit::IsAdjacentToTerrain(TerrainTypes iTerrainType) const
{
	return plot()->IsAdjacentToTerrain(iTerrainType);
}

bool CvUnit::IsWithinDistanceOfTerrain(TerrainTypes iTerrainType, int iDistance) const
{
	return plot()->IsWithinDistanceOfTerrain(iTerrainType, iDistance);
}
#endif


//	--------------------------------------------------------------------------------
/// Can a unit reach this destination in "X" turns of movement? (pass in 0 if need to make it in 1 turn with movement left)
bool CvUnit::CanReachInXTurns(const CvPlot* pTarget, int iTurns, bool bIgnoreUnits, int* piTurns /* = NULL */)
{
	if (!pTarget)
		return false;

	int iTurnsCalculated = TurnsToReachTarget(pTarget, bIgnoreUnits, false, iTurns);
	if (piTurns)
		*piTurns = iTurnsCalculated;

	return (iTurnsCalculated <= iTurns);
}

//	--------------------------------------------------------------------------------
/// How many turns will it take a unit to get to a target plot (returns MAX_INT if can't reach at all; returns 0 if makes it in 1 turn and has movement left)
int CvUnit::TurnsToReachTarget(const CvPlot* pTarget, bool bIgnoreUnits, bool bIgnoreStacking, int iTargetTurns)
{
	if(!pTarget)
		return INT_MAX;

	//make sure that iTargetTurns is valid
	iTargetTurns = max(1,iTargetTurns);

	if (iTargetTurns!=INT_MAX)
	{
		//see how far we could move in optimal circumstances
		int	iDistance = plotDistance(getX(), getY(), pTarget->getX(), pTarget->getY());

		//default range
		int iMoves = baseMoves() + getExtraMoves();
		int iRange = iMoves * iTargetTurns;

		//catch stupid cases
		if (iRange<=0)
			return pTarget==plot() ? INT_MAX : 0;

		//but routes increase it
		if (getDomainType()==DOMAIN_LAND && !flatMovementCost() )
		{
			CvTeam& kTeam = GET_TEAM(getTeam());
			RouteTypes eBestRouteType = kTeam.GetBestPossibleRoute();
			CvRouteInfo* pRouteInfo = GC.getRouteInfo(eBestRouteType);
			if (pRouteInfo &&  (pRouteInfo->getMovementCost() + kTeam.getRouteChange(eBestRouteType) != 0))
			{
				int iMultiplier = GC.getMOVE_DENOMINATOR() / (pRouteInfo->getMovementCost() + kTeam.getRouteChange(eBestRouteType));

				if (plot()->getRouteType()!=NO_ROUTE)
					//standing directly on a route
					iRange = iMoves * iTargetTurns * iMultiplier;
				else
					//need to move at least one plot in the first turn at full cost to get to the route - speed optimization for railroad and low turn count
					iRange = 1 + (iMoves-1)*iMultiplier + iMoves*(iTargetTurns-1)*iMultiplier;
			}
		}

		if (iDistance>iRange)
			return INT_MAX;
	}


	int rtnValue = MAX_INT;
	if(pTarget == plot())
		return 0;

	if(bIgnoreUnits) //ignore all other units (also enemy)
	{
		//this is a purely hypothetical path ... use a special pathfinder for that
		SPathFinderUserData data(this,CvUnit::MOVEFLAG_IGNORE_DANGER,iTargetTurns);
		data.ePathType	= PT_UNIT_IGNORE_OTHERS;

		if (GC.GetIgnoreUnitsPathFinder().GeneratePath(getX(), getY(), pTarget->getX(), pTarget->getY(), data))
		{
			CvAStarNode* pNode = GC.GetIgnoreUnitsPathFinder().GetLastNode();

			rtnValue = pNode->m_iTurns;
			if(rtnValue == 1)
			{
				if(pNode->m_iMoves > 0)
				{
					rtnValue = 0;
				}
			}
		}
	}
	else
	{
		//normal handling: use the path cache of the unit, so we can possibly re-use the path later
		//do not ignore danger
		int iFlags = 0;
		if(bIgnoreStacking) //ignore our own units
			iFlags |= CvUnit::MOVEFLAG_IGNORE_STACKING;

		if (!GeneratePath(pTarget, iFlags, iTargetTurns, &rtnValue) )
			return INT_MAX;
	}

	return rtnValue;
}


//	--------------------------------------------------------------------------------
DestructionNotification<UnitHandle>& CvUnit::getDestructionNotification()
{
	return m_destructionNotification;
}

//	--------------------------------------------------------------------------------
FDataStream& operator<<(FDataStream& saveTo, const CvUnit& readFrom)
{
	readFrom.write(saveTo);
	return saveTo;
}

//	--------------------------------------------------------------------------------
FDataStream& operator>>(FDataStream& loadFrom, CvUnit& writeTo)
{
	writeTo.read(loadFrom);
	return loadFrom;
}
