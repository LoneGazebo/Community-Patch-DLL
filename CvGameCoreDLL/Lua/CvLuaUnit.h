/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#pragma once
#ifndef CVLUAUNIT_H
#define CVLUAUNIT_H

#include "CvLuaScopedInstance.h"

class CvLuaUnit : public CvLuaScopedInstance<CvLuaUnit, CvUnit>
{
public:
	//! Push CvUnit methods into table t
	static void PushMethods(lua_State* L, int t);

	//! Error handle for missing instances
	static void HandleMissingInstance(lua_State* L);

	//! Gets the type name
	static const char* GetTypeName();

protected:

	static int lIsNone(lua_State* L);
	static int lConvert(lua_State* L);
	static int lKill(lua_State* L);

	static int lIsActionRecommended(lua_State* L);
	static int lIsBetterDefenderThan(lua_State* L);

	static int lCanDoCommand(lua_State* L);
	static int lDoCommand(lua_State* L);

	static int lGetPathEndTurnPlot(lua_State* L);
	static int lGeneratePath(lua_State* L);

	static int lCanEnterTerritory(lua_State* L);
	static int lCanEnterArea(lua_State* L);
	static int lGetDeclareWarRangeStrike(lua_State* L);
	static int lCanMoveOrAttackInto(lua_State* L);
	static int lCanMoveThrough(lua_State* L);
	static int lJumpToNearestValidPlot(lua_State* L);

	static int lGetCombatDamage(lua_State* L);
	static int lGetFireSupportUnit(lua_State* L);

	static int lCanAutomate(lua_State* L);
	static int lCanScrap(lua_State* L);
	static int lGetScrapGold(lua_State* L);
	static int lCanGift(lua_State* L);
	static int lCanDistanceGift(lua_State* L);
	static int lCanLoadUnit(lua_State* L);
	static int lCanLoad(lua_State* L);
	static int lCanUnload(lua_State* L);
	static int lCanUnloadAll(lua_State* L);
	static int lCanHold(lua_State* L);
	static int lCanSleep(lua_State* L);
	static int lCanFortify(lua_State* L);
	static int lCanAirPatrol(lua_State* L);
	static int lIsEmbarked(lua_State* L);
	static int lSetEmbarked(lua_State* L);
	static int lCanSeaPatrol(lua_State* L);
	static int lCanHeal(lua_State* L);
	static int lCanSentry(lua_State* L);
	static int lCanEmbark(lua_State* L);
	static int lCanDisembark(lua_State* L);
	static int lCanEmbarkOnto(lua_State* L);
	static int lCanDisembarkOnto(lua_State* L);
	static int lCanRebaseAt(lua_State* L);
	static int lEmbark(lua_State* L);

	static int lIsRangeAttackIgnoreLOS(lua_State* L);

	static int lCanAirlift(lua_State* L);
	static int lCanAirliftAt(lua_State* L);

	static int lIsNukeVictim(lua_State* L);
	static int lCanNuke(lua_State* L);
	static int lCanNukeAt(lua_State* L);

	static int lCanRangeStrike(lua_State* L);
	static int lCanRangeStrikeAt(lua_State* L);

	static int lCanParadrop(lua_State* L);
	static int lCanParadropAt(lua_State* L);

	static int lCanAirBomb(lua_State* L);
	static int lCanAirBombAt(lua_State* L);

	static int lBombardTarget(lua_State* L);
	static int lCanBombard(lua_State* L);
	static int lCanPillage(lua_State* L);

	static int lIsSelected(lua_State* L);

	static int lCanFound(lua_State* L);
	static int lCanSpread(lua_State* L);
	static int lCanJoin(lua_State* L);
	static int lCanConstruct(lua_State* L);

	static int lCanDiscover(lua_State* L);
	static int lGetTradeGold(lua_State* L);
	static int lCanTrade(lua_State* L);
	static int lCanBuildSpaceship(lua_State* L);

	static int lCanGoldenAge(lua_State* L);
	static int lGetGoldenAgeTurns(lua_State* L);
	static int lCanBuild(lua_State* L);
	static int lCanLead(lua_State* L);
	static int lLead(lua_State* L);
	static int lCanGiveExperience(lua_State* L);
	static int lGiveExperience(lua_State* L);

	static int lCanPromote(lua_State* L);
	static int lPromote(lua_State* L);

	static int lGetUpgradeUnitType(lua_State* L);
	static int lUpgradePrice(lua_State* L);
	static int lCanUpgradeRightNow(lua_State* L);
	static int lGetNumResourceNeededToUpgrade(lua_State* L);

	static int lGetHandicapType(lua_State* L);
	static int lGetCivilizationType(lua_State* L);
	static int lGetSpecialUnitType(lua_State* L);
	static int lGetCaptureUnitType(lua_State* L);
	static int lGetUnitCombatType(lua_State* L);
	static int lGetUnitAIType(lua_State* L);
	static int lGetDomainType(lua_State* L);
	static int lGetInvisibleType(lua_State* L);
	static int lGetSeeInvisibleType(lua_State* L);

	static int lFlavorValue(lua_State* L);
	static int lIsBarbarian(lua_State* L);

	static int lIsHuman(lua_State* L);
	static int lVisibilityRange(lua_State* L);

	static int lMaxMoves(lua_State* L);
	static int lMovesLeft(lua_State* L);

	static int lCanMove(lua_State* L);
	static int lHasMoved(lua_State* L);
	static int lRange(lua_State* L);
	static int lNukeDamageLevel(lua_State* L);

	static int lCanBuildRoute(lua_State* L);
	static int lGetBuildType(lua_State* L);
	static int lWorkRate(lua_State* L);

	static int lIsNoBadGoodies(lua_State* L);
	static int lIsOnlyDefensive(lua_State* L);

	static int lIsNoCapture(lua_State* L);
	static int lIsRivalTerritory(lua_State* L);
	static int lIsFound(lua_State* L);
	static int lIsWork(lua_State* L);
	static int lIsGoldenAge(lua_State* L);
	static int lCanCoexistWithEnemyUnit(lua_State* L);

	static int lIsGreatPerson(lua_State* L);

	static int lIsFighting(lua_State* L);
	static int lIsAttacking(lua_State* L);
	static int lIsDefending(lua_State* L);
	static int lIsInCombat(lua_State* L);

	static int lGetMaxHitPoints(lua_State* L);
	static int lGetCurrHitPoints(lua_State* L);
	static int lIsHurt(lua_State* L);
	static int lIsDead(lua_State* L);
	static int lIsDelayedDeath(lua_State* L);
	static int lSetBaseCombatStrength(lua_State* L);
	static int lGetBaseCombatStrength(lua_State* L);

	static int lGetMaxAttackStrength(lua_State* L);
	static int lGetMaxDefenseStrength(lua_State* L);

	static int lIsCombatUnit(lua_State* L);
	static int lIsCanDefend(lua_State* L);
	static int lIsCanAttackWithMove(lua_State* L);
	static int lIsCanAttackRanged(lua_State* L);
	static int lIsCanAttack(lua_State* L);
	static int lIsCanAttackWithMoveNow(lua_State* L);
	static int lCanSiege(lua_State* L);
	static int lIsEnemyInMovementRange(lua_State* L);

	static int lGetBaseRangedCombatStrength(lua_State* L);
	static int lGetMaxRangedCombatStrength(lua_State* L);
	static int lGetCombatLimit(lua_State* L);
	static int lGetRangedCombatLimit(lua_State* L);
	static int lCanAirAttack(lua_State* L);
	static int lCanAirDefend(lua_State* L);
	static int lGetAirCombatDamage(lua_State* L);
	static int lGetRangeCombatDamage(lua_State* L);
	static int lGetAirStrikeDefenseDamage(lua_State* L);
	static int lGetBestInterceptor(lua_State* L);
	static int lGetInterceptorCount(lua_State* L);
	static int lGetBestSeaPillageInterceptor(lua_State* L);

	static int lIsAutomated(lua_State* L);
	static int lIsWaiting(lua_State* L);
	static int lIsFortifyable(lua_State* L);
	static int lIsEverFortifyable(lua_State* L);
	static int lFortifyModifier(lua_State* L);
	static int lExperienceNeeded(lua_State* L);
	static int lAttackXPValue(lua_State* L);
	static int lDefenseXPValue(lua_State* L);
	static int lMaxXPValue(lua_State* L);
	static int lFirstStrikes(lua_State* L);
	static int lChanceFirstStrikes(lua_State* L);
	static int lMaxFirstStrikes(lua_State* L);
	static int lIsRanged(lua_State* L);
	static int lIsMustSetUpToRangedAttack(lua_State* L);
	static int lCanSetUpForRangedAttack(lua_State* L);
	static int lIsSetUpForRangedAttack(lua_State* L);
	static int lIsRangeAttackOnlyInDomain(lua_State* L);

	static int lImmuneToFirstStrikes(lua_State* L);
	static int lNoDefensiveBonus(lua_State* L);
	static int lIgnoreBuildingDefense(lua_State* L);
	static int lCanMoveImpassable(lua_State* L);
	static int lCanMoveAllTerrain(lua_State* L);
	static int lFlatMovementCost(lua_State* L);
	static int lIgnoreTerrainCost(lua_State* L);
	static int lIsNeverInvisible(lua_State* L);
	static int lIsInvisible(lua_State* L);

	static int lIsNukeImmune(lua_State* L);

	static int lMaxInterceptionProbability(lua_State* L);
	static int lCurrInterceptionProbability(lua_State* L);
	static int lEvasionProbability(lua_State* L);
	static int lWithdrawalProbability(lua_State* L);

	static int lGetAdjacentModifier(lua_State* L);
	static int lGetAttackModifier(lua_State* L);
	static int lGetDefenseModifier(lua_State* L);
	static int lGetRangedAttackModifier(lua_State* L);
	static int lCityAttackModifier(lua_State* L);
	static int lCityDefenseModifier(lua_State* L);
	static int lHillsAttackModifier(lua_State* L);
	static int lHillsDefenseModifier(lua_State* L);
	static int lOpenAttackModifier(lua_State* L);
	static int lOpenRangedAttackModifier(lua_State* L);
	static int lRoughAttackModifier(lua_State* L);
	static int lRoughRangedAttackModifier(lua_State* L);
	static int lAttackFortifiedModifier(lua_State* L);
	static int lAttackWoundedModifier(lua_State* L);
	static int lOpenDefenseModifier(lua_State* L);
	static int lRoughDefenseModifier(lua_State* L);

	static int lTerrainAttackModifier(lua_State* L);
	static int lTerrainDefenseModifier(lua_State* L);
	static int lFeatureAttackModifier(lua_State* L);
	static int lFeatureDefenseModifier(lua_State* L);
	static int lUnitClassAttackModifier(lua_State* L);
	static int lUnitClassDefenseModifier(lua_State* L);
	static int lUnitCombatModifier(lua_State* L);
	static int lDomainModifier(lua_State* L);
	static int lGetStrategicResourceCombatPenalty(lua_State* L);
	static int lAirSweepCombatMod(lua_State* L);

	static int lSpecialCargo(lua_State* L);
	static int lDomainCargo(lua_State* L);
	static int lCargoSpace(lua_State* L);
	static int lChangeCargoSpace(lua_State* L);
	static int lIsFull(lua_State* L);
	static int lCargoSpaceAvailable(lua_State* L);
	static int lHasCargo(lua_State* L);
	static int lCanCargoAllMove(lua_State* L);
	static int lGetUnitAICargo(lua_State* L);
	static int lGetID(lua_State* L);
	static int lGetDropRange(lua_State* L);

	static int lGetHotKeyNumber(lua_State* L);
	static int lSetHotKeyNumber(lua_State* L);

	static int lGetX(lua_State* L);
	static int lGetY(lua_State* L);
	static int lSetXY(lua_State* L);
	static int lAt(lua_State* L);
	static int lAtPlot(lua_State* L);
	static int lGetPlot(lua_State* L);
	static int lGetArea(lua_State* L);
	static int lGetReconPlot(lua_State* L);
	static int lSetReconPlot(lua_State* L);

	static int lGetGameTurnCreated(lua_State* L);
	static int lGetLastMoveTurn(lua_State* L);

	static int lGetDamage(lua_State* L);
	static int lSetDamage(lua_State* L);
	static int lChangeDamage(lua_State* L);

	static int lGetMoves(lua_State* L);
	static int lSetMoves(lua_State* L);
	static int lChangeMoves(lua_State* L);
	static int lFinishMoves(lua_State* L);
	static int lIsImmobile(lua_State* L);

	static int lGetExperience(lua_State* L);
	static int lSetExperience(lua_State* L);
	static int lChangeExperience(lua_State* L);
	static int lGetLevel(lua_State* L);
	static int lSetLevel(lua_State* L);
	static int lChangeLevel(lua_State* L);
	static int lGetFacingDirection(lua_State* L);
	static int lRotateFacingDirectionClockwise(lua_State* L);
	static int lRotateFacingDirectionCounterClockwise(lua_State* L);
	static int lGetCargo(lua_State* L);
	static int lGetFortifyTurns(lua_State* L);
	static int lGetBlitzCount(lua_State* L);
	static int lIsBlitz(lua_State* L);
	static int lGetAmphibCount(lua_State* L);
	static int lIsAmphib(lua_State* L);
	static int lGetRiverCrossingNoPenaltyCount(lua_State* L);
	static int lIsRiverCrossingNoPenalty(lua_State* L);
	static int lIsEnemyRoute(lua_State* L);
	static int lIsAlwaysHeal(lua_State* L);
	static int lIsHealOutsideFriendly(lua_State* L);
	static int lIsHillsDoubleMove(lua_State* L);
	static int lIsGarrisoned(lua_State* L);
	static int lGetGarrisonedCity(lua_State* L);

	static int lGetExtraVisibilityRange(lua_State* L);
	static int lGetExtraMoves(lua_State* L);
	static int lGetExtraMoveDiscount(lua_State* L);
	static int lGetExtraRange(lua_State* L);
	static int lGetExtraIntercept(lua_State* L);
	static int lGetExtraEvasion(lua_State* L);
	static int lGetExtraFirstStrikes(lua_State* L);
	static int lGetExtraChanceFirstStrikes(lua_State* L);
	static int lGetExtraWithdrawal(lua_State* L);
	static int lGetExtraEnemyHeal(lua_State* L);
	static int lGetExtraNeutralHeal(lua_State* L);
	static int lGetExtraFriendlyHeal(lua_State* L);

	static int lGetSameTileHeal(lua_State* L);
	static int lGetAdjacentTileHeal(lua_State* L);

	static int lGetExtraCombatPercent(lua_State* L);
	static int lGetFriendlyLandsModifier(lua_State* L);
	static int lGetFriendlyLandsAttackModifier(lua_State* L);
	static int lGetOutsideFriendlyLandsModifier(lua_State* L);
	static int lGetExtraCityAttackPercent(lua_State* L);
	static int lGetExtraCityDefensePercent(lua_State* L);
	static int lGetExtraHillsAttackPercent(lua_State* L);
	static int lGetExtraHillsDefensePercent(lua_State* L);
	static int lGetExtraOpenAttackPercent(lua_State* L);
	static int lGetExtraOpenRangedAttackMod(lua_State* L);
	static int lGetExtraRoughAttackPercent(lua_State* L);
	static int lGetExtraRoughRangedAttackMod(lua_State* L);
	static int lGetExtraAttackFortifiedMod(lua_State* L);
	static int lGetExtraAttackWoundedMod(lua_State* L);
	static int lGetExtraOpenDefensePercent(lua_State* L);
	static int lGetPillageChange(lua_State* L);
	static int lGetUpgradeDiscount(lua_State* L);
	static int lGetExperiencePercent(lua_State* L);
	static int lGetKamikazePercent(lua_State* L);

	static int lIsOutOfAttacks(lua_State* L);
	static int lSetMadeAttack(lua_State* L);
	static int lisOutOfInterceptions(lua_State* L);
	static int lSetMadeInterception(lua_State* L);

	static int lIsPromotionReady(lua_State* L);
	static int lSetPromotionReady(lua_State* L);
	static int lGetOwner(lua_State* L);
	static int lGetVisualOwner(lua_State* L);
	static int lGetCombatOwner(lua_State* L);
	static int lGetOriginalOwner(lua_State* L);
	static int lSetOriginalOwner(lua_State* L);
	static int lGetTeam(lua_State* L);
	static int lGetUnitFlagIconOffset( lua_State *L );
	static int lGetUnitPortraitOffset( lua_State *L );

	static int lGetUnitType(lua_State* L);
	static int lGetUnitClassType(lua_State* L);
	static int lGetLeaderUnitType(lua_State* L);
	static int lSetLeaderUnitType(lua_State* L);
	static int lIsNearGreatGeneral(lua_State* L);
	static int lGetReverseGreatGeneralModifier(lua_State* L);
	static int lGetNearbyImprovementModifier(lua_State* L);
	static int lIsFriendlyUnitAdjacent(lua_State* L);
	static int lGetNumEnemyUnitsAdjacent(lua_State* L);
	static int lIsEnemyCityAdjacent(lua_State* L);

	static int lGetTransportUnit(lua_State* L);
	static int lIsCargo(lua_State* L);

	static int lGetExtraDomainModifier(lua_State* L);

	static int lGetName(lua_State* L);
	static int lGetNameNoDesc(lua_State* L);
	static int lHasName(lua_State* L);
	static int lGetNameKey(lua_State* L);
	static int lSetName(lua_State* L);
	static int lIsTerrainDoubleMove(lua_State* L);
	static int lIsFeatureDoubleMove(lua_State* L);

	static int lGetScriptData(lua_State* L);
	static int lSetScriptData(lua_State* L);
	static int lGetScenarioData(lua_State* L);
	static int lSetScenarioData(lua_State* L);

	static int lGetExtraTerrainAttackPercent(lua_State* L);
	static int lGetExtraTerrainDefensePercent(lua_State* L);
	static int lGetExtraFeatureAttackPercent(lua_State* L);
	static int lGetExtraFeatureDefensePercent(lua_State* L);
	static int lGetExtraUnitCombatModifier(lua_State* L);
	static int lGetUnitClassModifier(lua_State* L);

	static int lCanAcquirePromotion(lua_State* L);
	static int lCanAcquirePromotionAny(lua_State* L);
	static int lIsPromotionValid(lua_State* L);
	static int lIsHasPromotion(lua_State* L);
	static int lSetHasPromotion(lua_State* L);

	static int lGetActivityType (lua_State* L);
	static int lIsReadyToMove (lua_State* L);
	static int lIsBusy (lua_State* L);

	// Helper Functions
	static int lRangeStrike(lua_State* L);

	static int lPushMission(lua_State* L);
	static int lPopMission(lua_State* L);
	static int lLastMissionPlot(lua_State* L);
	static int lCanStartMission(lua_State* L);

	static int lExecuteSpecialExploreMove(lua_State* L);

	static int lSetDeployFromOperationTurn(lua_State* L);
};


#endif //CVLUAUNIT_H
